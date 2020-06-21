/*================================================================
 module:    vpr04Long.c

 responsible: BurkhardD, ThomasS
 
 special area:   CPCDrv | Client | Longvarchar
 
 description: Longvarchar Conversion 
 
 see:  
 
 change history:

        2001-06-26 D025086 PTS 1110697
        To close the longdesc and end the internal kernel subtrans send
        at least one descriptor with last_putval has set.         

        2001-05-17 D025086 Use pr03PacketReqRec instead of p03reqrecpacket
        to protect unexpected communication errors.

        2001-03-23 D025086 MUT faild with -7065. 
        Set valmode to vm_alldata in pr04LongPutDescPos to prevent to 
        close a longdesc twice.


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
#include <stddef.h>
#include "vpr01SQL.h"
#include "vpr03Packet.h"
#include "vpr01Con.h"
#include "vpr04Long.h"
#include "gsp05.h"
#include "gpr03.h"
#include "gpr04.h"
#include "gpr08.h"
#include "hsp26.h"
#include "hsp30.h"
#include "hsp78_0.h"
#include "vpr01Module.h"
#include "vpr01Trace.h"
#include "vpr07Macro.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

boolean pr04LongIsPieceWise(boolean Input);

void p05int4tochr12 (tsp00_Int4, tsp00_C12*);

void p03cresultcountget (sqlrapointer, sqlgaentry*, tsp00_Int4*);

void p03datafread (sqlrapointer, tsp00_Int4, tsp00_Int4*,
			   tsp00_MoveObj*, tsp00_Int4, tsp00_Int2*,
			   struct SQLERROR*);

void p03datafclose (sqlrapointer, tsp00_Int4*, struct SQLERROR*);

void p03datafwrite (sqlratype*, tsp00_Int4, tsp00_Int4, char*,
			    tsp00_Int4, struct SQLERROR*);

void sqlnul (short *, short *, int *);

boolean p04isasciidata (const tsp00_DataType datatype);
boolean p04isunidata (const tsp00_DataType datatype);
tpr05_StringEncoding pr04cGetBufEncoding(const struct SQLCOL *this);

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

static void pr04LongFileNameTrace(sqlratype *sqlra, tsp00_VFilename *fnam, tsp00_Int4 flen);
static struct SQLLD *pr04LongLDAlloc();
static void pr04LongDeleteLD(struct SQLLD *ld);
static tpr05_StringEncoding pr04LongGetBufEncoding(const tsp00_DataType datatype);

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

static char *pr04LongAddr (sqlxatype *sqlxa, sqlv1pointer va, tsp00_Int4 lpind)
{
  char *addr = (va->pt.va1indi == SQLVAPT) ?
    *((char **) va->pt.va1addr + lpind) :
    (va->st.va1indi == SQLVAST) ?
    (char *) va->st.va1addr + lpind * va->st.va1size :
    (va->sc.va1indi == SQLVASC) ?
    (char *) va->sc.va1addr +
    lpind * sqlxa->sqlv2p[va->sc.va1indva2 - 1].va2size :
    0;
  
  return addr;
}

static int pr04LongGetHostAddr (sqlcatype *sqlca, sqlxatype *sqlxa, int actrow, int actcol,
                                  tsp00_Uint1 **varaddr, tsp00_Int4 *va2size, tsp00_Int2 *va2typ,
                                  tsp00_Uint1 **indaddr)
{
  tsp00_Int4 vaindex, idx, cmcnt, varind, indind;
  sqlkaentry *ka = sqlxa->xaSQLDesc->ka;
  sqlv1pointer va1, iva1;

  idx = ka->kapaindex + actcol;
  if (sqlxa->sqlpap[ka->kapaindex-1].lo.pakind == SQLPARLO)
    idx++;
  p03getparameteraddr (sqlca, sqlxa, &idx, &cmcnt, (void*)varaddr, &varind, (void*)indaddr, &indind);
  if (varind > 0) {
    va1 = sqlxa->sqlv1p + varind - 1;
    *varaddr = (unsigned char *) pr04LongAddr (sqlxa, va1, actrow);
  } else {
    *varaddr = 0;
  }

  if (indind > 0) {
    iva1 = sqlxa->sqlv1p + indind - 1;
    *indaddr = (unsigned char *) pr04LongAddr (sqlxa, iva1, actrow);
  } else {
    *indaddr = 0;
  }
  vaindex = varind - 1;
  *va2typ = sqlxa->sqlv2p[sqlxa->sqlv1p[vaindex].sc.va1indva2-1].va2typ;
  *va2size = sqlxa->sqlv2p[sqlxa->sqlv1p[vaindex].sc.va1indva2-1].va2size;

  return 1;
}

static bool pr04LongCheckParamNumber (sqlcatype *sqlca, sqlxatype *sqlxa) 
{
  int dan, orcnt;

  if (sqlca->sqlgap->gacmpkind == CPR_LA_ODBC)
    return true;

  if (sqlxa->xaSQLDesc->ka->katyp == cpr_com_sql_putval) {
    orcnt = sqlca->sqlrap->rasqlldp->ldorelastcmd->orcolcntacc;
  }
  if (sqlxa->xaSQLDesc->ka->katyp == cpr_com_sql_getval) {
    if ((orcnt = sqlca->sqlrap->rasqlldp->ldorelastcmd->orrescntacc) == 0)
      orcnt = sqlca->sqlrap->rasqlldp->ldorelastcmd->orcolcntacc;      
  }
  if (sqlxa->xaSQLDesc->ka->kapacount <= 0) {
    switch (sqlca->sqldbmode) {
    case cpr_kind_oracle:
    case cpr_kind_sapr3: {
      struct SQLDA *da = (struct SQLDA *) sqlca->sqlcxap->xasqldap;
      dan = da->F;
      break; }
    case cpr_kind_internal: {
      sqldatype *da = (sqldatype *) sqlca->sqlcxap->xasqldap;
      dan = da->sqln;
      break; }
    default:
      dan = 0;
      break;
    }
  } else {
    dan = sqlxa->xaSQLDesc->ka->kapacount;
    if (sqlxa->sqlpap[sqlxa->xaSQLDesc->ka->kapaindex-1].lo.pakind == SQLPARLO)
      dan--;
  }
  if ((sqlxa->xaSQLDesc->ka->katyp == cpr_com_sql_putval) ||
      (sqlxa->xaSQLDesc->ka->katyp == cpr_com_sql_getval)) {
    if (dan < orcnt) {
      pr01TraceRuntimeError (sqlca, sqlxa, cpr_paramlist_too_short);
      return false;
    }
    if (dan > orcnt) {
      pr01TraceRuntimeError (sqlca, sqlxa, cpr_paramlist_too_long);
      return false;
    }
  }

  return true;
}

static struct SQLLD *pr04LongNewLD (sqlcatype *sqlca, sqlxatype *sqlxa)
{
  struct SQLLD *ld = (struct SQLLD *) pr03mAllocat(sizeof(*ld), "ld");
  if (!ld) {
    p08runtimeerror (sqlca, sqlxa, cpr_memory_allocation_faild);
    return NULL;
  }
  ld->ldrestbufp = (struct SQLRESTBUF *)pr03mAllocat(sizeof(*ld->ldrestbufp), "ldrestbufp");
  if (!ld->ldrestbufp) {
    p08runtimeerror (sqlca, sqlxa, cpr_memory_allocation_faild);
    return NULL;
  }
  /* init ldrestbufp PTS 1102602 */
  ld->ldrestbufp->rbbufpart    = NULL;
  ld->ldrestbufp->rbcolno      = 0;
  ld->ldrestbufp->rbbufsize    = 0;
  ld->ldrestbufp->rbrestlen    = 0;
  ld->ldrestbufp->rbhostlensum = 0;
  ld->ldrestbufp->rblongcolmax = 0;
    
  ld->ldpc = NULL;
  ld->ldsp = NULL;
  /* init ldpc_mdesc */
  ld->ldpc_mdesc.descMaxelem  = 0;
  ld->ldpc_mdesc.descNelem    = 0;
  ld->ldpc_mdesc.descElemSize = sizeof (struct SQLPCLD);
  ld->ldpc_mdesc.descIncr     = sqlldmx;
  ld->ldpc_mdesc.descElemPtr  = NULL;
  /* init ldsp_mdesc */
  ld->ldsp_mdesc.descMaxelem  = 0;
  ld->ldsp_mdesc.descNelem    = 0;
  ld->ldsp_mdesc.descElemSize = sizeof (struct SQLSPLD);
  ld->ldsp_mdesc.descIncr     = sqlldmx;
  ld->ldsp_mdesc.descElemPtr  = NULL;

  ld->ldpcarr = NULL;
  ld->ldsparr = NULL;
  /* init ldpcarr_mdesc */
  ld->ldpcarr_mdesc.descMaxelem  = 0;
  ld->ldpcarr_mdesc.descNelem    = 0;
  ld->ldpcarr_mdesc.descElemSize = sizeof (struct SQLPCLD);
  ld->ldpcarr_mdesc.descIncr     = sqlldmx;
  ld->ldpcarr_mdesc.descElemPtr  = NULL;
  /* init ldsparr_mdesc */
  ld->ldsparr_mdesc.descMaxelem  = 0;
  ld->ldsparr_mdesc.descNelem    = 0;
  ld->ldsparr_mdesc.descElemSize = sizeof (struct SQLSPLD);
  ld->ldsparr_mdesc.descIncr     = sqlldmx;
  ld->ldsparr_mdesc.descElemPtr  = NULL;        
  return ld;
}

static void pr04LongDeleteLD(struct SQLLD *ld)
{
  if (ld->ldrestbufp) {
    pr03mFree(ld->ldrestbufp, "ld->ldrestbufp");
    ld->ldrestbufp = NULL;
  }
  pr03mFree(ld, "ld");
  ld = NULL;
}

static boolean pr04LongIsAsciiHostvar (struct SQLPCLD *ldpc)
{
  if ((ldpc->ldhosttype == cpr_vchar) ||
      (ldpc->ldhosttype == cpr_vansichar) ||
      (ldpc->ldhosttype == cpr_vcharc) ||
      (ldpc->ldhosttype == cpr_vcharz) ||
      (ldpc->ldhosttype == cpr_vansicharc) ||
      (ldpc->ldhosttype == cpr_vstring) ||
      (ldpc->ldhosttype == cpr_vstringc) ||
      (ldpc->ldhosttype == cpr_vstring1) ||
      (ldpc->ldhosttype == cpr_vstring4) ||
      (ldpc->ldhosttype == cpr_vvarchar_without_len))
    return true;
  else
    return false;
}

#ifndef SQLODBC
static boolean pr04LongIsUnicodeHostvar (struct SQLPCLD *ldpc)
{
  if ((ldpc->ldhosttype == cpr_vunicode) ||
      (ldpc->ldhosttype == cpr_vunicodec) ||
      (ldpc->ldhosttype == cpr_vucs2) ||
      (ldpc->ldhosttype == cpr_vutf16) ||
      (ldpc->ldhosttype == cpr_vstringunicode) ||
      (ldpc->ldhosttype == cpr_vstringunicode4))
    return true;
  else
    return false;
}
#endif

static tpr05_StringEncoding pr04LongGetBufEncoding(const tsp00_DataType datatype)
{
  struct SQLCOL coldb;
  coldb.coltype = datatype;
  coldb.colisuni = p04isunidata(datatype);
  return pr04cGetBufEncoding(&coldb);
}

static boolean pr04LongIsSQL_IGNORE(struct SQLPCLD *ldpc)
{
  if (ldpc->ldindktype == CPR_VINT2) {
    if (ldpc->ldindkaddr != 0) {
      tsp00_Int2 tmp;
      memcpy (&tmp, ldpc->ldindkaddr, sizeof (tsp00_Int2));
      return (tmp == SQL_IGNORE);
    } else {
      return false;
    }
  } else if (ldpc->ldindktype == CPR_VINT4) {
    if (ldpc->ldindkaddr != 0) {
      tsp00_Int4 tmp;
      memcpy (&tmp, ldpc->ldindkaddr, sizeof (tsp00_Int4));
      return (tmp == SQL_IGNORE);
    } else {
      return false;
    }
  }  
  return false;
}

static boolean pr04LongIsSQL_DEFAULT(struct SQLPCLD *ldpc)
{
  if (ldpc->ldindktype == CPR_VINT2) {
    if (ldpc->ldindkaddr != 0) {
      tsp00_Int2 tmp;
      memcpy (&tmp, ldpc->ldindkaddr, sizeof (tsp00_Int2));
      return (tmp == SQL_DEFAULT_PARAM);
    } else {
      return false;
    }
  } else if (ldpc->ldindktype == CPR_VINT4) {
    if (ldpc->ldindkaddr != 0) {
      tsp00_Int4 tmp;
      memcpy (&tmp, ldpc->ldindkaddr, sizeof (tsp00_Int4));
      return (tmp == SQL_DEFAULT_PARAM);
    } else {
      return false;
    }
  }  
  return false;
}


void pr04LongInitLD(sqlcatype *sqlca, sqlxatype *sqlxa)
{
  int i;
  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp;
  
  if (*ld) {
    for (i = 0; i < (*ld)->ldmaxo; i++) {
      struct SQLPCLD *ldpcact = &(*ld)->ldpc[i];
      if (ldpcact->ldrestdata) {
        if (ldpcact->ldrestdata->rbbufpart) {
          pr03mFree(ldpcact->ldrestdata->rbbufpart, 
                    "ldpcact->ldrestdata->rbbufpart");
          ldpcact->ldrestdata->rbbufpart = NULL;
        }
        pr03mFree (ldpcact->ldrestdata,
                   "ldpcact->ldrestdata");
        ldpcact->ldrestdata = NULL;
      }
    }
  } 

  if (*ld == NULL) {
    if ((*ld = pr04LongNewLD (sqlca, sqlxa)) == NULL)
      return;
  }
#ifndef SQLODBC
  (*ld)->ldretmaxi = 0;
  (*ld)->ldretmaxo = 0;
  (*ld)->lderrd3 = 0;
  (*ld)->ldretindpc = 0;
  (*ld)->ldretindsp = 0;
#endif
  (*ld)->ldmaxi = 0;
  (*ld)->ldmaxo = 0;
  (*ld)->ldindpc = 0;
  (*ld)->ldindsp = 0;
  (*ld)->ldcolkind = 0;
  (*ld)->ldputvalsend = 0;
  if ((sqlca->sqldbmode != cpr_kind_internal) && sqlxa->xaSQLDesc && sqlxa->xaSQLDesc->CursorDesc)
    (*ld)->ldorelastcmd = sqlxa->xaSQLDesc->CursorDesc->StmtNameDesc->ore;
  else
    (*ld)->ldorelastcmd = sqlxa->xaSQLDesc->ore;
}

void pr04LongLvcInitRec (sqlcatype *sqlca, tpr_longvarcharrec *lvcrec)
{
  lvcrec->lvccolno = 0;
  lvcrec->lvcdescno = 0;
  lvcrec->lvcfreepos = 0;
  lvcrec->lvcnext = 0;
  lvcrec->lvcvalmode = 0;
  lvcrec->lvcoldpos = 0;
  lvcrec->lvcoldlen = 0;
  lvcrec->lvcvalpos = 0;
  lvcrec->lvcvallen = 0;
  lvcrec->lvcvalmlen = 0;
  lvcrec->lvchostpos = 0;
  lvcrec->lvchostlen = 0;
  lvcrec->lvcaoffset = 0;
  lvcrec->lvcerror = 0;
  if (sqlca->sqlgap->gacmpkind == CPR_LA_ODBC)  
    lvcrec->lvcpiecewise = true;
  else
    lvcrec->lvcpiecewise = false;  
}

void pr04LongGetHostInfo(sqlcatype *sqlca, sqlxatype *sqlxa, tpr_longvarcharrec *lvcrec, char inp)
{
  tpr_runtime_errors_Enum error;
  tsp00_Int2 sint;
  tsp00_Int4 int4;
  int maxlen;
  tsp00_VFilename filename;
  tsp00_Int4 filelen;
  tsp05_RteFileMode read_write;
  struct SQLLD *sqlldp = sqlca->sqlrap->rasqlldp;
  struct SQLPCLD *ldpc = &sqlldp->ldpc[sqlldp->ldindpc-1];
  error = cpr_p_ok;
  lvcrec->lvchostlen = ldpc->ldhostlen;
  lvcrec->lvchostpos = 1;
  lvcrec->lvccolno = ldpc->ldcolno;
  lvcrec->lvcaoffset = 0;
  lvcrec->lvcnext = 1;
  lvcrec->lvcindicator = 0;
  lvcrec->lvcvalmlen = 0;
  ldpc->ldfileno = 0;

  if (inp) {
    lvcrec->lvcindicator = false;
  }
  switch (ldpc->ldhosttype) {
  case (cpr_vstring) : {
    if (inp || ldpc->ldhostlen == 0) {
      memcpy(&sint, ldpc->ldhostaddr, sizeof(sint));
      if (sint >= 0) {
	lvcrec->lvchostlen = sint;
      }
      else {
	/* 2**16 - sint */
	lvcrec->lvchostlen = sint + 65536;
      }
      lvcrec->lvcaoffset = 2;
    }
    else {
      lvcrec->lvcaoffset = 2;
      lvcrec->lvchostlen = ldpc->ldhostlen;
    }
    break;
  }
  case (cpr_vstring4) : {
    if (inp || ldpc->ldhostlen == 0) {
      memcpy(&int4, ldpc->ldhostaddr, sizeof(int4));
      lvcrec->lvchostlen = int4;
      lvcrec->lvcaoffset = 4;
    }
    else {
      lvcrec->lvcaoffset = 4;
      lvcrec->lvchostlen = ldpc->ldhostlen + -4;
    }
    break;
  }
  case (cpr_vstring1) : {
    if (inp || ldpc->ldhostlen == 0) {
      sint = ((char*)ldpc->ldhostaddr)[0];
      lvcrec->lvchostlen = sint;
      lvcrec->lvcaoffset = 1;
    }
    else {
      lvcrec->lvcaoffset = 1;
      lvcrec->lvchostlen = ldpc->ldhostlen + -1;
    }
    break;
  }
  case (cpr_vcharc) :
  case (cpr_vcharz) : {
    if ((ldpc->ldhostlen == 0) && /* (ldpc->ldindktype != SQL_IGNORE) */ !pr04LongIsSQL_IGNORE(ldpc)) {
      /* pointer = maxint4 length */
      maxlen = csp_maxint4;
    }
    else {
      maxlen = ldpc->ldhostlen;
    }
    if (ldpc->ldsfiotype == csp_info_input) {
      ldpc->ldhostlen = s30len(ldpc->ldhostaddr , (unsigned char)'\0' , maxlen);
    }
    else {
      ldpc->ldhostlen = maxlen;
    }
    if (ldpc->ldhostlen < 0) {
      ldpc->ldhostlen = 32767 - ldpc->ldhostlen;
    }
    lvcrec->lvchostlen = ldpc->ldhostlen;
    break;
  }
  case (cpr_vchar) :
  case (cpr_vbchar) :
  case (cpr_vbuf) :
  case (cpr_vbyte) : {}
  case (cpr_vdbchar) : {}
  case (cpr_vdbcharc) : {}
  case (cpr_vdbyte) : {}
  case (cpr_vraw) : {}
  case (cpr_vrawc) : {}
  case (cpr_vunicode) : {}
  case (cpr_vunicodec) : {
    if ((ldpc->ldhostlen == 0)) {
      if (!pr04LongIsSQL_IGNORE(ldpc)) {
        /* pointer */
        if (ldpc->ldsfiotype == csp_info_input) {
          error = cpr_incompatible_datatype;
        }
        else {
          /* pointer = maxint4 length */
          ldpc->ldhostlen = csp_maxint4;
        }
      }
    }
    else {
      if (ldpc->ldhostlen < 0) {
	ldpc->ldhostlen = csp_maxint2 - ldpc->ldhostlen;
      }
    }
    lvcrec->lvchostlen = ldpc->ldhostlen;
    break;
  }
  case (cpr_vlong_desc) : {
    sqllongdesc *pLongDesc = (sqllongdesc *)ldpc->ldhostaddr;
    /* ignore ldhostlen this is the size of longdesc */
    if (/* (ldpc->ldindktype != SQL_IGNORE) */ !pr04LongIsSQL_IGNORE(ldpc) && 
	(ldpc->ldhostlen != sizeof(sqllongdesc))) {
      error = cpr_incompatible_datatype;      
    }
    else {
      ldpc->ldhostaddr = pLongDesc->szBuf;
      ldpc->ldhostlen = pLongDesc->cbBufMax; 
      lvcrec->lvchostlen = ldpc->ldhostlen;
    }
    break;
  }
  case (CPR_VSTRINGUNICODE) : {
    if (inp || ldpc->ldhostlen == 0) {
      memcpy(&sint, ldpc->ldhostaddr, sizeof(sint));
      if (sint >= 0) {
	lvcrec->lvchostlen = sint;
      }
      else {
	/* 2**16 - sint */
	lvcrec->lvchostlen = sint + 65536;
      }
      lvcrec->lvcaoffset = 2;
    }
    else {
      lvcrec->lvcaoffset = 2;
      lvcrec->lvchostlen = ldpc->ldhostlen -2;
    }
    break;
  }
  case (CPR_VSTRINGUNICODE4) : {
    if (inp || ldpc->ldhostlen == 0) {
      memcpy(&int4, ldpc->ldhostaddr, sizeof(int4));
      lvcrec->lvchostlen = int4;
      lvcrec->lvcaoffset = 4;
    }
    else {
      lvcrec->lvcaoffset = 4;
      lvcrec->lvchostlen = ldpc->ldhostlen + -4;
    }    
    break;
  }
  case (cpr_vfile) : {}
  case (cpr_vfilec) : {
    memset(filename, bsp_c1, sizeof(filename));
    if (ldpc->ldhostlen == 0) {
      filelen = s30len(ldpc->ldhostaddr, '\0',  sizeof(filename));
    }
    else {
	if (ldpc->ldhosttype == cpr_vfile) {
	  filelen = s30lnr(ldpc->ldhostaddr, ' ', 1, ldpc->ldhostlen);
	}
	else {
	  filelen = s30len(ldpc->ldhostaddr, '\0', ldpc->ldhostlen);
	}
      }
    if (filelen > sizeof(filename)) {
      filelen = sizeof(filename);
    }
    memcpy(filename, ldpc->ldhostaddr, filelen);
    if (inp) {
      read_write = sp5vf_read;
    }
    else {
      read_write = sp5vf_append;
    }
    pr04LongFileNameTrace(sqlca->sqlrap , &filename , filelen);
    /* longcolumn length = maxint4 length */
    ldpc->ldhostlen = csp_maxint4 - 1;
    lvcrec->lvchostlen = ldpc->ldhostlen;
    p03datafopen(sqlca->sqlrap , filename , &ldpc->ldfileno , read_write , sqlca->sqlemp);
    break;
  }
  default : {
    error = cpr_incompatible_datatype;
    break;
  }
  }
  if (error != cpr_p_ok) {
    p04err(sqlca->sqlrap , sqlca->sqlemp , (char)error);
  }
}

static void pr04LongGetHostInfoOff(sqlcatype *sqlca, sqlxatype *sqlxa, int *lvcoffset)
{
  tpr_longvarcharrec lvcrec;

  pr04LongGetHostInfo(sqlca, sqlxa, &lvcrec, false);
  *lvcoffset = lvcrec.lvcaoffset;
}


static void pr04LongFileNameTrace(sqlratype *sqlra, tsp00_VFilename *fnam, tsp00_Int4 flen)
{
  sqltatype *sqlta  = sqlra->rasqltap;
  if ((int ) sqlta->tatrout == 3 || (int ) sqlta->tatrout == 5) {
    strcpy(sqlta->tastr80, "SQLFNAM: \"");
    strncat(sqlta->tastr80, (const char *) *fnam, flen);
    strcat(sqlta->tastr80,"\"");
    sqlta->tastr80l = (tsp00_Int4) strlen(sqlta->tastr80);
    p08vfwritetrace(sqlra);
  }
}

boolean pr04LongIsPieceWise(boolean Input)
{
#if defined (SQLODBC) || defined (SQLOCI)
    return true;
#else
    if (Input) {
      return false;
    }
    else {
      char *p;
      p = getenv("SAPDBPIECEWISE");
      if (p) 
	return true;
      else
	return false;
    }
#endif
}


/******************************************************************************
 *
 * pr04LongTraceInit: This function starts a new trace output for fetch / getval
 * command.
 *
 ******************************************************************************/

void pr04LongTraceInit (sqlcatype *sqlca, int trtxt, tsp00_Lname *vnam, int colno)
{
  tsp00_Namec nam;
  sqltatype *rasqltap = sqlca->sqlrap->rasqltap;

  if ((rasqltap->tatrout == cpr_trace_long) ||
      (rasqltap->tatrout == cpr_trace_modlong)) {
    if (trtxt == cpr_tr_loop) {
      strcpy ((char *) nam, "------------------");
      strncpy (rasqltap->tastr80, (const char *) nam, 9);
      rasqltap->tastr80l = 9;
      p08vfwritetrace (sqlca->sqlrap);
    }
    p04trvinit ((const char *) *vnam, colno, rasqltap->tastr80, &rasqltap->tastr80l, trtxt);
    p08vfwritetrace (sqlca->sqlrap);
  } 
}


/******************************************************************************
 *
 * pr04LongPutChr12: This function converts an integer to a string.
 *
 ******************************************************************************/

void pr04LongPutChr12 (tsp00_Int4 *ind, tsp00_Int2 *strl, char *str80)
{
  tsp00_C12 chr12;
  int i, n, len;

  p05int4tochr12 (*ind, &chr12);
  if (*ind < 100000)
    n = 5;
  else {
    if (*ind < 10000000)
      n = 7;
    else
      n = 10;
  }
  len = n + 3;
  for (i = 12; i >= 2; i--) {
    if (chr12[i-1] != bsp_c1) {
      str80[*strl+n-1] = chr12[i-1];
      n--;
    }
  }
  *strl += len;
}


/******************************************************************************
 *
 * pr04LongTraceFromTo: This function traces actual position value of 
 * hostvariable.
 *
 ******************************************************************************/

void pr04LongTraceFromTo (sqlcatype *sqlca, int trart, tsp00_Int4 fromind, tsp00_Int4 mlen)
{
  tsp00_Namec nam;
  tsp00_Int4 ind;
  sqltatype *rasqltap = sqlca->sqlrap->rasqltap;

  if (((rasqltap->tatrout == cpr_trace_long) ||
       (rasqltap->tatrout == cpr_trace_modlong)) &&
      ((sqlca->sqlemp->eprerr != cpr_datafile_open_error) &&
       (sqlca->sqlemp->eprerr != cpr_datafile_write_error) &&
       (sqlca->sqlemp->eprerr != cpr_datafile_read_error) &&
       (sqlca->sqlemp->eprerr != cpr_datafile_close_error))) {
    if (mlen > 0) {
      memset (rasqltap->tastr80, ' ', mxsp_c256);
      switch (trart) {
      case cpr_tr_inp:
	strcpy ((char *) nam, "INPUT  :          ");
	break;
      case cpr_tr_outp:
	strcpy ((char *) nam, "OUTPUT :          ");
	break;
      default:
	strcpy ((char *) nam, "------ :          ");
	break;
      }
      strncpy (rasqltap->tastr80, (const char *) nam, 9);
      rasqltap->tastr80l = 9;
      strcpy ((char *) nam, "LONG VARCHAR FROM ");
      strncpy (rasqltap->tastr80 + rasqltap->tastr80l, (const char *) nam, mxsp_name);
      rasqltap->tastr80l += mxsp_name;
      strcpy ((char *) nam, "TO POSITION :     ");
      strncpy (rasqltap->tastr80 + rasqltap->tastr80l, (const char *) nam, mxsp_name);
      rasqltap->tastr80l += mxsp_name;
      ind = fromind;
      pr04LongPutChr12 (&ind, &rasqltap->tastr80l, rasqltap->tastr80);
      rasqltap->tastr80[rasqltap->tastr80l-2] = '-';
      ind = fromind + mlen - 1;
      pr04LongPutChr12 (&ind, &rasqltap->tastr80l, rasqltap->tastr80);
      p08vfwritetrace (sqlca->sqlrap);
    }
  }
}


/******************************************************************************
 *
 * pr04LongFirstFromTo: This function traces out the actual long value in
 * hostvariable.
 *
 ******************************************************************************/

void pr04LongFirstFromTo (sqlcatype *sqlca, tsp1_param_info *sfinfo,
			  char *s, tsp00_Int4 pos)
{
  tsp00_Int4 dpos;
  tsp00_Namec nam;
  struct SQLCOL coldb;
  struct SQLROW rowdb;
  tsp00_Int2 iolen;
  tsp00_Int2 maxoutlen;

  sqltatype *rasqltap = sqlca->sqlrap->rasqltap;

  if (sfinfo->sp1i_length > 0) {
    if (((rasqltap->tatrout == cpr_trace_long) ||
	 (rasqltap->tatrout == cpr_trace_modlong)) &&
	((sqlca->sqlemp->eprerr != cpr_datafile_open_error) &&
	 (sqlca->sqlemp->eprerr != cpr_datafile_write_error) &&
	 (sqlca->sqlemp->eprerr != cpr_datafile_read_error) &&
	 (sqlca->sqlemp->eprerr != cpr_datafile_close_error))) {
      maxoutlen = 20;
      memset (rasqltap->tastr80, ' ', mxsp_c256);
      if (sfinfo->sp1i_length >= maxoutlen) {
	sfinfo->sp1i_length = maxoutlen;
	sfinfo->sp1i_in_out_len = maxoutlen + 1;
	strcpy ((char *) nam, "VALUE  :FIRST PART");
      } else
	strcpy ((char *) nam, "VALUE  :          ");
      strncpy (rasqltap->tastr80, (const char *) nam, mxsp_name);
      rasqltap->tastr80[47] = ':';
      rasqltap->tastr80l = 49;
      dpos = pos - 1;
      p04init (&rowdb);
      iolen = p04decode (sfinfo, &rowdb, &coldb);
      coldb.colislong = false;
      p04ptrvwrite (sqlca->sqlrap, &coldb, s, dpos);
      p08vfwritetrace (sqlca->sqlrap);
    }
  }
}


/******************************************************************************
 *
 * pr04LongTraceHostLen: This function traces out the length of the actual
 * hostvariable.
 *
 ******************************************************************************/

void pr04LongTraceHostLen (sqlcatype *sqlca, int varlen, tsp00_Int4 hostsum)
{
  tsp00_Namec nam;
  tsp00_Int4 ind;

  sqltatype *rasqltap = sqlca->sqlrap->rasqltap;

  if ((rasqltap->tatrout == cpr_trace_long) ||
      (rasqltap->tatrout == cpr_trace_modlong)) {
    memset (rasqltap->tastr80, ' ', mxsp_c256);
    strcpy ((char *) nam, "HOSTVAR LENGTH :  ");
    strncpy (rasqltap->tastr80, (const char *) nam, mxsp_name);
    rasqltap->tastr80l = mxsp_name;
    ind = varlen;
    pr04LongPutChr12 (&ind, &rasqltap->tastr80l, rasqltap->tastr80);
    if (hostsum > 0) {
      strcpy ((char *) nam, "      LONGVAR LENG");
      strncpy (rasqltap->tastr80 + rasqltap->tastr80l, (const char *) nam, mxsp_name);
      rasqltap->tastr80l += mxsp_name;
      strcpy ((char *) nam, "TH :              ");
      strncpy (rasqltap->tastr80 + rasqltap->tastr80l, (const char *) nam, mxsp_name);
      rasqltap->tastr80l += 6;
      ind = hostsum;
      pr04LongPutChr12 (&ind, &rasqltap->tastr80l, rasqltap->tastr80);
    }
    p08vfwritetrace (sqlca->sqlrap);
  }
}


/******************************************************************************
 *
 * pr04LongTraceValueLength: This function traces the length of the data stored
 * in the actual hostvariable.
 *
 ******************************************************************************/

void pr04LongTraceValueLength (sqlcatype *sqlca, int vallen)
{
  tsp00_Namec nam;
  tsp00_Int4 ind;

  sqltatype *rasqltap = sqlca->sqlrap->rasqltap;

  if ((rasqltap->tatrout == cpr_trace_long) ||
      (rasqltap->tatrout == cpr_trace_modlong)) {
    memset (rasqltap->tastr80, ' ', mxsp_c256);
    strcpy ((char *) nam, "VALUE LENGTH   :  ");
    strncpy (rasqltap->tastr80, (const char *) nam, mxsp_name);
    rasqltap->tastr80l = mxsp_name;
    ind = vallen;
    pr04LongPutChr12 (&ind, &rasqltap->tastr80l, rasqltap->tastr80);
    p08vfwritetrace (sqlca->sqlrap);
  }  
  
}


/******************************************************************************
 *
 * pr04LongTraceTrunc: This function traces out a truncated warning together
 * with the maximal length of the actual hostvariable.
 *
 ******************************************************************************/

void pr04LongTraceTrunc (sqlcatype *sqlca, tsp00_Int4 indicator)
{
  tsp00_Namec nam;
  tsp00_Int4 ind;

  sqltatype *rasqltap = sqlca->sqlrap->rasqltap;

  if ((rasqltap->tatrout == cpr_trace_long) ||
      (rasqltap->tatrout == cpr_trace_modlong)) {
    memset (rasqltap->tastr80, ' ', mxsp_c256);
    strcpy ((char *) nam, "WARNING: TRUNCATED");
    strncpy (rasqltap->tastr80, (const char *) nam, mxsp_name);
    rasqltap->tastr80l = mxsp_name;
    strcpy ((char *) nam, " ***** MAX LENGTH ");
    strncpy (rasqltap->tastr80 + rasqltap->tastr80l, (const char *) nam, mxsp_name);
    rasqltap->tastr80l += mxsp_name;
    strcpy ((char *) nam, "   :              ");
    strncpy (rasqltap->tastr80 + rasqltap->tastr80l, (const char *) nam, mxsp_name);
    rasqltap->tastr80l += 7;
    ind = indicator;
    pr04LongPutChr12 (&ind, &rasqltap->tastr80l, rasqltap->tastr80);
    p08vfwritetrace (sqlca->sqlrap);
  }
}


/******************************************************************************
 *
 * pr04LongWriteTracePart: This function initializes the tracing output and
 * writes several longdata information to the trace.
 *
 ******************************************************************************/

void pr04LongWriteTracePart (sqlcatype *sqlca, sqlxatype *sqlxa, struct SQLPCLD *ldpcact, 
			     int lvcoffset, int *rowno, tpr00_TraceTextEnum mode)
{
  tsp00_Lname vaname;
  tsp1_param_info sfinfo;
#ifndef SQLODBC
  tpr05_StringEncoding bufenc;
#endif

  if ((sqlca->sqlgap->gacmpkind != CPR_LA_ODBC) && (ldpcact->ldrowno != *rowno)) {
    /* A new row starts. */
    pr04LongTraceInit (sqlca, cpr_tr_loop, &vaname, ldpcact->ldrowno);
    *rowno = ldpcact->ldrowno;
  }

  memcpy (vaname, "PARAMETER                       ", sizeof (tsp00_Lname));
  pr04LongTraceInit (sqlca, cpr_tr_longvar, &vaname, ldpcact->ldcolno);

  if ((ldpcact->ldhosttype != cpr_vfile) &&
      (ldpcact->ldhosttype != cpr_vfilec))
    pr04LongTraceHostLen (sqlca, ldpcact->ldhostlen - lvcoffset, 
			  ldpcact->ldhostlensum - lvcoffset);
  
  memset (&sfinfo.sp1i_mode, '\0', 1);
  sfinfo.sp1i_io_type = sp1io_output;
  sfinfo.sp1i_data_type = ldpcact->ldsfdtype;
  if (ldpcact->ldhostlensum > 100)
    sfinfo.sp1i_length = 100;
  else
    sfinfo.sp1i_length = ldpcact->ldhostlensum - lvcoffset;
  sfinfo.sp1i_in_out_len = sfinfo.sp1i_length + 1;
  sfinfo.sp1i_frac = 0;
  sfinfo.variant.C_true.sp1i_bufpos_F = 1 + lvcoffset;
  if (ldpcact->ldhostlen > 0) {
    if (sqlca->sqlrap->rasqltap->tatrout != cpr_trace_off) {
#ifndef SQLODBC
      bufenc = pr04LongGetBufEncoding (ldpcact->ldsfdtype);
      if ((p04isunidata (ldpcact->ldsfdtype) && pr04LongIsAsciiHostvar (ldpcact)) ||
	  (p04isasciidata (ldpcact->ldsfdtype) && (ldpcact->ldhosttype != cpr_vlong_desc) &&
	   ((bufenc == sp77encodingUCS2) || (bufenc == sp77encodingUCS2Swapped)))) {
#else
      if (p04isunidata (ldpcact->ldsfdtype) && pr04LongIsAsciiHostvar (ldpcact)) {
#endif
	char tmpbuf[256];
	int lvcoffset;
	tsp00_Uint4   cbWrite;    /* number of bytes written */
	tsp00_Uint4   cbParsed;   /* number of bytes parsed */
	
	pr04LongGetHostInfoOff (sqlca, sqlxa, &lvcoffset);
	
	sp78convertString (sp77encodingUCS2Native,
			   tmpbuf,
			   sizeof(tmpbuf),
			   &cbWrite,
			   FALSE,
			   sp77encodingAscii,
			   (char *) ldpcact->ldhostaddr + lvcoffset,
			   ldpcact->ldhostlen,
			   &cbParsed);

        if (2*ldpcact->ldhostlensum > 100)
          sfinfo.sp1i_length = 100;
        else
          sfinfo.sp1i_length = 2*(ldpcact->ldhostlensum - lvcoffset);

	sfinfo.sp1i_in_out_len = sfinfo.sp1i_length + 1;	

        pr04LongFirstFromTo (sqlca, &sfinfo, 
                             tmpbuf,
                             1);        
      } else {
        pr04LongFirstFromTo (sqlca, &sfinfo, (char *) ldpcact->ldhostaddr + lvcoffset, 1);
        pr04LongTraceFromTo (sqlca, mode, 
                             ldpcact->ldsfbufpos - ldpcact->ldhostlensum + 1 + lvcoffset, 
                             ldpcact->ldhostlensum - lvcoffset); 
	if (sqlca->sqlgap->gacmpkind == CPR_LA_ODBC)
	  pr04LongTraceValueLength (sqlca, ldpcact->ldhostlensum - lvcoffset);
      }
    }
  }
}


void pr04LongSaveIDescriptors (sqlcatype *sqlca, int ind, int nr)
{
  int i, cnt;
  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp;
  struct SQLSPLD *ldldsp;
  struct SQLPCLD *ldldpc;

  if (nr > 0) {
    cnt = 0;
    for (i = ind; i < ind + nr; i++, cnt++) {
      memcpy (&(*ld)->ldpcarr[i], &(*ld)->ldpc[cnt], sizeof (struct SQLPCLD));
      memcpy (&(*ld)->ldsparr[i], &(*ld)->ldsp[cnt], sizeof (struct SQLSPLD));      
    }
  } else {
    for (i = 0; i < (*ld)->ldmaxi; i++) {
      ldldpc = (struct SQLPCLD *) p03dynalloc (&(*ld)->ldpcarr_mdesc);
      memcpy (ldldpc, &(*ld)->ldpc[i], sizeof (struct SQLPCLD));
      
      ldldsp = (struct SQLSPLD *) p03dynalloc (&(*ld)->ldsparr_mdesc);
      memcpy (ldldsp, &(*ld)->ldsp[i], sizeof (struct SQLSPLD));
    }
  }
}

void pr04LongLoadIDescriptors (sqlcatype *sqlca, int ind, int nr)
{
  int i, cnt;
  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp;

  cnt = 0;
  for (i = ind; i < ind + nr; i++, cnt++) {
    memcpy (&(*ld)->ldpc[cnt], &(*ld)->ldpcarr[i], sizeof (struct SQLPCLD));
    memcpy (&(*ld)->ldsp[cnt], &(*ld)->ldsparr[i], sizeof (struct SQLSPLD));
  }
}

bool pr04LongPutData (sqlcatype *sqlca, sqlxatype *sqlxa,
                      sqlgaentry *ga, tpr_longvarcharrec *lvcrec,
                      bool *send, bool odbc_proc)
{
  const int cmpposvalmod = 28;
  const int mx_valmove = 8;
  int mlen, dpos;
  tsp00_Int4 flen, vpos, maxblen, mlen4,
    trans_len, host_mlen;
  tsp00_Int2 desclen, dataeof, unitrans;
  tsp1_param_info zwsfinfo;
  tsp1_part_ptr data_ptr;
  tsp1_part_header *ph;
  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp;  
  struct SQLPCLD *ldpcact = &(*ld)->ldpc[(*ld)->ldindpc-1];
  struct SQLSPLD *ldspact;
  tpr05_StringEncoding bufenc;
#ifndef SQLODBC
  tsp00_Int2 asciitrans;
#endif
  
  if ((sqlca->sqlemp->eprerr == cpr_datafile_open_error) ||
      (sqlca->sqlemp->eprerr == cpr_datafile_write_error) ||
      (sqlca->sqlemp->eprerr == cpr_datafile_read_error) ||
      (sqlca->sqlemp->eprerr == cpr_datafile_close_error))
    return false;

  p03find_part (sqlca->sqlrap, sp1pk_data, &data_ptr);  
  if (data_ptr == 0)
    p03find_part (sqlca->sqlrap, sp1pk_longdata, &data_ptr);
  if (data_ptr == 0)
    return false;

  ph = &data_ptr->sp1p_part_header;
  maxblen = ph->sp1p_buf_size - 8;
  unitrans = cpr_is_false;
  mlen = maxblen - lvcrec->lvcfreepos;

#ifndef SQLODBC
  asciitrans = cpr_is_false;
#endif
  bufenc = pr04LongGetBufEncoding (ldpcact->ldsfdtype);

#ifndef SQLODBC
  if ((p04isunidata (ldpcact->ldsfdtype) && pr04LongIsAsciiHostvar (ldpcact)) ||
      (((bufenc == sp77encodingUCS2) || (bufenc == sp77encodingUCS2Swapped)) &&
       (!p04isunidata (ldpcact->ldsfdtype) && pr04LongIsAsciiHostvar (ldpcact))))
#else
  if (p04isunidata (ldpcact->ldsfdtype))
#endif  
  {
    unitrans = true;
    mlen = (maxblen - 8 - lvcrec->lvcfreepos) / 2;
  }

#ifndef SQLODBC
  if ((bufenc == sp77encodingAscii) &&
      (p04isasciidata (ldpcact->ldsfdtype) && pr04LongIsUnicodeHostvar (ldpcact))) {
    asciitrans = true;
  }
#endif
  
  if ((sqlxa->xaSQLDesc->PacketEncoding == sp77encodingUCS2) ||
      (sqlxa->xaSQLDesc->PacketEncoding == sp77encodingUCS2Swapped) ||
      (bufenc == sp77encodingUCS2) ||
      (bufenc == sp77encodingUCS2Swapped)) {
    if ((mlen % 2) != 0)
      mlen -= 1;
  }

  if (mlen < 0)
    return false;

  vpos = lvcrec->lvcvalmlen + 1;
  desclen = (*ld)->ldsp[(*ld)->ldindsp-1].ldlength;
  odbc_proc = odbc_proc;

  dataeof = cpr_is_false;
  if ((ldpcact->ldhosttype == cpr_vfile) ||
      (ldpcact->ldhosttype == cpr_vfilec)) {
    if (ldpcact->ldfileno != 0) {
      flen = mlen / mxsp_buf * mxsp_buf;
      trans_len = flen;
      host_mlen = flen;
      p03datafread (sqlca->sqlrap, ldpcact->ldfileno, &flen,
		    &data_ptr->sp1p_buf, lvcrec->lvcfreepos,
		    &dataeof, sqlca->sqlemp);
      p03traceerror (sqlca->sqlrap, sqlca->sqlemp);
      if (dataeof == cpr_is_true) {
	lvcrec->lvchostlen = lvcrec->lvchostpos + flen - 1;
	trans_len = flen;
	host_mlen = flen;
	p03datafclose (sqlca->sqlrap, &ldpcact->ldfileno, sqlca->sqlemp);
      } else {
	lvcrec->lvchostlen -= flen;
	lvcrec->lvchostpos += flen;
	host_mlen = lvcrec->lvchostlen;
      } 
    } else {
      p04err (sqlca->sqlrap, sqlca->sqlemp, cpr_datafile_read_error);
    }
  } else {
    trans_len = mlen;
    host_mlen = lvcrec->lvchostlen - lvcrec->lvchostpos + 1;

    if ((unitrans == true) && pr04LongIsAsciiHostvar (ldpcact)) {
      char *sp1p_buf_F = (char *) data_ptr->sp1p_buf;
      char *ldhostaddr = ldpcact->ldhostaddr;
      tsp78ConversionResult rc;
      tsp00_Uint4   cbWrite;    /* number of bytes written */
      tsp00_Uint4   cbParsed;   /* number of bytes parsed */

      trans_len = mlen + mlen;

      sp1p_buf_F += lvcrec->lvcfreepos - 1;
      ldhostaddr += lvcrec->lvchostpos + lvcrec->lvcaoffset - 1;
      rc = sp78convertString (sp77encodingUCS2Native,
			      sp1p_buf_F,
			      trans_len,
			      &cbWrite,
			      FALSE,
			      sp77encodingAscii,
			      ldhostaddr,
			      host_mlen,
			      &cbParsed);
      
      if (((unsigned int) mlen == cbParsed) && 
	  (rc == sp78_TargetExhausted)) {
	rc = sp78_Ok;
      }

      host_mlen = cbParsed;

      if (rc != sp78_Ok) {
	p08runtimeerror (sqlca, sqlxa,
			 cpr_conv_to_unicode_impossible);
      }

      trans_len = 2*host_mlen;
#ifndef SQLODBC
    } else if ((asciitrans == true) && pr04LongIsUnicodeHostvar (ldpcact)) {
      char *sp1p_buf_F = (char *) data_ptr->sp1p_buf;
      char *ldhostaddr = ldpcact->ldhostaddr;
      tsp78ConversionResult rc;
      tsp00_Uint4   cbWrite;    /* number of bytes written */
      tsp00_Uint4   cbParsed;   /* number of bytes parsed */

      sp1p_buf_F += lvcrec->lvcfreepos - 1;
      ldhostaddr += lvcrec->lvchostpos + lvcrec->lvcaoffset - 1;

      rc = sp78convertString (sp77encodingAscii,
			      sp1p_buf_F,
			      trans_len,
			      &cbWrite,
			      FALSE,
			      sp77encodingUCS2Native,
			      ldhostaddr,
			      host_mlen,
			      &cbParsed);
      if (rc != sp78_Ok) {
	p08runtimeerror (sqlca, sqlxa,
			 cpr_conv_from_unicode_impossible);
      }
      trans_len = host_mlen / 2;
#endif
    } else {
      if (lvcrec->lvchostlen == 0)
	host_mlen = 0;
      else {
	char *sp1p_buf_F = (char *) data_ptr->sp1p_buf;
	char *ldhostaddr = ldpcact->ldhostaddr;

	if (mlen <= host_mlen)
	  host_mlen = mlen;
	trans_len = host_mlen;
	
	sp1p_buf_F += lvcrec->lvcfreepos - 1;
	ldhostaddr += lvcrec->lvchostpos + lvcrec->lvcaoffset - 1;
	memcpy (sp1p_buf_F, ldhostaddr, host_mlen);
      }
    }
  }
  if (mlen <= host_mlen) {
    if (lvcrec->lvchostpos + host_mlen == lvcrec->lvchostlen + 1) {
      lvcrec->lvcnext = false;
      if (lvcrec->lvcpiecewise) {
	if (sqlca->sqlgap->gacmpkind == CPR_LA_ODBC) {
	  if (lvcrec->lvchostlen == 0)
	    lvcrec->lvcvalmode = vm_lastdata;
	  else
	    lvcrec->lvcvalmode = vm_datapart;
	} else {
	  lvcrec->lvcvalmode = vm_datapart;
	}
      } else {
        lvcrec->lvcvalmode = vm_lastdata;
      }
    } else
      lvcrec->lvcvalmode = vm_datapart;
    *send = true;
    lvcrec->lvcvalpos = lvcrec->lvcfreepos;
    lvcrec->lvcvallen = trans_len;
    lvcrec->lvchostpos += host_mlen;
    if ((*ld)->ldputvalsend)
      dpos = ph->sp1p_buf_len - desclen + cmpposvalmod;
    else
      dpos = ldpcact->ldsfbufpos + cmpposvalmod;
    data_ptr->sp1p_buf[dpos-1] = lvcrec->lvcvalmode;
    memcpy (data_ptr->sp1p_buf + dpos + 4, &lvcrec->lvcvalmode + 5, 
	    mx_valmove);
    ph->sp1p_buf_len += trans_len;
  } else {
    if (lvcrec->lvchostlen == 0) {
      trans_len = 0;
      host_mlen = 0;
    }
    if (lvcrec->lvcpiecewise) {
      if (sqlca->sqlgap->gacmpkind == CPR_LA_ODBC) {
	if (lvcrec->lvchostlen == 0)
	  lvcrec->lvcvalmode = vm_lastdata;
	else
	  lvcrec->lvcvalmode = vm_datapart;
      } else {
	lvcrec->lvcvalmode = vm_datapart;
      }
    } else {
      lvcrec->lvcvalmode = vm_lastdata;
    }
    lvcrec->lvcnext = false;
    lvcrec->lvcvalpos = lvcrec->lvcfreepos;
    lvcrec->lvcvallen = trans_len;
    lvcrec->lvchostpos += host_mlen;
    if ((*ld)->ldputvalsend)
      dpos = ph->sp1p_buf_len - desclen + cmpposvalmod;
    else
      dpos = ldpcact->ldsfbufpos + cmpposvalmod;
    data_ptr->sp1p_buf[dpos-1] = lvcrec->lvcvalmode;
    memcpy (data_ptr->sp1p_buf + dpos + 4, &lvcrec->lvcvalmode + 5, 
	    mx_valmove);
    ph->sp1p_buf_len += trans_len;
    if ((maxblen - ph->sp1p_buf_len < LONG_DESC_BLOCK_MXSP00 + 20) ||
	(((*ld)->ldmaxi == (*ld)->ldindsp) && ((*ld)->ldputvalsend)))
      *send = true;
    else
      *send = false;
  }
  ldspact = &(*ld)->ldsp[(*ld)->ldindsp-1];
  ldspact->ld.o.ld_valmode = lvcrec->lvcvalmode;
  if (lvcrec->lvcpiecewise) {
    if ((lvcrec->lvchostlen == 0) &&
        ((*ld)->ldmaxi > (*ld)->ldindsp) &&
        odbc_proc)
      *send = false;
    else
      *send = true;
  }
  if (((*ld)->ldmaxi == (*ld)->ldindsp) &&
      (lvcrec->lvcvalmode == vm_lastdata) &&
      *send) {
    if (maxblen - ph->sp1p_buf_len > LONG_DESC_BLOCK_MXSP00 + 1) {
      if ((*ld)->ldputvalsend) {
	struct SQLSPLD *ldspact = &(*ld)->ldsp[(*ld)->ldmaxi-1];
        struct SQLPCLD *ldpcact = &(*ld)->ldpc[ldspact->ld.o.ld_valind-1];
	desclen = (*ld)->ldsp[(*ld)->ldmaxi-1].ldlength;
	ldspact->ld.o.ld_valmode = vm_last_putval;
	memcpy (data_ptr->sp1p_buf + ph->sp1p_buf_len + 1, 
		&ldspact->ld.n, desclen);
	if (ldspact->ldkind == cpr_is_true)
	  data_ptr->sp1p_buf[ph->sp1p_buf_len] = (char) csp_long_desc_def_byte;
	else {
          if (pr04LongIsSQL_DEFAULT (ldpcact))
	    data_ptr->sp1p_buf[ph->sp1p_buf_len] = csp_default_byte;
          else
            data_ptr->sp1p_buf[ph->sp1p_buf_len] = csp_defined_byte;
        }
	ph->sp1p_buf_len += desclen + 1;
	(*ld)->ldcount += 1;
	ph->sp1p_arg_count = (*ld)->ldcount;
	lvcrec->lvcdescno = (*ld)->ldmaxi;
      } else
	(*ld)->ldallsend = true;
    }
  }
  if (lvcrec->lvcpiecewise) /* TSNEW */
    vpos += ldpcact->ldhostlensum - ldpcact->ldhostlen;
  if (*send)
    s26finish_part ((tsp1_packet *) ga->gareqptr, data_ptr);
  if ((lvcrec->lvcpiecewise || (vpos == 1)) && (mlen > 0)) {
  /* if (((vpos == 1) || (vpos - ldpcact->ldhostlen == 1)) && (mlen > 0)) { */
    zwsfinfo.sp1i_io_type = sp1io_input;
    zwsfinfo.sp1i_data_type = ldpcact->ldsfdtype;
    if (host_mlen > 100)
      zwsfinfo.sp1i_length = 100;
    else
      zwsfinfo.sp1i_length = host_mlen;
    zwsfinfo.sp1i_in_out_len = zwsfinfo.sp1i_length + 1;
    zwsfinfo.sp1i_frac = 0;
    zwsfinfo.variant.C_true.sp1i_bufpos_F = lvcrec->lvcfreepos;
    if (sqlca->sqlrap->rasqltap->tatrout != cpr_trace_off) {
      if ((unitrans == true) && pr04LongIsAsciiHostvar (ldpcact)) {
	char tmpbuf[256];
	tsp00_Uint4   cbWrite;    /* number of bytes written */
	tsp00_Uint4   cbParsed;   /* number of bytes parsed */

        if (2*host_mlen <= 100)
          zwsfinfo.sp1i_length = 2*host_mlen;
	else
	  zwsfinfo.sp1i_length = 100;

	zwsfinfo.sp1i_in_out_len = zwsfinfo.sp1i_length + 1;

	sp78convertString (sp77encodingUCS2Native,
			   tmpbuf,
			   sizeof(tmpbuf),
			   &cbWrite,
			   FALSE,
			   sp77encodingAscii,
			   (char *) ldpcact->ldhostaddr + lvcrec->lvcaoffset,
			   ldpcact->ldhostlen,
			   &cbParsed);
	

        pr04LongFirstFromTo (sqlca, &zwsfinfo, 
                             tmpbuf,
                             1);
      } else {
        pr04LongFirstFromTo (sqlca, &zwsfinfo, 
                             (char *) data_ptr->sp1p_buf /* ldpcact->ldhostaddr */ /* + lvcrec->lvcaoffset */,
                             lvcrec->lvcfreepos);
      }
    }
  }
  if ((unitrans == cpr_is_true) || 
      (ldpcact->ldhosttype == cpr_vfile) ||
      (ldpcact->ldhosttype == cpr_vfilec))
    mlen4 = trans_len;
  else
    mlen4 = host_mlen;
  pr04LongTraceFromTo (sqlca, cpr_tr_inp, vpos, mlen4);
  lvcrec->lvcvalmlen += mlen4;

  return true;
}

bool pr04LongInsertLongVarchar (sqlcatype *sqlca, sqlxatype *sqlxa,
				sqlgaentry *ga, tpr_longvarcharrec *lvcrec,
				tsp00_Int2 *putvalinit, bool *first,
				bool odbc_proc)
{
  int i, pos, index, reterrd3;
  tsp00_Int2 anz, desclen, restdata;
  tsp1_part_ptr data_ptr;
  tsp_int_map_c2 ind;
  bool send;
  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp;  

  struct SQLPCLD *ldpcact = &(*ld)->ldpc[(*ld)->ldindpc-1];

#ifndef SQLODBC
  /* set packetencoding sqlmode */
  p04mode (sqlca, ga);
#endif

  send = false;
  if ((sqlca->sqlemp->eprerr == cpr_datafile_open_error) ||
      (sqlca->sqlemp->eprerr == cpr_datafile_write_error) ||
      (sqlca->sqlemp->eprerr == cpr_datafile_read_error) ||
      (sqlca->sqlemp->eprerr == cpr_datafile_close_error)) {
    sqlca->sqlcode = sqlca->sqlemp->ereturncode;
    (*ld)->ldputvalsend = true;
    sqlxa->xacmdinit = cpr_is_false;
    lvcrec->lvcnext = false;
  } else {
    reterrd3 = sqlca->sqlerrd[2];
    if ((ldpcact->ldhosttype != cpr_vfile) &&
	(ldpcact->ldhosttype != cpr_vfilec)) {
      pr04LongTraceHostLen (sqlca, lvcrec->lvchostlen, 
			    ldpcact->ldhostlensum);
    }
    while (lvcrec->lvcnext) {
      struct SQLSPLD *ldspact;
      tsp1_part_header *ph;
      if ((*ld)->ldputvalsend && (*putvalinit == cpr_is_first)) {
	p03ccmdinit (sqlxa->xaSQLDesc, sqlca, ga, sp1m_putval);
	(*ld)->ldcount = 0;
	sqlca->sqlerrd[2] = reterrd3;
	s26new_part_init ((tsp1_packet *) ga->gareqptr, 
			  (tsp1_segment *) sqlca->sqlrap->rasegptr, 
			  &data_ptr);
	ph = &data_ptr->sp1p_part_header;
	ph->sp1p_part_kind = sp1pk_longdata; 
	ph->sp1p_buf_len = 0;
	sqlca->sqlrap->rasegmpartptr[sp1pk_longdata] = data_ptr;
	*putvalinit = cpr_is_false;
      } else {
	p03find_part (sqlca->sqlrap, sp1pk_data, &data_ptr);
	if (data_ptr == 0)
	  p03find_part (sqlca->sqlrap, sp1pk_longdata, &data_ptr);
        if (data_ptr == 0)
          return false;
      }
      ldspact = &(*ld)->ldsp[(*ld)->ldindsp-1];
      if (ldspact->ldkind == cpr_is_true) {
	if (lvcrec->lvchostlen == 0) {
	  ldspact->ld.n.ldb_full_len = ldpcact->ldhostlensum;
	} else {
	  ldspact->ld.n.ldb_full_len = ldpcact->ldhostlensum + 1;
	}
      }
      desclen = ldspact->ldlength;
      if ((*ld)->ldputvalsend) {
	tsp1_part_header *ph = &data_ptr->sp1p_part_header;
	/* tsp1_part_header *ph = &data_ptr->variant.C_1.sp1p_part_header_F; */
	pos = ph->sp1p_buf_len + 1;
	memcpy (data_ptr->sp1p_buf + pos, &ldspact->ld.n, desclen);
	if (ldspact->ldkind == cpr_is_true)
	  data_ptr->sp1p_buf[ph->sp1p_buf_len] = (char) csp_long_desc_def_byte;
	else {
          if (pr04LongIsSQL_DEFAULT (ldpcact)) 
	    data_ptr->sp1p_buf[pos-1] = csp_default_byte;
          else
            data_ptr->sp1p_buf[pos-1] = csp_defined_byte;
        }
	(*ld)->ldcount += 1;
	ph->sp1p_arg_count = (*ld)->ldcount;
	lvcrec->lvcfreepos = ph->sp1p_buf_len + desclen + 2;
	ph->sp1p_buf_len += desclen + 1;
      } else {
	tsp1_part_header *ph = &data_ptr->sp1p_part_header;
	pos = ldpcact->ldsfbufpos;
	memcpy (data_ptr->sp1p_buf + pos, &ldspact->ld.n, desclen);
	if (ldspact->ldkind == cpr_is_true)
	  data_ptr->sp1p_buf[ph->sp1p_buf_len] = (char) csp_long_desc_def_byte;
	else {
          if (pr04LongIsSQL_DEFAULT (ldpcact)) 
	    data_ptr->sp1p_buf[pos-1] = csp_default_byte;
          else
            data_ptr->sp1p_buf[pos-1] = csp_defined_byte;
        }
	(*ld)->ldcount += 1;
	lvcrec->lvcfreepos = ph->sp1p_buf_len + 1;
      }
      restdata = 100;
      if ((ldpcact->ldhosttype == cpr_vfile) ||
	  (ldpcact->ldhosttype == cpr_vfilec)) {
	restdata = mxsp_buf;
      }
      ph = &data_ptr->sp1p_part_header;
      if (lvcrec->lvcfreepos > ph->sp1p_buf_size - restdata) {
	send = true;
	s26finish_part ((tsp1_packet *) ga->gareqptr, data_ptr);
      } else {
	if (pr04LongPutData (sqlca, sqlxa, ga, lvcrec, &send, odbc_proc) == false)
          return false;
      }
      if (send) {
	pr03PacketReqRec (sqlxa->xaSQLDesc->ConDesc, sqlca->sqlemp);
	if (sqlca->sqlemp->ereturncode != 0) {
	  (*ld)->ldputvalsend = true;
	  p03returncodeget (sqlca, sqlxa);
	  sqlxa->xacmdinit = false;
	  lvcrec->lvcnext = false;
	  return false;
	} else {
	  if (!((*ld)->ldputvalsend)) {
	    if (sqlca->sqlcode == 0) 
	      p03cresultcountget (sqlca->sqlrap, ga, &sqlca->sqlerrd[2]);
	    reterrd3 = sqlca->sqlerrd[2];
	    (*ld)->ldputvalsend = true;
	    *first = true;
	  } else {
	    sqlca->sqlerrd[2] = reterrd3;
	  }
	  p03find_part (sqlca->sqlrap, sp1pk_longdata, &data_ptr);
	  if (data_ptr != 0) {
	    tsp1_part_header *ph = &data_ptr->sp1p_part_header;
	    anz = ph->sp1p_arg_count;
	    pos = 1;
	    if ((anz == 0) && (sqlca->sqlemp->ereturncode == 0)) {
	      sqlxa->xacmdinit = cpr_is_false;
	      lvcrec->lvcnext = false;
	    }
	    for (i = 1; i <= anz; i++) {
	      const int cmpposvalind = 29;
	      ind.variant.C_FALSE.map_c2_F[0] = 
		data_ptr->sp1p_buf[pos+cmpposvalind-1];
	      ind.variant.C_FALSE.map_c2_F[1] = 
		data_ptr->sp1p_buf[pos+cmpposvalind];
	      index = ind.variant.C_TRUE.map_int_F;
	      desclen = ldspact->ldlength;
	      memcpy (&(*ld)->ldsp[index-1].ld.n, data_ptr->sp1p_buf+pos,
		      desclen);
	      pos += desclen + 1;
	    }
	  }
	  *putvalinit = cpr_is_first;
	}
      }
    }
  }
  return true;
}

void pr04LongGetHostInfoFromDA (sqlcatype *sqlca, sqlxatype * sqlxa, int i);

bool pr04LongInputLvc (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga,
                       tpr_longvarcharrec *lvcrec)
{
  bool first;
  int maxcol, pos, i, iend;
  tsp00_Int2 putvalinit;
  tsp00_Int2 desclen;
  tsp00_Int2 rowno;
  tsp00_Int4 retsqlerrd3;
  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp;
  tsp1_part_header *ph;
  tsp1_part_ptr data_ptr;
  tsp00_Lname vaname;

  first = 1;
  putvalinit = cpr_is_false;
  if ((*ld)->ldputvalsend) {
    if (sqlca->sqlcode == 0) {
      p03cresultcountget (sqlca->sqlrap, ga, &sqlca->sqlerrd[2]);
    }
  }
  if ((*ld)->ldmaxi != 0) {
    p03find_part (sqlca->sqlrap, sp1pk_data, &data_ptr);
    if (data_ptr == 0)
      return false;
    ph = &data_ptr->sp1p_part_header;
    retsqlerrd3 = sqlca->sqlerrd[2];
    if ((*ld)->ldputvalsend) {
      maxcol = ph->sp1p_arg_count;
    } else {
      maxcol = (*ld)->ldmaxi;
    }
    if ((*ld)->ldputvalsend) {
      pos = ph->sp1p_buf_len;
    } else {
      pos = (*ld)->ldpc[0].ldsfbufpos;
    }
    for (i = 0; i < maxcol; i++) {
      desclen = (*ld)->ldsp[i].ldlength;
      (*ld)->ldpc[i].ldhostlensum = 0;
      memcpy (&(*ld)->ldsp[i].ld.n, 
              data_ptr->sp1p_buf+pos,
              desclen);
      if ((*ld)->ldputvalsend) {
        pos += desclen + 1;
      } else {
        if (i < maxcol) {
          pos = (*ld)->ldpc[i+1].ldsfbufpos;
        }
      }
    }
    if ((*ld)->ldputvalsend) {
      putvalinit = cpr_is_first;
    }
    rowno = -1;
    if (sqlca->sqlgap->gacmpkind == CPR_LA_ODBC)
      iend = 1;
    else
      iend = maxcol;

    for (i = 0; i < iend; i++) {
      if ((sqlca->sqlcode == 0) || (sqlca->sqlcode == 100)) {
        struct SQLPCLD *ldpcact;
        (*ld)->ldindsp = i+1;
        (*ld)->ldindpc = (*ld)->ldsp[i].ld.o.ld_valind;
        ldpcact = &(*ld)->ldpc[(*ld)->ldindpc-1];
        if (rowno != ldpcact->ldrowno) {
          rowno = ldpcact->ldrowno;
        }
        memcpy (vaname, "PARAMETER                       ", sizeof (tsp00_Lname));
        pr04LongTraceInit (sqlca, cpr_tr_longvar, &vaname, ldpcact->ldcolno);
        pr04LongGetHostInfoFromDA (sqlca, sqlxa, (*ld)->ldindpc-1);
        pr04LongGetHostInfo (sqlca, sqlxa, lvcrec, true);
        ldpcact->ldhostlen = lvcrec->lvchostlen;
        ldpcact->ldhostlensum += ldpcact->ldhostlen;
        if (pr04LongInsertLongVarchar (sqlca, sqlxa, ga, lvcrec, &putvalinit,
                                       &first, false) == false)
          return false;
        if (first && ((sqlca->sqlcode == 0) || (sqlca->sqlcode == 100))) {
          retsqlerrd3 = sqlca->sqlerrd[2];
          first = false;
        }
      }
    }
    if ((*ld)->ldputvalsend) {
      (*ld)->ldallsend = true;
    }
    if (lvcrec->lvcpiecewise) {
      (*ld)->ldallsend = true;
      (*ld)->ldretmaxi = (*ld)->ldmaxi;
      (*ld)->ldretmaxo = (*ld)->ldmaxo;
      (*ld)->ldretindpc = (*ld)->ldindpc;
      (*ld)->ldretindsp = (*ld)->ldindsp;
    }
    sqlca->sqlerrd[2] = retsqlerrd3;
  }
  return true;
}

bool pr04LongPutvalData (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga,
			 tpr_longvarcharrec *lvcrec, int spindex,
			 bool *first)
{
  int i, iend;
  tsp00_Lname vaname;
  tsp00_Int4 retsqlerrd3;
  tsp00_Int2 putvalinit;
  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp;  

  p04mode (sqlca, ga);

  p03ccmdinit (sqlxa->xaSQLDesc, sqlca, ga, sp1m_putval);
  (*ld)->ldputvalsend = true;
  putvalinit = cpr_is_first;
  *first = true;
  iend = spindex;

  /* if (((*ld)->ldpc[(*ld)->ldindpc-1].ldhostlen == 0) && (iend < (*ld)->ldmaxi))
     iend++; */

  for (i = spindex; i <= iend; i++) {
    struct SQLSPLD *ldspact = &(*ld)->ldsp[i-1];
    if ((sqlca->sqlcode == 0) || (sqlca->sqlcode == 100)) {
      if (ldspact->ld.o.ld_valmode != vm_lastdata) {
	(*ld)->ldindsp = i;
	(*ld)->ldindpc = ldspact->ld.o.ld_valind;
	memcpy (vaname, "PARAMETER                       ", 
		sizeof (tsp00_Lname));
	pr04LongTraceInit (sqlca, cpr_tr_longvar, &vaname, 
			   (*ld)->ldpc[(*ld)->ldindpc-1].ldcolno);
	if ((*ld)->ldpc[(*ld)->ldindpc-1].ldhostlen != 0) {
	  pr04LongGetHostInfoFromDA (sqlca, sqlxa, (*ld)->ldindpc-1);
	  pr04LongGetHostInfo (sqlca, sqlxa, lvcrec, true);
	  (*ld)->ldpc[(*ld)->ldindpc-1].ldhostlen = lvcrec->lvchostlen;
	  (*ld)->ldpc[(*ld)->ldindpc-1].ldhostlensum += 
	    (*ld)->ldpc[(*ld)->ldindpc-1].ldhostlen;
	}
	if (pr04LongInsertLongVarchar (sqlca, sqlxa, ga, lvcrec, &putvalinit,
                                       first, true) == false)
          return false;
	(*ld)->ldallsend = true;
	(*ld)->ldretmaxi = (*ld)->ldmaxi;
	(*ld)->ldretmaxo = (*ld)->ldmaxo;
	(*ld)->ldretindpc = (*ld)->ldindpc;
	(*ld)->ldretindsp = (*ld)->ldindsp;
	if (*first) {
	  retsqlerrd3 = sqlca->sqlerrd[2];
	  *first = false;
	}
      }
    }
  }
  return true;
}

bool pr04LongPutvalInput (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry* ga)
{
  sqluns4 i;
  int j, spindex, rowno;
  bool first;
  tpr_longvarcharrec lvcrec;
  tsp00_Lname vaname;
  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp;
  struct SQLPCLD *ldpcact;
  
  first = true;
  (*ld)->ldmaxi = (*ld)->ldpc_mdesc.descNelem;

  if (pr04LongCheckParamNumber (sqlca, sqlxa) == false)
    return false;

  if ((*ld)->ldmaxi == 0) {
    sqltatype *sqlta = sqlca->sqlrap->rasqltap;

    if ((int) sqlta->tatrout == 3 || (int) sqlta->tatrout == 5) {
      p03cseterror (sqlca->sqlemp, cpr_no_long_columns_found);
      p03csqlemptosqlca (sqlca, sqlca->sqlemp);
      strcpy (sqlta->tastr80, "PUTVAL         :  No long columns found.");
      sqlta->tastr80l = (tsp00_Int4) strlen (sqlta->tastr80);
      p08vfwritetrace (sqlca->sqlrap);
    }
  } else {
    i = 1;
    while (i <= (*ld)->ldpcarr_mdesc.descNelem) {
      pr04LongLoadIDescriptors (sqlca, i-1, (*ld)->ldmaxi); 
      pr04LongGetHostInfoFromDA (sqlca, sqlxa, 0);
      ldpcact = &(*ld)->ldpc[0];
      if ((ldpcact->ldhostlen > 0) && /* (ldpcact->ldindktype != SQL_IGNORE) */ !pr04LongIsSQL_IGNORE(ldpcact))
	pr04LongTraceInit (sqlca, cpr_tr_loop, &vaname, 
			   ldpcact->ldrowno);
      for (j = 1; j <= (*ld)->ldmaxi; j++) {
	pr04LongGetHostInfoFromDA (sqlca, sqlxa, j-1);
	ldpcact = &(*ld)->ldpc[j-1];
	if ((ldpcact->ldhostlen == 0) && /* (ldpcact->ldindktype != SQL_IGNORE) */ !pr04LongIsSQL_IGNORE(ldpcact)) {
	  rowno = -1;
	  pr04LongWriteTracePart (sqlca, sqlxa, ldpcact, 0, &rowno, cpr_tr_inp);
	  continue;
	}
	if (/* (ldpcact->ldindktype == SQL_IGNORE) */ pr04LongIsSQL_IGNORE(ldpcact) || (ldpcact->ldhostlen == 0))
	  continue;
	
	spindex = j;
	lvcrec.lvcpiecewise = 1;
	if (pr04LongPutvalData (sqlca, sqlxa, ga, &lvcrec, spindex, &first) == false)
          return false;
      }
#ifndef SQLODBC
      pr04LongSaveIDescriptors (sqlca, i-1, (*ld)->ldmaxi);
#endif
      i += (*ld)->ldmaxi;
    }   
  } 
  return true;
}

bool pr04LongILvcOneRecord (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga,
			    tpr00_ParseId *parsid, tsp1_part_ptr data_ptr,
			    tsp1_part_ptr cnt_ptr)
{
  tpr_longvarcharrec lvcrec;
  int retsqlerrd3;
  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp;
#ifdef SQLODBC
  boolean keep = true;
#else
  boolean keep = false;
#endif
  int sz;
  tpr01_CursorDesc *CursorDesc;

  if (((*ld)->ldmaxi == 0) || (sqlca->sqlcode != 0))  /* Return if no input descs. */
    return true;

  pr04LongLvcInitRec (sqlca, &lvcrec);
  (*ld)->ldputvalsend = false;
  (*ld)->ldallsend = false;

  CursorDesc = sqlxa->xaSQLDesc->CursorDesc;
  if (CursorDesc)
    CursorDesc->GetAttr (CursorDesc,
			 openkeep_epr01Cursor, &keep, &sz);

  if (keep)
    lvcrec.lvcpiecewise = 1;
  else
    lvcrec.lvcpiecewise = 0; 

  if (pr04LongInputLvc (sqlca, sqlxa, ga, &lvcrec) == false)
    return false;
  retsqlerrd3 = sqlca->sqlerrd[2];

  if (((*ld)->ldallsend) && (sqlca->sqlcode == 0)) {
    tsp00_KnlIdentifier sqlresn;
    tsp1_part_header *phd;
    tsp1_part_header *ph;
    memcpy (sqlresn, sqlca->sqlresn, sizeof (tsp00_KnlIdentifier));

    p03ccmdinit (sqlxa->xaSQLDesc, sqlca, ga, sp1m_execute);
    p03cpparsid (sqlca->sqlrap, ga, *parsid, sqlca->sqlemp);
    s26new_part_init ((tsp1_packet *) ga->gareqptr, 
		      (tsp1_segment *) sqlca->sqlrap->rasegptr, &cnt_ptr);
    ph = &cnt_ptr->sp1p_part_header;
    ph->sp1p_part_kind = sp1pk_resultcount; 
    cnt_ptr->sp1p_buf[0] = (char) csp_undef_byte;
    ph->sp1p_buf_len = MXPR_NUMB + 1;
    sqlca->sqlrap->rasegmpartptr[sp1pk_resultcount] = cnt_ptr;
    s26finish_part ((tsp1_packet *) ga->gareqptr, cnt_ptr);
    s26new_part_init ((tsp1_packet *) ga->gareqptr, 
		      (tsp1_segment *) sqlca->sqlrap->rasegptr, &data_ptr);    
    phd = &data_ptr->sp1p_part_header;
    phd->sp1p_part_kind = sp1pk_data;     
    phd->sp1p_buf_len = 0;
    sqlca->sqlrap->rasegmpartptr[sp1pk_data] = data_ptr;    
#ifndef SQLODBC
    /* Check if vm_last_putval was sent. In this case, the descriptors are not saved */
    if (lvcrec.lvcdescno != (*ld)->ldmaxi) {
      pr04LongSaveIDescriptors (sqlca, 0, 0);
    }
#endif
  }
  sqlca->sqlerrd[2] = retsqlerrd3;

  return true;
}

/******************************************************************************
 *
 * pr04LongPutDescPos: This function sends long descriptors for which more
 * data is needed via a getval message to the kernel.
 *
 ******************************************************************************/

static bool pr04LongPutDescPos (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, 
				bool putclose, int ibeg, int iend, tsp1_part_ptr data_ptr)
{
  int i;
  tsp00_Int2 anz;
  tsp00_Int2 desclen;

  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp; 
  
  /*   p03ccmdinit (sqlca, ga, sp1m_getval); */ /* Initialize getval command. */

  /* s26new_part_init ((tsp1_packet *) ga->gareqptr, 
   *(tsp1_segment *) sqlca->sqlrap->rasegptr, data_ptr); */

  tsp1_part_header *ph = &data_ptr->sp1p_part_header;
  ph->sp1p_part_kind = sp1pk_longdata; /* Sets part kind to sp1pk_longdata. */
  ph->sp1p_buf_len = 0;

  sqlca->sqlrap->rasegmpartptr[sp1pk_longdata] = data_ptr;

  anz = 0;
  for (i = ibeg; i < iend; i++) {
    /* Loop over all descriptors in ldsp and ldpc arrays. */
    struct SQLSPLD *ldspact = &(*ld)->ldsp[i];
    struct SQLPCLD *ldpcact = &(*ld)->ldpc[ldspact->ld.o.ld_valind-1];
    desclen = ldspact->ldlength;

    if (ldspact->ld.o.ld_intern_pos == 0) {
      ldspact->ld.o.ld_intern_pos = ldspact->ld.o.ld_maxlen + 1;
      ldspact->ld.o.ld_vallen = 0;
    }
    
    if (((ldpcact->ldhostlensum < ldpcact->ldhostlen) || (ldpcact->ldhostlen == 0)) &&
	((ldspact->ld.o.ld_valmode == vm_datapart) ||
	(ldspact->ld.o.ld_valmode == vm_nodata) ||
	(ldspact->ld.o.ld_valmode == vm_data_trunc) ||
	(putclose && ((ldspact->ld.o.ld_valmode == vm_close) ||
		      (ldspact->ld.o.ld_valmode == vm_lastdata))))) {
      /* Hostvariable is not filled completely and long descriptor valmode
         indicates that more longdata is wanted.
      
         ld_intern_pos is an internal flag how many bytes from the actual
         longcolumn have already been read. If the long descriptor was send
         to the kernel with ld_valmode = vm_data_trunc, the kernel sets
         ld_intern_pos to the length of the longcolumn plus one. Therefore,
         ld_intern_pos has the wrong value when send again to the kernel.
         By the use of ldsfbufpos the original ld_intern_pos is set. */

      /* If Hostvariable has length 0 and restbuffer of this variable is 
         already filled, don't go with the descriptor to kernel for more data. */ 
      if (!putclose &&
          (ldpcact->ldhostlen == 0) &&
          (ldpcact->ldrestdata) &&
          (ldpcact->ldrestdata->rbbufsize > 0) &&
          (ldpcact->ldrestdata->rbrestlen == 0))
        continue;

      if (putclose) {
        ldspact->ld.o.ld_valpos = ph->sp1p_buf_len + desclen + 2;
        ldspact->ld.o.ld_vallen = 0;
      } else {
	if (sqlca->sqlgap->gacmpkind != CPR_LA_ODBC) {
	  ldspact->ld.o.ld_intern_pos = ldpcact->ldsfbufpos + 1;
	} else if (((sqlca->sqlgap->gacmpkind == CPR_LA_ODBC) && 
		    (ldspact->ld.o.ld_intern_pos == ldspact->ld.o.ld_maxlen + 1) &&
		    (ldspact->ld.o.ld_vallen != 0))) {
	  if (((sqlxa->xaSQLDesc->PacketEncoding == sp77encodingUCS2) ||
	       (sqlxa->xaSQLDesc->PacketEncoding == sp77encodingUCS2Swapped)) &&
	      p04isasciidata (ldpcact->ldsfdtype)) {
	    ldspact->ld.o.ld_intern_pos = (ldpcact->ldsfbufpos / 2) + 1;
	  } else {
	    ldspact->ld.o.ld_intern_pos = ldpcact->ldsfbufpos + 1;
	  }
	}
      }

      /* Copies ldsp descriptor to datapart. */
      memcpy (data_ptr->variant.C_1.sp1p_buf_F + ph->sp1p_buf_len + 1, &ldspact->ld.n, desclen); 

      /* Set defined byte. */
      if (ldspact->ldkind == cpr_is_true)
	data_ptr->variant.C_1.sp1p_buf_F[ph->sp1p_buf_len] = (char) csp_long_desc_def_byte;
      else {
        if (!putclose && pr04LongIsSQL_DEFAULT (ldpcact))
	  data_ptr->variant.C_1.sp1p_buf_F[ph->sp1p_buf_len] = csp_default_byte;
        else
          data_ptr->variant.C_1.sp1p_buf_F[ph->sp1p_buf_len] = csp_defined_byte;
      }
      ph->sp1p_buf_len += (desclen + 1);

      /* D025086: MUT failed with -7065 if one longdescritor is send twice 
         with vm_lastdata.         
         Set to vm_alldata so it will be never send to the kernel by closing
         the descriptors. pr04LongCloseDescriptor will set the ld_valmode to 
         vm_lastdata if ld_valmode id not equal to vm_alldata */
      if (ldspact->ld.o.ld_valmode == vm_lastdata) 
        ldspact->ld.o.ld_valmode = vm_alldata;

      anz++;  /* Increase number of descriptors send to kernel. */
    }
  }
  if (putclose && (ibeg < iend)) {
    struct SQLSPLD *ldspact = &(*ld)->ldsp[ibeg];
    struct SQLPCLD *ldpcact = &(*ld)->ldpc[ldspact->ld.o.ld_valind-1];
    if (ldspact->ld.o.ld_valmode == vm_lastdata ||
        ldspact->ld.o.ld_valmode == vm_alldata) { /* PTS 1110697 */
      ldspact->ld.o.ld_valmode = vm_last_putval;

      memcpy (data_ptr->variant.C_1.sp1p_buf_F + ph->sp1p_buf_len + 1, 
              &ldspact->ld.n, desclen); 
      
      /* Set defined byte. */
      if (ldspact->ldkind == cpr_is_true)
        data_ptr->variant.C_1.sp1p_buf_F[ph->sp1p_buf_len] = (char) csp_long_desc_def_byte;
      else
        data_ptr->variant.C_1.sp1p_buf_F[ph->sp1p_buf_len] = csp_defined_byte;
      ph->sp1p_buf_len += (desclen + 1);
      anz++;  /* Increase number of descriptors send to kernel. */
    }
  }
  ph->sp1p_arg_count = anz;
  (*ld)->ldindsp = (*ld)->ldmaxo - anz + 1;
  s26finish_part ((tsp1_packet *) ga->gareqptr, data_ptr);
  pr03PacketReqRec (sqlxa->xaSQLDesc->ConDesc, sqlca->sqlemp); /* Send getval message to kernel. */
  if (sqlca->sqlemp->ereturncode != 0) {
    /* Error. */
    p03returncodeget (sqlca, sqlxa);
    sqlca->sqlerrd[2] = 0;
    sqlxa->xacmdinit = cpr_is_false;
    return false;
  }
  return true;
}

static bool pr04LongPutDesc (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, 
			     tsp1_cmd_mess_type_Param cmd, bool putclose)
{
  boolean ok;
  int desc_size, req_size, ibeg, iend, ldmax;
  tsp00_Int4 part_size;
  tsp1_part_ptr data_ptr;

  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp;

  switch (cmd) {
  case sp1m_putval:
    ldmax = (*ld)->ldmaxi;
    break;
  case sp1m_getval:
    ldmax = (*ld)->ldmaxo;
    break;
  default:
    ldmax = 0;
    break;
  }

  ibeg = 0;
  iend = ldmax;
  if (!putclose) {
    p03ccmdinit (sqlxa->xaSQLDesc, sqlca, ga, (char)cmd);  /* Initialize getval command. */
      
    s26new_part_init ((tsp1_packet *) ga->gareqptr, 
		      (tsp1_segment *) sqlca->sqlrap->rasegptr, &data_ptr);

    if (pr04LongPutDescPos (sqlca, sqlxa, ga, putclose, ibeg, iend, data_ptr) == false)
      return false;
  } else {
    ok = false;
    do {
      p03ccmdinit (sqlxa->xaSQLDesc, sqlca, ga, (char)cmd);  /* Initialize getval command. */
      
      s26new_part_init ((tsp1_packet *) ga->gareqptr, 
			(tsp1_segment *) sqlca->sqlrap->rasegptr, &data_ptr);
      
      part_size = s26size_new_part ((tsp1_packet *) ga->gareqptr, 
				    (tsp1_segment *) sqlca->sqlrap->rasegptr);
      
      desc_size = sizeof (struct OLDSQLSPLD) + 1;
      
      req_size = desc_size * (iend - ibeg);

      if (req_size <= part_size) {
	if (pr04LongPutDescPos (sqlca, sqlxa, ga, putclose, ibeg, iend, data_ptr) == false)
	  return false;
      } else {
	iend = ibeg + part_size / desc_size;
	if (pr04LongPutDescPos (sqlca, sqlxa, ga, putclose, ibeg, iend-1, data_ptr) == false)
	  return false;
	ibeg = iend;
      }
      if (iend >= ldmax)
	ok = true;
      iend = ldmax;
    } while (!ok);
  }
  return true;
}


/******************************************************************************
 *
 * pr04LongGetDesc: This function copies the long descriptors delivered by
 * the kernel into the descriptors of the ldsp array.
 *
 ******************************************************************************/

bool pr04LongGetDesc (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga)
{
  int argcnt, i, pos, desclen;
  tsp00_Int2 indsp;
  tsp00_Int4 lenval;
  tsp1_part_ptr data_ptr;
  tsp_int_map_c2 ind;
  const int cmpposvalind = 29;  /* Offset of ld_valind in struct ldsp. */
  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp;

  p03find_part (sqlca->sqlrap, sp1pk_longdata, &data_ptr);
  if (data_ptr == 0)
    return false;

  argcnt = data_ptr->variant.C_1.sp1p_part_header_F.sp1p_arg_count; /* Nr. of descriptors. */

  lenval = 0;
  pos = cmpposvalind;
  for (i = 0; i < argcnt; i++) {    
    /* Loop over returned long descriptors. Copy each descriptor identified
       by indsp into ldsp array. */

    /* Get desriptor index from datapart. */
    ind.variant.C_FALSE.map_c2_F[0] = data_ptr->sp1p_buf[pos];
    ind.variant.C_FALSE.map_c2_F[1] = data_ptr->sp1p_buf[pos+1];
    indsp = ind.variant.C_TRUE.map_int_F;

    /* Copy the length value of longdata delivered by kernel into lenval. */
    memcpy (&lenval, data_ptr->sp1p_buf+pos+8, mxsp_c4);
    desclen = ((*ld)->ldsp[indsp-1]).ldlength; /* Size of long descriptor. */

    /* Copy long descriptor delivrered by kernel into ldsp array. */
    memcpy (&(*ld)->ldsp[indsp-1].ld.o, &data_ptr->sp1p_buf[pos-cmpposvalind+1], 
	    desclen);

    pos += desclen + 1 + lenval;
  }
  return true;
}

void pr04LongFreeDesc (sqlmdesc *mfd)
{
  if (mfd) {
    mfd->descMaxelem = mfd->descNelem = 0;
    if (*mfd->descElemPtr) {
      pr03mFree (*mfd->descElemPtr, "descElemPtr");
      *mfd->descElemPtr = NULL;
    }
  }
}

bool pr04LongCloseIDescriptors (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga)
{
  /* D025086 WARNING:
     Do not use any CursorDesc in this function since it is not
     guarantee that a CursorDesc exist. */
#define tpr01_CursorDesc You_can_not_use_a_CursorDesc_in_this_function
  sqluns4 i;
  int j;
  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp; 

  if (*ld) {
    /* if ((*ld)->ldmaxi == 0) {
      (*ld)->ldmaxi = (*ld)->ldsparr_mdesc.descNelem;
      } */

    if ((*ld)->ldmaxi > 0) {
      for (i = 1; i <= (*ld)->ldpcarr_mdesc.descNelem; ) {
        pr04LongLoadIDescriptors (sqlca, i-1, (*ld)->ldmaxi);
        for (j = 0; j < (*ld)->ldmaxi; j++) {
          struct SQLSPLD *ldspact = &sqlca->sqlrap->rasqlldp->ldsp[j];
          struct SQLPCLD *ldpcact = &(*ld)->ldpc[ldspact->ld.o.ld_valind-1];
          
          ldpcact->ldhostlensum = 0; /* To satisfy if-condition in pr04LongPutDesc. */
          /* D025086: MUT failed with -7065 if one longdescritor is send
             twice with vm_lastdata.         
             Set all longdesc to vm_lastdata to close the descriptors.
             In pr04LongPutDesc the valmode will set to vm_alldata if 
             the vamode is vm_lastdata. This protect us to send a descriptor
             twice to the kernel with vm_lastdata set. */            
          if (ldspact->ld.o.ld_valmode != vm_alldata)
            ldspact->ld.o.ld_valmode = vm_lastdata;
        }
        if (pr04LongPutDesc (sqlca, sqlxa, ga, sp1m_putval, true) == false)
	  return false;
        i += (*ld)->ldmaxi;
      }
    }

    pr04LongFreeDesc (&(*ld)->ldpcarr_mdesc);
    pr04LongFreeDesc (&(*ld)->ldsparr_mdesc);
  }
  return true;
#ifdef tpr01_CursorDesc
#undef tpr01_CursorDesc
#endif
}

/******************************************************************************
 *
 * pr04LongCloseDescriptors: This function sends all long descriptors with
 * valmode == vm_close to the lernel to close the long columns. Then, memory
 * for restbuffers is deallocated.
 *
 ******************************************************************************/

bool pr04LongCloseODescriptors (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga)
{
  int i;
  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp; 

  if (*ld) {
    if ((*ld)->ldmaxo == 0)  /* There are no long descriptors to be closed. */
      return true;
    
    for (i = 0; i < (*ld)->ldmaxo; i++) {
      struct SQLSPLD *ldspact = &sqlca->sqlrap->rasqlldp->ldsp[i];
      struct SQLPCLD *ldpcact = &(*ld)->ldpc[ldspact->ld.o.ld_valind-1];
      
      ldpcact->ldhostlensum = 0; /* To satisfy if-condition in pr04LongPutDesc. */
      ldspact->ld.o.ld_valmode = vm_close;      
    }
    if (pr04LongPutDesc (sqlca, sqlxa, ga, sp1m_getval, true) == false)
      return false;
    
    /* Deallocate memory for restbuffers. */
    for (i = 0; i < (*ld)->ldmaxo; i++) {
      struct SQLPCLD *ldpcact = &(*ld)->ldpc[i];
      if (ldpcact->ldrestdata) {
        if (ldpcact->ldrestdata->rbbufpart) {
          pr03mFree(ldpcact->ldrestdata->rbbufpart, 
                    "ldpcact->ldrestdata->rbbufpart");
          ldpcact->ldrestdata->rbbufpart = NULL;
        }
        pr03mFree (ldpcact->ldrestdata,
                   "ldpcact->ldrestdata");
        ldpcact->ldrestdata = NULL;
      }
    }  
  }
  return true;
}


/******************************************************************************
 *
 * pr04LongCloseDescriptors: This function works as a wrapper function for
 * closing long descriptors by the use of tpr01_CursorDesc.
 *
 ******************************************************************************/

bool pr04LongCloseDescriptors (tpr01_ConDesc *ConDesc)
{
  if (pr04LongCloseIDescriptors (ConDesc->sqlca,
				 ConDesc->sqlxa,
				 ConDesc->ga) == false)
    return false;
  
  if (pr04LongCloseODescriptors (ConDesc->sqlca,
				 ConDesc->sqlxa,
				 ConDesc->ga) == false)
    return false;

  pr04LongInitLD (ConDesc->sqlca, ConDesc->sqlxa);
  ConDesc->sqlca->sqlrap->rasqlldp->ldorelastcmd = 0;
  return true;
}

void pr04LongRemoveDescriptors (sqlcatype *sqlca)
{
  if (sqlca->sqlemp->ereturncode != 0) {
    int i;
    struct SQLLD **ld = &sqlca->sqlrap->rasqlldp;
    
    if (*ld) {
      for (i = 0; i < (*ld)->ldmaxo; i++) {
        struct SQLPCLD *ldpcact = &(*ld)->ldpc[i];
        if (ldpcact->ldrestdata) {
          if (ldpcact->ldrestdata->rbbufpart)
            pr03mFree(ldpcact->ldrestdata->rbbufpart, 
                      "ldpcact->ldrestdata->rbbufpart");
	  ldpcact->ldrestdata->rbbufpart = NULL;
        }
        pr03mFree (ldpcact->ldrestdata,
                   "ldpcact->ldrestdata");
        ldpcact->ldrestdata = NULL;
      }  
      if (((*ld)->ldmaxi > 0) || ((*ld)->ldmaxo > 0)) {
        if ((*ld)->ldpc) {
          pr03mFree ((tsp00_Uint1 *)(*ld)->ldpc, "ldpc");
          (*ld)->ldpc = NULL;
        }
        if ((*ld)->ldsp) {
          pr03mFree ((tsp00_Uint1 *)(*ld)->ldsp, "ldsp");
          (*ld)->ldsp = NULL;
        }
      }

      (&(*ld)->ldpcarr_mdesc)->descNelem = 0;
      (&(*ld)->ldsparr_mdesc)->descNelem = 0;
      (*ld)->ldmaxi = 0;
      (*ld)->ldmaxo = 0;
    }
  }
}

/******************************************************************************
 *
 * pr04LongMoveToGetvalHostvar: This function moves longdata from the datapart
 * given by the kernel to the hostvariable indexed by ind.
 *
 ******************************************************************************/

void pr04LongMoveToGetvalHostvar (sqlcatype *sqlca, sqlxatype *sqlxa,
				  int ind, tsp1_part_ptr data_ptr, int lvcoffset)
{
#ifndef SQLODBC
  bool asciitrans = false;
  bool unitrans = false;
  short charlen, hcharlen;
  tsp78ConversionResult rc;
  tpr05_StringEncoding bufenc;
  tsp00_Uint4   cbWrite;    /* number of bytes written */
  tsp00_Uint4   cbParsed;   /* number of bytes parsed */
#endif
  struct SQLSPLD *ldspact = &sqlca->sqlrap->rasqlldp->ldsp[ind];
  struct SQLPCLD *ldpcact = &sqlca->sqlrap->rasqlldp->ldpc[ldspact->ld.o.ld_valind-1];
  struct SQLRESTBUF *buf = ldpcact->ldrestdata;

  int hostlen = ldpcact->ldhostlen;
  int hostlensum = ldpcact->ldhostlensum;  /* Amount of longdata already in hostvar. */
  char *hostaddr = (char *) ldpcact->ldhostaddr;
  int vallen = ldspact->ld.o.ld_vallen;  /* Length of longdata in datapart. */

  char *sp1p_buf_F = (char *) data_ptr->sp1p_buf; 
  hostaddr += hostlensum;
  sp1p_buf_F += ldspact->ld.o.ld_valpos - 1;  /* C-Style indexing */

#ifndef SQLODBC
  bufenc = pr04LongGetBufEncoding (ldpcact->ldsfdtype);

  hcharlen = 1;
  if ((p04isunidata (ldpcact->ldsfdtype) && pr04LongIsAsciiHostvar (ldpcact)) ||
      (((bufenc == sp77encodingUCS2) || (bufenc == sp77encodingUCS2Swapped)) &&
       (!p04isunidata (ldpcact->ldsfdtype) && pr04LongIsAsciiHostvar (ldpcact)))) {
    unitrans = true;
    charlen = 2;
  } else {
    unitrans = false;
    charlen = 1;
  }

  if ((bufenc == sp77encodingAscii) && 
      pr04LongIsUnicodeHostvar (ldpcact) &&
      p04isasciidata (ldpcact->ldsfdtype)) {
    asciitrans = true;
    hcharlen = 2;
  }
#endif

  if (hostlensum == 0) {
    /* First time data is moved to this hostvariable.
       The first byte(s) of hostvariable are length information. Data starts
       after these length bytes, so increase by lvcoffset. */
    hostaddr += lvcoffset;
    ldpcact->ldhostlensum += lvcoffset;
    hostlensum += lvcoffset;
  }

  if (buf->rbrestlen <= buf->rbbufsize) {
    /* There is data in restbuffer to move first to hostvariable. */
#ifndef SQLODBC
    if ((hostlen - hostlensum) / hcharlen >= (buf->rbbufsize - buf->rbrestlen) / charlen) 
#else
    if ((hostlen - hostlensum) >= (buf->rbbufsize - buf->rbrestlen)) 
#endif
    {
      /* Data from restbuffer fits completely into hostvariable. */
      if ((ldpcact->ldhosttype == cpr_vfile) ||
	  (ldpcact->ldhosttype == cpr_vfilec)) {
	p03datafwrite (sqlca->sqlrap, ldpcact->ldfileno, 
		       buf->rbbufsize - buf->rbrestlen,
		       (char *) buf->rbbufpart + buf->rbrestlen, 
		       1,  sqlca->sqlemp);
#ifndef SQLODBC
	if (unitrans) {
	  rc = sp78convertString (sp77encodingAscii,
				  hostaddr,
				  hostlen,
				  &cbWrite,
				  FALSE,
				  sp77encodingUCS2Native,
				  (char *) buf->rbbufpart + buf->rbrestlen,
				  buf->rbbufsize - buf->rbrestlen,
				  &cbParsed);
	  if (rc != sp78_Ok) {
	    p08runtimeerror (sqlca, sqlxa,
			     cpr_conv_from_unicode_impossible);
	  }
	} else if (asciitrans) {
	  rc = sp78convertString (sp77encodingUCS2Native,
				  hostaddr,
				  hostlen,
				  &cbWrite,
				  FALSE,
				  sp77encodingAscii,
				  (char *) buf->rbbufpart + buf->rbrestlen,
				  buf->rbbufsize - buf->rbrestlen,
				  &cbParsed);
	  if (rc != sp78_Ok) {
	    p08runtimeerror (sqlca, sqlxa,
			     cpr_conv_from_unicode_impossible);
	  }
	} else {	
	  memcpy (hostaddr, (char *) buf->rbbufpart + buf->rbrestlen, 
		  buf->rbbufsize - buf->rbrestlen);
	}
      }
#else
      } else {	
        memcpy (hostaddr, (char *) buf->rbbufpart + buf->rbrestlen, 
	        buf->rbbufsize - buf->rbrestlen);
      }      
#endif
      ldpcact->ldhostlensum += buf->rbbufsize - buf->rbrestlen;
      ldpcact->ldsfbufpos += buf->rbbufsize - buf->rbrestlen;
      buf->rbrestlen = buf->rbbufsize+1;
    } else if ((hostlen - hostlensum) > 0) {
      /* Data from restbuffer doesn't fit completely into hostvariable.
         Move next portion of restbuffer into hostvariable. */
      if ((ldpcact->ldhosttype == cpr_vfile) ||
	  (ldpcact->ldhosttype == cpr_vfilec)) {
	p03datafwrite (sqlca->sqlrap, ldpcact->ldfileno, 
		       hostlen - hostlensum,
		       (char *) buf->rbbufpart + buf->rbrestlen, 
		       1,  sqlca->sqlemp);
      } else {
	memcpy (hostaddr, (char *) buf->rbbufpart + buf->rbrestlen, 
		hostlen - hostlensum);
      }
      ldpcact->ldhostlensum += hostlen - hostlensum;
      ldpcact->ldsfbufpos += hostlen - hostlensum;
      buf->rbrestlen += hostlen - hostlensum;
    }   
  } else {
    /* Handling of restbuffer finished. No more data from restbuffer to move
       to hostvariable, so handle data from datapart. */
#ifndef SQLODBC
    if ((hostlen - hostlensum) >= ldspact->ld.o.ld_vallen / charlen) 
#else
    if ((hostlen - hostlensum) >= ldspact->ld.o.ld_vallen) 
#endif
    {
      /* Data from datapart fits completly into hostvariable. Copy all the 
         data into hostvariable. */
      if ((ldpcact->ldhosttype == cpr_vfile) ||
	  (ldpcact->ldhosttype == cpr_vfilec)) {
	p03datafwrite (sqlca->sqlrap, ldpcact->ldfileno, 
		       ldspact->ld.o.ld_vallen,
		       (char *) &data_ptr->sp1p_buf[ldspact->ld.o.ld_valpos-1], 
		       1,  sqlca->sqlemp);
      } else {
#ifndef SQLODBC
	if (unitrans) {
	  rc = sp78convertString (sp77encodingAscii,
				  hostaddr,
				  hostlen,
				  &cbWrite,
				  FALSE,
				  sp77encodingUCS2Native,
				  sp1p_buf_F,
				  PR07MIN((hostlen - hostlensum) * charlen, vallen),
				  &cbParsed);
	  if (rc != sp78_Ok) {
	    p08runtimeerror (sqlca, sqlxa,
			     cpr_conv_from_unicode_impossible);
	  }
	} else if (asciitrans) {
	  rc = sp78convertString (sp77encodingUCS2Native,
				  hostaddr,
				  hostlen,
				  &cbWrite,
				  FALSE,
				  sp77encodingAscii,
				  sp1p_buf_F,
				  PR07MIN((hostlen - hostlensum) / hcharlen, vallen),
				  &cbParsed);
	  if (rc != sp78_Ok) {
	    p08runtimeerror (sqlca, sqlxa,
			     cpr_conv_from_unicode_impossible);
	  }
	} else {
	  memcpy (hostaddr, &data_ptr->sp1p_buf[ldspact->ld.o.ld_valpos-1], 
		  ldspact->ld.o.ld_vallen);
	}
#else
	memcpy (hostaddr, &data_ptr->sp1p_buf[ldspact->ld.o.ld_valpos-1], 
		ldspact->ld.o.ld_vallen);
#endif
      }
#ifndef SQLODBC
      ldpcact->ldhostlensum += ldspact->ld.o.ld_vallen / charlen;
#else
      ldpcact->ldhostlensum += ldspact->ld.o.ld_vallen;
#endif
      ldpcact->ldsfbufpos += ldspact->ld.o.ld_vallen;
    } else {
      /* Data from datapart doesn't fit into hostvariable. Move
         one portion into hostvariable. */
      if ((ldpcact->ldhosttype == cpr_vfile) ||
	  (ldpcact->ldhosttype == cpr_vfilec)) {
	p03datafwrite (sqlca->sqlrap, ldpcact->ldfileno, 
		       hostlen - hostlensum,
		       (char *) &data_ptr->sp1p_buf[ldspact->ld.o.ld_valpos-1], 
		       1,  sqlca->sqlemp);
      } else {
	memcpy (hostaddr, &data_ptr->sp1p_buf[ldspact->ld.o.ld_valpos-1], 
		hostlen - hostlensum);
      }
      ldpcact->ldhostlensum += hostlen - hostlensum;
      ldpcact->ldsfbufpos += hostlen - hostlensum;      
    }
  }
}


/******************************************************************************
 *
 * pr04LongBufHandle: This function moves the contents of the restbuffer, if
 * any, to the hostvariable.
 * 
 ******************************************************************************/

#ifndef SQLODBC
void pr04LongBufHandle (sqlcatype *sqlca, sqlxatype *sqlxa, int ind, bool *flag, int lvcoffset)
#else
void pr04LongBufHandle (sqlcatype *sqlca, int ind, bool *flag, int lvcoffset)
#endif
{
#ifndef SQLODBC
  bool asciitrans = false;
  bool unitrans = false;
  short charlen, hcharlen;
  tsp78ConversionResult rc;
  tpr05_StringEncoding bufenc;
  tsp00_Uint4   cbWrite;    /* number of bytes written */
  tsp00_Uint4   cbParsed;   /* number of bytes parsed */
#endif
  /* Get actual ldsp and ldpc descriptors for the actual long descriptor. */
  struct SQLSPLD *ldspact = &sqlca->sqlrap->rasqlldp->ldsp[ind];
  struct SQLPCLD *ldpcact = &sqlca->sqlrap->rasqlldp->ldpc[ldspact->ld.o.ld_valind-1];
  struct SQLRESTBUF *buf = ldpcact->ldrestdata;

  int hostlen = ldpcact->ldhostlen;
  int hostlensum = ldpcact->ldhostlensum;
  char *hostaddr = (char *) ldpcact->ldhostaddr;
  hostaddr += hostlensum;

#ifndef SQLODBC
  bufenc = pr04LongGetBufEncoding (ldpcact->ldsfdtype);

  hcharlen = 1;
  if ((p04isunidata (ldpcact->ldsfdtype) && pr04LongIsAsciiHostvar (ldpcact)) ||
      (((bufenc == sp77encodingUCS2) || (bufenc == sp77encodingUCS2Swapped)) &&
       (!p04isunidata (ldpcact->ldsfdtype) && pr04LongIsAsciiHostvar (ldpcact)))) {
    unitrans = true;
    charlen = 2;
  } else {
    unitrans = false;
    charlen = 1;
  }

  if ((bufenc == sp77encodingAscii) && 
      pr04LongIsUnicodeHostvar (ldpcact) &&
      p04isasciidata (ldpcact->ldsfdtype)) {
    asciitrans = true;
    hcharlen = 2;
  }
#endif

  if (hostlensum == 0) {
    /* First time data is moved to this hostvariable.
       The first byte(s) of hostvariable are length information. Data starts
       after these length bytes, so increase by lvcoffset. */
    hostaddr += lvcoffset;
    ldpcact->ldhostlensum += lvcoffset;
    hostlensum += lvcoffset;
  }

  if (buf->rbrestlen <= buf->rbbufsize) {
    /* Not all of the restbuffer has been read. */
#ifndef SQLODBC
    if ((hostlen - hostlensum) / hcharlen >= (buf->rbbufsize - buf->rbrestlen) / charlen) 
#else
    if ((hostlen - hostlensum) >= (buf->rbbufsize - buf->rbrestlen)) 
#endif
    {
      /* Unread restbuffer contents fits completely into hostvariable. */
      if ((ldpcact->ldhosttype == cpr_vfile) ||
	  (ldpcact->ldhosttype == cpr_vfilec)) {
	p03datafwrite (sqlca->sqlrap, ldpcact->ldfileno, 
		       buf->rbbufsize - buf->rbrestlen,
		       (char *) buf->rbbufpart + buf->rbrestlen, 
		       1,  sqlca->sqlemp);
      } else {
#ifndef SQLODBC
	if (unitrans) {
	  rc = sp78convertString (sp77encodingAscii,
				  hostaddr,
				  hostlen,
				  &cbWrite,
				  FALSE,
				  sp77encodingUCS2Native,
				  (char *) buf->rbbufpart + buf->rbrestlen,
				  buf->rbbufsize - buf->rbrestlen,
				  &cbParsed);
	  if (rc != sp78_Ok) {
	    p08runtimeerror (sqlca, sqlxa,
			     cpr_conv_from_unicode_impossible);
	  }
	} else if (asciitrans) {
	  rc = sp78convertString (sp77encodingUCS2Native,
				  hostaddr,
				  hostlen,
				  &cbWrite,
				  FALSE,
				  sp77encodingAscii,
				  (char *) buf->rbbufpart + buf->rbrestlen,
				  buf->rbbufsize - buf->rbrestlen,
				  &cbParsed);
	  if (rc != sp78_Ok) {
	    p08runtimeerror (sqlca, sqlxa,
			     cpr_conv_from_unicode_impossible);
	  }
	} else {
	  memcpy (hostaddr, (char *) buf->rbbufpart + buf->rbrestlen, 
		  buf->rbbufsize - buf->rbrestlen);
	}
#else
	memcpy (hostaddr, (char *) buf->rbbufpart + buf->rbrestlen, 
		buf->rbbufsize - buf->rbrestlen);
#endif
      }
#ifndef SQLODBC
      ldpcact->ldhostlensum += (buf->rbbufsize - buf->rbrestlen) / charlen;
      ldpcact->ldsfbufpos += (buf->rbbufsize - buf->rbrestlen);
#else
      ldpcact->ldhostlensum += (buf->rbbufsize - buf->rbrestlen);
#endif
      buf->rbrestlen = buf->rbbufsize+1;
    } else {
      /* Not all of the unread restbuffer fits into hostvariable. */
      if ((ldpcact->ldhosttype == cpr_vfile) ||
	  (ldpcact->ldhosttype == cpr_vfilec)) {
	p03datafwrite (sqlca->sqlrap, ldpcact->ldfileno, 
		       hostlen - hostlensum,
		       (char *) buf->rbbufpart + buf->rbrestlen, 
		       1,  sqlca->sqlemp);
      } else {
#ifndef SQLODBC
	if (unitrans) {
	  rc = sp78convertString (sp77encodingAscii,
				  hostaddr,
				  hostlen,
				  &cbWrite,
				  FALSE,
				  sp77encodingUCS2Native,
				  (char *) buf->rbbufpart + buf->rbrestlen,
				  (hostlen - hostlensum) * charlen,
				  &cbParsed);
	  if (rc != sp78_Ok) {
	    p08runtimeerror (sqlca, sqlxa,
			     cpr_conv_from_unicode_impossible);
	  }
	} else if (asciitrans) {
	  rc = sp78convertString (sp77encodingUCS2Native,
				  hostaddr,
				  hostlen,
				  &cbWrite,
				  FALSE,
				  sp77encodingAscii,
				  (char *) buf->rbbufpart + buf->rbrestlen,
				  (hostlen - hostlensum) / hcharlen,
				  &cbParsed);
	  if (rc != sp78_Ok) {
	    p08runtimeerror (sqlca, sqlxa,
			     cpr_conv_from_unicode_impossible);
	  }
	} else {
	  memcpy (hostaddr, (char *) buf->rbbufpart + buf->rbrestlen, 
		  hostlen - hostlensum);
	}
#else
	memcpy (hostaddr, (char *) buf->rbbufpart + buf->rbrestlen, 
		hostlen - hostlensum);
#endif
      }
      ldpcact->ldhostlensum += hostlen - hostlensum;
#ifndef SQLODBC
      ldpcact->ldsfbufpos += (hostlen - hostlensum) * charlen;
      buf->rbrestlen += (hostlen - hostlensum) * charlen;
#else
      ldpcact->ldsfbufpos += hostlen - hostlensum;
      buf->rbrestlen += hostlen - hostlensum;
#endif
    }   
  } 
  
  if ((ldpcact->ldhostlen > ldpcact->ldhostlensum) &&
      (ldspact->ld.o.ld_valmode != vm_close)) {
    /* Restbuffer is handled, but hostvariable not filled completely.
       Therefore ask kernel for more longdata. */

    /* ld_vallen is the amount of data that is wanted from the kernel. */
#ifndef SQLODBC
    ldspact->ld.o.ld_vallen = (ldpcact->ldhostlen - ldpcact->ldhostlensum) * charlen;
#else
    ldspact->ld.o.ld_vallen = (ldpcact->ldhostlen - ldpcact->ldhostlensum);
#endif
    /* To prevent kernel to close the long column, set ld_infoset to
       ld_no_close. */
    ldspact->ld.o.ld_infoset |= (1 << ld_no_close);
    ldspact->ld.o.ld_valmode = vm_nodata;
    *flag = true;
  } else {
    /* Hostvariable is filled completely, dont't ask kernel for more longdata. */
    ldspact->ld.o.ld_vallen = 0;
    *flag = false;
  }
}


/******************************************************************************
 *
 * pr04Long_lvc02: This function fills the hostvariable indexed via ind in the
 * ldpc and ldsp arrays with longdata and determines if the kernel has to be
 * asked for more longdata to fill the hostvariable. In that case, the flag
 * getnext is set to true.
 *
 ******************************************************************************/

bool pr04Long_lvc02 (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga,
		     int ind, bool *getnext, int lvcoffset)
{
  tsp1_part_ptr data_ptr;

  /* Get actual ldsp and ldpc descriptors for the actual long descriptor. */
  struct SQLSPLD *ldspact = &sqlca->sqlrap->rasqlldp->ldsp[ind];
  struct SQLPCLD *ldpcact = &sqlca->sqlrap->rasqlldp->ldpc[ldspact->ld.o.ld_valind-1];

  p03find_part (sqlca->sqlrap, sp1pk_longdata, &data_ptr);
  if (data_ptr == 0)
    return false;

  /* Depending on ld_valmode, which is sent back from the kernel, 
     fill hostvariable. */
  switch (ldspact->ld.o.ld_valmode) {
  case vm_close:
    /* Long descriptor has been closed. All data for long column delivered
       by kernel. */
    if (ldpcact->ldrestdata->rbrestlen > ldpcact->ldrestdata->rbbufsize) {
      /* All of the restbuffer has been read. No more data needed from kernel. */
      *getnext = false;
    } else if (ldpcact->ldrestdata->rbbufsize != 0) {
      /* Move content of restbuffer to hostvariable. No more data needed from kernel. */
      pr04LongMoveToGetvalHostvar (sqlca, sqlxa, ind, data_ptr, lvcoffset);
      *getnext = false;
    }
    break;
  case vm_datapart:
  case vm_data_trunc:
    /* Next portion of longdata. */
    if ((data_ptr == 0) && 
	(ldpcact->ldrestdata->rbrestlen >= ldpcact->ldrestdata->rbbufsize)) {
      /* No more data to read in restbuffer. */
      if (ldpcact->ldhostlensum < ldpcact->ldhostlen) {
	/* The hostvariable has not been filled completly, so ask kernel
	   for more longdata. */

	/* ld_vallen is the amount of data that is wanted from the kernel. */
	ldspact->ld.o.ld_vallen = ldpcact->ldhostlen - ldpcact->ldhostlensum;
#ifndef SQLODBC
	if (p04isunidata (ldpcact->ldsfdtype) && pr04LongIsAsciiHostvar (ldpcact)) {
	  ldspact->ld.o.ld_vallen *= 2;
	}
#endif
	/* To prevent kernel to close the long column, set ld_infoset to
	   ld_no_close. */
	ldspact->ld.o.ld_infoset |= (1 << ld_no_close);
	ldspact->ld.o.ld_valmode = vm_nodata;
      }      
      *getnext = true;
    } else {
      /* Move data from restbuffer to hostvariable. */
      pr04LongMoveToGetvalHostvar (sqlca, sqlxa, ind, data_ptr, lvcoffset);
      if (ldpcact->ldhostlensum < ldpcact->ldhostlen) {
	/* The hostvariable has not been filled completly, so ask kernel
	   for more longdata. */

	ldspact->ld.o.ld_vallen = ldpcact->ldhostlen - ldpcact->ldhostlensum;
#ifndef SQLODBC
	if (p04isunidata (ldpcact->ldsfdtype) && pr04LongIsAsciiHostvar (ldpcact)) {
	  ldspact->ld.o.ld_vallen *= 2;
	}
#endif
	ldspact->ld.o.ld_infoset |= (1 << ld_no_close);
	ldspact->ld.o.ld_valmode = vm_nodata;
	*getnext = true;
      } else {
	/* Hostvariable has been filled completely. */
	*getnext = false;
      }
    }
    break;
  case vm_lastdata:
    /* All data from the longcolumn has been read. Move data to
       hostvariable, don't ask kernel for more data, close long descriptor. */
    pr04LongMoveToGetvalHostvar (sqlca, sqlxa, ind, data_ptr, lvcoffset);
    if ((data_ptr == 0) &&
	(ldpcact->ldhostlen > ldpcact->ldhostlensum)) {
      /* Hostvariable is not filled completely. */
      pr04LongMoveToGetvalHostvar (sqlca, sqlxa, ind, data_ptr, lvcoffset);
    }
    ldspact->ld.o.ld_valmode = vm_close;
    *getnext = false;
    break;
  case vm_nodata:
    /* The kernel has not delivered any data for the long descriptor yet.
       Ask for more data. */
    *getnext = true;
    break;
  default:
    /* Should never happen, but permits endless loop in case of error. */
    *getnext = false;
    break;
  }
  return true;
}


/******************************************************************************
 *
 * pr04LongSetHostvarLength: This function moves the actual length of data
 * in hostvariable to the first bytes of the hostvariable, depending of type.
 *
 ******************************************************************************/

void pr04LongSetHostvarLength (sqlcatype *sqlca, sqlxatype *sqlxa, 
                               struct SQLPCLD *ldpcact, int lvcoffset)
{
  int lpind;
  int actrow = (ldpcact->ldrowno == 0) ? 0 : (ldpcact->ldrowno - 1);
  int actcol = ldpcact->ldcolno - 1;
  sqllongdesc *ldesc;

  tsp00_Uint1 vallen1;
  tsp00_Int2 vallen2;
  tsp00_Int4 vallen4;

  if (ldpcact->ldhosttype == cpr_vlong_desc) {
    if (sqlxa->xaSQLDesc->ka->kapacount <= 0) {
      switch (sqlca->sqldbmode) {
      case cpr_kind_oracle:
      case cpr_kind_sapr3: {
        struct SQLDA *da = (struct SQLDA *) sqlca->sqlcxap->xasqldap; 
        lpind = actrow; 
        
        ldesc = (sqllongdesc *) (da->V[actcol] + lpind * da->L[actcol]);
        ldesc->cbBufLen = ldpcact->ldhostlensum;
        break; }
      case cpr_kind_internal: {
        sqldatype *da = (sqldatype *) sqlca->sqlcxap->xasqldap; 
        lpind = actrow; 
        
        ldesc = (sqllongdesc *) ((char *) da->sqlvar[actcol].hostvaraddr + lpind * da->sqlvar[actcol].hostcolsize);
        ldesc->cbBufLen = ldpcact->ldhostlensum;
        break; }
      default:
        break;
      }
    } else {
      tsp00_Uint1 *varaddr = 0;
      tsp00_Uint1 *indaddr = 0;
      tsp00_Int4 varsize;
      tsp00_Int2 vartyp;
      
      pr04LongGetHostAddr (sqlca, sqlxa, actrow, actcol, &varaddr, &varsize, &vartyp, &indaddr);
      ldesc = (sqllongdesc *) varaddr;
      ldesc->cbBufLen = ldpcact->ldhostlensum;
    }    
  } else {
    switch (lvcoffset) {
    case 4:
      vallen4 = ldpcact->ldhostlensum - lvcoffset;
      memcpy (ldpcact->ldhostaddr, &vallen4, sizeof (tsp00_Int4));
      break;
    case 2:
      vallen2 = ldpcact->ldhostlensum - lvcoffset;
      memcpy (ldpcact->ldhostaddr, &vallen2, sizeof (tsp00_Int2));
      break;
    case 1:
      vallen1 = ldpcact->ldhostlensum - lvcoffset;
      memcpy (ldpcact->ldhostaddr, &vallen1, sizeof (tsp00_Uint1));
      break;
    default:    
      break;
    }
  }
}

void pr04LongGetColLength (sqlcatype *sqlca, sqlxatype *sqlxa, int i)
{
  int lpind;
  sqllongdesc *ldesc = 0;
  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp;
  struct SQLSPLD *ldspact = &(*ld)->ldsp[i];
  struct SQLPCLD *ldpcact = &(*ld)->ldpc[ldspact->ld.o.ld_valind-1];
  int actrow = (ldpcact->ldrowno == 0) ? 0 : (ldpcact->ldrowno - 1);
  int actcol = ldpcact->ldcolno - 1;

  if (ldpcact->ldhosttype == cpr_vlong_desc) {
    if (sqlxa->xaSQLDesc->ka->kapacount <= 0) {
      switch (sqlca->sqldbmode) {
      case cpr_kind_oracle:
      case cpr_kind_sapr3: {
        struct SQLDA *da = (struct SQLDA *) sqlca->sqlcxap->xasqldap; 
        lpind = actrow;      
        
        ldesc = (sqllongdesc *) (da->V[actcol] + lpind * da->L[actcol]);
        break; }
      case cpr_kind_internal: {
        sqldatype *da = (sqldatype *) sqlca->sqlcxap->xasqldap; 
        lpind = actrow;      
        
        ldesc = (sqllongdesc *) da->sqlvar[actcol].hostvaraddr + lpind * da->sqlvar[actcol].hostcolsize;
        break; }
      default:
        break; }
    } else {
      tsp00_Uint1 *varaddr = 0;
      tsp00_Uint1 *indaddr = 0;
      tsp00_Int4 varsize;
      tsp00_Int2 vartyp;
      
      pr04LongGetHostAddr (sqlca, sqlxa, actrow, actcol, &varaddr, &varsize, &vartyp, &indaddr);
      ldesc = (sqllongdesc *) varaddr;
    }
    if (ldesc) {
      ldesc->cbColLen = ldspact->ld.o.ld_maxlen;
      if (((pr04LongGetBufEncoding (ldpcact->ldsfdtype) == sp77encodingUCS2) ||
	   (pr04LongGetBufEncoding (ldpcact->ldsfdtype) == sp77encodingUCS2Swapped)) &&
	  !p04isunidata (ldpcact->ldsfdtype))
	ldesc->cbColLen *= 2;
    }
  }
  
  return;
}

void pr04LongGetHostInfoFromDA (sqlcatype *sqlca, sqlxatype * sqlxa, int i)
{
  int lpind;
  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp;
  struct SQLSPLD *ldspact = &(*ld)->ldsp[i];
  /* SQLPCLD *ldpcact = &(*ld)->ldpc[ldspact->ld.o.ld_valind-1]; */
  struct SQLPCLD *ldpcact = &(*ld)->ldpc[i];
  int actrow = (ldpcact->ldrowno == 0) ? 0 : (ldpcact->ldrowno - 1);
  int actcol = ldpcact->ldcolno - 1;

  if (sqlca->sqlgap->gacmpkind == CPR_LA_ODBC)
    return;
  
  if (sqlxa->xaSQLDesc->ka->kapacount <= 0) {
    switch (sqlca->sqldbmode) {
    case cpr_kind_oracle:
    case cpr_kind_sapr3: {
      struct SQLDA *da = (struct SQLDA *) sqlca->sqlcxap->xasqldap; 
      char ival = 0;
      tsp00_Int2 vt;
      short type_code = 0;
      int null_status = 0;
      
      lpind = actrow; 
      ldpcact->ldhostlen = da->L[actcol];
      ldpcact->ldhostaddr = da->V[actcol] + lpind * da->L[actcol];
      ldpcact->ldindkaddr = ((da->I) ? da->I[actcol] : 0) ?
        ((char *) da->I[actcol] + lpind * sizeof (short)) : &ival;

      if (ldpcact->ldindkaddr == &ival) {
	ldpcact->ldindkaddr = 0;
	ldpcact->ldindktype = CPR_VINT2;
	ldpcact->ldindklen = 2;
	ldpcact->ldindkfrac = 0;
      }
      sqlnul ((short*)&da->T[actcol], &type_code, &null_status);      
      if (sqlxa->xaModDesc) 
        p04OraTypes2Internal (type_code, da->L[actcol], &vt, sqlxa->xaModDesc->PrecompiledforUnicode);
      else
        p04OraTypes2Internal (type_code, da->L[actcol], &vt, PrecompiledForAscii_epr01);
      ldpcact->ldhosttype = vt;
      break; }
    case cpr_kind_internal: {
      sqldatype *da = (sqldatype *) sqlca->sqlcxap->xasqldap;
      char ival = 0;
      sqlvartype *actvar = &da->sqlvar[actcol];
      
      lpind = actrow; 
      ldpcact->ldhostlen = actvar->hostcolsize;
      ldpcact->ldhostaddr = (char *) actvar->hostvaraddr + lpind * actvar->hostcolsize;
      ldpcact->ldindkaddr = (actvar->hostindaddr)
        ? ((tsp00_Int4 *) (actvar->hostindaddr + lpind)) : (int *) &ival;

      if (ldpcact->ldindkaddr == &ival) {
	ldpcact->ldindkaddr = 0;
	ldpcact->ldindktype = CPR_VINT2;
	ldpcact->ldindklen = 2;
	ldpcact->ldindkfrac = 0;
      }
      ldpcact->ldhosttype = actvar->hostvartype;
      break; }
    default:
      break; }
  } else {
    tsp00_Uint1 *varaddr = 0;
    tsp00_Uint1 *indaddr = 0;
    tsp00_Int4 varsize;
    tsp00_Int2 vartyp;

    pr04LongGetHostAddr (sqlca, sqlxa, actrow, actcol, &varaddr, &varsize, &vartyp, &indaddr);
    
    ldpcact->ldhostlen = varsize;
    ldpcact->ldhostaddr = varaddr;
    ldpcact->ldindkaddr = indaddr;
    ldpcact->ldhosttype = vartyp;
  }
  if (ldpcact->ldindkaddr) {
    if (pr04LongIsSQL_IGNORE(ldpcact)) {
      ldpcact->ldhostlen = 0;
    }
  }
}


/******************************************************************************
 *
 * pr04LongGetvalOutput: This function is the main entry point for piecewise
 * reading of long columns during an EXEC SQL GETVAL command.
 * First, the hostvariables are filled with the contents of the restbuffers
 * for each long descriptor filled by function pr04LongOutputLvc (if any).
 * After that, the hostvariables, which memory addresses are given in the ldpc
 * array of the ld descriptor, are filled with the next portion of longdata read
 * from the long columns. As long as there is at least one hostvariable for which
 * data exists in the longcolumn, but is not delivered via this variable, the 
 * data trunc warning flag in sqlca is set.
 * A positive spindex causes reading of longdata for the long column with that
 * index only. Only long columns are considered for spindex.
 *
 ******************************************************************************/

bool pr04LongGetvalOutput (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, int spindex)
{
  bool getnext, flag, trunc;
  int i, rowno, lvcoffset, ibeg, incr;
  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp;

  if (spindex > 0) {
    /* Get next portion of data for long column with index spindex only. */
    ibeg = spindex - 1; /* start with descriptor for that column in row 1. */

    /* Increment to handle only descriptors for col spindex. */
    if (sqlca->sqlgap->gacmpkind == CPR_LA_ODBC)
      incr = (*ld)->ldmaxo;  
    else
      incr = (*ld)->ldmaxo / (*ld)->lderrd3;
  } else {
    /* Get next portion of data for all long columns. */
    ibeg = 0;
    incr = 1;
  }

  trunc = false;
  getnext = false;
  flag = false;

  if (pr04LongCheckParamNumber (sqlca, sqlxa) == false)
    return false;

  /* First read contents of restbuffers. */
  for (i = ibeg; i < (*ld)->ldmaxo; i += incr) {
    int oldlen;
    struct SQLSPLD *ldspact = &(*ld)->ldsp[i];
    struct SQLPCLD *ldpcact = &(*ld)->ldpc[ldspact->ld.o.ld_valind-1];

    if ((sqlca->sqlgap->gacmpkind == CPR_LA_ODBC) &&
	(ldspact->ld.o.ld_intern_pos < ldspact->ld.o.ld_maxlen + 1)) {
      if (((sqlxa->xaSQLDesc->PacketEncoding == sp77encodingUCS2) ||
	  (sqlxa->xaSQLDesc->PacketEncoding == sp77encodingUCS2Swapped)) &&
	p04isasciidata (ldpcact->ldsfdtype)) {
	ldpcact->ldsfbufpos = (ldspact->ld.o.ld_intern_pos * 2) - 1;
      } else {
	ldpcact->ldsfbufpos = ldspact->ld.o.ld_intern_pos - 1;
      }
    }

    oldlen = ldpcact->ldhostlen;

    (*ld)->ldindpc = i+1;

    pr04LongGetHostInfoFromDA (sqlca, sqlxa, i);

    pr04LongGetHostInfoOff (sqlca, sqlxa, &lvcoffset);

    ldpcact->ldhostlensum = 0;

#ifndef SQLODBC
    pr04LongBufHandle (sqlca, sqlxa, i, &flag, lvcoffset);
#else
    pr04LongBufHandle (sqlca, i, &flag, lvcoffset);
#endif
    if (flag) {
      /* More data needed to fill hostvariable completely. */
      getnext = true;
    }
  }

  while (getnext) {
    /* Ask kernel for more longdata. */
    if (pr04LongPutDesc (sqlca, sqlxa, ga, sp1m_getval, false) == false)
      return false;
    if (pr04LongGetDesc (sqlca, sqlxa, ga) == false)
      return false;

    flag = false;
    getnext = false;
    for (i = ibeg; i < (*ld)->ldmaxo; i += incr) {
      /* Fill hostvariables with longdata. */
      if (pr04Long_lvc02 (sqlca, sqlxa, ga, i, &flag, lvcoffset) == false)
        return false;
      if (flag) {
	/* More data needed to fill hostvariable completely. */
	getnext = true;
      }
    }
  }

  rowno = -1;
  for (i = ibeg; i < (*ld)->ldmaxo; i += incr) {
    struct SQLSPLD *ldspact = &(*ld)->ldsp[i];
    struct SQLPCLD *ldpcact = &(*ld)->ldpc[ldspact->ld.o.ld_valind-1];

    pr04LongSetHostvarLength (sqlca, sqlxa, ldpcact, lvcoffset);

    pr04LongGetColLength (sqlca, sqlxa, i);

    if (/* ldpcact->ldindktype != SQL_IGNORE */ !pr04LongIsSQL_IGNORE(ldpcact))
      pr04LongWriteTracePart (sqlca, sqlxa, ldpcact, lvcoffset, &rowno, cpr_tr_outp);

    if (/* ldpcact->ldindktype != SQL_IGNORE */ !pr04LongIsSQL_IGNORE(ldpcact))
      if ((ldspact->ld.o.ld_valmode != vm_close) ||
	  ((ldspact->ld.o.ld_valmode == vm_close) &&
	   (ldpcact->ldrestdata->rbrestlen < ldpcact->ldrestdata->rbbufsize))) {
	/* There is longdata that doesn't fit into hostvariable. 
	   Data was truncated. */
	if (sqlca->sqlgap->gacmpkind != CPR_LA_ODBC)
	  pr04LongTraceTrunc (sqlca, ldpcact->ldhostlensum - lvcoffset);
	trunc = true;
      }
    if (sqlca->sqlgap->gacmpkind == CPR_LA_ODBC) {
      if (((sqlxa->xaSQLDesc->PacketEncoding == sp77encodingUCS2) ||
	  (sqlxa->xaSQLDesc->PacketEncoding == sp77encodingUCS2Swapped)) &&
	p04isasciidata (ldpcact->ldsfdtype)) {
	ldspact->ld.o.ld_intern_pos = (ldpcact->ldsfbufpos / 2) + 1;
      } else {
	ldspact->ld.o.ld_intern_pos = ldpcact->ldsfbufpos + 1;
      }      
    }
  }

  if (trunc) {
    /* Set the apropriate flag of sqlca to 'W', indicating that at least
       one hostvariable has been truncated. */

    char *p = (char *) sqlca;
    p += 124;
    *(p++) = 'W';
    *p = 'W';
  }

  sqlxa->xacmdinit = cpr_is_false; /* Getval command is finished. */
  return true;
}


/******************************************************************************
 *
 * pr04LongMoveToHostvar: This function moves longdata from the datapart given
 * by the kernel to the hostvariable indexed by ind.
 *
 ******************************************************************************/

bool pr04LongMoveToHostvar (sqlcatype *sqlca, sqlxatype *sqlxa, 
			    int ind, tsp1_part_ptr data_ptr, int lvcoffset)
{
#ifndef SQLODBC
  bool asciitrans = false;
  short hcharlen;
  tpr05_StringEncoding bufenc;
#endif
  short charlen;
  bool unitrans = false;
  tsp78ConversionResult rc;
  tsp00_Uint4   cbWrite;    /* number of bytes written */
  tsp00_Uint4   cbParsed;   /* number of bytes parsed */
  struct SQLSPLD *ldspact = &sqlca->sqlrap->rasqlldp->ldsp[ind];
  struct SQLPCLD *ldpcact = &sqlca->sqlrap->rasqlldp->ldpc[ldspact->ld.o.ld_valind-1];

  int hostlen = ldpcact->ldhostlen;
  int hostlensum = ldpcact->ldhostlensum; /* Amount of longdata already in hostvar. */
  char *hostaddr = (char *) ldpcact->ldhostaddr;
  int vallen = ldspact->ld.o.ld_vallen;  /* Length of longdata in datapart. */

  char *sp1p_buf_F = (char *) data_ptr->sp1p_buf; 
  hostaddr += hostlensum;  /* Point to the next free position in hostvariable. */
  sp1p_buf_F += ldspact->ld.o.ld_valpos - 1;  /* C-Style indexing */

#ifndef SQLODBC
  bufenc = pr04LongGetBufEncoding (ldpcact->ldsfdtype);
  hcharlen = 1;
#endif

#ifndef SQLODBC
  if ((p04isunidata (ldpcact->ldsfdtype) && pr04LongIsAsciiHostvar (ldpcact)) ||
      (((bufenc == sp77encodingUCS2) || (bufenc == sp77encodingUCS2Swapped)) &&
       (!p04isunidata (ldpcact->ldsfdtype) && pr04LongIsAsciiHostvar (ldpcact)))) 
#else
  if (p04isunidata (ldpcact->ldsfdtype) && pr04LongIsAsciiHostvar (ldpcact))
#endif
  {
    unitrans = true;
    charlen = 2;
  } else {
    unitrans = false;
    charlen = 1;
  }

#ifndef SQLODBC
  if ((bufenc == sp77encodingAscii) && 
      pr04LongIsUnicodeHostvar (ldpcact) &&
      p04isasciidata (ldpcact->ldsfdtype)) {
    asciitrans = true;
    hcharlen = 2;
  }
#endif

  if (hostlensum == 0) {
    /* First time data is moved to this hostvariable.
       The first byte(s) of hostvariable are length information. Data starts
       after these length bytes, so increase by lvcoffset. */
    hostaddr += lvcoffset;
    ldpcact->ldhostlensum += lvcoffset;
    hostlensum += lvcoffset;
  }
  
#ifndef SQLODBC
  if ((hostlen - hostlensum) / hcharlen >= (vallen / charlen)) 
#else
  if ((hostlen - hostlensum) >= (vallen / charlen))
#endif
  {
    /* Data from datapart fits completly into hostvariable. Copy all the
       data into hostvariable. */
    if ((ldpcact->ldhosttype == cpr_vfile) ||
	(ldpcact->ldhosttype == cpr_vfilec)) {
      p03datafwrite (sqlca->sqlrap, ldpcact->ldfileno, vallen,
		     sp1p_buf_F, 1,  sqlca->sqlemp);
    } else {
      if (unitrans) {
	rc = sp78convertString (sp77encodingAscii,
				hostaddr,
				hostlen,
				&cbWrite,
				FALSE,
				sp77encodingUCS2Native,
				sp1p_buf_F,
				vallen,
				&cbParsed);
	if (rc != sp78_Ok) {
	  p08runtimeerror (sqlca, sqlxa,
			   cpr_conv_from_unicode_impossible);
	}
#ifndef SQLODBC
      } else if (asciitrans) {
	rc = sp78convertString (sp77encodingUCS2Native,
				hostaddr,
				hostlen,
				&cbWrite,
				FALSE,
				sp77encodingAscii,
				sp1p_buf_F,
				vallen,
				&cbParsed);
	if (rc != sp78_Ok) {
	  p08runtimeerror (sqlca, sqlxa,
			   cpr_conv_from_unicode_impossible);
	}
#endif
      } else {
	memcpy (hostaddr, sp1p_buf_F, vallen);
      }
    }
    ldpcact->ldhostlensum += vallen / charlen;
    ldpcact->ldsfbufpos += vallen;
  } else {
    /* Data from datapart doesn't fit into hostvariable. Move
       one portion into hostvariable, store remaining portion
       in restbuffer. */
    if ((ldpcact->ldhosttype == cpr_vfile) ||
	(ldpcact->ldhosttype == cpr_vfilec)) {
      p03datafwrite (sqlca->sqlrap, ldpcact->ldfileno, hostlen - hostlensum,
		     sp1p_buf_F, 1,  sqlca->sqlemp);
    } else {
      if (unitrans) {
	rc = sp78convertString (sp77encodingAscii,
				hostaddr,
				hostlen,
				&cbWrite,
				FALSE,
				sp77encodingUCS2Native,
				sp1p_buf_F,
				PR07MIN((hostlen - hostlensum) * charlen, vallen),
				&cbParsed);
	if (rc != sp78_Ok) {
	  p08runtimeerror (sqlca, sqlxa,
			   cpr_conv_from_unicode_impossible);
	}
#ifndef SQLODBC
      } else if (asciitrans) {
	rc = sp78convertString (sp77encodingUCS2Native,
				hostaddr,
				hostlen,
				&cbWrite,
				FALSE,
				sp77encodingAscii,
				sp1p_buf_F,
				vallen/hcharlen,
				&cbParsed);
	if (rc != sp78_Ok) {
	  p08runtimeerror (sqlca, sqlxa,
			   cpr_conv_from_unicode_impossible);
	}
#endif
      } else {
	memcpy (hostaddr, sp1p_buf_F, hostlen - hostlensum);
      }
    }
    ldpcact->ldhostlensum = hostlen;
#ifndef SQLODBC
    ldpcact->ldsfbufpos += (hostlen - lvcoffset) * charlen; 
#else
    ldpcact->ldsfbufpos += hostlen - lvcoffset; 
#endif
    if (ldpcact->ldrestdata->rbbufpart == 0) {
      /* Allocate memory for restbuffer and copy longdata from
         datapart into it. */
      if ((vallen - charlen * (hostlen - hostlensum)) != 0) {
	if ((ldpcact->ldrestdata->rbbufpart = 
	     (char *) pr03mAllocat (vallen - charlen * (hostlen - hostlensum), 
				    "ldpcact->ldrestdata->rbbufpart")) == 0) {
	  p08runtimeerror (sqlca, sqlxa, cpr_memory_allocation_faild);
	  return false;
	}
      }
      ldpcact->ldrestdata->rbbufsize = vallen - charlen * (hostlen - hostlensum);
      memcpy (ldpcact->ldrestdata->rbbufpart, 
	      sp1p_buf_F + charlen * (hostlen - hostlensum),
	      vallen - charlen * (hostlen - hostlensum));
      ldpcact->ldrestdata->rbrestlen = 0; /* No data has been read from restbuffer. */
    }     
  }
  return true;
}


/******************************************************************************
 *
 * pr04Long_lvc01: This function fills the hostvariable indexed via ind in the
 * ldpc and ldsp arrays with longdata and determines if the kernel has to be
 * asked for more longdata to fill the hostvariable. In that case, the flag
 * getnext is set to true.
 *
 ******************************************************************************/

bool pr04Long_lvc01 (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, 
		     int ind, bool *getnext, int lvcoffset)
{
  tsp1_part_ptr data_ptr;

  /* Get actual ldsp and ldpc descriptors for the actual long descriptor. */
  struct SQLSPLD *ldspact = &sqlca->sqlrap->rasqlldp->ldsp[ind];
  struct SQLPCLD *ldpcact = &sqlca->sqlrap->rasqlldp->ldpc[ldspact->ld.o.ld_valind-1];

  if (ldpcact->ldhostlen != 0) {
    if (ldpcact->ldhostlensum  == ldpcact->ldhostlen) {
      /* Hostvariable is already filled completely. */
      *getnext = false;
      return true;
    }
  }

  p03find_part (sqlca->sqlrap, sp1pk_data, &data_ptr);
  if (data_ptr == 0)
    p03find_part (sqlca->sqlrap, sp1pk_longdata, &data_ptr);
  if (data_ptr == 0)
    return false;

  /* Depending on ld_valmode, which is sent back from the kernel,
     fill hostvariable. */
  switch (ldspact->ld.o.ld_valmode) {
  case vm_alldata:
  case vm_lastdata:
    /* All data from the longcolumn has been read. Move data to
       hostvariable, don't ask kernel for more data, close long descriptor. */
    if (pr04LongMoveToHostvar (sqlca, sqlxa, ind, data_ptr, lvcoffset) == false)
      return false;
    *getnext = false;
    ldspact->ld.o.ld_valmode = vm_close;
    break;
  case vm_data_trunc:
    /* The kernel has been asked (during a previous call of this function)
       for longdata of certain length. Move this portion to hostvariable.
       No more data from the long column is needed. */
    if (pr04LongMoveToHostvar (sqlca, sqlxa, ind, data_ptr, lvcoffset) == false)
      return false;
    *getnext = false;    
    break;
  case vm_datapart:
    /* The kernel has delivered only one portion of the long column.
       Move the portion to the hostvariable and eventually prepare long 
       descriptor to send to kernel again to ask for more data. */
    if (pr04LongMoveToHostvar (sqlca, sqlxa, ind, data_ptr, lvcoffset) == false)
      return false;
    if (ldpcact->ldhostlensum < ldpcact->ldhostlen) {
      /* The hostvariable has not been filled completly, so ask kernel
         for more longdata. */

      /* ld_vallen is the amount of data that is wanted from the kernel. */
      ldspact->ld.o.ld_vallen = ldpcact->ldhostlen - ldpcact->ldhostlensum;
      if (p04isunidata (ldpcact->ldsfdtype) && pr04LongIsAsciiHostvar (ldpcact)) {
	ldspact->ld.o.ld_vallen *= 2;
      }
      /* To prevent kernel to close the long column, set ld_infoset to
         ld_no_close. */
      ldspact->ld.o.ld_infoset |= (1 << ld_no_close);
      *getnext = true;
    } else {
      /* The hostvariable is completely filled with longdata. */
      *getnext = false;
    }
    break;
  case vm_nodata:
    /* The kernel has not delivered any data for the long descriptor yet.
       Ask for more data. */

    /* ld_vallen is the amount of data that is wanted from the kernel. */
    ldspact->ld.o.ld_vallen = ldpcact->ldhostlen - ldpcact->ldhostlensum;
    if (p04isunidata (ldpcact->ldsfdtype) && pr04LongIsAsciiHostvar (ldpcact)) {
      ldspact->ld.o.ld_vallen *= 2;
    }
    /* To prevent kernel to close the long column, set ld_infoset to
       ld_no_close. */
    ldspact->ld.o.ld_infoset |= (1 << ld_no_close);
    *getnext = true;
    break;
  case vm_close:
    /* Should never happen, but permits endless loop in case of error. */
    *getnext = false;
    break;
  default:
    /* Should never happen, but permits endless loop in case of error. */
    *getnext = false;
    break;
  }
  return true;
}

/******************************************************************************
 *
 * pr04LongInitRestDataBuf: This function initializes the data structure for
 * the restbuffer. This buffer stores data from the datapart sent from the 
 * kernel that does not fit into the hostvariable. It's contents is read by the
 * following calls of EXEC SQL GETVAL.
 *
 * TODO: Simplify data structure for restbuffer (only rbrestlen, rbbufsize
 *       and rbbufpart are needed.
 *
 ******************************************************************************/

bool pr04LongInitRestDataBuf (sqlcatype *sqlca, sqlxatype *sqlxa, struct SQLPCLD *ldpc)
{
  ldpc->ldrestdata = (struct SQLRESTBUF *) 
    pr03mAllocat (sizeof (*ldpc->ldrestdata), "ldpcact->ldrestdata");
  if (!ldpc->ldrestdata) {
    p08runtimeerror (sqlca, sqlxa, cpr_memory_allocation_faild);
    return false;
  }

  ldpc->ldrestdata->rbcolno = 0;
  ldpc->ldrestdata->rbfiller1 = 0;
  ldpc->ldrestdata->rbfiller2 = 0;
  ldpc->ldrestdata->rbrestlen = 0;
  ldpc->ldrestdata->rbhostlensum = 0;
  ldpc->ldrestdata->rblongcolmax = 0;
  ldpc->ldrestdata->rbbufsize = 0;
  ldpc->ldrestdata->rbbufpart = 0;

  return true;
}


/******************************************************************************
 *
 * pr04LongOutputLvc: This function is the main entry point for piecewise
 * reading of long column during an EXEC SQL FETCH command.
 * The hostvariables, which memory addresses are given in the ldpc array of
 * the ld descriptor, are filled with the first portion of longdata read from
 * the long columns. This function may be called several times until the
 * complete result set of the preceding select command is handled.
 *
 ******************************************************************************/

bool pr04LongOutputLvc (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, 
			struct tpr_sqlloop looprec)
{
  bool getnext, trunc;
  int i, rowno, lvcoffset;
  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp;

  int ibeg = (*ld)->ldindpc;  /* Start index for next long descriptors, if function
                                 is to be called several times for one result set. */

  (*ld)->lderrd3 = sqlca->sqlerrd[2];

  trunc = false;  /* No hostvariable has been truncated so far. */

  for (i = ibeg; i < (*ld)->ldmaxo; i++) {
    struct SQLSPLD *ldspact = &(*ld)->ldsp[i];
    struct SQLPCLD *ldpcact = &(*ld)->ldpc[ldspact->ld.o.ld_valind-1];

    ldpcact->ldhostlensum = 0; /* Actual data length in hostvariable. */
    ldpcact->ldsfbufpos = 0;  /* Actual read position in datapart buffer. */
  }
		
  rowno = -1;

  /* Loop over all long descriptors from the current portion of result set. */
  for (i = ibeg; i < (*ld)->ldmaxo; i++) {
    struct SQLSPLD *ldspact = &(*ld)->ldsp[i];
    struct SQLPCLD *ldpcact = &(*ld)->ldpc[ldspact->ld.o.ld_valind-1];
    (*ld)->ldindpc = i+1;
    pr04LongGetHostInfoOff (sqlca, sqlxa, &lvcoffset);
    if (pr04LongInitRestDataBuf (sqlca, sqlxa, ldpcact) == false)
      return false;

    ldpcact->ldsfbufpos = 0; /* Bytes read from the data part buffer. */
    getnext = true;

    while (getnext) {
      /* Loop until hostvariable is completly filled with longdata
         or no more longdata available. */
      if (pr04Long_lvc01 (sqlca, sqlxa, ga, i, &getnext, lvcoffset) == false)
        return false;
      if (getnext) {
	/* Ask kernel for more longdata. */
	if (pr04LongPutDesc (sqlca, sqlxa, ga, sp1m_getval, false) == false)
	  return false;
	if (pr04LongGetDesc (sqlca, sqlxa, ga) == false)
          return false;
      }
    }

    if (((ldpcact->ldhosttype == cpr_vfile) ||
	 (ldpcact->ldhosttype == cpr_vfilec)) &&
	(sqlca->sqlemp->ereturncode == 0)) {
      p03datafclose (sqlca->sqlrap, &ldpcact->ldfileno, sqlca->sqlemp);
    }

    pr04LongSetHostvarLength (sqlca, sqlxa, ldpcact, lvcoffset);

    pr04LongGetColLength (sqlca, sqlxa, i);

    pr04LongWriteTracePart (sqlca, sqlxa, ldpcact, lvcoffset, &rowno, cpr_tr_outp);

    if ((ldspact->ld.o.ld_valmode != vm_close) ||
	((ldspact->ld.o.ld_valmode == vm_close) && 
	 (ldpcact->ldrestdata->rbbufsize != 0))) {
      /* There is longdata that doesen't fit into hostvariable. 
         Data was truncated. */
      if (sqlca->sqlgap->gacmpkind != CPR_LA_ODBC)
	pr04LongTraceTrunc (sqlca, ldpcact->ldhostlensum - lvcoffset);
      trunc = true;
    }
  }

  if (trunc) {
    /* Set the apropriate flag of sqlca to 'W', indicating that at least
       one hostvariable has been truncated. */
    sqlca->sqlwarn[0] = 'W';
    sqlca->sqlwarn[1] = 'W';
  }

  sqlca->sqlerrd[2] = (*ld)->lderrd3;

  return true;
}

boolean pr04LongPOdbcData (sqlcatype *sqlca, sqlxatype *sqlxa,
			   sqlgaentry *ga, tpr_longvarcharrec *lvcrec,
			   tsp00_Int2 *spindex, bool *first)
{
  int i, iend;
  tsp00_Lname vaname;
  tsp00_Int2 putvalinit;
  tsp00_Int4 retsqlerrd3;
  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp;
  struct SQLPCLD *ldpcact = &(*ld)->ldpc[(*ld)->ldindpc-1];

  p03ccmdinit (sqlxa->xaSQLDesc, sqlca, ga, sp1m_putval);
  (*ld)->ldputvalsend = true;
  putvalinit = cpr_is_first;
  *first = true;
  iend = *spindex;
  if ((ldpcact->ldhostlen == 0) && (iend < (*ld)->ldmaxi)) {
    iend++;
  }
  for (i = *spindex; i <= iend; i++) {
    if ((sqlca->sqlcode == 0) || (sqlca->sqlcode == 100)) {
      struct SQLSPLD *ldspact = &(*ld)->ldsp[i-1];
      if (ldspact->ld.o.ld_valmode != vm_lastdata) {
	(*ld)->ldindsp = i;
	(*ld)->ldindpc = ldspact->ld.o.ld_valind;
	ldpcact = &(*ld)->ldpc[(*ld)->ldindpc-1];
	memcpy (vaname, "PARAMETER                       ", sizeof (tsp00_Lname));
	pr04LongTraceInit (sqlca, cpr_tr_longvar, &vaname, ldpcact->ldcolno);
	if (ldpcact->ldhostlen != 0) {
	  pr04LongGetHostInfo (sqlca, sqlxa, lvcrec, true);
	  ldpcact->ldhostlen = lvcrec->lvchostlen;
	  ldpcact->ldhostlensum += ldpcact->ldhostlen;
	}
        if (pr04LongInsertLongVarchar (sqlca, sqlxa, ga, lvcrec, &putvalinit,
                                       first, true) == false)
          return false;
	(*ld)->ldallsend = true;
	(*ld)->ldretmaxi = (*ld)->ldmaxi;
	(*ld)->ldretmaxo = (*ld)->ldmaxo;
	(*ld)->ldretindpc = (*ld)->ldindpc;
	(*ld)->ldretindsp = (*ld)->ldindsp;
	if (*first) {
	  retsqlerrd3 = sqlca->sqlerrd[2];
	  *first = false;
	}
      }
    }
  }
  return true;
}

boolean pr04LongGOdbcData (sqlcatype *sqlca, sqlxatype *sqlxa,
			sqlgaentry *ga, tpr_longvarcharrec *lvcrec,
			tsp00_Int2 *spindex)
{
  boolean getnext;
  int index;
  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp;
  struct SQLPCLD *ldpcact = &(*ld)->ldpc[(*ld)->ldindpc-1];
  struct SQLSPLD *ldspact = &(*ld)->ldsp[(*ld)->ldindsp-1];
  tsp00_Int4 *rbrestlen = &sqlca->sqlrap->rasqlldp->ldrestbufp->rbrestlen;
  void *rbbufpart = sqlca->sqlrap->rasqlldp->ldrestbufp->rbbufpart;

  pr04LongGetHostInfo (sqlca, sqlxa, lvcrec, false);
  ldpcact->ldhostlen = lvcrec->lvchostlen;
  lvcrec->lvcvalmode = ldspact->ld.o.ld_valmode;
  lvcrec->lvcvalpos = ldspact->ld.o.ld_valpos;
  lvcrec->lvcvallen = ldspact->ld.o.ld_vallen;
  if (*rbrestlen >= ldpcact->ldhostlen) {
    char *hostaddr = ldpcact->ldhostaddr;
    hostaddr += lvcrec->lvchostpos + lvcrec->lvcaoffset;
    memcpy (hostaddr, (char *) rbbufpart, 
	    ldpcact->ldhostlen);
    lvcrec->lvchostpos += ldpcact->ldhostlen;
    *rbrestlen = - (*rbrestlen - ldpcact->ldhostlen);
    memcpy (rbbufpart, (char *) rbbufpart + ldpcact->ldhostlen, -(*rbrestlen));
  }
  if (*rbrestlen > 0) {
    char *hostaddr = ldpcact->ldhostaddr;
    hostaddr += lvcrec->lvchostpos + lvcrec->lvcaoffset;
    memcpy (hostaddr, (char *) rbbufpart, *rbrestlen);
  }
  index = *spindex - 1;
  if (pr04Long_lvc01 (sqlca, sqlxa, ga, index, &getnext, lvcrec->lvcaoffset) == false)
    return false;  

  return true;
}

boolean pr04LongOdbcColIsTrunc (sqlcatype *sqlca, int colno) {
  struct SQLLD **ld = &sqlca->sqlrap->rasqlldp;
  struct SQLPCLD *ldpcact = &(*ld)->ldpc[(*ld)->ldindpc-1];
  struct SQLSPLD *ldspact = &(*ld)->ldsp[(*ld)->ldindpc-1];  

  if ((ldspact->ld.o.ld_valmode != vm_close) ||
      ((ldspact->ld.o.ld_valmode == vm_close) && 
       ((ldpcact->ldrestdata->rbbufsize - ldpcact->ldrestdata->rbrestlen) >= 0))) {
    /* There is longdata that doesen't fit into hostvariable. 
       Data was truncated. */
    return true;
  } else {
    return false;
  }
}

/******************************************************************************
 *
 * pr04LongGetTotalLength: Devlivers total length of long column stored in
 *                         the database.
 *
 *   sqlca: precompiler structure
 *   sqlxa: precompiler structure
 *   ind:   index of long column starting at 0
 *   len:   total length of long column with index ind
 *   
 *   Returns false, if index of long column is invalid, true otherwise.
 *
 ******************************************************************************/
boolean pr04LongGetTotalLength (sqlcatype *sqlca, int ind, int *len)
{
  struct SQLLD **ld;

  if (ind < 0)
    return false;

  ld = &sqlca->sqlrap->rasqlldp;
  *len = (*ld)->ldsp[ind].ld.o.ld_maxlen;

  return true;
}
