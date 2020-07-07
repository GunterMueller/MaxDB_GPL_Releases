/*!
    \file    ToolsParsersSyntEl_ParameterGroup.hpp
    \author  TiloH
    \ingroup syntax elements for parsers
    \brief   declaration of a base class for handling a groups of parameters  

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

#if !defined(TOOLSPARSERSSYNTEL_PARAMETERGROUP_HPP)
#define TOOLSPARSERSSYNTEL_PARAMETERGROUP_HPP

#include "SAPDB/ToolsCommon/Tools_List.hpp"
#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_NonTerminal.hpp"

/*! \brief a base class for groups of parameters */
class ToolsParsersSyntEl_ParameterGroup: public ToolsParsersSyntEl_NonTerminal
{
  public:
    /*! \brief destructor */
    ~ToolsParsersSyntEl_ParameterGroup();

    /*! \brief adding a new member to the group
    
        Effects are undefined, if one syntax part is added to more than one such
        group, so don't do it. Only pointers to group members are added, so
        responsibility for memory handling of the added object itself remains by
        the caller of this function.
       
        \param newGroupMember [IN] pointer to the member that should be added to
            this group
        \return true if pointer to group member could be added */
    bool push_back(ToolsParsersSyntEl_ParameterBase * newGroupMember);

  protected:
    Tools_List<ToolsParsersSyntEl_ParameterBase *> list; //!< the list storing pointers to the group members
};

#endif
