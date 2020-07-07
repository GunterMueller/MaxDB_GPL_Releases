

      tgg01_ContainerId = tgg00_FileId;
 
      tgg01_OmsVersionContext = RECORD
            ovc_tree          : tgg00_FileId;
            ovc_index_tree    : tgg00_FileId;
            ovc_key           : tsp00_Int4;
            ovc_trans_version : tgg91_TransNo;
      END;
 
 
      tgg01_OmsLockInfo = RECORD (* PTS 1110315 *)
            oli_handle      : tsp00_C8;
            oli_lockmode    : tsp00_C18;
            oli_requestmode : tsp00_C18;
            oli_taskid      : tsp00_TaskId;
            oli_timeout     : tsp00_Int4;
      END;
 
      tgg01_COMMonEnum = (
            cmiRunTime,
            cmiRunTimeMin, (* PTS 1000230, T.A. 1999-07-08 *)
            cmiRunTimeMax,
            cmiOmsRead,
            cmiLCRead,
            cmiLCKeyRead,
            cmiOmsStore,
            cmiLCStore,
            cmiOmsDelete,
            cmiLCDelete,
            cmiLCLock,
            cmiLCIter,     (* PTS 1107819 T.A 2000-09-13 *)
            cmiLogHop,
            cmiLogHopIter, (* PTS 1107819 T.A 2000-09-13 *)
            cmiExceptions,
            cmiOutOfDate,
            cmiTimeout,
            cmiOutOfMemory, (* PTS 1108772 T.A. 2000-12-13 *)
            cmiSubtransRollback,
            cmiSubtransCommit,
            cmiMaxSubtransLevel,
            cmiLoadVarObj,
            cmiStoreVarObj,
            cmiSumVarObjSize,
            cmiCntVarObj,
            cmiCreateVersion, (* PTS 1000247, T.A. 1999-07-08 *)
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
            cmiOmsTerminate, (* PTS 1112796, T.A. *)
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
            cmiOmsReadCacheHit,     (* PTS 1117571 *)
            cmiOmsReadCacheMiss,    (* PTS 1117571 *)
            cmiOmsReadVersion,      (* PTS 1117571 *)
            cmiOmsMaxHashChainLen,  (* PTS 1118855 *)
            cmiOmsRehash,           (* PTS 1118855 *)
            cmiCntWaitOmsLockObj,
            cmiWaitOmsLockObjSecs,
            cmiHashSumChainAccessed, (* PTS 1133314 *)
            cmiHashDerefCount,       (* PTS 1133314 *)
            cmiOmsReleaseCalled,     (* PTS 1133314 *)
            cmiOmsReleaseDone,       (* PTS 1133314 *) 
            cmiCacheSize);
 
      tgg01_COMMonitorInfo = RECORD
            cmiMethodName_gg00  : tsp00_KnlIdentifier;
            cmiCallCnt_gg00     : tsp00_8ByteCounter;
            cmiCounters_gg00    : ARRAY[tgg01_COMMonEnum] OF tsp00_8ByteCounter;
      END;
 
 
      tgg01_COMMonitorTransferBuf = RECORD
            ctbMessType_gg00 : tgg00_MessType;
            ctbDispId_gg00   : tsp00_Int4;
            ctbMonInfo_gg00  : tgg01_COMMonitorInfo;
      END;
      
      tgg01_OneStreamDescriptor = RECORD
            osdHandle             : tgg00_VoidPtr;
            osdBufPtr             : tgg00_VoidPtr;
            osdHiddenDesc         : tgg00_VoidPtr;
            osdStreamId           : tsp00_Int4;
            osdRowCount           : tsp00_Int4;
            osdRowSize            : tsp00_Int4;
            osdBufSize            : tsp00_Int4;
            osdHiddenDescSize     : tsp00_Int4;
            osdEndOfStream        : boolean;
            osdResetStream        : boolean; (* PTS 1131114, PG *)
            osdDefaultCompression : boolean; (* PTS 1135058, PG *)
      end;
      
      tgg01_StreamDescriptor = RECORD
            sdCount                  : tsp00_Int4;
            sdFiller0                : boolean; (* PTS 1135058, PG *)
            sdFiller1                : boolean;
            sdFiller2                : tsp00_Int2;
            sdDesc                   : ARRAY[1..MAX_STREAM_DESC_gg01] OF tgg01_OneStreamDescriptor;
      end;
       
 
