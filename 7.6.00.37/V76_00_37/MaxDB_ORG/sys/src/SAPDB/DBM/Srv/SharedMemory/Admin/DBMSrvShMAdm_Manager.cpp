/*!
  \file    DBMSrvShMAdm_Manager.cpp
  \author  MarcW
  \brief   shared memory administration

    ========== licence begin  GPL
    Copyright (c) 2002-2006 SAP AG

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

#include "heo01.h"
#include "heo06.h"

#include "DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_ReplyBuffer.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"
#include "DBM/Srv/SharedMemory/Admin/DBMSrvShMAdm_Manager.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_BaseUnlocker.hpp"
#include "DBM/Srv/SharedMemory/DBMSrvShM_Persistence.hpp"
#include "RunTime/RTE_Database.hpp" 
#include "RunTime/RTE_DBRegister.hpp"
#include "ToolsCommon/Parsers/Old/ToolsParsersOld_ProgramParameterParser.hpp"

const int DBMSrvShMAdm_Manager::returnValueOkay(0);
const int DBMSrvShMAdm_Manager::returnValueParameters(1);
const int DBMSrvShMAdm_Manager::returnValueNoSuccess(2);

DBMSrvShMAdm_Manager::DBMSrvShMAdm_Manager(int argc, char* argv[])
        : m_ExecutableName(argv[0]),
          m_ArgCount(argc),
          m_ArgVector(argv) {
}

DBMSrvShMAdm_Manager::~DBMSrvShMAdm_Manager() {
}

int DBMSrvShMAdm_Manager::run() {
    
    if( !checkArguments() )
        return returnValueParameters;

    DBMSrvMsg_Error errOut;
    bool success(false);
    
    if( m_Action == ActionUnlock )
        success = unlockSharedMemory(errOut);
    else if( m_Action == ActionDelete )
        success = deleteSharedMemory(errOut);
    else
        success = checkSharedMemory(errOut);

    DBMSrvInfrReply_ReplyBuffer replyBuffer;
    DBMSrv_Reply& reply(replyBuffer.getReply());
    if( success ) {
        // print success reply
        reply.startWithOK();
        fprintf(stdout, "%s\n", reply.giveData());
        return returnValueOkay;
    }
    else {
        // print error reply
        reply.startWithMessageList(errOut);
        
        fprintf(stderr, "%s\n", reply.giveData());
        return returnValueNoSuccess;
    }
}

bool DBMSrvShMAdm_Manager::checkArguments() {
    
    // prepare the syntax
    ToolsParsersOldSyntEl_Keyword actionUnlock("UNLOCK", true);
    ToolsParsersOldSyntEl_Keyword actionDelete("DELETE", true);
    ToolsParsersOldSyntEl_Keyword actionCheck("CHECK", true);

    ToolsParsersOldSyntEl_AlternativeList action;
    action.push_back(&actionUnlock);
    action.push_back(&actionDelete);
    action.push_back(&actionCheck);

    ToolsParsersOldSyntEl_Variable indepDataPath("indepDataPath");

    ToolsParsersOldSyntEl_Variable dbName("dbName");

    ToolsParsersOldSyntEl_Concatenation allArguments;
    allArguments.push_back(&action);
    allArguments.push_back(&indepDataPath);
    allArguments.push_back(&dbName);

    // parse the command line
    ToolsParsersOld_ProgramParameterParser commandLineParser(m_ArgCount, m_ArgVector, &allArguments);
    bool syntaxError(false);
    commandLineParser.parse(syntaxError);
    if( syntaxError ) {
        char buf[1024];
        buf[0] = 0;
        allArguments.printSyntax(buf, 1023);
        fprintf(stderr, "usage:\n%s %s\n", m_ExecutableName.CharPtr(), buf);
        return false;
    }
    else {
        // no syntax error, we know all we need

        // get the database name
        m_DBName = dbName.getValue();

        // get the action
        if( actionUnlock.isSet() )
            m_Action = ActionUnlock;
        else if( actionDelete.isSet() )
            m_Action = ActionDelete;
        else
            m_Action = ActionCheck;

        // get the folder name
        m_ShMFolder = indepDataPath.getValue();
        if( m_ShMFolder[m_ShMFolder.Size()-1] != PATH_DELIMITER_EO01 )
            m_ShMFolder.Append(PATH_DELIMITER_EO01);
        m_ShMFolder.Append("wrk");
        m_ShMFolder.Append(PATH_DELIMITER_EO01);


        // now check whether this executable is in the correct release for the database
        Msg_List ignoreMe;
        RTE_Database* database = RTE_DBRegister::Instance().GetDatabase(m_DBName.CharPtr(), ignoreMe);
        if( !database ) {
            // not found, exit
            fprintf(stderr, "database %s not found\n", m_DBName.CharPtr());
            return false;
        }
        else {
            Tools_DynamicUTF8String dbRoot((const char*)database->GetDBRoot());
            delete database;

            // check if this is the correct executable for this database
            tsp00_Pathc    execPath;
            tsp01_RteError rteError;
            sqlGetMyModuleFilename( m_ExecutableName.CharPtr(), execPath.asCharp(), execPath.size(), &rteError );
            // dbroot must be a prefix of execPath
            char* occurrence = strstr(execPath.asCharp(), dbRoot.CharPtr());
            if( occurrence == execPath.asCharp() ) {
                // execPath starts with dbroot, that's what we want
            }
            else {
                const char* executableName = strrchr(m_ExecutableName.CharPtr(), PATH_DELIMITER_EO01);
                // maybe the executable does not fit, exit
                fprintf(stderr, "possible version mismatch, call executable with this path:\n%s%cpgm%c%s\n",
                     dbRoot.CharPtr(), PATH_DELIMITER_EO01, PATH_DELIMITER_EO01, executableName==NULL?m_ExecutableName.CharPtr():executableName);
                return false;
            }
        }
        return true;
    }
}

bool DBMSrvShMAdm_Manager::unlockSharedMemory( DBMSrvMsg_Error& errOut ) {

    DBMSrvShM_Persistence persistence(m_ShMFolder.CharPtr(), m_DBName.CharPtr(), errOut);
    if( !errOut.IsEmpty() )
        return false;
    
    // check if the info file exists at all
    tsp05_RteFileInfo info_Info;
    tsp05_RteFileError err_Info;
    sqlfinfoc( persistence.getInfoFile(), &info_Info, &err_Info );
    bool errorFound(err_Info.sp5fe_result != vf_ok);
    if( errorFound ) {
        // report: error checking info file
        errOut.Overrule(DBMSrvMsg_RTEError(err_Info));
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_FILECHK,
            Msg_Arg(SDBMSGTAG_DBMSRV_FILECHK__FILENAME, persistence.getInfoFile())));
        return false;
    }

    if( !info_Info.sp5fi_exists ) {
        errOut.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_SHMNOINFOFILE));
        return false;
    }

    DBMSrvShM_BaseUnlocker* unlocker =
        DBMSrvShM_BaseUnlocker::getInstance(persistence.getInfoFile(), persistence.getDataFile(), errOut);
    
    bool okay(false);
    if( unlocker != NULL ) {
        okay = unlocker->isValid() && unlocker->forceUnlock(errOut);
        delete unlocker;
    }

    return okay;
}

bool DBMSrvShMAdm_Manager::deleteSharedMemory( DBMSrvMsg_Error& errOut ) {
    
    DBMSrvShM_Persistence persistence(m_ShMFolder.CharPtr(), m_DBName.CharPtr(), errOut);
    if( errOut.IsEmpty() )
        return persistence.remove(errOut);
    else
        return false;
}

bool DBMSrvShMAdm_Manager::checkSharedMemory(DBMSrvMsg_Error& errOut) {
    DBMSrvShM_Persistence persistence(m_ShMFolder.CharPtr(), m_DBName.CharPtr(), errOut);
    if( errOut.IsEmpty() ) {
        bool neverSeen(false);
        return persistence.exists(neverSeen, errOut);
    }
    else
        return false;
}
