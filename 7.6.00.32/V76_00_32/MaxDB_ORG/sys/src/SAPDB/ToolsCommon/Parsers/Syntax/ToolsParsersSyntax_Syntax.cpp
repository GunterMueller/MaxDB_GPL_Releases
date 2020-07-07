/*!
    \file    ToolsParsersSyntax_Syntax.cpp
    \author  TiloH
    \ingroup syntax for parser
    \brief   declaration of a class for creating a syntax tree from a syntax
             string

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

#include <stdio.h>

#include "ToolsCommon/Parsers/Syntax/ToolsParsersSyntax_Syntax.hpp"
#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Headers.hpp"
#include "ToolsCommon/Parsers/Utilities/ToolsParsersUtil_String.hpp"


ToolsParsersSyntax_Syntax::ToolsParsersSyntax_Syntax(
    const char * syntaxString)
  : m_SyntaxTree(0)
{
    if(generateSymbolString(syntaxString))
    {
        if(!generateSyntaxTreeFor(0, m_symbolString.size(), m_SyntaxTree))
        {
            if(0!=m_SyntaxTree)
            {
                delete m_SyntaxTree;
                m_SyntaxTree=0;
            }
        }
    }
}

ToolsParsersSyntax_Syntax::~ToolsParsersSyntax_Syntax(
    )
{
    Tools_Vector<ToolsParsersSyntEl_Terminal *>::iterator i=m_terminalObjects.begin();

    if(0!=m_SyntaxTree && !m_SyntaxTree->isTerminal())
        delete m_SyntaxTree;

    for(; i!=m_terminalObjects.end(); i++)
    {
        if(0!=(*i))
            delete (*i);
    }
}

ToolsParsersSyntEl_ParameterBase * ToolsParsersSyntax_Syntax::getSyntax(
    )
{
    if(0==m_SyntaxTree)
        return &m_InvalidDummy;
    else
        return m_SyntaxTree;
}

const ToolsParsersSyntEl_ParameterBase * ToolsParsersSyntax_Syntax::getSyntax(
    ) const
{
    if(0==m_SyntaxTree)
        return &m_InvalidDummy;
    else
        return m_SyntaxTree;
}

ToolsParsers_ParseResult & ToolsParsersSyntax_Syntax::getResult()
{
    return m_Result;
}

bool ToolsParsersSyntax_Syntax::printSyntax(
    ToolsParsersSyntEl_ParameterBase::PrintSyntaxStyle   style,
    char                                               * outText,
    size_t                                               spaceLeft) const
{
    bool spaceTooSmall=false;

    if(0!=outText && 0<spaceLeft)
    {
        size_t spaceNeeded=0;
        
        outText[0]='\0';

        getSyntax()->printSyntax(style, true, outText, spaceLeft, spaceNeeded, spaceTooSmall); //the complete syntax is always allone/outer element -> true
    }
    else
        spaceTooSmall=true;

    return !spaceTooSmall;
}

bool ToolsParsersSyntax_Syntax::generateSymbolString(
    const char * syntaxString)
{
    bool rc=true;

    while(rc && '\0'!=(*syntaxString)) //scan until error or end of syntax string
    {
        //ignore spaces
        syntaxString=ToolsParsersUtil_String::getNextNonWhiteSpace(syntaxString);

        //if not at end of syntax string
        if('\0'!=(*syntaxString))
        {
            //is it a non terminal symbol ?
            if( '('==(syntaxString[0]) ||
                ')'==(syntaxString[0]) ||
                '{'==(syntaxString[0]) ||
                '}'==(syntaxString[0]) ||
                '['==(syntaxString[0]) ||
                ']'==(syntaxString[0]) ||
                '|'==(syntaxString[0])    )
            {
                //take the non terminal symbol
                if(!m_symbolString.push_back(ToolsParsersSyntax_Symbol(syntaxString[0])))
                    rc=false;
                else
                {
                    //if we have a ")!" take the ! also
                    if(')'==(syntaxString[0]) && '!'==(syntaxString[1]))
                    {
                        if(!m_symbolString.push_back(ToolsParsersSyntax_Symbol(syntaxString[1])))
                            rc=false;
                        else
                            syntaxString++;
                    }

                    syntaxString++;
                }
            }
            else
            {
                //replace the terminal symbol with a symbol entry pointing to a fitting syntax element object
                const char * endOfTerminalName=syntaxString;
                const char * endOfTerminal=syntaxString;

                if('<'==syntaxString[0])
                    findEndOfVariable(syntaxString, endOfTerminalName, endOfTerminal);
                else
                    findEndOfKeyword(syntaxString, endOfTerminalName, endOfTerminal);

                //search object for terminal in list
                ToolsParsersSyntEl_ParameterBase * terminalObject=getTerminal(syntaxString, endOfTerminalName);

                //if such an object was not found, try to create one
                if(0==terminalObject)
                {
                    terminalObject=createTerminal(syntaxString, endOfTerminalName, endOfTerminal);

                    if(0==terminalObject)
                        rc=false;
                }

                //add the object to the provisional parse result of symbols
                if(rc && !m_symbolString.push_back(ToolsParsersSyntax_Symbol(terminalObject)))
                    rc=false;

                syntaxString=endOfTerminal;
            }
        }
    }

    if(!rc)
        m_Result.setErrorOutOfMemory();

    return rc;
}

bool ToolsParsersSyntax_Syntax::generateSyntaxTreeFor(
        size_t                              symbolStringStart,
        size_t                              symbolStringEnd,
        ToolsParsersSyntEl_ParameterBase *& tree)
{
    //the ToolsParsersSyntax_Syntax class must ensure, that symbolStringStart
    //is smaller or equal then symbolStringEnd

    bool rc=true;

    tree=0;

    //is there something to do? 
    if(symbolStringStart<symbolStringEnd)
    {
        bool done=false;

        ToolsParsersSyntEl_ParameterGroup * group=0;

        if(startsParenthesis(m_symbolString[symbolStringStart]))                             //string starts and ...
        {
            size_t endBracket=0;
            size_t startContent=0;
            size_t endContent=0;

            getEndAndContentOfParenthesis(
                symbolStringStart,
                symbolStringEnd,
                endBracket,
                startContent,
                endContent);

            if(endContent==endBracket) //missing closing bracket
            {
                m_Result.setErrorUnsupportedSyntaxDescription();
                rc=false;
            }
            else
                if(symbolStringEnd==endBracket) //... the closing bracket is last symbol of syntax string
                {
                    ToolsParsersSyntEl_ParameterBase * subTree=0;

                    if('['==m_symbolString[symbolStringStart].getNonTerminal() ||  // option with maximal 1 argument
                       '{'==m_symbolString[symbolStringStart].getNonTerminal()   ) // repetition with maximal 1 argument
                    {
                        rc=generateSyntaxTreeFor(startContent, endContent, subTree);

                        if(rc && 0!=subTree) //subTree can be 0 if parenthesis is empty
                        {
                            switch(m_symbolString[symbolStringStart].getNonTerminal())
                            {
                                case '[': tree=new ToolsParsersSyntEl_Option(subTree); break;
                                case '{': tree=new ToolsParsersSyntEl_Repetition(subTree); break;
                            }

                            if(0==tree)
                            {
                                m_Result.setErrorOutOfMemory();
                                rc=false;
                            }
                        };

                        done=true;
                    }
                    else
                        if('('==m_symbolString[symbolStringStart].getNonTerminal())
                        {
                            if(')'==m_symbolString[symbolStringEnd-1].getNonTerminal()) //found (...)
                                group=new ToolsParsersSyntEl_Concatenation;
                            else
                                if(m_symbolString[symbolStringEnd-1].equals('!'))   //found (...)!
                                    group=new ToolsParsersSyntEl_Set;
                                else
                                {
                                    m_Result.setErrorUnsupportedSyntaxDescription();
                                    rc=false;
                                }

                            symbolStringStart=startContent;
                            symbolStringEnd=endContent;

                            if(0==group)
                            {
                                m_Result.setErrorOutOfMemory();
                                rc=false;
                            }
                        }
                        else //unknown parenthesis type just
                        {
                            m_Result.setErrorUnsupportedSyntaxDescription();
                            rc=false;
                        }
                }
                //else found parenthesis does not contain the complete string -> just one parenthesis amongst others
        }

        if(rc && !done)
        {
            size_t i=symbolStringStart;
          
            //uppermost level contains at least one '|' -> handle the alternative first(' ' binds stronger than '|')
            if(symbolStringEnd!=findAlternativeEnd(i, symbolStringEnd))
            {
                //the found alternative will be the only member of the group -> forget about the group
                if(0!=group)
                    delete group;

                //get a new alternative list
                tree=group=new ToolsParsersSyntEl_AlternativeList;

                if(0==tree)
                {
                    m_Result.setErrorOutOfMemory();
                    rc=false;
                }

                //fill the alternative list with its members
                while(rc && i<symbolStringEnd)
                {
                    size_t endCurrent=findAlternativeEnd(i, symbolStringEnd); //find end of current list member
                    ToolsParsersSyntEl_ParameterBase * subTree=0;

                    rc=generateSyntaxTreeFor(i, endCurrent, subTree);         //generate the complete tree for that member

                    if(rc && !group->push_back(subTree))                      //add the member tree to the group
                    {
                        m_Result.setErrorOutOfMemory();
                        rc=false;
                    }
                                                                              //position to next member by...
                    i=endCurrent;                                             //... positioning to end current member

                    while(m_symbolString[i].equals('|') && i<symbolStringEnd) //...ignoring any '|' following the member
                        i++;
                }
            }
            else
                if(symbolStringEnd!=findEndOfSymbol(i, symbolStringEnd)) //more then one element?
                {
                    //if we have not already a group ("(...)" or "(...)!") get a concatenation ("x y")
                    if(0==group)
                        group=new ToolsParsersSyntEl_Concatenation;

                    if(0==group)
                    {
                        m_Result.setErrorOutOfMemory();
                        rc=false;
                    }

                    //fill the group with its members
                    while(rc && i<symbolStringEnd)
                    {
                        size_t endCurrent=findEndOfSymbol(i, symbolStringEnd); //find end of current list member
                        ToolsParsersSyntEl_ParameterBase * subTree=0;

                        rc=generateSyntaxTreeFor(i, endCurrent, subTree);      //generate the complete tree for that member

                        if(rc && !group->push_back(subTree))                   //add the member tree to the group
                        {
                            m_Result.setErrorOutOfMemory();
                            rc=false;
                        }

                        i=endCurrent;                                           //position to next member
                    }

                    tree=group;
                }
                else //group has exactly one element
                {
                    if(0!=group)        //forget about the group
                        delete group;

                    if(m_symbolString[symbolStringStart].isTerminal())
                        tree=m_symbolString[symbolStringStart].getTerminalObject();
                    else
                        rc=generateSyntaxTreeFor(symbolStringStart, symbolStringEnd, tree);
                }
        }
    }

    return rc;
}

ToolsParsersSyntEl_ParameterBase * ToolsParsersSyntax_Syntax::getTerminal(
    const char * startOfTerminalName,
    const char * endOfTerminalName)
{
    Tools_Vector<ToolsParsersSyntEl_Terminal *>::iterator i=m_terminalObjects.begin();

    for(; i!=m_terminalObjects.end() && !((*i)->getName().equals(startOfTerminalName, endOfTerminalName)); i++);

    if(i!=m_terminalObjects.end())
        return (*i);
    else
        return 0;
}

ToolsParsersSyntEl_ParameterBase * ToolsParsersSyntax_Syntax::createTerminal(
    const char * startOfTerminalName,
    const char * endOfTerminalName,
    const char * endOfTerminal)
{
    ToolsParsersSyntEl_Terminal * rc=0;

    if('<'==startOfTerminalName[0])
    {
        const char * startDefault=ToolsParsersUtil_String::findPositionBehindNext(endOfTerminalName, endOfTerminal, "DEF=");
        const char * endDefault=endOfTerminal;

        //search end of default
        if(startDefault<endDefault && '"'==startDefault[0])
        {
            startDefault++;
            endDefault=ToolsParsersUtil_String::findPositionOf(startDefault, endDefault, '"');
        }

        ToolsParsersSyntEl_Variable * newVar=new ToolsParsersSyntEl_Variable(startOfTerminalName, endOfTerminalName, startDefault, endDefault);

        if(0!=newVar)
        {
            if(newVar->isValid() && m_Result.pushBackVariable(newVar))
                rc=newVar;
            else
            {
                delete newVar;
                newVar=0;
            }
        }
    }
    else
    {
        ToolsParsersSyntEl_Keyword * newKey=new ToolsParsersSyntEl_Keyword(startOfTerminalName, endOfTerminalName);

        if(0!=newKey)
        {
            if(newKey->isValid() && m_Result.pushBackKeyword(newKey))
                rc=newKey;
            else
            {
                delete newKey;
                newKey=0;
            }
        }
    }

    if(0!=rc)
    {
        if(!m_terminalObjects.push_back(rc))
        {
            delete rc;

            rc=0;
        }
    }

    return rc;
}

size_t ToolsParsersSyntax_Syntax::findEndOfSymbol(
        size_t position,
        size_t end) const
{
    if(position<end)
    {
        if(startsParenthesis(m_symbolString[position]))
            position=getEndOfParenthesis(position, end);
        else
            position++;
    }
    else
        position=end;

    return position;
}

void ToolsParsersSyntax_Syntax::findEndOfVariable(
        const char  * position,
        const char *& endName,
        const char *& endVariable) const
{
    //search next '>' -> we allow spaces and brackets in variable names
    endName=ToolsParsersUtil_String::getPositionBehindNext(position, '>');

    endVariable=endName;

    if(0==strncmp("DEF=", endVariable, strlen("DEF="))) //"DEF=" is following the variable name
    {
        bool withinQuotes=false;

        endVariable+=strlen("DEF=");

        if('"'==(*endVariable))
        {
            withinQuotes=true;
            endVariable++;
        }
        
        while('\0'!=(*endVariable))
        {
            if(!withinQuotes)
            {
                if(isspace(*endVariable) ||
                   startsParenthesis(*endVariable) ||
                   endsParenthesis(*endVariable) ||
                   '|'==(*endVariable))
                {
                    break;
                }
            }

            if(withinQuotes && '"'==(*endVariable))
                withinQuotes=false;

            endVariable++;
        }
    }
}

void ToolsParsersSyntax_Syntax::findEndOfKeyword(
    const char  * position,
    const char *& endName,
    const char *& endKeyword) const
{
    endName=position;

    //search next space or bracket or '|'
    while('\0'!=(*endName) &&
          !isspace(*endName) &&
          !startsParenthesis(*endName) &&
          !endsParenthesis(*endName) &&
          '|'!=(*endName) )
    {
        endName++;
    }

    endKeyword=endName;
}

bool ToolsParsersSyntax_Syntax::startsParenthesis(char c) const
{
    return ('('==c || '{'==c || '['==c);
}

bool ToolsParsersSyntax_Syntax::endsParenthesis(char c) const
{
    return (')'==c || '}'==c || ']'==c);
}

bool ToolsParsersSyntax_Syntax::startsParenthesis(const ToolsParsersSyntax_Symbol & s) const
{
    return (!s.isTerminal() && startsParenthesis(s.getNonTerminal()));
}

size_t ToolsParsersSyntax_Syntax::getEndOfParenthesis(
    size_t startString,
    size_t endString) const
{
    size_t endParenthesis=0;
    size_t startContent=0;
    size_t endContent=0;

    getEndAndContentOfParenthesis(
        startString,
        endString,
        endParenthesis,
        startContent,
        endContent);

    return endParenthesis;
}

void ToolsParsersSyntax_Syntax::getEndAndContentOfParenthesis(
    size_t   startString,
    size_t   endString,
    size_t & endParenthesis,
    size_t & startContent,
    size_t & endContent) const
{
    size_t position=startString;

    endParenthesis=startContent=endContent=position;

    if(position<endString && startsParenthesis(m_symbolString[position]))
    {
        char         openingBracket=m_symbolString[position].getNonTerminal();
        char         closingBracket=0;
        unsigned int level=0;

        switch(openingBracket)
        {
            case '(': closingBracket=')'; break;
            case '{': closingBracket='}'; break;
            case '[': closingBracket=']'; break;
        }

        position++;            //step over the opening bracket
        startContent=position; //all opening parenthesis have size 1

        while(position<endString && (0!=level || closingBracket!=m_symbolString[position].getNonTerminal()))
        {
            if(openingBracket==m_symbolString[position].getNonTerminal())
                level++;

            if(closingBracket==m_symbolString[position].getNonTerminal())
                level--;

            position++;
        }

        endContent=position;

        if(position<endString && 0==level && closingBracket==m_symbolString[position].getNonTerminal())
        {
            position++;

            if(!m_symbolString[position].isTerminal() &&
               '!'==m_symbolString[position].getNonTerminal())
            {
                position++;
            }
        }

        endParenthesis=position;
    }
}

size_t ToolsParsersSyntax_Syntax::findAlternativeEnd(
        size_t startString,
        size_t endString)
{
    size_t position=startString;

    while(position<endString && !m_symbolString[position].equals('|'))
    {
        if(startsParenthesis(m_symbolString[position]))
            position=getEndOfParenthesis(position, endString);
        else
            position++;
    }

    return position;
}
