/*!
    \file    ToolsParsersSyntEl_Terminal.hpp
    \author  TiloH
    \ingroup syntax elements for parsers
    \brief   declaration of a base class for elements representing arguments

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

#if !defined(TOOLSPARSERSSYNTEL_TERMINAL_HPP)
#define TOOLSPARSERSSYNTEL_TERMINAL_HPP


#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_ParameterBase.hpp"
#include "ToolsCommon/Parsers/Utilities/ToolsParsersUtil_String.hpp"


/*! \brief a base class for different program parameter types */
class ToolsParsersSyntEl_Terminal: public ToolsParsersSyntEl_ParameterBase
{
  public:
    /*! \brief constuctor
        \param name [IN] the name of the parameter within the syntax string */
    ToolsParsersSyntEl_Terminal(
        const char * name);

    /*! \brief constuctor
        \param nameStart [IN] first character of the name of the parameter within the syntax string
        \param nameEnd   [IN] first character following the name of the parameter within the syntax string */
    ToolsParsersSyntEl_Terminal(
        const char * nameStart,
        const char * nameEnd);

    /*! \brief implementation of abstract virtual function isTerminal() of base
        class ToolsParsersSyntEl_ParameterBase.
        
        \see ToolsParsersSyntEl_ParameterBase::isTerminal
        
        \return true */
    virtual bool isTerminal() const;

    /*! \brief function supplying the name of the terminal (keyword or variable)
        \return name of the terminal in the syntax string */
    const ToolsParsersUtil_String & getName() const;

  protected:
    ToolsParsersUtil_String m_Name; //!< name of the terminal in the syntax string
};

#endif
