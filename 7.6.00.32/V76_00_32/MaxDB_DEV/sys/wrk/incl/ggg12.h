
/* some common prolog for every module
 */

#ifndef GGG12_H
#define GGG12_H


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


#if defined (__cplusplus)

     #if defined __cplusplus
     #include "ggg92.h"
     #endif
    /* */
typedef tgg92_KernelOid            tgg12_KernelOid;

#else

typedef tgg00_OidSpace             tgg12_KernelOid;

#endif

    /* */

typedef enum tgg12_OmsTraceType_Enum
{
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
    otTwoKeys_egg12
} tgg12_OmsTraceType_Enum;
typedef tgg12_OmsTraceType_Enum tgg12_OmsTraceType_Param;
typedef ENUM_TYPE_MSP00 (          tgg12_OmsTraceType, tgg12_OmsTraceType_Enum, otTwoKeys_egg12, unsigned char);
    /* */

typedef enum tgg12_OmsTraceType2_Enum
{
    ot2Nil_egg12,
    ot2Input_egg12,
    ot2InputVers_egg12,
    ot2InputSeq_egg12,
    ot2InputSeqVers_egg12,
    ot2Output_egg12,
    ot2OutputSeq_egg12
} tgg12_OmsTraceType2_Enum;
typedef tgg12_OmsTraceType2_Enum tgg12_OmsTraceType2_Param;
typedef ENUM_TYPE_MSP00 (          tgg12_OmsTraceType2, tgg12_OmsTraceType2_Enum, ot2OutputSeq_egg12, unsigned char);
    /* */

typedef enum tgg12_RelObjStatus_Enum
{
    rosPageNotFound_gg12,
    rosFileAlreadyDeleted_gg12,
    rosFileJustDeleted_gg12,
    rosWrongFileNo_gg12,
    rosWrongObjVersion_gg12,
    rosUnknownError_gg12,
    rosObjSucessfullyReleased_gg12,
    rosDelContObjFailed_gg12,
    rosDelKeyFailed_gg12,
    rosWrongKeyInfo_gg12,
    rosOIdReused_gg12
} tgg12_RelObjStatus_Enum;
typedef tgg12_RelObjStatus_Enum tgg12_RelObjStatus_Param;
typedef ENUM_TYPE_MSP00 (          tgg12_RelObjStatus, tgg12_RelObjStatus_Enum, rosOIdReused_gg12, unsigned char);

typedef struct                     tkb05_ObjFileno
{
    union
    {
    /* false */
        struct tkb05_ObjFileno_false
        {
            tgg00_ObjFileNo     ObjFileNo_I4_gg12_F;
        } C_false;
    /* true */
        struct tkb05_ObjFileno_true
        {
            tsp00_C8            ObjFileNo_C8_gg12_F;
        } C_true;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tgg00_ObjFileNo    &ObjFileNo_I4_gg12      ()  { return this->variant.C_false.ObjFileNo_I4_gg12_F; };
    tsp00_C8           &ObjFileNo_C8_gg12      ()  { return this->variant.C_true.ObjFileNo_C8_gg12_F; };
    /* const accessor functions */
    const tgg00_ObjFileNo &ObjFileNo_I4_gg12      () const  { return this->variant.C_false.ObjFileNo_I4_gg12_F; };
    const tsp00_C8     &ObjFileNo_C8_gg12      () const  { return this->variant.C_true.ObjFileNo_C8_gg12_F; };
#endif    /* defined (__cplusplus) */
} tkb05_ObjFileno;

typedef struct                     tgg12_OmsErrorTrace
{
    tgg00_MessType      oerrMessType_gg12;
    tgg00_MessType2     oerrMessType2_gg12;
    tgg12_OmsTraceType  oerrTrType_gg12;
    tgg12_OmsTraceType2 oerrTrType2_gg12;
    tsp00_PageNo        oerrRoot_gg12;
    tgg00_BasisError    oerrError_gg12;
    tkb05_ObjFileno     oerr;
} tgg12_OmsErrorTrace;

typedef struct                     tgg12_OmsFileInfoTrace
{
    tgg00_MessType      oinfMessType_gg12;
    tgg00_MessType2     oinfMessType2_gg12;
    tgg12_OmsTraceType  oinfTrType_gg12;
    tgg12_OmsTraceType2 oinfTrType2_gg12;
    tsp00_PageNo        oinfNumAllPages_gg12;
    tsp00_PageNo        oinfNumFreePages_gg12;
    tsp00_PageNo        oinfNumEmptyPages_gg12;
    tsp00_Int4          oinfNumEmptyPageLimit_gg12;
    tsp00_PageNo        oinfRoot_gg12;
} tgg12_OmsFileInfoTrace;

typedef struct                     tgg12_OmsFilenameTrace
{
    tgg00_MessType      ofnMessType_gg12;
    tgg00_MessType2     ofnMessType2_gg12;
    tgg12_OmsTraceType  ofnTrType_gg12;
    tgg12_OmsTraceType2 ofnTrType2_gg12;
    tgg00_Filename      ofnFileName_gg12;
} tgg12_OmsFilenameTrace;

typedef struct                     tgg12_OmsGarbCollTrace
{
    tgg00_MessType      ogrbMessType_gg12;
    tgg00_MessType2     ogrbMessType2_gg12;
    tgg12_OmsTraceType  ogrbTrType_gg12;
    tgg12_OmsTraceType2 ogrbTrType2_gg12;
    pasbool             ogrbIsActiv_gg12;
    pasbool             ogrbFiller1_gg12;
    tsp00_Int2          ogrbCollectorIndex_gg12;
} tgg12_OmsGarbCollTrace;

typedef struct                     tgg12_OmsPageContainerStatTrace
{
    tgg00_MessType      opcoMessType_gg12;
    tgg00_MessType2     opcoMessType2_gg12;
    tgg12_OmsTraceType  opcoTrType_gg12;
    tgg12_OmsTraceType2 opcoTrType2_gg12;
    tsp00_PageNo        opcoNumSubRoots_gg12;
    tsp00_Int4          opcoFreeObjCnt_gg12;
    tsp00_Int4          opcoFeotObjCnt_gg12;
    tsp00_Int4          opcoOccObjCnt_gg12;
    tsp00_Int4          opcoResObjCnt_gg12;
    tsp00_PageNo        opcoAllPageCnt_gg12;
    tsp00_PageNo        opcoEmptyPageCnt_gg12;
    tsp00_PageNo        opcoFreePageCnt_gg12;
    tsp00_PageNo        opcoFeotPageCnt_gg12;
    pasbool             opcoIsFileDeleted_gg12;
} tgg12_OmsPageContainerStatTrace;

typedef struct                     tgg12_OmsPageChainStatTrace
{
    tgg00_MessType      opctMessType_gg12;
    tgg00_MessType2     opctMessType2_gg12;
    tgg12_OmsTraceType  opctTrType_gg12;
    tgg12_OmsTraceType2 opctTrType2_gg12;
    tsp00_PageNo        opctSubRoot_gg12;
    tsp00_Int4          opctFreeObjCnt_gg12;
    tsp00_Int4          opctFeotObjCnt_gg12;
    tsp00_Int4          opctOccObjCnt_gg12;
    tsp00_Int4          opctResObjCnt_gg12;
    tsp00_PageNo        opctAllPageCnt_gg12;
    tsp00_PageNo        opctAllPageRootCnt_gg12;
    tsp00_PageNo        opctEmptyPageCnt_gg12;
    tsp00_PageNo        opctEmptyPageRootCnt_gg12;
    tsp00_PageNo        opctEmptyPageInFreeChainCnt_gg12;
    tsp00_PageNo        opctEmtyPageWithEmptyLabelCnt_gg12;
    tsp00_PageNo        opctFreePageCnt_gg12;
    tsp00_PageNo        opctFreePageRootCnt_gg12;
    tsp00_PageNo        opctFreePageInFreeChainCnt_gg12;
    tsp00_PageNo        opctFeotPageCnt_gg12;
    tsp00_PageNo        opctPageInFreeChainCnt_gg12;
    tsp00_PageNo        opctPageWithEmptyLabelCnt_gg12;
    tsp00_PageNo        opctPageWithFreeLabelCnt_gg12;
    tsp00_PageNo        opctPageWithFreeLabelInFreeChainCnt_gg12;
    tsp00_PageNo        opctFreePageWithFreeLabelInFreeChainCnt_gg12;
    pasbool             opctIsChainDeleted_gg12;
} tgg12_OmsPageChainStatTrace;

typedef struct                     tgg12_OmsTraceKeyHeader
{
    tgg00_MessType      okhMessType_gg12;
    tgg00_MessType2     okhMessType2_gg12;
    tgg12_OmsTraceType  okhTrType_gg12;
    tgg12_OmsTraceType2 okhTrType2_gg12;
    tsp00_Uint2         okhKeyLen_gg12;
    tkb05_ObjFileno     okh;
    tgg91_TransNo       okhConsView_gg12;
    tsp00_Int2          okhFiller2_gg12;
} tgg12_OmsTraceKeyHeader;

typedef struct                     tgg12_OmsKeyTrace
{
    tgg12_OmsTraceKeyHeader okyHead_gg12;
    tsp00_Key           okyKey_gg12;
} tgg12_OmsKeyTrace;

typedef struct                     tgg12_OmsOidTrace
{
    tgg00_MessType      ooidMessType_gg12;
    tgg00_MessType2     ooidMessType2_gg12;
    tgg12_OmsTraceType  ooidTrType_gg12;
    tgg12_OmsTraceType2 ooidTrType2_gg12;
    tgg91_TransNo       ooidConsView_gg12;
    tgg91_PageRef       ooidObjVers_gg12;
    tgg12_KernelOid     ooidObjId_gg12;
} tgg12_OmsOidTrace;

typedef struct                     tgg12_OmsRelOidTrace
{
    tgg00_MessType      roidMessType_gg12;
    tgg00_MessType2     roidMessType2_gg12;
    tgg12_OmsTraceType  roidTrType_gg12;
    tgg12_OmsTraceType2 roidTrType2_gg12;
    tgg91_TransNo       roidConsView_gg12;
    tgg91_PageRef       roidObjVers_gg12;
    tgg12_KernelOid     roidObjId_gg12;
    tgg12_RelObjStatus  roidRelObjStatus_gg12;
} tgg12_OmsRelOidTrace;

typedef struct                     tgg12_OmsPageStatTrace
{
    tgg00_MessType      opsMessType_gg12;
    tgg00_MessType2     opsMessType2_gg12;
    tgg12_OmsTraceType  opsTrType_gg12;
    tgg12_OmsTraceType2 opsTrType2_gg12;
    tsp00_PageNo        opsPno_gg12;
    tsp00_Int4          opsOccObjCnt_gg12;
    tsp00_Int4          opsFreeObjCnt_gg12;
    tsp00_Int4          opsFreeEotObjCnt_gg12;
    tsp00_Int4          opsResObjCnt_gg12;
} tgg12_OmsPageStatTrace;

typedef struct                     tgg12_OmsVersionErrorTrace
{
    tgg00_MessType      oveMessType_gg12;
    tgg00_MessType2     oveMessType2_gg12;
    tgg12_OmsTraceType  oveTrType_gg12;
    tgg12_OmsTraceType2 oveTrType2_gg12;
    tgg91_PageRef       oveObjVers_gg12;
    tgg91_PageRef       oveWantedVers_gg12;
    tgg12_KernelOid     oveObjId_gg12;
    tgg00_ObjFrameVers  oveFrameVers_gg12;
    tkb05_ObjFileno     ove;
} tgg12_OmsVersionErrorTrace;

typedef struct                     tgg12_OmsTwoKeyTrace
{
    tgg00_MessType      oskMessType_gg12;
    tgg00_MessType2     oskMessType2_gg12;
    tgg12_OmsTraceType  oskTrType_gg12;
    tgg12_OmsTraceType2 oskTrType2_gg12;
    tsp00_Int2          oskKeyLen_gg12;
    tgg00_VtraceKey     oskStartKey_gg12;
    tgg00_VtraceKey     oskStopKey_gg12;
} tgg12_OmsTwoKeyTrace;

struct tgg12_TemplateDummy {
    tgg12_RelObjStatus   dummy_tgg12_RelObjStatus;
    tgg12_OmsTraceType2  dummy_tgg12_OmsTraceType2;
    tgg12_OmsTraceType   dummy_tgg12_OmsTraceType;
};


#endif
