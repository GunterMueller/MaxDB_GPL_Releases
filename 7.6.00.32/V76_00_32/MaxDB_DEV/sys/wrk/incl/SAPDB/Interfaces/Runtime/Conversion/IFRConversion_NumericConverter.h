/*!
  @file           IFRConversion_NumericConverter.h
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
#ifndef IFRCONVERSION_NUMERICCONVERTER_H
#define IFRCONVERSION_NUMERICCONVERTER_H

#include "Interfaces/Runtime/Conversion/IFRConversion_Converter.h"

/**
 * @ingroup IFR_DataConv
 * Converter for numeric values. Note that all numeric database
 * values use this converter, as they share a common representation
 * (VDN Number) in the database.
 */
class IFRConversion_NumericConverter 
    : public IFRConversion_Converter 
{
public:    
    /**
     * Constructor.
     * @param shortinfo Short field info of the parameter.
     * @param allocator The allocator to be used.
     */
    IFRConversion_NumericConverter(IFR_ShortInfo& shortinfo,
                                   SAPDBMem_IRawAllocator& allocator,
                                   IFR_Connection& connection);
    
    /**
     * Destructor.
     */
    virtual ~IFRConversion_NumericConverter();


    virtual IFR_Retcode translateBinaryInput(IFRPacket_DataPart& datapart,
                                             char               *data,
                                             IFR_Length            datalength,
                                             IFR_Length*           lengthindicator,
                                             IFR_ConnectionItem& clink);
    

    virtual IFR_Retcode translateAsciiInput(IFRPacket_DataPart& datapart,
                                            char               *data,
                                            IFR_Length            datalength,
                                            IFR_Length*           lengthindicator,
                                            IFR_Bool              terminate,
                                            IFR_Bool              ascii7bit,
                                            IFR_ConnectionItem&   clink);


    virtual IFR_Retcode translateUCS2Input(IFRPacket_DataPart& datapart,
                                           char               *data,
                                           IFR_Bool            swapped,
                                           IFR_Length          datalength,
                                           IFR_Length*         lengthindicator,
                                           IFR_Bool            terminate,
                                           IFR_ConnectionItem& clink);

    virtual IFR_Retcode translateUTF8Input(IFRPacket_DataPart& datapart,
                                           char               *data,
                                           IFR_Length          datalength,
                                           IFR_Length*         lengthindicator,
                                           IFR_Bool            terminate,
                                           IFR_ConnectionItem& clink);

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

    
    virtual IFR_Retcode translateInput(IFRPacket_DataPart&   datapart,
                                        IFR_Int1&             data,
                                        IFR_Length*             lengthindicator,
                                       IFR_ConnectionItem&   clink,
                                       IFRConversion_Putval *pv);

    virtual IFR_Retcode translateInput(IFRPacket_DataPart&   datapart,
                                        IFR_Int2&             data,
                                        IFR_Length*             lengthindicator,
                                       IFR_ConnectionItem&   clink,
                                       IFRConversion_Putval *pv);

    virtual IFR_Retcode translateInput(IFRPacket_DataPart&   datapart,
                                        IFR_Int4&             data,
                                        IFR_Length*             lengthindicator,
                                       IFR_ConnectionItem&   clink,
                                       IFRConversion_Putval *pv);

    virtual IFR_Retcode translateInput(IFRPacket_DataPart&   datapart,
                                        IFR_Int8&             data,
                                        IFR_Length*             lengthindicator,
                                       IFR_ConnectionItem&   clink,
                                       IFRConversion_Putval *pv);

    virtual IFR_Retcode translateInput(IFRPacket_DataPart&   datapart,
                                        IFR_UInt1&             data,
                                        IFR_Length*             lengthindicator,
                                       IFR_ConnectionItem&   clink,
                                       IFRConversion_Putval *pv);

    virtual IFR_Retcode translateInput(IFRPacket_DataPart&   datapart,
                                        IFR_UInt2&             data,
                                        IFR_Length*             lengthindicator,
                                       IFR_ConnectionItem&   clink,
                                       IFRConversion_Putval *pv);

    virtual IFR_Retcode translateInput(IFRPacket_DataPart&   datapart,
                                        IFR_UInt4&             data,
                                        IFR_Length*             lengthindicator,
                                       IFR_ConnectionItem&   clink,
                                       IFRConversion_Putval *pv);
    
    virtual IFR_Retcode translateInput(IFRPacket_DataPart&   datapart,
                                        IFR_UInt8&             data,
                                        IFR_Length*             lengthindicator,
                                       IFR_ConnectionItem&   clink,
                                       IFRConversion_Putval *pv);

    virtual IFR_Retcode translateInput(IFRPacket_DataPart&   datapart,
                                        SQL_NUMERIC_STRUCT&      data,
                                        IFR_Length*             lengthindicator,
                                       IFR_ConnectionItem&   clink,
                                       IFRConversion_Putval *pv);

    virtual IFR_Retcode translateOutput(IFRPacket_DataPart&   datapart,
                                        SQL_NUMERIC_STRUCT&      data,
                                        IFR_Length*             lengthindicator,
                                        IFR_ConnectionItem&   clink);

    virtual IFR_Retcode translateDecimalInput(IFRPacket_DataPart& datapart,
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
    
    virtual IFR_Retcode translateOmsTimestampInput(IFRPacket_DataPart& datapart,
                                                   char               *data,
                                                   IFR_Length          datalength,
                                                   IFR_Length         *length,
                                                   IFR_ConnectionItem& clink,
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
                                        float&               data,
                                        IFR_Length*             lengthindicator,
                                        IFR_ConnectionItem&   clink);

    /** 
     * This method will convert the data from the given numeric datapart to 
     * an ASCII character buffer pointed to <code>data</code>.
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

    virtual IFR_Retcode translateAsciiOutput(IFRPacket_DataPart& datapart,
                                             char           *data,
                                             IFR_Length        datalength,
                                             IFR_Length*       lengthindicator,
                                             IFR_Bool       terminate,
                                             IFR_Bool       ascii7bit,
                                             IFR_ConnectionItem &clink);

    /** 
     * This method will convert the data from the given numeric datapart to 
     * an UTF8 character buffer pointed to <code>data</code>.
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
                                            IFR_Bool       terminate,
                                            IFR_ConnectionItem &clink);

    /** 
     * This method will convert the data from the given numeric datapart to 
     * an UCS2 character buffer pointed to <code>data</code>.
     * @param datapart        The source data part.
     * @param data            Pointer to the character data.
     * @param swapped         The output data is store in swapped byte order.
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

    virtual IFR_Retcode translateUCS2Output(IFRPacket_DataPart& datapart,
                                            char           *data,
                                            IFR_Bool        swapped,
                                            IFR_Length        datalength,
                                            IFR_Length*       lengthindicator,
                                            IFR_Bool       terminate,
                                            IFR_ConnectionItem &clink);

    
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

    
    virtual IFR_Retcode translateBinaryOutput(IFRPacket_DataPart& datapart,
                                              char           *data,
                                              IFR_Length      datalength,
                                              IFR_Length*     lengthindicator,
                                              IFR_ConnectionItem &clink);
    
    virtual IFR_Retcode appendBinaryOutput(IFRPacket_DataPart& datapart,
                                           char               *data,
                                           IFR_Length            datalength,
                                           IFR_Length*           lengthindicator,
                                           IFR_ConnectionItem& clink,
                                           IFR_Length         &dataoffset,
                                           IFR_Length         &offset,
                                           IFRConversion_Getval *getval);
    
private:
    /** 
     * This method will convert the data from the given numeric datapart to 
     * an character buffer pointed to <code>data</code> honoring the encoding
     * of the outputbuffer.
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

    IFR_Retcode translateCharacterOutput(IFRPacket_DataPart& datapart,
                                         char           *data,
                                         IFR_Length        datalength,
                                         IFR_Length*       lengthindicator,
                                         IFR_ConnectionItem &clink,
                                         IFR_StringEncoding encoding,
                                         IFR_Bool terminate);

private:
    IFR_Bool m_noscientific; // Scientific rep ignored?
    IFR_Bool m_truncate;     // Truncate numbers instead of rounding.
    
};

#endif //IFRCONVERSION_NUMERICCONVERTER_H
