/****************************************************************************

  module      : LVC_KernelInterfaceInstance.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: liveCache  
  description : kernel interface provided for OMS


  last changed: 2000-07-11  13:54
  see also    : example.html ...
  first created:2000-05-26  19:32

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end




*****************************************************************************/

#include "liveCache/LVC_KernelInterfaceInstance.hpp"
#include "liveCache/MemoryManagement/LVCMem_Allocator.hpp"
#include "liveCache/MemoryManagement/LVCMem_Wrapper.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_Exceptions.hpp"
#include "KernelCommon/Kernel_OpMsg.hpp"
#include "Oms/OMS_StackTraceCallbackInterface.hpp"
#include "Oms/OMS_DbpError.hpp"
#include "RunTime/System/RTESys_MicroTime.h"
#include "RunTime/RTE_IInterface.hpp"
#include "RunTime/Configuration/RTEConf_ParameterAccessKernelInterface.hpp"
#include "KernelCommon/MemoryManagement/KernelMem_RawAllocatorTracer.hpp"
#include "KernelCommon/Kernel_VTrace.hpp"

#include "vak001.h"
#include "hgg01.h"
#include "hgg01_3.h"
#include "heo52.h"  // vclock
#include "heo56.h"  // vsignal
#include "heo58.h"  // vGetAcvPtrFromCurrentTask
#include "geo573.h" // bad alloc
#include "hgg11.h"
#include "heo670.h"
#include "hsp77.h"
#include "heo51.h"  // vgetpid

#ifdef WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

LVC_KernelInterfaceInstance* LVC_KernelInterfaceInstance::m_Instance = 0;
int                          LVC_KernelInterfaceInstance::m_omsVersionUnloads = 0;

/*-----------------------------------------------------*/

LVC_KernelInterface& LVC_KernelInterfaceInstance::Instance()
{
  SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(LVC_KernelInterfaceInstance));
  if (0 == m_Instance)
  {
    m_Instance = new(Space) LVC_KernelInterfaceInstance;
  }
  return *m_Instance;
}

/*-----------------------------------------------------*/

LVC_KernelInterfaceInstance::LVC_KernelInterfaceInstance()
  : m_rw_locks(RTE_IInterface::Instance().Allocator())
{
}

/*-----------------------------------------------------*/

void* LVC_KernelInterfaceInstance::Allocate(SAPDB_ULong count, bool isUserRequest)
{
    return TheLiveCacheAllocator.AllocateImplementation (count, isUserRequest);
}

/*-----------------------------------------------------*/

void* LVC_KernelInterfaceInstance::AllocateThrowsNothing(SAPDB_ULong count)
{
    return TheLiveCacheAllocator.AllocateThrowsNothing(count);
}

/*-----------------------------------------------------*/

void* LVC_KernelInterfaceInstance::AllocateArray(SAPDB_ULong count, bool isUserRequest)
{
    return TheLiveCacheAllocator.AllocateImplementation (count, isUserRequest);
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::CalcAllocatorStatistics(SAPDB_ULong &BytesUsed, SAPDB_ULong &MaxBytesUsed, SAPDB_ULong &BytesControlled)
{
    TheLiveCacheAllocator.CalcStatistics(BytesUsed, MaxBytesUsed, BytesControlled);
}

/*-----------------------------------------------------*/

tsp00_Int4 LVC_KernelInterfaceInstance::Clock()
{
  tsp00_Int4 Sec;
  tsp00_Int4 MSec; 
  vclock(&Sec, &MSec); 
  return Sec;
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::Crash(const char* msg)
{
    KernelMem_RawAllocatorTracer::GetTracer().Exception(msg);
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::Deallocate(void* p, bool isUserRequest)
{
    TheLiveCacheAllocator.DeallocateImplementation (p, isUserRequest);
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::DeallocateArray(void* p, bool isUserRequest)
{
    TheLiveCacheAllocator.DeallocateImplementation (p, isUserRequest);
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::DeregisterAllocator(RTEMem_AllocatorInfo& allocatorInfo)
{
    RTEMem_AllocatorRegister::Instance().Deregister(allocatorInfo); 
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::DumpChunk (void* p, int size)
{
    KernelMem_RawAllocatorTracer::GetTracer().Dump(p, size);
}


/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::GetAllocatorCallStatistics(SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc) const
{
    TheLiveCacheAllocator.GetCallStatistics(CountAlloc, CountDealloc);
}

/*-----------------------------------------------------*/

const SAPDB_UTF8* LVC_KernelInterfaceInstance::GetAllocatorIdentifier() const
{
  return TheLiveCacheAllocator.GetIdentifier();
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc) const
{
    TheLiveCacheAllocator.GetBaseAllocatorCallStatistics(CountAlloc, CountDealloc);
}
  
/*-----------------------------------------------------*/

SAPDB_UInt4 LVC_KernelInterfaceInstance::GetHeapCheckLevel()
{
    RTEConf_Parameter::Integer value;
    SAPDBErr_MessageList       errorList;
    if (RTEConf_ParameterAccess::Instance()->GetInteger((unsigned char*) "HEAP_CHECK_LEVEL", value, errorList))
    {
        return (SAPDB_UInt4) value;
    }
    else
    {
        // there's any problem reading parameter, run without any checks
        return 0;
    }
}


/*-----------------------------------------------------*/

bool LVC_KernelInterfaceInstance::IsOneDBSystem(short &rc)
{   
    RTEConf_Parameter::String value;
    SAPDBErr_MessageList       errorList;
    if (RTEConf_ParameterAccess::Instance()->GetString((unsigned char*) "MCOD", value, errorList)){
      if( STRCMP_UTF8((const SAPDB_UTF8 *)"YES",value) == 0) {
            rc = e_ok;   
            return true;
        }
        else {
            rc = e_ok;
            return false;
        }
    }

    // there's any problem reading parameter
    if (!errorList.IsEmpty()) {
        RTE_Message(errorList);
    }
    rc = e_OMS_system_error;
    return false;
}

/*-----------------------------------------------------*/

bool LVC_KernelInterfaceInstance::HeapThresholdReached()
{
  SAPDB_ULong BytesUsed;
  SAPDB_ULong MaxBytesUsed;
  SAPDB_ULong BytesControlled;
  if (0 == g01oms_heap_limit())
  {
	  return false;
  }
  TheLiveCacheAllocator.CalcStatistics(BytesUsed, MaxBytesUsed, BytesControlled);
  double percent = (double) BytesUsed * 100.0 / (double) (g01oms_heap_limit() * 1024);
  return(percent > g01oms_heap_usage_threshold());
}

/*-----------------------------------------------------*/

void* LVC_KernelInterfaceInstance::GetKnlVersion(void* pBuf, int bufSize)
{
  if (bufSize >= sizeof(tsp00_Version) + 1)
  {
    g11kernel_version(*REINTERPRET_CAST(tsp00_Version*, pBuf));
    char* pVersion = REINTERPRET_CAST(char*, pBuf) + sizeof(tsp00_Version);
    do
      --pVersion;
    while (' ' == *pVersion);
    *pVersion = 0;
    return pBuf;
  }
  return NULL;
}

/*-----------------------------------------------------*/

tsp00_Int4 LVC_KernelInterfaceInstance::GetLockRequestTimeout()
{
  return g01timeout.timeReq_gg00;
}

/*-----------------------------------------------------*/

int LVC_KernelInterfaceInstance::GetOmsRegionCnt()
{
  return g01region_cnt(rgnOMS_egg00);
}

/*-----------------------------------------------------*/

IliveCacheSink* LVC_KernelInterfaceInstance::GetSinkPtr()
{
   tak_all_command_glob* pAcv = REINTERPRET_CAST(tak_all_command_glob*, vGetAcvPtrFromCurrentTask());
   return REINTERPRET_CAST(IliveCacheSink*, &pAcv->a_kernel_sink);
}

/*-----------------------------------------------------*/

tsp00_Uint4 LVC_KernelInterfaceInstance::GetOmsVersionThreshold()
{
  return g01oms_vers_free_threshold();
}

/*-----------------------------------------------------*/

tsp00_Int4 LVC_KernelInterfaceInstance::GetPacketMaxUsableArea()
{
     tak_all_command_glob* pAcv = REINTERPRET_CAST(tak_all_command_glob*, vGetAcvPtrFromCurrentTask());
     return pAcv->a_reply_packet_size - sizeof (tsp1_packet_header) -
         sizeof (tsp1_segment_header) - sizeof (tsp1_part_header) + 2 * sizeof(tsp00_Int4) - sizeof (tsp1_part_header);
}

/*-----------------------------------------------------*/

int LVC_KernelInterfaceInstance::GetOmsVersionUnloads()
{
    return m_Instance->m_omsVersionUnloads;
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::IncOmsVersionUnloadCounter()
{
  ++m_omsVersionUnloads;
}

/*-----------------------------------------------------*/

bool LVC_KernelInterfaceInstance::IsUnicodeInstance()
{
    return ( g01unicode != 0 );
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::RegisterAllocator(RTEMem_AllocatorInfo& allocatorInfo)
{
    RTEMem_AllocatorRegister::Instance().Register(allocatorInfo); 
}

/*-----------------------------------------------------*/

void  LVC_KernelInterfaceInstance::ResetOmsVersionUnloadCounter()
{
    m_Instance->m_omsVersionUnloads = 0;
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::Signal(tsp00_TaskId taskId, bool timeout)
{
    vsignal(taskId, !timeout);
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::SignalUnloadLiboms()
{
    TheLiveCacheAllocator.DisableClientInquiry();
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::TestBadAlloc()
{
    if (LVCMem_Wrapper::BadAllocRequired(vGetAcvPtrFromCurrentTask()))
    {
        THROW_BAD_ALLOC_GEO573;
    }
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::SimCtlAttachSink(OmsHandle &handle)
{
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::ConsoleError(const char *msg)
{
    KernelMem_RawAllocatorTracer::GetTracer().Trace(msg);
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::ConsoleMessage(const char *msg)
{
  	Kernel_OpInfo(csp3_n_obj) << msg;
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::DumpStackTrace()
{
  eo670_CTraceStack();
}

/*-----------------------------------------------------*/

unsigned int LVC_KernelInterfaceInstance::SaveStackTrace(
  void **buffer, unsigned int levels)
{
  unsigned int saved = eo670_FillCallArray(levels, buffer);
  levels -= saved;
  buffer += saved;
  while (levels--) {
    *(buffer++) = NULL;
  }
  return saved;
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::EvalStackTrace(
  void **buffer, unsigned int levels,
  OmsStackTraceCallbackInterface *cb)
{
  void *context = NULL;
  if (levels && eo670_GetAddressInfoProlog(&context)) {
    eo670AddressInfo addrInfo;
    while (levels--) {
      if (eo670_GetAddressInfo(*buffer, &addrInfo, context)) {
        cb->omsStackTrace(*buffer, addrInfo.symbol, 
          addrInfo.demangledSymbol,
          addrInfo.offsetToSymbol, addrInfo.moduleName,
          addrInfo.sourceFileName, addrInfo.lineNumber);
        eo670_FreeAddressInfo(&addrInfo);
      } else {
        cb->omsStackTrace(*buffer, "unknown", "unknown", 0,
          "unknown", "", -1);
      }
      ++buffer;
    }
    eo670_GetAddressInfoEpilog(context);
  }
}

/*-----------------------------------------------------*/

SAPDB_UInt8 LVC_KernelInterfaceInstance::GetMicroTime()
{
  return RTESys_MicroSecTimer();
}

/*-----------------------------------------------------*/

bool LVC_KernelInterfaceInstance::StatFile(
    const char  *fn,
    SAPDB_UInt8 &ctime,
    SAPDB_UInt8 &fsize)
{
#ifdef WIN32
  BY_HANDLE_FILE_INFORMATION info;
  HANDLE hFile = CreateFile(fn, GENERIC_READ, 0, NULL, OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    ctime = 0;
    fsize = 0;
    return false;
  }
  if (!GetFileInformationByHandle(hFile, &info)) {
    ctime = 0;
    fsize = 0;
    return false;
  }
  CloseHandle(hFile);
  ctime = reinterpret_cast<SAPDB_UInt8&>(info.ftLastWriteTime);
  fsize = info.nFileSizeLow | ((SAPDB_UInt8) info.nFileSizeHigh << 32);
#else
  struct stat sb;
  if (stat(fn, &sb) == 0) {
    ctime = sb.st_mtime;
    fsize = sb.st_size;
  } else {
    ctime = 0;
    fsize = 0;
    return false;
  }
#endif
  return true;
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::FormatTime(
    char        *buffer,
    size_t      bufsize,
    SAPDB_UInt8 time)
{
  static const char* wDays[7] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
  static const char* mNames[12] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

#ifdef WIN32
  SYSTEMTIME sysTime;
  FileTimeToSystemTime(reinterpret_cast<const FILETIME*>(&time),
    &sysTime);
  sp77sprintf(buffer, bufsize, "%s %s %02d %02d:%02d:%02d %04d\n",
    wDays[sysTime.wDayOfWeek], mNames[sysTime.wMonth - 1],
    sysTime.wDay, sysTime.wHour, sysTime.wMinute,
    sysTime.wSecond, sysTime.wYear);
#else
  static const int DAY_SEC = 24*60*60;
  static const int YEAR_SEC = 365*DAY_SEC;
  static const int YEAR4_SEC = 4*YEAR_SEC+DAY_SEC;
  static const int mdays[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  static const int lmdays[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  const int *days = mdays;
  int yr, yd, mo, da, hr, mi, se, wd;
  wd = ((time / DAY_SEC) + 4) % 7;
  yr = (time / YEAR4_SEC) * 4 + 1970;
  time %= YEAR4_SEC;
  if (time >= YEAR_SEC) {
    ++yr;
    time -= YEAR_SEC;
    if (time >= YEAR_SEC) {
      ++yr;
      time -= YEAR_SEC;
      if (time >= YEAR_SEC + DAY_SEC) {
        ++yr;
        time -= YEAR_SEC + DAY_SEC;
      } else {
        days = lmdays;
      }
    }
  }
  yd = time / DAY_SEC;
  time %= DAY_SEC;
  for (int i = 0; i < 12; ++i) {
    if (yd < days[i]) {
      da = yd;
      mo = i;
      break;
    }
    yd -= days[i];
  }
  hr = time / 3600;
  time %= 3600;
  mi = time / 60;
  se = time % 60;
  sp77sprintf(buffer, bufsize, "%s %s %02d %02d:%02d:%02d %04d\n",
    wDays[wd], mNames[mo], da + 1, hr, mi, se, yr);
#endif
}

/*-----------------------------------------------------*/

short LVC_KernelInterfaceInstance::LockRequest(LVC_LockRequest &lockReq)
{
  RTESync_IRWRegion *lock;
  if (lockReq.m_action <= LVC_LockRequest::RWLOCK_ACTION_MAX) {
    // R/W lock action, check ID and get proper lock
    if(lockReq.m_lockAddress != NULL){
      lock = reinterpret_cast<RTESync_IRWRegion *> (lockReq.m_lockAddress);
    }
    else {
      if (lockReq.m_lockId <= 0) {
        return e_OMS_invalid_parameter;
      }
      bool isCreate = lockReq.m_action == LVC_LockRequest::RWLOCK_CREATE;
      lock = m_rw_locks.getLock(lockReq.m_areaId, lockReq.m_lockId, isCreate);
      if (lock == NULL) {
        if (isCreate) {
          return e_OMS_no_more_memory;
        } else {
          return e_OMS_invalid_parameter;
        }
      }
      lockReq.m_lockAddress = lock;
    }
  } else {
    return e_OMS_invalid_command;
  }

  switch (lockReq.m_action) {
  case LVC_LockRequest::RWLOCK_CREATE:
    return 0;
  case LVC_LockRequest::RWLOCK_LOCK_EXCLUSIVE:
    lock->enter(true,lockReq.m_taskId);
    return 0;
  case LVC_LockRequest::RWLOCK_LOCK_SHARED:
    lock->enter(false,lockReq.m_taskId);
    return 0;
  case LVC_LockRequest::RWLOCK_TRYLOCK_EXCLUSIVE:
    if (lock->tryEnter(true,lockReq.m_taskId))
      return 0;
    else
      return e_OMS_request_timeout;
  case LVC_LockRequest::RWLOCK_TRYLOCK_SHARED:
    if (lock->tryEnter(false,lockReq.m_taskId))
      return 0;
    else
      return e_OMS_request_timeout;
  case LVC_LockRequest::RWLOCK_UNLOCK_EXCLUSIVE:
    lock->leave(true,lockReq.m_taskId);
    return 0;
  case LVC_LockRequest::RWLOCK_UNLOCK_SHARED:
    lock->leave(false,lockReq.m_taskId);
    return 0;
  case LVC_LockRequest::RWLOCK_IS_LOCKED:  // ONLY FOR DEBUGGING
  {
    short pid = lock->getLastEntered();
    if (pid == -1)
      return -1;
    else if (pid == lockReq.m_taskId)
      return 0;
    else
      return pid;
  }
  default:
    return e_OMS_invalid_command;
  }
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::RegisterArea(SAPDB_Int4 areaId, const char *name){
  m_rw_locks.registerArea(areaId, name);
}

/*-----------------------------------------------------*/

bool LVC_KernelInterfaceInstance::UseReaderWriterLocks(){
  //return g01cnstview_rw_sync() ? true : false;
  return true;
}

/*-----------------------------------------------------*/

RTE_IInterface &LVC_KernelInterfaceInstance::GetKernelRunTimeInterface(){
    return RTE_IInterface::Instance();
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::SetErrorCallback(DbpErrorCallbackInterface *cb){
  // Set the static ErrorCallbackInterface pointer in the kernel. PTS 1133630
  DbpError::dbpSetCallback(cb);
}

/*-----------------------------------------------------*/

void LVC_KernelInterfaceInstance::VTrace(const char* msg)
{
    Kernel_VTrace() << msg;
}