
/* some common prolog for every module
 */

#ifndef GGG11_H
#define GGG11_H


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


/******************************************
 *               T Y P E S                *
 ******************************************/

/* tgg11_SeqNo ignored */
     typedef tsp00_Uint8 tgg11_SeqNo;

typedef struct                     tgg11_VtraceHeader
{
    tsp00_Int2          vhLen_gg11;
    tgg00_Debug         vhLayer_gg11;
    tgg00_VtraceType    vhType_gg11;
    tgg91_SessionNo     vhSession_gg11;
    tgg91_TransNo       vhTrans_gg11;
    tsp00_Int2          vhTask_gg11;
    tgg11_SeqNo         vhSequenceNo_gg11;
} tgg11_VtraceHeader;
typedef tsp00_C132                 tgg11_VtraceBody;
typedef tgg11_VtraceBody          *tgg11_VtraceBodyPtr;
    /* */

typedef enum tgg11_BdTraceType_Enum
{
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
    bdtrWriteStr_egg11
} tgg11_BdTraceType_Enum;
typedef tgg11_BdTraceType_Enum tgg11_BdTraceType_Param;
typedef ENUM_TYPE_MSP00 (          tgg11_BdTraceType, tgg11_BdTraceType_Enum, bdtrWriteStr_egg11, unsigned char);

typedef struct                     tgg11_BdIntTrace
{
    tgg11_BdTraceType   bdiTrType_gg11;
    pasbool             bdiFiller_gg11;
    ARRAY_VAR_MSP00 (         bdiInt_gg11, tsp00_Int4, 2 - 0 + 1);
} tgg11_BdIntTrace;

typedef struct                     tgg11_BdAppendTransTrace
{
    tgg11_BdTraceType   bdatTrType_gg11;
    pasbool             bdatFiller_gg11;
    tgg00_MessType      bdatLogType_gg11;
    tgg00_MessType2     bdatLogType2_gg11;
    tsp00_Int4          bdatLen_gg11;
    tgg91_TransNo       bdatTrans_gg11;
    tsp00_Int2          bdatFiller2_gg11;
} tgg11_BdAppendTransTrace;

typedef struct                     tgg11_BdFilenameTrace
{
    tgg11_BdTraceType   bdfTrType_gg11;
    pasbool             bdfFiller_gg11;
    tgg00_Filename      bdfFn_gg11;
} tgg11_BdFilenameTrace;

typedef struct                     tgg11_BdKeyTrace
{
    tgg11_BdTraceType   bdkTrType_gg11;
    pasbool             bdkFiller_gg11;
    tsp00_Int2          bdkKeyLen_gg11;
    tgg00_VtraceKey     bdkKey_gg11;
} tgg11_BdKeyTrace;

typedef struct                     tgg11_BdMergeTrace
{
    tgg11_BdTraceType   bdmTrType_gg11;
    pasbool             bdmIsStartKey_gg11;
    pasbool             bdmIsPrimStop_gg11;
    pasbool             bdmFiller_gg11;
    ARRAY_VAR_MSP00 (         bdmKeyLen_gg11, tsp00_Int2, 1 - 0 + 1);
    ARRAY_VAR_MSP00 (         bdmKey_gg11, tgg00_VtraceKey, 1 - 0 + 1);
} tgg11_BdMergeTrace;

typedef struct                     tgg11_BdRenameTrace
{
    tgg11_BdTraceType   bdrnTrType_gg11;
    pasbool             bdrnFiller_gg11;
    tgg00_Filename      bdrnOldFn_gg11;
    tgg00_Filename      bdrnNewFn_gg11;
} tgg11_BdRenameTrace;

typedef struct                     tgg11_BdRootTrace
{
    tgg11_BdTraceType   bdrTrType_gg11;
    pasbool             bdrFiller_gg11;
    tgg00_BasisError    bdrError_gg11;
    tsp00_PageNo        bdrRoot_gg11;
} tgg11_BdRootTrace;

typedef struct                     tgg11_BdAppendRootTrace
{
    tgg11_BdRootTrace   bdarRootTrace_gg11;
    tsp00_PageNo        bdarRigthmost_gg11;
} tgg11_BdAppendRootTrace;

typedef struct                     tgg11_BdEvalPagesTrace
{
    tgg11_BdRootTrace   bdeRootTrace_gg11;
    tsp00_Int4          bdePageCnt_gg11;
    tsp00_Int4          bdeRecordCnt_gg11;
    tsp00_Int4          bdeAllPageCnt_gg11;
} tgg11_BdEvalPagesTrace;

typedef struct                     tgg11_BdTwoKeysTrace
{
    tgg11_BdTraceType   bdtkTrType_gg11;
    pasbool             bdtkFiller_gg11;
    ARRAY_VAR_MSP00 (         bdtkKeyLen_gg11, tsp00_Int2, 1 - 0 + 1);
    ARRAY_VAR_MSP00 (         bdtkKey_gg11, tgg00_VtraceKey, 1 - 0 + 1);
} tgg11_BdTwoKeysTrace;

typedef struct                     tgg11_BdVerifyTrace
{
    tgg11_BdRootTrace   bdvRootTrace_gg11;
    tsp00_Int4          bdvNumPages_gg11;
    tgg00_Filename      bdvFn_gg11;
} tgg11_BdVerifyTrace;

typedef struct                     tgg11_BdWriteStrHeader
{
    tgg11_BdTraceType   bdwhTrType_gg11;
    pasbool             bdwhFiller_gg11;
    tsp00_Int4          bdwhPos_gg11;
    tsp00_Int4          bdwhLen_gg11;
} tgg11_BdWriteStrHeader;

typedef struct                     tgg11_BdWriteStrTrace
{
    tgg11_BdWriteStrHeader bdwsHead_gg11;
    tgg11_VtraceBody    bdwsStr_gg11;
} tgg11_BdWriteStrTrace;

typedef struct                     tgg11_BdIoVtrace
{
    tsp00_Uint1         bdioDevType_gg11;
    tgg00_PageType      bdioPagetype_gg11;
    tgg00_PageType2     bdioPagetype2_gg11;
    tsp00_Uint1         bdioBufNo_gg11;
    tsp00_PageNo        bdioPno_gg11;
    tsp00_Int4          bdioDevOffset_gg11;
    tsp00_Int2          bdioDevNo_gg11;
} tgg11_BdIoVtrace;

typedef struct                     tgg11_KbEnterTrace
{
    tgg00_MessType      kbMessType_gg11;
    tgg00_MessType2     kbMessType2_gg11;
    tsp00_Int2          kbETFiller;
    tgg00_SubtransNo    kbSubtrans_gg11;
} tgg11_KbEnterTrace;

typedef struct                     tgg11_KbTrace
{
    tgg00_MessType      kbMessType_gg11;
    tgg00_MessType2     kbMessType2_gg11;
    tgg00_BasisError    kbError_gg11;
} tgg11_KbTrace;

typedef struct                     tgg11_KbConsistViewTrace
{
    tgg11_KbTrace       kbcvHead_gg11;
    tgg91_TransNo       kbcvConsistView_gg11;
} tgg11_KbConsistViewTrace;

typedef struct                     tgg11_KbDateTimeTrace
{
    tgg11_KbTrace       kbdtHead_gg11;
    tsp00_Int4          kbdtDate_gg11;
    tsp00_Int4          kbdtTime_gg11;
} tgg11_KbDateTimeTrace;

typedef struct                     tgg11_KbLockTraceHead
{
    tgg11_KbTrace       kblHead_gg11;
    tsp00_TaskId        kblReqTask_gg11;
    tsp00_TaskId        kblLockTask_gg11;
    tgg00_Surrogate     kblTabId_gg11;
    tgg00_LockReqMode   kblReqMode_gg11;
    pasbool             kblFiller_gg11;
    tsp00_Int2          kblKeyLen_gg11;
} tgg11_KbLockTraceHead;

typedef struct                     tgg11_KbLockTrace
{
    tgg11_KbLockTraceHead kblcHead_gg11;
    tgg11_VtraceBody    kblcInfo_gg11;
} tgg11_KbLockTrace;

typedef struct                     tgg11_KbRedoTrace
{
    tgg11_KbTrace       kbrHead_gg11;
    tgg00_LogPos        kbrRedoLogPos_gg11;
} tgg11_KbRedoTrace;

typedef struct                     tgg11_KbTabIdTrace
{
    tgg11_KbEnterTrace  kbtHead_gg11;
    tgg00_Surrogate     kbtTabId_gg11;
    tsp00_Int2          kbtKeyLen_gg11;
} tgg11_KbTabIdTrace;

typedef struct                     tgg11_KbTabKeyTrace
{
    tgg11_KbTabIdTrace  kbtkTabHead_gg11;
    tgg11_VtraceBody    kbtkKey_gg11;
} tgg11_KbTabKeyTrace;

typedef struct                     tgg11_KbTimeTrace
{
    tgg11_KbTrace       kbtmHead_gg11;
    tsp00_Int4          kbtmSec_gg11;
    tsp00_Int4          kbtmMsec_gg11;
} tgg11_KbTimeTrace;

typedef struct                     tgg11_KbTimeoutTrace
{
    tgg11_KbTrace       kbtmoHead_gg11;
    tsp00_TaskId        kbtmoTask_gg11;
    tgg91_SessionNo     kbtmoSession_gg11;
    tgg91_TransNo       kbtmoTransId_gg11;
    tsp00_Int2          kbtmoFiller;
    tgg00_SubtransNo    kbtmoSubtrans_gg11;
} tgg11_KbTimeoutTrace;

typedef enum tgg11_JoinTraceType_Enum
{
    jntrCtor_gg11,
    jntrDtor_gg11,
    jntrOpen_gg11,
    jntrOpen1_gg11,
    jntrClose_gg11,
    jntrSort_gg11,
    jntrLegacy_gg11
} tgg11_JoinTraceType_Enum;
typedef tgg11_JoinTraceType_Enum tgg11_JoinTraceType_Param;
typedef ENUM_TYPE_MSP00 (          tgg11_JoinTraceType, tgg11_JoinTraceType_Enum, jntrLegacy_gg11, unsigned char);

typedef struct                     tgg11_JoinTabIdTrace
{
    tgg11_KbTrace       jtHead_gg11;
    tgg11_JoinTraceType jtType_gg11;
    tgg00_Surrogate     jtTabId_gg11;
    tsp00_Int2          jtTabno_gg11;
} tgg11_JoinTabIdTrace;

struct tgg11_TemplateDummy {
    tgg11_BdTraceType    dummy_tgg11_BdTraceType;
    tgg11_JoinTraceType  dummy_tgg11_JoinTraceType;
};


#endif
