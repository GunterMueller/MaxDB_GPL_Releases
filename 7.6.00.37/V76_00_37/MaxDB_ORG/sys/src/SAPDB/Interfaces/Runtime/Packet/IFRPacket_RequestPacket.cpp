/*!
  @file           IFRPacket_RequestPacket.cpp
  @author         D039759
  @ingroup        IFR_Packet
  @brief          Request Packet.
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
#include "Interfaces/Runtime/Packet/IFRPacket_RequestPacket.h"
#include "Interfaces/Runtime/Packet/IFRPacket_Segment.h"
#include "Interfaces/Runtime/Packet/IFRPacket_Part.h"
#include "Interfaces/Runtime/Packet/IFRPacket_Lock.h"
#include "Interfaces/Runtime/Util/IFRUtil_VDNNumber.h"
#include "Interfaces/Runtime/IFR_ConnectionItem.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_Converter.h"

IFR_BEGIN_NAMESPACE

//----------------------------------------------------------------------
IFRPacket_RequestPacket::IFRPacket_RequestPacket (tsp1_packet *packetptr,
                                                  int size, 
                                                  int unicode,
                                                  int sqlmode,
                                                  const char *client_application,
                                                  const char *client_version,
                                                  IFRUtil_RuntimeItem& runtimeitem,
                                                  IFR_Bool& memory_ok)
:PIn_RequestPacket(packetptr, size, unicode, client_application, client_version),
 IFRUtil_RuntimeItem(runtimeitem),
 m_CurrentSqlMode (sqlmode),
 m_hasexclusivelock(false),
 m_packettype(IFRPacket_RequestPacket::Root_C)
{
    DBUG_METHOD_ENTER(IFRPacket_RequestPacket, IFRPacket_RequestPacket);
    DBUG_PRINTC((void*), packetptr);
    DBUG_PRINT(size);
    DBUG_PRINT(unicode);
    DBUG_PRINT(sqlmode);
    DBUG_PRINT(client_application);
    DBUG_PRINT(client_version); 
    m_lock=new IFR_ALLOCATOR(allocator) IFRPacket_RootLock(runtime, allocator, memory_ok);
    if(IFR_MEM_NOT_OK) {
        IFRUtil_Delete(m_lock, allocator);
        m_lock = 0;
    } else if(m_lock==0) {
        IFR_MEM_FAIL;
    }
}

//----------------------------------------------------------------------
IFRPacket_RequestPacket::IFRPacket_RequestPacket(tsp1_packet *packetptr,
                                                 int size,
                                                 int unicode,
                                                 int sqlmode,
                                                 const char *client_application,
                                                 const char *client_version,
                                                 IFRPacket_RequestPacket::PacketType packettype,
                                                 IFRPacket_Lock *lockinstance,
                                                 IFRUtil_RuntimeItem& runtimeitem)
:PIn_RequestPacket(packetptr, size, unicode, client_application, client_version),
 IFRUtil_RuntimeItem(runtimeitem),
 m_CurrentSqlMode (sqlmode),
 m_hasexclusivelock(false),
 m_packettype(packettype)
{
    DBUG_METHOD_ENTER(IFRPacket_RequestPacket, IFRPacket_RequestPacket);
    DBUG_PRINT(size);
    DBUG_PRINT(unicode);
    DBUG_PRINT(sqlmode);
    DBUG_PRINT(client_application);
    DBUG_PRINT(client_version);
    DBUG_PRINT(packettype);
    m_lock=lockinstance;
    if(packettype==Dynamic_C) {
        m_lock->acquireExclusiveLock();
        m_hasexclusivelock=true;
    }

}


//----------------------------------------------------------------------
IFRPacket_RequestPacket::
IFRPacket_RequestPacket(const IFRPacket_RequestPacket& requestpacket)
:PIn_RequestPacket(),
 IFRUtil_RuntimeItem(requestpacket)
{
    DBUG_METHOD_ENTER(IFRPacket_RequestPacket, IFRPacket_RequestPacket);
    DBUG_PRINTC((void*),&requestpacket);
    DBUG_PRINTC((void*),this);
    
    // the packet is already initialised, thus we set the
    // raw packet pointer 'behind the back'
    // also all data is kept
    this->rawPacket=requestpacket.rawPacket;
    m_CurrentSqlMode=requestpacket.m_CurrentSqlMode;
    m_lock=requestpacket.m_lock;
    
    switch(requestpacket.m_packettype) {
    case (Root_C):
    case (Root_Ref_C):
        m_packettype=Root_Ref_C;
        break;
    case (Dynamic_C):
        m_packettype=Dynamic_C;
    };
    
    // as this is a copy, we must increase / get the lock
    if(m_lock) {
        m_lock->acquireExclusiveLock();
        m_hasexclusivelock=true;
    } else {
        m_hasexclusivelock=false;
    }

    
}

//----------------------------------------------------------------------
IFRPacket_RequestPacket::IFRPacket_RequestPacket
  (IFRUtil_RuntimeItem &runtimeitem)
:IFRUtil_RuntimeItem(runtimeitem),
 m_CurrentSqlMode (IFR_SessionSqlmode),
 m_lock(0),
 m_hasexclusivelock(false),
 m_packettype(IFRPacket_RequestPacket::Root_Ref_C)
{
    DBUG_METHOD_ENTER(IFRPacket_RequestPacket, 
                      IFRPacket_RequestPacket);
}

//----------------------------------------------------------------------
IFRPacket_RequestPacket::~IFRPacket_RequestPacket()
{
  DBUG_METHOD_ENTER(IFRPacket_RequestPacket, ~IFRPacket_RequestPacket);
  
  destroyPacketData();
}

//----------------------------------------------------------------------
void 
IFRPacket_RequestPacket::destroyPacketData()
{
    DBUG_METHOD_ENTER(IFRPacket_RequestPacket, destroyPacketData);
    if(m_packettype==Root_C) {
        IFRUtil_Delete(m_lock, allocator);
    } else {
        if(m_hasexclusivelock) {
            m_lock->releaseExclusiveLock();
        }
    }
    if(m_packettype==Dynamic_C) {
        IFRPacket_DynamicLock *dlock=(IFRPacket_DynamicLock*)m_lock;
        dlock->lock();
        if(m_lock->isLocked()) {
            dlock->unlock();
        } else {
            dlock->freePacket(this->rawPacket);
            IFRUtil_Delete(m_lock, allocator);
        }
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFRPacket_RequestPacket::assign(const IFRPacket_RequestPacket& rhs, IFR_ErrorHndl& error, IFR_Int4* in_use)
{
    if(this == &rhs) {
        // noop for self-assignment, this may actually happen
        return IFR_OK;
    }
    
    // delete the currently set data.
    destroyPacketData();
    
    this->rawPacket=rhs.rawPacket;
    m_CurrentSqlMode=rhs.m_CurrentSqlMode;
    m_lock=rhs.m_lock;

    switch(rhs.m_packettype) {
    case (Root_C):
    case (Root_Ref_C):
        m_packettype=Root_Ref_C;
        break;
    case (Dynamic_C):
        m_packettype=Dynamic_C;
    };

    if(m_lock) {
        m_lock->acquireExclusiveLock();

        // in an mt scenarion we can only have aquired the
        // exclusive lock when 'in_use' is not longer 'in_use'.
        if(rhs.m_packettype==Root_C || rhs.m_packettype==Root_Ref_C) {
            if(in_use && *in_use==1) {
                m_lock->releaseExclusiveLock();
                m_hasexclusivelock = false;
                this->rawPacket = 0;
                error.setRuntimeError(IFR_ERR_SESSION_ALREADY_IN_USE);
                return IFR_NO_DATA_FOUND;
            }
        }
        m_hasexclusivelock=true;
    } else {
        m_hasexclusivelock=false;
    }
    
    return IFR_OK;
}


//----------------------------------------------------------------------
const IFR_StringEncoding
IFRPacket_RequestPacket::getEncoding() const
{
    DBUG_METHOD_ENTER(IFRPacket_RequestPacket, getEncoding);
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

//----------------------------------------------------------------------
const IFR_StringEncoding
IFRPacket_RequestPacket::setEncoding(const IFR_StringEncoding encoding) const
{
    DBUG_METHOD_ENTER(IFRPacket_RequestPacket, setEncoding);
    DBUG_PRINT(encoding);
    IFR_StringEncoding old_encoding = getEncoding();
    if (old_encoding == encoding) 
      return old_encoding;
    tsp1_packet_header *header = (tsp1_packet_header *) GetRawHeader();
    if (header) {
        switch(encoding) {
        case IFR_StringEncodingAscii:
            header->sp1h_mess_code = csp_ascii;  
            break;
        case IFR_StringEncodingUCS2:
            header->sp1h_mess_code = csp_unicode;
            break;
        case IFR_StringEncodingUCS2Swapped:
            header->sp1h_mess_code = csp_unicode_swap;
            break;
        case IFR_StringEncodingUTF8:
            header->sp1h_mess_code = csp_utf8;
            break;
        default:
            DBUG_RETURN(IFR_StringEncodingUnknown);
        }
    }
    DBUG_RETURN(old_encoding);
}

IFR_Bool 
IFRPacket_RequestPacket::isSwapped() const 
{
    tsp1_packet_header *header = (tsp1_packet_header *) GetRawHeader();
    return header->sp1h_mess_swap == sw_full_swapped;
}

IFR_Byte *IFRPacket_RequestPacket::getFirstSegmentPtr ()
{
  return ((IFR_Byte *) &rawPacket->sp1_segm ());
}

void IFRPacket_RequestPacket::setMassCommand ()
{
  DBUG_METHOD_ENTER(IFRPacket_RequestPacket, setMassCommand);
  tsp1_segment * rawSegment;
  IFR_Byte *hdr;
  
  rawSegment = &this->rawPacket->sp1_segm ();
  PIn_RequestSegment result (rawSegment);
  hdr = (IFR_Byte *) result.GetRawHeader ();
  hdr += IFRPacket_Segment::MassCmd_O;
  memset(hdr, 1, 1);
}

void IFRPacket_RequestPacket::setWithInfo ()
{
  DBUG_METHOD_ENTER(IFRPacket_RequestPacket, setParseAgain);
  tsp1_segment * rawSegment;
  IFR_Byte *hdr;
  
  rawSegment = &this->rawPacket->sp1_segm ();
  PIn_RequestSegment result (rawSegment);
  hdr = (IFR_Byte *) result.GetRawHeader ();
  hdr += IFRPacket_Segment::WithInfo_O;
  memset(hdr, 1, 1);
}

void IFRPacket_RequestPacket::setParseAgain ()
{
  DBUG_METHOD_ENTER(IFRPacket_RequestPacket, setParseAgain);
  tsp1_segment * rawSegment;
  IFR_Byte *hdr;
  
  rawSegment = &this->rawPacket->sp1_segm ();
  PIn_RequestSegment result (rawSegment);
  hdr = (IFR_Byte *) result.GetRawHeader ();
  hdr += IFRPacket_Segment::ParsingAgain_O;
  memset(hdr, 1, 1);
}

//----------------------------------------------------------------------
PIn_RequestSegment
IFRPacket_RequestPacket::addSegment(int messagetype, IFR_Bool parseagain, int sqlmode)
{
  DBUG_METHOD_ENTER(IFRPacket_RequestPacket, addSegment);
  
  const tsp1_sqlmode_Enum use_mode=(tsp1_sqlmode_Enum)sqlmode;
  const pasbool use_parseagain=parseagain;
  return AddSegment((enum tsp1_cmd_mess_type_Enum)messagetype, use_mode, use_parseagain);
}
                                    
//----------------------------------------------------------------------
IFRPacket_Lock*
IFRPacket_RequestPacket::getLock()
{
    
    return m_lock;
}

//----------------------------------------------------------------------
IFR_Int2
IFRPacket_RequestPacket::getNumberOfSegments() const
{
    return this->rawPacket->sp1_header.sp1h_no_of_segm;
}

//----------------------------------------------------------------------
IFR_Int4
IFRPacket_RequestPacket::remainingBytes() const
{
    DBUG_METHOD_ENTER(IFRPacket_RequestPacket, remainingBytes);
    DBUG_RETURN(this->rawPacket->sp1_header.sp1h_varpart_size 
                - this->rawPacket->sp1_header.sp1h_varpart_len);
}

//----------------------------------------------------------------------
IFR_Int4
IFRPacket_RequestPacket::dataSize() const
{
    DBUG_METHOD_ENTER(IFRPacket_RequestPacket, dataSize);
    DBUG_RETURN(this->rawPacket->sp1_header.sp1h_varpart_size);
}

//----------------------------------------------------------------------
IFR_Bool 
IFRPacket_RequestPacket::copyInto(IFRPacket_RequestPacket& source)
{
    DBUG_METHOD_ENTER(IFRPacket_RequestPacket, copyInto);
    
    if (!(this->rawPacket != source.rawPacket)) return false;
    if (!(source.remainingBytes() >= 0)) return false;

    memcpy(this->rawPacket, source.rawPacket, source.Length());
    return true;
}

//----------------------------------------------------------------------
IFR_Retcode 
IFRPacket_RequestPacket::replaceParseIDAdjustData(IFR_ParseID* parseid,
                                                  IFR_size_t   length,
                                                  IFRUtil_Vector<IFR_Int4>& old_bufpos,
                                                  IFRConversion_ConverterList& shortinfo,
                                                  IFR_ConnectionItem& clink)
{
    DBUG_METHOD_ENTER(IFRPacket_RequestPacket, replaceParseIDAdjustData);
    
    if(!isValid()) {
        clink.error().setRuntimeError(IFR_ERR_INVALID_REQUESTPACKET);
        DBUG_RETURN(IFR_NOT_OK);
    }
    
    // reset to 1 segment and the stored length
    this->rawPacket->sp1_header.sp1h_no_of_segm  = 1;
    // the length was ** including the packet header **
    this->rawPacket->sp1_header.sp1h_varpart_len = length - sizeof (tsp1_packet_header);
    
    tsp1_segment *rawSegment = & this->rawPacket->sp1_segm();
    char *eos =  ((char *)rawSegment) + rawSegment->sp1s_segm_len();
    
    if(rawSegment->sp1s_segm_header().sp1s_no_of_parts()) {
        tsp1_part *part = &rawSegment->sp1p_part ();
        while( part->sp1p_part_header().sp1p_part_kind != sp1pk_parsid ) {
            int aligned_len = ALIGN_EO00(part->sp1p_buf_len(), 8); // parts are aligned at multiples of 8
            char *n = part->sp1p_buf ().asCharp () + aligned_len;
            if(n >= eos) {
                clink.error().setRuntimeError(IFR_ERR_INVALID_REQUESTPACKET);
                DBUG_RETURN(IFR_NOT_OK);
            }
            part = (tsp1_part*)n;
        }

        // finally paste the parse id in.
        memcpy(part->sp1p_buf (), parseid->getParseID(), IFR_ParseID_Size);
        
    } else {
        clink.error().setRuntimeError(IFR_ERR_INVALID_REQUESTPACKET);
        DBUG_RETURN(IFR_NOT_OK);
    }

    // Check if we have an awful statement, and the bufpos values have changed.
    IFR_Bool bufpos_changed=false;
    IFR_UInt4 rowsize       =0;   // while iterating, compute the row size
    IFR_Int2 colCount= (IFR_Int2) old_bufpos.GetSize();
    for(IFR_UInt4 bp=0; bp<old_bufpos.GetSize(); ++bp) {
        if(shortinfo[bp]->supportsInput()) {
            if(old_bufpos[bp] != shortinfo[bp]->getBufpos()) {
                bufpos_changed=true;
            }
            rowsize=MAX(rowsize, shortinfo[bp]->getBufpos() + shortinfo[bp]->getIOLength() - 1);
        }
    }
    // If the data positions remained unchanged, we have nothing to do
    if(!bufpos_changed) {
        DBUG_RETURN(IFR_OK);
    }
    
    tsp1_part *part = &rawSegment->sp1p_part ();
    while( part->sp1p_part_header().sp1p_part_kind != sp1pk_data) {
        int aligned_len = ALIGN_EO00(part->sp1p_buf_len(), 8); // parts are aligned at multiples of 8
        char *n = part->sp1p_buf ().asCharp () + aligned_len;
        if(n >= eos) {
            clink.error().setRuntimeError(IFR_ERR_INVALID_REQUESTPACKET);
            DBUG_RETURN(IFR_NOT_OK);
        }
        part = (tsp1_part*)n;
    }
    
    // now we have the tsp1_part, fine ...
    char     * data     = part->sp1p_buf().asCharp();
    IFR_Int2   argCount = part->sp1p_part_header().sp1p_arg_count;
    char     *tmp_buffer= (char *) alloca(rowsize);

   
    for(IFR_Int2 i=0; i<argCount; ++i) {
        // evacuate the data
        memcpy(tmp_buffer, data, rowsize);
        IFR_Int4 offset = rowsize;
        // now copy them according the short field info
        for(IFR_Int2 j=0; j<colCount; ++j) {
            if(shortinfo[j]->supportsInput()) {
                memcpy(data + shortinfo[j]->getBufpos() - 1,
                       tmp_buffer + old_bufpos[j] - 1,
                       shortinfo[j]->getIOLength());
                if(shortinfo[j]->isLong()) {
                    IFR_Int4 vallen;
                    memcpy(&vallen, 
                           data + shortinfo[j]->getBufpos() + IFRPacket_LongDescriptor::ValLen_O,
                           sizeof(IFR_Int4));
                    offset += vallen;
                }
            }
        }
        data += offset; 
    }
    DBUG_RETURN(IFR_OK);
}


//----------------------------------------------------------------------
IFR_TraceStream&
operator << (IFR_TraceStream& s, 
             const IFRPacket_RequestPacket::PacketType type)
{
    switch(type) {
    case IFRPacket_RequestPacket::Root_C:
        s << "ROOT";
        break;
    case IFRPacket_RequestPacket::Root_Ref_C:
        s << "ROOT REF";
        break;
    case IFRPacket_RequestPacket::Dynamic_C:
        s << "DYNAMIC";
        break;
    case IFRPacket_RequestPacket::Any_C:
        s << "ANY";
        break;
    default:
        s << "(unknown " << (IFR_Int4)type << ")";
        break;
    }
    return s;
}

IFR_END_NAMESPACE
