/*!========================================================================

  @file         RTEIPC_Mailslot.cpp
  @ingroup      Interprocess Communication
  @author       StefanP

  @brief        Mailslot Handling

  @since        2003-11-13  16:05
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
 ============================================================================*/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if !defined (_WIN32)
#include <sys/param.h>
#endif

#if !defined (_WIN32)                           /*&if $OSSPEC=WIN32*/
#include    "RunTime/RTE_saveUNIXcalls.h"       /*nocheck*/
#include    "RunTime/RTE_UNIXAuthenticate.hpp"  /*nocheck*/
#endif                                          /*&endif*/

#include    "RunTime/IPC/RTEIPC_Mailslot.hpp"
#include    "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/RTE_IPC_Messages.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include    "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"







extern SAPDBTrace_Topic IPC_Trace; ///<Trace for Interprocess Communication


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
  *  METHODS                                                                  *
 *===========================================================================*/

RTEIPC_Mailslot::RTEIPC_Mailslot ()
#if !defined (_WIN32)
      : m_pBuffer                   (NULL),
        m_BytesInBuffer             (0),
        m_BufferSize                (0),
        m_MaxMessageLength          (0),
#else
      : m_MaxMessageLength          (MAX_MESSAGE_LENGTH),
#endif
        m_Handle                    (RTE_UNDEF_HANDLE),
        m_OpenMode                  (RTE_OpenUndef)
{
    SAPDBTRACE_METHOD_DEBUG ("RTEIPC_Mailslot::RTEIPC_Mailslot", IPC_Trace, 9);
}

/*---------------------------------------------------------------------------*/

RTEIPC_Mailslot::~RTEIPC_Mailslot ()
{
    SAPDBErr_MessageList       messageList;

    SAPDBTRACE_METHOD_DEBUG ("RTEIPC_Mailslot::~RTEIPC_Mailslot", IPC_Trace, 9);

    if (!Close (messageList))
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_CLOSE, 
                                                          m_MailslotName);
        RTE_Message (messageList);
    }
}

/*---------------------------------------------------------------------------*/

RTEIPC_CommunicationObject::CommObjRC  
        RTEIPC_Mailslot::Open  
(
    RTE_Path const &                    mailslotName,
    RTE_OpenMode const                  openMode,
    RTE_SECURITY_ATTRIBUTES &           secAttribute,   
    SAPDBErr_MessageList &              messageList
)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    RTE_SystemRc            rc = RTE_SYSTEMRC_NO_ERROR;    
    CommObjRC               retCode = NoError;

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTEIPC_Mailslot::Open", IPC_Trace, 9);

    m_OpenMode = openMode;
    strcpy (m_MailslotName, mailslotName);

    if (RTE_UNDEF_HANDLE != m_Handle)
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_OBJECT_ALREADY_OPEN,
            m_MailslotName); 
        return Error;
    }

    if (RTE_OpenNew == m_OpenMode)
    {
#if defined (_WIN32)
        m_Handle = CreateMailslot (m_MailslotName, m_MaxMessageLength, 
                                   MAILSLOT_WAIT_FOREVER, &secAttribute);
      
        if (RTE_UNDEF_HANDLE == m_Handle)
        {
            rc = GetLastError ();
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_CREATE, 
                                                              m_MailslotName, SAPDB_ToString (rc));
            retCode = Error;
        }
#else

        mode_t                          umask_old ;

        /*  The mailslot might exist from a previous run */
        Close (messageList);
        umask_old = umask (0);
        rc = mknod (m_MailslotName, S_IFIFO | secAttribute, 0);
        umask (umask_old);
        if (RTE_SYSTEMRC_NO_ERROR != rc)
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_CREATE, 
                                                              m_MailslotName, RTE_save_strerror (errno));
            retCode = Error;
        }
        else
        {
            SAPDB_Int4  dummy;
            SAPDB_Int4  groupId;

            if (!RTE_GetDefaultSapdbUserAndGroupIds (dummy, groupId, messageList))
            {
                messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_GET_SAPDB_OWNER_GROUP);
                retCode = Error;
            }
            else if (!RTE_SetFileOwnerAndGroup (m_MailslotName, getuid (), groupId, messageList))
            {
                messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_SHM_SET_OWNER_GROUP,
                                                    SAPDB_ToString (getuid ()),
                                                    SAPDB_ToString (groupId),
                                                    m_MailslotName);
                retCode = Error;
            }
            else
            {

            /*
            *  O_RDWR avoids blocking and does not deliver EOF when
            *  last client closes.
            */
                m_Handle = RTE_save_open ((const char *)m_MailslotName , O_RDWR);
                if (RTE_UNDEF_HANDLE == m_Handle)
                {
                    messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_OPEN, 
                        m_MailslotName, RTE_save_strerror (errno));
                    retCode = Error;
                }
            }
        }
#endif
    }
    else /* open existing mailslot */
    {
#if defined (_WIN32)

        m_Handle  = CreateFile (m_MailslotName, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 
                                NULL, OPEN_EXISTING, 0, NULL );
        if (RTE_UNDEF_HANDLE == m_Handle)
        {
            rc = GetLastError();
            if (RTE_SYSTEMRC_FILE_NOT_FOUND == rc)
            {
                messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_MISSING, 
                                                    m_MailslotName, SAPDB_ToString(rc));
                retCode = ResourceMissing;
            }
            else
            {
                messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_OPEN, 
                                                    m_MailslotName, SAPDB_ToString(rc));
                retCode = Error;
            }
        }

#else
        //Testing if this is really a FIFO
        retCode = IsFIFO (messageList);
        if (NoError == retCode)
        {
            m_Handle = RTE_save_open ( (const char *)m_MailslotName, O_WRONLY | O_NONBLOCK );
            if (m_Handle == RTE_UNDEF_HANDLE)
            {
                if ( RTE_SYSTEMRC_FILE_NOT_FOUND == errno)
                {
                    messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_MISSING, 
                                                        m_MailslotName, RTE_save_strerror (errno));

                    retCode = ResourceMissing;
                }
                else
                {
                    messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_OPEN, 
                                                        m_MailslotName, RTE_save_strerror (errno));

                    retCode = Error;
                }
            }
        }

#endif
    }

#if !defined (_WIN32)
    if (NoError == retCode)
    {
//RTE_save_fpathconf
        errno = 0;
        rc = fpathconf (m_Handle, _PC_PIPE_BUF);//Getting value of PIPE_BUF:
        //Up to PIPE_BUF bytes are guarantied to be written atomically
        //by the system. 

        if (RTE_UNDEF_ERROR == rc)
        {
            if (0 == errno)
            {   //Machine allows unlimited PIPE_BUF!
                m_BufferSize = MAX_MESSAGE_LENGTH;
            }
            else
            {
                messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_GET_PIPE_BUF, 
                    RTE_save_strerror (errno));
                return Error;
            }
        }
        else
        {
            m_BufferSize = SAPDB_MIN (MAX_MESSAGE_LENGTH, rc);
        }


        m_MaxMessageLength = (m_BufferSize - 2)/2;//One Byte at the beginning and the end of a message and
        //maximal two bytes for each byte to send due to the bit stuffing 
        //method is needed.

        if (NULL == (m_pBuffer = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate (m_BufferSize)))
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_ALLOC_BUFFER, 
                SAPDB_ToString (m_BufferSize));
            return Error;
        }
        else
        {
            memset (m_pBuffer, 0, m_BufferSize);
        }
    }
#endif

    return retCode;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool    RTEIPC_Mailslot::Close   
(
    SAPDBErr_MessageList &      messageList 
)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    SAPDB_Bool           retCode = true;
/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBErr_MessageList                       tmpMessageList;

    SAPDBTRACE_METHOD_DEBUG ("RTEIPC_Mailslot::Close", IPC_Trace, 9);

    if (RTE_UNDEF_HANDLE != m_Handle)
    {
#if defined (_WIN32)


        if (!CloseHandle(m_Handle))
        {
            RTE_SystemRc            rc = GetLastError();    
            tmpMessageList = tmpMessageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_CLOSE_HANDLE, 
                                                                    m_MailslotName,
                                                                    SAPDB_ToString (rc));
            retCode = false;
        }
#else
        if (0 != RTE_save_close (m_Handle))
        {
            tmpMessageList = tmpMessageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_CLOSE_HANDLE, 
                                                                    m_MailslotName,
                                                                    RTE_save_strerror (errno));
            retCode = false;
        }

        if (NULL != m_pBuffer)
        {
            RTEMem_RteAllocator::Instance().Deallocate(m_pBuffer);
        }
#endif
    }

#if !defined (_WIN32)
    if (RTE_OpenNew == m_OpenMode)
    {
        if (RTE_SYSTEMRC_NO_ERROR != RTE_save_unlink (m_MailslotName))
        {
            tmpMessageList = tmpMessageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_UNLINK, 
                                                                    m_MailslotName,
                                                                    RTE_save_strerror (errno));
            retCode = false;
        }
    }
#endif

    
    m_Handle = RTE_UNDEF_HANDLE;

    if (!retCode)
    {
        messageList = tmpMessageList;
    }

    return retCode;
}

/*---------------------------------------------------------------------------*/


RTEIPC_CommunicationObject::CommObjRC      
        RTEIPC_Mailslot::Write   
(
    SAPDB_Byte const * const    pMessage,
    SAPDB_UInt4 &               bytesToWrite,
    SAPDB_UInt4 &               bytesWritten,
    SAPDBErr_MessageList &      messageList
)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    RTE_SystemRc            rc = RTE_SYSTEMRC_NO_ERROR;    
    CommObjRC               retCode = NoError;
    SAPDBErr_MessageList    tmpMessageList;

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTEIPC_Mailslot::Write", IPC_Trace, 9);

    if (RTE_OpenExisting != m_OpenMode)
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_WRITE_NOT_ALLOWED,
                                            m_MailslotName);
        return Error;
    }

    if (RTE_UNDEF_HANDLE == m_Handle)
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_NOT_OPENED,
                                            m_MailslotName);
        return Error;
    }

    if (bytesToWrite > m_MaxMessageLength)
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_MESSAGE_LEN,
                                                          m_MailslotName,
                                                          SAPDB_ToString (m_MaxMessageLength),
                                                          SAPDB_ToString (bytesToWrite));
        return Error;
    }

#if defined (_WIN32)
    if ( !WriteFile (m_Handle, pMessage,  bytesToWrite, reinterpret_cast <DWORD *> (&bytesWritten), NULL))
    {
        rc = GetLastError ();
    }

    if (RTE_SYSTEMRC_NO_ERROR != rc)
    {
        if (rc == ERROR_FILE_NOT_FOUND)
        {
             tmpMessageList = tmpMessageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_MISSING, 
                                                               m_MailslotName, SAPDB_ToString (rc));
             retCode = ResourceMissing;
        }
        else
        {
            tmpMessageList = tmpMessageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_WRITE, 
                                                              m_MailslotName, SAPDB_ToString (rc));

            retCode = Error;
        }
    }
#else
    SAPDB_Long      bytesWrittenStuffed;
    SAPDB_UInt1 *   pData = (SAPDB_UInt1 *)pMessage;
    SAPDB_UInt4     bufferIdx = 0;

    /* Supply message with a start (0xFF) and end pattern (0xFE) (kind of a bit stuffing method) */
    memset (m_pBuffer, 0, m_BufferSize);
    m_pBuffer[bufferIdx++] = 0xFF;

    for (SAPDB_UInt4    idx = 0; idx < bytesToWrite; ++idx)
    {
        if (pData[idx] >= 0xFD)
        {
            m_pBuffer[bufferIdx++] = 0xFD;
            if (pData[idx] >= 0xFE)
            {
                m_pBuffer[bufferIdx++] = pData[idx] - 0xFD;  
            }
            else /* pData[idx] == 0xFD */
            {
                /*
                    Data                Buffer
                    FE              ->  FD|01
                    FF              ->  FD|02

                    FD|00           ->  FD|00|00
                    FD|01           ->  FD|00|01
                    FD|02           ->  FD|00|02
                    FD|03           ->  FD|03
                    ...
                */
                if (idx < bytesToWrite - 1 && pData[idx + 1] <= 0x02)
                {
                    m_pBuffer[bufferIdx++] = 0x00;
                }
            }
        }
        else 
        {
            m_pBuffer[bufferIdx++] = pData[idx];  
        }
    }

    m_pBuffer[bufferIdx++] = 0xFE;
    
    bytesToWrite = bufferIdx;
    bytesWrittenStuffed = RTE_save_write (m_Handle, (void *)m_pBuffer, bytesToWrite);
    if (0 > bytesWrittenStuffed)
    {
        if (ENXIO == errno || EPIPE == errno)
        {
            tmpMessageList = tmpMessageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_MISSING, 
                                                              m_MailslotName, RTE_save_strerror (errno));
            retCode = ResourceMissing;
        }
        else
        {
            tmpMessageList = tmpMessageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_WRITE, 
                                                              m_MailslotName, RTE_save_strerror (errno));

            retCode = Error;
        }
        bytesWritten = 0;
    }
    else
    {
        bytesWritten = static_cast <SAPDB_UInt4> (bytesWrittenStuffed);
    }

#endif
    return retCode;
}

/*---------------------------------------------------------------------------*/

RTEIPC_CommunicationObject::CommObjRC     
        RTEIPC_Mailslot::Read   
(
    SAPDB_Byte * const          pMessage,
    SAPDB_UInt4 &               bytesRead,
    SAPDBErr_MessageList &      messageList
)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    RTE_SystemRc            rc = RTE_SYSTEMRC_NO_ERROR;    
    SAPDBErr_MessageList    tmpMessageList;
/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTEIPC_Mailslot::Read", IPC_Trace, 9);

    if (RTE_OpenNew != m_OpenMode)
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_READ_NOT_ALLOWED,
                                            m_MailslotName);
        return Error;
    }

    if (RTE_UNDEF_HANDLE == m_Handle)
    {
        messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_NOT_OPENED,
                                                          m_MailslotName);
        return Error;
    }

#if defined (_WIN32)

    if (!ReadFile (m_Handle, pMessage, m_MaxMessageLength, reinterpret_cast <DWORD *> (&bytesRead), NULL))
    {
        rc = GetLastError ();
        if (ERROR_INSUFFICIENT_BUFFER == rc)
        {
            messageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_IPC_MAILSLOT_BUF_SIZE, 
                                                             m_MailslotName,
                                                             SAPDB_ToString (m_MaxMessageLength));
        }
        else
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_READ,
                                                              m_MailslotName,
                                                              SAPDB_ToString (rc));
        }

        return Error;
    }
#else
    SAPDB_UInt4     maxBytesLeftToRead = m_MaxMessageLength + 2; //Including start and end pattern
    SAPDB_UInt4     offsetBuffer = 0;
    SAPDB_UInt4     foundStartPattern = false;
    SAPDB_Bool      readingFurtherData = false;
    SAPDB_Long      bytesReadStuffed = 0;
    SAPDB_Bool      readDataCompleted = false;
    while (0 < maxBytesLeftToRead && !readDataCompleted)
    {
        do
        {
            //at this point it is ensured that offsetBuffer == 0
            if (0 == m_BytesInBuffer || readingFurtherData)
            {
                memset (m_pBuffer + m_BytesInBuffer, 0, m_BufferSize - m_BytesInBuffer);
                bytesReadStuffed = RTE_save_read (m_Handle, 
                                           m_pBuffer + m_BytesInBuffer, 
                                           m_BufferSize - m_BytesInBuffer);
                if (0 >= bytesReadStuffed)
                {
                    messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_READ,
                                                                      m_MailslotName,
                                                                      RTE_save_strerror (errno));
                    return Error;
                }

                m_BytesInBuffer += bytesReadStuffed;
                readingFurtherData = false;
            }

            if (!foundStartPattern)
            {
                //searching for start pattern in buffer
                for ( ; m_pBuffer[offsetBuffer] != 0xFF && offsetBuffer < m_BytesInBuffer; ++offsetBuffer) {;}

                if (offsetBuffer != 0)
                {
                    tmpMessageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEWARN_IPC_MAILSLOT_DIRTY_READ, 
                                                           m_MailslotName);
                    RTE_Message (tmpMessageList);
                }

                m_BytesInBuffer -= offsetBuffer;

                if (m_BytesInBuffer > 0) //start pattern was read
                {
                    ++offsetBuffer;
                    --m_BytesInBuffer;
                    SAPDB_MemMoveNoCheck (m_pBuffer, m_pBuffer + offsetBuffer, m_BytesInBuffer);
                    memset  (m_pBuffer + m_BytesInBuffer, 0, m_BufferSize - m_BytesInBuffer);
                    foundStartPattern = true;
                    --maxBytesLeftToRead;
                }

                offsetBuffer = 0;

            }
        }
        while (!foundStartPattern);

        while (offsetBuffer < m_BytesInBuffer)
        {
            if (maxBytesLeftToRead == 1)   //end pattern is expected to read
            {   
                if (m_pBuffer[offsetBuffer] != 0xFE)
                {
                    tmpMessageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEWARN_IPC_MAILSLOT_MESSAGE_TOO_LONG, 
                                               m_MailslotName, SAPDB_ToString (m_MaxMessageLength));
                    RTE_Message (tmpMessageList);

                    //Try reading the next message
                    maxBytesLeftToRead = m_MaxMessageLength + 2;
                    memset (pMessage, 0, bytesRead); 
                    bytesRead = 0;
                    foundStartPattern = false;
                    //Buffer is not incremented here because it may be the next start pattern
                }
                else
                {
                    --maxBytesLeftToRead;
                    ++offsetBuffer;
                }

                break;
            }


            if (m_pBuffer[offsetBuffer] < 0xFD)
            {
                SAPDB_UInt4     bytesToCopy;

                for (bytesToCopy = 0 ; 
                     offsetBuffer + bytesToCopy < m_BytesInBuffer 
                        && m_pBuffer[offsetBuffer + bytesToCopy] < 0xFD
                        && maxBytesLeftToRead > 1; 
                     ++bytesToCopy, --maxBytesLeftToRead)
                {;}

                SAPDB_MemCopyNoCheck (pMessage + bytesRead, m_pBuffer + offsetBuffer, bytesToCopy);
                offsetBuffer += bytesToCopy;
                bytesRead += bytesToCopy;
            }
            else if (m_pBuffer[offsetBuffer] == 0xFD)
            {
                if (m_BytesInBuffer - offsetBuffer > 1)
                {//There are at least two bytes left in the buffer
                    if (m_pBuffer[offsetBuffer + 1] == 0x00)
                    {
                        if (m_BytesInBuffer - offsetBuffer > 2)
                        {//There are at least three bytes left in the buffer
                            if (m_pBuffer[offsetBuffer + 2] <= 0x02)
                            {
                                pMessage[bytesRead++] = 0xFD;
                                pMessage[bytesRead++] = m_pBuffer[offsetBuffer + 2];
                                offsetBuffer += 3;
                                maxBytesLeftToRead -= 2;
                            }
                            else
                            {
                                //Protocol error: 0xFD|0x00 must not followed by a value greater than 0x02 
                                tmpMessageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEWARN_IPC_MAILSLOT_PROTOCOL, 
                                                               m_MailslotName);
                                RTE_Message (tmpMessageList);

                                //Try reading the next message
                                offsetBuffer += 3;
                                maxBytesLeftToRead = m_MaxMessageLength + 2;
                                memset (pMessage, 0, bytesRead); 
                                bytesRead = 0;
                                foundStartPattern = false;
                                break;
                            }
                        }
                        else //only two bytes left in the buffer: FD|00
                        {
                            //we need further data from the pipe
                            readingFurtherData = true;
                            break;
                        }
                    }
                    else
                    {
                        if (m_pBuffer[offsetBuffer + 1] <= 0x02) 
                        {
                            pMessage[bytesRead++] = 0xFD + m_pBuffer[offsetBuffer + 1];
                            offsetBuffer += 2;
                        }
                        else
                        {
                            pMessage[bytesRead++] = 0xFD;
                            ++offsetBuffer;
                        }

                        --maxBytesLeftToRead;
                    }
                }
                else //only one byte left in the buffer: FD
                {
                    //we need further data from the pipe
                    readingFurtherData = true;
                    break;
                }
            }
            else if  (m_pBuffer[offsetBuffer] == 0xFE)
            {//End pattern was read                    
                ++offsetBuffer;
                readDataCompleted = true;
                break;
            }
            else //0xFF, Protocol error: reading start pattern before end pattern
            {
                ++offsetBuffer;
                tmpMessageList = SAPDBErr_MessageList(RTE_CONTEXT, RTEWARN_IPC_MAILSLOT_MISSING_END_PATTERN, 
                                               m_MailslotName);
                RTE_Message (tmpMessageList);

                //Start reading the next message
                maxBytesLeftToRead = m_MaxMessageLength + 1;
                memset (pMessage, 0, bytesRead); 
                bytesRead = 0;
                //foundStartPattern is already true!
            }
        }

        m_BytesInBuffer -= offsetBuffer;
        SAPDB_MemMoveNoCheck (m_pBuffer, m_pBuffer + offsetBuffer, m_BytesInBuffer);
        memset  (m_pBuffer + m_BytesInBuffer, 0, m_BufferSize - m_BytesInBuffer);
        offsetBuffer = 0;
    }
#endif

    return NoError;
}

/*---------------------------------------------------------------------------*/

#if !defined (_WIN32)

RTEIPC_CommunicationObject::CommObjRC    
        RTEIPC_Mailslot::IsFIFO   
(
    SAPDBErr_MessageList &      messageList
) 
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    RTE_SystemRc            rc = RTE_SYSTEMRC_NO_ERROR;    
    CommObjRC               retCode = NoError;
    struct stat             statbuf;
/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTEIPC_Mailslot::IsFIFO", IPC_Trace, 9);

    /* Check, if file is a fifo */
    rc = RTE_save_stat ( (const char *)m_MailslotName , &statbuf );
    if (RTE_SYSTEMRC_NO_ERROR != rc)
    {
        if ( errno == ENOENT )
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_MISSING, 
                                                m_MailslotName, RTE_save_strerror (errno));

            retCode = ResourceMissing;
        }
        else
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_STAT, 
                                                m_MailslotName, RTE_save_strerror (errno));
            retCode = Error;
        }
    }
    else
    {
        if (!S_ISFIFO(statbuf.st_mode))
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_IPC_MAILSLOT_NO_FIFO, 
                                                m_MailslotName,
                                                SAPDB_ToString (statbuf.st_mode));
            retCode = Error;
        }
    }

    return retCode;
}


#endif