/*!
  \file    DBMSrvCmd_BackupExtIds.cpp
  \author  MarcW
  \ingroup DBMServer commands
  \brief   definition of classes for handling DBMServer
           commands dealing with external backup ids

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

#include "hcn32.h"

#include "DBM/Srv/Commands/DBMSrvCmd_BackupExtIds.hpp"

DBMSrvCmd_BackupExtIdsGet::DBMSrvCmd_BackupExtIdsGet()
: DBMSrv_Command( DBMSrv_Command::KeyBackupExtIdsGet, false, LongHelp_GetEBIDS_cn32) {}

tcn00_Error DBMSrvCmd_BackupExtIdsGet::run(
    VControlDataT* vcontrol,
    CommandT* command,
    DBMSrv_Reply& theReply) {

    tcn00_Error retVal = cn32GetEBIDs(
                                    vcontrol,
                                    command,
                                    theReply.giveData(),
                                    theReply.giveLength(),
                                    theReply.giveMaximalLength());

    theReply.adaptLength();
    return retVal;
}

DBMSrvCmd_BackupExtIdsGetFromTool::DBMSrvCmd_BackupExtIdsGetFromTool()
: DBMSrv_Command( DBMSrv_Command::KeyBackupExtIdsGetFromTool, false, LongHelp_GetEBIDSFromTool_cn32) {}

tcn00_Error DBMSrvCmd_BackupExtIdsGetFromTool::run(
    VControlDataT* vcontrol,
    CommandT* command,
    DBMSrv_Reply& theReply) {

    tcn00_Error retVal = cn32GetEBIDsFromTool(
                            vcontrol,
                            command,
                            theReply.giveData(),
                            theReply.giveLength(),
                            theReply.giveMaximalLength());

    theReply.adaptLength();
    return retVal;
}
