/*!
  @file           Catalog_MessBlockObject.hpp
  @author         ThomasA
  @brief          representation of mess block
  @ingroup        Catalog

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


#ifndef CATALOG_MESSBLOCKOBJECT_HPP
#define CATALOG_MESSBLOCKOBJECT_HPP
#include "SQLManager/Catalog/Catalog_Types.hpp"


class SAPDBMem_IRawAllocator;
class SQLMan_MessBlock;

/*!
  @brief encapsulation of pascal type tak_messblock
*/

class Catalog_MessBlockObject : public Catalog_MessBlockObjectPascal
{
public :
    /*!
       @brief creates a copy of the messblock using the given allocator.
       @param allocator [in] to be used for memory allocation
       @param offset [in] copy starts at returned pointer + offset
       @param memoryUsage [out] the memory used for the copy
    */
    void* CreateCopy (SAPDBMem_IRawAllocator& allocator, int offset, SAPDB_UInt4& memoryUsage);
    /*!
	   @brief deletes memory for mess block parts in given allocator
    */
	void DestroyParts(SAPDBMem_IRawAllocator& allocator);
    /*!
       @brief returns a const reference to the mess block contained in the instance
    */
    inline const SQLMan_MessBlock& GetMessBlock() const;
    /*!
       @brief invalidates the transaction info stored in the mess block.
    */
    void InvalidateTransaction();
    /*!
       @brief returns a reference to the mess block contained in the instance
    */
    inline SQLMan_MessBlock& MessBlock();
};

inline const SQLMan_MessBlock& Catalog_MessBlockObject::GetMessBlock() const
{
    return *REINTERPRET_CAST(const SQLMan_MessBlock*, &this->mbr_mess_block);
}

inline SQLMan_MessBlock& Catalog_MessBlockObject::MessBlock() 
{
    return *REINTERPRET_CAST(SQLMan_MessBlock*, &this->mbr_mess_block);
}

#endif