
/* some common prolog for every module
 */

#ifndef GKB00_H
#define GKB00_H


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

#ifndef GGG04_H
#include "ggg04.h"
#endif

/******************************************
 *           C O N S T A N T S            *
 ******************************************/

#define MAX_SHORTCOL_LEN_KB00          (MAX_RECLEN_GG00 - cgg_rec_key_offset - SURROGATE_MXGG00)
#define INDEX_LOCK_BYTE_KB00                    1

/******************************************
 *               T Y P E S                *
 ******************************************/


typedef enum tkb00_ConfigPhase_Enum
{
    cfpNone_ekb00,
    cfpStartConfig_ekb00,
    cfpConfigured_ekb00,
    cfpFiller1_ekb00,
    cfpFiller2_ekb00,
    cfpFiller3_ekb00,
    cfpFiller4_ekb00
} tkb00_ConfigPhase_Enum;
typedef tkb00_ConfigPhase_Enum tkb00_ConfigPhase_Param;
typedef ENUM_TYPE_MSP00 (          tkb00_ConfigPhase, tkb00_ConfigPhase_Enum, cfpFiller4_ekb00, unsigned char);
    /* */

typedef struct                     tkb00_Check
{
    pasbool             chkBackup_kb00;
    pasbool             chkLock_kb00;
    pasbool             chkLockSupply_kb00;
    pasbool             chkFiller1_kb00;
    pasbool             chkFiller2_kb00;
    pasbool             chkRegion_kb00;
    pasbool             chkTrans_kb00;
    pasbool             chkFiller3_kb00;
} tkb00_Check;

typedef struct                     tkb00_SaveptParam
{
    tsp00_Uint4         svpId_kb00;
    tsp00_Uint1         svpReason_kb00;
    pasbool             svpFiller1_kb00;
    tsp00_Uint2         svpOpenTransCnt_kb00;
    tsp00_Uint4         svpIOsequence_kb00;
    tsp00_Uint4         svpStartOffset_kb00;
    tsp00_Uint2         svpStartEntryOffset_kb00;
    tsp00_Uint1         svpStartEntryType_kb00;
    pasbool             svpFiller2_kb00;
    tsp00_Int4          svpFiller3_kb00;
    tsp00_PageNo        svpOpenTransRoot_kb00;
    tsp00_PageNo        svpHistoryRoot_kb00;
    tsp00_Uint4         svpDate_kb00;
    tsp00_Uint4         svpTime_kb00;
    tsp00_Uint4         svpStartEntryDate_kb00;
    tsp00_Uint4         svpStartEntryTime_kb00;
    tsp00_C48           svpFiller4_kb00;
    tsp00_Uint4         svpFiller5_kb00;
    tsp00_Uint4         svpFiller6_kb00;
} tkb00_SaveptParam;

typedef struct                     tkb00_SurrogateParam
{
    tgg91_SessionNo     srgSession_kb00;
    tsp00_Int4          srgFiller1_kb00;
    tgg00_Surrogate     srgSurrogate_kb00;
    tgg00_Surrogate     srgSyskey_kb00;
    tgg91_TransNo       srgWriteTrans_kb00;
    tsp00_Int2          srgFiller2_kb00;
} tkb00_SurrogateParam;

typedef struct                     tkb00_LogPageHeader
{
    tgg00_PageHeader    logHeader_kb00;
    tgg00_PagePos       logPposFree_kb00;
} tkb00_LogPageHeader;

typedef struct                     tkb00_ConfigRestartParam
{
    tsp00_Uint4         crSnapShotRestartRecordAddr_kb00;
    tsp00_Uint4         crConvRootBlockAddr_kb00;
    tsp00_PageNo        crFdirRoot_kb00;
    tsp00_PageNo        crLongDirRoot_kb00;
    tsp00_PageNo        crMaxDynamicPno_kb00;
    tsp00_PageNo        crMaxStaticPno_kb00;
    pasbool             crPrevDataRecoveryWasIncomplete_kb00;
    tsp00_Uint1         crVolumeBitCount_kb00;
    tsp00_Uint2         crClusterBlockCount_kb00;
    tsp00_Int4          crFdirEntryCount_kb00;
    tsp00_Uint4         crClusterReservedSpace_kb00;
    tsp00_Int4          crFiller3_kb00;
    tsp00_Version       crReleaseVers_kb00;
} tkb00_ConfigRestartParam;

typedef struct                     tkb00_Page
{
    union
    {
    /* 1 */
        struct tkb00_Page_1
        {
            tsp00_PageNo        pageLpno_kb00_F;
            tgg00_PageType      pageType_kb00_F;
            tgg00_PageType2     pageType2_kb00_F;
            tgg00_PageCheckType pageCheck_kb00_F;
            tgg00_PageMode      pageMode_kb00_F;
        } C_1;
    /* 2 */
        struct tkb00_Page_2
        {
            tgg00_PageHeader    pageHeader_kb00_F;
            tgg00_PagePos       pagePposFree_kb00_F;
            tgg00_LogBody       pageLogBody_kb00_F;
            tsp00_Int4          pageChecksum_kb00_F;
            tgg00_PageTrailer   pageTrailer_kb00_F;
        } C_2;
    /* 3 */
        struct tkb00_Page_3
        {
            tkb00_LogPageHeader pageLogHeader_kb00_F;
            tgg00_LogBody       pageFiller1_kb00_F;
            tgg00_PageCheckTrailer pageCheckTrailer_kb00_F;
        } C_3;
    /* 4 */
        struct tkb00_Page_4
        {
            tsp00_Page          pageBuf_kb00_F;
        } C_4;
    /* 5 */
        struct tkb00_Page_5
        {
            tgg00_LogPos        pageLogPos_kb00_F;
            tsp00_Int4          rstFiller1_kb00_F;
            pasbool             rstFiller2_kb00_F;
            tkb00_ConfigPhase   rstConfigPhase_kb00_F;
            pasbool             rstSetEndReadOnly_kb00_F;
            pasbool             rstLastSaveDataSuccessful_kb00_F;
            tsp00_Int4          rstFiller3_kb00_F;
            tsp00_Int4          rstConverterVersion_kb00_F;
            tsp00_Int4          rstPrevConverterVers_kb00_F;
            tsp00_Int4          rstCurrBackupVersion_kb00_F;
            tsp00_Int4          rstPrevBackupVersion_kb00_F;
            tsp00_Int4          rstFiller4_kb00_F;
            tsp00_Int4          rstFiller5_kb00_F;
            tkb00_ConfigRestartParam rstConfigParam_kb00_F;
            tsp00_C70           rstFurtherUse2a_kb00_F;
            tsp00_C2            rstFurtherUse2b_kb00_F;
            tkb00_SaveptParam   rstLastSavept_kb00_F;
            tsp00_C64           rstFurtherUse2_kb00_F;
            tkb00_SurrogateParam rstLastSurrogate_kb00_F;
            tgg00_Surrogate     rstLastFileId_kb00_F;
            tgg00_Surrogate     rstSys1Catalog_kb00_F;
            tgg00_Surrogate     rstSys2Catalog_kb00_F;
            tsp00_C48           rstFurtherUse3_kb00_F;
            tsp00_C70           rstFurtherUse5_kb00_F;
            tsp00_C70           rstFurtherUse6_kb00_F;
            tsp00_Int4          rstFiller6_kb00_F;
            tsp00_Int4          rstDataBackupCnt_kb00_F;
            tsp00_PageNo        rstLastDataBackup_kb00_F;
            tsp00_Version       rstReleaseVersion_kb00_F;
            tsp00_Line          rstDbIdent_kb00_F;
        } C_5;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_PageNo       &pageLpno_kb00          ()  { return this->variant.C_1.pageLpno_kb00_F; };
    tgg00_PageType     &pageType_kb00          ()  { return this->variant.C_1.pageType_kb00_F; };
    tgg00_PageType2    &pageType2_kb00         ()  { return this->variant.C_1.pageType2_kb00_F; };
    tgg00_PageCheckType &pageCheck_kb00         ()  { return this->variant.C_1.pageCheck_kb00_F; };
    tgg00_PageMode     &pageMode_kb00          ()  { return this->variant.C_1.pageMode_kb00_F; };
    tgg00_PageHeader   &pageHeader_kb00        ()  { return this->variant.C_2.pageHeader_kb00_F; };
    tgg00_PagePos      &pagePposFree_kb00      ()  { return this->variant.C_2.pagePposFree_kb00_F; };
    tgg00_LogBody      &pageLogBody_kb00       ()  { return this->variant.C_2.pageLogBody_kb00_F; };
    tsp00_Int4         &pageChecksum_kb00      ()  { return this->variant.C_2.pageChecksum_kb00_F; };
    tgg00_PageTrailer  &pageTrailer_kb00       ()  { return this->variant.C_2.pageTrailer_kb00_F; };
    tkb00_LogPageHeader &pageLogHeader_kb00     ()  { return this->variant.C_3.pageLogHeader_kb00_F; };
    tgg00_LogBody      &pageFiller1_kb00       ()  { return this->variant.C_3.pageFiller1_kb00_F; };
    tgg00_PageCheckTrailer &pageCheckTrailer_kb00  ()  { return this->variant.C_3.pageCheckTrailer_kb00_F; };
    tsp00_Page         &pageBuf_kb00           ()  { return this->variant.C_4.pageBuf_kb00_F; };
    tgg00_LogPos       &pageLogPos_kb00        ()  { return this->variant.C_5.pageLogPos_kb00_F; };
    tsp00_Int4         &rstFiller1_kb00        ()  { return this->variant.C_5.rstFiller1_kb00_F; };
    pasbool            &rstFiller2_kb00        ()  { return this->variant.C_5.rstFiller2_kb00_F; };
    tkb00_ConfigPhase  &rstConfigPhase_kb00    ()  { return this->variant.C_5.rstConfigPhase_kb00_F; };
    pasbool            &rstSetEndReadOnly_kb00 ()  { return this->variant.C_5.rstSetEndReadOnly_kb00_F; };
    pasbool            &rstLastSaveDataSuccessful_kb00 ()  { return this->variant.C_5.rstLastSaveDataSuccessful_kb00_F; };
    tsp00_Int4         &rstFiller3_kb00        ()  { return this->variant.C_5.rstFiller3_kb00_F; };
    tsp00_Int4         &rstConverterVersion_kb00 ()  { return this->variant.C_5.rstConverterVersion_kb00_F; };
    tsp00_Int4         &rstPrevConverterVers_kb00 ()  { return this->variant.C_5.rstPrevConverterVers_kb00_F; };
    tsp00_Int4         &rstCurrBackupVersion_kb00 ()  { return this->variant.C_5.rstCurrBackupVersion_kb00_F; };
    tsp00_Int4         &rstPrevBackupVersion_kb00 ()  { return this->variant.C_5.rstPrevBackupVersion_kb00_F; };
    tsp00_Int4         &rstFiller4_kb00        ()  { return this->variant.C_5.rstFiller4_kb00_F; };
    tsp00_Int4         &rstFiller5_kb00        ()  { return this->variant.C_5.rstFiller5_kb00_F; };
    tkb00_ConfigRestartParam &rstConfigParam_kb00    ()  { return this->variant.C_5.rstConfigParam_kb00_F; };
    tsp00_C70          &rstFurtherUse2a_kb00   ()  { return this->variant.C_5.rstFurtherUse2a_kb00_F; };
    tsp00_C2           &rstFurtherUse2b_kb00   ()  { return this->variant.C_5.rstFurtherUse2b_kb00_F; };
    tkb00_SaveptParam  &rstLastSavept_kb00     ()  { return this->variant.C_5.rstLastSavept_kb00_F; };
    tsp00_C64          &rstFurtherUse2_kb00    ()  { return this->variant.C_5.rstFurtherUse2_kb00_F; };
    tkb00_SurrogateParam &rstLastSurrogate_kb00  ()  { return this->variant.C_5.rstLastSurrogate_kb00_F; };
    tgg00_Surrogate    &rstLastFileId_kb00     ()  { return this->variant.C_5.rstLastFileId_kb00_F; };
    tgg00_Surrogate    &rstSys1Catalog_kb00    ()  { return this->variant.C_5.rstSys1Catalog_kb00_F; };
    tgg00_Surrogate    &rstSys2Catalog_kb00    ()  { return this->variant.C_5.rstSys2Catalog_kb00_F; };
    tsp00_C48          &rstFurtherUse3_kb00    ()  { return this->variant.C_5.rstFurtherUse3_kb00_F; };
    tsp00_C70          &rstFurtherUse5_kb00    ()  { return this->variant.C_5.rstFurtherUse5_kb00_F; };
    tsp00_C70          &rstFurtherUse6_kb00    ()  { return this->variant.C_5.rstFurtherUse6_kb00_F; };
    tsp00_Int4         &rstFiller6_kb00        ()  { return this->variant.C_5.rstFiller6_kb00_F; };
    tsp00_Int4         &rstDataBackupCnt_kb00  ()  { return this->variant.C_5.rstDataBackupCnt_kb00_F; };
    tsp00_PageNo       &rstLastDataBackup_kb00 ()  { return this->variant.C_5.rstLastDataBackup_kb00_F; };
    tsp00_Version      &rstReleaseVersion_kb00 ()  { return this->variant.C_5.rstReleaseVersion_kb00_F; };
    tsp00_Line         &rstDbIdent_kb00        ()  { return this->variant.C_5.rstDbIdent_kb00_F; };
    /* const accessor functions */
    const tsp00_PageNo &pageLpno_kb00          () const  { return this->variant.C_1.pageLpno_kb00_F; };
    const tgg00_PageType &pageType_kb00          () const  { return this->variant.C_1.pageType_kb00_F; };
    const tgg00_PageType2 &pageType2_kb00         () const  { return this->variant.C_1.pageType2_kb00_F; };
    const tgg00_PageCheckType &pageCheck_kb00         () const  { return this->variant.C_1.pageCheck_kb00_F; };
    const tgg00_PageMode &pageMode_kb00          () const  { return this->variant.C_1.pageMode_kb00_F; };
    const tgg00_PageHeader &pageHeader_kb00        () const  { return this->variant.C_2.pageHeader_kb00_F; };
    const tgg00_PagePos &pagePposFree_kb00      () const  { return this->variant.C_2.pagePposFree_kb00_F; };
    const tgg00_LogBody &pageLogBody_kb00       () const  { return this->variant.C_2.pageLogBody_kb00_F; };
    const tsp00_Int4   &pageChecksum_kb00      () const  { return this->variant.C_2.pageChecksum_kb00_F; };
    const tgg00_PageTrailer &pageTrailer_kb00       () const  { return this->variant.C_2.pageTrailer_kb00_F; };
    const tkb00_LogPageHeader &pageLogHeader_kb00     () const  { return this->variant.C_3.pageLogHeader_kb00_F; };
    const tgg00_LogBody &pageFiller1_kb00       () const  { return this->variant.C_3.pageFiller1_kb00_F; };
    const tgg00_PageCheckTrailer &pageCheckTrailer_kb00  () const  { return this->variant.C_3.pageCheckTrailer_kb00_F; };
    const tsp00_Page   &pageBuf_kb00           () const  { return this->variant.C_4.pageBuf_kb00_F; };
    const tgg00_LogPos &pageLogPos_kb00        () const  { return this->variant.C_5.pageLogPos_kb00_F; };
    const tsp00_Int4   &rstFiller1_kb00        () const  { return this->variant.C_5.rstFiller1_kb00_F; };
    const pasbool      &rstFiller2_kb00        () const  { return this->variant.C_5.rstFiller2_kb00_F; };
    const tkb00_ConfigPhase &rstConfigPhase_kb00    () const  { return this->variant.C_5.rstConfigPhase_kb00_F; };
    const pasbool      &rstSetEndReadOnly_kb00 () const  { return this->variant.C_5.rstSetEndReadOnly_kb00_F; };
    const pasbool      &rstLastSaveDataSuccessful_kb00 () const  { return this->variant.C_5.rstLastSaveDataSuccessful_kb00_F; };
    const tsp00_Int4   &rstFiller3_kb00        () const  { return this->variant.C_5.rstFiller3_kb00_F; };
    const tsp00_Int4   &rstConverterVersion_kb00 () const  { return this->variant.C_5.rstConverterVersion_kb00_F; };
    const tsp00_Int4   &rstPrevConverterVers_kb00 () const  { return this->variant.C_5.rstPrevConverterVers_kb00_F; };
    const tsp00_Int4   &rstCurrBackupVersion_kb00 () const  { return this->variant.C_5.rstCurrBackupVersion_kb00_F; };
    const tsp00_Int4   &rstPrevBackupVersion_kb00 () const  { return this->variant.C_5.rstPrevBackupVersion_kb00_F; };
    const tsp00_Int4   &rstFiller4_kb00        () const  { return this->variant.C_5.rstFiller4_kb00_F; };
    const tsp00_Int4   &rstFiller5_kb00        () const  { return this->variant.C_5.rstFiller5_kb00_F; };
    const tkb00_ConfigRestartParam &rstConfigParam_kb00    () const  { return this->variant.C_5.rstConfigParam_kb00_F; };
    const tsp00_C70    &rstFurtherUse2a_kb00   () const  { return this->variant.C_5.rstFurtherUse2a_kb00_F; };
    const tsp00_C2     &rstFurtherUse2b_kb00   () const  { return this->variant.C_5.rstFurtherUse2b_kb00_F; };
    const tkb00_SaveptParam &rstLastSavept_kb00     () const  { return this->variant.C_5.rstLastSavept_kb00_F; };
    const tsp00_C64    &rstFurtherUse2_kb00    () const  { return this->variant.C_5.rstFurtherUse2_kb00_F; };
    const tkb00_SurrogateParam &rstLastSurrogate_kb00  () const  { return this->variant.C_5.rstLastSurrogate_kb00_F; };
    const tgg00_Surrogate &rstLastFileId_kb00     () const  { return this->variant.C_5.rstLastFileId_kb00_F; };
    const tgg00_Surrogate &rstSys1Catalog_kb00    () const  { return this->variant.C_5.rstSys1Catalog_kb00_F; };
    const tgg00_Surrogate &rstSys2Catalog_kb00    () const  { return this->variant.C_5.rstSys2Catalog_kb00_F; };
    const tsp00_C48    &rstFurtherUse3_kb00    () const  { return this->variant.C_5.rstFurtherUse3_kb00_F; };
    const tsp00_C70    &rstFurtherUse5_kb00    () const  { return this->variant.C_5.rstFurtherUse5_kb00_F; };
    const tsp00_C70    &rstFurtherUse6_kb00    () const  { return this->variant.C_5.rstFurtherUse6_kb00_F; };
    const tsp00_Int4   &rstFiller6_kb00        () const  { return this->variant.C_5.rstFiller6_kb00_F; };
    const tsp00_Int4   &rstDataBackupCnt_kb00  () const  { return this->variant.C_5.rstDataBackupCnt_kb00_F; };
    const tsp00_PageNo &rstLastDataBackup_kb00 () const  { return this->variant.C_5.rstLastDataBackup_kb00_F; };
    const tsp00_Version &rstReleaseVersion_kb00 () const  { return this->variant.C_5.rstReleaseVersion_kb00_F; };
    const tsp00_Line   &rstDbIdent_kb00        () const  { return this->variant.C_5.rstDbIdent_kb00_F; };
#endif    /* defined (__cplusplus) */
} tkb00_Page;
typedef tkb00_Page                *tkb00_PagePtr;

typedef struct                     tkb00_LogQueueInfo
{
    tsp00_8ByteCounter  lqiCountGroup_kb00;
    tsp00_8ByteCounter  lqiCountInsert_kb00;
    tsp00_8ByteCounter  lqiCountRequest_kb00;
    tsp00_8ByteCounter  lqiCountWait_kb00;
    tsp00_8ByteCounter  lqiAvgSumPagewaits_kb00;
    tsp00_Int4          lqiLogQueueSize_kb00;
    tsp00_Int4          lqiMaxQueueUsed_kb00;
    tsp00_Int4          lqiMaxWaitLogpage_kb00;
    tsp00_Int4          lqiFiller_kb00;
} tkb00_LogQueueInfo;

typedef struct                     tkb00_LogDeviceInfo
{
    tsp00_Int4          ldiPagesUsed_kb00;
    tsp00_Int4          ldiPagesNotSaved_kb00;
    tsp00_Int4          ldiPagesSinceBup_kb00;
    tsp00_Int4          ldiSaveptCount_kb00;
} tkb00_LogDeviceInfo;

struct tkb00_TemplateDummy {
    tkb00_ConfigPhase    dummy_tkb00_ConfigPhase;
};


#endif
