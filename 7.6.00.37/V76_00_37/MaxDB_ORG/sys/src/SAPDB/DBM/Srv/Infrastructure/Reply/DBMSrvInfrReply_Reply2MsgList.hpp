/*!
    \file    DBMSrvInfrReply_Reply2MsgList.hpp
    \author  TiloH
    \ingroup infrastructure for DBM Server reply
    \brief   declaration of a class converting a DBM Server reply into a DBM
             Server message list

    \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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

#if !defined(DBMSRVINFRREPLY_REPLY2MSGLIST_HPP)
#define DBMSRVINFRREPLY_REPLY2MSGLIST_HPP

#include "DBM/Srv/DBMSrv_Reply.hpp"
#include "DBM/Srv/Message/DBMSrvMsg_Error.hpp"

/*! \brief this class can be used, to transform the content of a reply buffer
    into a message list.

    This is a temporary class, that was invented to avoid infection of new
    code using message lists by old code using the reply buffer. The final
    aim is to make this class redundant.

    \TODO delete class as soon as there is no need for it*/
class DBMSrvInfrReply_Reply2MsgList
{
  public:
    /*! \brief static function to transform a reply into a message list
    
    Lines representing DBM Server messages are appended as these DBM Server
    messages, all other lines are wrapped into the special DBM Server message
    INTERNAL_TRANSPORT_MSG (component DBMSrv, ID 222, code -24778), that
    is known by DBMSrv_Reply, vcn90.cpp and DBMLog_Logger
    
    \param reply     [IN]     the reply
    \param errorList [IN/Out] the message list that is filled
    \return true if errorList represents reply correctly */
    static bool transform(DBMSrv_Reply & reply, DBMSrvMsg_Error & errorList);

  private:
    static char set(DBMSrv_Reply & reply, DBMSrv_Reply::Position position, char newValue);
};

#endif
