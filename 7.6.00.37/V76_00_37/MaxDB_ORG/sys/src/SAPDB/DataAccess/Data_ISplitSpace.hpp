/*!
  @file           Data_ISplitSpace.hpp
  @author         UweH
  @brief          defines class Data_ISplitSpace

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
#ifndef Data_ISplitSpace_HPP
#define Data_ISplitSpace_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_EnhancedTypes.hpp"

class SAPDBErr_MessageList;

/*!
   @interface Data_ISplitSpace
   @brief     This handle offers splitted space access.
 */
class Data_ISplitSpace
{
public:
    /// Definition of partno to identify one part of the split space.
    typedef SAPDB_Invalidatable<SAPDB_Int, -1> PartNo;
    /// return true, if at least one part is accessable
    virtual bool IsAssigned () const = 0;
    /// Return the number of the current part.
    virtual PartNo CurrentPart () const = 0;
    /// From the specified part is the length and a pointer returned.
    virtual void GetPart (PartNo       Part,
                          SAPDB_UInt  &Length,
                          SAPDB_Byte* &Value) const = 0;
    /// The sum of length of all used parts of the space is returned in bytes.
    virtual SAPDB_UInt Length() const = 0;
    /// appends some info about current contents.
    virtual void AppendContextInfo (SAPDB_Char  const * const  filename,
                                    SAPDB_UInt4 const          linenumber,
                                    SAPDBErr_MessageList      &errlist,
                                    SAPDB_Char  const * const  title = 0) const = 0;
    /*!
    	@return  false, if space begin is not properly aligned
    	@brief   This checks if the split space begins at an aligned position
    */
    virtual bool CheckAlignment () const = 0;
};
#endif // Data_ISplitSpace_HPP
