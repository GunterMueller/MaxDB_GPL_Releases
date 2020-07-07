
      tbd02_SyncWithSavepoint = (
            synNone_ebd02,
            synCheckPreventSync_ebd02,
            synWait_ebd02,
            synUpdateSyncCnt_ebd02);
      (* *)
 
      tbd02_CachePageState = RECORD
            cpsFound_bd02               : boolean;
            cpsDirty_bd02               : boolean;
            cpsWaitForWrite_bd02        : boolean;
            cpsWaitForSvpEnd_bd02       : boolean;
            cpsIoDone_bd02              : boolean;
            cpsHaveToWaitForLock_bd02   : boolean;
            cpsIntendToWaitForLock_bd02 : boolean;
            cpsTaskRescheduled_bd02     : boolean;
            cpsSyncWithSavepoint_bd02   : tbd02_SyncWithSavepoint
      END;
 
      (* *)
      tbd02_ChangedState = (
            cstNone_ebd02,
            cstChanged_ebd02,
            cstSvpRelevant_ebd02);
      (* *)
      tbd02_DataIOState = (
            iosNone_ebd02,
            iosUserIO_ebd02,
            iosServerIO_ebd02,
            iosBlocked_ebd02);
      (* *)
      tbd02_SwapState = (
            swsIoDone_ebd02,
            swsTestIo_ebd02,
            swsDoIo_ebd02);
      (* *)
 
      tbd02_DataCBlockInit = RECORD
            dcbi_lru_next             : tsp00_BufAddr;
            dcbi_lru_prev             : tsp00_BufAddr;
            dcbi_io_next              : tsp00_BufAddr;
            dcbi_io_prev              : tsp00_BufAddr;
            dcbi_queue_occ            : tsp00_BufAddr;
            dcbi_queue_old_occ        : tsp00_BufAddr;
            dcbi_lock_req_head        : tsp00_BufAddr;
            dcbi_lock_req_tail        : tsp00_BufAddr;
            dcbi_occupant             : tsp00_PageNo;
            dcbi_OldOccupant_bd02     : tsp00_PageNo;
            dcbi_LastOwner_bd02       : tsp00_TaskId;
            dcbi_UsageCnt_bd02        : tsp00_Int2;
            dcbi_tfn                  : tgg00_Tfn;
            dcbi_lru_rechain          : boolean;
            dcbi_ChangedState_bd02    : tbd02_ChangedState;
            dcbi_io_state             : tbd02_DataIOState;
            dcbi_SvpSyncCnt_bd02      : tsp00_Int2;
            dcbi_IsSourceOfACopy_bd02 : boolean;
            dcbi_copy_no              : tsp00_Uint1;
            dcbi_dw_io_area           : boolean;
            dcbi_IsClustered_bd02     : boolean;
            dcbi_AccessCnt_bd02       : tsp00_Uint4;
            dcbi_CollisionCnt_bd02    : tsp00_Uint4;
            dcbi_UpdateCnt_bd02       : tsp00_Uint4;
            dcbi_Filler3_bd02         : tsp00_Int2;
            dcbi_RecMode_bd02         : tgg00_RecoveryMode;
            dcbi_OldRecMode_bd02      : tgg00_RecoveryMode;
      END;
 
      tbd02_TaskQueueReason = (
            tqrNil_ebd02,
            tqrShareLock_ebd02,
            tqrExclusiveLock_ebd02);
      (* *)
      tbd02_pTaskQueueItem = ^tbd02_TaskQueueItem;
 
      tbd02_TaskQueueItem = RECORD
            tqiTaskId_bd02    : tsp00_TaskId;
            tqiReason_bd02    : tbd02_TaskQueueReason;
            tqiFiller1_bd02   : boolean;
            tqiFiller2_bd02   : tsp00_Int2;
            tqiFixedNext_bd02 : tbd02_pTaskQueueItem;
            tqiVarNext_bd02   : tbd02_pTaskQueueItem;
      END;
 
      tbd02_pDataCBlock = ^tbd02_DataCBlock;
 
      tbd02_DataCBlock = RECORD
            dcb_FixedNext_bd02   : tbd02_pDataCBlock;
            dcb_pFrame_bd02      : tbd_nodeptr;
            dcb_VarNext_bd02     : tbd02_pDataCBlock;
            CASE boolean OF
                false :
                    (dcb_lru_next            : tbd02_pDataCBlock;
                    dcb_lru_prev             : tbd02_pDataCBlock;
                    (* *)
                    dcb_io_next              : tbd02_pDataCBlock;
                    dcb_io_prev              : tbd02_pDataCBlock;
                    (* *)
                    dcb_queue_occ            : tbd02_pTaskQueueItem;
                    dcb_queue_old_occ        : tbd02_pTaskQueueItem;
                    (* *)
                    dcb_lock_req_head        : tbd02_pTaskQueueItem;
                    dcb_lock_req_tail        : tbd02_pTaskQueueItem;
                    (* *)
                    dcb_occupant             : tsp00_PageNo;
                    dcb_OldOccupant_bd02     : tsp00_PageNo;
                    (* *)
                    dcb_LastOwner_bd02       : tsp00_TaskId;
                    dcb_UsageCnt_bd02        : tsp00_Int2;
                    dcb_tfn                  : tgg00_Tfn;
                    dcb_lru_rechain          : boolean;
                    (* *)
                    dcb_ChangedState_bd02    : tbd02_ChangedState;
                    dcb_io_state             : tbd02_DataIOState;
                    dcb_SvpSyncCnt_bd02      : tsp00_Int2;
                    dcb_IsSourceOfACopy_bd02 : boolean;
                    dcb_copy_no              : tsp00_Uint1;
                    dcb_dw_io_area           : boolean;
                    dcb_IsClustered_bd02     : boolean;
                    (* *)
                    dcb_AccessCnt_bd02       : tsp00_Uint4;
                    dcb_CollisionCnt_bd02    : tsp00_Uint4;
                    (* *)
                    dcb_UpdateCnt_bd02       : tsp00_Uint4;
                    dcb_Filler3              : tsp00_Int2;
                    dcb_RecMode_bd02         : tgg00_RecoveryMode;
                    dcb_OldRecMode_bd02      : tgg00_RecoveryMode);
                true :
                    (dcb_Init_bd02 : tbd02_DataCBlockInit)
                END;
            (*ENDCASE*) 
 
      tbd02_pDataCBlockList = ARRAY [1..25000000] OF tbd02_pDataCBlock;
      tbd02_pDataCBlockListPtr = ^tbd02_pDataCBlockList;
      (* *)
      tbd2_data_cb_flush_list = ARRAY [1..MAX_VF_BUFFERS_BD00] OF tbd02_pDataCBlock;
 
