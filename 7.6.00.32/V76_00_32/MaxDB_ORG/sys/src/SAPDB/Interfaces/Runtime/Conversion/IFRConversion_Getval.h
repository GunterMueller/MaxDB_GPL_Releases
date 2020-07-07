/*!
  @file           IFRConversion_Getval.h
  @author         D030044
  @ingroup        IFR_ConvData
  @brief          Implements a class for long data conversion
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
#ifndef IFRCONVERSION_GETVAL_H
#define IFRCONVERSION_GETVAL_H

#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/Packet/IFRPacket_Part.h"
#include "Interfaces/Runtime/Packet/IFRPacket_LongDescriptor.h"
#include "Interfaces/Runtime/IFR_ConnectionItem.h"
#include "Interfaces/Runtime/IFR_Parameter.h"

IFR_BEGIN_NAMESPACE

// Data part buffer for data kept.
class IFRConversion_GetvalDataPart
    : public IFRPacket_DataPart
{
public:
    IFRConversion_GetvalDataPart(IFR_Byte *copiedbuffer,
                                 SAPDBMem_IRawAllocator& _allocator,
                                 IFR_StringEncoding encoding)
    :IFRPacket_DataPart((tsp1_part*)copiedbuffer, encoding),
     allocator(_allocator)
    {
    }

    ~IFRConversion_GetvalDataPart()
    {
        IFR_Byte *to_del=(IFR_Byte*)GetRawPart();
        if(to_del) {
            IFR_ALLOCATOR(allocator).Deallocate(to_del);
        }
    }

public:
    SAPDBMem_IRawAllocator& allocator;
};

class IFRConversion_Getval;

/**
 * Structure needed for remembering positions when reading from LOB
 * objects.
 */
struct IFRConversion_GetvalInfo
{
    inline IFRConversion_GetvalInfo()
    :converter(0),
     getval(0),
     offset(0),
     datalength(0)
    {}

    inline IFRConversion_GetvalInfo(IFRConversion_Converter *c,
                                    IFRConversion_Getval    *g,
                                    IFR_Length               o,
                                    IFR_Length               l)
    :converter(c),
     getval(g),
     offset(o),
     datalength(l)
    {}

    IFRConversion_Converter *converter;
    IFRConversion_Getval    *getval;
    IFR_Length              offset;
    IFR_Length              datalength;
};

/**
 * @ingroup IFR_DataConv
 * @brief Base class for long data conversion.
 *
 * This class handles the management of the LONG descriptor
 * and the corresponding data for insert operations,
 * and is responsible for putting the data into the data part.
 */
class IFRConversion_Getval
{
public:
    enum Status {
        Normal_C,
        NeedsClose_C,
        Closed_C
    };

    /**
     * @brief Constructor.
     * @param column The column.
     * @param row    The row.
     * @param length The length.
     * @param iolength The io length.
     * @param definedbyte The defined byte.
     * @param clink  Link to the connection item for error reporting.
     */
    IFRConversion_Getval(IFR_Int2            column,
                         IFR_Int4            row,
                         IFR_Int2            length,
                         IFR_Int2            iolength,
                         IFR_Int1            definedbyte,
                         IFR_SQLType         datatype,
                         IFR_ConnectionItem& clink,
                         IFR_Bool bin2hex,
                         IFR_StringEncoding packetEncoding);

    /**
     * Destructor
     */
    ~IFRConversion_Getval();

    /**
     * Write the descriptor into the data part.
     * @param dataPart The destination data part.
     */
    void putDescriptor (IFRPacket_DataPart& dataPart);

    /**
     * Write the descriptor into the data part for a request to determine
     * the length of the LONG.
     * @param dataPart The destination data part.
     */
    void putDescriptorForGetLength(IFRPacket_DataPart& dataPart);

    /**
     * Copies the descriptor from the data part and updates internal
     * flags and values.
     * @param dataPart The destination data part.
     * @param position The position of the 1st data byte (the defined byte is
     *        inserted at <code>position - 1</code>).
     */
    void updateDescriptor (IFRPacket_DataPart& dataPart,
                           IFR_Int4 position);

    /**
     * Updates the descriptor and updates internal flags and values.
     * @param descriptor Pointer to the LONG descriptor
     */
    void updateDescriptor(char *descriptor);

    /**
     * Sets infoset of the long descriptor to 'no close'.
     */
    void setNoClose();

    /**
     * Retrieve whether the 'keep' flag is set.
     */
    IFR_Bool isNoClose() const;


    /**
     * Transfers integral data.
     */
    IFR_Retcode
    transferIntegral(IFRPacket_DataPart& datapart,
                     IFR_HostType hosttype,
                     void *data,
                     IFR_Length *lengthindicator);
                                           
    
    /**
     * Transfers a stream from a data part (or long data part).
     * @param datapart The source data part.
     * @param data The data to insert.
     * @param datalength The maximum data length.
     * @param lengthindicator The length or indicator value.
     * @param offset The start offset within the long value. The number of
     *   bytes transferred are added to this value.
     * @param encoding The requested encoding.
     * @param terminate True if result is to be terminate with a terminator.
     * @return @c IFR_OK on success, if all data has been read,
     *         @c IFR_NEED_DATA if less then datalength bytes transfered, and
     *            there is more data available.
     */
    IFR_Retcode transferStream(IFRPacket_DataPart& datapart,
                               char       *data,
                               IFR_Length  datalength,
                               IFR_Length *lengthindicator,
                               IFR_Length &offset,
                               IFR_StringEncoding encoding,
                               IFR_Bool terminate,
                               IFR_Bool ascii7bit);

    /**
     * Access the LONG descriptor.
     * @return The LONG descriptor.
     */
    const IFRPacket_LongDescriptor& getLongDescriptor() { return m_longdesc; }

    /**
     * Retrieve @c valind field of the LONG descriptor.
     * @return valind (start counting at 0)
     */
    IFR_Int2 getValIndex();

    /**
     * Sets the @c valind field of the LONG descriptor.
     */
    void setValIndex(IFR_Int2 valindex);

    /**
     * Gets the encoding of the data.
     * @return The encoding (derived from SQL type)
     */
    IFR_StringEncoding getEncoding();

    /**
     * Gets the column.
     * @return The column value.
     */
    inline IFR_Int2 getColumn() const { return m_column; }

    /**
     * Gets the row.
     * @return The row value.
     */
    inline IFR_Int4 getRow() const { return m_row; }

    /**
     * Gets the read offset, i.e. the current position 'within' the long to
     * read from.
     */
    inline IFR_Int4 getReadOffset() const { return m_readoffset; }

    /**
     * Sets the read offset. The read offset is 1-based.
     * @param readoffset The read offset.
     */
    inline void setReadOffset(IFR_Int4 readoffset) { m_readoffset = readoffset; }

    /**
     * Returns the last stored LONG data.
     * @return Pointer to stored long data.
     */
    IFRPacket_DataPart *getLongData() { return m_longdata; }

    /**
     * Sets the long data that is kept in the GETVAL instance.
     * @param longdatapart The long data part that is copied and stored.
     * @param allocator Allocator used to allocate the buffer for the stored part.
     * @param memory_ok Memory allocation check flag.
     */
    void setLongData(IFRPacket_DataPart& longdatapart,
                     SAPDBMem_IRawAllocator& allocator,
                     IFR_Bool& memory_ok);

    /**
     * Clears the stored long data.
     */
    void clearLongData();

    /**
     * Closes the GETVAL, hereby adding the descriptor to the garbage list of
     * the connection.
     */
    IFR_Retcode close();


    inline void setBinaryToHex(IFR_Bool value)
    {
        m_bin2hex = value;
    }

    inline IFR_Bool isBinaryToHex() const
    {
        return m_bin2hex;
    }

    inline IFR_Length getMaximumOutputLength()
    {
        return m_maxlength;
    }

    inline IFR_Bool isClosed() const { return m_status == Closed_C; }

    /**
     * Retrieve whether the getval has binary data.
     */
    inline IFR_Bool isBinary() const
    {
        return m_datatype == IFR_SQLTYPE_CHB
            || m_datatype == IFR_SQLTYPE_STRB
            || m_datatype == IFR_SQLTYPE_STRDB
            || m_datatype == IFR_SQLTYPE_LONGB
            || m_datatype == IFR_SQLTYPE_LONGDB
            || m_datatype == IFR_SQLTYPE_VARCHARB;
    }

    inline IFR_SQLType getDatatype() const
    {
        return m_datatype;
    }

private:
    IFRConversion_Getval(const IFRConversion_Getval& dontcopy);
    IFRConversion_Getval& operator =(const IFRConversion_Getval& getval);

    /**
     * Updates the long descriptor so that next time data from
     * position @c start and for @c length bytes is requested.
     * Also the 'dirty' flag is set.
     * @param start The start position (1-based)
     * @param length The length in bytes.
     * @param encoding Pointer to encoding, if this is not null, it will be checked for
     *   computing the right data length for the length.
     */
    void requestLongChunk(IFR_Int4 start, IFR_Int4 length, IFR_StringEncoding* encoding);

    /**
     * Picks up the long data part if necessary and returns the original return code
     * on success.
     * @param datapart The datapart to possible be picked up.
     * @param originalRetcode The original return code.
     */
    IFR_Retcode checkStoreLongData(IFRPacket_DataPart& datapart,
                                   IFR_Retcode originalRetcode);

    IFR_Int2                  m_column;       //!< The column.
    IFR_Int4                  m_row;          //!< The row.
    IFR_Int4                  m_maxlength;    //!< The length in bytes of this LONG data.
    IFR_Int4                  m_lastinternpos;//!< The last 'internpos'
    IFRPacket_LongDescriptor  m_longdesc;     //!< The descriptor of this LONG data.
    IFR_ConnectionItem       *m_clink;        //!< Connection link (tracing, error reporting).

    IFR_Int2                  m_length;       //!< Logical length.
    IFR_Int2                  m_iolength;     //!< Physical length.
    IFR_Int1                  m_definedbyte;  //!< Value of the 'defined byte'.
    IFR_SQLType               m_datatype;     //!< The SQL data type of the LONG.
    IFR_Bool                  m_dirty;        //!< Flag that is set if the long descriptor
                                              //!< is updated for the next 'getval'.
    IFR_Int4                  m_readoffset;   //!< The offset within the LONG value (1-based) in bytes
                                              //!< from which data is read.
    IFR_Int1                  m_status;       //!< Set if the LONG is open and requires a GETVAL having
                                              //!< the close flag set.
    IFR_StringEncoding        m_encoding;     //!< Encoding, derived from SQL type.
    IFRConversion_GetvalDataPart *m_longdata; //!< A copy of the last LONG data available.

    IFR_Bool                  m_bin2hex;      //!< Whether to make the 'binary to hex dump' conversion.
};

IFR_TraceStream& operator << (IFR_TraceStream& s, const IFRConversion_Getval& getval);

IFR_END_NAMESPACE

#endif // IFRCONVERSION_GETVAL_H

