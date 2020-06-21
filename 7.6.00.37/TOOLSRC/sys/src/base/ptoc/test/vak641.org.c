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
  unsigned char *tsp00_Addr;
typedef
  tsp00_Uint1 tsp_int1;
typedef
  tsp00_Int2 tsp_int2;
typedef
  tsp00_Int4 tsp_int4;
typedef
  double tsp_longreal;
typedef
  double tsp00_8ByteCounter;
typedef
  unsigned char tsp00_DataType;
typedef
  tsp00_DataType tsp_data_type;
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
  unsigned char tsp00_C12[12];
typedef
  unsigned char tsp00_C16[16];
typedef
  unsigned char tsp00_C20[20];
typedef
  unsigned char tsp00_C40[40];
typedef
  unsigned char tsp00_C64[64];
typedef
  unsigned char tsp00_C256[256];
typedef
  unsigned char tsp00_Buf[8192];
typedef
  tsp00_Uint1 tsp00_CodeType;
typedef
  tsp00_C8 tsp00_Date;
typedef
  unsigned char tsp00_DbName[18];
typedef
  unsigned char tsp00_Identifier[18];
typedef
  tsp00_C64 tsp00_KnlIdentifier;
typedef
  unsigned char tsp00_Key[1024];
typedef
  unsigned char tsp00_MoveObj[8192000];
typedef
  unsigned char tsp00_Name[18];
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
  unsigned char tsp00_TermId[18];
typedef
  tsp00_C8 tsp00_Time;
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
  tsp00_C12 tsp_c12;
typedef
  tsp00_C16 tsp_c16;
typedef
  unsigned char tsp_c18[18];
typedef
  tsp00_C20 tsp_c20;
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
  unsigned char tsp_cryptpw[24];
typedef
  tsp00_CodeType tsp_code_type;
typedef
  unsigned char tsp_ctable[256];
typedef
  tsp00_DbName tsp_dbname;
typedef
  tsp00_KnlIdentifier tsp_knl_identifier;
typedef
  tsp00_Key tsp_key;
typedef
  tsp00_MoveObj tsp_moveobj;
typedef
  unsigned char tsp_nodeid[64];
typedef
  tsp00_Number tsp_number;
typedef
  tsp00_ResNum tsp_resnum;
typedef
  unsigned char tsp_sqlstate[5];
typedef
  tsp00_TermId tsp_termid;
typedef
  unsigned char tsp_timestamp[20];
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
        tsp_int2 map_int;
        } C_true;
      struct  
        {
        tsp_c2 map_c2;
        } C_false;
      } boolean;
    } tsp_int_map_c2;
typedef
  struct ptoc_Type1
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
  struct ptoc_Type2
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
  struct ptoc_Type3
    {
    tsp00_C16 coclsid;
    tsp00_C16 iid;
    tsp00_Addr ifactory;
    tsp00_Addr idispatch;
    tsp00_Addr ikernel_sink;
    tsp00_Addr session_context;
    tsp00_C4 session;
    tsp00_C4 trans;
    tsp00_Int4 ret;
    tsp00_C64 errtext;
    } tsp_dcom_co_create_instance_parms;
typedef
  unsigned char tsp1_warningset_intern[2];
typedef
  unsigned char tsp1_command_options[1];
typedef
  unsigned char tsp1_part_attr[1];
typedef
  struct ptoc_Type4
    {
    unsigned char sp1p_part_kind;
    tsp1_part_attr sp1p_attributes;
    tsp_int2 sp1p_arg_count;
    tsp_int4 sp1p_segm_offset;
    tsp_int4 sp1p_buf_len;
    tsp_int4 sp1p_buf_size;
    } tsp1_part_header;
typedef
  struct ptoc_Type5
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
  tsp1_part *tsp1_part_ptr;
typedef
  struct ptoc_Type6
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
  struct ptoc_Type7
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
  struct ptoc_Type8
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
  struct ptoc_Type9
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
  struct ptoc_Type10
    {
    tsp_data_type para_type;
    unsigned char para_frac;
    tsp_int2 para_length;
    } tsp1_param_spec_list[2000];
typedef
  tsp00_Addr tgg00_VoidPtr;
typedef
  struct ptoc_Type11
    {
    tsp00_C4 ci4_gg00;
    } tgg90_Cint4;
typedef
  struct ptoc_Type12
    {
    tsp00_C2 ci2_gg00;
    } tgg91_FileVersion;
typedef
  tgg90_Cint4 tgg91_SessionNo;
typedef
  struct ptoc_Type13
    {
    tsp00_C6 ci6_gg00;
    } tgg91_TransNo;
typedef
  int tgg00_SubtransNo;
typedef
  unsigned char tgg00_BdUseSet[1];
typedef
  unsigned char tgg00_Debug;
typedef
  unsigned char tgg00_DumpStateSet[4];
typedef
  unsigned char tgg00_FiletypeSet[1];
typedef
  unsigned char tgg00_HandlingSet[2];
typedef
  unsigned char tgg00_LockReqState[1];
typedef
  unsigned char tgg00_StackOpBuildIn;
typedef
  unsigned char tgg00_StackOpType;
typedef
  unsigned char tgg00_TransState[2];
typedef
  unsigned char tgg00_AuxBuffers[16][8192];
typedef
  tgg00_AuxBuffers *tgg00_AuxBuffersPtr;
typedef
  tsp00_Int2 tgg00_BasisError;
typedef
  tsp00_C16 tgg00_Filename;
typedef
  tsp00_VfType tgg00_Filetypes[32];
typedef
  tsp00_C64 tgg00_MediaName;
typedef
  int tgg00_ObjFileNo;
typedef
  unsigned char tgg00_RecBody[8184];
typedef
  int tgg00_RootList[6];
typedef
  tsp00_C2 tgg00_ServerdbNo;
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
  struct ptoc_Type13 tgg00_UserRef;
typedef
  struct ptoc_Type14
    {
    tsp00_C12 utidId_gg00;
    tsp00_Int4 utidLineNo_gg00;
    } tgg00_UtilCmdId;
typedef
  struct ptoc_Type15
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
  struct ptoc_Type16
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
  struct ptoc_Type17
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
  struct ptoc_Type18
    {
    tsp00_Int4 plTop_gg00;
    int plPno_gg00[10];
    } tgg00_PnoList;
typedef
  struct ptoc_Type19
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
  struct ptoc_Type20
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
    struct ptoc_Type21
      {
      tsp00_C1 td_internal;
      tsp00_C1 td_external;
      tsp00_C8 td_comment;
      tsp00_C2 td_filler;
      } term_desc[128];
    } tgg00_TermsetRecord;
typedef
  struct ptoc_Type22
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
  struct ptoc_Type23
    {
    tsp00_Int2 csi_cnt;
    tsp00_Int2 csi_fill1;
    tsp00_Int4 csi_fill2;
    struct ptoc_Type24
      {
      tgg00_Surrogate ocs_seq_surrogate;
      tsp00_Number ocs_seq_value;
      tsp00_Int4 ocs_seq_filler;
      } csi_values[32];
    } tgg00_CurrSeqInfo;
typedef
  tgg00_CurrSeqInfo *tgg00_CurrSeqInfoPtr;
typedef
  struct ptoc_Type25
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
  struct ptoc_Type26
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
  struct ptoc_Type27
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
  struct ptoc_Type28
    {
    tgg00_Surrogate lockTabId_gg00;
    tgg00_ServerdbNo lockSite_gg00;
    unsigned char lockMode_gg00;
    tgg00_LockReqState lockState_gg00;
    tsp00_Int2 lockKeyLen_gg00;
    tsp00_Int2 lockKeyPos_gg00;
    } tgg00_Lock;
typedef
  struct ptoc_Type29
    {
    tgg00_Lkey listkey;
    tgg00_Lkey reckey;
    } tgg00_TwoKeys;
typedef
  struct ptoc_Type30
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
        unsigned char sshowkind;
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
  struct ptoc_Type31
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
  struct ptoc_Type32
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
  struct ptoc_Type33
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
  int tgg00_ColumnDistinctValues[1024];
typedef
  struct ptoc_Type34
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
  struct ptoc_Type35
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
  struct ptoc_Type36
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
  struct ptoc_Type37
    {
    tsp00_Int4 idc_prim_leafnodes;
    tsp00_Int4 idc_sec_leafnodes;
    tsp00_Int4 idc_prim_keycount;
    tsp00_Int4 idc_sec_keycount;
    tsp00_Int4 idc_nullcount;
    } tgg00_IndexCounter;
typedef
  struct ptoc_Type38
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
  struct ptoc_Type39
    {
    union  
      {
      struct  
        {
        unsigned char etype;
        tgg00_StackOpType eop;
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
        tgg00_StackOpBuildIn eop_build_in;
        unsigned char eformat;
        tsp00_DataType edatatype;
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
  tgg00_StackEntry tgg00_StackList[32767];
typedef
  tgg00_StackList *tgg00_StackListPtr;
typedef
  struct ptoc_Type40
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
  struct ptoc_Type41
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
  struct ptoc_Type42
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
  struct ptoc_Type43
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
    unsigned char m_strat_info;
    unsigned char m_qual_kind;
    tgg00_DataPartPtr m_data;
    } tgg00_FetchDesc;
typedef
  struct ptoc_Type44
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
  struct ptoc_Type45
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
  struct ptoc_Type46
    {
    tsp00_Int4 mb_qual_len;
    tsp00_Int4 mb_data_len;
    tsp00_Int4 mb_strat_len;
    tsp00_Int4 mb_fill2;
    tgg00_ServerdbNo mb_src_site;
    char mb_reply;
    unsigned char mb_replicated;
    unsigned char mb_type;
    unsigned char mb_type2;
    unsigned char mb_struct;
    char mb_fill1;
    } tgg00_MessBufHeader;
typedef
  struct tgg00_MessBlock *tgg00_MessBlockPtr;
typedef
  struct ptoc_Type47
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
      unsigned char mb_type;
      unsigned char mb_type2;
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
  tgg00_BasisError tgg_basis_error;
typedef
  struct ptoc_Type48
    {
    tsp00_CodeType ctype;
    tsp00_CodeType char_default;
    unsigned char date_time;
    unsigned char kernel_swap;
    } tgg_code_globals;
typedef
  struct ptoc_Type49
    {
    tsp00_C256 tables[8];
    tsp00_C256 termset[8];
    tsp00_C256 inv_termset[8];
    } tgg_code_tables;
typedef
  tgg00_DataPartPtr tgg_datapart_ptr;
typedef
  struct ptoc_Type50
    {
    tgg00_Surrogate dbf_codeid;
    tsp00_Addr dbf_acv_addr;
    tgg00_ServerdbNo dbf_site;
    char dbf_new_code;
    tsp_dcom_co_create_instance_parms dbf_com_params;
    tsp00_Int4 dbf_dispid;
    char dbf_created;
    } tgg_dbfunc_info;
typedef
  unsigned char tgg_linkset[1];
typedef
  struct tgg00_MessBlock tgg_mess_block;
typedef
  unsigned char tgg_privilege_set[1];
typedef
  tgg00_QualBufPtr tgg_qual_buf_ptr;
typedef
  struct ptoc_Type51
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
  tgg00_StackOpBuildIn tgg_stack_op_build_in;
typedef
  tgg00_StackOpType tgg_stack_op_type;
typedef
  struct ptoc_Type52
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
  struct ptoc_Type53
    {
    tgg00_FileId file_id;
    tgg00_FileId inv_id;
    } tgg_two_tree_ids;
typedef
  unsigned char tak00_ooption[1];
typedef
  struct ptoc_Type54
    {
    struct ptoc_Type55
      {
      tsp_knl_identifier ofname;
      tsp_int2 ofno;
      tsp_int2 ofstno;
      tak00_ooption ofasc;
      tsp00_Uint1 oftabno;
      tsp_int2 offno;
      tsp_int4 ofapos;
      tsp_int2 ofnode;
      tsp_int2 offill;
      } ofield[128];
    tsp_int2 ocntord;
    tsp_int2 ofill1;
    tsp_int4 ofill2;
    } tak00_ord_fields;
typedef
  tak00_ord_fields *tak00_ord_fields_ptr;
typedef
  unsigned char tak00_colpropset[2];
typedef
  struct ptoc_Type56
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
        tsp_data_type cdatatyp;
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
        tsp_data_type cfiller6;
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
        tsp_data_type cfiller_6;
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
  struct ptoc_Type57
    {
    tsp_int2 sci_len;
    tsp_int2 sci_iolen;
    tsp_int2 sci_frac;
    tsp_data_type sci_typ;
    unsigned char sci_dttyp;
    tak00_colpropset sci_cprops;
    tsp_int2 sci_com_type;
    tsp_c2 sci_udt_id;
    } tak00_scolinf;
typedef
  unsigned char tak_sc_symbol;
typedef
  unsigned char tak_convert_type;
typedef
  unsigned char tak_table_attribute_set[1];
typedef
  struct ptoc_Type58
    {
    tsp_c3 p_count;
    tsp_c1 p_id;
    unsigned char p_kind;
    tsp00_Uint1 p_no;
    tsp_int2 p_filler;
    } tak_parskey;
typedef
  unsigned char tak_special_node_info[1];
typedef
  struct ptoc_Type59
    {
    unsigned char n_proc;
    tak_special_node_info n_special;
    tak_sc_symbol n_symb;
    tsp_data_type n_datatype;
    tsp_int2 n_subproc;
    tsp_int2 n_length;
    tsp_int4 n_pos;
    tsp_int2 n_lo_level;
    tsp_int2 n_sa_level;
    } tak_ap_max_tree[32768];
typedef
  tak_ap_max_tree *tak_ap_max_tree_ptr;
typedef
  struct ptoc_Type60
    {
    tak_sc_symbol sc_symb;
    tak_sc_symbol sc_eof_symb;
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
  short tak_inv_colnos[32];
typedef
  tsp00_Uint1 tak_corlevel[4];
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
  struct ptoc_Type61
    {
    tsp_int4 rctxt_cnt;
    tsp00_C8 rctxt_roles[32768];
    } tak_role_context;
typedef
  tak_role_context *tak_role_context_ptr;
typedef
  unsigned char tak_map_set[768];
typedef
  struct ptoc_Type62
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
  struct ptoc_Type63
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
  struct ptoc_Type64
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
  struct ptoc_Type65
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
  struct ptoc_Type66
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
  struct ptoc_Type67
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
  struct ptoc_Type68
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
  struct ptoc_Type69
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
  struct ptoc_Type70
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
  struct ptoc_Type71
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
  struct ptoc_Type72
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
  struct ptoc_Type73
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
  struct ptoc_Type74
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
  struct ptoc_Type75
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
  struct ptoc_Type76
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
  struct ptoc_Type77
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
    struct ptoc_Type78
      {
      tgg_surrogate ofp_procid;
      tsp_c3 ofp_language;
      unsigned char ofp_sqlmode[1];
      } fct_procids[64];
    } tak_funcrefrecord;
typedef
  struct ptoc_Type79
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
  struct ptoc_Type80
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
    struct ptoc_Type81
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
  struct ptoc_Type82
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
    struct ptoc_Type83
      {
      tgg_surrogate ltableid;
      tak_oldidentifier linkn_prefix;
      tsp_int2 lfiller1;
      tsp_int4 ldatecre;
      tsp_int4 ltimecre;
      tsp_data_type ldatatyp;
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
  struct ptoc_Type84
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
  struct ptoc_Type85
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
    struct ptoc_Type86
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
  struct ptoc_Type87
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
  struct ptoc_Type88
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
  struct ptoc_Type89
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
    struct ptoc_Type90
      {
      tgg_surrogate usa_tableid;
      tsp_int2 usa_filler;
      unsigned char usa_tablekind;
      char usa_empty;
      } usagedef[672];
    } tak_usagerecord;
typedef
  struct ptoc_Type91
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
    struct ptoc_Type92
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
  struct ptoc_Type93
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
  struct ptoc_Type94
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
  struct ptoc_Type95
    {
    tak_sysbufferoffset ru_key_offset;
    tak_uref_restkey ru_rest_key;
    tsp_c2 ru_segmentid;
    tsp_int2 ru_filler;
    tgg_surrogate ru_user_id;
    } tak_userrefrecord;
typedef
  struct ptoc_Type96
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
  struct ptoc_Type97
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
  struct ptoc_Type98
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
  struct ptoc_Type99
    {
    tak_param_spec_ptr param_spec_ptr;
    tsp_int2 param_counter;
    tsp_int2 param_filler1;
    tsp_int4 param_filler2;
    } tak_param_list;
typedef
  struct ptoc_Type100
    {
    tsp_data_type param_datatype;
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
  struct ptoc_Type101
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
  struct ptoc_Type102
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
  struct ptoc_Type103
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
  struct ptoc_Type104
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
  struct ptoc_Type105
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
  struct ptoc_Type106
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
  struct ptoc_Type107
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
  struct ptoc_Type108
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
  struct ptoc_Type109
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
  struct ptoc_Type110
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
  struct ptoc_Type111
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
  struct ptoc_Type112
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
  unsigned char tak_joinset[8];
typedef
  struct ptoc_Type113
    {
    struct ptoc_Type114
      {
      tsp_int4 jop_multiplier;
      tsp_int2 jop_fieldno;
      tsp_int2 jop_startstack;
      tsp_int2 jop_cntstack;
      tsp00_Uint1 jop_tableno;
      tsp_data_type jop_datatyp;
      tsp_int2 jop_datalen;
      tsp_int2 jop_inoutlen;
      tak_jcolpropset jop_propset;
      char jop_outer_join;
      tsp_int2 jop_outpos;
      } jo_recs[2];
    tgg_stack_op_type jo_op;
    char jo_no_join;
    tsp00_Uint1 jo_partno;
    tsp00_Uint1 jo_col_upd_stat;
    } tak_joinarr[257];
typedef
  struct ptoc_Type115
    {
    tak_joinarr jrc_joinarr;
    tsp_int2 jrc_cnt;
    char jrc_col_upd;
    tsp00_Uint1 jrc_filler;
    } tak_joinrec;
typedef
  unsigned char tak_viewtext[8064];
typedef
  struct ptoc_Type116
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
        struct ptoc_Type117
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
  struct ptoc_Type118
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
    struct ptoc_Type119
      {
      tsp_int2 vfromtabno;
      tsp_int2 vn_pos;
      tsp_int2 vfromextcolno;
      tsp_int2 vextcolno;
      tsp_data_type vdatatype;
      char vfiller;
      tsp_int2 vfiller1;
      } vdescription[672];
    } tak_viewdescrecord;
typedef
  struct ptoc_Type120
    {
    tgg_surrogate vttableid;
    tsp_int2 vtfiller;
    char vtins_for_upd;
    char vtone_to_one;
    tak_charset vtnot_used_links;
    } tak_viewtabarr[183];
typedef
  struct ptoc_Type121
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
    tgg_stack_op_type j12operator;
    unsigned char j12fill1;
    tsp_int4 j12fill2;
    } tak_viewjoins[336];
typedef
  tgg00_StackEntry tak_view_stack_entries[1007];
typedef
  unsigned char tak_viewbuf[8056];
typedef
  struct ptoc_Type122
    {
    tsp_int2 drecno;
    tsp_int2 dextno;
    tsp_int2 dfiller;
    tsp00_Uint1 dexttabno;
    tsp00_Uint1 dtabno;
    } tak_derived_col_info[1009];
typedef
  struct ptoc_Type123
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
  struct ptoc_Type124
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
  struct ptoc_Type125
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
  struct ptoc_Type126
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
  struct ptoc_Type127
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
  struct ptoc_Type128
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
    struct ptoc_Type129
      {
      tsp_int4 cst_dist_values;
      tsp_int4 cst_pages;
      tsp_int4 cst_avg_lst_len;
      } sti_col_stats[660];
    } tak_statisticsinfo;
typedef
  struct ptoc_Type130
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
  struct ptoc_Type131
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
    struct ptoc_Type132
      {
      tsp_knl_identifier str_name;
      tgg00_SubtransNo str_no;
      } str_subtrans[50];
    } tak_subtransrecord;
typedef
  struct ptoc_Type133
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
  struct ptoc_Type134
    {
    tsp_int2 ch_startpos;
    unsigned char ch_length;
    unsigned char ch_type[1];
    } tak_column_change_arr[1024];
typedef
  struct ptoc_Type135
    {
    tsp_int2 cr_colcount;
    char cr_was_opt;
    char cr_filler;
    tak_column_change_arr cr_columns;
    } tak_changerecord;
typedef
  struct ptoc_Type136
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
  struct ptoc_Type137
    {
    tsp_int2 ks_pos;
    tsp_int2 ks_len;
    } tak_keyspec;
typedef
  struct ptoc_Type138
    {
    tak_keyspec reckeyspec;
    tak_keyspec listkeyspec;
    } tak_two_keyspecs;
typedef
  unsigned char tak_res_keysbuf[13316];
typedef
  struct ptoc_Type139
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
  struct ptoc_Type140
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
  struct ptoc_Type141
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
  struct ptoc_Type142
    {
    tsp1_param_opt_set sp1i_mode;
    unsigned char sp1i_io_type;
    tsp_data_type sp1i_data_type;
    tsp_int1 sp1i_frac;
    tsp_int2 sp1i_length;
    tsp_int2 sp1i_in_out_len;
    tsp_int4 sp1i_bufpos;
    } tak_paraminfoarr[2700];
typedef
  struct ptoc_Type143
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
  struct ptoc_Type144
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
  struct ptoc_Type145
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
  struct ptoc_Type146
    {
    tsp_int2 all_lock_count;
    tsp_int2 all_lock_fill;
    tgg00_Lock all_locks[240];
    } tak_ak_all_locks;
typedef
  struct ptoc_Type147
    {
    unsigned char fp_kind;
    unsigned char fp_etype;
    tsp_data_type fp_datatyp;
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
        tak_sc_symbol fp_sc_symbol;
        tsp00_C1 fp_objecttype;
        tak_len_and_frac fp_len_and_frac_v15;
        tsp_int2 fp_inoutlen_v15;
        } C_fp_get_name_value;
      } tak_fp_kind_type;
    } tak_parser_fields[2025];
typedef
  struct ptoc_Type148
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
        unsigned char p_mtyp;
        unsigned char p_mtyp2;
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
  struct ptoc_Type149
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
  struct ptoc_Type150
    {
    tak_parskey uparsid;
    tsp_int2 ufirstsel;
    tsp_int2 usecondsel;
    tsp_int2 ufiller;
    unsigned char uis_kind;
    char uis_all;
    } tak_union_array[493];
typedef
  struct ptoc_Type151
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
  struct ptoc_Type152
    {
    tak_parskey cparsk;
    tsp_int2 cnextpars;
    tak_complex_infoset centryinfo;
    unsigned char cqual_jv_upd;
    } tak_complex_array[669];
typedef
  struct ptoc_Type153
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
  unsigned char tak_one_table_specialname[1];
typedef
  struct ptoc_Type154
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
  struct ptoc_Type155
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
  struct ptoc_Type156
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
    struct ptoc_Type157
      {
      tsp00_Uint1 socoldtabno;
      tsp00_Uint1 socselecttabno;
      tsp00_Uint1 socfields;
      tsp00_Uint1 socfill;
      } tc_tables[16];
    tak_all_corcolumns tc_columns;
    } tak_correcord;
typedef
  struct ptoc_Type158
    {
    tsp_int2 lpos;
    tsp_int2 llen;
    } tak_linkposinfo[2048];
typedef
  struct ptoc_Type159
    {
    tsp_int2 lreclen;
    tsp_int2 lvarcol_offset;
    tsp_int2 lvarcol_cnt;
    tsp_int2 lkeylen;
    tgg_surrogate lsurrogate;
    tsp_c2 lentrytype;
    tsp_int2 lseqno;
    tsp_int4 lfullen;
    unsigned char lmess_type;
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
  struct ptoc_Type160
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
  struct ptoc_Type161
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
  struct ptoc_Type162
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
  struct ptoc_Type163
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
  struct ptoc_Type164
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
  struct ptoc_Type165
    {
    tsp_knl_identifier dbp_sysdba;
    tgg00_FileId dbp_tree;
    } tak_dbproc_array[5];
typedef
  struct ptoc_Type166
    {
    tsp_int2 dbc_cnt;
    tsp_int2 dbc_fil;
    tak_sysbufferaddress dbc_ptr;
    tak_dbproc_array dbc_cache;
    } tak_dbproc_cache;
typedef
  struct ptoc_Type167
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
  struct ptoc_Type168
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
  struct ptoc_Type169
    {
    tsp_int4 cst_top;
    tsp_moveobj_ptr cst_stack[16];
    } tak_cmd_stack;
typedef
  struct ptoc_Type170
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
  struct ptoc_Type171
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
  struct ptoc_Type172
    {
    tsp00_Int4 ai_uktId;
    tsp00_Int4 ai_badAllocCountDown;
    tak_AllocatorMonitorInfo ai_alloc_sys;
    tak_AllocatorMonitorInfo ai_alloc_usr;
    } tak_allocator_info;
typedef
  struct ptoc_Type173
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
  struct ptoc_Type174
    {
    tsp_knl_identifier ouser;
    tsp_knl_identifier otable;
    tsp_knl_identifier oreference;
    tgg_surrogate ofromtableid;
    tgg00_FileId otreeid;
    tsp_int4 opages;
    char oall_priv;
    tak_one_table_specialname ospecialname;
    char oview;
    char oviewqual;
    char oviewcheck;
    char ocomplex_view;
    tsp_int2 otab_node;
    unsigned char ospecs_needed;
    tsp00_Uint1 ocounttabs;
    tak_table_attribute_set oattributes;
    tsp_int1 ounused0;
    tsp_int4 ounused1;
    tak_columnset oprivset;
    tak_columnset osetallpriv;
    } tak_all_from_tables[64];
typedef
  struct ptoc_Type175
    {
    tak00_colinfo_ptr d_colptr;
    tak00_colinfo_ptr d_colbuf;
    tak_syspointerarr d_esparr;
    tsp_bufaddr d_resbuf_addr;
    unsigned char d_alpha_fill1[68];
    tsp_knl_identifier d_user;
    tsp_knl_identifier d_table;
    tsp_knl_identifier d_column;
    tsp_knl_identifier d_refname;
    tak_columnset d_upd_set;
    tak_columnset d_foundset;
    tsp_int2 d_param_st_index;
    tsp_int2 d_param_st_begin;
    tsp_int2 d_first_tab;
    tsp_c2 d_udt_datatype;
    tsp_int2 d_pargeslen;
    tsp_int2 d_movebefore;
    tsp_int2 d_index;
    tsp_int2 d_inoutpos;
    tsp_int4 d_vppos;
    tsp_int2 d_exprno;
    tsp_int2 d_cntfromtab;
    tsp_int2 d_acttabindex;
    tsp_int2 d_fieldno;
    tsp_int2 d_keylen;
    tsp_int2 d_outcolno;
    tsp_int4 d_maxlen;
    tsp_int2 d_pos_in_parsbuf;
    tsp_int2 d_parnum;
    char d_checkview;
    char d_join_dml;
    tsp_int2 d_colindex;
    tsp_int2 d_reclen;
    char d_standard;
    char d_wherepart;
    char d_range;
    char d_nullkey;
    char d_like;
    char d_key;
    char d_single;
    char d_use_order;
    char d_arith_where;
    char d_join;
    char d_reuse;
    char d_group;
    char d_having;
    tsp_data_type d_datatype;
    unsigned char d_allowed;
    unsigned char d_pars_kind;
    tsp_int2 d_maxcounttabs;
    tsp_int2 d_basetabno;
    unsigned char d_corr;
    tsp00_Uint1 d_lowpars;
    tak_corlevel d_level;
    unsigned char d_index_strat_poss;
    tsp00_Uint1 d_oldlowpars;
    char d_prep_corr_cols;
    char d_like_expression;
    tsp_int4 d_n_pos;
    tsp_int2 d_filled_bytes;
    tsp_c2 d_escape_char;
    tsp_int2 d_expand;
    char d_concat;
    char d_hex;
    char d_first_union;
    char d_change_date_time;
    char d_one_join_phase;
    char d_check_null_byte;
    char d_const_value_expr;
    char d_is_parameter_only;
    unsigned char d_strat_info;
    char d_truncate;
    tsp_int2 d_topos;
    char d_in_method_param;
    char d_escape;
    tsp_int4 d_mselect_rescnt;
    char d_execute_unlock;
    char d_pseudo_ins_select;
    tsp_c2 d_viewdesc_linkage;
    tak_inv_colnos d_inv_colno;
    tsp_knl_identifier d_resname;
    tak_joinset d_oj_tables;
    char d_all_count;
    char d_repl_reskey;
    tsp_int2 d_mselect_loop_cnt;
    tsp00_Uint1 d_inv_cnt;
    char d_align_fill;
    char d_single_expr;
    char d_only_table_scan;
    tak_syspointerarr d_sparr;
    tak_sysbufferaddress d_linkbuf;
    tak_sysbufferaddress d_viewtextbuf;
    tak_sysbufferaddress d_literalrec;
    unsigned char d_alpha_fill2[68];
    tgg_surrogate d_tableid;
    tsp_int2 d_act_node;
    tsp_int2 d_cntpar;
    tgg00_HandlingSet d_globstate;
    tgg00_HandlingSet d_unchanged_globstate;
    tsp_int2 d_pos_result;
    tsp_int2 d_rowno;
    char d_resname_found;
    char d_use_sub;
    unsigned char d_distinct;
    char d_view;
    tsp_knl_identifier d_viewusername;
    tsp_knl_identifier d_viewtablen;
    char d_subquery;
    tsp00_Uint1 d_sublevel;
    tsp_int2 d_subcount;
    tsp_int2 d_phase_cnt;
    char d_only_sem_check;
    char d_union;
    tsp_int2 d_union_order_n;
    char d_union_insert;
    char d_outer_join;
    char d_repl_nokey_ins_sel;
    char d_is_function_column;
    tsp_int2 d_global_pos_result;
    char d_longdesc_found;
    char d_unused1;
    tsp_int2 d_dist_optim;
    unsigned char d_qual_kind;
    char d_view_col_list;
    tsp_int2 d_view_col_node;
    tak_all_from_tables d_tabarr;
    tak_joinrec d_joins;
    tak_changerecord d_change;
    tsp_int4 d_change_align;
    tak00_ord_fields d_order_cols;
    tak00_ord_fields d_group_cols;
    tak00_ord_fields_ptr d_order_or_group_cols;
    unsigned char d_alpha_fill3[4];
    tsp_data_type d_ch_datatype;
    char d_like_optimize;
    tsp00_Uint1 d_like_part;
    char d_string_column;
    char d_unused11;
    char d_with_lock_clause;
    tsp_int2 d_first_longdesc_pos;
    tsp_int2 d_cnt_longdesc;
    tsp_int2 d_subquery_node;
    tsp_int4 d_last_filler;
    tgg_surrogate d_ins_sel_tabid;
    } tak_dml_info;
typedef
  struct ptoc_Type176
    {
    int fr_minus_n;
    int fr_jpos;
    char fr_error;
    char fr_describ;
    char fr_minus_symb;
    char fr_getnumb;
    } tak_factorrec;
typedef
  struct ptoc_Type177
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
  struct ptoc_Type178
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
  unsigned char tsp8_uni_error;
typedef
  unsigned char tsp8_uni_opt_set[1];
typedef
  short ak641stackentries[257];
tgg_sysinfokey a01defaultkey;
int a01char_size;
tsp_knl_identifier a01_il_b_identifier;
tak00_columninfo a642standard_colinfo;
tgg_code_globals g01code;
char g01unicode;
tgg_code_tables g02codetables;
int a01aligned_cmd_len();
int a01_put_node();
int a01_next_symbol();
int a02_put_identifier();
int a05_unsigned_int2_get();
int a05identifier_get();
int a05string_literal_get();
int a06get_username();
char a06_table_exist();
int a06_get_priv();
int a061assign_colname();
int a07_hex_uni_error();
int a07_b_put_error();
int a07_nb_put_error();
short a07_return_code();
int a10get_sysinfo();
char a12dbfunc_exist();
int a12describe_param();
int a12EvalDataLength();
char a12exist_type();
int a12InitParamInfoRec();
char a12method_exist();
char a12dbproc_exist();
int a12output_parameter();
int a12StoreParamInfo();
int a12get_domain();
int a16inc_vdesc_cnt();
char a23exist_sequence();
int a61_search_table();
int a61_rel_old_table();
int a640factor();
int a640not_first_factor();
int a642other_than_column();
int a65_look_for_datatypes();
int a65ch_format();
int a65_set_operator();
char a65_datatypes_ok();
int a651code_for_const_param_expr();
int a651value_calculate();
int g02tebcdic_to_ascii();
int g02tascii_to_ebcdic();
int g10fil();
int g10mv1();
int g10mv2();
int g10mv4();
int g10mv5();
int g10mv6();
int g10mv7();
int g20get_uni_key();
int g20get_uni_set();
short k79n_dest_len_ora_number_format();
short s78t_dest_len_date_format();
int s30map();
int s30lnr_defbyte();
int s40gsint();
int s41pbyte();
int s41p1byte();
int s80uni_trans();
int t01int4();
int t01lidentifier();
int t01name();
int t01qual();
int t01moveobj();
int t01messblock();
int cccprint();
int ak641addsubdate();
int ak641addsubtime();
int ak641ascii();
int ak641_add_months();
int ak641new_time();
int a641add_mapchar();
int a641get_mapcharset();
int ak641mapchar();
int ak641char();
int a641charset_get();
int ak641mbcs();
int ak641check_datatype();
int ak641chr();
int ak641chr_ora();
int ak641concat();
int ak641conversion();
int ak641datediff();
int ak641datetime();
int ak641days();
int ak641digits();
int ak641expand();
int ak641space();
int ak641fixed_float();
int ak641float();
int a641_get_length();
int ak641_hextoraw();
int ak641hex();
int ak641index();
int ak641last_day();
int ak641length_vsize();
int ak641to_char();
int ak641toidentifier();
int a641l_push_language();
int ak641s_push_string();
int ak641push_blank();
int a641f_push_format();
int a641d_push_date();
int a641translate_string_literal();
int ak641time_date_to_char_datatype();
int a641s_literal_value();
int a641u_literal_value();
int ak641lrpad();
int ak641to_date();
int ak641replace_translate();
int ak641trim();
int ak641makedate();
int ak641maketime();
int ak641months_between();
int ak641noround();
int ak641num();
int ak641name();
int ak641of();
int ak641put_maxlength();
int ak641put_roundlength();
int ak641soundex();
int ak641substr();
int ak641leftright();
int ak641timedate();
int ak641timediff();
int ak641not_first_type_or_timestamp_parameter();
int ak641type_or_timestamp_parameter();
int ak641timestamp();
int ak641truncround();
int ak641mod_power();
int ak641integer();
int ak641newcolin();
char ak641is_trans_ascii_to_uni();
char ak641is_trans_uni_to_ascii();
char ak641unicode_conv_destroyed();
int ak641destroy_dummy_entries();
int ak641decode_func();
int ak641special_colin();
int ak641special_func();
int a641check_datetime();
int a641_get_name();
int ak641get_name();
int a641col_function();
int a641stack_for_op_b_chr();
int a641string_set_operator();
int a641dbyte_string_set_operator();
int ak641user_defined_func();
int ak641describe_output_param();
int ak641describe_param();
int ak641emit_param();



int ak641addsubdate(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    int curr_n;
    tak00_scolinf colin1;
    tak_all_command_glob *ptoc_Var0;
    struct ptoc_Type59 *ptoc_Var1;
    tak_dml_info *ptoc_Var2;
    tak00_scolinf *ptoc_Var3;
    tak00_scolinf *ptoc_Var4;
    cccprint("AK641 ak641addsu");
    ptoc_Var0 = &*acv;
    ptoc_Var1 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var0->a_ap_tree , "vak641.p" , 14065))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var2 = &*dmli;
    curr_n = (int ) ptoc_Var1->n_lo_level;
    ak641type_or_timestamp_parameter(&(*acv) , &(*dmli) , &(*colin) , &curr_n , 10);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var0->a_return_segm , "vak641.p" , 14069)).integer.C_4.sp1r_returncode == 0)
        {
        curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var0->a_ap_tree , "vak641.p" , 14072))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
        colin1.sci_len = 0;
        ptoc_Var2->d_datatype = 15;
        a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var0->a_return_segm , "vak641.p" , 14076)).integer.C_4.sp1r_returncode == 0)
            {
            if ((int ) (*colin).sci_typ == 13)
                {
                ptoc_Var2->d_datatype = 13;
                ptoc_Var3 = &*colin;
                ptoc_Var3->sci_len = 20;
                ptoc_Var3->sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var3->sci_len , -32768 , 32767 , "vak641.p" , 14086) , -32768 , 32767);
                ptoc_Var3->sci_frac = 0;
                ptoc_Var3->sci_typ = 13;
                }
            else
                {
                ptoc_Var2->d_datatype = 10;
                ptoc_Var4 = &*colin;
                ptoc_Var4->sci_len = 8;
                ptoc_Var4->sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var4->sci_len , -32768 , 32767 , "vak641.p" , 14098) , -32768 , 32767);
                ptoc_Var4->sci_frac = 0;
                ptoc_Var4->sci_typ = 10;
                }
            if ((int ) ptoc_Var1->n_symb == 97)
                {
                a65_set_operator(&(*acv) , 47);
                }
            else
                {
                a65_set_operator(&(*acv) , 60);
                }
            }
        }
    cccprint("<ak641addsubdate");
    }



int ak641addsubtime(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    int curr_n;
    tak00_scolinf colin1;
    char is_timestamp;
    tak_all_command_glob *ptoc_Var5;
    struct ptoc_Type59 *ptoc_Var6;
    tak_dml_info *ptoc_Var7;
    tak00_scolinf *ptoc_Var8;
    tak00_scolinf *ptoc_Var9;
    cccprint("AK641 ak641addsu");
    ptoc_Var5 = &*acv;
    ptoc_Var6 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var5->a_ap_tree , "vak641.p" , 14134))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var7 = &*dmli;
    curr_n = (int ) ptoc_Var6->n_lo_level;
    ak641type_or_timestamp_parameter(&(*acv) , &(*dmli) , &(*colin) , &curr_n , 11);
    is_timestamp = (char ) sql__setrang((int ) (*colin).sci_typ == 13 , 0 , 1);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var5->a_return_segm , "vak641.p" , 14139)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var7->d_datatype = 11;
        curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var5->a_ap_tree , "vak641.p" , 14143))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
        colin1.sci_len = 0;
        a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
        if ((int ) colin1.sci_typ == 13)
            {
            *wrong_datatype = 1;
            }
        else
            {
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var5->a_return_segm , "vak641.p" , 14150)).integer.C_4.sp1r_returncode == 0)
                {
                if ((int ) colin1.sci_typ != 11)
                    {
                    a641check_datetime(&(*acv) , &(*dmli) , 11);
                    }
                if ((char ) sql__setrang(is_timestamp , 0 , 1))
                    {
                    ptoc_Var7->d_datatype = 13;
                    ptoc_Var8 = &*colin;
                    ptoc_Var8->sci_len = 20;
                    ptoc_Var8->sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var8->sci_len , -32768 , 32767 , "vak641.p" , 14164) , -32768 , 32767);
                    ptoc_Var8->sci_frac = 0;
                    ptoc_Var8->sci_typ = 13;
                    }
                else
                    {
                    ptoc_Var7->d_datatype = 11;
                    ptoc_Var9 = &*colin;
                    ptoc_Var9->sci_len = 8;
                    ptoc_Var9->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var9->sci_len + 1) , -32768 , 32767);
                    ptoc_Var9->sci_frac = 0;
                    ptoc_Var9->sci_typ = 11;
                    }
                if ((int ) ptoc_Var6->n_symb == 103)
                    {
                    a65_set_operator(&(*acv) , 49);
                    }
                else
                    {
                    a65_set_operator(&(*acv) , 62);
                    }
                }
            }
        }
    cccprint("<ak641addsubtime");
    }

static
  tak00_colpropset ptoc_Var10 = 
  {
  (unsigned char)'\002', (unsigned char)'\0'
  };


int ak641ascii(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    int curr_n;
    tak_all_command_glob *ptoc_Var11;
    struct ptoc_Type59 *ptoc_Var12;
    tak_dml_info *ptoc_Var13;
    tak00_scolinf *ptoc_Var14;
    struct tgg00_MessBlock *ptoc_Var15;
    tgg00_QualBuf *ptoc_Var16;
    cccprint("AK641 ak641ascii");
    ptoc_Var11 = &*acv;
    ptoc_Var12 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var11->a_ap_tree , "vak641.p" , 14213))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var13 = &*dmli;
    ptoc_Var14 = &*colin;
    ptoc_Var13->d_datatype = 14;
    curr_n = (int ) ptoc_Var12->n_lo_level;
    ptoc_Var14->sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var11->a_return_segm , "vak641.p" , 14219)).integer.C_4.sp1r_returncode == 21000)
        {
        (*(tsp1_segment *) sql__nil(ptoc_Var11->a_return_segm , "vak641.p" , 14222)).integer.C_4.sp1r_returncode = 0;
        ptoc_Var13->d_datatype = 16;
        a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var11->a_return_segm , "vak641.p" , 14227)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) ptoc_Var14->sci_typ == 18)
            {
            *wrong_datatype = 1;
            }
        else
            {
            a641string_set_operator(&(*acv) , 27 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
            ptoc_Var15 = &ptoc_Var11->a_mblock;
            ptoc_Var16 = (tgg00_QualBuf *) sql__nil(ptoc_Var15->integer.C_4.mb_qual , "vak641.p" , 14236);
            (*(tgg00_StackList *) sql__nil(ptoc_Var15->integer.C_5.mb_st , "vak641.p" , 14237))[sql__setrang((int ) ptoc_Var16->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_4.edatatype = (unsigned char ) sql__setrang(ptoc_Var14->sci_typ , 0 , 39);
            ptoc_Var14->sci_len = 3;
            ptoc_Var14->sci_frac = 0;
            ptoc_Var14->sci_iolen = 4;
            ptoc_Var14->sci_typ = 0;
            _ptoc_MOVE(unsigned char , 2, ptoc_Var14->sci_cprops, ptoc_Var10);
            ptoc_Var13->d_datatype = 15;
            }
        }
    cccprint("<ak641ascii     ");
    }



int ak641_add_months(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    char negativ;
    int curr_n;
    tak00_scolinf colin1;
    tak_all_command_glob *ptoc_Var17;
    struct ptoc_Type59 *ptoc_Var18;
    tak_dml_info *ptoc_Var19;
    tak00_scolinf *ptoc_Var20;
    struct tgg00_MessBlock *ptoc_Var21;
    tgg00_QualBuf *ptoc_Var22;
    cccprint("AK641 ak641_add_");
    ptoc_Var17 = &*acv;
    ptoc_Var18 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var17->a_ap_tree , "vak641.p" , 14270))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var19 = &*dmli;
    ptoc_Var19->d_datatype = 13;
    curr_n = (int ) ptoc_Var18->n_lo_level;
    (*colin).sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var17->a_return_segm , "vak641.p" , 14276)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) (*colin).sci_typ != 13)
            {
            a641check_datetime(&(*acv) , &(*dmli) , 13);
            }
        curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var17->a_ap_tree , "vak641.p" , 14283))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
        if ((int ) ptoc_Var18->n_symb == 191)
            {
            negativ = (char ) sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var17->a_ap_tree , "vak641.p" , 14287))[sql__setrang(curr_n , 0 , 32767)].n_symb == 10 , 0 , 1);
            if ((char ) sql__setrang(negativ , 0 , 1))
                {
                curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var17->a_ap_tree , "vak641.p" , 14290))[sql__setrang(curr_n , 0 , 32767)].n_lo_level;
                }
            ptoc_Var19->d_datatype = 15;
            }
        else
            {
            ptoc_Var19->d_datatype = 14;
            }
        colin1.sci_len = 0;
        a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
        if ((int ) ptoc_Var18->n_symb == 106 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var17->a_return_segm , "vak641.p" , 14300)).integer.C_4.sp1r_returncode == 21000)
            {
            (*(tsp1_segment *) sql__nil(ptoc_Var17->a_return_segm , "vak641.p" , 14303)).integer.C_4.sp1r_returncode = 0;
            ptoc_Var19->d_datatype = 16;
            a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var17->a_return_segm , "vak641.p" , 14308)).integer.C_4.sp1r_returncode == 0)
            {
            if ((int ) ptoc_Var19->d_datatype != 15 && (int ) ptoc_Var19->d_datatype != 2 && (int ) ptoc_Var19->d_datatype != 3 && (int ) ptoc_Var19->d_datatype != 24)
                {
                *wrong_datatype = 1;
                }
            else
                {
                if ((int ) ptoc_Var19->d_datatype == 24)
                    {
                    a641string_set_operator(&(*acv) , 66 , (int ) colin1.sci_len + 1 , 0 , (unsigned char)'\024' , (unsigned char)'\0');
                    }
                ptoc_Var19->d_datatype = 13;
                ptoc_Var20 = &*colin;
                ptoc_Var20->sci_len = 26;
                ptoc_Var20->sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var20->sci_len , -32768 , 32767 , "vak641.p" , 14328) , -32768 , 32767);
                ptoc_Var20->sci_frac = 0;
                ptoc_Var20->sci_typ = 13;
                if ((int ) ptoc_Var18->n_symb == 191)
                    {
                    if ((char ) sql__setrang(negativ , 0 , 1))
                        {
                        a65_set_operator(&(*acv) , 61);
                        }
                    else
                        {
                        a65_set_operator(&(*acv) , 48);
                        }
                    }
                else
                    {
                    a641string_set_operator(&(*acv) , 28 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                    ptoc_Var21 = &ptoc_Var17->a_mblock;
                    ptoc_Var22 = (tgg00_QualBuf *) sql__nil(ptoc_Var21->integer.C_4.mb_qual , "vak641.p" , 14345);
                    _ptoc_ASSIGN(unsigned char , 3, (*(tgg00_StackList *) sql__nil(ptoc_Var21->integer.C_5.mb_st , "vak641.p" , 14346))[sql__setrang((int ) ptoc_Var22->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_4.elanguage, ptoc_Var17->a_ak_language)
                    }
                }
            }
        }
    cccprint("<ak641_add_month");
    }



int ak641new_time(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    int curr_n;
    tak00_scolinf colin1;
    tak00_scolinf colin2;
    tak_all_command_glob *ptoc_Var23;
    struct ptoc_Type59 *ptoc_Var24;
    tak_dml_info *ptoc_Var25;
    tak00_scolinf *ptoc_Var26;
    cccprint("AK641 ak641new_t");
    ptoc_Var23 = &*acv;
    ptoc_Var24 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var23->a_ap_tree , "vak641.p" , 14374))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var25 = &*dmli;
    ptoc_Var25->d_datatype = 13;
    curr_n = (int ) ptoc_Var24->n_lo_level;
    (*colin).sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var23->a_return_segm , "vak641.p" , 14380)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) (*colin).sci_typ != 13)
            {
            a641check_datetime(&(*acv) , &(*dmli) , 13);
            }
        curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var23->a_ap_tree , "vak641.p" , 14387))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
        if ((char ) sql__setrang(g01unicode , 0 , 1) && (int ) g01code.char_default == 20)
            {
            ptoc_Var25->d_datatype = 2;
            }
        else
            {
            ptoc_Var25->d_datatype = 14;
            }
        colin1.sci_len = 0;
        a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var23->a_return_segm , "vak641.p" , 14397)).integer.C_4.sp1r_returncode == 0)
            {
            if ((int ) ptoc_Var25->d_datatype != 2 && (int ) ptoc_Var25->d_datatype != 3)
                {
                *wrong_datatype = 1;
                }
            else
                {
                curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var23->a_ap_tree , "vak641.p" , 14405))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
                if ((char ) sql__setrang(g01unicode , 0 , 1) && (int ) g01code.char_default == 20)
                    {
                    ptoc_Var25->d_datatype = 2;
                    }
                else
                    {
                    ptoc_Var25->d_datatype = 14;
                    }
                colin2.sci_len = 0;
                a640not_first_factor(&(*acv) , &(*dmli) , &colin2 , &curr_n);
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var23->a_return_segm , "vak641.p" , 14415)).integer.C_4.sp1r_returncode == 0)
                    {
                    if ((int ) ptoc_Var25->d_datatype != 2 && (int ) ptoc_Var25->d_datatype != 3)
                        {
                        *wrong_datatype = 1;
                        }
                    else
                        {
                        ptoc_Var25->d_datatype = 13;
                        ptoc_Var26 = &*colin;
                        ptoc_Var26->sci_len = 26;
                        ptoc_Var26->sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var26->sci_len , -32768 , 32767 , "vak641.p" , 14427) , -32768 , 32767);
                        ptoc_Var26->sci_frac = 0;
                        ptoc_Var26->sci_typ = 13;
                        a641string_set_operator(&(*acv) , 41 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                        }
                    }
                }
            }
        }
    cccprint("<ak641new_time  ");
    }

typedef
  unsigned char tak_map_set_a641add_mapchar[768];


int a641add_mapchar(acv, dmli, colin, setname, curr_n, first_int)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
tsp_knl_identifier setname;
int curr_n;
int first_int;
    {
    int max_maplen;
    tsp_int4 curr_data_pos;
    tak_sysbufferaddress sbuf;
    struct ptoc_Type179
      {
      union  
        {
        struct  
          {
          tak_map_set_a641add_mapchar *mapsetptr;
          } C_true;
        struct  
          {
          tsp_moveobj_ptr moveobjptr;
          } C_false;
        } boolean;
      } univ_ptr;
    tak_all_command_glob *ptoc_Var27;
    tak_dml_info *ptoc_Var28;
    struct tgg00_MessBlock *ptoc_Var29;
    tgg00_DataPart *ptoc_Var30;
    struct tak_systembuffer *ptoc_Var31;
    tak_mapset_record *ptoc_Var32;
    int ptoc_Var33;
    tgg00_QualBuf *ptoc_Var34;
    tgg00_StackEntry *ptoc_Var35;
    cccprint("AK641 a641add_ma");
    ptoc_Var27 = &*acv;
    ptoc_Var28 = &*dmli;
    a641get_mapcharset(&(*acv) , setname , (*(tak_ap_max_tree *) sql__nil(ptoc_Var27->a_ap_tree , "vak641.p" , 14474))[sql__setrang(curr_n , 0 , 32767)].n_pos , &max_maplen , &sbuf);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var27->a_return_segm , "vak641.p" , 14475)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var29 = &ptoc_Var27->a_mblock;
        ptoc_Var30 = (tgg00_DataPart *) sql__nil(ptoc_Var29->integer.C_3.mb_data , "vak641.p" , 14477);
        ptoc_Var31 = (struct tak_systembuffer *) sql__nil(sbuf , "vak641.p" , 14477);
        ptoc_Var32 = &ptoc_Var31->integer.C_53.smapset;
        if (ptoc_Var29->integer.C_2.mb_data_len + (int ) ptoc_Var32->map_count * 3 + 1 > ptoc_Var29->integer.C_2.mb_data_size)
            {
            a07_b_put_error(&(*acv) , 2801 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var27->a_ap_tree , "vak641.p" , 14481))[sql__setrang(curr_n , 0 , 32767)].n_pos);
            }
        else
            {
            ptoc_Var29->integer.C_2.mb_data_len = ptoc_Var29->integer.C_2.mb_data_len + 1;
            curr_data_pos = ptoc_Var29->integer.C_2.mb_data_len;
            if (((ptoc_Var33 = (int ) (*colin).sci_typ) == 11 || ptoc_Var33 == 10 || ptoc_Var33 == 13) && (int ) ptoc_Var32->map_code == 1)
                {
                ptoc_Var30->integer.C_1.mbp_buf[sql__setrang(curr_data_pos , 1 , 8192000) + -1] = (unsigned char)' ';
                univ_ptr.boolean.C_true.mapsetptr = (tak_map_set_a641add_mapchar *) ptoc_Var32->map_set;
                s30map(g02codetables.tables[1] , sql__nil(univ_ptr.boolean.C_false.moveobjptr , "vak641.p" , 14494) , 1 , ptoc_Var30->integer.C_1.mbp_buf , curr_data_pos + 1 , (int ) ptoc_Var32->map_count * 3);
                }
            else
                {
                if ((int ) (*colin).sci_typ == 2 && (int ) ptoc_Var32->map_code == 1)
                    {
                    ptoc_Var30->integer.C_1.mbp_buf[sql__setrang(curr_data_pos , 1 , 8192000) + -1] = (unsigned char)' ';
                    univ_ptr.boolean.C_true.mapsetptr = (tak_map_set_a641add_mapchar *) ptoc_Var32->map_set;
                    s30map(g02codetables.tables[1] , sql__nil(univ_ptr.boolean.C_false.moveobjptr , "vak641.p" , 14507) , 1 , ptoc_Var30->integer.C_1.mbp_buf , curr_data_pos + 1 , (int ) ptoc_Var32->map_count * 3);
                    }
                else
                    {
                    if ((int ) (*colin).sci_typ == 3 && (int ) ptoc_Var32->map_code == 0)
                        {
                        ptoc_Var30->integer.C_1.mbp_buf[sql__setrang(curr_data_pos , 1 , 8192000) + -1] = (unsigned char)'@';
                        univ_ptr.boolean.C_true.mapsetptr = (tak_map_set_a641add_mapchar *) ptoc_Var32->map_set;
                        s30map(g02codetables.tables[0] , sql__nil(univ_ptr.boolean.C_false.moveobjptr , "vak641.p" , 14520) , 1 , ptoc_Var30->integer.C_1.mbp_buf , curr_data_pos + 1 , (int ) ptoc_Var32->map_count * 3);
                        }
                    else
                        {
                        if ((int ) ptoc_Var32->map_code == 0)
                            {
                            ptoc_Var30->integer.C_1.mbp_buf[sql__setrang(curr_data_pos , 1 , 8192000) + -1] = (unsigned char)' ';
                            }
                        else
                            {
                            ptoc_Var30->integer.C_1.mbp_buf[sql__setrang(curr_data_pos , 1 , 8192000) + -1] = (unsigned char)'@';
                            }
                        g10mv1("VAK641" , 1 , sizeof(tak_map_set ) , ptoc_Var29->integer.C_2.mb_data_size , ptoc_Var32->map_set , 1 , ptoc_Var30->integer.C_1.mbp_buf , curr_data_pos + 1 , (int ) ptoc_Var32->map_count * 3 , &(*(tsp1_segment *) sql__nil(ptoc_Var27->a_return_segm , "vak641.p" , 14539)).integer.C_4.sp1r_returncode);
                        }
                    }
                }
            t01name(5 , "map_vorschrift    ");
            t01moveobj(5 , ptoc_Var30->integer.C_1.mbp_buf , curr_data_pos , curr_data_pos + (int ) ptoc_Var32->map_count * 3);
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var27->a_return_segm , "vak641.p" , 14547)).integer.C_4.sp1r_returncode == 0)
                {
                ptoc_Var34 = (tgg00_QualBuf *) sql__nil(ptoc_Var29->integer.C_4.mb_qual , "vak641.p" , 14550);
                if ((int ) ptoc_Var34->integer.C_2.mfirst_free > (int ) ptoc_Var29->integer.C_2.mb_st_max)
                    {
                    a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var29->integer.C_2.mb_st_max);
                    }
                else
                    {
                    ptoc_Var34->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var34->integer.C_2.mqual_cnt , -32768 , 32767 , "vak641.p" , 14557) , -32768 , 32767);
                    ptoc_Var35 = &(*(tgg00_StackList *) sql__nil(ptoc_Var29->integer.C_5.mb_st , "vak641.p" , 14558))[sql__setrang((int ) ptoc_Var34->integer.C_2.mfirst_free , 1 , 32767) + -1];
                    ptoc_Var35->integer.C_1.etype = 22;
                    ptoc_Var35->integer.C_1.eop = 0;
                    ptoc_Var35->integer.C_1.epos = (short ) sql__setrang((short ) curr_data_pos , -32768 , 32767);
                    ptoc_Var35->integer.C_1.elen_var = (short ) sql__setrang((short ) ((int ) ptoc_Var32->map_count * 3 + 1) , -32768 , 32767);
                    ptoc_Var35->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                    ptoc_Var35->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                    ptoc_Var34->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var34->integer.C_2.mfirst_free , -32768 , 32767 , "vak641.p" , 14568) , -32768 , 32767);
                    }
                ptoc_Var29->integer.C_2.mb_data_len = ptoc_Var29->integer.C_2.mb_data_len + (int ) ptoc_Var32->map_count * 3;
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var27->a_cmd_segm , "vak641.p" , 14573)).integer.C_3.sp1c_mess_type == 3)
                    {
                    ptoc_Var28->d_movebefore = (short ) sql__setrang((short ) ((int ) ptoc_Var28->d_movebefore + (int ) ptoc_Var32->map_count * 3 + 1) , -32768 , 32767);
                    }
                }
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var27->a_return_segm , "vak641.p" , 14584)).integer.C_4.sp1r_returncode == 0)
        {
        a65_set_operator(&(*acv) , 1);
        if ((ptoc_Var33 = (int ) (*colin).sci_typ) == 11 || ptoc_Var33 == 10 || ptoc_Var33 == 13)
            {
            if ((int ) g01code.ctype == 1)
                {
                (*colin).sci_typ = 3;
                }
            else
                {
                (*colin).sci_typ = 2;
                }
            }
        (*colin).sci_len = (short ) sql__setrang((short ) first_int , -32768 , 32767);
        (*colin).sci_frac = 0;
        (*colin).sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) (*colin).sci_len , -32768 , 32767 , "vak641.p" , 14599) , -32768 , 32767);
        a641string_set_operator(&(*acv) , 2 , first_int , 0 , (unsigned char)'\0' , (unsigned char)'\0');
        }
    cccprint("<a641add_mapchar");
    }



int a641get_mapcharset(acv, mapcharsetname, error_pos, max_maplen, sysbuf)
tak_all_command_glob *acv;
tsp_knl_identifier mapcharsetname;
int error_pos;
int *max_maplen;
tak_sysbufferaddress *sysbuf;
    {
    int pos;
    int cnt;
    tgg_basis_error b_err;
    tgg_sysinfokey sysk;
    cccprint("AK641 a641get_ma");
    sysk = a01defaultkey;
    _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0Q")
    _ptoc_ASSIGN(unsigned char , 64, sysk.integer.C_13.sidentifier, mapcharsetname)
    sysk.skeylen = (short ) sql__setrang((short ) (sizeof(tsp00_C8 ) + sizeof(tsp00_C2 ) + sizeof(tsp00_C2 ) + sizeof(tsp00_C64 )) , -32768 , 32767);
    a10get_sysinfo(&(*acv) , &sysk , 1 , &(*sysbuf) , &b_err);
    if ((int ) b_err != 0)
        {
        if ((int ) b_err == 1930)
            {
            a07_nb_put_error(&(*acv) , 6506 , error_pos , mapcharsetname);
            }
        else
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
            }
        }
    else
        {
        *max_maplen = 1;
        pos = 3;
        cnt = (int ) (*(struct tak_systembuffer *) sql__nil(*sysbuf , "vak641.p" , 14646)).integer.C_53.smapset.map_count;
        while (cnt > 0)
            if ((int ) (*(struct tak_systembuffer *) sql__nil(*sysbuf , "vak641.p" , 14648)).integer.C_53.smapset.map_set[sql__setrang(pos , 1 , 768) + -1] != 32)
                {
                *max_maplen = 2;
                cnt = 0;
                }
            else
                {
                cnt = cnt + -1;
                pos = pos + 3;
                }
        }
    cccprint("<a641get_mapchar");
    }



int ak641mapchar(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    int curr_n;
    tsp_int2 first_int;
    tsp_knl_identifier setname;
    tak_all_command_glob *ptoc_Var36;
    struct ptoc_Type59 *ptoc_Var37;
    tak_dml_info *ptoc_Var38;
    cccprint("AK641 ak641mapch");
    ptoc_Var36 = &*acv;
    ptoc_Var37 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var36->a_ap_tree , "vak641.p" , 14680))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var38 = &*dmli;
    if ((int ) ptoc_Var38->d_datatype == 14)
        {
        ptoc_Var38->d_datatype = 16;
        }
    ptoc_Var38->d_ch_datatype = 14;
    curr_n = (int ) ptoc_Var37->n_lo_level;
    (*colin).sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var36->a_return_segm , "vak641.p" , 14690)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) (*colin).sci_typ == 4 || (int ) (*colin).sci_typ == 18 || (int ) (*colin).sci_typ == 24)
            {
            *wrong_datatype = 1;
            }
        else
            {
            first_int = (short ) sql__setrang((*colin).sci_len , -32768 , 32767);
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var36->a_ap_tree , "vak641.p" , 14701))[sql__setrang(curr_n , 0 , 32767)].n_sa_level != 0 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var36->a_ap_tree , "vak641.p" , 14702))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var36->a_ap_tree , "vak641.p" , 14702))[sql__setrang(curr_n , 0 , 32767)].n_sa_level , 0 , 32767)].n_symb == 49)
                {
                curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var36->a_ap_tree , "vak641.p" , 14706))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
                a05_unsigned_int2_get(&(*acv) , (*(tak_ap_max_tree *) sql__nil(ptoc_Var36->a_ap_tree , "vak641.p" , 14707))[sql__setrang(curr_n , 0 , 32767)].n_pos , (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var36->a_ap_tree , "vak641.p" , 14708))[sql__setrang(curr_n , 0 , 32767)].n_length , -32768 , 32767) , 6296 , &first_int);
                if ((int ) first_int < 1 || (int ) first_int > 8000)
                    {
                    a07_b_put_error(&(*acv) , 6296 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var36->a_ap_tree , "vak641.p" , 14712))[sql__setrang(curr_n , 0 , 32767)].n_pos);
                    }
                }
            else
                {
                if ((char ) sql__setrang(ptoc_Var38->d_wherepart , 0 , 1) || (int ) ptoc_Var36->a_sqlmode == 2)
                    {
                    if ((int ) first_int <= 10)
                        {
                        first_int = (short ) sql__setrang((short ) ((int ) first_int * 2) , -32768 , 32767);
                        }
                    else
                        {
                        if ((int ) first_int <= 100)
                            {
                            first_int = (short ) sql__setrang((short ) ((int ) first_int + 10) , -32768 , 32767);
                            }
                        else
                            {
                            if ((int ) first_int <= 230)
                                {
                                first_int = (short ) sql__setrang((short ) ((int ) first_int + 20) , -32768 , 32767);
                                }
                            else
                                {
                                first_int = (short ) sql__setrang((short ) ((int ) first_int + (int ) first_int / 10) , -32768 , 32767);
                                if ((int ) ptoc_Var36->a_sqlmode == 2)
                                    {
                                    if ((int ) first_int > 8000)
                                        {
                                        first_int = 8000;
                                        }
                                    }
                                else
                                    {
                                    if ((int ) first_int > 8000)
                                        {
                                        first_int = 8000;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var36->a_ap_tree , "vak641.p" , 14753))[sql__setrang(curr_n , 0 , 32767)].n_sa_level != 0 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var36->a_ap_tree , "vak641.p" , 14754))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var36->a_ap_tree , "vak641.p" , 14754))[sql__setrang(curr_n , 0 , 32767)].n_sa_level , 0 , 32767)].n_symb == 42)
                {
                curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var36->a_ap_tree , "vak641.p" , 14758))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
                a05identifier_get(&(*acv) , curr_n , sizeof(tsp00_C64 ) , setname);
                }
            else
                {
                a061assign_colname("DEFAULTMAP        " , setname);
                }
            a641add_mapchar(&(*acv) , &(*dmli) , &(*colin) , setname , curr_n , (int ) first_int);
            }
        }
    cccprint("<ak641mapchar   ");
    }



int ak641char(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    int curr_n;
    tsp_data_type keep_datatype;
    tsp_data_type dtyp;
    tak_all_command_glob *ptoc_Var39;
    struct ptoc_Type59 *ptoc_Var40;
    tak_dml_info *ptoc_Var41;
    int ptoc_Var42;
    struct ptoc_Type59 *ptoc_Var43;
    tak00_scolinf *ptoc_Var44;
    tak00_scolinf *ptoc_Var45;
    struct tgg00_MessBlock *ptoc_Var46;
    tgg00_QualBuf *ptoc_Var47;
    tgg00_StackEntry *ptoc_Var48;
    cccprint("AK641 ak641char ");
    ptoc_Var39 = &*acv;
    ptoc_Var40 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var39->a_ap_tree , "vak641.p" , 14789))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var41 = &*dmli;
    if ((ptoc_Var42 = (int ) ptoc_Var41->d_datatype) != 14 && ptoc_Var42 != 16 && ptoc_Var42 != 2 && ptoc_Var42 != 3 && ptoc_Var42 != 24 && ptoc_Var42 != 10 && ptoc_Var42 != 11 && ptoc_Var42 != 13)
        {
        *wrong_datatype = 1;
        }
    else
        {
        keep_datatype = (unsigned char ) sql__setrang(ptoc_Var41->d_datatype , 0 , 39);
        ptoc_Var41->d_datatype = 14;
        curr_n = (int ) ptoc_Var40->n_lo_level;
        (*colin).sci_len = 0;
        a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var39->a_ap_tree , "vak641.p" , 14803))[sql__setrang((int ) ptoc_Var40->n_lo_level , 0 , 32767)].n_symb == 50)
            {
            ptoc_Var41->d_datatype = 10;
            (*colin).sci_typ = 10;
            }
        ptoc_Var43 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var39->a_ap_tree , "vak641.p" , 14810))[sql__setrang((int ) ptoc_Var40->n_lo_level , 0 , 32767)];
        if ((int ) ptoc_Var43->n_proc == 58 && (int ) ptoc_Var41->d_datatype == 2 && ((ptoc_Var42 = (int ) ptoc_Var43->n_datatype) == 10 || ptoc_Var42 == 11 || ptoc_Var42 == 13))
            {
            a641check_datetime(&(*acv) , &(*dmli) , (unsigned char ) sql__setrang(ptoc_Var43->n_datatype , 0 , 39));
            (*colin).sci_typ = (unsigned char ) sql__setrang(ptoc_Var43->n_datatype , 0 , 39);
            switch ((int ) ptoc_Var43->n_datatype)
                {
                case 10 :
                    (*colin).sci_len = 8;
                    break;
                case 11 :
                    (*colin).sci_len = 8;
                    break;
                case 13 :
                    (*colin).sci_len = 20;
                    break;
                default :
                    (unsigned char ) sql__caseerr("vak641.p" , 14817);
                    break;
                }
            ptoc_Var41->d_datatype = (unsigned char ) sql__setrang(ptoc_Var43->n_datatype , 0 , 39);
            }
        t01int4(5 , "dattype     " , sql__ord((int ) ptoc_Var41->d_datatype));
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var39->a_return_segm , "vak641.p" , 14831)).integer.C_4.sp1r_returncode == 21000 && (int ) ptoc_Var41->d_datatype != 0)
            {
            (*(tsp1_segment *) sql__nil(ptoc_Var39->a_return_segm , "vak641.p" , 14836)).integer.C_4.sp1r_returncode = 0;
            a07_b_put_error(&(*acv) , 6556 , ptoc_Var40->n_pos);
            }
        else
            {
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var39->a_return_segm , "vak641.p" , 14840)).integer.C_4.sp1r_returncode == 0)
                {
                if ((ptoc_Var42 = (int ) ptoc_Var41->d_datatype) != 0 && ptoc_Var42 != 15 && ptoc_Var42 != 11 && ptoc_Var42 != 10 && ptoc_Var42 != 13)
                    {
                    *wrong_datatype = 1;
                    }
                else
                    {
                    if ((ptoc_Var42 = (int ) ptoc_Var41->d_datatype) == 0 || ptoc_Var42 == 15)
                        {
                        if ((int ) ptoc_Var40->n_length != sql__ord(0) || (int ) ptoc_Var39->a_sqlmode != 1 || (int ) (*colin).sci_typ == 1 || (int ) (*colin).sci_typ == 12)
                            {
                            if ((int ) ptoc_Var39->a_sqlmode != 1 || (int ) (*colin).sci_typ == 1 || (int ) (*colin).sci_typ == 12)
                                {
                                a07_b_put_error(&(*acv) , 6282 , ptoc_Var40->n_pos);
                                }
                            else
                                {
                                a07_b_put_error(&(*acv) , 6304 , ptoc_Var40->n_pos);
                                }
                            }
                        else
                            {
                            ptoc_Var44 = &*colin;
                            if ((int ) ptoc_Var44->sci_frac != 0)
                                {
                                ptoc_Var44->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var44->sci_len + 2) , -32768 , 32767);
                                }
                            else
                                {
                                ptoc_Var44->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var44->sci_len + 1) , -32768 , 32767);
                                }
                            a641stack_for_op_b_chr(&(*acv) , &(*dmli) , &(*colin) , 0 , (unsigned char ) sql__setrang(keep_datatype , 0 , 39));
                            }
                        }
                    else
                        {
                        ptoc_Var45 = &*colin;
                        if ((int ) ptoc_Var40->n_length == sql__ord(0))
                            {
                            ptoc_Var40->n_length = (short ) sql__setrang((short ) sql__ord((int ) ptoc_Var39->a_dt_format) , -32768 , 32767);
                            }
                        dtyp = (unsigned char ) sql__setrang(ptoc_Var41->d_datatype , 0 , 39);
                        switch ((int ) ptoc_Var41->d_datatype)
                            {
                            case 10 :
                                if ((int ) ptoc_Var40->n_length != sql__ord(1))
                                    {
                                    ptoc_Var45->sci_len = 10;
                                    }
                                break;
                            case 11 :
                                if ((int ) ptoc_Var40->n_length != sql__ord(1))
                                    {
                                    ptoc_Var45->sci_len = 8;
                                    }
                                break;
                            case 13 :
                                if ((int ) ptoc_Var40->n_length != sql__ord(1))
                                    {
                                    if ((int ) ptoc_Var40->n_length != sql__ord(6))
                                        {
                                        ptoc_Var45->sci_len = 26;
                                        }
                                    }
                                break;
                            default :
                                (unsigned char ) sql__caseerr("vak641.p" , 14891);
                                break;
                            }
                        ptoc_Var41->d_datatype = 2;
                        ptoc_Var45->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var45->sci_len + 1) , -32768 , 32767);
                        ptoc_Var45->sci_frac = 0;
                        ptoc_Var45->sci_typ = (unsigned char ) sql__setrang(ptoc_Var41->d_datatype , 0 , 39);
                        ptoc_Var46 = &ptoc_Var39->a_mblock;
                        ptoc_Var47 = (tgg00_QualBuf *) sql__nil(ptoc_Var46->integer.C_4.mb_qual , "vak641.p" , 14927);
                        ptoc_Var48 = &(*(tgg00_StackList *) sql__nil(ptoc_Var46->integer.C_5.mb_st , "vak641.p" , 14927))[sql__setrang((int ) ptoc_Var47->integer.C_2.mfirst_free , 1 , 32767) + -1];
                        if ((int ) ptoc_Var47->integer.C_2.mfirst_free >= (int ) ptoc_Var46->integer.C_2.mb_st_max)
                            {
                            a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var46->integer.C_2.mb_st_max);
                            }
                        else
                            {
                            ptoc_Var48->integer.C_1.etype = 55;
                            ptoc_Var48->integer.C_4.eop_build_in = 9;
                            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var39->a_ap_tree , "vak641.p" , 14936))[sql__setrang(act_node , 0 , 32767)].n_length == sql__ord(1))
                                {
                                ptoc_Var48->integer.C_4.eformat = 1;
                                }
                            else
                                {
                                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var39->a_ap_tree , "vak641.p" , 14941))[sql__setrang(act_node , 0 , 32767)].n_length == sql__ord(2))
                                    {
                                    ptoc_Var48->integer.C_4.eformat = 2;
                                    }
                                else
                                    {
                                    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var39->a_ap_tree , "vak641.p" , 14946))[sql__setrang(act_node , 0 , 32767)].n_length == sql__ord(3))
                                        {
                                        ptoc_Var48->integer.C_4.eformat = 3;
                                        }
                                    else
                                        {
                                        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var39->a_ap_tree , "vak641.p" , 14951))[sql__setrang(act_node , 0 , 32767)].n_length == sql__ord(4))
                                            {
                                            ptoc_Var48->integer.C_4.eformat = 4;
                                            }
                                        else
                                            {
                                            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var39->a_ap_tree , "vak641.p" , 14956))[sql__setrang(act_node , 0 , 32767)].n_length == sql__ord(5))
                                                {
                                                ptoc_Var48->integer.C_4.eformat = 5;
                                                }
                                            else
                                                {
                                                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var39->a_ap_tree , "vak641.p" , 14961))[sql__setrang(act_node , 0 , 32767)].n_length == sql__ord(6))
                                                    {
                                                    ptoc_Var48->integer.C_4.eformat = 6;
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            ptoc_Var48->integer.C_4.edatatype = (unsigned char ) sql__setrang(dtyp , 0 , 39);
                            _ptoc_ASSIGN(unsigned char , 3, ptoc_Var48->integer.C_4.elanguage, ptoc_Var39->a_ak_language)
                            ptoc_Var48->integer.C_4.elength = (unsigned char ) sql__setrang((unsigned char ) (*colin).sci_iolen , 0 , 255);
                            ptoc_Var47->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var47->integer.C_2.mfirst_free , -32768 , 32767 , "vak641.p" , 14975) , -32768 , 32767);
                            ptoc_Var47->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var47->integer.C_2.mqual_cnt , -32768 , 32767 , "vak641.p" , 14976) , -32768 , 32767);
                            }
                        }
                    }
                }
            }
        }
    cccprint("<ak641char      ");
    }

static
  tsp8_uni_opt_set ptoc_Var49 = 
  {
  (unsigned char)'\0'
  };


int a641charset_get(acv, node, setname, messcode, codewidth)
tak_all_command_glob *acv;
tsp_int2 node;
tsp_knl_identifier setname;
tsp_code_type *messcode;
tsp00_Uint1 *codewidth;
    {
    tsp_int4 setnamelength;
    tsp8_uni_error uni_err;
    tsp_int4 err_char_no;
    tak_all_command_glob *ptoc_Var50;
    struct ptoc_Type59 *ptoc_Var51;
    tsp8_uni_opt_set ptoc_Var52;
    cccprint("AK641 a641charse");
    ptoc_Var50 = &*acv;
    ptoc_Var51 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var50->a_ap_tree , "vak641.p" , 15005))[sql__setrang((int ) node , 0 , 32767)];
    setnamelength = sizeof(tsp00_C64 );
    if ((char ) sql__setrang(g01unicode , 0 , 1))
        {
        s80uni_trans((tsp00_MoveObj *) &(*(tsp1_part *) sql__nil(ptoc_Var50->a_cmd_part , "vak641.p" , 15011)).integer.C_1.sp1p_buf[sql__setrang(ptoc_Var51->n_pos , 1 , 8192000) + -1] , (int ) ptoc_Var51->n_length , 20 , (tsp00_MoveObj *) setname , &setnamelength , 0 , *(tsp8_uni_opt_set *) (_ptoc_MOVE(unsigned char , 1, ptoc_Var52, ptoc_Var49) , ptoc_Var52) , &uni_err , &err_char_no);
        if ((int ) uni_err != 0)
            {
            a07_b_put_error(&(*acv) , 6527 , 1);
            }
        }
    else
        {
        if ((int ) ptoc_Var51->n_length > setnamelength)
            {
            a07_b_put_error(&(*acv) , 6527 , 1);
            }
        else
            {
            g10mv2("VAK641" , 2 , (*(tsp1_part *) sql__nil(ptoc_Var50->a_cmd_part , "vak641.p" , 15027)).integer.C_2.sp1p_buf_len , sizeof(tsp00_C64 ) , (*(tsp1_part *) sql__nil(ptoc_Var50->a_cmd_part , "vak641.p" , 15028)).integer.C_1.sp1p_buf , ptoc_Var51->n_pos , setname , 1 , (int ) ptoc_Var51->n_length , &(*(tsp1_segment *) sql__nil(ptoc_Var50->a_return_segm , "vak641.p" , 15029)).integer.C_4.sp1r_returncode);
            setnamelength = (int ) ptoc_Var51->n_length;
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var50->a_return_segm , "vak641.p" , 15035)).integer.C_4.sp1r_returncode == 0)
        {
        g10fil("VAK641" , 3 , sizeof(tsp00_C64 ) , setname , setnamelength + 1 , sizeof(tsp00_C64 ) - setnamelength , (unsigned char)' ' , &(*(tsp1_segment *) sql__nil(ptoc_Var50->a_return_segm , "vak641.p" , 15040)).integer.C_4.sp1r_returncode);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var50->a_return_segm , "vak641.p" , 15042)).integer.C_4.sp1r_returncode == 0)
        {
        g20get_uni_key(setname , &(*messcode) , &(*codewidth) , &uni_err);
        if ((int ) uni_err != 0)
            {
            a07_b_put_error(&(*acv) , 6527 , 1);
            }
        else
            {
            if ((int ) *messcode == 20 && (int ) (*(tsp1_packet *) sql__nil(ptoc_Var50->a_in_packet , "vak641.p" , 15051)).sp1_header.sp1h_mess_swap != 1)
                {
                *messcode = (unsigned char)'\023';
                }
            }
        }
    cccprint("<a641charset_get");
    }



int ak641mbcs(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    int curr_n;
    tsp_knl_identifier isostr;
    tsp_code_type messcode;
    tsp00_Uint1 codewidth;
    int curr_leng;
    int curr_data_pos;
    unsigned char ecoltab1;
    tak_all_command_glob *ptoc_Var53;
    struct ptoc_Type59 *ptoc_Var54;
    tak_dml_info *ptoc_Var55;
    tak_all_command_glob *ptoc_Var56;
    struct tgg00_MessBlock *ptoc_Var57;
    tgg00_QualBuf *ptoc_Var58;
    tgg00_DataPart *ptoc_Var59;
    tgg00_StackEntry *ptoc_Var60;
    tak00_scolinf *ptoc_Var61;
    cccprint("AK641 ak641mbcs ");
    ptoc_Var53 = &*acv;
    ptoc_Var54 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var53->a_ap_tree , "vak641.p" , 15081))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var55 = &*dmli;
    ptoc_Var55->d_datatype = 14;
    (*colin).sci_len = 0;
    curr_n = (int ) ptoc_Var54->n_lo_level;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var53->a_return_segm , "vak641.p" , 15087)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var53->a_ap_tree , "vak641.p" , 15089))[sql__setrang(curr_n , 0 , 32767)].n_sa_level != 0)
            {
            curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var53->a_ap_tree , "vak641.p" , 15092))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
            a641charset_get(&(*acv) , (short ) sql__setrang((short ) curr_n , -32768 , 32767) , isostr , &messcode , &codewidth);
            }
        else
            {
            messcode = (unsigned char ) sql__setrang((*(tsp1_packet *) sql__nil(ptoc_Var53->a_out_packet , "vak641.p" , 15097)).sp1_header.sp1h_mess_code , 0 , 255);
            g20get_uni_set((unsigned char ) sql__setrang(messcode , 0 , 255) , &codewidth , isostr);
            }
        }
    t01lidentifier(5 , isostr);
    t01int4(5 , "messcode    " , (int ) messcode);
    t01int4(5 , "codewidth   " , (int ) codewidth);
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak641.p" , 15105)).integer.C_4.sp1r_returncode == 0 && (int ) messcode > 20)
        {
        ptoc_Var56 = &*acv;
        ptoc_Var57 = &ptoc_Var56->a_mblock;
        ptoc_Var58 = (tgg00_QualBuf *) sql__nil(ptoc_Var57->integer.C_4.mb_qual , "vak641.p" , 15108);
        ptoc_Var59 = (tgg00_DataPart *) sql__nil(ptoc_Var57->integer.C_3.mb_data , "vak641.p" , 15108);
        messcode = (unsigned char)'\377';
        curr_data_pos = ptoc_Var56->a_mblock.integer.C_2.mb_data_len + 1;
        curr_leng = sizeof(tsp00_C64 );
        if ((int ) ptoc_Var58->integer.C_2.mfirst_free > (int ) ptoc_Var57->integer.C_2.mb_st_max)
            {
            a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var57->integer.C_2.mb_st_max);
            }
        else
            {
            if (ptoc_Var57->integer.C_2.mb_data_size < curr_data_pos + curr_leng + 1)
                {
                a07_b_put_error(&(*acv) , 2801 , -ptoc_Var57->integer.C_2.mb_data_size);
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var56->a_return_segm , "vak641.p" , 15122)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var59->integer.C_1.mbp_buf[sql__setrang(curr_data_pos , 1 , 8192000) + -1] = (unsigned char)' ';
            g10mv5("VAK641" , 4 , sizeof(tsp00_C64 ) , ptoc_Var57->integer.C_2.mb_data_size , isostr , 1 , ptoc_Var59->integer.C_1.mbp_buf , curr_data_pos + 1 , sizeof(tsp00_C64 ) , &(*(tsp1_segment *) sql__nil(ptoc_Var56->a_return_segm , "vak641.p" , 15129)).integer.C_4.sp1r_returncode);
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var56->a_return_segm , "vak641.p" , 15130)).integer.C_4.sp1r_returncode == 0)
                {
                ptoc_Var57->integer.C_2.mb_data_len = ptoc_Var57->integer.C_2.mb_data_len + curr_leng + 1;
                ptoc_Var58->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var58->integer.C_2.mqual_cnt , -32768 , 32767 , "vak641.p" , 15134) , -32768 , 32767);
                ptoc_Var60 = &(*(tgg00_StackList *) sql__nil(ptoc_Var57->integer.C_5.mb_st , "vak641.p" , 15135))[sql__setrang((int ) ptoc_Var58->integer.C_2.mfirst_free , 1 , 32767) + -1];
                ptoc_Var60->integer.C_1.etype = 22;
                ptoc_Var60->integer.C_1.eop = 0;
                ptoc_Var60->integer.C_1.epos = (short ) sql__setrang((short ) curr_data_pos , -32768 , 32767);
                ptoc_Var60->integer.C_1.elen_var = (short ) sql__setrang((short ) (curr_leng + 1) , -32768 , 32767);
                ptoc_Var60->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                ptoc_Var60->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                ptoc_Var58->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var58->integer.C_2.mfirst_free , -32768 , 32767 , "vak641.p" , 15145) , -32768 , 32767);
                }
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var53->a_return_segm , "vak641.p" , 15153)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var61 = &*colin;
        if ((int ) ptoc_Var61->sci_typ == 2)
            {
            ecoltab1 = (unsigned char)'\0';
            }
        else
            {
            ecoltab1 = (unsigned char)'\024';
            }
        ptoc_Var61->sci_typ = 4;
        ptoc_Var61->sci_len = (short ) sql__setrang((short ) ((int ) ptoc_Var61->sci_len * (int ) codewidth) , -32768 , 32767);
        ptoc_Var61->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var61->sci_len + 1) , -32768 , 32767);
        a641string_set_operator(&(*acv) , 66 , (int ) ptoc_Var61->sci_iolen , 0 , (unsigned char ) sql__setrang(ecoltab1 , 0 , 255) , (unsigned char ) sql__setrang((unsigned char ) messcode , 0 , 255));
        }
    t01messblock(5 , "a641mbcs    " , &(*acv).a_mblock);
    cccprint("<ak641mbcs      ");
    }



int ak641check_datatype(acv, colin, colin1, wrong_datatype)
tak_all_command_glob *acv;
tak00_scolinf *colin;
tak00_scolinf *colin1;
char *wrong_datatype;
    {
    int ptoc_Var62;
    cccprint("AK641 ak641check");
    switch ((int ) (*colin).sci_typ)
        {
        case 11 :
        case 10 :
        case 13 :
            if ((ptoc_Var62 = (int ) (*colin1).sci_typ) != 2 && ptoc_Var62 != 3 && ptoc_Var62 != 10 && ptoc_Var62 != 11 && ptoc_Var62 != 13)
                {
                *wrong_datatype = 1;
                }
            if ((int ) g01code.ctype == 0)
                {
                (*colin).sci_typ = 2;
                }
            else
                {
                (*colin).sci_typ = 3;
                }
            break;
        case 3 :
            if ((ptoc_Var62 = (int ) (*colin1).sci_typ) != 2 && ptoc_Var62 != 3 && ptoc_Var62 != 11 && ptoc_Var62 != 10 && ptoc_Var62 != 13)
                {
                *wrong_datatype = 1;
                }
            break;
        case 2 :
            if ((ptoc_Var62 = (int ) (*colin1).sci_typ) != 2 && ptoc_Var62 != 3 && ptoc_Var62 != 11 && ptoc_Var62 != 10 && ptoc_Var62 != 13)
                {
                if ((int ) (*colin1).sci_typ == 24)
                    {
                    (*colin).sci_typ = 24;
                    (*colin).sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) (*colin).sci_len * 2 , -2147483647 , 2147483647 , "vak641.p" , 15213) , -32768 , 32767);
                    }
                else
                    {
                    *wrong_datatype = 1;
                    }
                }
            break;
        case 4 :
        case 18 :
            if ((int ) (*colin1).sci_typ != (int ) (*colin).sci_typ)
                {
                *wrong_datatype = 1;
                }
            break;
        case 24 :
            if ((int ) (*colin1).sci_typ != (int ) (*colin).sci_typ)
                {
                if ((ptoc_Var62 = (int ) (*colin1).sci_typ) == 2 || ptoc_Var62 == 3)
                    {
                    if ((int ) (*colin1).sci_typ == 3)
                        {
                        a65_set_operator(&(*acv) , 2);
                        }
                    (*colin1).sci_typ = 24;
                    (*colin1).sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) (*colin1).sci_len * 2 , -2147483647 , 2147483647 , "vak641.p" , 15235) , -32768 , 32767);
                    a641string_set_operator(&(*acv) , 66 , (int ) (*colin1).sci_iolen , 0 , (unsigned char)'\0' , (unsigned char)'\024');
                    }
                else
                    {
                    *wrong_datatype = 1;
                    }
                }
            break;
        default :
            *wrong_datatype = 1;
            break;
        }
    cccprint("<ak641check_data");
    }



int ak641chr(acv, dmli, act_node, colin, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
int act_node;
tak00_scolinf *colin;
char *wrong_datatype;
    {
    tsp_data_type keep_datatype;
    int curr_n;
    tsp_int2 first_int;
    tak_all_command_glob *ptoc_Var63;
    struct ptoc_Type59 *ptoc_Var64;
    tak_dml_info *ptoc_Var65;
    int ptoc_Var66;
    tak00_scolinf *ptoc_Var67;
    cccprint("AK641 ak641chr  ");
    ptoc_Var63 = &*acv;
    ptoc_Var64 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var63->a_ap_tree , "vak641.p" , 15265))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var65 = &*dmli;
    keep_datatype = (unsigned char ) sql__setrang(ptoc_Var65->d_datatype , 0 , 39);
    ptoc_Var65->d_datatype = 14;
    curr_n = (int ) ptoc_Var64->n_lo_level;
    (*colin).sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var63->a_return_segm , "vak641.p" , 15272)).integer.C_4.sp1r_returncode == 21000)
        {
        (*(tsp1_segment *) sql__nil(ptoc_Var63->a_return_segm , "vak641.p" , 15275)).integer.C_4.sp1r_returncode = 0;
        ptoc_Var65->d_datatype = 15;
        a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var63->a_return_segm , "vak641.p" , 15280)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) (*colin).sci_typ == 18 || (int ) keep_datatype != 24 && (int ) keep_datatype != 14 && (int ) (*colin).sci_typ == 24 || (int ) keep_datatype == 4 && (int ) (*colin).sci_typ != 4 || (int ) keep_datatype != 4 && (int ) keep_datatype != 14 && (int ) (*colin).sci_typ == 4 || (int ) ptoc_Var63->a_sqlmode == 2 && ((ptoc_Var66 = (int ) ptoc_Var65->d_datatype) == 2 || ptoc_Var66 == 3))
            {
            *wrong_datatype = 1;
            }
        else
            {
            first_int = 0;
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var63->a_ap_tree , "vak641.p" , 15298))[sql__setrang(curr_n , 0 , 32767)].n_sa_level != 0)
                {
                curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var63->a_ap_tree , "vak641.p" , 15301))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
                a05_unsigned_int2_get(&(*acv) , (*(tak_ap_max_tree *) sql__nil(ptoc_Var63->a_ap_tree , "vak641.p" , 15302))[sql__setrang(curr_n , 0 , 32767)].n_pos , (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var63->a_ap_tree , "vak641.p" , 15303))[sql__setrang(curr_n , 0 , 32767)].n_length , -32768 , 32767) , 6296 , &first_int);
                if ((int ) first_int < 1 || (int ) first_int > 8000 || (int ) first_int * 2 > 8000 && (int ) ptoc_Var65->d_datatype == 24)
                    {
                    a07_b_put_error(&(*acv) , 6296 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var63->a_ap_tree , "vak641.p" , 15312))[sql__setrang(curr_n , 0 , 32767)].n_pos);
                    }
                }
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var63->a_return_segm , "vak641.p" , 15316)).integer.C_4.sp1r_returncode == 0)
                {
                if ((int ) ptoc_Var65->d_datatype == 24)
                    {
                    ptoc_Var67 = &*colin;
                    if ((int ) first_int > 0)
                        {
                        ptoc_Var67->sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) first_int * 2 , -2147483647 , 2147483647 , "vak641.p" , 15325) , -32768 , 32767);
                        if ((char ) sql__setrang(ptoc_Var65->d_concat , 0 , 1) || (char ) sql__setrang(ptoc_Var65->d_view , 0 , 1))
                            {
                            a641string_set_operator(&(*acv) , 8 , (int ) ptoc_Var67->sci_iolen , (int ) ptoc_Var67->sci_len , (unsigned char)'\0' , (unsigned char)'\0');
                            }
                        ptoc_Var65->d_expand = (short ) sql__setrang(first_int , -32768 , 32767);
                        ptoc_Var67->sci_frac = 0;
                        ptoc_Var67->sci_len = (short ) sql__setrang(first_int , -32768 , 32767);
                        a641string_set_operator(&(*acv) , 2 , (int ) (short ) sql__pred((int ) ptoc_Var67->sci_iolen , -32768 , 32767 , "vak641.p" , 15336) , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                        }
                    }
                else
                    {
                    a641stack_for_op_b_chr(&(*acv) , &(*dmli) , &(*colin) , (short ) sql__setrang(first_int , -32768 , 32767) , (unsigned char ) sql__setrang(keep_datatype , 0 , 39));
                    }
                }
            }
        }
    cccprint("<ak641chr       ");
    }



int ak641chr_ora(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    int curr_n;
    tak_all_command_glob *ptoc_Var68;
    struct ptoc_Type59 *ptoc_Var69;
    tak_dml_info *ptoc_Var70;
    tak00_scolinf *ptoc_Var71;
    cccprint("AK641 ak641chr_o");
    ptoc_Var68 = &*acv;
    ptoc_Var69 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var68->a_ap_tree , "vak641.p" , 15366))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var70 = &*dmli;
    (*colin).sci_len = 0;
    curr_n = (int ) ptoc_Var69->n_lo_level;
    ptoc_Var70->d_datatype = 15;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var68->a_return_segm , "vak641.p" , 15372)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var71 = &*colin;
        ptoc_Var71->sci_frac = 0;
        ptoc_Var71->sci_len = 1;
        ptoc_Var71->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var71->sci_len + 1) , -32768 , 32767);
        if ((int ) g01code.ctype == 0)
            {
            ptoc_Var70->d_datatype = 2;
            }
        else
            {
            ptoc_Var70->d_datatype = 3;
            }
        ptoc_Var71->sci_typ = (unsigned char ) sql__setrang(ptoc_Var70->d_datatype , 0 , 39);
        a641string_set_operator(&(*acv) , 18 , (int ) ptoc_Var71->sci_iolen , 0 , (unsigned char ) sql__setrang((unsigned char ) g01code.ctype , 0 , 255) , (unsigned char)'\0');
        }
    cccprint("<ak641chr_ora   ");
    }

static
  tak00_colpropset ptoc_Var72 = 
  {
  (unsigned char)'\002', (unsigned char)'\0'
  };


int ak641concat(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    int curr_n;
    int max_length;
    int max_loop_cnt;
    tak00_scolinf colin1;
    tsp_data_type keep_datatype;
    tsp_data_type h_datatype;
    tak_all_command_glob *ptoc_Var73;
    struct ptoc_Type59 *ptoc_Var74;
    struct tgg00_MessBlock *ptoc_Var75;
    tgg00_QualBuf *ptoc_Var76;
    tak_dml_info *ptoc_Var77;
    int ptoc_Var78;
    cccprint("AK641 ak641conca");
    ptoc_Var73 = &*acv;
    ptoc_Var74 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var73->a_ap_tree , "vak641.p" , 15418))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var75 = &ptoc_Var73->a_mblock;
    ptoc_Var76 = (tgg00_QualBuf *) sql__nil(ptoc_Var75->integer.C_4.mb_qual , "vak641.p" , 15418);
    ptoc_Var77 = &*dmli;
    keep_datatype = (unsigned char ) sql__setrang(ptoc_Var77->d_datatype , 0 , 39);
    if ((int ) ptoc_Var77->d_datatype == 10 || (int ) ptoc_Var77->d_datatype == 11 || (int ) ptoc_Var77->d_datatype == 13)
        {
        if ((int ) g01code.ctype == 0)
            {
            ptoc_Var77->d_datatype = 2;
            }
        else
            {
            ptoc_Var77->d_datatype = 3;
            }
        }
    else
        {
        if ((int ) ptoc_Var77->d_datatype == 14)
            {
            ptoc_Var77->d_datatype = 16;
            }
        }
    ptoc_Var77->d_concat = 1;
    curr_n = (int ) ptoc_Var74->n_lo_level;
    (*colin).sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var73->a_return_segm , "vak641.p" , 15441)).integer.C_4.sp1r_returncode == 0)
        {
        t01int4(5 , "colin.typ co" , sql__ord((int ) (*colin).sci_typ));
        t01int4(5 , "d_datatype =" , sql__ord((int ) ptoc_Var77->d_datatype));
        t01int4(5 , "d_change  co" , sql__ord((int ) (char ) sql__setrang(ptoc_Var77->d_change_date_time , 0 , 1)));
        curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var73->a_ap_tree , "vak641.p" , 15447))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
        if ((ptoc_Var78 = (int ) ptoc_Var77->d_datatype) != 4 && ptoc_Var78 != 24 && ((ptoc_Var78 = (int ) keep_datatype) == 14 || ptoc_Var78 == 16))
            {
            h_datatype = (unsigned char ) sql__setrang(ptoc_Var77->d_datatype , 0 , 39);
            ptoc_Var77->d_datatype = 14;
            a65_look_for_datatypes(&(*acv) , &(*dmli) , curr_n);
            if ((int ) ptoc_Var77->d_datatype == 24)
                {
                (*colin).sci_typ = 24;
                (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len * 2 + 1) , -32768 , 32767);
                if ((int ) h_datatype == 2)
                    {
                    a641string_set_operator(&(*acv) , 66 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\024');
                    }
                else
                    {
                    if ((int ) h_datatype == 3)
                        {
                        a65_set_operator(&(*acv) , 2);
                        a641string_set_operator(&(*acv) , 66 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\024');
                        }
                    }
                }
            else
                {
                ptoc_Var77->d_datatype = (unsigned char ) sql__setrang(h_datatype , 0 , 39);
                }
            }
        colin1.sci_len = 0;
        a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
        ptoc_Var77->d_concat = 0;
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var73->a_return_segm , "vak641.p" , 15485)).integer.C_4.sp1r_returncode == 0)
            {
            t01int4(5 , "atsubquery  " , sql__ord((int ) (char ) sql__setrang(ptoc_Var77->d_subquery , 0 , 1)));
            t01int4(5 , "dt_format   " , sql__ord((int ) ptoc_Var73->a_dt_format));
            t01int4(5 , "colin1.sci_t" , sql__ord((int ) colin1.sci_typ));
            t01int4(5 , "d_change    " , sql__ord((int ) (char ) sql__setrang(ptoc_Var77->d_change_date_time , 0 , 1)));
            if (1 && ((*colin).sci_cprops[0] & 2) != 0 || 1 && (colin1.sci_cprops[0] & 2) != 0)
                {
                _ptoc_MOVE(unsigned char , 2, (*colin).sci_cprops, ptoc_Var72);
                }
            if (((ptoc_Var78 = (int ) keep_datatype) == 14 || ptoc_Var78 == 16) && (int ) (*colin).sci_typ != 24 && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var75->integer.C_5.mb_st , "vak641.p" , 15499))[sql__setrang((int ) ptoc_Var76->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.etype == 55 && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var75->integer.C_5.mb_st , "vak641.p" , 15500))[sql__setrang((int ) ptoc_Var76->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_4.eop_build_in == 66 && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var75->integer.C_5.mb_st , "vak641.p" , 15501))[sql__setrang((int ) ptoc_Var76->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.ecol_tab[1] != 255)
                {
                (*(tgg00_QualBuf *) sql__nil(ptoc_Var75->integer.C_4.mb_qual , "vak641.p" , 15507)).integer.C_2.mfirst_free = (short ) sql__setrang((short ) ((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var75->integer.C_4.mb_qual , "vak641.p" , 15507)).integer.C_2.mfirst_free + -1) , -32768 , 32767);
                (*(tgg00_QualBuf *) sql__nil(ptoc_Var75->integer.C_4.mb_qual , "vak641.p" , 15508)).integer.C_2.mqual_cnt = (short ) sql__setrang((short ) ((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var75->integer.C_4.mb_qual , "vak641.p" , 15508)).integer.C_2.mqual_cnt + -1) , -32768 , 32767);
                (*colin).sci_typ = 24;
                (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len * 2 + 1) , -32768 , 32767);
                colin1.sci_typ = 24;
                colin1.sci_iolen = (short ) sql__setrang((short ) ((int ) colin1.sci_len * 2 + 1) , -32768 , 32767);
                }
            ak641check_datatype(&(*acv) , &(*colin) , &colin1 , &(*wrong_datatype));
            switch ((int ) ptoc_Var73->a_sqlmode)
                {
                case 1 :
                    max_length = 4000;
                    break;
                case 2 :
                    max_length = 8000;
                    break;
                case 3 :
                    max_length = 8000;
                    break;
                default :
                    max_length = 254;
                    break;
                }
            (*colin).sci_frac = 0;
            t01int4(5 , "c.typ       " , sql__ord((int ) (*colin).sci_typ));
            t01int4(5 , "c1.typ      " , sql__ord((int ) colin1.sci_typ));
            t01int4(5 , "c.sci_len   " , (int ) (*colin).sci_len);
            t01int4(5 , "c1.sci_len  " , (int ) colin1.sci_len);
            t01int4(5 , "c.sci_iolen " , (int ) (*colin).sci_iolen);
            t01int4(5 , "c1.sci_iolen" , (int ) colin1.sci_iolen);
            if ((int ) ptoc_Var73->a_recursive_state == 1)
                {
                max_loop_cnt = 50;
                (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) colin1.sci_iolen * 50 + (int ) (*colin).sci_iolen + -50) , -32768 , 32767);
                if ((int ) (*colin).sci_iolen > max_length + 1)
                    {
                    max_loop_cnt = (max_length - (int ) (*colin).sci_iolen + 1) / ((int ) colin1.sci_iolen + -1);
                    (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_iolen + max_loop_cnt * ((int ) colin1.sci_iolen + -1)) , -32768 , 32767);
                    }
                else
                    {
                    max_loop_cnt = 50;
                    }
                (*colin).sci_len = (short ) sql__setrang((short ) ((int ) (*colin).sci_len + max_loop_cnt * (int ) colin1.sci_len) , -32768 , 32767);
                }
            else
                {
                (*colin).sci_len = (short ) sql__setrang((short ) ((int ) (*colin).sci_len + (int ) colin1.sci_len) , -32768 , 32767);
                (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_iolen + (int ) colin1.sci_iolen + -1) , -32768 , 32767);
                }
            t01int4(5 , "cN.sci_len  " , (int ) (*colin).sci_len);
            t01int4(5 , "cN.sci_iolen" , (int ) (*colin).sci_iolen);
            if ((int ) (*colin).sci_iolen > max_length + 1)
                {
                a07_b_put_error(&(*acv) , 6374 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var73->a_ap_tree , "vak641.p" , 15572))[sql__setrang(act_node , 0 , 32767)].n_pos);
                }
            else
                {
                if ((int ) (*colin).sci_typ == 18 || (int ) (*colin).sci_typ == 24)
                    {
                    a641string_set_operator(&(*acv) , 6 , (int ) (*colin).sci_iolen , (int ) (*colin).sci_len , (unsigned char)'\002' , (unsigned char)'\0');
                    }
                else
                    {
                    a641string_set_operator(&(*acv) , 4 , (int ) (*colin).sci_iolen , (int ) (*colin).sci_len , (unsigned char)'\002' , (unsigned char)'\0');
                    }
                }
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var73->a_return_segm , "vak641.p" , 15590)).integer.C_4.sp1r_returncode == 0 && ((int ) keep_datatype == 10 || (int ) keep_datatype == 11 || (int ) keep_datatype == 13))
        {
        ptoc_Var77->d_datatype = (unsigned char ) sql__setrang(keep_datatype , 0 , 39);
        }
    cccprint("<ak641concat    ");
    }



int ak641conversion(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    int curr_n;
    tak_all_command_glob *ptoc_Var79;
    struct ptoc_Type59 *ptoc_Var80;
    tak_dml_info *ptoc_Var81;
    int ptoc_Var82;
    cccprint("AK641 ak641conve");
    ptoc_Var79 = &*acv;
    ptoc_Var80 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var79->a_ap_tree , "vak641.p" , 15614))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var81 = &*dmli;
    ptoc_Var81->d_ch_datatype = 14;
    if ((int ) ptoc_Var81->d_datatype == 14)
        {
        ptoc_Var81->d_datatype = 16;
        }
    curr_n = (int ) ptoc_Var80->n_lo_level;
    (*colin).sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) ptoc_Var81->d_datatype == 18)
        {
        *wrong_datatype = 1;
        }
    else
        {
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var79->a_return_segm , "vak641.p" , 15628)).integer.C_4.sp1r_returncode == 0)
            {
            switch ((int ) ptoc_Var80->n_symb)
                {
                case 71 :
                case 72 :
                case 164 :
                    if ((ptoc_Var82 = (int ) (*colin).sci_typ) == 2 || ptoc_Var82 == 3)
                        {
                        switch ((int ) ptoc_Var80->n_symb)
                            {
                            case 71 :
                                a65_set_operator(&(*acv) , 33);
                                break;
                            case 72 :
                                a65_set_operator(&(*acv) , 15);
                                break;
                            case 164 :
                                a65_set_operator(&(*acv) , 12);
                                break;
                            default :
                                (unsigned char ) sql__caseerr("vak641.p" , 15636);
                                break;
                            }
                        }
                    else
                        {
                        if ((int ) (*colin).sci_typ == 24)
                            {
                            switch ((int ) ptoc_Var80->n_symb)
                                {
                                case 71 :
                                    a65_set_operator(&(*acv) , 116);
                                    break;
                                case 72 :
                                    a65_set_operator(&(*acv) , 117);
                                    break;
                                case 164 :
                                    a65_set_operator(&(*acv) , 118);
                                    break;
                                default :
                                    (unsigned char ) sql__caseerr("vak641.p" , 15648);
                                    break;
                                }
                            }
                        else
                            {
                            if ((ptoc_Var82 = (int ) (*colin).sci_typ) == 10 || ptoc_Var82 == 11 || ptoc_Var82 == 13)
                                {
                                if ((int ) g01code.ctype == 0)
                                    {
                                    (*colin).sci_typ = 2;
                                    }
                                else
                                    {
                                    (*colin).sci_typ = 3;
                                    }
                                ptoc_Var81->d_datatype = (unsigned char ) sql__setrang((*colin).sci_typ , 0 , 39);
                                }
                            else
                                {
                                *wrong_datatype = 1;
                                }
                            }
                        }
                    break;
                case 20 :
                    if ((ptoc_Var82 = (int ) (*colin).sci_typ) != 2 && ptoc_Var82 != 3 && ptoc_Var82 != 24 && ptoc_Var82 != 10 && ptoc_Var82 != 11 && ptoc_Var82 != 13)
                        {
                        *wrong_datatype = 1;
                        }
                    else
                        {
                        if ((int ) (*colin).sci_typ == 24)
                            {
                            (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len + 1) , -32768 , 32767);
                            a641string_set_operator(&(*acv) , 66 , (int ) (*colin).sci_iolen , 0 , (unsigned char)'\024' , (unsigned char)'\0');
                            a65_set_operator(&(*acv) , 7);
                            }
                        else
                            {
                            if ((ptoc_Var82 = (int ) (*colin).sci_typ) == 2 || ptoc_Var82 == 10 || ptoc_Var82 == 11 || ptoc_Var82 == 13)
                                {
                                if ((int ) g01code.ctype == 0 || (int ) (*colin).sci_typ == 2)
                                    {
                                    a65_set_operator(&(*acv) , 7);
                                    }
                                }
                            }
                        ptoc_Var81->d_datatype = 3;
                        (*colin).sci_typ = 3;
                        }
                    break;
                case 19 :
                    if ((ptoc_Var82 = (int ) (*colin).sci_typ) != 2 && ptoc_Var82 != 3 && ptoc_Var82 != 24 && ptoc_Var82 != 10 && ptoc_Var82 != 11 && ptoc_Var82 != 13)
                        {
                        *wrong_datatype = 1;
                        }
                    else
                        {
                        if ((int ) (*colin).sci_typ == 24)
                            {
                            (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len + 1) , -32768 , 32767);
                            a641string_set_operator(&(*acv) , 66 , (int ) (*colin).sci_iolen , 0 , (unsigned char)'\024' , (unsigned char)'\0');
                            }
                        else
                            {
                            if ((ptoc_Var82 = (int ) (*colin).sci_typ) == 3 || ptoc_Var82 == 10 || ptoc_Var82 == 11 || ptoc_Var82 == 13)
                                {
                                if ((int ) g01code.ctype == 1 || (int ) (*colin).sci_typ == 3)
                                    {
                                    a65_set_operator(&(*acv) , 2);
                                    }
                                }
                            }
                        ptoc_Var81->d_datatype = 2;
                        (*colin).sci_typ = 2;
                        }
                    break;
                case 231 :
                    if ((ptoc_Var82 = (int ) (*colin).sci_typ) != 2 && ptoc_Var82 != 3 && ptoc_Var82 != 24 && ptoc_Var82 != 10 && ptoc_Var82 != 11 && ptoc_Var82 != 13)
                        {
                        *wrong_datatype = 1;
                        }
                    else
                        {
                        if ((int ) (*colin).sci_typ != 24)
                            {
                            if ((int ) g01code.ctype == 1 || (int ) (*colin).sci_typ == 3)
                                {
                                a65_set_operator(&(*acv) , 2);
                                }
                            (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len * 2 + 1) , -32768 , 32767);
                            a641string_set_operator(&(*acv) , 66 , (int ) (*colin).sci_iolen , 0 , (unsigned char)'\0' , (unsigned char)'\024');
                            }
                        ptoc_Var81->d_datatype = 24;
                        (*colin).sci_typ = 24;
                        }
                    break;
                default :
                    (unsigned char ) sql__caseerr("vak641.p" , 15631);
                    break;
                }
            }
        }
    cccprint("<ak641conversion");
    }



int ak641datediff(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    int curr_n;
    tak00_scolinf colin1;
    tak_all_command_glob *ptoc_Var83;
    tak_dml_info *ptoc_Var84;
    tak00_scolinf *ptoc_Var85;
    cccprint("AK641 ak641dated");
    ptoc_Var83 = &*acv;
    ptoc_Var84 = &*dmli;
    curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var83->a_ap_tree , "vak641.p" , 15798))[sql__setrang(act_node , 0 , 32767)].n_lo_level;
    ak641type_or_timestamp_parameter(&(*acv) , &(*dmli) , &(*colin) , &curr_n , 10);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var83->a_return_segm , "vak641.p" , 15800)).integer.C_4.sp1r_returncode == 0)
        {
        curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var83->a_ap_tree , "vak641.p" , 15803))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
        ak641not_first_type_or_timestamp_parameter(&(*acv) , &(*dmli) , &colin1 , &curr_n , 10);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var83->a_return_segm , "vak641.p" , 15808)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var84->d_datatype = 15;
        ptoc_Var85 = &*colin;
        ptoc_Var85->sci_len = 7;
        ptoc_Var85->sci_iolen = (short ) sql__setrang((short ) (((int ) ptoc_Var85->sci_len + 1) / 2 + 2) , -32768 , 32767);
        ptoc_Var85->sci_frac = 0;
        ptoc_Var85->sci_typ = 0;
        a65_set_operator(&(*acv) , 52);
        }
    cccprint("<ak641datediff  ");
    }



int ak641datetime(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    int curr_n;
    int pos;
    int off_set;
    int func;
    tsp_data_type wanted_type;
    tak_all_command_glob *ptoc_Var86;
    struct ptoc_Type59 *ptoc_Var87;
    tak_dml_info *ptoc_Var88;
    struct tgg00_MessBlock *ptoc_Var89;
    tgg00_QualBuf *ptoc_Var90;
    int ptoc_Var91;
    tak00_scolinf *ptoc_Var92;
    tgg00_StackEntry *ptoc_Var93;
    cccprint("AK641 ak641datet");
    ptoc_Var86 = &*acv;
    ptoc_Var87 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak641.p" , 15844))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var88 = &*dmli;
    ptoc_Var89 = &ptoc_Var86->a_mblock;
    ptoc_Var90 = (tgg00_QualBuf *) sql__nil(ptoc_Var89->integer.C_4.mb_qual , "vak641.p" , 15845);
    if ((ptoc_Var91 = (int ) ptoc_Var88->d_datatype) != 14 && ptoc_Var91 != 15)
        {
        *wrong_datatype = 1;
        }
    else
        {
        switch ((int ) ptoc_Var87->n_symb)
            {
            case 125 :
            case 132 :
            case 135 :
                wanted_type = 10;
                break;
            case 129 :
            case 131 :
            case 133 :
                wanted_type = 11;
                break;
            case 130 :
                wanted_type = 13;
                break;
            default :
                (unsigned char ) sql__caseerr("vak641.p" , 15851);
                break;
            }
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak641.p" , 15860))[sql__setrang((int ) ptoc_Var87->n_lo_level , 0 , 32767)].n_symb == 50)
            {
            ptoc_Var88->d_datatype = (unsigned char ) sql__setrang(wanted_type , 0 , 39);
            }
        else
            {
            ptoc_Var88->d_datatype = 14;
            }
        curr_n = (int ) ptoc_Var87->n_lo_level;
        (*colin).sci_len = 0;
        a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var86->a_return_segm , "vak641.p" , 15869)).integer.C_4.sp1r_returncode == 21000)
            {
            (*(tsp1_segment *) sql__nil(ptoc_Var86->a_return_segm , "vak641.p" , 15872)).integer.C_4.sp1r_returncode = 0;
            ptoc_Var88->d_datatype = (unsigned char ) sql__setrang(wanted_type , 0 , 39);
            a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var86->a_return_segm , "vak641.p" , 15877)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var92 = &*colin;
            t01int4(5 , "DM_DATATYPE " , sql__ord((int ) ptoc_Var88->d_datatype));
            if ((int ) (*colin).sci_typ == 2 || (int ) (*colin).sci_typ == 3 || (int ) (*colin).sci_typ == 24)
                {
                a641check_datetime(&(*acv) , &(*dmli) , (unsigned char ) sql__setrang(wanted_type , 0 , 39));
                ptoc_Var88->d_datatype = (unsigned char ) sql__setrang(wanted_type , 0 , 39);
                }
            ptoc_Var92->sci_len = 0;
            pos = 0;
            off_set = 0;
            func = 0;
            switch ((int ) ptoc_Var87->n_symb)
                {
                case 125 :
                    if (((ptoc_Var91 = (int ) ptoc_Var88->d_datatype) != 15 && ptoc_Var91 != 10 && ptoc_Var91 != 13 || (int ) ptoc_Var86->a_sqlmode != 1) && ((ptoc_Var91 = (int ) ptoc_Var88->d_datatype) != 10 && ptoc_Var91 != 13 || (int ) ptoc_Var86->a_sqlmode != 3))
                        {
                        *wrong_datatype = 1;
                        }
                    else
                        {
                        ptoc_Var92->sci_len = 2;
                        pos = 7;
                        func = 2;
                        }
                    break;
                case 129 :
                    if (((ptoc_Var91 = (int ) ptoc_Var88->d_datatype) != 15 && ptoc_Var91 != 11 && ptoc_Var91 != 13 || (int ) ptoc_Var86->a_sqlmode != 1) && ((ptoc_Var91 = (int ) ptoc_Var88->d_datatype) != 11 && ptoc_Var91 != 13 || (int ) ptoc_Var86->a_sqlmode != 3))
                        {
                        *wrong_datatype = 1;
                        }
                    else
                        {
                        pos = 1;
                        func = 3;
                        if ((int ) ptoc_Var88->d_datatype == 13)
                            {
                            ptoc_Var92->sci_len = 2;
                            off_set = 8;
                            }
                        else
                            {
                            ptoc_Var92->sci_len = 4;
                            }
                        }
                    break;
                case 131 :
                    if (((ptoc_Var91 = (int ) ptoc_Var88->d_datatype) != 15 && ptoc_Var91 != 11 && ptoc_Var91 != 13 || (int ) ptoc_Var86->a_sqlmode != 1) && ((ptoc_Var91 = (int ) ptoc_Var88->d_datatype) != 11 && ptoc_Var91 != 13 || (int ) ptoc_Var86->a_sqlmode != 3))
                        {
                        *wrong_datatype = 1;
                        }
                    else
                        {
                        ptoc_Var92->sci_len = 2;
                        pos = 5;
                        func = 4;
                        if ((int ) ptoc_Var88->d_datatype == 13)
                            {
                            off_set = 6;
                            }
                        }
                    break;
                case 130 :
                    if ((int ) ptoc_Var88->d_datatype != 13)
                        {
                        *wrong_datatype = 1;
                        }
                    else
                        {
                        ptoc_Var92->sci_len = 6;
                        pos = 15;
                        func = 6;
                        }
                    break;
                case 132 :
                    if (((ptoc_Var91 = (int ) ptoc_Var88->d_datatype) != 15 && ptoc_Var91 != 10 && ptoc_Var91 != 13 || (int ) ptoc_Var86->a_sqlmode != 1) && ((ptoc_Var91 = (int ) ptoc_Var88->d_datatype) != 10 && ptoc_Var91 != 13 || (int ) ptoc_Var86->a_sqlmode != 3))
                        {
                        *wrong_datatype = 1;
                        }
                    else
                        {
                        ptoc_Var92->sci_len = 2;
                        pos = 5;
                        func = 1;
                        }
                    break;
                case 133 :
                    if (((ptoc_Var91 = (int ) ptoc_Var88->d_datatype) != 15 && ptoc_Var91 != 11 && ptoc_Var91 != 13 || (int ) ptoc_Var86->a_sqlmode != 1) && ((ptoc_Var91 = (int ) ptoc_Var88->d_datatype) != 11 && ptoc_Var91 != 13 || (int ) ptoc_Var86->a_sqlmode != 3))
                        {
                        *wrong_datatype = 1;
                        }
                    else
                        {
                        ptoc_Var92->sci_len = 2;
                        pos = 7;
                        func = 5;
                        if ((int ) ptoc_Var88->d_datatype == 13)
                            {
                            off_set = 6;
                            }
                        }
                    break;
                case 135 :
                    if (((ptoc_Var91 = (int ) ptoc_Var88->d_datatype) != 15 && ptoc_Var91 != 10 && ptoc_Var91 != 13 || (int ) ptoc_Var86->a_sqlmode != 1) && ((ptoc_Var91 = (int ) ptoc_Var88->d_datatype) != 10 && ptoc_Var91 != 13 || (int ) ptoc_Var86->a_sqlmode != 3))
                        {
                        *wrong_datatype = 1;
                        }
                    else
                        {
                        ptoc_Var92->sci_len = 4;
                        pos = 1;
                        func = 0;
                        }
                    break;
                default :
                    (unsigned char ) sql__caseerr("vak641.p" , 15896);
                    break;
                }
            ptoc_Var92->sci_frac = 0;
            if ((int ) ptoc_Var87->n_symb == 135 || (int ) ptoc_Var87->n_symb == 129 && (int ) ptoc_Var88->d_datatype != 13)
                {
                ptoc_Var92->sci_iolen = 4;
                }
            else
                {
                if ((int ) ptoc_Var87->n_symb == 130)
                    {
                    ptoc_Var92->sci_iolen = 5;
                    }
                else
                    {
                    ptoc_Var92->sci_iolen = 3;
                    }
                }
            ptoc_Var92->sci_typ = 0;
            if ((int ) ptoc_Var90->integer.C_2.mfirst_free >= (int ) ptoc_Var89->integer.C_2.mb_st_max)
                {
                a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var89->integer.C_2.mb_st_max);
                }
            else
                {
                ptoc_Var93 = &(*(tgg00_StackList *) sql__nil(ptoc_Var89->integer.C_5.mb_st , "vak641.p" , 16042))[sql__setrang((int ) ptoc_Var90->integer.C_2.mfirst_free , 1 , 32767) + -1];
                ptoc_Var93->integer.C_1.etype = 55;
                ptoc_Var93->integer.C_4.eop_build_in = 5;
                ptoc_Var93->integer.C_1.elen_var = (short ) sql__setrang((*colin).sci_len , -32768 , 32767);
                ptoc_Var93->integer.C_1.epos = (short ) sql__setrang((short ) (pos + off_set) , -32768 , 32767);
                ptoc_Var93->integer.C_1.ecol_tab[0] = (unsigned char ) sql__setrang((unsigned char ) func , 0 , 255);
                ptoc_Var93->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                ptoc_Var90->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var90->integer.C_2.mfirst_free , -32768 , 32767 , "vak641.p" , 16050) , -32768 , 32767);
                ptoc_Var90->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var90->integer.C_2.mqual_cnt , -32768 , 32767 , "vak641.p" , 16051) , -32768 , 32767);
                }
            }
        ptoc_Var88->d_datatype = 15;
        }
    cccprint("<ak641datetime  ");
    }



int ak641days(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    int curr_n;
    tak_all_command_glob *ptoc_Var94;
    struct ptoc_Type59 *ptoc_Var95;
    tak_dml_info *ptoc_Var96;
    int ptoc_Var97;
    tak00_scolinf *ptoc_Var98;
    cccprint("AK641 ak641days ");
    ptoc_Var94 = &*acv;
    ptoc_Var95 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var94->a_ap_tree , "vak641.p" , 16079))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var96 = &*dmli;
    ptoc_Var96->d_datatype = 14;
    curr_n = (int ) ptoc_Var95->n_lo_level;
    (*colin).sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var94->a_return_segm , "vak641.p" , 16085)).integer.C_4.sp1r_returncode == 21000)
        {
        (*(tsp1_segment *) sql__nil(ptoc_Var94->a_return_segm , "vak641.p" , 16089)).integer.C_4.sp1r_returncode = 0;
        a07_b_put_error(&(*acv) , 6556 , ptoc_Var95->n_pos);
        }
    else
        {
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var94->a_return_segm , "vak641.p" , 16093)).integer.C_4.sp1r_returncode == 0)
            {
            if ((ptoc_Var97 = (int ) (*colin).sci_typ) != 10 && ptoc_Var97 != 13 && ptoc_Var97 != 2 && ptoc_Var97 != 3 && ptoc_Var97 != 24)
                {
                *wrong_datatype = 1;
                }
            else
                {
                if ((int ) ptoc_Var96->d_datatype == 24)
                    {
                    (*colin).sci_typ = 2;
                    (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len + 1) , -32768 , 32767);
                    a641string_set_operator(&(*acv) , 66 , (int ) (*colin).sci_iolen , 0 , (unsigned char)'\024' , (unsigned char)'\0');
                    }
                if ((ptoc_Var97 = (int ) (*colin).sci_typ) != 10 && ptoc_Var97 != 13)
                    {
                    a641check_datetime(&(*acv) , &(*dmli) , 10);
                    }
                ptoc_Var98 = &*colin;
                ptoc_Var98->sci_len = 7;
                ptoc_Var96->d_datatype = 15;
                ptoc_Var98->sci_frac = 0;
                ptoc_Var98->sci_typ = 0;
                ptoc_Var98->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var98->sci_len + -1) , -32768 , 32767);
                a65_set_operator(&(*acv) , 55);
                }
            }
        }
    cccprint("<ak641days      ");
    }



int ak641digits(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    int curr_n;
    tak_all_command_glob *ptoc_Var99;
    struct ptoc_Type59 *ptoc_Var100;
    tak_dml_info *ptoc_Var101;
    struct tgg00_MessBlock *ptoc_Var102;
    tgg00_QualBuf *ptoc_Var103;
    tgg00_StackEntry *ptoc_Var104;
    cccprint("AK641 ak641digit");
    ptoc_Var99 = &*acv;
    ptoc_Var100 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var99->a_ap_tree , "vak641.p" , 16146))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var101 = &*dmli;
    ptoc_Var101->d_datatype = 15;
    (*colin).sci_len = 0;
    curr_n = (int ) ptoc_Var100->n_lo_level;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var99->a_return_segm , "vak641.p" , 16152)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) g01code.ctype == 1)
            {
            (*colin).sci_typ = 3;
            ptoc_Var101->d_datatype = 3;
            }
        else
            {
            (*colin).sci_typ = 2;
            ptoc_Var101->d_datatype = 2;
            }
        (*colin).sci_frac = 0;
        (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len + 1) , -32768 , 32767);
        ptoc_Var102 = &ptoc_Var99->a_mblock;
        ptoc_Var103 = (tgg00_QualBuf *) sql__nil(ptoc_Var102->integer.C_4.mb_qual , "vak641.p" , 16169);
        if ((int ) ptoc_Var103->integer.C_2.mfirst_free > (int ) ptoc_Var102->integer.C_2.mb_st_max)
            {
            a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var102->integer.C_2.mb_st_max);
            }
        else
            {
            ptoc_Var104 = &(*(tgg00_StackList *) sql__nil(ptoc_Var102->integer.C_5.mb_st , "vak641.p" , 16174))[sql__setrang((int ) ptoc_Var103->integer.C_2.mfirst_free , 1 , 32767) + -1];
            ptoc_Var104->integer.C_1.etype = 26;
            ptoc_Var104->integer.C_1.eop = 6;
            ptoc_Var104->integer.C_1.epos = 0;
            ptoc_Var104->integer.C_1.elen_var = (short ) sql__setrang((*colin).sci_len , -32768 , 32767);
            if ((int ) (*colin).sci_frac > 0)
                {
                ptoc_Var104->integer.C_1.ecol_tab[0] = (unsigned char ) sql__setrang((unsigned char ) (*colin).sci_frac , 0 , 255);
                }
            else
                {
                ptoc_Var104->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                }
            ptoc_Var104->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
            ptoc_Var103->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var103->integer.C_2.mfirst_free , -32768 , 32767 , "vak641.p" , 16187) , -32768 , 32767);
            ptoc_Var103->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var103->integer.C_2.mqual_cnt , -32768 , 32767 , "vak641.p" , 16188) , -32768 , 32767);
            }
        }
    cccprint("<ak641digits    ");
    }



int ak641expand(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    tsp_data_type keep_datatype;
    int curr_n;
    tsp_int2 first_int;
    tak_all_command_glob *ptoc_Var105;
    struct ptoc_Type59 *ptoc_Var106;
    tak_dml_info *ptoc_Var107;
    tak00_scolinf *ptoc_Var108;
    int ptoc_Var109;
    cccprint("AK641 ak641expan");
    ptoc_Var105 = &*acv;
    ptoc_Var106 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var105->a_ap_tree , "vak641.p" , 16214))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var107 = &*dmli;
    ptoc_Var108 = &*colin;
    if ((int ) ptoc_Var107->d_datatype == 14)
        {
        ptoc_Var107->d_datatype = 16;
        }
    curr_n = (int ) ptoc_Var106->n_lo_level;
    ptoc_Var108->sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) ptoc_Var107->d_datatype == 18)
        {
        *wrong_datatype = 1;
        }
    else
        {
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var105->a_return_segm , "vak641.p" , 16227)).integer.C_4.sp1r_returncode == 0)
            {
            keep_datatype = (unsigned char ) sql__setrang(ptoc_Var107->d_datatype , 0 , 39);
            ptoc_Var107->d_datatype = 15;
            curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var105->a_ap_tree , "vak641.p" , 16232))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var105->a_ap_tree , "vak641.p" , 16233))[sql__setrang(curr_n , 0 , 32767)].n_symb != 50)
                {
                a05_unsigned_int2_get(&(*acv) , (*(tak_ap_max_tree *) sql__nil(ptoc_Var105->a_ap_tree , "vak641.p" , 16237))[sql__setrang(curr_n , 0 , 32767)].n_pos , (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var105->a_ap_tree , "vak641.p" , 16238))[sql__setrang(curr_n , 0 , 32767)].n_length , -32768 , 32767) , 6296 , &first_int);
                if ((int ) first_int <= (int ) ptoc_Var108->sci_len)
                    {
                    a07_b_put_error(&(*acv) , 6125 , ptoc_Var106->n_pos);
                    }
                else
                    {
                    if ((int ) first_int > 8000 || (int ) first_int * 2 > 8000 && (int ) keep_datatype == 24)
                        {
                        a07_b_put_error(&(*acv) , 6296 , ptoc_Var106->n_pos);
                        }
                    }
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var105->a_return_segm , "vak641.p" , 16253)).integer.C_4.sp1r_returncode == 0)
                    {
                    if ((int ) keep_datatype == 24)
                        {
                        ptoc_Var108->sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) first_int * 2 , -2147483647 , 2147483647 , "vak641.p" , 16258) , -32768 , 32767);
                        }
                    else
                        {
                        ptoc_Var108->sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) first_int , -32768 , 32767 , "vak641.p" , 16260) , -32768 , 32767);
                        }
                    if ((char ) sql__setrang(ptoc_Var107->d_concat , 0 , 1) || (char ) sql__setrang(ptoc_Var107->d_view , 0 , 1))
                        {
                        a641string_set_operator(&(*acv) , 8 , (int ) ptoc_Var108->sci_iolen , (int ) ptoc_Var108->sci_len , (unsigned char)'\0' , (unsigned char)'\0');
                        }
                    ptoc_Var107->d_expand = (short ) sql__setrang(first_int , -32768 , 32767);
                    ptoc_Var108->sci_frac = 0;
                    ptoc_Var108->sci_len = (short ) sql__setrang(first_int , -32768 , 32767);
                    }
                }
            else
                {
                a641string_set_operator(&(*acv) , 8 , 1 , 0 , (unsigned char)'\377' , (unsigned char)'\377');
                ptoc_Var107->d_expand = -2;
                ptoc_Var108->sci_frac = 0;
                ptoc_Var108->sci_len = 0;
                ptoc_Var108->sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var108->sci_len , -32768 , 32767 , "vak641.p" , 16281) , -32768 , 32767);
                }
            if ((ptoc_Var109 = (int ) keep_datatype) == 10 || ptoc_Var109 == 11 || ptoc_Var109 == 13)
                {
                if ((int ) g01code.ctype == 0)
                    {
                    ptoc_Var108->sci_typ = 2;
                    }
                else
                    {
                    ptoc_Var108->sci_typ = 3;
                    }
                ptoc_Var107->d_datatype = (unsigned char ) sql__setrang(ptoc_Var108->sci_typ , 0 , 39);
                }
            else
                {
                ptoc_Var107->d_datatype = (unsigned char ) sql__setrang(keep_datatype , 0 , 39);
                }
            }
        }
    cccprint("<ak641expand    ");
    }



int ak641space(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    int curr_n;
    tsp_int2 first_int;
    tak_all_command_glob *ptoc_Var110;
    struct ptoc_Type59 *ptoc_Var111;
    tak_dml_info *ptoc_Var112;
    tak00_scolinf *ptoc_Var113;
    int ptoc_Var114;
    cccprint("AK641 ak641space");
    ptoc_Var110 = &*acv;
    ptoc_Var111 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var110->a_ap_tree , "vak641.p" , 16319))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var112 = &*dmli;
    ptoc_Var113 = &*colin;
    curr_n = (int ) ptoc_Var111->n_sa_level;
    a05_unsigned_int2_get(&(*acv) , (*(tak_ap_max_tree *) sql__nil(ptoc_Var110->a_ap_tree , "vak641.p" , 16322))[sql__setrang(curr_n , 0 , 32767)].n_pos , (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var110->a_ap_tree , "vak641.p" , 16323))[sql__setrang(curr_n , 0 , 32767)].n_length , -32768 , 32767) , 6296 , &first_int);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var110->a_return_segm , "vak641.p" , 16324)).integer.C_4.sp1r_returncode == 0)
        {
        if ((ptoc_Var114 = (int ) ptoc_Var112->d_datatype) != 3 && ptoc_Var114 != 2 && ptoc_Var114 != 24 && ptoc_Var114 != 14)
            {
            *wrong_datatype = 1;
            }
        else
            {
            if ((int ) first_int <= 0 || (int ) first_int > 8000 || (int ) first_int * 2 > 8000 && (int ) ptoc_Var112->d_datatype == 24)
                {
                a07_b_put_error(&(*acv) , 6296 , ptoc_Var111->n_pos);
                }
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var110->a_return_segm , "vak641.p" , 16341)).integer.C_4.sp1r_returncode == 0)
                {
                if ((int ) ptoc_Var112->d_datatype == 14)
                    {
                    if ((char ) sql__setrang(g01unicode , 0 , 1))
                        {
                        ptoc_Var112->d_datatype = 24;
                        }
                    else
                        {
                        if ((int ) g01code.ctype == 1)
                            {
                            ptoc_Var112->d_datatype = 3;
                            }
                        else
                            {
                            ptoc_Var112->d_datatype = 2;
                            }
                        }
                    }
                ak641push_blank(&(*acv) , (unsigned char ) sql__setrang(ptoc_Var112->d_datatype , 0 , 39));
                ptoc_Var113->sci_typ = (unsigned char ) sql__setrang(ptoc_Var112->d_datatype , 0 , 39);
                if ((int ) ptoc_Var113->sci_typ == 24)
                    {
                    ptoc_Var113->sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) first_int * 2 , -2147483647 , 2147483647 , "vak641.p" , 16362) , -32768 , 32767);
                    }
                else
                    {
                    ptoc_Var113->sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) first_int , -32768 , 32767 , "vak641.p" , 16364) , -32768 , 32767);
                    }
                if ((char ) sql__setrang(ptoc_Var112->d_concat , 0 , 1) || (char ) sql__setrang(ptoc_Var112->d_view , 0 , 1))
                    {
                    a641string_set_operator(&(*acv) , 8 , (int ) ptoc_Var113->sci_iolen , 1 , (unsigned char)'\0' , (unsigned char)'\0');
                    }
                ptoc_Var113->sci_frac = 0;
                ptoc_Var113->sci_len = (short ) sql__setrang(first_int , -32768 , 32767);
                }
            }
        }
    cccprint("<ak641space     ");
    }

static
  tak00_colpropset ptoc_Var115 = 
  {
  (unsigned char)'\002', (unsigned char)'\0'
  };


int ak641fixed_float(acv, dmli, colin, symb, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
tak_sc_symbol symb;
int act_node;
    {
    int curr_n;
    tsp_int2 first_int;
    tsp_int2 second_int;
    tak_all_command_glob *ptoc_Var116;
    struct ptoc_Type59 *ptoc_Var117;
    tak_dml_info *ptoc_Var118;
    tak00_scolinf *ptoc_Var119;
    cccprint("AK641 ak641fixed");
    ptoc_Var116 = &*acv;
    ptoc_Var117 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var116->a_ap_tree , "vak641.p" , 16399))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var118 = &*dmli;
    ptoc_Var118->d_datatype = 15;
    curr_n = (int ) ptoc_Var117->n_lo_level;
    (*colin).sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var116->a_return_segm , "vak641.p" , 16405)).integer.C_4.sp1r_returncode == 0)
        {
        ak641put_maxlength(&(*acv) , &curr_n , (unsigned char ) sql__setrang(symb , 0 , 240) , &first_int , &second_int);
        ptoc_Var119 = &*colin;
        ptoc_Var119->sci_len = (short ) sql__setrang(first_int , -32768 , 32767);
        ptoc_Var119->sci_frac = (short ) sql__setrang(second_int , -32768 , 32767);
        ptoc_Var119->sci_iolen = (short ) sql__setrang((short ) (((int ) ptoc_Var119->sci_len + 1) / 2 + 2) , -32768 , 32767);
        if ((int ) symb == 33)
            {
            ptoc_Var119->sci_typ = 0;
            }
        else
            {
            ptoc_Var119->sci_typ = 12;
            }
        _ptoc_MOVE(unsigned char , 2, ptoc_Var119->sci_cprops, ptoc_Var115);
        }
    cccprint("<ak641fixed_floa");
    }



int ak641float(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    int curr_n;
    tak_all_command_glob *ptoc_Var120;
    struct ptoc_Type59 *ptoc_Var121;
    tak_dml_info *ptoc_Var122;
    tak00_scolinf *ptoc_Var123;
    cccprint("AK641 ak641float");
    ptoc_Var120 = &*acv;
    ptoc_Var121 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var120->a_ap_tree , "vak641.p" , 16444))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var122 = &*dmli;
    (*colin).sci_len = 0;
    curr_n = (int ) ptoc_Var121->n_lo_level;
    ptoc_Var122->d_datatype = 15;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var120->a_return_segm , "vak641.p" , 16450)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var123 = &*colin;
        ptoc_Var123->sci_frac = -1;
        ptoc_Var123->sci_typ = 1;
        ptoc_Var122->d_datatype = 15;
        }
    cccprint("<ak641float     ");
    }



int a641_get_length(colin, output_length, wrong_datatype)
tak00_scolinf *colin;
tsp_int2 *output_length;
char *wrong_datatype;
    {
    tak00_scolinf *ptoc_Var124;
    cccprint("AK641 a641_get_l");
    *wrong_datatype = 0;
    ptoc_Var124 = &*colin;
    switch ((int ) ptoc_Var124->sci_typ)
        {
        case 2 :
        case 3 :
        case 10 :
        case 11 :
        case 13 :
        case 24 :
            *output_length = (short ) sql__setrang(ptoc_Var124->sci_len , -32768 , 32767);
            break;
        case 1 :
        case 12 :
            if ((int ) ptoc_Var124->sci_len == 1)
                {
                *output_length = 6;
                }
            else
                {
                *output_length = (short ) sql__setrang((short ) ((int ) ptoc_Var124->sci_len + 6) , -32768 , 32767);
                }
            break;
        case 0 :
            if ((int ) ptoc_Var124->sci_len == (int ) ptoc_Var124->sci_frac)
                {
                *output_length = (short ) sql__setrang((short ) ((int ) ptoc_Var124->sci_len + 3) , -32768 , 32767);
                }
            else
                {
                if ((int ) ptoc_Var124->sci_frac != 0)
                    {
                    *output_length = (short ) sql__setrang((short ) ((int ) ptoc_Var124->sci_len + 2) , -32768 , 32767);
                    }
                else
                    {
                    *output_length = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var124->sci_len , -32768 , 32767 , "vak641.p" , 16495) , -32768 , 32767);
                    }
                }
            break;
        default :
            *wrong_datatype = 1;
            *output_length = 0;
            break;
        }
    cccprint("<a641_get_length");
    }



int ak641_hextoraw(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    int curr_n;
    tak_all_command_glob *ptoc_Var125;
    struct ptoc_Type59 *ptoc_Var126;
    tak_dml_info *ptoc_Var127;
    int ptoc_Var128;
    cccprint("AK641 ak641_hext");
    ptoc_Var125 = &*acv;
    ptoc_Var126 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var125->a_ap_tree , "vak641.p" , 16521))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var127 = &*dmli;
    ptoc_Var127->d_datatype = 14;
    (*colin).sci_len = 0;
    curr_n = (int ) ptoc_Var126->n_lo_level;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var125->a_return_segm , "vak641.p" , 16527)).integer.C_4.sp1r_returncode == 0)
        {
        if ((ptoc_Var128 = (int ) ptoc_Var127->d_datatype) != 2 && ptoc_Var128 != 3 && ptoc_Var128 != 24)
            {
            *wrong_datatype = 1;
            }
        else
            {
            if ((int ) ptoc_Var127->d_datatype == 24)
                {
                a641string_set_operator(&(*acv) , 66 , (int ) (*colin).sci_iolen , 0 , (unsigned char)'\024' , (unsigned char)'\0');
                }
            (*colin).sci_len = (short ) sql__setrang((short ) (((int ) (*colin).sci_len + 1) / 2) , -32768 , 32767);
            a65_set_operator(&(*acv) , 36);
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var125->a_return_segm , "vak641.p" , 16542)).integer.C_4.sp1r_returncode == 0)
                {
                ptoc_Var127->d_datatype = 4;
                (*colin).sci_typ = 4;
                (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len + 1) , -32768 , 32767);
                (*colin).sci_frac = 0;
                }
            }
        }
    cccprint("<ak641_hextoraw ");
    }



int ak641hex(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    int curr_n;
    int max_length;
    tak_all_command_glob *ptoc_Var129;
    struct ptoc_Type59 *ptoc_Var130;
    tak_dml_info *ptoc_Var131;
    int ptoc_Var132;
    cccprint("AK641 ak641hex  ");
    ptoc_Var129 = &*acv;
    ptoc_Var130 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak641.p" , 16572))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var131 = &*dmli;
    ptoc_Var131->d_hex = 1;
    ptoc_Var131->d_datatype = 14;
    (*colin).sci_len = 0;
    curr_n = (int ) ptoc_Var130->n_lo_level;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) ptoc_Var129->a_sqlmode == 2 && ((ptoc_Var132 = (int ) ptoc_Var131->d_datatype) == 10 || ptoc_Var132 == 11 || ptoc_Var132 == 13))
        {
        *wrong_datatype = 1;
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var129->a_return_segm , "vak641.p" , 16584)).integer.C_4.sp1r_returncode == 21000)
        {
        (*(tsp1_segment *) sql__nil(ptoc_Var129->a_return_segm , "vak641.p" , 16587)).integer.C_4.sp1r_returncode = 0;
        ptoc_Var131->d_datatype = 16;
        a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var129->a_return_segm , "vak641.p" , 16592)).integer.C_4.sp1r_returncode == 0)
        {
        switch ((int ) ptoc_Var129->a_sqlmode)
            {
            case 1 :
                max_length = 4000;
                break;
            case 2 :
                max_length = 8000;
                break;
            case 3 :
                max_length = 8000;
                break;
            default :
                max_length = 254;
                break;
            }
        if ((ptoc_Var132 = (int ) (*colin).sci_typ) == 2 || ptoc_Var132 == 3 || ptoc_Var132 == 4 || ptoc_Var132 == 10 || ptoc_Var132 == 11 || ptoc_Var132 == 13)
            {
            (*colin).sci_len = (short ) sql__setrang((short ) ((int ) (*colin).sci_len * 2) , -32768 , 32767);
            }
        else
            {
            (*colin).sci_len = (short ) sql__setrang((short ) ((int ) (*colin).sci_iolen * 2 + -2) , -32768 , 32767);
            }
        (*colin).sci_frac = 0;
        (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len + 1) , -32768 , 32767);
        if ((int ) (*colin).sci_iolen > max_length)
            {
            a07_b_put_error(&(*acv) , 6374 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak641.p" , 16619))[sql__setrang(act_node , 0 , 32767)].n_pos);
            }
        if ((int ) g01code.ctype == 1)
            {
            ptoc_Var131->d_datatype = 3;
            (*colin).sci_typ = 3;
            }
        else
            {
            ptoc_Var131->d_datatype = 2;
            (*colin).sci_typ = 2;
            }
        a65_set_operator(&(*acv) , 10);
        }
    ptoc_Var131->d_hex = 0;
    cccprint("<ak641hex       ");
    }



int ak641index(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    char index_from_null;
    int curr_n;
    int index;
    int param_cnt;
    int old_n;
    tsp_int2 start;
    tsp_num_error n_err;
    tak00_scolinf colin1;
    tak00_scolinf colin2;
    tak_all_command_glob *ptoc_Var133;
    struct ptoc_Type59 *ptoc_Var134;
    tak_dml_info *ptoc_Var135;
    struct tgg00_MessBlock *ptoc_Var136;
    tgg00_QualBuf *ptoc_Var137;
    tak00_scolinf *ptoc_Var138;
    cccprint("AK641 ak641index");
    ptoc_Var133 = &*acv;
    ptoc_Var134 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var133->a_ap_tree , "vak641.p" , 16662))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var135 = &*dmli;
    param_cnt = 2;
    ptoc_Var135->d_datatype = 16;
    curr_n = (int ) ptoc_Var134->n_lo_level;
    (*colin).sci_len = 0;
    index_from_null = (char ) sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var133->a_ap_tree , "vak641.p" , 16668))[sql__setrang(curr_n , 0 , 32767)].n_proc == 0 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var133->a_ap_tree , "vak641.p" , 16669))[sql__setrang(curr_n , 0 , 32767)].n_symb == 50 , 0 , 1);
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) ptoc_Var135->d_datatype == 18)
        {
        *wrong_datatype = 1;
        }
    else
        {
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var133->a_return_segm , "vak641.p" , 16675)).integer.C_4.sp1r_returncode == 0)
            {
            curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var133->a_ap_tree , "vak641.p" , 16678))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
            colin1.sci_len = 0;
            if ((int ) ptoc_Var135->d_datatype == 10 || (int ) ptoc_Var135->d_datatype == 11 || (int ) ptoc_Var135->d_datatype == 13)
                {
                if ((int ) g01code.ctype == 0)
                    {
                    ptoc_Var135->d_datatype = 2;
                    }
                else
                    {
                    ptoc_Var135->d_datatype = 3;
                    }
                }
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var133->a_ap_tree , "vak641.p" , 16691))[sql__setrang(curr_n , 0 , 32767)].n_symb == 46)
                {
                a07_b_put_error(&(*acv) , 6060 , ptoc_Var134->n_pos);
                }
            else
                {
                a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var133->a_return_segm , "vak641.p" , 16697)).integer.C_4.sp1r_returncode == 0)
                    {
                    ak641check_datatype(&(*acv) , &(*colin) , &colin1 , &(*wrong_datatype));
                    if (!(char ) sql__setrang(*wrong_datatype , 0 , 1))
                        {
                        ptoc_Var135->d_datatype = 15;
                        curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var133->a_ap_tree , "vak641.p" , 16706))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
                        old_n = curr_n;
                        if (curr_n != 0)
                            {
                            param_cnt = sql__succ(param_cnt , -2147483647 , 2147483647 , "vak641.p" , 16711);
                            colin1.sci_len = 0;
                            a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
                            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var133->a_ap_tree , "vak641.p" , 16714))[sql__setrang(old_n , 0 , 32767)].n_symb == 49)
                                {
                                ptoc_Var136 = &ptoc_Var133->a_mblock;
                                ptoc_Var137 = (tgg00_QualBuf *) sql__nil(ptoc_Var136->integer.C_4.mb_qual , "vak641.p" , 16716);
                                index = (int ) ptoc_Var137->integer.C_2.mfirst_free + -1;
                                s40gsint((*(tgg00_DataPart *) sql__nil(ptoc_Var133->a_mblock.integer.C_3.mb_data , "vak641.p" , 16719)).integer.C_1.mbp_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var136->integer.C_5.mb_st , "vak641.p" , 16720))[sql__setrang(index , 1 , 32767) + -1].integer.C_1.epos + 1 , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var136->integer.C_5.mb_st , "vak641.p" , 16721))[sql__setrang(index , 1 , 32767) + -1].integer.C_1.elen_var * 2 + -4 , &start , &n_err);
                                if ((int ) n_err == 0)
                                    {
                                    if ((int ) start < 1 || (int ) start > (int ) (*colin).sci_len && !(char ) sql__setrang(index_from_null , 0 , 1))
                                        {
                                        a07_b_put_error(&(*acv) , 6290 , ptoc_Var134->n_pos);
                                        }
                                    }
                                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var133->a_return_segm , "vak641.p" , 16735)).integer.C_4.sp1r_returncode == 0)
                                    {
                                    curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var133->a_ap_tree , "vak641.p" , 16739))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
                                    if (curr_n != 0)
                                        {
                                        param_cnt = sql__succ(param_cnt , -2147483647 , 2147483647 , "vak641.p" , 16743);
                                        colin2.sci_len = 0;
                                        a640not_first_factor(&(*acv) , &(*dmli) , &colin2 , &curr_n);
                                        }
                                    }
                                }
                            }
                        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var133->a_return_segm , "vak641.p" , 16756)).integer.C_4.sp1r_returncode == 0)
                            {
                            switch (param_cnt)
                                {
                                case 2 :
                                    a641string_set_operator(&(*acv) , 10 , 3 , (int ) (*colin).sci_len , (unsigned char)'\002' , (unsigned char)'\0');
                                    break;
                                case 3 :
                                    a641string_set_operator(&(*acv) , 10 , 3 , (int ) (*colin).sci_len , (unsigned char)'\003' , (unsigned char)'\0');
                                    break;
                                case 4 :
                                    a641string_set_operator(&(*acv) , 10 , 3 , (int ) (*colin).sci_len , (unsigned char)'\004' , (unsigned char)'\0');
                                    break;
                                default :
                                    sql__caseerr("vak641.p" , 16759);
                                    break;
                                }
                            ptoc_Var138 = &*colin;
                            ptoc_Var138->sci_len = 3;
                            ptoc_Var138->sci_frac = 0;
                            ptoc_Var138->sci_iolen = 4;
                            ptoc_Var138->sci_typ = 0;
                            }
                        }
                    }
                }
            }
        }
    cccprint("<ak641index     ");
    }



int ak641last_day(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    int curr_n;
    tak_all_command_glob *ptoc_Var139;
    struct ptoc_Type59 *ptoc_Var140;
    tak_dml_info *ptoc_Var141;
    tak00_scolinf *ptoc_Var142;
    cccprint("AK641 ak641last_");
    ptoc_Var139 = &*acv;
    ptoc_Var140 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var139->a_ap_tree , "vak641.p" , 16810))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var141 = &*dmli;
    ptoc_Var141->d_datatype = 13;
    curr_n = (int ) ptoc_Var140->n_lo_level;
    (*colin).sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var139->a_return_segm , "vak641.p" , 16816)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) (*colin).sci_typ != 13)
            {
            a641check_datetime(&(*acv) , &(*dmli) , 13);
            }
        ptoc_Var141->d_datatype = 13;
        ptoc_Var142 = &*colin;
        ptoc_Var142->sci_len = 26;
        ptoc_Var142->sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var142->sci_len , -32768 , 32767 , "vak641.p" , 16827) , -32768 , 32767);
        ptoc_Var142->sci_frac = 0;
        ptoc_Var142->sci_typ = 13;
        a65_set_operator(&(*acv) , 56);
        }
    cccprint("<ak641last_day  ");
    }

static
  tak00_colpropset ptoc_Var143 = 
  {
  (unsigned char)'\002', (unsigned char)'\0'
  };


int ak641length_vsize(acv, dmli, colin, act_node, symbol, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
tak_sc_symbol symbol;
char *wrong_datatype;
    {
    int curr_n;
    int oldlen;
    tsp_int2 res_len;
    tak_all_command_glob *ptoc_Var144;
    struct ptoc_Type59 *ptoc_Var145;
    tak_dml_info *ptoc_Var146;
    int ptoc_Var147;
    tak00_scolinf *ptoc_Var148;
    cccprint("AK641 ak641lengt");
    ptoc_Var144 = &*acv;
    ptoc_Var145 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var144->a_ap_tree , "vak641.p" , 16856))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var146 = &*dmli;
    ptoc_Var146->d_datatype = 14;
    curr_n = (int ) ptoc_Var145->n_lo_level;
    (*colin).sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var144->a_return_segm , "vak641.p" , 16862)).integer.C_4.sp1r_returncode == 21000)
        {
        (*(tsp1_segment *) sql__nil(ptoc_Var144->a_return_segm , "vak641.p" , 16865)).integer.C_4.sp1r_returncode = 0;
        ptoc_Var146->d_datatype = 16;
        a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var144->a_return_segm , "vak641.p" , 16870)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) ptoc_Var144->a_sqlmode != 2 || (int ) symbol == 70 || (ptoc_Var147 = (int ) ptoc_Var146->d_datatype) != 15 && ptoc_Var147 != 0 && ptoc_Var147 != 1 && ptoc_Var147 != 12)
            {
            if (((int ) ptoc_Var146->d_datatype == 18 || (int ) ptoc_Var146->d_datatype == 24) && (int ) symbol != 70)
                {
                a65_set_operator(&(*acv) , 3);
                }
            else
                {
                a65_set_operator(&(*acv) , 14);
                }
            }
        else
            {
            oldlen = (int ) (*colin).sci_len;
            a641_get_length(&(*colin) , &res_len , &(*wrong_datatype));
            if (!(char ) sql__setrang(*wrong_datatype , 0 , 1))
                {
                a641string_set_operator(&(*acv) , 26 , 0 , oldlen , (unsigned char)'\0' , (unsigned char ) sql__setrang((unsigned char ) res_len , 0 , 255));
                }
            }
        }
    if (!(char ) sql__setrang(*wrong_datatype , 0 , 1))
        {
        ptoc_Var148 = &*colin;
        ptoc_Var148->sci_len = 4;
        ptoc_Var148->sci_frac = 0;
        ptoc_Var148->sci_iolen = 4;
        ptoc_Var148->sci_typ = 0;
        _ptoc_MOVE(unsigned char , 2, ptoc_Var148->sci_cprops, ptoc_Var143);
        ptoc_Var146->d_datatype = 15;
        }
    cccprint("<ak641length_vsi");
    }



int ak641to_char(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    int curr_n;
    tsp_data_type keep_datatype;
    tak00_scolinf colin2;
    tsp_int2 max_length;
    tsp_int2 to_char_variant;
    tak_all_command_glob *ptoc_Var149;
    struct ptoc_Type59 *ptoc_Var150;
    tak_dml_info *ptoc_Var151;
    int ptoc_Var152;
    struct ptoc_Type59 *ptoc_Var153;
    struct tgg00_MessBlock *ptoc_Var154;
    tgg00_QualBuf *ptoc_Var155;
    struct ptoc_Type59 *ptoc_Var156;
    struct tgg00_MessBlock *ptoc_Var157;
    tgg00_QualBuf *ptoc_Var158;
    tak00_scolinf *ptoc_Var159;
    tak00_scolinf *ptoc_Var160;
    cccprint("AK641 ak641to_ch");
    ptoc_Var149 = &*acv;
    ptoc_Var150 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var149->a_ap_tree , "vak641.p" , 16932))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var151 = &*dmli;
    keep_datatype = (unsigned char ) sql__setrang(ptoc_Var151->d_datatype , 0 , 39);
    ptoc_Var151->d_datatype = 14;
    curr_n = (int ) ptoc_Var150->n_lo_level;
    (*colin).sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var149->a_return_segm , "vak641.p" , 16939)).integer.C_4.sp1r_returncode == 21000)
        {
        (*(tsp1_segment *) sql__nil(ptoc_Var149->a_return_segm , "vak641.p" , 16942)).integer.C_4.sp1r_returncode = 0;
        ptoc_Var151->d_datatype = 16;
        a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
        }
    t01int4(5 , "colin.scityp" , sql__ord((int ) (*colin).sci_typ));
    t01int4(5 , "d_datatype  " , sql__ord((int ) ptoc_Var151->d_datatype));
    curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var149->a_ap_tree , "vak641.p" , 16949))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var149->a_return_segm , "vak641.p" , 16950)).integer.C_4.sp1r_returncode == 0)
        {
        if ((ptoc_Var152 = (int ) (*colin).sci_typ) == 0 || ptoc_Var152 == 1 || ptoc_Var152 == 12 || ptoc_Var152 == 15)
            {
            if (curr_n == 0)
                {
                to_char_variant = 3;
                max_length = 40;
                }
            else
                {
                to_char_variant = 2;
                if ((char ) sql__setrang(g01unicode , 0 , 1) && (int ) g01code.char_default == 20)
                    {
                    ptoc_Var151->d_datatype = 2;
                    }
                else
                    {
                    ptoc_Var151->d_datatype = 14;
                    }
                a640not_first_factor(&(*acv) , &(*dmli) , &colin2 , &curr_n);
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var149->a_return_segm , "vak641.p" , 16972)).integer.C_4.sp1r_returncode == 21000)
                    {
                    (*(tsp1_segment *) sql__nil(ptoc_Var149->a_return_segm , "vak641.p" , 16975)).integer.C_4.sp1r_returncode = 0;
                    ptoc_Var151->d_datatype = 16;
                    a640not_first_factor(&(*acv) , &(*dmli) , &colin2 , &curr_n);
                    }
                if ((ptoc_Var152 = (int ) colin2.sci_typ) == 2 || ptoc_Var152 == 3 || ptoc_Var152 == 14)
                    {
                    ptoc_Var153 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var149->a_ap_tree , "vak641.p" , 16982))[sql__setrang(curr_n , 0 , 32767)];
                    if ((int ) ptoc_Var153->n_symb == 48)
                        {
                        ptoc_Var154 = &ptoc_Var149->a_mblock;
                        ptoc_Var155 = (tgg00_QualBuf *) sql__nil(ptoc_Var154->integer.C_4.mb_qual , "vak641.p" , 16985);
                        max_length = (short ) sql__setrang(k79n_dest_len_ora_number_format((*(tgg00_DataPart *) sql__nil(ptoc_Var154->integer.C_3.mb_data , "vak641.p" , 16987)).integer.C_1.mbp_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var154->integer.C_5.mb_st , "vak641.p" , 16988))[sql__setrang((int ) ptoc_Var155->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.epos + 1 , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var154->integer.C_5.mb_st , "vak641.p" , 16989))[sql__setrang((int ) ptoc_Var155->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.elen_var + -1) , -32768 , 32767);
                        }
                    else
                        {
                        max_length = 100;
                        }
                    }
                else
                    {
                    a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var149->a_ap_tree , "vak641.p" , 16997))[sql__setrang(curr_n , 0 , 32767)].n_pos);
                    }
                }
            }
        else
            {
            if ((int ) (*colin).sci_typ != 13)
                {
                a641check_datetime(&(*acv) , &(*dmli) , 13);
                }
            if ((int ) ptoc_Var150->n_symb == 197)
                {
                max_length = 14;
                a641string_set_operator(&(*acv) , 25 , (int ) max_length , (int ) (*colin).sci_len , (unsigned char)'\0' , (unsigned char)'\0');
                }
            else
                {
                if (curr_n == 0)
                    {
                    to_char_variant = 1;
                    max_length = 9;
                    }
                else
                    {
                    to_char_variant = 0;
                    if ((char ) sql__setrang(g01unicode , 0 , 1) && (int ) g01code.char_default == 20)
                        {
                        ptoc_Var151->d_datatype = 2;
                        }
                    else
                        {
                        ptoc_Var151->d_datatype = 14;
                        }
                    a640not_first_factor(&(*acv) , &(*dmli) , &colin2 , &curr_n);
                    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var149->a_return_segm , "vak641.p" , 17033)).integer.C_4.sp1r_returncode == 21000)
                        {
                        (*(tsp1_segment *) sql__nil(ptoc_Var149->a_return_segm , "vak641.p" , 17036)).integer.C_4.sp1r_returncode = 0;
                        ptoc_Var151->d_datatype = 16;
                        a640not_first_factor(&(*acv) , &(*dmli) , &colin2 , &curr_n);
                        }
                    if ((ptoc_Var152 = (int ) colin2.sci_typ) == 2 || ptoc_Var152 == 3 || ptoc_Var152 == 14)
                        {
                        ptoc_Var156 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var149->a_ap_tree , "vak641.p" , 17043))[sql__setrang(curr_n , 0 , 32767)];
                        if ((int ) ptoc_Var156->n_symb == 48)
                            {
                            ptoc_Var157 = &ptoc_Var149->a_mblock;
                            ptoc_Var158 = (tgg00_QualBuf *) sql__nil(ptoc_Var157->integer.C_4.mb_qual , "vak641.p" , 17046);
                            max_length = (short ) sql__setrang(s78t_dest_len_date_format((*(tgg00_DataPart *) sql__nil(ptoc_Var157->integer.C_3.mb_data , "vak641.p" , 17048)).integer.C_1.mbp_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var157->integer.C_5.mb_st , "vak641.p" , 17049))[sql__setrang((int ) ptoc_Var158->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.epos + 1 , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var157->integer.C_5.mb_st , "vak641.p" , 17050))[sql__setrang((int ) ptoc_Var158->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.elen_var + -1) , -32768 , 32767);
                            }
                        else
                            {
                            max_length = 100;
                            }
                        }
                    else
                        {
                        a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var149->a_ap_tree , "vak641.p" , 17058))[sql__setrang(curr_n , 0 , 32767)].n_pos);
                        }
                    }
                }
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var149->a_return_segm , "vak641.p" , 17066)).integer.C_4.sp1r_returncode == 0 && (int ) ptoc_Var150->n_symb != 197)
        {
        if ((int ) to_char_variant == 1)
            {
            a641f_push_format(&(*acv) , &(*dmli));
            max_length = 26;
            to_char_variant = 0;
            }
        if ((int ) to_char_variant <= 1)
            {
            a641l_push_language(&(*acv) , &(*dmli));
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var149->a_return_segm , "vak641.p" , 17081)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var159 = &*colin;
            a641string_set_operator(&(*acv) , 24 , (int ) max_length + 1 , (int ) to_char_variant , (unsigned char ) sql__setrang((unsigned char ) ptoc_Var159->sci_frac , 0 , 255) , (unsigned char ) sql__setrang((unsigned char ) ptoc_Var159->sci_len , 0 , 255));
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var149->a_return_segm , "vak641.p" , 17091)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var160 = &*colin;
        ptoc_Var160->sci_frac = 0;
        ptoc_Var160->sci_len = (short ) sql__setrang(max_length , -32768 , 32767);
        ptoc_Var160->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var160->sci_len + 1) , -32768 , 32767);
        ptoc_Var160->sci_typ = 2;
        }
    ptoc_Var151->d_datatype = 2;
    cccprint("<ak641to_char   ");
    }



int ak641toidentifier(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    int curr_n;
    tak_all_command_glob *ptoc_Var161;
    struct ptoc_Type59 *ptoc_Var162;
    tak_dml_info *ptoc_Var163;
    struct tgg00_MessBlock *ptoc_Var164;
    tgg00_QualBuf *ptoc_Var165;
    tgg00_StackEntry *ptoc_Var166;
    cccprint("AK641 ak641toide");
    ptoc_Var161 = &*acv;
    ptoc_Var162 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var161->a_ap_tree , "vak641.p" , 17120))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var163 = &*dmli;
    ptoc_Var163->d_datatype = 14;
    (*colin).sci_len = 0;
    curr_n = (int ) ptoc_Var162->n_lo_level;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var161->a_return_segm , "vak641.p" , 17126)).integer.C_4.sp1r_returncode == 21000)
        {
        (*(tsp1_segment *) sql__nil(ptoc_Var161->a_return_segm , "vak641.p" , 17129)).integer.C_4.sp1r_returncode = 0;
        if ((int ) g01code.ctype == 0)
            {
            ptoc_Var163->d_datatype = 2;
            }
        else
            {
            ptoc_Var163->d_datatype = 3;
            }
        ptoc_Var163->d_expand = 125;
        a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
        }
    ptoc_Var163->d_expand = 0;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var161->a_return_segm , "vak641.p" , 17141)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) (*colin).sci_typ == 24)
            {
            (*colin).sci_len = (short ) sql__setrang((short ) ((int ) (*colin).sci_len * 2 * 2 + 4) , -32768 , 32767);
            }
        else
            {
            (*colin).sci_len = (short ) sql__setrang((short ) ((int ) (*colin).sci_len * 2 + 2) , -32768 , 32767);
            }
        (*colin).sci_frac = 0;
        (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len + 1) , -32768 , 32767);
        if ((int ) (*colin).sci_iolen > 8000)
            {
            a07_b_put_error(&(*acv) , 6374 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var161->a_ap_tree , "vak641.p" , 17157))[sql__setrang(act_node , 0 , 32767)].n_pos);
            }
        ptoc_Var164 = &ptoc_Var161->a_mblock;
        ptoc_Var165 = (tgg00_QualBuf *) sql__nil(ptoc_Var164->integer.C_4.mb_qual , "vak641.p" , 17159);
        if ((int ) ptoc_Var165->integer.C_2.mfirst_free >= (int ) ptoc_Var164->integer.C_2.mb_st_max)
            {
            a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var164->integer.C_2.mb_st_max);
            }
        else
            {
            ptoc_Var166 = &(*(tgg00_StackList *) sql__nil(ptoc_Var164->integer.C_5.mb_st , "vak641.p" , 17164))[sql__setrang((int ) ptoc_Var165->integer.C_2.mfirst_free , 1 , 32767) + -1];
            ptoc_Var166->integer.C_1.etype = 55;
            ptoc_Var166->integer.C_4.eop_build_in = 58;
            ptoc_Var166->integer.C_1.elen_var = 0;
            ptoc_Var166->integer.C_1.epos = 0;
            ptoc_Var166->integer.C_1.ecol_tab[0] = (unsigned char ) sql__setrang((unsigned char ) ptoc_Var161->a_sqlmode , 0 , 255);
            ptoc_Var166->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
            ptoc_Var165->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var165->integer.C_2.mfirst_free , -32768 , 32767 , "vak641.p" , 17172) , -32768 , 32767);
            ptoc_Var165->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var165->integer.C_2.mqual_cnt , -32768 , 32767 , "vak641.p" , 17173) , -32768 , 32767);
            }
        }
    cccprint("<ak641toidentifi");
    }



int a641l_push_language(acv, dmli)
tak_all_command_glob *acv;
tak_dml_info *dmli;
    {
    tsp_int4 curr_data_pos;
    tak_all_command_glob *ptoc_Var167;
    struct tgg00_MessBlock *ptoc_Var168;
    tgg00_DataPart *ptoc_Var169;
    tgg00_QualBuf *ptoc_Var170;
    tgg00_StackEntry *ptoc_Var171;
    cccprint("AK641 a641l_push");
    ptoc_Var167 = &*acv;
    ptoc_Var168 = &ptoc_Var167->a_mblock;
    ptoc_Var169 = (tgg00_DataPart *) sql__nil(ptoc_Var168->integer.C_3.mb_data , "vak641.p" , 17197);
    ptoc_Var170 = (tgg00_QualBuf *) sql__nil(ptoc_Var168->integer.C_4.mb_qual , "vak641.p" , 17197);
    if ((int ) ptoc_Var170->integer.C_2.mfirst_free > (int ) ptoc_Var168->integer.C_2.mb_st_max)
        {
        a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var168->integer.C_2.mb_st_max);
        }
    else
        {
        curr_data_pos = ptoc_Var168->integer.C_2.mb_data_len + 1;
        ptoc_Var171 = &(*(tgg00_StackList *) sql__nil(ptoc_Var168->integer.C_5.mb_st , "vak641.p" , 17204))[sql__setrang((int ) ptoc_Var170->integer.C_2.mfirst_free , 1 , 32767) + -1];
        if ((char ) sql__setrang((*dmli).d_view , 0 , 1))
            {
            ptoc_Var171->integer.C_1.etype = 59;
            }
        else
            {
            ptoc_Var171->integer.C_1.etype = 22;
            }
        ptoc_Var171->integer.C_1.eop = 0;
        ptoc_Var171->integer.C_1.epos = (short ) sql__setrang((short ) curr_data_pos , -32768 , 32767);
        ptoc_Var171->integer.C_1.elen_var = 4;
        ptoc_Var171->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
        ptoc_Var171->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
        if (!(char ) sql__setrang((*dmli).d_view , 0 , 1))
            {
            ptoc_Var169->integer.C_1.mbp_buf[sql__setrang(curr_data_pos , 1 , 8192000) + -1] = (unsigned char)' ';
            if ((int ) ptoc_Var167->a_sqlmode != 2)
                {
                g10mv4("VAK641" , 5 , sizeof(tsp00_C3 ) , ptoc_Var168->integer.C_2.mb_data_size , ptoc_Var167->a_ak_language , 1 , ptoc_Var169->integer.C_1.mbp_buf , curr_data_pos + 1 , 3 , &(*(tsp1_segment *) sql__nil(ptoc_Var167->a_return_segm , "vak641.p" , 17228)).integer.C_4.sp1r_returncode);
                }
            else
                {
                g10mv4("VAK641" , 6 , sizeof(tsp00_C3 ) , ptoc_Var168->integer.C_2.mb_data_size , ptoc_Var167->a_nls_params.date_language , 1 , ptoc_Var169->integer.C_1.mbp_buf , curr_data_pos + 1 , 3 , &(*(tsp1_segment *) sql__nil(ptoc_Var167->a_return_segm , "vak641.p" , 17234)).integer.C_4.sp1r_returncode);
                }
            }
        ptoc_Var168->integer.C_2.mb_data_len = ptoc_Var168->integer.C_2.mb_data_len + 4;
        ptoc_Var170->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var170->integer.C_2.mfirst_free , -32768 , 32767 , "vak641.p" , 17239) , -32768 , 32767);
        ptoc_Var170->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var170->integer.C_2.mqual_cnt , -32768 , 32767 , "vak641.p" , 17240) , -32768 , 32767);
        }
    cccprint("<a641l_push_lang");
    }



int ak641s_push_string(acv, dtyp, tree_index, curr_data_len)
tak_all_command_glob *acv;
tsp_data_type dtyp;
int tree_index;
tsp_int4 curr_data_len;
    {
    tsp_int4 curr_data_pos;
    tak_all_command_glob *ptoc_Var172;
    struct tgg00_MessBlock *ptoc_Var173;
    tgg00_DataPart *ptoc_Var174;
    tgg00_QualBuf *ptoc_Var175;
    tgg00_StackEntry *ptoc_Var176;
    cccprint("AK641 ak641s_pus");
    ptoc_Var172 = &*acv;
    ptoc_Var173 = &ptoc_Var172->a_mblock;
    ptoc_Var174 = (tgg00_DataPart *) sql__nil(ptoc_Var173->integer.C_3.mb_data , "vak641.p" , 17259);
    ptoc_Var175 = (tgg00_QualBuf *) sql__nil(ptoc_Var173->integer.C_4.mb_qual , "vak641.p" , 17259);
    curr_data_pos = ptoc_Var173->integer.C_2.mb_data_len + 1;
    if ((int ) ptoc_Var175->integer.C_2.mfirst_free > (int ) ptoc_Var173->integer.C_2.mb_st_max)
        {
        a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var173->integer.C_2.mb_st_max);
        }
    else
        {
        if (ptoc_Var173->integer.C_2.mb_data_size < curr_data_pos + curr_data_len + 1)
            {
            a07_b_put_error(&(*acv) , 2801 , -ptoc_Var173->integer.C_2.mb_data_size);
            }
        else
            {
            if ((int ) dtyp == 24)
                {
                ptoc_Var174->integer.C_1.mbp_buf[sql__setrang(curr_data_pos , 1 , 8192000) + -1] = (unsigned char)'\001';
                }
            else
                {
                ptoc_Var174->integer.C_1.mbp_buf[sql__setrang(curr_data_pos , 1 , 8192000) + -1] = (unsigned char)' ';
                if ((char ) sql__setrang(g01unicode , 0 , 1))
                    {
                    curr_data_len = curr_data_len / 2;
                    }
                }
            a05string_literal_get(&(*acv) , tree_index , (unsigned char ) sql__setrang(dtyp , 0 , 39) , ptoc_Var174->integer.C_1.mbp_buf , curr_data_pos + 1 , curr_data_len);
            ptoc_Var176 = &(*(tgg00_StackList *) sql__nil(ptoc_Var173->integer.C_5.mb_st , "vak641.p" , 17285))[sql__setrang((int ) ptoc_Var175->integer.C_2.mfirst_free , 1 , 32767) + -1];
            ptoc_Var176->integer.C_1.etype = 22;
            ptoc_Var176->integer.C_1.eop = 0;
            ptoc_Var176->integer.C_1.epos = (short ) sql__setrang((short ) curr_data_pos , -32768 , 32767);
            ptoc_Var176->integer.C_1.elen_var = (short ) sql__setrang((short ) (curr_data_len + 1) , -32768 , 32767);
            ptoc_Var176->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
            ptoc_Var176->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
            ptoc_Var173->integer.C_2.mb_data_len = ptoc_Var173->integer.C_2.mb_data_len + curr_data_len + 1;
            ptoc_Var175->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var175->integer.C_2.mfirst_free , -32768 , 32767 , "vak641.p" , 17296) , -32768 , 32767);
            ptoc_Var175->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var175->integer.C_2.mqual_cnt , -32768 , 32767 , "vak641.p" , 17297) , -32768 , 32767);
            }
        }
    cccprint("<ak641s_push_str");
    }



int ak641push_blank(acv, dtyp)
tak_all_command_glob *acv;
tsp_data_type dtyp;
    {
    tsp_int4 curr_data_pos;
    tsp_int4 curr_data_len;
    tak_all_command_glob *ptoc_Var177;
    struct tgg00_MessBlock *ptoc_Var178;
    tgg00_DataPart *ptoc_Var179;
    tgg00_QualBuf *ptoc_Var180;
    tgg00_StackEntry *ptoc_Var181;
    cccprint("AK641 ak641push_");
    ptoc_Var177 = &*acv;
    ptoc_Var178 = &ptoc_Var177->a_mblock;
    ptoc_Var179 = (tgg00_DataPart *) sql__nil(ptoc_Var178->integer.C_3.mb_data , "vak641.p" , 17319);
    ptoc_Var180 = (tgg00_QualBuf *) sql__nil(ptoc_Var178->integer.C_4.mb_qual , "vak641.p" , 17319);
    if ((int ) dtyp == 24)
        {
        curr_data_len = 3;
        }
    else
        {
        curr_data_len = 2;
        }
    curr_data_pos = ptoc_Var178->integer.C_2.mb_data_len + 1;
    if ((int ) ptoc_Var180->integer.C_2.mfirst_free > (int ) ptoc_Var178->integer.C_2.mb_st_max)
        {
        a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var178->integer.C_2.mb_st_max);
        }
    else
        {
        if (ptoc_Var178->integer.C_2.mb_data_size < curr_data_pos + curr_data_len)
            {
            a07_b_put_error(&(*acv) , 2801 , -ptoc_Var178->integer.C_2.mb_data_size);
            }
        else
            {
            ptoc_Var181 = &(*(tgg00_StackList *) sql__nil(ptoc_Var178->integer.C_5.mb_st , "vak641.p" , 17337))[sql__setrang((int ) ptoc_Var180->integer.C_2.mfirst_free , 1 , 32767) + -1];
            ptoc_Var181->integer.C_1.etype = 22;
            ptoc_Var181->integer.C_1.eop = 0;
            ptoc_Var181->integer.C_1.epos = (short ) sql__setrang((short ) curr_data_pos , -32768 , 32767);
            ptoc_Var181->integer.C_1.elen_var = (short ) sql__setrang((short ) curr_data_len , -32768 , 32767);
            ptoc_Var181->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
            ptoc_Var181->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
            if ((int ) dtyp == 24)
                {
                ptoc_Var179->integer.C_1.mbp_buf[sql__setrang(curr_data_pos , 1 , 8192000) + -1] = (unsigned char)'\001';
                ptoc_Var179->integer.C_1.mbp_buf[sql__setrang(curr_data_pos + 1 , 1 , 8192000) + -1] = (unsigned char)'\0';
                ptoc_Var179->integer.C_1.mbp_buf[sql__setrang(curr_data_pos + 2 , 1 , 8192000) + -1] = (unsigned char)' ';
                }
            else
                {
                ptoc_Var179->integer.C_1.mbp_buf[sql__setrang(curr_data_pos , 1 , 8192000) + -1] = (unsigned char)' ';
                ptoc_Var179->integer.C_1.mbp_buf[sql__setrang(curr_data_pos + 1 , 1 , 8192000) + -1] = (unsigned char)' ';
                }
            ptoc_Var178->integer.C_2.mb_data_len = ptoc_Var178->integer.C_2.mb_data_len + curr_data_len;
            ptoc_Var180->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var180->integer.C_2.mfirst_free , -32768 , 32767 , "vak641.p" , 17361) , -32768 , 32767);
            ptoc_Var180->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var180->integer.C_2.mqual_cnt , -32768 , 32767 , "vak641.p" , 17362) , -32768 , 32767);
            }
        }
    cccprint("<ak641push_blank");
    }



int a641f_push_format(acv, dmli)
tak_all_command_glob *acv;
tak_dml_info *dmli;
    {
    tsp_int4 curr_data_pos;
    tsp_int4 curr_data_len;
    tak_all_command_glob *ptoc_Var182;
    struct tgg00_MessBlock *ptoc_Var183;
    tgg00_DataPart *ptoc_Var184;
    tgg00_QualBuf *ptoc_Var185;
    tgg00_StackEntry *ptoc_Var186;
    cccprint("AK641 a641f_push");
    ptoc_Var182 = &*acv;
    ptoc_Var183 = &ptoc_Var182->a_mblock;
    ptoc_Var184 = (tgg00_DataPart *) sql__nil(ptoc_Var183->integer.C_3.mb_data , "vak641.p" , 17384);
    ptoc_Var185 = (tgg00_QualBuf *) sql__nil(ptoc_Var183->integer.C_4.mb_qual , "vak641.p" , 17384);
    curr_data_pos = ptoc_Var183->integer.C_2.mb_data_len + 1;
    if ((char ) sql__setrang((*dmli).d_view , 0 , 1))
        {
        curr_data_len = 27;
        }
    else
        {
        curr_data_len = (int ) ptoc_Var182->a_nls_params.df_length;
        }
    if ((int ) ptoc_Var185->integer.C_2.mfirst_free > (int ) ptoc_Var183->integer.C_2.mb_st_max)
        {
        a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var183->integer.C_2.mb_st_max);
        }
    else
        {
        if (ptoc_Var183->integer.C_2.mb_data_size < curr_data_pos + curr_data_len)
            {
            a07_b_put_error(&(*acv) , 2801 , -ptoc_Var183->integer.C_2.mb_data_size);
            }
        else
            {
            ptoc_Var186 = &(*(tgg00_StackList *) sql__nil(ptoc_Var183->integer.C_5.mb_st , "vak641.p" , 17402))[sql__setrang((int ) ptoc_Var185->integer.C_2.mfirst_free , 1 , 32767) + -1];
            if ((char ) sql__setrang((*dmli).d_view , 0 , 1))
                {
                ptoc_Var186->integer.C_1.etype = 60;
                }
            else
                {
                ptoc_Var186->integer.C_1.etype = 22;
                }
            ptoc_Var186->integer.C_1.eop = 0;
            ptoc_Var186->integer.C_1.epos = (short ) sql__setrang((short ) curr_data_pos , -32768 , 32767);
            ptoc_Var186->integer.C_1.elen_var = (short ) sql__setrang((short ) curr_data_len , -32768 , 32767);
            ptoc_Var186->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
            ptoc_Var186->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
            if (!(char ) sql__setrang((*dmli).d_view , 0 , 1))
                {
                g10mv7("VAK641" , 7 , sizeof(unsigned char [50]) , ptoc_Var183->integer.C_2.mb_data_size , ptoc_Var182->a_nls_params.date_format , 1 , ptoc_Var184->integer.C_1.mbp_buf , curr_data_pos , (int ) (*acv).a_nls_params.df_length , &(*(tsp1_segment *) sql__nil(ptoc_Var182->a_return_segm , "vak641.p" , 17424)).integer.C_4.sp1r_returncode);
                }
            ptoc_Var183->integer.C_2.mb_data_len = ptoc_Var183->integer.C_2.mb_data_len + curr_data_len;
            ptoc_Var185->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var185->integer.C_2.mfirst_free , -32768 , 32767 , "vak641.p" , 17428) , -32768 , 32767);
            ptoc_Var185->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var185->integer.C_2.mqual_cnt , -32768 , 32767 , "vak641.p" , 17429) , -32768 , 32767);
            }
        }
    cccprint("<a641f_push_form");
    }



int a641d_push_date(acv, dmli)
tak_all_command_glob *acv;
tak_dml_info *dmli;
    {
    tsp_int2 put_node;
    int date_n;
    tak00_scolinf colin;
    tsp_data_type keep_datatype;
    char keep_change;
    tak_all_command_glob *ptoc_Var187;
    tak_dml_info *ptoc_Var188;
    cccprint("AK641 a641d_push");
    ptoc_Var187 = &*acv;
    ptoc_Var188 = &*dmli;
    ptoc_Var187->a_scv.sc_symb = 24;
    a01_put_node(&(*acv) , &put_node);
    keep_change = (char ) sql__setrang(ptoc_Var188->d_change_date_time , 0 , 1);
    keep_datatype = (unsigned char ) sql__setrang(ptoc_Var188->d_datatype , 0 , 39);
    ptoc_Var188->d_change_date_time = 0;
    ptoc_Var188->d_datatype = 10;
    date_n = (int ) put_node;
    colin.sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &colin , &date_n);
    ptoc_Var188->d_datatype = (unsigned char ) sql__setrang(keep_datatype , 0 , 39);
    ptoc_Var188->d_change_date_time = (char ) sql__setrang(keep_change , 0 , 1);
    cccprint("<a641d_push_date");
    }



int a641translate_string_literal(letter, keep_datatype)
unsigned char *letter;
tsp_data_type *keep_datatype;
    {
    cccprint("AK641 a641transl");
    if ((int ) *keep_datatype == 2 && (int ) g01code.ctype != 0)
        {
        g02tebcdic_to_ascii(&(*letter) , &(*letter) , 1);
        }
    else
        {
        if ((int ) *keep_datatype == 3 && (int ) g01code.ctype != 1)
            {
            g02tascii_to_ebcdic(&(*letter) , &(*letter) , 1);
            }
        }
    cccprint("<a641translate_s");
    }



int ak641time_date_to_char_datatype(erg_datatype, keep_datatype)
tsp_data_type *erg_datatype;
tsp_data_type *keep_datatype;
    {
    int ptoc_Var189;
    cccprint("AK641 ak641time_");
    if ((ptoc_Var189 = (int ) *keep_datatype) == 10 || ptoc_Var189 == 11 || ptoc_Var189 == 13)
        {
        if ((int ) g01code.ctype == 0)
            {
            *erg_datatype = 2;
            }
        else
            {
            *erg_datatype = 3;
            }
        }
    else
        {
        *erg_datatype = (unsigned char ) sql__setrang(*keep_datatype , 0 , 39);
        }
    cccprint("<ak641time_date_");
    }

static
  tsp8_uni_opt_set ptoc_Var190 = 
  {
  (unsigned char)'\0'
  };


int a641s_literal_value(acv, act_node, keep_datatype, string_allowed, string_found, letter, wrong_datatype)
tak_all_command_glob *acv;
int act_node;
tsp_data_type keep_datatype;
char string_allowed;
char *string_found;
unsigned char *letter;
char *wrong_datatype;
    {
    char invalid;
    int bytestr_len;
    tsp_c2 c2;
    tsp_int4 uni_len;
    tsp_int4 err_char_no;
    tsp8_uni_error uni_err;
    tak_all_command_glob *ptoc_Var191;
    struct ptoc_Type59 *ptoc_Var192;
    tsp8_uni_opt_set ptoc_Var193;
    cccprint("AK641 a641s_lite");
    *string_found = 0;
    ptoc_Var191 = &*acv;
    ptoc_Var192 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var191->a_ap_tree , "vak641.p" , 17541))[sql__setrang(act_node , 0 , 32767)];
    if ((int ) ptoc_Var192->n_symb == 48 && (int ) keep_datatype != 4)
        {
        if ((int ) ptoc_Var192->n_length == 0)
            {
            a07_b_put_error(&(*acv) , 6090 , ptoc_Var192->n_pos);
            }
        else
            {
            if ((char ) sql__setrang(g01unicode , 0 , 1))
                {
                if ((int ) (*(tsp1_part *) sql__nil(ptoc_Var191->a_cmd_part , "vak641.p" , 17553)).integer.C_1.sp1p_buf[sql__setrang(ptoc_Var192->n_pos , 1 , 8192000) + -1] == 0 && (int ) ptoc_Var192->n_length == 2)
                    {
                    *letter = (unsigned char ) sql__setrang((*(tsp1_part *) sql__nil(ptoc_Var191->a_cmd_part , "vak641.p" , 17556)).integer.C_1.sp1p_buf[sql__setrang(ptoc_Var192->n_pos + 1 , 1 , 8192000) + -1] , 0 , 255);
                    }
                else
                    {
                    if ((char ) sql__setrang(string_allowed , 0 , 1) && (int ) ptoc_Var192->n_length >= 2)
                        {
                        ak641s_push_string(&(*acv) , (unsigned char ) sql__setrang(keep_datatype , 0 , 39) , act_node , (int ) ptoc_Var192->n_length);
                        *letter = (unsigned char)'\0';
                        *string_found = 1;
                        }
                    else
                        {
                        *wrong_datatype = 1;
                        }
                    }
                }
            else
                {
                if ((int ) ptoc_Var192->n_length == 1)
                    {
                    *letter = (unsigned char ) sql__setrang((*(tsp1_part *) sql__nil(ptoc_Var191->a_cmd_part , "vak641.p" , 17573)).integer.C_1.sp1p_buf[sql__setrang(ptoc_Var192->n_pos , 1 , 8192000) + -1] , 0 , 255);
                    }
                else
                    {
                    if ((char ) sql__setrang(string_allowed , 0 , 1) && (int ) ptoc_Var192->n_length > 1)
                        {
                        ak641s_push_string(&(*acv) , (unsigned char ) sql__setrang(keep_datatype , 0 , 39) , act_node , (int ) ptoc_Var192->n_length);
                        *letter = (unsigned char)'\0';
                        *string_found = 1;
                        }
                    else
                        {
                        *wrong_datatype = 1;
                        }
                    }
                }
            }
        }
    else
        {
        if ((int ) ptoc_Var192->n_symb == 50)
            {
            a07_b_put_error(&(*acv) , 6224 , ptoc_Var192->n_pos);
            }
        else
            {
            if ((int ) ptoc_Var192->n_symb == 23)
                {
                invalid = 0;
                if ((int ) ptoc_Var192->n_length == 4 && (char ) sql__setrang(g01unicode , 0 , 1))
                    {
                    uni_len = 2;
                    s80uni_trans((tsp00_MoveObj *) &(*(tsp1_part *) sql__nil(ptoc_Var191->a_cmd_part , "vak641.p" , 17605)).integer.C_1.sp1p_buf[sql__setrang(ptoc_Var192->n_pos , 1 , 8192000) + -1] , (int ) ptoc_Var192->n_length , 20 , (tsp00_MoveObj *) c2 , &uni_len , 0 , *(tsp8_uni_opt_set *) (_ptoc_MOVE(unsigned char , 1, ptoc_Var193, ptoc_Var190) , ptoc_Var193) , &uni_err , &err_char_no);
                    if ((int ) uni_err == 0)
                        {
                        s41p1byte(&(*letter) , 1 , &bytestr_len , c2 , 1 , uni_len , &invalid);
                        }
                    else
                        {
                        a07_hex_uni_error(&(*acv) , (unsigned char ) sql__setrang(uni_err , 0 , 8) , ptoc_Var192->n_pos + err_char_no + -1 , 0 , (tsp00_MoveObj *) &(*(tsp1_part *) sql__nil(ptoc_Var191->a_cmd_part , "vak641.p" , 17615)).integer.C_1.sp1p_buf[sql__setrang(ptoc_Var192->n_pos + err_char_no + -1 , 1 , 8192000) + -1] , 2);
                        }
                    }
                else
                    {
                    if ((int ) ptoc_Var192->n_length == 2 && !(char ) sql__setrang(g01unicode , 0 , 1))
                        {
                        s41pbyte(&(*letter) , 1 , &bytestr_len , (*(tsp1_part *) sql__nil(ptoc_Var191->a_cmd_part , "vak641.p" , 17624)).integer.C_1.sp1p_buf , ptoc_Var192->n_pos , (int ) ptoc_Var192->n_length , &invalid);
                        }
                    else
                        {
                        invalid = 1;
                        }
                    }
                if ((char ) sql__setrang(invalid , 0 , 1))
                    {
                    a07_b_put_error(&(*acv) , 6224 , ptoc_Var192->n_pos);
                    }
                }
            else
                {
                *wrong_datatype = 1;
                }
            }
        }
    cccprint("<a641s_literal_v");
    }



int a641u_literal_value(acv, act_node, c2, wrong_datatype)
tak_all_command_glob *acv;
int act_node;
tsp_c2 c2;
char *wrong_datatype;
    {
    tak_all_command_glob *ptoc_Var194;
    struct ptoc_Type59 *ptoc_Var195;
    cccprint("AK641 a641u_lite");
    ptoc_Var194 = &*acv;
    ptoc_Var195 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var194->a_ap_tree , "vak641.p" , 17653))[sql__setrang(act_node , 0 , 32767)];
    if ((int ) ptoc_Var195->n_symb == 48 && (int ) ptoc_Var195->n_length == 2)
        {
        c2[0] = (unsigned char ) sql__setrang((*(tsp1_part *) sql__nil(ptoc_Var194->a_cmd_part , "vak641.p" , 17658)).integer.C_1.sp1p_buf[sql__setrang(ptoc_Var195->n_pos , 1 , 8192000) + -1] , 0 , 255);
        c2[1] = (unsigned char ) sql__setrang((*(tsp1_part *) sql__nil(ptoc_Var194->a_cmd_part , "vak641.p" , 17659)).integer.C_1.sp1p_buf[sql__setrang(ptoc_Var195->n_pos + 1 , 1 , 8192000) + -1] , 0 , 255);
        }
    else
        {
        if ((int ) ptoc_Var195->n_symb == 50)
            {
            a07_b_put_error(&(*acv) , 6224 , ptoc_Var195->n_pos);
            }
        else
            {
            *wrong_datatype = 1;
            }
        }
    cccprint("<a641u_literal_v");
    }



int ak641lrpad(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    char char_field_found;
    unsigned char c;
    tsp_c2 c2;
    tsp_data_type keep_datatype;
    int curr_n;
    int old_n;
    int max_length;
    tsp_int2 l;
    tak00_scolinf colin1;
    char string_pushed;
    tgg_stack_op_build_in operator;
    tak_all_command_glob *ptoc_Var196;
    struct ptoc_Type59 *ptoc_Var197;
    tak_dml_info *ptoc_Var198;
    int ptoc_Var199;
    struct ptoc_Type59 *ptoc_Var200;
    struct ptoc_Type59 *ptoc_Var201;
    struct ptoc_Type59 *ptoc_Var202;
    tak00_scolinf *ptoc_Var203;
    cccprint("AK641 ak641lrpad");
    ptoc_Var196 = &*acv;
    ptoc_Var197 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var196->a_ap_tree , "vak641.p" , 17696))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var198 = &*dmli;
    string_pushed = 0;
    ptoc_Var198->d_ch_datatype = 14;
    if ((int ) ptoc_Var198->d_datatype == 14)
        {
        ptoc_Var198->d_datatype = 16;
        }
    curr_n = (int ) ptoc_Var197->n_lo_level;
    (*colin).sci_len = 0;
    char_field_found = (char ) sql__setrang((ptoc_Var199 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var196->a_ap_tree , "vak641.p" , 17706))[sql__setrang(curr_n , 0 , 32767)].n_symb) == 43 || ptoc_Var199 == 44 || ptoc_Var199 == 45 || ptoc_Var199 == 50 , 0 , 1);
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) ptoc_Var198->d_datatype == 18)
        {
        *wrong_datatype = 1;
        }
    else
        {
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var196->a_return_segm , "vak641.p" , 17713)).integer.C_4.sp1r_returncode == 0)
            {
            if ((int ) ptoc_Var198->d_datatype == 16)
                {
                if ((int ) g01code.ctype == 0)
                    {
                    ptoc_Var198->d_datatype = 2;
                    }
                else
                    {
                    ptoc_Var198->d_datatype = 3;
                    }
                }
            keep_datatype = (unsigned char ) sql__setrang(ptoc_Var198->d_datatype , 0 , 39);
            if ((ptoc_Var199 = (int ) ptoc_Var198->d_datatype) != 2 && ptoc_Var199 != 3 && ptoc_Var199 != 4 && ptoc_Var199 != 24 || (int ) ptoc_Var198->d_ch_datatype != 14)
                {
                char_field_found = 0;
                }
            curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var196->a_ap_tree , "vak641.p" , 17731))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
            if ((ptoc_Var199 = (int ) ptoc_Var197->n_symb) == 73 || ptoc_Var199 == 74)
                {
                ptoc_Var198->d_datatype = 15;
                colin1.sci_len = 0;
                a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
                curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var196->a_ap_tree , "vak641.p" , 17739))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
                }
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var196->a_return_segm , "vak641.p" , 17742)).integer.C_4.sp1r_returncode == 0)
                {
                ptoc_Var200 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var196->a_ap_tree , "vak641.p" , 17745))[sql__setrang(curr_n , 0 , 32767)];
                if ((int ) ptoc_Var200->n_sa_level == 0 && (int ) ptoc_Var196->a_sqlmode == 2)
                    {
                    if ((int ) ptoc_Var200->n_symb == 50)
                        {
                        a07_b_put_error(&(*acv) , 6224 , ptoc_Var200->n_pos);
                        }
                    else
                        {
                        switch ((int ) keep_datatype)
                            {
                            case 10 :
                            case 11 :
                            case 13 :
                            case 2 :
                            case 3 :
                                c = (unsigned char)' ';
                                break;
                            case 4 :
                                c = (unsigned char)'\0';
                                break;
                            case 24 :
                                c = (unsigned char)'\377';
                                c2[0] = (unsigned char)'\0';
                                c2[1] = (unsigned char)' ';
                                break;
                            default :
                                break;
                            }
                        }
                    }
                else
                    {
                    a641s_literal_value(&(*acv) , curr_n , (unsigned char ) sql__setrang(keep_datatype , 0 , 39) , 1 , &string_pushed , &c , &(*wrong_datatype));
                    if (!(char ) sql__setrang(*wrong_datatype , 0 , 1) && (int ) keep_datatype == 24)
                        {
                        _ptoc_ASSIGN(unsigned char , 2, c2, "\0 ")
                        c2[1] = (unsigned char ) sql__setrang(c , 0 , 255);
                        }
                    }
                if (!(char ) sql__setrang(*wrong_datatype , 0 , 1))
                    {
                    if ((int ) keep_datatype != 24 && (int ) c != 255)
                        {
                        a641translate_string_literal(&c , &keep_datatype);
                        }
                    ptoc_Var201 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var196->a_ap_tree , "vak641.p" , 17797))[sql__setrang(curr_n , 0 , 32767)];
                    if ((ptoc_Var199 = (int ) ptoc_Var201->n_symb) != 49 && ptoc_Var199 != 50 || (int ) ptoc_Var196->a_sqlmode != 2)
                        {
                        old_n = curr_n;
                        curr_n = (int ) ptoc_Var201->n_sa_level;
                        }
                    if (curr_n != 0)
                        {
                        ptoc_Var202 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var196->a_ap_tree , "vak641.p" , 17809))[sql__setrang(curr_n , 0 , 32767)];
                        if ((int ) ptoc_Var202->n_symb != 50)
                            {
                            a05_unsigned_int2_get(&(*acv) , ptoc_Var202->n_pos , (short ) sql__setrang(ptoc_Var202->n_length , -32768 , 32767) , 6296 , &l);
                            }
                        else
                            {
                            l = 2;
                            c = (unsigned char)'\377';
                            c2[0] = (unsigned char)'\377';
                            c2[1] = (unsigned char)'\377';
                            }
                        switch ((int ) ptoc_Var196->a_sqlmode)
                            {
                            case 1 :
                                max_length = 4000;
                                break;
                            case 2 :
                                max_length = 8000;
                                break;
                            case 3 :
                                max_length = 8000;
                                break;
                            default :
                                max_length = 254;
                                break;
                            }
                        if ((int ) l < 1 || (int ) l > max_length || (int ) keep_datatype == 24 && (int ) l * 2 > max_length)
                            {
                            a07_b_put_error(&(*acv) , 6296 , ptoc_Var202->n_pos);
                            }
                        }
                    else
                        {
                        if ((char ) sql__setrang(char_field_found , 0 , 1))
                            {
                            l = (short ) sql__setrang((*colin).sci_len , -32768 , 32767);
                            }
                        else
                            {
                            l = 1;
                            t01int4(5 , "char_field**" , (int ) l);
                            a07_b_put_error(&(*acv) , 6125 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var196->a_ap_tree , "vak641.p" , 17853))[sql__setrang(old_n , 0 , 32767)].n_pos + (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var196->a_ap_tree , "vak641.p" , 17854))[sql__setrang(old_n , 0 , 32767)].n_length);
                            }
                        }
                    ak641time_date_to_char_datatype(&ptoc_Var198->d_datatype , &keep_datatype);
                    ptoc_Var203 = &*colin;
                    ptoc_Var203->sci_typ = (unsigned char ) sql__setrang(ptoc_Var198->d_datatype , 0 , 39);
                    ptoc_Var203->sci_frac = 0;
                    ptoc_Var203->sci_len = (short ) sql__setrang(l , -32768 , 32767);
                    if ((int ) ptoc_Var198->d_datatype == 24)
                        {
                        ptoc_Var203->sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var203->sci_len * 2 , -2147483647 , 2147483647 , "vak641.p" , 17867) , -32768 , 32767);
                        }
                    else
                        {
                        ptoc_Var203->sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var203->sci_len , -32768 , 32767 , "vak641.p" , 17869) , -32768 , 32767);
                        }
                    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var196->a_return_segm , "vak641.p" , 17873)).integer.C_4.sp1r_returncode == 0)
                        {
                        if ((int ) ptoc_Var198->d_datatype == 24)
                            {
                            switch ((int ) ptoc_Var197->n_symb)
                                {
                                case 73 :
                                    operator = 61;
                                    break;
                                case 74 :
                                    operator = 62;
                                    break;
                                case 90 :
                                    operator = 59;
                                    break;
                                case 89 :
                                    operator = 60;
                                    break;
                                default :
                                    (unsigned char ) sql__caseerr("vak641.p" , 17878);
                                    break;
                                }
                            a641dbyte_string_set_operator(&(*acv) , (unsigned char ) sql__setrang(operator , 0 , 67) , (int ) (*colin).sci_iolen , sql__ord((int ) (char ) sql__setrang(string_pushed , 0 , 1)) , c2);
                            }
                        else
                            {
                            switch ((int ) ptoc_Var197->n_symb)
                                {
                                case 73 :
                                    operator = 12;
                                    break;
                                case 74 :
                                    operator = 15;
                                    break;
                                case 90 :
                                    operator = 11;
                                    break;
                                case 89 :
                                    operator = 14;
                                    break;
                                default :
                                    (unsigned char ) sql__caseerr("vak641.p" , 17895);
                                    break;
                                }
                            a641string_set_operator(&(*acv) , (unsigned char ) sql__setrang(operator , 0 , 67) , (int ) (*colin).sci_iolen , sql__ord((int ) (char ) sql__setrang(string_pushed , 0 , 1)) , (unsigned char ) sql__setrang(c , 0 , 255) , (unsigned char)'\0');
                            }
                        }
                    }
                }
            }
        }
    cccprint("<ak641lrpad     ");
    }



int ak641to_date(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    tsp_data_type keep_datatype;
    int curr_n;
    int to_date_variant;
    tak00_scolinf colin2;
    tak_all_command_glob *ptoc_Var204;
    struct ptoc_Type59 *ptoc_Var205;
    tak_dml_info *ptoc_Var206;
    int ptoc_Var207;
    tak00_scolinf *ptoc_Var208;
    cccprint("AK641 ak641to_da");
    ptoc_Var204 = &*acv;
    ptoc_Var205 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var204->a_ap_tree , "vak641.p" , 17939))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var206 = &*dmli;
    keep_datatype = (unsigned char ) sql__setrang(ptoc_Var206->d_datatype , 0 , 39);
    ptoc_Var206->d_datatype = 14;
    curr_n = (int ) ptoc_Var205->n_lo_level;
    (*colin).sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var204->a_return_segm , "vak641.p" , 17946)).integer.C_4.sp1r_returncode == 21000)
        {
        (*(tsp1_segment *) sql__nil(ptoc_Var204->a_return_segm , "vak641.p" , 17949)).integer.C_4.sp1r_returncode = 0;
        ptoc_Var206->d_datatype = 16;
        a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
        }
    t01int4(5 , "colin.scityp" , sql__ord((int ) (*colin).sci_typ));
    t01int4(5 , "d_datatype  " , sql__ord((int ) ptoc_Var206->d_datatype));
    if ((int ) (*colin).sci_typ == 18)
        {
        *wrong_datatype = 1;
        }
    else
        {
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var204->a_return_segm , "vak641.p" , 17960)).integer.C_4.sp1r_returncode == 0)
            {
            if ((ptoc_Var207 = (int ) (*colin).sci_typ) == 0 || ptoc_Var207 == 1 || ptoc_Var207 == 12 || ptoc_Var207 == 15)
                {
                a641stack_for_op_b_chr(&(*acv) , &(*dmli) , &(*colin) , 0 , 2);
                }
            if ((int ) (*colin).sci_typ == 24)
                {
                a641string_set_operator(&(*acv) , 66 , (int ) (*colin).sci_len + 1 , 0 , (unsigned char)'\024' , (unsigned char)'\0');
                (*colin).sci_typ = 2;
                (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len + 1) , -32768 , 32767);
                }
            curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var204->a_ap_tree , "vak641.p" , 17977))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
            if (curr_n != 0)
                {
                to_date_variant = 0;
                if ((char ) sql__setrang(g01unicode , 0 , 1) && (int ) g01code.char_default == 20)
                    {
                    ptoc_Var206->d_datatype = 2;
                    }
                else
                    {
                    ptoc_Var206->d_datatype = 14;
                    }
                colin2.sci_len = 0;
                a640not_first_factor(&(*acv) , &(*dmli) , &colin2 , &curr_n);
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var204->a_return_segm , "vak641.p" , 17991)).integer.C_4.sp1r_returncode == 21000)
                    {
                    (*(tsp1_segment *) sql__nil(ptoc_Var204->a_return_segm , "vak641.p" , 17994)).integer.C_4.sp1r_returncode = 0;
                    ptoc_Var206->d_datatype = 16;
                    a640not_first_factor(&(*acv) , &(*dmli) , &colin2 , &curr_n);
                    }
                if ((ptoc_Var207 = (int ) (*colin).sci_typ) != 2 && ptoc_Var207 != 3 && ptoc_Var207 != 14)
                    {
                    a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var204->a_ap_tree , "vak641.p" , 18002))[sql__setrang(curr_n , 0 , 32767)].n_pos);
                    }
                }
            else
                {
                to_date_variant = 1;
                }
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var204->a_return_segm , "vak641.p" , 18008)).integer.C_4.sp1r_returncode == 0)
                {
                if (to_date_variant == 1)
                    {
                    a641f_push_format(&(*acv) , &(*dmli));
                    to_date_variant = 0;
                    }
                a641d_push_date(&(*acv) , &(*dmli));
                a641l_push_language(&(*acv) , &(*dmli));
                ptoc_Var208 = &*colin;
                a641string_set_operator(&(*acv) , 40 , 21 , to_date_variant , (unsigned char)'\0' , (unsigned char)'\0');
                ptoc_Var208->sci_frac = 0;
                ptoc_Var208->sci_len = 20;
                ptoc_Var208->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var208->sci_len + 1) , -32768 , 32767);
                ptoc_Var208->sci_typ = 13;
                }
            ptoc_Var206->d_datatype = 13;
            }
        }
    cccprint("<ak641to_date   ");
    }



int ak641replace_translate(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    char second_param;
    tsp_data_type keep_datatype;
    int curr_n;
    int max_length;
    tak00_scolinf colin1;
    tak00_scolinf colin2;
    tak_all_command_glob *ptoc_Var209;
    struct ptoc_Type59 *ptoc_Var210;
    tak_dml_info *ptoc_Var211;
    cccprint("AK641 ak641repla");
    ptoc_Var209 = &*acv;
    ptoc_Var210 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var209->a_ap_tree , "vak641.p" , 18058))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var211 = &*dmli;
    keep_datatype = (unsigned char ) sql__setrang(ptoc_Var211->d_datatype , 0 , 39);
    if ((int ) ptoc_Var211->d_datatype == 10 || (int ) ptoc_Var211->d_datatype == 11 || (int ) ptoc_Var211->d_datatype == 13)
        {
        if ((int ) g01code.ctype == 0)
            {
            ptoc_Var211->d_datatype = 2;
            }
        else
            {
            ptoc_Var211->d_datatype = 3;
            }
        }
    else
        {
        if ((int ) ptoc_Var211->d_datatype == 14)
            {
            ptoc_Var211->d_datatype = 16;
            }
        }
    ptoc_Var211->d_ch_datatype = 14;
    curr_n = (int ) ptoc_Var210->n_lo_level;
    (*colin).sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) ptoc_Var211->d_datatype == 18)
        {
        *wrong_datatype = 1;
        }
    else
        {
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var209->a_return_segm , "vak641.p" , 18085)).integer.C_4.sp1r_returncode == 0)
            {
            curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var209->a_ap_tree , "vak641.p" , 18088))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
            colin1.sci_len = 0;
            a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
            curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var209->a_ap_tree , "vak641.p" , 18091))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
            colin2.sci_len = 0;
            if (curr_n != 0)
                {
                a640not_first_factor(&(*acv) , &(*dmli) , &colin2 , &curr_n);
                if ((int ) colin2.sci_len > (int ) colin1.sci_len && (int ) ptoc_Var210->n_symb == 166)
                    {
                    (*colin).sci_len = (short ) sql__setrang((short ) ((int ) (*colin).sci_len / (int ) colin1.sci_len * (int ) colin2.sci_len) , -32768 , 32767);
                    }
                second_param = 1;
                switch ((int ) ptoc_Var209->a_sqlmode)
                    {
                    case 1 :
                        max_length = 4000;
                        break;
                    case 2 :
                        max_length = 8000;
                        break;
                    case 3 :
                        max_length = 8000;
                        break;
                    default :
                        max_length = 254;
                        break;
                    }
                if ((int ) (*colin).sci_len > max_length || (int ) (*colin).sci_typ == 24 && (int ) (*colin).sci_len * 2 > max_length)
                    {
                    a07_b_put_error(&(*acv) , 6374 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var209->a_ap_tree , "vak641.p" , 18120))[sql__setrang(act_node , 0 , 32767)].n_pos);
                    }
                }
            else
                {
                second_param = 0;
                }
            }
        }
    (*colin).sci_frac = 0;
    (*colin).sci_typ = (unsigned char ) sql__setrang(ptoc_Var211->d_datatype , 0 , 39);
    if ((int ) (*colin).sci_typ == 24)
        {
        (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len * 2 + 1) , -32768 , 32767);
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var209->a_return_segm , "vak641.p" , 18135)).integer.C_4.sp1r_returncode == 0)
            {
            if ((int ) ptoc_Var210->n_symb == 166)
                {
                if ((char ) sql__setrang(second_param , 0 , 1))
                    {
                    a65_set_operator(&(*acv) , 119);
                    }
                else
                    {
                    a65_set_operator(&(*acv) , 120);
                    }
                }
            else
                {
                a65_set_operator(&(*acv) , 121);
                }
            }
        }
    else
        {
        (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len + 1) , -32768 , 32767);
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var209->a_return_segm , "vak641.p" , 18155)).integer.C_4.sp1r_returncode == 0)
            {
            if ((int ) ptoc_Var210->n_symb == 166)
                {
                if ((char ) sql__setrang(second_param , 0 , 1))
                    {
                    a65_set_operator(&(*acv) , 25);
                    }
                else
                    {
                    a65_set_operator(&(*acv) , 24);
                    }
                }
            else
                {
                a65_set_operator(&(*acv) , 35);
                }
            }
        }
    cccprint("<ak641replace_tr");
    }



int ak641trim(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    tak00_scolinf colin1;
    int curr_n;
    int para_count;
    tgg_stack_op_build_in build_in_func;
    tsp_int2 res_len;
    tsp_int2 old_sci_len;
    tak_all_command_glob *ptoc_Var212;
    struct ptoc_Type59 *ptoc_Var213;
    tak_dml_info *ptoc_Var214;
    tak00_scolinf *ptoc_Var215;
    int ptoc_Var216;
    cccprint("AK641 ak641trim ");
    ptoc_Var212 = &*acv;
    ptoc_Var213 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var212->a_ap_tree , "vak641.p" , 18196))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var214 = &*dmli;
    ptoc_Var215 = &*colin;
    para_count = 1;
    ptoc_Var214->d_datatype = 14;
    curr_n = (int ) ptoc_Var213->n_lo_level;
    ptoc_Var215->sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var212->a_return_segm , "vak641.p" , 18203)).integer.C_4.sp1r_returncode == 21000)
        {
        (*(tsp1_segment *) sql__nil(ptoc_Var212->a_return_segm , "vak641.p" , 18206)).integer.C_4.sp1r_returncode = 0;
        a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var212->a_return_segm , "vak641.p" , 18211)).integer.C_4.sp1r_returncode == 0)
        {
        if ((ptoc_Var216 = (int ) ptoc_Var214->d_datatype) == 4 || ptoc_Var216 == 18)
            {
            *wrong_datatype = 1;
            }
        else
            {
            curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var212->a_ap_tree , "vak641.p" , 18218))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
            if (curr_n != 0)
                {
                colin1.sci_len = 0;
                if ((int ) ptoc_Var214->d_datatype != 24)
                    {
                    ptoc_Var214->d_datatype = 2;
                    }
                a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var212->a_return_segm , "vak641.p" , 18228)).integer.C_4.sp1r_returncode == 0)
                    {
                    if ((int ) (*colin).sci_typ != 24 && ((ptoc_Var216 = (int ) colin1.sci_typ) == 2 || ptoc_Var216 == 3) || (int ) (*colin).sci_typ == 24 && (int ) colin1.sci_typ == 24)
                        {
                        para_count = 2;
                        }
                    else
                        {
                        *wrong_datatype = 1;
                        }
                    }
                }
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var212->a_return_segm , "vak641.p" , 18243)).integer.C_4.sp1r_returncode == 0 && !(char ) sql__setrang(*wrong_datatype , 0 , 1))
                {
                if ((ptoc_Var216 = (int ) ptoc_Var215->sci_typ) == 0 || ptoc_Var216 == 1 || ptoc_Var216 == 12 || ptoc_Var216 == 15)
                    {
                    old_sci_len = (short ) sql__setrang(ptoc_Var215->sci_len , -32768 , 32767);
                    }
                else
                    {
                    old_sci_len = 0;
                    }
                if ((ptoc_Var216 = (int ) (*colin).sci_typ) == 2 || ptoc_Var216 == 3 || ptoc_Var216 == 24)
                    {
                    ptoc_Var214->d_datatype = (unsigned char ) sql__setrang(ptoc_Var215->sci_typ , 0 , 39);
                    }
                else
                    {
                    if ((int ) g01code.ctype == 0)
                        {
                        ptoc_Var214->d_datatype = 2;
                        }
                    else
                        {
                        ptoc_Var214->d_datatype = 3;
                        }
                    }
                a641_get_length(&(*colin) , &res_len , &(*wrong_datatype));
                if (!(char ) sql__setrang(*wrong_datatype , 0 , 1))
                    {
                    ptoc_Var215->sci_typ = (unsigned char ) sql__setrang(ptoc_Var214->d_datatype , 0 , 39);
                    ptoc_Var215->sci_len = (short ) sql__setrang(res_len , -32768 , 32767);
                    if ((int ) ptoc_Var214->d_datatype == 24)
                        {
                        ptoc_Var215->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var215->sci_len * 2 + 1) , -32768 , 32767);
                        switch ((int ) ptoc_Var213->n_symb)
                            {
                            case 68 :
                                build_in_func = 63;
                                break;
                            case 162 :
                                build_in_func = 65;
                                break;
                            case 163 :
                                build_in_func = 64;
                                break;
                            default :
                                (unsigned char ) sql__caseerr("vak641.p" , 18275);
                                break;
                            }
                        }
                    else
                        {
                        ptoc_Var215->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var215->sci_len + 1) , -32768 , 32767);
                        switch ((int ) ptoc_Var213->n_symb)
                            {
                            case 68 :
                                build_in_func = 19;
                                break;
                            case 162 :
                                build_in_func = 13;
                                break;
                            case 163 :
                                build_in_func = 16;
                                break;
                            default :
                                (unsigned char ) sql__caseerr("vak641.p" , 18288);
                                break;
                            }
                        }
                    a641string_set_operator(&(*acv) , (unsigned char ) sql__setrang(build_in_func , 0 , 67) , sql__ord((int ) ptoc_Var212->a_sqlmode) , (int ) ptoc_Var215->sci_frac , (unsigned char ) sql__setrang((unsigned char ) para_count , 0 , 255) , (unsigned char ) sql__setrang((unsigned char ) old_sci_len , 0 , 255));
                    ptoc_Var215->sci_frac = 0;
                    }
                }
            }
        }
    cccprint("<ak641trim      ");
    }



int ak641makedate(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    int curr_n;
    tak00_scolinf colin1;
    tak_all_command_glob *ptoc_Var217;
    struct ptoc_Type59 *ptoc_Var218;
    tak_dml_info *ptoc_Var219;
    tak00_scolinf *ptoc_Var220;
    cccprint("AK641 ak641maked");
    ptoc_Var217 = &*acv;
    ptoc_Var218 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var217->a_ap_tree , "vak641.p" , 18330))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var219 = &*dmli;
    ptoc_Var219->d_datatype = 15;
    curr_n = (int ) ptoc_Var218->n_lo_level;
    (*colin).sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var217->a_return_segm , "vak641.p" , 18336)).integer.C_4.sp1r_returncode == 0)
        {
        curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var217->a_ap_tree , "vak641.p" , 18339))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
        colin1.sci_len = 0;
        a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var217->a_return_segm , "vak641.p" , 18342)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var219->d_datatype = 10;
            ptoc_Var220 = &*colin;
            ptoc_Var220->sci_len = 8;
            ptoc_Var220->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var220->sci_len + 1) , -32768 , 32767);
            ptoc_Var220->sci_frac = 0;
            ptoc_Var220->sci_typ = 10;
            a65_set_operator(&(*acv) , 57);
            }
        }
    cccprint("<ak641makedate  ");
    }



int ak641maketime(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    int curr_n;
    tak00_scolinf colin1;
    tak_all_command_glob *ptoc_Var221;
    struct ptoc_Type59 *ptoc_Var222;
    tak_dml_info *ptoc_Var223;
    tak00_scolinf *ptoc_Var224;
    cccprint("AK641 ak641maket");
    ptoc_Var221 = &*acv;
    ptoc_Var222 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var221->a_ap_tree , "vak641.p" , 18377))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var223 = &*dmli;
    ptoc_Var223->d_datatype = 15;
    curr_n = (int ) ptoc_Var222->n_lo_level;
    (*colin).sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var221->a_return_segm , "vak641.p" , 18383)).integer.C_4.sp1r_returncode == 0)
        {
        curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var221->a_ap_tree , "vak641.p" , 18386))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
        colin1.sci_len = 0;
        a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var221->a_return_segm , "vak641.p" , 18389)).integer.C_4.sp1r_returncode == 0)
            {
            curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var221->a_ap_tree , "vak641.p" , 18392))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
            colin1.sci_len = 0;
            a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var221->a_return_segm , "vak641.p" , 18395)).integer.C_4.sp1r_returncode == 0)
                {
                ptoc_Var223->d_datatype = 11;
                ptoc_Var224 = &*colin;
                ptoc_Var224->sci_len = 8;
                ptoc_Var224->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var224->sci_len + 1) , -32768 , 32767);
                ptoc_Var224->sci_frac = 0;
                ptoc_Var224->sci_typ = 11;
                a65_set_operator(&(*acv) , 58);
                }
            }
        }
    cccprint("<ak641maketime  ");
    }



int ak641months_between(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    int curr_n;
    tak00_scolinf colin1;
    tak_all_command_glob *ptoc_Var225;
    struct ptoc_Type59 *ptoc_Var226;
    tak_dml_info *ptoc_Var227;
    tak00_scolinf *ptoc_Var228;
    cccprint("AK641 ak641month");
    ptoc_Var225 = &*acv;
    ptoc_Var226 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var225->a_ap_tree , "vak641.p" , 18432))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var227 = &*dmli;
    ptoc_Var227->d_datatype = 13;
    curr_n = (int ) ptoc_Var226->n_lo_level;
    (*colin).sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var225->a_return_segm , "vak641.p" , 18438)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) (*colin).sci_typ != 13)
            {
            a641check_datetime(&(*acv) , &(*dmli) , 13);
            }
        curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var225->a_ap_tree , "vak641.p" , 18445))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
        colin1.sci_len = 0;
        ptoc_Var227->d_datatype = 13;
        a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var225->a_return_segm , "vak641.p" , 18449)).integer.C_4.sp1r_returncode == 0)
            {
            if ((int ) (*colin).sci_typ != 13)
                {
                a641check_datetime(&(*acv) , &(*dmli) , 13);
                }
            ptoc_Var227->d_datatype = 15;
            ptoc_Var228 = &*colin;
            ptoc_Var228->sci_len = 38;
            ptoc_Var228->sci_frac = -1;
            ptoc_Var228->sci_typ = 1;
            ptoc_Var228->sci_iolen = (short ) sql__setrang((short ) (((int ) ptoc_Var228->sci_len + 1) / 2 + 2) , -32768 , 32767);
            a65_set_operator(&(*acv) , 59);
            }
        }
    cccprint("<ak641months_bet");
    }



int ak641noround(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    int curr_n;
    tak_all_command_glob *ptoc_Var229;
    struct ptoc_Type59 *ptoc_Var230;
    tak_dml_info *ptoc_Var231;
    struct tgg00_MessBlock *ptoc_Var232;
    tgg00_QualBuf *ptoc_Var233;
    tgg00_StackEntry *ptoc_Var234;
    cccprint("AK641 ak641norou");
    ptoc_Var229 = &*acv;
    ptoc_Var230 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var229->a_ap_tree , "vak641.p" , 18487))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var231 = &*dmli;
    ptoc_Var231->d_datatype = 15;
    ptoc_Var231->d_const_value_expr = 0;
    (*colin).sci_len = 0;
    curr_n = (int ) ptoc_Var230->n_lo_level;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var229->a_return_segm , "vak641.p" , 18494)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var232 = &ptoc_Var229->a_mblock;
        ptoc_Var233 = (tgg00_QualBuf *) sql__nil(ptoc_Var232->integer.C_4.mb_qual , "vak641.p" , 18496);
        if ((int ) ptoc_Var233->integer.C_2.mfirst_free > (int ) ptoc_Var232->integer.C_2.mb_st_max)
            {
            a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var232->integer.C_2.mb_st_max);
            }
        else
            {
            ptoc_Var234 = &(*(tgg00_StackList *) sql__nil(ptoc_Var232->integer.C_5.mb_st , "vak641.p" , 18501))[sql__setrang((int ) ptoc_Var233->integer.C_2.mfirst_free , 1 , 32767) + -1];
            ptoc_Var234->integer.C_1.etype = 40;
            ptoc_Var234->integer.C_1.eop = 0;
            ptoc_Var234->integer.C_1.epos = (short ) sql__setrang((*colin).sci_len , -32768 , 32767);
            ptoc_Var234->integer.C_1.elen_var = (short ) sql__setrang((*colin).sci_frac , -32768 , 32767);
            ptoc_Var234->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
            ptoc_Var234->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
            ptoc_Var233->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var233->integer.C_2.mfirst_free , -32768 , 32767 , "vak641.p" , 18509) , -32768 , 32767);
            ptoc_Var233->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var233->integer.C_2.mqual_cnt , -32768 , 32767 , "vak641.p" , 18510) , -32768 , 32767);
            }
        }
    cccprint("<ak641noround   ");
    }



int ak641num(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    int curr_n;
    tak_all_command_glob *ptoc_Var235;
    struct ptoc_Type59 *ptoc_Var236;
    tak_dml_info *ptoc_Var237;
    int ptoc_Var238;
    tak00_scolinf *ptoc_Var239;
    cccprint("AK641 ak641num  ");
    ptoc_Var235 = &*acv;
    ptoc_Var236 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var235->a_ap_tree , "vak641.p" , 18533))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var237 = &*dmli;
    ptoc_Var237->d_datatype = 14;
    curr_n = (int ) ptoc_Var236->n_lo_level;
    (*colin).sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var235->a_return_segm , "vak641.p" , 18539)).integer.C_4.sp1r_returncode == 21000)
        {
        (*(tsp1_segment *) sql__nil(ptoc_Var235->a_return_segm , "vak641.p" , 18548)).integer.C_4.sp1r_returncode = 0;
        ptoc_Var237->d_datatype = 16;
        a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var235->a_return_segm , "vak641.p" , 18553)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) ptoc_Var236->n_symb == 75 && ((ptoc_Var238 = (int ) ptoc_Var237->d_datatype) == 18 || ptoc_Var238 == 4) || (int ) ptoc_Var236->n_symb == 161 && (int ) ptoc_Var237->d_datatype != 2 && (int ) ptoc_Var237->d_datatype != 3 && (int ) ptoc_Var237->d_datatype != 24)
            {
            *wrong_datatype = 1;
            }
        else
            {
            if ((int ) ptoc_Var237->d_datatype == 15)
                {
                ptoc_Var237->d_const_value_expr = 0;
                }
            else
                {
                if ((ptoc_Var238 = (int ) ptoc_Var237->d_datatype) == 2 || ptoc_Var238 == 10 || ptoc_Var238 == 11 || ptoc_Var238 == 13)
                    {
                    if ((int ) g01code.ctype == 1)
                        {
                        a65_set_operator(&(*acv) , 7);
                        }
                    }
                else
                    {
                    if ((int ) ptoc_Var237->d_datatype == 3)
                        {
                        if ((int ) g01code.ctype == 0)
                            {
                            a65_set_operator(&(*acv) , 2);
                            }
                        }
                    else
                        {
                        if ((int ) ptoc_Var237->d_datatype == 24)
                            {
                            a641string_set_operator(&(*acv) , 66 , (int ) (*colin).sci_len + 1 , 0 , (unsigned char)'\024' , (unsigned char)'\0');
                            }
                        }
                    }
                ptoc_Var237->d_datatype = 15;
                a65_set_operator(&(*acv) , 17);
                ptoc_Var239 = &*colin;
                ptoc_Var239->sci_len = 38;
                ptoc_Var239->sci_frac = -1;
                ptoc_Var239->sci_iolen = (short ) sql__setrang((short ) (((int ) ptoc_Var239->sci_len + 1) / 2 + 2) , -32768 , 32767);
                ptoc_Var239->sci_typ = 12;
                }
            }
        }
    cccprint("<ak641num       ");
    }



int ak641name(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    int curr_n;
    tak_all_command_glob *ptoc_Var240;
    struct ptoc_Type59 *ptoc_Var241;
    tak_dml_info *ptoc_Var242;
    tak00_scolinf *ptoc_Var243;
    cccprint("AK641 ak641name ");
    ptoc_Var240 = &*acv;
    ptoc_Var241 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var240->a_ap_tree , "vak641.p" , 18637))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var242 = &*dmli;
    curr_n = (int ) ptoc_Var241->n_lo_level;
    ak641type_or_timestamp_parameter(&(*acv) , &(*dmli) , &(*colin) , &curr_n , 10);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var240->a_return_segm , "vak641.p" , 18641)).integer.C_4.sp1r_returncode == 0)
        {
        a641l_push_language(&(*acv) , &(*dmli));
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var240->a_return_segm , "vak641.p" , 18645)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var243 = &*colin;
            ptoc_Var243->sci_frac = 0;
            ptoc_Var243->sci_len = 12;
            if ((char ) sql__setrang(g01unicode , 0 , 1))
                {
                ptoc_Var243->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var243->sci_len * 2 + 1) , -32768 , 32767);
                ptoc_Var243->sci_typ = 24;
                ptoc_Var242->d_datatype = 24;
                }
            else
                {
                ptoc_Var243->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var243->sci_len + 1) , -32768 , 32767);
                ptoc_Var243->sci_typ = 2;
                ptoc_Var242->d_datatype = 2;
                }
            if ((int ) ptoc_Var241->n_symb == 220)
                {
                a641string_set_operator(&(*acv) , 57 , (int ) ptoc_Var243->sci_iolen , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                }
            else
                {
                a641string_set_operator(&(*acv) , 57 , (int ) ptoc_Var243->sci_iolen , 0 , (unsigned char)'\001' , (unsigned char)'\0');
                }
            }
        }
    cccprint("<ak641name      ");
    }



int ak641of(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    int curr_n;
    tak_all_command_glob *ptoc_Var244;
    struct ptoc_Type59 *ptoc_Var245;
    tak_dml_info *ptoc_Var246;
    tak00_scolinf *ptoc_Var247;
    cccprint("AK641 ak641of   ");
    ptoc_Var244 = &*acv;
    ptoc_Var245 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var244->a_ap_tree , "vak641.p" , 18695))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var246 = &*dmli;
    curr_n = (int ) ptoc_Var245->n_lo_level;
    ak641type_or_timestamp_parameter(&(*acv) , &(*dmli) , &(*colin) , &curr_n , 10);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var244->a_return_segm , "vak641.p" , 18699)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var246->d_datatype = 15;
        ptoc_Var247 = &*colin;
        switch ((int ) ptoc_Var245->n_symb)
            {
            case 100 :
                ptoc_Var247->sci_len = 1;
                a65_set_operator(&(*acv) , 53);
                break;
            case 101 :
                ptoc_Var247->sci_len = 2;
                a65_set_operator(&(*acv) , 66);
                break;
            case 102 :
                ptoc_Var247->sci_len = 3;
                a65_set_operator(&(*acv) , 54);
                break;
            case 219 :
                ptoc_Var247->sci_len = 2;
                a641string_set_operator(&(*acv) , 56 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                break;
            default :
                (unsigned char ) sql__caseerr("vak641.p" , 18705);
                break;
            }
        ptoc_Var247->sci_iolen = (short ) sql__setrang((short ) (((int ) ptoc_Var247->sci_len + 1) / 2 + 2) , -32768 , 32767);
        ptoc_Var247->sci_frac = 0;
        ptoc_Var247->sci_typ = 0;
        }
    cccprint("<ak641of        ");
    }



int ak641put_maxlength(acv, curr_n, symb, first_int, second_int)
tak_all_command_glob *acv;
int *curr_n;
tak_sc_symbol symb;
tsp_int2 *first_int;
tsp_int2 *second_int;
    {
    char undef;
    tak_all_command_glob *ptoc_Var248;
    struct ptoc_Type59 *ptoc_Var249;
    struct tgg00_MessBlock *ptoc_Var250;
    tgg00_QualBuf *ptoc_Var251;
    tgg00_StackEntry *ptoc_Var252;
    cccprint("AK641 ak641put_m");
    undef = 0;
    ptoc_Var248 = &*acv;
    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak641.p" , 18760))[sql__setrang(*curr_n , 0 , 32767)].n_sa_level != 0)
        {
        *curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak641.p" , 18763))[sql__setrang(*curr_n , 0 , 32767)].n_sa_level;
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak641.p" , 18764))[sql__setrang(*curr_n , 0 , 32767)].n_symb == 49)
            {
            a05_unsigned_int2_get(&(*acv) , (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak641.p" , 18767))[sql__setrang(*curr_n , 0 , 32767)].n_pos , (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak641.p" , 18768))[sql__setrang(*curr_n , 0 , 32767)].n_length , -32768 , 32767) , 6296 , &(*first_int));
            if ((int ) *first_int < 1 || (int ) *first_int > 38)
                {
                a07_b_put_error(&(*acv) , 6296 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak641.p" , 18772))[sql__setrang(*curr_n , 0 , 32767)].n_pos);
                }
            else
                {
                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak641.p" , 18774))[sql__setrang(*curr_n , 0 , 32767)].n_sa_level != 0)
                    {
                    *curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak641.p" , 18777))[sql__setrang(*curr_n , 0 , 32767)].n_sa_level;
                    ptoc_Var249 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak641.p" , 18778))[sql__setrang(*curr_n , 0 , 32767)];
                    if ((int ) ptoc_Var249->n_symb == 49)
                        {
                        a05_unsigned_int2_get(&(*acv) , ptoc_Var249->n_pos , (short ) sql__setrang(ptoc_Var249->n_length , -32768 , 32767) , 6296 , &(*second_int));
                        if ((int ) *second_int < 0 || (int ) *second_int > (int ) *first_int)
                            {
                            a07_b_put_error(&(*acv) , 6296 , ptoc_Var249->n_pos);
                            }
                        }
                    else
                        {
                        if ((int ) ptoc_Var249->n_symb == 50)
                            {
                            undef = 1;
                            }
                        else
                            {
                            *second_int = 0;
                            }
                        }
                    }
                else
                    {
                    *second_int = 0;
                    }
                }
            }
        else
            {
            undef = 1;
            }
        }
    else
        {
        *first_int = 38;
        *second_int = 0;
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var248->a_return_segm , "vak641.p" , 18817)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var250 = &ptoc_Var248->a_mblock;
        ptoc_Var251 = (tgg00_QualBuf *) sql__nil(ptoc_Var250->integer.C_4.mb_qual , "vak641.p" , 18819);
        if ((int ) ptoc_Var251->integer.C_2.mfirst_free >= (int ) ptoc_Var250->integer.C_2.mb_st_max)
            {
            a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var250->integer.C_2.mb_st_max);
            }
        else
            {
            ptoc_Var252 = &(*(tgg00_StackList *) sql__nil(ptoc_Var250->integer.C_5.mb_st , "vak641.p" , 18826))[sql__setrang((int ) ptoc_Var251->integer.C_2.mfirst_free , 1 , 32767) + -1];
            if ((int ) symb == 33)
                {
                ptoc_Var252->integer.C_1.etype = 26;
                ptoc_Var252->integer.C_1.eop = 9;
                }
            else
                {
                ptoc_Var252->integer.C_1.etype = 55;
                ptoc_Var252->integer.C_4.eop_build_in = 67;
                *second_int = -1;
                }
            if ((char ) sql__setrang(undef , 0 , 1))
                {
                if ((int ) symb == 33)
                    {
                    ptoc_Var252->integer.C_1.epos = 0;
                    ptoc_Var252->integer.C_1.elen_var = 0;
                    ptoc_Var252->integer.C_1.ecol_tab[0] = (unsigned char)'\377';
                    ptoc_Var252->integer.C_1.ecol_tab[1] = (unsigned char)'\377';
                    *first_int = 38;
                    *second_int = 0;
                    }
                else
                    {
                    a07_b_put_error(&(*acv) , 4320 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak641.p" , 18855))[sql__setrang(*curr_n , 0 , 32767)].n_pos);
                    }
                }
            else
                {
                ptoc_Var252->integer.C_1.epos = (short ) sql__setrang(*first_int , -32768 , 32767);
                ptoc_Var252->integer.C_1.elen_var = (short ) sql__setrang(*second_int , -32768 , 32767);
                ptoc_Var252->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                ptoc_Var252->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                }
            ptoc_Var251->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var251->integer.C_2.mqual_cnt , -32768 , 32767 , "vak641.p" , 18867) , -32768 , 32767);
            ptoc_Var251->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var251->integer.C_2.mfirst_free , -32768 , 32767 , "vak641.p" , 18868) , -32768 , 32767);
            }
        }
    cccprint("<ak641put_maxlen");
    }



int ak641put_roundlength(acv, curr_n, colin)
tak_all_command_glob *acv;
int *curr_n;
tak00_scolinf *colin;
    {
    int old_frac;
    tsp_int2 second_int;
    int next_n;
    tak_all_command_glob *ptoc_Var253;
    tak00_scolinf *ptoc_Var254;
    tak00_scolinf *ptoc_Var255;
    cccprint("AK641 ak641put_r");
    ptoc_Var253 = &*acv;
    second_int = (short ) sql__setrang((*colin).sci_frac , -32768 , 32767);
    if ((int ) second_int == -1)
        {
        second_int = (short ) sql__setrang((*colin).sci_len , -32768 , 32767);
        }
    next_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var253->a_ap_tree , "vak641.p" , 18900))[sql__setrang(*curr_n , 0 , 32767)].n_lo_level;
    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var253->a_ap_tree , "vak641.p" , 18901))[sql__setrang(*curr_n , 0 , 32767)].n_symb == 49 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var253->a_ap_tree , "vak641.p" , 18902))[sql__setrang(*curr_n , 0 , 32767)].n_sa_level == 0 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var253->a_ap_tree , "vak641.p" , 18903))[sql__setrang(*curr_n , 0 , 32767)].n_lo_level == 0 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var253->a_ap_tree , "vak641.p" , 18904))[sql__setrang(*curr_n , 0 , 32767)].n_symb == 11 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var253->a_ap_tree , "vak641.p" , 18905))[sql__setrang(next_n , 0 , 32767)].n_symb == 49 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var253->a_ap_tree , "vak641.p" , 18906))[sql__setrang(next_n , 0 , 32767)].n_sa_level == 0 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var253->a_ap_tree , "vak641.p" , 18907))[sql__setrang(next_n , 0 , 32767)].n_lo_level == 0)
        {
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var253->a_ap_tree , "vak641.p" , 18910))[sql__setrang(*curr_n , 0 , 32767)].n_symb == 11)
            {
            *curr_n = next_n;
            }
        old_frac = (int ) second_int;
        a05_unsigned_int2_get(&(*acv) , (*(tak_ap_max_tree *) sql__nil(ptoc_Var253->a_ap_tree , "vak641.p" , 18915))[sql__setrang(*curr_n , 0 , 32767)].n_pos , (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var253->a_ap_tree , "vak641.p" , 18916))[sql__setrang(*curr_n , 0 , 32767)].n_length , -32768 , 32767) , 6296 , &second_int);
        if ((int ) second_int < 0)
            {
            a07_b_put_error(&(*acv) , 6296 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var253->a_ap_tree , "vak641.p" , 18920))[sql__setrang(*curr_n , 0 , 32767)].n_pos);
            }
        else
            {
            if ((int ) second_int > old_frac)
                {
                second_int = (short ) sql__setrang((short ) old_frac , -32768 , 32767);
                }
            }
        }
    if ((int ) (*colin).sci_frac != -1)
        {
        ptoc_Var254 = &*colin;
        ptoc_Var254->sci_frac = (short ) sql__setrang(second_int , -32768 , 32767);
        if ((int ) second_int != 0)
            {
            ptoc_Var254->sci_len = (short ) sql__setrang((short ) ((int ) ptoc_Var254->sci_len - (int ) ptoc_Var254->sci_frac + (int ) second_int) , -32768 , 32767);
            ptoc_Var254->sci_iolen = (short ) sql__setrang((short ) (((int ) ptoc_Var254->sci_len + 1) / 2 + 2) , -32768 , 32767);
            }
        }
    ptoc_Var255 = &*colin;
    t01int4(5 , "sci_len     " , (int ) ptoc_Var255->sci_len);
    t01int4(5 , "sci_iolen   " , (int ) ptoc_Var255->sci_iolen);
    t01int4(5 , "sci_frac    " , (int ) ptoc_Var255->sci_frac);
    cccprint("<ak641put_roundl");
    }

static
  tak00_colpropset ptoc_Var256 = 
  {
  (unsigned char)'\002', (unsigned char)'\0'
  };


int ak641soundex(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    int curr_n;
    tsp_data_type keep_datatype;
    tak_all_command_glob *ptoc_Var257;
    struct ptoc_Type59 *ptoc_Var258;
    tak_dml_info *ptoc_Var259;
    cccprint("AK641 ak641sound");
    ptoc_Var257 = &*acv;
    ptoc_Var258 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var257->a_ap_tree , "vak641.p" , 18972))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var259 = &*dmli;
    curr_n = (int ) ptoc_Var258->n_lo_level;
    keep_datatype = (unsigned char ) sql__setrang(ptoc_Var259->d_datatype , 0 , 39);
    ptoc_Var259->d_ch_datatype = 14;
    if ((int ) ptoc_Var259->d_datatype == 14 || (int ) ptoc_Var259->d_datatype == 24)
        {
        ptoc_Var259->d_datatype = 2;
        }
    (*colin).sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var257->a_return_segm , "vak641.p" , 18984)).integer.C_4.sp1r_returncode == 0)
        {
        if (((int ) (*colin).sci_typ == 2 || (int ) (*colin).sci_typ == 3) && (int ) ptoc_Var259->d_ch_datatype == 14)
            {
            a65_set_operator(&(*acv) , 26);
            (*colin).sci_len = 4;
            if ((int ) keep_datatype == 24)
                {
                (*colin).sci_iolen = 9;
                a641string_set_operator(&(*acv) , 66 , (int ) (*colin).sci_iolen , 0 , (unsigned char)'\0' , (unsigned char)'\024');
                ptoc_Var259->d_datatype = 24;
                }
            else
                {
                (*colin).sci_iolen = 5;
                }
            (*colin).sci_frac = 0;
            (*colin).sci_typ = (unsigned char ) sql__setrang(ptoc_Var259->d_datatype , 0 , 39);
            _ptoc_MOVE(unsigned char , 2, (*colin).sci_cprops, ptoc_Var256);
            }
        else
            {
            *wrong_datatype = 1;
            }
        }
    cccprint("<ak641soundex   ");
    }



int ak641substr(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    char substr_from_null;
    tsp_data_type keep_datatype;
    tsp_data_type keep1_datatype;
    int curr_n;
    int index;
    int max_length;
    int old_n;
    tsp_int2 first_int;
    tsp_int2 second_int;
    tsp_num_error n_err;
    tgg_stack_op_build_in op;
    tak00_scolinf colin1;
    tak_all_command_glob *ptoc_Var260;
    struct ptoc_Type59 *ptoc_Var261;
    tak_dml_info *ptoc_Var262;
    struct tgg00_MessBlock *ptoc_Var263;
    tgg00_QualBuf *ptoc_Var264;
    struct tgg00_MessBlock *ptoc_Var265;
    tgg00_QualBuf *ptoc_Var266;
    int ptoc_Var267;
    cccprint("AK641 ak641subst");
    ptoc_Var260 = &*acv;
    ptoc_Var261 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var260->a_ap_tree , "vak641.p" , 19044))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var262 = &*dmli;
    keep1_datatype = (unsigned char ) sql__setrang(ptoc_Var262->d_datatype , 0 , 39);
    if ((int ) ptoc_Var262->d_datatype == 10 || (int ) ptoc_Var262->d_datatype == 11 || (int ) ptoc_Var262->d_datatype == 13)
        {
        if ((int ) g01code.ctype == 0)
            {
            ptoc_Var262->d_datatype = 2;
            }
        else
            {
            ptoc_Var262->d_datatype = 3;
            }
        }
    else
        {
        if ((int ) ptoc_Var262->d_datatype == 14)
            {
            ptoc_Var262->d_datatype = 16;
            }
        }
    curr_n = (int ) ptoc_Var261->n_lo_level;
    (*colin).sci_len = 0;
    substr_from_null = (char ) sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var260->a_ap_tree , "vak641.p" , 19065))[sql__setrang(curr_n , 0 , 32767)].n_proc == 0 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var260->a_ap_tree , "vak641.p" , 19066))[sql__setrang(curr_n , 0 , 32767)].n_symb == 50 , 0 , 1);
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var260->a_return_segm , "vak641.p" , 19068)).integer.C_4.sp1r_returncode == 0)
        {
        keep_datatype = (unsigned char ) sql__setrang(ptoc_Var262->d_datatype , 0 , 39);
        ptoc_Var262->d_datatype = 15;
        colin1.sci_len = 0;
        curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var260->a_ap_tree , "vak641.p" , 19074))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
        old_n = curr_n;
        a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var260->a_ap_tree , "vak641.p" , 19077))[sql__setrang(old_n , 0 , 32767)].n_symb == 49)
            {
            ptoc_Var263 = &ptoc_Var260->a_mblock;
            ptoc_Var264 = (tgg00_QualBuf *) sql__nil(ptoc_Var263->integer.C_4.mb_qual , "vak641.p" , 19079);
            index = (int ) ptoc_Var264->integer.C_2.mfirst_free + -1;
            s40gsint((*(tgg00_DataPart *) sql__nil(ptoc_Var260->a_mblock.integer.C_3.mb_data , "vak641.p" , 19082)).integer.C_1.mbp_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var263->integer.C_5.mb_st , "vak641.p" , 19083))[sql__setrang(index , 1 , 32767) + -1].integer.C_1.epos + 1 , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var263->integer.C_5.mb_st , "vak641.p" , 19084))[sql__setrang(index , 1 , 32767) + -1].integer.C_1.elen_var * 2 + -4 , &first_int , &n_err);
            t01int4(5 , "index       " , index);
            t01int4(5 , "first_int   " , (int ) first_int);
            t01int4(5 , "colin.sci_le" , (int ) (*colin).sci_len);
            if ((int ) n_err == 0)
                {
                if ((int ) first_int < 1 || (int ) first_int > (int ) (*colin).sci_len)
                    {
                    if ((char ) sql__setrang(substr_from_null , 0 , 1))
                        {
                        first_int = 1;
                        }
                    else
                        {
                        a07_b_put_error(&(*acv) , 6290 , ptoc_Var261->n_pos);
                        }
                    }
                }
            }
        else
            {
            first_int = 1;
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var260->a_return_segm , "vak641.p" , 19109)).integer.C_4.sp1r_returncode == 0)
            {
            curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var260->a_ap_tree , "vak641.p" , 19112))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
            old_n = curr_n;
            if (curr_n != 0)
                {
                colin1.sci_len = 0;
                a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var260->a_ap_tree , "vak641.p" , 19119))[sql__setrang(old_n , 0 , 32767)].n_symb == 49)
                    {
                    ptoc_Var265 = &ptoc_Var260->a_mblock;
                    ptoc_Var266 = (tgg00_QualBuf *) sql__nil(ptoc_Var265->integer.C_4.mb_qual , "vak641.p" , 19121);
                    switch ((int ) ptoc_Var260->a_sqlmode)
                        {
                        case 1 :
                            max_length = 4000;
                            break;
                        case 2 :
                            max_length = 8000;
                            break;
                        case 3 :
                            max_length = 8000;
                            break;
                        default :
                            max_length = 254;
                            break;
                        }
                    index = (int ) ptoc_Var266->integer.C_2.mfirst_free + -1;
                    s40gsint((*(tgg00_DataPart *) sql__nil(ptoc_Var260->a_mblock.integer.C_3.mb_data , "vak641.p" , 19135)).integer.C_1.mbp_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var265->integer.C_5.mb_st , "vak641.p" , 19136))[sql__setrang(index , 1 , 32767) + -1].integer.C_1.epos + 1 , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var265->integer.C_5.mb_st , "vak641.p" , 19137))[sql__setrang(index , 1 , 32767) + -1].integer.C_1.elen_var * 2 + -4 , &second_int , &n_err);
                    if ((int ) n_err == 0)
                        {
                        if ((int ) second_int <= 0 || (int ) second_int > max_length || ((int ) keep_datatype == 18 || (int ) keep_datatype == 24) && (int ) second_int * 2 > max_length)
                            {
                            a07_b_put_error(&(*acv) , 6290 , ptoc_Var261->n_pos);
                            }
                        }
                    }
                else
                    {
                    second_int = 0;
                    }
                }
            else
                {
                second_int = 0;
                }
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var260->a_return_segm , "vak641.p" , 19162)).integer.C_4.sp1r_returncode == 0)
                {
                if ((int ) keep_datatype == 18 || (int ) keep_datatype == 24)
                    {
                    op = 7;
                    if ((int ) second_int == 0)
                        {
                        second_int = (short ) sql__setrang((short ) ((int ) (*colin).sci_iolen - (int ) first_int * 2 + 1) , -32768 , 32767);
                        }
                    else
                        {
                        second_int = (short ) sql__setrang((short ) ((int ) second_int * 2) , -32768 , 32767);
                        }
                    (*colin).sci_len = (short ) sql__setrang((short ) ((int ) (*colin).sci_len * 2) , -32768 , 32767);
                    }
                else
                    {
                    op = 17;
                    if ((int ) second_int == 0)
                        {
                        second_int = (short ) sql__setrang((short ) ((int ) (*colin).sci_len - (int ) first_int + 1) , -32768 , 32767);
                        }
                    }
                if (old_n == 0)
                    {
                    a641string_set_operator(&(*acv) , (unsigned char ) sql__setrang(op , 0 , 67) , (int ) second_int , (int ) (*colin).sci_len , (unsigned char)'\002' , (unsigned char)'\0');
                    }
                else
                    {
                    a641string_set_operator(&(*acv) , (unsigned char ) sql__setrang(op , 0 , 67) , (int ) second_int , (int ) (*colin).sci_len , (unsigned char)'\003' , (unsigned char)'\0');
                    }
                if ((int ) op == 17)
                    {
                    (*colin).sci_len = (short ) sql__setrang(second_int , -32768 , 32767);
                    (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len + 1) , -32768 , 32767);
                    }
                else
                    {
                    (*colin).sci_len = (short ) sql__setrang((short ) ((int ) second_int / 2) , -32768 , 32767);
                    (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) second_int + 1) , -32768 , 32767);
                    }
                (*colin).sci_frac = 0;
                }
            }
        if ((ptoc_Var267 = (int ) keep_datatype) == 10 || ptoc_Var267 == 11 || ptoc_Var267 == 13)
            {
            if ((int ) g01code.ctype == 0)
                {
                ptoc_Var262->d_datatype = 2;
                }
            else
                {
                ptoc_Var262->d_datatype = 3;
                }
            (*colin).sci_typ = (unsigned char ) sql__setrang(ptoc_Var262->d_datatype , 0 , 39);
            }
        else
            {
            ptoc_Var262->d_datatype = (unsigned char ) sql__setrang(keep_datatype , 0 , 39);
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var260->a_return_segm , "vak641.p" , 19228)).integer.C_4.sp1r_returncode == 0 && ((int ) keep1_datatype == 10 || (int ) keep1_datatype == 11 || (int ) keep1_datatype == 13))
        {
        ptoc_Var262->d_datatype = (unsigned char ) sql__setrang(keep1_datatype , 0 , 39);
        }
    cccprint("<ak641substr    ");
    }



int ak641leftright(acv, dmli, colin, act_node, symbol)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
tak_sc_symbol symbol;
    {
    char substr_from_null;
    tsp_data_type keep_datatype;
    tsp_data_type keep1_datatype;
    int curr_n;
    int old_n;
    int index;
    tsp_int2 first_int;
    tsp_num_error n_err;
    tgg_stack_op_build_in op;
    tak00_scolinf colin1;
    tak_all_command_glob *ptoc_Var268;
    struct ptoc_Type59 *ptoc_Var269;
    tak_dml_info *ptoc_Var270;
    struct tgg00_MessBlock *ptoc_Var271;
    tgg00_QualBuf *ptoc_Var272;
    int ptoc_Var273;
    cccprint("AK641 ak641leftr");
    ptoc_Var268 = &*acv;
    ptoc_Var269 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var268->a_ap_tree , "vak641.p" , 19261))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var270 = &*dmli;
    if ((int ) symbol == 216)
        {
        op = 53;
        }
    else
        {
        op = 54;
        }
    keep1_datatype = (unsigned char ) sql__setrang(ptoc_Var270->d_datatype , 0 , 39);
    if ((int ) ptoc_Var270->d_datatype == 10 || (int ) ptoc_Var270->d_datatype == 11 || (int ) ptoc_Var270->d_datatype == 13)
        {
        if ((int ) g01code.ctype == 0)
            {
            ptoc_Var270->d_datatype = 2;
            }
        else
            {
            ptoc_Var270->d_datatype = 3;
            }
        }
    else
        {
        if ((int ) ptoc_Var270->d_datatype == 14)
            {
            ptoc_Var270->d_datatype = 16;
            }
        }
    curr_n = (int ) ptoc_Var269->n_lo_level;
    (*colin).sci_len = 0;
    substr_from_null = (char ) sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var268->a_ap_tree , "vak641.p" , 19288))[sql__setrang(curr_n , 0 , 32767)].n_proc == 0 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var268->a_ap_tree , "vak641.p" , 19289))[sql__setrang(curr_n , 0 , 32767)].n_symb == 50 , 0 , 1);
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var268->a_return_segm , "vak641.p" , 19291)).integer.C_4.sp1r_returncode == 0)
        {
        keep_datatype = (unsigned char ) sql__setrang(ptoc_Var270->d_datatype , 0 , 39);
        ptoc_Var270->d_datatype = 15;
        colin1.sci_len = 0;
        curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var268->a_ap_tree , "vak641.p" , 19297))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
        old_n = curr_n;
        a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var268->a_ap_tree , "vak641.p" , 19300))[sql__setrang(old_n , 0 , 32767)].n_symb == 49)
            {
            ptoc_Var271 = &ptoc_Var268->a_mblock;
            ptoc_Var272 = (tgg00_QualBuf *) sql__nil(ptoc_Var271->integer.C_4.mb_qual , "vak641.p" , 19302);
            index = (int ) ptoc_Var272->integer.C_2.mfirst_free + -1;
            s40gsint((*(tgg00_DataPart *) sql__nil(ptoc_Var268->a_mblock.integer.C_3.mb_data , "vak641.p" , 19305)).integer.C_1.mbp_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var271->integer.C_5.mb_st , "vak641.p" , 19306))[sql__setrang(index , 1 , 32767) + -1].integer.C_1.epos + 1 , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var271->integer.C_5.mb_st , "vak641.p" , 19307))[sql__setrang(index , 1 , 32767) + -1].integer.C_1.elen_var * 2 + -4 , &first_int , &n_err);
            if ((int ) n_err == 0)
                {
                if ((int ) first_int < 0 || (int ) first_int > 8000)
                    {
                    a07_b_put_error(&(*acv) , 6290 , ptoc_Var269->n_pos);
                    }
                }
            }
        else
            {
            first_int = (short ) sql__setrang((short ) ((int ) (*colin).sci_len + 1) , -32768 , 32767);
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var268->a_return_segm , "vak641.p" , 19323)).integer.C_4.sp1r_returncode == 0)
            {
            if ((ptoc_Var273 = (int ) keep_datatype) == 24 || ptoc_Var273 == 18)
                {
                first_int = (short ) sql__setrang((short ) ((int ) first_int * 2) , -32768 , 32767);
                (*colin).sci_len = (short ) sql__setrang((short ) ((int ) (*colin).sci_len * 2) , -32768 , 32767);
                a641string_set_operator(&(*acv) , (unsigned char ) sql__setrang(op , 0 , 67) , (int ) first_int , (int ) (*colin).sci_len , (unsigned char)'\002' , (unsigned char)'\0');
                (*colin).sci_len = (short ) sql__setrang((short ) ((int ) first_int / 2) , -32768 , 32767);
                (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) first_int + 1) , -32768 , 32767);
                }
            else
                {
                a641string_set_operator(&(*acv) , (unsigned char ) sql__setrang(op , 0 , 67) , (int ) first_int , (int ) (*colin).sci_len , (unsigned char)'\001' , (unsigned char)'\0');
                (*colin).sci_len = (short ) sql__setrang(first_int , -32768 , 32767);
                (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len + 1) , -32768 , 32767);
                }
            (*colin).sci_frac = 0;
            }
        if ((ptoc_Var273 = (int ) keep_datatype) == 10 || ptoc_Var273 == 11 || ptoc_Var273 == 13)
            {
            if ((int ) g01code.ctype == 0)
                {
                ptoc_Var270->d_datatype = 2;
                }
            else
                {
                ptoc_Var270->d_datatype = 3;
                }
            (*colin).sci_typ = (unsigned char ) sql__setrang(ptoc_Var270->d_datatype , 0 , 39);
            }
        else
            {
            ptoc_Var270->d_datatype = (unsigned char ) sql__setrang(keep_datatype , 0 , 39);
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var268->a_return_segm , "vak641.p" , 19363)).integer.C_4.sp1r_returncode == 0 && ((int ) keep1_datatype == 10 || (int ) keep1_datatype == 11 || (int ) keep1_datatype == 13))
        {
        ptoc_Var270->d_datatype = (unsigned char ) sql__setrang(keep1_datatype , 0 , 39);
        }
    cccprint("<ak641leftright ");
    }



int ak641timedate(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    int curr_n;
    char op_set;
    tak_all_command_glob *ptoc_Var274;
    struct ptoc_Type59 *ptoc_Var275;
    tak_dml_info *ptoc_Var276;
    tgg00_QualBuf *ptoc_Var277;
    tak_viewdescrecord *ptoc_Var278;
    struct ptoc_Type119 *ptoc_Var279;
    tak00_scolinf *ptoc_Var280;
    int ptoc_Var281;
    cccprint("AK641 ak641timed");
    ptoc_Var274 = &*acv;
    ptoc_Var275 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var274->a_ap_tree , "vak641.p" , 19388))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var276 = &*dmli;
    ptoc_Var277 = (tgg00_QualBuf *) sql__nil(ptoc_Var274->a_mblock.integer.C_4.mb_qual , "vak641.p" , 19389);
    op_set = 0;
    ptoc_Var276->d_datatype = 14;
    curr_n = (int ) ptoc_Var275->n_lo_level;
    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var274->a_ap_tree , "vak641.p" , 19394))[sql__setrang(curr_n , 0 , 32767)].n_symb == 48 && (int ) ptoc_Var274->a_is_ddl == 20)
        {
        a16inc_vdesc_cnt(&(*acv) , &(*dmli) , &ptoc_Var274->a_ptr10);
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var274->a_return_segm , "vak641.p" , 19399)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var278 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var274->a_ptr10 , "vak641.p" , 19401)).integer.C_46.sviewdesc;
            ptoc_Var279 = &ptoc_Var278->vdescription[sql__setrang((int ) ptoc_Var278->vdesc_cnt , 1 , 672) + -1];
            ptoc_Var279->vfromtabno = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var274->a_ap_tree , "vak641.p" , 19403))[sql__setrang(curr_n , 0 , 32767)].n_length , -32768 , 32767);
            ptoc_Var279->vfromextcolno = 0;
            ptoc_Var279->vn_pos = (short ) sql__setrang((short ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var274->a_ap_tree , "vak641.p" , 19405))[sql__setrang(curr_n , 0 , 32767)].n_pos , -32768 , 32767);
            ptoc_Var279->vextcolno = 0;
            if ((int ) ptoc_Var275->n_symb == 146)
                {
                ptoc_Var279->vdatatype = 10;
                }
            else
                {
                ptoc_Var279->vdatatype = 11;
                }
            }
        }
    (*colin).sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var274->a_return_segm , "vak641.p" , 19420)).integer.C_4.sp1r_returncode == 21000)
        {
        (*(tsp1_segment *) sql__nil(ptoc_Var274->a_return_segm , "vak641.p" , 19424)).integer.C_4.sp1r_returncode = 0;
        if ((int ) ptoc_Var275->n_symb == 146)
            {
            ptoc_Var276->d_datatype = 10;
            }
        else
            {
            ptoc_Var276->d_datatype = 11;
            }
        (*colin).sci_len = 0;
        a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var274->a_return_segm , "vak641.p" , 19435)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var280 = &*colin;
        if ((ptoc_Var281 = (int ) ptoc_Var280->sci_typ) == 18 || ptoc_Var281 == 4)
            {
            *wrong_datatype = 1;
            }
        else
            {
            if ((int ) ptoc_Var275->n_symb == 146)
                {
                if ((int ) ptoc_Var280->sci_typ == 11)
                    {
                    *wrong_datatype = 1;
                    }
                else
                    {
                    if ((ptoc_Var281 = (int ) ptoc_Var280->sci_typ) == 2 || ptoc_Var281 == 3 || ptoc_Var281 == 24)
                        {
                        a641check_datetime(&(*acv) , &(*dmli) , 10);
                        }
                    else
                        {
                        if ((int ) ptoc_Var280->sci_typ == 13)
                            {
                            a65_set_operator(&(*acv) , 51);
                            }
                        else
                            {
                            if ((ptoc_Var281 = (int ) ptoc_Var280->sci_typ) == 0 || ptoc_Var281 == 1 || ptoc_Var281 == 12)
                                {
                                op_set = 1;
                                }
                            }
                        }
                    ptoc_Var280->sci_len = 8;
                    ptoc_Var276->d_datatype = 10;
                    ptoc_Var280->sci_frac = 0;
                    ptoc_Var280->sci_typ = 10;
                    ptoc_Var280->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var280->sci_len + 1) , -32768 , 32767);
                    if ((char ) sql__setrang(op_set , 0 , 1))
                        {
                        a65_set_operator(&(*acv) , 50);
                        }
                    }
                }
            else
                {
                if ((ptoc_Var281 = (int ) ptoc_Var280->sci_typ) == 0 || ptoc_Var281 == 1 || ptoc_Var281 == 12 || ptoc_Var281 == 10)
                    {
                    *wrong_datatype = 1;
                    }
                else
                    {
                    if ((ptoc_Var281 = (int ) ptoc_Var280->sci_typ) == 2 || ptoc_Var281 == 3 || ptoc_Var281 == 24)
                        {
                        a641check_datetime(&(*acv) , &(*dmli) , 11);
                        }
                    else
                        {
                        if ((int ) ptoc_Var280->sci_typ == 13)
                            {
                            op_set = 1;
                            }
                        }
                    ptoc_Var280->sci_len = 8;
                    ptoc_Var276->d_datatype = 11;
                    ptoc_Var280->sci_frac = 0;
                    ptoc_Var280->sci_typ = 11;
                    ptoc_Var280->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var280->sci_len + 1) , -32768 , 32767);
                    if ((char ) sql__setrang(op_set , 0 , 1))
                        {
                        a65_set_operator(&(*acv) , 63);
                        }
                    }
                }
            }
        }
    cccprint("<ak641timedate  ");
    }



int ak641timediff(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    int curr_n;
    tak00_scolinf colin1;
    tak_all_command_glob *ptoc_Var282;
    tak_dml_info *ptoc_Var283;
    tak00_scolinf *ptoc_Var284;
    cccprint("AK641 ak641timed");
    ptoc_Var282 = &*acv;
    ptoc_Var283 = &*dmli;
    curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var282->a_ap_tree , "vak641.p" , 19527))[sql__setrang(act_node , 0 , 32767)].n_lo_level;
    ak641type_or_timestamp_parameter(&(*acv) , &(*dmli) , &(*colin) , &curr_n , 11);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var282->a_return_segm , "vak641.p" , 19529)).integer.C_4.sp1r_returncode == 0)
        {
        curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var282->a_ap_tree , "vak641.p" , 19532))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
        colin1.sci_len = 0;
        a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
        t01int4(5 , ">>d_datatype" , sql__ord((int ) ptoc_Var283->d_datatype));
        t01int4(5 , ">>colin1.typ" , sql__ord((int ) colin1.sci_typ));
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var282->a_return_segm , "vak641.p" , 19537)).integer.C_4.sp1r_returncode == 0 && (int ) ptoc_Var283->d_datatype != (int ) colin1.sci_typ)
            {
            a641check_datetime(&(*acv) , &(*dmli) , (unsigned char ) sql__setrang(ptoc_Var283->d_datatype , 0 , 39));
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var282->a_return_segm , "vak641.p" , 19544)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var283->d_datatype = 11;
        ptoc_Var284 = &*colin;
        ptoc_Var284->sci_len = 8;
        ptoc_Var284->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var284->sci_len + 1) , -32768 , 32767);
        ptoc_Var284->sci_frac = 0;
        ptoc_Var284->sci_typ = 11;
        a65_set_operator(&(*acv) , 64);
        }
    cccprint("<ak641timediff  ");
    }



int ak641not_first_type_or_timestamp_parameter(acv, dmli, colin, act_node, wanted_type)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int *act_node;
tsp_data_type wanted_type;
    {
    int st_begin;
    int st_index;
    cccprint("AK641 ak641not_f");
    st_begin = (int ) (*dmli).d_param_st_begin;
    st_index = (int ) (*dmli).d_param_st_index;
    (*dmli).d_param_st_begin = 0;
    (*dmli).d_param_st_index = 0;
    ak641type_or_timestamp_parameter(&(*acv) , &(*dmli) , &(*colin) , &(*act_node) , (unsigned char ) sql__setrang(wanted_type , 0 , 39));
    (*dmli).d_param_st_begin = (short ) sql__setrang((short ) st_begin , -32768 , 32767);
    if ((int ) (*dmli).d_param_st_index == 0)
        {
        (*dmli).d_param_st_index = (short ) sql__setrang((short ) st_index , -32768 , 32767);
        }
    cccprint("<ak641not_first_");
    }



int ak641type_or_timestamp_parameter(acv, dmli, colin, act_node, wanted_type)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int *act_node;
tsp_data_type wanted_type;
    {
    int top_node;
    tak_all_command_glob *ptoc_Var285;
    tak_dml_info *ptoc_Var286;
    int ptoc_Var287;
    struct ptoc_Type59 *ptoc_Var288;
    cccprint("AK641 ak641type_");
    ptoc_Var285 = &*acv;
    ptoc_Var286 = &*dmli;
    ptoc_Var286->d_datatype = 14;
    (*colin).sci_len = 0;
    top_node = *act_node;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &(*act_node));
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var285->a_return_segm , "vak641.p" , 19610)).integer.C_4.sp1r_returncode == 21000)
        {
        (*(tsp1_segment *) sql__nil(ptoc_Var285->a_return_segm , "vak641.p" , 19613)).integer.C_4.sp1r_returncode = 0;
        ptoc_Var286->d_datatype = (unsigned char ) sql__setrang(wanted_type , 0 , 39);
        *act_node = top_node;
        a640factor(&(*acv) , &(*dmli) , &(*colin) , &(*act_node));
        }
    t01int4(5 , ">>d_datatype" , sql__ord((int ) ptoc_Var286->d_datatype));
    t01int4(5 , ">>colin.typ " , sql__ord((int ) (*colin).sci_typ));
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var285->a_return_segm , "vak641.p" , 19621)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) (*colin).sci_typ == (int ) wanted_type || (int ) (*colin).sci_typ == 13)
            {
            ptoc_Var286->d_datatype = (unsigned char ) sql__setrang((*colin).sci_typ , 0 , 39);
            }
        else
            {
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var285->a_ap_tree , "vak641.p" , 19628))[sql__setrang(top_node , 0 , 32767)].n_symb == 50)
                {
                ptoc_Var286->d_datatype = (unsigned char ) sql__setrang(wanted_type , 0 , 39);
                }
            else
                {
                if ((ptoc_Var287 = (int ) (*colin).sci_typ) == 2 || ptoc_Var287 == 3 || ptoc_Var287 == 24)
                    {
                    ptoc_Var288 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var285->a_ap_tree , "vak641.p" , 19634))[sql__setrang(top_node , 0 , 32767)];
                    if ((int ) ptoc_Var288->n_symb == 48 && ((int ) wanted_type == 10 && (int ) ptoc_Var288->n_length > a01char_size * 10 || (int ) wanted_type == 11 && (int ) ptoc_Var288->n_length > a01char_size * 8))
                        {
                        a641check_datetime(&(*acv) , &(*dmli) , 13);
                        ptoc_Var286->d_datatype = 13;
                        (*colin).sci_typ = 13;
                        }
                    else
                        {
                        a641check_datetime(&(*acv) , &(*dmli) , (unsigned char ) sql__setrang(wanted_type , 0 , 39));
                        ptoc_Var286->d_datatype = (unsigned char ) sql__setrang(wanted_type , 0 , 39);
                        }
                    }
                else
                    {
                    a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var285->a_ap_tree , "vak641.p" , 19663))[sql__setrang(top_node , 0 , 32767)].n_pos);
                    }
                }
            }
        }
    cccprint("<ak641type_or_ti");
    }



int ak641timestamp(acv, dmli, frec, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_factorrec *frec;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    int date_n;
    int time_n;
    int value_cnt;
    tak_all_command_glob *ptoc_Var289;
    struct ptoc_Type59 *ptoc_Var290;
    tak_dml_info *ptoc_Var291;
    tak00_scolinf *ptoc_Var292;
    cccprint("AK641 ak641times");
    ptoc_Var289 = &*acv;
    ptoc_Var290 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var289->a_ap_tree , "vak641.p" , 19687))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var291 = &*dmli;
    if ((int ) ptoc_Var290->n_lo_level == 0)
        {
        a642other_than_column(&(*acv) , &(*dmli) , &(*frec) , &(*colin) , act_node);
        }
    else
        {
        value_cnt = 2;
        date_n = (int ) ptoc_Var290->n_lo_level;
        time_n = date_n;
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var289->a_ap_tree , "vak641.p" , 19697))[sql__setrang(time_n , 0 , 32767)].n_symb == 43)
            {
            time_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var289->a_ap_tree , "vak641.p" , 19699))[sql__setrang(time_n , 0 , 32767)].n_sa_level;
            }
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var289->a_ap_tree , "vak641.p" , 19701))[sql__setrang(time_n , 0 , 32767)].n_symb == 44)
            {
            time_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var289->a_ap_tree , "vak641.p" , 19703))[sql__setrang(time_n , 0 , 32767)].n_sa_level;
            }
        time_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var289->a_ap_tree , "vak641.p" , 19705))[sql__setrang(time_n , 0 , 32767)].n_sa_level;
        if (time_n != 0)
            {
            ptoc_Var291->d_datatype = 10;
            (*colin).sci_len = 0;
            a640factor(&(*acv) , &(*dmli) , &(*colin) , &date_n);
            if ((int ) (*colin).sci_typ == 13)
                {
                *wrong_datatype = 1;
                }
            else
                {
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var289->a_return_segm , "vak641.p" , 19716)).integer.C_4.sp1r_returncode == 0)
                    {
                    if ((int ) (*colin).sci_typ != 10)
                        {
                        a641check_datetime(&(*acv) , &(*dmli) , 10);
                        }
                    ptoc_Var291->d_datatype = 11;
                    (*colin).sci_len = 0;
                    a640not_first_factor(&(*acv) , &(*dmli) , &(*colin) , &time_n);
                    if ((int ) (*colin).sci_typ == 13)
                        {
                        *wrong_datatype = 1;
                        }
                    else
                        {
                        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var289->a_return_segm , "vak641.p" , 19730)).integer.C_4.sp1r_returncode == 0)
                            {
                            if ((int ) (*colin).sci_typ != 11)
                                {
                                a641check_datetime(&(*acv) , &(*dmli) , 11);
                                }
                            }
                        }
                    }
                }
            }
        else
            {
            value_cnt = 1;
            ptoc_Var291->d_datatype = 13;
            (*colin).sci_len = 0;
            a640factor(&(*acv) , &(*dmli) , &(*colin) , &date_n);
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var289->a_return_segm , "vak641.p" , 19748)).integer.C_4.sp1r_returncode == 0)
                {
                if ((int ) (*colin).sci_typ != 13)
                    {
                    a641check_datetime(&(*acv) , &(*dmli) , 13);
                    }
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var289->a_return_segm , "vak641.p" , 19757)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var292 = &*colin;
            ptoc_Var292->sci_len = 20;
            ptoc_Var292->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var292->sci_len + 1) , -32768 , 32767);
            ptoc_Var291->d_datatype = 13;
            ptoc_Var292->sci_typ = 13;
            ptoc_Var292->sci_frac = 0;
            if (value_cnt == 2)
                {
                a65_set_operator(&(*acv) , 65);
                }
            }
        }
    cccprint("<ak641timestamp ");
    }



int ak641truncround(acv, dmli, colin, act_node, wrong_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
char *wrong_datatype;
    {
    char negativ_trunc;
    int curr_n;
    int next_n;
    tak00_scolinf colin1;
    tak_all_command_glob *ptoc_Var293;
    struct ptoc_Type59 *ptoc_Var294;
    tak_dml_info *ptoc_Var295;
    tak00_scolinf *ptoc_Var296;
    cccprint("AK641 ak641trunc");
    ptoc_Var293 = &*acv;
    ptoc_Var294 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var293->a_ap_tree , "vak641.p" , 19794))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var295 = &*dmli;
    curr_n = (int ) ptoc_Var294->n_lo_level;
    (*colin).sci_len = 0;
    if ((int ) ptoc_Var293->a_sqlmode != 2)
        {
        ptoc_Var295->d_datatype = 15;
        }
    else
        {
        a65_look_for_datatypes(&(*acv) , &(*dmli) , curr_n);
        if ((int ) ptoc_Var295->d_datatype != 13)
            {
            ptoc_Var295->d_datatype = 15;
            }
        }
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var293->a_return_segm , "vak641.p" , 19811)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) ptoc_Var295->d_datatype == 13)
            {
            curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var293->a_ap_tree , "vak641.p" , 19816))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
            if (curr_n != 0)
                {
                colin1.sci_len = 0;
                colin1.sci_frac = 0;
                ptoc_Var295->d_datatype = 2;
                a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
                if ((int ) ptoc_Var295->d_datatype != 2 && (int ) ptoc_Var295->d_datatype != 3)
                    {
                    *wrong_datatype = 1;
                    }
                else
                    {
                    if ((int ) ptoc_Var294->n_symb == 63)
                        {
                        a641string_set_operator(&(*acv) , 42 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                        }
                    else
                        {
                        a641string_set_operator(&(*acv) , 43 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                        }
                    ptoc_Var295->d_datatype = 13;
                    }
                }
            else
                {
                if ((int ) ptoc_Var294->n_symb == 63)
                    {
                    a641string_set_operator(&(*acv) , 42 , 0 , 1 , (unsigned char)'\0' , (unsigned char)'\0');
                    }
                else
                    {
                    a641string_set_operator(&(*acv) , 43 , 0 , 1 , (unsigned char)'\0' , (unsigned char)'\0');
                    }
                }
            }
        else
            {
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var293->a_ap_tree , "vak641.p" , 19853))[sql__setrang(curr_n , 0 , 32767)].n_sa_level != 0)
                {
                curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var293->a_ap_tree , "vak641.p" , 19856))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
                colin1.sci_len = 0;
                colin1.sci_frac = 0;
                next_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var293->a_ap_tree , "vak641.p" , 19859))[sql__setrang(curr_n , 0 , 32767)].n_lo_level;
                negativ_trunc = (char ) sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var293->a_ap_tree , "vak641.p" , 19860))[sql__setrang(curr_n , 0 , 32767)].n_symb == 10 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var293->a_ap_tree , "vak641.p" , 19861))[sql__setrang(next_n , 0 , 32767)].n_symb == 49 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var293->a_ap_tree , "vak641.p" , 19862))[sql__setrang(next_n , 0 , 32767)].n_sa_level == 0 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var293->a_ap_tree , "vak641.p" , 19863))[sql__setrang(next_n , 0 , 32767)].n_lo_level == 0 , 0 , 1);
                a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
                if ((int ) ptoc_Var294->n_symb == 63)
                    {
                    a65_set_operator(&(*acv) , 74);
                    }
                else
                    {
                    a65_set_operator(&(*acv) , 73);
                    }
                if (!(char ) sql__setrang(negativ_trunc , 0 , 1))
                    {
                    ak641put_roundlength(&(*acv) , &curr_n , &(*colin));
                    }
                else
                    {
                    if ((int ) (*colin).sci_frac > 0)
                        {
                        ptoc_Var296 = &*colin;
                        ptoc_Var296->sci_len = (short ) sql__setrang((short ) ((int ) ptoc_Var296->sci_len - (int ) ptoc_Var296->sci_frac) , -32768 , 32767);
                        ptoc_Var296->sci_frac = 0;
                        ptoc_Var296->sci_iolen = (short ) sql__setrang((short ) (((int ) ptoc_Var296->sci_len + 1) / 2 + 2) , -32768 , 32767);
                        }
                    }
                }
            else
                {
                if ((int ) (*colin).sci_frac > 0)
                    {
                    (*colin).sci_frac = 0;
                    }
                if ((int ) ptoc_Var294->n_symb == 63)
                    {
                    a65_set_operator(&(*acv) , 79);
                    }
                else
                    {
                    a65_set_operator(&(*acv) , 76);
                    }
                }
            }
        }
    t01int4(5 , "tr: datatype" , sql__ord((int ) ptoc_Var295->d_datatype));
    cccprint("<ak641truncround");
    }



int ak641mod_power(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    int curr_n;
    int exponent_curr_n;
    tsp_int2 second_int;
    tak00_scolinf colin1;
    tak_all_command_glob *ptoc_Var297;
    struct ptoc_Type59 *ptoc_Var298;
    tak_dml_info *ptoc_Var299;
    tak00_scolinf *ptoc_Var300;
    tak00_scolinf *ptoc_Var301;
    struct ptoc_Type59 *ptoc_Var302;
    tak00_scolinf *ptoc_Var303;
    cccprint("AK641 ak641mod_p");
    ptoc_Var297 = &*acv;
    ptoc_Var298 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var297->a_ap_tree , "vak641.p" , 19924))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var299 = &*dmli;
    ptoc_Var299->d_datatype = 15;
    curr_n = (int ) ptoc_Var298->n_lo_level;
    (*colin).sci_len = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var297->a_ap_tree , "vak641.p" , 19930))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
    colin1.sci_len = 0;
    exponent_curr_n = curr_n;
    a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var297->a_return_segm , "vak641.p" , 19934)).integer.C_4.sp1r_returncode == 0)
        {
        switch ((int ) ptoc_Var298->n_symb)
            {
            case 170 :
                if ((int ) colin1.sci_frac != 0)
                    {
                    a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var297->a_ap_tree , "vak641.p" , 19941))[sql__setrang(curr_n , 0 , 32767)].n_pos);
                    }
                else
                    {
                    a65_set_operator(&(*acv) , 88);
                    }
                break;
            case 210 :
                ptoc_Var300 = &*colin;
                a641string_set_operator(&(*acv) , 47 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                ptoc_Var300->sci_len = 38;
                ptoc_Var300->sci_frac = -1;
                ptoc_Var300->sci_typ = 1;
                ptoc_Var300->sci_iolen = 21;
                break;
            case 175 :
                ptoc_Var301 = &*colin;
                a641string_set_operator(&(*acv) , 33 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                ptoc_Var301->sci_len = 38;
                ptoc_Var301->sci_frac = -1;
                ptoc_Var301->sci_typ = 1;
                ptoc_Var301->sci_iolen = 21;
                break;
            case 172 :
                if ((int ) colin1.sci_frac != 0)
                    {
                    a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var297->a_ap_tree , "vak641.p" , 19971))[sql__setrang(curr_n , 0 , 32767)].n_pos);
                    }
                else
                    {
                    ptoc_Var302 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var297->a_ap_tree , "vak641.p" , 19973))[sql__setrang(exponent_curr_n , 0 , 32767)];
                    ptoc_Var303 = &*colin;
                    a65_set_operator(&(*acv) , 75);
                    second_int = 38;
                    if ((int ) ptoc_Var302->n_symb == 49)
                        {
                        a05_unsigned_int2_get(&(*acv) , ptoc_Var302->n_pos , (short ) sql__setrang(ptoc_Var302->n_length , -32768 , 32767) , 6125 , &second_int);
                        if ((int ) second_int < 2)
                            {
                            second_int = 2;
                            }
                        }
                    if ((int ) ptoc_Var303->sci_len * (int ) second_int < 38 && (int ) ptoc_Var303->sci_typ == 0)
                        {
                        ptoc_Var303->sci_len = (short ) sql__setrang((short ) ((int ) ptoc_Var303->sci_len * (int ) second_int) , -32768 , 32767);
                        ptoc_Var303->sci_frac = (short ) sql__setrang((short ) ((int ) ptoc_Var303->sci_frac * (int ) second_int) , -32768 , 32767);
                        ptoc_Var303->sci_iolen = (short ) sql__setrang((short ) (((int ) ptoc_Var303->sci_len + 1) / 2 + 2) , -32768 , 32767);
                        }
                    else
                        {
                        ptoc_Var303->sci_len = 38;
                        ptoc_Var303->sci_frac = -1;
                        ptoc_Var303->sci_typ = 1;
                        ptoc_Var303->sci_iolen = 21;
                        }
                    }
                break;
            default :
                (unsigned char ) sql__caseerr("vak641.p" , 19936);
                break;
            }
        }
    cccprint("<ak641mod_power ");
    }



int ak641integer(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    int curr_n;
    tak_all_command_glob *ptoc_Var304;
    struct ptoc_Type59 *ptoc_Var305;
    tak_dml_info *ptoc_Var306;
    tak00_scolinf *ptoc_Var307;
    tak00_scolinf *ptoc_Var308;
    tak00_scolinf *ptoc_Var309;
    cccprint("AK641 ak641integ");
    ptoc_Var304 = &*acv;
    ptoc_Var305 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var304->a_ap_tree , "vak641.p" , 20027))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var306 = &*dmli;
    ptoc_Var306->d_datatype = 15;
    curr_n = (int ) ptoc_Var305->n_lo_level;
    (*colin).sci_len = 0;
    (*colin).sci_frac = 0;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var304->a_return_segm , "vak641.p" , 20034)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) ptoc_Var305->n_symb != 62)
            {
            switch ((int ) ptoc_Var305->n_symb)
                {
                case 167 :
                    a65_set_operator(&(*acv) , 71);
                    break;
                case 177 :
                    a641string_set_operator(&(*acv) , 35 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                    break;
                case 180 :
                    a641string_set_operator(&(*acv) , 38 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                    break;
                case 207 :
                    a641string_set_operator(&(*acv) , 44 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                    break;
                case 173 :
                    a641string_set_operator(&(*acv) , 31 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                    break;
                case 168 :
                    a65_set_operator(&(*acv) , 72);
                    break;
                case 39 :
                    a65_set_operator(&(*acv) , 79);
                    break;
                case 174 :
                    a641string_set_operator(&(*acv) , 32 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                    break;
                case 212 :
                    a641string_set_operator(&(*acv) , 49 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                    break;
                case 169 :
                    a65_set_operator(&(*acv) , 77);
                    break;
                case 176 :
                    a641string_set_operator(&(*acv) , 34 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                    break;
                case 179 :
                    a641string_set_operator(&(*acv) , 37 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                    break;
                case 208 :
                    a641string_set_operator(&(*acv) , 45 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                    break;
                case 171 :
                    a65_set_operator(&(*acv) , 78);
                    break;
                case 178 :
                    a641string_set_operator(&(*acv) , 36 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                    break;
                case 181 :
                    a641string_set_operator(&(*acv) , 39 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                    break;
                case 209 :
                    a641string_set_operator(&(*acv) , 46 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                    break;
                case 211 :
                    a641string_set_operator(&(*acv) , 48 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                    break;
                case 215 :
                    a641string_set_operator(&(*acv) , 51 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                    break;
                case 214 :
                    a641string_set_operator(&(*acv) , 52 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\0');
                    break;
                default :
                    (unsigned char ) sql__caseerr("vak641.p" , 20039);
                    break;
                }
            switch ((int ) ptoc_Var305->n_symb)
                {
                case 39 :
                case 168 :
                case 167 :
                    ptoc_Var307 = &*colin;
                    if ((int ) ptoc_Var307->sci_typ != 0)
                        {
                        ptoc_Var307->sci_typ = 0;
                        ptoc_Var307->sci_len = 38;
                        ptoc_Var307->sci_iolen = 21;
                        }
                    ptoc_Var307->sci_frac = 0;
                    break;
                case 169 :
                    ptoc_Var308 = &*colin;
                    ptoc_Var308->sci_len = 1;
                    ptoc_Var308->sci_frac = 0;
                    ptoc_Var308->sci_typ = 0;
                    ptoc_Var308->sci_iolen = (short ) sql__setrang((short ) (((int ) ptoc_Var308->sci_len + 1) / 2 + 2) , -32768 , 32767);
                    break;
                default :
                    ptoc_Var309 = &*colin;
                    ptoc_Var309->sci_len = 38;
                    ptoc_Var309->sci_frac = -1;
                    ptoc_Var309->sci_typ = 1;
                    ptoc_Var309->sci_iolen = 21;
                    break;
                }
            }
        else
            {
            a65_set_operator(&(*acv) , 70);
            }
        }
    cccprint("<ak641integer   ");
    }



int ak641newcolin(colin, colin1)
tak00_scolinf *colin;
tak00_scolinf *colin1;
    {
    cccprint("AK641 ak641newco");
    t01int4(5 , "colin  typ  " , sql__ord((int ) (*colin).sci_typ));
    t01int4(5 , "colin  len  " , (int ) (*colin).sci_len);
    t01int4(5 , "colin  frac " , (int ) (*colin).sci_frac);
    t01int4(5 , "colin  ilen " , (int ) (*colin).sci_iolen);
    t01int4(5 , "colin1 typ  " , sql__ord((int ) (*colin1).sci_typ));
    t01int4(5 , "colin1 len  " , (int ) (*colin1).sci_len);
    t01int4(5 , "colin1 frac " , (int ) (*colin1).sci_frac);
    t01int4(5 , "colin1 ilen " , (int ) (*colin1).sci_iolen);
    if ((int ) (*colin).sci_frac != -1 && (int ) (*colin).sci_frac != -1)
        {
        if ((int ) (*colin).sci_frac < (int ) (*colin1).sci_frac)
            {
            if ((int ) (*colin).sci_len < (int ) (*colin1).sci_len)
                {
                (*colin).sci_len = (short ) sql__setrang((*colin1).sci_len , -32768 , 32767);
                }
            else
                {
                (*colin).sci_len = (short ) sql__setrang((short ) ((int ) (*colin).sci_len + (int ) (*colin1).sci_frac - (int ) (*colin).sci_frac) , -32768 , 32767);
                }
            if ((int ) (*colin).sci_iolen < (int ) (*colin1).sci_iolen)
                {
                (*colin).sci_iolen = (short ) sql__setrang((*colin1).sci_iolen , -32768 , 32767);
                }
            else
                {
                (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_iolen + (int ) (*colin1).sci_frac - (int ) (*colin).sci_frac) , -32768 , 32767);
                }
            (*colin).sci_frac = (short ) sql__setrang((*colin1).sci_frac , -32768 , 32767);
            }
        else
            {
            if ((int ) (*colin).sci_len < (int ) (*colin1).sci_len)
                {
                (*colin).sci_len = (short ) sql__setrang((*colin1).sci_len , -32768 , 32767);
                }
            if ((int ) (*colin).sci_iolen < (int ) (*colin1).sci_iolen)
                {
                (*colin).sci_iolen = (short ) sql__setrang((*colin1).sci_iolen , -32768 , 32767);
                }
            }
        }
    else
        {
        if ((int ) (*colin).sci_len < (int ) (*colin1).sci_len)
            {
            (*colin).sci_len = (short ) sql__setrang((*colin1).sci_len , -32768 , 32767);
            (*colin).sci_iolen = (short ) sql__setrang((*colin1).sci_iolen , -32768 , 32767);
            }
        (*colin).sci_frac = -1;
        }
    if ((int ) (*colin).sci_typ == 14 && (int ) (*colin1).sci_typ != 14)
        {
        (*colin).sci_typ = (unsigned char ) sql__setrang((*colin1).sci_typ , 0 , 39);
        }
    t01int4(5 , "colin  typ  " , sql__ord((int ) (*colin).sci_typ));
    t01int4(5 , "colin  len  " , (int ) (*colin).sci_len);
    t01int4(5 , "colin  frac " , (int ) (*colin).sci_frac);
    t01int4(5 , "colin  ilen " , (int ) (*colin).sci_iolen);
    cccprint("<ak641newcolin  ");
    }



char ak641is_trans_ascii_to_uni(st)
tgg00_StackEntry *st;
    {
    char _ak641is_trans_ascii_to_uni;
    tgg00_StackEntry *ptoc_Var310;
    cccprint("AK641 ak641is_tr");
    ptoc_Var310 = &*st;
    _ak641is_trans_ascii_to_uni = (char ) sql__setrang((int ) ptoc_Var310->integer.C_1.etype == 55 && (int ) ptoc_Var310->integer.C_4.eop_build_in == 66 && (int ) ptoc_Var310->integer.C_1.ecol_tab[0] == 0 && (int ) ptoc_Var310->integer.C_1.ecol_tab[1] == 20 , 0 , 1);
    cccprint("<ak641is_trans_a");
    return(_ak641is_trans_ascii_to_uni);
    }



char ak641is_trans_uni_to_ascii(st)
tgg00_StackEntry *st;
    {
    char _ak641is_trans_uni_to_ascii;
    tgg00_StackEntry *ptoc_Var311;
    cccprint("AK641 ak641is_tr");
    ptoc_Var311 = &*st;
    _ak641is_trans_uni_to_ascii = (char ) sql__setrang((int ) ptoc_Var311->integer.C_1.etype == 55 && (int ) ptoc_Var311->integer.C_4.eop_build_in == 66 && (int ) ptoc_Var311->integer.C_1.ecol_tab[0] == 20 && (int ) ptoc_Var311->integer.C_1.ecol_tab[1] == 0 , 0 , 1);
    cccprint("<ak641is_trans_u");
    return(_ak641is_trans_uni_to_ascii);
    }



char ak641unicode_conv_destroyed(mb_st, stackentries, pair_cnt)
tgg00_StackListPtr mb_st;
ak641stackentries stackentries;
tsp_int2 pair_cnt;
    {
    char _ak641unicode_conv_destroyed;
    int ind;
    char conv_to_uni;
    cccprint("AK641 ak641unico");
    ind = 0;
    conv_to_uni = 1;
    while (ind <= (int ) pair_cnt && (char ) sql__setrang(conv_to_uni , 0 , 1))
        {
        if ((int ) stackentries[sql__setrang(ind , 0 , 256)] > 1)
            {
            if (ak641is_trans_ascii_to_uni(&(*(tgg00_StackList *) sql__nil(mb_st , "vak641.p" , 20277))[sql__setrang((int ) stackentries[sql__setrang(ind , 0 , 256)] , 1 , 32767) + -1]))
                {
                if (ak641is_trans_uni_to_ascii(&(*(tgg00_StackList *) sql__nil(mb_st , "vak641.p" , 20280))[sql__setrang((int ) stackentries[sql__setrang(ind , 0 , 256)] + -1 , 1 , 32767) + -1]))
                    {
                    conv_to_uni = 0;
                    (*(tgg00_StackList *) sql__nil(mb_st , "vak641.p" , 20284))[sql__setrang((int ) stackentries[sql__setrang(ind , 0 , 256)] , 1 , 32767) + -1].integer.C_1.etype = 39;
                    (*(tgg00_StackList *) sql__nil(mb_st , "vak641.p" , 20285))[sql__setrang((int ) stackentries[sql__setrang(ind , 0 , 256)] , 1 , 32767) + -1].integer.C_1.eop = 0;
                    (*(tgg00_StackList *) sql__nil(mb_st , "vak641.p" , 20286))[sql__setrang((int ) stackentries[sql__setrang(ind , 0 , 256)] + -1 , 1 , 32767) + -1].integer.C_1.etype = 39;
                    (*(tgg00_StackList *) sql__nil(mb_st , "vak641.p" , 20287))[sql__setrang((int ) stackentries[sql__setrang(ind , 0 , 256)] + -1 , 1 , 32767) + -1].integer.C_1.eop = 0;
                    }
                }
            else
                {
                conv_to_uni = 0;
                }
            }
        ind = sql__succ(ind , -2147483647 , 2147483647 , "vak641.p" , 20295);
        }
    if ((char ) sql__setrang(conv_to_uni , 0 , 1))
        {
        ind = 0;
        while (ind <= (int ) pair_cnt)
            {
            if ((int ) stackentries[sql__setrang(ind , 0 , 256)] > 1)
                {
                (*(tgg00_StackList *) sql__nil(mb_st , "vak641.p" , 20307))[sql__setrang((int ) stackentries[sql__setrang(ind , 0 , 256)] , 1 , 32767) + -1].integer.C_1.etype = 39;
                (*(tgg00_StackList *) sql__nil(mb_st , "vak641.p" , 20308))[sql__setrang((int ) stackentries[sql__setrang(ind , 0 , 256)] , 1 , 32767) + -1].integer.C_1.eop = 0;
                }
            ind = sql__succ(ind , -2147483647 , 2147483647 , "vak641.p" , 20311);
            }
        }
    _ak641unicode_conv_destroyed = (char ) sql__setrang(conv_to_uni , 0 , 1);
    cccprint("<ak641unicode_co");
    return(_ak641unicode_conv_destroyed);
    }



int ak641destroy_dummy_entries(mb_st, start_st, stop_st)
tgg00_StackListPtr mb_st;
tsp_int2 start_st;
tsp_int2 stop_st;
    {
    int _ak641destroy_dummy_entries;
    int ind;
    int diff;
    cccprint("AK641 ak641destr");
    diff = 0;
    ind = (int ) start_st + -1;
    while (ind <= (int ) stop_st)
        {
        if ((int ) (*(tgg00_StackList *) sql__nil(mb_st , "vak641.p" , 20337))[sql__setrang(ind , 1 , 32767) + -1].integer.C_1.etype == 39)
            {
            diff = sql__succ(diff , -2147483647 , 2147483647 , "vak641.p" , 20339);
            }
        else
            {
            (*(tgg00_StackList *) sql__nil(mb_st , "vak641.p" , 20341))[sql__setrang(ind - diff , 1 , 32767) + -1] = (*(tgg00_StackList *) sql__nil(mb_st , "vak641.p" , 20341))[sql__setrang(ind , 1 , 32767) + -1];
            }
        ind = sql__succ(ind , -2147483647 , 2147483647 , "vak641.p" , 20343);
        }
    _ak641destroy_dummy_entries = diff;
    cccprint("<ak641destroy_du");
    return(_ak641destroy_dummy_entries);
    }



int ak641decode_func(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    int curr_n;
    int check_n;
    int value_cnt;
    tak00_scolinf colin1;
    tsp_data_type search_datatype;
    tsp_data_type result_datatype;
    tsp_data_type ch_type;
    tsp_data_type ch1_type;
    ak641stackentries comp_st;
    ak641stackentries res_st;
    tsp_int2 pair_cnt;
    int ind;
    int diff;
    char dummy_found;
    tak_all_command_glob *ptoc_Var312;
    struct ptoc_Type59 *ptoc_Var313;
    tak_dml_info *ptoc_Var314;
    struct tgg00_MessBlock *ptoc_Var315;
    tgg00_QualBuf *ptoc_Var316;
    int ptoc_Var317;
    tgg00_StackEntry *ptoc_Var318;
    cccprint("AK641 ak641decod");
    ptoc_Var312 = &*acv;
    ptoc_Var313 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var312->a_ap_tree , "vak641.p" , 20375))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var314 = &*dmli;
    ptoc_Var315 = &ptoc_Var312->a_mblock;
    ptoc_Var316 = (tgg00_QualBuf *) sql__nil(ptoc_Var315->integer.C_4.mb_qual , "vak641.p" , 20375);
    result_datatype = (unsigned char ) sql__setrang(ptoc_Var314->d_datatype , 0 , 39);
    ptoc_Var314->d_datatype = 14;
    curr_n = (int ) ptoc_Var313->n_lo_level;
    colin1.sci_len = 0;
    ptoc_Var314->d_ch_datatype = 14;
    a640factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
    if ((char ) sql__setrang(g01unicode , 0 , 1) && (int ) ptoc_Var314->d_datatype == 2)
        {
        a641string_set_operator(&(*acv) , 66 , (int ) colin1.sci_len * 2 + 1 , 0 , (unsigned char)'\0' , (unsigned char)'\024');
        }
    comp_st[0] = (short ) sql__setrang((short ) ((int ) ptoc_Var316->integer.C_2.mfirst_free + -1) , -32768 , 32767);
    pair_cnt = 1;
    res_st[0] = 0;
    search_datatype = (unsigned char ) sql__setrang(ptoc_Var314->d_datatype , 0 , 39);
    ch_type = (unsigned char ) sql__setrang(ptoc_Var314->d_ch_datatype , 0 , 39);
    curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var312->a_ap_tree , "vak641.p" , 20401))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
    colin1.sci_len = 0;
    ptoc_Var314->d_ch_datatype = 14;
    a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
    if ((int ) ptoc_Var314->d_ch_datatype != 14)
        {
        if ((int ) ch_type != (int ) ptoc_Var314->d_ch_datatype)
            {
            if ((int ) ch_type != 14)
                {
                a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var312->a_ap_tree , "vak641.p" , 20412))[sql__setrang(curr_n , 0 , 32767)].n_pos);
                }
            else
                {
                ch_type = (unsigned char ) sql__setrang(ptoc_Var314->d_ch_datatype , 0 , 39);
                }
            }
        }
    if ((char ) sql__setrang(g01unicode , 0 , 1) && (int ) ptoc_Var314->d_datatype == 2)
        {
        a641string_set_operator(&(*acv) , 66 , (int ) colin1.sci_len * 2 + 1 , 0 , (unsigned char)'\0' , (unsigned char)'\024');
        }
    comp_st[sql__setrang((int ) pair_cnt , 0 , 256)] = (short ) sql__setrang((short ) ((int ) ptoc_Var316->integer.C_2.mfirst_free + -1) , -32768 , 32767);
    curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var312->a_ap_tree , "vak641.p" , 20428))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
    (*colin).sci_len = 0;
    ptoc_Var314->d_datatype = 14;
    ptoc_Var314->d_ch_datatype = 14;
    a640not_first_factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var312->a_return_segm , "vak641.p" , 20433)).integer.C_4.sp1r_returncode == 21000)
        {
        if ((int ) result_datatype != 14)
            {
            ptoc_Var314->d_datatype = (unsigned char ) sql__setrang(result_datatype , 0 , 39);
            }
        else
            {
            ptoc_Var314->d_datatype = (unsigned char ) sql__setrang(search_datatype , 0 , 39);
            }
        (*(tsp1_segment *) sql__nil(ptoc_Var312->a_return_segm , "vak641.p" , 20443)).integer.C_4.sp1r_returncode = 0;
        a640not_first_factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
        }
    if ((char ) sql__setrang(g01unicode , 0 , 1) && (int ) ptoc_Var314->d_datatype == 2)
        {
        a641string_set_operator(&(*acv) , 66 , (int ) (*colin).sci_len * 2 + 1 , 0 , (unsigned char)'\0' , (unsigned char)'\024');
        (*colin).sci_typ = 24;
        (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len * 2 + 1) , -32768 , 32767);
        }
    res_st[sql__setrang((int ) pair_cnt , 0 , 256)] = (short ) sql__setrang((short ) ((int ) ptoc_Var316->integer.C_2.mfirst_free + -1) , -32768 , 32767);
    result_datatype = (unsigned char ) sql__setrang(ptoc_Var314->d_datatype , 0 , 39);
    ch1_type = (unsigned char ) sql__setrang(ptoc_Var314->d_ch_datatype , 0 , 39);
    value_cnt = 3;
    t01int4(5 , "result_dtyp " , sql__ord((int ) result_datatype));
    t01int4(5 , "search_dtyp " , sql__ord((int ) search_datatype));
    t01int4(5 , "colin_dtyp  " , sql__ord((int ) (*colin).sci_typ));
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var312->a_return_segm , "vak641.p" , 20465)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var314->d_datatype = (unsigned char ) sql__setrang(search_datatype , 0 , 39);
        while ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var312->a_ap_tree , "vak641.p" , 20469))[sql__setrang(curr_n , 0 , 32767)].n_sa_level != 0 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var312->a_return_segm , "vak641.p" , 20470)).integer.C_4.sp1r_returncode == 0)
            {
            check_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var312->a_ap_tree , "vak641.p" , 20472))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var312->a_ap_tree , "vak641.p" , 20473))[sql__setrang(check_n , 0 , 32767)].n_symb == 43)
                {
                check_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var312->a_ap_tree , "vak641.p" , 20475))[sql__setrang(check_n , 0 , 32767)].n_sa_level;
                }
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var312->a_ap_tree , "vak641.p" , 20477))[sql__setrang(check_n , 0 , 32767)].n_symb == 44)
                {
                check_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var312->a_ap_tree , "vak641.p" , 20479))[sql__setrang(check_n , 0 , 32767)].n_sa_level;
                }
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var312->a_ap_tree , "vak641.p" , 20481))[sql__setrang(check_n , 0 , 32767)].n_sa_level == 0)
                {
                value_cnt = sql__succ(value_cnt , -2147483647 , 2147483647 , "vak641.p" , 20484);
                curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var312->a_ap_tree , "vak641.p" , 20485))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
                colin1.sci_len = 0;
                ptoc_Var314->d_ch_datatype = 14;
                ptoc_Var314->d_datatype = (unsigned char ) sql__setrang(result_datatype , 0 , 39);
                a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var312->a_return_segm , "vak641.p" , 20490)).integer.C_4.sp1r_returncode == 21000)
                    {
                    ptoc_Var314->d_datatype = (unsigned char ) sql__setrang(search_datatype , 0 , 39);
                    (*(tsp1_segment *) sql__nil(ptoc_Var312->a_return_segm , "vak641.p" , 20494)).integer.C_4.sp1r_returncode = 0;
                    a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
                    }
                if ((int ) ptoc_Var314->d_ch_datatype != 14)
                    {
                    if ((int ) ch1_type != (int ) ptoc_Var314->d_ch_datatype)
                        {
                        if ((int ) ch1_type != 14)
                            {
                            a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var312->a_ap_tree , "vak641.p" , 20506))[sql__setrang(curr_n , 0 , 32767)].n_pos);
                            }
                        else
                            {
                            ch1_type = (unsigned char ) sql__setrang(ptoc_Var314->d_ch_datatype , 0 , 39);
                            }
                        }
                    }
                if ((char ) sql__setrang(g01unicode , 0 , 1) && (int ) ptoc_Var314->d_datatype == 2)
                    {
                    a641string_set_operator(&(*acv) , 66 , (int ) colin1.sci_len * 2 + 1 , 0 , (unsigned char)'\0' , (unsigned char)'\024');
                    colin1.sci_typ = 24;
                    colin1.sci_iolen = (short ) sql__setrang((short ) ((int ) colin1.sci_len * 2 + 1) , -32768 , 32767);
                    }
                if ((int ) res_st[sql__setrang((int ) pair_cnt , 0 , 256)] == 0)
                    {
                    res_st[sql__setrang((int ) pair_cnt , 0 , 256)] = (short ) sql__setrang((short ) ((int ) ptoc_Var316->integer.C_2.mfirst_free + -1) , -32768 , 32767);
                    }
                else
                    {
                    res_st[0] = (short ) sql__setrang((short ) ((int ) ptoc_Var316->integer.C_2.mfirst_free + -1) , -32768 , 32767);
                    }
                if ((int ) result_datatype == 14 && (int ) ptoc_Var314->d_datatype != 14)
                    {
                    result_datatype = (unsigned char ) sql__setrang(ptoc_Var314->d_datatype , 0 , 39);
                    }
                ak641newcolin(&(*colin) , &colin1);
                }
            else
                {
                curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var312->a_ap_tree , "vak641.p" , 20539))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
                colin1.sci_len = 0;
                ptoc_Var314->d_ch_datatype = 14;
                a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
                if ((char ) sql__setrang(g01unicode , 0 , 1) && (int ) ptoc_Var314->d_datatype == 2)
                    {
                    a641string_set_operator(&(*acv) , 66 , (int ) colin1.sci_len * 2 + 1 , 0 , (unsigned char)'\0' , (unsigned char)'\024');
                    colin1.sci_typ = 24;
                    colin1.sci_iolen = (short ) sql__setrang((short ) ((int ) colin1.sci_len * 2 + 1) , -32768 , 32767);
                    }
                if (((ptoc_Var317 = value_cnt % 2) >= 0 ? ptoc_Var317 : ptoc_Var317 + 2) == 0)
                    {
                    if ((int ) ptoc_Var314->d_ch_datatype != 14)
                        {
                        if ((int ) ch1_type != (int ) ptoc_Var314->d_ch_datatype)
                            {
                            if ((int ) ch1_type != 14)
                                {
                                a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var312->a_ap_tree , "vak641.p" , 20565))[sql__setrang(curr_n , 0 , 32767)].n_pos);
                                }
                            else
                                {
                                ch1_type = (unsigned char ) sql__setrang(ptoc_Var314->d_ch_datatype , 0 , 39);
                                }
                            }
                        }
                    res_st[sql__setrang((int ) pair_cnt , 0 , 256)] = (short ) sql__setrang((short ) ((int ) ptoc_Var316->integer.C_2.mfirst_free + -1) , -32768 , 32767);
                    if ((int ) result_datatype == 14 && (int ) ptoc_Var314->d_datatype != 14)
                        {
                        result_datatype = (unsigned char ) sql__setrang(ptoc_Var314->d_datatype , 0 , 39);
                        }
                    ak641newcolin(&(*colin) , &colin1);
                    ptoc_Var314->d_datatype = (unsigned char ) sql__setrang(search_datatype , 0 , 39);
                    }
                else
                    {
                    pair_cnt = (short ) sql__setrang((short ) sql__succ((int ) pair_cnt , -32768 , 32767 , "vak641.p" , 20582) , -32768 , 32767);
                    if ((int ) ch_type != (int ) ptoc_Var314->d_ch_datatype)
                        {
                        if ((int ) ch_type != 14)
                            {
                            a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var312->a_ap_tree , "vak641.p" , 20589))[sql__setrang(curr_n , 0 , 32767)].n_pos);
                            }
                        else
                            {
                            ch_type = (unsigned char ) sql__setrang(ptoc_Var314->d_ch_datatype , 0 , 39);
                            }
                        }
                    comp_st[sql__setrang((int ) pair_cnt , 0 , 256)] = (short ) sql__setrang((short ) ((int ) ptoc_Var316->integer.C_2.mfirst_free + -1) , -32768 , 32767);
                    res_st[sql__setrang((int ) pair_cnt , 0 , 256)] = 0;
                    ptoc_Var314->d_datatype = (unsigned char ) sql__setrang(result_datatype , 0 , 39);
                    }
                value_cnt = sql__succ(value_cnt , -2147483647 , 2147483647 , "vak641.p" , 20599);
                }
            }
        ptoc_Var314->d_datatype = (unsigned char ) sql__setrang(result_datatype , 0 , 39);
        t01messblock(5 , "a641decode  " , &(*acv).a_mblock);
        t01int4(5 , "search_st   " , (int ) comp_st[0]);
        ptoc_Var317 = pair_cnt;
        if (1 <= ptoc_Var317)
            {
            ind = 1;
            do
                {
                t01int4(5 , "ind         " , ind);
                t01int4(5 , "comp_st     " , (int ) comp_st[sql__setrang(ind , 0 , 256)]);
                t01int4(5 , " res_st     " , (int ) res_st[sql__setrang(ind , 0 , 256)]);
                if (ind == ptoc_Var317)
                    {
                    break;
                    }
                ind += 1;
                }
            while (!0);
            }
        t01int4(5 , "def_st      " , (int ) res_st[0]);
        t01int4(5 , "result_dtyp " , sql__ord((int ) result_datatype));
        t01int4(5 , "search_dtyp " , sql__ord((int ) search_datatype));
        t01int4(5 , "colin_dtyp  " , sql__ord((int ) (*colin).sci_typ));
        if ((char ) sql__setrang(g01unicode , 0 , 1))
            {
            dummy_found = (char ) sql__setrang(ak641unicode_conv_destroyed(ptoc_Var315->integer.C_5.mb_st , comp_st , (short ) sql__setrang(pair_cnt , -32768 , 32767)) , 0 , 1);
            if (ak641unicode_conv_destroyed(ptoc_Var315->integer.C_5.mb_st , res_st , (short ) sql__setrang(pair_cnt , -32768 , 32767)))
                {
                (*colin).sci_typ = 2;
                (*colin).sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) (*colin).sci_len , -32768 , 32767 , "vak641.p" , 20626) , -32768 , 32767);
                dummy_found = 1;
                }
            if ((char ) sql__setrang(dummy_found , 0 , 1))
                {
                diff = ak641destroy_dummy_entries(ptoc_Var315->integer.C_5.mb_st , (short ) sql__setrang(comp_st[0] , -32768 , 32767) , (short ) sql__setrang((short ) ((int ) ptoc_Var316->integer.C_2.mfirst_free + -1) , -32768 , 32767));
                ptoc_Var316->integer.C_2.mfirst_free = (short ) sql__setrang((short ) ((int ) ptoc_Var316->integer.C_2.mfirst_free - diff) , -32768 , 32767);
                ptoc_Var316->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) ((int ) ptoc_Var316->integer.C_2.mqual_cnt - diff) , -32768 , 32767);
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var312->a_return_segm , "vak641.p" , 20640)).integer.C_4.sp1r_returncode == 0)
            {
            if ((int ) ptoc_Var316->integer.C_2.mfirst_free > (int ) ptoc_Var315->integer.C_2.mb_st_max)
                {
                a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var315->integer.C_2.mb_st_max);
                }
            else
                {
                if ((ptoc_Var317 = (int ) ptoc_Var314->d_datatype) == 10 || ptoc_Var317 == 11 || ptoc_Var317 == 13)
                    {
                    if ((int ) g01code.ctype == 0)
                        {
                        ptoc_Var314->d_datatype = 2;
                        }
                    else
                        {
                        ptoc_Var314->d_datatype = 3;
                        }
                    (*colin).sci_typ = (unsigned char ) sql__setrang(ptoc_Var314->d_datatype , 0 , 39);
                    }
                ptoc_Var318 = &(*(tgg00_StackList *) sql__nil(ptoc_Var315->integer.C_5.mb_st , "vak641.p" , 20660))[sql__setrang((int ) ptoc_Var316->integer.C_2.mfirst_free , 1 , 32767) + -1];
                ptoc_Var318->integer.C_1.etype = 55;
                ptoc_Var318->integer.C_4.eop_build_in = 23;
                ptoc_Var318->integer.C_1.epos = 0;
                ptoc_Var318->integer.C_1.elen_var = (short ) sql__setrang((short ) value_cnt , -32768 , 32767);
                ptoc_Var318->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                ptoc_Var318->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                ptoc_Var316->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var316->integer.C_2.mfirst_free , -32768 , 32767 , "vak641.p" , 20670) , -32768 , 32767);
                ptoc_Var316->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var316->integer.C_2.mqual_cnt , -32768 , 32767 , "vak641.p" , 20671) , -32768 , 32767);
                }
            }
        }
    cccprint("<ak641decode_fun");
    }



int ak641special_colin(colin, max_integer, max_sci_frac)
tak00_scolinf *colin;
tsp_int2 max_integer;
tsp_int2 max_sci_frac;
    {
    tak00_scolinf *ptoc_Var319;
    cccprint("AK641 ak641speci");
    ptoc_Var319 = &*colin;
    ptoc_Var319->sci_len = (short ) sql__setrang((short ) ((int ) max_integer + (int ) max_sci_frac) , -32768 , 32767);
    ptoc_Var319->sci_iolen = (short ) sql__setrang((short ) (((int ) ptoc_Var319->sci_len + 1) / 2 + 2) , -32768 , 32767);
    ptoc_Var319->sci_frac = (short ) sql__setrang(max_sci_frac , -32768 , 32767);
    cccprint("<ak641special_co");
    }



int ak641special_func(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    char no_more_fixed;
    int curr_n;
    int value_cnt;
    tak00_scolinf colin1;
    tgg_stack_op_build_in op;
    tsp_data_type ch_type;
    tsp_data_type d_type;
    tak_all_command_glob *ptoc_Var320;
    struct ptoc_Type59 *ptoc_Var321;
    tak_dml_info *ptoc_Var322;
    tak00_scolinf *ptoc_Var323;
    tak00_scolinf *ptoc_Var324;
    tak00_scolinf *ptoc_Var325;
    tak00_scolinf *ptoc_Var326;
    tak00_scolinf *ptoc_Var327;
    int ptoc_Var328;
    struct tgg00_MessBlock *ptoc_Var329;
    tgg00_QualBuf *ptoc_Var330;
    tgg00_StackEntry *ptoc_Var331;
    cccprint("AK641 ak641speci");
    ptoc_Var320 = &*acv;
    ptoc_Var321 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var320->a_ap_tree , "vak641.p" , 20720))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var322 = &*dmli;
    curr_n = (int ) ptoc_Var321->n_lo_level;
    (*colin).sci_len = 0;
    (*colin).sci_frac = 0;
    ptoc_Var322->d_ch_datatype = 14;
    a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
    no_more_fixed = (char ) sql__setrang((int ) (*colin).sci_frac == -1 , 0 , 1);
    ch_type = (unsigned char ) sql__setrang(ptoc_Var322->d_ch_datatype , 0 , 39);
    d_type = (unsigned char ) sql__setrang(ptoc_Var322->d_datatype , 0 , 39);
    value_cnt = 1;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var320->a_return_segm , "vak641.p" , 20732)).integer.C_4.sp1r_returncode == 0)
        {
        while ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var320->a_ap_tree , "vak641.p" , 20735))[sql__setrang(curr_n , 0 , 32767)].n_sa_level != 0 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var320->a_return_segm , "vak641.p" , 20736)).integer.C_4.sp1r_returncode == 0)
            {
            curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var320->a_ap_tree , "vak641.p" , 20738))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
            colin1.sci_len = 0;
            colin1.sci_frac = 0;
            ptoc_Var322->d_ch_datatype = 14;
            a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &curr_n);
            t01int4(5 , "colsci-typ V" , sql__ord((int ) (*colin).sci_typ));
            t01int4(5 , "co1sci-typ V" , sql__ord((int ) colin1.sci_typ));
            t01int4(5 , "n_pos      V" , (*(tak_ap_max_tree *) sql__nil(ptoc_Var320->a_ap_tree , "vak641.p" , 20745))[sql__setrang(curr_n , 0 , 32767)].n_pos);
            t01int4(5 , "colsci-len V" , (int ) (*colin).sci_len);
            t01int4(5 , "co1sci-len V" , (int ) colin1.sci_len);
            t01int4(5 , "colsci-fra V" , (int ) (*colin).sci_frac);
            t01int4(5 , "co1sci-fra V" , (int ) colin1.sci_frac);
            if ((int ) ptoc_Var322->d_ch_datatype != 14)
                {
                if ((int ) ch_type != (int ) ptoc_Var322->d_ch_datatype)
                    {
                    if ((int ) ch_type != 14)
                        {
                        a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var320->a_ap_tree , "vak641.p" , 20757))[sql__setrang(curr_n , 0 , 32767)].n_pos);
                        }
                    else
                        {
                        ch_type = (unsigned char ) sql__setrang(ptoc_Var322->d_ch_datatype , 0 , 39);
                        }
                    }
                }
            if ((int ) d_type == 14 && (int ) ptoc_Var322->d_datatype != 14)
                {
                *colin = colin1;
                d_type = (unsigned char ) sql__setrang(ptoc_Var322->d_datatype , 0 , 39);
                }
            if ((int ) colin1.sci_typ == 0 && !(char ) sql__setrang(no_more_fixed , 0 , 1))
                {
                if ((int ) (*colin).sci_frac < (int ) colin1.sci_frac)
                    {
                    if ((int ) (*colin).sci_len - (int ) (*colin).sci_frac < (int ) colin1.sci_len - (int ) colin1.sci_frac)
                        {
                        ptoc_Var323 = &colin1;
                        ak641special_colin(&(*colin) , (short ) sql__setrang((short ) ((int ) ptoc_Var323->sci_len - (int ) ptoc_Var323->sci_frac) , -32768 , 32767) , (short ) sql__setrang(ptoc_Var323->sci_frac , -32768 , 32767));
                        }
                    else
                        {
                        ptoc_Var324 = &*colin;
                        ak641special_colin(&(*colin) , (short ) sql__setrang((short ) ((int ) ptoc_Var324->sci_len - (int ) ptoc_Var324->sci_frac) , -32768 , 32767) , (short ) sql__setrang(colin1.sci_frac , -32768 , 32767));
                        }
                    }
                else
                    {
                    if ((int ) (*colin).sci_len - (int ) (*colin).sci_frac < (int ) colin1.sci_len - (int ) colin1.sci_frac)
                        {
                        ptoc_Var325 = &colin1;
                        ak641special_colin(&(*colin) , (short ) sql__setrang((short ) ((int ) ptoc_Var325->sci_len - (int ) ptoc_Var325->sci_frac) , -32768 , 32767) , (short ) sql__setrang((*colin).sci_frac , -32768 , 32767));
                        }
                    else
                        {
                        ptoc_Var326 = &*colin;
                        ak641special_colin(&(*colin) , (short ) sql__setrang((short ) ((int ) ptoc_Var326->sci_len - (int ) ptoc_Var326->sci_frac) , -32768 , 32767) , (short ) sql__setrang(ptoc_Var326->sci_frac , -32768 , 32767));
                        }
                    }
                if ((int ) (*colin).sci_len > 38)
                    {
                    ptoc_Var327 = &*colin;
                    ptoc_Var327->sci_len = 38;
                    ptoc_Var327->sci_frac = -1;
                    ptoc_Var327->sci_typ = 1;
                    ptoc_Var327->sci_iolen = (short ) sql__setrang((short ) (((int ) ptoc_Var327->sci_len + 1) / 2 + 2) , -32768 , 32767);
                    no_more_fixed = 1;
                    }
                }
            else
                {
                if ((int ) (*colin).sci_len < (int ) colin1.sci_len)
                    {
                    (*colin).sci_len = (short ) sql__setrang(colin1.sci_len , -32768 , 32767);
                    (*colin).sci_iolen = (short ) sql__setrang(colin1.sci_iolen , -32768 , 32767);
                    }
                if ((ptoc_Var328 = (int ) colin1.sci_typ) == 1 || ptoc_Var328 == 12)
                    {
                    (*colin).sci_typ = 12;
                    (*colin).sci_frac = -1;
                    }
                }
            value_cnt = value_cnt + 1;
            }
        ptoc_Var329 = &ptoc_Var320->a_mblock;
        ptoc_Var330 = (tgg00_QualBuf *) sql__nil(ptoc_Var329->integer.C_4.mb_qual , "vak641.p" , 20840);
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var320->a_return_segm , "vak641.p" , 20841)).integer.C_4.sp1r_returncode == 0)
            {
            if ((int ) ptoc_Var330->integer.C_2.mfirst_free > (int ) ptoc_Var329->integer.C_2.mb_st_max)
                {
                a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var329->integer.C_2.mb_st_max);
                }
            else
                {
                switch ((int ) ptoc_Var321->n_symb)
                    {
                    case 182 :
                        op = 20;
                        break;
                    case 183 :
                        op = 21;
                        break;
                    case 123 :
                        op = 22;
                        break;
                    default :
                        (unsigned char ) sql__caseerr("vak641.p" , 20864);
                        break;
                    }
                ptoc_Var331 = &(*(tgg00_StackList *) sql__nil(ptoc_Var329->integer.C_5.mb_st , "vak641.p" , 20873))[sql__setrang((int ) ptoc_Var330->integer.C_2.mfirst_free , 1 , 32767) + -1];
                ptoc_Var331->integer.C_1.etype = 55;
                ptoc_Var331->integer.C_4.eop_build_in = (unsigned char ) sql__setrang(op , 0 , 67);
                ptoc_Var331->integer.C_1.epos = 0;
                ptoc_Var331->integer.C_1.elen_var = (short ) sql__setrang((short ) value_cnt , -32768 , 32767);
                ptoc_Var331->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                ptoc_Var331->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                ptoc_Var330->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var330->integer.C_2.mfirst_free , -32768 , 32767 , "vak641.p" , 20883) , -32768 , 32767);
                ptoc_Var330->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var330->integer.C_2.mqual_cnt , -32768 , 32767 , "vak641.p" , 20884) , -32768 , 32767);
                }
            }
        }
    t01int4(5 , "erg col_len " , (int ) (*colin).sci_len);
    t01int4(5 , "erg col_iole" , (int ) (*colin).sci_iolen);
    t01int4(5 , "erg col_frac" , (int ) (*colin).sci_frac);
    t01int4(5 , "erg col_typ " , sql__ord((int ) (*colin).sci_typ));
    cccprint("<ak641special_fu");
    }



int a641check_datetime(acv, dmli, datatyp)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tsp_data_type datatyp;
    {
    tak_all_command_glob *ptoc_Var332;
    tak_dml_info *ptoc_Var333;
    struct tgg00_MessBlock *ptoc_Var334;
    tgg00_QualBuf *ptoc_Var335;
    tgg00_StackEntry *ptoc_Var336;
    cccprint("AK641 a641check_");
    t01int4(5 , "datatyp     " , sql__ord((int ) datatyp));
    ptoc_Var332 = &*acv;
    ptoc_Var333 = &*dmli;
    ptoc_Var334 = &ptoc_Var332->a_mblock;
    ptoc_Var335 = (tgg00_QualBuf *) sql__nil(ptoc_Var334->integer.C_4.mb_qual , "vak641.p" , 20910);
    ptoc_Var336 = &(*(tgg00_StackList *) sql__nil(ptoc_Var334->integer.C_5.mb_st , "vak641.p" , 20910))[sql__setrang((int ) ptoc_Var335->integer.C_2.mfirst_free , 1 , 32767) + -1];
    if ((int ) ptoc_Var335->integer.C_2.mfirst_free > (int ) ptoc_Var334->integer.C_2.mb_st_max)
        {
        a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var334->integer.C_2.mb_st_max);
        }
    else
        {
        ptoc_Var332->a_date_time_used = 1;
        ptoc_Var336->integer.C_1.etype = 55;
        ptoc_Var336->integer.C_4.eop_build_in = 1;
        if ((int ) datatyp == 13)
            {
            if ((int ) ptoc_Var332->a_dt_format != 1)
                {
                if ((int ) ptoc_Var332->a_dt_format == 6)
                    {
                    ptoc_Var336->integer.C_4.eformat = 6;
                    }
                else
                    {
                    if ((int ) ptoc_Var332->a_dt_format == 8)
                        {
                        ptoc_Var336->integer.C_4.eformat = (unsigned char ) sql__setrang(ptoc_Var332->a_dt_format , 0 , 9);
                        }
                    else
                        {
                        ptoc_Var336->integer.C_4.eformat = 2;
                        }
                    }
                }
            else
                {
                ptoc_Var336->integer.C_4.eformat = 1;
                }
            }
        else
            {
            ptoc_Var336->integer.C_4.eformat = (unsigned char ) sql__setrang(ptoc_Var332->a_dt_format , 0 , 9);
            }
        ptoc_Var336->integer.C_4.edatatype = (unsigned char ) sql__setrang(datatyp , 0 , 39);
        _ptoc_ASSIGN(unsigned char , 3, ptoc_Var336->integer.C_4.elanguage, ptoc_Var332->a_ak_language)
        ptoc_Var336->integer.C_4.elength = (unsigned char)'\0';
        ptoc_Var335->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var335->integer.C_2.mfirst_free , -32768 , 32767 , "vak641.p" , 20944) , -32768 , 32767);
        ptoc_Var335->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var335->integer.C_2.mqual_cnt , -32768 , 32767 , "vak641.p" , 20945) , -32768 , 32767);
        }
    cccprint("<a641check_datet");
    }

static
  tsp8_uni_opt_set ptoc_Var337 = 
  {
  (unsigned char)'\001'
  };
static
  unsigned char ptoc_Var338[1] = 
  {
  (unsigned char)'\0'
  };
static
  tak_priv_desc ptoc_Var339 = 
  {
  (unsigned char)'\0'
  };


int a641_get_name(acv, part_ptr, buf_pos, buf_len, get_owner, objecttype, returned_name)
tak_all_command_glob *acv;
tsp1_part_ptr part_ptr;
tsp_int4 buf_pos;
tsp_int2 buf_len;
char get_owner;
tsp_int2 objecttype;
tsp00_KnlIdentifier returned_name;
    {
    char ok;
    char kl_gr;
    unsigned char one_c;
    tsp_int4 i;
    tsp_int4 start;
    tsp_int2 put_node;
    tsp_int2 last_node;
    tsp_int4 err_char_no;
    tsp_int4 temp_cmd_len;
    tsp_int4 i_buf_len;
    int auth_node;
    tsp8_uni_error uni_err;
    tsp1_part_ptr temp_cmd_ptr;
    tak_scanner_glob scvh;
    tsp_knl_identifier authid;
    tsp_knl_identifier object_ident;
    tak_syspointerarr sparr;
    tak_sysbufferaddress sys_buf;
    tak_sysbufferaddress domain_ref;
    tak_sysbufferaddress domain_def;
    tak_privilege priv;
    tak_all_command_glob *ptoc_Var340;
    tsp8_uni_opt_set ptoc_Var341;
    tak_scanner_glob *ptoc_Var342;
    int ptoc_Var343;
    unsigned char ptoc_Var344[1];
    tak_priv_desc ptoc_Var345;
    cccprint("AK641 a641_get_n");
    ptoc_Var340 = &*acv;
    _ptoc_ASSIGN(unsigned char , 64, returned_name, a01_il_b_identifier)
    i_buf_len = (int ) buf_len;
    if ((char ) sql__setrang(g01unicode , 0 , 1))
        {
        s80uni_trans((tsp00_MoveObj *) &(*(tsp1_part *) sql__nil(part_ptr , "vak641.p" , 20992)).integer.C_1.sp1p_buf[sql__setrang(buf_pos , 1 , 8192000) + -1] , i_buf_len , 20 , (tsp00_MoveObj *) &(*(tsp1_part *) sql__nil(part_ptr , "vak641.p" , 20993)).integer.C_1.sp1p_buf[sql__setrang(buf_pos , 1 , 8192000) + -1] , &i_buf_len , 20 , *(tsp8_uni_opt_set *) (_ptoc_MOVE(unsigned char , 1, ptoc_Var341, ptoc_Var337) , ptoc_Var341) , &uni_err , &err_char_no);
        if ((int ) uni_err != 0)
            {
            a07_b_put_error(&(*acv) , 6527 , 1);
            }
        }
    else
        {
        i = buf_pos;
        kl_gr = 1;
        while (i < buf_pos + (int ) buf_len)
            {
            start = i;
            if ((char ) sql__setrang(kl_gr , 0 , 1))
                {
                while (i < buf_pos + (int ) buf_len && (int ) (*(tsp1_part *) sql__nil(part_ptr , "vak641.p" , 21011)).integer.C_1.sp1p_buf[sql__setrang(i , 1 , 8192000) + -1] != 34)
                    i = sql__succ(i , -2147483647 , 2147483647 , "vak641.p" , 21012);
                one_c = (unsigned char ) sql__setrang((*(tsp1_part *) sql__nil(part_ptr , "vak641.p" , 21014)).integer.C_1.sp1p_buf[sql__setrang(i , 1 , 8192000) + -1] , 0 , 255);
                }
            else
                {
                while (i < buf_pos + (int ) buf_len && (int ) (*(tsp1_part *) sql__nil(part_ptr , "vak641.p" , 21018)).integer.C_1.sp1p_buf[sql__setrang(i , 1 , 8192000) + -1] != (int ) one_c)
                    i = sql__succ(i , -2147483647 , 2147483647 , "vak641.p" , 21019);
                }
            if (i >= buf_pos + (int ) buf_len)
                {
                i = buf_pos + (int ) buf_len;
                }
            if ((char ) sql__setrang(kl_gr , 0 , 1))
                {
                s30map(g02codetables.tables[3] , (*(tsp1_part *) sql__nil(part_ptr , "vak641.p" , 21029)).integer.C_1.sp1p_buf , start , (*(tsp1_part *) sql__nil(part_ptr , "vak641.p" , 21030)).integer.C_1.sp1p_buf , start , i - start + 1);
                }
            kl_gr = (char ) sql__setrang(!(char ) sql__setrang(kl_gr , 0 , 1) , 0 , 1);
            i = sql__succ(i , -2147483647 , 2147483647 , "vak641.p" , 21033);
            }
        }
    temp_cmd_ptr = ptoc_Var340->a_cmd_part;
    temp_cmd_len = (*(tsp1_part *) sql__nil(ptoc_Var340->a_cmd_part , "vak641.p" , 21039)).integer.C_2.sp1p_buf_len;
    scvh = ptoc_Var340->a_scv;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var340->a_return_segm , "vak641.p" , 21041)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var340->a_cmd_part = part_ptr;
        (*(tsp1_part *) sql__nil(ptoc_Var340->a_cmd_part , "vak641.p" , 21046)).integer.C_2.sp1p_buf_len = buf_pos + i_buf_len + -1;
        ptoc_Var342 = &ptoc_Var340->a_scv;
        ptoc_Var342->sc_symb = 0;
        ptoc_Var342->sc_newpos = buf_pos;
        ptoc_Var342->sc_sypos = buf_pos;
        ptoc_Var342->sc_reserved_check = 0;
        ptoc_Var342->sc_double_quote = 0;
        ptoc_Var342->sc_pack = 1;
        ok = 0;
        _ptoc_ASSIGN(unsigned char , 64, authid, a01_il_b_identifier)
        _ptoc_ASSIGN(unsigned char , 64, object_ident, a01_il_b_identifier)
        a01_next_symbol(&(*acv));
        if ((int ) ptoc_Var340->a_scv.sc_symb == 42)
            {
            a02_put_identifier(&(*acv) , &put_node , &put_node);
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var340->a_return_segm , "vak641.p" , 21065)).integer.C_4.sp1r_returncode == 0)
                {
                if ((int ) ptoc_Var340->a_scv.sc_symb == 12 && (*acv).a_scv.sc_newpos < buf_pos + i_buf_len + -1)
                    {
                    a01_next_symbol(&(*acv));
                    if ((int ) ptoc_Var340->a_scv.sc_symb == 42)
                        {
                        (*(tak_ap_max_tree *) sql__nil(ptoc_Var340->a_ap_tree , "vak641.p" , 21076))[sql__setrang((int ) put_node , 0 , 32767)].n_symb = 43;
                        a02_put_identifier(&(*acv) , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var340->a_ap_tree , "vak641.p" , 21077))[sql__setrang((int ) put_node , 0 , 32767)].n_sa_level , &last_node);
                        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var340->a_return_segm , "vak641.p" , 21079)).integer.C_4.sp1r_returncode == 0)
                            {
                            auth_node = (int ) put_node;
                            a06get_username(&(*acv) , &auth_node , authid);
                            a05identifier_get(&(*acv) , (int ) last_node , (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var340->a_ap_tree , "vak641.p" , 21084))[sql__setrang((int ) last_node , 0 , 32767)].n_length , object_ident);
                            }
                        }
                    else
                        {
                        a07_b_put_error(&(*acv) , 6070 , buf_pos);
                        }
                    }
                else
                    {
                    a05identifier_get(&(*acv) , (int ) put_node , (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var340->a_ap_tree , "vak641.p" , 21095))[sql__setrang((int ) put_node , 0 , 32767)].n_length , object_ident);
                    }
                }
            }
        else
            {
            a07_b_put_error(&(*acv) , 6070 , buf_pos);
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var340->a_return_segm , "vak641.p" , 21107)).integer.C_4.sp1r_returncode == 0)
        {
        t01int4(5 , "objecttype  " , (int ) objecttype);
        t01int4(5 , "cak_ctable  " , sql__ord(1));
        switch ((int ) (unsigned char ) objecttype)
            {
            case 16 :
                if (_ptoc_MEMCMP(unsigned char , 64, authid , a01_il_b_identifier) == 0 ? 1 : 0)
                    {
                    _ptoc_ASSIGN(unsigned char , 64, authid, ptoc_Var340->a_curr_user_name)
                    }
                if (a12dbproc_exist(&(*acv) , authid , object_ident , 1 , &sys_buf))
                    {
                    ok = 1;
                    }
                break;
            case 70 :
                a12get_domain(&(*acv) , authid , object_ident , (int ) put_node , &domain_ref , &domain_def);
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var340->a_return_segm , "vak641.p" , 21129)).integer.C_4.sp1r_returncode == 0)
                    {
                    ok = 1;
                    }
                else
                    {
                    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var340->a_return_segm , "vak641.p" , 21133)).integer.C_4.sp1r_returncode == (int ) a07_return_code(6498 , (unsigned char ) sql__setrang(ptoc_Var340->a_sqlmode , 0 , 3)))
                        {
                        (*(tsp1_segment *) sql__nil(ptoc_Var340->a_return_segm , "vak641.p" , 21137)).integer.C_4.sp1r_returncode = 0;
                        (*(tsp1_segment *) sql__nil(ptoc_Var340->a_return_segm , "vak641.p" , 21138)).integer.C_4.sp1r_errorpos = 0;
                        }
                    }
                break;
            case 82 :
                if (a23exist_sequence(&(*acv) , (int ) put_node , &sys_buf , authid , object_ident))
                    {
                    ok = 1;
                    }
                else
                    {
                    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var340->a_return_segm , "vak641.p" , 21149)).integer.C_4.sp1r_returncode == (int ) a07_return_code(6518 , (unsigned char ) sql__setrang(ptoc_Var340->a_sqlmode , 0 , 3)))
                        {
                        (*(tsp1_segment *) sql__nil(ptoc_Var340->a_return_segm , "vak641.p" , 21153)).integer.C_4.sp1r_returncode = 0;
                        (*(tsp1_segment *) sql__nil(ptoc_Var340->a_return_segm , "vak641.p" , 21154)).integer.C_4.sp1r_errorpos = 0;
                        }
                    }
                break;
            case 72 :
                if (a06_table_exist(&(*acv) , 1 , authid , object_ident , &sparr , 0))
                    {
                    if ((char ) sql__setrang(sparr.psynfound , 0 , 1))
                        {
                        ok = 1;
                        }
                    }
                break;
            case 10 :
            case 1 :
                if (a06_table_exist(&(*acv) , 1 , authid , object_ident , &sparr , 0))
                    {
                    if ((int ) (unsigned char ) objecttype == 10 && !(char ) sql__setrang(sparr.psynfound , 0 , 1) && ((ptoc_Var343 = (int ) (*(struct tak_systembuffer *) sql__nil(sparr.pbasep , "vak641.p" , 21179)).integer.C_1.sbase.btablekind) == 5 || ptoc_Var343 == 7 || ptoc_Var343 == 10 || ptoc_Var343 == 14) || (int ) (unsigned char ) objecttype == 1 && !(char ) sql__setrang(sparr.psynfound , 0 , 1) && ((ptoc_Var343 = (int ) (*(struct tak_systembuffer *) sql__nil(sparr.pbasep , "vak641.p" , 21186)).integer.C_1.sbase.btablekind) == 1 || ptoc_Var343 == 2 || ptoc_Var343 == 15))
                        {
                        a06_get_priv(&(*acv) , &sparr.pbasep , &priv);
                        if (!sql__erq(sizeof(unsigned char [1]) , priv.integer.C_1.priv_all_set , ptoc_Var338) || !sql__erq(sizeof(tak_priv_desc ) , priv.integer.C_1.priv_col_exist , ptoc_Var339))
                            {
                            ok = 1;
                            }
                        }
                    }
                break;
            default :
                (unsigned char ) sql__caseerr("vak641.p" , 21113);
                break;
            }
        if ((char ) sql__setrang(ok , 0 , 1))
            {
            if ((char ) sql__setrang(get_owner , 0 , 1))
                {
                _ptoc_ASSIGN(unsigned char , 64, returned_name, authid)
                }
            else
                {
                _ptoc_ASSIGN(unsigned char , 64, returned_name, object_ident)
                }
            }
        }
    ptoc_Var340->a_cmd_part = temp_cmd_ptr;
    (*(tsp1_part *) sql__nil(ptoc_Var340->a_cmd_part , "vak641.p" , 21216)).integer.C_2.sp1p_buf_len = temp_cmd_len;
    ptoc_Var340->a_scv = scvh;
    cccprint("<a641_get_name  ");
    }



int ak641get_name(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    int curr_n;
    tsp_int4 curr_name_len;
    tsp00_KnlIdentifier returned_name;
    tak00_columninfo colinf;
    tak_all_command_glob *ptoc_Var346;
    struct ptoc_Type59 *ptoc_Var347;
    tak_dml_info *ptoc_Var348;
    struct tgg00_MessBlock *ptoc_Var349;
    tgg00_QualBuf *ptoc_Var350;
    tgg00_DataPart *ptoc_Var351;
    tgg00_StackEntry *ptoc_Var352;
    tak00_columninfo *ptoc_Var353;
    tak_parsinforecord *ptoc_Var354;
    struct ptoc_Type147 *ptoc_Var355;
    cccprint("AK641 ak641get_n");
    ptoc_Var346 = &*acv;
    ptoc_Var347 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var346->a_ap_tree , "vak641.p" , 21238))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var348 = &*dmli;
    if ((char ) sql__setrang(g01unicode , 0 , 1))
        {
        ptoc_Var348->d_datatype = 24;
        }
    else
        {
        ptoc_Var348->d_datatype = 2;
        }
    curr_n = (int ) ptoc_Var347->n_lo_level;
    (*colin).sci_len = (short ) sql__setrang((short ) (sizeof(tsp00_C64 ) / 2) , -32768 , 32767);
    (*colin).sci_frac = 0;
    (*colin).sci_typ = (unsigned char ) sql__setrang(ptoc_Var348->d_datatype , 0 , 39);
    (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len * a01char_size + 1) , -32768 , 32767);
    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var346->a_ap_tree , "vak641.p" , 21251))[sql__setrang(curr_n , 0 , 32767)].n_symb == 48)
        {
        a641_get_name(&(*acv) , ptoc_Var346->a_cmd_part , (*(tak_ap_max_tree *) sql__nil(ptoc_Var346->a_ap_tree , "vak641.p" , 21255))[sql__setrang(curr_n , 0 , 32767)].n_pos , (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var346->a_ap_tree , "vak641.p" , 21255))[sql__setrang(curr_n , 0 , 32767)].n_length , -32768 , 32767) , (char ) sql__setrang((int ) ptoc_Var347->n_symb == 239 , 0 , 1) , (short ) sql__setrang(ptoc_Var347->n_subproc , -32768 , 32767) , returned_name);
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var346->a_return_segm , "vak641.p" , 21257)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var349 = &ptoc_Var346->a_mblock;
            ptoc_Var350 = (tgg00_QualBuf *) sql__nil(ptoc_Var349->integer.C_4.mb_qual , "vak641.p" , 21259);
            ptoc_Var351 = (tgg00_DataPart *) sql__nil(ptoc_Var349->integer.C_3.mb_data , "vak641.p" , 21259);
            if ((int ) ptoc_Var350->integer.C_2.mfirst_free > (int ) ptoc_Var349->integer.C_2.mb_st_max)
                {
                a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var349->integer.C_2.mb_st_max);
                }
            else
                {
                ptoc_Var350->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var350->integer.C_2.mqual_cnt , -32768 , 32767 , "vak641.p" , 21267) , -32768 , 32767);
                ptoc_Var352 = &(*(tgg00_StackList *) sql__nil(ptoc_Var349->integer.C_5.mb_st , "vak641.p" , 21268))[sql__setrang((int ) ptoc_Var350->integer.C_2.mfirst_free , 1 , 32767) + -1];
                ptoc_Var352->integer.C_1.etype = 22;
                ptoc_Var352->integer.C_1.eop = 0;
                ptoc_Var352->integer.C_1.epos = (short ) sql__setrang((short ) (ptoc_Var349->integer.C_2.mb_data_len + 1) , -32768 , 32767);
                ptoc_Var352->integer.C_1.elen_var = 0;
                ptoc_Var352->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                ptoc_Var352->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                if (_ptoc_MEMCMP(unsigned char , 64, returned_name , a01_il_b_identifier) == 0 ? 1 : 0)
                    {
                    ptoc_Var351->integer.C_1.mbp_buf[sql__setrang(ptoc_Var349->integer.C_2.mb_data_len + 1 , 1 , 8192000) + -1] = (unsigned char)'\377';
                    (*(tgg00_StackList *) sql__nil(ptoc_Var349->integer.C_5.mb_st , "vak641.p" , 21282))[sql__setrang((int ) ptoc_Var350->integer.C_2.mfirst_free , 1 , 32767) + -1].integer.C_1.elen_var = 1;
                    }
                else
                    {
                    if ((int ) ptoc_Var348->d_datatype == 24)
                        {
                        ptoc_Var351->integer.C_1.mbp_buf[sql__setrang(ptoc_Var349->integer.C_2.mb_data_len + 1 , 1 , 8192000) + -1] = (unsigned char)'\001';
                        }
                    else
                        {
                        ptoc_Var351->integer.C_1.mbp_buf[sql__setrang(ptoc_Var349->integer.C_2.mb_data_len + 1 , 1 , 8192000) + -1] = (unsigned char)' ';
                        }
                    curr_name_len = s30lnr_defbyte((tsp00_KnlIdentifier *) returned_name , (unsigned char ) sql__setrang(ptoc_Var351->integer.C_1.mbp_buf[sql__setrang(ptoc_Var349->integer.C_2.mb_data_len + 1 , 1 , 8192000) + -1] , 0 , 255) , 1 , sizeof(tsp00_C64 ));
                    (*(tgg00_StackList *) sql__nil(ptoc_Var349->integer.C_5.mb_st , "vak641.p" , 21294))[sql__setrang((int ) ptoc_Var350->integer.C_2.mfirst_free , 1 , 32767) + -1].integer.C_1.elen_var = (short ) sql__setrang((short ) (curr_name_len + 1) , -32768 , 32767);
                    g10mv5("VAK641" , 8 , sizeof(tsp00_C64 ) , ptoc_Var349->integer.C_2.mb_data_size , returned_name , 1 , ptoc_Var351->integer.C_1.mbp_buf , ptoc_Var349->integer.C_2.mb_data_len + 2 , curr_name_len , &(*(tsp1_segment *) sql__nil(ptoc_Var346->a_return_segm , "vak641.p" , 21298)).integer.C_4.sp1r_returncode);
                    }
                ptoc_Var349->integer.C_2.mb_data_len = ptoc_Var349->integer.C_2.mb_data_len + (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var349->integer.C_5.mb_st , "vak641.p" , 21301))[sql__setrang((int ) ptoc_Var350->integer.C_2.mfirst_free , 1 , 32767) + -1].integer.C_1.elen_var;
                ptoc_Var350->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var350->integer.C_2.mfirst_free , -32768 , 32767 , "vak641.p" , 21302) , -32768 , 32767);
                ptoc_Var350->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var350->integer.C_2.mqual_cnt , -32768 , 32767 , "vak641.p" , 21303) , -32768 , 32767);
                }
            t01moveobj(5 , ptoc_Var351->integer.C_1.mbp_buf , 1 , ptoc_Var349->integer.C_2.mb_data_len);
            }
        }
    else
        {
        colinf = a642standard_colinfo;
        ptoc_Var353 = &colinf;
        ptoc_Var353->integer.C_1.cdatatyp = (unsigned char ) sql__setrang(ptoc_Var348->d_datatype , 0 , 39);
        ptoc_Var353->integer.C_1.cdatalen = (short ) sql__setrang((short ) (sizeof(tsp00_C64 ) + 10) , -32768 , 32767);
        ptoc_Var353->integer.C_1.cinoutlen = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var353->integer.C_1.cdatalen * a01char_size , -2147483647 , 2147483647 , "vak641.p" , 21320) , -32768 , 32767);
        ptoc_Var348->d_colptr = (tak00_columninfo *) &colinf;
        a640factor(&(*acv) , &(*dmli) , &(*colin) , &curr_n);
        ptoc_Var348->d_colptr = (tak00_columninfo *) (int *) 0;
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var346->a_return_segm , "vak641.p" , 21326)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var354 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var348->d_sparr.pparsp , "vak641.p" , 21329)).integer.C_23.sparsinfo;
            ptoc_Var355 = &ptoc_Var354->integer.C_2.p_pars_infos[sql__setrang((int ) ptoc_Var354->integer.C_2.p_cnt_infos , 1 , 2025) + -1];
            ptoc_Var355->fp_kind = 7;
            ptoc_Var355->tak_fp_kind_type.C_fp_get_name_value.fp_sc_symbol = (unsigned char ) sql__setrang(ptoc_Var347->n_symb , 0 , 240);
            ptoc_Var355->tak_fp_kind_type.C_fp_get_name_value.fp_objecttype[0] = (unsigned char ) sql__setrang((unsigned char ) ptoc_Var347->n_subproc , 0 , 255);
            }
        }
    cccprint("<ak641get_name  ");
    }



int a641col_function(acv, dmli, frec, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_factorrec *frec;
tak00_scolinf *colin;
int act_node;
    {
    char rem_change_date_time;
    int startstack;
    char const_value_expr;
    tsp_int2 st_begin;
    tsp_int2 st_end;
    tsp_int2 st_index;
    tsp_int2 expr_st_cnt;
    tak_all_command_glob *ptoc_Var356;
    struct ptoc_Type59 *ptoc_Var357;
    tak_dml_info *ptoc_Var358;
    tak_factorrec *ptoc_Var359;
    int ptoc_Var360;
    struct tgg00_MessBlock *ptoc_Var361;
    tgg00_QualBuf *ptoc_Var362;
    cccprint("AK641 a641col_fu");
    t01int4(5 , "act_node    " , act_node);
    ptoc_Var356 = &*acv;
    ptoc_Var357 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var356->a_ap_tree , "vak641.p" , 21365))[sql__setrang(act_node , 0 , 32767)];
    ptoc_Var358 = &*dmli;
    ptoc_Var359 = &*frec;
    if ((int ) ptoc_Var356->a_allow_functions == 1)
        {
        ptoc_Var356->a_allow_functions = 2;
        }
    const_value_expr = (char ) sql__setrang(ptoc_Var358->d_const_value_expr , 0 , 1);
    ptoc_Var358->d_const_value_expr = 1;
    st_begin = (short ) sql__setrang((*(tgg00_QualBuf *) sql__nil(ptoc_Var356->a_mblock.integer.C_4.mb_qual , "vak641.p" , 21373)).integer.C_2.mfirst_free , -32768 , 32767);
    st_index = (short ) sql__setrang(ptoc_Var358->d_param_st_index , -32768 , 32767);
    switch ((int ) ptoc_Var357->n_symb)
        {
        case 182 :
        case 183 :
        case 123 :
        case 71 :
        case 72 :
        case 164 :
        case 20 :
        case 165 :
            break;
        case 19 :
            if ((int ) ptoc_Var356->a_sqlmode == 2)
                {
                ptoc_Var358->d_colptr = (tak00_columninfo *) (int *) 0;
                }
            break;
        default :
            ptoc_Var358->d_colptr = (tak00_columninfo *) (int *) 0;
            break;
        }
    rem_change_date_time = (char ) sql__setrang(ptoc_Var358->d_change_date_time , 0 , 1);
    switch ((int ) ptoc_Var357->n_symb)
        {
        case 97 :
        case 98 :
        case 106 :
        case 103 :
        case 104 :
        case 99 :
        case 105 :
        case 114 :
        case 108 :
        case 100 :
        case 102 :
        case 101 :
        case 219 :
        case 220 :
        case 230 :
        case 135 :
        case 132 :
        case 125 :
        case 129 :
        case 131 :
        case 133 :
        case 130 :
        case 126 :
        case 191 :
        case 192 :
        case 193 :
        case 128 :
        case 63 :
        case 109 :
        case 69 :
        case 70 :
        case 75 :
        case 161 :
        case 232 :
        case 203 :
        case 123 :
        case 184 :
        case 182 :
        case 183 :
            ptoc_Var358->d_change_date_time = 0;
            break;
        case 146 :
        case 147 :
        case 148 :
        case 124 :
        case 205 :
        case 197 :
        case 200 :
        case 235 :
            ptoc_Var358->d_change_date_time = 0;
            break;
        default :
            ptoc_Var358->d_change_date_time = 1;
            break;
        }
    t01int4(5 , "d_change_dat" , sql__ord((int ) (char ) sql__setrang(ptoc_Var358->d_change_date_time , 0 , 1)));
    switch ((int ) ptoc_Var357->n_symb)
        {
        case 124 :
            ak641char(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
            break;
        case 76 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 18)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641chr(&(*acv) , &(*dmli) , act_node , &(*colin) , &ptoc_Var359->fr_error);
                }
            break;
        case 189 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 18)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641chr_ora(&(*acv) , &(*dmli) , &(*colin) , act_node);
                }
            break;
        case 66 :
            if ((int ) ptoc_Var358->d_datatype == 15)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641concat(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                }
            break;
        case 99 :
            if ((int ) ptoc_Var358->d_datatype != 15 && (int ) ptoc_Var358->d_datatype != 14)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641datediff(&(*acv) , &(*dmli) , &(*colin) , act_node);
                }
            break;
        case 126 :
            if ((int ) ptoc_Var358->d_datatype != 15 && (int ) ptoc_Var358->d_datatype != 14)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641days(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                }
            break;
        case 127 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 4 || (int ) ptoc_Var358->d_datatype == 18)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641digits(&(*acv) , &(*dmli) , &(*colin) , act_node);
                }
            break;
        case 116 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 18)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641expand(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                }
            break;
        case 218 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 18)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641space(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                }
            break;
        case 33 :
            if ((int ) ptoc_Var358->d_datatype != 15 && (int ) ptoc_Var358->d_datatype != 14)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641fixed_float(&(*acv) , &(*dmli) , &(*colin) , (unsigned char ) sql__setrang(ptoc_Var357->n_symb , 0 , 240) , act_node);
                }
            break;
        case 34 :
            if ((int ) ptoc_Var358->d_datatype != 15 && (int ) ptoc_Var358->d_datatype != 14)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                if ((int ) ptoc_Var356->a_sqlmode == 3)
                    {
                    ak641fixed_float(&(*acv) , &(*dmli) , &(*colin) , (unsigned char ) sql__setrang(ptoc_Var357->n_symb , 0 , 240) , act_node);
                    }
                else
                    {
                    ak641float(&(*acv) , &(*dmli) , &(*colin) , act_node);
                    }
                }
            break;
        case 146 :
        case 147 :
        case 148 :
            if ((ptoc_Var360 = (int ) ptoc_Var358->d_datatype) == 14 || ptoc_Var360 == 16 || ptoc_Var360 == 2 || ptoc_Var360 == 3 || (int ) ptoc_Var357->n_symb == 146 && (int ) ptoc_Var358->d_datatype == 10 || (int ) ptoc_Var357->n_symb == 147 && (int ) ptoc_Var358->d_datatype == 11 || (int ) ptoc_Var357->n_symb == 148 && (int ) ptoc_Var358->d_datatype == 13)
                {
                if ((ptoc_Var360 = (int ) ptoc_Var357->n_symb) == 146 || ptoc_Var360 == 147)
                    {
                    ak641timedate(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                    }
                else
                    {
                    ak641timestamp(&(*acv) , &(*dmli) , &(*frec) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                    }
                if (((ptoc_Var360 = (int ) ptoc_Var358->d_datatype) == 10 || ptoc_Var360 == 11 || ptoc_Var360 == 13) && (char ) sql__setrang(ptoc_Var358->d_change_date_time , 0 , 1))
                    {
                    startstack = (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var356->a_mblock.integer.C_4.mb_qual , "vak641.p" , 21528)).integer.C_2.mfirst_free;
                    t01int4(5 , "sci.len colf" , (int ) (*colin).sci_len);
                    a65ch_format(&(*acv) , &(*dmli) , &startstack , &(*colin) , (unsigned char ) sql__setrang(ptoc_Var358->d_datatype , 0 , 39) , (unsigned char ) sql__setrang(ptoc_Var358->d_datatype , 0 , 39));
                    t01int4(5 , "sci.len   2 " , (int ) (*colin).sci_len);
                    (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len + 1) , -32768 , 32767);
                    }
                }
            else
                {
                ptoc_Var359->fr_error = 1;
                }
            break;
        case 128 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 4 || (int ) ptoc_Var358->d_datatype == 18)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641hex(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                }
            break;
        case 190 :
            if ((int ) ptoc_Var358->d_datatype != 2 && (int ) ptoc_Var358->d_datatype != 3 && (int ) ptoc_Var358->d_datatype != 14)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641_hextoraw(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                }
            break;
        case 88 :
            if ((int ) ptoc_Var358->d_datatype != 15 && (int ) ptoc_Var358->d_datatype != 14)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641index(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                }
            break;
        case 69 :
        case 70 :
            if ((int ) ptoc_Var358->d_datatype != 15 && (int ) ptoc_Var358->d_datatype != 14)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641length_vsize(&(*acv) , &(*dmli) , &(*colin) , act_node , (unsigned char ) sql__setrang(ptoc_Var357->n_symb , 0 , 240) , &ptoc_Var359->fr_error);
                }
            break;
        case 114 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 11 || (int ) ptoc_Var358->d_datatype == 4 || (int ) ptoc_Var358->d_datatype == 18)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641makedate(&(*acv) , &(*dmli) , &(*colin) , act_node);
                }
            break;
        case 108 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 10 || (int ) ptoc_Var358->d_datatype == 4 || (int ) ptoc_Var358->d_datatype == 18)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641maketime(&(*acv) , &(*dmli) , &(*colin) , act_node);
                }
            break;
        case 110 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 18)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641mapchar(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                }
            break;
        case 232 :
            if ((int ) ptoc_Var358->d_datatype != 4 && (int ) ptoc_Var358->d_datatype != 14)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641mbcs(&(*acv) , &(*dmli) , &(*colin) , act_node);
                }
            break;
        case 156 :
            if ((int ) ptoc_Var358->d_datatype != 15 && (int ) ptoc_Var358->d_datatype != 14)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641noround(&(*acv) , &(*dmli) , &(*colin) , act_node);
                }
            break;
        case 75 :
        case 161 :
            if ((int ) ptoc_Var358->d_datatype != 15 && (int ) ptoc_Var358->d_datatype != 14)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641num(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                }
            break;
        case 139 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 18 || (int ) ptoc_Var358->d_datatype == 4)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641soundex(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                }
            break;
        case 67 :
            if ((int ) ptoc_Var358->d_datatype == 15)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641substr(&(*acv) , &(*dmli) , &(*colin) , act_node);
                }
            break;
        case 216 :
        case 217 :
            if ((int ) ptoc_Var358->d_datatype == 15)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641leftright(&(*acv) , &(*dmli) , &(*colin) , act_node , (unsigned char ) sql__setrang(ptoc_Var357->n_symb , 0 , 240));
                }
            break;
        case 105 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 10 || (int ) ptoc_Var358->d_datatype == 4 || (int ) ptoc_Var358->d_datatype == 18)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641timediff(&(*acv) , &(*dmli) , &(*colin) , act_node);
                }
            break;
        case 182 :
        case 183 :
        case 123 :
            ak641special_func(&(*acv) , &(*dmli) , &(*colin) , act_node);
            break;
        case 184 :
            ak641decode_func(&(*acv) , &(*dmli) , &(*colin) , act_node);
            break;
        case 97 :
        case 98 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 11 || (int ) ptoc_Var358->d_datatype == 4 || (int ) ptoc_Var358->d_datatype == 18)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641addsubdate(&(*acv) , &(*dmli) , &(*colin) , act_node);
                }
            break;
        case 103 :
        case 104 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 10 || (int ) ptoc_Var358->d_datatype == 4 || (int ) ptoc_Var358->d_datatype == 18)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641addsubtime(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                }
            break;
        case 19 :
            if ((int ) ptoc_Var356->a_sqlmode == 3)
                {
                if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 18 || (int ) ptoc_Var358->d_datatype == 4)
                    {
                    ptoc_Var359->fr_error = 1;
                    }
                else
                    {
                    ak641conversion(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                    }
                }
            else
                {
                ak641ascii(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                }
            break;
        case 20 :
        case 71 :
        case 72 :
        case 164 :
        case 231 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 18 || (int ) ptoc_Var358->d_datatype == 4)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641conversion(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                }
            break;
        case 125 :
        case 129 :
        case 131 :
        case 130 :
        case 132 :
        case 133 :
        case 135 :
            ak641datetime(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
            break;
        case 73 :
        case 74 :
        case 90 :
        case 89 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 18)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641lrpad(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                }
            break;
        case 204 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 18)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641to_date(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                }
            break;
        case 205 :
        case 197 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 18 || (int ) ptoc_Var358->d_datatype == 13)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641to_char(&(*acv) , &(*dmli) , &(*colin) , act_node);
                }
            break;
        case 68 :
        case 162 :
        case 163 :
            if ((int ) ptoc_Var358->d_datatype == 18)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641trim(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                }
            break;
        case 166 :
        case 165 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 18)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641replace_translate(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                }
            break;
        case 100 :
        case 102 :
        case 101 :
        case 219 :
            if ((int ) ptoc_Var358->d_datatype != 15 && (int ) ptoc_Var358->d_datatype != 14)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641of(&(*acv) , &(*dmli) , &(*colin) , act_node);
                }
            break;
        case 220 :
        case 230 :
            if ((int ) ptoc_Var358->d_datatype != 16 && (int ) ptoc_Var358->d_datatype != 14 && (int ) ptoc_Var358->d_datatype != 2 && (int ) ptoc_Var358->d_datatype != 3 && (int ) ptoc_Var358->d_datatype != 24)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641name(&(*acv) , &(*dmli) , &(*colin) , act_node);
                }
            break;
        case 39 :
        case 62 :
        case 167 :
        case 168 :
        case 169 :
        case 171 :
        case 173 :
        case 174 :
        case 176 :
        case 179 :
        case 177 :
        case 180 :
        case 178 :
        case 181 :
        case 211 :
        case 207 :
        case 208 :
        case 209 :
        case 212 :
        case 215 :
        case 214 :
            if ((int ) ptoc_Var358->d_datatype != 15 && (int ) ptoc_Var358->d_datatype != 14)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641integer(&(*acv) , &(*dmli) , &(*colin) , act_node);
                }
            break;
        case 170 :
        case 172 :
        case 175 :
        case 210 :
            if ((int ) ptoc_Var358->d_datatype != 15 && (int ) ptoc_Var358->d_datatype != 14)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641mod_power(&(*acv) , &(*dmli) , &(*colin) , act_node);
                }
            break;
        case 63 :
        case 109 :
            if ((int ) ptoc_Var358->d_datatype != 15 && (int ) ptoc_Var358->d_datatype != 14 && ((int ) ptoc_Var358->d_datatype != 13 && (int ) ptoc_Var358->d_datatype != 16 || (int ) ptoc_Var356->a_sqlmode != 2))
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641truncround(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                }
            break;
        case 191 :
        case 106 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 10 || (int ) ptoc_Var358->d_datatype == 11 || (int ) ptoc_Var358->d_datatype == 4 || (int ) ptoc_Var358->d_datatype == 18)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641_add_months(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                }
            break;
        case 203 :
            if ((int ) ptoc_Var358->d_datatype != 13 && (int ) ptoc_Var358->d_datatype != 16 && (int ) ptoc_Var358->d_datatype != 14)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641new_time(&(*acv) , &(*dmli) , &(*colin) , act_node , &ptoc_Var359->fr_error);
                }
            break;
        case 192 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 10 || (int ) ptoc_Var358->d_datatype == 11 || (int ) ptoc_Var358->d_datatype == 4 || (int ) ptoc_Var358->d_datatype == 18)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641last_day(&(*acv) , &(*dmli) , &(*colin) , act_node);
                }
            break;
        case 193 :
            if ((int ) ptoc_Var358->d_datatype != 15 && (int ) ptoc_Var358->d_datatype != 14)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641months_between(&(*acv) , &(*dmli) , &(*colin) , act_node);
                }
            break;
        case 224 :
            if ((int ) ptoc_Var358->d_datatype == 15 || (int ) ptoc_Var358->d_datatype == 10 || (int ) ptoc_Var358->d_datatype == 11 || (int ) ptoc_Var358->d_datatype == 13 || (int ) ptoc_Var358->d_datatype == 4 || (int ) ptoc_Var358->d_datatype == 18)
                {
                ptoc_Var359->fr_error = 1;
                }
            else
                {
                ak641toidentifier(&(*acv) , &(*dmli) , &(*colin) , act_node);
                }
            break;
        case 200 :
        case 235 :
            ak641user_defined_func(&(*acv) , &(*dmli) , &(*colin) , act_node);
            break;
        case 239 :
        case 240 :
            ak641get_name(&(*acv) , &(*dmli) , &(*colin) , act_node);
            break;
        default :
            t01int4(5 , "act_node    " , act_node);
            t01int4(5 , "n_proc      " , sql__ord((int ) ptoc_Var357->n_proc));
            t01int4(5 , "n_symb      " , sql__ord((int ) ptoc_Var357->n_symb));
            t01int4(5 , "n_length    " , (int ) ptoc_Var357->n_length);
            break;
        }
    switch ((int ) ptoc_Var357->n_symb)
        {
        case 110 :
        case 124 :
        case 66 :
        case 116 :
        case 218 :
        case 69 :
        case 67 :
        case 216 :
        case 217 :
        case 19 :
        case 20 :
        case 71 :
        case 72 :
        case 164 :
        case 68 :
        case 163 :
        case 166 :
        case 197 :
        case 70 :
        case 162 :
        case 165 :
        case 73 :
        case 74 :
        case 90 :
        case 89 :
            ptoc_Var358->d_ch_datatype = 14;
            break;
        default :
            break;
        }
    t01int4(5 , "node       C" , act_node);
    t01int4(5 , "colin.typ  C" , sql__ord((int ) (*colin).sci_typ));
    t01int4(5 , "change_d   C" , sql__ord((int ) (char ) sql__setrang(ptoc_Var358->d_change_date_time , 0 , 1)));
    ptoc_Var358->d_change_date_time = (char ) sql__setrang(rem_change_date_time , 0 , 1);
    t01int4(5 , "dm_datatype " , sql__ord((int ) ptoc_Var358->d_datatype));
    if ((char ) sql__setrang(ptoc_Var359->fr_error , 0 , 1))
        {
        a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var356->a_ap_tree , "vak641.p" , 21876))[sql__setrang((int ) ptoc_Var357->n_lo_level , 0 , 32767)].n_pos);
        }
    else
        {
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var356->a_return_segm , "vak641.p" , 21878)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var361 = &ptoc_Var356->a_mblock;
            ptoc_Var362 = (tgg00_QualBuf *) sql__nil(ptoc_Var361->integer.C_4.mb_qual , "vak641.p" , 21880);
            if ((char ) sql__setrang(ptoc_Var358->d_const_value_expr , 0 , 1))
                {
                if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var361->integer.C_5.mb_st , "vak641.p" , 21884))[sql__setrang((int ) ptoc_Var362->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.eop != 0 || (int ) ptoc_Var357->n_symb == 226)
                    {
                    if ((int ) ptoc_Var358->d_param_st_index > (int ) st_index)
                        {
                        expr_st_cnt = (short ) sql__setrang((short ) ((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var356->a_mblock.integer.C_4.mb_qual , "vak641.p" , 21890)).integer.C_2.mfirst_free - (int ) st_begin) , -32768 , 32767);
                        if ((int ) expr_st_cnt <= 255 && ptoc_Var361->integer.C_2.mb_data_len - (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var361->integer.C_5.mb_st , "vak641.p" , 21894))[sql__setrang((int ) st_begin , 1 , 32767) + -1].integer.C_1.epos + 1 >= (int ) (*colin).sci_iolen)
                            {
                            a651code_for_const_param_expr(&(*acv) , &(*dmli) , (short ) sql__setrang(st_begin , -32768 , 32767) , (short ) sql__setrang(expr_st_cnt , -32768 , 32767));
                            }
                        }
                    else
                        {
                        st_end = (short ) sql__setrang((short ) ((int ) ptoc_Var362->integer.C_2.mfirst_free + -1) , -32768 , 32767);
                        a651value_calculate(&(*acv) , (int ) st_begin , (int ) st_end , (char ) sql__setrang((int ) (*colin).sci_typ == 4 , 0 , 1) , (char ) sql__setrang(ptoc_Var358->d_like , 0 , 1) , (short ) sql__setrang((*colin).sci_iolen , -32768 , 32767) , (*(tak_ap_max_tree *) sql__nil(ptoc_Var356->a_ap_tree , "vak641.p" , 21906))[sql__setrang(act_node , 0 , 32767)].n_pos);
                        }
                    }
                ptoc_Var358->d_const_value_expr = (char ) sql__setrang(const_value_expr , 0 , 1);
                }
            }
        }
    cccprint("<a641col_functio");
    }



int a641stack_for_op_b_chr(acv, dmli, colin, first_int, keep_datatype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
tsp_int2 first_int;
tsp_data_type keep_datatype;
    {
    int oldlen;
    int newlen;
    tsp_int2 res_len;
    char dummy;
    tak_all_command_glob *ptoc_Var363;
    tak_dml_info *ptoc_Var364;
    tak00_scolinf *ptoc_Var365;
    int ptoc_Var366;
    cccprint("AK641 a641stack_");
    ptoc_Var363 = &*acv;
    ptoc_Var364 = &*dmli;
    ptoc_Var365 = &*colin;
    t01int4(5 , "keep_datatyp" , sql__ord((int ) keep_datatype));
    oldlen = (int ) ptoc_Var365->sci_len;
    if ((int ) first_int == 0)
        {
        switch ((int ) ptoc_Var365->sci_typ)
            {
            case 0 :
            case 1 :
            case 12 :
                a641_get_length(&(*colin) , &res_len , &dummy);
                ptoc_Var365->sci_len = (short ) sql__setrang(res_len , -32768 , 32767);
                break;
            default :
                break;
            }
        }
    else
        {
        ptoc_Var365->sci_len = (short ) sql__setrang(first_int , -32768 , 32767);
        }
    newlen = (int ) ptoc_Var365->sci_len;
    ptoc_Var365->sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var365->sci_len , -32768 , 32767 , "vak641.p" , 21957) , -32768 , 32767);
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak641.p" , 21958)).integer.C_4.sp1r_returncode == 0 && ((ptoc_Var366 = (int ) (*colin).sci_typ) == 0 || ptoc_Var366 == 1 || ptoc_Var366 == 12 || ptoc_Var366 == 23))
        {
        if ((int ) (*colin).sci_typ == 23)
            {
            if ((int ) ptoc_Var365->sci_len > 255)
                {
                newlen = 1;
                }
            a641string_set_operator(&(*acv) , 3 , (int ) (*colin).sci_frac , oldlen , (unsigned char)'\003' , (unsigned char ) sql__setrang((unsigned char ) newlen , 0 , 255));
            }
        else
            {
            if ((int ) ptoc_Var365->sci_len > 255)
                {
                newlen = oldlen + 6;
                }
            a641string_set_operator(&(*acv) , 3 , (int ) (*colin).sci_frac , oldlen , (unsigned char)'\002' , (unsigned char ) sql__setrang((unsigned char ) newlen , 0 , 255));
            }
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak641.p" , 21982)).integer.C_4.sp1r_returncode == 0 && (int ) ptoc_Var365->sci_len != newlen)
            {
            a641string_set_operator(&(*acv) , 8 , (int ) first_int + 1 , newlen , (unsigned char)'\0' , (unsigned char)'\0');
            }
        }
    if ((int ) keep_datatype == 2)
        {
        if ((int ) g01code.ctype != 0 || (int ) (*colin).sci_typ == 3)
            {
            a65_set_operator(&(*acv) , 2);
            }
        keep_datatype = 2;
        (*colin).sci_typ = 2;
        }
    else
        {
        if ((int ) keep_datatype == 3)
            {
            if ((int ) g01code.ctype != 1 || (int ) (*colin).sci_typ == 2)
                {
                a65_set_operator(&(*acv) , 7);
                }
            (*colin).sci_typ = 3;
            }
        else
            {
            if ((int ) keep_datatype == 24)
                {
                a641string_set_operator(&(*acv) , 66 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\024');
                (*colin).sci_typ = 24;
                ptoc_Var365->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var365->sci_len * 2 + 1) , -32768 , 32767);
                }
            else
                {
                if ((ptoc_Var366 = (int ) keep_datatype) == 14 || ptoc_Var366 == 16)
                    {
                    if ((int ) (*colin).sci_typ == 4)
                        {
                        keep_datatype = 4;
                        }
                    else
                        {
                        if ((int ) g01code.ctype == 0)
                            {
                            if ((int ) (*colin).sci_typ == 3)
                                {
                                a65_set_operator(&(*acv) , 2);
                                }
                            (*colin).sci_typ = 2;
                            keep_datatype = 2;
                            }
                        else
                            {
                            if ((int ) (*colin).sci_typ != 3)
                                {
                                a65_set_operator(&(*acv) , 7);
                                }
                            (*colin).sci_typ = 3;
                            keep_datatype = 3;
                            }
                        }
                    }
                }
            }
        }
    ptoc_Var364->d_datatype = (unsigned char ) sql__setrang(keep_datatype , 0 , 39);
    ptoc_Var365->sci_frac = 0;
    if ((ptoc_Var366 = (int ) (*colin).sci_typ) == 0 || ptoc_Var366 == 1 || ptoc_Var366 == 12)
        {
        if ((int ) keep_datatype == 2)
            {
            ptoc_Var365->sci_typ = 2;
            }
        else
            {
            ptoc_Var365->sci_typ = 3;
            }
        }
    else
        {
        if ((int ) first_int != 0)
            {
            a641string_set_operator(&(*acv) , 2 , (int ) (short ) sql__pred((int ) ptoc_Var365->sci_iolen , -32768 , 32767 , "vak641.p" , 22067) , 0 , (unsigned char)'\0' , (unsigned char)'\0');
            }
        }
    cccprint("<a641stack_for_o");
    }



int a641string_set_operator(acv, operator, destlength, sourcelength, tab1, tab2)
tak_all_command_glob *acv;
tgg_stack_op_build_in operator;
int destlength;
int sourcelength;
unsigned char tab1;
unsigned char tab2;
    {
    struct tgg00_MessBlock *ptoc_Var367;
    tgg00_QualBuf *ptoc_Var368;
    tgg00_StackEntry *ptoc_Var369;
    cccprint("AK641 a641string");
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak641.p" , 22084)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var367 = &(*acv).a_mblock;
        ptoc_Var368 = (tgg00_QualBuf *) sql__nil(ptoc_Var367->integer.C_4.mb_qual , "vak641.p" , 22086);
        if ((int ) ptoc_Var368->integer.C_2.mfirst_free > (int ) ptoc_Var367->integer.C_2.mb_st_max)
            {
            a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var367->integer.C_2.mb_st_max);
            }
        else
            {
            ptoc_Var368->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var368->integer.C_2.mqual_cnt , -32768 , 32767 , "vak641.p" , 22092) , -32768 , 32767);
            ptoc_Var369 = &(*(tgg00_StackList *) sql__nil(ptoc_Var367->integer.C_5.mb_st , "vak641.p" , 22093))[sql__setrang((int ) ptoc_Var368->integer.C_2.mfirst_free , 1 , 32767) + -1];
            ptoc_Var369->integer.C_1.etype = 55;
            ptoc_Var369->integer.C_4.eop_build_in = (unsigned char ) sql__setrang(operator , 0 , 67);
            ptoc_Var369->integer.C_1.epos = (short ) sql__setrang((short ) sourcelength , -32768 , 32767);
            ptoc_Var369->integer.C_1.elen_var = (short ) sql__setrang((short ) destlength , -32768 , 32767);
            ptoc_Var369->integer.C_1.ecol_tab[0] = (unsigned char ) sql__setrang(tab1 , 0 , 255);
            ptoc_Var369->integer.C_1.ecol_tab[1] = (unsigned char ) sql__setrang(tab2 , 0 , 255);
            ptoc_Var368->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var368->integer.C_2.mfirst_free , -32768 , 32767 , "vak641.p" , 22103) , -32768 , 32767);
            }
        }
    cccprint("<a641string_set_");
    }



int a641dbyte_string_set_operator(acv, operator, destlength, sourcelength, ptoc_ptr_c2)
tak_all_command_glob *acv;
tgg_stack_op_build_in operator;
int destlength;
int sourcelength;
tsp_c2 ptoc_ptr_c2;
    {
    struct tgg00_MessBlock *ptoc_Var370;
    tgg00_QualBuf *ptoc_Var371;
    tgg00_StackEntry *ptoc_Var372;
    tsp_c2 c2;
    _ptoc_MOVE(char, sizeof(tsp_c2 ), c2, ptoc_ptr_c2);
    cccprint("AK641 a641dbyte_");
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak641.p" , 22120)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var370 = &(*acv).a_mblock;
        ptoc_Var371 = (tgg00_QualBuf *) sql__nil(ptoc_Var370->integer.C_4.mb_qual , "vak641.p" , 22122);
        if ((int ) ptoc_Var371->integer.C_2.mfirst_free > (int ) ptoc_Var370->integer.C_2.mb_st_max)
            {
            a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var370->integer.C_2.mb_st_max);
            }
        else
            {
            ptoc_Var371->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var371->integer.C_2.mqual_cnt , -32768 , 32767 , "vak641.p" , 22128) , -32768 , 32767);
            ptoc_Var372 = &(*(tgg00_StackList *) sql__nil(ptoc_Var370->integer.C_5.mb_st , "vak641.p" , 22129))[sql__setrang((int ) ptoc_Var371->integer.C_2.mfirst_free , 1 , 32767) + -1];
            ptoc_Var372->integer.C_1.etype = 55;
            ptoc_Var372->integer.C_4.eop_build_in = (unsigned char ) sql__setrang(operator , 0 , 67);
            ptoc_Var372->integer.C_1.epos = (short ) sql__setrang((short ) sourcelength , -32768 , 32767);
            ptoc_Var372->integer.C_1.elen_var = (short ) sql__setrang((short ) destlength , -32768 , 32767);
            _ptoc_ASSIGN(unsigned char , 2, ptoc_Var372->integer.C_1.ecol_tab, c2)
            ptoc_Var371->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var371->integer.C_2.mfirst_free , -32768 , 32767 , "vak641.p" , 22138) , -32768 , 32767);
            }
        }
    cccprint("<a641dbyte_strin");
    }



int ak641user_defined_func(acv, dmli, colin, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int act_node;
    {
    char is_method_call;
    int is_in_out;
    int curr_n;
    int proc_no;
    int value_cnt;
    int init_qual_cnt;
    int init_first_free;
    int init_part2_len;
    int first_free;
    tsp_int2 dummy_iolen;
    tsp_int4 curr_data_pos;
    tsp_int4 param_cnt;
    tak_sysbufferaddress pEvalDataLength;
    tak00_scolinf proc_colin;
    tak00_scolinf param_colin;
    tak00_columninfo param_colinfo;
    tsp_data_type keep_datatype;
    tak_convert_type convert_t;
    tsp_knl_identifier owner;
    tsp_knl_identifier func_name;
    tak_sysbufferaddress proc_ptr;
    tak_sysbufferaddress proc_buf;
    tgg_dbfunc_info func_info;
    tak00_scolinf param_info[16];
    struct ptoc_Type180
      {
      union  
        {
        struct  
          {
          tsp_c2 type_flag;
          tsp_c4 gap;
          tsp_c2 id;
          } C_true;
        struct  
          {
          tgg_surrogate surrogate;
          } C_false;
        } boolean;
      } type_id;
    tak_all_command_glob *ptoc_Var373;
    tak_dml_info *ptoc_Var374;
    struct ptoc_Type59 *ptoc_Var375;
    struct tgg00_MessBlock *ptoc_Var376;
    tgg00_QualBuf *ptoc_Var377;
    int ptoc_Var378;
    struct tgg00_MessBlock *ptoc_Var379;
    tgg00_QualBuf *ptoc_Var380;
    tgg00_StackEntry *ptoc_Var381;
    tgg00_StackEntry *ptoc_Var382;
    tgg00_DataPart *ptoc_Var383;
    struct tgg00_MessBlock *ptoc_Var384;
    tgg00_QualBuf *ptoc_Var385;
    tgg00_StackEntry *ptoc_Var386;
    tgg00_StackEntry *ptoc_Var387;
    tgg00_DataPart *ptoc_Var388;
    cccprint("AK641 ak641user_");
    ptoc_Var373 = &*acv;
    ptoc_Var374 = &*dmli;
    a05identifier_get(&(*acv) , act_node , sizeof(tsp00_C64 ) , func_name);
    proc_no = 1;
    proc_buf = (struct tak_systembuffer *) (int *) 0;
    is_in_out = 0;
    is_method_call = (char ) sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var373->a_ap_tree , "vak641.p" , 22199))[sql__setrang(act_node , 0 , 32767)].n_symb == 235 , 0 , 1);
    ptoc_Var374->d_in_method_param = 1;
    if ((char ) sql__setrang(is_method_call , 0 , 1))
        {
        curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var373->a_ap_tree , "vak641.p" , 22204))[sql__setrang(act_node , 0 , 32767)].n_lo_level;
        a06get_username(&(*acv) , &curr_n , ptoc_Var374->d_user);
        ptoc_Var375 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var373->a_ap_tree , "vak641.p" , 22206))[sql__setrang(curr_n , 0 , 32767)];
        if ((int ) ptoc_Var375->n_symb == 44)
            {
            a05identifier_get(&(*acv) , curr_n , sizeof(tsp00_C64 ) , ptoc_Var374->d_table);
            curr_n = (int ) ptoc_Var375->n_sa_level;
            }
        a05identifier_get(&(*acv) , curr_n , sizeof(tsp00_C64 ) , ptoc_Var374->d_column);
        if ((char ) sql__setrang(ptoc_Var374->d_pseudo_ins_select , 0 , 1))
            {
            ptoc_Var374->d_tabarr[2] = ptoc_Var374->d_tabarr[0];
            ptoc_Var374->d_tabarr[0] = ptoc_Var374->d_tabarr[1];
            ptoc_Var374->d_acttabindex = 0;
            a61_rel_old_table(&(*acv) , &(*dmli) , 1);
            }
        a61_search_table(&(*acv) , &(*dmli));
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var373->a_return_segm , "vak641.p" , 22228)).integer.C_4.sp1r_returncode == 0)
            {
            _ptoc_ASSIGN(unsigned char , 8, type_id.boolean.C_false.surrogate, "\0\0\0\0\0\0\0\0")
            _ptoc_ASSIGN(unsigned char , 2, type_id.boolean.C_true.type_flag, "\377\371")
            _ptoc_ASSIGN(unsigned char , 2, type_id.boolean.C_true.id, (*(tak00_columninfo *) sql__nil((*dmli).d_colbuf , "vak641.p" , 22233)).integer.C_1.cudtdatatype)
            if (a12method_exist(&(*acv) , type_id.boolean.C_false.surrogate , func_name , 1 , &proc_buf))
                {
                }
            }
        }
    else
        {
        if (!a12dbfunc_exist(&(*acv) , func_name , &proc_buf))
            {
            if ((char ) sql__setrang(ptoc_Var373->a_insert_select , 0 , 1) && a12exist_type(&(*acv) , &act_node , owner , func_name , &proc_buf))
                {
                if (a12method_exist(&(*acv) , (*(struct tak_systembuffer *) sql__nil(proc_buf , "vak641.p" , 22247)).integer.C_66.smethod.me_surrogate , func_name , 1 , &proc_buf))
                    {
                    }
                }
            }
        }
    if (proc_buf == (struct tak_systembuffer *) (int *) 0)
        {
        a07_b_put_error(&(*acv) , 6499 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var373->a_ap_tree , "vak641.p" , 22259))[sql__setrang(act_node , 0 , 32767)].n_pos);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var373->a_return_segm , "vak641.p" , 22261)).integer.C_4.sp1r_returncode == 0)
        {
        param_cnt = (int ) (*(struct tak_systembuffer *) sql__nil(proc_buf , "vak641.p" , 22264)).integer.C_66.smethod.me_param_cnt;
        keep_datatype = (unsigned char ) sql__setrang(ptoc_Var374->d_datatype , 0 , 39);
        ptoc_Var376 = &ptoc_Var373->a_mblock;
        ptoc_Var377 = (tgg00_QualBuf *) sql__nil(ptoc_Var376->integer.C_4.mb_qual , "vak641.p" , 22266);
        init_qual_cnt = (int ) ptoc_Var377->integer.C_2.mqual_cnt;
        init_first_free = (int ) ptoc_Var377->integer.C_2.mfirst_free;
        init_part2_len = ptoc_Var376->integer.C_2.mb_data_len;
        if (param_cnt > 1 || (char ) sql__setrang(is_method_call , 0 , 1))
            {
            value_cnt = 0;
            curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var373->a_ap_tree , "vak641.p" , 22277))[sql__setrang(act_node , 0 , 32767)].n_lo_level;
            ak641describe_output_param(&(*acv) , &(*dmli) , proc_buf , &(*colin) , &is_in_out , &param_colinfo);
            if ((int ) (*colin).sci_len < 0)
                {
                a12InitParamInfoRec(&(*acv) , (*(struct tak_systembuffer *) sql__nil(proc_buf , "vak641.p" , 22281)).integer.C_66.smethod.me_surrogate , param_cnt , &pEvalDataLength);
                }
            else
                {
                pEvalDataLength = (struct tak_systembuffer *) (int *) 0;
                }
            if (is_in_out > 0)
                {
                param_cnt = param_cnt + 1;
                }
            dummy_iolen = 0;
            if (a65_datatypes_ok(&(*acv) , &(*dmli) , &keep_datatype , &dummy_iolen , (unsigned char ) sql__setrang((*colin).sci_typ , 0 , 39) , 0 , 1 , 1 , 0 , &convert_t))
                {
                while (curr_n != 0 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var373->a_return_segm , "vak641.p" , 22298)).integer.C_4.sp1r_returncode == 0)
                    {
                    t01int4(5 , "curr_n      " , curr_n);
                    value_cnt = value_cnt + 1;
                    ak641describe_param(&(*acv) , &(*dmli) , proc_buf , value_cnt , &proc_colin , &param_colinfo);
                    if ((char ) sql__setrang(ptoc_Var374->d_pseudo_ins_select , 0 , 1) && value_cnt == 2)
                        {
                        ptoc_Var374->d_tabarr[0] = ptoc_Var374->d_tabarr[2];
                        ptoc_Var374->d_acttabindex = 0;
                        a61_rel_old_table(&(*acv) , &(*dmli) , 1);
                        }
                    first_free = (int ) (*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak641.p" , 22312)).integer.C_2.mfirst_free;
                    param_colin.sci_len = 0;
                    a640not_first_factor(&(*acv) , &(*dmli) , &param_colin , &curr_n);
                    param_colin.sci_com_type = (short ) sql__setrang(proc_colin.sci_com_type , -32768 , 32767);
                    param_info[sql__setrang(value_cnt , 1 , 16) + -1] = param_colin;
                    if ((int ) param_colin.sci_typ != (int ) proc_colin.sci_typ && ((ptoc_Var378 = (int ) proc_colin.sci_typ) == 10 || ptoc_Var378 == 11 || ptoc_Var378 == 13))
                        {
                        if ((ptoc_Var378 = (int ) param_colin.sci_typ) == 2 || ptoc_Var378 == 3)
                            {
                            a641check_datetime(&(*acv) , &(*dmli) , (unsigned char ) sql__setrang(proc_colin.sci_typ , 0 , 39));
                            }
                        else
                            {
                            a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var373->a_ap_tree , "vak641.p" , 22326))[sql__setrang(curr_n , 0 , 32767)].n_pos);
                            }
                        }
                    t01int4(5 , "func dtype  " , sql__ord((int ) proc_colin.sci_typ));
                    t01int4(5 , "found dtype " , sql__ord((int ) param_colin.sci_typ));
                    t01int4(5 , "func len    " , sql__ord((int ) proc_colin.sci_len));
                    t01int4(5 , "found len   " , sql__ord((int ) param_colin.sci_len));
                    if (pEvalDataLength != (struct tak_systembuffer *) (int *) 0)
                        {
                        a12StoreParamInfo(&(*acv) , pEvalDataLength , &param_colin);
                        }
                    ak641emit_param(&(*acv) , &(*dmli) , proc_buf , (char ) sql__setrang(value_cnt == is_in_out , 0 , 1) , &proc_colin , first_free);
                    curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var373->a_ap_tree , "vak641.p" , 22343))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
                    }
                }
            }
        t01int4(5 , "value_cnt   " , value_cnt);
        while ((value_cnt < param_cnt + -1 || value_cnt == 0) && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var373->a_return_segm , "vak641.p" , 22354)).integer.C_4.sp1r_returncode == 0)
            {
            value_cnt = value_cnt + 1;
            if (param_cnt > 1)
                {
                ak641describe_param(&(*acv) , &(*dmli) , proc_buf , value_cnt , &proc_colin , &param_colinfo);
                }
            else
                {
                proc_colin.sci_len = 2;
                proc_colin.sci_frac = 0;
                }
            ptoc_Var379 = &(*acv).a_mblock;
            ptoc_Var380 = (tgg00_QualBuf *) sql__nil(ptoc_Var379->integer.C_4.mb_qual , "vak641.p" , 22368);
            if ((int ) ptoc_Var380->integer.C_2.mfirst_free + 1 > (int ) ptoc_Var379->integer.C_2.mb_st_max)
                {
                a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var379->integer.C_2.mb_st_max);
                }
            else
                {
                if (ptoc_Var379->integer.C_2.mb_data_len + 1 > ptoc_Var379->integer.C_2.mb_data_size)
                    {
                    a07_b_put_error(&(*acv) , 2801 , -ptoc_Var379->integer.C_2.mb_data_size);
                    }
                }
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var373->a_return_segm , "vak641.p" , 22381)).integer.C_4.sp1r_returncode == 0)
                {
                ptoc_Var381 = &(*(tgg00_StackList *) sql__nil(ptoc_Var379->integer.C_5.mb_st , "vak641.p" , 22384))[sql__setrang((int ) ptoc_Var380->integer.C_2.mfirst_free , 1 , 32767) + -1];
                ptoc_Var381->integer.C_1.etype = 22;
                ptoc_Var381->integer.C_1.eop = 0;
                ptoc_Var381->integer.C_1.epos = (short ) sql__setrang((short ) (ptoc_Var373->a_mblock.integer.C_2.mb_data_len + 1) , -32768 , 32767);
                ptoc_Var381->integer.C_1.elen_var = 1;
                ptoc_Var381->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                ptoc_Var381->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                ptoc_Var382 = &(*(tgg00_StackList *) sql__nil(ptoc_Var379->integer.C_5.mb_st , "vak641.p" , 22394))[sql__setrang((int ) ptoc_Var380->integer.C_2.mfirst_free + 1 , 1 , 32767) + -1];
                ptoc_Var382->integer.C_1.etype = 29;
                ptoc_Var382->integer.C_1.eop = 2;
                ptoc_Var382->integer.C_1.epos = (short ) sql__setrang(proc_colin.sci_len , -32768 , 32767);
                ptoc_Var382->integer.C_1.elen_var = (short ) sql__setrang(proc_colin.sci_frac , -32768 , 32767);
                ptoc_Var382->integer.C_1.ecol_tab[0] = (unsigned char ) sql__setrang((unsigned char ) sql__ord((int ) proc_colin.sci_typ) , 0 , 255);
                ptoc_Var382->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                if (value_cnt == param_cnt + -1 || param_cnt == 1)
                    {
                    ptoc_Var383 = (tgg00_DataPart *) sql__nil(ptoc_Var379->integer.C_3.mb_data , "vak641.p" , 22408);
                    curr_data_pos = ptoc_Var379->integer.C_2.mb_data_len;
                    ptoc_Var383->integer.C_1.mbp_buf[sql__setrang(curr_data_pos + 1 , 1 , 8192000) + -1] = (unsigned char)'\377';
                    ptoc_Var379->integer.C_2.mb_data_len = ptoc_Var379->integer.C_2.mb_data_len + 1;
                    }
                ptoc_Var380->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) ((int ) ptoc_Var380->integer.C_2.mqual_cnt + 2) , -32768 , 32767);
                ptoc_Var380->integer.C_2.mfirst_free = (short ) sql__setrang((short ) ((int ) ptoc_Var380->integer.C_2.mfirst_free + 2) , -32768 , 32767);
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var373->a_return_segm , "vak641.p" , 22424)).integer.C_4.sp1r_returncode == 0)
            {
            if (param_cnt == 1)
                {
                ak641describe_param(&(*acv) , &(*dmli) , proc_buf , 1 , &(*colin) , &param_colinfo);
                }
            ptoc_Var384 = &ptoc_Var373->a_mblock;
            ptoc_Var385 = (tgg00_QualBuf *) sql__nil(ptoc_Var384->integer.C_4.mb_qual , "vak641.p" , 22431);
            if ((int ) ptoc_Var385->integer.C_2.mfirst_free + 1 >= (int ) ptoc_Var384->integer.C_2.mb_st_max)
                {
                a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var384->integer.C_2.mb_st_max);
                }
            else
                {
                if (a01aligned_cmd_len(ptoc_Var384->integer.C_2.mb_data_len) + sizeof(struct ptoc_Type50 ) > ptoc_Var384->integer.C_2.mb_data_size)
                    {
                    a07_b_put_error(&(*acv) , 2801 , -ptoc_Var384->integer.C_2.mb_data_size);
                    }
                }
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var373->a_return_segm , "vak641.p" , 22445)).integer.C_4.sp1r_returncode == 0)
                {
                if ((int ) (*colin).sci_typ == 14)
                    {
                    *colin = param_info[sql__setrang((int ) (*colin).sci_len , 1 , 16) + -1];
                    }
                if ((ptoc_Var378 = (int ) (*colin).sci_typ) == 15 || ptoc_Var378 == 0 || ptoc_Var378 == 1 || ptoc_Var378 == 29 || ptoc_Var378 == 30)
                    {
                    ptoc_Var374->d_datatype = 15;
                    }
                else
                    {
                    ptoc_Var374->d_datatype = (unsigned char ) sql__setrang((*colin).sci_typ , 0 , 39);
                    }
                _ptoc_ASSIGN(unsigned char , 2, ptoc_Var374->d_udt_datatype, (*colin).sci_udt_id)
                ptoc_Var386 = &(*(tgg00_StackList *) sql__nil(ptoc_Var384->integer.C_5.mb_st , "vak641.p" , 22459))[sql__setrang((int ) ptoc_Var385->integer.C_2.mfirst_free , 1 , 32767) + -1];
                ptoc_Var386->integer.C_1.etype = 55;
                ptoc_Var386->integer.C_4.eop_build_in = 29;
                if ((char ) sql__setrang(ptoc_Var374->d_view , 0 , 1))
                    {
                    ptoc_Var386->integer.C_1.epos = (short ) sql__setrang((short ) (ptoc_Var384->integer.C_2.mb_data_len + 1) , -32768 , 32767);
                    }
                else
                    {
                    ptoc_Var386->integer.C_1.epos = (short ) sql__setrang((short ) (a01aligned_cmd_len(ptoc_Var384->integer.C_2.mb_data_len) + 1) , -32768 , 32767);
                    }
                curr_data_pos = (int ) ptoc_Var386->integer.C_1.epos;
                ptoc_Var386->integer.C_1.elen_var = (short ) sql__setrang((short ) sizeof(struct ptoc_Type50 ) , -32768 , 32767);
                ptoc_Var386->integer.C_1.ecol_tab[0] = (unsigned char ) sql__setrang((unsigned char ) value_cnt , 0 , 255);
                ptoc_Var386->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                ptoc_Var385->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) ((int ) ptoc_Var385->integer.C_2.mqual_cnt + 1) , -32768 , 32767);
                if (pEvalDataLength != (struct tak_systembuffer *) (int *) 0)
                    {
                    (*colin).sci_len = (short ) sql__setrang((short ) a12EvalDataLength(&(*acv) , pEvalDataLength) , -32768 , 32767);
                    if ((int ) (*colin).sci_len < 0)
                        {
                        a07_b_put_error(&(*acv) , 3400 , 1);
                        }
                    switch ((int ) (*colin).sci_typ)
                        {
                        case 15 :
                        case 0 :
                        case 1 :
                        case 30 :
                        case 29 :
                            (*colin).sci_iolen = (short ) sql__setrang((short ) (((int ) (*colin).sci_len + 1) / 2 + 2) , -32768 , 32767);
                            break;
                        default :
                            (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len + 1) , -32768 , 32767);
                            break;
                        }
                    }
                ptoc_Var385->integer.C_2.mfirst_free = (short ) sql__setrang((short ) ((int ) ptoc_Var385->integer.C_2.mfirst_free + 1) , -32768 , 32767);
                ptoc_Var387 = &(*(tgg00_StackList *) sql__nil(ptoc_Var384->integer.C_5.mb_st , "vak641.p" , 22495))[sql__setrang((int ) ptoc_Var385->integer.C_2.mfirst_free , 1 , 32767) + -1];
                ptoc_Var387->integer.C_1.etype = 39;
                ptoc_Var387->integer.C_1.eop = 0;
                ptoc_Var387->integer.C_1.epos = (short ) sql__setrang((*colin).sci_com_type , -32768 , 32767);
                ptoc_Var387->integer.C_1.elen_var = (short ) sql__setrang((*colin).sci_iolen , -32768 , 32767);
                ptoc_Var387->integer.C_1.ecol_tab[0] = (unsigned char ) sql__setrang((unsigned char ) sql__ord((int ) (*colin).sci_typ) , 0 , 255);
                ptoc_Var387->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                ptoc_Var385->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) ((int ) ptoc_Var385->integer.C_2.mqual_cnt + 1) , -32768 , 32767);
                ptoc_Var385->integer.C_2.mfirst_free = (short ) sql__setrang((short ) ((int ) ptoc_Var385->integer.C_2.mfirst_free + 1) , -32768 , 32767);
                ptoc_Var388 = (tgg00_DataPart *) sql__nil(ptoc_Var384->integer.C_3.mb_data , "vak641.p" , 22507);
                proc_ptr = proc_buf;
                func_info.dbf_acv_addr = (unsigned char *) &*acv;
                func_info.dbf_created = 0;
                func_info.dbf_dispid = (*(struct tak_systembuffer *) sql__nil(proc_ptr , "vak641.p" , 22513)).integer.C_66.smethod.me_dispid;
                _ptoc_ASSIGN(unsigned char , 16, func_info.dbf_com_params.coclsid, (*(struct tak_systembuffer *) sql__nil(proc_ptr , "vak641.p" , 22515)).integer.C_66.smethod.me_coclsid)
                _ptoc_ASSIGN(unsigned char , 16, func_info.dbf_com_params.iid, (*(struct tak_systembuffer *) sql__nil(proc_ptr , "vak641.p" , 22517)).integer.C_66.smethod.me_iid)
                _ptoc_ASSIGN(unsigned char , 4, func_info.dbf_com_params.session, (*acv).a_transinf.tri_trans.trSessionId_gg00.ci4_gg00)
                func_info.dbf_com_params.ikernel_sink = (unsigned char *) &(*acv).a_kernel_sink;
                g10mv6("VAK641" , 9 , sizeof(struct ptoc_Type50 ) , ptoc_Var384->integer.C_2.mb_data_size , &func_info , 1 , ptoc_Var388->integer.C_1.mbp_buf , curr_data_pos , sizeof(struct ptoc_Type50 ) , &(*(tsp1_segment *) sql__nil(ptoc_Var373->a_return_segm , "vak641.p" , 22528)).integer.C_4.sp1r_returncode);
                ptoc_Var384->integer.C_2.mb_data_len = curr_data_pos + sizeof(struct ptoc_Type50 ) + -1;
                }
            }
        }
    ptoc_Var374->d_in_method_param = 0;
    t01qual(5 , sql__nil(ptoc_Var373->a_mblock.integer.C_4.mb_qual , "vak641.p" , 22543));
    cccprint("<ak641user_defin");
    }



int ak641describe_output_param(acv, dmli, proc_buf, scolinf, is_in_out, colinfo)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_sysbufferaddress proc_buf;
tak00_scolinf *scolinf;
int *is_in_out;
tak00_columninfo *colinfo;
    {
    int ptoc_Var389;
    cccprint("AK641 ak641descr");
    a12output_parameter(&(*acv) , proc_buf , &(*is_in_out) , &(*scolinf));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak641.p" , 22559)).integer.C_4.sp1r_returncode == 0)
        {
        if ((ptoc_Var389 = (int ) (*scolinf).sci_typ) == 29 || ptoc_Var389 == 30)
            {
            (*scolinf).sci_typ = 0;
            }
        *colinfo = a642standard_colinfo;
        (*colinfo).integer.C_1.cdatatyp = (unsigned char ) sql__setrang((*scolinf).sci_typ , 0 , 39);
        (*colinfo).integer.C_1.cdatalen = (short ) sql__setrang((*scolinf).sci_len , -32768 , 32767);
        (*colinfo).integer.C_1.cinoutlen = (short ) sql__setrang((*scolinf).sci_iolen , -32768 , 32767);
        (*colinfo).integer.C_1.cdatafrac = (unsigned char ) sql__setrang((unsigned char ) (*scolinf).sci_frac , 0 , 255);
        (*colinfo).integer.C_1.ccolstack.integer.C_1.elen_var = (short ) sql__setrang((*scolinf).sci_iolen , -32768 , 32767);
        if ((int ) (*colinfo).integer.C_1.cdatalen == 0)
            {
            (*dmli).d_colptr = (tak00_columninfo *) (int *) 0;
            }
        else
            {
            (*dmli).d_colptr = (tak00_columninfo *) &*colinfo;
            }
        }
    cccprint("<ak641describe_o");
    }



int ak641describe_param(acv, dmli, proc_buf, param_no, scolinf, colinfo)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_sysbufferaddress proc_buf;
int param_no;
tak00_scolinf *scolinf;
tak00_columninfo *colinfo;
    {
    int ptoc_Var390;
    cccprint("AK641 ak641descr");
    (*scolinf).sci_len = 0;
    a12describe_param(&(*acv) , proc_buf , param_no , &(*scolinf));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak641.p" , 22595)).integer.C_4.sp1r_returncode == 0)
        {
        *colinfo = a642standard_colinfo;
        (*colinfo).integer.C_1.cdatatyp = (unsigned char ) sql__setrang((*scolinf).sci_typ , 0 , 39);
        (*colinfo).integer.C_1.cdatalen = (short ) sql__setrang((*scolinf).sci_len , -32768 , 32767);
        (*colinfo).integer.C_1.cinoutlen = (short ) sql__setrang((*scolinf).sci_iolen , -32768 , 32767);
        (*colinfo).integer.C_1.cdatafrac = (unsigned char ) sql__setrang((unsigned char ) (*scolinf).sci_frac , 0 , 255);
        (*colinfo).integer.C_1.ccolstack.integer.C_1.elen_var = (short ) sql__setrang((*scolinf).sci_iolen , -32768 , 32767);
        if ((int ) (*colinfo).integer.C_1.cdatalen == 0)
            {
            (*dmli).d_colptr = (tak00_columninfo *) (int *) 0;
            }
        else
            {
            (*dmli).d_colptr = (tak00_columninfo *) &*colinfo;
            }
        switch ((int ) (*scolinf).sci_typ)
            {
            case 29 :
            case 30 :
                (*scolinf).sci_typ = 0;
                (*dmli).d_datatype = 15;
                break;
            case 0 :
            case 1 :
                (*dmli).d_datatype = 15;
                break;
            default :
                (*dmli).d_datatype = (unsigned char ) sql__setrang((*scolinf).sci_typ , 0 , 39);
                _ptoc_ASSIGN(unsigned char , 2, (*dmli).d_udt_datatype, (*scolinf).sci_udt_id)
                if ((ptoc_Var390 = (int ) (*dmli).d_datatype) == 2 || ptoc_Var390 == 3 || ptoc_Var390 == 24)
                    {
                    (*dmli).d_expand = (short ) sql__setrang((*scolinf).sci_len , -32768 , 32767);
                    }
                break;
            }
        (*dmli).d_ch_datatype = 14;
        }
    cccprint("<ak641describe_p");
    }



int ak641emit_param(acv, dmli, proc_buf, is_in_out_param, proc_colin, first_free)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_sysbufferaddress proc_buf;
char is_in_out_param;
tak00_scolinf *proc_colin;
int first_free;
    {
    struct tgg00_MessBlock *ptoc_Var391;
    tgg00_QualBuf *ptoc_Var392;
    tgg00_StackEntry *ptoc_Var393;
    cccprint("AK641 ak641emit_");
    ptoc_Var391 = &(*acv).a_mblock;
    ptoc_Var392 = (tgg00_QualBuf *) sql__nil(ptoc_Var391->integer.C_4.mb_qual , "vak641.p" , 22644);
    if (first_free + 1 == (int ) (*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak641.p" , 22645)).integer.C_2.mfirst_free && ((int ) (*proc_colin).sci_com_type == 208 || (int ) (*dmli).d_datatype == 15))
        {
        (*(tgg00_StackList *) sql__nil(ptoc_Var391->integer.C_5.mb_st , "vak641.p" , 22650))[sql__setrang(first_free , 1 , 32767) + -1].integer.C_2.ecol_pos = (short ) sql__setrang((*proc_colin).sci_com_type , -32768 , 32767);
        }
    else
        {
        if ((int ) (*proc_colin).sci_typ != 23 && (int ) (*proc_colin).sci_len > 0)
            {
            if ((int ) ptoc_Var392->integer.C_2.mfirst_free > (int ) ptoc_Var391->integer.C_2.mb_st_max)
                {
                a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var391->integer.C_2.mb_st_max);
                }
            else
                {
                ptoc_Var392->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var392->integer.C_2.mqual_cnt , -32768 , 32767 , "vak641.p" , 22661) , -32768 , 32767);
                ptoc_Var393 = &(*(tgg00_StackList *) sql__nil(ptoc_Var391->integer.C_5.mb_st , "vak641.p" , 22662))[sql__setrang((int ) ptoc_Var392->integer.C_2.mfirst_free , 1 , 32767) + -1];
                ptoc_Var393->integer.C_1.etype = 29;
                if ((int ) (*(struct tak_systembuffer *) sql__nil(proc_buf , "vak641.p" , 22665)).integer.C_66.smethod.me_type != 2)
                    {
                    if ((char ) sql__setrang(is_in_out_param , 0 , 1))
                        {
                        ptoc_Var393->integer.C_1.eop = 16;
                        }
                    else
                        {
                        ptoc_Var393->integer.C_1.eop = 96;
                        }
                    }
                else
                    {
                    switch ((int ) (*proc_colin).sci_typ)
                        {
                        case 2 :
                        case 24 :
                            ptoc_Var393->integer.C_1.eop = 2;
                            break;
                        case 4 :
                            ptoc_Var393->integer.C_1.eop = 10;
                            break;
                        case 3 :
                            ptoc_Var393->integer.C_1.eop = 7;
                            break;
                        case 10 :
                            ptoc_Var393->integer.C_1.eop = 50;
                            break;
                        case 11 :
                            ptoc_Var393->integer.C_1.eop = 63;
                            break;
                        case 13 :
                            ptoc_Var393->integer.C_1.eop = 65;
                            break;
                        default :
                            ptoc_Var393->integer.C_1.eop = 96;
                            break;
                        }
                    }
                ptoc_Var393->integer.C_1.epos = (short ) sql__setrang((*proc_colin).sci_len , -32768 , 32767);
                ptoc_Var393->integer.C_1.elen_var = (short ) sql__setrang((*proc_colin).sci_frac , -32768 , 32767);
                if ((int ) (*proc_colin).sci_com_type > 0)
                    {
                    ptoc_Var393->integer.C_2.ecol_pos = (short ) sql__setrang((*proc_colin).sci_com_type , -32768 , 32767);
                    }
                else
                    {
                    ptoc_Var393->integer.C_1.ecol_tab[0] = (unsigned char ) sql__setrang((unsigned char ) sql__ord((int ) (*proc_colin).sci_typ) , 0 , 255);
                    ptoc_Var393->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                    }
                ptoc_Var392->integer.C_2.mfirst_free = (short ) sql__setrang((short ) ((int ) ptoc_Var392->integer.C_2.mfirst_free + 1) , -32768 , 32767);
                }
            }
        }
    cccprint("<ak641emit_param");
    }
