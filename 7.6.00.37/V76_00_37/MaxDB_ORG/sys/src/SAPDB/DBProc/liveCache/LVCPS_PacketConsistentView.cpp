/*!
 * \file    LVC_PacketConsistentView.cpp
 * \author  MarkusSi
 * \brief   ProcServer packet for handling of consistent views.
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

#include "SAPDB/DBProc/liveCache/LVCPS_PacketConsistentView.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "SAPDB/Oms/OMS_DbpError.hpp"

void LVCPS_PacketConsistentView::execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf)
{
  tsp00_Int2  DBError         = 0;

  switch (m_type) {
    //--------------------------------------------------------------
    case msgNew: {
		/* TODO marshalling of new Parameter isOmsVersion * PTS 1130354 FF */
      sink->NewConsistentView(false, (unsigned char*)&m_consistentView, &DBError);

      interf->consistentViewReply(m_consistentView, DBError);
      break;
    }

    //--------------------------------------------------------------
    case msgEnd: {
      sink->EndConsistentView((unsigned char*)&m_consistentView, &DBError);

      interf->consistentViewReply(m_consistentView, DBError);
      break;
    }
    
    //--------------------------------------------------------------
    //default: {
    //  throw(DbpError(e_case_not_found, "LVCPS_PacketConsistentView::execute"));
    //}
  }
}
