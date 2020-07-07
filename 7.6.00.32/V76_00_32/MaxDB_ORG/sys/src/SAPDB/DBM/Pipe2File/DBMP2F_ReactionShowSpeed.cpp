/*!
  \file     DBMP2F_ReactionShowSpeed.cpp
  \author   TiloH
  \ingroup  pipe2file
  \brief    implements a speed indicator as reaction for pipe2file
  
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

#include "SAPDB/DBM/Pipe2File/DBMP2F_ReactionShowSpeed.hpp"


// -----------------------------------------------------------------------------
// member functions DBMP2F_ReactionShowSpeed
// -----------------------------------------------------------------------------

DBMP2F_ReactionShowSpeed::DBMP2F_ReactionShowSpeed()
    :LastTime(0)
{
}

int DBMP2F_ReactionShowSpeed::React(void)
{
    return React(0, 0);
}

int DBMP2F_ReactionShowSpeed::React(const char * Info, int & Answer)
{
    return React(0, 0); //just don't touch Answer and Info
}

int DBMP2F_ReactionShowSpeed::React(teo00_Uint8 Info, time_t TimeInfo)
{
    time_t CurrentTime=time(0);

    if(CurrentTime>LastTime) // if speed was shown at least one second ago
    {
        if(CurrentTime>TimeInfo)
            printf("%s%f MB/s\n", ((MAX_UINT8_CN36==Info)?"more than ":""), (((double)((teo00_Int8)Info))/((double)(1024*1024)))/double(CurrentTime-TimeInfo)); 
        else
            printf("more than %f MB/s\n", double(teo00_Int8(Info))/double(1024*1024));  //teo00_Int8() needed as MSVC++: "C2520: conversion from unsigned __int64 to double not implemented, use signed __int64", TODO: class for big integers

        LastTime=CurrentTime; //remember, that we showed the speed in this second
    }

    return 1;
}
