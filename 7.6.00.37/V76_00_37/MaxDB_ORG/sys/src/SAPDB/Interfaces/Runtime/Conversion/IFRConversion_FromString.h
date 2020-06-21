/*!
  @file           IFRConversion_FromString.h
  @author         D039759
  @ingroup        Data conversion.
  @brief          Helper functions for data conversion.
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
#ifndef IFRCONVERSION_FROMSTRING_H
#define IFRCONVERSION_FROMSTRING_H

#include "Interfaces/Runtime/IFR_Types.h"
#include "Interfaces/Runtime/IFR_ErrorHndl.h"
#include "Interfaces/Runtime/Util/IFRUtil_DateTime.h"
#include "Interfaces/Runtime/Util/IFRUtil_SQLNumeric.h"

#include <errno.h>
#include <ctype.h>
#include <stdlib.h>

IFR_BEGIN_NAMESPACE


#define ISDIGIT(x) ((x) >= '0' && (x) <= '9')
#define ISSPACE(x) ((x)==' ' || (x)=='\t' || (x)==0x0D || (x)==0x0A)

//----------------------------------------------------------------------
template <class T>
class IFRConversion_FromString
{
public:
    typedef char BufferType[1];
    typedef T TargetType;

    inline IFR_Retcode convert(T& dest, IFR_Length* indicator, char *buffer, IFR_ErrorHndl& error)
    {
        return IFR_NOT_OK;
    }
};

//----------------------------------------------------------------------
template<>
class IFRConversion_FromString<double>
{
public:
    typedef char BufferType[64];
    typedef double TargetType;

    IFRConversion_FromString(IFR_Int4 _index)
    :index(_index)
    {}
    
    inline IFR_Retcode convert(double& dest, IFR_Length* indicator, char *buffer, IFR_ErrorHndl& error)
    {
        char *ep = 0;
        errno = 0;
        dest = strtod(buffer, &ep);
        if(errno == ERANGE && dest != 0.0) {
            error.setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, index);
            return IFR_NOT_OK;
        }
        if(ep) {
            while(*ep) {
                if(!ISSPACE(*ep)) {
                    error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
                    return IFR_NOT_OK;
                }
                ++ep;
            }
        }
        if(indicator) {
            *indicator = sizeof(double);
        }
        return IFR_OK;
    }
    
private:
    IFR_Int4 index;
};

//----------------------------------------------------------------------
template<>
class IFRConversion_FromString<float>
{
public:
    typedef char BufferType[64];
    typedef float TargetType;

    IFRConversion_FromString(IFR_Int4 _index)
    :index(_index)
    {}
    
    inline IFR_Retcode convert(float& dest, IFR_Length* indicator, char *buffer, IFR_ErrorHndl& error)
    {
        char *ep = 0;
        errno = 0;
        double tmp_dest;
        tmp_dest = strtod(buffer, &ep);
        if(errno == ERANGE && dest != 0.0) {
            error.setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, index);
            return IFR_NOT_OK;
        }
        if(tmp_dest >3.4028234663852886E38 || tmp_dest < -3.4028234663852886E38f) {
            error.setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, index);
            return IFR_NOT_OK;
        }
        dest = (float) tmp_dest;
        if(ep) {
            while(*ep) {
                if(!ISSPACE(*ep)) {
                    error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
                    return IFR_NOT_OK;
                }
                ++ep;
            }
        }
        if(indicator) {
            *indicator = sizeof(float);
        }
        return IFR_OK;
    }
    
private:
    IFR_Int4 index;
};

//----------------------------------------------------------------------
template<>
class IFRConversion_FromString<IFR_Int1>
{
public:
    typedef char BufferType [64];
    typedef IFR_Int1 TargetType;
    
    IFRConversion_FromString(IFR_Int4 _index)
    :index(_index)
    {}
    
    inline IFR_Retcode convert(IFR_Int1& dest, IFR_Length* indicator, char *buffer, IFR_ErrorHndl& error)
    {
        char *p = buffer;
        while(*p && ISSPACE(*p)) {
            ++p;
        }
        if(*p == 0) {
            dest = 0;
            if(indicator)
                *indicator = sizeof(TargetType);
            return IFR_OK;
        }
    
        char *trail=0;
        errno = 0;
        long l=strtol(p, &trail, 10);
        
        if(errno) {
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            return IFR_NOT_OK;
        }
        if( l < MIN_IFR_INT1 || l > MAX_IFR_INT1) {
            error.setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, index);
            return IFR_NOT_OK;
        }
        dest = (IFR_Int1) l;
        if(p == trail) {
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            return IFR_NOT_OK;
        }
        
        // trailing spaces are not treated as harmful
        while(*trail != '\0') {
            if(!(ISSPACE(*trail))) {
                error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
                return IFR_NOT_OK;
            }
            ++trail;
        }
        
        if(indicator)
            *indicator = sizeof(TargetType);
        return IFR_OK;
    }
private:
    IFR_Int4 index;
};

//----------------------------------------------------------------------
template<>
class IFRConversion_FromString<IFR_UInt1>
{
public:
    typedef char BufferType [64];
    typedef IFR_UInt1 TargetType;
    
    IFRConversion_FromString(IFR_Int4 _index)
    :index(_index)
    {}
    
    inline IFR_Retcode convert(IFR_UInt1& dest, IFR_Length* indicator, char *buffer, IFR_ErrorHndl& error)
    {
        char *p = buffer;
        while(*p && ISSPACE(*p)) {
            ++p;
        }
        if(*p == 0) {
            dest = 0;
            if(indicator)
                *indicator = sizeof(TargetType);
            return IFR_OK;
        }
        
        if(*p == '-') {
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            return IFR_NOT_OK;
        }
    
        char *trail=0;
        errno = 0;
        unsigned long l=strtoul(p, &trail, 10);
        
        if(errno) {
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            return IFR_NOT_OK;
        }
        if( l < MIN_IFR_UINT1 || l > MAX_IFR_UINT1) {
            error.setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, index);
            return IFR_NOT_OK;
        }
        dest = (TargetType) l;
        if(p == trail) {
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            return IFR_NOT_OK;
        }
        
        // trailing spaces are not treated as harmful
        while(*trail != '\0') {
            if(!(*trail == ' '  || *trail == '\t' || *trail==0x0D || *trail==0x0A)) {
                error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
                return IFR_NOT_OK;
            }
            ++trail;
        }
        
        if(indicator)
            *indicator = sizeof(TargetType);
        return IFR_OK;
}
    private:
    IFR_Int4 index;
};

//----------------------------------------------------------------------
template<>
class IFRConversion_FromString<IFR_Int2>
{
public:
    typedef char BufferType [64];
    typedef IFR_Int2 TargetType;
    
    IFRConversion_FromString(IFR_Int4 _index)
    :index(_index)
    {}
    
    inline IFR_Retcode convert(IFR_Int2& dest, IFR_Length* indicator, char *buffer, IFR_ErrorHndl& error)
    {
        char *p = buffer;
        while(*p && ISSPACE(*p)) {
            ++p;
        }
        if(*p == 0) {
            dest = 0;
            if(indicator)
                *indicator = sizeof(TargetType);
            return IFR_OK;
        }
    
        char *trail=0;
        errno = 0;
        long l=strtol(p, &trail, 10);
        
        if(errno) {
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            return IFR_NOT_OK;
        }
        if( l < MIN_IFR_INT2 || l > MAX_IFR_INT2) {
            error.setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, index);
            return IFR_NOT_OK;
        }
        dest = (TargetType) l;
        if(p == trail) {
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            return IFR_NOT_OK;
        }
        
        // trailing spaces are not treated as harmful
        while(*trail != '\0') {
            if(!(*trail == ' '  || *trail == '\t' || *trail==0x0D || *trail==0x0A)) {
                error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
                return IFR_NOT_OK;
            }
            ++trail;
        }
        
        if(indicator)
            *indicator = sizeof(TargetType);
        return IFR_OK;
    }
private:
    IFR_Int4 index;
};

//----------------------------------------------------------------------
template<>
class IFRConversion_FromString<IFR_UInt2>
{
public:
    typedef char BufferType [64];
    typedef IFR_UInt2 TargetType;
    
    IFRConversion_FromString(IFR_Int4 _index)
    :index(_index)
    {}
    
    inline IFR_Retcode convert(IFR_UInt2& dest, IFR_Length* indicator, char *buffer, IFR_ErrorHndl& error)
    {
        char *p = buffer;
        while(*p && ISSPACE(*p)) {
            ++p;
        }
        if(*p == 0) {
            dest = 0;
            if(indicator)
                *indicator = sizeof(TargetType);
            return IFR_OK;
        }

        if(*p == '-') {
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            return IFR_NOT_OK;
        }
    
        char *trail=0;
        errno = 0;
        unsigned long l=strtoul(p, &trail, 10);
        
        if(errno) {
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            return IFR_NOT_OK;
        }
        if( l < MIN_IFR_UINT2 || l > MAX_IFR_UINT2) {
            error.setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, index);
            return IFR_NOT_OK;
        }
        dest = (TargetType) l;
        if(p == trail) {
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            return IFR_NOT_OK;
        }
        
        // trailing spaces are not treated as harmful
        while(*trail != '\0') {
            if(!(*trail == ' '  || *trail == '\t' || *trail==0x0D || *trail==0x0A)) {
                error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
                return IFR_NOT_OK;
            }
            ++trail;
        }
        
        if(indicator)
            *indicator = sizeof(TargetType);
        return IFR_OK;
    }
private:
    IFR_Int4 index;
};

//----------------------------------------------------------------------
template<>
class IFRConversion_FromString<IFR_Int4>
{
public:
    typedef char BufferType [64];
    typedef IFR_Int4 TargetType;
    
    IFRConversion_FromString(IFR_Int4 _index)
    :index(_index)
    {}
    
    inline IFR_Retcode convert(IFR_Int4& dest, IFR_Length* indicator, char *buffer, IFR_ErrorHndl& error)
    {
        char *p = buffer;
        while(*p && ISSPACE(*p)) {
            ++p;
        }
        if(*p == 0) {
            dest = 0;
            if(indicator)
                *indicator = sizeof(TargetType);
            return IFR_OK;
        }
    
        char *trail=0;
        errno = 0;
#ifndef WIN32
#ifdef BIT64
        long l=strtol(p, &trail, 10);
#else
        long long l = strtoll(p, &trail, 10);
#endif
#else
        IFR_Int8 l = _strtoi64(p, &trail, 10);
#endif
        
        if(errno) {
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            return IFR_NOT_OK;
        }
        if( l < MIN_IFR_INT4|| l > MAX_IFR_INT4) {
            error.setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, index);
            return IFR_NOT_OK;
        }
        dest = (TargetType) l;
        if(p == trail) {
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            return IFR_NOT_OK;
        }
        
        // trailing spaces are not treated as harmful
        while(*trail != '\0') {
            if(!(*trail == ' '  || *trail == '\t' || *trail==0x0D || *trail==0x0A)) {
                error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
                return IFR_NOT_OK;
            }
            ++trail;
        }
        
        if(indicator)
            *indicator = sizeof(TargetType);
        return IFR_OK;

    }
private:
    IFR_Int4 index;
};

//----------------------------------------------------------------------
template<>
class IFRConversion_FromString<IFR_UInt4>
{
public:
    typedef char BufferType [64];
    typedef IFR_UInt4 TargetType;
    
    IFRConversion_FromString(IFR_Int4 _index)
    :index(_index)
    {}
    
    inline IFR_Retcode convert(IFR_UInt4& dest, IFR_Length* indicator, char *buffer, IFR_ErrorHndl& error)
    {
        char *p = buffer;
        while(*p && ISSPACE(*p)) {
            ++p;
        }
        if(*p == 0) {
            dest = 0;
            if(indicator)
                *indicator = sizeof(TargetType);
            return IFR_OK;
        }
    
        if(*p == '-') {
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            return IFR_NOT_OK;
        }

        char *trail=0;
        errno = 0;
#ifndef WIN32
#ifndef BIT64        
        unsigned long long l=strtoull(p, &trail, 10);
#else
        unsigned long l=strtoul(p, &trail, 10);
#endif
#else
        IFR_UInt8 l = _strtoui64(p, &trail, 10);
#endif
        
        if(errno) {
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            return IFR_NOT_OK;
        }
        if( l < MIN_IFR_UINT4 || l > MAX_IFR_UINT4) {
            error.setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, index);
            return IFR_NOT_OK;
        }
        dest = (TargetType) l;
        if(p == trail) {
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            return IFR_NOT_OK;
        }
        
        // trailing spaces are not treated as harmful
        while(*trail != '\0') {
            if(!(*trail == ' '  || *trail == '\t' || *trail==0x0D || *trail==0x0A)) {
                error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
                return IFR_NOT_OK;
            }
            ++trail;
        }
        
        if(indicator)
            *indicator = sizeof(TargetType);
        return IFR_OK;
        
    }
private:
    IFR_Int4 index;
};

//----------------------------------------------------------------------
template<>
class IFRConversion_FromString<IFR_Int8>
{
public:
    typedef char BufferType [64];
    typedef IFR_Int8 TargetType;
    
    IFRConversion_FromString(IFR_Int4 _index)
    :index(_index)
    {}
    
    inline IFR_Retcode convert(IFR_Int8& dest, IFR_Length* indicator, char *buffer, IFR_ErrorHndl& error)
    {
        char *p = buffer;
        while(*p && ISSPACE(*p)) {
            ++p;
        }
        if(*p == 0) {
            dest = 0;
            if(indicator)
                *indicator = sizeof(TargetType);
            return IFR_OK;
        }
    

        char *trail=0;
        errno = 0;
#ifndef WIN32
#ifdef BIT64
        dest =strtol(p, &trail, 10);
#else
        dest = strtoll(p, &trail, 10);
#endif
#else
        dest = _strtoi64(p, &trail, 10);
#endif
        if(errno) {
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            return IFR_NOT_OK;
        }

        if(p == trail) {
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            return IFR_NOT_OK;
        }
        
        // trailing spaces are not treated as harmful
        while(*trail != '\0') {
            if(!(*trail == ' '  || *trail == '\t' || *trail==0x0D || *trail==0x0A)) {
                error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
                return IFR_NOT_OK;
            }
            ++trail;
        }
        
        if(indicator)
            *indicator = sizeof(TargetType);
        return IFR_OK;

    }
private:
    IFR_Int4 index;
};

//----------------------------------------------------------------------
template<>
class IFRConversion_FromString<IFR_UInt8>
{
public:
    typedef char BufferType [64];
    typedef IFR_UInt8 TargetType;
    
    IFRConversion_FromString(IFR_Int4 _index)
    :index(_index)
    {}
    
    inline IFR_Retcode convert(IFR_UInt8& dest, IFR_Length* indicator, char *buffer, IFR_ErrorHndl& error)
    {
        char *p = buffer;
        while(*p && (*p == ' '  || *p == '\t' || *p==0x0D || *p==0x0A)) {
            ++p;
        }
        if(*p == 0) {
            dest = 0;
            if(indicator)
                *indicator = sizeof(TargetType);
            return IFR_OK;
        }
    

        if(*p == '-') {
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            return IFR_NOT_OK;
        }
        char *trail=0;
        errno = 0;
#ifndef WIN32
#ifdef BIT64
        dest =strtoul(p, &trail, 10);
#else
        dest = strtoull(p, &trail, 10);
#endif
#else
        dest = _strtoui64(p, &trail, 10);
#endif
        if(errno) {
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            return IFR_NOT_OK;
        }

        if(p == trail) {
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            return IFR_NOT_OK;
        }
        
        // trailing spaces are not treated as harmful
        while(*trail != '\0') {
            if(!(*trail == ' '  || *trail == '\t' || *trail==0x0D || *trail==0x0A)) {
                error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
                return IFR_NOT_OK;
            }
            ++trail;
        }
        
        if(indicator)
            *indicator = sizeof(TargetType);
        return IFR_OK;

    }
private:
    IFR_Int4 index;
};


//----------------------------------------------------------------------
template<>
class IFRConversion_FromString<SQL_DATE_STRUCT>
{
public:
    typedef char BufferType [64];
    typedef SQL_DATE_STRUCT TargetType;
    
    IFRConversion_FromString(IFR_Int4 _index, IFR_Int4 _dateformat)
    :index(_index),
     dateformat(_dateformat)
    {}
    
    inline IFR_Retcode convert(SQL_DATE_STRUCT& dest, IFR_Length* indicator, char *buffer, IFR_ErrorHndl& error)
    {
        const char *sscanf_str;
        switch(dateformat) {
        case IFR_DateTimeFormat::Normal_C:
            sscanf_str= (char *) "%4hd%2hd%2hd";
            break;
        case IFR_DateTimeFormat::Iso_C:
        case IFR_DateTimeFormat::Jis_C:
        case IFR_DateTimeFormat::WasAnsiNowIsSameAsIso_C:
            sscanf_str= (char *) "%4hd-%2hd-%2hd";
            break;
        default:
            error.setRuntimeError(IFR_ERR_DATETIMEFORMAT_UNSUPPORTED_I, index);
            return IFR_NOT_OK;
        }
        
        char *p = buffer;
        while(*p && (*p == ' '  || *p == '\t' || *p==0x0D || *p==0x0A)) {
            ++p;
        }
        
        int scanned=sscanf(buffer, sscanf_str, &dest.year, &dest.month, &dest.day);
        
        if(scanned!=3) {
            error.setRuntimeError(IFR_ERR_ILLEGAL_DATE_VALUE_I, index);
            return IFR_NOT_OK;
        }
        
        if(!IFRUtil_DateTime::isDateValid(dest)) {
            error.setRuntimeError(IFR_ERR_ILLEGAL_DATE_VALUE_I, index);
            return IFR_NOT_OK;
        }
        
        if(indicator) {
            *indicator = sizeof(SQL_DATE_STRUCT);
        }
        return IFR_OK;
    }
private:
    IFR_Int4 index;
    IFR_Int4 dateformat;
    
};

template<>
class IFRConversion_FromString<SQL_TIME_STRUCT>
{
public:
    typedef char BufferType [64];
    typedef SQL_TIME_STRUCT TargetType;
    
    IFRConversion_FromString(IFR_Int4 _index, IFR_Int4 _dateformat)
    :index(_index),
     dateformat(_dateformat)
    {}
    
    inline IFR_Retcode convert(SQL_TIME_STRUCT& dest, IFR_Length* indicator, char *buffer, IFR_ErrorHndl& error)
    {
        const char *sscanf_str;
        switch(dateformat) {
        case IFR_DateTimeFormat::Normal_C:
            sscanf_str= (char *) "%4hd%2hd%2hd";
            break;
        case IFR_DateTimeFormat::Iso_C:
        case IFR_DateTimeFormat::Jis_C:
        case IFR_DateTimeFormat::WasAnsiNowIsSameAsIso_C:
            sscanf_str= (char *) "%4hd:%2hd:%2hd";
            break;
        default:
            error.setRuntimeError(IFR_ERR_DATETIMEFORMAT_UNSUPPORTED_I, index);
            return IFR_NOT_OK;
        }
        char *p = buffer;
        while(*p && (*p == ' '  || *p == '\t' || *p==0x0D || *p==0x0A)) {
            ++p;
        }
        
        int scanned=sscanf(buffer, sscanf_str, &dest.hour, &dest.minute, &dest.second);
        
        if(scanned!=3) {
            error.setRuntimeError(IFR_ERR_ILLEGAL_TIME_VALUE_I, index);
            return IFR_NOT_OK;
        }
        
        if(!IFRUtil_DateTime::isTimeValid(dest)) {
            error.setRuntimeError(IFR_ERR_ILLEGAL_TIME_VALUE_I, index);
            return IFR_NOT_OK;
        }
        
        if(indicator) {
            *indicator = sizeof(SQL_TIME_STRUCT);
        }
        return IFR_OK;
    }
private:
    IFR_Int4 index;
    IFR_Int4 dateformat;
    
};



//----------------------------------------------------------------------
template<>
class IFRConversion_FromString<SQL_TIMESTAMP_STRUCT>
{
public:
    typedef char BufferType [64];
    typedef SQL_TIMESTAMP_STRUCT TargetType;
    
    IFRConversion_FromString(IFR_Int4 _index, IFR_Int4 _dateformat)
    :index(_index),
     dateformat(_dateformat)
    {}
    
    inline IFR_Retcode convert(SQL_TIMESTAMP_STRUCT& dest, IFR_Length* indicator, char *buffer, IFR_ErrorHndl& error)
    {
        char *p = buffer;
        while(*p && (*p == ' '  || *p == '\t' || *p==0x0D || *p==0x0A)) {
            ++p;
        }
        IFR_size_t buffer_length = strlen(p);
        switch(dateformat) {
        case IFR_DateTimeFormat::Normal_C: {
            if(buffer_length <14) {
                error.setRuntimeError(IFR_ERR_ILLEGAL_TIMESTAMP_VALUE_I, index);
                return IFR_NOT_OK;
            }
            for(IFR_Int4 i=0; i<(IFR_Int4)buffer_length; ++i) {
                if(!ISDIGIT(p[i])) {
                    error.setRuntimeError(IFR_ERR_ILLEGAL_TIMESTAMP_VALUE_I, index);
                    return IFR_NOT_OK;
                }
            }
            dest.year =
                (p[0] - '0') * 1000 +
                (p[1] - '0') * 100 +
                (p[2] - '0') * 10 +
                (p[3] - '0');
            dest.month =
                (p[4] - '0') * 10 +
                (p[5] - '0');
            dest.day =
                (p[6] - '0') * 10 +
                (p[7] - '0');
            
            dest.hour =
                (p[8] - '0') * 10 +
                (p[9] - '0');
            dest.minute =
                (p[10] - '0') * 10 +
                (p[11] - '0');
            dest.second =
                (p[12] - '0') * 10 +
                (p[13] - '0');
            if(buffer_length > 14) {
                p+=14;
                IFR_UInt4 nanos = 0;
                IFR_UInt4 mul=100000000;
                while(*p && ISDIGIT(*p) && mul!=0) {
                    nanos += ((*p)-'0') * mul;
                    mul /= 10;
                    ++p;
                }
                dest.fraction = nanos;
            } else {
                dest.fraction = 0;
            }
            break;
        }
        case IFR_DateTimeFormat::Usa_C:
        case IFR_DateTimeFormat::Eur_C:
        case IFR_DateTimeFormat::TsEur_C:
        case IFR_DateTimeFormat::Iso_C:
        case IFR_DateTimeFormat::Jis_C:
        case IFR_DateTimeFormat::WasAnsiNowIsSameAsIso_C: {
            if(buffer_length <19) {
                error.setRuntimeError(IFR_ERR_ILLEGAL_TIMESTAMP_VALUE_I, index);
                return IFR_NOT_OK;
            }
            
            // this is nasty, but the fastest way to check the
            // syntax rawly.
            if(!ISDIGIT(p[0]) ||
               !ISDIGIT(p[1]) ||
               !ISDIGIT(p[2]) ||
               !ISDIGIT(p[3]) ||
               (p[4] != '-')  ||
               !ISDIGIT(p[5]) ||
               !ISDIGIT(p[6]) ||
               (p[7] != '-')  ||
               !ISDIGIT(p[8]) ||
               !ISDIGIT(p[9]) ||
               !ISSPACE(p[10]) ||
               !ISDIGIT(p[11]) ||
               !ISDIGIT(p[12]) ||
               (p[13] != ':') ||
               !ISDIGIT(p[14]) ||
               !ISDIGIT(p[15]) ||
               (p[16] != ':') ||
               !ISDIGIT(p[17]) ||
               !ISDIGIT(p[18])) {
                error.setRuntimeError(IFR_ERR_ILLEGAL_DATE_VALUE_I, index);
                return IFR_NOT_OK;
            }
            
            dest.year =
                (p[0] - '0') * 1000 +
                (p[1] - '0') * 100 +
                (p[2] - '0') * 10 +
                (p[3] - '0');
            dest.month =
                (p[5] - '0') * 10 +
                (p[6] - '0');
            dest.day =
                (p[8] - '0') * 10 +
                (p[9] - '0');
            
            dest.hour =
                (p[11] - '0') * 10 +
                (p[12] - '0');
            dest.minute =
                (p[14] - '0') * 10 +
                (p[15] - '0');
            dest.second =
                (p[17] - '0') * 10 +
                (p[18] - '0');

            if(buffer_length > 20 && p[19] == '.') {
                p+=20;
                IFR_UInt4 nanos = 0;
                IFR_UInt4 mul=100000000;
                while(*p && ISDIGIT(*p) && mul!=0) {
                    nanos += ((*p)-'0') * mul;
                    mul /= 10;
                    ++p;
                }
                dest.fraction = nanos;
            } else {
                dest.fraction = 0;
            }
            break;
        }
        default:
            error.setRuntimeError(IFR_ERR_DATETIMEFORMAT_UNSUPPORTED_I, index);
            return IFR_NOT_OK;
        }
        if(indicator) {
            *indicator = sizeof(SQL_TIMESTAMP_STRUCT);
        }
        return IFR_OK;
    }
private:
    IFR_Int4 index;
    IFR_Int4 dateformat;
};

#undef ISDIGIT
#undef ISSPACE

template<>
class IFRConversion_FromString<SQL_NUMERIC_STRUCT>
{
public:
    typedef char BufferType[132];
    typedef SQL_NUMERIC_STRUCT TargetType;

    IFRConversion_FromString(IFR_Int4 _index)
    :index(_index)
    {}
    
    inline IFR_Retcode convert(SQL_NUMERIC_STRUCT& dest, IFR_Length* indicator, char *buffer, IFR_ErrorHndl& error)
    {
        IFR_Retcode rc = IFRUtil_SQLNumeric::asciiStringToNumeric(buffer, dest);
        switch(rc) {
        case IFR_NOT_OK:
            error.setRuntimeError(IFR_ERR_INVALID_NUMERIC_VALUE_I, index);
            break;
        case IFR_OVERFLOW:
            error.setRuntimeError(IFR_ERR_NUMERIC_OVERFLOW_I, index);
            break;
        default:
            if(indicator) {
                *indicator = sizeof(SQL_NUMERIC_STRUCT);
            }
        }
        
        return rc;
    }
private:
    IFR_Int4 index;
};

IFR_END_NAMESPACE

#endif
