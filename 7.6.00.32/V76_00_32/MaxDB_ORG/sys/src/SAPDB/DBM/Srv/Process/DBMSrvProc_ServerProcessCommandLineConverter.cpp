/*!
  \file    DBMSrvProc_ServerProcessCommandLineConverter.cpp
  \author  MarcW
  \brief   server process command line converter implementation

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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "heo01.h"

#include "DBM/Srv/Process/DBMSrvProc_ServerProcessCommandLineConverter.hpp"
#include "ToolsCommon/CString/ToolsCStr_Str.hpp"

#if defined (_WIN32)
  #define PATH_DELI "\\"
  #define PATH_TUEDDELCHEN "\""
#else
  #define PATH_DELI "/"
  #define PATH_TUEDDELCHEN ""
#endif

DBMSrvProc_ServerProcessCommandLineConverter::DBMSrvProc_ServerProcessCommandLineConverter(
        const char* aCommandLine)
        : m_IsValid(false),
          m_ProgramIndex(-1),
          m_ConvertedCommandLine(NULL) {

    // analyze index
    m_ProgramIndex = atoi(aCommandLine)-1;

    // check array range
    if( (m_ProgramIndex < 0) || (m_ProgramIndex >= (KeyInvalidEntry - 1)) )
        return;

    // check array entry
    if( m_ProgramIndex != (m_Programs[m_ProgramIndex].nKey - 1)) 
        return;

    // retrieve direcetory
    tsp00_Pathc    szDirectory;
    tsp01_RteError aRteError;
    switch (m_Programs[m_ProgramIndex].nDir){
      case DirDepBin:
        if(!sqlGetDbrootBinPath(szDirectory, TERM_WITH_DELIMITER_EO01, &aRteError)) {
          szDirectory.Init();
        } // end if
        break;
      case DirDepPgm:
        if(!sqlGetDbrootPgmPath(szDirectory, TERM_WITH_DELIMITER_EO01, &aRteError)) {
          szDirectory.Init();
        } // end if
        break;
      case DirIndepBin:
        if(!sqlGetIndependentBinPath(szDirectory, TERM_WITH_DELIMITER_EO01, &aRteError)) {
          szDirectory.Init();
        } // end if
        break;
      case DirIndepPgm:
        if(!sqlGetIndependentPgmPath(szDirectory, TERM_WITH_DELIMITER_EO01, &aRteError)) {
          szDirectory.Init();
        } // end if
        break;
      default: // DirNothing
        szDirectory.Init();
        break;
    } // end switch

    // skip program index
    const char* programArguments = strchr(aCommandLine, ' ');
    programArguments++;
    if( programArguments > aCommandLine + strlen(aCommandLine))
        return;

    // allocate memory
    m_ConvertedCommandLine = new char[
                strlen(szDirectory) +
                strlen(m_Programs[m_ProgramIndex].szProgram) +
                strlen(programArguments) +
                4]; // Tueddelchen + Tueddelchen + Space + Nullbyte
    // check memory
    if( m_ConvertedCommandLine == NULL )
        return;

    // generate command
    sprintf(m_ConvertedCommandLine,
                "%s%s%s%s %s",
                PATH_TUEDDELCHEN,
                szDirectory.asCharp(),
                m_Programs[m_ProgramIndex].szProgram,
                PATH_TUEDDELCHEN,
                programArguments);
    // constructor completed sucessfull
    m_IsValid = true;
}

DBMSrvProc_ServerProcessCommandLineConverter::~DBMSrvProc_ServerProcessCommandLineConverter() {
    if( m_ConvertedCommandLine != NULL )
        delete[] m_ConvertedCommandLine;
}

bool DBMSrvProc_ServerProcessCommandLineConverter::removePasswordFromCommandLine(
        const char* lineIn,
        char*& lineOut,
        DBMSrvMsg_Error& errOut ) {

    lineOut = NULL;
    if( !ToolsCStr_Str::AllocCpy(lineOut, lineIn, errOut) )
        return false;
    else {
        size_t terminatorIndex(strlen(lineOut));
        size_t currentIndex(0);
        bool notReplaced(true);
        while( notReplaced && currentIndex < terminatorIndex ) {
            if( lineOut[currentIndex] == '-' && ++currentIndex < terminatorIndex && lineOut[currentIndex] == 'u' && ++currentIndex < terminatorIndex && lineOut[currentIndex] == ' ') {
                // we have found the user option -u<blank>, now look for the ','
                while( currentIndex < terminatorIndex ) {
                    if( lineOut[currentIndex] != ',' )
                        ++currentIndex;
                    else {
                        // neither username nor password can contain blanks, that's cool :-)
                        ++currentIndex;
                        // now currentIndex points to the first character of the password
                        if( currentIndex < terminatorIndex ) {
                            size_t asteriskIndex(currentIndex);
                            lineOut[asteriskIndex] = '*'; // replace first character of password with '*'
                            // now move all characters behind the password behind the '*'
                            while( currentIndex < terminatorIndex && lineOut[currentIndex] != ' ' )
                                ++currentIndex;
                            if( currentIndex > asteriskIndex ) {
                                memmove(&lineOut[asteriskIndex+1], &lineOut[currentIndex], (terminatorIndex-currentIndex)+1);
                            }
                        }
                        notReplaced = false;
                    }
                }
            }
            else
                ++currentIndex;
        }
    }
    return true;
}

// these strings may not contain spaces...
// when adding a new string, don't forget to increment array (also in header file)
DBMSrvProc_ServerProcessCommandLineConverter::Programs DBMSrvProc_ServerProcessCommandLineConverter::m_Programs[] = {
// nKey                       bPipe  nDir         szProgram
  {KeySchedulerProgram,       true,  DirDepPgm,   "dbmsrvscd"  },
  {KeyEventDispatcherProgram, true,  DirDepBin,   "dbmevtdisp" },
  {KeyDbanalyzer,             true,  DirIndepBin, "dbanalyzer" },
#if defined(WIN32)
  {KeyDbmcli,                 true,  DirIndepPgm, "dbmcli"     },
#else
  {KeyDbmcli,                 true,  DirIndepBin, "dbmcli"     },
#endif
  {KeyInvalidEntry,           false, DirUnknown,  ""           }};
