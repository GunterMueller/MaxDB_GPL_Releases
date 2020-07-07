/*!
  @file           IFRConversion_CharAsciiConverter.h
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
#ifndef IFRCONVERSION_CHARASCIICONVERTER_H
#define IFRCONVERSION_CHARASCIICONVERTER_H

#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_Converter.h"
#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_ByteCharDataConverter.h"

/**
 * @ingroup IFR_DataConv
 * Class that handles the conversion from and into the <code>CHAR ASCII</code>
 * data type. This means the data is not trimmed and the defined byte is a space
 * (0x20).
 */
class IFRConversion_CharAsciiConverter
    : public IFRConversion_ByteCharDataConverter
{
public:
    /**
     * Constructor.
     * @param shortinfo The short field infos.
     * @param allocator The allocator to be used.
     */
    IFRConversion_CharAsciiConverter(IFR_ShortInfo& shortinfo,
                                     SAPDBMem_IRawAllocator& allocator,
                                     IFR_Connection& connection);
    /**
     * Destructor.
     */
    virtual ~IFRConversion_CharAsciiConverter();
};

#endif // IFRCONVERSION_CHARASCIICONVERTER_H
