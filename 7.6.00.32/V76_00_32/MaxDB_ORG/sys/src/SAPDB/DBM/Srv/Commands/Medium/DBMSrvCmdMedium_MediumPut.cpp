/*!
  \file    DBMSrvCmdMedium_MediumPut.cpp
  \author  TiloH
  \ingroup DBMServer commands
  \brief   implementation of a classes handling DBMServer
           command medium_put

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

#include "SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumPut.hpp"
#include "SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumPutNoDistribute.hpp"
#include "SAPDB/DBM/Srv/HSS/DBMSrvHSS_Nodes.hpp"


const char * const DBMSrvCmdMedium_MediumPut::m_LongHelp=
    "@command medium_put You create a new backup medium or you change a backup medium that already exists."
    "@preconditions You have the DBM operator authorization Backup."
    "@syntax medium_put <name> <location> <type> <backup_type> <size> <block_size> <overwrite> <autoloader> <os_command> <tool_type>"
    "@param <name> "
        "The backup medium specified in <name> is created or updated. The name of the medium may comprise a group name "
        "and a member name. These are separated by a slash (<name> ::= [<group_name>/]<member_name>). "
        "Groups of parallel backup media are used for parallel backups and restores. To create a group of "
        "parallel backup media, execute the command medium_put for each group member using the same <group_name>."
    "@param <location> "
        "The name of the backup device or file with which it can be addressed through operating system functions. "
        "If a log backup into a file is created a running number is added to the <location>."
    "@param <type> "
        "Type of the backup medium. Possible values are:"
        "@value AUTO Auto The media with the type AUTO has implicitly the type FILE (see below) and is the default medium "
            "for the command autolog_on. "
            "There can only be one backup medium of this type. If there is already a backup medium of type AUTO, and you create a backup "
            "medium of type AUTO again, the system changes the media type of the existing backup medium to FILE."
        "@value TAPE Tape The specified device is used as non rewinding tape."
        "@value FILE File The backup will be written to a file. "
        "@value PIPE Pipe The backup will be written to a pipe. On Microsoft Windows, the name of the pipe must "
            "have the following form: \\\\.\\pipe\\<pipe_name>. There are no special naming conventions on UNIX."
    "@param <backup_type> "
        "Backup type, for which the backup medium is to be used. Possible values are:"
        "@value DATA  \"complete data backup\" A complete data backup."
        "@value PAGES \"differential data backup\" A differential data backup containing the differences from the last complete data backup"
        "@value LOG   \"log backup\" A backup of log information from the log volume."
    "@param <size> "
        "Maximum number of pages that can be written to the backup medium. This is "
        "necessary to change a tape correctly, for example. If the backup media is "
        "of sufficient size, a zero can be specified."
    "@param <block_size> "
        "Number of pages that are transferred when accessing the medium once."
        "@default 8"
    "@param <overwrite> "
        "This option is only relevant for the medium type FILE and describes the behavior if the file is "
        "already present. Permitted values are:"
        "@value NO \"\" If the file already exists, the backup will fail."
        "@value YES \"\" If the file already exists, it will be overwritten with the new backup."
    "@param <autoloader> "
        "Specifies whether the tape device changes the tapes automatically. This option has an effect "
        "only on Microsoft Windows. Possible values are:"
        "@value YES \"\" The devices changes the tapes automatically."
        "@value NO  \"\" The devices does not change the tapes automatically."
    "@param <os_command> "
        "Operating system command to be executed before backing up to a succeeding medium or restoring from a succeeding medium."
    "@param <tool_type> The backup tool that should be used. If another value as NONE is specified, use PIPE only as the <type> and 0 as "
        "<size>. Whenever such a medium is used, the pipe must not already exist. Possible values for <tool_type> are:"
        "@value NONE \"None\"                   No external backup tool is used with the backup medium."
        "@value BACK \"Backint for MaxDB\"      The Backint for MaxDB interface of an external backup tool is used with the backup medium."
        "@value NSR  \"NetWorker\"              Legato's NetWorker is used with the backup medium."
        "@value TSM  \"Tivoli Storage Manager\" IBM's adint2 client for Tivoli Storage Manager is used with the backup medium."
    "@reply OK";

DBMSrvCmdMedium_MediumPut::DBMSrvCmdMedium_MediumPut()
    :DBMSrv_Command(KeyMediumPut, false, DBMSrvCmdMedium_MediumPut::m_LongHelp)
{
}

tcn00_Error DBMSrvCmdMedium_MediumPut::run(
    VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & theReply)
{
    DBMSrvCmdMedium_MediumPutNoDistribute commandObject;

    //classical medium_put (adding medium to local file dbm.mmm)
    tcn00_Error rc=commandObject.run(vcontrol, command, theReply);

    //handling for HSS cluster (adding the medium to all other nodes of the HSS)
    if(OK_CN00==rc)
    {
        const char * mediumPutNoDistribCommand=commandObject.constructCommand(command->args);

        if(0==mediumPutNoDistribCommand)
            rc=theReply.startWithError(ERR_MEM_CN00);
        else
            vcontrol->pHSSNodes->ExecuteOnAllOtherNodes(mediumPutNoDistribCommand, theReply);
    }

    return rc;
}

tcn00_Error DBMSrvCmdMedium_MediumPut::runCommand
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax)
{
    DBMSrv_Reply oReply(replyData, replyLen, replyLenMax);
    DBMSrvCmdMedium_MediumPut commandObject;

    return commandObject.run(vcontrol, command, oReply);
}
