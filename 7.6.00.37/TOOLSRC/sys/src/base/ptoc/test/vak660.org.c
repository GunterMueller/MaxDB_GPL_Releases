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
  unsigned char tsp_c24[24];
typedef
  tsp00_C40 tsp_c40;
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
    unsigned char para_type;
    unsigned char para_frac;
    tsp_int2 para_length;
    } tsp1_param_spec_list[2000];
typedef
  tsp00_Addr tgg00_VoidPtr;
typedef
  struct ptoc_Type10
    {
    tsp00_C4 ci4_gg00;
    } tgg90_Cint4;
typedef
  struct ptoc_Type11
    {
    tsp00_C2 ci2_gg00;
    } tgg91_FileVersion;
typedef
  tgg90_Cint4 tgg91_SessionNo;
typedef
  struct ptoc_Type12
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
  unsigned char tgg00_Distinct;
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
  unsigned char tgg00_StackEntryType;
typedef
  unsigned char tgg00_StackOpType;
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
  struct ptoc_Type12 tgg00_UserRef;
typedef
  struct ptoc_Type13
    {
    tsp00_C12 utidId_gg00;
    tsp00_Int4 utidLineNo_gg00;
    } tgg00_UtilCmdId;
typedef
  struct ptoc_Type14
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
  struct ptoc_Type15
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
  struct ptoc_Type16
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
  struct ptoc_Type17
    {
    tsp00_Int4 plTop_gg00;
    int plPno_gg00[10];
    } tgg00_PnoList;
typedef
  struct ptoc_Type18
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
  struct ptoc_Type19
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
    struct ptoc_Type20
      {
      tsp00_C1 td_internal;
      tsp00_C1 td_external;
      tsp00_C8 td_comment;
      tsp00_C2 td_filler;
      } term_desc[128];
    } tgg00_TermsetRecord;
typedef
  struct ptoc_Type21
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
  struct ptoc_Type22
    {
    tsp00_Int2 csi_cnt;
    tsp00_Int2 csi_fill1;
    tsp00_Int4 csi_fill2;
    struct ptoc_Type23
      {
      tgg00_Surrogate ocs_seq_surrogate;
      tsp00_Number ocs_seq_value;
      tsp00_Int4 ocs_seq_filler;
      } csi_values[32];
    } tgg00_CurrSeqInfo;
typedef
  tgg00_CurrSeqInfo *tgg00_CurrSeqInfoPtr;
typedef
  struct ptoc_Type24
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
  struct ptoc_Type25
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
  struct ptoc_Type26
    {
    tsp00_PageNo tpsPno_gg00;
    tsp00_Int4 tpsIndex_gg00;
    } tgg00_FilePos;
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
        tgg00_StackEntryType etype;
        tgg00_StackOpType eop;
        tsp00_Int2 epos;
        tsp00_Int2 elen_var;
        tsp00_C2 ecol_tab;
        } C_1;
      struct  
        {
        tgg00_StackEntryType efill2_type;
        unsigned char eop_func;
        tsp00_C2 epos_c2;
        tsp00_C2 var_elen;
        tsp00_Int2 ecol_pos;
        } C_2;
      struct  
        {
        tgg00_StackEntryType efill3_type;
        unsigned char eop_out;
        tsp00_Int2 ecolno;
        tsp00_Int4 eval4_var;
        } C_3;
      struct  
        {
        tgg00_StackEntryType efill4_type;
        unsigned char eop_build_in;
        unsigned char eformat;
        unsigned char edatatype;
        tsp00_C3 elanguage;
        tsp00_Uint1 elength;
        } C_4;
      struct  
        {
        tgg00_StackEntryType efill5_type;
        char eop_new_rec;
        } C_5;
      struct  
        {
        tgg00_StackEntryType efill6_type;
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
    tgg00_MessType mb_type;
    tgg00_MessType2 mb_type2;
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
  struct ptoc_Type48
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
  struct ptoc_Type49
    {
    tsp00_CodeType ctype;
    tsp00_CodeType char_default;
    unsigned char date_time;
    unsigned char kernel_swap;
    } tgg_code_globals;
typedef
  tgg00_DataPartPtr tgg_datapart_ptr;
typedef
  tgg00_Distinct tgg_distinct;
typedef
  unsigned char tgg_linkset[1];
typedef
  tgg00_Lkey tgg_lkey;
typedef
  struct tgg00_MessBlock tgg_mess_block;
typedef
  tgg00_MessBlockPtr tgg_mess_block_ptr;
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
  tgg00_Rec tgg_rec_buf;
typedef
  struct ptoc_Type50
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
  tgg00_StackDesc tgg_stack_desc;
typedef
  struct ptoc_Type39 tgg_stack_entry;
typedef
  tgg00_StackEntryType tgg_stack_entry_type;
typedef
  tgg00_StackList tgg_stack_list;
typedef
  tgg00_StackListPtr tgg_stack_list_ptr;
typedef
  tgg00_StackOpType tgg_stack_op_type;
typedef
  struct ptoc_Type51
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
  struct ptoc_Type52
    {
    tgg00_FileId file_id;
    tgg00_FileId inv_id;
    } tgg_two_tree_ids;
typedef
  unsigned char tak00_ooption[1];
typedef
  struct ptoc_Type53
    {
    struct ptoc_Type54
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
  struct ptoc_Type55
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
  unsigned char tak_complex_call_reason;
typedef
  unsigned char tak_progusagetyp;
typedef
  unsigned char tak_sc_symbol;
typedef
  unsigned char tak_ddl_descriptor;
typedef
  unsigned char tak_table_attribute_set[1];
typedef
  struct ptoc_Type56
    {
    tsp_c3 p_count;
    tsp_c1 p_id;
    tgg_message_type p_kind;
    tsp00_Uint1 p_no;
    tsp_int2 p_filler;
    } tak_parskey;
typedef
  unsigned char tak_special_node_info[1];
typedef
  struct ptoc_Type57
    {
    unsigned char n_proc;
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
  struct ptoc_Type58
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
  unsigned char tak_execution_kind;
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
  struct ptoc_Type39 tak_16stackentry[16];
typedef
  unsigned char tak_oms_version_id[22];
typedef
  unsigned char tak_sysbufferoffset[20];
typedef
  tsp00_C3 tak_pcountarr[100];
typedef
  struct ptoc_Type59
    {
    tsp_int4 rctxt_cnt;
    tsp00_C8 rctxt_roles[32768];
    } tak_role_context;
typedef
  tak_role_context *tak_role_context_ptr;
typedef
  unsigned char tak_map_set[768];
typedef
  struct ptoc_Type60
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
  struct ptoc_Type61
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
  struct ptoc_Type62
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
  struct ptoc_Type63
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
  struct ptoc_Type64
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
  struct ptoc_Type65
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
  struct ptoc_Type66
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
  struct ptoc_Type67
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
  struct ptoc_Type68
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
  struct ptoc_Type69
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
  struct ptoc_Type70
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
    tgg_distinct bv_distinct;
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
  struct ptoc_Type71
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
  struct ptoc_Type39 tak_ak_constr_stack[989];
typedef
  struct ptoc_Type72
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
  struct ptoc_Type73
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
  struct ptoc_Type74
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
  struct ptoc_Type75
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
    struct ptoc_Type76
      {
      tgg_surrogate ofp_procid;
      tsp_c3 ofp_language;
      unsigned char ofp_sqlmode[1];
      } fct_procids[64];
    } tak_funcrefrecord;
typedef
  struct ptoc_Type77
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
  struct ptoc_Type78
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
    struct ptoc_Type79
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
  struct ptoc_Type80
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
    struct ptoc_Type81
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
  struct ptoc_Type82
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
  struct ptoc_Type83
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
    struct ptoc_Type84
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
  struct ptoc_Type85
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
  struct ptoc_Type39 tak_trigger_stack[900];
typedef
  struct ptoc_Type86
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
  struct ptoc_Type87
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
    struct ptoc_Type88
      {
      tgg_surrogate usa_tableid;
      tsp_int2 usa_filler;
      unsigned char usa_tablekind;
      char usa_empty;
      } usagedef[672];
    } tak_usagerecord;
typedef
  struct ptoc_Type89
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
    struct ptoc_Type90
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
  struct ptoc_Type91
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
  struct ptoc_Type92
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
  struct ptoc_Type93
    {
    tak_sysbufferoffset ru_key_offset;
    tak_uref_restkey ru_rest_key;
    tsp_c2 ru_segmentid;
    tsp_int2 ru_filler;
    tgg_surrogate ru_user_id;
    } tak_userrefrecord;
typedef
  struct ptoc_Type94
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
  struct ptoc_Type95
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
  struct ptoc_Type96
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
  struct ptoc_Type97
    {
    tak_param_spec_ptr param_spec_ptr;
    tsp_int2 param_counter;
    tsp_int2 param_filler1;
    tsp_int4 param_filler2;
    } tak_param_list;
typedef
  struct ptoc_Type98
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
  struct ptoc_Type99
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
  struct ptoc_Type100
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
  struct ptoc_Type101
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
  struct ptoc_Type102
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
  struct ptoc_Type103
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
  struct ptoc_Type104
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
  struct ptoc_Type105
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
  struct ptoc_Type106
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
  struct ptoc_Type107
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
  struct ptoc_Type108
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
  struct ptoc_Type109
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
  struct ptoc_Type110
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
  struct ptoc_Type111
    {
    struct ptoc_Type112
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
    tgg_stack_op_type jo_op;
    char jo_no_join;
    tsp00_Uint1 jo_partno;
    tsp00_Uint1 jo_col_upd_stat;
    } tak_joinarr[257];
typedef
  struct ptoc_Type113
    {
    tak_joinarr jrc_joinarr;
    tsp_int2 jrc_cnt;
    char jrc_col_upd;
    tsp00_Uint1 jrc_filler;
    } tak_joinrec;
typedef
  unsigned char tak_viewtext[8064];
typedef
  struct ptoc_Type114
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
        struct ptoc_Type115
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
  struct ptoc_Type116
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
    struct ptoc_Type117
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
  struct ptoc_Type118
    {
    tgg_surrogate vttableid;
    tsp_int2 vtfiller;
    char vtins_for_upd;
    char vtone_to_one;
    tak_charset vtnot_used_links;
    } tak_viewtabarr[183];
typedef
  struct ptoc_Type119
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
  struct ptoc_Type39 tak_view_stack_entries[1007];
typedef
  unsigned char tak_viewbuf[8056];
typedef
  struct ptoc_Type120
    {
    tsp_int2 drecno;
    tsp_int2 dextno;
    tsp_int2 dfiller;
    tsp00_Uint1 dexttabno;
    tsp00_Uint1 dtabno;
    } tak_derived_col_info[1009];
typedef
  struct ptoc_Type121
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
  struct ptoc_Type122
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
    tsp_int2 vderivedcnt;
    tak_derived_col_info vderived_cols;
    } tak_viewqual_derivedcol_record;
typedef
  struct ptoc_Type125
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
  struct ptoc_Type126
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
    struct ptoc_Type127
      {
      tsp_int4 cst_dist_values;
      tsp_int4 cst_pages;
      tsp_int4 cst_avg_lst_len;
      } sti_col_stats[660];
    } tak_statisticsinfo;
typedef
  struct ptoc_Type128
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
    tgg_distinct bv_distinct;
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
  struct ptoc_Type129
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
    struct ptoc_Type130
      {
      tsp_knl_identifier str_name;
      tgg00_SubtransNo str_no;
      } str_subtrans[50];
    } tak_subtransrecord;
typedef
  struct ptoc_Type131
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
  struct ptoc_Type132
    {
    tsp_int2 ch_startpos;
    unsigned char ch_length;
    unsigned char ch_type[1];
    } tak_column_change_arr[1024];
typedef
  struct ptoc_Type133
    {
    tsp_int2 cr_colcount;
    char cr_was_opt;
    char cr_filler;
    tak_column_change_arr cr_columns;
    } tak_changerecord;
typedef
  struct ptoc_Type134
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
  struct ptoc_Type135
    {
    tsp_int2 ks_pos;
    tsp_int2 ks_len;
    } tak_keyspec;
typedef
  struct ptoc_Type136
    {
    tak_keyspec reckeyspec;
    tak_keyspec listkeyspec;
    } tak_two_keyspecs;
typedef
  unsigned char tak_res_keysbuf[13316];
typedef
  struct ptoc_Type137
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
  struct ptoc_Type138
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
  struct ptoc_Type139
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
  struct ptoc_Type140
    {
    tsp1_param_opt_set sp1i_mode;
    unsigned char sp1i_io_type;
    unsigned char sp1i_data_type;
    tsp_int1 sp1i_frac;
    tsp_int2 sp1i_length;
    tsp_int2 sp1i_in_out_len;
    tsp_int4 sp1i_bufpos;
    } tak_paraminfoarr[2700];
typedef
  struct ptoc_Type141
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
  struct ptoc_Type142
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
  struct ptoc_Type143
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
  struct ptoc_Type144
    {
    tsp_int2 all_lock_count;
    tsp_int2 all_lock_fill;
    tgg00_Lock all_locks[240];
    } tak_ak_all_locks;
typedef
  struct ptoc_Type145
    {
    unsigned char fp_kind;
    tgg_stack_entry_type fp_etype;
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
    } tak_parser_fields[2025];
typedef
  struct ptoc_Type146
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
  struct ptoc_Type147
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
    tak_ddl_descriptor scr_ddl;
    tsp_sqlmode scr_sql_mode;
    tak_stored_cmd_buf scr_buf;
    } tak_stored_cmd_record;
typedef
  struct ptoc_Type148
    {
    tak_parskey uparsid;
    tsp_int2 ufirstsel;
    tsp_int2 usecondsel;
    tsp_int2 ufiller;
    unsigned char uis_kind;
    char uis_all;
    } tak_union_array[493];
typedef
  struct ptoc_Type149
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
  struct ptoc_Type150
    {
    tak_parskey cparsk;
    tsp_int2 cnextpars;
    tak_complex_infoset centryinfo;
    unsigned char cqual_jv_upd;
    } tak_complex_array[669];
typedef
  struct ptoc_Type151
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
  struct ptoc_Type152
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
  unsigned char tak_cmd_part_type;
typedef
  struct ptoc_Type153
    {
    tsp_int2 cfieldno;
    tsp_int2 cnode;
    tak_corlevel clevel;
    tsp_int2 csetnode;
    tsp00_Uint1 ccntcol;
    tsp00_Uint1 ctabno;
    tak_cmd_part_type ccmd_part;
    tsp00_Uint1 cfiller1;
    tsp_int2 cfiller2;
    } tak_all_corcolumns[64];
typedef
  struct ptoc_Type154
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
    struct ptoc_Type155
      {
      tsp00_Uint1 socoldtabno;
      tsp00_Uint1 socselecttabno;
      tsp00_Uint1 socfields;
      tsp00_Uint1 socfill;
      } tc_tables[16];
    tak_all_corcolumns tc_columns;
    } tak_correcord;
typedef
  struct ptoc_Type156
    {
    tsp_int2 lpos;
    tsp_int2 llen;
    } tak_linkposinfo[2048];
typedef
  struct ptoc_Type157
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
  struct ptoc_Type158
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
  struct ptoc_Type159
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
  struct ptoc_Type160
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
  struct ptoc_Type161
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
  unsigned char tak_commandkind;
typedef
  tgg00_TransState tak_site_states[8][256];
typedef
  tak_site_states *tak_site_state_ptr;
typedef
  struct ptoc_Type162
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
  struct ptoc_Type163
    {
    tsp_knl_identifier dbp_sysdba;
    tgg00_FileId dbp_tree;
    } tak_dbproc_array[5];
typedef
  struct ptoc_Type164
    {
    tsp_int2 dbc_cnt;
    tsp_int2 dbc_fil;
    tak_sysbufferaddress dbc_ptr;
    tak_dbproc_array dbc_cache;
    } tak_dbproc_cache;
typedef
  struct ptoc_Type165
    {
    char o_do_optimize;
    unsigned char o_query_type;
    tsp_int2 o_mess_type;
    tak_execution_kind o_ex_kind;
    char o_optimized;
    tsp_int2 o_filler1;
    tgg00_FileId o_cmd_tree;
    tsp_c8 o_filler3;
    } tak_optimize_info;
typedef
  struct ptoc_Type166
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
  struct ptoc_Type167
    {
    tsp_int4 cst_top;
    tsp_moveobj_ptr cst_stack[16];
    } tak_cmd_stack;
typedef
  struct ptoc_Type168
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
  struct ptoc_Type169
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
  struct ptoc_Type170
    {
    tsp00_Int4 ai_uktId;
    tsp00_Int4 ai_badAllocCountDown;
    tak_AllocatorMonitorInfo ai_alloc_sys;
    tak_AllocatorMonitorInfo ai_alloc_usr;
    } tak_allocator_info;
typedef
  struct ptoc_Type171
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
    tak_execution_kind a_ex_kind;
    tsp00_Uint1 a_precomp_info_byte;
    tsp_int2 a_scv_index;
    tak_scanner_glob a_scv;
    char a_in_internal_subtr;
    tak_ddl_descriptor a_init_ddl;
    tsp_int2 a_count_variab;
    tak_commandkind a_command_kind;
    tak_ddl_descriptor a_is_ddl;
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
    tak_execution_kind a_init_ex_kind;
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
    tak_cmd_part_type a_cpart_type;
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
  unsigned char tak_lockenum;
typedef
  struct ptoc_Type172
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
  unsigned char tak_corr_type;
typedef
  struct ptoc_Type173
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
    tak_corr_type d_corr;
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
    tgg_distinct d_distinct;
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
  struct ptoc_Type174
    {
    tsp_knl_identifier exp_user;
    tsp_knl_identifier exp_table;
    tsp_knl_identifier exp_column;
    tsp_c1 exp_only_index;
    tsp_c1 exp_distinct_opt;
    tsp_c1 exp_temp_inv;
    tsp_c1 exp_more_qual;
    tsp_c40 exp_strat;
    tsp_int4 exp_pagecount;
    } tak_explain_rec;
typedef
  struct ptoc_Type175
    {
    tgg00_FileId sel_res_tree;
    tsp_knl_identifier sel_res_name;
    tsp_int2 sel_output_n;
    tsp_int2 sel_into_n;
    tsp_int2 sel_from_n;
    tsp_int2 sel_group_n;
    tsp_int2 sel_having_n;
    tsp_int2 sel_order_n;
    tsp_int2 sel_res_b;
    tsp_int2 sel_parinf_b;
    char sel_is_not_corr_search;
    tgg_distinct sel_new_atdistinct;
    char sel_last_pars_part;
    char sel_where_done;
    char sel_view_done;
    char sel_has_subquery;
    } tak_select_record;
typedef
  struct ptoc_Type176
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
  struct ptoc_Type177
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
  short tgg07_ColPosArr[20];
typedef
  tsp00_C1 tgg07_StrRaw[236];
typedef
  struct ptoc_Type178
    {
    tgg07_ColPosArr skir_keystart;
    tgg07_ColPosArr skir_keystop;
    tsp00_Int2 skir_IN_SUBQ_stpos;
    tsp00_Int2 skir_inelemcnt;
    } tgg07_StrKeyInRange;
typedef
  unsigned char tgg07_InvColSet[2];
typedef
  struct ptoc_Type179
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
  struct ptoc_Type180
    {
    tgg07_ColPosArr sji_keystart;
    tgg07_ColPosArr sji_keystop;
    tsp00_PageNo sji_invroot;
    tsp00_Int2 sji_invindex;
    tsp00_Int2 sji_jfiller;
    } tgg07_StrJoinInv;
typedef
  struct ptoc_Type181
    {
    tgg07_ColPosArr sjmf_keystart;
    tgg07_ColPosArr sjmf_keystop;
    tsp00_PageNo sjmf_invroot;
    tsp00_Int2 sjmf_index_no;
    tsp00_Int2 sjmf_cntfields;
    } tgg07_StrJoinMultFields;
typedef
  struct ptoc_Type182
    {
    tsp00_Int2 sjv_lastkeylen;
    tsp00_Int2 sjv_minkeylen;
    } tgg07_StrJoinViewkey;
typedef
  struct ptoc_Type183
    {
    unsigned char scat_show_kind;
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
  struct ptoc_Type184
    {
    unsigned char sr_strategy;
    char sr_must_result;
    char sr_use_rowno;
    char sr_use_subquery;
    char sr_distinct_bytes;
    char sr_fill1;
    tsp_int2 sr_invkeylen;
    } tak70_strat_rec;
typedef
  struct ptoc_Type185
    {
    tsp_int2 start;
    tsp_int2 length;
    } where_array[4096];
typedef
  where_array *where_arr_ptr;
typedef
  struct ptoc_Type186
    {
    tgg_sysinfokey ke;
    tgg_sysinfokey param_parskey;
    int hinoutpos;
    int hkeylen;
    int hreclen;
    char const_par_expr;
    char first_res;
    char h_use_rowno;
    tgg_distinct hd_distinct;
    char hd_single;
    int hd_colcount;
    int hd_rowno;
    char hdm_union;
    char messbuf_stored;
    tsp00_Uint1 lowpars;
    tgg_message2_type m2type;
    tsp_int2 old_subcntlevel;
    tgg00_FileId last_res_tree;
    tgg_mess_block_ptr hmbp;
    tgg_qual_buf_ptr hqbp;
    tak_sysbufferaddress hbuf;
    } store_for_endpart;
tgg_sysinfokey a01sysnullkey;
tgg_sysinfokey a01defaultkey;
tak_columnset a01fullset;
tsp_knl_identifier a01_i_tablekey;
tsp_knl_identifier a01_i_temp;
tsp_knl_identifier a01_il_b_identifier;
tsp_knl_identifier a01_zero_res_name;
tsp_knl_identifier a01_into_res_name;
tgg00_FileId b01niltree_id;
tgg_code_globals g01code;
tgg_serverdb_no g01localsite;
tgg_stack_desc g01nil_stack_desc;
char g01unicode;
int a663get_result_records();
int a663_get_result_info();
int a663_make_res_buf();
int a663restable_get();
char a663parse_for_execute();
int a663_intern_resname_drop();
int a05_unsigned_int2_get();
int a05identifier_get();
int a06determine_username();
int a06finish_curr_retpart();
int a06retpart_move();
int a06a_mblock_init();
int a06cpy_mblock();
char a06_table_exist();
int a06get_username();
int a06inc_linkage();
int a06get_priv();
int a061app_columnname();
char a061exist_columnname();
int a061td_colinfo();
int a07_b_put_error();
int a07_const_b_put_error();
int a07_kw_put_error();
int a07_nb_put_error();
short a07_return_code();
int a07put_error_pos();
int a07ak_system_error();
int a10mblock_into_cache();
int a10_copy_catalog_rec();
int a10repl_sysinfo();
int a10_nil_get_sysinfo();
int a10_key_del();
int a10del_sysinfo();
int a10_rel_sysinfo();
int a10get_sysinfo();
int a10add_sysinfo();
int a10prefix_parsid_delete();
int a10new();
int a10dispose();
int a10rel_sysinfo();
int a16col_to_view_description();
int a38_add_progusage();
int a40add_explain_record();
int a501get_resname();
int a502destroy_file();
int a502empty_result();
int a54_dml_init();
int a54_get_pparsp_pinfop();
int a54_select_last_part();
int a54_fixedpos();
int a54complicated_view();
int a54set_complex_entry();
int a54add_next_temp_lock();
int a54_loc_temp_locks();
tsp1_segment_ptr a542initial_cmd_segm();
int a55realloc_parsinfo();
int a60_get_longinfobuffer();
int a61_check_val_params();
int a61_search_table();
int a61_update_column();
int a61_set_jump();
int a61_put_last_func();
int a65_search_condition();
int a65_set_operator();
int a661get_from_select_table();
int a661a_subquery();
int a661_build_t_fromsel_tableid();
int a661_fdelete_fromtab_results();
int a661exec_sub();
int a662realloc_new_unionrec();
int a662_union_select();
int a662_start_union_select();
int a662_recursive_select();
int a664new_optimize_info();
int a67_s_sel_second_part();
int a67_update_atinoutpos();
int a67_sel2_second_part();
int a67_sel3_second_part();
int a67_put_all_views_into();
int a67_first_corr();
int a67_corr_search();
int a70_strategy_search();
char a680is_outer_predicate();
int a681check_outer_join();
int a683_output();
int a73_close_execute();
char a80_hint_exists();
int b07cget_record();
int b07cnext_record();
int g04check_if_top_level();
int g04build_temp_tree_id();
int g10mv1();
int g10mv2();
int g10mv3();
int g10mv4();
int g10mv6();
int g10mvr();
int g10fil1();
int s10mv1();
int s10mv2();
int t01knl_identifier();
int t01corr_type();
int t01execution_kind();
int t01bool();
int t01command_kind();
int t01messblock();
int t01name();
int t01lidentifier();
int t01int4();
int t01treeid();
int t01stqual();
int t01buf1();
int s30klen();
int s30unilnr();
int s80uni_trans();
int cccprint();
int ak660check_update_columns();
int ak660close_used_resultname();
int ak660describe_resultname();
int ak660endpart();
int ak660join_query_execute();
int ak660keycolumn();
int ak660later_output();
int ak660move_in_hbuf();
int ak660order_by();
int ak660qual_part_transfer();
int ak660subquery();
int ak660three_phase();
int ak660one_phase();
int ak660more_phase();
int ak660two_phase();
int ak660two_repl_complex_output();
int ak660where_transfer();
int a660fsame_pred();
int a660same_pred();
int a660lower_pred();
int a660pos_pred();
int a660_explain();
int a660_first_sel_part();
int a660_from_part();
int a660_isolevel();
int a660_lock();
int a660_lowpars_pparsp();
int a660_new_pparsp();
int a660_prefix_delete();
int a660_query_execute();
int a660_search_one_table();
int a660_select();
int a660_short_first_part();
int a660build_view_treeid();
int a660construct_subtree();
int a660select();
int ak660syn_owner();
int ak660put_tablename();
int ak660one_identifier();
int ak660put_sep();

static
  tak_columnset ptoc_Var0 = 
  {
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0'
  };


int ak660check_update_columns(acv, dmli, update_n)
tak_all_command_glob *acv;
tak_dml_info *dmli;
int update_n;
    {
    int curr_n;
    tak_all_command_glob *ptoc_Var1;
    tak_dml_info *ptoc_Var2;
    struct ptoc_Type57 *ptoc_Var3;
    int ptoc_Var4;
    struct ptoc_Type57 *ptoc_Var5;
    struct ptoc_Type57 *ptoc_Var6;
    tak_columnset ptoc_Var7;
    cccprint("AK660 ak660check");
    curr_n = update_n;
    ptoc_Var1 = &*acv;
    ptoc_Var2 = &*dmli;
    _ptoc_MOVE(unsigned char , 130, ptoc_Var2->d_foundset, ptoc_Var0);
    while (curr_n != 0 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var1->a_return_segm , "vak660.p" , 14915)).integer.C_4.sp1r_returncode == 0)
        {
        _ptoc_ASSIGN(unsigned char , 64, ptoc_Var2->d_column, a01_il_b_identifier)
        if ((int ) ptoc_Var1->a_comp_type == 7 || (int ) ptoc_Var1->a_comp_type == 5)
            {
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var2->d_user, a01_il_b_identifier)
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var2->d_table, a01_il_b_identifier)
            ptoc_Var3 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var1->a_ap_tree , "vak660.p" , 14923))[sql__setrang(curr_n , 0 , 32767)];
            if ((ptoc_Var4 = (int ) ptoc_Var3->n_symb) == 44 || ptoc_Var4 == 43)
                {
                ptoc_Var2->d_vppos = (*(tak_ap_max_tree *) sql__nil(ptoc_Var1->a_ap_tree , "vak660.p" , 14927))[sql__setrang(curr_n , 0 , 32767)].n_pos;
                ptoc_Var2->d_n_pos = ptoc_Var2->d_vppos;
                a06get_username(&(*acv) , &curr_n , ptoc_Var2->d_user);
                ptoc_Var5 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var1->a_ap_tree , "vak660.p" , 14930))[sql__setrang(curr_n , 0 , 32767)];
                if ((int ) ptoc_Var5->n_symb == 44)
                    {
                    a05identifier_get(&(*acv) , curr_n , sizeof(unsigned char [64]) , ptoc_Var2->d_table);
                    curr_n = (int ) ptoc_Var5->n_sa_level;
                    }
                a61_search_table(&(*acv) , &(*dmli));
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var1->a_return_segm , "vak660.p" , 14946)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var6 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var1->a_ap_tree , "vak660.p" , 14948))[sql__setrang(curr_n , 0 , 32767)];
            a05identifier_get(&(*acv) , curr_n , sizeof(unsigned char [64]) , ptoc_Var2->d_column);
            if (a061exist_columnname(&(*(struct tak_systembuffer *) sql__nil(ptoc_Var2->d_sparr.pbasep , "vak660.p" , 14952)).integer.C_1.sbase , ptoc_Var2->d_column , &ptoc_Var2->d_colbuf))
                {
                if (1 && ((*(tak00_columninfo *) sql__nil(ptoc_Var2->d_colbuf , "vak660.p" , 14956)).integer.C_1.ccolpropset[1] & 2) != 0 || !(char ) sql__setrang(ptoc_Var2->d_tabarr[sql__setrang((int ) ptoc_Var2->d_acttabindex , 1 , 64) + -1].oall_priv , 0 , 1) && ((int ) (*(tak00_columninfo *) sql__nil(ptoc_Var2->d_colbuf , "vak660.p" , 14958)).integer.C_1.cextcolno < 0 || (int ) (*(tak00_columninfo *) sql__nil(ptoc_Var2->d_colbuf , "vak660.p" , 14958)).integer.C_1.cextcolno > 1024 || (ptoc_Var2->d_tabarr[sql__setrang((int ) ptoc_Var2->d_acttabindex , 1 , 64) + -1].osetallpriv[(int ) (*(tak00_columninfo *) sql__nil(ptoc_Var2->d_colbuf , "vak660.p" , 14958)).integer.C_1.cextcolno >> 3] & 1 << ((int ) (*(tak00_columninfo *) sql__nil(ptoc_Var2->d_colbuf , "vak660.p" , 14958)).integer.C_1.cextcolno & 7)) == 0))
                    {
                    a07_nb_put_error(&(*acv) , 6492 , ptoc_Var6->n_pos , ptoc_Var2->d_column);
                    }
                else
                    {
                    sql__psetadd(ptoc_Var2->d_foundset , ptoc_Var2->d_foundset , sql__pcnst(ptoc_Var7 , 0 , 1024 , 0 , 1 , (int ) (*(tak00_columninfo *) sql__nil(ptoc_Var2->d_colbuf , "vak660.p" , 14965)).integer.C_1.cextcolno) , sizeof(tak_columnset ));
                    }
                }
            else
                {
                a07_nb_put_error(&(*acv) , 6492 , ptoc_Var6->n_pos , ptoc_Var2->d_column);
                }
            curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var1->a_ap_tree , "vak660.p" , 14973))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
            }
        }
    cccprint("<ak660check_upda");
    }



int ak660close_used_resultname(acv)
tak_all_command_glob *acv;
    {
    int name_n;
    tsp_knl_identifier n;
    int mret;
    int merr;
    tak_execution_kind mex_kind;
    tak_syspointerarr sparr;
    tak_all_command_glob *ptoc_Var8;
    struct ptoc_Type57 *ptoc_Var9;
    cccprint("AK660 ak660close");
    ptoc_Var8 = &*acv;
    name_n = 0;
    do
        name_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var8->a_ap_tree , "vak660.p" , 15005))[sql__setrang(name_n , 0 , 32767)].n_lo_level;
    while (!((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var8->a_ap_tree , "vak660.p" , 15007))[sql__setrang(name_n , 0 , 32767)].n_proc == 54 && ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var8->a_ap_tree , "vak660.p" , 15008))[sql__setrang(name_n , 0 , 32767)].n_subproc == 1 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var8->a_ap_tree , "vak660.p" , 15009))[sql__setrang(name_n , 0 , 32767)].n_subproc == 4)));
    name_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var8->a_ap_tree , "vak660.p" , 15011))[sql__setrang(name_n , 0 , 32767)].n_lo_level;
    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var8->a_ap_tree , "vak660.p" , 15012))[sql__setrang(name_n , 0 , 32767)].n_proc == 54 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var8->a_ap_tree , "vak660.p" , 15013))[sql__setrang(name_n , 0 , 32767)].n_subproc == 2)
        {
        name_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var8->a_ap_tree , "vak660.p" , 15016))[sql__setrang(name_n , 0 , 32767)].n_lo_level;
        ptoc_Var8->a_union_append = 0;
        }
    _ptoc_ASSIGN(unsigned char , 64, n, a01_il_b_identifier)
    ptoc_Var9 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var8->a_ap_tree , "vak660.p" , 15021))[sql__setrang(name_n , 0 , 32767)];
    if ((int ) ptoc_Var9->n_symb == 42)
        {
        a05identifier_get(&(*acv) , name_n , sizeof(unsigned char [64]) , n);
        }
    mret = (int ) (*(tsp1_segment *) sql__nil(ptoc_Var8->a_return_segm , "vak660.p" , 15027)).integer.C_4.sp1r_returncode;
    merr = (*(tsp1_segment *) sql__nil(ptoc_Var8->a_return_segm , "vak660.p" , 15028)).integer.C_4.sp1r_errorpos;
    (*(tsp1_segment *) sql__nil(ptoc_Var8->a_return_segm , "vak660.p" , 15029)).integer.C_4.sp1r_returncode = 0;
    mex_kind = (unsigned char ) sql__setrang(ptoc_Var8->a_ex_kind , 0 , 4);
    ptoc_Var8->a_ex_kind = 2;
    a73_close_execute(&(*acv) , &sparr , n , ptoc_Var8->a_modul_name);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var8->a_return_segm , "vak660.p" , 15033)).integer.C_4.sp1r_returncode == (int ) a07_return_code(6516 , (unsigned char ) sql__setrang(ptoc_Var8->a_sqlmode , 0 , 3)))
        {
        (*(tsp1_segment *) sql__nil(ptoc_Var8->a_return_segm , "vak660.p" , 15037)).integer.C_4.sp1r_returncode = 0;
        (*(tsp1_segment *) sql__nil(ptoc_Var8->a_return_segm , "vak660.p" , 15038)).integer.C_4.sp1r_errorpos = 0;
        ptoc_Var8->a_err_parm_cnt = 0;
        }
    ptoc_Var8->a_ex_kind = (unsigned char ) sql__setrang(mex_kind , 0 , 4);
    (*(tsp1_segment *) sql__nil(ptoc_Var8->a_return_segm , "vak660.p" , 15043)).integer.C_4.sp1r_returncode = (short ) sql__setrang((short ) mret , -32768 , 32767);
    (*(tsp1_segment *) sql__nil(ptoc_Var8->a_return_segm , "vak660.p" , 15044)).integer.C_4.sp1r_errorpos = merr;
    cccprint("<ak660close_used");
    }

static
  tsp1_param_opt_set ptoc_Var10 = 
  {
  (unsigned char)'\001'
  };


int ak660describe_resultname(acv, dmli, sel_res_name, tree_index)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tsp_knl_identifier sel_res_name;
int tree_index;
    {
    int index;
    int len;
    tak_all_command_glob *ptoc_Var11;
    tak_dml_info *ptoc_Var12;
    tak_parsinforecord *ptoc_Var13;
    struct ptoc_Type145 *ptoc_Var14;
    tak_shortinforecord *ptoc_Var15;
    struct ptoc_Type140 *ptoc_Var16;
    cccprint("AK660 ak660descr");
    ptoc_Var11 = &*acv;
    ptoc_Var12 = &*dmli;
    _ptoc_ASSIGN(unsigned char , 64, sel_res_name, a01_il_b_identifier)
    if ((int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var12->d_sparr.pparsp , "vak660.p" , 15066)).integer.C_23.sparsinfo.integer.C_2.p_cnt_infos >= (int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var12->d_sparr.pparsp , "vak660.p" , 15067)).integer.C_23.sparsinfo.integer.C_2.p_max_infos)
        {
        a55realloc_parsinfo(&(*acv) , &ptoc_Var12->d_sparr.pparsp);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var11->a_return_segm , "vak660.p" , 15071)).integer.C_4.sp1r_returncode == 0)
        {
        len = sizeof(unsigned char [64]);
        if ((char ) sql__setrang(g01unicode , 0 , 1))
            {
            len = len / 2;
            }
        ptoc_Var13 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var12->d_sparr.pparsp , "vak660.p" , 15079)).integer.C_23.sparsinfo;
        ptoc_Var13->integer.C_2.p_cnt_infos = (short ) sql__setrang((short ) ((int ) ptoc_Var13->integer.C_2.p_cnt_infos + 1) , -32768 , 32767);
        ptoc_Var14 = &ptoc_Var13->integer.C_2.p_pars_infos[sql__setrang((int ) ptoc_Var13->integer.C_2.p_cnt_infos , 1 , 2025) + -1];
        ptoc_Var14->fp_kind = 2;
        ptoc_Var14->tak_fp_kind_type.C_fp_result_name.fp_fromposv14 = ptoc_Var11->a_input_data_pos;
        ptoc_Var14->tak_fp_kind_type.C_fp_result_name.fp_datalenv14 = (short ) sql__setrang((short ) len , -32768 , 32767);
        if ((char ) sql__setrang(g01unicode , 0 , 1))
            {
            ptoc_Var14->tak_fp_kind_type.C_fp_result_name.fp_dataiolenv14 = (short ) sql__setrang((short ) (len * 2 + 1) , -32768 , 32767);
            }
        else
            {
            ptoc_Var14->tak_fp_kind_type.C_fp_result_name.fp_dataiolenv14 = (short ) sql__setrang((short ) (len * (int ) ptoc_Var11->a_max_codewidth + 1) , -32768 , 32767);
            }
        index = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 15097))[sql__setrang(tree_index , 0 , 32767)].n_length;
        ptoc_Var15 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var12->d_sparr.pinfop , "vak660.p" , 15098)).integer.C_31.sshortinfo;
        ptoc_Var16 = &ptoc_Var15->siinfo[sql__setrang(index , 1 , 2700) + -1];
        if (index > (int ) ptoc_Var15->sicount)
            {
            ptoc_Var15->sicount = (short ) sql__setrang((short ) index , -32768 , 32767);
            }
        if ((char ) sql__setrang(g01unicode , 0 , 1))
            {
            ptoc_Var16->sp1i_data_type = 24;
            }
        else
            {
            if ((int ) g01code.ctype == 1)
                {
                ptoc_Var16->sp1i_data_type = 3;
                }
            else
                {
                ptoc_Var16->sp1i_data_type = 2;
                }
            }
        _ptoc_MOVE(unsigned char , 1, ptoc_Var16->sp1i_mode, ptoc_Var10);
        ptoc_Var16->sp1i_io_type = 0;
        ptoc_Var16->sp1i_frac = (unsigned char)'\0';
        ptoc_Var16->sp1i_length = (short ) sql__setrang((short ) len , -32768 , 32767);
        if ((char ) sql__setrang(g01unicode , 0 , 1))
            {
            ptoc_Var16->sp1i_in_out_len = (short ) sql__setrang((short ) (len * 2 + 1) , -32768 , 32767);
            }
        else
            {
            ptoc_Var16->sp1i_in_out_len = (short ) sql__setrang((short ) (len * (int ) ptoc_Var11->a_max_codewidth + 1) , -32768 , 32767);
            }
        ptoc_Var16->sp1i_bufpos = ptoc_Var11->a_input_data_pos;
        ptoc_Var11->a_input_data_pos = ptoc_Var11->a_input_data_pos + (int ) ptoc_Var16->sp1i_in_out_len;
        }
    cccprint("<ak660describe_r");
    }

typedef
  struct ptoc_Type187
    {
    tgg00_FileId str_result_id;
    unsigned char str_strategy;
    char str_build_result;
    tgg00_Distinct str_distinc;
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
    } tgg07_StrategyInfo_ak660endpart;


int ak660endpart(acv, dmli, sfe_rec, order_n, last_pars_part, store_infos, use_lowpars, res_tree, select_rec, sr_rec)
tak_all_command_glob *acv;
tak_dml_info *dmli;
store_for_endpart *sfe_rec;
int order_n;
char last_pars_part;
char store_infos;
char use_lowpars;
tgg00_FileId *res_tree;
tak_select_record *select_rec;
tak70_strat_rec *sr_rec;
    {
    tgg_basis_error b_err;
    char store_parsinfos;
    int aux_data_len;
    int i;
    int dropped_pinfos;
    tak_sysbufferaddress hbuf;
    tak_sysbufferaddress param_parsbptr;
    tak_commandkind m_command_kind;
    tgg07_StrategyInfo_ak660endpart *strat_ptr;
    tak_all_command_glob *ptoc_Var17;
    tak_dml_info *ptoc_Var18;
    store_for_endpart *ptoc_Var19;
    struct tgg00_MessBlock *ptoc_Var20;
    tgg00_FileId *ptoc_Var21;
    tak_parsinforecord *ptoc_Var22;
    int ptoc_Var23;
    struct ptoc_Type145 *ptoc_Var24;
    cccprint("AK660 ak660endpa");
    ptoc_Var17 = &*acv;
    ptoc_Var18 = &*dmli;
    ptoc_Var19 = &*sfe_rec;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var17->a_return_segm , "vak660.p" , 15160)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var18->d_inoutpos = (short ) sql__setrang((short ) ptoc_Var19->hinoutpos , -32768 , 32767);
        ptoc_Var18->d_keylen = (short ) sql__setrang((short ) ptoc_Var19->hkeylen , -32768 , 32767);
        ptoc_Var18->d_reclen = (short ) sql__setrang((short ) ptoc_Var19->hreclen , -32768 , 32767);
        ptoc_Var18->d_use_order = (char ) sql__setrang(order_n != 0 , 0 , 1);
        if ((_ptoc_MEMCMP(unsigned char , 7, (*(tgg00_QualBuf *) sql__nil(ptoc_Var17->a_mblock.integer.C_4.mb_qual , "vak660.p" , 15167)).integer.C_4.mr_resnum , "\0\200\0\0\0\0\0") == 0 ? 0 : 1) || (int ) ptoc_Var17->a_ex_kind == 0 || (int ) ptoc_Var19->m2type == 67)
            {
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var17->a_return_segm , "vak660.p" , 15172)).integer.C_4.sp1r_returncode == 0)
                {
                a10get_sysinfo(&(*acv) , &ptoc_Var19->ke , 1 , &ptoc_Var19->hbuf , &b_err);
                if ((int ) b_err != 0)
                    {
                    a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                    }
                }
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var17->a_return_segm , "vak660.p" , 15182)).integer.C_4.sp1r_returncode == 0)
                {
                ptoc_Var20 = &ptoc_Var17->a_mblock;
                ptoc_Var19->hmbp = (struct tgg00_MessBlock *) &(*(struct tak_systembuffer *) sql__nil(ptoc_Var19->hbuf , "vak660.p" , 15186)).integer.C_62.smessblock.mbr_mess_block;
                ptoc_Var19->hqbp = (*(struct tgg00_MessBlock *) sql__nil(ptoc_Var19->hmbp , "vak660.p" , 15187)).integer.C_4.mb_qual;
                aux_data_len = ptoc_Var17->a_mblock.integer.C_2.mb_data_len;
                ptoc_Var17->a_mblock.integer.C_2.mb_data_len = 0;
                a06cpy_mblock(&(*acv) , sql__nil(ptoc_Var19->hmbp , "vak660.p" , 15190) , &ptoc_Var17->a_mblock , &b_err);
                ptoc_Var17->a_mblock.integer.C_2.mb_data_len = aux_data_len;
                if ((int ) b_err != 0)
                    {
                    a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                    }
                else
                    {
                    t01messblock(5 , "mess_block <" , sql__nil(ptoc_Var19->hmbp , "vak660.p" , 15197));
                    t01messblock(5 , "MESSBLOCK   " , &(*acv).a_mblock);
                    (*acv).a_mblock.integer.C_2.mb_struct = 9;
                    (*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak660.p" , 15200)).integer.C_2.mtree = *res_tree;
                    }
                }
            (*sr_rec).sr_must_result = 1;
            a70_strategy_search(&(*acv) , &(*dmli) , &ptoc_Var19->last_res_tree , &(*sr_rec));
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var17->a_return_segm , "vak660.p" , 15208)).integer.C_4.sp1r_returncode == 0)
                {
                store_parsinfos = 1;
                if ((char ) sql__setrang(ptoc_Var17->a_insert_select , 0 , 1) && (int ) ptoc_Var17->a_intern_select_cnt == (int ) ptoc_Var17->a_max_intern_select && !(char ) sql__setrang(ptoc_Var18->d_subquery , 0 , 1) && !(char ) sql__setrang(ptoc_Var17->a_from_select , 0 , 1) && (int ) ptoc_Var17->a_fromsel_n == 0)
                    {
                    if ((int ) ptoc_Var17->a_mblock.integer.C_2.mb_type2 != 67)
                        {
                        ptoc_Var21 = &(*(tgg00_QualBuf *) sql__nil(ptoc_Var17->a_mblock.integer.C_4.mb_qual , "vak660.p" , 15220)).integer.C_2.mtree;
                        if (1 && (ptoc_Var21->integer.C_1.fileType_gg00[0] & 1) != 0 && (int ) ptoc_Var21->integer.C_4.fileTfn_gg00 == 26 && (int ) ptoc_Var21->integer.C_8.fileTfnTemp_gg00 != 4)
                            {
                            store_parsinfos = 1;
                            }
                        else
                            {
                            strat_ptr = (tgg07_StrategyInfo_ak660endpart *) &(*(tsp00_MoveObj *) sql__nil((*acv).a_mblock.integer.C_11.mb_strat , "vak660.p" , 15229))[sql__setrang((int ) (*(tgg00_StackList *) sql__nil((*acv).a_mblock.integer.C_5.mb_st , "vak660.p" , 15230))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak660.p" , 15230)).integer.C_2.mstrat_pos , 1 , 32767) + -1].integer.C_1.epos , 1 , 8192000) + -1];
                            if ((int ) (*(struct ptoc_Type187 *) sql__nil(strat_ptr , "vak660.p" , 15231)).str_distinc == 0 && !(char ) sql__setrang((*(struct ptoc_Type187 *) sql__nil(strat_ptr , "vak660.p" , 15233)).str_use_rowno , 0 , 1) && (int ) (*(struct ptoc_Type187 *) sql__nil(strat_ptr , "vak660.p" , 15235)).str_qual_kind != 1 && ((int ) (*(struct ptoc_Type187 *) sql__nil(strat_ptr , "vak660.p" , 15237)).str_result_id.integer.C_4.fileTfn_gg00 != 26 || (int ) (*(struct ptoc_Type187 *) sql__nil(strat_ptr , "vak660.p" , 15239)).str_result_id.integer.C_8.fileTfnTemp_gg00 != 1))
                                {
                                store_parsinfos = 0;
                                }
                            }
                        }
                    }
                if ((char ) sql__setrang(store_parsinfos , 0 , 1))
                    {
                    if ((int ) ptoc_Var17->a_ex_kind == 0)
                        {
                        t01int4(5 , "d_corr      " , sql__ord((int ) ptoc_Var18->d_corr));
                        t01int4(5 , "d_lowpars   " , (int ) ptoc_Var18->d_lowpars);
                        t01int4(5 , "endpart_lowp" , 1);
                        if ((int ) ptoc_Var18->d_corr != 0 && (char ) sql__setrang(use_lowpars , 0 , 1))
                            {
                            a660_lowpars_pparsp(&(*acv) , &ptoc_Var18->d_sparr.pparsp , 0 , (char ) sql__setrang(!(char ) sql__setrang(ptoc_Var18->d_only_sem_check , 0 , 1) , 0 , 1) , (unsigned char ) sql__setrang((unsigned char ) ((int ) ptoc_Var19->lowpars + 1) , 0 , 255));
                            }
                        else
                            {
                            m_command_kind = (unsigned char ) sql__setrang(ptoc_Var17->a_command_kind , 0 , 11);
                            if ((char ) sql__setrang((*select_rec).sel_has_subquery , 0 , 1) && (int ) ptoc_Var17->a_command_kind == 2)
                                {
                                ptoc_Var17->a_command_kind = 4;
                                }
                            a660_new_pparsp(&(*acv) , &ptoc_Var18->d_sparr , (char ) sql__setrang((int ) ptoc_Var18->d_corr == 0 , 0 , 1) , (char ) sql__setrang((int ) ptoc_Var18->d_lowpars == 255 && ((char ) sql__setrang(ptoc_Var18->d_subquery , 0 , 1) && (int ) m_command_kind != 8 || (int ) m_command_kind == 5) , 0 , 1));
                            ptoc_Var17->a_command_kind = (unsigned char ) sql__setrang(m_command_kind , 0 , 11);
                            }
                        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var17->a_return_segm , "vak660.p" , 15283)).integer.C_4.sp1r_returncode == 0)
                            {
                            ptoc_Var22 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var18->d_sparr.pparsp , "vak660.p" , 15285)).integer.C_23.sparsinfo;
                            ptoc_Var22->integer.C_2.p_subcntlevel = (short ) sql__setrang(ptoc_Var19->old_subcntlevel , -32768 , 32767);
                            ptoc_Var22->integer.C_2.p_const_param_expr = (char ) sql__setrang(ptoc_Var19->const_par_expr , 0 , 1);
                            if ((int ) ptoc_Var18->d_corr != 0)
                                {
                                a10get_sysinfo(&(*acv) , &ptoc_Var19->param_parskey , 1 , &param_parsbptr , &b_err);
                                if ((int ) b_err != 0)
                                    {
                                    a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                                    }
                                else
                                    {
                                    dropped_pinfos = 0;
                                    ptoc_Var23 = (*(struct tak_systembuffer *) sql__nil(param_parsbptr , "vak660.p" , 15301)).integer.C_23.sparsinfo.integer.C_2.p_cnt_infos;
                                    if (1 <= ptoc_Var23)
                                        {
                                        i = 1;
                                        do
                                            {
                                            ptoc_Var22->integer.C_2.p_pars_infos[sql__setrang(i - dropped_pinfos , 1 , 2025) + -1] = (*(struct tak_systembuffer *) sql__nil(param_parsbptr , "vak660.p" , 15304)).integer.C_23.sparsinfo.integer.C_2.p_pars_infos[sql__setrang(i , 1 , 2025) + -1];
                                            ptoc_Var24 = &ptoc_Var22->integer.C_2.p_pars_infos[sql__setrang(i - dropped_pinfos , 1 , 2025) + -1];
                                            if ((int ) ptoc_Var24->fp_kind == 6)
                                                {
                                                if ((int ) ptoc_Var24->tak_fp_kind_type.C_fp_fill_part2_to_len.fp_len > ptoc_Var17->a_mblock.integer.C_2.mb_data_len)
                                                    {
                                                    dropped_pinfos = sql__succ(dropped_pinfos , -2147483647 , 2147483647 , "vak660.p" , 15310);
                                                    }
                                                }
                                            if (i == ptoc_Var23)
                                                {
                                                break;
                                                }
                                            i += 1;
                                            }
                                        while (!0);
                                        }
                                    ptoc_Var22->integer.C_2.p_cnt_infos = (short ) sql__setrang((short ) ((int ) (*(struct tak_systembuffer *) sql__nil(param_parsbptr , "vak660.p" , 15316)).integer.C_23.sparsinfo.integer.C_2.p_cnt_infos - dropped_pinfos) , -32768 , 32767);
                                    }
                                }
                            }
                        }
                    if ((char ) sql__setrang(store_infos , 0 , 1))
                        {
                        a54_select_last_part(&(*acv) , &(*dmli) , &ptoc_Var19->last_res_tree , (char ) sql__setrang(last_pars_part , 0 , 1));
                        }
                    }
                }
            }
        else
            {
            if (!(char ) sql__setrang(ptoc_Var18->d_single , 0 , 1))
                {
                a502empty_result(&(*acv) , &(*dmli) , &ptoc_Var19->last_res_tree);
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var17->a_return_segm , "vak660.p" , 15341)).integer.C_4.sp1r_returncode == 0)
                    {
                    (*(tgg00_QualBuf *) sql__nil(ptoc_Var17->a_mblock.integer.C_4.mb_qual , "vak660.p" , 15343)).integer.C_4.mr_restree = ptoc_Var19->last_res_tree;
                    }
                }
            }
        }
    cccprint("<ak660endpart   ");
    }



int ak660join_query_execute(acv, dmli, s_n, sr_rec, pseudo_resultset_select)
tak_all_command_glob *acv;
tak_dml_info *dmli;
int s_n;
tak70_strat_rec *sr_rec;
char *pseudo_resultset_select;
    {
    tgg_basis_error e;
    char m_acv_info_output;
    tak_execution_kind m_ex_kind;
    int cnt;
    tak_parskey parsk;
    tak_commandkind m_command_kind;
    tak_all_command_glob *ptoc_Var25;
    tak_dml_info *ptoc_Var26;
    cccprint("AK660 ak660join_");
    ptoc_Var25 = &*acv;
    ptoc_Var26 = &*dmli;
    a660_prefix_delete(&(*acv) , &ptoc_Var25->a_pars_last_key , &cnt , 4);
    e = 0;
    if (ptoc_Var26->d_esparr.pbasep != (struct tak_systembuffer *) (int *) 0)
        {
        a10del_sysinfo(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var26->d_esparr.pbasep , "vak660.p" , 15380)).integer.C_50.syskey , &e);
        ptoc_Var26->d_esparr.pbasep = (struct tak_systembuffer *) (int *) 0;
        }
    if ((int ) e != 0)
        {
        a07_b_put_error(&(*acv) , (short ) sql__setrang(e , -32768 , 32767) , 1);
        }
    else
        {
        if ((int ) ptoc_Var25->a_isolation_info != 2)
            {
            ptoc_Var25->a_isolation_info = 1;
            }
        m_command_kind = (unsigned char ) sql__setrang(ptoc_Var25->a_command_kind , 0 , 11);
        ptoc_Var25->a_command_kind = 3;
        m_acv_info_output = (char ) sql__setrang(ptoc_Var25->a_info_output , 0 , 1);
        t01int4(5 , "new m_info  " , sql__ord((int ) (char ) sql__setrang(m_acv_info_output , 0 , 1)));
        ptoc_Var25->a_info_output = 0;
        m_ex_kind = (unsigned char ) sql__setrang(ptoc_Var25->a_ex_kind , 0 , 4);
        ptoc_Var25->a_ex_kind = 0;
        *pseudo_resultset_select = (char ) sql__setrang((char ) sql__setrang(m_acv_info_output , 0 , 1) && (int ) m_ex_kind != 0 , 0 , 1);
        a660_query_execute(&(*acv) , &(*dmli) , s_n , (char ) sql__setrang(!(char ) sql__setrang(ptoc_Var25->a_from_select , 0 , 1) , 0 , 1) , 8 , &(*sr_rec) , &(*pseudo_resultset_select) , 1 , 1 , 0);
        a54set_complex_entry(&(*acv) , 2);
        parsk = ptoc_Var25->a_pars_last_key;
        parsk.p_id[0] = (unsigned char ) sql__setrang(ptoc_Var25->a_first_parskey , 0 , 255);
        parsk.p_kind = 10;
        parsk.p_no = (unsigned char)'\0';
        t01int4(5 , "s_n         " , s_n);
        t01int4(5 , "m_ex_kind   " , sql__ord((int ) m_ex_kind));
        t01int4(5 , "only_parsing" , sql__ord(0));
        t01int4(5 , "d_only_sem_c" , sql__ord((int ) (char ) sql__setrang(ptoc_Var26->d_only_sem_check , 0 , 1)));
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var25->a_return_segm , "vak660.p" , 15414)).integer.C_4.sp1r_returncode == 0 && !(char ) sql__setrang(ptoc_Var25->a_from_select , 0 , 1))
            {
            a54_loc_temp_locks(&(*acv) , ptoc_Var26->d_globstate , &ptoc_Var26->d_sparr);
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var25->a_return_segm , "vak660.p" , 15419)).integer.C_4.sp1r_returncode == 0 && ((int ) m_ex_kind != 0 && !(char ) sql__setrang(ptoc_Var26->d_only_sem_check , 0 , 1)))
            {
            a661exec_sub(&(*acv) , &(*dmli) , &parsk , (short ) sql__setrang((short ) s_n , -32768 , 32767) , &cnt , (char ) sql__setrang(m_acv_info_output , 0 , 1) , (char ) sql__setrang(*pseudo_resultset_select , 0 , 1));
            }
        else
            {
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var25->a_return_segm , "vak660.p" , 15425)).integer.C_4.sp1r_returncode != 0)
                {
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var25->a_return_segm , "vak660.p" , 15428)).integer.C_4.sp1r_returncode == (int ) a07_return_code(6226 , (unsigned char ) sql__setrang(ptoc_Var25->a_sqlmode , 0 , 3)))
                    {
                    if ((int ) ptoc_Var25->a_ex_kind != 0)
                        {
                        a660_prefix_delete(&(*acv) , &parsk , &cnt , 4);
                        }
                    (*(tak_ap_max_tree *) sql__nil(ptoc_Var25->a_ap_tree , "vak660.p" , 15437))[sql__setrang(s_n , 0 , 32767)].n_symb = 61;
                    }
                else
                    {
                    a660_prefix_delete(&(*acv) , &parsk , &cnt , 3);
                    }
                }
            }
        ptoc_Var25->a_ex_kind = (unsigned char ) sql__setrang(m_ex_kind , 0 , 4);
        t01int4(5 , "again m_info" , sql__ord((int ) (char ) sql__setrang(m_acv_info_output , 0 , 1)));
        ptoc_Var25->a_info_output = (char ) sql__setrang(m_acv_info_output , 0 , 1);
        ptoc_Var25->a_command_kind = (unsigned char ) sql__setrang(m_command_kind , 0 , 11);
        }
    cccprint("<ak660join_query");
    }

static
  tak00_colpropset ptoc_Var27 = 
  {
  (unsigned char)'\001', (unsigned char)'\002'
  };


int ak660keycolumn(acv, dmli)
tak_all_command_glob *acv;
tak_dml_info *dmli;
    {
    int ci;
    tsp_knl_identifier coln;
    tak00_columninfo *ptoc_Var28;
    struct ptoc_Type39 *ptoc_Var29;
    cccprint("AK660 ak660keyco");
    (*dmli).d_keylen = 4;
    if ((int ) (*dmli).d_distinct != 0 || (char ) sql__setrang((*dmli).d_union , 0 , 1))
        {
        (*dmli).d_keylen = (short ) sql__setrang((short ) ((int ) (*dmli).d_keylen + 4) , -32768 , 32767);
        }
    (*acv).a_union_append = (char ) sql__setrang((char ) sql__setrang((*acv).a_union_append , 0 , 1) && (int ) (*dmli).d_distinct == 0 , 0 , 1);
    (*dmli).d_inoutpos = (short ) sql__setrang((short ) ((int ) (*dmli).d_keylen + 9) , -32768 , 32767);
    if (((int ) (*acv).a_intern_select_cnt == (int ) (*acv).a_max_intern_select || (int ) (*acv).a_fromsel_n > 0 || (int ) (*acv).a_recursive_state == 2 || (*acv).a_union_cnt > 0) && (!(char ) sql__setrang((*dmli).d_view , 0 , 1) || (int ) (*dmli).d_phase_cnt >= 4))
        {
        _ptoc_ASSIGN(unsigned char , 64, coln, a01_i_tablekey)
        a061app_columnname(&(*acv) , &(*(struct tak_systembuffer *) sql__nil((*dmli).d_esparr.pbasep , "vak660.p" , 15485)).integer.C_1.sbase , coln , &ci);
        t01int4(5 , "ci          " , ci);
        ptoc_Var28 = (tak00_columninfo *) sql__nil((*(struct tak_systembuffer *) sql__nil((*dmli).d_esparr.pbasep , "vak660.p" , 15487)).integer.C_1.sbase.integer.C_2.bcolumn[sql__setrang(ci , 1 , 1536) + -1] , "vak660.p" , 15487);
        (*(struct tak_systembuffer *) sql__nil((*dmli).d_esparr.pbasep , "vak660.p" , 15489)).integer.C_1.sbase.bkeycolcount = 1;
        ptoc_Var28->integer.C_1.cextcolno = 1;
        ptoc_Var28->integer.C_1.creccolno = 1;
        ptoc_Var28->integer.C_1.cdatatyp = 3;
        _ptoc_MOVE(unsigned char , 2, ptoc_Var28->integer.C_1.ccolpropset, ptoc_Var27);
        ptoc_Var28->integer.C_1.cdatalen = (short ) sql__setrang((*dmli).d_keylen , -32768 , 32767);
        ptoc_Var28->integer.C_1.cdatafrac = (unsigned char)'\001';
        ptoc_Var28->integer.C_1.cinoutlen = (short ) sql__setrang(ptoc_Var28->integer.C_1.cdatalen , -32768 , 32767);
        ptoc_Var28->integer.C_2.cbinary = 0;
        ptoc_Var29 = &ptoc_Var28->integer.C_1.ccolstack;
        ptoc_Var29->integer.C_1.etype = 0;
        ptoc_Var29->integer.C_1.eop = 0;
        ptoc_Var29->integer.C_1.epos = 1;
        ptoc_Var29->integer.C_1.elen_var = (short ) sql__setrang(ptoc_Var28->integer.C_1.cdatalen , -32768 , 32767);
        ptoc_Var29->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
        ptoc_Var29->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
        }
    cccprint("<ak660keycolumn ");
    }



int ak660later_output(acv)
tak_all_command_glob *acv;
    {
    int stop;
    int i;
    struct tgg00_MessBlock *ptoc_Var30;
    tgg00_QualBuf *ptoc_Var31;
    cccprint("AK660 ak660later");
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 15528)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var30 = &(*acv).a_mblock;
        ptoc_Var31 = (tgg00_QualBuf *) sql__nil(ptoc_Var30->integer.C_4.mb_qual , "vak660.p" , 15530);
        i = (int ) ptoc_Var31->integer.C_2.mqual_pos;
        if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var30->integer.C_5.mb_st , "vak660.p" , 15533))[sql__setrang((int ) ptoc_Var31->integer.C_2.mqual_pos , 1 , 32767) + -1].integer.C_1.etype == 33)
            {
            stop = (int ) ptoc_Var31->integer.C_2.mqual_pos + (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var30->integer.C_5.mb_st , "vak660.p" , 15535))[sql__setrang((int ) ptoc_Var31->integer.C_2.mqual_pos , 1 , 32767) + -1].integer.C_1.epos + -2;
            }
        else
            {
            stop = (int ) ptoc_Var31->integer.C_2.mqual_pos;
            }
        while (i <= stop)
            {
            if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var30->integer.C_5.mb_st , "vak660.p" , 15541))[sql__setrang(i , 1 , 32767) + -1].integer.C_1.etype == 28)
                {
                if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var30->integer.C_5.mb_st , "vak660.p" , 15543))[sql__setrang(i , 1 , 32767) + -1].integer.C_3.eop_out == 4)
                    {
                    (*(tgg00_StackList *) sql__nil(ptoc_Var30->integer.C_5.mb_st , "vak660.p" , 15545))[sql__setrang(i + 1 , 1 , 32767) + -1].integer.C_3.eop_out = 6;
                    }
                }
            i = sql__succ(i , -2147483647 , 2147483647 , "vak660.p" , 15548);
            }
        }
    cccprint("<ak660later_outp");
    }



int ak660move_in_hbuf(acv, sfe_rec, no_of_hbuf, repl_only)
tak_all_command_glob *acv;
store_for_endpart *sfe_rec;
int no_of_hbuf;
char repl_only;
    {
    tgg_basis_error b_err;
    char init_msubquery;
    int i;
    int aux_data_len;
    tgg_mess_block_ptr hmbp;
    tgg_qual_buf_ptr hqbp;
    store_for_endpart *ptoc_Var32;
    struct tgg00_MessBlock *ptoc_Var33;
    tgg00_QualBuf *ptoc_Var34;
    tgg00_SysInfoKey *ptoc_Var35;
    struct tgg00_MessBlock *ptoc_Var36;
    tgg00_QualBuf *ptoc_Var37;
    tgg00_QualBuf *ptoc_Var38;
    cccprint("AK660 ak660move_");
    ptoc_Var32 = &*sfe_rec;
    ptoc_Var33 = &(*acv).a_mblock;
    ptoc_Var34 = (tgg00_QualBuf *) sql__nil(ptoc_Var33->integer.C_4.mb_qual , "vak660.p" , 15577);
    init_msubquery = (char ) sql__setrang(ptoc_Var34->integer.C_2.msubquery , 0 , 1);
    ptoc_Var34->integer.C_2.msubquery = 0;
    ptoc_Var33->integer.C_2.mb_qual_len = 112;
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 15584)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var35 = &ptoc_Var32->ke;
        ptoc_Var32->ke = a01sysnullkey;
        i = 1;
        do
            {
            ptoc_Var35->integer.C_3.sauthid[sql__setrang(i , 1 , 8) + -1] = (unsigned char)'\377';
            if (i == 8)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        _ptoc_ASSIGN(unsigned char , 2, ptoc_Var35->integer.C_2.sentrytyp, "\0\301")
        ptoc_Var35->integer.C_2.slinkage[1] = (unsigned char ) sql__setrang((unsigned char ) no_of_hbuf , 0 , 255);
        if ((char ) sql__setrang(repl_only , 0 , 1))
            {
            a10del_sysinfo(&(*acv) , &ptoc_Var32->ke , &b_err);
            }
        aux_data_len = (*acv).a_mblock.integer.C_2.mb_data_len;
        (*acv).a_mblock.integer.C_2.mb_data_len = 8;
        a10mblock_into_cache(&(*acv) , &ptoc_Var32->ke , &(*acv).a_mblock , 0 , &ptoc_Var32->hbuf , &b_err);
        (*acv).a_mblock.integer.C_2.mb_data_len = aux_data_len;
        if ((int ) b_err == 0)
            {
            ptoc_Var32->hmbp = (struct tgg00_MessBlock *) &(*(struct tak_systembuffer *) sql__nil(ptoc_Var32->hbuf , "vak660.p" , 15608)).integer.C_62.smessblock.mbr_mess_block;
            ptoc_Var32->hqbp = (*(struct tgg00_MessBlock *) sql__nil(ptoc_Var32->hmbp , "vak660.p" , 15609)).integer.C_4.mb_qual;
            t01messblock(5 , "mess_block >" , sql__nil(ptoc_Var32->hmbp , "vak660.p" , 15611));
            t01messblock(5 , "MESSBLOCK   " , &(*acv).a_mblock);
            if ((char ) sql__setrang(repl_only , 0 , 1))
                {
                a10add_sysinfo(&(*acv) , &ptoc_Var32->hbuf , &b_err);
                }
            }
        if ((int ) b_err != 0)
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
            }
        else
            {
            if (!(char ) sql__setrang(repl_only , 0 , 1))
                {
                ptoc_Var36 = &(*acv).a_mblock;
                ptoc_Var37 = (tgg00_QualBuf *) sql__nil(ptoc_Var36->integer.C_4.mb_qual , "vak660.p" , 15626);
                (*acv).a_mblock.integer.C_2.mb_qual_len = 112;
                ptoc_Var37->integer.C_3.mstack_desc = g01nil_stack_desc;
                ptoc_Var37->integer.C_2.mqual_pos = 1;
                ptoc_Var37->integer.C_2.mqual_cnt = 1;
                ptoc_Var37->integer.C_2.mfirst_free = 2;
                ptoc_Var37->integer.C_2.msubquery = (char ) sql__setrang(init_msubquery , 0 , 1);
                ptoc_Var37->integer.C_2.mst_max = (short ) sql__setrang((short ) ((*acv).a_stack_size / sizeof(struct ptoc_Type39 )) , -32768 , 32767);
                ptoc_Var36->integer.C_2.mb_st_max = (short ) sql__setrang(ptoc_Var37->integer.C_2.mst_max , -32768 , 32767);
                ptoc_Var37->integer.C_2.mst_addr = (*acv).a_stack_addr;
                ptoc_Var36->integer.C_5.mb_st = (*acv).a_stack_addr;
                ptoc_Var36->integer.C_2.mb_st_size = (*acv).a_stack_size;
                }
            }
        }
    ptoc_Var38 = (tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak660.p" , 15649);
    t01int4(5 , "first_free 0" , (int ) ptoc_Var38->integer.C_2.mfirst_free);
    cccprint("<ak660move_in_hb");
    }

static
  tak00_ooption ptoc_Var39 = 
  {
  (unsigned char)'\001'
  };
static
  unsigned char ptoc_Var40[1] = 
  {
  (unsigned char)'@'
  };
static
  unsigned char ptoc_Var41[1] = 
  {
  (unsigned char)' '
  };
static
  unsigned char ptoc_Var42[1] = 
  {
  (unsigned char)'\002'
  };
static
  unsigned char ptoc_Var43[1] = 
  {
  (unsigned char)'\030'
  };
static
  unsigned char ptoc_Var44[1] = 
  {
  (unsigned char)'\b'
  };
static
  unsigned char ptoc_Var45[1] = 
  {
  (unsigned char)'\001'
  };


int ak660order_by(acv, dmli, order_n)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tsp_int2 order_n;
    {
    int curr_n;
    int node;
    int o_node;
    int old_error;
    int old_return;
    int pos;
    int s_node;
    tak00_ord_fields *ptoc_Var46;
    struct ptoc_Type54 *ptoc_Var47;
    struct ptoc_Type57 *ptoc_Var48;
    struct ptoc_Type57 *ptoc_Var49;
    struct ptoc_Type57 *ptoc_Var50;
    struct ptoc_Type57 *ptoc_Var51;
    unsigned char ptoc_Var52[1];
    unsigned char ptoc_Var53[1];
    unsigned char ptoc_Var54[1];
    tak00_columninfo *ptoc_Var55;
    struct ptoc_Type172 *ptoc_Var56;
    struct ptoc_Type57 *ptoc_Var57;
    cccprint("AK660 ak660order");
    ptoc_Var46 = (tak00_ord_fields *) sql__nil((*dmli).d_order_or_group_cols , "vak660.p" , 15673);
    ptoc_Var46->ocntord = 0;
    if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 15676))[sql__setrang((int ) order_n , 0 , 32767)].n_proc == 54 && ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 15678))[sql__setrang((int ) order_n , 0 , 32767)].n_subproc == 5 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 15679))[sql__setrang((int ) order_n , 0 , 32767)].n_subproc == 7))
        {
        curr_n = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 15682))[sql__setrang((int ) order_n , 0 , 32767)].n_lo_level;
        while (curr_n != 0 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 15683)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var46->ocntord = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var46->ocntord , -32768 , 32767 , "vak660.p" , 15685) , -32768 , 32767);
            if ((int ) ptoc_Var46->ocntord > 128)
                {
                a07_b_put_error(&(*acv) , 6466 , -(int ) ptoc_Var46->ocntord);
                }
            else
                {
                ptoc_Var47 = &ptoc_Var46->ofield[sql__setrang((int ) ptoc_Var46->ocntord , 1 , 128) + -1];
                _ptoc_ASSIGN(unsigned char , 64, ptoc_Var47->ofname, a01_il_b_identifier)
                ptoc_Var47->ofno = 0;
                ptoc_Var47->offno = 0;
                ptoc_Var47->ofstno = 0;
                _ptoc_MOVE(unsigned char , 1, ptoc_Var47->ofasc, ptoc_Var39);
                ptoc_Var47->oftabno = (unsigned char)'\0';
                ptoc_Var47->ofapos = 0;
                ptoc_Var47->ofnode = 0;
                ptoc_Var47->offill = 0;
                _ptoc_ASSIGN(unsigned char , 64, (*dmli).d_user, a01_il_b_identifier)
                _ptoc_ASSIGN(unsigned char , 64, (*dmli).d_table, a01_il_b_identifier)
                _ptoc_ASSIGN(unsigned char , 64, (*dmli).d_column, a01_il_b_identifier)
                o_node = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 15706))[sql__setrang(curr_n , 0 , 32767)].n_lo_level;
                switch ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 15707))[sql__setrang(o_node , 0 , 32767)].n_symb)
                    {
                    case 49 :
                        ptoc_Var48 = &(*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 15709))[sql__setrang(o_node , 0 , 32767)];
                        a05_unsigned_int2_get(&(*acv) , ptoc_Var48->n_pos , (short ) sql__setrang(ptoc_Var48->n_length , -32768 , 32767) , 6508 , &ptoc_Var47->ofno);
                        ptoc_Var47->ofno = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var47->ofno , -32768 , 32767 , "vak660.p" , 15715) , -32768 , 32767);
                        break;
                    case 33 :
                        ptoc_Var47->ofno = (short ) sql__setrang((short ) sql__succ((*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 15719))[sql__setrang(o_node , 0 , 32767)].n_pos , -2147483647 , 2147483647 , "vak660.p" , 15719) , -32768 , 32767);
                        break;
                    case 43 :
                    case 44 :
                    case 45 :
                        if ((char ) sql__setrang((*dmli).d_union , 0 , 1))
                            {
                            a07_b_put_error(&(*acv) , 6377 , (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 15727))[sql__setrang(o_node , 0 , 32767)].n_pos);
                            }
                        else
                            {
                            node = o_node;
                            ptoc_Var49 = &(*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 15731))[sql__setrang(o_node , 0 , 32767)];
                            (*dmli).d_vppos = ptoc_Var49->n_pos;
                            (*dmli).d_n_pos = ptoc_Var49->n_pos;
                            a06get_username(&(*acv) , &o_node , (*dmli).d_user);
                            ptoc_Var50 = &(*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 15738))[sql__setrang(o_node , 0 , 32767)];
                            if ((int ) ptoc_Var50->n_symb == 44)
                                {
                                a05identifier_get(&(*acv) , o_node , sizeof(unsigned char [64]) , (*dmli).d_table);
                                o_node = (int ) ptoc_Var50->n_sa_level;
                                }
                            ptoc_Var51 = &(*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 15748))[sql__setrang(o_node , 0 , 32767)];
                            pos = ptoc_Var51->n_pos;
                            a05identifier_get(&(*acv) , o_node , sizeof(unsigned char [64]) , (*dmli).d_column);
                            old_return = (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 15755)).integer.C_4.sp1r_returncode;
                            old_error = (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 15756)).integer.C_4.sp1r_errorpos;
                            if ((_ptoc_MEMCMP(unsigned char , 64, (*dmli).d_user , a01_il_b_identifier) == 0 ? 1 : 0) && (_ptoc_MEMCMP(unsigned char , 64, (*dmli).d_table , a01_il_b_identifier) == 0 ? 1 : 0))
                                {
                                _ptoc_MOVE(unsigned char , 1, ptoc_Var47->ofasc, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var52 , ptoc_Var47->ofasc , ptoc_Var40 , sizeof(unsigned char [1])));
                                }
                            a61_search_table(&(*acv) , &(*dmli));
                            if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 15765)).integer.C_4.sp1r_returncode != 0)
                                {
                                if (((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 15768)).integer.C_4.sp1r_returncode == (int ) a07_return_code(6492 , (unsigned char ) sql__setrang((*acv).a_sqlmode , 0 , 3)) || (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 15771)).integer.C_4.sp1r_returncode == (int ) a07_return_code(6341 , (unsigned char ) sql__setrang((*acv).a_sqlmode , 0 , 3)) && (*dmli).d_order_or_group_cols == (tak00_ord_fields *) &(*dmli).d_order_cols) && (_ptoc_MEMCMP(unsigned char , 64, (*dmli).d_user , a01_il_b_identifier) == 0 ? 1 : 0) && (_ptoc_MEMCMP(unsigned char , 64, (*dmli).d_table , a01_il_b_identifier) == 0 ? 1 : 0))
                                    {
                                    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 15784)).integer.C_4.sp1r_returncode == (int ) a07_return_code(6341 , (unsigned char ) sql__setrang((*acv).a_sqlmode , 0 , 3)))
                                        {
                                        _ptoc_MOVE(unsigned char , 1, ptoc_Var47->ofasc, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var54 , ptoc_Var47->ofasc , ptoc_Var41 , sizeof(unsigned char [1])));
                                        }
                                    (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 15790)).integer.C_4.sp1r_returncode = (short ) sql__setrang((short ) old_return , -32768 , 32767);
                                    (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 15791)).integer.C_4.sp1r_errorpos = old_error;
                                    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var47->ofname, (*dmli).d_column)
                                    }
                                else
                                    {
                                    a07put_error_pos(&(*acv) , pos);
                                    }
                                }
                            else
                                {
                                a16col_to_view_description(&(*acv) , &(*dmli) , sql__nil((*dmli).d_colbuf , "vak660.p" , 15800) , 0);
                                ptoc_Var55 = (tak00_columninfo *) sql__nil((*dmli).d_colbuf , "vak660.p" , 15801);
                                ptoc_Var56 = &(*dmli).d_tabarr[sql__setrang((int ) (*dmli).d_acttabindex , 1 , 64) + -1];
                                ptoc_Var47->oftabno = (unsigned char ) sql__setrang((unsigned char ) (*dmli).d_acttabindex , 0 , 255);
                                if ((int ) ptoc_Var55->integer.C_1.ccolstack.integer.C_1.etype != 45 || (int ) ptoc_Var55->integer.C_1.ccolstack.integer.C_1.etype == 45 && (int ) ptoc_Var55->integer.C_1.ccolstack.integer.C_1.eop == 0)
                                    {
                                    if ((char ) sql__setrang(ptoc_Var56->oview , 0 , 1) && (1 && (ptoc_Var56->ospecialname[0] & 8) != 0))
                                        {
                                        ptoc_Var47->offno = (short ) sql__setrang(ptoc_Var55->integer.C_1.cextcolno , -32768 , 32767);
                                        }
                                    else
                                        {
                                        ptoc_Var47->offno = (short ) sql__setrang(ptoc_Var55->integer.C_1.creccolno , -32768 , 32767);
                                        }
                                    }
                                else
                                    {
                                    a07_nb_put_error(&(*acv) , 6318 , pos , (*dmli).d_column);
                                    }
                                if (1 && (ptoc_Var55->integer.C_1.ccolpropset[0] & 2) != 0)
                                    {
                                    _ptoc_MOVE(unsigned char , 1, ptoc_Var47->ofasc, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var52 , ptoc_Var47->ofasc , ptoc_Var42 , sizeof(unsigned char [1])));
                                    }
                                ptoc_Var47->ofnode = (short ) sql__setrang((short ) o_node , -32768 , 32767);
                                if ((int ) ptoc_Var55->integer.C_1.cdatatyp == 2 && !(char ) sql__setrang((*acv).a_nls_params.nls_binary , 0 , 1) && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 15830))[sql__setrang((int ) order_n , 0 , 32767)].n_subproc == 5)
                                    {
                                    ptoc_Var47->ofnode = (short ) sql__setrang((short ) node , -32768 , 32767);
                                    _ptoc_MOVE(unsigned char , 1, ptoc_Var47->ofasc, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var54 , ptoc_Var47->ofasc , ptoc_Var43 , sizeof(unsigned char [1])));
                                    }
                                else
                                    {
                                    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var47->ofname, (*dmli).d_column)
                                    }
                                }
                            }
                        break;
                    default :
                        if ((char ) sql__setrang((*dmli).d_union , 0 , 1))
                            {
                            a07_b_put_error(&(*acv) , 6377 , (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 15852))[sql__setrang(o_node , 0 , 32767)].n_pos);
                            }
                        else
                            {
                            ptoc_Var47->oftabno = (unsigned char ) sql__setrang((unsigned char ) (*dmli).d_acttabindex , 0 , 255);
                            ptoc_Var47->ofnode = (short ) sql__setrang((short ) o_node , -32768 , 32767);
                            _ptoc_MOVE(unsigned char , 1, ptoc_Var47->ofasc, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var52 , ptoc_Var47->ofasc , ptoc_Var44 , sizeof(unsigned char [1])));
                            }
                        break;
                    }
                ptoc_Var57 = &(*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 15862))[sql__setrang(o_node , 0 , 32767)];
                s_node = (int ) ptoc_Var57->n_sa_level;
                ptoc_Var47->ofapos = ptoc_Var57->n_pos;
                while (s_node != 0)
                    {
                    if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 15870))[sql__setrang(s_node , 0 , 32767)].n_symb == 51)
                        {
                        _ptoc_MOVE(unsigned char , 1, ptoc_Var47->ofasc, *(unsigned char (*)[1]) sql__setsub(ptoc_Var54 , ptoc_Var47->ofasc , ptoc_Var45 , sizeof(unsigned char [1])));
                        }
                    s_node = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 15874))[sql__setrang(s_node , 0 , 32767)].n_sa_level;
                    }
                curr_n = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 15877))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
                }
            }
        t01int4(5 , "cntord      " , (int ) ptoc_Var46->ocntord);
        t01int4(5 , "curr_n      " , curr_n);
        }
    cccprint("<ak660order_by  ");
    }



int ak660qual_part_transfer(acv, dmli, hbuf, functions_not_single_fields, start_pos, end_pos)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_sysbufferaddress *hbuf;
char functions_not_single_fields;
int start_pos;
int end_pos;
    {
    int function_stackentry_cnt;
    int next_pos;
    int qual_end;
    int qual_pos;
    int start_function_pos;
    int start_for_update_epos;
    tgg_mess_block_ptr hmbp;
    tgg_qual_buf_ptr hqbp;
    char output_hold_found;
    char update_epos;
    char update_filledbyte;
    char not_in_output_list;
    tgg_stack_op_type m_op;
    struct tgg00_MessBlock *ptoc_Var58;
    tgg00_QualBuf *ptoc_Var59;
    struct ptoc_Type39 *ptoc_Var60;
    int ptoc_Var61;
    int ptoc_Var62;
    struct ptoc_Type39 *ptoc_Var63;
    struct ptoc_Type39 *ptoc_Var64;
    struct ptoc_Type39 *ptoc_Var65;
    struct ptoc_Type39 *ptoc_Var66;
    struct ptoc_Type39 *ptoc_Var67;
    cccprint("AK660 ak660qual_");
    qual_pos = start_pos;
    qual_end = end_pos;
    if ((char ) sql__setrang(functions_not_single_fields , 0 , 1))
        {
        start_function_pos = 0;
        }
    else
        {
        start_function_pos = qual_end + 1;
        }
    ptoc_Var58 = &(*acv).a_mblock;
    ptoc_Var59 = (tgg00_QualBuf *) sql__nil(ptoc_Var58->integer.C_4.mb_qual , "vak660.p" , 15924);
    start_for_update_epos = (int ) ptoc_Var59->integer.C_2.mfirst_free;
    hmbp = (struct tgg00_MessBlock *) &(*(struct tak_systembuffer *) sql__nil(*hbuf , "vak660.p" , 15927)).integer.C_62.smessblock.mbr_mess_block;
    hqbp = (*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 15928)).integer.C_4.mb_qual;
    output_hold_found = 0;
    update_epos = 0;
    update_filledbyte = 0;
    not_in_output_list = (char ) sql__setrang(start_pos != (int ) (*(tgg00_QualBuf *) sql__nil(hqbp , "vak660.p" , 15932)).integer.C_2.mqual_pos + 1 , 0 , 1);
    while (qual_pos <= qual_end)
        {
        output_hold_found = 0;
        t01int4(5 , "d_inoutpos  " , (int ) (*dmli).d_inoutpos);
        t01int4(5 , "d_filled_by0" , (int ) (*dmli).d_filled_bytes);
        t01int4(5 , "qual_pos    " , qual_pos);
        t01int4(5 , "qual_end    " , qual_end);
        t01int4(5 , "first_free  " , (int ) ptoc_Var59->integer.C_2.mfirst_free);
        t01int4(5 , "start_funct " , start_function_pos);
        if (start_function_pos != qual_end + 1)
            {
            t01int4(5 , "stackentrcnt" , function_stackentry_cnt);
            }
        if (start_function_pos < qual_pos)
            {
            next_pos = qual_pos + -1;
            do
                next_pos = sql__succ(next_pos , -2147483647 , 2147483647 , "vak660.p" , 15951);
            while (!((int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 15953)).integer.C_5.mb_st , "vak660.p" , 15953))[sql__setrang(next_pos , 1 , 32767) + -1].integer.C_1.etype == 45 && (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 15954)).integer.C_5.mb_st , "vak660.p" , 15954))[sql__setrang(next_pos , 1 , 32767) + -1].integer.C_1.eop != 0 || next_pos == qual_end));
            if ((int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 15957)).integer.C_5.mb_st , "vak660.p" , 15957))[sql__setrang(next_pos , 1 , 32767) + -1].integer.C_1.etype != 45)
                {
                start_function_pos = qual_end + 1;
                }
            else
                {
                if ((int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 15962)).integer.C_5.mb_st , "vak660.p" , 15962))[sql__setrang(next_pos , 1 , 32767) + -1].integer.C_1.eop == 0)
                    {
                    (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 15964)).integer.C_5.mb_st , "vak660.p" , 15964))[sql__setrang(next_pos , 1 , 32767) + -1].integer.C_1.etype = 39;
                    }
                function_stackentry_cnt = sql__ord((int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 15967)).integer.C_5.mb_st , "vak660.p" , 15967))[sql__setrang(next_pos , 1 , 32767) + -1].integer.C_1.ecol_tab[1]);
                start_function_pos = next_pos - function_stackentry_cnt + 1;
                }
            t01int4(5 , "start_funct " , start_function_pos);
            if (start_function_pos != qual_end + 1)
                {
                t01int4(5 , "stackentrcnt" , function_stackentry_cnt);
                }
            }
        if (start_function_pos == qual_pos && function_stackentry_cnt > 0)
            {
            if ((int ) ptoc_Var59->integer.C_2.mfirst_free + function_stackentry_cnt > (int ) ptoc_Var58->integer.C_2.mb_st_max)
                {
                a07_b_put_error(&(*acv) , 2800 , 1);
                qual_pos = qual_end + 1;
                }
            else
                {
                g10mv4("VAK660" , 1 , (*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 15991)).integer.C_2.mb_st_size , ptoc_Var58->integer.C_2.mb_st_size , sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 15991)).integer.C_5.mb_st , "vak660.p" , 15991) , start_function_pos * 8 + -7 , sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 15993) , (int ) ptoc_Var59->integer.C_2.mfirst_free * 8 + -7 , function_stackentry_cnt * 8 , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 15995)).integer.C_4.sp1r_returncode);
                ptoc_Var59->integer.C_2.mfirst_free = (short ) sql__setrang((short ) ((int ) ptoc_Var59->integer.C_2.mfirst_free + function_stackentry_cnt + 1) , -32768 , 32767);
                ptoc_Var59->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) ((int ) ptoc_Var59->integer.C_2.mqual_cnt + function_stackentry_cnt + 1) , -32768 , 32767);
                next_pos = (int ) ptoc_Var59->integer.C_2.mfirst_free + -3;
                while (next_pos > 1 && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16000))[sql__setrang(next_pos , 1 , 32767) + -1].integer.C_1.etype != 45)
                    next_pos = sql__pred(next_pos , -2147483647 , 2147483647 , "vak660.p" , 16001);
                (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16003))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free + -2 , 1 , 32767) + -1].integer.C_1.epos = (short ) sql__setrang((short ) ((int ) ptoc_Var59->integer.C_2.mfirst_free - next_pos + -2) , -32768 , 32767);
                ptoc_Var60 = &(*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16004))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1];
                ptoc_Var60->integer.C_1.etype = 28;
                if (((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 16007))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 16007))[0].n_lo_level , 0 , 32767)].n_proc == 54 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 16008))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 16008))[0].n_lo_level , 0 , 32767)].n_proc == 53) && !(char ) sql__setrang((*dmli).d_subquery , 0 , 1))
                    {
                    ptoc_Var60->integer.C_3.eop_out = 9;
                    }
                else
                    {
                    ptoc_Var60->integer.C_3.eop_out = 0;
                    }
                ptoc_Var60->integer.C_1.epos = (short ) sql__setrang((*dmli).d_inoutpos , -32768 , 32767);
                ptoc_Var60->integer.C_1.elen_var = (short ) sql__setrang((*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16017))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free + -2 , 1 , 32767) + -1].integer.C_1.elen_var , -32768 , 32767);
                ptoc_Var60->integer.C_1.ecol_tab[0] = (unsigned char ) sql__setrang((*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16018))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free + -2 , 1 , 32767) + -1].integer.C_1.ecol_tab[0] , 0 , 255);
                ptoc_Var60->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                ptoc_Var61 = start_function_pos;
                ptoc_Var62 = start_function_pos + function_stackentry_cnt + -2;
                if (ptoc_Var61 <= ptoc_Var62)
                    {
                    next_pos = ptoc_Var61;
                    do
                        {
                        ptoc_Var63 = &(*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16024)).integer.C_5.mb_st , "vak660.p" , 16024))[sql__setrang(next_pos , 1 , 32767) + -1];
                        ptoc_Var63->integer.C_1.etype = 39;
                        ptoc_Var63->integer.C_1.eop = 0;
                        if (next_pos == ptoc_Var62)
                            {
                            break;
                            }
                        next_pos += 1;
                        }
                    while (!0);
                    }
                ptoc_Var64 = &(*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16031)).integer.C_5.mb_st , "vak660.p" , 16031))[sql__setrang(start_function_pos + function_stackentry_cnt + -1 , 1 , 32767) + -1];
                ptoc_Var64->integer.C_1.etype = 2;
                ptoc_Var64->integer.C_1.eop = 0;
                ptoc_Var64->integer.C_1.epos = (short ) sql__setrang((short ) ((int ) (*dmli).d_inoutpos + -12) , -32768 , 32767);
                if ((ptoc_Var62 = (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16037))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free + -2 , 1 , 32767) + -1].integer.C_2.eop_func) == 2 || ptoc_Var62 == 6 || ptoc_Var62 == 12 || ptoc_Var62 == 13 || ptoc_Var62 == 14 || ptoc_Var62 == 15)
                    {
                    (*dmli).d_inoutpos = (short ) sql__setrang((short ) ((int ) (*dmli).d_inoutpos + 7) , -32768 , 32767);
                    }
                t01int4(5 , "d_inoutpos1q" , (int ) (*dmli).d_inoutpos);
                if ((ptoc_Var62 = (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16045))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free + -2 , 1 , 32767) + -1].integer.C_2.eop_func) == 4 || ptoc_Var62 == 7 || ptoc_Var62 == 1)
                    {
                    if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16048))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.elen_var <= 7)
                        {
                        ptoc_Var64->integer.C_1.elen_var = 7;
                        }
                    else
                        {
                        ptoc_Var64->integer.C_1.elen_var = (short ) sql__setrang((*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16052))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.elen_var , -32768 , 32767);
                        }
                    }
                else
                    {
                    ptoc_Var64->integer.C_1.elen_var = (short ) sql__setrang((*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16055))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.elen_var , -32768 , 32767);
                    }
                ptoc_Var64->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                if (!(char ) sql__setrang((*dmli).d_outer_join , 0 , 1))
                    {
                    ptoc_Var64->integer.C_1.ecol_tab[1] = (unsigned char)'\001';
                    }
                (*dmli).d_inoutpos = (short ) sql__setrang((short ) ((int ) (*dmli).d_inoutpos + (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16063))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.elen_var) , -32768 , 32767);
                (*dmli).d_filled_bytes = (short ) sql__setrang((short ) ((int ) (*dmli).d_filled_bytes + (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16066))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.elen_var) , -32768 , 32767);
                update_filledbyte = 1;
                t01int4(5 , "d_inoutpos2q" , (int ) (*dmli).d_inoutpos);
                qual_pos = start_function_pos + function_stackentry_cnt;
                }
            }
        else
            {
            if ((ptoc_Var62 = (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16075)).integer.C_5.mb_st , "vak660.p" , 16075))[sql__setrang(qual_pos , 1 , 32767) + -1].integer.C_1.etype) == 0 || ptoc_Var62 == 1 || ptoc_Var62 == 2 || ptoc_Var62 == 3 || ptoc_Var62 == 13 || (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16079)).integer.C_5.mb_st , "vak660.p" , 16079))[sql__setrang(qual_pos , 1 , 32767) + -1].integer.C_1.etype == 22 && (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16080)).integer.C_5.mb_st , "vak660.p" , 16080))[sql__setrang(qual_pos , 1 , 32767) + -1].integer.C_1.eop == 19 && (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16081)).integer.C_5.mb_st , "vak660.p" , 16081))[sql__setrang(qual_pos + 1 , 1 , 32767) + -1].integer.C_1.etype == 28)
                {
                if ((int ) ptoc_Var59->integer.C_2.mfirst_free + 1 > (int ) ptoc_Var58->integer.C_2.mb_st_max)
                    {
                    a07_b_put_error(&(*acv) , 2800 , 1);
                    qual_pos = qual_end + 1;
                    }
                else
                    {
                    m_op = (unsigned char ) sql__setrang((*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16092)).integer.C_5.mb_st , "vak660.p" , 16092))[sql__setrang(qual_pos , 1 , 32767) + -1].integer.C_1.eop , 0 , 123);
                    if ((int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16093)).integer.C_5.mb_st , "vak660.p" , 16093))[sql__setrang(qual_pos , 1 , 32767) + -1].integer.C_1.eop == 23 || (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16095)).integer.C_5.mb_st , "vak660.p" , 16095))[sql__setrang(qual_pos + 1 , 1 , 32767) + -1].integer.C_1.etype != 28 || (ptoc_Var62 = (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16097)).integer.C_5.mb_st , "vak660.p" , 16097))[sql__setrang(qual_pos , 1 , 32767) + -1].integer.C_1.eop) != 0 && ptoc_Var62 != 19 && ptoc_Var62 != 28 && ptoc_Var62 != 20 && ptoc_Var62 != 29)
                        {
                        (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16102))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free , 1 , 32767) + -1] = (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16102)).integer.C_5.mb_st , "vak660.p" , 16102))[sql__setrang(qual_pos , 1 , 32767) + -1];
                        if ((ptoc_Var62 = (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16103)).integer.C_5.mb_st , "vak660.p" , 16103))[sql__setrang(qual_pos , 1 , 32767) + -1].integer.C_1.eop) != 0 && ptoc_Var62 != 19 && ptoc_Var62 != 28 && ptoc_Var62 != 20 && ptoc_Var62 != 29)
                            {
                            (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16107))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free , 1 , 32767) + -1].integer.C_1.eop = 0;
                            }
                        ptoc_Var65 = &(*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16109))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free + 1 , 1 , 32767) + -1];
                        ptoc_Var65->integer.C_1.etype = 28;
                        if (((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 16112))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 16112))[0].n_lo_level , 0 , 32767)].n_proc == 54 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 16113))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 16113))[0].n_lo_level , 0 , 32767)].n_proc == 53) && !(char ) sql__setrang((*dmli).d_subquery , 0 , 1))
                            {
                            ptoc_Var65->integer.C_3.eop_out = 9;
                            }
                        else
                            {
                            ptoc_Var65->integer.C_3.eop_out = 0;
                            }
                        ptoc_Var65->integer.C_1.elen_var = (short ) sql__setrang((*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16121)).integer.C_5.mb_st , "vak660.p" , 16121))[sql__setrang(qual_pos , 1 , 32767) + -1].integer.C_1.elen_var , -32768 , 32767);
                        (*dmli).d_filled_bytes = (short ) sql__setrang((short ) ((int ) (*dmli).d_filled_bytes + (int ) ptoc_Var65->integer.C_1.elen_var) , -32768 , 32767);
                        update_filledbyte = 1;
                        if (!(char ) sql__setrang((*dmli).d_one_join_phase , 0 , 1) || (char ) sql__setrang(not_in_output_list , 0 , 1))
                            {
                            ptoc_Var65->integer.C_1.epos = (short ) sql__setrang((*dmli).d_inoutpos , -32768 , 32767);
                            }
                        else
                            {
                            ptoc_Var65->integer.C_1.epos = 0;
                            update_epos = 1;
                            next_pos = qual_pos;
                            while ((int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16133)).integer.C_5.mb_st , "vak660.p" , 16133))[sql__setrang(next_pos , 1 , 32767) + -1].integer.C_1.etype != 28)
                                next_pos = sql__succ(next_pos , -2147483647 , 2147483647 , "vak660.p" , 16135);
                            }
                        ptoc_Var65->integer.C_2.ecol_pos = 0;
                        next_pos = qual_pos;
                        }
                    else
                        {
                        next_pos = qual_pos + 1;
                        while ((int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16147)).integer.C_5.mb_st , "vak660.p" , 16147))[sql__setrang(next_pos , 1 , 32767) + -1].integer.C_1.etype != 28)
                            next_pos = sql__succ(next_pos , -2147483647 , 2147483647 , "vak660.p" , 16149);
                        (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16151))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free , 1 , 32767) + -1] = (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16152)).integer.C_5.mb_st , "vak660.p" , 16152))[sql__setrang(qual_pos , 1 , 32767) + -1];
                        (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16153))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free + 1 , 1 , 32767) + -1] = (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16154)).integer.C_5.mb_st , "vak660.p" , 16154))[sql__setrang(next_pos , 1 , 32767) + -1];
                        output_hold_found = (char ) sql__setrang((char ) sql__setrang((*dmli).d_one_join_phase , 0 , 1) && (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16157)).integer.C_5.mb_st , "vak660.p" , 16157))[sql__setrang(next_pos , 1 , 32767) + -1].integer.C_1.etype == 28 && (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16158)).integer.C_5.mb_st , "vak660.p" , 16158))[sql__setrang(next_pos , 1 , 32767) + -1].integer.C_3.eop_out == 4 , 0 , 1);
                        if (!(char ) sql__setrang((*dmli).d_one_join_phase , 0 , 1))
                            {
                            (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16163))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free + 1 , 1 , 32767) + -1].integer.C_1.epos = (short ) sql__setrang((*dmli).d_inoutpos , -32768 , 32767);
                            (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16165))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free + 1 , 1 , 32767) + -1].integer.C_1.eop = 0;
                            }
                        }
                    ptoc_Var66 = &(*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16171)).integer.C_5.mb_st , "vak660.p" , 16171))[sql__setrang(qual_pos , 1 , 32767) + -1];
                    ptoc_Var66->integer.C_1.etype = 2;
                    if ((int ) m_op == 23)
                        {
                        ptoc_Var66->integer.C_1.eop = 23;
                        }
                    else
                        {
                        if ((ptoc_Var62 = (int ) ptoc_Var66->integer.C_1.eop) == 28 || ptoc_Var62 == 19 || ptoc_Var62 == 20 || ptoc_Var62 == 29)
                            {
                            ptoc_Var66->integer.C_1.eop = 0;
                            }
                        }
                    ptoc_Var66->integer.C_1.epos = (short ) sql__setrang((short ) ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16184))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free + 1 , 1 , 32767) + -1].integer.C_1.epos + -8) , -32768 , 32767);
                    if ((int ) ptoc_Var66->integer.C_1.epos > (int ) (*dmli).d_keylen)
                        {
                        ptoc_Var66->integer.C_1.epos = (short ) sql__setrang((short ) ((int ) ptoc_Var66->integer.C_1.epos - (int ) (*dmli).d_keylen) , -32768 , 32767);
                        }
                    ptoc_Var66->integer.C_1.elen_var = (short ) sql__setrang((*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16189))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free + 1 , 1 , 32767) + -1].integer.C_1.elen_var , -32768 , 32767);
                    ptoc_Var66->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                    if (!(char ) sql__setrang((*dmli).d_outer_join , 0 , 1))
                        {
                        ptoc_Var66->integer.C_1.ecol_tab[1] = (unsigned char)'\001';
                        }
                    if ((char ) sql__setrang(output_hold_found , 0 , 1))
                        {
                        ptoc_Var59->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var59->integer.C_2.mqual_cnt , -32768 , 32767 , "vak660.p" , 16201) , -32768 , 32767);
                        ptoc_Var59->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var59->integer.C_2.mfirst_free , -32768 , 32767 , "vak660.p" , 16202) , -32768 , 32767);
                        next_pos = sql__succ(next_pos , -2147483647 , 2147483647 , "vak660.p" , 16203);
                        (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16204))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free + 1 , 1 , 32767) + -1] = (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16205)).integer.C_5.mb_st , "vak660.p" , 16205))[sql__setrang(next_pos , 1 , 32767) + -1];
                        (*dmli).d_inoutpos = (short ) sql__setrang((short ) ((int ) (*dmli).d_inoutpos + (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16207))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free + 1 , 1 , 32767) + -1].integer.C_1.elen_var) , -32768 , 32767);
                        }
                    if ((char ) sql__setrang(not_in_output_list , 0 , 1) || !(char ) sql__setrang((*dmli).d_one_join_phase , 0 , 1) || (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16213)).integer.C_5.mb_st , "vak660.p" , 16213))[sql__setrang(next_pos , 1 , 32767) + -1].integer.C_1.etype == 28)
                        {
                        (*dmli).d_inoutpos = (short ) sql__setrang((short ) ((int ) (*dmli).d_inoutpos + (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16216))[sql__setrang((int ) ptoc_Var59->integer.C_2.mfirst_free + 1 , 1 , 32767) + -1].integer.C_1.elen_var) , -32768 , 32767);
                        }
                    ptoc_Var59->integer.C_2.mfirst_free = (short ) sql__setrang((short ) ((int ) ptoc_Var59->integer.C_2.mfirst_free + 2) , -32768 , 32767);
                    ptoc_Var59->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) ((int ) ptoc_Var59->integer.C_2.mqual_cnt + 2) , -32768 , 32767);
                    qual_pos = next_pos + 1;
                    t01int4(5 , "d_inoutpos q" , (int ) (*dmli).d_inoutpos);
                    t01stqual(5 , sql__nil(hqbp , "vak660.p" , 16222) , (*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16222)).integer.C_5.mb_st);
                    t01stqual(5 , sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak660.p" , 16223) , (*acv).a_mblock.integer.C_5.mb_st);
                    }
                }
            else
                {
                if ((int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16228)).integer.C_5.mb_st , "vak660.p" , 16228))[sql__setrang(qual_pos , 1 , 32767) + -1].integer.C_1.etype == 28)
                    {
                    if ((char ) sql__setrang(update_filledbyte , 0 , 1))
                        {
                        (*dmli).d_filled_bytes = (short ) sql__setrang((short ) ((int ) (*dmli).d_filled_bytes - (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16235)).integer.C_5.mb_st , "vak660.p" , 16235))[sql__setrang(qual_pos , 1 , 32767) + -1].integer.C_1.elen_var) , -32768 , 32767);
                        update_filledbyte = 0;
                        }
                    if ((char ) sql__setrang((*dmli).d_one_join_phase , 0 , 1))
                        {
                        (*dmli).d_inoutpos = (short ) sql__setrang((short ) ((int ) (*dmli).d_inoutpos + (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16242)).integer.C_5.mb_st , "vak660.p" , 16242))[sql__setrang(qual_pos , 1 , 32767) + -1].integer.C_1.elen_var) , -32768 , 32767);
                        }
                    }
                qual_pos = sql__succ(qual_pos , -2147483647 , 2147483647 , "vak660.p" , 16246);
                }
            }
        if ((int ) (*dmli).d_inoutpos > 8096)
            {
            a07_b_put_error(&(*acv) , 6450 , (int ) (*dmli).d_inoutpos);
            qual_pos = qual_end + 1;
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 16259)).integer.C_4.sp1r_returncode == 0 && (char ) sql__setrang(update_epos , 0 , 1))
        {
        qual_pos = start_for_update_epos;
        qual_end = (int ) ptoc_Var59->integer.C_2.mfirst_free;
        next_pos = 1;
        while (qual_pos <= qual_end)
            {
            if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16278))[sql__setrang(qual_pos , 1 , 32767) + -1].integer.C_1.etype == 28 && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16279))[sql__setrang(qual_pos , 1 , 32767) + -1].integer.C_1.epos == 0)
                {
                update_epos = 1;
                while (next_pos < (int ) (*(tgg00_QualBuf *) sql__nil(hqbp , "vak660.p" , 16283)).integer.C_2.mfirst_free && (char ) sql__setrang(update_epos , 0 , 1))
                    {
                    ptoc_Var67 = &(*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 16285)).integer.C_5.mb_st , "vak660.p" , 16285))[sql__setrang(next_pos , 1 , 32767) + -1];
                    if ((int ) ptoc_Var67->integer.C_1.etype == 2 && (int ) ptoc_Var67->integer.C_1.epos == -8)
                        {
                        ptoc_Var67->integer.C_1.epos = (short ) sql__setrang((short ) ((int ) (*dmli).d_inoutpos + -8) , -32768 , 32767);
                        if ((int ) ptoc_Var67->integer.C_1.epos > (int ) (*dmli).d_keylen)
                            {
                            ptoc_Var67->integer.C_1.epos = (short ) sql__setrang((short ) ((int ) ptoc_Var67->integer.C_1.epos - (int ) (*dmli).d_keylen) , -32768 , 32767);
                            }
                        update_epos = 0;
                        }
                    next_pos = sql__succ(next_pos , -2147483647 , 2147483647 , "vak660.p" , 16299);
                    }
                t01int4(5 , "next_pos    " , next_pos);
                (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16303))[sql__setrang(qual_pos , 1 , 32767) + -1].integer.C_1.epos = (short ) sql__setrang((*dmli).d_inoutpos , -32768 , 32767);
                (*dmli).d_inoutpos = (short ) sql__setrang((short ) ((int ) (*dmli).d_inoutpos + (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var58->integer.C_5.mb_st , "vak660.p" , 16305))[sql__setrang(qual_pos , 1 , 32767) + -1].integer.C_1.elen_var) , -32768 , 32767);
                }
            qual_pos = sql__succ(qual_pos , -2147483647 , 2147483647 , "vak660.p" , 16308);
            }
        }
    t01int4(5 , "d_inoutpos  " , (int ) (*dmli).d_inoutpos);
    t01int4(5 , "d_filled_by1" , (int ) (*dmli).d_filled_bytes);
    cccprint("<ak660qual_part_");
    }



int ak660subquery(acv, dmli, sr_rec, curr_n, pseudo_resultset_select)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak70_strat_rec *sr_rec;
tsp_int2 curr_n;
char *pseudo_resultset_select;
    {
    tgg_sysinfokey ke;
    tgg_sysinfokey ke1;
    tgg_basis_error b_err;
    char restore;
    tsp_c2 first_linkage;
    int i;
    int sysicount;
    int aux_rc;
    tak_commandkind m_command_kind;
    unsigned char m_first_parskey;
    unsigned char m_complex_pid;
    tak_all_command_glob *ptoc_Var68;
    tak_dml_info *ptoc_Var69;
    tgg00_SysInfoKey *ptoc_Var70;
    cccprint("AK660 ak660subqu");
    restore = 0;
    sysicount = 0;
    m_command_kind = (unsigned char ) sql__setrang((*acv).a_command_kind , 0 , 11);
    m_first_parskey = (unsigned char ) sql__setrang((*acv).a_first_parskey , 0 , 255);
    m_complex_pid = (unsigned char ) sql__setrang((*acv).a_complex_key[4] , 0 , 255);
    ptoc_Var68 = &*acv;
    ptoc_Var69 = &*dmli;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var68->a_return_segm , "vak660.p" , 16353)).integer.C_4.sp1r_returncode == 0)
        {
        t01int4(5 , "d_union     " , sql__ord((int ) (char ) sql__setrang(ptoc_Var69->d_union , 0 , 1)));
        t01int4(5 , "a_insert_sel" , sql__ord((int ) (char ) sql__setrang(ptoc_Var68->a_insert_select , 0 , 1)));
        t01int4(5 , "a_from_selec" , sql__ord((int ) (char ) sql__setrang(ptoc_Var68->a_from_select , 0 , 1)));
        t01int4(5 , "a_fromsel_n " , sql__ord((int ) ptoc_Var68->a_fromsel_n));
        t01int4(5 , "command_kind" , sql__ord((int ) ptoc_Var68->a_command_kind));
        t01int4(5 , "a_first_pkey" , sql__ord((int ) ptoc_Var68->a_first_parskey));
        if (ptoc_Var68->a_union_res_id == 2)
            {
            ke = a01sysnullkey;
            ptoc_Var70 = &ke;
            ptoc_Var70->integer.C_4.stempid = ptoc_Var68->a_union_res_id;
            _ptoc_ASSIGN(unsigned char , 2, ptoc_Var70->integer.C_2.sentrytyp, "\0\261")
            _ptoc_ASSIGN(unsigned char , 2, ptoc_Var70->integer.C_2.slinkage, "\0\001")
            restore = 1;
            ke1 = ke;
            _ptoc_ASSIGN(unsigned char , 2, first_linkage, ke.integer.C_2.slinkage)
            i = 1;
            do
                {
                a06inc_linkage(ke.integer.C_2.slinkage);
                if (i == 3)
                    {
                    break;
                    }
                i += 1;
                }
            while (!0);
            sysicount = 0;
            b_err = 0;
            while ((int ) b_err == 0 && sysicount < 3)
                {
                t01int4(5 , "ke          " , i);
                t01buf1(5 , &ke , 1 , 20);
                t01int4(5 , "ke1         " , i);
                t01buf1(5 , &ke1 , 1 , 20);
                a10_copy_catalog_rec(&(*acv) , &ke1 , 1 , &ke , "\0\0" , 1 , &b_err);
                if ((int ) b_err == 0)
                    {
                    a10_rel_sysinfo(&(*acv) , &ke);
                    a06inc_linkage(ke.integer.C_2.slinkage);
                    a06inc_linkage(ke1.integer.C_2.slinkage);
                    sysicount = sql__succ(sysicount , -2147483647 , 2147483647 , "vak660.p" , 16397);
                    }
                else
                    {
                    if (sysicount == 0)
                        {
                        restore = 0;
                        }
                    }
                }
            if ((int ) b_err == 1930)
                {
                b_err = 0;
                }
            if ((int ) b_err != 0)
                {
                a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var68->a_return_segm , "vak660.p" , 16424)).integer.C_4.sp1r_returncode == 0)
            {
            a661a_subquery(&(*acv) , &(*dmli) , &(*sr_rec) , (short ) sql__setrang(curr_n , -32768 , 32767) , &(*pseudo_resultset_select));
            }
        if ((int ) m_first_parskey > 0 && (int ) m_command_kind == 8)
            {
            ptoc_Var68->a_first_parskey = (unsigned char ) sql__setrang(m_first_parskey , 0 , 255);
            ptoc_Var68->a_complex_key[4] = (unsigned char ) sql__setrang(m_complex_pid , 0 , 255);
            }
        if ((char ) sql__setrang(restore , 0 , 1))
            {
            aux_rc = (int ) (*(tsp1_segment *) sql__nil(ptoc_Var68->a_return_segm , "vak660.p" , 16440)).integer.C_4.sp1r_returncode;
            (*(tsp1_segment *) sql__nil(ptoc_Var68->a_return_segm , "vak660.p" , 16441)).integer.C_4.sp1r_returncode = 0;
            _ptoc_ASSIGN(unsigned char , 2, ke1.integer.C_2.slinkage, first_linkage)
            ke = ke1;
            i = 1;
            do
                {
                a06inc_linkage(ke.integer.C_2.slinkage);
                if (i == 3)
                    {
                    break;
                    }
                i += 1;
                }
            while (!0);
            i = 0;
            while ((int ) b_err == 0 && i < sysicount)
                {
                t01int4(5 , "ke1         " , i);
                t01buf1(5 , &ke1 , 1 , 20);
                t01int4(5 , "ke          " , i);
                t01buf1(5 , &ke , 1 , 20);
                a10_copy_catalog_rec(&(*acv) , &ke , 1 , &ke1 , "\0\0" , 1 , &b_err);
                a06inc_linkage(ke.integer.C_2.slinkage);
                a06inc_linkage(ke1.integer.C_2.slinkage);
                i = sql__succ(i , -2147483647 , 2147483647 , "vak660.p" , 16459);
                }
            (*(tsp1_segment *) sql__nil(ptoc_Var68->a_return_segm , "vak660.p" , 16462)).integer.C_4.sp1r_returncode = (short ) sql__setrang((short ) aux_rc , -32768 , 32767);
            if ((int ) b_err != 0)
                {
                a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                }
            }
        }
    cccprint("<ak660subquery  ");
    }



int ak660three_phase(acv, dmli, select_rec, sr_rec)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_select_record *select_rec;
tak70_strat_rec *sr_rec;
    {
    tgg_basis_error b_err;
    char where_corr;
    char having_corr;
    char hd_union;
    char res_file_exists;
    tak_cmd_part_type h_cpt;
    tak_corr_type corr_type;
    int curr_n;
    int hd_maxcounttabs;
    int i;
    int m_filled_bytes;
    int m_return;
    int m_data_len;
    int next_n;
    int where_n;
    int aux_data_len;
    int old_infolen;
    int start_tabno;
    tgg00_FileId new_res_tree;
    tak_sysbufferaddress hbuf;
    store_for_endpart sfe_rec;
    store_for_endpart inner_sfe_rec;
    tak_all_command_glob *ptoc_Var71;
    struct tgg00_MessBlock *ptoc_Var72;
    tak_dml_info *ptoc_Var73;
    tak_select_record *ptoc_Var74;
    store_for_endpart *ptoc_Var75;
    int ptoc_Var76;
    struct ptoc_Type39 *ptoc_Var77;
    tak_parsinforecord *ptoc_Var78;
    cccprint("AK660 ak660three");
    ptoc_Var71 = &*acv;
    ptoc_Var72 = &ptoc_Var71->a_mblock;
    ptoc_Var73 = &*dmli;
    ptoc_Var74 = &*select_rec;
    ptoc_Var75 = &sfe_rec;
    old_infolen = 0;
    ptoc_Var75->old_subcntlevel = -1;
    ptoc_Var75->m2type = 0;
    if ((int ) ptoc_Var73->d_phase_cnt < 3)
        {
        ptoc_Var73->d_phase_cnt = 3;
        }
    ptoc_Var73->d_reuse = 1;
    res_file_exists = 0;
    ptoc_Var75->last_res_tree = ptoc_Var74->sel_res_tree;
    ptoc_Var75->h_use_rowno = (char ) sql__setrang((*sr_rec).sr_use_rowno , 0 , 1);
    (*sr_rec).sr_use_rowno = 0;
    hd_maxcounttabs = (int ) ptoc_Var73->d_maxcounttabs;
    ptoc_Var75->lowpars = (unsigned char)'\377';
    t01int4(5 , "d_Group     " , sql__ord((int ) (char ) sql__setrang(ptoc_Var73->d_group , 0 , 1)));
    t01int4(5 , "use_subquery" , sql__ord((int ) (char ) sql__setrang((*sr_rec).sr_use_subquery , 0 , 1)));
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16527)).integer.C_4.sp1r_returncode == 0 && (char ) sql__setrang(ptoc_Var73->d_use_order , 0 , 1))
        {
        a67_update_atinoutpos(&(*acv) , &(*dmli) , 1);
        }
    ptoc_Var73->d_group = 1;
    ptoc_Var73->d_join = 0;
    curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var71->a_ap_tree , "vak660.p" , 16533))[sql__setrang((int ) ptoc_Var74->sel_output_n , 0 , 32767)].n_sa_level;
    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var71->a_ap_tree , "vak660.p" , 16534))[sql__setrang(curr_n , 0 , 32767)].n_proc == 51 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var71->a_ap_tree , "vak660.p" , 16535))[sql__setrang(curr_n , 0 , 32767)].n_subproc == 3)
        {
        curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var71->a_ap_tree , "vak660.p" , 16537))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
        }
    t01int4(5 , "d_reclen    " , (int ) ptoc_Var73->d_reclen);
    t01int4(5 , "d_inoutpos  " , (int ) ptoc_Var73->d_inoutpos);
    ptoc_Var75->hinoutpos = (int ) ptoc_Var73->d_inoutpos;
    t01int4(5 , "d_Group     " , sql__ord((int ) (char ) sql__setrang(ptoc_Var73->d_group , 0 , 1)));
    having_corr = 0;
    where_corr = (char ) sql__setrang((ptoc_Var76 = (int ) ptoc_Var73->d_corr) == 2 || ptoc_Var76 == 1 , 0 , 1);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16545)).integer.C_4.sp1r_returncode == 0 && (int ) ptoc_Var74->sel_having_n != 0)
        {
        ptoc_Var73->d_having = 1;
        if ((int ) ptoc_Var73->d_corr == 2 && !(char ) sql__setrang(ptoc_Var71->a_from_select , 0 , 1))
            {
            a67_first_corr(&(*acv) , &(*dmli));
            }
        if (((ptoc_Var76 = (int ) ptoc_Var73->d_corr) == 2 || ptoc_Var76 == 1) && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16555)).integer.C_4.sp1r_returncode == 0 && !(char ) sql__setrang(ptoc_Var71->a_from_select , 0 , 1))
            {
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16559)).integer.C_4.sp1r_returncode == 0)
                {
                h_cpt = (unsigned char ) sql__setrang(ptoc_Var71->a_cpart_type , 0 , 3);
                ptoc_Var71->a_cpart_type = 3;
                t01messblock(5 , "MESSBLOCK   " , &(*acv).a_mblock);
                t01int4(5 , "d_inoutpos b" , (int ) ptoc_Var73->d_inoutpos);
                t01int4(5 , "d_keylen   b" , (int ) ptoc_Var73->d_keylen);
                t01messblock(5 , "MESSBLOCK   " , &(*acv).a_mblock);
                a67_corr_search(&(*acv) , &(*dmli) , 1 , &where_corr , &having_corr , &start_tabno , 0 , &old_infolen , &ptoc_Var75->last_res_tree);
                t01int4(5 , "old_infolen1" , old_infolen);
                old_infolen = (int ) ptoc_Var73->d_inoutpos - (int ) ptoc_Var73->d_keylen + old_infolen + -9;
                t01int4(5 , "old_infolen2" , old_infolen);
                t01int4(5 , "d_reclen 2  " , (int ) ptoc_Var73->d_reclen);
                t01int4(5 , "d_inoutpos h" , (int ) ptoc_Var73->d_inoutpos);
                t01int4(5 , "d_keylen   h" , (int ) ptoc_Var73->d_keylen);
                t01int4(5 , "fill_byte 1a" , (int ) ptoc_Var73->d_filled_bytes);
                t01messblock(5 , "MESSBLOCK   " , &(*acv).a_mblock);
                m_filled_bytes = (int ) ptoc_Var73->d_filled_bytes;
                ptoc_Var71->a_cpart_type = (unsigned char ) sql__setrang(h_cpt , 0 , 3);
                }
            }
        next_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var71->a_ap_tree , "vak660.p" , 16588))[sql__setrang((int ) ptoc_Var74->sel_having_n , 0 , 32767)].n_lo_level;
        if ((int ) ptoc_Var71->a_ex_kind == 0)
            {
            a54_fixedpos(&(*acv) , &(*dmli));
            }
        a65_search_condition(&(*acv) , &(*dmli) , &next_n);
        ptoc_Var73->d_having = 0;
        ptoc_Var73->d_joins.jrc_cnt = 0;
        }
    t01int4(5 , "d_reclen    " , (int ) ptoc_Var73->d_reclen);
    t01int4(5 , "d_inoutpos  " , (int ) ptoc_Var73->d_inoutpos);
    t01messblock(5 , "MESSBLOCK   " , &(*acv).a_mblock);
    ptoc_Var75->messbuf_stored = 0;
    if ((int ) ptoc_Var71->a_ex_kind == 0 && !(char ) sql__setrang(ptoc_Var73->d_only_sem_check , 0 , 1) && ptoc_Var73->d_sparr.pparsp != (struct tak_systembuffer *) (int *) 0)
        {
        ptoc_Var75->param_parskey = (*(struct tak_systembuffer *) sql__nil(ptoc_Var73->d_sparr.pparsp , "vak660.p" , 16607)).integer.C_50.syskey;
        }
    if ((char ) sql__setrang(ptoc_Var73->d_only_sem_check , 0 , 1))
        {
        ptoc_Var75->hd_single = 0;
        }
    else
        {
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16614)).integer.C_4.sp1r_returncode == 0)
            {
            ak660move_in_hbuf(&(*acv) , &sfe_rec , 1 , 0);
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16619)).integer.C_4.sp1r_returncode == 0)
                {
                ptoc_Var75->messbuf_stored = 1;
                ptoc_Var75->hmbp = (struct tgg00_MessBlock *) &(*(struct tak_systembuffer *) sql__nil(ptoc_Var75->hbuf , "vak660.p" , 16623)).integer.C_62.smessblock.mbr_mess_block;
                ptoc_Var75->hqbp = (*(struct tgg00_MessBlock *) sql__nil(ptoc_Var75->hmbp , "vak660.p" , 16624)).integer.C_4.mb_qual;
                (*(struct tgg00_MessBlock *) sql__nil(ptoc_Var75->hmbp , "vak660.p" , 16625)).integer.C_2.mb_type = 60;
                (*(struct tgg00_MessBlock *) sql__nil(ptoc_Var75->hmbp , "vak660.p" , 16626)).integer.C_2.mb_type2 = 0;
                (*(tgg00_QualBuf *) sql__nil(ptoc_Var75->hqbp , "vak660.p" , 16627)).integer.C_2.msubquery = (char ) sql__setrang((*(tgg00_QualBuf *) sql__nil(ptoc_Var72->integer.C_4.mb_qual , "vak660.p" , 16627)).integer.C_2.msubquery , 0 , 1);
                }
            if ((int ) ptoc_Var74->sel_group_n != 0)
                {
                ptoc_Var73->d_use_order = 1;
                ptoc_Var73->d_order_or_group_cols = (tak00_ord_fields *) &ptoc_Var73->d_group_cols;
                }
            else
                {
                ptoc_Var73->d_use_order = 0;
                }
            }
        t01name(5 , "nach group        ");
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16647)).integer.C_4.sp1r_returncode == 0)
            {
            t01stqual(5 , sql__nil(ptoc_Var75->hqbp , "vak660.p" , 16649) , (*(struct tgg00_MessBlock *) sql__nil(ptoc_Var75->hmbp , "vak660.p" , 16649)).integer.C_5.mb_st);
            }
        t01messblock(5 , "MESSBLOCK   " , &(*acv).a_mblock);
        t01int4(5 , "d_Group     " , sql__ord((int ) (char ) sql__setrang(ptoc_Var73->d_group , 0 , 1)));
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16653)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var75->lowpars = (unsigned char ) sql__setrang(ptoc_Var73->d_lowpars , 0 , 255);
            if ((int ) ptoc_Var73->d_corr != 0)
                {
                ptoc_Var75->lowpars = (unsigned char ) sql__setrang((unsigned char ) ((int ) ptoc_Var73->d_lowpars + -1) , 0 , 255);
                ptoc_Var73->d_lowpars = (unsigned char ) sql__setrang((unsigned char ) ((int ) ptoc_Var73->d_lowpars + -1) , 0 , 255);
                }
            t01int4(5 , "thre lowpars" , (int ) ptoc_Var75->lowpars);
            ptoc_Var73->d_inoutpos = 13;
            ptoc_Var75->hkeylen = (int ) ptoc_Var73->d_keylen;
            ptoc_Var73->d_keylen = 4;
            ptoc_Var75->hreclen = (int ) ptoc_Var73->d_reclen;
            ptoc_Var73->d_reclen = (short ) sql__setrang(ptoc_Var73->d_inoutpos , -32768 , 32767);
            ptoc_Var75->hd_distinct = (unsigned char ) sql__setrang(ptoc_Var73->d_distinct , 0 , 2);
            ptoc_Var73->d_distinct = 0;
            hd_union = (char ) sql__setrang(ptoc_Var73->d_union , 0 , 1);
            ptoc_Var73->d_union = 0;
            ptoc_Var75->hd_single = (char ) sql__setrang(ptoc_Var73->d_single , 0 , 1);
            ptoc_Var73->d_single = 0;
            ptoc_Var75->hd_colcount = (int ) ptoc_Var73->d_change.cr_colcount;
            ptoc_Var73->d_change.cr_colcount = 0;
            ptoc_Var75->hd_rowno = (int ) ptoc_Var73->d_rowno;
            ptoc_Var73->d_rowno = -1;
            ak660two_repl_complex_output(&(*acv) , &(*dmli) , &ptoc_Var75->hbuf , 0 , (char ) sql__setrang(ptoc_Var74->sel_view_done , 0 , 1) , 1);
            if ((int ) ptoc_Var71->a_mblock.integer.C_2.mb_type2 == 67)
                {
                a61_put_last_func(&(*acv));
                }
            a61_set_jump(&ptoc_Var71->a_mblock , 1 , 33);
            ptoc_Var77 = &(*(tgg00_StackList *) sql__nil(ptoc_Var71->a_mblock.integer.C_5.mb_st , "vak660.p" , 16688))[0];
            ptoc_Var77->integer.C_1.epos = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var77->integer.C_1.epos , -32768 , 32767 , "vak660.p" , 16689) , -32768 , 32767);
            t01int4(5 , "d_Group     " , sql__ord((int ) (char ) sql__setrang(ptoc_Var73->d_group , 0 , 1)));
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16692)).integer.C_4.sp1r_returncode == 0 && (int ) ptoc_Var74->sel_group_n != 0 && (int ) ptoc_Var73->d_maxcounttabs == 1)
                {
                if ((ptoc_Var76 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var71->a_ap_tree , "vak660.p" , 16697))[sql__setrang((int ) ptoc_Var74->sel_output_n , 0 , 32767)].n_symb) != 57 && ptoc_Var76 != 91 && ptoc_Var76 != 61)
                    {
                    ptoc_Var73->d_distinct = 1;
                    }
                a67_update_atinoutpos(&(*acv) , &(*dmli) , 0);
                }
            }
        next_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var71->a_ap_tree , "vak660.p" , 16707))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
        ptoc_Var73->d_group = 0;
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16709)).integer.C_4.sp1r_returncode == 0)
            {
            a10add_sysinfo(&(*acv) , &ptoc_Var75->hbuf , &b_err);
            if ((int ) b_err != 0)
                {
                a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                }
            else
                {
                a10_rel_sysinfo(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var75->hbuf , "vak660.p" , 16717)).integer.C_50.syskey);
                }
            }
        }
    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var71->a_ap_tree , "vak660.p" , 16723))[sql__setrang(curr_n , 0 , 32767)].n_proc == 57 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var71->a_ap_tree , "vak660.p" , 16724))[sql__setrang(curr_n , 0 , 32767)].n_subproc == 1)
        {
        curr_n = next_n;
        }
    next_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var71->a_ap_tree , "vak660.p" , 16728))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
    ptoc_Var73->d_group = 0;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16730)).integer.C_4.sp1r_returncode == 0 && next_n != 0)
        {
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var71->a_ap_tree , "vak660.p" , 16732))[sql__setrang(next_n , 0 , 32767)].n_proc == 54 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var71->a_ap_tree , "vak660.p" , 16733))[sql__setrang(next_n , 0 , 32767)].n_subproc == 10)
            {
            if ((int ) ptoc_Var73->d_maxcounttabs == 1)
                {
                next_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var71->a_ap_tree , "vak660.p" , 16739))[sql__setrang(next_n , 0 , 32767)].n_lo_level;
                if ((int ) ptoc_Var71->a_ex_kind == 0)
                    {
                    a54_fixedpos(&(*acv) , &(*dmli));
                    }
                a65_search_condition(&(*acv) , &(*dmli) , &next_n);
                }
            else
                {
                ptoc_Var73->d_group = 0;
                ptoc_Var73->d_join = 1;
                ptoc_Var73->d_joins.jrc_cnt = 0;
                where_n = next_n;
                next_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var71->a_ap_tree , "vak660.p" , 16752))[sql__setrang(next_n , 0 , 32767)].n_lo_level;
                if ((int ) ptoc_Var71->a_ex_kind == 0)
                    {
                    a54_fixedpos(&(*acv) , &(*dmli));
                    }
                a65_search_condition(&(*acv) , &(*dmli) , &next_n);
                ptoc_Var73->d_join = 0;
                }
            ptoc_Var74->sel_where_done = 1;
            }
        }
    t01name(5 , "vor two_phase     ");
    if ((int ) ptoc_Var73->d_maxcounttabs > (int ) ptoc_Var73->d_cntfromtab && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16767)).integer.C_4.sp1r_returncode == 0)
        {
        a67_put_all_views_into(&(*acv) , &(*dmli));
        ptoc_Var74->sel_view_done = 1;
        }
    ptoc_Var75->first_res = (char ) sql__setrang((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16774)).integer.C_4.sp1r_returncode == 0 , 0 , 1);
    ptoc_Var73->d_group = (char ) sql__setrang((int ) ptoc_Var74->sel_group_n != 0 , 0 , 1);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16776)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) ptoc_Var71->a_ex_kind == 0 && !(char ) sql__setrang(ptoc_Var73->d_only_sem_check , 0 , 1) && ptoc_Var73->d_sparr.pparsp != (struct tak_systembuffer *) (int *) 0)
            {
            ptoc_Var78 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var73->d_sparr.pparsp , "vak660.p" , 16783)).integer.C_23.sparsinfo;
            ptoc_Var75->const_par_expr = (char ) sql__setrang((*(struct tak_systembuffer *) sql__nil(ptoc_Var73->d_sparr.pparsp , "vak660.p" , 16785)).integer.C_23.sparsinfo.integer.C_2.p_const_param_expr , 0 , 1);
            ptoc_Var75->old_subcntlevel = (short ) sql__setrang(ptoc_Var78->integer.C_2.p_subcntlevel , -32768 , 32767);
            ptoc_Var78->integer.C_2.p_subcntlevel = -1;
            ptoc_Var76 = ptoc_Var78->integer.C_2.p_cnt_infos;
            if (1 <= ptoc_Var76)
                {
                i = 1;
                do
                    {
                    if ((int ) ptoc_Var78->integer.C_2.p_pars_infos[sql__setrang(i , 1 , 2025) + -1].fp_kind == 20)
                        {
                        ptoc_Var78->integer.C_2.p_pars_infos[sql__setrang(i , 1 , 2025) + -1].tak_fp_kind_type.C_fp_val_all_without_l.fp_stackpos = 0;
                        }
                    if (i == ptoc_Var76)
                        {
                        break;
                        }
                    i += 1;
                    }
                while (!0);
                }
            }
        else
            {
            ptoc_Var75->const_par_expr = 0;
            }
        g04build_temp_tree_id(&ptoc_Var74->sel_res_tree , &ptoc_Var71->a_transinf.tri_trans);
        ptoc_Var74->sel_res_tree.integer.C_8.fileTfnTemp_gg00 = 1;
        ptoc_Var74->sel_res_tree.integer.C_8.fileLevel_gg00[0] = (unsigned char ) sql__setrang((unsigned char ) ptoc_Var71->a_pars_last_key.p_no , 0 , 255);
        ptoc_Var74->sel_res_tree.integer.C_8.fileLevel_gg00[1] = (unsigned char)'\0';
        m_data_len = ptoc_Var72->integer.C_2.mb_data_len;
        corr_type = (unsigned char ) sql__setrang(ptoc_Var73->d_corr , 0 , 4);
        if (((ptoc_Var76 = (int ) ptoc_Var73->d_corr) == 2 || ptoc_Var76 == 1) && !(char ) sql__setrang(where_corr , 0 , 1))
            {
            ptoc_Var73->d_corr = 0;
            }
        if ((int ) ptoc_Var73->d_maxcounttabs == 1 && ((int ) ptoc_Var71->a_mblock.integer.C_2.mb_type2 != 67 || (ptoc_Var76 = (int ) ptoc_Var73->d_corr) != 2 && ptoc_Var76 != 1))
            {
            ptoc_Var73->d_corr = 0;
            a67_sel2_second_part(&(*acv) , &(*dmli) , &ptoc_Var74->sel_res_tree , 1 , 0 , (char ) sql__setrang(ptoc_Var74->sel_view_done , 0 , 1) , &(*sr_rec));
            }
        else
            {
            if ((char ) sql__setrang(where_corr , 0 , 1) && (int ) ptoc_Var74->sel_having_n != 0)
                {
                ptoc_Var73->d_corr = 1;
                }
            t01int4(5 , "d_corr      " , sql__ord((int ) ptoc_Var73->d_corr));
            t01int4(5 , "fill_byte 2a" , (int ) ptoc_Var73->d_filled_bytes);
            m_filled_bytes = (int ) ptoc_Var73->d_filled_bytes;
            t01int4(5 , "d_inoutpos a" , (int ) ptoc_Var73->d_inoutpos);
            t01int4(5 , "d_reclen    " , (int ) ptoc_Var73->d_reclen);
            ptoc_Var73->d_join = 1;
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16837)).integer.C_4.sp1r_returncode == 0 && (!(char ) sql__setrang(ptoc_Var73->d_group , 0 , 1) || (int ) ptoc_Var73->d_maxcounttabs != 1) && ((char ) sql__setrang(ptoc_Var73->d_group , 0 , 1) && (int ) ptoc_Var73->d_group_cols.ocntord > 0 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var71->a_ap_tree , "vak660.p" , 16843))[sql__setrang((int ) ptoc_Var74->sel_output_n , 0 , 32767)].n_symb == 61 && (char ) sql__setrang(ptoc_Var73->d_use_order , 0 , 1)))
                {
                a67_update_atinoutpos(&(*acv) , &(*dmli) , 0);
                }
            t01int4(5 , "d_inoutpos b" , (int ) ptoc_Var73->d_inoutpos);
            t01int4(5 , "d_reclen    " , (int ) ptoc_Var73->d_reclen);
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16850)).integer.C_4.sp1r_returncode == 0)
                {
                ak660more_phase(&(*acv) , &(*dmli) , &(*select_rec) , &(*sr_rec) , &inner_sfe_rec , 1);
                ak660two_phase(&(*acv) , &(*dmli) , &(*select_rec) , &(*sr_rec) , &inner_sfe_rec , 1);
                }
            t01int4(5 , "fill_byte 3a" , (int ) ptoc_Var73->d_filled_bytes);
            }
        ptoc_Var73->d_corr = (unsigned char ) sql__setrang(corr_type , 0 , 4);
        if ((int ) ptoc_Var71->a_ex_kind != 0 && !(char ) sql__setrang(ptoc_Var73->d_only_sem_check , 0 , 1))
            {
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16867)).integer.C_4.sp1r_returncode == 0)
                {
                ptoc_Var74->sel_res_tree = (*(tgg00_QualBuf *) sql__nil(ptoc_Var71->a_mblock.integer.C_4.mb_qual , "vak660.p" , 16870)).integer.C_4.mr_restree;
                res_file_exists = (char ) sql__setrang((_ptoc_MEMCMP(unsigned char , 7, (*(tgg00_QualBuf *) sql__nil(ptoc_Var71->a_mblock.integer.C_4.mb_qual , "vak660.p" , 16872)).integer.C_4.mr_resnum , "\0\200\0\0\0\0\0") == 0 ? 1 : 0) && (int ) ptoc_Var75->m2type != 67 , 0 , 1);
                }
            else
                {
                a502destroy_file(&(*acv) , &ptoc_Var74->sel_res_tree);
                }
            }
        ptoc_Var72->integer.C_2.mb_data_len = m_data_len;
        t01name(5 , "nach two_phase    ");
        t01messblock(5 , "MESSBLOCK   " , &(*acv).a_mblock);
        if ((int ) ptoc_Var71->a_ex_kind == 0 && !(char ) sql__setrang(ptoc_Var73->d_only_sem_check , 0 , 1))
            {
            if (ptoc_Var73->d_sparr.pparsp != (struct tak_systembuffer *) (int *) 0)
                {
                t01buf1(5 , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var73->d_sparr.pparsp , "vak660.p" , 16887)).integer.C_50.syskey , 1 , 16);
                }
            }
        t01int4(5 , "d_reclen  3 " , (int ) ptoc_Var73->d_reclen);
        t01int4(5 , "d_inoutpos  " , (int ) ptoc_Var73->d_inoutpos);
        t01int4(5 , "d_keylen    " , (int ) ptoc_Var73->d_keylen);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16895)).integer.C_4.sp1r_returncode == 0 && !(char ) sql__setrang(ptoc_Var73->d_only_sem_check , 0 , 1))
        {
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16898)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var73->d_single = (char ) sql__setrang(ptoc_Var75->hd_single , 0 , 1);
            ptoc_Var73->d_change.cr_colcount = (short ) sql__setrang((short ) ptoc_Var75->hd_colcount , -32768 , 32767);
            ptoc_Var73->d_rowno = (short ) sql__setrang((short ) ptoc_Var75->hd_rowno , -32768 , 32767);
            ptoc_Var73->d_distinct = (unsigned char ) sql__setrang(ptoc_Var75->hd_distinct , 0 , 2);
            ptoc_Var73->d_union = (char ) sql__setrang(hd_union , 0 , 1);
            }
        (*sr_rec).sr_use_rowno = (char ) sql__setrang(ptoc_Var75->h_use_rowno , 0 , 1);
        if ((char ) sql__setrang(having_corr , 0 , 1))
            {
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16912)).integer.C_4.sp1r_returncode == 0)
                {
                a10get_sysinfo(&(*acv) , &ptoc_Var75->ke , 1 , &ptoc_Var75->hbuf , &b_err);
                if ((int ) b_err != 0)
                    {
                    a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                    }
                }
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16922)).integer.C_4.sp1r_returncode == 0)
                {
                ptoc_Var75->hmbp = (struct tgg00_MessBlock *) &(*(struct tak_systembuffer *) sql__nil(ptoc_Var75->hbuf , "vak660.p" , 16925)).integer.C_62.smessblock.mbr_mess_block;
                ptoc_Var75->hqbp = (*(struct tgg00_MessBlock *) sql__nil(ptoc_Var75->hmbp , "vak660.p" , 16926)).integer.C_4.mb_qual;
                aux_data_len = ptoc_Var71->a_mblock.integer.C_2.mb_data_len;
                ptoc_Var71->a_mblock.integer.C_2.mb_data_len = 0;
                a06cpy_mblock(&(*acv) , sql__nil(ptoc_Var75->hmbp , "vak660.p" , 16929) , &ptoc_Var71->a_mblock , &b_err);
                ptoc_Var71->a_mblock.integer.C_2.mb_data_len = aux_data_len;
                if ((int ) b_err != 0)
                    {
                    a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                    }
                else
                    {
                    t01messblock(5 , "mess_block <" , sql__nil(ptoc_Var75->hmbp , "vak660.p" , 16936));
                    t01messblock(5 , "MESSBLOCK   " , &ptoc_Var71->a_mblock);
                    }
                (*acv).a_mblock.integer.C_2.mb_struct = 9;
                (*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak660.p" , 16941)).integer.C_2.mtree = ptoc_Var74->sel_res_tree;
                if ((int ) (*acv).a_mblock.integer.C_2.mb_type2 == 0)
                    {
                    ptoc_Var73->d_maxcounttabs = 1;
                    }
                }
            t01int4(5 , "d_reclen 4  " , (int ) ptoc_Var73->d_reclen);
            t01int4(5 , "d_inoutpos  " , (int ) ptoc_Var73->d_inoutpos);
            t01int4(5 , "d_keylen    " , (int ) ptoc_Var73->d_keylen);
            t01messblock(5 , "MESSBLOCK   " , &ptoc_Var71->a_mblock);
            ptoc_Var73->d_inoutpos = (short ) sql__setrang((short ) ptoc_Var75->hinoutpos , -32768 , 32767);
            ptoc_Var73->d_keylen = (short ) sql__setrang((short ) ptoc_Var75->hkeylen , -32768 , 32767);
            ptoc_Var73->d_reclen = (short ) sql__setrang((short ) ptoc_Var75->hreclen , -32768 , 32767);
            t01int4(5 , "fill_byte 4a" , (int ) ptoc_Var73->d_filled_bytes);
            ptoc_Var73->d_filled_bytes = (short ) sql__setrang((short ) m_filled_bytes , -32768 , 32767);
            t01int4(5 , "fill_byte 5a" , (int ) ptoc_Var73->d_filled_bytes);
            (*sr_rec).sr_use_subquery = 1;
            new_res_tree = ptoc_Var73->d_tabarr[0].otreeid;
            ptoc_Var73->d_tabarr[0].otreeid = ptoc_Var74->sel_res_tree;
            a67_corr_search(&(*acv) , &(*dmli) , 0 , &where_corr , &having_corr , &start_tabno , 1 , &old_infolen , &ptoc_Var75->last_res_tree);
            t01int4(5 , "hav oldlowpa" , (int ) ptoc_Var73->d_oldlowpars);
            t01int4(5 , "hav lowpars " , (int ) ptoc_Var73->d_lowpars);
            ptoc_Var73->d_oldlowpars = (unsigned char ) sql__setrang(ptoc_Var73->d_lowpars , 0 , 255);
            ptoc_Var73->d_tabarr[0].otreeid = new_res_tree;
            t01messblock(5 , "MESSBLOCK   " , &ptoc_Var71->a_mblock);
            t01name(5 , "nach corr_search  ");
            if (ptoc_Var73->d_sparr.pparsp != (struct tak_systembuffer *) (int *) 0 && !(char ) sql__setrang(ptoc_Var73->d_only_sem_check , 0 , 1))
                {
                t01buf1(5 , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var73->d_sparr.pparsp , "vak660.p" , 16973)).integer.C_50.syskey , 1 , 16);
                }
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 16975)).integer.C_4.sp1r_returncode == 0)
                {
                ak660move_in_hbuf(&(*acv) , &sfe_rec , 1 , 1);
                }
            a10repl_sysinfo(&(*acv) , &ptoc_Var75->hbuf , &b_err);
            }
        if (!(char ) sql__setrang(ptoc_Var73->d_subquery , 0 , 1) && !(char ) sql__setrang(ptoc_Var73->d_union , 0 , 1) && !(char ) sql__setrang(ptoc_Var71->a_from_select , 0 , 1) && (int ) ptoc_Var71->a_intern_select_cnt == (int ) ptoc_Var71->a_max_intern_select && !(char ) sql__setrang(ptoc_Var71->a_insert_select , 0 , 1))
            {
            ptoc_Var74->sel_last_pars_part = 1;
            }
        t01messblock(5 , "MESSBLOCK   " , &ptoc_Var71->a_mblock);
        t01name(5 , "vor endpart       ");
        if (ptoc_Var73->d_sparr.pparsp != (struct tak_systembuffer *) (int *) 0 && !(char ) sql__setrang(ptoc_Var73->d_only_sem_check , 0 , 1))
            {
            t01buf1(5 , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var73->d_sparr.pparsp , "vak660.p" , 16996)).integer.C_50.syskey , 1 , 16);
            }
        ak660endpart(&(*acv) , &(*dmli) , &sfe_rec , (int ) ptoc_Var74->sel_order_n , (char ) sql__setrang(ptoc_Var74->sel_last_pars_part , 0 , 1) , 1 , (char ) sql__setrang((int ) corr_type != 0 , 0 , 1) , &ptoc_Var74->sel_res_tree , &(*select_rec) , &(*sr_rec));
        ptoc_Var73->d_inoutpos = (short ) sql__setrang((short ) ptoc_Var75->hinoutpos , -32768 , 32767);
        t01int4(5 , "d_reclen    " , (int ) ptoc_Var73->d_reclen);
        t01int4(5 , "d_inoutpos  " , (int ) ptoc_Var73->d_inoutpos);
        new_res_tree = ptoc_Var74->sel_res_tree;
        ptoc_Var74->sel_res_tree = ptoc_Var75->last_res_tree;
        if (ptoc_Var71->a_union_cnt <= 1)
            {
            a67_sel3_second_part(&(*acv) , &(*dmli) , &(*select_rec) , &(*sr_rec));
            }
        ptoc_Var73->d_maxcounttabs = (short ) sql__setrang((short ) hd_maxcounttabs , -32768 , 32767);
        if ((char ) sql__setrang(ptoc_Var75->first_res , 0 , 1) && (char ) sql__setrang(res_file_exists , 0 , 1) && (int ) ptoc_Var71->a_ex_kind != 0)
            {
            a502destroy_file(&(*acv) , &new_res_tree);
            if ((int ) ptoc_Var71->a_transinf.tri_trans.trError_gg00 != 0)
                {
                a07_b_put_error(&(*acv) , (short ) sql__setrang(ptoc_Var71->a_transinf.tri_trans.trError_gg00 , -32768 , 32767) , 1);
                }
            }
        }
    if ((char ) sql__setrang(ptoc_Var75->messbuf_stored , 0 , 1))
        {
        m_return = (int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 17030)).integer.C_4.sp1r_returncode;
        (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 17031)).integer.C_4.sp1r_returncode = 0;
        a10del_sysinfo(&(*acv) , &ptoc_Var75->ke , &b_err);
        if (m_return != 0)
            {
            (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 17035)).integer.C_4.sp1r_returncode = (short ) sql__setrang((short ) m_return , -32768 , 32767);
            }
        }
    t01int4(5 , "ACOMMANDKIND" , sql__ord((int ) ptoc_Var71->a_command_kind));
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 17041)).integer.C_4.sp1r_returncode != 0 && !(char ) sql__setrang(ptoc_Var73->d_only_sem_check , 0 , 1) && (int ) ptoc_Var71->a_command_kind == 5)
        {
        m_return = (int ) (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 17046)).integer.C_4.sp1r_returncode;
        (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 17047)).integer.C_4.sp1r_returncode = 0;
        a10del_sysinfo(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var73->d_sparr.pparsp , "vak660.p" , 17048)).integer.C_50.syskey , &b_err);
        if (m_return != 0)
            {
            (*(tsp1_segment *) sql__nil(ptoc_Var71->a_return_segm , "vak660.p" , 17051)).integer.C_4.sp1r_returncode = (short ) sql__setrang((short ) m_return , -32768 , 32767);
            }
        }
    cccprint("<ak660three_phas");
    }



int ak660one_phase(acv, dmli, select_rec, sr_rec, sfe_rec)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_select_record *select_rec;
tak70_strat_rec *sr_rec;
store_for_endpart *sfe_rec;
    {
    tgg_basis_error b_err;
    char first;
    char res_file_exists;
    int m_return;
    int ix;
    int qstart;
    tak_all_command_glob *ptoc_Var79;
    tak_dml_info *ptoc_Var80;
    tak_joinrec *ptoc_Var81;
    tak_select_record *ptoc_Var82;
    store_for_endpart *ptoc_Var83;
    struct tgg00_MessBlock *ptoc_Var84;
    tgg00_QualBuf *ptoc_Var85;
    int ptoc_Var86;
    struct ptoc_Type111 *ptoc_Var87;
    cccprint("AK660 ak660one_p");
    ptoc_Var79 = &*acv;
    ptoc_Var80 = &*dmli;
    ptoc_Var81 = &ptoc_Var80->d_joins;
    ptoc_Var82 = &*select_rec;
    ptoc_Var83 = &*sfe_rec;
    res_file_exists = 0;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var79->a_return_segm , "vak660.p" , 17083)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var84 = &ptoc_Var79->a_mblock;
        ptoc_Var85 = (tgg00_QualBuf *) sql__nil(ptoc_Var84->integer.C_4.mb_qual , "vak660.p" , 17086);
        ptoc_Var85->integer.C_2.mresqual_cnt = 0;
        ptoc_Var85->integer.C_2.mresqual_pos = (short ) sql__setrang(ptoc_Var85->integer.C_2.mqual_pos , -32768 , 32767);
        qstart = (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(ptoc_Var83->hmbp , "vak660.p" , 17090)).integer.C_5.mb_st , "vak660.p" , 17090))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var83->hqbp , "vak660.p" , 17090)).integer.C_2.mqual_pos , 1 , 32767) + -1].integer.C_1.epos;
        ix = (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var83->hqbp , "vak660.p" , 17092)).integer.C_2.mfirst_free - (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(ptoc_Var83->hmbp , "vak660.p" , 17093)).integer.C_5.mb_st , "vak660.p" , 17093))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var83->hqbp , "vak660.p" , 17093)).integer.C_2.mqual_pos , 1 , 32767) + -1].integer.C_1.epos;
        t01int4(5 , "ix          " , ix);
        t01int4(5 , "mresqual_cnt" , (int ) ptoc_Var85->integer.C_2.mresqual_cnt);
        ptoc_Var85->integer.C_2.mresqual_cnt = (short ) sql__setrang((short ) ((int ) ptoc_Var85->integer.C_2.mresqual_cnt + ix) , -32768 , 32767);
        qstart = (int ) ptoc_Var85->integer.C_2.mqual_pos + (int ) ptoc_Var85->integer.C_2.mresqual_cnt;
        t01int4(5 , "ix          " , ix);
        if ((int ) ptoc_Var85->integer.C_2.mresqual_cnt + (int ) ptoc_Var85->integer.C_2.mfirst_free < (int ) ptoc_Var84->integer.C_2.mb_st_max)
            {
            g10mvr("VAK660" , 2 , ptoc_Var84->integer.C_2.mb_st_size , ptoc_Var84->integer.C_2.mb_st_size , sql__nil(ptoc_Var84->integer.C_5.mb_st , "vak660.p" , 17104) , (int ) ptoc_Var85->integer.C_2.mqual_pos * 8 + -7 , sql__nil(ptoc_Var84->integer.C_5.mb_st , "vak660.p" , 17105) , qstart * 8 + -7 , (int ) ptoc_Var85->integer.C_2.mqual_cnt * 8 , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 17107)).integer.C_4.sp1r_returncode);
            ptoc_Var85->integer.C_2.mqual_pos = (short ) sql__setrang((short ) qstart , -32768 , 32767);
            t01int4(5 , "qstart      " , qstart);
            t01messblock(5 , "MESSBLOCKoj1" , &(*acv).a_mblock);
            if ((int ) ptoc_Var85->integer.C_2.mresqual_cnt > 0 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 17113)).integer.C_4.sp1r_returncode == 0)
                {
                ptoc_Var85->integer.C_2.mresqual_cnt = 0;
                ptoc_Var85->integer.C_2.mresqual_pos = 1;
                ix = (int ) (short ) sql__succ((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var83->hqbp , "vak660.p" , 17118)).integer.C_2.mqual_pos , -32768 , 32767 , "vak660.p" , 17118);
                qstart = (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(ptoc_Var83->hmbp , "vak660.p" , 17119)).integer.C_5.mb_st , "vak660.p" , 17119))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var83->hqbp , "vak660.p" , 17119)).integer.C_2.mqual_pos , 1 , 32767) + -1].integer.C_1.epos;
                while (ix < qstart)
                    if ((int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(ptoc_Var83->hmbp , "vak660.p" , 17123)).integer.C_5.mb_st , "vak660.p" , 17123))[sql__setrang(ix , 1 , 32767) + -1].integer.C_1.etype == 2 && (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(ptoc_Var83->hmbp , "vak660.p" , 17124)).integer.C_5.mb_st , "vak660.p" , 17124))[sql__setrang(ix + 1 , 1 , 32767) + -1].integer.C_1.etype == 28)
                        {
                        if ((int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(ptoc_Var83->hmbp , "vak660.p" , 17127)).integer.C_5.mb_st , "vak660.p" , 17127))[sql__setrang(ix + 1 , 1 , 32767) + -1].integer.C_3.eop_out != 4)
                            {
                            ix = ix + 2;
                            }
                        else
                            {
                            ix = ix + 3;
                            }
                        }
                    else
                        {
                        if ((int ) ptoc_Var85->integer.C_2.mresqual_cnt == 0)
                            {
                            ptoc_Var85->integer.C_2.mresqual_cnt = 1;
                            (*(tgg00_StackList *) sql__nil(ptoc_Var84->integer.C_5.mb_st , "vak660.p" , 17140))[sql__setrang((int ) ptoc_Var85->integer.C_2.mresqual_pos , 1 , 32767) + -1] = (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(ptoc_Var83->hmbp , "vak660.p" , 17141)).integer.C_5.mb_st , "vak660.p" , 17141))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var83->hqbp , "vak660.p" , 17141)).integer.C_2.mqual_pos , 1 , 32767) + -1];
                            }
                        first = 1;
                        do
                            {
                            if ((int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(ptoc_Var83->hmbp , "vak660.p" , 17147)).integer.C_5.mb_st , "vak660.p" , 17147))[sql__setrang(ix , 1 , 32767) + -1].integer.C_1.etype != 28 || !(char ) sql__setrang(first , 0 , 1) || (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(ptoc_Var83->hmbp , "vak660.p" , 17149)).integer.C_5.mb_st , "vak660.p" , 17149))[sql__setrang(ix , 1 , 32767) + -1].integer.C_3.eop_out != 4)
                                {
                                (*(tgg00_StackList *) sql__nil(ptoc_Var84->integer.C_5.mb_st , "vak660.p" , 17153))[sql__setrang((int ) ptoc_Var85->integer.C_2.mresqual_pos + (int ) ptoc_Var85->integer.C_2.mresqual_cnt , 1 , 32767) + -1] = (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(ptoc_Var83->hmbp , "vak660.p" , 17154)).integer.C_5.mb_st , "vak660.p" , 17154))[sql__setrang(ix , 1 , 32767) + -1];
                                ptoc_Var85->integer.C_2.mresqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var85->integer.C_2.mresqual_cnt , -32768 , 32767 , "vak660.p" , 17155) , -32768 , 32767);
                                }
                            first = 0;
                            ix = sql__succ(ix , -2147483647 , 2147483647 , "vak660.p" , 17159);
                            }
                        while (!(ix >= qstart || (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(ptoc_Var83->hmbp , "vak660.p" , 17162)).integer.C_5.mb_st , "vak660.p" , 17162))[sql__setrang(ix + -1 , 1 , 32767) + -1].integer.C_1.etype == 28 && (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(ptoc_Var83->hmbp , "vak660.p" , 17163)).integer.C_5.mb_st , "vak660.p" , 17163))[sql__setrang(ix + -1 , 1 , 32767) + -1].integer.C_3.eop_out != 4));
                        }
                if ((int ) ptoc_Var85->integer.C_2.mresqual_cnt > 0)
                    {
                    (*(tgg00_StackList *) sql__nil(ptoc_Var84->integer.C_5.mb_st , "vak660.p" , 17171))[sql__setrang((int ) ptoc_Var85->integer.C_2.mresqual_pos , 1 , 32767) + -1].integer.C_1.epos = (short ) sql__setrang((short ) ((int ) ptoc_Var85->integer.C_2.mresqual_cnt + 1) , -32768 , 32767);
                    }
                }
            qstart = (int ) ptoc_Var85->integer.C_2.mresqual_pos + (int ) ptoc_Var85->integer.C_2.mresqual_cnt + -1;
            ix = (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var83->hqbp , "vak660.p" , 17177)).integer.C_2.mfirst_free - (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(ptoc_Var83->hmbp , "vak660.p" , 17178)).integer.C_5.mb_st , "vak660.p" , 17178))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var83->hqbp , "vak660.p" , 17178)).integer.C_2.mqual_pos , 1 , 32767) + -1].integer.C_1.epos;
            ptoc_Var85->integer.C_2.mresqual_cnt = (short ) sql__setrang((short ) ((int ) ptoc_Var85->integer.C_2.mresqual_cnt + ix) , -32768 , 32767);
            if (ix > 0)
                {
                g10mv4("VAK660" , 3 , (*(struct tgg00_MessBlock *) sql__nil(ptoc_Var83->hmbp , "vak660.p" , 17183)).integer.C_2.mb_st_size , ptoc_Var84->integer.C_2.mb_st_size , sql__nil((*(struct tgg00_MessBlock *) sql__nil(ptoc_Var83->hmbp , "vak660.p" , 17183)).integer.C_5.mb_st , "vak660.p" , 17183) , (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(ptoc_Var83->hmbp , "vak660.p" , 17184)).integer.C_5.mb_st , "vak660.p" , 17184))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var83->hqbp , "vak660.p" , 17184)).integer.C_2.mqual_pos , 1 , 32767) + -1].integer.C_1.epos * 8 + -7 , sql__nil(ptoc_Var84->integer.C_5.mb_st , "vak660.p" , 17186) , qstart * 8 + 1 , ix * 8 , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 17188)).integer.C_4.sp1r_returncode);
                }
            ptoc_Var85->integer.C_2.mfirst_free = (short ) sql__setrang((short ) ((int ) ptoc_Var85->integer.C_2.mfirst_free + (int ) ptoc_Var85->integer.C_2.mresqual_cnt) , -32768 , 32767);
            }
        else
            {
            a07_b_put_error(&(*acv) , 2800 , 1);
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var79->a_return_segm , "vak660.p" , 17196)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var86 = (int ) (*dmli).d_joins.jrc_cnt + -1;
            if (0 <= ptoc_Var86)
                {
                ix = 0;
                do
                    {
                    ptoc_Var87 = &(*dmli).d_joins.jrc_joinarr[sql__setrang(ix , 0 , 256)];
                    ptoc_Var87->jo_recs[0].jop_startstack = (short ) sql__setrang((short ) ((int ) ptoc_Var87->jo_recs[0].jop_startstack + (int ) ptoc_Var85->integer.C_2.mresqual_cnt) , -32768 , 32767);
                    ptoc_Var87->jo_recs[1].jop_startstack = (short ) sql__setrang((short ) ((int ) ptoc_Var87->jo_recs[1].jop_startstack + (int ) ptoc_Var85->integer.C_2.mresqual_cnt) , -32768 , 32767);
                    if (ix == ptoc_Var86)
                        {
                        break;
                        }
                    ix += 1;
                    }
                while (!0);
                }
            t01messblock(5 , "MESSBLOCKoj2" , &(*acv).a_mblock);
            ptoc_Var80->d_single = (char ) sql__setrang(ptoc_Var83->hd_single , 0 , 1);
            ptoc_Var80->d_rowno = (short ) sql__setrang((short ) ptoc_Var83->hd_rowno , -32768 , 32767);
            ptoc_Var80->d_change.cr_colcount = (short ) sql__setrang((short ) ptoc_Var83->hd_colcount , -32768 , 32767);
            ptoc_Var80->d_distinct = (unsigned char ) sql__setrang(ptoc_Var83->hd_distinct , 0 , 2);
            ptoc_Var80->d_union = (char ) sql__setrang(ptoc_Var83->hdm_union , 0 , 1);
            (*sr_rec).sr_use_rowno = (char ) sql__setrang(ptoc_Var83->h_use_rowno , 0 , 1);
            ptoc_Var82->sel_res_tree = ptoc_Var83->last_res_tree;
            a67_sel2_second_part(&(*acv) , &(*dmli) , &ptoc_Var82->sel_res_tree , 1 , (char ) sql__setrang((char ) sql__setrang(ptoc_Var82->sel_last_pars_part , 0 , 1) && !(char ) sql__setrang(ptoc_Var80->d_subquery , 0 , 1) , 0 , 1) , (char ) sql__setrang(ptoc_Var82->sel_view_done , 0 , 1) , &(*sr_rec));
            ptoc_Var80->d_inoutpos = (short ) sql__setrang(ptoc_Var80->d_reclen , -32768 , 32767);
            t01int4(5 , "d_inoutpos d" , (int ) ptoc_Var80->d_inoutpos);
            t01int4(5 , "d_reclen    " , (int ) ptoc_Var80->d_reclen);
            }
        }
    if ((char ) sql__setrang(ptoc_Var83->messbuf_stored , 0 , 1))
        {
        m_return = (int ) (*(tsp1_segment *) sql__nil(ptoc_Var79->a_return_segm , "vak660.p" , 17236)).integer.C_4.sp1r_returncode;
        (*(tsp1_segment *) sql__nil(ptoc_Var79->a_return_segm , "vak660.p" , 17237)).integer.C_4.sp1r_returncode = 0;
        a10del_sysinfo(&(*acv) , &ptoc_Var83->ke , &b_err);
        if (m_return != 0)
            {
            (*(tsp1_segment *) sql__nil(ptoc_Var79->a_return_segm , "vak660.p" , 17241)).integer.C_4.sp1r_returncode = (short ) sql__setrang((short ) m_return , -32768 , 32767);
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var79->a_return_segm , "vak660.p" , 17245)).integer.C_4.sp1r_returncode != 0 && !(char ) sql__setrang(ptoc_Var80->d_only_sem_check , 0 , 1) && (int ) ptoc_Var79->a_command_kind == 5)
        {
        m_return = (int ) (*(tsp1_segment *) sql__nil(ptoc_Var79->a_return_segm , "vak660.p" , 17250)).integer.C_4.sp1r_returncode;
        (*(tsp1_segment *) sql__nil(ptoc_Var79->a_return_segm , "vak660.p" , 17251)).integer.C_4.sp1r_returncode = 0;
        a10del_sysinfo(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var80->d_sparr.pparsp , "vak660.p" , 17252)).integer.C_50.syskey , &b_err);
        if (m_return != 0)
            {
            (*(tsp1_segment *) sql__nil(ptoc_Var79->a_return_segm , "vak660.p" , 17255)).integer.C_4.sp1r_returncode = (short ) sql__setrang((short ) m_return , -32768 , 32767);
            }
        }
    cccprint("<ak660one_phase ");
    }



int ak660more_phase(acv, dmli, select_rec, sr_rec, sfe_rec, prep_two_phase)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_select_record *select_rec;
tak70_strat_rec *sr_rec;
store_for_endpart *sfe_rec;
char prep_two_phase;
    {
    tgg_basis_error b_err;
    int curr_n;
    int next_n;
    tak_all_command_glob *ptoc_Var88;
    tak_dml_info *ptoc_Var89;
    tak_joinrec *ptoc_Var90;
    tak_select_record *ptoc_Var91;
    store_for_endpart *ptoc_Var92;
    struct ptoc_Type57 *ptoc_Var93;
    cccprint("AK660 ak660more_");
    ptoc_Var88 = &*acv;
    ptoc_Var89 = &*dmli;
    ptoc_Var90 = &ptoc_Var89->d_joins;
    ptoc_Var91 = &*select_rec;
    ptoc_Var92 = &*sfe_rec;
    t01messblock(5 , "MESSBLOCK   " , &(*acv).a_mblock);
    ptoc_Var92->old_subcntlevel = -1;
    ptoc_Var92->m2type = 0;
    ptoc_Var92->last_res_tree = ptoc_Var91->sel_res_tree;
    ptoc_Var92->h_use_rowno = (char ) sql__setrang((*sr_rec).sr_use_rowno , 0 , 1);
    (*sr_rec).sr_use_rowno = 0;
    ptoc_Var92->messbuf_stored = 0;
    ptoc_Var92->lowpars = (unsigned char)'\377';
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var88->a_return_segm , "vak660.p" , 17289)).integer.C_4.sp1r_returncode == 0)
        {
        curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var88->a_ap_tree , "vak660.p" , 17292))[sql__setrang((int ) ptoc_Var91->sel_from_n , 0 , 32767)].n_sa_level;
        ptoc_Var93 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var88->a_ap_tree , "vak660.p" , 17293))[sql__setrang(curr_n , 0 , 32767)];
        if ((int ) ptoc_Var93->n_proc != 54 || (int ) ptoc_Var93->n_subproc != 10)
            {
            curr_n = 0;
            }
        ptoc_Var89->d_join = 1;
        if (!(char ) sql__setrang(ptoc_Var91->sel_where_done , 0 , 1) && !(char ) sql__setrang(ptoc_Var91->sel_view_done , 0 , 1))
            {
            ptoc_Var89->d_joins.jrc_cnt = 0;
            }
        ak660move_in_hbuf(&(*acv) , &(*sfe_rec) , 2 , 0);
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var88->a_return_segm , "vak660.p" , 17305)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var92->messbuf_stored = 1;
            ptoc_Var92->hmbp = (struct tgg00_MessBlock *) &(*(struct tak_systembuffer *) sql__nil(ptoc_Var92->hbuf , "vak660.p" , 17309)).integer.C_62.smessblock.mbr_mess_block;
            ptoc_Var92->hqbp = (*(struct tgg00_MessBlock *) sql__nil(ptoc_Var92->hmbp , "vak660.p" , 17310)).integer.C_4.mb_qual;
            (*(struct tgg00_MessBlock *) sql__nil(ptoc_Var92->hmbp , "vak660.p" , 17311)).integer.C_2.mb_type = 60;
            if ((int ) (*(struct tgg00_MessBlock *) sql__nil(ptoc_Var92->hmbp , "vak660.p" , 17312)).integer.C_2.mb_type2 == 68)
                {
                (*(struct tgg00_MessBlock *) sql__nil(ptoc_Var92->hmbp , "vak660.p" , 17314)).integer.C_2.mb_type2 = 0;
                }
            ptoc_Var92->m2type = (unsigned char ) sql__setrang((*(struct tgg00_MessBlock *) sql__nil(ptoc_Var92->hmbp , "vak660.p" , 17316)).integer.C_2.mb_type2 , 0 , 95);
            ptoc_Var92->lowpars = (unsigned char ) sql__setrang(ptoc_Var89->d_lowpars , 0 , 255);
            if ((int ) ptoc_Var89->d_corr != 0)
                {
                ptoc_Var92->lowpars = (unsigned char ) sql__setrang((unsigned char ) ((int ) ptoc_Var89->d_lowpars + -1) , 0 , 255);
                ptoc_Var89->d_lowpars = (unsigned char ) sql__setrang((unsigned char ) ((int ) ptoc_Var89->d_lowpars + -1) , 0 , 255);
                }
            t01int4(5 , "two  lowpars" , (int ) ptoc_Var92->lowpars);
            ptoc_Var92->hinoutpos = (int ) ptoc_Var89->d_inoutpos;
            ptoc_Var92->hkeylen = (int ) ptoc_Var89->d_keylen;
            ptoc_Var92->hreclen = (int ) ptoc_Var89->d_reclen;
            ptoc_Var92->hd_distinct = (unsigned char ) sql__setrang(ptoc_Var89->d_distinct , 0 , 2);
            ptoc_Var89->d_distinct = 0;
            ptoc_Var92->hdm_union = (char ) sql__setrang(ptoc_Var89->d_union , 0 , 1);
            ptoc_Var89->d_union = 0;
            ptoc_Var92->hd_single = (char ) sql__setrang(ptoc_Var89->d_single , 0 , 1);
            ptoc_Var89->d_single = 0;
            ptoc_Var92->hd_rowno = (int ) ptoc_Var89->d_rowno;
            ptoc_Var89->d_rowno = -1;
            ptoc_Var92->hd_colcount = (int ) ptoc_Var89->d_change.cr_colcount;
            ptoc_Var89->d_change.cr_colcount = 0;
            if ((char ) sql__setrang(prep_two_phase , 0 , 1))
                {
                ptoc_Var89->d_keylen = (short ) sql__setrang((short ) sizeof(int ) , -32768 , 32767);
                }
            ptoc_Var89->d_inoutpos = (short ) sql__setrang((short ) (sizeof(int ) + 9) , -32768 , 32767);
            if (!(char ) sql__setrang(prep_two_phase , 0 , 1))
                {
                if ((int ) ptoc_Var92->hd_distinct != 0 || (char ) sql__setrang(ptoc_Var92->hdm_union , 0 , 1) || (char ) sql__setrang(ptoc_Var89->d_subquery , 0 , 1))
                    {
                    ptoc_Var89->d_inoutpos = (short ) sql__setrang((short ) ((int ) ptoc_Var89->d_inoutpos + 4) , -32768 , 32767);
                    }
                }
            ak660two_repl_complex_output(&(*acv) , &(*dmli) , &ptoc_Var92->hbuf , (char ) sql__setrang(ptoc_Var91->sel_where_done , 0 , 1) , (char ) sql__setrang(ptoc_Var91->sel_view_done , 0 , 1) , 0);
            t01int4(5 , "d_inoutpos c" , (int ) ptoc_Var89->d_inoutpos);
            t01int4(5 , "d_reclen    " , (int ) ptoc_Var89->d_reclen);
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var88->a_return_segm , "vak660.p" , 17360)).integer.C_4.sp1r_returncode == 0)
                {
                a10add_sysinfo(&(*acv) , &ptoc_Var92->hbuf , &b_err);
                if ((int ) b_err != 0)
                    {
                    a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                    }
                else
                    {
                    a10_rel_sysinfo(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var92->hbuf , "vak660.p" , 17368)).integer.C_50.syskey);
                    }
                }
            }
        }
    ptoc_Var89->d_group = 0;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var88->a_return_segm , "vak660.p" , 17377)).integer.C_4.sp1r_returncode == 0 && curr_n != 0 && !(char ) sql__setrang(ptoc_Var91->sel_where_done , 0 , 1))
        {
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var88->a_ap_tree , "vak660.p" , 17380))[sql__setrang(curr_n , 0 , 32767)].n_proc == 54 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var88->a_ap_tree , "vak660.p" , 17381))[sql__setrang(curr_n , 0 , 32767)].n_subproc == 10)
            {
            next_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var88->a_ap_tree , "vak660.p" , 17384))[sql__setrang(curr_n , 0 , 32767)].n_lo_level;
            if ((int ) ptoc_Var88->a_ex_kind == 0)
                {
                a54_fixedpos(&(*acv) , &(*dmli));
                }
            a65_search_condition(&(*acv) , &(*dmli) , &next_n);
            }
        }
    ptoc_Var92->first_res = (char ) sql__setrang((int ) (*(tsp1_segment *) sql__nil(ptoc_Var88->a_return_segm , "vak660.p" , 17393)).integer.C_4.sp1r_returncode == 0 , 0 , 1);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var88->a_return_segm , "vak660.p" , 17394)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) ptoc_Var88->a_ex_kind == 0 && !(char ) sql__setrang(ptoc_Var89->d_only_sem_check , 0 , 1) && ptoc_Var89->d_sparr.pparsp != (struct tak_systembuffer *) (int *) 0)
            {
            ptoc_Var92->const_par_expr = (char ) sql__setrang((*(struct tak_systembuffer *) sql__nil(ptoc_Var89->d_sparr.pparsp , "vak660.p" , 17402)).integer.C_23.sparsinfo.integer.C_2.p_const_param_expr , 0 , 1);
            ptoc_Var92->param_parskey = (*(struct tak_systembuffer *) sql__nil(ptoc_Var89->d_sparr.pparsp , "vak660.p" , 17404)).integer.C_50.syskey;
            }
        else
            {
            ptoc_Var92->const_par_expr = 0;
            }
        g04build_temp_tree_id(&ptoc_Var91->sel_res_tree , &ptoc_Var88->a_transinf.tri_trans);
        ptoc_Var91->sel_res_tree.integer.C_8.fileTfnTemp_gg00 = 1;
        ptoc_Var91->sel_res_tree.integer.C_8.fileLevel_gg00[0] = (unsigned char ) sql__setrang((unsigned char ) ptoc_Var88->a_pars_last_key.p_no , 0 , 255);
        if ((int ) ptoc_Var92->last_res_tree.integer.C_4.fileTfn_gg00 == 26 && (int ) ptoc_Var92->last_res_tree.integer.C_8.fileTfnTemp_gg00 == 1)
            {
            ptoc_Var92->last_res_tree.integer.C_8.fileLevel_gg00[1] = (unsigned char)'\001';
            }
        else
            {
            ptoc_Var91->sel_res_tree.integer.C_8.fileLevel_gg00[1] = (unsigned char)'\0';
            }
        }
    t01messblock(5 , "end more_p <" , sql__nil(ptoc_Var92->hmbp , "vak660.p" , 17421));
    t01messblock(5 , "MESSBLOCK   " , &(*acv).a_mblock);
    cccprint("<ak660more_phase");
    }



int ak660two_phase(acv, dmli, select_rec, sr_rec, sfe_rec, store_infos)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_select_record *select_rec;
tak70_strat_rec *sr_rec;
store_for_endpart *sfe_rec;
char store_infos;
    {
    tgg_basis_error b_err;
    char res_file_exists;
    int m_return;
    int m_data_len;
    tak_all_command_glob *ptoc_Var94;
    tak_dml_info *ptoc_Var95;
    tak_joinrec *ptoc_Var96;
    tak_select_record *ptoc_Var97;
    store_for_endpart *ptoc_Var98;
    tak_parsinforecord *ptoc_Var99;
    cccprint("AK660 ak660two_p");
    ptoc_Var94 = &*acv;
    ptoc_Var95 = &*dmli;
    ptoc_Var96 = &ptoc_Var95->d_joins;
    ptoc_Var97 = &*select_rec;
    ptoc_Var98 = &*sfe_rec;
    res_file_exists = 0;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var94->a_return_segm , "vak660.p" , 17448)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) ptoc_Var94->a_ex_kind == 0 && ptoc_Var94->a_union_cnt == 0 && !(char ) sql__setrang(ptoc_Var95->d_only_sem_check , 0 , 1))
            {
            ptoc_Var99 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var95->d_sparr.pparsp , "vak660.p" , 17456)).integer.C_23.sparsinfo;
            ptoc_Var98->old_subcntlevel = (short ) sql__setrang(ptoc_Var99->integer.C_2.p_subcntlevel , -32768 , 32767);
            ptoc_Var99->integer.C_2.p_subcntlevel = -1;
            }
        m_data_len = ptoc_Var94->a_mblock.integer.C_2.mb_data_len;
        a67_sel2_second_part(&(*acv) , &(*dmli) , &ptoc_Var97->sel_res_tree , 1 , 0 , (char ) sql__setrang(ptoc_Var97->sel_view_done , 0 , 1) , &(*sr_rec));
        t01int4(5 , "d_inoutpos d" , (int ) ptoc_Var95->d_inoutpos);
        t01int4(5 , "d_reclen    " , (int ) ptoc_Var95->d_reclen);
        if ((int ) ptoc_Var94->a_ex_kind != 0 && !(char ) sql__setrang(ptoc_Var95->d_only_sem_check , 0 , 1))
            {
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var94->a_return_segm , "vak660.p" , 17472)).integer.C_4.sp1r_returncode == 0)
                {
                ptoc_Var97->sel_res_tree = (*(tgg00_QualBuf *) sql__nil(ptoc_Var94->a_mblock.integer.C_4.mb_qual , "vak660.p" , 17475)).integer.C_4.mr_restree;
                res_file_exists = (char ) sql__setrang((_ptoc_MEMCMP(unsigned char , 7, (*(tgg00_QualBuf *) sql__nil(ptoc_Var94->a_mblock.integer.C_4.mb_qual , "vak660.p" , 17477)).integer.C_4.mr_resnum , "\0\200\0\0\0\0\0") == 0 ? 1 : 0) && (int ) ptoc_Var98->m2type != 67 , 0 , 1);
                }
            else
                {
                a502destroy_file(&(*acv) , &ptoc_Var97->sel_res_tree);
                }
            }
        ptoc_Var94->a_mblock.integer.C_2.mb_data_len = m_data_len;
        ptoc_Var95->d_single = (char ) sql__setrang(ptoc_Var98->hd_single , 0 , 1);
        ptoc_Var95->d_rowno = (short ) sql__setrang((short ) ptoc_Var98->hd_rowno , -32768 , 32767);
        ptoc_Var95->d_change.cr_colcount = (short ) sql__setrang((short ) ptoc_Var98->hd_colcount , -32768 , 32767);
        ptoc_Var95->d_distinct = (unsigned char ) sql__setrang(ptoc_Var98->hd_distinct , 0 , 2);
        ptoc_Var95->d_union = (char ) sql__setrang(ptoc_Var98->hdm_union , 0 , 1);
        (*sr_rec).sr_use_rowno = (char ) sql__setrang(ptoc_Var98->h_use_rowno , 0 , 1);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var94->a_return_segm , "vak660.p" , 17495)).integer.C_4.sp1r_returncode == 0 && !(char ) sql__setrang(ptoc_Var95->d_only_sem_check , 0 , 1))
        {
        if (ptoc_Var94->a_mblock.integer.C_2.mb_data_len == 0)
            {
            ptoc_Var94->a_mblock.integer.C_2.mb_data_len = 8;
            }
        ak660endpart(&(*acv) , &(*dmli) , &(*sfe_rec) , (int ) ptoc_Var97->sel_order_n , (char ) sql__setrang(ptoc_Var97->sel_last_pars_part , 0 , 1) , (char ) sql__setrang(store_infos , 0 , 1) , (char ) sql__setrang((int ) ptoc_Var95->d_corr != 0 , 0 , 1) , &ptoc_Var97->sel_res_tree , &(*select_rec) , &(*sr_rec));
        t01int4(5 , "d_inoutpos e" , (int ) ptoc_Var95->d_inoutpos);
        t01int4(5 , "d_reclen    " , (int ) ptoc_Var95->d_reclen);
        if ((char ) sql__setrang(ptoc_Var98->first_res , 0 , 1) && (char ) sql__setrang(res_file_exists , 0 , 1) && (int ) ptoc_Var94->a_ex_kind != 0)
            {
            a502destroy_file(&(*acv) , &ptoc_Var97->sel_res_tree);
            if ((int ) ptoc_Var94->a_transinf.tri_trans.trError_gg00 != 0)
                {
                a07_b_put_error(&(*acv) , (short ) sql__setrang(ptoc_Var94->a_transinf.tri_trans.trError_gg00 , -32768 , 32767) , 1);
                }
            }
        ptoc_Var97->sel_res_tree = ptoc_Var98->last_res_tree;
        }
    if ((char ) sql__setrang(ptoc_Var98->messbuf_stored , 0 , 1))
        {
        m_return = (int ) (*(tsp1_segment *) sql__nil(ptoc_Var94->a_return_segm , "vak660.p" , 17527)).integer.C_4.sp1r_returncode;
        (*(tsp1_segment *) sql__nil(ptoc_Var94->a_return_segm , "vak660.p" , 17528)).integer.C_4.sp1r_returncode = 0;
        a10del_sysinfo(&(*acv) , &ptoc_Var98->ke , &b_err);
        if (m_return != 0)
            {
            (*(tsp1_segment *) sql__nil(ptoc_Var94->a_return_segm , "vak660.p" , 17532)).integer.C_4.sp1r_returncode = (short ) sql__setrang((short ) m_return , -32768 , 32767);
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var94->a_return_segm , "vak660.p" , 17536)).integer.C_4.sp1r_returncode != 0 && !(char ) sql__setrang(ptoc_Var95->d_only_sem_check , 0 , 1) && (int ) ptoc_Var94->a_command_kind == 5)
        {
        m_return = (int ) (*(tsp1_segment *) sql__nil(ptoc_Var94->a_return_segm , "vak660.p" , 17541)).integer.C_4.sp1r_returncode;
        (*(tsp1_segment *) sql__nil(ptoc_Var94->a_return_segm , "vak660.p" , 17542)).integer.C_4.sp1r_returncode = 0;
        a10del_sysinfo(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var95->d_sparr.pparsp , "vak660.p" , 17543)).integer.C_50.syskey , &b_err);
        if (m_return != 0)
            {
            (*(tsp1_segment *) sql__nil(ptoc_Var94->a_return_segm , "vak660.p" , 17546)).integer.C_4.sp1r_returncode = (short ) sql__setrang((short ) m_return , -32768 , 32767);
            }
        }
    cccprint("<ak660two_phase ");
    }



int ak660two_repl_complex_output(acv, dmli, hbuf, where_done, view_done, functions_not_single_fields)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_sysbufferaddress *hbuf;
char where_done;
char view_done;
char functions_not_single_fields;
    {
    int end_pos;
    int start_pos;
    tgg_mess_block_ptr hmbp;
    tgg_qual_buf_ptr hqbp;
    struct ptoc_Type188
      {
      union  
        {
        struct  
          {
          where_arr_ptr w_arr;
          } C_true;
        struct  
          {
          tak_sysbufferaddress sbuf;
          } C_false;
        } boolean;
      } where_arr;
    struct tgg00_MessBlock *ptoc_Var100;
    tgg00_QualBuf *ptoc_Var101;
    struct ptoc_Type39 *ptoc_Var102;
    struct ptoc_Type39 *ptoc_Var103;
    struct tgg00_MessBlock *ptoc_Var104;
    tgg00_QualBuf *ptoc_Var105;
    cccprint("AK660 ak660two_r");
    ptoc_Var100 = &(*acv).a_mblock;
    ptoc_Var101 = (tgg00_QualBuf *) sql__nil(ptoc_Var100->integer.C_4.mb_qual , "vak660.p" , 17580);
    hmbp = (struct tgg00_MessBlock *) &(*(struct tak_systembuffer *) sql__nil(*hbuf , "vak660.p" , 17582)).integer.C_62.smessblock.mbr_mess_block;
    hqbp = (*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17583)).integer.C_4.mb_qual;
    t01name(5 , "Anfang Two_repl   ");
    t01stqual(5 , sql__nil(hqbp , "vak660.p" , 17585) , (*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17585)).integer.C_5.mb_st);
    t01stqual(5 , sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak660.p" , 17586) , (*acv).a_mblock.integer.C_5.mb_st);
    start_pos = (int ) (*(tgg00_QualBuf *) sql__nil(hqbp , "vak660.p" , 17587)).integer.C_2.mqual_pos + 1;
    end_pos = (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17588)).integer.C_5.mb_st , "vak660.p" , 17588))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(hqbp , "vak660.p" , 17588)).integer.C_2.mqual_pos , 1 , 32767) + -1].integer.C_1.epos + (int ) (*(tgg00_QualBuf *) sql__nil(hqbp , "vak660.p" , 17589)).integer.C_2.mqual_pos + -2;
    ak660qual_part_transfer(&(*acv) , &(*dmli) , &(*hbuf) , (char ) sql__setrang(functions_not_single_fields , 0 , 1) , start_pos , end_pos);
    start_pos = (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17592)).integer.C_5.mb_st , "vak660.p" , 17592))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(hqbp , "vak660.p" , 17592)).integer.C_2.mqual_pos , 1 , 32767) + -1].integer.C_1.epos + (int ) (*(tgg00_QualBuf *) sql__nil(hqbp , "vak660.p" , 17593)).integer.C_2.mqual_pos + -1;
    end_pos = (int ) (*(tgg00_QualBuf *) sql__nil(hqbp , "vak660.p" , 17594)).integer.C_2.mqual_pos + (int ) (*(tgg00_QualBuf *) sql__nil(hqbp , "vak660.p" , 17594)).integer.C_2.mqual_cnt + -1;
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 17595)).integer.C_4.sp1r_returncode == 0 && (char ) sql__setrang(functions_not_single_fields , 0 , 1) && start_pos <= end_pos)
        {
        t01name(5 , "middle1 of two_rep");
        t01stqual(5 , sql__nil(hqbp , "vak660.p" , 17601) , (*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17601)).integer.C_5.mb_st);
        t01stqual(5 , sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak660.p" , 17602) , (*acv).a_mblock.integer.C_5.mb_st);
        ak660qual_part_transfer(&(*acv) , &(*dmli) , &(*hbuf) , (char ) sql__setrang(functions_not_single_fields , 0 , 1) , start_pos , end_pos);
        }
    a61_set_jump(&(*acv).a_mblock , 1 , 33);
    ptoc_Var102 = &(*(tgg00_StackList *) sql__nil((*acv).a_mblock.integer.C_5.mb_st , "vak660.p" , 17608))[0];
    ptoc_Var102->integer.C_1.epos = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var102->integer.C_1.epos , -32768 , 32767 , "vak660.p" , 17609) , -32768 , 32767);
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 17611)).integer.C_4.sp1r_returncode == 0 && ((char ) sql__setrang(where_done , 0 , 1) || (char ) sql__setrang(view_done , 0 , 1)) && start_pos <= end_pos)
        {
        t01name(5 , "middle2 of two_rep");
        t01stqual(5 , sql__nil(hqbp , "vak660.p" , 17617) , (*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17617)).integer.C_5.mb_st);
        t01stqual(5 , sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak660.p" , 17618) , (*acv).a_mblock.integer.C_5.mb_st);
        a10new(&(*acv) , ((end_pos - start_pos) / 4 + 2) * sizeof(struct ptoc_Type185 ) , &where_arr.boolean.C_false.sbuf);
        if (where_arr.boolean.C_false.sbuf == (struct tak_systembuffer *) (int *) 0)
            {
            a07_b_put_error(&(*acv) , 1405 , 1);
            }
        else
            {
            (*(where_array *) sql__nil(where_arr.boolean.C_true.w_arr , "vak660.p" , 17626))[0].start = 0;
            ak660where_transfer(&(*acv) , &(*dmli) , &(*hbuf) , 1 , start_pos , end_pos , &where_arr.boolean.C_true.w_arr);
            a61_set_jump(&(*acv).a_mblock , 1 , 33);
            ptoc_Var103 = &(*(tgg00_StackList *) sql__nil((*acv).a_mblock.integer.C_5.mb_st , "vak660.p" , 17630))[0];
            ptoc_Var103->integer.C_1.epos = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var103->integer.C_1.epos , -32768 , 32767 , "vak660.p" , 17631) , -32768 , 32767);
            t01name(5 , "middle3 of two_rep");
            t01stqual(5 , sql__nil(hqbp , "vak660.p" , 17634) , (*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17634)).integer.C_5.mb_st);
            t01stqual(5 , sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak660.p" , 17635) , (*acv).a_mblock.integer.C_5.mb_st);
            if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 17636)).integer.C_4.sp1r_returncode == 0)
                {
                ak660where_transfer(&(*acv) , &(*dmli) , &(*hbuf) , 0 , start_pos , end_pos , &where_arr.boolean.C_true.w_arr);
                }
            }
        a10dispose(&(*acv) , &where_arr.boolean.C_false.sbuf);
        }
    ptoc_Var104 = &(*acv).a_mblock;
    ptoc_Var105 = (tgg00_QualBuf *) sql__nil(ptoc_Var104->integer.C_4.mb_qual , "vak660.p" , 17646);
    if (!(char ) sql__setrang(functions_not_single_fields , 0 , 1) && (int ) ptoc_Var104->integer.C_2.mb_type2 == 67)
        {
        ptoc_Var104->integer.C_2.mb_type2 = 0;
        }
    if ((char ) sql__setrang((*dmli).d_outer_join , 0 , 1) && (int ) (*dmli).d_joins.jrc_cnt == 0)
        {
        (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17657)).integer.C_5.mb_st , "vak660.p" , 17657))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(hqbp , "vak660.p" , 17657)).integer.C_2.mqual_pos , 1 , 32767) + -1].integer.C_1.eop = 23;
        }
    t01name(5 , "end of two_repl   ");
    t01stqual(5 , sql__nil(hqbp , "vak660.p" , 17660) , (*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17660)).integer.C_5.mb_st);
    t01stqual(5 , sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak660.p" , 17661) , (*acv).a_mblock.integer.C_5.mb_st);
    cccprint("<ak660two_repl_c");
    }

typedef
  short and_array_ak660where_transfer[4096];
typedef
  and_array_ak660where_transfer *and_arr_ptr_ak660where_transfer;


int ak660where_transfer(acv, dmli, hbuf, output, sourcestartpos, sourcestoppos, w_arr)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_sysbufferaddress *hbuf;
char output;
int sourcestartpos;
int sourcestoppos;
where_arr_ptr *w_arr;
    {
    char col_output;
    char corr_sub;
    char into_w_arr;
    char put_op_and;
    char top_level;
    char is_n_null;
    int and_pos;
    int col_tab;
    int curr_i;
    int deststartpos;
    int endpos;
    int act_join;
    int joinpos;
    int nextpos;
    int qual_end;
    int stackcnt;
    int stackpos;
    int startpos;
    int w_pos;
    tgg_mess_block_ptr hmbp;
    tgg_qual_buf_ptr hqbp;
    struct ptoc_Type189
      {
      union  
        {
        struct  
          {
          and_arr_ptr_ak660where_transfer and_ptr;
          } C_true;
        struct  
          {
          tak_sysbufferaddress sbuf;
          } C_false;
        } boolean;
      } and_arr;
    tak_all_command_glob *ptoc_Var106;
    struct tgg00_MessBlock *ptoc_Var107;
    tgg00_QualBuf *ptoc_Var108;
    int ptoc_Var109;
    tak_joinrec *ptoc_Var110;
    struct ptoc_Type112 *ptoc_Var111;
    tak_joinrec *ptoc_Var112;
    int ptoc_Var113;
    tak_joinrec *ptoc_Var114;
    struct ptoc_Type111 *ptoc_Var115;
    struct ptoc_Type39 *ptoc_Var116;
    struct ptoc_Type39 *ptoc_Var117;
    struct ptoc_Type185 *ptoc_Var118;
    cccprint("AK660 ak660where");
    ptoc_Var106 = &*acv;
    ptoc_Var107 = &ptoc_Var106->a_mblock;
    ptoc_Var108 = (tgg00_QualBuf *) sql__nil(ptoc_Var107->integer.C_4.mb_qual , "vak660.p" , 17713);
    a683_output(5 , &(*dmli));
    hmbp = (struct tgg00_MessBlock *) &(*(struct tak_systembuffer *) sql__nil(*hbuf , "vak660.p" , 17716)).integer.C_62.smessblock.mbr_mess_block;
    hqbp = (*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17717)).integer.C_4.mb_qual;
    deststartpos = (int ) ptoc_Var108->integer.C_2.mfirst_free;
    nextpos = sourcestartpos;
    qual_end = sourcestoppos;
    put_op_and = 0;
    w_pos = 1;
    joinpos = 0;
    and_pos = 0;
    a10new(&(*acv) , ((sourcestoppos - sourcestartpos) / 4 + 2) * sizeof(short ) , &and_arr.boolean.C_false.sbuf);
    if (and_arr.boolean.C_false.sbuf == (struct tak_systembuffer *) (int *) 0)
        {
        a07_b_put_error(&(*acv) , 1405 , 1);
        }
    else
        {
        while (nextpos <= qual_end && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var106->a_return_segm , "vak660.p" , 17733)).integer.C_4.sp1r_returncode == 0)
            {
            top_level = 0;
            while ((int ) (*(where_array *) sql__nil(*w_arr , "vak660.p" , 17736))[sql__setrang(w_pos , 1 , 4096) + -1].start == nextpos)
                {
                nextpos = nextpos + (int ) (*(where_array *) sql__nil(*w_arr , "vak660.p" , 17738))[sql__setrang(w_pos , 1 , 4096) + -1].length;
                w_pos = sql__succ(w_pos , -2147483647 , 2147483647 , "vak660.p" , 17739);
                }
            if (nextpos <= qual_end)
                {
                into_w_arr = 0;
                startpos = nextpos;
                while (nextpos <= qual_end && ((ptoc_Var109 = (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17748)).integer.C_5.mb_st , "vak660.p" , 17748))[sql__setrang(nextpos , 1 , 32767) + -1].integer.C_1.etype) != 0 && ptoc_Var109 != 1 && ptoc_Var109 != 2 && ptoc_Var109 != 3 && ptoc_Var109 != 13 && ptoc_Var109 != 46 && ptoc_Var109 != 56 && ptoc_Var109 != 21 && ptoc_Var109 != 32))
                    nextpos = sql__succ(nextpos , -2147483647 , 2147483647 , "vak660.p" , 17754);
                if (nextpos <= qual_end)
                    {
                    corr_sub = 0;
                    col_tab = 0;
                    if ((ptoc_Var109 = (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17761)).integer.C_5.mb_st , "vak660.p" , 17761))[sql__setrang(nextpos , 1 , 32767) + -1].integer.C_1.etype) == 0 || ptoc_Var109 == 1 || ptoc_Var109 == 2 || ptoc_Var109 == 3 || ptoc_Var109 == 13)
                        {
                        col_tab = (ptoc_Var109 = sql__ord((int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17765)).integer.C_5.mb_st , "vak660.p" , 17765))[sql__setrang(nextpos , 1 , 32767) + -1].integer.C_1.ecol_tab[1]) % 100) >= 0 ? ptoc_Var109 : ptoc_Var109 + 100;
                        }
                    col_output = (char ) sql__setrang((int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17768)).integer.C_5.mb_st , "vak660.p" , 17768))[sql__setrang(nextpos , 1 , 32767) + -1].integer.C_1.eop == 23 , 0 , 1);
                    is_n_null = 0;
                    while (nextpos <= qual_end && !(char ) sql__setrang(top_level , 0 , 1))
                        {
                        if ((ptoc_Var109 = (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17773)).integer.C_5.mb_st , "vak660.p" , 17773))[sql__setrang(nextpos , 1 , 32767) + -1].integer.C_1.etype) == 0 || ptoc_Var109 == 1 || ptoc_Var109 == 2 || ptoc_Var109 == 3 || ptoc_Var109 == 13)
                            {
                            if (col_tab == 0)
                                {
                                col_tab = (ptoc_Var109 = sql__ord((int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17780)).integer.C_5.mb_st , "vak660.p" , 17780))[sql__setrang(nextpos , 1 , 32767) + -1].integer.C_1.ecol_tab[1]) % 100) >= 0 ? ptoc_Var109 : ptoc_Var109 + 100;
                                }
                            else
                                {
                                if (((ptoc_Var109 = sql__ord((int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17782)).integer.C_5.mb_st , "vak660.p" , 17782))[sql__setrang(nextpos , 1 , 32767) + -1].integer.C_1.ecol_tab[1]) % 100) >= 0 ? ptoc_Var109 : ptoc_Var109 + 100) != col_tab)
                                    {
                                    col_tab = -1;
                                    }
                                }
                            if ((char ) sql__setrang(col_output , 0 , 1))
                                {
                                col_output = (char ) sql__setrang((char ) sql__setrang(col_output , 0 , 1) && (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17790)).integer.C_5.mb_st , "vak660.p" , 17790))[sql__setrang(nextpos , 1 , 32767) + -1].integer.C_1.eop == 23 , 0 , 1);
                                }
                            }
                        else
                            {
                            if ((int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17795)).integer.C_5.mb_st , "vak660.p" , 17795))[sql__setrang(nextpos , 1 , 32767) + -1].integer.C_1.etype == 22 && ((ptoc_Var109 = (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17796)).integer.C_5.mb_st , "vak660.p" , 17796))[sql__setrang(nextpos , 1 , 32767) + -1].integer.C_1.eop) == 104 || ptoc_Var109 == 102) && !a680is_outer_predicate(&(*dmli) , nextpos))
                                {
                                is_n_null = 1;
                                }
                            }
                        if (((int ) (*dmli).d_corr == 2 || (int ) (*dmli).d_corr == 1) && (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17804)).integer.C_5.mb_st , "vak660.p" , 17804))[sql__setrang(nextpos , 1 , 32767) + -1].integer.C_1.etype == 56)
                            {
                            corr_sub = 1;
                            }
                        if ((int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17808)).integer.C_5.mb_st , "vak660.p" , 17808))[sql__setrang(nextpos , 1 , 32767) + -1].integer.C_1.etype == 32)
                            {
                            g04check_if_top_level(&(*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17810)).integer.C_5.mb_st , nextpos , qual_end , &top_level);
                            }
                        if (!(char ) sql__setrang(top_level , 0 , 1))
                            {
                            nextpos = sql__succ(nextpos , -2147483647 , 2147483647 , "vak660.p" , 17815);
                            }
                        }
                    if (col_tab == -1 && (char ) sql__setrang((*dmli).d_join , 0 , 1))
                        {
                        ptoc_Var110 = &(*dmli).d_joins;
                        ptoc_Var109 = ptoc_Var110->jrc_cnt;
                        if (1 <= ptoc_Var109)
                            {
                            curr_i = 1;
                            do
                                {
                                ptoc_Var111 = &ptoc_Var110->jrc_joinarr[sql__setrang(curr_i + -1 , 0 , 256)].jo_recs[0];
                                if ((int ) ptoc_Var111->jop_startstack >= startpos && (int ) ptoc_Var111->jop_startstack + (int ) ptoc_Var111->jop_cntstack <= nextpos)
                                    {
                                    col_tab = 0;
                                    }
                                if (curr_i == ptoc_Var109)
                                    {
                                    break;
                                    }
                                curr_i += 1;
                                }
                            while (!0);
                            }
                        }
                    if (col_tab == -1 || (char ) sql__setrang(is_n_null , 0 , 1) && (char ) sql__setrang((*dmli).d_outer_join , 0 , 1))
                        {
                        col_output = 1;
                        }
                    if ((char ) sql__setrang(corr_sub , 0 , 1))
                        {
                        col_output = 0;
                        }
                    if ((char ) sql__setrang(col_output , 0 , 1))
                        {
                        if ((char ) sql__setrang(output , 0 , 1))
                            {
                            if (nextpos > qual_end)
                                {
                                ak660qual_part_transfer(&(*acv) , &(*dmli) , &(*hbuf) , 0 , startpos , qual_end);
                                }
                            else
                                {
                                ak660qual_part_transfer(&(*acv) , &(*dmli) , &(*hbuf) , 0 , startpos , nextpos);
                                }
                            into_w_arr = 1;
                            t01int4(5 , "startpos    " , startpos);
                            ptoc_Var112 = &(*dmli).d_joins;
                            while ((int ) ptoc_Var112->jrc_joinarr[sql__setrang(joinpos , 0 , 256)].jo_recs[0].jop_startstack < startpos && joinpos < (int ) ptoc_Var112->jrc_cnt)
                                joinpos = sql__succ(joinpos , -2147483647 , 2147483647 , "vak660.p" , 17866);
                            if (joinpos < (int ) ptoc_Var112->jrc_cnt)
                                {
                                if ((int ) ptoc_Var112->jrc_joinarr[sql__setrang(joinpos , 0 , 256)].jo_recs[0].jop_startstack == startpos)
                                    {
                                    ptoc_Var109 = joinpos;
                                    ptoc_Var113 = (int ) ptoc_Var112->jrc_cnt + -2;
                                    if (ptoc_Var109 <= ptoc_Var113)
                                        {
                                        act_join = ptoc_Var109;
                                        do
                                            {
                                            ptoc_Var112->jrc_joinarr[sql__setrang(act_join , 0 , 256)] = ptoc_Var112->jrc_joinarr[sql__setrang(act_join + 1 , 0 , 256)];
                                            if (act_join == ptoc_Var113)
                                                {
                                                break;
                                                }
                                            act_join += 1;
                                            }
                                        while (!0);
                                        }
                                    ptoc_Var112->jrc_cnt = (short ) sql__setrang((short ) sql__pred((int ) ptoc_Var112->jrc_cnt , -32768 , 32767 , "vak660.p" , 17876) , -32768 , 32767);
                                    }
                                }
                            }
                        }
                    else
                        {
                        if (!(char ) sql__setrang(output , 0 , 1))
                            {
                            if ((char ) sql__setrang(put_op_and , 0 , 1))
                                {
                                if ((int ) ptoc_Var108->integer.C_2.mfirst_free <= (int ) ptoc_Var107->integer.C_2.mb_st_max)
                                    {
                                    and_pos = sql__succ(and_pos , -2147483647 , 2147483647 , "vak660.p" , 17901);
                                    (*(and_array_ak660where_transfer *) sql__nil(and_arr.boolean.C_true.and_ptr , "vak660.p" , 17902))[sql__setrang(and_pos , 1 , 4096) + -1] = (short ) sql__setrang(ptoc_Var108->integer.C_2.mfirst_free , -32768 , 32767);
                                    ptoc_Var108->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var108->integer.C_2.mqual_cnt , -32768 , 32767 , "vak660.p" , 17903) , -32768 , 32767);
                                    ptoc_Var108->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var108->integer.C_2.mfirst_free , -32768 , 32767 , "vak660.p" , 17904) , -32768 , 32767);
                                    }
                                else
                                    {
                                    a07_b_put_error(&(*acv) , 2800 , 1);
                                    }
                                }
                            endpos = sql__pred(nextpos , -2147483647 , 2147483647 , "vak660.p" , 17911);
                            while ((int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17912)).integer.C_5.mb_st , "vak660.p" , 17912))[sql__setrang(endpos , 1 , 32767) + -1].integer.C_1.etype == 26 && ((ptoc_Var113 = (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17914)).integer.C_5.mb_st , "vak660.p" , 17914))[sql__setrang(endpos , 1 , 32767) + -1].integer.C_1.eop) == 98 || ptoc_Var113 == 97))
                                endpos = sql__pred(endpos , -2147483647 , 2147483647 , "vak660.p" , 17916);
                            if ((ptoc_Var113 = (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17918)).integer.C_5.mb_st , "vak660.p" , 17918))[sql__setrang(endpos , 1 , 32767) + -1].integer.C_1.eop) == 98 || ptoc_Var113 == 97)
                                {
                                (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17921)).integer.C_5.mb_st , "vak660.p" , 17921))[sql__setrang(endpos , 1 , 32767) + -1].integer.C_1.eop = 0;
                                }
                            if ((int ) ptoc_Var108->integer.C_2.mfirst_free + endpos - startpos > (int ) ptoc_Var107->integer.C_2.mb_st_max)
                                {
                                a07_b_put_error(&(*acv) , 2800 , 1);
                                nextpos = qual_end + 1;
                                }
                            else
                                {
                                stackcnt = endpos - startpos + 1;
                                if ((int ) (*(tgg00_QualBuf *) sql__nil(hqbp , "vak660.p" , 17933)).integer.C_2.mview_pos > 0 && (int ) (*(tgg00_QualBuf *) sql__nil(hqbp , "vak660.p" , 17934)).integer.C_2.mview_cnt > 0 && startpos >= (int ) (*(tgg00_QualBuf *) sql__nil(hqbp , "vak660.p" , 17935)).integer.C_2.mview_pos && startpos <= (int ) (*(tgg00_QualBuf *) sql__nil(hqbp , "vak660.p" , 17936)).integer.C_2.mview_pos + (int ) (*(tgg00_QualBuf *) sql__nil(hqbp , "vak660.p" , 17937)).integer.C_2.mview_cnt + -1)
                                    {
                                    if ((int ) ptoc_Var108->integer.C_2.mview_pos == 0)
                                        {
                                        ptoc_Var108->integer.C_2.mview_pos = (short ) sql__setrang(ptoc_Var108->integer.C_2.mfirst_free , -32768 , 32767);
                                        }
                                    ptoc_Var108->integer.C_2.mview_cnt = (short ) sql__setrang((short ) ((int ) ptoc_Var108->integer.C_2.mview_cnt + stackcnt) , -32768 , 32767);
                                    }
                                g10mv4("VAK660" , 4 , (int ) (*(tgg00_QualBuf *) sql__nil(hqbp , "vak660.p" , 17948)).integer.C_2.mst_max * 8 , ptoc_Var107->integer.C_2.mb_st_size , sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17950)).integer.C_5.mb_st , "vak660.p" , 17950) , startpos * 8 + -7 , sql__nil(ptoc_Var107->integer.C_5.mb_st , "vak660.p" , 17952) , (int ) ptoc_Var108->integer.C_2.mfirst_free * 8 + -7 , stackcnt * 8 , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 17955)).integer.C_4.sp1r_returncode);
                                t01int4(5 , "startpos    " , startpos);
                                if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 17957)).integer.C_4.sp1r_returncode == 0)
                                    {
                                    ptoc_Var114 = &(*dmli).d_joins;
                                    while ((int ) ptoc_Var114->jrc_joinarr[sql__setrang(joinpos , 0 , 256)].jo_recs[0].jop_startstack < startpos && joinpos < (int ) ptoc_Var114->jrc_cnt)
                                        joinpos = sql__succ(joinpos , -2147483647 , 2147483647 , "vak660.p" , 17965);
                                    if (joinpos < (int ) ptoc_Var114->jrc_cnt)
                                        {
                                        ptoc_Var115 = &ptoc_Var114->jrc_joinarr[sql__setrang(joinpos , 0 , 256)];
                                        if ((int ) ptoc_Var115->jo_recs[0].jop_startstack == startpos)
                                            {
                                            ptoc_Var115->jo_recs[0].jop_startstack = (short ) sql__setrang((short ) ((int ) ptoc_Var115->jo_recs[0].jop_startstack + (int ) ptoc_Var108->integer.C_2.mfirst_free - startpos) , -32768 , 32767);
                                            ptoc_Var115->jo_recs[1].jop_startstack = (short ) sql__setrang((short ) ((int ) ptoc_Var115->jo_recs[1].jop_startstack + (int ) ptoc_Var108->integer.C_2.mfirst_free - startpos) , -32768 , 32767);
                                            joinpos = sql__succ(joinpos , -2147483647 , 2147483647 , "vak660.p" , 17977);
                                            }
                                        }
                                    ptoc_Var108->integer.C_2.mfirst_free = (short ) sql__setrang((short ) ((int ) ptoc_Var108->integer.C_2.mfirst_free + stackcnt) , -32768 , 32767);
                                    ptoc_Var108->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) ((int ) ptoc_Var108->integer.C_2.mqual_cnt + stackcnt) , -32768 , 32767);
                                    ptoc_Var113 = startpos;
                                    ptoc_Var109 = endpos + -1;
                                    if (ptoc_Var113 <= ptoc_Var109)
                                        {
                                        stackpos = ptoc_Var113;
                                        do
                                            {
                                            ptoc_Var116 = &(*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17987)).integer.C_5.mb_st , "vak660.p" , 17987))[sql__setrang(stackpos , 1 , 32767) + -1];
                                            ptoc_Var116->integer.C_1.etype = 39;
                                            ptoc_Var116->integer.C_1.eop = 0;
                                            if (stackpos == ptoc_Var109)
                                                {
                                                break;
                                                }
                                            stackpos += 1;
                                            }
                                        while (!0);
                                        }
                                    ptoc_Var117 = &(*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 17994)).integer.C_5.mb_st , "vak660.p" , 17994))[sql__setrang(endpos , 1 , 32767) + -1];
                                    ptoc_Var117->integer.C_1.etype = 21;
                                    ptoc_Var117->integer.C_1.eop = 0;
                                    ptoc_Var117->integer.C_1.epos = 1;
                                    ptoc_Var117->integer.C_1.elen_var = 0;
                                    ptoc_Var117->integer.C_2.ecol_pos = 0;
                                    put_op_and = 1;
                                    }
                                }
                            }
                        }
                    }
                while (nextpos <= qual_end && ((ptoc_Var109 = (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(hmbp , "vak660.p" , 18014)).integer.C_5.mb_st , "vak660.p" , 18014))[sql__setrang(nextpos , 1 , 32767) + -1].integer.C_1.etype) == 31 || ptoc_Var109 == 32 || ptoc_Var109 == 26))
                    nextpos = sql__succ(nextpos , -2147483647 , 2147483647 , "vak660.p" , 18016);
                if ((char ) sql__setrang(into_w_arr , 0 , 1))
                    {
                    ptoc_Var118 = &(*(where_array *) sql__nil(*w_arr , "vak660.p" , 18021))[sql__setrang(w_pos , 1 , 4096) + -1];
                    ptoc_Var118->start = (short ) sql__setrang((short ) startpos , -32768 , 32767);
                    ptoc_Var118->length = (short ) sql__setrang((short ) (nextpos - startpos) , -32768 , 32767);
                    w_pos = sql__succ(w_pos , -2147483647 , 2147483647 , "vak660.p" , 18027);
                    (*(where_array *) sql__nil(*w_arr , "vak660.p" , 18028))[sql__setrang(w_pos , 1 , 4096) + -1].start = 0;
                    }
                }
            }
        ptoc_Var109 = and_pos;
        if (ptoc_Var109 >= 1)
            {
            stackpos = ptoc_Var109;
            do
                {
                a65_set_operator(&(*acv) , 98);
                a61_set_jump(&(*acv).a_mblock , (int ) (*(and_array_ak660where_transfer *) sql__nil(and_arr.boolean.C_true.and_ptr , "vak660.p" , 18039))[sql__setrang(stackpos , 1 , 4096) + -1] , 32);
                if (stackpos == 1)
                    {
                    break;
                    }
                stackpos -= 1;
                }
            while (!0);
            }
        a10dispose(&(*acv) , &and_arr.boolean.C_false.sbuf);
        }
    a683_output(5 , &(*dmli));
    cccprint("<ak660where_tran");
    }



int a660fsame_pred(acv, same_node, first_same_node, node_diff)
tak_all_command_glob *acv;
int same_node;
int *first_same_node;
int *node_diff;
    {
    int act_node;
    tak_all_command_glob *ptoc_Var119;
    cccprint("AK660 a660fsame_");
    ptoc_Var119 = &*acv;
    act_node = same_node;
    *node_diff = 0;
    do
        {
        *first_same_node = act_node;
        act_node = a660same_pred(&(*acv) , *first_same_node);
        if (act_node != 0)
            {
            *node_diff = *node_diff + 1;
            }
        }
    while (!(act_node == 0));
    cccprint("<a660fsame_pred ");
    }



int a660same_pred(acv, same_node)
tak_all_command_glob *acv;
int same_node;
    {
    int _a660same_pred;
    int act_node;
    tak_all_command_glob *ptoc_Var120;
    struct ptoc_Type57 *ptoc_Var121;
    cccprint("AK660 a660same_p");
    _a660same_pred = 0;
    act_node = 1;
    ptoc_Var120 = &*acv;
    while (act_node <= (int ) ptoc_Var120->a_scv_index)
        {
        ptoc_Var121 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var120->a_ap_tree , "vak660.p" , 18102))[sql__setrang(act_node , 0 , 32767)];
        if (1 && (ptoc_Var121->n_special[0] & 8) != 0)
            {
            act_node = ptoc_Var121->n_pos;
            }
        else
            {
            if ((int ) ptoc_Var121->n_sa_level == same_node)
                {
                _a660same_pred = act_node;
                act_node = 32767;
                }
            else
                {
                if (1 && (ptoc_Var121->n_special[0] & 4) != 0)
                    {
                    act_node = act_node + 2;
                    }
                else
                    {
                    act_node = act_node + 1;
                    }
                }
            }
        }
    cccprint("<a660same_pred  ");
    return(_a660same_pred);
    }



int a660lower_pred(acv, lower_node)
tak_all_command_glob *acv;
int lower_node;
    {
    int _a660lower_pred;
    int act_node;
    tak_all_command_glob *ptoc_Var122;
    struct ptoc_Type57 *ptoc_Var123;
    cccprint("AK660 a660lower_");
    _a660lower_pred = 0;
    act_node = 1;
    ptoc_Var122 = &*acv;
    while (act_node <= (int ) ptoc_Var122->a_scv_index)
        {
        ptoc_Var123 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var122->a_ap_tree , "vak660.p" , 18146))[sql__setrang(act_node , 0 , 32767)];
        if (1 && (ptoc_Var123->n_special[0] & 8) != 0)
            {
            act_node = ptoc_Var123->n_pos;
            }
        else
            {
            if ((int ) ptoc_Var123->n_lo_level == lower_node)
                {
                _a660lower_pred = act_node;
                act_node = 32767;
                }
            else
                {
                if (1 && (ptoc_Var123->n_special[0] & 4) != 0)
                    {
                    act_node = act_node + 2;
                    }
                else
                    {
                    act_node = act_node + 1;
                    }
                }
            }
        }
    cccprint("<a660lower_pred ");
    return(_a660lower_pred);
    }



int a660pos_pred(acv, pos_node)
tak_all_command_glob *acv;
int pos_node;
    {
    int _a660pos_pred;
    int act_node;
    tak_all_command_glob *ptoc_Var124;
    struct ptoc_Type57 *ptoc_Var125;
    cccprint("AK660 a660pos_pr");
    _a660pos_pred = 0;
    act_node = 1;
    ptoc_Var124 = &*acv;
    while (act_node <= (int ) ptoc_Var124->a_scv_index)
        {
        ptoc_Var125 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var124->a_ap_tree , "vak660.p" , 18190))[sql__setrang(act_node , 0 , 32767)];
        if (1 && (ptoc_Var125->n_special[0] & 8) != 0)
            {
            act_node = ptoc_Var125->n_pos;
            }
        else
            {
            if ((int ) ptoc_Var125->n_proc == 54 && ((int ) ptoc_Var125->n_subproc == 27 || (int ) ptoc_Var125->n_subproc == 12) && ptoc_Var125->n_pos == pos_node)
                {
                _a660pos_pred = act_node;
                act_node = 32767;
                }
            else
                {
                if (1 && (ptoc_Var125->n_special[0] & 4) != 0)
                    {
                    act_node = act_node + 2;
                    }
                else
                    {
                    act_node = act_node + 1;
                    }
                }
            }
        }
    cccprint("<a660pos_pred   ");
    return(_a660pos_pred);
    }



int a660_explain(acv, startnode)
tak_all_command_glob *acv;
tsp_int2 startnode;
    {
    char m_acv_info_output;
    tak_execution_kind mex_kind;
    tak_explain_rec expl;
    int del_cnt;
    int pos_result;
    tak_all_command_glob *ptoc_Var126;
    tak_explain_rec *ptoc_Var127;
    tak_resname_record *ptoc_Var128;
    cccprint("AK660 a660_expla");
    ptoc_Var126 = &*acv;
    ptoc_Var126->a_pars_explain = (char ) sql__setrang((int ) ptoc_Var126->a_ex_kind == 0 , 0 , 1);
    mex_kind = (unsigned char ) sql__setrang(ptoc_Var126->a_ex_kind , 0 , 4);
    ptoc_Var126->a_ex_kind = 0;
    m_acv_info_output = (char ) sql__setrang(ptoc_Var126->a_info_output , 0 , 1);
    ptoc_Var126->a_info_output = 0;
    ptoc_Var126->a_costcheck = 1;
    ptoc_Var126->a_intern_explain = 1;
    a660_select(&(*acv) , (short ) sql__setrang(startnode , -32768 , 32767));
    t01int4(5 , "Intern_selec" , (int ) ptoc_Var126->a_intern_select_cnt);
    t01int4(5 , "Max_intern_s" , (int ) ptoc_Var126->a_max_intern_select);
    if ((int ) ptoc_Var126->a_intern_select_cnt == (int ) ptoc_Var126->a_max_intern_select)
        {
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var126->a_return_segm , "vak660.p" , 18252)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var127 = &expl;
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var127->exp_user, ptoc_Var126->a_curr_user_name)
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var127->exp_column, a01_il_b_identifier)
            _ptoc_ASSIGN(unsigned char , 1, ptoc_Var127->exp_only_index, " ")
            _ptoc_ASSIGN(unsigned char , 1, ptoc_Var127->exp_distinct_opt, " ")
            _ptoc_ASSIGN(unsigned char , 1, ptoc_Var127->exp_temp_inv, " ")
            _ptoc_ASSIGN(unsigned char , 1, ptoc_Var127->exp_more_qual, " ")
            pos_result = 2;
            if (ptoc_Var126->a_resname_addr[sql__setrang(pos_result , 1 , 3) + -1] == (struct tak_systembuffer *) (int *) 0)
                {
                pos_result = 1;
                }
            ptoc_Var128 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var126->a_resname_addr[sql__setrang(pos_result , 1 , 3) + -1] , "vak660.p" , 18268)).integer.C_52.sresname;
            if (_ptoc_MEMCMP(unsigned char , 64, (*acv).a_expl_resname , a01_zero_res_name) == 0 ? 0 : 1)
                {
                _ptoc_ASSIGN(unsigned char , 64, ptoc_Var127->exp_table, (*acv).a_expl_resname)
                }
            else
                {
                _ptoc_ASSIGN(unsigned char , 64, ptoc_Var127->exp_table, a01_il_b_identifier)
                }
            if (1 && (ptoc_Var128->resstate[0] & 4) != 0 || 1 && (ptoc_Var128->resstate[0] & 2) != 0)
                {
                _ptoc_ASSIGN(unsigned char , 40, ptoc_Var127->exp_strat, "   RESULT IS NOT COPIED , COSTVALUE IS  ")
                }
            else
                {
                _ptoc_ASSIGN(unsigned char , 40, ptoc_Var127->exp_strat, "     RESULT IS COPIED   , COSTVALUE IS  ")
                }
            ptoc_Var128->resmaxlinkage = (unsigned char)'\0';
            ptoc_Var127->exp_pagecount = ptoc_Var126->a_costsum;
            if (ptoc_Var127->exp_pagecount == 0)
                {
                ptoc_Var127->exp_pagecount = 1;
                }
            if (!(char ) sql__setrang(ptoc_Var126->a_pars_explain , 0 , 1))
                {
                a40add_explain_record(&(*acv) , &expl);
                }
            }
        ptoc_Var126->a_info_output = (char ) sql__setrang(m_acv_info_output , 0 , 1);
        ptoc_Var126->a_ex_kind = (unsigned char ) sql__setrang(mex_kind , 0 , 4);
        t01execution_kind(5 , "a_ex_kin    " , (unsigned char ) sql__setrang(ptoc_Var126->a_ex_kind , 0 , 4));
        if ((int ) ptoc_Var126->a_ex_kind != 0)
            {
            a660_prefix_delete(&(*acv) , &ptoc_Var126->a_pars_last_key , &del_cnt , 3);
            }
        }
    cccprint("<a660_explain   ");
    }

static
  unsigned char ptoc_Var129[2] = 
  {
  (unsigned char)'\b', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var130[2] = 
  {
  (unsigned char)'\002', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var131[2] = 
  {
  (unsigned char)'\001', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var132[2] = 
  {
  (unsigned char)'\004', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var133[2] = 
  {
  (unsigned char)'\003', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var134[2] = 
  {
  (unsigned char)'\002', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var135[2] = 
  {
  (unsigned char)'\001', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var136[2] = 
  {
  (unsigned char)'\004', (unsigned char)'\0'
  };


int a660_first_sel_part(acv, dmli, select_rec, pseudo_resultset_select)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_select_record *select_rec;
char *pseudo_resultset_select;
    {
    tsp00_Int4 hintnode;
    tgg_surrogate syskey_tableid;
    struct ptoc_Type57 *ptoc_Var137;
    unsigned char ptoc_Var138[2];
    unsigned char ptoc_Var139[2];
    unsigned char ptoc_Var140[2];
    tak_unionrecord *ptoc_Var141;
    struct ptoc_Type57 *ptoc_Var142;
    struct ptoc_Type57 *ptoc_Var143;
    int ptoc_Var144;
    struct ptoc_Type57 *ptoc_Var145;
    tak_unionrecord *ptoc_Var146;
    cccprint("AK660 a660_first");
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 18327)).integer.C_4.sp1r_returncode == 0)
        {
        *pseudo_resultset_select = (char ) sql__setrang((char ) sql__setrang(*pseudo_resultset_select , 0 , 1) || (int ) (*acv).a_max_intern_select > 0 && (int ) (*acv).a_max_intern_select == (int ) (*acv).a_intern_select_cnt && (int ) (*acv).a_init_ex_kind != 0 && (1 && ((*dmli).d_globstate[0] & 64) != 0 || 1 && ((*dmli).d_globstate[0] & 128) != 0) && !(char ) sql__setrang((*dmli).d_view , 0 , 1) && !(char ) sql__setrang((*dmli).d_subquery , 0 , 1) , 0 , 1);
        t01bool(5 , "pseudo_resul" , (char ) sql__setrang(*pseudo_resultset_select , 0 , 1));
        t01bool(5 , "d_subquery  " , (char ) sql__setrang((*dmli).d_subquery , 0 , 1));
        t01bool(5 , "max_inter >0" , (char ) sql__setrang((int ) (*acv).a_max_intern_select > 0 , 0 , 1));
        t01bool(5 , "info_output " , (char ) sql__setrang((*acv).a_info_output , 0 , 1));
        t01execution_kind(5 , "a_ex_kind   " , (unsigned char ) sql__setrang((*acv).a_ex_kind , 0 , 4));
        t01bool(5 , "lock in stat" , (char ) sql__setrang(1 && ((*dmli).d_globstate[0] & 64) != 0 || 1 && ((*dmli).d_globstate[0] & 128) != 0 , 0 , 1));
        t01command_kind(5 , "command_kind" , (unsigned char ) sql__setrang((*acv).a_command_kind , 0 , 11));
        t01bool(5 , "d_view      " , (char ) sql__setrang((*dmli).d_view , 0 , 1));
        t01int4(5 , "a_union_cnt " , (*acv).a_union_cnt);
        (*select_rec).sel_output_n = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18352))[sql__setrang((int ) (*select_rec).sel_output_n , 0 , 32767)].n_lo_level , -32768 , 32767);
        ptoc_Var137 = &(*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18353))[sql__setrang((int ) (*select_rec).sel_output_n , 0 , 32767)];
        if ((int ) ptoc_Var137->n_proc == 54 && (int ) ptoc_Var137->n_subproc == 2)
            {
            (*dmli).d_distinct = 1;
            (*select_rec).sel_output_n = (short ) sql__setrang(ptoc_Var137->n_lo_level , -32768 , 32767);
            (*acv).a_union_append = 0;
            }
        else
            {
            if (!(char ) sql__setrang((*dmli).d_union , 0 , 1))
                {
                (*dmli).d_distinct = 0;
                }
            }
        (*dmli).d_inoutpos = 1;
        (*dmli).d_rowno = -1;
        if (!(char ) sql__setrang((*acv).a_insert_select , 0 , 1))
            {
            (*dmli).d_change.cr_colcount = 0;
            }
        (*dmli).d_outcolno = 1;
        (*dmli).d_exprno = 1;
        _ptoc_ASSIGN(unsigned char , 64, (*dmli).d_refname, a01_il_b_identifier)
        (*dmli).d_like = 0;
        (*dmli).d_movebefore = 8;
        (*dmli).d_pargeslen = 0;
        (*dmli).d_range = 0;
        (*dmli).d_maxlen = 0;
        (*select_rec).sel_last_pars_part = 0;
        _ptoc_ASSIGN(unsigned char , 2, (*dmli).d_globstate, (*dmli).d_unchanged_globstate)
        if ((char ) sql__setrang((*dmli).d_subquery , 0 , 1))
            {
            _ptoc_MOVE(unsigned char , 2, (*dmli).d_globstate, *(unsigned char (*)[2]) sql__setsub(ptoc_Var138 , (*dmli).d_globstate , ptoc_Var129 , sizeof(unsigned char [2])));
            if (1 && ((*dmli).d_globstate[0] & 2) != 0)
                {
                _ptoc_MOVE(unsigned char , 2, (*dmli).d_globstate, *(unsigned char (*)[2]) sql__setsub(ptoc_Var140 , (*dmli).d_globstate , ptoc_Var130 , sizeof(unsigned char [2])));
                if (1 && ((*dmli).d_globstate[0] & 128) != 0 || 1 && ((*dmli).d_globstate[0] & 64) != 0)
                    {
                    _ptoc_MOVE(unsigned char , 2, (*dmli).d_globstate, *(unsigned char (*)[2]) sql__psetadd(ptoc_Var138 , (*dmli).d_globstate , ptoc_Var131 , sizeof(unsigned char [2])));
                    }
                else
                    {
                    _ptoc_MOVE(unsigned char , 2, (*dmli).d_globstate, *(unsigned char (*)[2]) sql__psetadd(ptoc_Var140 , (*dmli).d_globstate , ptoc_Var132 , sizeof(unsigned char [2])));
                    }
                }
            a660construct_subtree(&(*acv) , &(*select_rec).sel_res_tree , (int ) (*dmli).d_subcount);
            if ((int ) (*acv).a_command_kind == 8 && (*acv).a_union_cnt > 0)
                {
                if ((*acv).a_union_cnt > 1)
                    {
                    (*select_rec).sel_res_tree.integer.C_8.fileTempCnt_gg00 = (*acv).a_union_cnt;
                    }
                _ptoc_ASSIGN(unsigned char , 64, (*select_rec).sel_res_name, a01_zero_res_name)
                a663_make_res_buf(&(*acv) , &(*dmli) , (*select_rec).sel_res_name , &(*select_rec).sel_res_tree);
                }
            (*dmli).d_single = 0;
            (*dmli).d_keylen = 8;
            (*dmli).d_inoutpos = (short ) sql__setrang((short ) ((int ) (*dmli).d_keylen + 9) , -32768 , 32767);
            (*select_rec).sel_into_n = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18420))[sql__setrang((int ) (*select_rec).sel_output_n , 0 , 32767)].n_sa_level , -32768 , 32767);
            (*select_rec).sel_from_n = (short ) sql__setrang((*select_rec).sel_into_n , -32768 , 32767);
            }
        else
            {
            if (1 && ((*dmli).d_globstate[0] & 8) != 0)
                {
                _ptoc_MOVE(unsigned char , 2, (*dmli).d_globstate, *(unsigned char (*)[2]) sql__setsub(ptoc_Var138 , (*dmli).d_globstate , ptoc_Var133 , sizeof(unsigned char [2])));
                }
            if ((*acv).a_union_cnt > 1)
                {
                ptoc_Var141 = &(*(struct tak_systembuffer *) sql__nil((*acv).a_unionrec_ptr , "vak660.p" , 18432)).integer.C_40.sunionrec;
                _ptoc_ASSIGN(unsigned char , 64, (*select_rec).sel_res_name, ptoc_Var141->ures_name)
                g04build_temp_tree_id(&(*select_rec).sel_res_tree , &(*acv).a_transinf.tri_trans);
                (*select_rec).sel_res_tree.integer.C_8.fileTfnTemp_gg00 = 19;
                (*select_rec).sel_res_tree.integer.C_8.fileLevel_gg00[0] = (unsigned char ) sql__setrang((unsigned char ) (*acv).a_union_cnt , 0 , 255);
                }
            if ((*acv).a_union_cnt <= 1)
                {
                _ptoc_ASSIGN(unsigned char , 64, (*select_rec).sel_res_name, a01_il_b_identifier)
                t01int4(5 , "a_int_sel_cn" , (int ) (*acv).a_intern_select_cnt);
                t01int4(5 , "a_max_intern" , (int ) (*acv).a_max_intern_select);
                if ((int ) (*acv).a_intern_select_cnt < (int ) (*acv).a_max_intern_select)
                    {
                    _ptoc_ASSIGN(unsigned char , 64, (*select_rec).sel_res_name, a01_zero_res_name)
                    }
                t01int4(5 , "sel_output_n" , (int ) (*select_rec).sel_output_n);
                if (!(char ) sql__setrang((*dmli).d_view , 0 , 1))
                    {
                    if ((char ) sql__setrang((*acv).a_intern_explain , 0 , 1) && a663parse_for_execute(&(*acv)))
                        {
                        _ptoc_ASSIGN(unsigned char , 64, (*select_rec).sel_res_name, (*acv).a_result_name)
                        (*dmli).d_resname_found = 1;
                        ptoc_Var142 = &(*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18463))[sql__setrang((int ) (*select_rec).sel_output_n , 0 , 32767)];
                        if ((int ) ptoc_Var142->n_proc != 51 || (int ) ptoc_Var142->n_subproc != 1)
                            {
                            (*select_rec).sel_output_n = (short ) sql__setrang(ptoc_Var142->n_sa_level , -32768 , 32767);
                            }
                        t01int4(5 , "sel_output_n" , (int ) (*select_rec).sel_output_n);
                        t01lidentifier(5 , (*select_rec).sel_res_name);
                        }
                    else
                        {
                        ptoc_Var143 = &(*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18475))[sql__setrang((int ) (*select_rec).sel_output_n , 0 , 32767)];
                        if ((int ) ptoc_Var143->n_proc == 51 && (int ) ptoc_Var143->n_subproc == 1)
                            {
                            if ((int ) (*acv).a_init_ex_kind == 2 && (!(char ) sql__setrang((*acv).a_from_select , 0 , 1) && !(char ) sql__setrang((*acv).a_insert_select , 0 , 1)))
                                {
                                if ((*acv).a_resname_part != (tsp1_part *) (int *) 0)
                                    {
                                    a501get_resname(&(*acv) , &(*dmli));
                                    _ptoc_ASSIGN(unsigned char , 64, (*select_rec).sel_res_name, (*dmli).d_resname)
                                    (*dmli).d_resname_found = 1;
                                    }
                                else
                                    {
                                    (*dmli).d_resname_found = 0;
                                    }
                                }
                            else
                                {
                                (*dmli).d_resname_found = 0;
                                }
                            }
                        else
                            {
                            (*dmli).d_resname_found = 1;
                            if ((int ) ptoc_Var143->n_symb == 42)
                                {
                                a05identifier_get(&(*acv) , (int ) (*select_rec).sel_output_n , sizeof(unsigned char [64]) , (*select_rec).sel_res_name);
                                }
                            else
                                {
                                ak660describe_resultname(&(*acv) , &(*dmli) , (*select_rec).sel_res_name , (int ) (*select_rec).sel_output_n);
                                }
                            if ((char ) sql__setrang((*acv).a_from_select , 0 , 1) || (char ) sql__setrang((*acv).a_insert_select , 0 , 1))
                                {
                                a07_nb_put_error(&(*acv) , 6364 , ptoc_Var143->n_pos , (*select_rec).sel_res_name);
                                }
                            else
                                {
                                (*select_rec).sel_output_n = (short ) sql__setrang(ptoc_Var143->n_sa_level , -32768 , 32767);
                                }
                            }
                        }
                    }
                if ((char ) sql__setrang((*acv).a_from_select , 0 , 1) || (char ) sql__setrang((*acv).a_insert_select , 0 , 1))
                    {
                    (*dmli).d_resname_found = 0;
                    _ptoc_ASSIGN(unsigned char , 64, (*select_rec).sel_res_name, a01_zero_res_name)
                    }
                }
            if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 18536)).integer.C_4.sp1r_returncode == 0)
                {
                if ((char ) sql__setrang((*acv).a_from_select , 0 , 1) || (int ) (*acv).a_command_kind == 8 && (int ) (*acv).a_fromsel_n > 0)
                    {
                    g04build_temp_tree_id(&(*select_rec).sel_res_tree , &(*acv).a_transinf.tri_trans);
                    a661_build_t_fromsel_tableid(syskey_tableid , (*select_rec).sel_res_tree.integer.C_4.fileTabId_gg00 , &(*acv).a_curr_ex_parskey , g01localsite , (*acv).a_transinf.tri_trans.trSessionId_gg00 , (short ) sql__setrang((short ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18551))[sql__setrang((int ) (*acv).a_fromsel_n , 0 , 32767)].n_pos , -32768 , 32767));
                    if ((int ) (*acv).a_command_kind == 8)
                        {
                        (*select_rec).sel_res_tree.integer.C_8.fileLevel_gg00[0] = (unsigned char ) sql__setrang((unsigned char ) ((int ) (*dmli).d_subcount / 100) , 0 , 255);
                        (*select_rec).sel_res_tree.integer.C_8.fileLevel_gg00[1] = (unsigned char ) sql__setrang((unsigned char ) ((ptoc_Var144 = (int ) (*dmli).d_subcount % 100) >= 0 ? ptoc_Var144 : ptoc_Var144 + 100) , 0 , 255);
                        if ((*acv).a_union_cnt > 1)
                            {
                            (*select_rec).sel_res_tree.integer.C_8.fileTempCnt_gg00 = (*acv).a_union_cnt;
                            }
                        _ptoc_ASSIGN(unsigned char , 64, (*select_rec).sel_res_name, a01_zero_res_name)
                        }
                    }
                (*select_rec).sel_into_n = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18566))[sql__setrang((int ) (*select_rec).sel_output_n , 0 , 32767)].n_sa_level , -32768 , 32767);
                ptoc_Var145 = &(*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18567))[sql__setrang((int ) (*select_rec).sel_into_n , 0 , 32767)];
                if ((int ) ptoc_Var145->n_proc == 51 && (int ) ptoc_Var145->n_subproc == 3)
                    {
                    if ((char ) sql__setrang(*pseudo_resultset_select , 0 , 1) || (char ) sql__setrang((*acv).a_intern_explain , 0 , 1))
                        {
                        _ptoc_ASSIGN(unsigned char , 64, (*select_rec).sel_res_name, a01_zero_res_name)
                        (*dmli).d_single = (char ) sql__setrang((char ) sql__setrang((*acv).a_intern_explain , 0 , 1) && !(char ) sql__setrang((*acv).a_parsing_again , 0 , 1) && a663parse_for_execute(&(*acv)) , 0 , 1);
                        a663_make_res_buf(&(*acv) , &(*dmli) , (*select_rec).sel_res_name , &(*select_rec).sel_res_tree);
                        (*dmli).d_global_pos_result = (short ) sql__setrang((*dmli).d_pos_result , -32768 , 32767);
                        }
                    else
                        {
                        (*dmli).d_single = 1;
                        (*dmli).d_pos_result = 3;
                        (*dmli).d_keylen = 4;
                        if ((int ) (*dmli).d_distinct != 0)
                            {
                            (*dmli).d_keylen = (short ) sql__setrang((short ) ((int ) (*dmli).d_keylen + 4) , -32768 , 32767);
                            (*acv).a_union_append = 0;
                            }
                        (*dmli).d_inoutpos = (short ) sql__setrang((short ) ((int ) (*dmli).d_keylen + 9) , -32768 , 32767);
                        (*select_rec).sel_res_tree = (*acv).a_into_tree;
                        (*acv).a_curr_res_id = 1;
                        }
                    (*select_rec).sel_from_n = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18601))[sql__setrang((int ) (*select_rec).sel_into_n , 0 , 32767)].n_sa_level , -32768 , 32767);
                    }
                else
                    {
                    if (1 && ((*dmli).d_globstate[0] & 2) != 0)
                        {
                        _ptoc_MOVE(unsigned char , 2, (*dmli).d_globstate, *(unsigned char (*)[2]) sql__setsub(ptoc_Var140 , (*dmli).d_globstate , ptoc_Var134 , sizeof(unsigned char [2])));
                        if (1 && ((*dmli).d_globstate[0] & 128) != 0 || 1 && ((*dmli).d_globstate[0] & 64) != 0)
                            {
                            if (0 || ((*dmli).d_globstate[0] & 8) == 0)
                                {
                                _ptoc_MOVE(unsigned char , 2, (*dmli).d_globstate, *(unsigned char (*)[2]) sql__psetadd(ptoc_Var138 , (*dmli).d_globstate , ptoc_Var135 , sizeof(unsigned char [2])));
                                }
                            }
                        else
                            {
                            _ptoc_MOVE(unsigned char , 2, (*dmli).d_globstate, *(unsigned char (*)[2]) sql__psetadd(ptoc_Var140 , (*dmli).d_globstate , ptoc_Var136 , sizeof(unsigned char [2])));
                            }
                        }
                    *pseudo_resultset_select = 0;
                    (*dmli).d_single = 0;
                    (*select_rec).sel_from_n = (short ) sql__setrang((*select_rec).sel_into_n , -32768 , 32767);
                    a663_make_res_buf(&(*acv) , &(*dmli) , (*select_rec).sel_res_name , &(*select_rec).sel_res_tree);
                    }
                }
            }
        if ((*acv).a_union_cnt == 1)
            {
            ptoc_Var146 = &(*(struct tak_systembuffer *) sql__nil((*acv).a_unionrec_ptr , "vak660.p" , 18639)).integer.C_40.sunionrec;
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var146->ures_name, (*select_rec).sel_res_name)
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var146->umodul_name, (*acv).a_modul_name)
            ptoc_Var146->ures_tree = (*select_rec).sel_res_tree;
            }
        t01treeid(5 , "res_tree    " , &(*select_rec).sel_res_tree);
        t01knl_identifier(5 , "sel_res_name" , (*select_rec).sel_res_name);
        (*select_rec).sel_res_b = (short ) sql__setrang((*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 18649)).integer.C_4.sp1r_returncode , -32768 , 32767);
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 18650)).integer.C_4.sp1r_returncode == 0)
            {
            if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18653))[sql__setrang((int ) (*select_rec).sel_from_n , 0 , 32767)].n_proc == 57)
                {
                t01name(5 , "ordered table sequ");
                (*dmli).d_standard = 1;
                (*select_rec).sel_from_n = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18659))[sql__setrang((int ) (*select_rec).sel_from_n , 0 , 32767)].n_sa_level , -32768 , 32767);
                }
            else
                {
                if (a80_hint_exists(&(*acv) , 5 , (int ) (*acv).a_select_node , &hintnode))
                    {
                    t01name(5 , "ordered table sequ");
                    (*dmli).d_standard = 1;
                    }
                else
                    {
                    (*dmli).d_standard = 0;
                    }
                }
            a660_from_part(&(*acv) , &(*dmli) , (int ) (*select_rec).sel_from_n);
            }
        }
    cccprint("<a660_first_sel_");
    }

static
  unsigned char ptoc_Var147[1] = 
  {
  (unsigned char)'\004'
  };


int a660_from_part(acv, dmli, from_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
int from_node;
    {
    char all;
    char is_insert_select;
    tgg_distinct m_distinct;
    tgg_basis_error b_err;
    int curr_n;
    int i;
    int next_n;
    int fs_n;
    tak_sysbufferaddress p;
    tgg_sysinfokey sysk;
    struct ptoc_Type57 *ptoc_Var148;
    tak_viewtextrecord *ptoc_Var149;
    struct ptoc_Type172 *ptoc_Var150;
    struct ptoc_Type57 *ptoc_Var151;
    unsigned char ptoc_Var152[1];
    unsigned char ptoc_Var153[1];
    unsigned char ptoc_Var154[1];
    struct ptoc_Type172 *ptoc_Var155;
    cccprint("AK660 a660_from_");
    t01int4(5 , "from_node=  " , from_node);
    ptoc_Var148 = &(*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18703))[sql__setrang(from_node , 0 , 32767)];
    if ((int ) ptoc_Var148->n_proc == 57 && (int ) ptoc_Var148->n_subproc == 3)
        {
        (*acv).a_fromsel_n = (short ) sql__setrang((short ) from_node , -32768 , 32767);
        next_n = from_node;
        }
    else
        {
        next_n = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18712))[sql__setrang(from_node , 0 , 32767)].n_lo_level;
        }
    i = next_n;
    all = 0;
    (*dmli).d_cntfromtab = 0;
    (*dmli).d_maxcounttabs = 0;
    t01int4(5 , "next_n      " , next_n);
    do
        {
        (*dmli).d_cntfromtab = (short ) sql__setrang((short ) sql__succ((int ) (*dmli).d_cntfromtab , -32768 , 32767 , "vak660.p" , 18721) , -32768 , 32767);
        i = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18722))[sql__setrang(i , 0 , 32767)].n_sa_level;
        }
    while (!(i == 0));
    t01int4(5 , "d_cntfromtab" , (int ) (*dmli).d_cntfromtab);
    if ((int ) (*dmli).d_cntfromtab > 64)
        {
        a07_b_put_error(&(*acv) , 6474 , -(int ) (*dmli).d_cntfromtab);
        }
    else
        {
        if ((1 && ((*dmli).d_globstate[0] & 64) != 0 || 1 && ((*dmli).d_globstate[0] & 128) != 0) && (int ) (*acv).a_command_kind == 0 && (int ) (*dmli).d_cntfromtab > 1 && !(char ) sql__setrang((*dmli).d_view , 0 , 1))
            {
            (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 18738)).integer.C_4.sp1r_returncode = 21001;
            }
        (*dmli).d_acttabindex = 1;
        (*dmli).d_tabarr[0].ocounttabs = (unsigned char)'\0';
        m_distinct = (unsigned char ) sql__setrang((*dmli).d_distinct , 0 , 2);
        if ((char ) sql__setrang((*dmli).d_view , 0 , 1) && (int ) (*dmli).d_corr == 0 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 18744)).integer.C_4.sp1r_returncode == 0)
            {
            sysk = a01defaultkey;
            _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_2.stableid, (*dmli).d_tableid)
            _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0\274")
            a10get_sysinfo(&(*acv) , &sysk , 1 , &p , &b_err);
            if ((int ) b_err == 0)
                {
                ptoc_Var149 = &(*(struct tak_systembuffer *) sql__nil(p , "vak660.p" , 18754)).integer.C_49.sviewtext;
                ptoc_Var149->vtfromtabcnt = (short ) sql__setrang((*dmli).d_cntfromtab , -32768 , 32767);
                a10repl_sysinfo(&(*acv) , &p , &b_err);
                }
            if ((int ) b_err != 0)
                {
                a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                }
            }
        while ((int ) (*dmli).d_acttabindex <= (int ) (*dmli).d_cntfromtab && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 18767)).integer.C_4.sp1r_returncode == 0)
            {
            if ((int ) (*dmli).d_acttabindex == (int ) (*dmli).d_cntfromtab)
                {
                all = 1;
                }
            t01int4(5 , "acttabindex " , (int ) (*dmli).d_acttabindex);
            t01int4(5 , "cntfromtab  " , (int ) (*dmli).d_cntfromtab);
            curr_n = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18775))[sql__setrang(next_n , 0 , 32767)].n_lo_level;
            ptoc_Var150 = &(*dmli).d_tabarr[sql__setrang((int ) (*dmli).d_acttabindex , 1 , 64) + -1];
            ptoc_Var151 = &(*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18776))[sql__setrang(next_n , 0 , 32767)];
            if ((int ) ptoc_Var151->n_proc == 57 && (int ) ptoc_Var151->n_subproc == 3)
                {
                fs_n = next_n;
                t01int4(5 , "curr_n      " , curr_n);
                t01int4(5 , "fs_n        " , fs_n);
                a661get_from_select_table(&(*acv) , &(*dmli) , (short ) sql__setrang((short ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18785))[sql__setrang(fs_n , 0 , 32767)].n_pos , -32768 , 32767) , (char ) sql__setrang(all , 0 , 1));
                if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18786))[sql__setrang(curr_n , 0 , 32767)].n_sa_level != 0 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 18787)).integer.C_4.sp1r_returncode == 0)
                    {
                    a05identifier_get(&(*acv) , (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18791))[sql__setrang(curr_n , 0 , 32767)].n_sa_level , sizeof(unsigned char [64]) , ptoc_Var150->oreference);
                    _ptoc_MOVE(unsigned char , 1, ptoc_Var150->ospecialname, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var152 , ptoc_Var150->ospecialname , ptoc_Var147 , sizeof(unsigned char [1])));
                    i = 1;
                    while ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 18795)).integer.C_4.sp1r_returncode == 0 && i < (int ) (*dmli).d_acttabindex)
                        {
                        if (_ptoc_MEMCMP(unsigned char , 64, (*dmli).d_tabarr[sql__setrang(i , 1 , 64) + -1].oreference , ptoc_Var150->oreference) == 0 ? 1 : 0)
                            {
                            a07_b_put_error(&(*acv) , 6256 , ptoc_Var151->n_pos);
                            }
                        else
                            {
                            if (_ptoc_MEMCMP(unsigned char , 64, (*dmli).d_tabarr[sql__setrang(i , 1 , 64) + -1].otable , ptoc_Var150->oreference) == 0 ? 1 : 0)
                                {
                                a07_b_put_error(&(*acv) , 6262 , ptoc_Var151->n_pos);
                                }
                            }
                        i = sql__succ(i , -2147483647 , 2147483647 , "vak660.p" , 18807);
                        }
                    }
                t01int4(5 , "returncode  " , (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 18813)).integer.C_4.sp1r_returncode);
                }
            else
                {
                a660_search_one_table(&(*acv) , &(*dmli) , curr_n , (char ) sql__setrang(all , 0 , 1) , 1 , 0 , 0);
                }
            next_n = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18820))[sql__setrang(next_n , 0 , 32767)].n_sa_level;
            (*dmli).d_acttabindex = (short ) sql__setrang((short ) sql__succ((int ) (*dmli).d_acttabindex , -32768 , 32767 , "vak660.p" , 18821) , -32768 , 32767);
            }
        if ((int ) (*dmli).d_cntfromtab > 1 && (int ) (*dmli).d_distinct != 0 && (int ) m_distinct == 0)
            {
            a07_b_put_error(&(*acv) , 3400 , 1);
            }
        else
            {
            if ((int ) (*acv).a_intern_select_cnt == 0)
                {
                is_insert_select = (char ) sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18832))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18832))[0].n_lo_level , 0 , 32767)].n_proc == 47 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18833))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 18833))[0].n_lo_level , 0 , 32767)].n_subproc == 10 , 0 , 1);
                }
            }
        if ((char ) sql__setrang((*dmli).d_union , 0 , 1))
            {
            (*dmli).d_distinct = (unsigned char ) sql__setrang(m_distinct , 0 , 2);
            }
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 18841)).integer.C_4.sp1r_returncode == 0 && (int ) (*dmli).d_maxcounttabs > 64)
            {
            a07_b_put_error(&(*acv) , 6474 , -(int ) (*dmli).d_maxcounttabs);
            }
        else
            {
            if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 18848)).integer.C_4.sp1r_returncode == 0 && (1 && ((*dmli).d_globstate[0] & 64) != 0 || 1 && ((*dmli).d_globstate[0] & 128) != 0) && (int ) (*acv).a_command_kind == 0 && (int ) (*dmli).d_maxcounttabs > 1 && !(char ) sql__setrang((*dmli).d_view , 0 , 1))
                {
                (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 18854)).integer.C_4.sp1r_returncode = 21001;
                }
            }
        if ((char ) sql__setrang((*acv).a_outer_join , 0 , 1) && (char ) sql__setrang((*dmli).d_subquery , 0 , 1))
            {
            (*acv).a_outer_join = 0;
            }
        }
    (*dmli).d_acttabindex = (short ) sql__setrang((*dmli).d_cntfromtab , -32768 , 32767);
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 18864)).integer.C_4.sp1r_returncode == 0)
        {
        curr_n = 1;
        while (curr_n < (int ) (*dmli).d_cntfromtab)
            {
            ptoc_Var155 = &(*dmli).d_tabarr[sql__setrang(curr_n , 1 , 64) + -1];
            next_n = curr_n + 1;
            while (next_n <= (int ) (*dmli).d_cntfromtab)
                {
                if ((_ptoc_MEMCMP(unsigned char , 64, (*dmli).d_tabarr[sql__setrang(next_n , 1 , 64) + -1].otable , ptoc_Var155->otable) == 0 ? 1 : 0) && (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var155->oreference , a01_il_b_identifier) == 0 ? 1 : 0) && (_ptoc_MEMCMP(unsigned char , 64, (*dmli).d_tabarr[sql__setrang(next_n , 1 , 64) + -1].oreference , a01_il_b_identifier) == 0 ? 1 : 0))
                    {
                    ptoc_Var155->ospecs_needed = 0;
                    (*dmli).d_tabarr[sql__setrang(next_n , 1 , 64) + -1].ospecs_needed = 0;
                    }
                next_n = sql__succ(next_n , -2147483647 , 2147483647 , "vak660.p" , 18885);
                }
            curr_n = sql__succ(curr_n , -2147483647 , 2147483647 , "vak660.p" , 18888);
            }
        }
    if ((int ) (*dmli).d_maxcounttabs > 1)
        {
        t01int4(5 , "precomp_inf1" , (int ) (*acv).a_precomp_info_byte);
        if ((_ptoc_MEMCMP(unsigned char , 5, (*acv).a_comp_vers , "62080") < 0 ? 0 : 1) && (int ) (*acv).a_comp_type != 0)
            {
            switch ((int ) (*acv).a_precomp_info_byte)
                {
                case 44 :
                    (*acv).a_precomp_info_byte = (unsigned char)'.';
                    break;
                case 45 :
                    (*acv).a_precomp_info_byte = (unsigned char)'/';
                    break;
                case 114 :
                    (*acv).a_precomp_info_byte = (unsigned char)'t';
                    break;
                case 115 :
                    (*acv).a_precomp_info_byte = (unsigned char)'u';
                    break;
                default :
                    break;
                }
            }
        t01int4(5 , "precomp_inf2" , (int ) (*acv).a_precomp_info_byte);
        }
    if ((int ) (*acv).a_max_intern_select > 0)
        {
        if ((int ) (*acv).a_intern_select_cnt == 0)
            {
            (*acv).a_info_output = 0;
            (*dmli).d_only_sem_check = 1;
            }
        }
    cccprint("<a660_from_part ");
    }

static
  tgg00_HandlingSet ptoc_Var156 = 
  {
  (unsigned char)'\001', (unsigned char)'\0'
  };
static
  tgg00_HandlingSet ptoc_Var157 = 
  {
  (unsigned char)'\001', (unsigned char)'\0'
  };
static
  tgg00_HandlingSet ptoc_Var158 = 
  {
  (unsigned char)'\002', (unsigned char)'\0'
  };
static
  tgg00_HandlingSet ptoc_Var159 = 
  {
  (unsigned char)'B', (unsigned char)'\0'
  };
static
  tgg00_HandlingSet ptoc_Var160 = 
  {
  (unsigned char)'@', (unsigned char)'\0'
  };
static
  tgg00_HandlingSet ptoc_Var161 = 
  {
  (unsigned char)'\200', (unsigned char)'\0'
  };


int a660_isolevel(acv, isolation_n, lock_state)
tak_all_command_glob *acv;
tsp_int2 isolation_n;
tgg00_HandlingSet lock_state;
    {
    tsp_int2 isolevel;
    tak_all_command_glob *ptoc_Var162;
    struct ptoc_Type57 *ptoc_Var163;
    cccprint("AK660 a660_isole");
    ptoc_Var162 = &*acv;
    ptoc_Var163 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var162->a_ap_tree , "vak660.p" , 18944))[sql__setrang((int ) isolation_n , 0 , 32767)];
    a05_unsigned_int2_get(&(*acv) , ptoc_Var163->n_pos , (short ) sql__setrang(ptoc_Var163->n_length , -32768 , 32767) , 6125 , &isolevel);
    if ((int ) ptoc_Var162->a_iso_level == 255 && (int ) isolevel <= 30)
        {
        ptoc_Var162->a_iso_level = (unsigned char ) sql__setrang((unsigned char ) isolevel , 0 , 255);
        }
    switch ((int ) isolevel)
        {
        case 9 :
        case 10 :
        case 15 :
        case 20 :
        case 30 :
            if ((int ) ptoc_Var162->a_sqlmode != 3)
                {
                a07_b_put_error(&(*acv) , 6125 , ptoc_Var163->n_pos);
                }
            break;
        case 0 :
        case 1 :
        case 2 :
        case 3 :
            isolevel = (short ) sql__setrang((short ) ((int ) isolevel * 10) , -32768 , 32767);
            break;
        default :
            a07_b_put_error(&(*acv) , 6125 , ptoc_Var163->n_pos);
            break;
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var162->a_return_segm , "vak660.p" , 18963)).integer.C_4.sp1r_returncode == 0)
        {
        switch ((int ) isolevel)
            {
            case 0 :
                _ptoc_MOVE(unsigned char , 2, lock_state, ptoc_Var156);
                break;
            case 9 :
                _ptoc_MOVE(unsigned char , 2, lock_state, ptoc_Var157);
                ptoc_Var162->a_check_lock = 1;
                break;
            case 10 :
                _ptoc_MOVE(unsigned char , 2, lock_state, ptoc_Var158);
                break;
            case 15 :
                _ptoc_MOVE(unsigned char , 2, lock_state, ptoc_Var159);
                break;
            case 20 :
                _ptoc_MOVE(unsigned char , 2, lock_state, ptoc_Var160);
                break;
            case 30 :
                _ptoc_MOVE(unsigned char , 2, lock_state, ptoc_Var161);
                break;
            default :
                (short ) sql__caseerr("vak660.p" , 18965);
                break;
            }
        }
    cccprint("<a660_isolevel  ");
    }

static
  unsigned char ptoc_Var164[2] = 
  {
  (unsigned char)'\b', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var165[2] = 
  {
  (unsigned char)'@', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var166[2] = 
  {
  (unsigned char)'\200', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var167[2] = 
  {
  (unsigned char)'\0', (unsigned char)'\001'
  };
static
  tgg00_HandlingSet ptoc_Var168 = 
  {
  (unsigned char)'\0', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var169[2] = 
  {
  (unsigned char)'\020', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var170[2] = 
  {
  (unsigned char)'\002', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var171[2] = 
  {
  (unsigned char)'\020', (unsigned char)' '
  };


int a660_lock(acv, lock_n, lock_state)
tak_all_command_glob *acv;
int lock_n;
tgg00_HandlingSet lock_state;
    {
    int curr_n;
    tgg00_HandlingSet init_lockstate;
    tak_all_command_glob *ptoc_Var172;
    unsigned char ptoc_Var173[2];
    unsigned char ptoc_Var174[2];
    unsigned char ptoc_Var175[2];
    cccprint("AK660 a660_lock ");
    ptoc_Var172 = &*acv;
    t01int4(5 , "lock_n      " , lock_n);
    curr_n = lock_n;
    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var172->a_ap_tree , "vak660.p" , 19011))[sql__setrang(lock_n , 0 , 32767)].n_proc == 42 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var172->a_ap_tree , "vak660.p" , 19012))[sql__setrang(lock_n , 0 , 32767)].n_subproc == 228)
        {
        curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var172->a_ap_tree , "vak660.p" , 19015))[sql__setrang(lock_n , 0 , 32767)].n_lo_level;
        a660_isolevel(&(*acv) , (short ) sql__setrang((short ) curr_n , -32768 , 32767) , lock_state);
        curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var172->a_ap_tree , "vak660.p" , 19017))[sql__setrang(curr_n , 0 , 32767)].n_lo_level;
        }
    _ptoc_ASSIGN(unsigned char , 2, init_lockstate, lock_state)
    if (curr_n > 0)
        {
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var172->a_ap_tree , "vak660.p" , 19023))[sql__setrang(curr_n , 0 , 32767)].n_proc == 51 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var172->a_ap_tree , "vak660.p" , 19024))[sql__setrang(curr_n , 0 , 32767)].n_subproc == 5)
            {
            _ptoc_MOVE(unsigned char , 2, lock_state, *(unsigned char (*)[2]) sql__psetadd(ptoc_Var173 , lock_state , ptoc_Var164 , sizeof(unsigned char [2])));
            curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var172->a_ap_tree , "vak660.p" , 19028))[sql__setrang(curr_n , 0 , 32767)].n_lo_level;
            }
        else
            {
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var172->a_ap_tree , "vak660.p" , 19031))[sql__setrang(curr_n , 0 , 32767)].n_proc == 51 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var172->a_ap_tree , "vak660.p" , 19032))[sql__setrang(curr_n , 0 , 32767)].n_subproc == 7)
                {
                if (sql__nre(sizeof(unsigned char [2]) , lock_state , ptoc_Var165) && sql__nre(sizeof(unsigned char [2]) , lock_state , ptoc_Var166))
                    {
                    _ptoc_MOVE(unsigned char , 2, lock_state, *(unsigned char (*)[2]) sql__psetadd(ptoc_Var175 , lock_state , ptoc_Var167 , sizeof(unsigned char [2])));
                    }
                curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var172->a_ap_tree , "vak660.p" , 19040))[sql__setrang(curr_n , 0 , 32767)].n_lo_level;
                }
            else
                {
                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var172->a_ap_tree , "vak660.p" , 19043))[sql__setrang(curr_n , 0 , 32767)].n_proc == 51 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var172->a_ap_tree , "vak660.p" , 19044))[sql__setrang(curr_n , 0 , 32767)].n_subproc == 6)
                    {
                    _ptoc_MOVE(unsigned char , 2, lock_state, ptoc_Var168);
                    curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var172->a_ap_tree , "vak660.p" , 19048))[sql__setrang(curr_n , 0 , 32767)].n_lo_level;
                    }
                }
            }
        }
    if (curr_n > 0)
        {
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var172->a_ap_tree , "vak660.p" , 19056))[sql__setrang(curr_n , 0 , 32767)].n_proc == 51 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var172->a_ap_tree , "vak660.p" , 19057))[sql__setrang(curr_n , 0 , 32767)].n_subproc == 4)
            {
            _ptoc_MOVE(unsigned char , 2, lock_state, *(unsigned char (*)[2]) sql__psetadd(ptoc_Var173 , lock_state , ptoc_Var169 , sizeof(unsigned char [2])));
            }
        else
            {
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var172->a_ap_tree , "vak660.p" , 19061))[sql__setrang(curr_n , 0 , 32767)].n_proc == 51 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var172->a_ap_tree , "vak660.p" , 19062))[sql__setrang(curr_n , 0 , 32767)].n_subproc == 8)
                {
                if (sql__erq(sizeof(unsigned char [2]) , init_lockstate , ptoc_Var170))
                    {
                    _ptoc_MOVE(unsigned char , 2, lock_state, *(unsigned char (*)[2]) sql__psetadd(ptoc_Var175 , lock_state , ptoc_Var171 , sizeof(unsigned char [2])));
                    }
                else
                    {
                    a07_b_put_error(&(*acv) , 6020 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var172->a_ap_tree , "vak660.p" , 19069))[sql__setrang(curr_n , 0 , 32767)].n_pos);
                    }
                }
            }
        }
    cccprint("<a660_lock      ");
    }

static
  tak_fcolpropset ptoc_Var176 = 
  {
  (unsigned char)'\0'
  };


int a660_lowpars_pparsp(acv, pparsp, needs_twuseold, to_release, lowpars)
tak_all_command_glob *acv;
tak_sysbufferaddress *pparsp;
char needs_twuseold;
char to_release;
tsp00_Uint1 lowpars;
    {
    tgg_basis_error b_err;
    char is_pv;
    int maxlen;
    tgg_sysinfokey ke;
    tak_all_command_glob *ptoc_Var177;
    tgg00_SysInfoKey *ptoc_Var178;
    tak_parsinforecord *ptoc_Var179;
    struct ptoc_Type145 *ptoc_Var180;
    cccprint("AK660 a660_lowpa");
    ptoc_Var177 = &*acv;
    if ((int ) ptoc_Var177->a_pars_last_key.p_no >= (int ) lowpars)
        {
        a07_b_put_error(&(*acv) , 6476 , 1);
        }
    else
        {
        is_pv = 0;
        if ((char ) sql__setrang(to_release , 0 , 1) && *pparsp != (struct tak_systembuffer *) (int *) 0)
            {
            is_pv = (char ) sql__setrang((*(struct tak_systembuffer *) sql__nil(*pparsp , "vak660.p" , 19104)).integer.C_23.sparsinfo.integer.C_2.p_const_param_expr , 0 , 1);
            a10_rel_sysinfo(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(*pparsp , "vak660.p" , 19105)).integer.C_50.syskey);
            }
        ptoc_Var178 = &ke;
        ke = a01sysnullkey;
        ptoc_Var178->integer.C_3.sauthid[0] = (unsigned char)'\377';
        s10mv1(sizeof(tak_parskey ) , sizeof(tsp00_C8 ) , &ptoc_Var177->a_pars_last_key , 1 , ptoc_Var178->integer.C_3.sauthid , 2 , 6);
        ptoc_Var178->integer.C_3.sauthid[6] = (unsigned char ) sql__setrang((unsigned char ) lowpars , 0 , 255);
        _ptoc_ASSIGN(unsigned char , 2, ptoc_Var178->integer.C_2.sentrytyp, "\0\201")
        maxlen = sizeof(tak_parsinforecord ) - sizeof(tak_parser_fields ) + ((int ) ptoc_Var177->a_count_variab + (int ) ptoc_Var177->a_no_of_likes * 2 + 25) * sizeof(struct ptoc_Type145 );
        if (maxlen > sizeof(tak_parsinforecord ))
            {
            maxlen = sizeof(tak_parsinforecord );
            }
        a10_nil_get_sysinfo(&(*acv) , &ke , 0 , (short ) sql__setrang((short ) maxlen , -32768 , 32767) , &(*pparsp) , &b_err);
        if ((int ) b_err != 0)
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , (*(tsp1_part *) sql__nil(ptoc_Var177->a_cmd_part , "vak660.p" , 19130)).integer.C_2.sp1p_buf_len);
            }
        else
            {
            ptoc_Var179 = &(*(struct tak_systembuffer *) sql__nil(*pparsp , "vak660.p" , 19132)).integer.C_23.sparsinfo;
            ptoc_Var179->integer.C_2.p_mtyp = 0;
            ptoc_Var179->integer.C_2.p_mtyp2 = 0;
            ptoc_Var179->integer.C_2.p_pagecnt = 2147483647;
            ptoc_Var179->integer.C_2.p_int_res_cnt = (short ) sql__setrang(ptoc_Var177->a_intern_select_cnt , -32768 , 32767);
            ptoc_Var179->integer.C_2.p_p_no = 0;
            ptoc_Var179->integer.C_2.p_subcntlevel = -1;
            ptoc_Var179->integer.C_2.p_cnt_infos = 0;
            ptoc_Var179->integer.C_2.p_max_infos = (short ) sql__setrang((short ) ((maxlen - (sizeof(tak_parsinforecord ) - sizeof(tak_parser_fields ))) / sizeof(struct ptoc_Type145 )) , -32768 , 32767);
            ptoc_Var179->integer.C_2.p_changeinfos_exist = 0;
            ptoc_Var179->integer.C_2.p_sqlmode = (unsigned char ) sql__setrang((*acv).a_sqlmode , 0 , 3);
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var179->integer.C_2.p_searchname, a01_il_b_identifier)
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var179->integer.C_2.p_modul_name, ptoc_Var177->a_modul_name)
            ptoc_Var179->integer.C_2.p_statement_kind = (unsigned char ) sql__setrang(ptoc_Var177->a_statement_kind , 0 , 176);
            ptoc_Var179->integer.C_2.p_const_param_expr = (char ) sql__setrang(is_pv , 0 , 1);
            ptoc_Var179->integer.C_2.p_function_code = (short ) sql__setrang((*(tsp1_segment *) sql__nil(ptoc_Var177->a_return_segm , "vak660.p" , 19151)).integer.C_4.sp1r_function_code , -32768 , 32767);
            ptoc_Var179->integer.C_2.p_prepare = (char ) sql__setrang((*(tsp1_segment *) sql__nil(a542initial_cmd_segm(&(*acv)) , "vak660.p" , 19152)).integer.C_3.sp1c_prepare , 0 , 1);
            ptoc_Var179->integer.C_2.p_session_isolevel = (unsigned char ) sql__setrang(ptoc_Var177->a_iso_level , 0 , 255);
            ptoc_Var179->integer.C_2.p_use_sess_isolevel = 0;
            ptoc_Var179->integer.C_2.p_longliteral = 0;
            ptoc_Var179->integer.C_2.p_parsing_again = (char ) sql__setrang(ptoc_Var177->a_parsing_again , 0 , 1);
            if ((char ) sql__setrang(needs_twuseold , 0 , 1))
                {
                ptoc_Var179->integer.C_2.p_cnt_infos = 1;
                ptoc_Var180 = &ptoc_Var179->integer.C_2.p_pars_infos[0];
                ptoc_Var180->fp_kind = 13;
                ptoc_Var180->fp_etype = 39;
                ptoc_Var180->fp_datatyp = 14;
                _ptoc_MOVE(unsigned char , 1, ptoc_Var180->fp_colset, ptoc_Var176);
                ptoc_Var180->tak_fp_kind_type.C_fp_fill_part2_to_len.fp_fill_71 = 2147483647;
                ptoc_Var180->tak_fp_kind_type.C_fp_fill_part2_to_len.fp_fill_72 = 2147483647;
                ptoc_Var180->tak_fp_kind_type.C_fp_fill_part2_to_len.fp_fill_73 = 32767;
                ptoc_Var180->tak_fp_kind_type.C_fp_fill_part2_to_len.fp_len = (short ) sql__setrang((short ) ptoc_Var177->a_mblock.integer.C_2.mb_data_len , -32768 , 32767);
                }
            else
                {
                ptoc_Var179->integer.C_2.p_cnt_infos = 0;
                }
            }
        }
    cccprint("<a660_lowpars_pp");
    }

static
  tak_fcolpropset ptoc_Var181 = 
  {
  (unsigned char)'\0'
  };


int a660_new_pparsp(acv, sparr, needs_twuseold, complicate)
tak_all_command_glob *acv;
tak_syspointerarr *sparr;
char needs_twuseold;
char complicate;
    {
    tgg_basis_error b_err;
    tgg_sysinfokey ke;
    int maxlen;
    tak_all_command_glob *ptoc_Var182;
    tak_parskey *ptoc_Var183;
    tak_parskey *ptoc_Var184;
    int ptoc_Var185;
    tgg00_SysInfoKey *ptoc_Var186;
    tak_parsinforecord *ptoc_Var187;
    struct ptoc_Type145 *ptoc_Var188;
    cccprint("AK660 a660_new_p");
    ptoc_Var182 = &*acv;
    t01int4(5 , "complicate  " , sql__ord((int ) (char ) sql__setrang(complicate , 0 , 1)));
    t01int4(5 , "union_cnt   " , ptoc_Var182->a_union_cnt);
    t01int4(5 , "command_kind" , sql__ord((int ) ptoc_Var182->a_command_kind));
    if ((char ) sql__setrang(complicate , 0 , 1))
        {
        ptoc_Var183 = &ptoc_Var182->a_pars_last_key;
        if (sql__ord((int ) ptoc_Var183->p_id[0]) < 255)
            {
            ptoc_Var183->p_id[0] = (unsigned char ) sql__setrang((unsigned char ) sql__succ((int ) ptoc_Var183->p_id[0] , 0 , 255 , "vak660.p" , 19220) , 0 , 255);
            }
        else
            {
            a07_b_put_error(&(*acv) , 6476 , 1);
            }
        ptoc_Var183->p_no = (unsigned char)'\0';
        a54set_complex_entry(&(*acv) , 0);
        }
    else
        {
        if ((int ) ptoc_Var182->a_pars_last_key.p_no + 1 > 255)
            {
            a07_b_put_error(&(*acv) , 6476 , 1);
            }
        else
            {
            ptoc_Var184 = &ptoc_Var182->a_pars_last_key;
            ptoc_Var184->p_no = (unsigned char ) sql__setrang((unsigned char ) ((int ) ptoc_Var184->p_no + 1) , 0 , 255);
            if ((ptoc_Var185 = (int ) ptoc_Var182->a_command_kind) == 1 || ptoc_Var185 == 3 || ptoc_Var185 == 4)
                {
                a54set_complex_entry(&(*acv) , 3);
                }
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var182->a_return_segm , "vak660.p" , 19250)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var186 = &ke;
        ke = a01sysnullkey;
        ptoc_Var186->integer.C_3.sauthid[0] = (unsigned char)'\377';
        s10mv1(sizeof(tak_parskey ) , sizeof(tsp00_C8 ) , &ptoc_Var182->a_pars_last_key , 1 , ptoc_Var186->integer.C_3.sauthid , 2 , 6);
        _ptoc_ASSIGN(unsigned char , 2, ptoc_Var186->integer.C_2.sentrytyp, "\0\201")
        maxlen = sizeof(tak_parsinforecord ) - sizeof(tak_parser_fields ) + ((int ) ptoc_Var182->a_count_variab + (int ) ptoc_Var182->a_no_of_likes * 2 + 25) * sizeof(struct ptoc_Type145 );
        if (maxlen > sizeof(tak_parsinforecord ))
            {
            maxlen = sizeof(tak_parsinforecord );
            }
        a10_nil_get_sysinfo(&(*acv) , &ke , 0 , (short ) sql__setrang((short ) maxlen , -32768 , 32767) , &(*sparr).pparsp , &b_err);
        if ((int ) b_err != 0)
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , (*(tsp1_part *) sql__nil(ptoc_Var182->a_cmd_part , "vak660.p" , 19275)).integer.C_2.sp1p_buf_len);
            }
        else
            {
            ptoc_Var187 = &(*(struct tak_systembuffer *) sql__nil((*sparr).pparsp , "vak660.p" , 19277)).integer.C_23.sparsinfo;
            ptoc_Var187->integer.C_2.p_mtyp = 0;
            ptoc_Var187->integer.C_2.p_mtyp2 = 0;
            ptoc_Var187->integer.C_2.p_pagecnt = 2147483647;
            ptoc_Var187->integer.C_2.p_int_res_cnt = (short ) sql__setrang(ptoc_Var182->a_intern_select_cnt , -32768 , 32767);
            ptoc_Var187->integer.C_2.p_p_no = 0;
            ptoc_Var187->integer.C_2.p_subcntlevel = -1;
            ptoc_Var187->integer.C_2.p_sqlmode = (unsigned char ) sql__setrang((*acv).a_sqlmode , 0 , 3);
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var187->integer.C_2.p_searchname, a01_il_b_identifier)
            ptoc_Var187->integer.C_2.p_select = 0;
            ptoc_Var187->integer.C_2.p_single = 0;
            ptoc_Var187->integer.C_2.p_longliteral = 0;
            ptoc_Var187->integer.C_2.p_parsing_again = (char ) sql__setrang((*acv).a_parsing_again , 0 , 1);
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var187->integer.C_2.p_modul_name, ptoc_Var182->a_modul_name)
            ptoc_Var187->integer.C_2.p_statement_kind = (unsigned char ) sql__setrang(ptoc_Var182->a_statement_kind , 0 , 176);
            ptoc_Var187->integer.C_2.p_const_param_expr = 0;
            ptoc_Var187->integer.C_2.p_function_code = (short ) sql__setrang((*(tsp1_segment *) sql__nil(ptoc_Var182->a_return_segm , "vak660.p" , 19295)).integer.C_4.sp1r_function_code , -32768 , 32767);
            ptoc_Var187->integer.C_2.p_prepare = (char ) sql__setrang((*(tsp1_segment *) sql__nil(a542initial_cmd_segm(&(*acv)) , "vak660.p" , 19296)).integer.C_3.sp1c_prepare , 0 , 1);
            ptoc_Var187->integer.C_2.p_changeinfos_exist = 0;
            ptoc_Var187->integer.C_2.p_session_isolevel = (unsigned char ) sql__setrang(ptoc_Var182->a_iso_level , 0 , 255);
            ptoc_Var187->integer.C_2.p_use_sess_isolevel = 0;
            ptoc_Var187->integer.C_2.p_max_infos = (short ) sql__setrang((short ) ((maxlen - (sizeof(tak_parsinforecord ) - sizeof(tak_parser_fields ))) / sizeof(struct ptoc_Type145 )) , -32768 , 32767);
            if ((char ) sql__setrang(needs_twuseold , 0 , 1))
                {
                ptoc_Var187->integer.C_2.p_cnt_infos = 1;
                ptoc_Var188 = &ptoc_Var187->integer.C_2.p_pars_infos[0];
                ptoc_Var188->fp_kind = 13;
                ptoc_Var188->fp_etype = 39;
                ptoc_Var188->fp_datatyp = 14;
                _ptoc_MOVE(unsigned char , 1, ptoc_Var188->fp_colset, ptoc_Var181);
                ptoc_Var188->tak_fp_kind_type.C_fp_fill_part2_to_len.fp_fill_71 = 2147483647;
                ptoc_Var188->tak_fp_kind_type.C_fp_fill_part2_to_len.fp_fill_72 = 2147483647;
                ptoc_Var188->tak_fp_kind_type.C_fp_fill_part2_to_len.fp_fill_73 = 32767;
                ptoc_Var188->tak_fp_kind_type.C_fp_fill_part2_to_len.fp_len = (short ) sql__setrang((short ) ptoc_Var182->a_mblock.integer.C_2.mb_data_len , -32768 , 32767);
                }
            else
                {
                ptoc_Var187->integer.C_2.p_cnt_infos = 0;
                }
            }
        }
    cccprint("<a660_new_pparsp");
    }



int a660_prefix_delete(acv, parsk, del_cnt, prefix_length)
tak_all_command_glob *acv;
tak_parskey *parsk;
int *del_cnt;
int prefix_length;
    {
    tgg_basis_error e;
    tgg_basis_error e1;
    tgg_bd_set_result_record set_result;
    tsp_int_map_c2 imap;
    tsp_int2 mret;
    int i;
    int pos;
    tgg00_FilePos tree_pos;
    tgg_lkey curr_key;
    tgg_sysinfokey skey;
    tak_all_command_glob *ptoc_Var189;
    struct tgg00_MessBlock *ptoc_Var190;
    tgg00_BdSetResultRecord *ptoc_Var191;
    cccprint("AK660 a660_prefi");
    ptoc_Var189 = &*acv;
    ptoc_Var190 = &ptoc_Var189->a_mblock;
    t01int4(5 , "prefix_len  " , prefix_length);
    mret = (short ) sql__setrang((*(tsp1_segment *) sql__nil(ptoc_Var189->a_return_segm , "vak660.p" , 19365)).integer.C_4.sp1r_returncode , -32768 , 32767);
    (*(tsp1_segment *) sql__nil(ptoc_Var189->a_return_segm , "vak660.p" , 19366)).integer.C_4.sp1r_returncode = 0;
    *del_cnt = 0;
    a10prefix_parsid_delete(&(*acv) , &(*parsk) , &(*del_cnt) , prefix_length);
    skey = a01sysnullkey;
    e = 350;
    curr_key.integer.C_11.k[0] = (unsigned char)'\377';
    g10mv3("VAK660" , 5 , sizeof(tak_parskey ) , sizeof(tsp00_Key ) , &(*parsk) , 1 , curr_key.integer.C_11.k , 2 , prefix_length , &e);
    curr_key.integer.C_11.len = (short ) sql__setrang((short ) (prefix_length + 1) , -32768 , 32767);
    ptoc_Var191 = &set_result;
    ptoc_Var191->bd_key_check_len = prefix_length + 1;
    ptoc_Var191->bd_max_rec_cnt = 32767;
    ptoc_Var191->bd_max_fill_len = ptoc_Var190->integer.C_2.mb_data_size;
    ptoc_Var191->bd_next = 1;
    while ((int ) e == 350)
        {
        tree_pos.tpsPno_gg00 = 2147483647;
        b07cnext_record(&ptoc_Var189->a_transinf.tri_trans , &ptoc_Var189->a_pars_curr , &curr_key , &set_result , &tree_pos , (*(tgg00_DataPart *) sql__nil(ptoc_Var190->integer.C_3.mb_data , "vak660.p" , 19388)).integer.C_1.mbp_buf);
        if ((int ) ptoc_Var189->a_transinf.tri_trans.trError_gg00 == 1200)
            {
            e = 0;
            }
        else
            {
            e = (short ) sql__setrang(ptoc_Var189->a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
            }
        if ((int ) e == 0 || (int ) e == 350)
            {
            i = 1;
            pos = 1;
            e1 = 0;
            while ((int ) e1 == 0 && i <= set_result.bd_rec_cnt)
                {
                g10mv2("VAK660" , 6 , ptoc_Var190->integer.C_2.mb_data_size , sizeof(tgg00_SysInfoKey ) , (*(tgg00_DataPart *) sql__nil(ptoc_Var190->integer.C_3.mb_data , "vak660.p" , 19405)).integer.C_1.mbp_buf , pos + 2 , &skey , 3 , 18 , &e1);
                a10del_sysinfo(&(*acv) , &skey , &e1);
                *del_cnt = sql__succ(*del_cnt , -2147483647 , 2147483647 , "vak660.p" , 19408);
                i = sql__succ(i , -2147483647 , 2147483647 , "vak660.p" , 19409);
                imap.boolean.C_false.map_c2[0] = (unsigned char ) sql__setrang((*(tgg00_DataPart *) sql__nil(ptoc_Var190->integer.C_3.mb_data , "vak660.p" , 19410)).integer.C_1.mbp_buf[sql__setrang(pos , 1 , 8192000) + -1] , 0 , 255);
                imap.boolean.C_false.map_c2[1] = (unsigned char ) sql__setrang((*(tgg00_DataPart *) sql__nil(ptoc_Var190->integer.C_3.mb_data , "vak660.p" , 19411)).integer.C_1.mbp_buf[sql__setrang(pos + 1 , 1 , 8192000) + -1] , 0 , 255);
                pos = pos + (int ) imap.boolean.C_true.map_int;
                }
            if ((int ) e1 != 0)
                {
                e = (short ) sql__setrang(e1 , -32768 , 32767);
                }
            if (i == set_result.bd_rec_cnt + 1)
                {
                pos = pos - (int ) imap.boolean.C_true.map_int;
                g10mv1("VAK660" , 7 , ptoc_Var190->integer.C_2.mb_data_size , sizeof(tsp00_Key ) , (*(tgg00_DataPart *) sql__nil(ptoc_Var190->integer.C_3.mb_data , "vak660.p" , 19425)).integer.C_1.mbp_buf , pos + 8 , curr_key.integer.C_11.k , 1 , 12 , &e);
                curr_key.integer.C_11.len = 12;
                }
            }
        }
    if ((int ) e != 0)
        {
        if ((int ) e == 1420 || (int ) e == 350)
            {
            e = 0;
            }
        else
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(e , -32768 , 32767) , 1);
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var189->a_return_segm , "vak660.p" , 19444)).integer.C_4.sp1r_returncode == 0 || (int ) mret != 0 && (int ) mret != 100)
        {
        (*(tsp1_segment *) sql__nil(ptoc_Var189->a_return_segm , "vak660.p" , 19448)).integer.C_4.sp1r_returncode = (short ) sql__setrang(mret , -32768 , 32767);
        }
    cccprint("<a660_prefix_del");
    }

typedef
  struct ptoc_Type111 tak_joinarr_a660_query_execute[257];
static
  unsigned char ptoc_Var192[2] = 
  {
  (unsigned char)'\007', (unsigned char)'\0'
  };


int a660_query_execute(acv, dmli, s_n, new_parsinfo, filled_part2_bytes, sr_rec, pseudo_resultset_select, level, no_in_level, from_sel_found)
tak_all_command_glob *acv;
tak_dml_info *dmli;
int s_n;
char new_parsinfo;
int filled_part2_bytes;
tak70_strat_rec *sr_rec;
char *pseudo_resultset_select;
int level;
int no_in_level;
char from_sel_found;
    {
    char is_updateable;
    char old_atgroup;
    char md_union;
    char f_ok;
    char set_change_found;
    char stamp_found;
    char delete_resinfo;
    char old_one_join_phase;
    tgg_basis_error b_err;
    int bytes_to_fill;
    int fetch_only_n;
    int i;
    int jx;
    int next_n;
    int reuse_n;
    int resdel_n;
    int in_n;
    int id_n;
    int op_n;
    int exists_n;
    int update_n;
    int where_n;
    int ix;
    int qstart;
    int m_return;
    int aux_data_len;
    tsp_knl_identifier modul_name;
    tak_syspointerarr p_arr;
    tak_sc_symbol h_symb;
    tak_select_record select_rec;
    store_for_endpart sfe_rec;
    tgg_message_type messagetype;
    tgg_sysinfokey hbuf_ke;
    tak_sysbufferaddress old_hbuf;
    tgg00_FileId old_sel_res_tree;
    struct ptoc_Type190
      {
      union  
        {
        struct  
          {
          tak_joinarr_a660_query_execute *joinarr_ptr;
          } C_true;
        struct  
          {
          tak_sysbufferaddress sbuf;
          } C_false;
        } boolean;
      } stored_joinarr;
    int ptoc_Var193;
    struct ptoc_Type57 *ptoc_Var194;
    struct ptoc_Type57 *ptoc_Var195;
    struct ptoc_Type172 *ptoc_Var196;
    struct tgg00_MessBlock *ptoc_Var197;
    tgg00_DataPart *ptoc_Var198;
    unsigned char ptoc_Var199[2];
    unsigned char ptoc_Var200[2];
    unsigned char ptoc_Var201[2];
    struct tgg00_MessBlock *ptoc_Var202;
    tgg00_QualBuf *ptoc_Var203;
    int ptoc_Var204;
    struct tgg00_MessBlock *ptoc_Var205;
    tgg00_QualBuf *ptoc_Var206;
    struct tgg00_MessBlock *ptoc_Var207;
    tgg00_QualBuf *ptoc_Var208;
    struct ptoc_Type39 *ptoc_Var209;
    struct ptoc_Type39 *ptoc_Var210;
    tgg00_SysInfoKey *ptoc_Var211;
    store_for_endpart *ptoc_Var212;
    struct ptoc_Type172 *ptoc_Var213;
    cccprint("AK660 a660_query");
    t01execution_kind(5 , "init_ex_kind" , (unsigned char ) sql__setrang((*acv).a_init_ex_kind , 0 , 4));
    t01execution_kind(5 , "ex_kind     " , (unsigned char ) sql__setrang((*acv).a_ex_kind , 0 , 4));
    select_rec.sel_is_not_corr_search = 1;
    ptoc_Var193 = sizeof(unsigned char [64]);
    if (1 <= ptoc_Var193)
        {
        i = 1;
        do
            {
            modul_name[sql__setrang(i , 1 , 64) + -1] = (unsigned char)' ';
            if (i == ptoc_Var193)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    select_rec.sel_output_n = (short ) sql__setrang((short ) s_n , -32768 , 32767);
    if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19521))[sql__setrang(s_n , 0 , 32767)].n_sa_level != 0)
        {
        select_rec.sel_has_subquery = 1;
        }
    else
        {
        select_rec.sel_has_subquery = 0;
        }
    (*dmli).d_reuse = 0;
    (*dmli).d_level[sql__setrang(level , 1 , 4) + -1] = (unsigned char ) sql__setrang((unsigned char ) no_in_level , 0 , 255);
    (*dmli).d_oldlowpars = (unsigned char ) sql__setrang((*dmli).d_lowpars , 0 , 255);
    fetch_only_n = 0;
    select_rec.sel_res_b = 2;
    select_rec.sel_parinf_b = 2;
    delete_resinfo = 0;
    if (((char ) sql__setrang((*dmli).d_subquery , 0 , 1) && (int ) (*dmli).d_corr == 0 || (char ) sql__setrang((*acv).a_insert_select , 0 , 1) && (int ) (*acv).a_command_kind != 5) && (int ) (*acv).a_command_kind != 8 && !(char ) sql__setrang((*acv).a_from_select , 0 , 1))
        {
        _ptoc_ASSIGN(unsigned char , 64, select_rec.sel_res_name, a01_zero_res_name)
        a06a_mblock_init(&(*acv) , 0 , 0 , &b01niltree_id);
        (*dmli).d_pos_result = 2;
        a663_get_result_info(&(*acv) , select_rec.sel_res_name , modul_name , &(*acv).a_resname_addr[sql__setrang((int ) (*dmli).d_pos_result , 1 , 3) + -1] , 1 , 21 , 0 , &f_ok);
        }
    if ((int ) (*acv).a_ex_kind == 0 && (char ) sql__setrang(new_parsinfo , 0 , 1))
        {
        if ((char ) sql__setrang((*acv).a_insert_select , 0 , 1))
            {
            messagetype = 36;
            }
        else
            {
            if ((char ) sql__setrang((*acv).a_intern_explain , 0 , 1))
                {
                messagetype = 63;
                }
            else
                {
                messagetype = 60;
                }
            }
        a54_get_pparsp_pinfop(&(*acv) , &(*dmli).d_sparr , (unsigned char ) sql__setrang(messagetype , 0 , 95));
        select_rec.sel_parinf_b = (short ) sql__setrang((*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 19571)).integer.C_4.sp1r_returncode , -32768 , 32767);
        }
    a660_first_sel_part(&(*acv) , &(*dmli) , &select_rec , &(*pseudo_resultset_select));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 19575)).integer.C_4.sp1r_returncode != 21001)
        {
        if ((int ) (*dmli).d_corr != 0)
            {
            (*acv).a_from_select = 0;
            }
        if ((char ) sql__setrang((*dmli).d_subquery , 0 , 1))
            {
            if ((int ) (*acv).a_command_kind != 8 || (*acv).a_union_cnt == 0)
                {
                op_n = a660pos_pred(&(*acv) , s_n);
                if (op_n > 0 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19591))[sql__setrang(op_n , 0 , 32767)].n_proc == 54 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19592))[sql__setrang(op_n , 0 , 32767)].n_subproc == 27)
                    {
                    (*dmli).d_cntpar = 1;
                    (*dmli).d_rowno = -2;
                    }
                else
                    {
                    in_n = a660same_pred(&(*acv) , s_n);
                    op_n = a660lower_pred(&(*acv) , s_n);
                    if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19602))[sql__setrang(in_n , 0 , 32767)].n_proc == 54 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19603))[sql__setrang(in_n , 0 , 32767)].n_subproc >= 13 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19604))[sql__setrang(in_n , 0 , 32767)].n_subproc <= 19 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19606))[sql__setrang(op_n , 0 , 32767)].n_proc == 54 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19607))[sql__setrang(op_n , 0 , 32767)].n_subproc >= 13 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19608))[sql__setrang(op_n , 0 , 32767)].n_subproc <= 19)
                        {
                        }
                    else
                        {
                        if ((char ) sql__setrang((*acv).a_from_select , 0 , 1))
                            {
                            (*dmli).d_cntpar = 1024;
                            }
                        else
                            {
                            in_n = a660same_pred(&(*acv) , s_n);
                            ptoc_Var194 = &(*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19620))[sql__setrang(in_n , 0 , 32767)];
                            if ((int ) ptoc_Var194->n_proc == 0 && ((ptoc_Var193 = (int ) ptoc_Var194->n_symb) == 78 || ptoc_Var193 == 77))
                                {
                                in_n = a660same_pred(&(*acv) , in_n);
                                }
                            t01int4(5 , "s_n         " , s_n);
                            t01int4(5 , "in_n        " , in_n);
                            ptoc_Var195 = &(*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19628))[sql__setrang(in_n , 0 , 32767)];
                            if ((int ) ptoc_Var195->n_proc == 47 && (int ) ptoc_Var195->n_subproc == 4)
                                {
                                (*dmli).d_cntpar = (short ) sql__setrang(ptoc_Var195->n_length , -32768 , 32767);
                                }
                            else
                                {
                                if ((int ) ptoc_Var195->n_proc == 47 && (int ) ptoc_Var195->n_subproc == 3)
                                    {
                                    (*dmli).d_cntpar = (short ) sql__setrang(ptoc_Var195->n_length , -32768 , 32767);
                                    }
                                else
                                    {
                                    (*dmli).d_cntpar = 1;
                                    }
                                }
                            }
                        }
                    t01int4(5 , "d_cntpar    " , (int ) (*dmli).d_cntpar);
                    id_n = a660same_pred(&(*acv) , s_n);
                    a660fsame_pred(&(*acv) , id_n , &op_n , &i);
                    op_n = a660lower_pred(&(*acv) , op_n);
                    if (id_n != 0 && op_n != 0 && (((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19651))[sql__setrang(id_n , 0 , 32767)].n_proc != 0 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19653))[sql__setrang(id_n , 0 , 32767)].n_symb != 78 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19654))[sql__setrang(id_n , 0 , 32767)].n_symb != 77) && ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19656))[sql__setrang(op_n , 0 , 32767)].n_proc != 55 || (ptoc_Var193 = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19657))[sql__setrang(op_n , 0 , 32767)].n_symb) != 28 && ptoc_Var193 != 27)))
                        {
                        (*dmli).d_rowno = -2;
                        }
                    else
                        {
                        exists_n = a660lower_pred(&(*acv) , s_n);
                        if (exists_n != 0 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19664))[sql__setrang(exists_n , 0 , 32767)].n_symb == 115)
                            {
                            (*dmli).d_rowno = 1;
                            (*dmli).d_cntpar = (short ) sql__setrang((short ) ((int ) (*acv).a_count_variab + 1024) , -32768 , 32767);
                            }
                        }
                    }
                }
            (*dmli).d_reuse = 1;
            }
        t01int4(5 , "d_cntpar    " , (int ) (*dmli).d_cntpar);
        t01bool(5 , "a_insert_sel" , (char ) sql__setrang((*acv).a_insert_select , 0 , 1));
        if ((char ) sql__setrang(*pseudo_resultset_select , 0 , 1))
            {
            (*dmli).d_rowno = 0;
            }
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 19686)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var196 = &(*dmli).d_tabarr[0];
            (*dmli).d_join = 0;
            if ((int ) (*dmli).d_maxcounttabs == 1)
                {
                if (1 && (ptoc_Var196->ospecialname[0] & 32) != 0)
                    {
                    a06a_mblock_init(&(*acv) , 63 , 0 , &b01niltree_id);
                    (*acv).a_mblock.integer.C_2.mb_struct = 9;
                    }
                else
                    {
                    a06a_mblock_init(&(*acv) , 60 , 0 , &ptoc_Var196->otreeid);
                    if (1 && (ptoc_Var196->ospecialname[0] & 64) != 0)
                        {
                        (*acv).a_mblock.integer.C_2.mb_replicated = 1;
                        }
                    }
                }
            else
                {
                a06a_mblock_init(&(*acv) , 60 , 68 , &b01niltree_id);
                }
            if ((char ) sql__setrang((*acv).a_insert_select , 0 , 1) && (int ) (*dmli).d_corr == 0)
                {
                ptoc_Var197 = &(*acv).a_mblock;
                ptoc_Var198 = (tgg00_DataPart *) sql__nil(ptoc_Var197->integer.C_3.mb_data , "vak660.p" , 19715);
                ptoc_Var197->integer.C_2.mb_data_len = 18;
                (*dmli).d_movebefore = (short ) sql__setrang((short ) ((int ) (*dmli).d_movebefore + 1) , -32768 , 32767);
                _ptoc_ASSIGN(unsigned char , 1, ptoc_Var198->integer.C_9.mbp_info.integer.C_21.rb_ins_sel_info, "\0")
                ptoc_Var198->integer.C_9.mbp_info.integer.C_21.rb_into_temp = 0;
                }
            else
                {
                t01int4(5 , "filled_part2" , filled_part2_bytes);
                (*acv).a_mblock.integer.C_2.mb_data_len = filled_part2_bytes;
                if ((char ) sql__setrang((*acv).a_insert_select , 0 , 1))
                    {
                    _ptoc_ASSIGN(unsigned char , 1, (*(tgg00_DataPart *) sql__nil((*acv).a_mblock.integer.C_3.mb_data , "vak660.p" , 19732)).integer.C_9.mbp_info.integer.C_21.rb_ins_sel_info, "\0")
                    (*(tgg00_DataPart *) sql__nil((*acv).a_mblock.integer.C_3.mb_data , "vak660.p" , 19733)).integer.C_9.mbp_info.integer.C_21.rb_into_temp = 0;
                    }
                if (filled_part2_bytes > 0)
                    {
                    if ((char ) sql__setrang((*acv).a_insert_select , 0 , 1))
                        {
                        bytes_to_fill = filled_part2_bytes + -18;
                        }
                    else
                        {
                        bytes_to_fill = filled_part2_bytes + -8;
                        }
                    if (bytes_to_fill > 0)
                        {
                        g10fil1("VAK660" , 8 , (*acv).a_mblock.integer.C_2.mb_data_size , (*(tgg00_DataPart *) sql__nil((*acv).a_mblock.integer.C_3.mb_data , "vak660.p" , 19750)).integer.C_1.mbp_buf , filled_part2_bytes - bytes_to_fill + 1 , bytes_to_fill , (unsigned char)'\375' , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 19753)).integer.C_4.sp1r_returncode);
                        }
                    }
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 19763)).integer.C_4.sp1r_returncode == 0 && (char ) sql__setrang((*acv).a_info_output , 0 , 1) && !(char ) sql__setrang((*dmli).d_only_sem_check , 0 , 1))
            {
            a60_get_longinfobuffer(&(*acv) , &(*dmli).d_sparr , 1024 , (*acv).a_curr_res_id);
            }
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 19770)).integer.C_4.sp1r_returncode == 0)
            {
            if ((int ) (*acv).a_ex_kind != 0 || !(char ) sql__setrang(new_parsinfo , 0 , 1))
                {
                select_rec.sel_parinf_b = (short ) sql__setrang((*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 19775)).integer.C_4.sp1r_returncode , -32768 , 32767);
                }
            if ((int ) (*acv).a_sqlmode == 1)
                {
                fetch_only_n = (int ) select_rec.sel_from_n;
                do
                    fetch_only_n = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19782))[sql__setrang(fetch_only_n , 0 , 32767)].n_sa_level;
                while (!(fetch_only_n == 0 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19785))[sql__setrang(fetch_only_n , 0 , 32767)].n_proc == 54 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19786))[sql__setrang(fetch_only_n , 0 , 32767)].n_subproc == 23));
                }
            if (!(char ) sql__setrang((*dmli).d_union , 0 , 1))
                {
                select_rec.sel_order_n = (short ) sql__setrang(select_rec.sel_from_n , -32768 , 32767);
                do
                    select_rec.sel_order_n = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19796))[sql__setrang((int ) select_rec.sel_order_n , 0 , 32767)].n_sa_level , -32768 , 32767);
                while (!((int ) select_rec.sel_order_n == 0 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19799))[sql__setrang((int ) select_rec.sel_order_n , 0 , 32767)].n_proc == 54 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19800))[sql__setrang((int ) select_rec.sel_order_n , 0 , 32767)].n_subproc == 5));
                }
            else
                {
                select_rec.sel_order_n = (short ) sql__setrang((*dmli).d_union_order_n , -32768 , 32767);
                }
            (*dmli).d_use_order = (char ) sql__setrang((int ) select_rec.sel_order_n != 0 , 0 , 1);
            (*dmli).d_order_cols.ocntord = 0;
            select_rec.sel_group_n = (short ) sql__setrang(select_rec.sel_from_n , -32768 , 32767);
            do
                select_rec.sel_group_n = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19810))[sql__setrang((int ) select_rec.sel_group_n , 0 , 32767)].n_sa_level , -32768 , 32767);
            while (!((int ) select_rec.sel_group_n == 0 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19813))[sql__setrang((int ) select_rec.sel_group_n , 0 , 32767)].n_proc == 54 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19814))[sql__setrang((int ) select_rec.sel_group_n , 0 , 32767)].n_subproc == 7));
            (*dmli).d_group = 0;
            if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19819))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb != 57 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19820))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb != 91 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19821))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb != 61 && (*acv).a_union_cnt > 0 && (int ) select_rec.sel_group_n != 0 || ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19826))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb == 57 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19827))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb == 91 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19828))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb == 61) && (*acv).a_union_cnt > 0 && (int ) select_rec.sel_group_n == 0)
                {
                (*dmli).d_distinct = 1;
                (*acv).a_union_append = 0;
                }
            if ((int ) select_rec.sel_group_n != 0)
                {
                select_rec.sel_having_n = (short ) sql__setrang(select_rec.sel_group_n , -32768 , 32767);
                }
            else
                {
                select_rec.sel_having_n = (short ) sql__setrang(select_rec.sel_from_n , -32768 , 32767);
                }
            do
                select_rec.sel_having_n = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19845))[sql__setrang((int ) select_rec.sel_having_n , 0 , 32767)].n_sa_level , -32768 , 32767);
            while (!((int ) select_rec.sel_having_n == 0 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19848))[sql__setrang((int ) select_rec.sel_having_n , 0 , 32767)].n_proc == 54 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19849))[sql__setrang((int ) select_rec.sel_having_n , 0 , 32767)].n_subproc == 8));
            if ((int ) select_rec.sel_group_n == 0 && (int ) select_rec.sel_having_n > 0)
                {
                if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19854))[sql__setrang((int ) select_rec.sel_having_n , 0 , 32767)].n_symb == 57)
                    {
                    select_rec.sel_group_n = (short ) sql__setrang(select_rec.sel_having_n , -32768 , 32767);
                    }
                }
            if ((*acv).a_union_cnt > 0 && (char ) sql__setrang((*acv).a_first_union , 0 , 1))
                {
                select_rec.sel_group_n = 0;
                }
            (*dmli).d_having = 0;
            if ((char ) sql__setrang((*dmli).d_single , 0 , 1) && (int ) (*acv).a_sqlmode != 2 || (int ) (*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.pbasep , "vak660.p" , 19867)).integer.C_1.sbase.btablekind == 7 && (int ) (*acv).a_sqlmode != 3 || (int ) (*dmli).d_distinct != 0 || (char ) sql__setrang((*dmli).d_subquery , 0 , 1) || (char ) sql__setrang((*acv).a_from_select , 0 , 1) || (char ) sql__setrang((*acv).a_insert_select , 0 , 1) || (char ) sql__setrang((*dmli).d_union , 0 , 1) || (char ) sql__setrang((*dmli).d_view , 0 , 1) || (int ) (*dmli).d_cntfromtab > 1 || (char ) sql__setrang(*pseudo_resultset_select , 0 , 1) || (int ) select_rec.sel_order_n != 0 && (int ) (*acv).a_sqlmode == 0 || (int ) select_rec.sel_group_n != 0 || (int ) select_rec.sel_having_n != 0 || fetch_only_n != 0 || 1 && ((*dmli).d_tabarr[0].ospecialname[0] & 1) != 0 || 1 && ((*dmli).d_tabarr[0].ospecialname[0] & 32) != 0 || (char ) sql__setrang((*dmli).d_tabarr[0].ocomplex_view , 0 , 1) || (_ptoc_MEMCMP(unsigned char , 64, select_rec.sel_res_name , a01_il_b_identifier) == 0 ? 1 : 0) && (int ) (*acv).a_qualified_jv_upd == 0 && (int ) (*acv).a_sqlmode != 2 && ((int ) (*acv).a_comp_type != 7 && (int ) (*acv).a_comp_type != 5 || (int ) (*acv).a_init_ex_kind != 0) || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19906))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb == 57 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19908))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb == 91 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19910))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb == 61)
                {
                is_updateable = 0;
                }
            else
                {
                is_updateable = 1;
                }
            update_n = (int ) select_rec.sel_from_n;
            do
                update_n = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19918))[sql__setrang(update_n , 0 , 32767)].n_sa_level;
            while (!(update_n == 0 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19921))[sql__setrang(update_n , 0 , 32767)].n_proc == 54 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19922))[sql__setrang(update_n , 0 , 32767)].n_subproc == 6));
            }
        if (update_n != 0 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 19926)).integer.C_4.sp1r_returncode == 0)
            {
            if ((int ) (*acv).a_qualified_jv_upd == 0)
                {
                _ptoc_MOVE(unsigned char , 2, (*dmli).d_globstate, *(unsigned char (*)[2]) sql__setsub(ptoc_Var199 , (*dmli).d_globstate , ptoc_Var192 , sizeof(unsigned char [2])));
                }
            if (!(char ) sql__setrang(is_updateable , 0 , 1))
                {
                a07_b_put_error(&(*acv) , 6130 , 1);
                }
            else
                {
                if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 19938)).integer.C_4.sp1r_returncode == 0 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19939))[sql__setrang(update_n , 0 , 32767)].n_lo_level != 0)
                    {
                    ak660check_update_columns(&(*acv) , &(*dmli) , (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19942))[sql__setrang(update_n , 0 , 32767)].n_lo_level);
                    }
                }
            if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 19945)).integer.C_4.sp1r_returncode == 0)
                {
                ak660put_tablename(&(*acv) , &(*dmli));
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 19951)).integer.C_4.sp1r_returncode == 0)
            {
            reuse_n = (int ) select_rec.sel_from_n;
            do
                reuse_n = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19956))[sql__setrang(reuse_n , 0 , 32767)].n_sa_level;
            while (!(reuse_n == 0 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19959))[sql__setrang(reuse_n , 0 , 32767)].n_proc == 54 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19960))[sql__setrang(reuse_n , 0 , 32767)].n_subproc == 11));
            if ((int ) (*dmli).d_maxcounttabs > 1 || reuse_n != 0)
                {
                (*dmli).d_reuse = 1;
                }
            resdel_n = (int ) select_rec.sel_from_n;
            do
                resdel_n = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19968))[sql__setrang(resdel_n , 0 , 32767)].n_sa_level;
            while (!(resdel_n == 0 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19971))[sql__setrang(resdel_n , 0 , 32767)].n_proc == 54 && ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19972))[sql__setrang(resdel_n , 0 , 32767)].n_subproc == 20 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19973))[sql__setrang(resdel_n , 0 , 32767)].n_subproc == 21)));
            if (resdel_n != 0)
                {
                if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 19977))[sql__setrang(resdel_n , 0 , 32767)].n_subproc == 21)
                    {
                    (*(struct tak_systembuffer *) sql__nil((*acv).a_resname_addr[sql__setrang((int ) (*dmli).d_pos_result , 1 , 3) + -1] , "vak660.p" , 19979)).integer.C_52.sresname.resdecresdel = 2;
                    }
                }
            if (!(char ) sql__setrang(new_parsinfo , 0 , 1) || (char ) sql__setrang(*pseudo_resultset_select , 0 , 1))
                {
                (*dmli).d_reuse = 1;
                }
            if ((char ) sql__setrang((*dmli).d_view , 0 , 1) && ((int ) select_rec.sel_group_n != 0 || (int ) select_rec.sel_having_n != 0 || (int ) (*dmli).d_distinct != 0))
                {
                (*dmli).d_phase_cnt = 4;
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 19995)).integer.C_4.sp1r_returncode == 0 && !(char ) sql__setrang((*dmli).d_single , 0 , 1) && (!(char ) sql__setrang((*dmli).d_subquery , 0 , 1) || (char ) sql__setrang((*acv).a_from_select , 0 , 1)))
            {
            ak660keycolumn(&(*acv) , &(*dmli));
            }
        if ((int ) select_rec.sel_group_n != 0 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20004)).integer.C_4.sp1r_returncode == 0)
            {
            md_union = (char ) sql__setrang((*dmli).d_union , 0 , 1);
            (*dmli).d_union = 0;
            (*dmli).d_group = 1;
            (*dmli).d_order_or_group_cols = (tak00_ord_fields *) &(*dmli).d_group_cols;
            ak660order_by(&(*acv) , &(*dmli) , (short ) sql__setrang(select_rec.sel_group_n , -32768 , 32767));
            (*dmli).d_group = 0;
            (*dmli).d_union = (char ) sql__setrang(md_union , 0 , 1);
            }
        if ((char ) sql__setrang((*dmli).d_use_order , 0 , 1) && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20016)).integer.C_4.sp1r_returncode == 0)
            {
            (*dmli).d_order_or_group_cols = (tak00_ord_fields *) &(*dmli).d_order_cols;
            ak660order_by(&(*acv) , &(*dmli) , (short ) sql__setrang(select_rec.sel_order_n , -32768 , 32767));
            }
        h_symb = (unsigned char ) sql__setrang((*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20023))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb , 0 , 240);
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20024)).integer.C_4.sp1r_returncode == 0)
            {
            if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20027))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb == 57 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20028))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb == 91 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20029))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb == 61 || (int ) select_rec.sel_group_n != 0)
                {
                (*dmli).d_reuse = 1;
                (*dmli).d_group = 1;
                }
            t01bool(5 , "d_GROUp     " , (char ) sql__setrang((*dmli).d_group , 0 , 1));
            if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20038))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb == 57 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20039))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb == 91 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20040))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb == 61)
                {
                (*acv).a_mblock.integer.C_2.mb_type2 = 67;
                }
            a61_check_val_params(&(*acv) , &(*dmli) , (int ) select_rec.sel_output_n , (int ) select_rec.sel_into_n);
            if ((int ) (*acv).a_mblock.integer.C_2.mb_type2 == 67)
                {
                is_updateable = 0;
                }
            if (update_n != 0)
                {
                if (!(char ) sql__setrang(is_updateable , 0 , 1))
                    {
                    a07_b_put_error(&(*acv) , 6130 , 1);
                    }
                }
            else
                {
                if ((char ) sql__setrang(is_updateable , 0 , 1) && (int ) (*acv).a_is_ddl != 50)
                    {
                    if ((int ) (*acv).a_sqlmode == 0 || (int ) (*acv).a_sqlmode == 1 || (int ) (*acv).a_sqlmode == 2 && (int ) (*acv).a_init_ex_kind == 0 && !(char ) sql__setrang((*(tsp1_segment *) sql__nil(a542initial_cmd_segm(&(*acv)) , "vak660.p" , 20068)).integer.C_3.sp1c_prepare , 0 , 1))
                        {
                        switch ((int ) (*acv).a_precomp_info_byte)
                            {
                            case 44 :
                                (*acv).a_precomp_info_byte = (unsigned char)'-';
                                break;
                            case 114 :
                                (*acv).a_precomp_info_byte = (unsigned char)'s';
                                break;
                            case 46 :
                                (*acv).a_precomp_info_byte = (unsigned char)'/';
                                break;
                            case 116 :
                                (*acv).a_precomp_info_byte = (unsigned char)'u';
                                break;
                            default :
                                break;
                            }
                        update_n = 1;
                        }
                    }
                }
            stamp_found = 0;
            if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20092)).integer.C_4.sp1r_returncode == 0 && (char ) sql__setrang((*acv).a_insert_select , 0 , 1))
                {
                ptoc_Var202 = &(*acv).a_mblock;
                ptoc_Var203 = (tgg00_QualBuf *) sql__nil(ptoc_Var202->integer.C_4.mb_qual , "vak660.p" , 20095);
                ptoc_Var193 = ptoc_Var203->integer.C_2.mqual_pos;
                ptoc_Var204 = (int ) ptoc_Var203->integer.C_2.mfirst_free + -1;
                if (ptoc_Var193 <= ptoc_Var204)
                    {
                    i = ptoc_Var193;
                    do
                        {
                        if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var202->integer.C_5.mb_st , "vak660.p" , 20097))[sql__setrang(i , 1 , 32767) + -1].integer.C_1.etype == 26 && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var202->integer.C_5.mb_st , "vak660.p" , 20098))[sql__setrang(i , 1 , 32767) + -1].integer.C_1.eop == 27)
                            {
                            stamp_found = 1;
                            }
                        if (i == ptoc_Var204)
                            {
                            break;
                            }
                        i += 1;
                        }
                    while (!0);
                    }
                }
            (*sr_rec).sr_use_rowno = (char ) sql__setrang(!(char ) sql__setrang((*acv).a_rowno_allowed , 0 , 1) , 0 , 1);
            (*dmli).d_reclen = (short ) sql__setrang((*dmli).d_inoutpos , -32768 , 32767);
            if ((int ) select_rec.sel_group_n == 0)
                {
                (*dmli).d_group = 0;
                }
            else
                {
                if ((char ) sql__setrang((*sr_rec).sr_use_rowno , 0 , 1))
                    {
                    (*acv).a_mblock.integer.C_2.mb_type2 = 67;
                    }
                }
            t01bool(5 , "d_GROUp     " , (char ) sql__setrang((*dmli).d_group , 0 , 1));
            t01int4(5 , "d_reclen    " , (int ) (*dmli).d_reclen);
            t01int4(5 , "d_inoutpos  " , (int ) (*dmli).d_inoutpos);
            }
        (*dmli).d_joins.jrc_cnt = 0;
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20122)).integer.C_4.sp1r_returncode == 0 && update_n != 0)
            {
            a61_update_column(&(*acv) , &(*dmli) , &i , (int ) select_rec.sel_output_n);
            }
        set_change_found = 0;
        if ((int ) (*acv).a_mblock.integer.C_2.mb_type2 == 67 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20132)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var205 = &(*acv).a_mblock;
            ptoc_Var206 = (tgg00_QualBuf *) sql__nil(ptoc_Var205->integer.C_4.mb_qual , "vak660.p" , 20134);
            i = (int ) ptoc_Var206->integer.C_2.mqual_pos;
            while (i < (int ) ptoc_Var206->integer.C_2.mqual_pos + (int ) ptoc_Var206->integer.C_2.mqual_cnt + -1)
                if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var205->integer.C_5.mb_st , "vak660.p" , 20138))[sql__setrang(i , 1 , 32767) + -1].integer.C_1.etype == 45 && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var205->integer.C_5.mb_st , "vak660.p" , 20139))[sql__setrang(i + 1 , 1 , 32767) + -1].integer.C_1.etype == 55 && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var205->integer.C_5.mb_st , "vak660.p" , 20140))[sql__setrang(i + 1 , 1 , 32767) + -1].integer.C_4.eop_build_in == 9)
                    {
                    set_change_found = 1;
                    i = (int ) ptoc_Var206->integer.C_2.mqual_pos + (int ) ptoc_Var206->integer.C_2.mqual_cnt;
                    }
                else
                    {
                    i = sql__succ(i , -2147483647 , 2147483647 , "vak660.p" , 20147);
                    }
            }
        if ((char ) sql__setrang((*dmli).d_use_order , 0 , 1) && (int ) (*dmli).d_distinct == 0 && (int ) (*dmli).d_maxcounttabs == 1 && (int ) select_rec.sel_group_n == 0 && (int ) select_rec.sel_having_n == 0 && !(char ) sql__setrang(set_change_found , 0 , 1) && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20159))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb == 57 && ((int ) (*dmli).d_corr != 2 && (int ) (*dmli).d_corr != 1))
            {
            ak660later_output(&(*acv));
            }
        select_rec.sel_where_done = 0;
        select_rec.sel_view_done = 0;
        (*acv).a_outer_join = 0;
        t01int4(5 , "sel_from_n  " , (int ) select_rec.sel_from_n);
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20169)).integer.C_4.sp1r_returncode == 0)
            {
            if (((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20172))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb == 61 || (int ) (*dmli).d_distinct != 0 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20175))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb == 57 || (int ) select_rec.sel_having_n != 0 || (char ) sql__setrang((*dmli).d_use_order , 0 , 1) && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20180))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb == 57 && ((int ) (*dmli).d_corr == 2 || (int ) (*dmli).d_corr == 1 || (int ) select_rec.sel_group_n != 0 || (*acv).a_union_cnt > 0 || (char ) sql__setrang(set_change_found , 0 , 1) || (int ) (*dmli).d_maxcounttabs > 1) || (*acv).a_union_cnt > 0 && (int ) select_rec.sel_group_n != 0) && ((int ) (*acv).a_command_kind != 8 || !(char ) sql__setrang((*dmli).d_only_sem_check , 0 , 1)))
                {
                if ((*acv).a_union_cnt > 0 && (int ) select_rec.sel_group_n != 0)
                    {
                    (*sr_rec).sr_distinct_bytes = 0;
                    }
                if ((int ) select_rec.sel_having_n != 0 && (int ) select_rec.sel_group_n == 0 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20206))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb != 61 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20207))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb != 57)
                    {
                    (*dmli).d_rowno = 1;
                    }
                ak660three_phase(&(*acv) , &(*dmli) , &select_rec , &(*sr_rec));
                }
            else
                {
                where_n = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20215))[sql__setrang((int ) select_rec.sel_from_n , 0 , 32767)].n_sa_level;
                if ((int ) (*dmli).d_maxcounttabs > 1 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20216)).integer.C_4.sp1r_returncode == 0 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20217))[sql__setrang(where_n , 0 , 32767)].n_proc == 54 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20218))[sql__setrang(where_n , 0 , 32767)].n_subproc == 10)
                    {
                    old_atgroup = (char ) sql__setrang((*dmli).d_group , 0 , 1);
                    (*dmli).d_group = 0;
                    (*dmli).d_join = 1;
                    next_n = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20224))[sql__setrang(where_n , 0 , 32767)].n_lo_level;
                    if ((int ) (*acv).a_ex_kind == 0)
                        {
                        a54_fixedpos(&(*acv) , &(*dmli));
                        }
                    a65_search_condition(&(*acv) , &(*dmli) , &next_n);
                    if ((char ) sql__setrang((*dmli).d_outer_join , 0 , 1) && (int ) (*acv).a_allow_functions == 2)
                        {
                        (*dmli).d_arith_where = 1;
                        }
                    (*dmli).d_join = 0;
                    (*dmli).d_group = (char ) sql__setrang(old_atgroup , 0 , 1);
                    select_rec.sel_where_done = 1;
                    }
                if ((int ) (*dmli).d_maxcounttabs > (int ) (*dmli).d_cntfromtab && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20245)).integer.C_4.sp1r_returncode == 0)
                    {
                    a67_put_all_views_into(&(*acv) , &(*dmli));
                    select_rec.sel_view_done = 1;
                    }
                stored_joinarr.boolean.C_false.sbuf = (struct tak_systembuffer *) (int *) 0;
                sfe_rec.messbuf_stored = 0;
                if ((int ) (*dmli).d_maxcounttabs > 1 && ((char ) sql__setrang(select_rec.sel_view_done , 0 , 1) && (int ) (*dmli).d_phase_cnt > 1 || (int ) (*acv).a_mblock.integer.C_2.mb_type2 == 67 || (char ) sql__setrang((*dmli).d_arith_where , 0 , 1) || (char ) sql__setrang(stamp_found , 0 , 1) || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20260))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb == 57) || (int ) (*dmli).d_maxcounttabs == 1 && (int ) (*acv).a_mblock.integer.C_2.mb_type2 == 67 && ((int ) (*dmli).d_corr == 2 || (int ) (*dmli).d_corr == 1))
                    {
                    t01int4(5 , "d_maxtcountt" , (int ) (*dmli).d_maxcounttabs);
                    t01int4(5 , "sel_view_don" , sql__ord((int ) (char ) sql__setrang(select_rec.sel_view_done , 0 , 1)));
                    t01int4(5 , "d_phase_cnt " , (int ) (*dmli).d_phase_cnt);
                    t01int4(5 , "mb_type2=fkt" , sql__ord((int ) ((int ) (*acv).a_mblock.integer.C_2.mb_type2 == 67)));
                    t01bool(5 , "stamp_found " , (char ) sql__setrang(stamp_found , 0 , 1));
                    t01int4(5 , "= s_count   " , sql__ord((int ) ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20275))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb == 57)));
                    t01corr_type(5 , "d_corr      " , (unsigned char ) sql__setrang((*dmli).d_corr , 0 , 4));
                    if ((char ) sql__setrang((*dmli).d_view , 0 , 1) && (int ) (*dmli).d_phase_cnt <= 2)
                        {
                        if ((int ) (*dmli).d_maxcounttabs <= (int ) (*dmli).d_cntfromtab && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20281)).integer.C_4.sp1r_returncode == 0)
                            {
                            a67_put_all_views_into(&(*acv) , &(*dmli));
                            select_rec.sel_view_done = 1;
                            }
                        (*dmli).d_phase_cnt = 2;
                        }
                    else
                        {
                        if (!(char ) sql__setrang((*dmli).d_union , 0 , 1) && ((char ) sql__setrang(from_sel_found , 0 , 1) && (int ) (*dmli).d_corr == 0 || !(char ) sql__setrang(from_sel_found , 0 , 1)) && (int ) (*acv).a_intern_select_cnt == (int ) (*acv).a_max_intern_select && !(char ) sql__setrang((*acv).a_insert_select , 0 , 1))
                            {
                            select_rec.sel_last_pars_part = 1;
                            }
                        (*dmli).d_join = 1;
                        (*dmli).d_one_join_phase = (char ) sql__setrang((int ) select_rec.sel_group_n == 0 && (int ) (*dmli).d_corr == 0 , 0 , 1);
                        if ((char ) sql__setrang((*dmli).d_one_join_phase , 0 , 1) && ((char ) sql__setrang(stamp_found , 0 , 1) || ((char ) sql__setrang(select_rec.sel_view_done , 0 , 1) && (int ) (*dmli).d_phase_cnt > 1 || (int ) (*acv).a_mblock.integer.C_2.mb_type2 == 67) || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20312))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb == 57))
                            {
                            ptoc_Var207 = &(*acv).a_mblock;
                            ptoc_Var208 = (tgg00_QualBuf *) sql__nil(ptoc_Var207->integer.C_4.mb_qual , "vak660.p" , 20315);
                            jx = (int ) ptoc_Var208->integer.C_2.mqual_pos + 1;
                            while ((char ) sql__setrang((*dmli).d_one_join_phase , 0 , 1) && jx < (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var207->integer.C_5.mb_st , "vak660.p" , 20319))[sql__setrang((int ) ptoc_Var208->integer.C_2.mqual_pos , 1 , 32767) + -1].integer.C_1.epos)
                                {
                                ptoc_Var209 = &(*(tgg00_StackList *) sql__nil(ptoc_Var207->integer.C_5.mb_st , "vak660.p" , 20320))[sql__setrang(jx , 1 , 32767) + -1];
                                (*dmli).d_one_join_phase = (char ) sql__setrang(((int ) ptoc_Var209->integer.C_1.etype != 45 || (ptoc_Var204 = (int ) ptoc_Var209->integer.C_2.eop_func) != 2 && ptoc_Var204 != 4 && ptoc_Var204 != 5 && ptoc_Var204 != 1 && ptoc_Var204 != 6 && ptoc_Var204 != 7 && ptoc_Var204 != 8 && ptoc_Var204 != 11 && ptoc_Var204 != 12 && ptoc_Var204 != 13 && ptoc_Var204 != 14 && ptoc_Var204 != 15) && (int ) ptoc_Var209->integer.C_1.etype != 46 , 0 , 1);
                                jx = sql__succ(jx , -2147483647 , 2147483647 , "vak660.p" , 20349);
                                }
                            while ((char ) sql__setrang((*dmli).d_one_join_phase , 0 , 1) && jx < (int ) ptoc_Var208->integer.C_2.mfirst_free)
                                {
                                ptoc_Var210 = &(*(tgg00_StackList *) sql__nil(ptoc_Var207->integer.C_5.mb_st , "vak660.p" , 20355))[sql__setrang(jx , 1 , 32767) + -1];
                                (*dmli).d_one_join_phase = (char ) sql__setrang((int ) ptoc_Var210->integer.C_1.etype != 21 , 0 , 1);
                                jx = sql__succ(jx , -2147483647 , 2147483647 , "vak660.p" , 20359);
                                }
                            }
                        old_one_join_phase = (char ) sql__setrang((*dmli).d_one_join_phase , 0 , 1);
                        t01int4(5 , "d_one_join_p" , sql__ord((int ) (char ) sql__setrang((*dmli).d_one_join_phase , 0 , 1)));
                        t01int4(5 , "d_inoutpos a" , (int ) (*dmli).d_inoutpos);
                        t01int4(5 , "d_reclen    " , (int ) (*dmli).d_reclen);
                        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20370)).integer.C_4.sp1r_returncode == 0 && ((char ) sql__setrang((*dmli).d_group , 0 , 1) && (int ) (*dmli).d_group_cols.ocntord > 0 || ((char ) sql__setrang(select_rec.sel_view_done , 0 , 1) || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20375))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb == 61) && (char ) sql__setrang((*dmli).d_use_order , 0 , 1) || (char ) sql__setrang((*dmli).d_use_order , 0 , 1)))
                            {
                            a67_update_atinoutpos(&(*acv) , &(*dmli) , 1);
                            }
                        t01int4(5 , "d_inoutpos b" , (int ) (*dmli).d_inoutpos);
                        t01int4(5 , "d_reclen    " , (int ) (*dmli).d_reclen);
                        old_hbuf = (struct tak_systembuffer *) (int *) 0;
                        if ((char ) sql__setrang(old_one_join_phase , 0 , 1) && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20386)).integer.C_4.sp1r_returncode == 0)
                            {
                            old_sel_res_tree = select_rec.sel_res_tree;
                            ptoc_Var211 = &hbuf_ke;
                            hbuf_ke = a01sysnullkey;
                            i = 1;
                            do
                                {
                                ptoc_Var211->integer.C_3.sauthid[sql__setrang(i , 1 , 8) + -1] = (unsigned char)'\377';
                                if (i == 8)
                                    {
                                    break;
                                    }
                                i += 1;
                                }
                            while (!0);
                            _ptoc_ASSIGN(unsigned char , 2, ptoc_Var211->integer.C_2.sentrytyp, "\0\301")
                            ptoc_Var211->integer.C_2.slinkage[1] = (unsigned char)'\003';
                            aux_data_len = (*acv).a_mblock.integer.C_2.mb_data_len;
                            (*acv).a_mblock.integer.C_2.mb_data_len = 8;
                            a10mblock_into_cache(&(*acv) , &hbuf_ke , &(*acv).a_mblock , 0 , &old_hbuf , &b_err);
                            (*acv).a_mblock.integer.C_2.mb_data_len = aux_data_len;
                            if ((int ) b_err != 0)
                                {
                                a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                                }
                            if ((int ) (*dmli).d_joins.jrc_cnt > 0)
                                {
                                a10new(&(*acv) , (int ) (*dmli).d_joins.jrc_cnt * sizeof(struct ptoc_Type111 ) , &stored_joinarr.boolean.C_false.sbuf);
                                if (stored_joinarr.boolean.C_false.sbuf != (struct tak_systembuffer *) (int *) 0)
                                    {
                                    g10mv6("VAK660" , 9 , sizeof(tak_joinarr ) , (int ) (*dmli).d_joins.jrc_cnt * sizeof(struct ptoc_Type111 ) , (*dmli).d_joins.jrc_joinarr , 1 , sql__nil(stored_joinarr.boolean.C_true.joinarr_ptr , "vak660.p" , 20422) , 1 , (int ) (*dmli).d_joins.jrc_cnt * sizeof(struct ptoc_Type111 ) , &b_err);
                                    if ((int ) b_err != 0)
                                        {
                                        a10dispose(&(*acv) , &stored_joinarr.boolean.C_false.sbuf);
                                        a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                                        }
                                    }
                                }
                            }
                        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20437)).integer.C_4.sp1r_returncode == 0)
                            {
                            ak660more_phase(&(*acv) , &(*dmli) , &select_rec , &(*sr_rec) , &sfe_rec , (char ) sql__setrang(!(char ) sql__setrang((*dmli).d_one_join_phase , 0 , 1) , 0 , 1));
                            ix = (int ) (short ) sql__succ((int ) (*(tgg00_QualBuf *) sql__nil(sfe_rec.hqbp , "vak660.p" , 20442)).integer.C_2.mqual_pos , -32768 , 32767 , "vak660.p" , 20442);
                            qstart = (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(sfe_rec.hmbp , "vak660.p" , 20443)).integer.C_5.mb_st , "vak660.p" , 20443))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(sfe_rec.hqbp , "vak660.p" , 20443)).integer.C_2.mqual_pos , 1 , 32767) + -1].integer.C_1.epos;
                            while (ix < qstart && (char ) sql__setrang((*dmli).d_one_join_phase , 0 , 1))
                                if ((int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(sfe_rec.hmbp , "vak660.p" , 20447)).integer.C_5.mb_st , "vak660.p" , 20447))[sql__setrang(ix , 1 , 32767) + -1].integer.C_1.etype == 2 && ((ptoc_Var204 = (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(sfe_rec.hmbp , "vak660.p" , 20448)).integer.C_5.mb_st , "vak660.p" , 20448))[sql__setrang(ix , 1 , 32767) + -1].integer.C_1.eop) == 0 || ptoc_Var204 == 19 || ptoc_Var204 == 28 || ptoc_Var204 == 20 || ptoc_Var204 == 29) && (int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(sfe_rec.hmbp , "vak660.p" , 20450)).integer.C_5.mb_st , "vak660.p" , 20450))[sql__setrang(ix + 1 , 1 , 32767) + -1].integer.C_1.etype == 28)
                                    {
                                    if ((int ) (*(tgg00_StackList *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(sfe_rec.hmbp , "vak660.p" , 20453)).integer.C_5.mb_st , "vak660.p" , 20453))[sql__setrang(ix + 1 , 1 , 32767) + -1].integer.C_3.eop_out != 4)
                                        {
                                        ix = ix + 2;
                                        }
                                    else
                                        {
                                        ix = ix + 3;
                                        }
                                    }
                                else
                                    {
                                    (*dmli).d_one_join_phase = 0;
                                    }
                            }
                        if ((int ) (char ) sql__setrang(old_one_join_phase , 0 , 1) != (int ) (char ) sql__setrang((*dmli).d_one_join_phase , 0 , 1) && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20468)).integer.C_4.sp1r_returncode == 0)
                            {
                            if ((char ) sql__setrang(sfe_rec.messbuf_stored , 0 , 1))
                                {
                                a10del_sysinfo(&(*acv) , &sfe_rec.ke , &b_err);
                                }
                            a10get_sysinfo(&(*acv) , &hbuf_ke , 1 , &old_hbuf , &b_err);
                            sfe_rec.hmbp = (struct tgg00_MessBlock *) &(*(struct tak_systembuffer *) sql__nil(old_hbuf , "vak660.p" , 20479)).integer.C_62.smessblock.mbr_mess_block;
                            sfe_rec.hqbp = (*(struct tgg00_MessBlock *) sql__nil(sfe_rec.hmbp , "vak660.p" , 20480)).integer.C_4.mb_qual;
                            aux_data_len = (*acv).a_mblock.integer.C_2.mb_data_len;
                            (*acv).a_mblock.integer.C_2.mb_data_len = 0;
                            a06cpy_mblock(&(*acv) , sql__nil(sfe_rec.hmbp , "vak660.p" , 20483) , &(*acv).a_mblock , &b_err);
                            (*acv).a_mblock.integer.C_2.mb_data_len = aux_data_len;
                            if ((int ) b_err != 0)
                                {
                                a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                                }
                            m_return = (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20489)).integer.C_4.sp1r_returncode;
                            (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20490)).integer.C_4.sp1r_returncode = 0;
                            a10del_sysinfo(&(*acv) , &hbuf_ke , &b_err);
                            if (m_return != 0)
                                {
                                (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20494)).integer.C_4.sp1r_returncode = (short ) sql__setrang((short ) m_return , -32768 , 32767);
                                }
                            ptoc_Var212 = &sfe_rec;
                            (*dmli).d_inoutpos = (short ) sql__setrang((short ) ptoc_Var212->hinoutpos , -32768 , 32767);
                            (*dmli).d_keylen = (short ) sql__setrang((short ) ptoc_Var212->hkeylen , -32768 , 32767);
                            (*dmli).d_reclen = (short ) sql__setrang((short ) ptoc_Var212->hreclen , -32768 , 32767);
                            (*dmli).d_single = (char ) sql__setrang(ptoc_Var212->hd_single , 0 , 1);
                            (*dmli).d_rowno = (short ) sql__setrang((short ) ptoc_Var212->hd_rowno , -32768 , 32767);
                            (*dmli).d_change.cr_colcount = (short ) sql__setrang((short ) ptoc_Var212->hd_colcount , -32768 , 32767);
                            (*dmli).d_distinct = (unsigned char ) sql__setrang(ptoc_Var212->hd_distinct , 0 , 2);
                            (*dmli).d_union = (char ) sql__setrang(ptoc_Var212->hdm_union , 0 , 1);
                            (*sr_rec).sr_use_rowno = (char ) sql__setrang(ptoc_Var212->h_use_rowno , 0 , 1);
                            if ((int ) (*dmli).d_corr != 0)
                                {
                                (*dmli).d_lowpars = (unsigned char ) sql__setrang((unsigned char ) ((int ) (*dmli).d_lowpars + 1) , 0 , 255);
                                }
                            select_rec.sel_res_tree = old_sel_res_tree;
                            (*dmli).d_join = 1;
                            if (stored_joinarr.boolean.C_false.sbuf != (struct tak_systembuffer *) (int *) 0)
                                {
                                g10mv6("VAK660" , 10 , (int ) (*dmli).d_joins.jrc_cnt * sizeof(struct ptoc_Type111 ) , sizeof(tak_joinarr ) , sql__nil(stored_joinarr.boolean.C_true.joinarr_ptr , "vak660.p" , 20521) , 1 , (*dmli).d_joins.jrc_joinarr , 1 , (int ) (*dmli).d_joins.jrc_cnt * sizeof(struct ptoc_Type111 ) , &b_err);
                                if ((int ) b_err != 0)
                                    {
                                    a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                                    }
                                }
                            if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20530)).integer.C_4.sp1r_returncode == 0)
                                {
                                ak660more_phase(&(*acv) , &(*dmli) , &select_rec , &(*sr_rec) , &sfe_rec , (char ) sql__setrang(!(char ) sql__setrang((*dmli).d_one_join_phase , 0 , 1) , 0 , 1));
                                }
                            }
                        if (old_hbuf != (struct tak_systembuffer *) (int *) 0 && ((int ) (char ) sql__setrang(old_one_join_phase , 0 , 1) == (int ) (char ) sql__setrang((*dmli).d_one_join_phase , 0 , 1) || (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20539)).integer.C_4.sp1r_returncode != 0))
                            {
                            m_return = (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20543)).integer.C_4.sp1r_returncode;
                            (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20544)).integer.C_4.sp1r_returncode = 0;
                            a10del_sysinfo(&(*acv) , &hbuf_ke , &b_err);
                            if (m_return != 0)
                                {
                                (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20548)).integer.C_4.sp1r_returncode = (short ) sql__setrang((short ) m_return , -32768 , 32767);
                                }
                            }
                        if (stored_joinarr.boolean.C_false.sbuf != (struct tak_systembuffer *) (int *) 0)
                            {
                            a10dispose(&(*acv) , &stored_joinarr.boolean.C_false.sbuf);
                            }
                        if ((char ) sql__setrang((*dmli).d_one_join_phase , 0 , 1))
                            {
                            ak660one_phase(&(*acv) , &(*dmli) , &select_rec , &(*sr_rec) , &sfe_rec);
                            }
                        else
                            {
                            ak660two_phase(&(*acv) , &(*dmli) , &select_rec , &(*sr_rec) , &sfe_rec , 1);
                            }
                        if ((char ) sql__setrang((*acv).a_intern_explain , 0 , 1) && !(char ) sql__setrang((*acv).a_parsing_again , 0 , 1))
                            {
                            (*dmli).d_single = 0;
                            }
                        t01int4(5 , "d_inoutpos f" , (int ) (*dmli).d_inoutpos);
                        t01int4(5 , "d_reclen    " , (int ) (*dmli).d_reclen);
                        t01int4(5 , "d_single 660" , sql__ord((int ) (char ) sql__setrang((*dmli).d_single , 0 , 1)));
                        if (!(char ) sql__setrang((*dmli).d_only_sem_check , 0 , 1) && (*acv).a_union_cnt <= 1)
                            {
                            a67_sel3_second_part(&(*acv) , &(*dmli) , &select_rec , &(*sr_rec));
                            }
                        }
                    }
                else
                    {
                    if ((char ) sql__setrang((*acv).a_rowno_allowed , 0 , 1) && ((ptoc_Var204 = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20589))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb) != 57 && ptoc_Var204 != 91 && ptoc_Var204 != 61) && (int ) select_rec.sel_group_n != 0)
                        {
                        select_rec.sel_new_atdistinct = 1;
                        }
                    else
                        {
                        select_rec.sel_new_atdistinct = 0;
                        }
                    if (!(char ) sql__setrang((*dmli).d_subquery , 0 , 1))
                        {
                        if ((int ) (*dmli).d_distinct == 0 && !(char ) sql__setrang((*dmli).d_use_order , 0 , 1) && (int ) select_rec.sel_new_atdistinct == 0 && (int ) (*dmli).d_maxcounttabs <= 1 && (int ) (*acv).a_mblock.integer.C_2.mb_type2 != 67)
                            {
                            (*sr_rec).sr_use_rowno = 0;
                            }
                        if (!(char ) sql__setrang((*dmli).d_union , 0 , 1) && ((char ) sql__setrang(from_sel_found , 0 , 1) && (int ) (*dmli).d_corr == 0 || !(char ) sql__setrang(from_sel_found , 0 , 1)) && (int ) (*acv).a_intern_select_cnt == (int ) (*acv).a_max_intern_select && !(char ) sql__setrang((*acv).a_insert_select , 0 , 1))
                            {
                            select_rec.sel_last_pars_part = 1;
                            }
                        }
                    a67_s_sel_second_part(&(*acv) , &(*dmli) , &select_rec , &(*sr_rec));
                    }
                }
            }
        else
            {
            a67_sel3_second_part(&(*acv) , &(*dmli) , &select_rec , &(*sr_rec));
            }
        if ((int ) (*acv).a_max_intern_select > 0 && (int ) (*acv).a_intern_select_cnt == (int ) (*acv).a_max_intern_select)
            {
            ptoc_Var204 = (*dmli).d_cntfromtab;
            if (1 <= ptoc_Var204)
                {
                i = 1;
                do
                    {
                    ptoc_Var213 = &(*dmli).d_tabarr[sql__setrang(i , 1 , 64) + -1];
                    if ((int ) ptoc_Var213->otreeid.integer.C_4.fileTfn_gg00 == 26 && (int ) ptoc_Var213->otreeid.integer.C_8.fileTfnTemp_gg00 == 36)
                        {
                        a663get_result_records(&(*acv) , 1 , &ptoc_Var213->otreeid.integer.C_8.fileTempCnt_gg00 , &p_arr , &f_ok);
                        if ((char ) sql__setrang(f_ok , 0 , 1))
                            {
                            a10del_sysinfo(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(p_arr.pbasep , "vak660.p" , 20642)).integer.C_50.syskey , &b_err);
                            }
                        }
                    if (i == ptoc_Var204)
                        {
                        break;
                        }
                    i += 1;
                    }
                while (!0);
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20650)).integer.C_4.sp1r_returncode == 0)
            {
            if (!(char ) sql__setrang((*acv).a_insert_select , 0 , 1) || (char ) sql__setrang((*dmli).d_subquery , 0 , 1) || (char ) sql__setrang((*acv).a_from_select , 0 , 1))
                {
                t01int4(5 , "I hate it : " , 1);
                t01bool(5 , "a_insert_sel" , (char ) sql__setrang((*acv).a_insert_select , 0 , 1));
                t01int4(5 , "a_intern_sel" , (int ) (*acv).a_intern_select_cnt);
                t01int4(5 , "a_max_intern" , (int ) (*acv).a_max_intern_select);
                t01int4(5 , "a_union_cnt " , (*acv).a_union_cnt);
                t01bool(5 , "d_subquery  " , (char ) sql__setrang((*dmli).d_subquery , 0 , 1));
                t01bool(5 , "a_from_selec" , (char ) sql__setrang((*acv).a_from_select , 0 , 1));
                a10rel_sysinfo((*dmli).d_sparr.pbasep);
                }
            }
        (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 20669))[sql__setrang((int ) select_rec.sel_output_n , 0 , 32767)].n_symb = (unsigned char ) sql__setrang(h_symb , 0 , 240);
        }
    if ((char ) sql__setrang(delete_resinfo , 0 , 1))
        {
        op_n = (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20676)).integer.C_4.sp1r_returncode;
        (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20677)).integer.C_4.sp1r_returncode = 0;
        a10del_sysinfo(&(*acv) , &(*(struct tak_systembuffer *) sql__nil((*acv).a_resname_addr[sql__setrang((int ) (*dmli).d_pos_result , 1 , 3) + -1] , "vak660.p" , 20678)).integer.C_50.syskey , &b_err);
        (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 20679)).integer.C_4.sp1r_returncode = (short ) sql__setrang((short ) op_n , -32768 , 32767);
        }
    cccprint("<a660_query_exec");
    }

static
  tak_one_table_specialname ptoc_Var214 = 
  {
  (unsigned char)'\0'
  };
static
  tak_table_attribute_set ptoc_Var215 = 
  {
  (unsigned char)'\0'
  };
static
  tak_columnset ptoc_Var216 = 
  {
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tak_columnset ptoc_Var217 = 
  {
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tak_columnset ptoc_Var218 = 
  {
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var219[1] = 
  {
  (unsigned char)'\004'
  };
static
  unsigned char ptoc_Var220[1] = 
  {
  (unsigned char)'\b'
  };
static
  unsigned char ptoc_Var221[1] = 
  {
  (unsigned char)'\001'
  };
static
  tak_columnset ptoc_Var222 = 
  {
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tak_columnset ptoc_Var223 = 
  {
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tak_table_attribute_set ptoc_Var224 = 
  {
  (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var225[1] = 
  {
  (unsigned char)'\002'
  };
static
  unsigned char ptoc_Var226[1] = 
  {
  (unsigned char)'@'
  };
static
  unsigned char ptoc_Var227[1] = 
  {
  (unsigned char)' '
  };
static
  unsigned char ptoc_Var228[1] = 
  {
  (unsigned char)'\003'
  };
static
  unsigned char ptoc_Var229[1] = 
  {
  (unsigned char)'\001'
  };
static
  unsigned char ptoc_Var230[1] = 
  {
  (unsigned char)'\017'
  };
static
  unsigned char ptoc_Var231[1] = 
  {
  (unsigned char)'\016'
  };
static
  unsigned char ptoc_Var232[1] = 
  {
  (unsigned char)'\001'
  };
static
  unsigned char ptoc_Var233[1] = 
  {
  (unsigned char)'\001'
  };
static
  unsigned char ptoc_Var234[1] = 
  {
  (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var235[1] = 
  {
  (unsigned char)'\016'
  };
static
  unsigned char ptoc_Var236[1] = 
  {
  (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var237[1] = 
  {
  (unsigned char)'\017'
  };
static
  unsigned char ptoc_Var238[1] = 
  {
  (unsigned char)'\0'
  };
static
  tak_columnset ptoc_Var239 = 
  {
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tak_columnset ptoc_Var240 = 
  {
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tak_columnset ptoc_Var241 = 
  {
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tak_columnset ptoc_Var242 = 
  {
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tak_columnset ptoc_Var243 = 
  {
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tak_columnset ptoc_Var244 = 
  {
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', 
  (unsigned char)'\0', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var245[1] = 
  {
  (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var246[1] = 
  {
  (unsigned char)' '
  };
static
  unsigned char ptoc_Var247[1] = 
  {
  (unsigned char)'\b'
  };


int a660_search_one_table(acv, dmli, table_node, all, check_teresult, lock_spec, wanted_priv)
tak_all_command_glob *acv;
tak_dml_info *dmli;
int table_node;
char all;
char check_teresult;
tak_lockenum lock_spec;
tgg_priv_r wanted_priv;
    {
    char complicated_view;
    char f_ok;
    char found;
    char syn_found;
    tak_ddl_descriptor ddl;
    tak_directory_state dst;
    tgg_basis_error b_err;
    int i;
    int ix;
    int kw_index;
    int next_n;
    tak_sysbufferaddress p;
    tgg_surrogate synid;
    tsp_c24 err_msg;
    tak_privilege privs;
    tgg_sysinfokey sysk;
    tgg_privilege_set needed_privs;
    tak_all_command_glob *ptoc_Var248;
    tak_dml_info *ptoc_Var249;
    struct ptoc_Type172 *ptoc_Var250;
    struct ptoc_Type57 *ptoc_Var251;
    struct ptoc_Type57 *ptoc_Var252;
    unsigned char ptoc_Var253[1];
    unsigned char ptoc_Var254[1];
    unsigned char ptoc_Var255[1];
    tak_baserecord *ptoc_Var256;
    int ptoc_Var257;
    int ptoc_Var258;
    tak_resname_record *ptoc_Var259;
    tak_resname_record *ptoc_Var260;
    unsigned char ptoc_Var261[1];
    unsigned char ptoc_Var262[1];
    unsigned char ptoc_Var263[1];
    tak_resname_record *ptoc_Var264;
    tak_baserecord *ptoc_Var265;
    tak_syspointerarr *ptoc_Var266;
    struct tak_systembuffer *ptoc_Var267;
    tak_baserecord *ptoc_Var268;
    tak_privilege *ptoc_Var269;
    unsigned char ptoc_Var270[1];
    unsigned char ptoc_Var271[1];
    unsigned char ptoc_Var272[1];
    tak_columnset ptoc_Var273;
    tak_columnset ptoc_Var274;
    struct tak_systembuffer *ptoc_Var275;
    tak_viewtextrecord *ptoc_Var276;
    struct ptoc_Type115 *ptoc_Var277;
    cccprint("AK660 a660_searc");
    ptoc_Var248 = &*acv;
    ptoc_Var249 = &*dmli;
    ptoc_Var250 = &ptoc_Var249->d_tabarr[sql__setrang((int ) ptoc_Var249->d_acttabindex , 1 , 64) + -1];
    syn_found = 0;
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var250->ouser, a01_il_b_identifier)
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var250->otable, a01_il_b_identifier)
    _ptoc_MOVE(unsigned char , 1, ptoc_Var250->ospecialname, ptoc_Var214);
    _ptoc_MOVE(unsigned char , 1, ptoc_Var250->oattributes, ptoc_Var215);
    _ptoc_MOVE(unsigned char , 130, ptoc_Var250->oprivset, ptoc_Var216);
    _ptoc_MOVE(unsigned char , 130, ptoc_Var250->osetallpriv, ptoc_Var217);
    ptoc_Var250->oall_priv = 0;
    ptoc_Var250->ospecs_needed = 0;
    ptoc_Var250->oview = 0;
    ptoc_Var250->ocomplex_view = 0;
    ptoc_Var250->oviewqual = 0;
    ptoc_Var250->oviewcheck = 0;
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var250->oreference, a01_il_b_identifier)
    complicated_view = 0;
    ptoc_Var250->otab_node = (short ) sql__setrang((short ) table_node , -32768 , 32767);
    if ((int ) ptoc_Var249->d_acttabindex == 1)
        {
        ptoc_Var250->ocounttabs = (unsigned char)'\0';
        }
    _ptoc_MOVE(unsigned char , 130, ptoc_Var249->d_upd_set, ptoc_Var218);
    t01int4(5 , "table_node  " , table_node);
    ptoc_Var251 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak660.p" , 20739))[sql__setrang(table_node , 0 , 32767)];
    ptoc_Var249->d_n_pos = ptoc_Var251->n_pos;
    if ((int ) ptoc_Var251->n_symb == 43)
        {
        next_n = table_node;
        a06get_username(&(*acv) , &table_node , ptoc_Var250->ouser);
        table_node = next_n;
        next_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak660.p" , 20748))[sql__setrang(table_node , 0 , 32767)].n_sa_level;
        }
    else
        {
        next_n = table_node;
        }
    a05identifier_get(&(*acv) , next_n , sizeof(unsigned char [64]) , ptoc_Var250->otable);
    ptoc_Var252 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak660.p" , 20756))[sql__setrang(next_n , 0 , 32767)];
    if ((int ) ptoc_Var252->n_sa_level != 0 && (int ) lock_spec == 0)
        {
        next_n = (int ) ptoc_Var252->n_sa_level;
        a05identifier_get(&(*acv) , next_n , sizeof(unsigned char [64]) , ptoc_Var250->oreference);
        _ptoc_MOVE(unsigned char , 1, ptoc_Var250->ospecialname, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var253 , ptoc_Var250->ospecialname , ptoc_Var219 , sizeof(unsigned char [1])));
        i = 1;
        while (i < (int ) ptoc_Var249->d_acttabindex)
            {
            if (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var249->d_tabarr[sql__setrang(i , 1 , 64) + -1].oreference , ptoc_Var250->oreference) == 0 ? 1 : 0)
                {
                a07_b_put_error(&(*acv) , 6256 , ptoc_Var252->n_pos);
                }
            else
                {
                if (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var249->d_tabarr[sql__setrang(i , 1 , 64) + -1].otable , ptoc_Var250->oreference) == 0 ? 1 : 0)
                    {
                    a07_b_put_error(&(*acv) , 6262 , ptoc_Var252->n_pos);
                    }
                }
            i = sql__succ(i , -2147483647 , 2147483647 , "vak660.p" , 20777);
            }
        }
    t01lidentifier(5 , ptoc_Var250->ouser);
    t01lidentifier(5 , ptoc_Var250->otable);
    t01lidentifier(5 , ptoc_Var250->oreference);
    if ((int ) ptoc_Var249->d_pos_result == 1 || (int ) ptoc_Var249->d_pos_result == 2)
        {
        f_ok = (char ) sql__setrang(_ptoc_MEMCMP(unsigned char , 64, (*(struct tak_systembuffer *) sql__nil(ptoc_Var248->a_resname_addr[sql__setrang((int ) ptoc_Var249->d_pos_result , 1 , 3) + -1] , "vak660.p" , 20790)).integer.C_52.sresname.reskey_name , ptoc_Var250->otable) == 0 ? 0 : 1 , 0 , 1);
        }
    else
        {
        f_ok = 1;
        }
    found = 0;
    if (((_ptoc_MEMCMP(unsigned char , 64, ptoc_Var250->ouser , a01_il_b_identifier) == 0 ? 1 : 0) || (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var250->ouser , ptoc_Var248->a_curr_user_name) == 0 ? 1 : 0)) && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var248->a_return_segm , "vak660.p" , 20799)).integer.C_4.sp1r_returncode == 0 && ((int ) ptoc_Var248->a_sqlmode == 3 || (int ) ptoc_Var248->a_sqlmode == 2))
        {
        if ((char ) sql__setrang(f_ok , 0 , 1))
            {
            f_ok = 1;
            a663restable_get(&(*acv) , 0 , ptoc_Var250->otable , &ptoc_Var249->d_sparr , &f_ok);
            found = (char ) sql__setrang(f_ok , 0 , 1);
            if ((char ) sql__setrang(f_ok , 0 , 1))
                {
                if ((char ) sql__setrang(ptoc_Var249->d_view , 0 , 1) || (int ) wanted_priv != 0 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak660.p" , 20817))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak660.p" , 20817))[0].n_lo_level , 0 , 32767)].n_proc == 53 && !(char ) sql__setrang(ptoc_Var249->d_subquery , 0 , 1) || (int ) lock_spec != 0)
                    {
                    a07_nb_put_error(&(*acv) , 6364 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak660.p" , 20822))[sql__setrang(table_node , 0 , 32767)].n_pos , ptoc_Var250->otable);
                    }
                else
                    {
                    ptoc_Var256 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var249->d_sparr.pbasep , "vak660.p" , 20825)).integer.C_1.sbase;
                    ptoc_Var257 = ptoc_Var256->bfirstindex;
                    ptoc_Var258 = ptoc_Var256->blastindex;
                    if (ptoc_Var257 <= ptoc_Var258)
                        {
                        ix = ptoc_Var257;
                        do
                            {
                            a061td_colinfo(sql__nil(ptoc_Var256->integer.C_2.bcolumn[sql__setrang(ix , 1 , 1536) + -1] , "vak660.p" , 20827) , ix);
                            if (ix == ptoc_Var258)
                                {
                                break;
                                }
                            ix += 1;
                            }
                        while (!0);
                        }
                    if ((char ) sql__setrang(check_teresult , 0 , 1))
                        {
                        ptoc_Var259 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var249->d_sparr.presnp , "vak660.p" , 20832)).integer.C_52.sresname;
                        if ((0 || (ptoc_Var259->resstate[0] & 1) == 0) && (0 || (ptoc_Var259->resstate[0] & 64) == 0))
                            {
                            a07_nb_put_error(&(*acv) , 6352 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak660.p" , 20837))[sql__setrang(table_node , 0 , 32767)].n_pos , ptoc_Var250->otable);
                            }
                        }
                    if ((int ) ptoc_Var249->d_pos_result == 2)
                        {
                        t01int4(5 , "intern_resu1" , (int ) ptoc_Var249->d_pos_result);
                        }
                    else
                        {
                        if ((int ) ptoc_Var249->d_pos_result == 1)
                            {
                            t01int4(5 , "user_result1" , (int ) ptoc_Var249->d_pos_result);
                            }
                        else
                            {
                            t01int4(5 , "d_pos_resul1" , (int ) ptoc_Var249->d_pos_result);
                            }
                        }
                    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var248->a_return_segm , "vak660.p" , 20852)).integer.C_4.sp1r_returncode == 0)
                        {
                        if ((int ) wanted_priv == 0 && !(char ) sql__setrang(ptoc_Var249->d_single , 0 , 1))
                            {
                            if ((int ) ptoc_Var249->d_pos_result == 1 || (int ) ptoc_Var249->d_pos_result == 2)
                                {
                                ptoc_Var260 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var248->a_resname_addr[sql__setrang((int ) ptoc_Var249->d_pos_result , 1 , 3) + -1] , "vak660.p" , 20860)).integer.C_52.sresname;
                                _ptoc_MOVE(unsigned char , 1, ptoc_Var260->resstate, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var261 , ptoc_Var260->resstate , ptoc_Var220 , sizeof(unsigned char [1])));
                                }
                            }
                        ptoc_Var264 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var249->d_sparr.presnp , "vak660.p" , 20865)).integer.C_52.sresname;
                        ptoc_Var250->otreeid = ptoc_Var264->restreeid;
                        _ptoc_ASSIGN(unsigned char , 8, ptoc_Var250->ofromtableid, ptoc_Var250->otreeid.integer.C_4.fileTabId_gg00)
                        _ptoc_MOVE(unsigned char , 1, ptoc_Var250->ospecialname, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var255 , ptoc_Var250->ospecialname , ptoc_Var221 , sizeof(unsigned char [1])));
                        _ptoc_ASSIGN(unsigned char , 64, ptoc_Var250->ouser, ptoc_Var248->a_curr_user_name)
                        ptoc_Var250->oall_priv = 1;
                        _ptoc_MOVE(unsigned char , 130, ptoc_Var250->oprivset, ptoc_Var222);
                        _ptoc_MOVE(unsigned char , 130, ptoc_Var250->osetallpriv, ptoc_Var223);
                        _ptoc_MOVE(unsigned char , 1, ptoc_Var250->oattributes, ptoc_Var224);
                        ptoc_Var250->ospecs_needed = 1;
                        if ((int ) ptoc_Var249->d_acttabindex < (int ) ptoc_Var249->d_cntfromtab)
                            {
                            ptoc_Var249->d_tabarr[sql__setrang((int ) ptoc_Var249->d_acttabindex + 1 , 1 , 64) + -1].ocounttabs = (unsigned char ) sql__setrang((unsigned char ) ((int ) ptoc_Var250->ocounttabs + 1) , 0 , 255);
                            }
                        else
                            {
                            ptoc_Var249->d_maxcounttabs = (short ) sql__setrang((short ) ((int ) ptoc_Var250->ocounttabs + 1) , -32768 , 32767);
                            }
                        found = 1;
                        }
                    }
                }
            }
        }
    if (!(char ) sql__setrang(found , 0 , 1) && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var248->a_return_segm , "vak660.p" , 20891)).integer.C_4.sp1r_returncode == 0)
        {
        if ((char ) sql__setrang(all , 0 , 1))
            {
            dst = 0;
            }
        else
            {
            dst = 1;
            }
        t01lidentifier(5 , ptoc_Var250->ouser);
        t01lidentifier(5 , ptoc_Var250->otable);
        t01lidentifier(5 , ptoc_Var250->oreference);
        t01int4(5 , "dst         " , sql__ord((int ) dst));
        if (!a06_table_exist(&(*acv) , (unsigned char ) sql__setrang(dst , 0 , 1) , ptoc_Var250->ouser , ptoc_Var250->otable , &ptoc_Var249->d_sparr , (char ) sql__setrang(all , 0 , 1)))
            {
            a07_nb_put_error(&(*acv) , 6524 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak660.p" , 20908))[sql__setrang(table_node , 0 , 32767)].n_pos , ptoc_Var250->otable);
            }
        else
            {
            syn_found = (char ) sql__setrang(ptoc_Var249->d_sparr.psynfound , 0 , 1);
            if ((char ) sql__setrang(syn_found , 0 , 1))
                {
                if ((char ) sql__setrang((*acv).a_proc_compile , 0 , 1))
                    {
                    a07_nb_put_error(&(*acv) , 6150 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak660.p" , 20918))[sql__setrang(table_node , 0 , 32767)].n_pos , ptoc_Var250->otable);
                    }
                else
                    {
                    _ptoc_ASSIGN(unsigned char , 8, synid, ptoc_Var249->d_sparr.psynid)
                    }
                }
            if ((char ) sql__setrang(ptoc_Var249->d_view , 0 , 1))
                {
                if ((_ptoc_MEMCMP(unsigned char , 64, ptoc_Var250->ouser , ptoc_Var249->d_viewusername) == 0 ? 1 : 0) && (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var250->otable , ptoc_Var249->d_viewtablen) == 0 ? 1 : 0))
                    {
                    a07_nb_put_error(&(*acv) , 6524 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak660.p" , 20930))[sql__setrang(table_node , 0 , 32767)].n_pos , ptoc_Var250->otable);
                    }
                else
                    {
                    if (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var250->ouser , a01_i_temp) == 0 ? 1 : 0)
                        {
                        a07_b_put_error(&(*acv) , 6442 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak660.p" , 20935))[sql__setrang(table_node , 0 , 32767)].n_pos);
                        }
                    else
                        {
                        if ((ptoc_Var258 = (int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var249->d_sparr.pbasep , "vak660.p" , 20937)).integer.C_1.sbase.btablekind) == 11 || ptoc_Var258 == 17)
                            {
                            ptoc_Var249->d_phase_cnt = 4;
                            }
                        }
                    }
                }
            else
                {
                ptoc_Var265 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var249->d_sparr.pbasep , "vak660.p" , 20946)).integer.C_1.sbase;
                if (((ptoc_Var258 = (int ) ptoc_Var265->btablekind) == 7 || ptoc_Var258 == 10) && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak660.p" , 20949))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak660.p" , 20949))[0].n_lo_level , 0 , 32767)].n_proc == 53 && !(char ) sql__setrang(ptoc_Var249->d_subquery , 0 , 1))
                    {
                    a07_b_put_error(&(*acv) , 6340 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak660.p" , 20953))[sql__setrang(table_node , 0 , 32767)].n_pos);
                    }
                if ((int ) wanted_priv != 0 && (int ) ptoc_Var265->btablekind == 7 && (int ) ptoc_Var248->a_sqlmode != 3)
                    {
                    a07_b_put_error(&(*acv) , 6350 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak660.p" , 20960))[sql__setrang(table_node , 0 , 32767)].n_pos);
                    }
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var248->a_return_segm , "vak660.p" , 20967)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var266 = &ptoc_Var249->d_sparr;
            ptoc_Var267 = (struct tak_systembuffer *) sql__nil(ptoc_Var266->pbasep , "vak660.p" , 20969);
            ptoc_Var268 = &ptoc_Var267->integer.C_1.sbase;
            if ((char ) sql__setrang(syn_found , 0 , 1))
                {
                _ptoc_MOVE(unsigned char , 1, ptoc_Var250->ospecialname, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var253 , ptoc_Var250->ospecialname , ptoc_Var225 , sizeof(unsigned char [1])));
                ak660syn_owner(&(*acv) , synid , ptoc_Var250->ouser);
                }
            if ((_ptoc_MEMCMP(unsigned char , 2, ptoc_Var268->bsegmentid , "\0\001") == 0 ? 1 : 0) && ((ptoc_Var258 = (int ) ptoc_Var268->btablekind) == 1 || ptoc_Var258 == 2 || ptoc_Var258 == 5))
                {
                _ptoc_MOVE(unsigned char , 1, ptoc_Var250->ospecialname, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var255 , ptoc_Var250->ospecialname , ptoc_Var226 , sizeof(unsigned char [1])));
                }
            else
                {
                if ((int ) ptoc_Var268->btablekind == 15)
                    {
                    _ptoc_MOVE(unsigned char , 1, ptoc_Var250->ospecialname, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var253 , ptoc_Var250->ospecialname , ptoc_Var227 , sizeof(unsigned char [1])));
                    }
                }
            _ptoc_ASSIGN(unsigned char , 1, ptoc_Var250->oattributes, ptoc_Var268->battributes)
            _ptoc_ASSIGN(unsigned char , 8, ptoc_Var250->ofromtableid, ptoc_Var267->integer.C_50.syskey.integer.C_2.stableid)
            ptoc_Var250->otreeid = ptoc_Var268->btreeid;
            ptoc_Var250->ospecs_needed = 1;
            if ((int ) ptoc_Var249->d_acttabindex < (int ) ptoc_Var249->d_cntfromtab)
                {
                ptoc_Var249->d_tabarr[sql__setrang((int ) ptoc_Var249->d_acttabindex + 1 , 1 , 64) + -1].ocounttabs = (unsigned char ) sql__setrang((unsigned char ) ((int ) ptoc_Var250->ocounttabs + (int ) ptoc_Var268->bv_tabcount) , 0 , 255);
                }
            else
                {
                ptoc_Var249->d_maxcounttabs = (short ) sql__setrang((short ) ((int ) ptoc_Var250->ocounttabs + (int ) ptoc_Var268->bv_tabcount) , -32768 , 32767);
                }
            if ((int ) ptoc_Var248->a_intern_select_cnt < (int ) ptoc_Var248->a_max_intern_select && (int ) ptoc_Var248->a_recursive_state == 0)
                {
                sql__pcnst(privs.integer.C_1.priv_all_set , 0 , 7 , 0 , 1 , (int ) wanted_priv);
                }
            else
                {
                switch ((int ) lock_spec)
                    {
                    case 0 :
                        if ((int ) wanted_priv == 1)
                            {
                            _ptoc_MOVE(unsigned char , 1, needed_privs, ptoc_Var228);
                            }
                        else
                            {
                            sql__pcnst(needed_privs , 0 , 7 , 0 , 1 , (int ) wanted_priv);
                            }
                        break;
                    case 1 :
                        _ptoc_MOVE(unsigned char , 1, needed_privs, ptoc_Var229);
                        break;
                    case 2 :
                        if ((int ) ptoc_Var248->a_sqlmode == 2)
                            {
                            _ptoc_MOVE(unsigned char , 1, needed_privs, ptoc_Var230);
                            }
                        else
                            {
                            _ptoc_MOVE(unsigned char , 1, needed_privs, ptoc_Var231);
                            }
                        break;
                    case 3 :
                        sql__pcnst(needed_privs , 0 , 7 , 0 , 1 , (int ) wanted_priv);
                        break;
                    case 4 :
                        _ptoc_MOVE(unsigned char , 1, needed_privs, ptoc_Var232);
                        break;
                    default :
                        _ptoc_MOVE(unsigned char , 1, needed_privs, ptoc_Var233);
                        break;
                    }
                a06get_priv(&(*acv) , &ptoc_Var266->pbasep , needed_privs , &privs);
                }
            ptoc_Var269 = &privs;
            if (sql__nre(sizeof(unsigned char [1]) , ptoc_Var269->integer.C_1.priv_all_set , ptoc_Var234))
                {
                _ptoc_ASSIGN(unsigned char , 130, ptoc_Var250->osetallpriv, a01fullset)
                }
            else
                {
                sql__psetadd(ptoc_Var250->osetallpriv , ptoc_Var269->integer.C_1.priv_sel_set , ptoc_Var269->integer.C_1.priv_upd_set , sizeof(tak_columnset ));
                }
            if ((int ) wanted_priv >= 0 && (int ) wanted_priv <= 7 && (ptoc_Var269->integer.C_1.priv_all_set[(int ) wanted_priv >> 3] & 1 << ((int ) wanted_priv & 7)) != 0 || (int ) lock_spec == 2 && sql__nre(sizeof(unsigned char [1]) , sql__psetm(ptoc_Var270 , ptoc_Var235 , ptoc_Var269->integer.C_1.priv_all_set , sizeof(unsigned char [1])) , ptoc_Var236) || (int ) lock_spec == 2 && (int ) ptoc_Var248->a_sqlmode == 2 && sql__nre(sizeof(unsigned char [1]) , sql__psetm(ptoc_Var272 , ptoc_Var237 , ptoc_Var269->integer.C_1.priv_all_set , sizeof(unsigned char [1])) , ptoc_Var238))
                {
                if ((int ) ptoc_Var248->a_sqlmode == 0 && (0 || (ptoc_Var269->integer.C_1.priv_all_set[0] & 1) == 0) && ((int ) wanted_priv == 1 || (int ) wanted_priv == 3))
                    {
                    ptoc_Var250->oall_priv = 0;
                    _ptoc_ASSIGN(unsigned char , 130, ptoc_Var250->oprivset, ptoc_Var269->integer.C_1.priv_sel_set)
                    _ptoc_ASSIGN(unsigned char , 130, ptoc_Var249->d_upd_set, a01fullset)
                    }
                else
                    {
                    ptoc_Var250->oall_priv = 1;
                    }
                }
            else
                {
                if (sql__erq(sizeof(tak_columnset ) , ptoc_Var269->integer.C_1.priv_sel_set , ptoc_Var239) && (int ) wanted_priv == 0 || sql__erq(sizeof(tak_columnset ) , ptoc_Var269->integer.C_1.priv_upd_set , ptoc_Var240) && (int ) wanted_priv == 1 || (int ) wanted_priv == 2 || (int ) wanted_priv == 3 || sql__erq(sizeof(tak_columnset ) , sql__psetadd(ptoc_Var273 , ptoc_Var269->integer.C_1.priv_upd_set , ptoc_Var269->integer.C_1.priv_sel_set , sizeof(tak_columnset )) , ptoc_Var241) && (int ) lock_spec == 2)
                    {
                    if (sql__erq(sizeof(tak_columnset ) , ptoc_Var250->osetallpriv , ptoc_Var242) && sql__erq(sizeof(tak_columnset ) , ptoc_Var269->integer.C_1.priv_upd_set , ptoc_Var243) && sql__erq(sizeof(tak_columnset ) , ptoc_Var269->integer.C_1.priv_sel_set , ptoc_Var244))
                        {
                        if ((int ) ptoc_Var248->a_is_ddl != 0 && (int ) ptoc_Var248->a_role_info != 1)
                            {
                            ddl = (unsigned char ) sql__setrang(ptoc_Var248->a_is_ddl , 0 , 72);
                            ptoc_Var248->a_is_ddl = 0;
                            a06get_priv(&(*acv) , &ptoc_Var266->pbasep , needed_privs , &privs);
                            ptoc_Var248->a_is_ddl = (unsigned char ) sql__setrang(ddl , 0 , 72);
                            }
                        if ((int ) ptoc_Var248->a_is_ddl != 0 && (sql__nre(sizeof(unsigned char [1]) , ptoc_Var269->integer.C_1.priv_all_set , ptoc_Var245) || 1 && (ptoc_Var269->integer.C_1.priv_col_exist[0] & 1) != 0))
                            {
                            _ptoc_ASSIGN(unsigned char , 24, err_msg, "ROLE DEFINED PRIVILEGE  ")
                            a07_const_b_put_error(&(*acv) , 6350 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak660.p" , 21099))[sql__setrang(table_node , 0 , 32767)].n_pos , (tsp00_MoveObj *) err_msg , sizeof(unsigned char [24]));
                            }
                        else
                            {
                            a07_nb_put_error(&(*acv) , 6524 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak660.p" , 21104))[sql__setrang(table_node , 0 , 32767)].n_pos , ptoc_Var250->otable);
                            }
                        }
                    else
                        {
                        if (1 && (needed_privs[0] & 4) != 0)
                            {
                            kw_index = 218;
                            }
                        else
                            {
                            if (1 && (needed_privs[0] & 8) != 0)
                                {
                                kw_index = 124;
                                }
                            else
                                {
                                if (1 && (needed_privs[0] & 2) != 0)
                                    {
                                    kw_index = 498;
                                    }
                                else
                                    {
                                    if (1 && (needed_privs[0] & 1) != 0)
                                        {
                                        kw_index = 411;
                                        }
                                    else
                                        {
                                        kw_index = 0;
                                        }
                                    }
                                }
                            }
                        a07_kw_put_error(&(*acv) , 6350 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak660.p" , 21132))[sql__setrang(table_node , 0 , 32767)].n_pos , kw_index);
                        }
                    }
                else
                    {
                    ptoc_Var250->oall_priv = 0;
                    if ((int ) wanted_priv == 0)
                        {
                        _ptoc_ASSIGN(unsigned char , 130, ptoc_Var250->oprivset, ptoc_Var269->integer.C_1.priv_sel_set)
                        }
                    else
                        {
                        if (1 && (ptoc_Var269->integer.C_1.priv_all_set[0] & 1) != 0)
                            {
                            _ptoc_ASSIGN(unsigned char , 130, ptoc_Var250->oprivset, a01fullset)
                            }
                        else
                            {
                            if ((int ) ptoc_Var248->a_sqlmode == 0)
                                {
                                _ptoc_ASSIGN(unsigned char , 130, ptoc_Var250->oprivset, ptoc_Var269->integer.C_1.priv_upd_set)
                                }
                            else
                                {
                                sql__psetadd(ptoc_Var250->oprivset , ptoc_Var269->integer.C_1.priv_upd_set , ptoc_Var269->integer.C_1.priv_sel_set , sizeof(tak_columnset ));
                                }
                            }
                        _ptoc_ASSIGN(unsigned char , 130, ptoc_Var249->d_upd_set, ptoc_Var269->integer.C_1.priv_upd_set)
                        }
                    }
                }
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var248->a_return_segm , "vak660.p" , 21164)).integer.C_4.sp1r_returncode == 0)
                {
                if ((char ) sql__setrang(ptoc_Var268->bunloaded , 0 , 1) && !(char ) sql__setrang(ptoc_Var249->d_view , 0 , 1))
                    {
                    a07_b_put_error(&(*acv) , 740 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak660.p" , 21169))[sql__setrang(table_node , 0 , 32767)].n_pos);
                    }
                else
                    {
                    if ((char ) sql__setrang(ptoc_Var248->a_progusage_add , 0 , 1))
                        {
                        a38_add_progusage(&(*acv) , 5 , ptoc_Var250->ouser , ptoc_Var250->otable , a01_il_b_identifier);
                        }
                    if (1 && (ptoc_Var249->d_globstate[0] & 64) != 0)
                        {
                        t01name(5 , "temp_lock         ");
                        }
                    if (1 && (ptoc_Var249->d_globstate[0] & 128) != 0)
                        {
                        t01name(5 , "perm_lock         ");
                        }
                    t01int4(5 , "isolation_in" , sql__ord((int ) ptoc_Var248->a_isolation_info));
                    t01int4(5 , "btablekind  " , sql__ord((int ) ptoc_Var268->btablekind));
                    if ((1 && (ptoc_Var249->d_globstate[0] & 64) != 0 || 1 && (ptoc_Var249->d_globstate[0] & 128) != 0) && (int ) ptoc_Var248->a_isolation_info == 2 && (int ) ptoc_Var268->btreeid.integer.C_4.fileTfn_gg00 != 26 && ((int ) ptoc_Var268->btablekind == 1 || (int ) ptoc_Var268->btablekind == 2 || (int ) ptoc_Var268->btablekind == 5))
                        {
                        a54add_next_temp_lock(&(*acv) , ptoc_Var268->btreeid.integer.C_4.fileTabId_gg00 , ptoc_Var249->d_globstate);
                        }
                    if ((ptoc_Var258 = (int ) ptoc_Var268->btablekind) == 4 || ptoc_Var258 == 5 || ptoc_Var258 == 7 || ptoc_Var258 == 10)
                        {
                        ptoc_Var250->oviewqual = (char ) sql__setrang(ptoc_Var268->bv_qualexist , 0 , 1);
                        ptoc_Var250->oviewcheck = (char ) sql__setrang(ptoc_Var268->bv_checkopt , 0 , 1);
                        if ((int ) ptoc_Var268->btablekind != 10)
                            {
                            if ((int ) ptoc_Var268->bv_level > (int ) ptoc_Var249->d_phase_cnt)
                                {
                                ptoc_Var249->d_phase_cnt = (short ) sql__setrang((short ) ptoc_Var268->bv_level , -32768 , 32767);
                                }
                            if ((int ) ptoc_Var268->bv_distinct != 0)
                                {
                                ptoc_Var249->d_distinct = (unsigned char ) sql__setrang(ptoc_Var268->bv_distinct , 0 , 2);
                                }
                            if ((ptoc_Var258 = (int ) ptoc_Var268->btablekind) == 4 || ptoc_Var258 == 5)
                                {
                                if ((int ) ptoc_Var268->bv_tablekind == 15)
                                    {
                                    _ptoc_MOVE(unsigned char , 1, ptoc_Var250->ospecialname, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var255 , ptoc_Var250->ospecialname , ptoc_Var246 , sizeof(unsigned char [1])));
                                    }
                                }
                            else
                                {
                                _ptoc_MOVE(unsigned char , 1, ptoc_Var250->ospecialname, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var253 , ptoc_Var250->ospecialname , ptoc_Var247 , sizeof(unsigned char [1])));
                                }
                            }
                        else
                            {
                            ptoc_Var250->ocomplex_view = 1;
                            if ((int ) (*acv).a_intern_select_cnt == 0)
                                {
                                ptoc_Var249->d_phase_cnt = (short ) sql__setrang((short ) ptoc_Var268->bv_level , -32768 , 32767);
                                complicated_view = 1;
                                if (!(char ) sql__setrang(ptoc_Var249->d_view , 0 , 1) && (int ) lock_spec == 0)
                                    {
                                    a54complicated_view(&(*acv) , &(*dmli) , ptoc_Var267->integer.C_50.syskey.integer.C_2.stableid);
                                    }
                                if ((int ) ptoc_Var249->d_cntfromtab == 1 && ((int ) ptoc_Var248->a_statement_kind == 41 || (char ) sql__setrang(ptoc_Var248->a_intern_explain , 0 , 1)) && !(char ) sql__setrang(ptoc_Var248->a_from_select , 0 , 1) && ptoc_Var248->a_union_cnt == 0)
                                    {
                                    a664new_optimize_info(&(*acv) , &(*dmli) , table_node , next_n);
                                    }
                                }
                            else
                                {
                                a660build_view_treeid(&(*acv) , ptoc_Var267->integer.C_50.syskey.integer.C_2.stableid , &ptoc_Var250->otreeid);
                                }
                            }
                        ptoc_Var250->oview = 1;
                        }
                    }
                }
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var248->a_return_segm , "vak660.p" , 21272)).integer.C_4.sp1r_returncode == 0)
        {
        i = 1;
        while (i < (int ) ptoc_Var249->d_acttabindex)
            {
            if ((_ptoc_MEMCMP(unsigned char , 64, ptoc_Var249->d_tabarr[sql__setrang(i , 1 , 64) + -1].ouser , ptoc_Var250->ouser) == 0 ? 1 : 0) && (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var249->d_tabarr[sql__setrang(i , 1 , 64) + -1].otable , ptoc_Var250->otable) == 0 ? 1 : 0) && (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var249->d_tabarr[sql__setrang(i , 1 , 64) + -1].oreference , ptoc_Var250->oreference) == 0 ? 1 : 0) || 1 && (ptoc_Var249->d_tabarr[sql__setrang(i , 1 , 64) + -1].ospecialname[0] & 4) != 0 && (0 || (ptoc_Var250->ospecialname[0] & 4) == 0) && (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var249->d_tabarr[sql__setrang(i , 1 , 64) + -1].oreference , ptoc_Var250->otable) == 0 ? 1 : 0))
                {
                a07_b_put_error(&(*acv) , 6262 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var248->a_ap_tree , "vak660.p" , 21288))[sql__setrang(table_node , 0 , 32767)].n_pos);
                }
            i = sql__succ(i , -2147483647 , 2147483647 , "vak660.p" , 21290);
            }
        }
    t01int4(5 , "d_view      " , sql__ord((int ) (char ) sql__setrang(ptoc_Var249->d_view , 0 , 1)));
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var248->a_return_segm , "vak660.p" , 21296)).integer.C_4.sp1r_returncode == 0 && (char ) sql__setrang(ptoc_Var249->d_view , 0 , 1) && (int ) ptoc_Var249->d_phase_cnt <= 4)
        {
        sysk = a01defaultkey;
        _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_2.stableid, ptoc_Var249->d_tableid)
        _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0\274")
        a10get_sysinfo(&(*acv) , &sysk , 1 , &p , &b_err);
        if ((int ) b_err == 0)
            {
            ptoc_Var275 = (struct tak_systembuffer *) sql__nil(p , "vak660.p" , 21306);
            ptoc_Var276 = &ptoc_Var275->integer.C_49.sviewtext;
            i = 1;
            while (i <= (int ) ptoc_Var276->vttabcount)
                if ((int ) ptoc_Var276->boolean.C_true.vttab[sql__setrang(i , 1 , 64) + -1].vtttab_n_pos == ptoc_Var249->d_n_pos)
                    {
                    i = 32767;
                    }
                else
                    {
                    i = i + 1;
                    }
            if (i < 32767)
                {
                ptoc_Var276->vttabcount = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var276->vttabcount , -32768 , 32767 , "vak660.p" , 21320) , -32768 , 32767);
                ptoc_Var277 = &ptoc_Var276->boolean.C_true.vttab[sql__setrang((int ) ptoc_Var276->vttabcount , 1 , 64) + -1];
                _ptoc_ASSIGN(unsigned char , 8, ptoc_Var277->vtttableid, ptoc_Var250->ofromtableid)
                ptoc_Var277->vttsynonym = (char ) sql__setrang(1 && (ptoc_Var250->ospecialname[0] & 2) != 0 , 0 , 1);
                if ((char ) sql__setrang(ptoc_Var277->vttsynonym , 0 , 1))
                    {
                    _ptoc_ASSIGN(unsigned char , 8, ptoc_Var277->vttreftabid, synid)
                    }
                else
                    {
                    _ptoc_ASSIGN(unsigned char , 8, ptoc_Var277->vttreftabid, "\0\0\0\0\0\0\0\0")
                    }
                ptoc_Var277->vtttabcount = (unsigned char ) sql__setrang(ptoc_Var250->ocounttabs , 0 , 255);
                ptoc_Var277->vttrefname = (char ) sql__setrang(1 && (ptoc_Var250->ospecialname[0] & 4) != 0 , 0 , 1);
                ptoc_Var277->vttcomplex_view = (char ) sql__setrang(complicated_view , 0 , 1);
                ptoc_Var277->vtttab_n_pos = (short ) sql__setrang((short ) ptoc_Var249->d_n_pos , -32768 , 32767);
                a10repl_sysinfo(&(*acv) , &p , &b_err);
                }
            }
        if ((int ) b_err != 0)
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
            }
        }
    cccprint("<a660_search_one");
    }

static
  tgg00_HandlingSet ptoc_Var278 = 
  {
  (unsigned char)'\001', (unsigned char)'\0'
  };


int a660_select(acv, startnode)
tak_all_command_glob *acv;
tsp_int2 startnode;
    {
    char execute_select;
    char complex_select;
    char init_info_output;
    char is_from_select;
    char is_outer_join;
    char pseudo_resultset_select;
    char complex_for_locks;
    tak_execution_kind init_ex_kind;
    int lock_n;
    int dummy;
    tak_parskey parsk;
    tak_dml_info dmli;
    int ptoc_Var279;
    tak_transinfo *ptoc_Var280;
    cccprint("AK660 a660_selec");
    a54_dml_init(&dmli , 0);
    (*acv).a_unionrec_ptr = (struct tak_systembuffer *) (int *) 0;
    (*acv).a_p_arr1.px[0] = (struct tak_systembuffer *) (int *) 0;
    (*acv).a_p_arr1.pbasep = (struct tak_systembuffer *) (int *) 0;
    (*acv).a_p_arr1.pcount = 0;
    is_from_select = (char ) sql__setrang((*acv).a_from_select , 0 , 1);
    (*acv).a_from_select = 0;
    (*acv).a_fromsel_n = 0;
    (*acv).a_union_cnt = 0;
    dmli.d_outer_join = (char ) sql__setrang((char ) sql__setrang(dmli.d_outer_join , 0 , 1) || (char ) sql__setrang((*acv).a_outer_join , 0 , 1) , 0 , 1);
    is_outer_join = (char ) sql__setrang(dmli.d_outer_join , 0 , 1);
    pseudo_resultset_select = 0;
    execute_select = 0;
    complex_select = 0;
    init_ex_kind = (unsigned char ) sql__setrang((*acv).a_ex_kind , 0 , 4);
    init_info_output = (char ) sql__setrang((*acv).a_info_output , 0 , 1);
    lock_n = (int ) startnode;
    if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21394))[sql__setrang(lock_n , 0 , 32767)].n_proc == 54 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21395))[sql__setrang(lock_n , 0 , 32767)].n_subproc == 24)
        {
        lock_n = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21398))[sql__setrang(lock_n , 0 , 32767)].n_lo_level;
        lock_n = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21399))[sql__setrang(lock_n , 0 , 32767)].n_sa_level;
        lock_n = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21400))[sql__setrang(lock_n , 0 , 32767)].n_sa_level;
        }
    do
        lock_n = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21404))[sql__setrang(lock_n , 0 , 32767)].n_lo_level;
    while (!((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21406))[sql__setrang(lock_n , 0 , 32767)].n_proc == 51 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21407))[sql__setrang(lock_n , 0 , 32767)].n_subproc == 1 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21408))[sql__setrang(lock_n , 0 , 32767)].n_proc == 0));
    do
        lock_n = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21412))[sql__setrang(lock_n , 0 , 32767)].n_sa_level;
    while (!(lock_n == 0 || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21415))[sql__setrang(lock_n , 0 , 32767)].n_proc == 51 && ((ptoc_Var279 = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21416))[sql__setrang(lock_n , 0 , 32767)].n_subproc) == 5 || ptoc_Var279 == 6 || ptoc_Var279 == 4 || ptoc_Var279 == 7 || ptoc_Var279 == 8) || (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21419))[sql__setrang(lock_n , 0 , 32767)].n_proc == 42 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21420))[sql__setrang(lock_n , 0 , 32767)].n_subproc == 228));
    if ((char ) sql__setrang((*acv).a_intern_explain , 0 , 1))
        {
        _ptoc_MOVE(unsigned char , 2, dmli.d_globstate, ptoc_Var278);
        }
    else
        {
        _ptoc_ASSIGN(unsigned char , 2, dmli.d_globstate, (*acv).a_transinf.tri_global_state)
        if (lock_n != 0)
            {
            dmli.d_with_lock_clause = 1;
            a660_lock(&(*acv) , lock_n , dmli.d_globstate);
            }
        }
    if ((int ) (*acv).a_max_intern_select > 0)
        {
        _ptoc_ASSIGN(unsigned char , 2, dmli.d_globstate, (*acv).a_cmd_globstate)
        }
    if ((int ) (*acv).a_max_intern_select > 0 && (int ) (*acv).a_ex_kind != 0)
        {
        ptoc_Var280 = &(*acv).a_transinf;
        if (1 && (dmli.d_globstate[0] & 64) != 0 || 1 && (dmli.d_globstate[0] & 128) != 0)
            {
            (*acv).a_ex_kind = 0;
            (*acv).a_info_output = 0;
            complex_select = (char ) sql__setrang((int ) (*acv).a_intern_select_cnt == (int ) (*acv).a_max_intern_select , 0 , 1);
            execute_select = 1;
            }
        }
    t01command_kind(5 , "command_kind" , (unsigned char ) sql__setrang((*acv).a_command_kind , 0 , 11));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 21463)).integer.C_4.sp1r_returncode == 0)
        {
        _ptoc_ASSIGN(unsigned char , 2, dmli.d_unchanged_globstate, dmli.d_globstate)
        complex_for_locks = 0;
        if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21468))[sql__setrang((int ) startnode , 0 , 32767)].n_proc == 54 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21469))[sql__setrang((int ) startnode , 0 , 32767)].n_subproc == 24)
            {
            (*acv).a_ex_kind = 0;
            (*acv).a_info_output = 0;
            complex_select = 1;
            execute_select = (char ) sql__setrang((int ) init_ex_kind != 0 && (!(char ) sql__setrang(complex_select , 0 , 1) || (int ) (*acv).a_intern_select_cnt == (int ) (*acv).a_max_intern_select) , 0 , 1);
            a662_recursive_select(&(*acv) , (short ) sql__setrang(startnode , -32768 , 32767) , &dmli);
            }
        else
            {
            if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21481))[sql__setrang((int ) startnode , 0 , 32767)].n_proc == 54 && ((ptoc_Var279 = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21482))[sql__setrang((int ) startnode , 0 , 32767)].n_subproc) == 13 || ptoc_Var279 == 14 || ptoc_Var279 == 15 || ptoc_Var279 == 16 || ptoc_Var279 == 17 || ptoc_Var279 == 18 || ptoc_Var279 == 19))
                {
                if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21487))[sql__setrang((int ) startnode , 0 , 32767)].n_subproc == 13)
                    {
                    if ((1 && (dmli.d_globstate[0] & 64) != 0 || 1 && (dmli.d_globstate[0] & 128) != 0) && (int ) (*acv).a_command_kind == 0)
                        {
                        (*acv).a_ex_kind = 0;
                        (*acv).a_info_output = 0;
                        if ((int ) (*acv).a_isolation_info != 2)
                            {
                            (*acv).a_isolation_info = 1;
                            }
                        (*acv).a_command_kind = 3;
                        complex_for_locks = 1;
                        complex_select = 1;
                        }
                    execute_select = (char ) sql__setrang((int ) init_ex_kind != 0 && (!(char ) sql__setrang(complex_select , 0 , 1) || (int ) (*acv).a_intern_select_cnt == (int ) (*acv).a_max_intern_select) , 0 , 1);
                    a662_start_union_select(&(*acv) , (short ) sql__setrang(startnode , -32768 , 32767) , &dmli , &pseudo_resultset_select , &parsk);
                    if ((char ) sql__setrang(complex_for_locks , 0 , 1))
                        {
                        a54set_complex_entry(&(*acv) , 2);
                        }
                    }
                else
                    {
                    a07_b_put_error(&(*acv) , 6292 , (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21518))[sql__setrang((int ) startnode , 0 , 32767)].n_pos);
                    }
                }
            else
                {
                a660select(&(*acv) , (short ) sql__setrang(startnode , -32768 , 32767) , &dmli , &pseudo_resultset_select);
                parsk = (*acv).a_pars_last_key;
                }
            }
        (*acv).a_info_output = (char ) sql__setrang(init_info_output , 0 , 1);
        if ((int ) (*acv).a_isolation_info == 2 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 21529)).integer.C_4.sp1r_returncode == 0 && (int ) (*acv).a_qualified_jv_upd == 0)
            {
            a54_loc_temp_locks(&(*acv) , (*acv).a_transinf.tri_global_state , &(*acv).a_p_arr1);
            }
        t01bool(5 , "complex_for_" , (char ) sql__setrang(complex_for_locks , 0 , 1));
        t01command_kind(5 , "command_kind" , (unsigned char ) sql__setrang((*acv).a_command_kind , 0 , 11));
        t01bool(5 , "execute_sele" , (char ) sql__setrang(execute_select , 0 , 1));
        t01int4(5 , "a_first_pars" , sql__ord((int ) (*acv).a_first_parskey));
        t01bool(5 , "complex_sele" , (char ) sql__setrang(complex_select , 0 , 1));
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 21542)).integer.C_4.sp1r_returncode == 0)
            {
            if ((char ) sql__setrang(execute_select , 0 , 1) && !(char ) sql__setrang(dmli.d_only_sem_check , 0 , 1))
                {
                if ((char ) sql__setrang(complex_select , 0 , 1))
                    {
                    parsk = (*acv).a_pars_last_key;
                    parsk.p_no = (unsigned char)'\0';
                    parsk.p_id[0] = (unsigned char ) sql__setrang((*acv).a_first_parskey , 0 , 255);
                    parsk.p_kind = 10;
                    }
                a661exec_sub(&(*acv) , &dmli , &parsk , (short ) sql__setrang(startnode , -32768 , 32767) , &dummy , (char ) sql__setrang((*acv).a_info_output , 0 , 1) , (char ) sql__setrang(pseudo_resultset_select , 0 , 1));
                }
            }
        (*acv).a_ex_kind = (unsigned char ) sql__setrang(init_ex_kind , 0 , 4);
        if ((char ) sql__setrang(is_from_select , 0 , 1) && ((int ) (*acv).a_ex_kind != 0 || (char ) sql__setrang((*acv).a_intern_explain , 0 , 1) || (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 21569)).integer.C_4.sp1r_returncode != 0))
            {
            a661_fdelete_fromtab_results(&(*acv));
            }
        if ((char ) sql__setrang(is_from_select , 0 , 1) || (int ) (*acv).a_intern_select_cnt > 0 && (int ) (*acv).a_intern_select_cnt < (int ) (*acv).a_max_intern_select)
            {
            a663_intern_resname_drop(&(*acv));
            }
        }
    else
        {
        ak660close_used_resultname(&(*acv));
        }
    if ((int ) dmli.d_pos_result == 2)
        {
        t01int4(5 , "intern_resul" , (int ) dmli.d_pos_result);
        }
    else
        {
        if ((int ) dmli.d_pos_result == 1)
            {
            t01int4(5 , "user_result " , (int ) dmli.d_pos_result);
            }
        else
            {
            t01int4(5 , "d_pos_result" , (int ) dmli.d_pos_result);
            }
        }
    (*acv).a_from_select = (char ) sql__setrang(is_from_select , 0 , 1);
    (*acv).a_outer_join = (char ) sql__setrang(is_outer_join , 0 , 1);
    (*acv).a_part_rollback = (char ) sql__setrang((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 21598)).integer.C_4.sp1r_returncode != 0 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 21599)).integer.C_4.sp1r_returncode != 100 , 0 , 1);
    t01int4(5 , "iso lvl ak66" , sql__ord((int ) (*acv).a_isolation_info));
    cccprint("<a660_select    ");
    }



int a660_short_first_part(acv, dmli, tree)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tgg00_FileId *tree;
    {
    tsp_knl_identifier resname;
    tak_dml_info *ptoc_Var281;
    cccprint("AK660 a660_short");
    ptoc_Var281 = &*dmli;
    ptoc_Var281->d_distinct = 0;
    ptoc_Var281->d_resname_found = 0;
    ptoc_Var281->d_inoutpos = 1;
    ptoc_Var281->d_change.cr_colcount = 0;
    ptoc_Var281->d_outcolno = 1;
    ptoc_Var281->d_exprno = 1;
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var281->d_refname, a01_il_b_identifier)
    ptoc_Var281->d_view = 0;
    ptoc_Var281->d_like = 0;
    ptoc_Var281->d_movebefore = 8;
    ptoc_Var281->d_pargeslen = 0;
    ptoc_Var281->d_range = 0;
    ptoc_Var281->d_maxlen = 0;
    ptoc_Var281->d_single = 0;
    _ptoc_ASSIGN(unsigned char , 64, resname, a01_into_res_name)
    a663_make_res_buf(&(*acv) , &(*dmli) , resname , &(*tree));
    cccprint("<a660_short_firs");
    }

static
  unsigned char ptoc_Var282[1] = 
  {
  (unsigned char)'\020'
  };


int a660build_view_treeid(acv, tableid, tree)
tak_all_command_glob *acv;
tgg_surrogate tableid;
tgg00_FileId *tree;
    {
    tgg_lkey k;
    tgg_rec_buf buf;
    unsigned char ptoc_Var283[1];
    unsigned char ptoc_Var284[1];
    unsigned char ptoc_Var285[1];
    cccprint("AK660 a660build_");
    *tree = (*acv).a_intern_res_tree;
    if ((char ) sql__setrang((*acv).a_shared_result , 0 , 1))
        {
        _ptoc_MOVE(unsigned char , 1, (*tree).integer.C_1.fileType_gg00, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var283 , (*tree).integer.C_1.fileType_gg00 , ptoc_Var282 , sizeof(unsigned char [1])));
        }
    s10mv2(sizeof(tsp00_C8 ) , sizeof(tsp00_Key ) , tableid , 1 , k.integer.C_11.k , 1 , 8);
    k.integer.C_11.len = 8;
    b07cget_record(&(*acv).a_transinf.tri_trans , &(*acv).a_intern_cmd_tree , &k , &buf);
    if ((int ) (*acv).a_transinf.tri_trans.trError_gg00 == 0)
        {
        (*tree).integer.C_8.fileLevel_gg00[0] = (unsigned char ) sql__setrang(buf.integer.C_12.info[8] , 0 , 255);
        (*tree).integer.C_8.fileLevel_gg00[1] = (unsigned char ) sql__setrang(buf.integer.C_12.info[9] , 0 , 255);
        }
    else
        {
        if ((int ) (*acv).a_transinf.tri_trans.trError_gg00 == 1200)
            {
            a07ak_system_error(&(*acv) , 660 , 1);
            }
        else
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767) , 1);
            }
        }
    cccprint("<a660build_view_");
    }

static
  unsigned char ptoc_Var286[1] = 
  {
  (unsigned char)'\020'
  };


int a660construct_subtree(acv, sub_tree, sub_count)
tak_all_command_glob *acv;
tgg00_FileId *sub_tree;
int sub_count;
    {
    int ptoc_Var287;
    unsigned char ptoc_Var288[1];
    unsigned char ptoc_Var289[1];
    unsigned char ptoc_Var290[1];
    cccprint("AK660 a660constr");
    g04build_temp_tree_id(&(*sub_tree) , &(*acv).a_transinf.tri_trans);
    (*sub_tree).integer.C_8.fileTfnTemp_gg00 = 12;
    (*sub_tree).integer.C_8.fileLevel_gg00[0] = (unsigned char ) sql__setrang((unsigned char ) (sub_count / 100) , 0 , 255);
    (*sub_tree).integer.C_8.fileLevel_gg00[1] = (unsigned char ) sql__setrang((unsigned char ) ((ptoc_Var287 = sub_count % 100) >= 0 ? ptoc_Var287 : ptoc_Var287 + 100) , 0 , 255);
    _ptoc_ASSIGN(unsigned char , 2, (*sub_tree).integer.C_2.fileResultSite_gg00, "\377\377")
    if ((char ) sql__setrang((*acv).a_shared_result , 0 , 1))
        {
        _ptoc_MOVE(unsigned char , 1, (*sub_tree).integer.C_1.fileType_gg00, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var288 , (*sub_tree).integer.C_1.fileType_gg00 , ptoc_Var286 , sizeof(unsigned char [1])));
        }
    cccprint("<a660construct_s");
    }



int a660select(acv, startnode, dmli, pseudo_resultset_select)
tak_all_command_glob *acv;
tsp_int2 startnode;
tak_dml_info *dmli;
char *pseudo_resultset_select;
    {
    char m_use_sub;
    char no_subq;
    int curr_n;
    tak_parskey parsk;
    tak70_strat_rec sr_rec;
    tsp_int4 m_curr_res_id;
    int ptoc_Var291;
    tak_unionrecord *ptoc_Var292;
    struct ptoc_Type148 *ptoc_Var293;
    cccprint("AK660 a660select");
    t01int4(5 , "startnode   " , (int ) startnode);
    t01bool(5 , "a_from_selec" , (char ) sql__setrang((*acv).a_from_select , 0 , 1));
    t01int4(5 , "a_fromsel_n " , (int ) (*acv).a_fromsel_n);
    t01int4(5 , "startn n_pos" , (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21721))[sql__setrang((int ) startnode , 0 , 32767)].n_pos);
    t01bool(5 , "d_union     " , (char ) sql__setrang((*dmli).d_union , 0 , 1));
    t01int4(5 , "a_union_cnt " , (*acv).a_union_cnt);
    t01command_kind(5 , "command_kind" , (unsigned char ) sql__setrang((*acv).a_command_kind , 0 , 11));
    sr_rec.sr_use_rowno = 0;
    sr_rec.sr_use_subquery = 0;
    sr_rec.sr_invkeylen = 32767;
    if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21728))[sql__setrang((int ) startnode , 0 , 32767)].n_proc == 54 && ((ptoc_Var291 = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21729))[sql__setrang((int ) startnode , 0 , 32767)].n_subproc) == 13 || ptoc_Var291 == 14 || ptoc_Var291 == 15 || ptoc_Var291 == 16 || ptoc_Var291 == 17 || ptoc_Var291 == 18 || ptoc_Var291 == 19))
        {
        if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21735))[sql__setrang((int ) startnode , 0 , 32767)].n_subproc == 13)
            {
            m_curr_res_id = (*acv).a_union_res_id;
            t01int4(5 , "a_curr_resi*" , (*acv).a_curr_res_id);
            t01int4(5 , "m_curr_resi*" , m_curr_res_id);
            a662_start_union_select(&(*acv) , (short ) sql__setrang(startnode , -32768 , 32767) , &(*dmli) , &(*pseudo_resultset_select) , &parsk);
            t01int4(5 , "a_curr_resi*" , (*acv).a_curr_res_id);
            t01int4(5 , "m_curr_res**" , m_curr_res_id);
            (*acv).a_union_res_id = m_curr_res_id;
            }
        else
            {
            a662_union_select(&(*acv) , (short ) sql__setrang(startnode , -32768 , 32767) , &(*dmli));
            }
        }
    else
        {
        if ((char ) sql__setrang((*dmli).d_union , 0 , 1))
            {
            ptoc_Var292 = &(*(struct tak_systembuffer *) sql__nil((*acv).a_unionrec_ptr , "vak660.p" , 21755)).integer.C_40.sunionrec;
            ptoc_Var293 = &ptoc_Var292->uselect_arr[sql__setrang((int ) ptoc_Var292->uselect_cnt , 1 , 493) + -1];
            ptoc_Var293->ufirstsel = 0;
            ptoc_Var293->usecondsel = 0;
            ptoc_Var293->uis_kind = 0;
            ptoc_Var293->uis_all = 0;
            (*acv).a_union_cnt = sql__succ((*acv).a_union_cnt , -2147483647 , 2147483647 , "vak660.p" , 21761);
            ptoc_Var292->uselect_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var292->uselect_cnt , -32768 , 32767 , "vak660.p" , 21762) , -32768 , 32767);
            t01int4(5 , "uselect_cnt " , (int ) ptoc_Var292->uselect_cnt);
            if ((int ) ptoc_Var292->uselect_cnt > 493)
                {
                a07_b_put_error(&(*acv) , 2809 , (int ) ptoc_Var292->uselect_cnt);
                }
            else
                {
                a662realloc_new_unionrec(&(*acv));
                }
            }
        curr_n = (int ) startnode;
        if ((int ) (*acv).a_command_kind != 8)
            {
            (*dmli).d_act_node = (short ) sql__setrang(startnode , -32768 , 32767);
            }
        (*acv).a_rowno_allowed = 1;
        sr_rec.sr_distinct_bytes = 1;
        no_subq = 1;
        if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21781))[sql__setrang(curr_n , 0 , 32767)].n_sa_level != 0 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 21782)).integer.C_4.sp1r_returncode == 0)
            {
            if ((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21784))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak660.p" , 21784))[sql__setrang(curr_n , 0 , 32767)].n_sa_level , 0 , 32767)].n_symb != 42)
                {
                t01int4(5 , "a_union_resi" , sql__ord((int ) (*acv).a_union_res_id));
                ak660subquery(&(*acv) , &(*dmli) , &sr_rec , (short ) sql__setrang((short ) curr_n , -32768 , 32767) , &(*pseudo_resultset_select));
                no_subq = 0;
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 21796)).integer.C_4.sp1r_returncode == 0 && (char ) sql__setrang(no_subq , 0 , 1))
            {
            m_use_sub = (char ) sql__setrang((*dmli).d_use_sub , 0 , 1);
            if ((int ) (*acv).a_command_kind == 8)
                {
                sr_rec.sr_use_subquery = (char ) sql__setrang((*dmli).d_use_sub , 0 , 1);
                (*dmli).d_use_sub = 1;
                }
            *pseudo_resultset_select = 0;
            if ((char ) sql__setrang((*dmli).d_view , 0 , 1) && (char ) sql__setrang((*acv).a_outer_join , 0 , 1))
                {
                (*dmli).d_phase_cnt = 4;
                }
            t01bool(5 , "a_from_selec" , (char ) sql__setrang((*acv).a_from_select , 0 , 1));
            if ((*dmli).d_sparr.pparsp != (struct tak_systembuffer *) (int *) 0)
                {
                t01int4(5 , "p_subcntleve" , sql__ord((int ) (*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.pparsp , "vak660.p" , 21817)).integer.C_23.sparsinfo.integer.C_2.p_subcntlevel));
                }
            t01int4(5 , "d_subcount  " , (int ) (*dmli).d_subcount);
            a660_query_execute(&(*acv) , &(*dmli) , curr_n , (char ) sql__setrang(!(char ) sql__setrang((*acv).a_from_select , 0 , 1) && !(char ) sql__setrang((*dmli).d_only_sem_check , 0 , 1) , 0 , 1) , 8 , &sr_rec , &(*pseudo_resultset_select) , 1 , 1 , 0);
            (*dmli).d_use_sub = (char ) sql__setrang(m_use_sub , 0 , 1);
            (*dmli).d_outer_join = (char ) sql__setrang((char ) sql__setrang((*dmli).d_outer_join , 0 , 1) || (char ) sql__setrang((*acv).a_outer_join , 0 , 1) , 0 , 1);
            if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 21826)).integer.C_4.sp1r_returncode == 21001)
                {
                (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 21829)).integer.C_4.sp1r_returncode = 0;
                ak660join_query_execute(&(*acv) , &(*dmli) , curr_n , &sr_rec , &(*pseudo_resultset_select));
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak660.p" , 21836)).integer.C_4.sp1r_returncode == 0 && (int ) (*acv).a_max_intern_select > 0 && (int ) (*acv).a_intern_select_cnt == 0 && !(char ) sql__setrang((*dmli).d_union , 0 , 1) && !(char ) sql__setrang((*acv).a_from_select , 0 , 1))
            {
            if ((*dmli).d_esparr.pbasep != (struct tak_systembuffer *) (int *) 0)
                {
                a10_key_del(&(*acv) , &(*(struct tak_systembuffer *) sql__nil((*dmli).d_esparr.pbasep , "vak660.p" , 21847)).integer.C_50.syskey);
                }
            (*dmli).d_esparr.pbasep = (struct tak_systembuffer *) (int *) 0;
            }
        }
    if ((char ) sql__setrang((*dmli).d_view , 0 , 1) && (char ) sql__setrang((*acv).a_outer_join , 0 , 1))
        {
        a681check_outer_join(&(*acv) , &(*dmli));
        }
    cccprint("<a660select     ");
    }



int ak660syn_owner(acv, synid, owner)
tak_all_command_glob *acv;
tgg_surrogate synid;
tsp_knl_identifier owner;
    {
    tgg_basis_error b_err;
    tak_sysbufferaddress p;
    tgg_sysinfokey sysk;
    cccprint("AK660 ak660syn_o");
    _ptoc_ASSIGN(unsigned char , 64, owner, (*acv).a_curr_user_name)
    sysk = a01defaultkey;
    _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_2.stableid, synid)
    a10get_sysinfo(&(*acv) , &sysk , 1 , &p , &b_err);
    if ((int ) b_err == 0)
        {
        if (_ptoc_MEMCMP(unsigned char , 8, (*(struct tak_systembuffer *) sql__nil(p , "vak660.p" , 21882)).integer.C_34.ssynonym.syn_owner , (*acv).a_curr_user_id) == 0 ? 0 : 1)
            {
            a06determine_username(&(*acv) , (*(struct tak_systembuffer *) sql__nil(p , "vak660.p" , 21885)).integer.C_34.ssynonym.syn_owner , owner);
            }
        }
    else
        {
        a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
        }
    cccprint("<ak660syn_owner ");
    }



int ak660put_tablename(acv, dmli)
tak_all_command_glob *acv;
tak_dml_info *dmli;
    {
    tak_all_command_glob *ptoc_Var294;
    tak_dml_info *ptoc_Var295;
    struct ptoc_Type172 *ptoc_Var296;
    cccprint("AK660 ak660put_t");
    ptoc_Var294 = &*acv;
    ptoc_Var295 = &*dmli;
    t01name(5 , "TAB BY SEL FOR UPD");
    t01int4(5 , " table count" , (int ) ptoc_Var295->d_cntfromtab);
    t01lidentifier(5 , ptoc_Var295->d_tabarr[sql__setrang((int ) ptoc_Var295->d_cntfromtab , 1 , 64) + -1].ouser);
    t01lidentifier(5 , ptoc_Var295->d_tabarr[sql__setrang((int ) ptoc_Var295->d_cntfromtab , 1 , 64) + -1].otable);
    if ((int ) ptoc_Var295->d_cntfromtab == 1)
        {
        ptoc_Var296 = &ptoc_Var295->d_tabarr[sql__setrang((int ) ptoc_Var295->d_cntfromtab , 1 , 64) + -1];
        if (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var296->ouser , a01_il_b_identifier) == 0 ? 0 : 1)
            {
            ak660put_sep(&(*acv) , (unsigned char)'\"');
            ak660one_identifier(&(*acv) , ptoc_Var296->ouser);
            ak660put_sep(&(*acv) , (unsigned char)'\"');
            ak660put_sep(&(*acv) , (unsigned char)'.');
            }
        ak660put_sep(&(*acv) , (unsigned char)'\"');
        ak660one_identifier(&(*acv) , ptoc_Var296->otable);
        ak660put_sep(&(*acv) , (unsigned char)'\"');
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var294->a_return_segm , "vak660.p" , 21925)).integer.C_4.sp1r_returncode == 0)
            {
            a06finish_curr_retpart(&(*acv) , 19 , 1);
            }
        }
    cccprint("<ak660put_tablen");
    }

static
  tsp8_uni_opt_set ptoc_Var297 = 
  {
  (unsigned char)'\0'
  };


int ak660one_identifier(acv, id)
tak_all_command_glob *acv;
tsp_knl_identifier id;
    {
    tsp_c256 name_buf;
    tsp_int4 name_len;
    tsp_moveobj_ptr name_ptr;
    tsp_int4 id_len;
    tsp8_uni_error e;
    tsp_int4 err_char_no;
    tak_all_command_glob *ptoc_Var298;
    tsp8_uni_opt_set ptoc_Var299;
    cccprint("AK660 ak660one_i");
    ptoc_Var298 = &*acv;
    if ((char ) sql__setrang(g01unicode , 0 , 1))
        {
        name_ptr = (tsp00_MoveObj *) name_buf;
        id_len = s30unilnr((tsp00_MoveObj *) id , "\0 " , 1 , sizeof(unsigned char [64]));
        if (id_len > 0)
            {
            name_len = sizeof(tsp00_C256 );
            s80uni_trans((tsp00_MoveObj *) id , id_len , 20 , (tsp00_MoveObj *) name_buf , &name_len , (short ) sql__setrang((short ) (*(tsp1_packet *) sql__nil(ptoc_Var298->a_out_packet , "vak660.p" , 21966)).sp1_header.sp1h_mess_code , -32768 , 32767) , *(tsp8_uni_opt_set *) (_ptoc_MOVE(unsigned char , 1, ptoc_Var299, ptoc_Var297) , ptoc_Var299) , &e , &err_char_no);
            }
        }
    else
        {
        id_len = s30klen(id , (unsigned char)' ' , sizeof(unsigned char [64]));
        name_ptr = (tsp00_MoveObj *) id;
        name_len = id_len;
        }
    if (name_len > 0)
        {
        a06retpart_move(&(*acv) , name_ptr , name_len);
        }
    cccprint("<ak660one_identi");
    }

static
  tsp8_uni_opt_set ptoc_Var300 = 
  {
  (unsigned char)'\0'
  };


int ak660put_sep(acv, sep)
tak_all_command_glob *acv;
unsigned char sep;
    {
    tsp_c2 uni_char;
    tsp_c4 out_char;
    tsp_int4 len;
    tsp8_uni_error e;
    tsp_int4 err_char_no;
    tak_all_command_glob *ptoc_Var301;
    tsp8_uni_opt_set ptoc_Var302;
    cccprint("AK660 ak660put_s");
    ptoc_Var301 = &*acv;
    if ((char ) sql__setrang(g01unicode , 0 , 1))
        {
        uni_char[0] = (unsigned char)'\0';
        uni_char[1] = (unsigned char ) sql__setrang(sep , 0 , 255);
        len = sizeof(tsp00_C4 );
        s80uni_trans((tsp00_MoveObj *) uni_char , sizeof(tsp00_C2 ) , 20 , (tsp00_MoveObj *) out_char , &len , (short ) sql__setrang((short ) (*(tsp1_packet *) sql__nil(ptoc_Var301->a_out_packet , "vak660.p" , 22011)).sp1_header.sp1h_mess_code , -32768 , 32767) , *(tsp8_uni_opt_set *) (_ptoc_MOVE(unsigned char , 1, ptoc_Var302, ptoc_Var300) , ptoc_Var302) , &e , &err_char_no);
        }
    else
        {
        out_char[0] = (unsigned char ) sql__setrang(sep , 0 , 255);
        len = 1;
        }
    if (len > 0)
        {
        a06retpart_move(&(*acv) , (tsp00_MoveObj *) out_char , len);
        }
    cccprint("<ak660put_sep   ");
    }
