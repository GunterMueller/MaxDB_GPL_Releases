/*!
  @file           IFRConversion_BooleanConverter.h
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
#ifndef IFRCONVERSION_BOOLEANCONVERTER_H
#define IFRCONVERSION_BOOLEANCONVERTER_H

#include "Interfaces/Runtime/Conversion/IFRConversion_Converter.h"

/**
 * @ingroup IFR_DataConv
 * @brief Converter for the boolean type. 
 *
 * A boolean value can be converted from and to all numeric types.
 * On input, a value of <code>TRUE</code> is inserted if the numeric
 * input is not zero, and <code>FALSE</code> if the input is zero.
 * On output, a value of <code>0</code> is assigned to the 
 * host variable if the column value is <code>FALSE</code>, and
 * a value of <code>1</code> otherwise.
 */
class IFRConversion_BooleanConverter 
    : public IFRConversion_Converter 
{
public:    
    /**
     * Constructor.
     * @param shortinfo Short field info of the parameter.
     * @param allocator The allocator to be used.
     */
    IFRConversion_BooleanConverter(IFR_ShortInfo& shortinfo,
                                   SAPDBMem_IRawAllocator& allocator);
    /**
     * Destructor.
     */
    virtual ~IFRConversion_BooleanConverter();
    

    virtual IFR_Retcode translateBinaryInput(IFRPacket_DataPart& datapart,
                                             char               *data,
                                             IFR_Length            datalength,
                                             IFR_Length*           lengthindicator,
                                             IFR_ConnectionItem& clink);

    virtual IFR_Retcode appendBinaryInput(IFRPacket_DataPart& datapart,
                                          char               *data,
                                          IFR_Length            datalength,
                                          IFR_Length*           lengthindicator,
                                          IFR_ConnectionItem & clink,
                                          IFR_Length         &offset,
                                          IFRConversion_Putval *putval);
    
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_Int1&           data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval* pv);
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_Int2&           data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval* pv);
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_Int4&           data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval* pv);
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_Int8&           data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval* pv);
        

    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_UInt1&          data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval* pv);
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_UInt2&          data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval* pv);
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_UInt4&          data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval* pv);
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_UInt8&          data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval* pv);

    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       double&             data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval* pv);
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       float&              data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval* pv);

    virtual IFR_Retcode translateInput(IFRPacket_DataPart&   datapart,
                                       SQL_NUMERIC_STRUCT&   data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem&   clink,
                                       IFRConversion_Putval *pv);

    virtual IFR_Retcode translateOutput(IFRPacket_DataPart&   datapart,
                                        IFR_Int1&             data,
                                        IFR_Length*             lengthindicator,
                                        IFR_ConnectionItem&   clink);

    virtual IFR_Retcode translateOutput(IFRPacket_DataPart&   datapart,
                                        IFR_Int2&             data,
                                        IFR_Length*             lengthindicator,
                                        IFR_ConnectionItem&   clink);

    virtual IFR_Retcode translateOutput(IFRPacket_DataPart&   datapart,
                                        IFR_Int4&             data,
                                        IFR_Length*             lengthindicator,
                                        IFR_ConnectionItem&   clink);

    virtual IFR_Retcode translateOutput(IFRPacket_DataPart&   datapart,
                                        IFR_Int8&             data,
                                        IFR_Length*             lengthindicator,
                                        IFR_ConnectionItem&   clink);

    virtual IFR_Retcode translateOutput(IFRPacket_DataPart&   datapart,
                                        IFR_UInt1&             data,
                                        IFR_Length*             lengthindicator,
                                        IFR_ConnectionItem&   clink);

    virtual IFR_Retcode translateOutput(IFRPacket_DataPart&   datapart,
                                        IFR_UInt2&             data,
                                        IFR_Length*             lengthindicator,
                                        IFR_ConnectionItem&   clink);

    virtual IFR_Retcode translateOutput(IFRPacket_DataPart&   datapart,
                                        IFR_UInt4&             data,
                                        IFR_Length*             lengthindicator,
                                        IFR_ConnectionItem&   clink);

    virtual IFR_Retcode translateOutput(IFRPacket_DataPart&   datapart,
                                        IFR_UInt8&             data,
                                        IFR_Length*             lengthindicator,
                                        IFR_ConnectionItem&   clink);

    virtual IFR_Retcode translateOutput(IFRPacket_DataPart&   datapart,
                                        double&               data,
                                        IFR_Length*             lengthindicator,
                                        IFR_ConnectionItem&   clink);

    virtual IFR_Retcode translateOutput(IFRPacket_DataPart&   datapart,
                                        float&                data,
                                        IFR_Length*             lengthindicator,
                                        IFR_ConnectionItem&   clink);

    virtual IFR_Retcode translateOutput(IFRPacket_DataPart&   datapart,
                                        SQL_NUMERIC_STRUCT&   data,
                                        IFR_Length*           lengthindicator,
                                        IFR_ConnectionItem&   clink);

    virtual IFR_Retcode translateAsciiOutput(IFRPacket_DataPart& datapart,
                                             char           *data,
                                             IFR_Length        datalength,
                                             IFR_Length*           lengthindicator,
                                             IFR_Bool        terminate,
                                             IFR_Bool        ascii7bit,
                                             IFR_ConnectionItem &clink);

    virtual IFR_Retcode translateUCS2Output(IFRPacket_DataPart& datapart,
                                            char           *data,
                                            IFR_Bool        swapped,
                                            IFR_Length        datalength,
                                            IFR_Length*       lengthindicator,
                                            IFR_Bool        terminate,
                                            IFR_ConnectionItem &clink);

    virtual IFR_Retcode translateUTF8Output(IFRPacket_DataPart& datapart,
                                            char           *data,
                                            IFR_Length        datalength,
                                            IFR_Length*       lengthindicator,
                                            IFR_Bool        terminate,
                                            IFR_ConnectionItem &clink);
    
    virtual IFR_Retcode  translateBinaryOutput(IFRPacket_DataPart& datapart,
                                               char *data,
                                               IFR_Length datalength,
                                               IFR_Length *lengthindicator,
                                               IFR_ConnectionItem& clink);
    
    virtual IFR_Retcode  appendBinaryOutput(IFRPacket_DataPart& datapart,
                                            char               *data,
                                            IFR_Length          datalength,
                                            IFR_Length*         lengthindicator,
                                            IFR_ConnectionItem& clink,
                                            IFR_Length         &dataoffset,
                                            IFR_Length         &offset,
                                            IFRConversion_Getval *getval);
    
};

#endif //IFRCONVERSION_BOOLEANCONVERTER_H
