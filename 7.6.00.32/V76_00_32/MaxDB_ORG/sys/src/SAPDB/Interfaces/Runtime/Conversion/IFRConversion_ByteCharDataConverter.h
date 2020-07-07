/*!
  @file           IFRConversion_ByteCharDataConverter.h
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
#ifndef IFRCONVERSION_BYTECHARDATACONVERTER_H
#define IFRCONVERSION_BYTECHARDATACONVERTER_H

#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_Converter.h"
#include "SAPDB/Interfaces/Runtime/IFR_Connection.h"

/**
 * Class that handles the conversion from and into data types that represent
 * byte oriented data. There are two kinds of character oriented data in
 * MaxDB - BYTE and UCS2 - and this one is for BYTE only. This is used
 * for the data types:
 *
 * - CHAR BYTE
 * - VARCHAR BYTE
 * - CHAR ASCII
 * - VARCHAR ASCII
 * - DATE, TIME, TIMESTAMP (in ASCII mode)
 *
 * Take note that the encoding at side of the database is <i>always</i>
 * ASCII (resp. plain binary). For any type that uses an UCS2 layout
 *  at the database @c IFRConversion_UCS2CharDataConverter exists.
 */
class IFRConversion_ByteCharDataConverter
    : public IFRConversion_Converter
{
public:
    /**
     * Constructor.
     * @param shortinfo The short field infos of the parameter.
     * @param allocator The allocator used.
     * @param istrimming Flag, whether this instance will trim
     *        the blank character on output (CHARACTER VARYING).
     * @param integerconversion Flag, whether conversion to/from numbers
     *        is supported.
     */
    IFRConversion_ByteCharDataConverter(IFR_ShortInfo& shortinfo,
                                        SAPDBMem_IRawAllocator& allocator,
                                        IFR_Bool istrimming,
                                        IFR_Bool integerconversion, 
                                        IFR_Bool oraclenull);
    
    /**
     * Destructor.
     */
    virtual ~IFRConversion_ByteCharDataConverter();

    /**
     * This method will copy the data into <code>data</code>. The number of
     * bytes that shall be copied is computed as follows:
     * <ol>
     *   <li>If <code>lengthindicator</code> is 0, <code>datalength</code>
     *       is used.
     *   </li>
     *   <li>If <code>*lengthindicator</code> is <code>IFR_NTS</code>,
     *       <code>min(datalength, strlen(data))</code> is used.
     *   </li>
     *   <li>Otherwise, <code>min(datalength, *lengthindicator)</code>
     *       is used.
     * </ol>
     * It is an error, if <code>*lengthindicator</code> is <code>&lt; 0</code>.
     * by this method.
     * @param datapart        The destination data part.
     * @param data            Pointer to the binary data.
     * @param datalength      The maximum byte length of the data.
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @param clink           Link to connection for error reporting.
     * @return <code>IFR_NOT_OK</code> if the conversion of this
     *   parameter did not succeed.  If that computed length is greater
     *   than the length which is supported by the data type,
     *   <code>IFR_DATA_TRUNC</code> is returned.  <code>IFR_OK</code>
     *   is returned if everything was ok. In case of
     *   <code>IFR_NOT_OK</code>, the appropriate error code is stored
     *   in <code>clink</code>.
     */
    virtual IFR_Retcode translateBinaryInput(IFRPacket_DataPart& datapart,
                                             char               *data,
                                             IFR_Length            datalength,
                                             IFR_Length*           lengthindicator,
                                             IFR_ConnectionItem &clink);
    /**
     * This method will copy the data into <code>data</code>. The number of
     * bytes that shall be copied is computed as follows:
     * <ol>
     *   <li>If <code>lengthindicator</code> is 0, <code>datalength</code>
     *       is used.
     *   </li>
     *   <li>If <code>*lengthindicator</code> is <code>IFR_NTS</code>,
     *       <code>min(datalength, strlen(data))</code> is used.
     *   </li>
     *   <li>Otherwise, <code>min(datalength, *lengthindicator)</code>
     *       is used.
     * </ol>
     * It is supposed that <var>offset</var> bytes already reside as data,
     * and this data is appended. After the data is copied, <var>offset</var>
     * is updated, it can be at most the byte length used by this parameter.
     * @param datapart        The destination data part.
     * @param data            Pointer to the binary data.
     * @param swapped         The swapping of the input data.
     * @param datalength      The maximum byte length of the data.
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @param terminate Specifies that the output buffer should be finished
     *        with a C style zero terminator. The <code>Terminated</code> flag
     *        works only for the hostvar type character (ASCII, UCS2 or UTF8).
     * @param clink           Link to connection for error reporting.
     * @param offset          The offset where to start the insertion.
     * @return <code>IFR_NOT_OK</code> if the conversion of this
     *   parameter did not succeed.  If that computed length plus the given offset
     *   is greater
     *   than the length which is supported by the data type,
     *   <code>IFR_DATA_TRUNC</code> is returned.  <code>IFR_OK</code>
     *   is returned if everything was ok. In case of
     *   <code>IFR_NOT_OK</code>, the appropriate error code is stored
     *   in <code>clink</code>.
     */
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
                                            IFR_Bool            terminate,
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

    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_Int1&           data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_Int2&           data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_Int4&           data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_Int8&           data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_UInt1&          data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_UInt2&          data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_UInt4&          data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_UInt8&          data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       double&             data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       float&              data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

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

    virtual IFR_Retcode translateInput(IFRPacket_DataPart&   datapart,
                                       SQL_NUMERIC_STRUCT&   data,
                                       IFR_Length*             lengthindicator,
                                       IFR_ConnectionItem&   clink,
                                       IFRConversion_Putval *pv);

    virtual IFR_Retcode translateInput(IFRPacket_DataPart&   datapart,
                                       GUID&                 data,
                                       IFR_Length*             lengthindicator,
                                       IFR_ConnectionItem&   clink,
                                       IFRConversion_Putval *pv);

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
                                        float&                 data,
                                        IFR_Length*             lengthindicator,
                                        IFR_ConnectionItem&   clink);

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

    virtual IFR_Retcode translateOutput(IFRPacket_DataPart&   datapart,
                                        SQL_NUMERIC_STRUCT&   data,
                                        IFR_Length*             lengthindicator,
                                        IFR_ConnectionItem&   clink);

    virtual IFR_Retcode translateOutput(IFRPacket_DataPart&   datapart,
                                        GUID&                 data,
                                        IFR_Length*             lengthindicator,
                                        IFR_ConnectionItem&   clink);

protected:

    /**
     * Moves data to the buffer, and signals if the data was truncated
     * or can't converted to the buffer.
     * @param datapart The destination data part.
     * @param buffer   Source buffer.
     * @param bufferlength Number of bytes to copy in the buffer.
     * @param clink for error reporting.
     * @return <code>IFR_OK</code> on success, <code>IFR_DATA_TRUNC</code>
     *   if the data was not copied completely or <code>IFR_NOT_OK</code>
     *   if a conversion error occured.
     *
     */
    IFR_Retcode moveDataToPart(IFRPacket_DataPart& datapart,
                               char *buffer,
                               IFR_size_t bufferlength,
                               IFR_ErrorHndl& error);

    struct ConversionFlags {
        unsigned trimming                  : 1;
        unsigned numbers                   : 1;
        unsigned bin2hex                   : 1;
        unsigned oraclenull                : 1;
    };

    ConversionFlags     m_flags;
    IFR_StringEncoding  m_partencoding;
private:

    /**
     * Moves data from the data part, hereby converting it to ASCII
     * characters.
     * @param datapart The source data part.
     * @param buffer The target buffer.
     */
    void moveDataToBuffer(IFRPacket_DataPart& datapart,
                          char *buffer);


};

#endif
