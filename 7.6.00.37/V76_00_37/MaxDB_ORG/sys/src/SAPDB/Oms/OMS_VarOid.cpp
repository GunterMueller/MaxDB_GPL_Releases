/*!
 * \file    OMS_VarOid.cpp
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


#include "Oms/OMS_VarOid.hpp"
#include "Oms/OMS_Handle.hpp"
#include "Oms/OMS_Globals.hpp"


OmsVarOid::OmsVarOid( OmsHandle&h, const OmsExternalOid&r )
{
  OmsObjectId::operator=( r );
  if (!omsCheckOid( h )) {
    OMS_ObjectId8 oid8(reinterpret_cast<const OmsObjectId&>(r));
    OMS_Globals::Throw(e_incompatible_oid, "OmsVarOid::OmsVarOid", oid8, __MY_FILE__, __LINE__);
  }
}

bool OmsVarOid::omsCheckOid ( OmsHandle&h, const OmsExternalOid &r )
{
  return h.omsCheckOid(reinterpret_cast<const OmsVarOid&>(r));
}

bool OmsVarOid::omsCheckOid ( OmsHandle&h ) const
{
  return h.omsCheckOid(*this);
}

const void* OmsVarOid::operator()(OmsHandle& h) const 
{
  return h.omsDeRef(*this);
}

const void* OmsVarOid::omsDeRef(OmsHandle& h) const
{
  return h.omsDeRef(*this);
}
