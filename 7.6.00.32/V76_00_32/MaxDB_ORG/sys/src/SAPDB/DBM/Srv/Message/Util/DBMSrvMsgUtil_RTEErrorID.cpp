/*!
    \file    DBMSrvMsgUtil_RTEErrorID.cpp
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

#include "DBM/Srv/Message/Util/DBMSrvMsgUtil_RTEErrorID.hpp"

DBMSrvMsgUtil_RTEErrorID::DBMSrvMsgUtil_RTEErrorID( Msg_List& errList )
    : m_ReturnString(NOMEM) {

    if( errList.IsEmpty() )
        m_ReturnString = NOID;
    else {
        DBMSrvMsg_Error internalError;
        if( !ToolsCStr_Str::AllocCpy(m_Buffer, SAPDB_ToString(errList.ID()), internalError) )
            m_ReturnString = NOMEM;
        else {
            m_ReturnString = BUFFER;
        }
    }
}

DBMSrvMsgUtil_RTEErrorID::~DBMSrvMsgUtil_RTEErrorID() {
    if( m_ReturnString == BUFFER )
        ToolsCStr_Str::Dealloc(m_Buffer);
}

DBMSrvMsgUtil_RTEErrorID::operator const char* () const {
    switch( m_ReturnString ) {
        case NOMEM:
            return "no_memory_for_rteerrorid";
            break;
        case NOID:
            return "no_rteerrorid";
            break;
        case BUFFER:
            return m_Buffer;
            break;
        default:
            return "impossible";
    }
}
