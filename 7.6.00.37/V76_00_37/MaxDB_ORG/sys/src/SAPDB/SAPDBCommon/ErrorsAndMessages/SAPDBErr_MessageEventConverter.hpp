/*!
  @file           SAPDBErr_MessageEventConverter.hpp
  @author         Stefan Paeck
  @special area   Konverter: MessageList <-> Eventlist
  @brief          
  @see            

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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



#ifndef SAPDBERR_MESSAGEEVENTCONVERTER
#define SAPDBERR_MESSAGEEVENTCONVERTER



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include    "geo200.h"
#include    "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageList.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/


/*===========================================================================*
 *  Console Error Codes                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  Communication types                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  PROTOTYPS                                                                *
 *===========================================================================*/

/*!
   @brief    Converting message to event list
                The conversion is done in reverse order. That means if we have 
                a event list 1|2|3|4 they are tranformed to a message list 4|3|2|1.
   @param          messageList [in] - Message list
   @param          eventList [in/out] - Event   list
   @return value   none

 */


void        SAPDBErr_ConvertMessageToEventList   (SAPDBErr_MessageList const &       messageList,
                                                  teo200_EventList &                 eventList);
/*!
   @brief    Converting event to message list
                The conversion is done in reverse order. That means if we have 
                a message list 1|2|3|4 they are tranformed to a event list 4|3|2|1.
                The resulting message list is appended to the given message list.
   @param          messageList [in/out] - Message list
   @param          eventList [in] - Event list
   @return value   none

 */


void        SAPDBErr_ConvertEventToMessageList   (SAPDBErr_MessageList &             messageList,
                                                  teo200_EventList const &           eventList);

#endif  /* SAPDBERR_MESSAGEEVENTCONVERTER */
