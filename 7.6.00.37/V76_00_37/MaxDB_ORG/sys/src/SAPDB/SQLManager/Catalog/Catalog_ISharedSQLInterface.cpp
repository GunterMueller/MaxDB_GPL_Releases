/*!***************************************************************************

  module      : Catalog_ISharedSQLInterface.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: Catalog Manager
  description : interface for shared SQL manager

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

#include "hsp30.h"
#include "SQLManager/Catalog/Catalog_ISharedSQLInterface.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SQLManager/Catalog/Catalog_Object.hpp"
#include "SQLManager/Catalog/Catalog_SessionCacheInline.hpp"
#include "SQLManager/SQLMan_Tracing.hpp"

int Catalog_ISharedSQLInterface::Compare (const void* l, const void* r)
{
    const Catalog_Object* pLeft  = Catalog_SessionCache::GetObject(l);
    const Catalog_Object* pRight = Catalog_SessionCache::GetObject(r);
    tsp00_LcompResult res;
    s30cmp (pLeft->GetKey(), 1, pLeft->KeyLength(), pRight->GetKey(), 1, pRight->KeyLength(), res);
	SQLMAN_TRACE_BUFFER (Catalog_Trace, 1, "Compare : left  :", pLeft->GetKey(),  pLeft->KeyLength());
	SQLMAN_TRACE_BUFFER (Catalog_Trace, 1, "Compare : right :", pRight->GetKey(), pRight->KeyLength());
    if (l_less == res   ) return -1;
    if (l_greater == res) return  1;
    return 0;
}

//----------------------------------------------------------------------------------------------

void Catalog_ISharedSQLInterface::DestroyPlanObject (SAPDBMem_IRawAllocator& allocator, void* p)
{
    SAPDBTRACE_ROUTINE_DEBUG ("Catalog_ISharedSQLInterface::DestroyPlanObject", Catalog_Trace, 5);
	Catalog_SessionCache::DestroyPlanObject(allocator, p);
}

