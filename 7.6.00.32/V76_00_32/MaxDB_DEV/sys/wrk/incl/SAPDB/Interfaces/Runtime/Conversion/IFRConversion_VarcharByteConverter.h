/*!
  @file           IFRConversion_VarcharByteConverter.h
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
#ifndef IFRCONVERSION_VARCHARBYTECONVERTER_H
#define IFRCONVERSION_VARCHARBYTECONVERTER_H

#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_Converter.h"
#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_ByteCharDataConverter.h"

/**
 * @ingroup IFR_DataConv
 * Class that handles the conversion from and into the <code>VARCHAR BYTE</code>
 * data type. This is very similar to the <code>CHAR BYTE</code> converter,
 * except that trimming is performed - trailing '\0's are trimmed.
 */
class IFRConversion_VarcharByteConverter
    : public IFRConversion_ByteCharDataConverter
{
public:
    /**
     * Constructor.
     * @param shortinfo Short field info of the parameter.
     * @param allocator The allocator to be used.
     */
    IFRConversion_VarcharByteConverter(IFR_ShortInfo& shortinfo,
                                       SAPDBMem_IRawAllocator& allocator,
                                       IFR_Connection& connection);

    /**
     * Destructor.
     */
    virtual ~IFRConversion_VarcharByteConverter();

    /**
     * Overridden, as we make the distinction for binary <-> UCS2 here.
     */
    virtual IFR_Retcode appendUCS2Input(IFRPacket_DataPart&  datapart,
                                        char                *data,
                                        IFR_Bool             swapped,
                                        IFR_Length           datalength,
                                        IFR_Length*          lengthindicator,
                                        IFR_Bool             terminate,
                                        IFR_ConnectionItem  &clink,
                                        IFR_Length&          offset,
                                        IFRConversion_Putval *putval);
    /**
     * Overridden to handle binary conversion.
     */
    virtual IFR_Retcode appendUTF8Input(IFRPacket_DataPart& datapart,
                                        char               *data,
                                        IFR_Length            datalength,
                                        IFR_Length*           lengthindicator,
                                        IFR_Bool              terminate,
                                        IFR_ConnectionItem &clink,
                                        IFR_Length&         offset,
                                        IFRConversion_Putval *putval);

    /**
     * Overridden, as we make the distinction for binary <-> UCS2 here.
     */
    virtual IFR_Retcode appendUCS2Output(IFRPacket_DataPart&   datapart,
                                         char                 *data,
                                         IFR_Bool              swapped,
                                         IFR_Length            datalength,
                                         IFR_Length*           lengthindicator,
                                         IFR_Bool              terminate,
                                         IFR_ConnectionItem   &clink,
                                         IFR_Length&           dataoffset,
                                         IFR_Length&           offset,
                                         IFRConversion_Getval *getval);


    virtual IFR_Retcode appendUTF8Output(IFRPacket_DataPart&   datapart,
                                         char                 *data,
                                         IFR_Length            datalength,
                                         IFR_Length*           lengthindicator,
                                         IFR_Bool              terminate,
                                         IFR_ConnectionItem   &clink,
                                         IFR_Length&           dataoffset,
                                         IFR_Length&           offset,
                                         IFRConversion_Getval *getval);

    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       SQL_DATE_STRUCT&    data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);
    
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       SQL_TIME_STRUCT&    data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

    virtual IFR_Retcode translateInput(IFRPacket_DataPart&   datapart,
                                       SQL_TIMESTAMP_STRUCT& data,
                                       IFR_Length*             lengthindicator,
                                       IFR_ConnectionItem&   clink,
                                       IFRConversion_Putval *pv);

    virtual IFR_Retcode translateOutput(IFRPacket_DataPart&   datapart,
                                        SQL_DATE_STRUCT&      data,
                                        IFR_Length*             lengthindicator,
                                        IFR_ConnectionItem&   clink);

    virtual IFR_Retcode translateOutput(IFRPacket_DataPart&   datapart,
                                        SQL_TIME_STRUCT&      data,
                                        IFR_Length*             lengthindicator,
                                        IFR_ConnectionItem&   clink);

    virtual IFR_Retcode translateOutput(IFRPacket_DataPart&   datapart,
                                        SQL_TIMESTAMP_STRUCT& data,
                                        IFR_Length*             lengthindicator,
                                        IFR_ConnectionItem&   clink);
};

#endif
