
      tgg11_SeqNo = tsp00_C8(*ptoc00Ignore*);
      (*inlineC typedef tsp00_Uint8 tgg11_SeqNo; *)
 
      tgg11_VtraceHeader = RECORD
            vhLen_gg11        : tsp00_Int2;
            vhLayer_gg11      : tgg00_Debug;
            vhType_gg11       : tgg00_VtraceType;
            vhSession_gg11    : tgg91_SessionNo;
            vhTrans_gg11      : tgg91_TransNo;
            vhTask_gg11       : tsp00_Int2;
            vhSequenceNo_gg11 : tgg11_SeqNo;
      END;
 
      tgg11_VtraceBody    = tsp00_C132;
      tgg11_VtraceBodyPtr = ^tgg11_VtraceBody;
      (* *)
      tgg11_BdTraceType = (
            bdtrNil_egg11,
            bdtrInt_egg11,
            bdtrAppendRoot_egg11,
            bdtrAppendTrans_egg11,
            bdtrEvalPages_egg11,
            bdtrFilename_egg11,
            bdtrKey_egg11,
            bdtrMerge_egg11,
            bdtrRename_egg11,
            bdtrRoot_egg11,
            bdtrTwoKeys_egg11,
            bdtrVerify_egg11,
            bdtrWriteStr_egg11);
 
      tgg11_BdIntTrace = RECORD
            bdiTrType_gg11 : tgg11_BdTraceType;
            bdiFiller_gg11 : boolean;
            bdiInt_gg11    : ARRAY [0..2] OF tsp00_Int4
      END;
 
 
      tgg11_BdAppendTransTrace = RECORD
            bdatTrType_gg11    : tgg11_BdTraceType;
            bdatFiller_gg11    : boolean;
            bdatLogType_gg11   : tgg00_MessType;
            bdatLogType2_gg11  : tgg00_MessType2;
            bdatLen_gg11       : tsp00_Int4;
            bdatTrans_gg11     : tgg91_TransNo;
            bdatFiller2_gg11   : tsp00_Int2
      END;
 
 
      tgg11_BdFilenameTrace = RECORD
            bdfTrType_gg11 : tgg11_BdTraceType;
            bdfFiller_gg11 : boolean;
            bdfFn_gg11     : tgg00_Filename
      END;
 
 
      tgg11_BdKeyTrace = RECORD
            bdkTrType_gg11 : tgg11_BdTraceType;
            bdkFiller_gg11 : boolean;
            bdkKeyLen_gg11 : tsp00_Int2;
            bdkKey_gg11    : tgg00_VtraceKey
      END;
 
 
      tgg11_BdMergeTrace = RECORD
            bdmTrType_gg11     : tgg11_BdTraceType;
            bdmIsStartKey_gg11 : boolean;
            bdmIsPrimStop_gg11 : boolean;
            bdmFiller_gg11     : boolean;
            bdmKeyLen_gg11     : ARRAY [0..1] OF tsp00_Int2;
            bdmKey_gg11        : ARRAY [0..1] OF tgg00_VtraceKey;
      END;
 
 
      tgg11_BdRenameTrace = RECORD
            bdrnTrType_gg11 : tgg11_BdTraceType;
            bdrnFiller_gg11 : boolean;
            bdrnOldFn_gg11  : tgg00_Filename;
            bdrnNewFn_gg11  : tgg00_Filename
      END;
 
 
      tgg11_BdRootTrace = RECORD
            bdrTrType_gg11 : tgg11_BdTraceType;
            bdrFiller_gg11 : boolean;
            bdrError_gg11  : tgg00_BasisError;
            bdrRoot_gg11   : tsp00_PageNo
      END;
 
 
      tgg11_BdAppendRootTrace = RECORD
            bdarRootTrace_gg11 : tgg11_BdRootTrace;
            bdarRigthmost_gg11 : tsp00_PageNo
      END;
 
 
      tgg11_BdEvalPagesTrace = RECORD
            bdeRootTrace_gg11  : tgg11_BdRootTrace;
            bdePageCnt_gg11    : tsp00_Int4;
            bdeRecordCnt_gg11  : tsp00_Int4;
            bdeAllPageCnt_gg11 : tsp00_Int4
      END;
 
 
      tgg11_BdTwoKeysTrace = RECORD
            bdtkTrType_gg11  : tgg11_BdTraceType;
            bdtkFiller_gg11  : boolean;
            bdtkKeyLen_gg11  : ARRAY [0..1] OF tsp00_Int2;
            bdtkKey_gg11     : ARRAY [0..1] OF tgg00_VtraceKey;
      END;
 
 
      tgg11_BdVerifyTrace = RECORD
            bdvRootTrace_gg11 : tgg11_BdRootTrace;
            bdvNumPages_gg11  : tsp00_Int4;
            bdvFn_gg11        : tgg00_Filename
      END;
 
 
      tgg11_BdWriteStrHeader = RECORD
            bdwhTrType_gg11 : tgg11_BdTraceType;
            bdwhFiller_gg11 : boolean;
            bdwhPos_gg11    : tsp00_Int4;
            bdwhLen_gg11    : tsp00_Int4
      END;
 
 
      tgg11_BdWriteStrTrace = RECORD
            bdwsHead_gg11 : tgg11_BdWriteStrHeader;
            bdwsStr_gg11  : tgg11_VtraceBody;    (* Note!   BdWriteStrTrace > tgg11_VtraceBody *)
      END;
 
 
      tgg11_BdIoVtrace = RECORD
            bdioDevType_gg11   : tsp00_Uint1;
            bdioPagetype_gg11  : tgg00_PageType;
            bdioPagetype2_gg11 : tgg00_PageType2;
            bdioBufNo_gg11     : tsp00_Uint1;
            bdioPno_gg11       : tsp00_PageNo;
            bdioDevOffset_gg11 : tsp00_Int4;
            bdioDevNo_gg11     : tsp00_Int2
      END;
 
 
      tgg11_KbEnterTrace = RECORD
            kbMessType_gg11  : tgg00_MessType;
            kbMessType2_gg11 : tgg00_MessType2;
            kbETFiller       : tsp00_Int2;         (* unused *)
            kbSubtrans_gg11  : tgg00_SubtransNo;   (* PTS 1109485 E.Z. *)
      END;
 
 
      tgg11_KbTrace = RECORD
            kbMessType_gg11  : tgg00_MessType;
            kbMessType2_gg11 : tgg00_MessType2;
            kbError_gg11     : tgg00_BasisError
      END;
 
 
      tgg11_KbConsistViewTrace = RECORD
            kbcvHead_gg11        : tgg11_KbTrace;
            kbcvConsistView_gg11 : tgg91_TransNo
      END;
 
 
      tgg11_KbDateTimeTrace = RECORD
            kbdtHead_gg11 : tgg11_KbTrace;
            kbdtDate_gg11 : tsp00_Int4;
            kbdtTime_gg11 : tsp00_Int4
      END;
 
 
      tgg11_KbLockTraceHead = RECORD
            kblHead_gg11     : tgg11_KbTrace;
            kblReqTask_gg11  : tsp00_TaskId;
            kblLockTask_gg11 : tsp00_TaskId;
            kblTabId_gg11    : tgg00_Surrogate;
            kblReqMode_gg11  : tgg00_LockReqMode;
            kblFiller_gg11   : boolean;
            kblKeyLen_gg11   : tsp00_Int2
      END;
 
 
      tgg11_KbLockTrace = RECORD
            kblcHead_gg11 : tgg11_KbLockTraceHead;
            kblcInfo_gg11 : tgg11_VtraceBody;            (* Note! tgg11_KbLockTrace > tgg11_VtraceBody *)
      END;
 
 
      tgg11_KbRedoTrace = RECORD
            kbrHead_gg11       : tgg11_KbTrace;
            kbrRedoLogPos_gg11 : tgg00_LogPos
      END;
 
 
      tgg11_KbTabIdTrace = RECORD
            kbtHead_gg11   : tgg11_KbEnterTrace;
            kbtTabId_gg11  : tgg00_Surrogate;
            kbtKeyLen_gg11 : tsp00_Int2;
        END;
 
 
      tgg11_KbTabKeyTrace = RECORD
            kbtkTabHead_gg11 : tgg11_KbTabIdTrace;
            kbtkKey_gg11     : tgg11_VtraceBody;    (* Note!   KbTabKeyTrace > tgg11_VtraceBody *)
      END;
 
 
      tgg11_KbTimeTrace = RECORD
            kbtmHead_gg11 : tgg11_KbTrace;
            kbtmSec_gg11  : tsp00_Int4;
            kbtmMsec_gg11 : tsp00_Int4
      END;
 
 
      tgg11_KbTimeoutTrace = RECORD
            kbtmoHead_gg11     : tgg11_KbTrace;
            kbtmoTask_gg11     : tsp00_TaskId;
            kbtmoSession_gg11  : tgg91_SessionNo;
            kbtmoTransId_gg11  : tgg91_TransNo;     
            kbtmoFiller        : tsp00_Int2;       (* PTS 1109485 E.Z. *)
            kbtmoSubtrans_gg11 : tgg00_SubtransNo  (* PTS 1109485 E.Z. *)
      END;
 
      tgg11_JoinTraceType = (
            jntrCtor_gg11,
            jntrDtor_gg11,
            jntrOpen_gg11,
            jntrOpen1_gg11,
            jntrClose_gg11,
            jntrSort_gg11,
            jntrLegacy_gg11
      );
      
      tgg11_JoinTabIdTrace = RECORD
            jtHead_gg11   : tgg11_KbTrace;
            jtType_gg11   : tgg11_JoinTraceType;
            jtTabId_gg11  : tgg00_Surrogate;
            jtTabno_gg11  : tsp00_Int2;
      END;
 
