/*!
  @file           Table_IResult.hpp
  @ingroup		  Table
  @author         UweH
  @brief          This is  interface for accessing results of Select of tables.

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
#ifndef Table_IResult_H
#define Table_IResult_H

// class Msg_List; // not yet possible
#include "SAPDB/Messages/Msg_List.hpp"

class SAPDBFields_IFieldSequence;

/*!
  @interface Table_IResult
  @brief     This interface offers methods to iterate over result sets.
             Which were created by Select on a table object.
 */
class Table_IResult
{
public:
    /*!
        @class Iterator
        @brief For accessing elements of result sets.
    */
    class Iterator
    {
        /*!
        @brief  The iterator is set to the next following element.
        @param  errlist [out] used if false is returned
        @return false on error
        
        If the next element does not exist.
        The iterator becomes invalid.
        Note that true is returned in this case.
        */
        virtual bool Next(Msg_List &errlist) = 0;
        /*!
        @brief The iterator is set to the previous element following element.
        @param  errlist [out] used if false is returned
        @return false on error

        If the previous element does not exist.
        The iterator becomes invalid.
        Note that true is returned in this case.
        */
        virtual bool Prev(Msg_List &errlist) = 0;
        /// returns true if the iterator can be dereferenced.
        virtual bool IsValid() const = 0;
        /*!
        @brief The iterator returns a constant reference to the fieldsequence it points to.
        @param result  [out] is set to false on any error
        @param errlist [out] on error this is filled.
        If the iterator is not valid or any other error occurs
        the result is false and errlist is filled.
        */
        virtual const SAPDBFields_IFieldSequence& Deref( bool     &result,
                                                         Msg_List &errlist ) const = 0;
    };
    
public:

    /*!
    @brief  Returns the iterator to the first element of the result set.
    @param  iter    [out] the iterator points to first element if one exists and no error occured.
    @param  errlist [out] filled on error
    @return false on any error
    */
    virtual bool GetFirst ( Iterator &iter, 
                            Msg_List &errlist ) const = 0;

    /*!
    @brief  Returns the iterator to the last element of the result set.
    @param  iter    [out] the iterator points to last element if one exists and no error occured.
    @param  errlist [out] filled on error
    @return false on any error
    */
    virtual bool GetLast ( Iterator &iter, 
                           Msg_List &errlist ) const = 0;
};
#endif  /* Table_IResult_H */
