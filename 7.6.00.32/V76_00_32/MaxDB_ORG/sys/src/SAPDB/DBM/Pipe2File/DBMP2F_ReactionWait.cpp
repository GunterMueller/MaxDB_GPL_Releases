/*******************************************************************************
  module:       DBMP2F_ReactionWait.cpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: pipe2file
  description:  implements a wait reaction for pipe2file
  last change:  2002-06-25 08:08
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

#include "SAPDB/DBM/Pipe2File/DBMP2F_ReactionWait.hpp"


// -----------------------------------------------------------------------------
// member functions DBMP2F_ReactionWait
// -----------------------------------------------------------------------------

int DBMP2F_ReactionWait::React(void)
{
    int Dummy;

    return React((char *)0, Dummy);
}

int DBMP2F_ReactionWait::React(const char * Info, int & Answer)
{
    int rc=1;

    char Line[10+1];

    int i=0;
    int LineBreak=0, EndOfFileReached=0;

    Answer=0;

    while(!LineBreak && !EndOfFileReached && i<11)
    {
        if(EOF==scanf("%c", Line+i))
            EndOfFileReached=1;
        else
            if('\n'==Line[i])
                LineBreak=1;
            else
                i++;
    }

    Line[i]='\0';

    if(EndOfFileReached || (Line[0]!='n' && Line[0]!='l'))
        rc=0;
    else
        Answer=(Line[0]=='l');

    return rc;
}

int DBMP2F_ReactionWait::React(teo00_Uint8 Info, time_t TimeInfo)
{
    int Dummy;

    return React((char *)0, Dummy);
}
