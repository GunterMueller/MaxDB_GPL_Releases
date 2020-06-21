/*!========================================================================
  @file         RTECKC_KernelCommunicationSegment.hpp
  @ingroup      Client Kernel Communication
  @author       StefanP

  @brief        Communication Segment for the local Client Kernel Communication
                Kernel Interface

  @since        2003-10-30  12:40
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


#ifndef RTECKC_KERNELCOMMUNICATIONSEGMENT_HPP
#define RTECKC_KERNELCOMMUNICATIONSEGMENT_HPP



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include    "RunTime/ClientKernelCommunication/RTECKC_CommunicationSegmentHeader.hpp"
#include    "RunTime/ClientKernelCommunication/RTECKC_CommunicationSegment.hpp"
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
    @class	RTECKC_KernelCommunicationSegment          
    @brief  This class is derived from RTECKC_CommunicationSegment and inludes the 
			the kernel specific methods.
			(Cf. RTECKC_CommunicationSegment.hpp for details about the communication
			segment).
*/


class RTECKC_KernelCommunicationSegment : public RTECKC_CommunicationSegment
{
public:

  /*!
     @brief			Constructor
  */	

    RTECKC_KernelCommunicationSegment   ()
        :   RTECKC_CommunicationSegment (RTECKC_Server)
        {   
            SAPDBTRACE_METHOD_DEBUG ("RTECKC_KernelCommunicationSegment::RTECKC_KernelCommunicationSegment", 
                                     CKC_Trace, 9);
        }


  /*!
     @brief			Set the asynchronous communication state of the server to the given state.
					This method has to be called by 'foreign' threads which want to close the 
					the connection. Thereby the client is informed about the close of the connection.

					!!! No resources are destroyed by calling this method. This has to be done
					!!! by the connection itself by calling the 'RTECKC_CommunicationSegment::Close'.

     @param         state [in]			- Communication state to be set

     @return        none

  */

    inline void     AsyncClose   (CommState const                       state);


  /*!
     @brief			Write the semaphore index of the task scheduler where the session actually runs in
					into the communication segment header (due to load balancing the session (task) 
					may move to another task scheduler during runtime).
					This index is needed by the client (cf. RTECKC_ClientCommunicationSegment.hpp)

     @param         idx [in]				- Index to be set  
  */

    void			SetActualTaskSchedulerSemaphoreIdx         (SAPDB_UInt4    idx);



private:

  /*!
     @brief			Copy Constructor - By declaring the copy constructor as private we prevent that
					he can be called from outside
  */
	inline  RTECKC_KernelCommunicationSegment  (const RTECKC_KernelCommunicationSegment & commSeg)
        : RTECKC_CommunicationSegment (RTECKC_Server)
    {;};

};

/*---------------------------------------------------------------------------*/

inline void    RTECKC_KernelCommunicationSegment::AsyncClose (CommState const						state)
{
    SAPDBTRACE_METHOD_DEBUG ("RTECKC_KernelCommunicationSegment::AsyncClose", CKC_Trace, 9);

    GetHeader()->SetMyCommState (RTECKC_CommSegHeader::Asynchronous, state);

    return;
}



#endif  /* RTECKC_KERNELCOMMUNICATIONSEGMENT_HPP */

