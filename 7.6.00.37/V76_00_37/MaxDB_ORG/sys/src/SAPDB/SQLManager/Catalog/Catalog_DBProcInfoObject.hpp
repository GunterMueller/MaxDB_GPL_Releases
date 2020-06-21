/*!
  @file           Catalog_DBProcInfoObject.hpp
  @author         ThomasA
  @brief          memory representation of a db-procedure's info  

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


#ifndef CATALOG_DBPROCINFOOBJECT_HPP
#define CATALOG_DBPROCINFOOBJECT_HPP
#include "SQLManager/Catalog/Catalog_Types.hpp"
#include "SQLManager/Catalog/Catalog_Object.hpp"

/*!
  @brief encapsulation of pascal type tak_method_infoinforecord. 
         contains db-procedure properties
*/

class Catalog_DBProcInfoObject : public Catalog_MethodInfoObjectPascal
{
public :
    /*!
       @brief key of a db-procedure file object
    */
    class Catalog_Key : public Catalog_Object::Catalog_Key
    {
    public :
        /*!
           @brief constructor
        */
        Catalog_Key(const SQLMan_Surrogate& procId);
    };
    /*!
       @brief returns the create date in an integer representation
    */
    SAPDB_Int4 GetCreateDate() const;
    /*!
       @brief returns the create time in an integer representation
    */
    SAPDB_Int4 GetCreateTime() const;
    /*!
       @brief returns the name of the db-procedure
    */
    const SQLMan_Identifier& GetName() const;
    /*!
       @brief returns the implementation language of the db-procedure
    */
    const SQLMan_Identifier& GetLanguage() const;
    /*!
       @brief returns the id of the schema containing the procedure
     */
    const Catalog_ObjectId& GetSchemaId() const;
};

#endif