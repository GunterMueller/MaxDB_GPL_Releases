/*!
  \file    DBMSrvMsg_Warning.cpp
  \author  MarcW
  \brief   message warning class

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

#include "DBM/Srv/Message/DBMSrvMsg_Warning.hpp"

DBMSrvMsg_Warning::DBMSrvMsg_Warning(
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
                Msg_List::Warning,
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


