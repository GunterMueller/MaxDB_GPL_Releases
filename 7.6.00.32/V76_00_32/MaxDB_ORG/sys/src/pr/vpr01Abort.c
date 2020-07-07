/*!================================================================
 module:    vpr01Abort.c

 responsible: Burkhard Diesing
 
 special area:   CPCDrv | Client | VTRACE | Diagnose | ABAP
 
 description: Aborting Sessions
 
 see:  
 
\if EMIT_LICENCE


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
 ===================================================================*/

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"
#include "gpr03.h"
#include "vpr01SQL.h"
#include "vpi10ConnSync.h"

/*==================================================================*
 *  DIRTY DECLARATIONS                                              *
 *==================================================================*/

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  EXPORTED FUNCTIONS                                                      *
 *==================================================================*/


/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

static void pr01AbortSessionTrace(sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae);

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

/*!
  Function: pr01AbortSession 

  description: Aborts the current or last session.
  
  arguments:
  sqlca      [in] Communication Area;
  sqlxa      [in]
  
*/

void pr01AbortSession (tpr01_SQLDesc *SQLDesc)
{
  tpr01_SQLContainer *SQL = SQLDesc->SQL;
  sqlcatype *sqlca = SQL->GetSqlca(SQLDesc);
  sqlxatype *sqlxa = SQL->GetSqlxa(SQLDesc);
  struct SQLERROR *sqlemp = sqlca->sqlemp;
  sqlgaentry *gae   = SQLDesc->ConDesc->ga;
  pr01AbortSessionTrace(sqlca, sqlxa, gae);
  p03sqlrelease (sqlca->sqlrap, sqlca->sqlgap, gae, sqlemp);
  sqlxa->xacmdinit = cpr_is_false;
  sqlca->sqlrap->raactsession = CPR_SE_EMPTY;
}

static void pr01AbortSessionTrace(sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae)
{
  sqlratype *sqlrap = sqlca->sqlrap;
  sqltatype *tap = sqlrap->rasqltap;
  if (tap->tatrout != CPR_TRACE_LONG && tap->tatrout != CPR_TRACE_MODLONG)
    return;
  {
    tap->tastr80[0] = ' '; tap->tastr80l=1; p08vfwritetrace(sqlrap);
    sprintf(tap->tastr80, "ABORT SESSION  : %d", sqlca->sqlrap->raactsession);
    tap->tastr80l=strlen(tap->tastr80);
    p08vfwritetrace (sqlrap);
    p01xtimetrace (sqlca, sqlxa, gae);
  }
}


