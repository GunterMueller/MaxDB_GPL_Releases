/*!***************************************************************************

  module      : Catalog_SharedSqlObject.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: Catalog
  description : catalog record storing properties of a shared sql statement

  last changed: 2000-12-06  19:04
  see also    : example.html ...
  first created:2000-03-09  18:21

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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




*****************************************************************************/

#include "SQLManager/Catalog/Catalog_SharedSqlObject.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

Catalog_SharedSqlObject::Catalog_Key::Catalog_Key()
: Catalog_Object::Catalog_Key()
{
    this->sentrytyp()[1] = TYPE_SHARED_SQL;
}

//------------------------------------------------------------------------

Catalog_SharedSqlObject::Catalog_SharedSqlObject(const tsp00_C12& describeId)
: Catalog_Object(Catalog_Object::DefaultKeyLen, sizeof(Catalog_SharedSqlObject::SharedSqlObject))
{
    SharedSqlObject* pSharedSqlObject = REINTERPRET_CAST(SharedSqlObject*, this);
    pSharedSqlObject->m_entrytype[1]  = TYPE_SHARED_SQL;
    SAPDB_MemCopyNoCheck(&pSharedSqlObject->m_describeId[0], &describeId[0], sizeof(describeId));
}