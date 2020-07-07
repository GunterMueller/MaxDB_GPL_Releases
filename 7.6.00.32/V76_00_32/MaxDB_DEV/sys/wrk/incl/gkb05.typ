
&     ifdef __cplusplus
      tkb05_KernelOid = tgg92_KernelOid;
&     else
      tkb05_KernelOid = tgg00_OidSpace;
&     endif
      (* *)
      tkb05_RegionNo     = tsp00_Int2;                               (* PTS 1105492 JA 2000-03-15 *)
      (* *)
      tkb05_RowId           = PACKED ARRAY [1..ROWID_MXKB05          ] OF char;
      (* PTS 1121179 UH 2003-04-07 *)
      tkb05_RowIdFirstPart  = PACKED ARRAY [1..ROWID_FIRSTPART_MXKB05] OF char;
      (* *)
      tkb05_ShortKey = tsp00_C10;
      (* *)
      tkb05_Handling = SET OF (
            hdCheckCollision_ekb05,
            hdInsLock_ekb05,
            hdInsReq_ekb05,
            hdCheckOwnReq_ekb05,
            hdFiller4_ekb05,
            hdFiller5_ekb05,
            hdFiller6_ekb05,
            hdFiller7_ekb05);
 
      tkb05_WantedLock = RECORD
            wlTabId_kb05       : tgg00_Surrogate;
            (* *)
            wlMode_kb05        : tgg00_LockReqMode;
            wlState_kb05       : tgg00_LockReqState;
            wlKeyLen_kb05      : tsp00_Int2;
            wlRowWanted_kb05   : boolean;
            wlHandling_kb05    : tkb05_Handling;
            wlGrantedMode_kb05 : tgg00_LockReqMode;
            wlFiller1_kb05     : boolean;
            CASE integer OF
                1:
                    (wlRowId_kb05       : tkb05_RowId);
                2:
                    (wlShortKey_kb05    : tkb05_ShortKey);
                END;
            (*ENDCASE*) 
 
      tkb05_ItemKind = (
            ikNone_ekb05,
            ikFree_ekb05,
            ikRequest_ekb05,
            ikRowLock_ekb05,
            ikRowTrans_ekb05,
            ikRowId_ekb05,
            ikTabLock_ekb05,
            ikTabTrans_ekb05,
            ikConsistView_ekb05,
            ikOpenTrans_ekb05,
            ikHashOpenTrans_ekb05);
      (**)
      tkb05_LocklistState = (
            lsOpen_ekb05,
            lsClosed_ekb05,
            lsClosedWaitForCheckpt_ekb05);
      (**)
      tkb05_TransInfo = SET OF (
            tiAuxPermExists_ekb05,
            tiAuxTempExists_ekb05,
            tiCollision_ekb05,
            tiExclusiveUser_ekb05,
            tiUserTask_ekb05,
            tiPriority_ekb05,
            tiCancelled_ekb05);
      (**)
      tkb05_TransLockMode = (
            tlmFree_ekb05,
            tlmShare_ekb05,
            tlmExcl_ekb05,
            tlmEotExcl_ekb05);
      (**)
      tkb05_ReqPtr = ^tkb05_Req;
 
      tkb05_Req = RECORD
            rqItemKind_kb05      : tkb05_ItemKind;
            rqExtentItem_kb05    : boolean;                          (* PTS 1105492 JA 2000-03-15 *)
            rqChkOwnRegion_kb05  : tsp00_Uint1;
            rqChkTransIndex_kb05 : tsp00_Uint1;
            rqChkTaskId_kb05     : tsp00_TaskId;
            (* *)
            rqNextReq_kb05       : tkb05_ReqPtr;
            rqPrevReq_kb05       : tkb05_ReqPtr;
            (* *)
            rqLastReq_kb05       : tkb05_ReqPtr;
&           ifdef BIT64
&           else
            rqFillerAddr_kb05    : tsp00_Addr;                       (* PTS 1105492 JA 2000-03-15 *)
&           endif
            (* *)
            rqTransIndex_kb05    : tgg00_TransIndex;
            rqEotWanted_kb05     : boolean;
            rqReqMode_kb05       : tgg00_LockReqMode;
            rqLockMode_kb05      : tgg00_LockReqMode;
            rqTransLockMode_kb05 : tkb05_TransLockMode;
            rqFiller1_kb05       : tsp00_Int2;
            (* *)
            rqRowShareLocks_kb05 : tsp00_Int4;
            rqRowExclLocks_kb05  : tsp00_Int4
      END;
 
 
      tkb05_RowIdItem = RECORD
            riItemKind_kb05      : tkb05_ItemKind;
            riExtentItem_kb05    : boolean;                          (* PTS 1105492 JA 2000-03-15 *)
            riChkOwnRegion_kb05  : tsp00_Uint1;
            riChkTransIndex_kb05 : tsp00_Uint1;
            riChkTaskId_kb05     : tsp00_TaskId;
            (* *)
            riRowId_kb05         : tkb05_RowId
      END;
 
      tkb05_RowIdItemPtr = ^tkb05_RowIdItem;
      (**)
      tkb05_RowLockPtr   = ^tkb05_RowLock;
 
      tkb05_RowLock = RECORD
            rlItemKind_kb05         : tkb05_ItemKind;
            rlExtentItem_kb05       : boolean;                       (* PTS 1105492 JA 2000-03-15 *)
            rlChkOwnRegion_kb05     : tsp00_Uint1;
            rlChkTransIndex_kb05    : tsp00_Uint1;
            rlChkTaskId_kb05        : tsp00_TaskId;
            (* *)
            rlTabId_kb05            : tgg00_Surrogate;
            (* *)
            rlNextHashRow_kb05      : tkb05_RowLockPtr;
            rlRowIdItem_kb05        : tkb05_RowIdItemPtr;
            (* *)
            rlFirstReqRow_kb05      : tkb05_ReqPtr;
&           ifdef BIT64
&           else
            rlFillerAddr_kb05       : tsp00_Addr;                    (* PTS 1105492 JA 2000-03-15 *)
&           endif
            (* *)
            rlShortKey_kb05         : tkb05_ShortKey;
            rlKeyLen_kb05           : tsp00_Int2;
            rlExclTransIndex_kb05   : tgg00_TransIndex;
            rlLockState_kb05        : tgg00_LockReqState;  (* PTS 1115302 UH 2002-05-02 *)
            rlFiller_kb05           : boolean;             (* PTS 1115302 UH 2002-05-02 *)
            (*                                 16 bytes *)
            rlRowShareLocks_kb05    : tsp00_Int4;
            rlRowOptimCnt_kb05      : tsp00_Int4;
            (* *)
            rlRowOptimVers_kb05     : tsp00_Int4;
            rlShare1TransIndex_kb05 : tgg00_TransIndex;
            rlShare2TransIndex_kb05 : tgg00_TransIndex
      END;
 
      tkb05_RowHash = ARRAY [1..MAX_ROW_HASH_KB05] OF tkb05_RowLockPtr;
      (**)
      tkb05_RowHashPtr = ^tkb05_RowHash;
      (**)
      tkb05_TabLockPtr = ^tkb05_TabLock;
 
      tkb05_TabLock = RECORD
            tlItemKind_kb05         : tkb05_ItemKind;
            tlExtentItem_kb05       : boolean;                       (* PTS 1105492 JA 2000-03-15 *)
            tlChkOwnRegion_kb05     : tsp00_Uint1;
            tlChkTransIndex_kb05    : tsp00_Uint1;
            tlChkTaskId_kb05        : tsp00_TaskId;
            (* *)
            tlTabId_kb05            : tgg00_Surrogate;  (* hashing like: HashOpenTrans, ConsistView*)
            (* *)
            tlNextHashTab_kb05      : tkb05_TabLockPtr; (* next hash:    HashOpenTrans, ConsistView*)
            tlFirstReqTab_kb05      : tkb05_ReqPtr;
            (* *)
            tlFirstReqRow_kb05      : tkb05_ReqPtr;
&           ifdef BIT64
&           else
            tlFillerAddr_kb05       : tsp00_Addr;                    (* PTS 1105492 JA 2000-03-15 *)
&           endif
            (* *)
            tlIsSysExcl_kb05        : boolean;
            tlFiller1_kb05          : boolean;
            tlExclTransIndex_kb05   : tgg00_TransIndex;
            tlShare1TransIndex_kb05 : tgg00_TransIndex;
            tlShare2TransIndex_kb05 : tgg00_TransIndex;
            (* *)
            tlSysShareLocks_kb05    : tsp00_Int4;
            tlTabShareLocks_kb05    : tsp00_Int4;
            (* *)
            tlTabOptimVers_kb05     : tsp00_Int4;
            tlRowShareLocks_kb05    : tsp00_Int4;
            (* *)
            tlRowOptimCnt_kb05      : tsp00_Int4;
            tlRowExclLocks_kb05     : tsp00_Int4
      END;
 
      tkb05_TabHash     = ARRAY [1..MAX_TAB_HASH_KB05] OF tkb05_TabLockPtr;
      (**)
      tkb05_TabHashPtr  = ^tkb05_TabHash;
      (**)
      tkb05_RowTransPtr = ^tkb05_RowTrans;
 
      tkb05_RowTrans = RECORD
            rtItemKind_kb05      : tkb05_ItemKind;
            rtExtentItem_kb05    : boolean;                          (* PTS 1105492 JA 2000-03-15 *)
            rtChkOwnRegion_kb05  : tsp00_Uint1;
            rtChkTransIndex_kb05 : tsp00_Uint1;
            rtChkTaskId_kb05     : tsp00_TaskId;
            (* *)
            rtRow_kb05           : tkb05_RowLockPtr;
            rtNextRow_kb05       : tkb05_RowTransPtr;
            (* *)
            rtPrevRow_kb05       : tkb05_RowTransPtr;
&           ifdef BIT64
&           else
            rtFillerAddr_kb05     : tsp00_Addr;                      (* PTS 1105492 JA 2000-03-15 *)
&           endif
            (* *)
            rtShortKey_kb05      : tkb05_ShortKey;
            rtKeyLen_kb05        : tsp00_Int2;
            rtLockMode_kb05      : tgg00_LockReqMode;
            rtLockState_kb05     : tgg00_LockReqState;
            rtReqMode_kb05       : tgg00_LockReqMode;
            rtReqState_kb05      : tgg00_LockReqState;
            (*                                        16 bytes *)
            rtRowRegion_kb05     : tkb05_RegionNo;
            rtSharedLocked_kb05  : boolean;                          (* PTS 1107452 JA 2000-08-08 *)
            rtOptimSet_kb05      : boolean;                          (* PTS 1107452 JA 2000-08-08 *)
            rtRowHash_kb05       : tsp00_Int4;
            (* *)
            rtRowOptimVers_kb05  : tsp00_Int4;
            rtTabOptimVers_kb05  : tsp00_Int4
      END;
 
      tkb05_TabTransPtr = ^tkb05_TabTrans;
 
      tkb05_TabTrans = RECORD
            ttItemKind_kb05      : tkb05_ItemKind;
            ttExtentItem_kb05    : boolean;                          (* PTS 1105492 JA 2000-03-15 *)
            ttChkOwnRegion_kb05  : tsp00_Uint1;
            ttChkTransIndex_kb05 : tsp00_Uint1;
            ttChkTaskId_kb05     : tsp00_TaskId;
            (* *)
            ttTabId_kb05         : tgg00_Surrogate;
            (* *)
            ttTab_kb05           : tkb05_TabLockPtr;
            ttFirstRow_kb05      : tkb05_RowTransPtr;
            (* *)
            ttConsistentRow_kb05 : tkb05_RowTransPtr;
            ttNextTab_kb05       : tkb05_TabTransPtr;
            (* *)
            ttLockMode_kb05      : tgg00_LockReqMode;
            ttLockState_kb05     : tgg00_LockReqState;
            ttReqMode_kb05       : tgg00_LockReqMode;
            ttReqState_kb05      : tgg00_LockReqState;
            ttKeySort_kb05       : boolean;                          (* PTS 1107056 JA 2001-03-01 *)
            ttFiller1_kb05       : boolean;                          (* PTS 1107056 JA 2001-03-01 *)
            ttTabRegion_kb05     : tkb05_RegionNo;
            (* *)
            ttTabHash_kb05       : tsp00_Int4;
            ttRowShareLocks_kb05 : tsp00_Int4;
            (* *)
            ttRowOptimCnt_kb05   : tsp00_Int4;
            ttRowExclLocks_kb05  : tsp00_Int4
      END;
 
 
      tkb05_SurrogateTransId = RECORD
            CASE integer OF
                1:
                    (surrId_kb05     : tgg00_Surrogate);
                2:
                    (surrSpace_kb05  : tsp00_Int2;
                    surrTransId_kb05 : tgg91_TransNo)
                END;
            (*ENDCASE*) 
 
      tkb05_HashOpenTransPtr = ^tkb05_HashOpenTrans;
 
      tkb05_HashOpenTrans = RECORD
            hshItemKind_kb05      : tkb05_ItemKind;
            hshExtentItem_kb05    : boolean;                         (* PTS 1105492 JA 2000-03-15 *)
            hshChkOwnRegion_kb05  : tsp00_Uint1;
            hshChkTransIndex_kb05 : tsp00_Uint1;
            hshChkTaskId_kb05     : tsp00_TaskId;
            (* *)
            hshTransId_kb05       : tkb05_SurrogateTransId;  (* hashing like: TabLock, ConsistView *)
            (* *)
            hshNextHashTab_kb05   : tkb05_TabLockPtr;        (* next hash:    TabLock, ConsistView *)
&           ifdef BIT64
&           else
            hshFillerAddr_kb05    : tsp00_Addr;                      (* PTS 1105492 JA 2000-03-15 *)
&           endif
            (* *)
            hshUsedCnt_kb05       : tsp00_Int4;
            hshOwnHashValue_kb05  : tsp00_Int4;                      (* PTS 1105492 JA 2000-03-15 *)
            hshNonVersUsedCnt_kb05: tsp00_Int4;                      (* PTS 1130354 FF 2004-06-28 *)
      END;
 
      tkb05_OpenTransPtr = ^tkb05_OpenTrans;
 
      tkb05_OpenTrans = RECORD                                       (* PTS 1105492 JA 2000-03-15 *)
            openItemKind_kb05      : tkb05_ItemKind;
            openExtentItem_kb05    : boolean;
            openChkOwnRegion_kb05  : tsp00_Uint1;
            openChkTransIndex_kb05 : tsp00_Uint1;
            openChkTaskId_kb05     : tsp00_TaskId;
            (* *)
            openNext_kb05          : tkb05_OpenTransPtr;
            openOwnHashTrans_kb05  : ARRAY [1..TRANS_PER_OPEN_ENTRY_KB05] OF tkb05_HashOpenTransPtr;
            (*                                                                        16/32 bytes *)
            openTransId_kb05       : ARRAY [1..TRANS_PER_OPEN_ENTRY_KB05] OF tgg91_TransNo;
            openHashRegion_kb05    : ARRAY [1..TRANS_PER_OPEN_ENTRY_KB05] OF tkb05_RegionNo;
            (*                                                                           24 bytes *)
            openCount_kb05         : tsp00_Int4; (* 1-3 open trans *)
            openFiller1_kb05       : tsp00_Int4
      END;
 
      tkb05_ConsistViewPtr = ^tkb05_ConsistView;
 
      tkb05_ConsistView = RECORD
            cstItemKind_kb05        : tkb05_ItemKind;
            cstExtentItem_kb05      : boolean;                       (* PTS 1105492 JA 2000-03-15 *)
            cstChkOwnRegion_kb05    : tsp00_Uint1;
            cstChkTransIndex_kb05   : tsp00_Uint1;
            cstChkTaskId_kb05       : tsp00_TaskId;
            (* *)
            cstConsistViewNo_kb05   : tkb05_SurrogateTransId;(*hashing like: TabLock, HashOpenTrans*)
            (* *)
            cstNextHashTab_kb05     : tkb05_TabLockPtr;      (*next hash:    TabLock, HashOpenTrans*)
            cstNextConsistView_kb05 : tkb05_ConsistViewPtr;
            (* *)
            cstPrevConsistView_kb05 : tkb05_ConsistViewPtr;
            cstFirstOpenTrans_kb05  : tkb05_OpenTransPtr;
            (* *)
            cstStartDate_kb05       : tsp00_Int4;                    (* PTS 1105492 JA 2000-03-15 *)
            cstStartTime_kb05       : tsp00_Int4;
            (* *)
            cstTimeLastUsed_kb05    : tsp00_Int4;                    (* PTS 1105492 JA 2000-03-29 *)
            cstCancelled_kb05       : boolean;                       (* PTS 1120151 FF 2003-01-29 *)
            (* cstInWork_kb05 used to synchronize kb51CancelConsistTrans and kb51DelConsistTrans  *)
            cstInWork_kb05          : boolean;                       (* PTS 1120151 FF 2003-01-29 *)
            (* cstFiller1_kb05         : tsp00_Int2;                    PTS 1120151 FF 2003-01-29 *)
            cstCreatorTransId_kb05  : tgg91_TransNo;                 (* PTS 1125503 FF 2003-11-19 *)
            (* *)
            cstMinOpenTransId_kb05  : tgg91_TransNo;
            cstCurrTransIndex_kb05  : tgg00_TransIndex;
            cstStartClockstamp_kb05 : tsp00_Int4;                    (* PTS 1122577 FF 2003-06-24 *)
            cstOmsVersion_kb05      : boolean;                       (* PTS 1130354 FF 2004-06-25 *)
            cstFiller1_kb05         : boolean;                       (* PTS 1130354 FF 2004-06-25 *)
            cstFiller2_kb05         : tsp00_Uint2;                   (* PTS 1130354 FF 2004-06-25 *)
      END;
 
      tkb05_SupplyItemPtr = ^tkb05_SupplyItem;
 
      tkb05_SupplyItem = RECORD                                      (* PTS 1105492 JA 2000-03-15 *)
            CASE integer OF
                1:
                    (siItemKind_kb05      : tkb05_ItemKind;
                    siExtentItem_kb05     : boolean;              (* to ensure correct item size *)
                    siChkOwnRegion_kb05   : tsp00_Uint1;          (* SupplyItem contains         *)
                    siChkTransIndex_kb05  : tsp00_Uint1;          (* all kinds of items          *)
                    siChkTaskId_kb05      : tsp00_TaskId;
                    siNext_kb05           : tkb05_SupplyItemPtr);
                2:
                    (siConsistView_kb05   : tkb05_ConsistView);
                3:
                    (siHashOpenTrans_kb05 : tkb05_HashOpenTrans);
                4:
                    (siOpenTrans_kb05     : tkb05_OpenTrans);
                5:
                    (siReq_kb05           : tkb05_Req);
                6:
                    (siRowIdItem_kb05     : tkb05_RowIdItem);
                7:
                    (siRowLock_kb05       : tkb05_RowLock);
                8:
                    (siRowTrans_kb05      : tkb05_RowTrans);
                9:
                    (siTabLock_kb05       : tkb05_TabLock);
                10:
                    (siTabTrans_kb05      : tkb05_TabTrans);
                END;
            (*ENDCASE*) 
 
      tkb05_SupplyBlock    = ARRAY [1..MAX_SUPPLY_BLOCK_ITEMS_KB05] OF tkb05_SupplyItem;
      (**)
      tkb05_SupplyBlockPtr = ^tkb05_SupplyBlock;                     (* PTS 1105492 JA 2000-03-15 *)
      (**)
      tkb05_SupplySegmPtr  = ^tkb05_SupplySegment;                   (* PTS 1105492 JA 2000-03-15 *)
 
      tkb05_SupplySegment = RECORD                                   (* PTS 1105492 JA 2000-03-15 *)
            sgmNext_kb05       : tkb05_SupplySegmPtr;
&           ifdef BIT64
&           else
            sgmFillerAddr_kb05 : tsp00_Addr;
&           endif
            sgmItemList_kb05   : ARRAY [1..ITEMS_PER_SUPPLY_SEGM_KB05] OF tkb05_SupplyItem;
      END;
 
      tkb05_SupplySegmPtrList    = ARRAY [1..MAX_SUPPLY_SEGMENTS_KB05] OF tkb05_SupplySegmPtr;
      (**)
      tkb05_SupplySegmPtrListPtr = ^tkb05_SupplySegmPtrList;         (* PTS 1105492 JA 2000-03-15 *)
 
      tkb05_SupplyPoolGlob = RECORD                                  (* PTS 1105492 JA 2000-03-15 *)
            poolSupplySegmList_kb05   : tkb05_SupplySegmPtrListPtr;
            poolFirstFreeSegm_kb05    : tkb05_SupplySegmPtr;
            (* *)
            poolFreeSegments_kb05     : tsp00_Int4;         (* all elements: lockpool region *)
            poolMinFreeSegms_kb05     : tsp00_Int4;
            (* *)
            poolFiller1_kb05          : tsp00_Int4;
            poolMinOpenTransId_kb05   : tgg91_TransNo;
            poolMinConsistViewNo_kb05 : tgg91_TransNo;
            (*                                         16 bytes *)
            poolSegmsUsedAmount_kb05  : tsp00_8ByteCounter;          (* PTS 1103743 JA 1999-08-25*)
            (* *)
            poolItemsUsedAmount_kb05  : tsp00_8ByteCounter;          (* PTS 1103743 JA 1999-08-25*)
            (* *)
            poolAmountPoints_kb05     : tsp00_8ByteCounter;          (* PTS 1103743 JA 1999-08-25*)
            (* *)
            poolEmptyWarning_kb05     : boolean;                    (* read outside region *)
            poolFiller2_kb05          : boolean;
            poolMinOpenIdx_kb05       : tsp00_Int2;					(* PTS 1139774 FF 2006-01-18*)
            poolOwnRegionId_kb05      : tsp00_RegionId;
            poolMinNonVersOpTrId_kb05 : tgg91_TransNo;              (* PTS 1130354 FF 2004-06-28*)
            poolMinNonVersCSVNo_kb05  : tgg91_TransNo;              (* PTS 1130354 FF 2004-06-28*)
            poolFiller4_kb05          : tsp00_Int4;
      END;
 
      tkb05_SupplyPoolGlobPtr = ^tkb05_SupplyPoolGlob;               (* PTS 1105492 JA 2000-03-15 *)
 
      tkb05_UnivItemPtr = RECORD                                     (* PTS 1105492 JA 2000-03-15 *)
            CASE integer OF
                1:
                    (uiConsistView_kb05   : tkb05_ConsistViewPtr);
                2:
                    (uiHashOpenTrans_kb05 : tkb05_HashOpenTransPtr);
                3:
                    (uiOpenTrans_kb05     : tkb05_OpenTransPtr);
                4:
                    (uiReq_kb05           : tkb05_ReqPtr);
                5:
                    (uiRowIdItem_kb05     : tkb05_RowIdItemPtr);
                6:
                    (uiRowLock_kb05       : tkb05_RowLockPtr);
                7:
                    (uiRowTrans_kb05      : tkb05_RowTransPtr);
                8:
                    (uiSupplyItem_kb05    : tkb05_SupplyItemPtr);
                9:
                    (uiTabLock_kb05       : tkb05_TabLockPtr);
                10:
                    (uiTabTrans_kb05      : tkb05_TabTransPtr);
                END;
            (*ENDCASE*) 
 
 
      tkb05_TransLogging = RECORD                                    (* PTS 1105492 JA 2000-03-15 *)
            tlogAuxFnNo_kb05          : tsp00_Int4;        (* outside region *)
            tlogTempFunctRollb_kb05   : tsp00_Int4;        (* outside region *)
            (* *)
            tlogTempEntryCnt_kb05     : tsp00_Int4;        (* outside region *)
            tlogTempFileRoot_kb05     : tsp00_PageNo;      (* outside region *)
            (* *)
            tlogTransPtr_kb05         : tsp00_Addr;        (* outside region - Logging74 UH *)
            tFillerAddr_kb05          : tsp00_Addr;        (* outside region - Logging74 UH *)
      END;
 
      tkb05_TransEntryPtr = ^tkb05_TransEntry;
 
      tkb05_TransEntry = RECORD
            teOwnAddr_kb05         : tkb05_TransEntryPtr;
            teNextHash_kb05        : tkb05_TransEntryPtr;
            (* *)
            tePrevHash_kb05        : tkb05_TransEntryPtr;
            teNextTrans_kb05       : tkb05_TransEntryPtr;
            (* *)
            tePrevTrans_kb05       : tkb05_TransEntryPtr;
            teFirstTab_kb05        : tkb05_TabTransPtr;
            (* *)
            teTabRecentlyUsed_kb05 : tkb05_TabTransPtr;
            teRowRecentlyUsed_kb05 : tkb05_RowTransPtr;
            (* *)
            teReqTab_kb05          : tkb05_TabTransPtr;
            teReqRow_kb05          : tkb05_RowTransPtr;
            (* *)
            teConsistView_kb05     : tkb05_ConsistViewPtr;
            teCancelAddr_kb05      : tsp00_RteCommAddr;
            (* *)
            teOwnIndex_kb05        : tgg00_TransIndex;    (* static *)
            teOwnRegion_kb05       : tkb05_RegionNo;      (* static *)
            teSessionId_kb05       : tgg91_SessionNo;
            (* *)
            teSelectTransId_kb05   : tgg91_TransNo;                  (* PTS 1108234 JA 2001-01-19 *)
            teWriteTransId_kb05    : tgg91_TransNo;                  (* PTS 1108234 JA 2000-12-13 *)
            teSubtransId_kb05      : tgg00_SubtransNo;    (* PTS 1109485 E.Z. *)
            (* *)
            teUserId_kb05          : tgg00_UserId;
            teTermId_kb05          : tsp00_TermId;
            teTimeoutEnabled_kb05  : boolean;
            teFiller2_kb05         : boolean;
            (*                                             24 bytes *)
            teTaskId_kb05          : tsp00_TaskId;
            teInfo_kb05            : tkb05_TransInfo;
            teLockMode_kb05        : tkb05_TransLockMode;
            teReqMode_kb05         : tkb05_TransLockMode;
            teFiller8_kb05         : boolean;
            (* *)
            teReqTimeout_kb05      : tsp00_Int4;
            teTimeLastWrite_kb05   : tsp00_Int4;
            (* *)
            teState_kb05           : tgg00_MessType;
            teObjLockMode_kb05     : tkb05_TransLockMode;
            teFiller3_kb05         : tsp00_Int2;                     (* PTS 1105435 JA 2000-02-01 *)
            teFiller4_kb05         : tsp00_Int4;                     (* PTS 1105435 JA 2000-02-01 *)
            (* *)
            teStartDate_kb05       : tsp00_Int4;
            teStartTime_kb05       : tsp00_Int4;
            (* *)
            teFirstCollLock_kb05   : tgg00_TransIndex;
            teObjReq_kb05          : tgg00_TransIndex;
            teObjReqTimeout_kb05   : tsp00_Int4;
            (* *)
            teWaiting_kb05         : boolean;            (* lockwait region *)
            teOmsLockMode_kb05     : tkb05_TransLockMode;(* PTS 1131932 & 1131982 FF 14-OCT-2004 *)
            teFiller6_kb05         : tsp00_Int2;         (* lockwait region *)
            teFiller7_kb05         : tsp00_Int4;         (* lockwait region *)
            (* *)
            teLog_kb05             : tkb05_TransLogging; (* PTS 1105492 JA 2000-03-15 *)
            (* *)
            teTransStartTime_kb05  : tsp00_Longreal;     (* PTS 1131982 FF used as SAPDB_UInt8 C++ only *)
            (* *)
            teLocalFileDirPtr_kb05 : tsp00_Addr;         (* outside region / UH 2003-05-19 *)
            teShrLockListPtr_kb05  : tsp00_Addr;         (* outside region / FF 2004-01-16 *)
            (* *)
            teExtTransMonPtr_kb05  : tsp00_Addr;         (* PTS 1130965 FF 2004-08-12 *)
            teFiller9_kb05         : tsp00_Addr;
      END;
 
      tkb05_TransList       = ARRAY [1..MAX_TRANS_GG04] OF tkb05_TransEntry;
      (**)
      tkb05_TransPtrList    = ARRAY [1..MAX_TRANS_GG04] OF tkb05_TransEntryPtr;  (* JA 2000-03-15 *)
      (**)
      tkb05_TransListPtr    = ^tkb05_TransList;
      (**)
      tkb05_TransPtrListPtr = ^tkb05_TransPtrList;                   (* PTS 1105492 JA 2000-03-15 *)
 
      tkb05_TransGlob = RECORD
            trFirst_kb05           : tkb05_TransEntryPtr;
            trFirstFree_kb05       : tkb05_TransEntryPtr;
            (* *)
            trTransHash_kb05       : tkb05_TransPtrListPtr;
&           ifdef BIT64
&           else
            trFillerAddr_kb05      : tsp00_Addr;                     (* PTS 1105492 JA 2000-03-15 *)
&           endif
            trFreeEntries_kb05     : tsp00_Int2;
            trConnectOnlyCnt_kb05  : tsp00_Int2;
            trLockTransCnt_kb05    : tsp00_Int2;
            trReqTransCnt_kb05     : tsp00_Int2;
            (* *)
            trObjLockTransCnt_kb05 : tsp00_Int2;
            trOwnRegion_kb05       : tkb05_RegionNo;                 (* PTS 1105492 JA 2000-03-15 *)
            trFiller1_kb05         : tsp00_Int4;
            (* *)
            trRowEntryAmount_kb05  : tsp00_8ByteCounter;             (* PTS 1103743 JA 1999-08-25 *)
            (* *)
            trTabEntryAmount_kb05  : tsp00_8ByteCounter;             (* PTS 1103743 JA 1999-08-25 *)
            (* *)
            trSqlReqTimeouts_kb05  : tsp00_8ByteCounter;             (* PTS 1103743 JA 1999-08-25 *)
            (* *)
            trObjReqTimeouts_kb05  : tsp00_8ByteCounter;             (* PTS 1103743 JA 1999-08-25 *)
            (* *)
            trDeadlocks_kb05       : tsp00_8ByteCounter;             (* PTS 1103743 JA 1999-08-25 *)
            (* *)
            trEscalations_kb05     : tsp00_8ByteCounter;             (* PTS 1103743 JA 1999-08-25 *)
      END;
 
      tkb05_TransGlobPtr  = ^tkb05_TransGlob;                        (* PTS 1105492 JA 2000-03-15 *)
 
      tkb05_RegionGlob = RECORD                                      (* PTS 1105492 JA 2000-03-15 *)
            rglTransGlob_kb05        : tkb05_TransGlobPtr;
            rglSupplyBlock_kb05      : tkb05_SupplyBlockPtr;
            (* *)
            rglRowHash_kb05          : tkb05_RowHashPtr;
            rglTabHash_kb05          : tkb05_TabHashPtr;
            (* *)
            rglFirstConsistView_kb05 : tkb05_ConsistViewPtr;
            rglFirstFree_kb05        : tkb05_SupplyItemPtr;
            (* *)
            rglCancelConsView_kb05   : tkb05_ConsistViewPtr;         (* PTS 1120151 FF 2003-01-29 *)
&           ifdef BIT64
&           else
            glFillerAddr_kb05        : tsp00_Addr;
&           endif
            (* *)
            rglExtentFirstSegm_kb05  : tkb05_SupplySegmPtr;
            rglExtentFirstFree_kb05  : tkb05_SupplyItemPtr;
            (* *)
            rglOwnRegion_kb05        : tkb05_RegionNo;
            rglFiller1_kb05          : tsp00_Int2;
            rglFreeItems_kb05        : tsp00_Int4;
            (* *)
            rglExtentItems_kb05      : tsp00_Int4;
            rglExtentFreeItems_kb05  : tsp00_Int4;
            (* *)
            rglConsistViewCnt_kb05   : tsp00_Int4;
            rglCancelCsViewCnt_kb05  : tsp00_Int4;                   (* PTS 1120151 FF 2003-01-29 *)
            rglCollisions_kb05       : tsp00_8ByteCounter;           (* PTS 1103743 JA 1999-08-25 *)
            rglOpenTransCnt_kb05     : tsp00_Int4;
            (* *)
            rglMinOpenTransId_kb05   : tgg91_TransNo;
            rglMinConsistViewNo_kb05 : tgg91_TransNo;
            rglMinNonVersOpTrId_kb05 : tgg91_TransNo;                (* PTS 1130354 FF 2004-06-28*)
            rglMinNonVersCSVNo_kb05  : tgg91_TransNo;                (* PTS 1130354 FF 2004-06-28*)
            rglFiller2_kb05          : tsp00_Int4;
      END;
 
      tkb05_RegionGlobPtr     = ^tkb05_RegionGlob;                   (* PTS 1105492 JA 2000-03-15 *)
      (**)
      tkb05_RgnGlobPtrList    = ARRAY [1..MAX_REGIONS_KB05] OF tkb05_RegionGlobPtr; (* 2000-03-15 *)
      (**)
      tkb05_RgnGlobPtrListPtr = ^tkb05_RgnGlobPtrList;               (* PTS 1105492 JA 2000-03-15 *)
 
      tkb05_LockStateGlob = RECORD                                   (* PTS 1105492 JA 2000-03-15 *)
            lglTimeLastClear_kb05 : tsp00_Int4;
            lglOwnRegionId_kb05   : tsp00_RegionId;
      END;
 
      tkb05_LockStateGlobPtr = ^tkb05_LockStateGlob;                 (* PTS 1105492 JA 2000-03-15 *)
 
      tkb05_ObjCollisionEntry = RECORD                               (* PTS 1105492 JA 2000-03-15 *)
            objcOwnIndex_kb05       : tgg00_TransIndex;
            objcFirstReq_kb05       : tgg00_TransIndex;
            objcNextCollLock_kb05   : tgg00_TransIndex;
            objcPrevCollLock_kb05   : tgg00_TransIndex;    (* all elements: first row region *)
            (* *)
            objcLockTransId_kb05    : tgg91_TransNo;
            objcLockTransIndex_kb05 : tgg00_TransIndex;
            (* *)
            objcOid_kb05            : tkb05_KernelOid;
            objcObjContHndl_kb05    : tsp00_C8                           (* PTS 1124356 FF 2003-10-01 *)
      END;
 
      tkb05_ObjCollEntryPtr = ^tkb05_ObjCollisionEntry;
      (**)
      tkb05_ObjCollList     = ARRAY [1..MAX_TRANS_GG04] OF tkb05_ObjCollisionEntry;
      (**)
      tkb05_ObjCollListPtr  = ^tkb05_ObjCollList;
 
      tkb05_ObjReqEntry = RECORD                                     (* PTS 1105492 JA 2000-03-15 *)
            objrReqTransId_kb05     : tgg91_TransNo;
            objrOwnIndex_kb05       : tgg00_TransIndex;
            (* *)
            objrCollEntryIndex_kb05 : tgg00_TransIndex;      (* all elements: first row region *)
            objrNextReq_kb05        : tgg00_TransIndex;
            objrWaitTaskId_kb05     : tsp00_TaskId;
            objrIgnCnstViewCheck    : boolean;               (* PTS 1128244 FF 2004-03-18 *)
            objrOmsShareLockReq     : boolean                (* PTS 1135178 FF 2005-04-25 *)
      END;
 
      tkb05_ObjReqEntryPtr = ^tkb05_ObjReqEntry;
      (**)
      tkb05_ObjReqList     = ARRAY [1..MAX_TRANS_GG04] OF tkb05_ObjReqEntry;     (* JA 2000-03-15 *)
      (**)
      tkb05_ObjReqListPtr  = ^tkb05_ObjReqList;                      (* PTS 1105492 JA 2000-03-15 *)
 
      tkb05_ObjCollGlob = RECORD                                     (* PTS 1105492 JA 2000-03-15 *)
            oglObjCollList_kb05      : tkb05_ObjCollListPtr;
            oglObjReqList_kb05       : tkb05_ObjReqListPtr;
            (* *)
            oglObjCollFirstFree_kb05 : tgg00_TransIndex;
            oglObjCollFreeCnt_kb05   : tsp00_Int2;
            oglObjReqCnt_kb05        : tsp00_Int2;
            oglFiller1_kb05          : tsp00_Int2;
            (* *)
            oglFiller2_kb05          : tsp00_Int4;
            oglOwnRegionId_kb05      : tsp00_RegionId;
            (* *)
            oglCollisions_kb05       : tsp00_8ByteCounter
      END;
 
      tkb05_ObjCollGlobPtr = ^tkb05_ObjCollGlob;                     (* PTS 1105492 JA 2000-03-15 *)
 
      tkb05_Glob = RECORD                                            (* PTS 1105492 JA 2000-03-15 *)
            glRegionGlob_kb05                : tkb05_RgnGlobPtrListPtr;
            glSupplyPoolGlob_kb05            : tkb05_SupplyPoolGlobPtr;
            (* *)
            glLockStateGlob_kb05             : tkb05_LockStateGlobPtr;
            glObjCollGlob_kb05               : tkb05_ObjCollGlobPtr;
            (* *)
            glTransList_kb05                 : tkb05_TransPtrListPtr;
&           ifdef BIT64
&           else
            glFillerAddr_kb05                : tsp00_Addr;
&           endif
            (* *)
            glGenerated_kb05                 : boolean;
            glHistoryNeeded_kb05             : boolean;              (* PTS 1140529 FF 2006-03-09 *)
            glRegions_kb05                   : tsp00_Int2;
            glRowRegions_kb05                : tsp00_Int2;
            glTabRegions_kb05                : tsp00_Int2;
            (* *)
            glTransRegions_kb05              : tsp00_Int2;
            glTransEntries_kb05              : tsp00_Int2;
            glTransEntriesPerRegion_kb05     : tsp00_Int2;
            glTransHashEntriesPerRegion_kb05 : tsp00_Int2;
            (* *)
            glFiller2_kb05                   : tsp00_Int4;
            glDeadlockDetection_kb05         : tsp00_Int4;
            (* *)
            glItems_kb05                     : tsp00_Int4;
            glMaxRowsPerTab_kb05             : tsp00_Int4;
            (* *)
            glSupplyItemsPerRegion_kb05      : tsp00_Int4;
            glMaxItemsPerRegion_kb05         : tsp00_Int4;
            (* *)
            glSupplyPoolSegments_kb05        : tsp00_Int4;
            glSupplyPoolItems_kb05           : tsp00_Int4;
            (* *)
            glRowHashEntriesPerRegion_kb05   : tsp00_Int4;
            glTabHashEntriesPerRegion_kb05   : tsp00_Int4;
            (* *)
            glSelectTransGeneration_kb05     : tgg91_TransNo;        (* PTS 1108234 JA 2000-12-13 *)
            glFiller3_kb05                   : tsp00_Int2;           (* PTS 1108234 JA 2000-12-13 *)
            (* *)
            glNilWantedLock_kb05             : tkb05_WantedLock;
            glNilOpenTrans_kb05              : tkb05_OpenTrans;
            glNilReq_kb05                    : tkb05_Req;
            glNilRowLock_kb05                : tkb05_RowLock;
            glNilRowTrans_kb05               : tkb05_RowTrans;
            glNilTabLock_kb05                : tkb05_TabLock;
            glNilTabTrans_kb05               : tkb05_TabTrans;
            glNilTransEntry_kb05             : tkb05_TransEntry
      END;
 
 
      tkb05_UnivPtr = RECORD                                         (* PTS 1105492 JA 2000-03-15 *)
            CASE integer OF
                1:
                    (uvAddr_kb05              : tsp00_ObjAddr);
                2:
                    (uvLockStateGlob_kb05     : tkb05_LockStateGlobPtr);
                3:
                    (uvObjCollGlob_kb05       : tkb05_ObjCollGlobPtr);
                4:
                    (uvObjCollList_kb05       : tkb05_ObjCollListPtr);
                5:
                    (uvObjReqList_kb05        : tkb05_ObjReqListPtr);
                6:
                    (uvRegionGlob_kb05        : tkb05_RegionGlobPtr);
                7:
                    (uvRgnGlobPtrList_kb05    : tkb05_RgnGlobPtrListPtr);
                8:
                    (uvRowHash_kb05           : tkb05_RowHashPtr);
                9:
                    (uvSupplyBlock_kb05       : tkb05_SupplyBlockPtr);
                10:
                    (uvSupplyPoolGlob_kb05    : tkb05_SupplyPoolGlobPtr);
                11:
                    (uvSupplySegm_kb05        : tkb05_SupplySegmPtr);
                12:
                    (uvSupplySegmPtrList_kb05 : tkb05_SupplySegmPtrListPtr);
                13:
                    (uvTabHash_kb05           : tkb05_TabHashPtr);
                14:
                    (uvTransGlob_kb05         : tkb05_TransGlobPtr);
                15:
                    (uvTransList_kb05         : tkb05_TransListPtr);
                16:
                    (uvTransPtrList_kb05      : tkb05_TransPtrListPtr)
                END;
            (*ENDCASE*) 
 
 
      tkb05_ConsistViewShow = RECORD                                (* PTS 1105492 JA 2000-03-15 *)
            cshRecLen_kb05       : tsp00_Int2;
            cshKeyLen_kb05       : tsp00_Int2;
            cshVarcolOffset_kb05 : tsp00_Int2;
            cshVarcolCnt_kb05    : tsp00_Int2;
            cshKey_kb05          : tsp00_Int4;
            (* *)
            cshCreatorTrans_kb05 : tgg91_TransNo;                   (* PTS 1125503 FF 2003-11-19 *)
            cshConsistView_kb05  : tgg91_TransNo;
            cshOpenTrans_kb05    : tgg91_TransNo;
            cshStartDate_kb05    : tsp00_Int4;
            cshStartTime_kb05    : tsp00_Int4;
            cshTimeLastUsed_kb05 : tsp00_Int4;
            cshCancelled_kb05    : boolean;                         (* PTS 1120151 FF 2003-01-29 *)
            cstFiller1_kb05      : boolean
      END;
 
 
      tkb05_TransShow = RECORD
            tshRecLen_kb05       : tsp00_Int2;
            tshKeyLen_kb05       : tsp00_Int2;
            tshVarcolOffset_kb05 : tsp00_Int2;
            tshVarcolCnt_kb05    : tsp00_Int2;
            tshKey_kb05          : tsp00_Int4;
            (* *)
            tshTaskId_kb05       : tsp00_TaskId;
            tshSession_kb05      : tgg91_SessionNo;
            (* *)
            tshWriteTransId_kb05 : tgg91_TransNo;
            tsh_filler1_kb05     : tsp00_Int2;            (* unused *)
            tshSubtrans_kb05     : tgg00_SubtransNo;      (* PTS 1109485 E.Z. *)
            tsh_filler2_kb05     : tsp00_Int4;            (* unused *)
            (* *)
            tshStartDate_kb05    : tsp00_Int4;
            tshStartTime_kb05    : tsp00_Int4;
            (* *)
            tshSelTransId_kb05   : tgg91_TransNo;                    (* PTS 1108234 JA 2001-01-19 *)
            tshTermId_kb05       : tsp00_TermId;
            (*                                          24 bytes *)
            tshUserId_kb05       : tgg00_UserId;
            tshReqTimeout_kb05   : tsp00_Int4;
            (* *)
            tshTimeWrite_kb05    : tsp00_Int4;
            tshTrLockMode_kb05   : tkb05_TransLockMode;
            tshTrReqMode_kb05    : tkb05_TransLockMode;
            tshLockMode_kb05     : tgg00_LockReqMode;
            tshReqMode_kb05      : tgg00_LockReqMode;
            (* *)
            tshLockState_kb05    : tgg00_LockReqState;
            tshReqState_kb05     : tgg00_LockReqState;
            tshConsistView_kb05  : tgg91_TransNo                     (* PTS 1105492 JA 2000-03-15 *)
      END;
 
 
      tkb05_LockShow = RECORD
            lshTransShow_kb05   : tkb05_TransShow;
            lshTabId_kb05       : tgg00_Surrogate;
            lshRowId_kb05       : tkb05_RowId;
            lshRowIdLen_kb05    : tsp00_Int2;
            lshUndefRowId_kb05  : boolean;
            lshFiller1_kb05     : boolean
      END;
 
 
      tkb05_ShowRec = RECORD
            CASE integer OF
                1:
                    (shrConsist_kb05 : tkb05_ConsistViewShow);       (* PTS 1105492 JA 2000-03-15 *)
                2:
                    (shrLock_kb05    : tkb05_LockShow);
                3:
                    (shrTrans_kb05   : tkb05_TransShow);
                4:
                    (shrRec_kb05     : tgg00_Rec)
                END;
            (*ENDCASE*) 
 
      tkb05_ShtHeader      = tsp00_C12;
      tkb05_ShtInfo        = PACKED ARRAY [1..256] OF char;
      tkb05_ShtDesc        = tsp00_C40;
      tkb05_ShtDescUnicode = tsp00_C80;
      tkb05_ShtVal         = tsp00_C12;
      tkb05_ShtValUnicode  = tsp00_C24;
 
      tkb05_ShowStatRec = RECORD
            CASE integer OF
                1:
                    (shtRecLen_kb05         : tsp00_Int2;
                    shtKeyLen_kb05          : tsp00_Int2;
                    shtVarcolOffset_kb05    : tsp00_Int2;
                    shtVarcolCnt_kb05       : tsp00_Int2;
                    shtKey_kb05             : tsp00_C4;
                    shtDefByte_kb05         : tsp00_C1;
                    shtInfo_kb05            : tkb05_ShtInfo);
                2:
                    (shtHeader_kb05         : tkb05_ShtHeader;
                    shtSpace1_kb05          : tsp00_C1;
                    shtDesc_kb05            : tkb05_ShtDesc;
                    shtDefByte2_kb05        : tsp00_C1;
                    shtVal_kb05             : tkb05_ShtVal);
                3:
                    (shtSpace2_kb05         : tkb05_ShtHeader;
                    shtSpace3_kb05          : tsp00_C1;
                    shtDescUnicode_kb05     : tkb05_ShtDescUnicode;
                    shtDefByte2Unicode_kb05 : tsp00_C1;
                    shtValUnicode_kb05      : tkb05_ShtValUnicode)
                END;
            (*ENDCASE*) 
 
 
      tkb05_LockInfo = RECORD
            infSqlCollisions_kb05  : tsp00_8ByteCounter;         (* PTS 1103743 JA 1999-08-25 *)
            infOmsCollisions_kb05  : tsp00_8ByteCounter;         (* PTS 1105492 JA 2000-03-15 *)
            infEscalations_kb05    : tsp00_8ByteCounter;
            infDeadlocks_kb05      : tsp00_8ByteCounter;
            infSqlReqTimeouts_kb05 : tsp00_8ByteCounter;
            infObjReqTimeouts_kb05 : tsp00_8ByteCounter;
            infRowEntryAmount_kb05 : tsp00_8ByteCounter;
            infTabEntryAmount_kb05 : tsp00_8ByteCounter;
            infKbCalls_kb05        : tsp00_8ByteCounter;
            (* *)
            infLockTrans_kb05      : tsp00_Int4;
            infReqTrans_kb05       : tsp00_Int4;
            (* *)
            infEscalValue_kb05     : tsp00_Int4;
            infObjLockTrans_kb05   : tsp00_Int4;
            (* *)
            infMaxEntries_kb05     : tsp00_Int4;
            infEntriesUsed_kb05    : tsp00_Int4;
            (* *)
            infAvgUsed_kb05        : tsp00_Int4;
            infMaxUsed_kb05        : tsp00_Int4;
            (* *)
            infConsistViewCnt_kb05 : tsp00_Int4;
            infOpenTransCnt_kb05   : tsp00_Int4;
      END;
 
      (* following entries are used for object share locks   PTS 1128244 FF 2004-03-18 *)
      tkb05_ObjShareLockPtr = ^tkb05_ObjShareLock;
 
      tkb05_ObjShareLock = RECORD
            objrNxtShareLock_kb05     : tkb05_ObjShareLockPtr;
            objrSlockTransId_kb05     : tgg91_TransNo;
            objrSlockTransIndex_kb05  : tgg00_TransIndex;
            objrLockRollbacked        : boolean;
            objrFiller1_kb05          : tsp00_Uint1;
            objrFiller2_kb05          : tsp00_Int2;
            objrFiller3_kb05          : tsp00_Int2;
            objrOwnIndex_kb05         : tsp00_Int2;
      END;
 
      tkb05_ObjShareLockCtrlPtr = ^tkb05_ObjShareLockCtrl;
 
      tkb05_ObjShareLockCtrl = RECORD
            objsNxtShareLockCtrl_kb05   : tkb05_ObjShareLockCtrlPtr;
            objsObjShareLock_kb05       : tkb05_ObjShareLockPtr;
            objsOid_kb05                : tkb05_KernelOid;
            objsCollLock_kb05           : tgg00_TransIndex;
            objsExclLockTransIndex_kb05 : tgg00_TransIndex;
            objsExclLockTransId_kb05    : tgg91_TransNo;
            objsInuseWithoutLock_kb05   : tsp00_Int2;
            objsReleaseLocks_kb05       : boolean;
            objsFiller1_kb05            : tsp00_Uint1;
            objsOwnIndex_kb05           : tsp00_Int2;
            CASE integer OF
                1:
                    (objsFileDirFileId_kb05     : tsp00_C8);
                2:
                    (objsFileObjFileNo_kb05     : tgg00_ObjFileNo;
                    objsFiller2_kb05           : tsp00_Int4);
                END;
 
