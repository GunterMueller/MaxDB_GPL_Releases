/*!
    \file    DBMSrvMsg_Warning.hpp
    \author  MarcW
    \brief   message warning class

    \if EMIT_LICENCE

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

    \endif
*/

#include "Messages/Msg_List.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Base.hpp"

#if !defined(DBMSRVMSG_WARNING_HPP)
#define DBMSRVMSG_WARNING_HPP

/*!
    @brief warning message
*/
class DBMSrvMsg_Warning : public Msg_List, public DBMSrvMsg_Base {
public:
    /*!
        @brief constructor

        Takes a string that is handeled as a format string (like printf), the number of
        arguments used and up to 10 string arguments.
        @param aText [IN] format string
        @param arg0 [IN] string argument 0
        @param arg1 [IN] string argument 1
        @param arg2 [IN] string argument 2
        @param arg3 [IN] string argument 3
        @param arg4 [IN] string argument 4
        @param arg5 [IN] string argument 5
        @param arg6 [IN] string argument 6
        @param arg7 [IN] string argument 7
        @param arg8 [IN] string argument 8
        @param arg9 [IN] string argument 9
    */
    DBMSrvMsg_Warning(
        const char* aText,
        const char* const arg0 = NULL,
        const char* const arg1 = NULL,
        const char* const arg2 = NULL,
        const char* const arg3 = NULL,
        const char* const arg4 = NULL,
        const char* const arg5 = NULL,
        const char* const arg6 = NULL,
        const char* const arg7 = NULL,
        const char* const arg8 = NULL,
        const char* const arg9 = NULL);

    /*! @brief copy constructor */
    DBMSrvMsg_Warning( DBMSrvMsg_Warning const &aWarning )
         : Msg_List(aWarning) {}

    /*! @brief empty destructor */
    virtual ~DBMSrvMsg_Warning() {}

private:
    /*! \brief operator for easy addition of errors
 
        As Msg_List changed the behaviour of its operator+ more than once
        before Overrule() was invented, the operator+ is declared private and
        not implemented. */
    Msg_List operator+ (const Msg_List& otherList) const;
};

#endif //DBMSrvMsg_Warning_hpp
