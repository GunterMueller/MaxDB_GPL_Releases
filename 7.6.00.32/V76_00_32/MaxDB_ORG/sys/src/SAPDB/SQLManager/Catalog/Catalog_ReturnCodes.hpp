/*!
  @file           Catalog_ReturnCodes.hpp
  @author         ThomasA
  @ingroup        Catalog
  @brief          all return codes of the catalog component

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

#ifndef CATALOG_RETURNCODES_HPP
#define CATALOG_RETURNCODES_HPP

/*!
   @brief defines all return codes of the catalog component
 */
class Catalog_ReturnCodes
{
public :
    enum AllReturnCodes 
    {
        CatRcOk,
        CatRcCancelled,
        CatRcNoMoreMemory,
        CatRcNotCreated,
        CatRcUnknownProcedure,
        CatRcSystemError
    };
    /*!
       @brief constructor
     */
    Catalog_ReturnCodes()
        : m_rc(CatRcOk)
    {
    }
    /*!
       @brief cast to AllReturnCodes
     */
    operator AllReturnCodes()
    {
        return m_rc;
    }
    int CreateSQLCode()
    {
        return 0;
    }
    AllReturnCodes GetEnum() const
    {
        return m_rc;
    }
    /*!
       @brief returns ture, if current code is CatRcOk
     */
    bool IsOk()
    {
        return (CatRcOk == m_rc);
    }
    AllReturnCodes m_rc;

};

template <class ENUM>
class Catalog_MethodReturnCode : public Catalog_ReturnCodes
{
public :
    Catalog_MethodReturnCode(ENUM e)
    {
        m_rc = (Catalog_ReturnCodes::AllReturnCodes) e;
    }
};

#endif