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
#include "RunTime/System/RTESys_MemoryInfo.hpp"
#include "RunTime/System/RTESys_SystemInfo.h"

void RTESys_MemoryInfo::update(void)
{
    RTESys_MemInfo(&m_PhysicalInstalled,&m_PhysicalAvailable,&m_VirtualInstalled,&m_VirtualAvailable);
}

SAPDB_ULong RTESys_MemoryInfo::GetPhysicalInstalled(void)
{
    // All the other values can be zero, but the size of the physical memory installed can't
    // So this is used to indicate uninitialized state
    if(0 == m_PhysicalInstalled)
    {
        update();
    }
    return m_PhysicalInstalled;
}

SAPDB_ULong RTESys_MemoryInfo::GetPhysicalAvailable(void)
{
    // All the other values can be zero, but the size of the physical memory installed can't
    // So this is used to indicate uninitialized state
    if(0 == m_PhysicalInstalled)
    {
        update();
    }
    return m_PhysicalAvailable;
}

SAPDB_ULong RTESys_MemoryInfo::GetVirtualInstalled(void)
{
    // All the other values can be zero, but the size of the physical memory installed can't
    // So this is used to indicate uninitialized state
    if(0 == m_PhysicalInstalled)
    {
        update();
    }
    return m_VirtualInstalled;
}

SAPDB_ULong RTESys_MemoryInfo::GetVirtualAvailable(void)
{
    // All the other values can be zero, but the size of the physical memory installed can't
    // So this is used to indicate uninitialized state
    if(0 == m_PhysicalInstalled)
    {
        update();
    }
    return m_VirtualAvailable;
}



SAPDB_ULong RTESys_MemoryInfo::m_PhysicalInstalled=0;

SAPDB_ULong RTESys_MemoryInfo::m_PhysicalAvailable=0;

SAPDB_ULong RTESys_MemoryInfo::m_VirtualInstalled=0;

SAPDB_ULong RTESys_MemoryInfo::m_VirtualAvailable=0;





