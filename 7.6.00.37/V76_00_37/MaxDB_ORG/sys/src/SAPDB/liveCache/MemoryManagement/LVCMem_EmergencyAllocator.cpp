/****************************************************************************

  module      : LVCMem_EmergencyAllocator.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: liveCache memory management 
  description : Block allocator for LVCMem_EmergencyAllocator

                This allocator is based on RteMem_BlockAllocator.

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


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "liveCache/MemoryManagement/LVCMem_EmergencyAllocator.hpp"
#include "liveCache/MemoryManagement/LVCMem_BlockAllocator.hpp"

#include "hgg08.h"
#include "heo51.h"  // vgetpid
#include "heo55k.h" // vbegexcl, vendexcl

LVCMem_EmergencyAllocator* LVCMem_EmergencyAllocator::m_Instance = 0;

/*-----------------------------------------------------*/

LVCMem_EmergencyAllocator::LVCMem_EmergencyAllocator()
: SAPDBMem_RawAllocator(
       (const SAPDB_UTF8 *) "LVCMem_EmergencyAllocator",
       LVCMem_BlockAllocator::Instance(), 
       REINTERPRET_CAST(RTESync_NamedSpinlock*, 0),
       10 * 1024 * 1024,
       0,
       SAPDBMem_RawAllocator::NOT_FREE_RAW_EXTENDS)
, m_Owner(0)
{
  void* p = SAPDBMem_RawAllocator::Allocate(1024 * 1024);
  this->Deallocate(p);
}

/*-----------------------------------------------------*/

void* LVCMem_EmergencyAllocator::Allocate(SAPDB_ULong count)
{
#ifdef SAPDBMem_USE_SYSTEM_ALLOC
  return malloc(count);
#else
  void*  p = 0;
  TaskId currentTask;
  vgetpid(currentTask);
  if (currentTask != m_Owner)
  {
    vbegexcl(currentTask, g08LVCEmergency);    
    m_Owner = currentTask;
  }
  else
  {
    p = SAPDBMem_RawAllocator::Allocate(count);
  }
  return p;
#endif
}

/*-----------------------------------------------------*/

void LVCMem_EmergencyAllocator::GetEmergencyAllocatorInfo
(
 SAPDB_ULong &BytesUsed,
 SAPDB_ULong &BytesControlled,
 SAPDB_ULong &MaxBytesUsed
 ) const
{
    this->CalcStatisticsConst(BytesUsed, MaxBytesUsed, BytesControlled); 
}

/*-----------------------------------------------------*/

LVCMem_EmergencyAllocator& LVCMem_EmergencyAllocator::Instance()
{
   SAPDBMEM_STATIC_RAW_ALLOCATE(Space, sizeof(LVCMem_EmergencyAllocator));
   if (0 == m_Instance)
   {
     m_Instance = new(Space) LVCMem_EmergencyAllocator();
   }
   return *m_Instance;
}

/*-----------------------------------------------------*/

void LVCMem_EmergencyAllocator::Release(TaskId taskId)
{
  if (m_Owner == taskId)
  {
    m_Owner = 0;
    vendexcl(taskId, g08LVCEmergency);    
  }
}

/*-----------------------------------------------------*/

bool LVCMem_EmergencyAllocator::Reserve(TaskId taskId)
{
  if (m_Owner != taskId)
  {
    vbegexcl(taskId, g08LVCEmergency);
    m_Owner = taskId;
    return true;
  }
  return false;
}
