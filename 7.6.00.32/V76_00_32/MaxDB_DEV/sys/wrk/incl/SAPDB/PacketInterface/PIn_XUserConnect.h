/*!================================================================

 module:        PIn_XUserConnect.h

 -------------------------------------------------------------------

 responsible:   DanielD

 special area:  SQL

 description:   XUser connect

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

#ifndef PIn_XUSERCONNECT_H
#define PIn_XUSERCONNECT_H

#include "vsp004.h"
#include "SAPDB/PacketInterface/PIn_Packet.h"

externCpp int PIn_XuserConnect (
    tsp4_xuser_record       & xuser,
    int                       packetCount,
    tsp00_Int4              & connectionHandle,
    PIn_RequestPacket      requestPacket []);

PIn_ReplyPacket PIn_Sqlaexecute (
    tsp00_Int4 connection,
    PIn_RequestPacket & requestPacket);


#endif

