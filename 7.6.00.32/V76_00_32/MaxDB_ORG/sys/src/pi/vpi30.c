/*!================================================================
 module:    vpi30.c

 responsible: BurkhardD
 
 special area:   CPCDrv | Client | CPC Entries
 
 description: CPC Entries

 change history: 

    2001-09-04 d025086 No implicite connect. 
    2000-11-16 d025086 (PTS 1108288) sqccver calls Precom->Version  
    2000-08-07  17:17 Marco Paskamp  Unicode Erweiterungen

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

#include <stddef.h>
#include <string.h>
#include <memory.h>
#include "gpr01.h"
#include "gpr04.h"
#include "vpr01Env.h"
#include "vpr01Precom.h"
#include "vpr01Module.h"
#include "vpr01Con.h"
#include "vpr01SQL.h"
#include "vpr01EXECSQL.h"
#include "vpr06ParseId.h"
#include "vpi10ConnSync.h"
#include "vpr07c.h"
#include "gpr03.h"
#include "gpr03m.h"
#include "gpr08.h"
#include "geo31.h"

#ifdef SQLXENUMA
#undef SQLXENUMA
#define SQLXENUMA(VarName, MemberName, VarType, MemberType ) \
SQLENUMA(VarName, MemberName), \
sizeof(MemberType), \
offsetof(VarType, MemberName)
#endif

typedef struct {
  SQLAREAS cbEnum;
  int cbSize;
  int cbOffsetOf;
} pi30SQLAreasStruct;

typedef struct sqlunused {
  void *sqlunused1;
  void *sqlunused2;
  void *sqlunused3;
  void *sqlunused4;
} sqlunused;

/* this includes the enum declaration so that the var declaration is in the */
/* same sequenze as the enum declaration */
static pi30SQLAreasStruct pi30SQLAreas[]={
#include "vpi00c-d.h"
};

tpr01_ModuleContainer *sqlModuleCont=NULL;
extern tpr01_EnvContainer *sqlEnvCont;
tpr_pcVersion sqlPCVersion;
extern tpr_pcVersion *p07pcversion;
char sqlPCRelStr[200];
void sqlCPCEndInit (sqlcatype *sqlca, sqlxatype *sqlxa, char *szPCVersion )
{
  M90TRACE(M90_TR_ENTRY, "sqlCPCEndInit", 0);
  if (!p07pcversion) {
    char *c;
    sqlPCRelStr[0] = '\0';
    sqlPCVersion.cbBuild = 0;
    for(c=szPCVersion; *c != '\0'; c++) {
      if(*c >= '0' && *c <= '9') {
        char *p = sqlPCRelStr;
        for(;*c != '\0'; c++,p++) {
          *p = *c;
          if (*c == ' ') {
            *p='\0';
            break;
          }
        }
        for(;*(c+6) != '\0'; c++) {
#ifdef WIN32
#define STRNICMP strnicmp
#else
#define STRNICMP strncasecmp
#endif
          if (!STRNICMP(c, "BUILD ", 6)) {
            int x =
            sscanf(c+6, "%d-", &sqlPCVersion.cbBuild);
            if (x != 1) {
              sqlPCVersion.cbBuild = 0;
            }
            break;
          }
        }        
        break;
      }
    }
    sqlPCVersion.szVersion = sqlPCRelStr;    
    if (sqlPCRelStr[0] != '\0') {
      p07pcversion = &sqlPCVersion;
    }    
  }
  M90TRACE(M90_TR_EXIT, "sqlCPCEndInit", 0);
}

static void pi30Check (sqlcatype *sqlca, sqlxatype *sqlxa, sqlint2 *sqlstci, 
                      sqlint2 *sqlexti, long sqlext, long connplen, tpr01_Precompiledfor pcfor)
{
  M90TRACE(M90_TR_ENTRY, "sqlCPCCheck", 0);
  {
    static tsp00_Int4 debugSwitch = 1;
    PR07_HEAPCHECK(NULL);
    if (debugSwitch) {
      sqlSetDebug("sapdbprof.ini", &debugSwitch );
      sqlDebugBreak();
      debugSwitch = 0;
    }
  }
  if (sqlext == 0)
    *sqlexti = 1;
  
  sqlxa->xalang = CPR_LA_C;
  if (*sqlstci <= 0) {
    if (*sqlstci == 0) {
      memset(sqlxa, 0, sizeof(sqlxatype));
      sqlxa->xalang = CPR_LA_C;
      sqlxa->xalcheck [0] = ' ';
      if (*sqlexti == 0) {
	*sqlexti = 1;
	memset(sqlca, 0, sizeof(sqlcatype));
	p03caci (sqlca, sqlxa, pcfor);
	sqlca->sqlext [0] = ' ';
      }
    }
    *sqlstci = 1;
    if (sqlca->sqlxadesc.descElemPtr == NULL) {
      sqlca->sqlxadesc.descElemPtr = pr03mAllocat(sizeof(void*), "descElemPtr");
      if (!sqlca->sqlxadesc.descElemPtr)
	sqlabort ();
    }
    *((sqlint2 **) p03dynalloc (&sqlca->sqlxadesc)) = sqlstci;
    *((sqlxatype **) p03dynalloc (&sqlca->sqlxadesc)) = sqlxa;
  }
  if (!sqlca->sqlEnvDesc) {
    if (!sqlEnvCont) {
      sqlEnvCont = pr01EnvNewCont();
    }
    sqlca->sqlEnvDesc = sqlEnvCont->AddDesc(sqlEnvCont, sqlca);
  }
  if (!sqlxa->xaModDesc) {
    tpr01_ModuleContainer *Module = sqlca->sqlEnvDesc->Module;
    sqlxa->xaModDesc = Module->AddDesc(Module, sqlxa);
    sqlxa->xaModDesc->PrecompiledforUnicode = pcfor;
  }
  p01check (sqlca, sqlxa, (int) connplen);
  M90TRACE(M90_TR_EXIT, "sqlCPCCheck", 0);
}

void sqlCPCCheck (sqlcatype *sqlca, sqlxatype *sqlxa,
		  sqlint2 *sqlstci, sqlint2 *sqlexti, long sqlext, long connplen)
{
  pi30Check (sqlca, sqlxa, sqlstci, sqlexti, sqlext, connplen, PrecompiledForAscii_epr01);
}

void sqlCPCCheckU (sqlcatype *sqlca, sqlxatype *sqlxa,
		   sqlint2 *sqlstci, sqlint2 *sqlexti, long sqlext, long connplen)
{
  pi30Check (sqlca, sqlxa, sqlstci, sqlexti, sqlext, connplen, PrecompiledForUnicode_epr01);
}

void sqccchk (sqlcatype *sqlca, sqlxatype *sqlxa,
	sqlint2 *sqlstci, sqlint2 *sqlexti, long sqlext, long connplen)
{
  sqlCPCCheck (sqlca, sqlxa, sqlstci, sqlexti, sqlext, connplen);
}

/* sqlCPCStructBind */
void sqls (sqlva1en *sqlva1, long va1ix, long va1ix1, long cnt,
	long size, void *addr)
{
  M90TRACE(M90_TR_ENTRY, "sqls", 0);
  {
    sqlva1en *va = sqlva1 + va1ix - 1;
    va->st.va1indi = SQLVAST;
    va->st.va1ix = (sqlint4) va1ix1;
    va->st.va1cmpcnt = (sqlint2) cnt;
    va->st.va1size = (sqlint4) size;
    va->st.va1addr = addr;
  }
  M90TRACE(M90_TR_EXIT, "sqls", 0);
}

/* sqlCPCPtrBind */
void sqlr (sqlva1en *sqlva1, long va1ix, long va1ix1, void *addr)
{
  M90TRACE(M90_TR_ENTRY, "sqlr", 0);
  {
    sqlva1en *va = sqlva1 + va1ix - 1;
    va->pt.va1indi = SQLVAPT;
    va->pt.va1ix = (sqlint4) va1ix1;
    va->pt.va1addr = addr;
  }
  M90TRACE(M90_TR_EXIT, "sqlr", 0);
}

/* sqlCPCV2Bind */
void sqccv2b (sqlva2en *sqlva2, long va2no, long va2typ, long va2size,
	long va2digit, long va2frac, long va2const)
{
  M90TRACE(M90_TR_ENTRY, "sqccv2b", 0);
  {
    sqlva2en *va = sqlva2 + va2no;
    va->va2typ = (sqlint2) va2typ;
    if (va->va2typ == cpr_vucs2) {
      va->va2size = (sqlint4) (va2size * sizeof (SQLUCS2));
    } else {
      va->va2size = (sqlint4) va2size;
    }
    va->va2digit = (sqlint2) va2digit;
    va->va2frac = (sqlint2) va2frac;
    va->va2const = (sqlint2) va2const;
  }
  M90TRACE(M90_TR_EXIT, "sqccv2b", 0);
}

/* sqlCPCV3Bind */
void sqccv3b (sqlva3en *sqlva3, long va3no, long va3naml, char *va3name)
{
  M90TRACE(M90_TR_ENTRY, "sqccv3b", 0);
  {
    sqlva3en *va = sqlva3 + va3no;
    long d = sizeof (va->va3name) - va3naml;
    memcpy (va->va3name, va3name, va3naml);
    if (d > 0)
      memset (&va->va3name [va3naml], ' ', d);
    va->va3naml = (sqlint2) va3naml;
  }
  M90TRACE(M90_TR_EXIT, "sqccv3b", 0);
}

/* sqlCPCCaBind */
void sqcccab (sqlcatype *sqlca, long dbkind, long datetime, long ansi)
{
  M90TRACE(M90_TR_ENTRY, "sqcccab", 0);
  if (sqlca->sqldbmode == CPR_KIND_EMPTY) {
    sqlca->sqldbmode = (sqlint2) dbkind;
    sqlca->sqlrap->rasqlansi = (sqlint2) ansi;
  }
  sqlca->sqlrap->radbmode = sqlca->sqldbmode;
  if (sqlca->sqldatetime == CPR_DT_EMPTY)
    sqlca->sqldatetime = (sqlint2) datetime;
  M90TRACE(M90_TR_EXIT, "sqcccab", 0);
}

/* sqlCPCPaBind */
void sqccpab (sqlparentry *sqlpa, long pano, long pakind, long paparm2,
	long paparm3, long paparm4)
{
  M90TRACE(M90_TR_ENTRY, "sqccpab", 0);
  {
    sqlparentry *pa = sqlpa + pano;
    pa->el.pakind = (sqlint2) pakind;
    switch (pakind) {
    case SQLPARLO:
      pa->lo.pava1index = (sqlint2) paparm2;
      pa->lo.paloopcnt = (sqlint4) paparm3;
      pa->lo.paloopmin = (sqlint4) paparm4;
      break;
    case SQLPARST:
      pa->st.pavarst = (sqlint2) paparm2;
      pa->st.paindst = (sqlint2) paparm3;
      pa->st.paelcnt = (sqlint2) paparm4;
      break;
    case SQLPAREL:
      pa->el.pavarno = (sqlint2) paparm2;
      pa->el.paindno = (sqlint2) paparm3;
      break;
    }
  }
  M90TRACE(M90_TR_EXIT, "sqccpab", 0);
}

/* sqlCPCKaBind */
void sqcckab (sqlorentry *sqlor, sqlkaentry *sqlka, long kano,
	long katype, long kastate, long kacount, long kaindex,
	long kastcount, long kastindex, long kamacro,
	long kaprindex, long kadiindex, long kalineno,
	long kafaindex, long kaatindex, long kacuindex, long kamode)
{
  M90TRACE(M90_TR_ENTRY, "sqcckab", 0);
  M90TRACE(M90_TR_DWORD, "kano", &kano);
  M90TRACE(M90_TR_DWORD, "katype", &katype);
  M90TRACE(M90_TR_DWORD, "kalineno", &kalineno);
  M90TRACE(M90_TR_DWORD, "kaprindex", &kaprindex);
  {
    sqlorentry *or = sqlor + kano;
    sqlkaentry *ka = sqlka + kano;
    pr03cOrInit(or);
    ka->katyp = (sqlint2) katype;
    ka->kastate = (sqlint2) kastate;
    ka->kapacount = (sqlint2) kacount;
    ka->kapaindex = (sqlint2) kaindex;
    ka->kaStcount = (sqlint2) kastcount;
    ka->kaStindex = (sqlint2) kastindex;
    ka->kamacro = (sqlint2) kamacro;
    ka->kaprindex = (sqlint2) kaprindex;
    ka->kadiindex = (sqlint2) kadiindex;
    ka->kalineno = (sqlint2) kalineno;
    ka->kafaindex = (sqlint2) kafaindex;
    ka->kaatindex = (sqlint2) kaatindex;
    ka->kacuindex = (sqlint2) kacuindex;
    ka->kamode = (sqlint2) kamode;
    ka->kaparserrc = 0;
    ka->kaversion = 0;
    ka->kaparamcnt = 0;
    pr06ParseIdCopy(&ka->kaParseInfo.ParseId, ParseIdNull_cpr00);
    ka->kasqlrow.ireclen = 0;
    ka->kasqlrow.oreclen = 0;
  }
  M90TRACE(M90_TR_EXIT, "sqcckab", 0);
}

/* sqlCPCPrBind */
void sqlCPCPrBind(sqlprentry *sqlpr, long prno, long prStmtNameIndex, 
		  long prCursorNameIndex, long prstate, long prkaindex, 
		  long prarea, long prDescribe, long prstcount, long prstindex,
		  long prcuindex, long prnaml, long prcunaml, 
		  char *prStmtName, char *prCursorName)
{
  M90TRACE(M90_TR_ENTRY, "sqlCPCPrBind", 0);
  M90TRACE(M90_TR_DWORD, "prno", &prno);
  M90TRACE(M90_TR_DWORD, "prStmtNameIndex", &prStmtNameIndex);
  M90TRACE(M90_TR_DWORD, "prCursorNameIndex", &prCursorNameIndex);
  M90TRACE(M90_TR_DWORD, "prstate", &prstate);
  M90TRACE(M90_TR_DWORD, "prkaindex", &prkaindex);
  M90TRACE(M90_TR_DWORD, "prarea", &prarea);
  M90TRACE(M90_TR_DWORD, "prDescribe", &prDescribe);
  M90TRACE(M90_TR_DWORD, "prstcount", &prstcount);
  M90TRACE(M90_TR_DWORD, "prstindex", &prstindex);
  M90TRACE(M90_TR_DWORD, "prcuindex", &prcuindex);
  M90TRACELEN(M90_TR_STRING, "prCursorName", prCursorName, prcunaml);
  M90TRACELEN(M90_TR_STRING, "prStmtName", prStmtName, prnaml);
  {    
    sqlprentry *pr = sqlpr + prno;
    long d1 = sizeof (pr->prStmtName) - prnaml;
    long d2 = sizeof (pr->prCursorName) - prcunaml;
    pr->prStmtNameIndex = (sqlint2) prStmtNameIndex;
    pr->prCursorNameIndex = (sqlint2) prCursorNameIndex;
    pr->prstate = (sqlint2) prstate;
    pr->prkaindex = (sqlint2) prkaindex;
    pr->prarea = (sqlint2) prarea;
    pr->prstcount = (sqlint2) prstcount;
    pr->prstindex = (sqlint2) prstindex;
    pr->prcuindex = (sqlint2) prcuindex;
    pr->prDescribe = (sqlint2) prDescribe;
    memcpy (pr->prStmtName, prStmtName, prnaml);
    memcpy (pr->prCursorName, prCursorName, prcunaml);
    if (d1 > 0)
      memset (&pr->prStmtName [prnaml], ' ', d1);
    if (d2 > 0)
      memset (&pr->prCursorName [prcunaml], ' ', d2);
    PR07_HEAPCHECK(NULL);
  }
  M90TRACE(M90_TR_EXIT, "sqlCPCPrBind", 0);
}

/* sqlCPCStBind */
void sqlCPCStBind (sqlcatype *sqlca, sqlxatype *sqlxa, long kano, long fOption, long sess, long stlen, char *stval)
{
  M90TRACE(M90_TR_ENTRY, "sqlCPCStBind", 0);
  M90TRACE(M90_TR_DWORD, "kano", &kano);
  M90TRACE(M90_TR_DWORD, "sess", &sess);
  M90TRACELEN(M90_TR_STRING, "stval", stval, stlen);
  {
    tpr01_ModuleDesc *ModuleDesc = sqlxa->xaModDesc;
    tpr01_ModuleContainer *Module = ModuleDesc->Module;
    tpr05_String SQLStatement;
    SQLStatement.allocationType = constant_epr05;
    pr05IfCom_String_InitString(&SQLStatement,
                                stval, 
                                stlen, 
                                sp77encodingAscii, 
                                constant_epr05);
    Module->StatementBind(ModuleDesc, &SQLStatement ,kano, fOption);
          
    PR07_HEAPCHECK(NULL);
  }
  M90TRACE(M90_TR_EXIT, "sqlCPCStBind", 0);
}

/* sqlCPCLocalBind */
void sqlCPCLocalBind (sqlcatype *sqlca, sqlxatype *sqlxa, long kano, long fOption, long sess, long stlen, char *stval)
{
  M90TRACE(M90_TR_ENTRY, "sqlCPCLocalBind", 0);
  M90TRACE(M90_TR_DWORD, "kano", &kano);
  M90TRACE(M90_TR_DWORD, "sess", &sess);
  M90TRACELEN(M90_TR_STRING, "stval", stval, stlen);
  if (kano > 0) {
    tpr01_ModuleDesc *ModuleDesc = sqlxa->xaModDesc;
    tpr01_ModuleContainer *Module = ModuleDesc->Module;
    tpr01_PrecomDesc *PrecomDesc = Module->GetPrecomDesc(ModuleDesc, sqlxa, sqlxa->xakano);
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    tpr01_ConDesc *ConDesc = Precom->GetConnection(PrecomDesc, ModuleDesc, sess);
    tpr05_String SQLStatement;
    SQLStatement.allocationType = constant_epr05;
    pr05IfCom_String_InitString(&SQLStatement,
                                stval, 
                                stlen, 
                                sp77encodingAscii, 
                                constant_epr05);
    Precom->StatementBind(PrecomDesc, &SQLStatement, fOption, ModuleDesc, ConDesc);      
  }
  M90TRACE(M90_TR_EXIT, "sqlCPCLocalBind", 0);
}

/* sqlCPCXaBind */
void sqccxab (sqlxatype *sqlxa, long xlang, long progc, long modc,
	char *progn, char *modn)
{
  M90TRACE(M90_TR_ENTRY, "sqccxab", 0);
  M90TRACE(M90_TR_PTR, "sqlxa", &sqlxa);
  M90TRACELEN(M90_TR_STRING, "progn", progn, progc);
  M90TRACELEN(M90_TR_STRING, "modn", modn, modc);
  {
    long d1 = sizeof (sqlxa->xaprogn) - progc;
    long d2 = sizeof (sqlxa->xamodn) - modc;
    sqlxa->xalang = (sqlint2) xlang;
    sqlxa->xaprogc = (sqlint2) progc;
    sqlxa->xamodc = (sqlint2) modc;
    memcpy (sqlxa->xaprogn, progn, progc);
    memcpy (sqlxa->xamodn, modn, modc);
    if (d1 > 0)
      memset (&sqlxa->xaprogn [progc], ' ', d1);
    if (d2 > 0)
      memset (&sqlxa->xamodn [modc], ' ', d2);
  }
  M90TRACE(M90_TR_EXIT, "sqccxab", 0);
}

/* sqlCPCSdBind */
void sqccsdb (void *sqlsd, long sdno, long sd1cnt, long sd1ix)
{
  /* never used anymore */
}

/* sqlCPCMaBind */
void sqccmab (sqlcatype *sqlca, sqlxatype *sqlxa, long mano, long malen,
	char *maval)
{
  M90TRACE(M90_TR_ENTRY, "sqccmab", 0);
  p01ma2bi (sqlca, sqlxa, (sqlint2) mano, (sqlint2) malen, maval);
  M90TRACE(M90_TR_EXIT, "sqccmab", 0);
}


static void pi30GaBind(sqlxuserrec *gaxu, long dbnl, long nodenl,
		       char *dbn, char *node)
{
  long len, d;
  char *c;
  if (dbnl > sizeof (tsp00_DbName))
    dbnl = sizeof (tsp00_DbName);
  c = memchr (dbn, 0, sizeof (tsp00_DbName));
  len = (long) ((c) ? c - dbn :  sizeof (tsp00_DbName));
  if (len > dbnl && dbnl > 0)
    len = dbnl;
  d = sizeof (tsp00_DbName) - len;
  memcpy (gaxu->xu_serverdb, dbn, len);
  if (d > 0)
    memset (&gaxu->xu_serverdb [len], ' ', d);
  if (nodenl > sizeof (tsp00_NodeId))
    nodenl = sizeof (tsp00_NodeId);
  c = memchr (node, 0, sizeof (tsp00_NodeId));
  len = (long) ((c) ? c - node : (long) sizeof (tsp00_NodeId));
  if (len > nodenl && nodenl > 0)
    len = nodenl;
  d = sizeof (tsp00_NodeId) - len;
  memcpy (gaxu->xu_servernode, node, len);
  if (d > 0)
    memset (&gaxu->xu_servernode [len], ' ', d);  
}

sqlCPCGaBind(sqlcatype *sqlca, sqlxatype *sqlxa, long nSession, 
	     long dbnl, long nodenl,
	     char *dbn, char *node)
{
  M90TRACE(M90_TR_ENTRY, "sqlCPCGaBind", 0);
  {
    tpr01_EnvDesc *EnvDesc = sqlca->sqlEnvDesc;
    tpr01_EnvContainer *Env = EnvDesc->Env;
    tpr00_DatabaseName szDatabaseName;
    tpr01_ConContainer *Connection = Env->GetConnectionCont(EnvDesc);
    tpr01_ConDesc *ConDesc;
    sqlgaentry *gae;
    sqlxuserrec *gaxu;
    epr01Con_SessionType SessionType = (nSession == 0) ? 
      SessionTemp_epr01Con : SessionDefault_epr01Con;

    sprintf(szDatabaseName, ":%d", nSession);
    if (Connection->OldStyle && nSession == 1) 
      SessionType = SessionPrimary_epr01Con;
      
    ConDesc = Connection->FindDesc(Connection, szDatabaseName, SessionType);
    if (!ConDesc)
      ConDesc = Connection->AddDesc(Connection, szDatabaseName, sqlxa, SessionType);
    gae = ConDesc->ga;
    gaxu = &gae->gaxuserrec;
    pi30GaBind(gaxu, dbnl, nodenl, dbn, node);
  }
  M90TRACE(M90_TR_EXIT, "sqlCPCGaBind", 0);
}

void sqccgab (sqlcatype *sqlca, long index, long dbnl, long nodenl,
	char *dbn, char *node)
{
  M90TRACE(M90_TR_ENTRY, "sqccgab", 0);
  {
    sqlxuserrec *gaxu = &sqlca->sqlgap->gaentry [index - 1]->gaxuserrec;
    pi30GaBind(gaxu, dbnl, nodenl, dbn, node);
  }
  M90TRACE(M90_TR_EXIT, "sqccgab", 0);
}

/* sqlCPCFnBind */
void sqccfnb (sqlfnentry *sqlfn, long fnno, long kanomin, long kanomax,
	long incllno, char *filen)
{
  M90TRACE(M90_TR_ENTRY, "sqccfnb", 0);
  {
    sqlfnpointer fn = sqlfn + fnno;
    int l = (int) strlen (filen);
    fn->fnkanomin = (sqlint2) kanomin;
    fn->fnkanomax = (sqlint2) kanomax;
    fn->fnincllno = (sqlint2) incllno;
    if (l > sizeof (fn->fnfilen))
      l = sizeof (fn->fnfilen);
    memset (fn->fnfilen, ' ', sizeof (fn->fnfilen));
    memcpy (fn->fnfilen, filen, l);
  }
  M90TRACE(M90_TR_EXIT, "sqccfnb", 0);
}

/* sqlCPCTrace */
void sqcctrs (sqlcatype *sqlca, long trcty)
{
  M90TRACE(M90_TR_DWORD, "sqcctrs", &trcty);
  sqlca->sqloap->oamodsettrty = (sqlint2) trcty;
}

/* sqlCPCTraceFile */
void sqccmts (sqlcatype *sqlca, long mtrcty, char *mtrcfn)
{
  M90TRACE(M90_TR_ENTRY, "sqccmts", 0);
  {
    long len, d;
    char *s;
    sqloatype *oa = sqlca->sqloap;
    oa->oamodtrace = sqlIsTrue;
    oa->oamodtracety = (sqlint2) mtrcty;
    s = memchr (mtrcfn, 0, sizeof (sqlhostname_t));
    len = (long) ((s) ? s - mtrcfn : (int) sizeof (sqlhostname_t));
    d = sizeof (sqlhostname_t) - len;
    memcpy (oa->oamodtracefn, mtrcfn, len);
    if (d > 0)
      memset (&oa->oamodtracefn [len], ' ', d);
  }
  M90TRACE(M90_TR_EXIT, "sqccmts", 0);
}

void sqlCPCOpen (sqlcatype *sqlca, sqlxatype *sqlxa, long stno, long sess)
{
  M90TRACE(M90_TR_ENTRY, "sqlCPCOpen", 0);
  if (!pi10TryEnterSession(sqlca->sqlgap)) {
    p08runtimeerror (sqlca, sqlxa, cpr_connection_busy);
    p03traceerror(sqlca->sqlrap, sqlca->sqlemp); 
  }
  else {
    sqlxa->xakano = (sqlint2) stno;
    sqlca->sqlrap->raactsession = (sqlint2) sess;
    {
      tpr01_ModuleDesc *ModuleDesc = sqlxa->xaModDesc;
      tpr01_ModuleContainer *Module = ModuleDesc->Module;
      tpr01_PrecomDesc *PrecomDesc = Module->GetPrecomDesc(ModuleDesc, sqlxa, stno);
      tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
      tpr01_ConDesc *ConDesc = Precom->GetConnection(PrecomDesc, ModuleDesc, sess);
      Precom->Open(PrecomDesc, ModuleDesc, ConDesc);
    }    
    pi10LeaveSession(sqlca->sqlgap);
  }
  M90TRACE(M90_TR_EXIT, "sqlCPCOpen", 0);
}

void sqlCPCExecDirect (sqlcatype *sqlca, sqlxatype *sqlxa, long stno, long sess)
{  
  M90TRACE(M90_TR_ENTRY, "sqlCPCExecDirect", 0);
  if (!pi10TryEnterSession(sqlca->sqlgap)) {
    p08runtimeerror (sqlca, sqlxa, cpr_connection_busy);
    p03traceerror(sqlca->sqlrap, sqlca->sqlemp); 
  }
  else {
    tpr01_ModuleDesc *ModuleDesc = sqlxa->xaModDesc;
    tpr01_ModuleContainer *Module = ModuleDesc->Module;
    tpr01_PrecomDesc *PrecomDesc = Module->GetPrecomDesc(ModuleDesc, sqlxa, stno);
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    tpr01_ConDesc *ConDesc = Precom->GetConnection(PrecomDesc, ModuleDesc, sess);
    Precom->PrepareStmtName(PrecomDesc, ModuleDesc, ConDesc);
    Precom->DeclareCursor(PrecomDesc, ModuleDesc, ConDesc);
    Precom->Open(PrecomDesc, ModuleDesc, ConDesc);
    pi10LeaveSession(sqlca->sqlgap);
  }
  M90TRACE(M90_TR_EXIT, "sqlCPCExecDirect", 0);
}

void sqlCPCDescribe (sqlcatype *sqlca, sqlxatype *sqlxa, long stno, long sess)
{
  M90TRACE(M90_TR_ENTRY, "sqlCPCDescribe", 0);
  if (!pi10TryEnterSession(sqlca->sqlgap)) {
    p08runtimeerror (sqlca, sqlxa, cpr_connection_busy);
    p03traceerror(sqlca->sqlrap, sqlca->sqlemp); 
  }
  else {
    {
      tpr01_ModuleDesc *ModuleDesc = sqlxa->xaModDesc;
      tpr01_ModuleContainer *Module = ModuleDesc->Module;
      tpr01_PrecomDesc *PrecomDesc = Module->GetPrecomDesc(ModuleDesc, sqlxa, stno);
      tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
      tpr01_ConDesc *ConDesc = Precom->GetConnection(PrecomDesc, ModuleDesc, sess);
      Precom->Describe(PrecomDesc, ModuleDesc, ConDesc);
    }    
    pi10LeaveSession(sqlca->sqlgap);
  }
  M90TRACE(M90_TR_EXIT, "sqlCPCDescribe", 0);
}

/* sqlCPCCmd */
void sqcccmd (sqlcatype *sqlca, sqlxatype *sqlxa, long kano)
{
  M90TRACE(M90_TR_ENTRY, "sqcccmd", 0);
  if (!pi10TryEnterSession(sqlca->sqlgap)) {
    p08runtimeerror (sqlca, sqlxa, cpr_connection_busy);
    p03traceerror(sqlca->sqlrap, sqlca->sqlemp); 
  }
  else {
    p01command (sqlca, sqlxa, sqlca->sqlgap->gaentry[sqlca->sqlrap->raactsession-1], &sqlxa->sqlkap[kano-1], (sqlint2) kano);
    pi10LeaveSession(sqlca->sqlgap);
  }
  M90TRACE(M90_TR_EXIT, "sqcccmd", 0);
}

static char *pi30DriverName;
void sqlCPCPutDriverName (char *szDriverName)
{
  M90TRACE(M90_TR_STRING, "sqlCPCPutDriverName", szDriverName);
  pi30DriverName = szDriverName;
}

char *sqlCPCGetDriverName (char *szDriverName)
{
  M90TRACE(M90_TR_STRING, "sqlCPCGetDriverName", pi30DriverName);
  return pi30DriverName;
}

void sqlCPCTraceLine (sqlcatype *sqlca, sqlxatype *sqlxa, long kano)
{
  M90TRACE(M90_TR_ENTRY, "sqlCPCTraceLine", 0);
  sqlxa->xakano = (sqlint2) kano;
  {
    tpr01_ModuleDesc *ModuleDesc = sqlxa->xaModDesc;
    tpr01_ModuleContainer *Module = ModuleDesc->Module;
    tpr01_PrecomDesc *PrecomDesc = Module->GetPrecomDesc(ModuleDesc, sqlxa, kano);
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    tpr01_ConDesc *ConDesc = Precom->GetConnection(PrecomDesc, ModuleDesc, cpr_se_primary);
    Precom->TraceLine(PrecomDesc, ModuleDesc, ConDesc);    
  }
  M90TRACE(M90_TR_EXIT, "sqlCPCTraceLine", 0);
}

void sqcctrl (sqlcatype *sqlca, sqlxatype *sqlxa, long stno)
{
  M90TRACE(M90_TR_ENTRY, "sqcctrl", 0);
  sqlCPCTraceLine (sqlca, sqlxa, stno);
  M90TRACE(M90_TR_EXIT, "sqcctrl", 0);
}

void sqccnol (sqlcatype *sqlca, long sess)
{
  M90TRACE(M90_TR_ENTRY, "sqccnol", 0);
  p03NologSession(sqlca->sqlgap, cpr_is_true);
  M90TRACE(M90_TR_EXIT, "sqccnol", 0);
}

void sqlCPCNoLog (sqlcatype *sqlca, sqlxatype *sqlxa, long sess)
{
  M90TRACE(M90_TR_ENTRY, "sqlCPCNoLog", 0);
  p01xtracefilecheck (sqlca, sqlxa);
  p04trline (sqlca->sqlrap, "");
  p04trline (sqlca->sqlrap, "SET NOLOG");
  p03NologSession(sqlca->sqlgap, cpr_is_true);
  M90TRACE(M90_TR_EXIT, "sqlCPCNoLog", 0);
}

void sqlCPCTryExecute (sqlcatype *sqlca, sqlxatype *sqlxa, long kano, long sess)
{
  M90TRACE(M90_TR_ENTRY, "sqlCPCTryExecute", 0);
  M90TRACE(M90_TR_DWORD, "kano", &kano);
  M90TRACE(M90_TR_DWORD, "sess", &sess);
    if (!pi10TryEnterSession(sqlca->sqlgap)) {
      p08runtimeerror (sqlca, sqlxa, cpr_connection_busy);
      p03traceerror(sqlca->sqlrap, sqlca->sqlemp); 
    }
    else {
      sqlxa->xakano = (sqlint2) kano;
      sqlca->sqlrap->raactsession = (sqlint2) sess;
      if (sqlxa->sqlkap[kano-1].katyp == cpr_com_abort_session) {
        sqlca->sqlrap->raactsession = (sqlint2) sess;
        {
          tpr01_ModuleDesc *ModuleDesc = sqlxa->xaModDesc;
          tpr01_ModuleContainer *Module = ModuleDesc->Module;
          tpr01_PrecomDesc *PrecomDesc = Module->GetPrecomDesc(ModuleDesc, sqlxa, kano);
          tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
          tpr01_ConDesc *ConDesc = Precom->GetConnection(PrecomDesc, ModuleDesc, sess);
          Precom->AbortSession(PrecomDesc, ModuleDesc, ConDesc);
        }
      }
      else {
        tpr01_ModuleDesc *ModuleDesc = sqlxa->xaModDesc;
        tpr01_ModuleContainer *Module = ModuleDesc->Module;
        tpr01_PrecomDesc *PrecomDesc = Module->GetPrecomDesc(ModuleDesc, sqlxa, kano);
        tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
        tpr01_ConDesc *ConDesc = Precom->GetConnection(PrecomDesc, ModuleDesc, sess);
        Precom->TryExecute(PrecomDesc, ModuleDesc, ConDesc);
      }
      pi10LeaveSession(sqlca->sqlgap);
    }
  M90TRACE(M90_TR_EXIT, "sqlCPCTryExecute", 0);
}

/* sqlCPCEnd */
void sqlCPCExecute (sqlcatype *sqlca, sqlxatype *sqlxa, long kano, long sess)
{
  M90TRACE(M90_TR_ENTRY, "sqlCPCExecute", 0);
  M90TRACE(M90_TR_DWORD, "kano", &kano);
  M90TRACE(M90_TR_DWORD, "sess", &sess);
  if (!pi10TryEnterSession(sqlca->sqlgap)) {
    p08runtimeerror (sqlca, sqlxa, cpr_connection_busy);
    p03traceerror(sqlca->sqlrap, sqlca->sqlemp); 
  }
  else {
    tpr01_ModuleDesc *ModuleDesc = sqlxa->xaModDesc;
    tpr01_ModuleContainer *Module = ModuleDesc->Module;
    tpr01_PrecomDesc *PrecomDesc = Module->GetPrecomDesc(ModuleDesc, sqlxa, kano);
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    tpr01_ConDesc *ConDesc = Precom->GetConnection(PrecomDesc, ModuleDesc, sess);
    Precom->Execute(PrecomDesc, ModuleDesc, ConDesc);
    pi10LeaveSession(sqlca->sqlgap);
  }
  M90TRACE(M90_TR_EXIT, "sqlCPCExecute", 0);
}

void sqlCPCPrepare(sqlcatype *sqlca, sqlxatype *sqlxa, long prind, long sess)
{
  M90TRACE(M90_TR_ENTRY, "sqlCPCPrepare", 0);
  M90TRACE(M90_TR_DWORD, "prind", &prind);
  M90TRACE(M90_TR_DWORD, "sess", &sess);
  if (!pi10TryEnterSession(sqlca->sqlgap)) {
    p08runtimeerror (sqlca, sqlxa, cpr_connection_busy);
    p03traceerror(sqlca->sqlrap, sqlca->sqlemp); 
  }
  else {
    {
      tpr01_ModuleDesc *ModuleDesc = sqlxa->xaModDesc;
      tpr01_ModuleContainer *Module = ModuleDesc->Module;
      tpr01_PrecomDesc *PrecomDesc = Module->GetPrecomDesc(ModuleDesc, sqlxa, prind);
      tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
      tpr01_ConDesc *ConDesc = Precom->GetConnection(PrecomDesc, ModuleDesc, sess);
      Precom->Prepare(PrecomDesc, ModuleDesc, ConDesc);
    }
    pi10LeaveSession(sqlca->sqlgap);
  }
  M90TRACE(M90_TR_EXIT, "sqlCPCPrepare", 0);
}

/* sqlCPCReConnect */
void sqccrcn (sqlcatype *sqlca, long sess)
{
  M90TRACE(M90_TR_ENTRY, "sqccrcn", 0);
  M90TRACE(M90_TR_DWORD, "sess", &sess);
  sqlca->sqlgap->gasqlconntyp = cpr_ci_implicit;
  sqlca->sqlgap->gaentry [sess - 1]->gareconnect = cpr_is_true;
  M90TRACE(M90_TR_EXIT, "sqccrcn", 0);
}

/* sqlCPCStop */
void sqccstp (sqlcatype *sqlca, sqlxatype *sqlxa)
{
  M90TRACE(M90_TR_ENTRY, "sqccstp", 0);
  if (!pi10TryEnterSession(sqlca->sqlgap)) {
    p08runtimeerror (sqlca, sqlxa, cpr_connection_busy);
    p03traceerror(sqlca->sqlrap, sqlca->sqlemp); 
  }
  else {
    p01stop (sqlca, sqlxa);
    pi10LeaveSession(sqlca->sqlgap);
  }
  M90TRACE(M90_TR_EXIT, "sqccstp", 0);
}

/* sqlCPCExit */
void sqccexi (sqlcatype *sqlca, sqlxatype *sqlxa)
{
  M90TRACE(M90_TR_ENTRY, "sqccexi", 0);
  if (!pi10TryEnterSession(sqlca->sqlgap)) {
    p08runtimeerror (sqlca, sqlxa, cpr_connection_busy);
    p03traceerror(sqlca->sqlrap, sqlca->sqlemp); 
  }
  else {
    p01exit (sqlca, sqlxa);
    pi10LeaveSession(sqlca->sqlgap);
  }
  M90TRACE(M90_TR_EXIT, "sqccexi", 0);
}

/* sqlCPCDaMax */
void sqccdam (sqlcatype *sqlca, sqldatype *sqlda)
{
  M90TRACE(M90_TR_ENTRY, "sqccdam", 0);
  sqlda->sqlmax = (sqlca->sqldbmode == CPR_KIND_DB2) ? 0 : sqlnmax;
  M90TRACE(M90_TR_EXIT, "sqccdam", 0);
  M90TRACE(M90_TR_WORD, "sqlmax", &sqlda->sqlmax);
}

/* sqlCPCDaBind */
void sqccdaa (sqlcatype *sqlca, sqldatype *sqlda)
{
  M90TRACE(M90_TR_ENTRY, "sqccdaa", 0);
  M90TRACE(M90_TR_PTR, "sqlda", &sqlda);
  sqlca->sqlcxap->xasqldap = sqlda;
  strcpy (sqlda->sqldaid, "SQLDA");
  M90TRACE(M90_TR_EXIT, "sqccdaa", 0);
}
	
/* sqlCPCAtBind */
void sqccatb (sqlatentry *sqlat, long atno, long dbindex, long dbnamix,
	      char *dbname, char *dbstring)
{
  M90TRACE(M90_TR_ENTRY, "sqccatb", 0);
  {
    sqlatentry *at = sqlat + atno;
    int l;
    
    at->atdbindex = (sqlint2) dbindex;
    at->atdbnamix = (sqlint2) dbnamix;
    at->atgaindex = 0;
    l = (int) strlen (dbname);
    if (l > sizeof (at->atdbname))
      l = sizeof (at->atdbname);
    memset (at->atdbname, ' ', sizeof (at->atdbname));
    memcpy (at->atdbname, dbname, l);
    l = (int) strlen (dbstring);
    if (l > sizeof (at->atdbstring))
      l = sizeof (at->atdbstring);
    memset (at->atdbstring, ' ',
	    sizeof (at->atdbstring));
    memcpy (at->atdbstring, dbstring, l);
  }
  M90TRACE(M90_TR_EXIT, "sqccatb", 0);
}

/* sqlCPCCancel */
void sqcccan (sqlcatype *sqlca, sqlxatype *sqlxa, long stno)
{
  M90TRACE(M90_TR_ENTRY, "sqcccan", 0);
  if (stno > 0) {
    /* use an PrecomDesc allocated on the stack */
    tpr01_PrecomDesc PrecomDesc;
    /* use an SQLDesc allocated on the stack */
    tpr01_SQLDesc SQLDesc;

    tpr01_ConDesc *ConDesc;
    tpr01_ModuleDesc *ModuleDesc = sqlxa->xaModDesc;
    tpr01_ModuleContainer *Module = ModuleDesc->Module;
    tpr01_PrecomContainer *Precom = ModuleDesc->Precom;
    tpr01_SQLContainer *SQL = Precom->SQL;

    /* init local SQLDesc like AddDesc will do */
    SQLDesc.DescType = SQLDesc_epr01;
    SQLDesc.SQL = SQL;
    SQLDesc.ErrorDesc = NULL;
    SQL->InitDesc(&SQLDesc, Precom->sqlxa);

    /* init local PrecomDesc like AddDesc will do */
    PrecomDesc.DescType = PrecomDesc_epr01;
    PrecomDesc.Precom = ModuleDesc->Precom;
    PrecomDesc.SQLDesc  = &SQLDesc;
    PrecomDesc.ka = Precom->sqlxa->sqlkap + stno-1;
    PrecomDesc.CmdNo = stno;
    PrecomDesc.pr = NULL;
    PrecomDesc.ComType = PrecomDesc.ka->katyp;
    PrecomDesc.ErrorDesc = NULL;
    PrecomDesc.StmtNameDesc = NULL;
    ConDesc = Precom->GetConnection(&PrecomDesc, ModuleDesc, cpr_se_primary);
    Precom->Cancel(&PrecomDesc, ModuleDesc, ConDesc);
  }
  M90TRACE(M90_TR_EXIT, "sqcccan", 0);
}

#define cpr_se_temp (-1)

/* sqlCPCVersion */
void sqccver (sqlcatype *sqlca, sqlxatype *sqlxa, long stno)
{
  M90TRACE(M90_TR_ENTRY, "sqccver", 0);
  if (stno > 0) {
    tpr01_ModuleDesc *ModuleDesc = sqlxa->xaModDesc;
    tpr01_ModuleContainer *Module = ModuleDesc->Module;
    tpr01_PrecomDesc *PrecomDesc = Module->GetPrecomDesc(ModuleDesc, sqlxa, stno);
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    /* creates a temporary ConDesc PTS 1110136 */
    tpr01_ConDesc *ConDesc = Precom->GetConnection(PrecomDesc, ModuleDesc, cpr_se_empty);
    Precom->Version(PrecomDesc, ModuleDesc, ConDesc);    
    if (ConDesc) {
      tpr01_ConContainer *Connection = ConDesc->Connection;      
      Connection->Release(ConDesc);
    }    
  }
  M90TRACE(M90_TR_EXIT, "sqccver", 0);
}

/* sqlCPCIsolevel */
void sqlcisol (sqlcatype *sqlca, long isol)
{
  M90TRACE(M90_TR_ENTRY, "sqlcisol", 0);
  M90TRACE(M90_TR_DWORD, "isol", &isol);
  sqlca->sqlgap->gamodisolation = (sqlint2)isol;
  M90TRACE(M90_TR_EXIT, "sqlcisol", 0);
}

void sqlCPCOption (sqlcatype *sqlca, sqlxatype *sqlxa, long kano)
{
  M90TRACE(M90_TR_ENTRY, "sqlCPCOption", 0);
  M90TRACE(M90_TR_DWORD, "kano", &kano);
  {
    tpr01_ModuleDesc *ModuleDesc = sqlxa->xaModDesc;
    tpr01_ModuleContainer *Module = ModuleDesc->Module;
    tpr01_PrecomDesc *PrecomDesc = Module->GetPrecomDesc(ModuleDesc, sqlxa, kano);
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    /* creates and uses a temporary ConDesc */
    tpr01_ConDesc *ConDesc = Precom->GetConnection(PrecomDesc, ModuleDesc, cpr_se_empty);
    Precom->Option(PrecomDesc, ModuleDesc, ConDesc);
    if (ConDesc) {
      tpr01_ConContainer *Connection = ConDesc->Connection;      
      Connection->Release(ConDesc);
    }  
  }
  M90TRACE(M90_TR_EXIT, "sqlCPCOption", 0);
}

#define PI30NUMELEM(v, t) (sizeof(v)/sizeof(t))

static BOOLEAN pi30c_IsValidDynamicArea(SQLAREAS cbAreaNum)
{
  M90TRACE(M90_TR_INT, "pi30c_IsValidDynamicArea", &cbAreaNum);
  {
    boolean ret = FALSE;
    if (cbAreaNum >= 0 && 
	cbAreaNum < PI30NUMELEM(pi30SQLAreas, pi30SQLAreasStruct)) {
      ret = TRUE;
    }
    else {
      ret = FALSE;
    }
    return ret;
  }
}


/* sqlCPCAddr */
void *sqlcaddr (SQLAREAS cbAreaNum, void *pBaseClass)
{
  M90TRACE(M90_TR_INT, "sqlcaddr", &cbAreaNum);
  if (pi30c_IsValidDynamicArea(cbAreaNum)) {
    PR07_HEAPCHECK(NULL);
    return(((char*)pBaseClass)+pi30SQLAreas[cbAreaNum].cbOffsetOf);
  }
  else {
    return(NULL);
  }
}

int sqlcsize (SQLAREAS cbAreaNum)
{
  M90TRACE(M90_TR_INT, "sqlcsize", &cbAreaNum);
  if (pi30c_IsValidDynamicArea(cbAreaNum))
    return(pi30SQLAreas[cbAreaNum].cbSize);
  else
    return(0);
}

void *sqlcdynp (SQLAREAS cbAreaNum, void *pBaseClass, int cbNumElem)
{
  void **p=NULL;
  unsigned char ok;
  void **e=NULL;
  M90TRACE(M90_TR_INT, "sqlcdynp", &cbAreaNum);
  if (pi30c_IsValidDynamicArea(cbAreaNum)) {
    p = sqlcaddr(cbAreaNum, pBaseClass);
  }
  else {
    return(NULL);
  }
  if (!*p) {
    ok = TRUE;
    cbNumElem *= pi30SQLAreas[cbAreaNum].cbSize;
    *p = pr03mAllocat(cbNumElem, "sqlcdynp");
    if (*p) {
      memset(*p, 0, cbNumElem);
      switch(cbAreaNum) {
      case (SQLENUMA(sqlxa, sqlpap)):
	e = sqlcaddr(SQLENUMA(sqlxa, sqlpae), pBaseClass);
	break;
      case (SQLENUMA(sqlxa, sqlkap)):
	e = sqlcaddr(SQLENUMA(sqlxa, sqlkae), pBaseClass);
	break;
      case (SQLENUMA(sqlxa, sqlorp)):
	e = sqlcaddr(SQLENUMA(sqlxa, sqlore), pBaseClass);
	break;
      case (SQLENUMA(sqlxa, sqlprp)):
	e = sqlcaddr(SQLENUMA(sqlxa, sqlpre), pBaseClass);
	break;
      case (SQLENUMA(sqlxa, sqlstp)):
	e = sqlcaddr(SQLENUMA(sqlxa, sqlste), pBaseClass);
	break;
      case (SQLENUMA(sqlunused, sqlunused1)):
	e = sqlcaddr(SQLENUMA(sqlunused, sqlunused1), pBaseClass);
	break;
      case (SQLENUMA(sqlunused, sqlunused2)):
	e = sqlcaddr(SQLENUMA(sqlunused, sqlunused2), pBaseClass);
	break;
      case (SQLENUMA(sqlxa, sqlfap)):
	e = sqlcaddr(SQLENUMA(sqlxa, sqlfae), pBaseClass);
	break;
      case (SQLENUMA(sqlxa, sqlfnp)):
	e = sqlcaddr(SQLENUMA(sqlxa, sqlfne), pBaseClass);
	break;
      case (SQLENUMA(sqlxa, sqlatp)):
	e = sqlcaddr(SQLENUMA(sqlxa, sqlate), pBaseClass);
	break;
      case (SQLENUMA(sqlxa, sqlcup)):
	e = sqlcaddr(SQLENUMA(sqlxa, sqlcue), pBaseClass);
	break;
      }
      if (!*e)
	*e = ((char*)*p)+cbNumElem;
    }
  }
  return(*p);
}

void sqlgetver(char *szVersion, char* szMinVersion)
{
  M90TRACE(M90_TR_ENTRY, "sqlgetver", 0);
  strcpy(szVersion, pr07RelNr());
  strcpy(szMinVersion, pr07MinRelNr());
  M90TRACE(M90_TR_EXIT, "sqlgetver", 0);
}

void sqlCPCDeclare (sqlcatype *sqlca, sqlxatype *sqlxa, long kano, long sess)
{
  M90TRACE(M90_TR_ENTRY, "sqlCPCDeclare", 0);
  M90TRACE(M90_TR_DWORD, "kano", &kano);
  M90TRACE(M90_TR_DWORD, "sess", &sess);
  {
    tpr01_ModuleDesc *ModuleDesc = sqlxa->xaModDesc;
    tpr01_ModuleContainer *Module = ModuleDesc->Module;
    tpr01_PrecomDesc *PrecomDesc = Module->GetPrecomDesc(ModuleDesc, sqlxa, kano);
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    tpr01_ConDesc *ConDesc = Precom->GetConnection(PrecomDesc, ModuleDesc, sess);
    Precom->DeclareCursor(PrecomDesc, ModuleDesc, ConDesc);
  }
  M90TRACE(M90_TR_EXIT, "sqlCPCDeclare", 0);
}
