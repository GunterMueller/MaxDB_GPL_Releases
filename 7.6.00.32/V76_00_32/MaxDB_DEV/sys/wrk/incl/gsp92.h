
/* some common prolog for every module
 */

#ifndef GSP92_H
#define GSP92_H


/******************************************
 *            I N C L U D E S             *
 ******************************************/


#ifndef GSP00_2_H
#include "gsp00_2.h"
#endif

#ifndef GSP00_H
#include "gsp00.h"
#endif

/******************************************
 *           C O N S T A N T S            *
 ******************************************/


/******************************************
 *               T Y P E S                *
 ******************************************/


typedef struct                     tsp92_RteRegionInfo
{
    tsp00_Longint       WaiterArray;
    tsp00_Int4          NumWaiter;
    tsp00_Int4          LastWaiterGiven;
    tsp00_Int4          LastRegionGiven;
    tsp00_Int4          RegionNo;
    tsp00_Region        RegionName;
    tsp00_Int4          OwnerTaskIndex;
    tsp00_Int4          WaitTaskIndex;
    tsp00_8ByteCounter  GetCount;
    tsp00_8ByteCounter  TasCount;
    tsp00_8ByteCounter  WaitCount;
    tsp00_8ByteCounter  CollisionCount;
    tsp00_Longreal      CollisionRate;
    tsp00_Int4          Exclusive;
} tsp92_RteRegionInfo;

typedef struct                     tsp92_RteIOInfo
{
    tsp00_Int4          NextDevNo;
    tsp00_Int4          DevNo;
    tsp00_8ByteCounter  ReadCnt;
    tsp00_8ByteCounter  ReadPageCnt;
    tsp00_8ByteCounter  WriteCnt;
    tsp00_8ByteCounter  WritePageCnt;
    tsp00_VFilename     DevspaceName;
} tsp92_RteIOInfo;

typedef struct                     tsp92_RteBackupInfo
{
    tsp00_Int4          NextDevNo;
    tsp00_Int4          DevNo;
    tsp00_8ByteCounter  ReadCnt;
    tsp00_8ByteCounter  WriteCnt;
    tsp00_8ByteCounter  PageCnt;
    tsp00_Int4          QueLen;
    tsp00_Int4          MaxQueLen;
    tsp00_VFilename     DevspaceName;
} tsp92_RteBackupInfo;

typedef struct                     tsp92_RteStorageInfo
{
    tsp00_Int4          StorageIdx;
    tsp00_Int4          StorageValue;
    tsp00_C64           StorageName;
    tsp00_C6            StorageUnit;
} tsp92_RteStorageInfo;

typedef struct                     tsp92_RteSpecThreadsInfo
{
    tsp00_Int4          SpecThreadIdx;
    tsp00_Int4          SpecThreadId;
    tsp00_C16           SpecThreadName;
} tsp92_RteSpecThreadsInfo;

typedef struct                     tsp92_RteUKTInfo
{
    tsp00_Longint       ActUKT;
    tsp00_Longint       LastUKT;
    tsp00_Int4          UKTId;
    tsp00_C16           UKTName;
    tsp00_C16           UKTState;
    tsp00_C64           UKTTaskCluster;
    tsp00_Int4          UKTActiveTasks;
    tsp00_Int4          UKTTotalTasks;
    tsp00_8ByteCounter  UKTDispatcherCnt;
    tsp00_8ByteCounter  UKTTaskSwitchCnt;
    tsp00_8ByteCounter  UKTIdleCount;
    tsp00_8ByteCounter  UKTCommandCnt;
    tsp00_Int4          UKTRunQueueLen;
} tsp92_RteUKTInfo;

typedef struct                     tsp92_RteIODetailsInfo
{
    tsp00_Longint       ActUKT;
    tsp00_Longint       LastUKT;
    tsp00_Longint       ActDEV;
    tsp00_Longint       LastDEV;
    tsp00_Int4          InternalDevno;
    tsp00_Int4          DevNo;
    tsp00_8ByteCounter  ReadCnt;
    tsp00_8ByteCounter  WriteCnt;
    tsp00_Int4          QueLen;
    tsp00_Int4          MaxQueLen;
    tsp00_Int4          ThreadId;
    tsp00_C16           ThreadName;
    tsp00_VFilename     DevspaceName;
} tsp92_RteIODetailsInfo;

typedef enum tsp92_RteTaskInfoStateToShow_Enum
{
    sp92State_All,
    sp92State_Active,
    sp92State_Runnable
} tsp92_RteTaskInfoStateToShow_Enum;
typedef tsp92_RteTaskInfoStateToShow_Enum tsp92_RteTaskInfoStateToShow_Param;
typedef ENUM_TYPE_MSP00 (          tsp92_RteTaskInfoStateToShow, tsp92_RteTaskInfoStateToShow_Enum, sp92State_Runnable, unsigned char);

typedef enum tsp92_RteTaskInfoTypeToShow_Enum
{
    sp92Type_All,
    sp92Type_User,
    sp92Type_Server,
    sp92Type_DataWriter
} tsp92_RteTaskInfoTypeToShow_Enum;
typedef tsp92_RteTaskInfoTypeToShow_Enum tsp92_RteTaskInfoTypeToShow_Param;
typedef ENUM_TYPE_MSP00 (          tsp92_RteTaskInfoTypeToShow, tsp92_RteTaskInfoTypeToShow_Enum, sp92Type_DataWriter, unsigned char);

typedef struct                     tsp92_RteTaskInfo
{
    tsp00_Longint       ActTask;
    tsp00_Longint       LastTask;
    tsp00_Int4          TaskTypeToShow;
    tsp00_Int4          TaskStateToShow;
    tsp00_Int4          UKTId;
    tsp00_Int4          TaskIndex;
    tsp00_Int4          ApplPid;
    tsp00_C16           TaskName;
    tsp00_C16           TaskState;
    tsp00_C64           ExtendedTaskState;
    tsp00_Int4          TimeOut;
    tsp00_Int4          HoldingRegion;
    tsp00_Int4          WaitingForTask;
    tsp00_Int4          WaitingForRoot;
    tsp00_PageNo        LockedPageNo;
    tsp00_C1            CancelFlag;
    tsp00_C1            OMSCallActive;
    tsp00_C1            SpecialFlag;
} tsp92_RteTaskInfo;

typedef struct                     tsp92_RteTotalCntInfo
{
    tsp00_8ByteCounter  DispatcherCnt;
    tsp00_8ByteCounter  CommandCnt;
    tsp00_8ByteCounter  VwaitCnt;
    tsp00_8ByteCounter  VsuspendCnt;
    tsp00_8ByteCounter  VsleepCnt;
    tsp00_8ByteCounter  PrioCnt;
    tsp00_8ByteCounter  UserTaskReadCnt;
    tsp00_8ByteCounter  UserTaskWriteCnt;
    tsp00_8ByteCounter  TaskIOCnt;
    tsp00_8ByteCounter  DevIOCnt;
    tsp00_8ByteCounter  RegionAccessCnt;
    tsp00_8ByteCounter  RegionCollCnt;
    tsp00_8ByteCounter  RegionWaitCnt;
    tsp00_8ByteCounter  VbegexclTasCollCnt;
    tsp00_8ByteCounter  VendexclTasCollCnt;
    tsp00_8ByteCounter  TaskSelfSuspCnt;
    tsp00_8ByteCounter  SymbolResolutionCnt;
} tsp92_RteTotalCntInfo;

typedef struct                     tsp92_RteTaskSelfIOInfo
{
    tsp00_8ByteCounter  Cnt;
    tsp00_8ByteCounter  Pages;
    tsp00_Longreal      AvgAbsTime;
    tsp00_Longreal      TotAbsTime;
} tsp92_RteTaskSelfIOInfo;

typedef struct                     tsp92_RteTaskDEVIOInfo
{
    tsp00_8ByteCounter  Cnt;
    tsp00_8ByteCounter  Pages;
    tsp00_Longreal      AvgRelTime;
    tsp00_Longreal      TotRelTime;
    tsp00_Longreal      AvgAbsTime;
    tsp00_Longreal      TotAbsTime;
} tsp92_RteTaskDEVIOInfo;

typedef struct                     tsp92_RteTaskStateInfo
{
    tsp00_8ByteCounter  Cnt;
    tsp00_Longreal      AvgRelTime;
    tsp00_Longreal      TotRelTime;
    tsp00_Longreal      AvgAbsTime;
    tsp00_Longreal      TotAbsTime;
} tsp92_RteTaskStateInfo;

typedef struct                     tsp92_RteTaskDetailsInfo
{
    tsp00_Longint       ActTask;
    tsp00_Longint       LastTask;
    tsp00_Int4          TaskIndex;
    tsp00_C16           TaskName;
    tsp00_C16           TaskState;
    tsp00_Int4          UKTId;
    tsp00_C64           RemoteApplNode;
    tsp00_Int4          ApplPid;
    tsp00_8ByteCounter  DispatcherCnt;
    tsp00_8ByteCounter  CommandCnt;
    tsp00_8ByteCounter  PrioCnt;
    tsp00_8ByteCounter  TaskSelfSuspCnt;
    tsp00_8ByteCounter  RegionAccessCnt;
    tsp92_RteTaskSelfIOInfo SelfIORead;
    tsp92_RteTaskSelfIOInfo SelfIOWrite;
    tsp92_RteTaskDEVIOInfo DEVIORead;
    tsp92_RteTaskDEVIOInfo DEVIOWrite;
    tsp92_RteTaskDEVIOInfo BackupRead;
    tsp92_RteTaskDEVIOInfo BackupWrite;
    tsp92_RteTaskStateInfo Vsuspend;
    tsp92_RteTaskStateInfo Vwait;
    tsp92_RteTaskStateInfo Vsleep;
    tsp92_RteTaskStateInfo ReplyReceive;
    tsp92_RteTaskStateInfo ReceiveReply;
    tsp00_KnlIdentifier COMProcedureName;
    tsp00_8ByteCounter  OMSCallbackCount;
    tsp00_C32           OMSMCallbackMethodName;
} tsp92_RteTaskDetailsInfo;

typedef struct                     tsp92_RteAllocatorInfo
{
    tsp00_Addr          InfoArray;
    tsp00_Int4          LastIndex;
    tsp00_Int4          InfoIndex;
    tsp00_C40           AllocatorName;
    tsp00_8ByteCounter  UsedBytes;
    tsp00_8ByteCounter  MaxUsedBytes;
    tsp00_8ByteCounter  AllocatedBytes;
    tsp00_8ByteCounter  CountAlloc;
    tsp00_8ByteCounter  CountDealloc;
    tsp00_8ByteCounter  BaseCountAlloc;
    tsp00_8ByteCounter  BaseCountDealloc;
    tsp00_Int4          ErrorCount;
    tsp00_C40           AllocatorBaseName;
} tsp92_RteAllocatorInfo;

typedef struct                     tsp92_RteSpinlockInfo
{
    tsp00_Addr          InfoArray;
    tsp00_Int4          LastIndex;
    tsp00_Int4          InfoIndex;
    tsp00_C40           SpinlockName;
    tsp00_8ByteCounter  Locks;
    tsp00_8ByteCounter  Collisions;
    tsp00_Int4          TotalSpinLoops;
    tsp00_Int4          TotalYieldLoops;
    tsp00_Int4          MaxSpinLoops;
    tsp00_Int4          MaxYieldLoops;
    tsp00_Int4          CurrentLoops;
} tsp92_RteSpinlockInfo;

typedef struct                     tsp92_RteConnectionInfo
{
    tsp00_Int4          TaskIndex;
    tsp00_Int4          UKTId;
    tsp00_Int4          ApplPid;
    tsp00_Int4          PeerPid;
    tsp00_C16           TaskName;
    tsp00_C32           ConnectState;
    tsp00_C32           ConnectTime;
    tsp00_C64           RemoteApplNode;
} tsp92_RteConnectionInfo;

struct tsp92_TemplateDummy {
    tsp92_RteTaskInfoStateToShow dummy_tsp92_RteTaskInfoStateToShow;
    tsp92_RteTaskInfoTypeToShow dummy_tsp92_RteTaskInfoTypeToShow;
};


#endif
