/*!
  @file           Table_UnsortedResult.hpp
  @ingroup		  Table
  @author         UweH

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
#ifndef Table_UnsortedResult_H
#define Table_UnsortedResult_H

#include "Table/Table_IResult.hpp"

// class Msg_List; // not yet possible
#include "SAPDB/Messages/Msg_List.hpp"

/*!
  @class Table_UnsortedResult
  @brief This class implements an unsorted result set.
 */
class Table_UnsortedResult : public Table_IResult
{
public:

    class Iterator : public Table_IResult::Iterator
    {
        virtual bool Next(Msg_List &errlist);
        virtual bool Prev(Msg_List &errlist);
        virtual bool IsValid() const;
        virtual const SAPDBFields_IFieldSequence& Deref( bool     &result,
                                                         Msg_List &errlist ) const;
    };
    
public:

    virtual bool GetFirst ( Table_IResult::Iterator &iter, 
                            Msg_List    &errlist ) const;
    virtual bool GetLast ( Table_IResult::Iterator &iter,
                           Msg_List    &errlist ) const;
};
#endif  /* Table_UnsortedResult_H */
