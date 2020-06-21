/*!
  @file           Catalog_ViewDescriptor.hpp
  @author         ThomasA
  @ingroup        Catalog
  @brief          yields information about a view

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


#ifndef CATALOG_VIEWDESCRIPTOR_HPP
#define CATALOG_VIEWDESCRIPTOR_HPP

#include "SQLManager/Catalog/Catalog_TableDescriptor.hpp"

/*!
   @brief provides properties of a view table 
 */
class Catalog_ViewDescriptor : public Catalog_TableDescriptor
{
public :
    typedef tgg00_DateTimeFormat_Enum DateTimeFormat; //!< date time enum 
    typedef tsp00_SqlMode_Enum        SQLMode;        //!< sql mode enum  
    /*!
       @brief constructor
     */
    Catalog_ViewDescriptor(SQLMan_Context& Context, const Catalog_ObjectId& ViewId);
    /*!
       @brief copy constructor
     */
    Catalog_ViewDescriptor(const Catalog_ViewDescriptor& Source)
        : Catalog_TableDescriptor (Source)
    {
    }
    /*!
       @brief return the date/time format at view definition time
     */
    DateTimeFormat GetDateTimeFormat() const;
    /*!
       brief returns the length of the view definition string
     */
    int GetDefinitionLength(bool SelectExpressionOnly) const;
    /*!
       @brief  return the sqlmode at view definition time
     */
    SQLMode GetSqlMode() const;
    /*!
       @brief copies the view definition string into a given buffer
     */
    bool CopyDefinition (bool SelectExpressionOnly, bool RemoveInternal, void* pDestination, SAPDB_UInt DestinationSize) const;
    /*!
       @brief assignment
     */
    void operator= (const Catalog_ViewDescriptor& Source)
    {
        Catalog_TableDescriptor::operator= (Source);
    }
private :
    /*!
       @brief reads the view definition text
     */
    int GetDefinition (bool SelectExpressionOnly, void* pDestination, SAPDB_UInt DestinationSize) const;
};

#endif