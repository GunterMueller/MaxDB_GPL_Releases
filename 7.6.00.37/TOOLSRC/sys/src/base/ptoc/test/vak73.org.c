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
  unsigned char tsp00_NumError;
typedef
  tsp00_NumError tsp_num_error;
typedef
  unsigned char tsp_sqlmode;
typedef
  unsigned char tsp00_SwapKind;
typedef
  tsp00_SwapKind tsp_swap_kind;
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
  unsigned char tsp00_KnlIdentifier[64];
typedef
  unsigned char tsp00_Key[1024];
typedef
  unsigned char tsp00_MoveObj[8192000];
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
  unsigned char tsp00_VFilename[64];
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
  unsigned char tsp_c64[64];
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
  unsigned char tsp1_warningset_intern[2];
typedef
  unsigned char tsp1_command_options[1];
typedef
  unsigned char tsp1_part_kind;
typedef
  unsigned char tsp1_part_attr[1];
typedef
  struct ptoc_Type3
    {
    tsp1_part_kind sp1p_part_kind;
    tsp1_part_attr sp1p_attributes;
    tsp_int2 sp1p_arg_count;
    tsp_int4 sp1p_segm_offset;
    tsp_int4 sp1p_buf_len;
    tsp_int4 sp1p_buf_size;
    } tsp1_part_header;
typedef
  struct ptoc_Type4
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
  struct ptoc_Type5
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
  struct ptoc_Type6
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
  struct ptoc_Type7
    {
    tsp_code_type sp1h_mess_code;
    tsp_swap_kind sp1h_mess_swap;
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
  struct ptoc_Type8
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
  struct ptoc_Type9
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
  struct ptoc_Type10
    {
    unsigned char para_type;
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
  unsigned char tgg00_DateTimeFormat;
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
  unsigned char tgg00_MessType;
typedef
  unsigned char tgg00_MessType2;
typedef
  unsigned char tgg00_PrivR;
typedef
  unsigned char tgg00_TfnTemp;
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
  unsigned char tgg00_MediaName[64];
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
  unsigned char tgg00_TapeNames[32][64];
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
    tsp00_PageNo tpsPno_gg00;
    tsp00_Int4 tpsIndex_gg00;
    } tgg00_FilePos;
typedef
  struct ptoc_Type28
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
        tgg00_TfnTemp fileTfnTemp_gg00;
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
        tgg00_TfnTemp fileTfnTemp__9_gg00;
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
  struct ptoc_Type29
    {
    tgg00_Surrogate lockTabId_gg00;
    tgg00_ServerdbNo lockSite_gg00;
    unsigned char lockMode_gg00;
    tgg00_LockReqState lockState_gg00;
    tsp00_Int2 lockKeyLen_gg00;
    tsp00_Int2 lockKeyPos_gg00;
    } tgg00_Lock;
typedef
  struct ptoc_Type30
    {
    tgg00_Lkey listkey;
    tgg00_Lkey reckey;
    } tgg00_TwoKeys;
typedef
  struct ptoc_Type31
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
  struct ptoc_Type32
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
  struct ptoc_Type33
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
  struct ptoc_Type34
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
  struct ptoc_Type35
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
  struct ptoc_Type36
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
  struct ptoc_Type37
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
  struct ptoc_Type38
    {
    tsp00_Int4 idc_prim_leafnodes;
    tsp00_Int4 idc_sec_leafnodes;
    tsp00_Int4 idc_prim_keycount;
    tsp00_Int4 idc_sec_keycount;
    tsp00_Int4 idc_nullcount;
    } tgg00_IndexCounter;
typedef
  struct ptoc_Type39
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
  struct ptoc_Type40
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
        tgg00_DateTimeFormat eformat;
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
    } tgg00_StackList[32767];
typedef
  tgg00_StackList *tgg00_StackListPtr;
typedef
  struct ptoc_Type41
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
        tgg00_DateTimeFormat mdatetimeformat;
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
        tsp00_SwapKind mswap_kind;
        } C_3;
      } integer;
    } tgg00_StackDesc;
typedef
  struct ptoc_Type42
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
  struct ptoc_Type43
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
  struct ptoc_Type44
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
  struct ptoc_Type45
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
  struct ptoc_Type46
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
        tgg00_DateTimeFormat mdatetimeformat;
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
  struct ptoc_Type47
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
  struct ptoc_Type48
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
  struct ptoc_Type49
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
  struct ptoc_Type50
    {
    tsp00_CodeType ctype;
    tsp00_CodeType char_default;
    tgg00_DateTimeFormat date_time;
    tsp00_SwapKind kernel_swap;
    } tgg_code_globals;
typedef
  tgg00_DataPartPtr tgg_datapart_ptr;
typedef
  tgg00_DateTimeFormat tgg_datetimeformat;
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
  tgg00_PrivR tgg_priv_r;
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
  struct ptoc_Type40 tgg_stack_entry;
typedef
  tgg00_StackListPtr tgg_stack_list_ptr;
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
  unsigned char tak_sc_symbol;
typedef
  unsigned char tak_procs;
typedef
  unsigned char tak_table_attribute_set[1];
typedef
  struct ptoc_Type57
    {
    tsp_c3 p_count;
    tsp_c1 p_id;
    tgg_message_type p_kind;
    tsp00_Uint1 p_no;
    tsp_int2 p_filler;
    } tak_parskey;
typedef
  struct ptoc_Type58
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
  struct ptoc_Type59
    {
    tak_procs n_proc;
    tak_special_node_info n_special;
    tak_sc_symbol n_symb;
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
  unsigned char tak_keyword[18];
typedef
  tsp_c18 tak_oldidentifier;
typedef
  tak_keyword tak_keywordtab[533];
typedef
  unsigned char tak_order_errortext[256];
typedef
  unsigned char tak_parsbuffer[8076];
typedef
  struct ptoc_Type40 tak_16stackentry[16];
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
  struct ptoc_Type40 tak_ak_constr_stack[989];
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
    tgg_datetimeformat resdatetimeformat;
    char resimpl_upd_stat;
    char resexecuted;
    unsigned char resselect_fetch;
    tgg_surrogate resanalyze_cmdid;
    } tak_resname_record;
typedef
  struct ptoc_Type40 tak_trigger_stack[900];
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
      unsigned char jop_datatyp;
      tsp_int2 jop_datalen;
      tsp_int2 jop_inoutlen;
      tak_jcolpropset jop_propset;
      char jop_outer_join;
      tsp_int2 jop_outpos;
      } jo_recs[2];
    unsigned char jo_op;
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
      unsigned char vdatatype;
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
    unsigned char j12operator;
    unsigned char j12fill1;
    tsp_int4 j12fill2;
    } tak_viewjoins[336];
typedef
  struct ptoc_Type40 tak_view_stack_entries[1007];
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
    tgg_datetimeformat res_pars_dt_format;
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
  tsp1_param_info tak_paraminfoarr[2700];
typedef
  struct ptoc_Type142
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
  struct ptoc_Type143
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
  struct ptoc_Type144
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
  struct ptoc_Type145
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
        tak_sc_symbol fp_sc_symbol;
        tsp00_C1 fp_objecttype;
        tak_len_and_frac fp_len_and_frac_v15;
        tsp_int2 fp_inoutlen_v15;
        } C_fp_get_name_value;
      } tak_fp_kind_type;
    } tak_field_pars;
typedef
  struct ptoc_Type146
    {
    tsp_int2 all_lock_count;
    tsp_int2 all_lock_fill;
    tgg00_Lock all_locks[240];
    } tak_ak_all_locks;
typedef
  tak_field_pars tak_parser_fields[2025];
typedef
  struct ptoc_Type147
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
  struct ptoc_Type148
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
  struct ptoc_Type149
    {
    tak_parskey uparsid;
    tsp_int2 ufirstsel;
    tsp_int2 usecondsel;
    tsp_int2 ufiller;
    unsigned char uis_kind;
    char uis_all;
    } tak_union_array[493];
typedef
  struct ptoc_Type150
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
  struct ptoc_Type151
    {
    tak_parskey cparsk;
    tsp_int2 cnextpars;
    tak_complex_infoset centryinfo;
    unsigned char cqual_jv_upd;
    } tak_complex_array[669];
typedef
  struct ptoc_Type152
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
  struct ptoc_Type153
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
  struct ptoc_Type154
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
  struct ptoc_Type155
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
    struct ptoc_Type156
      {
      tsp00_Uint1 socoldtabno;
      tsp00_Uint1 socselecttabno;
      tsp00_Uint1 socfields;
      tsp00_Uint1 socfill;
      } tc_tables[16];
    tak_all_corcolumns tc_columns;
    } tak_correcord;
typedef
  struct ptoc_Type157
    {
    tsp_int2 lpos;
    tsp_int2 llen;
    } tak_linkposinfo[2048];
typedef
  struct ptoc_Type158
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
  struct ptoc_Type159
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
  struct ptoc_Type160
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
  struct ptoc_Type161
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
  struct ptoc_Type162
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
  struct ptoc_Type163
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
  struct ptoc_Type164
    {
    tsp_knl_identifier dbp_sysdba;
    tgg00_FileId dbp_tree;
    } tak_dbproc_array[5];
typedef
  struct ptoc_Type165
    {
    tsp_int2 dbc_cnt;
    tsp_int2 dbc_fil;
    tak_sysbufferaddress dbc_ptr;
    tak_dbproc_array dbc_cache;
    } tak_dbproc_cache;
typedef
  struct ptoc_Type166
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
  struct ptoc_Type167
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
  struct ptoc_Type168
    {
    tsp_int4 cst_top;
    tsp_moveobj_ptr cst_stack[16];
    } tak_cmd_stack;
typedef
  struct ptoc_Type169
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
  struct ptoc_Type170
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
  struct ptoc_Type171
    {
    tsp00_Int4 ai_uktId;
    tsp00_Int4 ai_badAllocCountDown;
    tak_AllocatorMonitorInfo ai_alloc_sys;
    tak_AllocatorMonitorInfo ai_alloc_usr;
    } tak_allocator_info;
typedef
  struct ptoc_Type172
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
    tgg_datetimeformat a_dt_format;
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
  unsigned char tak_lockenum;
typedef
  struct ptoc_Type173
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
  struct ptoc_Type174
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
    unsigned char d_datatype;
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
    unsigned char d_ch_datatype;
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
  struct ptoc_Type175
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
  struct ptoc_Type176
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
  struct ptoc_Type177
    {
    int mfr_count1;
    int mfr_count2;
    int mfr_cnt;
    tsp_int2 mfr_reclength;
    char mfr_invarp;
    tgg_basis_error mfr_b_err;
    } mfetchrec;
tak_keywordtab a01kw;
tgg_sysinfokey a01sysnullkey;
tsp_knl_identifier a01_i_rest;
tsp_knl_identifier a01_il_b_identifier;
char a01diag_analyze_on;
tgg_code_globals g01code;
char g01diag_moni_parse_on;
char g01unicode;
int a01_next_symbol();
int a01_call_put();
int a01_get_keyword();
int a01_is_end_symbol();
int a01aligned_cmd_len();
char a01_eqkey();
char a01mandatory_keyword();
int a01_force_symbol();
int a02_aresulttablename();
int a02_l_aparameter_list();
int a02_s_aparameterspec();
int a02_atablename();
int a03_anumeric_literal();
int a03_ainteger();
int a05identifier_get();
int a05int4_get();
int a06extcolno();
int a06init_curr_retpart();
int a06finish_curr_retpart();
int a06retpart_move();
int a061td_colinfo();
int a07_error();
int a07_b_put_error();
int a07_nb_put_error();
short a07_return_code();
int a09activate_lock();
int a10get_sysinfo();
int a10add_sysinfo();
int a10repl_sysinfo();
int a10rel_sysinfo();
int a10_rel_sysinfo();
int a10_key_del();
int a10del_sysinfo();
int a260DropParseIds();
int a508_lget_long_columns();
char a508_lcol_found();
int a52_lock_statement();
int a54_dml_init();
int a54_get_pparsp_pinfop();
int a54store_cmd_shinfo();
int a54return_parsid();
int a545diag_parse_info();
int a57_update_statement();
int a58_delete();
int a60_change_results();
int a60_p_info_output();
int a60_get_longinfobuffer();
int a60rescount();
int a60_put_result();
int a61_p_short_info();
int a61_p_long_info();
int a62_direct_ordered_select();
int a660_search_one_table();
int a660_new_pparsp();
int a660_prefix_delete();
int a663_drop_parsid();
int a663_del_result();
int a663get_result_records();
int a663_get_result_info();
int a663pid_restable_get();
int a663restable_get();
int a663_resname();
int a74_search_fetch();
int a92_add_pcount();
int b07cget_record();
int b07cnext_record();
int b07cprev_record();
int g10fil();
int g10mv1();
int g10mv2();
int g10mv3();
int g10mv5();
int g10mv6();
int g10mv7();
int g10mv10();
int g10mvr();
int s10fil();
int s10mv4();
int s10mv9();
int s20ch4sw();
int s26find_part();
char s30eq();
int s41p4int();
int t01mess2type();
int t01int4();
int t01moveobj();
int t01lidentifier();
int t01buf();
int s40gsint();
int cccprint();
int ak73close_semantic();
int ak73describe_semantic();
int ak73describe_parsid_semantic();
int ak73drop_parseid();
int ak73rest_fetch();
int ak73fetch_semantic();
int ak73one_semantic_fetch();
int ak73get_longinfo();
int ak73next_stamp();
int ak73one_before_pos();
int a73_aclose_resulttable();
int a73_adescribe_resulttable();
int a73_adparseid();
int a73_afetch_statement();
int a73_anext_statement();
int a73_call_semantic();
int a73_close_execute();
int a73_describe_execute();
int a73_ex_describe();
int a73_infolen_get();
int a73_more_fetch();
int a73_one_fetch();
int a73_calc_unused_space();



int ak73close_semantic(acv)
tak_all_command_glob *acv;
    {
    int curr_n;
    tsp_knl_identifier res_name;
    tak_syspointerarr sparr;
    tak_all_command_glob *ptoc_Var0;
    struct ptoc_Type59 *ptoc_Var1;
    cccprint("AK73  ak73close_");
    ptoc_Var0 = &*acv;
    ptoc_Var1 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var0->a_ap_tree , "vak73.p" , 13687))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var0->a_ap_tree , "vak73.p" , 13687))[0].n_lo_level , 0 , 32767)];
    curr_n = 1;
    _ptoc_ASSIGN(unsigned char , 64, res_name, a01_il_b_identifier)
    if ((int ) ptoc_Var1->n_sa_level != 0)
        {
        curr_n = (int ) ptoc_Var1->n_sa_level;
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var0->a_ap_tree , "vak73.p" , 13695))[sql__setrang(curr_n , 0 , 32767)].n_symb == 42)
            {
            a05identifier_get(&(*acv) , curr_n , sizeof(unsigned char [64]) , res_name);
            }
        }
    a73_close_execute(&(*acv) , &sparr , res_name , ptoc_Var0->a_modul_name);
    cccprint("<ak73close_seman");
    }



int ak73describe_semantic(acv)
tak_all_command_glob *acv;
    {
    int curr_n;
    tsp_knl_identifier res_name;
    tak_syspointerarr sparr;
    tak_all_command_glob *ptoc_Var2;
    struct ptoc_Type59 *ptoc_Var3;
    cccprint("AK73  ak73descri");
    ptoc_Var2 = &*acv;
    ptoc_Var3 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var2->a_ap_tree , "vak73.p" , 13722))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var2->a_ap_tree , "vak73.p" , 13722))[0].n_lo_level , 0 , 32767)];
    curr_n = 1;
    _ptoc_ASSIGN(unsigned char , 64, res_name, a01_il_b_identifier)
    if ((int ) ptoc_Var3->n_sa_level != 0)
        {
        curr_n = (int ) ptoc_Var3->n_sa_level;
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var2->a_ap_tree , "vak73.p" , 13730))[sql__setrang(curr_n , 0 , 32767)].n_symb == 42)
            {
            a05identifier_get(&(*acv) , curr_n , sizeof(unsigned char [64]) , res_name);
            }
        }
    a73_describe_execute(&(*acv) , &sparr , res_name , ptoc_Var2->a_modul_name);
    cccprint("<ak73describe_se");
    }



int ak73describe_parsid_semantic(acv)
tak_all_command_glob *acv;
    {
    int i;
    tak00_colinfo_ptr col_ptr;
    tak_dml_info dmli;
    tak_parskey lparskey;
    tak_all_command_glob *ptoc_Var4;
    tak_dml_info *ptoc_Var5;
    cccprint("AK73  ak73descri");
    ptoc_Var4 = &*acv;
    ptoc_Var5 = &dmli;
    lparskey = ptoc_Var4->a_pars_last_key;
    if (ptoc_Var4->a_data_ptr != (tsp00_MoveObj *) (int *) 0)
        {
        g10mv1("VAK73 " , 1 , (*(tsp1_part *) sql__nil(ptoc_Var4->a_data_part , "vak73.p" , 13767)).integer.C_2.sp1p_buf_size , sizeof(tak_parskey ) , sql__nil(ptoc_Var4->a_data_ptr , "vak73.p" , 13768) , 5 , &lparskey , 1 , sizeof(tak_parskey ) , &(*(tsp1_segment *) sql__nil(ptoc_Var4->a_return_segm , "vak73.p" , 13770)).integer.C_4.sp1r_returncode);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var4->a_return_segm , "vak73.p" , 13772)).integer.C_4.sp1r_returncode == 0)
        {
        a54_dml_init(&dmli , 0);
        a663pid_restable_get(&(*acv) , &lparskey , 0 , &ptoc_Var5->d_sparr);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var4->a_return_segm , "vak73.p" , 13779)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var5->d_keylen = -8;
        a60_get_longinfobuffer(&(*acv) , &ptoc_Var5->d_sparr , (int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var5->d_sparr.pbasep , "vak73.p" , 13784)).integer.C_28.sresult.boutcolno + -1 , -1);
        }
    i = 2;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var4->a_return_segm , "vak73.p" , 13788)).integer.C_4.sp1r_returncode == 0)
        {
        while (i <= (int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var5->d_sparr.pbasep , "vak73.p" , 13790)).integer.C_28.sresult.boutcolno && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var4->a_return_segm , "vak73.p" , 13791)).integer.C_4.sp1r_returncode == 0)
            {
            a06extcolno(&(*(struct tak_systembuffer *) sql__nil(ptoc_Var5->d_sparr.pbasep , "vak73.p" , 13793)).integer.C_1.sbase , i , &col_ptr);
            ptoc_Var5->d_inoutpos = (short ) sql__setrang((*(tak00_columninfo *) sql__nil(col_ptr , "vak73.p" , 13794)).integer.C_1.ccolstack.integer.C_1.epos , -32768 , 32767);
            a61_p_long_info(&(*acv) , &dmli , sql__nil(col_ptr , "vak73.p" , 13795));
            i = i + 1;
            }
        }
    a60_p_info_output(&(*acv) , &ptoc_Var5->d_sparr);
    if (ptoc_Var5->d_sparr.pinfop != (struct tak_systembuffer *) (int *) 0)
        {
        a10_key_del(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var5->d_sparr.pinfop , "vak73.p" , 13804)).integer.C_50.syskey);
        ptoc_Var5->d_sparr.pinfop = (struct tak_systembuffer *) (int *) 0;
        }
    if (ptoc_Var5->d_sparr.pcolnamep != (struct tak_systembuffer *) (int *) 0)
        {
        a10_key_del(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var5->d_sparr.pcolnamep , "vak73.p" , 13811)).integer.C_50.syskey);
        ptoc_Var5->d_sparr.pcolnamep = (struct tak_systembuffer *) (int *) 0;
        }
    cccprint("<ak73describe_pa");
    }



int ak73drop_parseid(acv)
tak_all_command_glob *acv;
    {
    tak_parsid p;
    int del_cnt;
    tak_all_command_glob *ptoc_Var6;
    cccprint("AK73  ak73drop_p");
    ptoc_Var6 = &*acv;
    t01int4(5 , "a_data_lengt" , ptoc_Var6->a_data_length);
    if ((int ) ptoc_Var6->a_ex_kind != 2 && (int ) (*(tsp1_part *) sql__nil(ptoc_Var6->a_data_part , "vak73.p" , 13837)).integer.C_2.sp1p_part_kind != 10)
        {
        a07_b_put_error(&(*acv) , 6292 , 1);
        }
    else
        {
        if (ptoc_Var6->a_data_length != 12)
            {
            a07_b_put_error(&(*acv) , 6296 , ptoc_Var6->a_data_length);
            }
        else
            {
            if (!s30eq(ptoc_Var6->a_transinf.tri_trans.trSessionId_gg00.ci4_gg00 , sql__nil(ptoc_Var6->a_data_ptr , "vak73.p" , 13846) , 1 , 4))
                {
                a07_b_put_error(&(*acv) , 6510 , 1);
                }
            else
                {
                if ((int ) ptoc_Var6->a_pars_last_key.p_kind != 21)
                    {
                    g10mv7("VAK73 " , 2 , (*(tsp1_part *) sql__nil(ptoc_Var6->a_data_part , "vak73.p" , 13855)).integer.C_2.sp1p_buf_size , sizeof(tak_parsid ) , sql__nil(ptoc_Var6->a_data_ptr , "vak73.p" , 13855) , 1 , &p , 1 , sizeof(tak_parsid ) , &(*(tsp1_segment *) sql__nil(ptoc_Var6->a_return_segm , "vak73.p" , 13857)).integer.C_4.sp1r_returncode);
                    if ((int ) p.boolean.C_false.pid_appl_info[0] == 60 && (int ) p.boolean.C_true.pid_parsk.p_kind == 18)
                        {
                        a260DropParseIds(&(*acv) , &p.boolean.C_true.pid_parsk);
                        }
                    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var6->a_return_segm , "vak73.p" , 13864)).integer.C_4.sp1r_returncode == 0)
                        {
                        a663_drop_parsid(&(*acv) , &p.boolean.C_true.pid_parsk , &del_cnt);
                        }
                    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var6->a_return_segm , "vak73.p" , 13870)).integer.C_4.sp1r_returncode == 0 && del_cnt > 0)
                        {
                        a92_add_pcount(&(*acv) , p.boolean.C_true.pid_parsk.p_count);
                        }
                    }
                }
            }
        }
    cccprint("<ak73drop_parsei");
    }



int ak73rest_fetch(acv)
tak_all_command_glob *acv;
    {
    tak_all_command_glob *ptoc_Var7;
    struct tgg00_MessBlock *ptoc_Var8;
    cccprint("AK73  ak73rest_f");
    ptoc_Var7 = &*acv;
    ptoc_Var8 = &ptoc_Var7->a_mblock;
    a06retpart_move(&(*acv) , (tsp00_MoveObj *) (*(tgg00_DataPart *) sql__nil(ptoc_Var8->integer.C_3.mb_data , "vak73.p" , 13895)).integer.C_1.mbp_buf , ptoc_Var8->integer.C_2.mb_data_len);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var7->a_return_segm , "vak73.p" , 13896)).integer.C_4.sp1r_returncode == 0)
        {
        a06finish_curr_retpart(&(*acv) , 5 , 1);
        }
    cccprint("<ak73rest_fetch ");
    }



int ak73fetch_semantic(acv)
tak_all_command_glob *acv;
    {
    tgg_basis_error _b_err;
    int _multi_command;
    tsp_int2 _start_node;
    tsp_int2 _first_resname_n;
    tsp_int2 _pos_resname_n;
    tsp_int2 _curr_n;
    int _del_cnt;
    tak_syspointerarr _sparr;
    tgg_sysinfokey _sysk;
    tak_parskey _parsk;
    tsp_knl_identifier _new_res_name;
    tsp_knl_identifier _res_name;
    struct ptoc_Type59 *ptoc_Var9;
    cccprint("AK73  ak73fetch_");
    _start_node = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 13928))[0].n_lo_level , -32768 , 32767);
    if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 13929))[sql__setrang((int ) _start_node , 0 , 32767)].n_sa_level > 0)
        {
        _multi_command = 1;
        (*acv).a_ex_kind = 0;
        (*acv).a_command_kind = 3;
        }
    else
        {
        _multi_command = 0;
        }
    _first_resname_n = 0;
    _pos_resname_n = 0;
    while ((int ) _start_node > 0 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak73.p" , 13941)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 13943))[sql__setrang((int ) _start_node , 0 , 32767)].n_proc == 64)
            {
            ak73one_semantic_fetch(&(*acv) , &_sparr , (short ) sql__setrang(_start_node , -32768 , 32767) , &_pos_resname_n , _multi_command);
            if (_multi_command > 0)
                {
                if ((int ) _first_resname_n == 0)
                    {
                    _first_resname_n = (short ) sql__setrang(_pos_resname_n , -32768 , 32767);
                    _curr_n = (short ) sql__setrang(_first_resname_n , -32768 , 32767);
                    }
                else
                    {
                    (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 13959))[sql__setrang((int ) _curr_n , 0 , 32767)].n_sa_level = (short ) sql__setrang(_pos_resname_n , -32768 , 32767);
                    _curr_n = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 13960))[sql__setrang((int ) _curr_n , 0 , 32767)].n_sa_level , -32768 , 32767);
                    }
                if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak73.p" , 13963)).integer.C_4.sp1r_returncode == 0 && (int ) _pos_resname_n > 0)
                    {
                    a05identifier_get(&(*acv) , (int ) _pos_resname_n , sizeof(unsigned char [64]) , _new_res_name);
                    _curr_n = (short ) sql__setrang(_first_resname_n , -32768 , 32767);
                    while ((int ) _curr_n != (int ) _pos_resname_n)
                        {
                        a05identifier_get(&(*acv) , (int ) _curr_n , sizeof(unsigned char [64]) , _res_name);
                        if (_ptoc_MEMCMP(unsigned char , 64, _res_name , _new_res_name) == 0 ? 1 : 0)
                            {
                            a07_nb_put_error(&(*acv) , 6232 , (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 13978))[sql__setrang((int ) _pos_resname_n , 0 , 32767)].n_pos , _res_name);
                            _curr_n = (short ) sql__setrang(_pos_resname_n , -32768 , 32767);
                            }
                        else
                            {
                            _curr_n = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 13982))[sql__setrang((int ) _curr_n , 0 , 32767)].n_sa_level , -32768 , 32767);
                            }
                        }
                    }
                if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak73.p" , 13988)).integer.C_4.sp1r_returncode != 0)
                    {
                    _parsk = (*acv).a_pars_last_key;
                    _parsk.p_id[0] = (unsigned char ) sql__setrang((*acv).a_first_parskey , 0 , 255);
                    _parsk.p_kind = 10;
                    _parsk.p_no = (unsigned char)'\0';
                    a660_prefix_delete(&(*acv) , &_parsk , &_del_cnt , 3);
                    }
                }
            }
        else
            {
            _sysk = (*(struct tak_systembuffer *) sql__nil(_sparr.pinfop , "vak73.p" , 14003)).integer.C_50.syskey;
            a10add_sysinfo(&(*acv) , &_sparr.pinfop , &_b_err);
            if ((int ) _b_err != 0)
                {
                a07_b_put_error(&(*acv) , (short ) sql__setrang(_b_err , -32768 , 32767) , 1);
                }
            else
                {
                switch ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14010))[sql__setrang((int ) _start_node , 0 , 32767)].n_proc)
                    {
                    case 43 :
                        a52_lock_statement(&(*acv) , (short ) sql__setrang(_start_node , -32768 , 32767));
                        break;
                    case 48 :
                        a57_update_statement(&(*acv) , (short ) sql__setrang(_start_node , -32768 , 32767));
                        break;
                    case 49 :
                        a58_delete(&(*acv) , (short ) sql__setrang(_start_node , -32768 , 32767));
                        break;
                    case 53 :
                        a62_direct_ordered_select(&(*acv) , (short ) sql__setrang(_start_node , -32768 , 32767));
                        break;
                    default :
                        a07_b_put_error(&(*acv) , 3400 , (int ) _start_node);
                        break;
                    }
                a10del_sysinfo(&(*acv) , &_sysk , &_b_err);
                if ((int ) _b_err != 0)
                    {
                    a07_b_put_error(&(*acv) , (short ) sql__setrang(_b_err , -32768 , 32767) , 1);
                    }
                }
            }
        _multi_command = sql__succ(_multi_command , -2147483647 , 2147483647 , "vak73.p" , 14032);
        _start_node = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14033))[sql__setrang((int ) _start_node , 0 , 32767)].n_sa_level , -32768 , 32767);
        if ((int ) _start_node > 0)
            {
            ptoc_Var9 = &(*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14036))[sql__setrang((int ) _start_node , 0 , 32767)];
            if ((int ) ptoc_Var9->n_proc == 54 && ((int ) ptoc_Var9->n_subproc == 12 || (int ) ptoc_Var9->n_subproc == 27))
                {
                _start_node = 0;
                }
            }
        }
    cccprint("<ak73fetch_seman");
    }

static
  unsigned char ptoc_Var10[2] = 
  {
  (unsigned char)'\t', (unsigned char)'\0'
  };
static
  tsp1_param_opt_set ptoc_Var11 = 
  {
  (unsigned char)'\001'
  };


int ak73one_semantic_fetch(acv, sparr, start_node, resn_node, multi_command)
tak_all_command_glob *acv;
tak_syspointerarr *sparr;
tsp_int2 start_node;
tsp_int2 *resn_node;
int multi_command;
    {
    tgg_basis_error _b_err;
    char _deleted;
    char _f_ok;
    char _mfetch;
    char _pos_is_parameter;
    tsp_int2 _mfetch_count;
    tsp_int2 _position_node;
    int _curr_n;
    int _param_cnt;
    int _k;
    int _merror;
    int _mreturn;
    int _output_len;
    int _output_params;
    int _resnpos;
    tsp_int2 _mfetch_count2;
    tsp_int2 _args;
    tsp_int4 _count1;
    tgg_message2_type _mtype;
    tsp_knl_identifier _res_name;
    tak00_colinfo_ptr _col_ptr;
    tak_sysbufferaddress _resname_addr;
    tgg_sysinfokey _ke;
    tak_syspointerarr _isparr;
    tak_parsid _fetch_parsid;
    tak_parsid _select_parsid;
    tsp_num_error _res;
    tsp1_part_ptr _part_ptr;
    tsp_resnum _relative_pos;
    struct ptoc_Type59 *ptoc_Var12;
    struct ptoc_Type59 *ptoc_Var13;
    struct ptoc_Type59 *ptoc_Var14;
    struct ptoc_Type59 *ptoc_Var15;
    tak_resname_record *ptoc_Var16;
    tgg00_SysInfoKey *ptoc_Var17;
    tak_resname_record *ptoc_Var18;
    tak_resultkeyrecord *ptoc_Var19;
    tgg00_TransContext *ptoc_Var20;
    unsigned char ptoc_Var21[2];
    unsigned char ptoc_Var22[2];
    unsigned char ptoc_Var23[2];
    tak_shortinforecord *ptoc_Var24;
    tsp1_param_info *ptoc_Var25;
    tak_baserecord *ptoc_Var26;
    tak_baserecord *ptoc_Var27;
    int ptoc_Var28;
    tak00_columninfo *ptoc_Var29;
    tak_shortinforecord *ptoc_Var30;
    int ptoc_Var31;
    tak_resname_record *ptoc_Var32;
    tak_resname_record *ptoc_Var33;
    struct tak_systembuffer *ptoc_Var34;
    tak_shortinforecord *ptoc_Var35;
    tak_syspointerarr *ptoc_Var36;
    tak_parsinforecord *ptoc_Var37;
    cccprint("AK73  ak73one_se");
    _f_ok = 0;
    ptoc_Var12 = &(*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14090))[sql__setrang((int ) start_node , 0 , 32767)];
    if ((int ) ptoc_Var12->n_subproc == 1)
        {
        _mfetch = 0;
        }
    else
        {
        _mfetch = 1;
        }
    _curr_n = (int ) ptoc_Var12->n_lo_level;
    ptoc_Var13 = &(*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14102))[sql__setrang(_curr_n , 0 , 32767)];
    switch ((int ) ptoc_Var13->n_subproc)
        {
        case 3 :
            _mtype = 23;
            break;
        case 4 :
            _mtype = 41;
            break;
        case 5 :
            _mtype = 32;
            break;
        case 6 :
            _mtype = 53;
            break;
        case 8 :
            _mtype = 16;
            break;
        case 7 :
            _mtype = 58;
            break;
        case 17 :
            _mtype = 59;
            break;
        default :
            (short ) sql__caseerr("vak73.p" , 14104);
            break;
        }
    if (!(char ) sql__setrang(_mfetch , 0 , 1))
        {
        _mfetch_count2 = 1;
        }
    else
        {
        if ((int ) (*acv).a_ex_kind == 0)
            {
            _mfetch_count = 1;
            }
        else
            {
            if ((*acv).a_rescount_part == (tsp1_part *) (int *) 0)
                {
                a07_b_put_error(&(*acv) , 6488 , 1);
                }
            else
                {
                if ((*(tsp1_part *) sql__nil((*acv).a_rescount_part , "vak73.p" , 14136)).integer.C_2.sp1p_buf_len < 7 || (*(tsp1_part *) sql__nil((*acv).a_rescount_part , "vak73.p" , 14138)).integer.C_2.sp1p_buf_len < 11 && (_ptoc_MEMCMP(unsigned char , 5, (*acv).a_comp_vers , "62080") < 0 ? 1 : 0))
                    {
                    a07_b_put_error(&(*acv) , 6488 , 1);
                    }
                else
                    {
                    s40gsint((*(tsp1_part *) sql__nil((*acv).a_rescount_part , "vak73.p" , 14144)).integer.C_1.sp1p_buf , 2 , 10 , &_mfetch_count , &_res);
                    if ((int ) _res != 0 || (int ) (*(tsp1_part *) sql__nil((*acv).a_rescount_part , "vak73.p" , 14147)).integer.C_1.sp1p_buf[0] != 0)
                        {
                        a07_b_put_error(&(*acv) , 6488 , 1);
                        }
                    }
                }
            }
        }
    _count1 = -1;
    _pos_is_parameter = 0;
    _position_node = 0;
    t01mess2type(5 , "_mtype      " , (unsigned char ) sql__setrang(_mtype , 0 , 95));
    if ((int ) _mtype == 16 || (int ) _mtype == 59)
        {
        ptoc_Var14 = &(*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14164))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14164))[sql__setrang(_curr_n , 0 , 32767)].n_lo_level , 0 , 32767)];
        if ((int ) ptoc_Var14->n_symb == 49 || (int ) ptoc_Var14->n_symb == 10 || (int ) ptoc_Var14->n_symb == 11)
            {
            a05int4_get(&(*acv) , ptoc_Var14->n_pos , (short ) sql__setrang(ptoc_Var14->n_length , -32768 , 32767) , &_count1);
            if ((int ) _mtype == 59 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak73.p" , 14172)).integer.C_4.sp1r_returncode == 0)
                {
                if (_count1 == 0)
                    {
                    _mtype = 58;
                    }
                if (_ptoc_MEMCMP(unsigned char , 5, (*acv).a_comp_vers , "62080") < 0 ? 0 : 1)
                    {
                    _relative_pos[0] = (unsigned char)'\0';
                    s41p4int(_relative_pos , 2 , _count1 , &_res);
                    a06retpart_move(&(*acv) , (tsp00_MoveObj *) _relative_pos , sizeof(tsp00_ResNum ));
                    a06finish_curr_retpart(&(*acv) , 24 , 1);
                    }
                }
            }
        else
            {
            _position_node = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14195))[sql__setrang(_curr_n , 0 , 32767)].n_lo_level , -32768 , 32767);
            _pos_is_parameter = 1;
            }
        }
    _ptoc_ASSIGN(unsigned char , 64, _res_name, a01_il_b_identifier)
    _resnpos = 1;
    if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14204))[sql__setrang(_curr_n , 0 , 32767)].n_sa_level != 0)
        {
        _curr_n = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14207))[sql__setrang(_curr_n , 0 , 32767)].n_sa_level;
        ptoc_Var15 = &(*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14208))[sql__setrang(_curr_n , 0 , 32767)];
        if ((int ) ptoc_Var15->n_symb == 42)
            {
            a05identifier_get(&(*acv) , _curr_n , sizeof(unsigned char [64]) , _res_name);
            _resnpos = ptoc_Var15->n_pos;
            if ((int ) _mtype == 16)
                {
                *resn_node = (short ) sql__setrang((short ) _curr_n , -32768 , 32767);
                }
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak73.p" , 14224)).integer.C_4.sp1r_returncode == 0)
        {
        if (_ptoc_MEMCMP(unsigned char , 64, _res_name , a01_i_rest) == 0 ? 1 : 0)
            {
            ak73rest_fetch(&(*acv));
            }
        else
            {
            a663_resname(&(*acv) , _res_name , (*acv).a_modul_name , &_resname_addr , 0 , 1);
            if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak73.p" , 14232)).integer.C_4.sp1r_returncode == 0)
                {
                if (_ptoc_MEMCMP(unsigned char , 64, _res_name , a01_il_b_identifier) == 0 ? 0 : 1)
                    {
                    if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14237))[sql__setrang(_curr_n , 0 , 32767)].n_sa_level != 0)
                        {
                        _curr_n = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14239))[sql__setrang(_curr_n , 0 , 32767)].n_sa_level;
                        }
                    }
                ptoc_Var16 = &(*(struct tak_systembuffer *) sql__nil(_resname_addr , "vak73.p" , 14242)).integer.C_52.sresname;
                if ((char ) sql__setrang((*acv).a_info_output , 0 , 1) && (int ) ptoc_Var16->resinfobuf > 0 && (int ) (*acv).a_ex_kind != 0 && (int ) (*(struct tak_systembuffer *) sql__nil(_resname_addr , "vak73.p" , 14246)).integer.C_52.sresname.resselect_fetch != 1)
                    {
                    ak73get_longinfo(&(*acv) , &(*sparr) , &ptoc_Var16->restreeid.integer.C_8.fileTempCnt_gg00);
                    }
                if ((int ) (*(struct tak_systembuffer *) sql__nil(_resname_addr , "vak73.p" , 14252)).integer.C_52.sresname.resselect_fetch == 1)
                    {
                    if ((int ) _mtype == 16)
                        {
                        a07_b_put_error(&(*acv) , 6516 , (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14258))[sql__setrang(_curr_n , 0 , 32767)].n_pos);
                        }
                    }
                if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak73.p" , 14261)).integer.C_4.sp1r_returncode == 0)
                    {
                    _output_params = 0;
                    _param_cnt = 0;
                    _ke = a01sysnullkey;
                    ptoc_Var17 = &_ke;
                    ptoc_Var18 = &(*(struct tak_systembuffer *) sql__nil(_resname_addr , "vak73.p" , 14267)).integer.C_52.sresname;
                    if (!(char ) sql__setrang(ptoc_Var18->resexecuted , 0 , 1) && (int ) ptoc_Var18->reseparsk.p_kind != 63)
                        {
                        ptoc_Var17->integer.C_3.sauthid[0] = (unsigned char)'\377';
                        s10mv4(sizeof(tak_parskey ) , sizeof(tsp00_C8 ) , &ptoc_Var18->reseparsk , 1 , ptoc_Var17->integer.C_3.sauthid , 2 , 6);
                        ptoc_Var17->integer.C_3.sauthid[6] = (unsigned char)'\0';
                        }
                    else
                        {
                        ptoc_Var17->integer.C_4.stempid = ptoc_Var18->restreeid.integer.C_8.fileTempCnt_gg00;
                        }
                    _ptoc_ASSIGN(unsigned char , 2, ptoc_Var17->integer.C_2.sentrytyp, "\0\262")
                    t01int4(5 , "curr_n      " , _curr_n);
                    if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14285))[sql__setrang(_curr_n , 0 , 32767)].n_proc == 64 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14286))[sql__setrang(_curr_n , 0 , 32767)].n_subproc == 9 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak73.p" , 14287)).integer.C_4.sp1r_returncode == 0)
                        {
                        _curr_n = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14290))[sql__setrang(_curr_n , 0 , 32767)].n_lo_level;
                        _param_cnt = 1;
                        t01int4(5 , "curr_n      " , _curr_n);
                        while ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14293))[sql__setrang(_curr_n , 0 , 32767)].n_sa_level != 0)
                            {
                            _param_cnt = sql__succ(_param_cnt , -2147483647 , 2147483647 , "vak73.p" , 14295);
                            t01int4(5 , "count output" , _param_cnt);
                            _curr_n = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14297))[sql__setrang(_curr_n , 0 , 32767)].n_sa_level;
                            }
                        _output_params = _param_cnt;
                        }
                    if (multi_command == 0)
                        {
                        if ((int ) (*(struct tak_systembuffer *) sql__nil(_resname_addr , "vak73.p" , 14305)).integer.C_52.sresname.resselect_fetch != 1)
                            {
                            a10get_sysinfo(&(*acv) , &_ke , 0 , &(*sparr).px[1] , &_b_err);
                            if ((int ) _b_err != 0)
                                {
                                a07_b_put_error(&(*acv) , (short ) sql__setrang(_b_err , -32768 , 32767) , 1);
                                }
                            else
                                {
                                ptoc_Var19 = &(*(struct tak_systembuffer *) sql__nil((*sparr).px[1] , "vak73.p" , 14314)).integer.C_26.sreskey;
                                if ((int ) ptoc_Var19->res_outcolno != _output_params)
                                    {
                                    ptoc_Var20 = &(*acv).a_transinf.tri_trans;
                                    _ptoc_MOVE(unsigned char , 2, ptoc_Var20->trWarning_gg00, *(unsigned char (*)[2]) sql__psetadd(ptoc_Var21 , ptoc_Var20->trWarning_gg00 , ptoc_Var10 , sizeof(unsigned char [2])));
                                    if ((int ) ptoc_Var19->res_outcolno < _output_params || _output_params == 0)
                                        {
                                        _output_params = (int ) ptoc_Var19->res_outcolno;
                                        }
                                    }
                                }
                            }
                        else
                            {
                            if ((int ) (*acv).a_ex_kind != 0)
                                {
                                a07_b_put_error(&(*acv) , 6408 , 1);
                                }
                            }
                        }
                    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak73.p" , 14341)).integer.C_4.sp1r_returncode == 0)
                        {
                        if ((int ) (*acv).a_ex_kind == 0)
                            {
                            if (multi_command == 0)
                                {
                                if (_param_cnt == 0)
                                    {
                                    (*acv).a_count_variab = (short ) sql__setrang((short ) ((int ) (*acv).a_count_variab + _output_params) , -32768 , 32767);
                                    }
                                if ((char ) sql__setrang(_mfetch , 0 , 1))
                                    {
                                    a54_get_pparsp_pinfop(&(*acv) , &(*sparr) , 42);
                                    (*acv).a_precomp_info_byte = (unsigned char)'+';
                                    }
                                else
                                    {
                                    a54_get_pparsp_pinfop(&(*acv) , &(*sparr) , 28);
                                    (*acv).a_precomp_info_byte = (unsigned char)'*';
                                    }
                                if ((int ) (*(struct tak_systembuffer *) sql__nil(_resname_addr , "vak73.p" , 14365)).integer.C_52.sresname.resselect_fetch == 1)
                                    {
                                    (*acv).a_count_variab = 0;
                                    }
                                }
                            else
                                {
                                if (multi_command == 1)
                                    {
                                    a54_get_pparsp_pinfop(&(*acv) , &(*sparr) , 28);
                                    }
                                else
                                    {
                                    a660_new_pparsp(&(*acv) , &(*sparr) , 0 , 1);
                                    }
                                }
                            if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak73.p" , 14379)).integer.C_4.sp1r_returncode == 0)
                                {
                                if ((char ) sql__setrang(_pos_is_parameter , 0 , 1))
                                    {
                                    _k = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14385))[sql__setrang((int ) _position_node , 0 , 32767)].n_length;
                                    ptoc_Var24 = &(*(struct tak_systembuffer *) sql__nil((*sparr).pinfop , "vak73.p" , 14386)).integer.C_31.sshortinfo;
                                    ptoc_Var25 = &ptoc_Var24->siinfo[sql__setrang(_k , 1 , 2700) + -1];
                                    _ptoc_MOVE(unsigned char , 1, ptoc_Var25->sp1i_mode, ptoc_Var11);
                                    ptoc_Var25->sp1i_io_type = 0;
                                    ptoc_Var25->sp1i_data_type = 0;
                                    ptoc_Var25->sp1i_frac = (unsigned char)'\0';
                                    ptoc_Var25->sp1i_length = 10;
                                    ptoc_Var25->sp1i_in_out_len = 7;
                                    ptoc_Var25->sp1i_bufpos = (*acv).a_input_data_pos;
                                    if (_k > (int ) ptoc_Var24->sicount)
                                        {
                                        ptoc_Var24->sicount = (short ) sql__setrang((short ) _k , -32768 , 32767);
                                        }
                                    }
                                if ((int ) (*(struct tak_systembuffer *) sql__nil(_resname_addr , "vak73.p" , 14403)).integer.C_52.sresname.resselect_fetch != 1)
                                    {
                                    a663restable_get(&(*acv) , 1 , (*(struct tak_systembuffer *) sql__nil(_resname_addr , "vak73.p" , 14407)).integer.C_52.sresname.reskey_name , &_isparr , &_f_ok);
                                    }
                                else
                                    {
                                    _output_len = -_output_params;
                                    _f_ok = 1;
                                    }
                                if ((char ) sql__setrang(_f_ok , 0 , 1))
                                    {
                                    if (multi_command == 0)
                                        {
                                        (*acv).a_output_data_pos = 1;
                                        if ((int ) (*(struct tak_systembuffer *) sql__nil(_resname_addr , "vak73.p" , 14423)).integer.C_52.sresname.resselect_fetch != 1)
                                            {
                                            ptoc_Var26 = &(*(struct tak_systembuffer *) sql__nil(_isparr.pbasep , "vak73.p" , 14427)).integer.C_1.sbase;
                                            t01int4(5 , "bfirstcolind" , (int ) ptoc_Var26->bfirstcolind);
                                            ptoc_Var27 = &(*(struct tak_systembuffer *) sql__nil(_isparr.pbasep , "vak73.p" , 14431)).integer.C_1.sbase;
                                            _col_ptr = ptoc_Var27->integer.C_2.bcolumn[sql__setrang((int ) ptoc_Var27->bfirstcolind , 1 , 1536) + -1];
                                            ptoc_Var28 = _output_params + 1;
                                            if (2 <= ptoc_Var28)
                                                {
                                                _k = 2;
                                                do
                                                    {
                                                    a061td_colinfo(sql__nil(_col_ptr , "vak73.p" , 14436) , 1);
                                                    _col_ptr = (*(struct tak_systembuffer *) sql__nil(_isparr.pbasep , "vak73.p" , 14437)).integer.C_1.sbase.integer.C_2.bcolumn[sql__setrang((int ) (*(tak00_columninfo *) sql__nil(_col_ptr , "vak73.p" , 14438)).integer.C_1.cnextind , 1 , 1536) + -1];
                                                    (*acv).a_output_data_pos = (int ) (*(tak00_columninfo *) sql__nil(_col_ptr , "vak73.p" , 14439)).integer.C_1.ccolstack.integer.C_1.epos;
                                                    if ((char ) sql__setrang(_pos_is_parameter , 0 , 1))
                                                        {
                                                        a61_p_short_info(&(*acv) , &(*sparr) , sql__nil(_col_ptr , "vak73.p" , 14444) , _k , 0);
                                                        }
                                                    else
                                                        {
                                                        a61_p_short_info(&(*acv) , &(*sparr) , sql__nil(_col_ptr , "vak73.p" , 14447) , _k + -1 , 0);
                                                        }
                                                    if (_k == ptoc_Var28)
                                                        {
                                                        break;
                                                        }
                                                    _k += 1;
                                                    }
                                                while (!0);
                                                }
                                            if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak73.p" , 14451)).integer.C_4.sp1r_returncode == 0)
                                                {
                                                ptoc_Var29 = (tak00_columninfo *) sql__nil(_col_ptr , "vak73.p" , 14453);
                                                if ((_ptoc_MEMCMP(unsigned char , 5, (*acv).a_comp_vers , "62080") < 0 ? 1 : 0) && ((ptoc_Var28 = (int ) ptoc_Var29->integer.C_1.cdatatyp) == 0 || ptoc_Var28 == 1 || ptoc_Var28 == 12) && (int ) ptoc_Var29->integer.C_1.cdatalen > 18)
                                                    {
                                                    _output_len = (int ) ptoc_Var29->integer.C_1.ccolstack.integer.C_1.epos + 10;
                                                    }
                                                else
                                                    {
                                                    _output_len = (int ) ptoc_Var29->integer.C_1.ccolstack.integer.C_1.epos + (int ) ptoc_Var29->integer.C_1.cinoutlen + -1;
                                                    }
                                                }
                                            if ((char ) sql__setrang(_pos_is_parameter , 0 , 1))
                                                {
                                                _output_params = sql__succ(_output_params , -2147483647 , 2147483647 , "vak73.p" , 14468);
                                                }
                                            ptoc_Var30 = &(*(struct tak_systembuffer *) sql__nil((*sparr).pinfop , "vak73.p" , 14470)).integer.C_31.sshortinfo;
                                            ptoc_Var28 = _output_params + 1;
                                            ptoc_Var31 = ptoc_Var30->sicount;
                                            if (ptoc_Var28 <= ptoc_Var31)
                                                {
                                                _k = ptoc_Var28;
                                                do
                                                    {
                                                    ptoc_Var30->siinfo[sql__setrang(_k , 1 , 2700) + -1].sp1i_data_type = 14;
                                                    if (_k == ptoc_Var31)
                                                        {
                                                        break;
                                                        }
                                                    _k += 1;
                                                    }
                                                while (!0);
                                                }
                                            }
                                        _fetch_parsid.pid_session = (*acv).a_transinf.tri_trans.trSessionId_gg00;
                                        _fetch_parsid.boolean.C_true.pid_parsk = (*acv).a_pars_last_key;
                                        _fetch_parsid.boolean.C_false.pid_appl_info[0] = (unsigned char ) sql__setrang((unsigned char ) (*acv).a_precomp_info_byte , 0 , 255);
                                        _fetch_parsid.boolean.C_false.pid_dtime_info[0] = (unsigned char ) sql__setrang((unsigned char ) sql__ord(0) , 0 , 255);
                                        }
                                    _select_parsid.pid_session = (*acv).a_transinf.tri_trans.trSessionId_gg00;
                                    ptoc_Var32 = &(*(struct tak_systembuffer *) sql__nil(_resname_addr , "vak73.p" , 14488)).integer.C_52.sresname;
                                    if ((int ) ptoc_Var32->reseparsk.p_kind != 0)
                                        {
                                        _select_parsid.boolean.C_true.pid_parsk = ptoc_Var32->reseparsk;
                                        _select_parsid.boolean.C_false.pid_appl_info[0] = (unsigned char ) sql__setrang((unsigned char ) ptoc_Var32->resparsinfobyte , 0 , 255);
                                        _select_parsid.boolean.C_false.pid_dtime_info[0] = (unsigned char ) sql__setrang((unsigned char ) sql__ord((int ) ptoc_Var32->resdatetimeformat) , 0 , 255);
                                        }
                                    else
                                        {
                                        s10fil(sizeof(tak_parsid ) , &_select_parsid , 1 , sizeof(tak_parsid ) , (unsigned char)'\0');
                                        }
                                    if (multi_command == 0)
                                        {
                                        if ((char ) sql__setrang((*(tsp1_segment *) sql__nil((*acv).a_cmd_segm , "vak73.p" , 14509)).integer.C_3.sp1c_prepare , 0 , 1))
                                            {
                                            a54store_cmd_shinfo(&(*acv) , &(*acv).a_pars_last_key , (*sparr).pinfop);
                                            }
                                        if ((char ) sql__setrang(g01diag_moni_parse_on , 0 , 1))
                                            {
                                            if (_ptoc_MEMCMP(unsigned char , 4, _select_parsid.pid_session.ci4_gg00 , "\0\0\0\0") == 0 ? 1 : 0)
                                                {
                                                _ptoc_ASSIGN(unsigned char , 4, _select_parsid.pid_session.ci4_gg00, "\377\377\377\377")
                                                }
                                            a545diag_parse_info(&(*acv) , &_fetch_parsid , &_select_parsid);
                                            if (_ptoc_MEMCMP(unsigned char , 4, _select_parsid.pid_session.ci4_gg00 , "\377\377\377\377") == 0 ? 1 : 0)
                                                {
                                                _ptoc_ASSIGN(unsigned char , 4, _select_parsid.pid_session.ci4_gg00, "\0\0\0\0")
                                                }
                                            }
                                        if ((char ) sql__setrang(a01diag_analyze_on , 0 , 1))
                                            {
                                            ptoc_Var33 = &(*(struct tak_systembuffer *) sql__nil(_resname_addr , "vak73.p" , 14535)).integer.C_52.sresname;
                                            t01buf(5 , &(*(struct tak_systembuffer *) sql__nil(_resname_addr , "vak73.p" , 14537)).integer.C_52.sresname , 1 , sizeof(tak_resname_record ));
                                            }
                                        ptoc_Var34 = (struct tak_systembuffer *) sql__nil((*sparr).pinfop , "vak73.p" , 14542);
                                        ptoc_Var35 = &ptoc_Var34->integer.C_31.sshortinfo;
                                        a06retpart_move(&(*acv) , (tsp00_MoveObj *) ptoc_Var34->integer.C_31.sshortinfo.siinfo , (int ) ptoc_Var35->sicount * sizeof(tsp1_param_info ));
                                        _args = (short ) sql__setrang(ptoc_Var35->sicount , -32768 , 32767);
                                        a10_key_del(&(*acv) , &ptoc_Var34->integer.C_50.syskey);
                                        (*sparr).pinfop = (struct tak_systembuffer *) (int *) 0;
                                        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak73.p" , 14550)).integer.C_4.sp1r_returncode == 0)
                                            {
                                            a06finish_curr_retpart(&(*acv) , 14 , (short ) sql__setrang(_args , -32768 , 32767));
                                            }
                                        }
                                    }
                                }
                            if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak73.p" , 14563)).integer.C_4.sp1r_returncode == 0)
                                {
                                ptoc_Var36 = &*sparr;
                                ptoc_Var37 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var36->pparsp , "vak73.p" , 14567)).integer.C_23.sparsinfo;
                                ptoc_Var37->integer.C_2.p_select = 0;
                                _ptoc_ASSIGN(unsigned char , 64, ptoc_Var37->integer.C_2.p_resn, _res_name)
                                _ptoc_ASSIGN(unsigned char , 64, ptoc_Var37->integer.C_2.p_modul_name, (*acv).a_modul_name)
                                if ((char ) sql__setrang(_mfetch , 0 , 1))
                                    {
                                    ptoc_Var37->integer.C_2.p_mtyp = 42;
                                    ptoc_Var37->integer.C_2.p_reuse = 0;
                                    }
                                else
                                    {
                                    ptoc_Var37->integer.C_2.p_mtyp = 28;
                                    }
                                ptoc_Var37->integer.C_2.p_mtyp2 = (unsigned char ) sql__setrang(_mtype , 0 , 95);
                                ptoc_Var37->integer.C_2.p_session_isolevel = (unsigned char ) sql__setrang((*acv).a_iso_level , 0 , 255);
                                ptoc_Var37->integer.C_2.p_use_sess_isolevel = 0;
                                if ((char ) sql__setrang(_pos_is_parameter , 0 , 1))
                                    {
                                    ptoc_Var37->integer.C_2.p_posno = 32767;
                                    }
                                else
                                    {
                                    ptoc_Var37->integer.C_2.p_posno = _count1;
                                    }
                                ptoc_Var37->integer.C_2.p_p_no = (short ) sql__setrang((short ) (*acv).a_input_data_pos , -32768 , 32767);
                                (*acv).a_input_data_pos = (*acv).a_input_data_pos + 7;
                                ptoc_Var37->integer.C_2.p_prepare = 0;
                                ptoc_Var37->integer.C_2.p_resinfolen = (short ) sql__setrang((short ) _output_len , -32768 , 32767);
                                s10mv9(sizeof(tak_parsid ) , sizeof(tak_field_pars ) , &_select_parsid , 1 , &ptoc_Var37->integer.C_2.p_pars_infos[0] , 1 , sizeof(tak_parsid ));
                                (*(struct tak_systembuffer *) sql__nil(ptoc_Var36->pparsp , "vak73.p" , 14607)).integer.C_0.b_sl = 328;
                                (*(struct tak_systembuffer *) sql__nil(ptoc_Var36->pparsp , "vak73.p" , 14610)).integer.C_23.sparsinfo.integer.C_1.p_fullen = (int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var36->pparsp , "vak73.p" , 14610)).integer.C_0.b_sl;
                                (*(struct tak_systembuffer *) sql__nil(ptoc_Var36->pparsp , "vak73.p" , 14611)).integer.C_0.b_kl = 12;
                                a10add_sysinfo(&(*acv) , &ptoc_Var36->pparsp , &_b_err);
                                if ((int ) _b_err != 0)
                                    {
                                    a07_b_put_error(&(*acv) , (short ) sql__setrang(_b_err , -32768 , 32767) , 1);
                                    }
                                else
                                    {
                                    if (multi_command == 0)
                                        {
                                        a06retpart_move(&(*acv) , (tsp00_MoveObj *) &_fetch_parsid , sizeof(tak_parsid ));
                                        a06finish_curr_retpart(&(*acv) , 10 , 1);
                                        a06retpart_move(&(*acv) , (tsp00_MoveObj *) &_select_parsid , sizeof(tak_parsid ));
                                        a06finish_curr_retpart(&(*acv) , 11 , 1);
                                        }
                                    }
                                }
                            }
                        else
                            {
                            if (!(char ) sql__setrang((*(struct tak_systembuffer *) sql__nil(_resname_addr , "vak73.p" , 14638)).integer.C_52.sresname.resexecuted , 0 , 1))
                                {
                                a07_b_put_error(&(*acv) , 6516 , (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 14641))[sql__setrang(_curr_n , 0 , 32767)].n_pos);
                                }
                            else
                                {
                                if ((char ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*sparr).px[1] , "vak73.p" , 14643)).integer.C_26.sreskey.res_first_fetch , 0 , 1) && (int ) _mtype == 58)
                                    {
                                    a07_b_put_error(&(*acv) , 6232 , 7);
                                    }
                                else
                                    {
                                    if ((char ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*sparr).px[1] , "vak73.p" , 14650)).integer.C_26.sreskey.res_build , 0 , 1))
                                        {
                                        if (!(char ) sql__setrang(_mfetch , 0 , 1))
                                            {
                                            a73_one_fetch(&(*acv) , &(*sparr) , (unsigned char ) sql__setrang(_mtype , 0 , 95) , _count1);
                                            }
                                        else
                                            {
                                            a73_more_fetch(&(*acv) , &(*sparr) , (unsigned char ) sql__setrang(_mtype , 0 , 95) , _count1 , (int ) _mfetch_count);
                                            }
                                        }
                                    else
                                        {
                                        a74_search_fetch(&(*acv) , &(*sparr) , (unsigned char ) sql__setrang(_mtype , 0 , 95) , _count1 , &_resname_addr , (char ) sql__setrang(!(char ) sql__setrang(_mfetch , 0 , 1) , 0 , 1) , (int ) _mfetch_count);
                                        }
                                    _mreturn = (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak73.p" , 14668)).integer.C_4.sp1r_returncode;
                                    _merror = (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak73.p" , 14669)).integer.C_4.sp1r_errorpos;
                                    _deleted = 0;
                                    (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak73.p" , 14671)).integer.C_4.sp1r_returncode = 0;
                                    if ((int ) (*acv).a_sqlmode == 2)
                                        {
                                        if (_mreturn == (int ) a07_return_code(6408 , (unsigned char ) sql__setrang((*acv).a_sqlmode , 0 , 3)))
                                            {
                                            a663_del_result(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(_resname_addr , "vak73.p" , 14680)).integer.C_52.sresname , 1 , 1);
                                            _f_ok = 0;
                                            _deleted = 1;
                                            }
                                        else
                                            {
                                            if ((char ) sql__setrang(_mfetch , 0 , 1) && (_ptoc_MEMCMP(unsigned char , 5, (*acv).a_comp_vers , "62000") < 0 ? 0 : 1))
                                                {
                                                s26find_part(sql__nil((*acv).a_return_segm , "vak73.p" , 14691) , 5 , &_part_ptr);
                                                if (_part_ptr != (tsp1_part *) (int *) 0)
                                                    {
                                                    if (1 && ((*(tsp1_part *) sql__nil(_part_ptr , "vak73.p" , 14696)).integer.C_2.sp1p_attributes[0] & 1) != 0)
                                                        {
                                                        a663_del_result(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(_resname_addr , "vak73.p" , 14700)).integer.C_52.sresname , 1 , 1);
                                                        _f_ok = 0;
                                                        _deleted = 1;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    if (!(char ) sql__setrang(_deleted , 0 , 1))
                                        {
                                        a10repl_sysinfo(&(*acv) , &(*sparr).px[1] , &_b_err);
                                        if ((int ) _b_err != 0)
                                            {
                                            a07_b_put_error(&(*acv) , (short ) sql__setrang(_b_err , -32768 , 32767) , 1);
                                            }
                                        }
                                    if (_mreturn != 0)
                                        {
                                        (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak73.p" , 14728)).integer.C_4.sp1r_returncode = (short ) sql__setrang((short ) _mreturn , -32768 , 32767);
                                        (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak73.p" , 14729)).integer.C_4.sp1r_errorpos = _merror;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    if ((char ) sql__setrang(_f_ok , 0 , 1))
        {
        a10_rel_sysinfo(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(_resname_addr , "vak73.p" , 14746)).integer.C_50.syskey);
        }
    cccprint("<ak73one_semanti");
    }



int ak73get_longinfo(acv, sparr, res_id)
tak_all_command_glob *acv;
tak_syspointerarr *sparr;
tsp_int4 *res_id;
    {
    tgg_basis_error mfr_b_err;
    tgg_sysinfokey ke;
    tak_all_command_glob *ptoc_Var38;
    tgg00_SysInfoKey *ptoc_Var39;
    cccprint("AK73  ak73get_lo");
    ptoc_Var38 = &*acv;
    ke = a01sysnullkey;
    ptoc_Var39 = &ke;
    ptoc_Var39->integer.C_4.stempid = *res_id;
    _ptoc_ASSIGN(unsigned char , 2, ptoc_Var39->integer.C_2.sentrytyp, "\0\300")
    a10get_sysinfo(&(*acv) , &ke , 0 , &(*sparr).pcolnamep , &mfr_b_err);
    if ((int ) mfr_b_err == 0)
        {
        _ptoc_ASSIGN(unsigned char , 2, ke.integer.C_2.sentrytyp, "\0\204")
        a10get_sysinfo(&(*acv) , &ke , 1 , &(*sparr).pinfop , &mfr_b_err);
        }
    if ((int ) mfr_b_err != 0)
        {
        a07_b_put_error(&(*acv) , (short ) sql__setrang(mfr_b_err , -32768 , 32767) , 1);
        }
    else
        {
        a60_p_info_output(&(*acv) , &(*sparr));
        a10rel_sysinfo((*sparr).pcolnamep);
        }
    cccprint("<ak73get_longinf");
    }

static
  tsp1_param_opt_set ptoc_Var40 = 
  {
  (unsigned char)'\001'
  };


int ak73next_stamp(acv)
tak_all_command_glob *acv;
    {
    tgg_basis_error mfr_b_err;
    tsp1_param_info ts;
    tak_dml_info dmli;
    int curr_n;
    tak_parsid parsid;
    tak_parsid dummy_parsid;
    tak_all_command_glob *ptoc_Var41;
    tak_parskey *ptoc_Var42;
    tsp1_param_info *ptoc_Var43;
    tak_syspointerarr *ptoc_Var44;
    tak_parsinforecord *ptoc_Var45;
    cccprint("AK73  ak73next_s");
    ptoc_Var41 = &*acv;
    ptoc_Var42 = &ptoc_Var41->a_pars_last_key;
    ptoc_Var42->p_kind = 67;
    ptoc_Var43 = &ts;
    _ptoc_MOVE(unsigned char , 1, ptoc_Var43->sp1i_mode, ptoc_Var40);
    ptoc_Var43->sp1i_io_type = 1;
    ptoc_Var43->sp1i_data_type = 4;
    ptoc_Var43->sp1i_frac = (unsigned char)'\0';
    ptoc_Var43->sp1i_length = 8;
    ptoc_Var43->sp1i_in_out_len = 9;
    ptoc_Var43->sp1i_bufpos = 1;
    ptoc_Var44 = &dmli.d_sparr;
    curr_n = 1;
    curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var41->a_ap_tree , "vak73.p" , 14829))[sql__setrang(curr_n , 0 , 32767)].n_lo_level;
    if (curr_n != 0)
        {
        a54_dml_init(&dmli , 0);
        dmli.d_acttabindex = 1;
        a660_search_one_table(&(*acv) , &dmli , curr_n , 0 , 0 , 0 , 0);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var41->a_return_segm , "vak73.p" , 14839)).integer.C_4.sp1r_returncode == 0)
        {
        a54_get_pparsp_pinfop(&(*acv) , &dmli.d_sparr , 67);
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var41->a_return_segm , "vak73.p" , 14843)).integer.C_4.sp1r_returncode == 0)
            {
            if (dmli.d_sparr.pinfop != (struct tak_systembuffer *) (int *) 0)
                {
                a10_key_del(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(dmli.d_sparr.pinfop , "vak73.p" , 14849)).integer.C_50.syskey);
                dmli.d_sparr.pinfop = (struct tak_systembuffer *) (int *) 0;
                }
            ptoc_Var45 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var44->pparsp , "vak73.p" , 14853)).integer.C_23.sparsinfo;
            ptoc_Var45->integer.C_2.p_mtyp = 67;
            ptoc_Var45->integer.C_2.p_mtyp2 = 0;
            ptoc_Var45->integer.C_2.p_select = 0;
            ptoc_Var45->integer.C_2.p_prepare = 0;
            ptoc_Var45->integer.C_2.p_session_isolevel = (unsigned char ) sql__setrang((*acv).a_iso_level , 0 , 255);
            ptoc_Var45->integer.C_2.p_use_sess_isolevel = 0;
            (*(struct tak_systembuffer *) sql__nil(ptoc_Var44->pparsp , "vak73.p" , 14863)).integer.C_0.b_sl = 316;
            (*(struct tak_systembuffer *) sql__nil(ptoc_Var44->pparsp , "vak73.p" , 14864)).integer.C_23.sparsinfo.integer.C_1.p_fullen = (int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var44->pparsp , "vak73.p" , 14864)).integer.C_0.b_sl;
            (*(struct tak_systembuffer *) sql__nil(ptoc_Var44->pparsp , "vak73.p" , 14865)).integer.C_0.b_kl = 12;
            a10add_sysinfo(&(*acv) , &ptoc_Var44->pparsp , &mfr_b_err);
            if ((int ) mfr_b_err != 0)
                {
                a07_b_put_error(&(*acv) , (short ) sql__setrang(mfr_b_err , -32768 , 32767) , 1);
                }
            else
                {
                parsid.pid_session = ptoc_Var41->a_transinf.tri_trans.trSessionId_gg00;
                parsid.boolean.C_true.pid_parsk = ptoc_Var41->a_pars_last_key;
                parsid.boolean.C_false.pid_appl_info[0] = (unsigned char ) sql__setrang((unsigned char ) ptoc_Var41->a_precomp_info_byte , 0 , 255);
                parsid.boolean.C_false.pid_dtime_info[0] = (unsigned char ) sql__setrang((unsigned char ) sql__ord(0) , 0 , 255);
                if ((char ) sql__setrang(g01diag_moni_parse_on , 0 , 1))
                    {
                    _ptoc_ASSIGN(unsigned char , 4, dummy_parsid.pid_session.ci4_gg00, "\377\377\377\377")
                    a545diag_parse_info(&(*acv) , &parsid , &dummy_parsid);
                    }
                a06retpart_move(&(*acv) , (tsp00_MoveObj *) &ts , sizeof(tsp1_param_info ));
                a06finish_curr_retpart(&(*acv) , 14 , 1);
                a06retpart_move(&(*acv) , (tsp00_MoveObj *) &parsid , sizeof(tak_parsid ));
                a06finish_curr_retpart(&(*acv) , 10 , 1);
                }
            }
        }
    cccprint("<ak73next_stamp ");
    }



int ak73one_before_pos(acv, sparr, pos)
tak_all_command_glob *acv;
tak_syspointerarr *sparr;
tsp_int4 pos;
    {
    tgg_basis_error mfr_b_err;
    tgg_bd_set_result_record set_result;
    char previous;
    int i;
    int start;
    tsp_int4 mfr_count2;
    tgg00_FilePos tree_pos;
    tgg_lkey bd_lkey;
    tak_all_command_glob *ptoc_Var46;
    tak_resultkeyrecord *ptoc_Var47;
    tgg00_BdSetResultRecord *ptoc_Var48;
    int ptoc_Var49;
    cccprint("AK73  ak73one_be");
    previous = 0;
    ptoc_Var46 = &*acv;
    ptoc_Var47 = &(*(struct tak_systembuffer *) sql__nil((*sparr).px[1] , "vak73.p" , 14923)).integer.C_26.sreskey;
    if (pos == ptoc_Var47->res_nextpos || pos == ptoc_Var47->res_prevpos)
        {
        if (pos == ptoc_Var47->res_prevpos)
            {
            ptoc_Var47->res_nextkeys.reckeyspec.ks_len = (short ) sql__setrang(ptoc_Var47->res_prevkeys.reckeyspec.ks_len , -32768 , 32767);
            g10mv6("VAK73 " , 3 , sizeof(tak_res_keysbuf ) , sizeof(tak_res_keysbuf ) , ptoc_Var47->boolean.C_false.res_keysbuf , (int ) ptoc_Var47->res_prevkeys.reckeyspec.ks_pos , ptoc_Var47->boolean.C_false.res_keysbuf , (int ) ptoc_Var47->res_nextkeys.reckeyspec.ks_pos , (int ) ptoc_Var47->res_nextkeys.reckeyspec.ks_len , &(*(tsp1_segment *) sql__nil(ptoc_Var46->a_return_segm , "vak73.p" , 14937)).integer.C_4.sp1r_returncode);
            ptoc_Var47->res_nextpos = ptoc_Var47->res_prevpos;
            }
        }
    else
        {
        if (pos > ptoc_Var47->res_nextpos)
            {
            mfr_count2 = ptoc_Var47->res_nextpos + 1;
            }
        else
            {
            if (pos > ptoc_Var47->res_prevpos)
                {
                ptoc_Var47->res_nextpos = ptoc_Var47->res_prevpos;
                mfr_count2 = ptoc_Var47->res_nextpos + 1;
                ptoc_Var47->res_nextkeys.reckeyspec.ks_len = (short ) sql__setrang(ptoc_Var47->res_prevkeys.reckeyspec.ks_len , -32768 , 32767);
                g10mv6("VAK73 " , 4 , sizeof(tak_res_keysbuf ) , sizeof(tak_res_keysbuf ) , ptoc_Var47->boolean.C_false.res_keysbuf , (int ) ptoc_Var47->res_prevkeys.reckeyspec.ks_pos , ptoc_Var47->boolean.C_false.res_keysbuf , (int ) ptoc_Var47->res_nextkeys.reckeyspec.ks_pos , (int ) ptoc_Var47->res_nextkeys.reckeyspec.ks_len , &(*(tsp1_segment *) sql__nil(ptoc_Var46->a_return_segm , "vak73.p" , 14959)).integer.C_4.sp1r_returncode);
                }
            else
                {
                if (pos < ptoc_Var47->res_prevpos - pos || pos == 1)
                    {
                    ptoc_Var47->res_nextkeys.reckeyspec.ks_len = 0;
                    ptoc_Var47->res_nextpos = 0;
                    mfr_count2 = 1;
                    }
                else
                    {
                    ptoc_Var47->res_nextkeys.reckeyspec.ks_len = (short ) sql__setrang(ptoc_Var47->res_prevkeys.reckeyspec.ks_len , -32768 , 32767);
                    g10mv6("VAK73 " , 5 , sizeof(tak_res_keysbuf ) , sizeof(tak_res_keysbuf ) , ptoc_Var47->boolean.C_false.res_keysbuf , (int ) ptoc_Var47->res_prevkeys.reckeyspec.ks_pos , ptoc_Var47->boolean.C_false.res_keysbuf , (int ) ptoc_Var47->res_nextkeys.reckeyspec.ks_pos , (int ) ptoc_Var47->res_nextkeys.reckeyspec.ks_len , &(*(tsp1_segment *) sql__nil(ptoc_Var46->a_return_segm , "vak73.p" , 14978)).integer.C_4.sp1r_returncode);
                    ptoc_Var47->res_nextpos = ptoc_Var47->res_prevpos;
                    mfr_count2 = ptoc_Var47->res_prevpos;
                    previous = 1;
                    }
                }
            }
        ptoc_Var48 = &set_result;
        ptoc_Var48->bd_key_check_len = 0;
        ptoc_Var48->bd_max_rec_cnt = 1;
        ptoc_Var48->bd_max_fill_len = ptoc_Var46->a_mblock.integer.C_2.mb_data_size;
        ptoc_Var48->bd_next = 1;
        tree_pos.tpsPno_gg00 = 2147483647;
        if (!(char ) sql__setrang(previous , 0 , 1))
            {
            while ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var46->a_return_segm , "vak73.p" , 14997)).integer.C_4.sp1r_returncode == 0 && mfr_count2 < pos)
                {
                if (pos - mfr_count2 > 32767)
                    {
                    set_result.bd_max_rec_cnt = 32767;
                    }
                else
                    {
                    set_result.bd_max_rec_cnt = pos - mfr_count2;
                    }
                bd_lkey.integer.C_11.len = (short ) sql__setrang(ptoc_Var47->res_nextkeys.reckeyspec.ks_len , -32768 , 32767);
                g10mv3("VAK73 " , 6 , sizeof(tak_res_keysbuf ) , sizeof(tsp00_Key ) , ptoc_Var47->boolean.C_false.res_keysbuf , (int ) ptoc_Var47->res_nextkeys.reckeyspec.ks_pos , bd_lkey.integer.C_11.k , 1 , (int ) bd_lkey.integer.C_11.len , &(*(tsp1_segment *) sql__nil(ptoc_Var46->a_return_segm , "vak73.p" , 15011)).integer.C_4.sp1r_returncode);
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var46->a_return_segm , "vak73.p" , 15012)).integer.C_4.sp1r_returncode == 0)
                    {
                    b07cnext_record(&ptoc_Var46->a_transinf.tri_trans , &ptoc_Var47->res_restree , &bd_lkey , &set_result , &tree_pos , (*(tgg00_DataPart *) sql__nil(ptoc_Var46->a_mblock.integer.C_3.mb_data , "vak73.p" , 15017)).integer.C_1.mbp_buf);
                    switch ((int ) ptoc_Var46->a_transinf.tri_trans.trError_gg00)
                        {
                        case 1200 :
                        case 350 :
                            mfr_b_err = 0;
                            break;
                        case 1420 :
                            mfr_b_err = 1420;
                            a07_b_put_error(&(*acv) , 6408 , 1);
                            break;
                        default :
                            mfr_b_err = (short ) sql__setrang(ptoc_Var46->a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                            break;
                        }
                    }
                if ((int ) mfr_b_err != 0)
                    {
                    a07_b_put_error(&(*acv) , (short ) sql__setrang(mfr_b_err , -32768 , 32767) , 1);
                    }
                else
                    {
                    start = 1;
                    ptoc_Var49 = set_result.bd_rec_cnt + -1;
                    if (1 <= ptoc_Var49)
                        {
                        i = 1;
                        do
                            {
                            start = start + (int ) ptoc_Var47->res_reclen;
                            if (i == ptoc_Var49)
                                {
                                break;
                                }
                            i += 1;
                            }
                        while (!0);
                        }
                    g10mv2("VAK73 " , 7 , ptoc_Var46->a_mblock.integer.C_2.mb_data_size , sizeof(tak_res_keysbuf ) , (*(tgg00_DataPart *) sql__nil(ptoc_Var46->a_mblock.integer.C_3.mb_data , "vak73.p" , 15044)).integer.C_1.mbp_buf , start + 8 , ptoc_Var47->boolean.C_false.res_keysbuf , (int ) ptoc_Var47->res_nextkeys.reckeyspec.ks_pos , (int ) ptoc_Var47->res_keylen , &(*(tsp1_segment *) sql__nil(ptoc_Var46->a_return_segm , "vak73.p" , 15047)).integer.C_4.sp1r_returncode);
                    ptoc_Var47->res_nextkeys.reckeyspec.ks_len = (short ) sql__setrang(ptoc_Var47->res_keylen , -32768 , 32767);
                    ptoc_Var47->res_nextpos = ptoc_Var47->res_nextpos + set_result.bd_rec_cnt;
                    }
                mfr_count2 = mfr_count2 + set_result.bd_rec_cnt;
                set_result.bd_next = 0;
                }
            }
        else
            {
            while ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var46->a_return_segm , "vak73.p" , 15058)).integer.C_4.sp1r_returncode == 0 && mfr_count2 >= pos)
                {
                set_result.bd_max_rec_cnt = mfr_count2 - pos + 1;
                bd_lkey.integer.C_11.len = (short ) sql__setrang(ptoc_Var47->res_nextkeys.reckeyspec.ks_len , -32768 , 32767);
                g10mv3("VAK73 " , 8 , sizeof(tak_res_keysbuf ) , sizeof(tsp00_Key ) , ptoc_Var47->boolean.C_false.res_keysbuf , (int ) ptoc_Var47->res_nextkeys.reckeyspec.ks_pos , bd_lkey.integer.C_11.k , 1 , (int ) bd_lkey.integer.C_11.len , &(*(tsp1_segment *) sql__nil(ptoc_Var46->a_return_segm , "vak73.p" , 15067)).integer.C_4.sp1r_returncode);
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var46->a_return_segm , "vak73.p" , 15068)).integer.C_4.sp1r_returncode == 0)
                    {
                    b07cprev_record(&ptoc_Var46->a_transinf.tri_trans , &ptoc_Var47->res_restree , &bd_lkey , &set_result , &tree_pos , (*(tgg00_DataPart *) sql__nil(ptoc_Var46->a_mblock.integer.C_3.mb_data , "vak73.p" , 15073)).integer.C_1.mbp_buf);
                    switch ((int ) ptoc_Var46->a_transinf.tri_trans.trError_gg00)
                        {
                        case 1200 :
                        case 350 :
                            mfr_b_err = 0;
                            break;
                        case 1440 :
                            mfr_b_err = 1420;
                            a07_b_put_error(&(*acv) , 6408 , 1);
                            break;
                        default :
                            mfr_b_err = (short ) sql__setrang(ptoc_Var46->a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                            break;
                        }
                    }
                if ((int ) mfr_b_err != 0)
                    {
                    a07_b_put_error(&(*acv) , (short ) sql__setrang(mfr_b_err , -32768 , 32767) , 1);
                    }
                else
                    {
                    start = 1;
                    ptoc_Var49 = set_result.bd_rec_cnt + -1;
                    if (1 <= ptoc_Var49)
                        {
                        i = 1;
                        do
                            {
                            start = start + (int ) ptoc_Var47->res_reclen;
                            if (i == ptoc_Var49)
                                {
                                break;
                                }
                            i += 1;
                            }
                        while (!0);
                        }
                    g10mv2("VAK73 " , 9 , ptoc_Var46->a_mblock.integer.C_2.mb_data_size , sizeof(tak_res_keysbuf ) , (*(tgg00_DataPart *) sql__nil(ptoc_Var46->a_mblock.integer.C_3.mb_data , "vak73.p" , 15100)).integer.C_1.mbp_buf , start + 8 , ptoc_Var47->boolean.C_false.res_keysbuf , (int ) ptoc_Var47->res_nextkeys.reckeyspec.ks_pos , (int ) ptoc_Var47->res_keylen , &(*(tsp1_segment *) sql__nil(ptoc_Var46->a_return_segm , "vak73.p" , 15103)).integer.C_4.sp1r_returncode);
                    ptoc_Var47->res_nextkeys.reckeyspec.ks_len = (short ) sql__setrang(ptoc_Var47->res_keylen , -32768 , 32767);
                    ptoc_Var47->res_nextpos = ptoc_Var47->res_nextpos - set_result.bd_rec_cnt;
                    }
                mfr_count2 = mfr_count2 - set_result.bd_rec_cnt;
                set_result.bd_next = 0;
                }
            }
        }
    cccprint("<ak73one_before_");
    }



int a73_aclose_resulttable(acv, put_node)
tak_all_command_glob *acv;
tsp_int2 *put_node;
    {
    tsp_int2 curr_n;
    tak_all_command_glob *ptoc_Var50;
    tak_scanner_glob *ptoc_Var51;
    cccprint("AK73  a73_aclose");
    ptoc_Var50 = &*acv;
    ptoc_Var51 = &ptoc_Var50->a_scv;
    (*(tsp1_segment *) sql__nil(ptoc_Var50->a_return_segm , "vak73.p" , 15134)).integer.C_4.sp1r_function_code = 215;
    a01_call_put(&(*acv) , 64 , 10 , &curr_n);
    *put_node = (short ) sql__setrang(curr_n , -32768 , 32767);
    if ((int ) ptoc_Var51->sc_symb != 3)
        {
        a02_aresulttablename(&(*acv) , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var50->a_ap_tree , "vak73.p" , 15139))[sql__setrang((int ) curr_n , 0 , 32767)].n_sa_level , &curr_n);
        }
    a01_is_end_symbol(&(*acv));
    cccprint("<a73_aclose_resu");
    }



int a73_adescribe_resulttable(acv, put_node)
tak_all_command_glob *acv;
tsp_int2 *put_node;
    {
    tsp_int2 curr_n;
    tak_all_command_glob *ptoc_Var52;
    tak_scanner_glob *ptoc_Var53;
    cccprint("AK73  a73_adescr");
    ptoc_Var52 = &*acv;
    ptoc_Var53 = &ptoc_Var52->a_scv;
    a01_call_put(&(*acv) , 64 , 11 , &curr_n);
    *put_node = (short ) sql__setrang(curr_n , -32768 , 32767);
    a01_next_symbol(&(*acv));
    if ((int ) ptoc_Var53->sc_symb != 3)
        {
        a02_aresulttablename(&(*acv) , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var52->a_ap_tree , "vak73.p" , 15167))[sql__setrang((int ) curr_n , 0 , 32767)].n_sa_level , &curr_n);
        }
    else
        {
        if (ptoc_Var52->a_data_length == 12 && (int ) ptoc_Var52->a_ex_kind != 0)
            {
            (*(tak_ap_max_tree *) sql__nil(ptoc_Var52->a_ap_tree , "vak73.p" , 15171))[sql__setrang((int ) *put_node , 0 , 32767)].n_subproc = 15;
            }
        }
    a01_is_end_symbol(&(*acv));
    cccprint("<a73_adescribe_r");
    }



int a73_adparseid(acv, put_node)
tak_all_command_glob *acv;
tsp_int2 *put_node;
    {
    tak_all_command_glob *ptoc_Var54;
    tak_scanner_glob *ptoc_Var55;
    cccprint("AK73  a73_adpars");
    ptoc_Var54 = &*acv;
    ptoc_Var55 = &ptoc_Var54->a_scv;
    (*(tsp1_segment *) sql__nil(ptoc_Var54->a_return_segm , "vak73.p" , 15191)).integer.C_4.sp1r_function_code = 214;
    a01_call_put(&(*acv) , 64 , 12 , &(*put_node));
    a01_next_symbol(&(*acv));
    a01_is_end_symbol(&(*acv));
    cccprint("<a73_adparseid  ");
    }

static
  unsigned char ptoc_Var56[2] = 
  {
  (unsigned char)'\t', (unsigned char)'\0'
  };


int a73_afetch_statement(acv, put_node)
tak_all_command_glob *acv;
tsp_int2 *put_node;
    {
    char res_kw;
    int index;
    tsp_int2 curr_n;
    tsp_int2 last_n;
    tsp_int2 subproc;
    tak_all_command_glob *ptoc_Var57;
    tak_scanner_glob *ptoc_Var58;
    tgg00_TransContext *ptoc_Var59;
    unsigned char ptoc_Var60[2];
    unsigned char ptoc_Var61[2];
    unsigned char ptoc_Var62[2];
    cccprint("AK73  a73_afetch");
    ptoc_Var57 = &*acv;
    ptoc_Var58 = &ptoc_Var57->a_scv;
    if ((char ) sql__setrang((*(tsp1_segment *) sql__nil(ptoc_Var57->a_cmd_segm , "vak73.p" , 15219)).integer.C_3.sp1c_mass_cmd , 0 , 1))
        {
        a01_call_put(&(*acv) , 64 , 2 , &curr_n);
        }
    else
        {
        a01_call_put(&(*acv) , 64 , 1 , &curr_n);
        }
    *put_node = (short ) sql__setrang(curr_n , -32768 , 32767);
    a01_next_symbol(&(*acv));
    if ((int ) ptoc_Var58->sc_symb != 3 && (int ) ptoc_Var57->a_sqlmode == 3)
        {
        a01_get_keyword(&(*acv) , &index , &res_kw);
        }
    else
        {
        index = 0;
        }
    if (index == 352)
        {
        a01_call_put(&(*acv) , 64 , 8 , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var57->a_ap_tree , "vak73.p" , 15236))[sql__setrang((int ) curr_n , 0 , 32767)].n_lo_level);
        (*(tsp1_segment *) sql__nil(ptoc_Var57->a_return_segm , "vak73.p" , 15237)).integer.C_4.sp1r_function_code = 210;
        curr_n = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var57->a_ap_tree , "vak73.p" , 15238))[sql__setrang((int ) curr_n , 0 , 32767)].n_lo_level , -32768 , 32767);
        a01_next_symbol(&(*acv));
        a01_force_symbol(&(*acv) , 7 , &(*put_node) , &(*put_node));
        switch ((int ) ptoc_Var58->sc_symb)
            {
            case 46 :
                a02_s_aparameterspec(&(*acv) , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var57->a_ap_tree , "vak73.p" , 15243))[sql__setrang((int ) curr_n , 0 , 32767)].n_lo_level , &last_n);
                break;
            case 49 :
                a03_anumeric_literal(&(*acv) , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var57->a_ap_tree , "vak73.p" , 15246))[sql__setrang((int ) curr_n , 0 , 32767)].n_lo_level , &last_n);
                break;
            default :
                a07_error(&(*acv) , 6095 , &(*put_node) , &(*put_node));
                break;
            }
        a01_force_symbol(&(*acv) , 13 , &(*put_node) , &(*put_node));
        }
    else
        {
        if (index == 2 || index == 375)
            {
            if (index == 375)
                {
                a01_call_put(&(*acv) , 64 , 17 , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var57->a_ap_tree , "vak73.p" , 15264))[sql__setrang((int ) curr_n , 0 , 32767)].n_lo_level);
                (*(tsp1_segment *) sql__nil(ptoc_Var57->a_return_segm , "vak73.p" , 15265)).integer.C_4.sp1r_function_code = 247;
                }
            else
                {
                a01_call_put(&(*acv) , 64 , 8 , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var57->a_ap_tree , "vak73.p" , 15270))[sql__setrang((int ) curr_n , 0 , 32767)].n_lo_level);
                (*(tsp1_segment *) sql__nil(ptoc_Var57->a_return_segm , "vak73.p" , 15271)).integer.C_4.sp1r_function_code = 210;
                }
            curr_n = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var57->a_ap_tree , "vak73.p" , 15274))[sql__setrang((int ) curr_n , 0 , 32767)].n_lo_level , -32768 , 32767);
            a01_next_symbol(&(*acv));
            switch ((int ) ptoc_Var58->sc_symb)
                {
                case 46 :
                    a02_s_aparameterspec(&(*acv) , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var57->a_ap_tree , "vak73.p" , 15278))[sql__setrang((int ) curr_n , 0 , 32767)].n_lo_level , &last_n);
                    break;
                case 49 :
                    a03_anumeric_literal(&(*acv) , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var57->a_ap_tree , "vak73.p" , 15281))[sql__setrang((int ) curr_n , 0 , 32767)].n_lo_level , &last_n);
                    break;
                case 11 :
                case 10 :
                    a03_ainteger(&(*acv) , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var57->a_ap_tree , "vak73.p" , 15284))[sql__setrang((int ) curr_n , 0 , 32767)].n_lo_level , &last_n);
                    break;
                default :
                    a07_error(&(*acv) , 6095 , &(*put_node) , &(*put_node));
                    break;
                }
            }
        else
            {
            if (index == 170 || index == 292 || index == 235 || index == 356)
                {
                switch (index)
                    {
                    case 170 :
                        subproc = 3;
                        (*(tsp1_segment *) sql__nil(ptoc_Var57->a_return_segm , "vak73.p" , 15301)).integer.C_4.sp1r_function_code = 206;
                        break;
                    case 292 :
                        subproc = 4;
                        (*(tsp1_segment *) sql__nil(ptoc_Var57->a_return_segm , "vak73.p" , 15307)).integer.C_4.sp1r_function_code = 208;
                        break;
                    case 235 :
                        subproc = 5;
                        (*(tsp1_segment *) sql__nil(ptoc_Var57->a_return_segm , "vak73.p" , 15313)).integer.C_4.sp1r_function_code = 207;
                        break;
                    case 356 :
                        subproc = 6;
                        (*(tsp1_segment *) sql__nil(ptoc_Var57->a_return_segm , "vak73.p" , 15319)).integer.C_4.sp1r_function_code = 209;
                        break;
                    default :
                        sql__caseerr("vak73.p" , 15297);
                        break;
                    }
                a01_next_symbol(&(*acv));
                }
            else
                {
                if (index == 401)
                    {
                    subproc = 7;
                    a01_next_symbol(&(*acv));
                    (*(tsp1_segment *) sql__nil(ptoc_Var57->a_return_segm , "vak73.p" , 15332)).integer.C_4.sp1r_function_code = 211;
                    }
                else
                    {
                    subproc = 4;
                    (*(tsp1_segment *) sql__nil(ptoc_Var57->a_return_segm , "vak73.p" , 15338)).integer.C_4.sp1r_function_code = 208;
                    }
                }
            a01_call_put(&(*acv) , 64 , (short ) sql__setrang(subproc , -32768 , 32767) , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var57->a_ap_tree , "vak73.p" , 15343))[sql__setrang((int ) curr_n , 0 , 32767)].n_lo_level);
            curr_n = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var57->a_ap_tree , "vak73.p" , 15344))[sql__setrang((int ) curr_n , 0 , 32767)].n_lo_level , -32768 , 32767);
            }
        }
    if ((int ) ptoc_Var58->sc_symb != 3 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var57->a_return_segm , "vak73.p" , 15348)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) ptoc_Var57->a_sqlmode != 3 && (int ) ptoc_Var57->a_sqlmode != 2 || !a01_eqkey(a01kw[224] , (unsigned char ) sql__setrang(ptoc_Var57->a_sqlmode , 0 , 3) , (*(tsp1_part *) sql__nil(ptoc_Var57->a_cmd_part , "vak73.p" , 15353)).integer.C_1.sp1p_buf , &ptoc_Var57->a_scv))
            {
            a02_aresulttablename(&(*acv) , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var57->a_ap_tree , "vak73.p" , 15356))[sql__setrang((int ) curr_n , 0 , 32767)].n_sa_level , &curr_n);
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var57->a_return_segm , "vak73.p" , 15359)).integer.C_4.sp1r_returncode == 0)
        {
        if (!a01_eqkey(a01kw[224] , (unsigned char ) sql__setrang(ptoc_Var57->a_sqlmode , 0 , 3) , (*(tsp1_part *) sql__nil(ptoc_Var57->a_cmd_part , "vak73.p" , 15363)).integer.C_1.sp1p_buf , &ptoc_Var57->a_scv))
            {
            if ((int ) ptoc_Var57->a_ex_kind != 0)
                {
                if ((int ) ptoc_Var57->a_sqlmode != 3)
                    {
                    a07_error(&(*acv) , 6083 , &(*put_node) , &last_n);
                    }
                else
                    {
                    ptoc_Var59 = &ptoc_Var57->a_transinf.tri_trans;
                    _ptoc_MOVE(unsigned char , 2, ptoc_Var59->trWarning_gg00, *(unsigned char (*)[2]) sql__psetadd(ptoc_Var60 , ptoc_Var59->trWarning_gg00 , ptoc_Var56 , sizeof(unsigned char [2])));
                    }
                }
            }
        else
            {
            a01_next_symbol(&(*acv));
            a01_call_put(&(*acv) , 64 , 9 , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var57->a_ap_tree , "vak73.p" , 15384))[sql__setrang((int ) curr_n , 0 , 32767)].n_sa_level);
            curr_n = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var57->a_ap_tree , "vak73.p" , 15385))[sql__setrang((int ) curr_n , 0 , 32767)].n_sa_level , -32768 , 32767);
            a02_l_aparameter_list(&(*acv) , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var57->a_ap_tree , "vak73.p" , 15386))[sql__setrang((int ) curr_n , 0 , 32767)].n_lo_level , &last_n);
            }
        }
    if ((char ) sql__setrang((*(tsp1_segment *) sql__nil(ptoc_Var57->a_cmd_segm , "vak73.p" , 15392)).integer.C_3.sp1c_mass_cmd , 0 , 1))
        {
        (*(tsp1_segment *) sql__nil(ptoc_Var57->a_return_segm , "vak73.p" , 15394)).integer.C_4.sp1r_function_code = (short ) sql__setrang((short ) ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var57->a_return_segm , "vak73.p" , 15395)).integer.C_4.sp1r_function_code + 1000) , -32768 , 32767);
        }
    a01_is_end_symbol(&(*acv));
    cccprint("<a73_afetch_stat");
    }



int a73_anext_statement(acv, put_node)
tak_all_command_glob *acv;
tsp_int2 *put_node;
    {
    tsp_int2 last_node;
    tak_all_command_glob *ptoc_Var63;
    tak_scanner_glob *ptoc_Var64;
    cccprint("AK73  a73_anext_");
    ptoc_Var63 = &*acv;
    ptoc_Var64 = &ptoc_Var63->a_scv;
    a01_next_symbol(&(*acv));
    if ((int ) ptoc_Var63->a_ex_kind != 0 && (int ) ptoc_Var63->a_ex_kind != 4 || (int ) ptoc_Var63->a_sqlmode != 3)
        {
        a07_b_put_error(&(*acv) , 6292 , 1);
        }
    else
        {
        if (a01_eqkey(a01kw[433] , (unsigned char ) sql__setrang(ptoc_Var63->a_sqlmode , 0 , 3) , (*(tsp1_part *) sql__nil(ptoc_Var63->a_cmd_part , "vak73.p" , 15425)).integer.C_1.sp1p_buf , &ptoc_Var63->a_scv))
            {
            (*(tsp1_segment *) sql__nil(ptoc_Var63->a_return_segm , "vak73.p" , 15428)).integer.C_4.sp1r_function_code = 239;
            a01_call_put(&(*acv) , 64 , 13 , &(*put_node));
            a01_next_symbol(&(*acv));
            if (a01_eqkey(a01kw[176] , (unsigned char ) sql__setrang(ptoc_Var63->a_sqlmode , 0 , 3) , (*(tsp1_part *) sql__nil(ptoc_Var63->a_cmd_part , "vak73.p" , 15432)).integer.C_1.sp1p_buf , &ptoc_Var63->a_scv))
                {
                a01_next_symbol(&(*acv));
                a02_atablename(&(*acv) , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var63->a_ap_tree , "vak73.p" , 15436))[sql__setrang((int ) *put_node , 0 , 32767)].n_lo_level , &last_node);
                }
            if ((int ) ptoc_Var64->sc_symb == 46)
                {
                a01_next_symbol(&(*acv));
                }
            else
                {
                if (a01mandatory_keyword(&(*acv) , 225))
                    {
                    if ((int ) ptoc_Var64->sc_symb == 46)
                        {
                        a01_next_symbol(&(*acv));
                        }
                    else
                        {
                        a07_error(&(*acv) , 6070 , &(*put_node) , &(*put_node));
                        }
                    }
                }
            }
        else
            {
            a07_error(&(*acv) , 6083 , &(*put_node) , &(*put_node));
            }
        }
    a01_is_end_symbol(&(*acv));
    cccprint("<a73_anext_state");
    }



int a73_call_semantic(acv)
tak_all_command_glob *acv;
    {
    cccprint("AK73  a73_call_s");
    switch ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 15479))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak73.p" , 15479))[0].n_lo_level , 0 , 32767)].n_subproc)
        {
        case 10 :
            ak73close_semantic(&(*acv));
            break;
        case 11 :
            ak73describe_semantic(&(*acv));
            break;
        case 15 :
            ak73describe_parsid_semantic(&(*acv));
            break;
        case 12 :
            ak73drop_parseid(&(*acv));
            break;
        case 13 :
            ak73next_stamp(&(*acv));
            break;
        default :
            ak73fetch_semantic(&(*acv));
            break;
        }
    cccprint("<a73_call_semant");
    }



int a73_close_execute(acv, sparr, res_name, modul_name)
tak_all_command_glob *acv;
tak_syspointerarr *sparr;
tsp_knl_identifier res_name;
tsp_knl_identifier modul_name;
    {
    tgg_basis_error mfr_b_err;
    char f_ok;
    tgg_datetimeformat hpfiller;
    int i;
    tsp00_Uint1 hpinfobyte;
    tak_parskey hparsk;
    tak_parsid parsid;
    tak_parsid dummy_parsid;
    tsp_c12 dummy_c12;
    tak_all_command_glob *ptoc_Var65;
    tak_resname_record *ptoc_Var66;
    tak_syspointerarr *ptoc_Var67;
    tak_parsinforecord *ptoc_Var68;
    cccprint("AK73  a73_close_");
    ptoc_Var65 = &*acv;
    a663_get_result_info(&(*acv) , res_name , modul_name , &ptoc_Var65->a_resname_addr[2] , 0 , 0 , 1 , &f_ok);
    if ((char ) sql__setrang(f_ok , 0 , 1))
        {
        t01lidentifier(5 , res_name);
        ptoc_Var66 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var65->a_resname_addr[2] , "vak73.p" , 15526)).integer.C_52.sresname;
        hparsk = ptoc_Var66->reseparsk;
        hpinfobyte = (unsigned char ) sql__setrang(ptoc_Var66->resparsinfobyte , 0 , 255);
        hpfiller = (unsigned char ) sql__setrang(ptoc_Var66->resdatetimeformat , 0 , 9);
        f_ok = (char ) sql__setrang((char ) sql__setrang(ptoc_Var66->resexecuted , 0 , 1) || (int ) ptoc_Var65->a_ex_kind == 0 , 0 , 1);
        if ((int ) ptoc_Var65->a_ex_kind != 0 || !(char ) sql__setrang(ptoc_Var66->resexecuted , 0 , 1))
            {
            a663_del_result(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var65->a_resname_addr[2] , "vak73.p" , 15537)).integer.C_52.sresname , 1 , 1);
            }
        }
    else
        {
        hparsk.p_kind = 0;
        }
    if (!(char ) sql__setrang(f_ok , 0 , 1) && (int ) ptoc_Var65->a_sqlmode != 2)
        {
        if ((char ) sql__setrang(g01unicode , 0 , 1))
            {
            a07_b_put_error(&(*acv) , 6516 , 13);
            }
        else
            {
            a07_b_put_error(&(*acv) , 6516 , 7);
            }
        }
    if ((int ) ptoc_Var65->a_ex_kind == 1)
        {
        if ((int ) hparsk.p_kind != 0)
            {
            parsid.pid_session = ptoc_Var65->a_transinf.tri_trans.trSessionId_gg00;
            parsid.boolean.C_true.pid_parsk = hparsk;
            parsid.boolean.C_false.pid_appl_info[0] = (unsigned char ) sql__setrang((unsigned char ) hpinfobyte , 0 , 255);
            parsid.boolean.C_false.pid_dtime_info[0] = (unsigned char ) sql__setrang((unsigned char ) sql__ord((int ) hpfiller) , 0 , 255);
            a06retpart_move(&(*acv) , (tsp00_MoveObj *) &parsid , sizeof(tak_parsid ));
            }
        else
            {
            i = 1;
            do
                {
                dummy_c12[sql__setrang(i , 1 , 12) + -1] = (unsigned char)'\0';
                if (i == 12)
                    {
                    break;
                    }
                i += 1;
                }
            while (!0);
            a06retpart_move(&(*acv) , (tsp00_MoveObj *) dummy_c12 , sizeof(tak_parsid ));
            }
        a06finish_curr_retpart(&(*acv) , 11 , 1);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var65->a_return_segm , "vak73.p" , 15582)).integer.C_4.sp1r_returncode == 0 && (int ) ptoc_Var65->a_ex_kind == 0)
        {
        ptoc_Var67 = &*sparr;
        a54_get_pparsp_pinfop(&(*acv) , &(*sparr) , 6);
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var65->a_return_segm , "vak73.p" , 15588)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var65->a_precomp_info_byte = (unsigned char)'(';
            ptoc_Var68 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var67->pparsp , "vak73.p" , 15592)).integer.C_23.sparsinfo;
            ptoc_Var68->integer.C_2.p_mtyp = 6;
            ptoc_Var68->integer.C_2.p_select = 0;
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var68->integer.C_2.p_resn, res_name)
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var68->integer.C_2.p_modul_name, modul_name)
            ptoc_Var68->integer.C_2.p_statement_kind = 86;
            ptoc_Var68->integer.C_2.p_const_param_expr = 0;
            ptoc_Var68->integer.C_2.p_function_code = (short ) sql__setrang((*(tsp1_segment *) sql__nil(ptoc_Var65->a_return_segm , "vak73.p" , 15600)).integer.C_4.sp1r_function_code , -32768 , 32767);
            ptoc_Var68->integer.C_2.p_prepare = 0;
            ptoc_Var68->integer.C_2.p_session_isolevel = (unsigned char ) sql__setrang((*acv).a_iso_level , 0 , 255);
            ptoc_Var68->integer.C_2.p_use_sess_isolevel = 0;
            (*(struct tak_systembuffer *) sql__nil(ptoc_Var67->pparsp , "vak73.p" , 15606)).integer.C_0.b_sl = 316;
            (*(struct tak_systembuffer *) sql__nil(ptoc_Var67->pparsp , "vak73.p" , 15607)).integer.C_23.sparsinfo.integer.C_1.p_fullen = (int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var67->pparsp , "vak73.p" , 15607)).integer.C_0.b_sl;
            (*(struct tak_systembuffer *) sql__nil(ptoc_Var67->pparsp , "vak73.p" , 15608)).integer.C_0.b_kl = 12;
            a10add_sysinfo(&(*acv) , &ptoc_Var67->pparsp , &mfr_b_err);
            if ((int ) mfr_b_err != 0)
                {
                a07_b_put_error(&(*acv) , (short ) sql__setrang(mfr_b_err , -32768 , 32767) , 1);
                }
            else
                {
                parsid.pid_session = ptoc_Var65->a_transinf.tri_trans.trSessionId_gg00;
                parsid.boolean.C_true.pid_parsk = ptoc_Var65->a_pars_last_key;
                parsid.boolean.C_false.pid_appl_info[0] = (unsigned char ) sql__setrang((unsigned char ) ptoc_Var65->a_precomp_info_byte , 0 , 255);
                parsid.boolean.C_false.pid_dtime_info[0] = (unsigned char ) sql__setrang((unsigned char ) sql__ord(0) , 0 , 255);
                if ((char ) sql__setrang(g01diag_moni_parse_on , 0 , 1))
                    {
                    _ptoc_ASSIGN(unsigned char , 4, dummy_parsid.pid_session.ci4_gg00, "\377\377\377\377")
                    a545diag_parse_info(&(*acv) , &parsid , &dummy_parsid);
                    }
                a54return_parsid(&(*acv) , &parsid.boolean.C_true.pid_parsk);
                a06init_curr_retpart(&(*acv));
                a06finish_curr_retpart(&(*acv) , 14 , 0);
                }
            }
        }
    cccprint("<a73_close_execu");
    }



int a73_describe_execute(acv, sparr, res_name, modul_name)
tak_all_command_glob *acv;
tak_syspointerarr *sparr;
tsp_knl_identifier res_name;
tsp_knl_identifier modul_name;
    {
    tgg_basis_error mfr_b_err;
    char f_ok;
    tak_parsid parsid;
    tak_parsid dummy_parsid;
    tak_dml_info dmli;
    tak_all_command_glob *ptoc_Var69;
    tak_dml_info *ptoc_Var70;
    tak_syspointerarr *ptoc_Var71;
    tak_parsinforecord *ptoc_Var72;
    cccprint("AK73  a73_descri");
    ptoc_Var69 = &*acv;
    ptoc_Var70 = &dmli;
    a54_dml_init(&dmli , 0);
    a663_get_result_info(&(*acv) , res_name , modul_name , &ptoc_Var69->a_resname_addr[0] , 0 , 0 , 1 , &f_ok);
    if ((char ) sql__setrang(f_ok , 0 , 1))
        {
        ptoc_Var70->d_pos_result = 1;
        ptoc_Var70->d_global_pos_result = 1;
        t01lidentifier(5 , res_name);
        t01int4(5 , "POS_ERG     " , (int ) ptoc_Var70->d_pos_result);
        if ((int ) ptoc_Var69->a_ex_kind != 0)
            {
            a73_ex_describe(&(*acv) , &dmli , 2 , 0);
            }
        }
    else
        {
        if ((char ) sql__setrang(g01unicode , 0 , 1))
            {
            a07_b_put_error(&(*acv) , 6516 , 19);
            }
        else
            {
            a07_b_put_error(&(*acv) , 6516 , 10);
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var69->a_return_segm , "vak73.p" , 15686)).integer.C_4.sp1r_returncode == 0 && (int ) ptoc_Var69->a_ex_kind == 0)
        {
        ptoc_Var71 = &*sparr;
        a54_get_pparsp_pinfop(&(*acv) , &(*sparr) , 20);
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var69->a_return_segm , "vak73.p" , 15692)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var69->a_precomp_info_byte = (unsigned char)')';
            ptoc_Var72 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var71->pparsp , "vak73.p" , 15696)).integer.C_23.sparsinfo;
            ptoc_Var72->integer.C_2.p_mtyp = 20;
            ptoc_Var72->integer.C_2.p_select = 0;
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var72->integer.C_2.p_resn, res_name)
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var72->integer.C_2.p_modul_name, modul_name)
            ptoc_Var72->integer.C_2.p_prepare = 0;
            ptoc_Var72->integer.C_2.p_session_isolevel = (unsigned char ) sql__setrang((*acv).a_iso_level , 0 , 255);
            ptoc_Var72->integer.C_2.p_use_sess_isolevel = 0;
            (*(struct tak_systembuffer *) sql__nil(ptoc_Var71->pparsp , "vak73.p" , 15707)).integer.C_0.b_sl = 316;
            (*(struct tak_systembuffer *) sql__nil(ptoc_Var71->pparsp , "vak73.p" , 15708)).integer.C_23.sparsinfo.integer.C_1.p_fullen = (int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var71->pparsp , "vak73.p" , 15708)).integer.C_0.b_sl;
            (*(struct tak_systembuffer *) sql__nil(ptoc_Var71->pparsp , "vak73.p" , 15709)).integer.C_0.b_kl = 12;
            a10add_sysinfo(&(*acv) , &ptoc_Var71->pparsp , &mfr_b_err);
            if ((int ) mfr_b_err != 0)
                {
                a07_b_put_error(&(*acv) , (short ) sql__setrang(mfr_b_err , -32768 , 32767) , 1);
                }
            else
                {
                parsid.pid_session = ptoc_Var69->a_transinf.tri_trans.trSessionId_gg00;
                parsid.boolean.C_true.pid_parsk = ptoc_Var69->a_pars_last_key;
                parsid.boolean.C_false.pid_appl_info[0] = (unsigned char ) sql__setrang((unsigned char ) ptoc_Var69->a_precomp_info_byte , 0 , 255);
                parsid.boolean.C_false.pid_dtime_info[0] = (unsigned char ) sql__setrang((unsigned char ) sql__ord(0) , 0 , 255);
                if ((char ) sql__setrang(g01diag_moni_parse_on , 0 , 1))
                    {
                    _ptoc_ASSIGN(unsigned char , 4, dummy_parsid.pid_session.ci4_gg00, "\377\377\377\377")
                    a545diag_parse_info(&(*acv) , &parsid , &dummy_parsid);
                    }
                a54return_parsid(&(*acv) , &parsid.boolean.C_true.pid_parsk);
                }
            }
        }
    ptoc_Var69->a_resname_addr[0] = (struct tak_systembuffer *) (int *) 0;
    cccprint("<a73_describe_ex");
    }



int a73_ex_describe(acv, dmli, start_fieldno, subquery)
tak_all_command_glob *acv;
tak_dml_info *dmli;
int start_fieldno;
char subquery;
    {
    tgg_basis_error mfr_b_err;
    char done;
    char f_ok;
    int fieldno;
    int startpos;
    tsp_int4 res_id;
    tak00_colinfo_ptr col_ptr;
    tak_all_command_glob *ptoc_Var73;
    tak_dml_info *ptoc_Var74;
    tak_resname_record *ptoc_Var75;
    tak_dml_info *ptoc_Var76;
    tak_resname_record *ptoc_Var77;
    tak_resultrecord *ptoc_Var78;
    tak_resname_record *ptoc_Var79;
    struct tak_systembuffer *ptoc_Var80;
    cccprint("AK73  a73_ex_des");
    ptoc_Var73 = &*acv;
    ptoc_Var74 = &*dmli;
    t01int4(5 , "d_global_pos" , (int ) ptoc_Var74->d_global_pos_result);
    if ((char ) sql__setrang((*acv).a_subq_datatype_problem , 0 , 1))
        {
        done = 1;
        }
    else
        {
        done = 0;
        }
    if ((int ) ptoc_Var74->d_global_pos_result <= 2)
        {
        ptoc_Var75 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var73->a_resname_addr[sql__setrang((int ) ptoc_Var74->d_global_pos_result , 1 , 3) + -1] , "vak73.p" , 15774)).integer.C_52.sresname;
        if ((int ) ptoc_Var75->resinfobuf > 0)
            {
            ak73get_longinfo(&(*acv) , &ptoc_Var74->d_sparr , &ptoc_Var75->restreeid.integer.C_8.fileTempCnt_gg00);
            done = 1;
            }
        }
    if (!(char ) sql__setrang(done , 0 , 1))
        {
        ptoc_Var76 = &*dmli;
        ptoc_Var76->d_single = 0;
        ptoc_Var76->d_keylen = -8;
        _ptoc_ASSIGN(unsigned char , 64, ptoc_Var76->d_refname, a01_il_b_identifier)
        startpos = 0;
        ptoc_Var76->d_sparr.pinfop = (struct tak_systembuffer *) (int *) 0;
        ptoc_Var77 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var73->a_resname_addr[sql__setrang((int ) ptoc_Var76->d_global_pos_result , 1 , 3) + -1] , "vak73.p" , 15793)).integer.C_52.sresname;
        if ((int ) ptoc_Var76->d_global_pos_result <= 2)
            {
            if ((int ) ptoc_Var76->d_global_pos_result == 2)
                {
                res_id = 2;
                }
            else
                {
                res_id = ptoc_Var77->restreeid.integer.C_8.fileTempCnt_gg00;
                }
            }
        else
            {
            res_id = 1;
            }
        a663get_result_records(&(*acv) , 0 , &res_id , &ptoc_Var76->d_sparr , &f_ok);
        if (!(char ) sql__setrang(f_ok , 0 , 1))
            {
            a07_b_put_error(&(*acv) , 6516 , 1);
            }
        else
            {
            ptoc_Var78 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var76->d_sparr.pbasep , "vak73.p" , 15816)).integer.C_28.sresult;
            ptoc_Var79 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var73->a_resname_addr[sql__setrang((int ) ptoc_Var76->d_global_pos_result , 1 , 3) + -1] , "vak73.p" , 15818)).integer.C_52.sresname;
            if ((int ) ptoc_Var76->d_global_pos_result <= 2)
                {
                a60_get_longinfobuffer(&(*acv) , &ptoc_Var76->d_sparr , (int ) ptoc_Var78->boutcolno - start_fieldno + 1 , ptoc_Var79->restreeid.integer.C_8.fileTempCnt_gg00);
                }
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var73->a_return_segm , "vak73.p" , 15826)).integer.C_4.sp1r_returncode == 0)
                {
                fieldno = start_fieldno;
                while (fieldno <= (int ) ptoc_Var78->boutcolno)
                    {
                    a06extcolno(&(*(struct tak_systembuffer *) sql__nil(ptoc_Var76->d_sparr.pbasep , "vak73.p" , 15832)).integer.C_1.sbase , fieldno , &col_ptr);
                    ptoc_Var76->d_inoutpos = (short ) sql__setrang((short ) (startpos + (int ) (*(tak00_columninfo *) sql__nil(col_ptr , "vak73.p" , 15835)).integer.C_1.ccolstack.integer.C_1.epos) , -32768 , 32767);
                    a61_p_long_info(&(*acv) , &(*dmli) , sql__nil(col_ptr , "vak73.p" , 15836));
                    fieldno = sql__succ(fieldno , -2147483647 , 2147483647 , "vak73.p" , 15837);
                    }
                a10_rel_sysinfo(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var76->d_sparr.pbasep , "vak73.p" , 15840)).integer.C_50.syskey);
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var73->a_return_segm , "vak73.p" , 15841)).integer.C_4.sp1r_returncode == 0)
                    {
                    a60_p_info_output(&(*acv) , &ptoc_Var76->d_sparr);
                    }
                if (ptoc_Var76->d_sparr.pcolnamep != (struct tak_systembuffer *) (int *) 0)
                    {
                    if ((char ) sql__setrang(subquery , 0 , 1))
                        {
                        a10add_sysinfo(&(*acv) , &ptoc_Var76->d_sparr.pcolnamep , &mfr_b_err);
                        if ((int ) mfr_b_err != 0)
                            {
                            a07_b_put_error(&(*acv) , (short ) sql__setrang(mfr_b_err , -32768 , 32767) , 1);
                            }
                        }
                    else
                        {
                        a10_key_del(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var76->d_sparr.pcolnamep , "vak73.p" , 15859)).integer.C_50.syskey);
                        ptoc_Var76->d_sparr.pcolnamep = (struct tak_systembuffer *) (int *) 0;
                        }
                    }
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var73->a_return_segm , "vak73.p" , 15864)).integer.C_4.sp1r_returncode == 0)
                    {
                    if ((char ) sql__setrang(subquery , 0 , 1))
                        {
                        ptoc_Var80 = (struct tak_systembuffer *) sql__nil(ptoc_Var76->d_sparr.pinfop , "vak73.p" , 15869);
                        ptoc_Var80->integer.C_0.b_sl = (short ) sql__setrang((short ) (sizeof(tak_shortinforecord ) - sizeof(tak_paraminfoarr ) + (int ) ptoc_Var80->integer.C_31.sshortinfo.sicount * sizeof(tsp1_param_info )) , -32768 , 32767);
                        a10add_sysinfo(&(*acv) , &ptoc_Var76->d_sparr.pinfop , &mfr_b_err);
                        if ((int ) mfr_b_err != 0)
                            {
                            a07_b_put_error(&(*acv) , (short ) sql__setrang(mfr_b_err , -32768 , 32767) , 1);
                            }
                        }
                    else
                        {
                        a10_key_del(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var76->d_sparr.pinfop , "vak73.p" , 15886)).integer.C_50.syskey);
                        ptoc_Var76->d_sparr.pinfop = (struct tak_systembuffer *) (int *) 0;
                        }
                    }
                }
            }
        }
    cccprint("<a73_ex_describe");
    }



int a73_infolen_get(acv, no_of_out_cols, output_len)
tak_all_command_glob *acv;
int no_of_out_cols;
tsp_int2 *output_len;
    {
    char f_ok;
    tak_syspointerarr isparr;
    int k;
    tak00_colinfo_ptr col_ptr;
    tak_all_command_glob *ptoc_Var81;
    tak_baserecord *ptoc_Var82;
    int ptoc_Var83;
    tak00_columninfo *ptoc_Var84;
    cccprint("AK73  a73_infole");
    ptoc_Var81 = &*acv;
    a663restable_get(&(*acv) , 1 , (*(struct tak_systembuffer *) sql__nil(ptoc_Var81->a_resname_addr[0] , "vak73.p" , 15923)).integer.C_52.sresname.reskey_name , &isparr , &f_ok);
    if ((char ) sql__setrang(f_ok , 0 , 1))
        {
        ptoc_Var81->a_output_data_pos = 1;
        ptoc_Var82 = &(*(struct tak_systembuffer *) sql__nil(isparr.pbasep , "vak73.p" , 15929)).integer.C_1.sbase;
        col_ptr = ptoc_Var82->integer.C_2.bcolumn[sql__setrang((int ) ptoc_Var82->bfirstcolind , 1 , 1536) + -1];
        ptoc_Var83 = no_of_out_cols + 1;
        if (2 <= ptoc_Var83)
            {
            k = 2;
            do
                {
                col_ptr = (*(struct tak_systembuffer *) sql__nil(isparr.pbasep , "vak73.p" , 15934)).integer.C_1.sbase.integer.C_2.bcolumn[sql__setrang((int ) (*(tak00_columninfo *) sql__nil(col_ptr , "vak73.p" , 15934)).integer.C_1.cnextind , 1 , 1536) + -1];
                ptoc_Var81->a_output_data_pos = (int ) (*(tak00_columninfo *) sql__nil(col_ptr , "vak73.p" , 15935)).integer.C_1.ccolstack.integer.C_1.epos;
                if (k == ptoc_Var83)
                    {
                    break;
                    }
                k += 1;
                }
            while (!0);
            }
        ptoc_Var84 = (tak00_columninfo *) sql__nil(col_ptr , "vak73.p" , 15938);
        *output_len = (short ) sql__setrang((short ) ((int ) ptoc_Var84->integer.C_1.ccolstack.integer.C_1.epos + (int ) ptoc_Var84->integer.C_1.cinoutlen + -1) , -32768 , 32767);
        }
    cccprint("<a73_infolen_get");
    }

static
  tak_rollback_info ptoc_Var85 = 
  {
  (unsigned char)'\003'
  };
static
  tsp1_part_attr ptoc_Var86 = 
  {
  (unsigned char)'\001'
  };


int a73_more_fetch(acv, sparr, mtype, pos, mfetch_count)
tak_all_command_glob *acv;
tak_syspointerarr *sparr;
tgg_message2_type mtype;
tsp_int4 pos;
int mfetch_count;
    {
    tgg_basis_error mfr_b_err;
    tgg_bd_set_result_record set_result;
    char firstres;
    char not_null;
    int col_no;
    int i;
    tsp_int2 start;
    tgg00_FilePos tree_pos;
    mfetchrec mfr;
    char lcol_found;
    char lcol_lock;
    tgg_lkey bd_lkey;
    int unused_leng;
    tsp_int_map_c2 ic2;
    tak_all_command_glob *ptoc_Var87;
    tak_syspointerarr *ptoc_Var88;
    mfetchrec *ptoc_Var89;
    tak_resultkeyrecord *ptoc_Var90;
    struct tgg00_MessBlock *ptoc_Var91;
    tgg00_BdSetResultRecord *ptoc_Var92;
    tak_keyspec *ptoc_Var93;
    tak_keyspec *ptoc_Var94;
    int ptoc_Var95;
    struct tgg00_MessBlock *ptoc_Var96;
    tgg00_DataPart *ptoc_Var97;
    struct tgg00_MessBlock *ptoc_Var98;
    tgg00_DataPart *ptoc_Var99;
    struct tgg00_MessBlock *ptoc_Var100;
    tgg00_DataPart *ptoc_Var101;
    cccprint("AK73  a73_more_f");
    ptoc_Var87 = &*acv;
    ptoc_Var88 = &*sparr;
    ptoc_Var89 = &mfr;
    lcol_found = 0;
    a09activate_lock(&ptoc_Var87->a_transinf.tri_trans , ptoc_Var87->a_work_st_addr , (short ) sql__setrang(ptoc_Var87->a_work_st_max , -32768 , 32767) , &ptoc_Var89->mfr_b_err);
    if ((int ) ptoc_Var89->mfr_b_err == 4740)
        {
        _ptoc_MOVE(unsigned char , 1, ptoc_Var87->a_rollback_info, ptoc_Var85);
        ptoc_Var89->mfr_b_err = 4900;
        }
    if ((int ) ptoc_Var89->mfr_b_err != 0)
        {
        a07_b_put_error(&(*acv) , (short ) sql__setrang(ptoc_Var89->mfr_b_err , -32768 , 32767) , 1);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 15989)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var90 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var88->px[1] , "vak73.p" , 15992)).integer.C_26.sreskey;
        ptoc_Var91 = &ptoc_Var87->a_mblock;
        a60rescount(&(*acv) , 0);
        if (ptoc_Var87->a_curr_retpart == (tsp1_part *) (int *) 0)
            {
            a06init_curr_retpart(&(*acv));
            }
        if ((int ) ptoc_Var90->boolean.C_true.res_change.cr_colcount > 0)
            {
            lcol_found = (char ) sql__setrang(a508_lcol_found(&(*acv) , &ptoc_Var90->boolean.C_true.res_change) , 0 , 1);
            }
        t01int4(19 , "res_nextpos " , ptoc_Var90->res_nextpos);
        t01int4(19 , "res_prevpos " , ptoc_Var90->res_prevpos);
        ptoc_Var89->mfr_b_err = 0;
        ptoc_Var90->res_useupdk = (unsigned char)'\0';
        unused_leng = a73_calc_unused_space(&(*acv));
        if ((int ) ptoc_Var87->a_init_ex_kind == 1)
            {
            if ((int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var88->px[0] , "vak73.p" , 16011)).integer.C_23.sparsinfo.integer.C_2.p_resinfolen < 0)
                {
                col_no = -(int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var88->px[0] , "vak73.p" , 16014)).integer.C_23.sparsinfo.integer.C_2.p_resinfolen;
                if (col_no > (int ) ptoc_Var90->res_outcolno)
                    {
                    col_no = (int ) ptoc_Var90->res_outcolno;
                    }
                a73_infolen_get(&(*acv) , col_no , &ptoc_Var89->mfr_reclength);
                }
            else
                {
                ptoc_Var89->mfr_reclength = (short ) sql__setrang((*(struct tak_systembuffer *) sql__nil(ptoc_Var88->px[0] , "vak73.p" , 16022)).integer.C_23.sparsinfo.integer.C_2.p_resinfolen , -32768 , 32767);
                }
            }
        else
            {
            ptoc_Var89->mfr_reclength = (short ) sql__setrang((short ) ((int ) ptoc_Var90->res_length - (int ) ptoc_Var90->res_keylen + -8) , -32768 , 32767);
            }
        if (unused_leng < (int ) ptoc_Var89->mfr_reclength)
            {
            ptoc_Var89->mfr_count1 = 1;
            }
        else
            {
            ptoc_Var89->mfr_count1 = unused_leng / (int ) ptoc_Var89->mfr_reclength;
            }
        if (ptoc_Var89->mfr_count1 > mfetch_count)
            {
            ptoc_Var89->mfr_count1 = mfetch_count;
            }
        ptoc_Var92 = &set_result;
        ptoc_Var92->bd_key_check_len = 0;
        ptoc_Var92->bd_max_rec_cnt = 1;
        ptoc_Var92->bd_max_fill_len = ptoc_Var87->a_mblock.integer.C_2.mb_data_size;
        ptoc_Var92->bd_next = 1;
        t01int4(5 , "mtype       " , sql__ord((int ) mtype));
        t01int4(5 , "pos         " , pos);
        t01int4(5 , "res_nextpos " , ptoc_Var90->res_nextpos);
        t01int4(5 , "res_prevpos " , ptoc_Var90->res_prevpos);
        if ((int ) mtype == 59)
            {
            if ((char ) sql__setrang(ptoc_Var90->res_eof , 0 , 1) && pos >= 0 || (char ) sql__setrang(ptoc_Var90->res_first_fetch , 0 , 1) && pos <= 0)
                {
                a07_b_put_error(&(*acv) , 6408 , 1);
                }
            else
                {
                if (pos >= 0)
                    {
                    if (!(char ) sql__setrang(ptoc_Var90->res_first_fetch , 0 , 1))
                        {
                        pos = ptoc_Var90->res_nextpos + pos;
                        }
                    }
                else
                    {
                    if ((char ) sql__setrang(ptoc_Var90->res_eof , 0 , 1))
                        {
                        pos = ptoc_Var90->res_actres + pos + 1;
                        }
                    else
                        {
                        pos = ptoc_Var90->res_prevpos + pos;
                        }
                    }
                if (pos <= 0)
                    {
                    ptoc_Var90->res_first_fetch = 1;
                    ptoc_Var90->res_eof = 0;
                    a07_b_put_error(&(*acv) , 6408 , 1);
                    }
                else
                    {
                    mtype = 16;
                    }
                }
            }
        else
            {
            if ((int ) mtype == 58)
                {
                if ((char ) sql__setrang(ptoc_Var90->res_first_fetch , 0 , 1) || (char ) sql__setrang(ptoc_Var90->res_eof , 0 , 1))
                    {
                    a07_b_put_error(&(*acv) , 6232 , 1);
                    }
                else
                    {
                    if (ptoc_Var90->res_prevpos > 0)
                        {
                        mtype = 16;
                        pos = ptoc_Var90->res_prevpos;
                        }
                    }
                }
            }
        if ((int ) mtype == 16)
            {
            if (pos < 0)
                {
                pos = ptoc_Var90->res_actres + pos + 1;
                }
            if (pos > ptoc_Var90->res_actres)
                {
                ptoc_Var90->res_first_fetch = 0;
                ptoc_Var90->res_eof = 1;
                a07_b_put_error(&(*acv) , 6408 , 1);
                }
            else
                {
                if (pos <= 0)
                    {
                    ptoc_Var90->res_first_fetch = 1;
                    ptoc_Var90->res_eof = 0;
                    a07_b_put_error(&(*acv) , 6408 , 1);
                    }
                else
                    {
                    if ((char ) sql__setrang(ptoc_Var90->res_order , 0 , 1))
                        {
                        ak73one_before_pos(&(*acv) , &(*sparr) , pos);
                        if (pos == ptoc_Var90->res_nextpos)
                            {
                            not_null = 0;
                            ptoc_Var93 = &ptoc_Var90->res_nextkeys.reckeyspec;
                            do
                                if ((int ) ptoc_Var90->boolean.C_false.res_keysbuf[sql__setrang((int ) ptoc_Var93->ks_pos + (int ) ptoc_Var93->ks_len + -1 , 1 , 13316) + -1] == 0)
                                    {
                                    ptoc_Var93->ks_len = (short ) sql__setrang((short ) sql__pred((int ) ptoc_Var93->ks_len , -32768 , 32767 , "vak73.p" , 16142) , -32768 , 32767);
                                    }
                                else
                                    {
                                    not_null = 1;
                                    }
                            while (!((int ) ptoc_Var93->ks_len == 0 || (char ) sql__setrang(not_null , 0 , 1)));
                            if ((int ) ptoc_Var93->ks_len > 0)
                                {
                                ptoc_Var90->boolean.C_false.res_keysbuf[sql__setrang((int ) ptoc_Var93->ks_pos + (int ) ptoc_Var93->ks_len + -1 , 1 , 13316) + -1] = (unsigned char ) sql__setrang((unsigned char ) sql__pred(sql__ord((int ) ptoc_Var90->boolean.C_false.res_keysbuf[sql__setrang((int ) ptoc_Var93->ks_pos + (int ) ptoc_Var93->ks_len + -1 , 1 , 13316) + -1]) , -2147483647 , 2147483647 , "vak73.p" , 16153) , 0 , 255);
                                g10fil("VAK73 " , 10 , sizeof(tak_res_keysbuf ) , ptoc_Var90->boolean.C_false.res_keysbuf , (int ) ptoc_Var93->ks_pos + (int ) ptoc_Var93->ks_len , (int ) ptoc_Var90->res_keylen - (int ) ptoc_Var93->ks_len , (unsigned char)'\377' , &(*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16158)).integer.C_4.sp1r_returncode);
                                ptoc_Var93->ks_len = (short ) sql__setrang(ptoc_Var90->res_keylen , -32768 , 32767);
                                }
                            ptoc_Var90->res_nextpos = sql__pred(ptoc_Var90->res_nextpos , -2147483647 , 2147483647 , "vak73.p" , 16164);
                            }
                        mtype = 41;
                        }
                    else
                        {
                        s20ch4sw(pos + -1 , (unsigned char ) sql__setrang(g01code.kernel_swap , 0 , 3) , ptoc_Var90->boolean.C_false.res_keysbuf , (int ) ptoc_Var90->res_nextkeys.reckeyspec.ks_pos , 1);
                        ptoc_Var90->res_nextkeys.reckeyspec.ks_len = (short ) sql__setrang(ptoc_Var90->res_keylen , -32768 , 32767);
                        ptoc_Var90->res_nextpos = pos + -1;
                        mtype = 41;
                        }
                    }
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16184)).integer.C_4.sp1r_returncode == 0)
            {
            if ((int ) mtype == 23 || (int ) mtype == 23 && (char ) sql__setrang(ptoc_Var90->res_first_fetch , 0 , 1))
                {
                ptoc_Var90->res_nextpos = 0;
                ptoc_Var90->res_nextkeys.reckeyspec.ks_len = 0;
                }
            else
                {
                if ((int ) mtype == 32 || (int ) mtype == 53 && (char ) sql__setrang(ptoc_Var90->res_eof , 0 , 1) || (int ) mtype == 41 && (char ) sql__setrang(ptoc_Var90->res_eof , 0 , 1))
                    {
                    ptoc_Var94 = &ptoc_Var90->res_prevkeys.reckeyspec;
                    g10fil("VAK73 " , 11 , sizeof(tak_res_keysbuf ) , ptoc_Var90->boolean.C_false.res_keysbuf , (int ) ptoc_Var94->ks_pos , (int ) ptoc_Var90->res_keylen , (unsigned char)'\377' , &(*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16205)).integer.C_4.sp1r_returncode);
                    ptoc_Var90->res_prevpos = ptoc_Var90->res_actres + 1;
                    ptoc_Var90->res_prevkeys.reckeyspec.ks_len = (short ) sql__setrang(ptoc_Var90->res_keylen , -32768 , 32767);
                    }
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16213)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var89->mfr_invarp = 1;
            firstres = 1;
            ptoc_Var89->mfr_b_err = 350;
            if ((ptoc_Var95 = (int ) mtype) == 23 || ptoc_Var95 == 41)
                {
                ptoc_Var89->mfr_count2 = 0;
                tree_pos.tpsPno_gg00 = 2147483647;
                while ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16224)).integer.C_4.sp1r_returncode == 0 && ptoc_Var89->mfr_count2 < ptoc_Var89->mfr_count1 && (int ) ptoc_Var89->mfr_b_err == 350)
                    {
                    set_result.bd_max_rec_cnt = ptoc_Var89->mfr_count1 - ptoc_Var89->mfr_count2;
                    bd_lkey.integer.C_11.len = (short ) sql__setrang(ptoc_Var90->res_nextkeys.reckeyspec.ks_len , -32768 , 32767);
                    g10mv3("VAK73 " , 12 , sizeof(tak_res_keysbuf ) , sizeof(tsp00_Key ) , ptoc_Var90->boolean.C_false.res_keysbuf , (int ) ptoc_Var90->res_nextkeys.reckeyspec.ks_pos , bd_lkey.integer.C_11.k , 1 , (int ) bd_lkey.integer.C_11.len , &(*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16233)).integer.C_4.sp1r_returncode);
                    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16234)).integer.C_4.sp1r_returncode == 0)
                        {
                        b07cnext_record(&ptoc_Var87->a_transinf.tri_trans , &ptoc_Var90->res_restree , &bd_lkey , &set_result , &tree_pos , (*(tgg00_DataPart *) sql__nil(ptoc_Var91->integer.C_3.mb_data , "vak73.p" , 16239)).integer.C_1.mbp_buf);
                        if ((int ) ptoc_Var87->a_transinf.tri_trans.trError_gg00 == 1200)
                            {
                            ptoc_Var89->mfr_b_err = 0;
                            }
                        else
                            {
                            ptoc_Var89->mfr_b_err = (short ) sql__setrang(ptoc_Var87->a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                            }
                        }
                    if ((int ) ptoc_Var89->mfr_b_err != 0 && (int ) ptoc_Var89->mfr_b_err != 350 && (int ) ptoc_Var89->mfr_b_err != 1420)
                        {
                        a07_b_put_error(&(*acv) , (short ) sql__setrang(ptoc_Var89->mfr_b_err , -32768 , 32767) , 1);
                        }
                    else
                        {
                        ptoc_Var96 = &ptoc_Var87->a_mblock;
                        ptoc_Var97 = (tgg00_DataPart *) sql__nil(ptoc_Var96->integer.C_3.mb_data , "vak73.p" , 16254);
                        ptoc_Var90->res_first_fetch = 0;
                        ptoc_Var90->res_eof = 0;
                        set_result.bd_next = 0;
                        ptoc_Var89->mfr_cnt = set_result.bd_rec_cnt;
                        if ((char ) sql__setrang(firstres , 0 , 1) && ptoc_Var89->mfr_cnt > 0)
                            {
                            if ((int ) ptoc_Var90->res_keylen != (int ) ptoc_Var97->integer.C_4.mbp_keylen)
                                {
                                a07_b_put_error(&(*acv) , 6448 , 1);
                                }
                            else
                                {
                                if ((int ) ptoc_Var90->res_reclen != (int ) ptoc_Var97->integer.C_4.mbp_reclen)
                                    {
                                    a07_b_put_error(&(*acv) , 6450 , 1);
                                    }
                                else
                                    {
                                    firstres = 0;
                                    g10mv2("VAK73 " , 13 , ptoc_Var96->integer.C_2.mb_data_size , sizeof(tak_res_keysbuf ) , ptoc_Var97->integer.C_1.mbp_buf , 9 , ptoc_Var90->boolean.C_false.res_keysbuf , (int ) ptoc_Var90->res_prevkeys.reckeyspec.ks_pos , (int ) ptoc_Var90->res_keylen , &(*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16277)).integer.C_4.sp1r_returncode);
                                    ptoc_Var90->res_prevpos = sql__succ(ptoc_Var90->res_nextpos , -2147483647 , 2147483647 , "vak73.p" , 16279);
                                    ptoc_Var90->res_prevkeys.reckeyspec.ks_len = (short ) sql__setrang(ptoc_Var90->res_keylen , -32768 , 32767);
                                    }
                                }
                            }
                        start = 1;
                        if (unused_leng < (*(tsp1_part *) sql__nil(ptoc_Var87->a_curr_retpart , "vak73.p" , 16287)).integer.C_2.sp1p_buf_len + ptoc_Var89->mfr_cnt * (int ) ptoc_Var89->mfr_reclength)
                            {
                            if ((int ) ptoc_Var87->a_comp_type == 0 || (_ptoc_MEMCMP(unsigned char , 5, ptoc_Var87->a_comp_vers , "62000") < 0 ? 1 : 0))
                                {
                                ptoc_Var89->mfr_invarp = 0;
                                }
                            else
                                {
                                a07_b_put_error(&(*acv) , 4830 , 1);
                                }
                            }
                        ptoc_Var89->mfr_count2 = ptoc_Var89->mfr_count2 + ptoc_Var89->mfr_cnt;
                        ptoc_Var90->res_nextpos = ptoc_Var90->res_nextpos + ptoc_Var89->mfr_cnt;
                        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16299)).integer.C_4.sp1r_returncode == 0 && (char ) sql__setrang(ptoc_Var90->res_for_update , 0 , 1) && ptoc_Var89->mfr_count1 == 1)
                            {
                            ptoc_Var90->res_useupdk = (unsigned char ) sql__setrang(ptoc_Var90->res_upd_tabcnt , 0 , 255);
                            g10mv2("VAK73 " , 14 , ptoc_Var96->integer.C_2.mb_data_size , sizeof(tak_res_keysbuf ) , ptoc_Var97->integer.C_1.mbp_buf , (int ) ptoc_Var90->res_length + 1 , ptoc_Var90->boolean.C_false.res_keysbuf , (int ) ptoc_Var90->res_updkey.ks_pos , (int ) ptoc_Var90->res_reclen - (int ) ptoc_Var90->res_length , &(*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16308)).integer.C_4.sp1r_returncode);
                            }
                        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16311)).integer.C_4.sp1r_returncode == 0)
                            {
                            ptoc_Var95 = ptoc_Var89->mfr_cnt;
                            if (1 <= ptoc_Var95)
                                {
                                i = 1;
                                do
                                    {
                                    a60_change_results(&(*acv) , (*(tgg00_DataPart *) sql__nil(ptoc_Var96->integer.C_3.mb_data , "vak73.p" , 16317)).integer.C_1.mbp_buf , &ptoc_Var90->boolean.C_true.res_change , (int ) start + -1 , (int ) ptoc_Var89->mfr_reclength + (int ) ptoc_Var90->res_keylen + 8);
                                    if ((char ) sql__setrang(ptoc_Var89->mfr_invarp , 0 , 1))
                                        {
                                        a06retpart_move(&(*acv) , (tsp00_MoveObj *) &ptoc_Var97->integer.C_1.mbp_buf[sql__setrang((int ) start + (int ) ptoc_Var90->res_keylen + 8 , 1 , 8192000) + -1] , (int ) ptoc_Var89->mfr_reclength);
                                        }
                                    else
                                        {
                                        g10mv5("VAK73 " , 15 , ptoc_Var96->integer.C_2.mb_data_size , ptoc_Var96->integer.C_2.mb_data_size , ptoc_Var97->integer.C_1.mbp_buf , (int ) start + (int ) ptoc_Var90->res_keylen + 8 , ptoc_Var97->integer.C_1.mbp_buf , 3 , (int ) ptoc_Var89->mfr_reclength , &(*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16331)).integer.C_4.sp1r_returncode);
                                        ptoc_Var96->integer.C_2.mb_data_len = (int ) ptoc_Var89->mfr_reclength + 2;
                                        }
                                    if (i == ptoc_Var89->mfr_cnt && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16336)).integer.C_4.sp1r_returncode == 0)
                                        {
                                        g10mv2("VAK73 " , 16 , ptoc_Var96->integer.C_2.mb_data_size , sizeof(tak_res_keysbuf ) , (*(tgg00_DataPart *) sql__nil(ptoc_Var96->integer.C_3.mb_data , "vak73.p" , 16342)).integer.C_1.mbp_buf , (int ) start + 8 , ptoc_Var90->boolean.C_false.res_keysbuf , (int ) ptoc_Var90->res_nextkeys.reckeyspec.ks_pos , (int ) ptoc_Var90->res_keylen , &(*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16345)).integer.C_4.sp1r_returncode);
                                        ptoc_Var90->res_nextkeys.reckeyspec.ks_len = (short ) sql__setrang(ptoc_Var90->res_keylen , -32768 , 32767);
                                        }
                                    start = (short ) sql__setrang((short ) ((int ) start + (int ) ptoc_Var90->res_reclen) , -32768 , 32767);
                                    if (i == ptoc_Var95)
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
                if ((int ) ptoc_Var89->mfr_b_err == 1420)
                    {
                    ptoc_Var90->res_eof = 1;
                    if (ptoc_Var89->mfr_count2 > 0)
                        {
                        ptoc_Var89->mfr_b_err = 0;
                        }
                    else
                        {
                        ptoc_Var90->res_first_fetch = 0;
                        a07_b_put_error(&(*acv) , 6408 , 1);
                        }
                    }
                }
            else
                {
                ptoc_Var89->mfr_count2 = 0;
                if ((*(tsp1_part *) sql__nil(ptoc_Var87->a_curr_retpart , "vak73.p" , 16383)).integer.C_2.sp1p_buf_len + ptoc_Var89->mfr_count1 * (int ) ptoc_Var89->mfr_reclength > (*(tsp1_part *) sql__nil(ptoc_Var87->a_curr_retpart , "vak73.p" , 16384)).integer.C_2.sp1p_buf_size)
                    {
                    if ((int ) ptoc_Var87->a_comp_type == 0 || (_ptoc_MEMCMP(unsigned char , 5, ptoc_Var87->a_comp_vers , "62000") < 0 ? 1 : 0))
                        {
                        ptoc_Var89->mfr_invarp = 0;
                        }
                    else
                        {
                        a07_b_put_error(&(*acv) , 4830 , 1);
                        }
                    }
                else
                    {
                    (*(tsp1_part *) sql__nil(ptoc_Var87->a_curr_retpart , "vak73.p" , 16394)).integer.C_2.sp1p_buf_len = (*(tsp1_part *) sql__nil(ptoc_Var87->a_curr_retpart , "vak73.p" , 16395)).integer.C_2.sp1p_buf_len + ptoc_Var89->mfr_count1 * (int ) ptoc_Var89->mfr_reclength;
                    }
                tree_pos.tpsPno_gg00 = 2147483647;
                while ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16398)).integer.C_4.sp1r_returncode == 0 && ptoc_Var89->mfr_count2 < ptoc_Var89->mfr_count1 && (int ) ptoc_Var89->mfr_b_err == 350)
                    {
                    set_result.bd_max_rec_cnt = ptoc_Var89->mfr_count1 - ptoc_Var89->mfr_count2;
                    bd_lkey.integer.C_11.len = (short ) sql__setrang(ptoc_Var90->res_prevkeys.reckeyspec.ks_len , -32768 , 32767);
                    g10mv3("VAK73 " , 17 , sizeof(tak_res_keysbuf ) , sizeof(tsp00_Key ) , ptoc_Var90->boolean.C_false.res_keysbuf , (int ) ptoc_Var90->res_prevkeys.reckeyspec.ks_pos , bd_lkey.integer.C_11.k , 1 , (int ) bd_lkey.integer.C_11.len , &(*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16407)).integer.C_4.sp1r_returncode);
                    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16408)).integer.C_4.sp1r_returncode == 0)
                        {
                        b07cprev_record(&ptoc_Var87->a_transinf.tri_trans , &ptoc_Var90->res_restree , &bd_lkey , &set_result , &tree_pos , (*(tgg00_DataPart *) sql__nil(ptoc_Var91->integer.C_3.mb_data , "vak73.p" , 16413)).integer.C_1.mbp_buf);
                        if ((int ) ptoc_Var87->a_transinf.tri_trans.trError_gg00 == 1200)
                            {
                            ptoc_Var89->mfr_b_err = 0;
                            }
                        else
                            {
                            ptoc_Var89->mfr_b_err = (short ) sql__setrang(ptoc_Var87->a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                            }
                        }
                    if ((int ) ptoc_Var89->mfr_b_err != 0 && (int ) ptoc_Var89->mfr_b_err != 350 && (int ) ptoc_Var89->mfr_b_err != 1440)
                        {
                        a07_b_put_error(&(*acv) , (short ) sql__setrang(ptoc_Var89->mfr_b_err , -32768 , 32767) , 1);
                        }
                    else
                        {
                        ptoc_Var98 = &ptoc_Var87->a_mblock;
                        ptoc_Var99 = (tgg00_DataPart *) sql__nil(ptoc_Var98->integer.C_3.mb_data , "vak73.p" , 16428);
                        ptoc_Var90->res_first_fetch = 0;
                        ptoc_Var90->res_eof = 0;
                        set_result.bd_next = 0;
                        ptoc_Var89->mfr_cnt = set_result.bd_rec_cnt;
                        if ((char ) sql__setrang(firstres , 0 , 1) && ptoc_Var89->mfr_cnt > 0)
                            {
                            if ((int ) ptoc_Var90->res_keylen != (int ) ptoc_Var99->integer.C_4.mbp_keylen)
                                {
                                a07_b_put_error(&(*acv) , 6448 , 1);
                                }
                            else
                                {
                                if ((int ) ptoc_Var90->res_reclen != (int ) ptoc_Var99->integer.C_4.mbp_reclen)
                                    {
                                    a07_b_put_error(&(*acv) , 6450 , 1);
                                    }
                                else
                                    {
                                    firstres = 0;
                                    g10mv2("VAK73 " , 18 , ptoc_Var98->integer.C_2.mb_data_size , sizeof(tak_res_keysbuf ) , ptoc_Var99->integer.C_1.mbp_buf , 9 , ptoc_Var90->boolean.C_false.res_keysbuf , (int ) ptoc_Var90->res_nextkeys.reckeyspec.ks_pos , (int ) ptoc_Var90->res_keylen , &(*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16452)).integer.C_4.sp1r_returncode);
                                    ptoc_Var90->res_nextpos = sql__pred(ptoc_Var90->res_prevpos , -2147483647 , 2147483647 , "vak73.p" , 16454);
                                    ptoc_Var90->res_nextkeys.reckeyspec.ks_len = (short ) sql__setrang(ptoc_Var90->res_keylen , -32768 , 32767);
                                    }
                                }
                            }
                        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16460)).integer.C_4.sp1r_returncode == 0 && ((char ) sql__setrang(ptoc_Var90->res_for_update , 0 , 1) && ptoc_Var89->mfr_count1 == 1))
                            {
                            ptoc_Var90->res_useupdk = (unsigned char ) sql__setrang(ptoc_Var90->res_upd_tabcnt , 0 , 255);
                            g10mv2("VAK73 " , 19 , ptoc_Var98->integer.C_2.mb_data_size , sizeof(tak_res_keysbuf ) , (*(tgg00_DataPart *) sql__nil(ptoc_Var98->integer.C_3.mb_data , "vak73.p" , 16467)).integer.C_1.mbp_buf , (int ) ptoc_Var90->res_length + 1 , ptoc_Var90->boolean.C_false.res_keysbuf , (int ) ptoc_Var90->res_updkey.ks_pos , (int ) ptoc_Var90->res_reclen - (int ) ptoc_Var90->res_length , &(*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16469)).integer.C_4.sp1r_returncode);
                            }
                        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16472)).integer.C_4.sp1r_returncode == 0)
                            {
                            start = 1;
                            ptoc_Var95 = ptoc_Var89->mfr_cnt;
                            if (1 <= ptoc_Var95)
                                {
                                i = 1;
                                do
                                    {
                                    a60_change_results(&(*acv) , (*(tgg00_DataPart *) sql__nil(ptoc_Var98->integer.C_3.mb_data , "vak73.p" , 16479)).integer.C_1.mbp_buf , &ptoc_Var90->boolean.C_true.res_change , (int ) start + -1 , (int ) ptoc_Var89->mfr_reclength + (int ) ptoc_Var90->res_keylen + 8);
                                    if ((char ) sql__setrang(ptoc_Var89->mfr_invarp , 0 , 1))
                                        {
                                        g10mv5("VAK73 " , 20 , ptoc_Var98->integer.C_2.mb_data_size , (*(tsp1_part *) sql__nil(ptoc_Var87->a_curr_retpart , "vak73.p" , 16486)).integer.C_2.sp1p_buf_size , (*(tgg00_DataPart *) sql__nil(ptoc_Var98->integer.C_3.mb_data , "vak73.p" , 16487)).integer.C_1.mbp_buf , (int ) start + (int ) ptoc_Var90->res_keylen + 8 , (*(tsp1_part *) sql__nil(ptoc_Var87->a_curr_retpart , "vak73.p" , 16489)).integer.C_1.sp1p_buf , (ptoc_Var89->mfr_count1 - ptoc_Var89->mfr_count2 - i) * (int ) ptoc_Var89->mfr_reclength + 1 , (int ) ptoc_Var89->mfr_reclength , &(*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16491)).integer.C_4.sp1r_returncode);
                                        }
                                    else
                                        {
                                        if ((int ) start + (int ) ptoc_Var90->res_keylen + 4 > sizeof(tsp00_ResNum ) + 3)
                                            {
                                            g10mv5("VAK73 " , 21 , ptoc_Var98->integer.C_2.mb_data_size , ptoc_Var98->integer.C_2.mb_data_size , (*(tgg00_DataPart *) sql__nil(ptoc_Var98->integer.C_3.mb_data , "vak73.p" , 16499)).integer.C_1.mbp_buf , (int ) start + (int ) ptoc_Var90->res_keylen + 8 , (*(tgg00_DataPart *) sql__nil(ptoc_Var98->integer.C_3.mb_data , "vak73.p" , 16501)).integer.C_1.mbp_buf , sizeof(tsp00_ResNum ) + 3 , (int ) ptoc_Var89->mfr_reclength , &(*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16502)).integer.C_4.sp1r_returncode);
                                            }
                                        else
                                            {
                                            g10mvr("VAK73 " , 22 , ptoc_Var98->integer.C_2.mb_data_size , ptoc_Var98->integer.C_2.mb_data_size , (*(tgg00_DataPart *) sql__nil(ptoc_Var98->integer.C_3.mb_data , "vak73.p" , 16506)).integer.C_1.mbp_buf , (int ) start + (int ) ptoc_Var90->res_keylen + 8 , (*(tgg00_DataPart *) sql__nil(ptoc_Var98->integer.C_3.mb_data , "vak73.p" , 16508)).integer.C_1.mbp_buf , sizeof(tsp00_ResNum ) + 3 , (int ) ptoc_Var89->mfr_reclength , &(*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16509)).integer.C_4.sp1r_returncode);
                                            }
                                        ptoc_Var98->integer.C_2.mb_data_len = (int ) ptoc_Var89->mfr_reclength + sizeof(tsp00_ResNum ) + 2;
                                        }
                                    if (i == ptoc_Var89->mfr_cnt && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16515)).integer.C_4.sp1r_returncode == 0)
                                        {
                                        g10mv2("VAK73 " , 23 , ptoc_Var98->integer.C_2.mb_data_size , sizeof(tak_res_keysbuf ) , (*(tgg00_DataPart *) sql__nil(ptoc_Var98->integer.C_3.mb_data , "vak73.p" , 16521)).integer.C_1.mbp_buf , (int ) start + 8 , ptoc_Var90->boolean.C_false.res_keysbuf , (int ) ptoc_Var90->res_prevkeys.reckeyspec.ks_pos , (int ) ptoc_Var90->res_keylen , &(*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16525)).integer.C_4.sp1r_returncode);
                                        ptoc_Var90->res_prevkeys.reckeyspec.ks_len = (short ) sql__setrang(ptoc_Var90->res_keylen , -32768 , 32767);
                                        }
                                    start = (short ) sql__setrang((short ) ((int ) start + (int ) ptoc_Var90->res_reclen) , -32768 , 32767);
                                    if (i == ptoc_Var95)
                                        {
                                        break;
                                        }
                                    i += 1;
                                    }
                                while (!0);
                                }
                            ptoc_Var90->res_prevpos = ptoc_Var90->res_prevpos - ptoc_Var89->mfr_cnt;
                            ptoc_Var89->mfr_count2 = ptoc_Var89->mfr_count2 + ptoc_Var89->mfr_cnt;
                            }
                        }
                    }
                if ((int ) ptoc_Var89->mfr_b_err == 1440)
                    {
                    ptoc_Var90->res_first_fetch = 1;
                    if (ptoc_Var89->mfr_count2 > 0)
                        {
                        ptoc_Var89->mfr_b_err = 0;
                        }
                    else
                        {
                        ptoc_Var90->res_eof = 0;
                        a07_b_put_error(&(*acv) , 6408 , 1);
                        }
                    }
                if (ptoc_Var89->mfr_count2 < ptoc_Var89->mfr_count1 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16561)).integer.C_4.sp1r_returncode == 0)
                    {
                    if ((char ) sql__setrang(ptoc_Var89->mfr_invarp , 0 , 1))
                        {
                        g10mv5("VAK73 " , 24 , (*(tsp1_part *) sql__nil(ptoc_Var87->a_curr_retpart , "vak73.p" , 16567)).integer.C_2.sp1p_buf_size , (*(tsp1_part *) sql__nil(ptoc_Var87->a_curr_retpart , "vak73.p" , 16568)).integer.C_2.sp1p_buf_size , (*(tsp1_part *) sql__nil(ptoc_Var87->a_curr_retpart , "vak73.p" , 16569)).integer.C_1.sp1p_buf , ptoc_Var89->mfr_count1 * (int ) ptoc_Var89->mfr_reclength - ptoc_Var89->mfr_count2 * (int ) ptoc_Var89->mfr_reclength + 1 , (*(tsp1_part *) sql__nil(ptoc_Var87->a_curr_retpart , "vak73.p" , 16571)).integer.C_1.sp1p_buf , 1 , ptoc_Var89->mfr_count2 * (int ) ptoc_Var89->mfr_reclength , &(*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16573)).integer.C_4.sp1r_returncode);
                        (*(tsp1_part *) sql__nil(ptoc_Var87->a_curr_retpart , "vak73.p" , 16574)).integer.C_2.sp1p_buf_len = ptoc_Var89->mfr_count2 * (int ) ptoc_Var89->mfr_reclength;
                        }
                    }
                }
            t01int4(19 , "res_nextpos " , ptoc_Var90->res_nextpos);
            t01int4(19 , "res_prevpos " , ptoc_Var90->res_prevpos);
            if ((char ) sql__setrang(lcol_found , 0 , 1) && ptoc_Var89->mfr_cnt > 0)
                {
                lcol_lock = 1;
                a508_lget_long_columns(&(*acv) , &ptoc_Var90->boolean.C_true.res_change , &lcol_lock , ptoc_Var89->mfr_count2 , (int ) ptoc_Var89->mfr_reclength , -((int ) ptoc_Var90->res_keylen + 8));
                }
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16597)).integer.C_4.sp1r_returncode == 0)
        {
        if (ptoc_Var89->mfr_count2 < ptoc_Var89->mfr_count1)
            {
            (_ptoc_MOVE(unsigned char , 1, (*(tsp1_part *) sql__nil(ptoc_Var87->a_curr_retpart , "vak73.p" , 16602)).integer.C_2.sp1p_attributes, ptoc_Var86) , (*(tsp1_part *) sql__nil(ptoc_Var87->a_curr_retpart , "vak73.p" , 16602)).integer.C_2.sp1p_attributes);
            }
        a06finish_curr_retpart(&(*acv) , 5 , (short ) sql__setrang((short ) ptoc_Var89->mfr_count2 , -32768 , 32767));
        if ((int ) ptoc_Var87->a_sqlmode == 2 || (_ptoc_MEMCMP(unsigned char , 5, ptoc_Var87->a_comp_vers , "62000") < 0 ? 0 : 1))
            {
            a60rescount(&(*acv) , (*(struct tak_systembuffer *) sql__nil(ptoc_Var88->px[1] , "vak73.p" , 16608)).integer.C_26.sreskey.res_nextpos);
            }
        else
            {
            a60rescount(&(*acv) , ptoc_Var89->mfr_count2);
            }
        if (!(char ) sql__setrang(ptoc_Var89->mfr_invarp , 0 , 1))
            {
            ptoc_Var100 = &ptoc_Var87->a_mblock;
            ptoc_Var101 = (tgg00_DataPart *) sql__nil(ptoc_Var100->integer.C_3.mb_data , "vak73.p" , 16614);
            g10mv10("VAK73 " , 25 , sizeof(tsp00_ResNum ) , ptoc_Var100->integer.C_2.mb_data_size , ptoc_Var87->a_resultnum , 1 , ptoc_Var101->integer.C_1.mbp_buf , 1 , sizeof(tsp00_ResNum ) , &(*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16619)).integer.C_4.sp1r_returncode);
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16620)).integer.C_4.sp1r_returncode == 0)
                {
                ic2.boolean.C_true.map_int = (short ) sql__setrang((short ) ptoc_Var89->mfr_count2 , -32768 , 32767);
                if ((int ) (*(tsp1_packet *) sql__nil(ptoc_Var87->a_out_packet , "vak73.p" , 16624)).sp1_header.sp1h_mess_swap != (int ) g01code.kernel_swap)
                    {
                    ptoc_Var101->integer.C_1.mbp_buf[sql__setrang(sizeof(tsp00_ResNum ) + 1 , 1 , 8192000) + -1] = (unsigned char ) sql__setrang(ic2.boolean.C_false.map_c2[1] , 0 , 255);
                    ptoc_Var101->integer.C_1.mbp_buf[sql__setrang(sizeof(tsp00_ResNum ) + 2 , 1 , 8192000) + -1] = (unsigned char ) sql__setrang(ic2.boolean.C_false.map_c2[0] , 0 , 255);
                    }
                else
                    {
                    ptoc_Var101->integer.C_1.mbp_buf[sql__setrang(sizeof(tsp00_ResNum ) + 1 , 1 , 8192000) + -1] = (unsigned char ) sql__setrang(ic2.boolean.C_false.map_c2[0] , 0 , 255);
                    ptoc_Var101->integer.C_1.mbp_buf[sql__setrang(sizeof(tsp00_ResNum ) + 2 , 1 , 8192000) + -1] = (unsigned char ) sql__setrang(ic2.boolean.C_false.map_c2[1] , 0 , 255);
                    }
                (*(tsp1_segment *) sql__nil(ptoc_Var87->a_return_segm , "vak73.p" , 16637)).integer.C_4.sp1r_returncode = (short ) sql__setrang(a07_return_code(6536 , (unsigned char ) sql__setrang(ptoc_Var87->a_sqlmode , 0 , 3)) , -32768 , 32767);
                }
            }
        }
    cccprint("<a73_more_fetch ");
    }

static
  tak_rollback_info ptoc_Var102 = 
  {
  (unsigned char)'\003'
  };


int a73_one_fetch(acv, sparr, mtype, pos)
tak_all_command_glob *acv;
tak_syspointerarr *sparr;
tgg_message2_type mtype;
tsp_int4 pos;
    {
    char fill_ret_segm;
    tgg_basis_error mfr_b_err;
    tgg_bd_set_result_record set_result;
    tsp_int2 k;
    tgg00_FilePos tree_pos;
    char lcol_found;
    char lcol_lock;
    int func_code;
    tgg_lkey bd_lkey;
    tak_all_command_glob *ptoc_Var103;
    tak_syspointerarr *ptoc_Var104;
    tak_parsinforecord *ptoc_Var105;
    tak_resultkeyrecord *ptoc_Var106;
    struct tgg00_MessBlock *ptoc_Var107;
    tgg00_BdSetResultRecord *ptoc_Var108;
    struct tgg00_MessBlock *ptoc_Var109;
    int ptoc_Var110;
    struct tak_systembuffer *ptoc_Var111;
    tak_resultkeyrecord *ptoc_Var112;
    struct tgg00_MessBlock *ptoc_Var113;
    cccprint("AK73  a73_one_fe");
    ptoc_Var103 = &*acv;
    ptoc_Var104 = &*sparr;
    lcol_found = 0;
    fill_ret_segm = 1;
    if ((int ) ptoc_Var103->a_ex_kind == 1 && ptoc_Var104->px[0] != (struct tak_systembuffer *) (int *) 0)
        {
        ptoc_Var105 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var104->px[0] , "vak73.p" , 16677)).integer.C_23.sparsinfo;
        if ((int ) ptoc_Var105->integer.C_2.p_function_code > 1000)
            {
            func_code = (int ) ptoc_Var105->integer.C_2.p_function_code + -1000;
            }
        else
            {
            func_code = (int ) ptoc_Var105->integer.C_2.p_function_code;
            }
        if (func_code != 247 && (func_code < 206 || func_code > 211))
            {
            fill_ret_segm = 0;
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 16694)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var106 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var104->px[1] , "vak73.p" , 16696)).integer.C_26.sreskey;
        ptoc_Var107 = &ptoc_Var103->a_mblock;
        if ((int ) ptoc_Var106->boolean.C_true.res_change.cr_colcount > 0)
            {
            lcol_found = (char ) sql__setrang(a508_lcol_found(&(*acv) , &ptoc_Var106->boolean.C_true.res_change) , 0 , 1);
            }
        t01int4(19 , "res_nextpos " , ptoc_Var106->res_nextpos);
        t01int4(19 , "res_prevpos " , ptoc_Var106->res_prevpos);
        ptoc_Var108 = &set_result;
        ptoc_Var108->bd_key_check_len = 0;
        ptoc_Var108->bd_max_rec_cnt = 1;
        ptoc_Var108->bd_max_fill_len = ptoc_Var103->a_mblock.integer.C_2.mb_data_size;
        ptoc_Var108->bd_next = 1;
        mfr_b_err = 0;
        a09activate_lock(&ptoc_Var103->a_transinf.tri_trans , ptoc_Var103->a_work_st_addr , (short ) sql__setrang(ptoc_Var103->a_work_st_max , -32768 , 32767) , &mfr_b_err);
        if ((int ) mfr_b_err == 4740)
            {
            _ptoc_MOVE(unsigned char , 1, ptoc_Var103->a_rollback_info, ptoc_Var102);
            mfr_b_err = 4900;
            }
        if ((int ) mfr_b_err != 0)
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(mfr_b_err , -32768 , 32767) , 1);
            }
        else
            {
            if ((int ) mtype == 59)
                {
                if ((char ) sql__setrang(ptoc_Var106->res_eof , 0 , 1) && pos >= 0 || (char ) sql__setrang(ptoc_Var106->res_first_fetch , 0 , 1) && pos <= 0)
                    {
                    a07_b_put_error(&(*acv) , 6408 , 1);
                    }
                else
                    {
                    if (pos >= 0)
                        {
                        if (!(char ) sql__setrang(ptoc_Var106->res_first_fetch , 0 , 1))
                            {
                            pos = ptoc_Var106->res_nextpos + pos;
                            }
                        }
                    else
                        {
                        if ((char ) sql__setrang(ptoc_Var106->res_eof , 0 , 1))
                            {
                            pos = ptoc_Var106->res_actres + pos + 1;
                            }
                        else
                            {
                            pos = ptoc_Var106->res_prevpos + pos;
                            }
                        }
                    if (pos <= 0)
                        {
                        ptoc_Var106->res_first_fetch = 1;
                        ptoc_Var106->res_eof = 0;
                        a07_b_put_error(&(*acv) , 6408 , 1);
                        }
                    else
                        {
                        mtype = 16;
                        }
                    }
                }
            if ((int ) mtype == 16)
                {
                if (pos < 0)
                    {
                    pos = ptoc_Var106->res_actres + pos + 1;
                    }
                if (pos > ptoc_Var106->res_actres)
                    {
                    ptoc_Var106->res_first_fetch = 0;
                    ptoc_Var106->res_eof = 1;
                    a07_b_put_error(&(*acv) , 6408 , 1);
                    }
                else
                    {
                    if (pos <= 0)
                        {
                        ptoc_Var106->res_first_fetch = 1;
                        ptoc_Var106->res_eof = 0;
                        a07_b_put_error(&(*acv) , 6408 , 1);
                        }
                    else
                        {
                        if ((char ) sql__setrang(ptoc_Var106->res_order , 0 , 1))
                            {
                            ak73one_before_pos(&(*acv) , &(*sparr) , pos);
                            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 16794)).integer.C_4.sp1r_returncode == 0)
                                {
                                bd_lkey.integer.C_11.len = (short ) sql__setrang(ptoc_Var106->res_nextkeys.reckeyspec.ks_len , -32768 , 32767);
                                g10mv3("VAK73 " , 26 , sizeof(tak_res_keysbuf ) , sizeof(tsp00_Key ) , ptoc_Var106->boolean.C_false.res_keysbuf , (int ) ptoc_Var106->res_nextkeys.reckeyspec.ks_pos , bd_lkey.integer.C_11.k , 1 , (int ) bd_lkey.integer.C_11.len , &(*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 16802)).integer.C_4.sp1r_returncode);
                                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 16803)).integer.C_4.sp1r_returncode == 0)
                                    {
                                    if (pos == ptoc_Var106->res_nextpos)
                                        {
                                        b07cget_record(&ptoc_Var103->a_transinf.tri_trans , &ptoc_Var106->res_restree , &bd_lkey , &(*(tgg00_DataPart *) sql__nil(ptoc_Var107->integer.C_3.mb_data , "vak73.p" , 16810)).integer.C_3.mbp_rec);
                                        }
                                    else
                                        {
                                        tree_pos.tpsPno_gg00 = 2147483647;
                                        b07cnext_record(&ptoc_Var103->a_transinf.tri_trans , &ptoc_Var106->res_restree , &bd_lkey , &set_result , &tree_pos , (*(tgg00_DataPart *) sql__nil(ptoc_Var107->integer.C_3.mb_data , "vak73.p" , 16816)).integer.C_1.mbp_buf);
                                        }
                                    if ((int ) ptoc_Var103->a_transinf.tri_trans.trError_gg00 == 1200)
                                        {
                                        mfr_b_err = 0;
                                        }
                                    else
                                        {
                                        mfr_b_err = (short ) sql__setrang(ptoc_Var103->a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                                        }
                                    }
                                if ((int ) mfr_b_err == 1420 && set_result.bd_rec_cnt == 0)
                                    {
                                    a07_b_put_error(&(*acv) , 6408 , 1);
                                    }
                                else
                                    {
                                    if ((int ) mfr_b_err != 0)
                                        {
                                        a07_b_put_error(&(*acv) , (short ) sql__setrang(mfr_b_err , -32768 , 32767) , 1);
                                        }
                                    else
                                        {
                                        ptoc_Var109 = &ptoc_Var103->a_mblock;
                                        g10mv2("VAK73 " , 27 , ptoc_Var109->integer.C_2.mb_data_size , sizeof(tak_res_keysbuf ) , (*(tgg00_DataPart *) sql__nil(ptoc_Var109->integer.C_3.mb_data , "vak73.p" , 16843)).integer.C_1.mbp_buf , 9 , ptoc_Var106->boolean.C_false.res_keysbuf , (int ) ptoc_Var106->res_nextkeys.reckeyspec.ks_pos , (int ) (*(tgg00_DataPart *) sql__nil(ptoc_Var109->integer.C_3.mb_data , "vak73.p" , 16847)).integer.C_4.mbp_keylen , &(*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 16848)).integer.C_4.sp1r_returncode);
                                        ptoc_Var106->res_nextkeys.reckeyspec.ks_len = (short ) sql__setrang((*(tgg00_DataPart *) sql__nil(ptoc_Var109->integer.C_3.mb_data , "vak73.p" , 16850)).integer.C_4.mbp_keylen , -32768 , 32767);
                                        if (pos != ptoc_Var106->res_nextpos)
                                            {
                                            ptoc_Var106->res_nextpos = sql__succ(ptoc_Var106->res_nextpos , -2147483647 , 2147483647 , "vak73.p" , 16854);
                                            }
                                        }
                                    }
                                }
                            }
                        else
                            {
                            s20ch4sw(pos , (unsigned char ) sql__setrang(g01code.kernel_swap , 0 , 3) , ptoc_Var106->boolean.C_false.res_keysbuf , (int ) ptoc_Var106->res_nextkeys.reckeyspec.ks_pos , 1);
                            ptoc_Var106->res_nextkeys.reckeyspec.ks_len = (short ) sql__setrang(ptoc_Var106->res_keylen , -32768 , 32767);
                            bd_lkey.integer.C_11.len = (short ) sql__setrang(ptoc_Var106->res_nextkeys.reckeyspec.ks_len , -32768 , 32767);
                            g10mv3("VAK73 " , 28 , sizeof(tak_res_keysbuf ) , sizeof(tsp00_Key ) , ptoc_Var106->boolean.C_false.res_keysbuf , (int ) ptoc_Var106->res_nextkeys.reckeyspec.ks_pos , bd_lkey.integer.C_11.k , 1 , (int ) bd_lkey.integer.C_11.len , &(*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 16874)).integer.C_4.sp1r_returncode);
                            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 16875)).integer.C_4.sp1r_returncode == 0)
                                {
                                ptoc_Var106->res_nextpos = pos;
                                b07cget_record(&ptoc_Var103->a_transinf.tri_trans , &ptoc_Var106->res_restree , &bd_lkey , &(*(tgg00_DataPart *) sql__nil(ptoc_Var107->integer.C_3.mb_data , "vak73.p" , 16880)).integer.C_3.mbp_rec);
                                if ((int ) ptoc_Var103->a_transinf.tri_trans.trError_gg00 != 0)
                                    {
                                    a07_b_put_error(&(*acv) , (short ) sql__setrang(ptoc_Var103->a_transinf.tri_trans.trError_gg00 , -32768 , 32767) , 1);
                                    }
                                }
                            }
                        if ((char ) sql__setrang(fill_ret_segm , 0 , 1))
                            {
                            a60rescount(&(*acv) , 1);
                            }
                        }
                    }
                }
            else
                {
                if ((int ) mtype == 23 || (int ) mtype == 41 && (char ) sql__setrang(ptoc_Var106->res_first_fetch , 0 , 1))
                    {
                    ptoc_Var106->res_nextpos = 0;
                    ptoc_Var106->res_nextkeys.reckeyspec.ks_len = 0;
                    }
                else
                    {
                    if ((int ) mtype == 32 || (int ) mtype == 53 && (char ) sql__setrang(ptoc_Var106->res_eof , 0 , 1) || (int ) mtype == 41 && (char ) sql__setrang(ptoc_Var106->res_eof , 0 , 1))
                        {
                        g10fil("VAK73 " , 29 , sizeof(tak_res_keysbuf ) , ptoc_Var106->boolean.C_false.res_keysbuf , (int ) ptoc_Var106->res_prevkeys.reckeyspec.ks_pos , (int ) ptoc_Var106->res_keylen , (unsigned char)'\377' , &(*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 16918)).integer.C_4.sp1r_returncode);
                        ptoc_Var106->res_prevpos = ptoc_Var106->res_actres + 1;
                        ptoc_Var106->res_prevkeys.reckeyspec.ks_len = (short ) sql__setrang(ptoc_Var106->res_keylen , -32768 , 32767);
                        }
                    }
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 16924)).integer.C_4.sp1r_returncode == 0)
                    {
                    if ((int ) (*acv).a_qualified_jv_upd == 0)
                        {
                        if ((int ) ptoc_Var103->a_sqlmode == 2 || (_ptoc_MEMCMP(unsigned char , 5, ptoc_Var103->a_comp_vers , "62000") < 0 ? 0 : 1))
                            {
                            a60rescount(&(*acv) , sql__succ(ptoc_Var106->res_nextpos , -2147483647 , 2147483647 , "vak73.p" , 16931));
                            }
                        else
                            {
                            a60rescount(&(*acv) , 1);
                            }
                        }
                    }
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 16937)).integer.C_4.sp1r_returncode == 0)
                    {
                    if ((ptoc_Var110 = (int ) mtype) == 23 || ptoc_Var110 == 41)
                        {
                        tree_pos.tpsPno_gg00 = 2147483647;
                        bd_lkey.integer.C_11.len = (short ) sql__setrang(ptoc_Var106->res_nextkeys.reckeyspec.ks_len , -32768 , 32767);
                        g10mv3("VAK73 " , 30 , sizeof(tak_res_keysbuf ) , sizeof(tsp00_Key ) , ptoc_Var106->boolean.C_false.res_keysbuf , (int ) ptoc_Var106->res_nextkeys.reckeyspec.ks_pos , bd_lkey.integer.C_11.k , 1 , (int ) bd_lkey.integer.C_11.len , &(*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 16948)).integer.C_4.sp1r_returncode);
                        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 16949)).integer.C_4.sp1r_returncode == 0)
                            {
                            b07cnext_record(&ptoc_Var103->a_transinf.tri_trans , &ptoc_Var106->res_restree , &bd_lkey , &set_result , &tree_pos , (*(tgg00_DataPart *) sql__nil(ptoc_Var107->integer.C_3.mb_data , "vak73.p" , 16954)).integer.C_1.mbp_buf);
                            mfr_b_err = (short ) sql__setrang(ptoc_Var103->a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                            }
                        if ((int ) mfr_b_err == 0 || (int ) mfr_b_err == 1200)
                            {
                            ptoc_Var106->res_nextpos = sql__succ(ptoc_Var106->res_nextpos , -2147483647 , 2147483647 , "vak73.p" , 16960);
                            }
                        else
                            {
                            if ((int ) mfr_b_err == 1420)
                                {
                                ptoc_Var106->res_eof = 1;
                                ptoc_Var106->res_first_fetch = 0;
                                }
                            }
                        }
                    else
                        {
                        if ((int ) mtype == 58)
                            {
                            if ((char ) sql__setrang(ptoc_Var106->res_first_fetch , 0 , 1) || (char ) sql__setrang(ptoc_Var106->res_eof , 0 , 1))
                                {
                                a07_b_put_error(&(*acv) , 6232 , 1);
                                }
                            else
                                {
                                bd_lkey.integer.C_11.len = (short ) sql__setrang(ptoc_Var106->res_nextkeys.reckeyspec.ks_len , -32768 , 32767);
                                g10mv3("VAK73 " , 31 , sizeof(tak_res_keysbuf ) , sizeof(tsp00_Key ) , ptoc_Var106->boolean.C_false.res_keysbuf , (int ) ptoc_Var106->res_nextkeys.reckeyspec.ks_pos , bd_lkey.integer.C_11.k , 1 , (int ) bd_lkey.integer.C_11.len , &(*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 16985)).integer.C_4.sp1r_returncode);
                                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 16986)).integer.C_4.sp1r_returncode == 0)
                                    {
                                    b07cget_record(&ptoc_Var103->a_transinf.tri_trans , &ptoc_Var106->res_restree , &bd_lkey , &(*(tgg00_DataPart *) sql__nil(ptoc_Var107->integer.C_3.mb_data , "vak73.p" , 16990)).integer.C_3.mbp_rec);
                                    }
                                }
                            }
                        else
                            {
                            tree_pos.tpsPno_gg00 = 2147483647;
                            bd_lkey.integer.C_11.len = (short ) sql__setrang(ptoc_Var106->res_prevkeys.reckeyspec.ks_len , -32768 , 32767);
                            g10mv3("VAK73 " , 32 , sizeof(tak_res_keysbuf ) , sizeof(tsp00_Key ) , ptoc_Var106->boolean.C_false.res_keysbuf , (int ) ptoc_Var106->res_prevkeys.reckeyspec.ks_pos , bd_lkey.integer.C_11.k , 1 , (int ) bd_lkey.integer.C_11.len , &(*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 17002)).integer.C_4.sp1r_returncode);
                            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 17003)).integer.C_4.sp1r_returncode == 0)
                                {
                                b07cprev_record(&ptoc_Var103->a_transinf.tri_trans , &ptoc_Var106->res_restree , &bd_lkey , &set_result , &tree_pos , (*(tgg00_DataPart *) sql__nil(ptoc_Var107->integer.C_3.mb_data , "vak73.p" , 17008)).integer.C_1.mbp_buf);
                                mfr_b_err = (short ) sql__setrang(ptoc_Var103->a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                                }
                            if ((int ) mfr_b_err == 0 || (int ) mfr_b_err == 1200)
                                {
                                ptoc_Var106->res_prevpos = sql__pred(ptoc_Var106->res_prevpos , -2147483647 , 2147483647 , "vak73.p" , 17015);
                                }
                            else
                                {
                                if ((int ) mfr_b_err == 1440)
                                    {
                                    ptoc_Var106->res_eof = 0;
                                    ptoc_Var106->res_first_fetch = 1;
                                    }
                                }
                            }
                        }
                    }
                if ((int ) mfr_b_err == 1200 && (int ) mtype != 58 || (int ) mfr_b_err == 350)
                    {
                    mfr_b_err = 0;
                    }
                if ((int ) mfr_b_err != 0)
                    {
                    a07_b_put_error(&(*acv) , (short ) sql__setrang(mfr_b_err , -32768 , 32767) , 1);
                    }
                }
            }
        t01int4(19 , "res_nextpos " , ptoc_Var106->res_nextpos);
        t01int4(19 , "res_prevpos " , ptoc_Var106->res_prevpos);
        }
    ptoc_Var111 = (struct tak_systembuffer *) sql__nil(ptoc_Var104->px[1] , "vak73.p" , 17048);
    ptoc_Var112 = &ptoc_Var111->integer.C_26.sreskey;
    ptoc_Var113 = &ptoc_Var103->a_mblock;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 17050)).integer.C_4.sp1r_returncode != 0)
        {
        ptoc_Var112->res_useupdk = (unsigned char)'\0';
        }
    else
        {
        if ((int ) ptoc_Var112->res_keylen != (int ) (*(tgg00_DataPart *) sql__nil(ptoc_Var113->integer.C_3.mb_data , "vak73.p" , 17054)).integer.C_4.mbp_keylen)
            {
            a07_b_put_error(&(*acv) , 6448 , 1);
            }
        else
            {
            if ((int ) ptoc_Var112->res_reclen != (int ) (*(tgg00_DataPart *) sql__nil(ptoc_Var113->integer.C_3.mb_data , "vak73.p" , 17058)).integer.C_4.mbp_reclen)
                {
                a07_b_put_error(&(*acv) , 6450 , 1);
                }
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 17064)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var112->res_first_fetch = 0;
        ptoc_Var112->res_eof = 0;
        g10mv2("VAK73 " , 33 , ptoc_Var113->integer.C_2.mb_data_size , sizeof(tak_res_keysbuf ) , (*(tgg00_DataPart *) sql__nil(ptoc_Var113->integer.C_3.mb_data , "vak73.p" , 17071)).integer.C_1.mbp_buf , 9 , ptoc_Var112->boolean.C_false.res_keysbuf , (int ) ptoc_Var112->res_nextkeys.reckeyspec.ks_pos , (int ) (*(tgg00_DataPart *) sql__nil(ptoc_Var113->integer.C_3.mb_data , "vak73.p" , 17073)).integer.C_4.mbp_keylen , &(*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 17074)).integer.C_4.sp1r_returncode);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 17077)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var112->res_nextkeys.reckeyspec.ks_len = (short ) sql__setrang((*(tgg00_DataPart *) sql__nil(ptoc_Var113->integer.C_3.mb_data , "vak73.p" , 17080)).integer.C_4.mbp_keylen , -32768 , 32767);
        ptoc_Var112->res_prevkeys.reckeyspec.ks_len = (short ) sql__setrang((*(tgg00_DataPart *) sql__nil(ptoc_Var113->integer.C_3.mb_data , "vak73.p" , 17081)).integer.C_4.mbp_keylen , -32768 , 32767);
        g10mv6("VAK73 " , 34 , sizeof(tak_res_keysbuf ) , sizeof(tak_res_keysbuf ) , ptoc_Var112->boolean.C_false.res_keysbuf , (int ) ptoc_Var112->res_nextkeys.reckeyspec.ks_pos , ptoc_Var112->boolean.C_false.res_keysbuf , (int ) ptoc_Var112->res_prevkeys.reckeyspec.ks_pos , (int ) (*(tgg00_DataPart *) sql__nil(ptoc_Var113->integer.C_3.mb_data , "vak73.p" , 17086)).integer.C_4.mbp_keylen , &(*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 17086)).integer.C_4.sp1r_returncode);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 17089)).integer.C_4.sp1r_returncode == 0)
        {
        if ((ptoc_Var110 = (int ) mtype) == 32 || ptoc_Var110 == 53)
            {
            ptoc_Var112->res_nextpos = ptoc_Var112->res_prevpos;
            }
        else
            {
            ptoc_Var112->res_prevpos = ptoc_Var112->res_nextpos;
            }
        (*(tgg00_DataPart *) sql__nil(ptoc_Var113->integer.C_3.mb_data , "vak73.p" , 17098)).integer.C_4.mbp_reclen = (short ) sql__setrang(ptoc_Var112->res_length , -32768 , 32767);
        ptoc_Var113->integer.C_2.mb_data_len = (int ) (*(tgg00_DataPart *) sql__nil(ptoc_Var113->integer.C_3.mb_data , "vak73.p" , 17099)).integer.C_4.mbp_reclen;
        if ((char ) sql__setrang(ptoc_Var112->res_for_update , 0 , 1))
            {
            ptoc_Var112->res_useupdk = (unsigned char ) sql__setrang(ptoc_Var112->res_upd_tabcnt , 0 , 255);
            g10mv2("VAK73 " , 35 , ptoc_Var113->integer.C_2.mb_data_size , sizeof(tak_res_keysbuf ) , (*(tgg00_DataPart *) sql__nil(ptoc_Var113->integer.C_3.mb_data , "vak73.p" , 17106)).integer.C_1.mbp_buf , (int ) ptoc_Var112->res_length + 1 , ptoc_Var112->boolean.C_false.res_keysbuf , (int ) ptoc_Var112->res_updkey.ks_pos , (int ) ptoc_Var112->res_reclen - (int ) ptoc_Var112->res_length , &(*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 17108)).integer.C_4.sp1r_returncode);
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 17113)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) (*acv).a_qualified_jv_upd == 0 && (char ) sql__setrang(fill_ret_segm , 0 , 1))
            {
            a60_change_results(&(*acv) , (*(tgg00_DataPart *) sql__nil(ptoc_Var113->integer.C_3.mb_data , "vak73.p" , 17120)).integer.C_1.mbp_buf , &ptoc_Var112->boolean.C_true.res_change , 0 , (int ) ptoc_Var112->res_length);
            a60_put_result(&(*acv) , &ptoc_Var103->a_mblock , (int ) ptoc_Var112->res_keylen + 8);
            if ((char ) sql__setrang(lcol_found , 0 , 1) && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 17125)).integer.C_4.sp1r_returncode == 0)
                {
                lcol_lock = 1;
                a508_lget_long_columns(&(*acv) , &ptoc_Var112->boolean.C_true.res_change , &lcol_lock , 1 , (int ) ptoc_Var112->res_length , -((int ) (*(tgg00_DataPart *) sql__nil(ptoc_Var113->integer.C_3.mb_data , "vak73.p" , 17130)).integer.C_4.mbp_keylen + 8));
                }
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var103->a_return_segm , "vak73.p" , 17133)).integer.C_4.sp1r_returncode == 0)
                {
                a06finish_curr_retpart(&(*acv) , 5 , 1);
                }
            }
        t01moveobj(5 , (*(tgg00_DataPart *) sql__nil(ptoc_Var113->integer.C_3.mb_data , "vak73.p" , 17139)).integer.C_1.mbp_buf , 1 , (int ) (*(tgg00_DataPart *) sql__nil(ptoc_Var113->integer.C_3.mb_data , "vak73.p" , 17139)).integer.C_4.mbp_reclen);
        t01int4(5 , "part2_len   " , ptoc_Var113->integer.C_2.mb_data_len);
        }
    cccprint("<a73_one_fetch  ");
    }



int a73_calc_unused_space(acv)
tak_all_command_glob *acv;
    {
    int _a73_calc_unused_space;
    tsp1_part_ptr part_ptr;
    int no_of_part;
    char longinfos_found;
    char resname_found;
    tsp_int4 length;
    tak_all_command_glob *ptoc_Var114;
    tsp1_packet *ptoc_Var115;
    tsp1_part *ptoc_Var116;
    int ptoc_Var117;
    cccprint("AK73  a73_calc_u");
    ptoc_Var114 = &*acv;
    ptoc_Var115 = (tsp1_packet *) sql__nil(ptoc_Var114->a_out_packet , "vak73.p" , 17166);
    if ((int ) ptoc_Var114->a_comp_type == 0 || (_ptoc_MEMCMP(unsigned char , 5, ptoc_Var114->a_comp_vers , "62000") < 0 ? 1 : 0))
        {
        length = 8238;
        no_of_part = 1;
        longinfos_found = 0;
        resname_found = 0;
        part_ptr = (tsp1_part *) &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak73.p" , 17177)).integer.C_2.sp1p_part_header;
        do
            {
            ptoc_Var116 = (tsp1_part *) sql__nil(part_ptr , "vak73.p" , 17179);
            switch ((int ) ptoc_Var116->integer.C_2.sp1p_part_kind)
                {
                case 2 :
                    length = length - (int ) ptoc_Var116->integer.C_2.sp1p_arg_count * 24 + -2;
                    longinfos_found = 1;
                    break;
                case 5 :
                case 18 :
                    break;
                case 11 :
                    length = length + -12;
                    break;
                case 12 :
                    length = length + -11;
                    break;
                case 13 :
                    length = length - sizeof(unsigned char [18]);
                    resname_found = 1;
                    break;
                case 14 :
                    if (!(char ) sql__setrang(longinfos_found , 0 , 1))
                        {
                        length = length - (int ) ptoc_Var116->integer.C_2.sp1p_arg_count * sizeof(struct ptoc_Type178
 {
 union  
   {
   struct  
     {
     tsp_c2 simodeiotype;
     tsp_c2 sityplen_data;
     tsp_c2 sifrac_inoutl;
     tsp_int2 sibufpos;
     } C_true;
   struct  
     {
     tsp_int4 sifiller1;
     tsp_int2 silonglength;
     tsp_int2 sifiller2;
     } C_false;
   } boolean;
 } ) + -2;
                        }
                    break;
                default :
                    (unsigned char ) sql__caseerr("vak73.p" , 17181);
                    break;
                }
            part_ptr = (tsp1_part *) &(*(tsp1_part *) sql__nil(part_ptr , "vak73.p" , 17211)).integer.C_1.sp1p_buf[sql__setrang(a01aligned_cmd_len((*(tsp1_part *) sql__nil(part_ptr , "vak73.p" , 17212)).integer.C_2.sp1p_buf_len) + 1 , 1 , 8192000) + -1];
            no_of_part = sql__succ(no_of_part , -2147483647 , 2147483647 , "vak73.p" , 17213);
            }
        while (!(no_of_part > (int ) (*(tsp1_segment *) sql__nil(ptoc_Var114->a_return_segm , "vak73.p" , 17217)).integer.C_3.sp1s_no_of_parts));
        if (((ptoc_Var117 = (int ) (*(tsp1_segment *) sql__nil(ptoc_Var114->a_return_segm , "vak73.p" , 17219)).integer.C_4.sp1r_function_code) == 4 || ptoc_Var117 == 1004 || ptoc_Var117 == 1244 || ptoc_Var117 == 244 || ptoc_Var117 == 27 || ptoc_Var117 == 201 || ptoc_Var117 == 202 || ptoc_Var117 == 203 || ptoc_Var117 == 204 || ptoc_Var117 == 205) && !(char ) sql__setrang(resname_found , 0 , 1) && !(char ) sql__setrang((*(tsp1_segment *) sql__nil(ptoc_Var114->a_cmd_segm , "vak73.p" , 17226)).integer.C_3.sp1c_with_info , 0 , 1) && ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var114->a_cmd_segm , "vak73.p" , 17227)).integer.C_3.sp1c_mess_type == 13 || (int ) (*(tsp1_segment *) sql__nil(ptoc_Var114->a_cmd_segm , "vak73.p" , 17228)).integer.C_3.sp1c_mess_type == 2))
            {
            length = length - sizeof(unsigned char [18]);
            }
        t01int4(5 , "unused vspac" , length);
        }
    else
        {
        length = (*(tsp1_part *) sql__nil(ptoc_Var114->a_curr_retpart , "vak73.p" , 17235)).integer.C_2.sp1p_buf_size;
        }
    _a73_calc_unused_space = length;
    cccprint("<a73_calc_unused");
    return(_a73_calc_unused_space);
    }
