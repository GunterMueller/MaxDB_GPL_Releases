/*!========================================================================

  @file         RTETask_CQE_CommunicationReceiveWaitPart.h
  @ingroup      Runtime
  @author       RaymondR

  @brief        Common queue element part (communication receive wait)

                The following class provides methodes to access
                communication receive wait part. This object is part 
                of  the common queue element. 

                This element is used as input for the communication queue
                'RTETask_CommunicationQueue' described later on. It contains 
                all informations needed to handle a tasks communication wait
                situation like timeout and packet arrival.
                

  @since        2003-10-31  14:49
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


#ifndef RTETASK_CQE_COMMUNICATIONRECEIVEWAITPART_H
#define RTETASK_CQE_COMMUNICATIONRECEIVEWAITPART_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "RunTime/RTE_Types.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!---------------------------------------------------------------------------
  @class   RTETask_CQE_CommunicationReceiveWaitPart
  @brief   Common queue element part (communication receive wait)

           This element part is used as input for the communication queue
           'RTETask_CommunicationQueue' described later on. It contains 
           all informations needed to handle a tasks communication wait
           situation like timeout and packet arrival.

           This struct is designed to be used in C and C++ coding.
           It has no constructor and must be initialized via Init() methode.

           It is used as part of a union in conjunction with the legacy 
           part of the common queue element (RTETask_CommonQueueElement).
           
 ----------------------------------------------------------------------------*/

struct RTETask_CQE_CommunicationReceiveWaitPart
{
#if defined (__cplusplus)
public:
    /*!
    @brief         Initialize this object.

                   The values must be set via set methodes.

    @param         sessionTimeOut             [in] - Session timeout value. The communication
                                                     queue uses this value for session timeout
                                                     checks. If a timout has occurred the 
                                                     common queue element to which this part 
                                                     object belongs is dequeued and a timeout
                                                     is signaled.
    @param         commFlag                   [in] - pointer to the communication flag which
                                                     is part of a shared memory segment.
                                                     The client will set this flag to true if
                                                     a communication packet has been send
                                                     by the client. The communication queue
                                                     uses this flag as criterion to dequeue the
                                                     common queue element to which this part 
                                                     object belongs and to signal that a 
                                                     communiction packet has arrived.
    */
    void Init( SAPDB_ULong    sessionTimeOut,
               SAPDB_UInt4   &commFlag )
    {
        m_SessionTimeOutOccurred   = false;
        m_SessionTimeOut           = sessionTimeOut;
        m_CommFlag                 = &commFlag;
    }


#   if !defined (_WIN32)
    /*!
    @brief         Initialize this object (old UNIX communication).

                   The values must be set via set methodes.

    @param         sessionTimeOut             [in] - Session timeout value. The communication
                                                     queue uses this value for session timeout
                                                     checks. If a timout has occurred the 
                                                     common queue element to which this part 
                                                     object belongs is dequeued and a timeout
                                                     is signaled.
    @param         commState                  [in] - Reference to communication state variable
    @param         commBrakeState             [in] - The communication state that signals a brocken
                                                     communication channel
    @param         clientFlag                 [in] - Client flag (in shared memory) which signals 
                                                     that data has arived (value eq. 1)
                   !!!!!!!!!!  OLD UNIX COMMUNICATION !!!!!!!!!!

                   Remove if not longer used!

    */
    void Init( SAPDB_ULong  sessionTimeOut,
               SAPDB_Int4   &commState,
               SAPDB_Int4   commBrakeState,
               int          &clientFlag )
    {
        m_pCommState               = &commState;
        m_CommBrakeState           = commBrakeState;
        m_pClientFlag              = &clientFlag;

        m_SessionTimeOutOccurred   = false;
        m_SessionTimeOut           = sessionTimeOut;
        m_CommFlag                 = 0;
    }
#   endif


    /*!
    @brief         Check if a session timout is occured

    Methode can be used to check if a  session tiomeout was detected.

    @return        True, if a session timeout is occured
    */
    SAPDB_Bool SessionTimeoutOccurred()  { return m_SessionTimeOutOccurred; };


protected:
    /*! @brief    The following methodes can also be used by the communication queue */
    friend class RTETask_CommunicationQueue;
    friend class RTETask_CommunicationQueueTS;

    /*! 
    @brief         Check if communication data is available 
    @param         reset  [in] - the state will be reset automatically 
    @return        returns true if data is available
    */
    SAPDB_Bool  CommunicationDataAvailable ( SAPDB_Bool  reset = true) const                  
    { 
#       if !defined (_WIN32)
         if ( 0 != m_pCommState ) /* old UNIX communication */
             return (*m_pCommState == m_CommBrakeState) || (  *m_pClientFlag == 1 );
#       endif

        if ( *m_CommFlag != 0 ) 
        {
             if (reset) *m_CommFlag = 0;
             return true;
        }

        return false;
    }

   /*! 
    @brief         Session timed out at 
    @return        session timeout value
    */
    SAPDB_ULong  SessionTimeOutAt() const 
    { 
        return m_SessionTimeOut; 
    };

    /*! 
    @brief  Signal that a timeout has occured 
    */
    void   SetSessionTimeoutOccurred()                 
    { 
        m_SessionTimeOutOccurred = true; 
    }
    
protected: /* belongs to the following members */
#endif
    /*  
     *  The C and C++ member data goes here....
     */
    SAPDB_UInt4*               m_CommFlag;
    SAPDB_ULong                m_SessionTimeOut;
    SAPDB_Bool                 m_SessionTimeOutOccurred;

#   if !defined (_WIN32)
     SAPDB_Int4*                m_pCommState;
     SAPDB_Int4                 m_CommBrakeState;
     int*                       m_pClientFlag;
#   endif
};



#endif  /* RTETASK_CQE_COMMUNICATIONRECEIVEWAITPART_H */
