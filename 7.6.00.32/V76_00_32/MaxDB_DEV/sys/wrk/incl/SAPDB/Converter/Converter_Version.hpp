/*!
  @file     Converter_Version.hpp
  @author   TorstenS
  @ingroup  Converter
  @brief    Definition of the converter version
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

#ifndef CONVERTER_VERSION_HPP
#define CONVERTER_VERSION_HPP


#include "SAPDBCommon/SAPDB_Types.hpp"
#include "SAPDBCommon/SAPDB_EnhancedTypes.hpp"


/*!
   @class   Converter_Version
   @brief   Converter version (also: savepoint version)
 */
class  Converter_Version : public SAPDB_Invalidatable<SAPDB_UInt4, SAPDB_MAX_UINT4>
{
public:

    /*!
        @brief   default constructor
     */

    Converter_Version()
    {};

    /*!
       @brief   constructor
       @param   value [in] initial value
    */

    Converter_Version( const SAPDB_UInt4    &value )
    :
    SAPDB_Invalidatable<SAPDB_UInt4, SAPDB_MAX_UINT4>( value )
    {};

    /*!
       @brief   get invalid version number
       @return  (Converter_Version) invalid version number
    */
    static Converter_Version GetNilVersion()
    {
        return SAPDB_MAX_UINT4;
    }

    /*!
       @brief   set converter version to initial value (=1)
       @return  none
    */
    void SetToInitialVersion()
    {
        m_Value=1;
    };

    /*!
        @brief  Checks whether converter version is valid
        @return (SAPDB_Bool) true if valid
    */

    SAPDB_Bool IsValid() const
    {
        return(( m_Value != SAPDB_MAX_UINT4 ));
    }

    /*!
    @brief  Checks whether converter version is invalid
    @return (SAPDB_Bool) true if invalid
    */

    SAPDB_Bool IsInvalid() const
    {
        return(( m_Value == SAPDB_MAX_UINT4 ) || (0 == m_Value ));
    }

};

#endif // CONVERTER_VERSION_HPP

