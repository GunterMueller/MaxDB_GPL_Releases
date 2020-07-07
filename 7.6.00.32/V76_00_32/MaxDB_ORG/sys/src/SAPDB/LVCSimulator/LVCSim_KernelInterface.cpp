/*!***************************************************************************

  module      : LVCSim_KernelInterface.cpp

  -------------------------------------------------------------------------

  responsible : IvanS

  special area: liveCache Simulator
  description : session free kernel interface implementation for simulator

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

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "LVCSimulator/LVCSim_Internal.hpp"
#include "LVCSimulator/LVCSim_KernelInterface.hpp"
#include "LVCSimulator/LVCSim_DebugAllocator.hpp"
#include "Oms/OMS_StackTraceCallbackInterface.hpp"
#include "RunTime/System/RTESys_MicroTime.h"
#include "RunTime/RTE_KernelInterfaceInstance.hpp"

//#include "heo56.h"  // vsignal
#include <time.h>
#include <string.h>

#include "heo670.h"
#include "hsp77.h"

#ifdef WIN32
#define strcasecmp  _stricmp
#include <io.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif


/*===========================================================================*
 *  STATICS                                                                  *
 *===========================================================================*/

tsp00_Int4 LVCSim_KernelInterface::startupClock = -1;

SAPDBMem_IRawAllocator *LVCSim_KernelInterface::kernel_allocator = NULL;



void *operator new(size_t sz, double *ptr)
{
    return ptr;
}

void operator delete(void *p, double *ptr)
{
}

class LVCSim_SystemAllocator : public SAPDBMem_IRawAllocator
{
public:
	LVCSim_SystemAllocator() : m_alloccnt(0), m_dealloccnt(0)
	{
	}

	static LVCSim_SystemAllocator &Instance();

	virtual void* Allocate(SAPDB_ULong ByteCount)
	{
		m_alloccnt++;
		return malloc(ByteCount);
	}

	virtual void* Allocate(SAPDB_ULong ByteCount, const void *hint)
	{
		m_alloccnt++;
		return malloc(ByteCount);
	}

	virtual void Deallocate(void* p)
	{
		m_dealloccnt++;
		free(p);
	}

	virtual void GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc,
		SAPDB_ULong &CountDealloc) const
	{
		GetCallStatistics(CountAlloc, CountDealloc);
	}

	virtual void GetCallStatistics(SAPDB_ULong &CountAlloc,
		SAPDB_ULong &CountDealloc) const
	{
		CountAlloc = m_alloccnt;
		CountDealloc = m_dealloccnt;
	}

	SAPDB_ULong	m_alloccnt;
	SAPDB_ULong	m_dealloccnt;

private:
	static LVCSim_SystemAllocator	*m_instance;
};

LVCSim_SystemAllocator *LVCSim_SystemAllocator::m_instance = NULL;
LVCSim_KernelInterface *LVCSim_KernelInterface::m_instance = NULL;
static double           kernel_allocator_space[sizeof(LVCSim_SystemAllocator) / sizeof(double) + 1];
static double           kernel_interface_space[sizeof(LVCSim_KernelInterface) / sizeof(double) + 1];
static double           runtime_interface_space[sizeof(RTE_KernelInterfaceInstance) / sizeof(double) + 1];
static int              SIM_ALLOC = -1;

LVCSim_SystemAllocator &LVCSim_SystemAllocator::Instance()
{
	if (!m_instance) m_instance = new(kernel_allocator_space) LVCSim_SystemAllocator;
	return *m_instance;
}

LVCSim_KernelInterface *LVCSim_KernelInterface::Instance()
{
    if (!m_instance) {
        m_instance = new(&kernel_interface_space) LVCSim_KernelInterface;
        m_instance->GetKernelRunTimeInterface();
    }
    return m_instance;
}

static int getSimAlloc()
{
    if (SIM_ALLOC < 0) {
        char *env = getenv("LVCSIM_ALLOC");
        if (!env) {
            SIM_ALLOC = 2;
        } else if (strcmp(env, "2") == 0 || strcasecmp(env, "DEBUG") == 0) {
            SIM_ALLOC = 2;
        } else if (strcmp(env, "1") == 0 || strcasecmp(env, "KERNEL") == 0) {
            SIM_ALLOC = 1;
        } else if (strcmp(env, "0") == 0 || strcasecmp(env, "SYSTEM") == 0) {
            SIM_ALLOC = 0;
        } else {
            const char *buf = "Environment variable LVCSIM_ALLOC must be one of DEBUG, KERNEL or \n"
                "SYSTEM (or numerically 2, 1 or 0). Default: DEBUG\n";
#ifdef WIN32
            _write(2, buf, (unsigned int) strlen(buf));
#else
            write(2, buf, strlen(buf));
#endif
            throw;
        }

    }
    return SIM_ALLOC;
}

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


SAPDBMem_IRawAllocator *LVCSim_KernelInterface::getAllocator()
{
  if (!kernel_allocator) {
    switch (getSimAlloc()) {
        case 2:
          kernel_allocator = &LVCSim_DebugAllocator::Instance();
          break;
        case 1:
          kernel_allocator = &SAPDBMem_DefaultRawAllocator::Instance();
          break;
        case 0:
          kernel_allocator = &LVCSim_SystemAllocator::Instance();
          break;
    }
  }
  return kernel_allocator;
}

// hack, since we don't need spinlock pool in simulator
static RTESync_SpinlockPool *dummySpinlockPool = NULL;

LVCSim_KernelInterface::LVCSim_KernelInterface()
  : m_rw_locks(*getAllocator(), *dummySpinlockPool)
{
}

void LVCSim_KernelInterface::DebugAllocatorErrorTrace(const char *errMsg, const void *blk)
{
	if (getSimAlloc() == 2) {
		LVCSim_DebugAllocator::Instance().doBlockError(errMsg, blk);
	}
}

void* LVCSim_KernelInterface::Allocate(SAPDB_ULong count, bool isUserRequest)
{
	void *p  = getAllocator()->Allocate(count);
    return p;
}

void* LVCSim_KernelInterface::AllocateThrowsNothing(SAPDB_ULong count)
{
	return getAllocator()->AllocateThrowsNothing(count);
}

void LVCSim_KernelInterface::CalcAllocatorStatistics(SAPDB_ULong &BytesUsed, SAPDB_ULong &MaxBytesUsed, SAPDB_ULong &BytesControlled)
{
  getAllocator()->CalcStatistics(BytesUsed, MaxBytesUsed, BytesControlled);
}
 
tsp00_Int4 LVCSim_KernelInterface::Clock()
{
	tsp00_Int4 cur = (tsp00_Int4) time(NULL);
	if (startupClock < 0) {
		startupClock = cur;
		return 0;
	} else {
		return cur - startupClock;
	}
}

void LVCSim_KernelInterface::Crash(const char* msg)
{
}

void LVCSim_KernelInterface::Deallocate(void* p, bool isUserRequest)
{
	getAllocator()->Deallocate(p);
}

void LVCSim_KernelInterface::DeregisterAllocator(RTEMem_AllocatorInfo& allocatorInfo)
{
}

void LVCSim_KernelInterface::DumpChunk (void* p, int size)
{
}


void LVCSim_KernelInterface::GetAllocatorCallStatistics(SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc) const
{
  getAllocator()->GetCallStatistics(CountAlloc, CountDealloc);
}

const SAPDB_UTF8* LVCSim_KernelInterface::GetAllocatorIdentifier() const
{
  return getAllocator()->GetIdentifier();
}

void LVCSim_KernelInterface::GetBaseAllocatorCallStatistics(SAPDB_ULong &CountAlloc, SAPDB_ULong &CountDealloc) const
{
  getAllocator()->GetBaseAllocatorCallStatistics(CountAlloc, CountDealloc);
}

SAPDB_UInt4 LVCSim_KernelInterface::GetHeapCheckLevel()
{
  return LVCSim::m_heapCheckLevel;
}

bool LVCSim_KernelInterface::IsOneDBSystem(short &rc)
{
  rc = e_ok;
  return false;
}

void* LVCSim_KernelInterface::GetKnlVersion(void* pBuffer,int bufferLength)
{
	if (bufferLength >= 2) {
		strncpy((char*) pBuffer, "liveCache 7.4 Simulator 0.0a", bufferLength);
		((char*) pBuffer)[bufferLength - 1] = 0;
		return pBuffer;
	} else {
		return NULL;
	}
}

tsp00_Int4 LVCSim_KernelInterface::GetLockRequestTimeout()
{
	// TODO
	return 0;
}

int   LVCSim_KernelInterface::GetOmsRegionCnt()
{
	// TODO: default is 8, #of parallel tasks?
	return 1024;
}

tsp00_Int4 LVCSim_KernelInterface::GetPacketMaxUsableArea()
{
	// TODO: is this OK? Used by ABAP handling
	return 32000;
}

IliveCacheSink* LVCSim_KernelInterface::GetSinkPtr()
{
    return reinterpret_cast<IliveCacheSink*>(LVCSim::GetCurrentSink());
}

tsp00_Uint4 LVCSim_KernelInterface::GetOmsVersionThreshold()
{
	return 100;
}
  
bool  LVCSim_KernelInterface::HeapThresholdReached()
{
	// NOP - we operate always in memory
	return false;
}

void LVCSim_KernelInterface::IncOmsVersionUnloadCounter()
{
    // NOP - monitoring not possible
}

bool LVCSim_KernelInterface::IsUnicodeInstance()
{
	// TODO: report according to settings
	return false;
}

void LVCSim_KernelInterface::RegisterAllocator(RTEMem_AllocatorInfo& allocatorInfo)
{
}

void LVCSim_KernelInterface::Signal(tsp00_TaskId taskId, bool timeout)
{
	// TODO: this is only for timeouts, so we probably don't have to care...
	//vsignal(taskId, !timeout);
}

void LVCSim_KernelInterface::SignalUnloadLiboms()
{
	// this here is called when there is not enough space in memory
	// since we operate only in RAM, NOP
}

void LVCSim_KernelInterface::TestBadAlloc()
{
	// NOP
}

void LVCSim_KernelInterface::SimCtlAttachSink(OmsHandle &handle)
{
	LVCSim::Attach(handle);
}

void LVCSim_KernelInterface::ConsoleError(const char *msg) 
{
    fputs(msg, stdout);
	fflush(stdout);
}

void LVCSim_KernelInterface::ConsoleMessage(const char *msg)
{
	fputs(msg, stdout);
	fflush(stdout);
}

void* LVCSim_KernelInterface::AllocateArray(SAPDB_ULong count, bool isUserRequest)
{
    if (getSimAlloc() == 2) {
    	return reinterpret_cast<LVCSim_DebugAllocator*>(getAllocator())->AllocateArray(count);
    } else {
	    return getAllocator()->Allocate(count);
    }
}
 
void LVCSim_KernelInterface::DeallocateArray(void* p, bool isUserRequest)
{
    if (getSimAlloc() == 2) {
    	reinterpret_cast<LVCSim_DebugAllocator*>(getAllocator())->DeallocateArray(p);
    } else {
    	getAllocator()->Deallocate(p);
    }
}

void LVCSim_KernelInterface::DumpStackTrace()
{
    LVCSim_DebugAllocator::Escape(LVCSIM_DA_STACKTRACE);
}

/*-----------------------------------------------------*/

unsigned int LVCSim_KernelInterface::SaveStackTrace(
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

void LVCSim_KernelInterface::EvalStackTrace(
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

SAPDB_UInt8 LVCSim_KernelInterface::GetMicroTime()
{
  return RTESys_MicroSecTimer();
}

/*-----------------------------------------------------*/

bool LVCSim_KernelInterface::StatFile(
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

void LVCSim_KernelInterface::FormatTime(
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

short LVCSim_KernelInterface::LockRequest(LVC_LockRequest &lockReq)
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
      LVCSim::DebugAllocatorEscape(LVCSIM_DA_ENTER_NOLEAK);
      lock = m_rw_locks.getLock(lockReq.m_areaId, lockReq.m_lockId, isCreate);
      LVCSim::DebugAllocatorEscape(LVCSIM_DA_EXIT_NOLEAK);
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

void LVCSim_KernelInterface::RegisterArea(SAPDB_Int4 areaId, const char *name)
{
}

/*-----------------------------------------------------*/

bool LVCSim_KernelInterface::UseReaderWriterLocks()
{
  return true;
}

/*-----------------------------------------------------*/

RTE_IInterface & LVCSim_KernelInterface::GetKernelRunTimeInterface()
{
    if ( 0 == RTE_IInterface::it )
    {
        RTE_IInterface::it = new(&runtime_interface_space) RTE_KernelInterfaceInstance;
    }
    return *RTE_IInterface::it;
}

/*-----------------------------------------------------*/

void LVCSim_KernelInterface::SetErrorCallback(DbpErrorCallbackInterface *cb){
}

/*-----------------------------------------------------*/

void LVCSim_KernelInterface::VTrace(const char* msg)
{
}
