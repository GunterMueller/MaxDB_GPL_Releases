/*!
  @file           IFR_Trace.cpp
  @author         D039759
  @ingroup        IFR_Trace
  @brief          Tracing and logging
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

#include "Interfaces/Runtime/IFR_Types.h"
#include "Interfaces/Runtime/IFR_Trace.h"
#include "Interfaces/Runtime/Util/IFRUtil_Time.h"

IFR_BEGIN_NAMESPACE


// The first call to the checkTraceUpdate method will possibly set
// this to false, until then we assume there is at least one trace
IFR_Bool ifr_dbug_trace = true;

//----------------------------------------------------------------------
// The 'PIn' package refers to this (but does not use it).
PIn_TraceStream * PIn_GlobalTrace = 0;


//----------------------------------------------------------------------
//Dummy class to make the <code>PIn_TraceStream</code> work.
class IFR_Trace_DummyOutputStream
    : public isp12_OutputStream
{
public:
    virtual  ~IFR_Trace_DummyOutputStream () {}
    virtual int write (const void * buf, int len) { return 0; }
    virtual int writeln (const void * buf, int len = -1) { return 0; }
    virtual int flush () { return 0; }
    virtual int close () { return 0; }
};

//----------------------------------------------------------------------
SAPDBMEM_STATIC_RAW_ALLOCATE(ifrtrace_dummystream_memory, 
                             sizeof(IFR_Trace_DummyOutputStream));


//----------------------------------------------------------------------
static const char hexchar[] ={ '0', '1', '2', '3', '4', '5', '6', '7', 
                         '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' 
};

//----------------------------------------------------------------------
static inline void hexifystr(const char *dest, 
                             const char *src, 
                             int srclen)
{
    char *p=(char*)dest;
    for(int i=0; i<srclen; ++i) {
        *p=hexchar[ (src[i] >> 4) & 0xF ];
        ++p;
        *p=hexchar[ (src[i]) & 0xF ];
        ++p;
    }
    return;
}

#define TRACE_NULL_CHECK if(this == 0) { return *this; }

//----------------------------------------------------------------------
IFR_TraceStream& 
IFR_TraceStream::operator << (const char *sb)
{
    TRACE_NULL_CHECK;
    IFR_StringEncoding inputenc = (IFR_StringEncoding)0;
    IFR_Int4 inputlengthc = IFR_NTS;
    IFR_Bool  hexc = false;
    
    SQLDBC_IRuntime::TaskTraceContext *controller_traceflags=m_controller ? m_controller->traceflags() : 0;
    
    if(controller_traceflags) {
        inputenc     = controller_traceflags->encoding;
        inputlengthc = controller_traceflags->inputlength;
        hexc         = controller_traceflags->hex; 
    }
    
    if(inputlengthc == 0) {
        if(m_controller) {
            m_controller->clearFlags();
        }
        return *this;
    }
    
    // sanity check: If we have an invalid encoding, we fall back to ASCII.
    if( ((int)inputenc) < 1 ||
        ((int)inputenc) > 4) {
        inputenc = IFR_StringEncodingAscii;
        if(m_controller && controller_traceflags) {
            controller_traceflags->encoding = IFR_StringEncodingAscii;
        }
    }

    IFR_UInt charLength;
    IFR_UInt byteLength;
    IFR_Int isTerminated, isCorrupted, isExhausted;
    char *s = (char*)sb;
    if (!s) {
      s = (char *) "(null)";
      inputenc = IFR_StringEncodingAscii;
      inputlengthc=IFR_NTS;
    }
    if(inputlengthc == IFR_NTS) {
        if(inputenc == IFR_StringEncodingAscii) {
            charLength = byteLength = (IFR_UInt)strlen(s);
            isTerminated = 1;
            isExhausted = 0;
            isCorrupted = 0;
        } else {
            IFR_ENCODING(inputenc)->stringInfo(s, MAX_IFR_INT4, 0, 
                                               &charLength, &byteLength, &isTerminated, 
                                               &isCorrupted, &isExhausted);
        }
    } else {
        byteLength = inputlengthc;
    }
    
    if(hexc) {
      IFR_Length bytes2parse = byteLength;
      char buf[128];
      do {      
        IFR_Length len=MIN(64, bytes2parse);
        hexifystr(buf, s, len);
        print(buf, len*2);
        bytes2parse -= len;
        s += len;
        if (bytes2parse > 0)
          println("\\", 1);
      } while(bytes2parse > 0);
    } else {
      char buf[128];
      tsp78ConversionResult rc = sp78_Ok;
      IFR_UInt4 destbyteswritten;
      IFR_UInt4 srcbytesparsed=0;
      IFR_Length limit = inputlengthc;
      IFR_Length bytes2parse = byteLength;
      bytes2parse = (limit == IFR_NTS) ? bytes2parse : MIN(limit, bytes2parse);
      const char *src = s;
      if(m_controller) {
          m_controller->clearFlags();
      }

      do {
        rc = sp78convertString(IFR_ENCODING(IFR_StringEncodingUTF8), buf, sizeof(buf),
                               &destbyteswritten, false,
                               IFR_ENCODING(inputenc), src, bytes2parse,
                               &srcbytesparsed);
        src += srcbytesparsed;
        bytes2parse -= srcbytesparsed;
        if (rc == sp78_TargetNotTerminated) {
          rc = sp78_Ok;
        }
        if (rc == sp78_Ok) {
          print(buf, destbyteswritten);
        }
        else {
          if (rc == sp78_TargetExhausted) {
            print(buf, destbyteswritten);
            if (bytes2parse > 0)
              println("\\", 1);
          } else {
            if (rc==sp78_SourceExhausted && destbyteswritten > 0) {
              print(buf, destbyteswritten);
              println("***", -1);
            }
            IFR_Length pos = src-s;
            print("ERROR: ", -1);
            print(sp78errString(rc), -1);
            print(" at ", -1);
            (*this) << src-s;
            print(" '0x", -1);
            (*this) << ::hex << inputlength(MIN(bytes2parse, 10)) << src;
            println("'", -1); 
            break;
          }
        }
      } while(bytes2parse > 0);
    }
    if(m_controller) {
        m_controller->clearFlags();
    }
    return *this;
}



//----------------------------------------------------------------------
IFR_TraceStream&
IFR_TraceStream::operator <<(const IFR_Int1 i)
{
    TRACE_NULL_CHECK;
    int l;
    char s[64];
    SQLDBC_IRuntime::TaskTraceContext *controller_traceflags=m_controller ? m_controller->traceflags() : 0;
    if(controller_traceflags && controller_traceflags->hex) {
        l=IFR_Trace_sprintf(s, 64, "%X", (IFR_UInt4)i);
    } else {
        l=IFR_Trace_sprintf(s, 64, "%c", i);
    }
    print(s, l);
    if(m_controller) {
        m_controller->clearFlags();
    }
    return *this;
}


//----------------------------------------------------------------------
IFR_TraceStream&
IFR_TraceStream::operator <<(const IFR_UInt1 i)
{
    TRACE_NULL_CHECK;
    int l;
    char s[64];
    SQLDBC_IRuntime::TaskTraceContext *controller_traceflags=m_controller ? m_controller->traceflags() : 0;
    if(controller_traceflags && controller_traceflags->hex) {
        l=IFR_Trace_sprintf(s, 64, "%hhX", i);
    } else {
        l=IFR_Trace_sprintf(s, 64, "%uc", i);
    }
    print(s, l);
    if(m_controller) {
        m_controller->clearFlags();
    }
    return *this;
}

//----------------------------------------------------------------------
IFR_TraceStream&
IFR_TraceStream::operator <<(const IFR_Int2 i)
{
    TRACE_NULL_CHECK;
    int l;
    char s[64];
    SQLDBC_IRuntime::TaskTraceContext *controller_traceflags=m_controller ? m_controller->traceflags() : 0;
    if(controller_traceflags && controller_traceflags->hex) {
        l=IFR_Trace_sprintf(s, 64, "%hX", i);
    } else {
        l=IFR_Trace_sprintf(s, 64, "%hd", i);
    }
    print(s, l);
    if(m_controller) {
        m_controller->clearFlags();
    }
    return *this;
}

//----------------------------------------------------------------------
IFR_TraceStream&
IFR_TraceStream::operator <<(const IFR_UInt2 i)
{
    TRACE_NULL_CHECK;
    int l;
    char s[64];
    SQLDBC_IRuntime::TaskTraceContext *controller_traceflags=m_controller ? m_controller->traceflags() : 0;
    if(controller_traceflags && controller_traceflags->hex) {
        l=IFR_Trace_sprintf(s, 64, "%hX", i);
    } else {
        l=IFR_Trace_sprintf(s, 64, "%hu", i);
    }
    print(s, l);
    if(m_controller) {
        m_controller->clearFlags();
    }
    return *this;
}

//----------------------------------------------------------------------
IFR_TraceStream&
IFR_TraceStream::operator <<(const IFR_Int4 i)
{
    TRACE_NULL_CHECK;
    int l;
    char s[64];
    SQLDBC_IRuntime::TaskTraceContext *controller_traceflags=m_controller ? m_controller->traceflags() : 0;
    if(controller_traceflags && controller_traceflags->hex) {
        l=IFR_Trace_sprintf(s, 64, "%X", i);
    } else {
        switch(i) {
        case 0:
            s[0] ='0';
            s[1] ='\0';
            l = 1;
            break;
        case 1:
            s[0] ='1';
            s[1] ='\0';
            l = 1;
            break;
        default: {
            if(i > 1000000 || i < 0) {
                l=IFR_Trace_sprintf(s, 64, "%d", i);
            } else {
                l=0;
                IFR_Int4 divisor = 1000000;
                IFR_Bool started=false;
                while(divisor) {
                    if(started) {
                        s[l] = '0' + (i / divisor) % 10;
                        ++l;
                    } else  if( (i / divisor) % 10) {
                        started = true;
                        continue;
                    }
                    divisor = divisor / 10;
                    continue;
                }
                s[l]='\0';
            }
        }
        }
    }
    print(s, l);
    if(m_controller) {
        m_controller->clearFlags();
    }
    return *this;
}

//----------------------------------------------------------------------
IFR_TraceStream&
IFR_TraceStream::operator <<(const IFR_UInt4 i)
{
    TRACE_NULL_CHECK;
    int l;
    char s[64];
    SQLDBC_IRuntime::TaskTraceContext *controller_traceflags=m_controller ? m_controller->traceflags() : 0;
    if(controller_traceflags && controller_traceflags->hex) {
        l=IFR_Trace_sprintf(s, 64, "%X", i);
    } else {
        l=IFR_Trace_sprintf(s, 64, "%u", i);
    }
    print(s, l);
    if(m_controller) {
        m_controller->clearFlags();
    }
    return *this;
}

//----------------------------------------------------------------------
IFR_TraceStream&
IFR_TraceStream::operator <<(const IFR_Int8 i)
{
    TRACE_NULL_CHECK;
    int l;
    char s[64];
    SQLDBC_IRuntime::TaskTraceContext *controller_traceflags=m_controller ? m_controller->traceflags() : 0;
    if(controller_traceflags && controller_traceflags->hex) {
        l=sp77sprintf(s, 64, "%qX", i);
    } else {
        l=sp77sprintf(s, 64, "%qd", i);
    }
    print(s, l);
    if(m_controller) {
        m_controller->clearFlags();
    }
    return *this;
}

//----------------------------------------------------------------------
IFR_TraceStream&
IFR_TraceStream::operator <<(const IFR_UInt8 i)
{
    TRACE_NULL_CHECK;
    int l;
    char s[64];
    SQLDBC_IRuntime::TaskTraceContext *controller_traceflags=m_controller ? m_controller->traceflags() : 0;
    if(controller_traceflags && controller_traceflags->hex) {
        l=IFR_Trace_sprintf(s, 64, "%qX", i);
    } else {
        l=IFR_Trace_sprintf(s, 64, "%qu", i);
    }
    print(s, l);
    if(m_controller) {
        m_controller->clearFlags();
    }
    return *this;
}

//----------------------------------------------------------------------
IFR_TraceStream&
IFR_TraceStream::operator <<(const double d)
{
    TRACE_NULL_CHECK;
    // don't use IFR_Trace_sprintf here, system routines are usually
    // of more precision ...
    int l;
    char s[64];
    l = sprintf(s, "%e", d);
    print(s, l);
    if(m_controller) {
        m_controller->clearFlags();
    }
    return *this;
}

//----------------------------------------------------------------------
IFR_TraceStream&
IFR_TraceStream::operator <<(const IFR_Retcode retcode)
{
    TRACE_NULL_CHECK;
    switch(retcode) {
    case IFR_OK:
        write("IFR_OK");
        break;
    case IFR_NOT_OK:
        write("IFR_NOT_OK");
        break;
    case IFR_DATA_TRUNC:
        write("IFR_DATA_TRUNC");
        break;
    case IFR_OVERFLOW:
        write("IFR_OVERFLOW");
        break;
    case IFR_SUCCESS_WITH_INFO:
        write("IFR_SUCCESS_WITH_INFO");
        break;
    case IFR_NO_DATA_FOUND:
        write("IFR_NO_DATA_FOUND");
        break;
    case IFR_NEED_DATA:
        write("IFR_NEED_DATA");
        break;
    default:
        (*this) << "(unknown " << (IFR_Int4)retcode << ")";
        break;
    }
    return *this;
}

//----------------------------------------------------------------------
IFR_TraceStream&
IFR_TraceStream::operator <<(const SQL_DATE_STRUCT& date)
{
    TRACE_NULL_CHECK;
    char buf[64];
    IFR_Trace_sprintf(buf, 64, "%04.4hd-%02.2hd-%02.2hd", date.year, date.month, date.day);
    write(buf);
    return *this;
}

//----------------------------------------------------------------------
IFR_TraceStream&
IFR_TraceStream::operator <<(const SQL_TIME_STRUCT& time)
{
    TRACE_NULL_CHECK;
    char buf[64];
    IFR_Trace_sprintf(buf, 64, "%04.4hd:%02.2hd:%02.2hd", time.hour, time.minute, time.second);
    write(buf);
    return *this;
}

//----------------------------------------------------------------------
IFR_TraceStream&
IFR_TraceStream::operator <<(const SQL_TIMESTAMP_STRUCT& timestamp)
{
    TRACE_NULL_CHECK;
    char buf[64];
    IFR_Trace_sprintf(buf, 64, "%04.4hd-%02.2hd-%02.2hd %04.4hd:%02.2hd:%02.2hd.%09.9d",
                timestamp.year,
                timestamp.month,
                timestamp.day,
                timestamp.hour,
                timestamp.minute,
                timestamp.second,
                timestamp.fraction);
    write(buf);
    return *this;
}

//----------------------------------------------------------------------
IFR_TraceStream&
IFR_TraceStream::operator <<(const SQL_NUMERIC_STRUCT& numeric)
{
    TRACE_NULL_CHECK;
    (*this) << "SQL_NUMERIC_STRUCT[precision=" << (IFR_Int4)numeric.precision
            << ", scale=" << (IFR_Int4)numeric.scale
            << ", sign=" << (IFR_Int4)numeric.sign 
            << ", val="
            << ::hex 
            << inputlength(16) << (char *)numeric.val << "]";
    return (*this);
}

//----------------------------------------------------------------------
IFR_TraceStream&
IFR_TraceStream::operator <<(const IFR_SQLMode sqlmode)
{
    TRACE_NULL_CHECK;
    switch(sqlmode) {
    case IFR_Nil:
        write("nil");
        break;
    case IFR_SessionSqlmode:
        write("session");
        break;
    case IFR_INTERNAL:
        write("internal");
        break;
    case IFR_ANSI:
        write("ansi");
        break;
    case IFR_DB2:
        write("db2");
        break;
    case IFR_ORACLE:
        write("oracle");
        break;
    default:
        (*this) << "(unknown " << (IFR_Int4)sqlmode << ")";
        break;
    }
    return *this;
}
    

//----------------------------------------------------------------------
IFR_TraceStream&
IFR_TraceStream::operator <<(const void *p)
{
    TRACE_NULL_CHECK;
    char s[64];
    int l;
    if(p) {
        l=IFR_Trace_sprintf(s, 64, "%p", p);
    } else {
        l=IFR_Trace_sprintf(s, 64, "(null)");
    }
    print(s, l);
    return *this;
}

#if !(defined(SUN) || defined(SOLARIS))
//----------------------------------------------------------------------
IFR_TraceStream&
IFR_TraceStream::operator <<(const IFR_Bool b)
{
    TRACE_NULL_CHECK;
    if(b) {
        print("true", 4);
    } else {
        print("false", 5);
    }
    return *this;
}
#endif

//----------------------------------------------------------------------
IFR_TraceStream&
IFR_TraceStream::operator << (PIn_SQLPacket& packet)
{
    TRACE_NULL_CHECK;
    packet.TraceOn(this);
    return *this;
}

//----------------------------------------------------------------------
IFR_TraceStream&
IFR_TraceStream::operator << (PIn_Segment& segment)
{
    TRACE_NULL_CHECK;
    segment.TraceOn(this);
    return *this;
}

//----------------------------------------------------------------------
IFR_TraceStream&
IFR_TraceStream::operator << (PIn_Part& part)
{
    TRACE_NULL_CHECK;
    part.TraceOn(this);
    return *this;
}

//----------------------------------------------------------------------
// Supplemental functions.
//----------------------------------------------------------------------

//----------------------------------------------------------------------
void hex(IFR_TraceStream& s)
{
    if(&s == 0) { return; }
    if(s.m_controller) {
        s.m_controller->setHex();
    }
    return;
}

//----------------------------------------------------------------------
void dec(IFR_TraceStream& s)
{
    if(&s == 0) { return; }
    if(s.m_controller) {
        s.m_controller->setDecimal();
    }
    return;
}

//----------------------------------------------------------------------
void endl(IFR_TraceStream& s)
{
    if(&s == 0) { return; }
    s.writeln("", 0);
}

//----------------------------------------------------------------------
void stamp(IFR_TraceStream& s)
{
    if(&s == 0) { return; }
    s << IFRUtil_Time::currentMicroSeconds();
    return;
}

//----------------------------------------------------------------------
void currenttime(IFR_TraceStream& s)
{
    if(&s == 0) { return; }
    char buffer[32];
    IFRUtil_Time::currentTimeStamp(buffer);
    s << dec << buffer;
    return;
}

//----------------------------------------------------------------------
IFR_TraceStream&
operator << (IFR_TraceStream& s , const inputlength& l)
{
    if(&s == 0) { return s; }
    if(s.m_controller) {
        s.m_controller->setInputLength(l.length);
    }
    return s;
}

//----------------------------------------------------------------------
IFR_TraceStream&
operator << (IFR_TraceStream& s , const inputencoding& i)
{
    if(&s == 0) { return s; }
    if(s.m_controller) {
        s.m_controller->setInputEncoding(i.data);
    }
    return s;
}

//----------------------------------------------------------------------
IFR_TraceStream&
operator << (IFR_TraceStream& s , const lpad& i)
{
    if(&s == 0) { return s; }
    if(s.m_controller) {
        s.m_controller->setIndent(i.data);
    }
    return s;
}

//----------------------------------------------------------------------
class IFR_TraceStream;
IFR_TraceStream& operator << (IFR_TraceStream& s, const IFR_String& p )
{
    if(&s == 0) { return s; }
    if(s.m_controller->traceflags()->hex) {
        s << inputencoding(p.getEncoding()) << inputlength(p.getLength()) << p.getBuffer();
    }
    else {
        s << "'" << inputencoding(p.getEncoding()) << inputlength(p.getLength()) << p.getBuffer() << "'";
    }
    return s;
}

//----------------------------------------------------------------------
IFR_TraceStream&
operator <<(IFR_TraceStream& s, const IFR_StringEncodingType::Encoding encoding)
{
    if(&s == 0) { return s; }
    switch(encoding) {
    case IFR_StringEncodingAscii:
        s << "ASCII";
        break;
    case IFR_StringEncodingUCS2:
        s << "UCS2 little endian";
        if(IFR_StringEncodingUCS2Native == encoding) {
            s << " (native)";
        }
        break;
    case IFR_StringEncodingUCS2Swapped:
        s << "UCS2 big endian";
        if((SQLDBC_StringEncodingType::Encoding)encoding 
           == (SQLDBC_StringEncodingType::Encoding)IFR_StringEncodingUCS2Native) {
            s << " (native)";
        }
        break;
    case IFR_StringEncodingUTF8:
        s << "UTF8";
        break;
    default:
        s << "(unknown " << (IFR_Int4)encoding << ")";
        break;
    }
    return s;
}

//----------------------------------------------------------------------
void IFR_TraceStream::hexFromFor (
    const char * comment,
    const void * bufParm,
    int startPos,
    int len)
{
    char line [200];
    int i;
    int currentByte;
    const teo00_Byte * buf = STATIC_CAST (const teo00_Byte *, bufParm);
    static const char hexDigitsC [] = "0123456789abcdef";
    static const int posPosC = 0;
    static const int hexPosC = 10;
    static const int clearPosC = 60;
    static const int chunkSizeC = 16;

    if (comment != NULL) {
        this->writeln (comment, (int)strlen(comment));
    }
    while (len > 0) {
        memset (line, ' ', sizeof (line));
        // write pos
        sp77sprintf (line + posPosC, sizeof (line) + posPosC, "%8x", startPos);
        line [posPosC + 8] = ' ';  // sp77printf ended with '\0'
        // write hex
        for (i = 0; (i < chunkSizeC) && (i < len); ++i) {
            currentByte = buf [startPos + i];
            line [hexPosC + 3 * i] = hexDigitsC [currentByte / 16];
            line [hexPosC + 3 * i + 1] = hexDigitsC [currentByte % 16];
        }
        // write clear
        line [clearPosC] = '|';
        int p;
        for (p=0, i = 0; (p < chunkSizeC) && (p < len); ++p) {
            currentByte = buf [startPos + p];
            if (currentByte >= ' ') {
                if(currentByte & 0x80) {
                    line [clearPosC + 1 + i] = 0x80 | (currentByte & 0x3F);
                    ++i;
                    line [clearPosC + 1 + i] = 0xC0 | ( (currentByte & 0xFF ) >> 6 );
                    ++i;
                } else {
                    line [clearPosC + 1 + i] = currentByte;
                    ++i;
                } 
            }
            else {
                line [clearPosC + 1 + i] = '.';
                ++i;
            }
        }
        line [clearPosC + 1 + i] = '|';
        // write line
        this->writeln (line, clearPosC + 1 + i + 1);
        startPos += chunkSizeC;
        len -= chunkSizeC;
    }
}

IFR_END_NAMESPACE
