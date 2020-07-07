/*!---------------------------------------------------------------------
  @file           RTESys_NTLoadDLLFunc.cpp
  @author         StefanP
  @brief          System (NT) Loading functions of the KERNEL32.DLL
              (C-Wrapper)
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
#define _WIN32_WINNT 0x0500
#define _KERNEL32_  /* --- disables the definition of               */
                    /* --- DECLSPEC_IMPORT in <windows.h>           */

/* --- Those defines have to be placed before including <windows.h> */
/* --- in order to insure that changes in the NT prototypes are     */
/* --- detected during compilation time                             */

#define GetVolumePathNameA                  RTESys_NTGetVolumePathName
#define GetVolumeNameForVolumeMountPointA   RTESys_NTGetVolumeNameForVolumeMountPoint
#define GlobalMemoryStatusEx                RTESys_NTGlobalMemoryStatusEx   /* PTS 1110642 */
#define MapUserPhysicalPages                RTESys_NTMapUserPhysicalPages     /* PTS         */
#define FreeUserPhysicalPages               RTESys_NTFreeUserPhysicalPages   
#define AllocateUserPhysicalPages           RTESys_NTAllocateUserPhysicalPages   
#define GetNativeSystemInfo                 RTESys_NTGetNativeSystemInfo  


#include <windows.h>
#include "RunTime/System/RTESys_NTLoadKernelDLL.hpp"
#include "RunTime/System/RTESys_NTLoadDLLFunc.h"
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

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*!--------------------------------------------------------------------
   @brief          Retrieves the volume mount point at which the specified path is mounted (C-Wrapper)
   @param          lpszFileName [in] Specifies the input path string. Both absolute and relative file and 
                                          directory names, such as ".", are acceptable in this path.
   @param          lpszVolumePathName [out] Pointer to a string that receives the volume mount point for the input path.
   @param          cchBufferLength [in] Specifies the length, in TCHARs, of the output buffer.
   @return value   true / false: Function succeeded / failed


   Passed a specified path, GetVolumePathName returns the path to the volume mount point. 
   In other words, it returns the root of the volume where the end point of the specified path resides.

 --------------------------------------------------------------------*/


externC     BOOL WINAPI RTESys_NTGetVolumePathName (
    LPCSTR  lpszFileName,
    LPSTR   lpszVolumePathName,
    DWORD   cchBufferLength
    )
{
    return CALL_DLLFUNC(GetVolumePathNameA, (lpszFileName, lpszVolumePathName, cchBufferLength));
}


/*!--------------------------------------------------------------------
   @brief          Retrieves the unique volume name for a volume mount point or root directory (C-Wrapper)
   @param          lpszVolumeMountPoint [in] Pointer to a string that contains either the path of a 
                                          volume mount point with a trailing backslash (\) or a drive letter 
                                          indicating a root directory in the form "D:\".
   @param          lpszVolumePathName [out] Pointer to a string that receives the volume name. This name is a unique volume name 
                                          of the form "\\?\Volume{GUID}\" where GUID is the GUID that identifies the volume.
   @param          cchBufferLength [in] Specifies the length, in TCHARs, of the output buffer. A reasonable size for the buffer 
                                          to accommodate the largest possible volume name is 50 characters.
   @return value   true / false: Function succeeded / failed


   The function takes a volume mount point or root directory and returns the corresponding unique volume name.

 --------------------------------------------------------------------*/


externC     BOOL WINAPI RTESys_NTGetVolumeNameForVolumeMountPoint (
    LPCSTR  lpszVolumeMountPoint,
    LPSTR   lpszVolumePathName,
    DWORD   cchBufferLength
    )
{
    return CALL_DLLFUNC(GetVolumeNameForVolumeMountPointA, (lpszVolumeMountPoint, lpszVolumePathName, cchBufferLength));
}


/*!--------------------------------------------------------------------
   @brief          The RTESys_NTGlobalMemoryStatusEx function obtains information about the system's current usage of both physical and 
              virtual memory (C-Wrapper)
   @param          lpBuffer [in/out] Pointer to a RTE_NTMEMORYSTATUS structure. RTESys_NTGlobalMemoryStatusEx 
                                  stores information about current memory availability in this structure.
   @return value   true / false: Function succeeded / failed


   You can use the RTESys_NTGlobalMemoryStatusEx function to determine how much memory your application can allocate without severely 
   impacting other applications. 
   The information returned by the RTESys_NTGlobalMemoryStatusEx function is volatile. There is no guarantee that two sequential 
   calls to this function will return the same information. 

 --------------------------------------------------------------------*/


externC     BOOL WINAPI RTESys_NTGlobalMemoryStatusEx (     /* PTS 1110642 */
    RTE_NTMEMORYSTATUSEX * lpBuffer
    )
{
    return CALL_DLLFUNC(GlobalMemoryStatusEx, (lpBuffer));
}


/*!--------------------------------------------------------------------
   @brief          Cf. description of 'MapUserPhysicalPages' in the MSDN library  (C-Wrapper)
   @return value   

 --------------------------------------------------------------------*/


externC     BOOL WINAPI RTESys_NTMapUserPhysicalPages (     /* PTS 1111134 */
    PVOID       lpAddress,          // starting address of region
    ULONG_PTR   NumberOfPages,      // size of physical memory
    PULONG_PTR  UserPfnArray        // array of page frame numbers
    )
{
    return CALL_DLLFUNC(MapUserPhysicalPages, (lpAddress, NumberOfPages, UserPfnArray));
}


/*!--------------------------------------------------------------------
   @brief          Cf. description of 'FreeUserPhysicalPages' in the MSDN library  (C-Wrapper)
   @return value   

 --------------------------------------------------------------------*/


externC     BOOL WINAPI RTESys_NTFreeUserPhysicalPages (     /* PTS 1111134 */
    HANDLE      hProcess,           // handle to process
    PULONG_PTR  NumberOfPages,      // pages to free
    PULONG_PTR  UserPfnArray        // virtual address
    )
{
    return CALL_DLLFUNC(FreeUserPhysicalPages, (hProcess, NumberOfPages, UserPfnArray));
}


/*!--------------------------------------------------------------------
   @brief          Cf. description of 'AllocateUserPhysicalPages' in the MSDN library  (C-Wrapper)
   @return value   

 --------------------------------------------------------------------*/


externC     BOOL WINAPI RTESys_NTAllocateUserPhysicalPages (     /* PTS 1111134 */
    HANDLE      hProcess,           // handle to process
    PULONG_PTR  NumberOfPages,      // number of pages
    PULONG_PTR  UserPfnArray        // address of storage
    )
{
    return CALL_DLLFUNC(AllocateUserPhysicalPages, (hProcess, NumberOfPages, UserPfnArray));
}

/*!--------------------------------------------------------------------
    @brief          Cf. description of 'GetNativeSystemInfo' in the MSDN library  (C-Wrapper)
 --------------------------------------------------------------------*/


externC     BOOL WINAPI RTESys_NTGetNativeSystemInfo (     /* PTS 1110642 */
    LPSYSTEM_INFO   pSystemInfo
    )
{
    return CALL_DLLFUNC(GetNativeSystemInfo, (pSystemInfo));
}