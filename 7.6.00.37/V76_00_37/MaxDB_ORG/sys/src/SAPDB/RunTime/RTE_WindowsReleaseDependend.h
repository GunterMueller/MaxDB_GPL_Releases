/*!---------------------------------------------------------------------
  @file           RTE_WindowsReleaseDependend.h
  @author         RobinW
  @brief          wrapper for windows API functions that are not available in all windows releases
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

/*!
    Example:

    BOOL AttachConsole(DWORD pid) is a windows API function that is not available in all windows releases.

    For this function, a wrapper function  RTE_WindowsReleaseDependendAttachConsole() has been created. As parameters, 
    this function gets all the parameters AttachConsole() gets and additionaly a BOOL pointer. If AttachConsole() is
    present in the currently running windows, AttachConsole() will be called and the variable the BOOL pointer
    points to will receive the return value of AttachConsole(). The wrapper function returns true if AttachConsole()
    could be called and false if not. So if AttachConsole() was to be used like this (in Windows 2003, were it is 
    present):

    if(AttachConsole(0))
    {
        printf("AttachConsole successful!");
    }
    else
    {
        printf("AttachConsole unsuccessful!");
    }


    the following code could be used on all windows releases:

    BOOL success;
    if(RTE_WindowsReleaseDependendAttachConsole(0,&success))
    {
        if(success)
        {
            printf("AttachConsole successful!");
        }
        else
        {
            printf("AttachConsole unsuccessful!");
        }
    }
    else
    {
        // alternative code instead of AttachConsole()
    }

    Currently this module only contains wrappers for AttachConsole() and GetSystemTimes(). if other release dependend
    functions may become relevant, it should be extended by wrapper functions following the pattern described above.
    
*/

#include "SAPDBCommon/SAPDB_Types.h"
#include "SAPDBCommon/SAPDB_C_Cplusplus.h"

#include <windows.h> /* nocheck */

/*!
  @brief                Call the the windows API function GetSystemTimes() if available in the current windows release
  @param                t1          [out]   passed to GetSystemTimes() (if available) 
  @param                t2          [out]   passed to GetSystemTimes() (if available) 
  @param                t3          [out]   passed to GetSystemTimes() (if available) 
  @param                returnValue [out]   the return value of GetSystemTimes() (if available)
  @return value         true if GetSystemTimes() is available, false if not
 */
externC SAPDB_Bool RTE_WindowsReleaseDependendGetSystemTimes(FILETIME *t1,FILETIME *t2,FILETIME *t3,BOOL *returnValue);

/*!
  @brief                Call the the windows API function AttachConsole() if available in the current windows release
  @param                pid         [in]    passed to AttachConsole() (if available)
  @param                returnValue [out]   the return value of the AttachConsole() call (if available)
  @return value         true if AttachConsole() is available, false if not
 */
externC SAPDB_Bool RTE_WindowsReleaseDependendAttachConsole(DWORD pid,BOOL *returnValue);

