/*!
  @file           IFRPacket_ReplyPacket.cpp
  @author         D025086
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
#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/Packet/IFRPacket_ReplyPacket.h"

//----------------------------------------------------------------------
IFRPacket_ReplyPacket::IFRPacket_ReplyPacket()
:PIn_ReplyPacket(0),
 m_lock(0),
 m_hassharelock(false)
{
}

//----------------------------------------------------------------------
IFRPacket_ReplyPacket::~IFRPacket_ReplyPacket()
{
    if(m_hassharelock) {
        m_lock->releaseShareLock();
    }
}

//----------------------------------------------------------------------
IFRPacket_ReplyPacket::IFRPacket_ReplyPacket(const IFRPacket_ReplyPacket& copy)
{
    this->rawPacket=copy.rawPacket;
    m_lock=copy.m_lock;
    if(copy.m_hassharelock) {
        m_lock->acquireShareLock();
        m_hassharelock=true;
    } else {
        m_hassharelock=false;
        m_lock=0;
    }
}

//----------------------------------------------------------------------
IFRPacket_ReplyPacket& 
IFRPacket_ReplyPacket::operator =(const IFRPacket_ReplyPacket& rhs)
{
    if(this == &rhs) {
        return *this;
    }
    if(m_hassharelock) {
        m_lock->releaseShareLock();
    }
    this->rawPacket=rhs.rawPacket;
    m_lock=rhs.m_lock;
    if(rhs.m_hassharelock) {
        m_lock->acquireShareLock();
        m_hassharelock=true;
    } else {
        m_hassharelock=false;
    }
    return *this;
}

//----------------------------------------------------------------------
void
IFRPacket_ReplyPacket::setPacket(tsp1_packet *packetPtr, IFRPacket_Lock* lock)
{
	// similar to assign, only split up 
	// we must save the current lock instance,
	// and for safety, release a possible share lock
	// after this instance got changed.
	
	IFRPacket_Lock *oldlock=m_lock;
	IFR_Bool hadsharelock=m_hassharelock;	
	
    this->rawPacket=packetPtr;
    if(lock) {
        m_hassharelock=true;
        m_lock=lock;
        m_lock->acquireShareLock();
    } else {
        m_hassharelock=false;
        m_lock=0;
    }
    
    if(oldlock && hadsharelock) {
    	oldlock->releaseShareLock();
    }
}


//----------------------------------------------------------------------
void
IFRPacket_ReplyPacket::releaseLock()
{
    if(m_hassharelock) {
        m_lock->releaseShareLock();
        m_hassharelock=false;
    }
}

//----------------------------------------------------------------------
const IFR_StringEncoding
IFRPacket_ReplyPacket::getEncoding() const
{
    DBUG_CONTEXT_METHOD_ENTER(ReplyPacket, getEncoding, m_lock);
    const tsp1_packet_header *header = GetRawHeader();
    if (header) {
        switch(header->sp1h_mess_code) {
        case csp_ascii:
            DBUG_RETURN( IFR_StringEncodingAscii );
        case csp_unicode:
            DBUG_RETURN( IFR_StringEncodingUCS2 );
        case csp_unicode_swap:
            DBUG_RETURN( IFR_StringEncodingUCS2Swapped );
        case csp_utf8:
            DBUG_RETURN( IFR_StringEncodingUTF8 );
        default:
            DBUG_RETURN((IFR_StringEncoding)0);
        }
    }
    DBUG_RETURN((IFR_StringEncoding)0);
}
