
/* some common prolog for every module
 */

#ifndef GBD00_H
#define GBD00_H


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

#define ERRTEXTREC_MXBD00                      56
#define FDIRENTRY_MXBD00                       44
#define NODE_HEADER_MXBD00                     80
#define STD_FDIRENTRY_MXBD00                   36
    /* */
#define REC_ALIGN_BD00                          2
    /* */
    /* --- bufferhandling --- */
    /* */
#define MIN_RECORD_LEN_BD00            (cgg_rec_key_offset + 1)    /* define byte */
    /* */
    /* --- nodehandling --- */
    /* */
#define BODY_BEG_BD00                  (NODE_HEADER_MXBD00 + 1)
#define BODY_END_BD00                  (PAGE_MXSP00 - PAGE_CHECK_TRAILER_MXGG00)
#define MAX_BOTTOM_BD00                (BODY_END_BD00 + 1)
#define FIRST_REC_INDEX_BD00                    0
    /* */
#define LEAF_LEVEL_BD00                         0
#define FIRST_INDEX_LEVEL_BD00                  1
#define SECOND_INDEX_LEVEL_BD00                 2
#define THIRD_INDEX_LEVEL_BD00                  3
#define MAX_LEVEL_BD00                          5
#define MAX_BSTR_LEVEL_BD00                     2
    /* */
#define FULLCOVERING_BD00              (PAGE_MXSP00 - NODE_HEADER_MXBD00 - PAGE_CHECK_TRAILER_MXGG00)
#define MAX_BSTR_COVERING_BD00         FULLCOVERING_BD00
#define HALF_COVERING_BD00             (FULLCOVERING_BD00 / 2)
#define QUATER_COVERING_BD00           (FULLCOVERING_BD00 / 4)
#define NINETY_PERCENT_COV_BD00        (( FULLCOVERING_BD00 * 90 ) / 100)
    /* */
#define MAX_INVLISTLENGTH_BD00         (( FULLCOVERING_BD00 / 4 ) * 3)
#define POINTERSIZE_BD00               INT4_MXSP00
#define DIVISOR_POINTERSIZE_BD00       (POINTERSIZE_BD00 / 2)    /* Shift Value For Pointersize Division */
#define MAX_POINTERINDEX_BD00          (FULLCOVERING_BD00 / POINTERSIZE_BD00)
#define PNOS_PER_PAGE_BD00             (FULLCOVERING_BD00 / INT4_MXSP00)
    /* */
    /* --- miscellaneous --- */
    /* */
#define LOCK_TREE_EXCL_BD00            true
#define LOCK_LEAF_EXCL_BD00            true
#define SYNC_TREE_LOCK_BD00            true
    /* */
#define FILE_EXT_COR_BD00               ".cor"
    /* */
#define MAX_NODE_SCAN_BD00                      5
#define LWB_OF_LIST_BD00                        1
#define UPB_OF_LIST_BD00                        6
#define MAX_OBJ_GARB_COLL_BD00                 64
#define MAX_VF_BUFFERS_BD00                   128    /* see cserv.pcf */
#define MAX_FREE_QUEUE_BD00                     5
#define cbd_cspmaxint4_mxspbuf         (MAX_INT4_SP00 - PAGE_MXSP00)
    /* cbd_cspmaxint4_mxspbuf  = 2147479551; */
    /* */
#define NIL_RECINDEX_BD00              (- MAX_INT2_SP00)    /* must be < -1 */
    /* */
#define MIN_LOCK_REQUEST_SEQ_NO_BD00            0
#define POS_OFF_DIFF_BD00                       1    /* difference between position and offset, */
    /*                                 i.e. pascal and c arrays                */

/******************************************
 *               T Y P E S                *
 ******************************************/


typedef enum tbd00_DelInvReason_Enum
{
    dirSetDeletedFlag_ebd00,
    dirRemoveDeletedFlag_ebd00,
    dirGarbageCollection_ebd00,
    dirRollback_ebd00,
    dirDeletePrimKey_ebd00
} tbd00_DelInvReason_Enum;
typedef tbd00_DelInvReason_Enum tbd00_DelInvReason_Param;
typedef ENUM_TYPE_MSP00 (          tbd00_DelInvReason, tbd00_DelInvReason_Enum, dirDeletePrimKey_ebd00, unsigned char);

typedef enum tbd00_PageLockMode_Enum
{
    plmLock_ebd00,
    plmLockWithSVPSync_ebd00,
    plmLockWithSVPSyncNoWait_ebd00,
    plmLockWithoutReschedule_ebd00
} tbd00_PageLockMode_Enum;
typedef tbd00_PageLockMode_Enum tbd00_PageLockMode_Param;
typedef ENUM_TYPE_MSP00 (          tbd00_PageLockMode, tbd00_PageLockMode_Enum, plmLockWithoutReschedule_ebd00, unsigned char);
    /* */

typedef enum tbd_node_request_Enum
{
    nr_for_read,
    nr_for_update
} tbd_node_request_Enum;
typedef tbd_node_request_Enum tbd_node_request_Param;
typedef ENUM_TYPE_MSP00 (          tbd_node_request, tbd_node_request_Enum, nr_for_update, unsigned char);
    /* */

typedef enum tbd_lru_info_Enum
{
    lru_normal,
    lru_mid,
    lru_last
} tbd_lru_info_Enum;
typedef tbd_lru_info_Enum tbd_lru_info_Param;
typedef ENUM_TYPE_MSP00 (          tbd_lru_info, tbd_lru_info_Enum, lru_last, unsigned char);
    /* */

typedef enum tbd_file_state_set__Enum
{
    f_unused1_ebd00,
    f_bad,
    f_read_only,
    f_not_accessible,
    f_unused2_ebd00,
    f_unused3_ebd00
} tbd_file_state_set__Enum;
typedef tbd_file_state_set__Enum tbd_file_state_set__Param;
typedef SMALLSET_TYPE_MSP00 (      tbd_file_state_set, tbd_file_state_set__Enum, f_unused3_ebd00);
    /* */

typedef enum tbd_objpage_state_set__Enum
{
    objp_no_chain_to_freelist
} tbd_objpage_state_set__Enum;
typedef tbd_objpage_state_set__Enum tbd_objpage_state_set__Param;
typedef SMALLSET_TYPE_MSP00 (      tbd_objpage_state_set, tbd_objpage_state_set__Enum, objp_no_chain_to_freelist);
    /* */

typedef enum tbd_fileinfo_desc_set__Enum
{
    fid_fill0,
    fid_fill1,
    fid_fill2,
    fid_fill3,
    fid_fill4,
    fid_fill5,
    fid_fill6,
    fid_fill7
} tbd_fileinfo_desc_set__Enum;
typedef tbd_fileinfo_desc_set__Enum tbd_fileinfo_desc_set__Param;
typedef SMALLSET_TYPE_MSP00 (      tbd_fileinfo_desc_set, tbd_fileinfo_desc_set__Enum, fid_fill7);
    /* */

typedef struct                     tbd_fileinfo
{
    tgg00_FiletypeSet   fi_type;
    tbd_file_state_set  fi_state;
    pasbool             fi_fill1;
    tbd_fileinfo_desc_set fi_descr;
    tsp00_PageNo        fi_root;
    tgg91_FileVersion   fi_vers;
    tsp00_Int2          fi_col_cnt;
    tgg00_UserRef       fi_user_ref;
    tsp00_Int2          fi_var_col_cnt_pos;
} tbd_fileinfo;

typedef struct                     tbd00_FDirEntry
{
    union
    {
    /* 1 */
        struct tbd00_FDirEntry_1
        {
            OLDPASCALSTRING_VAR_MSP00 (  fdirBuf_bd00_F, FDIRENTRY_MXBD00);
        } C_1;
    /* 2 */
        struct tbd00_FDirEntry_2
        {
            tsp00_Int2          fdirEntryLength_bd00_F;
            tsp00_Int2          fdirFileNameLength_bd00_F;
            tsp00_Int2          fdirFill1_bd00_F;
            tsp00_Int2          fdirFill2_bd00_F;
            tgg00_Filename      fdirFileName_bd00_F;
            tbd_fileinfo        fdirFileInfo_bd00_F;
        } C_2;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_OldPascalString<FDIRENTRY_MXBD00> &fdirBuf_bd00           ()  { return this->variant.C_1.fdirBuf_bd00_F; };
    tsp00_Int2         &fdirEntryLength_bd00   ()  { return this->variant.C_2.fdirEntryLength_bd00_F; };
    tsp00_Int2         &fdirFileNameLength_bd00 ()  { return this->variant.C_2.fdirFileNameLength_bd00_F; };
    tsp00_Int2         &fdirFill1_bd00         ()  { return this->variant.C_2.fdirFill1_bd00_F; };
    tsp00_Int2         &fdirFill2_bd00         ()  { return this->variant.C_2.fdirFill2_bd00_F; };
    tgg00_Filename     &fdirFileName_bd00      ()  { return this->variant.C_2.fdirFileName_bd00_F; };
    tbd_fileinfo       &fdirFileInfo_bd00      ()  { return this->variant.C_2.fdirFileInfo_bd00_F; };
    /* const accessor functions */
    const tsp00_OldPascalString<FDIRENTRY_MXBD00> &fdirBuf_bd00           () const  { return this->variant.C_1.fdirBuf_bd00_F; };
    const tsp00_Int2   &fdirEntryLength_bd00   () const  { return this->variant.C_2.fdirEntryLength_bd00_F; };
    const tsp00_Int2   &fdirFileNameLength_bd00 () const  { return this->variant.C_2.fdirFileNameLength_bd00_F; };
    const tsp00_Int2   &fdirFill1_bd00         () const  { return this->variant.C_2.fdirFill1_bd00_F; };
    const tsp00_Int2   &fdirFill2_bd00         () const  { return this->variant.C_2.fdirFill2_bd00_F; };
    const tgg00_Filename &fdirFileName_bd00      () const  { return this->variant.C_2.fdirFileName_bd00_F; };
    const tbd_fileinfo &fdirFileInfo_bd00      () const  { return this->variant.C_2.fdirFileInfo_bd00_F; };
#endif    /* defined (__cplusplus) */
} tbd00_FDirEntry;

typedef struct                     tbd00_FreeQueue
{
    ARRAY_VAR_MSP00 (         fqPnos_bd00, tsp00_PageNo, MAX_FREE_QUEUE_BD00);
    ARRAY_VAR_MSP00 (         fqPageVersion_bd00, tgg00_ConverterVersion, MAX_FREE_QUEUE_BD00);
    tsp00_Int4          fqTop_bd00;
} tbd00_FreeQueue;
typedef tsp00_Int4                 tbd00_LockRequestSeqNo;
    /* */
typedef ARRAY_TYPE_MSP00 (         tbd_position_list, tgg00_PagePos, MAX_BSTR_LEVEL_BD00);
    /* */
typedef OLDPASCALSTRING_TYPE_MSP00 (  tbd_node_header, NODE_HEADER_MXBD00);
typedef CSTRING_TYPE_MSP00      (  tbd_node_headerc, NODE_HEADER_MXBD00);
    /* */
typedef OLDPASCALSTRING_TYPE_MSP00 (  tbd_node_body, BODY_END_BD00 - BODY_BEG_BD00 + 1);
typedef CSTRING_TYPE_MSP00      (  tbd_node_bodyc, BODY_END_BD00 - BODY_BEG_BD00 + 1);
    /* */
typedef ARRAY_TYPE_MSP00 (         tbd_pointer_list, tgg00_PagePos, MAX_POINTERINDEX_BD00);
    /* */

typedef enum tbd00_ConsistencyInfo__Enum
{
    c_historyInPagingFile_ebd00,
    c_historyOnPage_ebd00
} tbd00_ConsistencyInfo__Enum;
typedef tbd00_ConsistencyInfo__Enum tbd00_ConsistencyInfo__Param;
typedef SMALLSET_TYPE_MSP00 (      tbd00_ConsistencyInfo, tbd00_ConsistencyInfo__Enum, c_historyOnPage_ebd00);

typedef struct                     tbd00_PageFixSizeSpace
{
    tsp00_C50           filler1;
    tsp00_Int2          fixRecordLength;
    tsp00_Int4          filler2;
    tsp00_C24           filler3;
} tbd00_PageFixSizeSpace;

typedef struct                     tbd00_RedoPage
{
    tsp00_C42           filler1;
    tsp00_Int2          filler2;
    tsp00_Int4          firstEntrySequence;
    tsp00_Int4          filler3;
    tsp00_Int4          lastEntrySequence;
    tsp00_C8            filler4;
    tgg91_TransNo       transno;
    tsp00_C10           filler5;
} tbd00_RedoPage;

typedef struct                     tbd00_UndoPage
{
    tsp00_C50           filler1;
    tsp00_Int2          filler2;
    tsp00_Int4          lastEntrySequence;
    tsp00_Int4          pagecount;
    pasbool             relevantForGC;
    pasbool             filler3;
    tsp00_Int2          filler4;
    tgg91_TransNo       transno;
    tsp00_Int2          lastGC_offset;
    tsp00_PageNo        lastGC_pageno;
    tsp00_Int4          filler5;
} tbd00_UndoPage;

typedef struct                     tbd00_OpenTransPage
{
    tsp00_C18           filler1;
    pasbool             filler2;
    pasbool             isCreatedDuringSavepoint;
    tsp00_C32           filler5;
    tsp00_Int4          oldestEOTSequence;
    tsp00_C16           filler7;
    tsp00_Int4          savepointSequence;
    tsp00_Int4          filler8;
} tbd00_OpenTransPage;

typedef struct                     tbd_node
{
    union
    {
    /* 1 */
        struct tbd_node_1
        {
            tgg00_PageHeader    nd_header_F;
        } C_1;
    /* 2 */
        struct tbd_node_2
        {
            tbd_node_header     nd_full_header_F;
            tbd_node_body       nd_body_F;
            tgg00_PageCheckTrailer nd_full_trailer_F;
        } C_2;
    /* 3 */
        struct tbd_node_3
        {
            tsp00_PageNo        nd_id_F;
            tgg00_PageType      nd_pt_F;
            tgg00_PageType2     nd_pt2_F;
            tgg00_PageCheckType nd_checktype_F;
            tgg00_PageMode      nd_pmode_F;
            tgg00_PagePos       nd_bottom_F;
            tsp00_Int4          nd_record_cnt_F;
            tsp00_Int2          nd_level_F;
            tbd_file_state_set  nd_file_state_F;
            pasbool             nd_sorted_F;
            tsp00_PageNo        nd_root_F;
            tsp00_PageNo        nd_right_F;
            tsp00_PageNo        nd_left_F;
            tsp00_PageNo        nd_last_F;
            tsp00_Int4          nd_filler2_F;
            tgg00_ConverterVersion nd_conv_version_F;
            tsp00_Int4          nd_str_version_F;
            tgg91_FileVersion   nd_file_version_F;
            tbd00_ConsistencyInfo nd_history_info_F;
            tsp00_Uint1         ndPageVersion_bd00_F;
            tsp00_Int4          ndInvUsageCnt_bd00_F;
            tsp00_Int4          ndLeafCount_bd00_F;
            tsp00_Int4          ndTreeLeaves_bd00_F;
            tgg91_TransNo       nd_trans_id_F;
            tsp00_Int2          nd_filler4_F;
            tsp00_PageNo        ndInvRoot_bd00_F;
            tsp00_Int4          nd_write_cnt_F;
            tbd_pointer_list    nd_pointer_list_F;
            tsp00_Int4          nd_checksum_F;
            tgg00_PageTrailer   nd_trailer_F;
        } C_3;
    /* 4 */
        struct tbd_node_4
        {
            tsp00_C8            nd_header_space_F;
            tsp00_Uint2         nd_max_obj_cnt_F;
            tsp00_Uint2         nd_occ_obj_cnt_F;
            tsp00_Uint2         nd_free_eot_obj_cnt_F;
            tsp00_Uint2         nd_free_obj_cnt_F;
            tsp00_Int2          nd_level_space_F;
            tbd_file_state_set  nd_file_state_space_F;
            tbd_objpage_state_set nd_objpage_state_F;
            tsp00_PageNo        nd_root_space_F;
            tsp00_PageNo        nd_right_space_F;
            tsp00_PageNo        nd_left_space_F;
            tsp00_Int2          nd_filler5_2_F;
            pasbool             nd_in_free_chain_F;
            tsp00_Uint1         nd_num_key_files_F;
            tbd00_LockRequestSeqNo ndLockRequestSeqNoLow_bd00_F;
            tgg00_ConverterVersion nd_conv_vers_space_F;
            tsp00_Uint2         nd_obj_frame_len_F;
            tsp00_Uint2         nd_obj_key_len_F;
            tgg91_FileVersion   nd_file_vers_space_F;
            tgg00_ObjPagePos    nd_first_free_obj_F;
            tgg00_ObjPagePos    nd_first_obj_F;
            tsp00_Int2          nd_filler5_4_F;
            tsp00_C4            nd_obj_file_no1_F;
            tbd00_LockRequestSeqNo ndLockRequestSeqNoHigh_bd00_F;
            tsp00_PageNo        nd_next_free_F;
            tsp00_PageNo        nd_prev_free_F;
            tsp00_C4            nd_obj_file_no2_F;
            tsp00_Int4          nd_write_cnt_space_F;
        } C_4;
    /* 5 */
        struct tbd_node_5
        {
            tsp00_C8            ndStrHeaderSpace_bd00_F;
            tgg00_PagePos       ndStrBottomSpace_bd00_F;
            tsp00_Int4          ndStrFiller1_bd00_F;
            tsp00_Int2          ndStrLevelSpace_bd00_F;
            tbd_file_state_set  ndStrFileStateSpace_bd00_F;
            pasbool             ndStrFiller2_bd00_F;
            tsp00_PageNo        ndStrRootSpace_bd00_F;
            tsp00_PageNo        ndStrRightSpace_bd00_F;
            tsp00_PageNo        ndStrLeftSpace_bd00_F;
            tgg00_Surrogate     ndStrParentID_bd00_F;
            tgg00_ConverterVersion ndStrConvVersionSpace_bd00_F;
            tsp00_Int4          ndStrVersion_bd00_F;
            tgg91_FileVersion   ndStrFileVersionSpace_bd00_F;
            tsp00_C6            ndStrFiller4_bd00_F;
            tsp00_Int4          ndStrFileSize_bd00_F;
            tsp00_Int4          ndStrFiller5_bd00_F;
            tsp00_C8            ndStrFiller6_bd00_F;
            tsp00_Int4          ndStrFiller7_bd00_F;
            tsp00_Int4          ndStrWriteCntSpace_bd00_F;
        } C_5;
    /* 7 */
        struct tbd_node_7
        {
            tsp00_C8            ndHeaderSpace_bd00_F;
            tgg00_PagePos       ndFirstOffset_bd00_F;
            tsp00_Int4          ndFieldCount_bd00_F;
            tsp00_Int2          nd_level_space2_F;
            tbd_file_state_set  nd_file_state_space2_F;
            pasbool             nd_sorted_space2_F;
            tsp00_PageNo        nd_root_space2_F;
            tsp00_PageNo        nd_right_space2_F;
            tsp00_PageNo        nd_left_space2_F;
            tgg00_PagePos       ndFirstFreeOffset_bd00_F;
            tsp00_Int4          ndAllFieldInQueueCount_bd00_F;
        } C_7;
    /* 8 */
        struct tbd_node_8
        {
            tbd00_PageFixSizeSpace ndPageFixSizeSpace_bd00_F;
        } C_8;
    /* 9 */
        struct tbd_node_9
        {
            tbd00_UndoPage      ndUndoPage_bd00_F;
        } C_9;
    /* 10 */
        struct tbd_node_10
        {
            tbd00_RedoPage      ndRedoPage_bd00_F;
        } C_10;
    /* 11 */
        struct tbd_node_11
        {
            tbd00_OpenTransPage ndOpenTransPage_bd00_F;
        } C_11;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tgg00_PageHeader   &nd_header              ()  { return this->variant.C_1.nd_header_F; };
    tbd_node_header    &nd_full_header         ()  { return this->variant.C_2.nd_full_header_F; };
    tbd_node_body      &nd_body                ()  { return this->variant.C_2.nd_body_F; };
    tgg00_PageCheckTrailer &nd_full_trailer        ()  { return this->variant.C_2.nd_full_trailer_F; };
    tsp00_PageNo       &nd_id                  ()  { return this->variant.C_3.nd_id_F; };
    tgg00_PageType     &nd_pt                  ()  { return this->variant.C_3.nd_pt_F; };
    tgg00_PageType2    &nd_pt2                 ()  { return this->variant.C_3.nd_pt2_F; };
    tgg00_PageCheckType &nd_checktype           ()  { return this->variant.C_3.nd_checktype_F; };
    tgg00_PageMode     &nd_pmode               ()  { return this->variant.C_3.nd_pmode_F; };
    tgg00_PagePos      &nd_bottom              ()  { return this->variant.C_3.nd_bottom_F; };
    tsp00_Int4         &nd_record_cnt          ()  { return this->variant.C_3.nd_record_cnt_F; };
    tsp00_Int2         &nd_level               ()  { return this->variant.C_3.nd_level_F; };
    tbd_file_state_set &nd_file_state          ()  { return this->variant.C_3.nd_file_state_F; };
    pasbool            &nd_sorted              ()  { return this->variant.C_3.nd_sorted_F; };
    tsp00_PageNo       &nd_root                ()  { return this->variant.C_3.nd_root_F; };
    tsp00_PageNo       &nd_right               ()  { return this->variant.C_3.nd_right_F; };
    tsp00_PageNo       &nd_left                ()  { return this->variant.C_3.nd_left_F; };
    tsp00_PageNo       &nd_last                ()  { return this->variant.C_3.nd_last_F; };
    tsp00_Int4         &nd_filler2             ()  { return this->variant.C_3.nd_filler2_F; };
    tgg00_ConverterVersion &nd_conv_version        ()  { return this->variant.C_3.nd_conv_version_F; };
    tsp00_Int4         &nd_str_version         ()  { return this->variant.C_3.nd_str_version_F; };
    tgg91_FileVersion  &nd_file_version        ()  { return this->variant.C_3.nd_file_version_F; };
    tbd00_ConsistencyInfo &nd_history_info        ()  { return this->variant.C_3.nd_history_info_F; };
    tsp00_Uint1        &ndPageVersion_bd00     ()  { return this->variant.C_3.ndPageVersion_bd00_F; };
    tsp00_Int4         &ndInvUsageCnt_bd00     ()  { return this->variant.C_3.ndInvUsageCnt_bd00_F; };
    tsp00_Int4         &ndLeafCount_bd00       ()  { return this->variant.C_3.ndLeafCount_bd00_F; };
    tsp00_Int4         &ndTreeLeaves_bd00      ()  { return this->variant.C_3.ndTreeLeaves_bd00_F; };
    tgg91_TransNo      &nd_trans_id            ()  { return this->variant.C_3.nd_trans_id_F; };
    tsp00_Int2         &nd_filler4             ()  { return this->variant.C_3.nd_filler4_F; };
    tsp00_PageNo       &ndInvRoot_bd00         ()  { return this->variant.C_3.ndInvRoot_bd00_F; };
    tsp00_Int4         &nd_write_cnt           ()  { return this->variant.C_3.nd_write_cnt_F; };
    tbd_pointer_list   &nd_pointer_list        ()  { return this->variant.C_3.nd_pointer_list_F; };
    tsp00_Int4         &nd_checksum            ()  { return this->variant.C_3.nd_checksum_F; };
    tgg00_PageTrailer  &nd_trailer             ()  { return this->variant.C_3.nd_trailer_F; };
    tsp00_C8           &nd_header_space        ()  { return this->variant.C_4.nd_header_space_F; };
    tsp00_Uint2        &nd_max_obj_cnt         ()  { return this->variant.C_4.nd_max_obj_cnt_F; };
    tsp00_Uint2        &nd_occ_obj_cnt         ()  { return this->variant.C_4.nd_occ_obj_cnt_F; };
    tsp00_Uint2        &nd_free_eot_obj_cnt    ()  { return this->variant.C_4.nd_free_eot_obj_cnt_F; };
    tsp00_Uint2        &nd_free_obj_cnt        ()  { return this->variant.C_4.nd_free_obj_cnt_F; };
    tsp00_Int2         &nd_level_space         ()  { return this->variant.C_4.nd_level_space_F; };
    tbd_file_state_set &nd_file_state_space    ()  { return this->variant.C_4.nd_file_state_space_F; };
    tbd_objpage_state_set &nd_objpage_state       ()  { return this->variant.C_4.nd_objpage_state_F; };
    tsp00_PageNo       &nd_root_space          ()  { return this->variant.C_4.nd_root_space_F; };
    tsp00_PageNo       &nd_right_space         ()  { return this->variant.C_4.nd_right_space_F; };
    tsp00_PageNo       &nd_left_space          ()  { return this->variant.C_4.nd_left_space_F; };
    tsp00_Int2         &nd_filler5_2           ()  { return this->variant.C_4.nd_filler5_2_F; };
    pasbool            &nd_in_free_chain       ()  { return this->variant.C_4.nd_in_free_chain_F; };
    tsp00_Uint1        &nd_num_key_files       ()  { return this->variant.C_4.nd_num_key_files_F; };
    tbd00_LockRequestSeqNo &ndLockRequestSeqNoLow_bd00 ()  { return this->variant.C_4.ndLockRequestSeqNoLow_bd00_F; };
    tgg00_ConverterVersion &nd_conv_vers_space     ()  { return this->variant.C_4.nd_conv_vers_space_F; };
    tsp00_Uint2        &nd_obj_frame_len       ()  { return this->variant.C_4.nd_obj_frame_len_F; };
    tsp00_Uint2        &nd_obj_key_len         ()  { return this->variant.C_4.nd_obj_key_len_F; };
    tgg91_FileVersion  &nd_file_vers_space     ()  { return this->variant.C_4.nd_file_vers_space_F; };
    tgg00_ObjPagePos   &nd_first_free_obj      ()  { return this->variant.C_4.nd_first_free_obj_F; };
    tgg00_ObjPagePos   &nd_first_obj           ()  { return this->variant.C_4.nd_first_obj_F; };
    tsp00_Int2         &nd_filler5_4           ()  { return this->variant.C_4.nd_filler5_4_F; };
    tsp00_C4           &nd_obj_file_no1        ()  { return this->variant.C_4.nd_obj_file_no1_F; };
    tbd00_LockRequestSeqNo &ndLockRequestSeqNoHigh_bd00 ()  { return this->variant.C_4.ndLockRequestSeqNoHigh_bd00_F; };
    tsp00_PageNo       &nd_next_free           ()  { return this->variant.C_4.nd_next_free_F; };
    tsp00_PageNo       &nd_prev_free           ()  { return this->variant.C_4.nd_prev_free_F; };
    tsp00_C4           &nd_obj_file_no2        ()  { return this->variant.C_4.nd_obj_file_no2_F; };
    tsp00_Int4         &nd_write_cnt_space     ()  { return this->variant.C_4.nd_write_cnt_space_F; };
    tsp00_C8           &ndStrHeaderSpace_bd00  ()  { return this->variant.C_5.ndStrHeaderSpace_bd00_F; };
    tgg00_PagePos      &ndStrBottomSpace_bd00  ()  { return this->variant.C_5.ndStrBottomSpace_bd00_F; };
    tsp00_Int4         &ndStrFiller1_bd00      ()  { return this->variant.C_5.ndStrFiller1_bd00_F; };
    tsp00_Int2         &ndStrLevelSpace_bd00   ()  { return this->variant.C_5.ndStrLevelSpace_bd00_F; };
    tbd_file_state_set &ndStrFileStateSpace_bd00 ()  { return this->variant.C_5.ndStrFileStateSpace_bd00_F; };
    pasbool            &ndStrFiller2_bd00      ()  { return this->variant.C_5.ndStrFiller2_bd00_F; };
    tsp00_PageNo       &ndStrRootSpace_bd00    ()  { return this->variant.C_5.ndStrRootSpace_bd00_F; };
    tsp00_PageNo       &ndStrRightSpace_bd00   ()  { return this->variant.C_5.ndStrRightSpace_bd00_F; };
    tsp00_PageNo       &ndStrLeftSpace_bd00    ()  { return this->variant.C_5.ndStrLeftSpace_bd00_F; };
    tgg00_Surrogate    &ndStrParentID_bd00     ()  { return this->variant.C_5.ndStrParentID_bd00_F; };
    tgg00_ConverterVersion &ndStrConvVersionSpace_bd00 ()  { return this->variant.C_5.ndStrConvVersionSpace_bd00_F; };
    tsp00_Int4         &ndStrVersion_bd00      ()  { return this->variant.C_5.ndStrVersion_bd00_F; };
    tgg91_FileVersion  &ndStrFileVersionSpace_bd00 ()  { return this->variant.C_5.ndStrFileVersionSpace_bd00_F; };
    tsp00_C6           &ndStrFiller4_bd00      ()  { return this->variant.C_5.ndStrFiller4_bd00_F; };
    tsp00_Int4         &ndStrFileSize_bd00     ()  { return this->variant.C_5.ndStrFileSize_bd00_F; };
    tsp00_Int4         &ndStrFiller5_bd00      ()  { return this->variant.C_5.ndStrFiller5_bd00_F; };
    tsp00_C8           &ndStrFiller6_bd00      ()  { return this->variant.C_5.ndStrFiller6_bd00_F; };
    tsp00_Int4         &ndStrFiller7_bd00      ()  { return this->variant.C_5.ndStrFiller7_bd00_F; };
    tsp00_Int4         &ndStrWriteCntSpace_bd00 ()  { return this->variant.C_5.ndStrWriteCntSpace_bd00_F; };
    tsp00_C8           &ndHeaderSpace_bd00     ()  { return this->variant.C_7.ndHeaderSpace_bd00_F; };
    tgg00_PagePos      &ndFirstOffset_bd00     ()  { return this->variant.C_7.ndFirstOffset_bd00_F; };
    tsp00_Int4         &ndFieldCount_bd00      ()  { return this->variant.C_7.ndFieldCount_bd00_F; };
    tsp00_Int2         &nd_level_space2        ()  { return this->variant.C_7.nd_level_space2_F; };
    tbd_file_state_set &nd_file_state_space2   ()  { return this->variant.C_7.nd_file_state_space2_F; };
    pasbool            &nd_sorted_space2       ()  { return this->variant.C_7.nd_sorted_space2_F; };
    tsp00_PageNo       &nd_root_space2         ()  { return this->variant.C_7.nd_root_space2_F; };
    tsp00_PageNo       &nd_right_space2        ()  { return this->variant.C_7.nd_right_space2_F; };
    tsp00_PageNo       &nd_left_space2         ()  { return this->variant.C_7.nd_left_space2_F; };
    tgg00_PagePos      &ndFirstFreeOffset_bd00 ()  { return this->variant.C_7.ndFirstFreeOffset_bd00_F; };
    tsp00_Int4         &ndAllFieldInQueueCount_bd00 ()  { return this->variant.C_7.ndAllFieldInQueueCount_bd00_F; };
    tbd00_PageFixSizeSpace &ndPageFixSizeSpace_bd00 ()  { return this->variant.C_8.ndPageFixSizeSpace_bd00_F; };
    tbd00_UndoPage     &ndUndoPage_bd00        ()  { return this->variant.C_9.ndUndoPage_bd00_F; };
    tbd00_RedoPage     &ndRedoPage_bd00        ()  { return this->variant.C_10.ndRedoPage_bd00_F; };
    tbd00_OpenTransPage &ndOpenTransPage_bd00   ()  { return this->variant.C_11.ndOpenTransPage_bd00_F; };
    /* const accessor functions */
    const tgg00_PageHeader &nd_header              () const  { return this->variant.C_1.nd_header_F; };
    const tbd_node_header &nd_full_header         () const  { return this->variant.C_2.nd_full_header_F; };
    const tbd_node_body &nd_body                () const  { return this->variant.C_2.nd_body_F; };
    const tgg00_PageCheckTrailer &nd_full_trailer        () const  { return this->variant.C_2.nd_full_trailer_F; };
    const tsp00_PageNo &nd_id                  () const  { return this->variant.C_3.nd_id_F; };
    const tgg00_PageType &nd_pt                  () const  { return this->variant.C_3.nd_pt_F; };
    const tgg00_PageType2 &nd_pt2                 () const  { return this->variant.C_3.nd_pt2_F; };
    const tgg00_PageCheckType &nd_checktype           () const  { return this->variant.C_3.nd_checktype_F; };
    const tgg00_PageMode &nd_pmode               () const  { return this->variant.C_3.nd_pmode_F; };
    const tgg00_PagePos &nd_bottom              () const  { return this->variant.C_3.nd_bottom_F; };
    const tsp00_Int4   &nd_record_cnt          () const  { return this->variant.C_3.nd_record_cnt_F; };
    const tsp00_Int2   &nd_level               () const  { return this->variant.C_3.nd_level_F; };
    const tbd_file_state_set &nd_file_state          () const  { return this->variant.C_3.nd_file_state_F; };
    const pasbool      &nd_sorted              () const  { return this->variant.C_3.nd_sorted_F; };
    const tsp00_PageNo &nd_root                () const  { return this->variant.C_3.nd_root_F; };
    const tsp00_PageNo &nd_right               () const  { return this->variant.C_3.nd_right_F; };
    const tsp00_PageNo &nd_left                () const  { return this->variant.C_3.nd_left_F; };
    const tsp00_PageNo &nd_last                () const  { return this->variant.C_3.nd_last_F; };
    const tsp00_Int4   &nd_filler2             () const  { return this->variant.C_3.nd_filler2_F; };
    const tgg00_ConverterVersion &nd_conv_version        () const  { return this->variant.C_3.nd_conv_version_F; };
    const tsp00_Int4   &nd_str_version         () const  { return this->variant.C_3.nd_str_version_F; };
    const tgg91_FileVersion &nd_file_version        () const  { return this->variant.C_3.nd_file_version_F; };
    const tbd00_ConsistencyInfo &nd_history_info        () const  { return this->variant.C_3.nd_history_info_F; };
    const tsp00_Uint1  &ndPageVersion_bd00     () const  { return this->variant.C_3.ndPageVersion_bd00_F; };
    const tsp00_Int4   &ndInvUsageCnt_bd00     () const  { return this->variant.C_3.ndInvUsageCnt_bd00_F; };
    const tsp00_Int4   &ndLeafCount_bd00       () const  { return this->variant.C_3.ndLeafCount_bd00_F; };
    const tsp00_Int4   &ndTreeLeaves_bd00      () const  { return this->variant.C_3.ndTreeLeaves_bd00_F; };
    const tgg91_TransNo &nd_trans_id            () const  { return this->variant.C_3.nd_trans_id_F; };
    const tsp00_Int2   &nd_filler4             () const  { return this->variant.C_3.nd_filler4_F; };
    const tsp00_PageNo &ndInvRoot_bd00         () const  { return this->variant.C_3.ndInvRoot_bd00_F; };
    const tsp00_Int4   &nd_write_cnt           () const  { return this->variant.C_3.nd_write_cnt_F; };
    const tbd_pointer_list &nd_pointer_list        () const  { return this->variant.C_3.nd_pointer_list_F; };
    const tsp00_Int4   &nd_checksum            () const  { return this->variant.C_3.nd_checksum_F; };
    const tgg00_PageTrailer &nd_trailer             () const  { return this->variant.C_3.nd_trailer_F; };
    const tsp00_C8     &nd_header_space        () const  { return this->variant.C_4.nd_header_space_F; };
    const tsp00_Uint2  &nd_max_obj_cnt         () const  { return this->variant.C_4.nd_max_obj_cnt_F; };
    const tsp00_Uint2  &nd_occ_obj_cnt         () const  { return this->variant.C_4.nd_occ_obj_cnt_F; };
    const tsp00_Uint2  &nd_free_eot_obj_cnt    () const  { return this->variant.C_4.nd_free_eot_obj_cnt_F; };
    const tsp00_Uint2  &nd_free_obj_cnt        () const  { return this->variant.C_4.nd_free_obj_cnt_F; };
    const tsp00_Int2   &nd_level_space         () const  { return this->variant.C_4.nd_level_space_F; };
    const tbd_file_state_set &nd_file_state_space    () const  { return this->variant.C_4.nd_file_state_space_F; };
    const tbd_objpage_state_set &nd_objpage_state       () const  { return this->variant.C_4.nd_objpage_state_F; };
    const tsp00_PageNo &nd_root_space          () const  { return this->variant.C_4.nd_root_space_F; };
    const tsp00_PageNo &nd_right_space         () const  { return this->variant.C_4.nd_right_space_F; };
    const tsp00_PageNo &nd_left_space          () const  { return this->variant.C_4.nd_left_space_F; };
    const tsp00_Int2   &nd_filler5_2           () const  { return this->variant.C_4.nd_filler5_2_F; };
    const pasbool      &nd_in_free_chain       () const  { return this->variant.C_4.nd_in_free_chain_F; };
    const tsp00_Uint1  &nd_num_key_files       () const  { return this->variant.C_4.nd_num_key_files_F; };
    const tbd00_LockRequestSeqNo &ndLockRequestSeqNoLow_bd00 () const  { return this->variant.C_4.ndLockRequestSeqNoLow_bd00_F; };
    const tgg00_ConverterVersion &nd_conv_vers_space     () const  { return this->variant.C_4.nd_conv_vers_space_F; };
    const tsp00_Uint2  &nd_obj_frame_len       () const  { return this->variant.C_4.nd_obj_frame_len_F; };
    const tsp00_Uint2  &nd_obj_key_len         () const  { return this->variant.C_4.nd_obj_key_len_F; };
    const tgg91_FileVersion &nd_file_vers_space     () const  { return this->variant.C_4.nd_file_vers_space_F; };
    const tgg00_ObjPagePos &nd_first_free_obj      () const  { return this->variant.C_4.nd_first_free_obj_F; };
    const tgg00_ObjPagePos &nd_first_obj           () const  { return this->variant.C_4.nd_first_obj_F; };
    const tsp00_Int2   &nd_filler5_4           () const  { return this->variant.C_4.nd_filler5_4_F; };
    const tsp00_C4     &nd_obj_file_no1        () const  { return this->variant.C_4.nd_obj_file_no1_F; };
    const tbd00_LockRequestSeqNo &ndLockRequestSeqNoHigh_bd00 () const  { return this->variant.C_4.ndLockRequestSeqNoHigh_bd00_F; };
    const tsp00_PageNo &nd_next_free           () const  { return this->variant.C_4.nd_next_free_F; };
    const tsp00_PageNo &nd_prev_free           () const  { return this->variant.C_4.nd_prev_free_F; };
    const tsp00_C4     &nd_obj_file_no2        () const  { return this->variant.C_4.nd_obj_file_no2_F; };
    const tsp00_Int4   &nd_write_cnt_space     () const  { return this->variant.C_4.nd_write_cnt_space_F; };
    const tsp00_C8     &ndStrHeaderSpace_bd00  () const  { return this->variant.C_5.ndStrHeaderSpace_bd00_F; };
    const tgg00_PagePos &ndStrBottomSpace_bd00  () const  { return this->variant.C_5.ndStrBottomSpace_bd00_F; };
    const tsp00_Int4   &ndStrFiller1_bd00      () const  { return this->variant.C_5.ndStrFiller1_bd00_F; };
    const tsp00_Int2   &ndStrLevelSpace_bd00   () const  { return this->variant.C_5.ndStrLevelSpace_bd00_F; };
    const tbd_file_state_set &ndStrFileStateSpace_bd00 () const  { return this->variant.C_5.ndStrFileStateSpace_bd00_F; };
    const pasbool      &ndStrFiller2_bd00      () const  { return this->variant.C_5.ndStrFiller2_bd00_F; };
    const tsp00_PageNo &ndStrRootSpace_bd00    () const  { return this->variant.C_5.ndStrRootSpace_bd00_F; };
    const tsp00_PageNo &ndStrRightSpace_bd00   () const  { return this->variant.C_5.ndStrRightSpace_bd00_F; };
    const tsp00_PageNo &ndStrLeftSpace_bd00    () const  { return this->variant.C_5.ndStrLeftSpace_bd00_F; };
    const tgg00_Surrogate &ndStrParentID_bd00     () const  { return this->variant.C_5.ndStrParentID_bd00_F; };
    const tgg00_ConverterVersion &ndStrConvVersionSpace_bd00 () const  { return this->variant.C_5.ndStrConvVersionSpace_bd00_F; };
    const tsp00_Int4   &ndStrVersion_bd00      () const  { return this->variant.C_5.ndStrVersion_bd00_F; };
    const tgg91_FileVersion &ndStrFileVersionSpace_bd00 () const  { return this->variant.C_5.ndStrFileVersionSpace_bd00_F; };
    const tsp00_C6     &ndStrFiller4_bd00      () const  { return this->variant.C_5.ndStrFiller4_bd00_F; };
    const tsp00_Int4   &ndStrFileSize_bd00     () const  { return this->variant.C_5.ndStrFileSize_bd00_F; };
    const tsp00_Int4   &ndStrFiller5_bd00      () const  { return this->variant.C_5.ndStrFiller5_bd00_F; };
    const tsp00_C8     &ndStrFiller6_bd00      () const  { return this->variant.C_5.ndStrFiller6_bd00_F; };
    const tsp00_Int4   &ndStrFiller7_bd00      () const  { return this->variant.C_5.ndStrFiller7_bd00_F; };
    const tsp00_Int4   &ndStrWriteCntSpace_bd00 () const  { return this->variant.C_5.ndStrWriteCntSpace_bd00_F; };
    const tsp00_C8     &ndHeaderSpace_bd00     () const  { return this->variant.C_7.ndHeaderSpace_bd00_F; };
    const tgg00_PagePos &ndFirstOffset_bd00     () const  { return this->variant.C_7.ndFirstOffset_bd00_F; };
    const tsp00_Int4   &ndFieldCount_bd00      () const  { return this->variant.C_7.ndFieldCount_bd00_F; };
    const tsp00_Int2   &nd_level_space2        () const  { return this->variant.C_7.nd_level_space2_F; };
    const tbd_file_state_set &nd_file_state_space2   () const  { return this->variant.C_7.nd_file_state_space2_F; };
    const pasbool      &nd_sorted_space2       () const  { return this->variant.C_7.nd_sorted_space2_F; };
    const tsp00_PageNo &nd_root_space2         () const  { return this->variant.C_7.nd_root_space2_F; };
    const tsp00_PageNo &nd_right_space2        () const  { return this->variant.C_7.nd_right_space2_F; };
    const tsp00_PageNo &nd_left_space2         () const  { return this->variant.C_7.nd_left_space2_F; };
    const tgg00_PagePos &ndFirstFreeOffset_bd00 () const  { return this->variant.C_7.ndFirstFreeOffset_bd00_F; };
    const tsp00_Int4   &ndAllFieldInQueueCount_bd00 () const  { return this->variant.C_7.ndAllFieldInQueueCount_bd00_F; };
    const tbd00_PageFixSizeSpace &ndPageFixSizeSpace_bd00 () const  { return this->variant.C_8.ndPageFixSizeSpace_bd00_F; };
    const tbd00_UndoPage &ndUndoPage_bd00        () const  { return this->variant.C_9.ndUndoPage_bd00_F; };
    const tbd00_RedoPage &ndRedoPage_bd00        () const  { return this->variant.C_10.ndRedoPage_bd00_F; };
    const tbd00_OpenTransPage &ndOpenTransPage_bd00   () const  { return this->variant.C_11.ndOpenTransPage_bd00_F; };
#endif    /* defined (__cplusplus) */
} tbd_node;
typedef tbd_node                  *tbd_nodeptr;

typedef struct                     tbd_node_ptrs
{
    union
    {
    /* 1 */
        struct tbd_node_ptrs_1
        {
            tbd_nodeptr         np_ptr_F;
            tbd_nodeptr         np_cbptr_F;
        } C_1;
    /* 2 */
        struct tbd_node_ptrs_2
        {
            tsp00_PageAddr      np_ptr_buf_F;
            tsp00_PageAddr      np_cbptr_buf_F;
        } C_2;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tbd_nodeptr        &np_ptr                 ()  { return this->variant.C_1.np_ptr_F; };
    tbd_nodeptr        &np_cbptr               ()  { return this->variant.C_1.np_cbptr_F; };
    tsp00_PageAddr     &np_ptr_buf             ()  { return this->variant.C_2.np_ptr_buf_F; };
    tsp00_PageAddr     &np_cbptr_buf           ()  { return this->variant.C_2.np_cbptr_buf_F; };
    /* const accessor functions */
    const tbd_nodeptr  &np_ptr                 () const  { return this->variant.C_1.np_ptr_F; };
    const tbd_nodeptr  &np_cbptr               () const  { return this->variant.C_1.np_cbptr_F; };
    const tsp00_PageAddr &np_ptr_buf             () const  { return this->variant.C_2.np_ptr_buf_F; };
    const tsp00_PageAddr &np_cbptr_buf           () const  { return this->variant.C_2.np_cbptr_buf_F; };
#endif    /* defined (__cplusplus) */
} tbd_node_ptrs;
typedef ARRAY_TYPE_MSP00 (         tbd_page_list, tbd_node, MAX_VF_BUFFERS_BD00);
    /* */
typedef tbd_page_list             *tbd_pagelistptr;
    /* */
typedef ARRAY_TYPE_MSP00 (         tbd00_NodePtrList, tbd_nodeptr, MAX_VF_BUFFERS_BD00);

typedef struct                     tbd_univ_ptr
{
    union
    {
    /* 1 */
        struct tbd_univ_ptr_1
        {
            tbd_nodeptr         nodeaddr_F;
        } C_1;
    /* 2 */
        struct tbd_univ_ptr_2
        {
            tsp00_PageAddr      bufaddr_F;
        } C_2;
    /* 3 */
        struct tbd_univ_ptr_3
        {
            tbd_pagelistptr     plistaddr_F;
        } C_3;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tbd_nodeptr        &nodeaddr               ()  { return this->variant.C_1.nodeaddr_F; };
    tsp00_PageAddr     &bufaddr                ()  { return this->variant.C_2.bufaddr_F; };
    tbd_pagelistptr    &plistaddr              ()  { return this->variant.C_3.plistaddr_F; };
    /* const accessor functions */
    const tbd_nodeptr  &nodeaddr               () const  { return this->variant.C_1.nodeaddr_F; };
    const tsp00_PageAddr &bufaddr                () const  { return this->variant.C_2.bufaddr_F; };
    const tbd_pagelistptr &plistaddr              () const  { return this->variant.C_3.plistaddr_F; };
#endif    /* defined (__cplusplus) */
} tbd_univ_ptr;

typedef struct                     tbd_current_tree
{
    tgg00_TransContextPtr curr_trans;
    tbd_node_ptrs       currRootNptrs_bd00;
    tbd_node_ptrs       currIndexNptrs_bd00;
    tsp00_PageNo        curr_lvl_1_pno;
    tsp00_PageNo        curr_lvl_1_left;
    tsp00_PageNo        currLeftBound_bd00;
    tsp00_PageNo        currRightBound_bd00;
    tgg00_FileId        curr_tree_id;
    tbd00_FreeQueue     currFreeQueue_bd00;
    tsp00_Int4          currNumLeaves_bd00;
    tsp00_Int4          currLeafPageCountDelta_bd00;
    tsp00_Int4          currIndexPageCountDelta_bd00;
    tsp00_Int2          currCounter_bd00;
    tbd00_PageLockMode  currRootPageLockMode_bd00;
    tbd00_PageLockMode  currIndexPageLockMode_bd00;
    tgg00_MessType      curr_action;
    pasbool             currUnlockAllowed_bd00;
    pasbool             currRootUpdated_bd00;
    pasbool             currIndexUpdated_bd00;
    tgg00_ExtendedFiletypeSet currExtendedFileTypeSet_bd00;
    pasbool             currFill1;
    tsp00_Int2          currFill2;
} tbd_current_tree;

typedef struct                     tbd_neighbors
{
    tsp00_PageNo        pre_ln;
    tsp00_PageNo        ln;
    tsp00_PageNo        rn;
} tbd_neighbors;

typedef enum tbd_searchresult_Enum
{
    thisfound,
    nextfound,
    lastfound,
    nonefound
} tbd_searchresult_Enum;
typedef tbd_searchresult_Enum tbd_searchresult_Param;
typedef ENUM_TYPE_MSP00 (          tbd_searchresult, tbd_searchresult_Enum, nonefound, unsigned char);
    /* */
typedef tgg90_Cint4                tbd00_SepLeafCount;
    /* */

typedef struct                     tbd00_Separator
{
    union
    {
    /* 1 */
        struct tbd00_Separator_1
        {
            tgg00_HeaderRec     sepHead_bd00_F;
        } C_1;
    /* 2 */
        struct tbd00_Separator_2
        {
            tgg00_Lkey          sepKey_bd00_F;
            tbd00_SepLeafCount  sepLeafCount_bd00_F;
        } C_2;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tgg00_HeaderRec    &sepHead_bd00           ()  { return this->variant.C_1.sepHead_bd00_F; };
    tgg00_Lkey         &sepKey_bd00            ()  { return this->variant.C_2.sepKey_bd00_F; };
    tbd00_SepLeafCount &sepLeafCount_bd00      ()  { return this->variant.C_2.sepLeafCount_bd00_F; };
    /* const accessor functions */
    const tgg00_HeaderRec &sepHead_bd00           () const  { return this->variant.C_1.sepHead_bd00_F; };
    const tgg00_Lkey   &sepKey_bd00            () const  { return this->variant.C_2.sepKey_bd00_F; };
    const tbd00_SepLeafCount &sepLeafCount_bd00      () const  { return this->variant.C_2.sepLeafCount_bd00_F; };
#endif    /* defined (__cplusplus) */
} tbd00_Separator;
typedef tbd00_Separator           *tbd00_SeparatorPtr;
    /* */

typedef enum tbd00_TreeIndexOp_Enum
{
    opAdd_ebd00,
    opDel_ebd00,
    opRepl_ebd00
} tbd00_TreeIndexOp_Enum;
typedef tbd00_TreeIndexOp_Enum tbd00_TreeIndexOp_Param;
typedef ENUM_TYPE_MSP00 (          tbd00_TreeIndexOp, tbd00_TreeIndexOp_Enum, opRepl_ebd00, unsigned char);

typedef struct                     tbd00_TreeIndexOrder
{
    tbd00_TreeIndexOp   txoOp_bd00;
    pasbool             txoFill_bd00;
    tsp00_Int2          txoLevel_bd00;
    tgg00_Lkey          txoSearchKey_bd00;
    tbd00_Separator     txoSep_bd00;
} tbd00_TreeIndexOrder;
typedef tbd00_TreeIndexOrder      *tbd00_TreeIndexOrderPtr;

typedef struct                     tbd00_TreeStatistic
{
    tsp00_Int4          ts_IndexPages_bd00;
    tsp00_Int4          ts_LeafPages_bd00;
    tsp00_Int4          ts_RecordCount_bd00;
} tbd00_TreeStatistic;

typedef struct                     tbd00_OrderList
{
    tsp00_Int2          olstCount_bd00;
    tsp00_Int2          olstFill_bd00;
    ARRAY_VAR_MSP00 (         olstOrder_bd00, tbd00_TreeIndexOrder, UPB_OF_LIST_BD00 - LWB_OF_LIST_BD00 + 1);
} tbd00_OrderList;

typedef struct                     tbd_stat_aux_vars
{
    tsp00_Int4          sav_separators;
    pasbool             sav_long_oflw;
    pasbool             sav_reclen_oflw;
} tbd_stat_aux_vars;

struct tbd00_TemplateDummy {
    tbd_searchresult     dummy_tbd_searchresult;
    tbd_node_request     dummy_tbd_node_request;
    tbd00_PageLockMode   dummy_tbd00_PageLockMode;
    tbd00_DelInvReason   dummy_tbd00_DelInvReason;
    tbd00_TreeIndexOp    dummy_tbd00_TreeIndexOp;
    tbd_lru_info         dummy_tbd_lru_info;
    tbd_fileinfo_desc_set dummy_tbd_fileinfo_desc_set;
    tbd_file_state_set   dummy_tbd_file_state_set;
    tbd00_ConsistencyInfo dummy_tbd00_ConsistencyInfo;
    tbd_objpage_state_set dummy_tbd_objpage_state_set;
};


#endif
