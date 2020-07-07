/*!
  \file    DBMSrvMsg_ErrorHandler.hpp
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

#if !defined(DBMSrvMsg_ErrorHandler_hpp)
#define DBMSrvMsg_ErrorHandler_hpp

#include "hni35.h"
#include "Messages/Msg_List.hpp"

class DBMSrvMsg_ErrorHandler: public tni35_AErrorHandler {
public:
    DBMSrvMsg_ErrorHandler();
    virtual ~DBMSrvMsg_ErrorHandler() {}

    void reportString(const char * string);

    bool getLastError( Msg_List& aOutList );
    
    void activate();
    void deactivate();

private:
    tni35_AErrorHandler* m_OldHandler;
    static Msg_List m_TheList;

};

#endif //DBMSrvMsg_ErrorHandler_hpp
