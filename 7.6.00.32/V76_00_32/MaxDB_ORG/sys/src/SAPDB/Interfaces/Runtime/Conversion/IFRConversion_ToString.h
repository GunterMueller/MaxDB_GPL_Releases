/*!
  @file           IFRConversion_ToString.h
  @author         D039759
  @ingroup        Conversion
  @brief          Helper methods for conversion from integral types to string which
                  appear in many places.
  @see

\if EMIT_LICENCE
    ========== licence begin  GPL
    Copyright (C) 2001-2005 SAP AG

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
#ifndef IFRCONVERSION_TOSTRING_H
#define IFRCONVERSION_TOSTRING_H

#include "Interfaces/Runtime/IFR_Types.h"
#include "Interfaces/Runtime/IFR_ErrorHndl.h"
#include "Interfaces/Runtime/Util/IFRUtil_DateTime.h"
#include "Interfaces/Runtime/Util/IFRUtil_SQLNumeric.h"
#include "hsp77.h"

IFR_BEGIN_NAMESPACE

/**
 * Object used for the conversion.
 */
template <class T>
class IFRConversion_ToString
{
public:
    typedef char BufferType [1];
    
    inline IFR_Retcode convert(T& data, char * buffer, IFR_size_t& charsWritten, IFR_ErrorHndl& error)
    {
        error.setRuntimeError(IFR_ERR_CONVERSION_NOT_SUPPORTED_I, -1);
        return IFR_NOT_OK;
    }
};


template<> 
class IFRConversion_ToString<double>
{
public:
    typedef char BufferType[64];
    
    inline IFR_Retcode convert(double& data, char * buffer, IFR_size_t& charsWritten, IFR_ErrorHndl& error)
    {
        sp77sprintf(buffer, sizeof(BufferType), "%f", data);
        charsWritten = strlen(buffer);
        return IFR_OK;
    }
};

template<>
class IFRConversion_ToString<IFR_UInt8>
{
public:
    typedef char BufferType [32];
    
    inline IFR_Retcode convert(IFR_UInt8& data, char * buffer, IFR_size_t& charsWritten, IFR_ErrorHndl& error)
    {
        sp77sprintf(buffer, sizeof(BufferType), "%qu", data);
        charsWritten = strlen(buffer);
        return IFR_OK;
    }
    
};

template<>
class IFRConversion_ToString<IFR_Int8>
{
public:
    typedef char BufferType [32];
    
    inline IFR_Retcode convert(IFR_Int8& data, char * buffer, IFR_size_t& charsWritten, IFR_ErrorHndl& error)
    {
        sp77sprintf(buffer, sizeof(BufferType), "%qd", data);
        charsWritten = strlen(buffer);
        return IFR_OK;
    }
    
};


template<>
class IFRConversion_ToString<IFR_UInt4>
{
public:
    typedef char BufferType [13];
    
    inline IFR_Retcode convert(IFR_UInt4& data, char * buffer, IFR_size_t& charsWritten, IFR_ErrorHndl& error)
    {
        sp77sprintf(buffer, sizeof(BufferType), "%u", data);
        charsWritten = strlen(buffer);
        return IFR_OK;
    }
    
};

template<>
class IFRConversion_ToString<IFR_Int4>
{
public:
    typedef char BufferType [13];
    
    inline IFR_Retcode convert(IFR_Int4& data, char * buffer, IFR_size_t& charsWritten, IFR_ErrorHndl& error)
    {
        sp77sprintf(buffer, sizeof(BufferType), "%d", data);
        charsWritten = strlen(buffer);
        return IFR_OK;
    }
    
};


template<>
class IFRConversion_ToString<IFR_UInt2>
{
public:
    typedef char BufferType [7];
    
    inline IFR_Retcode convert(IFR_UInt2& data, char * buffer, IFR_size_t& charsWritten, IFR_ErrorHndl& error)
    {
        sp77sprintf(buffer, sizeof(BufferType), "%hu", data);
        charsWritten = strlen(buffer);
        return IFR_OK;
    }
    
};

template<>
class IFRConversion_ToString<IFR_Int2>
{
public:
    typedef char BufferType [7];
    
    inline IFR_Retcode convert(IFR_Int2& data, char * buffer, IFR_size_t& charsWritten, IFR_ErrorHndl& error)
    {
        sp77sprintf(buffer, sizeof(BufferType), "%hd", data);
        charsWritten = strlen(buffer);
        return IFR_OK;
    }
};

template<>
class IFRConversion_ToString<SQL_DATE_STRUCT>
{
public:
    
    typedef char BufferType[11];
    

    inline IFRConversion_ToString(IFR_Int4 _index, IFR_Int4 _dateformat)
    :index(_index),
     dateformat(_dateformat)
    {}
    
    
    inline IFR_Retcode convert(SQL_DATE_STRUCT& data, char *buffer, IFR_size_t& charsWritten, IFR_ErrorHndl& error)
    {
        if(!IFRUtil_DateTime::isDateValid(data)) {
            error.setRuntimeError(IFR_ERR_ILLEGAL_DATE_VALUE_I, index);
            return IFR_NOT_OK;
        } else {
            switch(dateformat) {
            case IFR_DateTimeFormat::Normal_C:
                sp77sprintf(buffer, sizeof(BufferType), "%.4hd%.2hd%.2hd", data.year, data.month, data.day);
                charsWritten = 8;
                break;
            case IFR_DateTimeFormat::Iso_C:
            case IFR_DateTimeFormat::Jis_C:
            case IFR_DateTimeFormat::WasAnsiNowIsSameAsIso_C:
                sp77sprintf(buffer, sizeof(BufferType), "%.4hd-%.2hd-%.2hd", data.year, data.month, data.day);
                charsWritten = 10;
                break;
            default:
                error.setRuntimeError(IFR_ERR_DATETIMEFORMAT_UNSUPPORTED_I, index);
                return IFR_NOT_OK;
            }
            return IFR_OK;
        }
        
    }

private:
    IFR_Int4 index;
    IFR_Int4 dateformat;
        
};


template<>
class IFRConversion_ToString<SQL_TIME_STRUCT>
{
public:
    typedef char BufferType [9];

    inline IFRConversion_ToString(IFR_Int4 _index, IFR_Int4 _dateformat)
    :index(_index),
     dateformat(_dateformat)
    {}

    inline IFR_Retcode convert(SQL_TIME_STRUCT& data, char *buffer, IFR_size_t& charsWritten, IFR_ErrorHndl& error)
    {
        if(!IFRUtil_DateTime::isTimeValid(data)) {
            error.setRuntimeError(IFR_ERR_ILLEGAL_TIME_VALUE_I, index);
            return IFR_NOT_OK;
        } else {
            switch(dateformat) {
            case IFR_DateTimeFormat::Normal_C:
                sp77sprintf(buffer, sizeof(BufferType), "%.2hd%.2hd%.2hd", data.hour, data.minute, data.second);
                charsWritten=6;
                break;
            case IFR_DateTimeFormat::Iso_C:
            case IFR_DateTimeFormat::Jis_C:
            case IFR_DateTimeFormat::WasAnsiNowIsSameAsIso_C:
                sp77sprintf(buffer, sizeof(BufferType), "%.2hd:%.2hd:%.2hd", data.hour, data.minute, data.second);
                charsWritten=8;
                break;
            default:
                error.setRuntimeError(IFR_ERR_DATETIMEFORMAT_UNSUPPORTED_I, index);
                return IFR_NOT_OK;
            }
        }
        return IFR_OK;
    }
    
private:
    IFR_Int4 index;
    IFR_Int4 dateformat;
};

template<>
class IFRConversion_ToString<SQL_TIMESTAMP_STRUCT>
{
public:
    typedef char BufferType[27];

    inline IFRConversion_ToString(IFR_Int4 _index, IFR_Int4 _dateformat)
    :index(_index),
     dateformat(_dateformat)
    {}

    inline IFR_Retcode convert(SQL_TIMESTAMP_STRUCT& data,
                               char *buffer,
                               IFR_size_t& charsWritten,
                               IFR_ErrorHndl& error)
    {
        if(!IFRUtil_DateTime::isTimestampValid(data)) {
            error.setRuntimeError(IFR_ERR_ILLEGAL_TIMESTAMP_VALUE_I, index); 
            return IFR_NOT_OK;
        } else {
            IFR_UInt4 millis=data.fraction/1000;
            switch(dateformat) {
            case IFR_DateTimeFormat::Normal_C:
                sp77sprintf(buffer, sizeof(BufferType), "%.4hd%.2hd%.2hd%.2hd%.2hd%.2hd%.6u", data.year, data.month, data.day,
                            data.hour, data.minute, data.second, millis);
                charsWritten=20;
                return IFR_OK;
            case IFR_DateTimeFormat::Usa_C:
            case IFR_DateTimeFormat::Eur_C:
            case IFR_DateTimeFormat::TsEur_C:
            case IFR_DateTimeFormat::Iso_C:
            case IFR_DateTimeFormat::Jis_C:
            case IFR_DateTimeFormat::WasAnsiNowIsSameAsIso_C:
                sp77sprintf(buffer, sizeof(BufferType), "%.4hd-%.2hd-%.2hd %.2hd:%.2hd:%.2hd.%.6u", data.year, data.month, data.day,
                            data.hour, data.minute, data.second, millis);
                charsWritten=26;
                return IFR_OK;
            default:
                error.setRuntimeError(IFR_ERR_DATETIMEFORMAT_UNSUPPORTED_I, index);
                return IFR_NOT_OK;
            }
        }
    }
    
private:
    IFR_Int4 index;
    IFR_Int4 dateformat;
};

template<> 
class IFRConversion_ToString<SQL_NUMERIC_STRUCT>
{
public:
    typedef char BufferType[132];
    
    IFRConversion_ToString(IFR_Int4 _index)
    :index(_index)
    {}

    inline IFR_Retcode convert(SQL_NUMERIC_STRUCT& data, char * buffer, IFR_size_t& charsWritten, IFR_ErrorHndl& error)
    {
        charsWritten = sizeof(BufferType); // numericToAsciiString needs length of buffer.
        IFR_Retcode rc = IFRUtil_SQLNumeric::numericToAsciiString(data,
                                                                  buffer,
                                                                  charsWritten);
        if(rc != IFR_OK) {
            error.setRuntimeError(IFR_ERR_PARAM_CONVERSION_TRUNCATEDATA_I, index);
            return rc;
        }
        
        return rc;
    }

private:
    IFR_Int4 index;
};

IFR_END_NAMESPACE

#endif

