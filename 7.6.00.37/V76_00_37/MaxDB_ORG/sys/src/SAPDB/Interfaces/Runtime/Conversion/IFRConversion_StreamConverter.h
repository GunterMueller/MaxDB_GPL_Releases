/*!
  @file           IFRConversion_StreamConverter.h
  @author         D030044
  @ingroup        IFR_DataConv
  @brief          Implements a class for processing long descriptors
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
#ifndef IFRCONVERSION_STREAMCONVERTER_H
#define IFRCONVERSION_STREAMCONVERTER_H

#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_Converter.h"
#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_ByteCharDataConverter.h"

/**
 * @ingroup IFR_DataConv
 * @brief Converter for LONG ASCII type.
 * The actual conversion is deferred to the <code>IFRConversion_Putval</code>
 * instance managed by this object. The translateXXX methods only put a 
 * LONG descriptor into the data part. 
 */
class IFRConversion_StreamConverter 
  : public IFRConversion_Converter
{
public:
    /**
     * Constructor.
     * @param shortinfo Short field info of the parameter.
     * @param allocator The allocator to be used.
     */
    IFRConversion_StreamConverter(IFR_ShortInfo& shortinfo,
                                  IFR_Bool binary,
                                  SAPDBMem_IRawAllocator& allocator,
                                  IFR_Connection& connection);   

    /**
     * @brief Destructor.
     */
    virtual ~IFRConversion_StreamConverter();
    
    virtual IFR_Retcode translateBinaryInput(IFRPacket_DataPart& datapart,
                                             char               *data,
                                             IFR_Length          datalength,
                                             IFR_Length*         lengthindicator,
                                             IFR_ConnectionItem& clink);
  
    virtual IFR_Retcode translateAsciiInput(IFRPacket_DataPart& datapart,
                                            char               *data,
                                            IFR_Length          datalength,
                                            IFR_Length*         lengthindicator,
                                            IFR_Bool            terminate,
                                            IFR_Bool            ascii7bit,
                                            IFR_ConnectionItem &clink);

    virtual IFR_Retcode translateUCS2Input(IFRPacket_DataPart& datapart,
                                           char               *data,
                                           IFR_Bool            swapped,
                                           IFR_Length            datalength,
                                           IFR_Length*           lengthindicator,
                                           IFR_Bool              terminate,
                                           IFR_ConnectionItem &clink);

    virtual IFR_Retcode translateUTF8Input(IFRPacket_DataPart& datapart,
                                           char               *data,
                                           IFR_Length            datalength,
                                           IFR_Length*           lengthindicator,
                                           IFR_Bool              terminate,
                                           IFR_ConnectionItem &clink);

    virtual IFR_Retcode appendAsciiInput(IFRPacket_DataPart&   datapart,
                                         char                 *data,
                                         IFR_Length            datalength,
                                         IFR_Length*           lengthindicator,
                                         IFR_Bool              terminate,
                                         IFR_Bool              ascii7bit,
                                         IFR_ConnectionItem   &clink,
                                         IFR_Length&           offset,
                                         IFRConversion_Putval *putval);

    virtual IFR_Retcode appendBinaryInput(IFRPacket_DataPart& datapart,
                                          char               *data,
                                          IFR_Length          datalength,
                                          IFR_Length*         lengthindicator,
                                          IFR_ConnectionItem &clink,
                                          IFR_Length&         offset,
                                          IFRConversion_Putval *putval);

    virtual IFR_Retcode appendUCS2Input(IFRPacket_DataPart&  datapart,
                                        char                *data,
                                        IFR_Bool            swapped,
                                        IFR_Length           datalength,
                                        IFR_Length*          lengthindicator,
                                        IFR_Bool             terminate,
                                        IFR_ConnectionItem  &clink,
                                        IFR_Length&          offset,
                                        IFRConversion_Putval *putval);

    virtual IFR_Retcode appendUTF8Input(IFRPacket_DataPart&  datapart,
                                        char                *data,
                                        IFR_Length           datalength,
                                        IFR_Length*          lengthindicator,
                                        IFR_Bool             terminate,
                                        IFR_ConnectionItem  &clink,
                                        IFR_Length&          offset,
                                        IFRConversion_Putval *putval);
    
    
    virtual IFR_Retcode translateAsciiOutput(IFRPacket_DataPart& datapart,
                                             char               *data,
                                             IFR_Length          datalength,
                                             IFR_Length*         lengthindicator,
                                             IFR_Bool            terminate,
                                             IFR_Bool            ascii7bit,
                                             IFR_ConnectionItem &clink);

    virtual IFR_Retcode appendAsciiOutput(IFRPacket_DataPart& datapart,
                                          char               *data,
                                          IFR_Length            datalength,
                                          IFR_Length           *lengthindicator,
                                          IFR_Bool            terminate,
                                          IFR_Bool            ascii7bit,
                                          IFR_ConnectionItem &clink,
                                          IFR_Length&         dataoffset,
                                          IFR_Length&         offset,
                                          IFRConversion_Getval *getval);

    virtual IFR_Retcode translateBinaryOutput(IFRPacket_DataPart& datapart,
					      char               *data,
					      IFR_Length          datalength,
					      IFR_Length*         lengthindicator,
					      IFR_ConnectionItem &clink);

    virtual IFR_Retcode appendBinaryOutput(IFRPacket_DataPart& datapart,
                                           char               *data,
                                           IFR_Length            datalength,
                                           IFR_Length           *lengthindicator,
                                           IFR_ConnectionItem &clink,
                                           IFR_Length&         dataoffset,
                                           IFR_Length&         offset,
                                           IFRConversion_Getval *getval);

    virtual IFR_Retcode translateUCS2Output(IFRPacket_DataPart& datapart,
                                            char           *data,
                                            IFR_Bool        swapped,
                                            IFR_Length        datalength,
                                            IFR_Length*       lengthindicator,
                                            IFR_Bool        terminate,
                                            IFR_ConnectionItem &clink);

    virtual IFR_Retcode appendUCS2Output(IFRPacket_DataPart& datapart,
                                         char               *data,
                                         IFR_Bool            swapped,
                                         IFR_Length            datalength,
                                         IFR_Length           *lengthindicator,
                                         IFR_Bool              terminate,
                                         IFR_ConnectionItem &clink,
                                         IFR_Length&         dataoffset,
                                         IFR_Length&         offset,
                                         IFRConversion_Getval *getval);


    virtual IFR_Retcode translateUTF8Output(IFRPacket_DataPart& datapart,
                                            char           *data,
                                            IFR_Length      datalength,
                                            IFR_Length*     lengthindicator,
                                            IFR_Bool        terminate,
                                            IFR_ConnectionItem &clink);
                         
    virtual IFR_Retcode appendUTF8Output(IFRPacket_DataPart& datapart,
                                         char           *data,
                                         IFR_Length        datalength,
                                         IFR_Length*       lengthindicator,
                                         IFR_Bool          terminate,
                                         IFR_ConnectionItem &clink,
                                         IFR_Length     &dataoffset,
                                         IFR_Length     &offset,
                                         IFRConversion_Getval *getval);

    virtual IFRConversion_Putval *createPutval(IFRPacket_DataPart& part, 
                                               IFR_Parameter& parameter, 
                                               IFR_ConnectionItem& clink,
                                               IFR_Int4 dataoffset,
                                               IFR_size_t rowsize);

    virtual IFRConversion_Getval *createGetval (IFRPacket_DataPart& part, 
                                                IFR_Parameter* parameter, 
                                                IFR_ConnectionItem& clink,
                                                IFR_Int4 row,
                                                IFR_Int4 dataoffset,
                                                IFR_size_t rowsize);
    
    virtual IFR_Retcode translateBinaryLOBInput(IFRPacket_DataPart& datapart,
                                                IFR_LOBData         &lob,
                                                IFR_Length*         lengthindicator,
                                                IFR_Int4              dataoffset,
                                                IFR_ConnectionItem& clink);
    
    virtual IFR_Retcode translateAsciiLOBInput(IFRPacket_DataPart& datapart,
                                               IFR_LOBData           &lob,
                                               IFR_Length*         lengthindicator,
                                               IFR_Int4              dataoffset,
                                               IFR_ConnectionItem& clink);
    
    virtual IFR_Retcode translateUCS2LOBInput(IFRPacket_DataPart& datapart,
                                              IFR_LOBData           &lob,
                                              IFR_Bool            swapped,
                                              IFR_Length*         lengthindicator,
                                              IFR_Int4              dataoffset,
                                              IFR_ConnectionItem& clink);
    
    virtual IFR_Retcode translateUTF8LOBInput(IFRPacket_DataPart& datapart,
                                              IFR_LOBData         &lob,
                                              IFR_Length*         lengthindicator,
                                              IFR_Int4              dataoffset,
                                              IFR_ConnectionItem& clink);
    
    virtual IFR_Retcode translateNullInput(IFR_Parameter& parameter,
                                           IFRPacket_DataPart& datapart,
                                           IFR_ConnectionItem& clink,
                                           IFR_Int4 dataoffset,
                                           IFR_size_t rowsize);
    
    virtual IFR_Retcode translateDefaultInput(IFR_Parameter& parameter,
                                              IFRPacket_DataPart& datapart,
                                              IFR_ConnectionItem& clink,
                                              IFR_Int4 dataoffset,
                                              IFR_size_t rowsize);

    virtual IFR_Retcode translateBinaryLOBOutput(IFRPacket_DataPart& datapart,
                                                 IFR_LOBData        &lob,
                                                 IFR_Length*         lengthindicator,
                                                 IFR_Int4            dataoffset,
                                                 IFR_ConnectionItem& clink);
    
    virtual IFR_Retcode translateAsciiLOBOutput(IFRPacket_DataPart& datapart,
                                                IFR_LOBData        &lob,
                                                IFR_Length*         lengthindicator,
                                                IFR_Int4            dataoffset,
                                                IFR_ConnectionItem& clink);
    
    virtual IFR_Retcode translateUCS2LOBOutput(IFRPacket_DataPart& datapart,
                                               IFR_LOBData        &lob,
                                               IFR_Bool swapped,
                                               IFR_Length*         lengthindicator,
                                               IFR_Int4            dataoffset,
                                               IFR_ConnectionItem& clink);

    virtual IFR_Retcode translateUTF8LOBOutput(IFRPacket_DataPart& datapart,
                                               IFR_LOBData        &lob,
                                               IFR_Length*         lengthindicator,
                                               IFR_Int4            dataoffset,
                                               IFR_ConnectionItem& clink);


#define TRANSLATE_AS(xxtype)                                          \
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,  \
                                       xxtype& data,                  \
                                       IFR_Length *lengthindicator,   \
                                       IFR_ConnectionItem& clink,     \
                                       IFRConversion_Putval *pv);     \
    virtual IFR_Retcode translateOutput(IFRPacket_DataPart& datapart, \
                                        xxtype& data,                 \
                                        IFR_Length *lengthindicator,  \
                                        IFR_ConnectionItem& clink)

    
    TRANSLATE_AS(IFR_Int1);
    TRANSLATE_AS(IFR_Int2);
    TRANSLATE_AS(IFR_Int4);
    TRANSLATE_AS(IFR_Int8);
    TRANSLATE_AS(IFR_UInt1);
    TRANSLATE_AS(IFR_UInt2);
    TRANSLATE_AS(IFR_UInt4);
    TRANSLATE_AS(IFR_UInt8);
    TRANSLATE_AS(double);
    TRANSLATE_AS(float);
    TRANSLATE_AS(SQL_DATE_STRUCT);
    TRANSLATE_AS(SQL_TIME_STRUCT);
    TRANSLATE_AS(SQL_TIMESTAMP_STRUCT);
#undef TRANSLATE_AS    
protected:
    virtual IFR_Retcode getIntegralData(IFRPacket_DataPart& datapart,
                                        void *data,
                                        IFR_Length* lengthindicator,
                                        IFR_Parameter& parameter,
                                        IFR_Length& dataoffset,
                                        IFR_Length& offset,
                                        IFR_ConnectionItem& clink,
                                        IFR_Length&         datalength,
                                        IFRConversion_Getval *getval);
    
private:
    IFR_Bool m_bin2hex;
    IFR_Bool m_binary;
};


#endif // IFRCONVERSION_STREAMCONVERTER_H

