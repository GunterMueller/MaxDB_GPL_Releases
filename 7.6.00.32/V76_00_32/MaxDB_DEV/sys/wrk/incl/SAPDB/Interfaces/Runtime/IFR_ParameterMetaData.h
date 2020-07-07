/*!
  @file           IFR_ParameterMetaData.h
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Util
  @brief          Metadata about IN/OUT/INOUT Parameter.
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
#ifndef IFR_PARAMETERMETADATA_H
#define IFR_PARAMETERMETADATA_H

#include "SAPDB/Interfaces/SQLDBC/SQLDBC.h"
#include "SAPDB/Interfaces/Runtime/Conversion/IFRConversion_Converter.h"

using namespace SQLDBC;

IFR_BEGIN_NAMESPACE

/**
 * @ingroup IFR_Statement
 * An object that can be used to find out about the types and properties of 
 * the paramters in a PreparedStatement object.
 */
class IFR_ParameterMetaData 
{
public:
   
  enum ParameterNullBehavior { 
    /**
     * The constant indicating that a
     * parameter will not allow <code>NULL</code> values.
     */
    parameterNoNulls = SQLDBC_ParameterMetaData::parameterNoNulls,
  
    /**
     * The constant indicating that a
     * parameter will allow <code>NULL</code> values.
     */
    parameterNullable = SQLDBC_ParameterMetaData::parameterNullable,
  
    /**
     * The constant indicating that the
     * nullability of a parameter is unknown.
     */
    parameterNullableUnknown = SQLDBC_ParameterMetaData::parameterNullableUnknown
  };

  enum ParameterMode { 
  
    /**
     * The constant indicating that the mode of the parameter is unknown.
     */
    parameterModeUnknown = SQLDBC_ParameterMetaData::parameterModeUnknown,

    /**
     * The constant indicating that the parameter's mode is IN.
     */
    parameterModeIn = SQLDBC_ParameterMetaData::parameterModeIn,

    /**
     * The constant indicating that the parameter's mode is INOUT.
     */
    parameterModeInOut = SQLDBC_ParameterMetaData::parameterModeInOut,

    /**
     * The constant indicating that the parameter's mode is  OUT.
     */
    parameterModeOut = SQLDBC_ParameterMetaData::parameterModeOut
  };  
    
  /**
   * @brief Retrieves the designated parameter's mode.
   * @param param - the first parameter is 1, the second is 2, ... 
   * @return mode of the parameter; one of 
   * IFR_ParameterMetaData.parameterModeIn, 
   * IFR_ParameterMetaData.parameterModeOut, or 
   * IFR_ParameterMetaData.parameterModeInOut.
   */        
   IFR_Int4 getParameterMode(IFR_Int2 param){
     DBUG_METHOD_ENTER(IFR_ParameterMetaData,getParameterMode);
     IFRConversion_Converter * pi = this->findParamInfo (param);
     if (!pi)
        DBUG_RETURN(parameterModeUnknown);
     if (pi->supportsInput()){
       if (pi->supportsOutput())
          DBUG_RETURN(parameterModeInOut);
       else
          DBUG_RETURN(parameterModeIn);   
     }
     DBUG_RETURN(parameterModeOut);   
   }
            
  /**
   * @brief Returns the number of paramteter/columns in this PreparedStatement/ResultSet object. 
   * @return <code>IFR_Int4</code> the number of parameters/columns.
   */    
   virtual IFR_Int2 getParameterCount() const = 0;
    
  /**
   * @brief Retrieves whether null values are allowed in the designated parameter.
   * @param param - the first parameter is 1, the second is 2, ... 
   * @return the nullability status of the given parameter; one of 
   * IFR_ParameterMetaData.parameterNoNulls, 
   * IFR_ParameterMetaData.parameterNullable, or 
   * IFR_ParameterMetaData.parameterNullableUnknown
   */        
   inline IFR_Int4 isNullable(IFR_Int2 param){
     DBUG_METHOD_ENTER(IFR_ParameterMetaData,isNullable);
      IFRConversion_Converter *conv = this->findParamInfo(param);
      DBUG_RETURN((conv)?((conv->isNullable())?parameterNullable:parameterNoNulls):parameterNullableUnknown);
   }
  
  /**
   * @brief Indicates the designated parameter's maximum width in characters.
   * @param param - the first parameter is 1, the second is 2, ... 
   * @return the maximum number of bytes allowed as the width
   *          of the designated parameter
   */  
   inline IFR_Int4 getParameterLength(IFR_Int2 param){
     DBUG_METHOD_ENTER(IFR_ParameterMetaData,getParameterLength);
      IFRConversion_Converter *conv = this->findParamInfo(param);
      DBUG_RETURN((conv)?((IFR_Int4) conv->getLength ()):0);
   } 
  /**
   * @brief Indicates the designated parameter's maximum physical width in bytes.
   * @param param - the first parameter is 1, the second is 2, ... 
   * @return the maximum number of bytes allowed as the physically width
   *          of the designated parameter
   */  
   inline IFR_Int4 getPhysicalLength(IFR_Int2 param){
     DBUG_METHOD_ENTER(IFR_ParameterMetaData,getPhysicalLength);
      IFRConversion_Converter *conv = this->findParamInfo(param);
      DBUG_RETURN((conv)?((IFR_Int4) conv->getIOLength ()-1):0);
   } 

    /**
     * @brief Get the designated parameter's name.
     * @param parameter the first parameter is 1, the second is 2, ...
     * @param buffer the buffer where the parameter name should be copied into
     * @param encoding the encoding of the parameter name that is requested
     * @param bufferLength the length of the buffer.
     * @return <code>IFR_OK</code> on success, <code>IFR_DATA_TRUNC</code> if the buffer is too small.
     */
    IFR_Retcode getParameterName(IFR_Int2 parameter, 
                              char *buffer,
                              IFR_StringEncoding encoding,
                                 IFR_Length         bufferSize,
                              IFR_Length         *bufferLength)
    {
     DBUG_METHOD_ENTER(IFR_ParameterMetaData,getParameterName);
     DBUG_PRINT(parameter);
     DBUG_PRINT(encoding);
     DBUG_PRINT(bufferSize);
     DBUG_PRINT(bufferLength);
        IFRConversion_Converter *conv = this->findParamInfo(parameter);
        if(conv == 0) {
            DBUG_RETURN(IFR_NOT_OK);
        } else {
          IFR_Retcode rc = conv->getName(buffer, encoding, bufferSize, bufferLength);
          DBUG_PRINT_BUFFER(buffer, *bufferLength, bufferSize, encoding);
          DBUG_RETURN(rc);          
        }
    }

    
  /**
   * @brief Get the designated parameter's number of decimal digits.
   * For number types, getPrecision gets the number of decimal digits.
   * For character types, it gets the maximum length in characters. For 
   * binary types, it gets the maximum length in bytes.
   * @param param - the first parameter is 1, the second is 2, ... 
   * @return precision
   */
   inline IFR_Int4 getPrecision(IFR_Int2 param){
     DBUG_METHOD_ENTER(IFR_ParameterMetaData,getPrecision);
     IFRConversion_Converter *conv = this->findParamInfo(param);
     DBUG_RETURN( (conv)?(conv->getLength()):0);
   }
  /**
   * Gets the designated parameter's number of digits to right of the decimal point.
   * @param param - the first parameter is 1, the second is 2, ... 
   * @return scale
   */
   inline IFR_Int4 getScale(IFR_Int2 param){
     DBUG_METHOD_ENTER(IFR_ParameterMetaData,getScale);
     IFRConversion_Converter *conv = this->findParamInfo(param);
     DBUG_RETURN( (conv)?(conv->getPrecision()):0);
   }
  /**
   * @brief Retrieves the designated parameter's SQL type.
   * @param param - the first parameter is 1, the second is 2, ... 
   * @return SQL type from <code>IFR_SQLType</code>
   */
   inline IFR_SQLType getParameterType(IFR_Int2 param){
     DBUG_METHOD_ENTER(IFR_ParameterMetaData,getParameterType);
     IFRConversion_Converter *conv = this->findParamInfo(param);
     DBUG_RETURN((conv)?(conv->getSQLType()):IFR_SQLTYPE_UNKNOWN);
   }

    /**
     * Destructor.
     */
    virtual ~IFR_ParameterMetaData();

protected:
    /**
     * @brief Returns the IFRConversion_Converter of the designated parameter.
     * @param param - the first parameter is 1, the second is 2, ... 
     * @return <code>IFRConversion_Converter</code> of the designated parameter.
     */
    virtual IFRConversion_Converter * findParamInfo (IFR_Int2 param) = 0;
    
};


inline
IFR_TraceStream& operator << (IFR_TraceStream& s, const IFR_ParameterMetaData *valueptr)
{
    char str[64];
    sp77sprintf(str, 64, "IFR_Statement@%p", valueptr);
    return s << str;
}

IFR_END_NAMESPACE

#endif //IFR_PARAMETERMETADATA_H
