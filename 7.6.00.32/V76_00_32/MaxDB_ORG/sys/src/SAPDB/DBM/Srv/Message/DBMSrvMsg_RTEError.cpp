/*!
    \file    DBMSrvMsg_RTEError.cpp
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

#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"
#include "DBM/Srv/Message/Util/DBMSrvMsgUtil_RTEErrorText.hpp"

DBMSrvMsg_RTEError::DBMSrvMsg_RTEError(
    const tsp05_RteFileError& rteFileError )
    : DBMSrvMsg_Error(SDBMSG_DBMSRV_RTE) {

    appendTransportMessage(
        SAPDB_ToString(rteFileError.sp5fe_result),
        (const char*)(DBMSrvMsgUtil_RTEErrorText<40>(rteFileError.sp5fe_text)));
}

DBMSrvMsg_RTEError::DBMSrvMsg_RTEError(
    const tsp01_CommErr& commError,
    const tsp00_ErrText& commErrText )
    : DBMSrvMsg_Error(SDBMSG_DBMSRV_RTE) {

    appendTransportMessage(
        SAPDB_ToString(commError),
        (const char*)(DBMSrvMsgUtil_RTEErrorText<40>(commErrText)));
}

DBMSrvMsg_RTEError::DBMSrvMsg_RTEError(
    const tsp9_rte_xerror& xError )
    : DBMSrvMsg_Error(SDBMSG_DBMSRV_RTE) {

    appendTransportMessage(
        SAPDB_ToString(xError.xe_result),
        (const char*)(DBMSrvMsgUtil_RTEErrorText<200>(xError.xe_native_error, xError.xe_text)));
}

DBMSrvMsg_RTEError::DBMSrvMsg_RTEError(
    const tsp00_Bool& ok,
    const tsp00_ErrTextc& errtext )
    : DBMSrvMsg_Error(SDBMSG_DBMSRV_RTE) {

    appendTransportMessage(
      SAPDB_ToString(ok ? 0 : 1),
        (const char*)(DBMSrvMsgUtil_RTEErrorText<40>(errtext)));
}

DBMSrvMsg_RTEError::DBMSrvMsg_RTEError()
    : DBMSrvMsg_Error(SDBMSG_DBMSRV_RTE) {

    appendTransportMessage(
        "no_id_provided_by_rte",
        "no_text_provided_by_rte");
}

DBMSrvMsg_RTEError::DBMSrvMsg_RTEError(
    const Msg_List& rteMsgList )
    : DBMSrvMsg_Error( ) {

    this->Assign(rteMsgList);
    this->Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_RTE));
}
