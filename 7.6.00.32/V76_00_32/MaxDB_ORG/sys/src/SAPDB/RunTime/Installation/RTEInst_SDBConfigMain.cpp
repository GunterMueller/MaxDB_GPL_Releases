/*!
    @file           RTEInst_SDBConfigMain.cpp
    @author         MarcW
    @brief          sdbconfig  - main function

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


*/

#include <string.h>

#include "RunTime/Installation/RTEInst_SDBConfigCommandParser.hpp"
#include "RunTime/Installation/RTEInst_SDBConfigConstants.hpp"
#include "RunTime/Installation/RTEInst_SDBConfiguration.hpp"

/*!
    @brief main function of sdbconfig program on windows
    @param argc [IN] number of passed arguments including program name
    @param argv [IN] argument vector
    @return RTEInst_SDBConfigConstants::OK in case of success, a different value otherwise
    @see RTEInst_SDBConfigConstants
*/
int main (int argc, char* argv[]) {
    
    RTEInst_SDBConfigCommandParser parser(argc, argv);

    if( argc==2 && 0==strcmp(argv[1], RTEInst_SDBConfigConstants::HELPREQUEST) ) {
        parser.printUsage(false);
        return RTEInst_SDBConfigConstants::OK;
    }
    else if( parser.parseError() ) {
        parser.printUsage(true);
        return RTEInst_SDBConfigConstants::SYNTAXERROR;
    }
    else {
        
        RTEInst_SDBConfiguration theSDBConfiguration;
        if( parser.listAll() )
            return theSDBConfiguration.listAllParameters();
        else if( parser.setIndepDataPath() )
            return theSDBConfiguration.setIndepDataPath(parser.getPath());
        else if( parser.getIndepDataPath() )
            return theSDBConfiguration.getIndepDataPath();
        else if( parser.setIndepProgramsPath() )
            return theSDBConfiguration.setIndepProgramsPath(parser.getPath());
        else if( parser.getIndepProgramsPath() )
            return theSDBConfiguration.getIndepProgramsPath();
        else
            // we never get here
            return RTEInst_SDBConfigConstants::NEVERRETURNEDRETURNCODE;
    }
}
