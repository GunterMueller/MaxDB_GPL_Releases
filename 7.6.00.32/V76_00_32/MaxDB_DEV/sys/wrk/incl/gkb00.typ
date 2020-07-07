
      tkb00_ConfigPhase = (
            cfpNone_ekb00,          (* after  activation (normal)*)
            cfpStartConfig_ekb00,   (* before init config        *)
            cfpConfigured_ekb00,    (* after  init config        *)
            cfpFiller1_ekb00,
            cfpFiller2_ekb00,
            cfpFiller3_ekb00,
            cfpFiller4_ekb00);
      (* *)
 
      tkb00_Check = RECORD                    (* PTS 1103957 JA 1999-09-15 *)
            chkBackup_kb00     : boolean;
            chkLock_kb00       : boolean;
            chkLockSupply_kb00 : boolean;
            chkFiller1_kb00    : boolean;
            chkFiller2_kb00    : boolean;
            chkRegion_kb00     : boolean;
            chkTrans_kb00      : boolean;
            chkFiller3_kb00    : boolean;
      END;
 
 
      tkb00_SaveptParam = RECORD
            svpId_kb00               : tsp00_Uint4;    (* SAPDB_UInt4 *)
            svpReason_kb00           : tsp00_Uint1;    (* Log_SavepointReason *)
            svpFiller1_kb00          : boolean;
            svpOpenTransCnt_kb00     : tsp00_Uint2;    (* SAPDB_UInt2 *)
            (* *)
            svpIOsequence_kb00       : tsp00_Uint4;    (* Log_IOSequenceNo *)
            svpStartOffset_kb00      : tsp00_Uint4;    (* Log_RawDeviceOffset *)
            (* *)
            svpStartEntryOffset_kb00 : tsp00_Uint2;    (* Data_EntryOffset *)
            svpStartEntryType_kb00   : tsp00_Uint1;    (* Log_ActionType   *)
            svpFiller2_kb00          : boolean;
            svpFiller3_kb00          : tsp00_Int4;
            (* *)
            svpOpenTransRoot_kb00    : tsp00_PageNo;   (* Data_PageNo *)
            svpHistoryRoot_kb00      : tsp00_PageNo;   (* Data_PageNo *)
            (* *)
            svpDate_kb00             : tsp00_Uint4;    (* SAPDB_UInt4 *)
            svpTime_kb00             : tsp00_Uint4;    (* SAPDB_UInt4 *)
            (* *)
            svpStartEntryDate_kb00   : tsp00_Uint4;    (* Kernel_Date *)
            svpStartEntryTime_kb00   : tsp00_Uint4;    (* Kernel_Time *)
            (* *)
            svpFiller4_kb00          : tsp00_C48;
            (* *)
            svpFiller5_kb00          : tsp00_Uint4;
            svpFiller6_kb00          : tsp00_Uint4;
      END;(*104*)
 
 
      tkb00_SurrogateParam = RECORD
            srgSession_kb00    : tgg91_SessionNo;
            srgFiller1_kb00    : tsp00_Int4;
            (* *)
            srgSurrogate_kb00  : tgg00_Surrogate;
            (* *)
            srgSyskey_kb00     : tgg00_Surrogate;
            (* *)
            srgWriteTrans_kb00 : tgg91_TransNo;                      (* PTS 1108234 JA 2000-12-13 *)
            srgFiller2_kb00    : tsp00_Int2
      END;
 
 
      tkb00_LogPageHeader = RECORD
            logHeader_kb00   : tgg00_PageHeader;
            logPposFree_kb00 : tgg00_PagePos;
      END;
 
 
      tkb00_ConfigRestartParam = RECORD
            crSnapShotRestartRecordAddr_kb00     : tsp00_Uint4;
            crConvRootBlockAddr_kb00             : tsp00_Uint4;
            (* *)
            crFdirRoot_kb00                      : tsp00_PageNo;
            crLongDirRoot_kb00                   : tsp00_PageNo;
            (* *)
            crMaxDynamicPno_kb00                 : tsp00_PageNo;
            crMaxStaticPno_kb00                  : tsp00_PageNo;
            (* *)
            crPrevDataRecoveryWasIncomplete_kb00 : boolean; (* PTS 1124994 UH 2004-01-26 *)
            crVolumeBitCount_kb00                : tsp00_Uint1;
            crClusterBlockCount_kb00             : tsp00_Uint2;
            crFdirEntryCount_kb00                : tsp00_Int4;
            (* *)
            crClusterReservedSpace_kb00          : tsp00_Uint4;
            crFiller3_kb00                       : tsp00_Int4;
            (* *)
            crReleaseVers_kb00                   : tsp00_Version;
      END;
 
 
      tkb00_Page = RECORD
            CASE integer OF
                1:
                    (pageLpno_kb00            : tsp00_PageNo;
                    pageType_kb00             : tgg00_PageType;
                    pageType2_kb00            : tgg00_PageType2;
                    pageCheck_kb00            : tgg00_PageCheckType;
                    pageMode_kb00             : tgg00_PageMode);
                2:
                    (pageHeader_kb00          : tgg00_PageHeader;
                    pagePposFree_kb00         : tgg00_PagePos;
                    pageLogBody_kb00          : tgg00_LogBody;
                    pageChecksum_kb00         : tsp00_Int4;
                    pageTrailer_kb00          : tgg00_PageTrailer);
                3:
                    (pageLogHeader_kb00       : tkb00_LogPageHeader;
                    pageFiller1_kb00          : tgg00_LogBody;
                    pageCheckTrailer_kb00     : tgg00_PageCheckTrailer);
                4:
                    (pageBuf_kb00             : tsp00_Page);
                5:
                    (pageLogPos_kb00               : tgg00_LogPos;
                    rstFiller1_kb00                : tsp00_Int4;
                    (* *)
                    rstFiller2_kb00                : boolean;
                    rstConfigPhase_kb00            : tkb00_ConfigPhase;
                    rstSetEndReadOnly_kb00         : boolean;
                    rstLastSaveDataSuccessful_kb00 : boolean;
                    rstFiller3_kb00                : tsp00_Int4;
                    (* *)
                    rstConverterVersion_kb00  : tsp00_Int4;
                    rstPrevConverterVers_kb00 : tsp00_Int4;
                    (* *)
                    rstCurrBackupVersion_kb00 : tsp00_Int4;
                    rstPrevBackupVersion_kb00 : tsp00_Int4;
                    (* *)
                    rstFiller4_kb00           : tsp00_Int4;
                    rstFiller5_kb00           : tsp00_Int4;
                    (* *)
                    rstConfigParam_kb00       : tkb00_ConfigRestartParam;
                    rstFurtherUse2a_kb00      : tsp00_C70;
                    rstFurtherUse2b_kb00      : tsp00_C2;
                    (* *)
                    rstLastSavept_kb00        : tkb00_SaveptParam;
                    rstFurtherUse2_kb00       : tsp00_C64;
                    (* *)
                    rstLastSurrogate_kb00     : tkb00_SurrogateParam;
                    rstLastFileId_kb00        : tgg00_Surrogate;
                    rstSys1Catalog_kb00       : tgg00_Surrogate;
                    rstSys2Catalog_kb00       : tgg00_Surrogate;
                    rstFurtherUse3_kb00       : tsp00_C48;
                    (* *)
                    rstFurtherUse5_kb00       : tsp00_C70;
                    rstFurtherUse6_kb00       : tsp00_C70;
                    rstFiller6_kb00           : tsp00_Int4;
                    (* *)
                    rstDataBackupCnt_kb00     : tsp00_Int4;
                    rstLastDataBackup_kb00    : tsp00_PageNo;
                    (* *)
                    rstReleaseVersion_kb00    : tsp00_Version;
                    rstDbIdent_kb00           : tsp00_Line)
                END;
            (*ENDCASE*) 
 
      tkb00_PagePtr = ^tkb00_Page;
 
      tkb00_LogQueueInfo = RECORD
            lqiCountGroup_kb00      : tsp00_8ByteCounter;        (* PTS 1103743 JA 1999-08-25 *)
            lqiCountInsert_kb00     : tsp00_8ByteCounter;
            lqiCountRequest_kb00    : tsp00_8ByteCounter;
            lqiCountWait_kb00       : tsp00_8ByteCounter;
            lqiAvgSumPagewaits_kb00 : tsp00_8ByteCounter;
            (* *)
            lqiLogQueueSize_kb00    : tsp00_Int4;
            lqiMaxQueueUsed_kb00    : tsp00_Int4;
            (* *)
            lqiMaxWaitLogpage_kb00  : tsp00_Int4;
            lqiFiller_kb00          : tsp00_Int4
      END;
 
 
      tkb00_LogDeviceInfo = RECORD
            ldiPagesUsed_kb00     : tsp00_Int4;
            ldiPagesNotSaved_kb00 : tsp00_Int4;
            (* *)
            ldiPagesSinceBup_kb00 : tsp00_Int4;
            ldiSaveptCount_kb00   : tsp00_Int4;
      END;
 
