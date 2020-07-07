/*!
    \file    DBMSrvMsgUtil_RTEErrorID.hpp
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

#if !defined(DBMSrvMsgUtil_RTEErrorID_hpp)
#define DBMSrvMsgUtil_RTEErrorID_hpp

#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"
#include "ToolsCommon/CString/ToolsCStr_Str.hpp"

class DBMSrvMsgUtil_RTEErrorID {
public:
    DBMSrvMsgUtil_RTEErrorID( Msg_List& errList );

    ~DBMSrvMsgUtil_RTEErrorID();

    operator const char* () const;

private:
    enum ReturnString {
        NOMEM,
        NOID,
        BUFFER
    };

    ReturnString m_ReturnString;
    char* m_Buffer;
};

#endif //DBMSrvMsgUtil_RTEErrorID_hpp
