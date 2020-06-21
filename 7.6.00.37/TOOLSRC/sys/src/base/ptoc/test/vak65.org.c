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
  unsigned char tsp_c28[28];
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
  unsigned char tsp1_part_attr[1];
typedef
  struct ptoc_Type3
    {
    unsigned char sp1p_part_kind;
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
    tsp_data_type para_type;
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
  unsigned char tgg00_StackEntryType;
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
  struct ptoc_Type27
    {
    tgg00_Surrogate lockTabId_gg00;
    tgg00_ServerdbNo lockSite_gg00;
    unsigned char lockMode_gg00;
    tgg00_LockReqState lockState_gg00;
    tsp00_Int2 lockKeyLen_gg00;
    tsp00_Int2 lockKeyPos_gg00;
    } tgg00_Lock;
typedef
  struct ptoc_Type28
    {
    tgg00_Lkey listkey;
    tgg00_Lkey reckey;
    } tgg00_TwoKeys;
typedef
  struct ptoc_Type29
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
  struct ptoc_Type30
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
  struct ptoc_Type31
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
  struct ptoc_Type32
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
  struct ptoc_Type33
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
  struct ptoc_Type34
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
  struct ptoc_Type35
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
  struct ptoc_Type36
    {
    tsp00_Int4 idc_prim_leafnodes;
    tsp00_Int4 idc_sec_leafnodes;
    tsp00_Int4 idc_prim_keycount;
    tsp00_Int4 idc_sec_keycount;
    tsp00_Int4 idc_nullcount;
    } tgg00_IndexCounter;
typedef
  struct ptoc_Type37
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
  struct ptoc_Type38
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
        tgg00_StackOpBuildIn eop_build_in;
        tgg00_DateTimeFormat eformat;
        tsp00_DataType edatatype;
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
  struct ptoc_Type39
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
        unsigned char mswap_kind;
        } C_3;
      } integer;
    } tgg00_StackDesc;
typedef
  struct ptoc_Type40
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
  struct ptoc_Type41
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
  struct ptoc_Type42
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
  struct ptoc_Type43
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
  struct ptoc_Type44
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
  struct ptoc_Type45
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
  struct ptoc_Type46
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
  struct ptoc_Type47
    {
    tsp00_CodeType ctype;
    tsp00_CodeType char_default;
    tgg00_DateTimeFormat date_time;
    unsigned char kernel_swap;
    } tgg_code_globals;
typedef
  tgg00_DataPartPtr tgg_datapart_ptr;
typedef
  tgg00_DateTimeFormat tgg_datetimeformat;
typedef
  unsigned char tgg_linkset[1];
typedef
  struct tgg00_MessBlock tgg_mess_block;
typedef
  unsigned char tgg_privilege_set[1];
typedef
  tgg00_QualBufPtr tgg_qual_buf_ptr;
typedef
  struct ptoc_Type48
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
  struct ptoc_Type38 tgg_stack_entry;
typedef
  tgg00_StackEntryType tgg_stack_entry_type;
typedef
  tgg00_StackList tgg_stack_list;
typedef
  tgg00_StackListPtr tgg_stack_list_ptr;
typedef
  tgg00_StackOpBuildIn tgg_stack_op_build_in;
typedef
  tgg00_StackOpType tgg_stack_op_type;
typedef
  struct ptoc_Type49
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
  struct ptoc_Type50
    {
    tgg00_FileId file_id;
    tgg00_FileId inv_id;
    } tgg_two_tree_ids;
typedef
  unsigned char tak00_ooption[1];
typedef
  struct ptoc_Type51
    {
    struct ptoc_Type52
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
  struct ptoc_Type53
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
  struct ptoc_Type54
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
  unsigned char tak_procs;
typedef
  unsigned char tak_convert_type;
typedef
  unsigned char tak_table_attribute_set[1];
typedef
  struct ptoc_Type55
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
  struct ptoc_Type56
    {
    tak_procs n_proc;
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
  struct ptoc_Type57
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
  struct ptoc_Type38 tak_16stackentry[16];
typedef
  unsigned char tak_oms_version_id[22];
typedef
  unsigned char tak_sysbufferoffset[20];
typedef
  tsp00_C3 tak_pcountarr[100];
typedef
  struct ptoc_Type58
    {
    tsp_int4 rctxt_cnt;
    tsp00_C8 rctxt_roles[32768];
    } tak_role_context;
typedef
  tak_role_context *tak_role_context_ptr;
typedef
  unsigned char tak_map_set[768];
typedef
  struct ptoc_Type59
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
  struct ptoc_Type60
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
  struct ptoc_Type61
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
  struct ptoc_Type62
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
  struct ptoc_Type63
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
  struct ptoc_Type64
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
  struct ptoc_Type65
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
  struct ptoc_Type66
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
  struct ptoc_Type67
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
  struct ptoc_Type68
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
  struct ptoc_Type69
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
  struct ptoc_Type70
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
  struct ptoc_Type38 tak_ak_constr_stack[989];
typedef
  struct ptoc_Type71
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
  struct ptoc_Type72
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
  struct ptoc_Type73
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
  struct ptoc_Type74
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
    struct ptoc_Type75
      {
      tgg_surrogate ofp_procid;
      tsp_c3 ofp_language;
      unsigned char ofp_sqlmode[1];
      } fct_procids[64];
    } tak_funcrefrecord;
typedef
  struct ptoc_Type76
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
  struct ptoc_Type77
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
    struct ptoc_Type78
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
  struct ptoc_Type79
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
    struct ptoc_Type80
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
  struct ptoc_Type81
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
  struct ptoc_Type82
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
    struct ptoc_Type83
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
  struct ptoc_Type84
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
  struct ptoc_Type38 tak_trigger_stack[900];
typedef
  struct ptoc_Type85
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
  struct ptoc_Type86
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
    struct ptoc_Type87
      {
      tgg_surrogate usa_tableid;
      tsp_int2 usa_filler;
      unsigned char usa_tablekind;
      char usa_empty;
      } usagedef[672];
    } tak_usagerecord;
typedef
  struct ptoc_Type88
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
    struct ptoc_Type89
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
  struct ptoc_Type90
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
  struct ptoc_Type91
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
  struct ptoc_Type92
    {
    tak_sysbufferoffset ru_key_offset;
    tak_uref_restkey ru_rest_key;
    tsp_c2 ru_segmentid;
    tsp_int2 ru_filler;
    tgg_surrogate ru_user_id;
    } tak_userrefrecord;
typedef
  struct ptoc_Type93
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
  struct ptoc_Type94
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
  struct ptoc_Type95
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
  struct ptoc_Type96
    {
    tak_param_spec_ptr param_spec_ptr;
    tsp_int2 param_counter;
    tsp_int2 param_filler1;
    tsp_int4 param_filler2;
    } tak_param_list;
typedef
  struct ptoc_Type97
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
  struct ptoc_Type98
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
  struct ptoc_Type99
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
  struct ptoc_Type100
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
  struct ptoc_Type101
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
  struct ptoc_Type102
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
  struct ptoc_Type103
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
  struct ptoc_Type104
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
  struct ptoc_Type105
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
  struct ptoc_Type106
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
  struct ptoc_Type107
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
  struct ptoc_Type108
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
  struct ptoc_Type109
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
  struct ptoc_Type110
    {
    struct ptoc_Type111
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
  struct ptoc_Type112
    {
    tak_joinarr jrc_joinarr;
    tsp_int2 jrc_cnt;
    char jrc_col_upd;
    tsp00_Uint1 jrc_filler;
    } tak_joinrec;
typedef
  unsigned char tak_viewtext[8064];
typedef
  struct ptoc_Type113
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
        struct ptoc_Type114
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
  struct ptoc_Type115
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
    struct ptoc_Type116
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
  struct ptoc_Type117
    {
    tgg_surrogate vttableid;
    tsp_int2 vtfiller;
    char vtins_for_upd;
    char vtone_to_one;
    tak_charset vtnot_used_links;
    } tak_viewtabarr[183];
typedef
  struct ptoc_Type118
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
  struct ptoc_Type38 tak_view_stack_entries[1007];
typedef
  unsigned char tak_viewbuf[8056];
typedef
  struct ptoc_Type119
    {
    tsp_int2 drecno;
    tsp_int2 dextno;
    tsp_int2 dfiller;
    tsp00_Uint1 dexttabno;
    tsp00_Uint1 dtabno;
    } tak_derived_col_info[1009];
typedef
  struct ptoc_Type120
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
    tsp_int2 vjoincount;
    tak_viewjoins vjoin;
    } tak_viewqual_join_record;
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
    tsp_int2 vderivedcnt;
    tak_derived_col_info vderived_cols;
    } tak_viewqual_derivedcol_record;
typedef
  struct ptoc_Type124
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
  struct ptoc_Type125
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
    struct ptoc_Type126
      {
      tsp_int4 cst_dist_values;
      tsp_int4 cst_pages;
      tsp_int4 cst_avg_lst_len;
      } sti_col_stats[660];
    } tak_statisticsinfo;
typedef
  struct ptoc_Type127
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
  struct ptoc_Type128
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
    struct ptoc_Type129
      {
      tsp_knl_identifier str_name;
      tgg00_SubtransNo str_no;
      } str_subtrans[50];
    } tak_subtransrecord;
typedef
  struct ptoc_Type130
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
  struct ptoc_Type131
    {
    tsp_int2 ch_startpos;
    unsigned char ch_length;
    unsigned char ch_type[1];
    } tak_column_change_arr[1024];
typedef
  struct ptoc_Type132
    {
    tsp_int2 cr_colcount;
    char cr_was_opt;
    char cr_filler;
    tak_column_change_arr cr_columns;
    } tak_changerecord;
typedef
  struct ptoc_Type133
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
  struct ptoc_Type134
    {
    tsp_int2 ks_pos;
    tsp_int2 ks_len;
    } tak_keyspec;
typedef
  struct ptoc_Type135
    {
    tak_keyspec reckeyspec;
    tak_keyspec listkeyspec;
    } tak_two_keyspecs;
typedef
  unsigned char tak_res_keysbuf[13316];
typedef
  struct ptoc_Type136
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
  struct ptoc_Type137
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
  struct ptoc_Type138
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
  struct ptoc_Type139
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
  struct ptoc_Type140
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
  struct ptoc_Type141
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
  struct ptoc_Type142
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
  struct ptoc_Type143
    {
    tsp_int2 all_lock_count;
    tsp_int2 all_lock_fill;
    tgg00_Lock all_locks[240];
    } tak_ak_all_locks;
typedef
  struct ptoc_Type144
    {
    unsigned char fp_kind;
    tgg_stack_entry_type fp_etype;
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
  struct ptoc_Type145
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
  struct ptoc_Type146
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
  struct ptoc_Type147
    {
    tak_parskey uparsid;
    tsp_int2 ufirstsel;
    tsp_int2 usecondsel;
    tsp_int2 ufiller;
    unsigned char uis_kind;
    char uis_all;
    } tak_union_array[493];
typedef
  struct ptoc_Type148
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
  struct ptoc_Type149
    {
    tak_parskey cparsk;
    tsp_int2 cnextpars;
    tak_complex_infoset centryinfo;
    unsigned char cqual_jv_upd;
    } tak_complex_array[669];
typedef
  struct ptoc_Type150
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
  struct ptoc_Type151
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
  struct ptoc_Type152
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
  struct ptoc_Type153
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
    struct ptoc_Type154
      {
      tsp00_Uint1 socoldtabno;
      tsp00_Uint1 socselecttabno;
      tsp00_Uint1 socfields;
      tsp00_Uint1 socfill;
      } tc_tables[16];
    tak_all_corcolumns tc_columns;
    } tak_correcord;
typedef
  struct ptoc_Type155
    {
    tsp_int2 lpos;
    tsp_int2 llen;
    } tak_linkposinfo[2048];
typedef
  struct ptoc_Type156
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
  unsigned char tak_sysdir_state[1];
typedef
  tsp_moveobj *tak_moveobj_ptr;
typedef
  struct tak_cache_dir_entry *tak_cache_dir_e_ptr;
typedef
  struct ptoc_Type157
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
  struct ptoc_Type158
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
  struct ptoc_Type159
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
  struct ptoc_Type160
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
  struct ptoc_Type161
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
  struct ptoc_Type162
    {
    tsp_knl_identifier dbp_sysdba;
    tgg00_FileId dbp_tree;
    } tak_dbproc_array[5];
typedef
  struct ptoc_Type163
    {
    tsp_int2 dbc_cnt;
    tsp_int2 dbc_fil;
    tak_sysbufferaddress dbc_ptr;
    tak_dbproc_array dbc_cache;
    } tak_dbproc_cache;
typedef
  struct ptoc_Type164
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
  struct ptoc_Type165
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
  struct ptoc_Type166
    {
    tsp_int4 cst_top;
    tsp_moveobj_ptr cst_stack[16];
    } tak_cmd_stack;
typedef
  struct ptoc_Type167
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
  struct ptoc_Type168
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
  struct ptoc_Type169
    {
    tsp00_Int4 ai_uktId;
    tsp00_Int4 ai_badAllocCountDown;
    tak_AllocatorMonitorInfo ai_alloc_sys;
    tak_AllocatorMonitorInfo ai_alloc_usr;
    } tak_allocator_info;
typedef
  struct ptoc_Type170
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
  struct ptoc_Type171
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
  struct ptoc_Type172
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
  struct ptoc_Type173
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
  struct ptoc_Type174
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
  unsigned char expected_datetime_value;
typedef
  struct ptoc_Type175
    {
    int count;
    struct ptoc_Type176
      {
      char is_field;
      tsp_data_type datatype;
      tsp_int2 datalen;
      tsp_int1 datafrac;
      char filler;
      tsp_int2 inout_len;
      tsp_int4 pos;
      } elem[1024];
    } list_elem_info;
tsp_knl_identifier a01_i_new;
tsp_knl_identifier a01_i_old;
tsp_knl_identifier a01_il_b_identifier;
tgg_code_globals g01code;
char g01unicode;
int a01_dt_put_datatype_node();
int a01_call_put();
int a01_function_put();
int a05dfts_date_from_timestamp();
int a05tfts_time_from_timestamp();
int a05tsfdt_timestamp_from_dattim();
int a05identifier_get();
int a05timestamp_get();
int a05get_digit();
int a06get_username();
int a07_b_put_error();
short a07_return_code();
char a12dbfunc_exist();
int a12describe_param();
int a16inc_vdesc_cnt();
int a55realloc_parsinfo();
int a56_datatype();
int a61_search_table();
int a61_set_jump();
int a61_format_change();
int a61_is_group_field();
int a640factor();
int a640not_first_factor();
int a641check_datetime();
int a641s_literal_value();
int a641string_set_operator();
int a641stack_for_op_b_chr();
int a641translate_string_literal();
int a641u_literal_value();
int a651const_between_expression();
int a651in_const_expression();
int a651code_for_const_param_expr();
int a651value_calculate();
int a660construct_subtree();
int a683_output();
char g01userstackoverflow();
int g01mb_data_size();
int g03fdcheck_date();
int g03ftcheck_time();
int g10mv1();
int g10mv2();
int g10mvr();
int g10fil();
int g10filuni();
int s10fil();
int t01int4();
int t01messblock();
int t01moveobj();
int t01aptree();
int cccprint();
int ak65analyze_joinarr();
int ak65between_predicate();
int ak65boolean_term();
int ak65compare_predicate();
int ak65ne_handle();
int a65ch_format();
int ak65check_change();
int ak65diff_predicates();
int ak65fix_escape();
int ak65in_predicate();
int ak65new_colinfo();
int ak65oracle_typecheck();
int ak65predicate();
int ak65s_num_into_tree();
int ak65se_condition();
int ak65set_like_operator();
int ak65sub_op_set();
int a65subq_get_val();
int ak65try_like_optimization();
int a65_convert();
char a65_datatypes_ok();
int a65_look_for_datatypes();
int ak65_dt_look_for_user_def();
int a65_search_condition();
int a65_set_operator();
int ak65set_mass_cmp();
int a65not_first_term();
int a65_term();
char ak65is_datetime_arith();
int ak65set_op_datetime_arith();
int ak65not_first_val_expr();
int a65_val_expr();
char ak65only_jvalue_found();
char ak65outer_cond_found();
char ak65subq_se();

static
  tak_jcolpropset ptoc_Var0 = 
  {
  (unsigned char)'\0'
  };


int ak65analyze_joinarr(acv, dmli, first_node, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
int first_node;
int act_node;
    {
    int ix;
    int curr_n;
    tak_all_command_glob *ptoc_Var1;
    tak_dml_info *ptoc_Var2;
    tak_joinrec *ptoc_Var3;
    struct ptoc_Type110 *ptoc_Var4;
    int ptoc_Var5;
    struct ptoc_Type56 *ptoc_Var6;
    struct ptoc_Type111 *ptoc_Var7;
    struct tgg00_MessBlock *ptoc_Var8;
    tgg00_QualBuf *ptoc_Var9;
    struct ptoc_Type38 *ptoc_Var10;
    struct tgg00_MessBlock *ptoc_Var11;
    tgg00_QualBuf *ptoc_Var12;
    struct ptoc_Type38 *ptoc_Var13;
    tak_joinrec *ptoc_Var14;
    struct ptoc_Type110 *ptoc_Var15;
    cccprint("AK65  ak65analyz");
    ptoc_Var1 = &*acv;
    ptoc_Var2 = &*dmli;
    ptoc_Var3 = &ptoc_Var2->d_joins;
    ptoc_Var4 = &ptoc_Var3->jrc_joinarr[sql__setrang((int ) ptoc_Var3->jrc_cnt , 0 , 256)];
    if (!(char ) sql__setrang(ptoc_Var4->jo_no_join , 0 , 1))
        {
        if ((int ) ptoc_Var4->jo_recs[0].jop_tableno > 0)
            {
            if ((int ) ptoc_Var4->jo_recs[0].jop_tableno == (int ) ptoc_Var4->jo_recs[1].jop_tableno)
                {
                if ((char ) sql__setrang(ptoc_Var4->jo_recs[0].jop_outer_join , 0 , 1) || (char ) sql__setrang(ptoc_Var4->jo_recs[1].jop_outer_join , 0 , 1))
                    {
                    a07_b_put_error(&(*acv) , 6378 , 1);
                    }
                ptoc_Var4->jo_recs[0].jop_tableno = (unsigned char)'\0';
                ptoc_Var4->jo_recs[1].jop_tableno = (unsigned char)'\0';
                ptoc_Var4->jo_recs[0].jop_outer_join = 0;
                ptoc_Var4->jo_recs[1].jop_outer_join = 0;
                }
            else
                {
                if ((int ) ptoc_Var3->jrc_cnt == 256)
                    {
                    if ((int ) ptoc_Var4->jo_recs[1].jop_tableno != 0)
                        {
                        ix = 0;
                        while (ix < 256)
                            if ((int ) ptoc_Var3->jrc_joinarr[sql__setrang(ix , 0 , 256)].jo_recs[1].jop_tableno == 99)
                                {
                                ptoc_Var3->jrc_joinarr[sql__setrang(ix , 0 , 256)] = ptoc_Var3->jrc_joinarr[256];
                                ix = 32767;
                                }
                            else
                                {
                                ix = ix + 1;
                                }
                        if (ix != 32767)
                            {
                            a07_b_put_error(&(*acv) , 6462 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var1->a_ap_tree , "vak65.p" , 14127))[sql__setrang(act_node , 0 , 32767)].n_pos);
                            }
                        else
                            {
                            ptoc_Var3->jrc_joinarr[256].jo_recs[0].jop_tableno = (unsigned char)'\0';
                            ptoc_Var3->jrc_joinarr[256].jo_recs[1].jop_tableno = (unsigned char)'\0';
                            ptoc_Var3->jrc_joinarr[256].jo_recs[0].jop_outer_join = 0;
                            ptoc_Var3->jrc_joinarr[256].jo_recs[1].jop_outer_join = 0;
                            }
                        }
                    }
                else
                    {
                    t01int4(5 , "first_node^^" , first_node);
                    t01int4(5 , "act_nodepred" , act_node);
                    t01int4(5 , "jrc_cnt     " , (int ) ptoc_Var2->d_joins.jrc_cnt);
                    ptoc_Var2->d_joins.jrc_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var2->d_joins.jrc_cnt , -32768 , 32767 , "vak65.p" , 14144) , -32768 , 32767);
                    if ((char ) sql__setrang(ptoc_Var2->d_join , 0 , 1))
                        {
                        a683_output(5 , &(*dmli));
                        }
                    ptoc_Var2->d_joins.jrc_cnt = (short ) sql__setrang((short ) sql__pred((int ) ptoc_Var2->d_joins.jrc_cnt , -32768 , 32767 , "vak65.p" , 14149) , -32768 , 32767);
                    curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var1->a_ap_tree , "vak65.p" , 14150))[sql__setrang(act_node , 0 , 32767)].n_sa_level;
                    if (curr_n > 0)
                        {
                        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var1->a_ap_tree , "vak65.p" , 14153))[sql__setrang(curr_n , 0 , 32767)].n_symb == 44)
                            {
                            curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var1->a_ap_tree , "vak65.p" , 14155))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
                            }
                        }
                    if (curr_n > 0)
                        {
                        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var1->a_ap_tree , "vak65.p" , 14160))[sql__setrang(curr_n , 0 , 32767)].n_symb == 45)
                            {
                            curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var1->a_ap_tree , "vak65.p" , 14162))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
                            }
                        }
                    if (curr_n > 0 && (int ) ptoc_Var4->jo_recs[0].jop_tableno != 0 && (int ) ptoc_Var4->jo_recs[1].jop_tableno == 0 && ((ptoc_Var5 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var1->a_ap_tree , "vak65.p" , 14169))[sql__setrang(act_node , 0 , 32767)].n_symb) == 43 || ptoc_Var5 == 44 || ptoc_Var5 == 45) && ((char ) sql__setrang(ptoc_Var1->a_outer_join , 0 , 1) && (int ) ptoc_Var4->jo_op != 0 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var1->a_ap_tree , "vak65.p" , 14173))[sql__setrang(first_node , 0 , 32767)].n_symb == 4 && (int ) ptoc_Var4->jo_op == 109))
                        {
                        ptoc_Var6 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var1->a_ap_tree , "vak65.p" , 14177))[sql__setrang(curr_n , 0 , 32767)];
                        ptoc_Var7 = &ptoc_Var4->jo_recs[1];
                        ptoc_Var8 = &ptoc_Var1->a_mblock;
                        ptoc_Var9 = (tgg00_QualBuf *) sql__nil(ptoc_Var8->integer.C_4.mb_qual , "vak65.p" , 14178);
                        ptoc_Var10 = &(*(tgg00_StackList *) sql__nil(ptoc_Var8->integer.C_5.mb_st , "vak65.p" , 14179))[sql__setrang((int ) ptoc_Var4->jo_recs[0].jop_startstack + 1 , 1 , 32767) + -1];
                        if ((int ) ptoc_Var6->n_sa_level == 0 && (int ) ptoc_Var6->n_lo_level == 0)
                            {
                            if (((ptoc_Var5 = (int ) ptoc_Var6->n_symb) == 46 || ptoc_Var5 == 48 || ptoc_Var5 == 49 || ptoc_Var5 == 40 || ptoc_Var5 == 41 || ptoc_Var5 == 50) && (int ) ptoc_Var10->integer.C_1.etype == 22 && (int ) ptoc_Var4->jo_recs[0].jop_inoutlen != 0)
                                {
                                if ((char ) sql__setrang(ptoc_Var4->jo_recs[1].jop_outer_join , 0 , 1))
                                    {
                                    a07_b_put_error(&(*acv) , 6379 , ptoc_Var6->n_pos);
                                    }
                                else
                                    {
                                    ptoc_Var7->jop_tableno = (unsigned char)'c';
                                    ptoc_Var7->jop_fieldno = 0;
                                    ptoc_Var7->jop_startstack = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var4->jo_recs[0].jop_startstack , -32768 , 32767 , "vak65.p" , 14202) , -32768 , 32767);
                                    ptoc_Var7->jop_cntstack = 1;
                                    ptoc_Var7->jop_datatyp = (unsigned char ) sql__setrang(ptoc_Var4->jo_recs[0].jop_datatyp , 0 , 39);
                                    ptoc_Var7->jop_inoutlen = (short ) sql__setrang(ptoc_Var4->jo_recs[0].jop_inoutlen , -32768 , 32767);
                                    ptoc_Var7->jop_datalen = (short ) sql__setrang(ptoc_Var4->jo_recs[0].jop_datalen , -32768 , 32767);
                                    _ptoc_MOVE(unsigned char , 1, ptoc_Var7->jop_propset, ptoc_Var0);
                                    ptoc_Var7->jop_outer_join = 0;
                                    ptoc_Var7->jop_outpos = 0;
                                    ptoc_Var7->jop_multiplier = 2147483647;
                                    t01int4(5 , "value_found " , sql__ord((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var1->a_ap_tree , "vak65.p" , 14212))[sql__setrang(curr_n , 0 , 32767)].n_symb));
                                    }
                                }
                            }
                        }
                    else
                        {
                        if ((int ) ptoc_Var4->jo_recs[1].jop_startstack > 0 && (int ) ptoc_Var4->jo_recs[1].jop_startstack <= (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var1->a_mblock.integer.C_4.mb_qual , "vak65.p" , 14225)).integer.C_2.mfirst_free && (int ) ptoc_Var4->jo_recs[1].jop_tableno != 0)
                            {
                            ptoc_Var11 = &ptoc_Var1->a_mblock;
                            ptoc_Var12 = (tgg00_QualBuf *) sql__nil(ptoc_Var11->integer.C_4.mb_qual , "vak65.p" , 14228);
                            ptoc_Var13 = &(*(tgg00_StackList *) sql__nil(ptoc_Var11->integer.C_5.mb_st , "vak65.p" , 14229))[sql__setrang((int ) ptoc_Var4->jo_recs[1].jop_startstack , 1 , 32767) + -1];
                            if (1 && (ptoc_Var4->jo_recs[1].jop_propset[0] & 16) != 0 && sql__ord((int ) ptoc_Var13->integer.C_1.ecol_tab[1]) < 100 && ((ptoc_Var5 = (int ) ptoc_Var13->integer.C_1.etype) == 0 || ptoc_Var5 == 1 || ptoc_Var5 == 2 || ptoc_Var5 == 3))
                                {
                                ptoc_Var13->integer.C_1.ecol_tab[1] = (unsigned char ) sql__setrang((unsigned char ) (sql__ord((int ) ptoc_Var13->integer.C_1.ecol_tab[1]) + 100) , 0 , 255);
                                }
                            }
                        }
                    t01int4(5 , "@@@@@@@@@@@@" , (int ) ptoc_Var4->jo_recs[1].jop_startstack);
                    t01messblock(2 , "ak65analyze " , &ptoc_Var1->a_mblock);
                    if ((int ) ptoc_Var4->jo_recs[1].jop_tableno == 0)
                        {
                        ptoc_Var2->d_joins.jrc_cnt = (short ) sql__setrang((short ) sql__pred((int ) ptoc_Var2->d_joins.jrc_cnt , -32768 , 32767 , "vak65.p" , 14247) , -32768 , 32767);
                        t01int4(5 , "jcnt!!!!!!!!" , (int ) ptoc_Var2->d_joins.jrc_cnt);
                        }
                    else
                        {
                        if ((int ) ptoc_Var4->jo_recs[0].jop_inoutlen == 0 || (int ) ptoc_Var4->jo_recs[1].jop_inoutlen == 0)
                            {
                            a07_b_put_error(&(*acv) , 6320 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var1->a_ap_tree , "vak65.p" , 14255))[sql__setrang(act_node , 0 , 32767)].n_pos);
                            }
                        }
                    t01int4(5 , "jrc_jcnt    " , (int ) ptoc_Var2->d_joins.jrc_cnt);
                    ptoc_Var2->d_joins.jrc_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var2->d_joins.jrc_cnt , -32768 , 32767 , "vak65.p" , 14259) , -32768 , 32767);
                    if ((char ) sql__setrang(ptoc_Var2->d_join , 0 , 1))
                        {
                        a683_output(5 , &(*dmli));
                        }
                    ptoc_Var2->d_joins.jrc_cnt = (short ) sql__setrang((short ) sql__pred((int ) ptoc_Var2->d_joins.jrc_cnt , -32768 , 32767 , "vak65.p" , 14264) , -32768 , 32767);
                    ptoc_Var3->jrc_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var3->jrc_cnt , -32768 , 32767 , "vak65.p" , 14265) , -32768 , 32767);
                    ptoc_Var2->d_joins.jrc_joinarr[sql__setrang((int ) ptoc_Var3->jrc_cnt , 0 , 256)].jo_col_upd_stat = (unsigned char)'\0';
                    ptoc_Var14 = &ptoc_Var2->d_joins;
                    ptoc_Var15 = &ptoc_Var14->jrc_joinarr[sql__setrang((int ) ptoc_Var14->jrc_cnt , 0 , 256)];
                    ptoc_Var15->jo_no_join = 0;
                    ptoc_Var15->jo_partno = (unsigned char)'\001';
                    ptoc_Var15->jo_recs[0].jop_tableno = (unsigned char)'\0';
                    ptoc_Var15->jo_recs[1].jop_tableno = (unsigned char)'\0';
                    ptoc_Var15->jo_recs[0].jop_outer_join = 0;
                    ptoc_Var15->jo_recs[1].jop_outer_join = 0;
                    }
                }
            }
        }
    cccprint("<ak65analyze_joi");
    }



int ak65between_predicate(acv, dmli, colin, sel_start_n, first_node, operator, field_found, e)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int sel_start_n;
int first_node;
tgg_stack_op_type operator;
tsp_int2 field_found;
tgg_basis_error *e;
    {
    char nochange;
    char no_check;
    int startstack;
    int i;
    int sel_n;
    tsp_int2 dummy_iolen;
    tsp_int2 st_begin;
    tsp_int2 st_begin1;
    char both_const_expr;
    tsp_data_type check_type;
    tak00_colinfo_ptr helpbuf;
    tsp00_DataType keep_dtype[3];
    int node[3];
    char val_const[3];
    char cchange[3];
    int startstck[3];
    tak00_scolinf ccolin[3];
    tak_convert_type convert_t;
    tak_all_command_glob *ptoc_Var16;
    struct tgg00_MessBlock *ptoc_Var17;
    tgg00_QualBuf *ptoc_Var18;
    tak_dml_info *ptoc_Var19;
    int ptoc_Var20;
    struct ptoc_Type38 *ptoc_Var21;
    cccprint("AK65  ak65betwee");
    ptoc_Var16 = &*acv;
    ptoc_Var17 = &ptoc_Var16->a_mblock;
    ptoc_Var18 = (tgg00_QualBuf *) sql__nil(ptoc_Var17->integer.C_4.mb_qual , "vak65.p" , 14317);
    ptoc_Var19 = &*dmli;
    sel_n = sel_start_n;
    both_const_expr = 0;
    node[0] = first_node;
    no_check = 0;
    startstack = (int ) ptoc_Var18->integer.C_2.mfirst_free;
    val_const[0] = (char ) sql__setrang((ptoc_Var20 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var16->a_ap_tree , "vak65.p" , 14324))[sql__setrang(node[0] , 0 , 32767)].n_symb) == 48 || ptoc_Var20 == 198 || ptoc_Var20 == 26 || ptoc_Var20 == 111 || ptoc_Var20 == 145 || ptoc_Var20 == 46 , 0 , 1);
    check_type = 14;
    if ((int ) field_found > 0)
        {
        if (1 && ((*(tak00_columninfo *) sql__nil(ptoc_Var19->d_colbuf , "vak65.p" , 14331)).integer.C_1.ccolpropset[0] & 32) != 0)
            {
            ptoc_Var21 = &(*(tgg00_StackList *) sql__nil(ptoc_Var17->integer.C_5.mb_st , "vak65.p" , 14333))[sql__setrang((int ) field_found , 1 , 32767) + -1];
            ptoc_Var21->integer.C_1.ecol_tab[1] = (unsigned char ) sql__setrang((unsigned char ) (sql__ord((int ) ptoc_Var21->integer.C_1.ecol_tab[1]) + 100) , 0 , 255);
            }
        }
    helpbuf = ptoc_Var19->d_colptr;
    i = 1;
    cchange[sql__setrang(i , 1 , 3) + -1] = (char ) sql__setrang((ptoc_Var20 = (int ) ptoc_Var19->d_datatype) == 10 || ptoc_Var20 == 11 || ptoc_Var20 == 13 , 0 , 1);
    if ((char ) sql__setrang(cchange[sql__setrang(i , 1 , 3) + -1] , 0 , 1))
        {
        check_type = (unsigned char ) sql__setrang(ptoc_Var19->d_datatype , 0 , 39);
        }
    else
        {
        if (!(char ) sql__setrang(val_const[sql__setrang(i , 1 , 3) + -1] , 0 , 1))
            {
            no_check = 1;
            }
        }
    startstck[sql__setrang(i , 1 , 3) + -1] = startstack;
    keep_dtype[sql__setrang(i , 1 , 3) + -1] = (unsigned char ) sql__setrang(ptoc_Var19->d_datatype , 0 , 39);
    ccolin[sql__setrang(i , 1 , 3) + -1] = *colin;
    t01int4(5 , "node       1" , node[sql__setrang(i , 1 , 3) + -1]);
    t01int4(5 , "d_datatype 1" , sql__ord((int ) ptoc_Var19->d_datatype));
    t01int4(5 , "startstack 1" , startstck[sql__setrang(i , 1 , 3) + -1]);
    t01int4(5 , "colin.typ  1" , sql__ord((int ) ccolin[sql__setrang(i , 1 , 3) + -1].sci_typ));
    i = sql__succ(i , -2147483647 , 2147483647 , "vak65.p" , 14357);
    node[sql__setrang(i , 1 , 3) + -1] = sel_n;
    if (((ptoc_Var20 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var16->a_ap_tree , "vak65.p" , 14360))[sql__setrang(sel_n , 0 , 32767)].n_symb) == 11 || ptoc_Var20 == 10 || ptoc_Var20 == 118 || ptoc_Var20 == 2 || ptoc_Var20 == 64 || ptoc_Var20 == 65) && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var16->a_ap_tree , "vak65.p" , 14362))[sql__setrang(sel_n , 0 , 32767)].n_proc == 55 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var16->a_ap_tree , "vak65.p" , 14364))[sql__setrang(sel_n , 0 , 32767)].n_proc == 88 || (int ) check_type != 14)
        {
        ptoc_Var19->d_colptr = (tak00_columninfo *) (int *) 0;
        }
    (*colin).sci_len = 0;
    if ((char ) sql__setrang(g01unicode , 0 , 1) && ((ptoc_Var20 = (int ) ptoc_Var19->d_datatype) == 10 || ptoc_Var20 == 11 || ptoc_Var20 == 13) && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var16->a_ap_tree , "vak65.p" , 14372))[sql__setrang(sel_n , 0 , 32767)].n_symb == 48)
        {
        ptoc_Var19->d_datatype = 2;
        }
    else
        {
        ptoc_Var19->d_datatype = 14;
        }
    ptoc_Var19->d_change_date_time = 0;
    st_begin = (short ) sql__setrang(ptoc_Var18->integer.C_2.mfirst_free , -32768 , 32767);
    ptoc_Var19->d_single_expr = 1;
    a65_val_expr(&(*acv) , &(*dmli) , &(*colin) , sel_n);
    if ((char ) sql__setrang(ptoc_Var19->d_const_value_expr , 0 , 1))
        {
        if ((int ) ptoc_Var18->integer.C_2.mfirst_free - (int ) st_begin > 1)
            {
            if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var16->a_mblock.integer.C_5.mb_st , "vak65.p" , 14386))[sql__setrang((int ) st_begin , 1 , 32767) + -1].integer.C_1.ecol_tab[1] == 0)
                {
                ptoc_Var19->d_const_value_expr = 0;
                }
            }
        }
    both_const_expr = (char ) sql__setrang((char ) sql__setrang(ptoc_Var19->d_const_value_expr , 0 , 1) && (int ) ptoc_Var16->a_is_ddl == 0 , 0 , 1);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var16->a_return_segm , "vak65.p" , 14393)).integer.C_4.sp1r_returncode == 21000)
        {
        val_const[sql__setrang(i , 1 , 3) + -1] = 1;
        ptoc_Var19->d_datatype = (unsigned char ) sql__setrang(keep_dtype[sql__setrang(i + -1 , 1 , 3) + -1] , 0 , 39);
        (*(tsp1_segment *) sql__nil(ptoc_Var16->a_return_segm , "vak65.p" , 14398)).integer.C_4.sp1r_returncode = 0;
        ptoc_Var19->d_colptr = helpbuf;
        a65_val_expr(&(*acv) , &(*dmli) , &(*colin) , sel_n);
        }
    else
        {
        val_const[sql__setrang(i , 1 , 3) + -1] = (char ) sql__setrang((ptoc_Var20 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var16->a_ap_tree , "vak65.p" , 14403))[sql__setrang(node[sql__setrang(i , 1 , 3) + -1] , 0 , 32767)].n_symb) == 48 || ptoc_Var20 == 198 || ptoc_Var20 == 26 || ptoc_Var20 == 111 || ptoc_Var20 == 145 , 0 , 1);
        }
    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var16->a_ap_tree , "vak65.p" , 14408))[sql__setrang(sel_n , 0 , 32767)].n_symb == 43)
        {
        sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var16->a_ap_tree , "vak65.p" , 14410))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
        }
    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var16->a_ap_tree , "vak65.p" , 14412))[sql__setrang(sel_n , 0 , 32767)].n_symb == 44)
        {
        sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var16->a_ap_tree , "vak65.p" , 14414))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
        }
    sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var16->a_ap_tree , "vak65.p" , 14416))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
    nochange = 0;
    ptoc_Var19->d_single_expr = 0;
    dummy_iolen = 0;
    if (a65_datatypes_ok(&(*acv) , &(*dmli) , &keep_dtype[sql__setrang(i + -1 , 1 , 3) + -1] , &dummy_iolen , (unsigned char ) sql__setrang(ptoc_Var19->d_datatype , 0 , 39) , 0 , node[sql__setrang(i , 1 , 3) + -1] , (*(tak_ap_max_tree *) sql__nil(ptoc_Var16->a_ap_tree , "vak65.p" , 14423))[sql__setrang(node[sql__setrang(i , 1 , 3) + -1] , 0 , 32767)].n_pos , 1 , &convert_t))
        {
        ptoc_Var19->d_single_expr = 1;
        cchange[sql__setrang(i , 1 , 3) + -1] = (char ) sql__setrang((ptoc_Var20 = (int ) ptoc_Var19->d_datatype) == 10 || ptoc_Var20 == 11 || ptoc_Var20 == 13 , 0 , 1);
        if ((char ) sql__setrang(cchange[sql__setrang(i , 1 , 3) + -1] , 0 , 1) && (int ) check_type == 14)
            {
            check_type = (unsigned char ) sql__setrang(ptoc_Var19->d_datatype , 0 , 39);
            }
        else
            {
            if (!(char ) sql__setrang(cchange[sql__setrang(i , 1 , 3) + -1] , 0 , 1) && !(char ) sql__setrang(val_const[sql__setrang(i , 1 , 3) + -1] , 0 , 1))
                {
                no_check = 1;
                }
            }
        startstck[sql__setrang(i , 1 , 3) + -1] = (int ) ptoc_Var18->integer.C_2.mfirst_free;
        keep_dtype[sql__setrang(i , 1 , 3) + -1] = (unsigned char ) sql__setrang(ptoc_Var19->d_datatype , 0 , 39);
        ccolin[sql__setrang(i , 1 , 3) + -1] = *colin;
        t01int4(5 , "node       2" , node[sql__setrang(i , 1 , 3) + -1]);
        t01int4(5 , "d_datatype 2" , sql__ord((int ) ptoc_Var19->d_datatype));
        t01int4(5 , "startstack 2" , startstck[sql__setrang(i , 1 , 3) + -1]);
        t01int4(5 , "colin.typ  2" , sql__ord((int ) ccolin[sql__setrang(i , 1 , 3) + -1].sci_typ));
        i = sql__succ(i , -2147483647 , 2147483647 , "vak65.p" , 14444);
        node[sql__setrang(i , 1 , 3) + -1] = sel_n;
        if (((ptoc_Var20 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var16->a_ap_tree , "vak65.p" , 14446))[sql__setrang(sel_n , 0 , 32767)].n_symb) == 11 || ptoc_Var20 == 10 || ptoc_Var20 == 118 || ptoc_Var20 == 2 || ptoc_Var20 == 64 || ptoc_Var20 == 65) && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var16->a_ap_tree , "vak65.p" , 14448))[sql__setrang(sel_n , 0 , 32767)].n_proc == 55 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var16->a_ap_tree , "vak65.p" , 14450))[sql__setrang(sel_n , 0 , 32767)].n_proc == 88 || (int ) check_type != 14)
            {
            ptoc_Var19->d_colptr = (tak00_columninfo *) (int *) 0;
            }
        (*colin).sci_len = 0;
        if ((char ) sql__setrang(g01unicode , 0 , 1) && ((ptoc_Var20 = (int ) keep_dtype[0]) == 10 || ptoc_Var20 == 11 || ptoc_Var20 == 13) && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var16->a_ap_tree , "vak65.p" , 14458))[sql__setrang(sel_n , 0 , 32767)].n_symb == 48)
            {
            ptoc_Var19->d_datatype = 2;
            }
        else
            {
            ptoc_Var19->d_datatype = 14;
            }
        ptoc_Var19->d_change_date_time = 0;
        ptoc_Var19->d_param_st_index = 0;
        ptoc_Var19->d_param_st_begin = 0;
        st_begin1 = (short ) sql__setrang(ptoc_Var18->integer.C_2.mfirst_free , -32768 , 32767);
        a65_val_expr(&(*acv) , &(*dmli) , &(*colin) , sel_n);
        if ((char ) sql__setrang(ptoc_Var19->d_const_value_expr , 0 , 1))
            {
            if ((int ) ptoc_Var18->integer.C_2.mfirst_free - (int ) st_begin1 > 1)
                {
                if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var16->a_mblock.integer.C_5.mb_st , "vak65.p" , 14473))[sql__setrang((int ) st_begin1 , 1 , 32767) + -1].integer.C_1.ecol_tab[1] == 0)
                    {
                    ptoc_Var19->d_const_value_expr = 0;
                    }
                }
            }
        both_const_expr = (char ) sql__setrang((char ) sql__setrang(both_const_expr , 0 , 1) && (char ) sql__setrang(ptoc_Var19->d_const_value_expr , 0 , 1) , 0 , 1);
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var16->a_return_segm , "vak65.p" , 14480)).integer.C_4.sp1r_returncode == 21000)
            {
            val_const[sql__setrang(i , 1 , 3) + -1] = 1;
            ptoc_Var19->d_datatype = (unsigned char ) sql__setrang(keep_dtype[sql__setrang(i + -2 , 1 , 3) + -1] , 0 , 39);
            (*(tsp1_segment *) sql__nil(ptoc_Var16->a_return_segm , "vak65.p" , 14485)).integer.C_4.sp1r_returncode = 0;
            ptoc_Var19->d_colptr = helpbuf;
            a65_val_expr(&(*acv) , &(*dmli) , &(*colin) , sel_n);
            }
        else
            {
            val_const[sql__setrang(i , 1 , 3) + -1] = (char ) sql__setrang((ptoc_Var20 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var16->a_ap_tree , "vak65.p" , 14491))[sql__setrang(node[sql__setrang(i , 1 , 3) + -1] , 0 , 32767)].n_symb) == 48 || ptoc_Var20 == 198 || ptoc_Var20 == 26 || ptoc_Var20 == 111 || ptoc_Var20 == 145 , 0 , 1);
            }
        ptoc_Var19->d_single_expr = 0;
        if (a65_datatypes_ok(&(*acv) , &(*dmli) , &keep_dtype[sql__setrang(i + -2 , 1 , 3) + -1] , &dummy_iolen , (unsigned char ) sql__setrang(ptoc_Var19->d_datatype , 0 , 39) , 0 , node[sql__setrang(i , 1 , 3) + -1] , (*(tak_ap_max_tree *) sql__nil(ptoc_Var16->a_ap_tree , "vak65.p" , 14500))[sql__setrang(node[sql__setrang(i , 1 , 3) + -1] , 0 , 32767)].n_pos , 1 , &convert_t))
            {
            if (a65_datatypes_ok(&(*acv) , &(*dmli) , &keep_dtype[sql__setrang(i + -1 , 1 , 3) + -1] , &dummy_iolen , (unsigned char ) sql__setrang(ptoc_Var19->d_datatype , 0 , 39) , 0 , node[sql__setrang(i , 1 , 3) + -1] , (*(tak_ap_max_tree *) sql__nil(ptoc_Var16->a_ap_tree , "vak65.p" , 14504))[sql__setrang(node[sql__setrang(i , 1 , 3) + -1] , 0 , 32767)].n_pos , 0 , &convert_t))
                {
                keep_dtype[sql__setrang(i , 1 , 3) + -1] = (unsigned char ) sql__setrang(ptoc_Var19->d_datatype , 0 , 39);
                cchange[sql__setrang(i , 1 , 3) + -1] = (char ) sql__setrang((ptoc_Var20 = (int ) ptoc_Var19->d_datatype) == 10 || ptoc_Var20 == 11 || ptoc_Var20 == 13 , 0 , 1);
                if ((char ) sql__setrang(cchange[sql__setrang(i , 1 , 3) + -1] , 0 , 1) && (int ) check_type == 14)
                    {
                    check_type = (unsigned char ) sql__setrang(ptoc_Var19->d_datatype , 0 , 39);
                    }
                else
                    {
                    if (!(char ) sql__setrang(cchange[sql__setrang(i , 1 , 3) + -1] , 0 , 1) && !(char ) sql__setrang(val_const[sql__setrang(i , 1 , 3) + -1] , 0 , 1))
                        {
                        no_check = 1;
                        }
                    }
                ccolin[sql__setrang(i , 1 , 3) + -1] = *colin;
                t01int4(5 , "node       3" , node[sql__setrang(i , 1 , 3) + -1]);
                t01int4(5 , "d_datatype 3" , sql__ord((int ) ptoc_Var19->d_datatype));
                t01int4(5 , "colin.typ  3" , sql__ord((int ) ccolin[sql__setrang(i , 1 , 3) + -1].sci_typ));
                nochange = (char ) sql__setrang((char ) sql__setrang(cchange[0] , 0 , 1) && (char ) sql__setrang(cchange[1] , 0 , 1) && (char ) sql__setrang(cchange[2] , 0 , 1) , 0 , 1);
                nochange = (char ) sql__setrang(((char ) sql__setrang(cchange[0] , 0 , 1) || (char ) sql__setrang(cchange[1] , 0 , 1) || (char ) sql__setrang(cchange[2] , 0 , 1)) && !(char ) sql__setrang(nochange , 0 , 1) , 0 , 1);
                }
            }
        }
    else
        {
        ptoc_Var19->d_single_expr = 1;
        }
    if ((char ) sql__setrang(nochange , 0 , 1) && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var16->a_return_segm , "vak65.p" , 14538)).integer.C_4.sp1r_returncode == 0)
        {
        i = 0;
        *e = 0;
        do
            {
            i = sql__succ(i , -2147483647 , 2147483647 , "vak65.p" , 14545);
            if (i == 3)
                {
                startstck[sql__setrang(i , 1 , 3) + -1] = (int ) ptoc_Var18->integer.C_2.mfirst_free;
                }
            if ((char ) sql__setrang(no_check , 0 , 1))
                {
                if (!(char ) sql__setrang(cchange[sql__setrang(i , 1 , 3) + -1] , 0 , 1) && (char ) sql__setrang(val_const[sql__setrang(i , 1 , 3) + -1] , 0 , 1))
                    {
                    cchange[sql__setrang(i , 1 , 3) + -1] = (char ) sql__setrang(val_const[sql__setrang(i , 1 , 3) + -1] , 0 , 1);
                    }
                }
            else
                {
                if ((char ) sql__setrang(val_const[0] , 0 , 1) || (char ) sql__setrang(val_const[1] , 0 , 1) || (char ) sql__setrang(val_const[2] , 0 , 1))
                    {
                    cchange[sql__setrang(i , 1 , 3) + -1] = 0;
                    }
                }
            ak65check_change(&(*acv) , &(*dmli) , (unsigned char ) sql__setrang(check_type , 0 , 39) , (char ) sql__setrang(cchange[sql__setrang(i , 1 , 3) + -1] , 0 , 1) , &node[sql__setrang(i , 1 , 3) + -1] , &startstck[sql__setrang(i , 1 , 3) + -1] , &ccolin[sql__setrang(i , 1 , 3) + -1] , (unsigned char ) sql__setrang(keep_dtype[sql__setrang(i , 1 , 3) + -1] , 0 , 39) , (unsigned char ) sql__setrang(keep_dtype[0] , 0 , 39) , &(*e));
            if (i == 1)
                {
                startstck[sql__setrang(i + 1 , 1 , 3) + -1] = startstck[sql__setrang(i + 1 , 1 , 3) + -1] + startstck[sql__setrang(i , 1 , 3) + -1] - startstack;
                }
            }
        while (!((int ) *e != 0 || i == 3));
        }
    t01int4(5 , "operator    " , sql__ord((int ) operator));
    t01int4(5 , "first_node  " , first_node);
    if ((char ) sql__setrang(both_const_expr , 0 , 1) && (int ) ptoc_Var19->d_param_st_index != 0 && (int ) ptoc_Var18->integer.C_2.mfirst_free - (int ) st_begin > 2)
        {
        a651const_between_expression(&(*acv) , &(*dmli) , (int ) st_begin , (unsigned char ) sql__setrang(operator , 0 , 123));
        }
    else
        {
        a65_set_operator(&(*acv) , (unsigned char ) sql__setrang(operator , 0 , 123));
        }
    cccprint("<ak65between_pre");
    }



int ak65boolean_term(acv, dmli, first_node, mtabs_allowed, list_info)
tak_all_command_glob *acv;
tak_dml_info *dmli;
int *first_node;
char mtabs_allowed;
list_elem_info *list_info;
    {
    char snot_bool;
    int act_node;
    int oldjoincnt;
    int stentryno;
    tak_all_command_glob *ptoc_Var22;
    struct tgg00_MessBlock *ptoc_Var23;
    tgg00_QualBuf *ptoc_Var24;
    struct ptoc_Type56 *ptoc_Var25;
    tak_dml_info *ptoc_Var26;
    tak_joinrec *ptoc_Var27;
    struct ptoc_Type110 *ptoc_Var28;
    tak_joinrec *ptoc_Var29;
    struct ptoc_Type110 *ptoc_Var30;
    tak_joinrec *ptoc_Var31;
    struct ptoc_Type110 *ptoc_Var32;
    cccprint("AK65  ak65boolea");
    ptoc_Var22 = &*acv;
    ptoc_Var23 = &ptoc_Var22->a_mblock;
    ptoc_Var24 = (tgg00_QualBuf *) sql__nil(ptoc_Var23->integer.C_4.mb_qual , "vak65.p" , 14609);
    if (g01userstackoverflow())
        {
        a07_b_put_error(&(*acv) , 4500 , 1);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var22->a_return_segm , "vak65.p" , 14615)).integer.C_4.sp1r_returncode == 0)
        {
        act_node = *first_node;
        snot_bool = (char ) sql__setrang(ak65subq_se(&(*acv) , &(*first_node)) , 0 , 1);
        ptoc_Var25 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var22->a_ap_tree , "vak65.p" , 14620))[sql__setrang(*first_node , 0 , 32767)];
        if ((int ) ptoc_Var25->n_symb == 52)
            {
            if ((char ) sql__setrang(mtabs_allowed , 0 , 1))
                {
                (*dmli).d_first_tab = 0;
                }
            ak65se_condition(&(*acv) , &(*dmli) , &(*first_node) , 0 , &(*list_info));
            }
        else
            {
            if ((int ) ptoc_Var25->n_symb != 54)
                {
                ak65predicate(&(*acv) , &(*dmli) , &act_node , (char ) sql__setrang(mtabs_allowed , 0 , 1) , &(*list_info));
                snot_bool = 0;
                *first_node = act_node;
                }
            else
                {
                act_node = (int ) ptoc_Var25->n_lo_level;
                if ((char ) sql__setrang((*dmli).d_join , 0 , 1) && (char ) sql__setrang(snot_bool , 0 , 1))
                    {
                    ptoc_Var26 = &*dmli;
                    if ((char ) sql__setrang(mtabs_allowed , 0 , 1))
                        {
                        ptoc_Var26->d_first_tab = 0;
                        }
                    oldjoincnt = (int ) ptoc_Var26->d_joins.jrc_cnt;
                    ak65predicate(&(*acv) , &(*dmli) , &act_node , 0 , &(*list_info));
                    stentryno = (int ) ptoc_Var24->integer.C_2.mfirst_free;
                    if (stentryno >= (int ) ptoc_Var23->integer.C_2.mb_st_max)
                        {
                        a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var23->integer.C_2.mb_st_max);
                        }
                    else
                        {
                        ptoc_Var24->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var24->integer.C_2.mqual_cnt , -32768 , 32767 , "vak65.p" , 14661) , -32768 , 32767);
                        ptoc_Var24->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var24->integer.C_2.mfirst_free , -32768 , 32767 , "vak65.p" , 14662) , -32768 , 32767);
                        act_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var22->a_ap_tree , "vak65.p" , 14663))[sql__setrang(act_node , 0 , 32767)].n_sa_level;
                        ak65predicate(&(*acv) , &(*dmli) , &act_node , 0 , &(*list_info));
                        a65_set_operator(&(*acv) , 98);
                        a61_set_jump(&ptoc_Var22->a_mblock , stentryno , 32);
                        }
                    if ((int ) ptoc_Var26->d_joins.jrc_cnt > oldjoincnt)
                        {
                        ptoc_Var26->d_joins.jrc_cnt = (short ) sql__setrang((short ) oldjoincnt , -32768 , 32767);
                        ptoc_Var27 = &ptoc_Var26->d_joins;
                        ptoc_Var28 = &ptoc_Var27->jrc_joinarr[sql__setrang((int ) ptoc_Var27->jrc_cnt , 0 , 256)];
                        ptoc_Var28->jo_no_join = 0;
                        ptoc_Var28->jo_partno = (unsigned char)'\001';
                        ptoc_Var28->jo_recs[0].jop_tableno = (unsigned char)'\0';
                        ptoc_Var28->jo_recs[1].jop_tableno = (unsigned char)'\0';
                        }
                    }
                else
                    {
                    ak65predicate(&(*acv) , &(*dmli) , &act_node , (char ) sql__setrang(mtabs_allowed , 0 , 1) , &(*list_info));
                    stentryno = (int ) ptoc_Var24->integer.C_2.mfirst_free;
                    if (stentryno >= (int ) ptoc_Var23->integer.C_2.mb_st_max)
                        {
                        a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var23->integer.C_2.mb_st_max);
                        }
                    else
                        {
                        ptoc_Var24->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var24->integer.C_2.mqual_cnt , -32768 , 32767 , "vak65.p" , 14697) , -32768 , 32767);
                        ptoc_Var24->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var24->integer.C_2.mfirst_free , -32768 , 32767 , "vak65.p" , 14698) , -32768 , 32767);
                        act_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var22->a_ap_tree , "vak65.p" , 14699))[sql__setrang(act_node , 0 , 32767)].n_sa_level;
                        if ((char ) sql__setrang((*dmli).d_join , 0 , 1))
                            {
                            ptoc_Var29 = &(*dmli).d_joins;
                            ptoc_Var30 = &ptoc_Var29->jrc_joinarr[sql__setrang((int ) ptoc_Var29->jrc_cnt , 0 , 256)];
                            if ((char ) sql__setrang(ptoc_Var30->jo_no_join , 0 , 1))
                                {
                                ptoc_Var30->jo_no_join = 0;
                                ptoc_Var30->jo_partno = (unsigned char)'\001';
                                ptoc_Var30->jo_recs[0].jop_tableno = (unsigned char)'\0';
                                ptoc_Var30->jo_recs[1].jop_tableno = (unsigned char)'\0';
                                ptoc_Var30->jo_recs[0].jop_outer_join = 0;
                                ptoc_Var30->jo_recs[1].jop_outer_join = 0;
                                }
                            }
                        ak65predicate(&(*acv) , &(*dmli) , &act_node , (char ) sql__setrang(mtabs_allowed , 0 , 1) , &(*list_info));
                        a65_set_operator(&(*acv) , 98);
                        a61_set_jump(&ptoc_Var22->a_mblock , stentryno , 32);
                        if ((char ) sql__setrang((*dmli).d_join , 0 , 1))
                            {
                            ptoc_Var31 = &(*dmli).d_joins;
                            ptoc_Var32 = &ptoc_Var31->jrc_joinarr[sql__setrang((int ) ptoc_Var31->jrc_cnt , 0 , 256)];
                            if ((char ) sql__setrang(ptoc_Var32->jo_no_join , 0 , 1))
                                {
                                ptoc_Var32->jo_no_join = 0;
                                ptoc_Var32->jo_partno = (unsigned char)'\001';
                                ptoc_Var32->jo_recs[0].jop_tableno = (unsigned char)'\0';
                                ptoc_Var32->jo_recs[1].jop_tableno = (unsigned char)'\0';
                                ptoc_Var32->jo_recs[0].jop_outer_join = 0;
                                ptoc_Var32->jo_recs[1].jop_outer_join = 0;
                                }
                            }
                        }
                    }
                }
            }
        if ((char ) sql__setrang(snot_bool , 0 , 1))
            {
            a65_set_operator(&(*acv) , 96);
            }
        }
    cccprint("<ak65boolean_ter");
    }

static
  tak_jcolpropset ptoc_Var33 = 
  {
  (unsigned char)'\0'
  };
static
  tak_jcolpropset ptoc_Var34 = 
  {
  (unsigned char)'\0'
  };


int ak65compare_predicate(acv, dmli, colin, first_node, snot_pred, field_found, list_info)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int first_node;
char *snot_pred;
tsp_int2 field_found;
list_elem_info *list_info;
    {
    char any;
    char all;
    char s_special_expr;
    int sel_n;
    int expr_n;
    int cnt_expr;
    tsp_int2 dummy_iolen;
    tak_convert_type convert_t;
    tgg_stack_op_type operator;
    tsp_data_type check_type;
    int ix;
    tsp00_DataType keep_dtype[2];
    int startstck[2];
    int node[2];
    tak00_scolinf ccolin[2];
    char cchange[2];
    char nochange;
    char types_ok;
    tak00_colinfo_ptr helpbuf;
    tgg_basis_error e;
    char val_const;
    tsp_int2 level_node;
    tsp_int2 list_node;
    int list_cnt;
    tsp_data_type dm_datatype;
    tsp_int2 st_begin;
    tsp_int2 expr_st_cnt;
    tak_all_command_glob *ptoc_Var35;
    struct tgg00_MessBlock *ptoc_Var36;
    struct ptoc_Type56 *ptoc_Var37;
    tak_dml_info *ptoc_Var38;
    tak_joinrec *ptoc_Var39;
    struct ptoc_Type110 *ptoc_Var40;
    struct ptoc_Type111 *ptoc_Var41;
    struct ptoc_Type111 *ptoc_Var42;
    int ptoc_Var43;
    tak_joinrec *ptoc_Var44;
    struct ptoc_Type110 *ptoc_Var45;
    struct ptoc_Type176 *ptoc_Var46;
    struct ptoc_Type56 *ptoc_Var47;
    struct ptoc_Type56 *ptoc_Var48;
    tgg00_QualBuf *ptoc_Var49;
    struct ptoc_Type38 *ptoc_Var50;
    struct ptoc_Type56 *ptoc_Var51;
    struct ptoc_Type56 *ptoc_Var52;
    struct ptoc_Type38 *ptoc_Var53;
    tak_joinrec *ptoc_Var54;
    struct ptoc_Type110 *ptoc_Var55;
    struct ptoc_Type111 *ptoc_Var56;
    tgg00_QualBuf *ptoc_Var57;
    tak_joinrec *ptoc_Var58;
    struct ptoc_Type110 *ptoc_Var59;
    struct ptoc_Type111 *ptoc_Var60;
    tak00_scolinf *ptoc_Var61;
    tak_joinrec *ptoc_Var62;
    struct ptoc_Type110 *ptoc_Var63;
    struct ptoc_Type111 *ptoc_Var64;
    tgg00_QualBuf *ptoc_Var65;
    tak00_scolinf *ptoc_Var66;
    struct ptoc_Type38 *ptoc_Var67;
    cccprint("AK65  ak65compar");
    ptoc_Var35 = &*acv;
    ptoc_Var36 = &ptoc_Var35->a_mblock;
    ptoc_Var37 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 14793))[sql__setrang(first_node , 0 , 32767)];
    ptoc_Var38 = &*dmli;
    if ((char ) sql__setrang(*snot_pred , 0 , 1))
        {
        switch ((int ) ptoc_Var37->n_symb)
            {
            case 4 :
                operator = 115;
                break;
            case 5 :
                operator = 113;
                break;
            case 6 :
                operator = 114;
                break;
            case 8 :
                operator = 111;
                break;
            case 9 :
                operator = 112;
                break;
            case 16 :
                operator = 109;
                break;
            case 56 :
                operator = 102;
                break;
            case 55 :
                operator = 104;
                break;
            case 157 :
                operator = 107;
                break;
            case 159 :
                operator = 106;
                break;
            default :
                (unsigned char ) sql__caseerr("vak65.p" , 14797);
                break;
            }
        }
    else
        {
        switch ((int ) ptoc_Var37->n_symb)
            {
            case 4 :
                operator = 109;
                break;
            case 5 :
                operator = 112;
                break;
            case 6 :
                operator = 111;
                break;
            case 8 :
                operator = 114;
                break;
            case 9 :
                operator = 113;
                break;
            case 16 :
                operator = 115;
                break;
            case 56 :
                operator = 104;
                break;
            case 55 :
                operator = 102;
                break;
            case 157 :
                operator = 106;
                break;
            case 159 :
                operator = 107;
                break;
            default :
                (unsigned char ) sql__caseerr("vak65.p" , 14830);
                break;
            }
        }
    s_special_expr = (char ) sql__setrang(ptoc_Var35->a_special_expr , 0 , 1);
    if ((char ) sql__setrang(ptoc_Var38->d_join , 0 , 1) && ((char ) sql__setrang(ptoc_Var35->a_outer_join , 0 , 1) || (char ) sql__setrang(ptoc_Var38->d_outer_join , 0 , 1) || (char ) sql__setrang(ptoc_Var38->d_view , 0 , 1)))
        {
        ptoc_Var39 = &ptoc_Var38->d_joins;
        ptoc_Var40 = &ptoc_Var39->jrc_joinarr[sql__setrang((int ) ptoc_Var39->jrc_cnt , 0 , 256)];
        switch ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 14859))[sql__setrang(first_node , 0 , 32767)].n_subproc)
            {
            case 12 :
            case 13 :
            case 14 :
                ptoc_Var35->a_special_expr = 1;
                break;
            default :
                break;
            }
        t01int4(5 , "first_node  " , first_node);
        t01int4(5 , "jrc_cnt     " , (int ) ptoc_Var39->jrc_cnt);
        t01int4(5 , "acv_outer_jo" , sql__ord((int ) (char ) sql__setrang(ptoc_Var35->a_outer_join , 0 , 1)));
        t01int4(5 , "dm_outer_joi" , sql__ord((int ) (char ) sql__setrang(ptoc_Var38->d_outer_join , 0 , 1)));
        ptoc_Var41 = &ptoc_Var40->jo_recs[0];
        t01int4(5 , "jop_tablen 1" , (int ) ptoc_Var41->jop_tableno);
        ptoc_Var42 = &ptoc_Var40->jo_recs[1];
        t01int4(5 , "jop_tablen 2" , (int ) ptoc_Var42->jop_tableno);
        switch ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 14880))[sql__setrang(first_node , 0 , 32767)].n_subproc)
            {
            case 12 :
                t01int4(8 , "l OUTER JOIN" , 12);
                break;
            case 13 :
                t01int4(8 , "r OUTER JOIN" , 13);
                break;
            case 14 :
                t01int4(8 , "f OUTER JOIN" , 14);
                break;
            default :
                break;
            }
        if ((int ) ptoc_Var40->jo_recs[1].jop_tableno != 99)
            {
            switch ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 14894))[sql__setrang(first_node , 0 , 32767)].n_subproc)
                {
                case 12 :
                    ptoc_Var40->jo_recs[0].jop_outer_join = 1;
                    ptoc_Var40->jo_recs[1].jop_outer_join = 0;
                    break;
                case 13 :
                    ptoc_Var40->jo_recs[0].jop_outer_join = 0;
                    ptoc_Var40->jo_recs[1].jop_outer_join = 1;
                    break;
                case 14 :
                    ptoc_Var40->jo_recs[0].jop_outer_join = 1;
                    ptoc_Var40->jo_recs[1].jop_outer_join = 1;
                    break;
                default :
                    ptoc_Var40->jo_recs[0].jop_outer_join = 0;
                    ptoc_Var40->jo_recs[1].jop_outer_join = 0;
                    break;
                }
            }
        else
            {
            switch ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 14918))[sql__setrang(first_node , 0 , 32767)].n_subproc)
                {
                case 12 :
                case 13 :
                case 14 :
                    ptoc_Var40->jo_recs[0].jop_outer_join = 1;
                    ptoc_Var40->jo_recs[1].jop_outer_join = 0;
                    break;
                default :
                    ptoc_Var40->jo_recs[0].jop_outer_join = 0;
                    ptoc_Var40->jo_recs[1].jop_outer_join = 0;
                    break;
                }
            }
        ptoc_Var35->a_outer_join = (char ) sql__setrang((char ) sql__setrang(ptoc_Var35->a_outer_join , 0 , 1) || (char ) sql__setrang(ptoc_Var40->jo_recs[0].jop_outer_join , 0 , 1) || (char ) sql__setrang(ptoc_Var40->jo_recs[1].jop_outer_join , 0 , 1) , 0 , 1);
        if (!(char ) sql__setrang(ptoc_Var38->d_outer_join , 0 , 1))
            {
            ptoc_Var38->d_outer_join = (char ) sql__setrang(ptoc_Var35->a_outer_join , 0 , 1);
            }
        }
    val_const = 0;
    check_type = 14;
    ix = 1;
    sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 14947))[sql__setrang(first_node , 0 , 32767)].n_lo_level;
    node[sql__setrang(ix , 1 , 2) + -1] = sel_n;
    cchange[sql__setrang(ix , 1 , 2) + -1] = (char ) sql__setrang((ptoc_Var43 = (int ) ptoc_Var38->d_datatype) == 10 || ptoc_Var43 == 11 || ptoc_Var43 == 13 , 0 , 1);
    if ((char ) sql__setrang(cchange[sql__setrang(ix , 1 , 2) + -1] , 0 , 1))
        {
        check_type = (unsigned char ) sql__setrang(ptoc_Var38->d_datatype , 0 , 39);
        }
    startstck[sql__setrang(ix , 1 , 2) + -1] = (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var36->integer.C_4.mb_qual , "vak65.p" , 14954)).integer.C_2.mfirst_free;
    keep_dtype[sql__setrang(ix , 1 , 2) + -1] = (unsigned char ) sql__setrang(ptoc_Var38->d_datatype , 0 , 39);
    helpbuf = ptoc_Var38->d_colptr;
    ccolin[sql__setrang(ix , 1 , 2) + -1] = *colin;
    if ((int ) ptoc_Var38->d_datatype != 23 && ((ptoc_Var43 = (int ) ptoc_Var37->n_symb) == 159 || ptoc_Var43 == 157))
        {
        a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 14961))[sql__setrang(sel_n , 0 , 32767)].n_pos);
        }
    if ((ptoc_Var43 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 14963))[sql__setrang(node[sql__setrang(ix , 1 , 2) + -1] , 0 , 32767)].n_symb) == 48 || ptoc_Var43 == 198 || ptoc_Var43 == 26 || ptoc_Var43 == 111 || ptoc_Var43 == 145)
        {
        val_const = 1;
        }
    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 14969))[sql__setrang(sel_n , 0 , 32767)].n_symb == 43)
        {
        sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 14971))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
        }
    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 14973))[sql__setrang(sel_n , 0 , 32767)].n_symb == 44)
        {
        sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 14975))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
        }
    sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 14977))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
    t01int4(5 , "node       4" , node[sql__setrang(ix , 1 , 2) + -1]);
    t01int4(5 , "d_datatype 4" , sql__ord((int ) ptoc_Var38->d_datatype));
    t01int4(5 , "startstack 4" , startstck[sql__setrang(ix , 1 , 2) + -1]);
    t01int4(5 , "colin.typ  4" , sql__ord((int ) ccolin[sql__setrang(ix , 1 , 2) + -1].sci_typ));
    ix = sql__succ(ix , -2147483647 , 2147483647 , "vak65.p" , 14982);
    node[sql__setrang(ix , 1 , 2) + -1] = sel_n;
    st_begin = (short ) sql__setrang((*(tgg00_QualBuf *) sql__nil(ptoc_Var36->integer.C_4.mb_qual , "vak65.p" , 14984)).integer.C_2.mfirst_free , -32768 , 32767);
    ptoc_Var38->d_const_value_expr = (char ) sql__setrang((int ) ptoc_Var35->a_is_ddl == 0 , 0 , 1);
    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 14986))[sql__setrang(sel_n , 0 , 32767)].n_proc == 82 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 14988))[sql__setrang(sel_n , 0 , 32767)].n_proc == 54 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 14989))[sql__setrang(sel_n , 0 , 32767)].n_subproc == 13 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 14991))[sql__setrang(sel_n , 0 , 32767)].n_symb == 77 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 14993))[sql__setrang(sel_n , 0 , 32767)].n_symb == 78 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 14995))[sql__setrang(sel_n , 0 , 32767)].n_proc == 47 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 14996))[sql__setrang(sel_n , 0 , 32767)].n_subproc == 4)
        {
        if ((char ) sql__setrang(ptoc_Var38->d_join , 0 , 1))
            {
            ptoc_Var44 = &ptoc_Var38->d_joins;
            ptoc_Var45 = &ptoc_Var44->jrc_joinarr[sql__setrang((int ) ptoc_Var44->jrc_cnt , 0 , 256)];
            if ((char ) sql__setrang(ptoc_Var45->jo_no_join , 0 , 1))
                {
                ptoc_Var38->d_arith_where = 1;
                }
            }
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15008))[sql__setrang(sel_n , 0 , 32767)].n_symb == 77)
            {
            all = 1;
            any = 0;
            sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15013))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
            }
        else
            {
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15016))[sql__setrang(sel_n , 0 , 32767)].n_symb == 78)
                {
                all = 0;
                any = 1;
                sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15021))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
                }
            else
                {
                all = 0;
                any = 0;
                }
            }
        if ((char ) sql__setrang(*snot_pred , 0 , 1) && ((char ) sql__setrang(all , 0 , 1) || (char ) sql__setrang(any , 0 , 1)) && (*list_info).count > 1)
            {
            all = (char ) sql__setrang(!(char ) sql__setrang(all , 0 , 1) , 0 , 1);
            any = (char ) sql__setrang(!(char ) sql__setrang(any , 0 , 1) , 0 , 1);
            }
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15037))[sql__setrang(sel_n , 0 , 32767)].n_proc == 82 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15038))[sql__setrang(sel_n , 0 , 32767)].n_proc == 54 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15039))[sql__setrang(sel_n , 0 , 32767)].n_subproc == 13)
            {
            t01int4(5 , "d_use_sub 65" , sql__ord((int ) (char ) sql__setrang(ptoc_Var38->d_use_sub , 0 , 1)));
            t01int4(5 , "d_datatype  " , sql__ord((int ) ptoc_Var38->d_datatype));
            if (!(char ) sql__setrang(ptoc_Var38->d_use_sub , 0 , 1))
                {
                list_node = (short ) sql__setrang((short ) sel_n , -32768 , 32767);
                list_cnt = 0;
                while (list_cnt < (*list_info).count)
                    {
                    list_cnt = sql__succ(list_cnt , -2147483647 , 2147483647 , "vak65.p" , 15053);
                    if ((char ) sql__setrang(cchange[0] , 0 , 1))
                        {
                        a65ch_format(&(*acv) , &(*dmli) , &startstck[0] , &ccolin[0] , 2 , (unsigned char ) sql__setrang(ptoc_Var38->d_datatype , 0 , 39));
                        (*list_info).elem[sql__setrang(list_cnt , 1 , 1024) + -1].inout_len = (short ) sql__setrang((short ) sql__succ((int ) ccolin[0].sci_len , -32768 , 32767 , "vak65.p" , 15060) , -32768 , 32767);
                        }
                    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15064))[sql__setrang((int ) list_node , 0 , 32767)].n_sa_level > 0)
                        {
                        level_node = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15067))[sql__setrang((int ) list_node , 0 , 32767)].n_sa_level , -32768 , 32767);
                        dm_datatype = (unsigned char ) sql__setrang((*list_info).elem[sql__setrang(list_cnt , 1 , 1024) + -1].datatype , 0 , 39);
                        t01int4(5 , "d_datatype  " , sql__ord((int ) keep_dtype[sql__setrang(ix , 1 , 2) + -1]));
                        t01int4(5 , "dm_datatype " , sql__ord((int ) dm_datatype));
                        types_ok = (char ) sql__setrang(a65_datatypes_ok(&(*acv) , &(*dmli) , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15072))[sql__setrang((int ) level_node , 0 , 32767)].n_datatype , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15073))[sql__setrang((int ) level_node , 0 , 32767)].n_length , (unsigned char ) sql__setrang(dm_datatype , 0 , 39) , 1 , sel_n , (*list_info).elem[sql__setrang(list_cnt , 1 , 1024) + -1].pos , (char ) sql__setrang((*list_info).count == 1 , 0 , 1) , &convert_t) , 0 , 1);
                        if ((int ) convert_t != 0 && (*list_info).count != 1)
                            {
                            a07_b_put_error(&(*acv) , 6282 , (*list_info).elem[sql__setrang(list_cnt , 1 , 1024) + -1].pos);
                            }
                        }
                    else
                        {
                        ptoc_Var46 = &(*list_info).elem[sql__setrang(list_cnt , 1 , 1024) + -1];
                        a01_dt_put_datatype_node(&(*acv) , &level_node , (unsigned char ) sql__setrang(ptoc_Var46->datatype , 0 , 39) , (short ) sql__setrang(ptoc_Var46->datalen , -32768 , 32767) , (short ) sql__setrang((short ) ptoc_Var46->datafrac , -32768 , 32767) , (int ) ptoc_Var46->inout_len);
                        (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15090))[sql__setrang((int ) list_node , 0 , 32767)].n_sa_level = (short ) sql__setrang(level_node , -32768 , 32767);
                        }
                    ptoc_Var47 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15093))[sql__setrang((int ) level_node , 0 , 32767)];
                    t01int4(5 , "list#       " , list_cnt);
                    t01int4(5 , "type(list#) " , sql__ord((int ) ptoc_Var47->n_datatype));
                    t01int4(5 , "len(list#)  " , (int ) ptoc_Var47->n_length);
                    list_node = (short ) sql__setrang(level_node , -32768 , 32767);
                    }
                types_ok = 1;
                }
            else
                {
                list_node = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15107))[sql__setrang(sel_n , 0 , 32767)].n_sa_level , -32768 , 32767);
                list_cnt = 0;
                while ((int ) list_node != 0 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var35->a_return_segm , "vak65.p" , 15109)).integer.C_4.sp1r_returncode == 0)
                    {
                    ptoc_Var48 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15110))[sql__setrang((int ) list_node , 0 , 32767)];
                    list_cnt = sql__succ(list_cnt , -2147483647 , 2147483647 , "vak65.p" , 15112);
                    dm_datatype = (unsigned char ) sql__setrang((*list_info).elem[sql__setrang(list_cnt , 1 , 1024) + -1].datatype , 0 , 39);
                    t01int4(5 , "d_datatype  " , sql__ord((int ) keep_dtype[sql__setrang(ix , 1 , 2) + -1]));
                    t01int4(5 , "dm_datatype " , sql__ord((int ) dm_datatype));
                    types_ok = (char ) sql__setrang(a65_datatypes_ok(&(*acv) , &(*dmli) , &ptoc_Var48->n_datatype , &ptoc_Var48->n_length , (unsigned char ) sql__setrang(dm_datatype , 0 , 39) , 1 , sel_n , (*list_info).elem[sql__setrang(list_cnt , 1 , 1024) + -1].pos , (char ) sql__setrang((*list_info).count == 1 , 0 , 1) , &convert_t) , 0 , 1);
                    if ((char ) sql__setrang(types_ok , 0 , 1) && (char ) sql__setrang(cchange[0] , 0 , 1) && (*list_info).count == 1)
                        {
                        a65ch_format(&(*acv) , &(*dmli) , &startstck[0] , &ccolin[0] , (unsigned char ) sql__setrang(keep_dtype[0] , 0 , 39) , (unsigned char ) sql__setrang(dm_datatype , 0 , 39));
                        }
                    list_node = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15127))[sql__setrang((int ) list_node , 0 , 32767)].n_sa_level , -32768 , 32767);
                    }
                }
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var35->a_return_segm , "vak65.p" , 15133)).integer.C_4.sp1r_returncode == 0 && (char ) sql__setrang(types_ok , 0 , 1))
                {
                if ((int ) field_found > 0 && (int ) operator == 109)
                    {
                    if (1 && ((*(tak00_columninfo *) sql__nil(ptoc_Var38->d_colbuf , "vak65.p" , 15139)).integer.C_1.ccolpropset[0] & 32) != 0)
                        {
                        ptoc_Var49 = (tgg00_QualBuf *) sql__nil(ptoc_Var36->integer.C_4.mb_qual , "vak65.p" , 15141);
                        ptoc_Var50 = &(*(tgg00_StackList *) sql__nil(ptoc_Var36->integer.C_5.mb_st , "vak65.p" , 15141))[sql__setrang((int ) field_found , 1 , 32767) + -1];
                        ptoc_Var50->integer.C_1.ecol_tab[1] = (unsigned char ) sql__setrang((unsigned char ) (sql__ord((int ) ptoc_Var50->integer.C_1.ecol_tab[1]) + 100) , 0 , 255);
                        }
                    }
                ak65sub_op_set(&(*acv) , (char ) sql__setrang(all , 0 , 1) , (char ) sql__setrang(any , 0 , 1) , (unsigned char ) sql__setrang(operator , 0 , 123) , sel_n);
                }
            }
        else
            {
            expr_n = sel_n;
            t01int4(5 , "expr_n      " , expr_n);
            t01int4(5 , ".n_proc     " , sql__ord((int ) ptoc_Var37->n_proc));
            t01int4(5 , ".n_subproc  " , sql__ord((int ) ptoc_Var37->n_subproc));
            ptoc_Var51 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15157))[sql__setrang(expr_n , 0 , 32767)];
            if ((int ) ptoc_Var51->n_proc == 47 && (int ) ptoc_Var51->n_subproc == 4)
                {
                list_node = (short ) sql__setrang(ptoc_Var51->n_sa_level , -32768 , 32767);
                expr_n = (int ) ptoc_Var51->n_lo_level;
                list_cnt = 1;
                }
            else
                {
                list_cnt = 0;
                list_node = 0;
                }
            cnt_expr = 0;
            helpbuf = ptoc_Var38->d_colptr;
            do
                {
                (*colin).sci_len = 0;
                if (list_cnt > 0)
                    {
                    ptoc_Var38->d_datatype = (unsigned char ) sql__setrang((*list_info).elem[sql__setrang(list_cnt , 1 , 1024) + -1].datatype , 0 , 39);
                    ptoc_Var38->d_colptr = (tak00_columninfo *) (int *) 0;
                    }
                else
                    {
                    ptoc_Var38->d_colptr = helpbuf;
                    }
                a65_val_expr(&(*acv) , &(*dmli) , &(*colin) , (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15186))[sql__setrang(expr_n , 0 , 32767)].n_lo_level);
                cnt_expr = sql__succ(cnt_expr , -2147483647 , 2147483647 , "vak65.p" , 15187);
                expr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15188))[sql__setrang(expr_n , 0 , 32767)].n_sa_level;
                if (expr_n == 0 && (int ) list_node != 0)
                    {
                    ptoc_Var52 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15191))[sql__setrang((int ) list_node , 0 , 32767)];
                    expr_n = (int ) ptoc_Var52->n_lo_level;
                    list_node = (short ) sql__setrang(ptoc_Var52->n_sa_level , -32768 , 32767);
                    list_cnt = 1;
                    }
                else
                    {
                    if (list_cnt > 0 && expr_n > 0)
                        {
                        list_cnt = sql__succ(list_cnt , -2147483647 , 2147483647 , "vak65.p" , 15201);
                        }
                    }
                }
            while (!(expr_n == 0));
            ak65set_mass_cmp(&(*acv) , (unsigned char ) sql__setrang(operator , 0 , 123) , (char ) sql__setrang(all , 0 , 1) , cnt_expr , (*list_info).count);
            }
        }
    else
        {
        if ((int ) field_found > 0 && ((ptoc_Var43 = (int ) operator) == 109 || ptoc_Var43 == 115 || ptoc_Var43 == 104 || ptoc_Var43 == 112 || ptoc_Var43 == 111 || ptoc_Var43 == 114 || ptoc_Var43 == 113))
            {
            if (1 && ((*(tak00_columninfo *) sql__nil(ptoc_Var38->d_colbuf , "vak65.p" , 15216)).integer.C_1.ccolpropset[0] & 32) != 0)
                {
                ptoc_Var53 = &(*(tgg00_StackList *) sql__nil(ptoc_Var36->integer.C_5.mb_st , "vak65.p" , 15218))[sql__setrang((int ) field_found , 1 , 32767) + -1];
                ptoc_Var53->integer.C_1.ecol_tab[1] = (unsigned char ) sql__setrang((unsigned char ) (sql__ord((int ) ptoc_Var53->integer.C_1.ecol_tab[1]) + 100) , 0 , 255);
                }
            }
        if (((ptoc_Var43 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15223))[sql__setrang(sel_n , 0 , 32767)].n_symb) == 11 || ptoc_Var43 == 10 || ptoc_Var43 == 118 || ptoc_Var43 == 2 || ptoc_Var43 == 64 || ptoc_Var43 == 65) && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15225))[sql__setrang(sel_n , 0 , 32767)].n_proc == 55 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15227))[sql__setrang(sel_n , 0 , 32767)].n_proc == 88 || (int ) check_type != 14 && ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15230))[sql__setrang(sel_n , 0 , 32767)].n_proc != 0 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15231))[sql__setrang(sel_n , 0 , 32767)].n_symb != 31))
            {
            ptoc_Var38->d_colptr = (tak00_columninfo *) (int *) 0;
            }
        (*colin).sci_len = 0;
        if ((int ) ptoc_Var38->d_datatype != 24)
            {
            ptoc_Var38->d_change_date_time = 0;
            }
        if ((char ) sql__setrang(g01unicode , 0 , 1) && ((ptoc_Var43 = (int ) ptoc_Var38->d_datatype) == 10 || ptoc_Var43 == 11 || ptoc_Var43 == 13) && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15243))[sql__setrang(sel_n , 0 , 32767)].n_symb == 48)
            {
            ptoc_Var38->d_datatype = 2;
            }
        else
            {
            ptoc_Var38->d_datatype = 14;
            }
        if ((char ) sql__setrang(ptoc_Var38->d_join , 0 , 1))
            {
            ptoc_Var54 = &ptoc_Var38->d_joins;
            ptoc_Var55 = &ptoc_Var54->jrc_joinarr[sql__setrang((int ) ptoc_Var54->jrc_cnt , 0 , 256)];
            ptoc_Var56 = &ptoc_Var55->jo_recs[1];
            ptoc_Var57 = (tgg00_QualBuf *) sql__nil(ptoc_Var36->integer.C_4.mb_qual , "vak65.p" , 15251);
            ptoc_Var55->jo_op = (unsigned char ) sql__setrang(operator , 0 , 123);
            ptoc_Var56->jop_startstack = (short ) sql__setrang(ptoc_Var57->integer.C_2.mfirst_free , -32768 , 32767);
            if (((ptoc_Var43 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15255))[sql__setrang(sel_n , 0 , 32767)].n_symb) == 11 || ptoc_Var43 == 10 || ptoc_Var43 == 118 || ptoc_Var43 == 2 || ptoc_Var43 == 64 || ptoc_Var43 == 65) && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15257))[sql__setrang(sel_n , 0 , 32767)].n_proc == 55 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15259))[sql__setrang(sel_n , 0 , 32767)].n_proc == 88)
                {
                ptoc_Var38->d_colptr = (tak00_columninfo *) (int *) 0;
                }
            ptoc_Var55->jo_partno = (unsigned char)'\002';
            }
        a65_val_expr(&(*acv) , &(*dmli) , &(*colin) , sel_n);
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var35->a_return_segm , "vak65.p" , 15268)).integer.C_4.sp1r_returncode == 21000)
            {
            val_const = 1;
            ptoc_Var38->d_datatype = (unsigned char ) sql__setrang(keep_dtype[sql__setrang(ix + -1 , 1 , 2) + -1] , 0 , 39);
            (*(tsp1_segment *) sql__nil(ptoc_Var35->a_return_segm , "vak65.p" , 15273)).integer.C_4.sp1r_returncode = 0;
            ptoc_Var38->d_colptr = helpbuf;
            a65_val_expr(&(*acv) , &(*dmli) , &(*colin) , sel_n);
            }
        else
            {
            if ((ptoc_Var43 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15278))[sql__setrang(node[sql__setrang(ix , 1 , 2) + -1] , 0 , 32767)].n_symb) == 48 || ptoc_Var43 == 198 || ptoc_Var43 == 26 || ptoc_Var43 == 111 || ptoc_Var43 == 145)
                {
                val_const = 1;
                }
            }
        cchange[sql__setrang(ix , 1 , 2) + -1] = (char ) sql__setrang((ptoc_Var43 = (int ) ptoc_Var38->d_datatype) == 10 || ptoc_Var43 == 11 || ptoc_Var43 == 13 , 0 , 1);
        if ((char ) sql__setrang(cchange[sql__setrang(ix , 1 , 2) + -1] , 0 , 1))
            {
            if ((int ) check_type == 14)
                {
                check_type = (unsigned char ) sql__setrang(ptoc_Var38->d_datatype , 0 , 39);
                }
            else
                {
                if ((int ) check_type != (int ) ptoc_Var38->d_datatype)
                    {
                    a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak65.p" , 15295))[sql__setrang(node[sql__setrang(ix , 1 , 2) + -1] , 0 , 32767)].n_pos);
                    }
                }
            }
        keep_dtype[sql__setrang(ix , 1 , 2) + -1] = (unsigned char ) sql__setrang(ptoc_Var38->d_datatype , 0 , 39);
        ccolin[sql__setrang(ix , 1 , 2) + -1] = *colin;
        startstck[sql__setrang(ix , 1 , 2) + -1] = (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var36->integer.C_4.mb_qual , "vak65.p" , 15301)).integer.C_2.mfirst_free;
        t01int4(5 , "node       5" , node[sql__setrang(ix , 1 , 2) + -1]);
        t01int4(5 , "d_datatype 5" , sql__ord((int ) ptoc_Var38->d_datatype));
        t01int4(5 , "startstack 5" , startstck[sql__setrang(ix , 1 , 2) + -1]);
        t01int4(5 , "colin.typ  5" , sql__ord((int ) ccolin[sql__setrang(ix , 1 , 2) + -1].sci_typ));
        nochange = 0;
        nochange = (char ) sql__setrang((char ) sql__setrang(cchange[0] , 0 , 1) && (char ) sql__setrang(cchange[1] , 0 , 1) , 0 , 1);
        nochange = (char ) sql__setrang(((char ) sql__setrang(cchange[0] , 0 , 1) || (char ) sql__setrang(cchange[1] , 0 , 1)) && !(char ) sql__setrang(nochange , 0 , 1) , 0 , 1);
        dummy_iolen = 0;
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var35->a_return_segm , "vak65.p" , 15311)).integer.C_4.sp1r_returncode == 0)
            {
            if (!a65_datatypes_ok(&(*acv) , &(*dmli) , &keep_dtype[0] , &dummy_iolen , (unsigned char ) sql__setrang(keep_dtype[1] , 0 , 39) , 0 , node[1] , (*(tak_ap_max_tree *) sql__nil(ptoc_Var35->a_ap_tree , "vak65.p" , 15316))[sql__setrang(node[1] , 0 , 32767)].n_pos , (char ) sql__setrang(!(char ) sql__setrang(nochange , 0 , 1) , 0 , 1) , &convert_t))
                {
                nochange = 0;
                }
            }
        if ((int ) ptoc_Var38->d_datatype == 37 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var35->a_return_segm , "vak65.p" , 15322)).integer.C_4.sp1r_returncode == 0)
            {
            a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak65.p" , 15325))[sql__setrang(node[sql__setrang(ix , 1 , 2) + -1] , 0 , 32767)].n_pos);
            }
        if ((char ) sql__setrang(nochange , 0 , 1) && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var35->a_return_segm , "vak65.p" , 15327)).integer.C_4.sp1r_returncode == 0)
            {
            ix = 0;
            e = 0;
            do
                {
                ix = sql__succ(ix , -2147483647 , 2147483647 , "vak65.p" , 15334);
                if (ix == 2)
                    {
                    startstck[sql__setrang(ix , 1 , 2) + -1] = (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var36->integer.C_4.mb_qual , "vak65.p" , 15337)).integer.C_2.mfirst_free;
                    }
                if ((char ) sql__setrang(val_const , 0 , 1))
                    {
                    cchange[sql__setrang(ix , 1 , 2) + -1] = 0;
                    }
                ak65check_change(&(*acv) , &(*dmli) , (unsigned char ) sql__setrang(check_type , 0 , 39) , (char ) sql__setrang(cchange[sql__setrang(ix , 1 , 2) + -1] , 0 , 1) , &node[sql__setrang(ix , 1 , 2) + -1] , &startstck[sql__setrang(ix , 1 , 2) + -1] , &ccolin[sql__setrang(ix , 1 , 2) + -1] , (unsigned char ) sql__setrang(keep_dtype[sql__setrang(ix , 1 , 2) + -1] , 0 , 39) , (unsigned char ) sql__setrang(keep_dtype[0] , 0 , 39) , &e);
                }
            while (!((int ) e != 0 || ix == 2));
            if ((int ) convert_t != 0)
                {
                a65_convert(&(*acv) , (unsigned char ) sql__setrang(convert_t , 0 , 12) , (int ) ccolin[sql__setrang(ix , 1 , 2) + -1].sci_iolen);
                }
            if ((char ) sql__setrang(cchange[0] , 0 , 1) && (char ) sql__setrang(ptoc_Var38->d_join , 0 , 1) && ((ptoc_Var43 = (int ) operator) != 104 && ptoc_Var43 != 102))
                {
                ptoc_Var58 = &ptoc_Var38->d_joins;
                ptoc_Var59 = &ptoc_Var58->jrc_joinarr[sql__setrang((int ) ptoc_Var58->jrc_cnt , 0 , 256)];
                ptoc_Var60 = &ptoc_Var59->jo_recs[0];
                ptoc_Var60->jop_cntstack = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var60->jop_cntstack , -32768 , 32767 , "vak65.p" , 15358) , -32768 , 32767);
                _ptoc_MOVE(unsigned char , 1, ptoc_Var60->jop_propset, ptoc_Var33);
                ptoc_Var61 = &ccolin[0];
                ptoc_Var60->jop_datatyp = (unsigned char ) sql__setrang(ptoc_Var61->sci_typ , 0 , 39);
                if ((int ) ptoc_Var61->sci_iolen >= 1024 - sizeof(int ))
                    {
                    ptoc_Var60->jop_datalen = 0;
                    ptoc_Var60->jop_inoutlen = 0;
                    }
                else
                    {
                    ptoc_Var60->jop_datalen = (short ) sql__setrang(ptoc_Var61->sci_len , -32768 , 32767);
                    ptoc_Var60->jop_inoutlen = (short ) sql__setrang(ptoc_Var61->sci_iolen , -32768 , 32767);
                    }
                }
            }
        if ((char ) sql__setrang(ptoc_Var38->d_join , 0 , 1) && (char ) sql__setrang(ptoc_Var38->d_outer_join , 0 , 1) && ((ptoc_Var43 = (int ) operator) == 104 || ptoc_Var43 == 102) && !(char ) sql__setrang(ptoc_Var38->d_joins.jrc_joinarr[sql__setrang((int ) ptoc_Var38->d_joins.jrc_cnt , 0 , 256)].jo_recs[0].jop_outer_join , 0 , 1))
            {
            ptoc_Var38->d_arith_where = 1;
            }
        if ((char ) sql__setrang(ptoc_Var38->d_join , 0 , 1) && ((ptoc_Var43 = (int ) operator) != 104 && ptoc_Var43 != 102))
            {
            ptoc_Var62 = &ptoc_Var38->d_joins;
            ptoc_Var63 = &ptoc_Var62->jrc_joinarr[sql__setrang((int ) ptoc_Var62->jrc_cnt , 0 , 256)];
            ptoc_Var64 = &ptoc_Var63->jo_recs[1];
            ptoc_Var65 = (tgg00_QualBuf *) sql__nil(ptoc_Var36->integer.C_4.mb_qual , "vak65.p" , 15391);
            if ((char ) sql__setrang(ptoc_Var63->jo_no_join , 0 , 1))
                {
                ptoc_Var38->d_arith_where = 1;
                }
            else
                {
                if ((int ) ptoc_Var64->jop_tableno != 0 && (int ) ptoc_Var63->jo_recs[0].jop_tableno != 0 && !(char ) sql__setrang(ptoc_Var63->jo_no_join , 0 , 1))
                    {
                    if ((int ) ptoc_Var63->jo_recs[0].jop_inoutlen != 0)
                        {
                        ptoc_Var64->jop_cntstack = (short ) sql__setrang((short ) ((int ) ptoc_Var65->integer.C_2.mfirst_free - (int ) ptoc_Var64->jop_startstack) , -32768 , 32767);
                        ptoc_Var64->jop_outpos = 0;
                        if ((int ) ptoc_Var64->jop_cntstack > 1 || (ptoc_Var43 = (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var35->a_mblock.integer.C_5.mb_st , "vak65.p" , 15407))[sql__setrang((int ) ptoc_Var64->jop_startstack , 1 , 32767) + -1].integer.C_1.eop) != 0 && ptoc_Var43 != 28 && ptoc_Var43 != 29 && ptoc_Var43 != 19 && ptoc_Var43 != 20)
                            {
                            if ((int ) ptoc_Var64->jop_cntstack != 1 || (ptoc_Var43 = (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var35->a_mblock.integer.C_5.mb_st , "vak65.p" , 15413))[sql__setrang((int ) ptoc_Var64->jop_startstack , 1 , 32767) + -1].integer.C_1.eop) != 2 && ptoc_Var43 != 7)
                                {
                                _ptoc_MOVE(unsigned char , 1, ptoc_Var64->jop_propset, ptoc_Var34);
                                }
                            ptoc_Var66 = &ccolin[1];
                            switch ((int ) convert_t)
                                {
                                case 1 :
                                case 6 :
                                    ptoc_Var66->sci_typ = 3;
                                    break;
                                case 2 :
                                case 4 :
                                    ptoc_Var66->sci_typ = 2;
                                    break;
                                case 3 :
                                case 5 :
                                    ptoc_Var66->sci_typ = 24;
                                    ptoc_Var66->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var66->sci_len * 2 + 1) , -32768 , 32767);
                                    break;
                                default :
                                    break;
                                }
                            ptoc_Var64->jop_datatyp = (unsigned char ) sql__setrang(ptoc_Var66->sci_typ , 0 , 39);
                            if ((int ) ptoc_Var66->sci_iolen >= 1024 - sizeof(int ))
                                {
                                ptoc_Var64->jop_datalen = 0;
                                ptoc_Var64->jop_inoutlen = 0;
                                a07_b_put_error(&(*acv) , 6320 , 1);
                                }
                            else
                                {
                                ptoc_Var64->jop_datalen = (short ) sql__setrang(ptoc_Var66->sci_len , -32768 , 32767);
                                ptoc_Var64->jop_inoutlen = (short ) sql__setrang(ptoc_Var66->sci_iolen , -32768 , 32767);
                                }
                            if ((ptoc_Var43 = (int ) ptoc_Var64->jop_datatyp) == 0 || ptoc_Var43 == 1 || ptoc_Var43 == 12)
                                {
                                ptoc_Var64->jop_datatyp = 1;
                                ptoc_Var64->jop_datalen = 38;
                                ptoc_Var64->jop_inoutlen = 21;
                                }
                            }
                        }
                    else
                        {
                        a07_b_put_error(&(*acv) , 6320 , 1);
                        }
                    }
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var35->a_return_segm , "vak65.p" , 15475)).integer.C_4.sp1r_returncode == 0)
            {
            expr_st_cnt = (short ) sql__setrang((short ) ((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var36->integer.C_4.mb_qual , "vak65.p" , 15478)).integer.C_2.mfirst_free - (int ) st_begin) , -32768 , 32767);
            t01int4(5 , "st_begin 999" , (int ) st_begin);
            t01int4(5 , "expr_st_cnt " , (int ) expr_st_cnt);
            t01int4(5 , "d_param_st_i" , (int ) ptoc_Var38->d_param_st_index);
            t01int4(5 , "d_const_valu" , sql__ord((int ) (char ) sql__setrang(ptoc_Var38->d_const_value_expr , 0 , 1)));
            t01int4(5 , "true        " , sql__ord(1));
            t01int4(5 , "mb_data_len " , ptoc_Var35->a_mblock.integer.C_2.mb_data_len);
            t01int4(5 , "epos        " , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var35->a_mblock.integer.C_5.mb_st , "vak65.p" , 15486))[sql__setrang((int ) st_begin , 1 , 32767) + -1].integer.C_1.epos);
            if ((char ) sql__setrang(ptoc_Var38->d_const_value_expr , 0 , 1) && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var36->integer.C_5.mb_st , "vak65.p" , 15488))[sql__setrang((int ) st_begin , 1 , 32767) + -1].integer.C_1.etype == 22 && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var36->integer.C_5.mb_st , "vak65.p" , 15489))[sql__setrang((int ) st_begin , 1 , 32767) + -1].integer.C_1.ecol_tab[1] != 0)
                {
                (*(tgg00_StackList *) sql__nil(ptoc_Var36->integer.C_5.mb_st , "vak65.p" , 15491))[sql__setrang((int ) st_begin , 1 , 32767) + -1].integer.C_1.eop = (unsigned char ) sql__setrang(operator , 0 , 123);
                }
            else
                {
                a65_set_operator(&(*acv) , (unsigned char ) sql__setrang(operator , 0 , 123));
                }
            if (!(char ) sql__setrang(ptoc_Var35->a_rowno_allowed , 0 , 1) && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var35->a_return_segm , "vak65.p" , 15495)).integer.C_4.sp1r_returncode == 0)
                {
                ptoc_Var67 = &(*(tgg00_StackList *) sql__nil(ptoc_Var36->integer.C_5.mb_st , "vak65.p" , 15498))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var36->integer.C_4.mb_qual , "vak65.p" , 15498)).integer.C_2.mfirst_free + -1 , 1 , 32767) + -1];
                if ((ptoc_Var43 = (int ) ptoc_Var67->integer.C_1.eop) == 114 || ptoc_Var43 == 113)
                    {
                    if ((int ) ptoc_Var67->integer.C_1.eop == 113)
                        {
                        ptoc_Var38->d_rowno = (short ) sql__setrang(ptoc_Var67->integer.C_1.epos , -32768 , 32767);
                        }
                    else
                        {
                        ptoc_Var38->d_rowno = (short ) sql__setrang((short ) -(int ) ptoc_Var67->integer.C_1.epos , -32768 , 32767);
                        }
                    }
                (*(tgg00_StackList *) sql__nil(ptoc_Var36->integer.C_5.mb_st , "vak65.p" , 15513))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var36->integer.C_4.mb_qual , "vak65.p" , 15513)).integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.etype = 21;
                (*(tgg00_StackList *) sql__nil(ptoc_Var36->integer.C_5.mb_st , "vak65.p" , 15514))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var36->integer.C_4.mb_qual , "vak65.p" , 15514)).integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.eop = 0;
                (*(tgg00_StackList *) sql__nil(ptoc_Var36->integer.C_5.mb_st , "vak65.p" , 15515))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var36->integer.C_4.mb_qual , "vak65.p" , 15515)).integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.epos = 1;
                (*(tgg00_StackList *) sql__nil(ptoc_Var36->integer.C_5.mb_st , "vak65.p" , 15516))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var36->integer.C_4.mb_qual , "vak65.p" , 15516)).integer.C_2.mfirst_free + -2 , 1 , 32767) + -1].integer.C_1.etype = 39;
                (*(tgg00_StackList *) sql__nil(ptoc_Var36->integer.C_5.mb_st , "vak65.p" , 15517))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var36->integer.C_4.mb_qual , "vak65.p" , 15517)).integer.C_2.mfirst_free + -2 , 1 , 32767) + -1].integer.C_1.eop = 0;
                ptoc_Var36->integer.C_2.mb_data_len = ptoc_Var36->integer.C_2.mb_data_len - (int ) (*colin).sci_iolen + 7;
                ptoc_Var35->a_rowno_allowed = 1;
                }
            }
        if ((int ) operator == 115)
            {
            ak65ne_handle(&(*acv) , &(*dmli) , &(*colin) , sel_n);
            }
        }
    ptoc_Var35->a_special_expr = (char ) sql__setrang(s_special_expr , 0 , 1);
    *snot_pred = 0;
    cccprint("<ak65compare_pre");
    }



int ak65ne_handle(acv, dmli, colin, sel_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
tsp_int4 sel_node;
    {
    tak_all_command_glob *ptoc_Var68;
    struct tgg00_MessBlock *ptoc_Var69;
    tgg00_QualBuf *ptoc_Var70;
    int ptoc_Var71;
    cccprint("AK65  ak65ne_han");
    ptoc_Var68 = &*acv;
    ptoc_Var69 = &ptoc_Var68->a_mblock;
    ptoc_Var70 = (tgg00_QualBuf *) sql__nil(ptoc_Var69->integer.C_4.mb_qual , "vak65.p" , 15550);
    if ((int ) ptoc_Var70->integer.C_2.mfirst_free + 4 <= (int ) ptoc_Var69->integer.C_2.mb_st_max && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak65.p" , 15554))[sql__setrang((int ) ptoc_Var70->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.etype == 22 && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak65.p" , 15556))[sql__setrang((int ) ptoc_Var70->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.eop == 115 && ((ptoc_Var71 = (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak65.p" , 15558))[sql__setrang((int ) ptoc_Var70->integer.C_2.mfirst_free + -2 , 1 , 32767) + -1].integer.C_1.etype) == 0 || ptoc_Var71 == 1 || ptoc_Var71 == 2 || ptoc_Var71 == 3) && ((ptoc_Var71 = (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak65.p" , 15561))[sql__setrang((int ) ptoc_Var70->integer.C_2.mfirst_free + -2 , 1 , 32767) + -1].integer.C_1.eop) == 19 || ptoc_Var71 == 20 || ptoc_Var71 == 0) && ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak65.p" , 15564))[sql__setrang((int ) ptoc_Var70->integer.C_2.mfirst_free + -2 , 1 , 32767) + -1].integer.C_1.ecol_tab[0] > 0 || (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak65.p" , 15565))[sql__setrang((int ) ptoc_Var70->integer.C_2.mfirst_free + -2 , 1 , 32767) + -1].integer.C_1.ecol_tab[1] > 100 || ((ptoc_Var71 = (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak65.p" , 15566))[sql__setrang((int ) ptoc_Var70->integer.C_2.mfirst_free + -2 , 1 , 32767) + -1].integer.C_1.etype) == 0 || ptoc_Var71 == 1)))
        {
        (*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak65.p" , 15569))[sql__setrang((int ) ptoc_Var70->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.eop = 114;
        ptoc_Var70->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var70->integer.C_2.mfirst_free , -32768 , 32767 , "vak65.p" , 15570) , -32768 , 32767);
        (*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak65.p" , 15571))[sql__setrang((int ) ptoc_Var70->integer.C_2.mfirst_free , 1 , 32767) + -1] = (*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak65.p" , 15571))[sql__setrang((int ) ptoc_Var70->integer.C_2.mfirst_free + -3 , 1 , 32767) + -1];
        ptoc_Var70->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var70->integer.C_2.mfirst_free , -32768 , 32767 , "vak65.p" , 15572) , -32768 , 32767);
        (*colin).sci_len = 0;
        a640not_first_factor(&(*acv) , &(*dmli) , &(*colin) , &sel_node);
        (*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak65.p" , 15575))[sql__setrang((int ) ptoc_Var70->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.eop = 112;
        ptoc_Var70->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) ((int ) ptoc_Var70->integer.C_2.mqual_cnt + 2) , -32768 , 32767);
        a65_set_operator(&(*acv) , 99);
        a61_set_jump(&(*acv).a_mblock , (int ) ptoc_Var70->integer.C_2.mfirst_free + -4 , 31);
        }
    cccprint("<ak65ne_handle  ");
    }



int a65ch_format(acv, dmli, startstack, colin, left_type, d_type)
tak_all_command_glob *acv;
tak_dml_info *dmli;
int *startstack;
tak00_scolinf *colin;
tsp_data_type left_type;
tsp_data_type d_type;
    {
    int entry_cnt;
    tak_all_command_glob *ptoc_Var72;
    struct tgg00_MessBlock *ptoc_Var73;
    tgg00_QualBuf *ptoc_Var74;
    tak_dml_info *ptoc_Var75;
    struct ptoc_Type38 *ptoc_Var76;
    struct ptoc_Type38 *ptoc_Var77;
    cccprint("AK65  a65ch_form");
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak65.p" , 15598)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var72 = &*acv;
        ptoc_Var73 = &ptoc_Var72->a_mblock;
        ptoc_Var74 = (tgg00_QualBuf *) sql__nil(ptoc_Var73->integer.C_4.mb_qual , "vak65.p" , 15600);
        ptoc_Var75 = &*dmli;
        if ((int ) ptoc_Var72->a_dt_format == 6 && (int ) d_type == 13)
            {
            entry_cnt = 3;
            }
        else
            {
            entry_cnt = 1;
            }
        if ((int ) ptoc_Var74->integer.C_2.mfirst_free + entry_cnt + -1 > (int ) ptoc_Var73->integer.C_2.mb_st_max)
            {
            a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var73->integer.C_2.mb_st_max);
            }
        else
            {
            ptoc_Var72->a_date_time_used = 1;
            if ((int ) ptoc_Var72->a_dt_format != 1)
                {
                if (*startstack < (int ) ptoc_Var74->integer.C_2.mfirst_free)
                    {
                    g10mvr("VAK65 " , 1 , ptoc_Var73->integer.C_2.mb_st_size , ptoc_Var73->integer.C_2.mb_st_size , sql__nil(ptoc_Var73->integer.C_5.mb_st , "vak65.p" , 15621) , *startstack * 8 + -7 , sql__nil(ptoc_Var73->integer.C_5.mb_st , "vak65.p" , 15622) , (*startstack + entry_cnt) * 8 + -7 , ((int ) ptoc_Var74->integer.C_2.mfirst_free - *startstack) * 8 , &(*(tsp1_segment *) sql__nil(ptoc_Var72->a_return_segm , "vak65.p" , 15624)).integer.C_4.sp1r_returncode);
                    }
                a61_format_change(&(*acv) , &(*dmli) , *startstack , (unsigned char ) sql__setrang(d_type , 0 , 39) , &(*colin).sci_len);
                *startstack = *startstack + entry_cnt;
                }
            else
                {
                switch ((int ) d_type)
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
                        (unsigned char ) sql__caseerr("vak65.p" , 15631);
                        break;
                    }
                }
            if ((int ) g01code.ctype != 0 && (int ) left_type != 2)
                {
                if (*startstack < (int ) ptoc_Var74->integer.C_2.mfirst_free)
                    {
                    g10mvr("VAK65 " , 2 , ptoc_Var73->integer.C_2.mb_st_size , ptoc_Var73->integer.C_2.mb_st_size , sql__nil(ptoc_Var73->integer.C_5.mb_st , "vak65.p" , 15648) , *startstack * 8 + -7 , sql__nil(ptoc_Var73->integer.C_5.mb_st , "vak65.p" , 15649) , *startstack * 8 + 1 , ((int ) ptoc_Var74->integer.C_2.mfirst_free - *startstack) * 8 , &(*(tsp1_segment *) sql__nil(ptoc_Var72->a_return_segm , "vak65.p" , 15651)).integer.C_4.sp1r_returncode);
                    }
                ptoc_Var76 = &(*(tgg00_StackList *) sql__nil(ptoc_Var73->integer.C_5.mb_st , "vak65.p" , 15653))[sql__setrang(*startstack + -1 , 1 , 32767) + -1];
                if ((int ) ptoc_Var74->integer.C_2.mfirst_free > (int ) ptoc_Var73->integer.C_2.mb_st_max)
                    {
                    a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var73->integer.C_2.mb_st_max);
                    }
                else
                    {
                    if ((int ) ptoc_Var74->integer.C_2.mupd_cnt == 0)
                        {
                        ptoc_Var74->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var74->integer.C_2.mqual_cnt , -32768 , 32767 , "vak65.p" , 15661) , -32768 , 32767);
                        }
                    else
                        {
                        ptoc_Var74->integer.C_2.mupd_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var74->integer.C_2.mupd_cnt , -32768 , 32767 , "vak65.p" , 15663) , -32768 , 32767);
                        }
                    ptoc_Var77 = &(*(tgg00_StackList *) sql__nil(ptoc_Var73->integer.C_5.mb_st , "vak65.p" , 15665))[sql__setrang(*startstack , 1 , 32767) + -1];
                    ptoc_Var77->integer.C_1.etype = 26;
                    ptoc_Var77->integer.C_1.eop = 7;
                    ptoc_Var77->integer.C_1.epos = 1;
                    ptoc_Var77->integer.C_1.elen_var = 0;
                    ptoc_Var77->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                    ptoc_Var77->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                    ptoc_Var74->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var74->integer.C_2.mfirst_free , -32768 , 32767 , "vak65.p" , 15675) , -32768 , 32767);
                    *startstack = sql__succ(*startstack , -2147483647 , 2147483647 , "vak65.p" , 15676);
                    }
                (*colin).sci_typ = 3;
                ptoc_Var75->d_datatype = 3;
                }
            else
                {
                (*colin).sci_typ = 2;
                ptoc_Var75->d_datatype = 2;
                }
            }
        }
    cccprint("<a65ch_format   ");
    }



int ak65check_change(acv, dmli, c_type, change, act_node, startstack, colin, d_type, left_type, e)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tsp_data_type c_type;
char change;
int *act_node;
int *startstack;
tak00_scolinf *colin;
tsp_data_type d_type;
tsp_data_type left_type;
tgg_basis_error *e;
    {
    int alen;
    tsp_int4 curr_data_pos;
    tsp_c28 re_buf;
    tak_all_command_glob *ptoc_Var78;
    struct tgg00_MessBlock *ptoc_Var79;
    tak_dml_info *ptoc_Var80;
    int ptoc_Var81;
    tak_viewdescrecord *ptoc_Var82;
    struct ptoc_Type116 *ptoc_Var83;
    tgg00_DataPart *ptoc_Var84;
    cccprint("AK65  ak65check_");
    ptoc_Var78 = &*acv;
    ptoc_Var79 = &ptoc_Var78->a_mblock;
    ptoc_Var80 = &*dmli;
    if ((ptoc_Var81 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var78->a_ap_tree , "vak65.p" , 15713))[sql__setrang(*act_node , 0 , 32767)].n_symb) == 48 || ptoc_Var81 == 198 || ptoc_Var81 == 26 || ptoc_Var81 == 111 || ptoc_Var81 == 145)
        {
        alen = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var78->a_ap_tree , "vak65.p" , 15719))[sql__setrang(*act_node , 0 , 32767)].n_length;
        t01messblock(5 , "ak65check_c1" , &ptoc_Var78->a_mblock);
        t01int4(5 , "alen        " , alen);
        t01int4(5 , "startstack  " , *startstack);
        t01int4(5 , "c_type      " , sql__ord((int ) c_type));
        t01int4(5 , "n_datatype  " , sql__ord((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var78->a_ap_tree , "vak65.p" , 15725))[sql__setrang(*act_node , 0 , 32767)].n_datatype));
        if ((int ) ptoc_Var78->a_is_ddl == 20 && ((ptoc_Var81 = (int ) c_type) == 10 || ptoc_Var81 == 11 || ptoc_Var81 == 13))
            {
            a16inc_vdesc_cnt(&(*acv) , &(*dmli) , &ptoc_Var78->a_ptr10);
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var78->a_return_segm , "vak65.p" , 15731)).integer.C_4.sp1r_returncode == 0)
                {
                ptoc_Var82 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var78->a_ptr10 , "vak65.p" , 15733)).integer.C_46.sviewdesc;
                ptoc_Var83 = &ptoc_Var82->vdescription[sql__setrang((int ) ptoc_Var82->vdesc_cnt , 1 , 672) + -1];
                ptoc_Var83->vfromtabno = (short ) sql__setrang((short ) alen , -32768 , 32767);
                ptoc_Var83->vfromextcolno = 0;
                ptoc_Var83->vn_pos = (short ) sql__setrang((short ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var78->a_ap_tree , "vak65.p" , 15737))[sql__setrang(*act_node , 0 , 32767)].n_pos , -32768 , 32767);
                ptoc_Var83->vextcolno = 0;
                ptoc_Var83->vdatatype = (unsigned char ) sql__setrang(c_type , 0 , 39);
                }
            }
        if ((char ) sql__setrang(g01unicode , 0 , 1))
            {
            alen = alen / 2;
            }
        switch ((int ) c_type)
            {
            case 10 :
                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var78->a_ap_tree , "vak65.p" , 15752))[sql__setrang(*act_node , 0 , 32767)].n_datatype == 13)
                    {
                    a05dfts_date_from_timestamp((*(tgg00_DataPart *) sql__nil(ptoc_Var79->integer.C_3.mb_data , "vak65.p" , 15754)).integer.C_1.mbp_buf , re_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var79->integer.C_5.mb_st , "vak65.p" , 15755))[sql__setrang(*startstack + -1 , 1 , 32767) + -1].integer.C_1.epos + 1 , sizeof(unsigned char [28]) , 1 , alen , (unsigned char ) sql__setrang(ptoc_Var78->a_dt_format , 0 , 9) , ptoc_Var78->a_ak_language , 1 , &(*e));
                    }
                else
                    {
                    g03fdcheck_date((*(tgg00_DataPart *) sql__nil(ptoc_Var79->integer.C_3.mb_data , "vak65.p" , 15759)).integer.C_1.mbp_buf , re_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var79->integer.C_5.mb_st , "vak65.p" , 15760))[sql__setrang(*startstack + -1 , 1 , 32767) + -1].integer.C_1.epos + 1 , 1 , alen , (unsigned char ) sql__setrang(ptoc_Var78->a_dt_format , 0 , 9) , 1 , &(*e));
                    }
                alen = 9;
                break;
            case 11 :
                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var78->a_ap_tree , "vak65.p" , 15767))[sql__setrang(*act_node , 0 , 32767)].n_datatype == 13)
                    {
                    a05tfts_time_from_timestamp((*(tgg00_DataPart *) sql__nil(ptoc_Var79->integer.C_3.mb_data , "vak65.p" , 15769)).integer.C_1.mbp_buf , re_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var79->integer.C_5.mb_st , "vak65.p" , 15770))[sql__setrang(*startstack + -1 , 1 , 32767) + -1].integer.C_1.epos + 1 , sizeof(unsigned char [28]) , 1 , alen , (unsigned char ) sql__setrang(ptoc_Var78->a_dt_format , 0 , 9) , ptoc_Var78->a_ak_language , 1 , &(*e));
                    }
                else
                    {
                    g03ftcheck_time((*(tgg00_DataPart *) sql__nil(ptoc_Var79->integer.C_3.mb_data , "vak65.p" , 15774)).integer.C_1.mbp_buf , re_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var79->integer.C_5.mb_st , "vak65.p" , 15775))[sql__setrang(*startstack + -1 , 1 , 32767) + -1].integer.C_1.epos + 1 , 1 , alen , (unsigned char ) sql__setrang(ptoc_Var78->a_dt_format , 0 , 9) , 1 , &(*e));
                    }
                alen = 9;
                break;
            case 13 :
                if ((ptoc_Var81 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var78->a_ap_tree , "vak65.p" , 15782))[sql__setrang(*act_node , 0 , 32767)].n_datatype) == 10 || ptoc_Var81 == 11)
                    {
                    a05tsfdt_timestamp_from_dattim((*(tgg00_DataPart *) sql__nil(ptoc_Var79->integer.C_3.mb_data , "vak65.p" , 15784)).integer.C_1.mbp_buf , re_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var79->integer.C_5.mb_st , "vak65.p" , 15785))[sql__setrang(*startstack + -1 , 1 , 32767) + -1].integer.C_1.epos + 1 , 1 , sizeof(unsigned char [28]) , alen , (unsigned char ) sql__setrang(ptoc_Var78->a_dt_format , 0 , 9) , (unsigned char ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var78->a_ap_tree , "vak65.p" , 15787))[sql__setrang(*act_node , 0 , 32767)].n_datatype , 0 , 39) , 1 , &(*e));
                    }
                else
                    {
                    a05timestamp_get(&(*acv) , (*(tgg00_DataPart *) sql__nil(ptoc_Var79->integer.C_3.mb_data , "vak65.p" , 15790)).integer.C_1.mbp_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var79->integer.C_5.mb_st , "vak65.p" , 15791))[sql__setrang(*startstack + -1 , 1 , 32767) + -1].integer.C_1.epos , re_buf , 1 , alen , 1 , &(*e));
                    }
                alen = 21;
                break;
            default :
                break;
            }
        if ((int ) *e != 0)
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(*e , -32768 , 32767) , (*(tak_ap_max_tree *) sql__nil(ptoc_Var78->a_ap_tree , "vak65.p" , 15801))[sql__setrang(*act_node , 0 , 32767)].n_pos);
            }
        else
            {
            ptoc_Var84 = (tgg00_DataPart *) sql__nil(ptoc_Var79->integer.C_3.mb_data , "vak65.p" , 15803);
            if (ptoc_Var79->integer.C_2.mb_data_len + alen <= ptoc_Var79->integer.C_2.mb_data_size)
                {
                (*(tgg00_StackList *) sql__nil(ptoc_Var79->integer.C_5.mb_st , "vak65.p" , 15807))[sql__setrang(*startstack + -1 , 1 , 32767) + -1].integer.C_1.epos = (short ) sql__setrang((short ) (ptoc_Var79->integer.C_2.mb_data_len + 1) , -32768 , 32767);
                (*(tgg00_StackList *) sql__nil(ptoc_Var79->integer.C_5.mb_st , "vak65.p" , 15808))[sql__setrang(*startstack + -1 , 1 , 32767) + -1].integer.C_1.elen_var = (short ) sql__setrang((short ) alen , -32768 , 32767);
                ptoc_Var79->integer.C_2.mb_data_len = sql__succ(ptoc_Var79->integer.C_2.mb_data_len , -2147483647 , 2147483647 , "vak65.p" , 15809);
                curr_data_pos = ptoc_Var79->integer.C_2.mb_data_len;
                ptoc_Var84->integer.C_1.mbp_buf[sql__setrang(curr_data_pos , 1 , 8192000) + -1] = (unsigned char)' ';
                g10mv2("VAK65 " , 3 , sizeof(unsigned char [28]) , ptoc_Var79->integer.C_2.mb_data_size , re_buf , 1 , ptoc_Var84->integer.C_1.mbp_buf , curr_data_pos + 1 , alen + -1 , &(*(tsp1_segment *) sql__nil(ptoc_Var78->a_return_segm , "vak65.p" , 15815)).integer.C_4.sp1r_returncode);
                ptoc_Var79->integer.C_2.mb_data_len = ptoc_Var79->integer.C_2.mb_data_len + alen + -1;
                d_type = (unsigned char ) sql__setrang(c_type , 0 , 39);
                }
            else
                {
                a07_b_put_error(&(*acv) , 2801 , 1);
                }
            }
        }
    if ((char ) sql__setrang(change , 0 , 1) && (int ) *e == 0)
        {
        t01int4(5 , "format_chang" , *act_node);
        t01int4(5 , "startstack  " , *startstack);
        a65ch_format(&(*acv) , &(*dmli) , &(*startstack) , &(*colin) , (unsigned char ) sql__setrang(left_type , 0 , 39) , (unsigned char ) sql__setrang(d_type , 0 , 39));
        t01messblock(5 , "ak65check_c2" , &ptoc_Var78->a_mblock);
        }
    cccprint("<ak65check_chang");
    }

static
  tak_jcolpropset ptoc_Var85 = 
  {
  (unsigned char)'\0'
  };


int ak65diff_predicates(acv, dmli, colin, first_node, snot_pred, field_found, list_info)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int *first_node;
char *snot_pred;
tsp_int2 field_found;
list_elem_info *list_info;
    {
    char escape_char_found;
    char wrong_datatype;
    tsp_c2 c2;
    int keep_part_pos;
    int sel_n;
    int escape_n;
    int i;
    int strat_size;
    tsp_int2 dummy_iolen;
    tak00_colinfo_ptr helpbuf;
    tsp_data_type check_type;
    int startstack;
    tsp00_DataType keep_dtype[3];
    int startstck[3];
    int node[3];
    tak00_scolinf ccolin[3];
    char cchange[3];
    tak_convert_type convert_t;
    tgg_stack_op_type operator;
    char nochange;
    tgg_basis_error e;
    char val_const[3];
    char no_check;
    char dont_care;
    tak_all_command_glob *ptoc_Var86;
    struct tgg00_MessBlock *ptoc_Var87;
    tak_dml_info *ptoc_Var88;
    int ptoc_Var89;
    tak_joinrec *ptoc_Var90;
    struct ptoc_Type110 *ptoc_Var91;
    tak_joinrec *ptoc_Var92;
    struct ptoc_Type110 *ptoc_Var93;
    tgg00_QualBuf *ptoc_Var94;
    struct ptoc_Type56 *ptoc_Var95;
    struct ptoc_Type56 *ptoc_Var96;
    tgg00_QualBuf *ptoc_Var97;
    struct ptoc_Type38 *ptoc_Var98;
    tak_joinrec *ptoc_Var99;
    struct ptoc_Type110 *ptoc_Var100;
    struct ptoc_Type111 *ptoc_Var101;
    struct tgg00_MessBlock *ptoc_Var102;
    tgg00_QualBuf *ptoc_Var103;
    struct ptoc_Type110 *ptoc_Var104;
    tak00_scolinf *ptoc_Var105;
    cccprint("AK65  ak65diff_p");
    ptoc_Var86 = &*acv;
    ptoc_Var87 = &ptoc_Var86->a_mblock;
    ptoc_Var88 = &*dmli;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var86->a_return_segm , "vak65.p" , 15882)).integer.C_4.sp1r_returncode == 0)
        {
        strat_size = sizeof(struct ptoc_Type177
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
     struct ptoc_Type178
       {
       short skir_keystart[20];
       short skir_keystop[20];
       tsp00_Int2 skir_IN_SUBQ_stpos;
       tsp00_Int2 skir_inelemcnt;
       } str_key_in_range;
     } C_strat_key_equal;
   struct  
     {
     struct ptoc_Type179
       {
       short siir_keystart[20];
       short siir_keystop[20];
       short siir_invstart[20];
       short siir_invstop[20];
       tsp00_Int2 siir_startcnt;
       tsp00_Int2 siir_stopcnt;
       tsp00_PageNo siir_invroot;
       tsp00_Int2 siir_indexno;
       tsp00_Int2 siir_icount;
       unsigned char siir_invcoldesc[2];
       tsp00_Int2 siir_invlen;
       tsp00_Int2 siir_inelemcnt;
       tsp00_Int2 siir_subq_idx;
       tsp00_Int2 siir_out_invseqlen;
       tsp00_Int2 siir_IN_SUBQ_stpos;
       unsigned char siir_strat_props[1];
       tsp_c3 siir_filler;
       } str_inv_in_range;
     } C_strat_inv_range;
   struct  
     {
     struct ptoc_Type180
       {
       short sji_keystart[20];
       short sji_keystop[20];
       tsp00_PageNo sji_invroot;
       tsp00_Int2 sji_invindex;
       tsp00_Int2 sji_jfiller;
       } str_join_inv;
     } C_strat_join_inv;
   struct  
     {
     struct ptoc_Type181
       {
       short sjmf_keystart[20];
       short sjmf_keystop[20];
       tsp00_PageNo sjmf_invroot;
       tsp00_Int2 sjmf_index_no;
       tsp00_Int2 sjmf_cntfields;
       } str_join_multfields;
     } C_strat_join_all_keys_equal;
   struct  
     {
     struct ptoc_Type182
       {
       tsp00_Int2 sjv_lastkeylen;
       tsp00_Int2 sjv_minkeylen;
       } str_join_viewkey;
     } C_strat_join_viewkey;
   struct  
     {
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
       } str_catalog;
     } C_strat_catalog;
   struct  
     {
     tsp00_C1 str_raw[236];
     } C_strat_undecided;
   } tgg07_StratEnum;
 } );
        if ((char ) sql__setrang(*snot_pred , 0 , 1))
            {
            switch ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 15889))[sql__setrang(*first_node , 0 , 32767)].n_symb)
                {
                case 121 :
                    operator = 103;
                    break;
                case 122 :
                    operator = 105;
                    break;
                case 119 :
                    operator = 101;
                    break;
                case 120 :
                    operator = 100;
                    break;
                case 30 :
                    operator = 95;
                    break;
                case 29 :
                    operator = 94;
                    break;
                case 28 :
                    operator = 16;
                    break;
                case 27 :
                    operator = 11;
                    break;
                default :
                    operator = 0;
                    break;
                }
            *snot_pred = 0;
            }
        else
            {
            switch ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 15913))[sql__setrang(*first_node , 0 , 32767)].n_symb)
                {
                case 121 :
                    operator = 105;
                    break;
                case 122 :
                    operator = 103;
                    break;
                case 119 :
                    operator = 100;
                    break;
                case 120 :
                    operator = 101;
                    break;
                case 30 :
                    operator = 94;
                    break;
                case 29 :
                    operator = 95;
                    break;
                case 28 :
                    operator = 11;
                    break;
                case 27 :
                    operator = 16;
                    break;
                default :
                    operator = 0;
                    break;
                }
            }
        ptoc_Var88->d_const_value_expr = (char ) sql__setrang((int ) ptoc_Var86->a_is_ddl == 0 , 0 , 1);
        ptoc_Var88->d_param_st_index = 0;
        ptoc_Var88->d_param_st_begin = 0;
        no_check = 0;
        sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 15939))[sql__setrang(*first_node , 0 , 32767)].n_lo_level;
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 15940))[sql__setrang(sel_n , 0 , 32767)].n_proc == 55 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 15941))[sql__setrang(sel_n , 0 , 32767)].n_subproc == 15)
            {
            ptoc_Var88->d_escape = 1;
            escape_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 15945))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
            sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 15946))[sql__setrang(sel_n , 0 , 32767)].n_lo_level;
            }
        node[0] = sel_n;
        val_const[0] = (char ) sql__setrang((ptoc_Var89 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 15950))[sql__setrang(node[0] , 0 , 32767)].n_symb) == 48 || ptoc_Var89 == 198 || ptoc_Var89 == 26 || ptoc_Var89 == 111 || ptoc_Var89 == 145 || ptoc_Var89 == 46 , 0 , 1);
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 15954))[sql__setrang(sel_n , 0 , 32767)].n_symb == 43)
            {
            sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 15956))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
            }
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 15958))[sql__setrang(sel_n , 0 , 32767)].n_symb == 44)
            {
            sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 15960))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
            }
        sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 15962))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
        if ((char ) sql__setrang(ptoc_Var88->d_join , 0 , 1) && !(char ) sql__setrang(ptoc_Var88->d_escape , 0 , 1) && ((ptoc_Var89 = (int ) operator) == 105 || ptoc_Var89 == 103 || ptoc_Var89 == 100 || ptoc_Var89 == 101) && ((ptoc_Var89 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 15966))[sql__setrang(sel_n , 0 , 32767)].n_symb) == 43 || ptoc_Var89 == 44 || ptoc_Var89 == 45) && (!(char ) sql__setrang(ptoc_Var86->a_outer_join , 0 , 1) && !(char ) sql__setrang(ptoc_Var88->d_outer_join , 0 , 1)))
            {
            ptoc_Var90 = &ptoc_Var88->d_joins;
            ptoc_Var91 = &ptoc_Var90->jrc_joinarr[sql__setrang((int ) ptoc_Var90->jrc_cnt , 0 , 256)];
            ptoc_Var91->jo_op = (unsigned char ) sql__setrang(operator , 0 , 123);
            ptoc_Var92 = &ptoc_Var88->d_joins;
            ptoc_Var93 = &ptoc_Var92->jrc_joinarr[sql__setrang((int ) ptoc_Var92->jrc_cnt , 0 , 256)];
            ptoc_Var94 = (tgg00_QualBuf *) sql__nil(ptoc_Var87->integer.C_4.mb_qual , "vak65.p" , 15973);
            ptoc_Var93->jo_recs[1].jop_startstack = (short ) sql__setrang(ptoc_Var94->integer.C_2.mfirst_free , -32768 , 32767);
            ptoc_Var91->jo_partno = (unsigned char)'\002';
            }
        startstack = (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var87->integer.C_4.mb_qual , "vak65.p" , 15980)).integer.C_2.mfirst_free;
        ptoc_Var95 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 15981))[sql__setrang(*first_node , 0 , 32767)];
        switch ((int ) ptoc_Var95->n_symb)
            {
            case 30 :
            case 29 :
                ak65between_predicate(&(*acv) , &(*dmli) , &(*colin) , sel_n , *first_node , (unsigned char ) sql__setrang(operator , 0 , 123) , (short ) sql__setrang(field_found , -32768 , 32767) , &e);
                break;
            case 28 :
            case 27 :
                ak65in_predicate(&(*acv) , &(*dmli) , &sel_n , &operator , (short ) sql__setrang(field_found , -32768 , 32767) , &(*list_info));
                break;
            case 119 :
            case 120 :
                if ((int ) ptoc_Var88->d_datatype == 15 || (int ) ptoc_Var88->d_datatype == 18)
                    {
                    a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 15994))[sql__setrang(sel_n , 0 , 32767)].n_pos);
                    }
                else
                    {
                    i = 0;
                    check_type = 14;
                    i = sql__succ(i , -2147483647 , 2147483647 , "vak65.p" , 15999);
                    cchange[sql__setrang(i , 1 , 3) + -1] = (char ) sql__setrang((ptoc_Var89 = (int ) ptoc_Var88->d_datatype) == 10 || ptoc_Var89 == 11 || ptoc_Var89 == 13 , 0 , 1);
                    if ((char ) sql__setrang(cchange[sql__setrang(i , 1 , 3) + -1] , 0 , 1))
                        {
                        check_type = (unsigned char ) sql__setrang(ptoc_Var88->d_datatype , 0 , 39);
                        }
                    startstck[sql__setrang(i , 1 , 3) + -1] = startstack;
                    keep_dtype[sql__setrang(i , 1 , 3) + -1] = (unsigned char ) sql__setrang(ptoc_Var88->d_datatype , 0 , 39);
                    ccolin[sql__setrang(i , 1 , 3) + -1] = *colin;
                    t01int4(5 , "node       6" , node[sql__setrang(i , 1 , 3) + -1]);
                    t01int4(5 , "d_datatype 6" , sql__ord((int ) ptoc_Var88->d_datatype));
                    t01int4(5 , "startstack 6" , startstck[sql__setrang(i , 1 , 3) + -1]);
                    t01int4(5 , "colin.typ  6" , sql__ord((int ) ccolin[sql__setrang(i , 1 , 3) + -1].sci_typ));
                    ptoc_Var88->d_like = 1;
                    ptoc_Var88->d_like_optimize = 0;
                    keep_part_pos = ptoc_Var86->a_input_data_pos;
                    if (((ptoc_Var89 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16019))[sql__setrang(sel_n , 0 , 32767)].n_symb) == 11 || ptoc_Var89 == 10 || ptoc_Var89 == 118 || ptoc_Var89 == 2 || ptoc_Var89 == 64 || ptoc_Var89 == 65) && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16021))[sql__setrang(sel_n , 0 , 32767)].n_proc == 55 || ((ptoc_Var89 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16023))[sql__setrang(sel_n , 0 , 32767)].n_symb) == 43 || ptoc_Var89 == 44 || ptoc_Var89 == 45) || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16025))[sql__setrang(sel_n , 0 , 32767)].n_proc == 88)
                        {
                        ptoc_Var88->d_like_expression = 1;
                        }
                    escape_char_found = 0;
                    if ((char ) sql__setrang(ptoc_Var88->d_escape , 0 , 1))
                        {
                        ptoc_Var96 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16032))[sql__setrang(escape_n , 0 , 32767)];
                        if (((int ) ptoc_Var96->n_symb == 48 || (int ) ptoc_Var96->n_symb == 23) && !(char ) sql__setrang(ptoc_Var88->d_like_expression , 0 , 1))
                            {
                            wrong_datatype = 0;
                            if ((int ) ptoc_Var88->d_datatype == 24 && (char ) sql__setrang(g01unicode , 0 , 1))
                                {
                                a641u_literal_value(&(*acv) , escape_n , c2 , &wrong_datatype);
                                }
                            else
                                {
                                _ptoc_ASSIGN(unsigned char , 2, c2, "\0 ")
                                a641s_literal_value(&(*acv) , escape_n , (unsigned char ) sql__setrang(ptoc_Var88->d_datatype , 0 , 39) , 0 , &dont_care , &c2[1] , &wrong_datatype);
                                }
                            if ((char ) sql__setrang(wrong_datatype , 0 , 1))
                                {
                                a07_b_put_error(&(*acv) , 6564 , ptoc_Var96->n_pos);
                                }
                            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var86->a_return_segm , "vak65.p" , 16058)).integer.C_4.sp1r_returncode == 0)
                                {
                                if ((int ) ptoc_Var88->d_datatype != 24 && (int ) c2[1] != 255)
                                    {
                                    a641translate_string_literal(&c2[1] , &ptoc_Var88->d_datatype);
                                    }
                                }
                            _ptoc_ASSIGN(unsigned char , 2, ptoc_Var88->d_escape_char, c2)
                            escape_char_found = 1;
                            }
                        else
                            {
                            ptoc_Var88->d_like = 0;
                            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16072))[sql__setrang(escape_n , 0 , 32767)].n_symb != 46)
                                {
                                ptoc_Var88->d_like_expression = 1;
                                }
                            else
                                {
                                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16077))[sql__setrang(sel_n , 0 , 32767)].n_symb != 46)
                                    {
                                    ptoc_Var88->d_like_expression = 1;
                                    }
                                }
                            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16083))[sql__setrang(escape_n , 0 , 32767)].n_symb == 46)
                                {
                                ptoc_Var88->d_expand = 1;
                                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16088))[sql__setrang(sel_n , 0 , 32767)].n_symb == 46)
                                    {
                                    ptoc_Var88->d_like_expression = 0;
                                    }
                                }
                            helpbuf = ptoc_Var88->d_colptr;
                            ptoc_Var88->d_colptr = (tak00_columninfo *) (int *) 0;
                            ptoc_Var88->d_const_value_expr = (char ) sql__setrang(ptoc_Var88->d_like_expression , 0 , 1);
                            t01int4(5 , "a640factor  " , 1);
                            a640factor(&(*acv) , &(*dmli) , &(*colin) , &escape_n);
                            if (!(char ) sql__setrang(ptoc_Var88->d_like_expression , 0 , 1))
                                {
                                ptoc_Var97 = (tgg00_QualBuf *) sql__nil(ptoc_Var87->integer.C_4.mb_qual , "vak65.p" , 16103);
                                ptoc_Var97->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) ((int ) ptoc_Var97->integer.C_2.mqual_cnt + -1) , -32768 , 32767);
                                ptoc_Var97->integer.C_2.mfirst_free = (short ) sql__setrang((short ) ((int ) ptoc_Var97->integer.C_2.mfirst_free + -1) , -32768 , 32767);
                                }
                            ptoc_Var88->d_colptr = helpbuf;
                            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16111))[sql__setrang(escape_n , 0 , 32767)].n_symb == 46)
                                {
                                ptoc_Var88->d_expand = 0;
                                }
                            else
                                {
                                if ((int ) ptoc_Var88->d_datatype == 24)
                                    {
                                    a641string_set_operator(&(*acv) , 8 , 3 , 1 , (unsigned char)'\0' , (unsigned char)'\0');
                                    }
                                else
                                    {
                                    a641string_set_operator(&(*acv) , 8 , 2 , 1 , (unsigned char)'\0' , (unsigned char)'\0');
                                    }
                                }
                            ptoc_Var88->d_like = 1;
                            (*colin).sci_len = 0;
                            }
                        }
                    if ((char ) sql__setrang(ptoc_Var88->d_like_expression , 0 , 1))
                        {
                        if (((ptoc_Var89 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16140))[sql__setrang(sel_n , 0 , 32767)].n_symb) == 11 || ptoc_Var89 == 10 || ptoc_Var89 == 118 || ptoc_Var89 == 2 || ptoc_Var89 == 64 || ptoc_Var89 == 65) && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16142))[sql__setrang(sel_n , 0 , 32767)].n_proc == 55 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16144))[sql__setrang(sel_n , 0 , 32767)].n_proc == 88 || (int ) check_type != 14)
                            {
                            ptoc_Var88->d_colptr = (tak00_columninfo *) (int *) 0;
                            }
                        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16149))[sql__setrang(sel_n , 0 , 32767)].n_symb != 46)
                            {
                            (*colin).sci_len = 0;
                            ptoc_Var88->d_change_date_time = 0;
                            if ((char ) sql__setrang(g01unicode , 0 , 1) && ((ptoc_Var89 = (int ) ptoc_Var88->d_datatype) == 10 || ptoc_Var89 == 11 || ptoc_Var89 == 13) && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16157))[sql__setrang(sel_n , 0 , 32767)].n_symb == 48)
                                {
                                ptoc_Var88->d_datatype = 2;
                                }
                            else
                                {
                                ptoc_Var88->d_datatype = 14;
                                }
                            }
                        t01int4(5 , "a640factor  " , 2);
                        a640factor(&(*acv) , &(*dmli) , &(*colin) , &sel_n);
                        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var86->a_return_segm , "vak65.p" , 16167)).integer.C_4.sp1r_returncode == 21000)
                            {
                            (*(tsp1_segment *) sql__nil(ptoc_Var86->a_return_segm , "vak65.p" , 16170)).integer.C_4.sp1r_returncode = 0;
                            ptoc_Var88->d_datatype = (unsigned char ) sql__setrang(keep_dtype[0] , 0 , 39);
                            a640factor(&(*acv) , &(*dmli) , &(*colin) , &sel_n);
                            }
                        if ((char ) sql__setrang(ptoc_Var88->d_escape , 0 , 1) && ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16176))[sql__setrang(sel_n , 0 , 32767)].n_symb == 43 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16178))[sql__setrang(sel_n , 0 , 32767)].n_symb == 44 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16180))[sql__setrang(sel_n , 0 , 32767)].n_symb == 45 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16183))[sql__setrang(sel_n , 0 , 32767)].n_symb == 48 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16186))[sql__setrang(sel_n , 0 , 32767)].n_symb == 23 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16189))[sql__setrang(sel_n , 0 , 32767)].n_symb == 46))
                            {
                            a641string_set_operator(&(*acv) , 8 , (int ) (*colin).sci_iolen , (int ) (*colin).sci_len , (unsigned char)'\0' , (unsigned char)'\0');
                            }
                        }
                    else
                        {
                        if ((int ) field_found > 0 && (int ) operator == 100)
                            {
                            ptoc_Var98 = &(*(tgg00_StackList *) sql__nil(ptoc_Var87->integer.C_5.mb_st , "vak65.p" , 16208))[sql__setrang((int ) field_found , 1 , 32767) + -1];
                            if (1 && ((*(tak00_columninfo *) sql__nil(ptoc_Var88->d_colbuf , "vak65.p" , 16209)).integer.C_1.ccolpropset[0] & 32) != 0)
                                {
                                ptoc_Var98->integer.C_1.ecol_tab[1] = (unsigned char ) sql__setrang((unsigned char ) (sql__ord((int ) ptoc_Var98->integer.C_1.ecol_tab[1]) + 100) , 0 , 255);
                                if ((int ) ptoc_Var86->a_no_of_likes * (int ) (*(tak00_columninfo *) sql__nil(ptoc_Var88->d_colbuf , "vak65.p" , 16217)).integer.C_1.cinoutlen * 3 + strat_size + 1024 < g01mb_data_size())
                                    {
                                    ptoc_Var88->d_like_optimize = 1;
                                    }
                                }
                            else
                                {
                                if (((ptoc_Var89 = (int ) ptoc_Var98->integer.C_1.etype) == 0 || ptoc_Var89 == 1 || sql__ord((int ) ptoc_Var98->integer.C_1.ecol_tab[0]) > 0) && (int ) ptoc_Var86->a_no_of_likes * (int ) (*(tak00_columninfo *) sql__nil(ptoc_Var88->d_colbuf , "vak65.p" , 16233)).integer.C_1.cinoutlen * 3 + strat_size + 1024 < g01mb_data_size())
                                    {
                                    ptoc_Var88->d_like_optimize = 1;
                                    }
                                }
                            }
                        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16244))[sql__setrang(sel_n , 0 , 32767)].n_symb == 46 && (char ) sql__setrang(escape_char_found , 0 , 1))
                            {
                            ak65fix_escape(&(*acv) , &(*dmli));
                            }
                        t01int4(5 , "a640factor  " , 3);
                        if (((ptoc_Var89 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16250))[sql__setrang(sel_n , 0 , 32767)].n_symb) == 11 || ptoc_Var89 == 10 || ptoc_Var89 == 118 || ptoc_Var89 == 2 || ptoc_Var89 == 64 || ptoc_Var89 == 65) && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16252))[sql__setrang(sel_n , 0 , 32767)].n_proc == 55 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16254))[sql__setrang(sel_n , 0 , 32767)].n_proc == 88 || (int ) check_type != 14)
                            {
                            ptoc_Var88->d_colptr = (tak00_columninfo *) (int *) 0;
                            }
                        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16260))[sql__setrang(sel_n , 0 , 32767)].n_symb != 46)
                            {
                            (*colin).sci_len = 0;
                            ptoc_Var88->d_change_date_time = 0;
                            if ((char ) sql__setrang(g01unicode , 0 , 1) && ((ptoc_Var89 = (int ) ptoc_Var88->d_datatype) == 10 || ptoc_Var89 == 11 || ptoc_Var89 == 13) && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16268))[sql__setrang(sel_n , 0 , 32767)].n_symb == 48)
                                {
                                ptoc_Var88->d_datatype = 2;
                                }
                            else
                                {
                                ptoc_Var88->d_datatype = 14;
                                }
                            }
                        else
                            {
                            if ((char ) sql__setrang(cchange[sql__setrang(i , 1 , 3) + -1] , 0 , 1))
                                {
                                if ((int ) g01code.ctype != 0)
                                    {
                                    ptoc_Var88->d_datatype = 3;
                                    }
                                else
                                    {
                                    ptoc_Var88->d_datatype = 2;
                                    }
                                }
                            }
                        ptoc_Var88->d_like_part = (unsigned char)'\001';
                        a640factor(&(*acv) , &(*dmli) , &(*colin) , &sel_n);
                        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var86->a_return_segm , "vak65.p" , 16288)).integer.C_4.sp1r_returncode == 0 && (char ) sql__setrang(ptoc_Var88->d_like_optimize , 0 , 1))
                            {
                            ak65try_like_optimization(&(*acv) , &(*dmli) , &sel_n , (unsigned char ) sql__setrang(operator , 0 , 123) , (int ) ccolin[0].sci_iolen , keep_part_pos);
                            }
                        }
                    i = sql__succ(i , -2147483647 , 2147483647 , "vak65.p" , 16301);
                    node[sql__setrang(i , 1 , 3) + -1] = sel_n;
                    cchange[sql__setrang(i , 1 , 3) + -1] = (char ) sql__setrang((ptoc_Var89 = (int ) ptoc_Var88->d_datatype) == 10 || ptoc_Var89 == 11 || ptoc_Var89 == 13 , 0 , 1);
                    if ((char ) sql__setrang(cchange[sql__setrang(i , 1 , 3) + -1] , 0 , 1))
                        {
                        if ((int ) check_type == 14)
                            {
                            check_type = (unsigned char ) sql__setrang(ptoc_Var88->d_datatype , 0 , 39);
                            }
                        else
                            {
                            if ((int ) check_type != (int ) ptoc_Var88->d_datatype)
                                {
                                a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak65.p" , 16313))[sql__setrang(node[sql__setrang(i , 1 , 3) + -1] , 0 , 32767)].n_pos);
                                }
                            }
                        }
                    keep_dtype[sql__setrang(i , 1 , 3) + -1] = (unsigned char ) sql__setrang(ptoc_Var88->d_datatype , 0 , 39);
                    ccolin[sql__setrang(i , 1 , 3) + -1] = *colin;
                    startstck[sql__setrang(i , 1 , 3) + -1] = (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var87->integer.C_4.mb_qual , "vak65.p" , 16319)).integer.C_2.mfirst_free;
                    t01int4(5 , "node       7" , node[sql__setrang(i , 1 , 3) + -1]);
                    t01int4(5 , "d_datatype 7" , sql__ord((int ) ptoc_Var88->d_datatype));
                    t01int4(5 , "startstack 7" , startstck[sql__setrang(i , 1 , 3) + -1]);
                    t01int4(5 , "colin.typ  7" , sql__ord((int ) ccolin[sql__setrang(i , 1 , 3) + -1].sci_typ));
                    nochange = 0;
                    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var86->a_return_segm , "vak65.p" , 16325)).integer.C_4.sp1r_returncode == 0)
                        {
                        if (a65_datatypes_ok(&(*acv) , &(*dmli) , &keep_dtype[0] , &dummy_iolen , (unsigned char ) sql__setrang(keep_dtype[1] , 0 , 39) , 0 , node[1] , (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16330))[sql__setrang(node[1] , 0 , 32767)].n_pos , 1 , &convert_t))
                            {
                            nochange = (char ) sql__setrang((char ) sql__setrang(cchange[0] , 0 , 1) || (char ) sql__setrang(cchange[1] , 0 , 1) , 0 , 1);
                            }
                        }
                    if ((char ) sql__setrang(nochange , 0 , 1) && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var86->a_return_segm , "vak65.p" , 16336)).integer.C_4.sp1r_returncode == 0)
                        {
                        e = 0;
                        if ((char ) sql__setrang(val_const[0] , 0 , 1))
                            {
                            cchange[0] = 0;
                            cchange[1] = 0;
                            }
                        ak65check_change(&(*acv) , &(*dmli) , (unsigned char ) sql__setrang(check_type , 0 , 39) , (char ) sql__setrang(cchange[0] , 0 , 1) , &node[0] , &startstck[0] , &ccolin[0] , (unsigned char ) sql__setrang(keep_dtype[0] , 0 , 39) , (unsigned char ) sql__setrang(keep_dtype[0] , 0 , 39) , &e);
                        startstck[1] = (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var87->integer.C_4.mb_qual , "vak65.p" , 16351)).integer.C_2.mfirst_free;
                        if ((int ) e == 0 && (char ) sql__setrang(cchange[1] , 0 , 1))
                            {
                            a65ch_format(&(*acv) , &(*dmli) , &startstck[1] , &ccolin[1] , (unsigned char ) sql__setrang(keep_dtype[0] , 0 , 39) , (unsigned char ) sql__setrang(keep_dtype[1] , 0 , 39));
                            }
                        }
                    if ((char ) sql__setrang(ptoc_Var88->d_like , 0 , 1))
                        {
                        ptoc_Var88->d_like = 0;
                        ak65set_like_operator(&(*acv) , (unsigned char ) sql__setrang(operator , 0 , 123) , (char ) sql__setrang(ptoc_Var88->d_like_expression , 0 , 1) , sql__ord((int ) (*colin).sci_typ) , (char ) sql__setrang(ptoc_Var88->d_escape , 0 , 1));
                        ptoc_Var88->d_like_expression = 0;
                        }
                    ptoc_Var88->d_escape = 0;
                    }
                break;
            case 121 :
            case 122 :
                if ((int ) ptoc_Var88->d_datatype == 15 || (int ) ptoc_Var88->d_datatype == 10 || (int ) ptoc_Var88->d_datatype == 11 || (int ) ptoc_Var88->d_datatype == 18 || (int ) ptoc_Var88->d_datatype == 24)
                    {
                    a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16385))[sql__setrang(sel_n , 0 , 32767)].n_pos);
                    }
                else
                    {
                    if (((ptoc_Var89 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16388))[sql__setrang(sel_n , 0 , 32767)].n_symb) == 11 || ptoc_Var89 == 10 || ptoc_Var89 == 118 || ptoc_Var89 == 2 || ptoc_Var89 == 64 || ptoc_Var89 == 65) && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16390))[sql__setrang(sel_n , 0 , 32767)].n_proc == 55 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var86->a_ap_tree , "vak65.p" , 16392))[sql__setrang(sel_n , 0 , 32767)].n_proc == 88)
                        {
                        ptoc_Var88->d_colptr = (tak00_columninfo *) (int *) 0;
                        }
                    a640factor(&(*acv) , &(*dmli) , &(*colin) , &sel_n);
                    a65_set_operator(&(*acv) , (unsigned char ) sql__setrang(operator , 0 , 123));
                    }
                break;
            default :
                (unsigned char ) sql__caseerr("vak65.p" , 15982);
                break;
            }
        if ((char ) sql__setrang(ptoc_Var88->d_join , 0 , 1))
            {
            ptoc_Var99 = &ptoc_Var88->d_joins;
            ptoc_Var100 = &ptoc_Var99->jrc_joinarr[sql__setrang((int ) ptoc_Var99->jrc_cnt , 0 , 256)];
            ptoc_Var101 = &ptoc_Var100->jo_recs[1];
            ptoc_Var102 = &ptoc_Var86->a_mblock;
            ptoc_Var103 = (tgg00_QualBuf *) sql__nil(ptoc_Var102->integer.C_4.mb_qual , "vak65.p" , 16405);
            if ((int ) ptoc_Var100->jo_partno == 2)
                {
                if ((char ) sql__setrang(ptoc_Var100->jo_no_join , 0 , 1))
                    {
                    ptoc_Var88->d_arith_where = 1;
                    }
                else
                    {
                    if ((int ) ptoc_Var101->jop_tableno != 0 && (int ) ptoc_Var100->jo_recs[0].jop_tableno != 0 && !(char ) sql__setrang(ptoc_Var100->jo_no_join , 0 , 1))
                        {
                        if ((int ) ptoc_Var100->jo_recs[0].jop_inoutlen != 0)
                            {
                            ptoc_Var89 = (int ) ptoc_Var99->jrc_cnt + -1;
                            if (0 <= ptoc_Var89)
                                {
                                i = 0;
                                do
                                    {
                                    ptoc_Var104 = &ptoc_Var99->jrc_joinarr[sql__setrang(i , 0 , 256)];
                                    if ((int ) ptoc_Var104->jo_op == 100 || (int ) ptoc_Var104->jo_op == 101 || (int ) ptoc_Var104->jo_op == 105 || (int ) ptoc_Var104->jo_op == 103)
                                        {
                                        ptoc_Var99->jrc_joinarr[sql__setrang((int ) ptoc_Var99->jrc_cnt , 0 , 256)].jo_no_join = 1;
                                        ptoc_Var88->d_arith_where = 1;
                                        }
                                    if (i == ptoc_Var89)
                                        {
                                        break;
                                        }
                                    i += 1;
                                    }
                                while (!0);
                                }
                            if (!(char ) sql__setrang(ptoc_Var88->d_arith_where , 0 , 1))
                                {
                                if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var102->integer.C_5.mb_st , "vak65.p" , 16436))[sql__setrang((int ) ptoc_Var103->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.etype == 26)
                                    {
                                    ptoc_Var101->jop_cntstack = (short ) sql__setrang((short ) ((int ) ptoc_Var103->integer.C_2.mfirst_free - (int ) ptoc_Var101->jop_startstack + -1) , -32768 , 32767);
                                    }
                                else
                                    {
                                    ptoc_Var101->jop_cntstack = (short ) sql__setrang((short ) ((int ) ptoc_Var103->integer.C_2.mfirst_free - (int ) ptoc_Var101->jop_startstack) , -32768 , 32767);
                                    }
                                ptoc_Var101->jop_outpos = 0;
                                if ((int ) ptoc_Var101->jop_cntstack > 1 || (ptoc_Var89 = (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var102->integer.C_5.mb_st , "vak65.p" , 16447))[sql__setrang((int ) ptoc_Var101->jop_startstack , 1 , 32767) + -1].integer.C_1.eop) != 0 && ptoc_Var89 != 28 && ptoc_Var89 != 29 && ptoc_Var89 != 19 && ptoc_Var89 != 20 && ptoc_Var89 != 2 && ptoc_Var89 != 7)
                                    {
                                    _ptoc_MOVE(unsigned char , 1, ptoc_Var101->jop_propset, ptoc_Var85);
                                    ptoc_Var105 = &*colin;
                                    ptoc_Var101->jop_datatyp = (unsigned char ) sql__setrang(ptoc_Var105->sci_typ , 0 , 39);
                                    if ((int ) ptoc_Var105->sci_iolen >= 1024 - sizeof(int ))
                                        {
                                        a07_b_put_error(&(*acv) , 6320 , 1);
                                        }
                                    else
                                        {
                                        ptoc_Var101->jop_datalen = (short ) sql__setrang(ptoc_Var105->sci_len , -32768 , 32767);
                                        ptoc_Var101->jop_inoutlen = (short ) sql__setrang(ptoc_Var105->sci_iolen , -32768 , 32767);
                                        }
                                    if ((ptoc_Var89 = (int ) ptoc_Var101->jop_datatyp) == 0 || ptoc_Var89 == 1 || ptoc_Var89 == 12)
                                        {
                                        ptoc_Var101->jop_datatyp = 1;
                                        ptoc_Var101->jop_datalen = 38;
                                        ptoc_Var101->jop_inoutlen = 21;
                                        }
                                    }
                                }
                            }
                        else
                            {
                            a07_b_put_error(&(*acv) , 6320 , 1);
                            }
                        }
                    }
                }
            }
        }
    cccprint("<ak65diff_predic");
    }

typedef
  struct ptoc_Type145 tak_parsinforecord_ak65fix_escape;
static
  tak_fcolpropset ptoc_Var106 = 
  {
  (unsigned char)'\0'
  };


int ak65fix_escape(acv, dmli)
tak_all_command_glob *acv;
tak_dml_info *dmli;
    {
    tak_parsinforecord_ak65fix_escape *parsinfo_ptr;
    struct ptoc_Type144 *ptoc_Var107;
    cccprint("AK65  ak65fix_es");
    parsinfo_ptr = (tak_parsinforecord_ak65fix_escape *) &(*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.pparsp , "vak65.p" , 16510)).integer.C_23.sparsinfo;
    if ((int ) (*(tak_parsinforecord *) sql__nil(parsinfo_ptr , "vak65.p" , 16511)).integer.C_2.p_cnt_infos >= (int ) (*(tak_parsinforecord *) sql__nil(parsinfo_ptr , "vak65.p" , 16511)).integer.C_2.p_max_infos)
        {
        a55realloc_parsinfo(&(*acv) , &(*dmli).d_sparr.pparsp);
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak65.p" , 16515)).integer.C_4.sp1r_returncode == 0)
            {
            parsinfo_ptr = (tak_parsinforecord_ak65fix_escape *) &(*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.pparsp , "vak65.p" , 16517)).integer.C_23.sparsinfo;
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak65.p" , 16521)).integer.C_4.sp1r_returncode == 0)
        {
        (*(tak_parsinforecord *) sql__nil(parsinfo_ptr , "vak65.p" , 16524)).integer.C_2.p_cnt_infos = (short ) sql__setrang((short ) ((int ) (*(tak_parsinforecord *) sql__nil(parsinfo_ptr , "vak65.p" , 16524)).integer.C_2.p_cnt_infos + 1) , -32768 , 32767);
        ptoc_Var107 = &(*(tak_parsinforecord *) sql__nil(parsinfo_ptr , "vak65.p" , 16525)).integer.C_2.p_pars_infos[sql__setrang((int ) (*(tak_parsinforecord *) sql__nil(parsinfo_ptr , "vak65.p" , 16525)).integer.C_2.p_cnt_infos , 1 , 2025) + -1];
        ptoc_Var107->fp_kind = 5;
        ptoc_Var107->fp_etype = 39;
        ptoc_Var107->fp_datatyp = 14;
        _ptoc_MOVE(unsigned char , 1, ptoc_Var107->fp_colset, ptoc_Var106);
        ptoc_Var107->tak_fp_kind_type.C_fp_escape_val.fp_fill_101 = 0;
        ptoc_Var107->tak_fp_kind_type.C_fp_escape_val.fp_fill_102 = 0;
        ptoc_Var107->tak_fp_kind_type.C_fp_escape_val.fp_fill_103 = 0;
        _ptoc_ASSIGN(unsigned char , 2, ptoc_Var107->tak_fp_kind_type.C_fp_escape_val.fp_escape_char, (*dmli).d_escape_char)
        }
    cccprint("<ak65fix_escape ");
    }



int ak65in_predicate(acv, dmli, first_node, operator, field_found, list_info)
tak_all_command_glob *acv;
tak_dml_info *dmli;
int *first_node;
tgg_stack_op_type *operator;
tsp_int2 field_found;
list_elem_info *list_info;
    {
    int cnt_fac;
    int be_q_cnt;
    tak_convert_type convert_t;
    tak00_scolinf colin;
    tak00_colinfo_ptr help_buf;
    char types_ok;
    int startstack;
    tsp_int2 level_node;
    int list_node;
    char in_check;
    char in_strat;
    tsp_int2 first_entry;
    tsp_int2 first_free;
    int list_cnt;
    tsp_data_type dm_datatype;
    tak_all_command_glob *ptoc_Var108;
    struct tgg00_MessBlock *ptoc_Var109;
    tak_dml_info *ptoc_Var110;
    struct ptoc_Type176 *ptoc_Var111;
    struct ptoc_Type56 *ptoc_Var112;
    struct ptoc_Type56 *ptoc_Var113;
    int ptoc_Var114;
    tgg00_QualBuf *ptoc_Var115;
    struct ptoc_Type38 *ptoc_Var116;
    struct ptoc_Type38 *ptoc_Var117;
    struct ptoc_Type56 *ptoc_Var118;
    struct tgg00_MessBlock *ptoc_Var119;
    struct ptoc_Type56 *ptoc_Var120;
    struct tgg00_MessBlock *ptoc_Var121;
    tgg00_QualBuf *ptoc_Var122;
    struct ptoc_Type38 *ptoc_Var123;
    cccprint("AK65  ak65in_pre");
    ptoc_Var108 = &*acv;
    ptoc_Var109 = &ptoc_Var108->a_mblock;
    ptoc_Var110 = &*dmli;
    ptoc_Var110->d_const_value_expr = 0;
    ptoc_Var110->d_param_st_index = 0;
    ptoc_Var110->d_param_st_begin = 0;
    ptoc_Var110->d_single_expr = 1;
    first_entry = (short ) sql__setrang((*(tgg00_QualBuf *) sql__nil(ptoc_Var108->a_mblock.integer.C_4.mb_qual , "vak65.p" , 16574)).integer.C_2.mfirst_free , -32768 , 32767);
    cnt_fac = 0;
    in_check = 1;
    be_q_cnt = (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var108->a_mblock.integer.C_4.mb_qual , "vak65.p" , 16577)).integer.C_2.mqual_cnt;
    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16578))[sql__setrang(*first_node , 0 , 32767)].n_proc == 82 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16580))[sql__setrang(*first_node , 0 , 32767)].n_proc == 54 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16581))[sql__setrang(*first_node , 0 , 32767)].n_subproc == 13)
        {
        if ((int ) *operator == 16)
            {
            *operator = 115;
            }
        else
            {
            *operator = 109;
            }
        if (!(char ) sql__setrang(ptoc_Var110->d_use_sub , 0 , 1))
            {
            list_node = *first_node;
            list_cnt = 0;
            while (list_cnt < (*list_info).count)
                {
                list_cnt = sql__succ(list_cnt , -2147483647 , 2147483647 , "vak65.p" , 16599);
                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16601))[sql__setrang(list_node , 0 , 32767)].n_sa_level > 0)
                    {
                    level_node = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16604))[sql__setrang(list_node , 0 , 32767)].n_sa_level , -32768 , 32767);
                    dm_datatype = (unsigned char ) sql__setrang((*list_info).elem[sql__setrang(list_cnt , 1 , 1024) + -1].datatype , 0 , 39);
                    t01int4(5 , "dm_datatype " , sql__ord((int ) dm_datatype));
                    types_ok = (char ) sql__setrang(a65_datatypes_ok(&(*acv) , &(*dmli) , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16608))[sql__setrang((int ) level_node , 0 , 32767)].n_datatype , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16609))[sql__setrang((int ) level_node , 0 , 32767)].n_length , (unsigned char ) sql__setrang(dm_datatype , 0 , 39) , 1 , *first_node , (*list_info).elem[sql__setrang(list_cnt , 1 , 1024) + -1].pos , (char ) sql__setrang((*list_info).count == 1 , 0 , 1) , &convert_t) , 0 , 1);
                    if ((int ) convert_t != 0 && (*list_info).count != 1)
                        {
                        a07_b_put_error(&(*acv) , 6282 , (*list_info).elem[sql__setrang(list_cnt , 1 , 1024) + -1].pos);
                        }
                    }
                else
                    {
                    ptoc_Var111 = &(*list_info).elem[sql__setrang(list_cnt , 1 , 1024) + -1];
                    a01_dt_put_datatype_node(&(*acv) , &level_node , (unsigned char ) sql__setrang(ptoc_Var111->datatype , 0 , 39) , (short ) sql__setrang(ptoc_Var111->datalen , -32768 , 32767) , (short ) sql__setrang((short ) ptoc_Var111->datafrac , -32768 , 32767) , (int ) ptoc_Var111->inout_len);
                    (*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16626))[sql__setrang(list_node , 0 , 32767)].n_sa_level = (short ) sql__setrang(level_node , -32768 , 32767);
                    }
                ptoc_Var112 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16629))[sql__setrang((int ) level_node , 0 , 32767)];
                t01int4(5 , "list#       " , list_cnt);
                t01int4(5 , "type(list#) " , sql__ord((int ) ptoc_Var112->n_datatype));
                t01int4(5 , "len(list#)  " , (int ) ptoc_Var112->n_length);
                list_node = (int ) level_node;
                }
            types_ok = 1;
            }
        else
            {
            list_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16643))[sql__setrang(*first_node , 0 , 32767)].n_sa_level;
            list_cnt = 0;
            while (list_node != 0 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var108->a_return_segm , "vak65.p" , 16645)).integer.C_4.sp1r_returncode == 0)
                {
                ptoc_Var113 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16646))[sql__setrang(list_node , 0 , 32767)];
                t01int4(5 , "dtype_node  " , list_node);
                list_cnt = sql__succ(list_cnt , -2147483647 , 2147483647 , "vak65.p" , 16649);
                dm_datatype = (unsigned char ) sql__setrang((*list_info).elem[sql__setrang(list_cnt , 1 , 1024) + -1].datatype , 0 , 39);
                startstack = (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var108->a_mblock.integer.C_4.mb_qual , "vak65.p" , 16652)).integer.C_2.mfirst_free;
                types_ok = (char ) sql__setrang((int ) ptoc_Var113->n_datatype == 14 , 0 , 1);
                if (!(char ) sql__setrang(types_ok , 0 , 1))
                    {
                    types_ok = (char ) sql__setrang(a65_datatypes_ok(&(*acv) , &(*dmli) , &ptoc_Var113->n_datatype , &ptoc_Var113->n_length , (unsigned char ) sql__setrang(dm_datatype , 0 , 39) , 1 , *first_node , (*list_info).elem[sql__setrang(list_cnt , 1 , 1024) + -1].pos , (char ) sql__setrang((*list_info).count == 1 , 0 , 1) , &convert_t) , 0 , 1);
                    }
                if ((char ) sql__setrang(types_ok , 0 , 1) && (*list_info).count == 1 && ((ptoc_Var114 = (int ) dm_datatype) == 10 || ptoc_Var114 == 11 || ptoc_Var114 == 13))
                    {
                    a65ch_format(&(*acv) , &(*dmli) , &startstack , &colin , (unsigned char ) sql__setrang(ptoc_Var113->n_datatype , 0 , 39) , (unsigned char ) sql__setrang(dm_datatype , 0 , 39));
                    }
                list_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16668))[sql__setrang(list_node , 0 , 32767)].n_sa_level;
                }
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var108->a_return_segm , "vak65.p" , 16672)).integer.C_4.sp1r_returncode == 0 && list_cnt != (*list_info).count)
                {
                list_node = *first_node;
                while ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16677))[sql__setrang(list_node , 0 , 32767)].n_proc != 0 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16678))[sql__setrang(list_node , 0 , 32767)].n_lo_level != 0)
                    list_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16679))[sql__setrang(list_node , 0 , 32767)].n_lo_level;
                if (list_cnt < (*list_info).count)
                    {
                    a07_b_put_error(&(*acv) , 6444 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16684))[sql__setrang(list_node , 0 , 32767)].n_pos);
                    }
                else
                    {
                    if (list_cnt > (*list_info).count)
                        {
                        a07_b_put_error(&(*acv) , 6410 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16689))[sql__setrang(list_node , 0 , 32767)].n_pos);
                        }
                    }
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var108->a_return_segm , "vak65.p" , 16696)).integer.C_4.sp1r_returncode == 0 && (char ) sql__setrang(types_ok , 0 , 1))
            {
            if ((int ) *operator == 115)
                {
                ak65sub_op_set(&(*acv) , 1 , 0 , 115 , *first_node);
                }
            else
                {
                if ((int ) field_found > 0)
                    {
                    if (1 && ((*(tak00_columninfo *) sql__nil(ptoc_Var110->d_colbuf , "vak65.p" , 16705)).integer.C_1.ccolpropset[0] & 32) != 0)
                        {
                        ptoc_Var115 = (tgg00_QualBuf *) sql__nil(ptoc_Var109->integer.C_4.mb_qual , "vak65.p" , 16707);
                        ptoc_Var116 = &(*(tgg00_StackList *) sql__nil(ptoc_Var109->integer.C_5.mb_st , "vak65.p" , 16707))[sql__setrang((int ) field_found , 1 , 32767) + -1];
                        ptoc_Var116->integer.C_1.ecol_tab[1] = (unsigned char ) sql__setrang((unsigned char ) (sql__ord((int ) ptoc_Var116->integer.C_1.ecol_tab[1]) + 100) , 0 , 255);
                        }
                    }
                ak65sub_op_set(&(*acv) , 0 , 1 , 109 , *first_node);
                }
            }
        }
    else
        {
        *first_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16721))[sql__setrang(*first_node , 0 , 32767)].n_lo_level;
        colin.sci_len = 0;
        help_buf = ptoc_Var110->d_colptr;
        if ((int ) field_found > 0)
            {
            if (1 && ((*(tak00_columninfo *) sql__nil(ptoc_Var110->d_colbuf , "vak65.p" , 16726)).integer.C_1.ccolpropset[0] & 32) != 0)
                {
                ptoc_Var117 = &(*(tgg00_StackList *) sql__nil(ptoc_Var109->integer.C_5.mb_st , "vak65.p" , 16728))[sql__setrang((int ) field_found , 1 , 32767) + -1];
                ptoc_Var117->integer.C_1.ecol_tab[1] = (unsigned char ) sql__setrang((unsigned char ) (sql__ord((int ) ptoc_Var117->integer.C_1.ecol_tab[1]) + 100) , 0 , 255);
                }
            }
        if ((int ) ptoc_Var108->a_sqlmode == 3 || (int ) ptoc_Var108->a_sqlmode == 2)
            {
            in_strat = 1;
            ptoc_Var118 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16737))[sql__setrang(*first_node , 0 , 32767)];
            if ((int ) ptoc_Var118->n_proc == 47 && (int ) ptoc_Var118->n_subproc == 4)
                {
                list_node = (int ) ptoc_Var118->n_sa_level;
                *first_node = (int ) ptoc_Var118->n_lo_level;
                list_cnt = 1;
                }
            else
                {
                list_cnt = 0;
                list_node = 0;
                }
            do
                {
                first_free = (short ) sql__setrang((*(tgg00_QualBuf *) sql__nil(ptoc_Var108->a_mblock.integer.C_4.mb_qual , "vak65.p" , 16753)).integer.C_2.mfirst_free , -32768 , 32767);
                if (list_cnt > 0)
                    {
                    ptoc_Var110->d_datatype = (unsigned char ) sql__setrang((*list_info).elem[sql__setrang(list_cnt , 1 , 1024) + -1].datatype , 0 , 39);
                    ptoc_Var110->d_colptr = (tak00_columninfo *) (int *) 0;
                    }
                t01int4(5 , "list_cnt    " , list_cnt);
                t01int4(5 , "dm_datatype " , sql__ord((int ) ptoc_Var110->d_datatype));
                a65_val_expr(&(*acv) , &(*dmli) , &colin , (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16764))[sql__setrang(*first_node , 0 , 32767)].n_lo_level);
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var108->a_return_segm , "vak65.p" , 16765)).integer.C_4.sp1r_returncode != 0)
                    {
                    *first_node = 0;
                    }
                else
                    {
                    ptoc_Var119 = &ptoc_Var108->a_mblock;
                    if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var119->integer.C_5.mb_st , "vak65.p" , 16772))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var119->integer.C_4.mb_qual , "vak65.p" , 16772)).integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.etype != 22)
                        {
                        in_check = 0;
                        }
                    in_strat = (char ) sql__setrang((char ) sql__setrang(in_strat , 0 , 1) && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var119->integer.C_5.mb_st , "vak65.p" , 16777))[sql__setrang((int ) first_free , 1 , 32767) + -1].integer.C_1.etype == 22 && ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var119->integer.C_5.mb_st , "vak65.p" , 16778))[sql__setrang((int ) first_free , 1 , 32767) + -1].integer.C_1.ecol_tab[1] != 0 || (int ) first_free == (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var119->integer.C_4.mb_qual , "vak65.p" , 16779)).integer.C_2.mfirst_free + -1) , 0 , 1);
                    cnt_fac = sql__succ(cnt_fac , -2147483647 , 2147483647 , "vak65.p" , 16782);
                    *first_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16783))[sql__setrang(*first_node , 0 , 32767)].n_sa_level;
                    if (*first_node == 0 && list_node != 0)
                        {
                        ptoc_Var120 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16786))[sql__setrang(list_node , 0 , 32767)];
                        *first_node = (int ) ptoc_Var120->n_lo_level;
                        list_node = (int ) ptoc_Var120->n_sa_level;
                        list_cnt = 1;
                        }
                    else
                        {
                        if (list_cnt > 0 && *first_node > 0)
                            {
                            list_cnt = sql__succ(list_cnt , -2147483647 , 2147483647 , "vak65.p" , 16796);
                            }
                        }
                    if (*first_node > 0)
                        {
                        colin.sci_len = 0;
                        ptoc_Var110->d_colptr = help_buf;
                        ptoc_Var110->d_const_value_expr = 0;
                        ptoc_Var110->d_param_st_index = 0;
                        ptoc_Var110->d_param_st_begin = 0;
                        }
                    }
                }
            while (!(*first_node == 0));
            }
        else
            {
            in_strat = 0;
            list_cnt = 0;
            a640factor(&(*acv) , &(*dmli) , &colin , &(*first_node));
            cnt_fac = sql__succ(cnt_fac , -2147483647 , 2147483647 , "vak65.p" , 16820);
            while ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16821))[sql__setrang(*first_node , 0 , 32767)].n_sa_level != 0)
                {
                *first_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var108->a_ap_tree , "vak65.p" , 16823))[sql__setrang(*first_node , 0 , 32767)].n_sa_level;
                colin.sci_len = 0;
                ptoc_Var110->d_colptr = help_buf;
                a640not_first_factor(&(*acv) , &(*dmli) , &colin , &(*first_node));
                cnt_fac = sql__succ(cnt_fac , -2147483647 , 2147483647 , "vak65.p" , 16827);
                }
            }
        if (cnt_fac == 1)
            {
            if ((int ) *operator == 16)
                {
                a65_set_operator(&(*acv) , 115);
                }
            else
                {
                a65_set_operator(&(*acv) , 109);
                }
            }
        else
            {
            ptoc_Var121 = &ptoc_Var108->a_mblock;
            ptoc_Var122 = (tgg00_QualBuf *) sql__nil(ptoc_Var121->integer.C_4.mb_qual , "vak65.p" , 16841);
            if ((int ) ptoc_Var122->integer.C_2.mfirst_free > (int ) ptoc_Var121->integer.C_2.mb_st_max)
                {
                a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var121->integer.C_2.mb_st_max);
                }
            else
                {
                ptoc_Var122->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var122->integer.C_2.mqual_cnt , -32768 , 32767 , "vak65.p" , 16849) , -32768 , 32767);
                ptoc_Var123 = &(*(tgg00_StackList *) sql__nil(ptoc_Var121->integer.C_5.mb_st , "vak65.p" , 16850))[sql__setrang((int ) ptoc_Var122->integer.C_2.mfirst_free , 1 , 32767) + -1];
                ptoc_Var123->integer.C_1.etype = 26;
                ptoc_Var123->integer.C_1.eop = (unsigned char ) sql__setrang(*operator , 0 , 123);
                if (list_cnt == 0)
                    {
                    ptoc_Var123->integer.C_1.epos = 1;
                    ptoc_Var123->integer.C_1.elen_var = (short ) sql__setrang((short ) cnt_fac , -32768 , 32767);
                    }
                else
                    {
                    ptoc_Var123->integer.C_1.epos = (short ) sql__setrang((short ) list_cnt , -32768 , 32767);
                    ptoc_Var123->integer.C_1.elen_var = (short ) sql__setrang((short ) (cnt_fac / list_cnt) , -32768 , 32767);
                    in_check = 0;
                    in_strat = 0;
                    }
                if ((char ) sql__setrang(in_strat , 0 , 1) && (char ) sql__setrang(in_check , 0 , 1))
                    {
                    ptoc_Var123->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                    }
                else
                    {
                    ptoc_Var123->integer.C_1.ecol_tab[0] = (unsigned char)'\002';
                    }
                ptoc_Var123->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                ptoc_Var122->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var122->integer.C_2.mfirst_free , -32768 , 32767 , "vak65.p" , 16877) , -32768 , 32767);
                }
            }
        if ((char ) sql__setrang(in_strat , 0 , 1) && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var108->a_return_segm , "vak65.p" , 16883)).integer.C_4.sp1r_returncode == 0)
            {
            a651in_const_expression(&(*acv) , &(*dmli) , (short ) sql__setrang(first_entry , -32768 , 32767) , (short ) sql__setrang((short ) ((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var108->a_mblock.integer.C_4.mb_qual , "vak65.p" , 16886)).integer.C_2.mfirst_free + -1) , -32768 , 32767));
            }
        ptoc_Var110->d_single_expr = 0;
        }
    cccprint("<ak65in_predicat");
    }

static
  unsigned char ptoc_Var124[2] = 
  {
  (unsigned char)'\002', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var125[2] = 
  {
  (unsigned char)'\002', (unsigned char)'\0'
  };
static
  tak00_colpropset ptoc_Var126 = 
  {
  (unsigned char)'\002', (unsigned char)'\0'
  };


int ak65new_colinfo(acv, col1, col2, n_symb)
tak_all_command_glob *acv;
tak00_scolinf *col1;
tak00_scolinf *col2;
tak_sc_symbol n_symb;
    {
    int diff1;
    int diff2;
    int first;
    int second;
    unsigned char ptoc_Var127[2];
    int ptoc_Var128;
    cccprint("AK65  ak65new_co");
    if (sql__erq(sizeof(unsigned char [2]) , (*col1).sci_cprops , ptoc_Var124) || sql__erq(sizeof(unsigned char [2]) , (*col2).sci_cprops , ptoc_Var125))
        {
        _ptoc_MOVE(unsigned char , 2, (*col1).sci_cprops, ptoc_Var126);
        }
    t01int4(5 , "col1 frac > " , (int ) (*col1).sci_frac);
    t01int4(5 , "col2 frac > " , (int ) (*col2).sci_frac);
    t01int4(5 , "col1 len  > " , (int ) (*col1).sci_len);
    t01int4(5 , "col2 len  > " , (int ) (*col2).sci_len);
    t01int4(5 , "col1 typ  > " , sql__ord((int ) (*col1).sci_typ));
    t01int4(5 , "col2 typ  > " , sql__ord((int ) (*col2).sci_typ));
    if ((int ) (*col1).sci_frac == -1 || (int ) (*col2).sci_frac == -1 || (int ) (*acv).a_recursive_state == 1)
        {
        (*col1).sci_len = 38;
        (*col1).sci_frac = -1;
        (*col1).sci_iolen = 21;
        (*col1).sci_typ = 12;
        }
    else
        {
        if ((int ) (*col1).sci_len == 0)
            {
            *col1 = *col2;
            }
        else
            {
            if ((int ) (*col1).sci_frac == -1)
                {
                diff1 = (int ) (*col1).sci_len;
                }
            else
                {
                diff1 = (int ) (*col1).sci_len - (int ) (*col1).sci_frac;
                }
            if ((int ) (*col2).sci_frac == -1)
                {
                diff2 = (int ) (*col2).sci_len;
                }
            else
                {
                diff2 = (int ) (*col2).sci_len - (int ) (*col2).sci_frac;
                }
            switch ((int ) n_symb)
                {
                case 11 :
                case 10 :
                    if (diff1 > diff2)
                        {
                        first = diff1;
                        }
                    else
                        {
                        first = diff2;
                        }
                    if ((int ) (*col1).sci_frac > (int ) (*col2).sci_frac)
                        {
                        second = (int ) (*col1).sci_frac;
                        }
                    else
                        {
                        second = (int ) (*col2).sci_frac;
                        }
                    if (first + second + 1 <= 38)
                        {
                        (*col1).sci_len = (short ) sql__setrang((short ) (first + second + 1) , -32768 , 32767);
                        (*col1).sci_frac = (short ) sql__setrang((short ) second , -32768 , 32767);
                        }
                    else
                        {
                        (*col1).sci_len = 38;
                        (*col1).sci_typ = 12;
                        }
                    break;
                case 118 :
                    if ((int ) (*col1).sci_len + (int ) (*col2).sci_len > 38)
                        {
                        (*col1).sci_len = 38;
                        (*col1).sci_typ = 12;
                        }
                    else
                        {
                        (*col1).sci_len = (short ) sql__setrang((short ) ((int ) (*col1).sci_len + (int ) (*col2).sci_len) , -32768 , 32767);
                        (*col1).sci_frac = (short ) sql__setrang((short ) ((int ) (*col1).sci_frac + (int ) (*col2).sci_frac) , -32768 , 32767);
                        }
                    break;
                case 2 :
                    if (diff1 + (int ) (*col2).sci_frac >= 38 && (int ) (*col2).sci_frac != -1)
                        {
                        (*col1).sci_len = 38;
                        (*col1).sci_typ = 12;
                        }
                    else
                        {
                        (*col1).sci_frac = (short ) sql__setrang((short ) ((int ) (*col1).sci_frac - (int ) (*col1).sci_len - (int ) (*col2).sci_frac + 38) , -32768 , 32767);
                        (*col1).sci_len = 38;
                        }
                    break;
                default :
                    (unsigned char ) sql__caseerr("vak65.p" , 16950);
                    break;
                }
            (*col1).sci_iolen = (short ) sql__setrang((short ) (((int ) (*col1).sci_len + 1) / 2 + 2) , -32768 , 32767);
            if ((ptoc_Var128 = (int ) (*col1).sci_typ) == 1 || ptoc_Var128 == 12 || ((ptoc_Var128 = (int ) (*col2).sci_typ) == 1 || ptoc_Var128 == 12))
                {
                (*col1).sci_frac = -1;
                (*col1).sci_typ = 12;
                }
            }
        }
    cccprint("<ak65new_colinfo");
    }



int ak65oracle_typecheck(acv, dmli, list_info, first_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
list_elem_info *list_info;
int first_node;
    {
    char only_numbers;
    char number_found;
    char subquery_list;
    int sel_n;
    int act_node;
    int left_node;
    int right_node;
    int list_elem;
    int list_elem_node;
    int list_node;
    int list_cnt;
    char left_fixed[1024];
    tak_all_command_glob *ptoc_Var129;
    tak_dml_info *ptoc_Var130;
    struct ptoc_Type56 *ptoc_Var131;
    int ptoc_Var132;
    cccprint("AK65  ak65oracle");
    ptoc_Var129 = &*acv;
    ptoc_Var130 = &*dmli;
    act_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17049))[sql__setrang(first_node , 0 , 32767)].n_lo_level;
    ptoc_Var131 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17050))[sql__setrang(act_node , 0 , 32767)];
    if ((int ) ptoc_Var131->n_proc == 47 && (int ) ptoc_Var131->n_subproc == 4)
        {
        list_elem = 1;
        do
            {
            (*list_info).elem[sql__setrang(list_elem , 1 , 1024) + -1].datatype = 14;
            if (list_elem == 1024)
                {
                break;
                }
            list_elem += 1;
            }
        while (!0);
        subquery_list = 0;
        list_node = act_node;
        list_cnt = 0;
        while (list_node > 0)
            {
            list_cnt = sql__succ(list_cnt , -2147483647 , 2147483647 , "vak65.p" , 17062);
            list_elem_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17063))[sql__setrang(list_node , 0 , 32767)].n_lo_level;
            list_elem = 0;
            while (list_elem_node > 0)
                {
                ptoc_Var130->d_datatype = 14;
                a65_look_for_datatypes(&(*acv) , &(*dmli) , (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17069))[sql__setrang(list_elem_node , 0 , 32767)].n_lo_level);
                if ((ptoc_Var132 = (int ) ptoc_Var130->d_datatype) == 0 || ptoc_Var132 == 1 || ptoc_Var132 == 12 || ptoc_Var132 == 15)
                    {
                    (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17072))[sql__setrang(list_elem_node , 0 , 32767)].n_datatype = 0;
                    }
                else
                    {
                    (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17074))[sql__setrang(list_elem_node , 0 , 32767)].n_datatype = 2;
                    }
                list_elem = sql__succ(list_elem , -2147483647 , 2147483647 , "vak65.p" , 17076);
                if ((int ) (*list_info).elem[sql__setrang(list_elem , 1 , 1024) + -1].datatype == 14)
                    {
                    (*list_info).elem[sql__setrang(list_elem , 1 , 1024) + -1].datatype = (unsigned char ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17081))[sql__setrang(list_elem_node , 0 , 32767)].n_datatype , 0 , 39);
                    if ((int ) (*list_info).elem[sql__setrang(list_elem , 1 , 1024) + -1].datatype == 0)
                        {
                        (*list_info).elem[sql__setrang(list_elem , 1 , 1024) + -1].datalen = (short ) sql__setrang((short ) ((*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17086))[sql__setrang(list_elem_node , 0 , 32767)].n_pos / 100) , -32768 , 32767);
                        (*list_info).elem[sql__setrang(list_elem , 1 , 1024) + -1].datafrac = (unsigned char ) sql__setrang((unsigned char ) ((ptoc_Var132 = (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17088))[sql__setrang(list_elem_node , 0 , 32767)].n_pos % 100) >= 0 ? ptoc_Var132 : ptoc_Var132 + 100) , 0 , 255);
                        }
                    else
                        {
                        (*list_info).elem[sql__setrang(list_elem , 1 , 1024) + -1].datalen = (short ) sql__setrang((short ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17092))[sql__setrang(list_elem_node , 0 , 32767)].n_pos , -32768 , 32767);
                        }
                    }
                else
                    {
                    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17096))[sql__setrang(list_elem_node , 0 , 32767)].n_datatype != (int ) (*list_info).elem[sql__setrang(list_elem , 1 , 1024) + -1].datatype)
                        {
                        a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17101))[sql__setrang(list_elem_node , 0 , 32767)].n_pos);
                        }
                    }
                list_elem_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17104))[sql__setrang(list_elem_node , 0 , 32767)].n_sa_level;
                }
            if (list_cnt == 1)
                {
                list_elem = 1;
                do
                    {
                    left_fixed[sql__setrang(list_elem , 1 , 1024) + -1] = (char ) sql__setrang((int ) (*list_info).elem[sql__setrang(list_elem , 1 , 1024) + -1].datatype == 0 , 0 , 1);
                    (*list_info).elem[sql__setrang(list_elem , 1 , 1024) + -1].datatype = 14;
                    if (list_elem == 1024)
                        {
                        break;
                        }
                    list_elem += 1;
                    }
                while (!0);
                list_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17117))[sql__setrang(list_node , 0 , 32767)].n_sa_level;
                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17118))[sql__setrang(list_node , 0 , 32767)].n_symb == 77 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17119))[sql__setrang(list_node , 0 , 32767)].n_symb == 78)
                    {
                    list_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17121))[sql__setrang(list_node , 0 , 32767)].n_sa_level;
                    }
                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17123))[sql__setrang(list_node , 0 , 32767)].n_proc == 82 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17125))[sql__setrang(list_node , 0 , 32767)].n_proc == 54 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17126))[sql__setrang(list_node , 0 , 32767)].n_subproc == 13)
                    {
                    subquery_list = 1;
                    list_node = 0;
                    }
                else
                    {
                    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17133))[sql__setrang(list_node , 0 , 32767)].n_proc == 55 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17134))[sql__setrang(list_node , 0 , 32767)].n_subproc == 8)
                        {
                        list_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17136))[sql__setrang(list_node , 0 , 32767)].n_lo_level;
                        }
                    }
                }
            else
                {
                list_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17141))[sql__setrang(list_node , 0 , 32767)].n_sa_level;
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var129->a_return_segm , "vak65.p" , 17145)).integer.C_4.sp1r_returncode == 0 && !(char ) sql__setrang(subquery_list , 0 , 1))
            {
            list_node = act_node;
            list_cnt = 0;
            while (list_node > 0 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var129->a_return_segm , "vak65.p" , 17152)).integer.C_4.sp1r_returncode == 0)
                {
                list_cnt = sql__succ(list_cnt , -2147483647 , 2147483647 , "vak65.p" , 17154);
                list_elem_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17155))[sql__setrang(list_node , 0 , 32767)].n_lo_level;
                list_elem = 0;
                while (list_elem_node > 0)
                    {
                    list_elem = sql__succ(list_elem , -2147483647 , 2147483647 , "vak65.p" , 17159);
                    if (list_cnt == 1 && !(char ) sql__setrang(left_fixed[sql__setrang(list_elem , 1 , 1024) + -1] , 0 , 1) && (int ) (*list_info).elem[sql__setrang(list_elem , 1 , 1024) + -1].datatype == 0 || list_cnt > 1 && (char ) sql__setrang(left_fixed[sql__setrang(list_elem , 1 , 1024) + -1] , 0 , 1) && (int ) (*list_info).elem[sql__setrang(list_elem , 1 , 1024) + -1].datatype == 2)
                        {
                        ak65s_num_into_tree(&(*acv) , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17169))[sql__setrang(list_elem_node , 0 , 32767)].n_lo_level , (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17170))[sql__setrang(list_elem_node , 0 , 32767)].n_lo_level , 0);
                        }
                    list_elem_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17172))[sql__setrang(list_elem_node , 0 , 32767)].n_sa_level;
                    }
                if (list_cnt == 1)
                    {
                    list_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17178))[sql__setrang(list_node , 0 , 32767)].n_sa_level;
                    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17179))[sql__setrang(list_node , 0 , 32767)].n_symb == 77 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17180))[sql__setrang(list_node , 0 , 32767)].n_symb == 78)
                        {
                        list_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17182))[sql__setrang(list_node , 0 , 32767)].n_sa_level;
                        }
                    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17184))[sql__setrang(list_node , 0 , 32767)].n_proc == 55 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17185))[sql__setrang(list_node , 0 , 32767)].n_subproc == 8)
                        {
                        list_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17187))[sql__setrang(list_node , 0 , 32767)].n_lo_level;
                        }
                    }
                else
                    {
                    list_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17191))[sql__setrang(list_node , 0 , 32767)].n_sa_level;
                    }
                }
            }
        }
    else
        {
        ptoc_Var130->d_datatype = 14;
        a65_look_for_datatypes(&(*acv) , &(*dmli) , act_node);
        if ((ptoc_Var132 = (int ) ptoc_Var130->d_datatype) != 0 && ptoc_Var132 != 1 && ptoc_Var132 != 12 && ptoc_Var132 != 15)
            {
            ptoc_Var130->d_datatype = 14;
            sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17206))[sql__setrang(first_node , 0 , 32767)].n_lo_level;
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17207))[sql__setrang(sel_n , 0 , 32767)].n_symb == 43)
                {
                sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17209))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
                }
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17211))[sql__setrang(sel_n , 0 , 32767)].n_symb == 44)
                {
                sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17213))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
                }
            sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17215))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
            if ((ptoc_Var132 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17216))[sql__setrang(first_node , 0 , 32767)].n_symb) == 4 || ptoc_Var132 == 5 || ptoc_Var132 == 6 || ptoc_Var132 == 8 || ptoc_Var132 == 9 || ptoc_Var132 == 16)
                {
                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17221))[sql__setrang(sel_n , 0 , 32767)].n_symb == 77 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17223))[sql__setrang(sel_n , 0 , 32767)].n_symb == 78)
                    {
                    sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17225))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
                    }
                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17228))[sql__setrang(sel_n , 0 , 32767)].n_proc != 82 && ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17230))[sql__setrang(sel_n , 0 , 32767)].n_proc != 54 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17231))[sql__setrang(sel_n , 0 , 32767)].n_subproc != 13) && ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17233))[sql__setrang(sel_n , 0 , 32767)].n_proc != 47 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17234))[sql__setrang(sel_n , 0 , 32767)].n_subproc != 4))
                    {
                    a65_look_for_datatypes(&(*acv) , &(*dmli) , sel_n);
                    if ((ptoc_Var132 = (int ) ptoc_Var130->d_datatype) == 0 || ptoc_Var132 == 1 || ptoc_Var132 == 12 || ptoc_Var132 == 15)
                        {
                        ak65s_num_into_tree(&(*acv) , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17242))[sql__setrang(first_node , 0 , 32767)].n_lo_level , act_node , sel_n);
                        }
                    }
                }
            else
                {
                if ((ptoc_Var132 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17249))[sql__setrang(first_node , 0 , 32767)].n_symb) == 30 || ptoc_Var132 == 29)
                    {
                    a65_look_for_datatypes(&(*acv) , &(*dmli) , sel_n);
                    left_node = sel_n;
                    if ((ptoc_Var132 = (int ) ptoc_Var130->d_datatype) == 0 || ptoc_Var132 == 1 || ptoc_Var132 == 12 || ptoc_Var132 == 15)
                        {
                        ak65s_num_into_tree(&(*acv) , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17258))[sql__setrang(first_node , 0 , 32767)].n_lo_level , act_node , sel_n);
                        ptoc_Var130->d_datatype = 14;
                        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17261))[sql__setrang(sel_n , 0 , 32767)].n_symb == 43)
                            {
                            sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17263))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
                            }
                        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17265))[sql__setrang(sel_n , 0 , 32767)].n_symb == 44)
                            {
                            sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17267))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
                            }
                        right_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17269))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
                        a65_look_for_datatypes(&(*acv) , &(*dmli) , right_node);
                        if ((ptoc_Var132 = (int ) ptoc_Var130->d_datatype) != 0 && ptoc_Var132 != 1 && ptoc_Var132 != 12 && ptoc_Var132 != 15)
                            {
                            a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17274))[sql__setrang(right_node , 0 , 32767)].n_pos);
                            }
                        }
                    else
                        {
                        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17292))[sql__setrang(sel_n , 0 , 32767)].n_symb == 43)
                            {
                            sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17294))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
                            }
                        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17296))[sql__setrang(sel_n , 0 , 32767)].n_symb == 44)
                            {
                            sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17298))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
                            }
                        ptoc_Var130->d_datatype = 14;
                        right_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17301))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
                        a65_look_for_datatypes(&(*acv) , &(*dmli) , right_node);
                        if ((ptoc_Var132 = (int ) ptoc_Var130->d_datatype) == 0 || ptoc_Var132 == 1 || ptoc_Var132 == 12 || ptoc_Var132 == 15)
                            {
                            a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17306))[sql__setrang(right_node , 0 , 32767)].n_pos);
                            }
                        }
                    }
                else
                    {
                    if ((ptoc_Var132 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17323))[sql__setrang(first_node , 0 , 32767)].n_symb) == 28 || ptoc_Var132 == 27)
                        {
                        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17326))[sql__setrang(sel_n , 0 , 32767)].n_symb == 77 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17328))[sql__setrang(sel_n , 0 , 32767)].n_symb == 78)
                            {
                            sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17330))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
                            }
                        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17333))[sql__setrang(sel_n , 0 , 32767)].n_proc != 82 && ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17335))[sql__setrang(sel_n , 0 , 32767)].n_proc != 54 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17336))[sql__setrang(sel_n , 0 , 32767)].n_subproc != 13))
                            {
                            number_found = 0;
                            only_numbers = 1;
                            left_node = sel_n;
                            sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17343))[sql__setrang(sel_n , 0 , 32767)].n_lo_level;
                            while (sel_n != 0)
                                {
                                ptoc_Var130->d_datatype = 14;
                                a65_look_for_datatypes(&(*acv) , &(*dmli) , (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17348))[sql__setrang(sel_n , 0 , 32767)].n_lo_level);
                                if ((ptoc_Var132 = (int ) ptoc_Var130->d_datatype) == 0 || ptoc_Var132 == 1 || ptoc_Var132 == 12 || ptoc_Var132 == 15)
                                    {
                                    (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17351))[sql__setrang(sel_n , 0 , 32767)].n_datatype = 0;
                                    }
                                else
                                    {
                                    (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17353))[sql__setrang(sel_n , 0 , 32767)].n_datatype = 2;
                                    }
                                number_found = (char ) sql__setrang((char ) sql__setrang(number_found , 0 , 1) || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17356))[sql__setrang(sel_n , 0 , 32767)].n_datatype == 0 , 0 , 1);
                                only_numbers = (char ) sql__setrang((char ) sql__setrang(only_numbers , 0 , 1) && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17358))[sql__setrang(sel_n , 0 , 32767)].n_datatype == 0 , 0 , 1);
                                sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17359))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
                                }
                            if ((char ) sql__setrang(number_found , 0 , 1))
                                {
                                if (!(char ) sql__setrang(only_numbers , 0 , 1))
                                    {
                                    a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17367))[sql__setrang(act_node , 0 , 32767)].n_pos);
                                    }
                                else
                                    {
                                    ak65s_num_into_tree(&(*acv) , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17372))[sql__setrang(first_node , 0 , 32767)].n_lo_level , act_node , left_node);
                                    act_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17374))[sql__setrang(first_node , 0 , 32767)].n_lo_level;
                                    sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17375))[sql__setrang(act_node , 0 , 32767)].n_sa_level;
                                    sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17376))[sql__setrang(sel_n , 0 , 32767)].n_lo_level;
                                    while (sel_n != 0)
                                        {
                                        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17379))[sql__setrang(sel_n , 0 , 32767)].n_datatype != 0)
                                            {
                                            ak65s_num_into_tree(&(*acv) , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17383))[sql__setrang(sel_n , 0 , 32767)].n_lo_level , (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17384))[sql__setrang(sel_n , 0 , 32767)].n_lo_level , 0);
                                            }
                                        sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var129->a_ap_tree , "vak65.p" , 17387))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    ptoc_Var130->d_datatype = 14;
    cccprint("<ak65oracle_type");
    }

static
  tak_jcolpropset ptoc_Var133 = 
  {
  (unsigned char)'\0'
  };
static
  tak_joinset ptoc_Var134 = 
  {
  (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0', (unsigned char)'\0'
  };


int ak65predicate(acv, dmli, first_node, mtabs_allowed, list_info)
tak_all_command_glob *acv;
tak_dml_info *dmli;
int *first_node;
char mtabs_allowed;
list_elem_info *list_info;
    {
    char normpred;
    char snot_pred;
    char snot_subq;
    char types_ok;
    int act_node;
    int pred_node;
    int sel_n;
    int col_node;
    int left_side;
    int right_side;
    tgg_stack_op_type operator;
    tak_convert_type convert_t;
    tak00_scolinf colin;
    int dtype_n;
    int list_node;
    int list_cnt;
    int this_size;
    int left_size;
    int startstack;
    int oldjoincnt;
    tsp_int2 field_found;
    int xi;
    int used_tab_cnt;
    tak_joinset used_tables;
    tak_all_command_glob *ptoc_Var135;
    tak_dml_info *ptoc_Var136;
    tak_joinrec *ptoc_Var137;
    struct ptoc_Type110 *ptoc_Var138;
    struct ptoc_Type56 *ptoc_Var139;
    int ptoc_Var140;
    struct ptoc_Type56 *ptoc_Var141;
    struct ptoc_Type56 *ptoc_Var142;
    tak_joinrec *ptoc_Var143;
    struct ptoc_Type110 *ptoc_Var144;
    tgg00_QualBuf *ptoc_Var145;
    struct ptoc_Type56 *ptoc_Var146;
    struct ptoc_Type56 *ptoc_Var147;
    tak_joinrec *ptoc_Var148;
    struct ptoc_Type110 *ptoc_Var149;
    struct ptoc_Type111 *ptoc_Var150;
    tgg00_QualBuf *ptoc_Var151;
    tak00_scolinf *ptoc_Var152;
    struct ptoc_Type56 *ptoc_Var153;
    struct ptoc_Type176 *ptoc_Var154;
    struct ptoc_Type56 *ptoc_Var155;
    struct ptoc_Type56 *ptoc_Var156;
    tak_joinrec *ptoc_Var157;
    struct ptoc_Type110 *ptoc_Var158;
    tak_dml_info *ptoc_Var159;
    tak_joinrec *ptoc_Var160;
    struct ptoc_Type110 *ptoc_Var161;
    tak_joinset ptoc_Var162;
    tak_joinrec *ptoc_Var163;
    struct ptoc_Type110 *ptoc_Var164;
    cccprint("AK65  ak65predic");
    ptoc_Var135 = &*acv;
    ptoc_Var136 = &*dmli;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var135->a_return_segm , "vak65.p" , 17446)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var136->d_const_value_expr = (char ) sql__setrang((int ) ptoc_Var135->a_is_ddl == 0 , 0 , 1);
        ptoc_Var136->d_param_st_index = 0;
        ptoc_Var136->d_param_st_begin = 0;
        ptoc_Var136->d_is_parameter_only = 0;
        if ((char ) sql__setrang(ptoc_Var136->d_join , 0 , 1))
            {
            ptoc_Var137 = &ptoc_Var136->d_joins;
            ptoc_Var138 = &ptoc_Var137->jrc_joinarr[sql__setrang((int ) ptoc_Var137->jrc_cnt , 0 , 256)];
            ptoc_Var138->jo_no_join = 0;
            ptoc_Var138->jo_partno = (unsigned char)'\001';
            ptoc_Var138->jo_recs[0].jop_tableno = (unsigned char)'\0';
            ptoc_Var138->jo_recs[1].jop_tableno = (unsigned char)'\0';
            ptoc_Var138->jo_recs[0].jop_outer_join = 0;
            ptoc_Var138->jo_recs[1].jop_outer_join = 0;
            oldjoincnt = (int ) ptoc_Var136->d_joins.jrc_cnt;
            }
        ptoc_Var136->d_datatype = 14;
        pred_node = *first_node;
        act_node = *first_node;
        snot_subq = (char ) sql__setrang(ak65subq_se(&(*acv) , &(*first_node)) , 0 , 1);
        snot_pred = 0;
        if (!(char ) sql__setrang(snot_subq , 0 , 1))
            {
            ptoc_Var139 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17476))[sql__setrang(*first_node , 0 , 32767)];
            if ((int ) ptoc_Var139->n_symb == 53)
                {
                snot_pred = 1;
                *first_node = (int ) ptoc_Var139->n_sa_level;
                }
            }
        left_side = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17492))[sql__setrang(*first_node , 0 , 32767)].n_lo_level;
        right_side = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17493))[sql__setrang(left_side , 0 , 32767)].n_sa_level;
        if (((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17494))[sql__setrang(left_side , 0 , 32767)].n_lo_level == 0 || ((ptoc_Var140 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17495))[sql__setrang(left_side , 0 , 32767)].n_symb) == 11 || ptoc_Var140 == 10 || ptoc_Var140 == 118 || ptoc_Var140 == 2 || ptoc_Var140 == 64 || ptoc_Var140 == 65) || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17497))[sql__setrang(left_side , 0 , 32767)].n_proc == 88) && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17498))[sql__setrang(left_side , 0 , 32767)].n_symb != 43 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17499))[sql__setrang(left_side , 0 , 32767)].n_symb != 44 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17500))[sql__setrang(left_side , 0 , 32767)].n_symb != 45 && !(char ) sql__setrang(ptoc_Var136->d_outer_join , 0 , 1) && !(char ) sql__setrang(ptoc_Var135->a_outer_join , 0 , 1))
            {
            col_node = right_side;
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17506))[sql__setrang(col_node , 0 , 32767)].n_symb == 43)
                {
                col_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17508))[sql__setrang(col_node , 0 , 32767)].n_sa_level;
                }
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17510))[sql__setrang(col_node , 0 , 32767)].n_symb == 44)
                {
                col_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17512))[sql__setrang(col_node , 0 , 32767)].n_sa_level;
                }
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17514))[sql__setrang(col_node , 0 , 32767)].n_symb == 45)
                {
                ptoc_Var141 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17516))[sql__setrang(*first_node , 0 , 32767)];
                if ((ptoc_Var140 = (int ) ptoc_Var141->n_symb) == 4 || ptoc_Var140 == 5 || ptoc_Var140 == 6 || ptoc_Var140 == 8 || ptoc_Var140 == 9 || ptoc_Var140 == 16 || ptoc_Var140 == 56 || ptoc_Var140 == 55)
                    {
                    (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17524))[sql__setrang(col_node , 0 , 32767)].n_sa_level = (short ) sql__setrang((short ) left_side , -32768 , 32767);
                    (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17525))[sql__setrang(*first_node , 0 , 32767)].n_lo_level = (short ) sql__setrang((short ) right_side , -32768 , 32767);
                    (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17526))[sql__setrang(left_side , 0 , 32767)].n_sa_level = 0;
                    switch ((int ) ptoc_Var141->n_symb)
                        {
                        case 5 :
                            ptoc_Var141->n_symb = 8;
                            break;
                        case 6 :
                            ptoc_Var141->n_symb = 9;
                            break;
                        case 8 :
                            ptoc_Var141->n_symb = 5;
                            break;
                        case 9 :
                            ptoc_Var141->n_symb = 6;
                            break;
                        default :
                            break;
                        }
                    }
                }
            }
        normpred = 0;
        ptoc_Var142 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17549))[sql__setrang(*first_node , 0 , 32767)];
        if ((int ) ptoc_Var142->n_symb == 52)
            {
            if ((char ) sql__setrang(mtabs_allowed , 0 , 1))
                {
                ptoc_Var136->d_first_tab = 0;
                }
            ak65se_condition(&(*acv) , &(*dmli) , &(*first_node) , 0 , &(*list_info));
            }
        else
            {
            if ((int ) ptoc_Var142->n_symb == 54)
                {
                if ((char ) sql__setrang(snot_pred , 0 , 1) || (char ) sql__setrang(snot_subq , 0 , 1))
                    {
                    if ((char ) sql__setrang(mtabs_allowed , 0 , 1))
                        {
                        ptoc_Var136->d_first_tab = 0;
                        }
                    ak65boolean_term(&(*acv) , &(*dmli) , &(*first_node) , 0 , &(*list_info));
                    }
                else
                    {
                    ak65boolean_term(&(*acv) , &(*dmli) , &(*first_node) , (char ) sql__setrang(mtabs_allowed , 0 , 1) , &(*list_info));
                    }
                }
            else
                {
                normpred = 1;
                act_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17581))[sql__setrang(*first_node , 0 , 32767)].n_lo_level;
                }
            }
        if ((char ) sql__setrang(normpred , 0 , 1))
            {
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17588))[sql__setrang(act_node , 0 , 32767)].n_proc == 55 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17589))[sql__setrang(act_node , 0 , 32767)].n_subproc == 15)
                {
                act_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17591))[sql__setrang(act_node , 0 , 32767)].n_lo_level;
                }
            ptoc_Var136->d_allowed = 1;
            if ((char ) sql__setrang(ptoc_Var136->d_join , 0 , 1))
                {
                ptoc_Var143 = &ptoc_Var136->d_joins;
                ptoc_Var144 = &ptoc_Var143->jrc_joinarr[sql__setrang((int ) ptoc_Var143->jrc_cnt , 0 , 256)];
                ptoc_Var145 = (tgg00_QualBuf *) sql__nil(ptoc_Var135->a_mblock.integer.C_4.mb_qual , "vak65.p" , 17596);
                ptoc_Var144->jo_recs[0].jop_startstack = (short ) sql__setrang(ptoc_Var145->integer.C_2.mfirst_free , -32768 , 32767);
                ptoc_Var144->jo_partno = (unsigned char)'\001';
                }
            colin.sci_len = 0;
            t01int4(5 , "act_nodepred" , act_node);
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17605))[sql__setrang(*first_node , 0 , 32767)].n_symb == 115)
                {
                ak65sub_op_set(&(*acv) , 1 , 1 , 109 , act_node);
                if (!(char ) sql__setrang(ptoc_Var136->d_use_sub , 0 , 1))
                    {
                    a01_dt_put_datatype_node(&(*acv) , &(*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17614))[sql__setrang(act_node , 0 , 32767)].n_sa_level , (unsigned char ) sql__setrang(ptoc_Var136->d_datatype , 0 , 39) , 0 , 0 , 0);
                    }
                }
            else
                {
                if ((int ) ptoc_Var135->a_sqlmode == 2 && ((ptoc_Var140 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17621))[sql__setrang(*first_node , 0 , 32767)].n_symb) == 4 || ptoc_Var140 == 5 || ptoc_Var140 == 6 || ptoc_Var140 == 8 || ptoc_Var140 == 9 || ptoc_Var140 == 16 || ptoc_Var140 == 30 || ptoc_Var140 == 29 || ptoc_Var140 == 28 || ptoc_Var140 == 27))
                    {
                    ak65oracle_typecheck(&(*acv) , &(*dmli) , &(*list_info) , *first_node);
                    act_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17630))[sql__setrang(*first_node , 0 , 32767)].n_lo_level;
                    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17631))[sql__setrang(act_node , 0 , 32767)].n_proc == 55 && (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17632))[sql__setrang(act_node , 0 , 32767)].n_pos == 15)
                        {
                        act_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17634))[sql__setrang(act_node , 0 , 32767)].n_lo_level;
                        }
                    }
                ptoc_Var146 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17638))[sql__setrang(act_node , 0 , 32767)];
                if ((int ) ptoc_Var146->n_proc == 47 && (int ) ptoc_Var146->n_subproc == 4)
                    {
                    ptoc_Var147 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17641))[sql__setrang((int ) ptoc_Var146->n_lo_level , 0 , 32767)];
                    list_node = (int ) ptoc_Var147->n_sa_level;
                    act_node = (int ) ptoc_Var147->n_lo_level;
                    }
                else
                    {
                    list_node = 0;
                    }
                sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17652))[sql__setrang(*first_node , 0 , 32767)].n_lo_level;
                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17653))[sql__setrang(sel_n , 0 , 32767)].n_symb == 43)
                    {
                    sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17655))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
                    }
                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17657))[sql__setrang(sel_n , 0 , 32767)].n_symb == 44)
                    {
                    sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17659))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
                    }
                sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17661))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17662))[sql__setrang(sel_n , 0 , 32767)].n_symb == 44)
                    {
                    sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17664))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
                    }
                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17666))[sql__setrang(sel_n , 0 , 32767)].n_symb == 77 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17667))[sql__setrang(sel_n , 0 , 32767)].n_symb == 78)
                    {
                    sel_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17669))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
                    }
                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17671))[sql__setrang(sel_n , 0 , 32767)].n_proc == 82 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17672))[sql__setrang(sel_n , 0 , 32767)].n_proc == 54 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17673))[sql__setrang(sel_n , 0 , 32767)].n_subproc == 13)
                    {
                    dtype_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17676))[sql__setrang(sel_n , 0 , 32767)].n_sa_level;
                    }
                else
                    {
                    dtype_n = -1;
                    }
                t01int4(5 , "first_node  " , *first_node);
                t01int4(5 , "sel_n       " , sel_n);
                t01int4(5 , "n_proc      " , sql__ord((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17683))[sql__setrang(sel_n , 0 , 32767)].n_proc));
                t01int4(5 , "dtype_n     " , dtype_n);
                left_size = 0;
                list_cnt = 1;
                while (list_cnt != 0)
                    {
                    if ((ptoc_Var140 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17689))[sql__setrang(act_node , 0 , 32767)].n_symb) == 45 || ptoc_Var140 == 44 || ptoc_Var140 == 43)
                        {
                        field_found = (short ) sql__setrang((*(tgg00_QualBuf *) sql__nil(ptoc_Var135->a_mblock.integer.C_4.mb_qual , "vak65.p" , 17692)).integer.C_2.mfirst_free , -32768 , 32767);
                        }
                    else
                        {
                        field_found = 0;
                        }
                    ptoc_Var136->d_colptr = (tak00_columninfo *) (int *) 0;
                    ptoc_Var136->d_colindex = 0;
                    ptoc_Var136->d_expand = 0;
                    ptoc_Var136->d_change_date_time = 0;
                    ak65not_first_val_expr(&(*acv) , &(*dmli) , &colin , act_node);
                    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var135->a_return_segm , "vak65.p" , 17702)).integer.C_4.sp1r_returncode == 21000)
                        {
                        (*(tsp1_segment *) sql__nil(ptoc_Var135->a_return_segm , "vak65.p" , 17705)).integer.C_4.sp1r_returncode = 0;
                        ptoc_Var136->d_is_parameter_only = 1;
                        a65_look_for_datatypes(&(*acv) , &(*dmli) , act_node);
                        if ((int ) ptoc_Var136->d_datatype == 14)
                            {
                            a07_b_put_error(&(*acv) , 6556 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17711))[sql__setrang(act_node , 0 , 32767)].n_pos);
                            }
                        else
                            {
                            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var135->a_return_segm , "vak65.p" , 17713)).integer.C_4.sp1r_returncode == 0)
                                {
                                colin.sci_len = 0;
                                ptoc_Var136->d_colptr = ptoc_Var136->d_colbuf;
                                ak65not_first_val_expr(&(*acv) , &(*dmli) , &colin , act_node);
                                }
                            }
                        }
                    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var135->a_return_segm , "vak65.p" , 17726)).integer.C_4.sp1r_returncode == 0)
                        {
                        if ((int ) field_found > 0)
                            {
                            ptoc_Var136->d_colptr = ptoc_Var136->d_colbuf;
                            }
                        if ((char ) sql__setrang(ptoc_Var136->d_join , 0 , 1))
                            {
                            ptoc_Var148 = &ptoc_Var136->d_joins;
                            ptoc_Var149 = &ptoc_Var148->jrc_joinarr[sql__setrang((int ) ptoc_Var148->jrc_cnt , 0 , 256)];
                            ptoc_Var150 = &ptoc_Var149->jo_recs[0];
                            ptoc_Var151 = (tgg00_QualBuf *) sql__nil(ptoc_Var135->a_mblock.integer.C_4.mb_qual , "vak65.p" , 17736);
                            if ((int ) ptoc_Var150->jop_tableno > 0 && !(char ) sql__setrang(ptoc_Var149->jo_no_join , 0 , 1))
                                {
                                ptoc_Var150->jop_cntstack = (short ) sql__setrang((short ) ((int ) ptoc_Var151->integer.C_2.mfirst_free - (int ) ptoc_Var150->jop_startstack) , -32768 , 32767);
                                ptoc_Var150->jop_outpos = 0;
                                if ((int ) ptoc_Var150->jop_cntstack > 1 || (ptoc_Var140 = (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var135->a_mblock.integer.C_5.mb_st , "vak65.p" , 17744))[sql__setrang((int ) ptoc_Var150->jop_startstack , 1 , 32767) + -1].integer.C_1.eop) != 0 && ptoc_Var140 != 28 && ptoc_Var140 != 29 && ptoc_Var140 != 19 && ptoc_Var140 != 20 && ptoc_Var140 != 2 && ptoc_Var140 != 7)
                                    {
                                    _ptoc_MOVE(unsigned char , 1, ptoc_Var150->jop_propset, ptoc_Var133);
                                    ptoc_Var152 = &colin;
                                    ptoc_Var150->jop_datatyp = (unsigned char ) sql__setrang(ptoc_Var152->sci_typ , 0 , 39);
                                    if ((int ) ptoc_Var152->sci_iolen >= 1024 - sizeof(int ))
                                        {
                                        ptoc_Var150->jop_datalen = 0;
                                        ptoc_Var150->jop_inoutlen = 0;
                                        }
                                    else
                                        {
                                        ptoc_Var150->jop_datalen = (short ) sql__setrang(ptoc_Var152->sci_len , -32768 , 32767);
                                        ptoc_Var150->jop_inoutlen = (short ) sql__setrang(ptoc_Var152->sci_iolen , -32768 , 32767);
                                        }
                                    if ((ptoc_Var140 = (int ) ptoc_Var150->jop_datatyp) == 0 || ptoc_Var140 == 1 || ptoc_Var140 == 12)
                                        {
                                        ptoc_Var150->jop_datatyp = 1;
                                        ptoc_Var150->jop_datalen = 38;
                                        ptoc_Var150->jop_inoutlen = 21;
                                        }
                                    }
                                }
                            else
                                {
                                if ((int ) ptoc_Var150->jop_tableno != 0)
                                    {
                                    ptoc_Var149->jo_no_join = 1;
                                    }
                                }
                            }
                        }
                    else
                        {
                        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var135->a_return_segm , "vak65.p" , 17791)).integer.C_4.sp1r_returncode == (int ) a07_return_code(6218 , (unsigned char ) sql__setrang(ptoc_Var135->a_sqlmode , 0 , 3)) && (int ) field_found > 0 && (((ptoc_Var140 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17796))[sql__setrang(*first_node , 0 , 32767)].n_symb) == 56 || ptoc_Var140 == 55) && (int ) ptoc_Var135->a_sqlmode != 0 || ((ptoc_Var140 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17800))[sql__setrang(*first_node , 0 , 32767)].n_symb) == 4 || ptoc_Var140 == 5 || ptoc_Var140 == 6 || ptoc_Var140 == 8 || ptoc_Var140 == 9 || ptoc_Var140 == 16) && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17804))[sql__setrang(sel_n , 0 , 32767)].n_symb == 31 && (int ) ptoc_Var135->a_sqlmode == 3))
                            {
                            (*(tsp1_segment *) sql__nil(ptoc_Var135->a_return_segm , "vak65.p" , 17808)).integer.C_4.sp1r_returncode = 0;
                            (*(tsp1_segment *) sql__nil(ptoc_Var135->a_return_segm , "vak65.p" , 17809)).integer.C_4.sp1r_errorpos = 0;
                            ptoc_Var136->d_colptr = ptoc_Var136->d_colbuf;
                            }
                        }
                    if (list_node != 0 || list_cnt > 1)
                        {
                        if (((ptoc_Var140 = (int ) ptoc_Var136->d_datatype) == 10 || ptoc_Var140 == 11 || ptoc_Var140 == 13) && dtype_n > 0)
                            {
                            startstack = (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var135->a_mblock.integer.C_4.mb_qual , "vak65.p" , 17822)).integer.C_2.mfirst_free;
                            a65ch_format(&(*acv) , &(*dmli) , &startstack , &colin , 2 , (unsigned char ) sql__setrang(ptoc_Var136->d_datatype , 0 , 39));
                            colin.sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) colin.sci_len , -32768 , 32767 , "vak65.p" , 17825) , -32768 , 32767);
                            }
                        if (dtype_n > 0)
                            {
                            ptoc_Var153 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17832))[sql__setrang(dtype_n , 0 , 32767)];
                            types_ok = (char ) sql__setrang(a65_datatypes_ok(&(*acv) , &(*dmli) , &ptoc_Var153->n_datatype , &ptoc_Var153->n_length , (unsigned char ) sql__setrang(ptoc_Var136->d_datatype , 0 , 39) , 1 , sel_n , (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17837))[sql__setrang(act_node , 0 , 32767)].n_pos , 1 , &convert_t) , 0 , 1);
                            if ((int ) convert_t != 0)
                                {
                                switch ((int ) convert_t)
                                    {
                                    case 1 :
                                    case 6 :
                                        ptoc_Var136->d_datatype = 3;
                                        break;
                                    case 2 :
                                    case 4 :
                                        ptoc_Var136->d_datatype = 2;
                                        break;
                                    case 3 :
                                    case 5 :
                                        ptoc_Var136->d_datatype = 24;
                                        break;
                                    default :
                                        break;
                                    }
                                if ((int ) ptoc_Var136->d_datatype == 24)
                                    {
                                    colin.sci_iolen = (short ) sql__setrang((short ) ((int ) colin.sci_len * 2 + 1) , -32768 , 32767);
                                    }
                                else
                                    {
                                    colin.sci_iolen = (short ) sql__setrang((short ) ((int ) colin.sci_len + 1) , -32768 , 32767);
                                    }
                                }
                            this_size = (int ) ptoc_Var153->n_length;
                            dtype_n = (int ) ptoc_Var153->n_sa_level;
                            a641string_set_operator(&(*acv) , 8 , this_size , 1 , (unsigned char)'\001' , (unsigned char)'\0');
                            }
                        else
                            {
                            this_size = (int ) colin.sci_iolen;
                            }
                        t01int4(5 , "this_size   " , this_size);
                        t01int4(5 , "sci_iolen   " , (int ) colin.sci_iolen);
                        t01int4(5 , "sci_len     " , (int ) colin.sci_len);
                        left_size = left_size + this_size;
                        if (list_cnt > 1 && dtype_n >= 0)
                            {
                            a641string_set_operator(&(*acv) , 4 , left_size , this_size , (unsigned char)'\001' , (unsigned char)'\0');
                            }
                        }
                    ptoc_Var154 = &(*list_info).elem[sql__setrang(list_cnt , 1 , 1024) + -1];
                    ptoc_Var154->datatype = (unsigned char ) sql__setrang(ptoc_Var136->d_datatype , 0 , 39);
                    ptoc_Var154->datalen = (short ) sql__setrang(colin.sci_len , -32768 , 32767);
                    ptoc_Var154->datafrac = (unsigned char ) sql__setrang((unsigned char ) ((int ) colin.sci_frac + 1) , 0 , 255);
                    ptoc_Var154->is_field = (char ) sql__setrang((int ) field_found > 0 , 0 , 1);
                    ptoc_Var154->inout_len = (short ) sql__setrang(colin.sci_iolen , -32768 , 32767);
                    ptoc_Var154->pos = (*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17896))[sql__setrang(act_node , 0 , 32767)].n_pos;
                    if (list_node != 0)
                        {
                        ptoc_Var155 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17901))[sql__setrang(list_node , 0 , 32767)];
                        list_cnt = sql__succ(list_cnt , -2147483647 , 2147483647 , "vak65.p" , 17903);
                        ptoc_Var136->d_datatype = 14;
                        act_node = (int ) ptoc_Var155->n_lo_level;
                        list_node = (int ) ptoc_Var155->n_sa_level;
                        }
                    else
                        {
                        (*list_info).count = list_cnt;
                        list_cnt = 0;
                        }
                    }
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var135->a_return_segm , "vak65.p" , 17917)).integer.C_4.sp1r_returncode == 0)
                    {
                    ptoc_Var156 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var135->a_ap_tree , "vak65.p" , 17919))[sql__setrang(*first_node , 0 , 32767)];
                    if ((ptoc_Var140 = (int ) ptoc_Var156->n_symb) == 4 || ptoc_Var140 == 5 || ptoc_Var140 == 6 || ptoc_Var140 == 8 || ptoc_Var140 == 9 || ptoc_Var140 == 16 || ptoc_Var140 == 56 || ptoc_Var140 == 55 || ptoc_Var140 == 157 || ptoc_Var140 == 159)
                        {
                        ak65compare_predicate(&(*acv) , &(*dmli) , &colin , *first_node , &snot_pred , (short ) sql__setrang(field_found , -32768 , 32767) , &(*list_info));
                        }
                    else
                        {
                        ak65diff_predicates(&(*acv) , &(*dmli) , &colin , &(*first_node) , &snot_pred , (short ) sql__setrang(field_found , -32768 , 32767) , &(*list_info));
                        }
                    ptoc_Var157 = &ptoc_Var136->d_joins;
                    ptoc_Var158 = &ptoc_Var157->jrc_joinarr[sql__setrang((int ) ptoc_Var157->jrc_cnt , 0 , 256)];
                    if ((char ) sql__setrang(ptoc_Var158->jo_no_join , 0 , 1))
                        {
                        ptoc_Var136->d_arith_where = 1;
                        }
                    }
                }
            }
        if ((char ) sql__setrang(snot_pred , 0 , 1) || (char ) sql__setrang(snot_subq , 0 , 1))
            {
            a65_set_operator(&(*acv) , 96);
            _ptoc_MOVE(unsigned char , 8, used_tables, ptoc_Var134);
            used_tab_cnt = 0;
            if ((char ) sql__setrang(ptoc_Var136->d_join , 0 , 1))
                {
                if ((int ) (*dmli).d_joins.jrc_cnt > oldjoincnt + 1)
                    {
                    xi = oldjoincnt;
                    while (xi < (int ) (*dmli).d_joins.jrc_cnt)
                        {
                        ptoc_Var159 = &*dmli;
                        ptoc_Var160 = &ptoc_Var159->d_joins;
                        ptoc_Var161 = &ptoc_Var160->jrc_joinarr[sql__setrang(xi , 0 , 256)];
                        if ((ptoc_Var140 = (int ) ptoc_Var161->jo_recs[0].jop_tableno + -1) < 0 || ptoc_Var140 > 63 || (used_tables[ptoc_Var140 >> 3] & 1 << (ptoc_Var140 & 7)) == 0)
                            {
                            used_tab_cnt = sql__succ(used_tab_cnt , -2147483647 , 2147483647 , "vak65.p" , 17975);
                            sql__psetadd(used_tables , used_tables , sql__pcnst(ptoc_Var162 , 1 , 63 , 0 , 1 , (int ) ptoc_Var161->jo_recs[0].jop_tableno) , sizeof(tak_joinset ));
                            }
                        if ((int ) ptoc_Var161->jo_recs[1].jop_tableno != 99 && ((ptoc_Var140 = (int ) ptoc_Var161->jo_recs[1].jop_tableno + -1) < 0 || ptoc_Var140 > 63 || (used_tables[ptoc_Var140 >> 3] & 1 << (ptoc_Var140 & 7)) == 0))
                            {
                            used_tab_cnt = sql__succ(used_tab_cnt , -2147483647 , 2147483647 , "vak65.p" , 17985);
                            sql__psetadd(used_tables , used_tables , sql__pcnst(ptoc_Var162 , 1 , 63 , 0 , 1 , (int ) ptoc_Var161->jo_recs[1].jop_tableno) , sizeof(tak_joinset ));
                            }
                        if (used_tab_cnt > 1)
                            {
                            (*dmli).d_arith_where = 1;
                            xi = (int ) (*dmli).d_joins.jrc_cnt;
                            }
                        xi = sql__succ(xi , -2147483647 , 2147483647 , "vak65.p" , 17997);
                        }
                    }
                }
            }
        if ((char ) sql__setrang(ptoc_Var136->d_join , 0 , 1))
            {
            ak65analyze_joinarr(&(*acv) , &(*dmli) , *first_node , act_node);
            if ((char ) sql__setrang(snot_pred , 0 , 1) || (char ) sql__setrang(snot_subq , 0 , 1))
                {
                ptoc_Var136->d_joins.jrc_cnt = (short ) sql__setrang((short ) oldjoincnt , -32768 , 32767);
                ptoc_Var163 = &ptoc_Var136->d_joins;
                ptoc_Var164 = &ptoc_Var163->jrc_joinarr[sql__setrang((int ) ptoc_Var163->jrc_cnt , 0 , 256)];
                ptoc_Var164->jo_no_join = 0;
                ptoc_Var164->jo_partno = (unsigned char)'\001';
                ptoc_Var164->jo_recs[0].jop_tableno = (unsigned char)'\0';
                ptoc_Var164->jo_recs[1].jop_tableno = (unsigned char)'\0';
                ptoc_Var164->jo_recs[0].jop_outer_join = 0;
                ptoc_Var164->jo_recs[1].jop_outer_join = 0;
                }
            }
        if ((char ) sql__setrang(ptoc_Var136->d_join , 0 , 1))
            {
            a683_output(5 , &(*dmli));
            }
        }
    cccprint("<ak65predicate  ");
    }



int ak65s_num_into_tree(acv, new_n, left_node, right_node)
tak_all_command_glob *acv;
tsp_int2 *new_n;
int left_node;
int right_node;
    {
    tsp_int2 put_node;
    tak_all_command_glob *ptoc_Var165;
    tak_scanner_glob *ptoc_Var166;
    struct ptoc_Type56 *ptoc_Var167;
    cccprint("AK65  ak65s_num_");
    ptoc_Var165 = &*acv;
    ptoc_Var166 = &ptoc_Var165->a_scv;
    ptoc_Var166->sc_symb = 75;
    ptoc_Var166->sc_sypos = (*(tak_ap_max_tree *) sql__nil(ptoc_Var165->a_ap_tree , "vak65.p" , 18054))[sql__setrang(left_node , 0 , 32767)].n_pos;
    a01_function_put(&(*acv) , &put_node);
    ptoc_Var167 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var165->a_ap_tree , "vak65.p" , 18056))[sql__setrang((int ) put_node , 0 , 32767)];
    ptoc_Var167->n_sa_level = (short ) sql__setrang((short ) right_node , -32768 , 32767);
    ptoc_Var167->n_lo_level = (short ) sql__setrang((short ) left_node , -32768 , 32767);
    *new_n = (short ) sql__setrang(put_node , -32768 , 32767);
    put_node = (short ) sql__setrang((short ) left_node , -32768 , 32767);
    while ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var165->a_ap_tree , "vak65.p" , 18064))[sql__setrang((int ) put_node , 0 , 32767)].n_sa_level != right_node)
        put_node = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var165->a_ap_tree , "vak65.p" , 18065))[sql__setrang((int ) put_node , 0 , 32767)].n_sa_level , -32768 , 32767);
    (*(tak_ap_max_tree *) sql__nil(ptoc_Var165->a_ap_tree , "vak65.p" , 18067))[sql__setrang((int ) put_node , 0 , 32767)].n_sa_level = 0;
    cccprint("<ak65s_num_into_");
    }



int ak65se_condition(acv, dmli, first_node, mtabs_allowed, list_info)
tak_all_command_glob *acv;
tak_dml_info *dmli;
int *first_node;
char mtabs_allowed;
list_elem_info *list_info;
    {
    char snot_se_con;
    int act_node;
    int oldjoincnt;
    int stentryno;
    tak_all_command_glob *ptoc_Var168;
    struct tgg00_MessBlock *ptoc_Var169;
    tgg00_QualBuf *ptoc_Var170;
    tak_dml_info *ptoc_Var171;
    struct ptoc_Type56 *ptoc_Var172;
    tak_joinrec *ptoc_Var173;
    struct ptoc_Type110 *ptoc_Var174;
    tak_joinrec *ptoc_Var175;
    struct ptoc_Type110 *ptoc_Var176;
    cccprint("AK65  ak65se_con");
    ptoc_Var168 = &*acv;
    ptoc_Var169 = &ptoc_Var168->a_mblock;
    ptoc_Var170 = (tgg00_QualBuf *) sql__nil(ptoc_Var169->integer.C_4.mb_qual , "vak65.p" , 18088);
    ptoc_Var171 = &*dmli;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var168->a_return_segm , "vak65.p" , 18089)).integer.C_4.sp1r_returncode == 0)
        {
        if ((char ) sql__setrang(ptoc_Var171->d_join , 0 , 1))
            {
            a683_output(5 , &(*dmli));
            }
        if ((char ) sql__setrang(ptoc_Var171->d_join , 0 , 1))
            {
            oldjoincnt = (int ) ptoc_Var171->d_joins.jrc_cnt;
            }
        ptoc_Var171->d_joins.jrc_joinarr[sql__setrang((int ) ptoc_Var171->d_joins.jrc_cnt , 0 , 256)].jo_col_upd_stat = (unsigned char)'\0';
        snot_se_con = (char ) sql__setrang(ak65subq_se(&(*acv) , &(*first_node)) , 0 , 1);
        ptoc_Var172 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var168->a_ap_tree , "vak65.p" , 18102))[sql__setrang(*first_node , 0 , 32767)];
        if ((int ) ptoc_Var172->n_symb != 52)
            {
            if ((char ) sql__setrang(snot_se_con , 0 , 1))
                {
                if ((char ) sql__setrang(mtabs_allowed , 0 , 1))
                    {
                    ptoc_Var171->d_first_tab = 0;
                    }
                ak65boolean_term(&(*acv) , &(*dmli) , &(*first_node) , 0 , &(*list_info));
                if ((int ) ptoc_Var171->d_first_tab < 0)
                    {
                    ptoc_Var171->d_arith_where = 1;
                    }
                if ((char ) sql__setrang(ptoc_Var171->d_join , 0 , 1))
                    {
                    if ((int ) ptoc_Var171->d_joins.jrc_cnt > oldjoincnt && !ak65only_jvalue_found(&(*dmli) , oldjoincnt))
                        {
                        ptoc_Var171->d_arith_where = 1;
                        ptoc_Var171->d_joins.jrc_cnt = (short ) sql__setrang((short ) oldjoincnt , -32768 , 32767);
                        ptoc_Var171->d_joins.jrc_joinarr[sql__setrang((int ) ptoc_Var171->d_joins.jrc_cnt , 0 , 256)].jo_col_upd_stat = (unsigned char)'\0';
                        ptoc_Var173 = &ptoc_Var171->d_joins;
                        ptoc_Var174 = &ptoc_Var173->jrc_joinarr[sql__setrang((int ) ptoc_Var173->jrc_cnt , 0 , 256)];
                        ptoc_Var174->jo_no_join = 0;
                        ptoc_Var174->jo_recs[0].jop_tableno = (unsigned char)'\0';
                        ptoc_Var174->jo_recs[1].jop_tableno = (unsigned char)'\0';
                        }
                    }
                }
            else
                {
                ak65boolean_term(&(*acv) , &(*dmli) , &(*first_node) , (char ) sql__setrang(mtabs_allowed , 0 , 1) , &(*list_info));
                }
            }
        else
            {
            act_node = (int ) ptoc_Var172->n_lo_level;
            ak65boolean_term(&(*acv) , &(*dmli) , &act_node , 0 , &(*list_info));
            stentryno = (int ) ptoc_Var170->integer.C_2.mfirst_free;
            if (stentryno >= (int ) ptoc_Var169->integer.C_2.mb_st_max)
                {
                a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var169->integer.C_2.mb_st_max);
                }
            else
                {
                ptoc_Var170->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var170->integer.C_2.mqual_cnt , -32768 , 32767 , "vak65.p" , 18156) , -32768 , 32767);
                ptoc_Var170->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var170->integer.C_2.mfirst_free , -32768 , 32767 , "vak65.p" , 18157) , -32768 , 32767);
                act_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var168->a_ap_tree , "vak65.p" , 18158))[sql__setrang(act_node , 0 , 32767)].n_sa_level;
                ak65boolean_term(&(*acv) , &(*dmli) , &act_node , 0 , &(*list_info));
                a65_set_operator(&(*acv) , 99);
                a61_set_jump(&ptoc_Var168->a_mblock , stentryno , 31);
                }
            if ((char ) sql__setrang(ptoc_Var171->d_join , 0 , 1))
                {
                if ((int ) ptoc_Var171->d_first_tab < 0)
                    {
                    ptoc_Var171->d_arith_where = 1;
                    }
                if ((int ) ptoc_Var171->d_joins.jrc_cnt > oldjoincnt && !ak65only_jvalue_found(&(*dmli) , oldjoincnt))
                    {
                    if ((char ) sql__setrang(ptoc_Var171->d_outer_join , 0 , 1) && ak65outer_cond_found(&(*dmli) , oldjoincnt))
                        {
                        a07_b_put_error(&(*acv) , 6379 , 1);
                        }
                    else
                        {
                        ptoc_Var171->d_arith_where = 1;
                        ptoc_Var171->d_joins.jrc_cnt = (short ) sql__setrang((short ) oldjoincnt , -32768 , 32767);
                        ptoc_Var175 = &ptoc_Var171->d_joins;
                        ptoc_Var176 = &ptoc_Var175->jrc_joinarr[sql__setrang((int ) ptoc_Var175->jrc_cnt , 0 , 256)];
                        ptoc_Var176->jo_col_upd_stat = (unsigned char)'\0';
                        ptoc_Var176->jo_no_join = 0;
                        ptoc_Var176->jo_recs[0].jop_tableno = (unsigned char)'\0';
                        ptoc_Var176->jo_recs[1].jop_tableno = (unsigned char)'\0';
                        }
                    }
                }
            }
        if ((char ) sql__setrang(snot_se_con , 0 , 1))
            {
            a65_set_operator(&(*acv) , 96);
            }
        }
    cccprint("<ak65se_conditio");
    }



int ak65set_like_operator(acv, operator, like_expression, ord_typ, escape)
tak_all_command_glob *acv;
tgg_stack_op_type operator;
char like_expression;
int ord_typ;
char escape;
    {
    tak_all_command_glob *ptoc_Var177;
    struct tgg00_MessBlock *ptoc_Var178;
    tgg00_QualBuf *ptoc_Var179;
    struct ptoc_Type38 *ptoc_Var180;
    cccprint("AK65  ak65set_li");
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak65.p" , 18221)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var177 = &*acv;
        ptoc_Var178 = &ptoc_Var177->a_mblock;
        ptoc_Var179 = (tgg00_QualBuf *) sql__nil(ptoc_Var178->integer.C_4.mb_qual , "vak65.p" , 18223);
        if ((int ) ptoc_Var179->integer.C_2.mfirst_free > (int ) ptoc_Var178->integer.C_2.mb_st_max)
            {
            a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var178->integer.C_2.mb_st_max);
            }
        else
            {
            ptoc_Var179->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var179->integer.C_2.mqual_cnt , -32768 , 32767 , "vak65.p" , 18230) , -32768 , 32767);
            ptoc_Var180 = &(*(tgg00_StackList *) sql__nil(ptoc_Var178->integer.C_5.mb_st , "vak65.p" , 18231))[sql__setrang((int ) ptoc_Var179->integer.C_2.mfirst_free , 1 , 32767) + -1];
            ptoc_Var180->integer.C_1.etype = 26;
            ptoc_Var180->integer.C_1.eop = (unsigned char ) sql__setrang(operator , 0 , 123);
            ptoc_Var180->integer.C_1.epos = (short ) sql__setrang((short ) sql__ord((int ) (*acv).a_sqlmode) , -32768 , 32767);
            ptoc_Var180->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
            if ((char ) sql__setrang(like_expression , 0 , 1))
                {
                ptoc_Var180->integer.C_1.elen_var = 1;
                ptoc_Var180->integer.C_1.ecol_tab[0] = (unsigned char ) sql__setrang((unsigned char ) ord_typ , 0 , 255);
                if ((char ) sql__setrang(escape , 0 , 1))
                    {
                    ptoc_Var180->integer.C_1.ecol_tab[1] = (unsigned char)'\001';
                    }
                }
            else
                {
                ptoc_Var180->integer.C_1.elen_var = 0;
                ptoc_Var180->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                }
            ptoc_Var179->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var179->integer.C_2.mfirst_free , -32768 , 32767 , "vak65.p" , 18255) , -32768 , 32767);
            }
        }
    cccprint("<ak65set_like_op");
    }



int ak65sub_op_set(acv, all, any, operator, first_node)
tak_all_command_glob *acv;
char all;
char any;
tgg_stack_op_type operator;
int first_node;
    {
    int subquery_count;
    tgg00_FileId itree;
    tak_all_command_glob *ptoc_Var181;
    struct tgg00_MessBlock *ptoc_Var182;
    tgg00_QualBuf *ptoc_Var183;
    struct ptoc_Type38 *ptoc_Var184;
    struct tgg00_MessBlock *ptoc_Var185;
    tgg00_DataPart *ptoc_Var186;
    cccprint("AK65  ak65sub_op");
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak65.p" , 18277)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var181 = &*acv;
        ptoc_Var182 = &ptoc_Var181->a_mblock;
        ptoc_Var183 = (tgg00_QualBuf *) sql__nil(ptoc_Var182->integer.C_4.mb_qual , "vak65.p" , 18279);
        if ((int ) ptoc_Var183->integer.C_2.mfirst_free > (int ) ptoc_Var182->integer.C_2.mb_st_max)
            {
            a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var182->integer.C_2.mb_st_max);
            }
        else
            {
            subquery_count = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak65.p" , 18286))[sql__setrang(first_node , 0 , 32767)].n_length;
            ptoc_Var183->integer.C_2.msubquery = 1;
            ptoc_Var184 = &(*(tgg00_StackList *) sql__nil(ptoc_Var182->integer.C_5.mb_st , "vak65.p" , 18288))[sql__setrang((int ) ptoc_Var183->integer.C_2.mfirst_free , 1 , 32767) + -1];
            ptoc_Var183->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var183->integer.C_2.mqual_cnt , -32768 , 32767 , "vak65.p" , 18290) , -32768 , 32767);
            ptoc_Var184->integer.C_1.etype = 56;
            ptoc_Var184->integer.C_1.eop = (unsigned char ) sql__setrang(operator , 0 , 123);
            ptoc_Var185 = &ptoc_Var181->a_mblock;
            ptoc_Var186 = (tgg00_DataPart *) sql__nil(ptoc_Var185->integer.C_3.mb_data , "vak65.p" , 18293);
            a660construct_subtree(&(*acv) , &itree , subquery_count);
            t01int4(5 , "first_node  " , first_node);
            if (ptoc_Var185->integer.C_2.mb_data_len + 40 <= ptoc_Var185->integer.C_2.mb_data_size)
                {
                g10mv1("VAK65 " , 4 , sizeof(tgg00_FileId ) , ptoc_Var185->integer.C_2.mb_data_size , &itree , 1 , ptoc_Var186->integer.C_1.mbp_buf , ptoc_Var185->integer.C_2.mb_data_len + 1 , sizeof(tgg00_FileId ) , &(*(tsp1_segment *) sql__nil(ptoc_Var181->a_return_segm , "vak65.p" , 18303)).integer.C_4.sp1r_returncode);
                ptoc_Var184->integer.C_1.epos = (short ) sql__setrang((short ) (ptoc_Var185->integer.C_2.mb_data_len + 1) , -32768 , 32767);
                ptoc_Var185->integer.C_2.mb_data_len = ptoc_Var185->integer.C_2.mb_data_len + 40;
                }
            else
                {
                a07_b_put_error(&(*acv) , 2801 , 1);
                }
            ptoc_Var184->integer.C_1.elen_var = -8;
            if ((char ) sql__setrang(all , 0 , 1))
                {
                ptoc_Var184->integer.C_1.ecol_tab[0] = (unsigned char)'\001';
                }
            else
                {
                ptoc_Var184->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                }
            if ((char ) sql__setrang(any , 0 , 1))
                {
                ptoc_Var184->integer.C_1.ecol_tab[1] = (unsigned char)'\001';
                }
            else
                {
                ptoc_Var184->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                }
            }
        ptoc_Var183->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var183->integer.C_2.mfirst_free , -32768 , 32767 , "vak65.p" , 18329) , -32768 , 32767);
        }
    cccprint("<ak65sub_op_set ");
    }



int a65subq_get_val(acv, first_node, iolen)
tak_all_command_glob *acv;
int first_node;
int iolen;
    {
    int subquery_count;
    int needed_len;
    tgg00_FileId itree;
    tak_all_command_glob *ptoc_Var187;
    struct tgg00_MessBlock *ptoc_Var188;
    tgg00_QualBuf *ptoc_Var189;
    struct ptoc_Type38 *ptoc_Var190;
    struct tgg00_MessBlock *ptoc_Var191;
    tgg00_DataPart *ptoc_Var192;
    cccprint("AK65  a65subq_ge");
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak65.p" , 18351)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var187 = &*acv;
        ptoc_Var188 = &ptoc_Var187->a_mblock;
        ptoc_Var189 = (tgg00_QualBuf *) sql__nil(ptoc_Var188->integer.C_4.mb_qual , "vak65.p" , 18353);
        if ((int ) ptoc_Var189->integer.C_2.mfirst_free > (int ) ptoc_Var188->integer.C_2.mb_st_max)
            {
            a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var188->integer.C_2.mb_st_max);
            }
        else
            {
            subquery_count = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak65.p" , 18360))[sql__setrang(first_node , 0 , 32767)].n_length;
            ptoc_Var189->integer.C_2.msubquery = 1;
            ptoc_Var190 = &(*(tgg00_StackList *) sql__nil(ptoc_Var188->integer.C_5.mb_st , "vak65.p" , 18362))[sql__setrang((int ) ptoc_Var189->integer.C_2.mfirst_free , 1 , 32767) + -1];
            ptoc_Var189->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var189->integer.C_2.mqual_cnt , -32768 , 32767 , "vak65.p" , 18364) , -32768 , 32767);
            ptoc_Var190->integer.C_1.etype = 56;
            ptoc_Var190->integer.C_1.eop = 34;
            ptoc_Var191 = &ptoc_Var187->a_mblock;
            ptoc_Var192 = (tgg00_DataPart *) sql__nil(ptoc_Var191->integer.C_3.mb_data , "vak65.p" , 18367);
            a660construct_subtree(&(*acv) , &itree , subquery_count);
            t01int4(5 , "first_node  " , first_node);
            if (iolen < 40)
                {
                needed_len = 40;
                }
            else
                {
                needed_len = iolen;
                }
            if (ptoc_Var191->integer.C_2.mb_data_len + needed_len <= ptoc_Var191->integer.C_2.mb_data_size)
                {
                g10mv1("VAK65 " , 5 , sizeof(tgg00_FileId ) , ptoc_Var191->integer.C_2.mb_data_size , &itree , 1 , ptoc_Var192->integer.C_1.mbp_buf , ptoc_Var191->integer.C_2.mb_data_len + 1 , sizeof(tgg00_FileId ) , &(*(tsp1_segment *) sql__nil(ptoc_Var187->a_return_segm , "vak65.p" , 18383)).integer.C_4.sp1r_returncode);
                ptoc_Var190->integer.C_1.epos = (short ) sql__setrang((short ) (ptoc_Var191->integer.C_2.mb_data_len + 1) , -32768 , 32767);
                ptoc_Var191->integer.C_2.mb_data_len = ptoc_Var191->integer.C_2.mb_data_len + needed_len;
                }
            else
                {
                a07_b_put_error(&(*acv) , 2801 , 1);
                }
            ptoc_Var190->integer.C_1.elen_var = -8;
            ptoc_Var190->integer.C_2.ecol_pos = (short ) sql__setrang((short ) iolen , -32768 , 32767);
            }
        ptoc_Var189->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var189->integer.C_2.mfirst_free , -32768 , 32767 , "vak65.p" , 18398) , -32768 , 32767);
        }
    cccprint("<a65subq_get_val");
    }



int ak65try_like_optimization(acv, dmli, first_node, operator, col_len, keep_part_pos)
tak_all_command_glob *acv;
tak_dml_info *dmli;
int *first_node;
tgg_stack_op_type operator;
int col_len;
int keep_part_pos;
    {
    char rest_blanks;
    char star_any_class_found;
    tsp_int2 ix;
    tsp_int4 i;
    tsp_int4 j;
    tsp_int4 k;
    tsp_int4 curr_data_len;
    tsp_int4 star_offset;
    tsp_int4 star_found_len;
    tsp_int4 value_len;
    tak00_scolinf colin;
    struct tgg00_MessBlock *ptoc_Var193;
    tgg00_DataPart *ptoc_Var194;
    int ptoc_Var195;
    tgg00_QualBuf *ptoc_Var196;
    int ptoc_Var197;
    cccprint("AK65  ak65try_li");
    ptoc_Var193 = &(*acv).a_mblock;
    ptoc_Var194 = (tgg00_DataPart *) sql__nil(ptoc_Var193->integer.C_3.mb_data , "vak65.p" , 18427);
    if ((int ) (*acv).a_ex_kind == 0 && (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak65.p" , 18430))[sql__setrang(*first_node , 0 , 32767)].n_symb == 46)
        {
        (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18443))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var193->integer.C_4.mb_qual , "vak65.p" , 18443)).integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.etype = 39;
        colin.sci_len = 0;
        (*dmli).d_like_part = (unsigned char)'\002';
        a640not_first_factor(&(*acv) , &(*dmli) , &colin , &(*first_node));
        (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18447))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var193->integer.C_4.mb_qual , "vak65.p" , 18447)).integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.etype = 39;
        colin.sci_len = 0;
        (*dmli).d_like_part = (unsigned char)'\003';
        a640not_first_factor(&(*acv) , &(*dmli) , &colin , &(*first_node));
        ak65set_like_operator(&(*acv) , (unsigned char ) sql__setrang(operator , 0 , 123) , 0 , 0 , 0);
        (*dmli).d_like = 0;
        }
    else
        {
        if ((ptoc_Var195 = (int ) (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak65.p" , 18456))[sql__setrang(*first_node , 0 , 32767)].n_symb) != 43 && ptoc_Var195 != 44 && ptoc_Var195 != 45)
            {
            ptoc_Var196 = (tgg00_QualBuf *) sql__nil(ptoc_Var193->integer.C_4.mb_qual , "vak65.p" , 18459);
            k = (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18461))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.elen_var;
            if (k > 1)
                {
                curr_data_len = ptoc_Var193->integer.C_2.mb_data_len;
                i = curr_data_len - k + 2;
                star_any_class_found = 0;
                star_found_len = 0;
                if ((int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18469))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.epos , 1 , 8192000) + -1] == 1)
                    {
                    i = sql__succ(i , -2147483647 , 2147483647 , "vak65.p" , 18473);
                    while (i <= curr_data_len && !(char ) sql__setrang(star_any_class_found , 0 , 1))
                        if (((int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 31 || (int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 30 || (int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 29) && (int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i + -1 , 1 , 8192000) + -1] == 0)
                            {
                            star_any_class_found = 1;
                            star_found_len = i - (curr_data_len - k);
                            ix = 2;
                            }
                        else
                            {
                            i = i + 2;
                            }
                    }
                else
                    {
                    while (i <= curr_data_len && !(char ) sql__setrang(star_any_class_found , 0 , 1))
                        if ((int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 31 || (int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 30 || (int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 29)
                            {
                            star_any_class_found = 1;
                            star_found_len = i - (curr_data_len - k);
                            ix = 1;
                            }
                        else
                            {
                            i = sql__succ(i , -2147483647 , 2147483647 , "vak65.p" , 18504);
                            }
                    }
                t01int4(5 , "col_len     " , col_len);
                t01int4(5 , "star_fou_len" , star_found_len);
                if ((char ) sql__setrang(star_any_class_found , 0 , 1))
                    {
                    t01int4(5 , "*/? found at" , i);
                    }
                else
                    {
                    t01int4(5 , "*/? notfound" , i);
                    }
                t01moveobj(5 , ptoc_Var194->integer.C_1.mbp_buf , curr_data_len - k + 1 , curr_data_len);
                if (star_found_len > col_len)
                    {
                    if ((char ) sql__setrang(star_any_class_found , 0 , 1) && (int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 31 && k == star_found_len && col_len + (int ) ix == k)
                        {
                        (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18526))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.elen_var = (short ) sql__setrang((short ) ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18527))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.elen_var - (int ) ix) , -32768 , 32767);
                        star_any_class_found = 0;
                        }
                    else
                        {
                        value_len = k;
                        }
                    }
                else
                    {
                    value_len = col_len;
                    }
                if (!(char ) sql__setrang(star_any_class_found , 0 , 1))
                    {
                    if ((int ) operator == 101)
                        {
                        a65_set_operator(&(*acv) , 115);
                        }
                    else
                        {
                        a65_set_operator(&(*acv) , 109);
                        }
                    (*dmli).d_like = 0;
                    }
                else
                    {
                    j = i + 1;
                    rest_blanks = 1;
                    if ((int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18555))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.epos , 1 , 8192000) + -1] == 1)
                        {
                        while (j <= curr_data_len && (char ) sql__setrang(rest_blanks , 0 , 1))
                            if ((int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(j , 1 , 8192000) + -1] != 0 || (int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(j + 1 , 1 , 8192000) + -1] != 32)
                                {
                                rest_blanks = 0;
                                }
                            else
                                {
                                j = j + 2;
                                }
                        }
                    else
                        {
                        while (j <= curr_data_len && (char ) sql__setrang(rest_blanks , 0 , 1))
                            if ((int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(j , 1 , 8192000) + -1] != (int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(curr_data_len - k + 1 , 1 , 8192000) + -1])
                                {
                                rest_blanks = 0;
                                }
                            else
                                {
                                j = sql__succ(j , -2147483647 , 2147483647 , "vak65.p" , 18573);
                                }
                        }
                    if ((int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 31 && (char ) sql__setrang(rest_blanks , 0 , 1) && ptoc_Var193->integer.C_2.mb_data_len + value_len - k + value_len <= ptoc_Var193->integer.C_2.mb_data_size)
                        {
                        star_offset = i - (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18594))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.epos;
                        ptoc_Var193->integer.C_2.mb_data_len = ptoc_Var193->integer.C_2.mb_data_len + value_len - k;
                        (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18596))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.elen_var = (short ) sql__setrang((short ) value_len , -32768 , 32767);
                        if ((int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18598))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.epos , 1 , 8192000) + -1] == 1)
                            {
                            star_offset = sql__pred(star_offset , -2147483647 , 2147483647 , "vak65.p" , 18605);
                            g10filuni("VAK65 " , 6 , ptoc_Var193->integer.C_2.mb_data_size , ptoc_Var194->integer.C_1.mbp_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18608))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.epos + star_offset , value_len - star_offset + 1 , "\0 " , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak65.p" , 18611)).integer.C_4.sp1r_returncode);
                            }
                        else
                            {
                            g10fil("VAK65 " , 7 , ptoc_Var193->integer.C_2.mb_data_size , ptoc_Var194->integer.C_1.mbp_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18616))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.epos + star_offset , value_len - star_offset , (unsigned char ) sql__setrang(ptoc_Var194->integer.C_1.mbp_buf[sql__setrang((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18618))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.epos , 1 , 8192000) + -1] , 0 , 255) , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak65.p" , 18619)).integer.C_4.sp1r_returncode);
                            }
                        (*acv).a_input_data_pos = keep_part_pos;
                        colin.sci_len = 0;
                        a640not_first_factor(&(*acv) , &(*dmli) , &colin , &(*first_node));
                        ptoc_Var193->integer.C_2.mb_data_len = ptoc_Var193->integer.C_2.mb_data_len + value_len - k;
                        (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18625))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.elen_var = (short ) sql__setrang((short ) value_len , -32768 , 32767);
                        g10fil("VAK65 " , 8 , ptoc_Var193->integer.C_2.mb_data_size , ptoc_Var194->integer.C_1.mbp_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18629))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.epos + star_offset , value_len - star_offset , (unsigned char)'\377' , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak65.p" , 18631)).integer.C_4.sp1r_returncode);
                        if ((int ) operator == 101)
                            {
                            a65_set_operator(&(*acv) , 95);
                            }
                        else
                            {
                            a65_set_operator(&(*acv) , 94);
                            }
                        (*dmli).d_like = 0;
                        }
                    else
                        {
                        if ((int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18641))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.epos , 1 , 8192000) + -1] == 1 && (int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 0 && (int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i + 1 , 1 , 8192000) + -1] == 29 && (int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i + 4 , 1 , 8192000) + -1] == 0 && (int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i + 5 , 1 , 8192000) + -1] == 28 && (int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i + 8 , 1 , 8192000) + -1] == 0 && (int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i + 9 , 1 , 8192000) + -1] == 29 || (int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18650))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.epos , 1 , 8192000) + -1] != 1 && (int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 29 && (int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i + 2 , 1 , 8192000) + -1] == 28 && (int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i + 4 , 1 , 8192000) + -1] == 29)
                            {
                            if ((int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18656))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.epos , 1 , 8192000) + -1] == 1)
                                {
                                j = i + 11;
                                rest_blanks = 1;
                                while (j <= curr_data_len && (char ) sql__setrang(rest_blanks , 0 , 1))
                                    if ((int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(j + -1 , 1 , 8192000) + -1] != 0 || (int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(j , 1 , 8192000) + -1] != 32)
                                        {
                                        rest_blanks = 0;
                                        }
                                    else
                                        {
                                        j = j + 2;
                                        }
                                if ((char ) sql__setrang(rest_blanks , 0 , 1))
                                    {
                                    ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] = (unsigned char ) sql__setrang(ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i + 2 , 1 , 8192000) + -1] , 0 , 255);
                                    ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i + 1 , 1 , 8192000) + -1] = (unsigned char ) sql__setrang(ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i + 3 , 1 , 8192000) + -1] , 0 , 255);
                                    g10filuni("VAK65 " , 9 , ptoc_Var193->integer.C_2.mb_data_size , ptoc_Var194->integer.C_1.mbp_buf , i + 2 , curr_data_len - i + -1 , "\0 " , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak65.p" , 18687)).integer.C_4.sp1r_returncode);
                                    j = curr_data_len - i;
                                    (*acv).a_input_data_pos = keep_part_pos;
                                    colin.sci_len = 0;
                                    a640not_first_factor(&(*acv) , &(*dmli) , &colin , &(*first_node));
                                    curr_data_len = ptoc_Var193->integer.C_2.mb_data_len;
                                    i = curr_data_len - j;
                                    ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] = (unsigned char ) sql__setrang(ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i + 6 , 1 , 8192000) + -1] , 0 , 255);
                                    ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i + 1 , 1 , 8192000) + -1] = (unsigned char ) sql__setrang(ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i + 7 , 1 , 8192000) + -1] , 0 , 255);
                                    g10filuni("VAK65 " , 10 , ptoc_Var193->integer.C_2.mb_data_size , ptoc_Var194->integer.C_1.mbp_buf , i + 2 , curr_data_len - i + -1 , "\0 " , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak65.p" , 18700)).integer.C_4.sp1r_returncode);
                                    if ((int ) operator == 101)
                                        {
                                        a65_set_operator(&(*acv) , 95);
                                        }
                                    else
                                        {
                                        a65_set_operator(&(*acv) , 94);
                                        }
                                    (*dmli).d_like = 0;
                                    }
                                }
                            else
                                {
                                j = i + 5;
                                rest_blanks = 1;
                                while (j <= curr_data_len && (char ) sql__setrang(rest_blanks , 0 , 1))
                                    if ((int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(j , 1 , 8192000) + -1] != (int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(curr_data_len - k + 1 , 1 , 8192000) + -1])
                                        {
                                        rest_blanks = 0;
                                        }
                                    else
                                        {
                                        j = sql__succ(j , -2147483647 , 2147483647 , "vak65.p" , 18720);
                                        }
                                if ((char ) sql__setrang(rest_blanks , 0 , 1))
                                    {
                                    ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] = (unsigned char ) sql__setrang(ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i + 1 , 1 , 8192000) + -1] , 0 , 255);
                                    ptoc_Var195 = i + 1;
                                    ptoc_Var197 = curr_data_len;
                                    if (ptoc_Var195 <= ptoc_Var197)
                                        {
                                        j = ptoc_Var195;
                                        do
                                            {
                                            ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(j , 1 , 8192000) + -1] = (unsigned char ) sql__setrang(ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(curr_data_len - k + 1 , 1 , 8192000) + -1] , 0 , 255);
                                            if (j == ptoc_Var197)
                                                {
                                                break;
                                                }
                                            j += 1;
                                            }
                                        while (!0);
                                        }
                                    j = curr_data_len - i;
                                    (*acv).a_input_data_pos = keep_part_pos;
                                    colin.sci_len = 0;
                                    a640not_first_factor(&(*acv) , &(*dmli) , &colin , &(*first_node));
                                    curr_data_len = ptoc_Var193->integer.C_2.mb_data_len;
                                    i = curr_data_len - j;
                                    ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] = (unsigned char ) sql__setrang(ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(i + 3 , 1 , 8192000) + -1] , 0 , 255);
                                    ptoc_Var197 = i + 1;
                                    ptoc_Var195 = curr_data_len;
                                    if (ptoc_Var197 <= ptoc_Var195)
                                        {
                                        j = ptoc_Var197;
                                        do
                                            {
                                            ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(j , 1 , 8192000) + -1] = (unsigned char ) sql__setrang(ptoc_Var194->integer.C_1.mbp_buf[sql__setrang(curr_data_len - k + 1 , 1 , 8192000) + -1] , 0 , 255);
                                            if (j == ptoc_Var195)
                                                {
                                                break;
                                                }
                                            j += 1;
                                            }
                                        while (!0);
                                        }
                                    if ((int ) operator == 101)
                                        {
                                        a65_set_operator(&(*acv) , 95);
                                        }
                                    else
                                        {
                                        a65_set_operator(&(*acv) , 94);
                                        }
                                    (*dmli).d_like = 0;
                                    }
                                }
                            }
                        else
                            {
                            if ((int ) operator == 100 && ptoc_Var193->integer.C_2.mb_data_len + value_len * 2 <= ptoc_Var193->integer.C_2.mb_data_size)
                                {
                                star_offset = i - (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18769))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.epos;
                                ptoc_Var193->integer.C_2.mb_data_len = ptoc_Var193->integer.C_2.mb_data_len + value_len - k;
                                (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18771))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.elen_var = (short ) sql__setrang((short ) value_len , -32768 , 32767);
                                (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18773))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.etype = 39;
                                if ((int ) ptoc_Var194->integer.C_1.mbp_buf[sql__setrang((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18774))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.epos , 1 , 8192000) + -1] == 1)
                                    {
                                    star_offset = sql__pred(star_offset , -2147483647 , 2147483647 , "vak65.p" , 18781);
                                    g10filuni("VAK65 " , 11 , ptoc_Var193->integer.C_2.mb_data_size , ptoc_Var194->integer.C_1.mbp_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18784))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.epos + star_offset , value_len - star_offset , "\0 " , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak65.p" , 18787)).integer.C_4.sp1r_returncode);
                                    }
                                else
                                    {
                                    g10fil("VAK65 " , 12 , ptoc_Var193->integer.C_2.mb_data_size , ptoc_Var194->integer.C_1.mbp_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18792))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.epos + star_offset , value_len - star_offset , (unsigned char ) sql__setrang(ptoc_Var194->integer.C_1.mbp_buf[sql__setrang((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18796))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.epos , 1 , 8192000) + -1] , 0 , 255) , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak65.p" , 18797)).integer.C_4.sp1r_returncode);
                                    }
                                (*acv).a_input_data_pos = keep_part_pos;
                                colin.sci_len = 0;
                                a640not_first_factor(&(*acv) , &(*dmli) , &colin , &(*first_node));
                                if (ptoc_Var193->integer.C_2.mb_data_len + value_len - k <= ptoc_Var193->integer.C_2.mb_data_size)
                                    {
                                    (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18806))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.elen_var = (short ) sql__setrang((short ) value_len , -32768 , 32767);
                                    ptoc_Var193->integer.C_2.mb_data_len = ptoc_Var193->integer.C_2.mb_data_len + value_len - k;
                                    }
                                g10fil("VAK65 " , 13 , ptoc_Var193->integer.C_2.mb_data_size , ptoc_Var194->integer.C_1.mbp_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18813))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.epos + star_offset , value_len - star_offset , (unsigned char)'\377' , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak65.p" , 18817)).integer.C_4.sp1r_returncode);
                                (*(tgg00_StackList *) sql__nil(ptoc_Var193->integer.C_5.mb_st , "vak65.p" , 18818))[sql__setrang((int ) ptoc_Var196->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1].integer.C_1.etype = 39;
                                (*acv).a_input_data_pos = keep_part_pos;
                                colin.sci_len = 0;
                                a640not_first_factor(&(*acv) , &(*dmli) , &colin , &(*first_node));
                                ak65set_like_operator(&(*acv) , 100 , 0 , 0 , 0);
                                (*dmli).d_like = 0;
                                }
                            }
                        }
                    }
                }
            }
        }
    cccprint("<ak65try_like_op");
    }



int a65_convert(acv, operator, maxlength)
tak_all_command_glob *acv;
tak_convert_type operator;
int maxlength;
    {
    cccprint("AK65  a65_conver");
    switch ((int ) operator)
        {
        case 1 :
            a65_set_operator(&(*acv) , 7);
            break;
        case 2 :
            a65_set_operator(&(*acv) , 2);
            break;
        case 3 :
            a641string_set_operator(&(*acv) , 66 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\024');
            break;
        case 4 :
            a641string_set_operator(&(*acv) , 66 , 0 , 0 , (unsigned char)'\024' , (unsigned char)'\0');
            break;
        case 5 :
            a65_set_operator(&(*acv) , 2);
            a641string_set_operator(&(*acv) , 66 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\024');
            break;
        case 6 :
            a641string_set_operator(&(*acv) , 66 , 0 , 0 , (unsigned char)'\024' , (unsigned char)'\0');
            a65_set_operator(&(*acv) , 7);
            break;
        case 7 :
            a65_set_operator(&(*acv) , 17);
            break;
        case 8 :
            a65_set_operator(&(*acv) , 2);
            a65_set_operator(&(*acv) , 17);
            break;
        case 9 :
            a641string_set_operator(&(*acv) , 66 , 0 , 0 , (unsigned char)'\024' , (unsigned char)'\0');
            a65_set_operator(&(*acv) , 17);
            break;
        case 10 :
        case 11 :
        case 12 :
            a641string_set_operator(&(*acv) , 3 , -1 , 76 , (unsigned char)'\002' , (unsigned char ) sql__setrang((unsigned char ) maxlength , 0 , 255));
            if ((int ) operator == 11)
                {
                a65_set_operator(&(*acv) , 7);
                }
            else
                {
                if ((int ) operator == 12)
                    {
                    a641string_set_operator(&(*acv) , 66 , 0 , 0 , (unsigned char)'\0' , (unsigned char)'\024');
                    }
                }
            break;
        default :
            break;
        }
    cccprint("<a65_convert    ");
    }



char a65_datatypes_ok(acv, dmli, dm_type, dm_iolen, ctype, is_subquery, first_node, error_pos, convert, convert_t)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tsp_data_type *dm_type;
tsp_int2 *dm_iolen;
tsp_data_type ctype;
char is_subquery;
int first_node;
tsp_int4 error_pos;
char convert;
tak_convert_type *convert_t;
    {
    char _a65_datatypes_ok;
    char is_ok;
    char may_become_ok;
    tak00_scolinf icolin;
    tsp_int2 expr_st_cnt;
    tak_all_command_glob *ptoc_Var198;
    int ptoc_Var199;
    tak_dml_info *ptoc_Var200;
    struct tgg00_MessBlock *ptoc_Var201;
    tgg00_QualBuf *ptoc_Var202;
    tak_dml_info *ptoc_Var203;
    struct tgg00_MessBlock *ptoc_Var204;
    tgg00_QualBuf *ptoc_Var205;
    cccprint("AK65  a65_dataty");
    ptoc_Var198 = &*acv;
    is_ok = 1;
    may_become_ok = 0;
    *convert_t = 0;
    t01int4(5 , "dm_type     " , sql__ord((int ) *dm_type));
    t01int4(5 , "ctype       " , sql__ord((int ) ctype));
    if ((int ) *dm_type != 16 && (int ) ctype != 16 && (int ) *dm_type != 14 && (int ) ctype != 14)
        {
        switch ((int ) *dm_type)
            {
            case 23 :
                if ((int ) ctype != 23)
                    {
                    is_ok = 0;
                    }
                break;
            case 0 :
            case 1 :
            case 12 :
            case 15 :
                if ((int ) ctype != 0 && (int ) ctype != 1 && (int ) ctype != 12 && (int ) ctype != 15)
                    {
                    is_ok = 0;
                    }
                break;
            case 4 :
                is_ok = (char ) sql__setrang((int ) ctype == 4 , 0 , 1);
                break;
            case 10 :
            case 11 :
            case 13 :
                if ((ptoc_Var199 = (int ) ctype) == 3 || ptoc_Var199 == 2 || ptoc_Var199 == 24 || ptoc_Var199 == 10 || ptoc_Var199 == 11 || ptoc_Var199 == 13)
                    {
                    switch ((int ) ctype)
                        {
                        case 3 :
                            if ((int ) g01code.ctype == 0)
                                {
                                *convert_t = 2;
                                }
                            break;
                        case 2 :
                            if ((int ) g01code.ctype != 0)
                                {
                                *convert_t = 1;
                                }
                            break;
                        case 24 :
                            if ((char ) sql__setrang(is_subquery , 0 , 1))
                                {
                                is_ok = 0;
                                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var198->a_ap_tree , "vak65.p" , 18985))[sql__setrang(first_node , 0 , 32767)].n_proc != 54 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var198->a_ap_tree , "vak65.p" , 18986))[sql__setrang(first_node , 0 , 32767)].n_subproc != 13)
                                    {
                                    *dm_type = 24;
                                    *dm_iolen = (short ) sql__setrang((short ) sql__succ((int ) (short ) sql__pred((int ) *dm_iolen , -32768 , 32767 , "vak65.p" , 18990) * 2 , -2147483647 , 2147483647 , "vak65.p" , 18990) , -32768 , 32767);
                                    may_become_ok = 1;
                                    }
                                }
                            else
                                {
                                *convert_t = 4;
                                }
                            break;
                        default :
                            if ((int ) *dm_type != (int ) ctype)
                                {
                                is_ok = 0;
                                }
                            break;
                        }
                    }
                else
                    {
                    is_ok = 0;
                    }
                break;
            case 3 :
                if ((ptoc_Var199 = (int ) ctype) == 3 || ptoc_Var199 == 2 || ptoc_Var199 == 24 || ptoc_Var199 == 10 || ptoc_Var199 == 11 || ptoc_Var199 == 13)
                    {
                    switch ((int ) ctype)
                        {
                        case 10 :
                        case 11 :
                        case 13 :
                            if ((int ) g01code.ctype == 0)
                                {
                                *convert_t = 1;
                                }
                            break;
                        case 2 :
                            *convert_t = 1;
                            break;
                        case 24 :
                            if ((char ) sql__setrang(is_subquery , 0 , 1))
                                {
                                is_ok = 0;
                                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var198->a_ap_tree , "vak65.p" , 19027))[sql__setrang(first_node , 0 , 32767)].n_proc != 54 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var198->a_ap_tree , "vak65.p" , 19028))[sql__setrang(first_node , 0 , 32767)].n_subproc != 13)
                                    {
                                    *dm_type = 24;
                                    *dm_iolen = (short ) sql__setrang((short ) sql__succ((int ) (short ) sql__pred((int ) *dm_iolen , -32768 , 32767 , "vak65.p" , 19032) * 2 , -2147483647 , 2147483647 , "vak65.p" , 19032) , -32768 , 32767);
                                    may_become_ok = 1;
                                    }
                                }
                            else
                                {
                                *convert_t = 6;
                                }
                            break;
                        default :
                            break;
                        }
                    }
                else
                    {
                    is_ok = 0;
                    }
                break;
            case 2 :
                if ((ptoc_Var199 = (int ) ctype) == 3 || ptoc_Var199 == 2 || ptoc_Var199 == 24 || ptoc_Var199 == 10 || ptoc_Var199 == 11 || ptoc_Var199 == 13)
                    {
                    switch ((int ) ctype)
                        {
                        case 10 :
                        case 11 :
                        case 13 :
                            if ((int ) g01code.ctype == 1)
                                {
                                *convert_t = 2;
                                }
                            break;
                        case 3 :
                            *convert_t = 2;
                            break;
                        case 24 :
                            if ((char ) sql__setrang(is_subquery , 0 , 1))
                                {
                                is_ok = 0;
                                if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var198->a_ap_tree , "vak65.p" , 19066))[sql__setrang(first_node , 0 , 32767)].n_proc != 54 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var198->a_ap_tree , "vak65.p" , 19067))[sql__setrang(first_node , 0 , 32767)].n_subproc != 13)
                                    {
                                    *dm_type = 24;
                                    *dm_iolen = (short ) sql__setrang((short ) sql__succ((int ) (short ) sql__pred((int ) *dm_iolen , -32768 , 32767 , "vak65.p" , 19071) * 2 , -2147483647 , 2147483647 , "vak65.p" , 19071) , -32768 , 32767);
                                    may_become_ok = 1;
                                    }
                                }
                            else
                                {
                                *convert_t = 4;
                                }
                            break;
                        default :
                            break;
                        }
                    }
                else
                    {
                    is_ok = 0;
                    }
                break;
            case 18 :
                is_ok = (char ) sql__setrang((int ) ctype == 18 , 0 , 1);
                break;
            case 24 :
                if ((int ) ctype != 24)
                    {
                    switch ((int ) ctype)
                        {
                        case 2 :
                        case 10 :
                        case 11 :
                        case 13 :
                            *convert_t = 3;
                            break;
                        case 3 :
                            *convert_t = 5;
                            break;
                        default :
                            is_ok = 0;
                            break;
                        }
                    }
                break;
            default :
                break;
            }
        t01int4(5 , "is_ok       " , sql__ord((int ) (char ) sql__setrang(is_ok , 0 , 1)));
        t01int4(5 , "may_becomeok" , sql__ord((int ) (char ) sql__setrang(may_become_ok , 0 , 1)));
        t01int4(5 , "convert     " , sql__ord((int ) (char ) sql__setrang(convert , 0 , 1)));
        t01int4(5 , "sqlmode     " , sql__ord((int ) ptoc_Var198->a_sqlmode));
        t01int4(5 , "is_subquery " , sql__ord((int ) (char ) sql__setrang(is_subquery , 0 , 1)));
        t01int4(5 , "first_node  " , first_node);
        if (!(char ) sql__setrang(is_ok , 0 , 1) && !(char ) sql__setrang(may_become_ok , 0 , 1) && ((char ) sql__setrang(convert , 0 , 1) || (char ) sql__setrang(is_subquery , 0 , 1)) && (int ) ptoc_Var198->a_sqlmode == 2 && ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var198->a_ap_tree , "vak65.p" , 19120))[sql__setrang(first_node , 0 , 32767)].n_proc != 54 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var198->a_ap_tree , "vak65.p" , 19121))[sql__setrang(first_node , 0 , 32767)].n_subproc != 13))
            {
            is_ok = 1;
            switch ((int ) *dm_type)
                {
                case 2 :
                case 3 :
                case 24 :
                    if ((int ) ctype != 0 && (int ) ctype != 1 && (int ) ctype != 12 && (int ) ctype != 15)
                        {
                        is_ok = 0;
                        }
                    else
                        {
                        if ((char ) sql__setrang(is_subquery , 0 , 1))
                            {
                            is_ok = 0;
                            *dm_type = (unsigned char ) sql__setrang(ctype , 0 , 39);
                            *dm_iolen = 21;
                            may_become_ok = 1;
                            }
                        else
                            {
                            if (!(char ) sql__setrang(convert , 0 , 1))
                                {
                                is_ok = 0;
                                }
                            else
                                {
                                icolin.sci_typ = 1;
                                icolin.sci_len = 76;
                                icolin.sci_frac = 38;
                                a641stack_for_op_b_chr(&(*acv) , &(*dmli) , &icolin , 0 , (unsigned char ) sql__setrang(*dm_type , 0 , 39));
                                }
                            }
                        }
                    break;
                case 4 :
                    is_ok = (char ) sql__setrang((int ) ctype == 4 , 0 , 1);
                    break;
                case 0 :
                case 1 :
                case 12 :
                case 15 :
                    if ((char ) sql__setrang(is_subquery , 0 , 1))
                        {
                        switch ((int ) ctype)
                            {
                            case 2 :
                            case 10 :
                            case 11 :
                            case 13 :
                                *convert_t = 7;
                                break;
                            case 3 :
                                *convert_t = 8;
                                break;
                            case 24 :
                                *convert_t = 9;
                                break;
                            default :
                                is_ok = 0;
                                break;
                            }
                        }
                    else
                        {
                        if (!(char ) sql__setrang(convert , 0 , 1) || (int ) ctype == 4)
                            {
                            is_ok = 0;
                            }
                        else
                            {
                            if ((ptoc_Var199 = (int ) ctype) == 2 || ptoc_Var199 == 10 || ptoc_Var199 == 11 || ptoc_Var199 == 13)
                                {
                                if ((int ) g01code.ctype == 1)
                                    {
                                    a65_set_operator(&(*acv) , 7);
                                    }
                                }
                            else
                                {
                                if ((int ) ctype == 3)
                                    {
                                    if ((int ) g01code.ctype == 0)
                                        {
                                        a65_set_operator(&(*acv) , 2);
                                        }
                                    }
                                else
                                    {
                                    if ((int ) ctype == 24)
                                        {
                                        a65_convert(&(*acv) , 4 , 0);
                                        }
                                    }
                                }
                            a65_set_operator(&(*acv) , 17);
                            }
                        }
                    break;
                default :
                    is_ok = 0;
                    break;
                }
            if ((char ) sql__setrang(is_ok , 0 , 1))
                {
                ptoc_Var200 = &*dmli;
                ptoc_Var201 = &ptoc_Var198->a_mblock;
                ptoc_Var202 = (tgg00_QualBuf *) sql__nil(ptoc_Var201->integer.C_4.mb_qual , "vak65.p" , 19222);
                if ((char ) sql__setrang(ptoc_Var200->d_const_value_expr , 0 , 1) && !(char ) sql__setrang(ptoc_Var200->d_group , 0 , 1))
                    {
                    expr_st_cnt = (short ) sql__setrang((short ) ((int ) ptoc_Var202->integer.C_2.mfirst_free - (int ) ptoc_Var200->d_param_st_begin) , -32768 , 32767);
                    if ((int ) ptoc_Var200->d_param_st_index > 0 && (int ) expr_st_cnt <= 255)
                        {
                        a651code_for_const_param_expr(&(*acv) , &(*dmli) , (short ) sql__setrang(ptoc_Var200->d_param_st_begin , -32768 , 32767) , (short ) sql__setrang(expr_st_cnt , -32768 , 32767));
                        }
                    }
                }
            }
        if (!(char ) sql__setrang(is_ok , 0 , 1))
            {
            if ((char ) sql__setrang(is_subquery , 0 , 1) && (char ) sql__setrang(may_become_ok , 0 , 1) && (int ) (*dmli).d_corr == 0 && ((int ) ptoc_Var198->a_max_intern_select == 0 || (int ) ptoc_Var198->a_intern_select_cnt == 0))
                {
                t01int4(5 , "subq_problem" , 1);
                if ((int ) ptoc_Var198->a_is_ddl != 0)
                    {
                    (*(tsp1_segment *) sql__nil(ptoc_Var198->a_return_segm , "vak65.p" , 19252)).integer.C_4.sp1r_returncode = 21005;
                    }
                else
                    {
                    is_ok = 1;
                    }
                ptoc_Var198->a_subq_datatype_problem = 1;
                }
            else
                {
                a07_b_put_error(&(*acv) , 6282 , error_pos);
                }
            }
        else
            {
            if ((char ) sql__setrang(convert , 0 , 1) && (int ) *convert_t != 0)
                {
                a65_convert(&(*acv) , (unsigned char ) sql__setrang(*convert_t , 0 , 12) , 0);
                ptoc_Var203 = &*dmli;
                ptoc_Var204 = &ptoc_Var198->a_mblock;
                ptoc_Var205 = (tgg00_QualBuf *) sql__nil(ptoc_Var204->integer.C_4.mb_qual , "vak65.p" , 19266);
                if ((char ) sql__setrang(ptoc_Var203->d_const_value_expr , 0 , 1) && !(char ) sql__setrang(ptoc_Var203->d_group , 0 , 1))
                    {
                    expr_st_cnt = (short ) sql__setrang((short ) ((int ) ptoc_Var205->integer.C_2.mfirst_free - (int ) ptoc_Var203->d_param_st_begin) , -32768 , 32767);
                    if ((int ) ptoc_Var203->d_param_st_index > 0 && (int ) expr_st_cnt <= 255)
                        {
                        a651code_for_const_param_expr(&(*acv) , &(*dmli) , (short ) sql__setrang(ptoc_Var203->d_param_st_begin , -32768 , 32767) , (short ) sql__setrang(expr_st_cnt , -32768 , 32767));
                        }
                    }
                }
            }
        }
    _a65_datatypes_ok = (char ) sql__setrang(is_ok , 0 , 1);
    cccprint("<a65_datatypes_o");
    return(_a65_datatypes_ok);
    }



int a65_look_for_datatypes(acv, dmli, first_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
int first_node;
    {
    char date_arith;
    int curr_n;
    tsp_data_type h_datatyp;
    int act_param;
    char in_param_list;
    int i;
    tak_all_command_glob *ptoc_Var206;
    struct ptoc_Type56 *ptoc_Var207;
    tak_dml_info *ptoc_Var208;
    tak_param_list *ptoc_Var209;
    struct ptoc_Type9 *ptoc_Var210;
    struct ptoc_Type56 *ptoc_Var211;
    struct ptoc_Type56 *ptoc_Var212;
    tak00_columninfo *ptoc_Var213;
    struct ptoc_Type56 *ptoc_Var214;
    struct ptoc_Type56 *ptoc_Var215;
    cccprint("AK65  a65_look_f");
    ptoc_Var206 = &*acv;
    ptoc_Var207 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var206->a_ap_tree , "vak65.p" , 19309))[sql__setrang(first_node , 0 , 32767)];
    ptoc_Var208 = &*dmli;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var206->a_return_segm , "vak65.p" , 19310)).integer.C_4.sp1r_returncode == 0)
        {
        t01int4(5 , "first_node  " , first_node);
        h_datatyp = 14;
        if ((int ) ptoc_Var207->n_symb == 46)
            {
            ptoc_Var209 = &ptoc_Var206->a_parameter_spec;
            act_param = (int ) ptoc_Var207->n_length;
            in_param_list = (char ) sql__setrang(act_param <= (int ) ptoc_Var209->param_counter && ptoc_Var209->param_spec_ptr != (tsp1_param_spec_list *) (int *) 0 , 0 , 1);
            if (!(char ) sql__setrang(in_param_list , 0 , 1) || (int ) (*(tsp1_param_spec_list *) sql__nil(ptoc_Var209->param_spec_ptr , "vak65.p" , 19324))[sql__setrang(act_param , 1 , 2000) + -1].para_length == 0)
                {
                h_datatyp = 14;
                }
            else
                {
                ptoc_Var210 = &(*(tsp1_param_spec_list *) sql__nil(ptoc_Var209->param_spec_ptr , "vak65.p" , 19328))[sql__setrang(act_param , 1 , 2000) + -1];
                switch ((int ) ptoc_Var210->para_type)
                    {
                    case 0 :
                    case 29 :
                    case 30 :
                    case 1 :
                    case 12 :
                        h_datatyp = 15;
                        break;
                    case 2 :
                    case 3 :
                    case 4 :
                    case 5 :
                    case 6 :
                    case 7 :
                    case 8 :
                    case 9 :
                    case 10 :
                    case 11 :
                    case 13 :
                    case 18 :
                    case 19 :
                    case 20 :
                    case 21 :
                    case 22 :
                    case 23 :
                    case 24 :
                    case 34 :
                    case 35 :
                    case 31 :
                    case 32 :
                    case 33 :
                        h_datatyp = (unsigned char ) sql__setrang(ptoc_Var210->para_type , 0 , 39);
                        break;
                    default :
                        h_datatyp = 14;
                        break;
                    }
                }
            }
        else
            {
            if ((int ) ptoc_Var207->n_proc == 58)
                {
                switch ((int ) ptoc_Var207->n_datatype)
                    {
                    case 0 :
                    case 29 :
                    case 30 :
                    case 1 :
                    case 12 :
                        ptoc_Var208->d_datatype = 15;
                        h_datatyp = 15;
                        break;
                    default :
                        ptoc_Var208->d_datatype = (unsigned char ) sql__setrang(ptoc_Var207->n_datatype , 0 , 39);
                        h_datatyp = (unsigned char ) sql__setrang(ptoc_Var207->n_datatype , 0 , 39);
                        break;
                    }
                }
            }
        if ((int ) h_datatyp == 14)
            {
            switch ((int ) ptoc_Var207->n_symb)
                {
                case 71 :
                case 72 :
                case 164 :
                case 76 :
                case 20 :
                case 128 :
                case 166 :
                case 66 :
                case 68 :
                case 162 :
                case 163 :
                case 165 :
                    if ((char ) sql__setrang(g01unicode , 0 , 1))
                        {
                        h_datatyp = 24;
                        }
                    else
                        {
                        if ((int ) g01code.ctype == 0)
                            {
                            h_datatyp = 2;
                            }
                        else
                            {
                            h_datatyp = 3;
                            }
                        }
                    break;
                case 19 :
                    if ((int ) ptoc_Var206->a_sqlmode == 3)
                        {
                        if ((int ) g01code.ctype == 0)
                            {
                            h_datatyp = 2;
                            }
                        else
                            {
                            h_datatyp = 3;
                            }
                        }
                    else
                        {
                        h_datatyp = 15;
                        }
                    break;
                default :
                    break;
                }
            }
        switch ((int ) ptoc_Var207->n_symb)
            {
            case 40 :
            case 41 :
            case 49 :
            case 195 :
            case 201 :
            case 237 :
                ptoc_Var208->d_datatype = 15;
                break;
            case 97 :
            case 98 :
            case 114 :
            case 24 :
                ptoc_Var208->d_datatype = 10;
                break;
            case 103 :
            case 104 :
            case 105 :
            case 108 :
            case 25 :
                ptoc_Var208->d_datatype = 11;
                break;
            case 238 :
            case 134 :
            case 191 :
            case 192 :
            case 106 :
            case 204 :
                ptoc_Var208->d_datatype = 13;
                break;
            case 198 :
                ptoc_Var208->d_datatype = 0;
                break;
            case 26 :
            case 111 :
            case 145 :
                ptoc_Var208->d_datatype = 2;
                break;
            case 100 :
            case 219 :
            case 101 :
            case 102 :
            case 99 :
            case 193 :
            case 75 :
            case 161 :
            case 88 :
            case 39 :
            case 62 :
            case 156 :
            case 65 :
            case 64 :
            case 69 :
            case 70 :
            case 34 :
            case 33 :
            case 167 :
            case 168 :
            case 169 :
            case 170 :
            case 172 :
            case 171 :
            case 173 :
            case 174 :
            case 175 :
            case 212 :
            case 207 :
            case 208 :
            case 209 :
            case 210 :
            case 211 :
            case 176 :
            case 179 :
            case 177 :
            case 180 :
            case 178 :
            case 181 :
            case 215 :
            case 214 :
            case 213 :
                ptoc_Var208->d_datatype = 15;
                break;
            case 135 :
            case 132 :
            case 125 :
                if ((int ) ptoc_Var207->n_proc == 88)
                    {
                    ptoc_Var208->d_datatype = 15;
                    }
                else
                    {
                    ptoc_Var208->d_datatype = 10;
                    }
                break;
            case 129 :
            case 131 :
            case 133 :
                if ((int ) ptoc_Var207->n_proc == 88)
                    {
                    ptoc_Var208->d_datatype = 15;
                    }
                else
                    {
                    ptoc_Var208->d_datatype = 11;
                    }
                break;
            case 130 :
                if ((int ) ptoc_Var207->n_proc == 88)
                    {
                    ptoc_Var208->d_datatype = 15;
                    }
                else
                    {
                    ptoc_Var208->d_datatype = 13;
                    }
                break;
            case 126 :
                ptoc_Var208->d_datatype = 15;
                break;
            case 58 :
            case 61 :
            case 185 :
            case 187 :
            case 92 :
            case 93 :
            case 186 :
            case 188 :
                ptoc_Var208->d_datatype = 15;
                break;
            case 127 :
            case 189 :
            case 197 :
            case 48 :
            case 190 :
            case 216 :
            case 217 :
            case 220 :
            case 230 :
            case 73 :
            case 74 :
            case 89 :
            case 90 :
            case 67 :
            case 239 :
            case 240 :
                if ((char ) sql__setrang(g01unicode , 0 , 1) && (int ) g01code.char_default == 20)
                    {
                    ptoc_Var208->d_datatype = 24;
                    }
                else
                    {
                    if ((int ) g01code.ctype == 0)
                        {
                        ptoc_Var208->d_datatype = 2;
                        }
                    else
                        {
                        ptoc_Var208->d_datatype = 3;
                        }
                    }
                break;
            case 232 :
            case 233 :
                ptoc_Var208->d_datatype = 4;
                break;
            case 200 :
                ak65_dt_look_for_user_def(&(*acv) , &(*dmli) , first_node);
                break;
            case 142 :
                ptoc_Var208->d_datatype = 18;
                break;
            case 43 :
            case 44 :
            case 45 :
                ptoc_Var208->d_is_parameter_only = 0;
                _ptoc_ASSIGN(unsigned char , 64, ptoc_Var208->d_user, a01_il_b_identifier)
                curr_n = first_node;
                ptoc_Var208->d_n_pos = (*(tak_ap_max_tree *) sql__nil(ptoc_Var206->a_ap_tree , "vak65.p" , 19494))[sql__setrang(curr_n , 0 , 32767)].n_pos;
                a06get_username(&(*acv) , &curr_n , ptoc_Var208->d_user);
                ptoc_Var211 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var206->a_ap_tree , "vak65.p" , 19496))[sql__setrang(curr_n , 0 , 32767)];
                if ((int ) ptoc_Var211->n_symb == 44)
                    {
                    a05identifier_get(&(*acv) , curr_n , sizeof(unsigned char [64]) , ptoc_Var208->d_table);
                    if ((int ) ptoc_Var206->a_is_ddl == 49)
                        {
                        if ((_ptoc_MEMCMP(unsigned char , 64, ptoc_Var208->d_table , a01_i_old) == 0 ? 0 : 1) && (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var208->d_table , a01_i_new) == 0 ? 0 : 1))
                            {
                            a07_b_put_error(&(*acv) , 6492 , ptoc_Var211->n_pos);
                            }
                        _ptoc_ASSIGN(unsigned char , 64, ptoc_Var208->d_table, a01_il_b_identifier)
                        }
                    curr_n = (int ) ptoc_Var211->n_sa_level;
                    }
                else
                    {
                    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var208->d_table, a01_il_b_identifier)
                    }
                ptoc_Var212 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var206->a_ap_tree , "vak65.p" , 19521))[sql__setrang(curr_n , 0 , 32767)];
                if ((int ) ptoc_Var212->n_symb == 45)
                    {
                    a05identifier_get(&(*acv) , curr_n , sizeof(unsigned char [64]) , ptoc_Var208->d_column);
                    ptoc_Var208->d_vppos = ptoc_Var212->n_pos;
                    a61_search_table(&(*acv) , &(*dmli));
                    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var206->a_return_segm , "vak65.p" , 19529)).integer.C_4.sp1r_returncode == 0)
                        {
                        ptoc_Var213 = (tak00_columninfo *) sql__nil(ptoc_Var208->d_colbuf , "vak65.p" , 19531);
                        if ((int ) ptoc_Var213->integer.C_1.cdatatyp == 0 || (int ) ptoc_Var213->integer.C_1.cdatatyp == 1 || (int ) ptoc_Var213->integer.C_1.cdatatyp == 12)
                            {
                            ptoc_Var208->d_datatype = 15;
                            }
                        else
                            {
                            ptoc_Var208->d_datatype = (unsigned char ) sql__setrang(ptoc_Var213->integer.C_1.cdatatyp , 0 , 39);
                            }
                        }
                    }
                else
                    {
                    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var208->d_column, a01_il_b_identifier)
                    }
                break;
            default :
                date_arith = 0;
                if ((int ) ptoc_Var207->n_proc == 55 && (int ) ptoc_Var207->n_subproc == 4)
                    {
                    date_arith = (char ) sql__setrang(ak65is_datetime_arith(&(*acv) , &(*dmli) , (short ) sql__setrang((short ) first_node , -32768 , 32767) , 1) , 0 , 1);
                    }
                if (!(char ) sql__setrang(date_arith , 0 , 1))
                    {
                    if ((int ) ptoc_Var207->n_symb != 77 && (int ) ptoc_Var207->n_symb != 78 && (int ) ptoc_Var207->n_proc != 82)
                        {
                        if ((int ) ptoc_Var207->n_lo_level != 0)
                            {
                            a65_look_for_datatypes(&(*acv) , &(*dmli) , (int ) ptoc_Var207->n_lo_level);
                            }
                        if ((int ) ptoc_Var208->d_datatype == 14 && (int ) ptoc_Var207->n_sa_level != 0)
                            {
                            a65_look_for_datatypes(&(*acv) , &(*dmli) , (int ) ptoc_Var207->n_sa_level);
                            }
                        t01int4(5 , "a_insert_sel" , sql__ord((int ) (char ) sql__setrang(ptoc_Var206->a_insert_select , 0 , 1)));
                        t01int4(5 , "d_subcount  " , (int ) (*dmli).d_subcount);
                        t01int4(5 , "d_outcolno  " , (int ) (*dmli).d_outcolno);
                        i = 1;
                        do
                            {
                            t01int4(5 , "ch_datatype " , sql__ord((int ) (*dmli).d_change.cr_columns[sql__setrang(i , 1 , 1024) + -1].ch_length));
                            if (i == 10)
                                {
                                break;
                                }
                            i += 1;
                            }
                        while (!0);
                        if ((int ) ptoc_Var208->d_datatype == 14 && !(char ) sql__setrang(ptoc_Var208->d_wherepart , 0 , 1) && !(char ) sql__setrang(ptoc_Var208->d_having , 0 , 1) && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var206->a_ap_tree , "vak65.p" , 19581))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var206->a_ap_tree , "vak65.p" , 19581))[0].n_lo_level , 0 , 32767)].n_proc == 47 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var206->a_ap_tree , "vak65.p" , 19582))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var206->a_ap_tree , "vak65.p" , 19582))[0].n_lo_level , 0 , 32767)].n_subproc == 10 && (char ) sql__setrang(ptoc_Var206->a_insert_select , 0 , 1) && (int ) (*dmli).d_subcount == 0)
                            {
                            if ((int ) ptoc_Var208->d_outcolno > (int ) ptoc_Var208->d_change.cr_colcount)
                                {
                                a07_b_put_error(&(*acv) , 6484 , 1);
                                }
                            else
                                {
                                a56_datatype((unsigned char ) sql__setrang(ptoc_Var208->d_change.cr_columns[sql__setrang((int ) ptoc_Var208->d_outcolno , 1 , 1024) + -1].ch_length , 0 , 255) , &ptoc_Var208->d_datatype);
                                }
                            }
                        if ((int ) ptoc_Var208->d_datatype == 14)
                            {
                            ptoc_Var208->d_datatype = (unsigned char ) sql__setrang(h_datatyp , 0 , 39);
                            }
                        }
                    else
                        {
                        if ((int ) ptoc_Var208->d_corr == 0)
                            {
                            curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var206->a_ap_tree , "vak65.p" , 19604))[sql__setrang(first_node , 0 , 32767)].n_sa_level;
                            ptoc_Var214 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var206->a_ap_tree , "vak65.p" , 19605))[sql__setrang(curr_n , 0 , 32767)];
                            if ((int ) ptoc_Var214->n_proc == 82)
                                {
                                curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var206->a_ap_tree , "vak65.p" , 19608))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
                                }
                            ptoc_Var215 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var206->a_ap_tree , "vak65.p" , 19611))[sql__setrang(curr_n , 0 , 32767)];
                            ptoc_Var208->d_datatype = (unsigned char ) sql__setrang(ptoc_Var215->n_datatype , 0 , 39);
                            }
                        }
                    }
                break;
            }
        }
    t01int4(5 , "look_datatyp" , sql__ord((int ) (*dmli).d_datatype));
    cccprint("<a65_look_for_da");
    }



int ak65_dt_look_for_user_def(acv, dmli, act_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
int act_node;
    {
    int curr_n;
    int value_cnt;
    tsp_int2 dummy_iolen;
    tak00_scolinf proc_colin;
    tak00_scolinf param_colin;
    tsp_data_type keep_datatype;
    tsp_data_type res_datatype;
    tsp_data_type param_datatype;
    tak_convert_type convert_t;
    tsp_knl_identifier func_name;
    tak_sysbufferaddress proc_buf;
    tak_all_command_glob *ptoc_Var216;
    tak_dml_info *ptoc_Var217;
    int ptoc_Var218;
    cccprint("AK65  ak65_dt_lo");
    ptoc_Var216 = &*acv;
    ptoc_Var217 = &*dmli;
    a05identifier_get(&(*acv) , act_node , sizeof(unsigned char [64]) , func_name);
    if (!a12dbfunc_exist(&(*acv) , func_name , &proc_buf))
        {
        a07_b_put_error(&(*acv) , 6499 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var216->a_ap_tree , "vak65.p" , 19656))[sql__setrang(act_node , 0 , 32767)].n_pos);
        }
    else
        {
        keep_datatype = (unsigned char ) sql__setrang(ptoc_Var217->d_datatype , 0 , 39);
        value_cnt = 0;
        curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var216->a_ap_tree , "vak65.p" , 19661))[sql__setrang(act_node , 0 , 32767)].n_lo_level;
        a12describe_param(&(*acv) , proc_buf , (int ) (*(struct tak_systembuffer *) sql__nil(proc_buf , "vak65.p" , 19663)).integer.C_66.smethod.me_param_cnt , &proc_colin);
        if ((ptoc_Var218 = (int ) proc_colin.sci_typ) == 0 || ptoc_Var218 == 1)
            {
            res_datatype = 15;
            }
        else
            {
            res_datatype = (unsigned char ) sql__setrang(proc_colin.sci_typ , 0 , 39);
            }
        dummy_iolen = 0;
        if (a65_datatypes_ok(&(*acv) , &(*dmli) , &keep_datatype , &dummy_iolen , (unsigned char ) sql__setrang(res_datatype , 0 , 39) , 0 , 1 , 1 , 0 , &convert_t))
            {
            while (curr_n != 0 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var216->a_return_segm , "vak65.p" , 19676)).integer.C_4.sp1r_returncode == 0)
                {
                value_cnt = sql__succ(value_cnt , -2147483647 , 2147483647 , "vak65.p" , 19678);
                param_colin.sci_len = 0;
                a12describe_param(&(*acv) , proc_buf , value_cnt , &param_colin);
                if ((ptoc_Var218 = (int ) param_colin.sci_typ) == 0 || ptoc_Var218 == 1)
                    {
                    param_datatype = 15;
                    }
                else
                    {
                    param_datatype = (unsigned char ) sql__setrang(param_colin.sci_typ , 0 , 39);
                    }
                a65_look_for_datatypes(&(*acv) , &(*dmli) , curr_n);
                if (a65_datatypes_ok(&(*acv) , &(*dmli) , &ptoc_Var217->d_datatype , &dummy_iolen , (unsigned char ) sql__setrang(param_datatype , 0 , 39) , 0 , 1 , 1 , 0 , &convert_t))
                    {
                    curr_n = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var216->a_ap_tree , "vak65.p" , 19693))[sql__setrang(curr_n , 0 , 32767)].n_sa_level;
                    }
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var216->a_return_segm , "vak65.p" , 19698)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var217->d_datatype = (unsigned char ) sql__setrang(res_datatype , 0 , 39);
            }
        else
            {
            if ((int ) keep_datatype != 14)
                {
                (*(tsp1_segment *) sql__nil(ptoc_Var216->a_return_segm , "vak65.p" , 19705)).integer.C_4.sp1r_returncode = 0;
                (*(tsp1_segment *) sql__nil(ptoc_Var216->a_return_segm , "vak65.p" , 19706)).integer.C_4.sp1r_errorpos = 0;
                }
            else
                {
                a07_b_put_error(&(*acv) , 6282 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var216->a_ap_tree , "vak65.p" , 19710))[sql__setrang(curr_n , 0 , 32767)].n_pos);
                }
            }
        }
    cccprint("<ak65_dt_look_fo");
    }



int a65_search_condition(acv, dmli, first_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
int *first_node;
    {
    char m_rowno_allowed;
    int truth_node;
    list_elem_info list_info;
    tak_all_command_glob *ptoc_Var219;
    tak_dml_info *ptoc_Var220;
    int ptoc_Var221;
    tak_joinrec *ptoc_Var222;
    struct ptoc_Type110 *ptoc_Var223;
    tgg00_QualBuf *ptoc_Var224;
    struct tgg00_MessBlock *ptoc_Var225;
    tgg00_QualBuf *ptoc_Var226;
    struct ptoc_Type38 *ptoc_Var227;
    struct tgg00_MessBlock *ptoc_Var228;
    cccprint("AK65  a65_search");
    ptoc_Var219 = &*acv;
    ptoc_Var220 = &*dmli;
    t01aptree(5 , sql__nil(ptoc_Var219->a_ap_tree , "vak65.p" , 19740) , (int ) ptoc_Var219->a_scv_index , (short ) sql__setrang(ptoc_Var219->a_first_hint_node , -32768 , 32767));
    ptoc_Var220->d_pars_kind = 20;
    ptoc_Var220->d_joins.jrc_cnt = 0;
    ptoc_Var220->d_joins.jrc_col_upd = 0;
    ptoc_Var220->d_joins.jrc_joinarr[sql__setrang((int ) ptoc_Var220->d_joins.jrc_cnt , 0 , 256)].jo_col_upd_stat = (unsigned char)'\0';
    if ((ptoc_Var221 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var219->a_ap_tree , "vak65.p" , 19746))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var219->a_ap_tree , "vak65.p" , 19746))[0].n_lo_level , 0 , 32767)].n_proc) == 7 || ptoc_Var221 == 47 || ptoc_Var221 == 48 || ptoc_Var221 == 49 || ptoc_Var221 == 54 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var219->a_ap_tree , "vak65.p" , 19747))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var219->a_ap_tree , "vak65.p" , 19747))[0].n_lo_level , 0 , 32767)].n_proc == 34 && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var219->a_ap_tree , "vak65.p" , 19748))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var219->a_ap_tree , "vak65.p" , 19748))[0].n_lo_level , 0 , 32767)].n_subproc == 4)
        {
        ptoc_Var220->d_wherepart = (char ) sql__setrang(!(char ) sql__setrang(ptoc_Var220->d_having , 0 , 1) , 0 , 1);
        }
    else
        {
        ptoc_Var220->d_wherepart = 1;
        }
    if ((int ) ptoc_Var220->d_maxcounttabs > 1)
        {
        ptoc_Var220->d_join = 1;
        ptoc_Var222 = &ptoc_Var220->d_joins;
        ptoc_Var223 = &ptoc_Var222->jrc_joinarr[sql__setrang((int ) ptoc_Var222->jrc_cnt , 0 , 256)];
        s10fil(sizeof(tak_joinarr ) , ptoc_Var222->jrc_joinarr , 1 , sizeof(tak_joinarr ) , (unsigned char)'\0');
        ptoc_Var223->jo_partno = (unsigned char)'\001';
        }
    else
        {
        ptoc_Var220->d_join = 0;
        }
    ptoc_Var220->d_first_tab = 0;
    ptoc_Var220->d_arith_where = 0;
    ptoc_Var220->d_allowed = 1;
    m_rowno_allowed = (char ) sql__setrang(ptoc_Var219->a_rowno_allowed , 0 , 1);
    ptoc_Var219->a_rowno_allowed = 1;
    ptoc_Var224 = (tgg00_QualBuf *) sql__nil(ptoc_Var219->a_mblock.integer.C_4.mb_qual , "vak65.p" , 19773);
    if ((int ) ptoc_Var224->integer.C_2.mqual_pos == 0)
        {
        ptoc_Var224->integer.C_2.mqual_pos = (short ) sql__setrang(ptoc_Var224->integer.C_2.mfirst_free , -32768 , 32767);
        }
    if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var219->a_ap_tree , "vak65.p" , 19779))[sql__setrang(*first_node , 0 , 32767)].n_subproc == 16)
        {
        truth_node = *first_node;
        *first_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var219->a_ap_tree , "vak65.p" , 19783))[sql__setrang(truth_node , 0 , 32767)].n_lo_level;
        }
    else
        {
        truth_node = 0;
        }
    ak65se_condition(&(*acv) , &(*dmli) , &(*first_node) , 1 , &list_info);
    if (truth_node > 0)
        {
        ptoc_Var225 = &ptoc_Var219->a_mblock;
        ptoc_Var226 = (tgg00_QualBuf *) sql__nil(ptoc_Var225->integer.C_4.mb_qual , "vak65.p" , 19791);
        if ((int ) ptoc_Var226->integer.C_2.mfirst_free >= (int ) ptoc_Var225->integer.C_2.mb_st_max)
            {
            a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var225->integer.C_2.mb_st_max);
            }
        else
            {
            ptoc_Var227 = &(*(tgg00_StackList *) sql__nil(ptoc_Var225->integer.C_5.mb_st , "vak65.p" , 19798))[sql__setrang((int ) ptoc_Var226->integer.C_2.mfirst_free , 1 , 32767) + -1];
            ptoc_Var227->integer.C_1.etype = 41;
            ptoc_Var227->integer.C_1.eop = 0;
            switch ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var219->a_ap_tree , "vak65.p" , 19802))[sql__setrang(truth_node , 0 , 32767)].n_symb)
                {
                case 157 :
                    ptoc_Var227->integer.C_1.epos = 1;
                    break;
                case 158 :
                    ptoc_Var227->integer.C_1.epos = 4;
                    break;
                case 159 :
                    ptoc_Var227->integer.C_1.epos = 0;
                    break;
                case 160 :
                    ptoc_Var227->integer.C_1.epos = 3;
                    break;
                case 56 :
                    ptoc_Var227->integer.C_1.epos = 2;
                    break;
                case 55 :
                    ptoc_Var227->integer.C_1.epos = 5;
                    break;
                default :
                    (unsigned char ) sql__caseerr("vak65.p" , 19802);
                    break;
                }
            ptoc_Var227->integer.C_1.elen_var = 0;
            ptoc_Var227->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
            ptoc_Var227->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
            ptoc_Var226->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var226->integer.C_2.mfirst_free , -32768 , 32767 , "vak65.p" , 19822) , -32768 , 32767);
            ptoc_Var226->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var226->integer.C_2.mqual_cnt , -32768 , 32767 , "vak65.p" , 19823) , -32768 , 32767);
            }
        }
    ptoc_Var219->a_rowno_allowed = (char ) sql__setrang(m_rowno_allowed , 0 , 1);
    if ((int ) ptoc_Var220->d_maxcounttabs > 1 && ptoc_Var219->a_mblock.integer.C_2.mb_data_len < ptoc_Var219->a_mblock.integer.C_2.mb_data_size)
        {
        ptoc_Var228 = &ptoc_Var219->a_mblock;
        ptoc_Var228->integer.C_2.mb_data_len = sql__succ(ptoc_Var228->integer.C_2.mb_data_len , -2147483647 , 2147483647 , "vak65.p" , 19837);
        (*(tgg00_DataPart *) sql__nil(ptoc_Var228->integer.C_3.mb_data , "vak65.p" , 19838)).integer.C_1.mbp_buf[sql__setrang(ptoc_Var228->integer.C_2.mb_data_len , 1 , 8192000) + -1] = (unsigned char)'\377';
        }
    t01messblock(5 , "end search_c" , &ptoc_Var219->a_mblock);
    cccprint("<a65_search_cond");
    }



int a65_set_operator(acv, operator)
tak_all_command_glob *acv;
tgg_stack_op_type operator;
    {
    tak_all_command_glob *ptoc_Var229;
    struct tgg00_MessBlock *ptoc_Var230;
    tgg00_QualBuf *ptoc_Var231;
    struct ptoc_Type38 *ptoc_Var232;
    struct ptoc_Type38 *ptoc_Var233;
    cccprint("AK65  a65_set_op");
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak65.p" , 19857)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var229 = &*acv;
        ptoc_Var230 = &ptoc_Var229->a_mblock;
        ptoc_Var231 = (tgg00_QualBuf *) sql__nil(ptoc_Var230->integer.C_4.mb_qual , "vak65.p" , 19859);
        ptoc_Var232 = &(*(tgg00_StackList *) sql__nil(ptoc_Var230->integer.C_5.mb_st , "vak65.p" , 19861))[sql__setrang((int ) ptoc_Var231->integer.C_2.mfirst_free + -1 , 1 , 32767) + -1];
        if ((int ) ptoc_Var232->integer.C_1.eop == 0 && (int ) operator != 98 && (int ) operator != 99 && (int ) ptoc_Var232->integer.C_1.etype != 41 && (int ) ptoc_Var232->integer.C_1.etype != 39 && (!(char ) sql__setrang((*acv).a_special_expr , 0 , 1) || (int ) ptoc_Var232->integer.C_1.etype == 22))
            {
            ptoc_Var232->integer.C_1.eop = (unsigned char ) sql__setrang(operator , 0 , 123);
            }
        else
            {
            if ((int ) ptoc_Var231->integer.C_2.mfirst_free > (int ) ptoc_Var230->integer.C_2.mb_st_max)
                {
                a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var230->integer.C_2.mb_st_max);
                }
            else
                {
                if ((int ) ptoc_Var231->integer.C_2.mupd_cnt == 0)
                    {
                    ptoc_Var231->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var231->integer.C_2.mqual_cnt , -32768 , 32767 , "vak65.p" , 19879) , -32768 , 32767);
                    }
                else
                    {
                    ptoc_Var231->integer.C_2.mupd_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var231->integer.C_2.mupd_cnt , -32768 , 32767 , "vak65.p" , 19881) , -32768 , 32767);
                    }
                ptoc_Var233 = &(*(tgg00_StackList *) sql__nil(ptoc_Var230->integer.C_5.mb_st , "vak65.p" , 19883))[sql__setrang((int ) ptoc_Var231->integer.C_2.mfirst_free , 1 , 32767) + -1];
                ptoc_Var233->integer.C_1.etype = 26;
                ptoc_Var233->integer.C_1.eop = (unsigned char ) sql__setrang(operator , 0 , 123);
                ptoc_Var233->integer.C_1.epos = 0;
                ptoc_Var233->integer.C_1.elen_var = 0;
                ptoc_Var233->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                ptoc_Var233->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                ptoc_Var231->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var231->integer.C_2.mfirst_free , -32768 , 32767 , "vak65.p" , 19893) , -32768 , 32767);
                }
            }
        }
    cccprint("<a65_set_operato");
    }



int ak65set_mass_cmp(acv, operator, all, expr_cnt, list_cnt)
tak_all_command_glob *acv;
tgg_stack_op_type operator;
char all;
int expr_cnt;
int list_cnt;
    {
    struct tgg00_MessBlock *ptoc_Var234;
    tgg00_QualBuf *ptoc_Var235;
    struct ptoc_Type38 *ptoc_Var236;
    cccprint("AK65  ak65set_ma");
    ptoc_Var234 = &(*acv).a_mblock;
    ptoc_Var235 = (tgg00_QualBuf *) sql__nil(ptoc_Var234->integer.C_4.mb_qual , "vak65.p" , 19913);
    if ((int ) ptoc_Var235->integer.C_2.mfirst_free > (int ) ptoc_Var234->integer.C_2.mb_st_max)
        {
        a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var234->integer.C_2.mb_st_max);
        }
    else
        {
        if (list_cnt < 1)
            {
            list_cnt = 1;
            }
        ptoc_Var235->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var235->integer.C_2.mqual_cnt , -32768 , 32767 , "vak65.p" , 19923) , -32768 , 32767);
        ptoc_Var236 = &(*(tgg00_StackList *) sql__nil(ptoc_Var234->integer.C_5.mb_st , "vak65.p" , 19924))[sql__setrang((int ) ptoc_Var235->integer.C_2.mfirst_free , 1 , 32767) + -1];
        ptoc_Var236->integer.C_1.etype = 57;
        ptoc_Var236->integer.C_1.eop = (unsigned char ) sql__setrang(operator , 0 , 123);
        ptoc_Var236->integer.C_1.epos = (short ) sql__setrang((short ) list_cnt , -32768 , 32767);
        ptoc_Var236->integer.C_1.elen_var = (short ) sql__setrang((short ) (expr_cnt / list_cnt) , -32768 , 32767);
        if ((char ) sql__setrang(all , 0 , 1))
            {
            ptoc_Var236->integer.C_1.ecol_tab[0] = (unsigned char)'\001';
            }
        else
            {
            ptoc_Var236->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
            }
        ptoc_Var236->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
        ptoc_Var235->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var235->integer.C_2.mfirst_free , -32768 , 32767 , "vak65.p" , 19939) , -32768 , 32767);
        }
    cccprint("<ak65set_mass_cm");
    }



int a65not_first_term(acv, dmli, colin, first_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int *first_node;
    {
    int st_begin;
    int st_index;
    cccprint("AK65  a65not_fir");
    st_begin = (int ) (*dmli).d_param_st_begin;
    st_index = (int ) (*dmli).d_param_st_index;
    (*dmli).d_param_st_begin = 0;
    (*dmli).d_param_st_index = 0;
    a65_term(&(*acv) , &(*dmli) , &(*colin) , &(*first_node));
    (*dmli).d_param_st_begin = (short ) sql__setrang((short ) st_begin , -32768 , 32767);
    if ((int ) (*dmli).d_param_st_index == 0)
        {
        (*dmli).d_param_st_index = (short ) sql__setrang((short ) st_index , -32768 , 32767);
        }
    cccprint("<a65not_first_te");
    }



int a65_term(acv, dmli, colin, first_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int *first_node;
    {
    int act_node;
    tak00_scolinf colin1;
    char const_value_expr;
    int st_begin;
    int st_end;
    int expr_st_cnt;
    tsp_int4 pos;
    tak_all_command_glob *ptoc_Var237;
    struct ptoc_Type56 *ptoc_Var238;
    tak_dml_info *ptoc_Var239;
    int ptoc_Var240;
    struct tgg00_MessBlock *ptoc_Var241;
    tgg00_QualBuf *ptoc_Var242;
    cccprint("AK65  a65_term  ");
    ptoc_Var237 = &*acv;
    ptoc_Var238 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var237->a_ap_tree , "vak65.p" , 19990))[sql__setrang(*first_node , 0 , 32767)];
    ptoc_Var239 = &*dmli;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var237->a_return_segm , "vak65.p" , 19991)).integer.C_4.sp1r_returncode == 0)
        {
        act_node = (int ) ptoc_Var238->n_lo_level;
        if ((int ) ptoc_Var238->n_proc == 55 && (int ) ptoc_Var238->n_subproc == 4)
            {
            a65_val_expr(&(*acv) , &(*dmli) , &(*colin) , *first_node);
            }
        else
            {
            if ((int ) ptoc_Var238->n_proc != 55 || (int ) ptoc_Var238->n_subproc != 5)
                {
                a640factor(&(*acv) , &(*dmli) , &(*colin) , &(*first_node));
                }
            else
                {
                const_value_expr = (char ) sql__setrang(ptoc_Var239->d_const_value_expr , 0 , 1);
                ptoc_Var239->d_const_value_expr = 1;
                st_begin = (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var237->a_mblock.integer.C_4.mb_qual , "vak65.p" , 20006)).integer.C_2.mfirst_free;
                (*dmli).d_colptr = (tak00_columninfo *) (int *) 0;
                a640factor(&(*acv) , &(*dmli) , &(*colin) , &act_node);
                act_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var237->a_ap_tree , "vak65.p" , 20009))[sql__setrang(act_node , 0 , 32767)].n_sa_level;
                if ((ptoc_Var240 = (int ) (*dmli).d_datatype) != 15 && ptoc_Var240 != 0 && ptoc_Var240 != 1 && ptoc_Var240 != 12 && ptoc_Var240 != 30 && ptoc_Var240 != 29)
                    {
                    a07_b_put_error(&(*acv) , 6276 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var237->a_ap_tree , "vak65.p" , 20014))[sql__setrang(act_node , 0 , 32767)].n_pos);
                    }
                else
                    {
                    colin1.sci_len = 0;
                    a640not_first_factor(&(*acv) , &(*dmli) , &colin1 , &act_node);
                    }
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var237->a_return_segm , "vak65.p" , 20021)).integer.C_4.sp1r_returncode == 0)
                    {
                    switch ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var237->a_ap_tree , "vak65.p" , 20024))[sql__setrang(*first_node , 0 , 32767)].n_symb)
                        {
                        case 118 :
                            ak65new_colinfo(&(*acv) , &(*colin) , &colin1 , 118);
                            a65_set_operator(&(*acv) , 89);
                            break;
                        case 2 :
                            ak65new_colinfo(&(*acv) , &(*colin) , &colin1 , 2);
                            a65_set_operator(&(*acv) , 83);
                            break;
                        case 64 :
                            a65_set_operator(&(*acv) , 84);
                            break;
                        case 65 :
                            a65_set_operator(&(*acv) , 87);
                            *colin = colin1;
                            break;
                        default :
                            (unsigned char ) sql__caseerr("vak65.p" , 20024);
                            break;
                        }
                    if ((char ) sql__setrang(ptoc_Var239->d_const_value_expr , 0 , 1) && !(char ) sql__setrang(ptoc_Var239->d_group , 0 , 1) && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var237->a_return_segm , "vak65.p" , 20045)).integer.C_4.sp1r_returncode == 0)
                        {
                        expr_st_cnt = (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var237->a_mblock.integer.C_4.mb_qual , "vak65.p" , 20048)).integer.C_2.mfirst_free - st_begin;
                        ptoc_Var241 = &ptoc_Var237->a_mblock;
                        ptoc_Var242 = (tgg00_QualBuf *) sql__nil(ptoc_Var241->integer.C_4.mb_qual , "vak65.p" , 20050);
                        if ((int ) ptoc_Var239->d_param_st_index > 0 && expr_st_cnt <= 255 && ptoc_Var241->integer.C_2.mb_data_len - (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var241->integer.C_5.mb_st , "vak65.p" , 20054))[sql__setrang(st_begin , 1 , 32767) + -1].integer.C_1.epos + 1 >= (int ) (*colin).sci_iolen)
                            {
                            a651code_for_const_param_expr(&(*acv) , &(*dmli) , (short ) sql__setrang((short ) st_begin , -32768 , 32767) , (short ) sql__setrang((short ) expr_st_cnt , -32768 , 32767));
                            }
                        else
                            {
                            if ((int ) ptoc_Var239->d_param_st_index == 0)
                                {
                                if ((ptoc_Var240 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var237->a_ap_tree , "vak65.p" , 20062))[sql__setrang(act_node , 0 , 32767)].n_symb) == 135 || ptoc_Var240 == 132 || ptoc_Var240 == 125 || ptoc_Var240 == 129 || ptoc_Var240 == 131 || ptoc_Var240 == 133 || ptoc_Var240 == 130)
                                    {
                                    pos = 1;
                                    }
                                else
                                    {
                                    pos = (*(tak_ap_max_tree *) sql__nil(ptoc_Var237->a_ap_tree , "vak65.p" , 20068))[sql__setrang(act_node , 0 , 32767)].n_pos;
                                    }
                                st_end = (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var237->a_mblock.integer.C_4.mb_qual , "vak65.p" , 20071)).integer.C_2.mfirst_free + -1;
                                a651value_calculate(&(*acv) , st_begin , st_end , (char ) sql__setrang((int ) (*colin).sci_typ == 4 , 0 , 1) , (char ) sql__setrang(ptoc_Var239->d_like , 0 , 1) , (short ) sql__setrang((*colin).sci_iolen , -32768 , 32767) , pos);
                                }
                            }
                        ptoc_Var239->d_const_value_expr = (char ) sql__setrang(const_value_expr , 0 , 1);
                        }
                    }
                }
            }
        }
    t01int4(5 , "factcoli len" , (int ) (*colin).sci_len);
    t01int4(5 , "factcoli fra" , (int ) (*colin).sci_frac);
    t01int4(5 , "factcoli iol" , (int ) (*colin).sci_iolen);
    cccprint("<a65_term       ");
    }



char ak65is_datetime_arith(acv, dmli, op_node, look_for_dtype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tsp_int2 op_node;
char look_for_dtype;
    {
    char _ak65is_datetime_arith;
    tsp_int2 left_n;
    tsp_int2 last_left_n;
    tsp_int2 right_n;
    tsp_int2 duration_n;
    tsp_int2 new_node;
    char date_arith;
    tsp_data_type op_dtype;
    tsp_data_type save_datatype;
    char save_change_date_time;
    tak_all_command_glob *ptoc_Var243;
    tak_dml_info *ptoc_Var244;
    int ptoc_Var245;
    struct ptoc_Type56 *ptoc_Var246;
    cccprint("AK65  ak65is_dat");
    date_arith = 0;
    ptoc_Var243 = &*acv;
    ptoc_Var244 = &*dmli;
    if ((ptoc_Var245 = (int ) ptoc_Var243->a_sqlmode) == 2 || ptoc_Var245 == 1)
        {
        save_datatype = (unsigned char ) sql__setrang(ptoc_Var244->d_datatype , 0 , 39);
        save_change_date_time = (char ) sql__setrang(ptoc_Var244->d_change_date_time , 0 , 1);
        duration_n = 0;
        left_n = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var243->a_ap_tree , "vak65.p" , 20131))[sql__setrang((int ) op_node , 0 , 32767)].n_lo_level , -32768 , 32767);
        right_n = (short ) sql__setrang(left_n , -32768 , 32767);
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var243->a_ap_tree , "vak65.p" , 20133))[sql__setrang((int ) right_n , 0 , 32767)].n_symb == 43)
            {
            right_n = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var243->a_ap_tree , "vak65.p" , 20135))[sql__setrang((int ) right_n , 0 , 32767)].n_sa_level , -32768 , 32767);
            }
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var243->a_ap_tree , "vak65.p" , 20137))[sql__setrang((int ) right_n , 0 , 32767)].n_symb == 44)
            {
            right_n = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var243->a_ap_tree , "vak65.p" , 20139))[sql__setrang((int ) right_n , 0 , 32767)].n_sa_level , -32768 , 32767);
            }
        last_left_n = (short ) sql__setrang(right_n , -32768 , 32767);
        right_n = (short ) sql__setrang((*(tak_ap_max_tree *) sql__nil(ptoc_Var243->a_ap_tree , "vak65.p" , 20142))[sql__setrang((int ) right_n , 0 , 32767)].n_sa_level , -32768 , 32767);
        a65_look_for_datatypes(&(*acv) , &(*dmli) , (int ) left_n);
        if ((ptoc_Var245 = (int ) ptoc_Var244->d_datatype) == 15 || ptoc_Var245 == 0 || ptoc_Var245 == 1 || ptoc_Var245 == 12 || ptoc_Var245 == 14 || ptoc_Var245 == 17)
            {
            a65_look_for_datatypes(&(*acv) , &(*dmli) , (int ) right_n);
            if (((ptoc_Var245 = (int ) ptoc_Var244->d_datatype) == 13 || ptoc_Var245 == 11 || ptoc_Var245 == 10) && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var243->a_ap_tree , "vak65.p" , 20150))[sql__setrang((int ) op_node , 0 , 32767)].n_symb == 11)
                {
                date_arith = 1;
                op_dtype = (unsigned char ) sql__setrang(ptoc_Var244->d_datatype , 0 , 39);
                duration_n = (short ) sql__setrang(left_n , -32768 , 32767);
                }
            }
        else
            {
            if ((ptoc_Var245 = (int ) ptoc_Var244->d_datatype) == 13 || ptoc_Var245 == 10 || ptoc_Var245 == 11)
                {
                op_dtype = (unsigned char ) sql__setrang(ptoc_Var244->d_datatype , 0 , 39);
                ptoc_Var246 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var243->a_ap_tree , "vak65.p" , 20164))[sql__setrang((int ) right_n , 0 , 32767)];
                if ((int ) ptoc_Var243->a_sqlmode == 2 && (int ) ptoc_Var246->n_proc == 55 && (int ) ptoc_Var246->n_symb == 125)
                    {
                    ptoc_Var244->d_datatype = 17;
                    }
                else
                    {
                    a65_look_for_datatypes(&(*acv) , &(*dmli) , (int ) right_n);
                    }
                if ((ptoc_Var245 = (int ) ptoc_Var244->d_datatype) == 15 || ptoc_Var245 == 0 || ptoc_Var245 == 1 || ptoc_Var245 == 12 || ptoc_Var245 == 14 || ptoc_Var245 == 17)
                    {
                    date_arith = 1;
                    duration_n = (short ) sql__setrang(right_n , -32768 , 32767);
                    }
                else
                    {
                    if (((ptoc_Var245 = (int ) ptoc_Var244->d_datatype) == 13 || ptoc_Var245 == 10 || ptoc_Var245 == 11) && (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var243->a_ap_tree , "vak65.p" , 20182))[sql__setrang((int ) op_node , 0 , 32767)].n_symb == 10)
                        {
                        date_arith = 1;
                        op_dtype = 15;
                        }
                    }
                }
            }
        if ((int ) duration_n != 0 && (int ) ptoc_Var243->a_sqlmode == 2 && !(char ) sql__setrang(look_for_dtype , 0 , 1))
            {
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var243->a_ap_tree , "vak65.p" , 20196))[sql__setrang((int ) duration_n , 0 , 32767)].n_proc != 55 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var243->a_ap_tree , "vak65.p" , 20197))[sql__setrang((int ) duration_n , 0 , 32767)].n_subproc != 11 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var243->a_ap_tree , "vak65.p" , 20198))[sql__setrang((int ) duration_n , 0 , 32767)].n_symb != 125)
                {
                ptoc_Var243->a_scv.sc_symb = 125;
                a01_call_put(&(*acv) , 55 , 11 , &new_node);
                (*(tak_ap_max_tree *) sql__nil(ptoc_Var243->a_ap_tree , "vak65.p" , 20203))[sql__setrang((int ) new_node , 0 , 32767)].n_symb = (unsigned char ) sql__setrang(ptoc_Var243->a_scv.sc_symb , 0 , 240);
                (*(tak_ap_max_tree *) sql__nil(ptoc_Var243->a_ap_tree , "vak65.p" , 20204))[sql__setrang((int ) new_node , 0 , 32767)].n_lo_level = (short ) sql__setrang(duration_n , -32768 , 32767);
                if ((int ) duration_n == (int ) left_n)
                    {
                    (*(tak_ap_max_tree *) sql__nil(ptoc_Var243->a_ap_tree , "vak65.p" , 20208))[sql__setrang((int ) op_node , 0 , 32767)].n_lo_level = (short ) sql__setrang(right_n , -32768 , 32767);
                    (*(tak_ap_max_tree *) sql__nil(ptoc_Var243->a_ap_tree , "vak65.p" , 20209))[sql__setrang((int ) right_n , 0 , 32767)].n_sa_level = (short ) sql__setrang(new_node , -32768 , 32767);
                    (*(tak_ap_max_tree *) sql__nil(ptoc_Var243->a_ap_tree , "vak65.p" , 20210))[sql__setrang((int ) last_left_n , 0 , 32767)].n_sa_level = 0;
                    }
                else
                    {
                    (*(tak_ap_max_tree *) sql__nil(ptoc_Var243->a_ap_tree , "vak65.p" , 20213))[sql__setrang((int ) last_left_n , 0 , 32767)].n_sa_level = (short ) sql__setrang(new_node , -32768 , 32767);
                    }
                }
            }
        ptoc_Var244->d_change_date_time = (char ) sql__setrang(save_change_date_time , 0 , 1);
        if ((char ) sql__setrang(look_for_dtype , 0 , 1) && (char ) sql__setrang(date_arith , 0 , 1))
            {
            ptoc_Var244->d_datatype = (unsigned char ) sql__setrang(op_dtype , 0 , 39);
            }
        else
            {
            ptoc_Var244->d_datatype = (unsigned char ) sql__setrang(save_datatype , 0 , 39);
            }
        }
    _ak65is_datetime_arith = (char ) sql__setrang(date_arith , 0 , 1);
    cccprint("<ak65is_datetime");
    return(_ak65is_datetime_arith);
    }



int ak65set_op_datetime_arith(acv, dmli, colin, colin1, act_node, is_plus, dt_expected)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
tak00_scolinf *colin1;
tsp_int4 act_node;
char is_plus;
expected_datetime_value dt_expected;
    {
    int dig_len;
    int dig_frac;
    tak_all_command_glob *ptoc_Var247;
    tak_dml_info *ptoc_Var248;
    struct tgg00_MessBlock *ptoc_Var249;
    tgg00_QualBuf *ptoc_Var250;
    tak00_scolinf *ptoc_Var251;
    int ptoc_Var252;
    struct ptoc_Type38 *ptoc_Var253;
    struct ptoc_Type38 *ptoc_Var254;
    struct ptoc_Type38 *ptoc_Var255;
    struct ptoc_Type38 *ptoc_Var256;
    struct ptoc_Type38 *ptoc_Var257;
    cccprint("AK65  ak65set_op");
    ptoc_Var247 = &*acv;
    ptoc_Var248 = &*dmli;
    ptoc_Var249 = &ptoc_Var247->a_mblock;
    ptoc_Var250 = (tgg00_QualBuf *) sql__nil(ptoc_Var249->integer.C_4.mb_qual , "vak65.p" , 20245);
    ptoc_Var251 = &*colin;
    if ((int ) ptoc_Var250->integer.C_2.mfirst_free >= (int ) ptoc_Var249->integer.C_2.mb_st_max)
        {
        a07_b_put_error(&(*acv) , 2800 , -(int ) ptoc_Var249->integer.C_2.mb_st_max);
        }
    else
        {
        if ((int ) ptoc_Var248->d_datatype == 24 && ((ptoc_Var252 = (int ) dt_expected) == 1 || ptoc_Var252 == 3))
            {
            (*colin).sci_typ = 2;
            ptoc_Var248->d_datatype = 2;
            (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len + 1) , -32768 , 32767);
            a641string_set_operator(&(*acv) , 66 , (int ) (*colin).sci_iolen , 0 , (unsigned char)'\024' , (unsigned char)'\0');
            }
        switch ((int ) dt_expected)
            {
            case 1 :
                if ((ptoc_Var252 = (int ) ptoc_Var248->d_datatype) != 10 && ptoc_Var252 != 15 && ptoc_Var252 != 17 && ptoc_Var252 != 2 && ptoc_Var252 != 3 || (int ) ptoc_Var248->d_datatype == 17 && ((ptoc_Var252 = (int ) (*colin1).sci_dttyp) != 0 && ptoc_Var252 != 1 && ptoc_Var252 != 2))
                    {
                    a07_b_put_error(&(*acv) , 6276 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var247->a_ap_tree , "vak65.p" , 20272))[sql__setrang(act_node , 0 , 32767)].n_pos);
                    }
                else
                    {
                    if ((int ) ptoc_Var248->d_datatype == 15 && ((int ) (*colin1).sci_len > 8 || (int ) (*colin1).sci_frac != 0))
                        {
                        a07_b_put_error(&(*acv) , 2500 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var247->a_ap_tree , "vak65.p" , 20279))[sql__setrang(act_node , 0 , 32767)].n_pos);
                        }
                    else
                        {
                        if ((ptoc_Var252 = (int ) ptoc_Var248->d_datatype) == 2 || ptoc_Var252 == 3)
                            {
                            a641check_datetime(&(*acv) , &(*dmli) , 10);
                            ptoc_Var248->d_datatype = 10;
                            }
                        ptoc_Var253 = &(*(tgg00_StackList *) sql__nil(ptoc_Var249->integer.C_5.mb_st , "vak65.p" , 20289))[sql__setrang((int ) ptoc_Var250->integer.C_2.mfirst_free , 1 , 32767) + -1];
                        ptoc_Var253->integer.C_1.etype = 47;
                        ptoc_Var253->integer.C_1.eop = 85;
                        if ((int ) ptoc_Var248->d_datatype == 10)
                            {
                            ptoc_Var248->d_datatype = 15;
                            ptoc_Var251->sci_typ = 0;
                            ptoc_Var251->sci_len = 8;
                            ptoc_Var251->sci_iolen = 6;
                            ptoc_Var253->integer.C_1.elen_var = 0;
                            ptoc_Var253->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                            }
                        else
                            {
                            ptoc_Var251->sci_typ = 10;
                            ptoc_Var251->sci_len = 8;
                            ptoc_Var251->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var251->sci_len + 1) , -32768 , 32767);
                            if ((int ) ptoc_Var248->d_datatype == 15)
                                {
                                ptoc_Var253->integer.C_1.elen_var = 1;
                                ptoc_Var253->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                                }
                            else
                                {
                                ptoc_Var253->integer.C_1.elen_var = 2;
                                ptoc_Var253->integer.C_1.ecol_tab[0] = (unsigned char ) sql__setrang((unsigned char ) sql__ord((int ) (*colin1).sci_dttyp) , 0 , 255);
                                }
                            ptoc_Var248->d_datatype = 10;
                            }
                        ptoc_Var253->integer.C_1.epos = 0;
                        ptoc_Var253->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                        }
                    }
                break;
            case 0 :
                if ((ptoc_Var252 = (int ) ptoc_Var248->d_datatype) != 15 && ptoc_Var252 != 17 || (int ) ptoc_Var248->d_datatype == 17 && ((ptoc_Var252 = (int ) (*colin1).sci_dttyp) != 0 && ptoc_Var252 != 1 && ptoc_Var252 != 2))
                    {
                    a07_b_put_error(&(*acv) , 6276 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var247->a_ap_tree , "vak65.p" , 20337))[sql__setrang(act_node , 0 , 32767)].n_pos);
                    }
                else
                    {
                    if ((int ) ptoc_Var248->d_datatype == 15 && ((int ) (*colin1).sci_len > 8 || (int ) (*colin1).sci_frac != 0))
                        {
                        a07_b_put_error(&(*acv) , 2500 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var247->a_ap_tree , "vak65.p" , 20344))[sql__setrang(act_node , 0 , 32767)].n_pos);
                        }
                    else
                        {
                        ptoc_Var254 = &(*(tgg00_StackList *) sql__nil(ptoc_Var249->integer.C_5.mb_st , "vak65.p" , 20346))[sql__setrang((int ) ptoc_Var250->integer.C_2.mfirst_free , 1 , 32767) + -1];
                        ptoc_Var251->sci_typ = 10;
                        ptoc_Var254->integer.C_1.etype = 47;
                        ptoc_Var254->integer.C_1.eop = 90;
                        ptoc_Var251->sci_len = 8;
                        ptoc_Var251->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var251->sci_len + 1) , -32768 , 32767);
                        if ((int ) ptoc_Var248->d_datatype == 15)
                            {
                            ptoc_Var254->integer.C_1.elen_var = 1;
                            ptoc_Var254->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                            }
                        else
                            {
                            ptoc_Var254->integer.C_1.elen_var = 2;
                            ptoc_Var254->integer.C_1.ecol_tab[0] = (unsigned char ) sql__setrang((unsigned char ) sql__ord((int ) (*colin1).sci_dttyp) , 0 , 255);
                            }
                        ptoc_Var248->d_datatype = 10;
                        ptoc_Var254->integer.C_1.epos = 0;
                        ptoc_Var254->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                        }
                    }
                break;
            case 3 :
                if ((ptoc_Var252 = (int ) ptoc_Var248->d_datatype) != 11 && ptoc_Var252 != 15 && ptoc_Var252 != 17 && ptoc_Var252 != 2 && ptoc_Var252 != 3 || (int ) ptoc_Var248->d_datatype == 17 && ((ptoc_Var252 = (int ) (*colin1).sci_dttyp) != 3 && ptoc_Var252 != 4 && ptoc_Var252 != 5))
                    {
                    a07_b_put_error(&(*acv) , 6276 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var247->a_ap_tree , "vak65.p" , 20380))[sql__setrang(act_node , 0 , 32767)].n_pos);
                    }
                else
                    {
                    if ((int ) ptoc_Var248->d_datatype == 15 && ((int ) (*colin1).sci_len > 6 || (int ) (*colin1).sci_frac != 0))
                        {
                        a07_b_put_error(&(*acv) , 2530 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var247->a_ap_tree , "vak65.p" , 20387))[sql__setrang(act_node , 0 , 32767)].n_pos);
                        }
                    else
                        {
                        if ((ptoc_Var252 = (int ) ptoc_Var248->d_datatype) == 2 || ptoc_Var252 == 3)
                            {
                            a641check_datetime(&(*acv) , &(*dmli) , 11);
                            ptoc_Var248->d_datatype = 11;
                            }
                        ptoc_Var255 = &(*(tgg00_StackList *) sql__nil(ptoc_Var249->integer.C_5.mb_st , "vak65.p" , 20397))[sql__setrang((int ) ptoc_Var250->integer.C_2.mfirst_free , 1 , 32767) + -1];
                        ptoc_Var255->integer.C_1.etype = 47;
                        ptoc_Var255->integer.C_1.eop = 85;
                        if ((int ) ptoc_Var248->d_datatype == 11)
                            {
                            ptoc_Var248->d_datatype = 15;
                            ptoc_Var251->sci_typ = 0;
                            ptoc_Var251->sci_len = 6;
                            ptoc_Var251->sci_iolen = 5;
                            ptoc_Var255->integer.C_1.elen_var = 0;
                            ptoc_Var255->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                            }
                        else
                            {
                            ptoc_Var251->sci_typ = 11;
                            ptoc_Var251->sci_len = 8;
                            ptoc_Var251->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var251->sci_len + 1) , -32768 , 32767);
                            if ((int ) ptoc_Var248->d_datatype == 15)
                                {
                                ptoc_Var255->integer.C_1.elen_var = 1;
                                ptoc_Var255->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                                }
                            else
                                {
                                ptoc_Var255->integer.C_1.elen_var = 2;
                                ptoc_Var255->integer.C_1.ecol_tab[0] = (unsigned char ) sql__setrang((unsigned char ) sql__ord((int ) (*colin1).sci_dttyp) , 0 , 255);
                                }
                            ptoc_Var248->d_datatype = 11;
                            }
                        ptoc_Var255->integer.C_1.epos = 1;
                        ptoc_Var255->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                        }
                    }
                break;
            case 2 :
                if ((ptoc_Var252 = (int ) ptoc_Var248->d_datatype) != 15 && ptoc_Var252 != 17 || (int ) ptoc_Var248->d_datatype == 17 && ((ptoc_Var252 = (int ) (*colin1).sci_dttyp) != 3 && ptoc_Var252 != 4 && ptoc_Var252 != 5))
                    {
                    a07_b_put_error(&(*acv) , 6276 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var247->a_ap_tree , "vak65.p" , 20445))[sql__setrang(act_node , 0 , 32767)].n_pos);
                    }
                else
                    {
                    if ((int ) ptoc_Var248->d_datatype == 15 && ((int ) (*colin1).sci_len > 6 || (int ) (*colin1).sci_frac != 0))
                        {
                        a07_b_put_error(&(*acv) , 2530 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var247->a_ap_tree , "vak65.p" , 20452))[sql__setrang(act_node , 0 , 32767)].n_pos);
                        }
                    else
                        {
                        ptoc_Var256 = &(*(tgg00_StackList *) sql__nil(ptoc_Var249->integer.C_5.mb_st , "vak65.p" , 20454))[sql__setrang((int ) ptoc_Var250->integer.C_2.mfirst_free , 1 , 32767) + -1];
                        ptoc_Var251->sci_typ = 11;
                        ptoc_Var256->integer.C_1.etype = 47;
                        ptoc_Var256->integer.C_1.eop = 90;
                        ptoc_Var251->sci_len = 8;
                        ptoc_Var251->sci_iolen = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var251->sci_len , -32768 , 32767 , "vak65.p" , 20460) , -32768 , 32767);
                        if ((int ) ptoc_Var248->d_datatype == 15)
                            {
                            ptoc_Var256->integer.C_1.elen_var = 1;
                            ptoc_Var256->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                            }
                        else
                            {
                            ptoc_Var256->integer.C_1.elen_var = 2;
                            ptoc_Var256->integer.C_1.ecol_tab[0] = (unsigned char ) sql__setrang((unsigned char ) sql__ord((int ) (*colin1).sci_dttyp) , 0 , 255);
                            }
                        ptoc_Var248->d_datatype = 11;
                        ptoc_Var256->integer.C_1.epos = 1;
                        ptoc_Var256->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                        }
                    }
                break;
            case 4 :
                if ((ptoc_Var252 = (int ) ptoc_Var248->d_datatype) != 15 && ptoc_Var252 != 17 && ptoc_Var252 != 13 || (int ) ptoc_Var248->d_datatype == 17 && ((ptoc_Var252 = (int ) (*colin1).sci_dttyp) != 0 && ptoc_Var252 != 1 && ptoc_Var252 != 2 && ptoc_Var252 != 3 && ptoc_Var252 != 4 && ptoc_Var252 != 5 && ptoc_Var252 != 6) || (int ) ptoc_Var248->d_datatype == 13 && ((int ) ptoc_Var247->a_sqlmode != 2 && (int ) ptoc_Var247->a_sqlmode != 1 || (char ) sql__setrang(is_plus , 0 , 1)))
                    {
                    a07_b_put_error(&(*acv) , 6276 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var247->a_ap_tree , "vak65.p" , 20493))[sql__setrang(act_node , 0 , 32767)].n_pos);
                    }
                else
                    {
                    ptoc_Var257 = &(*(tgg00_StackList *) sql__nil(ptoc_Var249->integer.C_5.mb_st , "vak65.p" , 20495))[sql__setrang((int ) ptoc_Var250->integer.C_2.mfirst_free , 1 , 32767) + -1];
                    if ((ptoc_Var252 = (int ) ptoc_Var248->d_datatype) == 2 || ptoc_Var252 == 3)
                        {
                        a641check_datetime(&(*acv) , &(*dmli) , 13);
                        ptoc_Var248->d_datatype = 13;
                        }
                    ptoc_Var257->integer.C_1.etype = 47;
                    if ((char ) sql__setrang(is_plus , 0 , 1))
                        {
                        ptoc_Var257->integer.C_1.eop = 90;
                        }
                    else
                        {
                        ptoc_Var257->integer.C_1.eop = 85;
                        }
                    ptoc_Var257->integer.C_1.epos = 2;
                    ptoc_Var257->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                    if ((int ) ptoc_Var248->d_datatype == 13)
                        {
                        ptoc_Var248->d_datatype = 15;
                        ptoc_Var257->integer.C_1.elen_var = 0;
                        switch ((int ) ptoc_Var247->a_sqlmode)
                            {
                            case 1 :
                                ptoc_Var251->sci_typ = 0;
                                ptoc_Var251->sci_iolen = 12;
                                ptoc_Var251->sci_len = 20;
                                ptoc_Var251->sci_frac = 6;
                                ptoc_Var257->integer.C_1.ecol_tab[0] = (unsigned char)'\001';
                                break;
                            default :
                                ptoc_Var251->sci_typ = 1;
                                ptoc_Var251->sci_iolen = 21;
                                ptoc_Var251->sci_len = 38;
                                ptoc_Var251->sci_frac = -1;
                                ptoc_Var257->integer.C_1.ecol_tab[0] = (unsigned char)'\004';
                                break;
                            }
                        }
                    else
                        {
                        if ((int ) ptoc_Var248->d_datatype == 15)
                            {
                            a05get_digit((*(tsp1_part *) sql__nil(ptoc_Var247->a_cmd_part , "vak65.p" , 20543)).integer.C_1.sp1p_buf , (*(tak_ap_max_tree *) sql__nil(ptoc_Var247->a_ap_tree , "vak65.p" , 20544))[sql__setrang(act_node , 0 , 32767)].n_pos , (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var247->a_ap_tree , "vak65.p" , 20545))[sql__setrang(act_node , 0 , 32767)].n_length , &dig_len , &dig_frac);
                            t01int4(5 , "dig_len     " , dig_len);
                            t01int4(5 , "dig_frac    " , dig_frac);
                            switch ((int ) ptoc_Var247->a_sqlmode)
                                {
                                case 1 :
                                    if (dig_len - dig_frac > 14 || dig_frac > 6)
                                        {
                                        a07_b_put_error(&(*acv) , 6276 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var247->a_ap_tree , "vak65.p" , 20556))[sql__setrang(act_node , 0 , 32767)].n_pos);
                                        }
                                    break;
                                default :
                                    if (dig_frac != 0)
                                        {
                                        a07_b_put_error(&(*acv) , 2540 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var247->a_ap_tree , "vak65.p" , 20563))[sql__setrang(act_node , 0 , 32767)].n_pos);
                                        }
                                    break;
                                }
                            }
                        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var247->a_return_segm , "vak65.p" , 20569)).integer.C_4.sp1r_returncode == 0)
                            {
                            ptoc_Var251->sci_typ = 13;
                            ptoc_Var251->sci_len = 20;
                            ptoc_Var251->sci_iolen = (short ) sql__setrang((short ) ((int ) ptoc_Var251->sci_len + 1) , -32768 , 32767);
                            if ((int ) ptoc_Var248->d_datatype == 15)
                                {
                                ptoc_Var257->integer.C_1.elen_var = 1;
                                ptoc_Var257->integer.C_1.ecol_tab[0] = (unsigned char)'\005';
                                }
                            else
                                {
                                ptoc_Var257->integer.C_1.elen_var = 2;
                                ptoc_Var257->integer.C_1.ecol_tab[0] = (unsigned char ) sql__setrang((unsigned char ) sql__ord((int ) (*colin1).sci_dttyp) , 0 , 255);
                                }
                            ptoc_Var248->d_datatype = 13;
                            }
                        }
                    }
                break;
            default :
                (unsigned char ) sql__caseerr("vak65.p" , 20263);
                break;
            }
        ptoc_Var250->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var250->integer.C_2.mfirst_free , -32768 , 32767 , "vak65.p" , 20598) , -32768 , 32767);
        ptoc_Var250->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var250->integer.C_2.mqual_cnt , -32768 , 32767 , "vak65.p" , 20599) , -32768 , 32767);
        }
    cccprint("<ak65set_op_date");
    }



int ak65not_first_val_expr(acv, dmli, colin, first_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int first_node;
    {
    int st_begin;
    int st_index;
    cccprint("AK65  ak65not_fi");
    st_begin = (int ) (*dmli).d_param_st_begin;
    st_index = (int ) (*dmli).d_param_st_index;
    (*dmli).d_param_st_begin = 0;
    (*dmli).d_param_st_index = 0;
    a65_val_expr(&(*acv) , &(*dmli) , &(*colin) , first_node);
    (*dmli).d_param_st_begin = (short ) sql__setrang((short ) st_begin , -32768 , 32767);
    if ((int ) (*dmli).d_param_st_index == 0)
        {
        (*dmli).d_param_st_index = (short ) sql__setrang((short ) st_index , -32768 , 32767);
        }
    cccprint("<ak65not_first_v");
    }



int a65_val_expr(acv, dmli, colin, first_node)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak00_scolinf *colin;
int first_node;
    {
    char datetime_arith;
    char change_date_time;
    char keep_plus;
    char dummy_ok;
    expected_datetime_value dt_expected;
    int act_node;
    tak_convert_type convert_t;
    tsp_data_type keep_datatype;
    tak00_scolinf colin1;
    char const_value_expr;
    int st_begin;
    int st_end;
    tsp_int4 pos;
    tsp_int2 expr_st_cnt;
    char save_single_expr;
    tak_all_command_glob *ptoc_Var258;
    struct ptoc_Type56 *ptoc_Var259;
    tak_dml_info *ptoc_Var260;
    int ptoc_Var261;
    struct tgg00_MessBlock *ptoc_Var262;
    tgg00_QualBuf *ptoc_Var263;
    cccprint("AK65  a65_val_ex");
    ptoc_Var258 = &*acv;
    ptoc_Var259 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var258->a_ap_tree , "vak65.p" , 20658))[sql__setrang(first_node , 0 , 32767)];
    ptoc_Var260 = &*dmli;
    t01int4(5 , "d_having    " , sql__ord((int ) (char ) sql__setrang(ptoc_Var260->d_having , 0 , 1)));
    t01int4(5 , "d_subquery  " , sql__ord((int ) (char ) sql__setrang(ptoc_Var260->d_subquery , 0 , 1)));
    t01int4(5 , "d_is_functio" , sql__ord((int ) (char ) sql__setrang(ptoc_Var260->d_is_function_column , 0 , 1)));
    save_single_expr = (char ) sql__setrang(ptoc_Var260->d_single_expr , 0 , 1);
    ptoc_Var260->d_single_expr = 0;
    if ((char ) sql__setrang(ptoc_Var260->d_having , 0 , 1) && !(char ) sql__setrang(ptoc_Var260->d_subquery , 0 , 1) && !(char ) sql__setrang(ptoc_Var260->d_is_function_column , 0 , 1) && (int ) ptoc_Var260->d_corr == 0)
        {
        a61_is_group_field(&(*acv) , &(*dmli) , ptoc_Var260->d_column , (tsp00_KnlIdentifier *) ptoc_Var260->d_column , 0 , 0 , 1 , (short ) sql__setrang((short ) first_node , -32768 , 32767) , ptoc_Var259->n_pos);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var258->a_return_segm , "vak65.p" , 20672)).integer.C_4.sp1r_returncode == 0)
        {
        const_value_expr = (char ) sql__setrang(ptoc_Var260->d_const_value_expr , 0 , 1);
        ptoc_Var260->d_const_value_expr = 1;
        st_begin = (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var258->a_mblock.integer.C_4.mb_qual , "vak65.p" , 20677)).integer.C_2.mfirst_free;
        ptoc_Var260->d_concat = 0;
        ptoc_Var260->d_hex = 0;
        if ((int ) ptoc_Var259->n_proc != 55 || (int ) ptoc_Var259->n_subproc != 4)
            {
            act_node = (int ) ptoc_Var259->n_lo_level;
            a65_term(&(*acv) , &(*dmli) , &(*colin) , &first_node);
            }
        else
            {
            keep_plus = (char ) sql__setrang((int ) ptoc_Var259->n_symb == 11 , 0 , 1);
            ptoc_Var260->d_colptr = (tak00_columninfo *) (int *) 0;
            keep_datatype = (unsigned char ) sql__setrang(ptoc_Var260->d_datatype , 0 , 39);
            ptoc_Var260->d_datatype = 14;
            change_date_time = (char ) sql__setrang(ptoc_Var260->d_change_date_time , 0 , 1);
            ptoc_Var260->d_change_date_time = 0;
            act_node = (int ) ptoc_Var259->n_lo_level;
            datetime_arith = (char ) sql__setrang(ak65is_datetime_arith(&(*acv) , &(*dmli) , (short ) sql__setrang((short ) first_node , -32768 , 32767) , 0) , 0 , 1);
            if ((char ) sql__setrang(datetime_arith , 0 , 1) && (int ) ptoc_Var258->a_sqlmode == 2)
                {
                dt_expected = 4;
                act_node = (int ) ptoc_Var259->n_lo_level;
                }
            a65_term(&(*acv) , &(*dmli) , &(*colin) , &act_node);
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var258->a_return_segm , "vak65.p" , 20706)).integer.C_4.sp1r_returncode == 21000)
                {
                (*(tsp1_segment *) sql__nil(ptoc_Var258->a_return_segm , "vak65.p" , 20709)).integer.C_4.sp1r_returncode = 0;
                a65_look_for_datatypes(&(*acv) , &(*dmli) , act_node);
                if ((int ) ptoc_Var260->d_datatype == 14)
                    {
                    ptoc_Var260->d_datatype = 15;
                    }
                a65_term(&(*acv) , &(*dmli) , &(*colin) , &act_node);
                }
            if ((int ) ptoc_Var258->a_sqlmode == 2 && (char ) sql__setrang(datetime_arith , 0 , 1))
                {
                ptoc_Var260->d_datatype = 14;
                }
            else
                {
                if ((ptoc_Var261 = (int ) ptoc_Var260->d_datatype) == 2 || ptoc_Var261 == 3 || ptoc_Var261 == 24 || ptoc_Var261 == 10 || ptoc_Var261 == 11 || ptoc_Var261 == 13)
                    {
                    if ((ptoc_Var261 = (int ) ptoc_Var260->d_datatype) == 2 || ptoc_Var261 == 3 || ptoc_Var261 == 24)
                        {
                        if ((int ) ptoc_Var258->a_sqlmode != 1)
                            {
                            a07_b_put_error(&(*acv) , 6276 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var258->a_ap_tree , "vak65.p" , 20731))[sql__setrang(first_node , 0 , 32767)].n_pos);
                            }
                        else
                            {
                            if ((int ) ptoc_Var260->d_datatype == 24)
                                {
                                a641string_set_operator(&(*acv) , 66 , (int ) (*colin).sci_len + 1 , 0 , (unsigned char)'\024' , (unsigned char)'\0');
                                (*colin).sci_iolen = (short ) sql__setrang((short ) ((int ) (*colin).sci_len + 1) , -32768 , 32767);
                                (*colin).sci_typ = 2;
                                }
                            switch ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var258->a_ap_tree , "vak65.p" , 20744))[sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var258->a_ap_tree , "vak65.p" , 20744))[sql__setrang(act_node , 0 , 32767)].n_sa_level , 0 , 32767)].n_symb)
                                {
                                case 135 :
                                case 132 :
                                case 125 :
                                    if ((int ) ptoc_Var259->n_proc == 0)
                                        {
                                        ptoc_Var260->d_datatype = 15;
                                        }
                                    else
                                        {
                                        if ((int ) (*colin).sci_len > 10)
                                            {
                                            ptoc_Var260->d_datatype = 13;
                                            }
                                        else
                                            {
                                            ptoc_Var260->d_datatype = 10;
                                            }
                                        }
                                    break;
                                case 129 :
                                case 131 :
                                case 133 :
                                    if ((int ) ptoc_Var259->n_proc == 0)
                                        {
                                        ptoc_Var260->d_datatype = 15;
                                        }
                                    else
                                        {
                                        if ((int ) (*colin).sci_len > 8)
                                            {
                                            ptoc_Var260->d_datatype = 13;
                                            }
                                        else
                                            {
                                            ptoc_Var260->d_datatype = 11;
                                            }
                                        }
                                    break;
                                default :
                                    a65_look_for_datatypes(&(*acv) , &(*dmli) , (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var258->a_ap_tree , "vak65.p" , 20772))[sql__setrang(act_node , 0 , 32767)].n_sa_level);
                                    break;
                                }
                            if ((ptoc_Var261 = (int ) ptoc_Var260->d_datatype) == 10 || ptoc_Var261 == 11 || ptoc_Var261 == 13)
                                {
                                a641check_datetime(&(*acv) , &(*dmli) , (unsigned char ) sql__setrang(ptoc_Var260->d_datatype , 0 , 39));
                                }
                            else
                                {
                                a07_b_put_error(&(*acv) , 6276 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var258->a_ap_tree , "vak65.p" , 20780))[sql__setrang(first_node , 0 , 32767)].n_pos);
                                }
                            }
                        }
                    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var258->a_return_segm , "vak65.p" , 20785)).integer.C_4.sp1r_returncode == 0)
                        {
                        if ((int ) ptoc_Var258->a_sqlmode == 1)
                            {
                            datetime_arith = 1;
                            switch ((int ) ptoc_Var260->d_datatype)
                                {
                                case 10 :
                                    if ((char ) sql__setrang(keep_plus , 0 , 1))
                                        {
                                        dt_expected = 0;
                                        }
                                    else
                                        {
                                        dt_expected = 1;
                                        }
                                    break;
                                case 11 :
                                    if ((char ) sql__setrang(keep_plus , 0 , 1))
                                        {
                                        dt_expected = 2;
                                        }
                                    else
                                        {
                                        dt_expected = 3;
                                        }
                                    break;
                                case 13 :
                                    dt_expected = 4;
                                    break;
                                default :
                                    (unsigned char ) sql__caseerr("vak65.p" , 20791);
                                    break;
                                }
                            ptoc_Var260->d_datatype = 14;
                            }
                        else
                            {
                            a07_b_put_error(&(*acv) , 6276 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var258->a_ap_tree , "vak65.p" , 20814))[sql__setrang(first_node , 0 , 32767)].n_pos);
                            }
                        }
                    }
                else
                    {
                    if ((ptoc_Var261 = (int ) ptoc_Var260->d_datatype) != 15 && ptoc_Var261 != 0 && ptoc_Var261 != 1 && ptoc_Var261 != 12)
                        {
                        a07_b_put_error(&(*acv) , 6276 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var258->a_ap_tree , "vak65.p" , 20822))[sql__setrang(first_node , 0 , 32767)].n_pos);
                        }
                    }
                }
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var258->a_return_segm , "vak65.p" , 20826)).integer.C_4.sp1r_returncode == 0)
                {
                act_node = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var258->a_ap_tree , "vak65.p" , 20829))[sql__setrang(act_node , 0 , 32767)].n_sa_level;
                colin1.sci_len = 0;
                a65not_first_term(&(*acv) , &(*dmli) , &colin1 , &act_node);
                }
            t01int4(5 , "pmtcoli  len" , (int ) (*colin).sci_len);
            t01int4(5 , "pmtcoli  fra" , (int ) (*colin).sci_frac);
            t01int4(5 , "pmtcoli  iol" , (int ) (*colin).sci_iolen);
            t01int4(5 , "pmtcoli1 len" , (int ) colin1.sci_len);
            t01int4(5 , "pmtcoli1 fra" , (int ) colin1.sci_frac);
            t01int4(5 , "pmtcoli1 iol" , (int ) colin1.sci_iolen);
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var258->a_return_segm , "vak65.p" , 20840)).integer.C_4.sp1r_returncode == 21000)
                {
                if (!(char ) sql__setrang(keep_plus , 0 , 1))
                    {
                    (*(tsp1_segment *) sql__nil(ptoc_Var258->a_return_segm , "vak65.p" , 20846)).integer.C_4.sp1r_returncode = 0;
                    a07_b_put_error(&(*acv) , 6556 , (*(tak_ap_max_tree *) sql__nil(ptoc_Var258->a_ap_tree , "vak65.p" , 20848))[sql__setrang(act_node , 0 , 32767)].n_pos);
                    }
                else
                    {
                    (*(tsp1_segment *) sql__nil(ptoc_Var258->a_return_segm , "vak65.p" , 20852)).integer.C_4.sp1r_returncode = 0;
                    a65_look_for_datatypes(&(*acv) , &(*dmli) , act_node);
                    if ((int ) ptoc_Var260->d_datatype == 14)
                        {
                        ptoc_Var260->d_datatype = 15;
                        }
                    a65not_first_term(&(*acv) , &(*dmli) , &colin1 , &act_node);
                    }
                }
            if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var258->a_return_segm , "vak65.p" , 20862)).integer.C_4.sp1r_returncode == 0)
                {
                if ((char ) sql__setrang(datetime_arith , 0 , 1))
                    {
                    ak65set_op_datetime_arith(&(*acv) , &(*dmli) , &(*colin) , &colin1 , act_node , (char ) sql__setrang(keep_plus , 0 , 1) , (unsigned char ) sql__setrang(dt_expected , 0 , 4));
                    }
                else
                    {
                    if ((char ) sql__setrang(keep_plus , 0 , 1))
                        {
                        ak65new_colinfo(&(*acv) , &(*colin) , &colin1 , 11);
                        a65_set_operator(&(*acv) , 90);
                        }
                    else
                        {
                        ak65new_colinfo(&(*acv) , &(*colin) , &colin1 , 10);
                        a65_set_operator(&(*acv) , 85);
                        }
                    }
                t01int4(5 , "pmtcoli  len" , (int ) (*colin).sci_len);
                t01int4(5 , "pmtcoli  fra" , (int ) (*colin).sci_frac);
                t01int4(5 , "pmtcoli  iol" , (int ) (*colin).sci_iolen);
                t01int4(5 , "keep_type   " , sql__ord((int ) keep_datatype));
                t01int4(5 , "d_datatype  " , sql__ord((int ) ptoc_Var260->d_datatype));
                t01int4(5 , "colin_type  " , sql__ord((int ) (*colin).sci_typ));
                t01int4(5 , "d_change    " , sql__ord((int ) (char ) sql__setrang(ptoc_Var260->d_change_date_time , 0 , 1)));
                }
            if ((int ) keep_datatype != 14)
                {
                dummy_ok = (char ) sql__setrang(a65_datatypes_ok(&(*acv) , &(*dmli) , &keep_datatype , &(*colin).sci_iolen , (unsigned char ) sql__setrang(ptoc_Var260->d_datatype , 0 , 39) , 0 , first_node , (*(tak_ap_max_tree *) sql__nil(ptoc_Var258->a_ap_tree , "vak65.p" , 20898))[sql__setrang(first_node , 0 , 32767)].n_pos , 1 , &convert_t) , 0 , 1);
                }
            ptoc_Var260->d_change_date_time = (char ) sql__setrang(change_date_time , 0 , 1);
            }
        }
    if ((char ) sql__setrang(ptoc_Var260->d_const_value_expr , 0 , 1) && !(char ) sql__setrang(ptoc_Var260->d_group , 0 , 1) && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var258->a_return_segm , "vak65.p" , 20907)).integer.C_4.sp1r_returncode == 0)
        {
        expr_st_cnt = (short ) sql__setrang((short ) ((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var258->a_mblock.integer.C_4.mb_qual , "vak65.p" , 20910)).integer.C_2.mfirst_free - st_begin) , -32768 , 32767);
        ptoc_Var262 = &ptoc_Var258->a_mblock;
        ptoc_Var263 = (tgg00_QualBuf *) sql__nil(ptoc_Var262->integer.C_4.mb_qual , "vak65.p" , 20911);
        if ((int ) ptoc_Var260->d_param_st_index > 0 && (int ) expr_st_cnt <= 255 && ((int ) ptoc_Var259->n_proc == 55 && (int ) ptoc_Var259->n_subproc == 4 || st_begin != (int ) ptoc_Var260->d_param_st_begin) && ptoc_Var262->integer.C_2.mb_data_len - (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var262->integer.C_5.mb_st , "vak65.p" , 20916))[sql__setrang(st_begin , 1 , 32767) + -1].integer.C_1.epos + 1 >= (int ) (*colin).sci_iolen)
            {
            a651code_for_const_param_expr(&(*acv) , &(*dmli) , (short ) sql__setrang((short ) st_begin , -32768 , 32767) , (short ) sql__setrang(expr_st_cnt , -32768 , 32767));
            }
        else
            {
            if ((int ) ptoc_Var260->d_param_st_index == 0)
                {
                if ((ptoc_Var261 = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var258->a_ap_tree , "vak65.p" , 20924))[sql__setrang(act_node , 0 , 32767)].n_symb) == 135 || ptoc_Var261 == 132 || ptoc_Var261 == 125 || ptoc_Var261 == 129 || ptoc_Var261 == 131 || ptoc_Var261 == 133 || ptoc_Var261 == 130)
                    {
                    pos = 1;
                    }
                else
                    {
                    pos = (*(tak_ap_max_tree *) sql__nil(ptoc_Var258->a_ap_tree , "vak65.p" , 20929))[sql__setrang(act_node , 0 , 32767)].n_pos;
                    }
                st_end = (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var258->a_mblock.integer.C_4.mb_qual , "vak65.p" , 20931)).integer.C_2.mfirst_free + -1;
                a651value_calculate(&(*acv) , st_begin , st_end , (char ) sql__setrang((int ) (*colin).sci_typ == 4 , 0 , 1) , (char ) sql__setrang(ptoc_Var260->d_like , 0 , 1) , (short ) sql__setrang((*colin).sci_iolen , -32768 , 32767) , pos);
                }
            }
        ptoc_Var260->d_const_value_expr = (char ) sql__setrang(const_value_expr , 0 , 1);
        }
    ptoc_Var260->d_single_expr = (char ) sql__setrang(save_single_expr , 0 , 1);
    cccprint("<a65_val_expr   ");
    }



char ak65only_jvalue_found(dmli, oldjoincnt)
tak_dml_info *dmli;
int oldjoincnt;
    {
    char _ak65only_jvalue_found;
    char only_jvalue_found;
    int j;
    int jval_cnt;
    tak_dml_info *ptoc_Var264;
    tak_joinrec *ptoc_Var265;
    struct ptoc_Type110 *ptoc_Var266;
    cccprint("AK65  ak65only_j");
    ptoc_Var264 = &*dmli;
    ptoc_Var265 = &ptoc_Var264->d_joins;
    j = oldjoincnt;
    jval_cnt = 0;
    while (j < (int ) ptoc_Var265->jrc_cnt)
        {
        if ((int ) ptoc_Var265->jrc_joinarr[sql__setrang(j , 0 , 256)].jo_recs[1].jop_tableno == 99)
            {
            jval_cnt = sql__succ(jval_cnt , -2147483647 , 2147483647 , "vak65.p" , 20970);
            }
        j = sql__succ(j , -2147483647 , 2147483647 , "vak65.p" , 20973);
        }
    only_jvalue_found = (char ) sql__setrang(jval_cnt > 0 && (int ) ptoc_Var265->jrc_cnt - oldjoincnt - jval_cnt == 0 , 0 , 1);
    t01int4(5 , "jrc_cnt     " , (int ) ptoc_Var265->jrc_cnt);
    t01int4(5 , "oldjoincnt  " , oldjoincnt);
    t01int4(5 , "jvalue_found" , sql__ord((int ) (char ) sql__setrang(only_jvalue_found , 0 , 1)));
    if ((char ) sql__setrang(only_jvalue_found , 0 , 1))
        {
        ptoc_Var265->jrc_cnt = (short ) sql__setrang((short ) oldjoincnt , -32768 , 32767);
        ptoc_Var264->d_joins.jrc_joinarr[sql__setrang((int ) ptoc_Var264->d_joins.jrc_cnt , 0 , 256)].jo_col_upd_stat = (unsigned char)'\0';
        ptoc_Var266 = &ptoc_Var265->jrc_joinarr[sql__setrang((int ) ptoc_Var265->jrc_cnt , 0 , 256)];
        ptoc_Var266->jo_no_join = 0;
        ptoc_Var266->jo_recs[0].jop_tableno = (unsigned char)'\0';
        ptoc_Var266->jo_recs[1].jop_tableno = (unsigned char)'\0';
        }
    _ak65only_jvalue_found = (char ) sql__setrang(only_jvalue_found , 0 , 1);
    cccprint("<ak65only_jvalue");
    return(_ak65only_jvalue_found);
    }



char ak65outer_cond_found(dmli, oldjoincnt)
tak_dml_info *dmli;
int oldjoincnt;
    {
    char _ak65outer_cond_found;
    char outer_cond_found;
    int j;
    tak_dml_info *ptoc_Var267;
    tak_joinrec *ptoc_Var268;
    cccprint("AK65  ak65outer_");
    ptoc_Var267 = &*dmli;
    ptoc_Var268 = &ptoc_Var267->d_joins;
    outer_cond_found = 0;
    j = oldjoincnt;
    while (j < (int ) ptoc_Var268->jrc_cnt && !(char ) sql__setrang(outer_cond_found , 0 , 1))
        {
        outer_cond_found = (char ) sql__setrang((char ) sql__setrang(ptoc_Var268->jrc_joinarr[sql__setrang(j , 0 , 256)].jo_recs[0].jop_outer_join , 0 , 1) || (char ) sql__setrang(ptoc_Var268->jrc_joinarr[sql__setrang(j , 0 , 256)].jo_recs[1].jop_outer_join , 0 , 1) , 0 , 1);
        j = sql__succ(j , -2147483647 , 2147483647 , "vak65.p" , 21022);
        }
    _ak65outer_cond_found = (char ) sql__setrang(outer_cond_found , 0 , 1);
    cccprint("<ak65outer_cond_");
    return(_ak65outer_cond_found);
    }



char ak65subq_se(acv, first_node)
tak_all_command_glob *acv;
int *first_node;
    {
    char _ak65subq_se;
    char snot;
    tsp_int4 ix;
    tak_all_command_glob *ptoc_Var269;
    struct ptoc_Type56 *ptoc_Var270;
    cccprint("AK65  ak65subq_s");
    t01int4(5 , "fi_node in  " , *first_node);
    snot = 0;
    ptoc_Var269 = &*acv;
    ptoc_Var270 = &(*(tak_ap_max_tree *) sql__nil(ptoc_Var269->a_ap_tree , "vak65.p" , 21046))[sql__setrang(*first_node , 0 , 32767)];
    if ((int ) ptoc_Var270->n_symb == 53)
        {
        ix = (int ) ptoc_Var270->n_sa_level;
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var269->a_ap_tree , "vak65.p" , 21054))[sql__setrang(ix , 0 , 32767)].n_sa_level > 0)
            {
            ix = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var269->a_ap_tree , "vak65.p" , 21056))[sql__setrang(ix , 0 , 32767)].n_sa_level;
            }
        else
            {
            ix = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var269->a_ap_tree , "vak65.p" , 21058))[sql__setrang(ix , 0 , 32767)].n_lo_level;
            }
        while (ix > 0 && !(char ) sql__setrang(snot , 0 , 1))
            {
            snot = (char ) sql__setrang((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var269->a_ap_tree , "vak65.p" , 21062))[sql__setrang(ix , 0 , 32767)].n_proc == 82 , 0 , 1);
            if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var269->a_ap_tree , "vak65.p" , 21063))[sql__setrang(ix , 0 , 32767)].n_sa_level > 0)
                {
                ix = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var269->a_ap_tree , "vak65.p" , 21065))[sql__setrang(ix , 0 , 32767)].n_sa_level;
                }
            else
                {
                ix = (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var269->a_ap_tree , "vak65.p" , 21067))[sql__setrang(ix , 0 , 32767)].n_lo_level;
                }
            }
        if ((int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var269->a_ap_tree , "vak65.p" , 21071))[sql__setrang((int ) ptoc_Var270->n_sa_level , 0 , 32767)].n_symb == 54 || (int ) (*(tak_ap_max_tree *) sql__nil(ptoc_Var269->a_ap_tree , "vak65.p" , 21072))[sql__setrang((int ) ptoc_Var270->n_sa_level , 0 , 32767)].n_symb == 52 || (char ) sql__setrang(snot , 0 , 1))
            {
            snot = 1;
            *first_node = (int ) ptoc_Var270->n_sa_level;
            }
        }
    t01int4(5 , "fi_node out " , *first_node);
    t01int4(5 , "snot        " , sql__ord((int ) (char ) sql__setrang(snot , 0 , 1)));
    _ak65subq_se = (char ) sql__setrang(snot , 0 , 1);
    cccprint("<ak65subq_se    ");
    return(_ak65subq_se);
    }
