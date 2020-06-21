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
  unsigned char tsp00_LdInfo[1];
typedef
  unsigned char tsp00_LdState[1];
typedef
  struct ptoc_Type3
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
    } tsp00_LongDescriptor;
typedef
  tsp00_LongDescriptor tsp_long_descriptor;
typedef
  unsigned char tsp1_warningset_intern[2];
typedef
  unsigned char tsp1_command_options[1];
typedef
  unsigned char tsp1_part_kind;
typedef
  unsigned char tsp1_part_attr[1];
typedef
  struct ptoc_Type4
    {
    tsp1_part_kind sp1p_part_kind;
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
  unsigned char tgg00_Debug;
typedef
  unsigned char tgg00_DumpStateSet[4];
typedef
  unsigned char tgg00_ErrorText;
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
  unsigned char tgg00_QualKind;
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
    tgg00_QualKind m_qual_kind;
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
  tgg00_BasisError tgg_basis_error;
typedef
  tgg00_DataPartPtr tgg_datapart_ptr;
typedef
  struct ptoc_Type48
    {
    union
      {
      struct
        {
        tsp00_C256 errt;
        } C_1;
      struct
        {
        tgg00_Surrogate errtableid;
        tgg00_StackEntry errstack;
        tgg00_ServerdbNo errtablesite;
        unsigned char errstacktype;
        char errrest1;
        tsp00_C20 errrest2;
        } C_2;
      struct
        {
        tgg00_Surrogate errfill1;
        tgg00_StackEntry errfill2;
        tgg00_ServerdbNo errfill3;
        unsigned char errStackType_gg00;
        } C_3;
      } integer;
    } tgg_err;
typedef
  tgg00_ErrorText tgg_errortext;
typedef
  unsigned char tgg_linkset[1];
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
  tgg00_QualKind tgg_qual_kind;
typedef
  tgg00_Rec tgg_rec_buf;
typedef
  struct ptoc_Type49
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
  tgg00_StackEntryType tgg_stack_entry_type;
typedef
  tgg00_StackList tgg_stack_list;
typedef
  tgg00_StackListPtr tgg_stack_list_ptr;
typedef
  tgg00_StackOpType tgg_stack_op_type;
typedef
  struct ptoc_Type50
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
  struct ptoc_Type51
    {
    tgg00_FileId file_id;
    tgg00_FileId inv_id;
    } tgg_two_tree_ids;
typedef
  unsigned char tak00_ooption[1];
typedef
  struct ptoc_Type52
    {
    struct ptoc_Type53
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
  struct ptoc_Type54
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
  struct ptoc_Type55
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
  struct ptoc_Type56
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
  struct ptoc_Type57
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
  tgg00_StackEntry tak_16stackentry[16];
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
  tgg00_StackEntry tak_ak_constr_stack[989];
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
    unsigned char resdatetimeformat;
    char resimpl_upd_stat;
    char resexecuted;
    unsigned char resselect_fetch;
    tgg_surrogate resanalyze_cmdid;
    } tak_resname_record;
typedef
  tgg00_StackEntry tak_trigger_stack[900];
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
      unsigned char vdatatype;
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
  tgg00_StackEntry tak_view_stack_entries[1007];
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
    unsigned char res_pars_dt_format;
    tsp_sqlmode res_pars_sqlmode;
    tsp00_Uint1 res_upd_tabcnt;
    tgg_qual_kind res_qual_kind;
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
    unsigned char sp1i_data_type;
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
  struct ptoc_Type145
    {
    tsp_int2 sf_index;
    tsp_int2 sf_parsno;
    tsp_int2 sf_use_id;
    tsp_int2 sf_fill;
    } tak_selfref_link_info;
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
    unsigned char scr_ddl;
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
    tgg_qual_kind d_qual_kind;
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
    tgg00_FileId co_corr_current;
    tsp_bufaddr co_corr_buf;
    char co_use_cinfo;
    unsigned char co_use_fields;
    tsp_int2 co_all_recs_len;
    tsp_int2 co_one_rec_len;
    tsp_int2 co_one_key_len;
    tsp_int2 co_lcomp_len;
    tsp_int2 co_startkeyfields;
    tsp_int2 co_act_pos;
    tsp_int2 co_sel_keypos;
    tsp_int2 co_upd_keylen;
    tsp_int2 co_minkeylen;
    tsp_int2 co_varkeyvalstack;
    tsp_int2 co_fill;
    } tak_corr_infos;
typedef
  struct ptoc_Type175
    {
    tsp_int2 fr_f_no;
    tsp_int2 fr_last_fno;
    tsp_int2 fr_total_leng;
    tsp_int2 fr_leng;
    tsp_c2 fr_uni_fill_c2;
    tsp_int2 fr_filler0;
    } tak_fill_rec;
typedef
  struct ptoc_Type176
    {
    tgg00_FileId ex_act_treeid;
    tsp_int4 ex_all_rescnt;
    tsp_int2 ex_plus;
    tsp_int2 ex_maxbufs;
    char ex_with_link;
    char ex_output_wanted;
    char ex_command_needed;
    char ex_fill;
    tgg00_DataPartPtr ex_mb_data;
    tsp_moveobj_ptr ex_updjv_ptr;
    } tak_exec_rec;
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
  short tgg07_ColPosArr[20];
typedef
  tsp00_C1 tgg07_StrRaw[236];
typedef
  struct ptoc_Type179
    {
    tgg07_ColPosArr skir_keystart;
    tgg07_ColPosArr skir_keystop;
    tsp00_Int2 skir_IN_SUBQ_stpos;
    tsp00_Int2 skir_inelemcnt;
    } tgg07_StrKeyInRange;
typedef
  unsigned char tgg07_InvColSet[2];
typedef
  struct ptoc_Type180
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
  struct ptoc_Type181
    {
    tgg07_ColPosArr sji_keystart;
    tgg07_ColPosArr sji_keystop;
    tsp00_PageNo sji_invroot;
    tsp00_Int2 sji_invindex;
    tsp00_Int2 sji_jfiller;
    } tgg07_StrJoinInv;
typedef
  struct ptoc_Type182
    {
    tgg07_ColPosArr sjmf_keystart;
    tgg07_ColPosArr sjmf_keystop;
    tsp00_PageNo sjmf_invroot;
    tsp00_Int2 sjmf_index_no;
    tsp00_Int2 sjmf_cntfields;
    } tgg07_StrJoinMultFields;
typedef
  struct ptoc_Type183
    {
    tsp00_Int2 sjv_lastkeylen;
    tsp00_Int2 sjv_minkeylen;
    } tgg07_StrJoinViewkey;
typedef
  struct ptoc_Type184
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
  unsigned char tgg07_StratEnum;
typedef
  unsigned char tgg07_StratEnumSet[4];
typedef
  struct ptoc_Type185
    {
    tgg00_FileId str_result_id;
    tgg07_StratEnum str_strategy;
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
    tgg00_QualKind str_qual_kind;
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
  struct ptoc_Type186
    {
    tgg07_StratEnum ssr_old_strategynum;
    tgg07_StratEnum ssr_new_strategynum;
    tsp_int2 ssr_invlen;
    tsp_int2 ssr_filler;
    } tgg07_selectstrat_rec;
typedef
  tsp_int2 tak68_joinarr_index;
typedef
  struct ptoc_Type187
    {
    union
      {
      struct
        {
        tgg07_StratEnum jos_joinstrat;
        tsp00_Uint1 jos_source;
        tsp00_Uint1 jos_indexno;
        tsp00_Uint1 jos_fieldcnt;
        tak68_joinarr_index jos_joinno;
        char jos_const_param_expr;
        } C_true;
      struct
        {
        tsp_c8 jos_filler;
        } C_false;
      } boolean;
    } tak68_sequence[64];
typedef
  struct ptoc_Type188
    {
    tgg_surrogate jv_tabid;
    tak_sysbufferaddress jv_keybuf;
    tsp_int2 jv_maxkeyl;
    char jv_checkview;
    char jv_replicated;
    } tak68_joinview_rec;
tgg_sysinfokey a01sysnullkey;
tgg_sysinfokey a01defaultkey;
tsp_knl_identifier a01_zero_res_name;
tgg07_StratEnumSet a70glob_fetch_strats;
tgg07_StratEnumSet a70glob_inv_strats;
tgg00_FileId b01niltree_id;
int a05qualification_test();
int a06cpy_mblock();
int a06retpart_move();
int a06finish_curr_retpart();
int a06put_errtext();
int a06a_mblock_init();
int a06dml_send_mess_buf();
int a06rsend_mess_buf();
int a06subquery_del_result();
int a07_b_put_error();
short a07_return_code();
int a07ak_system_error();
int a10get_sysinfo();
int a10rel_sysinfo();
int a10key_del();
int a10invalidate_root();
int a10mblock_into_cache();
int a10new();
int a10dispose();
int a10repl_sysinfo();
int a26execute_comment();
int a28sys_upd_statistics();
int a502correlation_execute();
int a502destroy_file();
int a503build_union_buffer();
int a506fieldvalues();
int a506lock_fieldvalues();
int a506result_name();
int a507last_command_part();
int a507_putval();
int a507_literal();
char a507optim_select_fetch();
int a52init_subtrans_of_new_trans();
int a52_rest_lock_statement();
int a54_dml_init();
int a54_view_put_into();
int a58destroy_linkfile();
int a58exec_link_caused_dml();
int a58delete_with_link();
int a59handle_qualification();
int a60_change_results();
int a663_get_result_info();
int a663_resname();
int a67_invkeylen();
int a682_execute_join();
int a682_only_ex_join();
int a71turn_into_build_strat();
int a71prepare_qual_on_index();
int a721strategy();
int b01empty_file();
int g01mblock_init();
int g01stack_init();
int g04build_temp_tree_id();
int g04inbetween_change();
int g04mblock_optimize_info();
int g04spec_null_check();
int g10mv();
int g10mv1();
int g10mv3();
int g10mv5();
int g10mv6();
int g10mv7();
int g10mv8();
int g10mv9();
int g10mv10();
int g10mv11();
int g10mv12();
int g10mv13();
int g10mv14();
int g10mvr();
int g10fil();
tsp00_Addr gg941Allocate();
int s10mv1();
int s30lnr_defbyte();
int s40g4int();
int t01bool();
int t01knl_identifier();
int t01lidentifier();
int t01int4();
int t01buf();
int t01buf1();
int t01name();
int t01treeid();
int t01longdescriptor();
int t01messblock();
int t01moveobj();
int t01stackentry();
int t01qual_kind();
int cccprint();
int a505check_if_executed();
int a505execute_link();
int a505fill_mess_buf();
int a505get_change_rec();
int a505get_longlit_rec();
int a505loop_most_execute();
int a505most_execute();
int a505next_command();
int a505strategy_search();
int ak505calculate_param_expression();
int ak505st_code_jump();
int ak505st_code_rollback_for_const_expr();
int ak505make_st_entry();
int ak505compare_output_expr();
int ak505between_expr();
int ak505in_expr();
int a505const_param_expression();
int ak505dist_lock_mess_bufs();
int ak505exec_link();
int ak505join_view_check();
int ak505long_support();



int a505check_if_executed(acv, test_name, test_tree)
tak_all_command_glob *acv;
tsp_knl_identifier test_name;
tgg00_FileId *test_tree;
    {
    tgg_basis_error b_err;
    tak_sysbufferaddress resname_buf;
    tgg_sysinfokey sysk;
    tak_all_command_glob *ptoc_Var0;
    tgg00_TransContext *ptoc_Var1;
    tgg00_SysInfoKey *ptoc_Var2;
    cccprint("AK505 a505check_");
    ptoc_Var0 = &*acv;
    ptoc_Var1 = &ptoc_Var0->a_transinf.tri_trans;
    sysk = a01defaultkey;
    ptoc_Var2 = &sysk;
    _ptoc_ASSIGN(unsigned char , 2, ptoc_Var2->integer.C_9.sresentrytype, "\0\265")
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var2->integer.C_9.srestablen, test_name)
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var2->integer.C_9.sresmodul_name, ptoc_Var0->a_modul_name)
    ptoc_Var2->skeylen = (short ) sql__setrang((short ) (sizeof(tsp00_C8 ) + sizeof(tsp00_C2 ) + sizeof(unsigned char [64]) + sizeof(unsigned char [64])) , -32768 , 32767);
    t01lidentifier(5 , ptoc_Var2->integer.C_9.srestablen);
    t01lidentifier(5 , ptoc_Var2->integer.C_9.sresmodul_name);
    a10get_sysinfo(&(*acv) , &sysk , 1 , &resname_buf , &b_err);
    if ((int ) b_err == 0)
        {
        if ((*(struct tak_systembuffer *) sql__nil(resname_buf , "vak505.p" , 14354)).integer.C_52.sresname.restreeid.integer.C_3.fileConstId_gg00 != (*test_tree).integer.C_3.fileConstId_gg00)
            {
            b_err = 1500;
            }
        else
            {
            if (!(char ) sql__setrang((*(struct tak_systembuffer *) sql__nil(resname_buf , "vak505.p" , 14359)).integer.C_52.sresname.resexecuted , 0 , 1))
                {
                b_err = 6516;
                }
            else
                {
                *test_tree = (*(struct tak_systembuffer *) sql__nil(resname_buf , "vak505.p" , 14363)).integer.C_52.sresname.restreeid;
                }
            }
        }
    else
        {
        b_err = 6516;
        }
    if ((int ) b_err != 0)
        {
        a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
        }
    cccprint("<a505check_if_ex");
    }



int a505execute_link(acv, parsk, pno)
tak_all_command_glob *acv;
tak_parskey *parsk;
int pno;
    {
    tak_sysbufferaddress mblock_buf;
    tak_parskey init_parsk;
    tgg_sysinfokey syskey;
    cccprint("AK505 a505execut");
    init_parsk = *parsk;
    syskey = a01sysnullkey;
    syskey.integer.C_3.sauthid[0] = (unsigned char)'\377';
    s10mv1(sizeof(tak_parskey ) , sizeof(tsp00_C8 ) , &(*parsk) , 1 , syskey.integer.C_3.sauthid , 2 , 6);
    syskey.integer.C_3.sauthid[6] = (unsigned char ) sql__setrang((unsigned char ) pno , 0 , 255);
    _ptoc_ASSIGN(unsigned char , 2, syskey.integer.C_2.sentrytyp, "\0\301")
    ak505exec_link(&(*acv) , &syskey , &mblock_buf , 1);
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 14401)).integer.C_4.sp1r_returncode != 0)
        {
        (*acv).a_in_internal_subtr = 1;
        (*acv).a_part_rollback = 1;
        }
    cccprint("<a505execute_lin");
    }

static
  unsigned char ptoc_Var3[2] =
  {
  (unsigned char)' ', (unsigned char)'\n'
  };
static
  unsigned char ptoc_Var4[2] =
  {
  (unsigned char)' ', (unsigned char)'\n'
  };
static
  unsigned char ptoc_Var5[2] =
  {
  (unsigned char)'\002', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var6[2] =
  {
  (unsigned char)'\004', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var7[2] =
  {
  (unsigned char)'\002', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var8[2] =
  {
  (unsigned char)'\001', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var9[2] =
  {
  (unsigned char)'\004', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var10[2] =
  {
  (unsigned char)'\001', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var11[2] =
  {
  (unsigned char)'\002', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var12[2] =
  {
  (unsigned char)'\001', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var13[2] =
  {
  (unsigned char)'\004', (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var14[2] =
  {
  (unsigned char)'\001', (unsigned char)'\0'
  };


int a505fill_mess_buf(acv, dmli, erec, frec, full_part2)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_exec_rec *erec;
tak_fill_rec *frec;
char full_part2;
    {
    tgg00_HandlingSet cmd_handling;
    tgg00_HandlingSet tree_handling;
    tgg_basis_error b_err;
    tsp00_Addr p;
    tgg_sysinfokey ke;
    tgg_mess_block_ptr pmbp;
    int aux_mb_data_len;
    tak_all_command_glob *ptoc_Var15;
    tak_dml_info *ptoc_Var16;
    tak_fill_rec *ptoc_Var17;
    tgg00_SysInfoKey *ptoc_Var18;
    tak_parsinforecord *ptoc_Var19;
    tgg00_QualBuf *ptoc_Var20;
    int ptoc_Var21;
    unsigned char ptoc_Var22[2];
    unsigned char ptoc_Var23[2];
    unsigned char ptoc_Var24[2];
    unsigned char ptoc_Var25[2];
    unsigned char ptoc_Var26[2];
    unsigned char ptoc_Var27[2];
    cccprint("AK505 a505fill_m");
    ptoc_Var15 = &*acv;
    ptoc_Var16 = &*dmli;
    ptoc_Var17 = &*frec;
    t01int4(5 , "pos_in_parsb" , (int ) ptoc_Var16->d_pos_in_parsbuf);
    ke = (*(struct tak_systembuffer *) sql__nil(ptoc_Var16->d_sparr.px[0] , "vak505.p" , 14433)).integer.C_50.syskey;
    ptoc_Var18 = &ke;
    ptoc_Var18->integer.C_3.sauthid[7] = (unsigned char ) sql__setrang(a01sysnullkey.integer.C_3.sauthid[7] , 0 , 255);
    _ptoc_ASSIGN(unsigned char , 2, ptoc_Var18->integer.C_2.sentrytyp, "\0\301")
    ptoc_Var16->d_sparr.pcount = (short ) sql__setrang((short ) ((int ) ptoc_Var16->d_sparr.pcount + 1) , -32768 , 32767);
    a10get_sysinfo(&(*acv) , &ke , 0 , &ptoc_Var16->d_sparr.px[sql__setrang((int ) ptoc_Var16->d_sparr.pcount , 1 , 8) + -1] , &b_err);
    if ((int ) b_err != 0)
        {
        a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , (*(tsp1_part *) sql__nil(ptoc_Var15->a_cmd_part , "vak505.p" , 14446)).integer.C_2.sp1p_buf_len);
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var15->a_return_segm , "vak505.p" , 14448)).integer.C_4.sp1r_returncode == 0)
        {
        a06a_mblock_init(&(*acv) , 0 , 0 , &b01niltree_id);
        pmbp = (struct tgg00_MessBlock *) &(*(struct tak_systembuffer *) sql__nil(ptoc_Var16->d_sparr.px[sql__setrang((int ) (*dmli).d_sparr.pcount , 1 , 8) + -1] , "vak505.p" , 14452)).integer.C_62.smessblock.mbr_mess_block;
        if ((char ) sql__setrang(ptoc_Var15->a_stored_proc_call , 0 , 1))
            {
            t01lidentifier(5 , (*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 14457)).integer.C_23.sparsinfo.integer.C_2.p_resn);
            _ptoc_ASSIGN(unsigned char , 64, (*acv).a_result_name, (*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 14458)).integer.C_23.sparsinfo.integer.C_2.p_resn)
            if ((int ) (*(tgg00_QualBuf *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(pmbp , "vak505.p" , 14459)).integer.C_4.mb_qual , "vak505.p" , 14459)).integer.C_2.mspecial > 0)
                {
                p = gg941Allocate(&(*acv).a_transinf.tri_trans , (int ) (*(tgg00_QualBuf *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(pmbp , "vak505.p" , 14464)).integer.C_4.mb_qual , "vak505.p" , 14464)).integer.C_2.mspecial);
                if (p == (unsigned char *) (int *) 0)
                    {
                    a07_b_put_error(&(*acv) , 1405 , 1);
                    }
                else
                    {
                    ptoc_Var15->a_mblock.integer.C_3.mb_data = (tgg00_DataPart *) (int *) sql__nil(p , "vak505.p" , 14470);
                    (*erec).ex_mb_data = ptoc_Var15->a_mblock.integer.C_3.mb_data;
                    ptoc_Var15->a_mblock.integer.C_2.mb_data_size = (int ) (*(tgg00_QualBuf *) sql__nil((*(struct tgg00_MessBlock *) sql__nil(pmbp , "vak505.p" , 14472)).integer.C_4.mb_qual , "vak505.p" , 14472)).integer.C_2.mspecial;
                    }
                }
            }
        ptoc_Var15->a_mblock.integer.C_2.mb_data_len = 0;
        t01messblock(5 , "MBLOCK ctlog" , sql__nil(pmbp , "vak505.p" , 14480));
        aux_mb_data_len = (*(struct tgg00_MessBlock *) sql__nil(pmbp , "vak505.p" , 14481)).integer.C_2.mb_data_len;
        if (!(char ) sql__setrang(full_part2 , 0 , 1) && (int ) (*(struct tgg00_MessBlock *) sql__nil(pmbp , "vak505.p" , 14482)).integer.C_2.mb_type != 28 && (*(struct tgg00_MessBlock *) sql__nil(pmbp , "vak505.p" , 14483)).integer.C_2.mb_data_len > 0 && (*(struct tgg00_MessBlock *) sql__nil(pmbp , "vak505.p" , 14484)).integer.C_3.mb_data != (tgg00_DataPart *) (int *) 0)
            {
            (*(struct tgg00_MessBlock *) sql__nil(pmbp , "vak505.p" , 14486)).integer.C_2.mb_data_len = 0;
            }
        a06cpy_mblock(&(*acv) , sql__nil(pmbp , "vak505.p" , 14488) , &ptoc_Var15->a_mblock , &b_err);
        (*(struct tgg00_MessBlock *) sql__nil(pmbp , "vak505.p" , 14489)).integer.C_2.mb_data_len = aux_mb_data_len;
        ptoc_Var15->a_mblock.integer.C_2.mb_data_len = (*(struct tgg00_MessBlock *) sql__nil(pmbp , "vak505.p" , 14490)).integer.C_2.mb_data_len;
        if ((int ) b_err != 0)
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
            }
        }
    if ((int ) ptoc_Var15->a_mblock.integer.C_2.mb_struct != 6)
        {
        ptoc_Var19 = &(*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 14499)).integer.C_23.sparsinfo;
        ptoc_Var20 = (tgg00_QualBuf *) sql__nil(ptoc_Var15->a_mblock.integer.C_4.mb_qual , "vak505.p" , 14499);
        t01int4(5 , "p_session_lv" , (int ) ptoc_Var19->integer.C_2.p_session_isolevel);
        t01int4(5 , "a_iso_level " , (int ) ptoc_Var15->a_iso_level);
        t01int4(5 , "p_use_sesslv" , sql__ord((int ) (char ) sql__setrang(ptoc_Var19->integer.C_2.p_use_sess_isolevel , 0 , 1)));
        if ((int ) ptoc_Var19->integer.C_2.p_session_isolevel != (int ) ptoc_Var15->a_iso_level && (char ) sql__setrang(ptoc_Var19->integer.C_2.p_use_sess_isolevel , 0 , 1) && (int ) ptoc_Var20->integer.C_2.mtree.integer.C_4.fileTfn_gg00 != 26)
            {
            if ((ptoc_Var21 = (int ) ptoc_Var19->integer.C_2.p_session_isolevel) != 0 && ptoc_Var21 != 1 || (ptoc_Var21 = (int ) ptoc_Var15->a_iso_level) != 0 && ptoc_Var21 != 1)
                {
                a07_b_put_error(&(*acv) , 1520 , 1);
                }
            else
                {
                _ptoc_MOVE(unsigned char , 2, cmd_handling, *(unsigned char (*)[2]) sql__setsub(ptoc_Var22 , ptoc_Var19->integer.C_2.p_handling_set , ptoc_Var3 , sizeof(unsigned char [2])));
                _ptoc_MOVE(unsigned char , 2, tree_handling, *(unsigned char (*)[2]) sql__setsub(ptoc_Var24 , ptoc_Var20->integer.C_2.mtree.integer.C_1.fileHandling_gg00 , ptoc_Var4 , sizeof(unsigned char [2])));
                if (sql__erq(sizeof(tgg00_HandlingSet ) , tree_handling , cmd_handling) || sql__erq(sizeof(unsigned char [2]) , tree_handling , sql__psetadd(ptoc_Var22 , sql__setsub(ptoc_Var24 , cmd_handling , ptoc_Var5 , sizeof(unsigned char [2])) , ptoc_Var6 , sizeof(unsigned char [2]))))
                    {
                    if ((int ) ptoc_Var15->a_iso_level == 0)
                        {
                        if (1 && (ptoc_Var20->integer.C_2.mtree.integer.C_1.fileHandling_gg00[0] & 2) != 0)
                            {
                            _ptoc_MOVE(unsigned char , 2, ptoc_Var20->integer.C_2.mtree.integer.C_1.fileHandling_gg00, *(unsigned char (*)[2]) sql__psetadd(ptoc_Var22 , sql__setsub(ptoc_Var25 , ptoc_Var20->integer.C_2.mtree.integer.C_1.fileHandling_gg00 , ptoc_Var7 , sizeof(unsigned char [2])) , ptoc_Var8 , sizeof(unsigned char [2])));
                            }
                        if (1 && (ptoc_Var20->integer.C_2.mtree.integer.C_1.fileHandling_gg00[0] & 4) != 0)
                            {
                            _ptoc_MOVE(unsigned char , 2, ptoc_Var20->integer.C_2.mtree.integer.C_1.fileHandling_gg00, *(unsigned char (*)[2]) sql__psetadd(ptoc_Var27 , sql__setsub(ptoc_Var22 , ptoc_Var20->integer.C_2.mtree.integer.C_1.fileHandling_gg00 , ptoc_Var9 , sizeof(unsigned char [2])) , ptoc_Var10 , sizeof(unsigned char [2])));
                            }
                        }
                    else
                        {
                        if ((int ) ptoc_Var15->a_mblock.integer.C_2.mb_type == 61 || (char ) sql__setrang(ptoc_Var19->integer.C_2.p_single , 0 , 1))
                            {
                            _ptoc_MOVE(unsigned char , 2, ptoc_Var20->integer.C_2.mtree.integer.C_1.fileHandling_gg00, *(unsigned char (*)[2]) sql__setsub(ptoc_Var23 , sql__psetadd(ptoc_Var27 , ptoc_Var20->integer.C_2.mtree.integer.C_1.fileHandling_gg00 , ptoc_Var11 , sizeof(unsigned char [2])) , ptoc_Var12 , sizeof(unsigned char [2])));
                            }
                        else
                            {
                            if ((int ) ptoc_Var15->a_mblock.integer.C_2.mb_type == 60)
                                {
                                _ptoc_MOVE(unsigned char , 2, ptoc_Var20->integer.C_2.mtree.integer.C_1.fileHandling_gg00, *(unsigned char (*)[2]) sql__setsub(ptoc_Var26 , sql__psetadd(ptoc_Var23 , ptoc_Var20->integer.C_2.mtree.integer.C_1.fileHandling_gg00 , ptoc_Var13 , sizeof(unsigned char [2])) , ptoc_Var14 , sizeof(unsigned char [2])));
                                }
                            }
                        }
                    }
                }
            }
        }
    if ((char ) sql__setrang(full_part2 , 0 , 1) || (int ) ptoc_Var15->a_mblock.integer.C_2.mb_type == 28)
        {
        ptoc_Var16->d_pos_in_parsbuf = (short ) sql__setrang((short ) (ptoc_Var15->a_mblock.integer.C_2.mb_data_len + 1) , -32768 , 32767);
        }
    else
        {
        ptoc_Var16->d_pos_in_parsbuf = 1;
        }
    if (!(char ) sql__setrang(full_part2 , 0 , 1))
        {
        ptoc_Var15->a_mblock.integer.C_2.mb_data_len = 0;
        }
    t01messblock(5 , "MBLOCK 505fi" , &ptoc_Var15->a_mblock);
    cccprint("<a505fill_mess_b");
    }



int a505get_change_rec(acv, sparr, change_rec)
tak_all_command_glob *acv;
tak_syspointerarr *sparr;
tak_changerecord *change_rec;
    {
    tak_sysbufferaddress sysbuf;
    tgg_sysinfokey sysk;
    tgg_basis_error b_err;
    tak_parsinforecord *ptoc_Var28;
    tak_changeinfos *ptoc_Var29;
    tak_changerecord *ptoc_Var30;
    cccprint("AK505 a505get_ch");
    if ((*sparr).px[0] != (struct tak_systembuffer *) (int *) 0)
        {
        ptoc_Var28 = &(*(struct tak_systembuffer *) sql__nil((*sparr).px[0] , "vak505.p" , 14596)).integer.C_23.sparsinfo;
        t01buf(5 , sql__nil((*sparr).px[0] , "vak505.p" , 14598) , 1 , (int ) (*(struct tak_systembuffer *) sql__nil((*sparr).px[0] , "vak505.p" , 14598)).integer.C_0.b_sl);
        if ((char ) sql__setrang(ptoc_Var28->integer.C_2.p_changeinfos_exist , 0 , 1) && (char ) sql__setrang(ptoc_Var28->integer.C_2.p_select , 0 , 1) && (char ) sql__setrang(ptoc_Var28->integer.C_2.p_single , 0 , 1))
            {
            sysk = (*(struct tak_systembuffer *) sql__nil((*sparr).px[0] , "vak505.p" , 14602)).integer.C_50.syskey;
            _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0\230")
            a10get_sysinfo(&(*acv) , &sysk , 1 , &sysbuf , &b_err);
            if ((int ) b_err != 0)
                {
                if ((int ) b_err == 1930)
                    {
                    (*change_rec).cr_colcount = 0;
                    }
                else
                    {
                    a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 2);
                    }
                }
            else
                {
                ptoc_Var29 = &(*(struct tak_systembuffer *) sql__nil(sysbuf , "vak505.p" , 14614)).integer.C_61.schangeinfos;
                ptoc_Var30 = &*change_rec;
                t01buf(5 , sql__nil(sysbuf , "vak505.p" , 14616) , 1 , (int ) (*(struct tak_systembuffer *) sql__nil(sysbuf , "vak505.p" , 14616)).integer.C_0.b_sl);
                ptoc_Var30->cr_colcount = (short ) sql__setrang(ptoc_Var29->ci_colcount , -32768 , 32767);
                t01buf(5 , sql__nil(sysbuf , "vak505.p" , 14618) , 1 , (int ) (*(struct tak_systembuffer *) sql__nil(sysbuf , "vak505.p" , 14618)).integer.C_0.b_sl);
                t01int4(5 , "cr_colcount " , (int ) (*change_rec).cr_colcount);
                g10mv3("VAK505" , 1 , sizeof(tak_column_change_arr ) , sizeof(tak_column_change_arr ) , ptoc_Var29->ci_ch_columns , 1 , (*change_rec).cr_columns , 1 , (int ) (*change_rec).cr_colcount * sizeof(struct ptoc_Type131 ) , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 14625)).integer.C_4.sp1r_returncode);
                }
            }
        else
            {
            (*change_rec).cr_colcount = 0;
            }
        }
    else
        {
        (*change_rec).cr_colcount = 0;
        }
    (*change_rec).cr_was_opt = 0;
    (*change_rec).cr_filler = 0;
    cccprint("<a505get_change_");
    }



int a505get_longlit_rec(acv, sparr, longlit_rec)
tak_all_command_glob *acv;
tak_syspointerarr *sparr;
tak_sysbufferaddress *longlit_rec;
    {
    tgg_sysinfokey sysk;
    tgg_basis_error b_err;
    tak_parsinforecord *ptoc_Var31;
    cccprint("AK505 a505get_lo");
    if ((*sparr).px[0] != (struct tak_systembuffer *) (int *) 0)
        {
        ptoc_Var31 = &(*(struct tak_systembuffer *) sql__nil((*sparr).px[0] , "vak505.p" , 14658)).integer.C_23.sparsinfo;
        t01buf(5 , sql__nil((*sparr).px[0] , "vak505.p" , 14660) , 1 , (int ) (*(struct tak_systembuffer *) sql__nil((*sparr).px[0] , "vak505.p" , 14660)).integer.C_0.b_sl);
        if ((char ) sql__setrang(ptoc_Var31->integer.C_2.p_longliteral , 0 , 1))
            {
            sysk = (*(struct tak_systembuffer *) sql__nil((*sparr).px[0] , "vak505.p" , 14664)).integer.C_50.syskey;
            _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0\231")
            a10get_sysinfo(&(*acv) , &sysk , 0 , &(*longlit_rec) , &b_err);
            if ((int ) b_err != 0)
                {
                a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 2);
                }
            else
                {
                (*(struct tak_systembuffer *) sql__nil(*longlit_rec , "vak505.p" , 14672)).integer.C_65.slongliteral.litcolcount = 0;
                t01buf(5 , sql__nil(*longlit_rec , "vak505.p" , 14673) , 1 , (int ) (*(struct tak_systembuffer *) sql__nil(*longlit_rec , "vak505.p" , 14673)).integer.C_0.b_sl);
                }
            }
        else
            {
            *longlit_rec = (struct tak_systembuffer *) (int *) 0;
            }
        }
    else
        {
        *longlit_rec = (struct tak_systembuffer *) (int *) 0;
        }
    cccprint("<a505get_longlit");
    }



int a505loop_most_execute(acv, dmli, change_rec, erec, parsk, viewkeybuf, series, cinfo, old_cinfo, inner_loop)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_changerecord *change_rec;
tak_exec_rec *erec;
tak_parskey *parsk;
tak_sysbufferaddress viewkeybuf;
tak68_sequence series;
tak_corr_infos *cinfo;
tak_corr_infos *old_cinfo;
char inner_loop;
    {
    char sub_found;
    char check_after_dml;
    tgg_err err;
    tak_exec_rec intern_erec;
    int i;
    tak68_joinview_rec jvrec;
    tgg00_FileId old_res_tree;
    tgg00_FileId sub_del_help_tree;
    tak_all_command_glob *ptoc_Var32;
    tak_dml_info *ptoc_Var33;
    tak_exec_rec *ptoc_Var34;
    int ptoc_Var35;
    tak68_joinview_rec *ptoc_Var36;
    tak_corr_infos *ptoc_Var37;
    tak_parskey *ptoc_Var38;
    cccprint("AK505 a505loop_m");
    ptoc_Var32 = &*acv;
    ptoc_Var33 = &*dmli;
    ptoc_Var34 = &intern_erec;
    g04build_temp_tree_id(&sub_del_help_tree , &ptoc_Var32->a_transinf.tri_trans);
    sub_del_help_tree.integer.C_8.fileTfnTemp_gg00 = 12;
    intern_erec = *erec;
    (*erec).ex_command_needed = 1;
    t01int4(5 , "plus        " , (int ) ptoc_Var34->ex_plus);
    t01int4(5 , "maxbufs     " , (int ) ptoc_Var34->ex_maxbufs);
    t01int4(5 , "   START    " , 1);
    if ((int ) ptoc_Var32->a_command_kind != 0 && ptoc_Var32->a_union_cnt == 0 && !(char ) sql__setrang(inner_loop , 0 , 1))
        {
        old_res_tree = (*(struct tak_systembuffer *) sql__nil(ptoc_Var32->a_ptr2 , "vak505.p" , 14720)).integer.C_3.scomplexrec.comptree;
        }
    else
        {
        old_res_tree = b01niltree_id;
        }
    ptoc_Var34->ex_act_treeid = b01niltree_id;
    while ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var32->a_return_segm , "vak505.p" , 14725)).integer.C_4.sp1r_returncode == 0 && (int ) ptoc_Var34->ex_plus + 1 <= (int ) ptoc_Var34->ex_maxbufs)
        {
        ptoc_Var34->ex_all_rescnt = 0;
        ptoc_Var34->ex_act_treeid.integer.C_1.fileRoot_gg00 = 2147483647;
        sub_found = 0;
        ptoc_Var34->ex_plus = (short ) sql__setrang((short ) ((int ) ptoc_Var34->ex_plus + 1) , -32768 , 32767);
        if ((char ) sql__setrang(ptoc_Var34->ex_command_needed , 0 , 1))
            {
            ptoc_Var35 = (*dmli).d_sparr.pcount;
            if (1 <= ptoc_Var35)
                {
                i = 1;
                do
                    {
                    a10rel_sysinfo((*dmli).d_sparr.px[sql__setrang(i , 1 , 8) + -1]);
                    if (i == ptoc_Var35)
                        {
                        break;
                        }
                    i += 1;
                    }
                while (!0);
                }
            a505next_command(&(*acv) , &(*dmli).d_sparr , &(*parsk) , (int ) ptoc_Var34->ex_plus);
            }
        else
            {
            ptoc_Var34->ex_command_needed = 1;
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var32->a_return_segm , "vak505.p" , 14743)).integer.C_4.sp1r_returncode == 0)
            {
            if ((int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var33->d_sparr.px[0] , "vak505.p" , 14745)).integer.C_23.sparsinfo.integer.C_2.p_mtyp == 39 && (int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var33->d_sparr.px[0] , "vak505.p" , 14746)).integer.C_23.sparsinfo.integer.C_2.p_mtyp2 == 42)
                {
                ptoc_Var36 = &jvrec;
                ptoc_Var37 = &*cinfo;
                ptoc_Var36->jv_keybuf = viewkeybuf;
                _ptoc_ASSIGN(unsigned char , 8, ptoc_Var36->jv_tabid, (*(struct tak_systembuffer *) sql__nil(ptoc_Var33->d_sparr.px[0] , "vak505.p" , 14752)).integer.C_23.sparsinfo.integer.C_2.p_treeid.integer.C_4.fileTabId_gg00)
                ptoc_Var36->jv_maxkeyl = (short ) sql__setrang((*(struct tak_systembuffer *) sql__nil(ptoc_Var33->d_sparr.px[0] , "vak505.p" , 14753)).integer.C_23.sparsinfo.integer.C_2.p_j_v_keylen , -32768 , 32767);
                ptoc_Var36->jv_checkview = (char ) sql__setrang((*(struct tak_systembuffer *) sql__nil(ptoc_Var33->d_sparr.px[0] , "vak505.p" , 14754)).integer.C_23.sparsinfo.integer.C_2.p_reuse , 0 , 1);
                ptoc_Var34->ex_with_link = 0;
                check_after_dml = (char ) sql__setrang(!(char ) sql__setrang((*(struct tak_systembuffer *) sql__nil(ptoc_Var33->d_sparr.px[0] , "vak505.p" , 14757)).integer.C_23.sparsinfo.integer.C_2.p_select , 0 , 1) , 0 , 1);
                t01int4(5 , "check_after_" , sql__ord((int ) (char ) sql__setrang(check_after_dml , 0 , 1)));
                ak505join_view_check(&(*acv) , &(*dmli) , &ptoc_Var34->ex_act_treeid , &jvrec , series , (int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var33->d_sparr.px[0] , "vak505.p" , 14762)).integer.C_23.sparsinfo.integer.C_2.p_joinviewtabcnt);
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var32->a_return_segm , "vak505.p" , 14763)).integer.C_4.sp1r_returncode == (int ) a07_return_code(6408 , (unsigned char ) sql__setrang(ptoc_Var32->a_sqlmode , 0 , 3)) && (char ) sql__setrang(check_after_dml , 0 , 1))
                    {
                    (*(tsp1_segment *) sql__nil(ptoc_Var32->a_return_segm , "vak505.p" , 14768)).integer.C_4.sp1r_returncode = 0;
                    _ptoc_ASSIGN(unsigned char , 8, err.integer.C_2.errtableid, (*(struct tak_systembuffer *) sql__nil(ptoc_Var33->d_sparr.px[0] , "vak505.p" , 14770)).integer.C_23.sparsinfo.integer.C_2.p_tabid)
                    a07_b_put_error(&(*acv) , 4700 , 1);
                    if ((int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var33->d_sparr.px[0] , "vak505.p" , 14773)).integer.C_23.sparsinfo.integer.C_2.p_function_code == 3)
                        {
                        a06put_errtext(&(*acv) , 4700 , 2 , 10 , err.integer.C_1.errt , &ptoc_Var32->a_ak_b_err);
                        }
                    else
                        {
                        a06put_errtext(&(*acv) , 1420 , 2 , 10 , err.integer.C_1.errt , &ptoc_Var32->a_ak_b_err);
                        }
                    }
                else
                    {
                    if (!(char ) sql__setrang(check_after_dml , 0 , 1) && (*erec).ex_updjv_ptr != (tsp00_MoveObj *) (int *) 0)
                        {
                        g10mv("VAK505" , 2 , ptoc_Var32->a_mblock.integer.C_2.mb_data_size , sizeof(tsp00_MoveObj ) , (*(tgg00_DataPart *) sql__nil(ptoc_Var32->a_mblock.integer.C_3.mb_data , "vak505.p" , 14794)).integer.C_1.mbp_buf , 1 , sql__nil((*erec).ex_updjv_ptr , "vak505.p" , 14795) , 1 , ptoc_Var32->a_mblock.integer.C_2.mb_data_len , &(*(tsp1_segment *) sql__nil(ptoc_Var32->a_return_segm , "vak505.p" , 14797)).integer.C_4.sp1r_returncode);
                        }
                    }
                }
            else
                {
                if ((int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var33->d_sparr.px[0] , "vak505.p" , 14804)).integer.C_23.sparsinfo.integer.C_2.p_subcntlevel >= 0)
                    {
                    sub_found = 1;
                    sub_del_help_tree.integer.C_8.fileLevel_gg00[0] = (unsigned char ) sql__setrang((unsigned char ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var33->d_sparr.px[0] , "vak505.p" , 14809)).integer.C_23.sparsinfo.integer.C_2.p_subcntlevel , 0 , 255);
                    }
                if ((int ) ptoc_Var34->ex_plus + 1 > (int ) ptoc_Var34->ex_maxbufs)
                    {
                    ptoc_Var34->ex_all_rescnt = (*erec).ex_all_rescnt;
                    ptoc_Var34->ex_act_treeid = (*erec).ex_act_treeid;
                    }
                t01name(5 , "co_corr_buf       ");
                if ((*cinfo).co_corr_buf == (tsp00_Buf *) (int *) 0)
                    {
                    t01name(5 , "    is NIL        ");
                    }
                else
                    {
                    t01buf1(5 , sql__nil((*cinfo).co_corr_buf , "vak505.p" , 14824) , 1 , 100);
                    }
                a505most_execute(&(*acv) , &(*dmli) , &(*change_rec) , &(*parsk) , &intern_erec , &(*cinfo) , &(*old_cinfo) , viewkeybuf , (char ) sql__setrang(inner_loop , 0 , 1));
                }
            }
        if ((int ) (*parsk).p_kind == 60 || (int ) (*parsk).p_kind == 61 || (int ) (*parsk).p_kind == 18 || (int ) (*parsk).p_kind == 76)
            {
            t01treeid(5 , "old_res_tree" , &old_res_tree);
            t01treeid(5 , "ierec.acttre" , &ptoc_Var34->ex_act_treeid);
            (*acv).a_from_select = (char ) sql__setrang((char ) sql__setrang((*acv).a_from_select , 0 , 1) || (int ) old_res_tree.integer.C_4.fileTfn_gg00 == 26 && (int ) old_res_tree.integer.C_8.fileTfnTemp_gg00 == 3 , 0 , 1);
            if (((int ) old_res_tree.integer.C_8.fileTfnTemp_gg00 == 21 || (int ) (*(tsp1_segment *) sql__nil(ptoc_Var32->a_return_segm , "vak505.p" , 14844)).integer.C_4.sp1r_returncode != 0 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var32->a_return_segm , "vak505.p" , 14845)).integer.C_4.sp1r_returncode != 100 && (int ) old_res_tree.integer.C_8.fileTfnTemp_gg00 == 19) && (int ) old_res_tree.integer.C_4.fileTfn_gg00 == 26)
                {
                a502destroy_file(&(*acv) , &old_res_tree);
                }
            }
        old_res_tree = ptoc_Var34->ex_act_treeid;
        if ((int ) ptoc_Var32->a_command_kind != 0 && ptoc_Var32->a_union_cnt < 2)
            {
            (*(struct tak_systembuffer *) sql__nil(ptoc_Var32->a_ptr2 , "vak505.p" , 14857)).integer.C_3.scomplexrec.comptree = ptoc_Var34->ex_act_treeid;
            }
        t01name(5 , "end loop most ex. ");
        t01treeid(5 , "old_res_tree" , &old_res_tree);
        if ((char ) sql__setrang(intern_erec.ex_with_link , 0 , 1) && !(char ) sql__setrang(inner_loop , 0 , 1))
            {
            a505execute_link(&(*acv) , &(*parsk) , (int ) ptoc_Var34->ex_plus);
            intern_erec.ex_with_link = 0;
            }
        t01int4(5 , "a_union_cnt." , ptoc_Var32->a_union_cnt);
        t01int4(5 , "a_first_unio" , sql__ord((int ) (char ) sql__setrang(ptoc_Var32->a_first_union , 0 , 1)));
        t01int4(5 , "a_send_messb" , sql__ord((int ) (char ) sql__setrang(ptoc_Var32->a_send_messbuf , 0 , 1)));
        t01int4(5 , "sub_found   " , sql__ord((int ) (char ) sql__setrang(sub_found , 0 , 1)));
        ptoc_Var32->a_sub_found = (char ) sql__setrang(sub_found , 0 , 1);
        if ((char ) sql__setrang(sub_found , 0 , 1) && (!(char ) sql__setrang(ptoc_Var32->a_first_union , 0 , 1) && ptoc_Var32->a_union_cnt <= 1 || (char ) sql__setrang(ptoc_Var32->a_send_messbuf , 0 , 1)))
            {
            ptoc_Var32->a_sub_found = 0;
            a06subquery_del_result(&(*acv) , &(*dmli) , &sub_del_help_tree , 11);
            if (sql__ord((int ) sub_del_help_tree.integer.C_8.fileLevel_gg00[0]) == 0)
                {
                (*dmli).d_sublevel = (unsigned char)'\377';
                }
            else
                {
                (*dmli).d_sublevel = (unsigned char ) sql__setrang((unsigned char ) (sql__ord((int ) sub_del_help_tree.integer.C_8.fileLevel_gg00[0]) + -1) , 0 , 255);
                }
            }
        t01int4(5 , "ftemptype   " , sql__ord((int ) old_res_tree.integer.C_8.fileTfnTemp_gg00));
        t01int4(5 , "flevel[ 1 ] " , sql__ord((int ) old_res_tree.integer.C_8.fileLevel_gg00[0]));
        t01int4(5 , "d_sublevel  " , (int ) (*dmli).d_sublevel);
        if ((int ) old_res_tree.integer.C_8.fileTfnTemp_gg00 == 12 && (int ) old_res_tree.integer.C_4.fileTfn_gg00 == 26)
            {
            if ((int ) (*dmli).d_sublevel > sql__ord((int ) old_res_tree.integer.C_8.fileLevel_gg00[0]))
                {
                (*dmli).d_sublevel = (unsigned char ) sql__setrang((unsigned char ) sql__ord((int ) old_res_tree.integer.C_8.fileLevel_gg00[0]) , 0 , 255);
                }
            }
        }
    ptoc_Var38 = &*parsk;
    if (((int ) ptoc_Var38->p_kind == 60 || (int ) ptoc_Var38->p_kind == 61 || (int ) ptoc_Var38->p_kind == 18 || (int ) ptoc_Var38->p_kind == 76 || (int ) ptoc_Var38->p_kind == 36) && (int ) ptoc_Var32->a_command_kind == 0 && (int ) (*acv).a_qualified_jv_upd == 0)
        {
        if ((int ) ptoc_Var34->ex_act_treeid.integer.C_8.fileTfnTemp_gg00 == 21 && (int ) ptoc_Var34->ex_act_treeid.integer.C_4.fileTfn_gg00 == 26)
            {
            a502destroy_file(&(*acv) , &ptoc_Var34->ex_act_treeid);
            }
        }
    if ((char ) sql__setrang(intern_erec.ex_with_link , 0 , 1) && !(char ) sql__setrang(inner_loop , 0 , 1))
        {
        a505execute_link(&(*acv) , &(*parsk) , (int ) ptoc_Var34->ex_plus);
        intern_erec.ex_with_link = 0;
        }
    (*erec).ex_plus = (short ) sql__setrang(intern_erec.ex_plus , -32768 , 32767);
    (*erec).ex_with_link = (char ) sql__setrang(intern_erec.ex_with_link , 0 , 1);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var32->a_return_segm , "vak505.p" , 14930)).integer.C_4.sp1r_returncode == 0)
        {
        (*erec).ex_act_treeid = ptoc_Var34->ex_act_treeid;
        (*erec).ex_all_rescnt = ptoc_Var34->ex_all_rescnt;
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var32->a_return_segm , "vak505.p" , 14937)).integer.C_4.sp1r_returncode != 0 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var32->a_return_segm , "vak505.p" , 14938)).integer.C_4.sp1r_returncode != (int ) a07_return_code(6408 , (unsigned char ) sql__setrang(ptoc_Var32->a_sqlmode , 0 , 3)) && (int ) ptoc_Var34->ex_act_treeid.integer.C_4.fileTfn_gg00 == 26)
        {
        if ((int ) ptoc_Var34->ex_act_treeid.integer.C_8.fileTfnTemp_gg00 == 8 && (int ) ptoc_Var34->ex_act_treeid.integer.C_4.fileTfn_gg00 == 26)
            {
            b01empty_file(&(*acv).a_transinf.tri_trans , &ptoc_Var34->ex_act_treeid);
            }
        }
    t01int4(5 , "d_sublevel  " , (int ) (*dmli).d_sublevel);
    t01int4(5 , "returncode  " , (int ) (*(tsp1_segment *) sql__nil(ptoc_Var32->a_return_segm , "vak505.p" , 14950)).integer.C_4.sp1r_returncode);
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var32->a_return_segm , "vak505.p" , 14951)).integer.C_4.sp1r_returncode != 0)
        {
        if ((int ) (*dmli).d_sublevel < 255)
            {
            a06subquery_del_result(&(*acv) , &(*dmli) , &sub_del_help_tree , 10);
            }
        if ((int ) (*parsk).p_kind != 61 && (int ) (*parsk).p_kind != 60 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var32->a_return_segm , "vak505.p" , 14961)).integer.C_4.sp1r_returncode != (int ) a07_return_code(6408 , (unsigned char ) sql__setrang(ptoc_Var32->a_sqlmode , 0 , 3)))
            {
            ptoc_Var32->a_part_rollback = 1;
            }
        }
    t01int4(5 , "plus        " , (int ) ptoc_Var34->ex_plus);
    t01int4(5 , "maxbufs     " , (int ) ptoc_Var34->ex_maxbufs);
    if (1 && (old_res_tree.integer.C_1.fileType_gg00[0] & 1) != 0)
        {
        t01treeid(5 , "old_res_tree" , &old_res_tree);
        }
    t01int4(5 , "   STOP     " , 1);
    if ((*acv).a_union_cnt > 0 && (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var32->a_mblock.integer.C_4.mb_qual , "vak505.p" , 14976)).integer.C_4.mr_restree.integer.C_4.fileTfn_gg00 == 26 && (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var32->a_mblock.integer.C_4.mb_qual , "vak505.p" , 14977)).integer.C_4.mr_restree.integer.C_8.fileTfnTemp_gg00 == 13)
        {
        t01treeid(5 , "mr_restree  " , &(*(tgg00_QualBuf *) sql__nil(ptoc_Var32->a_mblock.integer.C_4.mb_qual , "vak505.p" , 14979)).integer.C_4.mr_restree);
        }
    if ((*acv).a_union_cnt > 0)
        {
        if ((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var32->a_mblock.integer.C_4.mb_qual , "vak505.p" , 14985)).integer.C_4.mr_restree.integer.C_8.fileTfnTemp_gg00 == 13 && (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var32->a_mblock.integer.C_4.mb_qual , "vak505.p" , 14986)).integer.C_4.mr_restree.integer.C_4.fileTfn_gg00 == 26)
            {
            (*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak505.p" , 14988)).integer.C_4.mr_restree = old_res_tree;
            }
        }
    t01treeid(5 , "mr_restree  " , &(*(tgg00_QualBuf *) sql__nil(ptoc_Var32->a_mblock.integer.C_4.mb_qual , "vak505.p" , 14991)).integer.C_4.mr_restree);
    cccprint("<a505loop_most_e");
    }

typedef
  struct ptoc_Type185 tgg07_StrategyInfo_a505most_execute;
static
  unsigned char ptoc_Var39[2] =
  {
  (unsigned char)'\0', (unsigned char)'\002'
  };


int a505most_execute(acv, dmli, change_rec, parsk, erec, cinfo, old_cinfo, viewkeybuf, inner_loop)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_changerecord *change_rec;
tak_parskey *parsk;
tak_exec_rec *erec;
tak_corr_infos *cinfo;
tak_corr_infos *old_cinfo;
tak_sysbufferaddress viewkeybuf;
char inner_loop;
    {
    tgg_basis_error _b_err;
    char _not_last;
    char _last_and_single;
    char _last_single_no_union;
    char _w_link;
    tak_fill_rec _frec;
    int _i_length;
    int _pos;
    tsp_int4 _h_rescount;
    char _f_ok;
    tsp_num_error _re;
    tsp_int4 _munion_cnt;
    tgg07_StrategyInfo_a505most_execute *_gg_strategy_ptr;
    tgg_message_type _mtype;
    tgg_message2_type _m2type;
    tsp_knl_identifier _resnam;
    tsp_knl_identifier _searchname;
    tak_parskey _join_parsk;
    tgg00_FileId _aux_treeid;
    tak_sysbufferaddress _qbufp;
    tgg07_selectstrat_rec _ssr_rec;
    tgg00_TfnTemp _aux_tfn;
    tsp00_Uint1 _union_kind;
    tgg_mess_block_ptr _pmbp;
    tgg_datapart_ptr _pdbp;
    tak_corr_infos *ptoc_Var40;
    tak_parsinforecord *ptoc_Var41;
    tgg00_StackEntry *ptoc_Var42;
    tgg00_DataPart *ptoc_Var43;
    tak_parsinforecord *ptoc_Var44;
    struct tgg00_MessBlock *ptoc_Var45;
    tgg00_DataPart *ptoc_Var46;
    tak_parsinforecord *ptoc_Var47;
    int ptoc_Var48;
    unsigned char ptoc_Var49[2];
    unsigned char ptoc_Var50[2];
    unsigned char ptoc_Var51[2];
    struct tgg00_MessBlock *ptoc_Var52;
    tak_parsinforecord *ptoc_Var53;
    tak_parsinforecord *ptoc_Var54;
    tak_parsinforecord *ptoc_Var55;
    tak_parsinforecord *ptoc_Var56;
    tak_parsinforecord *ptoc_Var57;
    tak_parsinforecord *ptoc_Var58;
    cccprint("AK505 a505most_e");
    (*acv).a_union_append = 0;
    if ((int ) (*acv).a_command_kind == 6)
        {
        (*acv).a_command_kind = 0;
        }
    _not_last = (char ) sql__setrang((int ) (*erec).ex_plus + 1 <= (int ) (*erec).ex_maxbufs || (char ) sql__setrang((*cinfo).co_use_cinfo , 0 , 1) , 0 , 1);
    _last_and_single = (char ) sql__setrang(!(char ) sql__setrang(_not_last , 0 , 1) && (int ) (*acv).a_command_kind == 0 , 0 , 1);
    _last_single_no_union = (char ) sql__setrang((char ) sql__setrang(_last_and_single , 0 , 1) && (*acv).a_union_cnt == 0 , 0 , 1);
    (*acv).a_intern_select_cnt = (short ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15042)).integer.C_23.sparsinfo.integer.C_2.p_int_res_cnt , -32768 , 32767);
    if ((char ) sql__setrang(_last_and_single , 0 , 1) && (int ) (*acv).a_max_intern_select == 0)
        {
        (*acv).a_max_intern_select = (short ) sql__setrang((*acv).a_intern_select_cnt , -32768 , 32767);
        }
    t01bool(5 , "_not_last   " , (char ) sql__setrang(_not_last , 0 , 1));
    t01bool(5 , "_last_and_si" , (char ) sql__setrang(_last_and_single , 0 , 1));
    t01bool(5 , "_last_sing_n" , (char ) sql__setrang(_last_single_no_union , 0 , 1));
    t01int4(5 , "intern_selec" , (int ) (*acv).a_intern_select_cnt);
    t01int4(5 , "max_intern_s" , (int ) (*acv).a_max_intern_select);
    t01name(5 , "co_corr_buf       ");
    if ((*cinfo).co_corr_buf == (tsp00_Buf *) (int *) 0)
        {
        t01name(5 , "    is NIL        ");
        }
    else
        {
        t01buf1(5 , sql__nil((*cinfo).co_corr_buf , "vak505.p" , 15058) , 1 , 100);
        }
    a505get_change_rec(&(*acv) , &(*dmli).d_sparr , &(*change_rec));
    a505get_longlit_rec(&(*acv) , &(*dmli).d_sparr , &(*dmli).d_literalrec);
    _ptoc_ASSIGN(unsigned char , 64, _searchname, (*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15062)).integer.C_23.sparsinfo.integer.C_2.p_searchname)
    t01knl_identifier(5 , "_searchname " , _searchname);
    (*dmli).d_pos_in_parsbuf = 1;
    if ((char ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15065)).integer.C_23.sparsinfo.integer.C_2.p_select , 0 , 1) && !(char ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15066)).integer.C_23.sparsinfo.integer.C_2.p_single , 0 , 1))
        {
        a506result_name(&(*acv) , &(*dmli));
        if ((char ) sql__setrang((*dmli).d_resname_found , 0 , 1) && (_ptoc_MEMCMP(unsigned char , 64, (*dmli).d_resname , a01_zero_res_name) == 0 ? 0 : 1))
            {
            _ptoc_ASSIGN(unsigned char , 64, _resnam, (*dmli).d_resname)
            }
        else
            {
            _ptoc_ASSIGN(unsigned char , 64, _resnam, (*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15074)).integer.C_23.sparsinfo.integer.C_2.p_resn)
            }
        if ((char ) sql__setrang(_last_single_no_union , 0 , 1))
            {
            if (_ptoc_MEMCMP(unsigned char , 64, _resnam , a01_zero_res_name) == 0 ? 1 : 0)
                {
                _aux_tfn = 21;
                _pos = 2;
                }
            else
                {
                _aux_tfn = 4;
                _pos = 1;
                }
            if ((char ) sql__setrang((*dmli).d_repl_reskey , 0 , 1))
                {
                if ((int ) (*dmli).d_mselect_loop_cnt == -2)
                    {
                    a663_resname(&(*acv) , _resnam , (*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15097)).integer.C_23.sparsinfo.integer.C_2.p_modul_name , &(*acv).a_resname_addr[sql__setrang(_pos , 1 , 3) + -1] , 0 , 1);
                    }
                if ((*acv).a_resname_addr[sql__setrang(_pos , 1 , 3) + -1] == (struct tak_systembuffer *) (int *) 0)
                    {
                    a07ak_system_error(&(*acv) , 505 , 1);
                    }
                }
            else
                {
                a663_get_result_info(&(*acv) , _resnam , (*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15109)).integer.C_23.sparsinfo.integer.C_2.p_modul_name , &(*acv).a_resname_addr[sql__setrang(_pos , 1 , 3) + -1] , 1 , (unsigned char ) sql__setrang(_aux_tfn , 0 , 39) , 0 , &_f_ok);
                if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15112)).integer.C_4.sp1r_function_code == 27)
                    {
                    _ptoc_ASSIGN(unsigned char , 64, (*acv).a_result_name, _resnam)
                    }
                }
            }
        }
    (*erec).ex_with_link = 0;
    (*dmli).d_longdesc_found = 0;
    a505fill_mess_buf(&(*acv) , &(*dmli) , &(*erec) , &_frec , (char ) sql__setrang((*cinfo).co_use_cinfo , 0 , 1));
    _w_link = (char ) sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak505.p" , 15125)).integer.C_2.mlink_cnt > 0 && ((int ) (*acv).a_mblock.integer.C_2.mb_type == 35 || (int ) (*acv).a_mblock.integer.C_2.mb_type == 36 || (int ) (*acv).a_mblock.integer.C_2.mb_type == 76 || (int ) (*acv).a_mblock.integer.C_2.mb_type == 18) , 0 , 1);
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15130)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) (*acv).a_mblock.integer.C_2.mb_type == 28)
            {
            a502correlation_execute(&(*acv) , &(*dmli) , &(*change_rec) , &(*erec) , &(*parsk) , &(*cinfo) , (char ) sql__setrang(inner_loop , 0 , 1));
            }
        else
            {
            if ((char ) sql__setrang((*cinfo).co_use_cinfo , 0 , 1))
                {
                ptoc_Var40 = &*cinfo;
                t01messblock(5 , "MBLOCK 505mo" , &(*acv).a_mblock);
                t01int4(5 , "act_pos     " , (int ) ptoc_Var40->co_act_pos);
                t01int4(5 , "startkeyfiel" , (int ) ptoc_Var40->co_startkeyfields);
                t01int4(5 , "one_key_len " , (int ) ptoc_Var40->co_one_key_len);
                if ((int ) (*cinfo).co_use_fields == 1)
                    {
                    t01int4(5 , "use_keyfiel " , 1);
                    }
                ptoc_Var41 = &(*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15151)).integer.C_23.sparsinfo;
                _i_length = (*acv).a_mblock.integer.C_2.mb_data_len;
                _frec.fr_f_no = 1;
                _frec.fr_last_fno = (short ) sql__setrang(ptoc_Var41->integer.C_2.p_cnt_infos , -32768 , 32767);
                if ((int ) _frec.fr_last_fno >= (int ) _frec.fr_f_no)
                    {
                    a506fieldvalues(&(*acv) , &(*dmli) , &_frec , viewkeybuf , (*(tgg00_DataPart *) sql__nil((*acv).a_mblock.integer.C_3.mb_data , "vak505.p" , 15160)).integer.C_1.mbp_buf , (*acv).a_mblock.integer.C_2.mb_data_size);
                    }
                (*acv).a_mblock.integer.C_2.mb_data_len = _i_length;
                if ((int ) (*cinfo).co_use_fields == 1)
                    {
                    g10mv5("VAK505" , 3 , sizeof(tsp00_Buf ) , (*acv).a_mblock.integer.C_2.mb_data_size , sql__nil(ptoc_Var40->co_corr_buf , "vak505.p" , 15171) , (int ) ptoc_Var40->co_act_pos + (int ) ptoc_Var40->co_sel_keypos , (*(tgg00_DataPart *) sql__nil((*acv).a_mblock.integer.C_3.mb_data , "vak505.p" , 15172)).integer.C_1.mbp_buf , (int ) ptoc_Var40->co_startkeyfields , (int ) ptoc_Var40->co_upd_keylen , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15174)).integer.C_4.sp1r_returncode);
                    if ((int ) (*acv).a_mblock.integer.C_2.mb_type == 76 || (int ) (*acv).a_mblock.integer.C_2.mb_type == 18)
                        {
                        ptoc_Var42 = &(*(tgg00_StackList *) sql__nil((*acv).a_mblock.integer.C_5.mb_st , "vak505.p" , 15178))[sql__setrang((int ) ptoc_Var40->co_varkeyvalstack , 1 , 32767) + -1];
                        ptoc_Var43 = (tgg00_DataPart *) sql__nil((*acv).a_mblock.integer.C_3.mb_data , "vak505.p" , 15179);
                        ptoc_Var42->integer.C_1.elen_var = (short ) sql__setrang((short ) (s30lnr_defbyte((tsp00_MoveObj *) ptoc_Var43->integer.C_1.mbp_buf , (unsigned char ) sql__setrang(ptoc_Var43->integer.C_1.mbp_buf[sql__setrang((int ) ptoc_Var40->co_startkeyfields + (int ) ptoc_Var40->co_minkeylen + -1 , 1 , 8192000) + -1] , 0 , 255) , (int ) ptoc_Var40->co_startkeyfields + (int ) ptoc_Var40->co_minkeylen , (int ) ptoc_Var42->integer.C_1.elen_var - (int ) ptoc_Var40->co_minkeylen) + (int ) ptoc_Var40->co_minkeylen) , -32768 , 32767);
                        }
                    else
                        {
                        (*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak505.p" , 15189)).integer.C_2.mtree = ptoc_Var40->co_corr_current;
                        }
                    _gg_strategy_ptr = (tgg07_StrategyInfo_a505most_execute *) &(*(tsp00_MoveObj *) sql__nil((*acv).a_mblock.integer.C_11.mb_strat , "vak505.p" , 15192))[sql__setrang((int ) (*(tgg00_StackList *) sql__nil((*acv).a_mblock.integer.C_5.mb_st , "vak505.p" , 15193))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak505.p" , 15193)).integer.C_2.mstrat_pos , 1 , 32767) + -1].integer.C_1.epos , 1 , 8192000) + -1];
                    (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15194)).str_foundresults = (*erec).ex_all_rescnt;
                    (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15195)).str_result_id = (*erec).ex_act_treeid;
                    if ((*old_cinfo).co_corr_buf != (tsp00_Buf *) (int *) 0)
                        {
                        g10mv5("VAK505" , 4 , sizeof(tsp00_Buf ) , (*acv).a_mblock.integer.C_2.mb_data_size , sql__nil((*old_cinfo).co_corr_buf , "vak505.p" , 15201) , (int ) (*old_cinfo).co_one_key_len + (int ) (*old_cinfo).co_act_pos + 8 , (*(tgg00_DataPart *) sql__nil((*acv).a_mblock.integer.C_3.mb_data , "vak505.p" , 15204)).integer.C_1.mbp_buf , 9 , (int ) (*old_cinfo).co_lcomp_len , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15207)).integer.C_4.sp1r_returncode);
                        }
                    }
                else
                    {
                    if ((int ) ptoc_Var40->co_use_fields == 2)
                        {
                        g10mv5("VAK505" , 5 , sizeof(tsp00_Buf ) , (*acv).a_mblock.integer.C_2.mb_data_size , sql__nil(ptoc_Var40->co_corr_buf , "vak505.p" , 15216) , (int ) ptoc_Var40->co_one_key_len + (int ) ptoc_Var40->co_act_pos + 8 , (*(tgg00_DataPart *) sql__nil((*acv).a_mblock.integer.C_3.mb_data , "vak505.p" , 15218)).integer.C_1.mbp_buf , 9 , (int ) ptoc_Var40->co_lcomp_len , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15221)).integer.C_4.sp1r_returncode);
                        }
                    }
                t01int4(5 , "co_one_key_l" , (int ) ptoc_Var40->co_one_key_len);
                t01int4(5 , "co_act_pos  " , (int ) ptoc_Var40->co_act_pos);
                t01int4(5 , "co_lcomp_len" , (int ) ptoc_Var40->co_lcomp_len);
                t01messblock(5 , "MBLOCK 505mo" , &(*acv).a_mblock);
                t01int4(5 , "act_pos     " , (int ) ptoc_Var40->co_act_pos);
                t01int4(5 , "startkeyfiel" , (int ) ptoc_Var40->co_startkeyfields);
                t01int4(5 , "one_key_len " , (int ) ptoc_Var40->co_one_key_len);
                if ((int ) (*cinfo).co_use_fields == 1)
                    {
                    t01int4(5 , "use_keyfiel " , 1);
                    }
                if ((int ) (*acv).a_mblock.integer.C_2.mb_type2 != 68)
                    {
                    _gg_strategy_ptr = (tgg07_StrategyInfo_a505most_execute *) &(*(tsp00_MoveObj *) sql__nil((*acv).a_mblock.integer.C_11.mb_strat , "vak505.p" , 15238))[sql__setrang((int ) (*(tgg00_StackList *) sql__nil((*acv).a_mblock.integer.C_5.mb_st , "vak505.p" , 15239))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak505.p" , 15239)).integer.C_2.mstrat_pos , 1 , 32767) + -1].integer.C_1.epos , 1 , 8192000) + -1];
                    (*erec).ex_act_treeid = (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15240)).str_result_id;
                    }
                else
                    {
                    g10mv13("VAK505" , 6 , sizeof(tsp00_Buf ) , sizeof(tgg00_FileId ) , (*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak505.p" , 15247)).integer.C_1.buf , 1 , &(*erec).ex_act_treeid , 1 , 40 , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15249)).integer.C_4.sp1r_returncode);
                    }
                }
            else
                {
                ptoc_Var44 = &(*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15255)).integer.C_23.sparsinfo;
                switch ((int ) (*acv).a_mblock.integer.C_2.mb_type)
                    {
                    case 40 :
                    case 72 :
                    case 9 :
                    case 56 :
                        if ((int ) (*acv).a_mblock.integer.C_2.mb_type == 40 && (char ) sql__setrang((*dmli).d_execute_unlock , 0 , 1))
                            {
                            (*acv).a_mblock.integer.C_2.mb_type = 72;
                            (*acv).a_max_intern_select = (short ) sql__setrang((*acv).a_intern_select_cnt , -32768 , 32767);
                            }
                        a506lock_fieldvalues(&(*acv) , &(*dmli) , &_frec , viewkeybuf);
                        break;
                    default :
                        _frec.fr_f_no = 1;
                        _frec.fr_last_fno = (short ) sql__setrang(ptoc_Var44->integer.C_2.p_cnt_infos , -32768 , 32767);
                        if ((int ) _frec.fr_last_fno >= (int ) _frec.fr_f_no)
                            {
                            if ((*erec).ex_updjv_ptr != (tsp00_MoveObj *) (int *) 0)
                                {
                                a506fieldvalues(&(*acv) , &(*dmli) , &_frec , viewkeybuf , sql__nil((*erec).ex_updjv_ptr , "vak505.p" , 15282) , sizeof(tsp00_Buf ));
                                }
                            else
                                {
                                a506fieldvalues(&(*acv) , &(*dmli) , &_frec , viewkeybuf , (*(tgg00_DataPart *) sql__nil((*acv).a_mblock.integer.C_3.mb_data , "vak505.p" , 15286)).integer.C_1.mbp_buf , (*acv).a_mblock.integer.C_2.mb_data_size);
                                }
                            }
                        break;
                    }
                }
            if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15295)).integer.C_4.sp1r_returncode == 0)
                {
                ptoc_Var45 = &(*acv).a_mblock;
                ptoc_Var46 = (tgg00_DataPart *) sql__nil(ptoc_Var45->integer.C_3.mb_data , "vak505.p" , 15298);
                _pmbp = (struct tgg00_MessBlock *) &(*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[sql__setrang((int ) (*dmli).d_sparr.pcount , 1 , 8) + -1] , "vak505.p" , 15300)).integer.C_62.smessblock.mbr_mess_block;
                _pdbp = (*(struct tgg00_MessBlock *) sql__nil(_pmbp , "vak505.p" , 15302)).integer.C_3.mb_data;
                t01int4(5 , "pos_in_parsb" , (int ) (*dmli).d_pos_in_parsbuf);
                t01int4(5 , "len         " , ptoc_Var45->integer.C_2.mb_data_len);
                t01int4(5 , "keylen      " , (int ) ptoc_Var46->integer.C_4.mbp_keylen);
                if ((*(struct tgg00_MessBlock *) sql__nil(_pmbp , "vak505.p" , 15306)).integer.C_2.mb_data_len >= (int ) (*dmli).d_pos_in_parsbuf)
                    {
                    _i_length = (*(struct tgg00_MessBlock *) sql__nil(_pmbp , "vak505.p" , 15309)).integer.C_2.mb_data_len - (int ) (*dmli).d_pos_in_parsbuf + 1;
                    t01int4(5 , "pos_in_parsb" , (int ) (*dmli).d_pos_in_parsbuf);
                    t01int4(5 , "i_length    " , _i_length);
                    t01int4(5 , "len         " , ptoc_Var45->integer.C_2.mb_data_len);
                    t01int4(5 , "keylen      " , (int ) ptoc_Var46->integer.C_4.mbp_keylen);
                    g10mv("VAK505" , 7 , (*(struct tgg00_MessBlock *) sql__nil(_pmbp , "vak505.p" , 15316)).integer.C_2.mb_data_size , (*acv).a_mblock.integer.C_2.mb_data_size , (*(tgg00_DataPart *) sql__nil(_pdbp , "vak505.p" , 15317)).integer.C_1.mbp_buf , (int ) (*dmli).d_pos_in_parsbuf , (*(tgg00_DataPart *) sql__nil((*acv).a_mblock.integer.C_3.mb_data , "vak505.p" , 15318)).integer.C_1.mbp_buf , ptoc_Var45->integer.C_2.mb_data_len + 1 , _i_length , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15320)).integer.C_4.sp1r_returncode);
                    ptoc_Var45->integer.C_2.mb_data_len = ptoc_Var45->integer.C_2.mb_data_len + _i_length;
                    (*dmli).d_pos_in_parsbuf = (short ) sql__setrang((short ) ((int ) (*dmli).d_pos_in_parsbuf + _i_length) , -32768 , 32767);
                    }
                if ((char ) sql__setrang((*acv).a_stored_proc_call , 0 , 1))
                    {
                    if ((char ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15331)).integer.C_23.sparsinfo.integer.C_2.p_const_param_expr , 0 , 1))
                        {
                        a505const_param_expression(&(*acv) , &(*dmli) , &(*acv).a_mblock);
                        }
                    t01messblock(5 , "MBLOCK 505mo" , &(*acv).a_mblock);
                    (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15337)).integer.C_4.sp1r_returncode = 21007;
                    }
                }
            if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15342)).integer.C_4.sp1r_returncode == 0)
                {
                (*acv).a_mblock.integer.C_9.mb_trns = (tgg00_TransContext *) &(*acv).a_transinf.tri_trans;
                _mtype = (unsigned char ) sql__setrang((*acv).a_mblock.integer.C_2.mb_type , 0 , 95);
                _m2type = (unsigned char ) sql__setrang((*acv).a_mblock.integer.C_2.mb_type2 , 0 , 95);
                _b_err = 0;
                if ((int ) _m2type == 43)
                    {
                    (*acv).a_mblock.integer.C_2.mb_type2 = 4;
                    }
                else
                    {
                    if ((int ) _m2type == 19 && (int ) _mtype == 18)
                        {
                        _m2type = 54;
                        }
                    }
                _qbufp = (struct tak_systembuffer *) (int *) 0;
                ptoc_Var47 = &(*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15359)).integer.C_23.sparsinfo;
                if ((char ) sql__setrang(ptoc_Var47->integer.C_2.p_select , 0 , 1) && (char ) sql__setrang(ptoc_Var47->integer.C_2.p_single , 0 , 1) && (char ) sql__setrang(_last_single_no_union , 0 , 1))
                    {
                    _h_rescount = (*dmli).d_mselect_rescnt;
                    (*dmli).d_mselect_rescnt = 0;
                    }
                if ((int ) _m2type == 68 && ((ptoc_Var48 = (int ) _mtype) == 60 || ptoc_Var48 == 63))
                    {
                    _munion_cnt = (*acv).a_union_cnt;
                    if ((char ) sql__setrang(_not_last , 0 , 1))
                        {
                        (*acv).a_union_cnt = 0;
                        }
                    _ssr_rec.ssr_old_strategynum = 7;
                    _ssr_rec.ssr_new_strategynum = 7;
                    _ssr_rec.ssr_invlen = 0;
                    _join_parsk = *parsk;
                    _join_parsk.p_no = (unsigned char ) sql__setrang((unsigned char ) (*erec).ex_plus , 0 , 255);
                    a682_only_ex_join(&(*acv) , &(*dmli) , &_join_parsk , &(*erec).ex_act_treeid , (char ) sql__setrang(_last_single_no_union , 0 , 1));
                    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15387)).integer.C_4.sp1r_returncode == (int ) a07_return_code(1420 , (unsigned char ) sql__setrang((*acv).a_sqlmode , 0 , 3)) && (char ) sql__setrang((*erec).ex_output_wanted , 0 , 1) && (char ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15390)).integer.C_23.sparsinfo.integer.C_2.p_select , 0 , 1) && (char ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15391)).integer.C_23.sparsinfo.integer.C_2.p_single , 0 , 1) && ((int ) (*dmli).d_mselect_loop_cnt != 0 || _h_rescount > 0))
                        {
                        (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15397)).integer.C_4.sp1r_returncode = 0;
                        _b_err = 1420;
                        }
                    (*acv).a_union_cnt = _munion_cnt;
                    (*acv).a_first_union = 0;
                    }
                else
                    {
                    if ((int ) _mtype == 40 || (int ) _mtype == 72 || (int ) _mtype == 9 || (int ) _mtype == 56)
                        {
                        ak505dist_lock_mess_bufs(&(*acv) , (char ) sql__setrang((int ) _m2type == 0 , 0 , 1));
                        }
                    else
                        {
                        if ((char ) sql__setrang(_last_single_no_union , 0 , 1) && ((int ) _mtype == 35 || (int ) _mtype == 36 || (int ) _mtype == 18 || (int ) _mtype == 76))
                            {
                            (*dmli).d_mselect_rescnt = -2;
                            }
                        if ((char ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15424)).integer.C_23.sparsinfo.integer.C_2.p_const_param_expr , 0 , 1))
                            {
                            a505const_param_expression(&(*acv) , &(*dmli) , &(*acv).a_mblock);
                            }
                        if (((ptoc_Var48 = (int ) _mtype) == 60 || ptoc_Var48 == 63) && ((int ) _m2type == 0 || (int ) _m2type == 67) || ((ptoc_Var48 = (int ) _m2type) == 54 || ptoc_Var48 == 90 || ptoc_Var48 == 19) && ((int ) _mtype == 76 || (int ) _mtype == 18) || (int ) _mtype == 36)
                            {
                            (*erec).ex_act_treeid.integer.C_8.fileTempCnt_gg00 = -1;
                            a505strategy_search(&(*acv) , &(*dmli) , (unsigned char ) sql__setrang(_mtype , 0 , 95) , _searchname , &(*erec).ex_act_treeid , &_ssr_rec , &_qbufp , &(*parsk) , (char ) sql__setrang(_not_last , 0 , 1));
                            if ((char ) sql__setrang(_last_single_no_union , 0 , 1) && ((ptoc_Var48 = (int ) _mtype) == 60 || ptoc_Var48 == 63))
                                {
                                t01int4(5 , "loop_cnt    " , (int ) (*dmli).d_mselect_loop_cnt);
                                if ((int ) (*dmli).d_mselect_loop_cnt == 0 || ((int ) (*dmli).d_mselect_loop_cnt == -2 || (int ) (*dmli).d_mselect_loop_cnt == -3) && !(char ) sql__setrang((*dmli).d_repl_reskey , 0 , 1))
                                    {
                                    }
                                else
                                    {
                                    _gg_strategy_ptr = (tgg07_StrategyInfo_a505most_execute *) &(*(tsp00_MoveObj *) sql__nil((*acv).a_mblock.integer.C_11.mb_strat , "vak505.p" , 15459))[sql__setrang((int ) (*(tgg00_StackList *) sql__nil((*acv).a_mblock.integer.C_5.mb_st , "vak505.p" , 15460))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak505.p" , 15460)).integer.C_2.mstrat_pos , 1 , 32767) + -1].integer.C_1.epos , 1 , 8192000) + -1];
                                    (_ptoc_MOVE(unsigned char , 2, (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15461)).str_result_id.integer.C_1.fileHandling_gg00, *(unsigned char (*)[2]) sql__setsub(ptoc_Var49 , (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15462)).str_result_id.integer.C_1.fileHandling_gg00 , ptoc_Var39 , sizeof(unsigned char [2]))) , (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15461)).str_result_id.integer.C_1.fileHandling_gg00);
                                    }
                                if ((*dmli).d_mselect_rescnt > 0)
                                    {
                                    _gg_strategy_ptr = (tgg07_StrategyInfo_a505most_execute *) &(*(tsp00_MoveObj *) sql__nil((*acv).a_mblock.integer.C_11.mb_strat , "vak505.p" , 15469))[sql__setrang((int ) (*(tgg00_StackList *) sql__nil((*acv).a_mblock.integer.C_5.mb_st , "vak505.p" , 15470))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak505.p" , 15470)).integer.C_2.mstrat_pos , 1 , 32767) + -1].integer.C_1.epos , 1 , 8192000) + -1];
                                    (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15471)).str_foundresults = (*dmli).d_mselect_rescnt;
                                    }
                                }
                            }
                        if ((char ) sql__setrang((*cinfo).co_use_cinfo , 0 , 1) && (*acv).a_union_cnt > 0 && ((int ) (*cinfo).co_use_fields == 1 && (char ) sql__setrang((*acv).a_rest_equal_second , 0 , 1) || (int ) (*cinfo).co_use_fields == 0 && !(char ) sql__setrang((*acv).a_rest_equal_second , 0 , 1) && (int ) (*erec).ex_plus + 1 > (int ) (*erec).ex_maxbufs))
                            {
                            a503build_union_buffer(&(*acv) , 0);
                            }
                        else
                            {
                            if ((*acv).a_union_cnt <= 1 || (int ) (*erec).ex_act_treeid.integer.C_8.fileTfnTemp_gg00 == 13 && (int ) (*erec).ex_act_treeid.integer.C_4.fileTfn_gg00 == 26)
                                {
                                (*acv).a_send_messbuf = 1;
                                if ((*acv).a_union_cnt == 1)
                                    {
                                    if ((char ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*acv).a_unionrec_ptr , "vak505.p" , 15499)).integer.C_40.sunionrec.uappend_all , 0 , 1))
                                        {
                                        _union_kind = (unsigned char ) sql__setrang((unsigned char ) ((int ) (*acv).a_union_kind + 10) , 0 , 255);
                                        _gg_strategy_ptr = (tgg07_StrategyInfo_a505most_execute *) &(*(tsp00_MoveObj *) sql__nil((*acv).a_mblock.integer.C_11.mb_strat , "vak505.p" , 15505))[sql__setrang((int ) (*(tgg00_StackList *) sql__nil((*acv).a_mblock.integer.C_5.mb_st , "vak505.p" , 15506))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak505.p" , 15506)).integer.C_2.mstrat_pos , 1 , 32767) + -1].integer.C_1.epos , 1 , 8192000) + -1];
                                        (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15508)).str_union_kind = (unsigned char ) sql__setrang(_union_kind , 0 , 255);
                                        }
                                    }
                                }
                            }
                        if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15516)).integer.C_4.sp1r_returncode == 0)
                            {
                            if ((int ) _mtype == 77 && (int ) _m2type != 22)
                                {
                                a59handle_qualification(&(*acv) , sql__nil((*erec).ex_updjv_ptr , "vak505.p" , 15522) , &_b_err);
                                }
                            else
                                {
                                if ((int ) _mtype == 64)
                                    {
                                    a26execute_comment(&(*acv) , &(*dmli));
                                    }
                                else
                                    {
                                    ptoc_Var52 = &(*acv).a_mblock;
                                    if ((int ) ptoc_Var52->integer.C_2.mb_type == 18 && (int ) ptoc_Var52->integer.C_2.mb_type2 == 19)
                                        {
                                        a10invalidate_root(&(*acv) , &(*(tgg00_QualBuf *) sql__nil(ptoc_Var52->integer.C_4.mb_qual , "vak505.p" , 15534)).integer.C_2.mtree);
                                        }
                                    _aux_treeid = (*(tgg00_QualBuf *) sql__nil(ptoc_Var52->integer.C_4.mb_qual , "vak505.p" , 15536)).integer.C_2.mtree;
                                    t01int4(5 , "a_union_cnt*" , (*acv).a_union_cnt);
                                    t01int4(5 , "a_first_unio" , sql__ord((int ) (char ) sql__setrang((*acv).a_first_union , 0 , 1)));
                                    t01int4(5 , "a_send_messb" , sql__ord((int ) (char ) sql__setrang((*acv).a_send_messbuf , 0 , 1)));
                                    if ((char ) sql__setrang((*acv).a_first_union , 0 , 1) || (*acv).a_union_cnt <= 1 || (char ) sql__setrang((*acv).a_send_messbuf , 0 , 1))
                                        {
                                        a06dml_send_mess_buf(&(*acv) , &(*acv).a_mblock , &(*dmli) , &_b_err);
                                        }
                                    }
                                }
                            }
                        }
                    }
                if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15557)).integer.C_4.sp1r_returncode == 0)
                    {
                    ptoc_Var53 = &(*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15559)).integer.C_23.sparsinfo;
                    t01treeid(5 , "after dml_se" , &(*erec).ex_act_treeid);
                    t01int4(5 , "p_select    " , sql__ord((int ) (char ) sql__setrang(ptoc_Var53->integer.C_2.p_select , 0 , 1)));
                    t01int4(5 , "p_single    " , sql__ord((int ) (char ) sql__setrang(ptoc_Var53->integer.C_2.p_single , 0 , 1)));
                    t01int4(5 , "last_single " , sql__ord((int ) (char ) sql__setrang(_last_single_no_union , 0 , 1)));
                    if ((char ) sql__setrang(ptoc_Var53->integer.C_2.p_select , 0 , 1) && !(char ) sql__setrang(ptoc_Var53->integer.C_2.p_single , 0 , 1) && (int ) (*erec).ex_act_treeid.integer.C_8.fileTfnTemp_gg00 == 4 && (int ) (*erec).ex_act_treeid.integer.C_4.fileTfn_gg00 == 26)
                        {
                        (*(struct tak_systembuffer *) sql__nil((*acv).a_resname_addr[0] , "vak505.p" , 15573)).integer.C_52.sresname.restreeid.integer.C_8.fileTempCnt_gg00 = (*acv).a_max_res_id;
                        }
                    }
                if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15580)).integer.C_4.sp1r_returncode == 0)
                    {
                    if ((int ) _b_err == 1420 && (char ) sql__setrang((*erec).ex_output_wanted , 0 , 1) && (char ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15585)).integer.C_23.sparsinfo.integer.C_2.p_select , 0 , 1) && (char ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15586)).integer.C_23.sparsinfo.integer.C_2.p_single , 0 , 1) && ((int ) (*dmli).d_mselect_loop_cnt != 0 || _h_rescount > 0))
                        {
                        _b_err = 0;
                        _i_length = (int ) (*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15592)).integer.C_23.sparsinfo.integer.C_2.p_resreclen;
                        g10fil("VAK505" , 8 , (*acv).a_mblock.integer.C_2.mb_data_size , (*(tgg00_DataPart *) sql__nil((*acv).a_mblock.integer.C_3.mb_data , "vak505.p" , 15595)).integer.C_1.mbp_buf , 1 , _i_length , (unsigned char)'\377' , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15597)).integer.C_4.sp1r_returncode);
                        (*(tgg00_DataPart *) sql__nil((*acv).a_mblock.integer.C_3.mb_data , "vak505.p" , 15598)).integer.C_4.mbp_reclen = (short ) sql__setrang((short ) _i_length , -32768 , 32767);
                        (*(tgg00_DataPart *) sql__nil((*acv).a_mblock.integer.C_3.mb_data , "vak505.p" , 15599)).integer.C_4.mbp_keylen = 1;
                        _ptoc_ASSIGN(unsigned char , 7, (*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak505.p" , 15600)).integer.C_4.mr_resnum, "\0\200\0\0\0\0\0")
                        }
                    if ((int ) _b_err != 0)
                        {
                        if ((int ) _m2type == 4 || (int ) _m2type == 43)
                            {
                            if ((int ) _b_err == 550)
                                {
                                _b_err = 0;
                                }
                            else
                                {
                                a07_b_put_error(&(*acv) , 6270 , 1);
                                }
                            }
                        else
                            {
                            if (((int ) _b_err == 1420 || (int ) _b_err == 1200) && (*acv).a_mblock.integer.C_2.mb_qual_len == 172 && (int ) _aux_treeid.integer.C_4.fileTfn_gg00 != 26 && (int ) _m2type != 68)
                                {
                                ptoc_Var54 = &(*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15626)).integer.C_23.sparsinfo;
                                if ((*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak505.p" , 15627)).integer.C_4.mr_pagecnt > 200 && (float ) (*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak505.p" , 15629)).integer.C_4.mr_pagecnt > (float ) ptoc_Var54->integer.C_2.p_pagecnt * (float ) 1.20000000000000000000e+000)
                                    {
                                    a28sys_upd_statistics(&(*acv) , &_aux_treeid , (unsigned char ) sql__setrang(_mtype , 0 , 95) , (unsigned char ) sql__setrang(_m2type , 0 , 95) , ptoc_Var54->integer.C_2.p_pagecnt , (*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak505.p" , 15634)).integer.C_4.mr_pagecnt);
                                    }
                                }
                            a07_b_put_error(&(*acv) , (short ) sql__setrang(_b_err , -32768 , 32767) , 1);
                            }
                        if (_qbufp != (struct tak_systembuffer *) (int *) 0)
                            {
                            a10key_del(&(*acv) , &_qbufp);
                            }
                        ptoc_Var55 = &(*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15645)).integer.C_23.sparsinfo;
                        if ((char ) sql__setrang(ptoc_Var55->integer.C_2.p_select , 0 , 1) && !(char ) sql__setrang(ptoc_Var55->integer.C_2.p_single , 0 , 1) && (char ) sql__setrang(_last_single_no_union , 0 , 1))
                            {
                            (*(struct tak_systembuffer *) sql__nil((*acv).a_resname_addr[sql__setrang(_pos , 1 , 3) + -1] , "vak505.p" , 15649)).integer.C_52.sresname.resexecuted = 0;
                            }
                        }
                    else
                        {
                        if ((int ) _m2type == 43)
                            {
                            a07_b_put_error(&(*acv) , 6268 , 1);
                            }
                        else
                            {
                            if ((int ) (*parsk).p_kind == 9 || (int ) (*parsk).p_kind == 56)
                                {
                                a52init_subtrans_of_new_trans(&(*acv));
                                }
                            else
                                {
                                if ((char ) sql__setrang(_last_single_no_union , 0 , 1))
                                    {
                                    if ((ptoc_Var48 = (int ) _mtype) == 60 || ptoc_Var48 == 63)
                                        {
                                        ptoc_Var56 = &(*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15672)).integer.C_23.sparsinfo;
                                        s40g4int((*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak505.p" , 15674)).integer.C_4.mr_resnum , 2 , &(*dmli).d_mselect_rescnt , &_re);
                                        if ((char ) sql__setrang(ptoc_Var56->integer.C_2.p_select , 0 , 1) && (char ) sql__setrang(ptoc_Var56->integer.C_2.p_single , 0 , 1))
                                            {
                                            (*dmli).d_mselect_rescnt = _h_rescount + (*dmli).d_mselect_rescnt;
                                            }
                                        }
                                    if ((char ) sql__setrang((*dmli).d_longdesc_found , 0 , 1))
                                        {
                                        ak505long_support(&(*acv) , &(*dmli) , (unsigned char ) sql__setrang(_mtype , 0 , 95));
                                        }
                                    if ((*dmli).d_literalrec != (struct tak_systembuffer *) (int *) 0)
                                        {
                                        a507_literal(&(*acv) , &(*(struct tak_systembuffer *) sql__nil((*dmli).d_literalrec , "vak505.p" , 15692)).integer.C_65.slongliteral);
                                        }
                                    ptoc_Var57 = &(*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15694)).integer.C_23.sparsinfo;
                                    t01int4(5 , "pagecnt     " , ptoc_Var57->integer.C_2.p_pagecnt);
                                    if ((int ) _aux_treeid.integer.C_4.fileTfn_gg00 != 26 && ((int ) _mtype == 60 && (int ) _m2type != 68 && (char ) sql__setrang((*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak505.p" , 15701)).integer.C_4.mr_res_build , 0 , 1) || (int ) _mtype == 61 && (int ) _m2type != 16 || (int ) _mtype == 36 || ((ptoc_Var48 = (int ) _mtype) == 76 || ptoc_Var48 == 18) && ((ptoc_Var48 = (int ) _m2type) == 54 || ptoc_Var48 == 90 || ptoc_Var48 == 19)))
                                        {
                                        ptoc_Var58 = &(*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15709)).integer.C_23.sparsinfo;
                                        if ((*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak505.p" , 15710)).integer.C_4.mr_pagecnt > 200 && (float ) (*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak505.p" , 15712)).integer.C_4.mr_pagecnt > (float ) ptoc_Var58->integer.C_2.p_pagecnt * (float ) 1.20000000000000000000e+000)
                                            {
                                            a28sys_upd_statistics(&(*acv) , &_aux_treeid , (unsigned char ) sql__setrang(_mtype , 0 , 95) , (unsigned char ) sql__setrang(_m2type , 0 , 95) , ptoc_Var58->integer.C_2.p_pagecnt , (*(tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak505.p" , 15717)).integer.C_4.mr_pagecnt);
                                            }
                                        }
                                    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15721)).integer.C_4.sp1r_function_code != 27)
                                        {
                                        if ((int ) _mtype != 60 || (int ) (*dmli).d_mselect_loop_cnt == 1 || (int ) (*dmli).d_mselect_loop_cnt == 0 || (int ) (*dmli).d_mselect_loop_cnt == -3 || (int ) (*dmli).d_mselect_loop_cnt == -1)
                                            {
                                            a507last_command_part(&(*acv) , &(*dmli) , &(*change_rec) , &(*parsk) , _resnam , &_w_link , _qbufp , &_ssr_rec , _pos , (unsigned char ) sql__setrang(_mtype , 0 , 95) , (unsigned char ) sql__setrang(_m2type , 0 , 95) , (char ) sql__setrang((*erec).ex_output_wanted , 0 , 1));
                                            }
                                        else
                                            {
                                            if ((char ) sql__setrang((*erec).ex_output_wanted , 0 , 1) && (char ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15736)).integer.C_23.sparsinfo.integer.C_2.p_select , 0 , 1) && (char ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15737)).integer.C_23.sparsinfo.integer.C_2.p_single , 0 , 1))
                                                {
                                                t01int4(5 , "MSELECT INTO" , (int ) (*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15742)).integer.C_23.sparsinfo.integer.C_2.p_resinfolen);
                                                (*acv).a_long_desc_pos = 0;
                                                if ((int ) (*change_rec).cr_colcount > 0)
                                                    {
                                                    a60_change_results(&(*acv) , (*(tgg00_DataPart *) sql__nil((*acv).a_mblock.integer.C_3.mb_data , "vak505.p" , 15746)).integer.C_1.mbp_buf , &(*change_rec) , 0 , (*acv).a_mblock.integer.C_2.mb_data_len);
                                                    }
                                                _pos = (int ) (*(tgg00_DataPart *) sql__nil((*acv).a_mblock.integer.C_3.mb_data , "vak505.p" , 15750)).integer.C_4.mbp_keylen + 9;
                                                a06retpart_move(&(*acv) , (tsp00_MoveObj *) &(*(tgg00_DataPart *) sql__nil((*acv).a_mblock.integer.C_3.mb_data , "vak505.p" , 15751)).integer.C_1.mbp_buf[sql__setrang(_pos , 1 , 8192000) + -1] , (int ) (*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15752)).integer.C_23.sparsinfo.integer.C_2.p_resinfolen);
                                                if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15753)).integer.C_4.sp1r_returncode == 0 && ((int ) (*dmli).d_mselect_loop_cnt == 1 || (int ) (*dmli).d_mselect_loop_cnt == 0 || (int ) (*dmli).d_mselect_loop_cnt == -3 || (int ) (*dmli).d_mselect_loop_cnt == -1))
                                                    {
                                                    a06finish_curr_retpart(&(*acv) , 5 , (short ) sql__setrang((short ) ((*(tsp1_part *) sql__nil((*acv).a_curr_retpart , "vak505.p" , 15760)).integer.C_2.sp1p_buf_len / (int ) (*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 15761)).integer.C_23.sparsinfo.integer.C_2.p_resinfolen) , -32768 , 32767));
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    (*erec).ex_with_link = (char ) sql__setrang(_w_link , 0 , 1);
    cccprint("<a505most_execut");
    }



int a505next_command(acv, sparr, old_parsk, ex_plus)
tak_all_command_glob *acv;
tak_syspointerarr *sparr;
tak_parskey *old_parsk;
int ex_plus;
    {
    tgg_basis_error b_err;
    tgg_sysinfokey ke;
    tak_all_command_glob *ptoc_Var59;
    tgg00_SysInfoKey *ptoc_Var60;
    int ptoc_Var61;
    cccprint("AK505 a505next_c");
    ptoc_Var59 = &*acv;
    ke = a01sysnullkey;
    ptoc_Var60 = &ke;
    ptoc_Var60->integer.C_3.sauthid[0] = (unsigned char)'\377';
    s10mv1(sizeof(tak_parskey ) , sizeof(tsp00_C8 ) , &(*old_parsk) , 1 , ptoc_Var60->integer.C_3.sauthid , 2 , 6);
    ptoc_Var60->integer.C_3.sauthid[6] = (unsigned char ) sql__setrang((unsigned char ) ex_plus , 0 , 255);
    _ptoc_ASSIGN(unsigned char , 2, ptoc_Var60->integer.C_2.sentrytyp, "\0\201")
    a10get_sysinfo(&(*acv) , &ke , 0 , &(*sparr).px[0] , &b_err);
    if ((int ) b_err != 0)
        {
        if ((int ) b_err == 1930)
            {
            a07_b_put_error(&(*acv) , 1500 , (*(tsp1_part *) sql__nil(ptoc_Var59->a_cmd_part , "vak505.p" , 15815)).integer.C_2.sp1p_buf_len);
            }
        else
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , (*(tsp1_part *) sql__nil(ptoc_Var59->a_cmd_part , "vak505.p" , 15817)).integer.C_2.sp1p_buf_len);
            }
        }
    else
        {
        t01buf(5 , sql__nil((*sparr).px[0] , "vak505.p" , 15821) , 1 , (int ) (*(struct tak_systembuffer *) sql__nil((*sparr).px[0] , "vak505.p" , 15821)).integer.C_0.b_sl);
        _ptoc_ASSIGN(unsigned char , 64, (*acv).a_modul_name, (*(struct tak_systembuffer *) sql__nil((*sparr).px[0] , "vak505.p" , 15822)).integer.C_23.sparsinfo.integer.C_2.p_modul_name)
        if ((int ) (*acv).a_statement_kind == 28 && (int ) (*(struct tak_systembuffer *) sql__nil((*sparr).px[0] , "vak505.p" , 15824)).integer.C_23.sparsinfo.integer.C_2.p_statement_kind != 86)
            {
            (*acv).a_statement_kind = (unsigned char ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*sparr).px[0] , "vak505.p" , 15828)).integer.C_23.sparsinfo.integer.C_2.p_statement_kind , 0 , 176);
            }
        if (((ptoc_Var61 = (int ) (*acv).a_precomp_info_byte) == 70 || ptoc_Var61 == 114 || ptoc_Var61 == 115 || ptoc_Var61 == 116 || ptoc_Var61 == 117) && (int ) (*acv).a_precomp_info_byte != (int ) (*(struct tak_systembuffer *) sql__nil((*sparr).px[0] , "vak505.p" , 15837)).integer.C_23.sparsinfo.integer.C_2.p_precomp_info_byte && (int ) (*(struct tak_systembuffer *) sql__nil((*sparr).px[0] , "vak505.p" , 15838)).integer.C_23.sparsinfo.integer.C_2.p_precomp_info_byte != 0)
            {
            a07_b_put_error(&(*acv) , 6292 , 11);
            }
        (*acv).a_sqlmode = (unsigned char ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*sparr).px[0] , "vak505.p" , 15844)).integer.C_23.sparsinfo.integer.C_2.p_sqlmode , 0 , 3);
        t01int4(5 , "function_cod" , (int ) (*(struct tak_systembuffer *) sql__nil((*sparr).px[0] , "vak505.p" , 15846)).integer.C_23.sparsinfo.integer.C_2.p_function_code);
        (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15847)).integer.C_4.sp1r_function_code = (short ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*sparr).px[0] , "vak505.p" , 15848)).integer.C_23.sparsinfo.integer.C_2.p_function_code , -32768 , 32767);
        if ((int ) (*acv).a_precomp_info_byte != 70 && ((ptoc_Var61 = (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15850)).integer.C_4.sp1r_function_code) == 1003 || ptoc_Var61 == 1005 || ptoc_Var61 == 1009))
            {
            (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15854)).integer.C_4.sp1r_function_code = (short ) sql__setrang((short ) ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15855)).integer.C_4.sp1r_function_code + -1000) , -32768 , 32767);
            }
        (*sparr).pcount = 1;
        if ((char ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*sparr).px[0] , "vak505.p" , 15860)).integer.C_23.sparsinfo.integer.C_2.p_parsing_again , 0 , 1))
            {
            ptoc_Var59->a_parsing_again = 1;
            (*(struct tak_systembuffer *) sql__nil((*sparr).px[0] , "vak505.p" , 15864)).integer.C_23.sparsinfo.integer.C_2.p_parsing_again = 0;
            a10repl_sysinfo(&(*acv) , &(*sparr).px[0] , &b_err);
            }
        }
    cccprint("<a505next_comman");
    }

typedef
  struct ptoc_Type185 tgg07_StrategyInfo_a505strategy_search;


int a505strategy_search(acv, dmli, mtype, searchname, act_res_tree, ssr_rec, qbufp, parsk, not_last)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tgg_message_type mtype;
tsp_knl_identifier searchname;
tgg00_FileId *act_res_tree;
tgg07_selectstrat_rec *ssr_rec;
tak_sysbufferaddress *qbufp;
tak_parskey *parsk;
char not_last;
    {
    char _finding_possible;
    tsp_int2 _StratInfo_len;
    tsp_int4 _ix;
    tsp_int4 _jx;
    tsp_int4 _prev;
    tgg07_StrategyInfo_a505strategy_search *_gg_strategy_ptr;
    tsp_int4 _old_strat_len;
    tsp_int_map_c2 _ic2;
    tgg_basis_error _b_err;
    tgg_sysinfokey _ke;
    tgg07_StrategyInfo_a505strategy_search _gg_strategy;
    struct tgg00_MessBlock *ptoc_Var62;
    tgg00_QualBuf *ptoc_Var63;
    tgg00_StackDesc *ptoc_Var64;
    int ptoc_Var65;
    int ptoc_Var66;
    int ptoc_Var67;
    struct tgg00_MessBlock *ptoc_Var68;
    cccprint("AK505 a505strate");
    ptoc_Var62 = &(*acv).a_mblock;
    ptoc_Var63 = (tgg00_QualBuf *) sql__nil(ptoc_Var62->integer.C_4.mb_qual , "vak505.p" , 15899);
    ptoc_Var64 = &ptoc_Var63->integer.C_3.mstack_desc;
    _gg_strategy_ptr = (tgg07_StrategyInfo_a505strategy_search *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var62->integer.C_11.mb_strat , "vak505.p" , 15901))[sql__setrang((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var62->integer.C_5.mb_st , "vak505.p" , 15901))[sql__setrang((int ) ptoc_Var64->mstrat_pos , 1 , 32767) + -1].integer.C_1.epos , 1 , 8192000) + -1];
    if ((int ) ptoc_Var63->integer.C_2.mtree.integer.C_4.fileTfn_gg00 == 26)
        {
        if ((int ) ptoc_Var63->integer.C_2.mtree.integer.C_8.fileTfnTemp_gg00 == 4)
            {
            a505check_if_executed(&(*acv) , searchname , &ptoc_Var63->integer.C_2.mtree);
            }
        else
            {
            if ((ptoc_Var65 = (int ) (*acv).a_recursive_state) != 0 && ptoc_Var65 != 2 && (int ) ptoc_Var63->integer.C_2.mtree.integer.C_4.fileTfn_gg00 == 26 && (int ) ptoc_Var63->integer.C_2.mtree.integer.C_8.fileTfnTemp_gg00 == 37)
                {
                _ic2.boolean.C_true.map_int = (short ) sql__setrang((short ) ((int ) (*acv).a_recursive_no + -1) , -32768 , 32767);
                _ptoc_ASSIGN(unsigned char , 2, ptoc_Var63->integer.C_2.mtree.integer.C_8.fileLevel_gg00, _ic2.boolean.C_false.map_c2)
                }
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 15920)).integer.C_4.sp1r_returncode == 0)
        {
        if (!(char ) sql__setrang(not_last , 0 , 1) && (*acv).a_union_cnt > 0)
            {
            a503build_union_buffer(&(*acv) , 0);
            t01messblock(5 , "MBLOCK 505st" , &(*acv).a_mblock);
            }
        t01treeid(5 , "act_res_tree" , &(*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15930)).str_result_id);
        (*acv).a_from_select = (char ) sql__setrang((char ) sql__setrang((*acv).a_from_select , 0 , 1) || (int ) (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15932)).str_result_id.integer.C_8.fileTfnTemp_gg00 == 3 && (int ) (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15934)).str_result_id.integer.C_4.fileTfn_gg00 == 26 , 0 , 1);
        if ((int ) (*acv).a_init_ddl == 18 || (int ) (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15937)).str_result_id.integer.C_4.fileTfn_gg00 == 26)
            {
            if ((int ) (*acv).a_init_ddl == 18)
                {
                _ptoc_ASSIGN(unsigned char , 2, (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15943)).str_result_id.integer.C_1.fileVersion_gg00.ci2_gg00, "\377\377")
                }
            if ((int ) (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15946)).str_result_id.integer.C_4.fileTfn_gg00 == 26 && ((int ) (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15948)).str_result_id.integer.C_8.fileTfnTemp_gg00 == 4 || (int ) (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15950)).str_result_id.integer.C_8.fileTfnTemp_gg00 == 5 && (int ) (*acv).a_is_ddl == 18))
                {
                (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15954)).str_result_id.integer.C_8.fileTempCnt_gg00 = (*acv).a_max_res_id;
                }
            }
        if ((int ) (*acv).a_recursive_state == 3 && (int ) (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15960)).str_result_id.integer.C_4.fileTfn_gg00 == 26 && (int ) (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15961)).str_result_id.integer.C_8.fileTfnTemp_gg00 == 37)
            {
            _ic2.boolean.C_true.map_int = (short ) sql__setrang((*acv).a_recursive_no , -32768 , 32767);
            _ptoc_ASSIGN(unsigned char , 2, (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15966)).str_result_id.integer.C_8.fileLevel_gg00, _ic2.boolean.C_false.map_c2)
            }
        (*ssr_rec).ssr_old_strategynum = (unsigned char ) sql__setrang((*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15970)).str_strategy , 0 , 22);
        t01messblock(5 , "MBLOCK strat" , &(*acv).a_mblock);
        if ((ptoc_Var65 = (int ) (*acv).a_precomp_info_byte) == 114 || ptoc_Var65 == 115 || ptoc_Var65 == 116 || ptoc_Var65 == 117)
            {
            a71turn_into_build_strat(&(*acv) , &(*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15977)).str_strategy);
            (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15978)).str_build_result = 1;
            }
        if (((ptoc_Var65 = (int ) (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15982)).str_strategy) == 0 || ptoc_Var65 == 1) && (char ) sql__setrang((*dmli).d_only_table_scan , 0 , 1))
            {
            (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15988)).str_strategy = 8;
            (*ssr_rec).ssr_old_strategynum = (unsigned char ) sql__setrang((*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15989)).str_strategy , 0 , 22);
            }
        if ((int ) (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 15992)).str_strategy == 8)
            {
            g10mv8("VAK505" , 9 , ptoc_Var62->integer.C_2.mb_strat_size , sizeof(tgg07_StrategyInfo ) , sql__nil(ptoc_Var62->integer.C_11.mb_strat , "vak505.p" , 15998) , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var62->integer.C_5.mb_st , "vak505.p" , 15999))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var62->integer.C_4.mb_qual , "vak505.p" , 15999)).integer.C_2.mstrat_pos , 1 , 32767) + -1].integer.C_1.epos , &_gg_strategy , 1 , 76 , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 16002)).integer.C_4.sp1r_returncode);
            _ix = 0;
            do
                {
                _gg_strategy.tgg07_StratEnum.C_strat_key_equal.str_key_in_range.skir_keystart[sql__setrang(_ix , 0 , 19)] = 0;
                _gg_strategy.tgg07_StratEnum.C_strat_key_equal.str_key_in_range.skir_keystop[sql__setrang(_ix , 0 , 19)] = 0;
                if (_ix == 19)
                    {
                    break;
                    }
                _ix += 1;
                }
            while (!0);
            a721strategy(&(*acv) , &(*dmli) , &_gg_strategy , &_StratInfo_len , &(*parsk));
            if ((int ) _StratInfo_len > sizeof(tgg07_StrategyInfo ))
                {
                _StratInfo_len = (short ) sql__setrang((short ) sizeof(tgg07_StrategyInfo ) , -32768 , 32767);
                }
            if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 16015)).integer.C_4.sp1r_returncode == 0)
                {
                g10mv6("VAK505" , 10 , sizeof(tgg07_StrategyInfo ) , ptoc_Var62->integer.C_2.mb_strat_size , &_gg_strategy , 1 , sql__nil(ptoc_Var62->integer.C_11.mb_strat , "vak505.p" , 16021) , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var62->integer.C_5.mb_st , "vak505.p" , 16021))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var62->integer.C_4.mb_qual , "vak505.p" , 16021)).integer.C_2.mstrat_pos , 1 , 32767) + -1].integer.C_1.epos , (int ) _StratInfo_len , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 16023)).integer.C_4.sp1r_returncode);
                ptoc_Var62->integer.C_2.mb_strat_len = (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var62->integer.C_5.mb_st , "vak505.p" , 16024))[sql__setrang((int ) ptoc_Var64->mstrat_pos , 1 , 32767) + -1].integer.C_1.epos + (int ) _StratInfo_len + -1;
                (*(tgg00_StackList *) sql__nil(ptoc_Var62->integer.C_5.mb_st , "vak505.p" , 16025))[sql__setrang((int ) ptoc_Var64->mstrat_pos , 1 , 32767) + -1].integer.C_1.elen_var = (short ) sql__setrang(_StratInfo_len , -32768 , 32767);
                if ((int ) mtype != 60 || (int ) (*dmli).d_mselect_loop_cnt == 1 || (int ) (*dmli).d_mselect_loop_cnt == 0 || (int ) (*dmli).d_mselect_loop_cnt == -3 || (int ) (*dmli).d_mselect_loop_cnt == -1)
                    {
                    a67_invkeylen(&(*acv) , &_gg_strategy , &(*ssr_rec).ssr_invlen);
                    }
                else
                    {
                    (*ssr_rec).ssr_invlen = 0;
                    }
                }
            }
        if ((int ) (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 16040)).str_qual_kind != 1 && ((int ) (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 16042)).str_strategy >= 0 && (int ) (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 16042)).str_strategy <= 22 && (a70glob_inv_strats[(int ) (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 16042)).str_strategy >> 3] & 1 << ((int ) (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 16042)).str_strategy & 7)) != 0) && (!(char ) sql__setrang((*dmli).d_join , 0 , 1) && !(char ) sql__setrang((*dmli).d_view , 0 , 1)))
            {
            a71prepare_qual_on_index(&(*acv) , &(*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 16046)).str_qual_kind);
            }
        (*dmli).d_qual_kind = (unsigned char ) sql__setrang((*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 16048)).str_qual_kind , 0 , 2);
        t01qual_kind(5 , "d_qual_kind " , (unsigned char ) sql__setrang((*dmli).d_qual_kind , 0 , 2));
        (*ssr_rec).ssr_new_strategynum = (unsigned char ) sql__setrang((*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 16050)).str_strategy , 0 , 22);
        if ((int ) mtype == 60)
            {
            if ((char ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 16054)).integer.C_23.sparsinfo.integer.C_2.p_select , 0 , 1) && !(char ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 16055)).integer.C_23.sparsinfo.integer.C_2.p_single , 0 , 1) && ((int ) (*ssr_rec).ssr_new_strategynum >= 0 && (int ) (*ssr_rec).ssr_new_strategynum <= 22 && (a70glob_fetch_strats[(int ) (*ssr_rec).ssr_new_strategynum >> 3] & 1 << ((int ) (*ssr_rec).ssr_new_strategynum & 7)) != 0) && !(char ) sql__setrang(not_last , 0 , 1))
                {
                if (a507optim_select_fetch(&(*acv) , (char ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 16061)).integer.C_23.sparsinfo.integer.C_2.p_prepare , 0 , 1) , (unsigned char ) sql__setrang((*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 16062)).integer.C_23.sparsinfo.integer.C_2.p_precomp_info_byte , 0 , 255) , (int ) (*dmli).d_mselect_loop_cnt))
                    {
                    (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 16067)).str_search_first = 0;
                    }
                g04spec_null_check(&(*acv).a_mblock , &_b_err);
                if ((int ) _b_err != 0)
                    {
                    a07_b_put_error(&(*acv) , (short ) sql__setrang(_b_err , -32768 , 32767) , 1);
                    }
                else
                    {
                    g04inbetween_change(&(*acv).a_mblock , &_finding_possible);
                    (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 16077)).str_res_possible = (char ) sql__setrang(_finding_possible , 0 , 1);
                    _ke = a01sysnullkey;
                    _ke.integer.C_4.stempid = (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 16079)).str_result_id.integer.C_8.fileTempCnt_gg00;
                    _ptoc_ASSIGN(unsigned char , 2, _ke.integer.C_2.sentrytyp, "\0\301")
                    ptoc_Var62->integer.C_2.mb_qual_len = 112;
                    if ((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var62->integer.C_4.mb_qual , "vak505.p" , 16083)).integer.C_2.mqual_cnt > 0 && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var62->integer.C_5.mb_st , "vak505.p" , 16084))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var62->integer.C_4.mb_qual , "vak505.p" , 16084)).integer.C_2.mqual_pos , 1 , 32767) + -1].integer.C_1.etype == 33)
                        {
                        t01int4(7 , "str_key_len " , (int ) (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 16088)).str_key_len);
                        _prev = (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var62->integer.C_4.mb_qual , "vak505.p" , 16089)).integer.C_2.mqual_pos + 1;
                        ptoc_Var65 = (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var62->integer.C_4.mb_qual , "vak505.p" , 16090)).integer.C_2.mqual_pos + 1;
                        ptoc_Var66 = (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var62->integer.C_4.mb_qual , "vak505.p" , 16091)).integer.C_2.mqual_pos + (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var62->integer.C_5.mb_st , "vak505.p" , 16092))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var62->integer.C_4.mb_qual , "vak505.p" , 16092)).integer.C_2.mqual_pos , 1 , 32767) + -1].integer.C_1.epos + -1;
                        if (ptoc_Var65 <= ptoc_Var66)
                            {
                            _ix = ptoc_Var65;
                            do
                                {
                                if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var62->integer.C_5.mb_st , "vak505.p" , 16093))[sql__setrang(_ix , 1 , 32767) + -1].integer.C_1.etype == 28)
                                    {
                                    if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var62->integer.C_5.mb_st , "vak505.p" , 16096))[sql__setrang(_ix , 1 , 32767) + -1].integer.C_1.epos <= (int ) (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 16097)).str_key_len + 8)
                                        {
                                        ptoc_Var65 = _ix;
                                        ptoc_Var67 = _prev;
                                        if (ptoc_Var65 >= ptoc_Var67)
                                            {
                                            _jx = ptoc_Var65;
                                            do
                                                {
                                                (*(tgg00_StackList *) sql__nil(ptoc_Var62->integer.C_5.mb_st , "vak505.p" , 16100))[sql__setrang(_jx , 1 , 32767) + -1].integer.C_1.etype = 39;
                                                if (_jx == ptoc_Var67)
                                                    {
                                                    break;
                                                    }
                                                _jx -= 1;
                                                }
                                            while (!0);
                                            }
                                        }
                                    else
                                        {
                                        if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var62->integer.C_5.mb_st , "vak505.p" , 16103))[sql__setrang(_ix , 1 , 32767) + -1].integer.C_3.eop_out == 4 && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var62->integer.C_5.mb_st , "vak505.p" , 16105))[sql__setrang(_ix + 1 , 1 , 32767) + -1].integer.C_1.epos <= (int ) (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 16106)).str_key_len + 8)
                                            {
                                            (*(tgg00_StackList *) sql__nil(ptoc_Var62->integer.C_5.mb_st , "vak505.p" , 16108))[sql__setrang(_ix , 1 , 32767) + -1].integer.C_3.eop_out = 0;
                                            }
                                        }
                                    _prev = _ix + 1;
                                    }
                                if (_ix == ptoc_Var66)
                                    {
                                    break;
                                    }
                                _ix += 1;
                                }
                            while (!0);
                            }
                        (*(tgg00_StackList *) sql__nil(ptoc_Var62->integer.C_5.mb_st , "vak505.p" , 16115))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var62->integer.C_4.mb_qual , "vak505.p" , 16115)).integer.C_2.mqual_pos , 1 , 32767) + -1].integer.C_1.elen_var = (short ) sql__setrang((short ) ((int ) (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 16117)).str_key_len + 9) , -32768 , 32767);
                        t01int4(5 , "output_offse" , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var62->integer.C_5.mb_st , "vak505.p" , 16119))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var62->integer.C_4.mb_qual , "vak505.p" , 16119)).integer.C_2.mqual_pos , 1 , 32767) + -1].integer.C_1.elen_var);
                        }
                    g04mblock_optimize_info(&(*acv).a_mblock);
                    _old_strat_len = ptoc_Var62->integer.C_2.mb_strat_len;
                    ptoc_Var62->integer.C_2.mb_strat_len = (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var62->integer.C_5.mb_st , "vak505.p" , 16124))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var62->integer.C_4.mb_qual , "vak505.p" , 16124)).integer.C_2.mstrat_pos , 1 , 32767) + -1].integer.C_1.epos + -1;
                    a10mblock_into_cache(&(*acv) , &_ke , &(*acv).a_mblock , 0 , &(*qbufp) , &_b_err);
                    ptoc_Var62->integer.C_2.mb_strat_len = _old_strat_len;
                    if ((int ) _b_err == 0)
                        {
                        ptoc_Var68 = &(*(struct tak_systembuffer *) sql__nil(*qbufp , "vak505.p" , 16130)).integer.C_62.smessblock.mbr_mess_block;
                        (*(tgg00_StackList *) sql__nil(ptoc_Var68->integer.C_5.mb_st , "vak505.p" , 16131))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var68->integer.C_4.mb_qual , "vak505.p" , 16131)).integer.C_2.mstrat_pos , 1 , 32767) + -1].integer.C_1.etype = 39;
                        }
                    else
                        {
                        a07_b_put_error(&(*acv) , (short ) sql__setrang(_b_err , -32768 , 32767) , 1);
                        }
                    }
                }
            }
        *act_res_tree = (*(tgg07_StrategyInfo *) sql__nil(_gg_strategy_ptr , "vak505.p" , 16143)).str_result_id;
        }
    cccprint("<a505strategy_se");
    }



int ak505calculate_param_expression(acv, mblock, result_buf, len, index, enough_place)
tak_all_command_glob *acv;
tgg_mess_block *mblock;
tsp_buf result_buf;
int *len;
int index;
char *enough_place;
    {
    int i;
    tsp_int2 cal_begin;
    tsp_int2 cal_end;
    tsp_int2 param_pos;
    tsp_int2 part2_free;
    tgg_stack_entry restore_st;
    struct tgg00_MessBlock *ptoc_Var69;
    tgg00_QualBuf *ptoc_Var70;
    tgg00_StackEntry *ptoc_Var71;
    cccprint("AK505 ak505calcu");
    ptoc_Var69 = &*mblock;
    ptoc_Var70 = (tgg00_QualBuf *) sql__nil(ptoc_Var69->integer.C_4.mb_qual , "vak505.p" , 16168);
    cal_begin = (short ) sql__setrang((short ) (index + 1) , -32768 , 32767);
    cal_end = (short ) sql__setrang((short ) (index + sql__ord((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak505.p" , 16171))[sql__setrang(index , 1 , 32767) + -1].integer.C_1.ecol_tab[1]) + 1) , -32768 , 32767);
    t01int4(5 , "cal_begin   " , (int ) cal_begin);
    t01int4(5 , "cal_end     " , (int ) cal_end);
    restore_st = (*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak505.p" , 16174))[sql__setrang((int ) cal_end , 1 , 32767) + -1];
    ptoc_Var70->integer.C_2.mqual_pos = (short ) sql__setrang(cal_begin , -32768 , 32767);
    ptoc_Var70->integer.C_2.mqual_cnt = (short ) sql__setrang((short ) ((int ) cal_end - (int ) cal_begin + 1) , -32768 , 32767);
    ptoc_Var71 = &(*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak505.p" , 16177))[sql__setrang((int ) cal_end , 1 , 32767) + -1];
    ptoc_Var71->integer.C_1.etype = 28;
    ptoc_Var71->integer.C_3.eop_out = 12;
    ptoc_Var71->integer.C_1.epos = 1;
    ptoc_Var71->integer.C_1.elen_var = 255;
    ptoc_Var71->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
    ptoc_Var71->integer.C_1.ecol_tab[1] = (unsigned char)'\001';
    (*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak505.p" , 16187))[sql__setrang(index , 1 , 32767) + -1].integer.C_1.ecol_tab[1] = (unsigned char)'\0';
    (*mblock).integer.C_6.mb_work_st = (*acv).a_work_st_addr;
    (*mblock).integer.C_2.mb_work_st_max = (short ) sql__setrang((*acv).a_work_st_max , -32768 , 32767);
    (*mblock).integer.C_7.mb_workbuf = (*acv).a_work_buf_addr;
    (*mblock).integer.C_2.mb_workbuf_size = (*acv).a_work_buf_size;
    a05qualification_test(&(*acv) , &(*mblock) , 1 , 1 , (*(tsp1_part *) sql__nil((*acv).a_cmd_part , "vak505.p" , 16194)).integer.C_2.sp1p_buf_len , &(*(tgg00_DataPart *) sql__nil(ptoc_Var69->integer.C_3.mb_data , "vak505.p" , 16195)).integer.C_3.mbp_rec , result_buf , sizeof(tsp00_Buf ) , &(*len));
    if ((int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 16196)).integer.C_4.sp1r_returncode != 0)
        {
        _ptoc_ASSIGN(unsigned char , 2, (*acv).a_transinf.tri_trans.trState_gg00, (*(tgg00_TransContext *) sql__nil((*mblock).integer.C_9.mb_trns , "vak505.p" , 16199)).trState_gg00)
        }
    part2_free = 0;
    i = sql__succ(index , -2147483647 , 2147483647 , "vak505.p" , 16202);
    param_pos = (short ) sql__setrang((*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak505.p" , 16203))[sql__setrang(index , 1 , 32767) + -1].integer.C_1.epos , -32768 , 32767);
    while (i < (int ) cal_end && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak505.p" , 16205))[sql__setrang(i , 1 , 32767) + -1].integer.C_1.etype == 22 && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak505.p" , 16206))[sql__setrang(i , 1 , 32767) + -1].integer.C_1.epos != (int ) param_pos)
        i = sql__succ(i , -2147483647 , 2147483647 , "vak505.p" , 16207);
    while (i < (int ) cal_end)
        {
        if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak505.p" , 16211))[sql__setrang(i , 1 , 32767) + -1].integer.C_1.etype == 22)
            {
            part2_free = (short ) sql__setrang((short ) ((int ) part2_free + (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak505.p" , 16213))[sql__setrang(i , 1 , 32767) + -1].integer.C_1.elen_var) , -32768 , 32767);
            }
        i = sql__succ(i , -2147483647 , 2147483647 , "vak505.p" , 16215);
        }
    t01int4(5 , "result len  " , *len);
    t01int4(5 , "free part2  " , (int ) part2_free);
    (*(tgg00_StackList *) sql__nil(ptoc_Var69->integer.C_5.mb_st , "vak505.p" , 16220))[sql__setrang((int ) cal_end , 1 , 32767) + -1] = restore_st;
    *enough_place = (char ) sql__setrang(*len <= (int ) part2_free , 0 , 1);
    cccprint("<ak505calculate_");
    }



int ak505st_code_jump(mblock, index, count)
tgg_mess_block *mblock;
tsp_int2 index;
tsp_int2 count;
    {
    int i;
    struct tgg00_MessBlock *ptoc_Var72;
    tgg00_QualBuf *ptoc_Var73;
    tgg00_StackEntry *ptoc_Var74;
    int ptoc_Var75;
    tgg00_StackEntry *ptoc_Var76;
    cccprint("AK505 ak505st_co");
    ptoc_Var72 = &*mblock;
    ptoc_Var73 = (tgg00_QualBuf *) sql__nil(ptoc_Var72->integer.C_4.mb_qual , "vak505.p" , 16239);
    t01int4(5 , "index       " , (int ) index);
    t01int4(5 , "count       " , (int ) count);
    ptoc_Var74 = &(*(tgg00_StackList *) sql__nil(ptoc_Var72->integer.C_5.mb_st , "vak505.p" , 16243))[sql__setrang((int ) index , 1 , 32767) + -1];
    ptoc_Var74->integer.C_1.etype = 34;
    ptoc_Var74->integer.C_1.eop = 0;
    ptoc_Var74->integer.C_1.epos = (short ) sql__setrang(count , -32768 , 32767);
    ptoc_Var74->integer.C_1.elen_var = 0;
    ptoc_Var74->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
    ptoc_Var74->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
    ptoc_Var75 = count;
    if (1 <= ptoc_Var75)
        {
        i = 1;
        do
            {
            ptoc_Var76 = &(*(tgg00_StackList *) sql__nil(ptoc_Var72->integer.C_5.mb_st , "vak505.p" , 16254))[sql__setrang((int ) index + i , 1 , 32767) + -1];
            ptoc_Var76->integer.C_1.etype = 48;
            ptoc_Var76->integer.C_1.eop = 0;
            if (i == ptoc_Var75)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    cccprint("<ak505st_code_ju");
    }



int ak505st_code_rollback_for_const_expr(acv, mblock, index, expr_cnt, operator)
tak_all_command_glob *acv;
tgg_mess_block *mblock;
tsp_int2 index;
tsp_int2 expr_cnt;
tgg_stack_op_type operator;
    {
    tsp_int2 cal_begin;
    tsp_int2 cal_end;
    struct tgg00_MessBlock *ptoc_Var77;
    tgg00_QualBuf *ptoc_Var78;
    tgg00_StackEntry *ptoc_Var79;
    cccprint("AK505 ak505st_co");
    ptoc_Var77 = &*mblock;
    ptoc_Var78 = (tgg00_QualBuf *) sql__nil(ptoc_Var77->integer.C_4.mb_qual , "vak505.p" , 16279);
    cal_begin = (short ) sql__setrang((short ) ((int ) index + 1) , -32768 , 32767);
    cal_end = (short ) sql__setrang((short ) ((int ) index + (int ) expr_cnt + 1) , -32768 , 32767);
    g10mv12("VAK505" , 11 , ptoc_Var77->integer.C_2.mb_st_size , ptoc_Var77->integer.C_2.mb_st_size , sql__nil(ptoc_Var77->integer.C_5.mb_st , "vak505.p" , 16285) , (int ) index * 8 + 1 , sql__nil(ptoc_Var77->integer.C_5.mb_st , "vak505.p" , 16286) , (int ) index * 8 + -7 , ((int ) cal_end - (int ) cal_begin) * 8 , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 16288)).integer.C_4.sp1r_returncode);
    ptoc_Var79 = &(*(tgg00_StackList *) sql__nil(ptoc_Var77->integer.C_5.mb_st , "vak505.p" , 16289))[sql__setrang((int ) cal_end + -1 , 1 , 32767) + -1];
    ptoc_Var79->integer.C_1.etype = 26;
    ptoc_Var79->integer.C_1.eop = (unsigned char ) sql__setrang(operator , 0 , 123);
    ptoc_Var79->integer.C_1.epos = 0;
    ptoc_Var79->integer.C_1.elen_var = 0;
    ptoc_Var79->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
    ptoc_Var79->integer.C_1.ecol_tab[1] = (unsigned char)'\0';
    cccprint("<ak505st_code_ro");
    }



int ak505make_st_entry(mblock, index, e_type, e_op, pos, len, tab1, tab2)
tgg_mess_block *mblock;
tsp_int2 index;
tgg_stack_entry_type e_type;
tgg_stack_op_type e_op;
tsp_int2 pos;
tsp_int2 len;
unsigned char tab1;
unsigned char tab2;
    {
    struct tgg00_MessBlock *ptoc_Var80;
    tgg00_QualBuf *ptoc_Var81;
    tgg00_StackEntry *ptoc_Var82;
    cccprint("AK505 ak505make_");
    ptoc_Var80 = &*mblock;
    ptoc_Var81 = (tgg00_QualBuf *) sql__nil(ptoc_Var80->integer.C_4.mb_qual , "vak505.p" , 16313);
    ptoc_Var82 = &(*(tgg00_StackList *) sql__nil(ptoc_Var80->integer.C_5.mb_st , "vak505.p" , 16314))[sql__setrang((int ) index , 1 , 32767) + -1];
    ptoc_Var82->integer.C_1.etype = (unsigned char ) sql__setrang(e_type , 0 , 67);
    ptoc_Var82->integer.C_1.eop = (unsigned char ) sql__setrang(e_op , 0 , 123);
    ptoc_Var82->integer.C_1.epos = (short ) sql__setrang(pos , -32768 , 32767);
    ptoc_Var82->integer.C_1.elen_var = (short ) sql__setrang(len , -32768 , 32767);
    ptoc_Var82->integer.C_1.ecol_tab[0] = (unsigned char ) sql__setrang(tab1 , 0 , 255);
    ptoc_Var82->integer.C_1.ecol_tab[1] = (unsigned char ) sql__setrang(tab2 , 0 , 255);
    cccprint("<ak505make_st_en");
    }



int ak505compare_output_expr(acv, dmli, mblock, index)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tgg_mess_block *mblock;
tsp_int2 *index;
    {
    int len;
    tsp_int2 expr_cnt;
    char enough_place;
    tsp_buf result_buf;
    tgg_stack_op_type operator;
    struct tgg00_MessBlock *ptoc_Var83;
    tgg00_QualBuf *ptoc_Var84;
    tak_dml_info *ptoc_Var85;
    cccprint("AK505 ak505compa");
    ptoc_Var83 = &*mblock;
    ptoc_Var84 = (tgg00_QualBuf *) sql__nil(ptoc_Var83->integer.C_4.mb_qual , "vak505.p" , 16344);
    ptoc_Var85 = &*dmli;
    operator = (unsigned char ) sql__setrang((*(tgg00_StackList *) sql__nil(ptoc_Var83->integer.C_5.mb_st , "vak505.p" , 16346))[sql__setrang((int ) *index , 1 , 32767) + -1].integer.C_1.eop , 0 , 123);
    expr_cnt = (short ) sql__setrang((short ) sql__ord((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var83->integer.C_5.mb_st , "vak505.p" , 16347))[sql__setrang((int ) *index , 1 , 32767) + -1].integer.C_1.ecol_tab[1]) , -32768 , 32767);
    ak505calculate_param_expression(&(*acv) , &(*mblock) , result_buf , &len , (int ) *index , &enough_place);
    if ((char ) sql__setrang(enough_place , 0 , 1))
        {
        g10mv14("VAK505" , 12 , sizeof(tsp00_Buf ) , ptoc_Var83->integer.C_2.mb_data_size , result_buf , 1 , (*(tgg00_DataPart *) sql__nil(ptoc_Var83->integer.C_3.mb_data , "vak505.p" , 16356)).integer.C_1.mbp_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var83->integer.C_5.mb_st , "vak505.p" , 16356))[sql__setrang((int ) *index , 1 , 32767) + -1].integer.C_1.epos , len , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 16357)).integer.C_4.sp1r_returncode);
        ak505make_st_entry(&(*mblock) , (short ) sql__setrang(*index , -32768 , 32767) , 22 , (unsigned char ) sql__setrang(operator , 0 , 123) , (short ) sql__setrang((*(tgg00_StackList *) sql__nil(ptoc_Var83->integer.C_5.mb_st , "vak505.p" , 16359))[sql__setrang((int ) *index , 1 , 32767) + -1].integer.C_1.epos , -32768 , 32767) , (short ) sql__setrang((short ) len , -32768 , 32767) , (unsigned char)'\0' , (unsigned char)'\0');
        ak505st_code_jump(&(*mblock) , (short ) sql__setrang((short ) ((int ) *index + 1) , -32768 , 32767) , (short ) sql__setrang((short ) ((int ) expr_cnt + -1) , -32768 , 32767));
        }
    else
        {
        ak505st_code_rollback_for_const_expr(&(*acv) , &(*mblock) , (short ) sql__setrang(*index , -32768 , 32767) , (short ) sql__setrang(expr_cnt , -32768 , 32767) , (unsigned char ) sql__setrang(operator , 0 , 123));
        ptoc_Var85->d_only_table_scan = 1;
        }
    *index = (short ) sql__setrang((short ) ((int ) *index + sql__ord((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var83->integer.C_5.mb_st , "vak505.p" , 16369))[sql__setrang((int ) *index , 1 , 32767) + -1].integer.C_1.ecol_tab[1]) + -1) , -32768 , 32767);
    cccprint("<ak505compare_ou");
    }



int ak505between_expr(acv, dmli, mblock, index)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tgg_mess_block *mblock;
tsp_int2 *index;
    {
    char enough_place;
    int i;
    int j;
    int len;
    short expr_cnt[2];
    tsp_int2 st_end;
    tsp_int2 end_jump;
    tsp_int2 roll_cnt;
    tsp_buf result_buf;
    tgg_stack_op_type operator;
    tgg_stack_entry store_entry;
    char expression[2];
    char st_rollback[2];
    short st_index[2];
    struct tgg00_MessBlock *ptoc_Var86;
    tgg00_QualBuf *ptoc_Var87;
    tak_dml_info *ptoc_Var88;
    int ptoc_Var89;
    cccprint("AK505 ak505betwe");
    ptoc_Var86 = &*mblock;
    ptoc_Var87 = (tgg00_QualBuf *) sql__nil(ptoc_Var86->integer.C_4.mb_qual , "vak505.p" , 16399);
    ptoc_Var88 = &*dmli;
    st_end = (short ) sql__setrang((short ) ((int ) *index + sql__ord((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var86->integer.C_5.mb_st , "vak505.p" , 16401))[sql__setrang((int ) *index + 1 , 1 , 32767) + -1].integer.C_1.ecol_tab[1]) + 1) , -32768 , 32767);
    st_rollback[0] = 0;
    st_rollback[1] = 0;
    expression[0] = (char ) sql__setrang((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var86->integer.C_5.mb_st , "vak505.p" , 16405))[sql__setrang((int ) *index , 1 , 32767) + -1].integer.C_1.ecol_tab[0] == 3 , 0 , 1);
    expression[1] = (char ) sql__setrang((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var86->integer.C_5.mb_st , "vak505.p" , 16407))[sql__setrang((int ) *index + 1 , 1 , 32767) + -1].integer.C_1.ecol_tab[0] == 3 , 0 , 1);
    end_jump = (short ) sql__setrang((short ) sql__ord((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var86->integer.C_5.mb_st , "vak505.p" , 16408))[sql__setrang((int ) *index + 1 , 1 , 32767) + -1].integer.C_1.ecol_tab[1]) , -32768 , 32767);
    if ((char ) sql__setrang(expression[0] , 0 , 1))
        {
        st_index[0] = (short ) sql__setrang((short ) ((int ) *index + 2) , -32768 , 32767);
        st_index[1] = (short ) sql__setrang((short ) ((int ) *index + sql__ord((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var86->integer.C_5.mb_st , "vak505.p" , 16413))[sql__setrang((int ) *index + 2 , 1 , 32767) + -1].integer.C_1.ecol_tab[1]) + 3) , -32768 , 32767);
        }
    else
        {
        st_index[0] = 0;
        st_index[1] = (short ) sql__setrang((short ) ((int ) *index + 2) , -32768 , 32767);
        }
    expr_cnt[0] = (short ) sql__setrang((short ) sql__ord((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var86->integer.C_5.mb_st , "vak505.p" , 16421))[sql__setrang((int ) st_index[0] , 1 , 32767) + -1].integer.C_1.ecol_tab[1]) , -32768 , 32767);
    expr_cnt[1] = (short ) sql__setrang((short ) sql__ord((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var86->integer.C_5.mb_st , "vak505.p" , 16422))[sql__setrang((int ) st_index[1] , 1 , 32767) + -1].integer.C_1.ecol_tab[1]) , -32768 , 32767);
    t01int4(5 , "st_index[1] " , (int ) st_index[0]);
    t01int4(5 , "st_index[2] " , (int ) st_index[1]);
    t01int4(5 , "index       " , (int ) *index);
    operator = 0;
    i = 1;
    do
        {
        if ((char ) sql__setrang(expression[sql__setrang(i , 1 , 2) + -1] , 0 , 1))
            {
            if ((int ) expr_cnt[sql__setrang(i , 1 , 2) + -1] != 0)
                {
                ak505calculate_param_expression(&(*acv) , &(*mblock) , result_buf , &len , (int ) st_index[sql__setrang(i , 1 , 2) + -1] , &enough_place);
                if ((char ) sql__setrang(enough_place , 0 , 1))
                    {
                    g10mv14("VAK505" , 13 , sizeof(tsp00_Buf ) , ptoc_Var86->integer.C_2.mb_data_size , result_buf , 1 , (*(tgg00_DataPart *) sql__nil(ptoc_Var86->integer.C_3.mb_data , "vak505.p" , 16442)).integer.C_1.mbp_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var86->integer.C_5.mb_st , "vak505.p" , 16442))[sql__setrang((int ) st_index[sql__setrang(i , 1 , 2) + -1] , 1 , 32767) + -1].integer.C_1.epos , len , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 16443)).integer.C_4.sp1r_returncode);
                    }
                else
                    {
                    st_rollback[sql__setrang(i , 1 , 2) + -1] = 1;
                    }
                }
            else
                {
                len = (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var86->integer.C_5.mb_st , "vak505.p" , 16449))[sql__setrang((int ) st_index[sql__setrang(i , 1 , 2) + -1] , 1 , 32767) + -1].integer.C_1.elen_var;
                }
            if (!(char ) sql__setrang(st_rollback[sql__setrang(i , 1 , 2) + -1] , 0 , 1))
                {
                ak505make_st_entry(&(*mblock) , (short ) sql__setrang((short ) ((int ) *index + i + -1) , -32768 , 32767) , 22 , (unsigned char ) sql__setrang(operator , 0 , 123) , (short ) sql__setrang((*(tgg00_StackList *) sql__nil(ptoc_Var86->integer.C_5.mb_st , "vak505.p" , 16455))[sql__setrang((int ) st_index[sql__setrang(i , 1 , 2) + -1] , 1 , 32767) + -1].integer.C_1.epos , -32768 , 32767) , (short ) sql__setrang((short ) len , -32768 , 32767) , (unsigned char)'\0' , (unsigned char)'\0');
                ak505st_code_jump(&(*mblock) , (short ) sql__setrang(st_index[sql__setrang(i , 1 , 2) + -1] , -32768 , 32767) , (short ) sql__setrang(expr_cnt[sql__setrang(i , 1 , 2) + -1] , -32768 , 32767));
                }
            }
        operator = (unsigned char ) sql__setrang((*(tgg00_StackList *) sql__nil(ptoc_Var86->integer.C_5.mb_st , "vak505.p" , 16463))[sql__setrang((int ) *index + 1 , 1 , 32767) + -1].integer.C_1.eop , 0 , 123);
        if (i == 2)
            {
            break;
            }
        i += 1;
        }
    while (!0);
    if (!(char ) sql__setrang(st_rollback[0] , 0 , 1) && !(char ) sql__setrang(st_rollback[1] , 0 , 1))
        {
        *index = (short ) sql__setrang((short ) ((int ) *index + 2) , -32768 , 32767);
        }
    roll_cnt = 0;
    store_entry = (*(tgg00_StackList *) sql__nil(ptoc_Var86->integer.C_5.mb_st , "vak505.p" , 16471))[sql__setrang((int ) *index + 1 , 1 , 32767) + -1];
    if ((char ) sql__setrang(st_rollback[0] , 0 , 1))
        {
        roll_cnt = (short ) sql__setrang((short ) sql__succ((int ) roll_cnt , -32768 , 32767 , "vak505.p" , 16475) , -32768 , 32767);
        g10mv12("VAK505" , 14 , ptoc_Var86->integer.C_2.mb_st_size , ptoc_Var86->integer.C_2.mb_st_size , sql__nil(ptoc_Var86->integer.C_5.mb_st , "vak505.p" , 16478) , (int ) st_index[0] * 8 + 1 , sql__nil(ptoc_Var86->integer.C_5.mb_st , "vak505.p" , 16479) , (int ) *index * 8 + -7 , (int ) expr_cnt[0] * 8 , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 16481)).integer.C_4.sp1r_returncode);
        *index = (short ) sql__setrang((short ) ((int ) *index + (int ) expr_cnt[0]) , -32768 , 32767);
        end_jump = (short ) sql__setrang((short ) ((int ) end_jump - (int ) expr_cnt[0] + 1) , -32768 , 32767);
        (*(tgg00_StackList *) sql__nil(ptoc_Var86->integer.C_5.mb_st , "vak505.p" , 16484))[sql__setrang((int ) *index , 1 , 32767) + -1] = store_entry;
        (*(tgg00_StackList *) sql__nil(ptoc_Var86->integer.C_5.mb_st , "vak505.p" , 16485))[sql__setrang((int ) *index , 1 , 32767) + -1].integer.C_1.ecol_tab[1] = (unsigned char ) sql__setrang((unsigned char ) end_jump , 0 , 255);
        if (!(char ) sql__setrang(st_rollback[1] , 0 , 1))
            {
            ak505st_code_jump(&(*mblock) , (short ) sql__setrang((short ) ((int ) *index + 1) , -32768 , 32767) , (short ) sql__setrang((short ) ((int ) expr_cnt[1] + 2) , -32768 , 32767));
            }
        }
    else
        {
        *index = (short ) sql__setrang((short ) sql__succ((int ) *index , -32768 , 32767 , "vak505.p" , 16492) , -32768 , 32767);
        }
    if ((char ) sql__setrang(st_rollback[1] , 0 , 1))
        {
        roll_cnt = (short ) sql__setrang((short ) sql__succ((int ) roll_cnt , -32768 , 32767 , "vak505.p" , 16497) , -32768 , 32767);
        t01int4(5 , "st_index[2] " , (int ) st_index[1]);
        t01int4(5 , "index       " , (int ) *index);
        t01int4(5 , "expr_cnt[2] " , (int ) expr_cnt[1]);
        ptoc_Var89 = ptoc_Var87->integer.C_2.mfirst_free;
        if (1 <= ptoc_Var89)
            {
            j = 1;
            do
                {
                t01stackentry(5 , &(*(tgg00_StackList *) sql__nil(ptoc_Var86->integer.C_5.mb_st , "vak505.p" , 16502))[sql__setrang(j , 1 , 32767) + -1] , j);
                if (j == ptoc_Var89)
                    {
                    break;
                    }
                j += 1;
                }
            while (!0);
            }
        g10mv12("VAK505" , 15 , ptoc_Var86->integer.C_2.mb_st_size , ptoc_Var86->integer.C_2.mb_st_size , sql__nil(ptoc_Var86->integer.C_5.mb_st , "vak505.p" , 16506) , (int ) st_index[1] * 8 + 1 , sql__nil(ptoc_Var86->integer.C_5.mb_st , "vak505.p" , 16507) , (int ) *index * 8 + -7 , (int ) expr_cnt[1] * 8 , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 16509)).integer.C_4.sp1r_returncode);
        *index = (short ) sql__setrang((short ) ((int ) *index + (int ) expr_cnt[1]) , -32768 , 32767);
        end_jump = (short ) sql__setrang((short ) ((int ) end_jump - (int ) expr_cnt[1] + 1) , -32768 , 32767);
        ak505make_st_entry(&(*mblock) , (short ) sql__setrang(*index , -32768 , 32767) , 26 , (unsigned char ) sql__setrang(store_entry.integer.C_1.eop , 0 , 123) , 0 , 0 , (unsigned char)'\0' , (unsigned char ) sql__setrang((unsigned char ) end_jump , 0 , 255));
        if (!(char ) sql__setrang(st_rollback[0] , 0 , 1))
            {
            ak505st_code_jump(&(*mblock) , (short ) sql__setrang((short ) ((int ) *index + 1) , -32768 , 32767) , (short ) sql__setrang((short ) ((int ) expr_cnt[0] + 1) , -32768 , 32767));
            }
        else
            {
            ak505st_code_jump(&(*mblock) , (short ) sql__setrang((short ) ((int ) *index + 1) , -32768 , 32767) , 3);
            }
        ptoc_Var89 = ptoc_Var87->integer.C_2.mfirst_free;
        if (1 <= ptoc_Var89)
            {
            j = 1;
            do
                {
                t01stackentry(5 , &(*(tgg00_StackList *) sql__nil(ptoc_Var86->integer.C_5.mb_st , "vak505.p" , 16521))[sql__setrang(j , 1 , 32767) + -1] , j);
                if (j == ptoc_Var89)
                    {
                    break;
                    }
                j += 1;
                }
            while (!0);
            }
        }
    ptoc_Var88->d_only_table_scan = (char ) sql__setrang((char ) sql__setrang(st_rollback[0] , 0 , 1) || (char ) sql__setrang(st_rollback[1] , 0 , 1) , 0 , 1);
    *index = (short ) sql__setrang(st_end , -32768 , 32767);
    cccprint("<ak505between_ex");
    }



int ak505in_expr(acv, dmli, mblock, index)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tgg_mess_block *mblock;
tsp_int2 *index;
    {
    char enough_place;
    char st_rollback;
    tsp_int2 real_entry_ptr;
    tsp_int2 st_move_offset;
    tsp_int2 st_begin;
    tsp_int2 st_end;
    tsp_int2 expr_cnt;
    int len;
    tsp_buf result_buf;
    tgg_stack_list_ptr stack_list;
    struct tgg00_MessBlock *ptoc_Var90;
    tgg00_QualBuf *ptoc_Var91;
    tak_dml_info *ptoc_Var92;
    cccprint("AK505 ak505in_ex");
    ptoc_Var90 = &*mblock;
    ptoc_Var91 = (tgg00_QualBuf *) sql__nil(ptoc_Var90->integer.C_4.mb_qual , "vak505.p" , 16553);
    ptoc_Var92 = &*dmli;
    st_begin = (short ) sql__setrang(*index , -32768 , 32767);
    st_rollback = 0;
    while ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16557))[sql__setrang((int ) *index , 1 , 32767) + -1].integer.C_1.etype != 26 && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16558))[sql__setrang((int ) *index , 1 , 32767) + -1].integer.C_1.eop != 11)
        {
        if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16560))[sql__setrang((int ) *index , 1 , 32767) + -1].integer.C_1.etype == 22 && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16561))[sql__setrang((int ) *index , 1 , 32767) + -1].integer.C_1.ecol_tab[0] == 4)
            {
            real_entry_ptr = (short ) sql__setrang((short ) ((int ) *index + sql__ord((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16564))[sql__setrang((int ) *index , 1 , 32767) + -1].integer.C_1.ecol_tab[1])) , -32768 , 32767);
            expr_cnt = (short ) sql__setrang((short ) sql__ord((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16565))[sql__setrang((int ) real_entry_ptr , 1 , 32767) + -1].integer.C_1.ecol_tab[1]) , -32768 , 32767);
            ak505calculate_param_expression(&(*acv) , &(*mblock) , result_buf , &len , (int ) real_entry_ptr , &enough_place);
            if ((char ) sql__setrang(enough_place , 0 , 1))
                {
                g10mv14("VAK505" , 16 , sizeof(tsp00_Buf ) , ptoc_Var90->integer.C_2.mb_data_size , result_buf , 1 , (*(tgg00_DataPart *) sql__nil(ptoc_Var90->integer.C_3.mb_data , "vak505.p" , 16574)).integer.C_1.mbp_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16574))[sql__setrang((int ) real_entry_ptr , 1 , 32767) + -1].integer.C_1.epos , len , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 16575)).integer.C_4.sp1r_returncode);
                ak505make_st_entry(&(*mblock) , (short ) sql__setrang(*index , -32768 , 32767) , 22 , 0 , (short ) sql__setrang((*(tgg00_StackList *) sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16577))[sql__setrang((int ) real_entry_ptr , 1 , 32767) + -1].integer.C_1.epos , -32768 , 32767) , (short ) sql__setrang((short ) len , -32768 , 32767) , (unsigned char)'\0' , (unsigned char)'\0');
                ak505st_code_jump(&(*mblock) , (short ) sql__setrang(real_entry_ptr , -32768 , 32767) , (short ) sql__setrang(expr_cnt , -32768 , 32767));
                }
            else
                {
                st_rollback = 1;
                (*(tgg00_StackList *) sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16585))[sql__setrang((int ) real_entry_ptr , 1 , 32767) + -1].integer.C_1.ecol_tab[1] = (unsigned char ) sql__setrang((unsigned char ) expr_cnt , 0 , 255);
                }
            }
        *index = (short ) sql__setrang((short ) sql__succ((int ) *index , -32768 , 32767 , "vak505.p" , 16590) , -32768 , 32767);
        }
    st_end = (short ) sql__setrang((short ) ((int ) *index + sql__ord((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16593))[sql__setrang((int ) *index , 1 , 32767) + -1].integer.C_1.ecol_tab[1]) + -1) , -32768 , 32767);
    if ((char ) sql__setrang(st_rollback , 0 , 1))
        {
        *index = (short ) sql__setrang(st_begin , -32768 , 32767);
        st_move_offset = 0;
        while ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16599))[sql__setrang((int ) *index , 1 , 32767) + -1].integer.C_1.etype != 26 && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16600))[sql__setrang((int ) *index , 1 , 32767) + -1].integer.C_1.eop != 11)
            {
            if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16603))[sql__setrang((int ) *index , 1 , 32767) + -1].integer.C_1.etype == 22 && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16604))[sql__setrang((int ) *index , 1 , 32767) + -1].integer.C_1.ecol_tab[0] == 4)
                {
                real_entry_ptr = (short ) sql__setrang((short ) ((int ) *index + sql__ord((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16607))[sql__setrang((int ) *index , 1 , 32767) + -1].integer.C_1.ecol_tab[1]) - (int ) st_move_offset) , -32768 , 32767);
                expr_cnt = (short ) sql__setrang((short ) sql__ord((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16609))[sql__setrang((int ) real_entry_ptr , 1 , 32767) + -1].integer.C_1.ecol_tab[1]) , -32768 , 32767);
                st_move_offset = (short ) sql__setrang((short ) ((int ) st_move_offset + (int ) expr_cnt + -1) , -32768 , 32767);
                t01int4(5 , "st_rollback " , 4711);
                t01int4(5 , "real_entry_p" , (int ) real_entry_ptr);
                t01int4(5 , "expr_cnt    " , (int ) expr_cnt);
                t01int4(5 , "index       " , (int ) *index);
                a10new(&(*acv) , (int ) expr_cnt * 8 , &stack_list);
                if (stack_list == (tgg00_StackList *) (int *) 0)
                    {
                    a07_b_put_error(&(*acv) , 1405 , 1);
                    }
                else
                    {
                    g10mv12("VAK505" , 17 , ptoc_Var90->integer.C_2.mb_st_size , (int ) expr_cnt * 8 , sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16623) , (int ) real_entry_ptr * 8 + 1 , sql__nil(stack_list , "vak505.p" , 16624) , 1 , (int ) expr_cnt * 8 , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 16625)).integer.C_4.sp1r_returncode);
                    g10mvr("VAK505" , 18 , ptoc_Var90->integer.C_2.mb_st_size , ptoc_Var90->integer.C_2.mb_st_size , sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16628) , (int ) *index * 8 + 1 , sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16629) , ((int ) *index + (int ) expr_cnt) * 8 + -7 , ((int ) real_entry_ptr - (int ) *index) * 8 + -8 , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 16631)).integer.C_4.sp1r_returncode);
                    g10mv12("VAK505" , 19 , (int ) expr_cnt * 8 , ptoc_Var90->integer.C_2.mb_st_size , sql__nil(stack_list , "vak505.p" , 16634) , 1 , sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16635) , (int ) *index * 8 + -7 , (int ) expr_cnt * 8 , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 16637)).integer.C_4.sp1r_returncode);
                    ak505st_code_jump(&(*mblock) , (short ) sql__setrang((short ) ((int ) real_entry_ptr + (int ) expr_cnt + -1) , -32768 , 32767) , 1);
                    a10dispose(&(*acv) , &stack_list);
                    }
                }
            *index = (short ) sql__setrang((short ) sql__succ((int ) *index , -32768 , 32767 , "vak505.p" , 16645) , -32768 , 32767);
            }
        (*(tgg00_StackList *) sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16648))[sql__setrang((int ) *index , 1 , 32767) + -1].integer.C_1.ecol_tab[1] = (unsigned char ) sql__setrang((unsigned char ) (sql__ord((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var90->integer.C_5.mb_st , "vak505.p" , 16649))[sql__setrang((int ) *index , 1 , 32767) + -1].integer.C_1.ecol_tab[1]) - (int ) st_move_offset + -1) , 0 , 255);
        }
    *index = (short ) sql__setrang(st_end , -32768 , 32767);
    ptoc_Var92->d_only_table_scan = (char ) sql__setrang(st_rollback , 0 , 1);
    cccprint("<ak505in_expr   ");
    }



int a505const_param_expression(acv, dmli, mblock)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tgg_mess_block *mblock;
    {
    tsp_int2 aux_mqual_cnt;
    tsp_int2 aux_mqual_pos;
    tsp_int2 m_cnt;
    tsp_int2 m_pos;
    tsp_int2 index;
    tsp_moveobj_ptr opt_ptr;
    int m_opt_pos;
    struct tgg00_MessBlock *ptoc_Var93;
    tgg00_QualBuf *ptoc_Var94;
    int ptoc_Var95;
    tgg00_StackEntry *ptoc_Var96;
    cccprint("AK505 a505const_");
    ptoc_Var93 = &*mblock;
    ptoc_Var94 = (tgg00_QualBuf *) sql__nil(ptoc_Var93->integer.C_4.mb_qual , "vak505.p" , 16678);
    aux_mqual_cnt = (short ) sql__setrang(ptoc_Var94->integer.C_2.mqual_cnt , -32768 , 32767);
    aux_mqual_pos = (short ) sql__setrang(ptoc_Var94->integer.C_2.mqual_pos , -32768 , 32767);
    m_cnt = (short ) sql__setrang(ptoc_Var94->integer.C_2.mqual_cnt , -32768 , 32767);
    m_pos = (short ) sql__setrang(ptoc_Var94->integer.C_2.mqual_pos , -32768 , 32767);
    if ((int ) ptoc_Var94->integer.C_2.mst_optimize_pos > 0)
        {
        opt_ptr = (tsp00_MoveObj *) &(*(tgg00_DataPart *) sql__nil(ptoc_Var93->integer.C_3.mb_data , "vak505.p" , 16686)).integer.C_1.mbp_buf[sql__setrang((int ) ptoc_Var94->integer.C_2.mst_optimize_pos , 1 , 8192000) + -1];
        }
    do
        {
        index = (short ) sql__setrang(m_pos , -32768 , 32767);
        while ((int ) index < (int ) m_cnt + (int ) m_pos)
            {
            m_opt_pos = 0;
            if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var93->integer.C_5.mb_st , "vak505.p" , 16693))[sql__setrang((int ) index , 1 , 32767) + -1].integer.C_1.etype == 22 && ((ptoc_Var95 = (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var93->integer.C_5.mb_st , "vak505.p" , 16694))[sql__setrang((int ) index , 1 , 32767) + -1].integer.C_1.ecol_tab[0]) == 3 || ptoc_Var95 == 4 || (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var93->integer.C_5.mb_st , "vak505.p" , 16696))[sql__setrang((int ) index , 1 , 32767) + -1].integer.C_1.ecol_tab[0] == 0 && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var93->integer.C_5.mb_st , "vak505.p" , 16697))[sql__setrang((int ) index , 1 , 32767) + -1].integer.C_1.ecol_tab[1] != 0))
                {
                if ((int ) ptoc_Var94->integer.C_2.mst_optimize_pos > 0)
                    {
                    m_opt_pos = (int ) ptoc_Var94->integer.C_2.mst_optimize_pos;
                    ptoc_Var94->integer.C_2.mst_optimize_pos = 0;
                    if (sql__ord((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var93->integer.C_5.mb_st , "vak505.p" , 16707))[sql__setrang((int ) index , 1 , 32767) + -1].integer.C_1.eop) == sql__ord((int ) (*(tsp00_MoveObj *) sql__nil(opt_ptr , "vak505.p" , 16707))[sql__setrang((int ) index - (int ) aux_mqual_pos + 1 , 1 , 8192000) + -1]))
                        {
                        (*(tsp00_MoveObj *) sql__nil(opt_ptr , "vak505.p" , 16709))[sql__setrang((int ) index - (int ) aux_mqual_pos , 1 , 8192000) + -1] = (unsigned char)'\0';
                        }
                    (*(tsp00_MoveObj *) sql__nil(opt_ptr , "vak505.p" , 16711))[sql__setrang((int ) index - (int ) aux_mqual_pos + 1 , 1 , 8192000) + -1] = (unsigned char)'\0';
                    }
                switch ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var93->integer.C_5.mb_st , "vak505.p" , 16714))[sql__setrang((int ) index , 1 , 32767) + -1].integer.C_1.ecol_tab[0])
                    {
                    case 0 :
                        ak505compare_output_expr(&(*acv) , &(*dmli) , &(*mblock) , &index);
                        break;
                    case 3 :
                        if ((ptoc_Var95 = (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var93->integer.C_5.mb_st , "vak505.p" , 16719))[sql__setrang((int ) index , 1 , 32767) + -1].integer.C_1.eop) == 94 || ptoc_Var95 == 95)
                            {
                            index = (short ) sql__setrang((short ) sql__pred((int ) index , -32768 , 32767 , "vak505.p" , 16722) , -32768 , 32767);
                            }
                        ak505between_expr(&(*acv) , &(*dmli) , &(*mblock) , &index);
                        break;
                    case 4 :
                        ak505in_expr(&(*acv) , &(*dmli) , &(*mblock) , &index);
                        break;
                    default :
                        (*(tgg00_TransContext *) sql__nil(ptoc_Var93->integer.C_9.mb_trns , "vak505.p" , 16729)).trError_gg00 = 4840;
                        break;
                    }
                }
            if (m_opt_pos > 0)
                {
                ptoc_Var94->integer.C_2.mst_optimize_pos = (short ) sql__setrang((short ) m_opt_pos , -32768 , 32767);
                m_opt_pos = 0;
                }
            index = (short ) sql__setrang((short ) sql__succ((int ) index , -32768 , 32767 , "vak505.p" , 16741) , -32768 , 32767);
            }
        if ((int ) (*(tgg00_TransContext *) sql__nil(ptoc_Var93->integer.C_9.mb_trns , "vak505.p" , 16744)).trError_gg00 == 0)
            {
            index = (short ) sql__setrang(m_pos , -32768 , 32767);
            while ((int ) index < (int ) m_cnt + (int ) m_pos)
                {
                if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var93->integer.C_5.mb_st , "vak505.p" , 16750))[sql__setrang((int ) index , 1 , 32767) + -1].integer.C_1.etype == 34 && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var93->integer.C_5.mb_st , "vak505.p" , 16751))[sql__setrang((int ) index + (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var93->integer.C_5.mb_st , "vak505.p" , 16751))[sql__setrang((int ) index , 1 , 32767) + -1].integer.C_1.epos + 1 , 1 , 32767) + -1].integer.C_1.etype == 34)
                    {
                    ptoc_Var96 = &(*(tgg00_StackList *) sql__nil(ptoc_Var93->integer.C_5.mb_st , "vak505.p" , 16755))[sql__setrang((int ) index + (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var93->integer.C_5.mb_st , "vak505.p" , 16755))[sql__setrang((int ) index , 1 , 32767) + -1].integer.C_1.epos + 1 , 1 , 32767) + -1];
                    ptoc_Var96->integer.C_1.etype = 48;
                    ptoc_Var96->integer.C_1.eop = 0;
                    (*(tgg00_StackList *) sql__nil(ptoc_Var93->integer.C_5.mb_st , "vak505.p" , 16761))[sql__setrang((int ) index , 1 , 32767) + -1].integer.C_1.epos = (short ) sql__setrang((short ) ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var93->integer.C_5.mb_st , "vak505.p" , 16761))[sql__setrang((int ) index , 1 , 32767) + -1].integer.C_1.epos + (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var93->integer.C_5.mb_st , "vak505.p" , 16762))[sql__setrang((int ) index + (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var93->integer.C_5.mb_st , "vak505.p" , 16762))[sql__setrang((int ) index , 1 , 32767) + -1].integer.C_1.epos + 1 , 1 , 32767) + -1].integer.C_1.epos + 1) , -32768 , 32767);
                    index = (short ) sql__setrang((short ) sql__pred((int ) index , -32768 , 32767 , "vak505.p" , 16763) , -32768 , 32767);
                    }
                index = (short ) sql__setrang((short ) sql__succ((int ) index , -32768 , 32767 , "vak505.p" , 16766) , -32768 , 32767);
                }
            }
        m_cnt = (short ) sql__setrang(ptoc_Var94->integer.C_2.mresqual_cnt , -32768 , 32767);
        m_pos = (short ) sql__setrang(ptoc_Var94->integer.C_2.mresqual_pos , -32768 , 32767);
        }
    while (!((int ) index > (int ) ptoc_Var94->integer.C_2.mresqual_pos));
    ptoc_Var94->integer.C_2.mqual_cnt = (short ) sql__setrang(aux_mqual_cnt , -32768 , 32767);
    ptoc_Var94->integer.C_2.mqual_pos = (short ) sql__setrang(aux_mqual_pos , -32768 , 32767);
    cccprint("<a505const_param");
    }



int ak505dist_lock_mess_bufs(acv, return_result)
tak_all_command_glob *acv;
char return_result;
    {
    int i;
    tak_syspointerarr isparr;
    tak_ak_all_locks back_lock;
    tgg00_QualBuf *ptoc_Var97;
    int ptoc_Var98;
    cccprint("AK505 ak505dist_");
    ptoc_Var97 = (tgg00_QualBuf *) sql__nil((*acv).a_mblock.integer.C_4.mb_qual , "vak505.p" , 16797);
    back_lock.all_lock_count = (short ) sql__setrang(ptoc_Var97->integer.C_7.mlock_cnt , -32768 , 32767);
    ptoc_Var98 = ptoc_Var97->integer.C_7.mlock_cnt;
    if (1 <= ptoc_Var98)
        {
        i = 1;
        do
            {
            back_lock.all_locks[sql__setrang(i , 1 , 240) + -1] = ptoc_Var97->integer.C_7.mlock[sql__setrang(i , 1 , 240) + -1];
            if (i == ptoc_Var98)
                {
                break;
                }
            i += 1;
            }
        while (!0);
        }
    a52_rest_lock_statement(&(*acv) , &isparr , &back_lock , (char ) sql__setrang(return_result , 0 , 1) , 0);
    cccprint("<ak505dist_lock_");
    }



int ak505exec_link(acv, syskey, mblock_buf, pars_id)
tak_all_command_glob *acv;
tgg_sysinfokey *syskey;
tak_sysbufferaddress *mblock_buf;
tsp_int2 pars_id;
    {
    tgg_basis_error b_err;
    char cascade;
    tak_execution_kind init_ex_kind;
    tsp_int2 lo_level;
    tsp_int2 use_file_id;
    tak_selfref_link_info self_ref_info;
    struct tgg00_MessBlock *ptoc_Var99;
    tgg00_LinkChainInfo *ptoc_Var100;
    tak_all_command_glob *ptoc_Var101;
    tak_selfref_link_info *ptoc_Var102;
    cccprint("AK505 ak505exec_");
    use_file_id = 0;
    while ((int ) pars_id != 0 && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak505.p" , 16828)).integer.C_4.sp1r_returncode == 0)
        {
        (*syskey).integer.C_4.sintlinkage = (short ) sql__setrang(pars_id , -32768 , 32767);
        a10get_sysinfo(&(*acv) , &(*syskey) , 0 , &(*mblock_buf) , &b_err);
        if ((int ) b_err == 0)
            {
            ptoc_Var99 = &(*(struct tak_systembuffer *) sql__nil(*mblock_buf , "vak505.p" , 16834)).integer.C_62.smessblock.mbr_mess_block;
            ptoc_Var100 = &(*(tgg00_QualBuf *) sql__nil(ptoc_Var99->integer.C_4.mb_qual , "vak505.p" , 16835)).integer.C_14.mlc_info;
            pars_id = (short ) sql__setrang(ptoc_Var100->mlp_sa_level , -32768 , 32767);
            lo_level = (short ) sql__setrang(ptoc_Var100->mlp_lo_level , -32768 , 32767);
            t01int4(5 , "sa_level =  " , (int ) pars_id);
            use_file_id = (short ) sql__setrang(ptoc_Var100->mlp_use_fileid , -32768 , 32767);
            t01int4(5 , "use_fide_id " , (int ) use_file_id);
            if ((int ) ptoc_Var99->integer.C_2.mb_type == 78)
                {
                ptoc_Var101 = &*acv;
                ptoc_Var102 = &self_ref_info;
                (*(tgg00_QualBuf *) sql__nil(ptoc_Var101->a_mblock.integer.C_4.mb_qual , "vak505.p" , 16860)).integer.C_2.mtree = (*(tgg00_QualBuf *) sql__nil(ptoc_Var99->integer.C_4.mb_qual , "vak505.p" , 16860)).integer.C_2.mtree;
                init_ex_kind = (unsigned char ) sql__setrang(ptoc_Var101->a_ex_kind , 0 , 4);
                ptoc_Var101->a_ex_kind = 1;
                ptoc_Var101->a_pars_last_key.p_id[0] = (unsigned char ) sql__setrang((unsigned char ) (sql__ord((int ) ptoc_Var101->a_pars_last_key.p_id[0]) + 1) , 0 , 255);
                g10mv7("VAK505" , 20 , ptoc_Var99->integer.C_2.mb_qual_size , sizeof(tak_selfref_link_info ) , (*(tgg00_QualBuf *) sql__nil(ptoc_Var99->integer.C_4.mb_qual , "vak505.p" , 16867)).integer.C_1.buf , 41 , &self_ref_info , 1 , sizeof(tak_selfref_link_info ) , &(*(tsp1_segment *) sql__nil(ptoc_Var101->a_return_segm , "vak505.p" , 16869)).integer.C_4.sp1r_returncode);
                use_file_id = (short ) sql__setrang(ptoc_Var102->sf_use_id , -32768 , 32767);
                a58delete_with_link(&(*acv) , (*(tgg00_QualBuf *) sql__nil(ptoc_Var101->a_mblock.integer.C_4.mb_qual , "vak505.p" , 16872)).integer.C_2.mtree.integer.C_4.fileTabId_gg00 , (int ) ptoc_Var102->sf_index , (int ) ptoc_Var102->sf_parsno , (int ) ptoc_Var102->sf_use_id);
                ptoc_Var101->a_ex_kind = (unsigned char ) sql__setrang(init_ex_kind , 0 , 4);
                cascade = 0;
                }
            else
                {
                a58exec_link_caused_dml(&(*acv) , *mblock_buf , (short ) sql__setrang(use_file_id , -32768 , 32767) , &cascade , 0);
                }
            if ((char ) sql__setrang(cascade , 0 , 1))
                {
                t01int4(5 , "lo_level =  " , (int ) lo_level);
                if ((int ) lo_level != 0)
                    {
                    ak505exec_link(&(*acv) , &(*syskey) , &(*mblock_buf) , (short ) sql__setrang(lo_level , -32768 , 32767));
                    }
                }
            }
        else
            {
            a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
            }
        }
    a58destroy_linkfile(&(*acv) , (int ) use_file_id);
    cccprint("<ak505exec_link ");
    }



int ak505join_view_check(acv, dmli, act_res_tree, jvrec, series, tabcnt)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tgg00_FileId *act_res_tree;
tak68_joinview_rec *jvrec;
tak68_sequence series;
int tabcnt;
    {
    tak_parskey parsk;
    tak_all_command_glob *ptoc_Var103;
    tak68_joinview_rec *ptoc_Var104;
    struct tgg00_MessBlock *ptoc_Var105;
    tgg00_DataPart *ptoc_Var106;
    tak_dml_info *ptoc_Var107;
    struct ptoc_Type172 *ptoc_Var108;
    struct tgg00_MessBlock *ptoc_Var109;
    tgg00_DataPart *ptoc_Var110;
    cccprint("AK505 ak505join_");
    ptoc_Var103 = &*acv;
    ptoc_Var104 = &*jvrec;
    *act_res_tree = ptoc_Var103->a_into_tree;
    a06a_mblock_init(&(*acv) , 60 , 68 , &b01niltree_id);
    ptoc_Var105 = &ptoc_Var103->a_mblock;
    ptoc_Var106 = (tgg00_DataPart *) sql__nil(ptoc_Var105->integer.C_3.mb_data , "vak505.p" , 16926);
    ptoc_Var105->integer.C_2.mb_data_len = (int ) ptoc_Var104->jv_maxkeyl + 8;
    ptoc_Var106->integer.C_4.mbp_keylen = (short ) sql__setrang(ptoc_Var104->jv_maxkeyl , -32768 , 32767);
    a54_dml_init(&(*dmli) , 0);
    ptoc_Var107 = &*dmli;
    ptoc_Var108 = &ptoc_Var107->d_tabarr[0];
    ptoc_Var108->oview = 1;
    ptoc_Var108->otreeid = b01niltree_id;
    _ptoc_ASSIGN(unsigned char , 8, ptoc_Var108->otreeid.integer.C_4.fileTabId_gg00, ptoc_Var104->jv_tabid)
    _ptoc_ASSIGN(unsigned char , 8, ptoc_Var108->ofromtableid, ptoc_Var104->jv_tabid)
    ptoc_Var108->oall_priv = 1;
    ptoc_Var107->d_cntfromtab = (short ) sql__setrang((short ) tabcnt , -32768 , 32767);
    ptoc_Var107->d_acttabindex = 1;
    ptoc_Var107->d_view = 1;
    ptoc_Var107->d_single = 1;
    ptoc_Var107->d_checkview = (char ) sql__setrang(ptoc_Var104->jv_checkview , 0 , 1);
    a54_view_put_into(&(*acv) , &(*dmli));
    ptoc_Var109 = &ptoc_Var103->a_mblock;
    ptoc_Var110 = (tgg00_DataPart *) sql__nil(ptoc_Var109->integer.C_3.mb_data , "vak505.p" , 16948);
    ptoc_Var109->integer.C_2.mb_data_len = sql__succ(ptoc_Var109->integer.C_2.mb_data_len , -2147483647 , 2147483647 , "vak505.p" , 16952);
    (*(tgg00_DataPart *) sql__nil(ptoc_Var109->integer.C_3.mb_data , "vak505.p" , 16953)).integer.C_1.mbp_buf[sql__setrang(ptoc_Var109->integer.C_2.mb_data_len , 1 , 8192000) + -1] = (unsigned char)'\377';
    a682_execute_join(&(*acv) , &(*dmli) , series , &(*act_res_tree) , &parsk , &(*jvrec) , 0 , 0);
    cccprint("<ak505join_view_");
    }

static
  unsigned char ptoc_Var111[1] =
  {
  (unsigned char)'\001'
  };
static
  unsigned char ptoc_Var112[1] =
  {
  (unsigned char)'\001'
  };


int ak505long_support(acv, dmli, mtype)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tgg_message_type mtype;
    {
    tgg_basis_error e;
    char only_move;
    int cnt;
    int out_cnt;
    int desc_len;
    tsp_int4 long_desc_pos;
    tsp_int4 offset;
    tsp_long_descriptor long_desc;
    tgg_mess_block mblock;
    tgg_qual_buf curr_qual_buf;
    tgg_rec_buf curr_data_buf;
    tak_all_command_glob *ptoc_Var113;
    tak_dml_info *ptoc_Var114;
    tsp00_LongDescriptor *ptoc_Var115;
    int ptoc_Var116;
    struct tgg00_MessBlock *ptoc_Var117;
    tgg00_QualBuf *ptoc_Var118;
    unsigned char ptoc_Var119[1];
    unsigned char ptoc_Var120[1];
    unsigned char ptoc_Var121[1];
    cccprint("AK505 ak505long_");
    ptoc_Var113 = &*acv;
    ptoc_Var114 = &*dmli;
    ptoc_Var115 = &long_desc;
    t01int4(5 , "cmd_length  " , (*(tsp1_part *) sql__nil(ptoc_Var113->a_cmd_part , "vak505.p" , 16990)).integer.C_2.sp1p_buf_len);
    t01int4(5 , "data_length " , ptoc_Var113->a_data_length);
    t01moveobj(5 , (*(tsp1_part *) sql__nil(ptoc_Var113->a_cmd_part , "vak505.p" , 16992)).integer.C_1.sp1p_buf , 1 , (*(tsp1_part *) sql__nil(ptoc_Var113->a_cmd_part , "vak505.p" , 16992)).integer.C_2.sp1p_buf_len);
    if (ptoc_Var113->a_data_ptr != (tsp00_MoveObj *) (int *) 0)
        {
        t01moveobj(5 , sql__nil(ptoc_Var113->a_data_ptr , "vak505.p" , 16995) , 1 , ptoc_Var113->a_data_length);
        }
    if (_ptoc_MEMCMP(unsigned char , 5, ptoc_Var113->a_comp_vers , "62000") < 0 ? 1 : 0)
        {
        desc_len = 32;
        }
    else
        {
        desc_len = 40;
        }
    if ((int ) mtype == 35 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var113->a_return_segm , "vak505.p" , 17003)).integer.C_4.sp1r_returncode == 0)
        {
        long_desc_pos = (int ) ptoc_Var114->d_first_longdesc_pos;
        ptoc_Var116 = (int ) ptoc_Var114->d_cnt_longdesc + -1;
        if (0 <= ptoc_Var116)
            {
            cnt = 0;
            do
                {
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var113->a_return_segm , "vak505.p" , 17008)).integer.C_4.sp1r_returncode == 0)
                    {
                    g10mv10("VAK505" , 21 , ptoc_Var113->a_data_length , sizeof(tsp00_LongDescriptor ) , sql__nil(ptoc_Var113->a_data_ptr , "vak505.p" , 17013) , long_desc_pos + 1 , &long_desc , 1 , desc_len , &(*(tsp1_segment *) sql__nil(ptoc_Var113->a_return_segm , "vak505.p" , 17015)).integer.C_4.sp1r_returncode);
                    t01longdescriptor(5 , long_desc);
                    if ((int ) (*(tsp00_MoveObj *) sql__nil(ptoc_Var113->a_data_ptr , "vak505.p" , 17017))[sql__setrang(long_desc_pos , 1 , 8192000) + -1] != 255 && (0 || (long_desc.boolean.C_false.ld_infoset[0] & 128) == 0))
                        {
                        g01mblock_init(&(*acv).a_transinf.tri_trans , 35 , 0 , &mblock);
                        ptoc_Var117 = &mblock;
                        ptoc_Var117->integer.C_3.mb_data = (tgg00_DataPart *) &curr_data_buf;
                        ptoc_Var117->integer.C_2.mb_data_size = sizeof(tgg00_Rec );
                        g01stack_init(&mblock , (tgg00_StackList *) (int *) 0 , 0 , (tgg00_QualBuf *) &curr_qual_buf , sizeof(tgg00_QualBuf ));
                        ptoc_Var118 = (tgg00_QualBuf *) sql__nil(ptoc_Var117->integer.C_4.mb_qual , "vak505.p" , 17029);
                        ptoc_Var118->integer.C_2.mtree = (*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 17031)).integer.C_23.sparsinfo.integer.C_2.p_treeid;
                        if ((int ) ptoc_Var118->integer.C_2.mtree.integer.C_4.fileTfn_gg00 != 26)
                            {
                            _ptoc_ASSIGN(unsigned char , 2, ptoc_Var118->integer.C_2.mtree.integer.C_1.fileHandling_gg00, (*acv).a_transinf.tri_global_state)
                            }
                        (*(tgg00_DataPart *) sql__nil(ptoc_Var117->integer.C_3.mb_data , "vak505.p" , 17040)).integer.C_4.mbp_reclen = 0;
                        (*(tgg00_DataPart *) sql__nil(ptoc_Var117->integer.C_3.mb_data , "vak505.p" , 17041)).integer.C_4.mbp_keylen = 0;
                        (*(tgg00_QualBuf *) sql__nil(ptoc_Var117->integer.C_4.mb_qual , "vak505.p" , 17042)).integer.C_2.mtree.integer.C_4.fileTfn_gg00 = 18;
                        _ptoc_ASSIGN(unsigned char , 2, (*(tgg00_QualBuf *) sql__nil(ptoc_Var117->integer.C_4.mb_qual , "vak505.p" , 17043)).integer.C_2.mtree.integer.C_1.fileVersion_gg00.ci2_gg00, "\377\377")
                        (*(tgg00_QualBuf *) sql__nil(ptoc_Var117->integer.C_4.mb_qual , "vak505.p" , 17044)).integer.C_2.mtree.integer.C_1.fileRoot_gg00 = 2147483647;
                        ptoc_Var117->integer.C_2.mb_data_len = 16;
                        (*(tgg00_DataPart *) sql__nil(ptoc_Var117->integer.C_3.mb_data , "vak505.p" , 17048)).integer.C_4.mbp_reclen = (short ) sql__setrang((short ) ptoc_Var117->integer.C_2.mb_data_len , -32768 , 32767);
                        (*(tgg00_DataPart *) sql__nil(ptoc_Var117->integer.C_3.mb_data , "vak505.p" , 17049)).integer.C_4.mbp_keylen = 8;
                        (*(tgg00_DataPart *) sql__nil(ptoc_Var117->integer.C_3.mb_data , "vak505.p" , 17050)).integer.C_4.mbp_varcol_offset = 0;
                        (*(tgg00_DataPart *) sql__nil(ptoc_Var117->integer.C_3.mb_data , "vak505.p" , 17051)).integer.C_4.mbp_varcol_cnt = 0;
                        g10mv9("VAK505" , 22 , sizeof(tsp00_C8 ) , ptoc_Var117->integer.C_2.mb_data_size , long_desc.boolean.C_false.ld_descriptor , 1 , (*(tgg00_DataPart *) sql__nil(ptoc_Var117->integer.C_3.mb_data , "vak505.p" , 17055)).integer.C_1.mbp_buf , 9 , 8 , &(*(tsp1_segment *) sql__nil(ptoc_Var113->a_return_segm , "vak505.p" , 17057)).integer.C_4.sp1r_returncode);
                        a06rsend_mess_buf(&(*acv) , &mblock , 1 , &e);
                        if ((int ) e != 0)
                            {
                            a07_b_put_error(&(*acv) , (short ) sql__setrang(e , -32768 , 32767) , 1);
                            }
                        }
                    if ((char ) sql__setrang(ptoc_Var113->a_execute_trigger , 0 , 1))
                        {
                        _ptoc_MOVE(unsigned char , 1, long_desc.boolean.C_false.ld_infoset, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var119 , long_desc.boolean.C_false.ld_infoset , ptoc_Var111 , sizeof(unsigned char [1])));
                        g10mv11("VAK505" , 23 , sizeof(tsp00_LongDescriptor ) , ptoc_Var113->a_data_length , &long_desc , 1 , sql__nil(ptoc_Var113->a_data_ptr , "vak505.p" , 17076) , long_desc_pos + 1 , desc_len , &(*(tsp1_segment *) sql__nil(ptoc_Var113->a_return_segm , "vak505.p" , 17077)).integer.C_4.sp1r_returncode);
                        }
                    long_desc_pos = long_desc_pos + desc_len + 1;
                    }
                if (cnt == ptoc_Var116)
                    {
                    break;
                    }
                cnt += 1;
                }
            while (!0);
            }
        }
    else
        {
        if ((int ) mtype == 76 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var113->a_return_segm , "vak505.p" , 17086)).integer.C_4.sp1r_returncode == 0)
            {
            ptoc_Var116 = (int ) ptoc_Var114->d_cnt_longdesc + -1;
            if (0 <= ptoc_Var116)
                {
                cnt = 0;
                do
                    {
                    g10mv10("VAK505" , 24 , ptoc_Var113->a_data_length , sizeof(tsp00_LongDescriptor ) , sql__nil(ptoc_Var113->a_data_ptr , "vak505.p" , 17092) , (int ) ptoc_Var114->d_first_longdesc_pos + cnt * (desc_len + 1) + 1 , &long_desc , 1 , desc_len , &(*(tsp1_segment *) sql__nil(ptoc_Var113->a_return_segm , "vak505.p" , 17094)).integer.C_4.sp1r_returncode);
                    if (ptoc_Var113->a_mblock.integer.C_2.mb_qual_len == 172)
                        {
                        _ptoc_ASSIGN(unsigned char , 8, ptoc_Var115->boolean.C_false.ld_descriptor, "\0\0\0\0\0\0\0\0")
                        }
                    else
                        {
                        g10mv1("VAK505" , 25 , ptoc_Var113->a_mblock.integer.C_2.mb_qual_size , sizeof(tsp00_C8 ) , (*(tgg00_QualBuf *) sql__nil(ptoc_Var113->a_mblock.integer.C_4.mb_qual , "vak505.p" , 17103)).integer.C_1.buf , cnt * (sizeof(tsp00_C8 ) + 1) + 174 , ptoc_Var115->boolean.C_false.ld_descriptor , 1 , sizeof(tsp00_C8 ) , &(*(tsp1_segment *) sql__nil(ptoc_Var113->a_return_segm , "vak505.p" , 17107)).integer.C_4.sp1r_returncode);
                        if ((int ) (*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak505.p" , 17108)).integer.C_23.sparsinfo.integer.C_2.p_mtyp2 == 21)
                            {
                            ptoc_Var115->boolean.C_false.ld_intern_pos = 0;
                            }
                        else
                            {
                            if (1 && (ptoc_Var115->boolean.C_false.ld_infoset[0] & 128) != 0 && (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var113->a_mblock.integer.C_4.mb_qual , "vak505.p" , 17113)).integer.C_1.buf[sql__setrang(cnt * (sizeof(tsp00_C8 ) + 1) + 173 , 1 , 8192) + -1] == 1)
                                {
                                ptoc_Var115->boolean.C_false.ld_intern_pos = -1;
                                }
                            }
                        }
                    if (_ptoc_MEMCMP(unsigned char , 8, ptoc_Var115->boolean.C_false.ld_descriptor , "\0\0\0\0\0\0\0\0") == 0 ? 1 : 0)
                        {
                        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var113->a_return_segm , "vak505.p" , 17126)).integer.C_4.sp1r_function_code > 1000 && (int ) (*(tsp00_MoveObj *) sql__nil(ptoc_Var113->a_data_ptr , "vak505.p" , 17128))[sql__setrang((int ) ptoc_Var114->d_first_longdesc_pos + cnt * (desc_len + 1) , 1 , 8192000) + -1] != 255)
                            {
                            offset = (*(tsp1_part *) sql__nil(ptoc_Var113->a_data_part , "vak505.p" , 17131)).integer.C_2.sp1p_buf_len - ptoc_Var113->a_data_length;
                            }
                        else
                            {
                            offset = 0;
                            }
                        if (ptoc_Var113->a_input_data_pos < ptoc_Var115->boolean.C_false.ld_valpos - offset + ptoc_Var115->boolean.C_false.ld_vallen)
                            {
                            ptoc_Var113->a_input_data_pos = ptoc_Var115->boolean.C_false.ld_valpos - offset + ptoc_Var115->boolean.C_false.ld_vallen;
                            }
                        (*(tsp00_MoveObj *) sql__nil(ptoc_Var113->a_data_ptr , "vak505.p" , 17139))[sql__setrang((int ) ptoc_Var114->d_first_longdesc_pos + cnt * (desc_len + 1) , 1 , 8192000) + -1] = (unsigned char)'\377';
                        }
                    else
                        {
                        (*(tsp00_MoveObj *) sql__nil(ptoc_Var113->a_data_ptr , "vak505.p" , 17144))[sql__setrang((int ) ptoc_Var114->d_first_longdesc_pos + cnt * (desc_len + 1) , 1 , 8192000) + -1] = (unsigned char)'\0';
                        }
                    if ((char ) sql__setrang(ptoc_Var113->a_execute_trigger , 0 , 1))
                        {
                        _ptoc_MOVE(unsigned char , 1, ptoc_Var115->boolean.C_false.ld_infoset, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var121 , long_desc.boolean.C_false.ld_infoset , ptoc_Var112 , sizeof(unsigned char [1])));
                        }
                    g10mv11("VAK505" , 26 , sizeof(tsp00_LongDescriptor ) , ptoc_Var113->a_data_length , &long_desc , 1 , sql__nil(ptoc_Var113->a_data_ptr , "vak505.p" , 17155) , (int ) ptoc_Var114->d_first_longdesc_pos + cnt * (desc_len + 1) + 1 , desc_len , &(*(tsp1_segment *) sql__nil(ptoc_Var113->a_return_segm , "vak505.p" , 17157)).integer.C_4.sp1r_returncode);
                    if (cnt == ptoc_Var116)
                        {
                        break;
                        }
                    cnt += 1;
                    }
                while (!0);
                }
            }
        }
    t01moveobj(5 , sql__nil(ptoc_Var113->a_data_ptr , "vak505.p" , 17162) , 1 , ptoc_Var113->a_data_length);
    t01int4(5 , "d_cnt_longde" , (int ) ptoc_Var114->d_cnt_longdesc);
    only_move = 0;
    out_cnt = 0;
    cnt = 0;
    while ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var113->a_return_segm , "vak505.p" , 17167)).integer.C_4.sp1r_returncode == 0 && cnt < (int ) ptoc_Var114->d_cnt_longdesc)
        {
        t01int4(5 , "aktuellescnt" , cnt);
        t01int4(5 , "first_longde" , (int ) ptoc_Var114->d_first_longdesc_pos);
        t01moveobj(5 , sql__nil(ptoc_Var113->a_data_ptr , "vak505.p" , 17172) , (int ) ptoc_Var114->d_first_longdesc_pos + cnt * (desc_len + 1) , (int ) ptoc_Var114->d_first_longdesc_pos + cnt * (desc_len + 1) + desc_len);
        t01int4(5 , "only_move   " , sql__ord((int ) (char ) sql__setrang(only_move , 0 , 1)));
        if (ptoc_Var113->a_curr_retpart != (tsp1_part *) (int *) 0 && (*(tsp1_part *) sql__nil(ptoc_Var113->a_curr_retpart , "vak505.p" , 17177)).integer.C_2.sp1p_buf_len > 0)
            {
            t01int4(5 , "ret_buf_len " , (*(tsp1_part *) sql__nil(ptoc_Var113->a_curr_retpart , "vak505.p" , 17180)).integer.C_2.sp1p_buf_len);
            t01moveobj(5 , (*(tsp1_part *) sql__nil(ptoc_Var113->a_curr_retpart , "vak505.p" , 17181)).integer.C_1.sp1p_buf , 1 , (*(tsp1_part *) sql__nil(ptoc_Var113->a_curr_retpart , "vak505.p" , 17182)).integer.C_2.sp1p_buf_len);
            }
        if ((int ) (*(tsp00_MoveObj *) sql__nil(ptoc_Var113->a_data_ptr , "vak505.p" , 17185))[sql__setrang((int ) ptoc_Var114->d_first_longdesc_pos + cnt * (desc_len + 1) , 1 , 8192000) + -1] != 255)
            {
            if ((char ) sql__setrang(only_move , 0 , 1))
                {
                a06retpart_move(&(*acv) , (tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var113->a_data_ptr , "vak505.p" , 17192))[sql__setrang((int ) ptoc_Var114->d_first_longdesc_pos + cnt * (desc_len + 1) , 1 , 8192000) + -1] , desc_len + 1);
                out_cnt = sql__succ(out_cnt , -2147483647 , 2147483647 , "vak505.p" , 17194);
                }
            else
                {
                (*(tsp1_part *) sql__nil(ptoc_Var113->a_data_part , "vak505.p" , 17198)).integer.C_2.sp1p_arg_count = 1;
                ptoc_Var113->a_in_internal_subtr = 0;
                a507_putval(&(*acv) , (int ) ptoc_Var114->d_first_longdesc_pos + cnt * (desc_len + 1) , 1);
                if (ptoc_Var113->a_curr_retpart != (tsp1_part *) (int *) 0 && (*(tsp1_part *) sql__nil(ptoc_Var113->a_curr_retpart , "vak505.p" , 17203)).integer.C_2.sp1p_buf_len > 0)
                    {
                    out_cnt = 1;
                    only_move = 1;
                    }
                else
                    {
                    ptoc_Var113->a_in_internal_subtr = 1;
                    }
                }
            }
        cnt = sql__succ(cnt , -2147483647 , 2147483647 , "vak505.p" , 17215);
        }
    if (out_cnt > 0)
        {
        a06finish_curr_retpart(&(*acv) , 18 , (short ) sql__setrang((short ) out_cnt , -32768 , 32767));
        ptoc_Var113->a_in_internal_subtr = 0;
        ptoc_Var113->a_execute_trigger = 0;
        }
    t01moveobj(5 , sql__nil(ptoc_Var113->a_data_ptr , "vak505.p" , 17226) , 1 , ptoc_Var113->a_data_length);
    cccprint("<ak505long_suppo");
    }
