/*!
 * \file    LVC_PacketSequence.cpp
 * \author  MarkusSi
 * \brief   ProcServer packet for handling of sequences.
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

#include "SAPDB/DBProc/liveCache/LVCPS_PacketSequence.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "SAPDB/Oms/OMS_DbpError.hpp"

void LVCPS_PacketSequence::execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf)
{
  tgg00_BasisError  error = 0;

  switch (m_type) {
    //--------------------------------------------------------------
    case msgAttachSeq: {
      sink->AttachSequence(m_sequenceName, 
                           m_sequenceId, m_sequenceInfo, 
                           error);

      interf->sequenceReply(&m_sequenceId, &m_sequenceInfo, 0, error);
      break;
    }

    //--------------------------------------------------------------
    case msgNextVal: {
      tsp00_Uint4   nextVal = 0;

      sink->NextVal(m_sequenceId, m_sequenceInfo, nextVal, error);

      interf->sequenceReply(NULL, &m_sequenceInfo, nextVal, error);
      break;
    }
    
    
    //--------------------------------------------------------------
    //default: {
    //  throw(DbpError(e_case_not_found, "LVCPS_PacketSequence::execute"));
    //}
  }
}
