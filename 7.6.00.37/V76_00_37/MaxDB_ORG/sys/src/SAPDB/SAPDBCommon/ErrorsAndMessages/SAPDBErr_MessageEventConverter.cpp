/*!
  @file           SAPDBErr_MessageEventConverter.cpp
  @author         StefanP
  @special area   Console (XCons) <-> Console Thread
  @brief          Common Routines
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




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#include  "SAPDBCommon/ErrorsAndMessages/SAPDBErr_MessageEventConverter.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTION PROTOTYPES                                        *
 *===========================================================================*/

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void    SAPDBErr_ConvertEventToMessageList 
(
    SAPDBErr_MessageList            &   messageList,
    teo200_EventList        const   &   eventList
)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    SAPDBErr_MessageList::MessageType               messageType;
    SAPDB_UInt4                                     messageID; 
    teo200_EventList                    const   *   pTmpEventList = &eventList;

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    if( eventList.IsEmpty() ) {
        messageList.ClearMessageList();
        return;
    }

    while (NULL != pTmpEventList)
    {
        messageID   = pTmpEventList->eo200_EventID ();

        switch (eventList.eo200_EventType ())
        {
            case teo200_EventList::Error_eeo200:
                messageType = SAPDBErr_MessageList::Error;
                break;
            
            case teo200_EventList::Warning_eeo200:
                messageType = SAPDBErr_MessageList::Warning;
                break;
            
            case teo200_EventList::Info_eeo200:
            default:
                messageType = SAPDBErr_MessageList::Info;
                break;
        }

        messageList = SAPDBErr_MessageList (pTmpEventList->eo200_EventLabel (), 
                                                          "", 
                                                          0, 
                                                          messageType, 
                                                          messageID, 
                                                          pTmpEventList->eo200_EventMsg (), 
                                                          0)
                      + messageList;

        pTmpEventList = pTmpEventList->eo200_NextEvent ();
    }

    return;
}

/*---------------------------------------------------------------------------*/

void      SAPDBErr_ConvertMessageToEventList 
(
    SAPDBErr_MessageList   const   &   messageList,
    teo200_EventList               &   eventList
)
{
/*===========================================================================*
 *  Locals                                                                   *
 *===========================================================================*/
    teo200_EventList::teo200_EventID                eventID;
    teo200_EventList::teo200_EventType              eventType;
    SAPDB_UInt4                                     msgCount;
    SAPDBErr_MessageList                const   *   pTmpMessageList;
    _TCHAR                                          eventLabel[MAX_EVENTLABEL_LEN_EO200 + 1];

/*===========================================================================*
 *  Instructions                                                             *
 *===========================================================================*/
    if( messageList.IsEmpty() ) {
        eventList.eo200_ClearEventList();
        return;
    }

    for (msgCount = 0, pTmpMessageList = &messageList;
         NULL != pTmpMessageList; 
         ++msgCount, pTmpMessageList = pTmpMessageList->NextMessage ()) 
    {
        eventID         = pTmpMessageList->ID ();

        switch (pTmpMessageList->Type ())
        {
            case SAPDBErr_MessageList::Error:
                eventType = teo200_EventList::Error_eeo200;
                break;

            case SAPDBErr_MessageList::Warning:
                eventType = teo200_EventList::Warning_eeo200;
                break;

            case SAPDBErr_MessageList::Info:
            default:
                eventType = teo200_EventList::Info_eeo200;
                break;
        }

        memset (eventLabel, 0, MAX_EVENTLABEL_LEN_EO200 + 1);
        _tcsncpy (eventLabel, pTmpMessageList->Component (), MAX_EVENTLABEL_LEN_EO200);
        
        if (0 == msgCount)
        {
            eventList = teo200_EventList ("", 
                                          eventID, 
                                          eventType, 
                                          eventLabel, 
                                          pTmpMessageList->Message ());
        }
        else
        {
            eventList = teo200_EventList (eventList,
                                          "", 
                                          eventID, 
                                          eventType, 
                                          eventLabel, 
                                          pTmpMessageList->Message ());
        }
    }

    return;
}

