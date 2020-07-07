/*!
  @file           Catalog_CatalogTableDescriptor.hpp
  @author         ThomasA
  @ingroup        Catalog
  @brief          defines the interface of a descriptor that describes a catalog table

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

#ifndef CATALOG_CATALOGTABLEDESCRIPTOR_HPP
#define CATALOG_CATALOGTABLEDESCRIPTOR_HPP

#include "SQLManager/Catalog/Catalog_TableDescriptor.hpp"

/*!
   @brief defines the interface of a table descriptor describing a catalog table. These descriptors must
          implement trigger routines, which invalidate the shared catalog cache in case of 
          updates of rows of these tables.
 */

class Catalog_CatalogTableDescriptor : public Catalog_TableDescriptor
{
public :
    /*!
     @brief constructs a descriptor for table SYSCATALOG.<TableName>
     */
     Catalog_CatalogTableDescriptor(SQLMan_Context& Context, const SQLMan_Identifier& TableName);
     /*!
        @brief this method is called, whenever a row of SYSCATALOG.<TableName> is deleted
      */
     virtual tgg00_BasisError DeleteTrigger (RTE_TaskId TaskId, const tgg00_Rec& OldRecord) = 0;
     /*!
        @brief this method is called, whenever a row is interted into SYSCATALOG.<TableName>
      */
     virtual void InsertTrigger (RTE_TaskId TaskId, const tgg00_Rec& NewRecord) = 0;
     /*!
        @brief this method is called, whenever a row of SYSCATALOG.<TableName> is updated
      */
     virtual void UpdateTrigger (RTE_TaskId TaskId, const tgg00_Rec& OldRecord, const tgg00_Rec& NewRecord) = 0;
};

#endif