/*!
    \file    DBMSrvInfrReply_ReplyBuffer.cpp
    \author  TiloH
    \ingroup infrastructure for DBM Server reply
    \brief   implementation of a class for supplying a dummy reply buffer

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

#include "DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_ReplyBuffer.hpp"
#include "DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_Reply2MsgList.hpp"

const size_t DBMSrvInfrReply_ReplyBuffer::m_BufferSpace=32768;

DBMSrvInfrReply_ReplyBuffer::DBMSrvInfrReply_ReplyBuffer(
    )
  : m_Reply(),
    m_Buffer(0),
    m_Length(0),
    m_MaxLength(0)
{
    m_Buffer=new char [DBMSrvInfrReply_ReplyBuffer::m_BufferSpace];

    if(0!=m_Buffer)
    {
        m_Buffer[0]='\0';
        m_MaxLength=DBMSrvInfrReply_ReplyBuffer::m_BufferSpace-1;
        m_Reply.setTo(m_Buffer, &m_Length, m_MaxLength);
    }
}

DBMSrvInfrReply_ReplyBuffer::~DBMSrvInfrReply_ReplyBuffer()
{
    if(0!=m_Buffer)
        delete [] m_Buffer;
}

DBMSrv_Reply & DBMSrvInfrReply_ReplyBuffer::getReply()
{
    return m_Reply;
}

char * DBMSrvInfrReply_ReplyBuffer::getData()
{
    return m_Buffer;
}

int * DBMSrvInfrReply_ReplyBuffer::getLength()
{
    return &m_Length;
}

int DBMSrvInfrReply_ReplyBuffer::getMaximalLength() const
{
    return m_MaxLength;
}

bool DBMSrvInfrReply_ReplyBuffer::adaptLength(DBMSrvMsg_Error & errorList)
{
    if(0!=m_Buffer && !m_Reply.adaptLength())
    {
        //as buffer was overrun, heap maybe be corrupted -> we can only hope, that the following message will reach the user
        errorList.MarkAsSubMessages(); //try to lose none of the previous error messages, this error will be obscure anyway
        errorList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_REPLY_ENDLESS));
        errorList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_ERR));

        return false;
    }
    else
        return true;
}

bool DBMSrvInfrReply_ReplyBuffer::fillMessageList(DBMSrvMsg_Error & errorList)
{
    if(isValid(errorList) && adaptLength(errorList))
        return DBMSrvInfrReply_Reply2MsgList::transform(m_Reply, errorList);
    else
        return false;
}

bool DBMSrvInfrReply_ReplyBuffer::isValid()
{
    return (0!=m_Buffer);
}

bool DBMSrvInfrReply_ReplyBuffer::isValid(DBMSrvMsg_Error & errorList)
{
    if(0==m_Buffer)
    {
        errorList.ClearMessageList();
        errorList.Overrule(DBMSrvMsg_Error(SDBMSG_DBMSRV_MEM));
        return false;
    }
    else
        return true;
}
