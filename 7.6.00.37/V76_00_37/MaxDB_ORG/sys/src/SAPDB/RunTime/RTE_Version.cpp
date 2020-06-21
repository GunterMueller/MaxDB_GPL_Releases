/*!***************************************************************************

  module      : RTE_Version.cpp

  -------------------------------------------------------------------------

  responsible : Robin

  special area: RunTime and Kernel versions
  description : contains functions that return RunTime and Kernel versions strings

  last changed: 2004-11-16  17:52
  see also    : 

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG


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




*****************************************************************************/

#include "hsp100.h"
#include "geo002.h"
#include "hgg11.h"
#include "SAPDBCommon/SAPDB_Types.h"

/**************************************************************************************/

extern "C" SAPDB_Char *RTE_GetRTEVersion(void)
{
    static tsp00_Versionc versionc;
    static SAPDB_Bool everCalled = false;
    if(!everCalled)
    {
#ifdef _WIN32
        if ( sql02_get_platform_id() == VER_PLATFORM_WIN32_NT )
            sp100_GetVersionString(COMP_NAME_RTE_NT_SP100, s100buildnumber, versionc );
        else
            sp100_GetVersionString(COMP_NAME_RTE_WIN9X_SP100, s100buildnumber, versionc );
#else
        sp100_GetVersionString( COMP_NAME_RTE_SP100,s100buildnumber,versionc);
#endif
        everCalled = true;
    }
    return versionc.asCharp();
}

/**************************************************************************************/

extern "C" SAPDB_Char *RTE_GetKernelVersion(void)
{
    static tsp00_Versionc correctKernelVersion;
    static SAPDB_Bool everCalled = false;
    if(!everCalled)
    {
        tsp00_Version kernelVersion;
        g11kernel_version(kernelVersion);
        correctKernelVersion.p2c(kernelVersion);
        sp100_ConvertToVersionString(correctKernelVersion);
        everCalled = true;
    }
    return correctKernelVersion.asCharp();
}
