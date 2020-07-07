/*!================================================================
 module:    gpr04.h

 responsible: BurkhardD

 special area:   CPCDrv | Client | Converting Data

 description:

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

#ifndef __GPR04_H__
#define __GPR04_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/
#ifndef sql_oracle
#define sql_oracle
#endif

#ifndef sql_db2
#define sql_db2
#endif

#include "gpr00.h"
#include "gpr01.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/* Appends the oracle hostvartypes. */
#define SQL_ORA_STREAM_HANDLE 301

/*
  Oracle conform data types for using in Oracle SQLDA
*/

enum SQLORATypes {
        SQL_ORA_VARCHAR2             =  1,   /* char[n] */
        SQL_ORA_NUMBER               =  2,
        SQL_ORA_FIXEDPOINTINTEGER    =  3,
        SQL_ORA_FLOATPOINT           =  4,
        SQL_ORA_NULLTERMINATEDSTRING =  5,
        SQL_ORA_VARNUM               =  6,
        SQL_ORA_PACKEDDECIMAL        =  7,
        SQL_ORA_LONG                 =  8,
        SQL_ORA_VARCHAR              =  9,
        SQL_ORA_ROWID                = 11,
        SQL_ORA_INTERNDATE           = 12,
	SQL_ORA_INTERNJULIAN         = 14,
        SQL_ORA_VARRAW               = 15,
	SQL_ORA_RAW                  = 23,
        SQL_ORA_LONGRAW              = 24,
        SQL_ORA_VFILE                = 39,   /* datafile Cobol */
        SQL_ORA_VFILEC               = 40,   /* datafile C */
	SQL_ORA_UNSIGNED             = 68,
	SQL_ORA_DISPLAY              = 91,
	SQL_ORA_LONGVARCHAR          = 94,
        SQL_ORA_LONGVARRAW           = 95,
        SQL_ORA_CHAR                 = 96,
        SQL_ORA_CHARZ                = 97,
	SQL_ORA_MLSLABEL             =106,
	SQL_ORA_LONG_UNICODE         =201,
        SQL_ORA_ABAP_HANDLE          =SQL_ORA_STREAM_HANDLE, /* 301 */
        SQL_ORA_VARCHAR_WITHOUT_LEN  =302,
	SQL_ORA_LONG_DESC            =401,  /* descriptor for long column */
        SQL_ORA_TCHAR                =410,  /* TCHAR if unicode null terminated UCS2 else null terminated char*/
        SQL_ORA_TVARCHAR             =411,  /* TCHAR if unicode -> UCS2[n+2] else -> ora_varchar[n+2]*/
        SQL_ORA_TVARCHAR4            =412,  /* TCHAR if unicode -> UCS2[n+4] else -> ora_varchar4[n+4]*/
	SQL_ORA_UCS2                 =420,  /* null terminated UCS2  string */
	SQL_ORA_UTF16                =421,  /* null terminated UTF16 string */
	SQL_ORA_VARUCS2_2            =422   /* UCS2 string without null termination and length info */
    }; /* ORACLE types */


/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
void p04OraTypes2Internal (int T, tsp00_Int4 L, tsp00_Int2 * vt, tpr01_Precompiledfor PreFor);
void p04oradin (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae,
		sqlcuentry *cu, void *sqlda, struct tpr_sqlloop *lp);
void p04oradout (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae, void *sqlda, struct tpr_sqlloop *lp);
tsp00_Int2 p04oradsfi (sqlcatype *sqlca, sqlxatype *sqlxa, sqlorentry *ore, void *sqlda, char fetchdesc, tpr00_DescribeKindEnum DescribeKind);
void p04din (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae,
sqlcuentry *cu, struct tpr_sqlloop *lp);
void p04dout (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae, struct tpr_sqlloop *lp);
void p04sfi (sqlcatype *sqlca, sqlxatype *sqlxa, tpr00_DescribeKindEnum DescribeKind);
void
p04orat (long L, short T, const char *S, short C, char *name,
    tsp00_Int2 * vt, tsp00_Int4 * vl, tsp00_Int2 * vf, char *err, sqlxatype* sqlxa) ;
void
p04sftoora (struct SQLROW *row, struct SQLCOL *col,
    const tsp1_param_info *sfi, tsp00_Int4 *l, short *t) ;
void p04oracolinit (sqlcatype * sqlca, sqlxatype * sqlxa, void *sqlda);
void p04trline(sqlratype * sqlrap, char*szStr);
#ifdef P04TRENTRIES
void p04trentries(sqlratype * sqlrap, sqlcatype *cap, sqlxatype *xap, char *szStr);
#endif
void p04trint2(sqlratype * sqlrap, char *szStr, tsp00_Int2 cbValue);
void p04trint4(sqlratype * sqlrap, char *szStr, tsp00_Int4 cbValue);

void p04db2din (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae, void *sqlda);
void p04db2dout (sqlcatype *sqlca, sqlgaentry *gae, void *sqlda);
tsp00_Int2 p04db2dsfi (sqlcatype *sqlca, sqlxatype *sqlxa, sqlorentry *ore, void *sqlda, char fetchdesc, char again);
typedef struct sqlvar sqlvar;
void p04sftodb2 (sqlcatype *sqlca, sqlxatype *sqlxa, struct sqlvar *var,
		 struct SQLROW *row, const tsp1_param_info *sfi, char init);
void p04trvinit (const char *vname, int ix, char *line, tsp00_Int2 *ll,
                int inp);
void p04trloop (sqlratype *sqlrap, int lpind);
void p04ptrvwrite (sqlratype *sqlrap,
                const struct SQLCOL *col, char *buf, tsp00_Int4 pos);
tsp00_Int2 *p04swap2 (tsp00_Int2 *dbuf, tsp00_Int2 *sbuf, tsp00_Int4 len);
void p04init (struct SQLROW *row);
tsp00_Int2 p04pdecode (const char *buf, tsp00_Int2 pos,
                        struct SQLROW *row, struct SQLCOL *col);
tsp00_Int2 p04decode (const tsp1_param_info *sfi,
                        struct SQLROW *row, struct SQLCOL *col);

void pr04ColNames2ore(sqlcatype *sqlca, sqlxatype *sqlxa,sqlkaentry *kae, sqlorentry *ore);
void p04GetSFInfo(sqlcatype *sqlca, sqlxatype *sqlxa, sqlkaentry *kae, sqlorentry *ore);
void p04SFInfo2oreRes(sqlcatype *sqlca, sqlxatype *sqlxa, sqlkaentry *kae, sqlorentry *ore);


void p04mode (sqlcatype *sqlca, sqlgaentry *gae);
void p04ocimode (sqlratype *sqlrap, sqlgatype *sqlgap, sqlgaentry *gae);
void p04coltobuf (sqlratype *sqlrap, char *databuf, tsp00_Int4 *buflen,
                struct SQLCOL *col, const char *vn, const void *va,
                tsp00_Int2 vt, tsp00_Int4 vl, tsp00_Int2 vd, tsp00_Int2 vf,
		tsp00_Int4 ival, tsp00_Int4 offs, int pno,
                struct SQLERROR *sqlemp);
void p04colfrombuf (sqlratype *sqlrap, char *databuf, tsp00_Int4 *buflen,
                struct SQLCOL *col, const char *vn, void *va,
                tsp00_Int2 vt, tsp00_Int4 vl, tsp00_Int2 vd, tsp00_Int2 vf,
		tsp00_Int4 *ival, int offs, int pno,
                struct SQLERROR *sqlemp);
void p04coltoindi (const sqlcatype *sqlca, sqlxatype *sqlxa,
                struct SQLCOL *col, sqlv1pointer va1, char *base,
		tsp00_Int4 ival, char *err);
int p04col1toindi (struct SQLCOL *col, tsp00_Int4 ival, void *va,
                        tsp00_Int2 vt, tsp00_Int2 vl, tsp00_Int2 vf);
void p04colfromindi (const sqlcatype *sqlca, sqlxatype *sqlxa,
                struct SQLCOL *col, sqlv1pointer va1, char *base,
		tsp00_Int4 *ival, char *err);
void p04err (sqlratype *sqlrap, struct SQLERROR *sqlemp, char err);
void p04invaliderr (sqlratype * sqlrap, const char *buf, struct SQLCOL *col,
	tsp00_Int2 vt, tsp00_Int4 vl, tsp00_Int2 vf, tsp00_Int2 inout);
tsp00_Int4 p04rescount (tsp1_segment *seg, tsp00_Int4 datalen, int *countlen);
void p04input (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae, struct tpr_sqlloop *lp);
void p04output (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae, struct tpr_sqlloop *lp);
void p04sqldin (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae, void *sqlda,
                struct tpr_sqlloop *lp);
void p04sqldout (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae, void *sqlda, struct tpr_sqlloop *lp);
tsp00_Int2 p04sqldsfi (sqlcatype *sqlca, sqlxatype *sqlxa, sqlorentry *ore, void *sqlda, char fetchdesc, char again, tpr00_DescribeKindEnum DescribeKind);
tsp00_Int2 p04or2sqlda (sqlcatype * sqlca, sqlxatype * sqlxa, sqlorentry *ore, tpr00_DescribeKindEnum DescribeKind);
void p04or2da (sqlcatype * sqlca, sqlxatype * sqlxa, sqlorentry *ore, tpr00_DescribeKindEnum DescribeKind);
void p04ofltchr (char *va, tsp00_Int4 vl, tsp00_Int4 *cl,
                 char *rb, tsp00_Int4 rbl, char *res);
#ifdef sql_sqldb
void p04sftosql (sqlvartype *var, struct SQLROW *row,
    const tsp1_param_info *sfi, char init);
#endif

void p04traceda (sqlratype * sqlrap, char *daIn, struct SQLCOL  *col, tsp00_Int2 sqldbmode);

BOOLEAN p04IsEmptyString(char *va, tsp00_Int2 coltype, tsp00_Int2 vt, tsp00_Int4 vl);
BOOLEAN p04IsBinaryType(tsp00_Int2 coltype);
tsp00_Int4 pr04GetLoopCnt(sqlcatype *sqlca, sqlxatype *sqlxa, struct tpr_sqlloop *lp);
#ifdef __cplusplus
}
#endif
#endif
