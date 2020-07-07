/*!***************************************************************************

  module      : Catalog_MessBlockObject.cpp

  -------------------------------------------------------------------------

  responsible : ThomasA

  special area: Catalog Manager
  description : Catalog Messblock Object

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

#include "SQLManager/Catalog/Catalog_MessBlockObject.hpp"
#include "SQLManager/SQLMan_MessBlock.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IRawAllocator.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

void* Catalog_MessBlockObject::CreateCopy (SAPDBMem_IRawAllocator& allocator, int offset, SAPDB_UInt4& memoryUsage)
{
    const bool cDeepCopy = true;
    void* p = SQLMan_MessBlock::CreateCopy (this->GetMessBlock(), allocator, 
		offset + CATALOG_OFFSET(Catalog_MessBlockObject, mbr_mess_block), cDeepCopy, memoryUsage);
	if (p)
	{
		// copy offset of mbr_mess_block
		SAPDB_MemCopyNoCheck (REINTERPRET_CAST(unsigned char*, p) + offset, this, 
			CATALOG_OFFSET(Catalog_MessBlockObject, mbr_mess_block));
	}
	return p;
}

//---------------------------------------------------------------------------

void Catalog_MessBlockObject::DestroyParts(SAPDBMem_IRawAllocator& allocator)
{
  REINTERPRET_CAST(SQLMan_MessBlock*, &this->mbr_mess_block)->DestroyParts(allocator);
}

//---------------------------------------------------------------------------

void Catalog_MessBlockObject::InvalidateTransaction()
{
    REINTERPRET_CAST(SQLMan_MessBlock*, &this->mbr_mess_block)->InvalidateTransaction();
}