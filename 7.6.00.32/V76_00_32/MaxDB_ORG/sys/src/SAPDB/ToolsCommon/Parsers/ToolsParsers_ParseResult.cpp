/*!
    \file    ToolsParsers_ParseResult.cpp
    \author  TiloH
    \ingroup 
    \brief   definition of a class managing the results of a parse run

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

#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Keyword.hpp"
#include "ToolsCommon/Parsers/ToolsParsers_ParseResult.hpp"


ToolsParsers_ParseResult::ToolsParsers_ParseResult(
    )
  : m_ResultCode(ToolsParsers_ParseResult::NoError),
    m_KeywordDummy(""),
    m_VariableDummy("")
{
    if(!m_KeywordDummy.isValid() || !m_VariableDummy.isValid())
        setErrorOutOfMemory();
}

bool ToolsParsers_ParseResult::pushBackKeyword(
    ToolsParsersSyntEl_Keyword * newKeyword)
{
    return m_KeywordObjects.push_back(newKeyword);
}

bool ToolsParsers_ParseResult::pushBackVariable(
    ToolsParsersSyntEl_Variable * newVariable)
{
    return m_VariableObjects.push_back(newVariable);
}

void ToolsParsers_ParseResult::setErrorSyntax()
{
    m_ResultCode=ToolsParsers_ParseResult::ErrorSyntax;
}

void ToolsParsers_ParseResult::setErrorOutOfMemory()
{
    m_ResultCode=ToolsParsers_ParseResult::ErrorOutOfMemory;
}

void ToolsParsers_ParseResult::setErrorUnsupportedSyntaxDescription()
{
    m_ResultCode=ToolsParsers_ParseResult::ErrorUnsupportedSyntaxDescription;
}

ToolsParsers_ParseResult::ResultCode ToolsParsers_ParseResult::getResultCode() const
{
    return m_ResultCode;
}

bool ToolsParsers_ParseResult::foundSyntaxError() const
{
    return ToolsParsers_ParseResult::ErrorSyntax==m_ResultCode;
}

const ToolsParsersSyntEl_Keyword & ToolsParsers_ParseResult::getKeyword(
    const char * name) const
{
    return getObject<ToolsParsersSyntEl_Keyword>(name, m_KeywordObjects, m_KeywordDummy);
}

const ToolsParsersSyntEl_Variable & ToolsParsers_ParseResult::getVariable(
    const char * name) const
{
    return getObject<ToolsParsersSyntEl_Variable>(name, m_VariableObjects, m_VariableDummy);
}

template<typename T> const T & ToolsParsers_ParseResult::getObject(
    const char              * objectName,
    const Tools_Vector<T *> & objectVector,
    const T                 & objectDummy) const
{
    bool foundObject=false;

    typename Tools_Vector<T *>::iterator i=objectVector.begin();
    
    while(!foundObject && i!=objectVector.end())
    {
        if((*i)->getName().equals(objectName))
            foundObject=true;
        else
            i++;
    }

    if(foundObject)
        return (*(*i));
    else
        return objectDummy;
}
