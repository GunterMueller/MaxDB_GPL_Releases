/*!
  \file    DBMSrvCmdMedium_MediumPutNoDistribute.cpp
  \author  TiloH
  \ingroup DBMServer commands
  \brief   implementation a classes handling the internal DBMServer
           command for writing a medium in dbm.mmm (medium_put without HSS distribution)

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
#include "hcn36.h"

#include "SAPDB/DBM/Srv/Commands/Medium/DBMSrvCmdMedium_MediumPutNoDistribute.hpp"

DBMSrvCmdMedium_MediumPutNoDistribute::DBMSrvCmdMedium_MediumPutNoDistribute()
  :DBMSrv_Command(KeyMediumPutNoDistribute, false, 0, true), //a non obsolete, but hidden command without syntax and explain text
   commandBuffer(0)
{
}

DBMSrvCmdMedium_MediumPutNoDistribute::~DBMSrvCmdMedium_MediumPutNoDistribute()
{
    cn36_StrDealloc(commandBuffer);
}

tcn00_Error DBMSrvCmdMedium_MediumPutNoDistribute::run(
    VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & theReply)
{
    //just do a classical medium_put
    tcn00_Error rc=cn30BackupMediaPut(vcontrol, command, theReply.giveData(), theReply.giveLength(), theReply.giveMaximalLength());

    return rc;
}

tcn00_Error DBMSrvCmdMedium_MediumPutNoDistribute::runCommand
    ( VControlDataT * vcontrol,
      CommandT      * command,
      char          * replyData,
      int           * replyLen,
      int             replyLenMax)
{
    DBMSrv_Reply oReply(replyData, replyLen, replyLenMax);
    DBMSrvCmdMedium_MediumPutNoDistribute commandObject;

    return commandObject.run(vcontrol, command, oReply);
}

const char * DBMSrvCmdMedium_MediumPutNoDistribute::constructCommand(const char * argumentString)
{
    if(cn36_StrAlloc(commandBuffer, strlen("medium_put_nodistribute ")+strlen(argumentString)))
    {
        SAPDB_strcpy(commandBuffer, "medium_put_nodistribute ");
        SAPDB_strcat(commandBuffer, argumentString);
    }

    return commandBuffer;
}
