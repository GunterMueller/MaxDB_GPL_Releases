/*!
    \file    DBMSrvInfrReply_ReplyBufferWritingIntoMsgList.hpp
    \author  TiloH
    \ingroup infrastructure for DBM Server reply
    \brief   declaration of a class for supplying a dummy reply and converting
             that DBM Server reply into a DBM Server message list

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

#if !defined(DBMSRVINFRREPLY_REPLYBUFFERWRITINGINTOMSGLIST_HPP)
#define DBMSRVINFRREPLY_REPLYBUFFERWRITINGINTOMSGLIST_HPP

#include "DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_ReplyBuffer.hpp"


/*! \brief this class can be used, if a reply buffer is needed to call an
    older function and a message list must be returned
    
    This class was invented to prevent reply-buffer-infection of new code
    using message lists by old code using the reply buffer. The final
    aim is to make this class redundant.
    
    This object is allocating its own reply buffer. Therefore you should use
    the isValid() function to check if that memory was allocated successfully.

    The reply can be written into the message list explicitly by using the
    function fillMessageList. Or, if stated during construction, can also be
    implicitly written by the destructor of the class. The class
    DBMSrvInfrReply_Reply2MsgList is used to transform the reply into a message
    list and it is thereby defining the rules for the transformation.

    \see DBMSrvInfrReply_Reply2MsgList 
    \TODO delete class as soon as there is no need for it */
class DBMSrvInfrReply_ReplyBufferWritingIntoMsgList:public DBMSrvInfrReply_ReplyBuffer
{
  public:

    /*! \brief enumartion for switching filling in destructor on or off */
    enum FillingMode
    {
        DontFillInDestructor, //!< the message list is only filled during explicit calls to the fillMessageList() function
        FillInDestructor      //!< the message list is filled during explicit calls to the fillMessageList() function and in the destructor
    };
    /*! \brief constructor
    
        If the constructor fails to allocate space for the internal reply
        buffer the message list will contain an out of memory error.

        The message list is by default not filled during the destructor of this
        object.

        \param errorList [IN] the error list that will be filled with the reply
        \param fillingMode [IN] switching of message list filling in destructor */
    DBMSrvInfrReply_ReplyBufferWritingIntoMsgList(
        DBMSrvMsg_Error & errorList,
        FillingMode       fillingMode=DBMSrvInfrReply_ReplyBufferWritingIntoMsgList::DontFillInDestructor);

    /*! \brief destructor
    
        If filling mode FillInDestructor was activated during construction, the
        message list is filled (maybe again) by the destructor */
    ~DBMSrvInfrReply_ReplyBufferWritingIntoMsgList();

    /*! \function for determining state of the object
    
        If the object is not valid, the message list handled by the object
        will contain a corresponding message.

       \return false, if reply buffer was not allocated, true otherwise */
    bool isValid();

    /*! \function filling the message list
    
        The message list supplied in the constructor is cleared and the content
        of the internal reply buffer is written into the message list using class
        DBMSrvInfrReply_Reply2MsgList

        Any changes applied to the internal reply buffer after this function
        was called will have no effect for the message list until you call this
        function again
        
        \see DBMSrvInfrReply_Reply2MsgList
        
        \return true, if error list represents the reply, false otherwise */
    bool fillMessageList();

  private:
    DBMSrvMsg_Error             & m_MsgList;

    FillingMode m_fillingMode;
};

#endif
