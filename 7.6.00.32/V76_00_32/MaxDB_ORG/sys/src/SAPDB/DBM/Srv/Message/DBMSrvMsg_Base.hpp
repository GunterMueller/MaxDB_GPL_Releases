/*!
    \file    DBMSrvMsg_Base.hpp
    \author  MarcW
    \brief   message base class

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

#if !defined(DBMSRVMSG_BASE_HPP)
#define DBMSRVMSG_BASE_HPP

/*! 
    @brief base class for DBM server messages

    This class does nothing except for providing a common base class
    of DBM server messages. The subclasses DBMSrvMsg_Error, DBMSrvMsg_Info and
    DBMSrvMsg_Warning are also subclasses of Msg_List. The new message
    classes provide an easy way of creating Msg_List objects in the 
    DBM server context. These objects (like any other Msg_List objects),
    can be written to a protocol with the DBMLog_Logger
    @see DBMSrvMsg_Error
    @see DBMSrvMsg_Info
    @see DBMSrvMsg_Warning
    @see Msg_List
    @see DBMLog_Logger
*/
class DBMSrvMsg_Base {
public:

    /*! @brief required for subclassing */
    virtual ~DBMSrvMsg_Base() {}
};

#endif //DBMSrvMsg_Base_hpp
