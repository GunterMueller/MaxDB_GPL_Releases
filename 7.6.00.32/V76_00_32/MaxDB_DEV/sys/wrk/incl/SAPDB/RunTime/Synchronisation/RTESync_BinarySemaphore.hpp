/*!
  @defgroup     Sync     Synchronisation
*/

/*!========================================================================
  @ingroup      Sync  
  @file         RTESync_BinarySemaphore.hpp
  @author       StefanP

  @brief        Semaphore class for non-counting semaphores (also called binary semaphores or Events
                                                             respectively)

  @since        2003-11-13  15:03
  @sa           

  =======================================================================

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
  ----------------------------------------------------------------------------*/


#ifndef RTESYNC_BINARYSEMAPHORE_HPP
#define RTESYNC_BINARYSEMAPHORE_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#if !defined (_WIN32) && defined(_REENTRANT)
#include <pthread.h>
#endif

#include    "RunTime/RTE_MessageList.hpp"
#include    "SAPDBCommon/SAPDB_ToString.hpp"
#include    "RunTime/RTE_TagFileHandlingUNIX.hpp" //UNIX only

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/
#if defined (_WIN32)
#define     RTE_INFINITE_WAIT   INFINITE			///<Wait time infinite  
#else
#define     RTE_INFINITE_WAIT   (SAPDB_ULong)(-1)   ///<Wait time infinite  
#endif

#if defined(_WIN32)
#define RTE_SYNC_UNDEF_SEM_ID        NULL						///<Undefined semaphore ID (for initialization and unnamed semaphores)
#else
#define RTE_SYNC_UNDEF_SEM_ID        ((RTESync_SemaphoreID)-1)	///<Undefined semaphore ID (for initialization)
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


#if defined(_WIN32)
    typedef SAPDB_Char *        RTESync_SemaphoreID;	///<Type definition for the semaphore ID
#else
    typedef int                 RTESync_SemaphoreID;	///<Type definition for the semaphore ID
#endif



/*!
    @class    RTESync_BinarySemaphore
    @brief    Semaphore class for non-counting semaphores (also called binary semaphores or Events
                                                                 respectively)
                    
              Semaphore objects can be shared by different processes on UNIX. On NT it depends
              on a identifier wich can be given to the the OPEN method. If no identifier is given (NULL)
              the semaphore object is unnamed and can only be used in the creating process.

              This class offers the basic methods for using binary semaphores such as WAIT and POST
              methods.

              At construction time one has to determine the open mode of the semaphore object. 
              One can open a new object as well as an existing object. The constructor does not open 
              the semaphore object. This is first done by a call to the OPEN method! 
 */

class   RTESync_BinarySemaphore
{
public:

enum SemRC {NoError = 0,	///<No error 
            Error,			///<Error (not specified)
            AlreadyExist,	///<Semaphore already exist
            Timeout,		///<Timeout occurred
            Removed			///<Semaphore does not exist anymore
		   };		


#if !defined (_WIN32)
enum EventType		{UndefEvent = 0,	///<Event tyope is undefined
					 PThreadMutex,		///<PThread Mutex with Condition variable
					 IPCSemaphore		///<IPC Semaphore
					};	
#endif


#if !defined (_WIN32) // && defined (_REENTRANT)

enum EventState	{Unposted = 0,	///<Semaphore is an unposted state
				 Posted,		///<Semaphore is a posted state
				 Waiting		///<Wait for semaphore post
				};	

//#if !defined (LINUX)
struct EventUNIX
{
	EventState			state;
#if defined(_REENTRANT)
    pthread_mutex_t     mutex;
    pthread_cond_t      condition;
#endif
};
//#else
	//Futexes
//#endif
#endif



  /*!
     @brief			Constructor
     @return        none
  */
    RTESync_BinarySemaphore         ();

  /*!
     @brief			Destructor
     @return        none
  */

    ~RTESync_BinarySemaphore        ();



#if defined (_WIN32)
  /*!
     @brief			Create a semaphore. After creation the state of the semaphore is
                    cleared (nonsignaled)
                     
     @param         pSemID [in]            - Name of the semaphore object or NULL (unnamed semaphore)
											 Of course an unnamed semaphore can not be opened (attached) afterwards												 
     @param         secAttribute [in]      - Reference of the security attribute for creating the semaphore.
     @param         messageList [out]	   - Message list

     @return        NoError, Error, AlreadyExist
  */

    SemRC	Create      (RTESync_SemaphoreID const          pSemID, 
                         RTE_SECURITY_ATTRIBUTES  &         secAttribute,   
                         SAPDBErr_MessageList &             messageList);

  /*!
     @brief			Open a semaphore 
                     
     @param         semID [in]				- ID of an existing semaphore object to be opened
     @param         messageList [out]		- Message list

     @return        NoError, Error
  */

    SemRC	Open		(RTESync_SemaphoreID const          semID,          
                         SAPDBErr_MessageList &             messageList);

#else

//#if defined (_REENTRANT)

  /*!
     @brief			Create a semaphore as pthread mutex with condition variable. 
					Those semaphores support timeouts.
					After creation the state of the semaphore is cleared (nonsignaled)
                     
     @param         pEventUNIX [in]		- Memory location for the semaphore structure (e.g. inside
										  of a shared memory) or NULL
										  (if NULL is given the memory for the semaphore structure
										  is allocated internally. This is only possible for
										  scope == PTHREAD_PROCESS_PRIVATE)
     @param         scope [in]			- PTHREAD_PROCESS_SHARED (system scope)
										  or PTHREAD_PROCESS_PRIVATE (provess scope)

										  !!! Linux supports only PTHREAD_PROCESS_PRIVATE !!!

     @param         messageList [out]	- Message list

     @return        NoError, Error
  */

    SemRC	Create      (EventUNIX * const					pEventUNIX,      
						 SAPDB_Int							scope,   
                         SAPDBErr_MessageList &             messageList);

  /*!
     @brief			Open a semaphore as pthread mutex with condition variable. 
					Those semaphores support timeouts. 
                     
     @param         pEventUNIX [in]			- Pointer to an existing semaphore structure
     @param         messageList [out]		- Message list

     @return        NoError, Error
  */


	SemRC	Open		(EventUNIX * const					pEventUNIX,
						 SAPDBErr_MessageList &				messageList);


//#endif

  /*!
     @brief			Create an IPC semaphore. After creation the state of the semaphore is
                    cleared (nonsignaled)
					No timeout support yet!
                     
     @param         userType [in]			- Type of user (ClientUser, KernelUser)
     @param         serverDB [in]			- Database name
     @param         secAttribute [in]		- Reference of the security attribute for creating the semaphore.
  										      !!!Caution: The umask is not considered. Only read and write (alter)
											  access rights are supported. Execute rights for IPC objects are not supported.
     @param         messageList [out]		- Message list

     @return        NoError, Error
  */

    SemRC	Create		(RTE_UserType const                 userType,       
                         RTE_DBName const                   serverDB,       
                         RTE_SECURITY_ATTRIBUTES  &         secAttribute,   
                         SAPDBErr_MessageList &             messageList);

  /*!
     @brief			Open an IPC semaphore.
					No timeout support yet!
                          
     @param         semID [in]				- ID of an existing semaphore object to be opened
     @param         userType [in]			- Type of user (ClientUser, KernelUser)
     @param         serverDB [in]			- Database name
     @param         messageList [out]		- Message list


     @return        NoError, Error
  */

    SemRC   Open		(RTESync_SemaphoreID const          semID,          
                         RTE_UserType const                 userType,       
                         RTE_DBName const                   serverDB,       
                         SAPDBErr_MessageList &             messageList);

#endif

  /*!
     @brief			Post the semaphore (set the object state to signaled) 
                     
     @param         messageList [out]		- Message list

     @return        NoError, Error
                     

  */

                                     
    SemRC    Post       (SAPDBErr_MessageList &         messageList) const;


  /*!
     @brief			Wait for semaphore to be posted (signaled). If the semaphore is signaled the state 
                    is cleared by this function (set to nonsignaled state). 
                    Of course awakening from the wait state and clearing the semaphore is processed as an 
                    atomic instruction.
                     
     @param         semaphoreTimeout [in]   - Timeout value in milliseconds (NT only at present) 
     @param         messageList [out]		- Message list

     @return        NoError, Error, Timeout 

  */

    SemRC    Wait       (SAPDB_ULong const              semaphoreTimeout,
                         SAPDBErr_MessageList &         messageList) const;

  /*!
     @brief			Close semaphore 
                     
     @param         messageList [out]		- Message list

     @return        NoError, Error 

  */

    SemRC    Close      (SAPDBErr_MessageList &         messageList);


  /*!
     @brief			Return the semaphore ID (or RTE_SYNC_UNDEF_SEM_ID for unnamed semaphores on NT)  
  */

    RTESync_SemaphoreID  GetSemaphoreID     () 
        const
        {
            return m_SemID;
        }


  /*!
     @brief			Determines if the semaphore is already opened 
  */

    SAPDB_Bool  IsOpen     () const; 

private:
  /*!
     @brief			Prevent calling the copy constructor from outside
  */
	inline   RTESync_BinarySemaphore  (const RTESync_BinarySemaphore & sem) {;};

#if !defined (_WIN32)
	RTE_Path                m_TagFileName;			///<Is only to be used for IPC Semaphores
	EventType				m_EventType;
//#if defined (_REENTRANT)
	EventUNIX *				m_pEventUNIX;			///<Is only to be used for PThread Mutexe
	SAPDB_Bool				m_OwnSemaBuffer;		///<Is only to be used for PThread Mutexe
//#endif

#endif

    RTESync_SemaphoreID     m_SemID;        ///<NT: Name, 
                                            ///<UNIX: semaphore identifier; Is only to be used for IPC Semaphores 
    RTE_SemaphoreHandle     m_Handle;       ///<NT: Handle to semaphore object
                                            ///<UNIX: SemID == Handle; Is only to be used for IPC Semaphores 

};

/*!
   @brief Convert return code from semaphore methods into readable string
   @param rcSem [in] return code to convert
*/

inline SAPDB_ToStringClass SAPDB_ToString(RTESync_BinarySemaphore::SemRC     rcSem)
{
    switch (rcSem)
    {
    case RTESync_BinarySemaphore::NoError: 
        return SAPDB_ToStringClass("NOERROR");
    case RTESync_BinarySemaphore::Error: 
        return SAPDB_ToStringClass("ERROR");
    case RTESync_BinarySemaphore::AlreadyExist: 
        return SAPDB_ToStringClass("ALREADYEXIST");
    case RTESync_BinarySemaphore::Timeout: 
        return SAPDB_ToStringClass("TIMEOUT");
    case RTESync_BinarySemaphore::Removed: 
        return SAPDB_ToStringClass("REMOVED");
    default:                          
        break;
    }

    return SAPDB_ToStringClass((SAPDB_Int4)rcSem); 
}

#if !defined (_WIN32) && defined (_RENTRANT)
/*!
   @brief Convert semaphore state into readable string
   @param state [in] return code to convert
*/

inline SAPDB_ToStringClass SAPDB_ToString(RTESync_BinarySemaphore::EventState     state)
{
    switch (state)
    {
    case RTESync_BinarySemaphore::Unposted: 
        return SAPDB_ToStringClass("Unposted");
    case RTESync_BinarySemaphore::Posted: 
        return SAPDB_ToStringClass("Posted");
    case RTESync_BinarySemaphore::Waiting: 
        return SAPDB_ToStringClass("Waiting");
    default:                          
        break;
    }

    return SAPDB_ToStringClass((SAPDB_Int4)state); 
}

#endif

#endif  /* RTESYNC_BINARYSEMAPHORE_HPP */
