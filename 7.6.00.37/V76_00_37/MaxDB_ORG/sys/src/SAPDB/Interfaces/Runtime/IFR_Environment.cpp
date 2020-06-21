/*!
  @file           IFR_Environment.cpp
  @author         DL SAP DB INTERFACES
  @ingroup        IFR_Common
  @brief          Global environment
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
#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/IFR_Environment.h"
#include "Interfaces/Runtime/IFR_Connection.h"


#include "SAPDBCommon/SAPDB_ReuseLib.h" 
#include "Interfaces/SQLDBC/SQLDBC_IRuntime.h"
#include "gsp100.h" 

IFR_BEGIN_NAMESPACE

#ifdef WIN32
#  define STRICMP _stricmp
#else
#  define STRICMP strcasecmp
#endif

//======================================================================
// IFR_EnvironmentProfile
//======================================================================
#define MAP_PROFILE_COUNTER_ENV(x) (counter_8[x]+= m_counter_4[x##_ENV])
#define MAP_PROFILE_COUNTER_8_ENV(x) (counter_8[x]+= m_counter_8[x##_8_ENV])

void IFR_EnvironmentProfile::submitCounters(IFR_UInt8 *counter_8){
  MAP_PROFILE_COUNTER_ENV(IFR_CONNECTIONCOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_COMMITCOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_ROLLBACKCOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_SETISOLATIONCOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_SENDCOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_CANCELCOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_STATEMENTCOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_PREPAREDSTATEMENTCOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_NOTSELECTFETCHOPTIMIZED);
  MAP_PROFILE_COUNTER_ENV(IFR_DROPPARSEIDCOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_DROPCURSORCOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_DROPLONGDESCCOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_PREPARECOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_PARSEINFOHITCOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_PARSINFOMISSCOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_PARSEAGAINCOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_PARSEINFOSQUEEZECOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_EXECUTECOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_INSERTCOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_UPDATECOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_SELECTCOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_CALLDBPROCEDURECOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_DELETECOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_FETCHCOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_SQLERRORCOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_ROWNOTFOUNDCOUNT);
  MAP_PROFILE_COUNTER_ENV(IFR_FETCHROWCOUNT);

  MAP_PROFILE_COUNTER_8_ENV(IFR_SENDSIZE);
  MAP_PROFILE_COUNTER_8_ENV(IFR_RECEIVESIZE);
  MAP_PROFILE_COUNTER_8_ENV(IFR_DATAREADSIZE);
  MAP_PROFILE_COUNTER_8_ENV(IFR_DATAWRITESIZE);
  MAP_PROFILE_COUNTER_8_ENV(IFR_LONGDATAREADSIZE);
  MAP_PROFILE_COUNTER_8_ENV(IFR_LONGDATAWRITESIZE);

  this->resetCounters();
}

//----------------------------------------------------------------------
IFR_EnvironmentProfile::IFR_EnvironmentProfile()
:m_ConnectionList(0)
{ 
   this->resetCounters();
}

//----------------------------------------------------------------------
void IFR_EnvironmentProfile::collectCounters() {
   if (!this->m_ConnectionList) return;
  SQLDBC::SQLDBC_ConnectionItemStorageForConnection * profile = static_cast<SQLDBC::SQLDBC_ConnectionItemStorageForConnection*> (this->m_ConnectionList->First());
   if (!profile) return;
   do {
     profile->m_item->getConnection()->collectCounters();
     profile->m_item->getConnection()->submitCounters(this->m_counter_8, this->m_counter_4);
   } while (0 != (profile = static_cast<SQLDBC::SQLDBC_ConnectionItemStorageForConnection*>  (this->m_ConnectionList->Next(*profile))));
}

//----------------------------------------------------------------------

void IFR_EnvironmentProfile::resetCounters(){
      memset(&m_counter_8[0], 0, sizeof(m_counter_8));
      memset(&m_counter_4[0], 0, sizeof(m_counter_4));
}

/**
 * The byte swap kind of the platform.
 */
IFR_ClientSwap IFR_EnvironmentSwapKind=SwapUnknown;

//----------------------------------------------------------------------
IFR_Environment::IFR_Environment(SQLDBC_IRuntime* runtime)
:IFRUtil_RuntimeItem(*runtime)
  {   
    m_tracestream = new IFR_ALLOCATOR(allocator) IFR_TraceStream(this);

    // swapping - order not important 
    if (IFR_EnvironmentSwapKind == SwapUnknown) {
        union {
            tsp00_Int4 asInt;
            char       asChar [4];
        } swapTest;
        swapTest.asInt = 1;
        if (swapTest.asChar [3] == 1) {
            IFR_EnvironmentSwapKind=SwapNormal;
        }
        else if (swapTest.asChar [0] == 1) {
            IFR_EnvironmentSwapKind=SwapFull;
        }
        else {
            IFR_EnvironmentSwapKind=SwapHalf;
        }   
    }
    IFR_TRACE_UPDATE;
    // we bootstrap the call stack.
    if(traceflags()) {
        m_callstackroot.scope   = "?";
        m_callstackroot.file    = __FILE__;
        m_callstackroot.line    = __LINE__;
        m_callstackroot.level   = 0;
        m_callstackroot.context = traceflags();
        m_callstackroot.stream  = m_tracestream;
        m_callstackroot.previous = 0;
        if(m_callstackroot.context->currentEntry == 0) {
            m_callstackroot.context->currentEntry = &m_callstackroot;
        }
        // Bootstrap the tracing global, if necessary. 
        if(traceflags()->currentEntry == 0) {
            traceflags()->currentEntry = &m_callstackroot;
        }
    }
    {
        DBUG_METHOD_ENTER(IFR_Environment, IFR_Environment);
        
        IFRUtil_LinkedList<IFR_Environment> *environment_list = 
            reinterpret_cast<IFRUtil_LinkedList<IFR_Environment> *> 
            (& this->runtime.m_environmentlist);
        environment_list->Insert((IFR_Environment &) *(this));
    }
 }

//----------------------------------------------------------------------
IFR_Environment::~IFR_Environment()
{
    m_callstackroot.stream=0;
    m_callstackroot.context=0;
    if(traceflags()) {
        if(traceflags()->currentEntry == &m_callstackroot) {
            traceflags()->currentEntry = 0;
        }
    }
    this->submitCounters(&(getProfile()->m_counter[0]));
    IFRUtil_LinkedList<IFR_Environment> *environment_list = 
        reinterpret_cast
        <IFRUtil_LinkedList<IFR_Environment> *> (& this->runtime.m_environmentlist);
    environment_list->Remove((IFR_Environment &) *(this));
    IFRUtil_Delete(m_tracestream, allocator);
}

//----------------------------------------------------------------------
int IFR_Environment::getMinorVersion() const
{
    DBUG_METHOD_ENTER(IFR_Environment, getMinorVersion);
    DBUG_RETURN(<MINOR_VERSION>);
}

//----------------------------------------------------------------------
int IFR_Environment::getMajorVersion() const
{
    DBUG_METHOD_ENTER(IFR_Environment, getMajorVersion);
    DBUG_RETURN(<MAJOR_VERSION>);
}

//----------------------------------------------------------------------
IFR_Connection* IFR_Environment::getConnection()
{
    DBUG_METHOD_ENTER(IFR_Environment, getConnection);
    DBUG_RETURN(getConnection(allocator));
}

//----------------------------------------------------------------------
IFR_Connection* IFR_Environment::getConnection
  (SAPDBMem_IRawAllocator& customAllocator) 
{
    DBUG_METHOD_ENTER(IFR_Environment, getConnection);
    IFR_Bool memory_ok = true;
    IFR_Connection *result = 
        new IFR_ALLOCATOR(customAllocator) IFR_Connection(*this, customAllocator, memory_ok);
    if(!memory_ok) {
        IFRUtil_Delete(result, customAllocator);
        result = 0;
    }
    DBUG_RETURN(result);
}


//----------------------------------------------------------------------
void IFR_Environment::releaseConnection(IFR_Connection *conn) 
{
    DBUG_METHOD_ENTER(IFR_Environment, releaseConnection);
    conn->submitCounters(m_counter_8,m_counter_4);
    IFRUtil_Delete<IFR_Connection>(conn, conn->allocator);
}

//----------------------------------------------------------------------
IFR_Int4
IFR_Environment::getRuntimeVersion()
{    
    // static DBUG_METHOD_ENTER(IFR_Environment, getRuntimeVersion);
    return (<MAJOR_VERSION>*10000+<MINOR_VERSION>*100+<CORRECTION_LEVEL>);
}

//----------------------------------------------------------------------
static IFR_Bool
IFR_Environment_checkboolean(const char *str)
{
    if(STRICMP(str, "1") == 0) {
        return true;
    }
    if(STRICMP(str, "YES") == 0) {
        return true;
    }
    if(STRICMP(str, "TRUE") == 0) {
        return true;
    }
    return false;
}


//----------------------------------------------------------------------
void
IFR_Environment::setTraceOptions(const IFR_ConnectProperties& traceoptions)
{
    IFR_String traceoptioncmd(IFR_StringEncodingAscii, allocator);
    IFR_Bool first = true;
    IFR_Bool memory_ok = true;
    const char *sqltrace_str = traceoptions.getProperty("SQL", "0");
    if(IFR_Environment_checkboolean(sqltrace_str)) {
        traceoptioncmd.append("a", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        if(!memory_ok) {
            return;
        }
        first = false;
    }
    const char *longtrace_str = traceoptions.getProperty("LONG", "0");
    if(IFR_Environment_checkboolean(longtrace_str)) {
        if(first) {
            traceoptioncmd.append("d", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
            if(!memory_ok) {
                return;
            } 
            first = false;
        } else {
            traceoptioncmd.append(":d", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
            if(!memory_ok) {
                return;
            } 
        }
    } else {
        const char *shorttrace_str = traceoptions.getProperty("SHORT", "0");
        if(IFR_Environment_checkboolean(shorttrace_str)) {
            if(first) {
                traceoptioncmd.append("c", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
                if(!memory_ok) {
                    return;
                } 
                first = false;
            } else {
                traceoptioncmd.append(":c", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
                if(!memory_ok) {
                    return;
                } 
            }
        }
    }
    const char *packettrace_str = traceoptions.getProperty("PACKET", "0");
    if(IFR_Environment_checkboolean(packettrace_str)) {
        if(first) {
            traceoptioncmd.append("p", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
            if(!memory_ok) {
                return;
            }
            first = false;
        } else {
            traceoptioncmd.append(":p", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
            if(!memory_ok) {
                return;
            }
        }
    }
    const char *timestamptrace_str = traceoptions.getProperty("TIMESTAMP", "0");
    if(IFR_Environment_checkboolean(timestamptrace_str)) {
        if(first) {
            traceoptioncmd.append("T", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
            if(!memory_ok) {
                return;
            }
            first = false;
        } else {
            traceoptioncmd.append(":T", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
            if(!memory_ok) {
                return;
            }
        }
    }
    const char *filename_str = traceoptions.getProperty("FILENAME", 0);
    if(filename_str) {
        if(first) {
            traceoptioncmd.append("f", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
            if(!memory_ok) {
                return;
            }
            first = false;
        } else {
            traceoptioncmd.append(":f", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
            if(!memory_ok) {
                return;
            }
        }
        traceoptioncmd.append(filename_str, IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        if(!memory_ok) {
            return;
        }
    }
    const char *filesize_str = traceoptions.getProperty("FILESIZE", 0);
    if(filesize_str) {
        if(first) {
            traceoptioncmd.append("s", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
            if(!memory_ok) {
                return;
            }
            first=false;
        } else {
            traceoptioncmd.append(":s", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
            if(!memory_ok) {
                return;
            }
        }
        traceoptioncmd.append(filesize_str, IFR_StringEncodingAscii, IFR_NTS, memory_ok);
        if(!memory_ok) {
            return;
        }
    }
    const char *stoponerror_str = traceoptions.getProperty("STOPONERROR", "0");
    if(IFR_Environment_checkboolean(stoponerror_str)) {
        const char *errorcode_str = traceoptions.getProperty("ERRORCODE", 0);
        const char *errorcount_str = traceoptions.getProperty("ERRORCOUNT", 0);
        if(errorcode_str) {
            if(first) {
                traceoptioncmd.append("e", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
                first = false;
            } else {
                traceoptioncmd.append(":e", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
            }
            if(!memory_ok) {
                return;
            }
            traceoptioncmd.append(errorcode_str, IFR_StringEncodingAscii, IFR_NTS, memory_ok);
            if(!memory_ok) {
                return;
            }
            if(errorcount_str) {
                traceoptioncmd.append("/", IFR_StringEncodingAscii, IFR_NTS, memory_ok);
                if(!memory_ok) {
                    return;
                }
                traceoptioncmd.append(errorcount_str, IFR_StringEncodingAscii, IFR_NTS, memory_ok);
                if(!memory_ok) {
                    return;
                }
            }
        }
    }
    runtime.setTraceOptions(traceoptioncmd.getBuffer());
    IFR_TRACE_UPDATE;
}

//----------------------------------------------------------------------
void
IFR_Environment::getTraceOptions(IFR_ConnectProperties& traceoptions)
{
    traceoptions.clear();
    char buffer[1024];
    char *options = buffer;
    IFR_Int4 sz = runtime.getTraceOptions(options, sizeof(buffer));
    if(sz > sizeof(buffer)) {
        options = (char *) alloca(sz);
        runtime.getTraceOptions(options, sz);
    }
    
    IFR_Bool memory_ok = true;
    
    char *p = options;
    while(p && *p) {
        char *cpos = strchr(p, ':');
        if(cpos != 0) {
            *cpos='\0';
        }
        switch(*p) {
        case 'c':
            traceoptions.setProperty("SHORT", "1", memory_ok);
            break;
        }
        if(cpos != 0) {
            p = cpos + 1;
        } else {
            p = 0;
        }
    }
    return;
}

//----------------------------------------------------------------------
void 
IFR_Environment::write(const char *data, IFR_Int4 length)
{
    runtime.write(data, length);
}

//----------------------------------------------------------------------
void 
IFR_Environment::writeln(const char *data, IFR_Int4 length)
{
    runtime.writeln(data, length);
}

//----------------------------------------------------------------------
SQLDBC_IRuntime::TaskTraceContext *
IFR_Environment::traceflags()
{
    return runtime.getTaskTraceContext();
}

//----------------------------------------------------------------------
SAPDBMem_IRawAllocator& 
IFR_Environment::getAllocator()
{
    return allocator;
}

//----------------------------------------------------------------------
void
IFR_Environment::traceError(IFR_Int4 errorcode, IFR_Connection *connection)
{
    if(connection) {
        IFR_Bool oldtrace = ifr_dbug_trace;
        if(runtime.traceError(errorcode)) {
            IFR_TRACE_UPDATE;
        }
        if(oldtrace == true && ifr_dbug_trace == false) {
            connection->stopKerneltrace();
        }
    } else {
        if(runtime.traceError(errorcode)) {
            IFR_TRACE_UPDATE;
        }
        
    }
}

IFR_END_NAMESPACE
