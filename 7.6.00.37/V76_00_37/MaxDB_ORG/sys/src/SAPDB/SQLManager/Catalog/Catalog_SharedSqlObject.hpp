/*!
  @file           Catalog_SharedSqlObject.hpp
  @author         ThomasA
  @brief          implements a catalog object containing shared Sql properties
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


#ifndef CATALOG_SHAREDSQLOBJECT_HPP
#define CATALOG_SHAREDSQLOBJECT_HPP

#include "SQLManager/Catalog/Catalog_Types.hpp"
#include "SQLManager/Catalog/Catalog_Object.hpp"

/*!
   @brief class storing properties of a shared sql plan.
*/

class Catalog_SharedSqlObject : public Catalog_Object
{
private :
    /*!
       @brief defines the header of the shared sql object
     */
    struct SharedSqlObject
    {
          SAPDB_Int2       m_reclen;      
          SAPDB_Int2       m_keylen;         
          SAPDB_Int2       m_varcol_offset;  
          SAPDB_Int2       m_varcol_cnt;     
          SQLMan_Surrogate m_surrogate;
          tsp00_C2         m_entrytype;
          tsp00_C2         m_linkage;
          tsp00_C12        m_describeId;
    };
public :
    /*!
       @brief defines the key of a shared sql object
     */
    class Catalog_Key : public Catalog_Object::Catalog_Key
    {
    public :
        /*!
           @brief default constructor
         */
        Catalog_Key();
    };
    /*!
    @brief constructor, stores decribe id
    */
    Catalog_SharedSqlObject(const tsp00_C12& describeId);
    /*!
    @brief returns the decribe id stored in the instance
    */
    const tsp00_C12& GetDescribeId() const
    {
        return REINTERPRET_CAST(const SharedSqlObject*, this)->m_describeId;
    }
};

#endif