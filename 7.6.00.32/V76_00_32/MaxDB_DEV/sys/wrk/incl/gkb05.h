
/* some common prolog for every module
 */

#ifndef GKB05_H
#define GKB05_H


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

#ifndef GKB00_H
#include "gkb00.h"
#endif

/******************************************
 *           C O N S T A N T S            *
 ******************************************/

     #if defined __cplusplus
     #include "ggg92.h"
     #endif
#define ITEMS_PER_SUPPLY_SEGM_KB05            100    /* PTS 1105492 JA 2000-03-15 */
#define MAX_ROW_HASH_KB05                 2500000
#define MAX_SUPPLY_BLOCK_ITEMS_KB05        100000    /* PTS 1105492 JA 2000-03-15 */
#define MAX_TAB_HASH_KB05                   50000
#define MAX_TRANS_HASH_KB05                  1024
#define MAX_SORTED_ROW_LOCKS_KB05            1000    /* PTS 1107056 JA 2001-03-01 */
#define NIL_REGION_KB05                         0
#define REGIONS_KB05                           64
#define ROWID_MXKB05                           64    /* PTS 1121179 UH 2003-04-07 */
#define ROWID_FIRSTPART_MXKB05         (ROWID_MXKB05 - ( INT4_MXSP00 * 2 ))    /* PTS 1121179 UH 2003-04-07 */
#define SPACE_BETWEEN_REGIONS_KB05             16    /* PTS 1105492 JA 2000-03-15 */
#define TIMEOUT_EOT_EXCL_KB05                 300    /*  5 min */
#define TRANS_PER_OPEN_ENTRY_KB05               3    /* PTS 1105492 JA 2000-03-15 */
    /* */
#define MAX_SUPPLY_SEGMENTS_KB05       (MAX_INT4_SP00 / ITEMS_PER_SUPPLY_SEGM_KB05)    /*JA 2000-03-15 */
#define MAX_REGIONS_KB05               (3 * REGIONS_KB05)    /* 3 = trans + tab + row */
#define MAX_TRANS_REGIONS_KB05         REGIONS_KB05

/******************************************
 *               T Y P E S                *
 ******************************************/


#if defined (__cplusplus)

typedef tgg92_KernelOid            tkb05_KernelOid;

#else

typedef tgg00_OidSpace             tkb05_KernelOid;

#endif

    /* */
typedef tsp00_Int2                 tkb05_RegionNo;
    /* */
typedef OLDPASCALSTRING_TYPE_MSP00 (  tkb05_RowId, ROWID_MXKB05);
typedef CSTRING_TYPE_MSP00      (  tkb05_RowIdc, ROWID_MXKB05);
    /* PTS 1121179 UH 2003-04-07 */
typedef OLDPASCALSTRING_TYPE_MSP00 (  tkb05_RowIdFirstPart, ROWID_FIRSTPART_MXKB05);
typedef CSTRING_TYPE_MSP00      (  tkb05_RowIdFirstPartc, ROWID_FIRSTPART_MXKB05);
    /* */
typedef tsp00_C10                  tkb05_ShortKey;
    /* */

typedef enum tkb05_Handling__Enum
{
    hdCheckCollision_ekb05,
    hdInsLock_ekb05,
    hdInsReq_ekb05,
    hdCheckOwnReq_ekb05,
    hdFiller4_ekb05,
    hdFiller5_ekb05,
    hdFiller6_ekb05,
    hdFiller7_ekb05
} tkb05_Handling__Enum;
typedef tkb05_Handling__Enum tkb05_Handling__Param;
typedef SMALLSET_TYPE_MSP00 (      tkb05_Handling, tkb05_Handling__Enum, hdFiller7_ekb05);

typedef struct                     tkb05_WantedLock
{
    tgg00_Surrogate     wlTabId_kb05;
    tgg00_LockReqMode   wlMode_kb05;
    tgg00_LockReqState  wlState_kb05;
    tsp00_Int2          wlKeyLen_kb05;
    pasbool             wlRowWanted_kb05;
    tkb05_Handling      wlHandling_kb05;
    tgg00_LockReqMode   wlGrantedMode_kb05;
    pasbool             wlFiller1_kb05;
    union
    {
    /* 1 */
        struct tkb05_WantedLock_1
        {
            tkb05_RowId         wlRowId_kb05_F;
        } C_1;
    /* 2 */
        struct tkb05_WantedLock_2
        {
            tkb05_ShortKey      wlShortKey_kb05_F;
        } C_2;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tkb05_RowId        &wlRowId_kb05           ()  { return this->variant.C_1.wlRowId_kb05_F; };
    tkb05_ShortKey     &wlShortKey_kb05        ()  { return this->variant.C_2.wlShortKey_kb05_F; };
    /* const accessor functions */
    const tkb05_RowId  &wlRowId_kb05           () const  { return this->variant.C_1.wlRowId_kb05_F; };
    const tkb05_ShortKey &wlShortKey_kb05        () const  { return this->variant.C_2.wlShortKey_kb05_F; };
#endif    /* defined (__cplusplus) */
} tkb05_WantedLock;

typedef enum tkb05_ItemKind_Enum
{
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
    ikHashOpenTrans_ekb05
} tkb05_ItemKind_Enum;
typedef tkb05_ItemKind_Enum tkb05_ItemKind_Param;
typedef ENUM_TYPE_MSP00 (          tkb05_ItemKind, tkb05_ItemKind_Enum, ikHashOpenTrans_ekb05, unsigned char);
    /**/

typedef enum tkb05_LocklistState_Enum
{
    lsOpen_ekb05,
    lsClosed_ekb05,
    lsClosedWaitForCheckpt_ekb05
} tkb05_LocklistState_Enum;
typedef tkb05_LocklistState_Enum tkb05_LocklistState_Param;
typedef ENUM_TYPE_MSP00 (          tkb05_LocklistState, tkb05_LocklistState_Enum, lsClosedWaitForCheckpt_ekb05, unsigned char);
    /**/

typedef enum tkb05_TransInfo__Enum
{
    tiAuxPermExists_ekb05,
    tiAuxTempExists_ekb05,
    tiCollision_ekb05,
    tiExclusiveUser_ekb05,
    tiUserTask_ekb05,
    tiPriority_ekb05,
    tiCancelled_ekb05
} tkb05_TransInfo__Enum;
typedef tkb05_TransInfo__Enum tkb05_TransInfo__Param;
typedef SMALLSET_TYPE_MSP00 (      tkb05_TransInfo, tkb05_TransInfo__Enum, tiCancelled_ekb05);
    /**/

typedef enum tkb05_TransLockMode_Enum
{
    tlmFree_ekb05,
    tlmShare_ekb05,
    tlmExcl_ekb05,
    tlmEotExcl_ekb05
} tkb05_TransLockMode_Enum;
typedef tkb05_TransLockMode_Enum tkb05_TransLockMode_Param;
typedef ENUM_TYPE_MSP00 (          tkb05_TransLockMode, tkb05_TransLockMode_Enum, tlmEotExcl_ekb05, unsigned char);
    /**/
typedef struct tkb05_Req tkb05_Req;
typedef tkb05_Req                 *tkb05_ReqPtr;

 struct                     tkb05_Req
{
    tkb05_ItemKind      rqItemKind_kb05;
    pasbool             rqExtentItem_kb05;
    tsp00_Uint1         rqChkOwnRegion_kb05;
    tsp00_Uint1         rqChkTransIndex_kb05;
    tsp00_TaskId        rqChkTaskId_kb05;
    tkb05_ReqPtr        rqNextReq_kb05;
    tkb05_ReqPtr        rqPrevReq_kb05;
    tkb05_ReqPtr        rqLastReq_kb05;

#if defined (BIT64)


#else

    tsp00_Addr          rqFillerAddr_kb05;

#endif

    tgg00_TransIndex    rqTransIndex_kb05;
    pasbool             rqEotWanted_kb05;
    tgg00_LockReqMode   rqReqMode_kb05;
    tgg00_LockReqMode   rqLockMode_kb05;
    tkb05_TransLockMode rqTransLockMode_kb05;
    tsp00_Int2          rqFiller1_kb05;
    tsp00_Int4          rqRowShareLocks_kb05;
    tsp00_Int4          rqRowExclLocks_kb05;
} ;

typedef struct                     tkb05_RowIdItem
{
    tkb05_ItemKind      riItemKind_kb05;
    pasbool             riExtentItem_kb05;
    tsp00_Uint1         riChkOwnRegion_kb05;
    tsp00_Uint1         riChkTransIndex_kb05;
    tsp00_TaskId        riChkTaskId_kb05;
    tkb05_RowId         riRowId_kb05;
} tkb05_RowIdItem;
typedef tkb05_RowIdItem           *tkb05_RowIdItemPtr;
    /**/
typedef struct tkb05_RowLock tkb05_RowLock;
typedef tkb05_RowLock             *tkb05_RowLockPtr;

 struct                     tkb05_RowLock
{
    tkb05_ItemKind      rlItemKind_kb05;
    pasbool             rlExtentItem_kb05;
    tsp00_Uint1         rlChkOwnRegion_kb05;
    tsp00_Uint1         rlChkTransIndex_kb05;
    tsp00_TaskId        rlChkTaskId_kb05;
    tgg00_Surrogate     rlTabId_kb05;
    tkb05_RowLockPtr    rlNextHashRow_kb05;
    tkb05_RowIdItemPtr  rlRowIdItem_kb05;
    tkb05_ReqPtr        rlFirstReqRow_kb05;

#if defined (BIT64)


#else

    tsp00_Addr          rlFillerAddr_kb05;

#endif

    tkb05_ShortKey      rlShortKey_kb05;
    tsp00_Int2          rlKeyLen_kb05;
    tgg00_TransIndex    rlExclTransIndex_kb05;
    tgg00_LockReqState  rlLockState_kb05;
    pasbool             rlFiller_kb05;
    tsp00_Int4          rlRowShareLocks_kb05;
    tsp00_Int4          rlRowOptimCnt_kb05;
    tsp00_Int4          rlRowOptimVers_kb05;
    tgg00_TransIndex    rlShare1TransIndex_kb05;
    tgg00_TransIndex    rlShare2TransIndex_kb05;
} ;
typedef ARRAY_TYPE_MSP00 (         tkb05_RowHash, tkb05_RowLockPtr, MAX_ROW_HASH_KB05);
    /**/
typedef tkb05_RowHash             *tkb05_RowHashPtr;
    /**/
typedef struct tkb05_TabLock tkb05_TabLock;
typedef tkb05_TabLock             *tkb05_TabLockPtr;

 struct                     tkb05_TabLock
{
    tkb05_ItemKind      tlItemKind_kb05;
    pasbool             tlExtentItem_kb05;
    tsp00_Uint1         tlChkOwnRegion_kb05;
    tsp00_Uint1         tlChkTransIndex_kb05;
    tsp00_TaskId        tlChkTaskId_kb05;
    tgg00_Surrogate     tlTabId_kb05;
    tkb05_TabLockPtr    tlNextHashTab_kb05;
    tkb05_ReqPtr        tlFirstReqTab_kb05;
    tkb05_ReqPtr        tlFirstReqRow_kb05;

#if defined (BIT64)


#else

    tsp00_Addr          tlFillerAddr_kb05;

#endif

    pasbool             tlIsSysExcl_kb05;
    pasbool             tlFiller1_kb05;
    tgg00_TransIndex    tlExclTransIndex_kb05;
    tgg00_TransIndex    tlShare1TransIndex_kb05;
    tgg00_TransIndex    tlShare2TransIndex_kb05;
    tsp00_Int4          tlSysShareLocks_kb05;
    tsp00_Int4          tlTabShareLocks_kb05;
    tsp00_Int4          tlTabOptimVers_kb05;
    tsp00_Int4          tlRowShareLocks_kb05;
    tsp00_Int4          tlRowOptimCnt_kb05;
    tsp00_Int4          tlRowExclLocks_kb05;
} ;
typedef ARRAY_TYPE_MSP00 (         tkb05_TabHash, tkb05_TabLockPtr, MAX_TAB_HASH_KB05);
    /**/
typedef tkb05_TabHash             *tkb05_TabHashPtr;
    /**/
typedef struct tkb05_RowTrans tkb05_RowTrans;
typedef tkb05_RowTrans            *tkb05_RowTransPtr;

 struct                     tkb05_RowTrans
{
    tkb05_ItemKind      rtItemKind_kb05;
    pasbool             rtExtentItem_kb05;
    tsp00_Uint1         rtChkOwnRegion_kb05;
    tsp00_Uint1         rtChkTransIndex_kb05;
    tsp00_TaskId        rtChkTaskId_kb05;
    tkb05_RowLockPtr    rtRow_kb05;
    tkb05_RowTransPtr   rtNextRow_kb05;
    tkb05_RowTransPtr   rtPrevRow_kb05;

#if defined (BIT64)


#else

    tsp00_Addr          rtFillerAddr_kb05;

#endif

    tkb05_ShortKey      rtShortKey_kb05;
    tsp00_Int2          rtKeyLen_kb05;
    tgg00_LockReqMode   rtLockMode_kb05;
    tgg00_LockReqState  rtLockState_kb05;
    tgg00_LockReqMode   rtReqMode_kb05;
    tgg00_LockReqState  rtReqState_kb05;
    tkb05_RegionNo      rtRowRegion_kb05;
    pasbool             rtSharedLocked_kb05;
    pasbool             rtOptimSet_kb05;
    tsp00_Int4          rtRowHash_kb05;
    tsp00_Int4          rtRowOptimVers_kb05;
    tsp00_Int4          rtTabOptimVers_kb05;
} ;
typedef struct tkb05_TabTrans tkb05_TabTrans;
typedef tkb05_TabTrans            *tkb05_TabTransPtr;

 struct                     tkb05_TabTrans
{
    tkb05_ItemKind      ttItemKind_kb05;
    pasbool             ttExtentItem_kb05;
    tsp00_Uint1         ttChkOwnRegion_kb05;
    tsp00_Uint1         ttChkTransIndex_kb05;
    tsp00_TaskId        ttChkTaskId_kb05;
    tgg00_Surrogate     ttTabId_kb05;
    tkb05_TabLockPtr    ttTab_kb05;
    tkb05_RowTransPtr   ttFirstRow_kb05;
    tkb05_RowTransPtr   ttConsistentRow_kb05;
    tkb05_TabTransPtr   ttNextTab_kb05;
    tgg00_LockReqMode   ttLockMode_kb05;
    tgg00_LockReqState  ttLockState_kb05;
    tgg00_LockReqMode   ttReqMode_kb05;
    tgg00_LockReqState  ttReqState_kb05;
    pasbool             ttKeySort_kb05;
    pasbool             ttFiller1_kb05;
    tkb05_RegionNo      ttTabRegion_kb05;
    tsp00_Int4          ttTabHash_kb05;
    tsp00_Int4          ttRowShareLocks_kb05;
    tsp00_Int4          ttRowOptimCnt_kb05;
    tsp00_Int4          ttRowExclLocks_kb05;
} ;

typedef struct                     tkb05_SurrogateTransId
{
    union
    {
    /* 1 */
        struct tkb05_SurrogateTransId_1
        {
            tgg00_Surrogate     surrId_kb05_F;
        } C_1;
    /* 2 */
        struct tkb05_SurrogateTransId_2
        {
            tsp00_Int2          surrSpace_kb05_F;
            tgg91_TransNo       surrTransId_kb05_F;
        } C_2;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tgg00_Surrogate    &surrId_kb05            ()  { return this->variant.C_1.surrId_kb05_F; };
    tsp00_Int2         &surrSpace_kb05         ()  { return this->variant.C_2.surrSpace_kb05_F; };
    tgg91_TransNo      &surrTransId_kb05       ()  { return this->variant.C_2.surrTransId_kb05_F; };
    /* const accessor functions */
    const tgg00_Surrogate &surrId_kb05            () const  { return this->variant.C_1.surrId_kb05_F; };
    const tsp00_Int2   &surrSpace_kb05         () const  { return this->variant.C_2.surrSpace_kb05_F; };
    const tgg91_TransNo &surrTransId_kb05       () const  { return this->variant.C_2.surrTransId_kb05_F; };
#endif    /* defined (__cplusplus) */
} tkb05_SurrogateTransId;
typedef struct tkb05_HashOpenTrans tkb05_HashOpenTrans;
typedef tkb05_HashOpenTrans       *tkb05_HashOpenTransPtr;

 struct                     tkb05_HashOpenTrans
{
    tkb05_ItemKind      hshItemKind_kb05;
    pasbool             hshExtentItem_kb05;
    tsp00_Uint1         hshChkOwnRegion_kb05;
    tsp00_Uint1         hshChkTransIndex_kb05;
    tsp00_TaskId        hshChkTaskId_kb05;
    tkb05_SurrogateTransId hshTransId_kb05;
    tkb05_TabLockPtr    hshNextHashTab_kb05;

#if defined (BIT64)


#else

    tsp00_Addr          hshFillerAddr_kb05;

#endif

    tsp00_Int4          hshUsedCnt_kb05;
    tsp00_Int4          hshOwnHashValue_kb05;
    tsp00_Int4          hshNonVersUsedCnt_kb05;
} ;
typedef struct tkb05_OpenTrans tkb05_OpenTrans;
typedef tkb05_OpenTrans           *tkb05_OpenTransPtr;

 struct                     tkb05_OpenTrans
{
    tkb05_ItemKind      openItemKind_kb05;
    pasbool             openExtentItem_kb05;
    tsp00_Uint1         openChkOwnRegion_kb05;
    tsp00_Uint1         openChkTransIndex_kb05;
    tsp00_TaskId        openChkTaskId_kb05;
    tkb05_OpenTransPtr  openNext_kb05;
    ARRAY_VAR_MSP00 (         openOwnHashTrans_kb05, tkb05_HashOpenTransPtr, TRANS_PER_OPEN_ENTRY_KB05);
    ARRAY_VAR_MSP00 (         openTransId_kb05, tgg91_TransNo, TRANS_PER_OPEN_ENTRY_KB05);
    ARRAY_VAR_MSP00 (         openHashRegion_kb05, tkb05_RegionNo, TRANS_PER_OPEN_ENTRY_KB05);
    tsp00_Int4          openCount_kb05;
    tsp00_Int4          openFiller1_kb05;
} ;
typedef struct tkb05_ConsistView tkb05_ConsistView;
typedef tkb05_ConsistView         *tkb05_ConsistViewPtr;

 struct                     tkb05_ConsistView
{
    tkb05_ItemKind      cstItemKind_kb05;
    pasbool             cstExtentItem_kb05;
    tsp00_Uint1         cstChkOwnRegion_kb05;
    tsp00_Uint1         cstChkTransIndex_kb05;
    tsp00_TaskId        cstChkTaskId_kb05;
    tkb05_SurrogateTransId cstConsistViewNo_kb05;
    tkb05_TabLockPtr    cstNextHashTab_kb05;
    tkb05_ConsistViewPtr cstNextConsistView_kb05;
    tkb05_ConsistViewPtr cstPrevConsistView_kb05;
    tkb05_OpenTransPtr  cstFirstOpenTrans_kb05;
    tsp00_Int4          cstStartDate_kb05;
    tsp00_Int4          cstStartTime_kb05;
    tsp00_Int4          cstTimeLastUsed_kb05;
    pasbool             cstCancelled_kb05;
    pasbool             cstInWork_kb05;
    tgg91_TransNo       cstCreatorTransId_kb05;
    tgg91_TransNo       cstMinOpenTransId_kb05;
    tgg00_TransIndex    cstCurrTransIndex_kb05;
    tsp00_Int4          cstStartClockstamp_kb05;
    pasbool             cstOmsVersion_kb05;
    pasbool             cstFiller1_kb05;
    tsp00_Uint2         cstFiller2_kb05;
} ;
typedef struct tkb05_SupplyItem tkb05_SupplyItem;
typedef tkb05_SupplyItem          *tkb05_SupplyItemPtr;

 struct                     tkb05_SupplyItem
{
    union
    {
    /* 1 */
        struct tkb05_SupplyItem_1
        {
            tkb05_ItemKind      siItemKind_kb05_F;
            pasbool             siExtentItem_kb05_F;
            tsp00_Uint1         siChkOwnRegion_kb05_F;
            tsp00_Uint1         siChkTransIndex_kb05_F;
            tsp00_TaskId        siChkTaskId_kb05_F;
            tkb05_SupplyItemPtr siNext_kb05_F;
        } C_1;
    /* 2 */
        struct tkb05_SupplyItem_2
        {
            tkb05_ConsistView   siConsistView_kb05_F;
        } C_2;
    /* 3 */
        struct tkb05_SupplyItem_3
        {
            tkb05_HashOpenTrans siHashOpenTrans_kb05_F;
        } C_3;
    /* 4 */
        struct tkb05_SupplyItem_4
        {
            tkb05_OpenTrans     siOpenTrans_kb05_F;
        } C_4;
    /* 5 */
        struct tkb05_SupplyItem_5
        {
            tkb05_Req           siReq_kb05_F;
        } C_5;
    /* 6 */
        struct tkb05_SupplyItem_6
        {
            tkb05_RowIdItem     siRowIdItem_kb05_F;
        } C_6;
    /* 7 */
        struct tkb05_SupplyItem_7
        {
            tkb05_RowLock       siRowLock_kb05_F;
        } C_7;
    /* 8 */
        struct tkb05_SupplyItem_8
        {
            tkb05_RowTrans      siRowTrans_kb05_F;
        } C_8;
    /* 9 */
        struct tkb05_SupplyItem_9
        {
            tkb05_TabLock       siTabLock_kb05_F;
        } C_9;
    /* 10 */
        struct tkb05_SupplyItem_10
        {
            tkb05_TabTrans      siTabTrans_kb05_F;
        } C_10;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tkb05_ItemKind     &siItemKind_kb05        ()  { return this->variant.C_1.siItemKind_kb05_F; };
    pasbool            &siExtentItem_kb05      ()  { return this->variant.C_1.siExtentItem_kb05_F; };
    tsp00_Uint1        &siChkOwnRegion_kb05    ()  { return this->variant.C_1.siChkOwnRegion_kb05_F; };
    tsp00_Uint1        &siChkTransIndex_kb05   ()  { return this->variant.C_1.siChkTransIndex_kb05_F; };
    tsp00_TaskId       &siChkTaskId_kb05       ()  { return this->variant.C_1.siChkTaskId_kb05_F; };
    tkb05_SupplyItemPtr &siNext_kb05            ()  { return this->variant.C_1.siNext_kb05_F; };
    tkb05_ConsistView  &siConsistView_kb05     ()  { return this->variant.C_2.siConsistView_kb05_F; };
    tkb05_HashOpenTrans &siHashOpenTrans_kb05   ()  { return this->variant.C_3.siHashOpenTrans_kb05_F; };
    tkb05_OpenTrans    &siOpenTrans_kb05       ()  { return this->variant.C_4.siOpenTrans_kb05_F; };
    tkb05_Req          &siReq_kb05             ()  { return this->variant.C_5.siReq_kb05_F; };
    tkb05_RowIdItem    &siRowIdItem_kb05       ()  { return this->variant.C_6.siRowIdItem_kb05_F; };
    tkb05_RowLock      &siRowLock_kb05         ()  { return this->variant.C_7.siRowLock_kb05_F; };
    tkb05_RowTrans     &siRowTrans_kb05        ()  { return this->variant.C_8.siRowTrans_kb05_F; };
    tkb05_TabLock      &siTabLock_kb05         ()  { return this->variant.C_9.siTabLock_kb05_F; };
    tkb05_TabTrans     &siTabTrans_kb05        ()  { return this->variant.C_10.siTabTrans_kb05_F; };
    /* const accessor functions */
    const tkb05_ItemKind &siItemKind_kb05        () const  { return this->variant.C_1.siItemKind_kb05_F; };
    const pasbool      &siExtentItem_kb05      () const  { return this->variant.C_1.siExtentItem_kb05_F; };
    const tsp00_Uint1  &siChkOwnRegion_kb05    () const  { return this->variant.C_1.siChkOwnRegion_kb05_F; };
    const tsp00_Uint1  &siChkTransIndex_kb05   () const  { return this->variant.C_1.siChkTransIndex_kb05_F; };
    const tsp00_TaskId &siChkTaskId_kb05       () const  { return this->variant.C_1.siChkTaskId_kb05_F; };
    const tkb05_SupplyItemPtr &siNext_kb05            () const  { return this->variant.C_1.siNext_kb05_F; };
    const tkb05_ConsistView &siConsistView_kb05     () const  { return this->variant.C_2.siConsistView_kb05_F; };
    const tkb05_HashOpenTrans &siHashOpenTrans_kb05   () const  { return this->variant.C_3.siHashOpenTrans_kb05_F; };
    const tkb05_OpenTrans &siOpenTrans_kb05       () const  { return this->variant.C_4.siOpenTrans_kb05_F; };
    const tkb05_Req    &siReq_kb05             () const  { return this->variant.C_5.siReq_kb05_F; };
    const tkb05_RowIdItem &siRowIdItem_kb05       () const  { return this->variant.C_6.siRowIdItem_kb05_F; };
    const tkb05_RowLock &siRowLock_kb05         () const  { return this->variant.C_7.siRowLock_kb05_F; };
    const tkb05_RowTrans &siRowTrans_kb05        () const  { return this->variant.C_8.siRowTrans_kb05_F; };
    const tkb05_TabLock &siTabLock_kb05         () const  { return this->variant.C_9.siTabLock_kb05_F; };
    const tkb05_TabTrans &siTabTrans_kb05        () const  { return this->variant.C_10.siTabTrans_kb05_F; };
#endif    /* defined (__cplusplus) */
} ;
typedef ARRAY_TYPE_MSP00 (         tkb05_SupplyBlock, tkb05_SupplyItem, MAX_SUPPLY_BLOCK_ITEMS_KB05);
    /**/
typedef tkb05_SupplyBlock         *tkb05_SupplyBlockPtr;
    /**/
typedef struct tkb05_SupplySegment tkb05_SupplySegment;
typedef tkb05_SupplySegment       *tkb05_SupplySegmPtr;

 struct                     tkb05_SupplySegment
{
    tkb05_SupplySegmPtr sgmNext_kb05;

#if defined (BIT64)


#else

    tsp00_Addr          sgmFillerAddr_kb05;

#endif

    ARRAY_VAR_MSP00 (         sgmItemList_kb05, tkb05_SupplyItem, ITEMS_PER_SUPPLY_SEGM_KB05);
} ;
typedef ARRAY_TYPE_MSP00 (         tkb05_SupplySegmPtrList, tkb05_SupplySegmPtr, MAX_SUPPLY_SEGMENTS_KB05);
    /**/
typedef tkb05_SupplySegmPtrList   *tkb05_SupplySegmPtrListPtr;

typedef struct                     tkb05_SupplyPoolGlob
{
    tkb05_SupplySegmPtrListPtr poolSupplySegmList_kb05;
    tkb05_SupplySegmPtr poolFirstFreeSegm_kb05;
    tsp00_Int4          poolFreeSegments_kb05;
    tsp00_Int4          poolMinFreeSegms_kb05;
    tsp00_Int4          poolFiller1_kb05;
    tgg91_TransNo       poolMinOpenTransId_kb05;
    tgg91_TransNo       poolMinConsistViewNo_kb05;
    tsp00_8ByteCounter  poolSegmsUsedAmount_kb05;
    tsp00_8ByteCounter  poolItemsUsedAmount_kb05;
    tsp00_8ByteCounter  poolAmountPoints_kb05;
    pasbool             poolEmptyWarning_kb05;
    pasbool             poolFiller2_kb05;
    tsp00_Int2          poolMinOpenIdx_kb05;
    tsp00_RegionId      poolOwnRegionId_kb05;
    tgg91_TransNo       poolMinNonVersOpTrId_kb05;
    tgg91_TransNo       poolMinNonVersCSVNo_kb05;
    tsp00_Int4          poolFiller4_kb05;
} tkb05_SupplyPoolGlob;
typedef tkb05_SupplyPoolGlob      *tkb05_SupplyPoolGlobPtr;

typedef struct                     tkb05_UnivItemPtr
{
    union
    {
    /* 1 */
        struct tkb05_UnivItemPtr_1
        {
            tkb05_ConsistViewPtr uiConsistView_kb05_F;
        } C_1;
    /* 2 */
        struct tkb05_UnivItemPtr_2
        {
            tkb05_HashOpenTransPtr uiHashOpenTrans_kb05_F;
        } C_2;
    /* 3 */
        struct tkb05_UnivItemPtr_3
        {
            tkb05_OpenTransPtr  uiOpenTrans_kb05_F;
        } C_3;
    /* 4 */
        struct tkb05_UnivItemPtr_4
        {
            tkb05_ReqPtr        uiReq_kb05_F;
        } C_4;
    /* 5 */
        struct tkb05_UnivItemPtr_5
        {
            tkb05_RowIdItemPtr  uiRowIdItem_kb05_F;
        } C_5;
    /* 6 */
        struct tkb05_UnivItemPtr_6
        {
            tkb05_RowLockPtr    uiRowLock_kb05_F;
        } C_6;
    /* 7 */
        struct tkb05_UnivItemPtr_7
        {
            tkb05_RowTransPtr   uiRowTrans_kb05_F;
        } C_7;
    /* 8 */
        struct tkb05_UnivItemPtr_8
        {
            tkb05_SupplyItemPtr uiSupplyItem_kb05_F;
        } C_8;
    /* 9 */
        struct tkb05_UnivItemPtr_9
        {
            tkb05_TabLockPtr    uiTabLock_kb05_F;
        } C_9;
    /* 10 */
        struct tkb05_UnivItemPtr_10
        {
            tkb05_TabTransPtr   uiTabTrans_kb05_F;
        } C_10;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tkb05_ConsistViewPtr &uiConsistView_kb05     ()  { return this->variant.C_1.uiConsistView_kb05_F; };
    tkb05_HashOpenTransPtr &uiHashOpenTrans_kb05   ()  { return this->variant.C_2.uiHashOpenTrans_kb05_F; };
    tkb05_OpenTransPtr &uiOpenTrans_kb05       ()  { return this->variant.C_3.uiOpenTrans_kb05_F; };
    tkb05_ReqPtr       &uiReq_kb05             ()  { return this->variant.C_4.uiReq_kb05_F; };
    tkb05_RowIdItemPtr &uiRowIdItem_kb05       ()  { return this->variant.C_5.uiRowIdItem_kb05_F; };
    tkb05_RowLockPtr   &uiRowLock_kb05         ()  { return this->variant.C_6.uiRowLock_kb05_F; };
    tkb05_RowTransPtr  &uiRowTrans_kb05        ()  { return this->variant.C_7.uiRowTrans_kb05_F; };
    tkb05_SupplyItemPtr &uiSupplyItem_kb05      ()  { return this->variant.C_8.uiSupplyItem_kb05_F; };
    tkb05_TabLockPtr   &uiTabLock_kb05         ()  { return this->variant.C_9.uiTabLock_kb05_F; };
    tkb05_TabTransPtr  &uiTabTrans_kb05        ()  { return this->variant.C_10.uiTabTrans_kb05_F; };
    /* const accessor functions */
    const tkb05_ConsistViewPtr &uiConsistView_kb05     () const  { return this->variant.C_1.uiConsistView_kb05_F; };
    const tkb05_HashOpenTransPtr &uiHashOpenTrans_kb05   () const  { return this->variant.C_2.uiHashOpenTrans_kb05_F; };
    const tkb05_OpenTransPtr &uiOpenTrans_kb05       () const  { return this->variant.C_3.uiOpenTrans_kb05_F; };
    const tkb05_ReqPtr &uiReq_kb05             () const  { return this->variant.C_4.uiReq_kb05_F; };
    const tkb05_RowIdItemPtr &uiRowIdItem_kb05       () const  { return this->variant.C_5.uiRowIdItem_kb05_F; };
    const tkb05_RowLockPtr &uiRowLock_kb05         () const  { return this->variant.C_6.uiRowLock_kb05_F; };
    const tkb05_RowTransPtr &uiRowTrans_kb05        () const  { return this->variant.C_7.uiRowTrans_kb05_F; };
    const tkb05_SupplyItemPtr &uiSupplyItem_kb05      () const  { return this->variant.C_8.uiSupplyItem_kb05_F; };
    const tkb05_TabLockPtr &uiTabLock_kb05         () const  { return this->variant.C_9.uiTabLock_kb05_F; };
    const tkb05_TabTransPtr &uiTabTrans_kb05        () const  { return this->variant.C_10.uiTabTrans_kb05_F; };
#endif    /* defined (__cplusplus) */
} tkb05_UnivItemPtr;

typedef struct                     tkb05_TransLogging
{
    tsp00_Int4          tlogAuxFnNo_kb05;
    tsp00_Int4          tlogTempFunctRollb_kb05;
    tsp00_Int4          tlogTempEntryCnt_kb05;
    tsp00_PageNo        tlogTempFileRoot_kb05;
    tsp00_Addr          tlogTransPtr_kb05;
    tsp00_Addr          tFillerAddr_kb05;
} tkb05_TransLogging;
typedef struct tkb05_TransEntry tkb05_TransEntry;
typedef tkb05_TransEntry          *tkb05_TransEntryPtr;

 struct                     tkb05_TransEntry
{
    tkb05_TransEntryPtr teOwnAddr_kb05;
    tkb05_TransEntryPtr teNextHash_kb05;
    tkb05_TransEntryPtr tePrevHash_kb05;
    tkb05_TransEntryPtr teNextTrans_kb05;
    tkb05_TransEntryPtr tePrevTrans_kb05;
    tkb05_TabTransPtr   teFirstTab_kb05;
    tkb05_TabTransPtr   teTabRecentlyUsed_kb05;
    tkb05_RowTransPtr   teRowRecentlyUsed_kb05;
    tkb05_TabTransPtr   teReqTab_kb05;
    tkb05_RowTransPtr   teReqRow_kb05;
    tkb05_ConsistViewPtr teConsistView_kb05;
    tsp00_RteCommAddr   teCancelAddr_kb05;
    tgg00_TransIndex    teOwnIndex_kb05;
    tkb05_RegionNo      teOwnRegion_kb05;
    tgg91_SessionNo     teSessionId_kb05;
    tgg91_TransNo       teSelectTransId_kb05;
    tgg91_TransNo       teWriteTransId_kb05;
    tgg00_SubtransNo    teSubtransId_kb05;
    tgg00_UserId        teUserId_kb05;
    tsp00_TermId        teTermId_kb05;
    pasbool             teTimeoutEnabled_kb05;
    pasbool             teFiller2_kb05;
    tsp00_TaskId        teTaskId_kb05;
    tkb05_TransInfo     teInfo_kb05;
    tkb05_TransLockMode teLockMode_kb05;
    tkb05_TransLockMode teReqMode_kb05;
    pasbool             teFiller8_kb05;
    tsp00_Int4          teReqTimeout_kb05;
    tsp00_Int4          teTimeLastWrite_kb05;
    tgg00_MessType      teState_kb05;
    tkb05_TransLockMode teObjLockMode_kb05;
    tsp00_Int2          teFiller3_kb05;
    tsp00_Int4          teFiller4_kb05;
    tsp00_Int4          teStartDate_kb05;
    tsp00_Int4          teStartTime_kb05;
    tgg00_TransIndex    teFirstCollLock_kb05;
    tgg00_TransIndex    teObjReq_kb05;
    tsp00_Int4          teObjReqTimeout_kb05;
    pasbool             teWaiting_kb05;
    tkb05_TransLockMode teOmsLockMode_kb05;
    tsp00_Int2          teFiller6_kb05;
    tsp00_Int4          teFiller7_kb05;
    tkb05_TransLogging  teLog_kb05;
    tsp00_Longreal      teTransStartTime_kb05;
    tsp00_Addr          teLocalFileDirPtr_kb05;
    tsp00_Addr          teShrLockListPtr_kb05;
    tsp00_Addr          teExtTransMonPtr_kb05;
    tsp00_Addr          teFiller9_kb05;
} ;
typedef ARRAY_TYPE_MSP00 (         tkb05_TransList, tkb05_TransEntry, MAX_TRANS_GG04);
    /**/
typedef ARRAY_TYPE_MSP00 (         tkb05_TransPtrList, tkb05_TransEntryPtr, MAX_TRANS_GG04);
    /**/
typedef tkb05_TransList           *tkb05_TransListPtr;
    /**/
typedef tkb05_TransPtrList        *tkb05_TransPtrListPtr;

typedef struct                     tkb05_TransGlob
{
    tkb05_TransEntryPtr trFirst_kb05;
    tkb05_TransEntryPtr trFirstFree_kb05;
    tkb05_TransPtrListPtr trTransHash_kb05;

#if defined (BIT64)


#else

    tsp00_Addr          trFillerAddr_kb05;

#endif

    tsp00_Int2          trFreeEntries_kb05;
    tsp00_Int2          trConnectOnlyCnt_kb05;
    tsp00_Int2          trLockTransCnt_kb05;
    tsp00_Int2          trReqTransCnt_kb05;
    tsp00_Int2          trObjLockTransCnt_kb05;
    tkb05_RegionNo      trOwnRegion_kb05;
    tsp00_Int4          trFiller1_kb05;
    tsp00_8ByteCounter  trRowEntryAmount_kb05;
    tsp00_8ByteCounter  trTabEntryAmount_kb05;
    tsp00_8ByteCounter  trSqlReqTimeouts_kb05;
    tsp00_8ByteCounter  trObjReqTimeouts_kb05;
    tsp00_8ByteCounter  trDeadlocks_kb05;
    tsp00_8ByteCounter  trEscalations_kb05;
} tkb05_TransGlob;
typedef tkb05_TransGlob           *tkb05_TransGlobPtr;

typedef struct                     tkb05_RegionGlob
{
    tkb05_TransGlobPtr  rglTransGlob_kb05;
    tkb05_SupplyBlockPtr rglSupplyBlock_kb05;
    tkb05_RowHashPtr    rglRowHash_kb05;
    tkb05_TabHashPtr    rglTabHash_kb05;
    tkb05_ConsistViewPtr rglFirstConsistView_kb05;
    tkb05_SupplyItemPtr rglFirstFree_kb05;
    tkb05_ConsistViewPtr rglCancelConsView_kb05;

#if defined (BIT64)


#else

    tsp00_Addr          glFillerAddr_kb05;

#endif

    tkb05_SupplySegmPtr rglExtentFirstSegm_kb05;
    tkb05_SupplyItemPtr rglExtentFirstFree_kb05;
    tkb05_RegionNo      rglOwnRegion_kb05;
    tsp00_Int2          rglFiller1_kb05;
    tsp00_Int4          rglFreeItems_kb05;
    tsp00_Int4          rglExtentItems_kb05;
    tsp00_Int4          rglExtentFreeItems_kb05;
    tsp00_Int4          rglConsistViewCnt_kb05;
    tsp00_Int4          rglCancelCsViewCnt_kb05;
    tsp00_8ByteCounter  rglCollisions_kb05;
    tsp00_Int4          rglOpenTransCnt_kb05;
    tgg91_TransNo       rglMinOpenTransId_kb05;
    tgg91_TransNo       rglMinConsistViewNo_kb05;
    tgg91_TransNo       rglMinNonVersOpTrId_kb05;
    tgg91_TransNo       rglMinNonVersCSVNo_kb05;
    tsp00_Int4          rglFiller2_kb05;
} tkb05_RegionGlob;
typedef tkb05_RegionGlob          *tkb05_RegionGlobPtr;
    /**/
typedef ARRAY_TYPE_MSP00 (         tkb05_RgnGlobPtrList, tkb05_RegionGlobPtr, MAX_REGIONS_KB05);
    /**/
typedef tkb05_RgnGlobPtrList      *tkb05_RgnGlobPtrListPtr;

typedef struct                     tkb05_LockStateGlob
{
    tsp00_Int4          lglTimeLastClear_kb05;
    tsp00_RegionId      lglOwnRegionId_kb05;
} tkb05_LockStateGlob;
typedef tkb05_LockStateGlob       *tkb05_LockStateGlobPtr;

typedef struct                     tkb05_ObjCollisionEntry
{
    tgg00_TransIndex    objcOwnIndex_kb05;
    tgg00_TransIndex    objcFirstReq_kb05;
    tgg00_TransIndex    objcNextCollLock_kb05;
    tgg00_TransIndex    objcPrevCollLock_kb05;
    tgg91_TransNo       objcLockTransId_kb05;
    tgg00_TransIndex    objcLockTransIndex_kb05;
    tkb05_KernelOid     objcOid_kb05;
    tsp00_C8            objcObjContHndl_kb05;
} tkb05_ObjCollisionEntry;
typedef tkb05_ObjCollisionEntry   *tkb05_ObjCollEntryPtr;
    /**/
typedef ARRAY_TYPE_MSP00 (         tkb05_ObjCollList, tkb05_ObjCollisionEntry, MAX_TRANS_GG04);
    /**/
typedef tkb05_ObjCollList         *tkb05_ObjCollListPtr;

typedef struct                     tkb05_ObjReqEntry
{
    tgg91_TransNo       objrReqTransId_kb05;
    tgg00_TransIndex    objrOwnIndex_kb05;
    tgg00_TransIndex    objrCollEntryIndex_kb05;
    tgg00_TransIndex    objrNextReq_kb05;
    tsp00_TaskId        objrWaitTaskId_kb05;
    pasbool             objrIgnCnstViewCheck;
    pasbool             objrOmsShareLockReq;
} tkb05_ObjReqEntry;
typedef tkb05_ObjReqEntry         *tkb05_ObjReqEntryPtr;
    /**/
typedef ARRAY_TYPE_MSP00 (         tkb05_ObjReqList, tkb05_ObjReqEntry, MAX_TRANS_GG04);
    /**/
typedef tkb05_ObjReqList          *tkb05_ObjReqListPtr;

typedef struct                     tkb05_ObjCollGlob
{
    tkb05_ObjCollListPtr oglObjCollList_kb05;
    tkb05_ObjReqListPtr oglObjReqList_kb05;
    tgg00_TransIndex    oglObjCollFirstFree_kb05;
    tsp00_Int2          oglObjCollFreeCnt_kb05;
    tsp00_Int2          oglObjReqCnt_kb05;
    tsp00_Int2          oglFiller1_kb05;
    tsp00_Int4          oglFiller2_kb05;
    tsp00_RegionId      oglOwnRegionId_kb05;
    tsp00_8ByteCounter  oglCollisions_kb05;
} tkb05_ObjCollGlob;
typedef tkb05_ObjCollGlob         *tkb05_ObjCollGlobPtr;

typedef struct                     tkb05_Glob
{
    tkb05_RgnGlobPtrListPtr glRegionGlob_kb05;
    tkb05_SupplyPoolGlobPtr glSupplyPoolGlob_kb05;
    tkb05_LockStateGlobPtr glLockStateGlob_kb05;
    tkb05_ObjCollGlobPtr glObjCollGlob_kb05;
    tkb05_TransPtrListPtr glTransList_kb05;

#if defined (BIT64)


#else

    tsp00_Addr          glFillerAddr_kb05;

#endif

    pasbool             glGenerated_kb05;
    pasbool             glHistoryNeeded_kb05;
    tsp00_Int2          glRegions_kb05;
    tsp00_Int2          glRowRegions_kb05;
    tsp00_Int2          glTabRegions_kb05;
    tsp00_Int2          glTransRegions_kb05;
    tsp00_Int2          glTransEntries_kb05;
    tsp00_Int2          glTransEntriesPerRegion_kb05;
    tsp00_Int2          glTransHashEntriesPerRegion_kb05;
    tsp00_Int4          glFiller2_kb05;
    tsp00_Int4          glDeadlockDetection_kb05;
    tsp00_Int4          glItems_kb05;
    tsp00_Int4          glMaxRowsPerTab_kb05;
    tsp00_Int4          glSupplyItemsPerRegion_kb05;
    tsp00_Int4          glMaxItemsPerRegion_kb05;
    tsp00_Int4          glSupplyPoolSegments_kb05;
    tsp00_Int4          glSupplyPoolItems_kb05;
    tsp00_Int4          glRowHashEntriesPerRegion_kb05;
    tsp00_Int4          glTabHashEntriesPerRegion_kb05;
    tgg91_TransNo       glSelectTransGeneration_kb05;
    tsp00_Int2          glFiller3_kb05;
    tkb05_WantedLock    glNilWantedLock_kb05;
    tkb05_OpenTrans     glNilOpenTrans_kb05;
    tkb05_Req           glNilReq_kb05;
    tkb05_RowLock       glNilRowLock_kb05;
    tkb05_RowTrans      glNilRowTrans_kb05;
    tkb05_TabLock       glNilTabLock_kb05;
    tkb05_TabTrans      glNilTabTrans_kb05;
    tkb05_TransEntry    glNilTransEntry_kb05;
} tkb05_Glob;

typedef struct                     tkb05_UnivPtr
{
    union
    {
    /* 1 */
        struct tkb05_UnivPtr_1
        {
            tsp00_ObjAddr       uvAddr_kb05_F;
        } C_1;
    /* 2 */
        struct tkb05_UnivPtr_2
        {
            tkb05_LockStateGlobPtr uvLockStateGlob_kb05_F;
        } C_2;
    /* 3 */
        struct tkb05_UnivPtr_3
        {
            tkb05_ObjCollGlobPtr uvObjCollGlob_kb05_F;
        } C_3;
    /* 4 */
        struct tkb05_UnivPtr_4
        {
            tkb05_ObjCollListPtr uvObjCollList_kb05_F;
        } C_4;
    /* 5 */
        struct tkb05_UnivPtr_5
        {
            tkb05_ObjReqListPtr uvObjReqList_kb05_F;
        } C_5;
    /* 6 */
        struct tkb05_UnivPtr_6
        {
            tkb05_RegionGlobPtr uvRegionGlob_kb05_F;
        } C_6;
    /* 7 */
        struct tkb05_UnivPtr_7
        {
            tkb05_RgnGlobPtrListPtr uvRgnGlobPtrList_kb05_F;
        } C_7;
    /* 8 */
        struct tkb05_UnivPtr_8
        {
            tkb05_RowHashPtr    uvRowHash_kb05_F;
        } C_8;
    /* 9 */
        struct tkb05_UnivPtr_9
        {
            tkb05_SupplyBlockPtr uvSupplyBlock_kb05_F;
        } C_9;
    /* 10 */
        struct tkb05_UnivPtr_10
        {
            tkb05_SupplyPoolGlobPtr uvSupplyPoolGlob_kb05_F;
        } C_10;
    /* 11 */
        struct tkb05_UnivPtr_11
        {
            tkb05_SupplySegmPtr uvSupplySegm_kb05_F;
        } C_11;
    /* 12 */
        struct tkb05_UnivPtr_12
        {
            tkb05_SupplySegmPtrListPtr uvSupplySegmPtrList_kb05_F;
        } C_12;
    /* 13 */
        struct tkb05_UnivPtr_13
        {
            tkb05_TabHashPtr    uvTabHash_kb05_F;
        } C_13;
    /* 14 */
        struct tkb05_UnivPtr_14
        {
            tkb05_TransGlobPtr  uvTransGlob_kb05_F;
        } C_14;
    /* 15 */
        struct tkb05_UnivPtr_15
        {
            tkb05_TransListPtr  uvTransList_kb05_F;
        } C_15;
    /* 16 */
        struct tkb05_UnivPtr_16
        {
            tkb05_TransPtrListPtr uvTransPtrList_kb05_F;
        } C_16;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_ObjAddr      &uvAddr_kb05            ()  { return this->variant.C_1.uvAddr_kb05_F; };
    tkb05_LockStateGlobPtr &uvLockStateGlob_kb05   ()  { return this->variant.C_2.uvLockStateGlob_kb05_F; };
    tkb05_ObjCollGlobPtr &uvObjCollGlob_kb05     ()  { return this->variant.C_3.uvObjCollGlob_kb05_F; };
    tkb05_ObjCollListPtr &uvObjCollList_kb05     ()  { return this->variant.C_4.uvObjCollList_kb05_F; };
    tkb05_ObjReqListPtr &uvObjReqList_kb05      ()  { return this->variant.C_5.uvObjReqList_kb05_F; };
    tkb05_RegionGlobPtr &uvRegionGlob_kb05      ()  { return this->variant.C_6.uvRegionGlob_kb05_F; };
    tkb05_RgnGlobPtrListPtr &uvRgnGlobPtrList_kb05  ()  { return this->variant.C_7.uvRgnGlobPtrList_kb05_F; };
    tkb05_RowHashPtr   &uvRowHash_kb05         ()  { return this->variant.C_8.uvRowHash_kb05_F; };
    tkb05_SupplyBlockPtr &uvSupplyBlock_kb05     ()  { return this->variant.C_9.uvSupplyBlock_kb05_F; };
    tkb05_SupplyPoolGlobPtr &uvSupplyPoolGlob_kb05  ()  { return this->variant.C_10.uvSupplyPoolGlob_kb05_F; };
    tkb05_SupplySegmPtr &uvSupplySegm_kb05      ()  { return this->variant.C_11.uvSupplySegm_kb05_F; };
    tkb05_SupplySegmPtrListPtr &uvSupplySegmPtrList_kb05 ()  { return this->variant.C_12.uvSupplySegmPtrList_kb05_F; };
    tkb05_TabHashPtr   &uvTabHash_kb05         ()  { return this->variant.C_13.uvTabHash_kb05_F; };
    tkb05_TransGlobPtr &uvTransGlob_kb05       ()  { return this->variant.C_14.uvTransGlob_kb05_F; };
    tkb05_TransListPtr &uvTransList_kb05       ()  { return this->variant.C_15.uvTransList_kb05_F; };
    tkb05_TransPtrListPtr &uvTransPtrList_kb05    ()  { return this->variant.C_16.uvTransPtrList_kb05_F; };
    /* const accessor functions */
    const tsp00_ObjAddr &uvAddr_kb05            () const  { return this->variant.C_1.uvAddr_kb05_F; };
    const tkb05_LockStateGlobPtr &uvLockStateGlob_kb05   () const  { return this->variant.C_2.uvLockStateGlob_kb05_F; };
    const tkb05_ObjCollGlobPtr &uvObjCollGlob_kb05     () const  { return this->variant.C_3.uvObjCollGlob_kb05_F; };
    const tkb05_ObjCollListPtr &uvObjCollList_kb05     () const  { return this->variant.C_4.uvObjCollList_kb05_F; };
    const tkb05_ObjReqListPtr &uvObjReqList_kb05      () const  { return this->variant.C_5.uvObjReqList_kb05_F; };
    const tkb05_RegionGlobPtr &uvRegionGlob_kb05      () const  { return this->variant.C_6.uvRegionGlob_kb05_F; };
    const tkb05_RgnGlobPtrListPtr &uvRgnGlobPtrList_kb05  () const  { return this->variant.C_7.uvRgnGlobPtrList_kb05_F; };
    const tkb05_RowHashPtr &uvRowHash_kb05         () const  { return this->variant.C_8.uvRowHash_kb05_F; };
    const tkb05_SupplyBlockPtr &uvSupplyBlock_kb05     () const  { return this->variant.C_9.uvSupplyBlock_kb05_F; };
    const tkb05_SupplyPoolGlobPtr &uvSupplyPoolGlob_kb05  () const  { return this->variant.C_10.uvSupplyPoolGlob_kb05_F; };
    const tkb05_SupplySegmPtr &uvSupplySegm_kb05      () const  { return this->variant.C_11.uvSupplySegm_kb05_F; };
    const tkb05_SupplySegmPtrListPtr &uvSupplySegmPtrList_kb05 () const  { return this->variant.C_12.uvSupplySegmPtrList_kb05_F; };
    const tkb05_TabHashPtr &uvTabHash_kb05         () const  { return this->variant.C_13.uvTabHash_kb05_F; };
    const tkb05_TransGlobPtr &uvTransGlob_kb05       () const  { return this->variant.C_14.uvTransGlob_kb05_F; };
    const tkb05_TransListPtr &uvTransList_kb05       () const  { return this->variant.C_15.uvTransList_kb05_F; };
    const tkb05_TransPtrListPtr &uvTransPtrList_kb05    () const  { return this->variant.C_16.uvTransPtrList_kb05_F; };
#endif    /* defined (__cplusplus) */
} tkb05_UnivPtr;

typedef struct                     tkb05_ConsistViewShow
{
    tsp00_Int2          cshRecLen_kb05;
    tsp00_Int2          cshKeyLen_kb05;
    tsp00_Int2          cshVarcolOffset_kb05;
    tsp00_Int2          cshVarcolCnt_kb05;
    tsp00_Int4          cshKey_kb05;
    tgg91_TransNo       cshCreatorTrans_kb05;
    tgg91_TransNo       cshConsistView_kb05;
    tgg91_TransNo       cshOpenTrans_kb05;
    tsp00_Int4          cshStartDate_kb05;
    tsp00_Int4          cshStartTime_kb05;
    tsp00_Int4          cshTimeLastUsed_kb05;
    pasbool             cshCancelled_kb05;
    pasbool             cstFiller1_kb05;
} tkb05_ConsistViewShow;

typedef struct                     tkb05_TransShow
{
    tsp00_Int2          tshRecLen_kb05;
    tsp00_Int2          tshKeyLen_kb05;
    tsp00_Int2          tshVarcolOffset_kb05;
    tsp00_Int2          tshVarcolCnt_kb05;
    tsp00_Int4          tshKey_kb05;
    tsp00_TaskId        tshTaskId_kb05;
    tgg91_SessionNo     tshSession_kb05;
    tgg91_TransNo       tshWriteTransId_kb05;
    tsp00_Int2          tsh_filler1_kb05;
    tgg00_SubtransNo    tshSubtrans_kb05;
    tsp00_Int4          tsh_filler2_kb05;
    tsp00_Int4          tshStartDate_kb05;
    tsp00_Int4          tshStartTime_kb05;
    tgg91_TransNo       tshSelTransId_kb05;
    tsp00_TermId        tshTermId_kb05;
    tgg00_UserId        tshUserId_kb05;
    tsp00_Int4          tshReqTimeout_kb05;
    tsp00_Int4          tshTimeWrite_kb05;
    tkb05_TransLockMode tshTrLockMode_kb05;
    tkb05_TransLockMode tshTrReqMode_kb05;
    tgg00_LockReqMode   tshLockMode_kb05;
    tgg00_LockReqMode   tshReqMode_kb05;
    tgg00_LockReqState  tshLockState_kb05;
    tgg00_LockReqState  tshReqState_kb05;
    tgg91_TransNo       tshConsistView_kb05;
} tkb05_TransShow;

typedef struct                     tkb05_LockShow
{
    tkb05_TransShow     lshTransShow_kb05;
    tgg00_Surrogate     lshTabId_kb05;
    tkb05_RowId         lshRowId_kb05;
    tsp00_Int2          lshRowIdLen_kb05;
    pasbool             lshUndefRowId_kb05;
    pasbool             lshFiller1_kb05;
} tkb05_LockShow;

typedef struct                     tkb05_ShowRec
{
    union
    {
    /* 1 */
        struct tkb05_ShowRec_1
        {
            tkb05_ConsistViewShow shrConsist_kb05_F;
        } C_1;
    /* 2 */
        struct tkb05_ShowRec_2
        {
            tkb05_LockShow      shrLock_kb05_F;
        } C_2;
    /* 3 */
        struct tkb05_ShowRec_3
        {
            tkb05_TransShow     shrTrans_kb05_F;
        } C_3;
    /* 4 */
        struct tkb05_ShowRec_4
        {
            tgg00_Rec           shrRec_kb05_F;
        } C_4;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tkb05_ConsistViewShow &shrConsist_kb05        ()  { return this->variant.C_1.shrConsist_kb05_F; };
    tkb05_LockShow     &shrLock_kb05           ()  { return this->variant.C_2.shrLock_kb05_F; };
    tkb05_TransShow    &shrTrans_kb05          ()  { return this->variant.C_3.shrTrans_kb05_F; };
    tgg00_Rec          &shrRec_kb05            ()  { return this->variant.C_4.shrRec_kb05_F; };
    /* const accessor functions */
    const tkb05_ConsistViewShow &shrConsist_kb05        () const  { return this->variant.C_1.shrConsist_kb05_F; };
    const tkb05_LockShow &shrLock_kb05           () const  { return this->variant.C_2.shrLock_kb05_F; };
    const tkb05_TransShow &shrTrans_kb05          () const  { return this->variant.C_3.shrTrans_kb05_F; };
    const tgg00_Rec    &shrRec_kb05            () const  { return this->variant.C_4.shrRec_kb05_F; };
#endif    /* defined (__cplusplus) */
} tkb05_ShowRec;
typedef tsp00_C12                  tkb05_ShtHeader;
typedef OLDPASCALSTRING_TYPE_MSP00 (  tkb05_ShtInfo, 256);
typedef CSTRING_TYPE_MSP00      (  tkb05_ShtInfoc, 256);
typedef tsp00_C40                  tkb05_ShtDesc;
typedef tsp00_C80                  tkb05_ShtDescUnicode;
typedef tsp00_C12                  tkb05_ShtVal;
typedef tsp00_C24                  tkb05_ShtValUnicode;

typedef struct                     tkb05_ShowStatRec
{
    union
    {
    /* 1 */
        struct tkb05_ShowStatRec_1
        {
            tsp00_Int2          shtRecLen_kb05_F;
            tsp00_Int2          shtKeyLen_kb05_F;
            tsp00_Int2          shtVarcolOffset_kb05_F;
            tsp00_Int2          shtVarcolCnt_kb05_F;
            tsp00_C4            shtKey_kb05_F;
            tsp00_C1            shtDefByte_kb05_F;
            tkb05_ShtInfo       shtInfo_kb05_F;
        } C_1;
    /* 2 */
        struct tkb05_ShowStatRec_2
        {
            tkb05_ShtHeader     shtHeader_kb05_F;
            tsp00_C1            shtSpace1_kb05_F;
            tkb05_ShtDesc       shtDesc_kb05_F;
            tsp00_C1            shtDefByte2_kb05_F;
            tkb05_ShtVal        shtVal_kb05_F;
        } C_2;
    /* 3 */
        struct tkb05_ShowStatRec_3
        {
            tkb05_ShtHeader     shtSpace2_kb05_F;
            tsp00_C1            shtSpace3_kb05_F;
            tkb05_ShtDescUnicode shtDescUnicode_kb05_F;
            tsp00_C1            shtDefByte2Unicode_kb05_F;
            tkb05_ShtValUnicode shtValUnicode_kb05_F;
        } C_3;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_Int2         &shtRecLen_kb05         ()  { return this->variant.C_1.shtRecLen_kb05_F; };
    tsp00_Int2         &shtKeyLen_kb05         ()  { return this->variant.C_1.shtKeyLen_kb05_F; };
    tsp00_Int2         &shtVarcolOffset_kb05   ()  { return this->variant.C_1.shtVarcolOffset_kb05_F; };
    tsp00_Int2         &shtVarcolCnt_kb05      ()  { return this->variant.C_1.shtVarcolCnt_kb05_F; };
    tsp00_C4           &shtKey_kb05            ()  { return this->variant.C_1.shtKey_kb05_F; };
    tsp00_C1           &shtDefByte_kb05        ()  { return this->variant.C_1.shtDefByte_kb05_F; };
    tkb05_ShtInfo      &shtInfo_kb05           ()  { return this->variant.C_1.shtInfo_kb05_F; };
    tkb05_ShtHeader    &shtHeader_kb05         ()  { return this->variant.C_2.shtHeader_kb05_F; };
    tsp00_C1           &shtSpace1_kb05         ()  { return this->variant.C_2.shtSpace1_kb05_F; };
    tkb05_ShtDesc      &shtDesc_kb05           ()  { return this->variant.C_2.shtDesc_kb05_F; };
    tsp00_C1           &shtDefByte2_kb05       ()  { return this->variant.C_2.shtDefByte2_kb05_F; };
    tkb05_ShtVal       &shtVal_kb05            ()  { return this->variant.C_2.shtVal_kb05_F; };
    tkb05_ShtHeader    &shtSpace2_kb05         ()  { return this->variant.C_3.shtSpace2_kb05_F; };
    tsp00_C1           &shtSpace3_kb05         ()  { return this->variant.C_3.shtSpace3_kb05_F; };
    tkb05_ShtDescUnicode &shtDescUnicode_kb05    ()  { return this->variant.C_3.shtDescUnicode_kb05_F; };
    tsp00_C1           &shtDefByte2Unicode_kb05 ()  { return this->variant.C_3.shtDefByte2Unicode_kb05_F; };
    tkb05_ShtValUnicode &shtValUnicode_kb05     ()  { return this->variant.C_3.shtValUnicode_kb05_F; };
    /* const accessor functions */
    const tsp00_Int2   &shtRecLen_kb05         () const  { return this->variant.C_1.shtRecLen_kb05_F; };
    const tsp00_Int2   &shtKeyLen_kb05         () const  { return this->variant.C_1.shtKeyLen_kb05_F; };
    const tsp00_Int2   &shtVarcolOffset_kb05   () const  { return this->variant.C_1.shtVarcolOffset_kb05_F; };
    const tsp00_Int2   &shtVarcolCnt_kb05      () const  { return this->variant.C_1.shtVarcolCnt_kb05_F; };
    const tsp00_C4     &shtKey_kb05            () const  { return this->variant.C_1.shtKey_kb05_F; };
    const tsp00_C1     &shtDefByte_kb05        () const  { return this->variant.C_1.shtDefByte_kb05_F; };
    const tkb05_ShtInfo &shtInfo_kb05           () const  { return this->variant.C_1.shtInfo_kb05_F; };
    const tkb05_ShtHeader &shtHeader_kb05         () const  { return this->variant.C_2.shtHeader_kb05_F; };
    const tsp00_C1     &shtSpace1_kb05         () const  { return this->variant.C_2.shtSpace1_kb05_F; };
    const tkb05_ShtDesc &shtDesc_kb05           () const  { return this->variant.C_2.shtDesc_kb05_F; };
    const tsp00_C1     &shtDefByte2_kb05       () const  { return this->variant.C_2.shtDefByte2_kb05_F; };
    const tkb05_ShtVal &shtVal_kb05            () const  { return this->variant.C_2.shtVal_kb05_F; };
    const tkb05_ShtHeader &shtSpace2_kb05         () const  { return this->variant.C_3.shtSpace2_kb05_F; };
    const tsp00_C1     &shtSpace3_kb05         () const  { return this->variant.C_3.shtSpace3_kb05_F; };
    const tkb05_ShtDescUnicode &shtDescUnicode_kb05    () const  { return this->variant.C_3.shtDescUnicode_kb05_F; };
    const tsp00_C1     &shtDefByte2Unicode_kb05 () const  { return this->variant.C_3.shtDefByte2Unicode_kb05_F; };
    const tkb05_ShtValUnicode &shtValUnicode_kb05     () const  { return this->variant.C_3.shtValUnicode_kb05_F; };
#endif    /* defined (__cplusplus) */
} tkb05_ShowStatRec;

typedef struct                     tkb05_LockInfo
{
    tsp00_8ByteCounter  infSqlCollisions_kb05;
    tsp00_8ByteCounter  infOmsCollisions_kb05;
    tsp00_8ByteCounter  infEscalations_kb05;
    tsp00_8ByteCounter  infDeadlocks_kb05;
    tsp00_8ByteCounter  infSqlReqTimeouts_kb05;
    tsp00_8ByteCounter  infObjReqTimeouts_kb05;
    tsp00_8ByteCounter  infRowEntryAmount_kb05;
    tsp00_8ByteCounter  infTabEntryAmount_kb05;
    tsp00_8ByteCounter  infKbCalls_kb05;
    tsp00_Int4          infLockTrans_kb05;
    tsp00_Int4          infReqTrans_kb05;
    tsp00_Int4          infEscalValue_kb05;
    tsp00_Int4          infObjLockTrans_kb05;
    tsp00_Int4          infMaxEntries_kb05;
    tsp00_Int4          infEntriesUsed_kb05;
    tsp00_Int4          infAvgUsed_kb05;
    tsp00_Int4          infMaxUsed_kb05;
    tsp00_Int4          infConsistViewCnt_kb05;
    tsp00_Int4          infOpenTransCnt_kb05;
} tkb05_LockInfo;
    /* following entries are used for object share locks   PTS 1128244 FF 2004-03-18 */
typedef struct tkb05_ObjShareLock tkb05_ObjShareLock;
typedef tkb05_ObjShareLock        *tkb05_ObjShareLockPtr;

 struct                     tkb05_ObjShareLock
{
    tkb05_ObjShareLockPtr objrNxtShareLock_kb05;
    tgg91_TransNo       objrSlockTransId_kb05;
    tgg00_TransIndex    objrSlockTransIndex_kb05;
    pasbool             objrLockRollbacked;
    tsp00_Uint1         objrFiller1_kb05;
    tsp00_Int2          objrFiller2_kb05;
    tsp00_Int2          objrFiller3_kb05;
    tsp00_Int2          objrOwnIndex_kb05;
} ;
typedef struct tkb05_ObjShareLockCtrl tkb05_ObjShareLockCtrl;
typedef tkb05_ObjShareLockCtrl    *tkb05_ObjShareLockCtrlPtr;

 struct                     tkb05_ObjShareLockCtrl
{
    tkb05_ObjShareLockCtrlPtr objsNxtShareLockCtrl_kb05;
    tkb05_ObjShareLockPtr objsObjShareLock_kb05;
    tkb05_KernelOid     objsOid_kb05;
    tgg00_TransIndex    objsCollLock_kb05;
    tgg00_TransIndex    objsExclLockTransIndex_kb05;
    tgg91_TransNo       objsExclLockTransId_kb05;
    tsp00_Int2          objsInuseWithoutLock_kb05;
    pasbool             objsReleaseLocks_kb05;
    tsp00_Uint1         objsFiller1_kb05;
    tsp00_Int2          objsOwnIndex_kb05;
    union
    {
    /* 1 */
        struct tkb05_ObjShareLockCtrl_1
        {
            tsp00_C8            objsFileDirFileId_kb05_F;
        } C_1;
    /* 2 */
        struct tkb05_ObjShareLockCtrl_2
        {
            tgg00_ObjFileNo     objsFileObjFileNo_kb05_F;
            tsp00_Int4          objsFiller2_kb05_F;
        } C_2;
    } variant;
#if defined (__cplusplus)
    /* accessor functions */
    tsp00_C8           &objsFileDirFileId_kb05 ()  { return this->variant.C_1.objsFileDirFileId_kb05_F; };
    tgg00_ObjFileNo    &objsFileObjFileNo_kb05 ()  { return this->variant.C_2.objsFileObjFileNo_kb05_F; };
    tsp00_Int4         &objsFiller2_kb05       ()  { return this->variant.C_2.objsFiller2_kb05_F; };
    /* const accessor functions */
    const tsp00_C8     &objsFileDirFileId_kb05 () const  { return this->variant.C_1.objsFileDirFileId_kb05_F; };
    const tgg00_ObjFileNo &objsFileObjFileNo_kb05 () const  { return this->variant.C_2.objsFileObjFileNo_kb05_F; };
    const tsp00_Int4   &objsFiller2_kb05       () const  { return this->variant.C_2.objsFiller2_kb05_F; };
#endif    /* defined (__cplusplus) */
} ;

struct tkb05_TemplateDummy {
    tkb05_LocklistState  dummy_tkb05_LocklistState;
    tkb05_TransLockMode  dummy_tkb05_TransLockMode;
    tkb05_ItemKind       dummy_tkb05_ItemKind;
    tkb05_Handling       dummy_tkb05_Handling;
    tkb05_TransInfo      dummy_tkb05_TransInfo;
};


#endif
