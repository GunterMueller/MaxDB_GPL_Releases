/*!*********************************************************************

    @file         PkgCheck_Checker.cpp

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

#include "PacketInterface/PacketCheck/PkgCheck_Checker.hpp"

#include <stdio.h>

#include "gsp00.h"
#include "vsp001.h"
#include "PacketInterface/PacketCheck/PkgCheck_Iterator.hpp"
#include "PacketInterface/PacketCheck/PkgCheck_Util.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_VarData.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"

PkgCheck_Checker::
PkgCheck_Checker (
    tsp1_packet * packet_init,
    int packetSize_init,
    bool logErrors_init)
{
    this->packet = packet_init;
    this->packetSize = packetSize_init;
    this->logErrors = logErrors_init;
}

/*----------------------------------------*/

static bool
setError (
    char * errorBuf,
    const char * errorMsg)
{
    strcpy (errorBuf, errorMsg);
    return false;
}

/*----------------------------------------*/

static bool
checkPacketHeader (
    tsp1_packet_header * packetHeader,
    int                  packetSize,
    char               * error)
{
    switch (packetHeader->sp1h_mess_code) {
        case csp_ascii:
        case csp_ebcdic:
        case csp_codeneutral:
        case csp_unicode_swap:
        case csp_unicode:
        case csp_unicode_native:
        case csp_utf8:
        case csp_instance_code:
            break;
        default:
            return setError (error, "wrong code kind");
            break;
    }
    if (packetHeader->sp1h_varpart_size < 0) {
        return setError (error, "invalid varpart size");
    }
    if ((packetHeader->sp1h_varpart_len < 0) || (packetHeader->sp1h_varpart_len > packetHeader->sp1h_varpart_size)) {
        return setError (error, "invalid varpart len");
    }
    int calculatedLength = packetHeader->sp1h_varpart_len + sizeof (tsp1_packet_header);
    if (calculatedLength > packetSize) {
        return setError (error, "exceeding physical packet");
    }
    if (packetHeader->sp1h_no_of_segm < 1) {
        return setError (error, "at least one segment required");
    }
    return true;
}

/*----------------------------------------*/

static bool
checkSegmentHeader (
    tsp1_packet_header  * packetHeader,
    int                   segmentOffset,
    int                   segmentIndex,
    tsp1_segment_header * segmentHeader,
    char                * error)
{
    if (segmentHeader->sp1s_segm_offset () != (segmentOffset - (int) sizeof  (tsp1_packet_header))) {
        return setError (error, "segment internal offset wrong");
    }
    int segmLen = segmentHeader->sp1s_segm_len ();
    if (segmLen < sizeof (tsp1_segment_header)) {
        return setError (error, "segment length too short");
    }
    if (segmentOffset - (int) sizeof (tsp1_packet_header) + segmLen > packetHeader->sp1h_varpart_len) {
        /* try alternative calculation for faulty Python driver */
        int adjustedVarpartLen = PkgCheck_Util::align8 (packetHeader->sp1h_varpart_len
            + sizeof (tsp1_part_header));
        if (segmentOffset - (int) sizeof (tsp1_packet_header) + segmLen > adjustedVarpartLen) {
            return setError (error, "segment too large");
        }
    }
    if (segmentHeader->sp1s_no_of_parts () < 0) {
        return setError (error, "part count negative");
    }
    if (segmentHeader->sp1s_own_index () != segmentIndex) {
        return setError (error, "Segment internal index wrong");
    }
    bool isRequest;
    switch (segmentHeader->sp1s_segm_kind ()) {
        case sp1sk_cmd:
        case sp1sk_proccall:
            isRequest = true;
            break;
        case sp1sk_return:
        case sp1sk_procreply:
            isRequest = false;
            break;
        default:
            return setError (error, "unknown segment kind");
            break;
    }
    if (isRequest) {
        //sp1c_mess_type          : tsp1_cmd_mess_type;
        //sp1c_sqlmode            : tsp1_sqlmode;
        //sp1c_producer           : tsp1_producer;
        //(*  *)
        //sp1c_commit_immediately : boolean;
        //sp1c_ignore_costwarning : boolean;
        //sp1c_prepare            : boolean;
        //sp1c_with_info          : boolean;
        //sp1c_mass_cmd           : boolean;
        //sp1c_parsing_again      : boolean;
        //sp1c_command_options    : tsp1_command_options;
    }
    else {
        // sp1r_returncode         : tsp00_Int2;
        // sp1r_errorpos           : tsp00_Int4;
        // (*  *)
        // sp1r_extern_warning     : tsp00_WarningSet;
        // sp1r_intern_warning     : tsp1_warningset_intern;
        // sp1r_function_code      : tsp00_Int2; (* csp1_.._fc *)
        // sp1r_tracelevel         : tsp00_Uint1;
    }
    return true;
}

/*----------------------------------------*/

static bool
checkPartHeader (
    tsp1_segment_header * segmentHeader,
    int                   partOffset,
    tsp1_part_header    * partHeader,
    char                * error)
{
    // sp1p_part_kind   : tsp1_part_kind;
    // sp1p_attributes  : tsp1_part_attr;
    if (partHeader->sp1p_arg_count < 0) {
        return setError (error, "invalid number of arguments");
    }
    if (segmentHeader->sp1s_segm_offset () != partHeader->sp1p_segm_offset) {
        return setError (error, "part internal offset wrong");
    }
    if (partHeader->sp1p_buf_len < 0) {
        return setError (error, "part len too small");
    }
    int partExtent = partOffset + sizeof (tsp1_part_header)
        + partHeader->sp1p_buf_len;
    int segmentExtent = sizeof (tsp1_packet_header)
        + segmentHeader->sp1s_segm_offset ()
        + segmentHeader->sp1s_segm_len ();
    if (partExtent > segmentExtent) {
        return setError (error, "part len too large");
    }
    // sp1p_buf_size    : tsp00_Int4
    return true;
}

/*----------------------------------------*/

static bool
checkPartData (
    tsp1_packet         * packet,
    int                   partOffset,
    tsp1_part_header    * partHeader,
    char                * error)
{
    char * rawPacket = (char *) packet;
    void * partData = rawPacket + partOffset + sizeof (tsp1_part_header);
    unsigned char extract [200];

    switch (partHeader->sp1p_part_kind) {
        case sp1pk_vardata:
            memcpy (extract, rawPacket + partOffset, sizeof (extract));
            // the part inside packet isn't swapped yet
            // so we don't build a regular tsp1_part
            if (!SAPDBFields_VarData::Verify (partData, partHeader->sp1p_buf_len)) {
                return setError (error, "invalid vardata part");
            }
            break;
        default:
            /*
             * no internal struture to check
             */
            break;
    }
    return true;
}

/*----------------------------------------*/
#if 0
bool PkgCheck_Checker::
check (
    char * error)
{
    tsp00_SwapKind_Enum localSwap = PkgCheck_Util::isSwapped ()
        ? sw_full_swapped : sw_normal;
    tsp00_SwapKind_Enum remoteSwap = this->packet->sp1_header.sp1h_mess_swap;
    bool requireSwap;
    if ((remoteSwap != sw_normal) && (remoteSwap != sw_full_swapped)) {
        if (this->logErrors) {
            return setError (error, "wrong swap kind");
        }
    }
    if (localSwap != remoteSwap) {
        requireSwap = true;
    }
    else {
        requireSwap = false;
    }
    /*
     * check header
     */
    tsp1_packet_header packetHeader;
    SAPDB_MemCopyNoCheck (&packetHeader, &this->packet->sp1_header, sizeof (packetHeader));
    if (requireSwap) {
        PkgCheck_Util::swapPacketHeader (&packetHeader);
    }
    if (!checkPacketHeader (&packetHeader, error)) {
        return false;
    }
    /*
     * check segments
     */
    char * rawpacket = (char *) this->packet;
    unsigned int segmentPos = sizeof (tsp1_packet_header);
    for (int segi = 1; segi <= packetHeader.sp1h_no_of_segm; ++segi) {
        /*
         * check segment header
         */
        if (segmentPos > packetHeader.sp1h_varpart_len - sizeof (tsp1_segment_header)) {
            return setError (error, "segment outside of packet");
        }
        tsp1_segment_header segmentHeader;
        SAPDB_MemCopyNoCheck (&segmentHeader, rawpacket + segmentPos, sizeof (tsp1_segment_header));
        if (requireSwap) {
            PkgCheck_Util::swapSegmentHeader (&segmentHeader);
        }
        if (!checkSegmentHeader (&packetHeader, segmentPos, segi,
                &segmentHeader, error))
        {
            return false;
        }
        /*
         * check parts
         */
        unsigned int partOffset = segmentPos + sizeof (tsp1_part_header);
        for (int parti = 1; parti <= segmentHeader.sp1s_no_of_parts (); ++parti) {
            /*
             * check par header
             */
            if (partOffset > packetHeader.sp1h_varpart_len - sizeof (tsp1_part_header)) {
                return setError (error, "part outside of packet");
            }
            tsp1_part_header partHeader;
            SAPDB_MemCopyNoCheck (&partHeader, rawpacket + partOffset, sizeof (tsp1_part_header));
            if (requireSwap) {
                PkgCheck_Util::swapPartHeader (&partHeader);
            }
            if (!checkPartHeader (&segmentHeader, partOffset,
                    &partHeader, error))
            {
                return false;
            }
            partOffset += sizeof (tsp1_part_header) + aligned8 (partHeader.sp1p_buf_len);
        }
        segmentPos += aligned8 (segmentHeader.sp1s_segm_len ());
    }
    return true;
}
#endif
/*----------------------------------------*/


bool PkgCheck_Checker::
check (
    char * error)
{
    tsp00_SwapKind_Enum remoteSwap = this->packet->sp1_header.sp1h_mess_swap;
    if ((remoteSwap != sw_normal) && (remoteSwap != sw_full_swapped)) {
        if (this->logErrors) {
            return setError (error, "wrong swap kind");
        }
    }
    PkgCheck_Iterator iter (this->packet, this->packetSize);
    while (iter.next ()) {
        switch (iter.lastKind) {
            case PkgCheck_Iterator::wasPacket:
                if (!checkPacketHeader (&iter.packetHeader, this->packetSize, error)) {
                    return false;
                }
                break;
            case PkgCheck_Iterator::wasSegment:
                if (!checkSegmentHeader (
                        &iter.packetHeader, iter.segmentOffset, iter.segmentIndex,
                        &iter.segmentHeader, error)) {
                    return false;
                }
                break;
            case PkgCheck_Iterator::wasPart:
                if (!checkPartHeader (
                        &iter.segmentHeader, iter.partOffset,
                        &iter.partHeader, error))
                {
                    return false;
                }
                if (!checkPartData (this->packet, iter.partOffset,
                        &iter.partHeader, error))
                {
                    return false;
                }
                break;
            case PkgCheck_Iterator::error:
                return setError (error, iter.errorMsg);
                break;
            default:
                return setError (error, "invalid iterator state");
        }
    }
    return true;
}

/*----------------------------------------*/

extern "C" bool
PkgCheck_CheckPacket (
    tsp1_packet * packet,
    int           packetSize,
    char        * error)
{
    PkgCheck_Checker checker (packet, packetSize);
    return checker.check (error);
}

