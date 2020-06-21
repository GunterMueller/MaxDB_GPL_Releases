/*!
    \file    DBMSrvMsg_SQLError.cpp
    \author  MarcW
    \ingroup error, warning and information message handling for DBM Server
    \brief   class for handling RTE errors
    
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

#include "DBM/Srv/DBMSrv_Reply.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_SQLError.hpp"

DBMSrvMsg_SQLError::DBMSrvMsg_SQLError(
    int sqlerrID, const char* sqlerrText )
    : DBMSrvMsg_Error(SDBMSG_DBMSRV_SQL,
      Msg_Arg(SDBMSGTAG_DBMSRV_SQL__RETURNCODE,
            sqlerrID),
      Msg_Arg(SDBMSGTAG_DBMSRV_SQL__ERRORTEXT,
            sqlerrText)) {

    appendTransportMessage( SAPDB_ToString(sqlerrID), sqlerrText);
}

DBMSrvMsg_SQLError::DBMSrvMsg_SQLError(
    const char* sqlerrID, const char* sqlerrText )
    : DBMSrvMsg_Error(SDBMSG_DBMSRV_SQL,
      Msg_Arg(SDBMSGTAG_DBMSRV_SQL__RETURNCODE,
            sqlerrID),
      Msg_Arg(SDBMSGTAG_DBMSRV_SQL__ERRORTEXT,
            sqlerrText)) {

    appendTransportMessage( sqlerrID, sqlerrText);
}
