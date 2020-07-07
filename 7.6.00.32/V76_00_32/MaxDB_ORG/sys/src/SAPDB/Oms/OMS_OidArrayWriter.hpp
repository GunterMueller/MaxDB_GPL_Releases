/*!================================================================
 module:        OMS_OidArrayWriter.hpp

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


#ifndef OMS_OIDARRAYWRITER_HPP
#define OMS_OIDARRAYWRITER_HPP


#include "Oms/OMS_Defines.h"
#include "Oms/OMS_Oid.hpp"


/*! 
 * \brief Derived class for writing an array of OIDs
 */
class OMS_OidArrayWriter : public OmsIOidAppender
{
public:
  OMS_OidArrayWriter(OmsObjectId* pOids, int cntOids);
  bool omsAppend(const OmsObjectId& currOid);
  int omsGetCntr() const;
private:
  OmsObjectId* m_pOids;
  int m_cntOids;
  int m_cntr;
};


#endif
