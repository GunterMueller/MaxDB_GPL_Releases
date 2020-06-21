/*!
  @file           PacketSpy.cpp
  @author         D025086
  @ingroup        Tests
  @brief          Simulation of comunication and order interface errors.
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
#include "SAPDB/Interfaces/tests/PacketSpy.h"
#include "SAPDB/RunTime/MemoryManagement/RTEMem_RteAllocator.hpp"

static tsp1_packet *
GetSegmentPacket (
    tsp1_segment * segment)
{
    int segmOffset;
    teo00_Byte * segmAddr;
    teo00_Byte * packetAddr;
    tsp1_packet * packet;

    segmOffset = segment->sp1s_segm_offset ();
    segmAddr = REINTERPRET_CAST (teo00_Byte*, segment);
    packetAddr = segmAddr - segmOffset - sizeof (tsp1_packet_header);
    packet = REINTERPRET_CAST (tsp1_packet*, packetAddr);
    return packet;
}

ReplySegment::ReplySegment (tsp1_segment * segmParm)
    : PIn_Segment (segmParm)
{
}

void ReplySegment::ClosePart (PIn_Part & part)
{
    int alignedLen = ALIGN_EO00 (part.Length (), packetAlignment_PI);
    this->ExtendLength (alignedLen);
}

PIn_Part ReplySegment::AddPart (tsp1_part_kind_Param partKind)
{
    tsp1_part * partAddr;
    tsp1_segment_header * header = this->GetRawHeader ();
    teo00_Byte * rawPtr;

    // calculate part address
    rawPtr = REINTERPRET_CAST (teo00_Byte *, this->rawSegment)
        + this->rawSegment->sp1s_segm_len ();
    partAddr = REINTERPRET_CAST (tsp1_part *, rawPtr);
    // create Part object
    PIn_Part result (partAddr);
    int bytesRemaining = this->Size () - header->sp1s_segm_len ();
    result.Init (partKind, header->sp1s_segm_offset (), bytesRemaining);
    ++header->sp1s_no_of_parts ();
    this->ExtendLength (sizeof (tsp1_part_header));
    // return Part
    return result;
}

void ReplySegment::ExtendLength (int increment)
{
    this->rawSegment->sp1s_segm_len () += increment;
    GetSegmentPacket (this->rawSegment)->sp1_header.sp1h_varpart_len += increment;
}

void ReplySegment::Init (int offset, int index)
{
  tsp1_segment_header * header = &this->rawSegment->sp1s_segm_header ();
  
  memset (header, 0, sizeof (tsp1_segment_header));
  header->sp1s_segm_len () = 0;
  header->sp1s_segm_offset () = offset;
  header->sp1s_own_index () = index;
  header->sp1s_segm_kind ().becomes (sp1sk_return);
  header->sp1c_mess_type ().becomes (sp1m_nil);
  header->sp1c_sqlmode ().becomes (sp1sm_nil);
  header->sp1c_producer ().becomes (sp1pr_nil);
  memcpy(header->sp1r_sqlstate(), "00000", 5);
  header->sp1c_parsing_again () = false;
  this->ExtendLength (sizeof (tsp1_segment_header));
}

void ReplySegment::setErrorCode(tsp00_Int2 retcode)
{
    this->rawSegment->sp1r_returncode() = retcode;
}

ReplyPacket::ReplyPacket(tsp1_packet * packetPtr, int size, int unicode)
  : PIn_RequestPacket (packetPtr, size, unicode)
{
}

ReplySegment ReplyPacket::AddSegment ( )
{
    int newOffset = this->rawPacket->sp1_header.sp1h_varpart_len;
    tsp1_segment * segmPtr = REINTERPRET_CAST (tsp1_segment *,
        this->rawPacket->sp1_varpart ().asCharp () + newOffset);
    ReplySegment result (segmPtr);
    ++this->rawPacket->sp1_header.sp1h_no_of_segm;
    result.Init (newOffset, this->rawPacket->sp1_header.sp1h_no_of_segm);
    return result;
}

SAPDB_Bool PacketSpyRuntime::request(SAPDB_Int8  sessionID,
                               void       *requestData,
                               SAPDB_UInt4 requestDataLength,
                               SQLDBC_IRuntime::Error& error)
{
  if (!m_count && m_requestData) {
    memcpy(requestData, m_requestData, m_requestDataLength);
    m_requestData = NULL;
    m_requestDataLength = 0;
  }
  SAPDB_Bool ret = SQLDBC_ClientRuntime::request(sessionID,
                                                requestData, requestDataLength,
                                                error);
  if (!m_count && ret && m_requesterror ) {
    ret = false;
    memcpy(&error, &m_error, sizeof(m_error));
    clearRTEError();
  }
  return ret;
}

PacketSpyRuntime::PacketSpyRuntime()
   :m_requesterror(false),
    m_replyerror(false),
    m_sqlerror(false),
    m_sqlcode(0),
    m_sqlerrtext(RTEMem_RteAllocator::Instance())
{
  m_retpacket = (tsp1_packet *) new char[1000];
  m_replyData = NULL;
  m_requestData = NULL;
  m_count = 0;
}

SAPDB_Bool PacketSpyRuntime::receive(SAPDB_Int8 sessionID,
                               void **replyData,
                               SAPDB_Int4& replyDataLength,
                               SQLDBC_IRuntime::Error& error)
{
  SAPDB_Bool ret = SQLDBC_ClientRuntime::receive(sessionID,
                                                 replyData, replyDataLength,
                                                 error);
  if (!m_count && ret && m_replyerror ) {
    ret = false;
    memcpy(&error, &m_error, sizeof(m_error));
    clearRTEError();
  }
  if (!m_count && ret && m_sqlerror ) {
    tsp1_packet *packet = (tsp1_packet *)*replyData;
    tsp1_packet_header *header = &packet->sp1_header;
    char *component = header->sp1h_application.asCharp();
    char *version = header->sp1h_appl_version.asCharp();
    int unicode = (header->sp1h_mess_code != csp_ascii);
    ReplyPacket reply( m_retpacket, 1000, unicode);
    reply.InitVersion(component, version);
    ReplySegment segment = reply.AddSegment();
    PIn_Part part = segment.AddPart(sp1pk_errortext);
    part.AddData(m_sqlerrtext.getBuffer(), MIN(m_sqlerrtext.getLength(), 40));
    part.AddArgument();
    segment.ClosePart(part);
    segment.setErrorCode(m_sqlcode);
    *replyData = m_retpacket;
    replyDataLength = reply.Length()+sizeof(packet->sp1_header);
    clearSQLError();
  }
  else if (!m_count && ret && m_replyData) {
    memcpy(*replyData, m_replyData, m_replyDataLength);
    replyDataLength = m_replyDataLength;
    m_replyData = NULL;
    m_replyDataLength = 0;
  }
  if (m_count) m_count--;
  return ret;
}

void PacketSpyRuntime::setRTEError(tsp01_CommErr_Enum commErr, const char *errtext, bool OnReceive)
{
  if (OnReceive)
    m_replyerror = true;
  else
    m_requesterror = true;
  IFR_ErrorHndl tmperror(RTEMem_RteAllocator::Instance());
  tmperror.setRuntimeError(IFR_ERR_CONNECTION_DOWN_IIS, (int)commErr,
                           strlen(errtext), errtext);
  tmperror.toRuntimeError(m_error);
}

void PacketSpyRuntime::setSQLError(tsp00_Int2 sqlcode,const IFR_String& errtext)
{
  m_sqlerror = true;
  m_sqlcode = sqlcode;
  IFR_Bool memory_ok=true;
  m_sqlerrtext.assign(errtext, memory_ok);
}

void PacketSpyRuntime::setReplyData(void *replyData, SAPDB_UInt4 replyDataLength, SAPDB_UInt4 count)
{
  m_count = count;
  m_replyData = replyData;
  m_replyDataLength = replyDataLength;

}
void PacketSpyRuntime::setRequestData(void *requestData, SAPDB_UInt4 requestDataLength, SAPDB_UInt4 count)
{
  m_count = count;
  m_requestData = requestData;
  m_requestDataLength = requestDataLength;
}

void PacketSpyRuntime::clearRTEError()
{
  m_requesterror = false;
  m_replyerror = false;
}

void PacketSpyRuntime::clearSQLError()
{
  m_sqlerror = false;
}
