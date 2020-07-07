
/* some common prolog for every module
 */

#ifndef GGG01_H
#define GGG01_H


/******************************************
 *            I N C L U D E S             *
 ******************************************/


#ifndef GSP00_2_H
#include "gsp00_2.h"
#endif

#ifndef GSP00_H
#include "gsp00.h"
#endif

#ifndef GGG00_H
#include "ggg00.h"
#endif

/******************************************
 *           C O N S T A N T S            *
 ******************************************/

#define MAX_STREAM_DESC_gg01                   16
    /*- begin C++ SQL-RTE errors (basis error      = SQLCode) -*/
    /*-      Range from -27000 to -28999                 -*/
#define e_unknown_error                (- 27000)
#define e_inp_null_not_allowed         (- 27001)
#define e_inp_number_invalid           (- 27002)
#define e_inp_number_overflow          (- 27003)
#define e_param_list_too_long          (- 27004)
#define e_param_list_too_short         (- 27005)
#define e_incompatible_datatype        (- 27006)
#define e_too_many_hostvar             (- 27007)
#define e_missing_variable_addr        (- 27008)
#define e_cmd_too_long                 (- 27009)
#define e_copyop_not_allowed           (- 27010)
#define e_inp_string_truncated         (- 27011)
#define e_use_callstm                  (- 27012)
#define e_cmd_not_available            (- 27013)
    /* error codes -27014 to -27018 defined for externally used SQL class gco519CErrorHandling.h */
#define e_uni_to_ascii_conv_failed     (- 27019)    /* PTS 1116580 FF 2002-07-22 */
#define e_no_sqldbc_connection         (- 27020)    /* PTS 1128823 */
#define e_new_failed                   (- 28000)
#define e_nil_pointer                  (- 28001)
#define e_invalid_oid                  (- 28002)
#define e_unknown_guid                 (- 28003)
#define e_duplicate_object             (- 28005)
#define e_object_not_locked            (- 28006)
#define e_incompatible_oid             (- 28007)
#define e_var_obj_overwritten          (- 28008)
#define e_oms_istream_error            (- 28501)
#define e_oms_ostream_error            (- 28502)
#define e_com_object_crashed           (- 28503)
#define e_no_coclass_obj_found         (- 28505)
#define e_unsupported_param_type       (- 28508)
#define e_progid_not_found             (- 28509)
#define e_inprocserver_not_found       (- 28511)
#define e_localserver_not_found        (- 28512)
#define e_method_not_found             (- 28513)
#define e_unknown_version              (- 28514)    /* ADIS 1001807, T.A. 15.12.1998 */
#define e_version_already_open         (- 28515)
#define e_transaction_end_required     (- 28516)
#define e_already_in_critical_section  (- 28517)    /* ADIS CR 1000225, T.A. 16.02.1998 */
#define e_not_in_critical_section      (- 28518)    /*                                  */
#define e_already_in_version           (- 28521)
#define e_missing_before_image         (- 28522)
#define e_unknown_schema               (- 28523)
#define e_duplicate_schema             (- 28524)
#define e_proc_address_not_found       (- 28525)
#define e_directory_not_found          (- 28527)
#define e_lib_not_released             (- 28528)
#define e_buffer_too_small             (- 28529)
#define e_terminate                    (- 28530)
#define e_oms_read_only                (- 28531)    /* PTS 1112358 */
#define e_version_bound_by_trans       (- 28532)    /* PTS 1115023 */
#define e_packed_out_of_range          (- 28533)    /* PTS 1115372, T.A. */
#define e_package_out_of_date          (- 28534)
#define e_longpacked_out_of_range      (- 28535)    /* PTS 1122649 */
#define e_no_keyed_object              (- 28537)    /* PTS 1124070 */
#define e_wrong_key                    (- 28538)    /* PTS 1124070 */
#define e_invalid_rowsize              (- 28539)    /* PTS 1125169 */
#define e_invalid_csid                 (- 28540)    /* PTS 1125169 */
#define e_param_check                  (- 28541)
#define e_unexpected_abap_type         (- 28542)    /* PTS 1130281 */
#define e_reset_istream_not_supported  (- 28543)    /* PTS 1131114, PG */
#define e_reset_ostream_not_supported  (- 28544)    /* PTS 1131114, PG */
#define e_container_already_exists     (- 28545)    /* PTS 1135739, T.A. */
#define e_internal_stream_error        (- 28546)    /* PTS 1135058, PG */
#define e_wrong_class_entry_ptr        (- 28547)    /* PTS 1137922, MS */
#define e_internal_obj_link_corruption (- 28548)    /* PTS 1140189, RS */
    /*      ProcServer errors, range -28600 to -28699 */
#define e_dbproc_invalid_packet        (- 28600)
#define e_dbproc_no_packet             (- 28601)
#define e_dbproc_packet_size           (- 28602)
#define e_dbproc_packet_too_big        (- 28603)
#define e_dbproc_too_many_packets      (- 28604)
#define e_dbproc_invalid_operation     (- 28605)
#define e_dbproc_comm_error            (- 28606)
#define e_dbproc_packet_missing        (- 28607)
#define e_dbproc_invalid_reply         (- 28608)
#define e_dbproc_proc_not_found        (- 28609)
    /*      C++ object kernel errors  Range from -28800 to -28899 */
#define e_wrong_class_id               (- 28801)
#define e_wrong_object_version         (- 28802)
#define e_wrong_object_state           (- 28803)
#define e_too_old_oid                  (- 28804)
#define e_nil_page_pointer             (- 28806)
#define e_no_next_object               (- 28807)
#define e_hash_not_found               (- 28808)
#define e_invalid_obj_col_desc         (- 28809)
#define e_duplicate_hash_key           (- 28810)
#define e_hash_key_not_found           (- 28811)
#define e_invalid_iterator             (- 28812)
#define e_unknown_consist_vers         (- 28813)
#define e_object_not_found             (- 28814)
#define e_obj_history_not_found        (- 28815)
#define e_hash_memory_exceeded         (- 28816)
#define e_illegal_object_length        (- 28817)
#define e_illegal_object_pos           (- 28818)
#define e_object_dirty                 (- 28819)
#define e_hash_key_in_use              (- 28820)
#define e_wrong_obj_file_id            (- 28822)
#define e_page_no_in_use               (- 28823)
#define e_wrong_all_page_count         (- 28826)
#define e_inconsistent_drop_info       (- 28828)
#define e_illegal_cont_obj             (- 28830)    /* internal use, see vbd04.cpp */
#define e_wrong_obj_count              (- 28831)
#define e_container_dropped            (- 28832)    /* PTS 1119647 internal use, see vbd04.cpp */
#define e_consview_cancelled           (- 28833)
#define e_primkey_does_not_match       (- 28834)    /* PTS 1131963 internal use, see vbd22.cpp */
#define e_key_still_visible            (- 28835)    /* PTS 1126812 ff 2004-01-09 internal use, mapped by liboms */
#define e_recreate_index_failed        (- 28836)    /* PTS 1134856 FF */
#define e_case_not_found               (- 28901)
#define e_too_many_catch_levels        (- 28902)
#define e_missing_return               (- 28903)
#define e_dbfunc_with_sql_not_allowed  (- 28904)
#define e_dbfunc_index_error           (- 28905)
#define e_dbfunc_code_not_found        (- 28906)
#define e_dbfunc_stop_msg              (- 28907)
#define e_function_not_deterministic   (- 28908)
#define e_stop_0                       (- 28910)
#define e_missing_object_flush         (- 28950)
#define e_invalid_deallocate           (- 28951)
#define e_requested_dbperror           (- 28999)    /* PTS 1112358 */

/******************************************
 *               T Y P E S                *
 ******************************************/

typedef tgg00_FileId               tgg01_ContainerId;

typedef struct                     tgg01_OmsVersionContext
{
    tgg00_FileId        ovc_tree;
    tgg00_FileId        ovc_index_tree;
    tsp00_Int4          ovc_key;
    tgg91_TransNo       ovc_trans_version;
} tgg01_OmsVersionContext;

typedef struct                     tgg01_OmsLockInfo
{
    tsp00_C8            oli_handle;
    tsp00_C18           oli_lockmode;
    tsp00_C18           oli_requestmode;
    tsp00_TaskId        oli_taskid;
    tsp00_Int4          oli_timeout;
} tgg01_OmsLockInfo;

typedef enum tgg01_COMMonEnum_Enum
{
    cmiRunTime,
    cmiRunTimeMin,
    cmiRunTimeMax,
    cmiOmsRead,
    cmiLCRead,
    cmiLCKeyRead,
    cmiOmsStore,
    cmiLCStore,
    cmiOmsDelete,
    cmiLCDelete,
    cmiLCLock,
    cmiLCIter,
    cmiLogHop,
    cmiLogHopIter,
    cmiExceptions,
    cmiOutOfDate,
    cmiTimeout,
    cmiOutOfMemory,
    cmiSubtransRollback,
    cmiSubtransCommit,
    cmiMaxSubtransLevel,
    cmiLoadVarObj,
    cmiStoreVarObj,
    cmiSumVarObjSize,
    cmiCntVarObj,
    cmiCreateVersion,
    cmiOpenVersion,
    cmiCloseVersion,
    cmiDropVersion,
    cmiStoreInVersion,
    cmiDeleteInVersion,
    cmiLCReadInVersion,
    cmiLCKeyReadInVersion,
    cmiDeleteVarObject,
    cmiMaxWaitNewConsView,
    cmiMinWaitNewConsView,
    cmiSumWaitNewConsView,
    cmiCntWaitNewConsView,
    cmiMallocMin,
    cmiMallocMax,
    cmiMallocSum,
    cmiMallocCnt,
    cmiFreeMin,
    cmiFreeMax,
    cmiFreeSum,
    cmiFreeCnt,
    cmiDeltaMin,
    cmiDeltaMax,
    cmiDeltaAvg,
    cmiOmsTerminate,
    cmiMinReadApplCallbacks,
    cmiMaxReadApplCallbacks,
    cmiSumReadApplCallbacks,
    cmiMinStreamRowsRead,
    cmiMaxStreamRowsRead,
    cmiSumStreamRowsRead,
    cmiMinWriteApplCallbacks,
    cmiMaxWriteApplCallbacks,
    cmiSumWriteApplCallbacks,
    cmiMinStreamRowsWrite,
    cmiMaxStreamRowsWrite,
    cmiSumStreamRowsWrite,
    cmiOmsReadCacheHit,
    cmiOmsReadCacheMiss,
    cmiOmsReadVersion,
    cmiOmsMaxHashChainLen,
    cmiOmsRehash,
    cmiCntWaitOmsLockObj,
    cmiWaitOmsLockObjSecs,
    cmiHashSumChainAccessed,
    cmiHashDerefCount,
    cmiOmsReleaseCalled,
    cmiOmsReleaseDone,
    cmiCacheSize
} tgg01_COMMonEnum_Enum;
typedef tgg01_COMMonEnum_Enum tgg01_COMMonEnum_Param;
typedef ENUM_TYPE_MSP00 (          tgg01_COMMonEnum, tgg01_COMMonEnum_Enum, cmiCacheSize, unsigned char);

typedef struct                     tgg01_COMMonitorInfo
{
    tsp00_KnlIdentifier cmiMethodName_gg00;
    tsp00_8ByteCounter  cmiCallCnt_gg00;
    ARRAY_VAR_MSP00 (         cmiCounters_gg00, tsp00_8ByteCounter, cmiCacheSize - 0 + 1);
} tgg01_COMMonitorInfo;

typedef struct                     tgg01_COMMonitorTransferBuf
{
    tgg00_MessType      ctbMessType_gg00;
    tsp00_Int4          ctbDispId_gg00;
    tgg01_COMMonitorInfo ctbMonInfo_gg00;
} tgg01_COMMonitorTransferBuf;

typedef struct                     tgg01_OneStreamDescriptor
{
    tgg00_VoidPtr       osdHandle;
    tgg00_VoidPtr       osdBufPtr;
    tgg00_VoidPtr       osdHiddenDesc;
    tsp00_Int4          osdStreamId;
    tsp00_Int4          osdRowCount;
    tsp00_Int4          osdRowSize;
    tsp00_Int4          osdBufSize;
    tsp00_Int4          osdHiddenDescSize;
    pasbool             osdEndOfStream;
    pasbool             osdResetStream;
    pasbool             osdDefaultCompression;
} tgg01_OneStreamDescriptor;

typedef struct                     tgg01_StreamDescriptor
{
    tsp00_Int4          sdCount;
    pasbool             sdFiller0;
    pasbool             sdFiller1;
    tsp00_Int2          sdFiller2;
    ARRAY_VAR_MSP00 (         sdDesc, tgg01_OneStreamDescriptor, MAX_STREAM_DESC_gg01);
} tgg01_StreamDescriptor;

struct tgg01_TemplateDummy {
    tgg01_COMMonEnum     dummy_tgg01_COMMonEnum;
};


#endif
