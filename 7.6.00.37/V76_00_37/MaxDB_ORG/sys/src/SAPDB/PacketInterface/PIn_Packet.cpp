/*!================================================================
 module:        PIn_Packet.cpp

 -------------------------------------------------------------------

 responsible:   TTK

 special area:  SQL

 description:   Packet

 -------------------------------------------------------------------

 Copyright (c) 2000-2005 SAP AG




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




 ===================================================================*/

#include <stdio.h>

#define VSP100_CPP
#include "hsp100.h"
#include "SAPDB/PacketInterface/PIn_Packet.h"

/*----------------------------------------*/

PIn_SQLPacket::PIn_SQLPacket (tsp1_packet * packetPtr)
{
    this->rawPacket = packetPtr;
}

/*----------------------------------------*/

PIn_SQLPacket::~PIn_SQLPacket ()
{
}

/*----------------------------------------*/

static const char * messCodeStrings [] = {
    "ascii",
    "ebcdic",
    "codeneutral",
    "unknown3",
    "unknown4",
    "unknown5",
    "unknown6",
    "unknown7",
    "unknown8",
    "unknown9",
    "unknown10",
    "unknown11",
    "unknown12",
    "unknown13",
    "unknown14",
    "unknown15",
    "unknown16",
    "unknown17",
    "unknown18",
    "unicode_swap",
    "unicode"
};

static const PIn_EnumDesc messCodeDesc = {
    csp_unicode,
    "Invalid messcode %d",
    messCodeStrings
};

static const char * swapTypeStrings [] = {
    "dummy",
    "normal",
    "full",
    "part"
};

static const PIn_EnumDesc swapTypeDesc = {
    sw_part_swapped,
    "Unknwon %d",
    swapTypeStrings
};

/*----------------------------------------*/

void PIn_SQLPacket::TraceOn (PIn_TraceStream * trace, int buflimit)
{
    tsp1_packet_header * header = &this->rawPacket->sp1_header;
    char tmpBuf [tmpBufSize_PI] [2];

    if (trace != NULL) {
      if (!this->IsValid ()) {
        trace->writeln ("Invalid Packet");
      }
      trace->printf ("<Packet> %s  %s swap  %.3s-%.5s (transfer len %d)\n",
		     PIn_EnumDump (&messCodeDesc, header->sp1h_mess_code, tmpBuf [0]),
		     PIn_EnumDump (&swapTypeDesc, header->sp1h_mess_swap, tmpBuf [1]),
		     header->sp1h_application.asCharp (),
		     header->sp1h_appl_version.asCharp (),
		     this->Length ());
      trace->printf ("    %3d Segment(s) (%d of %d bytes)\n",
		     header->sp1h_no_of_segm, header->sp1h_varpart_len,
		     header->sp1h_varpart_size);
      
      PIn_ReplyPacket tmpPacket (this->rawPacket);
      PIn_ReplySegment segment = tmpPacket.GetFirstSegment ();
      while (segment.IsValid ()) {
        segment.TraceOn (trace, buflimit);
        segment = tmpPacket.GetNextSegment (segment);
      }
      trace->writeln ("</Packet>");
    }
}

/*----------------------------------------*/

int PIn_SQLPacket::Length () const
{
    return this->rawPacket->sp1_header.sp1h_varpart_len
        + sizeof (tsp1_packet_header);
}

/*----------------------------------------*/

bool PIn_SQLPacket::IsValid () const
{
    return this->rawPacket != NULL;
}

/*----------------------------------------*/

void PIn_SQLPacket::Invalidate ()
{
    this->rawPacket = NULL;
}

/*----------------------------------------*/

tsp1_packet * PIn_SQLPacket::GetRawPacket ()
{
    return this->rawPacket;
}

/*----------------------------------------*/

const tsp1_packet * PIn_SQLPacket::GetRawPacket () const
{
    return this->rawPacket;
}

/*----------------------------------------*/

tsp1_packet_header * PIn_SQLPacket::GetRawHeader ()
{
    return &this->rawPacket->sp1_header;
}

/*----------------------------------------*/

const tsp1_packet_header * PIn_SQLPacket::GetRawHeader () const
{
    return &this->rawPacket->sp1_header;
}

/*----------------------------------------*/

PIn_RequestPacket::PIn_RequestPacket (tsp1_packet * packetPtr, 
                                      int size, 
                                      int unicode)
    : PIn_SQLPacket (packetPtr)
{
    tsp1_packet_header * header = &this->rawPacket->sp1_header;
    union {
        tsp00_Int4 asInt;
        char       asChar [4];
    } swapTest;

    memset (header, '\0', sizeof (tsp1_packet_header));
    if  ( unicode ) {
      union { tsp00_Uint2 s; tsp00_Byte c[2]; } ucs2_blank;
      ucs2_blank.s = 32;
      if ((ucs2_blank.c[0] ? 1 : 0 )) {
        header->sp1h_mess_code = csp_unicode_swap;
      }
      else {
        header->sp1h_mess_code = csp_unicode;
      };
    } 
    else {
      header->sp1h_mess_code = csp_ascii;
    }

    swapTest.asInt = 1;
    if (swapTest.asChar [3] == 1) {
        header->sp1h_mess_swap.becomes (sw_normal);
    }
    else if (swapTest.asChar [0] == 1) {
        header->sp1h_mess_swap.becomes (sw_full_swapped);
    }
    else {
        header->sp1h_mess_swap.becomes (sw_part_swapped);
    }
    header->sp1h_varpart_len = 0;
    header->sp1h_no_of_segm = 0;
    header->sp1h_varpart_size = size - sizeof (tsp1_packet_header);
    this->InitVersion ("CPC");
}

//----------------------------------------------------------------------
PIn_RequestPacket::PIn_RequestPacket (tsp1_packet * packetPtr, 
                                      int size, 
                                      int unicode,
                                      const char *compname,
                                      const char *version)
: PIn_SQLPacket (packetPtr)
{
    tsp1_packet_header * header = &this->rawPacket->sp1_header;
    union {
        tsp00_Int4 asInt;
        char       asChar [4];
    } swapTest;

    memset (header, '\0', sizeof (tsp1_packet_header));
    if  ( unicode ) {
      union { tsp00_Uint2 s; tsp00_Byte c[2]; } ucs2_blank;
      ucs2_blank.s = 32;
      if ((ucs2_blank.c[0] ? 1 : 0 )) {
        header->sp1h_mess_code = csp_unicode_swap;
      }
      else {
        header->sp1h_mess_code = csp_unicode;
      };
    } 
    else {
      header->sp1h_mess_code = csp_ascii;
    }

    swapTest.asInt = 1;
    if (swapTest.asChar [3] == 1) {
        header->sp1h_mess_swap.becomes (sw_normal);
    }
    else if (swapTest.asChar [0] == 1) {
        header->sp1h_mess_swap.becomes (sw_full_swapped);
    }
    else {
        header->sp1h_mess_swap.becomes (sw_part_swapped);
    }
    header->sp1h_varpart_len = 0;
    header->sp1h_no_of_segm = 0;
    header->sp1h_varpart_size = size - sizeof (tsp1_packet_header);
    this->InitVersion (compname, version);
}


/*----------------------------------------*/


PIn_RequestPacket::PIn_RequestPacket (tsp1_packet * packetPtr)
    : PIn_SQLPacket (packetPtr)
{
    // We're done with the construction of the packet pointer
}


/*----------------------------------------*/

PIn_RequestPacket::PIn_RequestPacket ()
    : PIn_SQLPacket (NULL)
{
}

/*----------------------------------------*/

PIn_RequestPacket::~PIn_RequestPacket ()
{
    // nothing to do
}


/*----------------------------------------*/

void PIn_RequestPacket::InitVersion (const char * component, const char * version)
{
    this->rawPacket->sp1_header.sp1h_application.c2p (component);
    if (version != NULL) {
        this->rawPacket->sp1_header.sp1h_appl_version.c2p (version);
    }
    else {
      char szVersion[40];
      sprintf(szVersion,"%d%02d%02d", MAJOR_VERSION_NO_SP100, MINOR_VERSION_NO_SP100, CORRECTION_LEVEL_SP100);
      this->rawPacket->sp1_header.sp1h_appl_version.c2p (szVersion);
    }
}

/*----------------------------------------*/

void PIn_RequestPacket::Reset ()
{
    tsp1_packet_header * header = &this->rawPacket->sp1_header;

    header->sp1h_varpart_len = 0;
    header->sp1h_no_of_segm = 0;
}

/*----------------------------------------*/

void PIn_RequestPacket::CloseSegment (PIn_RequestSegment & segment)
{
    // nothing to do because segment keeps packet up to date
}

/*----------------------------------------*/

/* PTS 1108446 sqlMode added      */
/* PTS 1112690 parsingAgain added */
PIn_RequestSegment PIn_RequestPacket::AddSegment (tsp1_cmd_mess_type_Param messType,
                                                        const tsp1_sqlmode_Enum &sqlMode,
                                                        const pasbool parsingAgain )
{
    int newOffset = this->rawPacket->sp1_header.sp1h_varpart_len;
    tsp1_segment * segmPtr = REINTERPRET_CAST (tsp1_segment *,
        this->rawPacket->sp1_varpart ().asCharp () + newOffset);
    PIn_RequestSegment result (segmPtr);
    ++this->rawPacket->sp1_header.sp1h_no_of_segm;
    /* PTS 1108446 sqlMode added */
    result.Init (messType, sqlMode, parsingAgain, newOffset, this->rawPacket->sp1_header.sp1h_no_of_segm);
    return result;
}

/*----- PTS 1119960 ----------------------*/

PIn_RequestSegment PIn_RequestPacket::AddMassSegment (tsp1_cmd_mess_type_Param messType,
                                                            const tsp1_sqlmode_Enum &sqlMode,
                                                            const pasbool parsingAgain )
{
    int newOffset = this->rawPacket->sp1_header.sp1h_varpart_len;
    tsp1_segment * segmPtr = REINTERPRET_CAST (tsp1_segment *,
        this->rawPacket->sp1_varpart ().asCharp () + newOffset);
    PIn_RequestSegment result (segmPtr);
    ++this->rawPacket->sp1_header.sp1h_no_of_segm;
    result.MassInit (messType, sqlMode, parsingAgain, newOffset, this->rawPacket->sp1_header.sp1h_no_of_segm);
    return result;
}

/*----------------------------------------*/

PIn_ReplyPacket::PIn_ReplyPacket (tsp1_packet * packetPtr)
    : PIn_SQLPacket (packetPtr)
{
}

/*----------------------------------------*/

PIn_ReplyPacket::~PIn_ReplyPacket ()
{
    // nothing to do
}

/*----------------------------------------*/

const PIn_ReplySegment PIn_ReplyPacket::GetFirstSegment () const
{
    tsp1_segment * rawSegment;

    rawSegment = &this->rawPacket->sp1_segm ();
    PIn_ReplySegment result (rawSegment);
    return result;
}

/*----------------------------------------*/

const PIn_ReplySegment PIn_ReplyPacket::GetNextSegment (
    const PIn_ReplySegment & segment) const
{
    const tsp1_segment * segmPointer;
    const teo00_Byte * segmAddr;
    int nextSegmPos;
    const teo00_Byte * nextSegmAddr;
    const tsp1_segment * nextSegmPointer;

    segmPointer = segment.GetRawSegment ();
    segmAddr = REINTERPRET_CAST (const teo00_Byte *, segmPointer);
    nextSegmAddr = NULL;
    nextSegmPos = segmPointer->sp1s_segm_header().sp1s_segm_offset()
        + segment.Length ();   
    if (this->rawPacket->sp1_header.sp1h_varpart_len > nextSegmPos) {
        nextSegmAddr = segmAddr + segment.Length();
    } 
    nextSegmPointer = REINTERPRET_CAST (const tsp1_segment*, nextSegmAddr);
    PIn_ReplySegment result (CONST_CAST (tsp1_segment*, nextSegmPointer));
    return result;
}

/*----------------------------------------*/


