/*!***************************************************************************

  module      : LVCMem_Wrapper.hpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: Wrapper 
  description : Wrapper for PASCAL/C world to C++ world. Used to encapsulate 
                old method calls. 

  last changed: 2000-12-06  19:04
  see also    : example.html ...
  first created:2000-03-09  18:21

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


#ifndef LVCMEM_WRAPPER_HPP
#define LVCMEM_WRAPPER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "heo38.h"
#include "geo38.h"
#include "heo51.h"
#include "heo58.h"
#include "geo573.h"
#include "hak260.h"


/*! 
  Class: LVCMem_Wrapper

  Description: Wrapper class for encapsulation of pascal types and functions
 */

class LVCMem_Wrapper
{
public :
/*!----------------------------------------------------------------------
   function: BadAllocRequired
   description:  yields true, if bad alloc is required from an oms test. Has
                 meaning only in omststknl.
   arguments:   
   pContext[in] pointer to the context of the caller, must have been
                determined by a call of GetContext.
   return value: true, if bad alloc is required; otherwise false
   -----------------------------------------------------------------------*/ 

  inline static bool BadAllocRequired(void* pContext);

/*!----------------------------------------------------------------------
   function: GetContext
   description:  yields the context (acv) of the caller
   arguments:    none
   return value: pointer to the context of the caller or NULL, if the
                 context is not available
   -----------------------------------------------------------------------*/ 

  static void* GetContext()
  {
    return vGetAcvPtrFromCurrentTask();
  }

/*!----------------------------------------------------------------------
   function: GetUktId
   description:  yields the ukt identification stored in the context (acv) of the caller
   arguments:    none
   return value: ukt identification of the caller or 0, if the
                 context is null
   -----------------------------------------------------------------------*/   

  static tsp00_Int4 GetUktId(void* pContext) 
  { 
    tak_all_command_glob* pAcv = REINTERPRET_CAST(tak_all_command_glob*, pContext);
    if (0 != pAcv)
    {
      return pAcv->a_allocatorInfo.ai_uktId;
    }
    else
    {
      return 0;
    }
  }

/*!----------------------------------------------------------------------
   function: GetPid
   description:  yields the task identification of the caller
   arguments:    none
   return value: the task identification of the caller
   -----------------------------------------------------------------------*/  

  static tsp00_TaskId GetPid()
  {
    tsp00_TaskId pid;
    vgetpid(pid);
    return pid;
  }

/*!----------------------------------------------------------------------
   function: IsCurrentTaskInCppDBProc
   description:  tells whether the caller is currently in a C++ db-procedure 
   arguments:    none
   return value: true, if the caller is in a C++ db-procedure, otherwise false
   -----------------------------------------------------------------------*/
  
  static bool IsCurrentTaskInCppDBProc() 
  { 
    return ((pasbool) true == a260CurrentTaskInCppDBProc()); 
  }
  
/*!----------------------------------------------------------------------
   function: MonitorAlloc
   description:  monitors an allocate request in the current context 
   arguments:    
   pContext[in] pointer to the context of the caller, must have been
                determined by a call of GetContext.
   chunkSize[in] the size of the allocted chunk.
   isUsrRequest[in] if true, the chunk is monitored in the user area; otherwise
                    in the system area.
   return value: none
   -----------------------------------------------------------------------*/
  
  inline static void MonitorAlloc (void* pContext, SAPDB_ULong chunkSize, bool isUsrRequest);

/*!----------------------------------------------------------------------
   function: MonitorBadAlloc
   description:  monitors a bad allocat exception in the current context 
   arguments:    
   pContext[in] pointer to the context of the caller, must have been
                determined by a call of GetContext.
   return value: none
   -----------------------------------------------------------------------*/
 
   inline static void MonitorBadAlloc (void* pContext); // PTS 1126797
 
/*!----------------------------------------------------------------------
   function: MonitorDealloc
   description:  monitors an deallocate request in the current context 
   arguments:    
   chunkSize[in] the size of the deallocted chunk.
   isUsrRequest[in] if true, the chunk is monitored in the user area; otherwise
                    in the system area.
   return value: none
   -----------------------------------------------------------------------*/

  inline static void MonitorDealloc (SAPDB_ULong chunkSize, bool isUsrRequest);

private :

  inline static void MonitorDeleteChunk(tak_AllocatorMonitorInfo& info, SAPDB_ULong chunkSize);

  inline static void MonitorNewChunk(tak_AllocatorMonitorInfo& info, SAPDB_ULong chunkSize);
};

/*! endclass: LVCMem_Allocator */

inline bool LVCMem_Wrapper::BadAllocRequired(void* pContext)
{
  if (0 != pContext)
  {
    tak_all_command_glob* pAcv = REINTERPRET_CAST(tak_all_command_glob*, pContext);
    if (pAcv->a_allocatorInfo.ai_badAllocCountDown > 0)
    {
      --pAcv->a_allocatorInfo.ai_badAllocCountDown;
      if (0 == pAcv->a_allocatorInfo.ai_badAllocCountDown)
      {
        pAcv->a_allocatorInfo.ai_badAllocCountDown = -2;
        return true;
      }
    }
  }
  return false;
}

/*---------------------------------------------------------------------------------*/

inline void LVCMem_Wrapper::MonitorDeleteChunk(tak_AllocatorMonitorInfo& info, SAPDB_ULong chunkSize)
{
  if (chunkSize < info.ami_FreeMin)
  {
     info.ami_FreeMin = chunkSize;
  }
  if (chunkSize > info.ami_FreeMax)
  {
     info.ami_FreeMax = chunkSize;
  }
  info.ami_FreeSum += chunkSize;
  ++info.ami_FreeCnt;
  info.ami_Allocated -= chunkSize;
}

/*---------------------------------------------------------------------------------*/

inline void LVCMem_Wrapper::MonitorNewChunk(tak_AllocatorMonitorInfo& info, SAPDB_ULong chunkSize)
{
  if (chunkSize < info.ami_MallocMin)
  {
     info.ami_MallocMin = chunkSize;
  }
  if (chunkSize > info.ami_MallocMax)
  {
     info.ami_MallocMax = chunkSize;
  }
  info.ami_MallocSum += chunkSize;
  ++info.ami_MallocCnt;
  info.ami_Allocated += chunkSize;
}

/*---------------------------------------------------------------------------------*/

inline void LVCMem_Wrapper::MonitorAlloc (void* pContext, SAPDB_ULong chunkSize, bool isUsrRequest)
{
  if (0 != pContext)
  {
    tak_all_command_glob* pAcv = REINTERPRET_CAST(tak_all_command_glob*, pContext);
    pAcv->a_allocatorInfo.ai_HeapUsageCurrent += chunkSize;
    if( pAcv->a_allocatorInfo.ai_HeapUsageMax < pAcv->a_allocatorInfo.ai_HeapUsageCurrent)
      pAcv->a_allocatorInfo.ai_HeapUsageMax = pAcv->a_allocatorInfo.ai_HeapUsageCurrent;
    if (isUsrRequest)
    {
      LVCMem_Wrapper::MonitorNewChunk(pAcv->a_allocatorInfo.ai_alloc_usr, chunkSize);
    }
    else
    {
      LVCMem_Wrapper::MonitorNewChunk(pAcv->a_allocatorInfo.ai_alloc_sys, chunkSize);
    }
  }
}

/*---------------------------------------------------------------------------------*/

inline void LVCMem_Wrapper::MonitorBadAlloc (void* pContext)
{
    if (0 != pContext)
    {
        tak_all_command_glob* pAcv = REINTERPRET_CAST(tak_all_command_glob*, pContext);
        ++pAcv->a_allocatorInfo.ai_badAllocCount;
    }
}
 
/*---------------------------------------------------------------------------------*/

inline void LVCMem_Wrapper::MonitorDealloc (SAPDB_ULong chunkSize, bool isUsrRequest)
{
  void* pContext = GetContext();
  if (0 != pContext)
  {
    tak_all_command_glob* pAcv = REINTERPRET_CAST(tak_all_command_glob*, pContext);
    pAcv->a_allocatorInfo.ai_HeapUsageCurrent -= chunkSize;
    if (isUsrRequest)
    {
      LVCMem_Wrapper::MonitorDeleteChunk(pAcv->a_allocatorInfo.ai_alloc_usr, chunkSize);
    }
    else
    {
      LVCMem_Wrapper::MonitorDeleteChunk(pAcv->a_allocatorInfo.ai_alloc_sys, chunkSize);
    }
  }
}

#endif