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
#include "SAPDBCommon/SAPDB_Types.h"

class RTESys_MemoryInfo
{
public:
    static void update(void);

    /* All the following methods return CACHED values!!!!               */
    /* If current values are needed, update() must be called manually   */
    static SAPDB_ULong GetPhysicalInstalled(void);

    static SAPDB_ULong GetPhysicalAvailable(void);

    static SAPDB_ULong GetVirtualInstalled(void);

    static SAPDB_ULong GetVirtualAvailable(void);
private:
    static SAPDB_ULong m_PhysicalInstalled;

    static SAPDB_ULong m_PhysicalAvailable;

    static SAPDB_ULong m_VirtualInstalled;

    static SAPDB_ULong m_VirtualAvailable;
};





