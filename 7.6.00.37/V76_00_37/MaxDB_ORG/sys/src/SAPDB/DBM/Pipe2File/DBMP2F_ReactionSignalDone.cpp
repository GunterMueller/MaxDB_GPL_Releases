/*******************************************************************************
  module:       DBMP2F_ReactionSignalDone.cpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: pipe2file
  description:  implements the 'd' message as reaction for pipe2file
  last change:  2002-06-25 08:00
  version:      7.4.3
  see also:     
  ------------------------------------------------------------------------------
                         Copyright (c) 2002-2005 SAP AG



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




*******************************************************************************/

// -----------------------------------------------------------------------------
// includes
// -----------------------------------------------------------------------------

#include <stdio.h>

#include "SAPDB/DBM/Pipe2File/DBMP2F_ReactionSignalDone.hpp"


// -----------------------------------------------------------------------------
// member functions DBMP2F_ReactionSignalDone
// -----------------------------------------------------------------------------

int DBMP2F_ReactionSignalDone::React(void)
{
    int rc=1;

    printf("d");
    fflush(stdout);

    return rc;
}

int DBMP2F_ReactionSignalDone::React(const char * Info, int & Answer)
{
    return React(); //just don't touch Answer and Info
}

int DBMP2F_ReactionSignalDone::React(teo00_Uint8 Info, time_t TimeInfo)
{
    return React(); //just ignore Info and TimeInfo
}
