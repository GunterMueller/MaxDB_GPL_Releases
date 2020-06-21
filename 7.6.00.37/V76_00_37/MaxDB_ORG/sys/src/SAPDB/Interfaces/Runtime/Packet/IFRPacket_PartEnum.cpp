/*!
  @file           IFRPacket_PartEnum.cpp
  @author         ThomasS
  @ingroup        IFR_Packet
  @brief          Implements class for part enumeration.
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
#include "SAPDB/Interfaces/Runtime/IFR_Common.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_PartEnum.h"

#define DBUG_NULL_METHOD_ENTER(x,y) DBUG_CONTEXT_METHOD_ENTER(x, y, (IFR_TraceStream *)0)

//----------------------------------------------------------------------
IFRPacket_PartEnum::IFRPacket_PartEnum (IFRPacket_ReplySegment &replysegment)
:m_replysegment(&replysegment)
{
    short nrOfParts;
    tsp1_segment_header *hdr = replysegment.GetRawHeader ();
    memcpy (&nrOfParts, (char *) hdr + 8, sizeof (short));
    this->m_partsleft = nrOfParts;
    this->m_currentpart =  m_replysegment->getFirstPart ();
}

//----------------------------------------------------------------------
IFR_Bool
IFRPacket_PartEnum::nextElement(IFRPacket_Part& part)
{
    if(m_partsleft == 0) {
        return false;
    } else {
        --m_partsleft;
        part = m_currentpart =  m_replysegment->getNextPart ();
        return true;
    }
}

