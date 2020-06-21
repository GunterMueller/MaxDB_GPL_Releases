/*!
    \file    DBMSrvInfrReply_ReplyBuffer.hpp
    \author  TiloH
    \ingroup infrastructure for DBM Server reply
    \brief   declaration of a class for supplying a dummy reply buffer

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

#if !defined(DBMSRVINFRREPLY_REPLYBUFFER_HPP)
#define DBMSRVINFRREPLY_REPLYBUFFER_HPP

#include "DBM/Srv/Infrastructure/Reply/DBMSrvInfrReply_Reply2MsgList.hpp"


/*! \brief this class can be used, if a reply buffer is needed to call an
    older function
    
    This class was invented to prevent reply-buffer-infection of new code
    using message lists by old code using the reply buffer. The final
    aim is to make this class redundant.
    
    If your final aim is to transform the content of the reply buffer into a
    message list, you should use the class
    DBMSrvInfrReply_ReplyBufferWritingIntoMsgList, which is drived from this
    class and offers a tiny advantage in handling.
    
    This object is allocating its own reply buffer. Therefore you should use
    the isValid() function to check if that memory was allocated successfully.

    The reply must be written into the message list explicitly by using the function
    fillMessageList. The class DBMSrvInfrReply_Reply2MsgList is used to transform the
    reply into a message list and it is thereby defining the rules for the transformation.

    \see DBMSrvInfrReply_Reply2MsgList 
    \see DBMSrvInfrReply_ReplyBufferWritingIntoMsgList 
    \TODO delete class as soon as there is no need for it */
class DBMSrvInfrReply_ReplyBuffer
{
  public:
    /*! \brief constructor
    
        You must use function isValid(), to check if the constructor failed or
        succeeded to allocate space for the internal reply buffer. */
    DBMSrvInfrReply_ReplyBuffer();

    /*! \brief destructor
    
        The alocated space is freed again */
    ~DBMSrvInfrReply_ReplyBuffer();

    /*! \brief function giving access to the reply object

        \return reference to internal reply object */
    DBMSrv_Reply & getReply();

    /*! \brief function giving access to the buffer managed by the internal reply onject

        Same as getReply().giveData()

        \return pointer to buffer if isValid() returns true, 0 otherwise */
    char * getData();

    /*! \brief function giving access to the length of the reply nuffer

        Same as getReply().giveLength()

        \return pointer to internal length variable */
    int * getLength();

    /*! \brief function returning maximal length of buffer

        Same as getReply().giveMaximalLength()

        \return size of internal reply buffer in bytes */
    int getMaximalLength() const;

    /*! \brief function for determining state of the object
    
        \return false, if reply buffer was not allocated, true otherwise */
    bool isValid();

    /*! \brief function for determining state of the object
    
        \param errorList [OUT] message list that will contain an error message if object is not valid 
        \return false, if reply buffer was not allocated, true otherwise */
    bool isValid(DBMSrvMsg_Error & errorList);

    /*! \function filling a message list
    
        The message list supplied as parameter is cleared and the content
        of the internal reply buffer is written into the message list using class
        DBMSrvInfrReply_Reply2MsgList.
        
        \see DBMSrvInfrReply_Reply2MsgList

        \param errorList [OUT] the list containing the reply as message list
        \return true, if error list represents the reply, false otherwise */
    bool fillMessageList(DBMSrvMsg_Error & errorList);

  private:
    bool adaptLength(DBMSrvMsg_Error & errorList);

    DBMSrv_Reply m_Reply;

    char * m_Buffer;
    int    m_Length;
    int    m_MaxLength;

    static const size_t m_BufferSpace;
};

#endif
