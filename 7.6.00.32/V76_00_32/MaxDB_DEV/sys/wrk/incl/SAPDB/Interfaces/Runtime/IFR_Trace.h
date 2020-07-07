/*!
  @file           IFR_Trace.h
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
#ifndef IFR_TRACE_H
#define IFR_TRACE_H

#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "Interfaces/Runtime/Util/IFRUtil_New.h"
#include "Interfaces/Runtime/IFR_String.h"
#include "Interfaces/Runtime/IFR_Datatypes.h"
#include "SAPDB/Interfaces/SQLDBC/SQLDBC_IRuntime.h"
#include "Interfaces/Runtime/Util/IFRUtil_RuntimeItem.h"

IFR_BEGIN_NAMESPACE

struct IFR_Trace;
class  IFR_ITraceController;
class  IFR_TraceStream;
class  IFR_Connection;
struct inputlength;
struct lpad;
struct inputencoding;
extern SAPDB_Bool ifr_dbug_trace;

#if defined(BIT64)
#  define IFR_SQLTRACE_PARAMETER_HEADER "I   T          AT L          I                  D                  P"
#  define IFR_SQLTRACE_DATA_HEADER      "I   T          AT L          I           DATA"
#else
#  define IFR_SQLTRACE_PARAMETER_HEADER "I   T          AT L          I          D          P"
                                                                              
#  define IFR_SQLTRACE_DATA_HEADER      "I   T          AT L          I           DATA"
#endif

#if defined(IFR_NO_TRACE)
#  define IFR_DBUG_TRACE (false)
#else
#  define IFR_DBUG_TRACE (ifr_dbug_trace)
#endif

/**
 * Number of bytes to be traced of data buffers.
 */
#define IFR_MAX_TRACELENGTH 100

/**
 * @ingroup IFR_Trace
 * Structure to combine some constants.
 */
struct IFR_Trace
{
    /**
     * The kinds of trace that can be emitted from the runtime.
     */
    enum Type {
        CallTrace    = 0x01,  //!< Call trace.
        DebugTrace   = 0x02,  //!< Debug trace.
        PacketTrace  = 0x04,  //!< Packet trace.
        ProfileTrace = 0x08,  //!< Profile trace.
        SQLTrace     = 0x10   //!< SQL trace.
    };
};

/**
 * @ingroup IFR_Trace
 * Interface for the trace controller. The trace controller is the
 * instance that manages the threads and is the single channel to
 * trace output.
 */
class IFR_ITraceController
{
public:
    /** 
     * Writes a string into the stream. 
     * @param data The string to print.
     * @param length The length of the string to print, -1 for
     *   determining the length using <code>strlen</code>.
     */
    virtual void write(const char *data, IFR_Int4 length = -1) = 0;
    
    /**
     * Writes a string into a stream, and appends a newline.
     * @param data The string to print.
     * @param length The length of the string to print, -1 for
     *   determining the length using <code>strlen</code>.
     */
    virtual void writeln(const char *data, IFR_Int4 length = -1) = 0;

    /**
     * Retrieve the context.
     * @return The trace context, or 0 if no context is available.
     */
    virtual SQLDBC_IRuntime::TaskTraceContext *traceflags() = 0;
    
    /**
     * Get the allocator that shall be used when allocating temporary
     * space.
     * @return The allocator that should be used for temporary allocation.
     */
    virtual SAPDBMem_IRawAllocator& getAllocator() = 0;
    

    /**
     * Called when an error had happened.
     * This is used for the "STOP ON ERROR" feature.
     */
    virtual void traceError(IFR_Int4 errorcode, IFR_Connection *connection) = 0;
    
    /**
     * Set the output format to decimal.
     */
    inline void setDecimal()
    {
        SQLDBC_IRuntime::TaskTraceContext *ctx=traceflags();
        if(ctx) {
            ctx->hex=false; 
        }
        return; 
    }
    
    /**
     * Set the output format to hexadecimal.
     */
    inline void setHex()
    {
        SQLDBC_IRuntime::TaskTraceContext *ctx=traceflags();
        if(ctx) {
            ctx->hex=true; 
        }

        return;
    }
    
    /**
     * Set the input length. 
     * @param length The new inputlength. Use 0 or values less 
     * than 0 to unset the input
     */
    inline void setInputLength(IFR_Int4 length)
    {
        SQLDBC_IRuntime::TaskTraceContext *ctx=traceflags();
        if(ctx) {
            if(length < 0) {
                ctx->inputlength = (SAPDB_UInt4)IFR_NTS;
            } else {
                ctx->inputlength = (SAPDB_UInt4) length;
            }
        }
    }

    inline IFR_Int4 getInputLength() 
    {
        SQLDBC_IRuntime::TaskTraceContext *ctx=traceflags();
        if(ctx) {
            return ctx->inputlength;
        } else {
            return 0;
        }
    }
    
    /**
     * Clears all flags that do not persist over the traceing
     * of one value, namely input length and hexadecimal/decimal
     * format.
     */
    inline void clearFlags()
    {
        SQLDBC_IRuntime::TaskTraceContext *ctx=traceflags();
        if(ctx) {
            ctx->hex=false;
            ctx->inputlength=(SAPDB_UInt4)IFR_NTS;
            ctx->encoding=SQLDBC_StringEncodingAscii;
        }
    }
    
    inline void setIndent(IFR_Int4 indent)
    {
        SQLDBC_IRuntime::TaskTraceContext *ctx=traceflags();
        if(ctx) {
            ctx->indent = indent;
        }
    }

    inline IFR_Int4 getIndent() 
    {
        SQLDBC_IRuntime::TaskTraceContext *ctx=traceflags();
        if(ctx) {
            return ctx->indent;
        } else {
            return 0;
        }
    }

    inline IFR_UInt4 getFlags() 
    {
        SQLDBC_IRuntime::TaskTraceContext *ctx=traceflags();
        if(ctx) {
            return ctx->flags;
        } else {
            return 0;
        }
    }

    inline void setFlags(IFR_UInt4 flags)
    {
        SQLDBC_IRuntime::TaskTraceContext *ctx=traceflags();
        if(ctx) {
            ctx->flags = flags;
        }
        return;
    }
    
    inline SQLDBC_StringEncoding getInputEncoding()
    {
        SQLDBC_IRuntime::TaskTraceContext *ctx=traceflags();
        if(ctx) {
            return ctx->encoding;
        } else {
            return SQLDBC_StringEncodingAscii;
        }
    }
    
    inline void setInputEncoding(SQLDBC_StringEncoding encoding)
    {
        SQLDBC_IRuntime::TaskTraceContext *ctx=traceflags();
        if(ctx) {
            traceflags()->encoding = encoding;
        } 
    }
};

#define IFR_TRACE_NULL_CHECK if(this == 0) { return *this; }

/**
 * @ingroup IFR_Trace
 * The trace stream is the logical output interface for the
 * application to produce trace.
 */ 
class IFR_TraceStream
    : public PIn_TraceStream
{
public:
    /**
     * Constructor. 
     * @param controller The trace controller for the low-level operation.
     */ 
    inline IFR_TraceStream(IFR_ITraceController *controller)
    :PIn_TraceStream(0),
     m_controller(controller)
    {}
    
    /**
     * Destructor.
     */
    inline ~IFR_TraceStream()
    {
        m_controller = 0;
    }

    inline IFR_TraceStream& operator <<(void (*f)(IFR_TraceStream& s))
    {
        (*f)(*this);
        return *this;
    }   

    inline IFR_Bool isEnabled(IFR_Trace::Type type) const
    {
        return m_controller->traceflags()->flags & type ? true : false;
    }
    
    inline SQLDBC_IRuntime::TaskTraceContext* getContext()
    {
        if(m_controller) {
            return m_controller->traceflags();
        } else {
            return 0;
        }
    }

    /**
     * Operator that prints out a string. The string is printed up to the 
     * input length set, or the zero terminator, and it is printed as 
     * hex dump if the <code>hex</code> flag was set before on the stream.
     * @param s The string.
     * @return <code>*this</code>
     */
    IFR_TraceStream& operator <<(const char *s);
    
    /**
     * Operator that prints out a string. The string is printed up to the 
     * input length set, or the zero terminator, and it is printed as 
     * hex dump if the <code>hex</code> flag was set before on the stream.
     * @param s The string.
     * @return <code>*this</code>
     */
    IFR_TraceStream& operator <<(const unsigned char *s)
    {
        return (*this) << ((const char *)s);
    }
   
    /**
     * Outputs a character. If the hex flag is set, the character is printed as
     * hex.
     * @param i The character.
     * @return <code>*this</code>.
     */
    IFR_TraceStream& operator <<(const IFR_Int1 i);
    
    /**
     * Outputs an unsigned character. If the hex flag is set, the character is printed as
     * hex.
     * @param ui The character.
     * @return <code>*this</code>.
     */
    IFR_TraceStream& operator <<(const IFR_UInt1 ui);

    /**
     * Outputs a short. If the hex flag is set, the value is printed as
     * hex.
     * @param i The short value.
     * @return <code>*this</code>.
     */
    IFR_TraceStream& operator <<(const IFR_Int2 i);

    /**
     * Outputs an unsigned short. If the hex flag is set, the value is printed as
     * hex.
     * @param ui The unsigned short value.
     * @return <code>*this</code>.
     */
    IFR_TraceStream& operator <<(const IFR_UInt2 ui);

    /**
     * Outputs a 32 bit integer. If the hex flag is set, the value is printed as
     * hex.
     * @param i The integer value.
     * @return <code>*this</code>.
     */
    IFR_TraceStream& operator <<(const IFR_Int4 i);
    
    /**
     * Outputs a 32 bit unsigned integer. If the hex flag is set, the value is printed as
     * hex.
     * @param ui The unsigned integer value.
     * @return <code>*this</code>.
     */
    IFR_TraceStream& operator <<(const IFR_UInt4 ui);

#if defined(WIN32) && !defined(BIT64)
    inline IFR_TraceStream& operator << (const IFR_size_t s) 
    {
        return (*this) << (IFR_Int4)s;
    }
#endif

#if ((defined(AIX) || defined(SUN) || defined(SOLARIS) || defined(LINUX) || defined(HPUX) || defined(NMP)) && !defined(BIT64)) || (defined(WIN32) && defined(BIT64))
    inline IFR_TraceStream& operator << (const long int li) 
    {
        return (*this) << (IFR_Int4)li;
    }

    inline IFR_TraceStream& operator << (const unsigned long int uli) 
    {
        return (*this) << (IFR_UInt4)uli;
    }
#endif

#if (defined(OSF1) || defined(AIX) || defined(SUN) || defined(SOLARIS) || defined(LINUX) || defined(HPUX) || defined(NMP)) && defined(BIT64)
    inline IFR_TraceStream& operator << (const long int li) 
    {
        return (*this) << (IFR_Int8)li;
    }

    inline IFR_TraceStream& operator << (const unsigned long int uli) 
    {
        return (*this) << (IFR_UInt8)uli;
    }
#endif


    /**
     * Outputs a 64 bit integer. If the hex flag is set, the value is printed as
     * hex.
     * @param i The integer value.
     * @return <code>*this</code>.
     */
    IFR_TraceStream& operator <<(const IFR_Int8 i);
    
    /**
     * Outputs a 64 bit unsigned integer. If the hex flag is set, the value is printed as
     * hex.
     * @param ui The unsigned integer value.
     * @return <code>*this</code>.
     */
    IFR_TraceStream& operator <<(const IFR_UInt8 ui);

    /**
     * Outputs a double value.
     * @param d The double value.
     * @return <code>*this</code>.
     */
    IFR_TraceStream& operator <<(const double d);
    

    /**
     * Traces a #SQL_DATE_STRUCT.
     * @param date The #SQL_DATE_STRUCT to trace.
     * @return <code>*this</code>
     */
    IFR_TraceStream& operator <<(const SQL_DATE_STRUCT& date);

    /**
     * Traces a #SQL_TIME_STRUCT.
     * @param time The #SQL_DATE_STRUCT to trace.
     * @return <code>*this</code>
     */
    IFR_TraceStream& operator <<(const SQL_TIME_STRUCT& time);

    /**
     * Traces a #SQL_TIMESTAMP_STRUCT.
     * @param timestamp The #SQL_TIMESTAMP_STRUCT to trace.
     * @return <code>*this</code>
     */
    IFR_TraceStream& operator <<(const SQL_TIMESTAMP_STRUCT& timestamp);

    /**
     * Traces a #SQL_NUMERIC_STRUCT.
     * @param numeric The #SQL_NUMERIC_STRUCT to trace.
     * @return <code>*this</code>
     */
    IFR_TraceStream& operator <<(const SQL_NUMERIC_STRUCT& numeric);

    /**
     * Traces a return code, i.e. prints its textual representation.
     * @param retcode The return code to trace.
     * @return <code>*this</code>
     */
    IFR_TraceStream& operator <<(const IFR_Retcode retcode);

    /**
     * Traces a SQL mode.
     * @param sqlmode The sql mode to trace.
     * @return <code>*this</code>
     */
    IFR_TraceStream& operator <<(const IFR_SQLMode sqlmode);

    /**
     * Traces a <code>void *</code> pointer.
     * @param p The traced pointer. 
     * @return <code>*this</code>.
     */
    IFR_TraceStream& operator <<(const void *p);

#if !(defined(SUN) || defined(SOLARIS))
    /**
     * Traces a boolean.
     * @param b The boolean value to trace.
     * @return <code>*this</code>.
     */
    IFR_TraceStream& operator <<(const IFR_Bool b);
#endif

    /**
     * Traces a database packet.
     * @param packet The packet to trace.
     * @return <code>*this</code>
     */
    IFR_TraceStream& operator <<(PIn_SQLPacket& packet);

    /**
     * Traces a database packet segment.
     * @param segment The packet segment to trace.
     * @return <code>*this</code>
     */
    IFR_TraceStream& operator <<(PIn_Segment& segment);

    /**
     * Traces a database packet segment part.
     * @param part The packet segment part to trace.
     * @return <code>*this</code>
     */
    IFR_TraceStream& operator <<(PIn_Part& part);
    
    /**
     * Writes the given string into the output and appends a
     * trailing newline. 
     * @param name The str.
     * @param len The length of the string, or -1 if the number of
     *        bytes to print shall be determined using <code>strlen</code>.
     */
    inline void writeln(const char *str, int len)
    {
        if(this==0) { return; }
        if(m_controller) {
            m_controller->writeln(str, len);
        }
    }
    
    
    /**
     * Writes the given string into the output.
     * @param name The name.
     * @param len The length of the string, or -1 if the number of
     *        bytes to print shall be determined using <code>strlen</code>.
     */
    inline void write(const char *str, int len=-1)
    {
        if(this==0) { return; }
        if(m_controller) {
            m_controller->write(str, len);
        }
    }

    /**
     * Prints the given number of bytes from a string.
     * @param s The string.
     * @param n The number of bytes.
     */
    void print(const char *s, IFR_Int4 n=-1)
    {
        if(this==0) { return; }
        if(m_controller) {
            m_controller->write(s, n);
        }
    }
    
    /**
     * Prints the given number of bytes from a string and 
     * appends a newline.
     * @param s The string.
     * @param n The number of bytes.
     */
    void println(const char *s, IFR_Int4 n)
    {
        if(this==0) { return; }
        if(m_controller) {
            m_controller->writeln(s, n);
        }
    }

    virtual void hexFromFor (const char * comment, const void * bufParm,  int startPos,   int len);

    /**
     * Called when an error is traced.
     */
    inline void traceError(IFR_Int4 errorcode, IFR_Connection *connection)
    {
        if(m_controller) m_controller->traceError(errorcode, connection);
    }
private:
    IFR_ITraceController *m_controller;  //!< The trace controller used.

    friend void hex(IFR_TraceStream& s);
    friend void dec(IFR_TraceStream& s);
    friend void endl(IFR_TraceStream& s);
    friend void stamp(IFR_TraceStream& s);
    friend void currenttime(IFR_TraceStream& s);
    friend IFR_TraceStream& operator<<(IFR_TraceStream& s, const inputlength& l);
    friend IFR_TraceStream& operator<<(IFR_TraceStream& s, const lpad& l);
    friend IFR_TraceStream& operator<<(IFR_TraceStream& s, const inputencoding& i);
    friend IFR_TraceStream& operator<<(IFR_TraceStream& s, const IFR_String& p );
};

//----------------------------------------------------------------------
inline SQLDBC_IRuntime::TaskTraceContext *
IFR_GetTraceContext(IFR_TraceStream* tracestream)
{
    if(tracestream) {
        return tracestream->getContext();
    } else {
        return 0;
    }
}

/**
 * @ingroup IFR_Trace
 * Function to retrieve trace stream from itself.
 * @param tracestream The trace stream.
 * @return <code>tracestream</code>.
 */ 
inline IFR_TraceStream*
IFR_GetTraceStream(IFR_TraceStream* tracestream)
{
    return tracestream;
}

/**
 * @ingroup IFR_Trace
 * Function that switches the output to hexadecimal. Strings are
 * dumped hexified, and integer numbers are printed in hexadecimal
 * format.  Usually this is used:
 * <pre>stream << hex;</pre>.
 * @param stream The stream where to switch the output modus.
 */
void hex(IFR_TraceStream& s);

/**
 * @ingroup IFR_Trace 
 * Function that switches the output to decimal. This means a strings
 * are written out normally, and integer values are printed as decimal
 * numbers. There is seldom a need for doing this, as the standard
 * format used is <i>decimal</i>.  Usually this is used:
 * <pre>stream << dec;</pre>.  
 * @param stream The stream where to switch the output modus.
 */
void dec(IFR_TraceStream& s);


void endl(IFR_TraceStream& s);
void stamp(IFR_TraceStream& s);
void currenttime(IFR_TraceStream& s);

/**
 * Helper class for setting the input length.
 */
struct inputlength
{
    inline inputlength(IFR_Int4 l):length(l) {}
    IFR_Int4 length;
};

/**
 * Helper class for setting the input length.
 */
struct lpad
{
    inline lpad(IFR_Int4 i):data(i) {}
    IFR_Int4 data;
};

struct inputencoding
{
    inline inputencoding(IFR_StringEncoding encoding)
    :data(encoding)
    {}
    IFR_StringEncoding data;
};

IFR_TraceStream& operator<<(IFR_TraceStream& s, const inputlength& l);
IFR_TraceStream& operator<<(IFR_TraceStream& s, const lpad& i);
IFR_TraceStream& operator<<(IFR_TraceStream& s, const inputencoding& i);

/**
 * @ingroup IFR_Trace
 * Information about the current call.
 */
struct IFR_CallStackInfo
{
    inline IFR_CallStackInfo()
    :context(0),
     stream(0),
     previous(0)
    {}

    inline ~IFR_CallStackInfo();
    const char        *scope;                    //!< The scope entered.
    const char        *file;                     //!< The file.
    IFR_Int4     line;                           //!< The line
    IFR_Int4     level;                          //!< The level
    SQLDBC_IRuntime::TaskTraceContext *context;  //!< The context.
    IFR_TraceStream                   *stream;   //!< The stream.
    IFR_CallStackInfo                 *previous; //!< Link to previous.
};

/**
 * Helper function to determine whether a specific trace
 * level is enabled. 
 * @param The context in which the lookup takes place.
 * @param The tracelevel to look for.
 * @return <code>true</code> if the context makes it possible
 *         to access the trace context, and the trace level
 *         is enabled.
 */
template <class Context>
inline IFR_Bool 
IFR_IsTraceEnabled(Context *context, int tracelevel)
{
    SQLDBC_IRuntime::TaskTraceContext *trace_context=IFR_GetTraceContext(context);
    if(trace_context == 0) {
        return false;
    } else {
        return trace_context->flags & tracelevel ? true : false;
    }
}



/**
 * General macro to get the trace stream.
 * @param context The context where to get the stream from.
 * @param level The requested trace level.
 */
#define IFR_TRACESTREAM(context, level)                         \
    if(!IFR_DBUG_TRACE ||                                       \
       !IFR_IsTraceEnabled(context, (level)) ||                 \
       (IFR_GetTraceStream(context)==0) ) {                     \
    } else (* IFR_GetTraceStream(context) ) 

#define IFR_UNCOND_TRACESTREAM(context)                                     \
  if(IFR_GetTraceStream(context)==0) {} else (*IFR_GetTraceStream(context))

/**
 * Get the DEBUG/CALL trace stream.
 * @deprecated Use IFR_DEBUG_TRACE or IFR_CALL_TRACE instead.
 */
#define DBUG_TRACE if(IFR_DBUG_TRACE) IFR_TRACESTREAM(&__callstackinfo, IFR_Trace::CallTrace|IFR_Trace::DebugTrace)
#define IFR_STATIC_DEBUG_TRACE(context)   IFR_TRACESTREAM(context, IFR_Trace::DebugTrace)
#define IFR_STATIC_CALL_TRACE(context)    IFR_TRACESTREAM(context, IFR_Trace::CallTrace)
#define IFR_STATIC_PACKET_TRACE(context)  IFR_TRACESTREAM(context, IFR_Trace::PacketTrace)
#define IFR_STATIC_PROFILE_TRACE(context) IFR_TRACESTREAM(context, IFR_Trace::ProfileTrace)
#define IFR_STATIC_SQL_TRACE(context) IFR_TRACESTREAM(context, IFR_Trace::SQLTrace)

#define IFR_DEBUG_TRACE IFR_STATIC_DEBUG_TRACE(&__callstackinfo)
#define IFR_CALL_TRACE IFR_STATIC_CALL_TRACE(&__callstackinfo)
#define IFR_PACKET_TRACE IFR_STATIC_PACKET_TRACE(&__callstackinfo)
#define IFR_PROFILE_TRACE IFR_STATIC_PROFILE_TRACE(&__callstackinfo)
#define IFR_SQL_TRACE if (IFR_DBUG_TRACE) IFR_STATIC_SQL_TRACE(&__callstackinfo)
#define IFR_SQL_TRACE_IF(x) if (IFR_DBUG_TRACE && (x)) IFR_STATIC_SQL_TRACE(&__callstackinfo)

#define IFR_TRACE_STREAM (*IFR_GetTraceStream(&__callstackinfo))

#define IFR_SQL_TRACE_ENABLED (IFR_DBUG_TRACE && IFR_IsTraceEnabled(&__callstackinfo, IFR_Trace::SQLTrace) && (IFR_GetTraceStream(&__callstackinfo)!=0))

template <class Context>
void IFR_TraceEnter(Context *context, 
                    IFR_CallStackInfo& callstackinfo, 
                    const char *scope,
                    const char *file, 
                    const IFR_Int4 line)
{
    SQLDBC_IRuntime::TaskTraceContext *taskTraceContext
        = IFR_GetTraceContext(context);
    if(taskTraceContext == 0) {
        memset(&callstackinfo, 0, sizeof(IFR_CallStackInfo));
        return;
    }
    callstackinfo.context = taskTraceContext;
    callstackinfo.previous = 
        (IFR_CallStackInfo *)taskTraceContext->currentEntry;
    callstackinfo.stream = (callstackinfo.previous) 
        ? callstackinfo.previous->stream 
        : IFR_GetTraceStream(context);
    callstackinfo.level = (callstackinfo.previous)
        ? callstackinfo.previous->level + 1
        : 1;
    callstackinfo.scope=scope;
    callstackinfo.line=line;
    callstackinfo.file=file;
    taskTraceContext->currentEntry=&callstackinfo;

    // Only if we have a stream ...
    if(callstackinfo.stream) {
        IFR_STATIC_CALL_TRACE(context) << ">" 
                                       << callstackinfo.scope 
                                       << lpad(callstackinfo.level * 2)
                                       << endl;
    }
    
}

/**
 * @ingroup IFR_Trace
 * Method entry trace object for methods that have no association
 * to the environment under which they run.
 * @param clazz The class.
 * @param method The method.
 * @param context The context class from which the trace stream and -flags are
 *                retrieved.
 */
#define DBUG_CONTEXT_METHOD_ENTER(clazz, method, context)                        \
IFR_CallStackInfo __callstackinfo;                                               \
if(IFR_DBUG_TRACE) IFR_TraceEnter(context, __callstackinfo, #clazz "::" #method, \
               __FILE__, __LINE__)

/**
 * @ingroup IFR_Trace
 * 'Enter' trace macro for non-static methods.
 * @param clazz The class name (not enclosed in "").
 * @param method The method name (not enclosed in "").
 */
#define DBUG_METHOD_ENTER(clazz, method) \
  DBUG_CONTEXT_METHOD_ENTER(clazz, method, this)

/**
 * @ingroup IFR_Trace
 * Template helper method for the <code>DBUG_RETURN</code> macro.
 * The argument is traced and then returned.
 * @param v The value to trace.
 * @param callstackinfo The current callstack info structure.
 * @return <code>v</code>, unchanged.
 */
template <class Value>
inline const Value&
IFR_TraceReturn(const Value& v, IFR_CallStackInfo& callstackinfo)
{
    IFR_STATIC_CALL_TRACE(callstackinfo.stream) 
        << lpad((callstackinfo.level-1)*2) 
        << "<="
        << v
        << lpad((callstackinfo.level)*2)
        << endl;
    return v; 
}


/**
 * Returns from a method, and traces the return value.
 * Only a method entered with DBUG_METHOD_ENTER or
 * DBUG_STATIC_METHOD_ENTER can use this macro.
 * @param x The return value. The stream operator of the return
 *   value is used to print the value.
 */
#define DBUG_RETURN(x) return (IFR_DBUG_TRACE) ? IFR_TraceReturn(x, __callstackinfo) : x

/**
 * Trace macro for printing a casted return value.
 * @param t The type to which the return value is casted.
 * @param v The value.
 * @param s The name of the the value if it should be differ from the original
 * name given by x.
 */
#define DBUG_RETURNC(type, value) return (IFR_DBUG_TRACE) ? IFR_TraceReturn((type) value, __callstackinfo) : (type) value

#define DBUG_PRINTS(name, value) if(IFR_DBUG_TRACE) IFR_CALL_TRACE << #name << "=" << (value) << endl

#define DBUG_PRINT(value) DBUG_PRINTS(value, value)

#define DBUG_PRINTC(type, value) if(IFR_DBUG_TRACE) IFR_CALL_TRACE << #value << "=" << (type value) << endl

/**
 * Prints the given buffer with length honoring the encoding and limit it
 * with a maximum length.
 * @param buffer The buffer to print.
 * @param len The length of the buffer.
 * @param limit The maximum bytes to print.
 * @param encoding The encoding of the buffer.
 */
#define DBUG_PRINT_BUFFER(buffer, len, limit, encoding) if(IFR_DBUG_TRACE) \
    DBUG_TRACE << #buffer"(" << (IFR_Int4)len << ")='" \
               << inputlength(MIN(limit, len)) \
               << inputencoding(encoding) \
               << buffer \
               << (len>limit ? "..." : "'") << endl    

//----------------------------------------------------------------------
inline SQLDBC_IRuntime::TaskTraceContext *
IFR_GetTraceContext(IFR_CallStackInfo* callstackinfo)
{
    if(callstackinfo) {
        return callstackinfo->context;
    } else {
        return 0;
    }
}


//----------------------------------------------------------------------
inline IFR_TraceStream*
IFR_GetTraceStream(IFR_CallStackInfo* callstackinfo)
{
    if(callstackinfo) {
        return callstackinfo->stream;
    } else {
        return 0;
    }   
}

//----------------------------------------------------------------------
inline 
IFR_CallStackInfo::~IFR_CallStackInfo()
{
    if(context) {
        if(context->currentEntry) {
            context->currentEntry=previous;
        }
        if(IFR_DBUG_TRACE) {
            IFR_STATIC_CALL_TRACE(stream) << lpad((level-1)*2) ;
        }
    }
}

template <class Value>
IFR_TraceStream& operator << (IFR_TraceStream& s, const Value *valueptr)
{
    if(valueptr == 0) {
        return s << "(null)";
    } else {
        return s << "&(" << (*valueptr) << ")";
    }
}

#if defined(IFR_NO_TRACE)
#  define IFR_STATIC_TRACE_UPDATE(runtime_item) (runtime_item)
#else
#  define IFR_STATIC_TRACE_UPDATE(runtime_item) (runtime_item).runtime.updateTraceFlags(ifr_dbug_trace)
#endif
#define IFR_TRACE_UPDATE IFR_STATIC_TRACE_UPDATE(*this)


#if defined(USE_SP77)
#define IFR_Trace_sprintf sp77sprintf
#else
inline int IFR_Trace_sprintf(char *str, size_t len, const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    int result = vsprintf(str, format, ap);
    va_end(ap);
    return result;
}
#endif  

IFR_END_NAMESPACE

#endif
