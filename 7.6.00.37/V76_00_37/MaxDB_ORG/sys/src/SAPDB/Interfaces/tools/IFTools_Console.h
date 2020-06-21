/*!
  @file           IFTools_Console.h
  @author         D039759
  @ingroup        IFR_Trace
  @brief          Tools for SQLDBC observation and configuration.

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
#ifndef IFTOOLS_CONSOLE_H
#define IFTOOLS_CONSOLE_H

#include "SAPDB/Interfaces/sapdbifr.h"
#include "SAPDB/Interfaces/SQLDBC/SQLDBC_ClientRuntime.h"
#include "SAPDB/RunTime/System/RTESys_IO.h"

class IFTools_Output
    : public IFR_ITraceController
{
public:
    IFTools_Output(const char *filename);
    virtual ~IFTools_Output();
    
    virtual void write(const char *data, IFR_Int4 length);
    virtual void writeln(const char *data, IFR_Int4 length);
    virtual SQLDBC_IRuntime::TaskTraceContext *traceflags();
    virtual SAPDBMem_IRawAllocator& getAllocator();
    virtual void traceError(IFR_Int4 errorcode, IFR_Connection* connection);
private:

    IFR_Int4                          m_filehandle;
    SQLDBC_IRuntime::TaskTraceContext m_traceoptions;
};


class IFTools_Console
{
public:
    IFTools_Console();
    ~IFTools_Console();
    
    IFR_Retcode parseArguments(int argc, char ** argv);
    IFR_Retcode run();

    inline IFR_TraceStream& stdoutstream() { return *m_stdout; }
    inline IFR_TraceStream& stderrstream() { return *m_stderr; }
private:
    enum Command {
        C_NONE                   = 0,
        C_HELP,
        C_CONFIG_TRACE_SHORT,
        C_CONFIG_TRACE_LONG,
        C_CONFIG_TRACE_PROFILE,
        C_CONFIG_TRACE_PACKET,
        C_CONFIG_TRACE_SQL,
        C_CONFIG_TRACE_TIMESTAMP,
        C_CONFIG_TRACE_SIZE,
        C_CONFIG_TRACE_FILENAME,
        C_CONFIG_TRACE_STOP_ON_ERROR,
        C_CONFIG_TRACE_COMPRESS,
        
        C_TRACE_SHORT,
        C_TRACE_LONG,
        C_TRACE_PROFILE,
        C_TRACE_PACKET,
        C_TRACE_SQL,
        C_TRACE_TIMESTAMP,
        C_TRACE_SIZE,
        C_TRACE_REFRESH,
        C_TRACE_OFF,
        C_TRACE_STOP_ON_ERROR,
        C_TRACE_COMPRESS,
        C_PROFILE_RESET,
        C_PROFILE_COLLECT_ON,
        C_PROFILE_COLLECT_OFF,
        C_PROFILE_FILENAME,
        
        C_SHOW_CONFIG,
        C_SHOW_TRACESETTINGS,
        C_SHOW_TRACEFILE,
        C_SHOW_PROFILE,
        C_SHOW_PROFILE_DETAIL,
        C_SHOW_ALL
    };

    IFTools_Output  *m_stdoutwriter;
    IFR_TraceStream *m_stdout;
    IFTools_Output  *m_stderrwriter;
    IFR_TraceStream *m_stderr; 
    Command          m_command;
    IFR_Bool         m_verbose;
    IFR_Bool         m_force;
    IFR_Bool         m_pidpresent;
    IFR_Int4         m_pid;
    IFR_Bool         m_on;
    IFR_Int4         m_size;
    IFR_Bool         m_configoption;
    char            *m_filename;
    char            *m_profilename;
    char            *m_user;
    IFR_Bool         m_stop_on_error;
    IFR_Int4         m_errorcode;
    IFR_Int4         m_errorcount;

    void        usage(IFR_TraceStream& tracestream);
    IFR_Retcode showConfig();
    IFR_Retcode showTracesettings();
    IFR_Retcode showProfile(IFR_Bool showDetails);
    IFR_Retcode showFile();
    
    IFR_Retcode configTraceShort();
    IFR_Retcode configTraceLong();
    IFR_Retcode configTraceTimestamp();
    IFR_Retcode configTraceProfile(IFRUtil_TraceSharedMemory_TraceProfileAction type);
    IFR_Retcode configTracePacket();
    IFR_Retcode configTraceCompress();
    IFR_Retcode configTraceSQL();
    IFR_Retcode configTraceSize();
    IFR_Retcode configTraceFilename();
    IFR_Retcode configProfileFilename();
    IFR_Retcode configTraceStopOnError();
    
    IFR_Retcode traceRefresh();
    IFR_Retcode traceShort();
    IFR_Retcode traceLong();
    IFR_Retcode traceTimestamp();
    IFR_Retcode traceProfile(IFRUtil_TraceSharedMemory_TraceProfileAction type);
    IFR_Retcode tracePacket();
    IFR_Retcode traceSQL();
    IFR_Retcode traceSize();
    IFR_Retcode traceOff();
    IFR_Retcode traceStopOnError();
};


#endif
