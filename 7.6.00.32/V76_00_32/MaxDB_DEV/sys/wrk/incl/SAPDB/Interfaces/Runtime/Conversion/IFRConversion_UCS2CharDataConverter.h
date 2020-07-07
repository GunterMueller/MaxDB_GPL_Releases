/*!
  @file           IFRConversion_UCS2CharDataConverter.h
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
#ifndef IFRCONVERSION_UCS2CHARDATACONVERTER_H
#define IFRCONVERSION_UCS2CHARDATACONVERTER_H

#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_Converter.h"
#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_ByteCharDataConverter.h"

/**
 * @ingroup IFR_DataConv
 * Class that handles the conversion from and into data types that represent
 * character oriented data in UCS2 format in the database.
 */
class IFRConversion_UCS2CharDataConverter
    : public IFRConversion_ByteCharDataConverter
{
public:
    /**
     * Constructor.
     * @param shortinfo The short field infos of the parameter.
     * @param allocator The allocator that will be used.
     * @param istrimming Flag, whether this instance will trim blanks on output.
     * @param integerconversion Flag, whether integer values are converted and
     *        inserted, or not supported
     */
    IFRConversion_UCS2CharDataConverter(IFR_ShortInfo& shortinfo,
                                        SAPDBMem_IRawAllocator& allocator,
                                        IFR_Bool istrimming,
                                        IFR_Bool integerconversion,
                                        IFR_Connection& connection);
protected:
    IFRConversion_UCS2CharDataConverter(IFR_ShortInfo& shortinfo,
                                        SAPDBMem_IRawAllocator& allocator,
                                        IFR_Bool istrimming,
                                        IFR_Bool integerconversion);
public:    
    
    /**
     * Destructor.
     */
    virtual ~IFRConversion_UCS2CharDataConverter();

    virtual IFR_Retcode translateBinaryInput(IFRPacket_DataPart& datapart,
                                             char               *data,
                                             IFR_Length          datalength,
                                             IFR_Length*         lengthindicator,
                                             IFR_ConnectionItem &clink);

    virtual IFR_Retcode appendBinaryInput(IFRPacket_DataPart&   datapart,
                                          char                 *data,
                                          IFR_Length            datalength,
                                          IFR_Length*           lengthindicator,
                                          IFR_ConnectionItem   &clink,
                                          IFR_Length&           offset,
                                          IFRConversion_Putval *putval);


    virtual IFR_Retcode translateAsciiInput(IFRPacket_DataPart& datapart,
                                            char               *data,
                                            IFR_Length          datalength,
                                            IFR_Length*         lengthindicator,
                                            IFR_Bool              terminate,
                                            IFR_Bool            ascii7bit,
                                            IFR_ConnectionItem &clink);


    virtual IFR_Retcode appendAsciiInput(IFRPacket_DataPart& datapart,
                                         char               *data,
                                         IFR_Length            datalength,
                                         IFR_Length*           lengthindicator,
                                         IFR_Bool              terminate,
                                         IFR_Bool              ascii7bit,
                                         IFR_ConnectionItem &clink,
                                         IFR_Length&         offset,
                                         IFRConversion_Putval *putval);

    virtual IFR_Retcode translateUCS2Input(IFRPacket_DataPart& datapart,
                                           char               *data,
                                           IFR_Bool            swapped,
                                           IFR_Length            datalength,
                                           IFR_Length*           lengthindicator,
                                           IFR_Bool              terminate,
                                           IFR_ConnectionItem &clink);

    virtual IFR_Retcode appendUCS2Input(IFRPacket_DataPart&  datapart,
                                        char                *data,
                                        IFR_Bool            swapped,
                                        IFR_Length           datalength,
                                        IFR_Length*          lengthindicator,
                                        IFR_Bool              terminate,
                                        IFR_ConnectionItem  &clink,
                                        IFR_Length&          offset,
                                        IFRConversion_Putval *putval);

    virtual IFR_Retcode translateUTF8Input(IFRPacket_DataPart& datapart,
                                           char               *data,
                                           IFR_Length            datalength,
                                           IFR_Length*           lengthindicator,
                                           IFR_Bool              terminate,
                                           IFR_ConnectionItem &clink);

    virtual IFR_Retcode appendUTF8Input(IFRPacket_DataPart& datapart,
                                        char               *data,
                                        IFR_Length            datalength,
                                        IFR_Length*           lengthindicator,
                                        IFR_Bool              terminate,
                                        IFR_ConnectionItem &clink,
                                        IFR_Length&         offset,
                                        IFRConversion_Putval *putval);

    virtual IFR_Retcode translateBinaryOutput(IFRPacket_DataPart& datapart,
                                              char           *data,
                                              IFR_Length        datalength,
                                              IFR_Length*           lengthindicator,
                                              IFR_ConnectionItem &clink);

    virtual IFR_Retcode appendBinaryOutput(IFRPacket_DataPart&   datapart,
                                           char                 *data,
                                           IFR_Length            datalength,
                                           IFR_Length*           lengthindicator,
                                           IFR_ConnectionItem   &clink,
                                           IFR_Length&           dataoffset,
                                           IFR_Length&           offset,
                                           IFRConversion_Getval *getval);
    
    virtual IFR_Retcode translateAsciiOutput(IFRPacket_DataPart& datapart,
                                             char           *data,
                                             IFR_Length        datalength,
                                             IFR_Length*           lengthindicator,
                                             IFR_Bool        terminate,
                                             IFR_Bool        ascii7bit,
                                             IFR_ConnectionItem &clink);

    virtual IFR_Retcode appendAsciiOutput(IFRPacket_DataPart&   datapart,
                                          char                 *data,
                                          IFR_Length            datalength,
                                          IFR_Length*           lengthindicator,
                                          IFR_Bool              terminate,
                                          IFR_Bool              ascii7bit,
                                          IFR_ConnectionItem   &clink,
                                          IFR_Length&           dataoffset,
                                          IFR_Length&           offset,
                                          IFRConversion_Getval *getval);

    virtual IFR_Retcode translateUCS2Output(IFRPacket_DataPart& datapart,
                                            char           *data,
                                            IFR_Bool            swapped,
                                            IFR_Length        datalength,
                                            IFR_Length*       lengthindicator,
                                            IFR_Bool        terminate,
                                            IFR_ConnectionItem &clink);

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

    virtual IFR_Retcode translateUTF8Output(IFRPacket_DataPart& datapart,
                                            char           *data,
                                            IFR_Length        datalength,
                                            IFR_Length*       lengthindicator,
                                            IFR_Bool        terminate,
                                            IFR_ConnectionItem &clink);

    virtual IFR_Retcode appendUTF8Output(IFRPacket_DataPart&   datapart,
                                         char                 *data,
                                         IFR_Length            datalength,
                                         IFR_Length*           lengthindicator,
                                         IFR_Bool              terminate,
                                         IFR_ConnectionItem   &clink,
                                         IFR_Length&           dataoffset,
                                         IFR_Length&           offset,
                                         IFRConversion_Getval *getval);
};

#endif
