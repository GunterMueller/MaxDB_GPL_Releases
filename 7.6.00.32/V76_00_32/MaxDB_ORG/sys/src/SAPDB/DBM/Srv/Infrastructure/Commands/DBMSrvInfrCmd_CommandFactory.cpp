/*!
    \file    DBMSrvInfrCmd_CommandFactory.cpp
    \author  TiloH
    \ingroup infrastructure for DBM Server commands
    \brief   class member definitions of a class for generating command objects

             command classes, that should be handeled by the command factory
             must be inserted in DBMSrvInfrCmd_CommandFactory::addMatchingCommands

    \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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

#include "DBM/Srv/Commands/DBMSrvCmd_Headers.hpp"
#include "DBM/Srv/Commands/Auto/DBMSrvCmdAuto_Headers.hpp"
#include "DBM/Srv/Commands/Backup/DBMSrvCmdB_Headers.hpp"
#include "DBM/Srv/Commands/Db/DBMSrvCmdDb_Headers.hpp"
#include "DBM/Srv/Commands/Dbm/DBMSrvCmdDbm_Headers.hpp"
#include "DBM/Srv/Commands/Dbmevtdisp/Internal/DBMSrvCmdEvtdispInt_Headers.hpp"
#include "DBM/Srv/Commands/Medium/DBMSrvCmdMedium_Headers.hpp"
#include "DBM/Srv/Commands/SQL/DBMSrvCmdSQL_Headers.hpp"
#include "DBM/Srv/Commands/User/DBMSrvCmdUsr_Headers.hpp"

#include "DBM/Srv/Infrastructure/Commands/DBMSrvInfrCmd_CommandFactory.hpp"
#include "ToolsCommon/Tools_List.hpp"

const char                   * DBMSrvInfrCmd_CommandFactory::m_commandNamePrefix=0;
bool                         * DBMSrvInfrCmd_CommandFactory::m_memoryError=0;
Tools_List<DBMSrv_Command *> * DBMSrvInfrCmd_CommandFactory::m_resultList=0;

template <class T> inline void DBMSrvInfrCmd_CommandFactory::addCommandObject()
{
    DBMSrv_Command *rc=0;

    if(0==stricmp(DBMSrvInfrCmd_CommandFactory::m_commandNamePrefix, T::getCommandName()))
    {
        rc=new T;

        if(0==rc || !DBMSrvInfrCmd_CommandFactory::m_resultList->push_back(rc))
            (*DBMSrvInfrCmd_CommandFactory::m_memoryError)=true;
    }
}

void DBMSrvInfrCmd_CommandFactory::prepareForAddCommandObjects(
        const char                   * commandNamePrefix,
        bool                         & memoryError,
        Tools_List<DBMSrv_Command *> & resultList)
{
    m_commandNamePrefix=commandNamePrefix;
    m_memoryError=&memoryError;
    m_resultList=&resultList;
}

bool DBMSrvInfrCmd_CommandFactory::addMatchingCommands(const char * commandName, Tools_List<DBMSrv_Command *> & resultList)
{
    bool memoryAllocationError=false;

    prepareForAddCommandObjects(commandName, memoryAllocationError, resultList); //store parameters in member variables, to shorten the following "addCommandObject<...>();" lines

    /* insert new command classes below */
    addCommandObject<DBMSrvCmd_Bye>();
    addCommandObject<DBMSrvCmd_Exit>();
    addCommandObject<DBMSrvCmd_Quit>();
    addCommandObject<DBMSrvCmd_Release>();

    addCommandObject<DBMSrvCmdAuto_AutoRecover>();

    addCommandObject<DBMSrvCmdB_BackupLabelOffline>();  //obsolete version of medium_labeloffline

    addCommandObject<DBMSrvCmdDb_DbAdmin>();
    addCommandObject<DBMSrvCmdDb_DbAddVolume>();
    addCommandObject<DBMSrvCmdDb_DbCold>(); // obsolete version of db_admin
    addCommandObject<DBMSrvCmdDb_Dbcold>(); // obsolete version of db_admin
    addCommandObject<DBMSrvCmdDb_DbConnect>();
    addCommandObject<DBMSrvCmdDb_DBDeleteVolume>();
    addCommandObject<DBMSrvCmdDb_DbDrop>();
    addCommandObject<DBMSrvCmdDb_DbExecute>();
    addCommandObject<DBMSrvCmdDb_DbFetch>();
    addCommandObject<DBMSrvCmdDb_DbExecuteNice>();
    addCommandObject<DBMSrvCmdDb_DbFetchNice>();
    addCommandObject<DBMSrvCmdDb_DbOffline>();
    addCommandObject<DBMSrvCmdDb_Dboffline>(); //obsolete version if db_offline
    addCommandObject<DBMSrvCmdDb_DbOnline>();
    addCommandObject<DBMSrvCmdDb_DbReg>();
    addCommandObject<DBMSrvCmdDb_DbRegister>();  //obsolete version of db_reg
    addCommandObject<DBMSrvCmdDb_DbRelease>();
    addCommandObject<DBMSrvCmdDb_DbUnreg>();
    addCommandObject<DBMSrvCmdDb_DbWarm>(); //obsolete version of db_online
    addCommandObject<DBMSrvCmdDb_Dbwarm>(); //obsolete version of db_online

    addCommandObject<DBMSrvCmdDbm_DbmConfigGet>();
    addCommandObject<DBMSrvCmdDbm_DbmConfigSet>();
    addCommandObject<DBMSrvCmdDbm_DbmGetmemoryusage>();

    addCommandObject<DBMSrvCmdEvtdispInt_DbmevtdispInternalAdd>();
    addCommandObject<DBMSrvCmdEvtdispInt_DbmevtdispInternalCommunication>();
    addCommandObject<DBMSrvCmdEvtdispInt_DbmevtdispInternalDelete>();

    addCommandObject<DBMSrvCmdMedium_MediumLabelOffline>();

    addCommandObject<DBMSrvCmdSQL_SqlConnect>();
    addCommandObject<DBMSrvCmdSQL_SqlExecute>();
    addCommandObject<DBMSrvCmdSQL_SqlFetch>();
    addCommandObject<DBMSrvCmdSQL_SqlExecuteNice>();
    addCommandObject<DBMSrvCmdSQL_SqlFetchNice>();
    addCommandObject<DBMSrvCmdSQL_SqlInfo>();
    addCommandObject<DBMSrvCmdSQL_SqlRelease>();
    addCommandObject<DBMSrvCmdSQL_SqlUpdatestat>();
    addCommandObject<DBMSrvCmdSQL_SqlUpdatestatPerSystemtable>();

    addCommandObject<DBMSrvCmdUsr_UserGetChallenge>();
    addCommandObject<DBMSrvCmdUsr_UserRename>();
    addCommandObject<DBMSrvCmdUsr_UserResponse>();
    /* insert new command classes above */

    return !memoryAllocationError;
}

DBMSrv_Command * DBMSrvInfrCmd_CommandFactory::getCommand(const char * commandName, DBMSrvMsg_Error & msgList)
{
    DBMSrv_Command               * rc=0;
    Tools_List<DBMSrv_Command *>   commandList;

    bool hadMemory=DBMSrvInfrCmd_CommandFactory::addMatchingCommands(commandName, commandList);

    if(hadMemory)
    {
        if(1!=commandList.size())
        {
            msgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_NOTIMPL, Msg_Arg("COMMAND_NAME", commandName)));

            //free all command objects managed by the list
            for(Tools_List<DBMSrv_Command *>::iterator i=commandList.begin(); i!=commandList.end(); i++)
            {
                if(0!=(*i))
                    delete (*i);

                (*i)=0;
            }
        }
        else //we have exactly one element in the list ...
            rc=commandList.front(); //...so we return the command object, commandList contains only a pointer to that object and destroys itself
    }
    else
        msgList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));

    return rc;
}

tcn00_Error DBMSrvInfrCmd_CommandFactory::getCommand(
        const char                  * commandName,
        DBMSrv_Command::CommandKey    commandKey,
        DBMSrv_Command             *& commandObject,
        DBMSrv_Reply                & reply,
        DBMSrvMsg_Error             & msgList)
{
    tcn00_Error rc=0;

    // take syntax from command object
    if( DBMSrv_Command::KeyUsingCommandFactory == commandKey ) //it is a new command
    {   
        commandObject=DBMSrvInfrCmd_CommandFactory::getCommand(commandName, msgList);

        if(0==commandObject)
            rc=reply.startWithMessageList(msgList);
    }
    else //it is a command using command keys
    {
        commandObject=DBMSrv_Command::getCommand(commandKey, commandName, reply);

        if(0==commandObject)
            rc=ERR_CN00; //correct error message is already in the reply, we set ERR_CN00 to finish the loop
    }

    return rc;
}
