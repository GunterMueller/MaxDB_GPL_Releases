/*!
  @file           IFR_Parameter.h
  @author         D039759
  @ingroup        IFR_DataConv
  @brief          Handling of parameters and hostvars
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
#ifndef IFR_PARAMETER_H
#define IFR_PARAMETER_H

#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/IFR_Datatypes.h"
#include "Interfaces/Runtime/IFR_String.h"
#include "Interfaces/Runtime/IFR_LOB.h"

/**
 * @ingroup IFR_Statement
 * @brief A parameter. 
 *
 * A parameter is used to <i>bind</i> values for
 * input and/or output. It is used in prepared statements and 
 * for result sets to bind columns. 
 * 
 * A parameter may be bound normally or <i>by address</i>. This has affect
 * on the pointer that refers to the data - if bound by address the data
 * pointer points to a pointer to the data, otherwise it points directly
 * to the data. 
 */
class IFR_Parameter 

{
public:
    /**
     * The default constructor creates an uninitialized parameter.
     */
    inline IFR_Parameter() 
    :m_hosttype(IFR_HOSTTYPE_PARAMETER_NOTSET),
     m_data(0),
     m_lengthindicator(0),
     m_byteslength(0),
     m_addrbound(false),
     m_terminate(true),
     m_posindicator(0)
    {}
    
    /**
     * Constructor. Creates a parameter for a specfic host type.
     * @param hosttype The host type, one of the <code>IFR_HostType</code> values.
     * @param terminate Specifies that the output buffer should be finished
     *        with a C style zero terminator. The <code>Terminated</code> flag 
     *        works only for the hostvar type character (ASCII, UCS2 or UTF8).
     * @param data Pointer to the data.
     * @param lengthindicator Pointer to input length or output status indicator.
     * @param byteslength Length of the data, if necessary.
     * @param boundbyaddr Whether the data is bound by address, not directly.
     * @param posindicator Pointer to start position in bytes for reading of data.
     */
    inline IFR_Parameter (IFR_HostType hosttype, 
                          IFR_Bool terminate,
                          void *data=0,
                          IFR_Length *lengthindicator=0, 
                          IFR_Length byteslength=-1,
                          IFR_Length *posindicator=0,
                          IFR_Bool boundbyaddr=false)
    :m_hosttype(hosttype),
     m_data((unsigned char *)data),
     m_lengthindicator(lengthindicator),
     m_byteslength(byteslength),
     m_addrbound(boundbyaddr),
     m_terminate(terminate),
     m_posindicator(posindicator),
     m_digits(-1),
     m_fraction(-1)
    {
      switch (hosttype){
        case (IFR_HOSTTYPE_DECIMAL):{
          if( lengthindicator){
            m_digits   = SQLDBC_DECIMAL_DIGITS  (*lengthindicator);
            m_fraction = SQLDBC_DECIMAL_FRACTION(*lengthindicator);
          }
          break;
        }
        case (IFR_HOSTTYPE_OMS_PACKED_8_3):{
            m_digits   = 8;
            m_fraction = 3;
            break;
        }
        case (IFR_HOSTTYPE_OMS_PACKED_15_3):{
            m_digits   = 15;
            m_fraction = 3;
            break;
        }
      }
    }

    inline void setDecimalDigitsAndFraction(IFR_Length *lengthindicator){
          if( lengthindicator){
            m_digits   = SQLDBC_DECIMAL_DIGITS  (*lengthindicator);
            m_fraction = SQLDBC_DECIMAL_FRACTION(*lengthindicator);
          } else {
            m_digits   = -1;
            m_fraction = -1;
          }
    }
   
    /**
     * Determine whether a parameter is not set.
     * @return @c true if the parameter is not set and is here
     *  only because parameters are stored as a vector.
     */
    inline IFR_Bool isNotSet() const 
    {
        return m_hosttype == IFR_HOSTTYPE_PARAMETER_NOTSET;
    }

    /**
     * Get a pointer to the data.
     * @return A pointer to the data, which may be <code>0</code>.
     */
    inline const unsigned char *data() const
    {
        if(m_addrbound && m_data) {
            return * (const unsigned char **)m_data;
        } else {
            return m_data;
        }
    }

    /**
     * Get a pointer to the data.
     * @param offset The array offset of the data.
     * @param rowsize The row size for row-wise binding (not used if 0).
     * @return A pointer to the data, which may be <code>0</code>.
     */
    inline const unsigned char *data(IFR_size_t offset, IFR_size_t rowsize=0, unsigned char *addr = 0)
    {
        addr = (addr) ? addr : m_data;
        if(m_addrbound) {
            return * (const unsigned char **)(addr + rawSize(offset,  rowsize, sizeof(unsigned char *)));
        } else {
            return (unsigned char*) (addr + rawSize(offset, rowsize, getBytesLength()));
        }
    }

    /**
     * Gets the length of a single data item in bytes.
     * @return the size of a single data item, which may
     *   be -1 if not set.
     */
    const IFR_Length getBytesLength() const;
    
    /**
     * Get the host type.
     * @return The host type of the bound parameter.
     */
    inline IFR_HostType getHostType() const
    {
        return m_hosttype;
    }
    
    /**
     * Check if the parameter should be C style terminated.
     * @return The true if the parameter should be terminated with one or more
     *         zeros honoring the hosttype. Only charachter data can terminated
     *         with zeros.
     */
    inline IFR_Bool shouldTerminated() const
    {
        return m_terminate;
    }
    
    /**
     * Returns true if the host type is an integral type (i.e. no character or
     * byte buffer).
     * @return <code>true</code> if the host type is not a byte, ASCII character,
     *   UCS2 character or UTF8 character buffer.
     */
    inline IFR_Bool isIntegral() const
    {
        return m_hosttype != IFR_HOSTTYPE_BINARY &&
            m_hosttype != IFR_HOSTTYPE_ASCII &&
            m_hosttype != IFR_HOSTTYPE_UCS2_SWAPPED &&
            m_hosttype != IFR_HOSTTYPE_UCS2 &&
            m_hosttype != IFR_HOSTTYPE_UTF8;
    }
    
    /**
     * Retrieve whether the paremeter is a LOB.
     */
    inline IFR_Bool isLOB() const
    {
        return m_hosttype == IFR_HOSTTYPE_BLOB   ||
            m_hosttype == IFR_HOSTTYPE_ASCII_LOB ||
            m_hosttype == IFR_HOSTTYPE_UCS2_LOB  ||
            m_hosttype == IFR_HOSTTYPE_UCS2_SWAPPED_LOB ||
            m_hosttype == IFR_HOSTTYPE_UTF8_LOB;
    }
    
    /**
     * Retrieve whether the parameter is a binary parameter value.
     */
    inline IFR_Bool isBinary() const 
    {
        return m_hosttype == IFR_HOSTTYPE_BLOB 
            || m_hosttype == IFR_HOSTTYPE_BINARY;
    }
    
    /**
     * Checks whether this is a NULL value.
     * @return <code>true</code> if the indicator value 
     *         implies a NULL value.
     */
    IFR_Bool isNullInput() const;
        
    /**
     * Access the data as 1 byte integer.
     * @param offset The array offset of the data.
     * @param rowsize The row size for row-wise binding (not used if 0).
     * @return The data as 1 byte integer value.
     */
    inline IFR_Int1& asInt1(IFR_Int4 offset=0, IFR_size_t rowsize = 0, unsigned char *addr = 0)
    {
        addr = (addr) ? addr : m_data;
        if(m_addrbound) {
            return * * (IFR_Int1 **)(addr + rawSize(offset,  rowsize, sizeof(IFR_Int1 *)));
        } else {
            return * (IFR_Int1 *)(addr + rawSize(offset,  rowsize, sizeof(IFR_Int1)));
        }
    }

    /**
     * Access the data as 1 byte unsigned integer.
     * @param offset The array offset of the data.
     * @param rowsize The row size for row-wise binding (not used if 0).
     * @return The data as 1 byte unsigned integer value.
     */
    inline IFR_UInt1& asUInt1(IFR_Int4 offset=0, IFR_size_t rowsize=0, unsigned char *addr = 0)
    {
        addr = (addr) ? addr : m_data;
        if(m_addrbound) {
            return * * (IFR_UInt1 **)(addr + rawSize(offset,  rowsize, sizeof(IFR_UInt1 *)));
        } else {
            return * (IFR_UInt1 *)(addr + rawSize(offset,  rowsize, sizeof(IFR_UInt1)));
        }
    }

    /**
     * Access the data as 2 byte integer.
     * @param offset The array offset of the data.
     * @param rowsize The row size for row-wise binding (not used if 0).
     * @return The data as 2 byte integer value.
     */
    inline IFR_Int2& asInt2(IFR_Int4 offset=0, IFR_size_t rowsize=0, unsigned char *addr = 0)
    {
        addr = (addr) ? addr : m_data;
        if(m_addrbound) {
            return * * (IFR_Int2 **)(addr + rawSize(offset,  rowsize, sizeof(IFR_Int2 *)));
        } else {
            return * (IFR_Int2 *)(addr + rawSize(offset,  rowsize, sizeof(IFR_Int2)));
        }
    }
    
    /**
     * Access the data as 2 byte unsigned integer.
     * @param offset The array offset of the data.
     * @param rowsize The row size for row-wise binding (not used if 0).
     * @return The data as 2 byte unsigned integer value.
     */
    inline IFR_UInt2& asUInt2(IFR_Int4 offset=0, IFR_size_t rowsize=0, unsigned char *addr = 0)
    {
        addr = (addr) ? addr : m_data;
        if(m_addrbound) {
            return * * (IFR_UInt2 **)(addr + rawSize(offset,  rowsize, sizeof(IFR_UInt2 *)));
        } else {
            return * (IFR_UInt2 *)(addr + rawSize(offset,  rowsize, sizeof(IFR_UInt2)));
        }
    }


    /**
     * Access the data as 4 byte integer.
     * @param offset The array offset of the data.
     * @param rowsize The row size for row-wise binding (not used if 0).
     * @return The data as 4 byte integer value.
     */
    inline IFR_Int4& asInt4(IFR_Int4 offset=0, IFR_size_t rowsize=0, unsigned char *addr = 0)
    {
        addr = (addr) ? addr : m_data;
        if(m_addrbound) {
            return * * (IFR_Int4 **)(addr + rawSize(offset,  rowsize, sizeof(IFR_Int4 *)));
        } else {
            return * (IFR_Int4 *)(addr + rawSize(offset,  rowsize, sizeof(IFR_Int4)));
        }
    }

    /**
     * Access the data as 4 byte unsigned integer.
     * @param offset The array offset of the data.
     * @param rowsize The row size for row-wise binding (not used if 0).
     * @return The data as 4 byte unsigned integer value.
     */
    inline IFR_UInt4& asUInt4(IFR_Int4 offset=0, IFR_size_t rowsize=0, unsigned char *addr = 0)
    {
        addr = (addr) ? addr : m_data;
        if(m_addrbound) {
            return * * (IFR_UInt4 **)(addr + rawSize(offset,  rowsize, sizeof(IFR_UInt4 *)));
        } else {
            return * (IFR_UInt4 *)(addr + rawSize(offset,  rowsize, sizeof(IFR_UInt4)));
        }
    }

    /**
     * Access the data as 8 byte integer.
     * @param offset The array offset of the data.
     * @param rowsize The row size for row-wise binding (not used if 0).
     * @return The data as 8 byte integer value.
     */
    inline IFR_Int8& asInt8(IFR_Int4 offset=0, IFR_size_t rowsize=0, unsigned char *addr = 0)
    {
        addr = (addr) ? addr : m_data;
        if(m_addrbound) {
            return * * (IFR_Int8 **)(addr + rawSize(offset,  rowsize, sizeof(IFR_Int8 *)));
        } else {
            return * (IFR_Int8 *)(addr + rawSize(offset,  rowsize, sizeof(IFR_Int8)));
        }
    }

    /**
     * Access the data as 8 byte unsigned integer.
     * @param offset The array offset of the data.
     * @param rowsize The row size for row-wise binding (not used if 0).
     * @return The data as 8 byte unsigned integer value.
     */
    inline IFR_UInt8& asUInt8(IFR_Int offset=0, IFR_size_t rowsize=0, unsigned char *addr = 0)
    {
        addr = (addr) ? addr : m_data;
        if(m_addrbound) {
            return * * (IFR_UInt8 **)(addr + rawSize(offset,  rowsize, sizeof(IFR_UInt8 *)));
        } else {
            return * (IFR_UInt8 *)(addr + rawSize(offset,  rowsize, sizeof(IFR_UInt8)));
        }
    }

    /**
     * Access the data as <code>SQL_DATE_STRUCT</code>.
     * @param offset The array offset of the data..
     * @param rowsize The row size for row-wise binding (not used if 0).
     * @return The data as <code>SQL_DATE_STRUCT</code>.
     */
    inline SQL_DATE_STRUCT& asSQLDate(IFR_Int4 offset=0, IFR_size_t rowsize=0, unsigned char *addr = 0)
    {
        addr = (addr) ? addr : m_data;
        if(m_addrbound) {
            return * * (SQL_DATE_STRUCT **)(addr + rawSize(offset,  rowsize, sizeof(SQL_DATE_STRUCT *)));
        } else {
            return * (SQL_DATE_STRUCT *)(addr + rawSize(offset,  rowsize, sizeof(SQL_DATE_STRUCT)));
        }
    }

    /**
     * Access the data as <code>SQL_TIME_STRUCT</code>.
     * @param offset The array offset of the data..
     * @param rowsize The row size for row-wise binding (not used if 0).
     * @return The data as <code>SQL_TIME_STRUCT</code>.
     */
    inline SQL_TIME_STRUCT& asSQLTime(IFR_Int4 offset=0, IFR_size_t rowsize=0, unsigned char *addr = 0)
    {
        addr = (addr) ? addr : m_data;
        if(m_addrbound) {
            return * * (SQL_TIME_STRUCT **)(addr + rawSize(offset,  rowsize, sizeof(SQL_TIME_STRUCT *)));
        } else {
            return * (SQL_TIME_STRUCT *)(addr + rawSize(offset,  rowsize, sizeof(SQL_TIME_STRUCT)));
        }
    }

    /**
     * Access the data as <code>SQL_TIMESTAMP_STRUCT</code>.
     * @param offset The array offset of the data..
     * @param rowsize The row size for row-wise binding (not used if 0).
     * @return The data as <code>SQL_TIMESTAMP_STRUCT</code>.
     */
    inline SQL_TIMESTAMP_STRUCT& asSQLTimestamp(IFR_Int4 offset=0, IFR_size_t rowsize=0, unsigned char *addr = 0)
    {
        addr = (addr) ? addr : m_data;
        if(m_addrbound) {
            return * * (SQL_TIMESTAMP_STRUCT **)(addr + rawSize(offset,  rowsize, sizeof(SQL_TIMESTAMP_STRUCT *)));
        } else {
            return * (SQL_TIMESTAMP_STRUCT *)(addr + rawSize(offset,  rowsize, sizeof(SQL_TIMESTAMP_STRUCT)));
        }
    }
    
    /**
     * Access the data as SQL_NUMERIC_STRUCT.
     * @param offset The array offset of the data.
     * @param rowsize The row size for row-wise binding (not used if 0).
     * @return The data as <code>SQL_NUMERIC_STRUCT</code>.
     */
    inline SQL_NUMERIC_STRUCT& asSQLNumeric(IFR_Int4 offset=0, IFR_size_t rowsize=0, unsigned char *addr = 0)
    {
        addr = (addr) ? addr : m_data;
        if(m_addrbound) {
            return * * (SQL_NUMERIC_STRUCT **)(addr + rawSize(offset,  rowsize, sizeof(SQL_NUMERIC_STRUCT *)));
        } else {
            return * (SQL_NUMERIC_STRUCT *)(addr + rawSize(offset,  rowsize, sizeof(SQL_NUMERIC_STRUCT)));
        }
    }

    
    /**
     * Access the data as GUID.
     * @param offset The array offset of the data.
     * @param rowsize The row size for row-wise binding (not used if 0).
     * @return The data as double value.
     */
    inline GUID& asGUID(IFR_Int4 offset=0, IFR_size_t rowsize=0, unsigned char *addr = 0)
    {
        addr = (addr) ? addr : m_data;
        if(m_addrbound) {
            return * * (GUID **)(addr + rawSize(offset,  rowsize, sizeof(GUID *)));
        } else {
            return * (GUID *)(addr + rawSize(offset,  rowsize, sizeof(GUID)));
        }
    
    }
    
    /**
     * Access the data as LOB data.
     * @param offset The array offset of the data.
     * @param rowsize The row size for rowwise binding.
     * @return The data as @c IFR_LOBData reference
     */
    inline IFR_LOBData& asLOBData(IFR_Int4 offset=0, IFR_size_t rowsize=0, unsigned char *addr=0) 
    {
        addr = (addr) ? addr : m_data;
        if(m_addrbound) {
            return * * (IFR_LOBData **)(addr + rawSize(offset,  rowsize, sizeof(IFR_LOBData *)));
        } else {
            return * (IFR_LOBData *)(addr + rawSize(offset,  rowsize, sizeof(IFR_LOBData)));
        }        
    }

    /**
     * Access the data in the buffer as character array.
     * @param offset The array offset of the data, the length
     *        in bytes property is used for the array calculation,
     *        if the parameter is not bound by address.
     * @param rowsize The row size for row-wise binding (not used if 0).
     * @return A char pointer to the data.
     */
    inline char * asChar(IFR_Int4 offset=0, IFR_size_t rowsize=0, unsigned char *addr = 0)
    {
        addr = (addr) ? addr : m_data;
        if(m_addrbound) {
            return * (char **)(addr + rawSize(offset,  rowsize, sizeof(char *)));
        } else {
            return (char*) (addr + rawSize(offset,  rowsize, getBytesLength()));
        }
    }
    
    

    /**
     * Access the data as <code>double</code>.
     * @param offset The array offset of the data..
     * @param rowsize The row size for row-wise binding (not used if 0).
     * @return The data as double value.
     */
    inline double& asDouble(IFR_Int4 offset=0, IFR_size_t rowsize=0, unsigned char *addr = 0)
    {
        addr = (addr) ? addr : m_data;
        if(m_addrbound) {
            return * * (double **)(addr + rawSize(offset,  rowsize, sizeof(double *)));
        } else {
            return * (double *)(addr + rawSize(offset,  rowsize, sizeof(double)));
        }
    }

    /**
     * Access the data as <code>double</code>
     * @param offset The array offset of the data.
     * @param rowsize The row size for row-wise binding (not used if 0).
     * @return The data as float value.
     */
    inline float& asFloat(IFR_Int4 offset=0, IFR_size_t rowsize=0, unsigned char *addr = 0)
    {
        addr = (addr) ? addr : m_data;
        if(m_addrbound) {
            return * * (float **)(addr + rawSize(offset,  rowsize, sizeof(float *)));
        } else {
            return * (float *)(addr + rawSize(offset,  rowsize, sizeof(float)));
        }
    }

    /**
     * Gets the length or indicator.
     * @param offset Array offset for array parameters.
     * @param rowsize The row size for row-wise binding (not used if 0).
     * @return pointer to length/indicator at offset (which may be 0)
     */
    inline IFR_Length* getLengthIndicator(IFR_Int4 offset=0, IFR_size_t rowsize=0) 
    {
        if(m_lengthindicator == 0) {
            return 0;
        } else {
            if(rowsize) {
                return (IFR_Length*) (((char*)m_lengthindicator)+offset*rowsize);
            } else {
                return m_lengthindicator + offset;
            } 
        }
    }

    /**
     * Gets the start position in bytes for reading of non-integral data.
     * @param offset Array offset for array parameters.
     * @param rowsize The row size for row-wise binding (not used if 0).
     * @return The pointer to the start position (1-based), or 0 if none set.
     */
    inline const IFR_Length* getPosIndicator ()
    {
        return m_posindicator;
    }

    /**
     * Gets the name of the host type.
     * @return a static string with the name of the host type.
     */
    const char *getHosttypeString() const;
    
    /**
     * Gets the database type that is likely to match the host type.
     * This is used in case where the database needs help in determining
     * the parameter format.
     * @return The SQL type that is preferred for a given host type.
     */
    IFR_SQLType getPreferredSQLType() const;

    /**
     * Gets the decimal precision that is likely to match the host type.
     * This is used in case where the database needs help in determining
     * the parameter format.
     * @return The precision that is most likely to match 
     *         the parameter format.
     */
    inline IFR_Int1 getPreferredPrecision() const
    {
        return 0;
    }
    
    /**
     * Gets the length that is likely to match the host type.
     * This is used in case where the database needs help in determining
     * the parameter format.
     * @return The logical preferred length (digits or characters). 0 is 
     *   returned in case it cannot be determined.
     */
    IFR_size_t getPreferredLength() const;

    /**
     * Check, whether this parameter supplies its data using the
     * 'DATA AT EXECUTE' mechanism, i.e. application callbacks.
     * @param offset The offset to retrieve the indicator value.
     * @param rowsize The rowsize to retrieve the indicator.
     *
     * @return <code>true</code> if the 'DATA AT EXECUTE' mechanism is used, 
     *   <code>false</code> if not.
     */
    inline IFR_Bool hasDataAtExecute(IFR_Int4 offset=0, IFR_size_t rowsize=0)
    {
        return hasDataAtExecute(getLengthIndicator(offset, rowsize));
    }
    
    /**
     * Checks whether the data of this parameter points to the
     * ABAP table / OMS stream with the given id.
     * @param abapTabId The table id to look for.
     * @return @c true if this parameter identifies the requested
     *   ABAP table or OMS stream.
     */
    IFR_Bool isABAPTable(IFR_Int4 abapTabId);


    /**
     * Check whether a given indicator means that the data is delivered at
     * execute time instead of being delivered at prepare time.
     * @param lengthindicator The indicator value to check.
     * @return <code>true</code> if the 'DATA AT EXECUTE' mechanism is used, 
     *   <code>false</code> if not.
     */
    static inline IFR_Bool hasDataAtExecute(IFR_Length *lengthindicator)
    {
        if(lengthindicator==0) {
            return false;
        }
        if(*lengthindicator == IFR_DATA_AT_EXEC ||
           *lengthindicator < IFR_LEN_DATA_AT_EXEC_OFFSET) {
            return true;
        } else {
            return false;
        }
    }
    
    /**
     * Returns the encoding of character data of this host type.
     * @param hosttype The host type to check.
     */
    static IFR_StringEncoding getEncoding(IFR_HostType hosttype);

    /**
     * Traces the parameter. The data content is not traced, but the 
     * pointers that are stored in the parameter.
     * @param s The output stream.
     * @param parameterindex The index of the parameter.
     * @param nodata Do not print The indicator/data pointers
     * @return @c true if the parameter is really bound, @c false if it is not set.
     */
    IFR_Bool sqlTraceParameter(IFR_TraceStream& s, IFR_UInt2 parameterindex, IFR_Bool nodata=false);

    /**
     * Traces the parameter. The data content is not traced, but the 
     * pointers that are stored in the parameter.
     * @param s The output stream.
     * @param parametertag Tag printed instead of index.
     * @param nodata Do not print The indicator/data pointers
     * @return @c true if the parameter is really bound, @c false if it is not set.
     */
    IFR_Bool sqlTraceParameter(IFR_TraceStream& s, const char *parametertag, IFR_Bool nodata=false);
    
    /**
     * Traces the parameter. The data content is not traced, but the 
     * pointers that are stored in the parameter.
     * @param s              The output stream.
     * @param parameterindex The index of the parameter.
     * @param conversionRC   The return code of the conversion routine that previously did fill
     *                       the parameter.
     * @param offset         The offset for getting the parameter data.
     * @param rowsize        The rowsize for getting the parameter data.
     * @param parameterdata  Alternative parameter data.
     * @param lengthindicator Alternative length indicator.
     * @param input          Tracing is for input data (data is there).
     * @return @c true if the parameter is really bound, @c false if it is not set.
     */
    void sqlTraceParameterData(IFR_TraceStream& s, 
                               IFR_UInt2        parameterindex, 
                               IFR_Retcode      conversionRC,
                               IFR_Int4         offset,
                               IFR_Int4         rowsize,
                               unsigned char   *parameterdata,
                               IFR_Length      *lengthindicator,
                               IFR_Bool         input);
    
private:
    /**
     * Computes the raw size depending on offset, rowsize, and size of host type.
     * If the row size is <code>!=</code> 0, the offset is (offset * row size), 
     * otherwise it is (offset * size of host type).
     * @param offsetindex The offset index, starting with 0.
     * @param rowsize The row size, or 0 if there is no row-wise binding wanted.
     * @param hosttypesize The size of the host type, for column-wise binding.
     * @return The raw byte offset from the start.
     */
    inline IFR_ptrdiff_t rawSize(IFR_Int4 offsetindex, IFR_size_t rowsize, IFR_size_t hosttypesize)
    {
        if(rowsize) {
            return offsetindex * rowsize;
        } else {
            return offsetindex * hosttypesize;
        }
    }
    
    IFR_HostType   m_hosttype;                 //!< The host type.
    unsigned char *m_data;                     //!< Pointer to the data.
    IFR_Length    *m_lengthindicator;          //!< Length of the data or special indicator.
    IFR_Length     m_byteslength;              //!< Length of the data buffer in bytes. 
    IFR_Length    *m_posindicator;             //!< Start position for reading of long data.
    IFR_Int2       m_digits;                   //!< digits of numeric parameters. 
    IFR_Int2       m_fraction;                 //!< fraction of numeric parameters. 
    IFR_Bool       m_addrbound;                //!< Flag: Bound by Address.
    IFR_Bool       m_terminate;                //!< Flag: If the out Parameter should be zero terminated.
};


#endif // IFR_PARAMETER_H
