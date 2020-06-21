/*!***************************************************************************

  module      : Catalog_DBProcInfoObject.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: Catalog Manager
  description : Catalog DBProc File Info Object

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

#include "SQLManager/Catalog/Catalog_DBProcInfoObject.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"


Catalog_DBProcInfoObject::Catalog_Key::Catalog_Key(const SQLMan_Surrogate& procId)
: Catalog_Object::Catalog_Key()
{
    SAPDB_MemCopyNoCheck (&this->stableid()[0], &procId[0], sizeof(procId));
    this->sentrytyp()[1] = TYPE_METHODINFO[1];
    this->slinkage()[1]  = 1;
}

//-------------------------------------------------------------------------

const SQLMan_Identifier& Catalog_DBProcInfoObject::GetLanguage() const
{
    return *REINTERPRET_CAST(const SQLMan_Identifier*, &mei_language[0]);
}

//-------------------------------------------------------------------------

const SQLMan_Identifier& Catalog_DBProcInfoObject::GetName() const
{
    return *REINTERPRET_CAST(const SQLMan_Identifier*, &mei_name[0]);
}

//-------------------------------------------------------------------------

SAPDB_Int4 Catalog_DBProcInfoObject::GetCreateDate() const
{
    return mei_date;
}

//-------------------------------------------------------------------------

SAPDB_Int4 Catalog_DBProcInfoObject::GetCreateTime() const
{
    return mei_time;
}

//-------------------------------------------------------------------------

const Catalog_ObjectId& Catalog_DBProcInfoObject::GetSchemaId() const
{
    return *reinterpret_cast<const Catalog_ObjectId*>(&mei_schema[0]);
}