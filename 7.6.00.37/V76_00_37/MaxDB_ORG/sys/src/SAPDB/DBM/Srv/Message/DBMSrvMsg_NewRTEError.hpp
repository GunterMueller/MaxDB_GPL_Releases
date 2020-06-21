/*!
    \file    DBMSrvMsg_NewRTEError.hpp
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

#if !defined(DBMSrvMsg_NewRTEError_hpp)
#define DBMSrvMsg_NewRTEError_hpp

#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

#include "gsp01.h"

/*! @brief for friend declaration, see below */
//class DBMSrvInfrReply_Reply2MsgList;

/*!
    @brief error message containing new RTE errors
*/
class DBMSrvMsg_NewRTEError: public DBMSrvMsg_Error {

public:
    /*! @brief for a new RTE error */
    DBMSrvMsg_NewRTEError( tsp01_RteError& rteError );

    virtual ~DBMSrvMsg_NewRTEError() {}

private:
    void overruleWithTransportMessage( DBMSrvMsg_Error& outList, const char* errID, const char* errText);
};

#endif //DBMSrvMsg_NewRTEError_hpp
