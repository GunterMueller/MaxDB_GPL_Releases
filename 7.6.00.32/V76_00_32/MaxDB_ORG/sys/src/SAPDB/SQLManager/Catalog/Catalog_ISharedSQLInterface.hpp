/*!
  @file           Catalog_ISharedSQLInterface.hpp
  @author         ThomasA
  @brief          defines catalog methods used by shared SQL component

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

#ifndef CATALOG_ISHAREDSQLINTERFACE_HPP
#define CATALOG_ISHAREDSQLINTERFACE_HPP

class SAPDBMem_IRawAllocator;

/*!
  @brief defines a set of catalog functions used by shared SQL component
*/

class Catalog_ISharedSQLInterface
{
public :
    /*!
       @brief destoyes a catalog object stored in a plan of the  shared sql manager
       @param allocator to be user for releasing memory
       @param p pointer to the catalog object
    */
    static void DestroyPlanObject (SAPDBMem_IRawAllocator& allocator, void* p);
    /*!
       @brief compares the key of two catalog objects
    */
    static int  Compare(const void* NodeKey, const void* SearchKey);
};

#endif
