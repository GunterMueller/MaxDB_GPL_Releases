/*!========================================================================

  @file         RTECKC_KernelCommunication.hpp
  @ingroup      Runtime
  @author       RaymondR

  @brief        Used to reference all global data of the 
                kernel communication

  @since        2005-09-22  12:23
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (C) 2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end
  \endif
 ============================================================================*/


#ifndef RTECKC_KERNELCOMMUNICATION_HPP
#define RTECKC_KERNELCOMMUNICATION_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "RunTime/Tasking/RTETask_UnconnectedTaskPool.hpp"
#include "RunTime/RTE_KGS.hpp"
#include "RunTime/Synchronisation/RTESync_InterlockedCounter.hpp"

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
  @class   RTECKC_KernelCommunication
  @brief   Kernel communication object

           Used to reference all global information of the 
           kernel communication.

 ----------------------------------------------------------------------------*/

class RTECKC_KernelCommunication
{
public:
    /*!
      @brief         constructor.

                     Build up a kernel communication object.

      @param strategy                [in] - the connect strategy to be used
      @param searchUnrelConnCallback [in] - callback to allow release of laisy released tasks
    */
    RTECKC_KernelCommunication ( RTETask_UnconnectedTaskPool::Strategy   connectStrategy,
                                 searchUnreleasedConnectionsCallback     searchUnrelConnCallback )
                                 : m_KGS(RTE_KGS::Instance()),  // set shortcut to kgs
                                   m_UnconnectedTaskPool( connectStrategy, searchUnrelConnCallback ),
                                   m_ActiveCnt(0),
                                   m_SendTime(0) {;}

    /*!
      @brief  Get the pointer to the unconnected task pool

      @return  Pointer to the unconnected task pool
     */
    RTETask_UnconnectedTaskPool& UnconnectedTaskPool()
    {
        return m_UnconnectedTaskPool;
    }

    /*!
      @brief  Signal that the connection is active and not in command wait state
     */
    void ConnectionIsActiveAndNotInCommandWaitState()
    {
        m_ActiveCnt.Increment();
    }

    /*!
      @brief  Signal that the connection is in command wait state or inactive
     */
    void ConnectionIsInactiveOrInCommandWaitState()
    {
        m_ActiveCnt.Decrement();
    }


    /*!
      @brief  Update the time of the last received command
     */
    void UpdateLastSendTime()
    {
        m_SendTime.SetValue(m_KGS.CurrTime());
    }

    /*!
      @brief  Get the elapsed time since the last command 
              has been executed. 

      @return  elapsed time in seconds
     */
    SAPDB_ULong TimeElapsedSinceLastCommandExecution( bool ignoreMyself ) const
    {
        // command execution in progress ?
        return  m_ActiveCnt.GetValue() > (ignoreMyself ? 1:0) ?  0 : m_KGS.CurrTime() - m_SendTime.GetValue();
    }


private:
    // private, don't allow copies
    RTECKC_KernelCommunication( RTECKC_KernelCommunication &c )
        : m_UnconnectedTaskPool( RTETask_UnconnectedTaskPool::Equalize, 0 ),
          m_KGS(RTE_KGS::Instance()) {;}

    // private, don't allow copies
    RTECKC_KernelCommunication & operator=(const RTECKC_KernelCommunication& c ) { return *this;}

private:
    RTETask_UnconnectedTaskPool                m_UnconnectedTaskPool; //!< Pool of unconnected tasks
    RTE_KGS &                                  m_KGS;                 //!< Shortcut to kgs
    RTESync_InterlockedCounter<SAPDB_Long>     m_ActiveCnt;           //!< Communication active count
    RTESync_InterlockedCounter<SAPDB_ULong>    m_SendTime;            //!< Time of the last replyed command
};


#endif  /* RTECKC_KERNELCOMMUNICATION_HPP */
