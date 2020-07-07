/*!
 * \file    OMS_Globals.hpp
 * \author  MarkusSi, PeterG, Roterring
 * \brief   Global methods and variables.
 */
/*

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

*/

#ifndef __OMS_GLOBALS_HPP
#define __OMS_GLOBALS_HPP

#include "Oms/OMS_GlobalAllocatorWrapper.hpp"
#include "Oms/OMS_Oid.hpp"
#include "Oms/OMS_LibOmsInterfaceInstance.hpp"
#include "liveCache/LVC_KernelInterface.hpp"
#include "ggg250.h"
#include "Oms/OMS_VersionDictionary.hpp"
#include "Oms/OMS_ContainerHandle.hpp"    // FF 2003-09-23 new FileDir
#include "Oms/OMS_GlobalUserAllocatorDirectory.hpp"
#include "Oms/OMS_GlobalAnchorDirectory.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_NewDestroy.hpp"
#include "Oms/OMS_DynamicStack.hpp"

// PTS 1117690
// This size must equals half the size of the array in the procedure calls  
// in the AK layer and the definition of the array 'tak_oms_version_desc'  
// in vak001. The desciption is always stored in UCS2-format (2 bytes per 
// character, while the array size is given in bytes.
#define OMS_MAX_VERSION_DESC_LENGTH 256


/*!
**  Class is needed to buffer the information of entries in the version directory.
**  PTS 1129082
*/
class VersionEntry {
public:
  OmsVersionId       versionId;
  tsp00_Date         createDate;
  tsp00_Time         createTime;
  tsp00_Date         openDate;
  tsp00_Time         openTime;
  tgg91_TransNo      consistentView;
  tsp00_Bool         isMarked;
  tsp00_Bool         isOpen;
  tsp00_Bool         isUnloaded;
  tsp00_8ByteCounter heapUsage;
  tsp00_Int4         hashSize;
  tgg00_FileId       versionTree;
  tgg00_FileId       versionInvTree;
  tsp00_Bool         hasVersionDesc;
  OmsTypeWyde        versionDesc[OMS_MAX_VERSION_DESC_LENGTH];
};



#define OMS_DATA_ALIGNMENT  sizeof(void*)

class DbpError;
class OMS_DumpInterface;
class OMS_TraceInterface;
class OMS_VersionDictionary;
class OMS_Context;
class OMS_MonitorDirectory;

class OMS_ResetSink {
public :
  OMS_ResetSink() {}
  inline ~OMS_ResetSink();
};

class OMS_ActionOnError {
public:
  tsp00_Int4  errorNo;
  bool        dump;
  bool        stop;
  bool        flush;
};

typedef IliveCacheSink* (*OMS_GetSinkPtrFunc)(void);

class OMS_Globals {
private:
  OMS_Globals();
public:
  static void InitSingletons();

  void Dump(OMS_DumpInterface& dumpObj);
  bool ActionOnError(tsp00_Int4 errorNo, bool &dump, bool &stop, bool &flush);
  void DumpVersionDirectory(OMS_DumpInterface& dumpObj);
  bool InSimulator() const;
  bool InProcServer() const;
  bool IsOmsTestLib() const { return m_isOmsTestLib;}
  void SetDumpError(tsp00_Int4, OMS_DiagnoseAction);
  void SetHost(bool isKernel, bool isProcServer = false);
  void SetOmsVersionThreshold(tsp00_Uint4 threshold); /* PTS 1110149 */
  tsp00_Uint4 GetOmsVersionThreshold() const          /* PTS 1110149 */
  {
    return m_omsVersionThreshold;
  }
  OMS_TraceInterface* GetTracer();
  void SetTracer(OMS_TraceInterface* pTraceObj);
  void ResetInfo();
  int GetLockIdForVersionDirectory (const OmsVersionId &versionId) const; // PTS 1124170
  bool GetLockMethodForVersionDictionary() const;                         // PTS 1124170
  static IliveCacheSink* GetCurrentLcSink();
  //static void Throw(const DbpError& e);
  static void Throw(short errorNo, const char* msg, const char* pFile, unsigned int line, OMS_Session* pSession = NULL);
  static void Throw(short errorNo, const char* msg, const OMS_ObjectId8& oid, const char* pFile, unsigned int line, OMS_Session* pSession = NULL);
  static void MakeKnlIdentifier (const char* Source, tsp00_KnlIdentifier& Dest);
  static inline size_t AlignSize(size_t val)
  {
    return ((val + (OMS_DATA_ALIGNMENT - 1)) & ~(OMS_DATA_ALIGNMENT - 1)); 
  }
  static OMS_ExceptionClass MapErrorCodeToExceptionClass(short errorNo);
  // Return the default RawChunkSize byte
  SAPDB_ULong GetDefaultRawChunkSize() const {return m_defaultRawChunkSize;};
  /*----------------------------------------------------------------------*/


  //
  // caution : order of member variables is important !!
  //

  // allocator wrapper, used for all memory allocation of the liboms
  OMS_GlobalAllocatorWrapper &m_omsAllocatorWrapper;
  OMS_GlobalAllocatorWrapper &m_userAllocatorWrapper;
  OMS_GlobalAllocatorWrapperThrowsNothing &m_SQLDBCAllocatorWrapper;

  OMS_VersionDictionary      &m_versionDictionary;
  OMS_MonitorDirectory       &m_monitorDirectory;
  // global version iterator, used for 'select .. from sysdd.oms_versions
  OMS_VersionDictionary::Iter m_versionIter;
  /*! The versionBuffer is used while iteration over the version directory. 
  ** All information belonging to versions which are synchronized via
  ** the same region are stored in the buffer. After having stored the 
  ** information the region can be freed and so contention on the locks
  ** should be prevented. If the iterator has read all information from the
  ** buffer, the versions which belong to the next region are filled in the
  ** buffer, ...
  ** \since PTS 1129082
  */
  OMS_DynamicStack<VersionEntry,OMS_GlobalAllocatorWrapper>  m_versionBuffer;

  OMS_GlobalUserAllocatorDirectory &m_globalUserAllocatorDirectory;

  OMS_GlobalAnchorDirectory &m_globalAnchorDirectory;

private:
  enum { MAX_DUMP_ERRORS = 64 };
  enum LibHost
  {
    HostUnknown,
    HostKernel,
    HostSimulator,
    HostProcServer
  };
  bool               m_isOmsTestLib;
  LibHost            m_host;
  int                m_cntDumpErrors;
  int                m_heapThreshold;       /* PTS 1110149 */
  tsp00_Uint4        m_omsVersionThreshold; /* PTS 1110149 */
  OMS_TraceInterface *m_tracer;
  OMS_ActionOnError  m_actionOnError[MAX_DUMP_ERRORS];
  // Default RawChunkSize in byte for all liboms memory allocators
  SAPDB_ULong        m_defaultRawChunkSize;

  // m_resetSink must be last member, because the destructor ensures, that the
  // destructors of the other members don't ask for the kernel environment !
  OMS_ResetSink       m_resetSink;

public:

  static LVC_KernelInterface* KernelInterfaceInstance;
  static OMS_LibOmsInterfaceInstance *m_libOmsInterfaceInstance;
  static OMS_GetSinkPtrFunc   GetSinkAddrFunc;


  inline static size_t Min(size_t t1, size_t t2) 
  {
    if (t1 < t2) 
      return t1;
    else
      return t2;
  }

  inline static size_t Max(size_t t1, size_t t2) 
  {
    if (t1 > t2) 
      return t1;
    else
      return t2;
  }

  inline static LVC_KernelInterface* GetKernelInterface()
  {
    if (KernelInterfaceInstance == NULL)
      OMS_Globals::InitSingletons();
    return KernelInterfaceInstance;
  }

  static int WideStrlen(const OmsTypeWyde* s);

  OMS_ObjectId8  m_nilOid;
  tgg91_PageRef  m_nilSeq;

  static OMS_Globals *m_globalsInstance;
};

IliveCacheSink *OMS_GetProxySinkAddr();

inline OMS_ResetSink::~OMS_ResetSink()
{
  if (NULL != OMS_Globals::KernelInterfaceInstance)
  {
    OMS_Globals::KernelInterfaceInstance->SignalUnloadLiboms();
  }
  OMS_Globals::GetSinkAddrFunc = OMS_GetProxySinkAddr;
}

/*----------------------------------------------------------------------*/
/* PTS 1115231 */
extern bool omsIsUnicodeInstance();

/*----------------------------------------------------------------------*/
/* PTS 1135287 */
extern bool omsIsOneDBSystem();

/*----------------------------------------------------------------------*/

extern void omsSimConsoleMessage(const char*);

/*----------------------------------------------------------------------*/

extern void omsGetLiveCacheVersion(char *pBuf, int bufSize);

/*----------------------------------------------------------------------*/

extern unsigned int omsGetLCVersion();


/*----------------------------------------------------------------------*/
/* Compatibility stuff for SQL class, it will go away:                  */
/*----------------------------------------------------------------------*/

inline static LVC_KernelInterface* co10_GetKernelInterface()
{
  if (OMS_Globals::KernelInterfaceInstance == NULL) OMS_Globals::InitSingletons();
  return OMS_Globals::KernelInterfaceInstance;
}

/*----------------------------------------------------------------------*/

inline static IliveCacheSink* co10_GetLcSink()
{
  return OMS_Globals::KernelInterfaceInstance->GetSinkPtr();
}

//=================================================================

class CMyAbort{
  int a;

public:
  CMyAbort(): a(0) {}
  inline void Abort();
};

inline void CMyAbort::Abort()
{
  CMyAbort* p = NULL;
  int x = p->a;             // Deref Null-Pointer
  p->a = x + 1;;            // Writing Null-Pointer
  OmsTypeUInt8 pp = 1;
  p = reinterpret_cast<CMyAbort*> (pp);
  x = p->a;                 // Deref not aligned pointer
  p->a = x + 1;             // writing not aligned pointer
  x = 0;
  pp = 1 / x;               // Division by zero
  throw -1;
}


#endif  // __OMS_GLOBALS_HPP
