/* @lastChanged: "1998-06-22  11:16"
 * @filename:    geo38.h
 * @purpose:     "DCOM - Senke"
 * @release:     7.2.0.0
 * @see:         ""
 *
 * @Copyright (c) 1998-2005 SAP AG"


\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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



\endif

 */


#ifndef GEO38_H
#define GEO38_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "liveCache/LVC_IliveCacheSink.hpp"
#include "RunTime/Synchronisation/RTESync_Spinlock.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

struct tak_all_command_glob;
struct tsp92_RteTaskDetailsInfo;
class  SAPDBMem_RawAllocator;
class  LVCMem_CheckedUserAllocator;
class  LVC_ProcServerExtender;

class CIliveCacheSink : public IliveCacheSink
{
public:
  class AllocPool;
  /*
  caution ! if sizeof(CIliveCacheSink) is changed, check that corresponding
            struct tak_kernel_sink is able to accommodate CIliveCacheSink
  */
  /* The value for OMS_SYS_REGION must match the define 
  ** OMS_INTERNAL_CRITICAL_SECTIONS
  ** in the file OMS_Defines.h
  */
  enum { OMS_SYS_REGION = 7 };  
  tak_all_command_glob     *lpSessionContext;
  void                     *lpDefaultContext;
  void                     *lpTaskContext;       /* PTS 1106969 */
  tsp92_RteTaskDetailsInfo *lpCallTaskInfo;
  void                     *lpSqlSessionContext;     /* PTS 1120794 */
  SAPDBMem_RawAllocator    *lpOmsSessionAllocator;
  SAPDBMem_RawAllocator    *lpSqlClsAllocator;
  bool                     sqlErrorFlag;                    
  bool                     CallbackIsActive;
  bool                     m_inproc;                    /* PTS 1129814 */
  bool                     m_inRegion[OMS_SYS_REGION];
  LVCMem_CheckedUserAllocator *m_pSessionUserAllocator;
  LVCMem_CheckedUserAllocator *m_pTransactionUserAllocator;
  LVCMem_CheckedUserAllocator *m_pCOMRoutineUserAllocator;
  SAPDBMem_RawAllocator    *m_pSessionRawAllocator;
  LVC_ProcServerExtender   *m_pProcServerExtender;       /* PTS 1129814 */
  static RTESync_Spinlock  m_allocatorPoolLock;
  static AllocPool         *m_allocatorPool;
  static AllocPool         *m_allocatorPoolFreelist;

  STDMETHODIMP         QueryInterface(REFIID, PVOID*);
  
  STDMETHODIMP_(ULONG) AddRef(void);
  
  STDMETHODIMP_(ULONG) Release(void);

#define LIVECACHE_SINK_ONLY_MEMBER_FUNCS
#include "liveCache/LVC_IliveCacheSink.hpp" 
  CIliveCacheSink();
  void InitAllocator();
  void Destruct(); 

  static CIliveCacheSink* GetSessionSink();
  
  void *operator new( size_t stAllocateBlock, void *addr );
#if defined(PLACEMENT_DELETE_MEO00)
  void operator delete(void* p, void *addr ) {}
#endif
  void operator delete(void* p);
};

class RTE_DBProcError
{
public:
  short error_number;
  RTE_DBProcError(short no);
};
inline RTE_DBProcError::RTE_DBProcError(short no) {error_number = no;}

/*===========================================================================*
*  LVC_DispatcherHashEntry                                                               *
*===========================================================================*/

#include "liveCache/LVC_DispatcherHashEntry.hpp"

/*===========================================================================*
*  LVC_DispatcherCOMClassHash                                                              *
*===========================================================================*/

#include "liveCache/LVC_DispatcherCOMClassHash.hpp"

#endif  /* GEO38_H */
