/*!
  @file           RTE_ConsoleRequestCommunication.cpp
  @author         StefanP
  @special area   Request Communication XCons - Console Thread
  @brief          Common Communication Class
  @see            

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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined (_WIN32)    /*&if $OSSPEC = WIN32*/ 
#include  "gos41.h"     /* nocheck */
#else                   /*&else*/
#include  "gen41.h"     /* nocheck */
#include  "hen40.h"     /* nocheck */
#include  "RunTime/RTE_saveUNIXcalls.h"  /* nocheck */
#endif                  /*&endif*/

#include    "SAPDBCommon/SAPDB_C_Cplusplus.h"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"
#include    "RunTime/RTE_MessageList.hpp"
#include    "RunTime/RTE_Message.hpp"
#include    "RunTime/RTE_Console_Messages.hpp"
#include    "RunTime/RTE_ConsoleCommon.hpp"
#include    "RunTime/RTE_ConsoleRequestCommunication.hpp"
#include    "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"
#include    "SAPDBCommon/SAPDB_sprintf.h"
#include    "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/

extern SAPDBTrace_Topic Console_Trace; ///< global console trace object

/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/


RTE_ConsoleRequestCommunication::RTE_ConsoleRequestCommunication 
(
    SAPDB_Char const * const        pszServerDB,
    SAPDB_UInt4 const               openMode
)   
#if defined (_WIN32)
    :   m_hConsoleFIFO          (RTE_UNDEF_HANDLE),
        m_OpenMode              (openMode)
#else
    :   m_hConsoleFIFO          (RTE_UNDEF_HANDLE),
        m_OpenMode              (openMode),
        m_pRequestBuffer        (NULL),
        m_BytesInRequestBuffer  (0)
#endif

{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleRequestCommunication::RTE_ConsoleRequestCommunication", 
                             Console_Trace, 9);
    strcpy (m_ServerDB, pszServerDB);
    BuildFIFOName ();
}

/*---------------------------------------------------------------------------*/

RTE_ConsoleRequestCommunication::~RTE_ConsoleRequestCommunication ()
{
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleRequestCommunication::~RTE_ConsoleRequestCommunication", 
                             Console_Trace, 9);

    if (RTE_UNDEF_HANDLE != m_hConsoleFIFO)
    {
#if defined (_WIN32)
        CloseHandle (m_hConsoleFIFO);
#else
        close (m_hConsoleFIFO);
        if (RTE_CONSOLE_SERVER_MODE == m_OpenMode)
        {
            unlink (m_NameFIFO);
        }

        if (NULL != m_pRequestBuffer)
        {
            RTEMem_RteAllocator::Instance().Deallocate(m_pRequestBuffer);
        }
#endif
    }
}

/*---------------------------------------------------------------------------*/

void RTE_ConsoleRequestCommunication::BuildFIFOName ()
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleRequestCommunication::GetFIFOName", 
                             Console_Trace, 5);

#if defined (_WIN32)
    strcpy (m_NameFIFO, RTE_CONSOLE_FIFO_PATH);
    strcat (m_NameFIFO, RTE_CONSOLE_FIFO_NAME);
    strcat (m_NameFIFO, m_ServerDB);
#else
    SAPDB_UInt4     lenPath;

    sql41_get_ipc_dir (*((tsp00_Pathc *)m_NameFIFO));
    lenPath = strlen (m_NameFIFO);
    SAPDB_sprintf (m_NameFIFO + lenPath, sizeof(m_NameFIFO) - lenPath - 1, "db:%s/%s" , 
                   m_ServerDB, RTE_CONSOLE_FIFO_NAME);
#endif

    SAPDBTRACE_WRITELN(Console_Trace, 1, "Path of FIFO: " << m_NameFIFO);
}

/*---------------------------------------------------------------------------*/

RTE_ConsoleRequestCommunication::RequestRC  
    RTE_ConsoleRequestCommunication::Initialize 
(
    RTE_SECURITY_ATTRIBUTES * const     pSA,
    SAPDBErr_MessageList &              messageList
)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    RTE_Path            consoleFIFOPath;
    
/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleRequestCommunication::Initialize", 
                             Console_Trace, 1);

#if defined (_WIN32)
    RTE_Path            consoleFIFOName;
    RTE_SystemRc        rc = RTE_SYSTEMRC_NO_ERROR;
    DWORD               dummy;

    //
    //
    // --- Create named queue / mailslot for console
    //
    //

    strcpy (consoleFIFOPath, RTE_CONSOLE_FIFO_PATH); 
    strcpy (consoleFIFOName, RTE_CONSOLE_FIFO_NAME);
    strcat (consoleFIFOName, m_ServerDB);
    if (RTE_CONSOLE_SERVER_MODE == m_OpenMode)
    {
        rc = sql41c_create_queue_mailslot ( &m_hConsoleFIFO,
                                            consoleFIFOPath, 
                                            consoleFIFOName,
                                            sizeof (RTE_ConsoleRequest),
                                            pSA);           
        if (RTE_SYSTEMRC_NO_ERROR != rc)
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_CREATE_FIFO);
            return Error;
        }
    }
    else //Client mode
    {
        rc = sql41c_open_queue_mailslot (&dummy, &m_hConsoleFIFO, 
                                         consoleFIFOPath, consoleFIFOName );

        if ( rc == ERROR_FILE_NOT_FOUND )
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_DB_NOT_STARTED, 
                                                              m_ServerDB);
            return Offline;
        }
        else if ( rc != RTE_SYSTEMRC_NO_ERROR )
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_ATTACH_FIFO);
            return Error;
        }
    }

#else

    if (RTE_CONSOLE_SERVER_MODE == m_OpenMode)
    {
        if (-1 == sql41_create_fifo ((*(tsp00_Pathc *)m_NameFIFO), RTE_CONSOLE_FIFO_PERMISSION))
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_CREATE_FIFO);
            return Error;
        }
    }

    if (RTE_CONSOLE_SERVER_MODE == m_OpenMode)
    {   // Server side

        /*
         *  O_RDWR avoids blocking and does not deliver EOF when
         *  last client closes.
        */
        m_hConsoleFIFO = RTE_save_open (m_NameFIFO, O_RDWR);

        if (RTE_UNDEF_HANDLE == m_hConsoleFIFO)
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_OPEN_FIFO,
                                                              SAPDB_ToString (errno), sqlerrs ());
            return Error;
        }

        if (NULL == (m_pRequestBuffer = (SAPDB_Byte *) RTEMem_RteAllocator::Instance().Allocate 
                                                            (RTE_CONSOLE_FIFO_REQUEST_BUFFER_SIZE)))
        {
            messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_ALLOC_REQUEST_BUFFER_UNIX, 
                                                              SAPDB_ToString (RTE_CONSOLE_FIFO_REQUEST_BUFFER_SIZE));
            return Error;
        }
        
        memset (m_pRequestBuffer, 0, RTE_CONSOLE_FIFO_REQUEST_BUFFER_SIZE);
    }
    else
    {   // Client mode
        // This open is non blocking. That means it return immediately if no other
        // process has opened the pipe for reading
        m_hConsoleFIFO = RTE_save_open (m_NameFIFO, O_WRONLY | O_NONBLOCK);
        if (RTE_UNDEF_HANDLE == m_hConsoleFIFO)
        {
            if (ENXIO == errno  || ENOENT == errno)
            {
                messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_DB_NOT_STARTED, 
                                                                  m_ServerDB);
                return Offline;
            }
            else
            {
                messageList = SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_OPEN_FIFO,
                                                                  SAPDB_ToString (errno), sqlerrs ());
            }

            return Error;
        }
    }

#endif

    return NoError;
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool  RTE_ConsoleRequestCommunication::PutRequest 
(
    RTE_ConsoleRequest   const  &           request,
    SAPDBErr_MessageList        &           messageList
)
const
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/

    
/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleRequestCommunication::PutRequest", 
                             Console_Trace, 1);
#if defined (_WIN32)
    RTE_SystemRc        rc = RTE_SYSTEMRC_NO_ERROR;

    rc = sql41c_write_queue_mailslot (m_hConsoleFIFO,
                                      (PVOID) &request,
                                      sizeof (request),
                                      0,                            // dummy, OS/2 only
                                      NULL);                        // dummy, OS/2 only
    if (RTE_SYSTEMRC_NO_ERROR != rc)
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_WRITE_MAILSLOT, 
                                                          SAPDB_ToString(rc));
        if ( rc == ERROR_HANDLE_EOF )
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CONSOLE_DB_NOT_STARTED,
                                                             m_ServerDB);
        }

        return false;
    }

#else   //UNIX
    SAPDB_Long  bytesWritten = 0;

    bytesWritten = RTE_save_write (m_hConsoleFIFO, 
                                   (void *)&request, 
                                   sizeof (request));
    if (bytesWritten == -1)
    {
        messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_WRITE_FIFO, 
                                                          SAPDB_ToString (errno), sqlerrs ());
        if (ENXIO == errno  || ENOENT == errno)
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_DB_NOT_STARTED, 
                                                              m_ServerDB);
        }

        return false;
    }

#endif
  return true;
}

/***************************************************************/
#if defined(_WIN32)
/***************************************************************/

SAPDB_Bool  RTE_ConsoleRequestCommunication::GetRequest  
(
    RTE_ConsoleRequest &        request,
    SAPDBErr_MessageList &      messageList
)
const
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    DWORD               ulBytesRead;
    RTE_SystemRc        rc;

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleRequestCommunication::GetRequest", 
                             Console_Trace, 1);

    if (!ReadFile ( m_hConsoleFIFO, &request, sizeof (request), &ulBytesRead, NULL))
    {
        rc = GetLastError ();
        if ( rc == ERROR_INSUFFICIENT_BUFFER )
        {
            messageList = messageList + SAPDBErr_MessageList(RTE_CONTEXT, RTEERR_CONSOLE_REQ_BUF_SIZE, 
                                                             SAPDB_ToString (ulBytesRead), 
                                                             SAPDB_ToString (sizeof (request)));
        }
        else
        {
            messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_READ_FIFO, 
                                                              SAPDB_ToString (rc));
        }
        return false;
    }

    return true;
}

/***************************************************************/
#else /* UNIX, LINUX */
/***************************************************************/

SAPDB_Bool  RTE_ConsoleRequestCommunication::GetRequest  
(
    RTE_ConsoleRequest &            request,
    SAPDBErr_MessageList &          messageList
)
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
{
    SAPDB_Bool                  readFullRequest = false;
    SAPDB_Bool                  startReadingRequest = false;
    SAPDB_UInt4                 numBytesReadFromBuffer = 0;
    SAPDB_Long                  bytesRead = 0;

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    SAPDBTRACE_METHOD_DEBUG ("RTE_ConsoleRequestCommunication::GetRequest", 
                             Console_Trace, 1);


    do 
    {
         numBytesReadFromBuffer = 0;

        /* we have to read from the pipe again, if                            */
        /* the buffer is empty or there are some bytes                        */
        /* left but the hole command is not read yet.                         */
        if (0 == m_BytesInRequestBuffer || startReadingRequest) 
        {
            bytesRead = RTE_save_read (m_hConsoleFIFO, 
                                       m_pRequestBuffer + m_BytesInRequestBuffer, 
                                       RTE_CONSOLE_FIFO_REQUEST_BUFFER_SIZE - m_BytesInRequestBuffer);
            if (bytesRead == -1)
            {
                messageList = messageList + SAPDBErr_MessageList (RTE_CONTEXT, RTEERR_CONSOLE_READ_FIFO_UNIX, 
                                                                  SAPDB_ToString (errno), sqlerrs ());
                return false;

            }
            else
            {
                 m_BytesInRequestBuffer += bytesRead; 
            }
        }

        /*--------------------------------------------------------------------*/   
        /* --- Reading requests from Buffer ----------------------------------*/
        /*--------------------------------------------------------------------*/   
        startReadingRequest = true;
        /* Always reading full request */
        if (m_BytesInRequestBuffer >= sizeof (request)) 
        {
            SAPDB_MemCopyNoCheck (&request, m_pRequestBuffer + numBytesReadFromBuffer, sizeof (request));
            numBytesReadFromBuffer += sizeof (request);
            readFullRequest = true;

            /* Deleting request from buffer and copy remaining content of the buffer to the buttom */
            m_BytesInRequestBuffer -= numBytesReadFromBuffer;
            SAPDB_MemMoveNoCheck (m_pRequestBuffer, m_pRequestBuffer + numBytesReadFromBuffer, m_BytesInRequestBuffer);
            memset (m_pRequestBuffer + m_BytesInRequestBuffer, 0, RTE_CONSOLE_FIFO_REQUEST_BUFFER_SIZE - m_BytesInRequestBuffer);
        }
    }   
    while (!readFullRequest);

    return (true);
}

/***************************************************************/
#endif
/***************************************************************/
