/*!
  @file           IFRConversion_VarcharUCS2Converter.cpp
  @author         D039759
  @ingroup        IFR_DataConv
  @brief
  @see

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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
#include "SAPDB/Interfaces/Runtime/IFR_Common.h"
#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_VarcharUCS2Converter.h"

//----------------------------------------------------------------------
IFRConversion_VarcharUCS2Converter::IFRConversion_VarcharUCS2Converter(IFR_ShortInfo &shortinfo,
                                                                       SAPDBMem_IRawAllocator& allocator,
                                                                       IFR_Connection& connection)
:IFRConversion_UCS2CharDataConverter(shortinfo, allocator, true, true, connection)
{}

//----------------------------------------------------------------------
IFRConversion_VarcharUCS2Converter::~IFRConversion_VarcharUCS2Converter()
{
}
