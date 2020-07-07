/*!
    @file           DBMSrvShM_ShmMigrator.cpp
    @author         MarcW
    @brief          shared memory administration: migration

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

#include "gsp05.h"
#include "heo01.h"
#include "heo06.h"

#include "DBM/Srv/SharedMemory/DBMSrvShM_ShmMigrator.hpp"
#include "ToolsCommon/Tools_DynamicUTF8String.hpp"

const char* const DBMSrvShM_ShmMigrator::infoFile("dbm.shi");
const char* const DBMSrvShM_ShmMigrator::dataFile("dbm.shm");

/*!
    @brief precondition: passed rundirectory exists!
*/
void DBMSrvShM_ShmMigrator::deleteShmFromRundirectory(const char* runDirectory) {

    if( runDirectory == NULL || strlen(runDirectory)==0 )
        return;

    // prepare rundirectory string
    Tools_DynamicUTF8String runDir(runDirectory);
    if( runDir[runDir.Size()-1] != PATH_DELIMITER_EO01 )
        runDir.Append(PATH_DELIMITER_EO01);
    
    // filename
    Tools_DynamicUTF8String fileName;
    tsp05_RteFileError rteFileError;
    
    fileName = runDir;
    fileName.Append(infoFile);
    sqlferasec(fileName.CharPtr(), &rteFileError);
    fileName = runDir;
    fileName.Append(dataFile);
    sqlferasec(fileName.CharPtr(), &rteFileError);

}
