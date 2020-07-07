/****************************************************************************

  module      : vpr04orc.c

  -------------------------------------------------------------------------

  responsible : BurkhardD MarcoP ThomasS

  special area:
  description : Conversion of Oracle data types from/to SAPDB-types

Konvertierung Oracletype in SAPDB-type und umgekehrt:

       SAPDB       |          external Oracletype   max           |
  Type       | Code| sapdb Name       | oracle Name |length | Code|
             |     |                  |             | Bytes |     |
-------------|-----|------------------|-------------|-------|-----|
 CPR_VCHAR   |  6  | SQL_ORA_VARCHAR2 | VARCHAR2    |  2000 |  1  |
  -.-        |  -  | SQL_ORA_NUMBER   | NUMBER      |    22 |  2  |
  -.-        |  -  |                  | INTEGER     |     1 |  3  |
 CPR_VINT2   |  0  | SQL_ORA_FIXED-   | INTEGER     |     2 |  3  |
 CPR_VINT4   |  1  | POINTINTEGER     | INTEGER     |     4 |  3  |
 CPR_VINT8   | 33  |                  |             |     8 |  3  |
 CPR_VREAL4  |  2  | ORAFLOAT-        | FLOAT       |     4 |  4  |
 CPR_VREAL8  |  3  |   POINT          | FLOAT       |     8 |  4  |
 CPR_VCHARC  |  7  | ORANULLTER-      |NULL-TERMINAT|   n+1 |  5  |
	     |     |MINATEDSTRING     | STRING      |       |     |
 CPR_VSTRING1| 20  | SQL_ORA_VARNUM   | VARNUM      |    22 |  6  |
 CPR_VDECIMAL|  4  | ORAPACKED-       | PACKED      |   n   |  7  |
             |     |  DECIMAL         |  DECIMAL    |       |     |
 CPR_VCHAR   |  6  | SQL_ORA_LONG     | LONG        |2**31-1|  8  |
 CPR_VSTRING | 15  | SQL_ORA_VARCHAR  | VARCHAR     |2(l)+n |  9  |
  -.-        | -.- | SQL_ORA_ROWID    | ROWID       |       | 11  |
 CPR_VCHAR   |  6  | ORADATE          | DATE        |     7 | 12  |
 CPR_VSTRING | 15  | SQL_ORA_VARRAW   | VARRAW      |2(l)+n | 15  |
 CPR_VCHAR   |  6  | SQL_ORA_RAW      | RAW         |   255 | 23  |
 VCAHR       |  6  | SQL_ORA_LONGRAW  | LONG RAW    |2**31-1| 24  |
 CPR_VUNS2   | 16  | SQL_ORA_UNSIGNED | UNSIGNED INT|     2 | 68  |
 CPR_VUNS4   | 17  | SQL_ORA_UNSIGNED | UNSIGNED INT|     7 | 68  |
 CPR_VCHAR   |  6  | SQL_ORA_DISPLAY  | DISPLAY     |   n   | 91  |
 VSTRIN4     | 35  |ORALONGVARCHA     | LONG VARCHAR|2**31-5| 94  |
             |     |                  |             |4(l)+n |     |
 VSTRIN4     | 35  |ORALONGVARRAW     | LONG VARRAW |2**31-5| 95  |
             |     |                  |             |4(l)+n |     |
 CPR_VCHAR   |  6  | SQL_ORA_CHAR     | CHAR        |   255 | 96  |
 CPR_VCHARC  |  7  | SQL_ORA_CHARZ    | CHARZ       |   255 | 96  |
 CPR_VCHAR   |  6  | SQL_ORA_MLSLABEL | MLSLABEL    |   255 | 97  |
  -.-        | -.- | ORAINTERNJUL     | INTERN JULIAN       | 14  |
             |     |                  |             |       |     |

relase 6.1.2    24.10.95
convertierung  der sapdb datentypen in oracledatentypen
   siehe p04sftoora
   siehe program   ../bft/orapr1.cpc

Connected to ORACLE as user: DEMO
prepare ***  ***
	exec sql create table all_datatyp (
		c1 varchar2 (10),
		c2 number (6),
		c3 integer,
		c4 float,
		c5 decimal (6,2),
		c6 long,
		c7 varchar,
		c8 date,
		c9 raw );
colno = 0,  sapdb-coltype = 31, sapdb-collen = 10
               ora-coltype = 96,    ora-collen = 10

colno = 1,  sapdb-coltype = 0, sapdb-collen = 6
               ora-coltype = 2,    ora-collen = 1536

colno = 2,  sapdb-coltype = 0, sapdb-collen = 18
               ora-coltype = 2,    ora-collen = 4608

colno = 3,  sapdb-coltype = 1, sapdb-collen = 18
               ora-coltype = 4,    ora-collen = 0

colno = 4,  sapdb-coltype = 0, sapdb-collen = 6
               ora-coltype = 2,    ora-collen = 1538

colno = 5,  sapdb-coltype = 6, sapdb-collen = 40
               ora-coltype = 94,    ora-collen = 40

colno = 6,  sapdb-coltype = 31, sapdb-collen = 1
               ora-coltype = 96,    ora-collen = 1

colno = 7,  sapdb-coltype = 13, sapdb-collen = 26
               ora-coltype = 12,    ora-collen = 26

colno = 8,  sapdb-coltype = 4, sapdb-collen = 1
               ora-coltype = 23,    ora-collen = 1

execute ***  ***
prepare ***  ***
	exec sql create table rest_datatyp (
		c1 long raw,
		c2 char (60));
colno = 0,  sapdb-coltype = 8, sapdb-collen = 40
               ora-coltype = 24,    ora-collen = 40

colno = 1,  sapdb-coltype = 2, sapdb-collen = 60
               ora-coltype = 96,    ora-collen = 60

execute ***  ***
prepare ***  ***
	exec sql create table long_datatyp (
		c1 long varchar,
		c2 char (500) );
colno = 0,  sapdb-coltype = 6, sapdb-collen = 40
               ora-coltype = 94,    ora-collen = 40

colno = 1,  sapdb-coltype = 2, sapdb-collen = 500
               ora-coltype = 1,    ora-collen = 500

execute ***  ***
prepare ***  ***
	exec sql create table long2_datatyp (
		c1 varchar2 ,
		c2 varchar2 (15) ,
		c3 varchar2 (300),
		c4 varchar2 (1500),
		c5 varchar ,
		c6 varchar (15) ,
		c7 varchar (300),
		c8 varchar (1500));
colno = 0,  sapdb-coltype = 31, sapdb-collen = 1
               ora-coltype = 96,    ora-collen = 1

colno = 1,  sapdb-coltype = 31, sapdb-collen = 15
               ora-coltype = 96,    ora-collen = 15

colno = 2,  sapdb-coltype = 31, sapdb-collen = 300
               ora-coltype = 1,    ora-collen = 300

colno = 3,  sapdb-coltype = 31, sapdb-collen = 1500
               ora-coltype = 1,    ora-collen = 1500

colno = 4,  sapdb-coltype = 31, sapdb-collen = 1
               ora-coltype = 96,    ora-collen = 1

colno = 5,  sapdb-coltype = 31, sapdb-collen = 15
               ora-coltype = 96,    ora-collen = 15

colno = 6,  sapdb-coltype = 31, sapdb-collen = 300
               ora-coltype = 1,    ora-collen = 300

colno = 7,  sapdb-coltype = 31, sapdb-collen = 1500
               ora-coltype = 1,    ora-collen = 1500

execute ***  ***
Work commit  release

  created : 1993-09-21
  last changed: 2001-06-22  17:17
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG


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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//


*****************************************************************************/



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/
#define sql_sqldb
#define sql_oracle
#include "gpr00.h"
#include "hsp78_0.h"
#include "gpr04.h"
#include "gpr03.h"
#include "gpr04.h"
#include "livecachetypes.h" /* no check */
#include "vpr01SQL.h"
#include "gpr03.h"
#include "gpr08.h"
#include "vpr04Long.h"
#include "gpr01.h"
#include "vpr01Module.h"
#include "vpr07Macro.h"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#ifdef _IBMR2
/*extern char *malloc (); */
#else
#ifndef FREEBSD
#include <malloc.h>
#endif
#endif

/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS (PROTOTYPES)                                     *
 *===========================================================================*/


void    s41p4int (char *, int, int, char *);
void 	s26new_part_init (tsp1_packet_ptr, tsp1_segment_ptr,
	tsp1_part_ptr*);
void 	s26finish_part (tsp1_packet_ptr, tsp1_part_ptr);

void            sqlnul (short *, short *, int *);
void            sqlprc (const long *, int *, int *);
char pr04oPutResultCount(tsp00_Uint1 *buf, int cbRows);


/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

/*!
  Function:     p04OraTypes2Internal

  see also:

  Description:  mapping oracle datatypes to internal datatypes

  Arguments:
    T  [in] oracle data type
    L  [in]   length of T in bytes
    vt [out] internal datatype
    PreFor [in] flag that shows if the application was precompiled for unicode

  return value: void
 */
void
p04OraTypes2Internal (int T, tsp00_Int4 L, tsp00_Int2 * vt, tpr01_Precompiledfor PreFor)
{
    switch (T)
        {
        case SQL_ORA_VARCHAR2:
        case SQL_ORA_LONG:
        case SQL_ORA_DISPLAY:
        case SQL_ORA_CHAR:
        case SQL_ORA_MLSLABEL:
            *vt = CPR_VCHAR;
            break;
        case SQL_ORA_RAW:
        case SQL_ORA_LONGRAW:
            *vt = CPR_VRAW;
            break;
        case SQL_ORA_NUMBER:
            *vt = CPR_VONUMBER;
            break;
        case SQL_ORA_VARNUM:
            *vt = CPR_VOVARNUM ;
            break;
        case SQL_ORA_INTERNDATE:
            *vt = CPR_VODATE ;
            break;
        case SQL_ORA_VARCHAR:
        case SQL_ORA_VARRAW:
            *vt = CPR_VSTRING;
            break;
        case SQL_ORA_NULLTERMINATEDSTRING:
            *vt = CPR_VCHARZ; /* PTS 3177, 11.12.96, Horst */
            break;
        case SQL_ORA_CHARZ:
            *vt = CPR_VCHARC;
            break;
        case SQL_ORA_FIXEDPOINTINTEGER: {
          *vt = (L > 4) ? (short) CPR_VINT8 :
            ((L > 2) ? (short) CPR_VINT4 :
             ((L > 1 ) ? (short) CPR_VINT2 : (short) CPR_VINT1));
          break;
	}
        case SQL_ORA_UNSIGNED: {
            *vt = (L > 2) ? (short) CPR_VUNS4 :
	      ((L > 1 ) ? (short) CPR_VUNS2 : (short) CPR_VUNS1);
            break;
	}
        case SQL_ORA_FLOATPOINT:
            *vt = (L > 4) ? (short) CPR_VREAL8 : (short) CPR_VREAL4;
            break;
        case SQL_ORA_PACKEDDECIMAL:
            *vt = CPR_VDECIMAL;
            break;
        case SQL_ORA_LONGVARCHAR:
        case SQL_ORA_LONGVARRAW:
            *vt = CPR_VSTRING4;
            break;
        case SQL_ORA_VFILE:
            *vt = CPR_VFILE;
            break;
        case SQL_ORA_VFILEC:
            *vt = CPR_VFILEC;
            break;
	case SQL_ORA_UCS2:
	case SQL_ORA_UTF16:
            *vt = CPR_VUNICODEC;
            break;
	case SQL_ORA_LONG_UNICODE:
        case SQL_ORA_VARUCS2_2:
            *vt = CPR_VUNICODE;
            break;
        case SQL_ORA_ABAP_HANDLE:
            *vt = CPR_VABAPHANDLE;
            break;
        case SQL_ORA_VARCHAR_WITHOUT_LEN:
            *vt = CPR_VVARCHAR_WITHOUT_LEN;
            break;
        case SQL_ORA_LONG_DESC:
            *vt = CPR_VLONGDESC;
            break;
        case SQL_ORA_TVARCHAR:
            if( PreFor == PrecompiledForUnicode_epr01 ) {
              *vt = CPR_VSTRINGUNICODE;
            }
            else {
              *vt = CPR_VSTRING;
            }
            break;
        case SQL_ORA_TVARCHAR4:
            if( PreFor == PrecompiledForUnicode_epr01 ) {
              *vt = CPR_VSTRINGUNICODE4;
            }
            else {
              *vt = CPR_VSTRING4;
            }
            break;
        case SQL_ORA_TCHAR:
            if( PreFor == PrecompiledForUnicode_epr01 ) {
              *vt = CPR_VUNICODEC;
            }
            else {
              *vt = CPR_VCHARC;
            }
            break;
        default:
            *vt = -1;
        }
}     /*--------------------------------------------*/

void
p04orat (long L, short T, const char *S, short C, char *name,
    tsp00_Int2 * vt, tsp00_Int4 * vl, tsp00_Int2 * vf, char *err, sqlxatype *sqlxa)
{
    static char    *vapar = "PARAMETER                       ";
    int            null;
    int prec, scal;

    if (S)
        {
        int             ls = (C < sizeof (sqllname)) ? C : sizeof (sqllname);

        memcpy (name, S, ls);
        if (ls < sizeof (sqllname))
            memset (&name[ls], ' ', sizeof (sqllname) - ls);
        }
    else
        memcpy (name, vapar, sizeof (sqllname));
    *vl = L;
    *vf = 0;
    sqlnul (&T, &T, &null);
    if (sqlxa->xaModDesc)
      p04OraTypes2Internal (T, L, vt, sqlxa->xaModDesc->PrecompiledforUnicode);
    else
      p04OraTypes2Internal (T, L, vt, PrecompiledForAscii_epr01);
    switch (*vt)
	{
	case CPR_VREAL4:
	case CPR_VREAL8:
		*vf = -1;
		break;
	case CPR_VDECIMAL:
		sqlprc (&L, &prec, &scal);
		*vl = prec;
		*vf = (short) scal;
		break;
	case CPR_VUCS2:
        case CPR_VUTF16:
	case CPR_VUNICODEC:
          /* PTS 1108496 */
	  /*	*vl *= 2;*/
		break;
	case -1:
		if (*err == cpr_p_ok)
			*err = cpr_unknown_datatype;
		break;
	}
}     /*--------------------------------------------*/

int cprFetchop2 = 1;
void
p04oradin (sqlcatype * sqlca, sqlxatype * sqlxa, sqlgaentry *gae, sqlcuentry *cu, void *sqlda, struct tpr_sqlloop *lp)
{
  tsp1_packet *pck = (tsp1_packet*) gae->gareqptr;
  tsp1_segment *seg = (tsp1_segment *) sqlca->sqlrap->rasegptr;
  struct SQLDA   *da = (struct SQLDA *) sqlda;
  struct SQLLD   *ldp = sqlca->sqlrap->rasqlldp;
  tsp00_Int2       *ldmaxi = &ldp->ldmaxi;
  tsp00_Int2       *ldcolkind = &ldp->ldcolkind;
  char           *allsend = &ldp->ldallsend;
  tsp00_Int4        lppos = 0;
  tsp00_Int4        lpcnt = lp->lpcnt;
  tsp00_Int4        lpind = lp->lpindi;
  tsp00_Int4        rowno = 0;
  char            trfirst = 0;
  tsp00_Lname       blname;
  sqllname        name;
  tsp00_Int2        vt, vf;
  tsp00_Int4        vl;
  tpr00_ParseId   parsid;
  tsp00_Int4        pnofirst;
  int             i;
  char            err;
  tsp1_part *pidpart;
  tsp00_Uint1 *pid;
  tsp1_part *datapart;
  tsp00_Uint1 *data;
  tsp1_part *cntpart;
  tsp00_Uint1 *cnt;
  sqlkapointer    ka;
  int cbResCount;

  M90TRACE (M90_TR_ENTRY, "p04oradin   ", 0);
  memcpy (blname, "                                ", sizeof (tsp00_Lname));
  p03find_part (sqlca->sqlrap, sp1pk_parsid, &pidpart);
  pid = &pidpart->sp1p_buf[CPR_PARS_IDIX];
  s26new_part_init (pck, seg, &cntpart);
  cntpart->sp1p_part_header.sp1p_part_kind = sp1pk_resultcount;
  cntpart->sp1p_part_header.sp1p_buf_len = MXPR_NUMB + 1;
  sqlca->sqlrap->rasegmpartptr [sp1pk_resultcount] = cntpart;
  cnt = cntpart->sp1p_buf;
  *cnt = CPR_UNDEF_BYTE;
  s26finish_part (pck, cntpart);
  s26new_part_init (pck, seg, &datapart);
  sqlca->sqlrap->rasegmpartptr [sp1pk_data] = datapart;
  data = datapart->sp1p_buf;
  if (sqlca->sqlrap->ralang != CPR_LA_CALL) {
    ka = sqlxa->xaSQLDesc->ka;

    da->sqloffset = 0;
    da->sqlloop = (ka->kapaindex == 0) ? 0 : (ka->kapaindex < 0)
      ? 2 : (p08runtimeerror (sqlca, sqlxa, cpr_not_implemented), 0);
  }
  p04mode (sqlca, gae);
  if (lp->lpfirsti == cpr_is_true) {
    datapart->sp1p_part_header.sp1p_attributes[0] |= 1 << sp1pa_first_packet;
    lp->lpfirsti = cpr_is_false;
    if (da->sqlloop != 0) {
      if (sqlca->sqlrap->ralang != CPR_LA_CALL) {
	da->sqloffset = lp->lpindi;
	da->sqlloop = pr04GetLoopCnt(sqlca, sqlxa, lp);
      }
      else
	lp->lpindi = da->sqloffset;
    }
    lpcnt = (short) da->sqlloop + da->sqloffset;
    lpind = da->sqloffset;
    if (da->sqlloop != 0) {
      if (lpcnt < 1) {
	p08runtimeerror (sqlca, sqlxa, cpr_r_loop_init_wrong);
	lp->lpnext = cpr_is_false;
      }
      else {
	trfirst = 1;
	lp->lpcnt = lpcnt;
	pr04oPutResultCount(cnt, 0);
      }
    }
  }
  else {
    datapart->sp1p_part_header.sp1p_attributes[0] |= 1 << sp1pa_next_packet;
    trfirst = 1;
    if (lp->lperrd3 == 0)
      pr04oPutResultCount(cnt, lp->lperrd3);
    else {
      if (lp->lprescount != 0)
	pr04oPutResultCount(cnt, lp->lprescount);
      else
	pr04oPutResultCount(cnt, lp->lperrd3);
    }
  }
  pnofirst = lp->lpindi;
  if (lpcnt == 0) {
    if (*pid == csp1_p_mass_command
	|| *pid == csp1_p_mselect_found
	|| *pid == csp1_p_reuse_mselect_found
	|| *pid == csp1_p_for_upd_mselect_found
	|| *pid == csp1_p_reuse_upd_mselect_found)
      *pid -= csp1_p_mass_command;
    else {
      if (sqlca->sqlrap->ralang != CPR_LA_CALL)
	if (*pid == csp1_p_mfetch_found)
	  *pid = csp1_p_fetch_found;
    }
    /*bf 11.4.96 "parse mfetch" nur dynamisch internal-mode */
    if (sqlca->sqlrap->ralang == CPR_LA_CALL) {
      if ((*pid == csp1_p_mfetch_found)
	  && (sqlxa->xakano != cpr_com_mfetch)
	  && (sqlxa->xakano != cpr_com_mfetch_describe))
	*pid = csp1_p_fetch_found;
    }
  }
  else {
    switch (*pid) {
    case csp1_p_none:
      if (lpcnt == 1)
	lpcnt = lp->lpcnt = 0;
      else
	p08runtimeerror (sqlca, sqlxa, cpr_r_loop_init_wrong);
      break;
    case csp1_p_mselect_found:
    case csp1_p_for_upd_mselect_found:
    case csp1_p_reuse_mselect_found:
    case csp1_p_reuse_upd_mselect_found:
      if (lpcnt == 1) {
	lpcnt = lp->lpcnt = 0;
	*pid -= csp1_p_mass_command;
      }
      break;
    case csp1_p_mass_command:
    case csp1_p_mfetch_found: {
      break;
    }
    default:
      if (lpcnt == 1)
	lpcnt = lp->lpcnt = 0;
      else
	p08runtimeerror (sqlca, sqlxa, cpr_r_loop_init_wrong);
    }
  }
  memcpy (parsid, pidpart->sp1p_buf, ParseIdKnl_mxpr00);
  if (*pid == csp1_p_mfetch_found) {
    pr04oPutResultCount(cnt, lpcnt);
  }
  else {
    if (sqlca->sqlcode == 0 && da->F > 0) {
      do {
	for (i = 0; i < da->F; i++) {
	  short ival = 0;
	  struct SQLCOL *col = da->sqlcol + i;
	  char *V = da->V[i] + lpind * da->L[i];
	  short *I = ((da->I) ? da->I[i] : 0) ?
	    da->I[i] + lpind : &ival;
	  char *S = (da->S) ? da->S[i] : 0;
	  short C = (da->C) ? da->C[i] : (short) 0;

	  if (col->colio == CPR_INFO_OUTPUT)
	    continue;

	  if (trfirst) {
	    trfirst = 0;
	    if (lpind == 0) {
	      pr04LongTraceInit (sqlca, CPR_TR_ARRCNT, &blname, (int) da->sqlloop);
	      p04traceda(sqlca->sqlrap, (char*)da, da->sqlcol, cpr_kind_oracle);
	    }
	    rowno = lpind + lp->lpindi - pnofirst;
	    p04trloop (sqlca->sqlrap, (int)rowno);
	  }
	  err = cpr_p_ok;
	  p04orat (da->L[i], da->T[i], S, C, (char*)name, &vt, &vl, &vf, &err, sqlxa);
          if (vl == 0 && sqlca->sqlrap->rakamode == CPR_KIND_ORACLE)
            *I = CPR_NULL_DATA;
          if (datapart->sp1p_part_header.sp1p_buf_size > col->colpos + lppos) {
	  p04coltobuf (sqlca->sqlrap, (char*)data, &datapart->sp1p_part_header.
		       sp1p_buf_len, col, (char*)name, V, vt, vl, (tsp00_Int2) vl,
		       vf, *I, lppos, i + 1, sqlca->sqlemp);
	  err = (char) sqlca->sqlemp->eprerr;
          }
          else {
            err = cpr_request_area_overflow;
            p04err (sqlca->sqlrap, sqlca->sqlemp, err);
            p03csqlemptosqlca (sqlca, sqlca->sqlemp);
            return;
          }
	  p03csqlemptosqlca (sqlca, sqlca->sqlemp);
	  if (*ldcolkind == cpr_is_true) {
	    struct SQLPCLD *ldpc = ldp->ldpc;
	    struct SQLPCLD *p = ldpc + *ldmaxi - 1;
	    p->ldrowno = (lp->lpcnt == 0) ? (tsp00_Int2) 0 : (tsp00_Int2) (rowno + 1);
	    p->ldindkaddr = (I == &ival) ? 0 : I;
	    p->ldcoladdr = col;
	    p->ldindktype = CPR_VINT2;
	    p->ldindklen = 2;
	    p->ldindkfrac = 0;
	  }
	  if (lp->lperri == cpr_p_ok)
	    lp->lperri = err;
	} /* end for */
	if (lp->lpcnt != 0) {
	  if (sqlca->sqlcode == 0) {
	    if (lp->lprecli == 0)
	      lp->lprecli = datapart->sp1p_part_header.sp1p_buf_len;
	    lp->lpnext = cpr_is_true;
	    lpind++;
	  }
	  else {
	    datapart->sp1p_part_header.sp1p_buf_len = lppos;
	    lp->lpnext = cpr_is_false;
	  }
	  lpcnt = lpind - pnofirst;
	  if (lpind >= lp->lpcnt && lp->lpnext == cpr_is_true) {
	    datapart->sp1p_part_header.sp1p_attributes[0] |= 1 << sp1pa_last_packet;
	    if (sqlca->sqlrap->ralang != CPR_LA_CALL && cprFetchop2) {
	      ka = sqlxa->xaSQLDesc->ka;
	      if (ka->kastate == CPR_STATE_EXECUTE_AT_FETCH) {
		cbResCount = cu->culoopout;
		/* bis der Kern die 2. Stufe der Fetchoptimierung kann, muss der
		   ResultCount hier 0 sein */
		pr04oPutResultCount(cnt, cbResCount);
	      }
	    }
	  }
	  datapart->sp1p_part_header.sp1p_arg_count = (tsp00_Int2) lpcnt;
	  if (*pid != csp1_p_mselect_found
	      && *pid != csp1_p_reuse_mselect_found
	      && *pid != csp1_p_mass_command
	      && *pid != csp1_p_for_upd_mselect_found
	      && *pid != csp1_p_reuse_upd_mselect_found)
	    pr04oPutResultCount(cnt, lpcnt);
	}
	else {
	  /* PTS 1103453 */
	  datapart->sp1p_part_header.sp1p_attributes[0] |= 1 << sp1pa_last_packet;
	}
	pr04LongILvcOneRecord (sqlca, sqlxa, gae, &parsid, datapart, cntpart);
	if ((sqlca->sqlemp->ereturncode != 0) && (sqlca->sqlemp->ereturncode != 100))
	  return;
	/* PTS 1102362 */
	/* wenn Longspalten an den Kerngeschickt wurden konnen sich
	   die Partadressen veraendert haben */
	cnt = (cntpart) ? cntpart->sp1p_buf : NULL;
	data = (datapart) ? datapart->sp1p_buf : NULL;
	if (!cnt || !data)
	  p08runtimeerror(sqlca, sqlxa, cpr_memory_allocation_faild);
	p03csqlemptosqlca (sqlca, sqlca->sqlemp);
	if (da->sqlloop != 0) {
	  lppos = datapart->sp1p_part_header.sp1p_buf_len;
	  pr04LongInitLD (sqlca, sqlxa);
	  if (*allsend && sqlca->sqlcode == 0) {
	    short sint = (short) sqlca->sqlerrd[2];

	    M90TRACE (M90_TR_SWORD, "sqlerrd(3)", &sint);
	    lppos = 0;
	    datapart->sp1p_part_header.sp1p_buf_len = 0;
	    lpcnt = sint;
	    if (!(datapart->sp1p_part_header.sp1p_attributes[0] & 1 << sp1pa_last_packet))
	      pr04oPutResultCount(cnt, lpcnt);
	  }
	}
	p03csqlemptosqlca (sqlca, sqlca->sqlemp);
	trfirst = 1;
      }
      while (sqlca->sqlcode == 0
             && datapart->sp1p_part_header.sp1p_buf_len + lp->lprecli
	     + (lpind+1-pnofirst) * da->sqlrow.oreclen
	     <= datapart->sp1p_part_header.sp1p_buf_size
	     && lp->lpnext == cpr_is_true
	     && lpind < lp->lpcnt
	     && lppos != 0);
    }
    else {
      /* PTS 1103453 */
      datapart->sp1p_part_header.sp1p_attributes[0] |= 1 << sp1pa_last_packet;
    }
  }
  M90TRACE (M90_TR_SDWORD, "da->ireclen  ", &da->sqlrow.ireclen);
  M90TRACE (M90_TR_SDWORD, "da->oreclen  ", &da->sqlrow.oreclen);
  M90TRACE (M90_TR_SDWORD, "sqlerrd(3) vor", &sqlca->sqlerrd[2]);
  lp->lpindi = lpind;
  lp->lperrd3i = sqlca->sqlerrd[2];

  if (sqlca->sqlcode == 100)
    lpcnt = 0;

  if (lpcnt != 0)
    datapart->sp1p_part_header.sp1p_arg_count = (tsp00_Int2) lpcnt;

  M90TRACE (M90_TR_SDWORD, "sqlerrd(3)", &sqlca->sqlerrd[2]);
  if (*pid == csp1_p_mass_command)
    sqlca->sqlerrd[2] = lpcnt;  /* PTS 1108796 */
  else
    if (lpcnt != 0) {
      if (lp->lprescount != 0)
	sqlca->sqlerrd[2] = lp->lprescount;
      else
	sqlca->sqlerrd[2] = lpcnt;
    }
  M90TRACE (M90_TR_SDWORD, "lpcnt asaa", &lpcnt);
  M90TRACE (M90_TR_SDWORD, "sqlerrd(3)", &sqlca->sqlerrd[2]);
  if (lpind >= lp->lpcnt)
    lp->lpnext = cpr_is_false;
  else
    if (lp->lperri != cpr_p_ok)
      if (lp->lpcnt != 0 && lpcnt != 0) {
	sqlca->sqlcode = 0;
	sqlca->sqlemp->ereturncode = 0;
	sqlca->sqlemp->eprerr   = cpr_p_ok;
      }
  if (!sqlca->sqlrap->rasqlldp->ldallsend
      && datapart->sp1p_part_header.sp1p_buf_len > 0)
    s26finish_part (pck, datapart);
  else {
    /* PTS 1103453 */
    datapart->sp1p_part_header.sp1p_attributes[0] |= 1 << sp1pa_last_packet;
  }
  M90TRACE (M90_TR_EXIT, "p04oradin   ", 0);
}     /*--------------------------------------------*/

void
p04oradout (sqlcatype * sqlca, sqlxatype * sqlxa, sqlgaentry *gae, void *sqlda,
    struct tpr_sqlloop *lp)
{
  tsp1_packet *pck = (tsp1_packet*) gae->gareqptr;
  tsp1_segment *seg = (tsp1_segment *) sqlca->sqlrap->rasegptr;
  struct SQLERROR *em = sqlca->sqlemp;
  struct SQLDA   *da = (struct SQLDA *) sqlda;
  struct SQLLD   *ldp = sqlca->sqlrap->rasqlldp;
  tsp00_Int2       *ldmaxo = &ldp->ldmaxo;
  tsp00_Int2       *ldcolkind = &ldp->ldcolkind;
  tsp00_Int2        dbmode = sqlca->sqlrap->rakamode;
  tsp00_Int2       kafaindex;
  tsp00_Int2       famfindex;
  tsp00_Int4       temppos = 0;
  tsp00_Int4       *lppos = &temppos;
  /* ADIS 1001733	*/
  tsp00_Int4       *lpRecpos = &temppos;
  tsp00_Int4        lpcnt = 1;
  tsp00_Int4        lpind = 0;
  tsp00_Int4        rowno = 0;
  char            trfirst = 0;
  tsp00_Lname       blname;
  static const char errs[] = "ROW NOT FOUND";
  static const tsp00_Int2 ls = sizeof (errs) - 1;
  sqllname        name;
  tsp00_Int2        vt, vf;
  tsp00_Int4        vl;
  tsp00_Int4        pnofirst;
  tsp00_Int4        errd3;
  int             i;
  char            err, rownotfound;
  int             cntlen;
  tsp1_part *datapart;
  tsp00_Uint1 *data;
  tsp00_Int4 *nextlpcnt = NULL;
  tsp00_Int4 *lastrecord = NULL;

  if (da->sqlrow.oreclen == 0 || sqlca->sqlcode != 0) {
    return;
  }

  M90TRACE (M90_TR_ENTRY, "p04oradout  ", 0);
  memcpy (blname, "                                ", sizeof (tsp00_Lname));
  if (sqlca->sqlrap->ralang != CPR_LA_CALL) {
    sqlkapointer    ka = sqlxa->xaSQLDesc->ka;

    if (ka->kapaindex == 0)
      {
	da->sqloffset = 0;
	da->sqlloop = 0;
      }
    else if (ka->kapaindex < 0)
      {
	da->sqloffset = lp->lpindo;
	da->sqlloop = lp->lpcnt;
      }
    else
      p04err (sqlca->sqlrap, sqlca->sqlemp,
	      cpr_not_implemented);
    p03csqlemptosqlca (sqlca, sqlca->sqlemp);
  }
  else {
    if (da->sqlloop == 0)
      da->sqloffset = 0;
  }
  p04mode (sqlca, gae);
  if (lp->lpfirsto == cpr_is_true) {
    lp->lpfirsto = cpr_is_false;
    lp->lpmax = (tsp00_Int2) ((lp->lpcnt == 0) ?  1 : da->sqlloop);
    if (da->sqlloop != 0)
      pr04LongTraceInit (sqlca, CPR_TR_ARRCNT, &blname, (int) lp->lpmax);
    p04traceda(sqlca->sqlrap, (char*)da, da->sqlcol,cpr_kind_oracle);
  }
  /**select into ****/
  if (seg->sp1s_segm_header.sp1r_function_code == csp1_mselect_into_fc)
    lp->lprescount = (tsp00_Int2) sqlca->sqlerrd[2];
  else
    lp->lprescount = 0;
  p03find_part (sqlca->sqlrap, sp1pk_data, &datapart);
  errd3 = p04rescount (seg,
		       datapart->sp1p_part_header.sp1p_buf_len, &cntlen);
  if (errd3 == 0)	{
    em->ereturncode = 100;
    em->eerrorpos = 0;
    memcpy (em->etext, errs, ls);
    em->etextlen = ls;
    p03returncodeget (sqlca, sqlxa);
    M90TRACE (M90_TR_EXIT, "p04oradout  ", 0);
    return;
  }
  pnofirst = da->sqloffset;
  rowno = lpind + pnofirst;
  lp->lpreclo = da->sqlrow.oreclen;
  data = datapart->sp1p_buf;
  if (da->sqlloop != 0) {  /* Arraycommand */
    lpcnt = datapart->sp1p_part_header.sp1p_arg_count;
    if (lpcnt > da->sqlloop) {
      lpcnt = da->sqlloop;
    }
  }
  if (sqlca->sqlrap->ralang != CPR_LA_CALL) {
    kafaindex = sqlxa->xaSQLDesc->ka->kafaindex;
    if (kafaindex > 0)
      famfindex = sqlxa->xaSQLDesc->ka->kafae->famfindex;
    else
      famfindex = 0;
  }
  else
    famfindex = da->sqlkano;
  if (famfindex > 0) {
    sqlmfentry *mf = &sqlca->sqlmfp->mfentry[famfindex-1];
    nextlpcnt = &mf->mfNextLoopCnt;
    lppos = &mf->mfActualPos;
    lastrecord = &mf->mfAllRecordsRead;
    /* ADIS 1001733	*/
    lpRecpos = &mf->mfRecpos;
    if (*nextlpcnt == -1) {
      *nextlpcnt = datapart->sp1p_part_header.sp1p_arg_count;
      if (*nextlpcnt >= da->sqlloop) {
	*nextlpcnt -= da->sqlloop;
      }
      else {
	lpcnt = *nextlpcnt;
	*nextlpcnt = 0;
      }
    }
    else {
      if ( *nextlpcnt >= da->sqlloop) {
	*nextlpcnt -= da->sqlloop;
      }
      else {
	lpcnt = *nextlpcnt;
	*nextlpcnt = 0;
	lp->lpnext = cpr_is_true;
	/* ADIS 1001733	*/
	*lpRecpos = 0;
      }
    }
    if (*lppos >= datapart->sp1p_part_header.sp1p_buf_len)
      *lppos = 0;
    trfirst = 1;
  }
  if (da->F > da->N) {
    da->F = -da->F;
    p08runtimeerror (sqlca, sqlxa, cpr_incompatible_datatype);
    return;
  }
  while (sqlca->sqlcode == 0 && lpind < lpcnt) {
    int outpfound = 0;
    for (i = 0; i < PR07MIN(da->N,da->F) ; i++) {
      tsp00_Int4 ival;
      struct SQLCOL *col = da->sqlcol + i;
      char *V = da->V[i] + rowno * da->L[i];
      short *I = ((da->I) ? da->I[i] : 0) ?
	da->I[i] + rowno : 0;
      char *S = (da->S) ? da->S[i] : 0;
      short C = (da->C) ? da->C[i] : (short) 0;

      if (col->colio == CPR_INFO_INPUT)
	continue;
      outpfound = 1;
      if (trfirst) {
	trfirst = 0;
	p04trloop (sqlca->sqlrap, (int) rowno);
      }
      err = cpr_p_ok;
      p04orat (da->L[i], da->T[i], S, C, (char*)name, &vt, &vl, &vf, &err, sqlxa);
      p04colfrombuf (sqlca->sqlrap, (char*)data,
		     &datapart->sp1p_part_header.sp1p_buf_len, col,
		     (char*)name, V, vt, vl, (tsp00_Int2) vl, vf, &ival,
		     *lppos, i + 1, sqlca->sqlemp);
      err = (char) sqlca->sqlemp->eprerr;
      p03csqlemptosqlca (sqlca, sqlca->sqlemp);
      /* ldcolkind switched to true in p04colfrombuf */
      /* if the column in a longcolumn */
      /* otherwise it is false */
      if (*ldcolkind == cpr_is_true) {
	struct SQLPCLD *ldpc = ldp->ldpc;
	/* ldmaxo indexes the the actual longdescriptor  */
	/* ldmaxo is set by p04colfrombuf (p04longout) */
	struct SQLPCLD *p = ldpc + *ldmaxo - 1;

	p->ldrowno = (lp->lpcnt == 0) ?
	  (short) 1 : (short) (rowno + 1);
	p->ldindkaddr = I;
	p->ldcoladdr = col;
	p->ldindktype = CPR_VINT2;
	p->ldindklen = 2;
	p->ldindkfrac = 0;
      }
      if (I) {
	short sint = (short) ival;
	memcpy (I, &sint, sizeof (short));
      }
      else {
	if (ival == CPR_NULL_DATA) {
	  p04err (sqlca->sqlrap, sqlca->sqlemp,
		  cpr_out_null_not_allowed);
	  if (lp->lperro == cpr_p_ok)
	    lp->lperro = cpr_out_null_not_allowed;
	}
      }
      if (lp->lperro == cpr_p_ok)
	lp->lperro = err;
    }
    if (outpfound) {
      p03csqlemptosqlca (sqlca, sqlca->sqlemp);
      if (lppos)
	*lppos += lp->lpreclo;
      lpind++;
      rowno++;
      trfirst = 1;
    }
    else
      return;
  }
  rownotfound = 0;
  if (lpcnt == 0 && (dbmode == CPR_KIND_ORACLE || dbmode == CPR_KIND_SAPR3)) {
    rownotfound = 1;
    lp->lpnext = cpr_is_false;
  }
  else {
    errd3 = (sqlca->sqlcode == 0) ? lpind : lpind - 1;
    lp->lperrd3 += errd3;
    lp->lpindo = (short) (errd3 + pnofirst);
    if  (datapart->sp1p_part_header.sp1p_attributes[0] & 1
	 << sp1pa_last_packet ) {
      lp->lpnext = cpr_is_false;
      if (lastrecord)
	*lastrecord = cpr_is_true;
    }
    else {
      if (lp->lperrd3 >= lp->lpmax) {
	lp->lpnext = cpr_is_false;
      }
      else {
	lp->lpcnt -= errd3;
	lp->lpnext = cpr_is_true;
      }
    }
  }
  if (lpind == 0 || (rownotfound && sqlca->sqlcode == 0)) {
    em->ereturncode = 100;
    em->eerrorpos = 0;
    memcpy (em->etext, errs, ls);
    em->etextlen = ls;
    p03returncodeget (sqlca, sqlxa);
  }
  M90TRACE (M90_TR_SDWORD, "sqlerrd[2]  6", &sqlca->sqlerrd[2]);
  M90TRACE (M90_TR_SDWORD, "lp->lperrd3  ", &lp->lperrd3);
  M90TRACE (M90_TR_SDWORD, "lp->lpcnt    ", &lp->lpcnt);
  M90TRACE (M90_TR_SDWORD, "lp->lpnext   ", &lp->lpnext);
  M90TRACE (M90_TR_EXIT, "p04oradout  ", 0);
}     /*--------------------------------------------*/

void
p04sftoora (struct SQLROW *row, struct SQLCOL *col,
	    const tsp1_param_info *sfi, tsp00_Int4 *l, short *t)
{
  M90TRACE (M90_TR_ENTRY, "p04sftoora   ", 0);
  if (sfi)
    p04decode (sfi, row, col);
  *l = col->collen;
  M90TRACE (M90_TR_SCHAR, "col->coltype", &col->coltype);
  switch (col->coltype) {
  case dfixed: {
    *l = (col->collen << 8) + col->colfrac;
    *t = SQL_ORA_NUMBER;
    break;
  }
  case dfloat: {}
  case dvfloat: {
    *l = 0;
    *t =  SQL_ORA_FLOATPOINT;
    break;
  }
  case dsmallint: {}
  case dinteger: {
    *t =  SQL_ORA_FIXEDPOINTINTEGER;
    break;
  }
  case drowid: {
    *t =  SQL_ORA_ROWID;
    break;
  }
  case dboolean: {}
  case dcha: {}
  case dche: {}
  case dvarchara: {}
  case dvarchare: {}
  case dtime: {}
  case ddate: {
    if ((short) (col->collen <= (short) 254))
      *t = SQL_ORA_CHAR;
    else {
      if (col->collen > 2000)
	*t = SQL_ORA_LONG;
      else
	*t = SQL_ORA_VARCHAR2;

    }
    break;
  }
  case dtimestamp: {
    *t = SQL_ORA_INTERNDATE;
    break;
  }
  case ddbyteebcdic: {
    *l *= 2;            /* fall through */
    *t = SQL_ORA_RAW;
    break;
  }
  case dunicode: {
    *l = (col->coliolen-1)/2;
    *t = SQL_ORA_UCS2;
    break;
  }
  case dstruni: {}
  case dlonguni: {
    *l = (col->coliolen-1)/2;
    *t = SQL_ORA_LONG_UNICODE;
    break;
  }
  case dchb: {}
  case dvarcharb: {
    if ((short)(*l <= (short) 254))
      *t =  SQL_ORA_RAW;
    else
      *t =  SQL_ORA_VARRAW;
    break;
  }
  case dstra: {}
  case dstre: {}
  case dlonga: {}
  case dlonge: {
    *t = SQL_ORA_LONGVARCHAR;
    break;
  }
  case dstrb: {}
  case dlongb: {
    *t = SQL_ORA_LONGRAW;
    break;
  }
  case dstrdb: {}
  case dlongdb: {
    *t = SQL_ORA_LONGRAW;
    break;
  }
  case dabaptabhandle: {
    *t = SQL_ORA_ABAP_HANDLE;
    break;
  }
  default: {
    *t = -1; /*SQL_ORA_UNKNOWNDATATYPE;*/
    break;
  }
  }
  if (col->colmode & 1 << CPR_INFO_OPTIONAL)
    *t += -32768;
  M90TRACE (M90_TR_EXIT, "p04sftoora   ", 0);
}     /*--------------------------------------------*/

short
p04oradsfi (sqlcatype * sqlca, sqlxatype * sqlxa, sqlorentry *ore, void *sqlda, char fetchdesc, tpr00_DescribeKindEnum DescribeKind)
{
  tsp1_segment *seg = (tsp1_segment *) sqlca->sqlrap->rasegptr;
  struct SQLDA   *da = (struct SQLDA *) sqlda;
  struct SQLCOL  *col;
  tsp1_part *part;
  tsp00_Uint1 *buf, *ebuf;
  short ln;
  char          **S = da->S;
  short          *M = da->M, *C = da->C, *T = da->T;
  sqlint4        *L = da->L;
  int i;

  M90TRACE (M90_TR_ENTRY, "p04oradsfi   ", 0);
  p03find_part (sqlca->sqlrap, sp1pk_shortinfo, &part);
  if (!part) {
    if (!ore || (ore->orcolcntacc == 0 && ore->orrescntacc == 0)) {
      p08runtimeerror (sqlca, sqlxa, cpr_not_implemented);
      return 0;
    }
  }
  if (part) {
    da->F = part->sp1p_part_header.sp1p_arg_count;
  }
  else {
    if (DescribeKind == Describe_Params_epr00)
      da->F = ore->orcolcntacc;
    else
      da->F = ore->orrescntacc;
  }

  if (da->F > da->N) {
    da->F = -da->F;
    p08runtimeerror (sqlca, sqlxa, cpr_too_many_hostvar);
    return 0;
  }
  if (part) {
    buf = part->sp1p_buf;
    if (DescribeKind == Describe_Params_epr00)
      p04oracolinit (sqlca, sqlxa, sqlda);
  }
  if (part) {
    col = da->sqlcol;
  }
  else {
    if (DescribeKind == Describe_Params_epr00)
      col = ore->orcolptr;
    else
      col = ore->orresptr;
  }
  p04init (&da->sqlrow);
  for (i=0; i < da->F;i++) {
    if (part) {
      tsp1_param_info sfi;
      memcpy (&sfi, buf, sizeof (tsp1_param_info));
      p04sftoora (&da->sqlrow, col++, &sfi, L++, T++);
      buf += sizeof (tsp1_param_info);
    }
    else {
      p04sftoora (&da->sqlrow, col++, NULL, L++, T++);
    }
  }
  if (S) {
    p03find_part (sqlca->sqlrap, sp1pk_columnnames, &part);
    if (part) {
      buf = part->sp1p_buf;
      ebuf = buf + part->sp1p_part_header.sp1p_buf_len;
      while (buf < ebuf) {
	if (*S) {
	  ln = *buf++;
	  *C = (ln > *M) ? *M : ln;
	  memcpy (*S, buf, *C);
	}
	S++, C++, M++;
	buf += ln;
      }
    }
    else {
      if (! fetchdesc) {
	static const char s[] = "COLUMN";
	static const int ls = sizeof (s) - 1;
	static char     coln[sizeof (s) + 10] = "";
	int i = 1;
	while (i <= da->F) {
	  if (*S) {
	    if (coln[0] == '\0')
	      memcpy (coln, s, ls);
	    ln = (short) (ls + sprintf
			  (&coln[ls], "%d", i++));
	    *C = (ln > *M) ? *M : ln;
	    memcpy (*S, coln, *C);
	  }
	  S++, C++, M++;
	}
      }
    }
  }
  M90TRACE (M90_TR_EXIT, "p04oradsfi   ", 0);
  return da->sqlrow.oreclen;
}     /*--------------------------------------------*/

void
p04din (sqlcatype * sqlca, sqlxatype * sqlxa, sqlgaentry *gae, sqlcuentry *cu, struct tpr_sqlloop *lp)
{
  switch (sqlca->sqldbmode) {
  case CPR_KIND_INTERNAL: {
    p04sqldin (sqlca, sqlxa, gae, sqlca->sqlcxap->xasqldap, lp);
    break;
  }
  case CPR_KIND_ORACLE: {}
  case CPR_KIND_SAPR3: {
    p04oradin (sqlca, sqlxa, gae, cu, sqlca->sqlcxap->xasqldap, lp);
    break;
  }
  case CPR_KIND_DB2: {
    lp->lperrd3i = 0;
    p04db2din (sqlca, sqlxa, gae, sqlca->sqlcxap->xasqldap);
    break;
  }
  default: {
    p04sqldin (sqlca, sqlxa, gae, sqlca->sqlcxap->xasqldap, lp);
    break;
  }
  }
}     /*--------------------------------------------*/

void
p04dout (sqlcatype * sqlca, sqlxatype * sqlxa, sqlgaentry *gae, struct tpr_sqlloop *lp)
{
  tsp1_segment *seg = (tsp1_segment *) sqlca->sqlrap->rasegptr;
  tsp1_part *datapart;

  p03find_part (sqlca->sqlrap, sp1pk_data, &datapart);
  if (datapart) {
    switch (sqlca->sqldbmode) {
    case CPR_KIND_INTERNAL: {
      p04sqldout (sqlca, sqlxa, gae, sqlca->sqlcxap->xasqldap, lp);
      break;
    }
    case CPR_KIND_ORACLE: {}
    case CPR_KIND_SAPR3: {
      p04oradout (sqlca, sqlxa, gae, sqlca->sqlcxap->xasqldap, lp);
      break;
    }
    case CPR_KIND_DB2: {
      p04db2dout (sqlca, gae, sqlca->sqlcxap->xasqldap);
      break;
    }
    default: {
      p04sqldout (sqlca, sqlxa, gae, sqlca->sqlcxap->xasqldap, lp);
      break;
    }
    }
  }   /* datapart == null */
}      /*--------------------------------------------*/

short p04or2orcda (sqlcatype * sqlca, sqlxatype * sqlxa, sqlorentry *ore, tpr00_DescribeKindEnum DescribeKind) {

  SQLDA *da = sqlca->sqlcxap->xasqldap;
  struct SQLCOL  *col;
  char          **S = da->S;
  short          *M = da->M, *C = da->C, *T = da->T;
  sqlint4        *L = da->L;
  int i;

  if (DescribeKind == Describe_Params_epr00) {
    da->F = ore->orcolcntacc;
    col = ore->orcolptr;
  }
  else {
    da->F = ore->orrescntacc;
    col = ore->orresptr;
  }

  if (da->F > da->N) {
    da->F = -da->F;
    p08runtimeerror (sqlca, sqlxa, cpr_too_many_hostvar);
    return 0;
  }

  p04init (&da->sqlrow);

  for (i=0; i < da->F;i++) {
      p04sftoora (&da->sqlrow, col++, NULL, L++, T++);
  }

  /*handle Columnnames*/
  if (S){
    if (DescribeKind ==Describe_Columns_epr00
        && ore->orresnamacc != 0) {
      tsp00_Uint4 cbWrite, cbParsed;
      int i = 0, pos = 0, len;
      while (i <  da->F) {
        memset (da->S[i],' ',da->M[i]);
        len = (int) ((char*)ore->orresnamptr)[pos++];

        if (len > da->M[i])
          da->C[i] = da->M[i];
        else
          da->C[i] = len;

        sp78convertString(sp77encodingUTF8,
                         (void *)da->S[i],
                          da->M[i],
                          &cbWrite,
                          TRUE,
                          (tsp77encoding *)ore->orresnamencoding,
                          &(((char*)ore->orresnamptr)[pos]),
                          (tsp00_Uint4)len,
                          &cbParsed);

        pos += len;
        i ++;
      }
    }
    else{
      static const char s[] = "COLUMN";
      static const int ls = sizeof (s) - 1;
      static char     coln[sizeof (s) + 10] = "";
      int i = 1;
      short ln;
      while (i <= da->F) {
        if (*S) {
          if (coln[0] == '\0')
            memcpy (coln, s, ls);
          ln = (short) (ls + sprintf(&coln[ls], "%d", i++));
          *C = (ln > *M) ? *M : ln;
          memcpy (*S, coln, *C);
          if (*C < *M) (*S)[*C] = '\0';
        }
        S++, C++, M++;
      }
    }
  }/*end if (S)*/
  return da->sqlrow.oreclen;
}

void p04or2da (sqlcatype * sqlca, sqlxatype * sqlxa, sqlorentry *ore, tpr00_DescribeKindEnum DescribeKind) {

  switch (sqlca->sqldbmode) {
    case CPR_KIND_INTERNAL: {
      p04or2sqlda (sqlca, sqlxa, ore, DescribeKind);
      break;
    }
    case CPR_KIND_ORACLE: {}
    case CPR_KIND_SAPR3: {
      p04or2orcda (sqlca, sqlxa, ore, DescribeKind);
      break;
    }
    default: {
      p08runtimeerror (sqlca, sqlxa, cpr_not_implemented);
      break;
    }
  }
}     /*--------------------------------------------*/

void
p04sfi (sqlcatype * sqlca, sqlxatype * sqlxa, tpr00_DescribeKindEnum DescribeKind)
{
  sqlkaentry *ka = sqlxa->xaSQLDesc->ka;
  sqlprentry *pr = sqlxa->xaSQLDesc->pr;
  sqlorentry *ore = sqlxa->xaSQLDesc->ore;
  tsp00_Int4        len = 0;

  M90TRACE (M90_TR_ENTRY, "p04sfi   ", 0);

  if (sqlca->sqlcode < 0)
    return;

  if (ka->kapacount < 0) {
    if (sqlca->sqlcxap->xasqldap != NULL)
      if ((pr != NULL) ? pr->prDescribe == cpr_is_true : 0) {
	char            fetchdesc = (char) (ka->kapacount == -4);

	pr->prDescribe = cpr_is_init;

	((struct SQLDA *) sqlca->sqlcxap->xasqldap)->sqlkano = sqlxa->xakano;
	((struct SQLDA *) sqlca->sqlcxap->xasqldap)->sqlprno = ka->kaprindex;
	switch (sqlca->sqldbmode) {
	case CPR_KIND_INTERNAL: {
	  len = p04sqldsfi (sqlca, sqlxa, ore, sqlca->sqlcxap->xasqldap, fetchdesc, 0, DescribeKind);
	  break;
	}
	case CPR_KIND_ORACLE: {}
	case CPR_KIND_SAPR3: {
	  len = p04oradsfi (sqlca, sqlxa, ore, sqlca->sqlcxap->xasqldap, fetchdesc, DescribeKind);
	  break;
	}
	case CPR_KIND_DB2: {
	  len = p04db2dsfi (sqlca, sqlxa, ore, sqlca->sqlcxap->xasqldap, fetchdesc, 0);
	  break;
	}
	default: {
	  len = p04sqldsfi (sqlca, sqlxa, sqlca->sqlcxap->xasqldap,
			    ore, fetchdesc, 0, DescribeKind);
	  break;
	}
	}
      }
      else
	len = ((struct SQLDA *) sqlca->sqlcxap->xasqldap)->sqlrow.oreclen;
  }
  else {
    if (ka->kapaindex > 0) {
      tsp1_part *part;
      tsp00_Int2 sfcnt = 0, parcnt = 0;
      sqlparpointer   pa = sqlxa->sqlpap + ka->kapaindex - 1;
      sqlparpointer   pamx = pa + ka->kapacount - 1;

      p03find_part (sqlca->sqlrap, sp1pk_shortinfo, &part);

      if (!part) {
        if (!ore || (ore->orcolcntacc == 0 && ore->orrescntacc == 0)) {
          p08runtimeerror (sqlca, sqlxa, cpr_not_implemented);
          return;
        }
        sfcnt = ore->orcolcntacc;
      }
      else
        sfcnt = part->sp1p_part_header.sp1p_arg_count;

      /*count number of parameters in statement - ignore loop count*/
      if (pa->lo.pakind == SQLPARLO)
        pa++;
      for (; pa <= pamx; pa++) {
        if (pa->st.pakind == SQLPARST) {
	      parcnt += pa->st.paelcnt;
          pa++;
        }
        else {
          sqlv1pointer    va1 = sqlxa->sqlv1p + pa->el.pavarno - 1;
          if (va1->pt.va1indi == SQLVAPT)
            va1 = sqlxa->sqlv1p + va1->pt.va1ix - 1;
          parcnt += (va1->sc.va1indi == SQLVAST) ? va1->st.va1cmpcnt : (tsp00_Int2) 1;
        }
      }

      /*check matching number of paramters*/
      if (parcnt  > sfcnt) {
         p08runtimeerror (sqlca, sqlxa, cpr_paramlist_too_long);
      }
      else if (parcnt < sfcnt) {
        p08runtimeerror (sqlca, sqlxa, cpr_paramlist_too_short);
      }

      if (part) {
        /*copy parameter infos from sfipart*/
        p04GetSFInfo(sqlca, sqlxa, ka, ore);
      }

      len = ore->orsqlrow.oreclen;
    }
    /* PTS 1106262 */
    else {
      if (DescribeKind == Describe_Params_epr00) {
        p04GetSFInfo(sqlca, sqlxa, ka, ore);
      }
      else {
        boolean preparse_dummy = false;
        p01resinitsfinfo (sqlca, sqlxa, ka, ore, &preparse_dummy);
        p01resmovesfinfo (sqlca, sqlxa, ka, ore, &preparse_dummy);
      }
    }
  }
  if (ka->kafaindex > 0)
    ka->kafae->fareclen = len;
  M90TRACE (M90_TR_EXIT, "p04sfi   ", 0);
}     /*--------------------------------------------*/


void p04oracolinit (sqlcatype * sqlca, sqlxatype * sqlxa, void *sqlda)
{
  struct SQLDA   *da = (struct SQLDA *) sqlda;
  tsp00_Int4  len;
  tsp00_Int4  cnt;

  M90TRACE (M90_TR_ENTRY, "p04oracolinit", 0);
  M90TRACE (M90_TR_SDWORD, "da->F     ", &da->F);
  if (da->F > 0) {
    if (sqlca->sqlrap->ralang != CPR_LA_CALL) {
      /* allocate in sqlor-area */
      sqlorentry *ore = sqlxa->xaSQLDesc->ore;
      M90TRACE (M90_TR_SDWORD, "orcolcnt v", &ore->orcolcnt);
      M90TRACE (M90_TR_SDWORD, "orcolcntacc v", &ore->orcolcntacc);
      M90TRACE (M90_TR_SDWORD, "orcollen v", &ore->orcollen);
      M90TRACE (M90_TR_SDWORD, "orcolptr v", &ore->orcolptr);
      if (ore->orcolptr != NULL) {
	if (ore->orcolcnt < da->F) {
	  pr03mFree(ore->orcolptr, "orcolptr");
	  ore->orcolcnt = 0;
	  ore->orcollen = 0;
	  ore->orsqlrow.ireclen = 0;
	  ore->orsqlrow.oreclen = 0;
	  /* PTS 1102399 */
	  p03orfree (sqlca, ore->orcolptr, 1);
	  ore->orcolptr = NULL;
	}
      }
      if (ore->orcolptr == NULL) {
	ore->orcolcnt = da->F;
	ore->orcollen = da->F * sizeof (struct SQLCOL);
	ore->orcolptr = pr03mAllocat(ore->orcollen, "ore->orcolptr");
      }
      if (ore->orcolptr) {
	ore->orcolcntacc = da->F;
	da->sqlcol = ore->orcolptr;
	M90TRACE (M90_TR_SDWORD, "orcolcnt n", &ore->orcolcnt);
	M90TRACE (M90_TR_SDWORD,"orcolcntacc n",&ore->orcolcntacc);
	M90TRACE (M90_TR_SDWORD, "orcollen n", &ore->orcollen);
	M90TRACE (M90_TR_SDWORD, "orcolptr n", &ore->orcolptr);
      }
      else {
	p08runtimeerror (sqlca, sqlxa, cpr_memory_allocation_faild);
	sqlabort();
      }
    }
    else {
      /* allocate for call-interface */
      cnt = da->F;
      len = da->F * sizeof (struct SQLCOL);
      da->sqlcol = pr03mAllocat(len, "da->sqlcol");
      if (!da->sqlcol) {
	p08runtimeerror (sqlca, sqlxa, cpr_memory_allocation_faild);
	sqlabort();
      }
    }
  }
  M90TRACE (M90_TR_EXIT, "p04oracolinit", 0);
}

void
p04trline(sqlratype * sqlrap, char*szStr)
{

  sqltatype *tap = sqlrap->rasqltap;
  tsp00_Int2 *ll = &tap->tastr80l;
  if (tap->tatrout != CPR_TRACE_LONG && tap->tatrout != CPR_TRACE_MODLONG)
    return;
  *ll = (tsp00_Int2)strlen(szStr);
  *ll = (*ll > sizeof(tap->tastr80)-1) ? sizeof(tap->tastr80)-1 : *ll;
  memcpy(tap->tastr80, szStr, *ll);
  p08vfwritetrace (sqlrap);
}

#ifdef P04TRENTRIES
void
p04trentries(sqlratype * sqlrap, sqlcatype *cap, sqlxatype *xap, char *szStr)
{

  sqltatype *tap = (sqlrap) ? sqlrap->rasqltap : NULL;
  tsp00_Int2 *ll = (tap) ? &tap->tastr80l : NULL;
  if (tap->tatrout != CPR_TRACE_LONG && tap->tatrout != CPR_TRACE_MODLONG)
    return;
  if (xap) {
    *ll = sprintf(tap->tastr80, "%s", szStr);
    p08vfwritetrace (sqlrap);
    {
      int ka=-99, st=-99, pr=-99, fa=-99, cu=-99, mf=-99, or=-99;
      ka = xap->xakano-1;
      if (ka >= 0 && xap) {
	sqlkapointer kap=xap->sqlkap+ka;
	st = kap->kastindex-1;
	pr = kap->kaprindex-1;
	cu = kap->kacuindex-1;
	fa = kap->kafaindex-1;
	if (fa >= 0) {
	  sqlfapointer fap=xap->sqlfap+kap->kafaindex;
	  mf = fap->famfindex-1;
	  if (mf >= 0 && cap) {
	    sqlmfentry *mfp = &cap->sqlmfp->mfentry[mf];
	    or = mfp->mforindex;
	  }
	}
      }
      *ll = sprintf(tap->tastr80, "ka=%d, st=%d, pr=%d, fa=%d, cu=%d, mf=%d, or=%d",
		    ka, st, pr, fa, cu, mf, or);
      p08vfwritetrace (sqlrap);
    }
  }
}
#endif

void p04trint2(sqlratype * sqlrap, char *szStr, tsp00_Int2 cbValue)
{
  sqltatype *tap = (sqlrap) ? sqlrap->rasqltap : NULL;
  tsp00_Int2 *ll = (tap) ? &tap->tastr80l : NULL;
  if (tap->tatrout != CPR_TRACE_LONG && tap->tatrout != CPR_TRACE_MODLONG)
    return;
  *ll = sprintf(tap->tastr80, "%s: %d", szStr, cbValue);
  p08vfwritetrace (sqlrap);
}

void p04trint4(sqlratype * sqlrap, char *szStr, tsp00_Int4 cbValue)
{
  sqltatype *tap = (sqlrap) ? sqlrap->rasqltap : NULL;
  tsp00_Int2 *ll = (tap) ? &tap->tastr80l : NULL;
  if (tap->tatrout != CPR_TRACE_LONG && tap->tatrout != CPR_TRACE_MODLONG)
    return;
  *ll = sprintf(tap->tastr80, "%s: %ld", szStr, cbValue);
  p08vfwritetrace (sqlrap);
}

void p04traceda (sqlratype * sqlrap, char *daIn, struct SQLCOL *col, tsp00_Int2 sqldbmode)
{
  sqltatype      *tap = sqlrap->rasqltap;
  tsp00_Int2       *ll = &tap->tastr80l;
  int i;

  if (tap->tatrout != CPR_TRACE_LONG && tap->tatrout != CPR_TRACE_MODLONG)
    return;
  if (!daIn) {
    sprintf(tap->tastr80, "SQLDA is NULL");
    *ll = (tsp00_Int2)strlen(tap->tastr80);
    p08vfwritetrace (sqlrap);
  }
  else {
      *ll=sprintf(tap->tastr80, "PARAMETER DESCRIPTION: %#08p", daIn);
      p08vfwritetrace (sqlrap);
    if (   sqldbmode == cpr_kind_oracle
        || sqldbmode == cpr_kind_sapr3){
      SQLDA          *da = (SQLDA*)daIn;
      *ll=sprintf(tap->tastr80, "APPLICATION            KERNEL");
      p08vfwritetrace (sqlrap);
      /*                         Nr.  T     L           TYPE  LEN         FRAC */
      *ll=sprintf(tap->tastr80, "%-4s %-5s %-11s %-5s %-11s %-5s",
  		"Nr.", "T", "L", "TYPE", "LEN", "FRAC");
      p08vfwritetrace (sqlrap);
      *ll=sprintf(tap->tastr80, "---------------------------------------------");
      p08vfwritetrace (sqlrap);
      for(i=0; i < da->F; i++) {
        if (i< da->N) {
          *ll=sprintf(tap->tastr80, "%-4d %-5d %-11d %-5d %-11d %-5d",
                      i+1, da->T[i], da->L[i], col->coltype, col->collen, col->colfrac);
        }
        else {
          *ll=sprintf(tap->tastr80, "%-4d -     -           %-5d %-11d %-5d",
                      i+1, col->coltype, col->collen, col->colfrac);
        }
        p08vfwritetrace (sqlrap);
        col++;
      }
    }
    else if (sqldbmode == cpr_kind_internal){
      sqldatype *da = (sqldatype*)daIn;
      struct SQLCOL* sqlcol;
      *ll=sprintf(tap->tastr80, "Application            Kernel");
      p08vfwritetrace (sqlrap);
      /*                         Nr.  Type  Length      Type  Length      Fraction*/
      *ll=sprintf(tap->tastr80, "%-4s %-5s %-11s %-5s %-11s %-9s",
  		"Nr.", "Type", "Length", "Type", "Length", "Fraction");
      p08vfwritetrace (sqlrap);
      *ll=sprintf(tap->tastr80, "---------------------------------------------");
      p08vfwritetrace (sqlrap);

      for(i=0; i < da->sqln; i++) {
        sqlcol = (col == NULL)?&da->sqlvar[i].col:col++;
        if (i< da->sqlmax) {
          *ll=sprintf(tap->tastr80, "%-4d %-5d %-11d %-5d %-11d %-5d",
                      i+1, da->sqlvar[i].hostvartype, da->sqlvar[i].collength, sqlcol->coltype, sqlcol->collen, sqlcol->colfrac);
        }
        else {
          *ll=sprintf(tap->tastr80, "%-4d -     -           %-5d %-11d %-5d",
                      i+1, sqlcol->coltype, sqlcol->collen, sqlcol->colfrac);
        }
        p08vfwritetrace (sqlrap);
      }
    }
  }
}

tsp00_Int4 pr04GetLoopCnt(sqlcatype *sqlca, sqlxatype *sqlxa, struct tpr_sqlloop *lp)
{
  tsp00_Int4 lpcnt=0;
  /* PTS 1104383 */
  if (sqlca->sqlrap->ralang != CPR_LA_CALL) {
    sqlkapointer  ka = sqlxa->xaSQLDesc->ka;
    /* PTS 1104507 */
    int paind = (ka->kapaindex > 0) ? ka->kapaindex : -ka->kapaindex;
    /* PTS 1104942 */
    if (paind > 0) {
      sqlparpointer pa = sqlxa->sqlpap + paind - 1;
      if (pa->lo.pakind == SQLPARLO) {
	if (pa->lo.pava1index > 0) {
	  sqlv1pointer va1 = sqlxa->sqlv1p + pa->lo.pava1index - 1;
	  sqlv2pointer va2 = sqlxa->sqlv2p + va1->sc.va1indva2 - 1;
	  tsp00_Int2 sint;
	  tsp00_Int4 lint;

	  switch (va2->va2typ) {
	  case CPR_VINT2:
	  case CPR_VUNS2:
	    memcpy(&sint, va1->sc.va1addr, sizeof (sint));
	    lpcnt = sint;
	    break;
	  case CPR_VINT4:
	  case CPR_VUNS4:
	    memcpy(&lint, va1->sc.va1addr, sizeof (lint));
	    lpcnt = lint;
	    break;
	  default:
	    p08runtimeerror(sqlca, sqlxa, cpr_unknown_datatype);
	    if (lp)
	      if (lp->lperri == cpr_p_ok)
		lp->lperri = cpr_unknown_datatype;
	  }
	}
	else
	  lpcnt = pa->lo.paloopcnt;
      }
      else
	if (lp)
	  lpcnt = lp->lpcnt;
    }
    else
      if (lp)
	lpcnt = lp->lpcnt;
  }
  return(lpcnt);
}

char pr04oPutResultCount(tsp00_Uint1 *buf, int cbRows)
{
    char res;
    if (buf) {
      if (cbRows == 0)
	*buf = CPR_UNDEF_BYTE;
      else
	*buf = CPR_DEFINED_BYTE;
      s41p4int((char*)buf, 2, cbRows, &res);
    }
    return(res);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

static char ident_sr[] = "@(#)vpr04orc.c dated 2006-07-17 05:56:31 used 2008-05-09 01:04:06";
extern char *vpr04orc_force_data () { return( ident_sr ); }
static char ident_mf[] = "@(#)vpr04orc FAST I386 UNIX LINUX PTOC -REL30=1 -RELVER=R76 -LC_STATE=RAMP -COMPILER=UNDEF -MACHINE=UNDEF";
static char ident_cc[] = "@(#)vpr04orc cc -DREL30 -DLINUX -DI386 -DSAG -DSDB_RELEASE_070600 -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/usr/incl -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl -I/home/gunter/SANDBOX/MaxDB/V76_00_32/MaxDB_DEV/sys/wrk/incl/SAPDB -DSYSV -D_SVID -I/usr/include/ncurses -w -Wall -D_FILE_OFFSET_BITS=64 -fPIC -DDYNAHASH -DHAS_NPTL -fno-strict-aliasing -DSAPDB_FAST -D_REENTRANT -O3 -march=pentium -mcpu=pentiumpro -I/home/gunter/SANDBOX/MaxDB/DevTool/incl ";
extern char *vpr04orc_force_whatinfo () { return( ident_cc ); }
