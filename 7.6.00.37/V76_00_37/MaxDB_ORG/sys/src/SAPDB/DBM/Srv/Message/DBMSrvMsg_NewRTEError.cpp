/*!
    \file    DBMSrvMsg_NewRTEError.cpp
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
#include "DBM/Srv/Message/DBMSrvMsg_NewRTEError.hpp"

DBMSrvMsg_NewRTEError::DBMSrvMsg_NewRTEError(
    tsp01_RteError& rteError )
    : DBMSrvMsg_Error(SDBMSG_DBMSRV_RTEEXT) {

    char  rteErrString[RTE_ERROR_TEXT_MXSP01+1];
    char  osErrString[OS_ERROR_TEXT_MXSP01+1];
    
    strncpy(rteErrString, rteError.RteErrText, RTE_ERROR_TEXT_MXSP01);
    rteErrString[RTE_ERROR_TEXT_MXSP01] = '\0';
        
    strncpy(osErrString, rteError.OsErrText, OS_ERROR_TEXT_MXSP01);
    osErrString[OS_ERROR_TEXT_MXSP01] = '\0';

    DBMSrvMsg_Error transport;
    overruleWithTransportMessage(transport, SAPDB_ToString(rteError.OsErrCode), osErrString);
    overruleWithTransportMessage(transport, SAPDB_ToString(rteError.RteErrCode), rteErrString);

    transport.Overrule(*this);
    this->Assign(transport);
}


void DBMSrvMsg_NewRTEError::overruleWithTransportMessage(
        DBMSrvMsg_Error& outList, 
        const char* errID,
        const char* errText) {

    char textBuffer[1024];
    
    // write error ID and comma to buffer
    strncpy(textBuffer, errID, 1024);
    textBuffer[1024-1] = '\0';
    if( strlen(textBuffer) + strlen(DBMSrv_Reply::STRING_SEPARATOR_TOKEN) < 1024 ) {
        strcat(textBuffer, DBMSrv_Reply::STRING_SEPARATOR_TOKEN); // buffer is still 0-terminated
        int spaceLeft(1024-1-((int)strlen(textBuffer)));
        if( spaceLeft > 0 ) {
            strncat( textBuffer, errText, spaceLeft);
            textBuffer[1024-1] = '\0';
        }
    }
    
    outList.Overrule(
        DBMSrvMsg_Error(SDBMSG_DBMSRV_INTERNAL_TRANSPORT_MSG,
        Msg_Arg(SDBMSGTAG_DBMSRV_INTERNAL_TRANSPORT_MSG__TAG, textBuffer)));
}
