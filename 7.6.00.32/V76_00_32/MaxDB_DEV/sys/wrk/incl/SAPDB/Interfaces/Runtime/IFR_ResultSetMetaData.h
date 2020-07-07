/*!
  @file           IFR_ResultSetMetaData.h
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Util
  @brief          Metadata about a ResultSet.
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
#ifndef IFR_RESULTSETMETADATA_H
#define IFR_RESULTSETMETADATA_H

#include "SAPDB/Interfaces/SQLDBC/SQLDBC.h"
#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_Converter.h"

using namespace SQLDBC;

IFR_BEGIN_NAMESPACE

/**
 * @ingroup IFR_Statement
 * An object that can be used to find out about the types and properties of 
 * the columns in a ResultSet.
 * 
 */
class IFR_ResultSetMetaData 
{
public:
  enum ColumnNullBehavior {
      /**
     * The constant indicating that a
     * column does not allow <code>NULL</code> values.
     */
    columnNoNulls = 0,

    /**
     * The constant indicating that a
     * column allows <code>NULL</code> values.
     */
    columnNullable = 1,
     
    /**
     * The constant indicating that the
     * nullability of a column's values is unknown.
     */
    columnNullableUnknown = 2
  };
  /**
   * @brief Returns the number of columns in this ResultSet object. 
   * @return <code>IFR_Int2</code> the number of columns.
   */    
   virtual IFR_Int2 getColumnCount() = 0;
    
  /**
   * @brief Indicates the nullability of values in the designated column.
   * @param column - the first column is 1, the second is 2, ... 
   * @return the nullability status of the given column; one of columnNoNulls, 
   * columnNullable or columnNullableUnknown.
   */        
   inline IFR_Int4 isNullable(IFR_Int2 column){
     DBUG_METHOD_ENTER(IFR_ResultSetMetaData,isNullable);
     DBUG_PRINT(column);
      IFRConversion_Converter *conv = this->findColInfo(column);
      DBUG_RETURN((conv)?((conv->isNullable())?columnNullable:columnNoNulls):columnNullableUnknown);
   }
  
  /**
   * @brief Indicates the designated column's maximum width in characters.
   * @param column the first column is 1, the second is 2, ...
   * @return the maximum number of bytes allowed as the width
   *          of the designated column
   */  
   inline IFR_Int4 getColumnLength(IFR_Int2 column){
     DBUG_METHOD_ENTER(IFR_ResultSetMetaData,getColumnLength);
     DBUG_PRINT(column);
      IFRConversion_Converter *conv = this->findColInfo(column);
      DBUG_RETURN((conv)?((IFR_Int4) conv->getLength ()):0);
   } 
  /**
   * @brief Indicates the designated column's maximum physical width in bytes.
   * @param column the first column is 1, the second is 2, ...
   * @return the maximum number of bytes allowed as the physically width
   *          of the designated column
   */  
   inline IFR_Int4 getPhysicalLength(IFR_Int2 column){
     DBUG_METHOD_ENTER(IFR_ResultSetMetaData,getPhysicalLength);
     DBUG_PRINT(column);
      IFRConversion_Converter *conv = this->findColInfo(column);
      DBUG_RETURN( (conv)?((IFR_Int4) conv->getIOLength ()-1):0);
   } 

    /**
     * @brief Get the designated column's name.
     * @param column the first column is 1, the second is 2, ...
     * @param buffer the buffer where the column name should be copied into
     * @param encoding the encoding of the column name that is requested
     * @param bufferLength the length of the buffer.
     * @return <code>IFR_OK</code> on success, <code>IFR_DATA_TRUNC</code> if the buffer is too small.
     */
    IFR_Retcode getColumnName(IFR_Int2 column, 
                              char *buffer,
                              IFR_StringEncoding encoding,
                              IFR_Length         bufferSize,
                              IFR_Length         *bufferLength)
    { 
      DBUG_METHOD_ENTER(IFR_ResultSetMetaData,getColumnName);
      DBUG_PRINT(column);
      DBUG_PRINT(encoding);
      DBUG_PRINT(bufferSize);
      DBUG_PRINT(bufferLength);
      
        IFRConversion_Converter *conv = this->findColInfo(column);
        if(conv == 0) {
            DBUG_RETURN(IFR_NOT_OK);
        } else {
          IFR_Retcode rc = conv->getName(buffer, encoding, bufferSize, bufferLength);
          DBUG_PRINT_BUFFER(buffer, *bufferLength, bufferSize, encoding);
          DBUG_RETURN(rc);          
        }
    }
  /**
   * @brief Get the designated column's number of decimal digits.
   *
   * @param column the first column is 1, the second is 2, ...
   * @return precision
   */
   inline IFR_Int4 getPrecision(IFR_Int2 column){
     DBUG_METHOD_ENTER(IFR_ResultSetMetaData,getPrecision);
     DBUG_PRINT(column);
     IFRConversion_Converter *conv = this->findColInfo(column);
     DBUG_RETURN( (conv)?(conv->getLength()):0);
   }
   
  /**
   * Gets the designated column's number of digits to right of the decimal point.
   *
   * @param column the first column is 1, the second is 2, ...
   * @return scale
   */
   inline IFR_Int4 getScale(IFR_Int2 column){
     DBUG_METHOD_ENTER(IFR_ResultSetMetaData,getScale);
     DBUG_PRINT(column);
     IFRConversion_Converter *conv = this->findColInfo(column);
     DBUG_RETURN( (conv)?(conv->getPrecision()):0);
   }
  /**
   * @brief Retrieves the designated column's SQL type.
   *
   * @param column the first column is 1, the second is 2, ...
   * @return SQL type from <code>IFR_SQLType</code>
   */
   IFR_SQLType getColumnType(IFR_Int2 column){
     DBUG_METHOD_ENTER(IFR_ResultSetMetaData,getColumnType);
     DBUG_PRINT(column);
     IFRConversion_Converter *conv = this->findColInfo(column);
     DBUG_RETURN( (conv)?(conv->getSQLType()):IFR_SQLTYPE_UNKNOWN);
   }  
  /**
   * @brief Indicates whether it is possible for a write on the designated column to succeed.
   *
   * @param column the first column is 1, the second is 2, ...
   * @return <code>true</code> if so; <code>false</code> otherwise
   */
   inline IFR_Bool isWritable(IFR_Int2 column){
     return 0;
   }
    
    /**
     * Destructor.
     */
    virtual ~IFR_ResultSetMetaData();

protected:
    
    /**
     * @brief Returns the IFRConversion_Converter of the designated column.
     * @param column - the first column is 1, the second is 2, ... 
     * @return <code>IFRConversion_Converter</code> of the designated column.
     */
    virtual IFRConversion_Converter * findColInfo (IFR_Int2 columnIndex) = 0;

};

inline
IFR_TraceStream& operator << (IFR_TraceStream& s, const IFR_ResultSetMetaData *valueptr)
{
    char str[64];
    sp77sprintf(str, 64, "IFR_Statement@%p", valueptr);
    return s << str;
}

IFR_END_NAMESPACE

#endif //IFR_RESULTSETMETADATA_H
