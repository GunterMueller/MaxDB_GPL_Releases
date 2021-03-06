/* begin of second part of cpc.h */
#ifndef _CPC_H_2
#define _CPC_H_2 0

 /* ----------------------------------------------- */
 /*       CPC run time functions references         */
 /* ----------------------------------------------- */

#ifdef __OS2__
#define _CPC_FMOD _System
#else
#define _CPC_FMOD
#endif

#if defined(__STDC__) || defined(__cplusplus)
#ifdef sql_oracle
#include <stdlib.h>
#endif
#ifdef __cplusplus
extern "C" {
#endif
void
_CPC_FMOD sqccatb (sqlatpointer, long, long, long, char*, char*),
_CPC_FMOD sqcccan (sqlcatype *sqlca, sqlxatype *sqlxa, long stno),
_CPC_FMOD sqccver (sqlcatype *sqlca, sqlxatype *sqlxa, long stno),
_CPC_FMOD sqccchk(sqlcatype*, sqlxatype*, sqlint2*, sqlint2*, long, long),
_CPC_FMOD sqls (sqlva1en*, long, long, long, long, void*),
_CPC_FMOD sqlr (sqlva1en*, long, long, void*),
_CPC_FMOD sqccv2b(sqlva2en [], long, long, long, long, long,
	long),
_CPC_FMOD sqccv3b(sqlva3en [], long, long, char*),
_CPC_FMOD sqcccab(sqlcatype*, long, long, long),
_CPC_FMOD sqccpab(sqlparpointer, long, long, long, long, long),
_CPC_FMOD sqcckab(sqlorpointer, sqlkapointer, long, long, long,
	long, long, long, long, long, long, long, long, long, long, long,
	long),
_CPC_FMOD sqccprb(sqlprpointer, long, long, long, long, long, long, long,
	long, long, long, long, long, char*, char*),
_CPC_FMOD sqccstb(sqlstpointer, long, long, char*),
_CPC_FMOD sqccxab(sqlxatype*, long, long, long, char*, char*),
_CPC_FMOD sqccsdb(void *, long, long, long),
_CPC_FMOD sqccfnb(sqlfnpointer, long, long, long, long, char*),
_CPC_FMOD sqccmab(sqlcatype*, sqlxatype*, long, long, char*),
_CPC_FMOD sqccgab(sqlcatype*, long, long, long, char*, char*),
_CPC_FMOD sqcctrs(sqlcatype*, long),
_CPC_FMOD sqccmts(sqlcatype*, long, char*),
_CPC_FMOD sqcccmd(sqlcatype*, sqlxatype*, long),
_CPC_FMOD sqcctrl(sqlcatype*, sqlxatype*, long),
#if defined(sql_sqldb) || defined (sql_default) || defined (sql_ansi)
_CPC_FMOD sqccdam(sqlcatype*, sqldatype*),
_CPC_FMOD sqccdaa(sqlcatype*, sqldatype*),
#else
#ifdef sql_oracle
_CPC_FMOD sqccdaa(sqlcatype*, struct SQLDA*),
#endif
#endif
#if defined(sql_db2)
_CPC_FMOD sqccdaa(sqlcatype*, struct sqlda*),
#endif
_CPC_FMOD sqccrcn(sqlcatype*, long),
_CPC_FMOD sqccstp(sqlcatype*, sqlxatype*),
_CPC_FMOD sqccexi(sqlcatype*, sqlxatype*),
_CPC_FMOD sqccos(long*);

void *sqlcaddr (SQLAREAS, void *);
void *sqlcdynp (SQLAREAS, void *, int );
int   sqlcsize (SQLAREAS);
void  sqlcisol (sqlcatype*, long);

char *sqlCPCGetDriverName ();
void sqlCPCExecute(sqlcatype*, sqlxatype*, long, long);
void sqlCPCLocalBind (sqlcatype*, sqlxatype*, long, long, long, long, char*);
void sqlCPCCheck  (sqlcatype*, sqlxatype*, sqlint2 *, sqlint2 *, long, long);
void sqlCPCCheckU (sqlcatype*, sqlxatype*, sqlint2 *, sqlint2 *, long, long);
void  sqlCPCDeclare (sqlcatype *, sqlxatype *, long, long);
void  sqlCPCEndInit (sqlcatype*, sqlxatype*, char *);
void sqlCPCPrBind(sqlprpointer, long, long, long, long, long, long, long, long, long, long, long, long, char *, char *);
void sqlCPCTryExecute(sqlcatype*, sqlxatype*, long, long);
void sqlCPCStBind (sqlcatype *sqlca, sqlxatype *sqlxa, long stno, long fOption, long sess, long stlen, char *stval);
void sqlCPCPrepare(sqlcatype*, sqlxatype*, long, long);
void sqlCPCTraceLine(sqlcatype*, sqlxatype*, long);
void sqlCPCOption(sqlcatype*, sqlxatype*, long);
void sqlCPCPutDriverName(char *szDriverName);
void sqlCPCOpen(sqlcatype*, sqlxatype*, long, long);
void sqlCPCExecDirect(sqlcatype*, sqlxatype*, long, long);
void sqlCPCDescribe(sqlcatype*, sqlxatype*, long, long);
void sqlCPCDeclare (sqlcatype *, sqlxatype *, long, long);
void sqlCPCGaBind(sqlcatype*, sqlxatype *, long, long, long, char*, char*);
void sqlCPCNoLog(sqlcatype*, sqlxatype*, long);
#ifdef sql_oracle
extern struct SQLDA *sqlald (int , size_t, size_t);
extern void sqlclu (struct SQLDA*);
extern void sqlprc (long*, int*, int*);
extern void sqlnul (unsigned short*, unsigned short*, int*);
extern void sqlblnk (struct SQLDA*, int);
extern void sqlglm (unsigned char*, size_t*, size_t*);
#endif


#ifdef __cplusplus
}
#endif
static void
#ifdef sqlextern
sqlstart (sqlcatype*);
#else
sqlstart (void);
#endif
#else                 /* non ANSI C compilers */
extern void
_CPC_FMOD sqccchk(),
_CPC_FMOD sqlCPCDescribe(),
_CPC_FMOD sqlCPCOpen(),
_CPC_FMOD sqlCPCExecDirect(),
_CPC_FMOD sqccexi(),
_CPC_FMOD sqccgab(),
_CPC_FMOD sqlCPCGaBind(),
_CPC_FMOD sqcckab(),
_CPC_FMOD sqccmab(),

_CPC_FMOD sqccpab(),
_CPC_FMOD sqccprb(),
_CPC_FMOD sqlCPCPrepare(),
_CPC_FMOD sqls   (),
_CPC_FMOD sqlr   (),
_CPC_FMOD sqccstb(),
_CPC_FMOD sqlCPCTryExecute(),
_CPC_FMOD sqlCPCStBind(),
_CPC_FMOD sqcctrl(),
_CPC_FMOD sqlCPCTraceLine(),
_CPC_FMOD sqlCPCOption(),
_CPC_FMOD sqlCPCPutDriverName(),
_CPC_FMOD sqcctrs(),
_CPC_FMOD sqccv2b(),
_CPC_FMOD sqccv3b(),
_CPC_FMOD sqccxab(),
_CPC_FMOD sqlCPCExecute(),
_CPC_FMOD sqlCPCLocalBind(),
_CPC_FMOD sqlCPCCheck(),
_CPC_FMOD sqlCPCNoLog(),
_CPC_FMOD sqccrcn(),
_CPC_FMOD sqccsdb(),
_CPC_FMOD sqccmts(),
_CPC_FMOD sqccos (),
_CPC_FMOD sqccfna(),
_CPC_FMOD sqccfnb(),
#ifdef sql_oracle
_CPC_FMOD sqccdaa(),
#else
_CPC_FMOD sqccdaa(),
_CPC_FMOD sqccdam(),
#endif
_CPC_FMOD sqccatb(),
_CPC_FMOD sqcccan(),
_CPC_FMOD sqccstp(),
_CPC_FMOD sqcccab(),
_CPC_FMOD sqccver(),
_CPC_FMOD sqcccmd();
char *sqlCPCGetDriverName ();

#ifdef sql_oracle
extern struct SQLDA *sqlald ();
extern void sqlclu ();
extern void sqlprc ();
extern void sqlnul ();
extern void sqlblnk ();
extern void sqlglm ();
#endif

extern void _CPC_FMOD *sqlcaddr ();
extern void _CPC_FMOD *sqlcdynp ();
extern int _CPC_FMOD  sqlcsize ();
extern void _CPC_FMOD sqlcisol ();
extern void _CPC_FMOD sqlCPCDeclare ();
extern void _CPC_FMOD sqlCPCEndInit ();
extern void _CPC_FMOD sqlCPCPrBind();
extern void _CPC_FMOD sqlCPCStBind ();

static void sqlstart();
#endif

#endif
/* end of second part of cpc.h */
