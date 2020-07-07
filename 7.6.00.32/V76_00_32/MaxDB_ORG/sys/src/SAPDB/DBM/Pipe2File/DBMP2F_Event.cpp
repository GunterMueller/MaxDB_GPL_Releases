/*!
  \file     DBMP2F_Event.cpp
  \author   TiloH
  \ingroup  pipe2file
  \brief    class handling events
  
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

#include "SAPDB/DBM/Pipe2File/DBMP2F_Event.hpp"


// -----------------------------------------------------------------------------
// member functions DBMP2F_Event
// -----------------------------------------------------------------------------

DBMP2F_Event::DBMP2F_Event(EventEnum                      TheEventType)
:EventType(TheEventType)
{
}

DBMP2F_Event::~DBMP2F_Event()
{
    while(!Reactions.IsEmpty()) //delete all members of the reaction list
    {
        delete Reactions.First()->GiveInfo();
        Reactions.Del();
    }
}

int DBMP2F_Event::AddReaction(DBMP2F_Reaction * NewReaction)
{
    return (0!=Reactions.Add(NewReaction));
}

const DBMP2F_Event::EventEnum DBMP2F_Event::GiveEventType() const
{
    return EventType;
}

int DBMP2F_Event::React(void)
{
    int rc=1;

    ni31_ListElem<DBMP2F_Reaction *> *h;

    for(h=Reactions.First(); 0!=h; h=h->Next())
        rc=h->GiveInfo()->React();

    return rc;
}

int DBMP2F_Event::React(const char * Info, int & Answer)
{
    int rc=1;

    ni31_ListElem<DBMP2F_Reaction *> *h;

    for(h=Reactions.First(); 0!=h; h=h->Next())
        rc=h->GiveInfo()->React(Info, Answer);

    return rc;
}

int DBMP2F_Event::React(teo00_Uint8 Info, time_t TimeInfo)
{
    int rc=1;

    ni31_ListElem<DBMP2F_Reaction *> *h;

    for(h=Reactions.First(); 0!=h; h=h->Next())
        rc=h->GiveInfo()->React(Info, TimeInfo);

    return rc;
}
