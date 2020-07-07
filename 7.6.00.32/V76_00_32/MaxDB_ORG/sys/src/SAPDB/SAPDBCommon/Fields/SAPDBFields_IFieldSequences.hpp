/*!
  @file           SAPDBFields_IFieldSequences.hpp
  @author         UweH
  @ingroup        Field Container classes
  @brief          Declarations of interfaces for field containers.

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


#ifndef SAPDBFields_IFieldSequences_HPP
#define SAPDBFields_IFieldSequences_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/Fields/SAPDBFields_AttributedField.hpp"

class SAPDBErr_MessageList;

/*!
@interface      SAPDBFields_IFieldSequence
@brief          An interface to an ordered set of fields.
*/
class SAPDBFields_IFieldSequence
{
public:
    /// The range for the number of elements in field Containers
    typedef SAPDB_UInt                            IndexType;
    /// type for specifying sizes
    typedef SAPDBFields_AttributedField::SizeType SizeType;
public:
    /*!
    @brief          Returns the number of fields in the field sequence.
    @return         IndexType
    */
    virtual IndexType GetFieldCount() const = 0;
    /// result of GetField()
    enum GetFieldResult
    {
        fieldFound,
        fieldNotFound,
        fieldError
    };
    /*!
    @brief          Returns the &lt;N>-th field within the field sequence.
    @param          fieldno [in] The index of the field [0...GetFieldCount()-1]
    @param          field   [in/out] The field to be assigned to the &lt;N>-th component
    @param          errlist [out] if fieldError is returned this contains additional info
    @return         GetFieldResult
    */
    virtual GetFieldResult GetField ( IndexType                    fieldno,
                                      SAPDBFields_AttributedField &field,
                                      SAPDBErr_MessageList        &errlist) const = 0;
    /// Returns the length of the field with index &lt;N>. If the field does not exist 0 is returned.
    virtual SizeType GetFieldLength(IndexType fieldno) const = 0;
	/// Returns the sum of the length of all fields in the field sequence in bytes.
    virtual SizeType GetTotalLength() const = 0;
};
#endif // SAPDBFields_IFieldSequences_HPP

