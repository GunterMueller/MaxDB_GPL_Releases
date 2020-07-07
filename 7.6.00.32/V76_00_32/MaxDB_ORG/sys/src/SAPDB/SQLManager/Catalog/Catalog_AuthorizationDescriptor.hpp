/*!
  @file           Catalog_AuthorizationDescriptor.hpp
  @author         ThomasA
  @ingroup        Catalog
  @brief          descriptor for the catalog table AUTHORIZATION

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


#ifndef CATALOG_AUTHORIZATIONDESCRIPTOR_HPP
#define CATALOG_AUTHORIZATIONDESCRIPTOR_HPP

#include "SQLManager/Catalog/Catalog_CatalogTableDescriptor.hpp"
#include "SQLManager/Catalog/Catalog_Cache.hpp"

/*!
   @brief describes the catalog table AUTHORIZATION
 */
class Catalog_AuthorizationDescriptor : public Catalog_CatalogTableDescriptor
{
public :
    /*!
       @brief constructor
     */
    Catalog_AuthorizationDescriptor(SQLMan_Context& Context);
    /*!
      @brief trigger called, whenever a row of table AUTHORIZATION is deleted
    */
    virtual tgg00_BasisError DeleteTrigger (RTE_TaskId TaskId, const tgg00_Rec& OldRecord);
    /*!
      @brief trigger called, whenever a row of table AUTHORIZATION is inserted
    */
    virtual void InsertTrigger (RTE_TaskId TaskId, const tgg00_Rec& NewRecord);
    /*!
      @brief trigger called, whenever a row of table AUTHORIZATION is updated
    */
    virtual void UpdateTrigger (RTE_TaskId TaskId, const tgg00_Rec& OldRecord, const tgg00_Rec& NewRecord);
private :
    /*! 
      @brief yields the value of column NAME for a given row of AUTHORIZATION
     */
    inline void GetName (const tgg00_Rec& Row, SQLMan_Identifier& Name) const;
    /*! 
      @brief yields the value of column AUTHORIZATIONID for a given row of AUTHORIZATION
     */
    inline void GetId   (const tgg00_Rec& Row, Catalog_ObjectId& Id) const;
};

#endif