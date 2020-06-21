/*!
  @file           RTE_PendingEventHandler.hpp
  @author         JoergM
  @ingroup        RunTime
  @brief          Class for registration of pending envents
  
\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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

#ifndef RTE_PENDINGEVENTHANDLER_HPP
#define RTE_PENDINGEVENTHANDLER_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gsp00.h"
#include "vsp0031.h"
#include "SAPDBCommon/SAPDB_Types.h"
#include "RunTime/Threading/RTEThread_KernelThread.hpp"
#include "RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

/*! Default one every minute */
#define RTE_PENDING_EVENT_INTERVAL_DEFAULT 60

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class RTE_PendingEventList;

/*!
  @class RTE_PendingEventHandler
  @brief Singleton for registration of pending events

  Each pending event is given its own time interval which is used to reinsert it into the event queue.
  The registration automatically leads to an event insert. The deregistration does not create an event.
 */
class RTE_PendingEventHandler  : public RTEThread_KernelThread
{
public:
    /*!
      @brief singleton instance access
      @return reference of this singleton
     */
    static RTE_PendingEventHandler & Instance();

    /*!
     @brief register given event

     copy the given event and register it in the pending event list. Use given time interval
     to reissue the event until it is deregistered. Every event type can be registered only once.
     If the event is already registered, only the reissue_interval is updated.

     A value of 0 for reissue time interval uses RTE_PENDING_EVENT_INTERVAL_DEFAULT!

     @param event [in] the event to send
     @param reissue_interval_in_seconds [in] number of seconds between two events (Default = 0)
     */
    void Register( tsp31_event_description const &event
                 , SAPDB_UInt4 reissue_interval_in_seconds=0 );

    /*!
     @brief deregister given event using event iden identifier

     search for given event and deregister it from the pending event list.
 
     @param eventIdentifier [in] the event identifier of the event to deregister
     */
    void Deregister( tsp31_event_ident_Param const eventIdentifier );

private:
    /*!
      @brief private ctor
     */
    RTE_PendingEventHandler();

    /*!
    @description    The created thread runs the code of the methode 'ThreadMain'
    */
    virtual SAPDB_Int4  KernelThreadMain();


   /*!
    @description    Destroy object
                    Nothing to do here because the thread will never return!
    */
   virtual void  DestroyObject() {}


    RTE_PendingEventList*           m_pList;    //!< the pending event list
    static RTE_PendingEventHandler* m_Instance; //!< the singleton instance
}; /*! @endclass RTE_PendingEventHandler */

#endif  /* RTE_PENDINGEVENTHANDLER_HPP */
