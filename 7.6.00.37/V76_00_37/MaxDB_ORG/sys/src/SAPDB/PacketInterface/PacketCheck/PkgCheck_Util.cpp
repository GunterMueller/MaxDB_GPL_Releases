/*!*********************************************************************

    @file         PkgCheck_Util.cpp

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

#include "PacketInterface/PacketCheck/PkgCheck_Util.hpp"

#include "vsp001.h"

/*----------------------------------------*/

bool PkgCheck_Util::
isSwapped ()
{
    static unsigned int i = 1;
    static unsigned char * alias = (unsigned char *) &i;
    return alias [0] != '\0';
}

/*----------------------------------------*/

inline void
swap4Bytes (void * rawbytes)
{
    unsigned char * bytes = (unsigned char *) rawbytes;
    unsigned char tmp;
    tmp = bytes [0];
    bytes [0] = bytes [3];
    bytes [3] = tmp;
    tmp = bytes [1];
    bytes [1] = bytes [2];
    bytes [2] = tmp;
}

/*----------------------------------------*/

inline void
swap2Bytes (void * rawbytes)
{
    unsigned char * bytes = (unsigned char *) rawbytes;
    unsigned char tmp;
    tmp = bytes [0];
    bytes [0] = bytes [1];
    bytes [1] = tmp;
}

/*----------------------------------------*/

void PkgCheck_Util::
swapPacketHeader (
    tsp1_packet_header * header)
{
    swap4Bytes (&header->sp1h_varpart_size);
    swap4Bytes (&header->sp1h_varpart_len);
    swap2Bytes (&header->sp1h_no_of_segm);
}

/*----------------------------------------*/

void PkgCheck_Util::
swapSegmentHeader (
    tsp1_segment_header * header)
{
    swap4Bytes (&header->sp1s_segm_len ());
    swap4Bytes (&header->sp1s_segm_offset ());
    swap2Bytes (&header->sp1s_no_of_parts ());
    swap2Bytes (&header->sp1s_own_index ());
    switch (header->sp1s_segm_kind ()) {
        case sp1sk_cmd:
        case sp1sk_proccall:
            /* no integer fields for this variant */
            break;
        case sp1sk_return:
        case sp1sk_procreply:
            swap2Bytes (&header->sp1r_returncode ());
            swap4Bytes (&header->sp1r_errorpos ());
            swap2Bytes (&header->sp1r_function_code ());
            break;
    }
}

/*----------------------------------------*/

void PkgCheck_Util::
swapPartHeader (
    tsp1_part_header * header)
{
    swap2Bytes (&header->sp1p_arg_count);
    swap4Bytes (&header->sp1p_segm_offset);
    swap4Bytes (&header->sp1p_buf_len);
    swap4Bytes (&header->sp1p_buf_size);
}

/*----------------------------------------*/

int PkgCheck_Util::
align8 (
    int inval)
{
    int remainder = inval % 8;
    if (remainder == 0) {
        return inval;
    }
    else {
        return inval + 8 - remainder;
    }
}

/*----------------------------------------*/


