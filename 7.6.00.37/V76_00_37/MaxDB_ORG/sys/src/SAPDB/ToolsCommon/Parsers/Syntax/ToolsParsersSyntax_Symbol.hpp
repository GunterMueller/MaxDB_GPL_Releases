/*!
    \file    ToolsParsersSyntax_Symbol.hpp
    \author  TiloH
    \ingroup syntax for parser
    \brief   declaration of a class for handling symbols of a syntax string

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

#if !defined(TOOLSPARSERSSYNTFAC_SYMBOL_HPP)
#define TOOLSPARSERSSYNTFAC_SYMBOL_HPP

class ToolsParsersSyntEl_ParameterBase;

/*! \brief a class managing symbols of a syntax string
    
    This class is used by ToolsParsersSyntax_Syntax, otherwise there should be
    no need or reason to use this class */
class ToolsParsersSyntax_Symbol
{
  public:
    /*! \brief a constructor
    
        The symbol is initialized to the non terminal character 0 */
    ToolsParsersSyntax_Symbol();

    /*! \brief a constructor

        The symbol is representing a terminal object (e.g. variable or
        keyword).
        
        \param terminalObject [IN] reference to the terminal object */
    ToolsParsersSyntax_Symbol(ToolsParsersSyntEl_ParameterBase * terminalObject);

    /*! \brief a constructor
    
        The symbol is initialized to a non terminal character (e.g. '(', '[',
        ...).

        \param nonTerminal [IN] the non terminal */
    ToolsParsersSyntax_Symbol(char nonTerminal);

    /*! \brief check equality to non terminal
    
        \param c [IN] the non terminal to compare to
        \returns true if the symbol represents the non terminal character c,
            false otherwise */
    bool equals(char c) const;

    /*! \brief check for mebership in a set
        \param c1 [IN] first character in the set
        \param c2 [IN] second character in the set
        \param c3 [IN] third character in the set
        \return true if object equals c1, c2 or c3, false otherwise */
    bool isMemberOf(char c1, char c2, char c3) const;

    /*! \brief check for symbol type
        \return true if symbol is terminal, false otherwise */
    bool isTerminal() const;

    /*! \brief supply terminal object
        \return pointer to terminal object, if symbol represents a terminal
            object, 0 otherwise */
    ToolsParsersSyntEl_ParameterBase * getTerminalObject();

    /*! \brief supply non terminal object
        \return non terminal character, if symbol represents a non terminal
            object, '\0' otherwise */
    char getNonTerminal() const;

  private:
    ToolsParsersSyntEl_ParameterBase * m_terminalObject;
    char                               m_nonTerminal;
};

#endif
