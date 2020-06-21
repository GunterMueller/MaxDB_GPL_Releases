/*!
    \file    DBMSrvInfrCmd_ClassicCommand.hpp
    \author  TiloH
    \ingroup infrastructure for DBM Server commands
    \brief   declaration of a class template for usage of classical vcn10.cpp
             functions

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

#if !defined(DBMSRVINFRCMD_CLASSICCOMMAND_HPP)
#define DBMSRVINFRCMD_CLASSICCOMMAND_HPP

#include "gcn00.h"
#include "DBM/Srv/DBMSrv_Command.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"


/*!
    @brief a class template for DBMServer commands, that are implemented by
    prehistoric command functions from vcn10.cpp

    This template provides a mechanism to bind a classic function from
    vcn10.cpp to a new command class, it also supplies a static function
    for creating a command object

    \TODO check, if this template class should be eliminated over time, to
    increase readability
*/

class DBMSrvInfrCmd_ClassicCommand: public DBMSrv_Command
{
  public:
    /*! constructor
        \param classicFunction [IN] pointer to classic vnc10 function that should be implemented by the resulting object
        \param longHelp        [IN] pointer to explain text string */
    DBMSrvInfrCmd_ClassicCommand(cnFunctionPrototype_CN00 * classicFunction, const char * longHelp=0)
        :DBMSrv_Command(false),
        m_vcn10Function(classicFunction)
    {
        m_longHelp=longHelp;
    }

    /*! function implementing the DBM Server command
    
        this function calls the classic vcn10 function that is handled by the
        object \see DBMSrvInfrCmd_ClassicCommand() and adapts the length in the reply object
        \param vcontrol [IN/OUT] pointer to global control structure
        \param command  [IN]     information about the actually called command and its parameters
        \param reply    [OUT]    the reply buffer containing the answer of the DBM Server command */
    virtual tcn00_Error run(
        VControlDataT * vcontrol,
        CommandT      * command,
        DBMSrv_Reply  & reply)
    {
        tcn00_Error rc=m_vcn10Function(vcontrol, command, reply.giveData(), reply.giveLength(), reply.giveMaximalLength());

        if(!reply.adaptLength()) //report an error, if maximal length of reply was not honored (maybe it is reaching the client or a log file)
            rc=reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_ERR));

        return rc;
    }

  private:
    cnFunctionPrototype_CN00 * m_vcn10Function; //!< pointer to the classic vcn10 function, that is used during run()
};

#endif
