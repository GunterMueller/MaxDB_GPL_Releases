/*******************************************************************************
  module:       DBMP2F_ReactionRemoveFile.cpp
  ------------------------------------------------------------------------------
  responsible:  TiloH
  special area: pipe2file
  description:  implements remove a file as reaction for pipe2file
  last change:  2002-06-25 07:47
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

#include "SAPDB/DBM/Pipe2File/DBMP2F_ReactionRemoveFile.hpp"

#include "hni34.h"


// -----------------------------------------------------------------------------
// member functions DBMP2F_ReactionRemoveFile
// -----------------------------------------------------------------------------

int DBMP2F_ReactionRemoveFile::React(void)
{
    return 1;
}

int DBMP2F_ReactionRemoveFile::React(const char * Info, int & Answer)
{
    int rc=1;

    if(0!=Info)
    {
        tni34_AVirtualFile TheFile(Info, tni34_AVirtualFile::ToDiskAppend, tni34_AVirtualFile::DoNotOpenNow); // do not open

        if(!TheFile.Remove())
            rc=0;
    }

    return rc;
}

int DBMP2F_ReactionRemoveFile::React(teo00_Uint8 Info, time_t TimeInfo)
{
    return 1;
}
