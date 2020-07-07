/*!
  @file           Catalog_IndexIterator.hpp
  @author         ThomasA
  @ingroup        Catalog
  @brief          provides an iterator over all indexes of a base table

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

#ifndef CATALOG_INDEXITERATOR_HPP
#define CATALOG_INDEXITERATOR_HPP

#include "SQLManager/Catalog/Catalog_IndexDescriptor.hpp"

class  Catalog_IndexScanContext;
class  Catalog_TableDescriptor;

/*!
   @brief implements an iterator over all indexes of a base table
 */
class Catalog_IndexIterator
{
public :
     /*!
       @brief copy constructor
     */
    Catalog_IndexIterator (const Catalog_IndexIterator& Source);
    /*!
       @brief destructor, release resources
     */
    virtual ~Catalog_IndexIterator();
    /*!
    @brief steps to the next index
    */
    void operator++();
    /*!
    @brief true, if the iterator points to an index; otherwise false
    */
    operator bool() const;
    /*!
    @brief returns true, if 2 iterators are equal
    */
    bool operator== (const Catalog_IndexIterator& Compare) const;
    /*!
    @brief returns true, if 2 iterators are not equal
    */
    bool operator!= (const Catalog_IndexIterator& Compare) const;
    /*!
    @brief yields a column descriptor for the current column
    */
    const Catalog_IndexDescriptor operator*() const;
    /*!
    @brief assignment
    */
    void operator= (const Catalog_IndexIterator& Source);
private :
    Catalog_IndexIterator (SQLMan_Context& Context, const Catalog_TableDescriptor& TableDescriptor, bool WithKey);
    SQLMan_Context*           m_pContext;        //!< pointer to the sql manager context
    Catalog_IndexScanContext* m_iteratorContext; //!< current iterator state
    Catalog_TableDescriptor*  m_pTableDescriptor; //!< current table descriptor
    bool                      m_currIsKey;
    friend class Catalog_TableDescriptor;
};

#endif