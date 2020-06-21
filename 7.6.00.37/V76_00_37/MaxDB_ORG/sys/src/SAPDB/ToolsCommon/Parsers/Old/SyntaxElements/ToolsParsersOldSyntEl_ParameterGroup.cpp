/*!
    \file    ToolsParsersOldSyntEl_ParameterGroup.cpp
    \author  TiloH
    \ingroup backup tool simulators for tests
    \brief   implementation of a class for handling a list of alternative parameters 

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1998-2006 SAP AG

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
*/

#include "ToolsCommon/Parsers/Old/SyntaxElements/ToolsParsersOldSyntEl_ParameterGroup.hpp"

/*void ToolsParsersOldSyntEl_ParameterGroup::unsetFoundFlag()
{
    found=false;

    for(Tools_List<ToolsParsersOldSyntEl_ParameterBase *>::iterator i=list.begin(); i!=list.end(); i++)
        (*i)->unsetFoundFlag();
}*/

bool ToolsParsersOldSyntEl_ParameterGroup::push_back(ToolsParsersOldSyntEl_ParameterBase * newGroupMember)
{
    if(!list.push_back(newGroupMember) || !newGroupMember->isValid())
        valid=false;

    return valid;
}
