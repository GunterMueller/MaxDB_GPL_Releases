/*!
  \file    DBMSrvCmdUsr_UserRename.cpp
  \author  BerndV
  \ingroup DBMServer commands
  \brief   definition of class handling DBMServer command user_rename

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2002-2004 SAP AG

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

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include "hcn50.h"

#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

#include "DBM/Srv/Commands/User/DBMSrvCmdUsr_UserRename.hpp"

//-----------------------------------------------------------------------------
// members of class DBMSrvCmdUsr_UserRename
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Constructor DBMSrvCmdUsr_UserRename::DBMSrvCmdUsr_UserRename
//-----------------------------------------------------------------------------
DBMSrvCmdUsr_UserRename::DBMSrvCmdUsr_UserRename()
    : DBMSrv_Command( DBMSrv_Command::KeyUsingCommandFactory, false, DBMSrvCmdUsr_UserRename::m_LongHelp ) 
{
} // end DBMSrvCmdUsr_UserRename::DBMSrvCmdUsr_UserRename

//-----------------------------------------------------------------------------
// static member variable DBMSrvCmdUsr_UserRename::m_LongHelp
//-----------------------------------------------------------------------------
#define DBMSRV_CMD_UserRename         "user_rename"

const char * const DBMSrvCmdUsr_UserRename::m_LongHelp=
"@command "DBMSRV_CMD_UserRename" This command renames a DBM operator."
"@preconditions You have the server authorization UserMgm."
"@syntax "DBMSRV_CMD_UserRename" <username> <new username>"
"@param  <username>  The name of the DBM operator which should be renamed."
"@param  <new username>  The new name for the DBM operator."
"@reply OK<NL>";

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdUsr_UserRename::getCommandName
//-----------------------------------------------------------------------------
const char * DBMSrvCmdUsr_UserRename::getCommandName
    ( )
{
  return DBMSRV_CMD_UserRename;
} // end DBMSrvCmdUsr_UserRename::getCommandName

//-----------------------------------------------------------------------------
// public member function DBMSrvCmdUsr_UserRename::run
//-----------------------------------------------------------------------------
tcn00_Error DBMSrvCmdUsr_UserRename::run
  ( VControlDataT * vcontrol,
    CommandT      * command,
    DBMSrv_Reply  & Reply ) 
{
  tcn00_Error nFuncReturn = OK_CN00;

    // read user 
  Tools_DynamicUTF8String szUser;
  Tools_DynamicUTF8String szNewUser;
  szUser     = command->oArguments.getValue(1);
  szNewUser  = command->oArguments.getValue(2);

  if ((szUser.Length() == 0) || (szNewUser.Length() == 0)) {
    return Reply.startWithMessageList(DBMSrvMsg_Error(SDBMSG_DBMSRV_PARAM));
  } // end if

  cn50DBMUser oUser(vcontrol->dbname, szUser.CharPtr());

  if (!oUser.lastEvent().IsEmpty()) {
    return Reply.startWithMessageList(oUser.lastEvent());
  } // end if

  oUser.setUserName(szNewUser.CharPtr());

  if (oUser.save() != OK_CN00) {
    return Reply.startWithMessageList(oUser.lastEvent());
  } // end if

  cn50DBMUser oUserForDelete(vcontrol->dbname, szUser.CharPtr());
  oUserForDelete.setColdUser(false);
  oUserForDelete.setSDBAUser(false);
  if (oUserForDelete.deleteUsr() != OK_CN00) {
    return Reply.startWithMessageList(oUser.lastEvent());
  } // end if

  Reply.startWithOK();
  return OK_CN00;
} // end DBMSrvCmdUsr_UserRename::run
