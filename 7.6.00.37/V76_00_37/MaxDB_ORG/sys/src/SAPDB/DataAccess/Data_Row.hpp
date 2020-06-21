  /*!
  @file           Data_Row.hpp
  @author         ThomasA
  @brief          defines a row of a sql table

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
#ifndef DATA_ROW_HPP
#define DATA_ROW_HPP


#include "ggg00.h"

/*!
   @brief defines a row of a sql table
 */
class Data_Row : public tgg00_Rec
{
public :
    /*!
       @brief copies the current row
     */
    bool       CopyRow (Data_Row& Desitination) const;
    /*!
      @brief returns the length (in bytes) of the key
     */
    inline int GetKeyLen() const;
    /*!
      @brief returns the length (in bytes) of the row
     */
    inline int GetRowLen() const;
    /*!
      @brief returns the offset (in bytes) of variable length columns
     */
    inline int GetVarColOffset() const;
    /*!
      @brief returns the number of columns of variable length
     */
    inline int GetVarColCount() const;
    /*!
      @brief return true, if the row has been packed; otherwise false
     */
    inline bool IsPacked() const;
    /*!
       @brief sets the row length to the given value
     */
    inline void SetRowLen(int RowLen);
    /*!
       @brief sets the key length to the given value
     */
    inline void SetKeyLen(int KeyLen);
    /*!
       @brief sets the offset of columns of variable length
     */
    inline void SetVarColOffset(int VarColOffset);
    /*!
       @brief sets the number of columns of variable length
     */
    inline void SetVarColCount(int VarColCount);
    /*!
       @brief packs the current row, i.e. transforms the row from the standard layout to a more dense layout
     */
    bool Pack (Data_Row& CompressedRow) const;
    /*!
       @brief transforms the row to the standard layout
     */
    bool Unpack (Data_Row& CompressedRow) const;
    /*!
       @brief cast from the pascal definiton of a row to a Data_Row
     */
    inline static Data_Row& ToRow (tgg00_Rec& Rec);
};

//-----------------------------------------------------------------------------

inline int Data_Row::GetKeyLen() const
{
    return recKeyLen_gg00();
}

//-----------------------------------------------------------------------------

inline int Data_Row::GetRowLen() const
{
    return recLen_gg00();
}

//-----------------------------------------------------------------------------

inline int Data_Row::GetVarColOffset() const
{
    return recVarcolOffset_gg00();
}

//-----------------------------------------------------------------------------

inline int Data_Row::GetVarColCount() const
{
    return recVarcolCnt_gg00();
}

//-----------------------------------------------------------------------------

inline bool Data_Row::IsPacked() const
{
    return (GetVarColOffset() < 0);
}

//-----------------------------------------------------------------------------

inline Data_Row& Data_Row::ToRow (tgg00_Rec& Rec)
{
    return *reinterpret_cast<Data_Row*>(&Rec);
}

//-----------------------------------------------------------------------------

inline void Data_Row::SetRowLen(int RowLen)
{
    recLen_gg00() = RowLen;
}

//-----------------------------------------------------------------------------

inline void Data_Row::SetKeyLen(int KeyLen)
{
    recKeyLen_gg00() = KeyLen;
}

//-----------------------------------------------------------------------------

inline void Data_Row::SetVarColOffset(int VarColOffset)
{
    recVarcolOffset_gg00() = VarColOffset;
}

//-----------------------------------------------------------------------------

inline void Data_Row::SetVarColCount(int VarColCount)
{
    recVarcolCnt_gg00() = VarColCount;
}

#endif