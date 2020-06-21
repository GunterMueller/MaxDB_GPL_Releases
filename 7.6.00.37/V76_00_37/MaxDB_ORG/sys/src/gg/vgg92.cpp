/* @lastChanged: "1999-04-21"

  * @filename:   vgg92.cpp
  * @purpose:    "GG_kernel_classes"
  * @release:    7.2.0.0
  * @see:        ""
  *
  * @Copyright (c) 1999-2005 SAP AG"


    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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


*/

#include "ggg92.h"
#include "hgg17.h"

/* ========================================================================== */
/* ==========                tgg92_KernelOid                        ========= */
/* ========================================================================== */

void tgg92_KernelOid::gg92AppendToLine (int        &LineLen,
                                        tsp00_Line &Line)
{
    if ( this->gg92IsNil() )
    {
        gg90StrToLine (_T("NilOid"), LineLen, Line);
        return;
    }
    
    if ( this->gg92IsNilPno() )
        gg90StrToLine (_T("NilPno"), LineLen, Line);
    else
        g17trimint4_to_line (this->gg92GetPno(), LineLen, Line);
    
    gg90StrToLine (_T("."), LineLen, Line);

    if ( this->gg92IsNilPos() )
        gg90StrToLine (_T("NilPos"), LineLen, Line);
    else
        g17trimint4_to_line (this->gg92GetPos(), LineLen, Line);
    
 
    gg90StrToLine (_T(", (vers "), LineLen, Line);

    if ( this->gg92IsNilFrameVers() )
        gg90StrToLine (_T("Nil"), LineLen, Line);
    else
        g17trimint4_to_line (this->gg92GetFrameVers(), LineLen, Line);

    gg90StrToLine (_T(")"), LineLen, Line);
}

// end of progam
