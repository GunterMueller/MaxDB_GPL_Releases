/*!
  @file           IFRConversion_TimeConverter.h
  @author         D039759
  @ingroup        IFR_DataConv
  @brief          Conversion of TIME values
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
#ifndef IFRCONVERSION_TIMECONVERTER_H
#define IFRCONVERSION_TIMECONVERTER_H

#include "Interfaces/Runtime/Conversion/IFRConversion_ByteCharDataConverter.h"

/**
 * @ingroup IFR_DataConv
 * Converter for time values.
 */
class IFRConversion_TimeConverter
    : public IFRConversion_ByteCharDataConverter
{
public:
    /**
     * Constructor.
     * @param shortinfo Short field info of the parameter.
     * @param allocator The allocator to be used.
     */
    IFRConversion_TimeConverter(IFR_ShortInfo& shortinfo,
                                SAPDBMem_IRawAllocator& allocator,
                                IFR_Connection& connection);
    /**
     * Destructor.
     */
    virtual ~IFRConversion_TimeConverter();

    IFR_Retcode translateInput(IFRPacket_DataPart& part,
                               SQL_DATE_STRUCT& data,
                               IFR_Length* lengthindicator,
                               IFR_ConnectionItem& clink,
                               IFRConversion_Putval *pv);

    IFR_Retcode translateInput(IFRPacket_DataPart& part,
                               SQL_TIMESTAMP_STRUCT& data,
                               IFR_Length* lengthindicator,
                               IFR_ConnectionItem& clink,
                               IFRConversion_Putval* pv);

    IFR_Retcode translateOutput(IFRPacket_DataPart& part,
                                SQL_DATE_STRUCT& data,
                                IFR_Length* lengthindicator,
                                IFR_ConnectionItem& clink);

    IFR_Retcode translateOutput(IFRPacket_DataPart& part,
                                SQL_TIMESTAMP_STRUCT& data,
                                IFR_Length* lengthindicator,
                                IFR_ConnectionItem& clink);

    IFR_Retcode  appendBinaryOutput(IFRPacket_DataPart&   datapart,
                       char                 *data,
                       IFR_Length            datalength,
                       IFR_Length*           lengthindicator,
                       IFR_ConnectionItem   &clink,
                       IFR_Length&           dataoffset,
                       IFR_Length&           offset,
                       IFRConversion_Getval *getval);
    
    IFR_Retcode appendBinaryInput(IFRPacket_DataPart& datapart,
                                  char               *data,
                                  IFR_Length            datalength,
                                  IFR_Length*           lengthindicator,
                                  IFR_ConnectionItem & clink,
                                  IFR_Length         &offset,
                                  IFRConversion_Putval *putval);

    IFR_Retcode appendAsciiInput(IFRPacket_DataPart& datapart,
                                 char               *data,
                                 IFR_Length            datalength,
                                 IFR_Length*           lengthindicator,
                                 IFR_Bool              terminate,
                                 IFR_Bool              ascii7bit,
                                 IFR_ConnectionItem &clink,
                                 IFR_Length&         offset,
                                 IFRConversion_Putval *putval);

    IFR_Retcode appendUCS2Input(IFRPacket_DataPart&  datapart,
                                char                *data,
                                IFR_Bool            swapped,
                                IFR_Length           datalength,
                                IFR_Length*          lengthindicator,
                                IFR_Bool              terminate,
                                IFR_ConnectionItem  &clink,
                                IFR_Length&          offset,
                                IFRConversion_Putval *putval);

    IFR_Retcode appendUTF8Input(IFRPacket_DataPart& datapart,
                                char               *data,
                                IFR_Length            datalength,
                                IFR_Length*           lengthindicator,
                                IFR_Bool              terminate,
                                IFR_ConnectionItem &clink,
                                IFR_Length&         offset,
                                IFRConversion_Putval *putval);



#define TRANSLATE_METHOD(xxtype)                            \
    IFR_Retcode translateInput(IFRPacket_DataPart& part,    \
                               xxtype& data,                \
                               IFR_Length* lengthindicator, \
                               IFR_ConnectionItem& clink,   \
    IFRConversion_Putval *pv);                              \
    IFR_Retcode translateOutput(IFRPacket_DataPart& part,   \
                               xxtype& data,                \
                               IFR_Length* lengthindicator, \
                               IFR_ConnectionItem& clink)
    
    TRANSLATE_METHOD(IFR_Int1);
    TRANSLATE_METHOD(IFR_Int2);
    TRANSLATE_METHOD(IFR_Int4);
    TRANSLATE_METHOD(IFR_Int8);
    TRANSLATE_METHOD(IFR_UInt1);
    TRANSLATE_METHOD(IFR_UInt2);
    TRANSLATE_METHOD(IFR_UInt4);
    TRANSLATE_METHOD(IFR_UInt8);
    TRANSLATE_METHOD(float);
    TRANSLATE_METHOD(double);
    TRANSLATE_METHOD(SQL_NUMERIC_STRUCT);
    TRANSLATE_METHOD(GUID);

#undef TRANSLATE_METHOD
};

#endif //IFRCONVERSION_TIMECONVERTER_H
