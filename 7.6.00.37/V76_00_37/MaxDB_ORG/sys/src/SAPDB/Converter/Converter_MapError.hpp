/*!
  @file           Converter_MapError.hpp
  @author         TorstenS
*/

/*
  ========== licence begin  GPL
  Copyright (c) 2001-2004 SAP AG

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
  ========== licence end
*/


#ifndef CONVERTER_MAPERROR_HPP
#define CONVERTER_MAPERROR_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 **/

#include "Converter/Converter_Exception.hpp"
#include "Converter/Converter_Messages.hpp"    /* nocheck */
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "Converter/Converter_Types.hpp"
#include "DataAccess/Data_Types.hpp"

/*!
  @class          Converter_MapError
  @brief          Error messages of converter map
 */
class Converter_MapError : public Converter_Exception
{
public:

    /*!
       @brief   Constructor
       @param   filename   [in] filename (to be generated)
       @param   linenumber [in] linenumber (to be generated)
       @param   recMode    [in] recovery mode of given page
       @param   pageNo     [in] current page number
       @param   retCode    [in] error state
       @return  none
     */

    Converter_MapError
    (const SAPDB_Char*              filename,
     const SAPDB_UInt4              linenumber,
     const Data_PageRecoveryMode    &recMode,
     const Data_PageNo              pageNo,
     const Converter_ReturnCode     retCode)
    :
    Converter_Exception(
        filename,
        linenumber,
        CONVERTER_MAP_ERROR,
        SAPDB_ToString( pageNo, _T_d ),
    ( recMode.PageIsRecoverable() ? ( recMode.UseDynamicPageAddressing() ? "perm" : "static") : "temp" ),
        SAPDB_ToString( retCode ))
    {}
};

#endif  /* CONVERTER_MAPERROR_HPP */
