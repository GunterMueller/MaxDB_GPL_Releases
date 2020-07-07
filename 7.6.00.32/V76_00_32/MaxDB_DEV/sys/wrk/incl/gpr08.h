/*!================================================================
 module:    gpr08.h

 responsible: BurkhardD

 special area:   CPCDrv | Client | Prototypes fuer vpr03c

 description: Precompiler_Runtime_Routinen

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

#ifndef __GPR08_H__
#define __GPR08_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/
#ifdef __cplusplus
extern "C" {
#endif
void p08runtimeerror (sqlcatype*, sqlxatype*, int);
void p08vfwritetrace (sqlratype *sqlrap);
void p08analyzeconnectstatm (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgatype *sqlga, sqlgaentry *ga, sqlkaentry *ka, tsp00_Int4 buflen, char *buf);
void p08getconparameter (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga,
			 sqlkaentry *ka, int *param,
			 tsp00_KnlIdentifier VAR_ARRAY_REF userpassw);
void p08puttracename (sqlcatype *sqlca, sqlxatype *sqlxa, char *nam, tsp00_Int2 naml, BOOLEAN trwritei);
void p08puttraceknlid (sqlcatype *sqlca, sqlxatype *sqlxa, tsp00_KnlIdentifier nam, tsp00_Int2 naml, boolean trwritei);
void p08finishcmd (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, sqlkaentry *ka);
void p08_declare_search (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, sqlkaentry *ka, int pos);
void p08addspacevarpart(sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, int *pos, int *anf, int *plus);
int  p08macrosubstitution(sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, int *anf, int *mpos, int *macrono, tpr_macroline mline);
void p08privicmd(sqlcatype *sqlca, sqlxatype *sqlxa, sqlkaentry *ka, tsp00_Int2 *cmdfetch);
void p08oldpartfinish (tsp1_packet_ptr, tsp1_part_ptr*, tsp00_Int4);
void pr08cmakecommandinto (sqlcatype*, sqlxatype*, sqlgaentry*, int);

#ifdef __cplusplus
}
#endif

#endif
