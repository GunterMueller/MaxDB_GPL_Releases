/*!
  @file           Converter_ParentReference.hpp
  @author         TorstenS
  @author         TillL
  @ingroup        Converter
  @brief          Reference to parent page in the converter parent index
*/

/*
    ========== licence begin  GPL
    Copyright (c) 2000-2004 SAP AG

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
*/

#ifndef CONVERTER_PARENTREFERENCE_HPP
#define CONVERTER_PARENTREFERENCE_HPP


#include "Converter/Converter_Types.hpp"


/*!
   @class          Converter_ParentReference
   @brief          Handles a reference to a converter parent page within converter
                   parent index
 */
class Converter_ParentReference
{
public:

    /// shortcut to Converter_IndexEntryNo
    typedef Converter_IndexEntryNo              IndexEntryNo;

    /// shortcut to Converter_PageEntryNo
    typedef Converter_PageEntryNo               PageEntryNo;

    /*!
       @brief          Default constructor
       @return         none
     */
    Converter_ParentReference()
    {}

    /*!
       @brief          Constructor, assigns reference (using standard implementaion)
       @param          indexeno [in] reference to parent page in converter index
       @param          pageeno [in] reference to entry within parent page
       @return         none
     */
    Converter_ParentReference
        (const IndexEntryNo&            indexeno,
         const PageEntryNo&             pageeno)
        :
         m_IndexEntryNo(indexeno),
         m_PageEntryNo(pageeno)
    {}

    /*!
       @brief          Sets parent reference
       @param          indexeno [in] reference to parent page in converter index
       @param          pageeno [in] reference to entry within parent page
       @return         none
     */
    void SetReference
        (const IndexEntryNo&            indexeno,
         const PageEntryNo&             pageeno)
    {
        m_IndexEntryNo = indexeno;
        m_PageEntryNo  = pageeno;
    }

    /*!
       @brief          Sets reference to parent page in converter index
       @param          indexeno [in] reference to parent page in parent index
       @return         none
     */
    void SetIndexEntryNo
        (const IndexEntryNo&            indexeno)
    {
        m_IndexEntryNo = indexeno;
    }

    /*!
       @brief          Sets reference to entry within parent page
       @param          pageeno [in] reference to entry within parent page
       @return         none
     */
    void SetPageEntryNo
        (const PageEntryNo&             pageeno)
    {
        m_PageEntryNo  = pageeno;
    }

    /*!
       @brief          Returns reference to parent page in parent index
       @return         (const IndexEntryNo&amp;)
     */
    const IndexEntryNo& GetIndexEntryNo() const
    {
        return m_IndexEntryNo;
    }

    /*!
       @brief          Returns reference to entry within parent page
       @return         (const PageEntryNo&amp;)
     */
    const PageEntryNo& GetPageEntryNo() const
    {
        return m_PageEntryNo;
    }

private:

    /// reference to parent page in converter index
    IndexEntryNo m_IndexEntryNo;

    /// reference to entry within parent page
    PageEntryNo  m_PageEntryNo;
};


#endif // CONVERTER_PARENTREFERENCE_HPP



