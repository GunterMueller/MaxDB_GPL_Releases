/*!
  @file           IFR_String.h
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Common
  @brief          handling strings with encoding and length
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
#include "SAPDB/Interfaces/Runtime/IFR_String.h"
#include "SAPDB/Interfaces/Runtime/IFR_Trace.h"
#include "SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_Alloca.h"

#define UNDEFLEN (0)

//----------------------------------------------------------------------
IFR_String::IFR_String(SAPDBMem_IRawAllocator& allocator)
:m_allocator(allocator),
 m_buffer(0),
 m_bufsize(0),
 m_length_in_bytes(0),
 m_encoding_type(IFR_StringEncodingAscii),
 m_strlen(UNDEFLEN)
{
}

//----------------------------------------------------------------------
IFR_String::IFR_String(IFR_StringEncoding encoding, SAPDBMem_IRawAllocator& allocator)
:m_allocator(allocator),
 m_buffer(0),
 m_bufsize(0),
 m_length_in_bytes(0),
 m_encoding_type(encoding),
 m_strlen(UNDEFLEN)
{
}


//----------------------------------------------------------------------
IFR_String::~IFR_String()
{
    if(m_buffer) {
        IFR_ALLOCATOR(m_allocator).Deallocate(m_buffer);
    }
}


//----------------------------------------------------------------------
IFR_String::IFR_String(const IFR_String& src, IFR_Bool& memory_ok)
:m_allocator(src.m_allocator),
 m_length_in_bytes(src.m_length_in_bytes),
 m_bufsize(src.m_bufsize),
 m_encoding_type(src.m_encoding_type),
 m_strlen(src.m_strlen)
{
    //<<< MEMCHECK
    if(!memory_ok) {
        goto MEMORY_ERROR;
    }
    //>>> MEMCHECK
    
    if(m_bufsize==0) {
        m_buffer=0;
    } else {
        m_buffer=(char*) IFR_ALLOCATOR(m_allocator).Allocate(m_bufsize);
        //<<< MEMCHECK
        if(m_buffer == 0) {
            memory_ok = false;
            goto MEMORY_ERROR;
        }
        //>>> MEMCHECK
        memcpy(m_buffer, src.getBuffer(), m_bufsize);
    }
    return;
//>>> MEMCHECK
MEMORY_ERROR:
    m_length_in_bytes = 0;
    m_bufsize = 0;
    m_strlen = 0;
    m_buffer = 0;
    return;
//<<< MEMCHECK
}


//----------------------------------------------------------------------
IFR_String::IFR_String(SAPDBMem_IRawAllocator& allocator, 
                       const IFR_String& src,
                       IFR_Bool& memory_ok)
:m_allocator(allocator),
 m_length_in_bytes(src.m_length_in_bytes),
 m_bufsize(src.m_bufsize),
 m_encoding_type(src.m_encoding_type),
 m_strlen(src.m_strlen)
{
    //<<< MEMCHECK
    if(!memory_ok) {
        goto MEMORY_ERROR;
    }
    //>>> MEMCHECK
    
    if(m_bufsize==0) {
        m_buffer=0;
    } else {
        m_buffer=(char*) IFR_ALLOCATOR(m_allocator).Allocate(m_bufsize);
        //<<< MEMCHECK
        if(m_buffer == 0) {
            memory_ok = false;
            goto MEMORY_ERROR;
        }
        //>>> MEMCHECK
        memcpy(m_buffer, src.getBuffer(), m_bufsize);
    }
    return;
//>>> MEMCHECK
MEMORY_ERROR:
    m_length_in_bytes = 0;
    m_bufsize = 0;
    m_strlen = 0;
    m_buffer = 0;
    return;
//<<< MEMCHECK
}

//----------------------------------------------------------------------
inline IFR_Bool
emptystring(const char *buf, 
            IFR_StringEncoding encoding, 
            IFR_Length length=-1)
{
    if(length <0 || length >=2) {
        switch(encoding) {
            case IFR_StringEncodingUCS2:
            case IFR_StringEncodingUCS2Swapped:
                return *buf==0 && *(buf + 1)==0;
            case IFR_StringEncodingAscii:
            case IFR_StringEncodingUTF8:
            default:
                return *buf == 0;
        }
    } else {
        switch(encoding) {
            case IFR_StringEncodingUCS2:
            case IFR_StringEncodingUCS2Swapped:
                return true; // because 1 is too short for Unicode
            case IFR_StringEncodingAscii:
            case IFR_StringEncodingUTF8:
            default:
                return (length==1) && (*buf == 0);
        }
    }
}

//----------------------------------------------------------------------
IFR_String::IFR_String(const char *buf,
                       IFR_StringEncoding encoding,
                       SAPDBMem_IRawAllocator& allocator,
                       IFR_Bool& memory_ok)
:m_allocator(allocator),
 m_encoding_type(encoding),
 m_strlen(UNDEFLEN)
{
    //<<< MEMCHECK
    if(!memory_ok) {
        goto MEMORY_ERROR;
    }
    //>>> MEMCHECK

    if(buf==0 || emptystring(buf, encoding)) {
        m_buffer = 0;
        m_length_in_bytes = 0;  
        m_bufsize = 0;
    } else {
        // need an analyze run on buffer.
        unsigned int bytelength;
        int terminated;
        int corrupted;
        int exhausted;
        IFR_UInt4 tmp_strlen;
        
        IFR_ENCODING(m_encoding_type)->stringInfo(buf, MAX_IFR_UINT4, 1, &tmp_strlen, &bytelength, &terminated, 
                                                  &corrupted, &exhausted);
        m_strlen = tmp_strlen;
        m_bufsize=m_length_in_bytes=bytelength;
        m_bufsize+=IFR_ENCODING(m_encoding_type)->terminatorSize;
        m_buffer=(char*)IFR_ALLOCATOR(m_allocator).Allocate(m_bufsize);
        //<<< MEMCHECK
        if(m_buffer == 0) {
            memory_ok = false;
            goto MEMORY_ERROR;
        }
        //>>> MEMCHECK
        memcpy(m_buffer, buf, m_bufsize);
    }
    return;
//>>> MEMCHECK
MEMORY_ERROR:
    m_length_in_bytes = 0;
    m_bufsize = 0;
    m_strlen = 0;
    m_buffer = 0;
    return;
//<<< MEMCHECK
}

//----------------------------------------------------------------------
IFR_String::IFR_String(const char *buf,
                       IFR_Length length,
                       IFR_StringEncoding encoding,
                       SAPDBMem_IRawAllocator& allocator,
                       IFR_Bool& memory_ok)
:m_allocator(allocator),
 m_encoding_type(encoding),
 m_strlen(UNDEFLEN)
{
    //<<< MEMCHECK
    if(!memory_ok) {
        goto MEMORY_ERROR;
    }
    //>>> MEMCHECK
    
    if(buf==0 || emptystring(buf, encoding, length)) {
        m_buffer = 0;
        m_length_in_bytes = 0;
        m_bufsize = 0;
    } else {
        // need an analyze run on buffer.
        unsigned int bytelength;
        int terminated;
        int corrupted;
        int exhausted;
        // correct 'unlimited' length for stringInfo (IFR_NTS is -3)
        if(length < 0) {
            length = -1;
        }
        IFR_UInt4 tmp_strlen;
        IFR_ENCODING(m_encoding_type)->stringInfo(buf, length, 1, &tmp_strlen, &bytelength, &terminated, &corrupted, &exhausted);
        m_strlen = tmp_strlen;
        m_bufsize=m_length_in_bytes=bytelength;
        m_bufsize+=IFR_ENCODING(m_encoding_type)->terminatorSize;
        m_buffer=(char*)IFR_ALLOCATOR(m_allocator).Allocate(m_bufsize);
        //<<< MEMCHECK
        if(m_buffer == 0) {
            memory_ok = false;
            goto MEMORY_ERROR;
        }
        //>>> MEMCHECK
        if(terminated) {
            memcpy(m_buffer, buf, m_bufsize);
        } else {
            memcpy(m_buffer, buf, m_length_in_bytes);
            memcpy(m_buffer+m_length_in_bytes,
                   IFR_ENCODING(m_encoding_type)->charTable->terminator.bytes,
                   IFR_ENCODING(m_encoding_type)->charTable->terminator.byteCount);
        }
    }
    return;
//>>> MEMCHECK
MEMORY_ERROR:
    m_length_in_bytes = 0;
    m_bufsize = 0;
    m_strlen = 0;
    m_buffer = 0;
    return;
//<<< MEMCHECK
}

//----------------------------------------------------------------------
void
IFR_String::setBuffer(const char *buf,
                      IFR_Length length,
                      IFR_StringEncoding encoding,
                      IFR_Bool& memory_ok)
{
    //<<< MEMCHECK
    if(!memory_ok) {
        return;
    }
    //>>> MEMCHECK
    
    m_encoding_type=encoding;

    // need an analyze run on buffer.
    unsigned int bytelength;
    int terminated;
    int corrupted;
    int exhausted;
    IFR_UInt4 tmp_strlen;
    IFR_ENCODING(m_encoding_type)->stringInfo(buf, length, 1, &tmp_strlen, &bytelength, &terminated, &corrupted, &exhausted);
    m_strlen = tmp_strlen;
    
    IFR_Length bufferlength=bytelength+IFR_ENCODING(m_encoding_type)->terminatorSize;

    if(bufferlength > m_bufsize) {
        if(m_buffer) {
            IFR_ALLOCATOR(m_allocator).Deallocate(m_buffer);
        }
        m_buffer = (char*) IFR_ALLOCATOR(m_allocator).Allocate(bufferlength);
        //<<< MEMCHECK
        if(!m_buffer) {
            memory_ok         = false;
            m_bufsize         = 0;
            m_length_in_bytes = 0;
            m_strlen          = UNDEFLEN;
            return;
        }
        //>>> MEMCHECK
	    m_bufsize = bufferlength;
    }

    m_length_in_bytes=bytelength;
    if(terminated) {
        memcpy(m_buffer, buf, bufferlength);
    } else {
        memcpy(m_buffer, buf, m_length_in_bytes);
        memcpy(m_buffer+m_length_in_bytes,
               IFR_ENCODING(m_encoding_type)->charTable->terminator.bytes,
               IFR_ENCODING(m_encoding_type)->charTable->terminator.byteCount);
    }
    return;
}

//----------------------------------------------------------------------
IFR_Length
IFR_String::getStrLen() const
{
    // if string length is 0 because the string is empty, this is not
    // much overhead
    if(m_strlen==UNDEFLEN && m_buffer!=0) {
        unsigned int bytelength;
        int terminated;
        int corrupted;
        int exhausted;
        IFR_UInt4 tmp_strlen;
        IFR_ENCODING(m_encoding_type)->stringInfo(m_buffer, m_bufsize, 1, &tmp_strlen, 
                                                  &bytelength, &terminated, &corrupted, &exhausted);
        
        const_cast<IFR_Length&>(m_strlen) = tmp_strlen;
    }
    return m_strlen;
}



//----------------------------------------------------------------------
void
IFR_String::assign(const IFR_String& src, IFR_Bool& memory_ok)
{
    if(this == &src) {
        return;
    }
    //<<< MEMCHECK
    if(!memory_ok) {
        if(m_buffer) {
            IFR_ALLOCATOR(m_allocator).Deallocate(m_buffer);
            m_buffer = 0;
        }
        m_bufsize         = 0;
        m_length_in_bytes = 0;
        m_strlen          = UNDEFLEN;
    }
    //>>> MEMCHECK
    
    if(src.m_bufsize > m_bufsize) {
        if(m_buffer) IFR_ALLOCATOR(m_allocator).Deallocate(m_buffer);
        m_buffer = (char *) IFR_ALLOCATOR(m_allocator).Allocate(src.m_bufsize);
        //<<< MEMCHECK
        if(!m_buffer) {
            memory_ok         = false;
            m_bufsize         = 0;
            m_length_in_bytes = 0;
            m_strlen          = UNDEFLEN;
            return;
        }
        //>>> MEMCHECK
        m_bufsize=src.m_bufsize;
    }

    memcpy(m_buffer, src.getBuffer(), src.m_bufsize);
    m_length_in_bytes=src.m_length_in_bytes;
    m_encoding_type=src.m_encoding_type;
    m_strlen=src.m_strlen;
    return;
}


//----------------------------------------------------------------------
IFR_Bool 
IFR_String::append(const IFR_String& other, IFR_Bool& memory_ok)
{
    //<<< MEMCHECK
    if(!memory_ok) {
        return false;
    }
    //>>> MEMCHECK

    if(other.m_buffer == 0) {
        return true;
    }

    if(this == &other) {
        IFR_String tmp(other, memory_ok);
        //<<< MEMCHECK
        if(!memory_ok) {
            return false;
        }
        //>>> MEMCHECK
        return append(tmp, memory_ok);
    }
    
    if(m_encoding_type != other.m_encoding_type) {
        // what to do now:
        // - get the other encoding and buffer, and recode it to this encoding
        // - append the recoded stuff.
        IFR_Length additional_size=other.m_length_in_bytes * 4 + IFR_ENCODING(m_encoding_type)->terminatorSize;
        expand(additional_size+m_length_in_bytes, memory_ok);
        //<<< MEMCHECK
        if(!memory_ok) {
            return false;
        }
        //>>> MEMCHECK
        IFR_UInt4 destbyteswritten;
        IFR_UInt4 srcbytesparsed;
        tsp78ConversionResult rc =
        sp78convertString(IFR_ENCODING(m_encoding_type),
                          m_buffer+m_length_in_bytes,
                          additional_size,
                          &destbyteswritten,
                          1,
                          IFR_ENCODING(other.m_encoding_type),
                          other.getBuffer(),
                          other.m_length_in_bytes,
                          &srcbytesparsed);
        // don't know the strlen now
        m_strlen=UNDEFLEN; // can set to 0 because getStrLen will return the correct len
        m_length_in_bytes+=destbyteswritten-IFR_ENCODING(m_encoding_type)->terminatorSize;
        if (rc == sp78_Ok)
          return true;
        else 
          return false;
    } else {
        IFR_Length overall_size=m_length_in_bytes 
            + other.m_length_in_bytes + IFR_ENCODING(m_encoding_type)->terminatorSize;		
        expand(overall_size, memory_ok);
        //<<< MEMCHECK
        if(!memory_ok) {
            return false;
        }
        //>>> MEMCHECK
        memcpy(m_buffer + m_length_in_bytes,
               other.getBuffer(),
               overall_size - m_length_in_bytes);
        if (m_strlen != UNDEFLEN)
          m_strlen += other.m_strlen;
        m_length_in_bytes+=other.m_length_in_bytes;
        return true;
    }
}

//----------------------------------------------------------------------
IFR_Bool 
IFR_String::append(const char *to_add, 
                   IFR_StringEncoding encoding,
                   IFR_size_t length,
                   IFR_Bool& memory_ok)
{
    //<<< MEMCHECK
    if(!memory_ok) {
        return false;
    }
    //>>> MEMCHECK
    
    if(length==IFR_NTS) {
        IFR_String tmp(to_add, encoding, getAllocator(), memory_ok);
        //<<< MEMCHECK
        if(!memory_ok) {
            return false;
        }
        //>>> MEMCHECK
        return append(tmp, memory_ok);
    } else {
        IFR_String tmp(to_add, length, encoding, getAllocator(), memory_ok);
        //<<< MEMCHECK
        if(!memory_ok) {
            return false;
        }
        //>>> MEMCHECK
        return append(tmp, memory_ok);
    }
}

//----------------------------------------------------------------------
void
IFR_String::expand(IFR_Length newbufsize, IFR_Bool& memory_ok)
{
    if(!memory_ok) {
        return;
    }
    
    if(m_bufsize >= newbufsize) {
        return;
    }
    IFR_Length usefornewbuf=32;
    IFR_Int4  c=0;
    while(c < 26 && usefornewbuf < newbufsize) {
        ++c;
        usefornewbuf = usefornewbuf << 1;
    }
    if(usefornewbuf < newbufsize) {
        usefornewbuf=newbufsize;
    }
    char *tmp_buffer=(char*)IFR_ALLOCATOR(m_allocator).Allocate(usefornewbuf);
    // give it another try 
    if(tmp_buffer == 0) {
        usefornewbuf = newbufsize;
        tmp_buffer=(char*)IFR_ALLOCATOR(m_allocator).Allocate(usefornewbuf);
        //<<< MEMCHECK
        if(!tmp_buffer) {
            if(m_buffer) {
                IFR_ALLOCATOR(m_allocator).Deallocate(m_buffer);
                m_buffer = 0;
            }
            m_bufsize         = 0;
            m_length_in_bytes = 0;
            m_strlen          = UNDEFLEN;
            memory_ok         = false;
            return;
        }
    }
    if(m_buffer) {
        memcpy(tmp_buffer, m_buffer, m_bufsize);
        IFR_ALLOCATOR(m_allocator).Deallocate(m_buffer);
    }
    m_buffer=tmp_buffer;
    m_bufsize=usefornewbuf;
    return;
}


//----------------------------------------------------------------------
IFR_Int4
IFR_String::compare(const IFR_String& a, const IFR_String& b, IFR_Bool& memory_ok)
{
    //<<< MEMCHECK
    if(!memory_ok) {
        return 1;
    }
    //>>> MEMCHECK
    
    if(a.getEncoding() == b.getEncoding()) {
        IFR_size_t comparelength = a.getLength();
        if(b.getLength() < comparelength) {
            comparelength = b.getLength();
        }
        // Due to 0-termination, this will return correct results
        // because of the terminator of the shorter string
        return memcmp(a.getBuffer(), b.getBuffer(), comparelength + IFR_ENCODING(a.getEncoding())->terminatorSize);
    } else {
        unsigned int a_charlength;
        unsigned int a_bytelength;
        int terminated;
        int corrupted;
        int exhausted;
        IFR_ENCODING(a.getEncoding())->stringInfo(a.getBuffer(), a.getLength(), 1, &a_charlength, 
                                                  &a_bytelength, &terminated, &corrupted, &exhausted);
        unsigned int b_charlength;
        unsigned int b_bytelength;
        IFR_ENCODING(b.getEncoding())->stringInfo(b.getBuffer(), b.getLength(), 1, &b_charlength, 
                                                  &b_bytelength, &terminated, &corrupted, &exhausted);
        unsigned int compare_length;
        if(a_charlength <= b_charlength) {
            compare_length=a_charlength;
        } else {
            compare_length=b_charlength;
        }
        
        // now deduce the outputenc
        IFR_StringEncoding destencoding;

        // we want to avoid that more than one must be converted,
        // and that bytes are lost
        if(a.getEncoding() == IFR_StringEncodingAscii) {
            destencoding = b.getEncoding();
            IFR_size_t abuffer_length=a_bytelength * 4 + 2; // be safe for UTF8
            char *abuffer=(char *)alloca(abuffer_length);
            //>>> MEMCHECK
            if(abuffer == 0) {
                memory_ok = false;
                return 1;
            }
            //<<< MEMCHECK
            IFR_UInt4 destbyteswritten;
            IFR_UInt4 srcbytesparsed;
            sp78convertString(IFR_ENCODING(destencoding),
                              abuffer,
                              a_bytelength * 4 + 2,
                              &destbyteswritten,
                              1,
                              IFR_ENCODING(IFR_StringEncodingAscii),
                              a.getBuffer(),
                              a_bytelength,
                              &srcbytesparsed);
            if(destbyteswritten != b_bytelength) {
                return false;
            }
            int result = memcmp(abuffer, b.getBuffer(), compare_length);
            if(result==0) {
                if(a_bytelength < b_bytelength) {
                    result=-1;
                } 
                if(a_bytelength > b_bytelength) {
                    result=1;
                }
            }
            return result;
        } else {
            destencoding = a.getEncoding();
            IFR_size_t bbuffer_len=b_bytelength*4+2; // be safe for UTF8
            char *bbuffer=(char *)alloca(bbuffer_len);
            //>>> MEMCHECK
            if(bbuffer == 0) {
                memory_ok = false;
                return 1;
            }
            //<<< MEMCHECK
            IFR_UInt4 destbyteswritten;
            IFR_UInt4 srcbytesparsed;
            sp78convertString(IFR_ENCODING(destencoding),
                              bbuffer,
                              b_bytelength * 4 + 2,
                              &destbyteswritten,
                              1,
                              IFR_ENCODING(IFR_StringEncodingAscii),
                              b.getBuffer(),
                              b_bytelength,
                              &srcbytesparsed);
            if(destbyteswritten != a_bytelength) {
                return false;
            }
            int result = memcmp(bbuffer, a.getBuffer(), compare_length);
            if(result==0) {
                if(a_bytelength < b_bytelength) {
                    result=-1;
                } 
                if(a_bytelength > b_bytelength) {
                    result=1;
                }
            }
            return result;
        }
    }
}

//----------------------------------------------------------------------
IFR_Bool
IFR_String::equalsAscii(const char *str) const 
{
    if(getEncoding() != IFR_StringEncodingAscii) {
        return false;
    } else {
        return strcmp(getBuffer(), str) == 0;
    }
}

//----------------------------------------------------------------------
IFR_Bool
IFR_String::equalsWithEncoding(const IFR_String& a, const IFR_String& b)
{
    if(a.getEncoding() != b.getEncoding() ||
       a.getLength() != b.getLength()) {
        return false;
    } else {
        return memcmp((void *) a.getBuffer(), b.getBuffer(), b.getLength()) == 0;
    }
}



//----------------------------------------------------------------------
IFR_Int4
IFR_String::hashCode() const
{
    // the X31 hash formula is hash = (hash<<5) - hash + char(i) for i in 1 ... string length
    // as it degenerates when the input are 0's, a little bit decoration is added
    // to hash UTF8 data and UCS2 data equally. 
    // also chars > 128 are completely skipped.
    if(m_length_in_bytes==0) {
        return 0;
    } else {
        IFR_Int4 result=0;
        char *buffer=m_buffer;
        char *endbuffer=m_buffer+m_length_in_bytes;
        while(buffer != endbuffer) {
            if(*buffer && ((*buffer&0x80)==0)) {
                result = (result << 5) - result + (*buffer);
            }
            ++buffer;
        }
        return result;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_String::copyInto(char               *buffer,
                     IFR_StringEncoding& encoding,
                     IFR_Length          bufferSize,
                     IFR_Length          *bufferLength) const
{
    IFR_Length terminatorSize=IFR_ENCODING(m_encoding_type)->terminatorSize;

    if(bufferSize < terminatorSize) {
        if(bufferLength) {
            (*bufferLength) = m_length_in_bytes;
        }
        return IFR_DATA_TRUNC;
    }

    IFR_Length copyLength = m_length_in_bytes;
    if(copyLength + terminatorSize > bufferSize) {
        copyLength = bufferSize-terminatorSize;
    }
    // copy
    memcpy(buffer, getBuffer(), copyLength);
    encoding = m_encoding_type;
    
    // terminate
    for(IFR_Length i=0; i<terminatorSize; ++i) {
        buffer[copyLength + i] = '\0';
    }
    
    if(copyLength < (IFR_Length) m_length_in_bytes) {
        if(bufferLength)
            *bufferLength = m_length_in_bytes;    
        return IFR_DATA_TRUNC;
    } else {
        if(bufferLength)
            *bufferLength = copyLength;    
        return IFR_OK;
    }
}

//----------------------------------------------------------------------
IFR_Retcode
IFR_String::convertInto(char               *buffer,
                        IFR_StringEncoding  encoding,
                        IFR_Length          bufferSize,
                        IFR_Length         *bufferLength) const
{
    IFR_Length terminatorSize=IFR_ENCODING(encoding)->terminatorSize;
    if(bufferSize < terminatorSize) {
        if(bufferLength) {
            switch(encoding) {
            case IFR_StringEncodingAscii:
                switch(m_encoding_type) {
                case IFR_StringEncodingAscii:
                case IFR_StringEncodingUTF8:
                    (*bufferLength) = m_length_in_bytes;
                    break;
                case IFR_StringEncodingUCS2Swapped:
                case IFR_StringEncodingUCS2:
                    (*bufferLength) = m_length_in_bytes/2;
                    break;
                default:
                    (*bufferLength) = IFR_NO_TOTAL;
                }
                break;
            case IFR_StringEncodingUTF8:
                switch(m_encoding_type) {
                case IFR_StringEncodingAscii:
                case IFR_StringEncodingUTF8:
                    (*bufferLength) = m_length_in_bytes;
                    break;
                case IFR_StringEncodingUCS2Swapped:
                case IFR_StringEncodingUCS2:
                    (*bufferLength) = m_length_in_bytes * 2;
                    break;
                default:
                    (*bufferLength) = IFR_NO_TOTAL;
                }
                break;
            case IFR_StringEncodingUCS2:
            case IFR_StringEncodingUCS2Swapped:
                switch(m_encoding_type) {
                case IFR_StringEncodingAscii:
                    (*bufferLength) = m_length_in_bytes * 2;
                    break;
                case IFR_StringEncodingUTF8:
                    (*bufferLength) = IFR_NO_TOTAL;
                    break;
                case IFR_StringEncodingUCS2Swapped:
                case IFR_StringEncodingUCS2:
                    (*bufferLength) = m_length_in_bytes;
                    break;
                default:
                    (*bufferLength) = IFR_NO_TOTAL;
                }
                break;
            }
        }
        return IFR_DATA_TRUNC;
    }
    
    if(m_encoding_type == encoding) {
        return copyInto(buffer, encoding, bufferSize, bufferLength);
    } else {
        IFR_UInt4 destbyteswritten;
        IFR_UInt4 srcbytesparsed;
        tsp78ConversionResult rc = sp78convertString(IFR_ENCODING(encoding),
                                                     buffer,
                                                     bufferSize,
                                                     &destbyteswritten,
                                                     true,
                                                     IFR_ENCODING(m_encoding_type),
                                                     getBuffer(),
                                                     m_length_in_bytes,
                                                     &srcbytesparsed);
        switch(rc) {
        case sp78_Ok: {
          if(bufferLength)
            *bufferLength = destbyteswritten-terminatorSize;
          return IFR_OK;
        }
        case sp78_TargetNotTerminated: {
          // terminate
          for(IFR_Length i=0; i<terminatorSize; ++i) {
            buffer[destbyteswritten-terminatorSize + i] = '\0';
          }
        }
        // fall through
        case sp78_TargetExhausted: {
          if (encoding != IFR_StringEncodingAscii) {
            if(bufferLength)
              *bufferLength = m_length_in_bytes*2;
          }
          else {
            if(bufferLength)
              *bufferLength = m_length_in_bytes/2;
          }
          return IFR_DATA_TRUNC;
        }
        default:
            return IFR_NOT_OK;
        }
    }
}


