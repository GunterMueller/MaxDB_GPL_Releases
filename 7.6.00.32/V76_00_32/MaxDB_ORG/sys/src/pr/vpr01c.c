/*!================================================================
 module:    vpr01c.c

 responsible: BurkhardD

 special area:   CPCDrv | Client | SQL Commando Abarbeitung

 description: SQL Commando Abarbeitung

 last changed:
 see:

 change history:

        2001-11-08 D025086 Cancel session bugfix PTS 1112515 and PASCAL 
        port from p01cancel,

        2001-06-22 MarcoP adding code for FOrS

        2001-06-22 D025086 Check if the ParseInfocache is deactivated

        2001-05-17 D025086 Use pr03PacketReqRec instead of p03reqrecpacket
        to protect unexpected communication errors.

        2000-08-07  17:17 MarcoP added Unicode extensions


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

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "hsp26.h"
#define sql_sqldb
#define sql_oracle
#include "gpr00.h"
#include "gpr01.h"
#include "gpr03.h"
#include "gpr04.h"
#include "gpr05.h"
#include "gpr08.h"
#include "vpr08c.h"
#include "vpr07Macro.h"
#include "vpr01Env.h"
#include "vpr01Cursor.h"
#include "vpr01Precom.h"
#include "vpr01StmtName.h"
#include "vpr01Con.h"
#include "vpr01Module.h"
#include "vpr01SQL.h"
#include "vpr03Segment.h"
#include "vpr03Part.h"
#include "vpr03Packet.h"
#include "vpr06ParseId.h"
#include "vpr01Trace.h"
#include "vpr04Long.h"
#include "hsp78_0.h"
#include "vpr01ParseInfo.h"
/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

boolean p01prepare(sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, sqlkaentry *ka, sqlprentry *pre, sqlorentry *ore, boolean datafound);

static tsp00_Int4 pr01cCheckParseFetch (tpr01_SQLDesc *SQLDesc) ;

static void p01copendata(sqlcatype* sqlca, sqlxatype* sqlxa, sqlkaentry* kae, sqlcuentry* cue, sqlgaentry* gae, tsp00_Int4 mass_cmd, tsp00_Int4* found, tpr00_ParseId *selparsid);

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  EXPORTED FUNCTIONS                                              *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

tsp00_Int4 pr01P2NTS(char *p1, char *p2, tsp00_Int4 len);
tpr01_CursorDesc *pr01cFindCursorEntry(sqlxatype *sqlxa, char *cuname);
static void pr01cExecute(tpr01_SQLDesc *SQLDesc);

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/
/* grep means - that this function is not used
sqlcupointer
pr01cGetCursorDesc(sqlxatype *sqlxap, tpr00_CursorName szCursorName)
{
  sqlcupointer pDesc = NULL;
  sqlcupointer cup = sqlxap->sqlcup;
  sqlcupointer cue = sqlxap->sqlcue;
  int cbMaxCu = cue-cup;
  int i;
  for (i=0;i<cbMaxCu;i++) {
    if(!strcmp((char*)(cup+i)->cuname, szCursorName)) {
      pDesc = cup+i;
      break;
    }
  }
  return(pDesc);
}
*/

int s30klen(char *str, char val, int cnt);

tsp00_Int4 pr01P2NTS(char *p1, char *p2, tsp00_Int4 len)
{
  tsp00_Int4 l1 = s30klen(p2, bsp_c1, len);
  memcpy(p1, p2, l1);
  p1[l1] = '\0';
  return len;
}

boolean pr01cParsePrepare(tpr01_SQLDesc *SQLDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01cParsePrepare", 0);
  {
    tpr01_SQLContainer *SQL = SQLDesc->SQL;
    tpr01_ParseInfoContainer *ParseInfoCache = SQLDesc->ConDesc->ParseInfoCache;
    tpr01_ParseInfoDesc      *ParseInfoDesc = NULL;      
    sqlxatype *sqlxa = SQL->GetSqlxa(SQLDesc);
    sqlcatype *sqlca = SQL->GetSqlca(SQLDesc);
    sqlratype *sqlra = sqlca->sqlrap;
    struct SQLERROR *sqlemp = sqlca->sqlemp;
    sqlprentry *pr = SQLDesc->pr;
    sqlorentry *ore = SQLDesc->ore;
    sqlkaentry *ka   = SQLDesc->ka;
    sqlgaentry *ga = SQLDesc->ConDesc->ga;
    sqlsnentry *sn  = SQLDesc->StmtNameDesc->sn;
    boolean ret = FALSE;
    if (sqlca->sqlcode == 0) {
      tpr01_ConContainer *Connection = SQLDesc->ConDesc->Connection;
      Connection->InitPacket (SQLDesc->ConDesc, SQLDesc->PacketEncoding, sp1m_parse);
      ka->kastate = cpr_state_parse;
/*      ore->oropennew = cpr_is_init;*/
      pr01cBeg(SQLDesc);
      if (sqlxa->xacmdinit == cpr_is_true) {
        /* holt kommando in auftrags puffer */
        pr01cExecute(SQLDesc);
        if (!SQLDesc->Attribute.parspre) {
          tsp00_Int2 cmdfetch;
          tsp00_Int4 datafound;

          if ( (   SQLDesc->StmtNameDesc->StmtTypeDesc.StmtType == KeywordFETCH_epr01
                   || SQLDesc->StmtNameDesc->StmtTypeDesc.StmtType == KeywordFFETCH_epr01
                   || SQLDesc->StmtNameDesc->StmtTypeDesc.StmtType == KeywordMFETCH_epr01
                   )
               && SQLDesc->StmtNameDesc->StmtTypeDesc.UsingPos > 0
               ) {
            /*you cannot send a parse order for a command like "FETCH CURSOR USING DESCRIPTOR"
              because you don't know how much parameters this command contains*/    
            SQLDesc->ComType = cpr_com_fetch_describe;
            if (pr01TraceIsTrace(sqlra)) {
              pr01TracePrintf(sqlra, "\n");
              pr01TraceCmd(sqlra, SQLDesc->StmtNameDesc->SQLStatement);
              pr01TracePrintf(sqlra, "IGNORE PARSING");
              p01pparsidtrace(sqlca, sqlxa, ka, ka->kaParseInfo.ParseId, TraceParseIDSQLStmt_epr00);
              p01xtimetrace(sqlca, sqlxa, ga);
            }
          }
          else {
            if (SQLDesc->Attribute.massopen) {
              tpr05_StmtAnalysisDesc *StmtTypeDesc = &SQLDesc->StmtNameDesc->StmtTypeDesc;
              if ( (StmtTypeDesc->StmtType == KeywordFETCH_epr01
                    || StmtTypeDesc->StmtType == KeywordFFETCH_epr01
                    || StmtTypeDesc->StmtType == KeywordMFETCH_epr01)
                   && SQLDesc->StmtNameDesc->StmtTypeDesc.UsingPos > 0 ) {
                SQLDesc->Attribute.massopen = false;
              }              
            }
            if(SQLDesc->ConDesc->Parse(SQLDesc, &cmdfetch, &datafound)){
             if (sqlca->sqlcode == 0) {
                sn->snsession = ActualSession_cpr00;
                pr06ParseInfoCopy(&sn->snParseInfo, &ka->kaParseInfo);
                ret = TRUE;
              }
            }
          } 
        }
        else {
          tsp1_part *part_ptr;
          p03find_part(sqlra, sp1pk_command, &part_ptr);
          if (part_ptr != NULL ) { 
            /* parse prepare */
            tsp00_Int2 cmdfetch;
            tsp00_Int4 datafound = 0;
            pr03SegmentSetAttribute(sqlra->rasegptr, Prepare_epr03, TRUE);
            if (   SQLDesc->Attribute.massopen
                   && sqlra->rasqlansi != cpr_kind_ansi
                   && sqlca->sqldbmode != cpr_kind_db2) {
              pr03SegmentSetAttribute(sqlra->rasegptr, Mass_Cmd_epr03, TRUE);
            }
            /* PTS 1108177 */
            if(SQLDesc->ConDesc->Parse(SQLDesc, &cmdfetch, &datafound)){
              p01prepare (sqlca, sqlxa, ga, ka, pr, ore, (boolean) datafound);
              p03returncodeget (sqlca, sqlxa);
              p01xtimetrace (sqlca, sqlxa, ga);
              sqlxa->xacmdinit = cpr_is_false;
            }
            if (sqlca->sqlcode == 0) {
              sn->snsession = ActualSession_cpr00;
              pr06ParseInfoCopy(&sn->snParseInfo, &ka->kaParseInfo);
              ret = TRUE;
            }
          }
        }
      }
      if (sqlca->sqlcode == 0 && sqlemp->ereturncode == -4000) {
        sqlca->sqlcode = sqlemp->ereturncode;
      }
      if ( !SQLDesc->ConDesc->ParseInfoCache 
          && sqlca->sqlcode == 0 
          && SQLDesc->Attribute.parspre) {
        /* belege kaprasid, orcolpointer, kastate = execute */
        p04GetSFInfo(sqlca, sqlxa, ka, ore);
        /* belege  orrespointer, */
        p01resallocinfo(sqlca, sqlxa, ka, ore);
        /* belege  orresnampointer,*/
        pr04ColNames2ore(sqlca, sqlxa, ka, ore);
        /*update parse infos in parseinfo cache*/
      }
    }
    M90TRACE(M90_TR_EXIT, "pr01cParsePrepare", 0);
    M90TRACE(M90_TR_BOOL, "ret", &ret);
    return(ret);
  }
}

void p01cCmdParseExecute(sqlcatype  *sqlca,
                         sqlxatype  *sqlxa,
                         sqlgaentry *gae,
                         sqlkaentry *kae,
                         sqlprentry *pre,
                         sqlorentry *ore,
                         sqlcuentry *cue) {
  tpr01_ParseInfoContainer *ParseInfoCache = sqlxa->xaSQLDesc->ConDesc->ParseInfoCache;
  tpr01_ParseInfoDesc      *ParseInfoDesc = NULL;      
  tsp00_Int2 cmdfetch;
  tsp1_cmd_mess_type cmdtyp;
  tsp00_Int4 datafound = 0;
  if (   sqlca->sqlemp->ereturncode == 0) {
    if ( kae->kapacount == -4) {
      pr03SegmentSetMessCode((tsp1_segment *)sqlca->sqlrap->rasegptr, sp1m_dbs);
    }
    else {
      pr03SegmentSetMessCode((tsp1_segment *)sqlca->sqlrap->rasegptr, sp1m_parse);
    }
    if(sqlxa->xaSQLDesc->ConDesc->Parse(sqlxa->xaSQLDesc, &cmdfetch, &datafound)){
      if (!p01prepare(sqlca, sqlxa, gae, kae, pre, ore, (boolean)datafound)
          || (int ) sqlca->sqlrap->raparse_again == 1) {
         kae->kastate = cpr_state_execute;
         cmdtyp = sp1m_execute;
         p01adbsexecute(sqlca, sqlxa, gae, kae , ore, cue , &cmdfetch , &cmdtyp , 0);
      }
    }
  }
}

void pr01cOpen(tpr01_SQLDesc *SQLDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01cOpen", 0);
  {
    tpr01_SQLContainer *SQL = SQLDesc->SQL;
    sqlxatype *sqlxa = SQL->GetSqlxa(SQLDesc);
    pr01cBeg(SQLDesc);
    while (sqlxa->xacmdinit == sqlIsTrue) {
      pr01cExecu(SQLDesc);
      pr01cEnd(SQLDesc);
    }
  }
  M90TRACE(M90_TR_EXIT, "pr01cOpen", 0);
}

void pr01cBeg(tpr01_SQLDesc *SQLDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01cBeg", 0);
  {
    tsp00_Int2 cmdfetch;
    BOOLEAN prepars;
    BOOLEAN preopen;
    BOOLEAN open_cuerrd3;
    BOOLEAN execute_at_fetch;
    tpr01_SQLContainer *SQL = SQLDesc->SQL;
    sqlcatype *sqlca = SQL->GetSqlca(SQLDesc);
    sqlxatype *sqlxa = SQL->GetSqlxa(SQLDesc);
    sqlratype *sqlra = sqlca->sqlrap;
    struct SQLERROR *sqlemp = sqlca->sqlemp;
    sqlkaentry *ka   = SQLDesc->ka;
    sqltatype  *sqlta = sqlra->rasqltap;
    sqlgaentry *gae  = SQLDesc->ConDesc->ga;
    sqlorentry *ore  = SQLDesc->ore;
    sqlcuentry *cu = SQLDesc->cu;
    prepars = false;
    preopen = false;
    open_cuerrd3 = false;
    execute_at_fetch = false;
    sqlxa->xacmdinit = cpr_is_true;
    sqlra->rakamode  = ka->kamode;
    if (sqlca->sqlcode == 0) {
      if  (gae->gareconnect == cpr_is_true) {
	ka->kastate = cpr_state_empty;
      }
      if ( ka->kastate == cpr_state_execute
	   && sqlca->sqldbmode == cpr_kind_sapr3
	   && gae->gareference == 0) {
	ka->kastate = cpr_state_empty;
      }
      if (cu && SQLDesc->ComType != cpr_com_sql_open) {
	if (SQLDesc->ComType == cpr_com_sql_fetch || SQLDesc->ComType == cpr_com_mfetch
	    || ka->kapacount == -2) { /* fetch using descr */
	  sqlkaentry *OpenKa;
	  open_cuerrd3 = true;
	  OpenKa = pr01sIsExecuteAtFetch(sqlxa);
	  if (OpenKa != NULL) {
	    pr01sExecuteAtFetch(SQLDesc);
	  }
	}
      }
      if (ka->kastate == cpr_state_empty) {
	/* 20.2.97 describe infos get from prepare */
	if (ka->kapacount == -3) { /* describe */
	  if (SQLDesc->ComType == cpr_com_describe) {
	    p03csqlcaareainit (sqlca);
	    prepars = true;
	    p01colinitsfinfo (sqlca, sqlxa, ka, ore);
	    p01colmovesfinfo (sqlca, sqlxa, ka, ore);
	    p01nammovesfinfo (sqlca, sqlxa, ka, ore, SQLDesc->ComType);
	    sqlxa->xacmdinit = cpr_is_false;
	  }
	  else {
	    if (SQLDesc->ComType == cpr_com_ora_descr_sel) {
	      p03csqlcaareainit (sqlca);
	      prepars = true;
	      p01resinitsfinfo (sqlca, sqlxa, ka, ore, &prepars);
	      if (prepars) {
		p01resmovesfinfo (sqlca, sqlxa, ka, ore, &prepars);
	      }
	      if (prepars) {
		p01pparsidtrace(sqlca, sqlxa, ka, ka->kaParseInfo.ParseId, TraceParseIDOther_epr00);
		p01nammovesfinfo (sqlca, sqlxa, ka, ore, SQLDesc->ComType);
		sqlxa->xacmdinit = cpr_is_false;
	      }
	    }
	  }
	}
      }
      if ( ka->kastate == cpr_state_execute
	   || ka->kastate == cpr_state_execute_at_fetch
	   || open_cuerrd3) {
	/* describe bei prepare geparsed */
	if (ka->kapacount == -3 && SQLDesc->ComType != cpr_com_ora_descr_sel) {
	  prepars = true;
	  p01colmovesfinfo(sqlca, sqlxa, ka, ore);
	  sqlxa->xacmdinit = cpr_is_false;
	}
	else {
	  /* open cursor ohne using descriptor  b.f 1.8.94 */
	  if (ka->kapacount == 0 && SQLDesc->ComType == cpr_com_sql_open) {
	    preopen = true;
	  }
	  if (ka->kapacount == -1 && SQLDesc->ComType != cpr_com_declare_cursor) {
	    /* open cursor oder execute bei prepare geparsed */
	    preopen = true;
	    p01colmovesfinfo (sqlca, sqlxa, ka, ore);
	    if (sqlca->sqldbmode == cpr_kind_sapr3) {
	      if (pr01sExecuteAtFetchPossible(sqlxa, ka)) {
		execute_at_fetch = true;
		pr01sPrepareExecuteAtFetch(sqlxa->xaSQLDesc);
	      }
	    }
	  }
	  if (open_cuerrd3) {
	    /* fetch, bei open row not found, wird beendet */
	    if ( cu && cu->cuerrd3 == -100 ) {
	      prepars = true;
	      sqlxa->xacmdinit = cpr_is_false;
	      sqlca->sqlerrd[2] = 0;
	      p01pparsidtrace(sqlca, sqlxa, ka, ka->kaParseInfo.ParseId, TraceParseIDFetch_epr00);
	      sqlemp->ereturncode = 100;
	      sqlemp->eerrorpos  = 0;
	      sqlemp->etextlen = (tsp00_Int4)strlen("ROW NOT FOUND");
	      memcpy(sqlemp->etext, "ROW NOT FOUND", sqlemp->etextlen);
	      p03returncodeget (sqlca, sqlxa);
	      p01xtimetrace (sqlca, sqlxa, gae);
	      p01xcmdclose (sqlca, sqlxa, gae);
	    }
	    if (cu->custate == cpr_is_true && !prepars) {
	      ka->kastate = cpr_state_parse;
	      cu->custate = cpr_is_init;
	    }
	  }
	}
      }
      if (!preopen && !prepars && !execute_at_fetch) {
	if (ka->kastate == cpr_state_empty) {
	  ka->kastate = cpr_state_parse;
	}
	if (ka->kamacro == cpr_is_true) {
	  sqlmatype *sqlma = sqlca->sqlmap;
	  if (sqlma->maversion != ka->kaversion) {
	    ka->kastate = cpr_state_parse;
	  }
	}
	if (ka->kapacount == -2 && ka->kastate == cpr_state_execute) {
	  /* fetch  cursor oder execute  geparsed */
	  if (ka->kaopendata == cpr_is_true)
	    p01resmovesfinfo (sqlca, sqlxa, ka, ore, &prepars);
	  else
	    p01colmovesfinfo (sqlca, sqlxa, ka, ore);
	}
      }
      if (!prepars && !execute_at_fetch) {
	/* abarbeitung kommand */
	if (sqlxa->xaprogn[0] == ' ' || sqlxa->xaprogc == 0) {
	  /* precompiler check-option check */
	  sqlxa->xacmdinit = cpr_is_false;
	  p08runtimeerror (sqlca, sqlxa, cpr_invalid_program);
	  p01xhalt (cpr_result_sqlxa_invalid);
	}
	else {
	  p01xtracefilecheck (sqlca, sqlxa);
	  if (sqlca->sqlcode != 0) {
	    sqlxa->xacmdinit = cpr_is_false;
	  }
	  else {
	    sqlxa->xacmdinit = cpr_is_true;
	    sqlxa->xastopcnt = 0;
	    ka->kaparamcnt = 0;
	    gae->gastatus = cpr_co_ok;
	    if (SQLDesc->ComType == cpr_com_crea_ind) {
	      ka->kastate = cpr_state_command;
	    }
	    switch (ka->kastate) {
	    case (cpr_state_command) : {
	      SQLDesc->MessType = sp1m_dbs;
	      break;
	    }
	    case (cpr_state_parse) : {
	      SQLDesc->MessType = sp1m_parse;
	      break;
	    }
	    case (cpr_state_execute_at_fetch) : {}
	    case (cpr_state_execute) : {
	      SQLDesc->MessType = sp1m_execute;
	      break;
	    }
	    default : {
	      SQLDesc->MessType = sp1m_nil;
	      p08runtimeerror (sqlca, sqlxa, cpr_invalid_commandstate);
	      break;
	    }
	    }
	    if ( sqlemp->elzu != sp1ce_ok) {
	      if ( sqlta->tatrout != cpr_trace_off) {
                /* leerzeile drucken */
                p08puttracename (sqlca, sqlxa, NULL, -1, true);
                p08puttracename (sqlca, sqlxa, NULL, -1, true);
	      }
	      p03returncodeget (sqlca, sqlxa);
	      p01xcmdclose (sqlca, sqlxa, gae);
	      sqlxa->xacmdinit = cpr_is_false;
	    }
	    else {
	      if (sqlca->sqlcode == 0)
                {
                  SQLDesc->ConDesc->Connection->InitPacket (SQLDesc->ConDesc, SQLDesc->PacketEncoding, SQLDesc->MessType);
                }
	      else
                {
                  p03ccmdinit(SQLDesc, sqlca, gae, SQLDesc->MessType);
                }
	      if (ka->kastate == cpr_state_execute
		  || ka->kastate == cpr_state_execute_at_fetch)
                {
                  p01cmd_trace(sqlca, sqlxa, ka, gae);
                  p01pparsidtrace(sqlca, sqlxa, ka, ka->kaParseInfo.ParseId, TraceParseIDSQLStmt_epr00);
                  cmdfetch = cpr_is_true;
                  p01adbsexecute (sqlca, sqlxa, gae, ka, ore, cu, &cmdfetch, &SQLDesc->MessType, true);
                  pr04LongRemoveDescriptors (sqlca);
                  if (ka->kastate != cpr_state_parse)
		    {
                      /* nur wenn nicht parse again */
                      p01xcmdclose (sqlca, sqlxa, gae);
		    }
                }
	    }
	  }
	}
      }
    }
  }
  M90TRACE(M90_TR_EXIT, "pr01cBeg", 0);
}

static void pr01cExecute(tpr01_SQLDesc *SQLDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01cExecute", 0);
  {
    tpr01_SQLContainer *SQL = SQLDesc->SQL;
    sqlcatype *sqlca = SQL->GetSqlca(SQLDesc);
    sqlxatype *sqlxa = SQL->GetSqlxa(SQLDesc);
    sqlratype *sqlra = sqlca->sqlrap;
    struct SQLERROR *sqlemp = sqlca->sqlemp;
    sqlkaentry *ka   = SQLDesc->ka;
    sqlprentry *pr   = SQLDesc->pr;
    sqlgaentry *gae   = SQLDesc->ConDesc->ga;
    tsp1_part *part_ptr;
    tpr05_StmtAnalysisDesc *StmtTypeDesc;
    /* Sets attributes massen, parspre depending on statement type*/
    pr01SQLgetAllAttributes(SQLDesc);
    if (ka->kapacount == -1 )
      {
        if (pr && pr->prDescribe == cpr_is_false )
          {
            pr->prDescribe = cpr_is_true;
          }
      }

        if (sqlca->sqlcode == 0)
          {
            SQLDesc->ConDesc->Connection->InitPacket (SQLDesc->ConDesc, SQLDesc->PacketEncoding, SQLDesc->MessType);
          }
        else
          {
            p03ccmdinit(SQLDesc, sqlca, gae, SQLDesc->MessType);
          }
        /*put SQL-Statement to part */
        pr08cputStatementtoPart(SQLDesc);
        /*    p03cputpart(sqlra, gae, sp1pk_command, pSQLStatement->rawString, pSQLStatement->cbLen, sqlemp);*/
        p03find_part (sqlra, sp1pk_command, &part_ptr);
        if ( part_ptr != NULL )
          {
            if (SQLDesc->ComType == cpr_com_sql )
              {
                StmtTypeDesc = &SQLDesc->StmtNameDesc->StmtTypeDesc;
                switch (StmtTypeDesc->StmtType)
                  {

                  case (KeywordCONNECT_epr01):
                    {
                      ka->katyp = cpr_com_sql_conn;
                      SQLDesc->ComType  = cpr_com_sql_conn;
                      break;
                    }

                  case (KeywordCOMMIT_epr01): {
                    if (StmtTypeDesc->StmtOption == KeywordRELEASE_epr01)
                      SQLDesc->ComType = ka->katyp = cpr_com_commit_release;
                    else
                      SQLDesc->ComType = ka->katyp = cpr_com_commit;
                    ka->kastate = cpr_state_command;
                    break;
                  }
                  case (KeywordROLLBACK_epr01): {
                    if (StmtTypeDesc->StmtOption == KeywordRELEASE_epr01)
                      SQLDesc->ComType = ka->katyp = cpr_com_rollback_release;
                    else
                      SQLDesc->ComType = ka->katyp = cpr_com_rollback;
                    ka->kastate = cpr_state_command;
                    break;
                  }

                  case (KeywordCREATE_epr01):
                    {
                      if(SQLDesc->StmtNameDesc->StmtTypeDesc.StmtOption == KeywordINDEX_epr01)
                        {
                          /* create (unique) index or find */
                          ka->katyp  = cpr_com_crea_ind;
                          SQLDesc->ComType  = cpr_com_crea_ind;
                          ka->kastate = cpr_state_command;
                        }
                      break;
                    }
                  case (KeywordFETCH_epr01) :
                  case (KeywordFFETCH_epr01):
                  case (KeywordMFETCH_epr01):
                    {
                      ka->katyp = cpr_com_sql_fetch;
                      SQLDesc->ComType  = cpr_com_sql_fetch;
                      break;
                    }
                  case (KeywordSETISOLEVEL_epr01): {
                      ka->kastate = cpr_state_command;  
                      SQLDesc->ComType  = cpr_com_sql;
                      break;
                  } 
                  default :
                    {
                      ka->katyp = cpr_com_sql;
                      SQLDesc->ComType  = cpr_com_sql;
                      break;
                    }
                  } /*end switch */
              } /*end if cpr_com_sql*/
          } /*end if part_ptr */
  }
  M90TRACE(M90_TR_EXIT, "pr01cExecute", 0);
}

void pr01cEnd(tpr01_SQLDesc *SQLDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01cEnd", 0);
  {
    tsp00_Int4 retsqlcode;
    tpr01_SQLContainer *SQL = SQLDesc->SQL;
    sqlcatype *sqlca = SQL->GetSqlca(SQLDesc);
    sqlxatype *sqlxa = SQL->GetSqlxa(SQLDesc);
    sqlratype *sqlra = sqlca->sqlrap;
    struct SQLERROR *sqlemp = sqlca->sqlemp;
    sqlkaentry *ka   = SQLDesc->ka;
    sqlprentry *pr   = SQLDesc->pr;
    sqlgaentry *gae   = SQLDesc->ConDesc->ga;
    sqlorentry *ore   = SQLDesc->ore;
    sqlcuentry *cu = SQLDesc->cu;

    sqlra->rakamode = ka->kamode;
    if (sqlca->sqlcode != 0)
      {
        p01pparsidtrace(sqlca, sqlxa, ka, ka->kaParseInfo.ParseId, TraceParseIDSQLStmt_epr00);
        p01xtimetrace (sqlca, sqlxa, gae);
      }
    else
      {
        if (ka->kapacount == -2)
          {
            /* fetch using descriptor */
            pr01cDynmfetch(SQLDesc);
          }
      }

    if ( sqlra->raopprof == cpr_is_true
         || sqlra->raopprof == cpr_is_only_pc)
      {
    	/* rette kommando fur sysprofile */
    	p03putsysprofbuf (sqlra);
      }
    if (SQLDesc->StmtNameDesc &&
	((SQLDesc->StmtNameDesc->StmtTypeDesc.StmtType == KeywordFETCH_epr01) ||
	 (SQLDesc->StmtNameDesc->StmtTypeDesc.StmtType == KeywordFFETCH_epr01) ||
	 (SQLDesc->StmtNameDesc->StmtTypeDesc.StmtType == KeywordMFETCH_epr01)) &&
	(cu && cu->cuerrd3 == -100))
      {
    	sqlxa->xacmdinit = cpr_is_false;
    	sqlca->sqlerrd[2] = 0;
    	p01pparsidtrace(sqlca, sqlxa, ka, ka->kaParseInfo.ParseId, TraceParseIDFetch_epr00);
    	sqlemp->ereturncode = 100;
    	sqlemp->eerrorpos  = 0;
    	sqlemp->etextlen = (tsp00_Int4)strlen("ROW NOT FOUND");
    	memcpy(sqlemp->etext, "ROW NOT FOUND", sqlemp->etextlen);
    	p03returncodeget (sqlca, sqlxa);
    	p01xtimetrace (sqlca, sqlxa, gae);
    	p01xcmdclose (sqlca, sqlxa, gae);
      }
    else
      {
    	if (ka->kastate == cpr_state_parse)
          {
            /* parse - execute kommando */
            if (ka->katyp == cpr_com_sql_putval)
              {
                /* Special handling for putval command */
                p01cmd_trace (sqlca, sqlxa, sqlxa->xaSQLDesc->ka, gae);
                p01pparsidtrace (sqlca, sqlxa, sqlxa->xaSQLDesc->ka,
                                 sqlxa->xaSQLDesc->ka->kaParseInfo.ParseId,
                                 TraceParseIDSQLStmt_epr00);
                pr04LongPutvalInput (sqlca, sqlxa, gae);
                p01xtimetrace (sqlca, sqlxa, gae);
                sqlxa->xacmdinit = cpr_is_false;
              }
            else if (ka->katyp == cpr_com_sql_getval) {
              /* Special handling for getval command */
              p01cmd_trace (sqlca, sqlxa, sqlxa->xaSQLDesc->ka, gae);
              p01pparsidtrace (sqlca, sqlxa, sqlxa->xaSQLDesc->ka,
                               sqlxa->xaSQLDesc->ka->kaParseInfo.ParseId,
                               TraceParseIDSQLStmt_epr00);
              pr04LongGetvalOutput (sqlca, sqlxa, gae, 0);
              p01xtimetrace (sqlca, sqlxa, gae);
            }
            else
            {
              p01cCmdParseExecute (sqlca, sqlxa, gae, ka, pr, ore, cu);
            }
          }
    	else
          {
    	    p01cmd_execute_immediate (sqlca, sqlxa, gae, ka);
          }
      }
    if ( ( sqlxa->xacmdinit == cpr_is_true && sqlca->sqlcode == 0)
         || sqlca->sqlcode == csp_old_fileversion
         || sqlca->sqlcode == csp_use_new_pars_info
         || sqlca->sqlcode == cpr_file_or_table_droped)
      {

        /* parse again */
        retsqlcode = sqlca->sqlcode;
        /* ADIS 1001501 */
        if (retsqlcode != 0)
          {
            p01pparsidtrace(sqlca, sqlxa, ka, ka->kaParseInfo.ParseId, TraceParseIDAgain_epr00);
          }
        p01bdrop_parsid (sqlca, sqlxa, ka->kaParseInfo.ParseId);
        if (SQLDesc->ConDesc->ParseIdCache)
          {
            pr06ParseIdDrop(sqlca, sqlxa, gae, (tpr06_ParseIDCache *)SQLDesc->ConDesc->ParseIdCache, &ka->kaParseInfo.ParseId);
          }
        else
          {
            p03dropparsid (sqlca, sqlxa, gae, ka->kaParseInfo.ParseId);
          }
        SQLDesc->MessType = sp1m_parse;
        if (sqlca->sqlcode == 0)
          {
            SQLDesc->ConDesc->Connection->InitPacket (SQLDesc->ConDesc, SQLDesc->PacketEncoding, SQLDesc->MessType);
          }
        else
          {
            p03ccmdinit(SQLDesc, sqlca, gae, SQLDesc->MessType);
          }
        ka->kaparamcnt = 0;
        sqlxa->xacmdinit = cpr_is_true;
        sqlxa->xastopcnt = sqlxa->xastopcnt + 1;
        if (sqlxa->xastopcnt >  Errorloop_mxpr00)
          {
    	    sqlxa->xacmdinit = cpr_is_false;
            /* ADIS 1001501 */
            if ( retsqlcode != 0)
              {
                sqlca->sqlcode = retsqlcode;
              }
          }
      }
    else
      {
        p01xcmdclose (sqlca, sqlxa, gae);
        sqlxa->xacmdinit = cpr_is_false;
      }
  }
  M90TRACE(M90_TR_EXIT, "pr01cEnd", 0);
}

void pr01cExecu(tpr01_SQLDesc *SQLDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01cExecu", 0);
  {
    int intopos = 0;
    tpr01_SQLContainer *SQL = SQLDesc->SQL;
    sqlcatype *sqlca = SQL->GetSqlca(SQLDesc);
    sqlxatype *sqlxa = SQL->GetSqlxa(SQLDesc);
    sqlratype *sqlra = sqlca->sqlrap;
    struct SQLERROR *sqlemp = sqlca->sqlemp;
    sqlkaentry *ka   = SQLDesc->ka;
    sqlprentry *pr   = SQLDesc->pr;
    sqlgaentry *gae   = SQLDesc->ConDesc->ga;

    pr01cExecute(SQLDesc);
    if (   ka->kapacount == 0
	   && SQLDesc->ComType != cpr_com_sql_open
	   && (pr && pr->prStmtName[0] == bsp_c1)) {
      /* execute immediate command */

      /* function no longer required
         p01_katyp_get (sqlca, sqlxa); */

      if ( sqlca->sqlcode == 0 ) {
        pr->prstate = cpr_state_command;
      }
    }
    M90TRACE(M90_TR_EXIT, "pr01cExecu", 0);
  }
}

void pr01cTraceStmtName(tpr01_StmtNameDesc *StmtNameDesc, sqlcatype *sqlca)
{
  tpr01_StmtNameContainer *StmtName = StmtNameDesc->StmtName;
  StmtName->TraceStmtName(StmtNameDesc, sqlca);
}

void pr01cOpenCursor(tpr01_SQLDesc *SQLDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01cOpenCursor", 0);
  {
    tpr01_SQLContainer *SQL = SQLDesc->SQL;
    sqlcatype *sqlca = SQL->GetSqlca(SQLDesc);
    sqlxatype *sqlxa = SQL->GetSqlxa(SQLDesc);
    sqlkaentry *ka   = SQLDesc->ka;
    sqlprentry *pr   = SQLDesc->pr;
    sqlgaentry *gae   = SQLDesc->ConDesc->ga;
    tpr_runtime_errors_Enum erg = false;
    tsp00_KnlIdentifier pnam;
    tsp00_Int2 len= 0;
    char massopen;
    char selectfound;
    if (ka->kapacount == -1 || SQLDesc->ComType == cpr_com_sql_open) {
      massopen = 0;
      if (ka->kaParseInfo.ParseId[10] == csp1_p_mass_select_found
	  || ka->kaParseInfo.ParseId[10] == csp1_p_reuse_mass_select_found
	  || ka->kaParseInfo.ParseId[10] == csp1_p_select_for_update_found
	  || ka->kaParseInfo.ParseId[10] == csp1_p_reuse_update_sel_found
	  || SQLDesc->ComType == cpr_com_sql_open)	{
	selectfound = true;
      }
      else {
	selectfound = false;
      }
      if (SQLDesc->ComType == cpr_com_sql_open && ka->kapaindex < 0) {
	/* array select */
	massopen = true;
      }
      if (selectfound) {
	if (SQLDesc->CursorDesc) {
	  tpr_runtime_errors_Enum err = cpr_p_ok;
	  tpr01_ConDesc *ConDesc = SQLDesc->ConDesc;
	  tpr03_SegmDesc *SegmDesc = ConDesc->SegmDesc;
	  tpr05_String *CursorName = &(SQLDesc->CursorDesc->CursorNameDesc.CursorName);

	  tsp1_part *partPtr = pr03SegmentAddPart(SegmDesc, sp1pk_resulttablename);
	  tsp00_Int4 cbMaxLen = pr03PartGetFreePartSpace(partPtr);
	  tsp00_Int4 cbLen = pr03PartGetPartLength(partPtr);
	  erg = pr03PartConverttoPart(partPtr, &cbLen, &cbMaxLen, SQLDesc->PacketEncoding,
                                      CursorName->rawString, CursorName->cbLen, CursorName->encodingType);
	  pr03SegmentFinishPart(SegmDesc);
	  if(erg != cpr_p_ok)
	    p08runtimeerror (sqlca, sqlxa, erg);
	  else {
            tpr05_String tmpString;
            tmpString.allocationType = constant_epr05;
            pr05IfCom_String_InitString(&tmpString, (char*)pnam, sizeof(pnam), sp77encodingUTF8, constant_epr05);
            memset((char*)pnam, ' ', sizeof(pnam));
            pr05IfCom_String_Convert (&tmpString,CursorName);
            len = tmpString.cbLen;
	  }
	}
	if(erg == cpr_p_ok) {
	  if (sqlca->sqlrap->rasqltap->tatrout != cpr_trace_off) {
	    char *nam;
	    if (massopen) {
	      nam = "   MASS EXECUTE:  ";
	    }
	    else {
	      nam = "        EXECUTE:  ";
	    }
	    p08puttracename(sqlca, sqlxa, nam, mxsp_name, 0);
	    nam = "OPEN CURSOR       ";
	    p08puttracename(sqlca, sqlxa, nam, mxsp_name, 0);
	    p08puttraceknlid(sqlca, sqlxa, pnam, len , 1);
	  }
	}
      }
    }
  }
  M90TRACE(M90_TR_EXIT, "pr01cOpenCursor", 0);
}

void pr01cDynmfetch(tpr01_SQLDesc *SQLDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01cDynmfetch", 0);
  {
    tpr01_SQLContainer *SQL = SQLDesc->SQL;
    sqlcatype *sqlca = SQL->GetSqlca(SQLDesc);
    sqlxatype *sqlxa = SQL->GetSqlxa(SQLDesc);
    sqlkaentry *ka   = SQLDesc->ka;
    if (ka->kapaindex < 0)
      {
        tsp1_part_ptr part_ptr;
        sqlratype *sqlra = sqlca->sqlrap;
        sqlparpointer  sqlpa = sqlxa->sqlpap+(-ka->kapaindex)-1;
        if (sqlpa->lo.pakind == SQLPARLO)
          {
            sqlv1pointer sqlv1p = sqlxa->sqlv1p+sqlpa->lo.pava1index-1;
            tsp00_Int4 *sqlloop = &((sqldatype*)sqlca->sqlcxap->xasqldap)->sqlloop;
            if (sqlpa->lo.pava1index != 0)
              {
                sqlv2pointer sqlv2p = sqlxa->sqlv2p+sqlv1p->sc.va1indva2-1;
                if (sqlv2p->va2typ == cpr_vint2)
                  {
                    *sqlloop = (*(tsp00_Int2*)sqlv1p->sc.va1addr);
                  }
                if (sqlv2p->va2typ == cpr_vint4)
                  {
                    *sqlloop = (*(tsp00_Int4*)sqlv1p->sc.va1addr);
                  }
              }
	    else
              {
	        *sqlloop = sqlpa->lo.paloopmin;
              }
          }
        else
          {
	    pr01TraceRuntimeError(sqlca, sqlxa, cpr_r_loop_init_wrong);
          }
        p03find_part(sqlra, sp1pk_command, &part_ptr);
        if (part_ptr != NULL)
          {
	    pr03SegmentSetAttribute(sqlra->rasegptr, Mass_Cmd_epr03, TRUE);
          }
      }
  }
  M90TRACE(M90_TR_EXIT, "pr01cDynmfetch", 0);
}

void pr01cDescribe(tpr01_SQLDesc *SQLDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01cDescribe", 0);
  {
    tpr01_SQLContainer *SQL = SQLDesc->SQL;
    sqlcatype *sqlca = SQL->GetSqlca(SQLDesc);
    sqlxatype *sqlxa = SQL->GetSqlxa(SQLDesc);
    sqlratype *sqlra = sqlca->sqlrap;
    struct SQLERROR *sqlemp = sqlca->sqlemp;
    sqlkaentry *ka   = SQLDesc->ka;
    sqlprentry *pr   = SQLDesc->pr;
    sqlgaentry *gae   = SQLDesc->ConDesc->ga;
    tsp00_Int4 sendDescOrder = 0; /*flag that signals sending a describe order to kernel or not (0 = no, 1 = yes)*/
    tsp00_Int4 unlabledResultset = 0; /*flag that signals an unlabeled resultsetname was found (0 = no, 1 = yes)*/
    tsp00_Int4 hasResultset = 0; /* flag that signals that the statement 
                                    produce a resutlset */
    tpr00_sp1rFunctionCode sp1rFunctionCode; /* Function Code */
    tpr00_DescribeKindEnum descKind= Describe_Params_epr00;
    sqlorentry    *ore; 
    tpr00_ParseInfo *stmtParseInfo;
    tpr05_StringEncoding PacketEncoding;
    if (pr) 
      pr->prDescribe = cpr_is_init; /*needed for vpr04orc.c p04sfi()*/
 
    /*bind sqlda*/
    if (sqlca->sqldbmode == cpr_kind_internal){
      sqldatype *da = sqlca->sqlcxap->xasqldap;
      da->sqlkano   = sqlxa->xakano;
      da->sqlprno   = ka->kaprindex;
    }
    else if (sqlca->sqldbmode == cpr_kind_oracle || sqlca->sqldbmode == cpr_kind_sapr3) {
      SQLDA *da   = sqlca->sqlcxap->xasqldap;
      da->sqlkano = sqlxa->xakano;
      da->sqlprno = ka->kaprindex;
    }
    
    /*Determine corresponding ore-Area and ParseID*/
    if (      SQLDesc->StmtNameDesc->StmtTypeDesc.StmtType == KeywordFETCH_epr01
           || SQLDesc->StmtNameDesc->StmtTypeDesc.StmtType == KeywordFFETCH_epr01
           || SQLDesc->StmtNameDesc->StmtTypeDesc.StmtType == KeywordMFETCH_epr01 ){
      /*describe for a fetch command*/
      tpr01_CursorDesc *CursorDesc = SQLDesc->CursorDesc;
      if (CursorDesc){
        ore = CursorDesc->StmtNameDesc->ore;
        stmtParseInfo = &(CursorDesc->StmtNameDesc->ka->kaParseInfo);
        PacketEncoding = CursorDesc->StmtNameDesc->SQLStatement->encodingType;
      }
      else if (SQLDesc->StmtNameDesc->StmtTypeDesc.CursorLength == 0){
        /*unlabeled resultset*/      
        unlabledResultset = 1;
        sendDescOrder = 1;
        ore = SQLDesc->ore;
        stmtParseInfo = &(SQLDesc->ka->kaParseInfo);
        PacketEncoding = SQLDesc->PacketEncoding;
      }  
      else {
        p08runtimeerror (sqlca, sqlxa, cpr_unknown_cursor_name);      
        return;
      }
      if (  sqlca->sqldbmode == cpr_kind_internal 
          ||SQLDesc->ComType == cpr_com_ora_descr_sel){
        descKind = Describe_Columns_epr00;
      }
      else
        descKind = Describe_Params_epr00;    
    } 
    else {
      /*describe for prepared command*/ 
        ore = SQLDesc->ore;
        stmtParseInfo = &(SQLDesc->ka->kaParseInfo);
        PacketEncoding = SQLDesc->StmtNameDesc->SQLStatement->encodingType;
        if (  sqlca->sqldbmode == cpr_kind_internal
            ||SQLDesc->ComType != cpr_com_ora_descr_sel)
          descKind = Describe_Params_epr00;
        else {
          descKind = Describe_Columns_epr00;
        } 
    }    

    if  (   descKind == Describe_Columns_epr00 
         && ore->orrescntacc == 0 
         && ore->oropennew != cpr_is_describe)
      sendDescOrder = 1;
    if (stmtParseInfo->sp1rFunctionCode > csp1_masscmd_fc_offset) {
      sp1rFunctionCode = stmtParseInfo->sp1rFunctionCode - csp1_masscmd_fc_offset;
    }
    else {
      sp1rFunctionCode = stmtParseInfo->sp1rFunctionCode;
    }
    if ( sp1rFunctionCode == csp1_select_fc
         || sp1rFunctionCode == csp1_show_73_fc
         || sp1rFunctionCode == csp1_explain_fc
         || sp1rFunctionCode == csp1_cursor_pl_sql_execute_fc
         || (sp1rFunctionCode >= csp1_fetch_first_fc
             && sp1rFunctionCode <= csp1_fetch_same_fc)
         )
      hasResultset = 1;

    if ( sendDescOrder && hasResultset || unlabledResultset ) {
      /*built a describe order*/
      tpr01_ConContainer *Connection = SQLDesc->ConDesc->Connection;        
      tsp1_part *part_Ptr = gae->gareqptr;
      tpr_runtime_errors_epr00 erg = cpr_p_ok; /*return value of putPart-functions*/
      tsp00_Int4 free_part_space = 0;          /*free space in order packet*/
      tsp00_Int4 offset = 0;                   /*position for continue writing to part*/
      
      ka->kapacount = -4;

      Connection->InitPacket (SQLDesc->ConDesc, PacketEncoding, sp1m_dbs);

      /*find part*/
      part_Ptr = pr03PartFind(sqlra, sp1pk_command);
      if (part_Ptr == NULL) {
        part_Ptr = pr03SegmentAddPart(SQLDesc->ConDesc->SegmDesc, sp1pk_command);
      }
      free_part_space = pr03PartGetFreePartSpace(part_Ptr);
      erg = pr03PartConverttoPart(part_Ptr,
                                  &offset,
                                  &free_part_space,
                                  PacketEncoding,
                                  "DESCRIBE ",
                                  9,
                                  sp77encodingAscii);
      if (erg != cpr_p_ok) {
        pr01TraceRuntimeError(sqlca , sqlxa , erg);
      }
      pr03SegmentFinishPart(SQLDesc->ConDesc->SegmDesc);
      if (! unlabledResultset)
        p03cpparsid(sqlra, gae, (char*)stmtParseInfo->ParseId, sqlca->sqlemp);
      p03modulnameput (sqlca, sqlxa, gae, ka);
      
      /*sending decribe order to kernel*/
      pr03PacketReqRec (SQLDesc->ConDesc, sqlca->sqlemp);
      
      if ( sqlemp->ereturncode == 0) {
        pr04ColNames2ore (sqlca, sqlxa, ka, ore);
        p04SFInfo2oreRes(sqlca, sqlxa, ka, ore);
      }
      ore->oropennew = cpr_is_describe;
    }
      
    /*ore to sqlda*/
    p04or2da (sqlca, sqlxa, ore, descKind);

    /*Trace Output*/

    if (pr01TraceIsTrace(sqlra)) {
      if (descKind == Describe_Columns_epr00)
        pr01TracePrintf(sqlra, "\nDESCRIBE COLUMNS");
      else
        pr01TracePrintf(sqlra, "\nDESCRIBE PARAMETERS");
      p01pparsidtrace(sqlca, sqlxa, ka, (char*)stmtParseInfo->ParseId, TraceParseIDSQLStmt_epr00);
      p01pparsidtrace(sqlca, sqlxa, ka, (char*)stmtParseInfo->ParseId, TraceParseIDInput_epr00);
      if ( sqlemp->ereturncode != 0) 
        p03returncodeget (sqlca, sqlxa);
      else {
        if (descKind == Describe_Columns_epr00) 
          p04traceda(sqlra, sqlca->sqlcxap->xasqldap, ore->orresptr, sqlca->sqldbmode);
      }    
      p01xtimetrace(sqlca, sqlxa, gae);
    }
  }
  M90TRACE(M90_TR_EXIT, "pr01cDescribe", 0);
}

boolean pr01cIsComType(tpr01_SQLDesc *SQLDesc, tpr00_ComTypeEnum ComType)
{
  return (SQLDesc->ComType == ComType);
}

void pr01cStmtNameSetState(tpr01_StmtNameDesc *StmtNameDesc, tpr01_SQLStateBinary State)
{
  tpr01_StmtNameContainer *StmtName = StmtNameDesc->StmtName;
  StmtName->SetState(StmtNameDesc, State);
}

/*!
  Function:     pr01cParse

  see also: 

  Description:  parses a SQL statement by sending a parse order to database kernel
    
  Arguments:
    SQLDesc  [in|out] current SQL descripor 
    cmdfetch [in|out] 
    datafd   [in|out] 
    
  return value: true if execute is possible or 
                false if execute is not possible or the command is already executed by parsing 
*/
tsp00_Int4 pr01cParse(tpr01_SQLDesc *SQLDesc, tsp00_Int2 *cmdfetch, tsp00_Int4 *datafd)
{
  tpr01_SQLContainer *SQL = SQLDesc->SQL;
  sqlcatype *sqlca = SQL->GetSqlca(SQLDesc);
  sqlxatype *sqlxa = SQL->GetSqlxa(SQLDesc);
  sqlratype *sqlra = sqlca->sqlrap;
  sqlkaentry *ka   = SQLDesc->ka;
  sqlgaentry *gae  = SQLDesc->ConDesc->ga;
  sqlorentry *ore  = SQLDesc->ore;
  sqlcuentry *cue  = SQLDesc->cu;
  struct SQLERROR *sqlemp = sqlca->sqlemp;
  tsp00_Int4 mass_cmd = false, datafound = *datafd;
  boolean IsParsed = false;
  tpr00_ParseId *selparsid = NULL;  
#ifndef SQLODBC  
  /*storing KnlSessionID for later dropping of parseid*/
  pr06ParseIdKnlSessionIDCopy(&(SQLDesc->StmtNameDesc->KnlSessionID), &(SQLDesc->ConDesc->ga->gaKnlSessionID));
#endif
  /*checking if mass fetch should be used and initializing faentry*/
  p01bbparsen (sqlca, sqlxa, gae, ka, cmdfetch);

  if ( SQLDesc->Attribute.massopen
       && sqlra->rasqlansi != cpr_kind_ansi
       && sqlca->sqldbmode != cpr_kind_db2) {
    /*sets mass command flag in oder packet*/
    pr03SegmentSetAttribute(sqlra->rasegptr, Mass_Cmd_epr03, TRUE);
  }
  p01cmd_trace(sqlca, sqlxa, ka, gae);
  if (SQLDesc->ComType == cpr_com_ora_descr_sel) {
    /* describe select list kommando */
    p01pparsidtrace(sqlca, sqlxa, ka, ka->kaParseInfo.ParseId, TraceParseIDInput_epr00);
  }
  if (! pr01cCheckParseFetch(SQLDesc) ) {
    selparsid = &(SQLDesc->CursorDesc->OpenKa->kaParseInfo.ParseId);
    /*fake the parseid for mfetch and drop parseid routines*/
    pr06ParseIdSessionID2ParseID(&ka->kaParseInfo.ParseId, &(SQLDesc->ConDesc->ga->gaKnlSessionID));
    ka->kaParseInfo.ParseId[cpr_p_precom_index]=  csp1_p_mfetch_found; 
    p01pparsidtrace(sqlca, sqlxa, ka, ka->kaParseInfo.ParseId, TraceParseIDSQLStmt_epr00);
  }      
  else {
    /* no longer needed p01sbparsen (sqlca, sqlxa, gae, ka); */
    p03modulnameput (sqlca, sqlxa, gae, ka);
    pr03AbapInfoPutPart(sqlca, sqlra, gae);
    mass_cmd = (tsp00_Int4) pr03SegmentGetAttribute(sqlra->rasegptr, Mass_Cmd_epr03);
    pr03PacketReqRec (SQLDesc->ConDesc, sqlca->sqlemp);
  
    /* ADIS 1001311 */
    if ( sqlemp->ereturncode == 0) {
      if (! pr01cResultNameGet(sqlca->sqlrap, gae, sqlca->sqlresn, sizeof(sqlca->sqlresn))){
        /*command has no output columns i.e. INSERT
          set a flag in or-area that no further describe order is needed 
          */
        SQLDesc->ore->oropennew = cpr_is_describe;
      }  
      ka->kaParseInfo.sp1rFunctionCode = pr01sGetFunctionCode(sqlra);
      p03gparsid (sqlra, sqlemp, ka->kaParseInfo.ParseId, &gae->gaKnlSessionID);
      ka->kaparserrc = 0;
      p01pparsidtrace(sqlca, sqlxa, ka, ka->kaParseInfo.ParseId, TraceParseIDSQLStmt_epr00);
      if   (sqlemp->ewarning[0] != bsp_c1) {
        memcpy(&ka->kaparserrc, ((tsp1_packet*)gae->garecptr)->variant.C_2.sp1_segm_F.variant.C_1.sp1s_segm_header_F.variant.C_2.sp1r_extern_warning_F, 2);
      }
    }
  }/*end else pr01cCheckParseFetch */  

  if ( sqlemp->ereturncode != 0) {
      p03returncodeget (sqlca, sqlxa);
      p01xtimetrace (sqlca, sqlxa, gae);
      sqlxa->xacmdinit = cpr_is_false;
      IsParsed = false;
  }
  else {
    IsParsed = true;
    p01pparsidtrace(sqlca, sqlxa, ka, ka->kaParseInfo.ParseId, TraceParseIDOutput_epr00);
    /*checking if precompiler ca handle this command*/
    p08privicmd(sqlca, sqlxa, ka, cmdfetch);
    if ( sqlemp->ereturncode != 0) {
      p01pparsidtrace(sqlca, sqlxa, ka, ka->kaParseInfo.ParseId, TraceParseIDSQLStmt_epr00);
    }
    datafound = 0;
    if (sqlra->raparse_again == FALSE) {
      /*checking if open command has received result data and updateting faentry*/
      p01copendata(sqlca, sqlxa, ka, cue, gae, mass_cmd, &datafound, selparsid);
    }
    if (SQLDesc->ComType != cpr_com_sql_close)
    if (!datafound) {
      p04sfi (sqlca, sqlxa, Describe_Params_epr00);
      if ((sqlca->sqldbmode != cpr_kind_internal) && SQLDesc->CursorDesc)
        p01resallocinfo(sqlca, sqlxa, ka, SQLDesc->CursorDesc->StmtNameDesc->ore);
      /*looking for a corresponding mfetch buffer, adding a new buffer if not found*/
      p01baparsen (sqlca, sqlxa, gae, ka, cmdfetch);
    }
    /*consistence checks for select direct fast*/
    if (ka->kapacount < -3) {
      sqlxa->xacmdinit = cpr_is_false;
      p01xtimetrace (sqlca, sqlxa, gae);
      IsParsed = false;
    }
    else {
      if (ka->kaParseInfo.ParseId[cpr_p_precom_index] == csp1_p_command_executed) {
        /* kom. already executed */
        p03returncodeget (sqlca, sqlxa);
        p01xtimetrace (sqlca, sqlxa, gae);
        ka->kastate = cpr_state_command;
        sqlxa->xacmdinit = cpr_is_false;
        IsParsed = false;
      }
      else {
        if (ka->kaParseInfo.ParseId[cpr_p_precom_index] == csp1_p_use_adbs) {
          /* kom. use execute immed. */
          ka->kastate = cpr_state_command;
          p01pparsidtrace(sqlca, sqlxa, ka, ka->kaParseInfo.ParseId, TraceParseIDUseExecute_epr00);
          IsParsed = false;
        }
      }
    }
  }/*end if returncode != 0*/
  *datafd = datafound;
  return IsParsed;
}

sqlorentry *pr01cFromOpenGetOr(tpr01_SQLDesc *SQLDesc)
{
  if (SQLDesc && SQLDesc->CursorDesc)
    return SQLDesc->CursorDesc->StmtNameDesc->ore;
  else
    return NULL;

}

sqlkaentry *pr01cDupKa(sqlkaentry *kae)
{
  sqlkaentry *kaedup = pr03mAllocat(sizeof(sqlkaentry), "kae");
  if (kae) {
    memcpy(kaedup, kae, sizeof(sqlkaentry));
    if (kae->kafaindex > 0) {
      kaedup->kafae = pr03mAllocat(sizeof(sqlfaentry), "kae->kafae");
      memset(kaedup->kafae, 0, sizeof(sqlfaentry));
    }
    else {
      kaedup->kafae = NULL;
    }
  }
  else
    memset(kaedup, 0, sizeof(sqlkaentry));
  return kaedup;
}

void pr01cFreeKa(sqlkaentry *kae)
{
  if (kae->kafae) {
    pr03mFree(kae->kafae, "kae->kafae");
  }
  pr03mFree(kae, "kae");
}

/*!
  Function:     pr01cResultNameGet

  see also:

  Description:  Gets an UTF8 conforms resultname (cursoname) from the packet.
                If the given buffer szResultName is to short to hold the
                converted resultname the name will truncated to the right size.
                If no resultname is found the szResultname will fill up with
                blanks.

  Arguments:
    sqlrap  [in]  request area
    gae     [in]  connection area
    szResultName  [in|out] Buffer who is lager enougth to hold the resultame.
    cbLen   [in]  Maximum Length in bytes of szResultName

  return value: 1 if resulttablenamepart found
                0 if resulttablenamepart not found
 */
tsp00_Int4 pr01cResultNameGet(sqlratype *sqlrap, sqlgaentry *gae, tsp00_Byte *szResultName, int cbLen)
{
  tsp00_Uint4 destBufferLengthInBytes = cbLen;
  tsp00_Uint4 destBytesWritten = 0;
  tsp00_Int4 resTabFound = 0;

  if (gae->garecptr) {
    tpr05_StringEncoding packetEncoding = pr03PacketGetEncoding((tsp1_packet *)gae->garecptr);
    tsp1_part *partPtr = pr03PartFind(sqlrap, sp1pk_resulttablename);
    if (partPtr) {
      tsp78ConversionResult rc;
      tsp00_Uint4 srcBytesParsed;
      char *buf = (char*)partPtr->variant.C_1.sp1p_buf_F;
      tsp00_Uint4 srcBufferLengthInBytes = pr03PartGetPartLength(partPtr);
      rc = sp78convertString(sp77encodingUTF8, szResultName, destBufferLengthInBytes,
			     &destBytesWritten, FALSE,
			     packetEncoding, buf, srcBufferLengthInBytes,
			     &srcBytesParsed);
      resTabFound = 1;
    }
  }
  if (destBytesWritten < destBufferLengthInBytes) {
    /* fill up with blanks */
    void *FillBuf = szResultName + destBytesWritten;
    tsp00_Uint4  targetLength = (destBufferLengthInBytes - destBytesWritten);
    tsp00_Uint4  padLength = targetLength;
    sp77encodingUTF8->fillString(&FillBuf, &targetLength, padLength, ' ');
  }
  return resTabFound;
}

/* PTS 1108103 */
/* Aendert bei ParseId in allen erforderlichen Descriptoren nachdem der
   Kern das Kommando intern neu geparst hat. Der Kern hat zuvor den Fehlercode
   -9 geliefert und im ReturnSegment einen Parsidpart geliefert.
   Geandert werden muessen zusaetzlich die Shortfieldinfos in der or Area */
void pr01cUseNewParseInfo(tpr01_SQLDesc *SQLDesc)
{
  tsp00_Int2 mfindex;

  if (SQLDesc) {
    tpr01_SQLContainer *SQL = SQLDesc->SQL;
    sqlcatype *sqlca = SQL->GetSqlca(SQLDesc);
    sqlxatype *sqlxa = SQL->GetSqlxa(SQLDesc);
    sqlratype *sqlra = sqlca->sqlrap;
    sqlgaentry *ga = SQLDesc->ConDesc->ga;

    struct SQLERROR *sqlemp = sqlca->sqlemp;
    tpr00_ParseInfo NewParseInfo, OldParseInfo;
    NewParseInfo.sp1rFunctionCode = pr01sGetFunctionCode(sqlra);
    p03gparsid (sqlra, sqlemp, NewParseInfo.ParseId, &ga->gaKnlSessionID);
    pr06ParseInfoCopy(&OldParseInfo, &SQLDesc->ka->kaParseInfo);
    p01pparsidtrace (sqlca, sqlxa, SQLDesc->ka,
                     OldParseInfo.ParseId, TraceParseIDChange_epr00);
    p01pparsidtrace (sqlca, sqlxa, SQLDesc->ka, NewParseInfo.ParseId, TraceParseIDNew_epr00);
    mfindex = p01bmfentryget(sqlca, sqlxa, OldParseInfo.ParseId);
    if  (mfindex > 0) {
      memcpy(&sqlca->sqlmfp->mfentry[mfindex-1].mfSelparsid, &NewParseInfo.ParseId, ParseId_mxpr00 );
    }
    pr06ParseInfoCopy(&SQLDesc->ka->kaParseInfo, &NewParseInfo);

    p04GetSFInfo(sqlca, sqlxa, SQLDesc->ka, SQLDesc->ore);
    p01colinitsfinfo (sqlca, sqlxa, SQLDesc->ka, SQLDesc->ore);
    p01colmovesfinfo (sqlca, sqlxa, SQLDesc->ka, SQLDesc->ore);
    p01resallocinfo (sqlca, sqlxa, SQLDesc->ka, SQLDesc->ore);
    pr04ColNames2ore (sqlca, sqlxa, SQLDesc->ka, SQLDesc->ore);

    /* now change other the ParseId's */
    if (pr01TraceIsTrace(sqlra))
      pr01TracePrintf(sqlra, "CHANGE SQLDesc");

    if (SQLDesc->StmtNameDesc) {
      tpr01_StmtNameContainer *StmtName = SQLDesc->StmtNameDesc->StmtName;
      tpr01_StmtNameDesc *StmtNameDesc=NULL;
      
      StmtName->ResetIterator(StmtName);
      while(StmtNameDesc=StmtName->EnumDesc(StmtName)) {
        /*loop over all StmtNameDesc*/
        tpr01_ParseInfoContainer *ParseInfoCache = SQLDesc->ConDesc->ParseInfoCache;
        tpr00_ParseInfo *ParseInfo = &StmtNameDesc->ka->kaParseInfo;
	sqlsnentry *sn  = StmtNameDesc->sn;

        if (pr06ParseIdIsEqual(&OldParseInfo.ParseId, &ParseInfo->ParseId)) {
          /*StmtName with old parseid found -> update*/      
          pr06ParseInfoCopy(ParseInfo, &NewParseInfo);
          if (pr01TraceIsTrace(sqlra))
            pr01TracePrintf(sqlra, "CHANGE SQLDesc->StmtNameDesc");
          StmtName->TraceStmtName(SQLDesc->StmtNameDesc, sqlca);
	
	  /*update parseid in sn-entry*/
	  pr06ParseInfoCopy(&sn->snParseInfo, &NewParseInfo);
	  if (pr01TraceIsTrace(sqlra))
	    pr01TracePrintf(sqlra, "CHANGE SNSTMTNAME");
	  StmtName->TraceStmtName(StmtNameDesc, sqlca);
	  
	  /*update parseinfos in parse info cache*/
	  if (ParseInfoCache) {
            tpr01_ParseInfoDesc      *ParseInfoDesc  = NULL;      
            ParseInfoDesc = ParseInfoCache->FindDesc(ParseInfoCache, StmtNameDesc->SQLStatement, -1);
            if (ParseInfoDesc) {
              ParseInfoCache->PutParseInfos(ParseInfoDesc, SQLDesc->ore, SQLDesc->ka); 
            }
          }
	}
      }
    }

    if (SQLDesc->CursorDesc) {
      tpr00_ParseInfo *ParseInfo = &SQLDesc->CursorDesc->OpenKa->kaParseInfo;
      tpr01_StmtNameContainer *StmtName = SQLDesc->CursorDesc->StmtNameDesc->StmtName;
      if (pr06ParseIdIsEqual(&OldParseInfo.ParseId, &ParseInfo->ParseId)) {
	pr06ParseInfoCopy(ParseInfo, &NewParseInfo);
	StmtName->TraceStmtName(SQLDesc->StmtNameDesc, sqlca);
	if (pr01TraceIsTrace(sqlra))
	  pr01TracePrintf(sqlra, "CHANGE SQLDesc->CursorDesc");
      }
    }
  }
}

/*!
  Function:     pr01cSetLastDataReceived

  see also: 

  Description:  sets Cursor Attribut lastDataReceived
    
  If the execution of a command receives the complete resultset during
  the open cursor process the following fetch command must not be parsed.
  This function sets a Attribut "lastDataReceived" in the corresponding
  cursor descriptor which can be used to decide whether a fetch must be 
  parsed or not.
  
  Arguments:
    SQLDesc   [in|out] pointer to SQLDesc 
    aValue    [in]     value (0 or 1), to which the cursor attribut should be set  
    
  return value: pointer to the initialized tpr05_String or NULL (in case of error)
 */
void pr01cSetLastDataReceived (tpr01_SQLDesc *SQLDesc, tsp00_Int4 aValue) {
   tpr01_CursorDesc* CursorDesc = SQLDesc->CursorDesc;
   if (CursorDesc) {
     BOOLEAN myVal = (aValue)?TRUE:FALSE;
     CursorDesc->SetAttr (CursorDesc, lastDataReceived_epr01Cursor, &myVal, 1);
   }        
}

/*!
  Function:     p01copendata

  Arguments:
    sqlca    [in|out] sqlca area
    sqlxa    [in|out] sqlxa area  
    kae      [in]     current ka entry
    mass_cmd [in]     mass command flag
    found    [out]    data found flag    
    
  return value: void
 */
static void p01copendata(sqlcatype* sqlca, sqlxatype* sqlxa, sqlkaentry* kae, sqlcuentry* cue, sqlgaentry* gae, tsp00_Int4 mass_cmd, tsp00_Int4* found, tpr00_ParseId *selparsid) {
    tsp1_segment* segptr;
    sqlmfentry* mfentry;
    sqlorentry* ore;
    sqlfaentry* faentry;
    BOOLEAN     datafound = 0;    
    struct SQLERROR *sqlemp = sqlca->sqlemp;

    sqlratype *sqlra = sqlca->sqlrap;
    tpr00_ParseId tmpparsid;
    kae->kaopendata = 0;
    /*searching parsid_of_select part*/
    
    if (!selparsid) {
      p03gselparsid (sqlra, sqlemp, tmpparsid, &gae->gaKnlSessionID);
      selparsid = &tmpparsid;
      
    }
    if (selparsid) {
      if ( ! pr06ParseIdIsEqual(selparsid, (tpr00_ParseId*)CPR_PARSIDNULL) ) {
        int i= 1;
        p01pparsidtrace(sqlca, sqlxa, kae, *selparsid, TraceParseIDSelect_epr00);
        for (i=1; i <= sqlca->sqlmfetch; i++) {
          /*looking for corresponding mfentry in mass fetch buffer*/
          mfentry = &(sqlca->sqlmfp->mfentry[i + -1]);
          if (   pr06ParseIdIsEqual((tpr00_ParseId*)&(mfentry->mfSelparsid) , selparsid)
               &&mfentry->mfopendata == 1) {
            datafound = 1;
            kae->kaopendata = 1;
            faentry = (sqlfaentry *) kae->kafae;
            faentry->faselectno = mfentry->mfSelectno;
            faentry->famfindex = (short ) i;
            faentry->fapos = 0;
            faentry->faspec = 3;
            faentry->fafetch = 1;
            faentry->fareclen = (int ) mfentry->mfReclen;
            if (mass_cmd) {
              segptr = &( ((tsp1_packet*)mfentry->mfBufpointer)->variant.C_2.sp1_segm_F);
              segptr->sp1s_segm_header.sp1r_function_code = csp1_mfetch_next_fc;
            }
            break;  
          }
        }
      }
      if (datafound) {
        ore = pr01cFromOpenGetOr(sqlxa->xaSQLDesc);
        if (ore) {
          if (kae->kapaindex > 0) {
            p04sfi(sqlca ,sqlxa , 2);
          }
          else {
            BOOLEAN prepars;
            p01resinitsfinfo(sqlca, sqlxa, kae, ore, &(datafound));
            p01resmovesfinfo(sqlca, sqlxa, kae, ore, &prepars);
          }
        }
        if (cue){
          cue->cuerrd3 = 0;
        }
      }
    } /*end if part*/
    *found = (datafound)?1:0;
}

/*!
  Function:     pr01cCheckParseFetch

  see also: 

  Description:  checks if a fetch command should beparsed or not
    
  The FOrS optimization suppress the parsing of fetch commands if all
  resultdata was delivered durring the execution of SQL statement in the
  open cursor routine. This function checks if all criterions are fulfilled 
  to suppress the parsing of fetch.
  
  Arguments:
    SQLDesc   [in|out] pointer to SQLDesc 
    aValue    [in]     value (0 or 1), to which the cursor attribut should be set  
    
  return value: 1 if fetch must be parse
                0 if fetch must not be parsed
 */
static tsp00_Int4 pr01cCheckParseFetch (tpr01_SQLDesc *SQLDesc) 
{
   tpr01_CursorDesc* CursorDesc = SQLDesc->CursorDesc;
   tpr05_StmtAnalysisDesc* StmtTypeDesc = &SQLDesc->StmtNameDesc->StmtTypeDesc;
   tsp00_Int4 sz, ret = 1;
   BOOLEAN allDataOnOpen = FALSE;
   if (CursorDesc)
      CursorDesc->GetAttr (CursorDesc, lastDataReceived_epr01Cursor, &allDataOnOpen, &sz);
   if (allDataOnOpen
       && (   StmtTypeDesc->StmtType == KeywordFETCH_epr01
           || StmtTypeDesc->StmtType == KeywordFFETCH_epr01
           || StmtTypeDesc->StmtType == KeywordMFETCH_epr01 )
       && (   StmtTypeDesc->StmtOption == KeywordNOT_FOUND_epr01
           || StmtTypeDesc->StmtOption == KeywordNEXT_epr01)
      ) {
     ret = 0;
   }        
   return ret;
}

/*---------------------------------------------------------------------------*/

tsp00_Int4 pr01cGetVaIndex(sqlxatype *sqlxa, tsp00_Int4 index, tsp00_Uint1** pAddr)
{
  sqlcatype *sqlca=NULL;
  tsp00_Int4 cmcnt;
  void *vaaddr;
  tsp00_Int4 vaind;
  void *indaddr;
  tsp00_Int4 indind;

  M90TRACE(M90_TR_ENTRY, "pr01GetVaIndex", 0);
  p03getparameteraddr(sqlca, sqlxa, &index, &cmcnt, &vaaddr, &vaind, &indaddr, &indind);
  if (pAddr)
    *pAddr = vaaddr;

  M90TRACE(M90_TR_EXIT, "pr01GetVaIndex", 0);
  return vaind-1;
}

