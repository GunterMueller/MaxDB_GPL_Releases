/*!========================================================================

  @ingroup      CKC
  @file         RTECKC_Request.hpp
  @author       StefanP
  @brief        Requests used by the request channel between client applications
                and the kernel requestor for the local Client Kernel Communication.

  @since        2004-08-23  15:43
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


#ifndef RTECKC_REQUEST_HPP
#define RTECKC_REQUEST_HPP

#if defined (LINUX) && !defined(HAS_NPTL) && !defined(HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE)
#define HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE
#endif

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "RunTime/ClientKernelCommunication/RTECKC_ClientCommunication.hpp"
#include    "RunTime/Synchronisation/RTESync_BinarySemaphore.hpp"
#include    "RunTime/IPC/RTEIPC_CommunicationObject.hpp"
#include    "SAPDBCommon/SAPDB_ToString.hpp"
#include    "RunTime/Communication/RTEComm_ParseURI.hpp"
#include    "RunTime/Communication/RTEComm_URIBuilder.hpp"
#include    "RunTime/RTE_MessageList.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"


extern SAPDBTrace_Topic CKC_Trace;	///<CKC Trace
/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/



/*!
    @class	RTECKC_Request
    @brief  This class administers the requests send over the request channel
			from client application to the kernel requestor.

			Requests are loaded from a given communication object (such as a
			mailslot or pipe) into the internal administered request buffer. Also it 
			dumps this request buffer on demand into a given communication object.

			The memory for the request buffer is allocated by the instances of the class,
*/

class RTECKC_Request
{

private:

  /*!
     @brief     Structure of the connect buffer
  */

    typedef struct  { 
        SAPDB_UInt4     size;		///<Size of the request
        SAPDB_UInt4     offsetURI;  ///<equals sizeof(Request) currently               
    } Request;

public:

    enum RequestType  {Undef = 0,	///<Undefined request
                       Connect,		///<Connect request
                       Cancel,		///<Cancel request
                       Dump			///<Dump request
	};
 
  /*!
     @brief			Constructor

     @param         maxRequestSize [in]		- Maximal allowed size for a request (in bytes)
  */	

    RTECKC_Request			(SAPDB_UInt4    maxRequestSize)
        :   m_MaxRequestSize (maxRequestSize),
            m_pRequest (NULL),
            m_RequestType (Undef)
        {
            SAPDBTRACE_METHOD_DEBUG ("RTECKC_Request::RTECKC_Request", CKC_Trace, 9);
        }

  /*!
     @brief			Destructor
  */

    ~RTECKC_Request         ()
        {
            SAPDBTRACE_METHOD_DEBUG ("RTECKC_Request::~RTECKC_Request", CKC_Trace, 9);
            if (NULL != m_pRequest)
            {
                RTEMem_RteAllocator::Instance().Deallocate(m_pRequest);
                m_pRequest = NULL;
            }
        }


  /*!
     @brief			Dump the request to the given communication object (e.g. mailslot, pipe)
                    
     @param         comObject [in]			- Communication object
     @param         messageList [out]		- Message list

     @return        NoError, Error, ResourceMissing
  */

    RTEIPC_CommunicationObject::CommObjRC            
                        DumpRequest         (RTEIPC_CommunicationObject &       comObject,
                                             SAPDBErr_MessageList &             messageList) const;


  /*!
     @brief			Build a connect or reconnect request
                    
     @param         commSegID [in]			- Name of the communication segment 
     @param         clientSemID [in]		- ID of the client semaphore
											  (not needed for UNIX (not Linux) if 'TIMEOUT' is specified, 
											  because in that case the semaphore is located in the 
											  communication segment header) 
     @param         uniqueID [in]			- Unique ID (used for building an unique Name for
											  communication segment as well as the client semaphore
											  (NT only)
     @param         commSegSize [in]		- Size of the communication segment
     @param         pDBPath [in]			- URI database path part. This may be a normal
											  connect (e.g. for classic client connections) 
											  or a reconnect to a existing session 
											  (e.g. for UDE-Server)
											  (cf. URI specification)
     @param         messageList [out]		- Message list

     @return        true/false
  */

    SAPDB_Bool          PutConnectRequest   (SAPDB_Char const * const       commSegID, 
#if defined (_WIN32) || defined (HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE) || !defined (_REENTRANT)
                                             RTESync_SemaphoreID const      clientSemID,        
#endif
                                             SAPDB_Char const * const       uniqueID,
                                             SAPDB_UInt4 const              commSegSize, 
                                             SAPDB_Char const * const       pDBPath,
                                             SAPDBErr_MessageList &         messageList);

  /*!
     @brief			Build a cancel request 
                    
     @param         pDBPath [in]			- URI database path part to the session to be canceled.
											  Especially the session type and ID must be contained
											  in the path (cf. URI specification).
     @param         messageList [out]		- Message list

     @return        true/false
  */

    SAPDB_Bool          PutCancelRequest    (SAPDB_Char const * const       pDBPath,
                                             SAPDBErr_MessageList &         messageList);

  /*!
     @brief			Build a dump request. (dump request aborts implicitly the database!)
                    
     @param         pDBPath [in]			- URI database path part to the database to be 
											  aborted (a dump is generated by that).
     @param         messageList [out]		- Message list

     @return        true/false
  */

    SAPDB_Bool          PutDumpRequest      (SAPDB_Char const * const       pDBPath,
                                             SAPDBErr_MessageList &         messageList);

  /*!
     @brief			Load request from the given communication object (e.g. mailslot, pipe)

     @param         comObject [in]			- Communication object
     @param         messageList [out]		- Message list

     @return        true/false
  */

    SAPDB_Bool          LoadRequest         (RTEIPC_CommunicationObject &       comObject,
                                             SAPDBErr_MessageList &             messageList);

  /*!
     @brief			Determine if the actual loaded request is a connect request
                    
     @return        true/false	
  */

    SAPDB_Bool			IsConnect			()
        {
            SAPDBTRACE_METHOD_DEBUG ("RTECKC_Request::IsConnect", CKC_Trace, 9);
    
            return (Connect == m_RequestType);
        }

  /*!
     @brief			Determine if the actual loaded request is a cancel request
                    
     @return        true/false	
  */

    SAPDB_Bool			IsCancel			()
        {
            SAPDBTRACE_METHOD_DEBUG ("RTECKC_Request::IsCancel", CKC_Trace, 9);
            
            return (Cancel == m_RequestType);
        }

  /*!
     @brief			Determine if the actual loaded request is a dump request
                    
     @return        true/false	
  */

    SAPDB_Bool			IsDump				()
        {
            SAPDBTRACE_METHOD_DEBUG ("RTECKC_Request::IsDump", CKC_Trace, 9);

            return (Dump == m_RequestType);
        }


  /*!
     @brief			Get the parameters of the actual connect or reconnect request.
					(This method may be called after it was checked that the actual loaded
					request is a connect request.) 

     @param         commSegID [out]			- Pointer to the name of the communication segment 
     @param         clientSemID [out]		- ID of the client semaphore if any. If no ID is found the paramater is
											  left unchanged.
											  (for (UNIX && !HAS_NO_PTHREAD_MUTEX_PROCESS_SCOPE) the event is located in the communication segment itself)
     @param         uniqueID [out]			- Pointer to the unique ID (used for building an unique Name for the 
											  communication segment as well as the client semaphore
											  (NT only)
     @param         commSegSize [out]		- The size of the communication segment
     @param         pSessionTyp [out]		- Pointer to the session typ (cf. URI specification: 'session', 'user',
											  'event',...
											  (NULL if no type was found. In that case the function also
											  returns 'false' because a session type has always to be specified.)
     @param         pSessionID [out]		- Pointer to the session ID. 
											  Pointer == NULL: Connect request
											  Pointer != NULL: Reconnect request
     @param         messageList [out]		- Message list

     @return        true:	No error
					false:	Error occurred
  */

    SAPDB_Bool			GetConnectRequest   (SAPDB_Char const * &				commSegID, 
											 RTESync_SemaphoreID &				clientSemID,        
											 SAPDB_Char const * &				uniqueID,
											 SAPDB_UInt4 &						commSegSize, 
											 SAPDB_Char const * &				pSessionTyp,
											 SAPDB_Char const * &				pSessionID,
											 SAPDBErr_MessageList &				messageList) const;

  /*!
     @brief			Get the parameters of the actual cancel request.
					(This method may be called after it was checked that the actual loaded
					request is a cancel request.) 

     @param         pSessionTyp [out]		- Pointer to the session typ (cf. URI specification: 'session', 'user',
											  'event',...
											  (NULL if no type was found. In that case the function also
											  returns 'false' because a session type has always to be specified.)
     @param         pSessionID [out]		- Pointer to the session ID. 
											  (NULL if no ID was found. In that case the function also
											  returns 'false' because a session ID has always to be specified for
											  a cancel request.)
     @param         messageList [out]		- Message list

     @return        true:	No error
					false:	Error occurred
  */

    SAPDB_Bool			GetCancelRequest    (SAPDB_Char const * &           pSessionTyp,
											 SAPDB_Char const * &           pSessionID,
											 SAPDBErr_MessageList &         messageList) const;

  /*!
     @brief			Returns the request URI in the request buffer with the given
					database path as well as the request type and options.

     @return        reference to request URI
    */
    RTEComm_ParseURI const  &GetRequestURI () const  { return m_parseURIRequest; }

private:

  /*!
     @brief			Initialize an instance of this class. The attributes are initialized 
					by default values and the request buffer is allocated if not already done.

					This method is also used as a kind of a reinit method.
                    
     @param         messageList [out]		- Message list

     @return        true/false
  */

    SAPDB_Bool			Init				(SAPDBErr_MessageList &         messageList);


  /*!
     @brief			Initialize the request URI in the request buffer with the given
					database path as well as the request type. Further options are to
					be added as needed afterwards.
                    
     @param         pDBPath [in]			- URI database path part to the database to be 
											  aborted (a dump is generated by that).

     @param         type [in]				- Request type (Connect, Cancel,...)

     @param         messageList [out]		- Message list

     @return        true/false
  */

    SAPDB_Bool			InitRequestURI		(SAPDB_Char const * const       pDBPath, 
											 RequestType                    type,
											 SAPDBErr_MessageList &         messageList);


	///<Options to the request URI's:
    #define     OPTION_TYPE             (SAPDB_UTF8*)"Type"				///<Request type
    #define     OPTION_COMMSEGID        (SAPDB_UTF8*)"CommSegID"		///<ID of the communication segment
    #define     OPTION_CLIENTSEMID      (SAPDB_UTF8*)"ClientSemID"		///<ID of the client semaphore
    #define     OPTION_UNIQUEOBJID      (SAPDB_UTF8*)"UniqueObjectID"	///<Unique object ID
    #define     OPTION_COMMSEGSIZE      (SAPDB_UTF8*)"CommSegSize"		///<Size of the communication segment

    Request *                   m_pRequest;
    SAPDB_UInt4                 m_MaxRequestSize;
    RTEComm_ParseURI            m_parseURIRequest;
    RequestType                 m_RequestType;
    RTEComm_URIBuilder          m_URIBuilder;
};


#endif  /* RTECKC_REQUEST_HPP */
