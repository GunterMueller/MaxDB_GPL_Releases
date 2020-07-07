/*!---------------------------------------------------------------------
  @file           RTESys_NTLoadKernelDLL.cpp
  @author         StefanP
  @brief          System: Class for loading functions of the KERNEL32.DLL
              (methods)
  @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
---------------------------------------------------------------------*/


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include "RunTime/System/RTESys_NTLoadKernelDLL.hpp"
#include "RunTime/MemoryManagement/RTEMem_Allocator.hpp" 
#include "RunTime/RTE_Crash.hpp"
#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/RTE_Message.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/
RTESys_NTLoadKernel32DLL   *RTESys_NTLoadKernel32DLL::m_Instance = NULL; 

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

RTESys_NTLoadKernel32DLL::RTESys_NTLoadKernel32DLL() :  m_hDLL(NULL),
                                                        INIT_DLL_FUNC(GetVolumePathNameA),
                                                        INIT_DLL_FUNC(GetVolumeNameForVolumeMountPointA),
                                                        INIT_DLL_FUNC(GlobalMemoryStatusEx),     /* PTS 1110642 */
                                                        INIT_DLL_FUNC(MapUserPhysicalPages),     
                                                        INIT_DLL_FUNC(FreeUserPhysicalPages),
                                                        INIT_DLL_FUNC(AllocateUserPhysicalPages),
                                                        INIT_DLL_FUNC(GetNativeSystemInfo)
{
}


RTESys_NTLoadKernel32DLL::~RTESys_NTLoadKernel32DLL()
{
    m_Instance->NTFreeLibrary (m_Instance->m_hDLL, KERNEL32DLL);
}


RTESys_NTLoadKernel32DLL & RTESys_NTLoadKernel32DLL::Instance( )
{
    if (!m_Instance)
    {
        m_Instance = new (RTEMem_Allocator::Instance()) RTESys_NTLoadKernel32DLL;
        if (NULL == m_Instance)
        {
            RTE_Crash (SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_SYS_ALLOC_INST, "RTESys_NTLoadKernel32DLL"));
        }

        m_Instance->m_hDLL = m_Instance->NTLoadLibrary(KERNEL32DLL);


        if (NULL == m_Instance->m_hDLL)
        {
            return *m_Instance;
        }

        LOAD_FUNC(GetVolumePathNameA);
        LOAD_FUNC(GetVolumeNameForVolumeMountPointA);
        LOAD_FUNC(GlobalMemoryStatusEx);    /* PTS 1110642 */
        LOAD_FUNC(MapUserPhysicalPages);
        LOAD_FUNC(FreeUserPhysicalPages);    
        LOAD_FUNC(AllocateUserPhysicalPages);    
        LOAD_FUNC(GetNativeSystemInfo);
    }
 
  return *m_Instance;
}


BOOL    RTESys_NTLoadKernel32DLL::GetDLLFunctionError (SAPDB_Char * szFuncName)
{
    RTE_Message (SAPDBErr_MessageList(RTE_CONTEXT, GetLastError (), RTEINFO_SYS_CALL_DLL_FUNC, szFuncName));
    return (false);
}