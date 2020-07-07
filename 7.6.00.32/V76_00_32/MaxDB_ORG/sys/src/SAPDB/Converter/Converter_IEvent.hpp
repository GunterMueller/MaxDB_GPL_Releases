/*!
  @file           Converter_IEvent.hpp
  @author         TorstenS
  @brief

*/

/*
  ========== licence begin  GPL
  Copyright (c) 2001-2004 SAP AG

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
*/



#ifndef CONVERTER_IEVENT_HPP
#define CONVERTER_IEVENT_HPP


/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "KernelCommon/DatabaseEvents/KernelDBE_Event.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 **/


/*===========================================================================*
 *  MACROS                                                                   *
 **/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 **/

/*!
  @class          Converter_IEvent
  @brief          Converter_Converters Interface to Events
 */
class Converter_IEvent
{
public:

    /*!
    @brief          returns the backup interface of the converter
    @return         (Converter_IEvent) instance of the converter

    - This static function returns the interface of the converter which
      allows access to all methods defined in this interface class
  */

    static   Converter_IEvent & Instance();

    /*!
    @brief    defines an event, i.e. whenever the occupation of the database passes
              the given RelativeThreshold an event is triggered.
    @param    RelativeThreshold [in] relative filling level of the data base, whenever
              this critical value is passed an event is triggered
              critical value in 0.01 percent with respect to the total number of pages
              of the data base
    @param    bOverflow [in] if this is true the event is triggerd if the filling exceeds the
              critical value else an event is triggered if the filling falls below
              the critical value.
    @param    Priority [in] priority of the event
    @return   (SAPDB_Bool) true if the value could be added successfully

  */

    virtual SAPDB_Bool AddEvent (const SAPDB_Int                RelativeThreshold,
        const SAPDB_Bool               bOverflow,
        const tsp31_event_prio_Param   Priority) = 0;

    /*!
    @brief          removes an event from the set of events
    @param          RelativeThreshold [in] defines the critical value to be removed.
                    The value is given in units of 0.01 percent with respect to the base unit
    @param          bOverflow [in] displays whether the event of exceeding or falling below is to remove
    @return         (SAPDB_Bool) true if the event set is not empty
  */
    virtual SAPDB_Bool DelEvent  (const SAPDB_Int    RelativeThreshold,
                                  const SAPDB_Bool   bOverflow) = 0;

};

#endif  /* CONVERTER_IEVENT_HPP */
