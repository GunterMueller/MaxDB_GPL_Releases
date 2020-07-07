/*!*********************************************************************

    @file         PkgCheck_Checker.hpp

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

#ifndef PKGCHECK_CHECKER_HPP
#define PKGCHECK_CHECKER_HPP

struct tsp1_packet;

/*!

  @class PkgCheck_Checker

 */
class PkgCheck_Checker
{
public:
    PkgCheck_Checker (tsp1_packet * packet_init, int packetSize_init, bool logErrors_init = false);
    bool check (char * error);

private:
    tsp1_packet * packet;
    int           packetSize;
    bool          logErrors;
};

extern "C" bool PkgCheck_CheckPacket (tsp1_packet * packet, int packetSize, char * error);

#endif
