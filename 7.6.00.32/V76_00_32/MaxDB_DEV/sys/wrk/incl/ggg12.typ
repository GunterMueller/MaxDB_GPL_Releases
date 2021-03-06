
      (*inlineC #if defined __cplusplus *)
      (*inlineC #include "ggg92.h"      *)
      (*inlineC #endif                  *)
      (* *)
&     ifdef __cplusplus
      tgg12_KernelOid = tgg92_KernelOid;
&     else
      tgg12_KernelOid = tgg00_OidSpace;
&     endif
      (* *)
      tgg12_OmsTraceType = (
            otNil_egg12,
            otError_egg12,
            otFileInfo_egg12,
            otFilename_egg12,
            otGarbColl_egg12,
            otKey_egg12,
            otOid_egg12,
            otPageStat_egg12,
            otPageChainStat_egg12,
            otPageContainerStat_egg12,
            otVersionError_egg12,
            otRelOid_egg12,
            otTwoKeys_egg12);  (* PTS 1111934 AK 25/09/2001    *)
      (* *)
      tgg12_OmsTraceType2 = (
            ot2Nil_egg12,
            ot2Input_egg12,
            ot2InputVers_egg12,
            ot2InputSeq_egg12,
            ot2InputSeqVers_egg12,
            ot2Output_egg12,
            ot2OutputSeq_egg12);
      (* *)
      tgg12_RelObjStatus = (
            rosPageNotFound_gg12,
            rosFileAlreadyDeleted_gg12,
            (* PTS 1106724 Alexander Kley 23/05/2000 *)
            rosFileJustDeleted_gg12,
            rosWrongFileNo_gg12,
            rosWrongObjVersion_gg12,
            rosUnknownError_gg12,
            rosObjSucessfullyReleased_gg12,
            (* PTS 1106760  Alexander Kley 25/06/2000 *)
            rosDelContObjFailed_gg12,
            rosDelKeyFailed_gg12,
            rosWrongKeyInfo_gg12,
            rosOIdReused_gg12);
 
      tkb05_ObjFileno = RECORD
            CASE boolean OF
                false:
                    (ObjFileNo_I4_gg12  : tgg00_ObjFileNo);
                true:
                    (ObjFileNo_C8_gg12  : tsp00_C8);
                END;
            (*ENDCASE*) 
 
 
      tgg12_OmsErrorTrace = RECORD
            oerrMessType_gg12  : tgg00_MessType;
            oerrMessType2_gg12 : tgg00_MessType2;
            oerrTrType_gg12    : tgg12_OmsTraceType;
            oerrTrType2_gg12   : tgg12_OmsTraceType2;
            oerrRoot_gg12      : tsp00_PageNo;
            (**)
            oerrError_gg12     : tgg00_BasisError;
            oerr               : tkb05_ObjFileno;
      END;
 
 
      tgg12_OmsFileInfoTrace = RECORD
            oinfMessType_gg12            : tgg00_MessType;
            oinfMessType2_gg12           : tgg00_MessType2;
            oinfTrType_gg12              : tgg12_OmsTraceType;
            oinfTrType2_gg12             : tgg12_OmsTraceType2;
            oinfNumAllPages_gg12         : tsp00_PageNo;
            (**)
            oinfNumFreePages_gg12        : tsp00_PageNo;
            oinfNumEmptyPages_gg12       : tsp00_PageNo;
            (**)
            oinfNumEmptyPageLimit_gg12   : tsp00_Int4;
            oinfRoot_gg12                : tsp00_PageNo;
      END;
 
 
      tgg12_OmsFilenameTrace = RECORD
            ofnMessType_gg12  : tgg00_MessType;
            ofnMessType2_gg12 : tgg00_MessType2;
            ofnTrType_gg12    : tgg12_OmsTraceType;
            ofnTrType2_gg12   : tgg12_OmsTraceType2;
            (**)
            ofnFileName_gg12  : tgg00_Filename
      END;
 
 
      tgg12_OmsGarbCollTrace = RECORD
            ogrbMessType_gg12       : tgg00_MessType;
            ogrbMessType2_gg12      : tgg00_MessType2;
            ogrbTrType_gg12         : tgg12_OmsTraceType;
            ogrbTrType2_gg12        : tgg12_OmsTraceType2;
            ogrbIsActiv_gg12        : boolean;
            ogrbFiller1_gg12        : boolean;
            ogrbCollectorIndex_gg12 : tsp00_Int2
      END;
 
 
      tgg12_OmsPageContainerStatTrace = RECORD
            opcoMessType_gg12                   : tgg00_MessType;
            opcoMessType2_gg12                  : tgg00_MessType2;
            opcoTrType_gg12                     : tgg12_OmsTraceType;
            opcoTrType2_gg12                    : tgg12_OmsTraceType2;
            opcoNumSubRoots_gg12                : tsp00_PageNo;
            (**)
            opcoFreeObjCnt_gg12                 : tsp00_Int4;
            opcoFeotObjCnt_gg12                 : tsp00_Int4;
            (**)
            opcoOccObjCnt_gg12                  : tsp00_Int4;
            opcoResObjCnt_gg12                  : tsp00_Int4;
            (**)
            opcoAllPageCnt_gg12                 : tsp00_PageNo;
            opcoEmptyPageCnt_gg12               : tsp00_PageNo;
            (**)
            opcoFreePageCnt_gg12                : tsp00_PageNo;
            opcoFeotPageCnt_gg12                : tsp00_PageNo;
            (**)
            opcoIsFileDeleted_gg12              : boolean;
      END;
 
 
      tgg12_OmsPageChainStatTrace = RECORD
            opctMessType_gg12                   : tgg00_MessType;
            opctMessType2_gg12                  : tgg00_MessType2;
            opctTrType_gg12                     : tgg12_OmsTraceType;
            opctTrType2_gg12                    : tgg12_OmsTraceType2;
            opctSubRoot_gg12                    : tsp00_PageNo;
            (**)
            opctFreeObjCnt_gg12                 : tsp00_Int4;
            opctFeotObjCnt_gg12                 : tsp00_Int4;
            (**)
            opctOccObjCnt_gg12                  : tsp00_Int4;
            opctResObjCnt_gg12                  : tsp00_Int4;
            (**)
            opctAllPageCnt_gg12                 : tsp00_PageNo;
            opctAllPageRootCnt_gg12             : tsp00_PageNo;
            (**)
            opctEmptyPageCnt_gg12               : tsp00_PageNo;
            opctEmptyPageRootCnt_gg12           : tsp00_PageNo;
            (**)
            opctEmptyPageInFreeChainCnt_gg12    : tsp00_PageNo;
            opctEmtyPageWithEmptyLabelCnt_gg12  : tsp00_PageNo;
            (**)
            opctFreePageCnt_gg12                : tsp00_PageNo;
            opctFreePageRootCnt_gg12            : tsp00_PageNo;
            (**)
            opctFreePageInFreeChainCnt_gg12     : tsp00_PageNo;
            opctFeotPageCnt_gg12                : tsp00_PageNo;
            (**)
            opctPageInFreeChainCnt_gg12         : tsp00_PageNo;
            (**)
            opctPageWithEmptyLabelCnt_gg12      : tsp00_PageNo;
            opctPageWithFreeLabelCnt_gg12       : tsp00_PageNo;
            (**)
            opctPageWithFreeLabelInFreeChainCnt_gg12     : tsp00_PageNo;
            opctFreePageWithFreeLabelInFreeChainCnt_gg12 : tsp00_PageNo;
            (**)
            opctIsChainDeleted_gg12                      : boolean;
      END;
 
 
      tgg12_OmsTraceKeyHeader = RECORD
            okhMessType_gg12  : tgg00_MessType;
            okhMessType2_gg12 : tgg00_MessType2;
            okhTrType_gg12    : tgg12_OmsTraceType;
            okhTrType2_gg12   : tgg12_OmsTraceType2;
            okhKeyLen_gg12    : tsp00_Uint2;
            okh               : tkb05_ObjFileno;
            (**)
            okhConsView_gg12  : tgg91_TransNo;
            okhFiller2_gg12   : tsp00_Int2
      END;
 
 
      tgg12_OmsKeyTrace = RECORD
            okyHead_gg12 : tgg12_OmsTraceKeyHeader;
            (**)
            okyKey_gg12  : tsp00_Key
      END;
 
 
      tgg12_OmsOidTrace = RECORD
            ooidMessType_gg12  : tgg00_MessType;
            ooidMessType2_gg12 : tgg00_MessType2;
            ooidTrType_gg12    : tgg12_OmsTraceType;
            ooidTrType2_gg12   : tgg12_OmsTraceType2;
            ooidConsView_gg12  : tgg91_TransNo;
            ooidObjVers_gg12   : tgg91_PageRef;
            ooidObjId_gg12     : tgg12_KernelOid
      END;
 
 
      tgg12_OmsRelOidTrace = RECORD
            roidMessType_gg12     : tgg00_MessType;
            roidMessType2_gg12    : tgg00_MessType2;
            roidTrType_gg12       : tgg12_OmsTraceType;
            roidTrType2_gg12      : tgg12_OmsTraceType2;
            roidConsView_gg12     : tgg91_TransNo;
            roidObjVers_gg12      : tgg91_PageRef;
            roidObjId_gg12        : tgg12_KernelOid;
            roidRelObjStatus_gg12 : tgg12_RelObjStatus;
      END;
 
 
      tgg12_OmsPageStatTrace = RECORD
            opsMessType_gg12      : tgg00_MessType;
            opsMessType2_gg12     : tgg00_MessType2;
            opsTrType_gg12        : tgg12_OmsTraceType;
            opsTrType2_gg12       : tgg12_OmsTraceType2;
            opsPno_gg12           : tsp00_PageNo;
            (**)
            opsOccObjCnt_gg12     : tsp00_Int4;
            opsFreeObjCnt_gg12    : tsp00_Int4;
            (**)
            opsFreeEotObjCnt_gg12 : tsp00_Int4;
            opsResObjCnt_gg12     : tsp00_Int4
      END;
 
 
      tgg12_OmsVersionErrorTrace = RECORD
            oveMessType_gg12        : tgg00_MessType;
            oveMessType2_gg12       : tgg00_MessType2;
            oveTrType_gg12          : tgg12_OmsTraceType;
            oveTrType2_gg12         : tgg12_OmsTraceType2;
            oveObjVers_gg12         : tgg91_PageRef;
            oveWantedVers_gg12      : tgg91_PageRef;
            oveObjId_gg12           : tgg12_KernelOid;
            oveFrameVers_gg12       : tgg00_ObjFrameVers;
            ove                     : tkb05_ObjFileno;
      END;
 
 
      tgg12_OmsTwoKeyTrace = RECORD
            oskMessType_gg12        : tgg00_MessType;
            oskMessType2_gg12       : tgg00_MessType2;
            oskTrType_gg12          : tgg12_OmsTraceType;
            oskTrType2_gg12         : tgg12_OmsTraceType2;
            oskKeyLen_gg12          : tsp00_Int2;
            oskStartKey_gg12        : tgg00_VtraceKey;
            oskStopKey_gg12         : tgg00_VtraceKey;
      END;
 
