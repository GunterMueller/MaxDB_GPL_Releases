/*!
  \file    DBMSrvScd_ErrorHandler.cpp
  \author  MarcW
  \brief   handler for errors from the historic ni modules (file and pipe objects from there)

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

#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_ErrorHandler.hpp"
#include "SAPDBCommon/SAPDB_string.h"

Msg_List DBMSrvMsg_ErrorHandler::m_TheList;

void DBMSrvMsg_ErrorHandler::deactivate() {
    tni35_AErrorHandler::SetNewErrorHandler(m_OldHandler);
}

void DBMSrvMsg_ErrorHandler::activate() {
    m_OldHandler = SetAsCurrentErrorHandler();
}

bool DBMSrvMsg_ErrorHandler::getLastError( Msg_List& aOutList ) {
    if( m_TheList.IsEmpty() )
        return false;
    else {
        aOutList.Overrule(m_TheList);
        return true;
    }
}

DBMSrvMsg_ErrorHandler::DBMSrvMsg_ErrorHandler()
    : tni35_AErrorHandler(),
      m_OldHandler(NULL) {}

void DBMSrvMsg_ErrorHandler::reportString(const char * string)
{
    const size_t msgLen=strlen(string);

    char * outMessage=new char[msgLen+1];

    if( 0==outMessage )
        m_TheList = DBMSrvMsg_Error(SDBMSG_DBMSRV_NIERROR, Msg_Arg("DBMSRV_COMP", string));
    else
    {
        SAPDB_strcpy(outMessage, string);

        for(size_t i=0; i<=msgLen; ++i)
            if( outMessage[i]=='\n' )
                outMessage[i]=' ';

        m_TheList = DBMSrvMsg_Error(SDBMSG_DBMSRV_NIERROR, Msg_Arg("DBMSRV_COMP", outMessage));

        delete [] outMessage;
    }
}
