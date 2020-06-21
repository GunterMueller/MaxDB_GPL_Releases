/*!
  @file           Catalog_DBProcFileObject.hpp
  @author         ThomasA
  @brief          memory representation of a db-procedure's file info  
  @ingoup         Catalog

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


#ifndef CATALOG_DBPROCFILEOBJECT_HPP
#define CATALOG_DBPROCFILEOBJECT_HPP
#include "SQLManager/Catalog/Catalog_Types.hpp"
#include "SQLManager/Catalog/Catalog_Object.hpp"

/*!
  @brief encapsulation of pascal type tak_method_fileinforecord. 
         Describes a db-procedure file info
*/

class Catalog_DBProcFileObject : public Catalog_MethodFileObjectPascal
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
       @brief returns the external information as 0 terminated string. 
              create dbproc 'name' ... as language java name 'external information'
              The caller doesn't have to take care about memory management
     */
     const char* GetExternalInfo() const;
};

#endif