/*!
    \file    DBMSrvInfrReply_ReplyBufferWritingIntoMsgList.cpp
    \author  TiloH
    \ingroup infrastructure for DBM Server reply
    \brief   implementation of a class for supplying a dummy reply and
             converting that DBM Server reply into a DBM Server message list

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

#include "hcn36.h"

#include "DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_ReplyBufferWritingIntoMsgList.hpp"

DBMSrvInfrReply_ReplyBufferWritingIntoMsgList::DBMSrvInfrReply_ReplyBufferWritingIntoMsgList(
    DBMSrvMsg_Error & errorList,
    FillingMode       fillingMode)
  : DBMSrvInfrReply_ReplyBuffer(),
    m_MsgList(errorList),
    m_fillingMode(fillingMode)
{
    DBMSrvInfrReply_ReplyBuffer::isValid(m_MsgList); //check for validity and write error message if necessary
}

DBMSrvInfrReply_ReplyBufferWritingIntoMsgList::~DBMSrvInfrReply_ReplyBufferWritingIntoMsgList()
{
    if(m_fillingMode==DBMSrvInfrReply_ReplyBufferWritingIntoMsgList::FillInDestructor)
        fillMessageList();
}

bool DBMSrvInfrReply_ReplyBufferWritingIntoMsgList::fillMessageList()
{
    return DBMSrvInfrReply_ReplyBuffer::fillMessageList(m_MsgList);
}

bool DBMSrvInfrReply_ReplyBufferWritingIntoMsgList::isValid()
{
    return DBMSrvInfrReply_ReplyBuffer::isValid(m_MsgList);
}
