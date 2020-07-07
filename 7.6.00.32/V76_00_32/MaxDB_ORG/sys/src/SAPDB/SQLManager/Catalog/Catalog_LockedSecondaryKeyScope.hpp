/*!
  @file           Catalog_LockedSecondaryKeyScope.hpp
  @author         ThomasA
  @ingroup        Catalog
  @brief          locks a secondary in the scope of this class

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

#ifndef CATALOG_LOCKEDSECONDARYKEYSCOPE_HPP
#define CATALOG_LOCKEDSECONDARYKEYSCOPE_HPP

#include "SQLManager/Catalog/Catalog_Types.hpp"

class SQLMan_Context;

/*!
   @brief locks on secondary key. Releases the lock at destruction
 */
class Catalog_LockedSecondaryKeyScope
{
public :
    /*!
       @brief constructor
     */
    Catalog_LockedSecondaryKeyScope(SQLMan_Context& Context);
    /*!
       @brief destructor, releases the lock if granted
     */
    ~Catalog_LockedSecondaryKeyScope();
     /*!
       @brief locks the given key
     */
    int Lock(const Catalog_ObjectId& IndexId, const SAPDB_Byte* pKey, int KeyLength, SAPDB_Byte DefinedByte);
private :
    SQLMan_Context&  m_context; //!< sql manager context
    tgg00_Lkey       m_key;     //!< the key to be locked
    Catalog_ObjectId m_indexId; //!< index id
};

#endif