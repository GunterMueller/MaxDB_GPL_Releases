/*!*********************************************************************

    @file         PkgCheck_Iterator.hpp

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

#ifndef PKGCHECK_ITERATOR_HPP
#define PKGCHECK_ITERATOR_HPP

#include "vsp001.h"

/*!

  @class PkgCheck_Iterator

 */
class PkgCheck_Iterator
{
public:
    enum {noneYet, wasPacket, wasSegment, wasPart, eoPacket, error};
    PkgCheck_Iterator (tsp1_packet * packet, int size);
    bool next ();
    int                 lastKind;
    tsp1_packet_header  packetHeader;
    tsp1_segment_header segmentHeader;
    int                 segmentIndex;
    int                 segmentOffset;
    tsp1_part_header    partHeader;
    int                 partIndex;
    int                 partOffset;
    const void        * partBuf;
    char                errorMsg [64];

private:

    bool firstPacket ();
    void readSegment (int offset);
    bool firstSegment ();
    bool nextSegment ();
    void readPart (int offset);
    bool firstPart ();
    bool nextPart ();

    const char * rawpacket;
    int          packetSize;
    bool         requiresSwap;
};

#endif
