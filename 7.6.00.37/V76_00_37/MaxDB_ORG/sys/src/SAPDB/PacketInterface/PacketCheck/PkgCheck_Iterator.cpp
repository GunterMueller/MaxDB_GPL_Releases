/*!*********************************************************************

    @file         PkgCheck_Iterator.cpp

    @author       DanielD

    @brief

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2004-2005 SAP AG

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
***********************************************************************/

#include "PacketInterface/PacketCheck/PkgCheck_Iterator.hpp"

#include "PacketInterface/PacketCheck/PkgCheck_Util.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"

/*----------------------------------------*/

PkgCheck_Iterator::
PkgCheck_Iterator (
    tsp1_packet * packet,
    int size)
{
    this->rawpacket = (const char *) packet;
    this->packetSize = size;
    bool localIsSwapped = PkgCheck_Util::isSwapped ();
    bool remoteIsSwapped = packet->sp1_header.sp1h_mess_swap == sw_full_swapped;
    this->requiresSwap = localIsSwapped != remoteIsSwapped;
    this->lastKind = noneYet;
    memset (&this->packetHeader, '\0', sizeof (this->packetHeader));
    memset (&this->segmentHeader, '\0', sizeof (this->segmentHeader));
    memset (&this->partHeader, '\0', sizeof (this->partHeader));
    this->partBuf = 0;
    this->errorMsg [0] = '\0';
}

/*----------------------------------------*/

bool
PkgCheck_Iterator::
next ()
{
    switch (this->lastKind) {
        case noneYet:
            return this->firstPacket ();
            break;
        case wasPacket:
            return this->firstSegment ();
            break;
        case wasSegment:
            if (this->segmentHeader.sp1s_no_of_parts () == 0) {
                return this->nextSegment ();
            }
            else {
                return this->firstPart ();
            }
            break;
        case wasPart:
            if (this->partIndex == this->segmentHeader.sp1s_no_of_parts ()) {
                return this->nextSegment ();
            }
            else {
                return this->nextPart ();
            }
        case eoPacket:
        case error:
            return false;
        default:
            return false;
    }
}

/*----------------------------------------*/

bool PkgCheck_Iterator::
firstPacket ()
{
    SAPDB_MemCopyNoCheck (&this->packetHeader, this->rawpacket, sizeof (tsp1_packet_header));
    if (this->requiresSwap) {
        PkgCheck_Util::swapPacketHeader (&this->packetHeader);
    }
    this->lastKind = wasPacket;
    return true;
}

/*----------------------------------------*/

void PkgCheck_Iterator::
readSegment (
    int offset)
{
    if (offset > this->packetSize - (int) sizeof (tsp1_segment_header)) {
        strcpy (this->errorMsg, "segment out of bounds");
        this->lastKind = error;
        return;
    }
    this->segmentOffset = offset;
    SAPDB_MemCopyNoCheck (&this->segmentHeader, this->rawpacket + this->segmentOffset,
        sizeof (tsp1_segment_header));
    if (this->requiresSwap) {
        PkgCheck_Util::swapSegmentHeader (&this->segmentHeader);
    }
    this->lastKind = wasSegment;
}

/*----------------------------------------*/

bool PkgCheck_Iterator::
firstSegment ()
{
    if (this->packetHeader.sp1h_no_of_segm == 0) {
        this->lastKind = eoPacket;
        return false;
    }
    this->readSegment (sizeof (tsp1_packet_header));
    this->segmentIndex = 1;
    return true;
}

/*----------------------------------------*/

bool PkgCheck_Iterator::
nextSegment ()
{
    if (this->packetHeader.sp1h_no_of_segm == this->segmentIndex) {
        this->lastKind = eoPacket;
        return false;
    }
    this->readSegment (this->segmentOffset +
        PkgCheck_Util::align8 (this->segmentHeader.sp1s_segm_len ()));
    ++this->segmentIndex;
    return true;
}

/*----------------------------------------*/

void PkgCheck_Iterator::
readPart (
    int offset)
{
    if (offset > this->packetSize - (int) sizeof (tsp1_part_header)) {
        strcpy (this->errorMsg, "part out of bounds");
        this->lastKind = error;
        return;
    }
    this->partOffset = offset;
    SAPDB_MemCopyNoCheck (&this->partHeader, this->rawpacket + this->partOffset,
        sizeof (tsp1_part_header));
    if (this->requiresSwap) {
        PkgCheck_Util::swapPartHeader (&this->partHeader);
    }
    this->partBuf = this->rawpacket + this->partOffset + sizeof (tsp1_part_header);
    this->lastKind = wasPart;
}

/*----------------------------------------*/

bool PkgCheck_Iterator::
firstPart ()
{
    if (this->segmentHeader.sp1s_no_of_parts () == 0) {
        this->lastKind = eoPacket;
        return false;
    }
    this->readPart (this->segmentOffset + sizeof (tsp1_segment_header));
    this->partIndex = 1;
    return true;
}

/*----------------------------------------*/

bool PkgCheck_Iterator::
nextPart ()
{
    if (this->segmentHeader.sp1s_no_of_parts () == this->partIndex) {
        this->lastKind = eoPacket;
        return false;
    }
    this->readPart (this->partOffset
        + sizeof (tsp1_part_header)
        + PkgCheck_Util::align8 (this->partHeader.sp1p_buf_len));
    ++this->partIndex;
    return true;
}

/*----------------------------------------*/

