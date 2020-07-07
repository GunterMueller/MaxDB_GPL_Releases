/*!
  @file           Catalog_AuthorizationRefObject.hpp
  @author         ThomasA 
  @ingroup        Catalog
  @brief          defines the authorization references object, which maps an authorization name to
                  the authorization id.

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
#ifndef CATALOG_AUTHORIZATIONREFOBJECT_HPP
#define CATALOG_AUTHORIZATIONREFOBJECT_HPP

#include "SQLManager/Catalog/Catalog_Object.hpp"

/*!
   @brief class realizing the mapping from authorization names to authorization id's
*/
class Catalog_AuthorizationRefObject : public Catalog_Object
{
public :
    /*!
       @brief defines the key of an authorization reference object
     */
    class Catalog_Key : public Catalog_Object::Catalog_Key
    {
    public :
        /*!
           @brief constructs the key of an authorization reference object
         */
        Catalog_Key(const SQLMan_Identifier& Name);
        /*!
           @brief yields the authorization name
         */
        void GetName(SQLMan_Identifier& Name) const;
    };
    /*!
       @breif constructor
     */
    Catalog_AuthorizationRefObject(const SQLMan_Identifier Name, const SQLMan_Surrogate AuthId);
    /*!
       @brief creates an object from the catalog tables using the specified allocator 
     */
    static Catalog_Object* Incopy(SQLMan_Context& Context, const SQLMan_Identifier& Name, SAPDBMem_IRawAllocator& Allocator);
    /*!
       @brief yields the size of an Catalog_AuthorizationRefObject
     */
    static int SizeOf();
};

#endif