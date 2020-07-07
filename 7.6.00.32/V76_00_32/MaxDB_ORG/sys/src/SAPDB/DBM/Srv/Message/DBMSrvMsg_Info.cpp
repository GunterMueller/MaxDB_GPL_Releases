/*!
  \file    DBMSrvMsg_Info.cpp
  \author  MarcW
  \brief   message info class

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


*/

#include "DBM/Srv/Message/DBMSrvMsg_Info.hpp"
#include "SAPDBCommon/Messages/SDBMsg_DBMSrv.h"

DBMSrvMsg_Info::DBMSrvMsg_Info()
  : DBMSrvMsg_Base(),
    Msg_List()
{
}

DBMSrvMsg_Info::DBMSrvMsg_Info(
        const char* aText,
        const char* const arg0,
        const char* const arg1,
        const char* const arg2,
        const char* const arg3,
        const char* const arg4,
        const char* const arg5,
        const char* const arg6,
        const char* const arg7,
        const char* const arg8,
        const char* const arg9)
        : DBMSrvMsg_Base(),
          Msg_List(
                "",
                __FILE__,  //we never use it anyway...
                __LINE__,  //we never use it anyway...
                Msg_List::Info,
                (MessageID) 0,
                aText,
                (arg0?1:0)+(arg1?1:0)+(arg2?1:0)+(arg3?1:0)+(arg4?1:0)+(arg5?1:0)+(arg6?1:0)+(arg7?1:0)+(arg8?1:0)+(arg9?1:0),
                arg0,
                arg1,
                arg2,
                arg3,
                arg4,
                arg5,
                arg6,
                arg7,
                arg8,
                arg9) {}

#define DBMSRVMSG_INFO_MSGLISTARGS \
    Msg_List::Info,\
        0==strcmp(component, SDBMSG_DBMSRV_COMPONENT)?((MessageID) (-25000 + aID)):aID,\
        component,\
        file,\
        line,\
        aText,\
        argCheck,\
        messageVersion,\
        arg0,\
        arg1,\
        arg2,\
        arg3,\
        arg4,\
        arg5,\
        arg6,\
        arg7,\
        arg8


DBMSrvMsg_Info::DBMSrvMsg_Info(
        const Msg_List    & subMessage,
        int                 aID,
        const char        * component,
        const char        * file,
        int                 line,
        const char        * aText,
        Msg_Has2Args      * argCheck,
        const Msg_IArg    * messageVersion,
        Msg_IArg const    * arg0,
        Msg_IOptArg const * arg1,
        Msg_IOptArg const * arg2,
        Msg_IOptArg const * arg3,
        Msg_IOptArg const * arg4,
        Msg_IOptArg const * arg5,
        Msg_IOptArg const * arg6,
        Msg_IOptArg const * arg7,
        Msg_IOptArg const * arg8)
  : DBMSrvMsg_Base(),
    Msg_List(subMessage)
{
    this->MarkAsSubMessages();
    this->Msg_List::Overrule(Msg_List(DBMSRVMSG_INFO_MSGLISTARGS));
}

DBMSrvMsg_Info::DBMSrvMsg_Info(
        const Msg_List    & subMessage,
        int                 aID,
        const char        * component,
        const char        * file,
        int                 line,
        const char        * aText,
        Msg_Has3Args      * argCheck,
        const Msg_IArg    * messageVersion,
        Msg_IArg const    * arg0,
        Msg_IArg const    * arg1,
        Msg_IOptArg const * arg2,
        Msg_IOptArg const * arg3,
        Msg_IOptArg const * arg4,
        Msg_IOptArg const * arg5,
        Msg_IOptArg const * arg6,
        Msg_IOptArg const * arg7,
        Msg_IOptArg const * arg8)
  : DBMSrvMsg_Base(),
    Msg_List(subMessage)
{
    this->MarkAsSubMessages();
    this->Msg_List::Overrule(Msg_List(DBMSRVMSG_INFO_MSGLISTARGS));
}

void DBMSrvMsg_Info::Overrule(const DBMSrvMsg_Info & info)
{
    Msg_List::Overrule(info);
}
