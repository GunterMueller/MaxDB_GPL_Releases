/*!
  @file           IFR_ConnectProperties.cpp
  @author         D039759
  @ingroup        IFR_Connection
  @brief          Connection properties
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

// Prevents a Warning 5004 Uninitialized variable "static_i"
// from the HP ANSI C++ B3910B X.03.33.01 compiler.
// See http://pts:1080/webpts?wptsdetail=yes&ErrorType=1&ErrorID=1130907 - pragma not longer necessary
// #ifdef HPUX
// #pragma OPT_LEVEL 1
// #endif


#include "RunTime/RTE_IInterface.hpp"
#include "Interfaces/Runtime/IFR_ConnectProperties.h"

//----------------------------------------------------------------------
static IFR_Bool
urlencode(char *dest, IFR_Length destLength, IFR_String& src)
{
    IFR_Length destIdx=0;
    IFR_Length sLength=src.getStrLen();
    const char *s=src.getBuffer();
    for(IFR_Length i=0; i<sLength; ++i) {
        if(s[i] == ' ') {
            dest[destIdx]='+';
            destIdx++;
            if(destIdx == destLength) {
                dest[destIdx-1]='\0';
                return false;
            }
        } else if( (s[i] >= 'A' && s[i]<='Z') ||
                   (s[i] >= 'a' && s[i]<='z') ||
                   (s[i] >= '0' && s[i]<='9') ) {
            dest[destIdx]=s[i];
            destIdx++;
            if(destIdx == destLength) {
                dest[destIdx-1]='\0';
                return false;
            }
        } else {
            unsigned int upper = (((unsigned int) s[i]) & 0xF0) >> 4;
            unsigned int lower = ((unsigned int) s[i]) & 0x0F;
            dest[destIdx] = '%';
            ++destIdx;
            if(destIdx == destLength) {
                dest[destIdx-1]='\0';
                return false;
            }
            if(upper <= 9) {
                dest[destIdx] = '0' + upper;
            } else {
                dest[destIdx] = 'A' + (upper - 10);
            }
            ++destIdx;
            if(destIdx == destLength) {
                dest[destIdx-1]='\0';
                return false;
            }
            if(lower <= 9) {
                dest[destIdx] = '0' + lower;
            } else {
                dest[destIdx] = 'A' + (lower - 10);
            }
            ++destIdx;
            if(destIdx == destLength) {
                dest[destIdx-1]='\0';
                return false;
            }
        }
    }
    return true;
}

//----------------------------------------------------------------------
IFR_ConnectProperties::IFR_ConnectProperties(IFR_Bool& memory_ok)
:allocator(RTE_IInterface::Instance().Allocator()),
 m_dummy(allocator),
 m_properties(allocator, 
              0, 
              m_dummy,
              memory_ok),
 m_parameterstring(IFR_StringEncodingAscii, allocator)
{
}


//----------------------------------------------------------------------
IFR_ConnectProperties::IFR_ConnectProperties(SAPDBMem_IRawAllocator& _allocator, 
                                             IFR_Bool& memory_ok)
:allocator(_allocator),
 m_dummy(allocator),
 m_properties(allocator, 0, m_dummy, memory_ok),
 m_parameterstring(IFR_StringEncodingAscii, allocator)
{
}


//----------------------------------------------------------------------
IFR_ConnectProperties::IFR_ConnectProperties(const IFR_ConnectProperties& copy, IFR_Bool& memory_ok)
:allocator(copy.allocator),
 m_dummy(allocator),
 m_properties(allocator, copy.m_properties, m_dummy, memory_ok),
 m_parameterstring(allocator, copy.m_parameterstring, memory_ok)
{
}

//----------------------------------------------------------------------
IFR_ConnectProperties::IFR_ConnectProperties(const IFR_ConnectProperties& copy,
                                             SAPDBMem_IRawAllocator& _allocator,
                                             IFR_Bool& memory_ok)
:allocator(_allocator),
 m_dummy(allocator),
 m_properties(allocator, copy.m_properties, m_dummy, memory_ok),
 m_parameterstring(allocator, copy.m_parameterstring, memory_ok)
{
}

//----------------------------------------------------------------------
IFR_ConnectProperties::~IFR_ConnectProperties()
{
}


//----------------------------------------------------------------------
IFR_ConnectProperties::Property::Property(const IFR_ConnectProperties::Property& copy, IFR_Bool& memory_ok)
:key(copy.key, memory_ok),
 value(copy.value, memory_ok)
{
}

//----------------------------------------------------------------------
IFR_ConnectProperties::Property::Property(SAPDBMem_IRawAllocator& allocator)
:key(IFR_StringEncodingAscii, allocator),
 value(IFR_StringEncodingAscii, allocator)
{
}

//----------------------------------------------------------------------
void
IFR_ConnectProperties::Property::assign(const IFR_ConnectProperties::Property& assign, IFR_Bool& memory_ok)
{
    if(this != &assign) {
        key.assign(assign.key, memory_ok);
        value.assign(assign.value, memory_ok);
    }
    return;
}


//----------------------------------------------------------------------
void
IFR_ConnectProperties::setProperty(const char *key, const char *value, IFR_Bool& memory_ok)
{
    if(!memory_ok) {
        return;
    }
    
    IFR_Int4 sz=m_properties.GetSize();

    IFR_String key_str(key, IFR_StringEncodingAscii, allocator, memory_ok);
    IFR_String val_str(value, IFR_StringEncodingAscii, allocator, memory_ok);
    IFR_Bool assigned_existing = false;
    for(int i=0; i<sz; ++i) {
        if(IFR_String::compare(m_properties[i].key, key_str, memory_ok)==0) {
            m_properties[i].value.assign(val_str, memory_ok);
            assigned_existing = true;
            break;
        }
        if(!memory_ok) {
            return;
        }
    }
    if(!assigned_existing) {
        Property p(allocator); 
        p.key.assign(key_str, memory_ok); 
        p.value.assign(val_str, memory_ok);
        m_properties.InsertEnd(p, memory_ok);
        if(!memory_ok) {
            return;
        }

        IFR_Length key_len = (key_str.getStrLen() + 1) * 3;
        IFR_Length val_len = (val_str.getStrLen() + 1) * 3;
        
        char *tmp_key=(char *) IFR_ALLOCATOR(allocator).Allocate(key_len);
        if(tmp_key == 0) {
            memory_ok = false;
            return;
        }
        char *tmp_val=(char *) IFR_ALLOCATOR(allocator).Allocate(val_len);
        if(tmp_val==0) {
            memory_ok=false;
            IFR_ALLOCATOR(allocator).Deallocate(tmp_key);
            return;
        }
        
        memset(tmp_key, 0, key_len);
        memset(tmp_val, 0, val_len);
        
        IFR_Bool key_success = urlencode(tmp_key, key_len, key_str);
        IFR_Bool val_success = urlencode(tmp_val, val_len, val_str);
        
        if(m_parameterstring.getStrLen() != 0) {
            m_parameterstring.append("&", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        }
        m_parameterstring.append(tmp_key, IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        m_parameterstring.append("=", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        m_parameterstring.append(tmp_val, IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        IFR_ALLOCATOR(allocator).Deallocate(tmp_key);
        IFR_ALLOCATOR(allocator).Deallocate(tmp_val);
    } else {
        m_parameterstring.clear();
        IFR_size_t sz = m_properties.GetSize();
        IFR_Length key_len;
        IFR_Length val_len;
        char *tmp_key;
        char *tmp_val;
        IFR_Bool key_success;
        IFR_Bool val_success;
        
        for(IFR_size_t i=0; i<sz; ++i) {
            key_len = (m_properties[i].key.getStrLen() + 1) * 3;
            val_len = (m_properties[i].value.getStrLen() + 1) * 3;
        
            tmp_key=(char *) IFR_ALLOCATOR(allocator).Allocate(key_len);
            if(tmp_key == 0) {
                memory_ok = false;
                return;
            }
            
            tmp_val=(char *) IFR_ALLOCATOR(allocator).Allocate(val_len);
            if(tmp_val==0) {
                memory_ok=false;
                IFR_ALLOCATOR(allocator).Deallocate(tmp_key);
                return;
            }
        
            memset(tmp_key, 0, key_len);
            memset(tmp_val, 0, val_len);
        
            key_success = urlencode(tmp_key, key_len, m_properties[i].key);
            val_success = urlencode(tmp_val, val_len, m_properties[i].value);
            
            if(key_success && val_success) {
                if(m_parameterstring.getStrLen() != 0) {
                    m_parameterstring.append("&", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
                }
                m_parameterstring.append(tmp_key, IFR_StringEncodingAscii, IFR_NTS, memory_ok);
                m_parameterstring.append("=", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
                m_parameterstring.append(tmp_val, IFR_StringEncodingAscii, IFR_NTS, memory_ok);
            }
            IFR_ALLOCATOR(allocator).Deallocate(tmp_key);
            IFR_ALLOCATOR(allocator).Deallocate(tmp_val);
        }
    }
    return;
}


//----------------------------------------------------------------------
const char *
IFR_ConnectProperties::getProperty(const char *key, const char *defaultvalue) const
{
    IFR_Int4 sz=m_properties.GetSize();
    for(int i=0; i<sz; ++i) {
        if(m_properties[i].key.equalsAscii(key)) {
            return m_properties[i].value.getBuffer();
        }
    }
    return defaultvalue;
}

//----------------------------------------------------------------------
void
IFR_ConnectProperties::sqlTrace(IFR_TraceStream& s)
{
    IFR_Int4 sz=m_properties.GetSize();
    
    if(sz == 0) {
        s << "CONNECT PROPERTIES: (empty)" << endl;
    } else {
        if(sz == 1) {
            s << "CONNECT PROPERTIES: (1 entry)" << endl;
        } else {
            s << "CONNECT PROPERTIES: (" << sz << ")" << endl;
        }
        for(IFR_Int4 i=0; i<sz; ++i) {
            s << "  KEY: " << m_properties[i].key << " VALUE: " << m_properties[i].value << endl;
        }
    }
    return;
}

//----------------------------------------------------------------------
void
IFR_ConnectProperties::clear()
{
    m_properties.Clear();
    m_parameterstring.clear();
    return;
}
