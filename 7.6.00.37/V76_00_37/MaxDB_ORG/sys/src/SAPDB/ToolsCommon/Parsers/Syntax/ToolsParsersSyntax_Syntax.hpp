/*!
    \file    ToolsParsersSyntax_Syntax.hpp
    \author  TiloH
    \ingroup syntax for parser
    \brief   declaration of a class for creating a syntax tree
             from a syntax string

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

#if !defined(TOOLSPARSERSSYNTAX_SYNTAX_HPP)
#define TOOLSPARSERSSYNTAX_SYNTAX_HPP

#include "ToolsCommon/Parsers/ToolsParsers_ParseResult.hpp"
#include "ToolsCommon/Parsers/Syntax/ToolsParsersSyntax_Symbol.hpp"
#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Invalid.hpp"


class ToolsParsersSyntEl_Terminal;
class ToolsParsersSyntEl_ParameterBase;

class ToolsParsersUtil_String;

/*! \brief a class converting a syntax string into a tree of parameter objects,
    that can be used with the argument parser
    ToolsParsers_ProgramParameterParser
    
    \see ToolsParsers_ProgramParameterParser*/
class ToolsParsersSyntax_Syntax
{
  public:
    /*! \brief a constructor */
    ToolsParsersSyntax_Syntax(const char * syntaxString);

    /*! \brief the destructor */
    ~ToolsParsersSyntax_Syntax();

    /*! \brief returns syntax as tree of syntax objects
        \return syntax tree */
    ToolsParsersSyntEl_ParameterBase * getSyntax(
        );
    /*! \brief returns syntax as tree of syntax objects
        \return syntax tree */
    const ToolsParsersSyntEl_ParameterBase * getSyntax(
        ) const;

    /*! \brief returns parse result
        \see ToolsParsers_ParseResult
        \return parse result */
    ToolsParsers_ParseResult & getResult();

    /*! \brief prints syntax into a string

        \param style     [IN] requested format of the syntax (debug, simple, etc.)
        \param outText   [IN/OUT] buffer where the syntax is printed to
        \param spaceLeft [IN] space left in outText including the space for the terminating zero
        \return true if complete syntax fitted into buffer outText, false otherwise */
    bool printSyntax(
        ToolsParsersSyntEl_ParameterBase::PrintSyntaxStyle   style,
        char                                               * outText,
        size_t                                               spaceLeft) const;

  private:
    ToolsParsersSyntEl_ParameterBase * m_SyntaxTree;
    ToolsParsersSyntEl_Invalid         m_InvalidDummy;

    ToolsParsers_ParseResult m_Result;
    Tools_Vector<ToolsParsersSyntEl_Terminal *> m_terminalObjects;

    Tools_Vector<ToolsParsersSyntax_Symbol> m_symbolString;

    /*! \brief differentiates between non terminal and terminal symbols,
        introduces objects for terminal symbols (keywords and variables),
        removes spaces and stores the provisional result within m_symbolString */
    bool generateSymbolString(const char * syntaxString);

    bool setParameterGroupTo(const char * syntaxString);

    bool generateSyntaxTreeFor(
        size_t                              symbolStringStart,
        size_t                              symbolStringEnd,
        ToolsParsersSyntEl_ParameterBase *& tree);

    ToolsParsersSyntEl_ParameterBase * getTerminal(
        const char * startOfTerminalName,
        const char * endOfTerminalName);

    ToolsParsersSyntEl_ParameterBase * createTerminal(
        const char * startOfTerminalName,
        const char * endOfTerminalName,
        const char * endOfTerminal);

    size_t findEndOfSymbol(
        size_t position,
        size_t end) const;

    void findEndOfVariable(
        const char  * position,
        const char *& endName,
        const char *& endVariable) const;

    void findEndOfKeyword(
        const char  * position,
        const char *& endName,
        const char *& endKeyword) const;

    bool startsParenthesis(char c) const;
    bool endsParenthesis(char c) const;

    bool startsParenthesis(const ToolsParsersSyntax_Symbol & s) const;

    size_t getEndOfParenthesis(
        size_t startString,
        size_t endString) const;

    void   getEndAndContentOfParenthesis(
        size_t   startString,
        size_t   endString,
        size_t & endParenthesis,
        size_t & startContent,
        size_t & endContent) const;

    size_t findAlternativeEnd(
        size_t startString,
        size_t endString);
};

#endif
