
/* some common prolog for every module
 */

#ifndef GBD02_H
#define GBD02_H


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

#ifndef GBD00_H
#include "gbd00.h"
#endif

/******************************************
 *           C O N S T A N T S            *
 ******************************************/


/******************************************
 *               T Y P E S                *
 ******************************************/


typedef enum tbd02_SyncWithSavepoint_Enum
{
    synNone_ebd02,
    synCheckPreventSync_ebd02,
    synWait_ebd02,
    synUpdateSyncCnt_ebd02
} tbd02_SyncWithSavepoint_Enum;
typedef tbd02_SyncWithSavepoint_Enum tbd02_SyncWithSavepoint_Param;
typedef ENUM_TYPE_MSP00 (          tbd02_SyncWithSavepoint, tbd02_SyncWithSavepoint_Enum, synUpdateSyncCnt_ebd02, unsigned char);
    /* */

typedef struct                     tbd02_CachePageState
{
    pasbool             cpsFound_bd02;
    pasbool             cpsDirty_bd02;
    pasbool             cpsWaitForWrite_bd02;
    pasbool             cpsWaitForSvpEnd_bd02;
    pasbool             cpsIoDone_bd02;
    pasbool             cpsHaveToWaitForLock_bd02;
    pasbool             cpsIntendToWaitForLock_bd02;
    pasbool             cpsTaskRescheduled_bd02;
    tbd02_SyncWithSavepoint cpsSyncWithSavepoint_bd02;
} tbd02_CachePageState;
    /* */

typedef enum tbd02_ChangedState_Enum
{
    cstNone_ebd02,
    cstChanged_ebd02,
    cstSvpRelevant_ebd02
} tbd02_ChangedState_Enum;
typedef tbd02_ChangedState_Enum tbd02_ChangedState_Param;
typedef ENUM_TYPE_MSP00 (          tbd02_ChangedState, tbd02_ChangedState_Enum, cstSvpRelevant_ebd02, unsigned char);
    /* */

typedef enum tbd02_DataIOState_Enum
{
    iosNone_ebd02,
    iosUserIO_ebd02,
    iosServerIO_ebd02,
    iosBlocked_ebd02
} tbd02_DataIOState_Enum;
typedef tbd02_DataIOState_Enum tbd02_DataIOState_Param;
typedef ENUM_TYPE_MSP00 (          tbd02_DataIOState, tbd02_DataIOState_Enum, iosBlocked_ebd02, unsigned char);
    /* */

typedef enum tbd02_SwapState_Enum
{
    swsIoDone_ebd02,
    swsTestIo_ebd02,
    swsDoIo_ebd02
} tbd02_SwapState_Enum;
typedef tbd02_SwapState_Enum tbd02_SwapState_Param;
typedef ENUM_TYPE_MSP00 (          tbd02_SwapState, tbd02_SwapState_Enum, swsDoIo_ebd02, unsigned char);
    /* */

typedef struct                     tbd02_DataCBlockInit
{
    tsp00_BufAddr       dcbi_lru_next;
    tsp00_BufAddr       dcbi_lru_prev;
    tsp00_BufAddr       dcbi_io_next;
    tsp00_BufAddr       dcbi_io_prev;
    tsp00_BufAddr       dcbi_queue_occ;
    tsp00_BufAddr       dcbi_queue_old_occ;
    tsp00_BufAddr       dcbi_lock_req_head;
    tsp00_BufAddr       dcbi_lock_req_tail;
    tsp00_PageNo        dcbi_occupant;
    tsp00_PageNo        dcbi_OldOccupant_bd02;
    tsp00_TaskId        dcbi_LastOwner_bd02;
    tsp00_Int2          dcbi_UsageCnt_bd02;
    tgg00_Tfn           dcbi_tfn;
    pasbool             dcbi_lru_rechain;
    tbd02_ChangedState  dcbi_ChangedState_bd02;
    tbd02_DataIOState   dcbi_io_state;
    tsp00_Int2          dcbi_SvpSyncCnt_bd02;
    pasbool             dcbi_IsSourceOfACopy_bd02;
    tsp00_Uint1         dcbi_copy_no;
    pasbool             dcbi_dw_io_area;
    pasbool             dcbi_IsClustered_bd02;
    tsp00_Uint4         dcbi_AccessCnt_bd02;
    tsp00_Uint4         dcbi_CollisionCnt_bd02;
    tsp00_Uint4         dcbi_UpdateCnt_bd02;
    tsp00_Int2          dcbi_Filler3_bd02;
    tgg00_RecoveryMode  dcbi_RecMode_bd02;
    tgg00_RecoveryMode  dcbi_OldRecMode_bd02;
} tbd02_DataCBlockInit;

typedef enum tbd02_TaskQueueReason_Enum
{
    tqrNil_ebd02,
    tqrShareLock_ebd02,
    tqrExclusiveLock_ebd02
} tbd02_TaskQueueReason_Enum;
typedef tbd02_TaskQueueReason_Enum tbd02_TaskQueueReason_Param;
typedef ENUM_TYPE_MSP00 (          tbd02_TaskQueueReason, tbd02_TaskQueueReason_Enum, tqrExclusiveLock_ebd02, unsigned char);
    /* */
typedef struct tbd02_TaskQueueItem tbd02_TaskQueueItem;
typedef tbd02_TaskQueueItem       *tbd02_pTaskQueueItem;

 struct                     tbd02_TaskQueueItem
{
    tsp00_TaskId        tqiTaskId_bd02;
    tbd02_TaskQueueReason tqiReason_bd02;
    pasbool             tqiFiller1_bd02;
    tsp00_Int2          tqiFiller2_bd02;
    tbd02_pTaskQueueItem tqiFixedNext_bd02;
    tbd02_pTaskQueueItem tqiVarNext_bd02;
} ;
typedef struct tbd02_DataCBlock tbd02_DataCBlock;
typedef tbd02_DataCBlock          *tbd02_pDataCBlock;

 struct                     tbd02_DataCBlock
{
    tbd02_pDataCBlock   dcb_FixedNext_bd02;
    tbd_nodeptr         dcb_pFrame_bd02;
    tbd02_pDataCBlock   dcb_VarNext_bd02;
    union
    {
    /* false */
        struct tbd02_DataCBlock_false
        {
            tbd02_pDataCBlock   dcb_lru_next_F;
            tbd02_pDataCBlock   dcb_lru_prev_F;
            tbd02_pDataCBlock   dcb_io_next_F;
            tbd02_pDataCBlock   dcb_io_prev_F;
            tbd02_pTaskQueueItem dcb_queue_occ_F;
            tbd02_pTaskQueueItem dcb_queue_old_occ_F;
            tbd02_pTaskQueueItem dcb_lock_req_head_F;
            tbd02_pTaskQueueItem dcb_lock_req_tail_F;
            tsp00_PageNo        dcb_occupant_F;
            tsp00_PageNo        dcb_OldOccupant_bd02_F;
            tsp00_TaskId        dcb_LastOwner_bd02_F;
            tsp00_Int2          dcb_UsageCnt_bd02_F;
            tgg00_Tfn           dcb_tfn_F;
            pasbool             dcb_lru_rechain_F;
            tbd02_ChangedState  dcb_ChangedState_bd02_F;
            tbd02_DataIOState   dcb_io_state_F;
            tsp00_Int2          dcb_SvpSyncCnt_bd02_F;
            pasbool             dcb_IsSourceOfACopy_bd02_F;
            tsp00_Uint1         dcb_copy_no_F;
            pasbool             dcb_dw_io_area_F;
            pasbool             dcb_IsClustered_bd02_F;
            tsp00_Uint4         dcb_AccessCnt_bd02_F;
            tsp00_Uint4         dcb_CollisionCnt_bd02_F;
            tsp00_Uint4         dcb_UpdateCnt_bd02_F;
            tsp00_Int2          dcb_Filler3_F;
            tgg00_RecoveryMode  dcb_RecMode_bd02_F;
            tgg00_RecoveryMode  dcb_OldRecMode_bd02_F;
        } C_false;
    /* true */
        struct tbd02_DataCBlock_true
        {
            tbd02_DataCBlockInit dcb_Init_bd02_F;
        } C_true;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tbd02_pDataCBlock  &dcb_lru_next           ()  { return this->variant.C_false.dcb_lru_next_F; };
    tbd02_pDataCBlock  &dcb_lru_prev           ()  { return this->variant.C_false.dcb_lru_prev_F; };
    tbd02_pDataCBlock  &dcb_io_next            ()  { return this->variant.C_false.dcb_io_next_F; };
    tbd02_pDataCBlock  &dcb_io_prev            ()  { return this->variant.C_false.dcb_io_prev_F; };
    tbd02_pTaskQueueItem &dcb_queue_occ          ()  { return this->variant.C_false.dcb_queue_occ_F; };
    tbd02_pTaskQueueItem &dcb_queue_old_occ      ()  { return this->variant.C_false.dcb_queue_old_occ_F; };
    tbd02_pTaskQueueItem &dcb_lock_req_head      ()  { return this->variant.C_false.dcb_lock_req_head_F; };
    tbd02_pTaskQueueItem &dcb_lock_req_tail      ()  { return this->variant.C_false.dcb_lock_req_tail_F; };
    tsp00_PageNo       &dcb_occupant           ()  { return this->variant.C_false.dcb_occupant_F; };
    tsp00_PageNo       &dcb_OldOccupant_bd02   ()  { return this->variant.C_false.dcb_OldOccupant_bd02_F; };
    tsp00_TaskId       &dcb_LastOwner_bd02     ()  { return this->variant.C_false.dcb_LastOwner_bd02_F; };
    tsp00_Int2         &dcb_UsageCnt_bd02      ()  { return this->variant.C_false.dcb_UsageCnt_bd02_F; };
    tgg00_Tfn          &dcb_tfn                ()  { return this->variant.C_false.dcb_tfn_F; };
    pasbool            &dcb_lru_rechain        ()  { return this->variant.C_false.dcb_lru_rechain_F; };
    tbd02_ChangedState &dcb_ChangedState_bd02  ()  { return this->variant.C_false.dcb_ChangedState_bd02_F; };
    tbd02_DataIOState  &dcb_io_state           ()  { return this->variant.C_false.dcb_io_state_F; };
    tsp00_Int2         &dcb_SvpSyncCnt_bd02    ()  { return this->variant.C_false.dcb_SvpSyncCnt_bd02_F; };
    pasbool            &dcb_IsSourceOfACopy_bd02 ()  { return this->variant.C_false.dcb_IsSourceOfACopy_bd02_F; };
    tsp00_Uint1        &dcb_copy_no            ()  { return this->variant.C_false.dcb_copy_no_F; };
    pasbool            &dcb_dw_io_area         ()  { return this->variant.C_false.dcb_dw_io_area_F; };
    pasbool            &dcb_IsClustered_bd02   ()  { return this->variant.C_false.dcb_IsClustered_bd02_F; };
    tsp00_Uint4        &dcb_AccessCnt_bd02     ()  { return this->variant.C_false.dcb_AccessCnt_bd02_F; };
    tsp00_Uint4        &dcb_CollisionCnt_bd02  ()  { return this->variant.C_false.dcb_CollisionCnt_bd02_F; };
    tsp00_Uint4        &dcb_UpdateCnt_bd02     ()  { return this->variant.C_false.dcb_UpdateCnt_bd02_F; };
    tsp00_Int2         &dcb_Filler3            ()  { return this->variant.C_false.dcb_Filler3_F; };
    tgg00_RecoveryMode &dcb_RecMode_bd02       ()  { return this->variant.C_false.dcb_RecMode_bd02_F; };
    tgg00_RecoveryMode &dcb_OldRecMode_bd02    ()  { return this->variant.C_false.dcb_OldRecMode_bd02_F; };
    tbd02_DataCBlockInit &dcb_Init_bd02          ()  { return this->variant.C_true.dcb_Init_bd02_F; };
    /* const accessor functions */
    const tbd02_pDataCBlock &dcb_lru_next           () const  { return this->variant.C_false.dcb_lru_next_F; };
    const tbd02_pDataCBlock &dcb_lru_prev           () const  { return this->variant.C_false.dcb_lru_prev_F; };
    const tbd02_pDataCBlock &dcb_io_next            () const  { return this->variant.C_false.dcb_io_next_F; };
    const tbd02_pDataCBlock &dcb_io_prev            () const  { return this->variant.C_false.dcb_io_prev_F; };
    const tbd02_pTaskQueueItem &dcb_queue_occ          () const  { return this->variant.C_false.dcb_queue_occ_F; };
    const tbd02_pTaskQueueItem &dcb_queue_old_occ      () const  { return this->variant.C_false.dcb_queue_old_occ_F; };
    const tbd02_pTaskQueueItem &dcb_lock_req_head      () const  { return this->variant.C_false.dcb_lock_req_head_F; };
    const tbd02_pTaskQueueItem &dcb_lock_req_tail      () const  { return this->variant.C_false.dcb_lock_req_tail_F; };
    const tsp00_PageNo &dcb_occupant           () const  { return this->variant.C_false.dcb_occupant_F; };
    const tsp00_PageNo &dcb_OldOccupant_bd02   () const  { return this->variant.C_false.dcb_OldOccupant_bd02_F; };
    const tsp00_TaskId &dcb_LastOwner_bd02     () const  { return this->variant.C_false.dcb_LastOwner_bd02_F; };
    const tsp00_Int2   &dcb_UsageCnt_bd02      () const  { return this->variant.C_false.dcb_UsageCnt_bd02_F; };
    const tgg00_Tfn    &dcb_tfn                () const  { return this->variant.C_false.dcb_tfn_F; };
    const pasbool      &dcb_lru_rechain        () const  { return this->variant.C_false.dcb_lru_rechain_F; };
    const tbd02_ChangedState &dcb_ChangedState_bd02  () const  { return this->variant.C_false.dcb_ChangedState_bd02_F; };
    const tbd02_DataIOState &dcb_io_state           () const  { return this->variant.C_false.dcb_io_state_F; };
    const tsp00_Int2   &dcb_SvpSyncCnt_bd02    () const  { return this->variant.C_false.dcb_SvpSyncCnt_bd02_F; };
    const pasbool      &dcb_IsSourceOfACopy_bd02 () const  { return this->variant.C_false.dcb_IsSourceOfACopy_bd02_F; };
    const tsp00_Uint1  &dcb_copy_no            () const  { return this->variant.C_false.dcb_copy_no_F; };
    const pasbool      &dcb_dw_io_area         () const  { return this->variant.C_false.dcb_dw_io_area_F; };
    const pasbool      &dcb_IsClustered_bd02   () const  { return this->variant.C_false.dcb_IsClustered_bd02_F; };
    const tsp00_Uint4  &dcb_AccessCnt_bd02     () const  { return this->variant.C_false.dcb_AccessCnt_bd02_F; };
    const tsp00_Uint4  &dcb_CollisionCnt_bd02  () const  { return this->variant.C_false.dcb_CollisionCnt_bd02_F; };
    const tsp00_Uint4  &dcb_UpdateCnt_bd02     () const  { return this->variant.C_false.dcb_UpdateCnt_bd02_F; };
    const tsp00_Int2   &dcb_Filler3            () const  { return this->variant.C_false.dcb_Filler3_F; };
    const tgg00_RecoveryMode &dcb_RecMode_bd02       () const  { return this->variant.C_false.dcb_RecMode_bd02_F; };
    const tgg00_RecoveryMode &dcb_OldRecMode_bd02    () const  { return this->variant.C_false.dcb_OldRecMode_bd02_F; };
    const tbd02_DataCBlockInit &dcb_Init_bd02          () const  { return this->variant.C_true.dcb_Init_bd02_F; };
#endif    /* defined (__cplusplus) */
} ;
typedef ARRAY_TYPE_MSP00 (         tbd02_pDataCBlockList, tbd02_pDataCBlock, 25000000);
typedef tbd02_pDataCBlockList     *tbd02_pDataCBlockListPtr;
    /* */
typedef ARRAY_TYPE_MSP00 (         tbd2_data_cb_flush_list, tbd02_pDataCBlock, MAX_VF_BUFFERS_BD00);

struct tbd02_TemplateDummy {
    tbd02_SyncWithSavepoint dummy_tbd02_SyncWithSavepoint;
    tbd02_DataIOState    dummy_tbd02_DataIOState;
    tbd02_TaskQueueReason dummy_tbd02_TaskQueueReason;
    tbd02_ChangedState   dummy_tbd02_ChangedState;
    tbd02_SwapState      dummy_tbd02_SwapState;
};


#endif
