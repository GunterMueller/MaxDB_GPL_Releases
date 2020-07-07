/*!================================================================
 module:    vpr01Trace.h

 responsible: BurkhardD, MarcoP

 special area:   CPCDrv | Client | Trace SQL Commands
 description: Output to the Tracefile

 see:




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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




 ===================================================================*/

#ifndef __VPR01_TRACE_H__
#define __VPR01_TRACE_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"
/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/
#ifdef __cplusplus


/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/
#else
/* output to the tracefile */
externC boolean pr01TraceIsTrace(sqlratype *sqlrap);
externC void pr01TracePrintf(sqlratype * sqlrap, const char* szFmt, ...);
externC void pr01WriteToTrace (sqltatype *sqltap, char *pString, tsp00_Uint4 cBLenString, struct SQLERROR *sqlemp);
externC void pr01TraceCmd(sqlratype *sqlrap, tpr05_String *pString);
externC void pr01TraceRuntimeError (sqlcatype *sqlca, sqlxatype *sqlxa,  tpr_runtime_errors_epr00 error);
/* dynamic trace */
externC void pr01traceParameterRefresh(sqlcatype *my_sqlca, sqlxatype *my_sqlxa);
externC void pr01TraceSession(sqlgaentry *gae, sqlratype *sqlra);
#endif /*__cplusplus */


#endif /*__VPR01_TRACE_H__*/
