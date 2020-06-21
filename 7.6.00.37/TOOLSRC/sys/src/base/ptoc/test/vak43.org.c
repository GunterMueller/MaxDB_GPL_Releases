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
  unsigned char tsp00_C24[24];
typedef
  unsigned char tsp00_C40[40];
typedef
  unsigned char tsp00_C48[48];
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
  unsigned char tsp00_NodeId[64];
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
  unsigned char tsp_c10[10];
typedef
  tsp00_C12 tsp_c12;
typedef
  tsp00_C16 tsp_c16;
typedef
  unsigned char tsp_c18[18];
typedef
  tsp00_C20 tsp_c20;
typedef
  tsp00_C40 tsp_c40;
typedef
  unsigned char tsp_c50[50];
typedef
  unsigned char tsp_c64[64];
typedef
  unsigned char tsp_c80[80];
typedef
  unsigned char tsp_c120[120];
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
  unsigned char tsp_name[18];
typedef
  tsp00_NodeId tsp_nodeid;
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
  struct ptoc_Type12
    {
    tsp00_C6 ci6_gg00;
    } tgg91_PageRef;
typedef
  tgg90_Cint4 tgg91_SessionNo;
typedef
  struct ptoc_Type12 tgg91_TransNo;
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
  unsigned char tgg00_MessType;
typedef
  unsigned char tgg00_MessType2;
typedef
  unsigned char tgg00_PrivR;
typedef
  unsigned char tgg00_TableKind;
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
  unsigned char tgg00_SfpInitPart[2200];
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
  tgg91_PageRef tgg00_UserRef;
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
  tgg00_TwoKeys *tgg00_TwoKeysPtr;
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
        tsp00_SwapKind mswap_kind;
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
    tsp00_Int2 vpl_last;
    tsp00_Int2 vpl_last_fill1;
    tsp00_Int4 vpl_last_fill2;
    short vpl_pos_list[1024];
    } tgg00_VarColPosList;
typedef
  struct ptoc_Type49
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
  struct ptoc_Type50
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
  struct ptoc_Type51
    {
    tsp00_CodeType ctype;
    tsp00_CodeType char_default;
    unsigned char date_time;
    tsp00_SwapKind kernel_swap;
    } tgg_code_globals;
typedef
  tgg00_DataPartPtr tgg_datapart_ptr;
typedef
  struct ptoc_Type52
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
  tgg00_PrivR tgg_priv_r;
typedef
  unsigned char tgg_privilege_set[1];
typedef
  tgg00_QualBuf tgg_qual_buf;
typedef
  tgg00_QualBufPtr tgg_qual_buf_ptr;
typedef
  tgg00_Rec tgg_rec_buf;
typedef
  tgg00_SelectFieldsParam *tgg_select_param_ptr;
typedef
  struct ptoc_Type53
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
  struct ptoc_Type54
    {
    union  
      {
      struct  
        {
        tsp00_DbName svServerdb_gg00;
        tsp00_NodeId svServernode_gg00;
        } C_1;
      struct  
        {
        tsp00_DbName ggsv_serverdb;
        tsp00_NodeId ggsv_servernode;
        } C_2;
      } integer;
    } tgg_serverdb_ident;
typedef
  tgg00_ServerdbNo tgg_serverdb_no;
typedef
  tgg00_StackEntry tgg_stack_entry;
typedef
  tgg00_StackListPtr tgg_stack_list_ptr;
typedef
  struct ptoc_Type55
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
  tgg00_TableKind tgg_tablekind;
typedef
  struct ptoc_Type56
    {
    tgg00_FileId file_id;
    tgg00_FileId inv_id;
    } tgg_two_tree_ids;
typedef
  unsigned char tak00_colpropset[2];
typedef
  struct ptoc_Type57
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
  struct ptoc_Type58
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
  struct ptoc_Type59
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
  struct ptoc_Type60
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
  struct ptoc_Type61
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
  tsp_c18 tak_oldidentifier;
typedef
  unsigned char tak_keywordtab[533][18];
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
  struct ptoc_Type62
    {
    tsp_int4 rctxt_cnt;
    tsp00_C8 rctxt_roles[32768];
    } tak_role_context;
typedef
  tak_role_context *tak_role_context_ptr;
typedef
  unsigned char tak_map_set[768];
typedef
  struct ptoc_Type63
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
  struct ptoc_Type64
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
  struct ptoc_Type65
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
  struct ptoc_Type66
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
  struct ptoc_Type67
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
  struct ptoc_Type68
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
  struct ptoc_Type69
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
  struct ptoc_Type70
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
  struct ptoc_Type71
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
  struct ptoc_Type72
    {
    tak_sysbufferoffset rtablerefkey1;
    tsp_knl_identifier rtablerefkey2;
    tsp_c2 rsegmentid;
    char rsystable;
    char rfiller;
    tgg_tablekind rtablekind;
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
  struct ptoc_Type73
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
    tgg_tablekind btablekind;
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
    tgg_tablekind bv_tablekind;
    char bv_checkopt;
    char bv_qualexist;
    unsigned char bv_distinct;
    char bv_viewlist;
    char bdef_stamp;
    char bcorr_view;
    unsigned char bsqlmode;
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
  struct ptoc_Type74
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
  unsigned char tak_comment_type;
typedef
  tgg00_StackEntry tak_ak_constr_stack[989];
typedef
  struct ptoc_Type75
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
  struct ptoc_Type76
    {
    tsp_int2 com_rec_len;
    tsp_int2 com_key_len;
    tsp_int2 com_varcol_offset;
    tsp_int2 com_varcol_cnt;
    tgg_surrogate com_surrogate;
    tsp_c2 com_entrytype;
    tsp_c2 com_linkage;
    tak_comment_type com_type;
    tsp_c1 com_colno;
    tsp_c2 com_segmentid;
    tsp_int4 com_length;
    tak_comment_text com_text;
    } tak_comment_record;
typedef
  struct ptoc_Type77
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
  struct ptoc_Type78
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
    struct ptoc_Type79
      {
      tgg_surrogate ofp_procid;
      tsp_c3 ofp_language;
      unsigned char ofp_sqlmode[1];
      } fct_procids[64];
    } tak_funcrefrecord;
typedef
  struct ptoc_Type80
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
  struct ptoc_Type81
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
    struct ptoc_Type82
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
  struct ptoc_Type83
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
    struct ptoc_Type84
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
  struct ptoc_Type85
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
  struct ptoc_Type86
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
    struct ptoc_Type87
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
  struct ptoc_Type88
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
  struct ptoc_Type89
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
  struct ptoc_Type90
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
    struct ptoc_Type91
      {
      tgg_surrogate usa_tableid;
      tsp_int2 usa_filler;
      tgg_tablekind usa_tablekind;
      char usa_empty;
      } usagedef[672];
    } tak_usagerecord;
typedef
  struct ptoc_Type92
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
    struct ptoc_Type93
      {
      tgg_surrogate ut_surrogate;
      char ut_empty;
      unsigned char ut_kind;
      tsp_int2 ut_filler;
      } usertabdef[672];
    } tak_usertablerecord;
typedef
  unsigned char tak_userparams;
typedef
  int tak_userpar_arr[6];
typedef
  unsigned char tak_userrectype[1];
typedef
  struct ptoc_Type94
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
  struct ptoc_Type95
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
  struct ptoc_Type96
    {
    tak_sysbufferoffset ru_key_offset;
    tak_uref_restkey ru_rest_key;
    tsp_c2 ru_segmentid;
    tsp_int2 ru_filler;
    tgg_surrogate ru_user_id;
    } tak_userrefrecord;
typedef
  struct ptoc_Type97
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
  struct ptoc_Type98
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
  struct ptoc_Type99
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
  struct ptoc_Type100
    {
    tak_param_spec_ptr param_spec_ptr;
    tsp_int2 param_counter;
    tsp_int2 param_filler1;
    tsp_int4 param_filler2;
    } tak_param_list;
typedef
  struct ptoc_Type101
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
  struct ptoc_Type102
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
  struct ptoc_Type103
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
  struct ptoc_Type104
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
  struct ptoc_Type105
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
  struct ptoc_Type106
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
  struct ptoc_Type107
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
  struct ptoc_Type108
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
  struct ptoc_Type109
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
  struct ptoc_Type110
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
  struct ptoc_Type111
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
  struct ptoc_Type112
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
    tgg_tablekind syn_tablekind;
    char syncomment;
    tgg_surrogate syn_tableid;
    tsp_int4 syn_time;
    tsp_int4 syn_date;
    tgg_surrogate syn_owner;
    tgg_surrogate syn_authid;
    tsp_knl_identifier syn_tablen;
    } tak_synonymrecord;
typedef
  struct ptoc_Type113
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
    unsigned char j12operator;
    unsigned char j12fill1;
    tsp_int4 j12fill2;
    } tak_viewjoins[336];
typedef
  tgg00_StackEntry tak_view_stack_entries[1007];
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
    tgg_tablekind btablekind;
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
    tgg_tablekind bv_tablekind;
    char bv_checkopt;
    char bv_qualexist;
    unsigned char bv_distinct;
    char bv_viewlist;
    char bdef_stamp;
    char bcorr_view;
    unsigned char bsqlmode;
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
    unsigned char res_pars_sqlmode;
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
        unsigned char p_sqlmode;
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
    unsigned char scr_ddl;
    unsigned char scr_sql_mode;
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
  struct ptoc_Type153
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
    unsigned char o_ex_kind;
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
    unsigned char a_sqlmode;
    tsp_c4 a_trigger_trace_key;
    tsp_int2 a_restore_with_string;
    tsp_int2 a_outer_n;
    unsigned char a_session_sqlmode;
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
  struct ptoc_Type172
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
  struct ptoc_Type173
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
  short tgg07_ColPosArr[20];
typedef
  tsp00_C1 tgg07_StrRaw[236];
typedef
  struct ptoc_Type174
    {
    tgg07_ColPosArr skir_keystart;
    tgg07_ColPosArr skir_keystop;
    tsp00_Int2 skir_IN_SUBQ_stpos;
    tsp00_Int2 skir_inelemcnt;
    } tgg07_StrKeyInRange;
typedef
  unsigned char tgg07_InvColSet[2];
typedef
  struct ptoc_Type175
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
  struct ptoc_Type176
    {
    tgg07_ColPosArr sji_keystart;
    tgg07_ColPosArr sji_keystop;
    tsp00_PageNo sji_invroot;
    tsp00_Int2 sji_invindex;
    tsp00_Int2 sji_jfiller;
    } tgg07_StrJoinInv;
typedef
  struct ptoc_Type177
    {
    tgg07_ColPosArr sjmf_keystart;
    tgg07_ColPosArr sjmf_keystop;
    tsp00_PageNo sjmf_invroot;
    tsp00_Int2 sjmf_index_no;
    tsp00_Int2 sjmf_cntfields;
    } tgg07_StrJoinMultFields;
typedef
  struct ptoc_Type178
    {
    tsp00_Int2 sjv_lastkeylen;
    tsp00_Int2 sjv_minkeylen;
    } tgg07_StrJoinViewkey;
typedef
  struct ptoc_Type179
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
  struct ptoc_Type180
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
  struct ptoc_Type181
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
  struct ptoc_Type182
    {
    tsp_int2 shsl;
    tsp_int2 shkl;
    tsp_int2 shvarcol_offset;
    tsp_int2 shvarcol_cnt;
    tsp_c4 shkey;
    tak40_shinfo shinfo;
    } tak40_show_record;
typedef
  struct ptoc_Type183
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
  unsigned char t43_user_check;
tsp00_C8 a40sqlmode[4];
tsp00_C3 a40yes_no[2];
int a01char_size;
tgg_sysinfokey a01defaultkey;
tak_privilege a01emptypriv;
tsp_knl_identifier a01controluser;
tsp_knl_identifier a01_i_public;
tsp_knl_identifier a01_i_tablekey;
tsp_knl_identifier a01_i_temp;
tsp_knl_identifier a01_il_b_identifier;
tak_keywordtab a01kw;
tgg00_FileId b01niltree_id;
tgg_code_globals g01code;
tgg_kernel_globals g01glob;
tgg_serverdb_ident g01serverdb_ident;
int t01int4();
int t01surrogate();
char a722test_col_qual();
int a51build_userkey();
int a51cache_size_info();
int a51get_date_format();
int a40comment_descriptor();
int a40move();
int a40move_const();
int a40move_i4();
int a40put_date_time();
char a40usertab_next();
int a40get_catalog_table();
int a40_p_priv();
int a40ignore_down_error();
int a40init_table_scan();
char a40next_table();
int a40site();
char a23exist_sequence();
char a21is_owner();
char a19IsDefaultRole();
char a19user_knows_role();
int a10_key_del();
int a10get_sysinfo();
int a10next_sysinfo();
int a10rel_sysinfo();
int a07_b_put_error();
int a07ak_system_error();
int a06extcolno();
int a06inc_linkage();
int a06ignore_rsend_mess_buf();
char a06_table_exist();
int a06_get_priv();
char a06_role_excluded();
int a06determine_username();
int a06det_user_id();
int a06rsend_mess_buf();
int a06set_mtree();
int a06unpack_priv();
int a061get_colname();
int a01sets_identifier();
int b01destroy_file();
int b01empty_file();
int b07cadd_record();
int b07cnext_record();
int s20ch4sw();
int s20or4a();
int s20or4sw();
int s20sw_int4();
int s02encrypt();
int g02gettermsetdesc();
int g01datapart_init();
int g01mblock_init();
int g01qual_init();
int g04build_temp_tree_id();
char gg06IsNilRef();
int cccprint();
int a43execute();
int ak43build_control_userrec();
int ak43connect_param();
int ak43all_roles();
int ak43one_db_role();
int ak43roles();
int ak43session_roles();
int ak43one_role();
int ak43show_user();
int ak43all_user_show();
int ak43one_user_show();
int ak43connected_users();
int ak43add_connected_users();
int ak43perm_count();
int ak43privileges();
int ak43role_role_privileges();
int ak43role_privileges();
int ak43dependant_roles();
int ak43put_owner_privileges();
int ak43put_received_privileges();
int ak43put_sequence_privileges();
int ak43dbproc_privileges();
int ak43dbproc_granted_privileges();
int ak43granted_to();
int a43get_granted_priv();
int ak43spriv_granted_to_show();
int ak43put_one_priv();
int ak43one_col_priv();
int ak43table_kind();
char ak43unique();



int a43execute(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    cccprint("AK43  a43execute");
    switch ((int ) (*a41v).a4sh_kind)
        {
        case 4 :
            ak43connect_param(&(*acv) , &(*a41v));
            break;
        case 3 :
            ak43connected_users(&(*acv) , &(*a41v));
            break;
        case 20 :
            ak43granted_to(&(*acv) , &(*a41v));
            break;
        case 17 :
        case 79 :
        case 80 :
            ak43privileges(&(*acv) , &(*a41v));
            break;
        case 98 :
            ak43roles(&(*acv) , &(*a41v));
            break;
        case 99 :
            ak43all_roles(&(*acv) , &(*a41v));
            break;
        case 97 :
            ak43session_roles(&(*acv) , &(*a41v));
            break;
        case 54 :
        case 70 :
            ak43show_user(&(*acv) , &(*a41v));
            break;
        default :
            (unsigned char ) sql__caseerr("vak43.p" , 13671);
            break;
        }
    cccprint("<a43execute     ");
    }

static
  tak_userrectype ptoc_Var0 = 
  {
  (unsigned char)'\003'
  };


int ak43build_control_userrec(acv, a41v, userrec)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tak_userrecord *userrec;
    {
    tgg_basis_error b_err;
    tak_userparams pi;
    tak_sysbufferaddress sysbuf;
    tgg_sysinfokey sysk;
    tak_userrecord *ptoc_Var1;
    cccprint("AK43  ak43build_");
    ptoc_Var1 = &*userrec;
    _ptoc_ASSIGN(unsigned char , 8, ptoc_Var1->usurrogate, "\0\0\0\0\0\0\0\0")
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var1->username, a01controluser)
    _ptoc_MOVE(unsigned char , 1, ptoc_Var1->urecordtyp, ptoc_Var0);
    ptoc_Var1->userkind = 5;
    ptoc_Var1->usersysdba = 0;
    ptoc_Var1->uexclusive = 0;
    _ptoc_ASSIGN(unsigned char , 8, ptoc_Var1->usergroup_id, "\0\0\0\0\0\0\0\0")
    _ptoc_ASSIGN(unsigned char , 8, ptoc_Var1->userowner_id, g01glob.sysuser_id)
    sysk = a01defaultkey;
    _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_3.sauthid, ptoc_Var1->userowner_id)
    _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0B")
    a10get_sysinfo(&(*acv) , &sysk , 0 , &sysbuf , &b_err);
    if ((int ) b_err == 0)
        {
        ptoc_Var1->ucreatedate = (*(struct tak_systembuffer *) sql__nil(sysbuf , "vak43.p" , 13725)).integer.C_42.suser.ucreatedate;
        ptoc_Var1->ucreatetime = (*(struct tak_systembuffer *) sql__nil(sysbuf , "vak43.p" , 13726)).integer.C_42.suser.ucreatetime;
        ptoc_Var1->upwcreatedate = ptoc_Var1->ucreatedate;
        ptoc_Var1->upwcreatetime = ptoc_Var1->ucreatetime;
        ptoc_Var1->ualterdate = ptoc_Var1->ucreatedate;
        ptoc_Var1->ualtertime = ptoc_Var1->ucreatetime;
        }
    else
        {
        a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
        }
    pi = 0;
    do
        {
        ptoc_Var1->uparams[sql__setrang((int ) pi , 0 , 5)] = 2147483647;
        if (pi == 5)
            {
            break;
            }
        (unsigned char ) sql__setrang(pi += 1 , 0 , 5);
        }
    while (!0);
    ptoc_Var1->ucomment = 0;
    (*a41v).a4sysbuf = (struct tak_systembuffer *) &*userrec;
    cccprint("<ak43build_contr");
    }



int ak43connect_param(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    char ok;
    struct ptoc_Type184
      {
      union  
        {
        struct  
          {
          tsp00_C4 c4;
          } C_true;
        struct  
          {
          tsp_int4 i4;
          } C_false;
        } boolean;
      } abs_session;
    tsp_knl_identifier termcharname;
    tsp_c120 format_buf;
    tak_all_command_glob *ptoc_Var2;
    tak40_show_glob *ptoc_Var3;
    int ptoc_Var4;
    tak40_show_glob *ptoc_Var5;
    tak_resultrecord *ptoc_Var6;
    cccprint("AK43  ak43connec");
    ptoc_Var2 = &*acv;
    ptoc_Var3 = &*a41v;
    ptoc_Var3->a4sh_kind = 4;
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var2->a_return_segm , "vak43.p" , 13774)).integer.C_4.sp1r_returncode == 0 && (int ) ptoc_Var2->a_ex_kind != 0)
        {
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40sqlmode[sql__setrang((int ) ptoc_Var2->a_sqlmode , 0 , 3)] , sizeof(tsp00_C8 ));
        a40move_i4(&(*acv) , &(*a41v) , (int ) ptoc_Var2->a_iso_level , 0 , 0 , 0);
        a40move_i4(&(*acv) , &(*a41v) , (int ) ptoc_Var2->a_user_timeout , 0 , 0 , 0);
        _ptoc_ASSIGN(unsigned char , 64, termcharname, a01_il_b_identifier)
        g02gettermsetdesc(termcharname , &ptoc_Var2->a_mess_code , &ptoc_Var2->a_basic_code , &ok);
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) termcharname , sizeof(unsigned char [64]));
        _ptoc_ASSIGN(unsigned char , 4, abs_session.boolean.C_true.c4, (*acv).a_transinf.tri_trans.trSessionId_gg00.ci4_gg00)
        s20sw_int4(&abs_session.boolean.C_false.i4 , 1 , (unsigned char ) sql__setrang(g01code.kernel_swap , 0 , 3));
        abs_session.boolean.C_false.i4 = (ptoc_Var4 = abs_session.boolean.C_false.i4 , ptoc_Var4 >= 0 ? ptoc_Var4 : -ptoc_Var4);
        a40move_i4(&(*acv) , &(*a41v) , abs_session.boolean.C_false.i4 , 0 , 0 , 0);
        ptoc_Var5 = &*a41v;
        ptoc_Var6 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var5->a4p_arr.pinfop , "vak43.p" , 13793)).integer.C_28.sresult;
        a51get_date_format(&(*acv) , format_buf , (char ) sql__setrang((int ) (*(tak00_columninfo *) sql__nil(ptoc_Var6->integer.C_2.bcolumn[sql__setrang(ptoc_Var5->a4index , 1 , 1536) + -1] , "vak43.p" , 13795)).integer.C_1.cdatatyp == 24 , 0 , 1));
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &format_buf[1] , a01char_size * 50);
        }
    cccprint("<ak43connect_par");
    }



int ak43all_roles(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tgg_basis_error b_err;
    tak_sysbufferaddress role_buf;
    tak_sysbufferaddress priv_buf;
    tgg_sysinfokey privkey;
    tgg_sysinfokey sysk;
    cccprint("AK43  ak43all_ro");
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak43.p" , 13823)).integer.C_4.sp1r_returncode == 0)
        {
        sysk = a01defaultkey;
        sysk.integer.C_3.sauthid[0] = (unsigned char)'\377';
        sysk.integer.C_3.sauthid[1] = (unsigned char)'\372';
        do
            {
            a10next_sysinfo(&(*acv) , &sysk , 2 , 0 , "\0B" , &role_buf , &b_err);
            if ((int ) b_err == 0)
                {
                ak43one_db_role(&(*acv) , &(*a41v) , role_buf , (*(struct tak_systembuffer *) sql__nil(role_buf , "vak43.p" , 13835)).integer.C_42.suser.userowner_id);
                _ptoc_ASSIGN(unsigned char , 8, privkey.integer.C_2.stableid, (*(struct tak_systembuffer *) sql__nil(role_buf , "vak43.p" , 13836)).integer.C_50.syskey.integer.C_3.sauthid)
                _ptoc_ASSIGN(unsigned char , 2, privkey.integer.C_2.sentrytyp, "\0\007")
                privkey.skeylen = (short ) sql__setrang((short ) (sizeof(tsp00_C8 ) + sizeof(tsp00_C2 )) , -32768 , 32767);
                do
                    {
                    a10next_sysinfo(&(*acv) , &privkey , sizeof(tsp00_C8 ) + sizeof(tsp00_C2 ) , 1 , "\0\007" , &priv_buf , &b_err);
                    if ((int ) b_err == 0)
                        {
                        ak43one_db_role(&(*acv) , &(*a41v) , role_buf , (*(struct tak_systembuffer *) sql__nil(priv_buf , "vak43.p" , 13846)).integer.C_25.sprivuser.pru_grantee);
                        }
                    }
                while (!((int ) b_err != 0));
                if ((int ) b_err == 1420)
                    {
                    b_err = 0;
                    }
                a10rel_sysinfo(role_buf);
                }
            }
        while (!((int ) b_err != 0));
        if ((int ) b_err != 1420)
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
            }
        }
    cccprint("<ak43all_roles  ");
    }



int ak43one_db_role(acv, a41v, role_buf, ptoc_ptr_granteeId)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tak_sysbufferaddress role_buf;
tgg_surrogate ptoc_ptr_granteeId;
    {
    tsp00_KnlIdentifier owner;
    tsp00_KnlIdentifier grantee;
    tgg_surrogate granteeId;
    _ptoc_MOVE(char, sizeof(tgg_surrogate ), granteeId, ptoc_ptr_granteeId);
    cccprint("AK43  ak43one_db");
    a06determine_username(&(*acv) , (*(struct tak_systembuffer *) sql__nil(role_buf , "vak43.p" , 13884)).integer.C_42.suser.userowner_id , owner);
    a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) owner , sizeof(unsigned char [64]));
    a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (*(struct tak_systembuffer *) sql__nil(role_buf , "vak43.p" , 13886)).integer.C_42.suser.username , sizeof(unsigned char [64]));
    a06determine_username(&(*acv) , granteeId , grantee);
    a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) grantee , sizeof(unsigned char [64]));
    a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[sql__setrang((int ) a19IsDefaultRole(&(*acv) , granteeId , (*(struct tak_systembuffer *) sql__nil(role_buf , "vak43.p" , 13892)).integer.C_50.syskey.integer.C_3.sauthid) , 0 , 1)] , sizeof(tsp00_C3 ));
    cccprint("<ak43one_db_role");
    }



int ak43roles(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tgg_basis_error b_err;
    tak_sysbufferaddress role_buf;
    tsp_name pw;
    tsp_cryptpw cryptpw;
    tsp_knl_identifier owner;
    tgg_sysinfokey sysk;
    cccprint("AK43  ak43roles ");
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak43.p" , 13914)).integer.C_4.sp1r_returncode == 0)
        {
        sysk = a01defaultkey;
        sysk.integer.C_3.sauthid[0] = (unsigned char)'\377';
        sysk.integer.C_3.sauthid[1] = (unsigned char)'\372';
        do
            {
            a10next_sysinfo(&(*acv) , &sysk , 2 , 1 , "\0B" , &role_buf , &b_err);
            if ((int ) b_err == 0)
                {
                a06determine_username(&(*acv) , (*(struct tak_systembuffer *) sql__nil(role_buf , "vak43.p" , 13927)).integer.C_42.suser.userowner_id , owner);
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) owner , sizeof(unsigned char [64]));
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (*(struct tak_systembuffer *) sql__nil(role_buf , "vak43.p" , 13929)).integer.C_42.suser.username , sizeof(unsigned char [64]));
                _ptoc_ASSIGN(unsigned char , 18, pw, "                  ")
                s02encrypt(pw , cryptpw);
                a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[sql__setrang((int ) (_ptoc_MEMCMP(unsigned char , 24, cryptpw , (*(struct tak_systembuffer *) sql__nil(role_buf , "vak43.p" , 13934)).integer.C_42.suser.userpassw) == 0 ? 0 : 1) , 0 , 1)] , sizeof(tsp00_C3 ));
                a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[sql__setrang((int ) a19user_knows_role(&(*acv) , (*acv).a_curr_user_id , (*(struct tak_systembuffer *) sql__nil(role_buf , "vak43.p" , 13938)).integer.C_50.syskey.integer.C_3.sauthid) , 0 , 1)] , sizeof(tsp00_C3 ));
                a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a40yes_no[sql__setrang((int ) a19IsDefaultRole(&(*acv) , (*acv).a_curr_user_id , (*(struct tak_systembuffer *) sql__nil(role_buf , "vak43.p" , 13942)).integer.C_50.syskey.integer.C_3.sauthid) , 0 , 1)] , sizeof(tsp00_C3 ));
                a40put_date_time(&(*acv) , &(*a41v) , &(*(struct tak_systembuffer *) sql__nil(role_buf , "vak43.p" , 13943)).integer.C_42.suser.ucreatedate , 1 , 0);
                a40put_date_time(&(*acv) , &(*a41v) , &(*(struct tak_systembuffer *) sql__nil(role_buf , "vak43.p" , 13945)).integer.C_42.suser.ucreatetime , 0 , 0);
                }
            }
        while (!((int ) b_err != 0));
        if ((int ) b_err != 1420)
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
            }
        }
    cccprint("<ak43roles      ");
    }



int ak43session_roles(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tgg_basis_error b_err;
    int ix;
    tak_sysbufferaddress roletab_ptr;
    tgg_sysinfokey rolekey;
    int ptoc_Var7;
    tak_usertablerecord *ptoc_Var8;
    struct ptoc_Type93 *ptoc_Var9;
    cccprint("AK43  ak43sessio");
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak43.p" , 13977)).integer.C_4.sp1r_returncode == 0 && (int ) (*acv).a_ex_kind != 0)
        {
        if ((ptoc_Var7 = (int ) (*acv).a_role_info) == 0 || ptoc_Var7 == 2)
            {
            rolekey = a01defaultkey;
            _ptoc_ASSIGN(unsigned char , 8, rolekey.integer.C_3.sauthid, (*acv).a_curr_user_id)
            _ptoc_ASSIGN(unsigned char , 2, rolekey.integer.C_2.sentrytyp, "\0h")
            do
                {
                a10get_sysinfo(&(*acv) , &rolekey , 0 , &roletab_ptr , &b_err);
                if ((int ) b_err == 0)
                    {
                    ptoc_Var8 = &(*(struct tak_systembuffer *) sql__nil(roletab_ptr , "vak43.p" , 13991)).integer.C_45.susertab;
                    ix = 1;
                    while (ix <= (int ) ptoc_Var8->usertabcount)
                        {
                        ptoc_Var9 = &ptoc_Var8->usertabdef[sql__setrang(ix , 1 , 672) + -1];
                        if (!(char ) sql__setrang(ptoc_Var9->ut_empty , 0 , 1) && !a06_role_excluded(&(*acv) , ptoc_Var9->ut_surrogate))
                            {
                            ak43one_role(&(*acv) , &(*a41v) , ptoc_Var9->ut_surrogate);
                            }
                        ix = ix + 1;
                        }
                    if ((char ) sql__setrang(ptoc_Var8->usertabnext_exist , 0 , 1))
                        {
                        a06inc_linkage(rolekey.integer.C_2.slinkage);
                        }
                    else
                        {
                        b_err = 1930;
                        }
                    }
                if ((int ) b_err == 1930 && (_ptoc_MEMCMP(unsigned char , 8, rolekey.integer.C_3.sauthid , "\0\0\0\0\0\0\0\002") == 0 ? 0 : 1))
                    {
                    b_err = 0;
                    _ptoc_ASSIGN(unsigned char , 8, rolekey.integer.C_3.sauthid, "\0\0\0\0\0\0\0\002")
                    _ptoc_ASSIGN(unsigned char , 2, rolekey.integer.C_2.slinkage, "\0\001")
                    }
                }
            while (!((int ) b_err != 0));
            if ((int ) b_err != 1930)
                {
                a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                }
            }
        else
            {
            if ((*acv).a_role_ptr != (tak_role_context *) (int *) 0)
                {
                ix = 1;
                while (ix <= (*(tak_role_context *) sql__nil((*acv).a_role_ptr , "vak43.p" , 14037)).rctxt_cnt)
                    {
                    ak43one_role(&(*acv) , &(*a41v) , (*(tak_role_context *) sql__nil((*acv).a_role_ptr , "vak43.p" , 14039)).rctxt_roles[sql__setrang(ix , 1 , 32768) + -1]);
                    ix = ix + 1;
                    }
                }
            }
        b01empty_file(&(*acv).a_transinf.tri_trans , &(*acv).a_into_tree);
        }
    cccprint("<ak43session_rol");
    }



int ak43one_role(acv, a41v, role_id)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tgg_surrogate role_id;
    {
    tgg_basis_error b_err;
    tak_sysbufferaddress sysbuf;
    tgg_sysinfokey sysk;
    cccprint("AK43  ak43one_ro");
    if (ak43unique(&(*acv) , 0 , role_id , role_id))
        {
        sysk = a01defaultkey;
        _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_3.sauthid, role_id)
        _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0B")
        a10get_sysinfo(&(*acv) , &sysk , 1 , &sysbuf , &b_err);
        if ((int ) b_err == 0)
            {
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (*(struct tak_systembuffer *) sql__nil(sysbuf , "vak43.p" , 14077)).integer.C_42.suser.username , sizeof(unsigned char [64]));
            }
        else
            {
            if ((int ) b_err != 1930)
                {
                a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                }
            }
        }
    cccprint("<ak43one_role   ");
    }



int ak43show_user(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    t43_user_check user_check;
    tak_all_command_glob *ptoc_Var10;
    tak40_show_glob *ptoc_Var11;
    cccprint("AK43  ak43show_u");
    ptoc_Var10 = &*acv;
    ptoc_Var11 = &*a41v;
    ptoc_Var11->a4sh_kind = 54;
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var10->a_return_segm , "vak43.p" , 14105)).integer.C_4.sp1r_returncode == 0 && (int ) ptoc_Var10->a_ex_kind != 0)
        {
        user_check = 1;
        if (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var11->a4authname , a01_il_b_identifier) == 0 ? 1 : 0)
            {
            ak43all_user_show(&(*acv) , &(*a41v) , (unsigned char ) sql__setrang(user_check , 0 , 2));
            }
        else
            {
            ak43one_user_show(&(*acv) , &(*a41v) , 0);
            }
        }
    cccprint("<ak43show_user  ");
    }



int ak43all_user_show(acv, a41v, user_check)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
t43_user_check user_check;
    {
    tgg_basis_error b_err;
    tgg_sysinfokey sysk;
    tak_all_command_glob *ptoc_Var12;
    tak40_show_glob *ptoc_Var13;
    tgg00_SysInfoKey *ptoc_Var14;
    cccprint("AK43  ak43all_us");
    ptoc_Var12 = &*acv;
    ptoc_Var13 = &*a41v;
    sysk = a01defaultkey;
    ptoc_Var14 = &sysk;
    _ptoc_ASSIGN(unsigned char , 8, ptoc_Var14->integer.C_3.sauthid, "\0\0\0\0\0\0\0\0")
    ptoc_Var14->integer.C_3.sauthid[0] = (unsigned char)'\377';
    ptoc_Var14->integer.C_3.sauthid[1] = (unsigned char)'\377';
    _ptoc_ASSIGN(unsigned char , 2, ptoc_Var14->integer.C_2.sentrytyp, "\0B")
    b_err = 0;
    while ((int ) b_err == 0 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var12->a_return_segm , "vak43.p" , 14147)).integer.C_4.sp1r_returncode == 0)
        {
        a10next_sysinfo(&(*acv) , &sysk , 2 , 1 , "\0B" , &ptoc_Var13->a4sysbuf , &b_err);
        if ((int ) b_err == 0)
            {
            _ptoc_ASSIGN(unsigned char , 64, ptoc_Var13->a4authname, (*(struct tak_systembuffer *) sql__nil(ptoc_Var13->a4sysbuf , "vak43.p" , 14154)).integer.C_42.suser.username)
            ak43one_user_show(&(*acv) , &(*a41v) , (unsigned char ) sql__setrang(user_check , 0 , 2));
            }
        }
    if ((int ) b_err != 1420)
        {
        a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
        }
    else
        {
        ptoc_Var13->a4sysbuf = (struct tak_systembuffer *) (int *) 0;
        _ptoc_ASSIGN(unsigned char , 64, ptoc_Var13->a4authname, a01controluser)
        ak43one_user_show(&(*acv) , &(*a41v) , (unsigned char ) sql__setrang(user_check , 0 , 2));
        }
    cccprint("<ak43all_user_sh");
    }



int ak43one_user_show(acv, a41v, user_check)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
t43_user_check user_check;
    {
    char full_info;
    char show_curr_user;
    tak_userparams user_param;
    tgg_basis_error b_err;
    int ix;
    int mult;
    tsp_int4 int_value;
    tak_sysbufferaddress owner_buf;
    tsp_c8 uv;
    tsp_knl_identifier group_name;
    tsp_knl_identifier owner_name;
    tgg_sysinfokey owner_key;
    tak_userrecord control_userrec;
    tak_all_command_glob *ptoc_Var15;
    tak40_show_glob *ptoc_Var16;
    tak_userrecord *ptoc_Var17;
    tak_userrecord *ptoc_Var18;
    int ptoc_Var19;
    int ptoc_Var20;
    cccprint("AK43  ak43one_us");
    ptoc_Var15 = &*acv;
    ptoc_Var16 = &*a41v;
    b_err = 0;
    if (ptoc_Var16->a4sysbuf == (struct tak_systembuffer *) (int *) 0)
        {
        if (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var16->a4authname , a01controluser) == 0 ? 1 : 0)
            {
            ak43build_control_userrec(&(*acv) , &(*a41v) , &control_userrec);
            }
        else
            {
            a51build_userkey(ptoc_Var16->a4authname , &ptoc_Var16->a4sysk);
            a10get_sysinfo(&(*acv) , &ptoc_Var16->a4sysk , 1 , &ptoc_Var16->a4sysbuf , &b_err);
            if ((int ) b_err == 0)
                {
                _ptoc_ASSIGN(unsigned char , 8, ptoc_Var16->a4sysk.integer.C_3.sauthid, (*(struct tak_systembuffer *) sql__nil(ptoc_Var16->a4sysbuf , "vak43.p" , 14213)).integer.C_44.suserref.ru_user_id)
                }
            if ((int ) b_err == 0)
                {
                _ptoc_ASSIGN(unsigned char , 2, ptoc_Var16->a4sysk.integer.C_2.sentrytyp, "\0B")
                ptoc_Var16->a4sysk.skeylen = 12;
                a10get_sysinfo(&(*acv) , &ptoc_Var16->a4sysk , 0 , &ptoc_Var16->a4sysbuf , &b_err);
                if ((int ) b_err != 0)
                    {
                    a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                    }
                }
            }
        }
    if ((int ) b_err == 0 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var15->a_return_segm , "vak43.p" , 14230)).integer.C_4.sp1r_returncode == 0)
        {
        t01surrogate(5 , "user owner  " , (*(struct tak_systembuffer *) sql__nil(ptoc_Var16->a4sysbuf , "vak43.p" , 14234)).integer.C_42.suser.userowner_id);
        t01surrogate(5 , "curr user   " , ptoc_Var15->a_curr_user_id);
        t01surrogate(5 , "curr owner  " , ptoc_Var15->a_acc_dba_id);
        show_curr_user = (char ) sql__setrang((int ) ptoc_Var16->a4sh_kind == 70 , 0 , 1);
        if (!(char ) sql__setrang(show_curr_user , 0 , 1))
            {
            if ((int ) ptoc_Var15->a_current_user_kind == 4 || (int ) ptoc_Var15->a_current_user_kind == 1)
                {
                show_curr_user = (char ) sql__setrang((int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var16->a4sysbuf , "vak43.p" , 14247)).integer.C_42.suser.userkind == 4 || (int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var16->a4sysbuf , "vak43.p" , 14248)).integer.C_42.suser.userkind == 1 || (_ptoc_MEMCMP(unsigned char , 8, (*(struct tak_systembuffer *) sql__nil(ptoc_Var16->a4sysbuf , "vak43.p" , 14249)).integer.C_42.suser.userowner_id , ptoc_Var15->a_acc_dba_id) == 0 ? 1 : 0) || (_ptoc_MEMCMP(unsigned char , 8, (*(struct tak_systembuffer *) sql__nil(ptoc_Var16->a4sysbuf , "vak43.p" , 14250)).integer.C_42.suser.userowner_id , ptoc_Var15->a_curr_user_id) == 0 ? 1 : 0) || (int ) user_check != 2 , 0 , 1);
                }
            else
                {
                show_curr_user = (char ) sql__setrang((_ptoc_MEMCMP(unsigned char , 8, (*(struct tak_systembuffer *) sql__nil(ptoc_Var16->a4sysbuf , "vak43.p" , 14258)).integer.C_50.syskey.integer.C_3.sauthid , ptoc_Var15->a_acc_dba_id) == 0 ? 1 : 0) || (_ptoc_MEMCMP(unsigned char , 8, (*(struct tak_systembuffer *) sql__nil(ptoc_Var16->a4sysbuf , "vak43.p" , 14261)).integer.C_42.suser.userowner_id , ptoc_Var15->a_acc_dba_id) == 0 ? 1 : 0) && (int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var16->a4sysbuf , "vak43.p" , 14262)).integer.C_42.suser.userkind != 4 && (int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var16->a4sysbuf , "vak43.p" , 14263)).integer.C_42.suser.userkind != 1 || (int ) user_check == 0 , 0 , 1);
                }
            }
        if ((char ) sql__setrang(show_curr_user , 0 , 1) && (0 || ((*(struct tak_systembuffer *) sql__nil(ptoc_Var16->a4sysbuf , "vak43.p" , 14269)).integer.C_42.suser.urecordtyp[0] & 4) == 0))
            {
            if ((_ptoc_MEMCMP(unsigned char , 64, ptoc_Var16->a4authname , ptoc_Var15->a_acc_user) == 0 ? 1 : 0) || (int ) ptoc_Var16->a4sh_kind == 70)
                {
                full_info = 1;
                }
            else
                {
                full_info = (char ) sql__setrang(a21is_owner(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(ptoc_Var16->a4sysbuf , "vak43.p" , 14276)).integer.C_42.suser) , 0 , 1);
                }
            ptoc_Var17 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var16->a4sysbuf , "vak43.p" , 14278)).integer.C_42.suser;
            if (0 || ((*(struct tak_systembuffer *) sql__nil(ptoc_Var16->a4sysbuf , "vak43.p" , 14282)).integer.C_42.suser.urecordtyp[0] & 1) == 0)
                {
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a01_il_b_identifier , sizeof(unsigned char [64]));
                }
            else
                {
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) ptoc_Var16->a4authname , sizeof(unsigned char [64]));
                }
            if (0 || ((*(struct tak_systembuffer *) sql__nil(ptoc_Var16->a4sysbuf , "vak43.p" , 14292)).integer.C_42.suser.urecordtyp[0] & 1) == 0)
                {
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) ptoc_Var16->a4authname , sizeof(unsigned char [64]));
                }
            else
                {
                if (0 || (ptoc_Var17->urecordtyp[0] & 2) == 0)
                    {
                    a06determine_username(&(*acv) , ptoc_Var17->usergroup_id , group_name);
                    }
                else
                    {
                    _ptoc_ASSIGN(unsigned char , 64, group_name, a01_il_b_identifier)
                    }
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) group_name , sizeof(unsigned char [64]));
                }
            a06determine_username(&(*acv) , ptoc_Var17->userowner_id , owner_name);
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) owner_name , sizeof(unsigned char [64]));
            if ((char ) sql__setrang(ptoc_Var16->a4strategy.tgg07_StratEnum.C_strat_catalog.str_catalog.scat_col_needed[3] , 0 , 1))
                {
                owner_key = a01defaultkey;
                _ptoc_ASSIGN(unsigned char , 8, owner_key.integer.C_3.sauthid, ptoc_Var17->userowner_id)
                _ptoc_ASSIGN(unsigned char , 2, owner_key.integer.C_2.sentrytyp, "\0B")
                a10get_sysinfo(&(*acv) , &owner_key , 1 , &owner_buf , &b_err);
                if ((int ) b_err == 0)
                    {
                    ptoc_Var18 = &(*(struct tak_systembuffer *) sql__nil(owner_buf , "vak43.p" , 14326)).integer.C_42.suser;
                    a06determine_username(&(*acv) , ptoc_Var18->userowner_id , owner_name);
                    }
                else
                    {
                    a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                    }
                if ((char ) sql__setrang(full_info , 0 , 1))
                    {
                    a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) owner_name , sizeof(unsigned char [64]));
                    }
                else
                    {
                    a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
                    }
                }
            else
                {
                ptoc_Var16->a4pos = ptoc_Var16->a4pos + 1;
                }
            switch ((int ) ptoc_Var17->userkind)
                {
                case 1 :
                    if ((char ) sql__setrang(ptoc_Var17->usersysdba , 0 , 1))
                        {
                        _ptoc_ASSIGN(unsigned char , 8, uv, "SYSDBA  ")
                        }
                    else
                        {
                        _ptoc_ASSIGN(unsigned char , 8, uv, "DBA     ")
                        }
                    break;
                case 2 :
                    _ptoc_ASSIGN(unsigned char , 8, uv, "RESOURCE")
                    break;
                case 5 :
                    _ptoc_ASSIGN(unsigned char , 8, uv, "ADMIN   ")
                    break;
                default :
                    _ptoc_ASSIGN(unsigned char , 8, uv, "STANDARD")
                    break;
                }
            a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) uv , sizeof(tsp00_C8 ));
            if ((char ) sql__setrang(full_info , 0 , 1))
                {
                if ((char ) sql__setrang(ptoc_Var17->uexclusive , 0 , 1))
                    {
                    _ptoc_ASSIGN(unsigned char , 8, uv, "SINGLE  ")
                    }
                else
                    {
                    _ptoc_ASSIGN(unsigned char , 8, uv, "MULTIPLE")
                    }
                a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) uv , sizeof(tsp00_C8 ));
                }
            else
                {
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
                }
            user_param = 0;
            do
                {
                if ((int ) user_param > 1)
                    {
                    a40move_i4(&(*acv) , &(*a41v) , ptoc_Var17->uparams[sql__setrang((int ) user_param , 0 , 5)] , (char ) sql__setrang(!(char ) sql__setrang(full_info , 0 , 1) || ptoc_Var17->uparams[sql__setrang((int ) user_param , 0 , 5)] == 2147483647 , 0 , 1) , 0 , 0);
                    }
                if ((int ) ptoc_Var16->a4sh_kind == 47)
                    {
                    if ((int ) user_param == 0)
                        {
                        a40move_i4(&(*acv) , &(*a41v) , ak43perm_count(&(*acv)) , 0 , 0 , 0);
                        }
                    else
                        {
                        if ((int ) user_param == 1)
                            {
                            a40move_i4(&(*acv) , &(*a41v) , ptoc_Var15->a_transinf.tri_trans.trTempCount_gg00 , 0 , 0 , 0);
                            }
                        }
                    }
                if (user_param == 4)
                    {
                    break;
                    }
                (unsigned char ) sql__setrang(user_param += 1 , 0 , 5);
                }
            while (!0);
            int_value = 0;
            mult = 1;
            ptoc_Var19 = sizeof(tsp00_C8 );
            ptoc_Var20 = sizeof(tsp00_C8 ) + -3;
            if (ptoc_Var19 >= ptoc_Var20)
                {
                ix = ptoc_Var19;
                do
                    {
                    int_value = int_value + sql__ord((int ) ptoc_Var17->usurrogate[sql__setrang(ix , 1 , 8) + -1]) * mult;
                    mult = mult * 256;
                    if (ix == ptoc_Var20)
                        {
                        break;
                        }
                    ix -= 1;
                    }
                while (!0);
                }
            a40move_i4(&(*acv) , &(*a41v) , int_value , 0 , 0 , 0);
            a40put_date_time(&(*acv) , &(*a41v) , &ptoc_Var17->ucreatedate , 1 , 0);
            a40put_date_time(&(*acv) , &(*a41v) , &ptoc_Var17->ucreatetime , 0 , 0);
            a40put_date_time(&(*acv) , &(*a41v) , &ptoc_Var17->ualterdate , 1 , 0);
            a40put_date_time(&(*acv) , &(*a41v) , &ptoc_Var17->ualtertime , 0 , 0);
            a40put_date_time(&(*acv) , &(*a41v) , &ptoc_Var17->upwcreatedate , 1 , (char ) sql__setrang((int ) ptoc_Var17->userkind == 4 || !(char ) sql__setrang(full_info , 0 , 1) , 0 , 1));
            a40put_date_time(&(*acv) , &(*a41v) , &ptoc_Var17->upwcreatetime , 0 , (char ) sql__setrang((int ) ptoc_Var17->userkind == 4 || !(char ) sql__setrang(full_info , 0 , 1) , 0 , 1));
            a40site(&(*acv) , &(*a41v) , g01serverdb_ident.integer.C_1.svServerdb_gg00 , g01serverdb_ident.integer.C_1.svServernode_gg00);
            a40comment_descriptor(&(*acv) , &(*a41v) , (char ) sql__setrang(ptoc_Var17->ucomment , 0 , 1) , ptoc_Var17->usurrogate , 1 , 13);
            a10_key_del(&(*acv) , &ptoc_Var16->a4sysk);
            }
        }
    cccprint("<ak43one_user_sh");
    }

static
  unsigned char ptoc_Var21[1] = 
  {
  (unsigned char)'\020'
  };


int ak43connected_users(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tgg_basis_error b_err;
    tgg00_FileId aux_file;
    tak_sm_description sm_desc;
    tgg_mess_block mblock;
    tsp_buf data;
    tgg_qual_buf qual;
    tak_all_command_glob *ptoc_Var22;
    tak40_show_glob *ptoc_Var23;
    unsigned char ptoc_Var24[1];
    unsigned char ptoc_Var25[1];
    unsigned char ptoc_Var26[1];
    tak_sm_description *ptoc_Var27;
    cccprint("AK43  ak43connec");
    ptoc_Var22 = &*acv;
    ptoc_Var23 = &*a41v;
    if (((int ) ptoc_Var22->a_current_user_kind == 3 || (int ) ptoc_Var22->a_current_user_kind == 2) && (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var22->a_curr_user_name , a01controluser) == 0 ? 0 : 1))
        {
        a07_b_put_error(&(*acv) , 6350 , 1);
        }
    ptoc_Var23->a4sh_kind = 3;
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var22->a_return_segm , "vak43.p" , 14480)).integer.C_4.sp1r_returncode == 0 && (int ) ptoc_Var22->a_ex_kind != 0)
        {
        g04build_temp_tree_id(&aux_file , &ptoc_Var22->a_transinf.tri_trans);
        aux_file.integer.C_8.fileTempCnt_gg00 = 1;
        aux_file.integer.C_8.fileTfnTemp_gg00 = 17;
        _ptoc_MOVE(unsigned char , 1, aux_file.integer.C_1.fileType_gg00, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var24 , aux_file.integer.C_1.fileType_gg00 , ptoc_Var21 , sizeof(unsigned char [1])));
        g01mblock_init(&ptoc_Var22->a_transinf.tri_trans , 63 , 66 , &mblock);
        g01datapart_init(&mblock , (tgg00_DataPart *) data , sizeof(tsp00_Buf ));
        mblock.integer.C_2.mb_struct = 1;
        (*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak43.p" , 14491)).integer.C_1.mbp_buf[0] = (unsigned char ) sql__setrang((unsigned char ) sql__ord((int ) g01code.ctype) , 0 , 255);
        mblock.integer.C_2.mb_data_len = 1;
        g01qual_init(&mblock , (tgg00_QualBuf *) &qual , sizeof(tgg00_QualBuf ));
        a06set_mtree(&(*acv) , &mblock , &aux_file);
        mblock.integer.C_2.mb_qual_len = sizeof(tgg00_FileId );
        ptoc_Var27 = &sm_desc;
        ptoc_Var27->sm_error_cnt = 0;
        ptoc_Var27->sm_errorlist[0] = 4210;
        ptoc_Var27->sm_errorlist[1] = 4215;
        ptoc_Var27->sm_return_req = 0;
        ptoc_Var27->sm_must_local = 1;
        ptoc_Var27->sm_not_local = 0;
        a06ignore_rsend_mess_buf(&(*acv) , &mblock , &sm_desc , &b_err);
        if ((int ) b_err != 0)
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
            }
        else
            {
            ak43add_connected_users(&(*acv) , &(*a41v) , &aux_file , 1);
            b01destroy_file(&ptoc_Var22->a_transinf.tri_trans , &aux_file);
            }
        }
    cccprint("<ak43connected_u");
    }



int ak43add_connected_users(acv, a41v, tree, local_users)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tgg00_FileId *tree;
char local_users;
    {
    tgg_basis_error b_err;
    tsp_int_map_c2 rec_len;
    tsp_int_map_c2 key_len;
    int pos;
    tgg91_SessionNo session;
    tsp_int4 int_session;
    tsp_int4 ak_cache;
    tsp_moveobj_ptr b_ptr;
    tgg_bd_set_result_record set_result;
    tgg00_FilePos tree_pos;
    tgg_lkey zerokey;
    unsigned char b[8192];
    tak_all_command_glob *ptoc_Var28;
    tak40_show_glob *ptoc_Var29;
    tgg00_BdSetResultRecord *ptoc_Var30;
    int ptoc_Var31;
    cccprint("AK43  ak43add_co");
    ptoc_Var28 = &*acv;
    ptoc_Var29 = &*a41v;
    b_ptr = (tsp00_MoveObj *) b;
    ptoc_Var30 = &set_result;
    ptoc_Var30->bd_key_check_len = 0;
    ptoc_Var30->bd_max_rec_cnt = 32767;
    ptoc_Var30->bd_max_fill_len = sizeof(unsigned char [8192]);
    ptoc_Var30->bd_next = 0;
    zerokey.integer.C_11.len = 0;
    tree_pos.tpsPno_gg00 = 2147483647;
    do
        {
        b07cnext_record(&ptoc_Var28->a_transinf.tri_trans , &(*tree) , &zerokey , &set_result , &tree_pos , sql__nil(b_ptr , "vak43.p" , 14562));
        b_err = (short ) sql__setrang(ptoc_Var28->a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
        if ((int ) b_err == 0 || (int ) b_err == 1200 || (int ) b_err == 350)
            {
            pos = 1;
            while (pos < set_result.bd_fill_len)
                {
                rec_len.boolean.C_false.map_c2[0] = (unsigned char ) sql__setrang(b[sql__setrang(pos , 1 , 8192) + -1] , 0 , 255);
                rec_len.boolean.C_false.map_c2[1] = (unsigned char ) sql__setrang(b[sql__setrang(pos + 1 , 1 , 8192) + -1] , 0 , 255);
                key_len.boolean.C_false.map_c2[0] = (unsigned char ) sql__setrang(b[sql__setrang(pos + 2 , 1 , 8192) + -1] , 0 , 255);
                key_len.boolean.C_false.map_c2[1] = (unsigned char ) sql__setrang(b[sql__setrang(pos + 3 , 1 , 8192) + -1] , 0 , 255);
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &b[sql__setrang(pos + (int ) key_len.boolean.C_true.map_int + 9 , 1 , 8192) + -1] , sizeof(unsigned char [64]));
                a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &b[sql__setrang(pos + (int ) key_len.boolean.C_true.map_int + sizeof(unsigned char [64]) + 10 , 1 , 8192) + -1] , 18);
                int_session = (ptoc_Var31 = s20or4sw(sql__nil(b_ptr , "vak43.p" , 14582) , pos + (int ) rec_len.boolean.C_true.map_int + -6 , 1 , (unsigned char ) sql__setrang(g01code.kernel_swap , 0 , 3)) , ptoc_Var31 >= 0 ? ptoc_Var31 : -ptoc_Var31);
                if ((char ) sql__setrang(local_users , 0 , 1))
                    {
                    s20ch4sw(int_session , (unsigned char ) sql__setrang(g01code.kernel_swap , 0 , 3) , session.ci4_gg00 , 1 , 1);
                    a51cache_size_info(&(*acv) , session , &ak_cache);
                    }
                else
                    {
                    ak_cache = 2147483647;
                    }
                a40move_i4(&(*acv) , &(*a41v) , int_session , 0 , 0 , 0);
                a40move_i4(&(*acv) , &(*a41v) , ak_cache , (char ) sql__setrang(ak_cache == 2147483647 , 0 , 1) , 0 , 0);
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
    while (!((int ) b_err != 0 || (int ) (*(tsp1_segment *) sql__nil(ptoc_Var28->a_return_segm , "vak43.p" , 14612)).integer.C_4.sp1r_returncode != 0));
    cccprint("<ak43add_connect");
    }



int ak43perm_count(acv)
tak_all_command_glob *acv;
    {
    int _ak43perm_count;
    tgg_basis_error b_err;
    tsp_int4 perm_count;
    tgg00_FileId aux_tree;
    tgg00_FileId qual;
    tgg_mess_block mblock;
    cccprint("AK43  ak43perm_c");
    perm_count = 2147483647;
    if (!gg06IsNilRef(&(*acv).a_user_perm_ref))
        {
        aux_tree = b01niltree_id;
        aux_tree.integer.C_1.fileUserRef_gg00 = (*acv).a_user_perm_ref;
        g01mblock_init(&(*acv).a_transinf.tri_trans , 63 , 47 , &mblock);
        g01qual_init(&mblock , (tgg00_QualBuf *) &qual , sizeof(tgg00_FileId ));
        a06set_mtree(&(*acv) , &mblock , &aux_tree);
        mblock.integer.C_2.mb_qual_len = sizeof(tgg00_FileId );
        mblock.integer.C_2.mb_struct = 11;
        a06rsend_mess_buf(&(*acv) , &mblock , 1 , &b_err);
        if ((int ) b_err == 0)
            {
            perm_count = s20or4a((*(tgg00_QualBuf *) sql__nil(mblock.integer.C_4.mb_qual , "vak43.p" , 14650)).integer.C_1.buf , 5);
            }
        else
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 0);
            }
        }
    _ak43perm_count = perm_count;
    cccprint("<ak43perm_count ");
    return(_ak43perm_count);
    }



int ak43privileges(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    char full_spec;
    char is_private;
    char ok;
    char use_tables;
    char use_base;
    char use_view;
    char use_sequences;
    char use_dbprocs;
    char use_grantee;
    char use_grantor;
    char use_public;
    char use_roles;
    tsp_knl_identifier value;
    tak40_show_glob *ptoc_Var32;
    cccprint("AK43  ak43privil");
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak43.p" , 14694)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var32 = &*a41v;
        a01sets_identifier(value , "TABLE             ");
        use_base = (char ) sql__setrang(a722test_col_qual(&(*acv) , &(*a41v) , "TYPE              " , (unsigned char)' ' , (tsp00_MoveObj *) value , 10) , 0 , 1);
        a01sets_identifier(value , "VIEW              ");
        use_view = (char ) sql__setrang(a722test_col_qual(&(*acv) , &(*a41v) , "TYPE              " , (unsigned char)' ' , (tsp00_MoveObj *) value , 8) , 0 , 1);
        use_tables = (char ) sql__setrang((char ) sql__setrang(use_base , 0 , 1) || (char ) sql__setrang(use_view , 0 , 1) , 0 , 1);
        a01sets_identifier(value , "SEQUENCE          ");
        use_sequences = (char ) sql__setrang(a722test_col_qual(&(*acv) , &(*a41v) , "TYPE              " , (unsigned char)' ' , (tsp00_MoveObj *) value , 16) , 0 , 1);
        a01sets_identifier(value , "DBPROC            ");
        use_dbprocs = (char ) sql__setrang(a722test_col_qual(&(*acv) , &(*a41v) , "TYPE              " , (unsigned char)' ' , (tsp00_MoveObj *) value , 12) , 0 , 1);
        a01sets_identifier(value , "ROLE              ");
        use_roles = (char ) sql__setrang(a722test_col_qual(&(*acv) , &(*a41v) , "TYPE              " , (unsigned char)' ' , (tsp00_MoveObj *) value , 12) , 0 , 1);
        _ptoc_ASSIGN(unsigned char , 64, value, (*acv).a_curr_user_name)
        use_grantee = (char ) sql__setrang(a722test_col_qual(&(*acv) , &(*a41v) , "GRANTEE           " , (unsigned char)' ' , (tsp00_MoveObj *) value , sizeof(unsigned char [64])) , 0 , 1);
        use_grantor = (char ) sql__setrang(a722test_col_qual(&(*acv) , &(*a41v) , "GRANTOR           " , (unsigned char)' ' , (tsp00_MoveObj *) value , sizeof(unsigned char [64])) , 0 , 1);
        _ptoc_ASSIGN(unsigned char , 64, value, a01_i_public)
        use_public = (char ) sql__setrang(a722test_col_qual(&(*acv) , &(*a41v) , "GRANTEE           " , (unsigned char)' ' , (tsp00_MoveObj *) value , 12) , 0 , 1);
        full_spec = (char ) sql__setrang((_ptoc_MEMCMP(unsigned char , 64, ptoc_Var32->a4authname , a01_il_b_identifier) == 0 ? 0 : 1) && (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var32->a4tablen , a01_il_b_identifier) == 0 ? 0 : 1) , 0 , 1);
        if ((char ) sql__setrang(use_tables , 0 , 1) || (char ) sql__setrang(use_sequences , 0 , 1))
            {
            if (!(char ) sql__setrang(full_spec , 0 , 1))
                {
                a40init_table_scan(&(*acv) , &(*a41v) , 1 , 1 , 1 , 1 , 0 , 1);
                ptoc_Var32->a4use_base = (char ) sql__setrang(use_base , 0 , 1);
                ptoc_Var32->a4use_view = (char ) sql__setrang(use_view , 0 , 1);
                }
            do
                {
                if ((char ) sql__setrang(full_spec , 0 , 1))
                    {
                    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var32->a4next_owner, ptoc_Var32->a4authname)
                    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var32->a4next_tablen, ptoc_Var32->a4tablen)
                    ptoc_Var32->a4temp = (char ) sql__setrang(_ptoc_MEMCMP(unsigned char , 64, ptoc_Var32->a4next_owner , a01_i_temp) == 0 ? 1 : 0 , 0 , 1);
                    ptoc_Var32->a4private = (char ) sql__setrang(_ptoc_MEMCMP(unsigned char , 64, ptoc_Var32->a4next_owner , (*acv).a_curr_user_name) == 0 ? 1 : 0 , 0 , 1);
                    ptoc_Var32->a4nonprivate = (char ) sql__setrang(!(char ) sql__setrang(ptoc_Var32->a4private , 0 , 1) , 0 , 1);
                    ok = 0;
                    if ((char ) sql__setrang(use_tables , 0 , 1))
                        {
                        use_tables = 0;
                        ok = (char ) sql__setrang(a06_table_exist(&(*acv) , 0 , ptoc_Var32->a4next_owner , ptoc_Var32->a4next_tablen , &ptoc_Var32->a4p_arr , 1) , 0 , 1);
                        }
                    if (!(char ) sql__setrang(ok , 0 , 1) && (char ) sql__setrang(use_sequences , 0 , 1))
                        {
                        use_sequences = 0;
                        ok = (char ) sql__setrang(a23exist_sequence(&(*acv) , -2 , &ptoc_Var32->a4p_arr.pbasep , ptoc_Var32->a4next_owner , ptoc_Var32->a4next_tablen) , 0 , 1);
                        }
                    }
                else
                    {
                    ok = (char ) sql__setrang(a40next_table(&(*acv) , &(*a41v)) , 0 , 1);
                    }
                if ((char ) sql__setrang(ok , 0 , 1))
                    {
                    if (_ptoc_MEMCMP(unsigned char , 2, (*(struct tak_systembuffer *) sql__nil(ptoc_Var32->a4p_arr.pbasep , "vak43.p" , 14780)).integer.C_50.syskey.integer.C_2.sentrytyp , "\0R") == 0 ? 1 : 0)
                        {
                        if ((char ) sql__setrang(use_sequences , 0 , 1))
                            {
                            ak43put_sequence_privileges(&(*acv) , &(*a41v));
                            }
                        }
                    else
                        {
                        is_private = (char ) sql__setrang((char ) sql__setrang(ptoc_Var32->a4private , 0 , 1) || (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var32->a4next_owner , (*acv).a_curr_user_name) == 0 ? 1 : 0) , 0 , 1);
                        if ((char ) sql__setrang(ptoc_Var32->a4temp , 0 , 1) || (char ) sql__setrang(is_private , 0 , 1))
                            {
                            if ((char ) sql__setrang(use_grantor , 0 , 1) && (char ) sql__setrang(use_grantee , 0 , 1))
                                {
                                ak43put_owner_privileges(&(*acv) , &(*a41v));
                                }
                            if ((char ) sql__setrang(is_private , 0 , 1))
                                {
                                a43get_granted_priv(&(*acv) , &(*a41v) , ptoc_Var32->a4next_owner , ptoc_Var32->a4next_tablen);
                                }
                            }
                        else
                            {
                            ak43put_received_privileges(&(*acv) , &(*a41v) , (char ) sql__setrang(!(char ) sql__setrang(ptoc_Var32->a4nonprivate , 0 , 1) && !(char ) sql__setrang(ptoc_Var32->a4public , 0 , 1) , 0 , 1) , (char ) sql__setrang(use_grantee , 0 , 1) , (char ) sql__setrang(use_public , 0 , 1));
                            }
                        }
                    }
                }
            while (!(!(char ) sql__setrang(ok , 0 , 1) || (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak43.p" , 14813)).integer.C_4.sp1r_returncode != 0));
            }
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak43.p" , 14817)).integer.C_4.sp1r_returncode == 0 && (char ) sql__setrang(use_dbprocs , 0 , 1))
            {
            ak43dbproc_privileges(&(*acv) , &(*a41v));
            }
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak43.p" , 14821)).integer.C_4.sp1r_returncode == 0 && (char ) sql__setrang(use_roles , 0 , 1))
            {
            ak43role_role_privileges(&(*acv) , &(*a41v));
            }
        }
    cccprint("<ak43privileges ");
    }



int ak43role_role_privileges(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tgg_basis_error b_err;
    tak_sysbufferaddress roletab_ptr;
    int ix;
    tgg_sysinfokey sysk;
    tak_usertablerecord *ptoc_Var33;
    int ptoc_Var34;
    cccprint("AK43  ak43role_r");
    sysk = a01defaultkey;
    _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_3.sauthid, (*acv).a_curr_user_id)
    _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0h")
    do
        {
        a10get_sysinfo(&(*acv) , &sysk , 0 , &roletab_ptr , &b_err);
        if ((int ) b_err == 0)
            {
            ptoc_Var33 = &(*(struct tak_systembuffer *) sql__nil(roletab_ptr , "vak43.p" , 14853)).integer.C_45.susertab;
            ptoc_Var34 = ptoc_Var33->usertabcount;
            if (1 <= ptoc_Var34)
                {
                ix = 1;
                do
                    {
                    if (!(char ) sql__setrang(ptoc_Var33->usertabdef[sql__setrang(ix , 1 , 672) + -1].ut_empty , 0 , 1))
                        {
                        ak43role_privileges(&(*acv) , &(*a41v) , ptoc_Var33->usertabdef[sql__setrang(ix , 1 , 672) + -1].ut_surrogate);
                        }
                    if (ix == ptoc_Var34)
                        {
                        break;
                        }
                    ix += 1;
                    }
                while (!0);
                }
            ptoc_Var34 = ptoc_Var33->usertabcount;
            if (1 <= ptoc_Var34)
                {
                ix = 1;
                do
                    {
                    if (!(char ) sql__setrang(ptoc_Var33->usertabdef[sql__setrang(ix , 1 , 672) + -1].ut_empty , 0 , 1))
                        {
                        ak43dependant_roles(&(*acv) , &(*a41v) , ptoc_Var33->usertabdef[sql__setrang(ix , 1 , 672) + -1].ut_surrogate);
                        }
                    if (ix == ptoc_Var34)
                        {
                        break;
                        }
                    ix += 1;
                    }
                while (!0);
                }
            a10rel_sysinfo(roletab_ptr);
            if ((char ) sql__setrang(ptoc_Var33->usertabnext_exist , 0 , 1))
                {
                a06inc_linkage(sysk.integer.C_2.slinkage);
                }
            else
                {
                b_err = 1930;
                }
            }
        }
    while (!((int ) b_err != 0));
    if ((int ) b_err != 1930)
        {
        a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
        }
    b01empty_file(&(*acv).a_transinf.tri_trans , &(*acv).a_into_tree);
    cccprint("<ak43role_role_p");
    }

static
  unsigned char ptoc_Var35[1] = 
  {
  (unsigned char)'\001'
  };


int ak43role_privileges(acv, a41v, role_id)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tgg_surrogate role_id;
    {
    char first;
    tgg_basis_error b_err;
    tak_sysbufferaddress priv_p;
    tak_sysbufferaddress role_p;
    tsp_knl_identifier role_owner;
    tsp_knl_identifier role;
    tak_privilege priv;
    tgg_sysinfokey rolekey;
    tgg_sysinfokey sysk;
    cccprint("AK43  ak43role_p");
    if (ak43unique(&(*acv) , 0 , role_id , role_id))
        {
        first = 1;
        sysk = a01defaultkey;
        _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_3.sauthid, role_id)
        _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0\007")
        do
            {
            a10next_sysinfo(&(*acv) , &sysk , 10 , 1 , "\0\007" , &priv_p , &b_err);
            if ((int ) b_err == 0)
                {
                if ((char ) sql__setrang(first , 0 , 1))
                    {
                    first = 0;
                    priv = a01emptypriv;
                    _ptoc_MOVE(unsigned char , 1, priv.integer.C_1.priv_all_set, ptoc_Var35);
                    _ptoc_ASSIGN(unsigned char , 8, rolekey.integer.C_3.sauthid, role_id)
                    _ptoc_ASSIGN(unsigned char , 2, rolekey.integer.C_2.sentrytyp, "\0B")
                    _ptoc_ASSIGN(unsigned char , 2, rolekey.integer.C_2.slinkage, "\0\001")
                    rolekey.skeylen = 12;
                    a10get_sysinfo(&(*acv) , &rolekey , 1 , &role_p , &b_err);
                    if ((int ) b_err == 0)
                        {
                        _ptoc_ASSIGN(unsigned char , 64, role, (*(struct tak_systembuffer *) sql__nil(role_p , "vak43.p" , 14938)).integer.C_42.suser.username)
                        a06determine_username(&(*acv) , (*(struct tak_systembuffer *) sql__nil(role_p , "vak43.p" , 14940)).integer.C_42.suser.userowner_id , role_owner);
                        }
                    }
                }
            if ((int ) b_err == 0)
                {
                ak43spriv_granted_to_show(&(*acv) , &(*a41v) , role_owner , role , 391 , &priv , (*(struct tak_systembuffer *) sql__nil(priv_p , "vak43.p" , 14950)).integer.C_50.syskey.integer.C_2.suserid , (*(struct tak_systembuffer *) sql__nil(priv_p , "vak43.p" , 14950)).integer.C_50.syskey.integer.C_2.sgrantuser , (*(struct tak_systembuffer *) sql__nil(priv_p , "vak43.p" , 14951)).integer.C_25.sprivuser.pru_date , (*(struct tak_systembuffer *) sql__nil(priv_p , "vak43.p" , 14951)).integer.C_25.sprivuser.pru_time);
                }
            }
        while (!((int ) b_err != 0));
        if ((int ) b_err != 1420)
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
            }
        }
    cccprint("<ak43role_privil");
    }



int ak43dependant_roles(acv, a41v, ptoc_ptr_role_id)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tgg_surrogate ptoc_ptr_role_id;
    {
    tgg_basis_error b_err;
    tak_sysbufferaddress roletab_ptr;
    int ix;
    tgg_sysinfokey sysk;
    tak_usertablerecord *ptoc_Var36;
    tgg_surrogate role_id;
    _ptoc_MOVE(char, sizeof(tgg_surrogate ), role_id, ptoc_ptr_role_id);
    cccprint("AK43  ak43depend");
    sysk = a01defaultkey;
    _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_3.sauthid, role_id)
    _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0h")
    ix = 0;
    do
        {
        a10get_sysinfo(&(*acv) , &sysk , 0 , &roletab_ptr , &b_err);
        if ((int ) b_err == 0)
            {
            ptoc_Var36 = &(*(struct tak_systembuffer *) sql__nil(roletab_ptr , "vak43.p" , 14989)).integer.C_45.susertab;
            ix = ix + 1;
            if (ix > (int ) ptoc_Var36->usertabcount)
                {
                a10rel_sysinfo(roletab_ptr);
                if ((char ) sql__setrang(ptoc_Var36->usertabnext_exist , 0 , 1))
                    {
                    a06inc_linkage(sysk.integer.C_2.slinkage);
                    }
                else
                    {
                    b_err = 1930;
                    }
                }
            else
                {
                if (!(char ) sql__setrang(ptoc_Var36->usertabdef[sql__setrang(ix , 1 , 672) + -1].ut_empty , 0 , 1))
                    {
                    ak43role_privileges(&(*acv) , &(*a41v) , ptoc_Var36->usertabdef[sql__setrang(ix , 1 , 672) + -1].ut_surrogate);
                    a10rel_sysinfo(roletab_ptr);
                    ak43dependant_roles(&(*acv) , &(*a41v) , ptoc_Var36->usertabdef[sql__setrang(ix , 1 , 672) + -1].ut_surrogate);
                    }
                }
            }
        }
    while (!((int ) b_err != 0));
    if ((int ) b_err != 1930)
        {
        a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
        }
    cccprint("<ak43dependant_r");
    }



int ak43put_owner_privileges(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tak_privilege priv;
    tak_all_command_glob *ptoc_Var37;
    tak40_show_glob *ptoc_Var38;
    cccprint("AK43  ak43put_ow");
    ptoc_Var37 = &*acv;
    ptoc_Var38 = &*a41v;
    a06_get_priv(&(*acv) , &ptoc_Var38->a4p_arr.pbasep , &priv);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var37->a_return_segm , "vak43.p" , 15043)).integer.C_4.sp1r_returncode == 0)
        {
        ak43spriv_granted_to_show(&(*acv) , &(*a41v) , ptoc_Var37->a_curr_user_name , sql__nil((*(struct tak_systembuffer *) sql__nil(ptoc_Var38->a4p_arr.pbasep , "vak43.p" , 15046)).integer.C_1.sbase.btablen , "vak43.p" , 15046) , ak43table_kind(&(*(struct tak_systembuffer *) sql__nil(ptoc_Var38->a4p_arr.pbasep , "vak43.p" , 15047)).integer.C_1.sbase) , &priv , ptoc_Var37->a_curr_user_id , ptoc_Var37->a_curr_user_id , (*(struct tak_systembuffer *) sql__nil(ptoc_Var38->a4p_arr.pbasep , "vak43.p" , 15049)).integer.C_1.sbase.bdatecreate , (*(struct tak_systembuffer *) sql__nil(ptoc_Var38->a4p_arr.pbasep , "vak43.p" , 15050)).integer.C_1.sbase.btimecreate);
        }
    cccprint("<ak43put_owner_p");
    }

static
  unsigned char ptoc_Var39[1] = 
  {
  (unsigned char)'\0'
  };
static
  tak_columnset ptoc_Var40 = 
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
  tak_columnset ptoc_Var41 = 
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


int ak43put_received_privileges(acv, a41v, is_role, use_grantee, use_public)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
char is_role;
char use_grantee;
char use_public;
    {
    char public_loop;
    char put_granted;
    int tab_kind;
    tgg_basis_error b_err;
    tak_sysbufferaddress priv_p;
    tgg_sysinfokey sysk;
    tak_privilege priv;
    tak_all_command_glob *ptoc_Var42;
    tak40_show_glob *ptoc_Var43;
    unsigned char ptoc_Var44[1];
    tak_columnset ptoc_Var45;
    cccprint("AK43  ak43put_re");
    ptoc_Var42 = &*acv;
    ptoc_Var43 = &*a41v;
    t01int4(5 , "use_grantee " , sql__ord((int ) (char ) sql__setrang(use_grantee , 0 , 1)));
    t01int4(5 , "use_public  " , sql__ord((int ) (char ) sql__setrang(use_public , 0 , 1)));
    public_loop = (char ) sql__setrang(!(char ) sql__setrang(use_grantee , 0 , 1) && !(char ) sql__setrang(is_role , 0 , 1) , 0 , 1);
    put_granted = 0;
    tab_kind = ak43table_kind(&(*(struct tak_systembuffer *) sql__nil(ptoc_Var43->a4p_arr.pbasep , "vak43.p" , 15084)).integer.C_1.sbase);
    do
        {
        sysk = (*(struct tak_systembuffer *) sql__nil((*a41v).a4p_arr.pbasep , "vak43.p" , 15088)).integer.C_50.syskey;
        _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0\007")
        if ((char ) sql__setrang(public_loop , 0 , 1))
            {
            _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_2.suserid, "\0\0\0\0\0\0\0\002")
            }
        else
            {
            if ((char ) sql__setrang(is_role , 0 , 1))
                {
                _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_2.suserid, ptoc_Var43->a4next_key.integer.C_3.sauthid)
                }
            else
                {
                _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_2.suserid, (*acv).a_curr_user_id)
                }
            }
        sysk.skeylen = (short ) sql__setrang((short ) ((int ) sysk.skeylen + 8) , -32768 , 32767);
        do
            {
            a10next_sysinfo(&(*acv) , &sysk , 20 , 1 , "\0\007" , &priv_p , &b_err);
            if ((int ) b_err != 0)
                {
                if ((int ) b_err != 1420)
                    {
                    a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                    a40ignore_down_error(&(*acv));
                    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var42->a_return_segm , "vak43.p" , 15114)).integer.C_4.sp1r_returncode == 0)
                        {
                        b_err = -1;
                        }
                    }
                }
            else
                {
                priv = a01emptypriv;
                a06unpack_priv(&(*(struct tak_systembuffer *) sql__nil(priv_p , "vak43.p" , 15124)).integer.C_25.sprivuser.pru_priv , &priv);
                ak43spriv_granted_to_show(&(*acv) , &(*a41v) , ptoc_Var43->a4next_owner , ptoc_Var43->a4next_tablen , tab_kind , &priv , (*(struct tak_systembuffer *) sql__nil(priv_p , "vak43.p" , 15127)).integer.C_50.syskey.integer.C_2.suserid , (*(struct tak_systembuffer *) sql__nil(priv_p , "vak43.p" , 15127)).integer.C_50.syskey.integer.C_2.sgrantuser , (*(struct tak_systembuffer *) sql__nil(priv_p , "vak43.p" , 15128)).integer.C_25.sprivuser.pru_date , (*(struct tak_systembuffer *) sql__nil(priv_p , "vak43.p" , 15129)).integer.C_25.sprivuser.pru_time);
                if (!(char ) sql__setrang(public_loop , 0 , 1) && sql__nre(sizeof(unsigned char [1]) , priv.integer.C_1.priv_all_grant_set , ptoc_Var39) || sql__nre(sizeof(tak_columnset ) , priv.integer.C_1.priv_grant_sel_set , ptoc_Var40) || sql__nre(sizeof(tak_columnset ) , priv.integer.C_1.priv_grant_upd_set , ptoc_Var41))
                    {
                    put_granted = 1;
                    }
                }
            }
        while (!((int ) b_err != 0));
        if ((int ) b_err == 1420 && !(char ) sql__setrang(public_loop , 0 , 1) && (char ) sql__setrang(use_public , 0 , 1))
            {
            b_err = 0;
            public_loop = 1;
            }
        }
    while (!((int ) b_err != 0));
    if ((int ) b_err != 1420 && (int ) b_err != -1)
        {
        a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
        }
    else
        {
        if ((char ) sql__setrang(put_granted , 0 , 1))
            {
            a43get_granted_priv(&(*acv) , &(*a41v) , ptoc_Var43->a4next_owner , ptoc_Var43->a4next_tablen);
            }
        }
    cccprint("<ak43put_receive");
    }

static
  unsigned char ptoc_Var46[1] = 
  {
  (unsigned char)'\001'
  };
static
  unsigned char ptoc_Var47[1] = 
  {
  (unsigned char)'\001'
  };
static
  unsigned char ptoc_Var48[1] = 
  {
  (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var49[1] = 
  {
  (unsigned char)'\001'
  };
static
  unsigned char ptoc_Var50[1] = 
  {
  (unsigned char)'\001'
  };


int ak43put_sequence_privileges(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    char exit_loop;
    int repeat_cnt;
    tgg_basis_error b_err;
    tak_sysbufferaddress priv_p;
    tak_privilege priv;
    tgg_surrogate grantee_id;
    tsp_knl_identifier seq_owner_name;
    tgg_sysinfokey sysk;
    tak_all_command_glob *ptoc_Var51;
    tak40_show_glob *ptoc_Var52;
    tak_sequencerecord *ptoc_Var53;
    tak_proc_seq_privilege *ptoc_Var54;
    cccprint("AK43  ak43put_se");
    ptoc_Var51 = &*acv;
    ptoc_Var52 = &*a41v;
    ptoc_Var53 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var52->a4p_arr.pbasep , "vak43.p" , 15193)).integer.C_30.ssequence;
    priv = a01emptypriv;
    if (_ptoc_MEMCMP(unsigned char , 8, ptoc_Var53->seq_owner , ptoc_Var51->a_curr_user_id) == 0 ? 1 : 0)
        {
        _ptoc_MOVE(unsigned char , 1, priv.integer.C_1.priv_all_set, ptoc_Var46);
        _ptoc_MOVE(unsigned char , 1, priv.integer.C_1.priv_all_grant_set, ptoc_Var47);
        ak43spriv_granted_to_show(&(*acv) , &(*a41v) , ptoc_Var51->a_curr_user_name , ptoc_Var53->seq_name , 414 , &priv , ptoc_Var51->a_curr_user_id , ptoc_Var51->a_curr_user_id , ptoc_Var53->seq_date , ptoc_Var53->seq_time);
        _ptoc_MOVE(unsigned char , 1, priv.integer.C_1.priv_all_grant_set, ptoc_Var48);
        sysk = a01defaultkey;
        _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_2.stableid, ptoc_Var53->seq_surrogate)
        _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0S")
        do
            {
            a10next_sysinfo(&(*acv) , &sysk , 8 , 1 , "\0S" , &priv_p , &b_err);
            if ((int ) b_err == 0)
                {
                _ptoc_MOVE(unsigned char , 1, priv.integer.C_1.priv_all_set, ptoc_Var49);
                ptoc_Var54 = &(*(struct tak_systembuffer *) sql__nil(priv_p , "vak43.p" , 15218)).integer.C_55.sprocseqpriv;
                ak43spriv_granted_to_show(&(*acv) , &(*a41v) , ptoc_Var51->a_curr_user_name , ptoc_Var53->seq_name , 414 , &priv , sysk.integer.C_2.suserid , ptoc_Var51->a_curr_user_id , ptoc_Var54->psp_date , ptoc_Var54->psp_time);
                }
            }
        while (!((int ) b_err != 0 || (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak43.p" , 15227)).integer.C_4.sp1r_returncode != 0));
        if ((int ) b_err != 1420)
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
            }
        }
    else
        {
        a06determine_username(&(*acv) , ptoc_Var53->seq_owner , seq_owner_name);
        repeat_cnt = 0;
        exit_loop = 0;
        do
            {
            repeat_cnt = repeat_cnt + 1;
            if ((int ) ptoc_Var52->a4next_key.integer.C_3.sauthid[0] == 255 && (int ) ptoc_Var52->a4next_key.integer.C_3.sauthid[1] == 250)
                {
                exit_loop = 1;
                _ptoc_ASSIGN(unsigned char , 8, grantee_id, ptoc_Var52->a4next_key.integer.C_3.sauthid)
                }
            else
                {
                if ((char ) sql__setrang(ptoc_Var52->a4nonprivate , 0 , 1) && repeat_cnt == 1)
                    {
                    _ptoc_ASSIGN(unsigned char , 8, grantee_id, ptoc_Var51->a_curr_user_id)
                    }
                else
                    {
                    if (repeat_cnt <= 2 && (char ) sql__setrang(ptoc_Var53->seq_public , 0 , 1))
                        {
                        repeat_cnt = 2;
                        _ptoc_ASSIGN(unsigned char , 8, grantee_id, "\0\0\0\0\0\0\0\002")
                        }
                    else
                        {
                        repeat_cnt = 3;
                        }
                    }
                }
            if (repeat_cnt != 3)
                {
                sysk = a01defaultkey;
                _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_2.stableid, ptoc_Var53->seq_surrogate)
                _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0S")
                _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_2.suserid, grantee_id)
                sysk.skeylen = (short ) sql__setrang((short ) ((int ) sysk.skeylen + 8) , -32768 , 32767);
                a10get_sysinfo(&(*acv) , &sysk , 1 , &priv_p , &b_err);
                if ((int ) b_err == 0)
                    {
                    _ptoc_MOVE(unsigned char , 1, priv.integer.C_1.priv_all_set, ptoc_Var50);
                    ak43spriv_granted_to_show(&(*acv) , &(*a41v) , seq_owner_name , ptoc_Var53->seq_name , 414 , &priv , grantee_id , ptoc_Var53->seq_owner , (*(struct tak_systembuffer *) sql__nil(priv_p , "vak43.p" , 15284)).integer.C_55.sprocseqpriv.psp_date , (*(struct tak_systembuffer *) sql__nil(priv_p , "vak43.p" , 15285)).integer.C_55.sprocseqpriv.psp_time);
                    }
                else
                    {
                    a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                    }
                }
            }
        while (!(repeat_cnt == 3 || (char ) sql__setrang(exit_loop , 0 , 1)));
        }
    cccprint("<ak43put_sequenc");
    }

static
  unsigned char ptoc_Var55[1] = 
  {
  (unsigned char)'\001'
  };
static
  unsigned char ptoc_Var56[1] = 
  {
  (unsigned char)'\001'
  };
static
  unsigned char ptoc_Var57[1] = 
  {
  (unsigned char)'\0'
  };


int ak43dbproc_privileges(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    char init_unique;
    tgg_basis_error b_err;
    tgg_tablekind dummy;
    int repeat_cnt;
    tak_sysbufferaddress proc_p;
    tak_sysbufferaddress priv_p;
    tgg_surrogate grantee_id;
    tgg_surrogate proc_id;
    tsp_knl_identifier owner_name;
    tak_privilege priv;
    tgg_sysinfokey sysk;
    tgg_sysinfokey mei_sysk;
    tak_all_command_glob *ptoc_Var58;
    tak40_show_glob *ptoc_Var59;
    tak_method_inforecord *ptoc_Var60;
    cccprint("AK43  ak43dbproc");
    ptoc_Var58 = &*acv;
    ptoc_Var59 = &*a41v;
    b_err = 0;
    priv = a01emptypriv;
    _ptoc_MOVE(unsigned char , 1, priv.integer.C_1.priv_all_set, ptoc_Var55);
    if (a722test_col_qual(&(*acv) , &(*a41v) , "OWNER             " , (unsigned char)' ' , (tsp00_MoveObj *) ptoc_Var58->a_curr_user_name , sizeof(unsigned char [64])))
        {
        sysk = a01defaultkey;
        _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_3.sauthid, ptoc_Var58->a_curr_user_id)
        _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0p")
        do
            {
            a10next_sysinfo(&(*acv) , &sysk , 10 , 1 , "\0p" , &proc_p , &b_err);
            if ((int ) b_err == 0)
                {
                if ((int ) (*(struct tak_systembuffer *) sql__nil(proc_p , "vak43.p" , 15347)).integer.C_67.smethodref.mrf_type == 1)
                    {
                    mei_sysk = a01defaultkey;
                    _ptoc_ASSIGN(unsigned char , 8, mei_sysk.integer.C_2.stableid, (*(struct tak_systembuffer *) sql__nil(proc_p , "vak43.p" , 15351)).integer.C_67.smethodref.mrf_method_id)
                    _ptoc_ASSIGN(unsigned char , 2, mei_sysk.integer.C_2.sentrytyp, "\0r")
                    a10get_sysinfo(&(*acv) , &mei_sysk , 1 , &proc_p , &b_err);
                    if ((int ) b_err == 0)
                        {
                        _ptoc_MOVE(unsigned char , 1, priv.integer.C_1.priv_all_grant_set, ptoc_Var56);
                        ak43spriv_granted_to_show(&(*acv) , &(*a41v) , ptoc_Var58->a_curr_user_name , (*(struct tak_systembuffer *) sql__nil(proc_p , "vak43.p" , 15361)).integer.C_69.smethodinfo.mei_name , 112 , &priv , ptoc_Var58->a_curr_user_id , ptoc_Var58->a_curr_user_id , (*(struct tak_systembuffer *) sql__nil(proc_p , "vak43.p" , 15363)).integer.C_69.smethodinfo.mei_date , (*(struct tak_systembuffer *) sql__nil(proc_p , "vak43.p" , 15364)).integer.C_69.smethodinfo.mei_time);
                        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var58->a_return_segm , "vak43.p" , 15365)).integer.C_4.sp1r_returncode == 0)
                            {
                            ak43dbproc_granted_privileges(&(*acv) , &(*a41v) , &priv , proc_p);
                            }
                        }
                    }
                }
            }
        while (!((int ) b_err != 0 || (int ) (*(tsp1_segment *) sql__nil(ptoc_Var58->a_return_segm , "vak43.p" , 15380)).integer.C_4.sp1r_returncode != 0));
        if ((int ) b_err == 1420)
            {
            b_err = 0;
            }
        }
    if ((int ) b_err == 0 && (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var59->a4authname , ptoc_Var58->a_curr_user_name) == 0 ? 0 : 1))
        {
        init_unique = (char ) sql__setrang(ptoc_Var59->a4unique , 0 , 1);
        _ptoc_MOVE(unsigned char , 1, priv.integer.C_1.priv_all_grant_set, ptoc_Var57);
        repeat_cnt = 0;
        do
            {
            repeat_cnt = repeat_cnt + 1;
            a40init_table_scan(&(*acv) , &(*a41v) , 0 , 0 , 1 , 1 , 0 , 0);
            ptoc_Var59->a4b_err = 0;
            ptoc_Var59->a4next_key = a01defaultkey;
            _ptoc_ASSIGN(unsigned char , 2, ptoc_Var59->a4next_key.integer.C_2.sentrytyp, "\0G")
            if (repeat_cnt == 1)
                {
                _ptoc_ASSIGN(unsigned char , 8, ptoc_Var59->a4next_key.integer.C_3.sauthid, ptoc_Var58->a_curr_user_id)
                }
            else
                {
                _ptoc_ASSIGN(unsigned char , 8, ptoc_Var59->a4next_key.integer.C_3.sauthid, "\0\0\0\0\0\0\0\002")
                }
            ptoc_Var59->a4unique = 0;
            while (a40usertab_next(&(*acv) , &(*a41v) , 0 , 1 , 0 , proc_id , &dummy))
                {
                sysk = a01defaultkey;
                _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0r")
                _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_2.stableid, proc_id)
                a10get_sysinfo(&(*acv) , &sysk , 1 , &proc_p , &b_err);
                if ((int ) b_err == 0)
                    {
                    ptoc_Var60 = &(*(struct tak_systembuffer *) sql__nil(proc_p , "vak43.p" , 15425)).integer.C_69.smethodinfo;
                    if (_ptoc_MEMCMP(unsigned char , 8, ptoc_Var60->mei_owner , ptoc_Var58->a_curr_user_id) == 0 ? 0 : 1)
                        {
                        a06determine_username(&(*acv) , ptoc_Var60->mei_owner , owner_name);
                        if (repeat_cnt == 1)
                            {
                            _ptoc_ASSIGN(unsigned char , 8, grantee_id, ptoc_Var58->a_curr_user_id)
                            }
                        else
                            {
                            _ptoc_ASSIGN(unsigned char , 8, grantee_id, "\0\0\0\0\0\0\0\002")
                            }
                        _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0K")
                        _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_2.suserid, grantee_id)
                        sysk.skeylen = (short ) sql__setrang((short ) ((int ) sysk.skeylen + 8) , -32768 , 32767);
                        a10get_sysinfo(&(*acv) , &sysk , 1 , &priv_p , &b_err);
                        if ((int ) b_err == 0)
                            {
                            ak43spriv_granted_to_show(&(*acv) , &(*a41v) , owner_name , ptoc_Var60->mei_name , 112 , &priv , grantee_id , ptoc_Var60->mei_owner , (*(struct tak_systembuffer *) sql__nil(priv_p , "vak43.p" , 15448)).integer.C_55.sprocseqpriv.psp_date , (*(struct tak_systembuffer *) sql__nil(priv_p , "vak43.p" , 15449)).integer.C_55.sprocseqpriv.psp_time);
                            }
                        else
                            {
                            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                            }
                        }
                    }
                }
            }
        while (!(repeat_cnt >= 2 || (int ) (*(tsp1_segment *) sql__nil(ptoc_Var58->a_return_segm , "vak43.p" , 15460)).integer.C_4.sp1r_returncode != 0));
        ptoc_Var59->a4unique = (char ) sql__setrang(init_unique , 0 , 1);
        }
    if ((int ) b_err != 0)
        {
        a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
        }
    cccprint("<ak43dbproc_priv");
    }

static
  unsigned char ptoc_Var61[1] = 
  {
  (unsigned char)'\0'
  };


int ak43dbproc_granted_privileges(acv, a41v, priv, proc_p)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tak_privilege *priv;
tak_sysbufferaddress proc_p;
    {
    tgg_basis_error b_err;
    tak_sysbufferaddress priv_p;
    tsp_knl_identifier proc;
    tgg_sysinfokey sysk;
    tak_all_command_glob *ptoc_Var62;
    tak_proc_seq_privilege *ptoc_Var63;
    cccprint("AK43  ak43dbproc");
    ptoc_Var62 = &*acv;
    _ptoc_MOVE(unsigned char , 1, (*priv).integer.C_1.priv_all_grant_set, ptoc_Var61);
    sysk = a01defaultkey;
    _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_2.stableid, (*(struct tak_systembuffer *) sql__nil(proc_p , "vak43.p" , 15493)).integer.C_69.smethodinfo.mei_surrogate)
    _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0K")
    _ptoc_ASSIGN(unsigned char , 64, proc, (*(struct tak_systembuffer *) sql__nil(proc_p , "vak43.p" , 15495)).integer.C_69.smethodinfo.mei_name)
    do
        {
        a10next_sysinfo(&(*acv) , &sysk , 10 , 1 , "\0K" , &priv_p , &b_err);
        if ((int ) b_err == 0)
            {
            ptoc_Var63 = &(*(struct tak_systembuffer *) sql__nil(priv_p , "vak43.p" , 15501)).integer.C_55.sprocseqpriv;
            ak43spriv_granted_to_show(&(*acv) , &(*a41v) , ptoc_Var62->a_curr_user_name , proc , 112 , &(*priv) , sysk.integer.C_2.suserid , ptoc_Var62->a_curr_user_id , ptoc_Var63->psp_date , ptoc_Var63->psp_time);
            }
        }
    while (!((int ) b_err != 0 || (int ) (*(tsp1_segment *) sql__nil(ptoc_Var62->a_return_segm , "vak43.p" , 15508)).integer.C_4.sp1r_returncode != 0));
    if ((int ) b_err != 1420)
        {
        a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
        }
    cccprint("<ak43dbproc_gran");
    }

static
  unsigned char ptoc_Var64[1] = 
  {
  (unsigned char)'\0'
  };
static
  tak_priv_desc ptoc_Var65 = 
  {
  (unsigned char)'\0'
  };


int ak43granted_to(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tak_privilege priv;
    tak_all_command_glob *ptoc_Var66;
    tak40_show_glob *ptoc_Var67;
    unsigned char ptoc_Var68[1];
    tak_priv_desc ptoc_Var69;
    cccprint("AK43  ak43grante");
    ptoc_Var66 = &*acv;
    ptoc_Var67 = &*a41v;
    ptoc_Var67->a4sh_kind = 20;
    a40get_catalog_table(&(*acv) , &(*a41v));
    if ((_ptoc_MEMCMP(unsigned char , 64, ptoc_Var67->a4authname , a01_il_b_identifier) == 0 ? 0 : 1) && (_ptoc_MEMCMP(unsigned char , 64, ptoc_Var67->a4tablen , a01_il_b_identifier) == 0 ? 0 : 1) && (int ) ptoc_Var67->a4tab_len == sizeof(unsigned char [64]))
        {
        if (a06_table_exist(&(*acv) , 0 , ptoc_Var67->a4authname , ptoc_Var67->a4tablen , &ptoc_Var67->a4p_arr , 1))
            {
            a06_get_priv(&(*acv) , &ptoc_Var67->a4p_arr.pbasep , &priv);
            if (sql__nre(sizeof(unsigned char [1]) , priv.integer.C_1.priv_all_set , ptoc_Var64) || sql__nre(sizeof(tak_priv_desc ) , priv.integer.C_1.priv_col_exist , ptoc_Var65))
                {
                a43get_granted_priv(&(*acv) , &(*a41v) , ptoc_Var67->a4authname , ptoc_Var67->a4tablen);
                }
            }
        }
    else
        {
        a40init_table_scan(&(*acv) , &(*a41v) , 0 , 1 , 1 , 0 , 1 , 1);
        while (a40next_table(&(*acv) , &(*a41v)))
            a43get_granted_priv(&(*acv) , &(*a41v) , ptoc_Var67->a4next_owner , ptoc_Var67->a4next_tablen);
        }
    cccprint("<ak43granted_to ");
    }

static
  unsigned char ptoc_Var70[1] = 
  {
  (unsigned char)'\001'
  };


int a43get_granted_priv(acv, a41v, owner, tablename)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tsp_knl_identifier owner;
tsp_knl_identifier tablename;
    {
    char next;
    int tab_kind;
    tgg_basis_error b_err;
    tgg_surrogate granteeid;
    tak_sysbufferaddress sysp;
    tgg_sysinfokey sysk;
    tgg_sysinfokey userkey;
    tak_privilege priv;
    unsigned char ptoc_Var71[1];
    struct tak_systembuffer *ptoc_Var72;
    cccprint("AK43  a43get_gra");
    sysk = a01defaultkey;
    _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_2.stableid, (*(struct tak_systembuffer *) sql__nil((*a41v).a4p_arr.pbasep , "vak43.p" , 15596)).integer.C_50.syskey.integer.C_2.stableid)
    _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0\007")
    if (_ptoc_MEMCMP(unsigned char , 64, (*a41v).a4user , a01_il_b_identifier) == 0 ? 0 : 1)
        {
        if (_ptoc_MEMCMP(unsigned char , 64, (*a41v).a4user , a01_i_public) == 0 ? 1 : 0)
            {
            _ptoc_ASSIGN(unsigned char , 8, granteeid, "\0\0\0\0\0\0\0\002")
            }
        else
            {
            a06det_user_id(&(*acv) , (*a41v).a4user , granteeid);
            if (_ptoc_MEMCMP(unsigned char , 8, granteeid , "\0\0\0\0\0\0\0\0") == 0 ? 1 : 0)
                {
                a07_b_put_error(&(*acv) , 6530 , (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak43.p" , 15611))[sql__setrang((*a41v).a4ci , 0 , 32767)].n_pos);
                }
            else
                {
                userkey = a01defaultkey;
                _ptoc_ASSIGN(unsigned char , 8, userkey.integer.C_3.sauthid, granteeid)
                _ptoc_ASSIGN(unsigned char , 2, userkey.integer.C_2.sentrytyp, "\0B")
                a10get_sysinfo(&(*acv) , &userkey , 1 , &sysp , &b_err);
                if ((int ) b_err == 0)
                    {
                    if (sql__erq(sizeof(unsigned char [1]) , (*(struct tak_systembuffer *) sql__nil(sysp , "vak43.p" , 15622)).integer.C_42.suser.urecordtyp , ptoc_Var70))
                        {
                        _ptoc_ASSIGN(unsigned char , 8, granteeid, (*(struct tak_systembuffer *) sql__nil(sysp , "vak43.p" , 15624)).integer.C_42.suser.usergroup_id)
                        }
                    }
                else
                    {
                    a07ak_system_error(&(*acv) , 43 , 3);
                    }
                }
            }
        _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_2.suserid, granteeid)
        sysk.skeylen = (short ) sql__setrang((short ) ((int ) sysk.skeylen + 8) , -32768 , 32767);
        }
    ptoc_Var72 = (struct tak_systembuffer *) sql__nil((*a41v).a4p_arr.pbasep , "vak43.p" , 15638);
    tab_kind = ak43table_kind(&ptoc_Var72->integer.C_1.sbase);
    next = 1;
    while ((char ) sql__setrang(next , 0 , 1) && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak43.p" , 15645)).integer.C_4.sp1r_returncode == 0)
        {
        a10next_sysinfo(&(*acv) , &sysk , 0 , 0 , "\0\007" , &sysp , &b_err);
        if ((int ) b_err != 0)
            {
            next = 0;
            if ((int ) b_err != 1420)
                {
                a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                a40ignore_down_error(&(*acv));
                }
            }
        else
            {
            if ((_ptoc_MEMCMP(unsigned char , 8, (*(struct tak_systembuffer *) sql__nil(sysp , "vak43.p" , 15664)).integer.C_50.syskey.integer.C_2.stableid , (*(struct tak_systembuffer *) sql__nil((*a41v).a4p_arr.pbasep , "vak43.p" , 15665)).integer.C_50.syskey.integer.C_2.stableid) == 0 ? 0 : 1) || (_ptoc_MEMCMP(unsigned char , 64, (*a41v).a4user , a01_il_b_identifier) == 0 ? 0 : 1) && (_ptoc_MEMCMP(unsigned char , 8, (*(struct tak_systembuffer *) sql__nil(sysp , "vak43.p" , 15669)).integer.C_50.syskey.integer.C_2.suserid , granteeid) == 0 ? 0 : 1))
                {
                next = 0;
                }
            else
                {
                if ((_ptoc_MEMCMP(unsigned char , 8, (*(struct tak_systembuffer *) sql__nil((*a41v).a4p_arr.pbasep , "vak43.p" , 15674)).integer.C_1.sbase.bauthid , (*acv).a_curr_user_id) == 0 ? 1 : 0) || (_ptoc_MEMCMP(unsigned char , 8, (*(struct tak_systembuffer *) sql__nil(sysp , "vak43.p" , 15678)).integer.C_50.syskey.integer.C_2.sgrantuser , (*acv).a_curr_user_id) == 0 ? 1 : 0) || (int ) (*a41v).a4sh_kind == 63)
                    {
                    priv = a01emptypriv;
                    a06unpack_priv(&(*(struct tak_systembuffer *) sql__nil(sysp , "vak43.p" , 15686)).integer.C_25.sprivuser.pru_priv , &priv);
                    if (_ptoc_MEMCMP(unsigned char , 8, (*(struct tak_systembuffer *) sql__nil(sysp , "vak43.p" , 15688)).integer.C_50.syskey.integer.C_2.suserid , (*(struct tak_systembuffer *) sql__nil(sysp , "vak43.p" , 15688)).integer.C_50.syskey.integer.C_2.sgrantuser) == 0 ? 0 : 1)
                        {
                        ak43spriv_granted_to_show(&(*acv) , &(*a41v) , owner , tablename , tab_kind , &priv , (*(struct tak_systembuffer *) sql__nil(sysp , "vak43.p" , 15695)).integer.C_50.syskey.integer.C_2.suserid , (*(struct tak_systembuffer *) sql__nil(sysp , "vak43.p" , 15695)).integer.C_50.syskey.integer.C_2.sgrantuser , (*(struct tak_systembuffer *) sql__nil(sysp , "vak43.p" , 15696)).integer.C_25.sprivuser.pru_date , (*(struct tak_systembuffer *) sql__nil(sysp , "vak43.p" , 15697)).integer.C_25.sprivuser.pru_time);
                        }
                    }
                }
            }
        a10_key_del(&(*acv) , &sysk);
        }
    cccprint("<a43get_granted_");
    }

static
  unsigned char ptoc_Var73[1] = 
  {
  (unsigned char)'\020'
  };
static
  unsigned char ptoc_Var74[1] = 
  {
  (unsigned char)'\0'
  };
static
  tak_priv_desc ptoc_Var75 = 
  {
  (unsigned char)'\0'
  };


int ak43spriv_granted_to_show(acv, a41v, owner, tablename, object_kind, priv, granteeid, grantorid, date, time)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tsp_knl_identifier owner;
tsp_knl_identifier tablename;
int object_kind;
tak_privilege *priv;
tgg_surrogate granteeid;
tgg_surrogate grantorid;
tsp_int4 date;
tsp_int4 time;
    {
    char use_column;
    char undef_name2;
    char exit_loop;
    char ora_like;
    tgg_basis_error b_err;
    int i;
    int priv_colno;
    int name2_colno;
    int time_colno;
    tak_sysbufferaddress userrec_buf;
    tak00_colinfo_ptr col_ptr;
    tsp_c8 c8;
    tsp_knl_identifier grantee_name;
    tsp_knl_identifier grantee_owner;
    tsp_knl_identifier grantor_name;
    tsp_knl_identifier column;
    tak_columnset column_set;
    tsp_c40 prn;
    tsp_c40 all_col;
    tgg_sysinfokey sysk;
    tak_all_command_glob *ptoc_Var76;
    tak40_show_glob *ptoc_Var77;
    int ptoc_Var78;
    unsigned char ptoc_Var79[1];
    unsigned char ptoc_Var80[1];
    unsigned char ptoc_Var81[1];
    tak_priv_desc ptoc_Var82;
    tak_columnset ptoc_Var83;
    tak_columnset ptoc_Var84;
    tak_columnset ptoc_Var85;
    tak_columnset ptoc_Var86;
    tak_baserecord *ptoc_Var87;
    tak00_columninfo *ptoc_Var88;
    cccprint("AK43  ak43spriv_");
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak43.p" , 15741)).integer.C_4.sp1r_returncode == 0 && (int ) (*acv).a_ex_kind != 0)
        {
        ptoc_Var76 = &*acv;
        ptoc_Var77 = &*a41v;
        ptoc_Var77->a4pos = 1;
        ora_like = (char ) sql__setrang((ptoc_Var78 = (int ) ptoc_Var77->a4sh_kind) == 79 || ptoc_Var78 == 80 , 0 , 1);
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) owner , sizeof(unsigned char [64]));
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) tablename , sizeof(unsigned char [64]));
        name2_colno = ptoc_Var77->a4pos;
        ptoc_Var77->a4pos = ptoc_Var77->a4pos + 1;
        if ((int ) ptoc_Var77->a4sh_kind != 63 && (int ) ptoc_Var77->a4sh_kind != 20)
            {
            t01int4(5 , "object_kind " , object_kind);
            if ((char ) sql__setrang(ptoc_Var77->a4is_systable , 0 , 1))
                {
                _ptoc_ASSIGN(unsigned char , 8, c8, "SYSTEM  ")
                a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) c8 , sizeof(tsp00_C8 ));
                }
            else
                {
                a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) a01kw[sql__setrang(object_kind , 1 , 533) + -1] , sizeof(tsp00_C8 ));
                }
            }
        a06determine_username(&(*acv) , granteeid , grantee_name);
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) grantee_name , sizeof(unsigned char [64]));
        if ((int ) ptoc_Var77->a4sh_kind != 63 && (int ) ptoc_Var77->a4sh_kind != 20)
            {
            if ((char ) sql__setrang(ptoc_Var77->a4strategy.tgg07_StratEnum.C_strat_catalog.str_catalog.scat_col_needed[sql__setrang(ptoc_Var77->a4pos , 1 , 80) + -1] , 0 , 1))
                {
                _ptoc_ASSIGN(unsigned char , 64, grantee_owner, a01_il_b_identifier)
                sysk = a01defaultkey;
                _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_3.sauthid, granteeid)
                _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0B")
                a10get_sysinfo(&(*acv) , &sysk , 1 , &userrec_buf , &b_err);
                if ((int ) b_err == 0)
                    {
                    a06determine_username(&(*acv) , (*(struct tak_systembuffer *) sql__nil(userrec_buf , "vak43.p" , 15791)).integer.C_42.suser.userowner_id , grantee_owner);
                    }
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) grantee_owner , sizeof(unsigned char [64]));
                }
            else
                {
                ptoc_Var77->a4pos = ptoc_Var77->a4pos + 1;
                }
            }
        priv_colno = ptoc_Var77->a4pos;
        ptoc_Var77->a4pos = ptoc_Var77->a4pos + 1;
        a06determine_username(&(*acv) , grantorid , grantor_name);
        if ((int ) ptoc_Var77->a4sh_kind != 63 && (int ) ptoc_Var77->a4sh_kind != 20)
            {
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) grantor_name , sizeof(unsigned char [64]));
            a40put_date_time(&(*acv) , &(*a41v) , &date , 1 , (char ) sql__setrang(date < 0 , 0 , 1));
            time_colno = ptoc_Var77->a4pos;
            }
        if ((char ) sql__setrang(ora_like , 0 , 1))
            {
            _ptoc_MOVE(unsigned char , 1, (*priv).integer.C_1.priv_all_set, *(unsigned char (*)[1]) sql__setsub(ptoc_Var79 , (*priv).integer.C_1.priv_all_set , ptoc_Var73 , sizeof(unsigned char [1])));
            }
        exit_loop = 0;
        while (!(char ) sql__setrang(exit_loop , 0 , 1) && sql__nre(sizeof(unsigned char [1]) , (*priv).integer.C_1.priv_all_set , ptoc_Var74))
            {
            undef_name2 = 0;
            switch (object_kind)
                {
                case 414 :
                case 458 :
                case 518 :
                    if ((char ) sql__setrang(ora_like , 0 , 1))
                        {
                        ak43put_one_priv(&(*priv) , prn);
                        }
                    else
                        {
                        a40_p_priv(&(*priv) , prn , -1);
                        exit_loop = 1;
                        }
                    undef_name2 = 1;
                    break;
                case 112 :
                    _ptoc_ASSIGN(unsigned char , 40, prn, "                                        ")
                    if (1 && ((*priv).integer.C_1.priv_all_set[0] & 1) != 0)
                        {
                        prn[0] = (unsigned char)'E';
                        prn[1] = (unsigned char)'X';
                        prn[2] = (unsigned char)'E';
                        prn[3] = (unsigned char)'C';
                        prn[4] = (unsigned char)'U';
                        prn[5] = (unsigned char)'T';
                        prn[6] = (unsigned char)'E';
                        }
                    if (1 && ((*priv).integer.C_1.priv_all_grant_set[0] & 1) != 0)
                        {
                        prn[7] = (unsigned char)'+';
                        }
                    undef_name2 = 1;
                    exit_loop = 1;
                    break;
                case 391 :
                    _ptoc_ASSIGN(unsigned char , 40, prn, "                                        ")
                    undef_name2 = 1;
                    exit_loop = 1;
                    break;
                default :
                    sql__caseerr("vak43.p" , 15824);
                    break;
                }
            ptoc_Var77->a4pos = name2_colno;
            if ((char ) sql__setrang(undef_name2 , 0 , 1))
                {
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
                }
            else
                {
                a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) column , sizeof(unsigned char [64]));
                }
            ptoc_Var77->a4pos = priv_colno;
            a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) prn , sizeof(tsp00_C40 ));
            if ((int ) ptoc_Var77->a4sh_kind != 63 && (int ) ptoc_Var77->a4sh_kind != 20)
                {
                ptoc_Var77->a4pos = time_colno;
                a40put_date_time(&(*acv) , &(*a41v) , &time , 0 , (char ) sql__setrang(time < 0 , 0 , 1));
                }
            else
                {
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) grantor_name , sizeof(unsigned char [64]));
                }
            }
        _ptoc_ASSIGN(unsigned char , 40, all_col, prn)
        i = 1;
        if (sql__nre(sizeof(tak_priv_desc ) , (*priv).integer.C_1.priv_col_exist , ptoc_Var75))
            {
            sql__psetadd(column_set , sql__psetadd(ptoc_Var83 , sql__psetadd(ptoc_Var84 , sql__psetadd(ptoc_Var85 , sql__psetadd(ptoc_Var86 , (*priv).integer.C_1.priv_sel_set , (*priv).integer.C_1.priv_grant_sel_set , sizeof(tak_columnset )) , (*priv).integer.C_1.priv_upd_set , sizeof(tak_columnset )) , (*priv).integer.C_1.priv_grant_upd_set , sizeof(tak_columnset )) , (*priv).integer.C_1.priv_ref_set , sizeof(tak_columnset )) , (*priv).integer.C_1.priv_grant_ref_set , sizeof(tak_columnset ));
            ptoc_Var87 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var77->a4p_arr.pbasep , "vak43.p" , 15901)).integer.C_1.sbase;
            while (i <= (int ) ptoc_Var87->bmaxcol && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var76->a_return_segm , "vak43.p" , 15903)).integer.C_4.sp1r_returncode == 0)
                {
                a06extcolno(&(*(struct tak_systembuffer *) sql__nil(ptoc_Var77->a4p_arr.pbasep , "vak43.p" , 15905)).integer.C_1.sbase , i , &col_ptr);
                ptoc_Var88 = (tak00_columninfo *) sql__nil(col_ptr , "vak43.p" , 15906);
                use_column = (char ) sql__setrang((0 || (ptoc_Var88->integer.C_1.ccolpropset[0] & 128) == 0) && (0 || (ptoc_Var88->integer.C_1.ccolpropset[1] & 2) == 0) && ((int ) ptoc_Var88->integer.C_1.cextcolno >= 0 && (int ) ptoc_Var88->integer.C_1.cextcolno <= 1024 && (column_set[(int ) ptoc_Var88->integer.C_1.cextcolno >> 3] & 1 << ((int ) ptoc_Var88->integer.C_1.cextcolno & 7)) != 0) , 0 , 1);
                if ((char ) sql__setrang(use_column , 0 , 1))
                    {
                    a061get_colname(sql__nil(col_ptr , "vak43.p" , 15914) , column);
                    use_column = (char ) sql__setrang(_ptoc_MEMCMP(unsigned char , 64, column , a01_i_tablekey) == 0 ? 0 : 1 , 0 , 1);
                    }
                if ((char ) sql__setrang(use_column , 0 , 1))
                    {
                    ptoc_Var77->a4pos = name2_colno;
                    a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) ptoc_Var88->integer.C_1.ccolumnn , sql__ord((int ) ptoc_Var88->integer.C_1.ccolumnn_len));
                    ptoc_Var77->a4pos = priv_colno;
                    do
                        {
                        exit_loop = 1;
                        if ((char ) sql__setrang(ora_like , 0 , 1))
                            {
                            ak43one_col_priv(&(*priv) , prn , i , &exit_loop);
                            }
                        else
                            {
                            a40_p_priv(&(*priv) , prn , (short ) sql__setrang((short ) i , -32768 , 32767));
                            }
                        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) prn , sizeof(tsp00_C40 ));
                        if ((int ) ptoc_Var77->a4sh_kind != 63 && (int ) ptoc_Var77->a4sh_kind != 20)
                            {
                            ptoc_Var77->a4pos = time_colno;
                            a40put_date_time(&(*acv) , &(*a41v) , &time , 0 , (char ) sql__setrang(time < 0 , 0 , 1));
                            }
                        else
                            {
                            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) grantor_name , sizeof(unsigned char [64]));
                            }
                        }
                    while (!(char ) sql__setrang(exit_loop , 0 , 1));
                    }
                i = sql__succ(i , -2147483647 , 2147483647 , "vak43.p" , 15953);
                }
            }
        }
    cccprint("<ak43spriv_grant");
    }

typedef
  unsigned char tsp_c10_ak43put_one_priv[10];


int ak43put_one_priv(priv, c40)
tak_privilege *priv;
tsp_c40 c40;
    {
    char found;
    int ix;
    tgg_priv_r p;
    tsp_c10 c10;
    tsp_c10_ak43put_one_priv *c10_ptr;
    unsigned char ptoc_Var89[1];
    unsigned char ptoc_Var90[1];
    unsigned char ptoc_Var91[1];
    cccprint("AK43  ak43put_on");
    p = 0;
    found = 0;
    while (!(char ) sql__setrang(found , 0 , 1))
        if ((int ) p >= 0 && (int ) p <= 7 && ((*priv).integer.C_1.priv_all_set[(int ) p >> 3] & 1 << ((int ) p & 7)) != 0)
            {
            found = 1;
            switch ((int ) p)
                {
                case 0 :
                    _ptoc_ASSIGN(unsigned char , 10, c10, "SELECT    ")
                    break;
                case 1 :
                    _ptoc_ASSIGN(unsigned char , 10, c10, "UPDATE    ")
                    break;
                case 2 :
                    _ptoc_ASSIGN(unsigned char , 10, c10, "INSERT    ")
                    break;
                case 3 :
                    _ptoc_ASSIGN(unsigned char , 10, c10, "DELETE    ")
                    break;
                case 5 :
                    _ptoc_ASSIGN(unsigned char , 10, c10, "ALTER     ")
                    break;
                case 6 :
                    _ptoc_ASSIGN(unsigned char , 10, c10, "INDEX     ")
                    break;
                case 7 :
                    _ptoc_ASSIGN(unsigned char , 10, c10, "REFERENCES")
                    break;
                default :
                    (unsigned char ) sql__caseerr("vak43.p" , 15989);
                    break;
                }
            _ptoc_MOVE(unsigned char , 1, (*priv).integer.C_1.priv_all_set, *(unsigned char (*)[1]) sql__setsub(ptoc_Var89 , (*priv).integer.C_1.priv_all_set , sql__pcnst(ptoc_Var91 , 0 , 7 , 0 , 1 , (int ) p) , sizeof(unsigned char [1])));
            _ptoc_ASSIGN(unsigned char , 40, c40, "                                        ")
            c10_ptr = (tsp_c10_ak43put_one_priv *) c40;
            _ptoc_ASSIGN(unsigned char , 10, *(unsigned char (*)[10]) sql__nil(c10_ptr , "vak43.p" , 16009), c10)
            if ((int ) p >= 0 && (int ) p <= 7 && ((*priv).integer.C_1.priv_all_grant_set[(int ) p >> 3] & 1 << ((int ) p & 7)) != 0)
                {
                ix = sizeof(unsigned char [10]);
                while ((int ) c10[sql__setrang(ix , 1 , 10) + -1] == 32)
                    ix = ix + -1;
                c40[sql__setrang(ix + 1 , 1 , 40) + -1] = (unsigned char)'+';
                }
            }
        else
            {
            p = (unsigned char ) sql__setrang((unsigned char ) sql__succ((int ) p , 0 , 7 , "vak43.p" , 16022) , 0 , 7);
            }
    cccprint("<ak43put_one_pri");
    }



int ak43one_col_priv(priv, c40, colno, last_col_priv)
tak_privilege *priv;
tsp_c40 c40;
int colno;
char *last_col_priv;
    {
    tak_columnset ptoc_Var92;
    cccprint("AK43  ak43one_co");
    _ptoc_ASSIGN(unsigned char , 40, c40, "                                        ")
    if (colno >= 0 && colno <= 1024 && ((*priv).integer.C_1.priv_sel_set[colno >> 3] & 1 << (colno & 7)) != 0)
        {
        c40[0] = (unsigned char)'S';
        c40[1] = (unsigned char)'E';
        c40[2] = (unsigned char)'L';
        c40[3] = (unsigned char)'E';
        c40[4] = (unsigned char)'C';
        c40[5] = (unsigned char)'T';
        if (colno >= 0 && colno <= 1024 && ((*priv).integer.C_1.priv_grant_sel_set[colno >> 3] & 1 << (colno & 7)) != 0)
            {
            c40[6] = (unsigned char)'+';
            }
        sql__setsub((*priv).integer.C_1.priv_sel_set , (*priv).integer.C_1.priv_sel_set , sql__pcnst(ptoc_Var92 , 0 , 1024 , 0 , 1 , colno) , sizeof(tak_columnset ));
        *last_col_priv = (char ) sql__setrang(colno < 0 || colno > 1024 || ((*priv).integer.C_1.priv_upd_set[colno >> 3] & 1 << (colno & 7)) == 0 , 0 , 1);
        }
    else
        {
        if (colno >= 0 && colno <= 1024 && ((*priv).integer.C_1.priv_upd_set[colno >> 3] & 1 << (colno & 7)) != 0)
            {
            c40[0] = (unsigned char)'U';
            c40[1] = (unsigned char)'P';
            c40[2] = (unsigned char)'D';
            c40[3] = (unsigned char)'A';
            c40[4] = (unsigned char)'T';
            c40[5] = (unsigned char)'E';
            if (colno >= 0 && colno <= 1024 && ((*priv).integer.C_1.priv_grant_upd_set[colno >> 3] & 1 << (colno & 7)) != 0)
                {
                c40[6] = (unsigned char)'+';
                }
            *last_col_priv = 1;
            }
        }
    cccprint("<ak43one_col_pri");
    }



int ak43table_kind(baserec)
tak_baserecord *baserec;
    {
    int _ak43table_kind;
    cccprint("AK43  ak43table_");
    switch ((int ) (*baserec).btablekind)
        {
        case 5 :
        case 7 :
        case 10 :
            _ak43table_kind = 518;
            break;
        default :
            _ak43table_kind = 458;
            break;
        }
    cccprint("<ak43table_kind ");
    return(_ak43table_kind);
    }

typedef
  unsigned char tgg_surrogate_ak43unique[8];


char ak43unique(acv, is_synonym, tabid, ptoc_ptr_userid)
tak_all_command_glob *acv;
char is_synonym;
tgg_surrogate tabid;
tgg_surrogate ptoc_ptr_userid;
    {
    char _ak43unique;
    tgg_rec_buf buf;
    tgg_surrogate_ak43unique *surr_ptr;
    tak_all_command_glob *ptoc_Var93;
    tgg_surrogate userid;
    _ptoc_MOVE(char, sizeof(tgg_surrogate ), userid, ptoc_ptr_userid);
    cccprint("AK43  ak43unique");
    _ak43unique = 1;
    if (!(char ) sql__setrang(is_synonym , 0 , 1))
        {
        ptoc_Var93 = &*acv;
        _ak43unique = 0;
        buf.integer.C_12.keylen = 16;
        buf.integer.C_12.len = 24;
        buf.integer.C_2.recVarcolOffset_gg00 = 0;
        buf.integer.C_2.recVarcolCnt_gg00 = 0;
        surr_ptr = (tgg_surrogate_ak43unique *) &buf.integer.C_12.info[0];
        _ptoc_ASSIGN(unsigned char , 8, *(tsp00_C8 *) sql__nil(surr_ptr , "vak43.p" , 16121), tabid)
        surr_ptr = (tgg_surrogate_ak43unique *) &buf.integer.C_12.info[sql__setrang(sizeof(tsp00_C8 ) + 1 , 1 , 8184) + -1];
        _ptoc_ASSIGN(unsigned char , 8, *(tsp00_C8 *) sql__nil(surr_ptr , "vak43.p" , 16123), userid)
        b07cadd_record(&ptoc_Var93->a_transinf.tri_trans , &ptoc_Var93->a_into_tree , &buf);
        if ((int ) ptoc_Var93->a_transinf.tri_trans.trError_gg00 == 0)
            {
            _ak43unique = 1;
            }
        else
            {
            if ((int ) ptoc_Var93->a_transinf.tri_trans.trError_gg00 != 530)
                {
                a07_b_put_error(&(*acv) , (short ) sql__setrang(ptoc_Var93->a_transinf.tri_trans.trError_gg00 , -32768 , 32767) , 1);
                }
            }
        }
    cccprint("<ak43unique     ");
    return(_ak43unique);
    }
