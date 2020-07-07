/*!================================================================
  module:    gpr03.h

  responsible:  BurkhardD

  special area:   Header for Oldstyle PASCAL and C Modules (vpr03, vpr03c)

  description:

  see:

 -------------------------------------------------------------------





    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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

#ifndef __GPR03_H__
#define __GPR03_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/
#include "gpr00.h"
#include "gpr03m.h"
#include "gsp05.h"
#include "vpr01Con.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
void p03cmdtrace (sqlratype * sqlra, sqlgaentry *gaen, tsp00_Int2 nlcnt, tsp00_Int2 comkind,
	     tpr05_String *pSQLStatement);
void p03connectcmdtrace (sqlratype * sqlra, sqlgaentry * gaen);
void p03ccmdinit  (struct tpr01_SQLDesc *SQLDesc, sqlcatype * sqlca, sqlgaentry * gaentry, char  m_type);
tsp00_Int4 p03cGetPartBuffer(sqlratype * sqlra, tsp1_part_kind_Param PartKind, tsp00_Uint1 **pBuf);
void p03gaentryinitdefault(sqlgaentry * gaentry);
void p03sqlgaentryinit (sqlgaentry * gaentry, tsp00_Int2 xuindex, struct SQLERROR * sqlemp);
void p03returncodeget (sqlcatype *sqlca, sqlxatype *sqlxa);
void p03cseterror (struct SQLERROR * sqlemp,  tpr_runtime_errors_Enum error);
void p03sendconnect (struct tpr01_SQLDesc *SQLDesc, sqlratype *sqlrap, sqlgatype *sqlga, sqlgaentry *gaen, struct SQLERROR * sqlemp);
void p03initsqlem (struct SQLERROR *sqlemp);
void p03setformat (struct tpr01_SQLDesc *SQLDesc, sqlratype * sqlrap, sqlgaentry * gaen, tsp00_Int2 datetime, struct SQLERROR * sqlemp);
void p03NologSession(sqlgatype * sqlga, tsp00_Int2 ganolog);
tsp1_packet *p03cpacketinit  (struct tpr01_SQLDesc *SQLDesc, sqlratype * sqlra, sqlgaentry * gaentry, char  m_type);
void p03find_part(sqlrapointer sqlrap, tsp1_part_kind_Param part_kind, tsp1_part_ptr *part_ptr);
void p03cputpart(sqlratype * sqlrap, sqlgaentry * gaent, char part_kind, void *ptr, tsp00_Int4 len, struct SQLERROR *sqlemp);
void p03cpparsid(sqlratype * sqlrap, sqlgaentry * gaent, tpr00_ParseId VAR_ARRAY_REF parsid, struct SQLERROR *sqlemp);
void p03csqlcaareainit(sqlcatype * sqlca);
void p03dropparsid(sqlcatype * sqlca, sqlxatype *sqlxa, sqlgaentry *ga, tpr00_ParseId VAR_ARRAY_REF parsid);
void p03putsysprofbuf(sqlratype * sqlra);
void p03modulnameput  (sqlcatype * sqlca, sqlxatype *sqlxa, sqlgaentry *ga, sqlkaentry *ka);
void pr03AbapInfoPutPart(sqlcatype *sqlca, sqlratype *sqlra, sqlgaentry * gaen);
void p03reqrecpacket(sqlcatype *sqlca, sqlcatype *sqlxa, sqlgaentry *ga);
void p03freeld(	struct SQLLD   **ld);
void p03tvfopentrace (sqlratype * sqlra, tsp00_VFileOpCodes read_write, struct SQLERROR *sqlemp);
void p03tvfwritetrace (sqlratype * sqlra, struct SQLERROR *sqlemp);
void p03tvfclosetrace (sqlratype * sqlra, struct SQLERROR *sqlemp);
void p03varparttrace (sqlratype * sqlra, sqlgaentry *gaen);
void pr03cSessionInfoGet(sqlratype * sqlrap, sqlgaentry * gaen);
void p03sqlaconnect (sqlratype * sqlrap, sqlgatype * sqlga, sqlgaentry *gaen, struct SQLERROR *sqlemp);
void p03putconnect (struct tpr01_SQLDesc *SQLDesc, sqlratype * sqlrap, sqlgaentry * gaent, struct SQLERROR *sqlemp);
void p03creqrecpacket (sqlratype * sqlra, sqlgaentry * gaentry, struct SQLERROR *sqlemp);
void p03sqlrelease (sqlratype * sqlrap, sqlgatype * sqlga, sqlgaentry *gaen, struct SQLERROR *sqlemp);
void p03conninfoget (sqlgatype * sqlga, sqlgaentry *gaen, tsp00_Int2 sess);
void p03connect (sqlxatype *sqlxa, sqlratype * sqlrap, sqlgatype * sqlga, sqlgaentry *gaen, tsp00_Int2 datetime, struct SQLERROR *sqlemp);
void p03clzuerror (sqlgaentry * gaentry, int lzu_prog, struct SQLERROR * sqlemp);
void p03messcodeget (sqlgaentry * gaentry);
void p03setsegmode (sqlratype * sqlra, sqlgaentry * gaentry);
void p03crequest (sqlratype * sqlra, sqlgaentry * gaentry,
    struct SQLERROR *sqlemp);
void p03creceive (sqlratype * sqlra, sqlgaentry * gaentry,
    struct SQLERROR *sqlemp);
void p03csqlclock (sqlratype * sqlra, long cmd);
void p03partptrinit (sqlratype * sqlra);
void p03packeterror (sqlratype * sqlra, sqlgaentry * gaentry,
    struct SQLERROR *sqlemp);
void p03cresultcount(sqlratype *sqlra, tsp00_Int4 * resultcount);
void p03warningset (struct SQLERROR *sqlemp, tsp00_C2 warnset);
void p03DebugBreak();
void p03getpid   (tsp00_Int4 *pid);
void p03gparsid (sqlratype * sqlra, struct SQLERROR *sqlemp, tpr00_ParseId VAR_ARRAY_REF parsid, tpr00_SessionID *sessionid);
void p03gselparsid (sqlratype * sqlra, struct SQLERROR *sqlemp, tpr00_ParseId VAR_ARRAY_REF parsid, tpr00_SessionID *sessionid);
void p03packeterror (sqlratype * sqlra, sqlgaentry * gaentry, struct SQLERROR *sqlemp);
void *p03dynalloc (sqlmdesc * md);
void pr03cOrInit(sqlorentry *ore);
void pr03cCuInit(sqlcuentry *cue);
void p03ctraceerror (sqlratype * sqlrap, struct SQLERROR *sqlemp);
void p03traceerror (sqlratype * sqlrap, struct SQLERROR *sqlemp);
void p03csqlemptosqlca (sqlcatype * sqlcap, struct SQLERROR *sqlemp);
void p03EnvDelete( sqlcatype *sqlcar );
void p03EnvClear( sqlcatype *sqlcar );
void *p03caci (sqlcatype *sqlcar, sqlxatype *sqlxar,
               tpr01_Precompiledfor pcfor);
void p03cancel (sqlcatype* sqlca, sqlgaentry * gaentry);
void p03orfree (sqlcatype *sqlca, struct SQLCOL *orptr, tsp00_Int2 kind);
void p03csqlinit (sqlratype * sqlrap, char comp, tsp00_Int2 language);
void p03datafopen (sqlratype * sqlra, tsp00_VFilename VAR_ARRAY_REF filename, tsp00_Int4 * fileno, tsp05_RteFileMode read_write, struct SQLERROR *sqlemp);
void p03cpresname (sqlratype * sqlrap, sqlgaentry * gaent,
		   tsp00_KnlIdentifier sqlresn,
		   struct SQLERROR *sqlemp);
void p03encrypt (tsp00_Name VAR_ARRAY_REF clearname, tsp00_CryptPw VAR_ARRAY_REF crypt);
tsp1_sqlmode pr03cMapSqlMode(tsp00_Int2 rakamode, tsp00_Int2 rasqlansi);
void p03getparameteraddr (sqlcatype *sqlca, sqlxatype *sqlxa, int *paix, int *cmcnt, void **vaaddr, int *vaind, void **indaddr, int *indind);
void
pr03cPutResultName(struct tpr01_ConDesc *ConDesc, tsp00_KnlIdentifier VAR_ARRAY_REF sqlresn, struct SQLERROR *sqlemp);
  tpr05_StringEncoding pr03cGetPacketEncoding();
#ifdef __cplusplus
}
#endif

#endif
