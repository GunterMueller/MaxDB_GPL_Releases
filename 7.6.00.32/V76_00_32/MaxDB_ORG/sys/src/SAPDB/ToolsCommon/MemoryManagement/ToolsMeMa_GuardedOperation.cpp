/*!
  @file    ToolsMeMa_GuardedOperation.cpp
  @author  MarcW
  @brief   guarded memory manipulation operations

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#include "ToolsCommon/MemoryManagement/ToolsMeMa_GuardedOperation.hpp"

const char* ToolsMeMa_GuardedOperation::m_Component("TOOLS");
Msg_List ToolsMeMa_GuardedOperation::m_TotalList;
Msg_List ToolsMeMa_GuardedOperation::m_CurrentList;
