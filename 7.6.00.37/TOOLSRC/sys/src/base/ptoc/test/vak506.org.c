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
  tsp00_C20 tsp00_Timestamp;
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
  unsigned char tsp_c9[9];
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
  tsp00_Timestamp tsp_timestamp;
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
    } tsp_long_descriptor;
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
  unsigned char tgg00_MessType2;
typedef
  unsigned char tgg00_StackEntryType;
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
        tgg00_StackEntryType etype;
        unsigned char eop;
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
        tgg00_DateTimeFormat eformat;
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
      unsigned char mb_type;
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
  tgg00_BasisError tgg_basis_error;
typedef
  struct ptoc_Type49
    {
    tsp00_CodeType ctype;
    tsp00_CodeType char_default;
    tgg00_DateTimeFormat date_time;
    unsigned char kernel_swap;
    } tgg_code_globals;
typedef
  struct ptoc_Type50
    {
    tsp00_C256 tables[8];
    tsp00_C256 termset[8];
    tsp00_C256 inv_termset[8];
    } tgg_code_tables;
typedef
  tgg00_DataPartPtr tgg_datapart_ptr;
typedef
  tgg00_DateTimeFormat tgg_datetimeformat;
typedef
  unsigned char tgg_linkset[1];
typedef
  struct tgg00_MessBlock tgg_mess_block;
typedef
  tgg00_MessBlockPtr tgg_mess_block_ptr;
typedef
  tgg00_MessType2 tgg_message2_type;
typedef
  unsigned char tgg_privilege_set[1];
typedef
  tgg00_QualBufPtr tgg_qual_buf_ptr;
typedef
  tgg00_Rec tgg_rec_buf;
typedef
  struct ptoc_Type51
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
        unsigned char sfp_bd_mess_type;
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
        tgg00_DateTimeFormat sfp_dateTimeFormat;
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
    } tgg_select_fields_param;
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
  tgg00_StackDesc tgg_stack_desc;
typedef
  tgg00_StackEntry tgg_stack_entry;
typedef
  tgg00_StackEntryType tgg_stack_entry_type;
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
    tgg00_FileId file_id;
    tgg00_FileId inv_id;
    } tgg_two_tree_ids;
typedef
  unsigned char tak00_ooption[1];
typedef
  struct ptoc_Type55
    {
    struct ptoc_Type56
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
  struct ptoc_Type60
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
  struct ptoc_Type142
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
  unsigned char tak_fp_kind_type;
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
    tak_fp_kind_type fp_kind;
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
        tak_fp_kind_type fp_glob_fp_kind;
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
    } tak_field_pars;
typedef
  struct ptoc_Type147
    {
    tsp_int2 all_lock_count;
    tsp_int2 all_lock_fill;
    tgg00_Lock all_locks[240];
    } tak_ak_all_locks;
typedef
  tak_field_pars tak_parser_fields[2025];
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
    unsigned char d_datatype;
    unsigned char d_allowed;
    tak_fp_kind_type d_pars_kind;
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
  struct ptoc_Type176
    {
    tsp_int2 fr_f_no;
    tsp_int2 fr_last_fno;
    tsp_int2 fr_total_leng;
    tsp_int2 fr_leng;
    tsp_c2 fr_uni_fill_c2;
    tsp_int2 fr_filler0;
    } tak_fill_rec;
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
tsp_knl_identifier a01_il_b_identifier;
tsp_knl_identifier a01_zero_res_name;
tgg_sysinfokey a01defaultkey;
tgg_code_globals g01code;
char g01unicode;
tgg_code_tables g02codetables;
int a05constraint_fulfilled();
int a05_stamp_get();
int a05_transaction_get();
int a05_nextval_get();
int a05qualification_test();
int a05serial();
int a05timestamp_get();
int a05_verify_not_null();
int a07_hex_uni_error();
int a07_b_put_error();
int a07_nb_put_error();
int a10get_sysinfo();
int a55_const_from_resbuf();
int a56one_default_value();
int a56syskey();
int a56temp_table_key();
int a58_get_currentkey();
int a641_get_name();
int k71col_select();
int k720_test_subquery();
int g02pebcdic_pos_ascii();
int g02p1ebcdic_pos_ascii();
int g02pascii_pos_ebcdic();
int g02p1ascii_pos_ebcdic();
int g02fromtermchar();
int g03fdcheck_date();
int g03fdc_date();
int g03ftcheck_time();
int g03ftc_time();
int g03dfchange_format_date();
int g03tfchange_format_time();
int g03tsfchange_format_timestamp();
int g04init_select_fields();
int g10mv();
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
int g10fil();
int g10filuni();
int s10mv1();
int s30lnr();
int s30lnr_defbyte();
int s30unilnr();
int s30map();
int s41plrel();
int s49build_pattern();
int s49uni_build_pattern();
int s51kroun();
int s80uni_trans();
int vtimestamp();
int vUTCtimestamp();
double vTimeZoneDelta();
int t01int4();
int t01buf();
int t01buf1();
int t01char();
int t01name();
int t01messblock();
int cccprint();
int ak506normal_value();
int a506fieldvalues();
int a506lock_fieldvalues();
int a506result_name();
int ak506catalog_val_from_datapart();
int ak506datetime();
int ak506in_mess_buf_move();
int ak506long_support();
int ak506one_field_copy();
int ak506pseudo_insert_select();
int ak506val_from_part2();

static
  tak00_colpropset ptoc_Var0 =
  {
  (unsigned char)'\0', (unsigned char)'\0'
  };
static
  tak00_colpropset ptoc_Var1 =
  {
  (unsigned char)'\0', (unsigned char)'\0'
  };


int ak506normal_value(acv, dmli, frec, result, ind, currentdif)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_fill_rec *frec;
tsp_moveobj result;
int ind;
int *currentdif;
    {
    tgg_basis_error b_err;
    tak00_columninfo one_col;
    char stop_search;
    char patt_ok;
    char to_fill;
    char fill_part1;
    char ascii_type;
    char len_add;
    int lendiff;
    int iolen;
    int digits;
    int seq_exp;
    int errorpos;
    int frompos;
    int move_len;
    int start;
    int k;
    int i;
    tsp_int_map_c2 ic2;
    tak_sysbufferaddress seq;
    tgg_sysinfokey sysk;
    tsp_knl_identifier seq_name;
    struct ptoc_Type179
      {
      tgg_seq_info seq_info;
      tgg_surrogate seq_id;
      char seq_next;
      char seq_fill;
      } sequenceinfo;
    tak_all_command_glob *ptoc_Var2;
    struct tgg00_MessBlock *ptoc_Var3;
    tgg00_DataPart *ptoc_Var4;
    tak_dml_info *ptoc_Var5;
    tak_parsinforecord *ptoc_Var6;
    tak_field_pars *ptoc_Var7;
    tak_fill_rec *ptoc_Var8;
    int ptoc_Var9;
    tak00_columninfo *ptoc_Var10;
    tgg00_StackEntry *ptoc_Var11;
    struct ptoc_Type179 *ptoc_Var12;
    tgg00_StackEntry *ptoc_Var13;
    cccprint("AK506 ak506norma");
    ptoc_Var2 = &*acv;
    ptoc_Var3 = &ptoc_Var2->a_mblock;
    ptoc_Var4 = (tgg00_DataPart *) sql__nil(ptoc_Var3->integer.C_3.mb_data , "vak506.p" , 13742);
    ptoc_Var5 = &*dmli;
    ptoc_Var6 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var5->d_sparr.px[0] , "vak506.p" , 13743)).integer.C_23.sparsinfo;
    ptoc_Var7 = &ptoc_Var6->integer.C_2.p_pars_infos[sql__setrang(ind , 1 , 2025) + -1];
    ptoc_Var8 = &*frec;
    t01int4(5 , "-fp_movebefo" , (int ) ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_movebefore_v1);
    t01int4(5 , "-fp_frompos " , ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1);
    t01int4(5 , "-fp_extcolno" , (int ) ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_rangeextcolno);
    t01int4(5 , "-fp_inoutlen" , (int ) ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_inoutlen_v1);
    if (1 && (ptoc_Var7->fp_colset[0] & 1) != 0)
        {
        t01name(5 , "OPTIONAL          ");
        }
    if (1 && (ptoc_Var7->fp_colset[0] & 2) != 0)
        {
        t01name(5 , "LIKE CHECK        ");
        }
    if (1 && (ptoc_Var7->fp_colset[0] & 32) != 0)
        {
        t01name(5 , "DEFAULT           ");
        }
    if (1 && (ptoc_Var7->fp_colset[0] & 64) != 0)
        {
        t01name(5 , "ESCAPE            ");
        }
    if (1 && (ptoc_Var7->fp_colset[0] & 16) != 0)
        {
        t01name(5 , "SERIAL            ");
        }
    fill_part1 = 0;
    if ((int ) ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_movebefore_v1 > 0)
        {
        if ((int ) ptoc_Var7->fp_kind == 20)
            {
            ptoc_Var8->fr_total_leng = (short ) sql__setrang((short ) ((int ) ptoc_Var7->tak_fp_kind_type.C_fp_val_all_without_l.fp_movebefore_v2 - ptoc_Var3->integer.C_2.mb_data_len) , -32768 , 32767);
            if ((int ) ptoc_Var8->fr_total_leng > 0)
                {
                ak506in_mess_buf_move(&(*acv) , &(*dmli) , &(*frec));
                }
            }
        else
            {
            ptoc_Var8->fr_total_leng = (short ) sql__setrang(ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_movebefore_v1 , -32768 , 32767);
            ak506in_mess_buf_move(&(*acv) , &(*dmli) , &(*frec));
            }
        }
    lendiff = 0;
    iolen = (int ) ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_inoutlen_v1;
    if (((int ) ptoc_Var7->fp_kind == 18 && (int ) ptoc_Var7->fp_etype == 13 || (int ) ptoc_Var7->fp_kind == 19) && iolen > 255)
        {
        if ((int ) ptoc_Var7->fp_datatyp == 24 && (int ) ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_len_and_frac_v1.boolean.C_true.lf_datalen * 2 + 1 <= 255 || ((ptoc_Var9 = (int ) (*(tsp1_packet *) sql__nil(ptoc_Var2->a_in_packet , "vak506.p" , 13801)).sp1_header.sp1h_mess_code) == 19 || ptoc_Var9 == 20) && ((ptoc_Var9 = (int ) ptoc_Var7->fp_datatyp) == 2 || ptoc_Var9 == 3 || ptoc_Var9 == 31 || ptoc_Var9 == 32) && (int ) ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_len_and_frac_v1.boolean.C_true.lf_datalen + 1 <= 255)
            {
            lendiff = 1;
            }
        else
            {
            lendiff = 2;
            }
        }
    else
        {
        if ((int ) ptoc_Var7->fp_kind == 18 && (int ) ptoc_Var7->fp_etype == 3 || (int ) ptoc_Var7->fp_kind == 19)
            {
            lendiff = 1;
            }
        }
    start = ptoc_Var3->integer.C_2.mb_data_len + lendiff + 1;
    ptoc_Var3->integer.C_2.mb_data_len = ptoc_Var3->integer.C_2.mb_data_len + lendiff;
    ptoc_Var10 = &one_col;
    ptoc_Var11 = &ptoc_Var10->integer.C_1.ccolstack;
    ptoc_Var10->integer.C_2.cbinary = 0;
    to_fill = 0;
    if ((int ) ptoc_Var7->fp_kind == 20)
        {
        if ((int ) ptoc_Var7->tak_fp_kind_type.C_fp_val_all_without_l.fp_stackpos <= 0 || (int ) ptoc_Var7->tak_fp_kind_type.C_fp_val_all_without_l.fp_stackpos >= (int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var2->a_mblock.integer.C_4.mb_qual , "vak506.p" , 13830)).integer.C_2.mfirst_free)
            {
            to_fill = 1;
            }
        else
            {
            if ((ptoc_Var9 = (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var3->integer.C_5.mb_st , "vak506.p" , 13834))[sql__setrang((int ) ptoc_Var7->tak_fp_kind_type.C_fp_val_all_without_l.fp_stackpos , 1 , 32767) + -1].integer.C_1.etype) != 22 && ptoc_Var9 != 39 || (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var3->integer.C_5.mb_st , "vak506.p" , 13836))[sql__setrang((int ) ptoc_Var7->tak_fp_kind_type.C_fp_val_all_without_l.fp_stackpos , 1 , 32767) + -1].integer.C_1.epos != start)
                {
                to_fill = 1;
                }
            else
                {
                if ((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var3->integer.C_5.mb_st , "vak506.p" , 13840))[sql__setrang((int ) ptoc_Var7->tak_fp_kind_type.C_fp_val_all_without_l.fp_stackpos , 1 , 32767) + -1].integer.C_1.etype == 22 && ((ptoc_Var9 = (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var3->integer.C_5.mb_st , "vak506.p" , 13841))[sql__setrang((int ) ptoc_Var7->tak_fp_kind_type.C_fp_val_all_without_l.fp_stackpos , 1 , 32767) + -1].integer.C_1.ecol_tab[0]) == 3 || ptoc_Var9 == 4 || (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var3->integer.C_5.mb_st , "vak506.p" , 13843))[sql__setrang((int ) ptoc_Var7->tak_fp_kind_type.C_fp_val_all_without_l.fp_stackpos , 1 , 32767) + -1].integer.C_1.ecol_tab[0] == 0 && (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var3->integer.C_5.mb_st , "vak506.p" , 13844))[sql__setrang((int ) ptoc_Var7->tak_fp_kind_type.C_fp_val_all_without_l.fp_stackpos , 1 , 32767) + -1].integer.C_1.ecol_tab[1] != 0))
                    {
                    to_fill = 1;
                    }
                }
            }
        }
    else
        {
        if ((int ) ptoc_Var7->fp_datatyp == 24 && (int ) ptoc_Var7->fp_kind == 18 && ((ptoc_Var9 = (int ) ptoc_Var7->fp_etype) == 0 || ptoc_Var9 == 2))
            {
            to_fill = 1;
            }
        }
    ptoc_Var10->integer.C_1.cdatafrac = (unsigned char)'\001';
    if ((ptoc_Var9 = (int ) ptoc_Var7->fp_kind) == 18 || ptoc_Var9 == 19 || ptoc_Var9 == 24)
        {
        ptoc_Var10->integer.C_1.creccolno = (short ) sql__setrang(ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_rangeextcolno , -32768 , 32767);
        ptoc_Var10->integer.C_1.cextcolno = (short ) sql__setrang(ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_rangeextcolno , -32768 , 32767);
        }
    else
        {
        ptoc_Var10->integer.C_1.creccolno = 0;
        ptoc_Var10->integer.C_1.cextcolno = 0;
        }
    errorpos = ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1;
    frompos = ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1;
    ptoc_Var10->integer.C_1.cinoutlen = (short ) sql__setrang((short ) iolen , -32768 , 32767);
    ptoc_Var10->integer.C_1.ccolumnn_len = (unsigned char)'\001';
    _ptoc_ASSIGN(unsigned char , 64, ptoc_Var10->integer.C_1.ccolumnn, a01_il_b_identifier)
    _ptoc_MOVE(unsigned char , 2, ptoc_Var10->integer.C_1.ccolpropset, ptoc_Var0);
    ptoc_Var11->integer.C_1.etype = (unsigned char ) sql__setrang(ptoc_Var7->fp_etype , 0 , 67);
    ptoc_Var11->integer.C_1.eop = 0;
    ptoc_Var11->integer.C_1.epos = 0;
    ptoc_Var11->integer.C_1.elen_var = 0;
    ptoc_Var11->integer.C_1.ecol_tab[0] = (unsigned char)'\0';
    ptoc_Var11->integer.C_1.ecol_tab[1] = (unsigned char)' ';
    if (ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 < 0 && (int ) ptoc_Var7->fp_kind != 24)
        {
        if (ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 < -9)
            {
            move_len = 0;
            if (1 && (ptoc_Var7->fp_colset[0] & 16) != 0)
                {
                ptoc_Var10->integer.C_1.cdatalen = (short ) sql__setrang((short ) ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_len_and_frac_v1.boolean.C_false.lf_fixed_datalen , -32768 , 32767);
                if ((int ) ptoc_Var10->integer.C_1.cinoutlen > 0)
                    {
                    ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(start , 1 , 8192000) + -1] = (unsigned char)'\0';
                    ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(start + 1 , 1 , 8192000) + -1] = (unsigned char)'\200';
                    k = 2;
                    }
                else
                    {
                    start = start + (int ) ptoc_Var10->integer.C_1.cinoutlen;
                    k = -(int ) ptoc_Var10->integer.C_1.cinoutlen;
                    to_fill = 0;
                    }
                a05serial(&(*acv) , &one_col , &(*(tgg00_QualBuf *) sql__nil(ptoc_Var3->integer.C_4.mb_qual , "vak506.p" , 13916)).integer.C_2.mtree , ptoc_Var4->integer.C_1.mbp_buf , ptoc_Var3->integer.C_2.mb_data_size , start , &k);
                if ((int ) ptoc_Var10->integer.C_1.cinoutlen > 0)
                    {
                    ptoc_Var8->fr_leng = (short ) sql__setrang((short ) k , -32768 , 32767);
                    }
                else
                    {
                    ptoc_Var8->fr_leng = 0;
                    }
                }
            else
                {
                if ((int ) ptoc_Var7->fp_kind == 20)
                    {
                    ptoc_Var8->fr_total_leng = (short ) sql__setrang(ptoc_Var7->tak_fp_kind_type.C_fp_val_all_without_l.fp_inoutlen_v2 , -32768 , 32767);
                    ak506in_mess_buf_move(&(*acv) , &(*dmli) , &(*frec));
                    }
                move_len = sizeof(struct ptoc_Type52 ) + sizeof(tsp00_C8 ) + sizeof(char );
                sequenceinfo.seq_fill = 0;
                ptoc_Var8->fr_total_leng = (short ) sql__setrang((short ) move_len , -32768 , 32767);
                ak506in_mess_buf_move(&(*acv) , &(*dmli) , &(*frec));
                ptoc_Var3->integer.C_2.mb_data_len = ptoc_Var3->integer.C_2.mb_data_len - move_len;
                if (((ptoc_Var9 = (int ) ptoc_Var7->fp_kind) == 18 || ptoc_Var9 == 23) && ((ptoc_Var9 = (int ) ptoc_Var7->fp_etype) == 1 || ptoc_Var9 == 0))
                    {
                    ptoc_Var4->integer.C_4.mbp_keylen = (short ) sql__setrang((short ) ((int ) ptoc_Var4->integer.C_4.mbp_keylen - move_len) , -32768 , 32767);
                    }
                g10mv4("VAK506" , 1 , ptoc_Var3->integer.C_2.mb_data_size , sizeof(struct ptoc_Type52 ) + sizeof(tsp00_C8 ) + sizeof(char ) , ptoc_Var4->integer.C_1.mbp_buf , ptoc_Var3->integer.C_2.mb_data_len + 1 , &sequenceinfo.seq_info , 1 , move_len , &(*(tsp1_segment *) sql__nil(ptoc_Var2->a_return_segm , "vak506.p" , 13954)).integer.C_4.sp1r_returncode);
                ptoc_Var12 = &sequenceinfo;
                if ((char ) sql__setrang(ptoc_Var12->seq_next , 0 , 1))
                    {
                    a05_nextval_get(&(*acv) , 0 , ptoc_Var12->seq_id , &ptoc_Var12->seq_info , ptoc_Var4->integer.C_1.mbp_buf , start , &k);
                    }
                else
                    {
                    a05_nextval_get(&(*acv) , 58 , ptoc_Var12->seq_id , &ptoc_Var12->seq_info , ptoc_Var4->integer.C_1.mbp_buf , start , &k);
                    }
                (*frec).fr_leng = (short ) sql__setrang((short ) k , -32768 , 32767);
                if ((int ) ptoc_Var7->fp_datatyp == 1)
                    {
                    digits = (int ) ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_len_and_frac_v1.boolean.C_false.lf_fixed_datalen;
                    }
                else
                    {
                    digits = (int ) ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_len_and_frac_v1.boolean.C_false.lf_fixed_datalen - (int ) ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_len_and_frac_v1.boolean.C_false.lf_frac + 1;
                    }
                seq_exp = sql__ord((int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(start + 1 , 1 , 8192000) + -1]);
                if (digits < seq_exp + -192 || digits < 64 - seq_exp)
                    {
                    sysk = a01defaultkey;
                    _ptoc_ASSIGN(unsigned char , 8, sysk.integer.C_2.stableid, sequenceinfo.seq_id)
                    _ptoc_ASSIGN(unsigned char , 2, sysk.integer.C_2.sentrytyp, "\0R")
                    a10get_sysinfo(&(*acv) , &sysk , 1 , &seq , &b_err);
                    if ((int ) b_err == 0)
                        {
                        _ptoc_ASSIGN(unsigned char , 64, seq_name, (*(struct tak_systembuffer *) sql__nil(seq , "vak506.p" , 13985)).integer.C_30.ssequence.seq_name)
                        }
                    else
                        {
                        _ptoc_ASSIGN(unsigned char , 64, seq_name, a01_il_b_identifier)
                        }
                    a07_nb_put_error(&(*acv) , 4330 , 1 , seq_name);
                    }
                }
            if ((int ) ptoc_Var10->integer.C_1.cinoutlen > 0 && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var2->a_return_segm , "vak506.p" , 13994)).integer.C_4.sp1r_returncode == 0)
                {
                if ((char ) sql__setrang(to_fill , 0 , 1) || ((ptoc_Var9 = (int ) ptoc_Var7->fp_etype) == 0 || ptoc_Var9 == 2))
                    {
                    if (iolen > (int ) (*frec).fr_leng)
                        {
                        g10fil("VAK506" , 2 , ptoc_Var3->integer.C_2.mb_data_size , ptoc_Var4->integer.C_1.mbp_buf , start + (int ) (*frec).fr_leng , iolen - (int ) (*frec).fr_leng , (unsigned char)'\0' , &(*(tsp1_segment *) sql__nil(ptoc_Var2->a_return_segm , "vak506.p" , 14006)).integer.C_4.sp1r_returncode);
                        }
                    (*frec).fr_leng = (short ) sql__setrang((short ) iolen , -32768 , 32767);
                    }
                }
            if ((int ) ptoc_Var7->fp_kind == 20)
                {
                ptoc_Var3->integer.C_2.mb_data_len = ptoc_Var3->integer.C_2.mb_data_len + move_len;
                }
            *currentdif = *currentdif + move_len - (int ) (*frec).fr_leng;
            }
        else
            {
            ptoc_Var10->integer.C_1.cinoutlen = (short ) sql__setrang((short ) iolen , -32768 , 32767);
            switch ((int ) ptoc_Var7->fp_datatyp)
                {
                case 10 :
                    ptoc_Var10->integer.C_1.cinoutlen = 9;
                    break;
                case 11 :
                    ptoc_Var10->integer.C_1.cinoutlen = 9;
                    break;
                case 13 :
                    ptoc_Var10->integer.C_1.cinoutlen = 21;
                    break;
                case 24 :
                    ptoc_Var10->integer.C_1.cinoutlen = (short ) sql__setrang((short ) ((int ) ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_len_and_frac_v1.boolean.C_true.lf_datalen * 2 + 1) , -32768 , 32767);
                    break;
                case 2 :
                case 3 :
                case 31 :
                case 32 :
                    ptoc_Var10->integer.C_1.cinoutlen = (short ) sql__setrang((short ) ((int ) ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_len_and_frac_v1.boolean.C_true.lf_datalen + 1) , -32768 , 32767);
                    break;
                default :
                    break;
                }
            t01int4(5 , "cinoutlen   " , (int ) ptoc_Var10->integer.C_1.cinoutlen);
            t01int4(5 , "iolen       " , iolen);
            ak506datetime(&(*acv) , &ptoc_Var6->integer.C_2.p_pars_infos[sql__setrang((int ) ptoc_Var8->fr_f_no , 1 , 2025) + -1] , &(*frec) , (int ) ptoc_Var10->integer.C_1.cinoutlen , (char ) sql__setrang(to_fill , 0 , 1) , start);
            }
        frompos = -1;
        }
    else
        {
        ptoc_Var10->integer.C_1.cdatafrac = (unsigned char)'\0';
        t01int4(5 , "cinoutlen   " , (int ) ptoc_Var10->integer.C_1.cinoutlen);
        t01int4(5 , "fp_datatyp  " , sql__ord((int ) ptoc_Var7->fp_datatyp));
        if ((ptoc_Var9 = (int ) ptoc_Var7->fp_datatyp) == 0 || ptoc_Var9 == 29 || ptoc_Var9 == 30)
            {
            ptoc_Var10->integer.C_1.cdatatyp = 0;
            ptoc_Var10->integer.C_1.cdatalen = (short ) sql__setrang((short ) ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_len_and_frac_v1.boolean.C_false.lf_fixed_datalen , -32768 , 32767);
            ptoc_Var10->integer.C_1.cdatafrac = (unsigned char ) sql__setrang(ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_len_and_frac_v1.boolean.C_false.lf_frac , 0 , 255);
            if ((int ) ptoc_Var7->fp_datatyp != 0)
                {
                ptoc_Var10->integer.C_2.cbinary = 1;
                }
            }
        else
            {
            if ((int ) ptoc_Var7->fp_datatyp == 1)
                {
                ptoc_Var10->integer.C_1.cdatatyp = 1;
                ptoc_Var10->integer.C_1.cdatalen = (short ) sql__setrang((short ) ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_len_and_frac_v1.boolean.C_false.lf_fixed_datalen , -32768 , 32767);
                ptoc_Var10->integer.C_1.cdatafrac = (unsigned char ) sql__setrang(ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_len_and_frac_v1.boolean.C_false.lf_frac , 0 , 255);
                }
            else
                {
                ptoc_Var10->integer.C_1.cdatatyp = (unsigned char ) sql__setrang(ptoc_Var7->fp_datatyp , 0 , 39);
                ptoc_Var10->integer.C_1.cdatalen = (short ) sql__setrang(ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_len_and_frac_v1.boolean.C_true.lf_datalen , -32768 , 32767);
                }
            }
        t01int4(5 , "startpos    " , start);
        ptoc_Var10->integer.C_1.cinoutlen = (short ) sql__setrang((short ) iolen , -32768 , 32767);
        switch ((int ) ptoc_Var10->integer.C_1.cdatatyp)
            {
            case 10 :
                ptoc_Var10->integer.C_1.cinoutlen = 9;
                break;
            case 11 :
                ptoc_Var10->integer.C_1.cinoutlen = 9;
                break;
            case 13 :
                ptoc_Var10->integer.C_1.cinoutlen = 21;
                break;
            case 24 :
                ptoc_Var10->integer.C_1.cinoutlen = (short ) sql__setrang((short ) ((int ) ptoc_Var10->integer.C_1.cdatalen * 2 + 1) , -32768 , 32767);
                break;
            case 2 :
            case 3 :
            case 31 :
            case 32 :
                ptoc_Var10->integer.C_1.cinoutlen = (short ) sql__setrang((short ) ((int ) ptoc_Var10->integer.C_1.cdatalen + 1) , -32768 , 32767);
                break;
            default :
                break;
            }
        t01int4(5 , "cinoutlen   " , (int ) ptoc_Var10->integer.C_1.cinoutlen);
        if ((int ) ptoc_Var7->fp_kind == 24)
            {
            a55_const_from_resbuf(&(*acv) , result , &one_col , (unsigned char ) sql__setrang(ptoc_Var7->tak_fp_kind_type.C_fp_val_from_res_buffer.fp_glob_fp_kind , 0 , 24) , &start , &i , (int ) ptoc_Var7->tak_fp_kind_type.C_fp_val_from_res_buffer.fp_res_buf_index);
            ptoc_Var8->fr_leng = (short ) sql__setrang((short ) i , -32768 , 32767);
            }
        else
            {
            if (ptoc_Var2->a_data_ptr == (tsp00_MoveObj *) (int *) 0)
                {
                a07_b_put_error(&(*acv) , 6488 , 1);
                }
            else
                {
                if ((int ) (*(tsp00_MoveObj *) sql__nil(ptoc_Var2->a_data_ptr , "vak506.p" , 14109))[sql__setrang(ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 , 1 , 8192000) + -1] == 253)
                    {
                    if (1 && (ptoc_Var7->fp_colset[0] & 32) != 0)
                        {
                        ptoc_Var3->integer.C_2.mb_data_len = ptoc_Var3->integer.C_2.mb_data_len - lendiff;
                        lendiff = 0;
                        _ptoc_MOVE(unsigned char , 2, ptoc_Var10->integer.C_1.ccolpropset, ptoc_Var1);
                        a56one_default_value(&(*acv) , &(*dmli) , &one_col , (unsigned char ) sql__setrang(ptoc_Var7->fp_kind , 0 , 24));
                        if (ptoc_Var2->a_input_data_pos < ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + iolen)
                            {
                            ptoc_Var2->a_input_data_pos = ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + iolen;
                            }
                        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var2->a_return_segm , "vak506.p" , 14123)).integer.C_4.sp1r_returncode == 0)
                            {
                            if ((int ) ptoc_Var7->fp_etype == 1 || (int ) ptoc_Var7->fp_etype == 0)
                                {
                                ptoc_Var8->fr_leng = (short ) sql__setrang((short ) (ptoc_Var3->integer.C_2.mb_data_len - (int ) ptoc_Var4->integer.C_4.mbp_keylen + -8) , -32768 , 32767);
                                }
                            }
                        }
                    else
                        {
                        a07_b_put_error(&(*acv) , 6234 , -errorpos);
                        }
                    }
                else
                    {
                    ak506val_from_part2(&(*acv) , ptoc_Var7->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 , (unsigned char ) sql__setrang(ptoc_Var7->fp_kind , 0 , 24) , &one_col , iolen , &(*frec) , &to_fill);
                    }
                }
            }
        if (1 && (ptoc_Var7->fp_colset[0] & 16) != 0)
            {
            i = (int ) ptoc_Var8->fr_leng;
            a05serial(&(*acv) , &one_col , &(*(tgg00_QualBuf *) sql__nil(ptoc_Var3->integer.C_4.mb_qual , "vak506.p" , 14149)).integer.C_2.mtree , ptoc_Var4->integer.C_1.mbp_buf , ptoc_Var3->integer.C_2.mb_data_size , start , &i);
            ptoc_Var8->fr_leng = (short ) sql__setrang((short ) i , -32768 , 32767);
            }
        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var2->a_return_segm , "vak506.p" , 14154)).integer.C_4.sp1r_returncode == 0 && (int ) ptoc_Var7->fp_kind == 20 && !(char ) sql__setrang(to_fill , 0 , 1))
            {
            (*(tgg00_StackList *) sql__nil(ptoc_Var3->integer.C_5.mb_st , "vak506.p" , 14159))[sql__setrang((int ) ptoc_Var7->tak_fp_kind_type.C_fp_val_all_without_l.fp_stackpos , 1 , 32767) + -1].integer.C_1.elen_var = (short ) sql__setrang(ptoc_Var8->fr_leng , -32768 , 32767);
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var2->a_return_segm , "vak506.p" , 14163)).integer.C_4.sp1r_returncode == 0 && !(char ) sql__setrang(fill_part1 , 0 , 1))
        {
        if (lendiff == 1)
            {
            ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(start + -1 , 1 , 8192000) + -1] = (unsigned char ) sql__setrang((unsigned char ) ptoc_Var8->fr_leng , 0 , 255);
            }
        else
            {
            if (lendiff == 2)
                {
                ic2.boolean.C_true.map_int = (short ) sql__setrang(ptoc_Var8->fr_leng , -32768 , 32767);
                ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(start + -2 , 1 , 8192000) + -1] = (unsigned char ) sql__setrang(ic2.boolean.C_false.map_c2[0] , 0 , 255);
                ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(start + -1 , 1 , 8192000) + -1] = (unsigned char ) sql__setrang(ic2.boolean.C_false.map_c2[1] , 0 , 255);
                }
            }
        if (((ptoc_Var9 = (int ) ptoc_Var7->fp_kind) == 18 || ptoc_Var9 == 23) && ((ptoc_Var9 = (int ) ptoc_Var7->fp_etype) == 1 || ptoc_Var9 == 0))
            {
            ptoc_Var4->integer.C_4.mbp_keylen = (short ) sql__setrang((short ) ((int ) ptoc_Var4->integer.C_4.mbp_keylen + (int ) ptoc_Var8->fr_leng) , -32768 , 32767);
            if ((int ) ptoc_Var7->fp_kind == 23 && (int ) ptoc_Var7->tak_fp_kind_type.C_fp_val_all_without_l.fp_stackpos != 0)
                {
                ptoc_Var13 = &(*(tgg00_StackList *) sql__nil(ptoc_Var3->integer.C_5.mb_st , "vak506.p" , 14189))[sql__setrang((int ) ptoc_Var7->tak_fp_kind_type.C_fp_val_all_without_l.fp_stackpos , 1 , 32767) + -1];
                ptoc_Var13->integer.C_1.elen_var = (short ) sql__setrang((short ) ((int ) ptoc_Var13->integer.C_1.elen_var + (int ) ptoc_Var8->fr_leng) , -32768 , 32767);
                }
            }
        if ((int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(start , 1 , 8192000) + -1] == 255)
            {
            if (0 || (ptoc_Var7->fp_colset[0] & 1) == 0)
                {
                a07_b_put_error(&(*acv) , 6336 , -errorpos);
                }
            }
        if (1 && (ptoc_Var7->fp_colset[0] & 2) != 0)
            {
            i = start + 1;
            stop_search = 0;
            if ((int ) ptoc_Var7->fp_datatyp == 24)
                {
                while (i < start + (int ) ptoc_Var8->fr_leng && !(char ) sql__setrang(stop_search , 0 , 1))
                    if (1 && (ptoc_Var7->fp_colset[0] & 64) != 0 && (int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == (int ) (*dmli).d_escape_char[0] && (int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(i + 1 , 1 , 8192000) + -1] == (int ) (*dmli).d_escape_char[1])
                        {
                        i = i + 4;
                        }
                    else
                        {
                        if ((int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 0 && ((int ) ptoc_Var2->a_sqlmode == 3 && ((int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(i + 1 , 1 , 8192000) + -1] == 42 || (int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(i + 1 , 1 , 8192000) + -1] == 63 || (int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(i + 1 , 1 , 8192000) + -1] == 40)) || (int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(i + 1 , 1 , 8192000) + -1] == 95 || (int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(i + 1 , 1 , 8192000) + -1] == 37)
                            {
                            stop_search = 1;
                            }
                        else
                            {
                            i = i + 2;
                            }
                        }
                }
            else
                {
                while (i < start + (int ) ptoc_Var8->fr_leng && !(char ) sql__setrang(stop_search , 0 , 1))
                    if (1 && (ptoc_Var7->fp_colset[0] & 64) != 0 && (int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == (int ) (*dmli).d_escape_char[1])
                        {
                        i = i + 2;
                        }
                    else
                        {
                        if ((int ) ptoc_Var7->fp_datatyp == 3 && ((int ) ptoc_Var2->a_sqlmode == 3 && ((int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 92 || (int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 111 || (int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 77) || (int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 109 || (int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 108) || (int ) ptoc_Var7->fp_datatyp != 3 && ((int ) ptoc_Var2->a_sqlmode == 3 && ((int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 42 || (int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 63 || (int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 40) || (int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 95 || (int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(i , 1 , 8192000) + -1] == 37))
                            {
                            stop_search = 1;
                            }
                        else
                            {
                            i = sql__succ(i , -2147483647 , 2147483647 , "vak506.p" , 14269);
                            }
                        }
                }
            t01int4(5 , "fr_leng     " , (int ) ptoc_Var8->fr_leng);
            t01int4(5 , "i           " , i);
            t01int4(5 , "cinoutlen   " , (int ) ptoc_Var10->integer.C_1.cinoutlen);
            t01int4(5 , "start       " , start);
            if (start + (int ) ptoc_Var8->fr_leng - i > 0)
                {
                if (1 && (ptoc_Var7->fp_colset[0] & 4) != 0)
                    {
                    if ((int ) ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(start , 1 , 8192000) + -1] == 1)
                        {
                        g10filuni("VAK506" , 3 , ptoc_Var3->integer.C_2.mb_data_size , ptoc_Var4->integer.C_1.mbp_buf , i , start + (int ) (*frec).fr_leng - i , "\0 " , &(*(tsp1_segment *) sql__nil(ptoc_Var2->a_return_segm , "vak506.p" , 14288)).integer.C_4.sp1r_returncode);
                        }
                    else
                        {
                        g10fil("VAK506" , 4 , ptoc_Var3->integer.C_2.mb_data_size , ptoc_Var4->integer.C_1.mbp_buf , i , start + (int ) ptoc_Var8->fr_leng - i , (unsigned char ) sql__setrang(ptoc_Var4->integer.C_1.mbp_buf[sql__setrang(start , 1 , 8192000) + -1] , 0 , 255) , &(*(tsp1_segment *) sql__nil(ptoc_Var2->a_return_segm , "vak506.p" , 14293)).integer.C_4.sp1r_returncode);
                        }
                    }
                else
                    {
                    if (1 && (ptoc_Var7->fp_colset[0] & 8) != 0)
                        {
                        g10fil("VAK506" , 5 , ptoc_Var3->integer.C_2.mb_data_size , ptoc_Var4->integer.C_1.mbp_buf , i , start + (int ) ptoc_Var10->integer.C_1.cinoutlen - i , (unsigned char)'\377' , &(*(tsp1_segment *) sql__nil(ptoc_Var2->a_return_segm , "vak506.p" , 14302)).integer.C_4.sp1r_returncode);
                        if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var2->a_return_segm , "vak506.p" , 14303)).integer.C_4.sp1r_returncode == 0 && (int ) ptoc_Var7->fp_kind == 20 && !(char ) sql__setrang(to_fill , 0 , 1))
                            {
                            (*(tgg00_StackList *) sql__nil(ptoc_Var3->integer.C_5.mb_st , "vak506.p" , 14308))[sql__setrang((int ) ptoc_Var7->tak_fp_kind_type.C_fp_val_all_without_l.fp_stackpos , 1 , 32767) + -1].integer.C_1.elen_var = (short ) sql__setrang(ptoc_Var10->integer.C_1.cinoutlen , -32768 , 32767);
                            }
                        }
                    }
                }
            patt_ok = 0;
            if ((int ) ptoc_Var7->fp_datatyp == 24)
                {
                s49uni_build_pattern(ptoc_Var4->integer.C_1.mbp_buf , start + 1 , start + (int ) ptoc_Var8->fr_leng + -1 , (*dmli).d_escape_char , (char ) sql__setrang(1 && (ptoc_Var7->fp_colset[0] & 64) != 0 , 0 , 1) , (unsigned char ) sql__setrang(ptoc_Var2->a_sqlmode , 0 , 3) , &patt_ok);
                }
            else
                {
                ascii_type = (char ) sql__setrang((int ) ptoc_Var7->fp_datatyp != 3 && ((int ) ptoc_Var7->fp_datatyp != 4 || (int ) g01code.ctype != 1) , 0 , 1);
                s49build_pattern(ptoc_Var4->integer.C_1.mbp_buf , (char ) sql__setrang(ascii_type , 0 , 1) , start + 1 , start + (int ) ptoc_Var8->fr_leng + -1 , (unsigned char ) sql__setrang((*dmli).d_escape_char[1] , 0 , 255) , (char ) sql__setrang(1 && (ptoc_Var7->fp_colset[0] & 64) != 0 , 0 , 1) , 0 , (unsigned char ) sql__setrang(ptoc_Var2->a_sqlmode , 0 , 3) , &patt_ok);
                }
            if (!(char ) sql__setrang(patt_ok , 0 , 1))
                {
                if ((int ) ptoc_Var2->a_sqlmode == 0)
                    {
                    a07_b_put_error(&(*acv) , 6562 , 1);
                    }
                else
                    {
                    a07_b_put_error(&(*acv) , 2550 , 1);
                    }
                }
            }
        if (frompos < 0 || (int ) ptoc_Var7->fp_kind == 24)
            {
            len_add = 1;
            }
        else
            {
            if ((int ) (*(tsp00_MoveObj *) sql__nil(ptoc_Var2->a_data_ptr , "vak506.p" , 14353))[sql__setrang(frompos , 1 , 8192000) + -1] != 253)
                {
                len_add = 1;
                }
            else
                {
                len_add = 0;
                }
            }
        if ((char ) sql__setrang(len_add , 0 , 1))
            {
            if ((int ) ptoc_Var7->fp_kind == 20)
                {
                ptoc_Var3->integer.C_2.mb_data_len = ptoc_Var3->integer.C_2.mb_data_len + (int ) ptoc_Var10->integer.C_1.cinoutlen;
                ptoc_Var5->d_pos_in_parsbuf = (short ) sql__setrang((short ) ((int ) ptoc_Var5->d_pos_in_parsbuf + (int ) ptoc_Var10->integer.C_1.cinoutlen) , -32768 , 32767);
                }
            else
                {
                ptoc_Var3->integer.C_2.mb_data_len = ptoc_Var3->integer.C_2.mb_data_len + (int ) ptoc_Var8->fr_leng;
                }
            }
        }
    cccprint("<ak506normal_val");
    }

static
  tsp8_uni_opt_set ptoc_Var14 =
  {
  (unsigned char)'\0'
  };
static
  tsp8_uni_opt_set ptoc_Var15 =
  {
  (unsigned char)'\0'
  };
static
  tsp8_uni_opt_set ptoc_Var16 =
  {
  (unsigned char)'\0'
  };
static
  tsp8_uni_opt_set ptoc_Var17 =
  {
  (unsigned char)'\0'
  };


int a506fieldvalues(acv, dmli, frec, viewkeybuf, result, resultBufSize)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_fill_rec *frec;
tak_sysbufferaddress viewkeybuf;
tsp_moveobj result;
tsp00_Int4 resultBufSize;
    {
    int currentdif;
    int k;
    int ind;
    tsp_c2 escape_c2;
    int uniinlen;
    int univallen;
    tsp_int4 data_len;
    tsp8_uni_error error;
    tsp_int4 err_char_no;
    tsp_knl_identifier currentnam;
    tgg_mess_block mblock;
    tak_all_command_glob *ptoc_Var18;
    tak_dml_info *ptoc_Var19;
    tak_fill_rec *ptoc_Var20;
    struct tgg00_MessBlock *ptoc_Var21;
    tgg00_DataPart *ptoc_Var22;
    tak_parsinforecord *ptoc_Var23;
    tak_field_pars *ptoc_Var24;
    tak_messblock_record *ptoc_Var25;
    tak_viewkeyrecord *ptoc_Var26;
    tsp8_uni_opt_set ptoc_Var27;
    int ptoc_Var28;
    cccprint("AK506 a506fieldv");
    currentdif = 0;
    data_len = 0;
    (*dmli).d_first_longdesc_pos = 0;
    (*dmli).d_cnt_longdesc = 0;
    (*dmli).d_longdesc_found = 0;
    if ((int ) (*(tsp1_packet *) sql__nil((*acv).a_in_packet , "vak506.p" , 14413)).sp1_header.sp1h_mess_code == 20 || (int ) (*(tsp1_packet *) sql__nil((*acv).a_in_packet , "vak506.p" , 14414)).sp1_header.sp1h_mess_swap == 1)
        {
        _ptoc_ASSIGN(unsigned char , 2, (*frec).fr_uni_fill_c2, "\0 ")
        }
    else
        {
        _ptoc_ASSIGN(unsigned char , 2, (*frec).fr_uni_fill_c2, " \0")
        }
    while ((int ) (*frec).fr_f_no <= (int ) (*frec).fr_last_fno && (int ) (*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak506.p" , 14421)).integer.C_4.sp1r_returncode == 0)
        {
        ind = (int ) (*frec).fr_f_no;
        ptoc_Var18 = &*acv;
        ptoc_Var19 = &*dmli;
        ptoc_Var20 = &*frec;
        ptoc_Var21 = &ptoc_Var18->a_mblock;
        ptoc_Var22 = (tgg00_DataPart *) sql__nil(ptoc_Var21->integer.C_3.mb_data , "vak506.p" , 14426);
        ptoc_Var23 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var19->d_sparr.px[0] , "vak506.p" , 14427)).integer.C_23.sparsinfo;
        ptoc_Var24 = &ptoc_Var23->integer.C_2.p_pars_infos[sql__setrang(ind , 1 , 2025) + -1];
        t01name(5 , "one parse-task :  ");
        t01int4(5 , "-reclen     " , (int ) ptoc_Var22->integer.C_4.mbp_reclen);
        t01int4(5 , "-datalen    " , ptoc_Var21->integer.C_2.mb_data_len);
        t01int4(5 , "-fp_kind    " , sql__ord((int ) ptoc_Var24->fp_kind));
        t01int4(5 , "-fp_etype   " , sql__ord((int ) ptoc_Var24->fp_etype));
        t01int4(5 , "-fp_datatyp " , sql__ord((int ) ptoc_Var24->fp_datatyp));
        t01buf1(5 , &ptoc_Var23->integer.C_2.p_pars_infos[sql__setrang(ind , 1 , 2025) + -1] , 5 , 16);
        t01int4(5 , "frec.fr_f_no" , (int ) (*frec).fr_f_no);
        t01int4(5 , "-fp_movebefo" , (int ) ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_movebefore_v1);
        t01int4(5 , "-fp_frompos " , ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1);
        t01int4(5 , "-fp_extcolno" , (int ) ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_rangeextcolno);
        t01int4(5 , "-fp_inoutlen" , (int ) ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_inoutlen_v1);
        switch ((int ) ptoc_Var24->fp_kind)
            {
            case 2 :
                break;
            case 0 :
                ak506catalog_val_from_datapart(&(*acv) , (unsigned char ) sql__setrang(ptoc_Var24->fp_etype , 0 , 67) , ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 , (int ) ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_inoutlen_v1);
                break;
            case 3 :
                ptoc_Var20->fr_total_leng = (short ) sql__setrang((short ) ((int ) ptoc_Var24->tak_fp_kind_type.C_fp_current_of_key.fp_curr_keylen + (int ) ptoc_Var24->tak_fp_kind_type.C_fp_current_of_key.fp_movebefore_v5) , -32768 , 32767);
                ak506in_mess_buf_move(&(*acv) , &(*dmli) , &(*frec));
                ptoc_Var21->integer.C_2.mb_data_len = ptoc_Var21->integer.C_2.mb_data_len - (int ) ptoc_Var24->tak_fp_kind_type.C_fp_current_of_key.fp_curr_keylen;
                g10mv3("VAK506" , 6 , ptoc_Var21->integer.C_2.mb_data_size , sizeof(unsigned char [64]) , ptoc_Var22->integer.C_1.mbp_buf , ptoc_Var21->integer.C_2.mb_data_len + 1 , currentnam , 1 , sizeof(unsigned char [64]) , &(*(tsp1_segment *) sql__nil(ptoc_Var18->a_return_segm , "vak506.p" , 14458)).integer.C_4.sp1r_returncode);
                a58_get_currentkey(&(*acv) , currentnam , ptoc_Var23->integer.C_2.p_tabid , 1);
                if ((int ) ptoc_Var21->integer.C_2.mb_type == 61 && (int ) ptoc_Var21->integer.C_2.mb_type2 != 16)
                    {
                    (*(tgg00_StackList *) sql__nil(ptoc_Var21->integer.C_5.mb_st , "vak506.p" , 14463))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var21->integer.C_4.mb_qual , "vak506.p" , 14463)).integer.C_2.mstrat_pos , 1 , 32767) + -1].integer.C_1.elen_var = (short ) sql__setrang(ptoc_Var22->integer.C_4.mbp_keylen , -32768 , 32767);
                    }
                currentdif = (int ) ptoc_Var24->tak_fp_kind_type.C_fp_current_of_key.fp_curr_keylen - (int ) ptoc_Var22->integer.C_4.mbp_keylen;
                break;
            case 5 :
                _ptoc_ASSIGN(unsigned char , 2, (*dmli).d_escape_char, ptoc_Var24->tak_fp_kind_type.C_fp_escape_val.fp_escape_char)
                break;
            case 6 :
                ptoc_Var20->fr_total_leng = (short ) sql__setrang((short ) ((int ) ptoc_Var24->tak_fp_kind_type.C_fp_fill_part2_to_len.fp_len - ptoc_Var21->integer.C_2.mb_data_len - currentdif) , -32768 , 32767);
                ak506in_mess_buf_move(&(*acv) , &(*dmli) , &(*frec));
                ptoc_Var21->integer.C_2.mb_data_len = ptoc_Var21->integer.C_2.mb_data_len + currentdif;
                currentdif = 0;
                t01int4(5 , "pos_in_parsb" , (int ) ptoc_Var19->d_pos_in_parsbuf);
                t01messblock(5 , "MBLOCK 506fi" , &ptoc_Var18->a_mblock);
                ptoc_Var19->d_pos_in_parsbuf = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var24->tak_fp_kind_type.C_fp_fill_part2_to_len.fp_len , -32768 , 32767 , "vak506.p" , 14477) , -32768 , 32767);
                t01messblock(5 , "MBLOCK 506fi" , &ptoc_Var18->a_mblock);
                t01int4(5 , "total_leng  " , (int ) ptoc_Var20->fr_total_leng);
                t01int4(5 , "pos_in_parsb" , (int ) ptoc_Var19->d_pos_in_parsbuf);
                break;
            case 8 :
                ak506one_field_copy(&(*acv) , &(*dmli) , &(*frec));
                break;
            case 9 :
                ptoc_Var20->fr_total_leng = (short ) sql__setrang(ptoc_Var24->tak_fp_kind_type.C_fp_pseudo_ins_select.fp_movebeforev11 , -32768 , 32767);
                ak506in_mess_buf_move(&(*acv) , &(*dmli) , &(*frec));
                ak506pseudo_insert_select(&(*acv) , &(*dmli));
                ptoc_Var25 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var19->d_sparr.px[sql__setrang((int ) ptoc_Var19->d_sparr.pcount , 1 , 8) + -1] , "vak506.p" , 14489)).integer.C_62.smessblock;
                ptoc_Var19->d_pos_in_parsbuf = (short ) sql__setrang((short ) (ptoc_Var25->mbr_mess_block.integer.C_2.mb_data_size + 1) , -32768 , 32767);
                break;
            case 10 :
                ptoc_Var20->fr_total_leng = (short ) sql__setrang(ptoc_Var24->tak_fp_kind_type.C_fp_last_lock.fp_movebefore_v4 , -32768 , 32767);
                ak506in_mess_buf_move(&(*acv) , &(*dmli) , &(*frec));
                (*(tgg00_DataPart *) sql__nil(ptoc_Var21->integer.C_3.mb_data , "vak506.p" , 14497)).integer.C_4.mbp_reclen = (short ) sql__setrang((short ) ptoc_Var21->integer.C_2.mb_data_len , -32768 , 32767);
                if ((int ) ptoc_Var21->integer.C_2.mb_type == 35 && (int ) (*(tgg00_DataPart *) sql__nil(ptoc_Var21->integer.C_3.mb_data , "vak506.p" , 14499)).integer.C_4.mbp_reclen > 8096)
                    {
                    a07_b_put_error(&(*acv) , 6450 , -8096);
                    }
                break;
            case 11 :
                a56syskey(&(*acv) , &(*dmli));
                break;
            case 12 :
                ptoc_Var20->fr_total_leng = (short ) sql__setrang(ptoc_Var24->tak_fp_kind_type.C_fp_pseudo_ins_select.fp_movebeforev11 , -32768 , 32767);
                if ((int ) ptoc_Var20->fr_total_leng == 0 && (int ) ptoc_Var19->d_pos_in_parsbuf == 1)
                    {
                    ptoc_Var20->fr_total_leng = 18;
                    ak506in_mess_buf_move(&(*acv) , &(*dmli) , &(*frec));
                    }
                else
                    {
                    ak506in_mess_buf_move(&(*acv) , &(*dmli) , &(*frec));
                    ptoc_Var19->d_pos_in_parsbuf = (short ) sql__setrang((short ) ((int ) ptoc_Var19->d_pos_in_parsbuf + 8) , -32768 , 32767);
                    }
                t01messblock(5 , "MBLOCK 506fi" , &ptoc_Var18->a_mblock);
                a56temp_table_key(&(*acv) , &(*dmli));
                if ((int ) ptoc_Var18->a_mblock.integer.C_2.mb_type == 36)
                    {
                    if (ptoc_Var21->integer.C_2.mb_data_len < 18)
                        {
                        ptoc_Var21->integer.C_2.mb_data_len = 18;
                        }
                    }
                break;
            case 13 :
                if ((int ) ptoc_Var18->a_mblock.integer.C_2.mb_type == 36)
                    {
                    ptoc_Var20->fr_total_leng = 2;
                    ak506in_mess_buf_move(&(*acv) , &(*dmli) , &(*frec));
                    ptoc_Var19->d_pos_in_parsbuf = (short ) sql__setrang((short ) ((int ) (short ) sql__succ((int ) ptoc_Var24->tak_fp_kind_type.C_fp_fill_part2_to_len.fp_len , -32768 , 32767 , "vak506.p" , 14539) + -2) , -32768 , 32767);
                    ptoc_Var21->integer.C_2.mb_data_len = (int ) ptoc_Var24->tak_fp_kind_type.C_fp_fill_part2_to_len.fp_len;
                    }
                else
                    {
                    if ((int ) ptoc_Var19->d_corr == 1)
                        {
                        ptoc_Var20->fr_total_leng = (short ) sql__setrang((short ) ((int ) ptoc_Var24->tak_fp_kind_type.C_fp_fill_part2_to_len.fp_len - ptoc_Var21->integer.C_2.mb_data_len - currentdif) , -32768 , 32767);
                        ak506in_mess_buf_move(&(*acv) , &(*dmli) , &(*frec));
                        }
                    else
                        {
                        ptoc_Var19->d_pos_in_parsbuf = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var24->tak_fp_kind_type.C_fp_fill_part2_to_len.fp_len , -32768 , 32767 , "vak506.p" , 14553) , -32768 , 32767);
                        ptoc_Var21->integer.C_2.mb_data_len = (int ) ptoc_Var24->tak_fp_kind_type.C_fp_fill_part2_to_len.fp_len;
                        }
                    }
                break;
            case 14 :
                ptoc_Var20->fr_total_leng = (short ) sql__setrang(ptoc_Var24->tak_fp_kind_type.C_fp_pseudo_ins_select.fp_movebeforev11 , -32768 , 32767);
                ak506in_mess_buf_move(&(*acv) , &(*dmli) , &(*frec));
                t01messblock(5 , "MBLOCK 506fi" , &ptoc_Var18->a_mblock);
                a05constraint_fulfilled(&(*acv) , &(*(tgg00_DataPart *) sql__nil(ptoc_Var18->a_mblock.integer.C_3.mb_data , "vak506.p" , 14564)).integer.C_3.mbp_rec , &(*(tgg00_QualBuf *) sql__nil(ptoc_Var18->a_mblock.integer.C_4.mb_qual , "vak506.p" , 14565)).integer.C_2.mtree , (int ) ptoc_Var24->tak_fp_kind_type.C_fp_pseudo_ins_select.fp_reccolno);
                break;
            case 16 :
                a05_verify_not_null(&(*acv) , &(*(tgg00_DataPart *) sql__nil(ptoc_Var18->a_mblock.integer.C_3.mb_data , "vak506.p" , 14570)).integer.C_3.mbp_rec , &ptoc_Var24->tak_fp_kind_type.C_fp_verify_not_null.fp_stackentry);
                break;
            case 15 :
                ptoc_Var20->fr_total_leng = (short ) sql__setrang(ptoc_Var24->tak_fp_kind_type.C_fp_pseudo_ins_select.fp_movebeforev11 , -32768 , 32767);
                ak506in_mess_buf_move(&(*acv) , &(*dmli) , &(*frec));
                t01messblock(5 , "MBLOCK 506fi" , &ptoc_Var18->a_mblock);
                mblock.integer.C_1.mb_header = ptoc_Var18->a_mblock.integer.C_1.mb_header;
                mblock.integer.C_1.mb_trailer = ptoc_Var18->a_mblock.integer.C_1.mb_trailer;
                mblock.integer.C_6.mb_work_st = ptoc_Var18->a_work_st_addr;
                mblock.integer.C_2.mb_work_st_max = (short ) sql__setrang(ptoc_Var18->a_work_st_max , -32768 , 32767);
                mblock.integer.C_7.mb_workbuf = ptoc_Var18->a_work_buf_addr;
                mblock.integer.C_2.mb_workbuf_size = ptoc_Var18->a_work_buf_size;
                a05qualification_test(&(*acv) , &mblock , 0 , 1 , 1 , &(*(tgg00_DataPart *) sql__nil(ptoc_Var18->a_mblock.integer.C_3.mb_data , "vak506.p" , 14586)).integer.C_3.mbp_rec , result , resultBufSize , &k);
                ptoc_Var18->a_mblock.integer.C_1.mb_header = mblock.integer.C_1.mb_header;
                if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var18->a_return_segm , "vak506.p" , 14588)).integer.C_4.sp1r_returncode != 0)
                    {
                    _ptoc_ASSIGN(unsigned char , 2, ptoc_Var18->a_transinf.tri_trans.trState_gg00, (*(tgg00_TransContext *) sql__nil(mblock.integer.C_9.mb_trns , "vak506.p" , 14591)).trState_gg00)
                    }
                else
                    {
                    (*(tgg00_QualBuf *) sql__nil(ptoc_Var18->a_mblock.integer.C_4.mb_qual , "vak506.p" , 14593)).integer.C_2.mview_cnt = 0;
                    }
                break;
            case 17 :
                ptoc_Var26 = &(*(struct tak_systembuffer *) sql__nil(viewkeybuf , "vak506.p" , 14597)).integer.C_47.sviewkey;
                ptoc_Var19->d_pos_in_parsbuf = (short ) sql__setrang((short ) ((int ) ptoc_Var19->d_pos_in_parsbuf + 8) , -32768 , 32767);
                ptoc_Var22->integer.C_4.mbp_keylen = (short ) sql__setrang(ptoc_Var26->vkjvkeylen , -32768 , 32767);
                ptoc_Var21->integer.C_2.mb_data_len = (int ) ptoc_Var22->integer.C_4.mbp_keylen + 8;
                currentdif = (int ) ptoc_Var24->tak_fp_kind_type.C_fp_current_of_key.fp_curr_keylen - (int ) ptoc_Var22->integer.C_4.mbp_keylen;
                g10mv5("VAK506" , 7 , sizeof(tsp00_Key ) , ptoc_Var21->integer.C_2.mb_data_size , ptoc_Var26->vkjvkey , 1 , ptoc_Var22->integer.C_1.mbp_buf , 9 , (int ) ptoc_Var22->integer.C_4.mbp_keylen , &(*(tsp1_segment *) sql__nil(ptoc_Var18->a_return_segm , "vak506.p" , 14607)).integer.C_4.sp1r_returncode);
                t01int4(5 , "currentdif  " , currentdif);
                break;
            case 7 :
                ptoc_Var20->fr_total_leng = (short ) sql__setrang((short ) ((int ) ptoc_Var24->tak_fp_kind_type.C_fp_get_name_value.fp_movebefore_v15 - ptoc_Var21->integer.C_2.mb_data_len) , -32768 , 32767);
                if ((int ) ptoc_Var20->fr_total_leng > 0)
                    {
                    ak506in_mess_buf_move(&(*acv) , &(*dmli) , &(*frec));
                    }
                if ((int ) (*(tsp00_MoveObj *) sql__nil(ptoc_Var18->a_data_ptr , "vak506.p" , 14619))[sql__setrang(ptoc_Var24->tak_fp_kind_type.C_fp_get_name_value.fp_frompos_v15 , 1 , 8192000) + -1] == 255)
                    {
                    a07_b_put_error(&(*acv) , 6336 , -ptoc_Var24->tak_fp_kind_type.C_fp_get_name_value.fp_frompos_v15);
                    }
                else
                    {
                    _ptoc_ASSIGN(unsigned char , 64, currentnam, a01_il_b_identifier)
                    if ((char ) sql__setrang(g01unicode , 0 , 1) && (int ) (*(tsp1_packet *) sql__nil((*acv).a_in_packet , "vak506.p" , 14628)).sp1_header.sp1h_mess_swap != 1 && (int ) (*(tsp1_packet *) sql__nil((*acv).a_in_packet , "vak506.p" , 14629)).sp1_header.sp1h_mess_code != 20)
                        {
                        univallen = (int ) ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_inoutlen_v1 + -1;
                        s80uni_trans((tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var18->a_data_ptr , "vak506.p" , 14634))[sql__setrang(ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + 1 , 1 , 8192000) + -1] , (int ) ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_inoutlen_v1 + -1 , 19 , (tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var18->a_data_ptr , "vak506.p" , 14636))[sql__setrang(ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + 1 , 1 , 8192000) + -1] , &univallen , 20 , *(tsp8_uni_opt_set *) (_ptoc_MOVE(unsigned char , 1, ptoc_Var27, ptoc_Var14) , ptoc_Var27) , &error , &err_char_no);
                        if ((int ) error != 0)
                            {
                            t01int4(5 , "err_char_no " , err_char_no);
                            a07_hex_uni_error(&(*acv) , (unsigned char ) sql__setrang(error , 0 , 8) , -(ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + err_char_no) , 1 , (tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var18->a_data_ptr , "vak506.p" , 14644))[sql__setrang(ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + err_char_no , 1 , 8192000) + -1] , (int ) ptoc_Var18->a_max_codewidth);
                            }
                        }
                    a641_get_name(&(*acv) , ptoc_Var18->a_data_part , sql__succ(ptoc_Var24->tak_fp_kind_type.C_fp_get_name_value.fp_frompos_v15 , -2147483647 , 2147483647 , "vak506.p" , 14651) , (short ) sql__setrang((short ) sql__pred((int ) ptoc_Var24->tak_fp_kind_type.C_fp_get_name_value.fp_inoutlen_v15 , -32768 , 32767 , "vak506.p" , 14651) , -32768 , 32767) , (char ) sql__setrang((int ) ptoc_Var24->tak_fp_kind_type.C_fp_get_name_value.fp_sc_symbol == 239 , 0 , 1) , (short ) sql__setrang((short ) sql__ord((int ) ptoc_Var24->tak_fp_kind_type.C_fp_get_name_value.fp_objecttype[0]) , -32768 , 32767) , currentnam);
                    if ((char ) sql__setrang(g01unicode , 0 , 1) && (int ) (*(tsp1_packet *) sql__nil((*acv).a_in_packet , "vak506.p" , 14657)).sp1_header.sp1h_mess_swap != 1 && (int ) (*(tsp1_packet *) sql__nil((*acv).a_in_packet , "vak506.p" , 14658)).sp1_header.sp1h_mess_code != 20)
                        {
                        univallen = (int ) ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_inoutlen_v1 + -1;
                        s80uni_trans((tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var18->a_data_ptr , "vak506.p" , 14663))[sql__setrang(ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + 1 , 1 , 8192000) + -1] , (int ) ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_inoutlen_v1 + -1 , 20 , (tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var18->a_data_ptr , "vak506.p" , 14665))[sql__setrang(ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + 1 , 1 , 8192000) + -1] , &univallen , 19 , *(tsp8_uni_opt_set *) (_ptoc_MOVE(unsigned char , 1, ptoc_Var27, ptoc_Var15) , ptoc_Var27) , &error , &err_char_no);
                        if ((int ) error != 0)
                            {
                            t01int4(5 , "err_char_no " , err_char_no);
                            a07_hex_uni_error(&(*acv) , (unsigned char ) sql__setrang(error , 0 , 8) , -(ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + err_char_no) , 1 , (tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var18->a_data_ptr , "vak506.p" , 14673))[sql__setrang(ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + err_char_no , 1 , 8192000) + -1] , (int ) ptoc_Var18->a_max_codewidth);
                            }
                        }
                    if (_ptoc_MEMCMP(unsigned char , 64, currentnam , a01_il_b_identifier) == 0 ? 1 : 0)
                        {
                        ptoc_Var22->integer.C_1.mbp_buf[sql__setrang(ptoc_Var21->integer.C_2.mb_data_len + 1 , 1 , 8192000) + -1] = (unsigned char)'\377';
                        g10fil("VAK506" , 8 , ptoc_Var21->integer.C_2.mb_data_size , ptoc_Var22->integer.C_1.mbp_buf , ptoc_Var21->integer.C_2.mb_data_len + 2 , (int ) ptoc_Var24->tak_fp_kind_type.C_fp_get_name_value.fp_inoutlen_v15 + -1 , (unsigned char)'\0' , &(*(tsp1_segment *) sql__nil(ptoc_Var18->a_return_segm , "vak506.p" , 14686)).integer.C_4.sp1r_returncode);
                        }
                    else
                        {
                        if ((int ) ptoc_Var24->fp_datatyp == 24)
                            {
                            ptoc_Var22->integer.C_1.mbp_buf[sql__setrang(ptoc_Var21->integer.C_2.mb_data_len + 1 , 1 , 8192000) + -1] = (unsigned char)'\001';
                            }
                        else
                            {
                            ptoc_Var22->integer.C_1.mbp_buf[sql__setrang(ptoc_Var21->integer.C_2.mb_data_len + 1 , 1 , 8192000) + -1] = (unsigned char)' ';
                            }
                        g10mv11("VAK506" , 9 , sizeof(unsigned char [64]) , ptoc_Var21->integer.C_2.mb_data_size , currentnam , 1 , ptoc_Var22->integer.C_1.mbp_buf , ptoc_Var21->integer.C_2.mb_data_len + 2 , sizeof(unsigned char [64]) , &(*(tsp1_segment *) sql__nil(ptoc_Var18->a_return_segm , "vak506.p" , 14699)).integer.C_4.sp1r_returncode);
                        if ((int ) ptoc_Var22->integer.C_1.mbp_buf[sql__setrang(ptoc_Var21->integer.C_2.mb_data_len + 1 , 1 , 8192000) + -1] == 1)
                            {
                            g10filuni("VAK506" , 10 , ptoc_Var21->integer.C_2.mb_data_size , ptoc_Var22->integer.C_1.mbp_buf , ptoc_Var21->integer.C_2.mb_data_len + sizeof(unsigned char [64]) + 2 , (int ) ptoc_Var24->tak_fp_kind_type.C_fp_get_name_value.fp_inoutlen_v15 - sizeof(unsigned char [64]) + -1 , "\0 " , &(*(tsp1_segment *) sql__nil(ptoc_Var18->a_return_segm , "vak506.p" , 14705)).integer.C_4.sp1r_returncode);
                            }
                        else
                            {
                            g10fil("VAK506" , 11 , ptoc_Var21->integer.C_2.mb_data_size , ptoc_Var22->integer.C_1.mbp_buf , ptoc_Var21->integer.C_2.mb_data_len + sizeof(unsigned char [64]) + 2 , (int ) ptoc_Var24->tak_fp_kind_type.C_fp_get_name_value.fp_inoutlen_v15 - sizeof(unsigned char [64]) + -1 , (unsigned char)' ' , &(*(tsp1_segment *) sql__nil(ptoc_Var18->a_return_segm , "vak506.p" , 14710)).integer.C_4.sp1r_returncode);
                            }
                        }
                    ptoc_Var21->integer.C_2.mb_data_len = ptoc_Var21->integer.C_2.mb_data_len + (int ) ptoc_Var24->tak_fp_kind_type.C_fp_get_name_value.fp_inoutlen_v15;
                    }
                break;
            default :
                if ((ptoc_Var28 = (int ) ptoc_Var24->fp_datatyp) == 6 || ptoc_Var28 == 7 || ptoc_Var28 == 8 || ptoc_Var28 == 9 || ptoc_Var28 == 34 || ptoc_Var28 == 19 || ptoc_Var28 == 20 || ptoc_Var28 == 35 || ptoc_Var28 == 21 || ptoc_Var28 == 22)
                    {
                    ak506long_support(&(*acv) , &(*dmli) , &(*frec) , ind);
                    }
                else
                    {
                    if ((int ) ptoc_Var24->fp_kind == 20 && (1 && (ptoc_Var24->fp_colset[0] & 64) != 0) && (0 || (ptoc_Var24->fp_colset[0] & 2) == 0))
                        {
                        if (ptoc_Var18->a_data_ptr == (tsp00_MoveObj *) (int *) 0)
                            {
                            a07_b_put_error(&(*acv) , 6488 , 1);
                            }
                        else
                            {
                            if ((int ) (*(tsp00_MoveObj *) sql__nil(ptoc_Var18->a_data_ptr , "vak506.p" , 14733))[sql__setrang(ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 , 1 , 8192000) + -1] == 255)
                                {
                                a07_b_put_error(&(*acv) , 6336 , -ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1);
                                }
                            else
                                {
                                if ((int ) ptoc_Var24->fp_datatyp == 3 || (int ) ptoc_Var24->fp_datatyp == 2)
                                    {
                                    g02fromtermchar((unsigned char ) sql__setrang(ptoc_Var18->a_mess_code , 0 , 255) , sql__nil(ptoc_Var18->a_data_ptr , "vak506.p" , 14744) , ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + 1 , 1);
                                    }
                                if ((int ) (*(tsp1_packet *) sql__nil(ptoc_Var18->a_in_packet , "vak506.p" , 14747)).sp1_header.sp1h_mess_code <= 10 && ((int ) ptoc_Var24->fp_datatyp == 3 || (int ) ptoc_Var24->fp_datatyp == 2) || (int ) ptoc_Var24->fp_datatyp == 4)
                                    {
                                    (*dmli).d_escape_char[1] = (unsigned char ) sql__setrang((*(tsp00_MoveObj *) sql__nil(ptoc_Var18->a_data_ptr , "vak506.p" , 14755))[sql__setrang(ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + 1 , 1 , 8192000) + -1] , 0 , 255);
                                    }
                                else
                                    {
                                    if ((int ) ptoc_Var24->fp_datatyp == 24)
                                        {
                                        if ((int ) (*(tsp1_packet *) sql__nil(ptoc_Var18->a_in_packet , "vak506.p" , 14760)).sp1_header.sp1h_mess_swap == 1)
                                            {
                                            (*dmli).d_escape_char[0] = (unsigned char ) sql__setrang((*(tsp00_MoveObj *) sql__nil(ptoc_Var18->a_data_ptr , "vak506.p" , 14763))[sql__setrang(ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + 1 , 1 , 8192000) + -1] , 0 , 255);
                                            (*dmli).d_escape_char[1] = (unsigned char ) sql__setrang((*(tsp00_MoveObj *) sql__nil(ptoc_Var18->a_data_ptr , "vak506.p" , 14764))[sql__setrang(ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + 2 , 1 , 8192000) + -1] , 0 , 255);
                                            }
                                        else
                                            {
                                            (*dmli).d_escape_char[0] = (unsigned char ) sql__setrang((*(tsp00_MoveObj *) sql__nil(ptoc_Var18->a_data_ptr , "vak506.p" , 14768))[sql__setrang(ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + 2 , 1 , 8192000) + -1] , 0 , 255);
                                            (*dmli).d_escape_char[1] = (unsigned char ) sql__setrang((*(tsp00_MoveObj *) sql__nil(ptoc_Var18->a_data_ptr , "vak506.p" , 14769))[sql__setrang(ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + 1 , 1 , 8192000) + -1] , 0 , 255);
                                            }
                                        }
                                    else
                                        {
                                        if ((ptoc_Var28 = (int ) (*(tsp1_packet *) sql__nil(ptoc_Var18->a_in_packet , "vak506.p" , 14774)).sp1_header.sp1h_mess_code) == 20 || ptoc_Var28 == 19)
                                            {
                                            uniinlen = (int ) ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_inoutlen_v1 + -1;
                                            }
                                        else
                                            {
                                            uniinlen = s30lnr(sql__nil(ptoc_Var18->a_data_ptr , "vak506.p" , 14779) , (unsigned char)' ' , ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + 1 , (int ) ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_inoutlen_v1 + -1);
                                            }
                                        univallen = 2;
                                        s80uni_trans((tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var18->a_data_ptr , "vak506.p" , 14783))[sql__setrang(ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + 1 , 1 , 8192000) + -1] , uniinlen , (short ) sql__setrang((short ) (*(tsp1_packet *) sql__nil(ptoc_Var18->a_in_packet , "vak506.p" , 14784)).sp1_header.sp1h_mess_code , -32768 , 32767) , (tsp00_MoveObj *) escape_c2 , &univallen , 20 , *(tsp8_uni_opt_set *) (_ptoc_MOVE(unsigned char , 1, ptoc_Var27, ptoc_Var16) , ptoc_Var27) , &error , &err_char_no);
                                        if ((int ) error == 0)
                                            {
                                            uniinlen = 1;
                                            s80uni_trans((tsp00_MoveObj *) escape_c2 , univallen , 20 , (tsp00_MoveObj *) &(*dmli).d_escape_char[1] , &uniinlen , 0 , *(tsp8_uni_opt_set *) (_ptoc_MOVE(unsigned char , 1, ptoc_Var27, ptoc_Var17) , ptoc_Var27) , &error , &err_char_no);
                                            }
                                        if ((int ) error != 0)
                                            {
                                            t01int4(5 , "err_char_no " , err_char_no);
                                            a07_hex_uni_error(&(*acv) , (unsigned char ) sql__setrang(error , 0 , 8) , -(ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + err_char_no) , 1 , (tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var18->a_data_ptr , "vak506.p" , 14802))[sql__setrang(ptoc_Var24->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + err_char_no , 1 , 8192000) + -1] , (int ) ptoc_Var18->a_max_codewidth);
                                            }
                                        }
                                    }
                                if ((int ) ptoc_Var18->a_basic_code != 1 && (int ) ptoc_Var24->fp_datatyp == 3)
                                    {
                                    g02p1ascii_pos_ebcdic((*dmli).d_escape_char , 2 , (*dmli).d_escape_char , 2 , 1);
                                    }
                                else
                                    {
                                    if ((int ) ptoc_Var18->a_basic_code == 1 && (int ) ptoc_Var24->fp_datatyp == 2)
                                        {
                                        g02p1ebcdic_pos_ascii((*dmli).d_escape_char , 2 , (*dmli).d_escape_char , 2 , 1);
                                        }
                                    }
                                }
                            }
                        }
                    else
                        {
                        ak506normal_value(&(*acv) , &(*dmli) , &(*frec) , result , ind , &currentdif);
                        }
                    }
                break;
            }
        if ((char ) sql__setrang((*acv).a_stored_proc_call , 0 , 1) && (*acv).a_mblock.integer.C_2.mb_data_len > data_len)
            {
            data_len = (*acv).a_mblock.integer.C_2.mb_data_len;
            }
        (*frec).fr_f_no = (short ) sql__setrang((short ) sql__succ((int ) (*frec).fr_f_no , -32768 , 32767 , "vak506.p" , 14840) , -32768 , 32767);
        }
    if ((char ) sql__setrang((*acv).a_stored_proc_call , 0 , 1))
        {
        (*acv).a_mblock.integer.C_2.mb_data_len = data_len;
        }
    t01messblock(5 , "MBLOCK 506fi" , &(*acv).a_mblock);
    cccprint("<a506fieldvalues");
    }



int a506lock_fieldvalues(acv, dmli, frec, viewkeybuf)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_fill_rec *frec;
tak_sysbufferaddress viewkeybuf;
    {
    char last_value;
    int i;
    tsp_knl_identifier currentnam;
    tgg_surrogate nil_surrogate;
    tak_dml_info *ptoc_Var29;
    tak_fill_rec *ptoc_Var30;
    tak_all_command_glob *ptoc_Var31;
    struct tgg00_MessBlock *ptoc_Var32;
    tgg00_QualBuf *ptoc_Var33;
    tak_parsinforecord *ptoc_Var34;
    tak_field_pars *ptoc_Var35;
    tgg00_Lock *ptoc_Var36;
    tgg00_Lock *ptoc_Var37;
    cccprint("AK506 a506lock_f");
    ptoc_Var29 = &*dmli;
    ptoc_Var30 = &*frec;
    ptoc_Var31 = &*acv;
    ptoc_Var32 = &ptoc_Var31->a_mblock;
    ptoc_Var33 = (tgg00_QualBuf *) sql__nil(ptoc_Var32->integer.C_4.mb_qual , "vak506.p" , 14866);
    _ptoc_ASSIGN(unsigned char , 8, nil_surrogate, "\0\0\0\0\0\0\0\0")
    ptoc_Var30->fr_f_no = 1;
    i = 1;
    ptoc_Var31->a_mblock.integer.C_2.mb_data_len = 8;
    ptoc_Var29->d_pos_in_parsbuf = (short ) sql__setrang((short ) ((int ) ptoc_Var29->d_pos_in_parsbuf + 8) , -32768 , 32767);
    t01int4(5 , "lock_count  " , (int ) ptoc_Var33->integer.C_7.mlock_cnt);
    t01int4(5 , "p_cnt_infos " , (int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var29->d_sparr.px[0] , "vak506.p" , 14875)).integer.C_23.sparsinfo.integer.C_2.p_cnt_infos);
    while (i <= (int ) ptoc_Var33->integer.C_7.mlock_cnt && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var31->a_return_segm , "vak506.p" , 14876)).integer.C_4.sp1r_returncode == 0)
        {
        t01messblock(5 , "MBLOCK 506lo" , &ptoc_Var31->a_mblock);
        if ((int ) ptoc_Var33->integer.C_7.mlock[sql__setrang(i , 1 , 240) + -1].lockMode_gg00 != 3 && (int ) ptoc_Var33->integer.C_7.mlock[sql__setrang(i , 1 , 240) + -1].lockMode_gg00 != 4)
            {
            ptoc_Var33->integer.C_7.mlock[sql__setrang(i , 1 , 240) + -1].lockKeyPos_gg00 = (short ) sql__setrang((short ) (ptoc_Var31->a_mblock.integer.C_2.mb_data_len + -7) , -32768 , 32767);
            if ((int ) ptoc_Var30->fr_f_no <= (int ) (*(struct tak_systembuffer *) sql__nil(ptoc_Var29->d_sparr.px[0] , "vak506.p" , 14884)).integer.C_23.sparsinfo.integer.C_2.p_cnt_infos)
                {
                last_value = 0;
                while (!(char ) sql__setrang(last_value , 0 , 1) && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var31->a_return_segm , "vak506.p" , 14888)).integer.C_4.sp1r_returncode == 0)
                    {
                    ptoc_Var34 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var29->d_sparr.px[0] , "vak506.p" , 14890)).integer.C_23.sparsinfo;
                    ptoc_Var35 = &ptoc_Var34->integer.C_2.p_pars_infos[sql__setrang((int ) ptoc_Var30->fr_f_no , 1 , 2025) + -1];
                    if ((int ) ptoc_Var35->fp_kind == 22)
                        {
                        if ((int ) ptoc_Var35->tak_fp_kind_type.C_fp_last_lock.fp_movebefore_v4 > 0)
                            {
                            ptoc_Var30->fr_total_leng = (short ) sql__setrang(ptoc_Var35->tak_fp_kind_type.C_fp_last_lock.fp_movebefore_v4 , -32768 , 32767);
                            ak506in_mess_buf_move(&(*acv) , &(*dmli) , &(*frec));
                            }
                        last_value = 1;
                        ptoc_Var36 = &ptoc_Var33->integer.C_7.mlock[sql__setrang(i , 1 , 240) + -1];
                        ptoc_Var36->lockKeyLen_gg00 = (short ) sql__setrang((short ) (ptoc_Var31->a_mblock.integer.C_2.mb_data_len - (int ) ptoc_Var36->lockKeyPos_gg00 + -7) , -32768 , 32767);
                        }
                    else
                        {
                        if ((int ) ptoc_Var35->fp_kind == 21)
                            {
                            ptoc_Var30->fr_last_fno = (short ) sql__setrang(ptoc_Var30->fr_f_no , -32768 , 32767);
                            a506fieldvalues(&(*acv) , &(*dmli) , &(*frec) , viewkeybuf , (*(tgg00_DataPart *) sql__nil(ptoc_Var31->a_mblock.integer.C_3.mb_data , "vak506.p" , 14916)).integer.C_1.mbp_buf , ptoc_Var31->a_mblock.integer.C_2.mb_data_size);
                            }
                        else
                            {
                            ptoc_Var37 = &ptoc_Var33->integer.C_7.mlock[sql__setrang(i , 1 , 240) + -1];
                            ptoc_Var30->fr_f_no = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var30->fr_f_no , -32768 , 32767 , "vak506.p" , 14921) , -32768 , 32767);
                            ptoc_Var30->fr_total_leng = (short ) sql__setrang((short ) ((int ) ptoc_Var35->tak_fp_kind_type.C_fp_current_of_key.fp_curr_keylen + (int ) ptoc_Var35->tak_fp_kind_type.C_fp_current_of_key.fp_movebefore_v5) , -32768 , 32767);
                            ak506in_mess_buf_move(&(*acv) , &(*dmli) , &(*frec));
                            t01int4(5 , "total_leng  " , (int ) ptoc_Var30->fr_total_leng);
                            t01int4(5 , "pos_in_parsb" , (int ) ptoc_Var29->d_pos_in_parsbuf);
                            t01messblock(5 , "MBLOCK 506lo" , &ptoc_Var31->a_mblock);
                            ptoc_Var32->integer.C_2.mb_data_len = ptoc_Var32->integer.C_2.mb_data_len - (int ) ptoc_Var35->tak_fp_kind_type.C_fp_current_of_key.fp_curr_keylen;
                            g10mv3("VAK506" , 12 , ptoc_Var32->integer.C_2.mb_data_size , sizeof(unsigned char [64]) , (*(tgg00_DataPart *) sql__nil(ptoc_Var32->integer.C_3.mb_data , "vak506.p" , 14933)).integer.C_1.mbp_buf , ptoc_Var32->integer.C_2.mb_data_len + 1 , currentnam , 1 , sizeof(unsigned char [64]) , &(*(tsp1_segment *) sql__nil(ptoc_Var31->a_return_segm , "vak506.p" , 14936)).integer.C_4.sp1r_returncode);
                            a58_get_currentkey(&(*acv) , currentnam , nil_surrogate , 1);
                            }
                        }
                    }
                ptoc_Var30->fr_f_no = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var30->fr_f_no , -32768 , 32767 , "vak506.p" , 14946) , -32768 , 32767);
                t01messblock(5 , "MBLOCK 506lo" , &ptoc_Var31->a_mblock);
                }
            }
        i = sql__succ(i , -2147483647 , 2147483647 , "vak506.p" , 14952);
        }
    cccprint("<a506lock_fieldv");
    }

static
  tsp8_uni_opt_set ptoc_Var38 =
  {
  (unsigned char)'\0'
  };
static
  tsp8_uni_opt_set ptoc_Var39 =
  {
  (unsigned char)'\0'
  };


int a506result_name(acv, dmli)
tak_all_command_glob *acv;
tak_dml_info *dmli;
    {
    int movelen;
    tsp_int4 err_char_no;
    tsp8_uni_error uni_err;
    tak_all_command_glob *ptoc_Var40;
    tak_parsinforecord *ptoc_Var41;
    tak_field_pars *ptoc_Var42;
    tsp8_uni_opt_set ptoc_Var43;
    cccprint("AK506 a506result");
    ptoc_Var40 = &*acv;
    ptoc_Var41 = &(*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak506.p" , 14974)).integer.C_23.sparsinfo;
    if ((int ) ptoc_Var41->integer.C_2.p_cnt_infos > 0)
        {
        ptoc_Var42 = &ptoc_Var41->integer.C_2.p_pars_infos[0];
        if ((int ) ptoc_Var42->fp_kind == 2)
            {
            if ((char ) sql__setrang(g01unicode , 0 , 1))
                {
                movelen = sizeof(unsigned char [64]);
                if ((int ) (*(tsp1_packet *) sql__nil((*acv).a_in_packet , "vak506.p" , 14985)).sp1_header.sp1h_mess_swap != 1 && (int ) (*(tsp1_packet *) sql__nil((*acv).a_in_packet , "vak506.p" , 14986)).sp1_header.sp1h_mess_code != 20)
                    {
                    s80uni_trans((tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var40->a_data_ptr , "vak506.p" , 14988))[sql__setrang(ptoc_Var42->tak_fp_kind_type.C_fp_result_name.fp_fromposv14 + 1 , 1 , 8192000) + -1] , (int ) ptoc_Var42->tak_fp_kind_type.C_fp_result_name.fp_dataiolenv14 + -1 , 19 , (tsp00_MoveObj *) (*dmli).d_resname , &movelen , 20 , *(tsp8_uni_opt_set *) (_ptoc_MOVE(unsigned char , 1, ptoc_Var43, ptoc_Var38) , ptoc_Var43) , &uni_err , &err_char_no);
                    }
                else
                    {
                    s80uni_trans((tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var40->a_data_ptr , "vak506.p" , 14993))[sql__setrang(ptoc_Var42->tak_fp_kind_type.C_fp_result_name.fp_fromposv14 + 1 , 1 , 8192000) + -1] , (int ) ptoc_Var42->tak_fp_kind_type.C_fp_result_name.fp_dataiolenv14 + -1 , 20 , (tsp00_MoveObj *) (*dmli).d_resname , &movelen , 20 , *(tsp8_uni_opt_set *) (_ptoc_MOVE(unsigned char , 1, ptoc_Var43, ptoc_Var39) , ptoc_Var43) , &uni_err , &err_char_no);
                    }
                if ((int ) uni_err != 0 && (int ) uni_err != 3)
                    {
                    a07_b_put_error(&(*acv) , 6040 , 1);
                    }
                }
            else
                {
                g02fromtermchar((unsigned char ) sql__setrang(ptoc_Var40->a_mess_code , 0 , 255) , sql__nil(ptoc_Var40->a_data_ptr , "vak506.p" , 15006) , ptoc_Var42->tak_fp_kind_type.C_fp_result_name.fp_fromposv14 + 1 , (int ) ptoc_Var42->tak_fp_kind_type.C_fp_result_name.fp_dataiolenv14 + -1);
                g10mv3("VAK506" , 13 , ptoc_Var40->a_data_length , sizeof(unsigned char [64]) , sql__nil(ptoc_Var40->a_data_ptr , "vak506.p" , 15010) , ptoc_Var42->tak_fp_kind_type.C_fp_result_name.fp_fromposv14 + 1 , (*dmli).d_resname , 1 , (int ) ptoc_Var42->tak_fp_kind_type.C_fp_result_name.fp_dataiolenv14 + -1 , &(*(tsp1_segment *) sql__nil(ptoc_Var40->a_return_segm , "vak506.p" , 15012)).integer.C_4.sp1r_returncode);
                }
            if (_ptoc_MEMCMP(unsigned char , 64, (*dmli).d_resname , a01_zero_res_name) == 0 ? 1 : 0)
                {
                a07_b_put_error(&(*acv) , 6304 , -ptoc_Var42->tak_fp_kind_type.C_fp_result_name.fp_fromposv14);
                }
            else
                {
                (*dmli).d_resname_found = 1;
                }
            if (ptoc_Var40->a_input_data_pos < ptoc_Var42->tak_fp_kind_type.C_fp_result_name.fp_fromposv14 + (int ) ptoc_Var42->tak_fp_kind_type.C_fp_result_name.fp_datalenv14)
                {
                ptoc_Var40->a_input_data_pos = ptoc_Var42->tak_fp_kind_type.C_fp_result_name.fp_fromposv14 + (int ) ptoc_Var42->tak_fp_kind_type.C_fp_result_name.fp_datalenv14;
                }
            }
        }
    cccprint("<a506result_name");
    }



int ak506catalog_val_from_datapart(acv, etype, valpos, inoutlen)
tak_all_command_glob *acv;
tgg_stack_entry_type etype;
int valpos;
int inoutlen;
    {
    tgg_basis_error b_err;
    int actlen;
    int ix;
    tsp_int_map_c2 i2c2;
    tsp_c2 segm_id;
    tak_sysbufferaddress sysbuf;
    tgg_sysinfokey sysk;
    tak_all_command_glob *ptoc_Var44;
    struct tgg00_MessBlock *ptoc_Var45;
    tgg00_DataPart *ptoc_Var46;
    int ptoc_Var47;
    struct tak_systembuffer *ptoc_Var48;
    cccprint("AK506 ak506catal");
    ptoc_Var44 = &*acv;
    ptoc_Var45 = &ptoc_Var44->a_mblock;
    ptoc_Var46 = (tgg00_DataPart *) sql__nil(ptoc_Var45->integer.C_3.mb_data , "vak506.p" , 15051);
    t01int4(5 , "data_length " , ptoc_Var44->a_data_length);
    t01int4(5 , "valpos      " , valpos);
    t01int4(5 , "inoutlen    " , inoutlen);
    if (ptoc_Var45->integer.C_2.mb_data_len == 0)
        {
        ptoc_Var45->integer.C_2.mb_data_len = 8;
        }
    if (ptoc_Var44->a_data_length + 1 < valpos + inoutlen)
        {
        a07_b_put_error(&(*acv) , 6488 , -valpos);
        }
    else
        {
        if ((int ) (*(tsp00_MoveObj *) sql__nil(ptoc_Var44->a_data_ptr , "vak506.p" , 15064))[sql__setrang(valpos , 1 , 8192000) + -1] == 255)
            {
            a07_b_put_error(&(*acv) , 6336 , -valpos);
            }
        else
            {
            if ((int ) etype == 1)
                {
                actlen = s30lnr(sql__nil(ptoc_Var44->a_data_ptr , "vak506.p" , 15072) , (unsigned char)'\0' , valpos + 1 , inoutlen + -1);
                }
            else
                {
                i2c2.boolean.C_false.map_c2[0] = (unsigned char ) sql__setrang((*(tsp00_MoveObj *) sql__nil(ptoc_Var44->a_data_ptr , "vak506.p" , 15076))[sql__setrang(valpos + 1 , 1 , 8192000) + -1] , 0 , 255);
                i2c2.boolean.C_false.map_c2[1] = (unsigned char ) sql__setrang((*(tsp00_MoveObj *) sql__nil(ptoc_Var44->a_data_ptr , "vak506.p" , 15077))[sql__setrang(valpos + 2 , 1 , 8192000) + -1] , 0 , 255);
                actlen = (int ) i2c2.boolean.C_true.map_int;
                valpos = valpos + 2;
                }
            t01int4(5 , "actlen      " , actlen);
            g10mv("VAK506" , 14 , sizeof(tsp00_MoveObj ) , ptoc_Var45->integer.C_2.mb_data_size , sql__nil(ptoc_Var44->a_data_ptr , "vak506.p" , 15085) , valpos + 1 , ptoc_Var46->integer.C_1.mbp_buf , ptoc_Var45->integer.C_2.mb_data_len + 1 , actlen , &(*(tsp1_segment *) sql__nil(ptoc_Var44->a_return_segm , "vak506.p" , 15087)).integer.C_4.sp1r_returncode);
            ptoc_Var45->integer.C_2.mb_data_len = ptoc_Var45->integer.C_2.mb_data_len + actlen;
            if ((int ) etype == 1)
                {
                ptoc_Var46->integer.C_4.mbp_keylen = (short ) sql__setrang((short ) (ptoc_Var45->integer.C_2.mb_data_len + -8) , -32768 , 32767);
                if ((int ) ptoc_Var45->integer.C_2.mb_type == 61 && (int ) ptoc_Var45->integer.C_2.mb_type2 != 16)
                    {
                    (*(tgg00_StackList *) sql__nil(ptoc_Var45->integer.C_5.mb_st , "vak506.p" , 15096))[sql__setrang((int ) (*(tgg00_QualBuf *) sql__nil(ptoc_Var45->integer.C_4.mb_qual , "vak506.p" , 15096)).integer.C_2.mstrat_pos , 1 , 32767) + -1].integer.C_1.elen_var = (short ) sql__setrang(ptoc_Var46->integer.C_4.mbp_keylen , -32768 , 32767);
                    }
                else
                    {
                    if ((int ) ptoc_Var45->integer.C_2.mb_type == 18)
                        {
                        sysk.skeylen = (short ) sql__setrang(ptoc_Var46->integer.C_4.mbp_keylen , -32768 , 32767);
                        ptoc_Var47 = actlen;
                        if (1 <= ptoc_Var47)
                            {
                            ix = 1;
                            do
                                {
                                sysk.integer.C_8.all[sql__setrang(ix , 1 , 140) + -1] = (unsigned char ) sql__setrang(ptoc_Var46->integer.C_1.mbp_buf[sql__setrang(ix + 8 , 1 , 8192000) + -1] , 0 , 255);
                                if (ix == ptoc_Var47)
                                    {
                                    break;
                                    }
                                ix += 1;
                                }
                            while (!0);
                            }
                        a10get_sysinfo(&(*acv) , &sysk , 1 , &sysbuf , &b_err);
                        if ((int ) b_err == 0)
                            {
                            ptoc_Var48 = (struct tak_systembuffer *) sql__nil(sysbuf , "vak506.p" , 15108);
                            segm_id[0] = (unsigned char ) sql__setrang(ptoc_Var48->integer.C_51.ssysbuf[sql__setrang((int ) ptoc_Var48->integer.C_0.b_kl + 9 , 1 , 32760) + -1] , 0 , 255);
                            segm_id[1] = (unsigned char ) sql__setrang(ptoc_Var48->integer.C_51.ssysbuf[sql__setrang((int ) ptoc_Var48->integer.C_0.b_kl + 10 , 1 , 32760) + -1] , 0 , 255);
                            if (_ptoc_MEMCMP(unsigned char , 2, segm_id , "\0\001") == 0 ? 1 : 0)
                                {
                                ptoc_Var45->integer.C_2.mb_replicated = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    cccprint("<ak506catalog_va");
    }



int ak506datetime(acv, fp, frec, mbufiolen, to_fill, start)
tak_all_command_glob *acv;
tak_field_pars *fp;
tak_fill_rec *frec;
int mbufiolen;
char to_fill;
int start;
    {
    int i;
    tgg_basis_error b_err;
    tsp_c9 curr_time;
    tsp00_Longreal utcdiff;
    tsp00_Timestamp timestamp;
    tsp_num_error num_err;
    tak_field_pars *ptoc_Var49;
    struct tgg00_MessBlock *ptoc_Var50;
    tgg00_DataPart *ptoc_Var51;
    int ptoc_Var52;
    tgg00_TransContext *ptoc_Var53;
    cccprint("AK506 ak506datet");
    ptoc_Var49 = &*fp;
    ptoc_Var50 = &(*acv).a_mblock;
    ptoc_Var51 = (tgg00_DataPart *) sql__nil(ptoc_Var50->integer.C_3.mb_data , "vak506.p" , 15154);
    if ((int ) ptoc_Var49->fp_datatyp == 3)
        {
        ptoc_Var51->integer.C_1.mbp_buf[sql__setrang(start , 1 , 8192000) + -1] = (unsigned char)'@';
        }
    else
        {
        if ((int ) ptoc_Var49->fp_datatyp == 2 || (int ) ptoc_Var49->fp_datatyp == 10 || (int ) ptoc_Var49->fp_datatyp == 11 || (int ) ptoc_Var49->fp_datatyp == 13)
            {
            ptoc_Var51->integer.C_1.mbp_buf[sql__setrang(start , 1 , 8192000) + -1] = (unsigned char)' ';
            }
        else
            {
            if ((int ) ptoc_Var49->fp_datatyp == 24)
                {
                ptoc_Var51->integer.C_1.mbp_buf[sql__setrang(start , 1 , 8192000) + -1] = (unsigned char)'\001';
                }
            else
                {
                ptoc_Var51->integer.C_1.mbp_buf[sql__setrang(start , 1 , 8192000) + -1] = (unsigned char)'\0';
                }
            }
        }
    if (ptoc_Var49->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 == -1 || ptoc_Var49->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 == -2 || ptoc_Var49->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 == -3 || ptoc_Var49->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 == -4)
        {
        if (ptoc_Var49->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 == -4)
            {
            vUTCtimestamp(timestamp);
            }
        else
            {
            if (_ptoc_MEMCMP(unsigned char , 20, (*acv).a_timestamp , "                    ") == 0 ? 1 : 0)
                {
                vtimestamp((*acv).a_timestamp);
                }
            _ptoc_ASSIGN(unsigned char , 20, timestamp, (*acv).a_timestamp)
            }
        if ((int ) ptoc_Var49->fp_datatyp == 10 || (int ) ptoc_Var49->fp_datatyp == 11 || (int ) ptoc_Var49->fp_datatyp == 13 || (int ) (*acv).a_dt_format == 1)
            {
            (*frec).fr_leng = 9;
            if (ptoc_Var49->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 == -1)
                {
                g10mv2("VAK506" , 15 , sizeof(tsp00_C20 ) , ptoc_Var50->integer.C_2.mb_data_size , timestamp , 1 , ptoc_Var51->integer.C_1.mbp_buf , start + 1 , 8 , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak506.p" , 15206)).integer.C_4.sp1r_returncode);
                }
            else
                {
                if (ptoc_Var49->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 == -2)
                    {
                    ptoc_Var51->integer.C_1.mbp_buf[sql__setrang(start + 1 , 1 , 8192000) + -1] = (unsigned char)'0';
                    ptoc_Var51->integer.C_1.mbp_buf[sql__setrang(start + 2 , 1 , 8192000) + -1] = (unsigned char)'0';
                    g10mv2("VAK506" , 16 , sizeof(tsp00_C20 ) , ptoc_Var50->integer.C_2.mb_data_size , timestamp , 9 , ptoc_Var51->integer.C_1.mbp_buf , start + 3 , 6 , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak506.p" , 15217)).integer.C_4.sp1r_returncode);
                    }
                else
                    {
                    g10mv2("VAK506" , 17 , sizeof(tsp00_C20 ) , ptoc_Var50->integer.C_2.mb_data_size , timestamp , 1 , ptoc_Var51->integer.C_1.mbp_buf , start + 1 , sizeof(tsp00_C20 ) , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak506.p" , 15225)).integer.C_4.sp1r_returncode);
                    (*frec).fr_leng = 21;
                    }
                }
            }
        else
            {
            switch (ptoc_Var49->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1)
                {
                case -1 :
                    b_err = 0;
                    g03dfchange_format_date(timestamp , ptoc_Var51->integer.C_1.mbp_buf , 1 , start + 1 , (unsigned char ) sql__setrang((*acv).a_dt_format , 0 , 9) , &b_err);
                    if ((int ) b_err != 0)
                        {
                        a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                        }
                    else
                        {
                        (*frec).fr_leng = 11;
                        }
                    break;
                case -2 :
                    b_err = 0;
                    curr_time[1] = (unsigned char)'0';
                    curr_time[2] = (unsigned char)'0';
                    s10mv1(sizeof(tsp00_C20 ) , sizeof(unsigned char [9]) , timestamp , 9 , curr_time , 4 , 6);
                    g03tfchange_format_time(curr_time , ptoc_Var51->integer.C_1.mbp_buf , 2 , start + 1 , (unsigned char ) sql__setrang((*acv).a_dt_format , 0 , 9) , &b_err);
                    if ((int ) b_err != 0)
                        {
                        a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                        }
                    else
                        {
                        (*frec).fr_leng = 9;
                        }
                    break;
                case -3 :
                case -4 :
                    b_err = 0;
                    g03tsfchange_format_timestamp(timestamp , ptoc_Var51->integer.C_1.mbp_buf , 1 , start + 1 , (unsigned char ) sql__setrang((*acv).a_dt_format , 0 , 9) , (*acv).a_ak_language , &b_err);
                    if ((int ) b_err != 0)
                        {
                        a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , 1);
                        }
                    else
                        {
                        (*frec).fr_leng = 27;
                        }
                    break;
                default :
                    sql__caseerr("vak506.p" , 15232);
                    break;
                }
            }
        if ((int ) g01code.ctype == 1 && ((int ) ptoc_Var49->fp_datatyp == 2 || (int ) ptoc_Var49->fp_datatyp == 10 || (int ) ptoc_Var49->fp_datatyp == 11 || (int ) ptoc_Var49->fp_datatyp == 13))
            {
            g02pebcdic_pos_ascii(ptoc_Var51->integer.C_1.mbp_buf , start + 1 , ptoc_Var51->integer.C_1.mbp_buf , start + 1 , (int ) (*frec).fr_leng);
            }
        else
            {
            if ((int ) g01code.ctype == 0 && (int ) ptoc_Var49->fp_datatyp == 3)
                {
                g02pascii_pos_ebcdic(ptoc_Var51->integer.C_1.mbp_buf , start + 1 , ptoc_Var51->integer.C_1.mbp_buf , start + 1 , (int ) (*frec).fr_leng);
                }
            else
                {
                if ((int ) ptoc_Var49->fp_datatyp == 24)
                    {
                    ptoc_Var52 = (int ) (*frec).fr_leng + -1;
                    if (ptoc_Var52 >= 1)
                        {
                        i = ptoc_Var52;
                        do
                            {
                            ptoc_Var51->integer.C_1.mbp_buf[sql__setrang(start + i * 2 , 1 , 8192000) + -1] = (unsigned char ) sql__setrang(ptoc_Var51->integer.C_1.mbp_buf[sql__setrang(start + i , 1 , 8192000) + -1] , 0 , 255);
                            ptoc_Var51->integer.C_1.mbp_buf[sql__setrang(start + i * 2 + -1 , 1 , 8192000) + -1] = (unsigned char)'\0';
                            if (i == 1)
                                {
                                break;
                                }
                            i -= 1;
                            }
                        while (!0);
                        }
                    (*frec).fr_leng = (short ) sql__setrang((short ) ((int ) (*frec).fr_leng * 2 + -1) , -32768 , 32767);
                    }
                }
            }
        }
    else
        {
        if (ptoc_Var49->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 == -7)
            {
            a05_stamp_get(&(*acv) , ptoc_Var51->integer.C_1.mbp_buf , start + 1 , ptoc_Var50->integer.C_2.mb_data_size);
            (*frec).fr_leng = 9;
            }
        else
            {
            if (ptoc_Var49->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 == -8)
                {
                a05_transaction_get(&(*acv) , ptoc_Var51->integer.C_1.mbp_buf , start + 1 , ptoc_Var50->integer.C_2.mb_data_size);
                ptoc_Var53 = &(*acv).a_transinf.tri_trans;
                (*frec).fr_leng = (short ) sql__setrang((short ) (sizeof(struct ptoc_Type13 ) + 1) , -32768 , 32767);
                }
            else
                {
                if (ptoc_Var49->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 == -5)
                    {
                    utcdiff = vTimeZoneDelta();
                    s41plrel(ptoc_Var51->integer.C_1.mbp_buf , start + 1 , 4 , 2 , utcdiff , &num_err);
                    if ((int ) num_err != 0)
                        {
                        switch ((int ) num_err)
                            {
                            case 3 :
                                a07_b_put_error(&(*acv) , 4320 , 1);
                                break;
                            case 1 :
                                a07_b_put_error(&(*acv) , 6372 , 1);
                                break;
                            case 2 :
                                a07_b_put_error(&(*acv) , 4330 , 1);
                                break;
                            default :
                                (unsigned char ) sql__caseerr("vak506.p" , 15332);
                                break;
                            }
                        }
                    else
                        {
                        (*frec).fr_leng = 4;
                        }
                    }
                else
                    {
                    if (ptoc_Var49->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 == -6)
                        {
                        utcdiff = vTimeZoneDelta();
                        utcdiff = (double ) (sql__trunc(utcdiff) * 10000) + (utcdiff - (double ) sql__trunc(utcdiff)) * (double ) 6.00000000000000000000e+003;
                        s41plrel(ptoc_Var51->integer.C_1.mbp_buf , start + 1 , 6 , 0 , utcdiff , &num_err);
                        if ((int ) num_err != 0)
                            {
                            switch ((int ) num_err)
                                {
                                case 3 :
                                    a07_b_put_error(&(*acv) , 4320 , 1);
                                    break;
                                case 1 :
                                    a07_b_put_error(&(*acv) , 6372 , 1);
                                    break;
                                case 2 :
                                    a07_b_put_error(&(*acv) , 4330 , 1);
                                    break;
                                default :
                                    (unsigned char ) sql__caseerr("vak506.p" , 15360);
                                    break;
                                }
                            }
                        else
                            {
                            (*frec).fr_leng = 5;
                            }
                        }
                    }
                }
            }
        }
    if ((char ) sql__setrang(to_fill , 0 , 1) || ((ptoc_Var52 = (int ) ptoc_Var49->fp_etype) == 0 || ptoc_Var52 == 2))
        {
        if (mbufiolen > (int ) (*frec).fr_leng)
            {
            if ((int ) ptoc_Var49->fp_datatyp == 24)
                {
                g10filuni("VAK506" , 18 , ptoc_Var50->integer.C_2.mb_data_size , ptoc_Var51->integer.C_1.mbp_buf , start + (int ) (*frec).fr_leng , mbufiolen - (int ) (*frec).fr_leng , "\0 " , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak506.p" , 15392)).integer.C_4.sp1r_returncode);
                }
            else
                {
                g10fil("VAK506" , 19 , ptoc_Var50->integer.C_2.mb_data_size , ptoc_Var51->integer.C_1.mbp_buf , start + (int ) (*frec).fr_leng , mbufiolen - (int ) (*frec).fr_leng , (unsigned char ) sql__setrang(ptoc_Var51->integer.C_1.mbp_buf[sql__setrang(start , 1 , 8192000) + -1] , 0 , 255) , &(*(tsp1_segment *) sql__nil((*acv).a_return_segm , "vak506.p" , 15397)).integer.C_4.sp1r_returncode);
                }
            }
        (*frec).fr_leng = (short ) sql__setrang((short ) mbufiolen , -32768 , 32767);
        }
    cccprint("<ak506datetime  ");
    }



int ak506in_mess_buf_move(acv, dmli, frec)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_fill_rec *frec;
    {
    tgg_mess_block_ptr pmbp;
    tgg_datapart_ptr pdbp;
    tak_all_command_glob *ptoc_Var54;
    struct tgg00_MessBlock *ptoc_Var55;
    tgg00_DataPart *ptoc_Var56;
    tak_dml_info *ptoc_Var57;
    tak_fill_rec *ptoc_Var58;
    cccprint("AK506 ak506in_me");
    ptoc_Var54 = &*acv;
    ptoc_Var55 = &ptoc_Var54->a_mblock;
    ptoc_Var56 = (tgg00_DataPart *) sql__nil(ptoc_Var55->integer.C_3.mb_data , "vak506.p" , 15421);
    ptoc_Var57 = &*dmli;
    ptoc_Var58 = &*frec;
    pmbp = (struct tgg00_MessBlock *) &(*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[sql__setrang((int ) ptoc_Var57->d_sparr.pcount , 1 , 8) + -1] , "vak506.p" , 15423)).integer.C_62.smessblock.mbr_mess_block;
    pdbp = (*(struct tgg00_MessBlock *) sql__nil(pmbp , "vak506.p" , 15425)).integer.C_3.mb_data;
    if ((int ) ptoc_Var58->fr_total_leng > 0 && pdbp != (tgg00_DataPart *) (int *) 0)
        {
        t01int4(5 , "from size   " , (*(struct tgg00_MessBlock *) sql__nil(pmbp , "vak506.p" , 15429)).integer.C_2.mb_data_size);
        t01int4(5 , "to   size   " , ptoc_Var55->integer.C_2.mb_data_size);
        t01int4(5 , "from pos    " , (int ) ptoc_Var57->d_pos_in_parsbuf);
        t01int4(5 , "to   pos    " , ptoc_Var55->integer.C_2.mb_data_len + 1);
        g10mv("VAK506" , 20 , (*(struct tgg00_MessBlock *) sql__nil(pmbp , "vak506.p" , 15434)).integer.C_2.mb_data_size , ptoc_Var55->integer.C_2.mb_data_size , (*(tgg00_DataPart *) sql__nil(pdbp , "vak506.p" , 15435)).integer.C_1.mbp_buf , (int ) ptoc_Var57->d_pos_in_parsbuf , (*(tgg00_DataPart *) sql__nil(ptoc_Var55->integer.C_3.mb_data , "vak506.p" , 15436)).integer.C_1.mbp_buf , ptoc_Var55->integer.C_2.mb_data_len + 1 , (int ) ptoc_Var58->fr_total_leng , &(*(tsp1_segment *) sql__nil(ptoc_Var54->a_return_segm , "vak506.p" , 15438)).integer.C_4.sp1r_returncode);
        }
    ptoc_Var58->fr_leng = 0;
    t01int4(5 , "total_leng  " , (int ) ptoc_Var58->fr_total_leng);
    t01int4(5 , "pos_in_parsb" , (int ) ptoc_Var57->d_pos_in_parsbuf);
    t01int4(5 , "leng        " , (int ) ptoc_Var58->fr_leng);
    t01int4(5 , "mb_data_len " , ptoc_Var55->integer.C_2.mb_data_len);
    t01int4(5 , "mbp_reclen  " , (int ) ptoc_Var56->integer.C_4.mbp_reclen);
    t01int4(5 , "mbp_keylen  " , (int ) ptoc_Var56->integer.C_4.mbp_keylen);
    ptoc_Var55->integer.C_2.mb_data_len = ptoc_Var55->integer.C_2.mb_data_len + (int ) ptoc_Var58->fr_total_leng;
    ptoc_Var57->d_pos_in_parsbuf = (short ) sql__setrang((short ) ((int ) ptoc_Var57->d_pos_in_parsbuf + (int ) ptoc_Var58->fr_total_leng) , -32768 , 32767);
    cccprint("<ak506in_mess_bu");
    }

static
  tsp00_LdState ptoc_Var59 =
  {
  (unsigned char)'\0'
  };
static
  tsp00_LdInfo ptoc_Var60 =
  {
  (unsigned char)'\200'
  };
static
  tsp00_LdState ptoc_Var61 =
  {
  (unsigned char)'\020'
  };
static
  tsp00_LdInfo ptoc_Var62 =
  {
  (unsigned char)'\0'
  };
static
  unsigned char ptoc_Var63[1] =
  {
  (unsigned char)'@'
  };
static
  unsigned char ptoc_Var64[1] =
  {
  (unsigned char)'\002'
  };
static
  unsigned char ptoc_Var65[1] =
  {
  (unsigned char)'\b'
  };
static
  unsigned char ptoc_Var66[1] =
  {
  (unsigned char)'\001'
  };
static
  unsigned char ptoc_Var67[1] =
  {
  (unsigned char)'\004'
  };


int ak506long_support(acv, dmli, frec, ind)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_fill_rec *frec;
int ind;
    {
    int desc_len;
    tsp_long_descriptor long_desc;
    char is_longlit;
    tak_all_command_glob *ptoc_Var68;
    tak_dml_info *ptoc_Var69;
    tak_parsinforecord *ptoc_Var70;
    tak_field_pars *ptoc_Var71;
    struct tgg00_MessBlock *ptoc_Var72;
    tgg00_DataPart *ptoc_Var73;
    tak_longliteral_rec *ptoc_Var74;
    struct ptoc_Type3 *ptoc_Var75;
    int ptoc_Var76;
    unsigned char ptoc_Var77[1];
    unsigned char ptoc_Var78[1];
    unsigned char ptoc_Var79[1];
    unsigned char ptoc_Var80[1];
    unsigned char ptoc_Var81[1];
    unsigned char ptoc_Var82[1];
    tak_longliteral_rec *ptoc_Var83;
    struct ptoc_Type177 *ptoc_Var84;
    cccprint("AK506 ak506long_");
    ptoc_Var68 = &*acv;
    ptoc_Var69 = &*dmli;
    ptoc_Var70 = &(*(struct tak_systembuffer *) sql__nil(ptoc_Var69->d_sparr.px[0] , "vak506.p" , 15469)).integer.C_23.sparsinfo;
    ptoc_Var71 = &ptoc_Var70->integer.C_2.p_pars_infos[sql__setrang(ind , 1 , 2025) + -1];
    ptoc_Var72 = &ptoc_Var68->a_mblock;
    ptoc_Var73 = (tgg00_DataPart *) sql__nil(ptoc_Var72->integer.C_3.mb_data , "vak506.p" , 15470);
    if ((int ) ptoc_Var71->fp_kind == 20)
        {
        (*frec).fr_total_leng = (short ) sql__setrang((short ) ((int ) ptoc_Var71->tak_fp_kind_type.C_fp_val_all_without_l.fp_movebefore_v2 - ptoc_Var72->integer.C_2.mb_data_len) , -32768 , 32767);
        if ((int ) (*frec).fr_total_leng > 0)
            {
            ak506in_mess_buf_move(&(*acv) , &(*dmli) , &(*frec));
            }
        }
    else
        {
        if ((int ) ptoc_Var71->tak_fp_kind_type.C_fp_catalog_column.fp_movebefore_v1 > 0)
            {
            (*frec).fr_total_leng = (short ) sql__setrang(ptoc_Var71->tak_fp_kind_type.C_fp_catalog_column.fp_movebefore_v1 , -32768 , 32767);
            ak506in_mess_buf_move(&(*acv) , &(*dmli) , &(*frec));
            }
        }
    is_longlit = 0;
    if ((*dmli).d_literalrec != (struct tak_systembuffer *) (int *) 0)
        {
        ptoc_Var74 = &(*(struct tak_systembuffer *) sql__nil((*dmli).d_literalrec , "vak506.p" , 15493)).integer.C_65.slongliteral;
        ptoc_Var74->litcolcount = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var74->litcolcount , -32768 , 32767 , "vak506.p" , 15495) , -32768 , 32767);
        is_longlit = (char ) sql__setrang(ptoc_Var74->boolean.C_true.litcoldesc[sql__setrang((int ) ptoc_Var74->litcolcount , 1 , 335) + -1].llcdatapos != -1 , 0 , 1);
        }
    if (!(char ) sql__setrang(is_longlit , 0 , 1))
        {
        ptoc_Var69->d_longdesc_found = 1;
        ptoc_Var69->d_cnt_longdesc = (short ) sql__setrang((short ) sql__succ((int ) ptoc_Var69->d_cnt_longdesc , -32768 , 32767 , "vak506.p" , 15504) , -32768 , 32767);
        if ((int ) ptoc_Var69->d_first_longdesc_pos == 0)
            {
            ptoc_Var69->d_first_longdesc_pos = (short ) sql__setrang((short ) ptoc_Var71->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 , -32768 , 32767);
            }
        if (_ptoc_MEMCMP(unsigned char , 5, ptoc_Var68->a_comp_vers , "62000") < 0 ? 1 : 0)
            {
            desc_len = 32;
            }
        else
            {
            desc_len = 40;
            }
        if (ptoc_Var68->a_input_data_pos < ptoc_Var71->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + desc_len + 1)
            {
            ptoc_Var68->a_input_data_pos = ptoc_Var71->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + desc_len + 1;
            }
        (*(tsp1_part *) sql__nil(ptoc_Var68->a_data_part , "vak506.p" , 15519)).integer.C_2.sp1p_arg_count = (short ) sql__setrang(ptoc_Var69->d_cnt_longdesc , -32768 , 32767);
        t01int4(5 , "d_first_ldps" , (int ) ptoc_Var69->d_first_longdesc_pos);
        t01int4(5 , "d_cntlongdes" , (int ) ptoc_Var69->d_cnt_longdesc);
        t01int4(5 , "fp_fromposv1" , ptoc_Var71->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1);
        t01int4(5 , "input_data_p" , ptoc_Var68->a_input_data_pos);
        if ((int ) (*(tsp00_MoveObj *) sql__nil(ptoc_Var68->a_data_ptr , "vak506.p" , 15524))[sql__setrang(ptoc_Var71->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 , 1 , 8192000) + -1] == 255)
            {
            ptoc_Var73->integer.C_1.mbp_buf[sql__setrang(ptoc_Var72->integer.C_2.mb_data_len + 1 , 1 , 8192000) + -1] = (unsigned char)'\377';
            g10fil("VAK506" , 21 , ptoc_Var72->integer.C_2.mb_data_size , ptoc_Var73->integer.C_1.mbp_buf , ptoc_Var72->integer.C_2.mb_data_len + 2 , 8 , (unsigned char)'\0' , &(*(tsp1_segment *) sql__nil(ptoc_Var68->a_return_segm , "vak506.p" , 15531)).integer.C_4.sp1r_returncode);
            ptoc_Var72->integer.C_2.mb_data_len = ptoc_Var72->integer.C_2.mb_data_len + 9;
            }
        else
            {
            g10mv7("VAK506" , 22 , ptoc_Var68->a_data_length , sizeof(struct ptoc_Type3 ) , sql__nil(ptoc_Var68->a_data_ptr , "vak506.p" , 15538) , ptoc_Var71->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + 1 , &long_desc , 1 , desc_len , &(*(tsp1_segment *) sql__nil(ptoc_Var68->a_return_segm , "vak506.p" , 15540)).integer.C_4.sp1r_returncode);
            ptoc_Var75 = &long_desc;
            _ptoc_ASSIGN(unsigned char , 8, ptoc_Var75->boolean.C_false.ld_tabid, ptoc_Var70->integer.C_2.p_treeid.integer.C_4.fileTabId_gg00)
            ptoc_Var75->boolean.C_false.ld_intern_pos = 1;
            if ((ptoc_Var76 = (int ) ptoc_Var71->fp_datatyp) == 19 || ptoc_Var76 == 20 || ptoc_Var76 == 35 || ptoc_Var76 == 21 || ptoc_Var76 == 22)
                {
                if ((int ) ptoc_Var68->a_mblock.integer.C_2.mb_type == 35 || (int ) ptoc_Var68->a_mblock.integer.C_2.mb_type == 36)
                    {
                    ptoc_Var75->boolean.C_false.ld_intern_pos = -1;
                    }
                _ptoc_MOVE(unsigned char , 1, ptoc_Var75->boolean.C_false.ld_state, ptoc_Var59);
                _ptoc_MOVE(unsigned char , 1, ptoc_Var75->boolean.C_false.ld_infoset, ptoc_Var60);
                }
            else
                {
                _ptoc_MOVE(unsigned char , 1, ptoc_Var75->boolean.C_false.ld_state, ptoc_Var61);
                _ptoc_MOVE(unsigned char , 1, ptoc_Var75->boolean.C_false.ld_infoset, ptoc_Var62);
                }
            if ((ptoc_Var76 = (int ) ptoc_Var71->fp_datatyp) == 34 || ptoc_Var76 == 35)
                {
                _ptoc_MOVE(unsigned char , 1, ptoc_Var75->boolean.C_false.ld_infoset, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var77 , ptoc_Var75->boolean.C_false.ld_infoset , ptoc_Var63 , sizeof(unsigned char [1])));
                }
            if ((int ) ptoc_Var68->a_basic_code != 1 && ((int ) ptoc_Var71->fp_datatyp == 7 || (int ) ptoc_Var71->fp_datatyp == 20) || (int ) ptoc_Var68->a_basic_code == 1 && ((int ) ptoc_Var71->fp_datatyp == 6 || (int ) ptoc_Var71->fp_datatyp == 19))
                {
                _ptoc_MOVE(unsigned char , 1, ptoc_Var75->boolean.C_false.ld_state, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var80 , ptoc_Var75->boolean.C_false.ld_state , ptoc_Var64 , sizeof(unsigned char [1])));
                }
            if (((int ) ptoc_Var71->fp_datatyp == 34 || (int ) ptoc_Var71->fp_datatyp == 35) && (int ) (*(tsp1_packet *) sql__nil(ptoc_Var68->a_out_packet , "vak506.p" , 15581)).sp1_header.sp1h_mess_swap != 1)
                {
                _ptoc_MOVE(unsigned char , 1, ptoc_Var75->boolean.C_false.ld_state, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var82 , ptoc_Var75->boolean.C_false.ld_state , ptoc_Var65 , sizeof(unsigned char [1])));
                }
            if (((ptoc_Var76 = (int ) ptoc_Var71->fp_datatyp) == 6 || ptoc_Var76 == 7 || ptoc_Var76 == 19 || ptoc_Var76 == 20) && (int ) (*(tsp1_packet *) sql__nil(ptoc_Var68->a_out_packet , "vak506.p" , 15586)).sp1_header.sp1h_mess_code >= 3 && (int ) (*(tsp1_packet *) sql__nil(ptoc_Var68->a_out_packet , "vak506.p" , 15588)).sp1_header.sp1h_mess_code <= 10)
                {
                _ptoc_MOVE(unsigned char , 1, ptoc_Var75->boolean.C_false.ld_state, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var80 , ptoc_Var75->boolean.C_false.ld_state , ptoc_Var66 , sizeof(unsigned char [1])));
                }
            if (((ptoc_Var76 = (int ) ptoc_Var71->fp_datatyp) == 6 || ptoc_Var76 == 7 || ptoc_Var76 == 19 || ptoc_Var76 == 20) && ((ptoc_Var76 = (int ) (*(tsp1_packet *) sql__nil(ptoc_Var68->a_out_packet , "vak506.p" , 15594)).sp1_header.sp1h_mess_code) == 20 || ptoc_Var76 == 19))
                {
                _ptoc_MOVE(unsigned char , 1, ptoc_Var75->boolean.C_false.ld_state, *(unsigned char (*)[1]) sql__psetadd(ptoc_Var82 , ptoc_Var75->boolean.C_false.ld_state , ptoc_Var67 , sizeof(unsigned char [1])));
                }
            ptoc_Var73->integer.C_1.mbp_buf[sql__setrang(ptoc_Var72->integer.C_2.mb_data_len + 1 , 1 , 8192000) + -1] = (unsigned char)'\0';
            if ((int ) ptoc_Var68->a_mblock.integer.C_2.mb_type == 35 || (int ) ptoc_Var68->a_mblock.integer.C_2.mb_type == 36)
                {
                a05_stamp_get(&(*acv) , ptoc_Var73->integer.C_1.mbp_buf , ptoc_Var72->integer.C_2.mb_data_len + 2 , ptoc_Var72->integer.C_2.mb_data_size);
                g10mv7("VAK506" , 23 , ptoc_Var72->integer.C_2.mb_data_size , sizeof(struct ptoc_Type3 ) , ptoc_Var73->integer.C_1.mbp_buf , ptoc_Var72->integer.C_2.mb_data_len + 2 , &long_desc , 1 , 8 , &(*(tsp1_segment *) sql__nil(ptoc_Var68->a_return_segm , "vak506.p" , 15610)).integer.C_4.sp1r_returncode);
                ptoc_Var72->integer.C_2.mb_data_len = ptoc_Var72->integer.C_2.mb_data_len + 9;
                if ((int ) ptoc_Var68->a_mblock.integer.C_2.mb_type == 36)
                    {
                    ptoc_Var69->d_pos_in_parsbuf = (short ) sql__setrang((short ) ((int ) ptoc_Var69->d_pos_in_parsbuf + 9) , -32768 , 32767);
                    }
                }
            else
                {
                g10fil("VAK506" , 24 , ptoc_Var72->integer.C_2.mb_data_size , ptoc_Var73->integer.C_1.mbp_buf , ptoc_Var72->integer.C_2.mb_data_len + 1 , 9 , (unsigned char)'\0' , &(*(tsp1_segment *) sql__nil(ptoc_Var68->a_return_segm , "vak506.p" , 15622)).integer.C_4.sp1r_returncode);
                ptoc_Var72->integer.C_2.mb_data_len = ptoc_Var72->integer.C_2.mb_data_len + 9;
                }
            g10mv8("VAK506" , 25 , sizeof(struct ptoc_Type3 ) , ptoc_Var68->a_data_length , &long_desc , 1 , sql__nil(ptoc_Var68->a_data_ptr , "vak506.p" , 15628) , ptoc_Var71->tak_fp_kind_type.C_fp_catalog_column.fp_frompos_v1 + 1 , desc_len , &(*(tsp1_segment *) sql__nil(ptoc_Var68->a_return_segm , "vak506.p" , 15629)).integer.C_4.sp1r_returncode);
            }
        }
    else
        {
        ptoc_Var73->integer.C_1.mbp_buf[sql__setrang(ptoc_Var72->integer.C_2.mb_data_len + 1 , 1 , 8192000) + -1] = (unsigned char)'\0';
        if ((int ) ptoc_Var68->a_mblock.integer.C_2.mb_replicated == 1)
            {
            a05_stamp_get(&(*acv) , ptoc_Var73->integer.C_1.mbp_buf , ptoc_Var72->integer.C_2.mb_data_len + 2 , ptoc_Var72->integer.C_2.mb_data_size);
            }
        else
            {
            a05_stamp_get(&(*acv) , ptoc_Var73->integer.C_1.mbp_buf , ptoc_Var72->integer.C_2.mb_data_len + 2 , ptoc_Var72->integer.C_2.mb_data_size);
            }
        ptoc_Var83 = &(*(struct tak_systembuffer *) sql__nil((*dmli).d_literalrec , "vak506.p" , 15646)).integer.C_65.slongliteral;
        ptoc_Var84 = &ptoc_Var83->boolean.C_true.litcoldesc[sql__setrang((int ) ptoc_Var83->litcolcount , 1 , 335) + -1];
        g10mv9("VAK506" , 26 , ptoc_Var72->integer.C_2.mb_data_size , sizeof(tsp00_C8 ) , ptoc_Var73->integer.C_1.mbp_buf , ptoc_Var72->integer.C_2.mb_data_len + 2 , ptoc_Var84->llclongid , 1 , 8 , &(*(tsp1_segment *) sql__nil(ptoc_Var68->a_return_segm , "vak506.p" , 15651)).integer.C_4.sp1r_returncode);
        ptoc_Var72->integer.C_2.mb_data_len = ptoc_Var72->integer.C_2.mb_data_len + 9;
        }
    cccprint("<ak506long_suppo");
    }



int ak506one_field_copy(acv, dmli, frec)
tak_all_command_glob *acv;
tak_dml_info *dmli;
tak_fill_rec *frec;
    {
    int ind;
    int curval_len;
    tsp_int_map_c2 currlen;
    tak_all_command_glob *ptoc_Var85;
    tak_fill_rec *ptoc_Var86;
    tak_parsinforecord *ptoc_Var87;
    tak_field_pars *ptoc_Var88;
    struct tgg00_MessBlock *ptoc_Var89;
    tgg00_DataPart *ptoc_Var90;
    tak_parsinforecord *ptoc_Var91;
    tak_field_pars *ptoc_Var92;
    tgg00_StackEntry *ptoc_Var93;
    cccprint("AK506 ak506one_f");
    ptoc_Var85 = &*acv;
    ptoc_Var86 = &*frec;
    ptoc_Var87 = &(*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak506.p" , 15675)).integer.C_23.sparsinfo;
    ptoc_Var88 = &ptoc_Var87->integer.C_2.p_pars_infos[sql__setrang((int ) ptoc_Var86->fr_f_no , 1 , 2025) + -1];
    ptoc_Var89 = &ptoc_Var85->a_mblock;
    ptoc_Var90 = (tgg00_DataPart *) sql__nil(ptoc_Var89->integer.C_3.mb_data , "vak506.p" , 15676);
    ind = (int ) ptoc_Var86->fr_f_no;
    if ((int ) ptoc_Var88->tak_fp_kind_type.C_fp_ordered_select_key.fp_movebefore_v8 > 0)
        {
        ptoc_Var86->fr_total_leng = (short ) sql__setrang(ptoc_Var88->tak_fp_kind_type.C_fp_ordered_select_key.fp_movebefore_v8 , -32768 , 32767);
        ak506in_mess_buf_move(&(*acv) , &(*dmli) , &(*frec));
        }
    ptoc_Var91 = &(*(struct tak_systembuffer *) sql__nil((*dmli).d_sparr.px[0] , "vak506.p" , 15686)).integer.C_23.sparsinfo;
    ptoc_Var92 = &ptoc_Var91->integer.C_2.p_pars_infos[sql__setrang(ind , 1 , 2025) + -1];
    ptoc_Var86->fr_leng = (short ) sql__setrang((*(tgg00_StackList *) sql__nil(ptoc_Var89->integer.C_5.mb_st , "vak506.p" , 15688))[sql__setrang((int ) ptoc_Var92->tak_fp_kind_type.C_fp_ordered_select_key.fp_keystack , 1 , 32767) + -1].integer.C_1.elen_var , -32768 , 32767);
    if ((int ) ptoc_Var92->tak_fp_kind_type.C_fp_ordered_select_key.fp_key_low_handl != 0)
        {
        curval_len = s30lnr_defbyte((tsp00_MoveObj *) ptoc_Var90->integer.C_1.mbp_buf , (unsigned char ) sql__setrang(ptoc_Var90->integer.C_1.mbp_buf[sql__setrang((int ) (*(tgg00_StackList *) sql__nil(ptoc_Var89->integer.C_5.mb_st , "vak506.p" , 15693))[sql__setrang((int ) ptoc_Var92->tak_fp_kind_type.C_fp_ordered_select_key.fp_keystack , 1 , 32767) + -1].integer.C_1.epos , 1 , 8192000) + -1] , 0 , 255) , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var89->integer.C_5.mb_st , "vak506.p" , 15694))[sql__setrang((int ) ptoc_Var92->tak_fp_kind_type.C_fp_ordered_select_key.fp_keystack , 1 , 32767) + -1].integer.C_1.epos + 1 , (int ) ptoc_Var86->fr_leng + -1) + 1;
        if ((int ) ptoc_Var92->tak_fp_kind_type.C_fp_ordered_select_key.fp_key_low_handl == -1)
            {
            ptoc_Var86->fr_leng = (short ) sql__setrang((short ) curval_len , -32768 , 32767);
            }
        }
    if ((int ) ptoc_Var92->tak_fp_kind_type.C_fp_ordered_select_key.fp_to_stack == 0)
        {
        if (ptoc_Var92->tak_fp_kind_type.C_fp_ordered_select_key.fp_fill_81 > 255)
            {
            currlen.boolean.C_true.map_int = (short ) sql__setrang(ptoc_Var86->fr_leng , -32768 , 32767);
            ptoc_Var90->integer.C_1.mbp_buf[sql__setrang(ptoc_Var89->integer.C_2.mb_data_len + 1 , 1 , 8192000) + -1] = (unsigned char ) sql__setrang(currlen.boolean.C_false.map_c2[0] , 0 , 255);
            ptoc_Var90->integer.C_1.mbp_buf[sql__setrang(ptoc_Var89->integer.C_2.mb_data_len + 2 , 1 , 8192000) + -1] = (unsigned char ) sql__setrang(currlen.boolean.C_false.map_c2[1] , 0 , 255);
            ptoc_Var89->integer.C_2.mb_data_len = ptoc_Var89->integer.C_2.mb_data_len + 2;
            }
        else
            {
            ptoc_Var89->integer.C_2.mb_data_len = sql__succ(ptoc_Var89->integer.C_2.mb_data_len , -2147483647 , 2147483647 , "vak506.p" , 15713);
            ptoc_Var90->integer.C_1.mbp_buf[sql__setrang(ptoc_Var89->integer.C_2.mb_data_len , 1 , 8192000) + -1] = (unsigned char ) sql__setrang((unsigned char ) ptoc_Var86->fr_leng , 0 , 255);
            }
        }
    else
        {
        ptoc_Var93 = &(*(tgg00_StackList *) sql__nil(ptoc_Var89->integer.C_5.mb_st , "vak506.p" , 15718))[sql__setrang((int ) ptoc_Var92->tak_fp_kind_type.C_fp_ordered_select_key.fp_to_stack , 1 , 32767) + -1];
        ptoc_Var93->integer.C_1.elen_var = (short ) sql__setrang((short ) ((int ) ptoc_Var93->integer.C_1.elen_var + (int ) ptoc_Var86->fr_leng) , -32768 , 32767);
        }
    g10mv("VAK506" , 27 , ptoc_Var89->integer.C_2.mb_data_size , ptoc_Var89->integer.C_2.mb_data_size , ptoc_Var90->integer.C_1.mbp_buf , (int ) (*(tgg00_StackList *) sql__nil(ptoc_Var89->integer.C_5.mb_st , "vak506.p" , 15724))[sql__setrang((int ) ptoc_Var92->tak_fp_kind_type.C_fp_ordered_select_key.fp_keystack , 1 , 32767) + -1].integer.C_1.epos , ptoc_Var90->integer.C_1.mbp_buf , ptoc_Var89->integer.C_2.mb_data_len + 1 , (int ) ptoc_Var86->fr_leng , &(*(tsp1_segment *) sql__nil(ptoc_Var85->a_return_segm , "vak506.p" , 15726)).integer.C_4.sp1r_returncode);
    if ((int ) ptoc_Var92->tak_fp_kind_type.C_fp_ordered_select_key.fp_key_low_handl == 1)
        {
        g10fil("VAK506" , 28 , ptoc_Var89->integer.C_2.mb_data_size , ptoc_Var90->integer.C_1.mbp_buf , ptoc_Var89->integer.C_2.mb_data_len + curval_len + 1 , (int ) ptoc_Var86->fr_leng - curval_len , (unsigned char)'\0' , &(*(tsp1_segment *) sql__nil(ptoc_Var85->a_return_segm , "vak506.p" , 15732)).integer.C_4.sp1r_returncode);
        }
    ptoc_Var89->integer.C_2.mb_data_len = ptoc_Var89->integer.C_2.mb_data_len + (int ) ptoc_Var86->fr_leng;
    t01name(5 , "after copy_withlen");
    t01messblock(5 , "MBLOCK 506on" , &ptoc_Var85->a_mblock);
    cccprint("<ak506one_field_");
    }



int ak506pseudo_insert_select(acv, dmli)
tak_all_command_glob *acv;
tak_dml_info *dmli;
    {
    char dummy_bool;
    int aux_len;
    tgg_rec_buf rec_buf;
    tgg_rec_buf res_buf;
    tgg_select_fields_param sel;
    tak_all_command_glob *ptoc_Var94;
    struct tgg00_MessBlock *ptoc_Var95;
    tgg00_DataPart *ptoc_Var96;
    cccprint("AK506 ak506pseud");
    ptoc_Var94 = &*acv;
    ptoc_Var95 = &ptoc_Var94->a_mblock;
    ptoc_Var96 = (tgg00_DataPart *) sql__nil(ptoc_Var95->integer.C_3.mb_data , "vak506.p" , 15760);
    ptoc_Var95->integer.C_6.mb_work_st = ptoc_Var94->a_work_st_addr;
    ptoc_Var95->integer.C_2.mb_work_st_max = (short ) sql__setrang(ptoc_Var94->a_work_st_max , -32768 , 32767);
    ptoc_Var95->integer.C_7.mb_workbuf = ptoc_Var94->a_work_buf_addr;
    ptoc_Var95->integer.C_2.mb_workbuf_size = ptoc_Var94->a_work_buf_size;
    g04init_select_fields(&sel , (tsp00_MoveObj *) ptoc_Var96->integer.C_1.mbp_buf , ptoc_Var95->integer.C_2.mb_data_size , ptoc_Var95->integer.C_6.mb_work_st , (short ) sql__setrang(ptoc_Var95->integer.C_2.mb_work_st_max , -32768 , 32767) , ptoc_Var95->integer.C_7.mb_workbuf , ptoc_Var95->integer.C_2.mb_workbuf_size , (unsigned char ) sql__setrang(ptoc_Var94->a_sqlmode , 0 , 3));
    sel.integer.C_1.sfp_result_wanted = 1;
    sel.integer.C_1.sfp_result_length = 8;
    sel.integer.C_2.sfp_m_result_addr = (tsp00_MoveObj *) &res_buf;
    sel.integer.C_1.sfp_m_result_size = sizeof(tgg00_Rec );
    rec_buf.integer.C_12.len = 8;
    rec_buf.integer.C_12.keylen = 0;
    if ((char ) sql__setrang((*(tgg00_QualBuf *) sql__nil(ptoc_Var94->a_mblock.integer.C_4.mb_qual , "vak506.p" , 15775)).integer.C_2.msubquery , 0 , 1))
        {
        k720_test_subquery(&ptoc_Var94->a_mblock , &res_buf);
        }
    res_buf.integer.C_12.keylen = 0;
    k71col_select(sql__nil(ptoc_Var95->integer.C_9.mb_trns , "vak506.p" , 15780) , &sel , &(*(tgg00_QualBuf *) sql__nil(ptoc_Var95->integer.C_4.mb_qual , "vak506.p" , 15780)).integer.C_3.mstack_desc , &rec_buf , &dummy_bool);
    if ((int ) (*(tgg00_TransContext *) sql__nil(ptoc_Var95->integer.C_9.mb_trns , "vak506.p" , 15782)).trError_gg00 != 0)
        {
        a07_b_put_error(&(*acv) , (short ) sql__setrang((*(tgg00_TransContext *) sql__nil(ptoc_Var94->a_mblock.integer.C_9.mb_trns , "vak506.p" , 15785)).trError_gg00 , -32768 , 32767) , 1);
        }
    else
        {
        t01buf(5 , res_buf.integer.C_11.buf , 1 , (int ) res_buf.integer.C_12.len);
        res_buf.integer.C_12.len = (short ) sql__setrang(sel.integer.C_1.sfp_result_length , -32768 , 32767);
        g10mv6("VAK506" , 29 , sizeof(tsp00_Buf ) , ptoc_Var95->integer.C_2.mb_data_size , res_buf.integer.C_11.buf , 1 , ptoc_Var96->integer.C_1.mbp_buf , 1 , (int ) sel.integer.C_1.sfp_result_length , &(*(tsp1_segment *) sql__nil(ptoc_Var94->a_return_segm , "vak506.p" , 15793)).integer.C_4.sp1r_returncode);
        (*(tgg00_QualBuf *) sql__nil(ptoc_Var95->integer.C_4.mb_qual , "vak506.p" , 15794)).integer.C_2.mqual_cnt = 0;
        (*(tgg00_QualBuf *) sql__nil(ptoc_Var95->integer.C_4.mb_qual , "vak506.p" , 15795)).integer.C_2.mstrat_cnt = 0;
        ptoc_Var95->integer.C_2.mb_data_len = (int ) res_buf.integer.C_12.len;
        ptoc_Var95->integer.C_2.mb_type = 35;
        if ((int ) (*(tgg00_DataPart *) sql__nil(ptoc_Var95->integer.C_3.mb_data , "vak506.p" , 15798)).integer.C_4.mbp_keylen == 0)
            {
            ptoc_Var96->integer.C_1.mbp_buf[8] = (unsigned char)'\0';
            if ((int ) ptoc_Var95->integer.C_2.mb_replicated == 1)
                {
                a05_stamp_get(&(*acv) , (*(tgg00_DataPart *) sql__nil(ptoc_Var95->integer.C_3.mb_data , "vak506.p" , 15805)).integer.C_1.mbp_buf , 10 , ptoc_Var95->integer.C_2.mb_data_size);
                }
            else
                {
                if (1 && ((*(tgg00_QualBuf *) sql__nil(ptoc_Var95->integer.C_4.mb_qual , "vak506.p" , 15808)).integer.C_2.mtree.integer.C_1.fileType_gg00[0] & 1) != 0)
                    {
                    aux_len = ptoc_Var95->integer.C_2.mb_data_len;
                    a56temp_table_key(&(*acv) , &(*dmli));
                    (*(tgg00_DataPart *) sql__nil(ptoc_Var95->integer.C_3.mb_data , "vak506.p" , 15813)).integer.C_4.mbp_reclen = (short ) sql__setrang((short ) aux_len , -32768 , 32767);
                    ptoc_Var95->integer.C_2.mb_data_len = aux_len;
                    }
                else
                    {
                    ptoc_Var95->integer.C_2.mb_type2 = 42;
                    }
                }
            (*(tgg00_DataPart *) sql__nil(ptoc_Var95->integer.C_3.mb_data , "vak506.p" , 15820)).integer.C_4.mbp_keylen = 9;
            }
        else
            {
            ptoc_Var95->integer.C_2.mb_type2 = 0;
            }
        }
    cccprint("<ak506pseudo_ins");
    }

static
  tsp8_uni_opt_set ptoc_Var97 =
  {
  (unsigned char)'\0'
  };
static
  tsp8_uni_opt_set ptoc_Var98 =
  {
  (unsigned char)'\0'
  };
static
  tsp8_uni_opt_set ptoc_Var99 =
  {
  (unsigned char)'\0'
  };
static
  tsp8_uni_opt_set ptoc_Var100 =
  {
  (unsigned char)'\0'
  };
static
  tsp8_uni_opt_set ptoc_Var101 =
  {
  (unsigned char)'\0'
  };


int ak506val_from_part2(acv, valpos, field_kind, colinfo, iolen, frec, to_fill)
tak_all_command_glob *acv;
int valpos;
tak_fp_kind_type field_kind;
tak00_columninfo *colinfo;
int iolen;
tak_fill_rec *frec;
char *to_fill;
    {
    tgg_basis_error b_err;
    char truncated;
    int destpos;
    int i;
    int val;
    int numlen;
    int uniinlen;
    int univallen;
    tsp_number intern_num;
    tsp_num_error err;
    tsp8_uni_error error;
    tsp_int4 err_char_no;
    tsp_c64 c64;
    tsp_moveobj_ptr c64_ptr;
    char no_ebcdic_to_ascii;
    tak_all_command_glob *ptoc_Var102;
    struct tgg00_MessBlock *ptoc_Var103;
    tgg00_DataPart *ptoc_Var104;
    tak00_columninfo *ptoc_Var105;
    int ptoc_Var106;
    tsp8_uni_opt_set ptoc_Var107;
    int ptoc_Var108;
    cccprint("AK506 ak506val_f");
    ptoc_Var102 = &*acv;
    ptoc_Var103 = &ptoc_Var102->a_mblock;
    ptoc_Var104 = (tgg00_DataPart *) sql__nil(ptoc_Var103->integer.C_3.mb_data , "vak506.p" , 15862);
    ptoc_Var105 = &*colinfo;
    t01int4(5 , "data_length " , ptoc_Var102->a_data_length);
    t01int4(5 , "valpos      " , valpos);
    b_err = 0;
    truncated = 0;
    destpos = ptoc_Var103->integer.C_2.mb_data_len + 1;
    (*frec).fr_leng = 0;
    if (ptoc_Var102->a_input_data_pos < valpos + iolen)
        {
        ptoc_Var102->a_input_data_pos = valpos + iolen;
        }
    if (ptoc_Var102->a_data_length + 1 < valpos + iolen)
        {
        if (((_ptoc_MEMCMP(unsigned char , 5, ptoc_Var102->a_comp_vers , "62080") < 0 ? 1 : 0) || (int ) ptoc_Var102->a_comp_type == 0) && (int ) ptoc_Var105->integer.C_1.cdatalen > 18 && ((int ) ptoc_Var105->integer.C_1.cdatatyp == 0 || (int ) ptoc_Var105->integer.C_1.cdatatyp == 1 || (int ) ptoc_Var105->integer.C_1.cdatatyp == 12))
            {
            if (ptoc_Var102->a_data_length + 1 < valpos + 11)
                {
                a07_b_put_error(&(*acv) , 6488 , -valpos);
                }
            }
        else
            {
            a07_b_put_error(&(*acv) , 6488 , -valpos);
            }
        }
    if ((int ) (*(tsp1_segment *) sql__nil(ptoc_Var102->a_return_segm , "vak506.p" , 15895)).integer.C_4.sp1r_returncode == 0)
        {
        if ((int ) (*(tsp00_MoveObj *) sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 15897))[sql__setrang(valpos , 1 , 8192000) + -1] == 255)
            {
            ptoc_Var104->integer.C_1.mbp_buf[sql__setrang(destpos , 1 , 8192000) + -1] = (unsigned char)'\377';
            if ((int ) ptoc_Var105->integer.C_1.ccolstack.integer.C_1.etype == 1 || (int ) ptoc_Var105->integer.C_1.ccolstack.integer.C_1.etype == 3 || (int ) ptoc_Var105->integer.C_1.ccolstack.integer.C_1.etype == 13)
                {
                (*frec).fr_leng = 0;
                }
            else
                {
                (*frec).fr_leng = (short ) sql__setrang((short ) ((int ) ptoc_Var105->integer.C_1.cinoutlen + -1) , -32768 , 32767);
                g10fil("VAK506" , 30 , ptoc_Var103->integer.C_2.mb_data_size , ptoc_Var104->integer.C_1.mbp_buf , destpos + 1 , (int ) (*frec).fr_leng , (unsigned char)'\0' , &(*(tsp1_segment *) sql__nil(ptoc_Var102->a_return_segm , "vak506.p" , 15911)).integer.C_4.sp1r_returncode);
                }
            }
        else
            {
            if (((int ) ptoc_Var105->integer.C_1.ccolstack.integer.C_1.etype == 1 || (int ) ptoc_Var105->integer.C_1.ccolstack.integer.C_1.etype == 3 || (int ) ptoc_Var105->integer.C_1.ccolstack.integer.C_1.etype == 13) && !(char ) sql__setrang(*to_fill , 0 , 1))
                {
                if ((int ) ptoc_Var105->integer.C_1.cdatatyp == 0 || (int ) ptoc_Var105->integer.C_1.cdatatyp == 1 || (int ) ptoc_Var105->integer.C_1.cdatatyp == 12 || (int ) ptoc_Var105->integer.C_1.cdatatyp == 30 || (int ) ptoc_Var105->integer.C_1.cdatatyp == 29 || (int ) ptoc_Var105->integer.C_1.cdatatyp == 4)
                    {
                    if (((_ptoc_MEMCMP(unsigned char , 5, ptoc_Var102->a_comp_vers , "62080") < 0 ? 1 : 0) || (int ) ptoc_Var102->a_comp_type == 0) && (int ) ptoc_Var105->integer.C_1.cdatalen > 18 && ((int ) ptoc_Var105->integer.C_1.cdatatyp == 0 || (int ) ptoc_Var105->integer.C_1.cdatatyp == 1 || (int ) ptoc_Var105->integer.C_1.cdatatyp == 12))
                        {
                        (*frec).fr_leng = (short ) sql__setrang((short ) s30lnr(sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 15940) , (unsigned char)'\0' , valpos + 1 , 10) , -32768 , 32767);
                        }
                    else
                        {
                        (*frec).fr_leng = (short ) sql__setrang((short ) s30lnr(sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 15943) , (unsigned char)'\0' , valpos + 1 , iolen + -1) , -32768 , 32767);
                        }
                    truncated = 1;
                    }
                else
                    {
                    if (((ptoc_Var106 = (int ) (*(tsp1_packet *) sql__nil(ptoc_Var102->a_in_packet , "vak506.p" , 15952)).sp1_header.sp1h_mess_code) == 20 || ptoc_Var106 == 19) && ((ptoc_Var106 = (int ) ptoc_Var105->integer.C_1.cdatatyp) == 24 || ptoc_Var106 == 2 || ptoc_Var106 == 3 || ptoc_Var106 == 31 || ptoc_Var106 == 32 || ptoc_Var106 == 10 || ptoc_Var106 == 11 || ptoc_Var106 == 13) || (int ) ptoc_Var105->integer.C_1.cdatatyp == 24)
                        {
                        (*frec).fr_leng = (short ) sql__setrang((short ) s30unilnr(ptoc_Var102->a_data_ptr , (*frec).fr_uni_fill_c2 , valpos + 1 , iolen + -1) , -32768 , 32767);
                        }
                    else
                        {
                        (*frec).fr_leng = (short ) sql__setrang((short ) s30lnr(sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 15964) , (unsigned char)' ' , valpos + 1 , iolen + -1) , -32768 , 32767);
                        }
                    truncated = 1;
                    }
                }
            else
                {
                if (((_ptoc_MEMCMP(unsigned char , 5, ptoc_Var102->a_comp_vers , "62080") < 0 ? 1 : 0) || (int ) ptoc_Var102->a_comp_type == 0) && (int ) ptoc_Var105->integer.C_1.cdatalen > 18 && ((int ) ptoc_Var105->integer.C_1.cdatatyp == 0 || (int ) ptoc_Var105->integer.C_1.cdatatyp == 1 || (int ) ptoc_Var105->integer.C_1.cdatatyp == 12 || (int ) ptoc_Var105->integer.C_1.cdatatyp == 30 || (int ) ptoc_Var105->integer.C_1.cdatatyp == 29))
                    {
                    if ((int ) field_kind == 18 && ((ptoc_Var106 = (int ) ptoc_Var105->integer.C_1.ccolstack.integer.C_1.etype) == 0 || ptoc_Var106 == 2))
                        {
                        *to_fill = 1;
                        }
                    (*frec).fr_leng = 10;
                    }
                else
                    {
                    (*frec).fr_leng = (short ) sql__setrang((short ) (iolen + -1) , -32768 , 32767);
                    }
                }
            t01int4(5 , "to_fill     " , sql__ord((int ) (char ) sql__setrang(*to_fill , 0 , 1)));
            t01int4(5 , "valpos      " , valpos);
            t01char(5 , "undef_byte  " , (unsigned char ) sql__setrang((*(tsp00_MoveObj *) sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 15997))[sql__setrang(valpos , 1 , 8192000) + -1] , 0 , 255));
            t01int4(5 , "start       " , destpos);
            t01int4(5 , "frec.fr_leng" , (int ) (*frec).fr_leng);
            switch ((int ) ptoc_Var105->integer.C_1.cdatatyp)
                {
                case 24 :
                    ptoc_Var104->integer.C_1.mbp_buf[sql__setrang(destpos , 1 , 8192000) + -1] = (unsigned char)'\001';
                    if ((int ) (*frec).fr_leng == iolen + -1)
                        {
                        uniinlen = s30unilnr(ptoc_Var102->a_data_ptr , (*frec).fr_uni_fill_c2 , valpos + 1 , iolen + -1);
                        }
                    else
                        {
                        uniinlen = (int ) (*frec).fr_leng;
                        }
                    if ((int ) (*(tsp1_packet *) sql__nil(ptoc_Var102->a_in_packet , "vak506.p" , 16012)).sp1_header.sp1h_mess_swap != 1 && (int ) (*(tsp1_packet *) sql__nil(ptoc_Var102->a_in_packet , "vak506.p" , 16013)).sp1_header.sp1h_mess_code != 20)
                        {
                        univallen = (int ) ptoc_Var105->integer.C_1.cinoutlen + -1;
                        s80uni_trans((tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16017))[sql__setrang(valpos + 1 , 1 , 8192000) + -1] , uniinlen , 19 , (tsp00_MoveObj *) &ptoc_Var104->integer.C_1.mbp_buf[sql__setrang(destpos + 1 , 1 , 8192000) + -1] , &univallen , 20 , *(tsp8_uni_opt_set *) (_ptoc_MOVE(unsigned char , 1, ptoc_Var107, ptoc_Var97) , ptoc_Var107) , &error , &err_char_no);
                        }
                    else
                        {
                        univallen = uniinlen;
                        error = 0;
                        g10mv("VAK506" , 31 , ptoc_Var102->a_data_length , ptoc_Var103->integer.C_2.mb_data_size , sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16028) , valpos + 1 , ptoc_Var104->integer.C_1.mbp_buf , destpos + 1 , uniinlen , &(*(tsp1_segment *) sql__nil(ptoc_Var102->a_return_segm , "vak506.p" , 16030)).integer.C_4.sp1r_returncode);
                        }
                    if ((int ) error == 0)
                        {
                        if (((int ) ptoc_Var105->integer.C_1.ccolstack.integer.C_1.etype == 1 || (int ) ptoc_Var105->integer.C_1.ccolstack.integer.C_1.etype == 3 || (int ) ptoc_Var105->integer.C_1.ccolstack.integer.C_1.etype == 13) && !(char ) sql__setrang(*to_fill , 0 , 1))
                            {
                            (*frec).fr_leng = (short ) sql__setrang((short ) univallen , -32768 , 32767);
                            }
                        else
                            {
                            i = (int ) ptoc_Var105->integer.C_1.cinoutlen - univallen + -1;
                            t01int4(5 , "fill_length " , i);
                            if (i > 0)
                                {
                                g10filuni("VAK506" , 32 , ptoc_Var103->integer.C_2.mb_data_size , ptoc_Var104->integer.C_1.mbp_buf , destpos + univallen + 1 , i , "\0 " , &(*(tsp1_segment *) sql__nil(ptoc_Var102->a_return_segm , "vak506.p" , 16052)).integer.C_4.sp1r_returncode);
                                }
                            (*frec).fr_leng = (short ) sql__setrang((short ) sql__pred((int ) ptoc_Var105->integer.C_1.cinoutlen , -32768 , 32767 , "vak506.p" , 16054) , -32768 , 32767);
                            }
                        }
                    else
                        {
                        t01int4(5 , "err_char_no " , err_char_no);
                        a07_hex_uni_error(&(*acv) , (unsigned char ) sql__setrang(error , 0 , 8) , -(valpos + err_char_no) , 1 , (tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16062))[sql__setrang(valpos + err_char_no , 1 , 8192000) + -1] , (int ) ptoc_Var102->a_max_codewidth);
                        }
                    break;
                case 23 :
                    ptoc_Var104->integer.C_1.mbp_buf[sql__setrang(destpos , 1 , 8192000) + -1] = (unsigned char)'\0';
                    if ((int ) (*(tsp00_MoveObj *) sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16070))[sql__setrang(valpos + 1 , 1 , 8192000) + -1] != 1 && (int ) (*(tsp00_MoveObj *) sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16071))[sql__setrang(valpos + 1 , 1 , 8192000) + -1] != 0)
                        {
                        a07_b_put_error(&(*acv) , 6224 , -valpos);
                        }
                    else
                        {
                        ptoc_Var104->integer.C_1.mbp_buf[sql__setrang(destpos + 1 , 1 , 8192000) + -1] = (unsigned char ) sql__setrang((*(tsp00_MoveObj *) sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16076))[sql__setrang(valpos + 1 , 1 , 8192000) + -1] , 0 , 255);
                        }
                    break;
                case 0 :
                case 1 :
                case 12 :
                case 29 :
                case 30 :
                    ptoc_Var104->integer.C_1.mbp_buf[sql__setrang(destpos , 1 , 8192000) + -1] = (unsigned char)'\0';
                    ptoc_Var106 = valpos + 2;
                    ptoc_Var108 = valpos + (int ) (*frec).fr_leng;
                    if (ptoc_Var106 <= ptoc_Var108)
                        {
                        i = ptoc_Var106;
                        do
                            {
                            val = sql__ord((int ) (*(tsp00_MoveObj *) sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16084))[sql__setrang(i , 1 , 8192000) + -1]);
                            if (val > 153 || ((ptoc_Var106 = val % 16) >= 0 ? ptoc_Var106 : ptoc_Var106 + 16) > 9)
                                {
                                a07_b_put_error(&(*acv) , 6302 , -valpos);
                                }
                            if (i == ptoc_Var108)
                                {
                                break;
                                }
                            i += 1;
                            }
                        while (!0);
                        }
                    s51kroun(sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16092) , valpos + 1 , (int ) (*frec).fr_leng , ptoc_Var104->integer.C_1.mbp_buf , destpos + 1 , (int ) ptoc_Var105->integer.C_1.cdatalen , (int ) ptoc_Var105->integer.C_1.cdatafrac + -1 , &numlen , &err);
                    if ((int ) err == 0 && (char ) sql__setrang(ptoc_Var105->integer.C_2.cbinary , 0 , 1))
                        {
                        _ptoc_ASSIGN(unsigned char , 20, intern_num, "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0")
                        g10mv10("VAK506" , 33 , ptoc_Var103->integer.C_2.mb_data_size , sizeof(tsp00_C20 ) , ptoc_Var104->integer.C_1.mbp_buf , destpos + 1 , intern_num , 1 , numlen , &(*(tsp1_segment *) sql__nil(ptoc_Var102->a_return_segm , "vak506.p" , 16104)).integer.C_4.sp1r_returncode);
                        if ((int ) ptoc_Var105->integer.C_1.cdatalen == 5)
                            {
                            if ((_ptoc_MEMCMP(unsigned char , 20, intern_num , "\3052vp\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0") > 0 ? 1 : 0) || (_ptoc_MEMCMP(unsigned char , 20, intern_num , ";g# \0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0") < 0 ? 1 : 0))
                                {
                                err = 2;
                                }
                            }
                        else
                            {
                            if ((_ptoc_MEMCMP(unsigned char , 20, intern_num , "\312!GH6G\0\0\0\0\0\0\0\0\0\0\0\0\0\0") > 0 ? 1 : 0) || (_ptoc_MEMCMP(unsigned char , 20, intern_num , "6xRQcR\0\0\0\0\0\0\0\0\0\0\0\0\0\0") < 0 ? 1 : 0))
                                {
                                err = 2;
                                }
                            }
                        }
                    if ((int ) err != 0)
                        {
                        switch ((int ) err)
                            {
                            case 3 :
                                a07_b_put_error(&(*acv) , 4320 , -valpos);
                                break;
                            case 1 :
                                a07_b_put_error(&(*acv) , 6372 , -valpos);
                                break;
                            case 2 :
                                a07_b_put_error(&(*acv) , 4330 , -valpos);
                                break;
                            default :
                                (unsigned char ) sql__caseerr("vak506.p" , 16125);
                                break;
                            }
                        }
                    break;
                case 10 :
                    ptoc_Var104->integer.C_1.mbp_buf[sql__setrang(destpos , 1 , 8192000) + -1] = (unsigned char)' ';
                    if ((ptoc_Var108 = (int ) (*(tsp1_packet *) sql__nil(ptoc_Var102->a_in_packet , "vak506.p" , 16142)).sp1_header.sp1h_mess_code) == 20 || ptoc_Var108 == 19)
                        {
                        (*frec).fr_leng = (short ) sql__setrang((short ) s30unilnr(ptoc_Var102->a_data_ptr , (*frec).fr_uni_fill_c2 , valpos + 1 , (int ) (*frec).fr_leng) , -32768 , 32767);
                        univallen = sizeof(unsigned char [64]);
                        c64_ptr = (tsp00_MoveObj *) c64;
                        s80uni_trans((tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16151))[sql__setrang(valpos + 1 , 1 , 8192000) + -1] , (int ) (*frec).fr_leng , (short ) sql__setrang((short ) (*(tsp1_packet *) sql__nil(ptoc_Var102->a_in_packet , "vak506.p" , 16152)).sp1_header.sp1h_mess_code , -32768 , 32767) , c64_ptr , &univallen , 0 , *(tsp8_uni_opt_set *) (_ptoc_MOVE(unsigned char , 1, ptoc_Var107, ptoc_Var98) , ptoc_Var107) , &error , &err_char_no);
                        if ((int ) error == 0)
                            {
                            g03fdc_date((tsp00_MoveObj *) c64 , (tsp00_MoveObj *) ptoc_Var104->integer.C_1.mbp_buf , 1 , destpos + 1 , univallen , (unsigned char ) sql__setrang(ptoc_Var102->a_dt_format , 0 , 9) , 1 , &b_err);
                            }
                        else
                            {
                            a07_hex_uni_error(&(*acv) , (unsigned char ) sql__setrang(error , 0 , 8) , -(valpos + err_char_no) , 0 , (tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16163))[sql__setrang(valpos + err_char_no , 1 , 8192000) + -1] , (int ) ptoc_Var102->a_max_codewidth);
                            }
                        }
                    else
                        {
                        g02fromtermchar((unsigned char ) sql__setrang(ptoc_Var102->a_mess_code , 0 , 255) , sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16169) , valpos , iolen);
                        (*frec).fr_leng = (short ) sql__setrang((short ) s30lnr(sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16171) , (unsigned char)' ' , valpos + 1 , 10) , -32768 , 32767);
                        t01int4(5 , "g1code.ctype" , (int ) g01code.ctype);
                        t01int4(5 , "basic_code  " , (int ) ptoc_Var102->a_basic_code);
                        no_ebcdic_to_ascii = (char ) sql__setrang((int ) g01code.ctype == (int ) ptoc_Var102->a_basic_code || (int ) ptoc_Var102->a_basic_code != 1 , 0 , 1);
                        g03fdcheck_date(sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16178) , ptoc_Var104->integer.C_1.mbp_buf , valpos + 1 , destpos + 1 , (int ) (*frec).fr_leng , (unsigned char ) sql__setrang(ptoc_Var102->a_dt_format , 0 , 9) , (char ) sql__setrang(no_ebcdic_to_ascii , 0 , 1) , &b_err);
                        }
                    (*frec).fr_leng = 8;
                    break;
                case 11 :
                    ptoc_Var104->integer.C_1.mbp_buf[sql__setrang(destpos , 1 , 8192000) + -1] = (unsigned char)' ';
                    if ((ptoc_Var108 = (int ) (*(tsp1_packet *) sql__nil(ptoc_Var102->a_in_packet , "vak506.p" , 16188)).sp1_header.sp1h_mess_code) == 20 || ptoc_Var108 == 19)
                        {
                        (*frec).fr_leng = (short ) sql__setrang((short ) s30unilnr(ptoc_Var102->a_data_ptr , (*frec).fr_uni_fill_c2 , valpos + 1 , (int ) (*frec).fr_leng) , -32768 , 32767);
                        univallen = sizeof(unsigned char [64]);
                        c64_ptr = (tsp00_MoveObj *) c64;
                        s80uni_trans((tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16197))[sql__setrang(valpos + 1 , 1 , 8192000) + -1] , (int ) (*frec).fr_leng , (short ) sql__setrang((short ) (*(tsp1_packet *) sql__nil(ptoc_Var102->a_in_packet , "vak506.p" , 16198)).sp1_header.sp1h_mess_code , -32768 , 32767) , c64_ptr , &univallen , 0 , *(tsp8_uni_opt_set *) (_ptoc_MOVE(unsigned char , 1, ptoc_Var107, ptoc_Var99) , ptoc_Var107) , &error , &err_char_no);
                        if ((int ) error == 0)
                            {
                            g03ftc_time((tsp00_MoveObj *) c64 , (tsp00_MoveObj *) ptoc_Var104->integer.C_1.mbp_buf , 1 , destpos + 1 , univallen , (unsigned char ) sql__setrang(ptoc_Var102->a_dt_format , 0 , 9) , 1 , &b_err);
                            }
                        else
                            {
                            a07_hex_uni_error(&(*acv) , (unsigned char ) sql__setrang(error , 0 , 8) , -(valpos + err_char_no) , 0 , (tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16209))[sql__setrang(valpos + err_char_no , 1 , 8192000) + -1] , (int ) ptoc_Var102->a_max_codewidth);
                            }
                        }
                    else
                        {
                        g02fromtermchar((unsigned char ) sql__setrang(ptoc_Var102->a_mess_code , 0 , 255) , sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16215) , valpos , iolen);
                        (*frec).fr_leng = (short ) sql__setrang((short ) s30lnr(sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16217) , (unsigned char)' ' , valpos + 1 , 8) , -32768 , 32767);
                        no_ebcdic_to_ascii = (char ) sql__setrang((int ) g01code.ctype == (int ) ptoc_Var102->a_basic_code || (int ) ptoc_Var102->a_basic_code != 1 , 0 , 1);
                        g03ftcheck_time(sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16222) , ptoc_Var104->integer.C_1.mbp_buf , valpos + 1 , destpos + 1 , (int ) (*frec).fr_leng , (unsigned char ) sql__setrang(ptoc_Var102->a_dt_format , 0 , 9) , (char ) sql__setrang(no_ebcdic_to_ascii , 0 , 1) , &b_err);
                        }
                    (*frec).fr_leng = 8;
                    break;
                case 13 :
                    ptoc_Var104->integer.C_1.mbp_buf[sql__setrang(destpos , 1 , 8192000) + -1] = (unsigned char)' ';
                    if ((ptoc_Var108 = (int ) (*(tsp1_packet *) sql__nil(ptoc_Var102->a_in_packet , "vak506.p" , 16232)).sp1_header.sp1h_mess_code) == 20 || ptoc_Var108 == 19)
                        {
                        (*frec).fr_leng = (short ) sql__setrang((short ) s30unilnr(ptoc_Var102->a_data_ptr , (*frec).fr_uni_fill_c2 , valpos + 1 , (int ) (*frec).fr_leng) , -32768 , 32767);
                        c64[0] = (unsigned char)' ';
                        c64_ptr = (tsp00_MoveObj *) &c64[1];
                        univallen = sizeof(unsigned char [64]) + -1;
                        s80uni_trans((tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16242))[sql__setrang(valpos + 1 , 1 , 8192000) + -1] , (int ) (*frec).fr_leng , (short ) sql__setrang((short ) (*(tsp1_packet *) sql__nil(ptoc_Var102->a_in_packet , "vak506.p" , 16243)).sp1_header.sp1h_mess_code , -32768 , 32767) , c64_ptr , &univallen , 0 , *(tsp8_uni_opt_set *) (_ptoc_MOVE(unsigned char , 1, ptoc_Var107, ptoc_Var100) , ptoc_Var107) , &error , &err_char_no);
                        c64_ptr = (tsp00_MoveObj *) c64;
                        if ((int ) error == 0)
                            {
                            a05timestamp_get(&(*acv) , sql__nil(c64_ptr , "vak506.p" , 16252) , 1 , ptoc_Var104->integer.C_1.mbp_buf , destpos + 1 , univallen , 1 , &b_err);
                            }
                        else
                            {
                            a07_hex_uni_error(&(*acv) , (unsigned char ) sql__setrang(error , 0 , 8) , -(valpos + err_char_no) , 0 , (tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16258))[sql__setrang(valpos + err_char_no , 1 , 8192000) + -1] , (int ) ptoc_Var102->a_max_codewidth);
                            }
                        }
                    else
                        {
                        g02fromtermchar((unsigned char ) sql__setrang(ptoc_Var102->a_mess_code , 0 , 255) , sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16265) , valpos , iolen);
                        (*frec).fr_leng = (short ) sql__setrang((short ) s30lnr(sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16266) , (unsigned char)' ' , valpos + 1 , 26) , -32768 , 32767);
                        no_ebcdic_to_ascii = (char ) sql__setrang((int ) g01code.ctype == (int ) ptoc_Var102->a_basic_code || (int ) ptoc_Var102->a_basic_code != 1 , 0 , 1);
                        a05timestamp_get(&(*acv) , sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16272) , valpos , ptoc_Var104->integer.C_1.mbp_buf , destpos + 1 , (int ) (*frec).fr_leng , (char ) sql__setrang(no_ebcdic_to_ascii , 0 , 1) , &b_err);
                        }
                    (*frec).fr_leng = 20;
                    break;
                default :
                    switch ((int ) ptoc_Var105->integer.C_1.cdatatyp)
                        {
                        case 2 :
                        case 31 :
                            ptoc_Var104->integer.C_1.mbp_buf[sql__setrang(destpos , 1 , 8192000) + -1] = (unsigned char)' ';
                            g02fromtermchar((unsigned char ) sql__setrang(ptoc_Var102->a_mess_code , 0 , 255) , sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16285) , valpos , iolen);
                            break;
                        case 3 :
                        case 32 :
                            ptoc_Var104->integer.C_1.mbp_buf[sql__setrang(destpos , 1 , 8192000) + -1] = (unsigned char)'@';
                            g02fromtermchar((unsigned char ) sql__setrang(ptoc_Var102->a_mess_code , 0 , 255) , sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16291) , valpos , iolen);
                            break;
                        default :
                            ptoc_Var104->integer.C_1.mbp_buf[sql__setrang(destpos , 1 , 8192000) + -1] = (unsigned char)'\0';
                            break;
                        }
                    if (((ptoc_Var108 = (int ) (*(tsp1_packet *) sql__nil(ptoc_Var102->a_in_packet , "vak506.p" , 16298)).sp1_header.sp1h_mess_code) == 20 || ptoc_Var108 == 19) && ((ptoc_Var108 = (int ) ptoc_Var105->integer.C_1.cdatatyp) == 2 || ptoc_Var108 == 3 || ptoc_Var108 == 31 || ptoc_Var108 == 32))
                        {
                        univallen = (int ) ptoc_Var105->integer.C_1.cdatalen;
                        s80uni_trans((tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16305))[sql__setrang(valpos + 1 , 1 , 8192000) + -1] , (int ) (*frec).fr_leng , (short ) sql__setrang((short ) (*(tsp1_packet *) sql__nil(ptoc_Var102->a_in_packet , "vak506.p" , 16306)).sp1_header.sp1h_mess_code , -32768 , 32767) , (tsp00_MoveObj *) &ptoc_Var104->integer.C_1.mbp_buf[sql__setrang(destpos + 1 , 1 , 8192000) + -1] , &univallen , 0 , *(tsp8_uni_opt_set *) (_ptoc_MOVE(unsigned char , 1, ptoc_Var107, ptoc_Var101) , ptoc_Var107) , &error , &err_char_no);
                        if ((int ) error == 0)
                            {
                            (*frec).fr_leng = (short ) sql__setrang((short ) univallen , -32768 , 32767);
                            if ((int ) ptoc_Var105->integer.C_1.cdatatyp == 3)
                                {
                                s30map(g02codetables.tables[0] , ptoc_Var104->integer.C_1.mbp_buf , destpos + 1 , ptoc_Var104->integer.C_1.mbp_buf , destpos + 1 , (int ) (*frec).fr_leng);
                                }
                            }
                        else
                            {
                            a07_hex_uni_error(&(*acv) , (unsigned char ) sql__setrang(error , 0 , 8) , -(valpos + err_char_no) , 0 , (tsp00_MoveObj *) &(*(tsp00_MoveObj *) sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16323))[sql__setrang(valpos + err_char_no , 1 , 8192000) + -1] , (int ) ptoc_Var102->a_max_codewidth);
                            }
                        }
                    else
                        {
                        if ((int ) ptoc_Var105->integer.C_1.cdatatyp == 3)
                            {
                            s30map(g02codetables.tables[0] , sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16332) , valpos + 1 , ptoc_Var104->integer.C_1.mbp_buf , destpos + 1 , (int ) (*frec).fr_leng);
                            }
                        else
                            {
                            g10mv("VAK506" , 34 , ptoc_Var102->a_data_length , ptoc_Var103->integer.C_2.mb_data_size , sql__nil(ptoc_Var102->a_data_ptr , "vak506.p" , 16337) , valpos + 1 , ptoc_Var104->integer.C_1.mbp_buf , destpos + 1 , (int ) (*frec).fr_leng , &(*(tsp1_segment *) sql__nil(ptoc_Var102->a_return_segm , "vak506.p" , 16339)).integer.C_4.sp1r_returncode);
                            }
                        }
                    break;
                }
            if ((int ) b_err != 0)
                {
                a07_b_put_error(&(*acv) , (short ) sql__setrang(b_err , -32768 , 32767) , -valpos);
                }
            if ((int ) ptoc_Var105->integer.C_1.ccolstack.integer.C_1.etype == 1 && !(char ) sql__setrang(truncated , 0 , 1) && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var102->a_return_segm , "vak506.p" , 16351)).integer.C_4.sp1r_returncode == 0 && (int ) ptoc_Var102->a_mblock.integer.C_2.mb_type == 39)
                {
                (*frec).fr_leng = (short ) sql__setrang((short ) s30lnr_defbyte((tsp00_MoveObj *) ptoc_Var104->integer.C_1.mbp_buf , (unsigned char ) sql__setrang(ptoc_Var104->integer.C_1.mbp_buf[sql__setrang(destpos , 1 , 8192000) + -1] , 0 , 255) , destpos + 1 , (int ) (*frec).fr_leng) , -32768 , 32767);
                }
            if ((char ) sql__setrang(*to_fill , 0 , 1) && (int ) (*(tsp1_segment *) sql__nil(ptoc_Var102->a_return_segm , "vak506.p" , 16363)).integer.C_4.sp1r_returncode == 0)
                {
                i = (int ) ptoc_Var105->integer.C_1.cinoutlen - (int ) (*frec).fr_leng + -1;
                t01int4(5 , "fill_length " , i);
                if (i > 0)
                    {
                    switch ((int ) ptoc_Var104->integer.C_1.mbp_buf[sql__setrang(destpos , 1 , 8192000) + -1])
                        {
                        case 32 :
                            g10fil("VAK506" , 35 , ptoc_Var103->integer.C_2.mb_data_size , ptoc_Var104->integer.C_1.mbp_buf , destpos + (int ) (*frec).fr_leng + 1 , i , (unsigned char)' ' , &(*(tsp1_segment *) sql__nil(ptoc_Var102->a_return_segm , "vak506.p" , 16375)).integer.C_4.sp1r_returncode);
                            break;
                        case 64 :
                            g10fil("VAK506" , 36 , ptoc_Var103->integer.C_2.mb_data_size , ptoc_Var104->integer.C_1.mbp_buf , destpos + (int ) (*frec).fr_leng + 1 , i , (unsigned char)'@' , &(*(tsp1_segment *) sql__nil(ptoc_Var102->a_return_segm , "vak506.p" , 16380)).integer.C_4.sp1r_returncode);
                            break;
                        case 1 :
                            g10filuni("VAK506" , 37 , ptoc_Var103->integer.C_2.mb_data_size , ptoc_Var104->integer.C_1.mbp_buf , destpos + (int ) (*frec).fr_leng + 1 , i , "\0 " , &(*(tsp1_segment *) sql__nil(ptoc_Var102->a_return_segm , "vak506.p" , 16385)).integer.C_4.sp1r_returncode);
                            break;
                        default :
                            g10fil("VAK506" , 38 , ptoc_Var103->integer.C_2.mb_data_size , ptoc_Var104->integer.C_1.mbp_buf , destpos + (int ) (*frec).fr_leng + 1 , i , (unsigned char)'\0' , &(*(tsp1_segment *) sql__nil(ptoc_Var102->a_return_segm , "vak506.p" , 16390)).integer.C_4.sp1r_returncode);
                            break;
                        }
                    }
                (*frec).fr_leng = (short ) sql__setrang((short ) sql__pred((int ) ptoc_Var105->integer.C_1.cinoutlen , -32768 , 32767 , "vak506.p" , 16395) , -32768 , 32767);
                }
            }
        }
    (*frec).fr_leng = (short ) sql__setrang((short ) sql__succ((int ) (*frec).fr_leng , -32768 , 32767 , "vak506.p" , 16401) , -32768 , 32767);
    cccprint("<ak506val_from_p");
    }
