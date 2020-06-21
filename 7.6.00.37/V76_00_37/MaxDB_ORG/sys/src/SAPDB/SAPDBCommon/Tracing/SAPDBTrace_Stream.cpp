/*!*********************************************************************

  module:       SAPDBTrace_Stream.cpp

  ----------------------------------------------------------------------

  responsible:  DanielD

  special area: SAPDB Tracing

  description:  an output stream with special methods
                to support tracing.

  ----------------------------------------------------------------------

  copyright:            Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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




  *********************************************************************/

#include "SAPDBCommon/Tracing/SAPDBTrace_Stream.hpp"

/*----------------------------------------*/

void SAPDBTrace_Stream::LineBuffer::Overflow ()
{
    const SAPDB_Char * start = this->Begin ();
    const SAPDB_Char * end = this->Next ();
    const SAPDB_Char * nlPos;

    /*
     * break into lines
     */
    nlPos = reinterpret_cast <const SAPDB_Char *> (memchr (start, '\n',
        end - start));
    while (nlPos != NULL) {
        this->linePrinter->PrintLn (start,
            static_cast<int> (nlPos - start));
        start = nlPos + 1;
        nlPos = reinterpret_cast <const SAPDB_Char *> (memchr (start, '\n', end - start));
    }
    /*
     * write remainder as one line
     */
    if (end - start > 0) {
        this->linePrinter->PrintLn (start,
            static_cast<int> (end - start));
    }
}


