/*!
    \file    DBMSrvMsg_SQLError.hpp
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

#if !defined(DBMSrvMsg_SQLError_hpp)
#define DBMSrvMsg_SQLError_hpp

#include "DBM/Srv/Message/DBMSrvMsg_RTEError.hpp"

/*! @brief for friend declaration, see below */
class DBMSrvInfrReply_Reply2MsgList;

/*!
    @brief error message containing RTE errors
*/
class DBMSrvMsg_SQLError: public DBMSrvMsg_Error {

public:
    /*! @brief the only constructor */
    DBMSrvMsg_SQLError( int sqlerrID, const char* sqlerrText );

    virtual ~DBMSrvMsg_SQLError() {}

private:
    DBMSrvMsg_SQLError( const char* sqlerrID, const char* sqlerrText );
    friend class DBMSrvInfrReply_Reply2MsgList; // enable access to private constructor!
};

#endif //DBMSrvMsg_SQLError_hpp
