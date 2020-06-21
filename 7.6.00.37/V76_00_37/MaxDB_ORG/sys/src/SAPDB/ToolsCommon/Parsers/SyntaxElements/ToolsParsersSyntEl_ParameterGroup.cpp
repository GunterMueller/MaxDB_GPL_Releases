/*!
    \file    ToolsParsersSyntEl_ParameterGroup.cpp
    \author  TiloH
    \ingroup syntax elements for parsers
    \brief   implementation of a base class for handling a group of parameters 

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

*/

#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_ParameterGroup.hpp"

ToolsParsersSyntEl_ParameterGroup::~ToolsParsersSyntEl_ParameterGroup()
{
    Tools_List<ToolsParsersSyntEl_ParameterBase *>::iterator i=list.begin();

    for(;i!=list.end(); i++)
    {
        if(0!=(*i) && !(*i)->isTerminal())
        {
            delete (*i);
        }
    }
}

bool ToolsParsersSyntEl_ParameterGroup::push_back(ToolsParsersSyntEl_ParameterBase * newGroupMember)
{
    if(!list.push_back(newGroupMember) || !newGroupMember->isValid())
        valid=false;

    return valid;
}
