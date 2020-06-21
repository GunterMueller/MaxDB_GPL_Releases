/*



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





*/
#ifndef __VPR10C_H__
#define __VPR10C_H__
#include "gpr00.h"
#include "vpr01Con.h"
#include "vpr05IfCom_String.h"
#include "vpa40DBC.h"

tpr01_ConDesc *pr10Connect (tpa40DBC *api_dbc_ptr, tpr05_String *ConStr,
			   sqldbnode sqcdbnode, sqldbname sqcdbname,
                            tpr05_String *UID, tpr05_String *PWD);
void pr10Disconnect (tpa40DBC *api_dbc_ptr);

void pr10DropParseID (tpa40DBC *api_dbc_ptr, const sqlparsid sqcparsid);

void p10connect (sqlcatype*, sqlgaentry *gae, const short*, sqldbnode, sqldbname,
	const sqlint4*, const char*, sqluser, const sqlpassword);
void p10statement (sqlcatype *sqlca, sqlgaentry *gae, const sqlint2 *sqcdbno,
	const sqlint4 *sqcstmlen, const char *sqcstmbuf, tpr05_StringEncoding encodingType);
void p10parse (sqlcatype *sqlca, sqlgaentry *gae, const sqlint2 *sqcdbno,
	const sqlint4 *sqcstmlen, const char *sqcstmbuf, tpr05_StringEncoding encodingType, sqldatype *sqlda,
	sqlparsid sqcparsid);
void p10execute (sqlcatype *sqlca, sqlgaentry *gae, const sqlint2 *sqcdbno,
	const sqlint4 *sqcstmlen, const char *sqcstmbuf, tpr05_StringEncoding encodingType, sqldatype *sqlda,
	const sqlparsid sqcparsid, tsp00_Int4 sqcrowcount);
void p10forexecute (sqlcatype *sqlca, sqlgaentry *gae, const sqlint2 *sqcdbno,
	const sqlint4 *sqcstmlen, const char *sqcstmbuf, tpr05_StringEncoding encodingType, sqldatype *sqlda,
	const sqlparsid sqcparsid, const sqlint2 *count, const sqlint2 *offs);
void p10command (sqlcatype *sqlca, sqlgaentry *gae, const sqlint2 *sqcmode,
	const sqlint4 *sqcstmlen, const char *sqcstmbuf, tpr05_StringEncoding encodingType, sqlint2 *sqcresult);
void p10setmacro (sqlcatype *sqlca, const sqlint2 *macrono,
	const char *macroval);
void p10dropparsid (sqlcatype *sqlca, sqlgaentry *gae, const sqlint2 *sqcdbno,
	const sqlparsid sqcparsid);
void p10release (sqlcatype *sqlca, sqlgaentry *gae, const sqlint2 *sqcdbno);
void p10finish (sqlcatype *sqlca);
void p10getinfo (sqlcatype *sqlca, const sqlint2 *sqcvarno,
	void *sqcvarinfo, const void *sqlda, sqlint2 *sqcerr);
void p10putlongdata (sqlcatype* sqlca, sqlgaentry *gae, const short* dbno, const short* varno, sqldatype* sqlda, const sqlparsid);     
void p10getlongdata (sqlcatype* sqlca, sqlgaentry *gae,const short* dbno, const short* varno, sqldatype* sqlda, const sqlparsid);     
void p10putinfo (sqlcatype *sqlca, const sqlint2 *sqcvarno,
	const void *sqcvarinfo, void *sqlda, sqlint2 *sqcerr);
void p10int2bind (sqlcatype *sqlca, const sqlint2 *sqcvarno,
	const sqlint2 *varint2, void *sqlda, sqlint2 *sqcerr);
void p10int4bind (sqlcatype *sqlca, const sqlint2 *sqcvarno,
	const sqlint4 *varint4, void *sqlda, sqlint2 *sqcerr);
void p10uns2bind (sqlcatype *sqlca, const sqlint2 *sqcvarno,
	const sqluns2 *varuns2, void *sqlda, sqlint2 *sqcerr);
void p10uns4bind (sqlcatype *sqlca, const sqlint2 *sqcvarno,
	const sqluns4 *varuns4, void *sqlda, sqlint2 *sqcerr);
void p10r4bind (sqlcatype *sqlca, const sqlint2 *sqcvarno,
	const sqlr4 *varreal4, void *sqlda, sqlint2 *sqcerr);
void p10r8bind (sqlcatype *sqlca, const sqlint2 *sqcvarno,
	const sqlr8 *varreal8, void *sqlda, sqlint2 *sqcerr);
void p10ccharbind (sqlcatype *sqlca, const sqlint2 *sqcvarno,
	const sqlint4 *sqcvarlen, const char *varchar, void *sqlda,
	sqlint2 *sqcerr);
void p10zonedbind (sqlcatype *sqlca, const sqlint2 *sqcvarno,
	const sqlint2 *sqcvarlen, const sqlint2 *sqcvarfrac,
	const char *varzoned, void *sqlda, sqlint2 *sqcerr);
void p10decbind (sqlcatype *sqlca, const sqlint2 *sqcvarno,
	const sqlint2 *sqcvarlen, const sqlint2 *sqcvarfrac,
	const char *vardec, void *sqlda, sqlint2 *sqcerr);
#endif
