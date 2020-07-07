
/*!***************************************************************************

  module:       CnsRead_SeparatedRowHeader.hpp

  responsible : ThomasA

  special area: Consistent Read

  description:  Defines the record header separated from a row

  last changed: 2000-08-10  12:27
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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


*****************************************************************************/


#ifndef CNSREAD_SEPARATEDROWHEADER_HPP
#define CNSREAD_SEPARATEDROWHEADER_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"

/*!
   @brief defines the portion of a row header, which is paged out, when the header is
          used to contain a swizzled pointer
 */
class CnsRead_SeparatedRowHeader
{
public :
    int GetRecLen() const
    {
        return m_recLen;
    }
    int GetKeyLen() const
    {
        return m_keyLen;
    }
#if defined(BIT64)
    int GetVarColOffset() const
    {
        return m_varColOffset;
    }
    int GetVarColCount() const
    {
        return m_varColCount;
    }
    /*!
      @brief return true, if the delete flag is set.
     */
    inline bool IsDeleted() const;
    /*!
       @brief add or removes deleted flag t/from header
     */
    inline void MarkDeleted(bool);
#endif
    /*!
       @brief sets the record length
     */
    inline void SetRecLen(int RecLen);
private :
    SAPDB_Int2 m_recLen;
    SAPDB_Int2 m_keyLen;
#if defined(BIT64)
    SAPDB_Int2 m_varColOffset;
    SAPDB_Int2 m_varColCount;
#endif
};

#if defined(BIT64)

inline bool CnsRead_SeparatedRowHeader::IsDeleted() const
{
    return ((m_varColCount & 0x4000) > 0);
}

inline void CnsRead_SeparatedRowHeader::MarkDeleted(bool markDeleted)
{
    if (markDeleted)
    {
        m_varColCount |= 0x4000;
    }
    else
    {
        m_varColCount -= 0x4000;
    }
}

#endif

inline void CnsRead_SeparatedRowHeader::SetRecLen(int RecLen)
{
    m_recLen = RecLen;
}
#endif