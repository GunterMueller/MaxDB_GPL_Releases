/*!
  @file           Catalog_ViewtextObject.cpp
  @author         ThomasA
  @brief          memory representation of a view definition string 
  @ingoup         Catalog

\if EMIT_LICENCE

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



\endif
*/


#include "SQLManager/Catalog/Catalog_ViewtextObject.hpp"

Catalog_ViewtextObject::Catalog_Key::Catalog_Key(const Catalog_ObjectId& ViewId)
{
    SAPDB_MemCopyNoCheck (&this->stableid()[0], &ViewId, sizeof(this->stableid()));
    this->sentrytyp()[0] = 0;
    this->sentrytyp()[1] = TYPE_VIEWTEXT;
    this->slinkage()[0]  = 0;
    this->slinkage()[1]  = 1;
}

//-----------------------------------------------------------------------------

tgg00_DateTimeFormat_Enum Catalog_ViewtextObject::GetDateTimeFormat() const
{
    if (dtf_was_ansi_now_is_same_as_iso == vtcontext % 10)
    {
        return dtf_iso;
    }
    return static_cast<tgg00_DateTimeFormat_Enum>(vtcontext % 10);
}