/*!
  @file           IFRConversion_VarcharAsciiConverter.cpp
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
#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/Conversion/IFRConversion_VarcharAsciiConverter.h"

#define DBUG_CLINK_METHOD_ENTER(x,y) DBUG_CONTEXT_METHOD_ENTER(x, y, &clink)

//----------------------------------------------------------------------
IFRConversion_VarcharAsciiConverter::IFRConversion_VarcharAsciiConverter(IFR_ShortInfo &shortinfo,
                                      SAPDBMem_IRawAllocator& allocator,
                                      IFR_Connection& connection)
:IFRConversion_ByteCharDataConverter(shortinfo, allocator, true, true, connection.getSQLMode() == IFR_ORACLE)
{
}

//----------------------------------------------------------------------
IFRConversion_VarcharAsciiConverter::~IFRConversion_VarcharAsciiConverter()
{}

