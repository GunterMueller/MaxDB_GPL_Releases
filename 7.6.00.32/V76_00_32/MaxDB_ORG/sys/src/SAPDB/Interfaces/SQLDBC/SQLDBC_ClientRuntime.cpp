/*!
  @file           SQLDBC_ClientRuntime
  @author         D039759
  @ingroup        SQLDBC
  @brief          Interface to the runtime environment.
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

#include "SAPDB/Interfaces/SQLDBC/SQLDBC_ClientRuntime.h"

#include "SAPDB/RunTime/Synchronisation/RTESync_Spinlock.hpp"
#include "SAPDB/RunTime/Synchronisation/RTESync_InterlockedCounter.hpp"
#include "SAPDB/RunTime/Communication/RTEComm_ParseURI.hpp"

#include "SAPDB/RunTime/RTE_IInterface.hpp"


#include "SAPDB/SAPDBCommon/MemoryManagement/SAPDBMem_Alloca.h"

#include "SAPDB/Interfaces/Runtime/IFR_Common.h"
#include "SAPDB/Interfaces/Runtime/IFR_ErrorHndl.h"

#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_Tls.h"
#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_Time.h"
#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_RuntimeItem.h"
#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_Configuration.h"
#include "SAPDB/Interfaces/Runtime/Util/IFRUtil_LinkedList.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_RequestPacket.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_ReplyPacket.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_RequestSegment.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_ReplySegment.h"
#include "SAPDB/Interfaces/Runtime/Packet/IFRPacket_Part.h"
#include "SAPDB/Interfaces/Runtime/IFR_Connection.h"

#include "extern/zlib/zlib.h"

// XUSER
#include "heo02.h"
#include "heo13.h"
#include "hsp78_0.h"
#include "heo670.h"
#include <ctype.h>

#ifdef WIN32
#  define STRICMP _stricmp
#else
#  define STRICMP strcasecmp
#endif

externPascal void s02applencrypt (tsp00_Name VAR_ARRAY_REF pw_clear,
                                  tsp00_CryptPw VAR_ARRAY_REF pw_crypt);

externPascal void s02appldecrypt (tsp00_Name VAR_ARRAY_REF pw_clear,
                                  tsp00_CryptPw VAR_ARRAY_REF pw_crypt);

// application diagnostics in UNIX case shall go to /dev/null for
// this library.
#ifdef UNIX
extern int             sqldbglvl;
extern int             sql22_timestamp;
extern int				sql22_keep_open;
extern char           *sql22_file;
extern int				sql22_fd;
static int              have_appldag_file_opened = 0;
#endif

// Type definitions for C initialisation functions. Sun complains
// about a cast because it wants a C function type.
extern "C" {

typedef void (*SQLGETTLS_InitFuncType)(void*);
typedef void* (*SQLONCE_InitFuncType)(void*);

}

//----------------------------------------------------------------------
inline IFR_Bool
IFR_ConnectProperty_checkBooleanOption(IFR_ConnectProperties& p, const char *key, bool defaultvalue=false)
{
    const char *checkstr = p.getProperty(key, defaultvalue?"1":"0");

    if(STRICMP(checkstr, "1") == 0) {
        return true;
    }

    if(STRICMP(checkstr, "YES") == 0) {
        return true;
    }

    if(STRICMP(checkstr, "TRUE") == 0) {
        return true;
    }
    return false;
}

#define OOM_ERROR(error) do {                             \
   error.errorcode = -10760;                              \
   strcpy(error.errortext, "Memory allocation failed.");  \
} while(0)


//----------------------------------------------------------------------
class SQLDBC_ClientRuntime_TraceWriter
{
public:
    SQLDBC_ClientRuntime_TraceWriter();
    ~SQLDBC_ClientRuntime_TraceWriter();

    void write(SQLDBC_IRuntime::TaskTraceContext* ctx,
               const char *s,
               IFR_Int4 length,
               IFR_Bool multithreaded=true);

    void writeln(SQLDBC_IRuntime::TaskTraceContext* ctx,
                 const char *s,
                 IFR_Int4 length,
                 IFR_Bool multithreaded=true);

    void close();
    void open();

    /**
     * Returns whether the stream is open.
     * @return <code>true</code> if the file handle is not -1.
     */
    inline IFR_Bool isOpen()
    {
        return m_filehandle.filehandle!=-1;
    }


    void setFileSize(IFR_Int4 fileSizeLimit);

    void setFileName(char *nametemplate);
    inline const char *getTraceFileName() const { return m_tracefilename; }
    void setTimeStampPrefix(IFR_Bool flag) { m_timestampprefix = flag; }
    void flushBuffer();

    void setCompressed(IFR_Bool compressed);
    void newline();
    void simpleWrite(const char *s, IFR_Int4 length) { writeToBuffer(s, length); }
private:
    void writeHeader();
    void internalClose();
    void writeToBuffer(const char *s, IFR_Int4 length);
    void wrapTraceFile();
    void pad(IFR_Int4 length);
    IFR_Bool checkThreadChange(IFR_Bool multithreaded);

    union FileHandle
    {
        IFR_Int4 filehandle;
        gzFile   zipped_filehandle;
    };

    FileHandle      m_filehandle;
    IFR_Int4        m_filesize;
    IFR_Int4        m_filesizelimit;
    char            *m_tracefilename;
    teo07_Mutex     m_writelock;
    teo07_ThreadId  m_lastwriter;
    IFR_Int4        m_lastpos;
    IFR_Int4        m_wrapcount;
    IFR_Bool        m_timestampprefix;
    char            m_buffer[8192];
    IFR_Int4        m_buffer_used;
    IFR_Bool        m_compress_tracefile;

public:
    class TraceWriterBufferFlusher
    {
    public:
        TraceWriterBufferFlusher()
        {
            writer = 0;
        }

        ~TraceWriterBufferFlusher()
        {
            if(writer) {
                writer->flushBuffer();
                writer->close();
            }
        }

        SQLDBC_ClientRuntime_TraceWriter *writer;
    };

    static TraceWriterBufferFlusher s_bufferflusher;
};

//----------------------------------------------------------------------
SQLDBC_ClientRuntime_TraceWriter::TraceWriterBufferFlusher
SQLDBC_ClientRuntime_TraceWriter::s_bufferflusher;

//----------------------------------------------------------------------
SQLDBC_ClientRuntime_TraceWriter::SQLDBC_ClientRuntime_TraceWriter()
:m_filesize(0),
 m_filesizelimit(-1),
 m_tracefilename(0),
 m_lastwriter(0),
 m_lastpos(0),
 m_wrapcount(0),
 m_buffer_used(0),
 m_timestampprefix(IFR_FALSE),
 m_compress_tracefile(false)
{
    m_filehandle.filehandle = -1;
    s_bufferflusher.writer = this;
    sqlcreatemutex(&m_writelock);
}


//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime_TraceWriter::writeln
(SQLDBC_IRuntime::TaskTraceContext* ctx,
 const char *s,
 IFR_Int4    length,
 IFR_Bool multithreaded)
{
    // multithreaded = true;

    // Check whether the stream is open, and
    // open the stream, if necessary.
    if(!isOpen()) {
        open();
    }
    if(multithreaded) {
        sqlbeginmutex(&m_writelock);
    }
    if(!isOpen()) {
        if(multithreaded) {
            sqlendmutex(&m_writelock);
        }
        return;
    }

    IFR_Int4 writelength=length;
    if(length < 0) {
        writelength = (IFR_Int4)strlen(s);
    }

    // Check thread change, will print thread change.
    IFR_Bool threadchange=checkThreadChange(multithreaded);

    // We will pad
    //  - ctx->pos chars if there was a thread change, and ctx-pos is not 0
    //  - ctx->indent if m_lastpos is 0 and there was no thread change, or
    //    if there was a thread change and ctx->pos is 0
    IFR_Int4 padlen=0;
    if(threadchange) {
        if(ctx) {
            if(ctx->pos) {
                padlen=ctx->pos;
            } else {
                padlen=ctx->indent;
            }
        }
    } else if(m_lastpos == 0) {
        if(ctx) {
            padlen = ctx->indent;
        }
    }
    if(m_filesizelimit >0 &&
       (IFR_Int4)(m_filesize + writelength + ctx->indent + 2 + (m_timestampprefix ? 64 : 0)) > m_filesizelimit) {
        wrapTraceFile();
    }
    IFR_Int4 timeStrLen=0;
    if(padlen != 0 || m_lastpos == 0) {
        if(m_timestampprefix) {
            char    timestr[64];
            char    buf[128];
            IFRUtil_Time::currentTimeStamp(timestr);
            timeStrLen = sp77sprintf(buf, 128, "%s ", timestr);
            writeToBuffer(buf, timeStrLen);
        }
    }
    if(padlen) {
        pad(padlen);
    }
    m_lastpos=padlen + timeStrLen;
    writeToBuffer(s, writelength);
    newline();

    if(ctx) {
        ctx->pos = m_lastpos;
    }

    if(multithreaded) {
        sqlendmutex(&m_writelock);
    }
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime_TraceWriter::write
(SQLDBC_IRuntime::TaskTraceContext* ctx,
 const char *s,
 IFR_Int4    length,
 IFR_Bool multithreading)
{
    // multithreading = true;

    // Check whether the stream is open, and
    // open the stream, if necessary.
    if(!isOpen()) {
        open();
    }
    if(multithreading) {
        sqlbeginmutex(&m_writelock);
    }
    if(!isOpen()) {
        if(multithreading) {
            sqlendmutex(&m_writelock);
        }
        return;
    }

    IFR_Int4 writelength=length;
    if(length < 0) {
        writelength = (IFR_Int4)strlen(s);
    }

    // Check thread change, will print thread change.
    IFR_Bool threadchange=checkThreadChange(multithreading);

    // We will pad
    //  - ctx->pos chars if there was a thread change, and ctx-pos is not 0
    //  - ctx->indent if m_lastpos is 0 and there was no thread change, or
    //    if there was a thread change and ctx->pos is 0
    IFR_Int4 padlen=0;
    if(threadchange) {
        if(ctx) {
            if(ctx->pos) {
                padlen=ctx->pos;
            } else {
                padlen=ctx->indent;
            }
        }
    } else if(m_lastpos == 0) {
        if(ctx) {
            padlen = ctx->indent;
        }
    }
    if(m_filesizelimit >0 &&
       (IFR_Int4)(m_filesize + writelength + ctx->indent + 2 + (m_timestampprefix ? 64 : 0) ) > m_filesizelimit) {
        wrapTraceFile();
    }
    IFR_Int4 timeStrLen=0;
    if(padlen != 0 || m_lastpos == 0) {
        if(m_timestampprefix) {
            char    timestr[64];
            char    buf[128];
            IFRUtil_Time::currentTimeStamp(timestr);
            timeStrLen = sp77sprintf(buf, 128, "%s ", timestr);
            writeToBuffer(buf, timeStrLen);
        }
    }
    pad(padlen);
    m_lastpos=padlen + timeStrLen;
    writeToBuffer(s, writelength);
    if(ctx) {
        ctx->pos = m_lastpos;
    }
    if(multithreading) {
        sqlendmutex(&m_writelock);
    }
}


//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime_TraceWriter::newline()
{
    char c[3] = { 0x0D, 0x0A, 0x00 };
#ifdef _WIN32
    writeToBuffer(c, 2);
#else
    writeToBuffer(c+1, 1);
#endif
    m_lastpos = 0;
    return;
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime_TraceWriter::pad(IFR_Int4 length)
{
    const char *s="                                                         "
        "                       ";
    for(int i=0; i<length/80; ++i) {
        writeToBuffer(s, 80);
    }
    writeToBuffer(s, length % 80);
    return;
}

//----------------------------------------------------------------------
IFR_Bool
SQLDBC_ClientRuntime_TraceWriter::checkThreadChange(IFR_Bool multithreading)
{
    static char *checkthreadchange_oldstackvalue = 0;
    if(multithreading) {
        char stackvalue;
        if(&stackvalue == checkthreadchange_oldstackvalue) {
            return false;
        } else {
            checkthreadchange_oldstackvalue = &stackvalue;
        }

        teo07_ThreadId currentthread=sqlgetthreadid();
        if(m_lastwriter == currentthread) {
            return false;
        }
        char    str[512];
        char    timestr[64];
        IFRUtil_Time::currentTimeStamp(timestr);
        IFR_Int4 numchars=sp77sprintf(str, 512, "--- THREAD %X --- %s ---", currentthread, timestr);
        if(m_filesizelimit >0 &&
           m_filesize + numchars + 4 > m_filesizelimit) {
            wrapTraceFile();
        }
        if(m_lastpos != 0) {
            newline();
        }
        writeToBuffer(str, numchars);
        newline();
        m_lastwriter = currentthread;
        return true;
    } else {
        return false;
    }
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime_TraceWriter::open()
{
    sqlbeginmutex(&m_writelock);
    if(m_tracefilename == 0) {
        sqlendmutex(&m_writelock);
        return;
    }
    if(m_compress_tracefile) {
        gzFile zfile = gzopen(m_tracefilename, "wb6f");
        if(zfile == 0) {
            m_filehandle.filehandle = -1;
        } else {
            m_filehandle.zipped_filehandle = zfile;
        }
    } else {
        tsp05_RteFileError error;
        sqlfopenc(m_tracefilename,
                  sp5vf_binary,
                  sp5vf_write,
                  sp5bk_unbuffered,
                  &m_filehandle.filehandle,
                  &error);
        if(error.sp5fe_result != vf_ok) {
            m_filehandle.filehandle = -1;
        }
    }
    writeHeader();
    sqlendmutex(&m_writelock);
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime_TraceWriter::writeHeader()
{
    if(m_filehandle.filehandle==-1) {
        return;
    }
    tsp05_RteFileError error;
    const char *headerstr = "<html><head><meta http-equiv=\"Content-Type\""
        " content=\"text/html; charset=utf-8\"></head><body><PRE><PLAINTEXT>";
    if(m_compress_tracefile) {
        if(!gzwrite(m_filehandle.zipped_filehandle,
                    (void *)headerstr,
                    108)) {
            internalClose();
            return;
        }
    } else {
        sqlfwritec(m_filehandle.filehandle,
                   headerstr,
                   108,
                   &error);
        if(error.sp5fe_result != vf_ok) {
            internalClose();
            return;
        }
    }
    m_filesize = 108;

    {
        char versioninfo[512];
        char c[3] = { 0x0D, 0x0A, 0x00 };
#ifdef WIN32
        char *nl=(char *)c;
#else
        char *nl=(char *)c+1;
#endif
        sp77sprintf(versioninfo, sizeof(versioninfo), "SQLDBC %s %s %s%s",
                    "<MAJOR_VERSION>.<MINOR_VERSION>.<CORRECTION_LEVEL> BUILD <BUILD_NUMBER_EX_STRING>",
                    __DATE__,
                    __TIME__,
                    nl);
        IFR_Int4 writelen=(IFR_Int4)strlen(versioninfo);
        if(m_compress_tracefile) {
            if(!gzwrite(m_filehandle.zipped_filehandle,
                        versioninfo,
                        writelen)) {
                internalClose();
                return;
            }
        } else {
            sqlfwritec(m_filehandle.filehandle,
                       versioninfo,
                       writelen,
                       &error);
            if(error.sp5fe_result != vf_ok) {
                internalClose();
                return;
            }
        }
        m_filesize += writelen;
        m_lastpos=0;
    }


    if(m_wrapcount > 0) {
        char wrapinfo[256];
        char c[3] = { 0x0D, 0x0A, 0x00 };
        sp77sprintf(wrapinfo, 256, "Warning: Trace wrapped around %d times.", m_wrapcount);
#ifdef _WIN32
        strcat(wrapinfo, c);
#else
        strcat(wrapinfo, c+1);
#endif
        int writelen = (int)strlen(wrapinfo);
        if(m_compress_tracefile) {
            if(!gzwrite(m_filehandle.zipped_filehandle,
                        wrapinfo,
                        writelen)) {
                internalClose();
                return;
            }
        } else {
            sqlfwritec(m_filehandle.filehandle,
                       wrapinfo,
                       writelen,
                       &error);
            if(error.sp5fe_result != vf_ok) {
                internalClose();
                return;
            }
        }
        m_filesize += writelen;
        m_lastpos=0;
    }


}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime_TraceWriter::writeToBuffer(const char *s,
                                                IFR_Int4 length)
{
    while(length > sizeof(m_buffer)) {
        writeToBuffer(s, sizeof(m_buffer));
        s += sizeof(m_buffer);
        length -= sizeof(m_buffer);
    }

    if(length + m_buffer_used > sizeof(m_buffer)) {
        flushBuffer();
    }

    memcpy((char *)m_buffer + m_buffer_used, s, length);
    m_buffer_used += length;

    flushBuffer();
    return;
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime_TraceWriter::flushBuffer()
{
    IFR_Int4 length = m_buffer_used;
    char *s = (char *) m_buffer;
#define CWP_MARKER_SIZE 27
again_after_wrap:

    if(length == 0) {
        return;
    }
    
    if(m_filesizelimit >0 &&
       m_filesize + length > m_filesizelimit) {

        IFR_Int4 length_end = m_filesizelimit - m_filesize;
        if(m_compress_tracefile) {
            if(!gzwrite(m_filehandle.zipped_filehandle,
                        s,
                        length_end)) {
                internalClose();
                m_buffer_used = 0;
                return;
            }
            gzflush(m_filehandle.zipped_filehandle, Z_SYNC_FLUSH);
        } else {
            tsp05_RteFileError error;
            sqlfwritec(m_filehandle.filehandle,
                       s,
                       length_end,
                       &error);
            if(error.sp5fe_result != vf_ok) {
                internalClose();
                m_buffer_used = 0;
                return;
            }
        }
        wrapTraceFile();
        s += length_end;
        length -= length_end;
        goto again_after_wrap;
    } else {
        m_filesize += length;
        if(m_compress_tracefile) {
            if(!gzwrite(m_filehandle.zipped_filehandle,
                        s,
                        length)) {
                internalClose();
                m_buffer_used = 0;
                return;
            }
            gzflush(m_filehandle.zipped_filehandle, Z_SYNC_FLUSH);
        } else {
            tsp05_RteFileError error;
            sqlfwritec(m_filehandle.filehandle,
                       s,
                       length,
                       &error);
            sqlfwritec(m_filehandle.filehandle,
                       "\015\012<CURRENT WRITE POSITION>",
                       CWP_MARKER_SIZE,
                       &error);
            sqlfseekc(m_filehandle.filehandle,
                      -CWP_MARKER_SIZE,
                      sp5vf_seek_cur,
                      &error);
            if(error.sp5fe_result != vf_ok) {
                internalClose();
                m_buffer_used = 0;
                return;
            }
        }
        m_lastpos += length;
        m_buffer_used = 0;
    }
    return;
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime_TraceWriter::wrapTraceFile()
{
    if(m_compress_tracefile) {
        internalClose();
        open();
    } else {
        tsp05_RteFileError error;
        sqlfseekc(m_filehandle.filehandle,
                  0,
                  sp5vf_seek_begin,
                  &error);
        if(error.sp5fe_result != vf_ok) {
            internalClose();
            return;
        }
    }
    m_filesize=0;
    m_wrapcount++;
    writeHeader();
    return;
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime_TraceWriter::setFileSize(IFR_Int4 fileSizeLimit)
{
    sqlbeginmutex(&m_writelock);
    if(isOpen() && fileSizeLimit > 0) {
        if(m_filesize > fileSizeLimit) {
            if(!m_compress_tracefile) {
                tsp05_RteFileError error;
                sqlftruncatec(m_filehandle.filehandle,
                              fileSizeLimit,
                              &error);
            }
            wrapTraceFile();
        }
    }
    m_filesizelimit = fileSizeLimit;
    sqlendmutex(&m_writelock);
    return;
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime_TraceWriter::setFileName(char *nametemplate)
{
    char *name = (char *) RTE_IInterface::Instance().Allocator().Allocate(strlen(nametemplate)*3 + 1);
    *name      = '\0';

    char     pidStr[30];
    IFR_Int4 pid;

    sqlgetpid(&pid);
    sp77sprintf(pidStr, 30, "%d", pid);

    char *p=nametemplate;
    char *pidpos;
    do {
        pidpos=strstr(p, "%p");
        if(pidpos == 0) {
            strcat(name, p);
        } else {
            strncat(name, p, pidpos - p);
            strcat(name, pidStr);
            p = pidpos + 2;
        }
    } while(pidpos != 0);

    sqlbeginmutex(&m_writelock);
    if(isOpen()) {
        if(strcmp(m_tracefilename, name) == 0) {
            RTE_IInterface::Instance().Allocator().Deallocate(name);
            sqlendmutex(&m_writelock);
            return;
        }
        internalClose();
        RTE_IInterface::Instance().Allocator().Deallocate(m_tracefilename);
        m_tracefilename = name;
        open();
    } else {
        if(m_tracefilename != 0) {
            RTE_IInterface::Instance().Allocator().Deallocate(m_tracefilename);
        }
        m_tracefilename = name;
    }
    sqlendmutex(&m_writelock);
}


//----------------------------------------------------------------------
SQLDBC_ClientRuntime_TraceWriter::~SQLDBC_ClientRuntime_TraceWriter()
{
    if(s_bufferflusher.writer == this) {
        s_bufferflusher.writer = 0;
    }
    sqlbeginmutex(&m_writelock);
    internalClose();
    if(m_tracefilename) {
        RTE_IInterface::Instance().Allocator().Deallocate(m_tracefilename);
    }
    sqlendmutex(&m_writelock);
    sqldestroymutex(&m_writelock);
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime_TraceWriter::close()
{
    sqlbeginmutex(&m_writelock);
    if(m_filehandle.filehandle == -1) {
        sqlendmutex(&m_writelock);
        return;
    }
    internalClose();
    sqlendmutex(&m_writelock);
    return;
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime_TraceWriter::setCompressed(IFR_Bool compressed)
{
    if(m_compress_tracefile == compressed) {
        return;
    }
    if(isOpen()) {
        internalClose();
        m_compress_tracefile = compressed;
        open();
    } else {
        m_compress_tracefile = compressed;
    }
}


//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime_TraceWriter::internalClose()
{
    flushBuffer();
    if(m_compress_tracefile) {
        gzflush(m_filehandle.zipped_filehandle, Z_FINISH);
        gzclose(m_filehandle.zipped_filehandle);
    } else {
        tsp05_RteFileError ignored;
        sqlfclosec(m_filehandle.filehandle, sp5vf_close_normal, &ignored);
    }
    m_filehandle.filehandle = -1;
    m_filesize   = 0;
    m_wrapcount  = 0;
}

//----------------------------------------------------------------------
class SQLDBC_ClientRuntime_SpinLock
  : public SQLDBC_Spinlock {
  public:
    SQLDBC_ClientRuntime_SpinLock (SQLDBC_ClientRuntime* runtime):
    m_runtime(runtime)
    {};
    virtual ~SQLDBC_ClientRuntime_SpinLock(){
      deleteLock();
    };

    virtual SAPDB_Bool lock() {
      m_lock.Lock();
      return SQLDBC_TRUE;}

    virtual SAPDB_Bool unlock() {
      m_lock.Unlock();
      return SQLDBC_TRUE;}

    virtual SAPDB_Bool deleteLock() {
      m_lock.Unlock();
      return SQLDBC_TRUE;
  }

  private:
    RTESync_Spinlock m_lock;
    SQLDBC_ClientRuntime *m_runtime;
};
//----------------------------------------------------------------------
SQLDBC_ClientRuntime::SQLDBC_ClientRuntime()
: SQLDBC_IRuntime(0)
{
    m_tracewriter = new SQLDBC_ClientRuntime_TraceWriter();

    m_traceoptions.currentEntry   = 0;
    m_traceoptions.hex            = false;
    m_traceoptions.inputlength    = 0;
    m_traceoptions.encoding       = IFR_StringEncodingAscii;
    m_traceoptions.indent         = 0;
    m_traceoptions.flags          = 0;
    m_traceoptions.traceableLimit = 1000;
    m_traceoptions.pos            = 0;

    m_tracesettings.flags          = 0;
    m_tracesettings.traceableLimit = 1000;
    m_tracesettings.filesizeLimit  = -1;
    m_tracesettings.timestampPrefix = false;
    m_tracesettings.stopOnError = false;
    m_tracesettings.stopOnErrorCount = 0;
    m_tracesettings.stopOnErrorCode = 0;
    m_tracesettings.profileAction = IFR_TraceProfileAction_NOTSET;

    getTraceOptionsFromConfig();
    IFR_ErrorHndl ignoredErrorHandle(getGlobalAllocator());
    m_sharedmem.attach(ignoredErrorHandle);

//     IFR_STATIC_ASSERT(sizeof(IFRUtil_LinkedList<IFR_Environment>) ==
//                       sizeof(SQLDBC_IRuntime::Environment));
    IFRUtil_LinkedList<IFR_Environment> * environment_list =
        reinterpret_cast< IFRUtil_LinkedList<IFR_Environment> *>(&m_environmentlist);
    environment_list->setLock(this->createSpinlock(m_allocator));
    m_profilePointer = reinterpret_cast<IFR_Profile*>(&(this->m_profile));
    m_sessioninfolist = 0;
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime::setTraceOptions(const char *optionstring)
{
    setTraceOptions(optionstring, true);
}

static
char *option_strchr (const char *str, int c)
{
  while (*str) {
    if (*str == '\"') {
      do ++str; while (*str != '\"');
    }
    if (*str == c)
      return (char *) str;
    ++str;
  }

  return NULL;
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime::setTraceOptions(const char *optionstring, SAPDB_Bool fromAPI)
{
    m_tracesettings.flags = (m_tracesettings.flags & (~31));
    m_tracesettings.traceableLimit=1000;
    m_tracesettings.filesizeLimit=-1;
    m_tracesettings.timestampPrefix=false;
    m_tracesettings.stopOnError = false;
    m_tracesettings.stopOnErrorCount = 0;
    m_tracesettings.stopOnErrorCode = 0;
    m_tracesettings.profileAction = IFR_TraceProfileAction_NOTSET;
    IFR_Bool set_compressed = false;
    char *p=(char *)alloca(strlen(optionstring)+1);
    strcpy(p, optionstring);
    while(p && *p) {
        char *cpos=option_strchr(p, ':');
        if(cpos != 0)
            *cpos=0;
        switch(*p) {
        case 'c':
            m_tracesettings.flags|=1;
            break;
        case 'd':
            m_tracesettings.flags|=3;
            break;
        case 'G': {
            m_tracesettings.flags|=8;
            ++p;
            if(*p) {
                m_tracesettings.profileAction=(IFRUtil_TraceSharedMemory_TraceProfileAction) atoi(p);
                if(m_tracesettings.profileAction!= IFR_TraceProfileAction_RESET
                    && m_tracesettings.profileAction!= IFR_TraceProfileAction_COLLECT) {
                    m_tracesettings.profileAction= IFR_TraceProfileAction_NOTSET;
                }
            }
            break;
          }
       case 'a':
            m_tracesettings.flags|=16;
            break;
        case 'p': {
            ++p;
            if(*p) {
                m_tracesettings.traceableLimit=atoi(p);
                if(m_tracesettings.traceableLimit <= 0) {
                    m_tracesettings.traceableLimit=1000;
                }
            }
            m_tracesettings.flags|=4;
            break;
        }
        case 'e': {
            ++p;
            m_errorcounter = 0;
            char *slpos = strchr(p, '/');
            if(slpos == 0) {
                m_tracesettings.stopOnErrorCode = atoi(p);
                if(m_tracesettings.stopOnErrorCode!=0) {
                    m_tracesettings.stopOnError = true;
                }
            } else {
                *slpos = '\0';
                m_tracesettings.stopOnErrorCode = atoi(p);
                if(m_tracesettings.stopOnErrorCode!=0) {
                    m_tracesettings.stopOnError = true;
                    m_tracesettings.stopOnErrorCount = atoi(slpos + 1);
                    if(m_tracesettings.stopOnErrorCount < 0) {
                        m_tracesettings.stopOnErrorCount = 0;
                    }
                }
                *slpos = '/';
            }
            break;
        }
        case 'f': {
            // only if called from trace API
            if(!fromAPI) {
                break;
            }
            ++p;
            if(!*p) {
                break;
            }
            char buffer[1024];
            char ignored_errortext[128];
            if (*p == '\"') ++p;
            char *qpos = strchr (p, '\"');
            if (qpos != 0)
              *qpos = 0;
            if(IFRUtil_Configuration::getFullFileName(0, p, buffer, sizeof(buffer),
                                                      ignored_errortext, sizeof(ignored_errortext)) == IFR_OK) {
                m_tracewriter->setFileName(buffer);
            }
            break;
        }
        case 'z': {
            set_compressed = true;
            break;
        }
        case 's':
            ++p;
            if(*p) {
                m_tracesettings.filesizeLimit=atoi(p);
                if(m_tracesettings.filesizeLimit!=-1 &&
                   m_tracesettings.filesizeLimit < 8192) {
                    m_tracesettings.filesizeLimit=8192;
                }
            }
            break;
        case 'T':
            m_tracesettings.timestampPrefix=true;
            break;
        }
        if(cpos != 0) {
            p=cpos + 1;
        } else {
            p = 0;
        }
    }

    // -- now put these options through to the respective management
    //    classes/data structures
    m_traceoptions.flags = m_tracesettings.flags;
    m_traceoptions.traceableLimit = m_tracesettings.traceableLimit;
    if(m_tracewriter) {
        m_tracewriter->setCompressed(set_compressed);
        if(m_traceoptions.flags == 0) {
            m_tracewriter->close();
        } else {
            m_tracewriter->setFileSize(m_tracesettings.filesizeLimit);
            m_tracewriter->setTimeStampPrefix(m_tracesettings.timestampPrefix);
        }

    }
    return;
}

//----------------------------------------------------------------------
SAPDB_Int4
SQLDBC_ClientRuntime::getTraceOptions(char *options, SAPDB_Int4 length)
{
    IFR_String traceoptions(IFR_StringEncodingAscii, getGlobalAllocator());
    IFR_Bool first = true;
    char buffer[512];
    getFlags(buffer);
    IFR_Bool memory_ok = true;
    traceoptions.append(buffer, IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    if(traceoptions.getStrLen() != 0) {
        first = false;
    }

    if(first) {
        traceoptions.append("f", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    } else {
        traceoptions.append(":f", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    }
    traceoptions.append("\"", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    traceoptions.append(m_tracewriter->getTraceFileName(), IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    traceoptions.append("\"", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
    if(options && length > 1) {
        *options = '\0';
    }
    if(length < traceoptions.getStrLen() + 1) {
        return traceoptions.getStrLen() + 1;
    } else {
        memcpy(options, traceoptions.getBuffer(), traceoptions.getStrLen() + 1);
    }
    return traceoptions.getStrLen() + 1;
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime::getTraceOptionsFromConfig()
{
    char buffer[1024];
    char ignored_errortext[128];
    if(IFRUtil_Configuration::getFullTraceFileName(0, buffer, 1024,
                                                   ignored_errortext, sizeof(ignored_errortext)) == IFR_OK) {
        char buffer2[1024];
        if(IFRUtil_Configuration::getTraceFlags(0, buffer2, 1024,
                                                ignored_errortext, sizeof(ignored_errortext)) == IFR_OK) {
            setTraceOptions(buffer2, false);
        }
        m_tracewriter->setFileName(buffer);
    }
}


//----------------------------------------------------------------------
SQLDBC_ClientRuntime::~SQLDBC_ClientRuntime()
{
    // DO NOT PUT ANYTHING HERE AS FOR A CLIENT THIS
    // WILL NEVER BE CALLED.
}

//----------------------------------------------------------------------
const char *
SQLDBC_ClientRuntime::getIdentifier() const
{
    return "SQLDBC_ClientRuntime <MAJOR_VERSION>.<MINOR_VERSION>.<CORRECTION_LEVEL>    "
        "BUILD <BUILD_NUMBER_EX_STRING>";
}

//----------------------------------------------------------------------
static size_t blank_strlen(char *p, size_t p_len)
{
    for(size_t l=p_len; l > 0; --l) {
        if(p[l-1] != ' ') {
            return l;
        }
    }
    return 0;
}

//----------------------------------------------------------------------
static IFR_Bool is_zero_buffer(char *p, size_t len)
{
    for(size_t i=0; i<len; ++i) {
        if(p[i]) {
            return false;
        }
    }
    return true;
}

//----------------------------------------------------------------------
static size_t blank_strlen_ucs2native(char *p, size_t p_len)
{
    if(IFR_StringEncodingUCS2Native == IFR_StringEncodingUCS2) {
       for(size_t l=p_len; l > 0; l-=2) {
           if((p[l-1] != ' ') || (p[l-2])) {
               return l;
           }
       }
    } else {
        for(size_t l=p_len; l > 0; l-=2) {
           if((p[l-2] != ' ') || (p[l-1])) {
               return l;
           }
       }
    }
    return 0;
}

//----------------------------------------------------------------------
static void
split_string(int& argc, char **argv, char *s)
{
    argc=0;
    char *previous=s;

    while(*s) {
        if(isspace(*s)) {
            *s='\0';
            if(previous!=s && (previous+1)!=s) {
                argv[argc]=previous;
                argc++;
            }
            previous = s + 1;
        }
        ++s;
    }

    if(previous!=s && (previous+1)!=s) {
        argv[argc]=previous;
        argc++;
    }
}


//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::checkKeyOption(const char *keyoption,
                                     char *servernode,
                                     SAPDB_Int4& servernodelength,
                                     char *serverdb,
                                     SAPDB_Int4& serverdblength,
                                     SAPDB_Int4& isolationlevel,
                                     SAPDB_Int4& cachelimit,
                                     SAPDB_Int4& sqlmode,
                                     SAPDB_Int4& timeout,
                                     char *username,
                                     SAPDB_Int4& usernamelength,
                                     SQLDBC_StringEncoding& usernameEncoding,
                                     SQLDBC_IRuntime::Error& error)
{
    // We need them later, when they are possibly overridden
    // by the default key.
    SAPDB_Int4 servernode_room=servernodelength;
    SAPDB_Int4 serverdb_room=serverdblength;
    SAPDB_Int4 username_room=usernamelength;

    char *used_keyoption = 0;
    SAPDB_Bool keydefault = false;
    SAPDB_Bool keysqlopt  = false;
    if(keyoption == 0) {
        IFR_ErrorHndl tmperror(RTE_IInterface::Instance().Allocator());
        tmperror.setRuntimeError(IFR_ERR_INVALID_XUSERKEY);
        tmperror.toRuntimeError(error);
        return false;
    } else {
        if(strcmp(keyoption, "DEFAULT")==0) {
            used_keyoption = (char *)"DEFAULT";
            keydefault = true;
        } else if(strcmp(keyoption, "SQLOPT")==0) {
            keysqlopt = true;
        } else {
            used_keyoption = (char *)keyoption;
        }
    }

    // now look what we have to do ...
    if(used_keyoption || keysqlopt) {
        tsp4_xuser_record xuser_record;
        tsp00_ErrText errorText;
        tsp00_Bool    ok=true;
        if(used_keyoption) {
            size_t keyoption_length = strlen(used_keyoption);
            if(keyoption_length >30) {
                IFR_ErrorHndl tmperror(RTE_IInterface::Instance().Allocator());
                tmperror.setRuntimeError(IFR_ERR_INVALID_XUSERKEY);
                tmperror.toRuntimeError(error);
                return false;
            }
            memset(xuser_record.xu_key, ' ', sizeof(xuser_record.xu_key));
            memcpy(xuser_record.xu_key, used_keyoption, keyoption_length);
            sqlgetuser(&xuser_record, NULL, errorText, &ok);
            if(!ok) {
                tsp00_ErrTextc cError;
                cError.p2c(errorText);
                IFR_ErrorHndl tmperror(RTE_IInterface::Instance().Allocator());
                tmperror.setRuntimeError(IFR_ERR_XUSER_ERROR_S, cError.asCharp());
                tmperror.toRuntimeError(error);
                return false;
            }
        }

        // In case of DEFAULT or SQLOPT we will ask sqlarg3 for more ...
        if(keydefault || keysqlopt) {
            tsp00_Pw          ignored_password;
            tsp4_args_options options;
            tsp4_xuserset     xusertype;
            ok = true;
            options.opt_component.becomes(sp4co_sql_pc_runtime);
            sqlarg3(&xuser_record,
                    ignored_password,
                    &options,
                    xusertype,
                    errorText,
                    &ok);
            if(!ok) {
                tsp00_ErrTextc cError;
                cError.p2c(errorText);
                IFR_ErrorHndl tmperror(RTE_IInterface::Instance().Allocator());
                tmperror.setRuntimeError(IFR_ERR_XUSER_ERROR_S, cError.asCharp());
                tmperror.toRuntimeError(error);
                return false;
            }
        }

        // OK, now we look at the individual elements of the XUSER record.

        // servernode:
        IFR_Int4 xu_servernode_length=(IFR_Int4)blank_strlen(xuser_record.xu_servernode, sizeof(tsp00_NodeId));
        if(xu_servernode_length >= servernodelength) {
            IFR_ErrorHndl tmperror(RTE_IInterface::Instance().Allocator());
            tmperror.setRuntimeError(IFR_ERR_XUSER_ERROR_S, "Server node name too long");
            tmperror.toRuntimeError(error);
            return false;
        } else {
            if(xu_servernode_length) {
                servernodelength = xu_servernode_length;
                memcpy(servernode, xuser_record.xu_servernode, xu_servernode_length);
                servernode[servernodelength]='\0';
            } else {
                servernodelength = 0;
            }
        }

        // serverdb
        IFR_Int4 xu_serverdb_length=(IFR_Int4)blank_strlen(xuser_record.xu_serverdb, sizeof(tsp00_DbName));
        if(xu_serverdb_length >= serverdblength) {
            IFR_ErrorHndl tmperror(RTE_IInterface::Instance().Allocator());
            tmperror.setRuntimeError(IFR_ERR_XUSER_ERROR_S, "Database name too long");
            tmperror.toRuntimeError(error);
            return false;
        } else {
            if(xu_serverdb_length) {
                serverdblength = xu_serverdb_length;
                memcpy(serverdb, xuser_record.xu_serverdb, xu_serverdb_length);
                serverdb[serverdblength]='\0';
            } else {
                serverdblength = 0;
            }
        }
        // transfer the values, or -1
        isolationlevel = xuser_record.xu_isolation;
        cachelimit = xuser_record.xu_cachelimit;
        timeout = xuser_record.xu_timeout;

        if(memcmp(xuser_record.xu_sqlmode, "SAPR3   ", 8)==0) {
            sqlmode = IFR_SAPR3;
        } else if(memcmp(xuser_record.xu_sqlmode, "INTERNAL", 8)==0) {
            sqlmode = IFR_INTERNAL;
        } else if(memcmp(xuser_record.xu_sqlmode, "ORACLE  ", 8)==0) {
            sqlmode = IFR_ORACLE;
        } else if(memcmp(xuser_record.xu_sqlmode, "ANSI    ", 8)==0) {
            sqlmode = IFR_ANSI;
        } else if(memcmp(xuser_record.xu_sqlmode, "DB2     ", 8)==0) {
            sqlmode = IFR_ANSI;
        } else {
            sqlmode = IFR_Nil;
        }

        // now the user
        IFR_Int4 xu_usernamelength;
        IFR_Int4 terminatorsize=1;
        if(usernameEncoding == IFR_StringEncodingAscii) {
            xu_usernamelength = (IFR_Int4)blank_strlen(xuser_record.xu_user, sizeof(xuser_record.xu_user));
        } else {
            xu_usernamelength = (IFR_Int4)blank_strlen_ucs2native(xuser_record.xu_userUCS2, sizeof(xuser_record.xu_userUCS2));
            terminatorsize = 2;
        }
        if(xu_usernamelength + terminatorsize >= usernamelength) {
            IFR_ErrorHndl tmperror(RTE_IInterface::Instance().Allocator());
            tmperror.setRuntimeError(IFR_ERR_XUSER_ERROR_S, "User name too long");
            tmperror.toRuntimeError(error);
            return false;
        } else {
            usernamelength = xu_usernamelength;
            if(usernamelength) {
                if(usernameEncoding == IFR_StringEncodingAscii) {
                    memcpy(username, xuser_record.xu_user, usernamelength);
                    username[usernamelength]='\0';
                } else {
                    memcpy(username, xuser_record.xu_userUCS2, usernamelength);
                    username[usernamelength]='\0';
                    username[usernamelength+1]='\0';
                }
            }
        }
    }

    return true;
}


//----------------------------------------------------------------------
SAPDBMem_IRawAllocator&
SQLDBC_ClientRuntime::getGlobalAllocator()
{
    return m_allocator;
}

//----------------------------------------------------------------------
static void urldecode(IFR_String& s, IFR_Bool& memory_ok)
{
    IFR_String encoded_src(s, memory_ok);
    char space=' ';
    s.clear();

    char *p=(char *) encoded_src.getBuffer();

    while(*p) {
        if(*p == '+') {
            s.append(&space, IFR_StringEncodingAscii, 1, memory_ok);
            ++p;
        } else if(*p == '%') {
            unsigned char upper = (unsigned char) * (p+1);
            unsigned char lower = (unsigned char) * (p+2);

            if(upper >='0' && upper <='9') {
                upper -= '0';
            } else if(upper >='A' && upper <= 'F') {
                upper -= ('A' - 10);
            }

            if(lower >='0' && lower <='9') {
                lower -= '0';
            } else if(lower >='A' && lower <= 'F') {
                lower -= ('A' - 10);
            }

            upper <<= 4;
            upper += lower;

            s.append((char *)&upper, IFR_StringEncodingAscii, 1, memory_ok);
            p += 3;
        } else {
            s.append(p, IFR_StringEncodingAscii, 1, memory_ok);
            ++p;
        }
        if(!memory_ok) {
            return;
        }
    }
}

//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::parseConnectURL(const char *connecturl,
                                      IFR_ConnectProperties& properties,
                                      SQLDBC_IRuntime::Error& error)
{
#define PARSE_ERROR                                                         \
    {                                                                       \
        strncpy(error.errortext, errortext, sizeof(error.errortext));       \
        error.errortext[sizeof(error.errortext) -1] = '\0';                 \
        return false;                                                       \
    }


    IFR_Bool memory_ok=true;
    char *errortext=(char*)"Parse error in connect URL";
    RTEComm_ParseURI uriparser;
    Msg_List tmp_msglist;
    if(uriparser.Parse((SAPDB_Char const * const) connecturl,
                       tmp_msglist) != RTEComm_URIUtils::NoError) {
        strncpy(error.errortext, tmp_msglist.Message(), sizeof(error.errortext));
        error.errortext[sizeof(error.errortext) - 1] =  '\0';
        return false;
    }

    SAPDB_UTF8 *optname = 0;
    SAPDB_UTF8 *optvalue = 0;
    IFR_Bool    first=true;
    while(first || optname) {
        if(first) {
            optname = (SAPDB_UTF8*) uriparser.FirstOption((const unsigned char *&)optvalue);
            first = false;
        } else {
            optname = (SAPDB_UTF8*) uriparser.NextOption(optname, (const unsigned char *&)optvalue);
        }
        if(optname) {
            properties.setProperty((const char *)optname, (const char *)optvalue, memory_ok);
            if(!memory_ok) {
                OOM_ERROR(error);
                return false;
            }
        }
    }

    return true;

}

//----------------------------------------------------------------------
static SQLDBC_Bool passwd_from_sqlopt(tsp00_Pw& pw)
{
    char *sqlopt_env = getenv("SQLOPT");
    SQLDBC_Bool found=false;
    if(sqlopt_env) {
        int slen;
        char *s=(char *)alloca((slen=(int)strlen(sqlopt_env))+1);
        strcpy(s, sqlopt_env);
        char **argv=(char **)alloca(sizeof(char **)*slen);
        int    argc;
        split_string(argc, argv, s);
        int i=0;
        while(i<argc) {
            if(strcmp(argv[i],"-u")==0) {
                if(i==argc-1) {
                    return false;
                }
                ++i;
                char *comma=strchr(argv[i], ',');
                if(comma == 0) {
                    return false;
                }
                char *passwd = comma+1;
                strncpy(pw, passwd, MIN_EO00(strlen(passwd), sizeof(tsp00_Pw)));
                found=true;
                ++i;
            } else {
                ++i;
            }
        }
    }
    return found;
}

//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::connectSession(const char   *connectUrl,
                                     SAPDB_Int8&   sessionID,
                                     SAPDB_UInt4&  packetSize,
                                     SAPDB_Int4&   packetListSize,
                                     void        **packetList,
                                     SQLDBC_IRuntime::Error&     error,
                                     SAPDBMem_IRawAllocator *allocator,
                                     SQLDBC_Int4 connectType)
{
    IFR_Bool memory_ok = true;
    sessionID = -1;

    if(allocator == 0) {
        allocator = &getGlobalAllocator();
    }

    // servernode will be the URI itself
    // serverdb is extracted from the URI
    RTEComm_ParseURI uriparser;
    Msg_List tmp_msglist;
    if(uriparser.Parse((SAPDB_Char const * const) connectUrl,
                       tmp_msglist) != RTEComm_URIUtils::NoError) {
        strncpy(error.errortext, tmp_msglist.Message(), sizeof(error.errortext));
        error.errortext[sizeof(error.errortext) -1] = '\0';
        return false;
    }

    RTEComm_URI_DBPath const * path = uriparser.DBPath();

    tsp00_ErrTextc     errortext;
    tsp01_CommErr_Enum commerr;
    packetList[0]=packetList[1]=0;
    SAPDB_Int4 sqlaconnect_sessionid=-1;

    // The question what to use for connect is not easy to answer.
    char *servernode_to_use = 0;
    char *protocolname = (char *) uriparser.Protocol();
    if(strcmp(protocolname, "sapnis")==0 ||
       strcmp(protocolname, "remotes")==0) {
        servernode_to_use = (char *)connectUrl;
    } else {
        if(uriparser.IsLocalLocation()) {
            servernode_to_use = (char *)"";
        } else {
            if(uriparser.IsIPLocation()) {
                const RTEComm_URI_IPLocation *iplocation = uriparser.IPLocation();
                char *tmphost = (char *) iplocation->Hostname();
                char *tmpport = (char *) iplocation->PortNo();
                if(tmpport) {
                    servernode_to_use = (char *) alloca(strlen(tmphost) + strlen(tmpport) + 2);
                } else {
                    servernode_to_use = (char *) alloca(strlen(tmphost) + 1);
                }
                strcpy(servernode_to_use, tmphost);
                if(tmpport) {
                    strcat(servernode_to_use, ":");
                    strcat(servernode_to_use, tmpport);
                }
            } else if(uriparser.IsNILocation()) {
                const RTEComm_URI_NILocation * nilocation = uriparser.NILocation();
                servernode_to_use = (char *) nilocation->SAPRouter();
            } else {
                strcpy(error.errortext, "Invalid communication method.");
                return false;
            }
        }
    }
    m_connectlock.lockConnect();
    commerr = SqlAConnect((char *)servernode_to_use,
                          REFCAST_MEO00 (tsp00_DbNamec) (path->ServerDB()),
                          (tsp01_Service_Enum)connectType,
                          packetListSize,
                          sqlaconnect_sessionid,
                          (tsp00_Int4 &)packetSize,
                          packetList,
                          errortext);
    m_connectlock.releaseConnect();
    sessionID = sqlaconnect_sessionid;

    if(commerr != commErrOk_esp01) {
        IFR_String errStr(errortext,
                          sizeof(tsp00_ErrText),
                          IFR_StringEncodingAscii,
                          *allocator, memory_ok);
        if(!memory_ok) {
            OOM_ERROR(error);
            return false;
        }
        strncpy(error.errortext, errStr.getBuffer(), sizeof(error.errortext));
        error.errortext[sizeof(error.errortext) -1] = '\0';
        return false;
    }

    return true;
}

//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::getSession(const char  *connectUrl,
                                 const char  *connectCommand,
                                 const char  *password,
                                 SAPDB_Int4   passwordLength,
                                 const SQLDBC_StringEncoding commandEncoding,
                                 SAPDB_Int8&  sessionID,
                                 SAPDB_UInt4& packetSize,
                                 SAPDB_Int4&  packetListSize,
                                 void       **packetList,
                                 void       **sessionInfoReply,
                                 SQLDBC_IRuntime::Error& errorMessages,
                                 SAPDBMem_IRawAllocator *allocator)
{
    return getSession(connectUrl,
                      connectCommand,
                      0,
                      0,
                      password,
                      passwordLength,
                      commandEncoding,
                      sessionID,
                      packetSize,
                      packetListSize,
                      packetList,
                      sessionInfoReply,
                      errorMessages,
                      allocator);
}

//----------------------------------------------------------------------
IFR_Retcode
SQLDBC_ClientRuntime::initiateChallengeResponse(SAPDB_Int8&  sessionID,
                                                const char *username,
                                                SAPDB_Int4  usernamelength,
                                                IFR_StringEncoding encoding,
                                                int client_unicode,
                                                int client_sqlmode,
                                                const char *client_application,
                                                const char *client_version,
                                                IFRUtil_Vector<SQLDBC_ClientAuthenticationInfo>& client_authentication_info,
                                                void **packetList,
                                                SAPDB_UInt4 packetSize,
                                                SQLDBC_IRuntime::Error& error,
                                                SAPDBMem_IRawAllocator* allocator)
{
    IFR_Bool memory_ok = true;
    IFRPacket_RequestPacket *requestpacket = 0;
    IFR_StringEncoding packetencoding = client_unicode ? IFR_StringEncodingUCS2Native : IFR_StringEncodingAscii;
    IFR_String username_str(packetencoding, *allocator);

    // trim white spaces at the end.
    unsigned int username_padcount = IFR_ENCODING(encoding)->countPadChars((void *)username,
                                                                           usernamelength,
                                                                           ' ');
    username_str.append(username, encoding, usernamelength - (username_padcount * IFR_ENCODING(encoding)->terminatorSize), memory_ok);
    if(!memory_ok) {
        OOM_ERROR(error);
        goto error_exit;
    }
    {
        IFRUtil_RuntimeItem runtimeitem(*this, *allocator);
        requestpacket =
            new IFR_ALLOCATOR(*allocator) IFRPacket_RequestPacket((tsp1_packet*)packetList[0],
                                                                  packetSize,
                                                                  client_unicode,
                                                                  client_sqlmode,
                                                                  client_application,
                                                                  client_version,
                                                                  runtimeitem,
                                                                  memory_ok);
        if(requestpacket==0 || !memory_ok) {
            OOM_ERROR(error);
            return IFR_NOT_OK;
        }


        IFRPacket_RequestSegment segment=IFRPacket_RequestSegment(*requestpacket,
                                                                  IFRPacket_CommandMessageType::Dbs_C);
        IFRPacket_CommandPart connect_auth_part;
        segment.addPart(connect_auth_part);
        IFR_String connectauth_str(packetencoding, *allocator);
        connectauth_str.append("CONNECT \"", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        connectauth_str.append(username_str.getBuffer(), packetencoding, username_str.getLength(), memory_ok);
        connectauth_str.append("\" AUTHENTICATION", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        if(!memory_ok) {
            OOM_ERROR(error);
            return IFR_NOT_OK;
        }
        IFR_ErrorHndl tmperror(RTE_IInterface::Instance().Allocator());
        if(connect_auth_part.setText(connectauth_str, tmperror) != IFR_OK) {
            tmperror.toRuntimeError(error);
            goto error_exit;
        }
        {
            IFRPacket_DataPart datapart;
            segment.addPart(IFRPacket_PartKind::VarData_C, datapart);
            const RTESec_Authentication *authentication= 0;
            RTESec_Authentication::Iterator auth_iterator;
            int                    argument_count = 0;
            SQLDBC_ClientAuthenticationInfo authentication_info;
            // necessary, because argument count needs to be written
            // at the begin of the part.
            datapart.setInputParameterCount((IFR_Int2)argument_count);
            while(auth_iterator.next(authentication)) {
                const char *authentication_name = authentication->Name();
                const void *data;
                int         data_length;
                authentication_info.name = (char *) authentication_name;
                authentication_info.challenge_response = 0;
                authentication_info.challenge_response_length = 0;
                authentication_info.propertylist = 0;
                authentication_info.propertylist_length = 0;
                memset(&authentication_info.buffer, 0, sizeof(RTESec_ClientAuthenticationBuffer));
                if(authentication->DataForChallengeRequest(authentication_info.buffer,
                                                           username_str.getBuffer(),
                                                           username_str.getStrLen(),
                                                           data,
                                                           data_length)) {
                    datapart.addBinaryParameter(authentication_name, (IFR_UInt4)strlen(authentication_name));
                    datapart.addBinaryParameter(data, data_length);
                    argument_count += 2;
                    client_authentication_info.InsertEnd(authentication_info, memory_ok);
                    if(!memory_ok) {
                        OOM_ERROR(error);
                        return IFR_NOT_OK;
                    }
                }
            }
            // It may happen that we find nothing here because the authentication
            // method could not suitable initialize itself.
            if(argument_count == 0) {
                strcpy(error.errortext,
                       "Internal error: No working authentication method found.");
                goto error_exit;
            }
            datapart.setInputParameterCount((IFR_Int2)argument_count);
            datapart.setArgCount(1);
            segment.closePart();
            segment.close();

            SAPDB_Bool success = request(sessionID,
                                         requestpacket->GetRawPacket(),
                                         requestpacket->Length(),
                                         error);
            if(!success) {
                goto error_exit;
            } else {
                tsp1_packet *raw_reply;
                SAPDB_Int4   raw_reply_length;

                success = receive(sessionID, (void **)&raw_reply, raw_reply_length, error);
                if(!success) {
                    goto error_exit;
                } else {
                    IFRPacket_ReplyPacket replypacket;
                    replypacket.setPacket(raw_reply, requestpacket->getLock());
                    // check whether an error happened
                    IFRPacket_ReplySegment reply_segment(replypacket);
                    IFR_ErrorHndl auth_connect_error;
                    reply_segment.getSQLError(auth_connect_error, *allocator);
                    if(auth_connect_error) {
                        if(auth_connect_error.getErrorCode() == -5015) {
                            goto not_supported_exit;
                        } else {
                            auth_connect_error.toRuntimeError(error);
                            goto error_exit;
                        }
                    } else {
                        // walk on the part and look for data.
                        IFRPacket_VarDataPart vardata;
                        if(reply_segment.getPart(vardata) != IFR_OK) {
                            strcpy(error.errortext,
                                   "Internal error: Missing challenge reply part.");
                            goto error_exit;
                        }
                        IFR_Int2    arguments = vardata.getPartArguments();
                        IFR_Int4    found_authentication = 0;
                        IFR_Bool    first_loop=true;

                        // Old obsolete style had only 2 arguments
                        if(arguments % 3 != 0) {
                            goto not_supported_exit;
                        }


                        for(IFR_Int2 i=0; i<arguments; i+=3) {
                            IFR_Retcode rc = vardata.nextRow();
                            char       *authentication_name;
                            IFR_Length  authentication_name_length;
                            char       *authentication_data;
                            IFR_Length  authentication_data_length;
                            char       *propertylist_data;
                            IFR_Length  propertylist_data_length;

                            if(rc == IFR_NO_DATA_FOUND) {
                                break;
                            } else if(rc != IFR_OK) {
                                strcpy(error.errortext,
                                       "Internal error: Corrupted challenge reply.");
                                goto error_exit;
                            }

                            rc = vardata.nextField(authentication_name, authentication_name_length);
                            if(rc != IFR_OK) {
                                strcpy(error.errortext,
                                       "Internal error: Corrupted challenge reply (no authentication name).");
                                goto error_exit;
                            }
                            rc = vardata.nextField(authentication_data, authentication_data_length);
                            if(rc != IFR_OK) {
                                strcpy(error.errortext,
                                       "Internal error: Corrupted challenge reply (no authentication data).");
                                goto error_exit;
                            }
                            rc = vardata.nextField(propertylist_data, propertylist_data_length);
                            if(rc != IFR_OK) {
                                // First implementation did not know about maxpasswordlength.
                                if(first_loop) {
                                    goto not_supported_exit;
                                } else {
                                    strcpy(error.errortext,
                                           "Internal error: Corrupted challenge reply (no property data).");
                                    goto error_exit;
                                }
                            }

                            for(IFR_UInt4 i=0; i<client_authentication_info.GetSize(); ++i) {
                                if(strlen(client_authentication_info[i].name) == authentication_name_length &&
                                   memcmp(client_authentication_info[i].name, authentication_name, authentication_name_length) == 0) {

                                    client_authentication_info[i].challenge_response =
                                        allocator->Allocate(authentication_data_length);
                                    if(client_authentication_info[i].challenge_response == 0) {
                                        OOM_ERROR(error);
                                        goto error_exit;
                                    }

                                    client_authentication_info[i].propertylist =
                                        allocator->Allocate(propertylist_data_length);
                                    if(client_authentication_info[i].propertylist == 0) {
                                        OOM_ERROR(error);
                                        goto error_exit;
                                    }


                                    memcpy(client_authentication_info[i].challenge_response,
                                           authentication_data,
                                           authentication_data_length);
                                    memcpy(client_authentication_info[i].propertylist,
                                           propertylist_data,
                                           propertylist_data_length);

                                    client_authentication_info[i].challenge_response_length = authentication_data_length;
                                    client_authentication_info[i].propertylist_length = propertylist_data_length;
                                    ++found_authentication;
                                    break;
                                }
                            }

                            first_loop = false; // no possibility so that
                        }

                        if(found_authentication == 0) {
                            strcpy(error.errortext,
                                   "Internal error: No suitable authentication method found.");
                            goto error_exit;
                        }
                    }
                }
            }
        }
    }
    {
        IFRUtil_Delete(requestpacket, *allocator);
        return IFR_OK;
    }
error_exit:
    {
        IFRUtil_Delete(requestpacket, *allocator);
        SQLDBC_IRuntime::Error ignored;
        releaseSession(sessionID, ignored);
        return IFR_NOT_OK;
    }
not_supported_exit:
    {
        IFRUtil_Delete(requestpacket, *allocator);
        SQLDBC_IRuntime::Error ignored;
        releaseSession(sessionID, ignored);
        return IFR_NO_DATA_FOUND;
    }
}

struct SQLDBC_ClientRuntime_ClientAuthenticationInfo_Cleanup
{
    SQLDBC_ClientRuntime_ClientAuthenticationInfo_Cleanup(SAPDBMem_IRawAllocator& allocator,
                                                          IFRUtil_Vector<SQLDBC_ClientAuthenticationInfo>& vec)
    :m_allocator(allocator),
     m_vector(vec)
    {}

    ~SQLDBC_ClientRuntime_ClientAuthenticationInfo_Cleanup()
    {
        for(IFR_UInt4 i=0; i<m_vector.GetSize(); ++i) {
            if(m_vector[i].challenge_response != 0) {
                m_allocator.Deallocate(m_vector[i].challenge_response);
            }
            if(m_vector[i].propertylist != 0) {
                m_allocator.Deallocate(m_vector[i].propertylist);
            }
        }
    }

    SAPDBMem_IRawAllocator& m_allocator;
    IFRUtil_Vector<SQLDBC_ClientAuthenticationInfo>& m_vector;
};


//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::getSession(const char  *connectUrl,
                                 const char  *connectCommand,
                                 const char  *username,
                                 SAPDB_Int4   usernameLength,
                                 const char  *password,
                                 SAPDB_Int4   passwordLength,
                                 const SQLDBC_StringEncoding commandEncoding,
                                 SAPDB_Int8&  sessionID,
                                 SAPDB_UInt4& packetSize,
                                 SAPDB_Int4&  packetListSize,
                                 void       **packetList,
                                 void       **sessionInfoReply,
                                 SQLDBC_IRuntime::Error& error,
                                 SAPDBMem_IRawAllocator *allocator)
{
    IFR_Bool memory_ok=true;
    // Use global allocator if no allocator passed.
    if(allocator == 0) {
        allocator = &getGlobalAllocator();
    }

    IFR_ConnectProperties properties(*allocator, memory_ok);

    if(!parseConnectURL(connectUrl,
                        properties,
                        error)) {
        return false;
    }

    // look whether a compname is present, and use it for the sqlinit
    const char *compname_property=properties.getProperty(IFR_CONNECTPROPERTY_COMPNAME, "R ifr80");
    tsp00_CompName compname;
    memset(compname, ' ', sizeof(tsp00_CompName));
    strncpy (compname, compname_property, sizeof(tsp00_CompName));
    compname[63]='\0';
    // make a runtime initialisation
    sqlinit (compname, NULL);

    SQLDBC_Int4 connectType = srvUser_esp01;
    const char *connecttype_property = properties.getProperty(IFR_CONNECTPROPERTY_CONNECTTYPE);
    if(connecttype_property) {
        if(STRICMP(connecttype_property, "OLTP") == 0) {
            connectType = srvOLTPUser_esp01;
        } else if(STRICMP(connecttype_property, "LVC") == 0) {
            connectType = srvLVCUser_esp01;
        }
    }


    if(usernameLength == IFR_NTS && username!=0) {
        unsigned int character_length;
        unsigned int byte_length;
        int terminated;
        int corrupted = 0;
        int exhausted = 0;
        IFR_ENCODING(commandEncoding)->stringInfo(username,
                                                  256,                // a max. length
                                                  1,                  // in bytes
                                                  &character_length,
                                                  &byte_length,
                                                  &terminated,
                                                  &corrupted,
                                                  &exhausted);
        if(corrupted || exhausted) {
            strcpy(error.errortext,
                   "Corrupted/incomplete username specified.");
            return false;
        }
        usernameLength = byte_length;
    }

    if(passwordLength == SQLDBC_NTS) {
        unsigned int characters;
        unsigned int bytes;
        int terminated, corrupted, exhausted;
        if(IFR_ENCODING(commandEncoding)->stringInfo(password,
                                                     256,
                                                     1,
                                                     &characters,
                                                     &bytes,
                                                     &terminated,
                                                     &corrupted,
                                                     &exhausted)) {
            if(corrupted || exhausted) {
                strcpy(error.errortext,
                       "Corrupted/incomplete password specified.");
                return false;
            }
        }
        passwordLength = bytes;
    }

    if(!connectSession(connectUrl,
                       sessionID,
                       packetSize,
                       packetListSize,
                       packetList,
                       error,
                       allocator,
                       connectType)) {
        return false;
    }

    int  client_unicode=0;
    const char *unicode_str=properties.getProperty(IFR_CONNECTPROPERTY_UNICODE, "0");
    if(STRICMP(unicode_str, "TRUE")==0 || atoi(unicode_str)) {
        client_unicode=1;
    } else {
        client_unicode=0;
    }

    if(username != 0 && password != 0) {
        SAPDB_Bool real_unicode = false;
        switch(commandEncoding) {
        case IFR_StringEncodingAscii:
            client_unicode = 0;
            break;
        case IFR_StringEncodingUCS2: {
            for(SAPDB_Int4 i1 = 0; i1<usernameLength; i1+=2) {
                if(username[i1] != 0) {
                    real_unicode = true;
                    break;
                }
            }
            if(!real_unicode) {
                for(SAPDB_Int4 i2 = 0; i2<passwordLength; i2+=2) {
                    if(password[i2] != 0) {
                        real_unicode = true;
                        break;
                    }
                }
            }
            if(!real_unicode) {
                client_unicode = 0;
            }
            break;
        }
        case IFR_StringEncodingUCS2Swapped: {
            for(SAPDB_Int4 i1 = 1; i1<usernameLength; i1+=2) {
                if(username[i1] != 0) {
                    real_unicode = true;
                    break;
                }
            }
            if(!real_unicode) {
                for(SAPDB_Int4 i2 = 1; i2<passwordLength; i2+=2) {
                    if(password[i2] != 0) {
                        real_unicode = true;
                        break;
                    }
                }
            }
            if(!real_unicode) {
                client_unicode = 0;
            }
            break;
        }
        }
    }
    
    // ... now hopefully, we had mixed it up correctly
    char client_application[4];
    char client_version[6];
    int  client_sqlmode=IFR_INTERNAL;

    // -- check properties for packet construction
    const char *client_application_property=properties.getProperty(IFR_CONNECTPROPERTY_APPLICATION);
    if(client_application_property == 0) {
        OOM_ERROR(error);
        return false;
    }
    strncpy(client_application, client_application_property, 3);
    client_application[3]='\0';

    const char *client_version_property=properties.getProperty(IFR_CONNECTPROPERTY_APPVERSION);
    if(client_version_property==0) {
        OOM_ERROR(error);
        return false;
    }
    strncpy(client_version, client_version_property, 5);
    client_version[5]='\0';

    const char *sqlmode_str=properties.getProperty(IFR_CONNECTPROPERTY_SQLMODE, "INTERNAL");
    if(sqlmode_str==0) {
        OOM_ERROR(error);
        return false;
    }

    if((STRICMP(sqlmode_str, "ADABAS")==0)
       || (STRICMP(sqlmode_str, "SAPDB")==0)
       || (STRICMP(sqlmode_str, "INTERNAL")==0)) {
        client_sqlmode=IFR_INTERNAL;
    } else if(STRICMP(sqlmode_str, "ORACLE")==0
              || STRICMP(sqlmode_str, "SAPR3")==0) {
        client_sqlmode=IFR_ORACLE;
    } else if(STRICMP(sqlmode_str, "ANSI")==0) {
        client_sqlmode=IFR_ANSI;
    } else if(STRICMP(sqlmode_str, "DB2")==0) {
        client_sqlmode=IFR_DB2;
    }

    IFR_Bool challenge_response_used = false;
    IFRUtil_Vector<SQLDBC_ClientAuthenticationInfo> client_authentication_buffer(*allocator);
    SQLDBC_ClientRuntime_ClientAuthenticationInfo_Cleanup myCleanUp(*allocator, client_authentication_buffer);

    IFR_StringEncoding packetencoding = client_unicode ? IFR_StringEncodingUCS2Native : IFR_StringEncodingAscii;
    tsp00_ErrTextc ignored_errortext;
    if(!SqlBasicAuthenticationOnly((tsp00_Int4)sessionID, ignored_errortext) && username != 0) {
        IFR_Retcode rc = initiateChallengeResponse(sessionID,
                                                   username,
                                                   usernameLength,
                                                   commandEncoding,
                                                   client_unicode,
                                                   client_sqlmode,
                                                   client_application,
                                                   client_version,
                                                   client_authentication_buffer,
                                                   packetList,
                                                   packetSize,
                                                   error,
                                                   allocator);
        if(rc == IFR_NO_DATA_FOUND) {
            // kernel didn't want challenge response ...
            if(!connectSession(connectUrl,
                               sessionID,
                               packetSize,
                               packetListSize,
                               packetList,
                               error,
                               allocator,
                               connectType)) {
                return false;
            }
        } else if(rc == IFR_NOT_OK) {
            return false;
        } else {
            challenge_response_used = true;
        }
    }

    IFR_String username_str(packetencoding, *allocator);
    if(username) {
        unsigned int username_padcount = IFR_ENCODING(commandEncoding)->countPadChars((void *)username,
                                                                                      usernameLength,
                                                                                      ' ');
        IFR_Bool memory_ok = true;
        username_str.append(username, commandEncoding,
                            usernameLength - (username_padcount * IFR_ENCODING(commandEncoding)->terminatorSize),
                            memory_ok);
    }


    const char *xuserkey_property = properties.getProperty(IFR_CONNECTPROPERTY_KEY);
    // look at first at the given password.
    tsp00_CryptPw crypted_password;
    tsp00_Pw internal_password;
    memset(crypted_password, 0, sizeof(crypted_password));

    // pad the password with blanks, in the correct encoding
    if(password && passwordLength>0) {
        if(!client_unicode) {
            for(int k=0; k<sizeof(tsp00_Pw); ++k) {
                internal_password[k]=' ';
            }
        } else {
            if(IFR_StringEncodingUCS2Native == IFR_StringEncodingUCS2) {
                for(int k=0; k<sizeof(tsp00_Pw); ++k) {
                    if(k % 2) {
                        internal_password[k] = ' ';
                    } else {
                        internal_password[k] = 0;
                    }
                }
            } else {
                for(int k=0; k<sizeof(tsp00_Pw); ++k) {
                    if(k % 2) {
                        internal_password[k] = 0;
                    } else {
                        internal_password[k] = ' ';
                    }
                }
            }
        }
        {
            tsp00_Uint4 byteswritten;
            tsp00_Uint4 srcbytesparsed;
            sp78convertString(client_unicode ? sp77encodingUCS2Native : sp77encodingAscii,
                              internal_password,
                              sizeof(tsp00_Pw),
                              &byteswritten,
                              false,
                              IFR_ENCODING(commandEncoding),
                              password,
                              passwordLength,
                              &srcbytesparsed);
        }
        s02applencrypt (internal_password, crypted_password);
    }
    // so far, so good ... now comes the XUSER
    if(xuserkey_property != 0 && strcmp(xuserkey_property, "SQLOPT") != 0 ) {
        tsp4_xuser_record xuser_record;
        memset(xuser_record.xu_key, ' ', sizeof(xuser_record.xu_key));
        memcpy(xuser_record.xu_key, xuserkey_property, MIN_EO00(strlen(xuserkey_property), sizeof(xuser_record.xu_key)));
        tsp00_ErrText errorText;
        tsp00_Bool    ok=true;
        sqlgetuser(&xuser_record, NULL, errorText, &ok);
        if(!ok) {
            tsp00_ErrTextc cError;
            cError.p2c(errorText);
            IFR_ErrorHndl tmperror(RTE_IInterface::Instance().Allocator());
            tmperror.setRuntimeError(IFR_ERR_XUSER_ERROR_S, cError.asCharp());
            tmperror.toRuntimeError(error);
            SQLDBC_IRuntime::Error ignored;
            releaseSession(sessionID, ignored);
            sessionID = -1;
            return false;
        }
        if(client_unicode) {
            if(!is_zero_buffer(xuser_record.xu_passwordUCS2, sizeof(xuser_record.xu_passwordUCS2))) {
                memcpy(crypted_password, xuser_record.xu_passwordUCS2, sizeof(tsp00_CryptPw));
            }
        } else {
            if(!is_zero_buffer(xuser_record.xu_password, sizeof(xuser_record.xu_password))) {
                memcpy(crypted_password, xuser_record.xu_password, sizeof(tsp00_CryptPw));
            }
        }
    }
    // as if this wasn't enough hassle, we have SQLOPT in case of SQLOPT and DEFAULT
    if(xuserkey_property != 0 &&
       (strcmp(xuserkey_property, "DEFAULT")==0  || strcmp(xuserkey_property, "SQLOPT")==0)) {
        tsp4_xuser_record xuser_record;
        memset(xuser_record.xu_key, ' ', sizeof(xuser_record.xu_key));
        memcpy(xuser_record.xu_key, xuserkey_property, MIN_EO00(strlen(xuserkey_property), sizeof(xuser_record.xu_key)));
        tsp00_ErrText errorText;
        tsp00_Bool    ok=true;
        tsp4_args_options options;
        tsp4_xuserset     xusertype;
        tsp00_Pw tmp_internal_password;
        options.opt_component.becomes(sp4co_sql_pc_runtime);
        sqlarg3(&xuser_record,
                tmp_internal_password,
                &options,
                xusertype,
                errorText,
                &ok);
        if(!ok) {
            tsp00_ErrTextc cError;
            cError.p2c(errorText);
            IFR_ErrorHndl tmperror(RTE_IInterface::Instance().Allocator());
            tmperror.setRuntimeError(IFR_ERR_XUSER_ERROR_S, cError.asCharp());
            tmperror.toRuntimeError(error);
            SQLDBC_IRuntime::Error ignored;
            releaseSession(sessionID, ignored);
            sessionID = -1;
            return false;
        }
        if(xusertype.includes(sp4xu_sql_userparms)) {
            // now we have even more hassle ...
            // tmp_internal_password is always in ASCII, so if we have an UNICODE
            // connect we are fooled here ...
            if(!client_unicode) {
                s02applencrypt (tmp_internal_password, crypted_password);
            } else {
                // copy the tmp_internal_password into internal password and
                // recode it if necessary.
                if(IFR_StringEncodingUCS2Native == IFR_StringEncodingUCS2) {
                    for(int k=0; k<sizeof(tsp00_Pw); ++k) {
                        if(k % 2) {
                            internal_password[k] = ' ';
                        } else {
                            internal_password[k] = 0;
                        }
                    }
                } else {
                    for(int k=0; k<sizeof(tsp00_Pw); ++k) {
                        if(k % 2) {
                            internal_password[k] = 0;
                        } else {
                            internal_password[k] = ' ';
                        }
                    }
                }
                tsp00_Uint4 byteswritten;
                tsp00_Uint4 srcbytesparsed;
                sp78convertString(sp77encodingUCS2Native,
                                  internal_password,
                                  sizeof(tsp00_Pw),
                                  &byteswritten,
                                  false,
                                  IFR_ENCODING(IFR_StringEncodingAscii),
                                  tmp_internal_password,
                                  sizeof(tsp00_Pw),
                                  &srcbytesparsed);
                s02applencrypt (internal_password, crypted_password);
            }
        }
    }


    IFRUtil_RuntimeItem runtimeitem(*this, *allocator);

    // -- create the packet
    IFRPacket_RequestPacket *requestpacket =
        new IFR_ALLOCATOR(*allocator) IFRPacket_RequestPacket((tsp1_packet*)packetList[0],
                                                              packetSize,
                                                              client_unicode,
                                                              client_sqlmode,
                                                              client_application,
                                                              client_version,
                                                              runtimeitem,
                                                              memory_ok);
    if(requestpacket==0 || IFR_MEM_NOT_OK) {
        OOM_ERROR(error);
        return false;
    }
    IFRPacket_RequestSegment connectCommandSegment=IFRPacket_RequestSegment(*requestpacket,
                                                                            IFRPacket_CommandMessageType::Dbs_C);
    IFRPacket_CommandPart connect_cmdpart;
    connectCommandSegment.addPart(connect_cmdpart);
    IFR_String connectCommandString(connectCommand, commandEncoding, *allocator, memory_ok);
    if(!memory_ok) {
        OOM_ERROR(error);
        SQLDBC_IRuntime::Error ignored;
        releaseSession(sessionID, ignored);
        return false;
    }
    IFR_ErrorHndl tmperror(RTE_IInterface::Instance().Allocator());
    if (IFR_OK != connect_cmdpart.setText(connectCommandString, tmperror)) {
      tmperror.toRuntimeError(error);
      SQLDBC_IRuntime::Error ignored;
      releaseSession(sessionID, ignored);
      return false;
    }

    if(!challenge_response_used) {
        IFRPacket_DataPart connect_datapart;
        connectCommandSegment.addPart(connect_datapart);
        connect_datapart.AddArgument();

        unsigned char *dest=connect_datapart.GetWriteData();
        if(username != 0) {
            *dest = csp_defined_byte;
            ++dest;
            void *p = dest;
            unsigned int targetLength = 64;
            IFR_ENCODING(packetencoding)->fillString(&p, &targetLength, 64, ' ');
            memcpy(dest, username_str.getBuffer(), username_str.getLength());
            dest += 64;
            connect_datapart.ExtendLength(1 + 64);
        }
        *dest=csp_defined_byte;
        ++dest;
        memcpy(dest, crypted_password, sizeof(tsp00_CryptPw));
        connect_datapart.ExtendLength(1 + sizeof(tsp00_CryptPw));
        dest += sizeof(tsp00_CryptPw);
        tsp00_TermId termid;
        memset(termid, 0, sizeof(tsp00_TermId));
        sqltermid(termid);
        *dest=csp_defined_byte;
        ++dest;
        memcpy (dest, termid, sizeof(tsp00_TermId));
        connect_datapart.ExtendLength (1 + sizeof (tsp00_TermId));
        connectCommandSegment.closePart();
    } else {
        IFRPacket_DataPart connect_datapart;
        connectCommandSegment.addPart(IFRPacket_PartKind::VarData_C, connect_datapart);
        // write user name and term id
        unsigned char *dest=connect_datapart.GetWriteData();
        connect_datapart.setArgCount(3);
        connect_datapart.setInputParameterCount(3);
        char buffer[64];
        void *p = &(buffer[0]);
        unsigned int targetLength = 64;
        IFR_ENCODING(packetencoding)->fillString(&p, &targetLength, 64, ' ');
        memcpy(buffer, username_str.getBuffer(), username_str.getLength() > 64 ? 64 : username_str.getLength());
        connect_datapart.addBinaryParameter(buffer, 64);

//         if(username != 0) {
//             *dest = csp_defined_byte;
//             ++dest;
//             void *p = dest;
//             unsigned int targetLength = 64;
//             IFR_ENCODING(commandEncoding)->fillString(&p, &targetLength, 64, ' ');
//             memcpy(dest, username, usernameLength);
//             dest += 64;
//             connect_datapart.ExtendLength(1 + 64);
//         }

        // and now to the challenge ...

        tsp00_Name clear_password;
        s02appldecrypt (clear_password, crypted_password);
        RTESec_Authentication *authentication;
        IFR_UInt4 used_method_index = 0;
        for(used_method_index = 0; used_method_index < client_authentication_buffer.GetSize(); ++used_method_index) {
            if(client_authentication_buffer[used_method_index].challenge_response) {
                break;
            }
        }
        if(used_method_index == client_authentication_buffer.GetSize()) {
            strcpy(error.errortext,
                   "No suitable authentication method found.");
            SQLDBC_IRuntime::Error ignored;
            releaseSession(sessionID, ignored);
            return false;
        }
        authentication = (RTESec_Authentication *) RTESec_Authentication::FindByName(client_authentication_buffer[used_method_index].name);
        if(authentication == 0) {
            strcpy(error.errortext,
                   "No suitable authentication method found.");
            SQLDBC_IRuntime::Error ignored;
            releaseSession(sessionID, ignored);
            return false;
        }
        const void *response = 0;
        int   response_length;
        if(!authentication->CreateResponse(client_authentication_buffer[used_method_index].buffer, // client buffer
                                           response,
                                           response_length,
                                           client_authentication_buffer[used_method_index].challenge_response,
                                           client_authentication_buffer[used_method_index].challenge_response_length,
                                           clear_password,
                                           sizeof(tsp00_Name))) {
            strcpy(error.errortext,
                   "Creating server verifier failed.");
            SQLDBC_IRuntime::Error ignored;
            releaseSession(sessionID, ignored);
            return false;
        }
        connect_datapart.addBinaryParameter(client_authentication_buffer[used_method_index].name,
                                            (IFR_UInt4)strlen(client_authentication_buffer[used_method_index].name));
        connect_datapart.addBinaryParameter(response, response_length);
        connectCommandSegment.closePart();
    }

    {
        tsp00_TermId termid;
        char c=' ';
        memset(termid, 0, sizeof(tsp00_TermId));
        sqltermid(termid);
        IFRPacket_Part clientid_part;
        connectCommandSegment.addPart(IFRPacket_PartKind::ClientId_C, clientid_part);
        clientid_part.setArgCount(1);
        clientid_part.AddData(&c, 1);
        clientid_part.AddData(&termid, sizeof(termid));
        connectCommandSegment.closePart();

    }



    IFR_Bool space_option_feature = IFR_ConnectProperty_checkBooleanOption(properties, IFR_CONNECTPROPERTY_SPACEOPTION);
    IFR_Bool multiple_drop_parseid_feature = IFR_ConnectProperty_checkBooleanOption(properties, IFR_CONNECTPROPERTY_MULTIDROPPARSEID);
    IFR_Bool variable_input_feature = IFR_ConnectProperty_checkBooleanOption(properties, IFR_CONNECTPROPERTY_VARIABLEINPUT);
    IFR_Bool optimized_streams_feature = IFR_ConnectProperty_checkBooleanOption(properties, IFR_CONNECTPROPERTY_OPTIMIZEDSTREAMS);
    IFR_Bool check_scrollableoption_feature = IFR_ConnectProperty_checkBooleanOption(properties, IFR_CONNECTPROPERTY_CHECKSCROLLABLE);

    if(space_option_feature ||
       multiple_drop_parseid_feature ||
       variable_input_feature ||
       optimized_streams_feature ||
       check_scrollableoption_feature) {

        IFRPacket_FeaturePart featurepart;
        connectCommandSegment.addPart(featurepart);

        if(space_option_feature) {
            if(featurepart.requestFeature(IFRPacket_Feature::SpaceOption_C)) {
                OOM_ERROR(error);
                return false;
            }
        }

        if(multiple_drop_parseid_feature) {
            if(featurepart.requestFeature(IFRPacket_Feature::MultipleDropParseID_C)) {
                OOM_ERROR(error);
                return false;
            }
        }

        if(variable_input_feature) {
            if(featurepart.requestFeature(IFRPacket_Feature::VariableInput_C)) {
                OOM_ERROR(error);
                return false;
            }
        }

        if(optimized_streams_feature) {
            if(featurepart.requestFeature(IFRPacket_Feature::OptimizedStreams_C)) {
                OOM_ERROR(error);
                return false;
            }
        }

        if(check_scrollableoption_feature) {
            if(featurepart.requestFeature(IFRPacket_Feature::CheckScrollableOption_C)) {
                OOM_ERROR(error);
                return false;
            }
        }
        connectCommandSegment.closePart();
    }
    connectCommandSegment.close();
    // -- send the packet
    SAPDB_Bool success = request(sessionID,
                                 requestpacket->GetRawPacket(),
                                 requestpacket->Length(),
                                 error);
    if(success) {
        SAPDB_Int4 replyLength;
        success = receive(sessionID,
                          sessionInfoReply,
                          replyLength,
                          error);
    }

    if(!success) {
        SQLDBC_IRuntime::Error ignored;
        releaseSession(sessionID, ignored);
        sessionID = -1;
    } else {
        char user_info[64 * 3];
        IFRPacket_ReplyPacket replypacket;
        replypacket.setPacket((tsp1_packet *)(*sessionInfoReply), 0);
        IFRPacket_ReplySegment replysegment(replypacket);
        memset(user_info, 0, sizeof(user_info));
        IFRPacket_Part userinfo_part;
        IFR_Retcode partsearch_rc = replysegment.getPart(IFRPacket_PartKind::UserInfoReturned_C, userinfo_part);
        if(partsearch_rc == IFR_OK) {
            memcpy(user_info, userinfo_part.GetReadData(), userinfo_part.Length());
            createSessionInfo(sessionID,
                              packetSize,
                              packetListSize,
                              packetList,
                              user_info,
                              client_unicode?true:false);
        }
    }
    IFRUtil_Delete(requestpacket, *allocator);
    return success;
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime::createSessionInfo(SAPDB_Int8 session_id,
                                        SAPDB_UInt4 packet_size,
                                        SAPDB_Int4  packet_list_size,
                                        void     ** packet_list,
                                        char        user_info[64 * 3 + 16],
                                        SAPDB_Bool  user_info_ucs2)
{
    SessionInfo *newsessioninfo = (SessionInfo *)
        getGlobalAllocator().Allocate(sizeof(SessionInfo));
    memset(newsessioninfo, 0, sizeof(SessionInfo));
    newsessioninfo->session_id    = session_id;
    newsessioninfo->packet_size  = packet_size;
    newsessioninfo->page_size = 8192; // !! hardcoded
    newsessioninfo->packet_list_size = packet_list_size;
    memcpy(newsessioninfo->packet_list, packet_list, sizeof(void*) * packet_list_size);
    memcpy(newsessioninfo->user_info, user_info, sizeof(newsessioninfo->user_info));
    newsessioninfo->user_info_ucs2 = user_info_ucs2;
    newsessioninfo->next = m_sessioninfolist;
    m_sessioninfolist = newsessioninfo;
    return;
}

//----------------------------------------------------------------------
SAPDB_Bool 
SQLDBC_ClientRuntime::getSessionInformation(SQLDBC_Connection *connection,
                                            SAPDB_Int8& sessionID,
                                            SAPDB_UInt4& packetSize,
                                            SAPDB_UInt4& pageSize,
                                            SAPDB_Int4&  packetListSize,
                                            void **packetList,
                                            char *usergroup,
                                            char *sysdba,
                                            SQLDBC_StringEncoding& userencoding)
{
    // we do not want to become friend to SQLDBC_Connection
    // as this would expose SQLDBC_ClientRuntime, which isn't 
    // wanted. so we have to redefine it partially
    struct Temp_SQLDBC_Connection {
        SQLDBC_ConnectionItemStorage *m_citem;
    };
    Temp_SQLDBC_Connection *tmp = reinterpret_cast<Temp_SQLDBC_Connection *>(connection);
    IFR_Connection *ifr_connnection = (IFR_Connection *)(tmp->m_citem->m_item);

    sessionID = ifr_connnection->getSessionID();
    SessionInfo *it = m_sessioninfolist;
    while(it) {
        if(it->session_id == sessionID) {
            break;
        }
        it = it->next;
    }
    if(it == 0) {
        return false;
    }
    packetSize     = it->packet_size;
    pageSize       = it->page_size;
    packetListSize = it->packet_list_size;
    memcpy(packetList, it->packet_list, packetListSize * sizeof(void*));
    if(it->user_info_ucs2) {
        userencoding = SQLDBC_StringEncodingUCS2Native;
    } else {
        userencoding = SQLDBC_StringEncodingAscii;
    }
    // put in username, usergroup, sysdba from userinfo part
    unsigned char *p = (unsigned char *)(it->user_info);
    memcpy(sysdba, (void *)(p+1), *p);
    sysdba[*p] = '\0';
    if(it->user_info_ucs2) {
        sysdba[(*p) + 1] = '\0';
    }
    p += (*p) + 1; // next dba
    p += (*p) + 1; // usertype
    memcpy(usergroup, (void *)(p + 1), *p);
    usergroup[*p] = '\0';
    if(it->user_info_ucs2) {
        usergroup[(*p) + 1] = '\0';
    }
    return true;
    
}


//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::releaseSession(SAPDB_Int8 sessionID,
                                     SQLDBC_IRuntime::Error& errorMessages)
{
    if(m_tracesettings.profileAction == IFR_TraceProfileAction_COLLECT){
      this->dumpProfile();
    }
    if(sessionID != -1) {
        m_connectlock.lockConnect();
        sqlarelease((SAPDB_Int4)sessionID);
        m_connectlock.releaseConnect();
        if(m_tracewriter) {
            m_tracewriter->flushBuffer();
        }
    } else {
        if(m_tracewriter) {
            m_tracewriter->flushBuffer();
        }
        return false;
    }
    {
        SessionInfo *it = m_sessioninfolist;
        SessionInfo *last_it = 0;
        while(it != 0) {
            if(it->session_id = sessionID) {
                break;
            } else {
                last_it = it;
                it = it->next;
            }
        }
        if(it != 0) {
            if(last_it == 0) {
                m_sessioninfolist = it->next;
            } else {
                last_it->next = it->next;
            }
            IFRUtil_Delete(it, getGlobalAllocator());
        }
    }
    return true;
}

//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::request(SAPDB_Int8 sessionID,
                              void *requestData,
                              SAPDB_UInt4 requestDataLength,
                              SQLDBC_IRuntime::Error& error)
{
    tsp00_ErrText errorText;
    tsp01_CommErr commErr;
    m_connectlock.lockExecute();
    sqlarequest((SAPDB_Int4)sessionID, requestData, requestDataLength, errorText, commErr);
    if(commErr != commErrOk_esp01) {
        m_connectlock.releaseExecute();
        IFR_ErrorHndl tmperror(RTE_IInterface::Instance().Allocator());
        tmperror.setRuntimeError(IFR_ERR_CONNECTION_DOWN_IIS, (int)commErr,
                                 sizeof(errorText), errorText.asCharp());
        tmperror.toRuntimeError(error);
        return false;
    }
    if(m_tracewriter) {
        m_tracewriter->flushBuffer();
    }
    return true;
}

//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::receive(SAPDB_Int8 sessionID,
                              void **replyData,
                              SAPDB_Int4& replyDataLength,
                              SQLDBC_IRuntime::Error& error)
{
    tsp00_ErrText errorText;
    tsp01_CommErr commErr;
    sqlareceive((SAPDB_Int4)sessionID, replyData, replyDataLength, errorText, commErr);
    m_connectlock.releaseExecute();
    if(commErr != commErrOk_esp01) {
        IFR_ErrorHndl tmperror(RTE_IInterface::Instance().Allocator()) ;
        tmperror.setRuntimeError(IFR_ERR_CONNECTION_DOWN_IIS, (int)commErr,
                                 sizeof(errorText), errorText.asCharp());
        tmperror.toRuntimeError(error);
        return false;
    }
    return true;
}

//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::isReplyAvailable(SAPDB_Int8 sessionID)
{
    tsp00_ErrText errorText;
    tsp01_CommErr commErr;
    sqlareplyavailable((SAPDB_Int4)sessionID, errorText, commErr);
    if(commErr == commErrWouldBlock_esp01) {
        return false;
    } else {
        return true;
    }
}

//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::cancelCurrentCommand(SAPDB_Int8 sessionID,
                                           SQLDBC_IRuntime::Error& errorMessages)
{
    // Whether there is a current command or not can only the application decide.
    sqlacancel((SAPDB_Int4)sessionID);
    return true;
}

//----------------------------------------------------------------------
SQLDBC_Spinlock* SQLDBC_ClientRuntime::createSpinlock(SAPDBMem_IRawAllocator& allocator){
    return new IFR_ALLOCATOR(allocator) SQLDBC_ClientRuntime_SpinLock(this);
}
//----------------------------------------------------------------------
SAPDB_Bool SQLDBC_ClientRuntime::releaseSpinlock(SAPDBMem_IRawAllocator& allocator, SQLDBC_Spinlock* lock){
    if (!lock) return false;
    allocator.Deallocate(lock);
    return true;
}

//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::createMutex(SQLDBC_IRuntime::MutexHandle& mutexHandle,
                                  SAPDBMem_IRawAllocator& allocator,
                                  SQLDBC_IRuntime::Error& errorMessages)
{

    teo07_Mutex *hMutex=(teo07_Mutex*)
        allocator.Allocate(sizeof(teo07_Mutex));
    if(hMutex) {
        sqlcreatemutex(hMutex);
    }
    mutexHandle=hMutex;
    return true;
}




//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::lockMutex(SQLDBC_IRuntime::MutexHandle mutexHandle)
{
    teo07_Mutex* hMutex=(teo07_Mutex*)mutexHandle;
    if (!hMutex) return false;
    // printf("LOCK MUTEX %X\n", hMutex);
    sqlbeginmutex(hMutex);
    return true;
}

//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::releaseMutex(SQLDBC_IRuntime::MutexHandle mutexHandle)
{
    teo07_Mutex* hMutex=(teo07_Mutex*)mutexHandle;
    if (!hMutex) return false;
    // printf("RELEASE MUTEX %X\n", hMutex);
    sqlendmutex(hMutex);
    return true;
}

//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::destroyMutex(SQLDBC_IRuntime::MutexHandle& mutexHandle,
                                   SAPDBMem_IRawAllocator& allocator,
                                   SQLDBC_IRuntime::Error& errorMessages)
{
    teo07_Mutex *hMutex = (teo07_Mutex *) mutexHandle;
    if (!hMutex) return false;
    // printf("DESTROY MUTEX %X\n", hMutex);
    sqldestroymutex(hMutex);
    allocator.Deallocate(mutexHandle);
    mutexHandle=0;
    return true;
}


//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::createCounter(CounterHandle& counterHandle,
                                    SAPDBMem_IRawAllocator& allocator,
                                    SQLDBC_IRuntime::Error& errorMessages)
{
    RTESync_InterlockedCounter<long> *counter=new IFR_ALLOCATOR(allocator) RTESync_InterlockedCounter<long>(0);
    counterHandle=counter;
    return true;
}

//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::destroyCounter(CounterHandle counterHandle,
                                     SAPDBMem_IRawAllocator& allocator,
                                     SQLDBC_IRuntime::Error& errorMessages)
{
    RTESync_InterlockedCounter<long> *counter=(RTESync_InterlockedCounter<long> *) counterHandle;
    IFRUtil_Delete(counter, allocator);
    return true;
}

//----------------------------------------------------------------------
SAPDB_UInt4
SQLDBC_ClientRuntime::nextCounter(CounterHandle counterHandle)
{
    RTESync_InterlockedCounter<long> *counter =
        (RTESync_InterlockedCounter<long> *) counterHandle;
    SAPDB_UInt4 result = (SAPDB_UInt4) counter->Increment();
    return result;
}
//----------------------------------------------------------------------
SQLDBC_IRuntime::TaskID
SQLDBC_ClientRuntime::getCurrentTaskID()
{
    return (SQLDBC_IRuntime::TaskID) sqlgetthreadid();
}

//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::createSemaphore(SemaphoreHandle& semaphoreHandle,
                                      SAPDB_Int4 initialValue,
                                      SAPDBMem_IRawAllocator& allocator,
                                      SQLDBC_IRuntime::Error& error)
{
  teo07_ThreadSemaphore *result = new IFR_ALLOCATOR(allocator) teo07_ThreadSemaphore;
  if(result == 0) {
      OOM_ERROR(error);
      return false;
  }
  tsp00_ErrTextc errtext;
  teo07_ThreadErr err;
  sqlcreatesem(result,
               initialValue,
               errtext,
               &err);
  if(err /* != THR_OK_EO07 */ ) {
      IFRUtil_Delete(result, allocator);
      IFR_ErrorHndl tmperror(allocator);
      tmperror.setRuntimeError(IFR_ERR_SYSTEM_ERROR_S,(char *)errtext);
      tmperror.toRuntimeError(error);
      semaphoreHandle = 0;
      return false;
   }
   semaphoreHandle = result;
   return true;
}

//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::waitSemaphore(SemaphoreHandle semaphoreHandle)
{
    teo07_ThreadSemaphore *semaphore =
        (teo07_ThreadSemaphore *) semaphoreHandle;
    if (!semaphore) return false;
    // DEADLOCK DEBUGGING
#if 0
    teo07_ThreadErr err;
    sqltimedwaitsem(*semaphore, 5, &err);
    if(err == THR_TIMEOUT_EO07) {
        __asm { int 3 };
    }
#else
    // printf("-------------------->                     WAIT SEM %p\n", semaphoreHandle);
    sqlwaitsem(*semaphore);

#endif
    return true;
}

//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::signalSemaphore(SemaphoreHandle semaphoreHandle)
{
    teo07_ThreadSemaphore *semaphore =
        (teo07_ThreadSemaphore *) semaphoreHandle;
    if (!semaphore) return false;
    // printf("-------------------->                     SIGNAL SEM %p\n", semaphoreHandle);
    sqlsignalsem(*semaphore);
    return true;
}

//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::destroySemaphore(SemaphoreHandle& semaphoreHandle,
                                       SAPDBMem_IRawAllocator& allocator,
                                       SQLDBC_IRuntime::Error& error)
{
    teo07_ThreadSemaphore *semaphore =
        (teo07_ThreadSemaphore *) semaphoreHandle;
    if (!semaphore) return false;
    sqldestroysem(*semaphore);
    IFRUtil_Delete(semaphore, allocator);
    semaphoreHandle=0;
    return true;
}

//======================================================================
// Tracing
//======================================================================

//----------------------------------------------------------------------
static void initTaskTraceContext(void *p)
{
    SQLDBC_IRuntime::TaskTraceContext* tt =
        (SQLDBC_IRuntime::TaskTraceContext*)p;
    tt->currentEntry      = 0;
    tt->hex               = false;
    tt->inputlength       = 0;
    tt->encoding          = SQLDBC_StringEncodingAscii;
    tt->indent            = 0;
    tt->flags             = 0;
    tt->traceableLimit    = 0;
    tt->pos               = 0;
    return;
}

//----------------------------------------------------------------------
SQLDBC_IRuntime::TaskTraceContext*
SQLDBC_ClientRuntime::getTaskTraceContext()
{
    if(m_traceoptions.flags) {
        SQLDBC_IRuntime::TaskTraceContext *result=
            (SQLDBC_IRuntime::TaskTraceContext *)
            sqltlsgetmemory(0,
                            sizeof(SQLDBC_IRuntime::TaskTraceContext),
                            (SQLGETTLS_InitFuncType)initTaskTraceContext,
                            true);
        if(result) {
            result->flags          = m_traceoptions.flags;
            result->traceableLimit = m_traceoptions.traceableLimit;
        }
        return result;
    } else {
        return 0;
    }
}

//== Trace Output ======================================================

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime::write(const char *s, SAPDB_Int4 size)
{
    if(m_tracewriter) {
        m_tracewriter->write(getTaskTraceContext(), s, size);
    }
    return;
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime::writeln(const char *s, SAPDB_Int4 size)
{
    if(m_tracewriter) {
        m_tracewriter->writeln(getTaskTraceContext(), s, size);
    }
    return;
}


//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime::write_singlethreaded(TaskTraceContext *context, const char *s, SAPDB_Int4 size)
{
    if(m_tracewriter) {
        m_tracewriter->write(context, s, size, false);
    }
    return;
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime::writeln_singlethreaded(TaskTraceContext *context, const char *s, SAPDB_Int4 size)
{
    if(m_tracewriter) {
        m_tracewriter->writeln(context, s, size, false);
    }
    return;
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime::updateTraceFlags(SAPDB_Bool& traceEnabled)
{
    SAPDB_UInt4 oldflags = m_traceoptions.flags;
    checkTraceUpdate();
    if(m_tracewriter && (m_traceoptions.flags != oldflags)) {
        m_tracewriter->flushBuffer();
    }
    if(m_traceoptions.flags || m_tracesettings.stopOnError) {
        traceEnabled = true;
    } else {
        traceEnabled = false;
    }
    return;
}
//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime::dumpProfile()
{
  SQLDBC_ClientRuntime_TraceWriter twriter = SQLDBC_ClientRuntime_TraceWriter();

    char buffer[1024];
    char ignored_errortext[128];
    if(IFRUtil_Configuration::getFullProfileFileName(0, buffer, 1024,
                                                   ignored_errortext, sizeof(ignored_errortext)) == IFR_OK) {
      twriter.setFileName(buffer);
      IFR_Profile* prof = m_profilePointer;
      prof->collectCounters();
      twriter.writeln(0, "[PROFILE]", 9, IFR_FALSE);
      for (int i=0; i < SQLDBC_PROFILE_MAX; i++){
        int len = sp77sprintf(&buffer[0], sizeof(buffer),"%s=%d", prof->getProfileCounterName((IFR_ProfileCountValues) i), prof->getCounter((IFR_ProfileCountValues)i));
        twriter.writeln(0, buffer, len, IFR_FALSE);
      }
    }
}
//----------------------------------------------------------------------
SAPDB_Bool
SQLDBC_ClientRuntime::traceError(SAPDB_Int4 errorcode)
{
    if(m_tracesettings.stopOnError) {
        if(errorcode == m_tracesettings.stopOnErrorCode) {
            ++m_errorcounter;
            if(m_errorcounter > m_tracesettings.stopOnErrorCount) {
                writeln("TRACE STOPPED (STOP ON ERROR)", 29);
                setTraceOptions("", false);
                char flags[256];
                getFlags(flags);
                m_sharedmem.setFlags(flags);

                if(m_tracewriter) {
                    m_tracewriter->flushBuffer();
                }

                return true;
            }
        }
    }

    if(m_tracewriter) {
        m_tracewriter->flushBuffer();
    }

    return false;
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime::getFlags(char *flags)
{
    *flags='\0';
    IFR_Bool optionset=false;
    if(m_tracesettings.flags & 0x01) {
        strcat(flags, "c");
        optionset=true;
    }
    if(m_tracesettings.flags & 0x02) {
        strcat(flags, optionset?":d":"d");
        optionset=true;
    }
    if(m_tracesettings.flags & 0x04) {
        char buffer[100];
        sp77sprintf(buffer, 100, optionset?":p%d":"p%d", m_tracesettings.traceableLimit);
        strcat(flags, buffer);
        optionset=true;
    }
    if(m_tracesettings.flags & 0x08) {
        char buffer[128];
        sp77sprintf(buffer, 128, optionset ? ":G%d" : "G%d", m_tracesettings.profileAction);
        strcat(flags, buffer);
        optionset = true;
      }
    if(m_tracesettings.flags & 0x10) {
      strcat(flags, optionset?":a":"a");
      optionset=true;
    }
    if(m_tracesettings.timestampPrefix) {
        strcat(flags, optionset?":T":"T");
        optionset=true;
    }
    if(m_tracesettings.filesizeLimit > 0) {
        char buffer[100];
        sp77sprintf(buffer, 100, optionset?":s%d":"s%d", m_tracesettings.filesizeLimit);
        strcat(flags, buffer);
    }
    if(m_tracesettings.stopOnError) {
        if(m_tracesettings.stopOnErrorCount == 0) {
            char buffer[100];
            sp77sprintf(buffer, 100, optionset?":e%d":"e%d", m_tracesettings.stopOnErrorCode);
            strcat(flags, buffer);
        } else {
            char buffer[100];
            sp77sprintf(buffer, 100, optionset?":e%d/%d":"e%d/%d",
                        m_tracesettings.stopOnErrorCode,
                        m_tracesettings.stopOnErrorCount);
            strcat(flags, buffer);
        }
    }
    return;
}


//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime::checkTraceUpdate()
{
    if(!m_sharedmem.isCurrent()) {
        // first try the global sledge hammer.
        if(m_sharedmem.mustReReadConfiguration()) {
            getTraceOptionsFromConfig();
            char flags[256];
            getFlags(flags);
            m_sharedmem.setFlags(flags);
        } else if(m_sharedmem.settingsQueried()) {
            char flags[256];
            getFlags(flags);
            m_sharedmem.setFlags(flags);
            return;
        } else if(m_sharedmem.refreshRequested()) {
            getTraceOptionsFromConfig();
            char flags[256];
            getFlags(flags);
            m_sharedmem.setFlags(flags);
        } else {
            char *newflags = m_sharedmem.flagsChanged();
            if(newflags != 0) {
                setTraceOptions(newflags, false);
            } else {
                m_sharedmem.acknowledge();
            }
            return;
        }
        if(m_tracesettings.profileAction == IFR_TraceProfileAction_COLLECT){
          this->dumpProfile();
        } else if (m_tracesettings.profileAction == IFR_TraceProfileAction_RESET){
            (reinterpret_cast<IFR_Profile*>(&(this->m_profile)))->resetProfileCounters();
        }
    }
    return;
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime::addTraceFlags(unsigned int flags)
{
    m_tracesettings.flags |= flags;
    m_traceoptions.flags = m_tracesettings.flags;
    return;
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime::removeTraceFlags(unsigned int flags)
{
    unsigned int mask = 0xFFFFFFFF - flags;
    m_tracesettings.flags &= mask;
    m_traceoptions.flags = m_tracesettings.flags;
    return;
}

//----------------------------------------------------------------------
SAPDBMEM_STATIC_RAW_ALLOCATE(ClientRuntime_InstanceData, sizeof(SQLDBC_ClientRuntime));

static SQLDBC_Int4 ClientRuntime_Initialized;

static SQLDBC_ClientRuntime *ClientRuntime_Instance;

extern "C" void IFR_Connection_GlobalLock_Initialize();


#ifdef UNIX
//----------------------------------------------------------------------
extern "C" void close_appldiag_file(void)
{
    if(have_appldag_file_opened && sql22_fd >=0) {
        have_appldag_file_opened = 0;
        sql22_keep_open = 0;
        close(sql22_fd);
        sql22_fd = -1;
    }
    return;
}
#endif

static void SQLDBC_InitializeExceptionHandling();

//----------------------------------------------------------------------
static void* initializeClientRuntime(void *)
{
#ifdef UNIX
    // 0. disable applcation diagnostic messages written to unix console
    sql22_file = 0;
    sql22_fd = open("test.txt", O_WRONLY);
    if(sql22_fd >= 0) {
        have_appldag_file_opened = 1;
    }

    if(!atexit(&close_appldiag_file)) {
        sql22_keep_open = 1; // we want to avoid continous open and close of
        // /dev/null for performance reasons.
    } else {
        sql22_keep_open = 0;
    }
#endif
     // 1. threading
    teo07_Thread thread;
    tsp00_ErrTextc errortext;
    teo07_ThreadErr threaderror;
    sqlinit_multithreading(&thread,
                           errortext,
                           &threaderror);

    // 2. Ensure that trace is written in case of exception
    SQLDBC_InitializeExceptionHandling();

    void *p = (void *) ClientRuntime_InstanceData;
    ClientRuntime_Instance = new (p) SQLDBC_ClientRuntime();
    return 0;
}

#ifndef WIN32
//----------------------------------------------------------------------
SQLDBC_ClientRuntime::ConnectLock::ConnectLock()
:m_writelocking(false),
 m_waitingreaders(0),
 m_waitingwriters(0),
 m_activereaders(0)
{
    sqlcreatemutex(&m_writerlock);
    tsp00_ErrTextc  errtext;
    teo07_ThreadErr error;
    sqlcreatesem(&m_readersem, 0, errtext, &error);
}

//----------------------------------------------------------------------
SQLDBC_ClientRuntime::ConnectLock::~ConnectLock()
{
    sqldestroymutex(&m_writerlock);
    sqldestroysem(m_readersem);
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime::ConnectLock::lockExecute()
{
    m_datalock.Lock();
    // if we are in the process of acquiring a
    // write lock for a connect, or in the
    // mid of a connect, we must block here

    if(m_writelocking) {
        // we will add us to the queue, and
        // release the lock ...
        m_waitingreaders++;
        m_datalock.Unlock();
        // ... and wait until our semaphore goes to green
        sqlwaitsem(m_readersem);
        // if we have been waked up, the
        // number of active readers is managed by
        // the wakeup call,
        // and the writerlock remains locked.
    } else {
        // add ourself to the number of readers
        m_activereaders++;
        // the first reader must lock the door
        // for any writer
        if(m_activereaders == 1) {
            sqlbeginmutex(&m_writerlock);
        }
        m_datalock.Unlock();
    }
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime::ConnectLock::releaseExecute()
{
    m_datalock.Lock();
    // dumpstate("RELEASE EXECUTE");
    m_activereaders--;

    // if we are the last that goes away, we will unlock the writer
    // lock, and maybe passing a writer (connect) through.
    if(m_activereaders == 0) {
        // printf("LAST READER\n");
        sqlendmutex(&m_writerlock);
    } else {
        // printf("READER # %d\n", m_activereaders);
    }
    m_datalock.Unlock();
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime::ConnectLock::lockConnect()
{
    m_datalock.Lock();

    if(m_writelocking) {
        m_waitingwriters++;
        m_datalock.Unlock();
        sqlbeginmutex(&m_writerlock);
    } else {
        m_writelocking=true;
        m_waitingwriters++;
        m_datalock.Unlock();
        sqlbeginmutex(&m_writerlock);
    }
    m_datalock.Lock();
    m_waitingwriters--;
    m_datalock.Unlock();
}

//----------------------------------------------------------------------
void
SQLDBC_ClientRuntime::ConnectLock::releaseConnect()
{
    m_datalock.Lock();

    if(m_waitingwriters > 0) {
        // simply let the writer through could result
        // in a starvation of the other requests
        if(m_waitingreaders > 0) {
            // ... so we must wake up all those
            // readers that wait.
            for(int i=0; i<m_waitingreaders; ++i) {
                sqlsignalsem(m_readersem);
            }
            // adjust the number of active readers,
            // such that the writer lock is posted
            // when the last reader is gone.
            m_activereaders=m_waitingreaders;
            // the writer lock will remain locked,
            // the last reader will unlock it
            m_waitingreaders=0;
        } else {
            // ... there is no reader waiting for action, we may
            // let the writer through
            sqlendmutex(&m_writerlock);
        }
    } else {
        // There is no writer waiting, so simply wake up all
        // readers, and give up 'writelocking' too.
        if(m_waitingreaders > 0) {
            m_writelocking=false;
            m_activereaders=m_waitingreaders;
            for(int i=0; i<m_waitingreaders; ++i) {
                sqlsignalsem(m_readersem);
            }
            m_waitingreaders=0;
        } else {
            m_writelocking=false;
            sqlendmutex(&m_writerlock);
        }
    }
    m_datalock.Unlock();
}

#endif




//----------------------------------------------------------------------
SQLDBC_IRuntime *
SQLDBC_ClientRuntimeInit()
{
    sqlonce(&ClientRuntime_Initialized,
            (SQLONCE_InitFuncType)initializeClientRuntime,
            0);

    return ClientRuntime_Instance;
}


//======================================================================
//----------------------------------------------------------------------
static SQLDBC_SingleThreadedRuntime *singlethreaded_runtime = 0;

SAPDBMEM_STATIC_RAW_ALLOCATE(singlethreaded_runtime_instance, sizeof(SQLDBC_SingleThreadedRuntime));

    SQLDBC_SingleThreadedRuntime::SQLDBC_SingleThreadedRuntime(SQLDBC_ClientRuntime *baseruntime)
    :m_runtime(baseruntime),
    SQLDBC_IRuntime(&m_dummySpinlock)
    {
    }

    const char * SQLDBC_SingleThreadedRuntime::getIdentifier() const
    {
        return "SQLDBC_SingleThreadClientRuntime <MAJOR_VERSION>.<MINOR_VERSION>.<CORRECTION_LEVEL>    "
            "BUILD <BUILD_NUMBER_EX_STRING>";
    }

    SAPDB_Bool  SQLDBC_SingleThreadedRuntime::checkKeyOption(const char *keyoption,
                                      char *servernode,
                                      SAPDB_Int4& servernodelength,
                                      char *serverdb,
                                      SAPDB_Int4& serverdblength,
                                      SAPDB_Int4& isolationlevel,
                                      SAPDB_Int4& cachelimit,
                                      SAPDB_Int4& sqlmode,
                                      SAPDB_Int4& timeout,
                                      char *username,
                                      SAPDB_Int4& usernamelength,
                                      SQLDBC_StringEncoding& usernameEncoding,
                                      SQLDBC_IRuntime::Error& errorMessages)
    {
        return m_runtime->checkKeyOption(keyoption,
                                         servernode,
                                         servernodelength,
                                         serverdb,
                                         serverdblength,
                                         isolationlevel,
                                         cachelimit,
                                         sqlmode,
                                         timeout,
                                         username,
                                         usernamelength,
                                         usernameEncoding,
                                         errorMessages);
    }

    SAPDBMem_IRawAllocator&  SQLDBC_SingleThreadedRuntime::getGlobalAllocator()
    {
        return m_runtime->getGlobalAllocator();
    }


    SAPDB_Bool  SQLDBC_SingleThreadedRuntime::getSession(const char  *connectUrl,
                                  const char  *connectCommand,
                                  const char  *password,
                                  SAPDB_Int4    passwordLength,
                                  const SQLDBC_StringEncoding commandEncoding,
                                  SAPDB_Int8&  sessionID,
                                  SAPDB_UInt4& packetSize,
                                  SAPDB_Int4&  packetListSize,
                                  void       **packetList,
                                  void       **sessionInfoReply,
                                  SQLDBC_IRuntime::Error& errorMessages,
                                  SAPDBMem_IRawAllocator *allocator)
    {
        return m_runtime->getSession(connectUrl,
                                     connectCommand,
                                     password,
                                     passwordLength,
                                     commandEncoding,
                                     sessionID,
                                     packetSize,
                                     packetListSize,
                                     packetList,
                                     sessionInfoReply,
                                     errorMessages,
                                     allocator);
    }


    SAPDB_Bool  SQLDBC_SingleThreadedRuntime::getSession(const char  *connectUrl,
                                  const char  *connectCommand,
                                  const char  *username,
                                  SAPDB_Int4   usernamelength,
                                  const char  *password,
                                  SAPDB_Int4    passwordLength,
                                  const SQLDBC_StringEncoding commandEncoding,
                                  SAPDB_Int8&  sessionID,
                                  SAPDB_UInt4& packetSize,
                                  SAPDB_Int4&  packetListSize,
                                  void       **packetList,
                                  void       **sessionInfoReply,
                                  SQLDBC_IRuntime::Error& errorMessages,
                                  SAPDBMem_IRawAllocator *allocator)
    {
        return m_runtime->getSession(connectUrl,
                                     connectCommand,
                                     username,
                                     usernamelength,
                                     password,
                                     passwordLength,
                                     commandEncoding,
                                     sessionID,
                                     packetSize,
                                     packetListSize,
                                     packetList,
                                     sessionInfoReply,
                                     errorMessages,
                                     allocator);
    }

    SAPDB_Bool  SQLDBC_SingleThreadedRuntime::releaseSession(SAPDB_Int8 sessionID,
                                      SQLDBC_IRuntime::Error& errorMessages)
    {
        return m_runtime->releaseSession(sessionID, errorMessages);
    }

    SAPDB_Bool  SQLDBC_SingleThreadedRuntime::request(SAPDB_Int8  sessionID,
                               void       *requestData,
                               SAPDB_UInt4 requestDataLength,
                               SQLDBC_IRuntime::Error& errorMessages)
    {
        return m_runtime->request(sessionID, requestData, requestDataLength, errorMessages);
    }

    SAPDB_Bool  SQLDBC_SingleThreadedRuntime::receive(SAPDB_Int8 sessionID,
                               void **replyData,
                               SAPDB_Int4& replyDataLength,
                               SQLDBC_IRuntime::Error& errorMessages)
    {
        return m_runtime->receive(sessionID, replyData, replyDataLength, errorMessages);
    }

    SAPDB_Bool SQLDBC_SingleThreadedRuntime::isReplyAvailable(SAPDB_Int8 sessionID)
    {
        return m_runtime->isReplyAvailable(sessionID);
    }

    SAPDB_Bool  SQLDBC_SingleThreadedRuntime::cancelCurrentCommand(SAPDB_Int8 sessionID,
                                            SQLDBC_IRuntime::Error& errorMessages)
    {
        return m_runtime->cancelCurrentCommand(sessionID, errorMessages);
    }

    //----------------------------------------------------------------------

    SAPDB_Bool  SQLDBC_SingleThreadedRuntime::createMutex(MutexHandle& mutexHandle,
                                   SAPDBMem_IRawAllocator& allocator,
                                   SQLDBC_IRuntime::Error& errorMessages)
    {
        mutexHandle = (MutexHandle) & m_dummy;
        return true;
    }

    SAPDB_Bool  SQLDBC_SingleThreadedRuntime::lockMutex(MutexHandle mutexHandle)
    {
        return true;
    }

    SAPDB_Bool  SQLDBC_SingleThreadedRuntime::releaseMutex(MutexHandle mutexHandle)
    {
        return true;
    }

    SAPDB_Bool  SQLDBC_SingleThreadedRuntime::destroyMutex(MutexHandle& mutexHandle,
                                    SAPDBMem_IRawAllocator& allocator,
                                    SQLDBC_IRuntime::Error& errorMessages)
    {
        return true;
    }


    SAPDB_Bool   SQLDBC_SingleThreadedRuntime::createCounter(CounterHandle& counterHandle,
                                      SAPDBMem_IRawAllocator& allocator,
                                      SQLDBC_IRuntime::Error& errorMessages)
    {
        SAPDB_UInt4 *p = (SAPDB_UInt4 *)allocator.Allocate(sizeof(SAPDB_UInt4));
        *p = 0;
        counterHandle = (CounterHandle *) p;
        return true;
    }

    SAPDB_UInt4  SQLDBC_SingleThreadedRuntime::nextCounter(CounterHandle counterHandle)
    {
        SAPDB_UInt4 *p = (SAPDB_UInt4*) counterHandle;
        return (*p)++;
    }

    SAPDB_Bool   SQLDBC_SingleThreadedRuntime::destroyCounter(CounterHandle counterHandle,
                                       SAPDBMem_IRawAllocator& allocator,
                                       SQLDBC_IRuntime::Error& errorMessages)
    {
        allocator.Deallocate(counterHandle);
        return true;
    }

    SQLDBC_IRuntime::TaskID  SQLDBC_SingleThreadedRuntime::getCurrentTaskID()
    {
        static tsp00_Int4 pid = -1;
        if(pid == -1) {
            sqlgetpid(&pid);
        }
        return (SQLDBC_IRuntime::TaskID)pid;
    }

    SAPDB_Bool  SQLDBC_SingleThreadedRuntime::createSemaphore(SemaphoreHandle& semaphoreHandle,
                                       SAPDB_Int4 initialValue,
                                       SAPDBMem_IRawAllocator& allocator,
                                       SQLDBC_IRuntime::Error& errorMessages)
    {
        semaphoreHandle = (SemaphoreHandle) &m_dummy;
        return true;
    }

    SAPDB_Bool  SQLDBC_SingleThreadedRuntime::waitSemaphore(SemaphoreHandle semaphoreHandle)
    {
        return true;
    }

    SAPDB_Bool  SQLDBC_SingleThreadedRuntime::signalSemaphore(SemaphoreHandle semaphoreHandle)
    {
        return true;
    }

    SAPDB_Bool  SQLDBC_SingleThreadedRuntime::destroySemaphore(SemaphoreHandle& semaphoreHandle,
                                        SAPDBMem_IRawAllocator& allocator,
                                        SQLDBC_IRuntime::Error& errorMessages)
    {
        return true;
    }

    SQLDBC_IRuntime::TaskTraceContext*  SQLDBC_SingleThreadedRuntime::getTaskTraceContext()
    {
        return &m_runtime->m_traceoptions;
    }

    void  SQLDBC_SingleThreadedRuntime::write(const char *s, SAPDB_Int4 size)
    {
        m_runtime->write_singlethreaded(&m_runtime->m_traceoptions, s, size);
    }

    void  SQLDBC_SingleThreadedRuntime::writeln(const char *s, SAPDB_Int4 size)
    {
        m_runtime->writeln_singlethreaded(&m_runtime->m_traceoptions, s, size);
    }

    void  SQLDBC_SingleThreadedRuntime::updateTraceFlags(SAPDB_Bool& traceEnabled)
    {
        m_runtime->updateTraceFlags(traceEnabled);
    }

    void SQLDBC_SingleThreadedRuntime::addTraceFlags(unsigned int flags)
    {
	m_runtime->addTraceFlags(flags);
    }

    void SQLDBC_SingleThreadedRuntime::removeTraceFlags(unsigned int flags)
    {
	m_runtime->removeTraceFlags(flags);
    }


	SAPDB_Int4
	SQLDBC_SingleThreadedRuntime::getTraceOptions(char *options, SAPDB_Int4 length)
	{
		return m_runtime->getTraceOptions(options, length);
	}

	void
	SQLDBC_SingleThreadedRuntime::setTraceOptions(const char *optionstring)
	{
	    m_runtime->setTraceOptions(optionstring);
	}

	SAPDB_Bool
	SQLDBC_SingleThreadedRuntime::traceError(SAPDB_Int4 errorcode)
	{
		return m_runtime->traceError(errorcode);
  }

  SAPDB_Bool
  SQLDBC_SingleThreadedRuntime::getSessionInformation(SQLDBC_Connection *connection,
                                                      SAPDB_Int8& sessionID,
                                                      SAPDB_UInt4& packetSize,
                                                      SAPDB_UInt4& pagesSize,
                                                      SAPDB_Int4&  packetListSize,
                                                      void **packetList,
                                                      char *usergroup,
                                                      char *sysdba,
                                                      SQLDBC_StringEncoding& userencoding)
  {
      return m_runtime->getSessionInformation(connection, sessionID, packetSize, pagesSize, packetListSize, packetList,
                                              usergroup, sysdba, userencoding);
  }
    

SQLDBC_IRuntime *  SQLDBC_SingleThreadedRuntime::GetSingleThreadClientRuntime(char *errorText, const SQLDBC_Int4 errorTextSize ){
    if(singlethreaded_runtime == 0) {
        SQLDBC_ClientRuntime *tmp = (SQLDBC_ClientRuntime *) ::SQLDBC::GetClientRuntime(errorText, errorTextSize);
        if(tmp == 0) {
            return 0;
        }
        void *p = (void *) singlethreaded_runtime_instance;
        singlethreaded_runtime = new (p) SQLDBC_SingleThreadedRuntime(tmp);
        tmp->m_profilePointer = reinterpret_cast<IFR_Profile *>(&(singlethreaded_runtime->m_profile));
    }
    return singlethreaded_runtime;
}


typedef SQLDBC_IRuntime *(*ClientRuntimeInitProc)();

SQLDBC_DLLEXPORT SQLDBC_IRuntime *SQLDBC::GetClientRuntime(char *errorText, const SQLDBC_Int4 errorTextSize )
{
  return SQLDBC_ClientRuntimeInit();
}



SQLDBC_DLLEXPORT SQLDBC_IRuntime *SQLDBC::GetSingleThreadClientRuntime(char *errorText, const SQLDBC_Int4 errorTextSize )
{
    return SQLDBC_SingleThreadedRuntime::GetSingleThreadClientRuntime(errorText,errorTextSize);
}

#ifdef _WIN32

// Do some extra quirks for ensuring that the trace is completely written on exit, and that the trace
// contains a notice about potential problems.
//
// On windows, this is a little bit cumbersome:
// - Potential critical situations may be catched by using vectored exception handling.
// - Unfortunately, vectored exception handling isn't available on all supported versions,
//   so the methods have to be resolved from the kernel32.dll directly.
// - the vectored exception handling method will flush the trace / provide additional info

static HMODULE SQLDBC_kernel32_dll = 0;
static PVOID   (__stdcall * SQLDBC_AddVectoredExceptionHandler)(ULONG, PVECTORED_EXCEPTION_HANDLER) = 0;
static ULONG   (__stdcall * SQLDBC_RemoveVectoredExceptionHandler)(PVOID) = 0;
static PVOID   SQLDBC_VectoredExceptionHandler = 0;
static int     SQLDBC_InExceptionHandling = 0;

void SQLDBC_StackOutput(char const * const str,
                        void * const       pOutContext)
{
    // only called when writer is verified ...
    int length = strlen(str);
    SQLDBC_ClientRuntime_TraceWriter::s_bufferflusher.writer->simpleWrite(str, length);
    SQLDBC_ClientRuntime_TraceWriter::s_bufferflusher.writer->newline();
    SQLDBC_ClientRuntime_TraceWriter::s_bufferflusher.writer->flushBuffer();
    return;
}

static void SQLDBC_RemoveSelf()
{
    SQLDBC_RemoveVectoredExceptionHandler(SQLDBC_VectoredExceptionHandler);
    SQLDBC_VectoredExceptionHandler = 0;
}

SQLDBC_Bool SQLDBC_ExceptionIsWorthBacktrace(PEXCEPTION_POINTERS ExceptionInfo)
{
    if(ExceptionInfo->ExceptionRecord->ExceptionFlags == EXCEPTION_NONCONTINUABLE) {
        return true;
    }
    switch(ExceptionInfo->ExceptionRecord->ExceptionCode) {
    case EXCEPTION_ACCESS_VIOLATION:
    case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
    case EXCEPTION_DATATYPE_MISALIGNMENT:
    case EXCEPTION_FLT_DIVIDE_BY_ZERO:
    case EXCEPTION_ILLEGAL_INSTRUCTION:
    case EXCEPTION_INT_DIVIDE_BY_ZERO:
    case EXCEPTION_NONCONTINUABLE_EXCEPTION:
        return true;
    default:
        return false;
    }

}

LONG CALLBACK SQLDBC_ExceptionHandler(PEXCEPTION_POINTERS ExceptionInfo)
{
    if(!SQLDBC_InExceptionHandling) {
        if(SQLDBC_ExceptionIsWorthBacktrace(ExceptionInfo)) {
            SQLDBC_InExceptionHandling = 1;
            if(SQLDBC_ClientRuntime_TraceWriter::s_bufferflusher.writer &&
               SQLDBC_ClientRuntime_TraceWriter::s_bufferflusher.writer->isOpen()) {
                SQLDBC_ClientRuntime_TraceWriter::s_bufferflusher.writer->flushBuffer();
                SQLDBC_StackOutput("", 0);
                SQLDBC_StackOutput("Exception/Fatal Error occurred in application program, stack backtrace follows.", 0);
                eo670_CTraceStackOCB(&SQLDBC_StackOutput, 0);
            }
            SQLDBC_RemoveSelf();
            SQLDBC_InExceptionHandling = 0;
        }
    }
    return EXCEPTION_CONTINUE_SEARCH;
}


static void SQLDBC_InitializeExceptionHandling()
{
    SQLDBC_kernel32_dll = LoadLibrary("kernel32.dll");
    if(!SQLDBC_kernel32_dll) {
        return;
    }
    SQLDBC_AddVectoredExceptionHandler =
        (PVOID (__stdcall *)(ULONG, PVECTORED_EXCEPTION_HANDLER))
        GetProcAddress(SQLDBC_kernel32_dll,    "AddVectoredExceptionHandler");

    SQLDBC_RemoveVectoredExceptionHandler = (ULONG (__stdcall *)(PVOID))
        GetProcAddress(SQLDBC_kernel32_dll, "RemoveVectoredExceptionHandler");

    if(SQLDBC_AddVectoredExceptionHandler && SQLDBC_RemoveVectoredExceptionHandler) {
        ULONG id = 2;
        SQLDBC_VectoredExceptionHandler =
            SQLDBC_AddVectoredExceptionHandler(id, &SQLDBC_ExceptionHandler);
    }
}

extern "C" {

void SQLDBC_DetachDynamicLibrary()
{
    if(SQLDBC_VectoredExceptionHandler) {
        SQLDBC_RemoveSelf();
    }
    return;
}

}

#else

static void SQLDBC_InitializeExceptionHandling()
{}

#endif
