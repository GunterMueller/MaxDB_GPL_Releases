/*!
  @file           IFR_TraceContext.cpp
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
#include "Interfaces/Runtime/IFR_Common.h"
#include "Interfaces/Runtime/IFR_TraceContext.h"
#include "Interfaces/Runtime/IFR_ConnectionItem.h"
#include "Interfaces/Runtime/Util/IFRUtil_RuntimeItem.h"
#include "Interfaces/Runtime/IFR_Environment.h"
#include "Interfaces/Runtime/IFR_Connection.h"
#include "Interfaces/Runtime/IFR_ResultSet.h"
#include "Interfaces/Runtime/IFR_RowSet.h"
#include "Interfaces/Runtime/IFR_FetchInfo.h"
#include "Interfaces/Runtime/IFR_Statement.h"

IFR_BEGIN_NAMESPACE

//----------------------------------------------------------------------
SQLDBC_IRuntime::TaskTraceContext *IFR_GetTraceContext(IFR_ErrorHndl* errhndl)
{
    if(errhndl) {
        if(errhndl->getConnection()) {
            return (errhndl->getConnection()->getEnvironment().traceflags());
        }
    }
    return 0;
}

//----------------------------------------------------------------------
IFR_TraceStream*
IFR_GetTraceStream(IFR_ErrorHndl* errhndl)
{
    if(errhndl) {
        if(errhndl->getConnection()) {
            return (errhndl->getConnection()->getEnvironment().getTraceStream());
        }
    }
    return 0;
}

//----------------------------------------------------------------------
SQLDBC_IRuntime::TaskTraceContext *IFR_GetTraceContext(IFR_ConnectionItem* connectionitem)
{
    if(connectionitem) {
        if(connectionitem->getConnection()) {
            return ((IFR_ConnectionItem*)connectionitem)->getConnection()->getEnvironment().traceflags();
        }
    }
    return 0;
}

//----------------------------------------------------------------------
IFR_TraceStream*
IFR_GetTraceStream(IFR_ConnectionItem *connectionitem)
{
    if(connectionitem) {
        if(((IFR_ConnectionItem*)connectionitem)->getConnection()) {
            return ((IFR_ConnectionItem*)connectionitem)->getConnection()->getEnvironment().getTraceStream();
        }
    }
    return 0;
}

//----------------------------------------------------------------------
SQLDBC_IRuntime::TaskTraceContext* 
IFR_GetTraceContext(const IFRUtil_RuntimeItem* runtimeitem)
{
    if(runtimeitem == 0) {
        return 0;
    }

    IFRUtil_RuntimeItem *ri = (IFRUtil_RuntimeItem *)runtimeitem;
    
    SQLDBC_IRuntime::TaskTraceContext* context=
        ri->runtime.getTaskTraceContext();
    if(context && context->currentEntry) {
        IFR_CallStackInfo *ci=(IFR_CallStackInfo*)context->currentEntry;
        if(ci && ci->stream) {
            return context;
        }
    } 
    return 0;
}

//----------------------------------------------------------------------
SQLDBC_IRuntime::TaskTraceContext* 
IFR_GetTraceContext(SQLDBC_IRuntime* runtime)
{
    if(runtime == 0) {
        return 0;
    }
    SQLDBC_IRuntime::TaskTraceContext* context= runtime->getTaskTraceContext();
    if(context && context->currentEntry) {
        IFR_CallStackInfo *ci=(IFR_CallStackInfo*)context->currentEntry;
        if(ci->stream) {
            return context;
        }
    } 
    return 0;
}

//----------------------------------------------------------------------
IFR_TraceStream* 
IFR_GetTraceStream(SQLDBC_IRuntime* runtime)
{
    SQLDBC_IRuntime::TaskTraceContext *context=IFR_GetTraceContext(runtime);
    if(context && context->currentEntry) {
        IFR_CallStackInfo *ci=(IFR_CallStackInfo*)context->currentEntry;
        if(ci) {
            return ci->stream;
        } else {
            return 0;
        }
    } 
    return 0;
}


//----------------------------------------------------------------------
IFR_TraceStream* 
IFR_GetTraceStream(const IFRUtil_RuntimeItem* runtimeitem)
{
    SQLDBC_IRuntime::TaskTraceContext *context=IFR_GetTraceContext(runtimeitem);
    if(context && context->currentEntry) {
        IFR_CallStackInfo *ci=(IFR_CallStackInfo*)context->currentEntry;
        if(ci) {
            return ci->stream;
        } else {
            return 0;
        }
    } 
    return 0;
}

//----------------------------------------------------------------------
SQLDBC_IRuntime::TaskTraceContext * 
IFR_GetTraceContext(const IFR_Environment* environment)
{
    if(environment) {
        return ((IFR_Environment*)environment)->traceflags();
    } else {
        return 0;
    }
}

//----------------------------------------------------------------------
IFR_TraceStream *
IFR_GetTraceStream(const IFR_Environment* environment)
{
    if(environment) {
        return ((IFR_Environment*)environment)->getTraceStream();
    } else {
        return 0;
    }
}

//----------------------------------------------------------------------
SQLDBC_IRuntime::TaskTraceContext * 
IFR_GetTraceContext(const IFR_Connection *connection)
{
    return IFR_GetTraceContext((IFR_ConnectionItem *)connection);
}

//----------------------------------------------------------------------
IFR_TraceStream *
IFR_GetTraceStream(const IFR_Connection *connection)
{
    return IFR_GetTraceStream((IFR_ConnectionItem *) connection);
}

//----------------------------------------------------------------------
SQLDBC_IRuntime::TaskTraceContext * 
IFR_GetTraceContext(const IFR_ResultSet *resultset)
{
    return IFR_GetTraceContext((IFR_ConnectionItem *)resultset);
}

//----------------------------------------------------------------------
IFR_TraceStream *
IFR_GetTraceStream(const IFR_ResultSet *resultset)
{
    return IFR_GetTraceStream((IFR_ConnectionItem *) resultset);
}

//----------------------------------------------------------------------
SQLDBC_IRuntime::TaskTraceContext * 
IFR_GetTraceContext(const IFR_Statement *statement)
{
    return IFR_GetTraceContext((IFR_ConnectionItem *)statement);
}

//----------------------------------------------------------------------
IFR_TraceStream *
IFR_GetTraceStream(const IFR_Statement *statement)
{
    return IFR_GetTraceStream((IFR_ConnectionItem *)statement);
}

//----------------------------------------------------------------------
SQLDBC_IRuntime::TaskTraceContext * 
IFR_GetTraceContext(const IFR_RowSet *rowset)
{
    return IFR_GetTraceContext((IFR_ConnectionItem *)rowset->m_resultset);
}

//----------------------------------------------------------------------
IFR_TraceStream *
IFR_GetTraceStream(const IFR_RowSet *rowset)
{
    return IFR_GetTraceStream((IFR_ConnectionItem *) rowset->m_resultset);
}

//----------------------------------------------------------------------
SQLDBC_IRuntime::TaskTraceContext* 
IFR_GetTraceContext(const IFR_FetchInfo *fetchinfo)
{
    const IFRUtil_RuntimeItem *ri=fetchinfo;
    return IFR_GetTraceContext(ri);
}

IFR_TraceStream* IFR_GetTraceStream(const IFR_FetchInfo *fetchinfo)
{
    const IFRUtil_RuntimeItem *ri=fetchinfo;
    return IFR_GetTraceStream(ri);
}

IFR_END_NAMESPACE
