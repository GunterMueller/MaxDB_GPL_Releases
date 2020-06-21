/*!
  @file           IFRPacket_ReplyPacket.h
  @author         D030044
  @ingroup        IFR_Packet
  @brief          
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
#ifndef IFRPACKET_REPLYPACKET_H
#define IFRPACKET_REPLYPACKET_H

#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "Interfaces/Runtime/IFR_Types.h"
#include "Interfaces/Runtime/Packet/IFRPacket_Lock.h"
#include "Interfaces/Runtime/IFR_String.h"

IFR_BEGIN_NAMESPACE

/**
 * Overlay on the <code>PIn_ReplyPacket</code>.
 */
class IFRPacket_ReplyPacket 
    : public PIn_ReplyPacket 
{
public:
    /**
     * Creates an invalid packet.
     */
    IFRPacket_ReplyPacket();
    
    
    /**
     * Destructor. Gives up the possibly held share lock.
     */
    ~IFRPacket_ReplyPacket();

    /**
     * Copy constructor.
     * @param packet The packet to copy.
     */
    IFRPacket_ReplyPacket(const IFRPacket_ReplyPacket& packet);
    
    /**
     * Assignment operator.
     * @param rhs The packet to assign to this packet.
     * @return <code>*this</code>.
     */
    IFRPacket_ReplyPacket& operator =(const IFRPacket_ReplyPacket& rhs);

    
    /**
     * Releases the lock. <b>Use this method with extreme caution and 
     * under the circumstance that you will never need anything from
     * this reply packet again!</b>
     */
    void releaseLock();

    /**
     * Gets the encoding of the packet. This is stored in the raw header 
     * (<code>mess_code</code> and <code>mess_swap</code>.
     * @return the encoding of the packet, or 0 if no valid encoding was found,
     *         or the packet is not valid.
     */
    const IFR_StringEncoding getEncoding() const;    

    /**
     * Gets the lock.
     */
    inline IFRPacket_Lock* getLock() 
    { 
        return m_lock; 
    }

    /**
     * Creates a reply packet from the reply pointer 
     * handed out from the <code>sqlareceive</code> call.
     * It will acquire a <b>share lock</b> on the packet.
     * @param packetPtr Pointer to the reply packet structure.
     * @param lock Pointer to the packet lock instance.
     */
    void setPacket(tsp1_packet *packetPtr, IFRPacket_Lock* lock);
   
private:
    
    IFRPacket_Lock  *m_lock;         //!< Lock instance.
    IFR_Bool         m_hassharelock; //!< Has this packet a share lock?
    
    friend class IFR_Connection;
};

IFR_END_NAMESPACE

#endif //IFRPACKET_REPLYPACKET_H
