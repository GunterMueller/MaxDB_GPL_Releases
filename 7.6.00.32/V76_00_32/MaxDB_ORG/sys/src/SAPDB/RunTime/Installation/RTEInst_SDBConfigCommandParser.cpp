/*!
  \file    RTEInst_SDBConfigCommandParser.cpp
  \author  MarcW
  \ingroup RTE Installation
  \brief   parse command line of sdbconfig

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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

#include <stdio.h>

#include "RunTime/Installation/RTEInst_SDBConfigCommandParser.hpp"
#include "RunTime/Installation/RTEInst_SDBConfigConstants.hpp"
#include "SAPDBCommon/SAPDB_string.h"

const size_t RTEInst_SDBConfigCommandParser::m_PROGLEN(strlen(RTEInst_SDBConfigConstants::KEY_INDEPPROGRAMS));
const size_t RTEInst_SDBConfigCommandParser::m_DATALEN(strlen(RTEInst_SDBConfigConstants::KEY_INDEPDATA));

RTEInst_SDBConfigCommandParser::RTEInst_SDBConfigCommandParser( int argc, char* argv[] )
        : m_ParseError(true),
          m_CommandLine(NULL),
          m_Path(NULL),
          m_Command(None) {

    if( argv == NULL )
        m_ParseError = true;

    size_t length(0);
    for( int i=1; i<argc; ++i )
        length += strlen(argv[i]);

    if( length == 0 ) {
        // no arguments, list all
        m_Command = ListAll;
        m_ParseError = false;
        return;
    }

    m_CommandLine = new char[length];
    if( m_CommandLine == NULL )
        return;
    else {
        m_CommandLine[0] = '\0';
        for( int i=1; i<argc; ++i )
            strcat(m_CommandLine, argv[i]);
    }

    // now find out what to do
    
    if( length>=m_PROGLEN && 0==strncmp(m_CommandLine, RTEInst_SDBConfigConstants::KEY_INDEPPROGRAMS, m_PROGLEN) ) {
        if( length == m_PROGLEN ) {
            m_Command = GetIndepPrograms;
            m_ParseError = false;
        }
        else {
            if( m_CommandLine[m_PROGLEN] == '=' ) {
                m_Command = SetIndepPrograms;
                m_Path = &(m_CommandLine[m_PROGLEN+1]);
                m_ParseError = false;
            }
        }
    }
    else if( length>=m_DATALEN && 0==strncmp(m_CommandLine, RTEInst_SDBConfigConstants::KEY_INDEPDATA, m_DATALEN) ) {
        if( length == m_DATALEN ) {
            m_Command = GetIndepData;
            m_ParseError = false;
        }
        else {
            if( m_CommandLine[m_DATALEN] == '=' ) {
                m_Command = SetIndepData;
                m_Path = &(m_CommandLine[m_DATALEN+1]);
                m_ParseError = false;
            }
        }
    }
}

RTEInst_SDBConfigCommandParser::~RTEInst_SDBConfigCommandParser() {
    if( m_CommandLine != NULL )
        delete m_CommandLine;
}

bool RTEInst_SDBConfigCommandParser::listAll() {
    return m_Command == ListAll && !m_ParseError;
}

bool RTEInst_SDBConfigCommandParser::setIndepDataPath() {
    return m_Command == SetIndepData && !m_ParseError;
}

bool RTEInst_SDBConfigCommandParser::getIndepDataPath() {
    return m_Command == GetIndepData && !m_ParseError;
}

bool RTEInst_SDBConfigCommandParser::setIndepProgramsPath() {
    return m_Command == SetIndepPrograms && !m_ParseError;
}

bool RTEInst_SDBConfigCommandParser::getIndepProgramsPath() {
    return m_Command == GetIndepPrograms && !m_ParseError;
}

const char* RTEInst_SDBConfigCommandParser::getPath() {
    if( m_ParseError )
        return NULL;
    else
        return m_Path;
}

void RTEInst_SDBConfigCommandParser::printUsage( bool asErrorResponse ) {
    fprintf(asErrorResponse?stderr:stdout,
        "Syntax: sdbconfig [<name>[=<value>]]\n"
        "        <name> ::== %s|%s\n"
        "Usage: list, request or set global configuration parameter name\n"
        "If no <name> is given, content is listed\n",
        RTEInst_SDBConfigConstants::KEY_INDEPPROGRAMS,
        RTEInst_SDBConfigConstants::KEY_INDEPDATA);

}