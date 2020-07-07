/*!
  @file           IFRConversion_Putval.h
  @author         D030044
  @ingroup        IFR_DataConv
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
#ifndef IFRCONVERSION_PUTVAL_H
#define IFRCONVERSION_PUTVAL_H

#include "Interfaces/Runtime/Packet/IFRPacket_Part.h"
#include "Interfaces/Runtime/Packet/IFRPacket_LongDescriptor.h"
#include "Interfaces/Runtime/IFR_ConnectionItem.h"
#include "Interfaces/Runtime/IFR_Parameter.h"

IFR_BEGIN_NAMESPACE

/**
 * @ingroup IFR_DataConv
 * @brief Base class for long data conversion.
 * 
 * This class handles the management of the LONG descriptor
 * and the corresponding data for insert operations,
 * and is responsible for putting the data into the data part.
 */
class IFRConversion_Putval
{
public:
    /**
     * @brief Default Constructor. 
     * Creates an empty putval object.
     */
    IFRConversion_Putval ();

    /**
     * @brief Constructor. 
     * @param column     The column.
     * @param row        The row.
     * @param shortinfo  The short field info.
     * @param hosttype   The host type, as supplied by the binding operation.
     * @param data       The data pointer, as it was supplied in the
     *                   binding operation.
     * @param datalength The data length, as supplied in the binding
     *                   operation.
     * @param lengthindicator The length or indicator value, as supplied
     *                   by the binding operation.
     * @param clink      Link to the connection item for error reporting.    
     */
    IFRConversion_Putval (IFR_Int2 column,
                          IFR_Int4 row,
                          IFR_ShortInfo& shortinfo,
                          IFR_HostType hosttype,
                          char    * data,
                          IFR_Length  datalength,
                          IFR_Length* lengthindicator,
                          IFR_ConnectionItem& clink,
                          IFR_Bool bin2hex,
                          IFR_StringEncoding packetEncoding);

    /**
     * Destructor.
     */
    ~IFRConversion_Putval();
    
    /**
     * Write the descriptor into the data part.
     * @param dataPart The destination data part.
     */
    void putDescriptor (IFRPacket_LongDataPart& dataPart);

    /**
     * Write the descriptor into the data part. The stored short field info
     * is used to determine the position.
     * @param dataPart The destination data part.
     */
    void putDescriptor (IFRPacket_DataPart& dataPart); 

    
    /**
     * Transfers the stream into the data part.
     * @param dataPart The destination data part.
     * @return <code>IFR_OK</code> if the stream was fully inserted. 
     *         <code>IFR_DATA_TRUNC</code> if the stream is not yet completely
     *         written.
     */
    IFR_Retcode transferStream(IFRPacket_DataPart& dataPart);
    
    /**
     * Transfers a stream into a data part (or long data part).
     * @param datapart The destination data part.
     * @param data The data to insert.
     * @param datalength The maximum data length.
     * @param lengthindicator The length or indicator value.
     * @param offset The amount of data already written for this LONG in this packet.
     */
    IFR_Retcode transferStream(IFRPacket_DataPart& datapart,
                               char               *data,
                               IFR_Length            datalength,
                               IFR_Length           *lengthindicator,
                               IFR_Bool              ascii7bit,
                               IFR_Length         &offset);

    /**
     * Transfers the stream into a longdata part. Here,
     * the descriptor is putted into the part, followed by 
     * the LONG data. 
     * @param longdatapart The longdata part.
     * @param empty Flag to be set when an empty stream is found.
     * @return <code>IFR_OK</code> if the stream was fully inserted,
     *         <code>IFR_DATA_TRUNC</code> if the stream was not yet
     *         completely inserted.
     */
    IFR_Retcode transferStream(IFRPacket_LongDataPart& longdatapart, IFR_Bool& empty);
    
    /**
     * Updates the descriptor with the new value.
     * @param descriptor Pointer to the LONG descriptor
     */
    void updateDescriptor(char *descriptor);

    /**
     * Retrieve whether the data is completely read.
     * @return <code>true</code> if there is data set (no DATA AT EXECUTE) and
     *         the data is completely written.
     */
    inline IFR_Bool atEnd() const
    {
        if(m_atend) {
            return true; 
        } else {
            return (m_dataend != 0 &&
                    m_datapos == m_dataend);
        }
    }
    
    /**
     * Return true if the data describes a parameter 
     * that is delivered at execution time.
     */
    inline IFR_Bool hasDataAtExecute() const
    {
        return IFR_Parameter::hasDataAtExecute(m_lengthindicator);
    }

    inline void setStreamValMode(IFRPacket_LongDescriptor::ValMode valmode)
    {
        IFRPacket_LongDescriptor::setStreamValMode(m_longdesc_in_dp, valmode);
    }
    
    inline IFR_Int2 getValIndex() const
    {
        return m_longdesc.valind;
    }

    void setValIndex(IFR_Int2 valindex);
    
    /**
     * Retrieve the column.
     */
    inline IFR_Int2 getColumn() const 
    {
        return m_column;
    }

    /**
     * Retrieve the row.
     */
    inline IFR_Int4 getRow() const 
    {
        return m_row;
    }
    
    IFR_Retcode putCloseDescriptor(IFRPacket_LongDataPart& longdata, IFR_Bool appendLastPutval);
    
    inline IFR_Int4 getBufpos() 
    {
        return m_shortinfo.pos.bufpos;
    }

private:    
    IFR_Int4                  m_column;
    IFR_Int2                  m_row;
    
    IFR_ShortInfo             m_shortinfo;       //!< Short field info copied.
    IFRPacket_LongDescriptor  m_longdesc;        //!< The descriptor of this LONG data.
    IFR_HostType              m_hosttype;        //!< The host type at the time of the binding operation.
    char                     *m_data;            //!< The data pointer at the time of the binding operation.
    IFR_Length                m_datalength;      //!< The length of the data.
    IFR_Length               *m_lengthindicator; //!< Pointer to the indicator value.
    IFR_ConnectionItem       *m_clink;          //!< Link for error reporting.
    char                     *m_dataend;        //!< If computed, points to one after the last valid value of data.
    char                     *m_datapos;        //!< If computed, points to the current read position in the data.
    char                     *m_longdesc_in_dp; //!< Pointer to the long descriptor in the data part, for later 
                                                //!< updates after an initial <code>putDescriptor</code> call.
    IFR_Bool                  m_bin2hex;
    IFR_Bool                  m_atend;          //!< Extra flag, whether this putval has been putted completely.
    IFR_StringEncoding        m_encoding;
    IFR_StringEncoding        m_sourceencoding; 

    char                     *m_dyn_data;      //!< Dynamic data in case the long value was converted from
                                               //!< an integer/float/date
	 /**
    * Computes the derived <code>m_dataXXX</code> members.
    * @return <code>IFR_OK</code> on succes, <code>IFR_NOT_OK</code> if the 
    *         stored lengthindicator has a wrong value.
    */                                                
    IFR_Retcode computeDataEnd();

    /**
     * Converts the data to string for later handling. This is done for all
     * integral types (numbers, time types, ...)
     */
    void convertDataToString();
};

IFR_TraceStream& operator << (IFR_TraceStream& s, const IFRConversion_Putval& putval);

IFR_END_NAMESPACE

#endif // IFRCONVERSION_PUTVAL_H

