#include <ptoc.h>

typedef
  unsigned char tsp00_Char;
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
  unsigned char tsp00_LcompResult;
typedef
  tsp00_LcompResult tsp_lcomp_result;
typedef
  unsigned char tsp00_VfReturn;
typedef
  tsp00_VfReturn tsp_vf_return;
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
  unsigned char tsp00_C30[30];
typedef
  unsigned char tsp00_C40[40];
typedef
  unsigned char tsp00_C48[48];
typedef
  unsigned char tsp00_C256[256];
typedef
  unsigned char tsp00_AnyPackedChar[65535];
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
  tsp00_C12 tsp_c12;
typedef
  tsp00_C16 tsp_c16;
typedef
  unsigned char tsp_c18[18];
typedef
  tsp00_C20 tsp_c20;
typedef
  tsp00_C24 tsp_c24;
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
  tsp00_Buf tsp_buf;
typedef
  unsigned char tsp_cryptpw[24];
typedef
  tsp00_CodeType tsp_code_type;
typedef
  tsp00_DbName tsp_dbname;
typedef
  unsigned char tsp_errtext[40];
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
  tsp00_TaskId tsp_process_id;
typedef
  int tsp_region_id;
typedef
  tsp00_ResNum tsp_resnum;
typedef
  unsigned char tsp_sqlstate[5];
typedef
  tsp00_TermId tsp_termid;
typedef
  unsigned char tsp_timestamp[20];
typedef
  tsp00_VFilename tsp_vfilename;
typedef
  tsp00_WarningSet tsp_warningset;
typedef
  tsp00_KnlIdentifier *tsp_knl_identifier_ptr;
typedef
  tsp00_AnyPackedChar *tsp_objaddr;
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
  unsigned char tsp00_VfBuf[16][8192];
typedef
  tsp00_VfBuf *tsp00_VfBufaddr;
typedef
  tsp00_VfBuf *tsp_vf_bufaddr;
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
  unsigned char tgg00_CacheType;
typedef
  unsigned char tgg00_Debug;
typedef
  unsigned char tgg00_DirectoryMonitorOp;
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
    tgg00_LockReqMode lockMode_gg00;
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
    } tgg_buf_len_info;
typedef
  tgg00_CacheType tgg_cache_type;
typedef
  tgg00_DataPartPtr tgg_datapart_ptr;
typedef
  tgg00_Debug tgg_debug;
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
  struct ptoc_Type52
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
  struct ptoc_Type53
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
  struct ptoc_Type54
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
  struct ptoc_Type55
    {
    tgg00_FileId file_id;
    tgg00_FileId inv_id;
    } tgg_two_tree_ids;
typedef
  struct ptoc_Type56
    {
    tsp00_Int4 no;
    tsp00_VfBufaddr buf;
    tsp00_Int4 poolptr;
    tsp00_Int2 filler;
    tsp00_Int2 buf_cnt;
    } tgg_vf_fileref;
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
    tsp_int2 del_qual_cnt;
    tsp_int2 del_colno;
    tsp00_C2 del_qual[16];
    } tak_del_tab_qual;
typedef
  struct ptoc_Type63
    {
    tsp_int4 rctxt_cnt;
    tsp00_C8 rctxt_roles[32768];
    } tak_role_context;
typedef
  tak_role_context *tak_role_context_ptr;
typedef
  unsigned char tak_map_set[768];
typedef
  struct ptoc_Type64
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
  struct ptoc_Type65
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
  struct ptoc_Type66
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
  struct ptoc_Type67
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
  struct ptoc_Type68
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
  struct ptoc_Type69
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
  struct ptoc_Type70
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
  struct ptoc_Type71
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
  struct ptoc_Type72
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
  struct ptoc_Type73
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
  struct ptoc_Type74
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
  struct ptoc_Type75
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
  struct ptoc_Type76
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
  struct ptoc_Type77
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
  struct ptoc_Type78
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
  struct ptoc_Type79
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
    struct ptoc_Type80
      {
      tgg_surrogate ofp_procid;
      tsp_c3 ofp_language;
      unsigned char ofp_sqlmode[1];
      } fct_procids[64];
    } tak_funcrefrecord;
typedef
  struct ptoc_Type81
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
  struct ptoc_Type82
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
    struct ptoc_Type83
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
  struct ptoc_Type84
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
    struct ptoc_Type85
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
  struct ptoc_Type86
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
  struct ptoc_Type87
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
    struct ptoc_Type88
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
  struct ptoc_Type89
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
  tak_resname_record tak_resn_rec_arr[50];
typedef
  tgg00_StackEntry tak_trigger_stack[900];
typedef
  struct ptoc_Type90
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
  struct ptoc_Type91
    {
    tgg_surrogate usa_tableid;
    tsp_int2 usa_filler;
    unsigned char usa_tablekind;
    char usa_empty;
    } tak_usagedef;
typedef
  struct ptoc_Type92
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
    tak_usagedef usagedef[672];
    } tak_usagerecord;
typedef
  struct ptoc_Type93
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
    struct ptoc_Type94
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
  struct ptoc_Type95
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
  struct ptoc_Type96
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
  struct ptoc_Type97
    {
    tak_sysbufferoffset ru_key_offset;
    tak_uref_restkey ru_rest_key;
    tsp_c2 ru_segmentid;
    tsp_int2 ru_filler;
    tgg_surrogate ru_user_id;
    } tak_userrefrecord;
typedef
  struct ptoc_Type98
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
  struct ptoc_Type99
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
  struct ptoc_Type100
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
  struct ptoc_Type101
    {
    tak_param_spec_ptr param_spec_ptr;
    tsp_int2 param_counter;
    tsp_int2 param_filler1;
    tsp_int4 param_filler2;
    } tak_param_list;
typedef
  struct ptoc_Type102
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
  tak_param_info *tak_param_info_ptr;
typedef
  tak_param_info_ptr tak_param_array[1024];
typedef
  struct ptoc_Type103
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
  struct ptoc_Type104
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
  struct ptoc_Type105
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
  struct ptoc_Type106
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
  struct ptoc_Type107
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
  struct ptoc_Type108
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
  struct ptoc_Type109
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
  struct ptoc_Type110
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
  struct ptoc_Type111
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
  struct ptoc_Type112
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
  struct ptoc_Type113
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
  struct ptoc_Type114
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
  unsigned char tak_viewtext[8064];
typedef
  struct ptoc_Type115
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
        struct ptoc_Type116
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
  struct ptoc_Type117
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
    struct ptoc_Type118
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
  struct ptoc_Type119
    {
    tgg_surrogate vttableid;
    tsp_int2 vtfiller;
    char vtins_for_upd;
    char vtone_to_one;
    tak_charset vtnot_used_links;
    } tak_viewtabarr[183];
typedef
  struct ptoc_Type120
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
  struct ptoc_Type121
    {
    tsp_int2 drecno;
    tsp_int2 dextno;
    tsp_int2 dfiller;
    tsp00_Uint1 dexttabno;
    tsp00_Uint1 dtabno;
    } tak_derived_col_info[1009];
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
    char vfiller;
    char vjoin_exists;
    char vstack_exists;
    char vderived_exists;
    tsp_int2 vbasetabcnt;
    tak_viewtabarr vtable;
    } tak_viewqual_basis_record;
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
    tsp_int2 vjoincount;
    tak_viewjoins vjoin;
    } tak_viewqual_join_record;
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
    tsp_int2 vderivedcnt;
    tak_derived_col_info vderived_cols;
    } tak_viewqual_derivedcol_record;
typedef
  struct ptoc_Type126
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
  struct ptoc_Type127
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
    struct ptoc_Type128
      {
      tsp_int4 cst_dist_values;
      tsp_int4 cst_pages;
      tsp_int4 cst_avg_lst_len;
      } sti_col_stats[660];
    } tak_statisticsinfo;
typedef
  struct ptoc_Type129
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
  struct ptoc_Type130
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
    struct ptoc_Type131
      {
      tsp_knl_identifier str_name;
      tgg00_SubtransNo str_no;
      } str_subtrans[50];
    } tak_subtransrecord;
typedef
  struct ptoc_Type132
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
  struct ptoc_Type133
    {
    tsp_int2 ch_startpos;
    unsigned char ch_length;
    unsigned char ch_type[1];
    } tak_column_change_arr[1024];
typedef
  struct ptoc_Type134
    {
    tsp_int2 cr_colcount;
    char cr_was_opt;
    char cr_filler;
    tak_column_change_arr cr_columns;
    } tak_changerecord;
typedef
  struct ptoc_Type135
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
  struct ptoc_Type136
    {
    tsp_int2 ks_pos;
    tsp_int2 ks_len;
    } tak_keyspec;
typedef
  struct ptoc_Type137
    {
    tak_keyspec reckeyspec;
    tak_keyspec listkeyspec;
    } tak_two_keyspecs;
typedef
  unsigned char tak_res_keysbuf[13316];
typedef
  struct ptoc_Type138
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
  struct ptoc_Type139
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
  struct ptoc_Type140
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
  struct ptoc_Type141
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
    tsp_int2 all_lock_count;
    tsp_int2 all_lock_fill;
    tgg00_Lock all_locks[240];
    } tak_ak_all_locks;
typedef
  struct ptoc_Type146
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
    unsigned char scr_sql_mode;
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
  unsigned char tak_viewscantype;
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
  struct ptoc_Type163
    {
    tsp_knl_identifier vsc_baseauthname;
    tsp_knl_identifier vsc_basetablen;
    tgg91_FileVersion vsc_filevers;
    char vsc_rp_rt_handling;
    tak_viewscantype vsc_init_type;
    tgg_surrogate vsc_base_tabid;
    tgg_sysinfokey vsc_key;
    char vsc_edi;
    char vsc_free;
    tsp_int2 vsc_ti;
    tsp_int2 vsc_tabno;
    tak_viewscantype vsc_type;
    char vsc_delete_view;
    char vsc_mult_index;
    char vsc_modified;
    tgg_surrogate vsc_from_tableid;
    char vsc_col_list;
    char vsc_col_dropped;
    tak_usagedef vsc_usagedef;
    tgg_surrogate vsc_start_tabid;
    char vsc_vdesc_modified[8];
    tsp_int4 vsc_rows;
    tak_joinset vsc_tabno_set;
    tsp_int2 vsc_vdesc_cnt;
    tgg00_FileId vsc_tree_id;
    tak_sysbufferaddress vsc_buf;
    tsp_knl_identifier vsc_into_authname;
    tsp_knl_identifier vsc_into_tablen;
    char vsc_save_into;
    unsigned char vsc_tablekind;
    tgg00_FileId vsc_into_treeid;
    tsp_int4 vsc_cmd_cnt;
    char vsc_first_save;
    char vsc_last_save;
    char vsc_user_save;
    char vsc_all_save;
    } tak_save_viewscan_par;
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
  struct ptoc_Type174
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
  struct ptoc_Type175
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
typedef
  unsigned char tak_balance_info;
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
      tak_balance_info cd_balance;
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
  unsigned char tsp3_priority;
typedef
  unsigned char tsp3_msg_type;
typedef
  short tgg07_ColPosArr[20];
typedef
  tsp00_C1 tgg07_StrRaw[236];
typedef
  struct ptoc_Type176
    {
    tgg07_ColPosArr skir_keystart;
    tgg07_ColPosArr skir_keystop;
    tsp00_Int2 skir_IN_SUBQ_stpos;
    tsp00_Int2 skir_inelemcnt;
    } tgg07_StrKeyInRange;
typedef
  unsigned char tgg07_InvColSet[2];
typedef
  struct ptoc_Type177
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
  struct ptoc_Type178
    {
    tgg07_ColPosArr sji_keystart;
    tgg07_ColPosArr sji_keystop;
    tsp00_PageNo sji_invroot;
    tsp00_Int2 sji_invindex;
    tsp00_Int2 sji_jfiller;
    } tgg07_StrJoinInv;
typedef
  struct ptoc_Type179
    {
    tgg07_ColPosArr sjmf_keystart;
    tgg07_ColPosArr sjmf_keystop;
    tsp00_PageNo sjmf_invroot;
    tsp00_Int2 sjmf_index_no;
    tsp00_Int2 sjmf_cntfields;
    } tgg07_StrJoinMultFields;
typedef
  struct ptoc_Type180
    {
    tsp00_Int2 sjv_lastkeylen;
    tsp00_Int2 sjv_minkeylen;
    } tgg07_StrJoinViewkey;
typedef
  struct ptoc_Type181
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
  struct ptoc_Type182
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
  struct ptoc_Type183
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
  struct ptoc_Type184
    {
    tsp_int2 shsl;
    tsp_int2 shkl;
    tsp_int2 shvarcol_offset;
    tsp_int2 shvarcol_cnt;
    tsp_c4 shkey;
    tak40_shinfo shinfo;
    } tak40_show_record;
typedef
  struct ptoc_Type185
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
  struct ptoc_Type186
    {
    tak_cache_dir_pointer fi_ptr;
    tsp_lcomp_result fi_res;
    unsigned char fi_kind;
    } tak10_find_info;
typedef
  struct ptoc_Type187
    {
    tsp00_C4 ch_label;
    tsp_int4 ch_size;
    tsp_bufaddr ch_next_cache;
    tsp00_Int4 ch_filler;
    } tak10cache_header;
typedef
  struct ptoc_Type188
    {
    tsp_moveobj_ptr addr;
    tsp_int4 size;
    tsp_int4 len;
    } tak10_mblock_info[4];
typedef
  struct ptoc_Type189
    {
    unsigned char q_hd[112];
    tgg00_StackEntry q_st[8];
    } tak10qual;
typedef
  tak10cache_header *tak10cache_header_ptr;
typedef
  struct ptoc_Type190
    {
    tsp_int4 dmp_pno;
    int dmp_pos;
    int dmp_header_pos;
    tsp_vf_return dmp_error;
    tgg_basis_error dmp_b_err;
    tgg_vf_fileref dmp_file;
    tsp_buf dmp_buf;
    } tak10_dump_context;
typedef
  struct ptoc_Type191
    {
    tsp_int4 pos;
    tsp_int4 len;
    } tak10_used_blocks[10000];
tgg_sysinfokey a01defaultkey;
tgg_sysinfokey a01sysnullkey;
tsp_knl_identifier a01_i_ak_cache;
tsp_knl_identifier a01_il_b_identifier;
int a01identifier_size;
char g01restart_local;
tgg_tabid_globals g01tabid;
char g01unicode;
tsp_region_id g08diagcache;
tsp00_VFilename b01blankfilename;
tgg00_FileId b01niltree_id;
char a10chk;
tsp00_C4 ak10ChunkLabel;
tsp_int4 ak10offset;
tsp_int4 ak10all_cache_size;
tsp_int4 ak10init_cache_size;
tsp_int4 ak10cache_cnt;
tsp_int_map_c2 ak10joinViewVersion;
tsp00_C2 ak10displaceTypes[4];
int a06cpy_mblock();
int a06ignore_rsend_mess_buf();
int a06inc_linkage();
int a06lsend_mess_buf();
int a06set_mtree();
int a061app_columnname();
int a061add_name();
int a061copy_colinfo();
int a061get_colname();
int a061identifier_len();
int a061compress_baserec();
int a061td_colinfo();
int a07_const_b_put_error();
int a07_b_put_error();
int a07_nb_put_error();
int a07ak_system_error();
char a10_ptr_ok();
char a102IsCacheOk();
short a11firstindex();
int a262DeleteTriggerParsinfo();
int a27init_viewscanpar();
int a27view_scan();
int a40get_catalog_table();
int a40move();
int a40move_const();
int a40move_i4();
int a51inc_ak_cache_size();
char a663parse_for_execute();
int a71add_default_strat();
int a71del_sysinfo_strategy();
int g01cache_size();
int g01catalog_cache_init_size();
int g01datapart_init();
int g01maxuser();
int g01mblock_init();
int g01qual_init();
int g01stack_init();
int g01stratpart_init();
int g01abort();
int g01opmsg();
int g01optextmsg();
int g01new_dump_page();
int g10fil();
int g10fil1();
int g10mv();
int g10mv1();
int g10mv2();
int g10mv3();
int g10mv4();
int g10mv5();
int g10mv6();
int g10mv7();
int g10mv8();
int g10mv9();
int g10mv10();
int g10mv11();
int g10mv12();
int g10mv14();
int g10mv15();
int g10mvr();
int g17hexto_line();
int b01treset_file();
int b07cnext_record();
int b07cadd_record();
int b07cget_record();
int b07crepl_record();
int b07cdel_record();
int b73directory_ref_statistic();
int k53wait();
int s10mv1();
int s10mv2();
int s30cmp();
int s30lnr();
int s30unilnr();
tsp_bufaddr s35add_bufaddr();
int vbegexcl();
int vendexcl();
int vfclose();
int vfopen();
int vfwrite();
int vmfree();
int vmalloc();
int t01addr();
int t01catalog_entrytype();
int t01str30();
int t01p2int4();
int t01int4();
int t01buf();
int t01buf1();
int t01moveobj();
int t01site();
int t01stackentry();
int t01surrogate();
int t01messblock();
char t01trace();
int cccprint();
int a10_add_repl_sysinfo();
int a10ak_cache_init();
int a10alter_long_version();
int a10cache_used();
int a10clean_cache();
int a10cmd_rollback();
int a10_copy_catalog_rec();
int a10cpy_result();
int a10DeallocTemp();
int a10_del_tab_sysinfo();
int a10del_user_sysinfo();
int a10dispose();
int a10find_result();
int a10_fix_len_get_sysinfo();
int ak10fill_mblock_info();
int a10flush();
int ak10FlushResnames();
int a10FlushResnames();
int a10free_cache();
int a10hit_rate();
int a10increment_ap_tree();
int a10inquire_resnames();
int a10intermediate_mblock();
int ak10inq_resname();
int a10ins_or_upd();
int a10invalidate_root();
char a10is_fixed();
int a10lock_fixed();
int a10gap_dispose();
int ak10gaps_baserecord();
int ak10gap_free_colnamesrecord();
int ak10gap_free_mblock();
int ak10gap_free_method_record();
int a10look_for_gaps();
int ak10add_block();
int a10mark_updated();
int a10mark_unrelease();
int a10mark_release();
int a10mblock_into_cache();
int a10new();
int a10new_packet();
int a10prefix_parsid_delete();
int a10refresh_cache();
int a10release_table();
int a10remove_parsinfos();
int a10shrink_syntax_tree();
int a10table_cache_delete();
int a10unlock_fixed();
int a10add_sysinfo();
int a10_all_release();
int a10Displace();
int a10_cache_delete();
int a10del_sysinfo();
int a10get_sysinfo();
int a10_init_cache();
int a10_key_del();
int a10key_del();
int a10_lock_sysinfo();
int a10next_sysinfo();
int a10_nil_get_sysinfo();
int a10_rel_sysinfo();
int a10rel_sysinfo();
int a10repl_sysinfo();
int a10SessionCacheInit();
int a10_upd_key();
int a10_version();
int ak10add_base_record();
int ak10add_method_record();
int ak10store_colnames_record();
int ak10add_upd_perm_record();
int ak10no_more_memory();
int ak10send_ins_upd();
int ak10all_temp_add();
int ak10alloc_mem();
int a10BaseRecPersistentOffset();
int ak10base_record();
int ak10block_into_freelist();
int ak10CacheCheck();
int ak10cache_error();
int ak10check_hit_rate();
int ak10del();
int ak10del_balance_left();
int ak10del_balance_right();
int ak10del_in_dir();
int ak10del_in_directory();
int a10expand_rec();
int ak10find_syskey();
int ak10free_cache();
int ak10free_baserecord();
int ak10free_colnamesrecord();
int ak10free_method_record();
int ak10free_mblock();
int ak10free_rest();
int ak10get_next_avail();
int ak10getmem();
int ak10get_mess_block();
int ak10hit_rate();
int ak10get_colnames_record();
int ak10get_method_record();
int ak10get_perm_record();
int ak10get_supplement();
int ak10ins_directory();
int ak10lock_unlock_fixed();
int ak10invalidate_root();
int ak10put_mbuf_into_cache();
int ak10release_table();
int ak10restore_record_suffix();
int ak10send_mess_block();
int ak10size_check();
int ak10space_for_mblock();
int ak10store_record_suffix();
int ak10store_mess_block();
int ak10store_syskey();
int ak10table_cache_delete();
int ak10temp_add();
int ak10synonym_record();
int ak10tdcheck_order();
int ak10writeln();
int a10verify_cache();
int ak10verify_cache();
int td_tree();
int ak10check_tree();
int ak10td_dirlist();
int ak10dirlist();
int ak10td_freelist();
int xtd_directory();
int ak10count();
int ak10dump_write();
char a10akdump();
int a10dump();
int ak10dump();
int a10parsinfos();
int ak10cache_parsinfos();
int ak10parsfile_infos();
int ak10one_parsinfo();



int a10_add_repl_sysinfo(acv, syspoint, add_sysinfo, b_err)
tak_all_command_glob *acv;
tak_sysbufferaddress *syspoint;
char add_sysinfo;
tgg_basis_error *b_err;
    {
    cccprint("AK10  a10_add_re");
    t01buf1(0 , &(*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 15381)).integer.C_50.syskey , 1 , (int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 15382)).integer.C_50.syskey.skeylen + 8);
    *b_err = 0;
    if (!(char ) sql__setrang(add_sysinfo , 0 , 1))
        {
        a10repl_sysinfo(&(*acv) , &(*syspoint) , &(*b_err));
        }
    if ((char ) sql__setrang(add_sysinfo , 0 , 1) || (int ) *b_err == 1930)
        {
        a10add_sysinfo(&(*acv) , &(*syspoint) , &(*b_err));
        }
    cccprint("<a10_add_repl_sy");
    }



int a10ak_cache_init()
    {
    int required_len;
    int ptoc_Var0;
    cccprint("AK10  a10ak_cach");
    ak10ChunkLabel[0] = (unsigned char)'A';
    ak10ChunkLabel[1] = (unsigned char)'C';
    ak10ChunkLabel[2] = (unsigned char ) sql__setrang((unsigned char ) (sql__ord(48) + sizeof(tsp00_Addr )) , 0 , 255);
    ak10ChunkLabel[3] = (unsigned char ) sql__setrang((unsigned char ) sql__ord(48) , 0 , 255);
    ak10all_cache_size = 0;
    ak10cache_cnt = 0;
    ak10offset = 0;
    ak10joinViewVersion.boolean.C_true.map_int = 0;
    ak10init_cache_size = g01catalog_cache_init_size() / sizeof(tsp00_Buf );
    required_len = 2;
    required_len = sizeof(tgg00_SysInfoKey );
    if (((ptoc_Var0 = required_len % 4) >= 0 ? ptoc_Var0 : ptoc_Var0 + 4) != 0)
        {
        required_len = required_len - ((ptoc_Var0 = required_len % 4) >= 0 ? ptoc_Var0 : ptoc_Var0 + 4) + 4;
        }
    if (required_len < 48)
        {
        required_len = 48;
        }
    ak10offset = ak10offset + required_len;
    _ptoc_ASSIGN(unsigned char , 2, ak10displaceTypes[0], "\0g")
    _ptoc_ASSIGN(unsigned char , 2, ak10displaceTypes[1], "\0f")
    _ptoc_ASSIGN(unsigned char , 2, ak10displaceTypes[2], "\0\022")
    _ptoc_ASSIGN(unsigned char , 2, ak10displaceTypes[3], "\0\021")
    cccprint("<a10ak_cache_ini");
    }



int a10alter_long_version(acv, base_rec, old_long_cnt, m_type, view_scan)
tak_all_command_glob *acv;
tak_baserecord *base_rec;
int old_long_cnt;
tgg_message_type m_type;
char view_scan;
    {
    tgg_basis_error b_err;
    tak_syspointerarr aux_arr;
    tak_save_viewscan_par viewscanpar;
    tgg_mess_block mblock;
    tak10qual qual;
    tak_all_command_glob *ptoc_Var1;
    int ptoc_Var2;
    cccprint("AK10  a10alter_l");
    qual.q_st[0].integer.C_1.etype = 22;
    b_err = 0;
    ptoc_Var1 = &*acv;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var1->a_return_segm , "vak10.p" , 15457)).integer.C_4.sp1r_returncode == 0)
        {
        if (((ptoc_Var2 = (int ) (*base_rec).btablekind) == 1 || ptoc_Var2 == 2) && !(char ) sql__setrang((*base_rec).bunloaded , 0 , 1))
            {
            g01mblock_init(&(*acv).a_transinf.tri_trans , (unsigned char ) sql__setrang(m_type , 0 , 95) , 22 , &mblock);
            mblock.integer.C_2.mb_struct = 1;
            g01qual_init(&mblock , (tgg00_QualBuf *) &qual , sizeof(tak10qual ));
            a06set_mtree(&(*acv) , &mblock , &(*base_rec).btreeid);
            (*(tgg00_QualBuf *) sql__nil(mblock.integer.C_4.mb_qual , "vak10.p" , 15468)).integer.C_5.mold_long_cnt = (short ) sql__setrang((short ) old_long_cnt , -32768 , 32767);
            (*(tgg00_QualBuf *) sql__nil(mblock.integer.C_4.mb_qual , "vak10.p" , 15469)).integer.C_5.mnew_long_cnt = (short ) sql__setrang((*base_rec).bstringcount , -32768 , 32767);
            mblock.integer.C_2.mb_qual_len = sizeof(tgg00_FileId ) + sizeof(short ) * 2;
            a06lsend_mess_buf(&(*acv) , &mblock , 0 , &b_err);
            if ((int ) b_err == 0)
                {
                a262DeleteTriggerParsinfo(&(*acv) , &(*base_rec));
                (*base_rec).btreeid.integer.C_1.fileVersion_gg00 = (*(tgg00_QualBuf *) sql__nil(mblock.integer.C_4.mb_qual , "vak10.p" , 15479)).integer.C_2.mtree.integer.C_1.fileVersion_gg00;
                if ((char ) sql__setrang(view_scan , 0 , 1) && (int ) b_err == 0 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var1->a_return_segm , "vak10.p" , 15481)).integer.C_4.sp1r_returncode == 0)
                    {
                    a27init_viewscanpar(&(*acv) , &viewscanpar , 0);
                    viewscanpar.vsc_filevers = (*base_rec).btreeid.integer.C_1.fileVersion_gg00;
                    aux_arr = ptoc_Var1->a_p_arr1;
                    ptoc_Var1->a_p_arr1.pbasep = (struct tak_systembuffer *) &*base_rec;
                    a27view_scan(&(*acv) , (*(tgg00_QualBuf *) sql__nil(mblock.integer.C_4.mb_qual , "vak10.p" , 15489)).integer.C_2.mtree.integer.C_4.fileTabId_gg00 , &viewscanpar);
                    ptoc_Var1->a_p_arr1 = aux_arr;
                    }
                }
            if ((int ) b_err != 0)
                {
                if ((int ) b_err == 720)
                    {
                    a07_b_put_error(&(*acv) , 1500 , 1);
                    }
                else
                    {
                    a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                    }
                }
            }
        }
    cccprint("<a10alter_long_v");
    }



int a10cache_used()
    {
    int _a10cache_used;
    cccprint("AK10  a10cache_u");
    _a10cache_used = ak10all_cache_size;
    cccprint("<a10cache_used  ");
    return(_a10cache_used);
    }



int a10clean_cache(acv)
tak_all_command_glob *acv;
    {
    tak_cache_dir_pointer curr;
    tak_cache_dir_pointer prev;
    tak_sysbufferaddress sysp;
    cccprint("AK10  a10clean_c");
    prev = (*acv).a_sysdir.ci_cache_dir_header;
    curr = (*(struct tak_cache_dir_entry *) sql__nil((*acv).a_sysdir.ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 15543)).integer.C_1.cd_lru_next;
    while (curr.integer.C_1.cdir_ptr != (*acv).a_sysdir.ci_cache_dir_trailor.integer.C_1.cdir_ptr)
        {
        if ((*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 15546)).integer.C_1.cd_cmd_no == (*acv).a_command_count && (0 || ((*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 15547)).integer.C_1.cd_state[0] & 8) == 0) && (0 || ((*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 15548)).integer.C_1.cd_state[0] & 16) == 0))
            {
            sysp = (struct tak_systembuffer *) &(*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 15551)).integer.C_2.cd_sysinfo;
            a10key_del(&(*acv) , &sysp);
            curr = prev;
            }
        prev = curr;
        curr = (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 15557)).integer.C_1.cd_lru_next;
        }
    cccprint("<a10clean_cache ");
    }



int a10cmd_rollback(acv)
tak_all_command_glob *acv;
    {
    char last_rec_buf;
    char del_succ;
    tgg_basis_error b_err;
    tsp_int_map_c2 curr_keylen;
    int first_keylen;
    int rec_pos;
    int rec_len;
    tak_cache_dir_pointer curr;
    tak_cache_dir_pointer prev;
    tgg00_FilePos tree_pos;
    tgg_bd_set_result_record set_result;
    tak10_find_info find_info;
    tgg_lkey zerokey;
    tgg_sysinfokey syskey;
    tgg_rec_buf b;
    struct tak_cache_dir_entry *ptoc_Var3;
    tgg00_BdSetResultRecord *ptoc_Var4;
    int ptoc_Var5;
    cccprint("AK10  a10cmd_rol");
    prev = (*acv).a_sysdir.ci_cache_dir_header;
    curr = (*(struct tak_cache_dir_entry *) sql__nil((*acv).a_sysdir.ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 15590)).integer.C_1.cd_lru_next;
    while (curr.integer.C_1.cdir_ptr != (*acv).a_sysdir.ci_cache_dir_trailor.integer.C_1.cdir_ptr)
        {
        if ((*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 15593)).integer.C_1.cd_cmd_no == (*acv).a_command_count)
            {
            if ((*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 15596)).integer.C_1.cd_create_cmd_no == -2 && (*acv).a_sysdir.ci_cmd_rollback_tree.integer.C_1.fileRoot_gg00 != 2147483647)
                {
                ptoc_Var3 = (struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 15599);
                b07cget_record(&(*acv).a_transinf.tri_trans , &(*acv).a_sysdir.ci_cmd_rollback_tree , &ptoc_Var3->integer.C_1.cd_syskey , &b);
                if ((int ) (*acv).a_transinf.tri_trans.trError_gg00 == 0)
                    {
                    ptoc_Var3->integer.C_1.cd_create_cmd_no = ptoc_Var3->integer.C_1.cd_cmd_no;
                    }
                }
            if (((*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 15612)).integer.C_1.cd_create_cmd_no == (*acv).a_command_count || (int ) (*acv).a_init_ddl != 0) && ((_ptoc_MEMCMP(unsigned char , 2, (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 15615)).integer.C_1.cd_syskey.integer.C_2.sentrytyp , "\0\265") == 0 ? 0 : 1) || !(char ) sql__setrang((*acv).a_intern_explain , 0 , 1)))
                {
                ak10find_syskey(&(*acv) , &(*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 15619)).integer.C_1.cd_syskey , &find_info);
                if ((int ) find_info.fi_res == 1)
                    {
                    ak10block_into_freelist(&(*acv) , find_info.fi_ptr , 1 , 1);
                    }
                }
            else
                {
                prev = curr;
                }
            }
        else
            {
            prev = curr;
            }
        curr = (*(struct tak_cache_dir_entry *) sql__nil(prev.integer.C_1.cdir_ptr , "vak10.p" , 15633)).integer.C_1.cd_lru_next;
        }
    if ((*acv).a_sysdir.ci_cmd_rollback_tree.integer.C_1.fileRoot_gg00 != 2147483647)
        {
        ptoc_Var4 = &set_result;
        ptoc_Var4->bd_key_check_len = 0;
        ptoc_Var4->bd_max_rec_cnt = 32767;
        ptoc_Var4->bd_max_fill_len = sizeof(tsp00_Buf );
        ptoc_Var4->bd_next = 0;
        rec_pos = 1;
        rec_len = 0;
        zerokey.integer.C_11.len = 0;
        tree_pos.tpsPno_gg00 = 2147483647;
        last_rec_buf = 0;
        do
            {
            b_err = 0;
            if (rec_pos >= rec_len)
                {
                rec_pos = 0;
                if ((char ) sql__setrang(last_rec_buf , 0 , 1))
                    {
                    b_err = 1420;
                    }
                else
                    {
                    b07cnext_record(&(*acv).a_transinf.tri_trans , &(*acv).a_sysdir.ci_cmd_rollback_tree , &zerokey , &set_result , &tree_pos , b.integer.C_11.buf);
                    b_err = (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                    (*acv).a_transinf.tri_trans.trError_gg00 = 0;
                    }
                if ((int ) b_err == 0 || (int ) b_err == 1200 || (int ) b_err == 350)
                    {
                    if ((int ) b_err != 350)
                        {
                        last_rec_buf = 1;
                        }
                    rec_len = set_result.bd_fill_len;
                    b_err = 0;
                    }
                }
            if ((int ) b_err == 0)
                {
                curr_keylen.boolean.C_false.map_c2[0] = (unsigned char ) sql__setrang(b.integer.C_11.buf[sql__setrang(rec_pos + 3 , 1 , 8192) + -1] , 0 , 255);
                curr_keylen.boolean.C_false.map_c2[1] = (unsigned char ) sql__setrang(b.integer.C_11.buf[sql__setrang(rec_pos + 4 , 1 , 8192) + -1] , 0 , 255);
                syskey.skeylen = (short ) sql__setrang(curr_keylen.boolean.C_true.map_int , -32768 , 32767);
                g10mv5("VAK10 " , 1 , sizeof(tsp00_Buf ) , sizeof(tgg00_SysInfoKey ) , b.integer.C_11.buf , rec_pos + 1 , &syskey , 1 , (int ) syskey.skeylen + 8 , &b_err);
                del_succ = 0;
                if ((ptoc_Var5 = (int ) syskey.integer.C_2.sentrytyp[1]) == 177 || ptoc_Var5 == 192 || ptoc_Var5 == 130 || ptoc_Var5 == 129 || ptoc_Var5 == 149 || ptoc_Var5 == 193 || ptoc_Var5 == 24 || ptoc_Var5 == 132 || ptoc_Var5 == 178 || ptoc_Var5 == 190)
                    {
                    del_succ = 1;
                    }
                first_keylen = (int ) syskey.skeylen;
                if ((int ) b_err == 0)
                    {
                    do
                        {
                        b07cdel_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , &syskey);
                        if ((char ) sql__setrang(del_succ , 0 , 1))
                            {
                            if ((int ) syskey.skeylen == first_keylen)
                                {
                                syskey.skeylen = (short ) sql__setrang((short ) ((int ) syskey.skeylen + 1) , -32768 , 32767);
                                syskey.integer.C_8.all[sql__setrang((int ) syskey.skeylen , 1 , 140) + -1] = (unsigned char)'\001';
                                }
                            else
                                {
                                syskey.integer.C_8.all[sql__setrang((int ) syskey.skeylen , 1 , 140) + -1] = (unsigned char ) sql__setrang((unsigned char ) sql__succ((int ) syskey.integer.C_8.all[sql__setrang((int ) syskey.skeylen , 1 , 140) + -1] , 0 , 255 , "vak10.p" , 15721) , 0 , 255);
                                }
                            }
                        }
                    while (!(!(char ) sql__setrang(del_succ , 0 , 1) || (int ) (*acv).a_transinf.tri_trans.trError_gg00 != 0));
                    if ((int ) (*acv).a_transinf.tri_trans.trError_gg00 == 1200)
                        {
                        b_err = 0;
                        }
                    else
                        {
                        b_err = (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                        }
                    }
                (*acv).a_transinf.tri_trans.trError_gg00 = 0;
                rec_pos = rec_pos + (int ) curr_keylen.boolean.C_true.map_int + 8;
                }
            }
        while (!((int ) b_err != 0));
        (*acv).a_sysdir.ci_cmd_rollback_tree.integer.C_1.fileRoot_gg00 = 2147483647;
        }
    cccprint("<a10cmd_rollback");
    }

static
  unsigned char ptoc_Var6[1] = 
  {
  (unsigned char)'8'
  };
static
  unsigned char ptoc_Var7[1] = 
  {
  (unsigned char)'\b'
  };


int a10_copy_catalog_rec(acv, old_key, del_old_rec, new_key, ptoc_ptr_new_segment_id, add_new_rec, b_err)
tak_all_command_glob *acv;
tgg_sysinfokey *old_key;
char del_old_rec;
tgg_sysinfokey *new_key;
tsp_c2 ptoc_ptr_new_segment_id;
char add_new_rec;
tgg_basis_error *b_err;
    {
    char old_deleted;
    char dummy;
    tsp_c2 segmentid;
    int ix;
    int firstindex;
    int lastindex;
    int new_index;
    tak_basecolinfo_ptr base_colptr;
    tak_sysbufferaddress sysp;
    tak_cache_dir_pointer p;
    tak00_colinfo_ptr curr_col;
    tak10_find_info find_info;
    tsp_knl_identifier colname;
    int ptoc_Var8;
    tak_baserecord *ptoc_Var9;
    int ptoc_Var10;
    unsigned char ptoc_Var11[1];
    unsigned char ptoc_Var12[1];
    unsigned char ptoc_Var13[1];
    tsp_c2 new_segment_id;
    _ptoc_MOVE(char, sizeof(tsp_c2 ), new_segment_id, ptoc_ptr_new_segment_id);
    cccprint("AK10  a10_copy_c");
    ak10find_syskey(&(*acv) , &(*old_key) , &find_info);
    if ((int ) find_info.fi_res != 1)
        {
        a10get_sysinfo(&(*acv) , &(*old_key) , 1 , &sysp , &(*b_err));
        ak10find_syskey(&(*acv) , &(*old_key) , &find_info);
        }
    else
        {
        sysp = (struct tak_systembuffer *) &(*(struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 15784)).integer.C_2.cd_sysinfo;
        *b_err = 0;
        }
    if ((int ) *b_err == 0)
        {
        t01int4(0 , "del_old_rec " , sql__ord((int ) (char ) sql__setrang(del_old_rec , 0 , 1)));
        t01buf1(0 , &(*old_key) , 9 , (int ) (*old_key).skeylen + 8);
        t01buf1(0 , &(*new_key) , 9 , (int ) (*new_key).skeylen + 8);
        ak10del_in_dir(&(*acv) , &(*old_key));
        if ((ptoc_Var8 = (int ) (*old_key).integer.C_2.sentrytyp[1]) == 1 || ptoc_Var8 == 177)
            {
            base_colptr = (*(struct tak_systembuffer *) sql__nil(sysp , "vak10.p" , 15802)).integer.C_1.sbase.bptr;
            (*(struct tak_systembuffer *) sql__nil(sysp , "vak10.p" , 15803)).integer.C_1.sbase.bptr = (struct tak_basecolinfo *) (int *) 0;
            }
        else
            {
            base_colptr = (struct tak_basecolinfo *) (int *) 0;
            }
        if ((char ) sql__setrang(del_old_rec , 0 , 1) && (1 && ((*(struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 15809)).integer.C_1.cd_state[0] & 16) != 0 || (_ptoc_MEMCMP(unsigned char , 2, (*old_key).integer.C_2.sentrytyp , "\0\200") < 0 ? 1 : 0)))
            {
            old_deleted = 1;
            if (_ptoc_MEMCMP(unsigned char , 2, (*old_key).integer.C_2.sentrytyp , "\0\200") > 0 ? 0 : 1)
                {
                segmentid[0] = (unsigned char ) sql__setrang((*(struct tak_systembuffer *) sql__nil(sysp , "vak10.p" , 15819)).integer.C_51.ssysbuf[sql__setrang((int ) (*(struct tak_systembuffer *) sql__nil(sysp , "vak10.p" , 15819)).integer.C_0.b_kl + 9 , 1 , 32760) + -1] , 0 , 255);
                segmentid[1] = (unsigned char ) sql__setrang((*(struct tak_systembuffer *) sql__nil(sysp , "vak10.p" , 15821)).integer.C_51.ssysbuf[sql__setrang((int ) (*(struct tak_systembuffer *) sql__nil(sysp , "vak10.p" , 15821)).integer.C_0.b_kl + 10 , 1 , 32760) + -1] , 0 , 255);
                }
            a10del_sysinfo(&(*acv) , &(*old_key) , &(*b_err));
            }
        else
            {
            old_deleted = 0;
            }
        if ((int ) *b_err == 0)
            {
            p = find_info.fi_ptr;
            ak10find_syskey(&(*acv) , &(*new_key) , &find_info);
            if ((int ) find_info.fi_res == 1)
                {
                *b_err = 540;
                ak10store_syskey(&(*acv) , &(*new_key));
                if (!(char ) sql__setrang(old_deleted , 0 , 1))
                    {
                    ak10block_into_freelist(&(*acv) , p , 1 , 1);
                    }
                }
            else
                {
                s10mv2(sizeof(tgg00_SysInfoKey ) , sizeof(tgg00_SysInfoKey ) , &(*new_key) , 9 , &(*(struct tak_systembuffer *) sql__nil(sysp , "vak10.p" , 15852)).integer.C_50.syskey , 9 , (int ) (*new_key).skeylen);
                if (base_colptr != (struct tak_basecolinfo *) (int *) 0)
                    {
                    (*(struct tak_systembuffer *) sql__nil(sysp , "vak10.p" , 15856)).integer.C_1.sbase.breccnt = 0;
                    (*(struct tak_systembuffer *) sql__nil(sysp , "vak10.p" , 15857)).integer.C_1.sbase.bptr = base_colptr;
                    }
                else
                    {
                    if ((ptoc_Var8 = (int ) (*old_key).integer.C_2.sentrytyp[1]) == 1 || ptoc_Var8 == 177)
                        {
                        ptoc_Var9 = &(*(struct tak_systembuffer *) sql__nil(sysp , "vak10.p" , 15862)).integer.C_1.sbase;
                        ptoc_Var9->breccnt = 0;
                        ptoc_Var9->bptr = (struct tak_basecolinfo *) (int *) 0;
                        firstindex = (int ) ptoc_Var9->bfirstindex;
                        lastindex = (int ) ptoc_Var9->blastindex;
                        ptoc_Var9->blastindex = (short ) sql__setrang((short ) ((int ) ptoc_Var9->bfirstindex + -1) , -32768 , 32767);
                        ptoc_Var9->bmaxcol = 0;
                        a061add_name(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(sysp , "vak10.p" , 15870)).integer.C_1.sbase , sql__nil(ptoc_Var9->btablen , "vak10.p" , 15870) , &ptoc_Var9->btablen);
                        ptoc_Var8 = firstindex;
                        ptoc_Var10 = lastindex;
                        if (ptoc_Var8 <= ptoc_Var10)
                            {
                            ix = ptoc_Var8;
                            do
                                {
                                curr_col = ptoc_Var9->integer.C_2.bcolumn[sql__setrang(ix , 1 , 1536) + -1];
                                a061get_colname(sql__nil(curr_col , "vak10.p" , 15874) , colname);
                                a061app_columnname(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(sysp , "vak10.p" , 15876)).integer.C_1.sbase , colname , &new_index);
                                if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 15877)).integer.C_4.sp1r_returncode == 0)
                                    {
                                    a061copy_colinfo(sql__nil(curr_col , "vak10.p" , 15879) , sql__nil(ptoc_Var9->integer.C_2.bcolumn[sql__setrang(new_index , 1 , 1536) + -1] , "vak10.p" , 15880));
                                    }
                                if (ix == ptoc_Var10)
                                    {
                                    break;
                                    }
                                ix += 1;
                                }
                            while (!0);
                            }
                        }
                    }
                ak10ins_directory(&(*acv) , &find_info , &(*acv).a_sysdir.ci_dir[sql__setrang(sql__ord((int ) (*new_key).integer.C_2.sentrytyp[1]) , 1 , 195) + -1] , &p , &dummy);
                (_ptoc_MOVE(unsigned char , 1, (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 15891)).integer.C_1.cd_state, *(unsigned char (*)[1]) sql__setsub(ptoc_Var11 , (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 15891)).integer.C_1.cd_state , ptoc_Var6 , sizeof(unsigned char [1]))) , (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 15891)).integer.C_1.cd_state);
                if (_ptoc_MEMCMP(unsigned char , 2, (*new_key).integer.C_2.sentrytyp , "\0\200") < 0 ? 1 : 0)
                    {
                    (*(struct tak_systembuffer *) sql__nil(sysp , "vak10.p" , 15898)).integer.C_51.ssysbuf[sql__setrang((int ) (*new_key).skeylen + 9 , 1 , 32760) + -1] = (unsigned char ) sql__setrang(new_segment_id[0] , 0 , 255);
                    (*(struct tak_systembuffer *) sql__nil(sysp , "vak10.p" , 15900)).integer.C_51.ssysbuf[sql__setrang((int ) (*new_key).skeylen + 10 , 1 , 32760) + -1] = (unsigned char ) sql__setrang(new_segment_id[1] , 0 , 255);
                    if ((char ) sql__setrang(add_new_rec , 0 , 1))
                        {
                        a10add_sysinfo(&(*acv) , &sysp , &(*b_err));
                        }
                    }
                else
                    {
                    if ((char ) sql__setrang(add_new_rec , 0 , 1))
                        {
                        (_ptoc_MOVE(unsigned char , 1, (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 15911)).integer.C_1.cd_state, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var13 , (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 15912)).integer.C_1.cd_state , ptoc_Var7 , sizeof(unsigned char [1]))) , (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 15911)).integer.C_1.cd_state);
                        if ((ptoc_Var10 = (int ) (*(struct tak_systembuffer *) sql__nil(sysp , "vak10.p" , 15913)).integer.C_50.syskey.integer.C_2.sentrytyp[1]) == 130 || ptoc_Var10 == 129 || ptoc_Var10 == 132 || ptoc_Var10 == 178 || ptoc_Var10 == 190)
                            {
                            (*(struct tak_systembuffer *) sql__nil(sysp , "vak10.p" , 15917)).integer.C_23.sparsinfo.integer.C_1.p_fullen = (int ) (*(struct tak_systembuffer *) sql__nil(sysp , "vak10.p" , 15917)).integer.C_0.b_sl;
                            }
                        }
                    }
                }
            }
        }
    ak10verify_cache(&(*acv));
    cccprint("<a10_copy_catalo");
    }

static
  tak_sysdir_state ptoc_Var14 = 
  {
  (unsigned char)'\b'
  };


int a10cpy_result(acv, pars_syskey, res_syskey, rescnt, b_err)
tak_all_command_glob *acv;
tgg_sysinfokey *pars_syskey;
tgg_sysinfokey *res_syskey;
tsp_int4 rescnt;
tgg_basis_error *b_err;
    {
    char dummy;
    char is_fixed;
    int ix;
    tak_cache_dir_pointer p;
    tak_cache_dir_pointer dir_header;
    tak_cache_dir_pointer sysbuf;
    tak_cache_dir_pointer curr;
    tsp_int4 syslen;
    tak10_find_info find_info;
    tak_resultrecord *ptoc_Var15;
    int ptoc_Var16;
    int ptoc_Var17;
    struct tak_cache_dir_entry *ptoc_Var18;
    tak_resultrecord *ptoc_Var19;
    tak_resultrecord *ptoc_Var20;
    cccprint("AK10  a10cpy_res");
    dir_header = (*acv).a_sysdir.ci_cache_dir_header;
    is_fixed = 1;
    a10get_sysinfo(&(*acv) , &(*pars_syskey) , 1 , &sysbuf.integer.C_4.sys_ptr , &(*b_err));
    if ((int ) *b_err == 0)
        {
        ptoc_Var15 = &(*(struct tak_systembuffer *) sql__nil(sysbuf.integer.C_4.sys_ptr , "vak10.p" , 15964)).integer.C_28.sresult;
        ptoc_Var16 = ptoc_Var15->bfirstindex;
        ptoc_Var17 = ptoc_Var15->blastindex;
        if (ptoc_Var16 <= ptoc_Var17)
            {
            ix = ptoc_Var16;
            do
                {
                a061td_colinfo(sql__nil(ptoc_Var15->integer.C_2.bcolumn[sql__setrang(ix , 1 , 1536) + -1] , "vak10.p" , 15966) , ix);
                if (ix == ptoc_Var17)
                    {
                    break;
                    }
                ix += 1;
                }
            while (!0);
            }
        curr.integer.C_2.buf_ptr = s35add_bufaddr(sysbuf.integer.C_2.buf_ptr , -48);
        is_fixed = (char ) sql__setrang((*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 15971)).integer.C_1.cd_fix_next.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0 , 0 , 1);
        if (!(char ) sql__setrang(is_fixed , 0 , 1))
            {
            (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 15974)).integer.C_1.cd_fix_next = curr;
            }
        ak10find_syskey(&(*acv) , &(*res_syskey) , &find_info);
        if ((int ) find_info.fi_res == 1)
            {
            ak10store_syskey(&(*acv) , &(*res_syskey));
            *b_err = 540;
            }
        }
    if ((int ) *b_err == 0)
        {
        syslen = (int ) (*(struct tak_systembuffer *) sql__nil(sysbuf.integer.C_4.sys_ptr , "vak10.p" , 15989)).integer.C_0.b_sl + 48;
        ak10getmem(&(*acv) , -2 , &syslen , &p.integer.C_2.buf_ptr);
        if (p.integer.C_2.buf_ptr != (tsp00_Buf *) (int *) 0)
            {
            ptoc_Var18 = (struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 15993);
            ptoc_Var18->integer.C_1.cd_cmd_no = (*acv).a_command_count;
            ptoc_Var18->integer.C_1.cd_create_cmd_no = (*acv).a_command_count;
            _ptoc_MOVE(unsigned char , 1, ptoc_Var18->integer.C_1.cd_state, ptoc_Var14);
            g10mv12("VAK10 " , 2 , (int ) (*(struct tak_systembuffer *) sql__nil(sysbuf.integer.C_4.sys_ptr , "vak10.p" , 15999)).integer.C_0.b_sl , sizeof(struct tak_systembuffer ) , sql__nil(sysbuf.integer.C_4.sys_ptr , "vak10.p" , 16000) , 1 , &ptoc_Var18->integer.C_2.cd_sysinfo , 1 , (int ) (*(struct tak_systembuffer *) sql__nil(sysbuf.integer.C_4.sys_ptr , "vak10.p" , 16001)).integer.C_0.b_sl , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 16001)).integer.C_4.sp1r_returncode);
            s10mv2(sizeof(tgg00_SysInfoKey ) , sizeof(tgg00_SysInfoKey ) , &(*res_syskey) , 9 , &ptoc_Var18->integer.C_1.cd_syskey , 9 , (int ) (*res_syskey).skeylen);
            ptoc_Var19 = &ptoc_Var18->integer.C_2.cd_sysinfo.integer.C_28.sresult;
            (*(struct tak_basecolinfo *) sql__nil(ptoc_Var19->bptr , "vak10.p" , 16008)).bci_ref_cnt = (short ) sql__setrang((short ) ((int ) (*(struct tak_basecolinfo *) sql__nil(ptoc_Var19->bptr , "vak10.p" , 16008)).bci_ref_cnt + 1) , -32768 , 32767);
            if (rescnt != -1)
                {
                ptoc_Var19->brows = rescnt;
                rescnt = -1;
                if ((int ) ptoc_Var19->bmaxreclen > 6476)
                    {
                    if (rescnt == 0)
                        {
                        ptoc_Var19->bpages = 1;
                        }
                    else
                        {
                        ptoc_Var19->bpages = rescnt;
                        }
                    }
                else
                    {
                    ptoc_Var19->bpages = ptoc_Var19->brows / (6476 / (int ) ptoc_Var19->bmaxreclen) + 1;
                    }
                }
            t01buf(0 , &ptoc_Var18->integer.C_2.cd_sysinfo , 1 , (int ) ptoc_Var18->integer.C_2.cd_sysinfo.integer.C_0.b_sl);
            ptoc_Var20 = &ptoc_Var18->integer.C_2.cd_sysinfo.integer.C_28.sresult;
            ptoc_Var17 = ptoc_Var20->bfirstindex;
            ptoc_Var16 = ptoc_Var20->blastindex;
            if (ptoc_Var17 <= ptoc_Var16)
                {
                ix = ptoc_Var17;
                do
                    {
                    a061td_colinfo(sql__nil(ptoc_Var20->integer.C_2.bcolumn[sql__setrang(ix , 1 , 1536) + -1] , "vak10.p" , 16033) , ix);
                    if (ix == ptoc_Var16)
                        {
                        break;
                        }
                    ix += 1;
                    }
                while (!0);
                }
            ak10ins_directory(&(*acv) , &find_info , &(*acv).a_sysdir.ci_dir[sql__setrang(sql__ord((int ) (*res_syskey).integer.C_2.sentrytyp[1]) , 1 , 195) + -1] , &p , &dummy);
            if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 16039)).integer.C_4.sp1r_returncode == 0)
                {
                ptoc_Var18->integer.C_1.cd_lru_next = (*(struct tak_cache_dir_entry *) sql__nil(dir_header.integer.C_1.cdir_ptr , "vak10.p" , 16044)).integer.C_1.cd_lru_next;
                ptoc_Var18->integer.C_1.cd_lru_prev = dir_header;
                (*(struct tak_cache_dir_entry *) sql__nil((*(struct tak_cache_dir_entry *) sql__nil(dir_header.integer.C_1.cdir_ptr , "vak10.p" , 16046)).integer.C_1.cd_lru_next.integer.C_1.cdir_ptr , "vak10.p" , 16046)).integer.C_1.cd_lru_prev = p;
                (*(struct tak_cache_dir_entry *) sql__nil(dir_header.integer.C_1.cdir_ptr , "vak10.p" , 16048)).integer.C_1.cd_lru_next = p;
                (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 16049)).integer.C_1.cd_fix_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
                }
            }
        }
    if (!(char ) sql__setrang(is_fixed , 0 , 1))
        {
        (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 16059)).integer.C_1.cd_fix_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
        }
    cccprint("<a10cpy_result  ");
    }



int a10DeallocTemp(acv)
tak_all_command_glob *acv;
    {
    tak_cache_dir_pointer next;
    tak_cache_dir_pointer curr;
    tak_sysdirectory *ptoc_Var21;
    cccprint("AK10  a10Dealloc");
    ptoc_Var21 = &(*acv).a_sysdir;
    if (ptoc_Var21->ci_cache_p != (tsp00_Buf *) (int *) 0)
        {
        next = (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var21->ci_cache_dir_trailor.integer.C_1.cdir_ptr , "vak10.p" , 16083)).integer.C_1.cd_lru_prev;
        while (next.integer.C_1.cdir_ptr != ptoc_Var21->ci_cache_dir_header.integer.C_1.cdir_ptr)
            {
            curr = next;
            next = (*(struct tak_cache_dir_entry *) sql__nil(next.integer.C_1.cdir_ptr , "vak10.p" , 16087)).integer.C_1.cd_lru_prev;
            if (_ptoc_MEMCMP(unsigned char , 2, (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 16088)).integer.C_1.cd_syskey.integer.C_2.sentrytyp , "\0\200") > 0 ? 1 : 0)
                {
                ak10block_into_freelist(&(*acv) , curr , 1 , 1);
                }
            }
        ak10td_dirlist(&(*acv) , 0);
        ak10td_freelist(&(*acv) , 0);
        }
    cccprint("<a10DeallocTemp ");
    }

static
  unsigned char ptoc_Var22[2] = 
  {
  (unsigned char)'\0', (unsigned char)'\001'
  };
static
  unsigned char ptoc_Var23[2] = 
  {
  (unsigned char)'\001', (unsigned char)'\0'
  };
static
  tsp00_WarningSet ptoc_Var24 = 
  {
  (unsigned char)'\0', (unsigned char)'\0'
  };


int a10_del_tab_sysinfo(acv, tableid, qual, temp_table, b_err)
tak_all_command_glob *acv;
tgg_surrogate tableid;
tak_del_tab_qual *qual;
char temp_table;
tgg_basis_error *b_err;
    {
    char send_mess;
    int i;
    int cnt_in;
    int systemfileno;
    tak_sm_description sm_desc;
    tgg_sysinfokey sysk;
    tgg00_StackEntry stack[30];
    tgg_mess_block mblock;
    tak10qual qual_buf;
    struct ptoc_Type192
      {
      union  
        {
        struct  
          {
          tgg_surrogate dt_tabid;
          } C_1;
        struct  
          {
          tgg_serverdb_no dt_site;
          } C_2;
        struct  
          {
          tsp_buf buf;
          } C_3;
        } integer;
      } data;
    tak_all_command_glob *ptoc_Var25;
    struct tgg00_MessBlock *ptoc_Var26;
    tak_sm_description *ptoc_Var27;
    tgg00_QualBuf *ptoc_Var28;
    tgg00_StackEntry *ptoc_Var29;
    tgg00_StackEntry *ptoc_Var30;
    int ptoc_Var31;
    unsigned char ptoc_Var32[2];
    unsigned char ptoc_Var33[2];
    unsigned char ptoc_Var34[2];
    cccprint("AK10  a10_del_ta");
    ptoc_Var25 = &*acv;
    ptoc_Var26 = &mblock;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var25->a_return_segm , "vak10.p" , 16139)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var27 = &sm_desc;
        ptoc_Var27->sm_error_cnt = 1;
        ptoc_Var27->sm_errorlist[0] = 1200;
        ptoc_Var27->sm_return_req = 0;
        ptoc_Var27->sm_must_local = 0;
        ptoc_Var27->sm_not_local = 0;
        systemfileno = 1;
        *b_err = 0;
        do
            {
            send_mess = 1;
            g01mblock_init(&(*acv).a_transinf.tri_trans , 18 , 54 , &mblock);
            g01stack_init(&mblock , (tgg00_StackList *) &stack[0] , sizeof(tgg00_StackEntry [30]) , (tgg00_QualBuf *) &qual_buf , sizeof(tak10qual ));
            ptoc_Var26->integer.C_2.mb_qual_len = sizeof(unsigned char [112]);
            g01datapart_init(&mblock , (tgg00_DataPart *) &data , sizeof(struct ptoc_Type192 ));
            g01stratpart_init(&mblock , ptoc_Var25->a_mb_strat_addr , ptoc_Var25->a_mb_strat_size);
            ptoc_Var28 = (tgg00_QualBuf *) sql__nil(ptoc_Var26->integer.C_4.mb_qual , "vak10.p" , 16162);
            if ((char ) sql__setrang(temp_table , 0 , 1))
                {
                ptoc_Var28->integer.C_2.mtree = ptoc_Var25->a_pars_curr;
                }
            else
                {
                if (systemfileno == 1)
                    {
                    ptoc_Var28->integer.C_2.mtree = g01tabid.sys1_cat;
                    }
                else
                    {
                    ptoc_Var28->integer.C_2.mtree = g01tabid.sys2_cat;
                    }
                }
            ptoc_Var28->integer.C_2.mqual_pos = 1;
            ptoc_Var28->integer.C_2.mqual_cnt = 2;
            _ptoc_ASSIGN(unsigned char , 8, data.integer.C_1.dt_tabid, tableid)
            ptoc_Var26->integer.C_2.mb_data_len = sizeof(tsp00_C8 );
            ptoc_Var29 = &(*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16181))[0];
            ptoc_Var29->integer.C_1.etype = 0;
            ptoc_Var29->integer.C_1.eop = 0;
            ptoc_Var29->integer.C_1.epos = 1;
            ptoc_Var29->integer.C_1.elen_var = 8;
            ptoc_Var29->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
            ptoc_Var29->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
            ptoc_Var30 = &(*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16191))[1];
            ptoc_Var30->integer.C_1.etype = 22;
            ptoc_Var30->integer.C_1.eop = 110;
            ptoc_Var30->integer.C_1.epos = 1;
            ptoc_Var30->integer.C_1.elen_var = 8;
            _ptoc_ASSIGN(unsigned char , 2, ptoc_Var30->integer.C_1.ecol_tab, "\0\0")
            if ((int ) (*qual).del_colno > 0)
                {
                (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16203))[2] = (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16203))[0];
                (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16204))[2].integer.C_1.epos = 11;
                (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16205))[2].integer.C_1.elen_var = 2;
                (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16206))[3] = (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16206))[1];
                (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16207))[3].integer.C_1.epos = 9;
                (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16208))[3].integer.C_1.elen_var = 2;
                (*(tgg00_DataPart *) sql__nil(ptoc_Var26->integer.C_3.mb_data , "vak10.p" , 16209)).integer.C_1.mbp_buf[8] = (unsigned char ) sql__setrang((unsigned char ) ((int ) (*qual).del_colno / 256) , 0 , 255);
                (*(tgg00_DataPart *) sql__nil(ptoc_Var26->integer.C_3.mb_data , "vak10.p" , 16211)).integer.C_1.mbp_buf[9] = (unsigned char ) sql__setrang((unsigned char ) ((ptoc_Var31 = (int ) (*qual).del_colno % 256) >= 0 ? ptoc_Var31 : ptoc_Var31 + 256) , 0 , 255);
                ptoc_Var26->integer.C_2.mb_data_len = 10;
                (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16214))[4] = (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16214))[2];
                (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16215))[4].integer.C_1.epos = 9;
                ptoc_Var28->integer.C_2.mfirst_free = 6;
                cnt_in = 0;
                ptoc_Var31 = (*qual).del_qual_cnt;
                if (1 <= ptoc_Var31)
                    {
                    i = 1;
                    do
                        {
                        if (systemfileno == 1 && (_ptoc_MEMCMP(unsigned char , 2, (*qual).del_qual[sql__setrang(i , 1 , 16) + -1] , "\0@") < 0 ? 1 : 0) || systemfileno == 2 && (_ptoc_MEMCMP(unsigned char , 2, (*qual).del_qual[sql__setrang(i , 1 , 16) + -1] , "\0@") > 0 ? 1 : 0))
                            {
                            cnt_in = sql__succ(cnt_in , -2147483647 , 2147483647 , "vak10.p" , 16225);
                            (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16226))[sql__setrang((int ) ptoc_Var28->integer.C_2.mfirst_free , 1 , 32767) + -1] = (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16226))[3];
                            (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16227))[sql__setrang((int ) ptoc_Var28->integer.C_2.mfirst_free , 1 , 32767) + -1].integer.C_1.epos = (short ) sql__setrang((short ) (ptoc_Var26->integer.C_2.mb_data_len + 1) , -32768 , 32767);
                            (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16228))[sql__setrang((int ) ptoc_Var28->integer.C_2.mfirst_free , 1 , 32767) + -1].integer.C_1.eop = 0;
                            (*(tgg00_DataPart *) sql__nil(ptoc_Var26->integer.C_3.mb_data , "vak10.p" , 16229)).integer.C_1.mbp_buf[sql__setrang(ptoc_Var26->integer.C_2.mb_data_len + 1 , 1 , 8192000) + -1] = (unsigned char ) sql__setrang((*qual).del_qual[sql__setrang(i , 1 , 16) + -1][0] , 0 , 255);
                            (*(tgg00_DataPart *) sql__nil(ptoc_Var26->integer.C_3.mb_data , "vak10.p" , 16231)).integer.C_1.mbp_buf[sql__setrang(ptoc_Var26->integer.C_2.mb_data_len + 2 , 1 , 8192000) + -1] = (unsigned char ) sql__setrang((*qual).del_qual[sql__setrang(i , 1 , 16) + -1][1] , 0 , 255);
                            ptoc_Var26->integer.C_2.mb_data_len = ptoc_Var26->integer.C_2.mb_data_len + 2;
                            ptoc_Var28->integer.C_2.mfirst_free = (short ) sql__setrang((short ) ((int ) ptoc_Var28->integer.C_2.mfirst_free + 1) , -32768 , 32767);
                            }
                        if (i == ptoc_Var31)
                            {
                            break;
                            }
                        i += 1;
                        }
                    while (!0);
                    }
                (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16238))[sql__setrang((int ) ptoc_Var28->integer.C_2.mfirst_free , 1 , 32767) + -1].integer.C_1.etype = 26;
                (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16239))[sql__setrang((int ) ptoc_Var28->integer.C_2.mfirst_free , 1 , 32767) + -1].integer.C_1.eop = 11;
                (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16240))[sql__setrang((int ) ptoc_Var28->integer.C_2.mfirst_free , 1 , 32767) + -1].integer.C_1.epos = 1;
                (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16241))[sql__setrang((int ) ptoc_Var28->integer.C_2.mfirst_free , 1 , 32767) + -1].integer.C_1.elen_var = (short ) sql__setrang((short ) cnt_in , -32768 , 32767);
                (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16242))[sql__setrang((int ) ptoc_Var28->integer.C_2.mfirst_free , 1 , 32767) + -1].integer.C_2.ecol_pos = 0;
                if (cnt_in == (int ) (*qual).del_qual_cnt)
                    {
                    systemfileno = 2;
                    }
                else
                    {
                    if (cnt_in == 0)
                        {
                        send_mess = 0;
                        }
                    }
                (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16252))[sql__setrang((int ) ptoc_Var28->integer.C_2.mfirst_free + 1 , 1 , 32767) + -1] = (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16252))[sql__setrang((int ) ptoc_Var28->integer.C_2.mfirst_free , 1 , 32767) + -1];
                (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16253))[sql__setrang((int ) ptoc_Var28->integer.C_2.mfirst_free + 1 , 1 , 32767) + -1].integer.C_1.eop = 98;
                (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16254))[sql__setrang((int ) ptoc_Var28->integer.C_2.mfirst_free + 2 , 1 , 32767) + -1] = (*(tgg00_StackList *) sql__nil(ptoc_Var26->integer.C_5.mb_st , "vak10.p" , 16254))[sql__setrang((int ) ptoc_Var28->integer.C_2.mfirst_free + 1 , 1 , 32767) + -1];
                ptoc_Var28->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) ((int ) ptoc_Var28->integer.C_2.mfirst_free + 2) , -32768 , 32767);
                }
            if ((char ) sql__setrang(send_mess , 0 , 1))
                {
                ptoc_Var28->integer.C_2.mfirst_free = (short ) sql__setrang((short ) ((int ) ptoc_Var28->integer.C_2.mqual_cnt + 1) , -32768 , 32767);
                a71del_sysinfo_strategy(140 , &mblock);
                a06ignore_rsend_mess_buf(&(*acv) , &mblock , &sm_desc , &(*b_err));
                }
            systemfileno = sql__succ(systemfileno , -2147483647 , 2147483647 , "vak10.p" , 16269);
            }
        while (!((int ) *b_err != 0 || systemfileno > 2 || (char ) sql__setrang(temp_table , 0 , 1)));
        _ptoc_MOVE(unsigned char , 2, ptoc_Var25->a_transinf.tri_trans.trWarning_gg00, *(unsigned char (*)[2]) sql__setsub(ptoc_Var32 , ptoc_Var25->a_transinf.tri_trans.trWarning_gg00 , ptoc_Var22 , sizeof(unsigned char [2])));
        if (sql__erq(sizeof(unsigned char [2]) , ptoc_Var25->a_transinf.tri_trans.trWarning_gg00 , ptoc_Var23))
            {
            _ptoc_MOVE(unsigned char , 2, ptoc_Var25->a_transinf.tri_trans.trWarning_gg00, ptoc_Var24);
            }
        if ((int ) (*qual).del_colno == 0)
            {
            ak10table_cache_delete(&(*acv) , tableid , 0);
            }
        else
            {
            sysk = a01defaultkey;
            _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_2.stableid, tableid)
            sysk.integer.C_2.slinkage[0] = (unsigned char ) sql__setrang((unsigned char ) ((int ) (*qual).del_colno / 256) , 0 , 255);
            sysk.integer.C_2.slinkage[1] = (unsigned char ) sql__setrang((unsigned char ) ((ptoc_Var31 = (int ) (*qual).del_colno % 256) >= 0 ? ptoc_Var31 : ptoc_Var31 + 256) , 0 , 255);
            ptoc_Var31 = (*qual).del_qual_cnt;
            if (1 <= ptoc_Var31)
                {
                i = 1;
                do
                    {
                    _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, (*qual).del_qual[sql__setrang(i , 1 , 16) + -1])
                    a10_key_del(&(*acv) , &sysk);
                    if (i == ptoc_Var31)
                        {
                        break;
                        }
                    i += 1;
                    }
                while (!0);
                }
            }
        }
    else
        {
        *b_err = 400;
        }
    cccprint("<a10_del_tab_sys");
    }

static
  unsigned char ptoc_Var35[2] = 
  {
  (unsigned char)'\0', (unsigned char)'\001'
  };
static
  unsigned char ptoc_Var36[2] = 
  {
  (unsigned char)'\001', (unsigned char)'\0'
  };
static
  tsp00_WarningSet ptoc_Var37 = 
  {
  (unsigned char)'\0', (unsigned char)'\0'
  };


int a10del_user_sysinfo(acv, auth, b_err)
tak_all_command_glob *acv;
tgg_surrogate auth;
tgg_basis_error *b_err;
    {
    int ix;
    int st_pos;
    int systemfileno;
    tsp00_C2 in_list[5];
    tak_sm_description sm_desc;
    tgg00_StackEntry stack[30];
    tgg_mess_block mblock;
    tak10qual qual;
    struct ptoc_Type193
      {
      union  
        {
        struct  
          {
          tsp_c2 dt_epriv;
          tsp_c2 dt_link;
          tgg_surrogate dt_userid;
          tsp_c2 dt_eprivuser;
          tsp_c2 dt_link2;
          tgg_surrogate dt_userid2;
          } C_true;
        struct  
          {
          tsp_buf dt_buf;
          } C_false;
        } boolean;
      } data;
    tak_all_command_glob *ptoc_Var38;
    struct tgg00_MessBlock *ptoc_Var39;
    tak_sm_description *ptoc_Var40;
    tgg00_StackEntry *ptoc_Var41;
    tgg00_StackEntry *ptoc_Var42;
    tgg00_StackEntry *ptoc_Var43;
    tgg00_StackEntry *ptoc_Var44;
    tgg00_StackEntry *ptoc_Var45;
    tgg00_StackEntry *ptoc_Var46;
    tgg00_StackEntry *ptoc_Var47;
    tgg00_StackEntry *ptoc_Var48;
    tgg00_StackEntry *ptoc_Var49;
    tgg00_StackEntry *ptoc_Var50;
    unsigned char ptoc_Var51[2];
    unsigned char ptoc_Var52[2];
    unsigned char ptoc_Var53[2];
    cccprint("AK10  a10del_use");
    ptoc_Var38 = &*acv;
    ptoc_Var39 = &mblock;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var38->a_return_segm , "vak10.p" , 16346)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var40 = &sm_desc;
        ptoc_Var40->sm_error_cnt = 1;
        ptoc_Var40->sm_errorlist[0] = 1200;
        ptoc_Var40->sm_return_req = 0;
        ptoc_Var40->sm_must_local = 0;
        ptoc_Var40->sm_not_local = 0;
        systemfileno = 1;
        do
            {
            g01mblock_init(&(*acv).a_transinf.tri_trans , 18 , 54 , &mblock);
            g01stack_init(&mblock , (tgg00_StackList *) &stack[0] , sizeof(tgg00_StackEntry [30]) , (tgg00_QualBuf *) &qual , sizeof(tak10qual ));
            g01datapart_init(&mblock , (tgg00_DataPart *) &data , sizeof(struct ptoc_Type193 ));
            g01stratpart_init(&mblock , ptoc_Var38->a_mb_strat_addr , ptoc_Var38->a_mb_strat_size);
            if (systemfileno == 1)
                {
                (*(tgg00_QualBuf *) sql__nil(ptoc_Var39->integer.C_4.mb_qual , "vak10.p" , 16368)).integer.C_2.mtree = g01tabid.sys1_cat;
                }
            else
                {
                (*(tgg00_QualBuf *) sql__nil(ptoc_Var39->integer.C_4.mb_qual , "vak10.p" , 16370)).integer.C_2.mtree = g01tabid.sys2_cat;
                }
            (*(tgg00_QualBuf *) sql__nil(ptoc_Var39->integer.C_4.mb_qual , "vak10.p" , 16372)).integer.C_2.mqual_pos = 1;
            _ptoc_ASSIGN(unsigned char , 2, data.boolean.C_true.dt_epriv, "\0\006")
            _ptoc_ASSIGN(unsigned char , 2, data.boolean.C_true.dt_link, "\0\001")
            _ptoc_ASSIGN(unsigned char , 8, data.boolean.C_true.dt_userid, auth)
            ptoc_Var39->integer.C_2.mb_data_len = sizeof(tsp00_C2 ) + sizeof(tsp00_C2 ) + sizeof(tsp00_C8 );
            if (systemfileno == 1)
                {
                _ptoc_ASSIGN(unsigned char , 2, data.boolean.C_true.dt_eprivuser, "\0\007")
                _ptoc_ASSIGN(unsigned char , 2, data.boolean.C_true.dt_link2, "\0\001")
                _ptoc_ASSIGN(unsigned char , 8, data.boolean.C_true.dt_userid2, auth)
                ptoc_Var39->integer.C_2.mb_data_len = ptoc_Var39->integer.C_2.mb_data_len + sizeof(tsp00_C2 ) + sizeof(tsp00_C2 ) + sizeof(tsp00_C8 );
                ptoc_Var41 = &(*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16387))[0];
                ptoc_Var41->integer.C_1.etype = 0;
                ptoc_Var41->integer.C_1.eop = 0;
                ptoc_Var41->integer.C_1.epos = 9;
                ptoc_Var41->integer.C_1.elen_var = (short ) sql__setrang((short ) (sizeof(tsp00_C2 ) + sizeof(tsp00_C2 ) + 8) , -32768 , 32767);
                ptoc_Var41->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                ptoc_Var41->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                ptoc_Var42 = &(*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16398))[1];
                ptoc_Var42->integer.C_1.etype = 22;
                ptoc_Var42->integer.C_1.eop = 110;
                ptoc_Var42->integer.C_1.epos = 1;
                ptoc_Var42->integer.C_1.elen_var = (short ) sql__setrang((short ) (sizeof(tsp00_C2 ) + sizeof(tsp00_C2 ) + 8) , -32768 , 32767);
                _ptoc_ASSIGN(unsigned char , 2, ptoc_Var42->integer.C_1.ecol_tab, "\0\0")
                ptoc_Var43 = &(*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16408))[2];
                ptoc_Var43->integer.C_1.etype = 31;
                ptoc_Var43->integer.C_1.eop = 0;
                ptoc_Var43->integer.C_1.epos = 4;
                ptoc_Var43->integer.C_1.elen_var = 0;
                ptoc_Var43->integer.C_2.ecol_pos = 0;
                (*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16417))[3] = (*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16417))[0];
                (*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16418))[4] = (*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16418))[1];
                (*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16419))[4].integer.C_1.epos = (short ) sql__setrang((short ) (sizeof(tsp00_C2 ) + sizeof(tsp00_C2 ) + 9) , -32768 , 32767);
                ptoc_Var44 = &(*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16421))[5];
                ptoc_Var44->integer.C_1.etype = 26;
                ptoc_Var44->integer.C_1.eop = 99;
                ptoc_Var44->integer.C_1.epos = 0;
                ptoc_Var44->integer.C_1.elen_var = 0;
                ptoc_Var44->integer.C_2.ecol_pos = 0;
                (*(tgg00_QualBuf *) sql__nil(ptoc_Var39->integer.C_4.mb_qual , "vak10.p" , 16430)).integer.C_2.mqual_cnt = 6;
                (*(tgg00_QualBuf *) sql__nil(ptoc_Var39->integer.C_4.mb_qual , "vak10.p" , 16431)).integer.C_2.mfirst_free = 7;
                a71add_default_strat(&mblock);
                }
            else
                {
                ptoc_Var45 = &(*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16436))[0];
                ptoc_Var45->integer.C_1.etype = 0;
                ptoc_Var45->integer.C_1.eop = 0;
                ptoc_Var45->integer.C_1.epos = 1;
                ptoc_Var45->integer.C_1.elen_var = 8;
                ptoc_Var45->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
                ptoc_Var45->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
                ptoc_Var46 = &(*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16446))[1];
                ptoc_Var46->integer.C_1.etype = 22;
                ptoc_Var46->integer.C_1.eop = 110;
                ptoc_Var46->integer.C_1.epos = (short ) sql__setrang((short ) (sizeof(tsp00_C2 ) + sizeof(tsp00_C2 ) + 1) , -32768 , 32767);
                ptoc_Var46->integer.C_1.elen_var = 8;
                _ptoc_ASSIGN(unsigned char , 2, ptoc_Var46->integer.C_1.ecol_tab, "\0\0")
                ptoc_Var47 = &(*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16456))[2];
                ptoc_Var47->integer.C_1.etype = 32;
                ptoc_Var47->integer.C_1.eop = 0;
                ptoc_Var47->integer.C_1.epos = 9;
                ptoc_Var47->integer.C_1.elen_var = 0;
                ptoc_Var47->integer.C_2.ecol_pos = 0;
                (*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16465))[3] = (*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16465))[0];
                ptoc_Var48 = &(*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16466))[3];
                ptoc_Var48->integer.C_1.epos = 9;
                ptoc_Var48->integer.C_1.elen_var = 2;
                st_pos = 4;
                _ptoc_ASSIGN(unsigned char , 2, in_list[0], "\0A")
                _ptoc_ASSIGN(unsigned char , 2, in_list[1], "\0B")
                _ptoc_ASSIGN(unsigned char , 2, in_list[2], "\0G")
                _ptoc_ASSIGN(unsigned char , 2, in_list[3], "\0R")
                _ptoc_ASSIGN(unsigned char , 2, in_list[4], "\0`")
                ix = 1;
                do
                    {
                    st_pos = st_pos + 1;
                    (*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16481))[sql__setrang(st_pos , 1 , 32767) + -1] = (*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16481))[1];
                    ptoc_Var49 = &(*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16482))[sql__setrang(st_pos , 1 , 32767) + -1];
                    ptoc_Var49->integer.C_1.eop = 0;
                    ptoc_Var49->integer.C_1.epos = (short ) sql__setrang((short ) (ptoc_Var39->integer.C_2.mb_data_len + 1) , -32768 , 32767);
                    ptoc_Var49->integer.C_1.elen_var = 2;
                    (*(tgg00_DataPart *) sql__nil(ptoc_Var39->integer.C_3.mb_data , "vak10.p" , 16489)).integer.C_1.mbp_buf[sql__setrang(ptoc_Var39->integer.C_2.mb_data_len + 1 , 1 , 8192000) + -1] = (unsigned char ) sql__setrang(in_list[sql__setrang(ix , 1 , 5) + -1][0] , 0 , 255);
                    (*(tgg00_DataPart *) sql__nil(ptoc_Var39->integer.C_3.mb_data , "vak10.p" , 16490)).integer.C_1.mbp_buf[sql__setrang(ptoc_Var39->integer.C_2.mb_data_len + 2 , 1 , 8192000) + -1] = (unsigned char ) sql__setrang(in_list[sql__setrang(ix , 1 , 5) + -1][1] , 0 , 255);
                    ptoc_Var39->integer.C_2.mb_data_len = ptoc_Var39->integer.C_2.mb_data_len + 2;
                    if (ix == 5)
                        {
                        break;
                        }
                    ix += 1;
                    }
                while (!0);
                st_pos = st_pos + 1;
                ptoc_Var50 = &(*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16495))[sql__setrang(st_pos , 1 , 32767) + -1];
                ptoc_Var50->integer.C_1.etype = 26;
                ptoc_Var50->integer.C_1.eop = 11;
                ptoc_Var50->integer.C_1.epos = 1;
                ptoc_Var50->integer.C_1.elen_var = 5;
                ptoc_Var50->integer.C_2.ecol_pos = 0;
                st_pos = st_pos + 1;
                (*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16505))[sql__setrang(st_pos , 1 , 32767) + -1] = (*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16505))[sql__setrang(st_pos + -1 , 1 , 32767) + -1];
                (*(tgg00_StackList *) sql__nil(ptoc_Var39->integer.C_5.mb_st , "vak10.p" , 16506))[sql__setrang(st_pos , 1 , 32767) + -1].integer.C_1.eop = 98;
                (*(tgg00_QualBuf *) sql__nil(ptoc_Var39->integer.C_4.mb_qual , "vak10.p" , 16507)).integer.C_2.mqual_cnt = (short ) sql__setrang((short ) st_pos , -32768 , 32767);
                (*(tgg00_QualBuf *) sql__nil(ptoc_Var39->integer.C_4.mb_qual , "vak10.p" , 16508)).integer.C_2.mfirst_free = (short ) sql__setrang((short ) (st_pos + 1) , -32768 , 32767);
                a71del_sysinfo_strategy(140 , &mblock);
                }
            a06ignore_rsend_mess_buf(&(*acv) , &mblock , &sm_desc , &(*b_err));
            systemfileno = sql__succ(systemfileno , -2147483647 , 2147483647 , "vak10.p" , 16514);
            }
        while (!((int ) *b_err != 0 || systemfileno > 2));
        _ptoc_MOVE(unsigned char , 2, ptoc_Var38->a_transinf.tri_trans.trWarning_gg00, *(unsigned char (*)[2]) sql__setsub(ptoc_Var51 , ptoc_Var38->a_transinf.tri_trans.trWarning_gg00 , ptoc_Var35 , sizeof(unsigned char [2])));
        if (sql__erq(sizeof(unsigned char [2]) , ptoc_Var38->a_transinf.tri_trans.trWarning_gg00 , ptoc_Var36))
            {
            _ptoc_MOVE(unsigned char , 2, ptoc_Var38->a_transinf.tri_trans.trWarning_gg00, ptoc_Var37);
            }
        }
    else
        {
        *b_err = 400;
        }
    cccprint("<a10del_user_sys");
    }



int a10dispose(acv, p)
tak_all_command_glob *acv;
tak_sysbufferaddress *p;
    {
    tak_cache_dir_pointer curr;
    tgg_mess_block_ptr mblock_ptr;
    cccprint("AK10  a10dispose");
    curr.integer.C_4.sys_ptr = *p;
    curr.integer.C_2.buf_ptr = s35add_bufaddr(curr.integer.C_2.buf_ptr , -48);
    t01int4(0 , "pos         " , (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 16547)).integer.C_1.cd_pos);
    t01int4(0 , "mem_type    " , sql__ord((int ) (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 16548)).integer.C_2.cd_mem_type));
    t01addr(0 , "ADDR        " , curr.integer.C_2.buf_ptr);
    if ((int ) (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 16550)).integer.C_2.cd_mem_type == 13)
        {
        mblock_ptr = (struct tgg00_MessBlock *) &(*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 16556)).integer.C_2.cd_sysinfo;
        ak10free_mblock(&(*acv) , sql__nil(mblock_ptr , "vak10.p" , 16557));
        }
    (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 16560)).integer.C_1.cd_lru_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
    (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 16561)).integer.C_1.cd_fix_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
    ak10block_into_freelist(&(*acv) , curr , 0 , 1);
    *p = (struct tak_systembuffer *) (int *) 0;
    cccprint("<a10dispose     ");
    }



int a10find_result(acv, syskey, prefix_len, syspoint, b_err)
tak_all_command_glob *acv;
tgg_sysinfokey *syskey;
int prefix_len;
tak_sysbufferaddress *syspoint;
tgg_basis_error *b_err;
    {
    tsp_lcomp_result res;
    tak_cache_dir_pointer curr;
    struct tak_cache_dir_entry *ptoc_Var54;
    cccprint("AK10  a10find_re");
    t01buf1(0 , &(*syskey) , 9 , (int ) (*syskey).skeylen + 8);
    *b_err = 1420;
    curr = (*acv).a_sysdir.ci_dir[sql__setrang(sql__ord((int ) (*syskey).integer.C_2.sentrytyp[1]) , 1 , 195) + -1];
    while (curr.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        ptoc_Var54 = (struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 16585);
        if (ptoc_Var54->integer.C_1.cd_pos + ptoc_Var54->integer.C_1.cd_check != 0)
            {
            ak10cache_error(&(*acv) , "a10find_result          " , curr);
            curr.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
            }
        else
            {
            s30cmp(&(*syskey) , 9 , prefix_len , &ptoc_Var54->integer.C_1.cd_syskey , 9 , prefix_len , &res);
            if ((int ) res == 1)
                {
                if (prefix_len == 4 && (int ) ptoc_Var54->integer.C_1.cd_syskey.integer.C_2.stableid[4] == 0 && (int ) ptoc_Var54->integer.C_1.cd_syskey.integer.C_2.stableid[5] == 0)
                    {
                    curr = (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 16609)).integer.C_1.cd_greater;
                    }
                else
                    {
                    *syskey = ptoc_Var54->integer.C_1.cd_syskey;
                    _ptoc_ASSIGN(unsigned char , 2, (*syskey).integer.C_2.slinkage, "\0\001")
                    a10get_sysinfo(&(*acv) , &(*syskey) , 0 , &(*syspoint) , &(*b_err));
                    curr.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
                    }
                }
            else
                {
                if ((int ) res == 2)
                    {
                    curr = (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 16625)).integer.C_1.cd_greater;
                    }
                else
                    {
                    curr = (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 16627)).integer.C_1.cd_less;
                    }
                }
            }
        }
    if ((int ) *b_err == 1420)
        {
        a10next_sysinfo(&(*acv) , &(*syskey) , prefix_len , 0 , (*syskey).integer.C_2.sentrytyp , &(*syspoint) , &(*b_err));
        }
    cccprint("<a10find_result ");
    }

static
  unsigned char ptoc_Var55[1] = 
  {
  (unsigned char)'@'
  };


int a10_fix_len_get_sysinfo(acv, syskey, dstate, required_len, plus, syspoint, b_err)
tak_all_command_glob *acv;
tgg_sysinfokey *syskey;
tak_directory_state dstate;
int required_len;
int plus;
tak_sysbufferaddress *syspoint;
tgg_basis_error *b_err;
    {
    char dummy;
    char rel_sysinfo;
    tak_cache_dir_pointer fix_next;
    tak_cache_dir_pointer fix_prev;
    tak_cache_dir_pointer p;
    tak_cache_dir_e_ptr old_entry;
    tsp_int4 req_len;
    tak10_find_info find_info;
    int ptoc_Var56;
    tak_sysdirectory *ptoc_Var57;
    struct tak_cache_dir_entry *ptoc_Var58;
    unsigned char ptoc_Var59[1];
    unsigned char ptoc_Var60[1];
    unsigned char ptoc_Var61[1];
    cccprint("AK10  a10_fix_le");
    t01int4(0 , "required_len" , required_len);
    t01int4(0 , "plus        " , plus);
    a10get_sysinfo(&(*acv) , &(*syskey) , (unsigned char ) sql__setrang(dstate , 0 , 1) , &(*syspoint) , &(*b_err));
    if ((int ) *b_err == 0)
        {
        req_len = required_len;
        if (plus <= 0)
            {
            plus = req_len - (int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 16672)).integer.C_0.b_sl;
            }
        if ((int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 16674)).integer.C_0.b_sl + plus > sizeof(tsp00_Buf ) && ((ptoc_Var56 = (int ) (*syskey).integer.C_2.sentrytyp[1]) != 192 && ptoc_Var56 != 130 && ptoc_Var56 != 129 && ptoc_Var56 != 132 && ptoc_Var56 != 178 && ptoc_Var56 != 190))
            {
            plus = sizeof(tsp00_Buf ) - (int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 16679)).integer.C_0.b_sl;
            }
        if (plus > 0)
            {
            ak10find_syskey(&(*acv) , &(*syskey) , &find_info);
            ptoc_Var57 = &(*acv).a_sysdir;
            if ((*(struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 16686)).integer.C_1.cd_syslen < (int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 16687)).integer.C_0.b_sl + plus + 48)
                {
                xtd_directory(0 , find_info.fi_ptr);
                old_entry = find_info.fi_ptr.integer.C_1.cdir_ptr;
                fix_next = (*(struct tak_cache_dir_entry *) sql__nil(old_entry , "vak10.p" , 16692)).integer.C_1.cd_fix_next;
                fix_prev = (*(struct tak_cache_dir_entry *) sql__nil(old_entry , "vak10.p" , 16693)).integer.C_1.cd_fix_prev;
                if (fix_next.integer.C_1.cdir_ptr == (struct tak_cache_dir_entry *) (int *) 0)
                    {
                    a10get_sysinfo(&(*acv) , &(*syskey) , 0 , &(*syspoint) , &(*b_err));
                    rel_sysinfo = (char ) sql__setrang((int ) dstate == 1 , 0 , 1);
                    }
                else
                    {
                    rel_sysinfo = 0;
                    }
                req_len = (int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 16706)).integer.C_0.b_sl + plus + 48;
                if ((ptoc_Var56 = (int ) (*syskey).integer.C_2.sentrytyp[1]) == 192 || ptoc_Var56 == 130 || ptoc_Var56 == 129 || ptoc_Var56 == 132 || ptoc_Var56 == 178 || ptoc_Var56 == 190)
                    {
                    ak10size_check(&(*acv) , (*syskey).integer.C_2.sentrytyp , (int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 16712)).integer.C_0.b_sl + plus);
                    }
                if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 16714)).integer.C_4.sp1r_returncode == 0)
                    {
                    ak10getmem(&(*acv) , -2 , &req_len , &p.integer.C_2.buf_ptr);
                    }
                if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 16719)).integer.C_4.sp1r_returncode == 0)
                    {
                    ptoc_Var58 = (struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 16721);
                    g10mv12("VAK10 " , 3 , sizeof(struct tak_systembuffer ) , sizeof(struct tak_systembuffer ) , sql__nil(*syspoint , "vak10.p" , 16725) , 1 , &ptoc_Var58->integer.C_2.cd_sysinfo , 1 , (int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 16726)).integer.C_0.b_sl , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 16727)).integer.C_4.sp1r_returncode);
                    *syspoint = (struct tak_systembuffer *) &ptoc_Var58->integer.C_2.cd_sysinfo;
                    ptoc_Var58->integer.C_1.cd_create_cmd_no = (*acv).a_command_count;
                    ptoc_Var58->integer.C_1.cd_cmd_no = (*(struct tak_cache_dir_entry *) sql__nil(old_entry , "vak10.p" , 16730)).integer.C_1.cd_cmd_no;
                    _ptoc_ASSIGN(unsigned char , 1, ptoc_Var58->integer.C_1.cd_state, (*(struct tak_cache_dir_entry *) sql__nil(old_entry , "vak10.p" , 16731)).integer.C_1.cd_state)
                    (_ptoc_MOVE(unsigned char , 1, (*(struct tak_cache_dir_entry *) sql__nil(old_entry , "vak10.p" , 16733)).integer.C_1.cd_state, *(unsigned char (*)[1]) sql__setsub(ptoc_Var59 , (*(struct tak_cache_dir_entry *) sql__nil(old_entry , "vak10.p" , 16733)).integer.C_1.cd_state , ptoc_Var55 , sizeof(unsigned char [1]))) , (*(struct tak_cache_dir_entry *) sql__nil(old_entry , "vak10.p" , 16733)).integer.C_1.cd_state);
                    ak10block_into_freelist(&(*acv) , find_info.fi_ptr , 1 , 0);
                    (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 16738)).integer.C_1.cd_lru_next = (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var57->ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 16739)).integer.C_1.cd_lru_next;
                    (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 16740)).integer.C_1.cd_lru_prev = ptoc_Var57->ci_cache_dir_header;
                    (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var57->ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 16741)).integer.C_1.cd_lru_next = p;
                    (*(struct tak_cache_dir_entry *) sql__nil((*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 16742)).integer.C_1.cd_lru_next.integer.C_1.cdir_ptr , "vak10.p" , 16742)).integer.C_1.cd_lru_prev = p;
                    (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 16743)).integer.C_1.cd_fix_next = fix_next;
                    (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 16744)).integer.C_1.cd_fix_prev = fix_prev;
                    if (fix_next.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
                        {
                        (*(struct tak_cache_dir_entry *) sql__nil(fix_prev.integer.C_1.cdir_ptr , "vak10.p" , 16748)).integer.C_1.cd_fix_next = p;
                        (*(struct tak_cache_dir_entry *) sql__nil(fix_next.integer.C_1.cdir_ptr , "vak10.p" , 16749)).integer.C_1.cd_fix_prev = p;
                        }
                    ak10ins_directory(&(*acv) , &find_info , &ptoc_Var57->ci_dir[sql__setrang(sql__ord((int ) ptoc_Var58->integer.C_1.cd_syskey.integer.C_2.sentrytyp[1]) , 1 , 195) + -1] , &p , &dummy);
                    if ((char ) sql__setrang(rel_sysinfo , 0 , 1))
                        {
                        a10rel_sysinfo(*syspoint);
                        }
                    }
                xtd_directory(0 , p);
                }
            }
        }
    else
        {
        if ((int ) *b_err == 1930)
            {
            a10_nil_get_sysinfo(&(*acv) , &(*syskey) , (unsigned char ) sql__setrang(dstate , 0 , 1) , (short ) sql__setrang((short ) required_len , -32768 , 32767) , &(*syspoint) , &(*b_err));
            if ((int ) *b_err == 0)
                {
                *b_err = 1930;
                }
            }
        }
    if ((int ) *b_err != 0 && (int ) *b_err != 1930)
        {
        a07_b_put_error(&(*acv) , (short ) sql__setrang(*b_err , -32768 , 32767) , 1);
        }
    ak10verify_cache(&(*acv));
    cccprint("<a10_fix_len_get");
    }



int ak10fill_mblock_info(mblock_rec, mblock_info, part_cnt)
tak_messblock_record *mblock_rec;
tak10_mblock_info mblock_info;
int *part_cnt;
    {
    int ix;
    int len;
    tak_messblock_record *ptoc_Var62;
    struct tgg00_MessBlock *ptoc_Var63;
    int ptoc_Var64;
    cccprint("AK10  ak10fill_m");
    ptoc_Var62 = &*mblock_rec;
    ptoc_Var63 = &ptoc_Var62->mbr_mess_block;
    t01int4(0 , "mbs_stack_le" , ptoc_Var62->mbr_stack_len);
    *part_cnt = 0;
    if (ptoc_Var63->integer.C_2.mb_qual_len > 0 && ptoc_Var63->integer.C_4.mb_qual != (tgg00_QualBuf *) (int *) 0)
        {
        mblock_info[0].addr = (tsp00_MoveObj *) (int *) sql__nil(ptoc_Var63->integer.C_4.mb_qual , "vak10.p" , 16810);
        mblock_info[0].size = ptoc_Var63->integer.C_2.mb_qual_size;
        mblock_info[0].len = ptoc_Var63->integer.C_2.mb_qual_len;
        *part_cnt = 1;
        }
    if (ptoc_Var62->mbr_stack_len > 0)
        {
        *part_cnt = *part_cnt + 1;
        mblock_info[sql__setrang(*part_cnt , 1 , 4) + -1].addr = (tsp00_MoveObj *) (int *) sql__nil(ptoc_Var63->integer.C_5.mb_st , "vak10.p" , 16820);
        mblock_info[sql__setrang(*part_cnt , 1 , 4) + -1].size = ptoc_Var63->integer.C_2.mb_st_size;
        mblock_info[sql__setrang(*part_cnt , 1 , 4) + -1].len = ptoc_Var62->mbr_stack_len;
        ix = 1;
        len = 0;
        while (len < ptoc_Var62->mbr_stack_len)
            {
            t01stackentry(0 , &(*(tgg00_StackList *) sql__nil(ptoc_Var63->integer.C_5.mb_st , "vak10.p" , 16828))[sql__setrang(ix , 1 , 32767) + -1] , ix);
            len = len + sizeof(tgg00_StackEntry );
            ix = ix + 1;
            }
        }
    if (ptoc_Var63->integer.C_2.mb_data_len > 0 && ptoc_Var63->integer.C_3.mb_data != (tgg00_DataPart *) (int *) 0)
        {
        *part_cnt = *part_cnt + 1;
        mblock_info[sql__setrang(*part_cnt , 1 , 4) + -1].addr = (tsp00_MoveObj *) (int *) sql__nil(ptoc_Var63->integer.C_3.mb_data , "vak10.p" , 16839);
        mblock_info[sql__setrang(*part_cnt , 1 , 4) + -1].size = ptoc_Var63->integer.C_2.mb_data_size;
        mblock_info[sql__setrang(*part_cnt , 1 , 4) + -1].len = ptoc_Var63->integer.C_2.mb_data_len;
        }
    if (ptoc_Var63->integer.C_2.mb_strat_len > 0 && ptoc_Var63->integer.C_11.mb_strat != (tsp00_MoveObj *) (int *) 0)
        {
        *part_cnt = *part_cnt + 1;
        mblock_info[sql__setrang(*part_cnt , 1 , 4) + -1].addr = (tsp00_MoveObj *) (int *) sql__nil(ptoc_Var63->integer.C_11.mb_strat , "vak10.p" , 16848);
        mblock_info[sql__setrang(*part_cnt , 1 , 4) + -1].size = ptoc_Var63->integer.C_2.mb_strat_size;
        mblock_info[sql__setrang(*part_cnt , 1 , 4) + -1].len = ptoc_Var63->integer.C_2.mb_strat_len;
        }
    ptoc_Var64 = *part_cnt;
    if (1 <= ptoc_Var64)
        {
        ix = 1;
        do
            {
            t01p2int4(0 , "size        " , mblock_info[sql__setrang(ix , 1 , 4) + -1].size , "len         " , mblock_info[sql__setrang(ix , 1 , 4) + -1].len);
            if (ix == ptoc_Var64)
                {
                break;
                }
            ix += 1;
            }
        while (!0);
        }
    cccprint("<ak10fill_mblock");
    }



int a10flush(acv, syskey, disable_cmd_rollback)
tak_all_command_glob *acv;
tgg_sysinfokey *syskey;
char disable_cmd_rollback;
    {
    tsp_int4 cmd_cnt;
    tak10_find_info find_info;
    cccprint("AK10  a10flush  ");
    ak10find_syskey(&(*acv) , &(*syskey) , &find_info);
    if ((int ) find_info.fi_res == 1 && (_ptoc_MEMCMP(unsigned char , 2, (*syskey).integer.C_2.sentrytyp , "\0\200") > 0 ? 1 : 0))
        {
        cmd_cnt = (*acv).a_command_count;
        if ((char ) sql__setrang(disable_cmd_rollback , 0 , 1))
            {
            (*acv).a_command_count = -1;
            }
        ak10temp_add(&(*acv) , find_info.fi_ptr , 0);
        (*acv).a_command_count = cmd_cnt;
        }
    else
        {
        a07_b_put_error(&(*acv) , 1930 , 1);
        ak10store_syskey(&(*acv) , &(*syskey));
        }
    cccprint("<a10flush       ");
    }



int ak10FlushResnames(acv, p)
tak_all_command_glob *acv;
tak_cache_dir_pointer p;
    {
    struct tak_cache_dir_entry *ptoc_Var65;
    cccprint("AK10  ak10FlushR");
    if (p.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        ptoc_Var65 = (struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 16907);
        ak10temp_add(&(*acv) , p , 0);
        ak10FlushResnames(&(*acv) , ptoc_Var65->integer.C_1.cd_less);
        ak10FlushResnames(&(*acv) , ptoc_Var65->integer.C_1.cd_greater);
        }
    cccprint("<ak10FlushResnam");
    }



int a10FlushResnames(acv)
tak_all_command_glob *acv;
    {
    tsp_c2 aux;
    cccprint("AK10  a10FlushRe");
    _ptoc_ASSIGN(unsigned char , 2, aux, "\0\265")
    ak10FlushResnames(&(*acv) , (*acv).a_sysdir.ci_dir[sql__setrang(sql__ord((int ) aux[1]) , 1 , 195) + -1]);
    cccprint("<a10FlushResname");
    }



int a10free_cache(acv)
tak_all_command_glob *acv;
    {
    tsp_objaddr p;
    struct ptoc_Type194
      {
      union  
        {
        struct  
          {
          tsp_bufaddr bufaddr;
          } C_1;
        struct  
          {
          tak10cache_header_ptr chptr;
          } C_2;
        struct  
          {
          tsp_objaddr objptr;
          } C_3;
        } integer;
      } aux_ptr;
    cccprint("AK10  a10free_ca");
    if ((*acv).a_sysdir.ci_cache_p != (tsp00_Buf *) (int *) 0)
        {
        aux_ptr.integer.C_1.bufaddr = (*acv).a_sysdir.ci_cache_p;
        vbegexcl((*acv).a_transinf.tri_trans.trTaskId_gg00 , g08diagcache);
        while (aux_ptr.integer.C_1.bufaddr != (tsp00_Buf *) (int *) 0)
            {
            ak10all_cache_size = ak10all_cache_size - (*(tak10cache_header *) sql__nil(aux_ptr.integer.C_2.chptr , "vak10.p" , 16967)).ch_size;
            p = aux_ptr.integer.C_3.objptr;
            aux_ptr.integer.C_1.bufaddr = (*(tak10cache_header *) sql__nil(aux_ptr.integer.C_2.chptr , "vak10.p" , 16969)).ch_next_cache;
            vmfree(p);
            }
        ak10cache_cnt = ak10cache_cnt + -1;
        vendexcl((*acv).a_transinf.tri_trans.trTaskId_gg00 , g08diagcache);
        (*acv).a_sysdir.ci_cache_p = (tsp00_Buf *) (int *) 0;
        }
    cccprint("<a10free_cache  ");
    }



int a10hit_rate(acv)
tak_all_command_glob *acv;
    {
    int _a10hit_rate;
    tak_sysdirectory *ptoc_Var66;
    cccprint("AK10  a10hit_rat");
    ptoc_Var66 = &(*acv).a_sysdir;
    _a10hit_rate = ak10hit_rate(ptoc_Var66->ci_tmp_read_found , ptoc_Var66->ci_tmp_read_cnt);
    cccprint("<a10hit_rate    ");
    return(_a10hit_rate);
    }

static
  tak_special_node_info ptoc_Var67 = 
  {
  (unsigned char)'\b'
  };
static
  tak_special_node_info ptoc_Var68 = 
  {
  (unsigned char)'\0'
  };


int a10increment_ap_tree(acv)
tak_all_command_glob *acv;
    {
    tgg_basis_error e;
    tak_cache_dir_pointer curr;
    tak_cache_dir_pointer prev;
    tak_cache_dir_pointer found;
    tak_cache_dir_pointer ap_ptr;
    tak_sysbufferaddress p;
    tsp_int4 required_len;
    tsp_int4 assigned_len;
    tsp_int4 pos;
    tsp_int4 tree_start_pos;
    struct ptoc_Type195
      {
      union  
        {
        struct  
          {
          tak_ap_max_tree_ptr ap;
          } C_1;
        struct  
          {
          tak_cache_dir_e_ptr cdir;
          } C_2;
        struct  
          {
          tsp_bufaddr buf_ptr;
          } C_3;
        } integer;
      } aux_ptr;
    struct tak_cache_dir_entry *ptoc_Var69;
    struct ptoc_Type60 *ptoc_Var70;
    int ptoc_Var71;
    int ptoc_Var72;
    struct ptoc_Type60 *ptoc_Var73;
    cccprint("AK10  a10increme");
    t01int4(5 , "scv_index   " , (int ) (*acv).a_scv_index);
    t01int4(5 , "max_ap_node " , (int ) (*acv).a_ap_max_node);
    ak10td_freelist(&(*acv) , 0);
    aux_ptr.integer.C_1.ap = (*acv).a_ap_tree;
    ap_ptr.integer.C_2.buf_ptr = s35add_bufaddr(aux_ptr.integer.C_3.buf_ptr , -48);
    tree_start_pos = (*(struct tak_cache_dir_entry *) sql__nil(ap_ptr.integer.C_1.cdir_ptr , "vak10.p" , 17035)).integer.C_1.cd_pos + 48;
    t01int4(5 , "tree_start_p" , tree_start_pos);
    found = ap_ptr;
    while (found.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        pos = (*(struct tak_cache_dir_entry *) sql__nil(found.integer.C_1.cdir_ptr , "vak10.p" , 17040)).integer.C_1.cd_pos;
        t01int4(5 , "pos         " , pos);
        t01int4(5 , "len         " , (*(struct tak_cache_dir_entry *) sql__nil(found.integer.C_1.cdir_ptr , "vak10.p" , 17043)).integer.C_1.cd_syslen + -48);
        t01int4(5 , "syslen      " , (*(struct tak_cache_dir_entry *) sql__nil(found.integer.C_1.cdir_ptr , "vak10.p" , 17045)).integer.C_1.cd_syslen);
        prev = found;
        found = (*(struct tak_cache_dir_entry *) sql__nil(found.integer.C_1.cdir_ptr , "vak10.p" , 17047)).integer.C_1.cd_fix_next;
        }
    required_len = sizeof(struct ptoc_Type60 ) * 257;
    ak10get_next_avail(&(*acv) , required_len , pos , &assigned_len , &curr.integer.C_2.buf_ptr);
    if (curr.integer.C_2.buf_ptr != (tsp00_Buf *) (int *) 0)
        {
        ak10td_freelist(&(*acv) , 0);
        t01int4(5 , "curr addr   " , curr.integer.C_5.i + 48);
        ptoc_Var69 = (struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 17058);
        ptoc_Var69->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
        ptoc_Var69->integer.C_1.cd_syslen = assigned_len + 48;
        ptoc_Var69->integer.C_2.cd_mem_type = 15;
        p = (struct tak_systembuffer *) &ptoc_Var69->integer.C_2.cd_sysinfo;
        (*(struct tak_cache_dir_entry *) sql__nil(prev.integer.C_1.cdir_ptr , "vak10.p" , 17067)).integer.C_1.cd_fix_next = curr;
        t01int4(5 , "assigned pos" , (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 17068)).integer.C_1.cd_pos);
        t01int4(5 , "assigned len" , assigned_len);
        pos = tree_start_pos;
        (*acv).a_scv_index = 0;
        while (pos < (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 17072)).integer.C_1.cd_pos + 48)
            {
            (*acv).a_scv_index = (short ) sql__setrang((short ) ((int ) (*acv).a_scv_index + 1) , -32768 , 32767);
            pos = pos + sizeof(struct ptoc_Type60 );
            }
        (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak10.p" , 17080))[sql__setrang((int ) (*acv).a_ap_max_node + 1 , 0 , 32767)].n_pos = (int ) (*acv).a_scv_index;
        (*acv).a_scv_index = (short ) sql__setrang((short ) ((int ) (*acv).a_scv_index + -1) , -32768 , 32767);
        (*acv).a_ap_max_node = (short ) sql__setrang((short ) ((int ) (*acv).a_scv_index + (assigned_len - (pos - (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 17084)).integer.C_1.cd_pos) + 48) / sizeof(struct ptoc_Type60 )) , -32768 , 32767);
        ptoc_Var70 = &(*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak10.p" , 17086))[sql__setrang((int ) (*acv).a_ap_max_node , 0 , 32767)];
        ptoc_Var70->n_proc = 1;
        _ptoc_MOVE(unsigned char , 1, ptoc_Var70->n_special, ptoc_Var67);
        ptoc_Var70->n_pos = 0;
        ptoc_Var70->n_sa_level = 0;
        ptoc_Var70->n_lo_level = 0;
        (*acv).a_ap_max_node = (short ) sql__setrang((short ) ((int ) (*acv).a_ap_max_node + -1) , -32768 , 32767);
        }
    else
        {
        ak10td_dirlist(&(*acv) , 39);
        ak10no_more_memory(&(*acv) , &e);
        a07_b_put_error(&(*acv) , (short ) sql__setrang(e , -32768 , 32767) , 1);
        }
    ptoc_Var71 = (int ) (*acv).a_scv_index + 1;
    ptoc_Var72 = (*acv).a_ap_max_node;
    if (ptoc_Var71 <= ptoc_Var72)
        {
        pos = ptoc_Var71;
        do
            {
            ptoc_Var73 = &(*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak10.p" , 17105))[sql__setrang(pos , 0 , 32767)];
            ptoc_Var73->n_proc = 81;
            _ptoc_MOVE(unsigned char , 1, ptoc_Var73->n_special, ptoc_Var68);
            ptoc_Var73->n_symb = 42;
            ptoc_Var73->n_datatype = 14;
            ptoc_Var73->n_subproc = 0;
            ptoc_Var73->n_length = 0;
            ptoc_Var73->n_pos = 0;
            ptoc_Var73->n_lo_level = 0;
            ptoc_Var73->n_sa_level = 0;
            if (pos == ptoc_Var72)
                {
                break;
                }
            pos += 1;
            }
        while (!0);
        }
    t01int4(5 , "scv_index   " , (int ) (*acv).a_scv_index);
    t01int4(5 , "max_ap_node " , (int ) (*acv).a_ap_max_node);
    ak10verify_cache(&(*acv));
    cccprint("<a10increment_ap");
    }



int a10inquire_resnames(acv, cnt, resname_arr)
tak_all_command_glob *acv;
tsp_int4 *cnt;
tak_resn_rec_arr resname_arr;
    {
    tsp_c2 aux;
    tgg_sysinfokey syskey;
    cccprint("AK10  a10inquire");
    _ptoc_ASSIGN(unsigned char , 8, syskey.integer.C_3.sauthid, resname_arr[sql__setrang(*cnt , 1 , 50) + -1].reskey_idfill)
    _ptoc_ASSIGN(unsigned char , 2, syskey.integer.C_2.sentrytyp, "\0\265")
    _ptoc_ASSIGN(unsigned char , 64, syskey.integer.C_9.srestablen, resname_arr[sql__setrang(*cnt , 1 , 50) + -1].reskey_name)
    _ptoc_ASSIGN(unsigned char , 64, syskey.integer.C_9.sresmodul_name, resname_arr[sql__setrang(*cnt , 1 , 50) + -1].reskey_modul_name)
    syskey.skeylen = (short ) sql__setrang((short ) (sizeof(unsigned char [64]) + sizeof(unsigned char [64]) + 10) , -32768 , 32767);
    *cnt = 0;
    _ptoc_ASSIGN(unsigned char , 2, aux, "\0\265")
    ak10inq_resname(&(*acv) , &syskey , (*acv).a_sysdir.ci_dir[sql__setrang(sql__ord((int ) aux[1]) , 1 , 195) + -1] , &(*cnt) , resname_arr);
    cccprint("<a10inquire_resn");
    }



int a10intermediate_mblock(acv, mblock, mblock_ptr)
tak_all_command_glob *acv;
tgg_mess_block *mblock;
tgg_mess_block_ptr *mblock_ptr;
    {
    tgg_basis_error b_err;
    tsp_int4 stack_len;
    tsp_int4 required_len;
    tak_cache_dir_pointer univ_p;
    struct tak_cache_dir_entry *ptoc_Var74;
    cccprint("AK10  a10interme");
    required_len = sizeof(struct tgg00_MessBlock ) + 48;
    ak10getmem(&(*acv) , -2 , &required_len , &univ_p.integer.C_2.buf_ptr);
    if (univ_p.integer.C_2.buf_ptr != (tsp00_Buf *) (int *) 0)
        {
        ptoc_Var74 = (struct tak_cache_dir_entry *) sql__nil(univ_p.integer.C_1.cdir_ptr , "vak10.p" , 17171);
        ptoc_Var74->integer.C_2.cd_mem_type = 13;
        *mblock_ptr = (struct tgg00_MessBlock *) &ptoc_Var74->integer.C_2.cd_sysinfo;
        }
    else
        {
        *mblock_ptr = (struct tgg00_MessBlock *) (int *) 0;
        }
    if (*mblock_ptr != (struct tgg00_MessBlock *) (int *) 0)
        {
        *(struct tgg00_MessBlock *) sql__nil(*mblock_ptr , "vak10.p" , 17183) = *mblock;
        stack_len = -2;
        ak10space_for_mblock(&(*acv) , &(*mblock) , &stack_len , sql__nil(*mblock_ptr , "vak10.p" , 17185) , &b_err);
        if ((int ) b_err == 0)
            {
            (*(struct tgg00_MessBlock *) sql__nil(*mblock_ptr , "vak10.p" , 17189)).integer.C_2.mb_data_len = 0;
            a06cpy_mblock(&(*acv) , &(*mblock) , sql__nil(*mblock_ptr , "vak10.p" , 17190) , &b_err);
            }
        if ((int ) b_err != 0)
            {
            *mblock_ptr = (struct tgg00_MessBlock *) (int *) 0;
            a10dispose(&(*acv) , &univ_p.integer.C_4.sys_ptr);
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
            }
        else
            {
            (*(struct tgg00_MessBlock *) sql__nil(*mblock_ptr , "vak10.p" , 17201)).integer.C_1.mb_trailer.integer.C_9.mb_trns = (tgg00_TransContext *) &(*acv).a_transinf.tri_trans;
            }
        }
    cccprint("<a10intermediate");
    }



int ak10inq_resname(acv, syskey, p, cnt, resname_arr)
tak_all_command_glob *acv;
tgg_sysinfokey *syskey;
tak_cache_dir_pointer p;
tsp_int4 *cnt;
tak_resn_rec_arr resname_arr;
    {
    tsp_lcomp_result res;
    struct tak_cache_dir_entry *ptoc_Var75;
    cccprint("AK10  ak10inq_re");
    if (p.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0 && *cnt < 50)
        {
        ptoc_Var75 = (struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 17222);
        s30cmp(&(*syskey) , 9 , (int ) (*syskey).skeylen , &ptoc_Var75->integer.C_1.cd_syskey , 9 , (int ) (*syskey).skeylen , &res);
        if ((int ) res == 0)
            {
            ak10inq_resname(&(*acv) , &(*syskey) , ptoc_Var75->integer.C_1.cd_less , &(*cnt) , resname_arr);
            if (*cnt < 50)
                {
                if ((0 || (ptoc_Var75->integer.C_1.cd_state[0] & 16) == 0) && ((int ) ptoc_Var75->integer.C_1.cd_syskey.integer.C_2.sentrytyp[1] != 181 || (_ptoc_MEMCMP(unsigned char , 8, ptoc_Var75->integer.C_1.cd_syskey.integer.C_3.sauthid , (*acv).a_curr_user_id) == 0 ? 0 : 1)))
                    {
                    *cnt = *cnt + 1;
                    resname_arr[sql__setrang(*cnt , 1 , 50) + -1] = ptoc_Var75->integer.C_2.cd_sysinfo.integer.C_52.sresname;
                    }
                ak10inq_resname(&(*acv) , &(*syskey) , ptoc_Var75->integer.C_1.cd_greater , &(*cnt) , resname_arr);
                }
            }
        else
            {
            ak10inq_resname(&(*acv) , &(*syskey) , ptoc_Var75->integer.C_1.cd_greater , &(*cnt) , resname_arr);
            }
        }
    cccprint("<ak10inq_resname");
    }

static
  unsigned char ptoc_Var76[1] = 
  {
  (unsigned char)' '
  };
static
  unsigned char ptoc_Var77[1] = 
  {
  (unsigned char)'\b'
  };


int a10ins_or_upd(acv, syspoint, b_err)
tak_all_command_glob *acv;
tak_sysbufferaddress *syspoint;
tgg_basis_error *b_err;
    {
    tak10_find_info find_info;
    struct tak_cache_dir_entry *ptoc_Var78;
    unsigned char ptoc_Var79[1];
    unsigned char ptoc_Var80[1];
    unsigned char ptoc_Var81[1];
    cccprint("AK10  a10ins_or_");
    if (_ptoc_MEMCMP(unsigned char , 2, (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 17271)).integer.C_50.syskey.integer.C_2.sentrytyp , "\0\200") > 0 ? 1 : 0)
        {
        ak10find_syskey(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 17274)).integer.C_50.syskey , &find_info);
        if ((int ) find_info.fi_res == 1)
            {
            if (find_info.fi_ptr.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
                {
                ptoc_Var78 = (struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 17280);
                if (1 && (ptoc_Var78->integer.C_1.cd_state[0] & 16) != 0)
                    {
                    _ptoc_MOVE(unsigned char , 1, ptoc_Var78->integer.C_1.cd_state, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var79 , ptoc_Var78->integer.C_1.cd_state , ptoc_Var76 , sizeof(unsigned char [1])));
                    }
                else
                    {
                    _ptoc_MOVE(unsigned char , 1, ptoc_Var78->integer.C_1.cd_state, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var81 , ptoc_Var78->integer.C_1.cd_state , ptoc_Var77 , sizeof(unsigned char [1])));
                    }
                }
            *b_err = 0;
            }
        else
            {
            *b_err = 1930;
            ak10store_syskey(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 17294)).integer.C_50.syskey);
            }
        }
    else
        {
        *b_err = 1020;
        }
    cccprint("<a10ins_or_upd  ");
    }



int a10invalidate_root(acv, treeid)
tak_all_command_glob *acv;
tgg00_FileId *treeid;
    {
    tsp_c2 aux;
    cccprint("AK10  a10invalid");
    _ptoc_ASSIGN(unsigned char , 2, aux, "\0\001")
    ak10invalidate_root(&(*treeid) , (*acv).a_sysdir.ci_dir[sql__setrang(sql__ord((int ) aux[1]) , 1 , 195) + -1]);
    cccprint("<a10invalidate_r");
    }



char a10is_fixed(acv, p)
tak_all_command_glob *acv;
tak_sysbufferaddress p;
    {
    char _a10is_fixed;
    tak10_find_info fi;
    cccprint("AK10  a10is_fixe");
    _a10is_fixed = 0;
    if (p != (struct tak_systembuffer *) (int *) 0)
        {
        ak10find_syskey(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 17337)).integer.C_50.syskey , &fi);
        }
    if (fi.fi_ptr.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        xtd_directory(0 , fi.fi_ptr);
        _a10is_fixed = (char ) sql__setrang((*(struct tak_cache_dir_entry *) sql__nil(fi.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 17343)).integer.C_1.cd_fix_next.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0 , 0 , 1);
        }
    cccprint("<a10is_fixed    ");
    return(_a10is_fixed);
    }



int a10lock_fixed(acv)
tak_all_command_glob *acv;
    {
    cccprint("AK10  a10lock_fi");
    ak10lock_unlock_fixed(&(*acv) , 1);
    cccprint("<a10lock_fixed  ");
    }



int a10gap_dispose(used_blocks, p)
tak10_used_blocks used_blocks;
tak_sysbufferaddress *p;
    {
    tak_cache_dir_pointer curr;
    cccprint("AK10  a10gap_dis");
    curr.integer.C_4.sys_ptr = *p;
    curr.integer.C_2.buf_ptr = s35add_bufaddr(curr.integer.C_2.buf_ptr , -48);
    ak10add_block(used_blocks , (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 17376)).integer.C_1.cd_pos , (int ) (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 17377)).integer.C_1.cd_buf_no , (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 17377)).integer.C_1.cd_syslen);
    cccprint("<a10gap_dispose ");
    }



int ak10gaps_baserecord(used_blocks, baserec)
tak10_used_blocks used_blocks;
tak_baserecord *baserec;
    {
    tak_basecolinfo_ptr p;
    tak_sysbufferaddress aux;
    cccprint("AK10  ak10gaps_b");
    if ((int ) (*baserec).btablekind != 8)
        {
        if ((*baserec).bptr != (struct tak_basecolinfo *) (int *) 0)
            {
            p = (*baserec).bptr;
            while (p != (struct tak_basecolinfo *) (int *) 0)
                {
                aux = (struct tak_systembuffer *) (int *) sql__nil(p , "vak10.p" , 17402);
                p = (*(struct tak_basecolinfo *) sql__nil(p , "vak10.p" , 17403)).bci_next;
                a10gap_dispose(used_blocks , &aux);
                }
            }
        }
    cccprint("<ak10gaps_basere");
    }



int ak10gap_free_colnamesrecord(used_blocks, colnamesrec)
tak10_used_blocks used_blocks;
tak_columnnamesrecord *colnamesrec;
    {
    tak_sysbufferaddress p;
    tak_sysbufferaddress next;
    cccprint("AK10  ak10gap_fr");
    if ((int ) (*colnamesrec).cnreclen == 8096)
        {
        p = (*colnamesrec).boolean.C_false.cnnext;
        while (p != (struct tak_systembuffer *) (int *) 0)
            {
            if ((int ) (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 17432)).integer.C_0.b_sl == 8096)
                {
                next = (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 17434)).integer.C_59.scolnames.boolean.C_false.cnnext;
                }
            else
                {
                next = (struct tak_systembuffer *) (int *) 0;
                }
            a10gap_dispose(used_blocks , &p);
            p = next;
            }
        }
    cccprint("<ak10gap_free_co");
    }



int ak10gap_free_mblock(used_blocks, mblock)
tak10_used_blocks used_blocks;
tgg_mess_block *mblock;
    {
    int ix;
    struct ptoc_Type196
      {
      union  
        {
        struct  
          {
          tsp_bufaddr buf_ptr;
          } C_1;
        struct  
          {
          tgg_qual_buf_ptr qual_ptr;
          } C_2;
        struct  
          {
          tak_cache_dir_pointer cdir_ptr;
          } C_3;
        struct  
          {
          tgg_datapart_ptr data_ptr;
          } C_4;
        struct  
          {
          tgg_stack_list_ptr st_ptr;
          } C_5;
        struct  
          {
          tsp00_MoveObjPtr strat_ptr;
          } C_6;
        } integer;
      } p[4];
    cccprint("AK10  ak10gap_fr");
    p[0].integer.C_2.qual_ptr = (*mblock).integer.C_4.mb_qual;
    p[1].integer.C_4.data_ptr = (*mblock).integer.C_3.mb_data;
    p[2].integer.C_5.st_ptr = (*mblock).integer.C_5.mb_st;
    p[3].integer.C_6.strat_ptr = (*mblock).integer.C_11.mb_strat;
    ix = 1;
    do
        {
        if (p[sql__setrang(ix , 1 , 4) + -1].integer.C_1.buf_ptr != (tsp00_Buf *) (int *) 0)
            {
            t01addr(0 , "addr        " , p[sql__setrang(ix , 1 , 4) + -1].integer.C_1.buf_ptr);
            p[sql__setrang(ix , 1 , 4) + -1].integer.C_1.buf_ptr = s35add_bufaddr(p[sql__setrang(ix , 1 , 4) + -1].integer.C_1.buf_ptr , -48);
            t01addr(0 , "addr        " , p[sql__setrang(ix , 1 , 4) + -1].integer.C_1.buf_ptr);
            ak10add_block(used_blocks , (*(struct tak_cache_dir_entry *) sql__nil(p[sql__setrang(ix , 1 , 4) + -1].integer.C_3.cdir_ptr.integer.C_1.cdir_ptr , "vak10.p" , 17490)).integer.C_1.cd_pos , (int ) (*(struct tak_cache_dir_entry *) sql__nil(p[sql__setrang(ix , 1 , 4) + -1].integer.C_3.cdir_ptr.integer.C_1.cdir_ptr , "vak10.p" , 17491)).integer.C_1.cd_buf_no , (*(struct tak_cache_dir_entry *) sql__nil(p[sql__setrang(ix , 1 , 4) + -1].integer.C_3.cdir_ptr.integer.C_1.cdir_ptr , "vak10.p" , 17492)).integer.C_1.cd_syslen);
            }
        if (ix == 4)
            {
            break;
            }
        ix += 1;
        }
    while (!0);
    cccprint("<ak10gap_free_mb");
    }



int ak10gap_free_method_record(used_blocks, method)
tak10_used_blocks used_blocks;
tak_methodrecord *method;
    {
    int ix;
    struct ptoc_Type197
      {
      union  
        {
        struct  
          {
          tak_param_info_ptr parm_ptr;
          } C_true;
        struct  
          {
          tak_sysbufferaddress sys_ptr;
          } C_false;
        } boolean;
      } p;
    int ptoc_Var82;
    cccprint("AK10  ak10gap_fr");
    ptoc_Var82 = (*method).me_param_cnt;
    if (1 <= ptoc_Var82)
        {
        ix = 1;
        do
            {
            p.boolean.C_true.parm_ptr = (*method).me_param_list[sql__setrang(ix , 1 , 1024) + -1];
            if (p.boolean.C_false.sys_ptr != (struct tak_systembuffer *) (int *) 0)
                {
                a10gap_dispose(used_blocks , &p.boolean.C_false.sys_ptr);
                }
            if (ix == ptoc_Var82)
                {
                break;
                }
            ix += 1;
            }
        while (!0);
        }
    cccprint("<ak10gap_free_me");
    }



int a10look_for_gaps(acv)
tak_all_command_glob *acv;
    {
    int ix;
    tak10_used_blocks used_blocks;
    tak_cache_dir_pointer next;
    tak_cache_dir_pointer curr;
    tak_cache_dir_pointer ap_ptr;
    int bufno;
    tsp_int4 gap_pos;
    tsp_int4 gap_len;
    struct ptoc_Type198
      {
      union  
        {
        struct  
          {
          tsp_bufaddr bufaddr;
          } C_true;
        struct  
          {
          tak10cache_header_ptr chptr;
          } C_false;
        } boolean;
      } prev_cache;
    tak_sysdirectory *ptoc_Var83;
    struct tak_cache_dir_entry *ptoc_Var84;
    struct tak_cache_dir_entry *ptoc_Var85;
    cccprint("AK10  a10look_fo");
    used_blocks[0].pos = 0;
    used_blocks[0].len = 0;
    used_blocks[1].pos = 2147483647;
    used_blocks[1].len = 0;
    ptoc_Var83 = &(*acv).a_sysdir;
    if (ptoc_Var83->ci_cache_p != (tsp00_Buf *) (int *) 0)
        {
        next = (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var83->ci_cache_dir_trailor.integer.C_1.cdir_ptr , "vak10.p" , 17571)).integer.C_1.cd_lru_prev;
        while (next.integer.C_1.cdir_ptr != ptoc_Var83->ci_cache_dir_header.integer.C_1.cdir_ptr)
            {
            curr = next;
            ptoc_Var84 = (struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 17575);
            t01buf1(0 , &ptoc_Var84->integer.C_1.cd_syskey , 9 , (int ) ptoc_Var84->integer.C_1.cd_syskey.skeylen + 8);
            next = (*(struct tak_cache_dir_entry *) sql__nil(next.integer.C_1.cdir_ptr , "vak10.p" , 17579)).integer.C_1.cd_lru_prev;
            ak10add_block(used_blocks , ptoc_Var84->integer.C_1.cd_pos , (int ) ptoc_Var84->integer.C_1.cd_buf_no , ptoc_Var84->integer.C_1.cd_syslen);
            switch ((int ) ptoc_Var84->integer.C_1.cd_syskey.integer.C_2.sentrytyp[1])
                {
                case 192 :
                    ak10gap_free_colnamesrecord(used_blocks , &ptoc_Var84->integer.C_2.cd_sysinfo.integer.C_59.scolnames);
                    break;
                case 1 :
                case 177 :
                    ak10gaps_baserecord(used_blocks , &ptoc_Var84->integer.C_2.cd_sysinfo.integer.C_1.sbase);
                    break;
                case 193 :
                case 24 :
                case 149 :
                    ak10gap_free_mblock(used_blocks , &ptoc_Var84->integer.C_2.cd_sysinfo.integer.C_62.smessblock.mbr_mess_block);
                    break;
                case 105 :
                    ak10gap_free_method_record(used_blocks , &ptoc_Var84->integer.C_2.cd_sysinfo.integer.C_66.smethod);
                    break;
                default :
                    break;
                }
            }
        t01int4(0 , "Free List   " , 1);
        curr = (*(struct tak_cache_dir_entry *) sql__nil((*acv).a_sysdir.ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 17600)).integer.C_2.cd_next_free;
        while (curr.integer.C_2.buf_ptr != (*acv).a_sysdir.ci_cache_dir_trailor.integer.C_2.buf_ptr)
            {
            ptoc_Var85 = (struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 17603);
            ak10add_block(used_blocks , ptoc_Var85->integer.C_2.cd_block_pos , (int ) ptoc_Var85->integer.C_1.cd_buf_no , ptoc_Var85->integer.C_1.cd_syslen);
            curr = (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 17606)).integer.C_2.cd_next_free;
            }
        ap_ptr.integer.C_2.buf_ptr = (tsp00_Buf *) (int *) sql__nil((*acv).a_ap_tree , "vak10.p" , 17612);
        ap_ptr.integer.C_2.buf_ptr = s35add_bufaddr(ap_ptr.integer.C_2.buf_ptr , -48);
        ak10add_block(used_blocks , (*(struct tak_cache_dir_entry *) sql__nil(ap_ptr.integer.C_1.cdir_ptr , "vak10.p" , 17615)).integer.C_1.cd_pos , (int ) (*(struct tak_cache_dir_entry *) sql__nil(ap_ptr.integer.C_1.cdir_ptr , "vak10.p" , 17616)).integer.C_1.cd_buf_no , (*(struct tak_cache_dir_entry *) sql__nil(ap_ptr.integer.C_1.cdir_ptr , "vak10.p" , 17616)).integer.C_1.cd_syslen);
        curr = (*(struct tak_cache_dir_entry *) sql__nil(ap_ptr.integer.C_1.cdir_ptr , "vak10.p" , 17617)).integer.C_1.cd_fix_next;
        while (curr.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
            {
            a10gap_dispose(used_blocks , &curr.integer.C_4.sys_ptr);
            curr = (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 17621)).integer.C_1.cd_fix_next;
            }
        prev_cache.boolean.C_true.bufaddr = (*acv).a_sysdir.ci_cache_p;
        bufno = 0;
        while ((*(tak10cache_header *) sql__nil(prev_cache.boolean.C_false.chptr , "vak10.p" , 17626)).ch_next_cache != (tsp00_Buf *) (int *) 0)
            {
            bufno = bufno + 1;
            ak10add_block(used_blocks , 1 , bufno , sizeof(tak10cache_header ));
            prev_cache.boolean.C_true.bufaddr = (*(tak10cache_header *) sql__nil(prev_cache.boolean.C_false.chptr , "vak10.p" , 17630)).ch_next_cache;
            }
        ix = 2;
        while (used_blocks[sql__setrang(ix , 1 , 10000) + -1].pos != 2147483647)
            {
            gap_pos = used_blocks[sql__setrang(ix + -1 , 1 , 10000) + -1].pos + used_blocks[sql__setrang(ix + -1 , 1 , 10000) + -1].len;
            gap_len = used_blocks[sql__setrang(ix , 1 , 10000) + -1].pos - gap_pos;
            if (ix > 2)
                {
                t01int4(39 , "gap pos     " , gap_pos);
                t01int4(39 , "gap len     " , gap_len);
                g01abort(51114 , "AK CACHE" , "GAP IN CACHE FOUND      " , gap_pos);
                g01abort(51114 , "AK CACHE" , "GAP IN CACHE FOUND      " , gap_len);
                }
            ix = ix + 1;
            }
        }
    cccprint("<a10look_for_gap");
    }



int ak10add_block(used_blocks, pos, buf_no, len)
tak10_used_blocks used_blocks;
tsp_int4 pos;
int buf_no;
tsp_int4 len;
    {
    int ix;
    int jx;
    int kx;
    int ptoc_Var86;
    int ptoc_Var87;
    cccprint("AK10  ak10add_bl");
    if (buf_no > 0)
        {
        pos = pos + g01catalog_cache_init_size() + (buf_no * 4 + -4) * sizeof(tsp00_Buf );
        }
    t01p2int4(0 , "Pos         " , pos , "Len         " , len);
    ix = 1;
    while (used_blocks[sql__setrang(ix , 1 , 10000) + -1].pos < pos)
        ix = ix + 1;
    if (used_blocks[sql__setrang(ix , 1 , 10000) + -1].pos != pos && used_blocks[sql__setrang(ix + -1 , 1 , 10000) + -1].pos + used_blocks[sql__setrang(ix + -1 , 1 , 10000) + -1].len <= pos)
        {
        if (used_blocks[sql__setrang(ix + -1 , 1 , 10000) + -1].pos + used_blocks[sql__setrang(ix + -1 , 1 , 10000) + -1].len == pos)
            {
            used_blocks[sql__setrang(ix + -1 , 1 , 10000) + -1].len = used_blocks[sql__setrang(ix + -1 , 1 , 10000) + -1].len + len;
            if (used_blocks[sql__setrang(ix + -1 , 1 , 10000) + -1].pos + used_blocks[sql__setrang(ix + -1 , 1 , 10000) + -1].len == used_blocks[sql__setrang(ix , 1 , 10000) + -1].pos)
                {
                used_blocks[sql__setrang(ix + -1 , 1 , 10000) + -1].len = used_blocks[sql__setrang(ix + -1 , 1 , 10000) + -1].len + used_blocks[sql__setrang(ix , 1 , 10000) + -1].len;
                while (used_blocks[sql__setrang(ix + 1 , 1 , 10000) + -1].pos != 2147483647)
                    {
                    used_blocks[sql__setrang(ix , 1 , 10000) + -1] = used_blocks[sql__setrang(ix + 1 , 1 , 10000) + -1];
                    ix = ix + 1;
                    }
                used_blocks[sql__setrang(ix , 1 , 10000) + -1].pos = 2147483647;
                used_blocks[sql__setrang(ix , 1 , 10000) + -1].len = 0;
                }
            }
        else
            {
            if (pos + len == used_blocks[sql__setrang(ix , 1 , 10000) + -1].pos)
                {
                used_blocks[sql__setrang(ix , 1 , 10000) + -1].pos = pos;
                used_blocks[sql__setrang(ix , 1 , 10000) + -1].len = used_blocks[sql__setrang(ix , 1 , 10000) + -1].len + len;
                }
            else
                {
                jx = ix;
                while (used_blocks[sql__setrang(jx , 1 , 10000) + -1].pos != 2147483647)
                    jx = jx + 1;
                ptoc_Var86 = jx;
                ptoc_Var87 = ix;
                if (ptoc_Var86 >= ptoc_Var87)
                    {
                    kx = ptoc_Var86;
                    do
                        {
                        used_blocks[sql__setrang(kx + 1 , 1 , 10000) + -1] = used_blocks[sql__setrang(kx , 1 , 10000) + -1];
                        if (kx == ptoc_Var87)
                            {
                            break;
                            }
                        kx -= 1;
                        }
                    while (!0);
                    }
                used_blocks[sql__setrang(ix , 1 , 10000) + -1].pos = pos;
                used_blocks[sql__setrang(ix , 1 , 10000) + -1].len = len;
                }
            }
        }
    ix = 2;
    while (used_blocks[sql__setrang(ix , 1 , 10000) + -1].pos != 2147483647)
        {
        t01p2int4(0 , "BlockPos    " , used_blocks[sql__setrang(ix , 1 , 10000) + -1].pos , "BlockLen    " , used_blocks[sql__setrang(ix , 1 , 10000) + -1].len);
        ix = ix + 1;
        }
    cccprint("<ak10add_block  ");
    }

static
  unsigned char ptoc_Var88[1] = 
  {
  (unsigned char)' '
  };


int a10mark_updated(p)
tak_sysbufferaddress p;
    {
    tak_cache_dir_pointer curr;
    struct tak_cache_dir_entry *ptoc_Var89;
    unsigned char ptoc_Var90[1];
    unsigned char ptoc_Var91[1];
    unsigned char ptoc_Var92[1];
    cccprint("AK10  a10mark_up");
    if (_ptoc_MEMCMP(unsigned char , 2, (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 17746)).integer.C_50.syskey.integer.C_2.sentrytyp , "\0\200") < 0 ? 1 : 0)
        {
        g01abort(51114 , "AK CACHE" , "NO TEMP SYSKEY          " , 0);
        }
    if (p != (struct tak_systembuffer *) (int *) 0)
        {
        curr.integer.C_4.sys_ptr = p;
        curr.integer.C_2.buf_ptr = s35add_bufaddr(curr.integer.C_2.buf_ptr , -48);
        ptoc_Var89 = (struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 17755);
        if (1 && (ptoc_Var89->integer.C_1.cd_state[0] & 16) != 0)
            {
            _ptoc_MOVE(unsigned char , 1, ptoc_Var89->integer.C_1.cd_state, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var90 , ptoc_Var89->integer.C_1.cd_state , ptoc_Var88 , sizeof(unsigned char [1])));
            }
        }
    cccprint("<a10mark_updated");
    }

static
  unsigned char ptoc_Var93[1] = 
  {
  (unsigned char)'\002'
  };


int a10mark_unrelease(p)
tak_sysbufferaddress p;
    {
    tak_cache_dir_pointer curr;
    struct tak_cache_dir_entry *ptoc_Var94;
    unsigned char ptoc_Var95[1];
    unsigned char ptoc_Var96[1];
    unsigned char ptoc_Var97[1];
    cccprint("AK10  a10mark_un");
    if (p != (struct tak_systembuffer *) (int *) 0)
        {
        curr.integer.C_4.sys_ptr = p;
        curr.integer.C_2.buf_ptr = s35add_bufaddr(curr.integer.C_2.buf_ptr , -48);
        ptoc_Var94 = (struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 17783);
        _ptoc_MOVE(unsigned char , 1, ptoc_Var94->integer.C_1.cd_state, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var95 , ptoc_Var94->integer.C_1.cd_state , ptoc_Var93 , sizeof(unsigned char [1])));
        }
    cccprint("<a10mark_unrelea");
    }

static
  unsigned char ptoc_Var98[1] = 
  {
  (unsigned char)'\002'
  };


int a10mark_release(p)
tak_sysbufferaddress p;
    {
    tak_cache_dir_pointer curr;
    struct tak_cache_dir_entry *ptoc_Var99;
    unsigned char ptoc_Var100[1];
    unsigned char ptoc_Var101[1];
    unsigned char ptoc_Var102[1];
    cccprint("AK10  a10mark_re");
    if (p != (struct tak_systembuffer *) (int *) 0)
        {
        curr.integer.C_4.sys_ptr = p;
        curr.integer.C_2.buf_ptr = s35add_bufaddr(curr.integer.C_2.buf_ptr , -48);
        ptoc_Var99 = (struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 17808);
        _ptoc_MOVE(unsigned char , 1, ptoc_Var99->integer.C_1.cd_state, *(unsigned char (*)[1]) sql__setsub(ptoc_Var100 , ptoc_Var99->integer.C_1.cd_state , ptoc_Var98 , sizeof(unsigned char [1])));
        }
    cccprint("<a10mark_release");
    }



int a10mblock_into_cache(acv, syskey, mblock, dstate, syspoint, b_err)
tak_all_command_glob *acv;
tgg_sysinfokey *syskey;
tgg_mess_block *mblock;
tak_directory_state dstate;
tak_sysbufferaddress *syspoint;
tgg_basis_error *b_err;
    {
    tak_messblock_record *ptoc_Var103;
    cccprint("AK10  a10mblock_");
    a10_nil_get_sysinfo(&(*acv) , &(*syskey) , 0 , (short ) sql__setrang((short ) sizeof(tak_messblock_record ) , -32768 , 32767) , &(*syspoint) , &(*b_err));
    if ((int ) *b_err == 0)
        {
        ptoc_Var103 = &(*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 17829)).integer.C_62.smessblock;
        ptoc_Var103->mbr_stack_len = -2;
        ak10space_for_mblock(&(*acv) , &(*mblock) , &ptoc_Var103->mbr_stack_len , &ptoc_Var103->mbr_mess_block , &(*b_err));
        if ((int ) *b_err == 0)
            {
            ptoc_Var103->mbr_mess_block.integer.C_2.mb_data_len = 0;
            a06cpy_mblock(&(*acv) , &(*mblock) , &ptoc_Var103->mbr_mess_block , &(*b_err));
            if ((int ) *b_err == 0)
                {
                ptoc_Var103->mbr_mess_block.integer.C_1.mb_trailer.integer.C_9.mb_trns = (tgg00_TransContext *) &(*acv).a_transinf.tri_trans;
                }
            }
        if ((int ) dstate != 0)
            {
            a10_rel_sysinfo(&(*acv) , &(*syskey));
            }
        ak10verify_cache(&(*acv));
        }
    cccprint("<a10mblock_into_");
    }



int a10new(acv, obj_size, p)
tak_all_command_glob *acv;
tsp_int4 obj_size;
tak_sysbufferaddress *p;
    {
    tsp_int4 required_len;
    tak_cache_dir_pointer univ_p;
    tsp00_Addr aux_ptr1;
    tsp00_Addr aux_ptr2;
    cccprint("AK10  a10new    ");
    required_len = obj_size + 48;
    ak10getmem(&(*acv) , -2 , &required_len , &univ_p.integer.C_2.buf_ptr);
    if (univ_p.integer.C_2.buf_ptr != (tsp00_Buf *) (int *) 0)
        {
        aux_ptr1 = (unsigned char *) &(*acv).a_ap_tree;
        aux_ptr2 = (unsigned char *) &*p;
        if (aux_ptr1 == aux_ptr2)
            {
            (*(struct tak_cache_dir_entry *) sql__nil(univ_p.integer.C_1.cdir_ptr , "vak10.p" , 17881)).integer.C_2.cd_mem_type = 15;
            }
        else
            {
            (*(struct tak_cache_dir_entry *) sql__nil(univ_p.integer.C_1.cdir_ptr , "vak10.p" , 17883)).integer.C_2.cd_mem_type = 11;
            }
        *p = (struct tak_systembuffer *) &(*(struct tak_cache_dir_entry *) sql__nil(univ_p.integer.C_1.cdir_ptr , "vak10.p" , 17885)).integer.C_2.cd_sysinfo;
        }
    else
        {
        *p = (struct tak_systembuffer *) (int *) 0;
        }
    cccprint("<a10new         ");
    }



int a10new_packet(acv, size, ptr)
tak_all_command_glob *acv;
tsp_int4 size;
tsp_moveobj_ptr *ptr;
    {
    tsp_int4 required_len;
    tak_cache_dir_pointer univ_p;
    struct ptoc_Type199
      {
      union  
        {
        struct  
          {
          tak_sysbufferaddress sys;
          } C_true;
        struct  
          {
          tsp_moveobj_ptr packet_ptr;
          } C_false;
        } boolean;
      } p;
    struct tak_cache_dir_entry *ptoc_Var104;
    cccprint("AK10  a10new_pac");
    required_len = size + 48;
    ak10getmem(&(*acv) , -2 , &required_len , &univ_p.integer.C_2.buf_ptr);
    if (univ_p.integer.C_2.buf_ptr != (tsp00_Buf *) (int *) 0)
        {
        ptoc_Var104 = (struct tak_cache_dir_entry *) sql__nil(univ_p.integer.C_1.cdir_ptr , "vak10.p" , 17920);
        ptoc_Var104->integer.C_2.cd_mem_type = 12;
        p.boolean.C_true.sys = (struct tak_systembuffer *) &ptoc_Var104->integer.C_2.cd_sysinfo;
        *ptr = p.boolean.C_false.packet_ptr;
        }
    else
        {
        *ptr = (tsp00_MoveObj *) (int *) 0;
        }
    cccprint("<a10new_packet  ");
    }



int a10prefix_parsid_delete(acv, parsk, del_cnt, prefix_length)
tak_all_command_glob *acv;
tak_parskey *parsk;
int *del_cnt;
int prefix_length;
    {
    char nothing_found;
    tgg_basis_error b_err;
    tsp_lcomp_result res;
    int i;
    tak_cache_dir_pointer p;
    tgg_sysinfokey syskey;
    tgg00_SysInfoKey *ptoc_Var105;
    cccprint("AK10  a10prefix_");
    syskey = a01sysnullkey;
    ptoc_Var105 = &syskey;
    ptoc_Var105->integer.C_3.sauthid[0] = (unsigned char)'\377';
    s10mv1(sizeof(tak_parskey ) , sizeof(tsp00_C8 ) , &(*parsk) , 1 , ptoc_Var105->integer.C_3.sauthid , 2 , prefix_length);
    *del_cnt = 0;
    i = sql__ord(128);
    do
        {
        i = i + 1;
        do
            {
            nothing_found = 1;
            p = (*acv).a_sysdir.ci_dir[sql__setrang(i , 1 , 195) + -1];
            while (p.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
                if ((*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 17967)).integer.C_1.cd_pos + (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 17967)).integer.C_1.cd_check != 0)
                    {
                    ak10cache_error(&(*acv) , "a10prefix_parsid        " , p);
                    p.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
                    }
                else
                    {
                    s30cmp(&syskey , 9 , prefix_length + 1 , &(*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 17976)).integer.C_1.cd_syskey , 9 , prefix_length + 1 , &res);
                    if ((int ) res == 1)
                        {
                        nothing_found = 0;
                        a10del_sysinfo(&(*acv) , &(*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 17981)).integer.C_1.cd_syskey , &b_err);
                        if ((int ) b_err != 0)
                            {
                            nothing_found = 1;
                            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                            }
                        else
                            {
                            *del_cnt = *del_cnt + 1;
                            }
                        p.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
                        }
                    else
                        {
                        if ((int ) res == 0)
                            {
                            p = (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 17996)).integer.C_1.cd_less;
                            }
                        else
                            {
                            p = (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 17998)).integer.C_1.cd_greater;
                            }
                        }
                    }
            }
        while (!(char ) sql__setrang(nothing_found , 0 , 1));
        }
    while (!(i == 195));
    cccprint("<a10prefix_parsi");
    }



int a10refresh_cache(acv)
tak_all_command_glob *acv;
    {
    tak_cache_dir_pointer next;
    tak_cache_dir_pointer curr;
    tak_sysdirectory *ptoc_Var106;
    cccprint("AK10  a10refresh");
    ptoc_Var106 = &(*acv).a_sysdir;
    if (ptoc_Var106->ci_cache_p != (tsp00_Buf *) (int *) 0)
        {
        next = (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var106->ci_cache_dir_trailor.integer.C_1.cdir_ptr , "vak10.p" , 18031)).integer.C_1.cd_lru_prev;
        while (next.integer.C_1.cdir_ptr != ptoc_Var106->ci_cache_dir_header.integer.C_1.cdir_ptr)
            {
            curr = next;
            next = (*(struct tak_cache_dir_entry *) sql__nil(next.integer.C_1.cdir_ptr , "vak10.p" , 18035)).integer.C_1.cd_lru_prev;
            if (_ptoc_MEMCMP(unsigned char , 2, (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 18036)).integer.C_1.cd_syskey.integer.C_2.sentrytyp , "\0\200") < 0 ? 1 : 0)
                {
                ak10block_into_freelist(&(*acv) , curr , 1 , 1);
                }
            }
        ak10td_dirlist(&(*acv) , 0);
        ak10td_freelist(&(*acv) , 0);
        }
    cccprint("<a10refresh_cach");
    }



int a10release_table(acv, tableid)
tak_all_command_glob *acv;
tgg_surrogate tableid;
    {
    int i;
    tsp_lcomp_result res;
    tak_cache_dir_pointer p;
    tgg_sysinfokey syskey;
    struct tak_cache_dir_entry *ptoc_Var107;
    cccprint("AK10  a10release");
    _ptoc_ASSIGN(unsigned char , 8, syskey.integer.C_2.stableid, tableid)
    i = 1;
    while (i < 195)
        {
        p = (*acv).a_sysdir.ci_dir[sql__setrang(i , 1 , 195) + -1];
        while (p.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
            {
            ptoc_Var107 = (struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 18072);
            if (ptoc_Var107->integer.C_1.cd_pos + ptoc_Var107->integer.C_1.cd_check != 0)
                {
                ak10cache_error(&(*acv) , "a10release_table        " , p);
                p.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
                i = 195;
                }
            else
                {
                s30cmp(&syskey , 9 , 8 , &ptoc_Var107->integer.C_1.cd_syskey , 9 , 8 , &res);
                if ((int ) res == 1)
                    {
                    ak10release_table(tableid , p);
                    p.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
                    }
                else
                    {
                    if ((int ) res == 0)
                        {
                        p = ptoc_Var107->integer.C_1.cd_less;
                        }
                    else
                        {
                        p = ptoc_Var107->integer.C_1.cd_greater;
                        }
                    }
                }
            }
        i = i + 1;
        }
    cccprint("<a10release_tabl");
    }



int a10remove_parsinfos(acv)
tak_all_command_glob *acv;
    {
    tak_cache_dir_pointer curr;
    tak_cache_dir_pointer prev;
    tak10_find_info find_info;
    cccprint("AK10  a10remove_");
    prev = (*acv).a_sysdir.ci_cache_dir_header;
    curr = (*(struct tak_cache_dir_entry *) sql__nil((*acv).a_sysdir.ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 18125)).integer.C_1.cd_lru_next;
    while (curr.integer.C_1.cdir_ptr != (*acv).a_sysdir.ci_cache_dir_trailor.integer.C_1.cdir_ptr)
        {
        if ((*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 18128)).integer.C_1.cd_cmd_no < 0)
            {
            if (_ptoc_MEMCMP(unsigned char , 2, (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 18130)).integer.C_1.cd_syskey.integer.C_2.sentrytyp , "\0\200") > 0 ? 1 : 0)
                {
                ak10find_syskey(&(*acv) , &(*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 18133)).integer.C_1.cd_syskey , &find_info);
                if ((int ) find_info.fi_res == 1)
                    {
                    ak10block_into_freelist(&(*acv) , find_info.fi_ptr , 1 , 1);
                    curr = prev;
                    }
                }
            else
                {
                (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 18144)).integer.C_1.cd_cmd_no = (*acv).a_command_count;
                }
            }
        prev = curr;
        curr = (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 18148)).integer.C_1.cd_lru_next;
        }
    cccprint("<a10remove_parsi");
    }



int a10shrink_syntax_tree(acv)
tak_all_command_glob *acv;
    {
    tak_cache_dir_pointer ap_ptr;
    tak_cache_dir_pointer curr;
    tak_cache_dir_pointer found;
    cccprint("AK10  a10shrink_");
    ap_ptr.integer.C_2.buf_ptr = (tsp00_Buf *) (int *) sql__nil((*acv).a_ap_tree , "vak10.p" , 18168);
    ap_ptr.integer.C_2.buf_ptr = s35add_bufaddr(ap_ptr.integer.C_2.buf_ptr , -48);
    curr = (*(struct tak_cache_dir_entry *) sql__nil(ap_ptr.integer.C_1.cdir_ptr , "vak10.p" , 18171)).integer.C_1.cd_fix_next;
    while (curr.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        found.integer.C_4.sys_ptr = (struct tak_systembuffer *) &(*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 18174)).integer.C_2.cd_sysinfo;
        curr = (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 18175)).integer.C_1.cd_fix_next;
        a10dispose(&(*acv) , &found.integer.C_4.sys_ptr);
        }
    (*(struct tak_cache_dir_entry *) sql__nil(ap_ptr.integer.C_1.cdir_ptr , "vak10.p" , 18179)).integer.C_1.cd_fix_next.integer.C_2.buf_ptr = (tsp00_Buf *) (int *) 0;
    (*acv).a_ap_max_node = 512;
    (*(tak_ap_max_tree *) sql__nil((*acv).a_ap_tree , "vak10.p" , 18181))[513].n_pos = 0;
    cccprint("<a10shrink_synta");
    }



int a10table_cache_delete(acv, tableid)
tak_all_command_glob *acv;
tgg_surrogate tableid;
    {
    cccprint("AK10  a10table_c");
    ak10table_cache_delete(&(*acv) , tableid , 1);
    cccprint("<a10table_cache_");
    }



int a10unlock_fixed(acv)
tak_all_command_glob *acv;
    {
    cccprint("AK10  a10unlock_");
    ak10lock_unlock_fixed(&(*acv) , 0);
    cccprint("<a10unlock_fixed");
    }

static
  unsigned char ptoc_Var108[1] = 
  {
  (unsigned char)'\b'
  };


int a10add_sysinfo(acv, syspoint, b_err)
tak_all_command_glob *acv;
tak_sysbufferaddress *syspoint;
tgg_basis_error *b_err;
    {
    char via_kb;
    int rest;
    tak10_find_info find_info;
    struct ptoc_Type200
      {
      union  
        {
        struct  
          {
          tak_basecolinfo_ptr colp;
          } C_true;
        struct  
          {
          tak_sysbufferaddress sysp;
          } C_false;
        } boolean;
      } last;
    tak_all_command_glob *ptoc_Var109;
    struct tak_systembuffer *ptoc_Var110;
    tgg00_SysInfoKey *ptoc_Var111;
    int ptoc_Var112;
    struct tak_cache_dir_entry *ptoc_Var113;
    unsigned char ptoc_Var114[1];
    unsigned char ptoc_Var115[1];
    unsigned char ptoc_Var116[1];
    cccprint("AK10  a10add_sys");
    if (t01trace(0))
        {
        t01catalog_entrytype(0 , (unsigned char ) sql__setrang((*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 18238)).integer.C_50.syskey.integer.C_2.sentrytyp[1] , 0 , 255));
        t01buf1(0 , &(*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 18239)).integer.C_50.syskey , 1 , (int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 18240)).integer.C_50.syskey.skeylen + 8);
        }
    else
        {
        t01catalog_entrytype(5 , (unsigned char ) sql__setrang((*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 18244)).integer.C_50.syskey.integer.C_2.sentrytyp[1] , 0 , 255));
        t01buf1(5 , &(*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 18245)).integer.C_50.syskey , 1 , (int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 18246)).integer.C_50.syskey.skeylen + 8);
        }
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 18249)).integer.C_4.sp1r_returncode == 0)
        {
        ptoc_Var109 = &*acv;
        ptoc_Var110 = (struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 18251);
        ptoc_Var111 = &ptoc_Var110->integer.C_50.syskey;
        ak10find_syskey(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 18253)).integer.C_50.syskey , &find_info);
        if ((int ) find_info.fi_res == 1)
            {
            via_kb = (char ) sql__setrang((_ptoc_MEMCMP(unsigned char , 2, ptoc_Var111->integer.C_2.sentrytyp , "\0\200") < 0 ? 1 : 0) || (_ptoc_MEMCMP(unsigned char , 2, ptoc_Var111->integer.C_2.sentrytyp , "\0\260") > 0 ? 1 : 0) && !(char ) sql__setrang(ptoc_Var109->a_in_ddl_trigger , 0 , 1) && (0 || (ptoc_Var109->a_pars_curr.integer.C_1.fileHandling_gg00[0] & 32) == 0) && !a663parse_for_execute(&(*acv)) , 0 , 1);
            *b_err = 0;
            if (((ptoc_Var112 = (int ) ptoc_Var111->integer.C_2.sentrytyp[1]) == 1 || ptoc_Var112 == 177) && (_ptoc_MEMCMP(unsigned char , 2, ptoc_Var111->integer.C_2.slinkage , "\0\001") == 0 ? 1 : 0))
                {
                if ((int ) ptoc_Var110->integer.C_1.sbase.btablekind != 8)
                    {
                    a061compress_baserec(&ptoc_Var110->integer.C_1.sbase , &(*b_err));
                    }
                }
            if ((int ) *b_err == 0)
                {
                if ((char ) sql__setrang(via_kb , 0 , 1))
                    {
                    ak10add_upd_perm_record(&(*acv) , &find_info , *syspoint , 35 , &(*b_err));
                    if ((int ) *b_err == 530)
                        {
                        *b_err = 540;
                        ak10store_syskey(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 18283)).integer.C_50.syskey);
                        }
                    }
                else
                    {
                    if ((ptoc_Var112 = (int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 18289)).integer.C_50.syskey.integer.C_2.sentrytyp[1]) == 130 || ptoc_Var112 == 129 || ptoc_Var112 == 132 || ptoc_Var112 == 178 || ptoc_Var112 == 190)
                        {
                        (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 18293)).integer.C_23.sparsinfo.integer.C_1.p_fullen = (int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 18293)).integer.C_0.b_sl;
                        }
                    t01buf(5 , sql__nil(*syspoint , "vak10.p" , 18295) , 1 , (int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 18295)).integer.C_0.b_sl);
                    }
                }
            if ((int ) *b_err == 0)
                {
                ptoc_Var113 = (struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 18313);
                if (!(char ) sql__setrang(via_kb , 0 , 1))
                    {
                    if (1 && (ptoc_Var113->integer.C_1.cd_state[0] & 8) != 0 || 1 && (ptoc_Var113->integer.C_1.cd_state[0] & 16) != 0)
                        {
                        *b_err = 540;
                        ak10store_syskey(&(*acv) , &ptoc_Var113->integer.C_1.cd_syskey);
                        }
                    else
                        {
                        _ptoc_MOVE(unsigned char , 1, ptoc_Var113->integer.C_1.cd_state, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var114 , ptoc_Var113->integer.C_1.cd_state , ptoc_Var108 , sizeof(unsigned char [1])));
                        }
                    }
                if (_ptoc_MEMCMP(unsigned char , 2, (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 18328)).integer.C_50.syskey.integer.C_2.sentrytyp , "\0\300") == 0 ? 1 : 0)
                    {
                    *syspoint = (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 18330)).integer.C_59.scolnames.boolean.C_false.cnlast;
                    }
                ak10free_rest(&(*acv) , *syspoint , (int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 18332)).integer.C_0.b_sl);
                if ((ptoc_Var112 = (int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 18333)).integer.C_50.syskey.integer.C_2.sentrytyp[1]) == 1 || ptoc_Var112 == 177)
                    {
                    if ((int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 18336)).integer.C_1.sbase.btablekind != 8)
                        {
                        last.boolean.C_true.colp = (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 18339)).integer.C_1.sbase.bptr;
                        while ((*(struct tak_basecolinfo *) sql__nil(last.boolean.C_true.colp , "vak10.p" , 18341)).bci_next != (struct tak_basecolinfo *) (int *) 0)
                            last.boolean.C_true.colp = (*(struct tak_basecolinfo *) sql__nil(last.boolean.C_true.colp , "vak10.p" , 18342)).bci_next;
                        ak10free_rest(&(*acv) , last.boolean.C_false.sysp , sizeof(struct tak_basecolinfo ) - sizeof(unsigned char [4096]) + (*(struct tak_basecolinfo *) sql__nil(last.boolean.C_true.colp , "vak10.p" , 18348)).bci_index + -1);
                        }
                    }
                }
            }
        else
            {
            *b_err = 1930;
            ak10store_syskey(&(*acv) , &ptoc_Var110->integer.C_50.syskey);
            }
        }
    ak10verify_cache(&(*acv));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 18366)).integer.C_4.sp1r_returncode != 0)
        {
        *b_err = 400;
        }
    cccprint("<a10add_sysinfo ");
    }



int a10_all_release(acv)
tak_all_command_glob *acv;
    {
    tak_cache_dir_pointer p;
    tak_cache_dir_pointer first;
    tak_sysdirectory *ptoc_Var117;
    cccprint("AK10  a10_all_re");
    p.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
    if ((char ) sql__setrang(a10chk , 0 , 1))
        {
        ak10CacheCheck(&(*acv));
        }
    ptoc_Var117 = &(*acv).a_sysdir;
    if (ptoc_Var117->ci_cache_p != (tsp00_Buf *) (int *) 0)
        {
        first = ptoc_Var117->ci_cache_dir_trailor;
        p = (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var117->ci_cache_dir_trailor.integer.C_1.cdir_ptr , "vak10.p" , 18396)).integer.C_1.cd_fix_prev;
        while (p.integer.C_1.cdir_ptr != ptoc_Var117->ci_cache_dir_header.integer.C_1.cdir_ptr)
            if ((*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 18399)).integer.C_1.cd_pos + (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 18399)).integer.C_1.cd_check != 0)
                {
                ak10cache_error(&(*acv) , "a10all_release          " , p);
                p = ptoc_Var117->ci_cache_dir_header;
                }
            else
                {
                if (1 && ((*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 18407)).integer.C_1.cd_state[0] & 2) != 0)
                    {
                    (*(struct tak_cache_dir_entry *) sql__nil(first.integer.C_1.cdir_ptr , "vak10.p" , 18410)).integer.C_1.cd_fix_prev = p;
                    (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 18411)).integer.C_1.cd_fix_next = first;
                    first = p;
                    }
                else
                    {
                    (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 18415)).integer.C_1.cd_fix_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
                    }
                p = (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 18417)).integer.C_1.cd_fix_prev;
                }
        (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var117->ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 18422)).integer.C_1.cd_fix_next = first;
        (*(struct tak_cache_dir_entry *) sql__nil(first.integer.C_1.cdir_ptr , "vak10.p" , 18423)).integer.C_1.cd_fix_prev = ptoc_Var117->ci_cache_dir_header;
        }
    cccprint("<a10_all_release");
    }



int a10Displace(acv)
tak_all_command_glob *acv;
    {
    tak_cache_dir_pointer next;
    tak_cache_dir_pointer curr;
    int notDisplacedCnt;
    int displaceCnt;
    tak_sysdirectory *ptoc_Var118;
    struct tak_cache_dir_entry *ptoc_Var119;
    cccprint("AK10  a10Displac");
    ptoc_Var118 = &(*acv).a_sysdir;
    if (ptoc_Var118->ci_cache_p != (tsp00_Buf *) (int *) 0)
        {
        if (ptoc_Var118->ci_displaceCnt > 0)
            {
            displaceCnt = ptoc_Var118->ci_displaceCnt;
            t01int4(0 , "displaceCnt " , ptoc_Var118->ci_displaceCnt);
            notDisplacedCnt = 0;
            next = (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var118->ci_cache_dir_trailor.integer.C_1.cdir_ptr , "vak10.p" , 18457)).integer.C_1.cd_lru_prev;
            while (next.integer.C_1.cdir_ptr != ptoc_Var118->ci_cache_dir_header.integer.C_1.cdir_ptr)
                {
                curr = next;
                ptoc_Var119 = (struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 18461);
                next = (*(struct tak_cache_dir_entry *) sql__nil(next.integer.C_1.cdir_ptr , "vak10.p" , 18463)).integer.C_1.cd_lru_prev;
                if (1 && (ptoc_Var119->integer.C_1.cd_state[0] & 64) != 0)
                    {
                    displaceCnt = displaceCnt + -1;
                    if (1 && (ptoc_Var119->integer.C_1.cd_state[0] & 2) != 0)
                        {
                        notDisplacedCnt = notDisplacedCnt + 1;
                        }
                    else
                        {
                        ak10block_into_freelist(&(*acv) , curr , 1 , 1);
                        if (ptoc_Var118->ci_displaceCnt == 0)
                            {
                            next.integer.C_1.cdir_ptr = ptoc_Var118->ci_cache_dir_header.integer.C_1.cdir_ptr;
                            }
                        }
                    }
                }
            if (displaceCnt != 0)
                {
                g01abort(51114 , "AK CACHE" , "ci_displaceCnt <> 0     " , displaceCnt);
                }
            ptoc_Var118->ci_displaceCnt = notDisplacedCnt;
            }
        }
    cccprint("<a10Displace    ");
    }



int a10_cache_delete(acv, is_rollback)
tak_all_command_glob *acv;
char is_rollback;
    {
    tak_cache_dir_pointer next;
    tak_cache_dir_pointer curr;
    tak_sysdirectory *ptoc_Var120;
    struct tak_cache_dir_entry *ptoc_Var121;
    cccprint("AK10  a10_cache_");
    ptoc_Var120 = &(*acv).a_sysdir;
    if (ptoc_Var120->ci_cache_p != (tsp00_Buf *) (int *) 0)
        {
        next = (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var120->ci_cache_dir_trailor.integer.C_1.cdir_ptr , "vak10.p" , 18519)).integer.C_1.cd_lru_prev;
        while (next.integer.C_1.cdir_ptr != ptoc_Var120->ci_cache_dir_header.integer.C_1.cdir_ptr)
            {
            curr = next;
            ptoc_Var121 = (struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 18523);
            next = (*(struct tak_cache_dir_entry *) sql__nil(next.integer.C_1.cdir_ptr , "vak10.p" , 18525)).integer.C_1.cd_lru_prev;
            if (((_ptoc_MEMCMP(unsigned char , 2, ptoc_Var121->integer.C_1.cd_syskey.integer.C_2.sentrytyp , "\0\265") == 0 ? 0 : 1) || !(char ) sql__setrang((*acv).a_intern_explain , 0 , 1)) && (0 || (ptoc_Var121->integer.C_1.cd_state[0] & 2) == 0))
                {
                t01str30(0 , "dislodge block                ");
                t01buf1(0 , &ptoc_Var121->integer.C_1.cd_syskey , 9 , (int ) ptoc_Var121->integer.C_1.cd_syskey.skeylen + 8);
                ak10temp_add(&(*acv) , curr , (char ) sql__setrang(is_rollback , 0 , 1));
                ak10block_into_freelist(&(*acv) , curr , 1 , 1);
                }
            }
        ptoc_Var120->ci_tmp_dislodged = 0;
        ak10td_dirlist(&(*acv) , 0);
        ak10td_freelist(&(*acv) , 0);
        }
    cccprint("<a10_cache_delet");
    }



int a10del_sysinfo(acv, syskey, b_err)
tak_all_command_glob *acv;
tgg_sysinfokey *syskey;
tgg_basis_error *b_err;
    {
    char del;
    char del_succ;
    tak10_find_info find_info;
    tgg_sysinfokey aux_key;
    tgg_mess_block mblock;
    tak10qual qual;
    struct ptoc_Type201
      {
      union  
        {
        struct  
          {
          tgg_sysinfokey dt_syskey;
          } C_true;
        struct  
          {
          tsp_buf dt_buf;
          } C_false;
        } boolean;
      } data;
    int ptoc_Var122;
    cccprint("AK10  a10del_sys");
    del_succ = 0;
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 18582)).integer.C_4.sp1r_returncode == 0)
        {
        if (t01trace(0))
            {
            t01catalog_entrytype(0 , (unsigned char ) sql__setrang((*syskey).integer.C_2.sentrytyp[1] , 0 , 255));
            t01buf1(0 , &(*syskey) , 1 , (int ) (*syskey).skeylen + 8);
            }
        else
            {
            t01catalog_entrytype(5 , (unsigned char ) sql__setrang((*syskey).integer.C_2.sentrytyp[1] , 0 , 255));
            t01buf1(5 , &(*syskey) , 1 , (int ) (*syskey).skeylen + 8);
            }
        *b_err = 0;
        ak10find_syskey(&(*acv) , &(*syskey) , &find_info);
        if ((int ) find_info.fi_res == 1)
            {
            xtd_directory(0 , find_info.fi_ptr);
            }
        del = 1;
        if ((int ) find_info.fi_res == 1)
            {
            if (_ptoc_MEMCMP(unsigned char , 2, (*syskey).integer.C_2.sentrytyp , "\0\200") > 0 ? 1 : 0)
                {
                if (0 || ((*(struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 18609)).integer.C_1.cd_state[0] & 16) == 0)
                    {
                    del = 0;
                    }
                else
                    {
                    switch ((int ) (*syskey).integer.C_2.sentrytyp[1])
                        {
                        case 130 :
                        case 129 :
                        case 132 :
                        case 178 :
                        case 190 :
                            if ((*(struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 18616)).integer.C_2.cd_sysinfo.integer.C_23.sparsinfo.integer.C_1.p_fullen > 8096)
                                {
                                del_succ = 1;
                                }
                            break;
                        case 192 :
                            del_succ = (char ) sql__setrang((int ) (*(struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 18623)).integer.C_2.cd_sysinfo.integer.C_0.b_sl == 8096 , 0 , 1);
                            break;
                        case 193 :
                        case 24 :
                        case 149 :
                            del_succ = 1;
                            break;
                        case 177 :
                            del_succ = 1;
                            break;
                        default :
                            break;
                        }
                    }
                }
            else
                {
                del_succ = (char ) sql__setrang(_ptoc_MEMCMP(unsigned char , 2, (*syskey).integer.C_2.sentrytyp , "\0\030") == 0 ? 1 : 0 , 0 , 1);
                }
            }
        else
            {
            if ((ptoc_Var122 = (int ) (*syskey).integer.C_2.sentrytyp[1]) == 192 || ptoc_Var122 == 130 || ptoc_Var122 == 129 || ptoc_Var122 == 178 || ptoc_Var122 == 190 || ptoc_Var122 == 193 || ptoc_Var122 == 24 || ptoc_Var122 == 132 || ptoc_Var122 == 149 || ptoc_Var122 == 177)
                {
                del_succ = 1;
                }
            }
        if ((char ) sql__setrang(del , 0 , 1))
            {
            aux_key = *syskey;
            switch ((int ) aux_key.integer.C_2.sentrytyp[1])
                {
                case 77 :
                case 88 :
                    if ((char ) sql__setrang(g01unicode , 0 , 1))
                        {
                        aux_key.skeylen = (short ) sql__setrang((short ) (s30unilnr((tsp00_MoveObj *) aux_key.integer.C_13.sidentifier , "\0 " , 1 , sizeof(unsigned char [64]) - sizeof(tsp00_C8 )) + 12) , -32768 , 32767);
                        }
                    else
                        {
                        aux_key.skeylen = (short ) sql__setrang((short ) (s30lnr(aux_key.integer.C_13.sidentifier , (unsigned char)' ' , 1 , sizeof(unsigned char [64]) - sizeof(tsp00_C8 )) + 12) , -32768 , 32767);
                        }
                    break;
                case 65 :
                case 78 :
                case 22 :
                case 81 :
                case 79 :
                    aux_key.skeylen = (short ) sql__setrang((short ) (a061identifier_len(aux_key.integer.C_13.sidentifier) + 12) , -32768 , 32767);
                    break;
                default :
                    break;
                }
            aux_key.sreclen = 0;
            if ((_ptoc_MEMCMP(unsigned char , 2, (*syskey).integer.C_2.sentrytyp , "\0\200") < 0 ? 1 : 0) || (_ptoc_MEMCMP(unsigned char , 2, (*syskey).integer.C_2.sentrytyp , "\0\260") > 0 ? 1 : 0) && !(char ) sql__setrang((*acv).a_in_ddl_trigger , 0 , 1) && (0 || ((*acv).a_pars_curr.integer.C_1.fileHandling_gg00[0] & 32) == 0) && !a663parse_for_execute(&(*acv)))
                {
                do
                    {
                    g01mblock_init(&(*acv).a_transinf.tri_trans , 18 , 0 , &mblock);
                    g01stack_init(&mblock , (tgg00_StackList *) (int *) 0 , 0 , (tgg00_QualBuf *) &qual , sizeof(tak10qual ));
                    g01datapart_init(&mblock , (tgg00_DataPart *) &data , sizeof(struct ptoc_Type201 ));
                    mblock.integer.C_2.mb_qual_len = sizeof(unsigned char [112]);
                    mblock.integer.C_2.mb_data_len = (int ) aux_key.skeylen + 8;
                    data.boolean.C_true.dt_syskey = aux_key;
                    ak10send_mess_block(&(*acv) , &mblock , (*syskey).integer.C_2.sentrytyp , 0 , &(*b_err));
                    if ((char ) sql__setrang(del_succ , 0 , 1))
                        {
                        if ((int ) aux_key.skeylen == (int ) (*syskey).skeylen)
                            {
                            aux_key.skeylen = (short ) sql__setrang((short ) ((int ) aux_key.skeylen + 1) , -32768 , 32767);
                            aux_key.integer.C_8.all[sql__setrang((int ) aux_key.skeylen , 1 , 140) + -1] = (unsigned char)'\001';
                            }
                        else
                            {
                            aux_key.integer.C_8.all[sql__setrang((int ) aux_key.skeylen , 1 , 140) + -1] = (unsigned char ) sql__setrang((unsigned char ) sql__succ((int ) aux_key.integer.C_8.all[sql__setrang((int ) aux_key.skeylen , 1 , 140) + -1] , 0 , 255 , "vak10.p" , 18703) , 0 , 255);
                            }
                        }
                    }
                while (!(!(char ) sql__setrang(del_succ , 0 , 1) || (int ) *b_err != 0));
                if ((int ) *b_err == 1200 && (char ) sql__setrang(del_succ , 0 , 1) && (int ) aux_key.skeylen > (int ) (*syskey).skeylen)
                    {
                    *b_err = 0;
                    }
                }
            else
                {
                b07cdel_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , &(*syskey));
                if ((int ) find_info.fi_res != 1 && (int ) (*acv).a_transinf.tri_trans.trError_gg00 == 1200)
                    {
                    *b_err = 0;
                    }
                else
                    {
                    *b_err = (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                    if ((char ) sql__setrang(del_succ , 0 , 1) && (int ) *b_err == 0)
                        {
                        if ((int ) (*syskey).integer.C_2.sentrytyp[1] == 177)
                            {
                            do
                                {
                                a06inc_linkage(aux_key.integer.C_2.slinkage);
                                b07cdel_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , &aux_key);
                                }
                            while (!((int ) (*acv).a_transinf.tri_trans.trError_gg00 != 0));
                            }
                        else
                            {
                            aux_key.skeylen = (short ) sql__setrang((short ) ((int ) aux_key.skeylen + 1) , -32768 , 32767);
                            aux_key.integer.C_8.all[sql__setrang((int ) aux_key.skeylen , 1 , 140) + -1] = (unsigned char)'\0';
                            do
                                {
                                aux_key.integer.C_8.all[sql__setrang((int ) aux_key.skeylen , 1 , 140) + -1] = (unsigned char ) sql__setrang((unsigned char ) sql__succ((int ) aux_key.integer.C_8.all[sql__setrang((int ) aux_key.skeylen , 1 , 140) + -1] , 0 , 255 , "vak10.p" , 18744) , 0 , 255);
                                b07cdel_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , &aux_key);
                                }
                            while (!((int ) (*acv).a_transinf.tri_trans.trError_gg00 != 0));
                            }
                        }
                    }
                (*acv).a_transinf.tri_trans.trError_gg00 = 0;
                }
            }
        if ((int ) find_info.fi_res == 1)
            {
            ak10block_into_freelist(&(*acv) , find_info.fi_ptr , 1 , 1);
            }
        if ((int ) *b_err == 1200)
            {
            *b_err = 1930;
            ak10store_syskey(&(*acv) , &(*syskey));
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 18774)).integer.C_4.sp1r_returncode != 0)
        {
        *b_err = 400;
        }
    ak10verify_cache(&(*acv));
    cccprint("<a10del_sysinfo ");
    }



int a10get_sysinfo(acv, syskey, dstate, syspoint, b_err)
tak_all_command_glob *acv;
tgg_sysinfokey *syskey;
tak_directory_state dstate;
tak_sysbufferaddress *syspoint;
tgg_basis_error *b_err;
    {
    int keylen;
    tak10_find_info find_info;
    tgg_rec_buf rbuf;
    tak_all_command_glob *ptoc_Var123;
    tak_sysdirectory *ptoc_Var124;
    struct tak_cache_dir_entry *ptoc_Var125;
    cccprint("AK10  a10get_sys");
    *syspoint = (struct tak_systembuffer *) (int *) 0;
    ptoc_Var123 = &*acv;
    ptoc_Var124 = &ptoc_Var123->a_sysdir;
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var123->a_return_segm , "vak10.p" , 18798)).integer.C_4.sp1r_returncode == 0)
        {
        *b_err = 0;
        find_info.fi_kind = 1;
        if (t01trace(0))
            {
            t01catalog_entrytype(0 , (unsigned char ) sql__setrang((*syskey).integer.C_2.sentrytyp[1] , 0 , 255));
            t01buf1(0 , &(*syskey) , 9 , (int ) (*syskey).skeylen + 8);
            }
        else
            {
            t01catalog_entrytype(5 , (unsigned char ) sql__setrang((*syskey).integer.C_2.sentrytyp[1] , 0 , 255));
            t01buf1(5 , &(*syskey) , 9 , (int ) (*syskey).skeylen + 8);
            }
        ak10find_syskey(&(*acv) , &(*syskey) , &find_info);
        if ((int ) find_info.fi_res == 1)
            {
            b73directory_ref_statistic(0);
            ptoc_Var125 = (struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 18822);
            if ((int ) (*acv).a_iso_level > 0 && (int ) (*acv).a_command_kind == 7 && (_ptoc_MEMCMP(unsigned char , 2, (*syskey).integer.C_2.sentrytyp , "\0\200") < 0 ? 1 : 0) && ptoc_Var125->integer.C_1.cd_cmd_no != (*acv).a_command_count)
                {
                find_info.fi_res = 0;
                *syspoint = (struct tak_systembuffer *) &ptoc_Var125->integer.C_2.cd_sysinfo;
                a10key_del(&(*acv) , &(*syspoint));
                }
            else
                {
                if (_ptoc_MEMCMP(unsigned char , 2, (*syskey).integer.C_2.sentrytyp , "\0\200") > 0 ? 1 : 0)
                    {
                    if (ptoc_Var124->ci_tmp_read_cnt < 2147483647)
                        {
                        ptoc_Var124->ci_tmp_read_cnt = ptoc_Var124->ci_tmp_read_cnt + 1;
                        ptoc_Var124->ci_tmp_read_found = ptoc_Var124->ci_tmp_read_found + 1;
                        }
                    else
                        {
                        ptoc_Var124->ci_tmp_read_found = ak10hit_rate(ptoc_Var124->ci_tmp_read_found , ptoc_Var124->ci_tmp_read_cnt);
                        ptoc_Var124->ci_tmp_read_cnt = 1000;
                        }
                    }
                ptoc_Var125->integer.C_1.cd_cmd_no = (*acv).a_command_count;
                *syspoint = (struct tak_systembuffer *) &ptoc_Var125->integer.C_2.cd_sysinfo;
                (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var125->integer.C_1.cd_lru_prev.integer.C_1.cdir_ptr , "vak10.p" , 18858)).integer.C_1.cd_lru_next = ptoc_Var125->integer.C_1.cd_lru_next;
                (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var125->integer.C_1.cd_lru_next.integer.C_1.cdir_ptr , "vak10.p" , 18859)).integer.C_1.cd_lru_prev = ptoc_Var125->integer.C_1.cd_lru_prev;
                ptoc_Var125->integer.C_1.cd_lru_next = (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var124->ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 18862)).integer.C_1.cd_lru_next;
                ptoc_Var125->integer.C_1.cd_lru_prev = ptoc_Var124->ci_cache_dir_header;
                (*(struct tak_cache_dir_entry *) sql__nil((*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var124->ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 18864)).integer.C_1.cd_lru_next.integer.C_1.cdir_ptr , "vak10.p" , 18864)).integer.C_1.cd_lru_prev = find_info.fi_ptr;
                (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var124->ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 18867)).integer.C_1.cd_lru_next = find_info.fi_ptr;
                if ((int ) dstate == 0 && ptoc_Var125->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr == (struct tak_cache_dir_entry *) (int *) 0)
                    {
                    ptoc_Var125->integer.C_1.cd_fix_next = (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var124->ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 18875)).integer.C_1.cd_fix_next;
                    ptoc_Var125->integer.C_1.cd_fix_prev = ptoc_Var124->ci_cache_dir_header;
                    (*(struct tak_cache_dir_entry *) sql__nil((*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var124->ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 18877)).integer.C_1.cd_fix_next.integer.C_1.cdir_ptr , "vak10.p" , 18877)).integer.C_1.cd_fix_prev = find_info.fi_ptr;
                    (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var124->ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 18880)).integer.C_1.cd_fix_next = find_info.fi_ptr;
                    }
                }
            }
        if ((int ) find_info.fi_res != 1)
            {
            keylen = (int ) (*syskey).skeylen;
            switch ((int ) (*syskey).integer.C_2.sentrytyp[1])
                {
                case 77 :
                case 88 :
                    if ((char ) sql__setrang(g01unicode , 0 , 1))
                        {
                        (*syskey).skeylen = (short ) sql__setrang((short ) (s30unilnr((tsp00_MoveObj *) (*syskey).integer.C_13.sidentifier , "\0 " , 1 , sizeof(unsigned char [64]) - sizeof(tsp00_C8 )) + 12) , -32768 , 32767);
                        }
                    else
                        {
                        (*syskey).skeylen = (short ) sql__setrang((short ) (s30lnr((*syskey).integer.C_13.sidentifier , (unsigned char)' ' , 1 , sizeof(unsigned char [64]) - sizeof(tsp00_C8 )) + 12) , -32768 , 32767);
                        }
                    break;
                case 65 :
                case 78 :
                case 22 :
                case 81 :
                case 79 :
                    (*syskey).skeylen = (short ) sql__setrang((short ) (a061identifier_len((*syskey).integer.C_13.sidentifier) + 12) , -32768 , 32767);
                    break;
                default :
                    break;
                }
            if (_ptoc_MEMCMP(unsigned char , 2, (*syskey).integer.C_2.sentrytyp , "\0\200") > 0 ? 1 : 0)
                {
                b07cget_record(&ptoc_Var123->a_transinf.tri_trans , &ptoc_Var123->a_pars_curr , &(*syskey) , &rbuf);
                *b_err = (short ) sql__setrang(ptoc_Var123->a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                ptoc_Var123->a_transinf.tri_trans.trError_gg00 = 0;
                if ((char ) sql__setrang(ptoc_Var124->ci_tmp_dislodged , 0 , 1) && (int ) *b_err == 0)
                    {
                    ak10check_hit_rate(&(*acv));
                    }
                }
            else
                {
                ak10get_perm_record(&(*acv) , &(*syskey) , &rbuf , &(*b_err));
                }
            if ((int ) *b_err == 0)
                {
                b73directory_ref_statistic(1);
                ak10put_mbuf_into_cache(&(*acv) , &rbuf , (unsigned char ) sql__setrang(dstate , 0 , 1) , &find_info , &(*syskey) , &(*syspoint));
                }
            else
                {
                (*syskey).skeylen = (short ) sql__setrang((short ) keylen , -32768 , 32767);
                if ((int ) *b_err == 1200)
                    {
                    *b_err = 1930;
                    ak10store_syskey(&(*acv) , &(*syskey));
                    }
                }
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 18956)).integer.C_4.sp1r_returncode != 0)
        {
        *b_err = 400;
        }
    ak10verify_cache(&(*acv));
    cccprint("<a10get_sysinfo ");
    }



int a10_init_cache(acv)
tak_all_command_glob *acv;
    {
    tgg_basis_error e;
    tsp_int4 required_len;
    tsp_int4 dummy;
    struct ptoc_Type202
      {
      union  
        {
        struct  
          {
          tsp_bufaddr sbuf;
          } C_1;
        struct  
          {
          tak_site_state_ptr sptr;
          } C_2;
        struct  
          {
          tak_sysbufferaddress sysp;
          } C_3;
        struct  
          {
          tak10cache_header_ptr chptr;
          } C_4;
        } integer;
      } aux_ptr;
    tak_sysdirectory *ptoc_Var126;
    cccprint("AK10  a10_init_c");
    a10chk = 1;
    ak10alloc_mem(&(*acv) , 1 , 0 , &(*acv).a_sysdir.ci_cache_p , &dummy);
    ptoc_Var126 = &(*acv).a_sysdir;
    if (ptoc_Var126->ci_cache_p != (tsp00_Buf *) (int *) 0)
        {
        ptoc_Var126->ci_cmd_rollback_tree = b01niltree_id;
        ptoc_Var126->ci_tmp_dislodged = 0;
        ptoc_Var126->ci_displaceCnt = 0;
        ptoc_Var126->ci_tmp_read_cnt = 0;
        ptoc_Var126->ci_tmp_read_found = 0;
        aux_ptr.integer.C_1.sbuf = ptoc_Var126->ci_cache_p;
        _ptoc_ASSIGN(unsigned char , 4, (*(tak10cache_header *) sql__nil(aux_ptr.integer.C_4.chptr , "vak10.p" , 19004)).ch_label, ak10ChunkLabel)
        (*(tak10cache_header *) sql__nil(aux_ptr.integer.C_4.chptr , "vak10.p" , 19005)).ch_size = ak10init_cache_size;
        (*(tak10cache_header *) sql__nil(aux_ptr.integer.C_4.chptr , "vak10.p" , 19006)).ch_next_cache = (tsp00_Buf *) (int *) 0;
        (*(tak10cache_header *) sql__nil(aux_ptr.integer.C_4.chptr , "vak10.p" , 19007)).ch_filler = 0;
        ptoc_Var126->ci_cache_dir_header.integer.C_2.buf_ptr = s35add_bufaddr(ptoc_Var126->ci_cache_p , sizeof(tak10cache_header ));
        ptoc_Var126->ci_cache_dir_trailor.integer.C_2.buf_ptr = s35add_bufaddr(ptoc_Var126->ci_cache_p , sizeof(tak10cache_header ) + 48);
        ak10free_cache(&(*acv) , 0);
        required_len = sizeof(tgg00_SysInfoKey );
        a10new(&(*acv) , required_len , &(*acv).a_error_key_ptr);
        }
    else
        {
        ak10no_more_memory(&(*acv) , &e);
        a07_nb_put_error(&(*acv) , (short ) sql__setrang(e , -32768 , 32767) , 1 , a01_i_ak_cache);
        }
    cccprint("<a10_init_cache ");
    }



int a10_key_del(acv, syskey)
tak_all_command_glob *acv;
tgg_sysinfokey *syskey;
    {
    tak10_find_info find_info;
    cccprint("AK10  a10_key_de");
    if (t01trace(0))
        {
        t01catalog_entrytype(0 , (unsigned char ) sql__setrang((*syskey).integer.C_2.sentrytyp[1] , 0 , 255));
        t01buf1(0 , &(*syskey) , 1 , (int ) (*syskey).skeylen + 8);
        }
    else
        {
        t01catalog_entrytype(5 , (unsigned char ) sql__setrang((*syskey).integer.C_2.sentrytyp[1] , 0 , 255));
        t01buf1(5 , &(*syskey) , 1 , (int ) (*syskey).skeylen + 8);
        }
    ak10find_syskey(&(*acv) , &(*syskey) , &find_info);
    if ((int ) find_info.fi_res == 1)
        {
        ak10temp_add(&(*acv) , find_info.fi_ptr , 0);
        ak10block_into_freelist(&(*acv) , find_info.fi_ptr , 1 , 1);
        }
    ak10verify_cache(&(*acv));
    cccprint("<a10_key_del    ");
    }



int a10key_del(acv, syspointer)
tak_all_command_glob *acv;
tak_sysbufferaddress *syspointer;
    {
    tak_cache_dir_pointer curr;
    cccprint("AK10  a10key_del");
    if (*syspointer != (struct tak_systembuffer *) (int *) 0)
        {
        if (t01trace(0))
            {
            t01catalog_entrytype(0 , (unsigned char ) sql__setrang((*(struct tak_systembuffer *) sql__nil(*syspointer , "vak10.p" , 19082)).integer.C_50.syskey.integer.C_2.sentrytyp[1] , 0 , 255));
            t01buf1(0 , &(*(struct tak_systembuffer *) sql__nil(*syspointer , "vak10.p" , 19083)).integer.C_50.syskey , 1 , (int ) (*(struct tak_systembuffer *) sql__nil(*syspointer , "vak10.p" , 19084)).integer.C_50.syskey.skeylen + 8);
            }
        else
            {
            t01catalog_entrytype(5 , (unsigned char ) sql__setrang((*(struct tak_systembuffer *) sql__nil(*syspointer , "vak10.p" , 19088)).integer.C_50.syskey.integer.C_2.sentrytyp[1] , 0 , 255));
            t01buf1(5 , &(*(struct tak_systembuffer *) sql__nil(*syspointer , "vak10.p" , 19089)).integer.C_50.syskey , 1 , (int ) (*(struct tak_systembuffer *) sql__nil(*syspointer , "vak10.p" , 19090)).integer.C_50.syskey.skeylen + 8);
            }
        curr.integer.C_4.sys_ptr = *syspointer;
        *syspointer = (struct tak_systembuffer *) (int *) 0;
        curr.integer.C_2.buf_ptr = s35add_bufaddr(curr.integer.C_2.buf_ptr , -48);
        ak10temp_add(&(*acv) , curr , 0);
        ak10block_into_freelist(&(*acv) , curr , 1 , 1);
        ak10verify_cache(&(*acv));
        }
    cccprint("<a10key_del     ");
    }

static
  tgg00_LockReqState ptoc_Var127 = 
  {
  (unsigned char)'\002'
  };
static
  tgg00_LockReqState ptoc_Var128 = 
  {
  (unsigned char)'\0'
  };


int a10_lock_sysinfo(acv, syskey, lockm)
tak_all_command_glob *acv;
tgg_sysinfokey *syskey;
tgg00_LockReqMode lockm;
    {
    tgg_basis_error b_err;
    tgg_mess_block mblock;
    tgg_qual_buf qual;
    tsp_buf data;
    tgg00_Lock *ptoc_Var129;
    cccprint("AK10  a10_lock_s");
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 19120)).integer.C_4.sp1r_returncode == 0)
        {
        t01buf1(5 , &(*syskey) , 9 , (int ) (*syskey).skeylen + 8);
        b_err = 0;
        g01mblock_init(&(*acv).a_transinf.tri_trans , 40 , 0 , &mblock);
        g01qual_init(&mblock , (tgg00_QualBuf *) &qual , sizeof(tgg00_QualBuf ));
        g01datapart_init(&mblock , (tgg00_DataPart *) data , sizeof(tsp00_Buf ));
        mblock.integer.C_2.mb_struct = 6;
        (*(tgg00_QualBuf *) sql__nil(mblock.integer.C_4.mb_qual , "vak10.p" , 19131)).integer.C_7.mlock_cnt = 1;
        ptoc_Var129 = &(*(tgg00_QualBuf *) sql__nil(mblock.integer.C_4.mb_qual , "vak10.p" , 19132)).integer.C_7.mlock[0];
        if (_ptoc_MEMCMP(unsigned char , 2, (*syskey).integer.C_2.sentrytyp , "\0@") > 0 ? 1 : 0)
            {
            _ptoc_ASSIGN(unsigned char , 8, ptoc_Var129->lockTabId_gg00, g01tabid.sys2_cat.integer.C_4.fileTabId_gg00)
            ptoc_Var129->lockKeyLen_gg00 = (short ) sql__setrang((*syskey).skeylen , -32768 , 32767);
            }
        else
            {
            _ptoc_ASSIGN(unsigned char , 8, ptoc_Var129->lockTabId_gg00, (*syskey).integer.C_2.stableid)
            ptoc_Var129->lockKeyLen_gg00 = 8;
            }
        ptoc_Var129->lockMode_gg00 = (unsigned char ) sql__setrang(lockm , 0 , 7);
        if ((int ) ptoc_Var129->lockMode_gg00 == 2)
            {
            _ptoc_MOVE(unsigned char , 1, ptoc_Var129->lockState_gg00, ptoc_Var127);
            }
        else
            {
            _ptoc_MOVE(unsigned char , 1, ptoc_Var129->lockState_gg00, ptoc_Var128);
            }
        ptoc_Var129->lockKeyPos_gg00 = 1;
        g10mv("VAK10 " , 4 , sizeof(tgg00_SysInfoKey ) , mblock.integer.C_2.mb_data_size , &(*syskey) , 9 , (*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak10.p" , 19157)).integer.C_1.mbp_buf , 1 , (int ) ptoc_Var129->lockKeyLen_gg00 , &b_err);
        mblock.integer.C_2.mb_qual_len = 20;
        mblock.integer.C_2.mb_data_len = (int ) ptoc_Var129->lockKeyLen_gg00 + 8;
        if ((int ) b_err == 0)
            {
            a06lsend_mess_buf(&(*acv) , &mblock , 0 , &b_err);
            }
        if ((int ) b_err == 5210)
            {
            k53wait(sql__nil(mblock.integer.C_9.mb_trns , "vak10.p" , 19169) , 40 , 0);
            b_err = (short ) sql__setrang((*(tgg00_TransContext *) sql__nil(mblock.integer.C_9.mb_trns , "vak10.p" , 19170)).trError_gg00 , -32768 , 32767);
            }
        if ((int ) b_err != 0)
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
            }
        }
    cccprint("<a10_lock_sysinf");
    }



int a10next_sysinfo(acv, syskey, stop_prefix, dstate, ptoc_ptr_rec_kind, syspoint, b_err)
tak_all_command_glob *acv;
tgg_sysinfokey *syskey;
int stop_prefix;
tak_directory_state dstate;
tsp_c2 ptoc_ptr_rec_kind;
tak_sysbufferaddress *syspoint;
tgg_basis_error *b_err;
    {
    int entrypos;
    tak10_find_info find_info;
    tgg00_StackEntry stack[6];
    tsp_buf data;
    tak10qual qual;
    tgg_mess_block mblock;
    tgg00_QualBuf *ptoc_Var130;
    tgg00_StackEntry *ptoc_Var131;
    tgg00_StackEntry *ptoc_Var132;
    tgg00_StackEntry *ptoc_Var133;
    tgg00_StackEntry *ptoc_Var134;
    tgg00_StackEntry *ptoc_Var135;
    tgg00_StackEntry *ptoc_Var136;
    tsp_c2 rec_kind;
    _ptoc_MOVE(char, sizeof(tsp_c2 ), rec_kind, ptoc_ptr_rec_kind);
    cccprint("AK10  a10next_sy");
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 19203)).integer.C_4.sp1r_returncode == 0)
        {
        t01buf1(5 , &(*syskey) , 9 , (int ) (*syskey).skeylen + 8);
        entrypos = 9;
        g01mblock_init(&(*acv).a_transinf.tri_trans , 61 , 41 , &mblock);
        g01stack_init(&mblock , (tgg00_StackList *) stack , sizeof(tgg00_StackEntry [6]) , (tgg00_QualBuf *) &qual , sizeof(tak10qual ));
        g01datapart_init(&mblock , (tgg00_DataPart *) data , sizeof(tsp00_Buf ));
        mblock.integer.C_2.mb_qual_len = sizeof(unsigned char [112]);
        ptoc_Var130 = (tgg00_QualBuf *) sql__nil(mblock.integer.C_4.mb_qual , "vak10.p" , 19215);
        ptoc_Var130->integer.C_2.mqual_pos = (short ) sql__setrang(ptoc_Var130->integer.C_2.mfirst_free , -32768 , 32767);
        if ((_ptoc_MEMCMP(unsigned char , 2, rec_kind , "\0\0") == 0 ? 1 : 0) || (_ptoc_MEMCMP(unsigned char , 2, rec_kind , "\0@") == 0 ? 1 : 0) || (_ptoc_MEMCMP(unsigned char , 2, rec_kind , "\0\200") == 0 ? 1 : 0))
            {
            ptoc_Var130->integer.C_2.mqual_cnt = 1;
            ptoc_Var131 = &(*(tgg00_StackList *) sql__nil(mblock.integer.C_5.mb_st , "vak10.p" , 19224))[0];
            ptoc_Var131->integer.C_1.etype = 30;
            ptoc_Var131->integer.C_1.eop = 0;
            ptoc_Var131->integer.C_1.epos = 0;
            ptoc_Var131->integer.C_1.elen_var = 0;
            ptoc_Var131->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
            ptoc_Var131->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
            }
        else
            {
            ptoc_Var130->integer.C_2.mqual_cnt = 3;
            ptoc_Var132 = &(*(tgg00_StackList *) sql__nil(mblock.integer.C_5.mb_st , "vak10.p" , 19238))[0];
            ptoc_Var132->integer.C_1.etype = 0;
            ptoc_Var132->integer.C_1.eop = 0;
            ptoc_Var132->integer.C_1.epos = (short ) sql__setrang((short ) entrypos , -32768 , 32767);
            ptoc_Var132->integer.C_1.elen_var = 2;
            ptoc_Var132->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
            ptoc_Var132->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
            ptoc_Var133 = &(*(tgg00_StackList *) sql__nil(mblock.integer.C_5.mb_st , "vak10.p" , 19248))[1];
            ptoc_Var133->integer.C_1.etype = 22;
            ptoc_Var133->integer.C_1.eop = 110;
            ptoc_Var133->integer.C_1.epos = 9;
            ptoc_Var133->integer.C_1.elen_var = 2;
            _ptoc_ASSIGN(unsigned char , 2, ptoc_Var133->integer.C_1.ecol_tab, "\0\0")
            ptoc_Var134 = &(*(tgg00_StackList *) sql__nil(mblock.integer.C_5.mb_st , "vak10.p" , 19261))[2];
            ptoc_Var134->integer.C_1.etype = 30;
            ptoc_Var134->integer.C_1.eop = 0;
            ptoc_Var134->integer.C_1.epos = 0;
            ptoc_Var134->integer.C_1.elen_var = 0;
            _ptoc_ASSIGN(unsigned char , 2, ptoc_Var134->integer.C_1.ecol_tab, "\0\0")
            (*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak10.p" , 19270)).integer.C_1.mbp_buf[8] = (unsigned char ) sql__setrang(rec_kind[0] , 0 , 255);
            (*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak10.p" , 19271)).integer.C_1.mbp_buf[9] = (unsigned char ) sql__setrang(rec_kind[1] , 0 , 255);
            }
        ptoc_Var130->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var130->integer.C_2.mqual_cnt , -32768 , 32767 , "vak10.p" , 19274) , -32768 , 32767);
        ptoc_Var130->integer.C_2.mstrat_pos = (short ) sql__setrang(ptoc_Var130->integer.C_2.mfirst_free , -32768 , 32767);
        ptoc_Var135 = &(*(tgg00_StackList *) sql__nil(mblock.integer.C_5.mb_st , "vak10.p" , 19277))[sql__setrang((int ) ptoc_Var130->integer.C_2.mfirst_free , 1 , 32767) + -1];
        ptoc_Var135->integer.C_1.etype = 38;
        ptoc_Var135->integer.C_1.eop = 0;
        ptoc_Var135->integer.C_1.epos = 11;
        ptoc_Var135->integer.C_1.elen_var = (short ) sql__setrang((*syskey).skeylen , -32768 , 32767);
        ptoc_Var135->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
        ptoc_Var135->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
        g10mv("VAK10 " , 5 , sizeof(tgg00_SysInfoKey ) , mblock.integer.C_2.mb_data_size , &(*syskey) , 9 , (*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak10.p" , 19290)).integer.C_1.mbp_buf , 11 , (int ) (*syskey).skeylen , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 19291)).integer.C_4.sp1r_returncode);
        mblock.integer.C_2.mb_data_len = (int ) (*syskey).skeylen + 10;
        ptoc_Var130->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var130->integer.C_2.mfirst_free , -32768 , 32767 , "vak10.p" , 19294) , -32768 , 32767);
        (*(tgg00_StackList *) sql__nil(mblock.integer.C_5.mb_st , "vak10.p" , 19296))[sql__setrang((int ) ptoc_Var130->integer.C_2.mfirst_free , 1 , 32767) + -1] = (*(tgg00_StackList *) sql__nil(mblock.integer.C_5.mb_st , "vak10.p" , 19296))[sql__setrang((int ) ptoc_Var130->integer.C_2.mstrat_pos , 1 , 32767) + -1];
        ptoc_Var136 = &(*(tgg00_StackList *) sql__nil(mblock.integer.C_5.mb_st , "vak10.p" , 19297))[sql__setrang((int ) ptoc_Var130->integer.C_2.mfirst_free , 1 , 32767) + -1];
        if (stop_prefix > 0)
            {
            ptoc_Var136->integer.C_1.epos = (short ) sql__setrang((short ) ((int ) (*syskey).skeylen + 11) , -32768 , 32767);
            ptoc_Var136->integer.C_1.elen_var = 140;
            }
        else
            {
            ptoc_Var136->integer.C_1.epos = 0;
            ptoc_Var136->integer.C_1.elen_var = 0;
            }
        ptoc_Var130->integer.C_2.mfirst_free = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var130->integer.C_2.mfirst_free , -32768 , 32767 , "vak10.p" , 19313) , -32768 , 32767);
        ptoc_Var130->integer.C_2.mstrat_cnt = 2;
        if (stop_prefix > 0)
            {
            g10mv("VAK10 " , 6 , sizeof(tgg00_SysInfoKey ) , mblock.integer.C_2.mb_data_size , &(*syskey) , 9 , (*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak10.p" , 19320)).integer.C_1.mbp_buf , mblock.integer.C_2.mb_data_len + 1 , stop_prefix , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 19322)).integer.C_4.sp1r_returncode);
            g10fil("VAK10 " , 7 , mblock.integer.C_2.mb_data_size , (*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak10.p" , 19324)).integer.C_1.mbp_buf , mblock.integer.C_2.mb_data_len + stop_prefix + 1 , 140 - stop_prefix , (unsigned char)'\377' , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 19327)).integer.C_4.sp1r_returncode);
            mblock.integer.C_2.mb_data_len = mblock.integer.C_2.mb_data_len + 140;
            }
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 19333)).integer.C_4.sp1r_returncode == 0)
            {
            ak10send_mess_block(&(*acv) , &mblock , rec_kind , 1 , &(*b_err));
            if ((int ) *b_err == 0)
                {
                a10expand_rec(&(*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak10.p" , 19341)).integer.C_3.mbp_rec , &(*b_err));
                find_info.fi_kind = 2;
                if ((int ) *b_err == 0)
                    {
                    ak10put_mbuf_into_cache(&(*acv) , &(*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak10.p" , 19345)).integer.C_3.mbp_rec , (unsigned char ) sql__setrang(dstate , 0 , 1) , &find_info , &(*syskey) , &(*syspoint));
                    }
                }
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 19354)).integer.C_4.sp1r_returncode != 0)
        {
        *b_err = 400;
        }
    ak10verify_cache(&(*acv));
    cccprint("<a10next_sysinfo");
    }



int a10_nil_get_sysinfo(acv, syskey, dstate, syslen, syspoint, b_err)
tak_all_command_glob *acv;
tgg_sysinfokey *syskey;
tak_directory_state dstate;
tsp_int2 syslen;
tak_sysbufferaddress *syspoint;
tgg_basis_error *b_err;
    {
    tak10_find_info find_info;
    tgg_buf_len_info rbuf;
    struct ptoc_Type203
      {
      union  
        {
        struct  
          {
          tak_sysbufferaddress mobj;
          } C_true;
        struct  
          {
          tak_basecolinfo_ptr cip;
          } C_false;
        } boolean;
      } p;
    int ptoc_Var137;
    cccprint("AK10  a10_nil_ge");
    *syspoint = (struct tak_systembuffer *) (int *) 0;
    if ((int ) syslen < 20)
        {
        a07_b_put_error(&(*acv) , 1020 , 1);
        }
    else
        {
        ak10size_check(&(*acv) , (*syskey).integer.C_2.sentrytyp , (int ) syslen);
        }
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 19392)).integer.C_4.sp1r_returncode == 0)
        {
        if (t01trace(0))
            {
            t01str30(0 , "===== a10_nil_get_sysinfo ====");
            t01catalog_entrytype(0 , (unsigned char ) sql__setrang((*syskey).integer.C_2.sentrytyp[1] , 0 , 255));
            t01buf1(0 , &(*syskey) , 9 , (int ) (*syskey).skeylen + 8);
            t01int4(0 , "required len" , (int ) syslen);
            }
        else
            {
            t01str30(5 , "===== a10_nil_get_sysinfo ====");
            t01catalog_entrytype(5 , (unsigned char ) sql__setrang((*syskey).integer.C_2.sentrytyp[1] , 0 , 255));
            t01buf1(5 , &(*syskey) , 9 , (int ) (*syskey).skeylen + 8);
            t01int4(5 , "required len" , (int ) syslen);
            }
        *b_err = 0;
        find_info.fi_kind = 0;
        ak10find_syskey(&(*acv) , &(*syskey) , &find_info);
        if ((int ) find_info.fi_res == 1)
            {
            *syspoint = (struct tak_systembuffer *) &(*(struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 19419)).integer.C_2.cd_sysinfo;
            if ((int ) dstate == 0 && (*(struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 19421)).integer.C_1.cd_fix_next.integer.C_1.cdir_ptr == (struct tak_cache_dir_entry *) (int *) 0)
                {
                (*(struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 19425)).integer.C_1.cd_fix_next = (*(struct tak_cache_dir_entry *) sql__nil((*acv).a_sysdir.ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 19426)).integer.C_1.cd_fix_next;
                (*(struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 19427)).integer.C_1.cd_fix_prev = (*acv).a_sysdir.ci_cache_dir_header;
                (*(struct tak_cache_dir_entry *) sql__nil((*(struct tak_cache_dir_entry *) sql__nil((*acv).a_sysdir.ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 19429)).integer.C_1.cd_fix_next.integer.C_1.cdir_ptr , "vak10.p" , 19429)).integer.C_1.cd_fix_prev = find_info.fi_ptr;
                (*(struct tak_cache_dir_entry *) sql__nil((*acv).a_sysdir.ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 19432)).integer.C_1.cd_fix_next = find_info.fi_ptr;
                }
            *b_err = 540;
            ak10store_syskey(&(*acv) , &(*syskey));
            }
        else
            {
            rbuf.boolean.C_false.ri.integer.C_11.basesyskey = *syskey;
            rbuf.boolean.C_false.ri.integer.C_21.rb_len = (short ) sql__setrang(syslen , -32768 , 32767);
            rbuf.boolean.C_false.ri.integer.C_21.rb_varcol_offset = 0;
            rbuf.boolean.C_false.ri.integer.C_21.rb_varcol_cnt = 0;
            ak10put_mbuf_into_cache(&(*acv) , &rbuf.boolean.C_true.rb , (unsigned char ) sql__setrang(dstate , 0 , 1) , &find_info , &(*syskey) , &(*syspoint));
            }
        }
    if (((ptoc_Var137 = (int ) (*syskey).integer.C_2.sentrytyp[1]) == 1 || ptoc_Var137 == 177) && (int ) find_info.fi_res != 1 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 19453)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) syslen >= sizeof(tak_baserecord ) - sizeof(tak_columnarr ))
            {
            (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 19459)).integer.C_1.sbase.breccnt = 0;
            (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 19460)).integer.C_1.sbase.bptr = (struct tak_basecolinfo *) (int *) 0;
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 19464)).integer.C_4.sp1r_returncode != 0)
        {
        *b_err = 400;
        }
    if ((int ) *b_err == 0 && ((ptoc_Var137 = (int ) (*syskey).integer.C_2.sentrytyp[1]) == 193 || ptoc_Var137 == 24 || ptoc_Var137 == 149))
        {
        (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 19472)).integer.C_62.smessblock.mbr_mess_block.integer.C_4.mb_qual = (tgg00_QualBuf *) (int *) 0;
        (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 19473)).integer.C_62.smessblock.mbr_mess_block.integer.C_3.mb_data = (tgg00_DataPart *) (int *) 0;
        (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 19474)).integer.C_62.smessblock.mbr_mess_block.integer.C_5.mb_st = (tgg00_StackList *) (int *) 0;
        (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 19475)).integer.C_62.smessblock.mbr_mess_block.integer.C_11.mb_strat = (tsp00_MoveObj *) (int *) 0;
        }
    ak10verify_cache(&(*acv));
    cccprint("<a10_nil_get_sys");
    }



int a10_rel_sysinfo(acv, syskey)
tak_all_command_glob *acv;
tgg_sysinfokey *syskey;
    {
    tak10_find_info find_info;
    struct tak_cache_dir_entry *ptoc_Var138;
    cccprint("AK10  a10_rel_sy");
    ak10find_syskey(&(*acv) , &(*syskey) , &find_info);
    if ((int ) find_info.fi_res == 1)
        {
        ptoc_Var138 = (struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 19497);
        xtd_directory(0 , find_info.fi_ptr);
        if (ptoc_Var138->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0 && (0 || (ptoc_Var138->integer.C_1.cd_state[0] & 2) == 0))
            {
            (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var138->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr , "vak10.p" , 19504)).integer.C_1.cd_fix_prev = ptoc_Var138->integer.C_1.cd_fix_prev;
            (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var138->integer.C_1.cd_fix_prev.integer.C_1.cdir_ptr , "vak10.p" , 19505)).integer.C_1.cd_fix_next = ptoc_Var138->integer.C_1.cd_fix_next;
            ptoc_Var138->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
            }
        }
    ak10verify_cache(&(*acv));
    cccprint("<a10_rel_sysinfo");
    }



int a10rel_sysinfo(syspointer)
tak_sysbufferaddress syspointer;
    {
    tak_cache_dir_pointer curr;
    struct tak_cache_dir_entry *ptoc_Var139;
    cccprint("AK10  a10rel_sys");
    if (syspointer != (struct tak_systembuffer *) (int *) 0)
        {
        curr.integer.C_4.sys_ptr = syspointer;
        curr.integer.C_2.buf_ptr = s35add_bufaddr(curr.integer.C_2.buf_ptr , -48);
        ptoc_Var139 = (struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 19534);
        xtd_directory(0 , curr);
        if (ptoc_Var139->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0 && (0 || (ptoc_Var139->integer.C_1.cd_state[0] & 2) == 0))
            {
            (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var139->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr , "vak10.p" , 19541)).integer.C_1.cd_fix_prev = ptoc_Var139->integer.C_1.cd_fix_prev;
            (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var139->integer.C_1.cd_fix_prev.integer.C_1.cdir_ptr , "vak10.p" , 19542)).integer.C_1.cd_fix_next = ptoc_Var139->integer.C_1.cd_fix_next;
            ptoc_Var139->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
            }
        }
    cccprint("<a10rel_sysinfo ");
    }

static
  unsigned char ptoc_Var140[1] = 
  {
  (unsigned char)' '
  };


int a10repl_sysinfo(acv, syspoint, b_err)
tak_all_command_glob *acv;
tak_sysbufferaddress *syspoint;
tgg_basis_error *b_err;
    {
    tak10_find_info find_info;
    struct tak_cache_dir_entry *ptoc_Var141;
    unsigned char ptoc_Var142[1];
    unsigned char ptoc_Var143[1];
    unsigned char ptoc_Var144[1];
    int ptoc_Var145;
    cccprint("AK10  a10repl_sy");
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 19565)).integer.C_4.sp1r_returncode == 0)
        {
        if (t01trace(0))
            {
            t01catalog_entrytype(0 , (unsigned char ) sql__setrang((*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 19571)).integer.C_50.syskey.integer.C_2.sentrytyp[1] , 0 , 255));
            t01buf1(0 , &(*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 19573)).integer.C_50.syskey , 1 , (int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 19573)).integer.C_50.syskey.skeylen + 8);
            }
        else
            {
            t01catalog_entrytype(5 , (unsigned char ) sql__setrang((*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 19577)).integer.C_50.syskey.integer.C_2.sentrytyp[1] , 0 , 255));
            t01buf1(5 , &(*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 19579)).integer.C_50.syskey , 1 , (int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 19579)).integer.C_50.syskey.skeylen + 8);
            }
        ak10find_syskey(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 19582)).integer.C_50.syskey , &find_info);
        if ((int ) find_info.fi_res == 1)
            {
            xtd_directory(0 , find_info.fi_ptr);
            if ((_ptoc_MEMCMP(unsigned char , 2, (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 19587)).integer.C_50.syskey.integer.C_2.sentrytyp , "\0\200") < 0 ? 1 : 0) || (_ptoc_MEMCMP(unsigned char , 2, (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 19588)).integer.C_50.syskey.integer.C_2.sentrytyp , "\0\260") > 0 ? 1 : 0) && !(char ) sql__setrang((*acv).a_in_ddl_trigger , 0 , 1) && (0 || ((*acv).a_pars_curr.integer.C_1.fileHandling_gg00[0] & 32) == 0) && !a663parse_for_execute(&(*acv)))
                {
                ak10add_upd_perm_record(&(*acv) , &find_info , *syspoint , 77 , &(*b_err));
                }
            else
                {
                ptoc_Var141 = (struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 19596);
                if (1 && (ptoc_Var141->integer.C_1.cd_state[0] & 16) != 0)
                    {
                    _ptoc_MOVE(unsigned char , 1, ptoc_Var141->integer.C_1.cd_state, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var142 , ptoc_Var141->integer.C_1.cd_state , ptoc_Var140 , sizeof(unsigned char [1])));
                    }
                *b_err = 0;
                if ((ptoc_Var145 = (int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 19603)).integer.C_50.syskey.integer.C_2.sentrytyp[1]) == 130 || ptoc_Var145 == 129 || ptoc_Var145 == 132 || ptoc_Var145 == 178 || ptoc_Var145 == 190)
                    {
                    (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 19607)).integer.C_23.sparsinfo.integer.C_1.p_fullen = (int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 19608)).integer.C_0.b_sl;
                    }
                if ((0 || (ptoc_Var141->integer.C_1.cd_state[0] & 8) == 0) && (0 || (ptoc_Var141->integer.C_1.cd_state[0] & 16) == 0))
                    {
                    *b_err = 1200;
                    }
                }
            }
        else
            {
            *b_err = 1200;
            }
        if ((int ) *b_err == 1200)
            {
            *b_err = 1930;
            ak10store_syskey(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 19626)).integer.C_50.syskey);
            }
        }
    else
        {
        *b_err = 400;
        }
    ak10verify_cache(&(*acv));
    cccprint("<a10repl_sysinfo");
    }



int a10SessionCacheInit(acv)
tak_all_command_glob *acv;
    {
    tak_cache_dir_pointer next;
    tak_cache_dir_pointer curr;
    tak_sysdirectory *ptoc_Var146;
    cccprint("AK10  a10Session");
    ptoc_Var146 = &(*acv).a_sysdir;
    if (ptoc_Var146->ci_cache_p != (tsp00_Buf *) (int *) 0)
        {
        next = (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var146->ci_cache_dir_trailor.integer.C_1.cdir_ptr , "vak10.p" , 19655)).integer.C_1.cd_lru_prev;
        while (next.integer.C_1.cdir_ptr != ptoc_Var146->ci_cache_dir_header.integer.C_1.cdir_ptr)
            {
            curr = next;
            next = (*(struct tak_cache_dir_entry *) sql__nil(next.integer.C_1.cdir_ptr , "vak10.p" , 19659)).integer.C_1.cd_lru_prev;
            ak10block_into_freelist(&(*acv) , curr , 1 , 1);
            }
        ptoc_Var146->ci_tmp_dislodged = 0;
        ak10td_dirlist(&(*acv) , 0);
        ak10td_freelist(&(*acv) , 0);
        }
    cccprint("<a10SessionCache");
    }

static
  unsigned char ptoc_Var147[1] = 
  {
  (unsigned char)'8'
  };


int a10_upd_key(acv, parsk, displacement, b_err)
tak_all_command_glob *acv;
tak_parskey *parsk;
int displacement;
tgg_basis_error *b_err;
    {
    char dummy;
    int cnt;
    int max_cnt;
    tak_sysbufferaddress sysp;
    tak_cache_dir_pointer p;
    tak_parskey old_parsk;
    tak10_find_info find_info;
    tgg_sysinfokey sysk;
    struct tak_cache_dir_entry *ptoc_Var148;
    unsigned char ptoc_Var149[1];
    unsigned char ptoc_Var150[1];
    unsigned char ptoc_Var151[1];
    int ptoc_Var152;
    cccprint("AK10  a10_upd_ke");
    cnt = 0;
    max_cnt = 1;
    sysk = a01sysnullkey;
    sysk.integer.C_3.sauthid[0] = (unsigned char)'\377';
    _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0\201")
    old_parsk = *parsk;
    (*parsk).p_no = (unsigned char ) sql__setrang((unsigned char ) ((int ) (*parsk).p_no - displacement) , 0 , 255);
    do
        {
        cnt = sql__succ(cnt , -2147483647 , 2147483647 , "vak10.p" , 19700);
        s10mv1(sizeof(tak_parskey ) , sizeof(tsp00_C8 ) , &old_parsk , 1 , sysk.integer.C_3.sauthid , 2 , 6);
        a10get_sysinfo(&(*acv) , &sysk , 1 , &sysp , &(*b_err));
        if ((int ) *b_err == 0)
            {
            ak10find_syskey(&(*acv) , &sysk , &find_info);
            ak10del_in_dir(&(*acv) , &sysk);
            if (1 && ((*(struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 19709)).integer.C_1.cd_state[0] & 16) != 0)
                {
                a10del_sysinfo(&(*acv) , &sysk , &(*b_err));
                }
            if ((int ) *b_err == 0)
                {
                s10mv1(sizeof(tak_parskey ) , sizeof(tsp00_C8 ) , &(*parsk) , 1 , sysk.integer.C_3.sauthid , 2 , 6);
                p = find_info.fi_ptr;
                ak10find_syskey(&(*acv) , &sysk , &find_info);
                if ((int ) find_info.fi_res == 1)
                    {
                    *b_err = 540;
                    ak10store_syskey(&(*acv) , &sysk);
                    }
                else
                    {
                    _ptoc_ASSIGN(unsigned char , 12, (*(struct tak_systembuffer *) sql__nil(sysp , "vak10.p" , 19728)).integer.C_50.syskey.integer.C_1.sstandardkey, sysk.integer.C_1.sstandardkey)
                    ak10ins_directory(&(*acv) , &find_info , &(*acv).a_sysdir.ci_dir[sql__setrang(sql__ord((int ) sysk.integer.C_2.sentrytyp[1]) , 1 , 195) + -1] , &p , &dummy);
                    ptoc_Var148 = (struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 19732);
                    _ptoc_MOVE(unsigned char , 1, ptoc_Var148->integer.C_1.cd_state, *(unsigned char (*)[1]) sql__setsub(ptoc_Var149 , ptoc_Var148->integer.C_1.cd_state , ptoc_Var147 , sizeof(unsigned char [1])));
                    a10add_sysinfo(&(*acv) , &sysp , &(*b_err));
                    }
                }
            }
        sysk.integer.C_3.sauthid[7] = (unsigned char ) sql__setrang((unsigned char ) (sql__ord((int ) sysk.integer.C_3.sauthid[7]) + 1) , 0 , 255);
        if (cnt == max_cnt)
            {
            if ((ptoc_Var152 = (int ) sysk.integer.C_2.sentrytyp[1]) == 129 || ptoc_Var152 == 193)
                {
                *b_err = 0;
                max_cnt = max_cnt + 1;
                _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.slinkage, "\0\0")
                if (_ptoc_MEMCMP(unsigned char , 2, sysk.integer.C_2.sentrytyp , "\0\201") == 0 ? 1 : 0)
                    {
                    _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0\301")
                    }
                else
                    {
                    _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0\230")
                    }
                sysk.integer.C_3.sauthid[7] = (unsigned char ) sql__setrang(a01sysnullkey.integer.C_3.sauthid[7] , 0 , 255);
                }
            }
        }
    while (!((int ) *b_err != 0 || cnt == max_cnt));
    if ((int ) *b_err == 1930 && ((ptoc_Var152 = (int ) sysk.integer.C_2.sentrytyp[1]) == 152 || ptoc_Var152 == 193))
        {
        *b_err = 0;
        }
    cccprint("<a10_upd_key    ");
    }



int a10_version(acv, base_rec, m_type, view_scan)
tak_all_command_glob *acv;
tak_baserecord *base_rec;
tgg_message_type m_type;
char view_scan;
    {
    cccprint("AK10  a10_versio");
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 19784)).integer.C_4.sp1r_returncode == 0)
        {
        a10alter_long_version(&(*acv) , &(*base_rec) , (int ) (*base_rec).bstringcount , (unsigned char ) sql__setrang(m_type , 0 , 95) , (char ) sql__setrang(view_scan , 0 , 1));
        }
    cccprint("<a10_version    ");
    }



int ak10add_base_record(acv, syspoint, via_kb, m_type, b_err)
tak_all_command_glob *acv;
tak_sysbufferaddress syspoint;
char via_kb;
tgg_message_type m_type;
tgg_basis_error *b_err;
    {
    tsp00_C2 version;
    int ix;
    int move_len;
    tak_sysbufferaddress p;
    int buf_cnt;
    int rec_cnt;
    tgg_rec_buf rec;
    tak00_colinfo_ptr colptr;
    struct ptoc_Type204
      {
      union  
        {
        struct  
          {
          tak_basecolinfo_ptr colp;
          } C_true;
        struct  
          {
          tak_sysbufferaddress sysp;
          } C_false;
        } boolean;
      } last;
    tgg_sysinfokey sysk;
    int ptoc_Var153;
    cccprint("AK10  ak10add_ba");
    *b_err = 0;
    p = (struct tak_systembuffer *) &rec;
    if ((int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 19824)).integer.C_1.sbase.btablekind == 8)
        {
        move_len = 60;
        g10mv9("VAK10 " , 8 , sizeof(tak_baserecord ) , sizeof(tsp00_Buf ) , sql__nil(syspoint , "vak10.p" , 19830) , 1 , rec.integer.C_11.buf , 1 , move_len , &(*b_err));
        rec.integer.C_12.len = (short ) sql__setrang((short ) move_len , -32768 , 32767);
        move_len = a061identifier_len((*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 19833)).integer.C_34.ssynonym.syn_tablen);
        rec.integer.C_11.buf[sql__setrang((int ) rec.integer.C_12.len + 1 , 1 , 8192) + -1] = (unsigned char ) sql__setrang((unsigned char ) move_len , 0 , 255);
        g10mv6("VAK10 " , 9 , sizeof(unsigned char [64]) , sizeof(tsp00_Buf ) , (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 19837)).integer.C_34.ssynonym.syn_tablen , 1 , rec.integer.C_11.buf , (int ) rec.integer.C_12.len + 2 , move_len , &(*b_err));
        rec.integer.C_12.len = (short ) sql__setrang((short ) ((int ) rec.integer.C_12.len + move_len + 1) , -32768 , 32767);
        if ((int ) *b_err == 0)
            {
            ak10send_ins_upd(&(*acv) , p , (unsigned char ) sql__setrang(m_type , 0 , 95) , &(*b_err));
            }
        }
    else
        {
        if ((ptoc_Var153 = (int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 19847)).integer.C_1.sbase.btablekind) == 7 || ptoc_Var153 == 10)
            {
            vbegexcl((*acv).a_transinf.tri_trans.trTaskId_gg00 , g08diagcache);
            ak10joinViewVersion.boolean.C_true.map_int = (short ) sql__setrang((short ) ((int ) ak10joinViewVersion.boolean.C_true.map_int + 1) , -32768 , 32767);
            vendexcl((*acv).a_transinf.tri_trans.trTaskId_gg00 , g08diagcache);
            _ptoc_ASSIGN(unsigned char , 2, version, ak10joinViewVersion.boolean.C_false.map_c2)
            }
        else
            {
            _ptoc_ASSIGN(unsigned char , 2, version, (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 19856)).integer.C_1.sbase.btreeid.integer.C_1.fileVersion_gg00.ci2_gg00)
            }
        buf_cnt = (int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 19858)).integer.C_1.sbase.breccnt;
        t01int4(0 , "buf_cnt     " , buf_cnt);
        (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 19860)).integer.C_1.sbase.breccnt = 0;
        move_len = a10BaseRecPersistentOffset();
        g10mv9("VAK10 " , 10 , sizeof(tak_baserecord ) , sizeof(tsp00_Buf ) , sql__nil(syspoint , "vak10.p" , 19864) , 1 , rec.integer.C_11.buf , 1 , move_len , &(*b_err));
        rec.integer.C_12.len = (short ) sql__setrang((short ) move_len , -32768 , 32767);
        move_len = a061identifier_len(sql__nil((*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 19867)).integer.C_1.sbase.btablen , "vak10.p" , 19867));
        rec.integer.C_11.buf[sql__setrang((int ) rec.integer.C_12.len + 1 , 1 , 8192) + -1] = (unsigned char ) sql__setrang((unsigned char ) move_len , 0 , 255);
        g10mv6("VAK10 " , 11 , sizeof(unsigned char [64]) , sizeof(tsp00_Buf ) , sql__nil((*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 19871)).integer.C_1.sbase.btablen , "vak10.p" , 19871) , 1 , rec.integer.C_11.buf , (int ) rec.integer.C_12.len + 2 , move_len , &(*b_err));
        rec.integer.C_12.len = (short ) sql__setrang((short ) ((int ) rec.integer.C_12.len + move_len + 1) , -32768 , 32767);
        rec_cnt = 0;
        t01int4(5 , "bfirstindex " , (int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 19875)).integer.C_1.sbase.bfirstindex);
        ix = (int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 19876)).integer.C_1.sbase.bfirstindex;
        while (ix <= (int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 19877)).integer.C_1.sbase.blastindex && (int ) *b_err == 0)
            {
            colptr = (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 19879)).integer.C_1.sbase.integer.C_2.bcolumn[sql__setrang(ix , 1 , 1536) + -1];
            move_len = sizeof(tak00_columninfo ) - sizeof(unsigned char [64]) + sql__ord((int ) (*(tak00_columninfo *) sql__nil(colptr , "vak10.p" , 19881)).integer.C_1.ccolumnn_len);
            if ((int ) rec.integer.C_12.len + move_len > 8096)
                {
                if ((char ) sql__setrang(via_kb , 0 , 1))
                    {
                    if (rec_cnt + 1 > buf_cnt)
                        {
                        m_type = 35;
                        }
                    else
                        {
                        m_type = 77;
                        }
                    _ptoc_ASSIGN(unsigned char , 2, (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 19894)).integer.C_1.sbase.bsegmentid, version)
                    ak10send_ins_upd(&(*acv) , p , (unsigned char ) sql__setrang(m_type , 0 , 95) , &(*b_err));
                    }
                else
                    {
                    if ((int ) m_type == 35)
                        {
                        b07cadd_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , sql__nil(p , "vak10.p" , 19903));
                        }
                    else
                        {
                        b07crepl_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , sql__nil(p , "vak10.p" , 19906));
                        }
                    *b_err = (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                    }
                a06inc_linkage((*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 19911)).integer.C_50.syskey.integer.C_2.slinkage);
                if ((int ) *b_err == 0)
                    {
                    rec_cnt = rec_cnt + 1;
                    (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 19916)).integer.C_1.sbase.breccnt = (short ) sql__setrang((short ) ((int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 19917)).integer.C_1.sbase.breccnt + 1) , -32768 , 32767);
                    rec.integer.C_12.len = (short ) sql__setrang((short ) (sizeof(tsp00_C2 ) + 20) , -32768 , 32767);
                    }
                }
            g10mv4("VAK10 " , 12 , move_len , sizeof(tsp00_Buf ) , sql__nil(colptr , "vak10.p" , 19926) , 1 , rec.integer.C_11.buf , (int ) rec.integer.C_12.len + 1 , move_len , &(*b_err));
            rec.integer.C_12.len = (short ) sql__setrang((short ) ((int ) rec.integer.C_12.len + move_len) , -32768 , 32767);
            ix = ix + 1;
            }
        if ((int ) *b_err == 0)
            {
            if ((char ) sql__setrang(via_kb , 0 , 1))
                {
                if (rec_cnt + 1 > buf_cnt)
                    {
                    m_type = 35;
                    }
                else
                    {
                    m_type = 77;
                    }
                _ptoc_ASSIGN(unsigned char , 2, (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 19943)).integer.C_1.sbase.bsegmentid, version)
                ak10send_ins_upd(&(*acv) , p , (unsigned char ) sql__setrang(m_type , 0 , 95) , &(*b_err));
                }
            else
                {
                if ((int ) m_type == 35)
                    {
                    b07cadd_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , sql__nil(p , "vak10.p" , 19952));
                    }
                else
                    {
                    b07crepl_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , sql__nil(p , "vak10.p" , 19955));
                    }
                *b_err = (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                }
            (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 19960)).integer.C_1.sbase.breccnt = (short ) sql__setrang((short ) ((int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 19960)).integer.C_1.sbase.breccnt + 1) , -32768 , 32767);
            if ((int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 19961)).integer.C_1.sbase.breccnt < buf_cnt && (int ) *b_err == 0)
                {
                sysk = (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 19965)).integer.C_50.syskey;
                buf_cnt = buf_cnt - (int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 19966)).integer.C_1.sbase.breccnt;
                while (buf_cnt > 0 && (int ) *b_err == 0)
                    {
                    buf_cnt = buf_cnt + -1;
                    a06inc_linkage(sysk.integer.C_2.slinkage);
                    if ((char ) sql__setrang(via_kb , 0 , 1))
                        {
                        a10del_sysinfo(&(*acv) , &sysk , &(*b_err));
                        }
                    else
                        {
                        b07cdel_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , &sysk);
                        }
                    }
                }
            }
        last.boolean.C_true.colp = (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 19984)).integer.C_1.sbase.bptr;
        while ((*(struct tak_basecolinfo *) sql__nil(last.boolean.C_true.colp , "vak10.p" , 19986)).bci_next != (struct tak_basecolinfo *) (int *) 0)
            last.boolean.C_true.colp = (*(struct tak_basecolinfo *) sql__nil(last.boolean.C_true.colp , "vak10.p" , 19987)).bci_next;
        ak10free_rest(&(*acv) , last.boolean.C_false.sysp , sizeof(struct tak_basecolinfo ) - sizeof(unsigned char [4096]) + (*(struct tak_basecolinfo *) sql__nil(last.boolean.C_true.colp , "vak10.p" , 19991)).bci_index + -1);
        }
    cccprint("<ak10add_base_re");
    }



int ak10add_method_record(acv, syspoint, m_type, b_err)
tak_all_command_glob *acv;
tak_sysbufferaddress syspoint;
tgg_message_type m_type;
tgg_basis_error *b_err;
    {
    int parm_no;
    int move_len;
    tak_sysbufferaddress p;
    tgg_rec_buf rec;
    tak_param_info *ptoc_Var154;
    cccprint("AK10  ak10add_me");
    p = (struct tak_systembuffer *) &rec;
    *b_err = 0;
    move_len = sizeof(tak_methodrecord ) - sizeof(tak_param_array );
    g10mv9("VAK10 " , 13 , sizeof(tak_methodrecord ) , sizeof(tsp00_Buf ) , sql__nil(syspoint , "vak10.p" , 20017) , 1 , rec.integer.C_11.buf , 1 , move_len , &(*b_err));
    rec.integer.C_12.len = (short ) sql__setrang((short ) move_len , -32768 , 32767);
    parm_no = 1;
    while (parm_no <= (int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20021)).integer.C_66.smethod.me_param_cnt && (int ) *b_err == 0)
        {
        ptoc_Var154 = (tak_param_info *) sql__nil((*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20023)).integer.C_66.smethod.me_param_list[sql__setrang(parm_no , 1 , 1024) + -1] , "vak10.p" , 20023);
        move_len = sizeof(tak_param_info ) - sizeof(tsp00_C256 ) + sql__ord((int ) ptoc_Var154->param_name_len);
        if ((int ) rec.integer.C_12.len + move_len > 8096)
            {
            ak10send_ins_upd(&(*acv) , p , (unsigned char ) sql__setrang(m_type , 0 , 95) , &(*b_err));
            a06inc_linkage((*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 20032)).integer.C_50.syskey.integer.C_2.slinkage);
            rec.integer.C_12.len = (short ) sql__setrang((short ) (sizeof(tsp00_C2 ) + 20) , -32768 , 32767);
            }
        if ((int ) *b_err == 0)
            {
            g10mv14("VAK10 " , 14 , move_len , sizeof(tsp00_Buf ) , sql__nil((*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20042)).integer.C_66.smethod.me_param_list[sql__setrang(parm_no , 1 , 1024) + -1] , "vak10.p" , 20042) , 1 , rec.integer.C_11.buf , (int ) rec.integer.C_12.len + 1 , move_len , &(*b_err));
            rec.integer.C_12.len = (short ) sql__setrang((short ) ((int ) rec.integer.C_12.len + move_len) , -32768 , 32767);
            parm_no = parm_no + 1;
            }
        }
    if ((int ) *b_err == 0)
        {
        ak10send_ins_upd(&(*acv) , p , (unsigned char ) sql__setrang(m_type , 0 , 95) , &(*b_err));
        }
    cccprint("<ak10add_method_");
    }



int ak10store_colnames_record(acv, syspoint, m_type, b_err)
tak_all_command_glob *acv;
tak_sysbufferaddress syspoint;
tgg_message_type m_type;
tgg_basis_error *b_err;
    {
    cccprint("AK10  ak10store_");
    while (syspoint != (struct tak_systembuffer *) (int *) 0)
        {
        if ((int ) m_type == 35)
            {
            b07cadd_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , sql__nil(syspoint , "vak10.p" , 20072));
            }
        else
            {
            b07crepl_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , sql__nil(syspoint , "vak10.p" , 20075));
            }
        *b_err = (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
        if ((int ) *b_err == 0 && (int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20078)).integer.C_0.b_sl == 8096)
            {
            syspoint = (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20080)).integer.C_59.scolnames.boolean.C_false.cnnext;
            }
        else
            {
            syspoint = (struct tak_systembuffer *) (int *) 0;
            }
        }
    cccprint("<ak10store_colna");
    }

static
  unsigned char ptoc_Var155[1] = 
  {
  (unsigned char)'\020'
  };
static
  unsigned char ptoc_Var156[1] = 
  {
  (unsigned char)'\b'
  };
static
  unsigned char ptoc_Var157[1] = 
  {
  (unsigned char)' '
  };


int ak10add_upd_perm_record(acv, find_info, syspoint, m_type, b_err)
tak_all_command_glob *acv;
tak10_find_info *find_info;
tak_sysbufferaddress syspoint;
tgg_message_type m_type;
tgg_basis_error *b_err;
    {
    tsp00_PageNo store_root;
    struct tak_cache_dir_entry *ptoc_Var158;
    tak_baserecord *ptoc_Var159;
    struct tak_cache_dir_entry *ptoc_Var160;
    unsigned char ptoc_Var161[1];
    unsigned char ptoc_Var162[1];
    unsigned char ptoc_Var163[1];
    unsigned char ptoc_Var164[1];
    unsigned char ptoc_Var165[1];
    unsigned char ptoc_Var166[1];
    unsigned char ptoc_Var167[1];
    unsigned char ptoc_Var168[1];
    unsigned char ptoc_Var169[1];
    tak_baserecord *ptoc_Var170;
    cccprint("AK10  ak10add_up");
    store_root = 2147483647;
    if (_ptoc_MEMCMP(unsigned char , 2, (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20102)).integer.C_50.syskey.integer.C_2.sentrytyp , "\0\200") > 0 ? 1 : 0)
        {
        ptoc_Var158 = (struct tak_cache_dir_entry *) sql__nil((*find_info).fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 20105);
        if ((int ) m_type == 77 && (0 || (ptoc_Var158->integer.C_1.cd_state[0] & 16) == 0))
            {
            m_type = 35;
            }
        }
    else
        {
        if (_ptoc_MEMCMP(unsigned char , 2, (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20114)).integer.C_50.syskey.integer.C_2.sentrytyp , "\0\001") == 0 ? 1 : 0)
            {
            ptoc_Var159 = &(*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20116)).integer.C_1.sbase;
            if ((int ) ptoc_Var159->btablekind == 1 || (int ) ptoc_Var159->btablekind == 2 || (int ) ptoc_Var159->btablekind == 5)
                {
                store_root = ptoc_Var159->btreeid.integer.C_1.fileRoot_gg00;
                ptoc_Var159->btreeid.integer.C_1.fileRoot_gg00 = 2147483647;
                }
            }
        }
    switch ((int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20129)).integer.C_50.syskey.integer.C_2.sentrytyp[1])
        {
        case 24 :
            ak10store_mess_block(&(*acv) , &(*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20131)).integer.C_62.smessblock , (char ) sql__setrang((int ) m_type == 35 , 0 , 1) , &(*b_err));
            break;
        case 1 :
        case 177 :
            ak10add_base_record(&(*acv) , syspoint , 1 , (unsigned char ) sql__setrang(m_type , 0 , 95) , &(*b_err));
            break;
        case 105 :
            ak10add_method_record(&(*acv) , syspoint , (unsigned char ) sql__setrang(m_type , 0 , 95) , &(*b_err));
            break;
        default :
            ak10send_ins_upd(&(*acv) , syspoint , (unsigned char ) sql__setrang(m_type , 0 , 95) , &(*b_err));
            break;
        }
    if ((int ) *b_err == 0)
        {
        ptoc_Var160 = (struct tak_cache_dir_entry *) sql__nil((*find_info).fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 20144);
        _ptoc_MOVE(unsigned char , 1, ptoc_Var160->integer.C_1.cd_state, *(unsigned char (*)[1]) sql__setsub(ptoc_Var161 , sql__setsub(ptoc_Var163 , sql__psetadd(ptoc_Var165 , ptoc_Var160->integer.C_1.cd_state , ptoc_Var155 , sizeof(unsigned char [1])) , ptoc_Var156 , sizeof(unsigned char [1])) , ptoc_Var157 , sizeof(unsigned char [1])));
        if (_ptoc_MEMCMP(unsigned char , 2, (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20147)).integer.C_50.syskey.integer.C_2.sentrytyp , "\0\001") == 0 ? 1 : 0)
            {
            ptoc_Var170 = &(*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20149)).integer.C_1.sbase;
            if ((int ) ptoc_Var170->btablekind == 1 || (int ) ptoc_Var170->btablekind == 2 || (int ) ptoc_Var170->btablekind == 5)
                {
                ptoc_Var170->btreeid.integer.C_1.fileRoot_gg00 = store_root;
                }
            }
        }
    cccprint("<ak10add_upd_per");
    }



int ak10no_more_memory(acv, e)
tak_all_command_glob *acv;
tgg_basis_error *e;
    {
    cccprint("AK10  ak10no_mor");
    if ((char ) sql__setrang((*acv).a_parsing_again , 0 , 1))
        {
        *e = 1405;
        }
    else
        {
        *e = 1500;
        }
    cccprint("<ak10no_more_mem");
    }



int ak10send_ins_upd(acv, syspoint, m_type, b_err)
tak_all_command_glob *acv;
tak_sysbufferaddress syspoint;
tgg_message_type m_type;
tgg_basis_error *b_err;
    {
    int initLen;
    int keylen;
    int trunc;
    int move_len;
    tgg_mess_block mblock;
    tgg_rec_buf data;
    tak10qual qual;
    cccprint("AK10  ak10send_i");
    t01buf(0 , sql__nil(syspoint , "vak10.p" , 20199) , 1 , (int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20199)).integer.C_0.b_sl);
    initLen = (int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20200)).integer.C_0.b_sl;
    *b_err = 0;
    g01mblock_init(&(*acv).a_transinf.tri_trans , (unsigned char ) sql__setrang(m_type , 0 , 95) , 0 , &mblock);
    g01stack_init(&mblock , (tgg00_StackList *) (int *) 0 , 0 , (tgg00_QualBuf *) &qual , sizeof(tak10qual ));
    g01datapart_init(&mblock , (tgg00_DataPart *) &data , sizeof(tgg00_Rec ));
    mblock.integer.C_2.mb_qual_len = sizeof(unsigned char [112]);
    switch ((int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20206)).integer.C_50.syskey.integer.C_2.sentrytyp[1])
        {
        case 77 :
        case 88 :
        case 65 :
        case 78 :
        case 22 :
        case 81 :
        case 79 :
            switch ((int ) (unsigned char ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20210)).integer.C_50.syskey.integer.C_2.sentrytyp[1])
                {
                case 77 :
                case 88 :
                    trunc = sizeof(tak_uref_restkey );
                    break;
                default :
                    trunc = sizeof(unsigned char [64]);
                    break;
                }
            t01int4(5 , "trunc       " , trunc);
            if ((char ) sql__setrang(g01unicode , 0 , 1))
                {
                keylen = s30unilnr((tsp00_MoveObj *) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20221)).integer.C_50.syskey.integer.C_13.sidentifier , "\0 " , 1 , trunc) + 12;
                }
            else
                {
                keylen = s30lnr((*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20225)).integer.C_50.syskey.integer.C_13.sidentifier , (unsigned char)' ' , 1 , trunc) + 12;
                }
            t01int4(5 , "keylen      " , keylen);
            g10mv3("VAK10 " , 15 , sizeof(struct tak_systembuffer ) , mblock.integer.C_2.mb_data_size , sql__nil(syspoint , "vak10.p" , 20230) , 1 , (*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak10.p" , 20230)).integer.C_1.mbp_buf , 1 , keylen + 8 , &(*b_err));
            (*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak10.p" , 20232)).integer.C_4.mbp_keylen = (short ) sql__setrang((short ) keylen , -32768 , 32767);
            move_len = (int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20233)).integer.C_0.b_sl - (int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20234)).integer.C_50.syskey.skeylen + -8;
            g10mv3("VAK10 " , 16 , sizeof(struct tak_systembuffer ) , mblock.integer.C_2.mb_data_size , sql__nil(syspoint , "vak10.p" , 20237) , (int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20237)).integer.C_50.syskey.skeylen + 9 , (*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak10.p" , 20238)).integer.C_1.mbp_buf , keylen + 9 , move_len , &(*b_err));
            (*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak10.p" , 20240)).integer.C_4.mbp_reclen = (short ) sql__setrang((short ) (keylen + move_len + 8) , -32768 , 32767);
            break;
        default :
            switch ((int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20245)).integer.C_50.syskey.integer.C_2.sentrytyp[1])
                {
                case 97 :
                    (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20247)).integer.C_0.b_sl = (short ) sql__setrang((short ) (a061identifier_len((*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20248)).integer.C_56.scol_uses_dom.cud_name) + 32) , -32768 , 32767);
                    break;
                case 66 :
                    (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20250)).integer.C_0.b_sl = (short ) sql__setrang((short ) (a061identifier_len((*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20251)).integer.C_42.suser.username) + 152) , -32768 , 32767);
                    break;
                case 82 :
                    if ((int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20253)).integer.C_0.b_sl >= 148)
                        {
                        (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20255)).integer.C_0.b_sl = (short ) sql__setrang((short ) (a061identifier_len((*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20256)).integer.C_30.ssequence.seq_name) + 148) , -32768 , 32767);
                        }
                    break;
                case 70 :
                    (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20259)).integer.C_0.b_sl = (short ) sql__setrang((short ) (sizeof(tak00_columninfo ) - sizeof(unsigned char [64]) + a061identifier_len((*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20262)).integer.C_9.sdomain.dom_colinfo.integer.C_1.ccolumnn) + 56) , -32768 , 32767);
                    break;
                default :
                    break;
                }
            g10mv3("VAK10 " , 17 , sizeof(struct tak_systembuffer ) , mblock.integer.C_2.mb_data_size , sql__nil(syspoint , "vak10.p" , 20268) , 1 , (*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak10.p" , 20268)).integer.C_1.mbp_buf , 1 , (int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20268)).integer.C_0.b_sl , &(*b_err));
            break;
        }
    mblock.integer.C_2.mb_data_len = (int ) (*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak10.p" , 20272)).integer.C_4.mbp_reclen;
    if ((int ) m_type == 35)
        {
        (*(tgg00_QualBuf *) sql__nil(mblock.integer.C_4.mb_qual , "vak10.p" , 20275)).integer.C_3.mstack_desc.integer.C_2.mfixedcol_len = -2;
        }
    if ((int ) *b_err == 0)
        {
        ak10send_mess_block(&(*acv) , &mblock , (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20279)).integer.C_50.syskey.integer.C_2.sentrytyp , 0 , &(*b_err));
        }
    (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20282)).integer.C_0.b_sl = (short ) sql__setrang((short ) initLen , -32768 , 32767);
    cccprint("<ak10send_ins_up");
    }



int ak10all_temp_add(acv, p)
tak_all_command_glob *acv;
tak_cache_dir_pointer p;
    {
    cccprint("AK10  ak10all_te");
    if (p.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        ak10temp_add(&(*acv) , p , 0);
        ak10all_temp_add(&(*acv) , (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 20299)).integer.C_1.cd_less);
        ak10all_temp_add(&(*acv) , (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 20300)).integer.C_1.cd_greater);
        }
    cccprint("<ak10all_temp_ad");
    }



int ak10alloc_mem(acv, do_init_cache, required_bytes, p, supplement)
tak_all_command_glob *acv;
char do_init_cache;
tsp_int4 required_bytes;
tsp_bufaddr *p;
tsp_int4 *supplement;
    {
    char ok;
    cccprint("AK10  ak10alloc_");
    *p = (tsp00_Buf *) (int *) 0;
    if ((char ) sql__setrang(do_init_cache , 0 , 1))
        {
        *supplement = ak10init_cache_size;
        }
    else
        {
        *supplement = 4;
        required_bytes = required_bytes + sizeof(tak10cache_header );
        if (*supplement * sizeof(tsp00_Buf ) < required_bytes)
            {
            *supplement = (required_bytes + -1) / sizeof(tsp00_Buf ) + 1;
            }
        }
    vbegexcl((*acv).a_transinf.tri_trans.trTaskId_gg00 , g08diagcache);
    if ((char ) sql__setrang(do_init_cache , 0 , 1))
        {
        ak10cache_cnt = ak10cache_cnt + 1;
        }
    if (*supplement + ak10all_cache_size + (g01maxuser() - ak10cache_cnt) * ak10init_cache_size <= g01cache_size(0))
        {
        vmalloc(*supplement * sizeof(tsp00_Buf ) + sizeof(tgg00_SysInfoKey ) , &(*p) , &ok);
        if ((char ) sql__setrang(ok , 0 , 1))
            {
            ak10all_cache_size = ak10all_cache_size + *supplement;
            }
        else
            {
            if ((char ) sql__setrang(do_init_cache , 0 , 1))
                {
                ak10cache_cnt = ak10cache_cnt + -1;
                }
            }
        }
    vendexcl((*acv).a_transinf.tri_trans.trTaskId_gg00 , g08diagcache);
    cccprint("<ak10alloc_mem  ");
    }

typedef
  struct ptoc_Type74 tak_baserecord_a10BaseRecPersistentOffset;


int a10BaseRecPersistentOffset()
    {
    int _a10BaseRecPersistentOffset;
    tak_baserecord_a10BaseRecPersistentOffset *pBaseRec;
    struct ptoc_Type205
      {
      union  
        {
        struct  
          {
          tsp00_Addr align;
          } C_true;
        struct  
          {
          tsp00_C256 buf;
          } C_false;
        } boolean;
      } auxBuf;
    cccprint("AK10  a10BaseRec");
    pBaseRec = (tak_baserecord_a10BaseRecPersistentOffset *) &auxBuf;
    _a10BaseRecPersistentOffset = sizeof(short ) + sizeof(short ) + sizeof(short ) + sizeof(short ) + sizeof(tsp00_C8 ) + sizeof(tsp00_C2 ) + sizeof(tsp00_C2 ) + sizeof(tsp00_C2 ) + sizeof(short ) + sizeof(short ) + sizeof(unsigned char ) + sizeof(unsigned char [1]) + sizeof(tgg00_FileId ) + sizeof(tsp00_C8 ) + sizeof(short ) + sizeof(short ) + sizeof(short ) + sizeof(short ) + sizeof(short ) + sizeof(short ) + sizeof(short ) + sizeof(short ) + sizeof(unsigned char ) + sizeof(char ) + sizeof(short ) + sizeof(short ) + sizeof(short ) + sizeof(int ) + sizeof(int ) + sizeof(int ) + sizeof(int ) + sizeof(int ) + sizeof(int ) + sizeof(int ) + sizeof(int ) + sizeof(int ) + sizeof(unsigned char [1]) + sizeof(tak_priv_desc ) + sizeof(char ) + sizeof(char ) + sizeof(unsigned char ) + sizeof(unsigned char ) + sizeof(unsigned char ) + sizeof(char ) + sizeof(char ) + sizeof(unsigned char ) + sizeof(char ) + sizeof(char ) + sizeof(char ) + sizeof(unsigned char ) + sizeof(short ) + sizeof(unsigned char ) + sizeof(char ) + sizeof(unsigned char ) + sizeof(tak_table_attribute_set );
    cccprint("<a10BaseRecPersi");
    return(_a10BaseRecPersistentOffset);
    }



int ak10base_record(acv, rec, syspoint)
tak_all_command_glob *acv;
tgg_rec_buf *rec;
tak_sysbufferaddress syspoint;
    {
    tgg_basis_error b_err;
    tsp00_C2 version;
    int colinfo_head_len;
    int cnext_delta;
    int pos;
    int colno;
    int move_len;
    int colindex;
    int needed_ident_cnt;
    int needed_len;
    tak_sysbufferaddress rec_ptr;
    tak_basecolinfo_ptr colptr;
    struct ptoc_Type206
      {
      union  
        {
        struct  
          {
          tak_sysbufferaddress mobj;
          } C_true;
        struct  
          {
          tak_basecolinfo_ptr cip;
          } C_false;
        } boolean;
      } p;
    tgg_sysinfokey sysk;
    tak_baserecord *ptoc_Var171;
    cccprint("AK10  ak10base_r");
    b_err = 0;
    pos = a10BaseRecPersistentOffset() + 1;
    needed_ident_cnt = 1;
    colinfo_head_len = sizeof(struct tak_basecolinfo ) + -4096;
    needed_len = colinfo_head_len + needed_ident_cnt * ((sizeof(unsigned char [64]) + -1) / sizeof(tsp00_C4 ) + 1) * sizeof(tsp00_C4 ) + (int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20485)).integer.C_1.sbase.bmaxcol * ((sizeof(tak00_columninfo ) - sizeof(unsigned char [64]) + a01identifier_size + -1) / sizeof(tsp00_C4 ) + 1) * sizeof(tsp00_C4 );
    if (needed_len > sizeof(struct tak_basecolinfo ))
        {
        needed_len = sizeof(struct tak_basecolinfo );
        }
    a10new(&(*acv) , needed_len , &p.boolean.C_true.mobj);
    if (p.boolean.C_true.mobj != (struct tak_systembuffer *) (int *) 0)
        {
        ptoc_Var171 = &(*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 20495)).integer.C_1.sbase;
        _ptoc_ASSIGN(unsigned char , 2, version, ptoc_Var171->bsegmentid)
        (*(struct tak_basecolinfo *) sql__nil(p.boolean.C_false.cip , "vak10.p" , 20498)).bci_buf_size = (short ) sql__setrang((short ) (needed_len - colinfo_head_len) , -32768 , 32767);
        t01int4(0 , "blastindex  " , (int ) ptoc_Var171->blastindex);
        t01int4(0 , "bfirstindex " , (int ) ptoc_Var171->bfirstindex);
        t01int4(0 , "pos         " , pos);
        t01addr(0 , "bptr        " , (tsp00_Buf *) (int *) sql__nil(p.boolean.C_false.cip , "vak10.p" , 20502));
        cnext_delta = (int ) ptoc_Var171->blastindex - (int ) ptoc_Var171->bmaxcol - (int ) ptoc_Var171->bfirstindex + 1;
        ptoc_Var171->bfirstcolind = (short ) sql__setrang((short ) ((int ) ptoc_Var171->bfirstcolind - cnext_delta) , -32768 , 32767);
        if ((int ) ptoc_Var171->blastkeyind > 0)
            {
            ptoc_Var171->blastkeyind = (short ) sql__setrang((short ) ((int ) ptoc_Var171->blastkeyind - cnext_delta) , -32768 , 32767);
            }
        ptoc_Var171->bptr = p.boolean.C_false.cip;
        (*(struct tak_basecolinfo *) sql__nil(ptoc_Var171->bptr , "vak10.p" , 20511)).bci_index = 1;
        ptoc_Var171->breccnt = 1;
        (*(struct tak_basecolinfo *) sql__nil(ptoc_Var171->bptr , "vak10.p" , 20513)).bci_next = (struct tak_basecolinfo *) (int *) 0;
        (*(struct tak_basecolinfo *) sql__nil(ptoc_Var171->bptr , "vak10.p" , 20514)).bci_ref_cnt = 1;
        ptoc_Var171->btablen = (tsp00_KnlIdentifier *) &(*(struct tak_basecolinfo *) sql__nil(ptoc_Var171->bptr , "vak10.p" , 20515)).boolean.C_false.bci_buf[sql__setrang((*(struct tak_basecolinfo *) sql__nil(ptoc_Var171->bptr , "vak10.p" , 20515)).bci_index , 1 , 4096) + -1];
        (*(struct tak_basecolinfo *) sql__nil(ptoc_Var171->bptr , "vak10.p" , 20516)).bci_index = (*(struct tak_basecolinfo *) sql__nil(ptoc_Var171->bptr , "vak10.p" , 20516)).bci_index + ((sizeof(unsigned char [64]) + -1) / sizeof(tsp00_C4 ) + 1) * sizeof(tsp00_C4 );
        _ptoc_ASSIGN(unsigned char , 64, *(unsigned char (*)[64]) sql__nil(ptoc_Var171->btablen , "vak10.p" , 20519), a01_il_b_identifier)
        g10mv7("VAK10 " , 18 , sizeof(tsp00_Buf ) , sizeof(unsigned char [64]) , (*rec).integer.C_11.buf , pos + 1 , sql__nil(ptoc_Var171->btablen , "vak10.p" , 20522) , 1 , sql__ord((int ) (*rec).integer.C_11.buf[sql__setrang(pos , 1 , 8192) + -1]) , &b_err);
        pos = pos + sql__ord((int ) (*rec).integer.C_11.buf[sql__setrang(pos , 1 , 8192) + -1]) + 1;
        colptr = ptoc_Var171->bptr;
        rec_ptr = (struct tak_systembuffer *) (int *) 0;
        colindex = (int ) ptoc_Var171->bfirstindex;
        colno = 0;
        while (colno < (int ) ptoc_Var171->bmaxcol && (int ) b_err == 0)
            {
            t01int4(0 , "pos         " , pos);
            t01int4(0 , "rec.len     " , (int ) (*rec).integer.C_12.len);
            if (pos > (int ) (*rec).integer.C_12.len)
                {
                if (rec_ptr == (struct tak_systembuffer *) (int *) 0)
                    {
                    rec_ptr = (struct tak_systembuffer *) &*rec;
                    sysk = (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 20540)).integer.C_50.syskey;
                    }
                a06inc_linkage(sysk.integer.C_2.slinkage);
                if ((int ) b_err == 0)
                    {
                    if (_ptoc_MEMCMP(unsigned char , 2, sysk.integer.C_2.sentrytyp , "\0\200") < 0 ? 1 : 0)
                        {
                        ak10get_perm_record(&(*acv) , &sysk , &(*rec) , &b_err);
                        if ((int ) b_err == 0)
                            {
                            if (_ptoc_MEMCMP(unsigned char , 2, version , (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 20554)).integer.C_1.sbase.bsegmentid) == 0 ? 0 : 1)
                                {
                                b_err = 1500;
                                }
                            }
                        }
                    else
                        {
                        b07cget_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , &sysk , &(*rec));
                        b_err = (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                        (*acv).a_transinf.tri_trans.trError_gg00 = 0;
                        }
                    }
                if ((int ) b_err == 0)
                    {
                    ptoc_Var171->breccnt = (short ) sql__setrang((short ) ((int ) ptoc_Var171->breccnt + 1) , -32768 , 32767);
                    pos = sizeof(tsp00_C2 ) + 21;
                    }
                }
            move_len = sizeof(tak00_columninfo ) - sizeof(unsigned char [64]) + sql__ord((int ) (*rec).integer.C_11.buf[sql__setrang(pos + sizeof(tak00_columninfo ) - sizeof(unsigned char [64]) + -1 , 1 , 8192) + -1]);
            if (needed_len - colinfo_head_len - (*(struct tak_basecolinfo *) sql__nil(colptr , "vak10.p" , 20586)).bci_index + 1 < move_len && (int ) b_err == 0)
                {
                needed_len = colinfo_head_len + ((int ) ptoc_Var171->bmaxcol - colno) * ((sizeof(tak00_columninfo ) - sizeof(unsigned char [64]) + a01identifier_size + -1) / sizeof(tsp00_C4 ) + 1) * sizeof(tsp00_C4 );
                if (needed_len > sizeof(struct tak_basecolinfo ))
                    {
                    needed_len = sizeof(struct tak_basecolinfo );
                    }
                a10new(&(*acv) , needed_len , &p.boolean.C_true.mobj);
                if (p.boolean.C_true.mobj != (struct tak_systembuffer *) (int *) 0)
                    {
                    (*(struct tak_basecolinfo *) sql__nil(p.boolean.C_false.cip , "vak10.p" , 20602)).bci_buf_size = (short ) sql__setrang((short ) (needed_len - colinfo_head_len) , -32768 , 32767);
                    (*(struct tak_basecolinfo *) sql__nil(colptr , "vak10.p" , 20603)).bci_next = p.boolean.C_false.cip;
                    colptr = p.boolean.C_false.cip;
                    (*(struct tak_basecolinfo *) sql__nil(colptr , "vak10.p" , 20605)).bci_index = 1;
                    (*(struct tak_basecolinfo *) sql__nil(colptr , "vak10.p" , 20606)).bci_next = (struct tak_basecolinfo *) (int *) 0;
                    }
                else
                    {
                    ak10no_more_memory(&(*acv) , &b_err);
                    }
                }
            if ((int ) b_err == 0)
                {
                t01int4(5 , "move_len    " , move_len);
                t01int4(5 , "bci_index   " , (*(struct tak_basecolinfo *) sql__nil(colptr , "vak10.p" , 20617)).bci_index);
                t01int4(5 , "colindex    " , colindex);
                ptoc_Var171->integer.C_2.bcolumn[sql__setrang(colindex , 1 , 1536) + -1] = (tak00_columninfo *) &(*(struct tak_basecolinfo *) sql__nil(colptr , "vak10.p" , 20619)).boolean.C_false.bci_buf[sql__setrang((*(struct tak_basecolinfo *) sql__nil(colptr , "vak10.p" , 20619)).bci_index , 1 , 4096) + -1];
                (*(struct tak_basecolinfo *) sql__nil(colptr , "vak10.p" , 20620)).bci_index = (*(struct tak_basecolinfo *) sql__nil(colptr , "vak10.p" , 20620)).bci_index + ((move_len + -1) / sizeof(tsp00_C4 ) + 1) * sizeof(tsp00_C4 );
                g10mv8("VAK10 " , 19 , sizeof(tsp00_Buf ) , sizeof(unsigned char [4096]) , (*rec).integer.C_11.buf , pos , sql__nil(ptoc_Var171->integer.C_2.bcolumn[sql__setrang(colindex , 1 , 1536) + -1] , "vak10.p" , 20625) , 1 , move_len , &b_err);
                ptoc_Var171->integer.C_1.bextcolindex[sql__setrang((int ) (*(tak00_columninfo *) sql__nil(ptoc_Var171->integer.C_2.bcolumn[sql__setrang(colindex , 1 , 1536) + -1] , "vak10.p" , 20626)).integer.C_1.cextcolno , 1 , 1024) + -1] = (short ) sql__setrang((short ) colno , -32768 , 32767);
                if ((int ) (*(tak00_columninfo *) sql__nil(ptoc_Var171->integer.C_2.bcolumn[sql__setrang(colindex , 1 , 1536) + -1] , "vak10.p" , 20627)).integer.C_1.cnextind != 0)
                    {
                    (*(tak00_columninfo *) sql__nil(ptoc_Var171->integer.C_2.bcolumn[sql__setrang(colindex , 1 , 1536) + -1] , "vak10.p" , 20629)).integer.C_1.cnextind = (short ) sql__setrang((short ) ((int ) (*(tak00_columninfo *) sql__nil(ptoc_Var171->integer.C_2.bcolumn[sql__setrang(colindex , 1 , 1536) + -1] , "vak10.p" , 20630)).integer.C_1.cnextind - cnext_delta) , -32768 , 32767);
                    }
                ptoc_Var171->blastindex = (short ) sql__setrang((short ) colindex , -32768 , 32767);
                colindex = colindex + 1;
                pos = pos + move_len;
                colno = colno + 1;
                }
            }
        if ((int ) b_err != 0)
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
            }
        }
    else
        {
        ak10no_more_memory(&(*acv) , &b_err);
        a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
        }
    cccprint("<ak10base_record");
    }

static
  tak_sysdir_state ptoc_Var172 = 
  {
  (unsigned char)'\200'
  };


int ak10block_into_freelist(acv, block_addr, del_in_directory, del_dependecies)
tak_all_command_glob *acv;
tak_cache_dir_pointer block_addr;
char del_in_directory;
char del_dependecies;
    {
    tak_sysbufferaddress p;
    tak_cache_dir_pointer curr;
    tak_cache_dir_pointer prev;
    tak_sysdirectory *ptoc_Var173;
    struct tak_cache_dir_entry *ptoc_Var174;
    cccprint("AK10  ak10block_");
    ptoc_Var173 = &(*acv).a_sysdir;
    ptoc_Var174 = (struct tak_cache_dir_entry *) sql__nil(block_addr.integer.C_1.cdir_ptr , "vak10.p" , 20669);
    if (ptoc_Var174->integer.C_1.cd_pos + ptoc_Var174->integer.C_1.cd_check != 0)
        {
        xtd_directory(0 , block_addr);
        ak10cache_error(&(*acv) , "ak10block_into_freelist " , block_addr);
        }
    else
        {
        if ((char ) sql__setrang(del_in_directory , 0 , 1))
            {
            if (1 && (ptoc_Var174->integer.C_1.cd_state[0] & 64) != 0)
                {
                (*acv).a_sysdir.ci_displaceCnt = (*acv).a_sysdir.ci_displaceCnt + -1;
                t01int4(0 , "-displaceCnt" , (*acv).a_sysdir.ci_displaceCnt);
                t01buf1(0 , &ptoc_Var174->integer.C_1.cd_syskey , 9 , (int ) ptoc_Var174->integer.C_1.cd_syskey.skeylen + 8);
                if ((*acv).a_sysdir.ci_displaceCnt < 0)
                    {
                    g01abort(51114 , "AK CACHE" , "ci_displaceCnt < 0      " , 0);
                    }
                }
            if ((char ) sql__setrang(del_dependecies , 0 , 1))
                {
                switch ((int ) ptoc_Var174->integer.C_1.cd_syskey.integer.C_2.sentrytyp[1])
                    {
                    case 192 :
                        ak10free_colnamesrecord(&(*acv) , &ptoc_Var174->integer.C_2.cd_sysinfo.integer.C_59.scolnames);
                        break;
                    case 1 :
                    case 177 :
                        ak10free_baserecord(&(*acv) , &ptoc_Var174->integer.C_2.cd_sysinfo.integer.C_1.sbase);
                        break;
                    case 193 :
                    case 24 :
                    case 149 :
                        ak10free_mblock(&(*acv) , &ptoc_Var174->integer.C_2.cd_sysinfo.integer.C_62.smessblock.mbr_mess_block);
                        break;
                    case 181 :
                        if (_ptoc_MEMCMP(unsigned char , 2, ptoc_Var174->integer.C_1.cd_syskey.integer.C_2.sentrytyp , "\0\265") == 0 ? 1 : 0)
                            {
                            p = (struct tak_systembuffer *) &ptoc_Var174->integer.C_2.cd_sysinfo;
                            if ((*acv).a_resname_addr[0] == p)
                                {
                                (*acv).a_resname_addr[0] = (struct tak_systembuffer *) (int *) 0;
                                }
                            else
                                {
                                if ((*acv).a_resname_addr[1] == p)
                                    {
                                    (*acv).a_resname_addr[1] = (struct tak_systembuffer *) (int *) 0;
                                    }
                                }
                            }
                        break;
                    case 105 :
                        ak10free_method_record(&(*acv) , &ptoc_Var174->integer.C_2.cd_sysinfo.integer.C_66.smethod);
                        break;
                    default :
                        break;
                    }
                }
            ak10del_in_dir(&(*acv) , &ptoc_Var174->integer.C_1.cd_syskey);
            }
        _ptoc_MOVE(unsigned char , 1, ptoc_Var174->integer.C_1.cd_state, ptoc_Var172);
        ptoc_Var174->integer.C_2.cd_mem_type = 10;
        if (ptoc_Var174->integer.C_1.cd_lru_next.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
            {
            (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var174->integer.C_1.cd_lru_prev.integer.C_1.cdir_ptr , "vak10.p" , 20736)).integer.C_1.cd_lru_next = ptoc_Var174->integer.C_1.cd_lru_next;
            (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var174->integer.C_1.cd_lru_next.integer.C_1.cdir_ptr , "vak10.p" , 20738)).integer.C_1.cd_lru_prev = ptoc_Var174->integer.C_1.cd_lru_prev;
            ptoc_Var174->integer.C_1.cd_lru_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
            ptoc_Var174->integer.C_1.cd_lru_prev.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
            }
        if (ptoc_Var174->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
            {
            (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var174->integer.C_1.cd_fix_prev.integer.C_1.cdir_ptr , "vak10.p" , 20748)).integer.C_1.cd_fix_next = ptoc_Var174->integer.C_1.cd_fix_next;
            (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var174->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr , "vak10.p" , 20749)).integer.C_1.cd_fix_prev = ptoc_Var174->integer.C_1.cd_fix_prev;
            ptoc_Var174->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
            ptoc_Var174->integer.C_1.cd_fix_prev.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
            }
        ptoc_Var174->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
        ptoc_Var174->integer.C_1.cd_fix_prev.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
        ptoc_Var174->integer.C_1.cd_less.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
        ptoc_Var174->integer.C_1.cd_greater.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
        curr = (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var173->ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 20758)).integer.C_2.cd_next_free;
        prev = ptoc_Var173->ci_cache_dir_header;
        t01int4(0 , "block_pos   " , ptoc_Var174->integer.C_1.cd_pos);
        t01int4(0 , "block_len   " , ptoc_Var174->integer.C_1.cd_syslen);
        t01int4(0 , "buf_no      " , (int ) ptoc_Var174->integer.C_1.cd_buf_no);
        while ((int ) (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 20763)).integer.C_1.cd_buf_no < (int ) ptoc_Var174->integer.C_1.cd_buf_no || (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 20765)).integer.C_1.cd_pos < ptoc_Var174->integer.C_1.cd_pos && (int ) (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 20766)).integer.C_1.cd_buf_no == (int ) ptoc_Var174->integer.C_1.cd_buf_no)
            {
            t01p2int4(0 , "curr bl pos " , (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 20769)).integer.C_1.cd_pos , "curr bl buf " , (int ) (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 20770)).integer.C_1.cd_buf_no);
            prev = curr;
            curr = (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 20772)).integer.C_2.cd_next_free;
            if ((int ) (*(struct tak_cache_dir_entry *) sql__nil(prev.integer.C_1.cdir_ptr , "vak10.p" , 20773)).integer.C_1.cd_buf_no == (int ) (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 20773)).integer.C_1.cd_buf_no && (*(struct tak_cache_dir_entry *) sql__nil(prev.integer.C_1.cdir_ptr , "vak10.p" , 20774)).integer.C_1.cd_pos == (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 20774)).integer.C_1.cd_pos)
                {
                a10dump(&(*acv));
                g01abort(51113 , "AK CACHE" , "INCORRECT POINTER STRUCT" , 0);
                }
            }
        if ((*(struct tak_cache_dir_entry *) sql__nil(prev.integer.C_1.cdir_ptr , "vak10.p" , 20784)).integer.C_1.cd_pos + (*(struct tak_cache_dir_entry *) sql__nil(prev.integer.C_1.cdir_ptr , "vak10.p" , 20784)).integer.C_1.cd_syslen == ptoc_Var174->integer.C_1.cd_pos && (int ) (*(struct tak_cache_dir_entry *) sql__nil(prev.integer.C_1.cdir_ptr , "vak10.p" , 20787)).integer.C_1.cd_buf_no == (int ) ptoc_Var174->integer.C_1.cd_buf_no)
            {
            (*(struct tak_cache_dir_entry *) sql__nil(prev.integer.C_1.cdir_ptr , "vak10.p" , 20790)).integer.C_1.cd_syslen = (*(struct tak_cache_dir_entry *) sql__nil(prev.integer.C_1.cdir_ptr , "vak10.p" , 20791)).integer.C_1.cd_syslen + ptoc_Var174->integer.C_1.cd_syslen;
            if ((*(struct tak_cache_dir_entry *) sql__nil(prev.integer.C_1.cdir_ptr , "vak10.p" , 20792)).integer.C_1.cd_pos + (*(struct tak_cache_dir_entry *) sql__nil(prev.integer.C_1.cdir_ptr , "vak10.p" , 20792)).integer.C_1.cd_syslen == (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 20793)).integer.C_1.cd_pos && (int ) (*(struct tak_cache_dir_entry *) sql__nil(prev.integer.C_1.cdir_ptr , "vak10.p" , 20794)).integer.C_1.cd_buf_no == (int ) (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 20795)).integer.C_1.cd_buf_no)
                {
                (*(struct tak_cache_dir_entry *) sql__nil(prev.integer.C_1.cdir_ptr , "vak10.p" , 20799)).integer.C_1.cd_syslen = (*(struct tak_cache_dir_entry *) sql__nil(prev.integer.C_1.cdir_ptr , "vak10.p" , 20800)).integer.C_1.cd_syslen + (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 20801)).integer.C_1.cd_syslen;
                (*(struct tak_cache_dir_entry *) sql__nil(prev.integer.C_1.cdir_ptr , "vak10.p" , 20802)).integer.C_2.cd_next_free = (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 20803)).integer.C_2.cd_next_free;
                }
            }
        else
            {
            (*(struct tak_cache_dir_entry *) sql__nil(prev.integer.C_1.cdir_ptr , "vak10.p" , 20809)).integer.C_2.cd_next_free = block_addr;
            if (ptoc_Var174->integer.C_1.cd_pos + ptoc_Var174->integer.C_1.cd_syslen == (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 20810)).integer.C_1.cd_pos && (int ) ptoc_Var174->integer.C_1.cd_buf_no == (int ) (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 20811)).integer.C_1.cd_buf_no)
                {
                ptoc_Var174->integer.C_2.cd_next_free = (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 20814)).integer.C_2.cd_next_free;
                ptoc_Var174->integer.C_1.cd_syslen = (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 20816)).integer.C_1.cd_syslen + ptoc_Var174->integer.C_1.cd_syslen;
                }
            else
                {
                ptoc_Var174->integer.C_2.cd_next_free = curr;
                }
            }
        }
    cccprint("<ak10block_into_");
    }



int ak10CacheCheck(acv)
tak_all_command_glob *acv;
    {
    cccprint("AK10  ak10CacheC");
    if (!a102IsCacheOk(&(*acv)))
        {
        a10dump(&(*acv));
        g01abort(51113 , "AK CACHE" , "VERIFY FAILED           " , 0);
        }
    cccprint("<ak10CacheCheck ");
    }



int ak10cache_error(acv, ptoc_ptr_desc, ptr)
tak_all_command_glob *acv;
tsp_c24 ptoc_ptr_desc;
tak_cache_dir_pointer ptr;
    {
    tsp_c24 desc;
    _ptoc_MOVE(char, sizeof(tsp_c24 ), desc, ptoc_ptr_desc);
    cccprint("AK10  ak10cache_");
    if ((char ) sql__setrang(a10chk , 0 , 1))
        {
        if (ptr.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
            {
            ak10writeln(ptr);
            }
        g01opmsg(2 , 1 , 51105 , "AK CACHE" , "DUMP                    " , 0);
        ak10verify_cache(&(*acv));
        g01abort(51114 , "AK CACHE" , desc , 0);
        }
    a10dump(&(*acv));
    ak10td_dirlist(&(*acv) , 39);
    g01abort(51106 , "AK CACHE" , "DIRECTORY FULL          " , 0);
    cccprint("<ak10cache_error");
    }



int ak10check_hit_rate(acv)
tak_all_command_glob *acv;
    {
    tsp_bufaddr dummy;
    tak_all_command_glob *ptoc_Var175;
    tak_sysdirectory *ptoc_Var176;
    cccprint("AK10  ak10check_");
    ptoc_Var175 = &*acv;
    ptoc_Var176 = &ptoc_Var175->a_sysdir;
    if (ptoc_Var176->ci_tmp_read_cnt > 100)
        {
        if (ptoc_Var176->ci_tmp_read_cnt < 2147483647)
            {
            ptoc_Var176->ci_tmp_read_cnt = ptoc_Var176->ci_tmp_read_cnt + 1;
            }
        else
            {
            ptoc_Var176->ci_tmp_read_found = ak10hit_rate(ptoc_Var176->ci_tmp_read_found , ptoc_Var176->ci_tmp_read_cnt);
            ptoc_Var176->ci_tmp_read_cnt = 1001;
            }
        if (ak10hit_rate(ptoc_Var176->ci_tmp_read_found , ptoc_Var176->ci_tmp_read_cnt) < 900)
            {
            ak10get_supplement(&(*acv) , &dummy , 0);
            ptoc_Var176->ci_tmp_read_found = 0;
            ptoc_Var176->ci_tmp_read_cnt = 0;
            ptoc_Var176->ci_tmp_dislodged = 0;
            }
        }
    cccprint("<ak10check_hit_r");
    }



int ak10del(acv, p, deleted_p, decremented)
tak_all_command_glob *acv;
tak_cache_dir_pointer *p;
tak_cache_dir_pointer *deleted_p;
char *decremented;
    {
    cccprint("AK10  ak10del   ");
    if ((*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 20932)).integer.C_1.cd_greater.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        ak10del(&(*acv) , &(*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 20935)).integer.C_1.cd_greater , &(*deleted_p) , &(*decremented));
        if ((char ) sql__setrang(*decremented , 0 , 1))
            {
            ak10del_balance_right(&(*acv) , &(*p) , &(*decremented));
            }
        }
    else
        {
        *deleted_p = *p;
        *p = (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 20944)).integer.C_1.cd_less;
        *decremented = 1;
        }
    cccprint("<ak10del        ");
    }



int ak10del_balance_left(acv, p, decremented)
tak_all_command_glob *acv;
tak_cache_dir_pointer *p;
char *decremented;
    {
    tak_balance_info b1;
    tak_balance_info b2;
    tak_cache_dir_pointer p1;
    tak_cache_dir_pointer p2;
    cccprint("AK10  ak10del_ba");
    xtd_directory(0 , *p);
    if ((*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 20968)).integer.C_1.cd_pos + (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 20968)).integer.C_1.cd_check != 0)
        {
        ak10cache_error(&(*acv) , "ak10del_bal_left        " , *p);
        }
    else
        {
        switch ((int ) (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 20972)).integer.C_1.cd_balance)
            {
            case 0 :
                (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 20974)).integer.C_1.cd_balance = 1;
                break;
            case 1 :
                (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 20977)).integer.C_1.cd_balance = 2;
                *decremented = 0;
                break;
            case 2 :
                p1 = (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 20982)).integer.C_1.cd_greater;
                b1 = (unsigned char ) sql__setrang((*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 20983)).integer.C_1.cd_balance , 0 , 2);
                if ((int ) b1 >= 1)
                    {
                    (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 20987)).integer.C_1.cd_greater = (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 20987)).integer.C_1.cd_less;
                    (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 20988)).integer.C_1.cd_less = *p;
                    if ((int ) b1 == 1)
                        {
                        (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 20992)).integer.C_1.cd_balance = 2;
                        (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 20993)).integer.C_1.cd_balance = 0;
                        *decremented = 0;
                        }
                    else
                        {
                        (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 20998)).integer.C_1.cd_balance = 1;
                        (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 20999)).integer.C_1.cd_balance = 1;
                        }
                    *p = p1;
                    }
                else
                    {
                    p2 = (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 21006)).integer.C_1.cd_less;
                    b2 = (unsigned char ) sql__setrang((*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 21007)).integer.C_1.cd_balance , 0 , 2);
                    (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 21008)).integer.C_1.cd_less = (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 21008)).integer.C_1.cd_greater;
                    (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 21009)).integer.C_1.cd_greater = p1;
                    (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21010)).integer.C_1.cd_greater = (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 21010)).integer.C_1.cd_less;
                    (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 21011)).integer.C_1.cd_less = *p;
                    if ((int ) b2 == 2)
                        {
                        (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21014)).integer.C_1.cd_balance = 0;
                        }
                    else
                        {
                        (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21016)).integer.C_1.cd_balance = 1;
                        }
                    if ((int ) b2 == 0)
                        {
                        (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 21020)).integer.C_1.cd_balance = 2;
                        }
                    else
                        {
                        (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 21022)).integer.C_1.cd_balance = 1;
                        }
                    *p = p2;
                    (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 21025)).integer.C_1.cd_balance = 1;
                    }
                break;
            default :
                ak10cache_error(&(*acv) , "ak10del_bal_left1       " , *p);
                break;
            }
        }
    cccprint("<ak10del_balance");
    }



int ak10del_balance_right(acv, p, decremented)
tak_all_command_glob *acv;
tak_cache_dir_pointer *p;
char *decremented;
    {
    tak_balance_info b1;
    tak_balance_info b2;
    tak_cache_dir_pointer p1;
    tak_cache_dir_pointer p2;
    cccprint("AK10  ak10del_ba");
    xtd_directory(0 , *p);
    if ((*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21054)).integer.C_1.cd_pos + (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21054)).integer.C_1.cd_check != 0)
        {
        ak10cache_error(&(*acv) , "ak10del_bal_right       " , *p);
        }
    else
        {
        switch ((int ) (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21058)).integer.C_1.cd_balance)
            {
            case 0 :
                p1 = (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21061)).integer.C_1.cd_less;
                xtd_directory(0 , p1);
                b1 = (unsigned char ) sql__setrang((*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 21063)).integer.C_1.cd_balance , 0 , 2);
                if ((int ) b1 <= 1)
                    {
                    (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21067)).integer.C_1.cd_less = (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 21067)).integer.C_1.cd_greater;
                    (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 21068)).integer.C_1.cd_greater = *p;
                    if ((int ) b1 == 1)
                        {
                        (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21072)).integer.C_1.cd_balance = 0;
                        (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 21073)).integer.C_1.cd_balance = 2;
                        *decremented = 0;
                        }
                    else
                        {
                        (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21078)).integer.C_1.cd_balance = 1;
                        (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 21079)).integer.C_1.cd_balance = 1;
                        }
                    *p = p1;
                    }
                else
                    {
                    p2 = (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 21086)).integer.C_1.cd_greater;
                    b2 = (unsigned char ) sql__setrang((*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 21087)).integer.C_1.cd_balance , 0 , 2);
                    xtd_directory(0 , p2);
                    (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 21089)).integer.C_1.cd_greater = (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 21089)).integer.C_1.cd_less;
                    (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 21090)).integer.C_1.cd_less = p1;
                    (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21091)).integer.C_1.cd_less = (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 21091)).integer.C_1.cd_greater;
                    (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 21092)).integer.C_1.cd_greater = *p;
                    if ((int ) b2 == 0)
                        {
                        (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21095)).integer.C_1.cd_balance = 2;
                        }
                    else
                        {
                        (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21097)).integer.C_1.cd_balance = 1;
                        }
                    if ((int ) b2 == 2)
                        {
                        (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 21101)).integer.C_1.cd_balance = 0;
                        }
                    else
                        {
                        (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 21103)).integer.C_1.cd_balance = 1;
                        }
                    *p = p2;
                    (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 21106)).integer.C_1.cd_balance = 1;
                    }
                break;
            case 1 :
                (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21112)).integer.C_1.cd_balance = 0;
                *decremented = 0;
                break;
            case 2 :
                (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21116)).integer.C_1.cd_balance = 1;
                break;
            default :
                ak10cache_error(&(*acv) , "ak10del_bal_right1      " , *p);
                break;
            }
        }
    cccprint("<ak10del_balance");
    }

static
  unsigned char ptoc_Var177[1] = 
  {
  (unsigned char)'@'
  };


int ak10del_in_dir(acv, syskey)
tak_all_command_glob *acv;
tgg_sysinfokey *syskey;
    {
    int etype;
    char decremented;
    tgg_sysinfokey currkey;
    tak10_find_info find_info;
    unsigned char ptoc_Var178[1];
    unsigned char ptoc_Var179[1];
    unsigned char ptoc_Var180[1];
    struct tak_cache_dir_entry *ptoc_Var181;
    cccprint("AK10  ak10del_in");
    decremented = 0;
    if (_ptoc_MEMCMP(unsigned char , 2, (*syskey).integer.C_2.sentrytyp , "\0\001") == 0 ? 1 : 0)
        {
        currkey = *syskey;
        etype = 1;
        do
            {
            _ptoc_ASSIGN(unsigned char , 2, currkey.integer.C_2.sentrytyp, ak10displaceTypes[sql__setrang(etype , 1 , 4) + -1])
            ak10find_syskey(&(*acv) , &currkey , &find_info);
            if ((int ) find_info.fi_res == 1)
                {
                if (0 || ((*(struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 21152)).integer.C_1.cd_state[0] & 64) == 0)
                    {
                    (_ptoc_MOVE(unsigned char , 1, (*(struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 21155)).integer.C_1.cd_state, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var178 , (*(struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 21156)).integer.C_1.cd_state , ptoc_Var177 , sizeof(unsigned char [1]))) , (*(struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 21155)).integer.C_1.cd_state);
                    (*acv).a_sysdir.ci_displaceCnt = (*acv).a_sysdir.ci_displaceCnt + 1;
                    t01int4(0 , "+displaceCnt" , (*acv).a_sysdir.ci_displaceCnt);
                    ptoc_Var181 = (struct tak_cache_dir_entry *) sql__nil(find_info.fi_ptr.integer.C_1.cdir_ptr , "vak10.p" , 21159);
                    t01buf1(0 , &ptoc_Var181->integer.C_1.cd_syskey , 9 , (int ) ptoc_Var181->integer.C_1.cd_syskey.skeylen + 8);
                    }
                }
            if (etype == 4)
                {
                break;
                }
            etype += 1;
            }
        while (!0);
        }
    ak10del_in_directory(&(*acv) , &(*syskey) , &(*acv).a_sysdir.ci_dir[sql__setrang(sql__ord((int ) (*syskey).integer.C_2.sentrytyp[1]) , 1 , 195) + -1] , &decremented);
    cccprint("<ak10del_in_dir ");
    }



int ak10del_in_directory(acv, syskey, p, decremented)
tak_all_command_glob *acv;
tgg_sysinfokey *syskey;
tak_cache_dir_pointer *p;
char *decremented;
    {
    tak_cache_dir_pointer aux;
    tak_cache_dir_pointer deleted_p;
    tsp_lcomp_result res;
    cccprint("AK10  ak10del_in");
    if ((*p).integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        s30cmp(&(*syskey) , 9 , (int ) (*syskey).skeylen , &(*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21194)).integer.C_1.cd_syskey , 9 , (int ) (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21195)).integer.C_1.cd_syskey.skeylen , &res);
        switch ((int ) res)
            {
            case 0 :
                ak10del_in_directory(&(*acv) , &(*syskey) , &(*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21200)).integer.C_1.cd_less , &(*decremented));
                if ((char ) sql__setrang(*decremented , 0 , 1))
                    {
                    ak10del_balance_left(&(*acv) , &(*p) , &(*decremented));
                    }
                break;
            case 2 :
                ak10del_in_directory(&(*acv) , &(*syskey) , &(*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21209)).integer.C_1.cd_greater , &(*decremented));
                if ((char ) sql__setrang(*decremented , 0 , 1))
                    {
                    ak10del_balance_right(&(*acv) , &(*p) , &(*decremented));
                    }
                break;
            default :
                aux = *p;
                if ((*(struct tak_cache_dir_entry *) sql__nil(aux.integer.C_1.cdir_ptr , "vak10.p" , 21218)).integer.C_1.cd_greater.integer.C_1.cdir_ptr == (struct tak_cache_dir_entry *) (int *) 0)
                    {
                    *p = (*(struct tak_cache_dir_entry *) sql__nil(aux.integer.C_1.cdir_ptr , "vak10.p" , 21221)).integer.C_1.cd_less;
                    *decremented = 1;
                    }
                else
                    {
                    if ((*(struct tak_cache_dir_entry *) sql__nil(aux.integer.C_1.cdir_ptr , "vak10.p" , 21225)).integer.C_1.cd_less.integer.C_1.cdir_ptr == (struct tak_cache_dir_entry *) (int *) 0)
                        {
                        *p = (*(struct tak_cache_dir_entry *) sql__nil(aux.integer.C_1.cdir_ptr , "vak10.p" , 21228)).integer.C_1.cd_greater;
                        *decremented = 1;
                        }
                    else
                        {
                        ak10del(&(*acv) , &(*(struct tak_cache_dir_entry *) sql__nil(aux.integer.C_1.cdir_ptr , "vak10.p" , 21233)).integer.C_1.cd_less , &deleted_p , &(*decremented));
                        (*(struct tak_cache_dir_entry *) sql__nil(deleted_p.integer.C_1.cdir_ptr , "vak10.p" , 21235)).integer.C_1.cd_less = (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21236)).integer.C_1.cd_less;
                        (*(struct tak_cache_dir_entry *) sql__nil(deleted_p.integer.C_1.cdir_ptr , "vak10.p" , 21237)).integer.C_1.cd_greater = (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21238)).integer.C_1.cd_greater;
                        (*(struct tak_cache_dir_entry *) sql__nil(deleted_p.integer.C_1.cdir_ptr , "vak10.p" , 21239)).integer.C_1.cd_balance = (unsigned char ) sql__setrang((*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 21240)).integer.C_1.cd_balance , 0 , 2);
                        *p = deleted_p;
                        if ((char ) sql__setrang(*decremented , 0 , 1))
                            {
                            ak10del_balance_left(&(*acv) , &(*p) , &(*decremented));
                            }
                        }
                    }
                break;
            }
        }
    cccprint("<ak10del_in_dire");
    }



int a10expand_rec(rec_buf, b_err)
tgg_rec_buf *rec_buf;
tgg_basis_error *b_err;
    {
    tak_sysbufferaddress p;
    int len;
    int diff;
    cccprint("AK10  a10expand_");
    p = (struct tak_systembuffer *) &*rec_buf;
    diff = 0;
    *b_err = 0;
    switch ((int ) (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21274)).integer.C_50.syskey.integer.C_2.sentrytyp[1])
        {
        case 82 :
            if ((int ) (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21276)).integer.C_0.b_sl >= 148 && (int ) (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21277)).integer.C_0.b_sl < sizeof(unsigned char [64]) + 148)
                {
                g10mv6("VAK10 " , 20 , sizeof(unsigned char [64]) , sizeof(tsp00_Buf ) , a01_il_b_identifier , 1 , (*rec_buf).integer.C_11.buf , (int ) (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21283)).integer.C_0.b_sl + 1 , sizeof(tak_sequencerecord ) - (int ) (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21283)).integer.C_0.b_sl , &(*b_err));
                (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21284)).integer.C_0.b_sl = (short ) sql__setrang((short ) sizeof(tak_sequencerecord ) , -32768 , 32767);
                }
            break;
        case 65 :
        case 78 :
        case 22 :
        case 81 :
        case 79 :
            diff = sizeof(unsigned char [64]) - (int ) (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21290)).integer.C_50.syskey.skeylen + 12;
            break;
        case 66 :
            if ((int ) (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21292)).integer.C_0.b_sl < sizeof(unsigned char [64]) + 152)
                {
                g10mv6("VAK10 " , 21 , sizeof(unsigned char [64]) , sizeof(tsp00_Buf ) , a01_il_b_identifier , 1 , (*rec_buf).integer.C_11.buf , (int ) (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21298)).integer.C_0.b_sl + 1 , sizeof(tak_userrecord ) - (int ) (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21298)).integer.C_0.b_sl , &(*b_err));
                (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21299)).integer.C_0.b_sl = (short ) sql__setrang((short ) sizeof(tak_userrecord ) , -32768 , 32767);
                }
            break;
        case 70 :
            if ((int ) (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21303)).integer.C_0.b_sl < sizeof(unsigned char [64]) + 56)
                {
                g10mv6("VAK10 " , 22 , sizeof(unsigned char [64]) , sizeof(tsp00_Buf ) , a01_il_b_identifier , 1 , (*rec_buf).integer.C_11.buf , (int ) (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21309)).integer.C_0.b_sl + 1 , sizeof(tak_domainrecord ) - (int ) (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21309)).integer.C_0.b_sl , &(*b_err));
                (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21310)).integer.C_0.b_sl = (short ) sql__setrang((short ) sizeof(tak_domainrecord ) , -32768 , 32767);
                }
            break;
        case 97 :
            if ((int ) (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21314)).integer.C_0.b_sl < sizeof(unsigned char [64]) + 32)
                {
                g10mv6("VAK10 " , 23 , sizeof(unsigned char [64]) , sizeof(tsp00_Buf ) , a01_il_b_identifier , 1 , (*rec_buf).integer.C_11.buf , (int ) (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21320)).integer.C_0.b_sl + 1 , sizeof(tak_col_uses_dom ) - (int ) (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21320)).integer.C_0.b_sl , &(*b_err));
                (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21321)).integer.C_0.b_sl = (short ) sql__setrang((short ) sizeof(tak_col_uses_dom ) , -32768 , 32767);
                }
            break;
        case 77 :
        case 88 :
            diff = sizeof(tak_uref_restkey ) - (int ) (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21326)).integer.C_50.syskey.skeylen + 12;
            break;
        default :
            break;
        }
    if (diff > 0)
        {
        g10mvr("VAK10 " , 24 , sizeof(tsp00_Buf ) , sizeof(tsp00_Buf ) , (*rec_buf).integer.C_11.buf , (int ) (*rec_buf).integer.C_12.keylen + 9 , (*rec_buf).integer.C_11.buf , (int ) (*rec_buf).integer.C_12.keylen + diff + 9 , (int ) (*rec_buf).integer.C_12.len - (int ) (*rec_buf).integer.C_12.keylen + -8 , &(*b_err));
        g10mv6("VAK10 " , 25 , sizeof(unsigned char [64]) , sizeof(tsp00_Buf ) , a01_il_b_identifier , 1 , (*rec_buf).integer.C_11.buf , (int ) (*rec_buf).integer.C_12.keylen + 9 , diff , &(*b_err));
        (*rec_buf).integer.C_12.keylen = (short ) sql__setrang((short ) ((int ) (*rec_buf).integer.C_12.keylen + diff) , -32768 , 32767);
        (*rec_buf).integer.C_12.len = (short ) sql__setrang((short ) ((int ) (*rec_buf).integer.C_12.len + diff) , -32768 , 32767);
        }
    t01buf(5 , sql__nil(p , "vak10.p" , 21347) , 1 , (int ) (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21347)).integer.C_0.b_sl);
    cccprint("<a10expand_rec  ");
    }



int ak10find_syskey(acv, syskey, find_info)
tak_all_command_glob *acv;
tgg_sysinfokey *syskey;
tak10_find_info *find_info;
    {
    tak_cache_dir_pointer curr;
    int entrytype;
    struct tak_cache_dir_entry *ptoc_Var182;
    cccprint("AK10  ak10find_s");
    (*find_info).fi_ptr.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
    (*find_info).fi_res = 0;
    entrytype = sql__ord((int ) (*syskey).integer.C_2.sentrytyp[1]);
    if (entrytype == 0 || entrytype > 195 || (int ) (*syskey).skeylen < 1 || (int ) (*syskey).skeylen > sizeof(tgg00_SysInfoKey ) + -8)
        {
        a07ak_system_error(&(*acv) , 10 , 2);
        }
    else
        {
        curr = (*acv).a_sysdir.ci_dir[sql__setrang(entrytype , 1 , 195) + -1];
        while (curr.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
            {
            ptoc_Var182 = (struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 21376);
            if (ptoc_Var182->integer.C_1.cd_pos + ptoc_Var182->integer.C_1.cd_check != 0)
                {
                ak10cache_error(&(*acv) , "ak10find_syskey         " , curr);
                curr.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
                }
            else
                {
                (*find_info).fi_ptr = curr;
                s30cmp(&(*syskey) , 9 , (int ) (*syskey).skeylen , &ptoc_Var182->integer.C_1.cd_syskey , 9 , (int ) ptoc_Var182->integer.C_1.cd_syskey.skeylen , &(*find_info).fi_res);
                if ((int ) (*find_info).fi_res == 1)
                    {
                    curr.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
                    }
                else
                    {
                    if ((int ) (*find_info).fi_res == 2)
                        {
                        curr = (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 21397)).integer.C_1.cd_greater;
                        }
                    else
                        {
                        curr = (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 21399)).integer.C_1.cd_less;
                        }
                    }
                }
            }
        }
    cccprint("<ak10find_syskey");
    }

static
  tak_sysdir_state ptoc_Var183 = 
  {
  (unsigned char)'\200'
  };


int ak10free_cache(acv, offset)
tak_all_command_glob *acv;
tsp_int4 offset;
    {
    int i;
    tak_cache_dir_pointer cache_dir_ptr;
    tak_cache_dir_pointer prev_dir_ptr;
    struct ptoc_Type207
      {
      union  
        {
        struct  
          {
          tsp_bufaddr bufaddr;
          } C_1;
        struct  
          {
          tak10cache_header_ptr chptr;
          } C_2;
        struct  
          {
          tak_cache_dir_pointer cdptr;
          } C_3;
        } integer;
      } univ_ptr;
    tak_all_command_glob *ptoc_Var184;
    tak_sysdirectory *ptoc_Var185;
    struct tak_cache_dir_entry *ptoc_Var186;
    struct tak_cache_dir_entry *ptoc_Var187;
    struct tak_cache_dir_entry *ptoc_Var188;
    struct tak_cache_dir_entry *ptoc_Var189;
    cccprint("AK10  ak10free_c");
    ptoc_Var184 = &*acv;
    ptoc_Var185 = &ptoc_Var184->a_sysdir;
    ptoc_Var186 = (struct tak_cache_dir_entry *) sql__nil(ptoc_Var185->ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 21441);
    ptoc_Var186->integer.C_1.cd_buf_no = 0;
    ptoc_Var186->integer.C_1.cd_pos = -1;
    ptoc_Var186->integer.C_1.cd_check = 1;
    ptoc_Var186->integer.C_1.cd_syslen = 0;
    ptoc_Var186->integer.C_1.cd_less.integer.C_2.buf_ptr = (tsp00_Buf *) (int *) 0;
    ptoc_Var186->integer.C_1.cd_lru_next = ptoc_Var185->ci_cache_dir_trailor;
    ptoc_Var186->integer.C_1.cd_lru_prev.integer.C_2.buf_ptr = (tsp00_Buf *) (int *) 0;
    ptoc_Var186->integer.C_1.cd_fix_next = ptoc_Var185->ci_cache_dir_trailor;
    ptoc_Var186->integer.C_1.cd_fix_prev.integer.C_2.buf_ptr = (tsp00_Buf *) (int *) 0;
    ptoc_Var187 = (struct tak_cache_dir_entry *) sql__nil(ptoc_Var185->ci_cache_dir_trailor.integer.C_1.cdir_ptr , "vak10.p" , 21454);
    ptoc_Var187->integer.C_1.cd_buf_no = 32767;
    ptoc_Var187->integer.C_1.cd_pos = 2147483647;
    ptoc_Var187->integer.C_1.cd_check = -2147483647;
    ptoc_Var187->integer.C_1.cd_syslen = 0;
    ptoc_Var187->integer.C_2.cd_next_free.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
    ptoc_Var187->integer.C_1.cd_lru_next.integer.C_2.buf_ptr = (tsp00_Buf *) (int *) 0;
    ptoc_Var187->integer.C_1.cd_lru_prev = ptoc_Var185->ci_cache_dir_header;
    ptoc_Var187->integer.C_1.cd_fix_next.integer.C_2.buf_ptr = (tsp00_Buf *) (int *) 0;
    ptoc_Var187->integer.C_1.cd_fix_prev = ptoc_Var185->ci_cache_dir_header;
    cache_dir_ptr.integer.C_2.buf_ptr = s35add_bufaddr(ptoc_Var185->ci_cache_p , sizeof(tak10cache_header ) + offset + 96);
    ptoc_Var188 = (struct tak_cache_dir_entry *) sql__nil(cache_dir_ptr.integer.C_1.cdir_ptr , "vak10.p" , 21470);
    ptoc_Var188->integer.C_1.cd_buf_no = 0;
    ptoc_Var188->integer.C_1.cd_less.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
    ptoc_Var188->integer.C_1.cd_greater.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
    ptoc_Var188->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
    ptoc_Var188->integer.C_1.cd_fix_prev.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
    ptoc_Var188->integer.C_1.cd_lru_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
    ptoc_Var188->integer.C_1.cd_lru_prev.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
    ptoc_Var188->integer.C_1.cd_pos = sizeof(tak10cache_header ) + offset + 97;
    _ptoc_MOVE(unsigned char , 1, ptoc_Var188->integer.C_1.cd_state, ptoc_Var183);
    ptoc_Var188->integer.C_2.cd_mem_type = 10;
    ptoc_Var188->integer.C_1.cd_check = -ptoc_Var188->integer.C_1.cd_pos;
    ptoc_Var188->integer.C_1.cd_syslen = ak10init_cache_size * sizeof(tsp00_Buf ) - sizeof(tak10cache_header ) - offset + -96;
    ptoc_Var188->integer.C_2.cd_next_free = ptoc_Var185->ci_cache_dir_trailor;
    (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var185->ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 21490)).integer.C_2.cd_next_free = cache_dir_ptr;
    prev_dir_ptr = cache_dir_ptr;
    univ_ptr.integer.C_1.bufaddr = ptoc_Var185->ci_cache_p;
    while ((*(tak10cache_header *) sql__nil(univ_ptr.integer.C_2.chptr , "vak10.p" , 21493)).ch_next_cache != (tsp00_Buf *) (int *) 0)
        {
        t01str30(0 , "succ cache found              ");
        cache_dir_ptr.integer.C_2.buf_ptr = s35add_bufaddr((*(tak10cache_header *) sql__nil(univ_ptr.integer.C_2.chptr , "vak10.p" , 21497)).ch_next_cache , sizeof(tak10cache_header ));
        (*(struct tak_cache_dir_entry *) sql__nil(prev_dir_ptr.integer.C_1.cdir_ptr , "vak10.p" , 21499)).integer.C_2.cd_next_free = cache_dir_ptr;
        ptoc_Var189 = (struct tak_cache_dir_entry *) sql__nil(cache_dir_ptr.integer.C_1.cdir_ptr , "vak10.p" , 21500);
        ptoc_Var189->integer.C_1.cd_pos = sizeof(tak10cache_header ) + 1;
        ptoc_Var189->integer.C_1.cd_check = -ptoc_Var189->integer.C_1.cd_pos;
        ptoc_Var189->integer.C_1.cd_syslen = sizeof(tsp00_Buf ) * 4 - sizeof(tak10cache_header );
        ptoc_Var189->integer.C_2.cd_next_free = ptoc_Var185->ci_cache_dir_trailor;
        prev_dir_ptr = cache_dir_ptr;
        univ_ptr.integer.C_1.bufaddr = (*(tak10cache_header *) sql__nil(univ_ptr.integer.C_2.chptr , "vak10.p" , 21510)).ch_next_cache;
        }
    i = 1;
    do
        {
        ptoc_Var185->ci_dir[sql__setrang(i , 1 , 195) + -1].integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
        if (i == 195)
            {
            break;
            }
        i += 1;
        }
    while (!0);
    ak10td_freelist(&(*acv) , 0);
    if (offset > 0)
        {
        ak10verify_cache(&(*acv));
        }
    cccprint("<ak10free_cache ");
    }



int ak10free_baserecord(acv, baserec)
tak_all_command_glob *acv;
tak_baserecord *baserec;
    {
    tak_basecolinfo_ptr p;
    tak_sysbufferaddress aux;
    cccprint("AK10  ak10free_b");
    if ((int ) (*baserec).btablekind != 8)
        {
        if ((*baserec).bptr != (struct tak_basecolinfo *) (int *) 0)
            {
            t01addr(0 , "bptr        " , (tsp00_Buf *) (int *) sql__nil((*baserec).bptr , "vak10.p" , 21545));
            (*(struct tak_basecolinfo *) sql__nil((*baserec).bptr , "vak10.p" , 21546)).bci_ref_cnt = (short ) sql__setrang((short ) ((int ) (*(struct tak_basecolinfo *) sql__nil((*baserec).bptr , "vak10.p" , 21546)).bci_ref_cnt + -1) , -32768 , 32767);
            if ((int ) (*(struct tak_basecolinfo *) sql__nil((*baserec).bptr , "vak10.p" , 21547)).bci_ref_cnt == 0)
                {
                p = (*baserec).bptr;
                (*baserec).bptr = (struct tak_basecolinfo *) (int *) 0;
                while (p != (struct tak_basecolinfo *) (int *) 0)
                    {
                    aux = (struct tak_systembuffer *) (int *) sql__nil(p , "vak10.p" , 21554);
                    p = (*(struct tak_basecolinfo *) sql__nil(p , "vak10.p" , 21555)).bci_next;
                    a10dispose(&(*acv) , &aux);
                    }
                }
            }
        }
    cccprint("<ak10free_basere");
    }



int ak10free_colnamesrecord(acv, colnamesrec)
tak_all_command_glob *acv;
tak_columnnamesrecord *colnamesrec;
    {
    tak_sysbufferaddress p;
    tak_sysbufferaddress next;
    cccprint("AK10  ak10free_c");
    if ((int ) (*colnamesrec).cnreclen == 8096)
        {
        p = (*colnamesrec).boolean.C_false.cnnext;
        while (p != (struct tak_systembuffer *) (int *) 0)
            {
            if ((int ) (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21586)).integer.C_0.b_sl == 8096)
                {
                next = (*(struct tak_systembuffer *) sql__nil(p , "vak10.p" , 21588)).integer.C_59.scolnames.boolean.C_false.cnnext;
                }
            else
                {
                next = (struct tak_systembuffer *) (int *) 0;
                }
            a10dispose(&(*acv) , &p);
            p = next;
            }
        }
    cccprint("<ak10free_colnam");
    }



int ak10free_method_record(acv, method)
tak_all_command_glob *acv;
tak_methodrecord *method;
    {
    int ix;
    struct ptoc_Type208
      {
      union  
        {
        struct  
          {
          tak_param_info_ptr parm_ptr;
          } C_true;
        struct  
          {
          tak_sysbufferaddress sys_ptr;
          } C_false;
        } boolean;
      } p;
    int ptoc_Var190;
    cccprint("AK10  ak10free_m");
    ptoc_Var190 = (*method).me_param_cnt;
    if (1 <= ptoc_Var190)
        {
        ix = 1;
        do
            {
            p.boolean.C_true.parm_ptr = (*method).me_param_list[sql__setrang(ix , 1 , 1024) + -1];
            if (p.boolean.C_false.sys_ptr != (struct tak_systembuffer *) (int *) 0)
                {
                a10dispose(&(*acv) , &p.boolean.C_false.sys_ptr);
                }
            if (ix == ptoc_Var190)
                {
                break;
                }
            ix += 1;
            }
        while (!0);
        }
    cccprint("<ak10free_method");
    }



int ak10free_mblock(acv, mblock)
tak_all_command_glob *acv;
tgg_mess_block *mblock;
    {
    int ix;
    struct ptoc_Type209
      {
      union  
        {
        struct  
          {
          tsp_bufaddr buf_ptr;
          } C_1;
        struct  
          {
          tgg_qual_buf_ptr qual_ptr;
          } C_2;
        struct  
          {
          tak_cache_dir_pointer cdir_ptr;
          } C_3;
        struct  
          {
          tgg_datapart_ptr data_ptr;
          } C_4;
        struct  
          {
          tgg_stack_list_ptr st_ptr;
          } C_5;
        struct  
          {
          tsp00_MoveObjPtr strat_ptr;
          } C_6;
        } integer;
      } p[4];
    cccprint("AK10  ak10free_m");
    p[0].integer.C_2.qual_ptr = (*mblock).integer.C_4.mb_qual;
    p[1].integer.C_4.data_ptr = (*mblock).integer.C_3.mb_data;
    p[2].integer.C_5.st_ptr = (*mblock).integer.C_5.mb_st;
    p[3].integer.C_6.strat_ptr = (*mblock).integer.C_11.mb_strat;
    ix = 1;
    do
        {
        if (p[sql__setrang(ix , 1 , 4) + -1].integer.C_1.buf_ptr != (tsp00_Buf *) (int *) 0)
            {
            t01addr(0 , "addr        " , p[sql__setrang(ix , 1 , 4) + -1].integer.C_1.buf_ptr);
            p[sql__setrang(ix , 1 , 4) + -1].integer.C_1.buf_ptr = s35add_bufaddr(p[sql__setrang(ix , 1 , 4) + -1].integer.C_1.buf_ptr , -48);
            t01addr(0 , "addr        " , p[sql__setrang(ix , 1 , 4) + -1].integer.C_1.buf_ptr);
            ak10block_into_freelist(&(*acv) , p[sql__setrang(ix , 1 , 4) + -1].integer.C_3.cdir_ptr , 0 , 1);
            }
        if (ix == 4)
            {
            break;
            }
        ix += 1;
        }
    while (!0);
    cccprint("<ak10free_mblock");
    }



int ak10free_rest(acv, syspointer, used_len)
tak_all_command_glob *acv;
tak_sysbufferaddress syspointer;
tsp_int4 used_len;
    {
    tak_cache_dir_pointer curr;
    tak_cache_dir_pointer free;
    tsp_int4 block_pos;
    tsp_int4 rest_len;
    int ptoc_Var191;
    cccprint("AK10  ak10free_r");
    curr.integer.C_4.sys_ptr = syspointer;
    curr.integer.C_2.buf_ptr = s35add_bufaddr(curr.integer.C_2.buf_ptr , -48);
    if (((ptoc_Var191 = used_len % 4) >= 0 ? ptoc_Var191 : ptoc_Var191 + 4) != 0)
        {
        used_len = used_len - ((ptoc_Var191 = used_len % 4) >= 0 ? ptoc_Var191 : ptoc_Var191 + 4) + 4;
        }
    rest_len = (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 21708)).integer.C_1.cd_syslen - used_len + -48;
    t01int4(0 , "used        " , used_len);
    t01int4(0 , "cd_syslen   " , (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 21710)).integer.C_1.cd_syslen);
    if (rest_len >= 100)
        {
        (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 21714)).integer.C_1.cd_syslen = used_len + 48;
        free.integer.C_2.buf_ptr = s35add_bufaddr(curr.integer.C_2.buf_ptr , used_len + 48);
        (*(struct tak_cache_dir_entry *) sql__nil(free.integer.C_1.cdir_ptr , "vak10.p" , 21717)).integer.C_1.cd_buf_no = (short ) sql__setrang((*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 21717)).integer.C_1.cd_buf_no , -32768 , 32767);
        block_pos = (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 21718)).integer.C_2.cd_block_pos + used_len + 48;
        (*(struct tak_cache_dir_entry *) sql__nil(free.integer.C_1.cdir_ptr , "vak10.p" , 21720)).integer.C_2.cd_block_pos = block_pos;
        (*(struct tak_cache_dir_entry *) sql__nil(free.integer.C_1.cdir_ptr , "vak10.p" , 21721)).integer.C_2.cd_block_check = -block_pos;
        (*(struct tak_cache_dir_entry *) sql__nil(free.integer.C_1.cdir_ptr , "vak10.p" , 21722)).integer.C_1.cd_syslen = rest_len;
        (*(struct tak_cache_dir_entry *) sql__nil(free.integer.C_1.cdir_ptr , "vak10.p" , 21723)).integer.C_1.cd_lru_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
        (*(struct tak_cache_dir_entry *) sql__nil(free.integer.C_1.cdir_ptr , "vak10.p" , 21724)).integer.C_1.cd_fix_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
        (*(struct tak_cache_dir_entry *) sql__nil(free.integer.C_1.cdir_ptr , "vak10.p" , 21725)).integer.C_1.cd_lru_prev.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
        (*(struct tak_cache_dir_entry *) sql__nil(free.integer.C_1.cdir_ptr , "vak10.p" , 21726)).integer.C_1.cd_fix_prev.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
        ak10block_into_freelist(&(*acv) , free , 0 , 1);
        }
    cccprint("<ak10free_rest  ");
    }



int ak10get_next_avail(acv, required_len, required_pos, assigned_len, p)
tak_all_command_glob *acv;
tsp_int4 required_len;
tsp_int4 required_pos;
tsp_int4 *assigned_len;
tsp_bufaddr *p;
    {
    tsp_int4 min_pos;
    int min_len;
    tak_cache_dir_pointer first_free;
    tak_cache_dir_pointer curr;
    tak_cache_dir_pointer prev;
    tak_cache_dir_pointer ptr;
    tak_all_command_glob *ptoc_Var192;
    tak_sysdirectory *ptoc_Var193;
    int ptoc_Var194;
    struct tak_cache_dir_entry *ptoc_Var195;
    struct tak_cache_dir_entry *ptoc_Var196;
    cccprint("AK10  ak10get_ne");
    ptoc_Var192 = &*acv;
    ptoc_Var193 = &ptoc_Var192->a_sysdir;
    t01int4(0 , "required_len" , required_len);
    t01int4(0 , "required_pos" , required_pos);
    min_len = sizeof(struct ptoc_Type60 ) * 4 + 48;
    required_len = required_len + 48;
    if (((ptoc_Var194 = required_len % 4) >= 0 ? ptoc_Var194 : ptoc_Var194 + 4) != 0)
        {
        required_len = required_len - ((ptoc_Var194 = required_len % 4) >= 0 ? ptoc_Var194 : ptoc_Var194 + 4) + 4;
        }
    *p = (tsp00_Buf *) (int *) 0;
    first_free.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
    curr = (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var193->ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 21764)).integer.C_2.cd_next_free;
    prev = ptoc_Var193->ci_cache_dir_header;
    while (curr.integer.C_2.buf_ptr != ptoc_Var193->ci_cache_dir_trailor.integer.C_2.buf_ptr)
        {
        ptoc_Var195 = (struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 21767);
        if ((int ) ptoc_Var195->integer.C_1.cd_buf_no > 0)
            {
            curr = ptoc_Var193->ci_cache_dir_trailor;
            }
        else
            {
            if (ptoc_Var195->integer.C_2.cd_block_pos > required_pos && ptoc_Var195->integer.C_1.cd_syslen > min_len)
                {
                first_free = curr;
                curr = ptoc_Var193->ci_cache_dir_trailor;
                }
            else
                {
                prev = curr;
                curr = (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 21782)).integer.C_2.cd_next_free;
                }
            }
        }
    ptr = (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var193->ci_cache_dir_trailor.integer.C_1.cdir_ptr , "vak10.p" , 21788)).integer.C_1.cd_lru_prev;
    min_pos = 2147483647;
    curr.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
    while (ptr.integer.C_1.cdir_ptr != ptoc_Var193->ci_cache_dir_header.integer.C_1.cdir_ptr)
        {
        ptoc_Var196 = (struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 21792);
        if (ptoc_Var196->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr == (struct tak_cache_dir_entry *) (int *) 0 && ptoc_Var196->integer.C_1.cd_pos < min_pos && ptoc_Var196->integer.C_1.cd_pos > required_pos && (int ) ptoc_Var196->integer.C_1.cd_buf_no == 0 && ptoc_Var196->integer.C_1.cd_syslen > min_len)
            {
            min_pos = ptoc_Var196->integer.C_1.cd_pos;
            curr = ptr;
            }
        xtd_directory(0 , ptr);
        ptr = (*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 21806)).integer.C_1.cd_lru_prev;
        }
    t01int4(0 , "min_pos     " , min_pos);
    if (first_free.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        t01int4(0 , "free_pos    " , (*(struct tak_cache_dir_entry *) sql__nil(first_free.integer.C_1.cdir_ptr , "vak10.p" , 21814)).integer.C_2.cd_block_pos);
        }
    if (first_free.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        if ((*(struct tak_cache_dir_entry *) sql__nil(first_free.integer.C_1.cdir_ptr , "vak10.p" , 21818)).integer.C_2.cd_block_pos < min_pos)
            {
            curr = (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var193->ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 21821)).integer.C_2.cd_next_free;
            (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var193->ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 21822)).integer.C_2.cd_next_free = first_free;
            if (required_len > (*(struct tak_cache_dir_entry *) sql__nil(first_free.integer.C_1.cdir_ptr , "vak10.p" , 21823)).integer.C_1.cd_syslen)
                {
                required_len = (*(struct tak_cache_dir_entry *) sql__nil(first_free.integer.C_1.cdir_ptr , "vak10.p" , 21825)).integer.C_1.cd_syslen;
                }
            ak10getmem(&(*acv) , -2 , &required_len , &(*p));
            if (prev.integer.C_1.cdir_ptr != ptoc_Var193->ci_cache_dir_header.integer.C_1.cdir_ptr)
                {
                (*(struct tak_cache_dir_entry *) sql__nil(prev.integer.C_1.cdir_ptr , "vak10.p" , 21831)).integer.C_2.cd_next_free = (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var193->ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 21832)).integer.C_2.cd_next_free;
                (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var193->ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 21833)).integer.C_2.cd_next_free = curr;
                }
            }
        }
    if (*p == (tsp00_Buf *) (int *) 0 && curr.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        if ((*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 21842)).integer.C_1.cd_syslen < required_len)
            {
            required_len = (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 21844)).integer.C_1.cd_syslen;
            }
        ak10temp_add(&(*acv) , curr , 0);
        if (_ptoc_MEMCMP(unsigned char , 2, (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 21847)).integer.C_1.cd_syskey.integer.C_2.sentrytyp , "\0\200") > 0 ? 1 : 0)
            {
            ptoc_Var193->ci_tmp_dislodged = 1;
            }
        ak10block_into_freelist(&(*acv) , curr , 1 , 1);
        ak10td_freelist(&(*acv) , 0);
        ak10verify_cache(&(*acv));
        ak10getmem(&(*acv) , required_pos , &required_len , &(*p));
        }
    *assigned_len = required_len + -48;
    cccprint("<ak10get_next_av");
    }

static
  tak_sysdir_state ptoc_Var197 = 
  {
  (unsigned char)'\200'
  };


int ak10getmem(acv, required_pos, required_len, p)
tak_all_command_glob *acv;
tsp_int4 required_pos;
tsp_int4 *required_len;
tsp_bufaddr *p;
    {
    tgg_basis_error b_err;
    char use_block;
    char exit_loop;
    int loop_cnt;
    tak_cache_dir_pointer curr;
    tak_cache_dir_pointer prev;
    tak_cache_dir_pointer ptr;
    tsp_int4 block_pos;
    tsp_int4 block_len;
    tsp_c16 param;
    int ptoc_Var198;
    struct tak_cache_dir_entry *ptoc_Var199;
    struct tak_cache_dir_entry *ptoc_Var200;
    cccprint("AK10  ak10getmem");
    t01int4(0 , "required_len" , *required_len);
    if (((ptoc_Var198 = *required_len % 4) >= 0 ? ptoc_Var198 : ptoc_Var198 + 4) != 0)
        {
        *required_len = *required_len - ((ptoc_Var198 = *required_len % 4) >= 0 ? ptoc_Var198 : ptoc_Var198 + 4) + 4;
        }
    if (*required_len < 48)
        {
        *required_len = 48;
        }
    *p = (tsp00_Buf *) (int *) 0;
    loop_cnt = 0;
    exit_loop = 0;
    while (*p == (tsp00_Buf *) (int *) 0 && !(char ) sql__setrang(exit_loop , 0 , 1) && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 21900)).integer.C_4.sp1r_returncode == 0)
        {
        loop_cnt = loop_cnt + 1;
        curr = (*(struct tak_cache_dir_entry *) sql__nil((*acv).a_sysdir.ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 21903)).integer.C_2.cd_next_free;
        prev = (*acv).a_sysdir.ci_cache_dir_header;
        while (curr.integer.C_2.buf_ptr != (*acv).a_sysdir.ci_cache_dir_trailor.integer.C_2.buf_ptr)
            {
            ptoc_Var199 = (struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 21907);
            if (ptoc_Var199->integer.C_2.cd_block_pos + ptoc_Var199->integer.C_2.cd_block_check != 0)
                {
                t01int4(0 , "cd_block_pos" , ptoc_Var199->integer.C_2.cd_block_pos);
                t01int4(0 , "cd_block_chk" , ptoc_Var199->integer.C_2.cd_block_check);
                t01int4(0 , "curr.i      " , curr.integer.C_5.i);
                ak10cache_error(&(*acv) , "ak10getmem              " , curr);
                curr = (*acv).a_sysdir.ci_cache_dir_trailor;
                }
            else
                {
                loop_cnt = loop_cnt + 1;
                if ((int ) ptoc_Var199->integer.C_2.cd_mem_type != 10)
                    {
                    g01abort(1 , "AK CACHE" , "dyn_free <> mem_ytpe    " , sql__ord((int ) ptoc_Var199->integer.C_2.cd_mem_type));
                    }
                t01p2int4(0 , "free bl pos " , ptoc_Var199->integer.C_2.cd_block_pos , "free bl len " , ptoc_Var199->integer.C_1.cd_syslen);
                t01p2int4(0 , "free bl buf " , (int ) ptoc_Var199->integer.C_1.cd_buf_no , "free bl adr " , curr.integer.C_5.i);
                t01addr(0 , "free bl addr" , curr.integer.C_2.buf_ptr);
                if (required_pos > 0)
                    {
                    use_block = (char ) sql__setrang((int ) ptoc_Var199->integer.C_1.cd_buf_no == 0 && ptoc_Var199->integer.C_2.cd_block_pos > required_pos && ptoc_Var199->integer.C_1.cd_syslen >= *required_len , 0 , 1);
                    }
                else
                    {
                    use_block = (char ) sql__setrang(ptoc_Var199->integer.C_1.cd_syslen >= *required_len , 0 , 1);
                    }
                if ((char ) sql__setrang(use_block , 0 , 1))
                    {
                    *p = curr.integer.C_2.buf_ptr;
                    if (ptoc_Var199->integer.C_1.cd_syslen - *required_len >= 100)
                        {
                        block_len = ptoc_Var199->integer.C_1.cd_syslen - *required_len;
                        block_pos = ptoc_Var199->integer.C_2.cd_block_pos + *required_len;
                        ptr.integer.C_2.buf_ptr = s35add_bufaddr(*p , *required_len);
                        (*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 21951)).integer.C_1.cd_buf_no = (short ) sql__setrang(ptoc_Var199->integer.C_1.cd_buf_no , -32768 , 32767);
                        (*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 21952)).integer.C_2.cd_block_pos = block_pos;
                        (*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 21953)).integer.C_2.cd_block_check = -block_pos;
                        (_ptoc_MOVE(unsigned char , 1, (*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 21954)).integer.C_1.cd_state, ptoc_Var197) , (*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 21954)).integer.C_1.cd_state);
                        (*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 21955)).integer.C_2.cd_mem_type = 10;
                        (*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 21956)).integer.C_1.cd_syslen = block_len;
                        (*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 21957)).integer.C_1.cd_less.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
                        (*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 21958)).integer.C_1.cd_greater.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
                        (*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 21959)).integer.C_1.cd_fix_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
                        (*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 21960)).integer.C_1.cd_fix_prev.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
                        (*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 21961)).integer.C_1.cd_lru_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
                        (*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 21962)).integer.C_1.cd_lru_prev.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
                        (*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 21963)).integer.C_2.cd_next_free = ptoc_Var199->integer.C_2.cd_next_free;
                        (*(struct tak_cache_dir_entry *) sql__nil(prev.integer.C_1.cdir_ptr , "vak10.p" , 21964)).integer.C_2.cd_next_free = ptr;
                        }
                    else
                        {
                        *required_len = ptoc_Var199->integer.C_1.cd_syslen;
                        (*(struct tak_cache_dir_entry *) sql__nil(prev.integer.C_1.cdir_ptr , "vak10.p" , 21969)).integer.C_2.cd_next_free = ptoc_Var199->integer.C_2.cd_next_free;
                        }
                    ptoc_Var199->integer.C_1.cd_syslen = *required_len;
                    curr = (*acv).a_sysdir.ci_cache_dir_trailor;
                    }
                else
                    {
                    prev = curr;
                    curr = (*(struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 21978)).integer.C_2.cd_next_free;
                    }
                }
            }
        exit_loop = 1;
        if (*p == (tsp00_Buf *) (int *) 0 && required_pos == -2 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 21989)).integer.C_4.sp1r_returncode == 0)
            {
            ptr = (*(struct tak_cache_dir_entry *) sql__nil((*acv).a_sysdir.ci_cache_dir_trailor.integer.C_1.cdir_ptr , "vak10.p" , 21992)).integer.C_1.cd_lru_prev;
            while (ptr.integer.C_1.cdir_ptr != (*acv).a_sysdir.ci_cache_dir_header.integer.C_1.cdir_ptr)
                {
                ptoc_Var200 = (struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 21995);
                if (ptoc_Var200->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr == (struct tak_cache_dir_entry *) (int *) 0)
                    {
                    t01str30(0 , "dislodge block                ");
                    t01buf1(0 , &ptoc_Var200->integer.C_1.cd_syskey , 9 , (int ) ptoc_Var200->integer.C_1.cd_syskey.skeylen + 8);
                    ak10temp_add(&(*acv) , ptr , 0);
                    if (_ptoc_MEMCMP(unsigned char , 2, ptoc_Var200->integer.C_1.cd_syskey.integer.C_2.sentrytyp , "\0\200") > 0 ? 1 : 0)
                        {
                        (*acv).a_sysdir.ci_tmp_dislodged = 1;
                        }
                    ak10block_into_freelist(&(*acv) , ptr , 1 , 1);
                    ptr = (*acv).a_sysdir.ci_cache_dir_header;
                    ak10td_freelist(&(*acv) , 0);
                    ak10verify_cache(&(*acv));
                    exit_loop = 0;
                    }
                else
                    {
                    ptr = (*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 22016)).integer.C_1.cd_lru_prev;
                    }
                }
            }
        }
    if (*p == (tsp00_Buf *) (int *) 0 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 22025)).integer.C_4.sp1r_returncode == 0)
        {
        g01optextmsg(2 , 3 , 51107 , "AK CACHE" , "REQUIRE CACHE DUE TO OVERFLOW           ");
        ak10get_supplement(&(*acv) , &(*p) , *required_len);
        }
    if (*p == (tsp00_Buf *) (int *) 0)
        {
        ak10td_freelist(&(*acv) , 0);
        ak10td_dirlist(&(*acv) , 0);
        ak10no_more_memory(&(*acv) , &b_err);
        _ptoc_ASSIGN(unsigned char , 16, param, "CAT_CACHE_SUPPLY")
        a07_const_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1 , (tsp00_MoveObj *) param , sizeof(tsp00_C16 ));
        }
    cccprint("<ak10getmem     ");
    }



int ak10get_mess_block(acv, sysbuf)
tak_all_command_glob *acv;
tak_sysbufferaddress sysbuf;
    {
    char release_mem;
    tgg_basis_error b_err;
    int ix;
    int part_pos;
    int rec_pos;
    int move_len;
    tgg_sysinfokey sysk;
    int part_cnt;
    tak10_mblock_info mblock_info;
    tgg_rec_buf b;
    tak_messblock_record *ptoc_Var201;
    cccprint("AK10  ak10get_me");
    ptoc_Var201 = &(*(struct tak_systembuffer *) sql__nil(sysbuf , "vak10.p" , 22067)).integer.C_62.smessblock;
    ptoc_Var201->mbr_mess_block.integer.C_9.mb_trns = (tgg00_TransContext *) &(*acv).a_transinf.tri_trans;
    ak10space_for_mblock(&(*acv) , &ptoc_Var201->mbr_mess_block , &ptoc_Var201->mbr_stack_len , &ptoc_Var201->mbr_mess_block , &b_err);
    if ((int ) b_err == 0)
        {
        release_mem = 1;
        sysk = (*(struct tak_systembuffer *) sql__nil(sysbuf , "vak10.p" , 22076)).integer.C_50.syskey;
        sysk.skeylen = (short ) sql__setrang((short ) ((int ) sysk.skeylen + 1) , -32768 , 32767);
        sysk.integer.C_8.all[sql__setrang((int ) sysk.skeylen , 1 , 140) + -1] = (unsigned char)'\0';
        ak10fill_mblock_info(&(*(struct tak_systembuffer *) sql__nil(sysbuf , "vak10.p" , 22079)).integer.C_62.smessblock , mblock_info , &part_cnt);
        }
    else
        {
        release_mem = 0;
        }
    ix = 1;
    rec_pos = 1;
    b.integer.C_12.len = 0;
    while (ix <= part_cnt && (int ) b_err == 0)
        {
        part_pos = 1;
        do
            {
            if (rec_pos > (int ) b.integer.C_12.len)
                {
                sysk.integer.C_8.all[sql__setrang((int ) sysk.skeylen , 1 , 140) + -1] = (unsigned char ) sql__setrang((unsigned char ) sql__succ((int ) sysk.integer.C_8.all[sql__setrang((int ) sysk.skeylen , 1 , 140) + -1] , 0 , 255 , "vak10.p" , 22095) , 0 , 255);
                if (_ptoc_MEMCMP(unsigned char , 2, (*(struct tak_systembuffer *) sql__nil(sysbuf , "vak10.p" , 22096)).integer.C_50.syskey.integer.C_2.sentrytyp , "\0\030") == 0 ? 1 : 0)
                    {
                    ak10get_perm_record(&(*acv) , &sysk , &b , &b_err);
                    }
                else
                    {
                    b07cget_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , &sysk , &b);
                    b_err = (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                    (*acv).a_transinf.tri_trans.trError_gg00 = 0;
                    }
                rec_pos = (int ) sysk.skeylen + 9;
                }
            if ((int ) b_err == 0)
                {
                move_len = (int ) b.integer.C_12.len - rec_pos + 1;
                if (part_pos + move_len + -1 > mblock_info[sql__setrang(ix , 1 , 4) + -1].len)
                    {
                    move_len = mblock_info[sql__setrang(ix , 1 , 4) + -1].len - part_pos + 1;
                    }
                g10mv11("VAK10 " , 26 , sizeof(tsp00_Buf ) , mblock_info[sql__setrang(ix , 1 , 4) + -1].size , b.integer.C_11.buf , rec_pos , sql__nil(mblock_info[sql__setrang(ix , 1 , 4) + -1].addr , "vak10.p" , 22120) , part_pos , move_len , &b_err);
                part_pos = part_pos + move_len;
                rec_pos = rec_pos + move_len;
                }
            }
        while (!(part_pos > mblock_info[sql__setrang(ix , 1 , 4) + -1].len || (int ) b_err != 0));
        if (ptoc_Var201->mbr_mess_block.integer.C_4.mb_qual != (tgg00_QualBuf *) (int *) 0 && ptoc_Var201->mbr_mess_block.integer.C_5.mb_st != (tgg00_StackList *) (int *) 0)
            {
            (*(tgg00_QualBuf *) sql__nil(ptoc_Var201->mbr_mess_block.integer.C_4.mb_qual , "vak10.p" , 22132)).integer.C_2.mst_addr = ptoc_Var201->mbr_mess_block.integer.C_5.mb_st;
            }
        ix = ix + 1;
        }
    if ((int ) b_err != 0)
        {
        if ((char ) sql__setrang(release_mem , 0 , 1))
            {
            ak10free_mblock(&(*acv) , &ptoc_Var201->mbr_mess_block);
            }
        a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
        }
    t01messblock(0 , "get_mess_blo" , &ptoc_Var201->mbr_mess_block);
    cccprint("<ak10get_mess_bl");
    }



int ak10hit_rate(hits, reads)
tsp_int4 hits;
tsp_int4 reads;
    {
    int _ak10hit_rate;
    int rate;
    int pos;
    tsp_c40 msg40;
    int ptoc_Var202;
    cccprint("AK10  ak10hit_ra");
    if (reads > 0)
        {
        if (reads > 2147483)
            {
            hits = hits / 1000;
            reads = reads / 1000;
            }
        _ak10hit_rate = hits * 1000 / reads + 5;
        if (hits * 1000 / reads + 5 < 900)
            {
            _ptoc_ASSIGN(unsigned char , 40, msg40, "HIT RATE : 000.0 %                      ")
            pos = 16;
            rate = hits * 1000 / reads + 5;
            while (rate > 0)
                {
                msg40[sql__setrang(pos , 1 , 40) + -1] = (unsigned char ) sql__setrang((unsigned char ) (((ptoc_Var202 = rate % 10) >= 0 ? ptoc_Var202 : ptoc_Var202 + 10) + sql__ord(48)) , 0 , 255);
                if (pos == 16)
                    {
                    pos = pos + -2;
                    }
                else
                    {
                    pos = pos + -1;
                    }
                rate = rate / 10;
                }
            g01optextmsg(2 , 3 , 51108 , "AK CACHE" , msg40);
            }
        }
    else
        {
        _ak10hit_rate = 0;
        }
    cccprint("<ak10hit_rate   ");
    return(_ak10hit_rate);
    }

typedef
  struct ptoc_Type33 tgg_rec_buf_ak10get_colnames_record;


int ak10get_colnames_record(acv, syspoint)
tak_all_command_glob *acv;
tak_sysbufferaddress syspoint;
    {
    tgg_basis_error b_err;
    tsp_int4 restlen;
    tsp_int4 required;
    tak_sysbufferaddress curr;
    tgg_sysinfokey sysk;
    struct ptoc_Type210
      {
      union  
        {
        struct  
          {
          tak_sysbufferaddress sysp;
          } C_true;
        struct  
          {
          tgg_rec_buf_ak10get_colnames_record *rbuf;
          } C_false;
        } boolean;
      } p;
    tak_cache_dir_pointer aux;
    cccprint("AK10  ak10get_co");
    (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 22234)).integer.C_59.scolnames.boolean.C_false.cnnext = (struct tak_systembuffer *) (int *) 0;
    (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 22235)).integer.C_59.scolnames.boolean.C_false.cnlast = syspoint;
    t01int4(0 , "cnfullen    " , (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 22236)).integer.C_59.scolnames.boolean.C_true.cnfullen);
    restlen = (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 22237)).integer.C_59.scolnames.boolean.C_true.cnfullen - sizeof(int ) - (sizeof(tak_colnamesbuf ) - sizeof(int ) - sizeof(short ));
    if (restlen > 0)
        {
        curr = syspoint;
        s10mv2(sizeof(tgg00_SysInfoKey ) , sizeof(tgg00_SysInfoKey ) , &(*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 22247)).integer.C_50.syskey , 1 , &sysk , 1 , (int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 22248)).integer.C_50.syskey.skeylen + 8);
        do
            {
            a06inc_linkage(sysk.integer.C_2.slinkage);
            t01int4(0 , "restlen     " , restlen);
            required = (int ) sysk.skeylen + restlen + 8;
            if (required >= 8096)
                {
                required = sizeof(tak_columnnamesrecord );
                }
            a10new(&(*acv) , required , &p.boolean.C_true.sysp);
            if (p.boolean.C_true.sysp != (struct tak_systembuffer *) (int *) 0)
                {
                b07cget_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , &sysk , sql__nil(p.boolean.C_false.rbuf , "vak10.p" , 22262));
                b_err = (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                if ((int ) b_err == 0)
                    {
                    restlen = restlen - ((int ) (*(tgg00_Rec *) sql__nil(p.boolean.C_false.rbuf , "vak10.p" , 22268)).integer.C_12.len - (int ) (*(tgg00_Rec *) sql__nil(p.boolean.C_false.rbuf , "vak10.p" , 22268)).integer.C_12.keylen) + 8;
                    (*(struct tak_systembuffer *) sql__nil(curr , "vak10.p" , 22269)).integer.C_59.scolnames.boolean.C_false.cnnext = p.boolean.C_true.sysp;
                    (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 22270)).integer.C_59.scolnames.boolean.C_false.cnlast = p.boolean.C_true.sysp;
                    curr = p.boolean.C_true.sysp;
                    if ((int ) (*(struct tak_systembuffer *) sql__nil(curr , "vak10.p" , 22272)).integer.C_0.b_sl == 8096)
                        {
                        (*(struct tak_systembuffer *) sql__nil(curr , "vak10.p" , 22274)).integer.C_59.scolnames.boolean.C_false.cnnext = (struct tak_systembuffer *) (int *) 0;
                        }
                    }
                }
            else
                {
                ak10no_more_memory(&(*acv) , &b_err);
                }
            }
        while (!(restlen <= 0 || (int ) b_err != 0));
        if ((int ) b_err != 0)
            {
            aux.integer.C_4.sys_ptr = curr;
            aux.integer.C_2.buf_ptr = s35add_bufaddr(aux.integer.C_2.buf_ptr , -48);
            ak10block_into_freelist(&(*acv) , aux , 0 , 1);
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
            }
        }
    cccprint("<ak10get_colname");
    }



int ak10get_method_record(acv, rec, syspoint)
tak_all_command_glob *acv;
tgg_rec_buf *rec;
tak_sysbufferaddress syspoint;
    {
    tgg_basis_error e;
    int ix;
    int pos;
    int parm_no;
    int move_len;
    tak_sysbufferaddress rec_ptr;
    tgg_sysinfokey sysk;
    struct ptoc_Type211
      {
      union  
        {
        struct  
          {
          tak_sysbufferaddress mobj;
          } C_true;
        struct  
          {
          tak_param_info_ptr pp;
          } C_false;
        } boolean;
      } p;
    int ptoc_Var203;
    cccprint("AK10  ak10get_me");
    e = 0;
    rec_ptr = (struct tak_systembuffer *) (int *) 0;
    pos = sizeof(tak_methodrecord ) - sizeof(tak_param_array ) + 1;
    parm_no = 1;
    while (parm_no <= (int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 22333)).integer.C_66.smethod.me_param_cnt)
        {
        if (pos > (int ) (*rec).integer.C_12.len)
            {
            if (rec_ptr == (struct tak_systembuffer *) (int *) 0)
                {
                rec_ptr = (struct tak_systembuffer *) &*rec;
                sysk = (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 22342)).integer.C_50.syskey;
                }
            a06inc_linkage(sysk.integer.C_2.slinkage);
            ak10get_perm_record(&(*acv) , &sysk , &(*rec) , &e);
            pos = sizeof(tsp00_C2 ) + 21;
            }
        if ((int ) e == 0)
            {
            move_len = sizeof(tak_param_info ) - sizeof(tsp00_C256 ) + sql__ord((int ) (*rec).integer.C_11.buf[sql__setrang(pos + sizeof(tak_param_info ) - sizeof(tsp00_C256 ) + -1 , 1 , 8192) + -1]);
            t01int4(5 , "move_len    " , move_len);
            a10new(&(*acv) , move_len , &p.boolean.C_true.mobj);
            if (p.boolean.C_true.mobj == (struct tak_systembuffer *) (int *) 0)
                {
                ak10no_more_memory(&(*acv) , &e);
                }
            else
                {
                (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 22364)).integer.C_66.smethod.me_param_list[sql__setrang(parm_no , 1 , 1024) + -1] = p.boolean.C_false.pp;
                g10mv1("VAK10 " , 27 , sizeof(tsp00_Buf ) , move_len , (*rec).integer.C_11.buf , pos , sql__nil(p.boolean.C_false.pp , "vak10.p" , 22367) , 1 , move_len , &e);
                pos = pos + move_len;
                }
            }
        if ((int ) e != 0)
            {
            ptoc_Var203 = parm_no + -1;
            if (1 <= ptoc_Var203)
                {
                ix = 1;
                do
                    {
                    p.boolean.C_false.pp = (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 22378)).integer.C_66.smethod.me_param_list[sql__setrang(ix , 1 , 1024) + -1];
                    a10dispose(&(*acv) , &p.boolean.C_true.mobj);
                    if (ix == ptoc_Var203)
                        {
                        break;
                        }
                    ix += 1;
                    }
                while (!0);
                }
            if ((int ) e == 1200)
                {
                e = 1930;
                }
            a07_b_put_error(&(*acv) , (short ) sql__setrang(e , -32768 , 32767) , 1);
            parm_no = 32767;
            }
        else
            {
            parm_no = parm_no + 1;
            }
        }
    cccprint("<ak10get_method_");
    }



int ak10get_perm_record(acv, syskey, rbuf, b_err)
tak_all_command_glob *acv;
tgg_sysinfokey *syskey;
tgg_rec_buf *rbuf;
tgg_basis_error *b_err;
    {
    tgg_mess_block mblock;
    tak10qual qual;
    cccprint("AK10  ak10get_pe");
    *b_err = 0;
    g01mblock_init(&(*acv).a_transinf.tri_trans , 30 , 16 , &mblock);
    g01stack_init(&mblock , (tgg00_StackList *) (int *) 0 , 0 , (tgg00_QualBuf *) &qual , sizeof(tak10qual ));
    g01datapart_init(&mblock , (tgg00_DataPart *) &*rbuf , sizeof(tgg00_Rec ));
    mblock.integer.C_2.mb_qual_len = sizeof(unsigned char [112]);
    g10mv("VAK10 " , 28 , sizeof(tgg00_SysInfoKey ) , mblock.integer.C_2.mb_data_size , &(*syskey) , 9 , (*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak10.p" , 22417)).integer.C_1.mbp_buf , 9 , (int ) (*syskey).skeylen , &(*b_err));
    (*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak10.p" , 22418)).integer.C_4.mbp_keylen = (short ) sql__setrang((*syskey).skeylen , -32768 , 32767);
    mblock.integer.C_2.mb_data_len = (int ) (*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak10.p" , 22420)).integer.C_4.mbp_keylen + 8;
    if ((int ) *b_err == 0)
        {
        ak10send_mess_block(&(*acv) , &mblock , (*syskey).integer.C_2.sentrytyp , 1 , &(*b_err));
        }
    if ((int ) *b_err == 0)
        {
        a10expand_rec(&(*(tgg00_DataPart *) sql__nil(mblock.integer.C_3.mb_data , "vak10.p" , 22428)).integer.C_3.mbp_rec , &(*b_err));
        }
    cccprint("<ak10get_perm_re");
    }

static
  tak_sysdir_state ptoc_Var204 = 
  {
  (unsigned char)'\200'
  };
static
  tak_sysdir_state ptoc_Var205 = 
  {
  (unsigned char)'\200'
  };


int ak10get_supplement(acv, p, required_len)
tak_all_command_glob *acv;
tsp_bufaddr *p;
tsp_int4 required_len;
    {
    int bufno;
    tak_cache_dir_pointer curr;
    tsp_int4 supplement;
    struct ptoc_Type212
      {
      union  
        {
        struct  
          {
          tak_cache_dir_pointer cdir_ptr;
          } C_1;
        struct  
          {
          tak10cache_header_ptr chptr;
          } C_2;
        struct  
          {
          tsp_int4 i;
          } C_3;
        } integer;
      } univ_ptr;
    struct ptoc_Type213
      {
      union  
        {
        struct  
          {
          tsp_bufaddr bufaddr;
          } C_true;
        struct  
          {
          tak10cache_header_ptr chptr;
          } C_false;
        } boolean;
      } prev_cache;
    struct tak_cache_dir_entry *ptoc_Var206;
    struct tak_cache_dir_entry *ptoc_Var207;
    cccprint("AK10  ak10get_su");
    prev_cache.boolean.C_true.bufaddr = (*acv).a_sysdir.ci_cache_p;
    bufno = 1;
    while ((*(tak10cache_header *) sql__nil(prev_cache.boolean.C_false.chptr , "vak10.p" , 22471)).ch_next_cache != (tsp00_Buf *) (int *) 0)
        {
        bufno = bufno + 1;
        prev_cache.boolean.C_true.bufaddr = (*(tak10cache_header *) sql__nil(prev_cache.boolean.C_false.chptr , "vak10.p" , 22474)).ch_next_cache;
        }
    ak10alloc_mem(&(*acv) , 0 , required_len , &univ_ptr.integer.C_1.cdir_ptr.integer.C_2.buf_ptr , &supplement);
    if (univ_ptr.integer.C_1.cdir_ptr.integer.C_2.buf_ptr != (tsp00_Buf *) (int *) 0)
        {
        g01optextmsg(2 , 3 , 51109 , "AK CACHE" , "CACHE SUPPLEMENT GRANTED                ");
        a51inc_ak_cache_size((*acv).a_size_ptr , supplement);
        (*(tak10cache_header *) sql__nil(prev_cache.boolean.C_false.chptr , "vak10.p" , 22486)).ch_next_cache = univ_ptr.integer.C_1.cdir_ptr.integer.C_2.buf_ptr;
        (*(tak10cache_header *) sql__nil(univ_ptr.integer.C_2.chptr , "vak10.p" , 22487)).ch_next_cache = (tsp00_Buf *) (int *) 0;
        _ptoc_ASSIGN(unsigned char , 4, (*(tak10cache_header *) sql__nil(univ_ptr.integer.C_2.chptr , "vak10.p" , 22488)).ch_label, ak10ChunkLabel)
        (*(tak10cache_header *) sql__nil(univ_ptr.integer.C_2.chptr , "vak10.p" , 22489)).ch_label[3] = (unsigned char ) sql__setrang((unsigned char ) (sql__ord(48) + bufno) , 0 , 255);
        (*(tak10cache_header *) sql__nil(univ_ptr.integer.C_2.chptr , "vak10.p" , 22490)).ch_size = supplement;
        if (required_len > 0)
            {
            curr.integer.C_2.buf_ptr = s35add_bufaddr(univ_ptr.integer.C_1.cdir_ptr.integer.C_2.buf_ptr , sizeof(tak10cache_header ));
            ptoc_Var206 = (struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 22496);
            ptoc_Var206->integer.C_2.cd_block_pos = sizeof(tak10cache_header ) + 1;
            ptoc_Var206->integer.C_2.cd_block_check = -ptoc_Var206->integer.C_2.cd_block_pos;
            ptoc_Var206->integer.C_1.cd_syslen = required_len;
            ptoc_Var206->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
            ptoc_Var206->integer.C_1.cd_fix_prev.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
            ptoc_Var206->integer.C_1.cd_less.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
            ptoc_Var206->integer.C_1.cd_greater.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
            ptoc_Var206->integer.C_1.cd_lru_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
            ptoc_Var206->integer.C_1.cd_lru_prev.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
            _ptoc_MOVE(unsigned char , 1, ptoc_Var206->integer.C_1.cd_state, ptoc_Var204);
            ptoc_Var206->integer.C_1.cd_buf_no = (short ) sql__setrang((short ) bufno , -32768 , 32767);
            *p = curr.integer.C_2.buf_ptr;
            }
        if (supplement * sizeof(tsp00_Buf ) - required_len - sizeof(tak10cache_header ) >= 148)
            {
            curr.integer.C_2.buf_ptr = s35add_bufaddr(univ_ptr.integer.C_1.cdir_ptr.integer.C_2.buf_ptr , sizeof(tak10cache_header ) + required_len);
            ptoc_Var207 = (struct tak_cache_dir_entry *) sql__nil(curr.integer.C_1.cdir_ptr , "vak10.p" , 22522);
            ptoc_Var207->integer.C_2.cd_block_pos = sizeof(tak10cache_header ) + required_len + 1;
            ptoc_Var207->integer.C_2.cd_block_check = -ptoc_Var207->integer.C_2.cd_block_pos;
            ptoc_Var207->integer.C_1.cd_syslen = supplement * sizeof(tsp00_Buf ) - ptoc_Var207->integer.C_2.cd_block_pos + 1;
            ptoc_Var207->integer.C_1.cd_buf_no = (short ) sql__setrang((short ) bufno , -32768 , 32767);
            ptoc_Var207->integer.C_1.cd_lru_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
            ptoc_Var207->integer.C_1.cd_lru_prev.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
            ptoc_Var207->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
            ptoc_Var207->integer.C_1.cd_fix_prev.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
            _ptoc_MOVE(unsigned char , 1, ptoc_Var207->integer.C_1.cd_state, ptoc_Var205);
            ak10block_into_freelist(&(*acv) , curr , 0 , 1);
            ak10td_freelist(&(*acv) , 0);
            }
        }
    cccprint("<ak10get_supplem");
    }



int ak10ins_directory(acv, find_info, p, ins_p, incremented)
tak_all_command_glob *acv;
tak10_find_info *find_info;
tak_cache_dir_pointer *p;
tak_cache_dir_pointer *ins_p;
char *incremented;
    {
    tak_cache_dir_pointer p1;
    tak_cache_dir_pointer p2;
    struct tak_cache_dir_entry *ptoc_Var208;
    cccprint("AK10  ak10ins_di");
    if ((*p).integer.C_1.cdir_ptr == (struct tak_cache_dir_entry *) (int *) 0)
        {
        *p = *ins_p;
        *incremented = 1;
        ptoc_Var208 = (struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22566);
        ptoc_Var208->integer.C_1.cd_less.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
        ptoc_Var208->integer.C_1.cd_greater.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
        ptoc_Var208->integer.C_1.cd_balance = 1;
        }
    else
        {
        s30cmp(&(*(struct tak_cache_dir_entry *) sql__nil((*ins_p).integer.C_1.cdir_ptr , "vak10.p" , 22576)).integer.C_1.cd_syskey , 9 , (int ) (*(struct tak_cache_dir_entry *) sql__nil((*ins_p).integer.C_1.cdir_ptr , "vak10.p" , 22577)).integer.C_1.cd_syskey.skeylen , &(*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22578)).integer.C_1.cd_syskey , 9 , (int ) (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22579)).integer.C_1.cd_syskey.skeylen , &(*find_info).fi_res);
        switch ((int ) (*find_info).fi_res)
            {
            case 0 :
                ak10ins_directory(&(*acv) , &(*find_info) , &(*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22583)).integer.C_1.cd_less , &(*ins_p) , &(*incremented));
                if ((char ) sql__setrang(*incremented , 0 , 1))
                    {
                    switch ((int ) (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22587)).integer.C_1.cd_balance)
                        {
                        case 0 :
                            p1 = (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22590)).integer.C_1.cd_less;
                            if ((int ) (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 22591)).integer.C_1.cd_balance == 0)
                                {
                                (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22594)).integer.C_1.cd_less = (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 22595)).integer.C_1.cd_greater;
                                (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 22596)).integer.C_1.cd_greater = *p;
                                (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22597)).integer.C_1.cd_balance = 1;
                                *p = p1;
                                }
                            else
                                {
                                p2 = (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 22602)).integer.C_1.cd_greater;
                                (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 22603)).integer.C_1.cd_greater = (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 22604)).integer.C_1.cd_less;
                                (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 22605)).integer.C_1.cd_less = p1;
                                (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22606)).integer.C_1.cd_less = (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 22606)).integer.C_1.cd_greater;
                                (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 22607)).integer.C_1.cd_greater = *p;
                                if ((int ) (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 22608)).integer.C_1.cd_balance == 0)
                                    {
                                    (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22610)).integer.C_1.cd_balance = 2;
                                    }
                                else
                                    {
                                    (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22612)).integer.C_1.cd_balance = 1;
                                    }
                                if ((int ) (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 22614)).integer.C_1.cd_balance == 2)
                                    {
                                    (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 22616)).integer.C_1.cd_balance = 0;
                                    }
                                else
                                    {
                                    (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 22618)).integer.C_1.cd_balance = 1;
                                    }
                                *p = p2;
                                }
                            (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22623)).integer.C_1.cd_balance = 1;
                            *incremented = 0;
                            break;
                        case 1 :
                            (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22627)).integer.C_1.cd_balance = 0;
                            break;
                        case 2 :
                            (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22630)).integer.C_1.cd_balance = 1;
                            *incremented = 0;
                            break;
                        default :
                            ak10cache_error(&(*acv) , "ak10ins_directory       " , *p);
                            break;
                        }
                    }
                break;
            case 2 :
                ak10ins_directory(&(*acv) , &(*find_info) , &(*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22641)).integer.C_1.cd_greater , &(*ins_p) , &(*incremented));
                if ((char ) sql__setrang(*incremented , 0 , 1))
                    {
                    switch ((int ) (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22645)).integer.C_1.cd_balance)
                        {
                        case 0 :
                            (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22648)).integer.C_1.cd_balance = 1;
                            *incremented = 0;
                            break;
                        case 1 :
                            (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22652)).integer.C_1.cd_balance = 2;
                            break;
                        case 2 :
                            p1 = (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22655)).integer.C_1.cd_greater;
                            if ((int ) (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 22656)).integer.C_1.cd_balance == 2)
                                {
                                (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22659)).integer.C_1.cd_greater = (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 22660)).integer.C_1.cd_less;
                                (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 22661)).integer.C_1.cd_less = *p;
                                (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22662)).integer.C_1.cd_balance = 1;
                                *p = p1;
                                }
                            else
                                {
                                p2 = (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 22667)).integer.C_1.cd_less;
                                (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 22668)).integer.C_1.cd_less = (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 22669)).integer.C_1.cd_greater;
                                (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 22670)).integer.C_1.cd_greater = p1;
                                (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22671)).integer.C_1.cd_greater = (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 22671)).integer.C_1.cd_less;
                                (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 22672)).integer.C_1.cd_less = *p;
                                if ((int ) (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 22673)).integer.C_1.cd_balance == 2)
                                    {
                                    (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22675)).integer.C_1.cd_balance = 0;
                                    }
                                else
                                    {
                                    (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22677)).integer.C_1.cd_balance = 1;
                                    }
                                if ((int ) (*(struct tak_cache_dir_entry *) sql__nil(p2.integer.C_1.cdir_ptr , "vak10.p" , 22679)).integer.C_1.cd_balance == 0)
                                    {
                                    (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 22681)).integer.C_1.cd_balance = 2;
                                    }
                                else
                                    {
                                    (*(struct tak_cache_dir_entry *) sql__nil(p1.integer.C_1.cdir_ptr , "vak10.p" , 22683)).integer.C_1.cd_balance = 1;
                                    }
                                *p = p2;
                                }
                            (*(struct tak_cache_dir_entry *) sql__nil((*p).integer.C_1.cdir_ptr , "vak10.p" , 22688)).integer.C_1.cd_balance = 1;
                            *incremented = 0;
                            break;
                        default :
                            ak10cache_error(&(*acv) , "ak10ins_directory       " , *p);
                            break;
                        }
                    }
                break;
            default :
                *incremented = 0;
                break;
            }
        }
    cccprint("<ak10ins_directo");
    }

static
  unsigned char ptoc_Var209[1] = 
  {
  (unsigned char)'\002'
  };
static
  unsigned char ptoc_Var210[1] = 
  {
  (unsigned char)'\002'
  };


int ak10lock_unlock_fixed(acv, lock)
tak_all_command_glob *acv;
char lock;
    {
    tak_cache_dir_pointer p;
    tak_all_command_glob *ptoc_Var211;
    tak_sysdirectory *ptoc_Var212;
    struct tak_cache_dir_entry *ptoc_Var213;
    unsigned char ptoc_Var214[1];
    unsigned char ptoc_Var215[1];
    unsigned char ptoc_Var216[1];
    cccprint("AK10  ak10lock_u");
    t01int4(0 , "lock        " , sql__ord((int ) (char ) sql__setrang(lock , 0 , 1)));
    ptoc_Var211 = &*acv;
    ptoc_Var212 = &ptoc_Var211->a_sysdir;
    p = (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var212->ci_cache_dir_trailor.integer.C_1.cdir_ptr , "vak10.p" , 22723)).integer.C_1.cd_fix_prev;
    while (p.integer.C_1.cdir_ptr != ptoc_Var212->ci_cache_dir_header.integer.C_1.cdir_ptr)
        if ((*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 22726)).integer.C_1.cd_pos + (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 22726)).integer.C_1.cd_check != 0)
            {
            ak10cache_error(&(*acv) , "ak10lock_unlock         " , p);
            p = ptoc_Var212->ci_cache_dir_header;
            }
        else
            {
            ptoc_Var213 = (struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 22733);
            xtd_directory(0 , p);
            if ((char ) sql__setrang(lock , 0 , 1))
                {
                _ptoc_MOVE(unsigned char , 1, ptoc_Var213->integer.C_1.cd_state, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var214 , ptoc_Var213->integer.C_1.cd_state , ptoc_Var209 , sizeof(unsigned char [1])));
                }
            else
                {
                _ptoc_MOVE(unsigned char , 1, ptoc_Var213->integer.C_1.cd_state, *(unsigned char (*)[1]) sql__setsub(ptoc_Var216 , ptoc_Var213->integer.C_1.cd_state , ptoc_Var210 , sizeof(unsigned char [1])));
                }
            p = (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 22742)).integer.C_1.cd_fix_prev;
            }
    cccprint("<ak10lock_unlock");
    }



int ak10invalidate_root(treeid, p)
tgg00_FileId *treeid;
tak_cache_dir_pointer p;
    {
    struct tak_cache_dir_entry *ptoc_Var217;
    cccprint("AK10  ak10invali");
    if (p.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        ptoc_Var217 = (struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 22764);
        if (_ptoc_MEMCMP(unsigned char , 2, ptoc_Var217->integer.C_1.cd_syskey.integer.C_2.sentrytyp , "\0\001") == 0 ? 1 : 0)
            {
            if (_ptoc_MEMCMP(unsigned char , 16, ptoc_Var217->integer.C_2.cd_sysinfo.integer.C_1.sbase.btreeid.integer.C_1.fileName_gg00 , (*treeid).integer.C_1.fileName_gg00) == 0 ? 1 : 0)
                {
                ptoc_Var217->integer.C_2.cd_sysinfo.integer.C_1.sbase.btreeid.integer.C_1.fileRoot_gg00 = 2147483647;
                }
            }
        ak10invalidate_root(&(*treeid) , ptoc_Var217->integer.C_1.cd_less);
        ak10invalidate_root(&(*treeid) , ptoc_Var217->integer.C_1.cd_greater);
        }
    cccprint("<ak10invalidate_");
    }

static
  tak_sysdir_state ptoc_Var218 = 
  {
  (unsigned char)'\0'
  };
static
  tak_sysdir_state ptoc_Var219 = 
  {
  (unsigned char)'\020'
  };


int ak10put_mbuf_into_cache(acv, rbuf, dstate, find_info, syskey, syspoint)
tak_all_command_glob *acv;
tgg_rec_buf *rbuf;
tak_directory_state dstate;
tak10_find_info *find_info;
tgg_sysinfokey *syskey;
tak_sysbufferaddress *syspoint;
    {
    char found_in_cache;
    char dummy;
    char is_base_rec;
    char is_syn_rec;
    int maxcol;
    int move_len;
    int rest_len;
    tsp_int4 syslen;
    tak_cache_dir_pointer p;
    tak_sysbufferaddress rbuf_ptr;
    struct ptoc_Type214
      {
      union  
        {
        struct  
          {
          tsp_bufaddr buf_ptr;
          } C_true;
        struct  
          {
          tak_sysbufferaddress sys_ptr;
          } C_false;
        } boolean;
      } univ_ptr;
    int ptoc_Var220;
    tak_baserecord *ptoc_Var221;
    struct tak_cache_dir_entry *ptoc_Var222;
    cccprint("AK10  ak10put_mb");
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 22812)).integer.C_4.sp1r_returncode == 0)
        {
        rbuf_ptr = (struct tak_systembuffer *) &*rbuf;
        found_in_cache = 0;
        is_base_rec = 0;
        rest_len = 0;
        if ((int ) (*find_info).fi_kind == 2)
            {
            g10mv5("VAK10 " , 29 , sizeof(tsp00_Buf ) , sizeof(tgg00_SysInfoKey ) , (*rbuf).integer.C_11.buf , 1 , &(*syskey) , 1 , (int ) (*rbuf).integer.C_12.keylen + 8 , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 22825)).integer.C_4.sp1r_returncode);
            ak10find_syskey(&(*acv) , &(*syskey) , &(*find_info));
            if ((int ) (*find_info).fi_res == 1)
                {
                p = (*find_info).fi_ptr;
                *syspoint = (struct tak_systembuffer *) &(*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 22831)).integer.C_2.cd_sysinfo;
                found_in_cache = 1;
                (*(struct tak_cache_dir_entry *) sql__nil((*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 22834)).integer.C_1.cd_lru_prev.integer.C_1.cdir_ptr , "vak10.p" , 22834)).integer.C_1.cd_lru_next = (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 22835)).integer.C_1.cd_lru_next;
                (*(struct tak_cache_dir_entry *) sql__nil((*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 22836)).integer.C_1.cd_lru_next.integer.C_1.cdir_ptr , "vak10.p" , 22836)).integer.C_1.cd_lru_prev = (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 22837)).integer.C_1.cd_lru_prev;
                }
            }
        if (!(char ) sql__setrang(found_in_cache , 0 , 1))
            {
            if ((int ) (*find_info).fi_kind != 0)
                {
                if ((_ptoc_MEMCMP(unsigned char , 2, (*syskey).integer.C_2.sentrytyp , "\0\301") == 0 ? 1 : 0) || (_ptoc_MEMCMP(unsigned char , 2, (*syskey).integer.C_2.sentrytyp , "\0\030") == 0 ? 1 : 0) || (_ptoc_MEMCMP(unsigned char , 2, (*syskey).integer.C_2.sentrytyp , "\0\225") == 0 ? 1 : 0))
                    {
                    (*rbuf).integer.C_12.len = (short ) sql__setrang((short ) sizeof(tak_messblock_record ) , -32768 , 32767);
                    }
                }
            syslen = (int ) (*rbuf).integer.C_12.len;
            if ((int ) (*find_info).fi_kind != 0)
                {
                if ((int ) (*rbuf).integer.C_12.info[9] < 128)
                    {
                    if (_ptoc_MEMCMP(unsigned char , 2, (*syskey).integer.C_2.sentrytyp , "\0\030") == 0 ? 1 : 0)
                        {
                        ak10get_mess_block(&(*acv) , (struct tak_systembuffer *) &*rbuf);
                        }
                    }
                else
                    {
                    if ((int ) (*find_info).fi_kind == 1)
                        {
                        switch ((int ) (*syskey).integer.C_2.sentrytyp[1])
                            {
                            case 130 :
                            case 129 :
                            case 132 :
                            case 178 :
                            case 190 :
                                univ_ptr.boolean.C_true.buf_ptr = (tsp00_Buf *) &*rbuf;
                                rest_len = (*(struct tak_systembuffer *) sql__nil(univ_ptr.boolean.C_false.sys_ptr , "vak10.p" , 22875)).integer.C_23.sparsinfo.integer.C_1.p_fullen - syslen;
                                syslen = (*(struct tak_systembuffer *) sql__nil(univ_ptr.boolean.C_false.sys_ptr , "vak10.p" , 22877)).integer.C_23.sparsinfo.integer.C_1.p_fullen;
                                break;
                            case 192 :
                                if (syslen == 8096)
                                    {
                                    syslen = sizeof(tak_columnnamesrecord );
                                    }
                                break;
                            case 193 :
                            case 149 :
                                ak10get_mess_block(&(*acv) , (struct tak_systembuffer *) &*rbuf);
                                break;
                            default :
                                break;
                            }
                        }
                    }
                }
            if ((int ) (*find_info).fi_kind != 0 && ((ptoc_Var220 = (int ) (*(struct tak_systembuffer *) sql__nil(rbuf_ptr , "vak10.p" , 22894)).integer.C_50.syskey.integer.C_2.sentrytyp[1]) == 1 || ptoc_Var220 == 177 || ptoc_Var220 == 105))
                {
                switch ((int ) (*(struct tak_systembuffer *) sql__nil(rbuf_ptr , "vak10.p" , 22897)).integer.C_50.syskey.integer.C_2.sentrytyp[1])
                    {
                    case 105 :
                        syslen = sizeof(tak_methodrecord ) - sizeof(tak_param_array ) + (int ) (*(struct tak_systembuffer *) sql__nil(rbuf_ptr , "vak10.p" , 22902)).integer.C_66.smethod.me_param_cnt * sizeof(tak_param_info_ptr ) + 48;
                        break;
                    default :
                        ptoc_Var221 = &(*(struct tak_systembuffer *) sql__nil(rbuf_ptr , "vak10.p" , 22905)).integer.C_1.sbase;
                        is_base_rec = 1;
                        if ((int ) ptoc_Var221->btablekind != 8)
                            {
                            is_syn_rec = 0;
                            if ((int ) (*acv).a_is_ddl == 0)
                                {
                                maxcol = (int ) ptoc_Var221->bmaxcol;
                                }
                            else
                                {
                                maxcol = 1024;
                                }
                            ptoc_Var221->bfirstindex = (short ) sql__setrang(a11firstindex(maxcol , &(*(struct tak_systembuffer *) sql__nil(rbuf_ptr , "vak10.p" , 22918)).integer.C_1.sbase) , -32768 , 32767);
                            syslen = sizeof(tak_baserecord ) - sizeof(tak_columnarr ) + ((int ) ptoc_Var221->bfirstindex + maxcol) * sizeof(tak00_colinfo_ptr ) + 48;
                            }
                        else
                            {
                            is_syn_rec = 1;
                            syslen = sizeof(tak_synonymrecord ) + 48;
                            }
                        break;
                    }
                }
            else
                {
                syslen = syslen + 48;
                }
            ak10getmem(&(*acv) , -2 , &syslen , &p.integer.C_2.buf_ptr);
            if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 22939)).integer.C_4.sp1r_returncode == 0)
                {
                ptoc_Var222 = (struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 22942);
                *syspoint = (struct tak_systembuffer *) &ptoc_Var222->integer.C_2.cd_sysinfo;
                ptoc_Var222->integer.C_1.cd_cmd_no = (*acv).a_command_count;
                if ((int ) (*find_info).fi_kind == 0)
                    {
                    ptoc_Var222->integer.C_1.cd_create_cmd_no = (*acv).a_command_count;
                    _ptoc_MOVE(unsigned char , 1, ptoc_Var222->integer.C_1.cd_state, ptoc_Var218);
                    move_len = (int ) (*rbuf).integer.C_12.keylen + 8;
                    }
                else
                    {
                    _ptoc_MOVE(unsigned char , 1, ptoc_Var222->integer.C_1.cd_state, ptoc_Var219);
                    ptoc_Var222->integer.C_1.cd_create_cmd_no = -2;
                    if ((char ) sql__setrang(is_base_rec , 0 , 1))
                        {
                        if ((char ) sql__setrang(is_syn_rec , 0 , 1))
                            {
                            move_len = 60;
                            }
                        else
                            {
                            move_len = a10BaseRecPersistentOffset();
                            }
                        }
                    else
                        {
                        switch ((int ) (*(struct tak_systembuffer *) sql__nil(rbuf_ptr , "vak10.p" , 22966)).integer.C_50.syskey.integer.C_2.sentrytyp[1])
                            {
                            case 105 :
                                move_len = sizeof(tak_methodrecord ) - sizeof(tak_param_array );
                                break;
                            default :
                                move_len = (int ) (*rbuf).integer.C_12.len;
                                break;
                            }
                        }
                    }
                t01int4(0 , "move_len    " , move_len);
                g10mv2("VAK10 " , 30 , sizeof(tsp00_Buf ) , sizeof(tsp00_Buf ) , (*rbuf).integer.C_11.buf , 1 , sql__nil(*syspoint , "vak10.p" , 22980) , 1 , move_len , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 22981)).integer.C_4.sp1r_returncode);
                if (rest_len > 0)
                    {
                    ak10restore_record_suffix(&(*acv) , &(*rbuf) , *syspoint , rest_len);
                    }
                else
                    {
                    if ((char ) sql__setrang(is_base_rec , 0 , 1))
                        {
                        if ((char ) sql__setrang(is_syn_rec , 0 , 1))
                            {
                            ak10synonym_record(&(*rbuf) , *syspoint , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 22992)).integer.C_4.sp1r_returncode);
                            }
                        else
                            {
                            (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 22995)).integer.C_0.b_sl = (short ) sql__setrang((short ) (syslen + -48) , -32768 , 32767);
                            ak10base_record(&(*acv) , &(*rbuf) , *syspoint);
                            }
                        }
                    else
                        {
                        if ((int ) (*find_info).fi_kind != 0)
                            {
                            switch ((int ) (*syskey).integer.C_2.sentrytyp[1])
                                {
                                case 192 :
                                    if ((int ) (*(struct tak_systembuffer *) sql__nil(*syspoint , "vak10.p" , 23004)).integer.C_0.b_sl == 8096)
                                        {
                                        ak10get_colnames_record(&(*acv) , *syspoint);
                                        }
                                    break;
                                case 105 :
                                    ak10get_method_record(&(*acv) , &(*rbuf) , *syspoint);
                                    break;
                                default :
                                    break;
                                }
                            }
                        }
                    }
                if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 23017)).integer.C_4.sp1r_returncode == 0)
                    {
                    ak10ins_directory(&(*acv) , &(*find_info) , &(*acv).a_sysdir.ci_dir[sql__setrang(sql__ord((int ) ptoc_Var222->integer.C_1.cd_syskey.integer.C_2.sentrytyp[1]) , 1 , 195) + -1] , &p , &dummy);
                    }
                xtd_directory(0 , p);
                }
            }
        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 23030)).integer.C_4.sp1r_returncode == 0)
            {
            (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 23034)).integer.C_1.cd_lru_next = (*(struct tak_cache_dir_entry *) sql__nil((*acv).a_sysdir.ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 23035)).integer.C_1.cd_lru_next;
            (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 23036)).integer.C_1.cd_lru_prev = (*acv).a_sysdir.ci_cache_dir_header;
            (*(struct tak_cache_dir_entry *) sql__nil((*(struct tak_cache_dir_entry *) sql__nil((*acv).a_sysdir.ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 23037)).integer.C_1.cd_lru_next.integer.C_1.cdir_ptr , "vak10.p" , 23037)).integer.C_1.cd_lru_prev = p;
            (*(struct tak_cache_dir_entry *) sql__nil((*acv).a_sysdir.ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 23039)).integer.C_1.cd_lru_next = p;
            if ((int ) dstate == 0 && (!(char ) sql__setrang(found_in_cache , 0 , 1) || (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 23042)).integer.C_1.cd_fix_next.integer.C_1.cdir_ptr == (struct tak_cache_dir_entry *) (int *) 0))
                {
                (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 23046)).integer.C_1.cd_fix_next = (*(struct tak_cache_dir_entry *) sql__nil((*acv).a_sysdir.ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 23047)).integer.C_1.cd_fix_next;
                (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 23048)).integer.C_1.cd_fix_prev = (*acv).a_sysdir.ci_cache_dir_header;
                (*(struct tak_cache_dir_entry *) sql__nil((*(struct tak_cache_dir_entry *) sql__nil((*acv).a_sysdir.ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 23049)).integer.C_1.cd_fix_next.integer.C_1.cdir_ptr , "vak10.p" , 23049)).integer.C_1.cd_fix_prev = p;
                (*(struct tak_cache_dir_entry *) sql__nil((*acv).a_sysdir.ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 23051)).integer.C_1.cd_fix_next = p;
                }
            }
        }
    cccprint("<ak10put_mbuf_in");
    }



int ak10release_table(tabid, p)
tgg00_Surrogate tabid;
tak_cache_dir_pointer p;
    {
    struct tak_cache_dir_entry *ptoc_Var223;
    cccprint("AK10  ak10releas");
    ptoc_Var223 = (struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 23070);
    if ((_ptoc_MEMCMP(unsigned char , 8, ptoc_Var223->integer.C_1.cd_syskey.integer.C_2.stableid , tabid) == 0 ? 1 : 0) && ptoc_Var223->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var223->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr , "vak10.p" , 23075)).integer.C_1.cd_fix_prev = ptoc_Var223->integer.C_1.cd_fix_prev;
        (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var223->integer.C_1.cd_fix_prev.integer.C_1.cdir_ptr , "vak10.p" , 23076)).integer.C_1.cd_fix_next = ptoc_Var223->integer.C_1.cd_fix_next;
        ptoc_Var223->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
        }
    if (ptoc_Var223->integer.C_1.cd_less.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        ak10release_table(tabid , ptoc_Var223->integer.C_1.cd_less);
        }
    if (ptoc_Var223->integer.C_1.cd_greater.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        ak10release_table(tabid , ptoc_Var223->integer.C_1.cd_greater);
        }
    cccprint("<ak10release_tab");
    }



int ak10restore_record_suffix(acv, rbuf, syspoint, rest_len)
tak_all_command_glob *acv;
tgg_rec_buf *rbuf;
tak_sysbufferaddress syspoint;
int rest_len;
    {
    tgg_basis_error b_err;
    int move_len;
    tgg_sysinfokey syskey;
    cccprint("AK10  ak10restor");
    syskey = (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 23107)).integer.C_50.syskey;
    syskey.skeylen = (short ) sql__setrang((short ) ((int ) syskey.skeylen + 1) , -32768 , 32767);
    syskey.integer.C_8.all[sql__setrang((int ) syskey.skeylen , 1 , 140) + -1] = (unsigned char)'\0';
    do
        {
        syskey.integer.C_8.all[sql__setrang((int ) syskey.skeylen , 1 , 140) + -1] = (unsigned char ) sql__setrang((unsigned char ) sql__succ((int ) syskey.integer.C_8.all[sql__setrang((int ) syskey.skeylen , 1 , 140) + -1] , 0 , 255 , "vak10.p" , 23111) , 0 , 255);
        b07cget_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , &syskey , &(*rbuf));
        b_err = (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
        if ((int ) b_err == 0)
            {
            move_len = (int ) (*rbuf).integer.C_12.len - (int ) (*rbuf).integer.C_12.keylen + -8;
            g10mv2("VAK10 " , 31 , sizeof(tsp00_Buf ) , sizeof(struct tak_systembuffer ) , (*rbuf).integer.C_11.buf , (int ) (*rbuf).integer.C_12.keylen + 9 , sql__nil(syspoint , "vak10.p" , 23122) , (int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 23122)).integer.C_0.b_sl + 1 , move_len , &b_err);
            (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 23123)).integer.C_0.b_sl = (short ) sql__setrang((short ) ((int ) (*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 23123)).integer.C_0.b_sl + move_len) , -32768 , 32767);
            rest_len = rest_len - move_len;
            }
        if ((int ) b_err != 0)
            {
            a07ak_system_error(&(*acv) , 10 , 3);
            rest_len = 0;
            }
        }
    while (!(rest_len <= 0));
    cccprint("<ak10restore_rec");
    }

static
  tgg00_HandlingSet ptoc_Var224 = 
  {
  (unsigned char)'\024', (unsigned char)'\0'
  };
static
  tgg00_HandlingSet ptoc_Var225 = 
  {
  (unsigned char)'\004', (unsigned char)'\0'
  };
static
  tgg00_HandlingSet ptoc_Var226 = 
  {
  (unsigned char)'\0', (unsigned char)'\0'
  };


int ak10send_mess_block(acv, mblock, ptoc_ptr_entrytype, return_req, b_err)
tak_all_command_glob *acv;
tgg_mess_block *mblock;
tsp_c2 ptoc_ptr_entrytype;
char return_req;
tgg_basis_error *b_err;
    {
    tak_sm_description sm_desc;
    tak_sm_description *ptoc_Var227;
    tsp_c2 entrytype;
    _ptoc_MOVE(char, sizeof(tsp_c2 ), entrytype, ptoc_ptr_entrytype);
    cccprint("AK10  ak10send_m");
    if (_ptoc_MEMCMP(unsigned char , 2, entrytype , "\0\200") < 0 ? 0 : 1)
        {
        (*(tgg00_QualBuf *) sql__nil((*mblock).integer.C_4.mb_qual , "vak10.p" , 23154)).integer.C_2.mtree = (*acv).a_pars_curr;
        }
    else
        {
        if (_ptoc_MEMCMP(unsigned char , 2, entrytype , "\0@") < 0 ? 0 : 1)
            {
            (*(tgg00_QualBuf *) sql__nil((*mblock).integer.C_4.mb_qual , "vak10.p" , 23158)).integer.C_2.mtree = g01tabid.sys2_cat;
            }
        else
            {
            (*(tgg00_QualBuf *) sql__nil((*mblock).integer.C_4.mb_qual , "vak10.p" , 23160)).integer.C_2.mtree = g01tabid.sys1_cat;
            }
        }
    if ((int ) (*acv).a_command_kind == 7)
        {
        (_ptoc_MOVE(unsigned char , 2, (*(tgg00_QualBuf *) sql__nil((*mblock).integer.C_4.mb_qual , "vak10.p" , 23165)).integer.C_2.mtree.integer.C_1.fileHandling_gg00, ptoc_Var224) , (*(tgg00_QualBuf *) sql__nil((*mblock).integer.C_4.mb_qual , "vak10.p" , 23165)).integer.C_2.mtree.integer.C_1.fileHandling_gg00);
        }
    else
        {
        if ((int ) (*acv).a_is_ddl == 0 && ((_ptoc_MEMCMP(unsigned char , 2, entrytype , "\0@") < 0 ? 0 : 1) || !(char ) sql__setrang((*acv).a_parsing_again , 0 , 1)))
            {
            (_ptoc_MOVE(unsigned char , 2, (*(tgg00_QualBuf *) sql__nil((*mblock).integer.C_4.mb_qual , "vak10.p" , 23170)).integer.C_2.mtree.integer.C_1.fileHandling_gg00, ptoc_Var225) , (*(tgg00_QualBuf *) sql__nil((*mblock).integer.C_4.mb_qual , "vak10.p" , 23170)).integer.C_2.mtree.integer.C_1.fileHandling_gg00);
            }
        else
            {
            if (!(char ) sql__setrang(g01restart_local , 0 , 1))
                {
                (_ptoc_MOVE(unsigned char , 2, (*(tgg00_QualBuf *) sql__nil((*mblock).integer.C_4.mb_qual , "vak10.p" , 23174)).integer.C_2.mtree.integer.C_1.fileHandling_gg00, ptoc_Var226) , (*(tgg00_QualBuf *) sql__nil((*mblock).integer.C_4.mb_qual , "vak10.p" , 23174)).integer.C_2.mtree.integer.C_1.fileHandling_gg00);
                }
            }
        }
    ptoc_Var227 = &sm_desc;
    ptoc_Var227->sm_error_cnt = 0;
    ptoc_Var227->sm_return_req = (char ) sql__setrang(return_req , 0 , 1);
    ptoc_Var227->sm_must_local = 0;
    ptoc_Var227->sm_not_local = 0;
    g10fil("VAK10 " , 32 , (*mblock).integer.C_2.mb_data_size , (*(tgg00_DataPart *) sql__nil((*mblock).integer.C_3.mb_data , "vak10.p" , 23188)).integer.C_1.mbp_buf , (*mblock).integer.C_2.mb_data_len + 1 , (*mblock).integer.C_2.mb_data_size - (*mblock).integer.C_2.mb_data_len , (unsigned char)'\0' , &(*b_err));
    a06ignore_rsend_mess_buf(&(*acv) , &(*mblock) , &sm_desc , &(*b_err));
    cccprint("<ak10send_mess_b");
    }



int ak10size_check(acv, ptoc_ptr_entrytype, length)
tak_all_command_glob *acv;
tsp_c2 ptoc_ptr_entrytype;
int length;
    {
    int len;
    tgg_basis_error error;
    tsp_c2 entrytype;
    _ptoc_MOVE(char, sizeof(tsp_c2 ), entrytype, ptoc_ptr_entrytype);
    cccprint("AK10  ak10size_c");
    error = 6450;
    switch ((int ) entrytype[1])
        {
        case 1 :
            len = sizeof(tak_baserecord );
            break;
        case 192 :
            len = sizeof(tak_columnnamesrecord );
            break;
        case 96 :
            len = sizeof(tak_comment_record );
            break;
        case 137 :
            len = sizeof(tak_complexrecord );
            break;
        case 18 :
            len = sizeof(tak_constraintrecord );
            break;
        case 22 :
            len = sizeof(tak_constraintnamerecord );
            break;
        case 131 :
            len = sizeof(tak_correcord );
            break;
        case 17 :
            len = sizeof(tak_defaultrecord );
            break;
        case 70 :
            len = sizeof(tak_domainrecord );
            break;
        case 78 :
            len = sizeof(tak_domainref );
            break;
        case 130 :
            len = sizeof(struct ptoc_Type215
 {
 tsp_int2 jp_reclen;
 tsp_int2 jp_keylen;
 tsp_int2 jp_varcol_offset;
 tsp_int2 jp_varcol_cnt;
 tgg_surrogate jp_surrogate;
 tsp_c2 jp_entrytype;
 tsp_int2 jp_segno;
 tsp_int4 jp_fullen;
 tgg00_FileId jp_restree;
 tsp_int2 jp_d_inoutpos;
 tsp_int2 jp_d_cntfromtab;
 tsp_int2 jp_d_reclen;
 tsp_int2 jp_d_keylen;
 tsp_int2 jp_eq_cnt;
 unsigned char jp_sr_strategy;
 char jp_sr_use_rowno;
 char jp_sr_use_subquery;
 char jp_sr_distinct_bytes;
 char jp_d_standard;
 char jp_d_single;
 unsigned char jp_d_distinct;
 char jp_d_union;
 char jp_search_sequence;
 char jp_a_outer_join;
 char jp_nextexist;
 tsp00_Uint1 jp_joincnt;
 tsp_int2 jp_d_order_cnt;
 tsp_int2 jp_d_rowno;
 tgg00_HandlingSet jp_d_globstate;
 tsp00_Uint1 jp_d_session_isolevel;
 char jp_d_use_session_isolevel;
 char jp_d_one_join_phase;
 tsp_c1 jp_d_filler1;
 tsp_c4 jp_d_filler2;
 unsigned char jp_d_buffer[22416];
 } );
            break;
        case 20 :
        case 19 :
            len = sizeof(tak_linkrecord );
            break;
        case 15 :
            len = sizeof(tak_link_def_record );
            break;
        case 148 :
            len = sizeof(tak_linkparsrecord );
            break;
        case 190 :
            len = sizeof(tak_linkinforecord );
            break;
        case 67 :
            len = sizeof(tak_serverdbrecord );
            break;
        case 69 :
            len = sizeof(tak_serverdbnamerecord );
            break;
        case 105 :
            len = sizeof(tak_methodrecord );
            break;
        case 180 :
            len = sizeof(tak_longinforecord );
            break;
        case 103 :
            len = sizeof(tak_mindexrecord );
            break;
        case 129 :
            len = sizeof(tak_parsinforecord );
            break;
        case 6 :
            len = sizeof(tak_privrecord );
            break;
        case 7 :
            len = sizeof(tak_privuserrecord );
            break;
        case 178 :
            len = sizeof(tak_resultkeyrecord );
            break;
        case 177 :
            len = sizeof(tak_resultrecord );
            break;
        case 187 :
            len = sizeof(tak_scoldescrecord );
            break;
        case 82 :
            len = sizeof(tak_sequencerecord );
            break;
        case 132 :
            len = sizeof(tak_shortinforecord );
            break;
        case 150 :
            len = sizeof(tak_subtransrecord );
            break;
        case 73 :
            len = sizeof(tak_sysdbarecord );
            break;
        case 65 :
            len = sizeof(tak_tablereference );
            break;
        case 79 :
            len = sizeof(tgg00_TermsetRecord );
            break;
        case 21 :
            len = sizeof(tak_triggerrecord );
            break;
        case 135 :
            len = sizeof(tak_unionrecord );
            break;
        case 72 :
            len = sizeof(tak_usagerecord );
            break;
        case 66 :
            len = sizeof(tak_userrecord );
            break;
        case 85 :
            len = sizeof(tak_useridrecord );
            break;
        case 77 :
            len = sizeof(tak_userrefrecord );
            break;
        case 71 :
            len = sizeof(tak_usertablerecord );
            break;
        case 10 :
            len = sizeof(tak_viewdescrecord );
            break;
        case 136 :
            len = sizeof(tak_viewkeyrecord );
            break;
        case 32 :
            len = sizeof(tak_viewqual_basis_record );
            break;
        case 33 :
            len = sizeof(tak_viewqual_join_record );
            break;
        case 34 :
            len = sizeof(tak_viewqual_stack_record );
            break;
        case 35 :
            len = sizeof(tak_viewqual_stack_record );
            break;
        case 36 :
            len = sizeof(tak_viewqual_derivedcol_record );
            break;
        case 8 :
            len = sizeof(tak_viewtextrecord );
            break;
        case 181 :
            len = sizeof(tak_resname_record );
            break;
        case 81 :
            len = sizeof(tak_mapset_record );
            break;
        case 88 :
            len = sizeof(tak_funcrefrecord );
            break;
        case 75 :
        case 83 :
            len = sizeof(tak_proc_seq_privilege );
            break;
        case 97 :
            len = sizeof(tak_col_uses_dom );
            break;
        default :
            len = 8096;
            break;
        }
    if (length > len || len > 32767)
        {
        t01int4(0 , "etype       " , sql__ord((int ) entrytype[1]));
        t01int4(0 , "required_len" , length);
        t01int4(0 , "max_len     " , len);
        a07_b_put_error(&(*acv) , (short ) sql__setrang(error , -32768 , 32767) , 1);
        }
    cccprint("<ak10size_check ");
    }



int ak10space_for_mblock(acv, in_mblock, stack_length, out_mblock, b_err)
tak_all_command_glob *acv;
tgg_mess_block *in_mblock;
tsp_int4 *stack_length;
tgg_mess_block *out_mblock;
tgg_basis_error *b_err;
    {
    int ix;
    int block_cnt;
    tsp_int4 required_len;
    tak_cache_dir_pointer block_addr[4];
    int ptoc_Var228;
    cccprint("AK10  ak10space_");
    *b_err = 0;
    (*out_mblock).integer.C_2.mb_qual_size = 0;
    (*out_mblock).integer.C_2.mb_data_size = 0;
    (*out_mblock).integer.C_2.mb_strat_size = 0;
    (*out_mblock).integer.C_2.mb_st_size = 0;
    (*out_mblock).integer.C_3.mb_data = (tgg00_DataPart *) (int *) 0;
    (*out_mblock).integer.C_4.mb_qual = (tgg00_QualBuf *) (int *) 0;
    (*out_mblock).integer.C_5.mb_st = (tgg00_StackList *) (int *) 0;
    (*out_mblock).integer.C_11.mb_strat = (tsp00_MoveObj *) (int *) 0;
    block_cnt = 0;
    if ((*in_mblock).integer.C_2.mb_qual_len > 0)
        {
        if ((*in_mblock).integer.C_5.mb_st != (tgg00_StackList *) (int *) 0 && *stack_length == -2)
            {
            *stack_length = ((int ) (*(tgg00_QualBuf *) sql__nil((*in_mblock).integer.C_4.mb_qual , "vak10.p" , 23360)).integer.C_2.mfirst_free + -1) * sizeof(tgg00_StackEntry );
            }
        required_len = (*in_mblock).integer.C_2.mb_qual_len + 48;
        if (*stack_length > 0 && required_len < sizeof(tgg00_FileId ) + sizeof(tgg00_StackDesc ) + 48)
            {
            required_len = sizeof(tgg00_FileId ) + sizeof(tgg00_StackDesc ) + 48;
            }
        ak10getmem(&(*acv) , -2 , &required_len , &block_addr[0].integer.C_2.buf_ptr);
        if (block_addr[0].integer.C_2.buf_ptr != (tsp00_Buf *) (int *) 0)
            {
            block_cnt = 1;
            (*out_mblock).integer.C_2.mb_qual_size = (*in_mblock).integer.C_2.mb_qual_len;
            (*out_mblock).integer.C_4.mb_qual = (tgg00_QualBuf *) &(*(struct tak_cache_dir_entry *) sql__nil(block_addr[sql__setrang(block_cnt , 1 , 4) + -1].integer.C_1.cdir_ptr , "vak10.p" , 23379)).integer.C_2.cd_sysinfo;
            if (*stack_length > 0)
                {
                required_len = *stack_length + 48;
                ak10getmem(&(*acv) , -2 , &required_len , &block_addr[sql__setrang(block_cnt + 1 , 1 , 4) + -1].integer.C_2.buf_ptr);
                if (block_addr[sql__setrang(block_cnt + 1 , 1 , 4) + -1].integer.C_2.buf_ptr != (tsp00_Buf *) (int *) 0)
                    {
                    block_cnt = block_cnt + 1;
                    (*out_mblock).integer.C_5.mb_st = (tgg00_StackList *) &(*(struct tak_cache_dir_entry *) sql__nil(block_addr[sql__setrang(block_cnt , 1 , 4) + -1].integer.C_1.cdir_ptr , "vak10.p" , 23391)).integer.C_2.cd_sysinfo;
                    (*(tgg00_QualBuf *) sql__nil((*out_mblock).integer.C_4.mb_qual , "vak10.p" , 23392)).integer.C_2.mst_addr = (*out_mblock).integer.C_5.mb_st;
                    (*out_mblock).integer.C_2.mb_st_size = required_len + -48;
                    (*out_mblock).integer.C_2.mb_st_max = (short ) sql__setrang((short ) ((*out_mblock).integer.C_2.mb_st_size / sizeof(tgg00_StackEntry )) , -32768 , 32767);
                    }
                else
                    {
                    ak10no_more_memory(&(*acv) , &(*b_err));
                    }
                }
            }
        else
            {
            ak10no_more_memory(&(*acv) , &(*b_err));
            }
        }
    if ((*in_mblock).integer.C_2.mb_data_len > 0 && (int ) *b_err == 0)
        {
        required_len = (*in_mblock).integer.C_2.mb_data_len + 48;
        ak10getmem(&(*acv) , -2 , &required_len , &block_addr[sql__setrang(block_cnt + 1 , 1 , 4) + -1].integer.C_2.buf_ptr);
        if (block_addr[sql__setrang(block_cnt + 1 , 1 , 4) + -1].integer.C_2.buf_ptr != (tsp00_Buf *) (int *) 0)
            {
            block_cnt = block_cnt + 1;
            (*out_mblock).integer.C_2.mb_data_size = (*in_mblock).integer.C_2.mb_data_len;
            (*out_mblock).integer.C_3.mb_data = (tgg00_DataPart *) &(*(struct tak_cache_dir_entry *) sql__nil(block_addr[sql__setrang(block_cnt , 1 , 4) + -1].integer.C_1.cdir_ptr , "vak10.p" , 23421)).integer.C_2.cd_sysinfo;
            }
        else
            {
            ak10no_more_memory(&(*acv) , &(*b_err));
            }
        }
    if ((*in_mblock).integer.C_2.mb_strat_len > 0 && (int ) *b_err == 0)
        {
        required_len = (*in_mblock).integer.C_2.mb_strat_len + 48;
        ak10getmem(&(*acv) , -2 , &required_len , &block_addr[sql__setrang(block_cnt + 1 , 1 , 4) + -1].integer.C_2.buf_ptr);
        if (block_addr[sql__setrang(block_cnt + 1 , 1 , 4) + -1].integer.C_2.buf_ptr != (tsp00_Buf *) (int *) 0)
            {
            block_cnt = block_cnt + 1;
            (*out_mblock).integer.C_2.mb_strat_size = (*in_mblock).integer.C_2.mb_strat_len;
            (*out_mblock).integer.C_11.mb_strat = (tsp00_MoveObj *) &(*(struct tak_cache_dir_entry *) sql__nil(block_addr[sql__setrang(block_cnt , 1 , 4) + -1].integer.C_1.cdir_ptr , "vak10.p" , 23440)).integer.C_2.cd_sysinfo;
            }
        else
            {
            ak10no_more_memory(&(*acv) , &(*b_err));
            }
        }
    if ((int ) *b_err != 0)
        {
        (*out_mblock).integer.C_2.mb_qual_size = 0;
        (*out_mblock).integer.C_2.mb_data_size = 0;
        (*out_mblock).integer.C_2.mb_strat_size = 0;
        (*out_mblock).integer.C_2.mb_st_size = 0;
        (*out_mblock).integer.C_3.mb_data = (tgg00_DataPart *) (int *) 0;
        (*out_mblock).integer.C_4.mb_qual = (tgg00_QualBuf *) (int *) 0;
        (*out_mblock).integer.C_11.mb_strat = (tsp00_MoveObj *) (int *) 0;
        (*out_mblock).integer.C_5.mb_st = (tgg00_StackList *) (int *) 0;
        ptoc_Var228 = block_cnt;
        if (1 <= ptoc_Var228)
            {
            ix = 1;
            do
                {
                ak10block_into_freelist(&(*acv) , block_addr[sql__setrang(ix , 1 , 4) + -1] , 0 , 1);
                if (ix == ptoc_Var228)
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
        ptoc_Var228 = block_cnt;
        if (1 <= ptoc_Var228)
            {
            ix = 1;
            do
                {
                (*(struct tak_cache_dir_entry *) sql__nil(block_addr[sql__setrang(ix , 1 , 4) + -1].integer.C_1.cdir_ptr , "vak10.p" , 23465)).integer.C_2.cd_mem_type = 14;
                if (ix == ptoc_Var228)
                    {
                    break;
                    }
                ix += 1;
                }
            while (!0);
            }
        }
    t01int4(0 , "mb_qual_size" , (*out_mblock).integer.C_2.mb_qual_size);
    t01int4(0 , "mb_data_size" , (*out_mblock).integer.C_2.mb_data_size);
    t01int4(0 , "mb_strat_siz" , (*out_mblock).integer.C_2.mb_strat_size);
    t01int4(0 , "mb_st_size  " , (*out_mblock).integer.C_2.mb_st_size);
    if ((*out_mblock).integer.C_3.mb_data != (tgg00_DataPart *) (int *) 0)
        {
        t01addr(0 , "mb_data     " , (tsp00_Buf *) (int *) sql__nil((*out_mblock).integer.C_3.mb_data , "vak10.p" , 23474));
        }
    if ((*out_mblock).integer.C_4.mb_qual != (tgg00_QualBuf *) (int *) 0)
        {
        t01addr(0 , "mb_qual     " , (tsp00_Buf *) (int *) sql__nil((*out_mblock).integer.C_4.mb_qual , "vak10.p" , 23478));
        }
    if ((*out_mblock).integer.C_5.mb_st != (tgg00_StackList *) (int *) 0)
        {
        t01addr(0 , "mb_st       " , (tsp00_Buf *) (int *) sql__nil((*out_mblock).integer.C_5.mb_st , "vak10.p" , 23482));
        }
    if ((*out_mblock).integer.C_11.mb_strat != (tsp00_MoveObj *) (int *) 0)
        {
        t01addr(0 , "mb_strat    " , (tsp00_Buf *) (int *) sql__nil((*out_mblock).integer.C_11.mb_strat , "vak10.p" , 23486));
        }
    cccprint("<ak10space_for_m");
    }



int ak10store_record_suffix(acv, sysinfo, do_add, rest_len, b_err)
tak_all_command_glob *acv;
struct tak_systembuffer *sysinfo;
char do_add;
int rest_len;
tgg_basis_error *b_err;
    {
    int move_len;
    int pos;
    tsp_buf rec;
    tak_sysbufferaddress rec_ptr;
    cccprint("AK10  ak10store_");
    t01buf(0 , &(*sysinfo) , 1 , (int ) (*sysinfo).integer.C_0.b_sl);
    rec_ptr = (struct tak_systembuffer *) rec;
    (*sysinfo).integer.C_0.b_sl = (short ) sql__setrang((short ) ((int ) (*sysinfo).integer.C_0.b_sl + rest_len) , -32768 , 32767);
    (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23509)).integer.C_50.syskey = (*sysinfo).integer.C_50.syskey;
    (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23510)).integer.C_50.syskey.skeylen = (short ) sql__setrang((short ) ((int ) (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23510)).integer.C_50.syskey.skeylen + 1) , -32768 , 32767);
    (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23511)).integer.C_50.syskey.integer.C_8.all[sql__setrang((int ) (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23511)).integer.C_50.syskey.skeylen , 1 , 140) + -1] = (unsigned char)'\0';
    pos = 8097;
    while (rest_len > 0 && (int ) *b_err == 0)
        {
        (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23515)).integer.C_50.syskey.integer.C_8.all[sql__setrang((int ) (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23515)).integer.C_50.syskey.skeylen , 1 , 140) + -1] = (unsigned char ) sql__setrang((unsigned char ) sql__succ((int ) (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23516)).integer.C_50.syskey.integer.C_8.all[sql__setrang((int ) (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23516)).integer.C_50.syskey.skeylen , 1 , 140) + -1] , 0 , 255 , "vak10.p" , 23516) , 0 , 255);
        if ((int ) (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23517)).integer.C_50.syskey.skeylen + rest_len + 8 > 8096)
            {
            move_len = 8088 - (int ) (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23521)).integer.C_50.syskey.skeylen;
            }
        else
            {
            move_len = rest_len;
            }
        (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23525)).integer.C_0.b_sl = (short ) sql__setrang((short ) ((int ) (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23526)).integer.C_50.syskey.skeylen + move_len + 8) , -32768 , 32767);
        g10mv12("VAK10 " , 33 , sizeof(struct tak_systembuffer ) , sizeof(tsp00_Buf ) , &(*sysinfo) , pos , sql__nil(rec_ptr , "vak10.p" , 23529) , (int ) (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23529)).integer.C_50.syskey.skeylen + 9 , move_len , &(*b_err));
        if ((int ) *b_err == 0)
            {
            if ((char ) sql__setrang(do_add , 0 , 1))
                {
                b07cadd_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , sql__nil(rec_ptr , "vak10.p" , 23537));
                }
            else
                {
                b07crepl_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , sql__nil(rec_ptr , "vak10.p" , 23540));
                }
            *b_err = (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
            (*acv).a_transinf.tri_trans.trError_gg00 = 0;
            rest_len = rest_len - move_len;
            pos = pos + move_len;
            }
        }
    cccprint("<ak10store_recor");
    }



int ak10store_mess_block(acv, mblock_rec, do_add, b_err)
tak_all_command_glob *acv;
tak_messblock_record *mblock_rec;
char do_add;
tgg_basis_error *b_err;
    {
    tgg_message_type m_type;
    int move_len;
    int part_cnt;
    int ix;
    int part_pos;
    int cnt;
    tgg_rec_buf_ak10get_colnames_record rec_buf;
    tak_sysbufferaddress rec_ptr;
    tak_sysbufferaddress mblock_ptr;
    tgg_sysinfokey sysk;
    tak10_mblock_info mblock_info;
    int ptoc_Var229;
    cccprint("AK10  ak10store_");
    t01messblock(0 , "ak10store_me" , &(*mblock_rec).mbr_mess_block);
    (*mblock_rec).mbr_reclen = (short ) sql__setrang((short ) ((int ) (*mblock_rec).mbr_reclen - sizeof(tgg00_MessBufTrailer )) , -32768 , 32767);
    mblock_ptr = (struct tak_systembuffer *) &*mblock_rec;
    if (_ptoc_MEMCMP(unsigned char , 2, (*mblock_rec).mbr_entrytype , "\0\030") == 0 ? 1 : 0)
        {
        if ((char ) sql__setrang(do_add , 0 , 1))
            {
            m_type = 35;
            }
        else
            {
            m_type = 77;
            }
        ak10send_ins_upd(&(*acv) , (struct tak_systembuffer *) &*mblock_rec , (unsigned char ) sql__setrang(m_type , 0 , 95) , &(*b_err));
        }
    else
        {
        if ((char ) sql__setrang(do_add , 0 , 1))
            {
            b07cadd_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , sql__nil(mblock_ptr , "vak10.p" , 23595));
            }
        else
            {
            b07crepl_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , sql__nil(mblock_ptr , "vak10.p" , 23598));
            }
        *b_err = (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
        (*acv).a_transinf.tri_trans.trError_gg00 = 0;
        }
    (*mblock_rec).mbr_reclen = (short ) sql__setrang((short ) ((int ) (*mblock_rec).mbr_reclen + sizeof(tgg00_MessBufTrailer )) , -32768 , 32767);
    rec_ptr = (struct tak_systembuffer *) &rec_buf;
    (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23607)).integer.C_50.syskey = (*(struct tak_systembuffer *) sql__nil(mblock_ptr , "vak10.p" , 23607)).integer.C_50.syskey;
    (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23608)).integer.C_50.syskey.skeylen = (short ) sql__setrang((short ) ((int ) (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23608)).integer.C_50.syskey.skeylen + 1) , -32768 , 32767);
    (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23609)).integer.C_50.syskey.integer.C_8.all[sql__setrang((int ) (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23609)).integer.C_50.syskey.skeylen , 1 , 140) + -1] = (unsigned char)'\001';
    ak10fill_mblock_info(&(*mblock_rec) , mblock_info , &part_cnt);
    ix = 1;
    cnt = 1;
    rec_buf.integer.C_12.len = (short ) sql__setrang((short ) ((int ) (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23613)).integer.C_50.syskey.skeylen + 8) , -32768 , 32767);
    while ((int ) *b_err == 0 && ix <= part_cnt)
        {
        part_pos = 1;
        do
            {
            move_len = mblock_info[sql__setrang(ix , 1 , 4) + -1].len - part_pos + 1;
            if ((int ) rec_buf.integer.C_12.len + move_len > 8096)
                {
                move_len = 8096 - (int ) rec_buf.integer.C_12.len;
                }
            g10mv10("VAK10 " , 34 , mblock_info[sql__setrang(ix , 1 , 4) + -1].size , sizeof(tgg00_Rec ) , sql__nil(mblock_info[sql__setrang(ix , 1 , 4) + -1].addr , "vak10.p" , 23625) , part_pos , rec_buf.integer.C_11.buf , (int ) rec_buf.integer.C_12.len + 1 , move_len , &(*b_err));
            rec_buf.integer.C_12.len = (short ) sql__setrang((short ) ((int ) rec_buf.integer.C_12.len + move_len) , -32768 , 32767);
            part_pos = part_pos + move_len;
            if ((int ) *b_err == 0)
                {
                if ((int ) rec_buf.integer.C_12.len == 8096 || ix == part_cnt && part_pos > mblock_info[sql__setrang(ix , 1 , 4) + -1].len)
                    {
                    if (_ptoc_MEMCMP(unsigned char , 2, (*mblock_rec).mbr_entrytype , "\0\030") == 0 ? 1 : 0)
                        {
                        ak10send_ins_upd(&(*acv) , (struct tak_systembuffer *) (int *) sql__nil(rec_ptr , "vak10.p" , 23638) , (unsigned char ) sql__setrang(m_type , 0 , 95) , &(*b_err));
                        }
                    else
                        {
                        if ((char ) sql__setrang(do_add , 0 , 1))
                            {
                            b07cadd_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , sql__nil(rec_ptr , "vak10.p" , 23644));
                            }
                        else
                            {
                            b07crepl_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , sql__nil(rec_ptr , "vak10.p" , 23647));
                            }
                        *b_err = (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                        (*acv).a_transinf.tri_trans.trError_gg00 = 0;
                        }
                    rec_buf.integer.C_12.len = (short ) sql__setrang((short ) ((int ) (*(struct tak_systembuffer *) sql__nil(rec_ptr , "vak10.p" , 23653)).integer.C_50.syskey.skeylen + 8) , -32768 , 32767);
                    cnt = cnt + 1;
                    rec_buf.integer.C_11.buf[sql__setrang((int ) rec_buf.integer.C_12.len , 1 , 8192) + -1] = (unsigned char ) sql__setrang((unsigned char ) cnt , 0 , 255);
                    }
                }
            }
        while (!(part_pos > mblock_info[sql__setrang(ix , 1 , 4) + -1].len || (int ) *b_err != 0));
        ix = ix + 1;
        }
    if ((int ) *b_err != 0 && ((ptoc_Var229 = (int ) (*mblock_rec).mbr_entrytype[1]) == 193 || ptoc_Var229 == 149))
        {
        sysk = (*(struct tak_systembuffer *) sql__nil(mblock_ptr , "vak10.p" , 23669)).integer.C_50.syskey;
        b07cdel_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , &sysk);
        sysk.skeylen = (short ) sql__setrang((short ) ((int ) sysk.skeylen + 1) , -32768 , 32767);
        ptoc_Var229 = cnt;
        if (1 <= ptoc_Var229)
            {
            ix = 1;
            do
                {
                sysk.integer.C_8.all[sql__setrang((int ) sysk.skeylen , 1 , 140) + -1] = (unsigned char ) sql__setrang((unsigned char ) ix , 0 , 255);
                b07cdel_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , &sysk);
                if (ix == ptoc_Var229)
                    {
                    break;
                    }
                ix += 1;
                }
            while (!0);
            }
        }
    cccprint("<ak10store_mess_");
    }



int ak10store_syskey(acv, syskey)
tak_all_command_glob *acv;
tgg_sysinfokey *syskey;
    {
    cccprint("AK10  ak10store_");
    (*(struct tak_systembuffer *) sql__nil((*acv).a_error_key_ptr , "vak10.p" , 23694)).integer.C_50.syskey.skeylen = (short ) sql__setrang((*syskey).skeylen , -32768 , 32767);
    s10mv2(sizeof(tgg00_SysInfoKey ) , sizeof(tgg00_SysInfoKey ) , &(*syskey) , 9 , &(*(struct tak_systembuffer *) sql__nil((*acv).a_error_key_ptr , "vak10.p" , 23697)).integer.C_50.syskey , 9 , (int ) (*syskey).skeylen);
    cccprint("<ak10store_syske");
    }



int ak10table_cache_delete(acv, tableid, flush_tmp)
tak_all_command_glob *acv;
tgg_surrogate tableid;
char flush_tmp;
    {
    char nothing_found;
    tsp_lcomp_result res;
    int loop_cnt;
    tak_cache_dir_pointer p;
    tgg_sysinfokey syskey;
    cccprint("AK10  ak10table_");
    _ptoc_ASSIGN(unsigned char , 8, syskey.integer.C_2.stableid, tableid)
    loop_cnt = 0;
    do
        {
        loop_cnt = loop_cnt + 1;
        do
            {
            nothing_found = 1;
            p = (*acv).a_sysdir.ci_dir[sql__setrang(loop_cnt , 1 , 195) + -1];
            while (p.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
                if ((*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 23727)).integer.C_1.cd_pos + (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 23727)).integer.C_1.cd_check != 0)
                    {
                    ak10cache_error(&(*acv) , "a10table_cache_delete   " , p);
                    p.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
                    loop_cnt = 195;
                    }
                else
                    {
                    s30cmp(&syskey , 9 , 8 , &(*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 23737)).integer.C_1.cd_syskey , 9 , 8 , &res);
                    if ((int ) res == 1 && (0 || ((*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 23739)).integer.C_1.cd_state[0] & 2) == 0))
                        {
                        t01buf1(0 , &(*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 23742)).integer.C_1.cd_syskey , 9 , (int ) (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 23743)).integer.C_1.cd_syskey.skeylen + 8);
                        if ((char ) sql__setrang(flush_tmp , 0 , 1))
                            {
                            ak10temp_add(&(*acv) , p , 0);
                            }
                        ak10block_into_freelist(&(*acv) , p , 1 , 1);
                        nothing_found = 0;
                        p.integer.C_1.cdir_ptr = (struct tak_cache_dir_entry *) (int *) 0;
                        }
                    else
                        {
                        if ((int ) res == 0)
                            {
                            p = (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 23756)).integer.C_1.cd_less;
                            }
                        else
                            {
                            p = (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 23758)).integer.C_1.cd_greater;
                            }
                        }
                    }
            }
        while (!(char ) sql__setrang(nothing_found , 0 , 1));
        }
    while (!(loop_cnt >= 195));
    ak10verify_cache(&(*acv));
    cccprint("<ak10table_cache");
    }

static
  unsigned char ptoc_Var230[1] = 
  {
  (unsigned char)'\020'
  };
static
  unsigned char ptoc_Var231[1] = 
  {
  (unsigned char)'\b'
  };


int ak10temp_add(acv, ptr, is_rollback)
tak_all_command_glob *acv;
tak_cache_dir_pointer ptr;
char is_rollback;
    {
    tgg_basis_error b_err;
    char is_object;
    int aux_len;
    tsp_int4 rest_len;
    tsp_int4 full_len;
    struct tak_cache_dir_entry *ptoc_Var232;
    struct tak_systembuffer *ptoc_Var233;
    unsigned char ptoc_Var234[1];
    unsigned char ptoc_Var235[1];
    unsigned char ptoc_Var236[1];
    unsigned char ptoc_Var237[1];
    unsigned char ptoc_Var238[1];
    unsigned char ptoc_Var239[1];
    tak_sysdirectory *ptoc_Var240;
    tgg00_FileId *ptoc_Var241;
    cccprint("AK10  ak10temp_a");
    ptoc_Var232 = (struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 23795);
    xtd_directory(0 , ptr);
    if (_ptoc_MEMCMP(unsigned char , 2, ptoc_Var232->integer.C_1.cd_syskey.integer.C_2.sentrytyp , "\0\200") > 0 ? 1 : 0)
        {
        b_err = 0;
        rest_len = 0;
        is_object = 0;
        ptoc_Var233 = &(*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 23804)).integer.C_2.cd_sysinfo;
        switch ((int ) ptoc_Var232->integer.C_1.cd_syskey.integer.C_2.sentrytyp[1])
            {
            case 130 :
            case 129 :
            case 132 :
            case 178 :
            case 190 :
                full_len = ptoc_Var233->integer.C_23.sparsinfo.integer.C_1.p_fullen;
                if (full_len > 8096)
                    {
                    ptoc_Var233->integer.C_0.b_sl = 8096;
                    rest_len = full_len + -8096;
                    }
                else
                    {
                    ptoc_Var233->integer.C_0.b_sl = (short ) sql__setrang((short ) full_len , -32768 , 32767);
                    }
                break;
            case 192 :
                is_object = 1;
                break;
            case 177 :
                is_object = 1;
                break;
            case 193 :
            case 149 :
                is_object = 1;
                break;
            default :
                break;
            }
        if (1 && (ptoc_Var232->integer.C_1.cd_state[0] & 32) != 0)
            {
            if (!(char ) sql__setrang(is_object , 0 , 1))
                {
                b07crepl_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , &ptoc_Var232->integer.C_2.cd_sysinfo);
                if ((int ) (*acv).a_transinf.tri_trans.trError_gg00 == 1200)
                    {
                    b_err = 0;
                    }
                else
                    {
                    b_err = (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                    }
                (*acv).a_transinf.tri_trans.trError_gg00 = 0;
                if (rest_len > 0)
                    {
                    ak10store_record_suffix(&(*acv) , &(*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 23848)).integer.C_2.cd_sysinfo , 0 , rest_len , &b_err);
                    }
                }
            else
                {
                switch ((int ) ptoc_Var232->integer.C_1.cd_syskey.integer.C_2.sentrytyp[1])
                    {
                    case 193 :
                    case 149 :
                        ak10store_mess_block(&(*acv) , &(*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 23856)).integer.C_2.cd_sysinfo.integer.C_62.smessblock , 0 , &b_err);
                        break;
                    case 177 :
                        ak10add_base_record(&(*acv) , (struct tak_systembuffer *) &ptoc_Var232->integer.C_2.cd_sysinfo , 0 , 77 , &b_err);
                        break;
                    case 192 :
                        ak10store_colnames_record(&(*acv) , (struct tak_systembuffer *) &ptoc_Var232->integer.C_2.cd_sysinfo , 77 , &b_err);
                        break;
                    default :
                        (unsigned char ) sql__caseerr("vak10.p" , 23853);
                        break;
                    }
                }
            }
        else
            {
            if (1 && (ptoc_Var232->integer.C_1.cd_state[0] & 8) != 0)
                {
                _ptoc_MOVE(unsigned char , 1, ptoc_Var232->integer.C_1.cd_state, *(unsigned char (*)[1]) sql__setsub(ptoc_Var234 , sql__psetadd(ptoc_Var236 , ptoc_Var232->integer.C_1.cd_state , ptoc_Var230 , sizeof(unsigned char [1])) , ptoc_Var231 , sizeof(unsigned char [1])));
                if (!(char ) sql__setrang(is_object , 0 , 1))
                    {
                    b07cadd_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , &ptoc_Var232->integer.C_2.cd_sysinfo);
                    if ((int ) (*acv).a_transinf.tri_trans.trError_gg00 == 530)
                        {
                        b_err = 540;
                        ak10store_syskey(&(*acv) , &ptoc_Var232->integer.C_1.cd_syskey);
                        }
                    else
                        {
                        b_err = (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                        }
                    (*acv).a_transinf.tri_trans.trError_gg00 = 0;
                    if (rest_len > 0)
                        {
                        ak10store_record_suffix(&(*acv) , &(*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 23892)).integer.C_2.cd_sysinfo , 1 , rest_len , &b_err);
                        }
                    }
                else
                    {
                    switch ((int ) ptoc_Var232->integer.C_1.cd_syskey.integer.C_2.sentrytyp[1])
                        {
                        case 193 :
                        case 149 :
                            ak10store_mess_block(&(*acv) , &(*(struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 23900)).integer.C_2.cd_sysinfo.integer.C_62.smessblock , 1 , &b_err);
                            break;
                        case 177 :
                            ak10add_base_record(&(*acv) , (struct tak_systembuffer *) &ptoc_Var232->integer.C_2.cd_sysinfo , 0 , 35 , &b_err);
                            break;
                        case 192 :
                            ak10store_colnames_record(&(*acv) , (struct tak_systembuffer *) &ptoc_Var232->integer.C_2.cd_sysinfo , 35 , &b_err);
                            break;
                        default :
                            (unsigned char ) sql__caseerr("vak10.p" , 23897);
                            break;
                        }
                    }
                if ((int ) b_err == 0 && ptoc_Var232->integer.C_1.cd_create_cmd_no == (*acv).a_command_count)
                    {
                    if ((*acv).a_sysdir.ci_cmd_rollback_tree.integer.C_1.fileRoot_gg00 == 2147483647)
                        {
                        ptoc_Var240 = &(*acv).a_sysdir;
                        ptoc_Var241 = &ptoc_Var240->ci_cmd_rollback_tree;
                        ptoc_Var240->ci_cmd_rollback_tree = (*acv).a_pars_curr;
                        ptoc_Var241->integer.C_1.fileRoot_gg00 = 2147483647;
                        ptoc_Var241->integer.C_8.fileTfnTemp_gg00 = 31;
                        b01treset_file(&(*acv).a_transinf.tri_trans , &(*acv).a_sysdir.ci_cmd_rollback_tree);
                        }
                    if ((int ) (*acv).a_transinf.tri_trans.trError_gg00 == 0)
                        {
                        aux_len = (int ) ptoc_Var232->integer.C_2.cd_sysinfo.integer.C_0.b_sl;
                        ptoc_Var232->integer.C_2.cd_sysinfo.integer.C_0.b_sl = (short ) sql__setrang((short ) ((int ) ptoc_Var232->integer.C_2.cd_sysinfo.integer.C_0.b_kl + 8) , -32768 , 32767);
                        b07cadd_record(&(*acv).a_transinf.tri_trans , &(*acv).a_sysdir.ci_cmd_rollback_tree , &ptoc_Var232->integer.C_2.cd_sysinfo);
                        ptoc_Var232->integer.C_2.cd_sysinfo.integer.C_0.b_sl = (short ) sql__setrang((short ) aux_len , -32768 , 32767);
                        if ((int ) (*acv).a_transinf.tri_trans.trError_gg00 == 530)
                            {
                            (*acv).a_transinf.tri_trans.trError_gg00 = 0;
                            }
                        }
                    b_err = (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
                    (*acv).a_transinf.tri_trans.trError_gg00 = 0;
                    }
                }
            }
        if ((int ) b_err != 0 && !(char ) sql__setrang(is_rollback , 0 , 1))
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
            }
        }
    cccprint("<ak10temp_add   ");
    }



int ak10synonym_record(rec, syspoint, e)
tgg_rec_buf *rec;
tak_sysbufferaddress syspoint;
tgg_basis_error *e;
    {
    int pos;
    tak_synonymrecord *ptoc_Var242;
    cccprint("AK10  ak10synony");
    pos = sizeof(tak_synonymrecord ) - sizeof(unsigned char [64]) + 1;
    ptoc_Var242 = &(*(struct tak_systembuffer *) sql__nil(syspoint , "vak10.p" , 23978)).integer.C_34.ssynonym;
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var242->syn_tablen, a01_il_b_identifier)
    g10mv7("VAK10 " , 35 , sizeof(tsp00_Buf ) , sizeof(unsigned char [64]) , (*rec).integer.C_11.buf , pos + 1 , ptoc_Var242->syn_tablen , 1 , sql__ord((int ) (*rec).integer.C_11.buf[sql__setrang(pos , 1 , 8192) + -1]) , &(*e));
    cccprint("<ak10synonym_rec");
    }



int ak10tdcheck_order(p)
tak_cache_dir_pointer p;
    {
    tsp_lcomp_result res;
    struct tak_cache_dir_entry *ptoc_Var243;
    cccprint("AK10  ak10tdchec");
    if (p.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        ptoc_Var243 = (struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24003);
        if (ptoc_Var243->integer.C_1.cd_lru_next.integer.C_1.cdir_ptr == (struct tak_cache_dir_entry *) (int *) 0 || ptoc_Var243->integer.C_1.cd_lru_prev.integer.C_1.cdir_ptr == (struct tak_cache_dir_entry *) (int *) 0)
            {
            t01str30(39 , "dir entry not in lru          ");
            xtd_directory(39 , p);
            g01abort(51110 , "AK CACHE" , "NOT IN LRU              " , 0);
            }
        if (ptoc_Var243->integer.C_1.cd_less.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
            {
            s30cmp(&(*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var243->integer.C_1.cd_less.integer.C_1.cdir_ptr , "vak10.p" , 24018)).integer.C_1.cd_syskey , 9 , (int ) (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var243->integer.C_1.cd_less.integer.C_1.cdir_ptr , "vak10.p" , 24019)).integer.C_1.cd_syskey.skeylen , &ptoc_Var243->integer.C_1.cd_syskey , 9 , (int ) ptoc_Var243->integer.C_1.cd_syskey.skeylen , &res);
            if ((int ) res != 0)
                {
                t01str30(39 , "ordering destroyed : less     ");
                xtd_directory(39 , p);
                xtd_directory(39 , ptoc_Var243->integer.C_1.cd_less);
                g01abort(51111 , "AK CACHE" , "WRONG LESS PTR          " , 0);
                }
            }
        if (ptoc_Var243->integer.C_1.cd_greater.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
            {
            s30cmp(&(*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var243->integer.C_1.cd_greater.integer.C_1.cdir_ptr , "vak10.p" , 24036)).integer.C_1.cd_syskey , 9 , (int ) (*(struct tak_cache_dir_entry *) sql__nil(ptoc_Var243->integer.C_1.cd_greater.integer.C_1.cdir_ptr , "vak10.p" , 24037)).integer.C_1.cd_syskey.skeylen , &ptoc_Var243->integer.C_1.cd_syskey , 9 , (int ) ptoc_Var243->integer.C_1.cd_syskey.skeylen , &res);
            if ((int ) res != 2)
                {
                t01str30(39 , "ordering destroyed : greater  ");
                xtd_directory(39 , p);
                xtd_directory(39 , ptoc_Var243->integer.C_1.cd_greater);
                g01abort(51112 , "AK CACHE" , "WRONG GREATER PTR       " , 0);
                }
            }
        ak10tdcheck_order(ptoc_Var243->integer.C_1.cd_less);
        ak10tdcheck_order(ptoc_Var243->integer.C_1.cd_greater);
        }
    cccprint("<ak10tdcheck_ord");
    }



int ak10writeln(p)
tak_cache_dir_pointer p;
    {
    int ix;
    int max;
    int ln_len;
    tsp_c40 msg;
    struct ptoc_Type216
      {
      union  
        {
        struct  
          {
          tsp_int4 i4;
          } C_true;
        struct  
          {
          tsp_c4 c4;
          } C_false;
        } boolean;
      } i4c4;
    struct tak_cache_dir_entry *ptoc_Var244;
    int ptoc_Var245;
    cccprint("AK10  ak10writel");
    ptoc_Var244 = (struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24084);
    ptoc_Var245 = sizeof(tsp00_C40 );
    if (1 <= ptoc_Var245)
        {
        ix = 1;
        do
            {
            msg[sql__setrang(ix , 1 , 40) + -1] = (unsigned char)' ';
            if (ix == ptoc_Var245)
                {
                break;
                }
            ix += 1;
            }
        while (!0);
        }
    ln_len = 0;
    if ((int ) ptoc_Var244->integer.C_1.cd_syskey.skeylen > sizeof(tsp00_C40 ))
        {
        max = sizeof(tsp00_C40 );
        }
    else
        {
        max = (int ) ptoc_Var244->integer.C_1.cd_syskey.skeylen;
        }
    ptoc_Var245 = max;
    if (1 <= ptoc_Var245)
        {
        ix = 1;
        do
            {
            if (ix < sizeof(tsp00_C40 ) / 2)
                {
                g17hexto_line((unsigned char ) sql__setrang(ptoc_Var244->integer.C_1.cd_syskey.integer.C_8.all[sql__setrang(ix , 1 , 140) + -1] , 0 , 255) , &ln_len , msg);
                }
            if (ix == ptoc_Var245)
                {
                break;
                }
            ix += 1;
            }
        while (!0);
        }
    g01optextmsg(2 , 3 , 51105 , "AK CACHE" , msg);
    _ptoc_ASSIGN(unsigned char , 40, msg, "cd_pos :                                ")
    i4c4.boolean.C_true.i4 = ptoc_Var244->integer.C_1.cd_pos;
    ln_len = 9;
    ptoc_Var245 = sizeof(tsp00_C4 );
    if (1 <= ptoc_Var245)
        {
        ix = 1;
        do
            {
            g17hexto_line((unsigned char ) sql__setrang(i4c4.boolean.C_false.c4[sql__setrang(ix , 1 , 4) + -1] , 0 , 255) , &ln_len , msg);
            if (ix == ptoc_Var245)
                {
                break;
                }
            ix += 1;
            }
        while (!0);
        }
    g01optextmsg(2 , 3 , 51105 , "AK CACHE" , msg);
    _ptoc_ASSIGN(unsigned char , 40, msg, "cd_less :                               ")
    i4c4.boolean.C_true.i4 = ptoc_Var244->integer.C_1.cd_less.integer.C_5.i;
    ln_len = 10;
    ptoc_Var245 = sizeof(tsp00_C4 );
    if (1 <= ptoc_Var245)
        {
        ix = 1;
        do
            {
            g17hexto_line((unsigned char ) sql__setrang(i4c4.boolean.C_false.c4[sql__setrang(ix , 1 , 4) + -1] , 0 , 255) , &ln_len , msg);
            if (ix == ptoc_Var245)
                {
                break;
                }
            ix += 1;
            }
        while (!0);
        }
    g01optextmsg(2 , 3 , 51105 , "AK CACHE" , msg);
    _ptoc_ASSIGN(unsigned char , 40, msg, "cd_greater :                            ")
    i4c4.boolean.C_true.i4 = ptoc_Var244->integer.C_1.cd_greater.integer.C_5.i;
    ln_len = 13;
    ptoc_Var245 = sizeof(tsp00_C4 );
    if (1 <= ptoc_Var245)
        {
        ix = 1;
        do
            {
            g17hexto_line((unsigned char ) sql__setrang(i4c4.boolean.C_false.c4[sql__setrang(ix , 1 , 4) + -1] , 0 , 255) , &ln_len , msg);
            if (ix == ptoc_Var245)
                {
                break;
                }
            ix += 1;
            }
        while (!0);
        }
    g01optextmsg(2 , 3 , 51105 , "AK CACHE" , msg);
    if (ptoc_Var244->integer.C_1.cd_pos + ptoc_Var244->integer.C_1.cd_check != 0)
        {
        g01optextmsg(2 , 3 , 51105 , "AK CACHE" , "cd_check destroyed!!!                   ");
        }
    g01optextmsg(2 , 3 , 51105 , "AK CACHE" , "----------------------------------------");
    cccprint("<ak10writeln    ");
    }



int a10verify_cache(acv)
tak_all_command_glob *acv;
    {
    cccprint("AK10  a10verify_");
    ak10verify_cache(&(*acv));
    cccprint("<a10verify_cache");
    }



int ak10verify_cache(acv)
tak_all_command_glob *acv;
    {
    char exit_loop;
    char found;
    int ix;
    tak_cache_dir_pointer lru;
    tak_cache_dir_pointer curr;
    tak_cache_dir_pointer next;
    tak_cache_dir_pointer prev;
    tak_cache_dir_pointer err_p;
    tak_cache_dir_pointer p;
    tsp_c24 descr;
    tak_basecolinfo_ptr bcp;
    tgg_mess_block_ptr mblock_ptr;
    struct ptoc_Type217
      {
      union  
        {
        struct  
          {
          tak_ap_max_tree_ptr ap;
          } C_true;
        struct  
          {
          tak_cache_dir_e_ptr cdir;
          } C_false;
        } boolean;
      } ap_ptr;
    struct ptoc_Type218
      {
      union  
        {
        struct  
          {
          tsp_bufaddr buf_ptr;
          } C_1;
        struct  
          {
          tgg_qual_buf_ptr qual_ptr;
          } C_2;
        struct  
          {
          tak_cache_dir_pointer cd_ptr;
          } C_3;
        struct  
          {
          tgg_datapart_ptr data_ptr;
          } C_4;
        struct  
          {
          tgg_stack_list_ptr st_ptr;
          } C_5;
        struct  
          {
          tsp00_MoveObjPtr strat_ptr;
          } C_6;
        } integer;
      } pa[4];
    tak_cache_dir_pointer ptr_stack[500];
    int stack_top;
    cccprint("AK10  ak10verify");
    cccprint("<ak10verify_cach");
    }



int td_tree(p)
tak_cache_dir_pointer p;
    {
    struct tak_cache_dir_entry *ptoc_Var246;
    struct tak_cache_dir_entry *ptoc_Var247;
    cccprint("AK10  td_tree   ");
    if (p.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        t01str30(0 , "========== node ==============");
        xtd_directory(0 , p);
        if ((*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24226)).integer.C_1.cd_less.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
            {
            ptoc_Var246 = (struct tak_cache_dir_entry *) sql__nil((*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24228)).integer.C_1.cd_less.integer.C_1.cdir_ptr , "vak10.p" , 24228);
            t01str30(0 , "========== left ==============");
            xtd_directory(0 , (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24231)).integer.C_1.cd_less);
            }
        if ((*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24235)).integer.C_1.cd_greater.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
            {
            ptoc_Var247 = (struct tak_cache_dir_entry *) sql__nil((*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24237)).integer.C_1.cd_greater.integer.C_1.cdir_ptr , "vak10.p" , 24237);
            t01str30(0 , "========== right =============");
            xtd_directory(0 , (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24240)).integer.C_1.cd_greater);
            }
        td_tree((*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24244)).integer.C_1.cd_less);
        td_tree((*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24245)).integer.C_1.cd_greater);
        }
    cccprint("<td_tree        ");
    }



int ak10check_tree(p, hight, abort)
tak_cache_dir_pointer p;
int *hight;
char *abort;
    {
    int h_left;
    int h_right;
    tsp_c24 descr;
    struct tak_cache_dir_entry *ptoc_Var248;
    int ptoc_Var249;
    cccprint("AK10  ak10check_");
    ptoc_Var248 = (struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24264);
    if (ptoc_Var248->integer.C_1.cd_lru_next.integer.C_1.cdir_ptr == (struct tak_cache_dir_entry *) (int *) 0 || ptoc_Var248->integer.C_1.cd_lru_prev.integer.C_1.cdir_ptr == (struct tak_cache_dir_entry *) (int *) 0)
        {
        g01abort(51113 , "AK CACHE" , descr , 0);
        }
    if (ptoc_Var248->integer.C_1.cd_less.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        ak10check_tree(ptoc_Var248->integer.C_1.cd_less , &h_left , &(*abort));
        }
    else
        {
        h_left = 0;
        }
    if (ptoc_Var248->integer.C_1.cd_greater.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        ak10check_tree(ptoc_Var248->integer.C_1.cd_greater , &h_right , &(*abort));
        }
    else
        {
        h_right = 0;
        }
    if ((ptoc_Var249 = h_left - h_right , ptoc_Var249 >= 0 ? ptoc_Var249 : -ptoc_Var249) > 1)
        {
        *abort = 1;
        }
    if (h_left > h_right)
        {
        *hight = h_left + 1;
        }
    else
        {
        *hight = h_right + 1;
        }
    cccprint("<ak10check_tree ");
    }



int ak10td_dirlist(acv, layer)
tak_all_command_glob *acv;
tgg_debug layer;
    {
    int i;
    cccprint("AK10  ak10td_dir");
    t01str30((unsigned char ) sql__setrang(layer , 0 , 55) , "========== directory =========");
    i = 1;
    do
        {
        if ((*acv).a_sysdir.ci_dir[sql__setrang(i , 1 , 195) + -1].integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
            {
            ak10dirlist((unsigned char ) sql__setrang(layer , 0 , 55) , (*acv).a_sysdir.ci_dir[sql__setrang(i , 1 , 195) + -1]);
            }
        if (i == 195)
            {
            break;
            }
        i += 1;
        }
    while (!0);
    cccprint("<ak10td_dirlist ");
    }



int ak10dirlist(layer, p)
tgg_debug layer;
tak_cache_dir_pointer p;
    {
    cccprint("AK10  ak10dirlis");
    if (p.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        xtd_directory((unsigned char ) sql__setrang(layer , 0 , 55) , p);
        ak10dirlist((unsigned char ) sql__setrang(layer , 0 , 55) , (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24335)).integer.C_1.cd_less);
        ak10dirlist((unsigned char ) sql__setrang(layer , 0 , 55) , (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24336)).integer.C_1.cd_greater);
        }
    cccprint("<ak10dirlist    ");
    }



int ak10td_freelist(acv, layer)
tak_all_command_glob *acv;
tgg_debug layer;
    {
    tak_cache_dir_pointer p;
    tsp_int4 free_size;
    struct tak_cache_dir_entry *ptoc_Var250;
    cccprint("AK10  ak10td_fre");
    t01str30((unsigned char ) sql__setrang(layer , 0 , 55) , "========== free list =========");
    free_size = 0;
    p = (*(struct tak_cache_dir_entry *) sql__nil((*acv).a_sysdir.ci_cache_dir_header.integer.C_1.cdir_ptr , "vak10.p" , 24357)).integer.C_2.cd_next_free;
    while (p.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        ptoc_Var250 = (struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24359);
        t01p2int4((unsigned char ) sql__setrang(layer , 0 , 55) , "free bl pos " , ptoc_Var250->integer.C_2.cd_block_pos , "free bl bno " , (int ) ptoc_Var250->integer.C_1.cd_buf_no);
        t01p2int4((unsigned char ) sql__setrang(layer , 0 , 55) , "            " , 0 , "free bl len " , ptoc_Var250->integer.C_1.cd_syslen);
        free_size = free_size + ptoc_Var250->integer.C_1.cd_syslen;
        p = ptoc_Var250->integer.C_2.cd_next_free;
        }
    t01int4((unsigned char ) sql__setrang(layer , 0 , 55) , "free space  " , free_size);
    cccprint("<ak10td_freelist");
    }



int xtd_directory(layer, ptr)
tgg_debug layer;
tak_cache_dir_pointer ptr;
    {
    struct tak_cache_dir_entry *ptoc_Var251;
    cccprint("AK10  xtd_direct");
    ptoc_Var251 = (struct tak_cache_dir_entry *) sql__nil(ptr.integer.C_1.cdir_ptr , "vak10.p" , 24383);
    t01buf1((unsigned char ) sql__setrang(layer , 0 , 55) , &ptoc_Var251->integer.C_1.cd_syskey , 9 , (int ) ptoc_Var251->integer.C_1.cd_syskey.skeylen + 8);
    t01p2int4((unsigned char ) sql__setrang(layer , 0 , 55) , "block pos   " , ptoc_Var251->integer.C_1.cd_pos , "block len   " , ptoc_Var251->integer.C_1.cd_syslen);
    t01int4((unsigned char ) sql__setrang(layer , 0 , 55) , "b_sl        " , (int ) ptoc_Var251->integer.C_2.cd_sysinfo.integer.C_0.b_sl);
    t01int4((unsigned char ) sql__setrang(layer , 0 , 55) , "buf no      " , (int ) ptoc_Var251->integer.C_1.cd_buf_no);
    t01int4((unsigned char ) sql__setrang(layer , 0 , 55) , "balance     " , sql__ord((int ) ptoc_Var251->integer.C_1.cd_balance));
    if (ptoc_Var251->integer.C_1.cd_fix_next.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        t01int4((unsigned char ) sql__setrang(layer , 0 , 55) , "fix         " , 1);
        }
    else
        {
        t01int4((unsigned char ) sql__setrang(layer , 0 , 55) , "not fix     " , 0);
        }
    if (1 && (ptoc_Var251->integer.C_1.cd_state[0] & 16) != 0)
        {
        t01int4((unsigned char ) sql__setrang(layer , 0 , 55) , "dwritten    " , 1);
        }
    if (1 && (ptoc_Var251->integer.C_1.cd_state[0] & 8) != 0)
        {
        t01int4((unsigned char ) sql__setrang(layer , 0 , 55) , "dadd_rec    " , 1);
        }
    if (1 && (ptoc_Var251->integer.C_1.cd_state[0] & 32) != 0)
        {
        t01int4((unsigned char ) sql__setrang(layer , 0 , 55) , "dmodified   " , 1);
        }
    if (1 && (ptoc_Var251->integer.C_1.cd_state[0] & 2) != 0)
        {
        t01int4((unsigned char ) sql__setrang(layer , 0 , 55) , "dlocked     " , 1);
        }
    t01int4((unsigned char ) sql__setrang(layer , 0 , 55) , "block addr i" , ptr.integer.C_5.i);
    t01addr((unsigned char ) sql__setrang(layer , 0 , 55) , "block addr  " , ptr.integer.C_2.buf_ptr);
    t01addr((unsigned char ) sql__setrang(layer , 0 , 55) , "less        " , ptoc_Var251->integer.C_1.cd_less.integer.C_2.buf_ptr);
    t01addr((unsigned char ) sql__setrang(layer , 0 , 55) , "greater     " , ptoc_Var251->integer.C_1.cd_greater.integer.C_2.buf_ptr);
    if (ptoc_Var251->integer.C_1.cd_pos + ptoc_Var251->integer.C_1.cd_check != 0)
        {
        t01int4((unsigned char ) sql__setrang(layer , 0 , 55) , "wrong check " , 1);
        }
    cccprint("<xtd_directory  ");
    }



int ak10count(p)
tak_cache_dir_pointer p;
    {
    int _ak10count;
    int cnt;
    cccprint("AK10  ak10count ");
    if (p.integer.C_1.cdir_ptr == (struct tak_cache_dir_entry *) (int *) 0)
        {
        cnt = 0;
        }
    else
        {
        cnt = ak10count((*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24444)).integer.C_1.cd_less) + ak10count((*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24445)).integer.C_1.cd_greater) + 1;
        }
    _ak10count = cnt;
    cccprint("<ak10count      ");
    return(_ak10count);
    }

typedef
  unsigned char tsp_c8_ak10dump_write[8];


int ak10dump_write(dump_context, dump_id)
tak10_dump_context *dump_context;
int dump_id;
    {
    tsp_int_map_c2 i2c2;
    int pos;
    tsp_c8 dmp_label;
    tsp_c8_ak10dump_write *label_ptr;
    tsp_errtext errtext;
    tsp_moveobj_ptr mobj_ptr;
    tak10_dump_context *ptoc_Var252;
    cccprint("AK10  ak10dump_w");
    switch (dump_id)
        {
        case 1801 :
            _ptoc_ASSIGN(unsigned char , 8, dmp_label, "A10CACHE")
            break;
        case 1802 :
            _ptoc_ASSIGN(unsigned char , 8, dmp_label, "A10FREE ")
            break;
        default :
            break;
        }
    ptoc_Var252 = &*dump_context;
    if ((int ) ptoc_Var252->dmp_b_err == 0)
        {
        pos = ptoc_Var252->dmp_header_pos;
        label_ptr = (tsp_c8_ak10dump_write *) &ptoc_Var252->dmp_buf[sql__setrang(pos , 1 , 8192) + -1];
        _ptoc_ASSIGN(unsigned char , 8, *(tsp00_C8 *) sql__nil(label_ptr , "vak10.p" , 24484), dmp_label)
        pos = pos + sizeof(tsp00_C8 );
        i2c2.boolean.C_true.map_int = (short ) sql__setrang((short ) dump_id , -32768 , 32767);
        ptoc_Var252->dmp_buf[sql__setrang(pos , 1 , 8192) + -1] = (unsigned char ) sql__setrang(i2c2.boolean.C_false.map_c2[0] , 0 , 255);
        ptoc_Var252->dmp_buf[sql__setrang(pos + 1 , 1 , 8192) + -1] = (unsigned char ) sql__setrang(i2c2.boolean.C_false.map_c2[1] , 0 , 255);
        pos = pos + 2;
        i2c2.boolean.C_true.map_int = (short ) sql__setrang((short ) (ptoc_Var252->dmp_pos + -1) , -32768 , 32767);
        ptoc_Var252->dmp_buf[sql__setrang(pos , 1 , 8192) + -1] = (unsigned char ) sql__setrang(i2c2.boolean.C_false.map_c2[0] , 0 , 255);
        ptoc_Var252->dmp_buf[sql__setrang(pos + 1 , 1 , 8192) + -1] = (unsigned char ) sql__setrang(i2c2.boolean.C_false.map_c2[1] , 0 , 255);
        ptoc_Var252->dmp_pos = pos + 2;
        mobj_ptr = (tsp00_MoveObj *) ptoc_Var252->dmp_buf;
        t01moveobj(0 , sql__nil(mobj_ptr , "vak10.p" , 24495) , 1 , 8192);
        g01new_dump_page(&ptoc_Var252->dmp_file , ptoc_Var252->dmp_buf , &ptoc_Var252->dmp_pno , &ptoc_Var252->dmp_pos , &ptoc_Var252->dmp_error , errtext);
        ptoc_Var252->dmp_header_pos = ptoc_Var252->dmp_pos;
        if ((int ) ptoc_Var252->dmp_error != 0)
            {
            ptoc_Var252->dmp_b_err = 3700;
            }
        }
    cccprint("<ak10dump_write ");
    }



char a10akdump(acv)
tak_all_command_glob *acv;
    {
    char _a10akdump;
    cccprint("AK10  a10akdump ");
    ak10dump(&(*acv) , 0);
    _a10akdump = 1;
    cccprint("<a10akdump      ");
    return(_a10akdump);
    }



int a10dump(acv)
tak_all_command_glob *acv;
    {
    cccprint("AK10  a10dump   ");
    ak10dump(&(*acv) , 1);
    cccprint("<a10dump        ");
    }



int ak10dump(acv, synchronize)
tak_all_command_glob *acv;
char synchronize;
    {
    int cnt;
    int buf_cnt;
    int bufpos;
    int movelen;
    tsp_errtext errtext;
    tak10_dump_context dump_context;
    struct ptoc_Type219
      {
      union  
        {
        struct  
          {
          tsp_vfilename fn;
          } C_true;
        struct  
          {
          tsp_c16 c16;
          } C_false;
        } boolean;
      } dump_fn;
    struct ptoc_Type220
      {
      union  
        {
        struct  
          {
          tsp_bufaddr bufaddr;
          } C_true;
        struct  
          {
          tak10cache_header_ptr chptr;
          } C_false;
        } boolean;
      } aux_ptr;
    tsp_bufaddr bufaddr;
    tak10_dump_context *ptoc_Var253;
    cccprint("AK10  ak10dump  ");
    dump_context.dmp_pno = 1;
    dump_context.dmp_pos = 1;
    dump_context.dmp_file.buf_cnt = 0;
    _ptoc_ASSIGN(unsigned char , 64, dump_fn.boolean.C_true.fn, b01blankfilename)
    _ptoc_ASSIGN(unsigned char , 16, dump_fn.boolean.C_false.c16, "AK.dmp          ")
    if ((char ) sql__setrang(synchronize , 0 , 1))
        {
        vbegexcl((*acv).a_transinf.tri_trans.trTaskId_gg00 , g08diagcache);
        }
    ptoc_Var253 = &dump_context;
    vfopen(dump_fn.boolean.C_true.fn , &ptoc_Var253->dmp_file.no , &ptoc_Var253->dmp_error , errtext);
    if ((int ) ptoc_Var253->dmp_error == 0)
        {
        g01new_dump_page(&ptoc_Var253->dmp_file , ptoc_Var253->dmp_buf , &ptoc_Var253->dmp_pno , &ptoc_Var253->dmp_pos , &ptoc_Var253->dmp_error , errtext);
        }
    if ((int ) ptoc_Var253->dmp_error == 0)
        {
        ptoc_Var253->dmp_header_pos = ptoc_Var253->dmp_pos;
        ptoc_Var253->dmp_pos = ptoc_Var253->dmp_pos + 12;
        ptoc_Var253->dmp_b_err = 0;
        ak10dump_write(&dump_context , 1801);
        aux_ptr.boolean.C_true.bufaddr = (*acv).a_sysdir.ci_cache_p;
        while (aux_ptr.boolean.C_true.bufaddr != (tsp00_Buf *) (int *) 0 && (int ) ptoc_Var253->dmp_error == 0)
            {
            bufaddr = aux_ptr.boolean.C_true.bufaddr;
            buf_cnt = (*(tak10cache_header *) sql__nil(aux_ptr.boolean.C_false.chptr , "vak10.p" , 24607)).ch_size;
            cnt = 0;
            do
                {
                cnt = cnt + 1;
                vfwrite(ptoc_Var253->dmp_file.no , (tsp00_VfBuf *) (int *) sql__nil(bufaddr , "vak10.p" , 24611) , &ptoc_Var253->dmp_error , errtext);
                bufaddr = s35add_bufaddr(bufaddr , sizeof(tsp00_Buf ));
                }
            while (!(cnt == buf_cnt || (int ) ptoc_Var253->dmp_error != 0));
            buf_cnt = 4;
            aux_ptr.boolean.C_true.bufaddr = (*(tak10cache_header *) sql__nil(aux_ptr.boolean.C_false.chptr , "vak10.p" , 24617)).ch_next_cache;
            }
        vfclose(ptoc_Var253->dmp_file.no , &ptoc_Var253->dmp_error , errtext);
        }
    if ((*acv).a_cmd_part != (tsp1_part *) (int *) 0)
        {
        dump_fn.boolean.C_false.c16[3] = (unsigned char)'s';
        dump_fn.boolean.C_false.c16[4] = (unsigned char)'t';
        dump_fn.boolean.C_false.c16[5] = (unsigned char)'m';
        vfopen(dump_fn.boolean.C_true.fn , &ptoc_Var253->dmp_file.no , &ptoc_Var253->dmp_error , errtext);
        if ((int ) ptoc_Var253->dmp_error == 0)
            {
            ptoc_Var253->dmp_pos = 1;
            bufpos = 1;
            if ((*acv).a_abap_part != (tsp1_part *) (int *) 0)
                {
                movelen = sql__ord((int ) (*(tsp1_part *) sql__nil((*acv).a_abap_part , "vak10.p" , 24638)).integer.C_1.sp1p_buf[0]);
                g10mv15("VAK10 " , 36 , (*(tsp1_part *) sql__nil((*acv).a_abap_part , "vak10.p" , 24640)).integer.C_2.sp1p_buf_size , sizeof(tsp00_Buf ) , (*(tsp1_part *) sql__nil((*acv).a_abap_part , "vak10.p" , 24641)).integer.C_1.sp1p_buf , 2 , ptoc_Var253->dmp_buf , ptoc_Var253->dmp_pos , movelen , &ptoc_Var253->dmp_b_err);
                ptoc_Var253->dmp_pos = ptoc_Var253->dmp_pos + movelen + 1;
                movelen = sql__ord((int ) (*(tsp1_part *) sql__nil((*acv).a_abap_part , "vak10.p" , 24644)).integer.C_1.sp1p_buf[sql__setrang(movelen + 2 , 1 , 8192000) + -1]);
                ptoc_Var253->dmp_buf[sql__setrang(ptoc_Var253->dmp_pos + -1 , 1 , 8192) + -1] = (unsigned char)',';
                g10mv15("VAK10 " , 37 , (*(tsp1_part *) sql__nil((*acv).a_abap_part , "vak10.p" , 24647)).integer.C_2.sp1p_buf_size , sizeof(tsp00_Buf ) , (*(tsp1_part *) sql__nil((*acv).a_abap_part , "vak10.p" , 24648)).integer.C_1.sp1p_buf , sql__ord((int ) (*(tsp1_part *) sql__nil((*acv).a_abap_part , "vak10.p" , 24649)).integer.C_1.sp1p_buf[0]) + 3 , ptoc_Var253->dmp_buf , ptoc_Var253->dmp_pos , movelen , &ptoc_Var253->dmp_b_err);
                ptoc_Var253->dmp_pos = ptoc_Var253->dmp_pos + movelen + 1;
                ptoc_Var253->dmp_buf[sql__setrang(ptoc_Var253->dmp_pos + -1 , 1 , 8192) + -1] = (unsigned char)',';
                }
            while (bufpos < (*(tsp1_part *) sql__nil((*acv).a_cmd_part , "vak10.p" , 24655)).integer.C_2.sp1p_buf_len && (int ) ptoc_Var253->dmp_error == 0)
                {
                if ((*(tsp1_part *) sql__nil((*acv).a_cmd_part , "vak10.p" , 24658)).integer.C_2.sp1p_buf_len - bufpos + 1 > sizeof(tsp00_Buf ) - ptoc_Var253->dmp_pos + 1)
                    {
                    movelen = sizeof(tsp00_Buf ) - ptoc_Var253->dmp_pos + 1;
                    }
                else
                    {
                    movelen = (*(tsp1_part *) sql__nil((*acv).a_cmd_part , "vak10.p" , 24663)).integer.C_2.sp1p_buf_len - bufpos + 1;
                    }
                g10mv15("VAK10 " , 38 , (*(tsp1_part *) sql__nil((*acv).a_cmd_part , "vak10.p" , 24666)).integer.C_2.sp1p_buf_size , sizeof(tsp00_Buf ) , (*(tsp1_part *) sql__nil((*acv).a_cmd_part , "vak10.p" , 24667)).integer.C_1.sp1p_buf , bufpos , ptoc_Var253->dmp_buf , ptoc_Var253->dmp_pos , movelen , &ptoc_Var253->dmp_b_err);
                ptoc_Var253->dmp_pos = ptoc_Var253->dmp_pos + movelen;
                if (sizeof(tsp00_Buf ) > ptoc_Var253->dmp_pos + -1)
                    {
                    g10fil1("VAK10 " , 39 , sizeof(tsp00_Buf ) , ptoc_Var253->dmp_buf , ptoc_Var253->dmp_pos , sizeof(tsp00_Buf ) - ptoc_Var253->dmp_pos + 1 , (unsigned char)' ' , &ptoc_Var253->dmp_b_err);
                    }
                vfwrite(ptoc_Var253->dmp_file.no , (tsp00_VfBuf *) ptoc_Var253->dmp_buf , &ptoc_Var253->dmp_error , errtext);
                ptoc_Var253->dmp_pos = 1;
                bufpos = bufpos + movelen;
                }
            vfclose(ptoc_Var253->dmp_file.no , &ptoc_Var253->dmp_error , errtext);
            }
        }
    if ((char ) sql__setrang(synchronize , 0 , 1))
        {
        vendexcl((*acv).a_transinf.tri_trans.trTaskId_gg00 , g08diagcache);
        }
    cccprint("<ak10dump       ");
    }



int a10parsinfos(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    int i;
    int ptoc_Var254;
    cccprint("AK10  a10parsinf");
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 24709)).integer.C_4.sp1r_returncode == 0)
        {
        a40get_catalog_table(&(*acv) , &(*a41v));
        }
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 24713)).integer.C_4.sp1r_returncode == 0 && (int ) (*acv).a_ex_kind != 0)
        {
        ptoc_Var254 = sql__ord(128);
        if (ptoc_Var254 <= 195)
            {
            i = ptoc_Var254;
            do
                {
                if ((*acv).a_sysdir.ci_dir[sql__setrang(i , 1 , 195) + -1].integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
                    {
                    ak10cache_parsinfos(&(*acv) , &(*a41v) , (*acv).a_sysdir.ci_dir[sql__setrang(i , 1 , 195) + -1]);
                    }
                if (i == 195)
                    {
                    break;
                    }
                i += 1;
                }
            while (!0);
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak10.p" , 24723)).integer.C_4.sp1r_returncode == 0 && (int ) (*acv).a_ex_kind != 0)
        {
        ak10parsfile_infos(&(*acv) , &(*a41v));
        }
    cccprint("<a10parsinfos   ");
    }



int ak10cache_parsinfos(acv, a41v, p)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
tak_cache_dir_pointer p;
    {
    cccprint("AK10  ak10cache_");
    if (p.integer.C_1.cdir_ptr != (struct tak_cache_dir_entry *) (int *) 0)
        {
        if ((0 || ((*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24744)).integer.C_1.cd_state[0] & 16) == 0) && (0 || ((*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24746)).integer.C_1.cd_state[0] & 32) == 0))
            {
            ak10one_parsinfo(&(*acv) , &(*a41v) , &(*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24749)).integer.C_2.cd_sysinfo , 1);
            }
        ak10cache_parsinfos(&(*acv) , &(*a41v) , (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24751)).integer.C_1.cd_less);
        ak10cache_parsinfos(&(*acv) , &(*a41v) , (*(struct tak_cache_dir_entry *) sql__nil(p.integer.C_1.cdir_ptr , "vak10.p" , 24752)).integer.C_1.cd_greater);
        }
    cccprint("<ak10cache_parsi");
    }



int ak10parsfile_infos(acv, a41v)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
    {
    tgg_basis_error b_err;
    tgg00_FilePos tree_pos;
    tgg_bd_set_result_record set_result;
    tgg_lkey zerokey;
    struct ptoc_Type221
      {
      union  
        {
        struct  
          {
          struct tak_systembuffer sb;
          } C_1;
        struct  
          {
          tsp_buf b;
          } C_2;
        struct  
          {
          tgg_rec_buf_ak10get_colnames_record rb;
          } C_3;
        } integer;
      } b;
    tgg00_BdSetResultRecord *ptoc_Var255;
    cccprint("AK10  ak10parsfi");
    ptoc_Var255 = &set_result;
    ptoc_Var255->bd_key_check_len = 0;
    ptoc_Var255->bd_max_rec_cnt = 1;
    ptoc_Var255->bd_max_fill_len = sizeof(struct ptoc_Type221 );
    ptoc_Var255->bd_next = 0;
    zerokey.integer.C_11.len = 0;
    tree_pos.tpsPno_gg00 = 2147483647;
    do
        {
        b_err = 0;
        b07cnext_record(&(*acv).a_transinf.tri_trans , &(*acv).a_pars_curr , &zerokey , &set_result , &tree_pos , b.integer.C_2.b);
        b_err = (short ) sql__setrang((*acv).a_transinf.tri_trans.trError_gg00 , -32768 , 32767);
        (*acv).a_transinf.tri_trans.trError_gg00 = 0;
        if ((int ) b_err == 0 || (int ) b_err == 1200 || (int ) b_err == 350)
            {
            b_err = 0;
            set_result.bd_next = 1;
            a10expand_rec(&b.integer.C_3.rb , &b_err);
            if ((int ) b_err == 0)
                {
                ak10one_parsinfo(&(*acv) , &(*a41v) , &b.integer.C_1.sb , 0);
                }
            }
        }
    while (!((int ) b_err != 0));
    cccprint("<ak10parsfile_in");
    }



int ak10one_parsinfo(acv, a41v, p, in_cache)
tak_all_command_glob *acv;
tak40_show_glob *a41v;
struct tak_systembuffer *p;
char in_cache;
    {
    int offset;
    tsp_int4 i4;
    tsp00_C12 tabtype;
    struct tak_systembuffer *ptoc_Var256;
    int ptoc_Var257;
    cccprint("AK10  ak10one_pa");
    t01buf(0 , &(*p) , 1 , 40);
    t01int4(0 , "stempid     " , (*p).integer.C_50.syskey.integer.C_4.stempid);
    t01int4(0 , "curr_res_id " , (*acv).a_curr_res_id);
    t01surrogate(0 , "sauthid     " , (*p).integer.C_50.syskey.integer.C_3.sauthid);
    t01surrogate(0 , "curr_user_id" , (*acv).a_curr_user_id);
    t01site(0 , "ssite       " , (*p).integer.C_50.syskey.integer.C_6.ssite);
    t01site(0 , "show_table_s" , "\377\373");
    t01int4(0 , "sshowkind   " , sql__ord((int ) (*p).integer.C_50.syskey.integer.C_11.sshowkind));
    t01int4(0 , "sh_kind     " , sql__ord((int ) (*a41v).a4sh_kind));
    ptoc_Var256 = &*p;
    if (((ptoc_Var257 = (int ) ptoc_Var256->integer.C_50.syskey.integer.C_2.sentrytyp[1]) == 132 || ptoc_Var257 == 177 || ptoc_Var257 == 192) && ptoc_Var256->integer.C_50.syskey.integer.C_4.stempid == (*acv).a_curr_res_id || (int ) ptoc_Var256->integer.C_50.syskey.integer.C_2.sentrytyp[1] == 181 && (_ptoc_MEMCMP(unsigned char , 8, ptoc_Var256->integer.C_50.syskey.integer.C_3.sauthid , (*acv).a_curr_user_id) == 0 ? 1 : 0) || (int ) ptoc_Var256->integer.C_50.syskey.integer.C_2.sentrytyp[1] == 177 && (_ptoc_MEMCMP(unsigned char , 2, ptoc_Var256->integer.C_50.syskey.integer.C_6.ssite , "\377\373") == 0 ? 1 : 0) && (int ) ptoc_Var256->integer.C_50.syskey.integer.C_11.sshowkind == (int ) (*a41v).a4sh_kind || (int ) ptoc_Var256->integer.C_50.syskey.integer.C_2.sentrytyp[1] == 187)
        {
        }
    else
        {
        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) ptoc_Var256->integer.C_50.syskey.integer.C_1.sstandardkey , (int ) ptoc_Var256->integer.C_0.b_kl);
        if ((int ) ptoc_Var256->integer.C_50.syskey.integer.C_2.sentrytyp[1] == 181)
            {
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) 0 , 0);
            }
        else
            {
            a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) ptoc_Var256->integer.C_50.syskey.integer.C_2.slinkage , 2);
            }
        i4 = sql__ord((int ) ptoc_Var256->integer.C_50.syskey.integer.C_2.slinkage[1]);
        switch ((int ) ptoc_Var256->integer.C_50.syskey.integer.C_2.sentrytyp[1])
            {
            case 129 :
                if (i4 == 1 && (char ) sql__setrang(ptoc_Var256->integer.C_23.sparsinfo.integer.C_2.p_select , 0 , 1))
                    {
                    a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) ptoc_Var256->integer.C_23.sparsinfo.integer.C_2.p_resn , sizeof(unsigned char [64]));
                    }
                else
                    {
                    a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                    }
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "PARSINFO    ")
                break;
            case 130 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "JPARSINFO   ")
                break;
            case 131 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "CORRINFO    ")
                break;
            case 132 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                i4 = (int ) ptoc_Var256->integer.C_31.sshortinfo.sicount;
                a40move_i4(&(*acv) , &(*a41v) , i4 , 0 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "SHORTINFO   ")
                break;
            case 134 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                a40move_i4(&(*acv) , &(*a41v) , i4 , 0 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "STRATINFO   ")
                break;
            case 135 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) ptoc_Var256->integer.C_40.sunionrec.ures_name , sizeof(unsigned char [64]));
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "UNIONREC    ")
                break;
            case 136 :
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "VIEWKEY     ")
                break;
            case 137 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                i4 = (int ) ptoc_Var256->integer.C_3.scomplexrec.compcnt;
                a40move_i4(&(*acv) , &(*a41v) , i4 , 0 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "COMPLEX     ")
                break;
            case 139 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "TEMPPRIV    ")
                break;
            case 141 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "RESSITEINFO ")
                break;
            case 148 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                i4 = (int ) ptoc_Var256->integer.C_16.slinkparsinfo.lpcount;
                a40move_i4(&(*acv) , &(*a41v) , i4 , 0 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "LINKPARSINFO")
                break;
            case 149 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "TRIGGERPARS ")
                break;
            case 150 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                i4 = (int ) ptoc_Var256->integer.C_33.ssubtrans.str_count;
                a40move_i4(&(*acv) , &(*a41v) , i4 , 0 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "SUBTRANS    ")
                break;
            case 151 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "STORED_CMD  ")
                break;
            case 152 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                i4 = (int ) ptoc_Var256->integer.C_61.schangeinfos.ci_colcount;
                a40move_i4(&(*acv) , &(*a41v) , i4 , 0 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "CHANGEINFOS ")
                break;
            case 153 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                i4 = (int ) ptoc_Var256->integer.C_65.slongliteral.litcolcount;
                a40move_i4(&(*acv) , &(*a41v) , i4 , 0 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "LONGLITERAL ")
                break;
            case 176 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "LOGGED_TEMP ")
                break;
            case 177 :
                if (i4 == 1)
                    {
                    if (!(char ) sql__setrang(in_cache , 0 , 1))
                        {
                        offset = sizeof(tak_baserecord ) - sizeof(tak_columnarr ) - sizeof(tak_basecolinfo_ptr ) - sizeof(tsp00_KnlIdentifier *);
                        i4 = sql__ord((int ) (*p).integer.C_51.ssysbuf[sql__setrang(offset + 1 , 1 , 32760) + -1]);
                        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &(*p).integer.C_51.ssysbuf[sql__setrang(offset + 2 , 1 , 32760) + -1] , i4);
                        }
                    else
                        {
                        a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) (int *) sql__nil(ptoc_Var256->integer.C_28.sresult.btablen , "vak10.p" , 25016) , sizeof(unsigned char [64]));
                        }
                    }
                else
                    {
                    a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                    }
                i4 = (int ) ptoc_Var256->integer.C_28.sresult.bmaxcol;
                a40move_i4(&(*acv) , &(*a41v) , i4 , 0 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "RESULT      ")
                break;
            case 178 :
                if (i4 == 0)
                    {
                    a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) ptoc_Var256->integer.C_26.sreskey.res_resultname , sizeof(unsigned char [64]));
                    }
                else
                    {
                    a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                    }
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "RESKEY      ")
                break;
            case 180 :
                break;
            case 181 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) ptoc_Var256->integer.C_52.sresname.reskey_name , sizeof(unsigned char [64]));
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "RESNAME     ")
                break;
            case 182 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "TEMPPROC    ")
                break;
            case 183 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "TEMPTABLEREF")
                break;
            case 184 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "TEMPDEFAULT ")
                break;
            case 185 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "TEMPCONSTR  ")
                break;
            case 186 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "TEMPVIEWDESC")
                break;
            case 187 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "TEMPCOLDESC ")
                break;
            case 188 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "TEMPVIEWTEXT")
                break;
            case 189 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "TMPCNSTRNAME")
                break;
            case 190 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "LINKPOS     ")
                break;
            case 191 :
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "DBPROCPARS  ")
                break;
            case 192 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "COLNAMES    ")
                break;
            case 193 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "MESSBLOCK   ")
                break;
            case 194 :
                a40move(&(*acv) , &(*a41v) , (tsp00_MoveObj *) &ptoc_Var256->integer.C_50.syskey , 0);
                a40move_i4(&(*acv) , &(*a41v) , i4 , 1 , 0 , 0);
                _ptoc_ASSIGN(unsigned char , 12, tabtype, "COMMAND_INFO")
                break;
            default :
                break;
            }
        a40move_const(&(*acv) , &(*a41v) , (tsp00_MoveObj *) tabtype , sizeof(tsp00_C12 ));
        i4 = (int ) ptoc_Var256->integer.C_0.b_sl;
        a40move_i4(&(*acv) , &(*a41v) , i4 , 0 , 0 , 0);
        }
    cccprint("<ak10one_parsinf");
    }
