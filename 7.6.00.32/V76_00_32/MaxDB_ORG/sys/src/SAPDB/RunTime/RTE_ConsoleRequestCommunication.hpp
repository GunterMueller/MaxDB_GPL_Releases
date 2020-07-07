/*!
  @file           RTE_ConsoleRequestCommunication.hpp
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



#ifndef RTE_CONSOLEREQUESTCOMMUNICATION_HPP
#define RTE_CONSOLEREQUESTCOMMUNICATION_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
/*===========================================================================*
 *  Request FIFO                                                             *
 *===========================================================================*/
/* 
    Name of FIFO which is created by the console thread. On UNIX the FIFO is 
    located in the "...ipc/db:<DBNAME> directory                                      
*/



#if defined (_WIN32)
#define RTE_CONSOLE_FIFO_PATH                   "\\\\.\\mailslot\\SAP DBTech\\CONS\\"
#define RTE_CONSOLE_FIFO_NAME                   "FIFO-CONSOLE-"
#else //UNIX
#define RTE_CONSOLE_FIFO_NAME                   "FIFO-CONSOLE"
#define RTE_CONSOLE_FIFO_PERMISSION             (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | \
                                                 S_IROTH | S_IWOTH ) 
#endif

#define RTE_CONSOLE_FIFO_REQUEST_BUFFER_SIZE    4096

#define RTE_CONSOLE_FIFO_PROTOCOL_VERSION       1

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @class          RTE_ConsoleRequestCommunication
   @description    Common Request Console Communication


                This class implements a FIFO. It is used on the client and server 
                side to put or respectively get the console requests.
                The protocol handling is not subject of this class.

 */


class RTE_ConsoleRequestCommunication
{
public:

enum RequestRC {NoError = 0,	///<No error 
                Error,			///<Error (not specified)
                Offline 	    ///<Database is offline
               };		

  /*!
     @description    Constructor
     @param          pszServerDB [in] - Name of the DB
     @param          openMode [in] - RTE_CONSOLE_CLIENT/SERVER_MODE
     @return value   none

   */

    RTE_ConsoleRequestCommunication (SAPDB_Char const * const       pszServerDB,
                                     SAPDB_UInt4 const              openMode);

  /*!
     @description    Destructor
     @return value   none

   */


    ~RTE_ConsoleRequestCommunication ();


  /*!
     @description    Initialize the RTE_ConsoleRequestCommunication instance.


                  This method opens or respectively creates the FIFO (NT: Mailslot,
                  UNIX: Pipe).
     @param          pSA [in] - Pointer to security attributes (UNIX: NULL)
     @param          messageList [in/out] - Message list
     @return value   NoError, Error, Offline

   */


    RequestRC       Initialize      (RTE_SECURITY_ATTRIBUTES * const    pSA,
                                     SAPDBErr_MessageList &             messageList);
    
  /*!
     @description    Putting request into FIFO.
     @param          request [in] - Reference to a Console Request
     @param          messageList [in/out] - Message list
     @return value   true/false

   */

    SAPDB_Bool      PutRequest      (RTE_ConsoleRequest const &     request,
                                     SAPDBErr_MessageList &         messageList) const;

  /*!
     @description    Getting request from FIFO.


                  The call is blocking. That means if no request is actually in the FIFO
                  the call listens at the FIFO until a request is available.
     @param          request [out] - Reference to a Console Request
     @param          messageList [in/out] - Message list
     @return value   true/false

   */

#if defined (_WIN32)
    SAPDB_Bool      GetRequest      (RTE_ConsoleRequest &           request,
                                     SAPDBErr_MessageList &         messageList) const;
#else
    SAPDB_Bool      GetRequest      (RTE_ConsoleRequest &           request,
                                     SAPDBErr_MessageList &         messageList);
#endif
private:
    void            BuildFIFOName     ();

    RTE_FileHandle      m_hConsoleFIFO;
    RTE_Path            m_NameFIFO;     //used only for UNIX
    RTE_DBName          m_ServerDB;
    SAPDB_UInt4         m_OpenMode;

#if !defined(_WIN32)
    SAPDB_Byte *        m_pRequestBuffer;
    SAPDB_UInt4         m_BytesInRequestBuffer;
#endif

};

/*!
   @brief Convert return code from console request communication methods into readable string
   @param rcRequest [in] return code to convert
*/

inline SAPDB_ToStringClass SAPDB_ToString(RTE_ConsoleRequestCommunication::RequestRC     rcRequest)
{
    switch (rcRequest)
    {
    case RTE_ConsoleRequestCommunication::NoError: 
        return SAPDB_ToStringClass("NoError");
    case RTE_ConsoleRequestCommunication::Error: 
        return SAPDB_ToStringClass("Error");
    case RTE_ConsoleRequestCommunication::Offline: 
        return SAPDB_ToStringClass("Offline");
    default:                          
        break;
    }

    return SAPDB_ToStringClass((SAPDB_Int4)rcRequest); 
}


#endif  /* RTE_CONSOLEREQUESTCOMMUNICATION_HPP */
