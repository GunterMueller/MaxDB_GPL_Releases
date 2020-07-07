/*!---------------------------------------------------------------------
  @file           RTESys_NTLoadDLL.hpp
  @author         StefanP
  @brief          System (NT) Class for loading system DLL during runtime
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



#ifndef RTESYS_NTLOADDLL_HPP
#define RTESYS_NTLOADDLL_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include <windows.h>

#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/RTE_Message.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*!--------------------------------------------------------------------
  @class          RTESys_NTLoadDLL
   @description    This class loads and releases NT dynamic system libraries (DLL's)


  This class is introduced to load system DLL's of NT such as KERNEL32.DLL during runtime.

 --------------------------------------------------------------------*/

class RTESys_NTLoadDLL
{

public:
  /*!------------------------------------------------------------------
     @brief          Load system DLL
     @param          szDLLName [in] Name of the DLL to be load
     @return value   Handle to the loaded system library (DLL)


     This function maps the specified .DLL file into the address space of the calling process.

   ------------------------------------------------------------------*/

    HINSTANCE     NTLoadLibrary (LPCTSTR  szDLLName)
    {
        HINSTANCE     hLibrary;

        hLibrary = LoadLibrary (szDLLName);		
	
        if (hLibrary == NULL)
        {
            RTE_Message (SAPDBErr_MessageList(RTE_CONTEXT, GetLastError (), RTEERR_SYS_LOAD_DLL, szDLLName));
        }

        return (hLibrary);
    }

  /*!------------------------------------------------------------------
     @brief          Releases system DLL
     @param          szDLLName [in] Name of the library to be released
     @param          hLibrary [in] Handle of the library to be released
     @return value   true (false): releasing (not) successful


     This function decrements the reference count of the loaded dynamic-link library (DLL) module. 
     When the reference count reaches zero, the module is unmapped from the address space 
     of the calling process and the handle is no longer valid. 

   ------------------------------------------------------------------*/

    BOOL    NTFreeLibrary (HINSTANCE    hLibrary, LPCTSTR   szDLLName)
    {
        BOOL    success = SAPDB_FALSE;

        if (!(success = FreeLibrary (hLibrary)))		
	    {
            RTE_Message (SAPDBErr_MessageList(RTE_CONTEXT, GetLastError(), RTEERR_SYS_FREE_DLL, szDLLName));
        }

        return (success);
    }


}; 


#endif /* RTESYS_NTLOADDLL_HPP */


