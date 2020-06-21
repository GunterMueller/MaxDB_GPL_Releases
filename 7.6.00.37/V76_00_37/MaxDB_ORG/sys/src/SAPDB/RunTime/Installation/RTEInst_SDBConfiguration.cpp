/*!
  \file    RTEInst_SDBConfiguration.cpp
  \author  MarcW
  \ingroup RTE Installation
  \brief   get/set configuration parameters

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

#include "heo01.h"

#include "RunTime/Installation/RTEInst_SDBConfigConstants.hpp"
#include "RunTime/Installation/RTEInst_SDBConfiguration.hpp"

int RTEInst_SDBConfiguration::setIndepDataPath(const char* newPath) {
    return setPath( newPath, &sqlSetIndependentDataPath);
}

int RTEInst_SDBConfiguration::getIndepDataPath(bool printWithKey) {
    
    return getPath( RTEInst_SDBConfigConstants::KEY_INDEPDATA,
                    &sqlGetIndependentDataPath,
                    &sqlIsIndependentDataPathInRegistry,
                    printWithKey);
}

int RTEInst_SDBConfiguration::setIndepProgramsPath(const char* newPath) {
    return setPath( newPath, &sqlSetIndependentProgramsPath);
}

int RTEInst_SDBConfiguration::getIndepProgramsPath(bool printWithKey) {
    
    return getPath( RTEInst_SDBConfigConstants::KEY_INDEPPROGRAMS,
                    &sqlGetIndependentProgramsPath,
                    &sqlIsIndependentProgramsPathInRegistry,
                    printWithKey);
}

int RTEInst_SDBConfiguration::listAllParameters() {
    
    int data(getIndepDataPath(true));
    int programs(getIndepProgramsPath(true));

    if( RTEInst_SDBConfigConstants::OK == data && RTEInst_SDBConfigConstants::OK == programs )
        return RTEInst_SDBConfigConstants::OK;
    else
        return RTEInst_SDBConfigConstants::WORKERROR;
}

int RTEInst_SDBConfiguration::setPath(const char* newPath, PathSetterFuntionPointer pathSetterFuntion) {

    tsp00_Pathc newPathArray;
    if( !movePathTo(newPathArray, newPath) )
        return RTEInst_SDBConfigConstants::INTERNALERROR;

    tsp01_RteError rteError;
    if( pathSetterFuntion( newPathArray, &rteError ) )
        return RTEInst_SDBConfigConstants::OK;
    else {
        // something went wrong, print the error texts
        printRTEError(rteError);
        return RTEInst_SDBConfigConstants::WORKERROR;
    }
}

int RTEInst_SDBConfiguration::getPath(
        const char* key,
        PathGetterFuntionPointer pathGetterFunction,
        RegistryCheckFuntionPointer checkFunction,
        bool printWithKey) {
    
    tsp00_Pathc pathArray;
    tsp01_RteError rteError;
    if( pathGetterFunction(pathArray, TERM_WITHOUT_DELIMITER_EO01, &rteError) ) {
        printValue( key, pathArray.asCharp(), checkFunction()?true:false, printWithKey );
        return RTEInst_SDBConfigConstants::OK;
    }
    else {
        // something went wrong, print the error texts
        printRTEError(rteError);
        return RTEInst_SDBConfigConstants::WORKERROR;
    }
}

bool RTEInst_SDBConfiguration::movePathTo( tsp00_Pathc& pathArray, const char* path ) {
    
    if( path == NULL ) {
        fprintf(stderr, "new path is NULL\n");
        return false;
    }
    
    if( (int)strlen(path) > pathArray.size() ) {
        fprintf(stderr, "new path \"%s\" is too long (longer than %d characters)\n", path, pathArray.size());
        return false;
    }

    pathArray.rawAssign(path);
    return true;
}

void RTEInst_SDBConfiguration::printRTEError(tsp01_RteError& rteError) {
    
    // we make sure that the strings are 0-terminated
    if( rteError.RteErrText[RTE_ERROR_TEXT_MXSP01-1] != '\0' )
        rteError.RteErrText[RTE_ERROR_TEXT_MXSP01-1] = '\0';
    if( rteError.OsErrText[OS_ERROR_TEXT_MXSP01-1] != '\0' )
        rteError.OsErrText[OS_ERROR_TEXT_MXSP01-1] = '\0';

    fprintf(stderr,
        "Error!\n"
        "\tRTE code: %d\n"
        "\tRTE text: %s\n"
        "\tOS  code: %d\n"
        "\tOS  text: %s\n",
        (int) rteError.RteErrCode,
        rteError.RteErrCode==RTE_NO_ERROR_SP01?"":rteError.RteErrText,
        (int) rteError.OsErrCode,
        rteError.OsErrCode==OS_NO_ERROR_SP01?"":rteError.OsErrText);
}

void RTEInst_SDBConfiguration::printValue(const char* key, const char* value, bool takenFromRegistry, bool printWithKey) {
    if( printWithKey )
        fprintf(stdout,
            "%s=%s%s\n",
            key==NULL?"":key,
            value==NULL?"":value,
            takenFromRegistry?"":" (extracted from environment variable DBROOT)");
    else
        fprintf(stdout,
            "%s%s\n",
            value==NULL?"":value,
            takenFromRegistry?"":" (extracted from environment variable DBROOT)");
}