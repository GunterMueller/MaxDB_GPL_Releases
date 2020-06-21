/*!
  @file           IFRPacket_PartEnum.h
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
#ifndef IFRPACKET_PARTENUM_H
#define IFRPACKET_PARTENUM_H
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_ReplySegment.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_Part.h"

/**
 * Part enumeration to iterate over the parts of a segment.
 */
class IFRPacket_PartEnum 
{
public:
    IFRPacket_PartEnum (IFRPacket_ReplySegment &replysegment);
    
    /**
     * Returns whether more elements are in this enumeration.
     * @return @c true if there are more parts.
     */
    inline IFR_Bool hasMoreElements() const 
    {
        return m_partsleft > 0;
    }

    IFR_Bool nextElement(IFRPacket_Part& part);

    inline IFRPacket_PartKind::PartKind getPartKind()
    {
        return (IFRPacket_PartKind::PartKind)m_replysegment->partKind ();
    }

    inline void getPart(IFRPacket_Part& part)
    {
        part = m_currentpart;
    }

    inline void next()
    {
        if(m_partsleft) {
            --m_partsleft;
            m_currentpart = m_replysegment->getNextPart();
        }
    }
    
private:
    IFR_Int4                m_partsleft;
    IFRPacket_ReplySegment *m_replysegment;
    IFRPacket_Part          m_currentpart;
};
#endif //IFRPACKET_PARTENUM_H
