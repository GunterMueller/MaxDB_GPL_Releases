/*!
 * \file    LVC_PacketABAPStream.cpp
 * \author  MarkusSi
 * \brief   ProcServer packet for ABAP stream.-handling.
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

#include "SAPDB/DBProc/liveCache/LVCPS_PacketABAPStream.hpp"
#include "liveCache/LVC_IliveCacheSink.hpp"
#include "SAPDB/Oms/OMS_DbpError.hpp"

void LVCPS_PacketABAPStream::execute(IliveCacheSink *sink, LVC_ProcServerInterface *interf)
{
  tgg00_BasisError  error = 0;

  switch (m_type) {
    //--------------------------------------------------------------
    case msgRead: {
      OmsTypeInt4   count   = 0;
      OmsTypeInt4   bufSize = 0;
      void *pData = NULL;

      interf->getObjectForRead(pData, m_dataOffset);

      //sink->ABAPRead(m_tabHandle, 
      //               count, bufSize, 
      //               (unsigned char*&)pData, 
      //               error);

      interf->ABAPStreamReply(m_tabHandle, count, bufSize, error);
      break;
    }

    //--------------------------------------------------------------
    case msgWrite: {
      void *pData = NULL;

      interf->getObjectForRead(pData, m_dataOffset);
      //sink->ABAPWrite(&m_tabHandle, 
      //                m_rowSize,
      //                m_rowCount,
      //                (unsigned char*&)pData, 
      //                &error);

      interf->errorStateReply(false, error);
      break;
    }
    

    //--------------------------------------------------------------
    //default: {
    //  throw(DbpError(e_case_not_found, "LVCPS_PacketABAPStream::execute"));
    //}
  }
}
