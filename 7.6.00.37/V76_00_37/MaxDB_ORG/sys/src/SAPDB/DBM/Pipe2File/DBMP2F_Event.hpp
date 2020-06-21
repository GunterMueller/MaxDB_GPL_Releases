/*!
  \file     DBMP2F_Event.hpp
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


#if !defined(_DBMP2F_Event_hpp)
#define _DBMP2F_Event_hpp


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "hni31.h"

#include "SAPDB/DBM/Pipe2File/DBMP2F_Reaction.hpp"


// -----------------------------------------------------------------------------
// class DBMP2F_Event
// -----------------------------------------------------------------------------

class DBMP2F_Event
{
  public:
    enum EventEnum{ADelay,
                   AEndOfSubDevice,
                   AWait,
                   AShowSpeed,
                   AMessage};

    DBMP2F_Event(EventEnum TheEventType);
    ~DBMP2F_Event();

    int AddReaction(DBMP2F_Reaction * NewReaction);

    const EventEnum GiveEventType() const;
    int             React(void);
    int             React(const char * Info, int & Answer);
    int             React(teo00_Uint8 Info, time_t TimeInfo);

  private:
    EventEnum                    EventType;
    ni31_List<DBMP2F_Reaction *> Reactions;
};

#endif
