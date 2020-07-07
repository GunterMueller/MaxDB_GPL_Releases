/*!
  \file     DBMP2F_EventChecker.hpp
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

#if !defined(_DBMP2F_EventChecker_hpp)
#define _DBMP2F_EventChecker_hpp


// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include "hni31.h"

#include "SAPDB/DBM/Pipe2File/DBMP2F_Event.hpp"
#include "SAPDB/DBM/Pipe2File/DBMP2F_Reaction.hpp"


// -----------------------------------------------------------------------------
// class DBMP2F_EventChecker
// -----------------------------------------------------------------------------

class DBMP2F_EventChecker
{
  public:
    ~DBMP2F_EventChecker();

    int AddEventWithReactions(DBMP2F_Event * NewEvent);
    
    int ReactionsFor(DBMP2F_Event::EventEnum CurrentEvent);
    int ReactionsFor(DBMP2F_Event::EventEnum CurrentEvent, const char * Info, int & Answer);
    int ReactionsFor(DBMP2F_Event::EventEnum CurrentEvent, teo00_Uint8 Info, time_t Time);

  private:
    ni31_List<DBMP2F_Event *> EventList;
};

#endif
