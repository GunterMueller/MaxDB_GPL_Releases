/*!*********************************************************************

  module:       Kernel_VTrace.hpp

  ----------------------------------------------------------------------

  responsible:  DanielD

  special area: Kernel | Debug

  description:  Stream to write formatted vtrace entries

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


***********************************************************************/

#include "heo51.h"

#include "SAPDB/KernelCommon/Kernel_VTrace.hpp"
#include "Trace/Trace_Entry.hpp"

Kernel_VTrace::LineBuffer::LineBuffer ()
: SAPDB_StreamBuffer (line, line + sizeof(line))
{
    vgetpid (this->transContext.trTaskId_gg00);
    this->transContext.trSessionId_gg00.becomes (cgg_nil_session);
    this->transContext.trTransId_gg00.becomes (cgg_nil_trans);
    this->transContext.trWriteTransId_gg00.becomes (cgg_nil_trans);
    this->transContext.trSubtransId_gg00 = cgg_zero_subtrans;
}

/*----------------------------------------*/

void Kernel_VTrace::LineBuffer::Overflow ()
{
    const char * start = this->Begin ();
    const char * end = this->Next ();
    const char * nlPos;

    /*
    * break into lines
    */
    nlPos = REINTERPRET_CAST (const char *, memchr (start, '\n', end - start));
    while (nlPos != NULL) {

        Trace_CommonEntry entry ( tgg00_Debug::fromConst(ta),
            tgg00_VtraceType::fromConst(td_none),
            reinterpret_cast<const tsp00_Byte *> (start),
            static_cast <SAPDB_Int4> (nlPos - start));

        start = nlPos + 1;
        nlPos = REINTERPRET_CAST (const char *, memchr (start, '\n', end - start));
    }
    /*
    * write remainder as one line
    */
    if (end - start > 0) {
        Trace_CommonEntry entry ( tgg00_Debug::fromConst(ta),
            tgg00_VtraceType::fromConst(td_none),
            reinterpret_cast<const tsp00_Byte *> (start),
            static_cast <SAPDB_Int4> (end - start));
    }
}

/*----------------------------------------*/

Kernel_VTrace::Kernel_VTrace ()
    : SAPDB_OStream (buf)
{
}

/*----------------------------------------*/

Kernel_VTrace::~Kernel_VTrace ()
{
    this->Flush ();
}

/*----------------------------------------*/

