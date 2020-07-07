/*!*********************************************************************

    @file         SAPDBErr_Contracts.cpp
    
    @author       DanielD

    @brief

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
***********************************************************************/

#include "SAPDBCommon/ErrorsAndMessages/SAPDBErr_Contracts.hpp"

#include <stdio.h>

/*----------------------------------------*/

static void enforceContract (
    const char * contractKind,
    const char * filename, 
    int          line, 
    const char * conditiontext)
{
    fprintf (stdout, "%s:%s:%d:%s\n", 
        contractKind, conditiontext, line, filename);
    int * crash = 0;
    *crash = 0;
}

/*----------------------------------------*/

void SAPDBErr_Lawyer::
preconditionFailed (
    const char * filename, 
    int          line, 
    const char * conditiontext)
{
    enforceContract ("PRE", filename, line, conditiontext);
}

/*----------------------------------------*/

void SAPDBErr_Lawyer::
postconditionFailed (
    const char * filename, 
    int          line, 
    const char * conditiontext)
{
    enforceContract ("POST", filename, line, conditiontext);
}

/*----------------------------------------*/

void SAPDBErr_Lawyer::
invariantFailed (
    const char * filename, 
    int          line, 
    const char * conditiontext)
{
    enforceContract ("INV", filename, line, conditiontext);
}

/*----------------------------------------*/

void SAPDBErr_Lawyer::
runtimeCheckFailed (
    const char * filename, 
    int line, 
    const char * conditiontext,
    const char * comment)
{
    if (comment != 0) {
        enforceContract ("MISS", filename, line, comment);
    }
    else {
        enforceContract ("MISS", filename, line, conditiontext);
    }
}

/*----------------------------------------*/


