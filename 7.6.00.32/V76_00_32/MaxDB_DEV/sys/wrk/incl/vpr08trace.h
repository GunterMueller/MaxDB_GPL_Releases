/*



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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





*/
#ifndef VPR08TRACE_H
#define VPR08TRACE_H

/*******************************************************************************

  module:       vpr08trace.h

  ------------------------------------------------------------------------------

  responsible:

  special area: PR layer (precompiler runtime)

  description:  trace routines

  see also:
  ------------------------------------------------------------------------------


*******************************************************************************/

/*------------------------------------------------------------------------------
    Include files:
  ------------------------------------------------------------------------------*/
#include "gsp00.h"
#include "gpr00.h"

#define FLUSHTRACE 1
#define DONTFLUSHTRACE 0

void pr08EnterFunction(sqlratype * pSqlra);
void pr08sint2trace (sqlratype * pSqlra, char *szStr, tsp00_Int2 sb2Number, char cWriteTrace);
void pr08sint4trace (sqlratype * pSqlra, char *szStr, tsp00_Int4 sb4Number, char cWriteTrace);
void pr08uint2trace (sqlratype * pSqlra, char *szStr, tsp00_Uint2 ub2Number, char cWriteTrace);
void pr08uint4trace (sqlratype * pSqlra, char *szStr, tsp00_Uint4 ub4Number, char cWriteTrace);
void pr08TraceString (sqlratype * pSqlra, char *text, char cWriteTrace);
void pr08TraceZeroTerminatedString (sqlratype * pSqlra, char *szComment, char *text, char cWriteTrace);
void pr08TraceNonTerminatedString (sqlratype * pSqlra, char *szComment, char *text, tsp00_Int4 len, char cWriteTrace);
void pr08longhextrace (sqlratype * pSqlra, char *szComment, tsp00_Int4 * buf, char cWriteTrace);
void pr08TraceBufferCompleteAsHex (sqlratype * pSqlra, char * buf, tsp00_Int4 len, char cWriteTrace);
void pr08TraceLineAsHex(sqlratype * pSqlra, char * buf, tsp00_Int4 len, char *PosPrefix, char cWriteTrace);

void pr08vfwrtrc (sqltatype * sqlta);
void pr08traceErr (sqlratype * pSqlra, char *szComment, struct SQLERROR *ret);

#endif


