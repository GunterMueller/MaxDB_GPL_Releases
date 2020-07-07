/*!================================================================
 module:        OMS_OidArrayWriter.cpp

 responsible:   PeterG
 
 special area:  OMS
 
 description:   OID handling
 
 see:           
 
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


#include "Oms/OMS_OidArrayWriter.hpp"


OMS_OidArrayWriter::OMS_OidArrayWriter(OmsObjectId* pOids, int cntOids)
{
  m_pOids = pOids;
  m_cntOids = cntOids;
  m_cntr = 0;
}

bool OMS_OidArrayWriter::omsAppend(const OmsObjectId& currOid)
{
  if (m_cntr < m_cntOids)
  {
    *m_pOids = currOid;
    ++m_cntr;
    ++m_pOids;
    return true;
  }
  else
    return false;
}

int OMS_OidArrayWriter::omsGetCntr() const
{
  return m_cntr;
}

