/*!
  @file           IFR_TraceContext.h
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
#ifndef IFR_TRACECONTEXT_H
#define IFR_TRACECONTEXT_H

#ifndef INCLUDED_FROM_IFR_COMMON
#  include "Interfaces/Runtime/IFR_Trace.h"
#endif

IFR_BEGIN_NAMESPACE

class IFR_ConnectionItem;
class IFRUtil_RuntimeItem;
class IFR_Environment;
class IFR_Connection;
class IFR_ResultSet;
class IFR_Statement;
class IFR_RowSet;
class IFR_ResultSetMetaData;
class IFR_ParameterMetaData;
class IFR_FetchInfo;
class SQLDBC_IRuntime;

SQLDBC_IRuntime::TaskTraceContext *IFR_GetTraceContext(IFR_ConnectionItem* connectionitem);
IFR_TraceStream* IFR_GetTraceStream(IFR_ConnectionItem *connectionitem);

SQLDBC_IRuntime::TaskTraceContext* IFR_GetTraceContext(const IFRUtil_RuntimeItem* runtimeitem);
IFR_TraceStream* IFR_GetTraceStream(const IFRUtil_RuntimeItem* runtimeitem);

SQLDBC_IRuntime::TaskTraceContext *IFR_GetTraceContext(SQLDBC_IRuntime* runtime);
IFR_TraceStream* IFR_GetTraceStream(SQLDBC_IRuntime* runtime);

SQLDBC_IRuntime::TaskTraceContext* IFR_GetTraceContext(const IFR_Environment *environment);
IFR_TraceStream* IFR_GetTraceStream(const IFR_Environment* environment);

SQLDBC_IRuntime::TaskTraceContext* IFR_GetTraceContext(const IFR_Connection *connection);
IFR_TraceStream* IFR_GetTraceStream(const IFR_Connection *connection);

SQLDBC_IRuntime::TaskTraceContext* IFR_GetTraceContext(const IFR_ResultSet *resultset);
IFR_TraceStream* IFR_GetTraceStream(const IFR_ResultSet *resultset);

SQLDBC_IRuntime::TaskTraceContext* IFR_GetTraceContext(const IFR_Statement *statement);
IFR_TraceStream* IFR_GetTraceStream(const IFR_Statement* statement);

SQLDBC_IRuntime::TaskTraceContext* IFR_GetTraceContext(const IFR_RowSet *rowset);
IFR_TraceStream* IFR_GetTraceStream(const IFR_RowSet *rowset);

SQLDBC_IRuntime::TaskTraceContext* IFR_GetTraceContext(const IFR_FetchInfo *fetchinfo);
IFR_TraceStream* IFR_GetTraceStream(const IFR_FetchInfo *fetchinfo);


inline SQLDBC_IRuntime::TaskTraceContext* IFR_GetTraceContext(const IFR_ResultSetMetaData *metadata)
{
    return 0;
}

inline IFR_TraceStream* IFR_GetTraceStream(const IFR_ResultSetMetaData *metadata) 
{
    return 0;
}

inline SQLDBC_IRuntime::TaskTraceContext* IFR_GetTraceContext(const IFR_ParameterMetaData *metadata)
{
    return 0;
}

inline IFR_TraceStream* IFR_GetTraceStream(const IFR_ParameterMetaData *metadata) 
{
    return 0;
}


#if 0
template <class T>
inline SQLDBC_IRuntime::TaskTraceContext* IFR_GetTraceContext(const T *t)
{
    return 0;
}

template <class T>
inline IFR_TraceStream* IFR_GetTraceStream(const T *t)
{
    return 0;
}
#endif

IFR_END_NAMESPACE

#endif
