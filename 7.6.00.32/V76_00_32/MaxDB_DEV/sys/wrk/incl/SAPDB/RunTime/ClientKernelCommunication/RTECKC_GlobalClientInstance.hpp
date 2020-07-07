/*!
  @defgroup     CKC     Client Kernel Communication
*/
/*!========================================================================
  @ingroup      CKC
  @file         RTECKC_GlobalClientInstance.hpp
  @author       StefanP
  @brief        Global Client Instance

  @since        2003-09-16  19:04
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


#ifndef RTECKC_GLOBALCLIENTINSTANCE_HPP
#define RTECKC_GLOBALCLIENTINSTANCE_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "RunTime/ClientKernelCommunication/RTECKC_ClientCommunication.hpp"
#include    "RunTime/RTE_MessageList.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Topic.hpp"
#include    "SAPDBCommon/Tracing/SAPDBTrace_Usage.hpp"


extern SAPDBTrace_Topic CKC_Trace;///<CKC Trace

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
    @class	RTECKC_GlobalClientInstance
    @brief  The instance of this singleton class administers the global items common for all 
			connections.
*/

class RTECKC_GlobalClientInstance
{
public:

  /*!
     @brief			Return the reference to the single instance of RTECKC_GlobalClientInstance.

					The first call creates the single instance of the class. Thereby the ID
					generator is initialized (cf. RTE_UniqueId.h).
  */

    static  RTECKC_GlobalClientInstance &   Instance ();

  /*!
     @brief          Destructor - All connections are closed
  */

    ~RTECKC_GlobalClientInstance    ();

  /*!
     @brief			Try to close all open connections

     @param         messageList [out]		- Message list

     @return        true/false
  */

    SAPDB_Bool      CloseAllConnections     (SAPDBErr_MessageList &         messageList);


  /*!
     @brief			Get the handle manager
					(Background:	Each connection possesses an unique handle which is administered
									by the handle manager.)
  */

    RTE_HandleManager & GetHandleManager    ()
    {
        SAPDBTRACE_METHOD_DEBUG ("RTECKC_GlobalClientInstance::GetHandleManager", CKC_Trace, 9);
        return m_HandleManager;
    }

 

private:

#define    MAX_WAITING_PERIOD_FOR_CONNECTION_CLOSE            2000   ///<Total wait time for all connections to be closed (in milliseconds)
#define    CLOSE_REPEAT_INTERVAL                              100    ///<Lookup interval for all connections to be closed (in milliseconds)    

  /*!
     @brief         Constructor
					The Constructor is privat to this class to avoid instantiation outside 
					of the class.
  */

    RTECKC_GlobalClientInstance   ()
    {
        SAPDBTRACE_METHOD_DEBUG ("RTECKC_GlobalClientInstance::RTECKC_GlobalClientInstance", CKC_Trace, 9);
    }


    static  RTECKC_GlobalClientInstance *     m_Instance; 


    RTE_HandleManager       m_HandleManager;
}; 




#endif  /* RTECKC_GLOBALCLIENTINSTANCE_HPP */
