/*!
  @file           Catalog_SchemaObject.hpp
  @author         ThomasA
  @brief          implementation of the catalog object describing a schema   
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


#ifndef CATALOG_SCHEMAOBJECT_HPP
#define CATALOG_SCHEMAOBJECT_HPP

#include "SQLManager/Catalog/Catalog_AuthorizationObject.hpp"

/*!
   @brief transient description of a schema 
*/

class Catalog_SchemaObject : public Catalog_AuthorizationObject
{
public :
    /*!
       @brief returns the id of the owner of the schema
     */
    inline const Catalog_ObjectId& GetSchemaOwnerId() const;
};

//--------------------------------------------------------------------------------------------

inline const Catalog_ObjectId& Catalog_SchemaObject::GetSchemaOwnerId() const
{
    if (IsUserObject())
    {
        return GetObjectId();
    }
    else
    {
        return GetOwnerId();
    }
}

#endif



