/*!
 * \file    LVC_PacketSQL.cpp
 * \author  MarkusSi
 * \brief   ProcServer packet for operations on a version.
 */
/*

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



*/

#include "SAPDB/DBProc/liveCache/LVCPS_PacketSQL.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "SAPDB/Oms/OMS_DbpError.hpp"

void LVCPS_PacketSQL::execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf)
{
  switch (m_type) {
    //--------------------------------------------------------------
    case msgRequest: {
      void  *pData = NULL;
      interf->getObjectForRead(pData, m_offset);

      sink->SQLRequest(m_length, (unsigned char*)pData);

      interf->simpleReply(0);
      break;
    }
    
    //--------------------------------------------------------------
    //default: {
    //  throw(DbpError(e_case_not_found, "LVCPS_PacketSQL::execute"));
    //}
  }
}
