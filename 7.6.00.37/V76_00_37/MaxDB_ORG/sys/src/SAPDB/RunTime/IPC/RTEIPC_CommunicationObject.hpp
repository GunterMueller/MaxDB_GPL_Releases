/*!
  @defgroup     IPC     Interprocess Communication
*/

/*!========================================================================
  @ingroup      IPC  
  @file         RTEIPC_CommunicationObject.hpp
  @author       StefanP

  @brief        Interface for Communication Objects such as Pipes, Sockets, 
                Shared Memory,...

  @since        2003-10-10  18:02
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


#ifndef RTEIPC_COMMUNICATIONOBJECT_HPP
#define RTEIPC_COMMUNICATIONOBJECT_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "RunTime/RTE_MessageList.hpp"
#include    "SAPDBCommon/SAPDB_ToString.hpp"


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
    @class          RTEIPC_CommunicationObject
    @brief			Interface class which can be used as a base class for different kinds of 
					communication objects (e.g. mailslots, shared memory) 

                    Offers pure virtual methods for read/write data from/to the 
					communication object. Also a close method is given.
					A open method is not subject of this class.
*/


class   RTEIPC_CommunicationObject
{
public:

    enum CommObjRC {NoError = 0,		///<No error	
                    Error,				///<Error, not specified 
                    ResourceMissing		///<Communication object not found
    };

  /*!
     @brief			Close the communication object    
     @param         messageList [out]       -   Message list

     @return		true/false
  */

    virtual SAPDB_Bool    Close   (SAPDBErr_MessageList &         messageList) = 0;
    
  /*!
     @brief			Write data to the communication object

     @param         pMessage [in]           -   Pointer to the data buffer
     @param         bytesToWrite [in]       -   Number of bytes to be written
     @param         bytesWritten [out]      -   Number of bytes written
     @param         messageList [out]       -   Message list

     @return        NoError, Error, ResourceMissing
  */

    virtual CommObjRC     Write   (SAPDB_Byte const * const    pMessage,
                                   SAPDB_UInt4 &               bytesToWrite,
                                   SAPDB_UInt4 &               bytesWritten,
                                   SAPDBErr_MessageList &      messageList) = 0;

  /*!
     @brief			Read data from the communication object
                  
     @param         pMessage [out]          -   Pointer to the data buffer
     @param         bytesRead [out]         -   Number of bytes read
     @param         messageList [out]       -   Message list

     @return        NoError, Error
  */

    virtual CommObjRC     Read    (SAPDB_Byte * const          pMessage,
                                   SAPDB_UInt4 &               bytesRead,
                                   SAPDBErr_MessageList &      messageList) = 0;

  /*!
     @brief			Determine the maximum allowed length of a single message (buffer) written/read to/from
					the communication object (e.g. for mailslots there is a platform specific limit for
					a buffer to be witten atomically). 

     @return		Length
  */

    virtual SAPDB_UInt8      GetMaxAllowedObjectLength   () const = 0;

};

/*!
	@brief Convert return code from methods of the communication object class into readable string

	@param			rcComm [in] return code to convert
 */

inline SAPDB_ToStringClass SAPDB_ToString(RTEIPC_CommunicationObject::CommObjRC     rcComm)
{
    switch (rcComm)
    {
    case RTEIPC_CommunicationObject::NoError: 
        return SAPDB_ToStringClass("NOERROR");
    case RTEIPC_CommunicationObject::Error: 
        return SAPDB_ToStringClass("ERROR");
    case RTEIPC_CommunicationObject::ResourceMissing: 
        return SAPDB_ToStringClass("RESOURCEMISSING");
    default:                          
        break;
    }

    return SAPDB_ToStringClass((SAPDB_Int4)rcComm); 
}

#endif  /* RTEIPC_COMMUNICATIONOBJECT_HPP */
