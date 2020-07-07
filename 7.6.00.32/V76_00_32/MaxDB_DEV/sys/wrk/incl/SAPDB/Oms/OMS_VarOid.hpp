/*!
 * \file    OMS_VarOid.hpp
 * \author  IvanS, MarkusSi, PeterG
 * \brief   OID for variable sized persistent objects
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

#ifndef __OMS_VAROID_HPP
#define __OMS_VAROID_HPP

#include "Oms/OMS_Oid.hpp"

class OmsHandle;

/*--------------------------------------------------------------------*/
/// Defines an OID of a persistent object of variable length.
struct AFX_EXT_CLASS OmsVarOid : public OmsObjectId
{
public:
  /// Constructor
  OmsVarOid() : OmsObjectId() {}
  OmsVarOid(OmsHandle&h, const OmsExternalOid&r);

  static bool omsCheckOid ( OmsHandle&, const OmsExternalOid& r );
  bool        omsCheckOid ( OmsHandle& ) const;

  const void* operator()(OmsHandle&) const;
  const void* omsDeRef(OmsHandle&) const;
};

#endif // __OMS_VAROID_HPP
