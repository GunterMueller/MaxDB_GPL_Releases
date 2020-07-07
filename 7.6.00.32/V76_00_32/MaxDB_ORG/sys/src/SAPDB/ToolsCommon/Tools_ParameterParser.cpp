/*!
    @file           Tools_ParameterParser.cpp
    @author         MarcW
    @brief          parsing and checking logon parameters - Implementation

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

/* 
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "SAPDBCommon/SAPDB_Types.hpp"
#include "ToolsCommon/Tools_Vector.hpp"
#include "ToolsCommon/Tools_ParameterParser.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

/* 
  -----------------------------------------------------------------------------
  private declarations
  -----------------------------------------------------------------------------
*/

#define SWITCH_CHAR '-'

Tools_Parameter::Tools_Parameter( const Tools_DynamicUTF8String& name,
                                  const Tools_DynamicUTF8String& desc,
                                  const SAPDB_Bool isRequired,
                                  const SAPDB_Bool needsValue )
        : m_sName(name),
          m_sDesc(desc),
          m_bIsRequired(isRequired),
          m_sValue(""),
          m_bNeedsValue(needsValue),
          m_bIsSet(false)
{};

Tools_Parameter::Tools_Parameter()
        : m_sName(""),
          m_bIsRequired(false),
          m_sValue(""),
          m_bNeedsValue(false),
          m_bIsSet(false)
{};

// this is most probably never visible...
Tools_DynamicUTF8String Tools_Parameter::textdefault("a parameter");

Tools_ParameterParser::Tools_ParameterParser()
        : m_vFormalParams(),
          m_sAppName("")
{};

void Tools_ParameterParser::addFormalParameter( Tools_Parameter& param ) {
    m_vFormalParams.push_back(&param);
}

Tools_DynamicUTF8String Tools_ParameterParser::getParameterValue( 
    const Tools_DynamicUTF8String& paramName ) const {
    // step through vector
        for( Tools_Vector<Tools_Parameter*>::iterator iParam = m_vFormalParams.begin();
                iParam != m_vFormalParams.end();
                iParam++ ) {
        Tools_Parameter* pParam = *iParam;
        if( pParam->isSet() && pParam->getName() == paramName )
            return pParam->getValue();
    }
    return "";
};

SAPDB_Bool Tools_ParameterParser::isParameterSet(
            const Tools_DynamicUTF8String& paramName ) const {
    for( Tools_Vector<Tools_Parameter*>::iterator iParam = m_vFormalParams.begin();
                iParam != m_vFormalParams.end();
                iParam++ ) {
        Tools_Parameter* pParam = *iParam;
        if( pParam->isSet() && pParam->getName() == paramName )
            return true;
    }
    return false;
}

void Tools_ParameterParser::setActualParameters( int argc, char* argv[] ) {
    // set the application name    
    m_sAppName = argv[0];

    for( int acount=1; acount<argc; acount++ ) {
        if( argv[acount][0] == SWITCH_CHAR ) {
            Tools_DynamicUTF8String paramName(argv[acount]+1);
            for( Tools_Vector<Tools_Parameter*>::iterator iParam = m_vFormalParams.begin();
                    iParam != m_vFormalParams.end();
                    iParam++ ) {
                Tools_Parameter* pParam = *iParam;     
                if( pParam->getName() == paramName ) {
                    // an actual parameter name matches a formal one
                    pParam->setSet();
                    if( acount+1 < argc && pParam->needsValue() ) {
                        // it needs a value, so we take the next argument as value
                        acount++;
                        pParam->setValue(argv[acount]);
                    }
                }
            }
        }
    }
};

SAPDB_Bool Tools_ParameterParser::isParameterlistValid() const {
    for( Tools_Vector<Tools_Parameter*>::iterator iParam = m_vFormalParams.begin();
                iParam != m_vFormalParams.end();
                iParam++ ) {
        Tools_Parameter* pParam = *iParam;
        if( !pParam->isSet() && pParam->isRequired() )
            return false;
    }
    // we get here, if all required parameters are set
    return true;
};

void Tools_ParameterParser::printUsage(FILE *dest) {
    fprintf (dest, "usage: %s [<options>]\n", getAppName().CharPtr());
    fprintf (dest, "  <options>:\n");
    for( Tools_Vector<Tools_Parameter*>::iterator iParam = m_vFormalParams.begin();
                iParam != m_vFormalParams.end();
                iParam++ ) {
        Tools_Parameter* pParam = *iParam;
        if( !pParam->isRequired() )
            fprintf (dest, "    -%-8s %s, optional\n", pParam->getName().CharPtr(), pParam->getDescription().CharPtr());
        else
            fprintf (dest, "    -%-8s %s\n", pParam->getName().CharPtr(), pParam->getDescription().CharPtr());        
    }
};
