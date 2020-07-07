/*!
  @file           IFRConversion_VarcharAsciiConverter.h
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
#ifndef IFRCONVERSION_VARCHARASCIICONVERTER_H
#define IFRCONVERSION_VARCHARASCIICONVERTER_H

#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_Converter.h"
#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_ByteCharDataConverter.h"

/**
 * @ingroup IFR_DataConv
 * Class that handles the conversion from and into the <code>VARCHAR ASCII</code>
 * data type. This is very similar to the <code>CHAR ASCII</code> converter,
 * except that trimming is performed. The defined byte is the space character (0x20).
 */
class IFRConversion_VarcharAsciiConverter
    : public IFRConversion_ByteCharDataConverter
{
public:
    /**
     * Constructor.
     * @param shortinfo The short field infos.
     * @param allocator The allocator to be used.
     */
    IFRConversion_VarcharAsciiConverter(IFR_ShortInfo& shortinfo,
                                        SAPDBMem_IRawAllocator& allocator,
                                        IFR_Connection& connection);
    
    /**
     * Destructor.
     */
    virtual ~IFRConversion_VarcharAsciiConverter();
};

#endif // IFRCONVERSION_VARCHARASCIICONVERTER_H
