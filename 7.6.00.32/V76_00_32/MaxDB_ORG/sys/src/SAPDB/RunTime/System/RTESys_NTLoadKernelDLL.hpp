/*!---------------------------------------------------------------------
  @file           RTESys_NTLoadKernelDLL.hpp
  @author         StefanP
  @brief          System (NT): Class for loading functions of the KERNEL32.DLL (Singleton)
  @see            

\if EMIT_LICENCE

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



\endif
---------------------------------------------------------------------*/



#ifndef RTESYS_NTLOADKERNELDLL_HPP
#define RTESYS_NTLOADKERNELDLL_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include <windows.h>

#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/RTE_Message.hpp"
#include "RunTime/System/RTESys_NTLoadDLL.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define     KERNEL32DLL     "Kernel32.dll"

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/
#define     MK_TP_OBJ_VOID(_f,_p)   typedef void    (WINAPI *   T_RTESys_NT##_f) ##_p;\
                                    T_RTESys_NT##_f    m_pRTESys_NT##_f

#define     MK_TP_OBJ(_f,_p)        typedef BOOL    (WINAPI *   T_RTESys_NT##_f) ##_p;\
                                    T_RTESys_NT##_f    m_pRTESys_NT##_f

#define     INIT_DLL_FUNC(_f)       m_pRTESys_NT##_f(NULL)   

#define     CALL_FUNC_VOID(_f,_p)   ((m_pRTESys_NT##_f == NULL) ? 0 : m_pRTESys_NT##_f##_p)

#define     FUNC_VOID_EXIST(_f)     ((m_pRTESys_NT##_f == NULL) ? false : true)

#define     CALL_FUNC(_f,_p)        (m_pRTESys_NT##_f == NULL) ? false : (m_pRTESys_NT##_f##_p ? true: GetDLLFunctionError (#_f))


#define     CALL_DLLFUNC(_f,_p)     RTESys_NTLoadKernel32DLL::Instance().NT##_f##_p


#define     LOAD_FUNC(_f)           (m_Instance->m_pRTESys_NT##_f = \
                                         (T_RTESys_NT##_f) GetProcAddress (m_Instance->m_hDLL,#_f))
                                    
/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!--------------------------------------------------------------------
  @class          RTESys_NTLoadKernel32DLL
   @description    Class loading KERNEL32.DLL functions (Singleton)


  This class is introduced to load KERNEL32.DLL functions during runtime.

 --------------------------------------------------------------------*/

class RTESys_NTLoadKernel32DLL: public RTESys_NTLoadDLL
{

public:
  /*!------------------------------------------------------------------
     @brief          Instance of RTESys_NTLoadKernelDLL
     @return value   Reference to RTESys_NTLoadKernelDLL instance.


     Get reference to single instance of RTESys_NTLoadKernelDLL.

   ------------------------------------------------------------------*/

    static  RTESys_NTLoadKernel32DLL    &   Instance ();

  /*!------------------------------------------------------------------
     @brief          Default destructor


     Default destruktor.

   ------------------------------------------------------------------*/

    ~RTESys_NTLoadKernel32DLL();

  /*!------------------------------------------------------------------
     @brief          Retrieves the volume mount point at which the specified path is mounted
     @param          lpszFileName [in] Specifies the input path string. Both absolute and relative file and 
                                        directory names, such as ".", are acceptable in this path.
     @param          lpszVolumePathName [out] Pointer to a string that receives the volume mount point for the input path.
     @param          cchBufferLength [in] Specifies the length, in TCHARs, of the output buffer.
     @return value   true / false: Function succeeded / failed


     Passed a specified path, GetVolumePathName returns the path to the volume mount point. 
     In other words, it returns the root of the volume where the end point of the specified path resides.

   ------------------------------------------------------------------*/

    BOOL    NTGetVolumePathNameA    (LPCSTR  lpszFileName, 
                                     LPSTR lpszVolumePathName, 
                                     DWORD cchBufferLength)
    {
        return CALL_FUNC(GetVolumePathNameA, (lpszFileName, lpszVolumePathName, cchBufferLength));
    }

  /*!------------------------------------------------------------------
     @brief          Retrieves the unique volume name for a volume mount point or root directory
     @param          lpszVolumeMountPoint [in] Pointer to a string that contains either the path of a 
                                          volume mount point with a trailing backslash (\) or a drive letter 
                                          indicating a root directory in the form "D:\".
     @param          lpszVolumePathName [out] Pointer to a string that receives the volume name. This name is a unique volume name 
                                          of the form "\\?\Volume{GUID}\" where GUID is the GUID that identifies the volume.
     @param          cchBufferLength [in] Specifies the length, in TCHARs, of the output buffer. A reasonable size for the buffer 
                                          to accommodate the largest possible volume name is 50 characters.
     @return value   true / false: Function succeeded / failed


     The function takes a volume mount point or root directory and returns the corresponding unique volume name.

   ------------------------------------------------------------------*/

    BOOL    NTGetVolumeNameForVolumeMountPointA (LPCSTR  lpszVolumeMountPoint, 
                                                 LPSTR lpszVolumePathName, 
                                                 DWORD cchBufferLength)
    {
        return CALL_FUNC(GetVolumeNameForVolumeMountPointA, (lpszVolumeMountPoint, lpszVolumePathName, cchBufferLength));
    }

    /*!----------------------------------------------------------------
       @brief          The NTGlobalMemoryStatusEx function obtains information about the system's current usage of both  
              physical and virtual memory
       @param          lpBuffer [in/out] Pointer to a RTE_NTMEMORYSTATUS structure. NTGlobalMemoryStatusEx 
                                    stores information about current memory availability in this structure.
       @return value   true / false: Function succeeded / failed


       You can use the NTGlobalMemoryStatusEx function to determine how much memory your application can allocate without severely 
       impacting other applications. 
       The information returned by the NTGlobalMemoryStatusEx function is volatile. There is no guarantee that two sequential 
       calls to this function will return the same information. 

     ----------------------------------------------------------------*/

    BOOL    NTGlobalMemoryStatusEx (RTE_NTMEMORYSTATUSEX * lpBuffer)    /* PTS 1110642 */
    {
        return CALL_FUNC(GlobalMemoryStatusEx, (lpBuffer));
    }

    /* PTS 1111134 */

    /*!----------------------------------------------------------------
       @brief          Cf. description of 'MapUserPhysicalPages' in the MSDN library
       @return value   

     ----------------------------------------------------------------*/


    BOOL NTMapUserPhysicalPages     (PVOID       lpAddress,     // starting address of region  
                                     ULONG_PTR   NumberOfPages, // size of physical memory
                                     PULONG_PTR  UserPfnArray)  // array of page frame numbers
    {
        return CALL_FUNC(MapUserPhysicalPages, (lpAddress, NumberOfPages, UserPfnArray));
    }


    /*!----------------------------------------------------------------
       @brief          Cf. description of 'FreeUserPhysicalPages' in the MSDN library
       @return value   

     ----------------------------------------------------------------*/


    BOOL NTFreeUserPhysicalPages    (HANDLE      hProcess,      // handle to process
                                     PULONG_PTR  NumberOfPages, // pages to free
                                     PULONG_PTR  UserPfnArray)  // virtual address
    {
        return CALL_FUNC(FreeUserPhysicalPages, (hProcess, NumberOfPages, UserPfnArray));
    }


    /*!----------------------------------------------------------------
       @brief          Cf. description of 'AllocateUserPhysicalPages' in the MSDN library
       @return value   

     ----------------------------------------------------------------*/


    BOOL NTAllocateUserPhysicalPages (HANDLE      hProcess,     // handle to process
                                      PULONG_PTR  NumberOfPages,// number of pages
                                      PULONG_PTR  UserPfnArray) // address of storage
    {
        return CALL_FUNC(AllocateUserPhysicalPages, (hProcess, NumberOfPages, UserPfnArray));
    }

    /*!----------------------------------------------------------------

     ----------------------------------------------------------------*/

    BOOL    NTGetNativeSystemInfo    (LPSYSTEM_INFO pSystemInfo)
    {
        CALL_FUNC_VOID(GetNativeSystemInfo, (pSystemInfo));
        return (FUNC_VOID_EXIST(GetNativeSystemInfo));
    }


private:
  /*!------------------------------------------------------------------
     @brief          Default constructor


     The default constructor is privat to this class to avoid instantiation outside of the class.

   ------------------------------------------------------------------*/

    RTESys_NTLoadKernel32DLL();
    
  /*!------------------------------------------------------------------
     @brief          Error message function
     @param          szfuncName [in] Name of a KERNEL32.DLL function
     @return value   false


     This function gives out an error message to KNLDIAG for a KERNEL32.DLL function which is not loaded

   ------------------------------------------------------------------*/

    BOOL    GetDLLFunctionError (SAPDB_Char * szfuncName);

    /* Make typedef for function pointer (return type BOOL) and according function pointer member variable */ 
    MK_TP_OBJ(GetVolumePathNameA, (LPCSTR, LPSTR, DWORD));
    MK_TP_OBJ(GetVolumeNameForVolumeMountPointA, (LPCSTR, LPSTR, DWORD));
    MK_TP_OBJ(GlobalMemoryStatusEx, (RTE_NTMEMORYSTATUSEX *));  /* PTS 1110642 */
    MK_TP_OBJ(MapUserPhysicalPages, (PVOID, ULONG_PTR, PULONG_PTR));  /* PTS  */
    MK_TP_OBJ(FreeUserPhysicalPages, (HANDLE, PULONG_PTR, PULONG_PTR));
    MK_TP_OBJ(AllocateUserPhysicalPages, (HANDLE, PULONG_PTR, PULONG_PTR));
    MK_TP_OBJ_VOID(GetNativeSystemInfo, (LPSYSTEM_INFO));

    static  RTESys_NTLoadKernel32DLL   *m_Instance; 
            HINSTANCE                   m_hDLL;
}; 


#endif /* RTESYS_NTLOADKERNELDLL_HPP */


