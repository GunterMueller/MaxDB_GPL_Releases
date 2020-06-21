/*!
  \file    DBMSrvCmdMedium_MediumDelete.cpp
  \author  TiloH
  \ingroup DBMServer commands
  \brief   implementation of a classes handling DBMServer
           command medium_delete

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

#include "hcn30.h"

#include "SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumDelete.hpp"
#include "SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumDeleteNoDistribute.hpp"
#include "SAPDB/DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp"


const char * const DBMSrvCmdMedium_MediumDelete::m_LongHelp=
    "@command medium_delete You delete the specified backup medium."
    "@preconditions You have the DBM operator authorization Backup."
    "@syntax medium_delete <name>"
    "@param <name> "
        "The backup medium specified in <name> is deleted. The name of the medium may comprise a group name "
        "and a member name. These are separated by a slash (<name> ::= [<group_name>/]<member_name>). "
        "Groups of parallel backup media are used for parallel backups and restores. To delete a group of "
        "parallel backup media, execute the command medium_delete for each group member using the same <group_name>."
    "@reply OK";

DBMSrvCmdMedium_MediumDelete::DBMSrvCmdMedium_MediumDelete()
    :DBMSrv_Command(KeyMediumDelete, false, DBMSrvCmdMedium_MediumDelete::m_LongHelp)
{
}

tcn00_Error DBMSrvCmdMedium_MediumDelete::run(
    VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & theReply)
{
    DBMSrvCmdMedium_MediumDeleteNoDistribute commandObject;

    //classic medium_delete to local file dbm.mmm
    tcn00_Error rc=commandObject.run(vcontrol, command, theReply);
    
    //handling for HSS cluster (delete medium on all other HSS nodes too)
    if(OK_CN00==rc)
    {
        const char * mediumDeleteNoDistribCommand=commandObject.constructCommand(command->args);

        if(0==mediumDeleteNoDistribCommand)
            rc=theReply.startWithError(ERR_MEM_CN00);
        else
            vcontrol->pHSSNodes->ExecuteOnAllOtherNodes(mediumDeleteNoDistribCommand, theReply);
    }

    return rc;
}

tcn00_Error DBMSrvCmdMedium_MediumDelete::runCommand
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax)
{
    DBMSrv_Reply oReply(replyData, replyLen, replyLenMax);
    DBMSrvCmdMedium_MediumDelete commandObject;

    return commandObject.run(vcontrol, command, oReply);
}
