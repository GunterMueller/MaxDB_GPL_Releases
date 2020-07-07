/*!
    \file    DBMSrvInfrCmd_CommandFactory.hpp
    \author  TiloH
    \ingroup infrastructure for DBM Server commands
    \brief   definition of a class for generating command objects

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


*/

#if !defined(DBMSRVINFRCMD_COMMANDFACTORY_HPP)
#define DBMSRVINFRCMD_COMMANDFACTORY_HPP


#include "ToolsCommon/Tools_List.hpp"
#include "DBM/Srv/DBMSrv_Command.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"


/*!
    @brief a class for generating DBMServer command objects

    This class has to know all command classes, therefore any new command
    class that should become effective must be inserted into the definition
    of the member function addMatchingCommands() of this class
*/

class DBMSrvInfrCmd_CommandFactory
{
  public:
    /*! add all known commands matching a certain command name prefix to a list

        The list is not emptied by the function and the command objects pointed
        to by the list elements must be deleted using delete, when they are no
        longer needed.

        If you implement a new command class insert it in the implementation of
        this function.

        \param commandNamePrefix [IN] the command name prefix (can be "")
        \param result            [OUT] the list containing the result
        \return false, if needed memory could not be allocated, true otherwise */
    static bool addMatchingCommands(const char * commandNamePrefix, Tools_List<DBMSrv_Command *> & result);

    /*! create a command object for a certain command name
        
        The returned command object is created using new and must be deleted,
        when it is no longer needed

        \param commandName [IN]  name of the DBM command we are looking for
        \param msgList     [OUT] error encountered during function 
        \return 0 in case of error, pointer to the command object otherwise */
    static DBMSrv_Command * getCommand(const char * commandName, DBMSrvMsg_Error & msgList);

    /*! function replacing DBMSrv_Command::getCommand()

        This function eases the transition from command keys to the
        command factory. It creates  a command object using a command name and
        a command key. Key can also be DBMSrv_Command::KeyUsingCommandFactory.
        The resulting object must be deleted, when the object is no longer
        needed.

        \TODO remove this function as soon, as DBMSrv_Command::CommandKey
        can be removed and substitute it by
        DBMSrvInfrCmd_CommandFactory::getCommand(const char *, DBMSrvMsg_Error &)

        \param commandName [IN] name of the command
        \param commandKey  [IN] the key of the command
        \param commandObject [OUT] pointer to new object, 0 in case of errors
        \param reply       [OUT] error output
        \param msgList     [OUT] the reply in form of a msgList 
        \return OK_CN00 if object was constructed, something else otherwise*/
    static tcn00_Error getCommand(
        const char                  * commandName,
        DBMSrv_Command::CommandKey    commandKey,
        DBMSrv_Command             *& commandObject,
        DBMSrv_Reply                & reply,
        DBMSrvMsg_Error             & msgList);

  private:
    static void prepareForAddCommandObjects(
        const char                   * commandNamePrefix,
        bool                         & memoryError,
        Tools_List<DBMSrv_Command *> & resultList);

    template <class T> inline static void addCommandObject();

    static const char                   * m_commandNamePrefix;
    static bool                         * m_memoryError;
    static Tools_List<DBMSrv_Command *> * m_resultList;
};

#endif
