/*!
    \file    ToolsParsers_ParseResult.hpp
    \author  TiloH
    \ingroup 
    \brief   declaring a class managing the results of a parse run

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

#if !defined(TOOLSPARSERS_PARSERESULT_HPP)
#define TOOLSPARSERS_PARSERESULT_HPP


#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Keyword.hpp"
#include "ToolsCommon/Parsers/SyntaxElements/ToolsParsersSyntEl_Variable.hpp"
#include "ToolsCommon/Tools_Vector.hpp"


class ToolsParsersSyntEl_Keyword;
class ToolsParsersSyntEl_Variable;

/*! \brief a class managing a parsing result

    The class ToolsParsers_ProgramParameterParser uses an object of this class
    to store the results of the parse preparation and the parse run itself.

    As the parser class ToolsParsers_ProgramParameterParser returns only a
    const reference to its parse result object you will be restricted to the
    const functions of this class during analyzation of the parse result.
    
    \see ToolsParsers_ProgramParameterParser */
class ToolsParsers_ParseResult
{
  public:
    /*! \brief enumeration for possible result codes */
    enum ResultCode
    {
        NoError,                          /*!< no error occured before and during parsing */
        ErrorSyntax,                      /*!< the parameters could not be parsed completely, as they did not fit the expected syntax */
        ErrorOutOfMemory,                 /*!< not enough system memory could be alocated during preparation or parsing */
        ErrorUnsupportedSyntaxDescription /*!< the parser could not understand the syntax you supplied in its constructor */ 
    };

    /*! \brief constructor
    
        Used by class ToolsParsers_ProgramParameterParser. Usually there is no need,
        that you should construct a parse result object yourself. */
    ToolsParsers_ParseResult();

    /*! \brief find a keyword object for a certain keyword string

        This function must be used to get a reference of the keyword object of
        any keyword in the syntax used by a parser of class
        ToolsParsers_ProgramParameterParser. You have to specify the keyword as
        string.
        
        Look into class ToolsParsersSyntEl_Keyword for information about how
        the returned keyword object can be used.
        
        If the specified keyword was not part of the syntax, a reference
        to a dummy keyword is returned. The state of this dummy keyword will
        always be not found.

        \see ToolsParsersSyntEl_Keyword
        
        \param name [IN] name of the keyword
        \return reference to a matching keyword object, or to a generic dummy
            object which was not found */
    const ToolsParsersSyntEl_Keyword & getKeyword(
        const char * name) const;

    /*! \brief find a variable object for a certain variable name

        This function must be used to get a reference of the variable object of
        any variable in the syntax used by a parser of class
        ToolsParsers_ProgramParameterParser. You have to specify the variable
        name as string, the name can be enclosed in angle brackets ("\<xyz\>") or
        not ("xyz").
        
        Look into class ToolsParsersSyntEl_Variable for information about how
        the returned variable object can be used.

        If the specified variable was not part of the syntax, a
        reference to a dummy variable object is returned. The state of this
        dummy variable will always be not found and it will always have the
        empty string as variable value.
        
        \see ToolsParsersSyntEl_Variable

        \param name [IN] name of the variable
        \return reference to a matching variable object, or to a generic dummy
            object which was not found and has the empty string as value */
    const ToolsParsersSyntEl_Variable & getVariable(
        const char * name) const;

    /*! \brief get result code of parse preparation and parse run
        
        The result code represents the current state of the parse
        preparation and of the parse run itself

        \return result code as enum */
    ResultCode getResultCode() const;

    /*! \brief function for checking for a real parse error
        
        This function is a short version of <code>ToolsParsers_ParseResult::ErrorSyntax==getResultCode()?true:false</code>.

        \return true, if actual parameters did not match the syntax during the
            parse run, false otherwise */
    bool foundSyntaxError() const;

    /*! \brief function used during parser construction, to store a reference
        to a keyword object within the result
        
        \param newKeyword [IN] pointer to the keyword object */
    bool pushBackKeyword(
        ToolsParsersSyntEl_Keyword * newKeyword);

    /*! \brief function used during parser construction, to store a reference
        to a variable object within the result
        
        \param newVariable [IN] pointer to the variable object*/
    bool pushBackVariable(
        ToolsParsersSyntEl_Variable * newVariable);

    /*! \brief function for changing parse result
    
        parse result is set to ToolsParsers_ParseResult::ErrorSyntax */
    void setErrorSyntax();

    /*! \brief function for changing parse result
    
        parse result is set to ToolsParsers_ParseResult::ErrorOutOfMemory */
    void setErrorOutOfMemory();

    /*! \brief function for changing parse result
    
        parse result is set to ToolsParsers_ParseResult::ErrorUnsupportedSyntaxDescription */
    void setErrorUnsupportedSyntaxDescription();

  private:
    Tools_Vector<ToolsParsersSyntEl_Keyword *>  m_KeywordObjects;
    ToolsParsersSyntEl_Keyword                  m_KeywordDummy;

    Tools_Vector<ToolsParsersSyntEl_Variable *> m_VariableObjects;
    ToolsParsersSyntEl_Variable                 m_VariableDummy;

    ResultCode m_ResultCode;

    /*! \brief helper function to find keywords or variables by their name */
    template<class T> const T & getObject(
        const char              * objectName,
        const Tools_Vector<T *> & objectVector,
        const T                 & objectDummy) const;
};

#endif
