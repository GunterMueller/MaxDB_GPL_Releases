/*!
    \file    DBMSrvMsg_RTEError.hpp
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

#if !defined(DBMSrvMsg_RTEError_hpp)
#define DBMSrvMsg_RTEError_hpp

#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

#include "gsp01.h"
#include "gsp05.h"
#include "gsp09.h"

/*!
    @brief error message containing RTE errors
*/
class DBMSrvMsg_RTEError: public DBMSrvMsg_Error {

public:
    /*! @brief for a file error */
    DBMSrvMsg_RTEError(const tsp05_RteFileError& rteFileError );

    /*! @brief for a communication error */
    DBMSrvMsg_RTEError(const tsp01_CommErr& commError, const tsp00_ErrText& commErrText );

    /*! @brief for a, well, xerror... */
    DBMSrvMsg_RTEError(const tsp9_rte_xerror& xError );

    /*! @brief for a error with tsp00_ErrTextc and tsp00_Bool */
    DBMSrvMsg_RTEError(const tsp00_Bool& ok, const tsp00_ErrTextc& errtext);

    /*! @brief we know something went wrong, but RTE did not tell us anything */
    DBMSrvMsg_RTEError();

    /*!
        @brief for errors when calling a RTE method that returns a message list.
        
        In the far future, when all RTE methods return a Msg_List,
        this should be the only constructor...
    */
    DBMSrvMsg_RTEError( const Msg_List& rteMsgList );

    virtual ~DBMSrvMsg_RTEError() {}
};

#endif //DBMSrvMsg_RTEError_hpp
