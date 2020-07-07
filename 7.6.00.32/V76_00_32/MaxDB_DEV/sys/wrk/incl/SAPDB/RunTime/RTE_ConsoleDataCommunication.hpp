/*!
  @file           RTE_ConsoleDataCommunication.hpp
  @author         StefanP
  @special area   Data Communication XCons - Console Thread
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



#ifndef RTE_CONSOLEDATACOMMUNICATION_HPP
#define RTE_CONSOLEDATACOMMUNICATION_HPP

#if (defined (LINUX) && !defined(HAS_NPTL) && !defined(HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)) || defined (SUN) || defined(SOLARIS)
#define HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE
#endif

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#if !defined (_WIN32) && !defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
#include <pthread.h>
#endif

#include    "geo900.h"
#include    "RunTime/RTE_UniqueId.h"
#include    "RunTime/Communication/RTEComm_PacketHeader.h"
#include    "RunTime/RTE_ConsoleCommon.hpp"

#if defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)       /*&if ($OSSPEC = LINUX && $__HAS_NPTL != 1)  || $MACH = SUN */
#include    "RunTime/System/RTESys_Spinlock.h"  /* nocheck */
#endif                                          /*&endif*/

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#define RTE_CONSOLE_SHM_NAME_SKELETON           "SAPDBTech-CONSOLE-SHM-"
#define RTE_CONSOLE_SERVER_SEM_NAME_SKELETON    "SAPDBTech-CONSOLE-SEM-"
#define RTE_CONSOLE_CLIENT_SEM_NAME_SKELETON    "SAPDBTech-XCONS-SEM-"

#define RTE_CONSOLE_UNDEF_RES_PACKETS       0xFFFFFFFF

#define RTE_CONSOLE_SM_PROTOCOL_VERSION     1

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/
/*!
  console communication semaphore 
 */
typedef struct _RTE_ConsoleSemaphore
{
#if defined (_WIN32) 
    RTE_ConsoleSemID        semID; 
#else
#define RTE_CONSOLE_SEM_WAIT        1
#define RTE_CONSOLE_SEM_RUNNING     2
#define RTE_CONSOLE_SEM_POST        3
#define RTE_CONSOLE_SEM_TIMEOUT     4
    SAPDB_UInt4             state;
#if defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE) //does not support pthread mutex on system scope
    RTE_SemaphoreHandle      handle;
    RTE_Lock                lock;
#else
    pthread_mutex_t         mutex;
    pthread_cond_t          condition;
#endif
#endif
} RTE_ConsoleSemaphore;



/*!
  @class          RTE_ConsoleDataCommunication
  @description    Common Data Console Communication


                This class handles all the abstract data communication stuff 
                between XCons and the console thread. This includes also the 
                protocol handling of the data packets which uses the RTE Header. 
                Therefor this class separats the protocol of the pure data transfer
                from the protocol on the data level.
                This class is used on the client and server side.

 */


class RTE_ConsoleDataCommunication
{
  /*!
     @description    Structure for the shared memory used for the communication.

   */
public:

  /*!
     @description    Constructors
     @param          openMode [in]  - RTE_CONSOLE_CLIENT/SERVER_MODE
     @param          myRef [in]     - Own reference
     @param          remotePID [in] - Process ID of the remote side (only used on the server side)
     @param          remoteRef [in] - Remote reference

   */


    RTE_ConsoleDataCommunication    (SAPDB_UInt4 const      openMode,
                                     SAPDB_UInt4 const      myRef      = RTE_COMM_UNDEF_REF,
                                     RTE_OSPid   const      remotePID  = RTE_UNDEF_OSPID,
                                     SAPDB_UInt4 const      remoteRef  = RTE_COMM_UNDEF_REF);

  /*!
     @description    Destructor

   */


    ~RTE_ConsoleDataCommunication   ();


  /*!
     @description    Sending data to the remote side.


                  SendData
                  The functions handles all the work for data transport by using the RTE Header.
                  If it necessary the data are send in several packages.
                  SendDataNext
                  has to be used to send subsequent data after 'SendData'. This may be necessary 
                  if we would need very large buffer to hold all the data at once.
     @param          pData [in]     - Data to be send
     @param          size [in]      - Size of data in bytes
     @param          respType [in]  - Responds type (OPEN_REP, DATA_REP, CLOSE_REP, ALL_REP)
     @param          messageList [in/out] - Message list
     @return value   RTE_CONS_NO_ERROR/RTE_CONS_ERROR/RTE_CONS_TIMEOUT

   */


    SAPDB_UInt4 SendData            (SAPDB_Byte const * const       pData,
                                     SAPDB_UInt4 const              size,
                                     SAPDB_UInt1 const              respType,
                                     SAPDBErr_MessageList &         messageList);

    SAPDB_UInt4 SendDataNext        (SAPDB_Byte const * const       pData,
                                     SAPDB_UInt4 const              size,
                                     SAPDB_UInt1 const              respType,
                                     SAPDBErr_MessageList &         messageList);

  /*!
     @description    Getting data from the remote side.


                  GetData
                  This function has always to be called first to receive data from the 
                  remote side send by 'SendData'. If the send data do not fit in the buffer 
                  consecutive data can be received by calling
                  GetDataNext
                  has to be called first for subsequent sends initiated by 'SendDataNext'. 
                  As 'GetData' this function has to be called only once 
                  followed by subsequent calls (if necessary) to 'GetDataContinue'.
                  GetDataContinue
                  This function can be called as often as necessary after 'GetDataNext' and 'GetData'
                  to get all the data from subsequent data which were send by the remote side.
     @param          pBuffer [in] - Buffer for the data to be read
     @param          expectedType [in] - expected request/ respond type (OPEN_REQ, OPEN_REP,...)
     @param          bufferSize [in] - Size of buffer in bytes
     @param          numBytesRead [out] - number of bytes read
     @param          numBytesTotal [out] - total number of bytes send by the remote side
     @param          messageList [in/out] - Message list
     @return         RTE_CONS_NO_ERROR/RTE_CONS_ERROR/RTE_CONS_TIMEOUT
   */


    SAPDB_UInt4 GetData             (SAPDB_Byte * const             pBuffer,
                                     SAPDB_UInt1 const              expectedType,
                                     SAPDB_UInt4 const              bufferSize,
                                     SAPDB_UInt4 &                  numBytesRead,
                                     SAPDB_UInt4 &                  numBytesTotal,
                                     SAPDBErr_MessageList &         messageList);

    SAPDB_UInt4 GetDataNext         (SAPDB_Byte * const             pBuffer,
                                     SAPDB_UInt1 const              expectedType,
                                     SAPDB_UInt4 const              bufferSize,
                                     SAPDB_UInt4 &                  numBytesRead,
                                     SAPDB_UInt4 &                  numBytesTotal,
                                     SAPDBErr_MessageList &         messageList);

    SAPDB_UInt4 GetDataContinue     (SAPDB_Byte * const             pBuffer,
                                     SAPDB_UInt1 const              expectedType,
                                     SAPDB_UInt4 const              bufferSize,
                                     SAPDB_UInt4 &                  numBytesRead,
                                     SAPDB_UInt4 &                  numBytesTotal,
                                     SAPDBErr_MessageList &         messageList);

  /*!
     @description   Waiting for confirmation from remote side (WaitSem).
                    Is only be used during release of a connection.

     @return        RTE_CONS_NO_ERROR/RTE_CONS_ERROR/RTE_CONS_TIMEOUT

   */

    SAPDB_UInt4  WaitForConfirmation (SAPDBErr_MessageList &         messageList) const;

  /*!
     @description   Signal remote side (PostSem).
                    Is only be used during release of a connection.

     @return        RTE_CONS_NO_ERROR/RTE_CONS_ERROR/RTE_CONS_TIMEOUT

   */

    SAPDB_UInt4  SignalConfirmation    (SAPDBErr_MessageList &         messageList) const;  


  /*!
     @description    Supplies the remote reference (remote reference needed for the RTE Header)
     @return value   Remote reference

  */

    SAPDB_UInt4 GetRemoteRef        () const    {return m_RemoteRef;}

/*!
   @description    Supplies the name of the Server DB
   @return value   Name of Server DB

 */

    SAPDB_Char *GetDBName           ()     
        {return m_ServerDB;}

  /*!
     @description    Initialize the RTE_ConsoleDataCommunication instance.


                  This method open/creates the shared memory and the semaphores.
     @param          szServerDB [in] - Name of the server DB
     @param          pSA [in] - Pointer to security attributes (UNIX: NULL)
     @param          shmID [in] - ID for the Shared Memory
     @param          shmSize [in] - size of the communication shared memory (XCons/Console Thread)
     @param          semTimeout [in] - Timeout for the semaphores on the shared memory
     @param          messageList [in/out] - Message list
     @return value   true/false

   */


    SAPDB_Bool  Initialize          (RTE_DBName const                szServerDB,
                                     RTE_SECURITY_ATTRIBUTES * const pSA,
                                     RTE_UniqueIdBuffer const        shmID,
                                     SAPDB_UInt4                     shmSize,
                                     SAPDB_UInt4                     semTimeout,
                                     SAPDBErr_MessageList &          messageList);


  /*!
     @description    Initialize the RTE_ConsoleDataCommunication instance.


                  This method open/creates the shared memory and the semaphores.
     @param          szServerDB  [in] - Name of the server DB
     @param          pSA         [in] - Pointer to security attributes (UNIX: NULL)
     @param          shmID       [in] - ID for the Shared Memory
     @param          shmSize     [in] - size of the communication shared memory (XCons/Console Thread)
     @param          myRef       [in] - Own reference
     @param          remotePID   [in] - Process ID of the remote side (only used on the server side)
     @param          remoteRef   [in] - Remote reference
     @param          semTimeout  [in] - Timeout for the semaphores on the shared memory
     @param          messageList [in/out] - Message list
     @return value   true/false

   */
    SAPDB_Bool  Initialize          (RTE_DBName const                szServerDB,
                                     RTE_SECURITY_ATTRIBUTES * const pSA,
                                     RTE_UniqueIdBuffer const        shmID,
                                     SAPDB_UInt4                     shmSize,
                                     SAPDB_UInt4 const               myRef,
                                     RTE_OSPid   const               remotePID,
                                     SAPDB_UInt4 const               remoteRef,
                                     SAPDB_UInt4                     semTimeout,
                                     SAPDBErr_MessageList &          messageList)
    {
        m_MyRef      = myRef;
        m_RemoteRef  = remoteRef;
        m_RemotePID  = remotePID;

        return  Initialize (szServerDB, pSA, shmID, shmSize, semTimeout, messageList);
    }

  /*!
     @description    Supplies the own reference (sender reference needed for the RTE Header)
     @return value   Own reference

   */

    SAPDB_UInt4 GetMyRef            () const    {return m_MyRef;};

#if defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
  /*!
     @description    Set unique index.  for the timeout list
     @return value   
  */

    void        SetIndex            (SAPDB_UInt4 const              index)
                    {m_ThreadIndex = index;}
#endif

private:

    void        ResetPacketData     ();

//Schnittstellen sind anzupassen:
#if defined (_WIN32)
    SAPDB_Bool  CreateSem               (RTE_Filename const                 nameSkeleton,
                                         RTE_SemaphoreHandle &               hSem,
                                         RTE_ConsoleSemaphore *             pSem, 
                                         RTE_SECURITY_ATTRIBUTES * const    pSA,
                                         SAPDBErr_MessageList &             messageList) const; 

    SAPDB_UInt4 PostSem                 (RTE_SemaphoreHandle const           hSem,
                                         RTE_ConsoleSemaphore const * const pSem,
                                         SAPDBErr_MessageList &             messageList) const; 


    SAPDB_UInt4 WaitSem                 (RTE_SemaphoreHandle const           hSem,
                                         RTE_ConsoleSemaphore const * const pSem,
                                         SAPDBErr_MessageList &             messageList) const;
    
    void        RemoveSem               (RTE_SemaphoreHandle                 hSem,
                                         RTE_ConsoleSemaphore const * const pSem);
#else
    SAPDB_Char const * ConsoleError     (int const                          err) const;

    SAPDB_Bool  CreateSem               (RTE_ConsoleSemaphore *             pSem,
                                         SAPDBErr_MessageList &             messageList); 

    SAPDB_UInt4 PostSem                 (RTE_ConsoleSemaphore * const       pSem,
                                         SAPDBErr_MessageList &             messageList) const; 

    SAPDB_UInt4 WaitSem                 (RTE_ConsoleSemaphore * const       pSem,
                                         SAPDBErr_MessageList &             messageList) const;

    void        RemoveSem               (RTE_ConsoleSemaphore * const       pSem);

#endif

    void        GetShmPath              (RTE_Path                       path) const;

    SAPDB_Bool  CreateShm               (RTE_SECURITY_ATTRIBUTES * const pSA,            //NT only
                                         SAPDBErr_MessageList &         messageList);  

    void        RemoveShm               (); 

    SAPDB_Bool  CheckHeader             (SAPDB_UInt1 const              expectedType,
                                         SAPDB_UInt1 &                  reqRespType,
                                         SAPDB_UInt4 &                  numBytesTotal,
                                         SAPDB_UInt4 &                  currPacketDataLen,
                                         SAPDB_UInt4 &                  senderRef,
                                         SAPDBErr_MessageList &         messageList);

    SAPDB_UInt4 TimeoutOccurred         (SAPDBErr_MessageList &         messageList);


    SAPDB_UInt4                         m_MyRef;
    SAPDB_UInt4                         m_RemoteRef;

    RTE_DBName                          m_ServerDB;

    RTE_UniqueIdBuffer                  m_ShmID;
#if defined (_WIN32)
    RTE_SemaphoreHandle                  m_hMySem;             //NT client and server have their own      
    RTE_SemaphoreHandle                  m_hRemoteSem;         //handles for the semaphores!  
#endif

    RTE_ConsoleSemaphore *              m_pMySem;
    RTE_ConsoleSemaphore *              m_pRemoteSem;

    SAPDB_UInt4                         m_SemTimeout;
    RTEProc_Handle                      m_hRemoteSide;       //Only valid on the server side for the XCons client
    RTE_OSPid                           m_RemotePID;


   /*------------------------------------------------------------------------*
       Shared memory segment for communication between x_cons and console
       thread
     
            +----------------------------------------------------+
            |     RTE_ConsoleSemaphore (client semaphore)        |
            +----------------------------------------------------+                                 
            |     RTE_ConsoleSemaphore (server semaphore)        |   
            +----------------------------------------------------+ <- m_pShm + m_HeaderOffset  
            |     RTEComm_PacketHeaderRecord                     |   
            |     (cf. RTEComm_PacketHeader.h)                   |   
            +----------------------------------------------------+ <- m_pDataPart  
            |                ...                                 |   
            |             <Data part>                            |   
            |                ...                                 |   
            +----------------------------------------------------+   
    */

    SAPDB_Byte *                        m_pShm;
    SAPDB_UInt4                         m_HeaderOffset;
    SAPDB_Byte *                        m_pDataPart;
    SAPDB_UInt4                         m_ShmSize;
    SAPDB_UInt4                         m_MaxPacketLen;

   /*------------------------------------------------------------------------*/

    RTEComm_PacketHeader                m_header;

    RTE_FileHandle                      m_hShm;             //Only necessary for NT; dummy for UNIX
    
    SAPDB_UInt4                         m_BytesLeftPacket;
    SAPDB_UInt4                         m_NumResidualPackets;

    SAPDB_UInt4                         m_OpenMode;
#if defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
    SAPDB_UInt4                         m_ThreadIndex;      //Index of the worker thread which is working
                                                        //on the connection. Only valid while worker thread
                                                        //is working with the connection.
                                                        //Is used for a unique index in the timeout list.
#endif
};


#endif  /* RTE_CONSOLEDATACOMMUNICATION_HPP */
