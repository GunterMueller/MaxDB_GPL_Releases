/*!
  @file           Catalog_ViewtextObject.hpp
  @author         ThomasA
  @brief          memory representation of a view definition string 
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


#ifndef CATALOG_VIEWTEXTOBJECT_HPP
#define CATALOG_VIEWTEXTOBJECT_HPP

#include "SQLManager/Catalog/Catalog_Types.hpp"
#include "SQLManager/Catalog/Catalog_Object.hpp"

/*!
  @brief encapsulation of pascal type tak_viewtextrecord. Describes the definition string of a view
*/

class Catalog_ViewtextObject : public Catalog_ViewtextObjectPascal
{
public :
    /*!
       @brief key of a view text object
    */
    class Catalog_Key : public Catalog_Object::Catalog_Key
    {
    public :
        /*!
           @brief constructor
        */
        Catalog_Key(const Catalog_ObjectId& ViewId);
    };
    /*!
       @brief return the date/time format of the transaction which created the view
     */
    tgg00_DateTimeFormat_Enum GetDateTimeFormat() const;
    /*!
       @brief returns the text length stored in the instance
     */
    inline int GetLength() const;
    /*!
       @brief returns the offset of the keyword SELECT in the create view statement
     */
    inline int GetSelectOffset() const;
    /*!
       @brief return the sql mode of the transaction which created the view
     */
    inline tsp00_SqlMode_Enum GetSqlMode() const;
    /*!
       @brief returns a pointer to the definition text stored in the instance
     */
    inline const SAPDB_Byte* GetTextPtr() const;
    /*!
       @brief returns true, if the current instance has a successor containing the rest of
              the definition
     */
    inline bool NextExists() const;
 };

inline int Catalog_ViewtextObject::GetLength() const
{
    return vttextlength;
}

inline int Catalog_ViewtextObject::GetSelectOffset() const
{
    return vtselect_pos - (vttabcount * mxak_vttabdef) - 1;
}

inline const SAPDB_Byte* Catalog_ViewtextObject::GetTextPtr() const
{
    return &vttbuf()[(1 == vtsegno[1]) ? (vttabcount * mxak_vttabdef) : 0];
}

inline tsp00_SqlMode_Enum Catalog_ViewtextObject::GetSqlMode() const
{
    switch (vtcontext / 10)
    {
    case 1  : return sqlm_ansi;
    case 2  : return sqlm_db2;
    case 3  : return sqlm_oracle;
    default : return sqlm_internal;
    }
}

inline bool Catalog_ViewtextObject::NextExists() const
{
    return (vtnextexist != 0);
}

#endif