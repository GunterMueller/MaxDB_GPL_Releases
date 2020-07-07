/*!
  \file     DBMP2F_EventChecker.cpp
  \author   TiloH
  \ingroup  pipe2file
  \brief    class for handling reactions for events
  
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

// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include <stdio.h>

#include "SAPDB/DBM/Pipe2File/DBMP2F_EventChecker.hpp"


// -----------------------------------------------------------------------------
// member functions DBMP2F_EventChecker
// -----------------------------------------------------------------------------

DBMP2F_EventChecker::~DBMP2F_EventChecker()
{
    while(!EventList.IsEmpty())
    {
        delete EventList.First()->GiveInfo();
        EventList.Del();
    }
}

int DBMP2F_EventChecker::AddEventWithReactions(DBMP2F_Event * NewEvent)
{
    int rc=1;

    if(0==EventList.Add(NewEvent))
    {
        fprintf(stderr, "Out of memory.\n");
        rc=0;
    }

    return rc;
}

int DBMP2F_EventChecker::ReactionsFor(DBMP2F_Event::EventEnum CurrentEvent)
{
    int rc=1;

    ni31_ListElem<DBMP2F_Event *> *h=0;

    for(h=EventList.First(); rc && 0!=h; h=h->Next())
    {
        if(CurrentEvent==h->GiveInfo()->GiveEventType())
            rc=h->GiveInfo()->React();
    }

    return rc;
}

int DBMP2F_EventChecker::ReactionsFor(DBMP2F_Event::EventEnum CurrentEvent, const char * Info, int & Answer)
{
    int rc=1;

    ni31_ListElem<DBMP2F_Event *> *h=0;

    for(h=EventList.First(); rc && 0!=h; h=h->Next())
    {
        if(CurrentEvent==h->GiveInfo()->GiveEventType())
            rc=h->GiveInfo()->React(Info, Answer);
    }

    return rc;
}

int DBMP2F_EventChecker::ReactionsFor(DBMP2F_Event::EventEnum CurrentEvent, teo00_Uint8 Info, time_t Time)
{
    int rc=1;

    ni31_ListElem<DBMP2F_Event *> *h=0;

    for(h=EventList.First(); rc && 0!=h; h=h->Next())
    {
        if(CurrentEvent==h->GiveInfo()->GiveEventType())
            rc=h->GiveInfo()->React(Info, Time);
    }

    return rc;
}
