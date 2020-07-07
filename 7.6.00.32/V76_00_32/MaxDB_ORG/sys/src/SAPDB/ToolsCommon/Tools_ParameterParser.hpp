/*!
    @file           Tools_ParameterParser.hpp
    @author         MarcW
    @brief          parsing and checking logon parameters - Specification

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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

#ifndef _Tools_ParameterParser_HPP_
#define _Tools_ParameterParser_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */
#include <stdio.h>

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "ToolsCommon/Tools_Vector.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

/*!
    @brief     Parameter class

    One instance of this class represents one parameter, that is to be passed to
    a program via the command line. A parameter has a name (e.g. "d" if
    on the command line "-d"), a description (displayed in a "usage" message, if
    the program was not called correctly), a flag whether it is required and a flag
    whether in needs a value of if it's just a switch.
*/
class Tools_Parameter {
public:
    /*! @brief to be able to use this in template class Tools_Vector */
    Tools_Parameter();

    /*!
        @brief Constructor

        A constructor that takes values for all attributes of a parameter
        @param name       [IN] name of the parameter
        @param desc       [IN] short description
        @param isRequired [IN] is parameter required or optional
        @param needsValue [IN] does it take a value?
    */
    Tools_Parameter( const Tools_DynamicUTF8String& name,
                     const Tools_DynamicUTF8String& desc = textdefault,
                     const SAPDB_Bool isRequired = true, 
                     const SAPDB_Bool needsValue = true);

    /*! 
        @brief get name of this parameter

        @return name of this parameter
    */
    Tools_DynamicUTF8String getName() const { return m_sName; };

    /*!
        @brief get description of this parameter

        @return description of this parameter
    */
    Tools_DynamicUTF8String getDescription() const { return m_sDesc; };

    /*!
        @brief get value of this parameter
        
        @return value of this parameter
    */
    Tools_DynamicUTF8String getValue() const {return m_sValue; };

    /*!
        @brief set value of this paramenter

        Value is actually set only of the parameter needs a value.
        @param value [IN] the value
    */
    void setValue( const Tools_DynamicUTF8String& value ) { if(m_bNeedsValue) m_sValue = value; };

    /*!
        @brief document that the parameter's value is set

        Necessary for paramenters that do not take a value (switches).
        Call this if the switch is set on command line.
    */
    void setSet() { m_bIsSet = true; };
    
    /*!
        @brief get required flag of this parameter

        @return required flag of this parameter
    */
    SAPDB_Bool isRequired() const { return m_bIsRequired; };

    /*!
        @brief does this parameter need a value?

        @return true if this parameter needs a value, false otherwise
    */
    SAPDB_Bool needsValue() const { return m_bNeedsValue; };

    /*!
        @brief check whether is parameter is set

        @return true if this parameter is set, false otherwise
    */
    SAPDB_Bool isSet() const { return m_bIsSet; };

private:
    /*! @brief parameter's name */
    Tools_DynamicUTF8String m_sName;
    /*! @brief parameter's short description */
    Tools_DynamicUTF8String m_sDesc;
    /*! @brief parameter's value */
    Tools_DynamicUTF8String m_sValue;
    /*! @brief stores whether parameter is required */
    SAPDB_Bool m_bIsRequired;
    /*! @brief stores whether parameter needs a value (or is a switch) */
    SAPDB_Bool m_bNeedsValue;
    /*! @brief stores whether parameter is set already */
    SAPDB_Bool m_bIsSet;
    
    /*! @brief stores whether parameter is required */
    static Tools_DynamicUTF8String textdefault;
};

/*!
    @brief     Parameter parser class

    This parameter parser class handles parameter existance checking. It can be used by
    command line program to check, whether the required parameters are passed, which optional
    parameters are passed and which switches are set.

    The client program just creates and adds formal parameters to the parser and applies the actual
    command line (that contains the actual parameters) to the parser. After this, the program
    checks with the parser, whether the command line is valid in a sense that all required
    parameters are passed. If it is not valid, the program can print a "usage" message to some
    I/O-stream using the parser.

    The Tools_Parameter objects that were added to the parser must exist until the last usage
    of the parser, since it only handles references to theses objects!

    On the actual command line, the parameter names and their values must be separated by at least
    one blank char.
*/
class Tools_ParameterParser {
public:
    /*! @brief Constructor */
    Tools_ParameterParser();
    /*!
        @brief check if current parameter list is valid
        
        After adding formal parameters to the parser and setting the actual, this method returns,
        whether the actual parameters "fulfill" the requirements of the formal parameters.
        @see addFormalParameter()
        @see setActualParameters()
        @return indicator if actual parameterlist is valid
    */
    SAPDB_Bool isParameterlistValid() const;
    /*!
        @brief get a parameter's value

        @param paramName [IN] name of parameter
        @return paramName's value or an empty string if parameter is not set or does not exist
    */
    Tools_DynamicUTF8String getParameterValue( const Tools_DynamicUTF8String& paramName ) const;

    /*!
        @brief get a parameter's value

        @param paramName [IN] name of parameter
        @return paramName's value or an empty string if parameter is not set or does not exist
    */
    Tools_DynamicUTF8String getParameterValue( const char* paramName ) const;

    /*!
        @brief get a parameter's value

        @param paramName [IN] name of parameter
        @return paramName's value or an empty string if parameter is not set or does not exist
    */
    Tools_DynamicUTF8String getParameterValue( const Tools_Parameter& parameter ) const;

    /*!
        @brief check whether a parameter is set

        Checks, if a parameter of the given name is set.
        @param paramName [IN] name of parameter
        @return true, if parameter or switch is set
    */
    SAPDB_Bool isParameterSet( const Tools_DynamicUTF8String& paramName ) const;

    /*!
        @brief check whether a parameter is set

        Checks, if a parameter which has the same name as the given parameter
        is set.
        @param parameter [IN] parameter to check
        @return true, if a parameter or switch with the same name is set
    */
    SAPDB_Bool isParameterSet( const Tools_Parameter& parameter ) const;

    /*!
        @brief set the actual parameters

        The actual command line is passed to the parser. The arguments passed to
        the main function can just be forwarded. Values of the parameters are
        assigned to the Tools_Parameter objects that were added to the parser
        before.
        @see addFormalParameter()
        @param argc [IN] the argc parameter passed to main function
        @param argv [IN] the argv parameter passed to main function
    */
    void setActualParameters( int argc, char* argv[] );
    
    /*!
        @brief get application name

        Returns the application name. Should only be called after setting the actual
        parameters.
        @see setActualParameters()
        @return application name
    */
    Tools_DynamicUTF8String getAppName() const { return m_sAppName; };

    /*!
        @brief add one formal parameter

        A reference of a Tools_Parameter is added. After applying the actual
        values, the passed object carries it's actual value from the command line.
        @see Tools_Parameter
        @see setActualParameters()
        @param param [IN] parameter to add
    */
    void addFormalParameter( Tools_Parameter& param );

    /*!
        @brief dump a usage message

        Dumps a usage message to the passed I/O-stream. The message is composed from the
        actual parameters' attributes (name, description, required).
        @see Tools_Parameter
        @param dest [IN] stream to print the usage message to
    */
    void printUsage(FILE *dest);

private:
    /*! @brief formal parameters */
    Tools_Vector<Tools_Parameter*> m_vFormalParams;
    /*! @brief application name. Used in usage message. @see printUsage() */
    Tools_DynamicUTF8String m_sAppName;
};

inline SAPDB_Bool Tools_ParameterParser::isParameterSet(
        const Tools_Parameter& parameter ) const {
    return isParameterSet(parameter.getName());
}

inline Tools_DynamicUTF8String Tools_ParameterParser::getParameterValue(
        const char* paramName ) const {
    const Tools_DynamicUTF8String sName( paramName );
    return getParameterValue( sName );
};

inline Tools_DynamicUTF8String Tools_ParameterParser::getParameterValue(
        const Tools_Parameter& parameter ) const {
    return getParameterValue( parameter.getName() );
};

#endif /* _Tools_ParameterParser_HPP_ */
