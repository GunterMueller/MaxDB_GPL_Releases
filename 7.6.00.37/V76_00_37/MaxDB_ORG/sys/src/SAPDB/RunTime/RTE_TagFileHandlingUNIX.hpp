/*!========================================================================
  @ingroup      Runtime
  @file         RTE_TagFileHandlingUNIX.hpp
  @author       StefanP
  @brief        Functions handling the Tag Files for the IPC resources (UNIX only)

  @since        2003-10-10  19:19
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


#ifndef RTE_TAGFILEHANDLINGUNIX_HPP
#define RTE_TAGFILEHANDLINGUNIX_HPP


#if !defined (_WIN32)
/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "SAPDBCommon/SAPDB_ToString.hpp"
#include    "RunTime/RTE_MessageList.hpp"


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
  brief        RTE_UserType
               Describes the user type of the resource (client or kernel)
 */

typedef enum {UndefUser = 0,
              ClientUser,
              KernelUser
} RTE_UserType;

/*!
  brief        RTE_ResourceType
               Describes the different kinds of ipc or synchronisation objects
               (semaphores, shared memory, mailslot,...)
 */

typedef enum {UndefResource = 0,
              Semaphore,
              SharedMemory,
              Mailslot
} RTE_ResourceType;

typedef int     RTE_Handle;	///<UNIX handle
/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/
 /*!
     @brief			Build tag filename from given paramters (including the full path)
                     
     @param         path [out]				- Tag filename including the entire path
     @param         userType [in]			- User type
     @param         serverDB [in]			- Name of the server DB
     @param         resourceType [in]		- Resource type
     @param         tagID [in]				- ID of the tag file (e.g. special name or handle (converted to string))
     @param         messageList [out]		- Message list

     @return        true/false
 */

SAPDB_Bool      RTE_GetIPCFileName     (RTE_Path &                      path,
                                        RTE_UserType const              userType,
                                        RTE_DBName const                serverDB,
                                        RTE_ResourceType const          resourceType,
                                        RTE_Path const                  tagID,
                                        SAPDBErr_MessageList &          messageList);

 /*!
     @brief			Build the path name of the IPC directory
                     
     @param         path [out]						- Full path name of the IPC directory
     @param         terminateWithDelimiter [in]		- Terminate with delimeter ('/') or not (true/false)
     @param         messageList [out]				- Message list 

     @return        true/false
 */

SAPDB_Bool      RTE_GetIPCDir          (RTE_Path &                      path,
                                        SAPDB_Bool                      terminateWithDelimiter,
										SAPDBErr_MessageList &			messageList);

 /*!
     @brief			Remove a specific tag file
                     
     @param         userType [in]			- User type
     @param         serverDB [in]			- Name of the server DB
     @param         resourceType [in]		- Resource type
     @param         tagID [in]				- ID of the tag file
     @param         messageList [out]		- Message list

     @return        true/false
 */

SAPDB_Bool      RTE_RemoveIPCFile      (RTE_UserType const              userType,
                                        RTE_DBName const                serverDB,
                                        RTE_ResourceType const          resourceType,
                                        RTE_Handle const                tagID,
                                        SAPDBErr_MessageList &          messageList);

 /*!
	@brief			Convert resource types into readable strings

	@param			type [in]				- resource type
 */

inline SAPDB_ToStringClass SAPDB_ToString(RTE_ResourceType     type)
{
    switch(type)
    {
    case UndefResource: 
        return SAPDB_ToStringClass("Undefined");
    case Semaphore: 
        return SAPDB_ToStringClass("Semaphore");
    case SharedMemory: 
        return SAPDB_ToStringClass("SharedMemory");
    default:                          
        break;
    }
    return SAPDB_ToStringClass((SAPDB_Int4)type); 
}

 /*!
	@brief			Convert user types into readable strings

	@param			type [in]			- user type
 */

inline SAPDB_ToStringClass SAPDB_ToString(RTE_UserType     type)
{
    switch(type)
    {
    case UndefUser: 
        return SAPDB_ToStringClass("Undefined");
    case ClientUser: 
        return SAPDB_ToStringClass("Client");
    case KernelUser: 
        return SAPDB_ToStringClass("Kernel");
    default:                          
        break;
    }
    return SAPDB_ToStringClass((SAPDB_Int4)type); 
}


#endif  /* UNIX */
#endif  /* RTE_TAGFILEHANDLINGUNIX_HPP */
