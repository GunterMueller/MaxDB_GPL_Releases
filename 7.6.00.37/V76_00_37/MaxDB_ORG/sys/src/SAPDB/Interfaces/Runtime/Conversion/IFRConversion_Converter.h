/*!
  @file           IFRConversion_Converter.h
  @author         D030044
  @ingroup        IFR_DataConv
  @brief          Conversion of input and output data
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
#ifndef IFRCONVERSION_CONVERTER_H
#define IFRCONVERSION_CONVERTER_H

#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/IFR_Datatypes.h"
#include "Interfaces/Runtime/IFR_ErrorHndl.h"
#include "Interfaces/Runtime/IFR_String.h"
#include "Interfaces/Runtime/IFR_Parameter.h"
#include "Interfaces/Runtime/IFR_ShortInfo.h"
#include "Interfaces/Runtime/IFR_ConnectionItem.h"

#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_Part.h"
#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_Putval.h"
#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_Getval.h"

#include "SAPDB/SAPDBCommon/Algorithms/SAPDBAlgo_QuickSort.hpp"

#include <string.h>

IFR_BEGIN_NAMESPACE

/**
 * @ingroup IFR_DataConv
 * @brief Base class for data conversion.
 *
 * Base class used for data conversion. A concrete instance will exist
 * for all database types. A translation function will exist for all
 * host types.
 */
class IFRConversion_Converter
{
public:
    /**
     * Constructor.
     * @param shortinfo Short field info of the parameter.
     * @param allocator The allocator to be used.
     * @param paramindex Index of the parameter.
     */
    IFRConversion_Converter(IFR_ShortInfo& shortinfo,
                            SAPDBMem_IRawAllocator& allocator,
                            IFR_Int4 paramindex=-1);

    /**
     * Destructor.
     */
    virtual ~IFRConversion_Converter();

    /**
     * Translates an input parameter from a host type into the database's
     * native format, and puts it at the appropriate position in the
     * data part.
     * @param part       The data part to fill.
     * @param parameter  The parameter to insert.
     * @param clink      The link to the connection.
     * @param dataoffset The array offset of the data stored in the part, default 0.
     * @param rowsize    The row size of one row of host variables for row-wise binding, default 0.
     * @return <code>IFR_NOT_OK</code> if the conversion of this parameter did not succeed,
     *     <code>IFR_DATA_TRUNC</code> if a data truncation of the inserted parameter happened,
     *     <code>IFR_OK</code> if everything was ok. In case of <code>IFR_NOT_OK</code>, the
     *     appropriate error code is stored in <code>clink</code>.
     */
    IFR_Retcode translateInput(IFRPacket_DataPart& part,
                               IFR_Parameter& parameter,
                               IFR_ConnectionItem& clink,
                               IFR_Int4 dataoffset=0,
                               IFR_size_t rowsize=0);

    /**
     * Sets the input parameter to the NULL value.
     * @param datapart destination data part
     * @param clink link for error reporting.
     */
    virtual IFR_Retcode translateNullInput(IFRPacket_DataPart& datapart,
                                           IFR_ConnectionItem& clink);


    /**
     * Sets the input parameter to the DEFAULT value.
     * @param datapart destination data part
     * @param clink link for error reporting.
     */
    virtual IFR_Retcode translateDefaultInput(IFR_Parameter& parameter,
                                              IFRPacket_DataPart& datapart,
                                              IFR_ConnectionItem& clink,
                                              IFR_Int4 dataoffset,
                                              IFR_size_t rowsize);

    /**
     * Translates binary input data.
     * @param datapart        The data part to fill.
     * @param data            A pointer to binary data.
     * @param datalength      The maximum byte length of the data
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @param clink           A link to connection for error reporting.
     * @return <code>IFR_NOT_OK</code> if the conversion of this parameter did not succeed,
     *     <code>IFR_DATA_TRUNC</code> if a data truncation of the inserted parameter happened,
     *     <code>IFR_OK</code> if everything was ok. In case of <code>IFR_NOT_OK</code>, the
     *     appropriate error code is stored in <code>clink</code>.
     */
    virtual IFR_Retcode translateBinaryInput(IFRPacket_DataPart& datapart,
                                             char               *data,
                                             IFR_Length            datalength,
                                             IFR_Length*           lengthindicator,
                                             IFR_ConnectionItem& clink);

    virtual IFR_Retcode appendBinaryInput(IFRPacket_DataPart& datapart,
                                          char               *data,
                                          IFR_Length            datalength,
                                          IFR_Length*           lengthindicator,
                                          IFR_ConnectionItem& clink,
                                          IFR_Length         &offset,
                                          IFRConversion_Putval *putval);

    /**
     * Translates ascii input data.
     * @param datapart        The destination data part.
     * @param data            The pointer to the data.
     * @param datalength      The maximum byte length of the data.
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @return <code>IFR_NOT_OK</code> if the conversion of this parameter did not succeed,
     *     <code>IFR_DATA_TRUNC</code> if a data truncation of the inserted parameter happened,
     *     <code>IFR_OK</code> if everything was ok. In case of <code>IFR_NOT_OK</code>, the
     *     appropriate error code is stored in <code>clink</code>.
     */
    virtual IFR_Retcode translateAsciiInput(IFRPacket_DataPart& datapart,
                                            char               *data,
                                            IFR_Length            datalength,
                                            IFR_Length*           lengthindicator,
                                            IFR_Bool            terminate,
                                            IFR_Bool            ascii7bit,
                                            IFR_ConnectionItem& clink);


    virtual IFR_Retcode appendAsciiInput(IFRPacket_DataPart& datapart,
                                         char               *data,
                                         IFR_Length            datalength,
                                         IFR_Length*           lengthindicator,
                                         IFR_Bool              terminate,
                                         IFR_Bool              ascii7bit,
                                         IFR_ConnectionItem& clink,
                                         IFR_Length         &offset,
                                         IFRConversion_Putval *putval);

    /**
     * Translates UCS2 <b>big endian</b> input data.
     * @param datapart        The destination data part.
     * @param data            The pointer to the UCS2 data.
     * @param swapped         The input data is store in swapped byte order.
     * @param datalength      The maximum byte length of the data.
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @param clink           A link to connection for error reporting.
     * @return <code>IFR_NOT_OK</code> if the conversion of this parameter did not succeed,
     *     <code>IFR_DATA_TRUNC</code> if a data truncation of the inserted parameter happened,
     *     <code>IFR_OK</code> if everything was ok. In case of <code>IFR_NOT_OK</code>, the
     *     appropriate error code is stored in <code>clink</code>.
     */
    virtual IFR_Retcode translateUCS2Input(IFRPacket_DataPart& datapart,
                                           char               *data,
                                           IFR_Bool            swapped,
                                           IFR_Length          datalength,
                                           IFR_Length*         lengthindicator,
                                           IFR_Bool            terminate,
                                           IFR_ConnectionItem& clink);


    virtual IFR_Retcode appendUCS2Input(IFRPacket_DataPart& datapart,
                                        char               *data,
                                        IFR_Bool            swapped,
                                        IFR_Length          datalength,
                                        IFR_Length*         lengthindicator,
                                        IFR_Bool            terminate,
                                        IFR_ConnectionItem& clink,
                                        IFR_Length         &offset,
                                        IFRConversion_Putval *putval=0);

    /**
     * Translates UTF8 input data.
     * @param datapart        The destination data part.
     * @param data            The pointer to the UTF8 data.
     * @param datalength      The maximum byte length of the data.
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @param clink           A link to connection for error reporting.
     * @return <code>IFR_NOT_OK</code> if the conversion of this parameter did not succeed,
     *     <code>IFR_DATA_TRUNC</code> if a data truncation of the inserted parameter happened,
     *     <code>IFR_OK</code> if everything was ok. In case of <code>IFR_NOT_OK</code>, the
     *     appropriate error code is stored in <code>clink</code>.
     */
    virtual IFR_Retcode translateUTF8Input(IFRPacket_DataPart& datapart,
                                           char               *data,
                                           IFR_Length            datalength,
                                           IFR_Length*           lengthindicator,
                                           IFR_Bool              terminate,
                                           IFR_ConnectionItem& clink);


    virtual IFR_Retcode appendUTF8Input(IFRPacket_DataPart& datapart,
                                        char               *data,
                                        IFR_Length            datalength,
                                        IFR_Length*           lengthindicator,
                                        IFR_Bool              terminate,
                                        IFR_ConnectionItem& clink,
                                        IFR_Length         &offset,
                                        IFRConversion_Putval *putval=0);

    /**
     * Translates an 1-byte integer.
     * @param datapart        The destination data part.
     * @param data            Reference to the input data.
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @param clink      The link to the connection.
     * @return <code>IFR_NOT_OK</code> if the conversion of this parameter did not succeed,
     *     <code>IFR_DATA_TRUNC</code> if a data truncation of the inserted parameter happened,
     *     <code>IFR_OK</code> if everything was ok. In case of <code>IFR_NOT_OK</code>, the
     *     appropriate error code is stored in <code>clink</code>.
     */
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_Int1&           data,
                                       IFR_Length*         lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

    /**
     * Translates a 2-byte integer.
     * @param datapart        The destination data part.
     * @param data            Reference to the input data.
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @param clink      The link to the connection.
     * @return <code>IFR_NOT_OK</code> if the conversion of this parameter did not succeed,
     *     <code>IFR_DATA_TRUNC</code> if a data truncation of the inserted parameter happened,
     *     <code>IFR_OK</code> if everything was ok. In case of <code>IFR_NOT_OK</code>, the
     *     appropriate error code is stored in <code>clink</code>.
     */
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_Int2&           data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

    /**
     * Translates a 4-byte integer.
     * @param datapart        The destination data part.
     * @param data            Reference to the input data.
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @param clink      The link to the connection.
     * @return <code>IFR_NOT_OK</code> if the conversion of this parameter did not succeed,
     *     <code>IFR_DATA_TRUNC</code> if a data truncation of the inserted parameter happened,
     *     <code>IFR_OK</code> if everything was ok. In case of <code>IFR_NOT_OK</code>, the
     *     appropriate error code is stored in <code>clink</code>.
     */
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_Int4&           data,
                                       IFR_Length*         lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

    /**
     * Translates a 8-byte integer.
     * @param datapart        The destination data part.
     * @param data            Reference to the input data.
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @param clink      The link to the connection.
     * @return <code>IFR_NOT_OK</code> if the conversion of this parameter did not succeed,
     *     <code>IFR_DATA_TRUNC</code> if a data truncation of the inserted parameter happened,
     *     <code>IFR_OK</code> if everything was ok. In case of <code>IFR_NOT_OK</code>, the
     *     appropriate error code is stored in <code>clink</code>.
     */
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_Int8&           data,
                                       IFR_Length*         lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

    /**
     * Translates an unsigned 1-byte integer.
     * @param datapart        The destination data part.
     * @param data            Reference to the input data.
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @param clink      The link to the connection.
     * @return <code>IFR_NOT_OK</code> if the conversion of this parameter did not succeed,
     *     <code>IFR_DATA_TRUNC</code> if a data truncation of the inserted parameter happened,
     *     <code>IFR_OK</code> if everything was ok. In case of <code>IFR_NOT_OK</code>, the
     *     appropriate error code is stored in <code>clink</code>.
     */
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_UInt1&          data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

    /**
     * Translates an unsigned 2-byte integer.
     * @param datapart        The destination data part.
     * @param data            Reference to the input data.
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @param clink      The link to the connection.
     * @return <code>IFR_NOT_OK</code> if the conversion of this parameter did not succeed,
     *     <code>IFR_DATA_TRUNC</code> if a data truncation of the inserted parameter happened,
     *     <code>IFR_OK</code> if everything was ok. In case of <code>IFR_NOT_OK</code>, the
     *     appropriate error code is stored in <code>clink</code>.
     */
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_UInt2&          data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

    /**
     * Translates an unsigned 4-byte integer.
     * @param datapart        The destination data part.
     * @param data            Reference to the input data.
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @param clink      The link to the connection.
     * @return <code>IFR_NOT_OK</code> if the conversion of this parameter did not succeed,
     *     <code>IFR_DATA_TRUNC</code> if a data truncation of the inserted parameter happened,
     *     <code>IFR_OK</code> if everything was ok. In case of <code>IFR_NOT_OK</code>, the
     *     appropriate error code is stored in <code>clink</code>.
     */
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_UInt4&          data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

    /**
     * Translates an unsigned 8-byte integer.
     * @param datapart        The destination data part.
     * @param data            Reference to the input data.
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @param clink      The link to the connection.
     * @return <code>IFR_NOT_OK</code> if the conversion of this parameter did not succeed,
     *     <code>IFR_DATA_TRUNC</code> if a data truncation of the inserted parameter happened,
     *     <code>IFR_OK</code> if everything was ok. In case of <code>IFR_NOT_OK</code>, the
     *     appropriate error code is stored in <code>clink</code>.
     */
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       IFR_UInt8&          data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

    /**
     * Translates a double value.
     * @param datapart        The destination data part.
     * @param data            Reference to the input data.
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @param clink      The link to the connection.
     * @return <code>IFR_NOT_OK</code> if the conversion of this parameter did not succeed,
     *     <code>IFR_DATA_TRUNC</code> if a data truncation of the inserted parameter happened,
     *     <code>IFR_OK</code> if everything was ok. In case of <code>IFR_NOT_OK</code>, the
     *     appropriate error code is stored in <code>clink</code>.
     */
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       double&             data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

    /**
     * Translates a float value.
     * @param datapart        The destination data part.
     * @param data            Reference to the input data.
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @param clink      The link to the connection.
     * @return <code>IFR_NOT_OK</code> if the conversion of this parameter did not succeed,
     *     <code>IFR_DATA_TRUNC</code> if a data truncation of the inserted parameter happened,
     *     <code>IFR_OK</code> if everything was ok. In case of <code>IFR_NOT_OK</code>, the
     *     appropriate error code is stored in <code>clink</code>.
     */
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       float&              data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

    /**
     * Translates a <code>SQL_DATE_STRUCT</code> value.
     * @param datapart        The destination data part.
     * @param data            Reference to the input data.
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @param clink      The link to the connection.
     * @return <code>IFR_NOT_OK</code> if the conversion of this parameter did not succeed,
     *     <code>IFR_DATA_TRUNC</code> if a data truncation of the inserted parameter happened,
     *     <code>IFR_OK</code> if everything was ok. In case of <code>IFR_NOT_OK</code>, the
     *     appropriate error code is stored in <code>clink</code>.
     */
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       SQL_DATE_STRUCT&    data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);
    /**
     * Translates a <code>SQL_TIME_STRUCT</code> value.
     * @param datapart        The destination data part.
     * @param data            Reference to the input data.
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @param clink      The link to the connection.
     * @return <code>IFR_NOT_OK</code> if the conversion of this parameter did not succeed,
     *     <code>IFR_DATA_TRUNC</code> if a data truncation of the inserted parameter happened,
     *     <code>IFR_OK</code> if everything was ok. In case of <code>IFR_NOT_OK</code>, the
     *     appropriate error code is stored in <code>clink</code>.
     */
    virtual IFR_Retcode translateInput(IFRPacket_DataPart& datapart,
                                       SQL_TIME_STRUCT&    data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem& clink,
                                       IFRConversion_Putval *pv);

    /**
     * Translates a <code>SQL_TIMESTAMP_STRUCT</code> value.
     * @param datapart        The destination data part.
     * @param data            Reference to the input data.
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @param clink      The link to the connection.
     * @return <code>IFR_NOT_OK</code> if the conversion of this parameter did not succeed,
     *     <code>IFR_DATA_TRUNC</code> if a data truncation of the inserted parameter happened,
     *     <code>IFR_OK</code> if everything was ok. In case of <code>IFR_NOT_OK</code>, the
     *     appropriate error code is stored in <code>clink</code>.
     */
    virtual IFR_Retcode translateInput(IFRPacket_DataPart&   datapart,
                                       SQL_TIMESTAMP_STRUCT& data,
                                       IFR_Length*          lengthindicator,
                                       IFR_ConnectionItem&  clink,
                                       IFRConversion_Putval *pv);

    /**
     * Translates a <code>SQL_NUMERIC_STRUCT</code> value.
     * @param datapart        The destination data part.
     * @param data            Reference to the input data.
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @param clink      The link to the connection.
     * @return <code>IFR_NOT_OK</code> if the conversion of this parameter did not succeed,
     *     <code>IFR_DATA_TRUNC</code> if a data truncation of the inserted parameter happened,
     *     <code>IFR_OK</code> if everything was ok. In case of <code>IFR_NOT_OK</code>, the
     *     appropriate error code is stored in <code>clink</code>.
     */
    virtual IFR_Retcode translateInput(IFRPacket_DataPart&   datapart,
                                       SQL_NUMERIC_STRUCT&   data,
                                       IFR_Length*           lengthindicator,
                                       IFR_ConnectionItem&   clink,
                                       IFRConversion_Putval *pv);

    /**
     * Translates a <code>GUID</code> value.
     * @param datapart        The destination data part.
     * @param data            Reference to the input data.
     * @param lengthindicator The actual length of the data, or a special indicator value.
     * @param clink      The link to the connection.
     * @return <code>IFR_NOT_OK</code> if the conversion of this parameter did not succeed,
     *     <code>IFR_DATA_TRUNC</code> if a data truncation of the inserted parameter happened,
     *     <code>IFR_OK</code> if everything was ok. In case of <code>IFR_NOT_OK</code>, the
     *     appropriate error code is stored in <code>clink</code>.
     */
    virtual IFR_Retcode translateInput(IFRPacket_DataPart&   datapart,
                                       GUID&                 data,
                                       IFR_Length*             lengthindicator,
                                       IFR_ConnectionItem&   clink,
                                       IFRConversion_Putval *pv);
    
    virtual IFR_Retcode translateOmsTimestampInput(IFRPacket_DataPart& datapart,
                                                   char               *data,
                                                   IFR_Length          datalength,
                                                   IFR_Length         *length,
                                                   IFR_ConnectionItem& clink,
                                                   IFRConversion_Putval *pv);

    virtual IFR_Retcode translateOmsPacked_8_3_Input(IFRPacket_DataPart& datapart,
                                                     char               *data,
                                                     IFR_Length          datalength,
                                                     IFR_Length         *length,
                                                     IFR_ConnectionItem& clink,
                                                     IFRConversion_Putval *pv);

    virtual IFR_Retcode translateOmsPacked_15_3_Input(IFRPacket_DataPart& datapart,
                                                      char               *data,
                                                      IFR_Length          datalength,
                                                      IFR_Length         *length,
                                                      IFR_ConnectionItem& clink,
                                                      IFRConversion_Putval *pv);
    
    virtual IFR_Retcode translateDecimalInput(IFRPacket_DataPart& datapart,
                                              char               *data,
                                              IFR_Length          datalength,
                                              IFR_Length         *length,
                                              IFR_ConnectionItem& clink,
                                              IFRConversion_Putval *pv);


    IFR_Retcode translateOutput(IFRPacket_DataPart& part,
                                IFR_Parameter& parameter,
                                IFR_ConnectionItem& clink,
                                IFR_Int4 dataoffset=0,
                                IFR_size_t rowsize=0,
                                IFR_Bool ignoreUnsetParameter = false);


    IFR_Retcode translateRawHexOutput(IFRPacket_DataPart& datapart,
                                      char               *data,
                                      IFR_Length          datalength,
                                      IFR_Length*         lengthindicator,
                                      IFR_Bool            terminate,
                                      IFR_ConnectionItem &clink);


    virtual IFR_Retcode translateBinaryOutput(IFRPacket_DataPart& datapart,
                                              char           *data,
                                              IFR_Length        datalength,
                                              IFR_Length*           lengthindicator,
                                              IFR_ConnectionItem &clink);

    /**
     * Appends binary output.
     * @param datapart   The data part where the data resides.
     * @param data       A pointer to the output data.
     * @param datalength The total data length.
     * @param lengthindicator Indicator.
     * @param clink Link to connection.
     * @param dataoffset Offset for output. Data is actually appended at
     *      <code>(data + dataoffset)</code>. @c dataoffset is increased by the number of
     *      bytes transferred by this method.
     * @param offset Offset in the data that is read. This is updated by the number of bytes
     *   in the data that have been read.
     * @param getval Getval instance.
     */
    virtual IFR_Retcode appendBinaryOutput(IFRPacket_DataPart& datapart,
                                           char               *data,
                                           IFR_Length            datalength,
                                           IFR_Length*           lengthindicator,
                                           IFR_ConnectionItem& clink,
                                           IFR_Length         &dataoffset,
                                           IFR_Length         &offset,
                                           IFRConversion_Getval *getval);
    /**
     * This method will convert the data from the given datapart to an ASCII
     * character buffer pointed to <code>data</code>.
     * @param datapart        The source data part.
     * @param data            Pointer to the character data.
     * @param datalength      The maximum byte length of the data.
     * @param lengthindicator [out] The number of bytes without the possible
     *        termination bytes copied buffer pointed data. For termination
     *        of character data see the <code>terminate</code> argument.
     * @param terminate Specifies that the output buffer should be finished
     *        with a C style zero terminator.
     * @param clink           Link to connection for error reporting.
     * @return <code>IFR_NOT_OK</code> if the conversion of this
     *   parameter did not succeed.  If that computed length is greater
     *   than the length which is supported by the data type,
     *   <code>IFR_DATA_TRUNC</code> is returned.  <code>IFR_OK</code>
     *   is returned if everything was ok. In case of
     *   <code>IFR_NOT_OK</code>, the appropriate error code is stored
     *   in <code>clink</code>.
     */

    virtual IFR_Retcode translateAsciiOutput(IFRPacket_DataPart& datapart,
                                             char           *data,
                                             IFR_Length        datalength,
                                             IFR_Length*           lengthindicator,
                                             IFR_Bool        terminate,
                                             IFR_Bool        ascii7bit,
                                             IFR_ConnectionItem &clink);
    

    virtual IFR_Retcode appendAsciiOutput(IFRPacket_DataPart& datapart,
                                          char               *data,
                                          IFR_Length            datalength,
                                          IFR_Length*           lengthindicator,
                                          IFR_Bool            terminate,
                                          IFR_Bool            ascii7bit,
                                          IFR_ConnectionItem& clink,
                                          IFR_Length         &dataoffset,
                                          IFR_Length         &offset,
                                          IFRConversion_Getval *getval);

    
    /**
     * This method will convert the data from the given datapart to an UCS2
     * character buffer pointed to <code>data</code>.
     * @param datapart        The source data part.
     * @param data            Pointer to the character data.
     * @param swapped         The output data is store in swapped byte order.
     * @param datalength      The maximum byte length of the data.
     * @param lengthindicator [out] The number of bytes without the possible
     *        termination bytes copied buffer pointed data. For termination
     *        of character data see the <code>terminate</code> argument.
     * @param terminate Specifies that the output buffer should be finished
     *        with a C style zero terminator.
     * @param clink           Link to connection for error reporting.
     * @return <code>IFR_NOT_OK</code> if the conversion of this
     *   parameter did not succeed.  If that computed length is greater
     *   than the length which is supported by the data type,
     *   <code>IFR_DATA_TRUNC</code> is returned.  <code>IFR_OK</code>
     *   is returned if everything was ok. In case of
     *   <code>IFR_NOT_OK</code>, the appropriate error code is stored
     *   in <code>clink</code>.
     */

    virtual IFR_Retcode translateUCS2Output(IFRPacket_DataPart& datapart,
                                            char           *data,
                                            IFR_Bool        swapped,
                                            IFR_Length        datalength,
                                            IFR_Length*       lengthindicator,
                                            IFR_Bool        terminate,
                                            IFR_ConnectionItem &clink);

    virtual IFR_Retcode appendUCS2Output(IFRPacket_DataPart& datapart,
                                         char           *data,
                                         IFR_Bool        swapped,
                                         IFR_Length        datalength,
                                         IFR_Length*       lengthindicator,
                                         IFR_Bool          terminate,
                                         IFR_ConnectionItem &clink,
                                         IFR_Length     &dataoffset,
                                         IFR_Length     &offset,
                                         IFRConversion_Getval *getval);

    /**
     * This method will convert the data from the given datapart to an UTF8
     * character buffer pointed to <code>data</code>.
     * @param datapart        The source data part.
     * @param data            Pointer to the character data.
     * @param datalength      The maximum byte length of the data.
     * @param lengthindicator [out] The number of bytes without the possible
     *        termination bytes copied buffer pointed data. For termination
     *        of character data see the <code>terminate</code> argument.
     * @param terminate Specifies that the output buffer should be finished
     *        with a C style zero terminator. For UCS2 data a double zero is
     *        append to the data.
     * @param clink           Link to connection for error reporting.
     * @return <code>IFR_NOT_OK</code> if the conversion of this
     *   parameter did not succeed.  If that computed length is greater
     *   than the length which is supported by the data type,
     *   <code>IFR_DATA_TRUNC</code> is returned.  <code>IFR_OK</code>
     *   is returned if everything was ok. In case of
     *   <code>IFR_NOT_OK</code>, the appropriate error code is stored
     *   in <code>clink</code>.
     */

    virtual IFR_Retcode translateUTF8Output(IFRPacket_DataPart& datapart,
                                            char           *data,
                                            IFR_Length        datalength,
                                            IFR_Length*       lengthindicator,
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

    virtual IFR_Retcode translateOmsTimestampOutput(IFRPacket_DataPart& datapart,
                                                   char               *data,
                                                   IFR_Length          datalength,
                                                   IFR_Length         *length,
                                                   IFR_ConnectionItem& clink);

    virtual IFR_Retcode translateOmsPacked_8_3_Output(IFRPacket_DataPart& datapart,
                                                     char               *data,
                                                     IFR_Length          datalength,
                                                     IFR_Length         *length,
                                                     IFR_ConnectionItem& clink);

    virtual IFR_Retcode translateOmsPacked_15_3_Output(IFRPacket_DataPart& datapart,
                                                       char               *data,
                                                       IFR_Length          datalength,
                                                       IFR_Length         *length,
                                                       IFR_ConnectionItem& clink);
    
    virtual IFR_Retcode translateDecimalOutput(IFRPacket_DataPart& datapart,
                                              char               *data,
                                              IFR_Length          datalength,
                                              IFR_Length         *length,
                                              IFR_ConnectionItem& clink);
    
    /**
     * Puts input data that is delivered for late binding.
     * There are three different cases:
     * - integral data types (numbers, time, date, timestamp) must be delivered in
     *   one piece.
     * - string data (binary, ASCII, UCS2) can be delivered in pieces, as long as the
     *   host type is not an integral data type (a number or structure).
     * - same for long data, but internal handling varies due to necessary PUTDATA
     *   operations.
     * @param datapart The datapart where to insert the data.
     * @param data     The piece of data delivered now.
     * @param lengthindicator The length or indicator value.
     * @param parameter The parameter to set.
     * @param offset The offset (bytes) of the data already have been written. The value
     *               is updated by this method ,
     * @param nullordefault Whether a NULL or DEFAULT value is inserted by this method.
     * @param integral Whether the inserted value is integral (i.e. an append is not possible).
     * @param clink Link to connection for error reporting.
     * @param putval The PUTVAL object that is associated with this operation.
     * @return The status of the conversion and insertion, which may be <code>IFR_OK</code>,
     *         <code>IFR_NOT_OK</code>, <code>IFR_OVERFLOW</code> or <code>IFR_DATA_TRUNC</code>,
     *         <code>IFR_NO_DATA_FOUND</code> if there was no data supplied.
     */
    IFR_Retcode putData(IFRPacket_DataPart& datapart,
                        void *data,
                        IFR_Length* lengthindicator,
                        IFR_Parameter& parameter,
                        IFR_Length& offset,
                        IFR_Bool &nullordefault,
                        IFR_Bool &integral,
                        IFR_ConnectionItem& clink,
                        IFRConversion_Putval *putval);

    /**
     * Gets long data piecewise from long column.
     * @param datapart The datapart where to get the data from.
     * @param data     The buffer the data has to be delivered.
     * @param lengthindicator The length or indicator value.
     * @param parameter The parameter to set.
     * @param offset     The offset (bytes) of data that has already been read. 
     * @param dataoffset The offset (bytes) of the data already have been written. The value
     *                   is updated by this method
     * @param nullordefault Whether a NULL or DEFAULT value is inserted by this method.
     * @param integral Whether the inserted value is integral (i.e. an append is not possible).
     * @param clink Link to connection for error reporting.
     * @param getval The GETVAL object that is associated with this operation.
     * @return The status of the conversion and insertion, which may be <code>IFR_OK</code>,
     *         <code>IFR_NOT_OK</code> or <code>IFR_NEED_DATA</code>.
     */
    IFR_Retcode getData(IFRPacket_DataPart& datapart,
                        void *data,
                        IFR_Length* lengthindicator,
                        IFR_Parameter& parameter,
                        IFR_Length& dataoffset,
                        IFR_Length& offset,
                        IFR_ConnectionItem& clink,
                        IFR_Length&         datalength,
                        IFRConversion_Getval *getval);

    /**
     * Gets the index of this parameter.
     * @return the parameter index, or -1 if this was not set.
     */
    inline IFR_Int4 getIndex()
    {
        return m_index;
    }

    /**
     * Sets the parameter index.
     * @param index the parameter index
     */
    inline void setIndex(IFR_Int4 index)
    {
        m_index=index;
    }

    /**
     * Sets the parameter name.
     * @param name The name of the parameter (e.g. from column names)
     * @param ok   Set to false if the assignment fails due to
     *             memory allocation problems.
     */
    inline void setName(const IFR_String& name,IFR_Bool& ok)
    {
        m_name.assign(name, ok);
    }

    /**
     * @brief Get the parameter/column name.
     * @param buffer the buffer where the column name should be copied into
     * @param encoding the encoding of the column name that is requested
     * @param bufferSize The size in bytes of the target buffer.
     * @param bufferLength [out] The number of bytes copied to the buffer,
     *        except the number of bytes necessary for the zero terminator.
     *        If the source string exceeds the <code>bufferSize</code>.
     *        IFR_DATA_TRUNC will returned and the <code>bufferLength</code>
     *        is set to the number of bytes (except the terminator bytes)
     *        needed to copy without truncation.
     * @return <code>IFR_OK</code> on success, <code>IFR_DATA_TRUNC</code> if the buffer is too small.
     */
    IFR_Retcode getName(char *buffer,
                        IFR_StringEncoding encoding,
                        IFR_Length bufferSize,
                        IFR_Length *bufferLength) const;

    inline const IFR_String& getName() const
    {
        return m_name;
    }

    /**
     * Get the logical length.
     * @return The length from the short field info.
     */
    inline IFR_UInt2 getLength() const
    {
        return m_shortinfo.length;
    }

    /**
     * Get the physical data length.
     */
    inline IFR_UInt2 getIOLength() const
    {
        return m_shortinfo.iolength;
    }

    /**
     * Gets the precision.
     */
    inline IFR_UInt1 getPrecision() const
    {
        return m_shortinfo.frac;
    }

    /**
     * Return whether this column is nullable.
     * @return <code>true</code> if the parameter accepts NULL values,
     *   <code>false</code> if not.
     */
    inline IFR_Bool isNullable() const
    {
        return m_shortinfo.mode & 1 ? false : true;
    }

    /**
     * Get current position in datapart.
     */
    inline IFR_UInt4 getBufpos() const
    {
        return m_shortinfo.pos.bufpos;
    }

    /**
     * Returns true if this is an input parameter.
     */
    inline IFR_Bool supportsInput() const
    {
        return m_shortinfo.isInput();
    }

    /**
     * Returns true if this is an output parameter.
     */
    inline IFR_Bool supportsOutput() const
    {
        return m_shortinfo.isOutput();
    }

    /**
     * Checks whether this value is <code>NULL</code>.
     * @param datapart The datapart which contains the value.
     * @param clink The link to the connection, for tracing and error reporting.
     * @return <code>true</code> if a <code>NULL</code> value
     *         is found, false otherwise.
     */
    IFR_Bool isNull(IFRPacket_DataPart& datapart, IFR_ConnectionItem& clink);


    /**
     * Creates a new PUTVAL object for this converter instance.
     * @param part       The data part to fill.
     * @param parameter  The parameter to insert.
     * @param clink      The link to the connection.
     * @param dataoffset The array offset of the data stored in the part, default 0.
     * @param rowsize    The row size of one row of host variables for row-wise binding, default 0.
     *
     * @return The newly created PUTVAL object, or 0 if that
     *         operation is not applicable.
     */
    virtual IFRConversion_Putval *createPutval(IFRPacket_DataPart& part,
                                               IFR_Parameter& parameter,
                                               IFR_ConnectionItem& clink,
                                               IFR_Int4 dataoffset=0,
                                               IFR_size_t rowsize=0);


    /**
     * Creates a new GETVAL object for this converter instance.
     * @param part       The data part to fill.
     * @param parameter  The parameter to insert.
     * @param clink      The link to the connection.
     * @param dataoffset The array offset of the data stored in the part, default 0.
     * @param rowsize    The row size of one row of host variables for row-wise binding, default 0.
     *
     * @return The newly created GETVAL object, or 0 if that
     *         operation is not applicable.
     */
    virtual IFRConversion_Getval *createGetval (IFRPacket_DataPart& part,
                                                IFR_Parameter* parameter,
                                                IFR_ConnectionItem& clink,
                                                IFR_Int4 row, 
                                                IFR_Int4 dataoffset,
                                                IFR_size_t rowsize);


    virtual IFR_Retcode translateBinaryLOBInput(IFRPacket_DataPart& datapart,
                                                IFR_LOBData           &lob,
                                                IFR_Length*         lengthindicator,
                                                IFR_Int4            dataoffset,
                                                IFR_ConnectionItem& clink);

    virtual IFR_Retcode translateAsciiLOBInput(IFRPacket_DataPart& datapart,
                                               IFR_LOBData           &lob,
                                               IFR_Length*         lengthindicator,
                                               IFR_Int4            dataoffset,
                                               IFR_ConnectionItem& clink);

    virtual IFR_Retcode translateUCS2LOBInput(IFRPacket_DataPart& datapart,
                                              IFR_LOBData           &lob,
                                              IFR_Bool            swapped,
                                              IFR_Length*         lengthindicator,
                                              IFR_Int4            dataoffset,
                                              IFR_ConnectionItem& clink);

    virtual IFR_Retcode translateUTF8LOBInput(IFRPacket_DataPart& datapart,
                                              IFR_LOBData           &lob,
                                              IFR_Length*         lengthindicator,
                                              IFR_Int4            dataoffset,
                                              IFR_ConnectionItem& clink);


    virtual IFR_Retcode translateABAPStreamInput(IFRPacket_DataPart& datapart,
                                                 void               *streamDescriptor,
                                                 IFR_Length           datalength,
                                                 IFR_Length         *lengthindicator,
                                                 IFR_Int4            dataoffset,
                                                 IFR_ConnectionItem& clink);


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

    /**
     * Puts an ABAP stream.
     * @param parameter The stream parameter.
     * @param datapart The data part to fill.
     * @param clink The link to the connection.
     */
    virtual IFR_Retcode putABAPStream(IFR_Parameter& parameter,
                                      IFRPacket_DataPart& datapart,
                                      void               *maskptr,
                                      IFR_Int4            masklength,
                                      IFR_Int4            requiredrows,
                                      IFR_Bool &          last,
                                      IFR_ConnectionItem& clink);

    /**
     * Puts an ABAP stream.
     * @param parameter The stream parameter.
     * @param datapart The data part to fill.
     * @param maskptr The exclusion mask ptr, as delivered by the stream.
     * @param abap_rc The return code from the callback function.
     * @param clink The link to the connection.
     */
    virtual IFR_Retcode getABAPStream(IFR_Parameter& parameter,
                                      IFRPacket_DataPart& datapart,
                                      IFR_Int4 masklength,
                                      void *maskptr,
                                      IFR_Int4& abap_rc,
                                      IFR_ConnectionItem& clink);

    

    /**
     * @brief Get whether this data item is LONG data.
     * @return <code>true</code> if long data present,
     *   <code>false</code> otherwise
     */
    inline IFR_Bool isLong() const
    {
        return m_shortinfo.isLong();
    }

    /**
     * Returns <i>true</i> if this value
     * represents an integral data type, not a character or
     * byte sequence.
     * @return <code>true</code> if this data type is not
     *   a CHAR, VARCHAR or LONG.
     */
    inline IFR_Bool isIntegral() const
    {
        return m_shortinfo.isIntegral();
    }

    /**
     * Get whether the parameter is an ABAP/OMS stream.
     * @return @c true if the parameter is an ABAP/OMS stream.
     */
    inline IFR_Bool isABAPStream() const
    {
        return m_shortinfo.isABAPStream();
    }

    /**
     * Gets the 'defined byte' for the data type stored in
     * this short info object.
     * @param forUnicode whether this is queried for an unicode database.
     * @return the value of the defined byte for the datatype.
     */
    inline IFR_Byte getDefinedByte(IFR_Bool forUnicode=false)
    {
        return m_shortinfo.getDefinedByte(forUnicode);
    }

    /**
     * Gets the data type.
     * @return The SQL type.
     */
    inline IFR_SQLType getSQLType() const
    {
        return (IFR_SQLType) m_shortinfo.datatype;
    }

    inline IFR_ShortInfo& shortinfo()
    {
        return m_shortinfo;
    }

    void sqlTrace(IFR_TraceStream& s);

protected:
    /**
     * Called when integral data is read from a LONG column. 
     * 
     */
     virtual IFR_Retcode getIntegralData(IFRPacket_DataPart& datapart,
                                void *data,
                                IFR_Length* lengthindicator,
                                IFR_Parameter& parameter,
                                         IFR_Length& dataoffset,
                                IFR_Length& offset,
                                         IFR_ConnectionItem& clink,
                                         IFR_Length&         datalength,
                                         IFRConversion_Getval *getval);


    IFR_ShortInfo m_shortinfo;     //!< the short field infos that are used for conversion
    IFR_String    m_name;          //!< The parameter or column name.
    IFR_Int4      m_index;         //!< The parameter or column index

    friend IFR_TraceStream& operator <<(IFR_TraceStream&,const IFRConversion_Converter&);

};

/**
 * List of converters.
 * The methods of the vector are not exposed, only some are available.
 */
class IFRConversion_ConverterList : protected IFRUtil_Vector<IFRConversion_Converter*>
{
public:
    IFRConversion_ConverterList(SAPDBMem_IRawAllocator& allocator)
    :IFRUtil_Vector< IFRConversion_Converter* >(allocator),
     m_reference(false),
     m_posvector(0),
     m_indirectpos(false)
    {}

    ~IFRConversion_ConverterList()
    {
        clear();
    }

    void clear()
    {
        if(!m_reference) {
            IFRConversion_Converter **p=Data();
            SAPDBMem_IRawAllocator& alloc = allocator();
            IFR_UInt4 sz = GetSize();
            for(IFR_UInt4 i=0; i < sz; ++i) {
                IFRUtil_Delete(p[i], alloc);
            }
        }
        if(m_posvector) {
            allocator().Deallocate(m_posvector);
            m_posvector = 0;
        }
        Clear();
    }

    inline void add(IFRConversion_Converter* c, IFR_Bool& memory_ok)
    {
        InsertEnd(c, memory_ok);
        m_reference = false;
    }

    inline void buildIndex(IFR_Bool& memory_ok)
    {
        if(memory_ok) {
            m_indirectpos = true;
            if(m_posvector) {
                allocator().Deallocate(m_posvector);
            }
            IFR_UInt4 sz = size();
            m_posvector = (IFR_Int2 *) allocator().Allocate(sz * sizeof(IFR_Int2));
            if(m_posvector == 0) {
                memory_ok = false;
                return;
            }
            for(IFR_UInt4 i=0; i<sz; ++i) {
                m_posvector[i] = i;
            }
            SAPDBAlgo_QuickSort(m_posvector, sz, *this);
        }
    }

    inline void swap(IFRConversion_ConverterList& l, IFR_Bool& memory_ok)
    {
        Swap(l, memory_ok);
        IFRUtil_Swap(m_reference, l.m_reference);
        IFRUtil_Swap(m_indirectpos, l.m_indirectpos);
        IFRUtil_Swap(m_posvector, l.m_posvector);
    }

    inline void append(IFRConversion_ConverterList& l, IFR_Bool& memory_ok)
    {
        if(memory_ok) {
            Iterator e = l.End();
            for(Iterator i=l.Begin(); i!=e; ++i) {
                add(*i, memory_ok);
            }
            l.Clear();
        }
    }

    inline void copyref(IFRConversion_ConverterList& l, IFR_Bool& memory_ok)
    {
        if(memory_ok) {
            clear();
            IFR_UInt4 sz = l.size();
            Reserve(sz, memory_ok);
            if(memory_ok) {
                memcpy(Data(), l.Data(), sizeof(IFRConversion_Converter*) * sz);
                setSize(sz);
                m_reference = true;
                m_indirectpos = l.m_indirectpos;
                if(m_indirectpos) {
                    m_posvector = (IFR_Int2 *)allocator().Allocate(sz * sizeof(IFR_Int2));
                    if(m_posvector == 0) {
                        memory_ok = false;
                        return;
                    }
                    memcpy(m_posvector, l.m_posvector, sz);
                }
            }
        }
    }

    inline void copyref(IFRConversion_Converter** l, IFR_UInt4 count, IFR_Bool indirectpos, IFR_Bool& memory_ok)
    {
        if(memory_ok) {
            clear();
            IFR_UInt4 sz = count;
            Reserve(sz, memory_ok);
            if(memory_ok) {
                memcpy(Data(), l, sizeof(IFRConversion_Converter*) * sz);
                setSize(sz);
                m_reference = true;
                if(indirectpos) {
                    buildIndex(memory_ok);
                }
            }
        }
    }

    inline IFR_size_t size()
    {
        return GetSize();
    }

    inline IFR_size_t size() const
    {
        return GetSize();
    }

    inline IFRConversion_Converter*& operator[] (IFR_Int4 index)
    {
        return IFRUtil_Vector<IFRConversion_Converter*>::operator [](index);
    }
    
    inline IFR_Int4 parameterForInputPosition(IFR_Int4 index)
    {
        if(m_indirectpos) {
            return m_posvector[index];
        } else {
            return index;
        }
    }
    
    IFR_Bool IsPredecessor(IFR_UInt2 a, IFR_UInt2 b)
    {
        IFR_ShortInfo *shortinfo_a = &(IFRUtil_Vector<IFRConversion_Converter*>::operator [](m_posvector[a])->shortinfo());
        IFR_ShortInfo *shortinfo_b = &(IFRUtil_Vector<IFRConversion_Converter*>::operator [](m_posvector[b])->shortinfo());
        if(shortinfo_a->isOutput()) {
            return a < b;
        } else {
            if(shortinfo_b->isOutput()) {
                return a < b;
            } else {
                return shortinfo_a->pos.newpos.position < shortinfo_b->pos.newpos.position;
            }
        }
    }

    inline IFR_Bool hasVariableInput() const
    {
        return m_indirectpos;
    }

private:

    IFRConversion_ConverterList& operator = (const IFRConversion_ConverterList&);
    IFRConversion_ConverterList(const IFRConversion_ConverterList&);

    IFR_Bool   m_reference;
    IFR_Int2  *m_posvector;
    IFR_Bool   m_indirectpos;
};


/**
 * @ingroup IFR_Trace
 * Trace output operator.
 */
IFR_TraceStream&
operator << (IFR_TraceStream& s,const IFRConversion_Converter& c);

inline IFR_Length IFRConversion_Strnlen(char *p, IFR_Length sz)
{
    register char *pf = (char *) memchr(p, 0, sz);
    if(pf) {
        return pf - p;
    } else {
        return sz;
    }
}

/**
 * Length in bytes for UCS2.
 */
inline IFR_Length IFRConversion_StrnlenUCS2(char *p, IFR_Length sz)
{
    register char *pp = p;
    IFR_Length all = sz;
    while(sz >= 2) {
        if(*p == 0) {
            ++p;
            if(*p == 0) {
                return p - pp - 1;
            }
             ++p;
        } else {
            p += 2;
        }
        sz -= 2;
    }
    return all; 
}

/**
 * Common method for determining the length of binary/ASCII/UTF8 input.
 * @param datalength      The data length specified for the parameter.
 * @param lengthindicator The length/indicator value.
 * @param data            The data.
 * @param byteslength     The length of the data in bytes (output).
 * @return @c 0 if the computation succeeds, and @c byteslength is filled,
 *    a value not equal to @c 0 otherwise.
 */
inline IFR_Int4 IFRConversion_InputDataLength(IFR_Length datalength,
                                              IFR_Length *lengthindicator,
                                              char       *data,
                                              IFR_Length& byteslength,
                                              IFR_Bool    terminate)
{
    if(lengthindicator) {
        register IFR_Length l = *lengthindicator;
        if(l < 0) {
            if(l == IFR_NTS) {
                if(datalength == 0) {
                    byteslength = strlen(data);
                } else {
                    byteslength = IFRConversion_Strnlen(data, l);
                }
                return 0;
            }
            return 1;
        } else {
            if(datalength != 0
               && datalength < l) {
                byteslength = datalength;
                return 0;
            } else {
                byteslength = l;
                return 0;
            }
        }
    } else {
        if(terminate) {
            if(datalength == 0) {
                byteslength = strlen(data);
            } else {
                byteslength = IFRConversion_Strnlen(data, datalength);
            }
        } else {
            byteslength = datalength;
        }
        return 0;
    }
}

/**
 * Input data length for UCS2 strings.
 */
inline IFR_Int4 IFRConversion_InputDataLengthUCS2(IFR_Length datalength,
                                                  IFR_Length *lengthindicator,
                                                  char       *data,
                                                  IFR_Length& byteslength,
                                                  IFR_Bool    terminate)
{
    if(lengthindicator) {
        register IFR_Length l = *lengthindicator;
        if(l < 0) {
            if(l == IFR_NTS) {
                if(datalength == 0) {
                    byteslength = IFRConversion_StrnlenUCS2(data, MAX_IFR_INT4);
                } else {
                    byteslength = IFRConversion_StrnlenUCS2(data, datalength);
                }
                return 0;
            }
            return 1;
        } else {
            if(datalength != 0 && datalength < l) {
                byteslength = datalength;
                return 0;
            } else {
                byteslength = l;
                return 0;
            }
        }
    } else {
        if(terminate) {
            if(datalength == 0) {
                byteslength = IFRConversion_StrnlenUCS2(data, MAX_IFR_INT4);
            } else {
                byteslength = IFRConversion_StrnlenUCS2(data, datalength);
            }
        } else {
            byteslength = datalength;
        }
        return 0;
    }
}



/**
 * Count ASCII padding characters.
 */
inline IFR_Length IFRConversion_StringPadLength(char *data, IFR_Length byteslength, char padchar)
{
    register char *end = (--data) + byteslength;
    while(end != data) {
        if(*end != padchar) {
            return end - data;
        }
        --end;
    }
    return 0;
}

/**
 * Count UCS2 padding characters. The length of the string in bytes is
 * returned, assuming the string is padded at the end with the padding
 * character.
 * @param data        The string to be checked.
 * @param byteslength The maximum string length in bytes.
 * @param padchar     The padding character.
 * @param swapped     Whether the string is swapped (little-endian) or not.
 */
inline IFR_Length IFRConversion_StringPadLengthUCS2(char *data,
                                                    IFR_Length byteslength,
                                                    IFR_Int4 padchar,
                                                    IFR_Bool swapped)
{
    register char *p = (--data) + byteslength;
    register IFR_Int4 first;
    register IFR_Int4 second;
    if(swapped) {
        first  = padchar & 0xFF;
        second = (padchar >> 8) & 0xFF;
    } else {
        second = padchar & 0xFF;
        first  = (padchar >> 8) & 0xFF;
    }
    // 1st byte is 'second', as we move backwards
    while(p > data) {
        if(*p != second) {
            return p - data;
        }
        --p;
        if(p > data) {
            if(*p != first) {
                return (p - data) + 1;
            }
        }
        --p;
    }
    return 0;
}

/**
 * Determine the length of an UCS2 string as UTF8. 
 */
inline IFR_Length 
IFRConversion_StringUCS2LengthAsUTF8(unsigned char *buffer,
                                     IFR_Length bytelength,
                                     IFR_Bool swapped)
{
    IFR_Length utf8length=0;
    if(swapped) {
        for(IFR_Length i=0; i<bytelength-1; i+=2) {
            IFR_UInt4 c = buffer[i] + (((IFR_UInt4)buffer[i+1]) << 8);
            if(c < 0x80) {
                utf8length += 1;
            } else if(c < 0x800) {
                utf8length += 2;
            } else {
                utf8length += 3;
            }
        }
    } else {
        for(IFR_Length i=0; i<bytelength-1; i+=2) {
            IFR_UInt4 c = buffer[i+1] + (((IFR_UInt4)buffer[i]) << 8);
            if(c < 0x80) {
                utf8length += 1;
            } else if(c < 0x800) {
                utf8length += 2;
            } else {
                utf8length += 3;
            }
        }
    }
    return utf8length;
}

inline IFR_Length 
IFRConversion_StringAsciiLengthAsUTF8(unsigned char *buffer,
                                      IFR_Length bytelength)
{
    IFR_Length utf8length = 0;
    for(IFR_Length i=0; i<bytelength; ++i) {
        if(buffer[i] < 0x80) {
            ++utf8length;
        } else {
            utf8length+=2;
        }
    }
    return utf8length;
}

inline IFR_Bool
IFRConversion_StringIsAscii7(unsigned char *buffer, 
                             IFR_Length length)
{
    while(length > 0) {
        if(*buffer & 0x80) {
            return false;
        }
        --buffer;
        --length;
    }
    return true;
}

IFR_END_NAMESPACE                                                

#endif //IFRCONVERSION_CONVERTER_H
