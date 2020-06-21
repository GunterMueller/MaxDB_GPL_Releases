/*!***************************************************************************

  module:       SAPDB_ReuseLib.hpp

  responsible : D025086

  special area: Common Base Header for Reuse Library

  description:  

  last changed: 2002-03-10
  see also    :

  -------------------------------------------------------------------------


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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




*****************************************************************************/

/*
 * Do not SAPDB_USE_REUSESDK in this version.
 */
#ifndef SAPDB_REUSELIB_H
#define SAPDB_REUSELIB_H

#include "heo01.h"
#include "heo02.h"
#include "heo03.h"
#include "heo06.h"
#include "heo07.h"
#include "heo41.h"
#include "SAPDB/PacketInterface/PIn_FileTraceStream.h"
#include "SAPDB/PacketInterface/PIn_Packet.h"
#include "SAPDB/PacketInterface/PIn_ReplyParser.h"
#include "SAPDB/PacketInterface/PIn_RequestWriter.h"
#include "SAPDB/PacketInterface/PIn_XUserConnect.h"
#include "hsp77.h"
#include "hsp78_0.h"

#ifdef __cplusplus
extern "C" {
#endif
  #include "geo900.h"
#ifdef __cplusplus
}
#endif

#endif 