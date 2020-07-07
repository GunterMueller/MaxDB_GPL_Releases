/*!**************************************************************************

  module:    vpr03c.c

  -------------------------------------------------------------------------

  responsible: BurkhardD

  special area:   CPCDrv | Client | Orderinterface
  description :   SAPDB-Order-Interface

  last changed: 2000-09-20  9:05

  see also    :

  change history:

    2001-09-04 d025086 No implicite connect. 
    2001-05-17 D025086 Use pr03PacketReqRec instead of p03reqrecpacket to 
                       protect unexpected communication errors.

  -------------------------------------------------------------------------


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




*****************************************************************************/

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <time.h>
#include <ctype.h>
#if defined (WIN32)
#else
#include <unistd.h>
#endif
#include "gsp05.h"
#include "vsp008.h"
#include "hsp26.h"
#include "gpr03.h"
#include "gpr08.h"
#include "vpr07c.h"
#include "vpr01Env.h"
#include "vpr01SQL.h"
#include "vpr03Packet.h"
#include "vpr03Part.h"
#include "vpr01Trace.h"
#include "vpr05IfCom_String.h"
#include "gmt90.h"
#include "hsp78_0.h"
#include "vpr01Module.h"
#include "vpr01Precom.h"
#include "vpr01Con.h"
#include "vpr03Con.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

#define P03_ISO8859  "ISO8859_1"
#define   USER_PASSW_WRONG         -4008
#define   DATABASE_NOT_ACCESSIBLE  -8000
#define   USER_NOT_CONNECTED       -8001
#define   USER_ALREADY_CONNECTED   -8002

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

extern void     p07_copyright (void);

extern tsp00_Int2 s30lnr (tsp00_C18, char val,
    int pos, int cnt);
extern tsp00_Int2 s30lnr1 (tsp00_C70, char val,
    int pos, int cnt);

/********  vsp40  proceduren  **********/
extern    s40g4int (const char *, int, tsp00_Int4 *, char *);

#ifdef SQLODBC
#include "heo07.h"

/********  ODBC async proceduren vpa09ac, cmp vpa09.h  **********/
#define PA09_WAIT_FOR_SEMAPHORE    1
#define PA09_RELEASE_SEMAPHORE     2
#define PA09_SEMAPHORE_SQLCANCEL   10
#define PA09_SEMAPHORE_WRITETRACE  11

#define PA09_ADD                   100
#define PA09_SUB                   101
#define PA09_CONNECT_BEGIN         102
#define PA09_WAIT_FOR_BEGIN        103
#define PA09_CONNECT_END           104

extern tsp00_Int4 pa09IsCanceled(tsp00_Int4 reference);
extern void pa09Semaphore (int sema, int action);
extern int pa09MTHandler (int action);   /* PTS 1120833 */

/********  COMUNICATION WITH ORDER INTERFACE vpa01c  **********/
extern void pa01TraceSession(tsp00_Int2 comtype);
extern void pa01GetService(tsp00_Service *service);
extern void pa01GetMesscode(char *m_type);
extern tsp00_Int2 pa01UtilityConnect(void);
#endif

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

void
p03getenvdblang (tsp00_Int2 * dblang)
{
	char * dblangp;
	static	char    envname [] = "DBLANG" ;

	M90TRACE (M90_TR_ENTRY, "p03getenvdblang", 0);
	*dblang  = CPR_SINGLE_BYTE;
	M90TRACE (M90_TR_SWORD, "dblang", dblang);
	dblangp = ( char * ) getenv ( envname ) ;
	if ( dblangp != NULL )
	{
		int   len ;
                char *dbl_string ;

		len = (int)strlen ( dblangp ) ;
		dbl_string = (char *)pr03mAllocat((len+1) * sizeof(char), "dbl_string");
		if (!dbl_string)
			sqlabort();

		if ( dbl_string != NULL )
                  strcpy ( dbl_string, dblangp ) ;
                else
                  dbl_string = dblangp ;
		M90TRACE (M90_TR_SWORD, "len  ", &len);
		M90TRACEBUF ("dbl_string  ", dbl_string, 0, len);
                if (memcmp (dbl_string, "ja_jpn.sjis", 11) == 0 )
			*dblang = CPR_MULTI_SJIS;
                if (memcmp (dbl_string, "JA_JPN.SJIS", 11) == 0 )
			*dblang = CPR_MULTI_SJIS;
                if (memcmp (dbl_string, "ja_jpn.euc", 10) == 0 )
			*dblang = CPR_MULTI_EUC;
                if (memcmp (dbl_string, "JA_JPN.EUC", 10) == 0 )
			*dblang = CPR_MULTI_EUC;
		M90TRACE (M90_TR_SWORD, "dblang", dblang);
        }
	M90TRACE (M90_TR_EXIT, "p03getenvdblang", 0);
}

void p03sqlfree (sqlcatype *sqlca)
{
	sqlmfentry *mf = sqlca->sqlmfp->mfentry;
	sqlmfentry *mfe = sqlca->sqlmfp->mfentry + sqlca->sqlmfetch;
	sqlmdesc *mfd = &sqlca->sqlmfp->mfdesc;
	struct SQLLD   **ld = &sqlca->sqlrap->rasqlldp;
	void **stmtp = &sqlca->sqlrap->raprofstmtp;

	if (*stmtp)
	{
		pr03mFree (*stmtp, "stmtp");
		*stmtp = NULL;
	}
	for (;mf < mfe; mf++)
		if (mf->mfBufinit == cpr_is_true)
		{
			pr03mFree (mf->mfrecpointer, "mfrecpointer");
			pr03mFree (mf->mfBufpointer, "mfBufpointer");
			mf->mfrecpointer = NULL;
			mf->mfBufpointer = NULL;
			mf->mfBufinit = cpr_is_false;
		}
	sqlca->sqlmfetch = 0;
	if (mfd) {
	  mfd->descMaxelem = mfd->descNelem = 0;
	  if (*mfd->descElemPtr) {
	    pr03mFree (*mfd->descElemPtr, "descElemPtr");
	    *mfd->descElemPtr = NULL;
	  }
	}
	if (*ld)
	  p03freeld(ld);

	if (sqlca->sqlxadesc.descElemPtr)
	{
		sqluns4 nmod = sqlca->sqlxadesc.descNelem / 2;
		sqlint2 **sqlstci = (sqlint2**) *sqlca->sqlxadesc.descElemPtr;
		sqlint2 **maxstci = sqlstci + sqlca->sqlxadesc.descNelem;
		sqlxatype **sqlxap = (sqlxatype**)
			*sqlca->sqlxadesc.descElemPtr + 1;
		sqlxatype **maxxap = sqlxap + sqlca->sqlxadesc.descNelem - 1;

		for (;sqlxap < maxxap; sqlxap += 2)
		{
		  sqlxatype *sqlxa = *sqlxap;
		  sqlkaentry *ka = sqlxa->sqlkap;
		  sqlparentry *pa = sqlxa->sqlpap;
		  sqlorentry *or = sqlxa->sqlorp;
		  sqlorentry *ornext;
		  sqlsnentry *sn = sqlxa->sqlsnp.snentptr;
		  sqlsnentry *sne = sqlxa->sqlsnp.snentptr + sqlxa->sqlsndesc.descNelem;

		  for (;ka < sqlxa->sqlkae; ka++)
		    ka->kastate = CPR_STATE_EMPTY;
		  for (;or < sqlxa->sqlore; or++)
		    if (or->orcolptr) {
		      for (ornext = or + 1; ornext < sqlxa->sqlore; ornext++) {
			if (or->orcolptr == ornext->orcolptr) {
			  ornext->orcolptr = NULL;
			}
		      }
		      pr03mFree ((tsp00_Uint1 *)or->orcolptr, "orcolptr");
		      or->orcolptr = NULL;
		    }
		  if (sqlxa->sqlsnp.snentcnt > -1) {
		    for (;sn < sne; sn++)
		      if (sn->snstmtp) {
			pr03mFree ((tsp00_Uint1 *)sn->snstmtp, "snstmtp");
			sn->snstmtp = NULL;
		      }
		    if (sqlxa->sqlsnp.snentptr) {
		      pr03mFree ((tsp00_Uint1 *)sqlxa->sqlsnp.snentptr, "snentptr");
		      sqlxa->sqlsnp.snentptr = NULL;
		    }
		    sqlxa->sqlsnp.snentcnt = -1;
		  }
		}
		for (;sqlstci < maxstci; sqlstci += 2)
			**sqlstci = -1;
		pr03mFree ((void*) *sqlca->sqlxadesc.descElemPtr, "*descElemPtr");
		pr03mFree ((void*) sqlca->sqlxadesc.descElemPtr, "descElemPtr");
		sqlca->sqlxadesc.descNelem = 0;
		sqlca->sqlxadesc.descMaxelem = 0;
		sqlca->sqlxadesc.descElemSize = sizeof (void*);
		sqlca->sqlxadesc.descIncr = 32;
		sqlca->sqlxadesc.descElemPtr = NULL;
	}
#ifdef SQLODBC
	p03EnvDelete( sqlca );
#endif
}     /*--------------------------------------------*/

void p03freeld(	struct SQLLD   **ld)
{
  if (*ld) {
    if ((*ld)->ldrestbufp) {
      if ((*ld)->ldrestbufp->rbbufpart) {
	/* free restbuffer (is allocated in p01lgetdata) PTS 1102602 */
	pr03mFree ((tsp00_Uint1*) (*ld)->ldrestbufp->rbbufpart, "rbbufpart");
      };
      pr03mFree ((tsp00_Uint1 *)(*ld)->ldrestbufp, "ldrestbufp");
    }
    if ((*ld)->ldpc)
      pr03mFree ((tsp00_Uint1 *)(*ld)->ldpc, "ldpc");
    if ((*ld)->ldsp)
      pr03mFree ((tsp00_Uint1 *)(*ld)->ldsp, "ldsp");
    pr03mFree ((tsp00_Uint1 *)*ld, "*ld");
    *ld = NULL;
  }
}


void p03orfree (sqlcatype *sqlca, struct SQLCOL *orptr, tsp00_Int2 kind)
{

	if (sqlca->sqlxadesc.descElemPtr)
	{
		sqlxatype **sqlxap = (sqlxatype**)
			*sqlca->sqlxadesc.descElemPtr + 1;
		sqlxatype **maxxap = sqlxap + sqlca->sqlxadesc.descNelem - 1;

		for (;sqlxap < maxxap; sqlxap += 2)
		{
			sqlxatype *sqlxa = *sqlxap;
			sqlorentry *or = sqlxa->sqlorp;
				for (;or < sqlxa->sqlore; or++)
				{
				  switch (kind)
				    {
				    case 1 :
				      if (or->orcolptr)
					if (or->orcolptr == orptr)
					  {
					    or->orcolptr = NULL;
					  }
				          break;
				    case 2 :
				      if (or->orresptr)
					if (or->orresptr == orptr)
					  {
					    or->orresptr = NULL;
					  }
				          break;
				    case 3 :
				      if (or->orresnamptr)
					if (or->orresnamptr == orptr)
					  {
					    or->orresnamptr = NULL;
					  }
				          break;
				    default:
				         break;

				       }   /*switch*/
			        }
		}
	}
}     /*--------------------------------------------*/
void
p03pointerinit (sqlcatype * sqlca,
    unsigned char *ok)
{
/***    tsp1_packet    *connp;*****/
    long            i;
    long            s;
    *ok = true;

    M90TRACE (M90_TR_ENTRY, "p03pointerinit", 0);
    s = sizeof (sqlcatype);
    M90TRACE (M90_TR_SWORD, "sizeof (sqlca)", &s);
    sqlca->sqlgap = pr03mAllocat(sizeof(sqlgatype), "sqlca->sqlgap");
    if (!sqlca->sqlgap)
	sqlabort();

    for (i = 0; i < sqlgamax; i++) {
      sqlca->sqlgap->gaentry[i] = pr03mAllocat(sizeof(sqlgaentry), "sqlgap->gaentry");
      if (!sqlca->sqlgap->gaentry[i])
	sqlabort();
    }

    sqlca->sqlrap = pr03mAllocat(sizeof(sqlratype), "sqlca->sqlrap");
    if (sqlca->sqlrap) {
      sqlca->sqlrap->rasqlldp = NULL; /* allocated by p01elvcallocat */
       sqlca->sqlrap->rasqltap = pr03mAllocat(sizeof(sqltatype), "sqlrap->rasqltap");
    }
    if (!sqlca->sqlrap->rasqltap)
      sqlabort();

    sqlca->sqloap = pr03mAllocat(sizeof(sqloatype), "sqlca->sqloap");
    if (!sqlca->sqloap)
	sqlabort();

    sqlca->sqlmap = pr03mAllocat(sizeof(sqlmatype), "sqlca->sqlmap");
    if (!sqlca->sqlmap)
      sqlabort();

    sqlca->sqlmfp = pr03mAllocat(sizeof(sqlmftype), "sqlca->sqlmfp");
    if (!sqlca->sqlmfp)
      sqlabort();

    if (sqlca->sqlmfp)
      /* Initialisierung f. mfentry */
      memset(sqlca->sqlmfp, 0, sizeof(sqlmftype));

    sqlca->sqlemp = pr03mAllocat(sizeof(struct SQLERROR), "sqlca->sqlemp");
    if (!sqlca->sqlemp)
	sqlabort();

    sqlca->sqlcxap = pr03mAllocat(sizeof(sqlcxatype), "sqlca->sqlcxap");
    if (!sqlca->sqlcxap)
      sqlabort();

    sqlca->sqlplp = NULL;

    sqlca->sqlxadesc.descNelem = 0;
    sqlca->sqlxadesc.descMaxelem = 0;
    sqlca->sqlxadesc.descElemSize = sizeof (void*);
    sqlca->sqlxadesc.descIncr = 32;
    sqlca->sqlxadesc.descElemPtr = NULL;
    M90TRACE (M90_TR_EXIT, "p03pointerinit", 0);
}     /*--------------------------------------------*/

void p03raprofinit (sqlcatype * sqlca, tsp00_Int4 proflen)
{
  if  (sqlca->sqlrap->raopprof != cpr_is_false &&
       sqlca->sqlrap->raprofstmtp == NULL) {
    sqlca->sqlrap->raprofstmtp = pr03mAllocat(proflen, "sqlrap->raprofstmtp");
    if (!sqlca->sqlrap->raprofstmtp)
      sqlabort();
  }
}

void pr03cOrInit(sqlorentry *ore)
{
  if (ore)
    memset(ore, 0, sizeof(sqlorentry));
}

void pr03cCuInit(sqlcuentry *cue)
{
  if (cue)
    memset(cue, 0, sizeof(sqlcuentry));
}

void           *
p03dynalloc (sqlmdesc * md)
{
  unsigned long   nchar = md->descNelem * md->descElemSize;

  if (++md->descNelem > md->descMaxelem) {
    void           *newElem;

    md->descMaxelem += md->descIncr;
    newElem = pr03mAllocat(md->descMaxelem * md->descElemSize, "descElemPtr");
    if (!newElem)
      sqlabort();

    if (nchar > 0) {
      memcpy (newElem, *md->descElemPtr, nchar);
      pr03mFree(*md->descElemPtr, "descElemPtr");
    }
    *md->descElemPtr = newElem;
  }
  return (char *) *md->descElemPtr + nchar;
}     /*--------------------------------------------*/


/*** sn-area statementname ******/
#define p03mxpr_sqlsn 100

void p03sninit   (sqlxatype * sqlxa)
{
    sqlmdesc    *sndescp = &sqlxa->sqlsndesc;
    sqlsn       *snp     = &sqlxa->sqlsnp;

    snp->snentcnt = 0;
    snp->snentact = 0;
    snp->snentptr = NULL;
    sndescp->descMaxelem = 0;
    sndescp->descNelem   = 0;
    sndescp->descElemSize = sizeof (sqlsnentry);
    sndescp->descIncr    = p03mxpr_sqlsn;
    sndescp->descElemPtr = (void**) &snp->snentptr;
}

void
p03sqlsnallocate (sqlxatype * sqlxa, tsp00_Int4 * index)
{
    sqlmdesc    *sndescp = &sqlxa->sqlsndesc;
    sqlsnentry   *snent;
    short size;
    size = sizeof (sqlsnentry);
    M90TRACE (M90_TR_SWORD, "sizeof (sqlsnentry)", &size);

    snent = (sqlsnentry *) p03dynalloc (sndescp);
    M90TRACE (M90_TR_SWORD, "sndescp->descNelem", &sndescp->descNelem);
    *index = (tsp00_Int2) sndescp->descNelem;
}     /*--------------------------------------------*/

/********  trace proceduren  **********/

void
p03connectcmdtrace (sqlratype * sqlra, sqlgaentry * gaen)
{
    sqltatype      *tap = sqlra->rasqltap;

    M90TRACE (M90_TR_STRING, "p03connectcmdtrace", "entry");
    if (tap->tatrout != CPR_TRACE_OFF)
        {
        sqlxuserrec    *gaxu = &gaen->gaxuserrec;
        short          *t80l = &sqlra->rasqltap->tastr80l;
        struct SQLERROR errmsg;

        *t80l = 0;
        p03tvfwritetrace (sqlra, &errmsg);
#ifndef SQLODBC
        *t80l = 12;
        memcpy (&tap->tastr80, "SESSION   : ", *t80l);
        *t80l += sprintf(&tap->tastr80[*t80l], "%d;", sqlra->raactsession);
        p03tvfwritetrace (sqlra, &errmsg);
	if (strncmp ((char*)gaen->gaatdbname, bsp_c64,
                     sizeof(gaen->gaatdbname)) != 0) {
          tsp00_Int2  len;
          *t80l = 12;
          memcpy (tap->tastr80, "DATABASE  : ", *t80l);
          len = s30lnr1 (gaen->gaatdbname, bsp_c1, 1,
                         sizeof(gaen->gaatdbname));
          memcpy (&tap->tastr80[*t80l], &gaen->gaatdbname, len);
          *t80l += len;
          p03tvfwritetrace (sqlra, &errmsg);
	}
#else
        pa01TraceSession(cpr_com_sql_conn);
        p03tvfwritetrace (sqlra, &errmsg);
#endif
        if (strncmp ((const char *) gaxu->xu_key, bsp_c18, sizeof(gaxu->xu_key)))
            pr01TracePrintf(sqlra, "USERKEY   : %.*s", sizeof(gaxu->xu_key), gaxu->xu_key);
        *t80l = mxsp_name;
        switch (sqlra->radbmode)
            {
            case CPR_KIND_INTERNAL:
                if (sqlra->rasqlansi == CPR_KIND_ANSI)
                    memcpy (&tap->tastr80, "SQLMODE   : ANSI  ", *t80l);
                else
                    memcpy (&tap->tastr80, "SQLMODE   : INTERNAL", *t80l);
                break;
            case CPR_KIND_DB2:
                memcpy (&tap->tastr80, "SQLMODE   :DB2/SAA", *t80l);
                break;
            case CPR_KIND_ORACLE:
                if (sqlra->rasqlansi == CPR_KIND_ANSI)
                    {
                    *t80l += 2;
                    memcpy (&tap->tastr80, "SQLMODE   : ANSIORA ", *t80l);
                    }
                else
                    memcpy (&tap->tastr80, "SQLMODE   : ORACLE", *t80l);
                break;
            case CPR_KIND_SAPR3:
                memcpy (&tap->tastr80, "SQLMODE   : SAPR3 ", *t80l);
                break;
            default:
                memcpy (&tap->tastr80, "SQLMODE   : EMPTY ", *t80l);
                break;
            }
        p03tvfwritetrace (sqlra, &errmsg);
        *t80l = 12;
        memcpy (&tap->tastr80, "SERVERDB  : ", *t80l);
        memcpy (&tap->tastr80[*t80l], &gaxu->xu_serverdb, sizeof(gaxu->xu_serverdb));
        *t80l += sizeof(gaxu->xu_serverdb);
        p03tvfwritetrace (sqlra, &errmsg);
        *t80l = 12;
        memcpy (&tap->tastr80, "SERVERNODE: ", *t80l);
        if (gaen->gaServerNode != NULL) {
            int copyLen = PR07MIN( (int)strlen(gaen->gaServerNode),
                               sizeof(tap->tastr80) - *t80l );
            memcpy (&tap->tastr80[*t80l], gaen->gaServerNode, copyLen );
            *t80l += copyLen;
        } else {
            memcpy (&tap->tastr80[*t80l], &gaxu->xu_servernode,
                    sizeof(gaxu->xu_servernode));
            *t80l += sizeof(gaxu->xu_servernode);
        };
        p03tvfwritetrace (sqlra, &errmsg);
        if (gaen->gastatus == CPR_CO_USER_OP)
            {
            *t80l = mxsp_name;
            memcpy (&tap->tastr80, "OPTION-CONNECT :  ", *t80l);
            p03tvfwritetrace (sqlra, &errmsg);
            }

        else if (gaen->gastatus == CPR_CO_IMPLICIT)
            {
            *t80l = mxsp_name;
            memcpy (&tap->tastr80, "IMPLICIT-CONNECT :", *t80l);
            p03tvfwritetrace (sqlra, &errmsg);
            }
        p03varparttrace (sqlra, gaen);
        }
}     /*--------------------------------------------*/


void
p03cmdtrace (sqlratype * sqlra, sqlgaentry *gae,
    tsp00_Int2 nlcnt, tsp00_Int2 comkind,
	     tpr05_String *pSQLStatement)
{
  sqltatype      *tap = sqlra->rasqltap;

  M90TRACE (M90_TR_STRING, "p03cmdtrace", "entry");
  if (tap->tatrout != CPR_TRACE_OFF) {
    short          *t80l = &sqlra->rasqltap->tastr80l;
    struct SQLERROR errmsg;
    tsp00_Int2        i;

    *t80l = 0;
    for (i = 0; i < nlcnt; i++)
      p03tvfwritetrace (sqlra, &errmsg);
    if (sqlra->radbmode != sqlra->rakamode) {
      memcpy (&tap->tastr80, "STMMODE   : ", 12);
      *t80l = 12;
      switch (sqlra->rakamode)
	{
	case CPR_KIND_INTERNAL:
	  if (sqlra->rasqlansi == CPR_KIND_ANSI)
	    memcpy (&tap->tastr80[*t80l], "ANSI    ", 8);
	  else
	    memcpy (&tap->tastr80[*t80l], "INTERNAL", 8);
	  break;
	case CPR_KIND_DB2:
	  memcpy (&tap->tastr80[*t80l], "DB2/SAA ", 8);
	  break;
	case CPR_KIND_ORACLE:
	  if (sqlra->rasqlansi == CPR_KIND_ANSI)
	    memcpy (&tap->tastr80[*t80l], "ANSIORA ", 8);
	  else
	    memcpy (&tap->tastr80[*t80l], "ORACLE  ", 8);
	  break;
	case CPR_KIND_SAPR3:
	  memcpy (&tap->tastr80[*t80l], "SAPR3   ", 8);
	  break;
	default:
	  memcpy (&tap->tastr80[*t80l], "EMPTY   ", 8);
	  break;
	}
      *t80l += 8;
      p03tvfwritetrace (sqlra, &errmsg);
    }

#ifndef SQLODBC
    pr01TraceSession(gae, sqlra);
#else
    pa01TraceSession(cpr_com_sql);
#endif
    if (comkind == cpr_com_describe) {
      static char szTmp[]="DESCRIBE :  ";
      memcpy (&tap->tastr80[*t80l], szTmp, sizeof(szTmp)-1);
      *t80l += sizeof(szTmp)-1;
    }
    if (comkind == cpr_com_sql_open) {
      static char szTmp[]="OPEN STATEMENT :  ";
      memcpy (&tap->tastr80[*t80l], szTmp, sizeof(szTmp)-1);
      *t80l += sizeof(szTmp)-1;
      p03tvfwritetrace (sqlra, &errmsg);
    }
    if (comkind == cpr_com_command) {
      static char szTmp[]="SQLCOMMAND :  ";
      memcpy (&tap->tastr80[*t80l], szTmp, sizeof(szTmp)-1);
      *t80l += sizeof(szTmp)-1;
    }
    if (comkind == cpr_com_trace) {
      static char szTmp[]="TRACE LINE :  ";
      memcpy (&tap->tastr80[*t80l], szTmp, sizeof(szTmp)-1);
      *t80l += sizeof(szTmp)-1;
    }
    if (comkind == cpr_com_option) {
      static char szTmp[]="OPTION :  ";
      memcpy (&tap->tastr80[*t80l], szTmp, sizeof(szTmp)-1);
      *t80l += sizeof(szTmp)-1;
    }
    if(!pSQLStatement)
      p03varparttrace (sqlra, gae);
    else
      pr01TraceCmd(sqlra, pSQLStatement);
  }
}     /*--------------------------------------------*/

void
p03puttracename (sqlratype * sqlra, char *string, tsp00_Int2 strl,
    BOOLEAN trwrite, struct SQLERROR *sqlemp)
{
    sqltatype      *tap = sqlra->rasqltap;
    short          *t80l = &sqlra->rasqltap->tastr80l;

    M90TRACE (M90_TR_STRING, "p03puttracename", "entry");
    memcpy (tap->tastr80, &string, strl);
    *t80l += strl;
    if (trwrite == TRUE)
        p03tvfwritetrace (sqlra, sqlemp);
}     /*--------------------------------------------*/

/*!
  Function:     p03varparttrace

  see also:

  Description:

  Arguments:
    sqlra [in] pointer to sqlra area, which contains paramters for
               write trace

  return value: void
 */
void p03varparttrace (sqlratype * sqlra, sqlgaentry *gaen)
{
  sqltatype      *tap = sqlra->rasqltap;

  M90TRACE (M90_TR_STRING, "p03varparttrace", "entry");
  if (tap->tatrout != CPR_TRACE_OFF) {
    short           *t80l = &sqlra->rasqltap->tastr80l;
    struct SQLERROR errmsg;
    tsp00_Int4        partl;
    tsp00_Int4        zeropos;
    tsp00_Int4        offset;
    tsp1_part      *part_ptr;
    tsp1_segment   *segmp;
    tpr05_String   pString;
    tpr05_StringEncoding encType;
    
    if (gaen && gaen->gareqptr) {
      encType = pr03PacketGetEncoding (gaen->gareqptr);
    } else {
      /*default encoding is Ascii, because not all calling
        functions have a gaentry */
      encType = sp77encodingAscii;
    }
    
    if (encType == sp77encodingAscii) {
      zeropos = 0;
      offset = 1;
    } else if (encType == sp77encodingUCS2) {
      zeropos = 0;
      offset = 2;
    } else if (encType == sp77encodingUCS2Swapped) {
      zeropos = -1;
      offset = 2;
    } else {
      zeropos = 0;
      offset = 1;
    }
    
    p03find_part (sqlra, (char) sp1pk_command,&part_ptr);
    if (part_ptr != NULL) {
      partl = (part_ptr->sp1p_part_header.sp1p_buf_len - 1);
      /*ignore padding blanks*/
      while ((  (part_ptr->sp1p_buf[partl+zeropos] == bsp_c1)
                ||(part_ptr->sp1p_buf[partl+zeropos] == CPR_PC_ENDSIGN))
             && (partl > 1)) {
        partl -= offset;
      }          
      if (partl <= 0)
        /*no command in part the exit*/
        {
          return;
        }
      segmp = sqlra->rasegptr;
      if (segmp->sp1s_segm_header.sp1c_prepare == TRUE) {
        memcpy (&tap->tastr80[*t80l], "PREPARE        :  ", 18);
        *t80l += 18;
        p03tvfwritetrace (sqlra, &errmsg);
      }
      if (segmp->sp1s_segm_header.sp1c_mass_cmd == TRUE) {
        memcpy (&tap->tastr80[*t80l], "MASS STATEMENT :  ", 18);
        *t80l += 18;
        p03tvfwritetrace (sqlra, &errmsg);
      }
      /*put command to trace*/
      pString.rawString = (char*)part_ptr->sp1p_buf;
      pString.cbLen = partl+1;
      if (gaen && gaen->gareqptr) {
        pString.encodingType = pr03PacketGetEncoding( gaen->gareqptr );
      }
      else {
        /*default encoding is Ascii, because not all calling
          functions have a gaentry */
        pString.encodingType = sp77encodingAscii;
      }
      pr01TraceCmd(sqlra, &pString);
    }/*end if part*/
  }/*end if trace off*/
}     /*--------------------------------------------*/

/*!
  Function:     p03tvfopentrace

  see also:

  Description:
  Er?offnet den Tracefile (tatracefn mit tatraceno).
  Ist read_write mit VREAD angegeben, so wird er
  mit direction VREAD  er?offnet, andernfalls wird er in abh?angigkeit von
  rasqlinit = CPR_is_end mit direction = VAPPEND, sonst mit VOWERWRITE
  er?offnet. Treten beim ?Offnen Fehler aus, so wird im sqlem-Record
  elzu=sqlnotok gesetzt, der Fehlertext in etext abgelegt, eprerr=
  tracefile open error gesetzt und das Traceschreiben wird abgeschaltet.

  Arguments:
    sqlra [in]
    read_write [in]
    sqlemp [out]

  return value: void
 */
void
p03tvfopentrace (sqlratype * sqlra, tsp00_VFileOpCodes read_write,
    struct SQLERROR *sqlemp)
{
    tsp05_RteFileError   vferr;
    tsp00_VfFormat   format;
    tsp00_VFileOpCodes     direction;
    tsp00_Int4        rec_len;
    sqltatype      *tap = sqlra->rasqltap;

    M90TRACE (M90_TR_STRING, "p03tvfopentrace", "entry");
    vferr.sp5fe_result = vf_ok;
    rec_len = 0;
    format = vf_plaintext;
    direction = read_write;
    if (read_write != sp5vf_read)
        if (sqlra->rasqlinit == cpr_is_end)
            direction = sp5vf_append;
        else
            direction = sp5vf_write;
    M90TRACELEN (M90_TR_STRING, "tracefilename", &tap->tatracefn, 30);
    sqlfopenp (tap->tatracefn, sp5vf_text, direction, sp5bk_unbuffered, (tsp00_Int4*) &tap->tatraceno, &vferr);
    if (vferr.sp5fe_result != vf_ok) {
      sqlemp->elzu = sp1ce_notok;
      memcpy (sqlemp->etext, vferr.sp5fe_text, 40);
      sqlemp->eprerr = cpr_tracefile_open_error;
      p03cseterror (sqlemp, sqlemp->eprerr);
      tap->tatrout = CPR_TRACE_OFF;
      tap->tatracety = CPR_TRACE_OFF;
      tap->tatraceno = 0;
      sqlresult (CPR_RESULT_TRACE_ERROR);
    }
    else {
      if (direction == sp5vf_write) {
        tsp00_Int2       *ll = &tap->tastr80l;
        *ll=sprintf(tap->tastr80, "<html><head><meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\"></head><body ><PRE><PLAINTEXT>");
        p03tvfwritetrace (sqlra, sqlemp);
      }
    }
}     /*--------------------------------------------*/

/*!
  Function:     p03tvfwritetrace

  see also:

  Description:
  Schreibt aus der sqlta-area tastr80 del L?ange tastr80l in den Tracefile
  (tatraceno).
  Treten beim Schreiben Fehler auf, so wird der sqlem-Record belegt und
  das Traceschreiben wird abgeschaltet.

  Arguments:
    sqlra [in]
    sqlemp [out]

  return value: void
 */
void
p03tvfwritetrace (sqlratype * sqlra, struct SQLERROR *sqlemp)
{
    tsp05_RteFileError   vferr;
    sqltatype      *tap = sqlra->rasqltap;
    char           *buf = tap->tastr80;
    tsp00_Int4        strlen;

    M90TRACE (M90_TR_STRING, "p03tvfwritetrace", "entry");
    vferr.sp5fe_result = vf_ok;
    strlen = (tsp00_Int4) tap->tastr80l;
    sqlemp->elzu = sp1ce_ok;
    if (tap->tatraceno == 0
		&& (  tap->tatracety > CPR_TRACE_OFF
		    ||tap->tatrout   > CPR_TRACE_OFF)) {
		p03tvfopentrace (sqlra, vwrite, sqlemp);
	}
    if (sqlemp->elzu != sp1ce_notok) {
#ifdef SQLODBC
      pa09Semaphore (PA09_SEMAPHORE_WRITETRACE, PA09_WAIT_FOR_SEMAPHORE);
#endif
      sqlfwritep (tap->tatraceno, buf, strlen, &vferr);
      tap->tastr80l = 0;
#ifdef SQLODBC
      pa09Semaphore (PA09_SEMAPHORE_WRITETRACE, PA09_RELEASE_SEMAPHORE);
#endif
    }
    if (   vferr.sp5fe_result != vf_ok
        || sqlemp->elzu == sp1ce_notok)
        {
        sqlemp->elzu = sp1ce_notok;
        memcpy (sqlemp->etext, vferr.sp5fe_text, 40);
        sqlemp->eprerr = cpr_tracefile_write_error;
        p03cseterror (sqlemp, sqlemp->eprerr);
        tap->tatrout = CPR_TRACE_OFF;
        tap->tatracety = CPR_TRACE_OFF;
        tap->tatraceno = 0;
        sqlresult (CPR_RESULT_TRACE_ERROR);
        }
}     /*--------------------------------------------*/

/*!
  Function:     p03tvfreadtrace

  see also:

  Description:
  Lie?zt in die sqlta-area tastr80 mit der L?ange tastr80l aus dem Tracefile
  (tatraceno).
  Treten beim Lesen Fehler auf, so wird der sqlem-Record belegt und
  das Traceschreiben wird abgeschaltet.

  Arguments:
    sqlra [in]
    sqlemp [out]

  return value: void
 */
void
p03tvfreadtrace (sqlratype * sqlra, struct SQLERROR *sqlemp)
{
  tsp05_RteFileError   vferr;
    sqltatype      *tap = sqlra->rasqltap;
    char           *buf = tap->tastr80;
    tsp00_Longint  strlen;


    M90TRACE (M90_TR_STRING, "p03tvfreadtrace", "entry");
    vferr.sp5fe_result = vf_ok;
    strlen = 0;
    sqlfreadp (tap->tatraceno, buf, sizeof(tap->tastr80), &strlen, &vferr);
    tap->tastr80l = (tsp00_Int2) strlen;
    if (vferr.sp5fe_result != vf_ok)
        {
        sqlemp->elzu = sp1ce_notok;
        memcpy (sqlemp->etext, vferr.sp5fe_text, 40);
        sqlemp->eprerr = cpr_tracefile_write_error;
        p03cseterror (sqlemp, sqlemp->eprerr);
        tap->tatrout = CPR_TRACE_OFF;
        tap->tatracety = CPR_TRACE_OFF;
        tap->tatraceno = 0;
        }
}     /*--------------------------------------------*/

/*!
  Function:     p03tvfclosetrace

  see also:

  Description:
  Schlie?zt den in der sqlta-area angegebenen Tracefile (tatraceno).
  Treten beim Schlie?zen Fehler auf, so wird der sqlem-Record belegt und
  das Traceschreiben wird abgeschaltet.

  Arguments:
          sqlra [in]
          sqlemp [out]

  return value: void
 */

void
p03tvfclosetrace (sqlratype * sqlra, struct SQLERROR *sqlemp)
{
    tsp05_RteFileError   vferr;
    sqltatype      *tap = sqlra->rasqltap;

    M90TRACE (M90_TR_STRING, "p03tvfclosetrace", "entry");
    M90TRACELEN (M90_TR_STRING, "tracefilename", &tap->tatracefn, 30);
    if (tap->tatraceno != 0)
        {
        vferr.sp5fe_result = vf_ok;
        sqlfclosep (tap->tatraceno, sp5vf_close_normal, &vferr);
	if (vferr.sp5fe_result != vf_ok)
            {
            sqlemp->elzu = sp1ce_notok;
            memcpy (sqlemp->etext, vferr.sp5fe_text, 40);
            sqlemp->eprerr = cpr_tracefile_close_error;
            p03cseterror (sqlemp, sqlemp->eprerr);
            }
        tap->tatraceno = 0;
        }
}     /*--------------------------------------------*/

void
p03datafopen (sqlratype * sqlra,
    tsp00_VFilename filename, tsp00_Int4 * fileno, tsp05_RteFileMode read_write,
    struct SQLERROR *sqlemp)
{
    tsp05_RteFileError   vferr;
    sqltatype      *tap = sqlra->rasqltap;

    M90TRACE (M90_TR_STRING, "p03datafopen", "entry");
    *fileno = 0;
    M90TRACELEN (M90_TR_STRING, "datafilename", filename, 30);
    sqlfopenp (filename, sp5vf_binary, read_write, sp5bk_buffered, fileno, &vferr);
    if (vferr.sp5fe_result != vf_ok)
      {
        sqlemp->elzu = sp1ce_notok;

        memcpy (sqlemp->etext, vferr.sp5fe_text, PR07MIN(sizeof(sqlemp->etext), sizeof(vferr.sp5fe_text)));
        sqlemp->eprerr = cpr_datafile_open_error;
        p03cseterror (sqlemp, sqlemp->eprerr);
        *fileno = 0;
        }
}     /*--------------------------------------------*/

void
p03datafwrite (sqlratype * sqlra,
    tsp00_Int4 fileno, tsp00_Int4 filelen, char *databuf,
    tsp00_Int4 datapos, struct SQLERROR *sqlemp)
{
    tsp05_RteFileError   vferr;
    sqltatype      *tap = sqlra->rasqltap;
    tsp00_Int4       flen;
    tsp00_Int4       len;

    M90TRACE (M90_TR_STRING, "p03datafwrite", "entry");
    flen = mxsp_buf;
    len = 0;
    databuf += (datapos - 1);
    do
        {
        if (filelen - len < mxsp_buf)
            flen = filelen - len;
#ifdef SQLODBC
        pa09Semaphore (PA09_SEMAPHORE_WRITETRACE, PA09_WAIT_FOR_SEMAPHORE);
#endif
	sqlfwritep (fileno, databuf, flen, &vferr);
#ifdef SQLODBC
        pa09Semaphore (PA09_SEMAPHORE_WRITETRACE, PA09_RELEASE_SEMAPHORE);
#endif
        databuf += flen;
        len = len + flen;
        }
    while ((filelen > len) && (vferr.sp5fe_result == vf_ok));
    if (vferr.sp5fe_result != vf_ok)
        {
        sqlemp->elzu = sp1ce_notok;
	memcpy (sqlemp->etext, vferr.sp5fe_text, 40);
        sqlemp->eprerr = cpr_datafile_write_error;
        p03cseterror (sqlemp, sqlemp->eprerr);
        }
}     /*--------------------------------------------*/


void
p03datafread (sqlratype * sqlra,
    tsp00_Int4 fileno, tsp00_Int4 * filelen, char *databuf,
    tsp00_Int4 datapos, tsp00_Int2 * dataeof, struct SQLERROR *sqlemp)
{
  tsp05_RteFileError   vferr;
  sqltatype      *tap = sqlra->rasqltap;
  tsp00_Longint   flen;
  tsp00_Int4        len;

  M90TRACE (M90_TR_STRING, "p03datafread", "entry");
  flen = mxsp_buf;
  len = 0;
  databuf += (datapos - 1);
  *dataeof = cpr_is_false;
  M90TRACE (M90_TR_SDWORD, "filelen  ", filelen);
  do {
    if (*filelen - len < mxsp_buf) {
      flen = *filelen - len;
    }
    sqlfreadp (fileno, databuf, mxsp_buf, &flen, &vferr);
    databuf += flen;
    len = len + (tsp00_Int4)flen;
  }
  while ((*filelen > len) && (vferr.sp5fe_result == vf_ok));
  *filelen = len;
  if (vferr.sp5fe_result == vf_eof) {
    *dataeof = cpr_is_true;
  }
  else if (vferr.sp5fe_result != vf_ok) {
    sqlemp->elzu = sp1ce_notok;
    memcpy (sqlemp->etext, vferr.sp5fe_text, PR07MIN(sizeof(sqlemp->etext), sizeof(vferr.sp5fe_text)));
    sqlemp->eprerr = cpr_datafile_read_error;
    p03cseterror (sqlemp, sqlemp->eprerr);
  }
}     /*--------------------------------------------*/



void
p03datafclose (sqlratype * sqlra,
    tsp00_Int4 *fileno, struct SQLERROR *sqlemp)
{
  tsp05_RteFileError vferr;
  sqltatype      *tap = sqlra->rasqltap;

  M90TRACE (M90_TR_STRING, "p03datafclose", "entry");
  if (*fileno != 0) {
    sqlfclosep (*fileno, sp5vf_close_normal, &vferr);
    if (vferr.sp5fe_result != vf_ok) 	{
      sqlemp->elzu = sp1ce_notok;
      memcpy (sqlemp->etext, vferr.sp5fe_text, PR07MIN(sizeof(sqlemp->etext), sizeof(vferr.sp5fe_text)));
      sqlemp->eprerr = cpr_datafile_close_error;
      p03cseterror (sqlemp, sqlemp->eprerr);
    }
    *fileno = 0;
  }
}     /*--------------------------------------------*/

/*!
  Function:     p03connect

  see also:

  Description:  Connects to the kernel and sets the date/time format for
  this session. If racomponent[0] != 'R' p03connect will also sends a set
  language cmd to the kernel. The KnlSessionID received by connecting to the
  kernel will be stored in the corresponding gaentry.


  Arguments:
           sqlrap [in]   Request Area
           sqlga  [in]
           datetime [in] date/time format (CPR_DT_EMPTY, CPR_DT_NORMAL,
           CPR_DT_ISO, CPR_DT_USA, CPR_DT_EUR, CPR_DT_JIS, CPR_DT_ANSI)
           sqlemp [out]  ErrorStruct

  return value: none
*/
extern tpr01_EnvContainer *sqlEnvCont;

void
p03connect (sqlxatype *sqlxa, sqlratype * sqlrap, 
            sqlgatype * sqlga, sqlgaentry *gaen,
            tsp00_Int2 datetime, struct SQLERROR *sqlemp)
{
  tpr01_ModuleDesc *ModuleDesc = sqlxa->xaModDesc;
  tpr01_ModuleContainer *Module = ModuleDesc->Module;
  tpr01_PrecomDesc *PrecomDesc = Module->GetPrecomDesc(ModuleDesc, sqlxa, sqlxa->xakano);
  tpr00_DatabaseName szDatabaseName;

  tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
  tpr01_EnvDesc *EnvDesc = Precom->sqlca->sqlEnvDesc;
  tpr01_EnvContainer *Env = EnvDesc->Env;
  tpr01_ConContainer *Connection = Env->GetConnectionCont(EnvDesc);
  tpr01_ConDesc *ConDesc = NULL;

  int nSession = sqlrap->raactsession;

  epr01Con_SessionType SessionType = (nSession == 0) ? 
    SessionTemp_epr01Con : SessionDefault_epr01Con;
  
  sprintf(szDatabaseName, ":%d", nSession);
  if (nSession == 1) 
    SessionType = SessionPrimary_epr01Con;
  
  ConDesc = Connection->FindDesc(Connection, szDatabaseName, SessionType);
  if (!ConDesc)
    ConDesc = Connection->AddDesc(Connection, szDatabaseName, sqlxa, SessionType);
  
  sqlxa->xalang = CPR_LA_C;

  memcpy (ConDesc->ga->gaxuserrec.xu_servernode,
          gaen->gaxuserrec.xu_servernode,
          sizeof (gaen->gaxuserrec.xu_servernode));
  memcpy (ConDesc->ga->gaxuserrec.xu_serverdb,
          gaen->gaxuserrec.xu_serverdb,
          sizeof (gaen->gaxuserrec.xu_serverdb));
  memcpy (ConDesc->ga->gaxuserrec.xu_user,
          gaen->gaxuserrec.xu_user,
          sizeof (gaen->gaxuserrec.xu_user));
  memcpy (ConDesc->ga->gaxuserrec.xu_password,
          gaen->gaxuserrec.xu_password,
          sizeof (gaen->gaxuserrec.xu_password));
  if (*(const unsigned short *) &gaen->gaxuserrec.xu_userUCS2[0] != ' ') {
    memcpy (ConDesc->ga->gaxuserrec.xu_userUCS2,
            gaen->gaxuserrec.xu_userUCS2,
            sizeof (gaen->gaxuserrec.xu_userUCS2));
    memcpy (ConDesc->ga->gaxuserrec.xu_passwordUCS2,
            gaen->gaxuserrec.xu_passwordUCS2,
            sizeof (gaen->gaxuserrec.xu_passwordUCS2));
  }

  p03conninfoget (sqlga, ConDesc->ga, (tsp00_Int2) nSession);
  p03messcodeget (ConDesc->ga);
  pr03ConConnect (ConDesc);
  gaen = ConDesc->ga;
  sqlga->gaentry[ConDesc->SessionNr-1] = gaen;
}

void pr03cSessionInfoGet(sqlratype * sqlrap, sqlgaentry * gaen)
{
  tpr03_KernelVersionC szKernelVersion;
  if (pr03PacketGetKernelParameters( sqlrap->rasegptr, KernelVersionString_epr03, szKernelVersion)) {
    sscanf(szKernelVersion, "%d", &gaen->gaKernelVersion);
  }
  else {
    gaen->gaKernelVersion = VersionUnknown_cpr00;
  }
}

/*!
  Function:     p03sqlaconnect

  see also:

  Description:
  Die Procedure belegt die actuellen User-Information mit der eventuellen
  Uebersteuerung der User-Optionen in den entsprechenden gaentry.
  Danch wird die LZU-sqlaconnect aufgerufen, sie stellt die Verbindung
  zur Datenbank der gaentry [sqlrap->raactsession-1] her.
  Dieser Procedure muss vor dem Aufbau des Connectkommandos aufgerufen
  werden.

  Arguments:
           sqlrap [in]   Request Area
           sqlga  [in]
           gaen   [in]
           sqlemp [out]  ErrorStruct

  return value: none
*/

void
p03sqlaconnect (sqlratype * sqlrap, sqlgatype * sqlga, sqlgaentry *gaen,
    struct SQLERROR *sqlemp)
{
  sqlxuserrec    *gaxu = &gaen->gaxuserrec;
  tsp1_packet    *gaqp = (tsp1_packet *) gaen->gareqptr;
  tsp00_Int4        packet_len = 0;
  tsp1_packet_list packetlist;

  M90TRACE (M90_TR_ENTRY, "p03sqlaconnect", 0);

  if (sqlemp->ereturncode == 0) {

    if (sqlrap->raactsession == CPR_SE_EMPTY)
      p03cseterror (sqlemp, cpr_no_session_connected);
    else {
      tsp00_ErrTextc szErrText;
      if (gaen->gareference == 0) {
	tsp00_Service service;
        BOOLEAN OldStyle = TRUE;
        if (sqlga->gaConnection) {
          tpr01_ConContainer *Connection = sqlga->gaConnection;
          OldStyle = Connection->OldStyle;
        }
#ifndef SQLODBC
	service=sql_user;
#else
	pa01GetService(&service);
#endif
	sqluid ((tsp00_TaskId*)&sqlrap->raprocessid);
        if (OldStyle)
          p03conninfoget (sqlga, gaen, sqlrap->raactsession);
	sqlemp->elzu = sp1ce_ok;
	packetlist[0] = NULL;
	packetlist[1] = NULL;
#ifdef SQLODBC
    {
    int maxWait  = 0;
    int maxRetry = 0;

    pa09MTHandler (PA09_CONNECT_BEGIN);
    while (pa09MTHandler (PA09_WAIT_FOR_BEGIN) != TRUE)
    /* if the thread does not get permission to connect after some time,
       release the connect semaphore and wait. Retry after that.
       http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1141612 */
#ifdef WIN32
      Sleep (10);
#else
      {
      usleep (10000*maxWait);

      if (maxWait++ > 10) {
        maxWait = 0;
        if (maxRetry++ > 10) {
          /* no connect could be performed, some thread seems to block. */
          p03cseterror (sqlemp, cpr_session_not_connected);
          break;
        } else {
          /* let the other threads work for 3 seconds */
          pa09MTHandler (PA09_CONNECT_END);
          usleep (3000000);
          pa09MTHandler (PA09_CONNECT_BEGIN);
        }
      }
      }
#endif
    }
#endif
	if (gaen->gaServerNode) {
	  tsp00_DbNamec szServerDB;
	  pr07P2C(szServerDB, gaxu->xu_serverdb, sizeof(gaxu->xu_serverdb));
	  M90TRACE (M90_TR_STRING, "gaServerNode", &gaen->gaServerNode);
	  M90TRACE (M90_TR_STRING, "szServerDB", &szServerDB);
	  sqlemp->elzu = SqlAConnect(gaen->gaServerNode, szServerDB, service,
				     1, &gaen->gareference, &packet_len,
				     (void*)packetlist, szErrText);
	}
	else {
	  M90TRACELEN (M90_TR_STRING, "xu_servernode", &gaxu->xu_servernode, sizeof(gaxu->xu_servernode));
	  M90TRACELEN (M90_TR_STRING, "xu_serverdb", &gaxu->xu_serverdb, sizeof(gaxu->xu_serverdb));
	  szErrText[sizeof(tsp00_ErrText)] = '\0';
	  sqlaconnect (sqlrap->raprocessid, gaxu->xu_servernode,
		       gaxu->xu_serverdb, service, 1,
		       (tsp00_Int4*) &gaen->gareference, &packet_len,
		       (void*)packetlist, (unsigned char*)szErrText, (unsigned char*)
		       &sqlemp->elzu);
	}
	if (sqlemp->elzu != sp1ce_ok) {
	  pr07C2P(sqlemp->etext, szErrText, sizeof(sqlemp->etext));
	  p03clzuerror (gaen, CPR_LZU_CONNECT, sqlemp);
	}
	else {
	  /** *packetlist[1]  for query only **/
	  gaen->gapacketlist[0] = packetlist[0];
	  gaen->gapacketlist[1] = packetlist[1];
	  if (packetlist[1] != NULL) {
	    M90TRACE (M90_TR_STRING, "p03sqlaconnect ", "list[1]");
	    gaqp = packetlist[1];
	    gaqp->sp1_header.sp1h_varpart_size = packet_len - sizeof (tsp1_packet_header);
	  }
	  gaqp = packetlist[0];
	  gaqp->sp1_header.sp1h_varpart_size = packet_len - sizeof (tsp1_packet_header);
	  gaen->gareqptr = gaqp;
	}
#ifdef SQLODBC
    pa09MTHandler (PA09_CONNECT_END);
#endif
	if (sqlemp->eprerr != cpr_p_ok) {
	  sqltatype      *tap = sqlrap->rasqltap;

	  /** only for trace output  if error ***/
	  if (tap->tatrout != CPR_TRACE_OFF) {
	    if (sqlrap->ranotrtimeoutp == cpr_is_false) {
	      sqldattime (sqlrap->rasqltap->tastartdate,
			  sqlrap->rasqltap->tastarttime);
	      memcpy (sqlrap->rasqltap->taenddate,
		      sqlrap->rasqltap->tastartdate, 8);
	      memcpy (sqlrap->rasqltap->taendtime,
		      sqlrap->rasqltap->tastarttime, 8);
	    }
	    gaen->gareqptr = pr03PacketNewPacket(1000);
	    p03putconnect (NULL, sqlrap, gaen, sqlemp);
	    p03connectcmdtrace (sqlrap, gaen);
	    pr03PacketDeletePacket(gaen->gareqptr);
	    gaen->gareqptr = NULL;
	  }
	  p03cseterror (sqlemp, sqlemp->eprerr);
	}
      }
    }
  }
  M90TRACE (M90_TR_EXIT, "p03sqlaconnect", 0);
}     /*--------------------------------------------*/

/*!
  Function:     p03setformat

  see also:

  Description:
  Diese Procedure teilt dem Kern mit in welchem Format die Date-Time-Funktionen
  behandelt werden sollen. Die Art der Funktion wird als Option "-D xxx"
  beim Precompileren fest gelegt. Die Angabe steht in sqlca.sqldatetime und
  wird als Parameter datetime mitgegeben. Sie wird nach dem Connectkommando
  an den Kern gesendet.

  Arguments:
           sqlrap [in]   Request Area
           gaent  [in]
           datetime [in] date/time format (CPR_DT_EMPTY, CPR_DT_NORMAL,
           CPR_DT_ISO, CPR_DT_USA, CPR_DT_EUR, CPR_DT_JIS, CPR_DT_ANSI)
           sqlemp [out]  ErrorStruct

  return value: void
 */
void
p03setformat (struct tpr01_SQLDesc *SQLDesc, sqlratype * sqlrap, sqlgaentry * gaen,
    tsp00_Int2 datetime,
    struct SQLERROR *sqlemp)
{
    tsp00_Int2  setf;

    setf = cpr_is_true;
    if  (((sqlrap->rasqlansi == CPR_KIND_ANSI)
        || (sqlrap->radbmode == CPR_KIND_ORACLE)
        || (sqlrap->radbmode == CPR_KIND_SAPR3))
	&& ((datetime == CPR_DT_NORMAL)
	   || (datetime == CPR_DT_EMPTY)) )
		setf = cpr_is_false;
    else
    {
        tsp1_packet    *pck = (tsp1_packet *) gaen->gareqptr;
        tsp1_part      *part_ptr;
        tsp00_Int4       *p1l;
        tsp00_Int2     retkamode;

        M90TRACE (M90_TR_STRING, "p03setformat ", "entry");
	retkamode = sqlrap->rakamode;
	if  ((sqlrap->rasqlansi == CPR_KIND_ANSI)
		|| (sqlrap->radbmode == CPR_KIND_ORACLE)
		|| (sqlrap->radbmode == CPR_KIND_SAPR3))
			sqlrap->rakamode = CPR_KIND_INTERNAL;
        p03cpacketinit (SQLDesc, sqlrap, gaen, sp1m_dbs);
	pr03PacketSetEncoding(pck, sp77encodingAscii);
        s26new_part_init (pck, sqlrap->rasegptr, &part_ptr);
        part_ptr->sp1p_part_header.sp1p_part_kind = (char) sp1pk_command;
        p1l = &part_ptr->sp1p_part_header.sp1p_buf_len;
        sqlrap->rasegmpartptr [sp1pk_command] = part_ptr;
        *p1l = 0;
        p03initsqlem (sqlemp);
        strcpy ((char*)part_ptr->sp1p_buf, "SET FORMAT ");
        switch (datetime)
            {
            case CPR_DT_ISO:
                strcat ((char*)part_ptr->sp1p_buf, "ISO ");
                break;
            case CPR_DT_USA:
                strcat ((char*)part_ptr->sp1p_buf, "USA ");
                break;
            case CPR_DT_EUR:
                strcat ((char*)part_ptr->sp1p_buf, "EUR ");
                break;
            case CPR_DT_JIS:
                strcat ((char*)part_ptr->sp1p_buf, "JIS ");
                break;
            case CPR_DT_ANSI:
/** only odbc ***/
                strcat ((char*)part_ptr->sp1p_buf, "ANSI ");
                break;
            default:
                strcat ((char*)part_ptr->sp1p_buf, "INTERNAL ");
                break;
            }
        (*p1l) = (tsp00_Int4)strlen ((char*)part_ptr->sp1p_buf);
        p03cmdtrace (sqlrap, gaen, 0, cpr_com_empty, NULL);
        part_ptr->sp1p_part_header.sp1p_buf_len = *p1l;
        s26finish_part (pck, part_ptr);

        if (SQLDesc)
          pr03PacketReqRec(SQLDesc->ConDesc, sqlemp);
        else
          p03creqrecpacket (sqlrap, gaen, sqlemp);          
	sqlrap->rakamode = retkamode;
    }
}     /*--------------------------------------------*/

/*!
  Function:     p03putconnect

  see also:

  Description:
  Baut das Kommando Connect im Auftragspuffer auf. Vor dem Aufbau
  muss die Procedure p03sqlaconnect aufgerufen worden sein.

  Arguments:
           sqlrap [in]   Request Area
           gaent [in]
           sqlemp [out]  ErrorStruct

  return value: none
*/
void
p03putconnect (tpr01_SQLDesc *SQLDesc, sqlratype * sqlrap, sqlgaentry * gaent,
    struct SQLERROR *sqlemp)
{
#define MULTI_SET " CHARACTER SET "
#define L_MULTI_SET strlen(MULTI_SET)
  tsp1_packet    *pck = (tsp1_packet *) gaent->gareqptr;
  tsp1_part      *part_ptr;
  tsp00_Int4       *p1l;
  sqlxuserrec    *gaxu = &gaent->gaxuserrec;
  tsp00_KnlIdentifier termchar;
  tsp00_C18         tid;
  BOOLEAN         defaultlevel;
  char  chr12[12];
  int             len;

  M90TRACE (M90_TR_ENTRY, "p03putconnect", 0);
  if (!SQLDesc)
    p03messcodeget (gaent);
  memset (termchar, bsp_c1, sizeof(tsp00_KnlIdentifier));
  sqlcharsetname (termchar);
  p03cpacketinit (SQLDesc, sqlrap, gaent, sp1m_dbs);

  s26new_part_init (pck, sqlrap->rasegptr, &part_ptr);
  part_ptr->sp1p_part_header.sp1p_part_kind = (char) sp1pk_command;
  p1l = &part_ptr->sp1p_part_header.sp1p_buf_len;
  sqlrap->rasegmpartptr [sp1pk_command] = part_ptr;
  *p1l = 0;
  memcpy (&part_ptr->sp1p_buf[*p1l], "CONNECT ", 8);
  (*p1l) += 8;
  if (strncmp ((char*)gaxu->xu_user, bsp_c64, sizeof(gaxu->xu_user)) == 0) {
    p03cseterror (sqlemp, cpr_missing_user_passw);
  }
  len = s30lnr1 (gaxu->xu_user, bsp_c1, 1, sizeof(gaxu->xu_user));
  if (len < mxsp_name)
    len = mxsp_name;
  part_ptr->sp1p_buf[*p1l] = '"';
  (*p1l) += 1;
  memcpy (&part_ptr->sp1p_buf[*p1l], gaxu->xu_user, len);
  (*p1l) += len;
  part_ptr->sp1p_buf[*p1l] = '"';
  (*p1l) += 1;
  memcpy (&part_ptr->sp1p_buf[*p1l], " IDENTIFIED BY :A  ", 19);
  (*p1l) += 19;
  switch (sqlrap->radbmode) {
  case CPR_KIND_DB2: {
    memcpy (&part_ptr->sp1p_buf[*p1l], "SQLMODE DB2 ", 12);
    (*p1l) += 12;
    break;
  }
  case CPR_KIND_ORACLE: {
    if (sqlrap->rasqlansi == CPR_KIND_ANSI)
      memcpy (&part_ptr->sp1p_buf[*p1l], "SQLMODE ORACLE ", 15);
    else
      memcpy (&part_ptr->sp1p_buf[*p1l], "SQLMODE ORACLE ", 15);
    (*p1l) += 15;
    break;
  }
  case CPR_KIND_SAPR3: {
    memcpy (&part_ptr->sp1p_buf[*p1l], "SQLMODE SAPR3 ", 14);
    (*p1l) += 14;
    break;
  }
  default: {
      /** cpr_kind_internal**/
      if (sqlrap->rasqlansi == CPR_KIND_ANSI) {
	memcpy (&part_ptr->sp1p_buf[*p1l], "SQLMODE ANSI ", 13);
	(*p1l) += 13;
      }
      break;
    }
  }
  if ((gaent->ganolog == cpr_is_true)
      && (sqlrap->racomponent[0] == 'R')) {
    memcpy (&part_ptr->sp1p_buf[*p1l], " NOLOG ", 7);
    (*p1l) += 7;
  }
  if (gaxu->xu_isolation != CPR_LO_EMPTY) {
    defaultlevel = FALSE;
    switch (sqlrap->radbmode) {
    case CPR_KIND_INTERNAL: {
      if (sqlrap->rasqlansi == CPR_KIND_ANSI) {
	if ((gaxu->xu_isolation == CPR_LO_ISOLEV3)
	    || (gaxu->xu_isolation == CPR_LO_ISOLEV30))
	  defaultlevel = TRUE;
      }
      else {
	if (gaxu->xu_isolation == CPR_LO_ISOLEV10)
	  defaultlevel = TRUE;
      }
      break;
    }
    case CPR_KIND_DB2: {
      if ((gaxu->xu_isolation == CPR_LO_ISOLEV1)
	  || (gaxu->xu_isolation == CPR_LO_ISOLEV10))
	defaultlevel = TRUE;
      break;
    }
    case CPR_KIND_ORACLE:{}
    case CPR_KIND_SAPR3:{
      if (sqlrap->rasqlansi == CPR_KIND_ANSI) {
	if ((gaxu->xu_isolation == CPR_LO_ISOLEV3)
	    || (gaxu->xu_isolation == CPR_LO_ISOLEV30))
	  defaultlevel = TRUE;
      }
      else
	{
	  if ((gaxu->xu_isolation == CPR_LO_ISOLEV1)
	      || (gaxu->xu_isolation == CPR_LO_ISOLEV10))
	    defaultlevel = TRUE;
	}
      break;
    }
    default: {
      defaultlevel = FALSE;
      break;
    }
    }
    if (defaultlevel == FALSE) {
      memcpy (&part_ptr->sp1p_buf[*p1l], " ISOLATION LEVEL ", 17);
      (*p1l) += 17;
      len = sprintf (chr12, "%d", gaxu->xu_isolation);
      memcpy (&part_ptr->sp1p_buf[*p1l], chr12, len);
      (*p1l) += len;
    }
  }
  M90TRACE (M90_TR_SWORD, "gaxutimeout  ", &gaxu->xu_timeout);
  if (gaxu->xu_timeout != -1) {
    memcpy (&part_ptr->sp1p_buf[*p1l], "  TIMEOUT ", 10);
    (*p1l) += 10;
    len = sprintf (chr12, "%d", gaxu->xu_timeout);
    memcpy (&part_ptr->sp1p_buf[*p1l], chr12, len);
    (*p1l) += len;
  }
  if (gaxu->xu_cachelimit != -1) {
    memcpy (&part_ptr->sp1p_buf[*p1l], "  CACHELIMIT ", 13);
    (*p1l) += 13;
    len = sprintf (chr12, "%d", gaxu->xu_cachelimit);
    memcpy (&part_ptr->sp1p_buf[*p1l], chr12, len);
    (*p1l) += len;
  }
  if (strncmp ((char*)termchar, bsp_c64, sizeof(tsp00_KnlIdentifier)) != 0) {
    memcpy (&part_ptr->sp1p_buf[*p1l], "  TERMCHAR SET ", 15);
    (*p1l) += 15;
    memcpy (&part_ptr->sp1p_buf[*p1l], &termchar,
	    sizeof(tsp00_KnlIdentifier));
    (*p1l) += sizeof(tsp00_KnlIdentifier);
  }
  if (strncmp ((char*)gaxu->xu_dblang, bsp_c18, mxsp_c18) != 0) {
    memcpy (&part_ptr->sp1p_buf[*p1l], MULTI_SET, L_MULTI_SET);
    (*p1l) += (tsp00_Int4)L_MULTI_SET;
    memcpy (&part_ptr->sp1p_buf[*p1l], &gaxu->xu_dblang, mxsp_c18);
    (*p1l) += mxsp_c18 ;
  }
  part_ptr->sp1p_part_header.sp1p_buf_len = *p1l;
  s26finish_part (pck, part_ptr);

  s26new_part_init (pck, sqlrap->rasegptr, &part_ptr);
  p1l = &part_ptr->sp1p_part_header.sp1p_buf_len;
  part_ptr->sp1p_part_header.sp1p_part_kind = (char) sp1pk_data;
  sqlrap->rasegmpartptr [sp1pk_data] = part_ptr;
  *p1l = 0;
  part_ptr->sp1p_buf[*p1l] = '\0';
  memcpy (&part_ptr->sp1p_buf[1], gaxu->xu_password, mxsp_c24);
  (*p1l) = mxsp_c24 + 1;
  memset (&tid, bsp_c1, mxsp_termid);
  sqltermid (tid);
  part_ptr->sp1p_buf[*p1l] = ' ';
  memcpy (&part_ptr->sp1p_buf[*p1l + 1], &tid, mxsp_termid);
  (*p1l) += (mxsp_termid + 1);
  part_ptr->sp1p_part_header.sp1p_buf_len = *p1l;
  s26finish_part (pck, part_ptr);
  M90TRACE (M90_TR_EXIT, "p03putconnect", 0);
}     /*--------------------------------------------*/



/*!
  Function:     p03messcodeget

  see also:

  Description:
  Belegt den ga_messcode im gaentry richtig. Die Procedure muss einmal vor
  der Belegung des Connectauftragspuffers aufgerufen werden.

  Arguments:
           gaentry  [in]

  return value: void
 */
void
p03messcodeget (sqlgaentry * gaentry)
{
  union sw
  {
    char            c[4];
    tsp00_Int4      l;
  }
  swapb;

  if (' ' == (char) 32)
    gaentry->gamesscode[0] = 0x00;
  else
    gaentry->gamesscode[0] = 0x01;
  swapb.l = 1;
  if (swapb.c[3] == 0x01)
    gaentry->gamesscode[1] = 0x01;
  else if (swapb.c[0] == 0x01)
    gaentry->gamesscode[1] = 0x02;
  else
    gaentry->gamesscode[1] = 0x03;
  M90TRACEBUF ("gamesscode", &gaentry->gamesscode, 0, 2);
}     /*--------------------------------------------*/


/*!
  Function:     p03conninfoget

  see also:

  Description:
  Vor dem Aufruf sqlaconnect der LZU muss diese Porcedure aufgerufen werden.
  Sie setzt die User-und Server-Angaben in gaentry richtig. User-Optionen
  ?ubersteuern die Angaben und werden in gaentry umgestzt, so da?z in
  gaentry immer die richtigen Angaben stehen, auch sp?ater f?ue ein reconnect.

  Arguments:
           sqlga [in]
           gaen  [in]
           SessionCnt [in] number sessions connected

  return value: void
 */
void
p03conninfoget (sqlgatype * sqlga, sqlgaentry *gaen, tsp00_Int2 SessionCnt)
{
  sqlxuserrec    *gaxu = &gaen->gaxuserrec;
  sqlxuserrec    *gaop = &sqlga->gaopxuserrec;
  tsp00_Int4     overwriteable = 0; /* overdrive != 0 means parameters are can be overwritten by sqlopt*/

  M90TRACE(M90_TR_ENTRY, "p03conninfoget", 0);
  M90TRACE(M90_TR_SWORD, "useropset    ", &sqlga->gauseropset);
  M90TRACE(M90_TR_SWORD, "gamodisolation", &sqlga->gamodisolation);
  M90TRACE(M90_TR_SWORD, "gaop_isolation", &gaop->xu_isolation);
  M90TRACE(M90_TR_SWORD, "gaxuisolation", &gaxu->xu_isolation);
  if (sqlga->gaConnection) {
    tpr01_ConContainer *Connection = sqlga->gaConnection;
    SessionCnt = Connection->SessionCnt;
  }
  if (  SessionCnt == 1
      ||gaen->gastatus == cpr_co_implicit) {
    overwriteable = 1;
  }
  if ((sqlga->gamodisolation != CPR_LO_EMPTY)
      && (overwriteable))
    gaxu->xu_isolation = sqlga->gamodisolation;
  if ((sqlga->gauseropset == cpr_is_true) && (overwriteable)) {
    /* PTS 1107595 nur fuer die Erste Session */
    if (strncmp ((char*)gaop->xu_user, bsp_c64, sizeof(gaop->xu_user)) != 0) {
      memcpy (gaxu->xu_user, gaop->xu_user, sizeof(gaop->xu_user));
      memcpy (gaxu->xu_password, gaop->xu_password, CRYPTPW_MXSP00);
    }
    if (gaop->xu_userUCS2[0] != ' ') {
      memcpy (gaxu->xu_userUCS2, gaop->xu_userUCS2, sizeof (gaop->xu_userUCS2));
      memcpy (gaxu->xu_passwordUCS2, gaop->xu_passwordUCS2, CRYPTPW_MXSP00);
      if (!strncmp ((char*)gaop->xu_user, bsp_c64, sizeof (gaop->xu_user))) {
        memset (gaxu->xu_user, bsp_c1, sizeof (gaxu->xu_user));
        memset (gaxu->xu_password, bsp_c1, sizeof (gaxu->xu_password));
      }
    }

    if (strncmp ((char*)gaop->xu_servernode, bsp_nodeid, NODEID_MXSP00) != 0)
      memcpy (gaxu->xu_servernode, gaop->xu_servernode, NODEID_MXSP00);

    if (strncmp ((char*)gaop->xu_serverdb, bsp_c64, sizeof(gaop->xu_serverdb)) != 0)
      memcpy (gaxu->xu_serverdb, gaop->xu_serverdb, sizeof(gaop->xu_serverdb));
    if (strncmp ((char*)gaop->xu_dblang, bsp_c64, sizeof(gaop->xu_dblang)) != 0)
      memcpy (gaxu->xu_dblang, gaop->xu_dblang, sizeof(gaop->xu_dblang));
    M90TRACELEN (M90_TR_STRING, "xu_dblang     ", &gaxu->xu_dblang,18);
    M90TRACELEN (M90_TR_STRING, "xu_serverdb   ", &gaxu->xu_serverdb,18);
    M90TRACELEN (M90_TR_STRING, "xu_servernode ", &gaxu->xu_servernode,64);
    if (gaop->xu_timeout != -1)
      gaxu->xu_timeout = gaop->xu_timeout;
    if (gaop->xu_cachelimit != -1)
      gaxu->xu_cachelimit = gaop->xu_cachelimit;
    /** sqlopt option ?ubersteuert  mod option  **/
    if (gaop->xu_isolation != CPR_LO_EMPTY)
      gaxu->xu_isolation = gaop->xu_isolation;
  }
  M90TRACE (M90_TR_SWORD, "gaxuisolation n", &gaxu->xu_isolation);
  M90TRACE (M90_TR_EXIT, "p03conninfoget", 0);
}     /*--------------------------------------------*/


/*!
  Function:     p03gaxuserinit

  see also:

  Description:
  Die Procedure belegt alle gaentrys mit Defaultwerten,
  und belegt den gaxuserrecord mit Angaben aus der Xuser-Datei,
  soweit diese Datei Werte enth?alt. (8 gaentrys maximal)
  Treten bei dem Lzu_aufruf (werte aus der Xuserdatei holen) Fehler
  auf wird in den sqlem-Record elzu=sqlnotok und in etext der
  Fehlertext des Lzu-aufrufs zur?uckgegeben.

  Arguments:
           sqlga  [in]
           sqlemp [out]  ErrorStruct

  return value: void
 */
void
p03gaxuserinit (sqlgatype * sqlga, struct SQLERROR *sqlemp)
{
  unsigned char   ok;
  short           i;

  M90TRACE (M90_TR_ENTRY, "p03gaxuserinit", 0);
  ok = TRUE;
  for (i = 0; i < sqlgamax; i++) {
    sqlgaentry     *ga = sqlga->gaentry[i];
    sqlxuserrec    *gaxu = &sqlga->gaentry[i]->gaxuserrec;

    if (ok == TRUE)
      p03sqlgaentryinit (ga, (tsp00_Int2) (i + 1), sqlemp);
    else
      p03gaentryinitdefault (ga);
    if (strncmp ((char*)gaxu->xu_user, bsp_c64, sizeof(gaxu->xu_user)) == 0) {
      ok = FALSE;
      sqlemp->elzu = sp1ce_ok;
    }
  }
  M90TRACE (M90_TR_EXIT, "p03gaxuserinit", 0);
}     /*--------------------------------------------*/

/*!
  Function:     p03gaentryinitdefault

  see also:

  Description:
  Belegt den angebenen gaentry mit Defaultwerten.

  Arguments:
    ga [in]

  return value: void
 */
void
p03gaentryinitdefault (sqlgaentry * ga)
{

  sqlxuserrec    *gaxu = &ga->gaxuserrec;

  M90TRACE (M90_TR_ENTRY, "p03gaentryinit", 0);

  ga->gareference = (tsp00_Int2) 0;
  memset (ga->gamesscode, bsp_c1, sizeof(ga->gamesscode));
  ga->gastatus = CPR_CO_OK;
  ga->gavaindex = 0;
  ga->ganodeindex = 0;
  ga->gareconnect = cpr_is_true;
  ga->ganolog = cpr_is_false;
  ga->gaprofinit = cpr_is_false;
  ga->gadbcodeset = 0;
  ga->gafiller2 = 0;
  memset (ga->gaatdbname, bsp_c1, sizeof(ga->gaatdbname));
  ga->gapatchptr = NULL;
  ga->gareqptr = NULL;
  ga->garecptr = NULL;
  ga->gaKernelVersion = VersionUnknown_cpr00;
  ga->gaServerNode = NULL;
  ga->gaIsCanceled = false;
  memset (&ga->gaKnlSessionID, '\0', sizeof(ga->gaKnlSessionID));
  memset (gaxu->xu_key, bsp_c1, sizeof(gaxu->xu_key));
  gaxu->xu_fill = 0;
  memset (gaxu->xu_servernode, bsp_c1, sizeof(gaxu->xu_servernode));
  memset (gaxu->xu_serverdb, bsp_c1, sizeof(gaxu->xu_serverdb));
  memset (gaxu->xu_user, bsp_c1, sizeof(gaxu->xu_user));
  memset (gaxu->xu_password, bsp_c1, sizeof(gaxu->xu_password));
  memset (gaxu->xu_sqlmode, bsp_c1, sizeof(gaxu->xu_sqlmode));
  gaxu->xu_cachelimit = -1;
  gaxu->xu_timeout = -1;
  gaxu->xu_isolation = CPR_LO_EMPTY;
  memset (gaxu->xu_dblang, bsp_c1, sizeof(gaxu->xu_dblang));
  *(unsigned short *) &gaxu->xu_userUCS2[0] = (unsigned short) 0;
  *(unsigned short *) &gaxu->xu_passwordUCS2[0] = (unsigned short) 0;

  M90TRACE (M90_TR_EXIT, "p03gaentryinit", 0);
}     /*--------------------------------------------*/


/*!
  Function:     p03sqlgaentryinit

  see also:

  Description:
  Belegt den angebenen gaentry mit Defaultwerten,
  und holt aus der Xuser-Datei den xuindex-ten Eintrag (1,2,..)
  in den angegebenen gaentry. Treten bei dem Lzu_aufruf Fehler
  auf wird in den sqlem-Record elzu=sqlnotok und in etext der
  Fehlertext des Lzu-aufrufs zur?uckgegeben.

  Arguments:
           ga [in]
           xuindex [in]
           sqlemp [out]  ErrorStruct

  return value: void
 */
void
p03sqlgaentryinit (sqlgaentry * ga, tsp00_Int2 xuindex,
    struct SQLERROR *sqlemp)
{
    tsp4_xuser_record    *gaxu = &ga->gaxuserrec;
    unsigned char   ok;

    M90TRACE (M90_TR_ENTRY, "p03sqlgaentryinit", 0);
    M90TRACE (M90_TR_PTR,   "ga", &ga);
    p03gaentryinitdefault (ga);
    if (xuindex > 0) {  /* don't look up xuser if xuindex == 0 */
      sqlindexuser ((tsp00_Int2) (xuindex), gaxu, NULL, sqlemp->etext, &ok);
      M90TRACE (M90_TR_SWORD, "xuindex ", &xuindex);
      M90TRACE (M90_TR_SCHAR, "ok      ", &ok);
      if (ok == FALSE)
        sqlemp->elzu = sp1ce_notok;
    }
    M90TRACE (M90_TR_EXIT, "p03sqlgaentryinit", 0);
}     /*--------------------------------------------*/

/*!
  Function:     p03csqlinit

  see also:

  Description:
  Diese Procedure muss einmal am Anfang aufgerufen werden (Precompiler,
  Anwendung).
  Sie belegt in der sqlra-area die Sprache ralang, die Component racomponent,
  ruft die Lzu-procedure sqlinit auf, d.h. initalisiert die das H?andling
  der Xuserprocesse, belegt rasqlinit mit CPR_is_init, d.h. sqlinit
  ist aufgerufen worden, und belegt mit dem Lzuaufruf sqlos die
  Betriebsystemangabe in rasqlos.

  Arguments:
          sqlrap [in]
          comp   [in]
          language [in]

  return value: void
 */
void
p03csqlinit (sqlratype * sqlrap, char comp, tsp00_Int2 language)
{
    tsp00_C64         component;
    static tsp00_BoolAddr    booladdr = NULL;
    tsp00_Int2 xuserrecsize = 0 ;

    M90TRACE (M90_TR_STRING, "p03csqlinit", "entry");
    xuserrecsize = sizeof(sqlxuserrec) ;
    M90TRACE (M90_TR_SWORD, "xuserrecsize: ", &xuserrecsize);
    memset (component, bsp_c1, mxsp_c64);
    memset (sqlrap->racomponent, bsp_c1, 3);
    component[0] = comp;
    sqlrap->racomponent[0] = comp;
    sqlrap->ralang = language;
    sqlrap->rasqlinit = cpr_is_init;
    sqlinit (component, booladdr);
    sqlos ((tsp00_Os*) &sqlrap->rasqlos);
}     /*--------------------------------------------*/

/*!
  Function:     p03ccmdinit

  see also:

  Description:
  Initalisiert die Meldungen f?ur den Anwender in der sqlca-area
  (sqlcode, warnings, ..), initalisiert den Fehlermeldungsrecord (sqlem),
  und initalisiert mit Defaultwerten das Auftragssegment (tsp_packet).

  Arguments:
          SQLDesc [in]
          sqlca   [in]
          gaen    [in]
          m_type  [in]   message code

  return value: void
 */
void
p03ccmdinit (tpr01_SQLDesc *SQLDesc, sqlcatype * sqlca, sqlgaentry *gaen, char m_type)
{
  sqlratype      *rap = sqlca->sqlrap;

  M90TRACE (M90_TR_ENTRY, "p03ccmdinit", 0);
  p03csqlcaareainit (sqlca);
  p03cpacketinit (SQLDesc, rap, gaen, m_type);
  M90TRACE (M90_TR_EXIT, "p03ccmdinit", 0);
}     /*--------------------------------------------*/

#ifdef SQLODBC
void pa01SetAutocommitFlag( tsp1_segment_ptr first_segm_ptr );
void pa01SetProducer( tsp1_segment_ptr first_segm_ptr );
#endif

/*!
  Function:     p03cpacketinit

  see also:

  Description:
  Initalisiert mit Defaultwerten das Auftragssegment (tsp_packet).

  Arguments:
           SQLDesc [in]
           sqlra   [in]
           gaen    [in]
           m_type  [in]

  return value: pointer to the orderinterface
 */
tsp1_packet *p03cpacketinit(struct tpr01_SQLDesc *SQLDesc,
			    sqlratype * sqlra,
			    sqlgaentry * gaen,
			    char m_type)
{
  tsp1_packet    *pck=NULL;
  tsp1_segment   *segp;

  M90TRACE (M90_TR_ENTRY, "p03cpacketinit", 0);
  M90TRACE (M90_TR_PTR,   "gaen", &gaen);
  if (SQLDesc) {
    pck = SQLDesc->ConDesc->SegmDesc->packetPtr;
    segp = pr03SegmentInit(SQLDesc->ConDesc->SegmDesc);
    sqlra->rasegptr = segp;
#ifdef SQLODBC
    pa01GetMesscode(&m_type);
    pa01SetAutocommitFlag( sqlra->rasegptr );
    pa01SetProducer( sqlra->rasegptr );
#endif
    segp->sp1s_segm_header.sp1c_mess_type = (char) m_type;
    segp->sp1s_segm_header.variant.C_1.sp1s_no_of_parts_F = 0;
    segp->sp1s_segm_header.sp1s_segm_len = sizeof (tsp1_segment_header);
    memset(sqlra->rasegmpartptr, 0, sizeof(sqlra->rasegmpartptr));
    pr03PacketSetEncoding(pck, SQLDesc->PacketEncoding);
    pr03PacketSetSwapKind(pck, SQLDesc->ConDesc->Connection->EnvDesc->swapKind);
    pr03SegmentSetSqlMode(segp, pr03cMapSqlMode(sqlra->rakamode, sqlra->rasqlansi));
  }
  else {
    M90TRACE (M90_TR_HANDLE, "gareqptr", &gaen->gareqptr);
    pck = (tsp1_packet *) gaen->gareqptr;
    s26init_cmd_packet (pck, &segp);
    memcpy (pck->sp1_header.sp1h_appl_version, sqlra->ratermid, 8);
    sqlra->rasegptr = segp;
#ifdef SQLODBC
    pa01GetMesscode(&m_type);
    pa01SetAutocommitFlag( sqlra->rasegptr );
#endif
    segp->sp1s_segm_header.sp1c_mess_type = (char) m_type;
    segp->sp1s_segm_header.variant.C_1.sp1s_no_of_parts_F = 0;
    segp->sp1s_segm_header.sp1s_segm_len = sizeof (tsp1_segment_header);
    memset(sqlra->rasegmpartptr, 0, sizeof(sqlra->rasegmpartptr));
    pr03PacketSetEncoding( pck, sp77encodingAscii );
    p03setsegmode(sqlra, gaen);
  }
  M90TRACE (M90_TR_PTR, "pck", &pck);
  M90TRACE (M90_TR_EXIT, "p03cpacketinit", 0);
  return(pck);
}

static tsp1_sqlmode pr03cMapSqlModeTab [] =
{ sp1sm_internal,
  sp1sm_internal,
  sp1sm_db2,
  sp1sm_ansi,
  sp1sm_oracle,
  sp1sm_oracle };

static tsp1_sqlmode pr03cMapAnsiModeTab [] =
{ sp1sm_ansi,
  sp1sm_ansi,
  sp1sm_db2,
  sp1sm_ansi,
  sp1sm_oracle,
  sp1sm_oracle };

tsp1_sqlmode pr03cMapSqlMode(tsp00_Int2 sqlmode, tsp00_Int2 sqlansi)
{
  if (sqlansi != CPR_KIND_ANSI ) {
    return pr03cMapSqlModeTab[sqlmode];
  }
  else {
    return pr03cMapAnsiModeTab[sqlmode];
  }
}

/*!
  Function:     p03csqlcaareainit

  see also:

  Description:
  Initalisiert die Meldungen f?ur den Anwender in der sqlca-area
  (sqlcode, warnings, ..)
  und initalisiert den Fehlermeldungsrecord (sqlem).

  Arguments:
    sqlca [in]

  return value: void
 */
void
p03csqlcaareainit (sqlcatype * sqlca)
{
  int             i;

  /* initialisiere sqlca */
  M90TRACE (M90_TR_ENTRY, "p03csqlcaareainit", 0);
  sqlca->sqlcode = 0;
  if (sqlca->sqlerrml != 0 || sqlca->sqlerrmc[0] == '\0') {
    sqlca->sqlerrml = 0;
    memset (&sqlca->sqlerrmc, bsp_c1, mxsp_c70);
  }
  if ((sqlca->sqldbmode == CPR_KIND_ORACLE)
      || (sqlca->sqldbmode == CPR_KIND_SAPR3)
      || (sqlca->sqlrap->ralang == CPR_LA_C))
    sqlca->sqlerrmc[0] = '\0';

  memcpy (sqlca->sqlerrp, "NOT SET ", 8);
  for (i = 0; i < 6; i++)
    sqlca->sqlerrd[i] = 0;
  if (sqlca->sqlwarn0 != bsp_c1) {
    sqlca->sqlwarn0 = bsp_c1;
    sqlca->sqlwarn1 = bsp_c1;
    sqlca->sqlwarn2 = bsp_c1;
    sqlca->sqlwarn3 = bsp_c1;
    sqlca->sqlwarn4 = bsp_c1;
    sqlca->sqlwarn5 = bsp_c1;
    sqlca->sqlwarn6 = bsp_c1;
    sqlca->sqlwarn7 = bsp_c1;
    sqlca->sqlwarn8 = bsp_c1;
    sqlca->sqlwarn9 = bsp_c1;
    sqlca->sqlwarna = bsp_c1;
    sqlca->sqlwarnb = bsp_c1;
    sqlca->sqlwarnc = bsp_c1;
    sqlca->sqlwarnd = bsp_c1;
    sqlca->sqlwarne = bsp_c1;
    sqlca->sqlwarnf = bsp_c1;
  }
  if (sqlca->sqlgap->gacmpkind != CPR_LA_ODBC)
    memset (sqlca->sqlresn, bsp_c1, sizeof(tsp00_KnlIdentifier));
  memset (sqlca->sqlstate, '0', 5);
  sqlca->sqlstate[5] = (char) 0;
  p03initsqlem (sqlca->sqlemp);
  M90TRACE (M90_TR_EXIT, "p03csqlcaareainit", 0);
}     /*--------------------------------------------*/


/*!
  Function:     p03initsqlem

  see also:

  Description:
  Initialisiert den sqlemp Record.

  Arguments:
    sqlemp [in/out]

  return value: void
 */
void
p03initsqlem (struct SQLERROR *sqlemp)
{
  M90TRACE (M90_TR_ENTRY, "p03initsqlem", 0);
  memset (sqlemp->etext, ' ', 70);
  sqlemp->etext[0] = (char) 0;
  memset (sqlemp->ewarning, ' ', 16);
  memset (sqlemp->esqlstate, '0', 5);
  sqlemp->esqlstate[5] = (char) 0;
  sqlemp->ereturncode = 0;
  sqlemp->eerrorpos = 0;
  sqlemp->etextlen = 0;
  sqlemp->eprerr = cpr_p_ok;
  sqlemp->elzu = sp1ce_ok;
  M90TRACE (M90_TR_EXIT, "p03initsqlem", 0);
}     /*--------------------------------------------*/


/*!
  Function:     p03creqrecpacket

  see also:

  Description:
  Die Procedure setzt den Messagetype f?ur INTERNAL mode richtig
  (p03setsegmode).
  Ruft p03crequest p03creceive auf und belegt den sqlemp-record,
  wenn Errors bei den Lzu-proceduren gefunden wurden.

  Arguments:

  return value: void
 */
void
p03creqrecpacket (sqlratype * sqlra, sqlgaentry * gaentry,
    struct SQLERROR *sqlemp)
{
  M90TRACE (M90_TR_ENTRY, "p03creqrecpacket", 0);
  if (sqlemp->ereturncode == 0) {
#ifdef SQLODBC
    pa09MTHandler (PA09_ADD);
#endif
    p03crequest(sqlra, gaentry, sqlemp);
    if (sqlemp->eprerr == cpr_p_ok)  {
      p03creceive(sqlra, gaentry, sqlemp);
    }
#ifdef SQLODBC
    pa09MTHandler (PA09_SUB);
#endif
    if ((sqlemp->eprerr != cpr_p_ok)
	&& (sqlemp->ereturncode == 0))
      p03cseterror(sqlemp, sqlemp->eprerr);
  }
  M90TRACE (M90_TR_EXIT, "p03creqrecpacket", 0);
}     /*--------------------------------------------*/

/*************************************************************/

/*!
  Function:     p03crequest

  see also:

  Description:
  Beim eingeschaltetem Profilein wird die Zeit gemessen (p03csqlclock).
  Beim eingeschatetem Trace wird die Anfangszeit des Kommandos genommen.
  Die Procedure  sendet einen Auftragsbuffer an den Kern.
  Setzt eventuellen Lzu_error und Lzu_errtext in sqlemp.

  Arguments:
          sqlra [in]
          gaentry [in]
          sqlemp [in]

  return value: void
 */
void
p03crequest (sqlratype * sqlra, sqlgaentry * gaentry,
    struct SQLERROR *sqlemp)
{
  tsp00_ErrText     errtext;
  sqltatype      *tap = sqlra->rasqltap;

  M90TRACE (M90_TR_ENTRY, "p03crequest", 0);
  if (sqlra->raopprof == cpr_is_only_pc)
    p03csqlclock (sqlra, cpr_is_end);
  if (tap->tatrout != CPR_TRACE_OFF) {
    if (sqlra->ranotrtimeoutp == cpr_is_false) {
      sqldattime (sqlra->rasqltap->tastartdate,
		  sqlra->rasqltap->tastarttime);
      memcpy (sqlra->rasqltap->taenddate,
	      sqlra->rasqltap->tastartdate, 8);
      memcpy (sqlra->rasqltap->taendtime,
	      sqlra->rasqltap->tastarttime, 8);
    }
  }
  if (sqlra->raprocessid == 0 || !gaentry->gareference)
    sqlemp->elzu = sp1ce_notok;
  else {
    tsp1_packet    *pck = (tsp1_packet *) gaentry->gareqptr;
    tsp1_segment_header *seghp
      = (pck) ? (tsp1_segment_header *)&pck->sp1_segm : NULL;
    tsp00_Int4        ln = (tsp00_Int4) (s26packet_len (pck));
    M90TRACE(M90_TR_UCHAR, "sp1c_mess_type", &seghp->sp1c_mess_type);
    if ( seghp->sp1c_mess_type == sp1m_parse ) {
      seghp->sp1c_parsing_again = sqlra->raparse_again;
    }
    else {
      seghp->sp1c_parsing_again = FALSE;
    }
    M90TRACE (M90_TR_BOOL, "raparse_again ",
	      &sqlra->raparse_again);
    M90TRACE (M90_TR_BOOL, "parsing again ",
	      &seghp->sp1c_parsing_again);
    M90TRACEBUF ("garequest packet ", gaentry->gareqptr, 0,
		 (int) ((ln > 2000) ? 2000 : ln));
    pck->sp1_header.sp1h_varpart_len = s26packet_len (pck) -
      sizeof (tsp1_packet_header);
    sqlra->ramesstype = (tsp00_Int2) seghp->sp1c_mess_type;
#ifdef SQLODBC
    pa09Semaphore (PA09_SEMAPHORE_SQLCANCEL, PA09_WAIT_FOR_SEMAPHORE);
#endif

    sqlarequest (gaentry->gareference, pck, ln,
		 errtext, (unsigned char*) &sqlemp->elzu);

#ifdef SQLODBC
    pa09Semaphore (PA09_SEMAPHORE_SQLCANCEL, PA09_RELEASE_SEMAPHORE);
#endif
    if (sqlemp->elzu != sp1ce_ok)
      memcpy (sqlemp->etext, errtext, 40);
  }
  if (sqlemp->elzu != sp1ce_ok)
    p03clzuerror (gaentry, CPR_LZU_REQUEST, sqlemp);
  M90TRACE (M90_TR_EXIT, "p03crequest", 0);
}     /*--------------------------------------------*/


void pr03Cancel(int session);

/*!
  Function:     p03creceive

  see also:

  Description:
  Die Procedure  wartet auf einen Auftragsbuffer vom Kern.
  Setzt eventuellen Lzu_error und Lzu_errtext in sqlemp.
  Beim eingeschaltetem Profilein wird die Zeit gemessen (p03csqlclock).
  Beim eingeschatetem Trace wird die Endzeit des Kommandos genommen.

  Arguments:
          sqlra     [in]
          gaentry [in]
          sqlemp    [in]

  return value: void
 */
void
p03creceive (sqlratype * sqlra, sqlgaentry * gaentry,
    struct SQLERROR *sqlemp)
{
  tsp00_Int4        answlen = 0;
  tsp00_ErrText     errtext;
  char           *s = "";
  int             len;

  tsp1_packet    *pck;
  sqltatype      *tap = sqlra->rasqltap;

  M90TRACE (M90_TR_ENTRY, "p03creceive", 0);
#ifdef SQLODBC
  if (pa09IsCanceled(gaentry->gareference)) {
    pa09Semaphore (PA09_SEMAPHORE_SQLCANCEL, PA09_WAIT_FOR_SEMAPHORE);
    sqlacancel(gaentry->gareference);
    pa09Semaphore (PA09_SEMAPHORE_SQLCANCEL, PA09_RELEASE_SEMAPHORE);
  }
#else
    pr03Cancel(sqlra->raactsession);
#endif
  sqlareceive (gaentry->gareference, (void **) &gaentry->garecptr,
	       &answlen, errtext, (unsigned char*) &sqlemp->elzu);

  if (sqlemp->elzu != sp1ce_ok)
    memcpy (sqlemp->etext, errtext, 40);
  p03clzuerror (gaentry, CPR_LZU_RECEIVE, sqlemp);

  pck = (tsp1_packet*) gaentry->garecptr;
  sqlra->rasegptr = (pck) ? &pck->sp1_segm : NULL;
  if (pck && answlen > 0) {
    p03partptrinit (sqlra);
    M90TRACE (M90_TR_SDWORD, "answlen   ", &answlen);
    if (answlen > 0)
      if (strncmp ((char*)&sqlra->ratermid[0],
		   (char*)&pck->sp1_header.sp1h_appl_version[0], 5) != 0) {
	sqlemp->eprerr = cpr_reflex_connect_notok;
	sqlemp->elzu = sp1ce_notok;
	s = "SERVERDB NO 6.2 RELEASE";
	len = (int)strlen (s);
	memcpy (sqlemp->etext, s, len);
      }
      else
	p03packeterror (sqlra, gaentry, sqlemp);
  }
  if (sqlra->raopprof == cpr_is_only_pc)
    p03csqlclock (sqlra, cpr_is_first);
  if (tap->tatrout != CPR_TRACE_OFF)
    if (sqlra->ranotrtimeoutp == cpr_is_false) {
      sqldattime (sqlra->rasqltap->taenddate,
		  sqlra->rasqltap->taendtime);
    }
  M90TRACE (M90_TR_EXIT, "p03creceive", 0);
}     /*--------------------------------------------*/

/*!
  Function:     p03clzuerror

  see also:

  Description:
  Die Procedure interpretiert den Lzu-error in Abh?angigkeit von der
  zuvor aufgrufenen Lzu-prog den Error und belegt den entsprechenden
  Runtimeenumerationwert in sqlemp.eprerr.

  Arguments:
          gaentry [in]
          lzu_prog [in] lzu function wich produces the error
          sqlemp    [in]

  return value: void
 */
void p03clzuerror (sqlgaentry * gaentry, int lzu_prog,
        struct SQLERROR * sqlemp)
{
  tsp1_packet    *pck = (tsp1_packet*) gaentry->garecptr;
  tsp00_TaskId      retreference;
  tsp1_segment_header *seghp
    = (pck) ? (tsp1_segment_header *)&pck->sp1_segm : NULL;


  M90TRACE (M90_TR_ENTRY, "p03clzuerror", 0);
  M90TRACE (M90_TR_SCHAR, "sqlemp->elzu    ", &sqlemp->elzu);
  sqlemp->eprerr = cpr_p_ok;
  retreference = gaentry->gareference;
  switch (sqlemp->elzu) {
  case sp1ce_ok: {
    switch (lzu_prog) {
    case CPR_LZU_RECEIVE: {
      if (seghp->sp1r_returncode == USER_PASSW_WRONG)
	sqlresult (CPR_RESULT_USER_PASSW);
      if (seghp->sp1r_returncode == DATABASE_NOT_ACCESSIBLE) {
	sqlemp->elzu = sp1ce_start_required;
	sqlemp->eprerr = cpr_reflex_shutdown;
	sqlresult (CPR_RESULT_SYSTEM_REQUIRED);
      }
      break;
    }
    default: {
      break;
    }
    }
    break;
  }
  case sp1ce_tasklimit: {
    gaentry->gareference = 0;
    sqlresult (CPR_RESULT_TOO_MANY_USERS);
    sqlemp->eprerr = cpr_too_many_users;
    break;
  }
  case sp1ce_timeout: {
    gaentry->gareference = 0;
    sqlemp->eprerr = cpr_reflex_timeout;
    break;
  }
  case sp1ce_crash: {
    if (lzu_prog == CPR_LZU_CANCEL)
      sqlemp->eprerr = cpr_r_sql_cancelled;
    else {
      gaentry->gareference = 0;
      sqlemp->eprerr = cpr_reflex_crash;
    }
    break;
  }
  case sp1ce_start_required: {
    sqlresult (CPR_RESULT_SYSTEM_REQUIRED);
    gaentry->gareference = 0;
    if (lzu_prog == CPR_LZU_CONNECT)
      sqlemp->eprerr = cpr_reflex_connect_notok;
    else
      sqlemp->eprerr = cpr_reflex_start_required;
    break;
  }
  case sp1ce_shutdown: {
    sqlemp->eprerr = cpr_reflex_shutdown;
    break;
  }
  /* PTS 1108322 */
  case sp1ce_notok: {
    sqlresult (CPR_RESULT_SYSTEM_REQUIRED);
    gaentry->gareference = 0;
    if (lzu_prog == CPR_LZU_CONNECT)
      sqlemp->eprerr = cpr_reflex_connect_notok;
    else
      sqlemp->eprerr = cpr_reflex_notok;
    break;
  }
  }
  if (sqlemp->elzu != sp1ce_ok) {
    if ((lzu_prog == CPR_LZU_REQUEST)
	|| (lzu_prog == CPR_LZU_RECEIVE))
      if (retreference == 0) {
	sqlemp->elzu = sp1ce_notok;
      }
      else {
	sqlarelease (retreference);
	gaentry->gareference = 0;
	gaentry->gareqptr = NULL;
	gaentry->garecptr = NULL;
      }
  }
  M90TRACE (M90_TR_SCHAR, "sqlemp->eprerr  ", &sqlemp->eprerr);
  M90TRACE (M90_TR_EXIT, "p03clzuerror", 0);
}     /*--------------------------------------------*/

void
p03cresultcountget (sqlratype * sqlra, sqlgaentry * gaentry,
    tsp00_Int4 * resultcount)
{
  tsp1_packet    *pck = (tsp1_packet*) gaentry->garecptr;

  M90TRACE (M90_TR_ENTRY, "p03cresultcountget", 0);
  *resultcount = 0;
  if (gaentry->garecptr != NULL) {
    p03cresultcount(sqlra, resultcount);
  }
  M90TRACE (M90_TR_EXIT, "p03cresultcountget", 0);
}     /*--------------------------------------------*/

void p03cresultcount(sqlratype *sqlra, tsp00_Int4 * resultcount)
{
    /* this function fetches the result count */
  tsp1_part      *part_ptr;
  char            res;
  M90TRACE (M90_TR_ENTRY, "p03cresultcount", 0);
  p03find_part (sqlra, (char) sp1pk_resultcount, &part_ptr);
  if (part_ptr != NULL) {
    *resultcount = 1;
    if (memcmp (part_ptr->sp1p_buf+1, CPR_NUMB1, MXPR_NUMB)) {
      s40g4int ((const char*)part_ptr->sp1p_buf, 2, resultcount, &res);
      if (res != cpr_p_ok)
	*resultcount = -1;
    }
  }
  M90TRACE (M90_TR_EXIT, "p03cresultcount", 0);
}

void
p03packeterror (sqlratype * sqlra, sqlgaentry * gaentry,
    struct SQLERROR *sqlemp)
{
  tsp1_packet    *pck = (tsp1_packet*) gaentry->garecptr;
  tsp1_segment_header *seghp
    = (pck) ? (tsp1_segment_header *)&pck->sp1_segm : NULL;
  tsp1_part      *part_ptr;
  tsp00_C2          warnset;

  M90TRACE (M90_TR_ENTRY, "p03packeterror", 0);
  if (gaentry->garecptr != NULL) {
    p03find_part (sqlra, (char) sp1pk_errortext,
		  &part_ptr);
    if ((part_ptr == NULL)   /** id 2695 **/
	&& (sqlemp->ereturncode == 0)){
      sqlemp->ereturncode = seghp->sp1r_returncode;
      sqlemp->eerrorpos = seghp->sp1r_errorpos;
    }
    if ((part_ptr != NULL)
        && (sqlemp->ereturncode == 0)) {
      tsp78ConversionResult ConvRes;
      tsp00_Uint4 srcBytesParsed;
      tsp00_Uint4 destBytesWritten;
      sqlemp->ereturncode = seghp->sp1r_returncode;
      sqlemp->eerrorpos = seghp->sp1r_errorpos;
      ConvRes = sp78convertString(sp77encodingUTF8, sqlemp->etext,
			     sizeof (sqlemp->etext), &destBytesWritten,
			     FALSE,
			     pr03PacketGetEncoding(pck), part_ptr->sp1p_buf,
			     part_ptr->sp1p_part_header.sp1p_buf_len, &srcBytesParsed);
      if (ConvRes != sp78_Ok && ConvRes != sp78_TargetExhausted) {
	sqlemp->etextlen = sp77sprintf((char*)sqlemp->etext, sizeof(sqlemp->etext)-1, "Errortext corrupted (%s)", sp78errString(ConvRes));
      }
      else {
	sqlemp->etextlen = destBytesWritten;
      }
      memcpy (&sqlemp->esqlstate, seghp->sp1r_sqlstate, 5);
      if (sqlra->rasqlansi == CPR_KIND_ANSI)
	if ((sqlemp->ereturncode == 250)
	    || (sqlemp->ereturncode == 300)
	    || (sqlemp->ereturncode == 320))
	  sqlemp->ereturncode = -sqlemp->ereturncode;
      M90TRACE (M90_TR_SWORD, "ereturncode  ", &sqlemp->ereturncode);
      M90TRACE (M90_TR_SDWORD, "eerrorpos    ", &sqlemp->eerrorpos);
      M90TRACE (M90_TR_SWORD, "etextlen     ", &sqlemp->etextlen);
      M90TRACEBUF ("etext    ", sqlemp->etext, 0, sqlemp->etextlen);
      M90TRACEBUF ("esqlstate", sqlemp->esqlstate, 0, 6);
    }
    memcpy (warnset, &seghp->sp1r_extern_warning, 2);
    p03warningset (sqlemp, warnset);
    M90TRACEBUF ("ewarning", sqlemp->ewarning, 0, 16);
  }
  M90TRACE (M90_TR_EXIT, "p03packeterror", 0);
}     /*--------------------------------------------*/

void
p03warningset (struct SQLERROR *sqlemp, tsp00_C2 warnset)
{
  M90TRACE (M90_TR_ENTRY, "p03warningset", 0);
  if ((warnset[0] != (char) 0)
      || (warnset[1] != (char) 0)) {
    sqlemp->ewarning[0] = (unsigned char) 'W';
    if (1 && (warnset[0] & 2) != 0)
      sqlemp->ewarning[1] = (unsigned char) 'W';
    if (1 && (warnset[0] & 4) != 0)
      sqlemp->ewarning[2] = (unsigned char) 'W';
    if (1 && (warnset[0] & 8) != 0)
      sqlemp->ewarning[3] = (unsigned char) 'W';
    if (1 && (warnset[0] & 16) != 0)
      sqlemp->ewarning[4] = (unsigned char) 'W';
    if (1 && (warnset[0] & 32) != 0)
      sqlemp->ewarning[5] = (unsigned char) 'W';
    if (1 && (warnset[0] & 64) != 0)
      sqlemp->ewarning[6] = (unsigned char) 'W';
    if (1 && (warnset[0] & 128) != 0)
      sqlemp->ewarning[7] = (unsigned char) 'W';
    if (1 && (warnset[1] & 1) != 0)
      sqlemp->ewarning[8] = (unsigned char) 'W';
    if (1 && (warnset[1] & 2) != 0)
      sqlemp->ewarning[9] = (unsigned char) 'W';
    if (1 && (warnset[1] & 4) != 0)
      sqlemp->ewarning[10] = (unsigned char) 'W';
    if (1 && (warnset[1] & 8) != 0)
      sqlemp->ewarning[11] = (unsigned char) 'W';
    if (1 && (warnset[1] & 16) != 0)
      sqlemp->ewarning[12] = (unsigned char) 'W';
    if (1 && (warnset[1] & 32) != 0)
      sqlemp->ewarning[13] = (unsigned char) 'W';
    if (1 && (warnset[1] & 64) != 0)
      sqlemp->ewarning[14] = (unsigned char) 'W';
    if (1 && (warnset[1] & 128) != 0)
      sqlemp->ewarning[15] = (unsigned char) 'W';
  }
  M90TRACE (M90_TR_EXIT, "p03warningset", 0);
}     /*--------------------------------------------*/

/*!
  Function:     p03setsegmode

  see also:

  Description:
  rakamod auf sp1c_sqlmode setzen
  L?auft eine Anwendung in einem anderen Mode als INTERNAL, so k?onnen
  mit 'exec internal' Statements angegeben werden die im INTERNAL mode
  laufen sollen. Dazu muss der mess_type im Auftragsegment an den
  Kern ge?andert werden. Diese Procedure setzt den kamode in sp1c_sqlmode.

  In sqlra.radbmode steht der Mode der Anwendung. In sqlra.rakamode
  steht der Mode des Kommandos.

  Arguments:
          sqlra [in]
          gaentry [in]

  return value: void
 */
void
p03setsegmode (sqlratype * sqlra, sqlgaentry * gaentry)
{
  tsp1_packet    *pck;

  M90TRACE (M90_TR_ENTRY, "p03setsegmode", 0);
    pck = (tsp1_packet *) gaentry->gareqptr;

  {
    tsp1_segment_header *seghp
      = (pck) ? (tsp1_segment_header *)&pck->sp1_segm : NULL;

    /*  historical code fragment disabled, because problem during unicode enhancement */
    /*  pck->sp1_header.sp1h_mess_code = gaentry->gamesscode[0];       */
    pck->sp1_header.sp1h_mess_swap = gaentry->gamesscode[1];

    /* rakamod auf sp1c_sqlmode setzen */
    switch (sqlra->rakamode)
      {
      case CPR_KIND_EMPTY:
      case CPR_KIND_INTERNAL:
	seghp->sp1c_sqlmode = (char) sp1sm_internal;
	if (sqlra->rasqlansi == CPR_KIND_ANSI)
	  seghp->sp1c_sqlmode = (char) sp1sm_ansi;
	break;
      case CPR_KIND_DB2:
	seghp->sp1c_sqlmode = (char) sp1sm_db2;
	break;
      case CPR_KIND_ORACLE:
      case CPR_KIND_SAPR3:
	seghp->sp1c_sqlmode = (char) sp1sm_oracle;
	break;
      default:
	break;
      }
  }
  M90TRACE (M90_TR_EXIT, "p03setsegmode", 0);
}     /*--------------------------------------------*/

void
p03ctraceerror (sqlratype * sqlrap, struct SQLERROR *sqlemp)
{

  sqltatype      *tap = sqlrap->rasqltap;

  M90TRACE (M90_TR_ENTRY, "p03ctraceerror", 0);
  if (sqlemp->ereturncode != 0) {
  }
  M90TRACE (M90_TR_EXIT, "p03ctraceerror", 0);
}     /*--------------------------------------------*/

/*!
  Function:     p03cseterror

  see also:

  Description:
  Setzt den tpr_sqlerror um in einen ereturncode mit entsprechendem
  Errortext, und beleget im tsp_error_msg_type den
  ereturncode, ewarning, etext und eprerr. In errlen wird die L?ange des etext
  zur?uckgegeben.

  Arguments:
          sqlemp [in]
          error [in]

  return value: void
 */

void
p03cseterror (struct SQLERROR *sqlemp,
    tpr_runtime_errors_Enum error)
{
  char           *s = "";
  int             len;
  int             lent;
  char            chr12[12];

  M90TRACE (M90_TR_ENTRY, "p03cseterror", 0);
  if (sqlemp->ereturncode == 0) {
    sqlemp->eprerr = (char) error;
    switch (error) {
    case cpr_p_ok:
      sqlemp->ereturncode = 0;
      break;
    case cpr_r_sql_cancelled:
      sqlemp->ereturncode = -102;
      s = "SQL statement cancelled";
      break;
    case cpr_inp_default_not_allowed:
      sqlemp->ereturncode = -822;
      s = "No default value available";
      break;
    case cpr_inp_null_not_allowed:
      sqlemp->ereturncode = -801;
      s = "Input parameter cannot be NULL value";
      break;
    case cpr_inp_number_invalid:
      sqlemp->ereturncode = -802;
      s = "Invalid numeric input parameter value";
      break;
    case cpr_inp_number_overflow:
      sqlemp->ereturncode = -751;
      s = "Numeric input parameter overflow";
      break;
    case cpr_inp_number_truncated:
      sqlemp->eprerr = cpr_p_ok;
      sqlemp->ereturncode = 0;
      sqlemp->ewarning[0] = 'W';
      sqlemp->ewarning[1] = 'W';
      break;
    case cpr_invalid_commandstate:
      p03DebugBreak();
      sqlemp->ereturncode = -9801;
      s = "Invalid command state";
      break;
    case cpr_invalid_program:
      sqlemp->ereturncode = -805;
      s = "Program must be precompiled";
      break;
    case cpr_db_session_not_allowed:
      sqlemp->ereturncode = -806;
      s = "DB session number not allowed";
      break;
    case cpr_no_session_connected:
    case cpr_reflex_notok:
      sqlemp->ereturncode = -807;
      s = "Connection down, session released";
      break;
    case cpr_not_implemented:
      sqlemp->ereturncode = -808;
      s = "Not yet implemented";
      break;
    case cpr_out_null_not_allowed:
      sqlemp->ereturncode = -809;
      s = "Missing indicator, output param with NULL value";
      break;
    case cpr_out_number_invalid:
      sqlemp->ereturncode = -9806;
      s = "Invalid output value";
      break;
    case cpr_out_number_overflow:
      sqlemp->ereturncode = -811;
      s = "Numeric output parameter overflow";
      break;
    case cpr_output_part_too_short:
      sqlemp->eprerr = cpr_p_ok;
      sqlemp->ereturncode = 0;
      sqlemp->ewarning[0] = 'W';
      sqlemp->ewarning[3] = 'W';
      break;
    case cpr_paramlist_too_long:
      sqlemp->ereturncode = -804;
      s = "Parameter list too long";
      break;
    case cpr_paramlist_too_short:
      sqlemp->ereturncode = -812;
      s = "Parameter list too short";
      break;
    case cpr_paramlist_wrong:
      sqlemp->ereturncode = -883;
      s = "Invalid runtime options";
      break;
    case cpr_rmissing_indicatorvariable:
      sqlemp->ereturncode = -884;
      s = "Missing indicator variable";
      break;
    case cpr_reflex_start_required:
      sqlemp->ereturncode = -813;
      s = "SERVERDB must be restarted";
      break;
    case cpr_reflex_timeout:
      sqlemp->ereturncode = +700;
      s = "Command inactivity timeout (work rolled back)";
      break;
    case cpr_reflex_crash:
      sqlemp->ereturncode = +800;
      s = "Implicit SERVERDB restart";
      break;
    case cpr_cmd_too_long:
    case cpr_request_area_overflow:
      sqlemp->ereturncode = -706;
      s = "SQL statement too long";
      break;
    case cpr_tracefile_close_error:
      sqlemp->ereturncode = -814;
      s = "Trace file close error";
      break;
    case cpr_tracefile_open_error:
      sqlemp->ereturncode = -815;
      s = "Trace file open error";
      break;
    case cpr_tracefile_write_error:
      sqlemp->ereturncode = -816;
      s = "Trace file write error";
      break;
    case cpr_trace_error:
      sqlemp->ereturncode = -803;
      s = "Trace error";
      break;
    case cpr_incompatible_datatype:
      sqlemp->ereturncode = -817;
      s = "Incompatible data types";
      break;
    case cpr_unknown_datatype:
      sqlemp->ereturncode = -9803;
      s = "Unknown data type";
      break;
    case cpr_unknown_statement_name:
      sqlemp->ereturncode = -880;
      s = "Unknown statement name";
      break;
    case cpr_unknown_cursor_name:
      sqlemp->ereturncode = -874;
      s = "Unknown cursor name";
      break;
    case cpr_area_overflow:
      sqlemp->ereturncode = -761;
      s = "AREA SIZE IS TOO SMALL";
      break;
    case cpr_memory_allocation_faild:
      sqlemp->ereturncode = -760;
      s = "Memory allocation failed";
      break;
    case cpr_too_many_hostvar:
      sqlemp->ereturncode = -893;
      s = "Too many hostvariables";
      break;
    case cpr_too_many_hostvar_callif:
      sqlemp->ereturncode = -712;
      s = "Too many parameters, SQLMAX too small";
      break;
    case cpr_macro_not_init:
      sqlemp->ereturncode = -818;
      s = "Missing macro parameter initialization";
      break;
    case cpr_paramlist_not_allowed:
      sqlemp->ereturncode = -819;
      s = "Parameter in SQL statement not allowed";
      break;
    case cpr_invalid_declare_command:
      sqlemp->ereturncode = -820;
      s = "Invalid declare statement";
      break;
    case cpr_r_loop_init_wrong:
      sqlemp->ereturncode = -856;
      s = "Invalid loop parameter value";
      break;
    case cpr_missing_variable_addr:
      sqlemp->ereturncode = -702;
      s = "Missing variable address in SQLDA";
      break;
    case cpr_duplicate_fetch:
      sqlemp->ereturncode = -703;
      s = "Duplicate FETCH statem.with MFETCH-opt not allowed";
      break;
    case cpr_duplicate_fetch_callif:
      sqlemp->ereturncode = -703;
      s = "Duplicate FFETCH statement for same SELECT";
      break;
    case cpr_too_many_fetch_statements:
      sqlemp->ereturncode = -705;
      s = "Too many FFETCH statements";
      break;
    case cpr_connect_syntax_wrong:
      sqlemp->ereturncode = -707;
      s = "Invalid CONNECT statement syntax";
      break;
    case cpr_reflex_shutdown:
      sqlemp->ereturncode = -708;
      s = "SERVERDB system not available";
      break;
    case cpr_reflex_connect_notok:
      sqlemp->ereturncode = -709;
      s = "CONNECT:";
      break;
    case cpr_select_parsid_missing:
      p03DebugBreak();
      sqlemp->ereturncode = -710;
      s = "Missing SELECT PARSID";
      break;
    case cpr_implicit_connect_wrong:
      sqlemp->ereturncode = -711;
      s = "Implicit CONNECT : missing user or SERVERDB";
      break;
    case cpr_missing_user_passw:
      sqlemp->ereturncode = -715;
      s = "Missing username or password for CONNECT";
      break;
    case cpr_unknown_sysprofile:
      sqlemp->ereturncode = -719;
      s = "Unknown LOCALSYSUSER.SYSPROFILE table";
      break;
    case cpr_errorno_from_sysprofile:
      /*  p05inttochr12 (sqlemp->ereturncode, chr12); */
      len = sprintf (chr12, "%d", sqlemp->ereturncode);
      sqlemp->ereturncode = -720;
      s = "SQLCODE         comes from SYSPROFILE command";
      memcpy (&s[8], chr12, len);
      break;
    case cpr_inp_string_truncated:
      sqlemp->ereturncode = -743;
      s = "Input string too long";
      break;
    case cpr_inp_string_truncated_oracle:
      sqlemp->eprerr = cpr_p_ok;
      sqlemp->ereturncode = 0;
      sqlemp->ewarning[0] = 'W';
      sqlemp->ewarning[1] = 'W';
      break;
    case cpr_unterminated_c_string:
      sqlemp->ereturncode = -767;
      s = "Unterminated C string";
      break;
    case cpr_invalid_escape:
      sqlemp->ereturncode = -768;
      s = "Invalid escape value";
      break;
    case cpr_multibyteset_not_allowed:
      sqlemp->ereturncode = -772;
      s = "Multibyte char set not allowed";
      break;
    case cpr_datafile_open_error:
      sqlemp->ereturncode = -773;
      s = "Datafile open error";
      break;
    case cpr_datafile_write_error:
      sqlemp->ereturncode = -774;
      s = "Datafile write error";
      break;
    case cpr_datafile_read_error:
      sqlemp->ereturncode = -775;
      s = "Datafile read error";
      break;
    case cpr_datafile_close_error:
      sqlemp->ereturncode = -776;
      s = "Datafile close error";
      break;
    case cpr_conv_from_unicode_impossible:
      sqlemp->ereturncode = -777;
      s = "Conversion from UNICODE impossible";
      break;
    case cpr_conv_to_unicode_impossible:
      sqlemp->ereturncode = -778;
      s = "Conversion to UNICODE impossible";
      break;
    case cpr_no_long_columns_found:
      sqlemp->ereturncode = -779;
      s = "PUTVAL: No long columns found";
      break;
    case cpr_use_call_stm:
      sqlemp->ereturncode = -744;
      s = "Use procedure sqcstatement";
      break;
    case cpr_valbuf_too_short:
      sqlemp->ereturncode = -747;
      s = "Value buffer is too short";
      break;
    case cpr_ffetch_not_allowed:
      sqlemp->ereturncode = -749;
      s = "FFETCH not allowed for this select";
      break;
    case cpr_ffetch_fetch_mix_not_allowed:
      sqlemp->ereturncode = -750;
      s = "ARRAY-FETCH and FETCH mix not allowed";
      break;
    case cpr_session_already_connected:
      sqlemp->ereturncode = -752;
      s = "Session already connected";
      break;
    case cpr_rt_no_output_variable:
      sqlemp->ereturncode = -753;
      s = "Output parameter with CONST type";
      break;
    case cpr_differ_sqlmode_not_allowed:
      sqlemp->ereturncode = -756;
      s = "Different SQLMODEs not allowed";
      break;
    case cpr_missing_previous_fetch:
      sqlemp->ereturncode = -7026;
      s = "SQL statement not allowed without prev. FETCH";
      break;
    case cpr_r_too_many_statements:
      sqlemp->ereturncode = -868;
      s = "Too many SQL statements";
      break;
    case cpr_too_many_users:
      sqlemp->ereturncode = -9807;
      s = "Too many users";
      break;
    case cpr_set_format_error:
      sqlemp->ereturncode = -9808;
      s = "SET FORMAT error";
      break;
    case cpr_xuser_file_open_error:
      sqlemp->ereturncode = -757;
      s = "XUSER error";
      break;
    case cpr_cmd_not_available:
      /*-101 */
      sqlemp->ereturncode = CPR_ERR_CMD_NOT_AVAILABLE;
      s = "Command not available in this mode";
      break;
    case cpr_abap_desc_not_set: {
      sqlemp->ereturncode = -861;
      s = "ABAP descriptor not set";
      break;
    }
    case cpr_abap_exception: {
      sqlemp->ereturncode = -862;
      s = "ABAP exception ";
      break;
    }
    case cpr_invalid_option: {
      sqlemp->ereturncode = cpr_error_invalid_option;
      s = "Invalid option";
      break;
    }
    case cpr_session_not_connected: {
      sqlemp->ereturncode = cpr_error_session_not_connected;
      s = "Session not connected";
      break;
    }
    case cpr_connection_busy: {
      sqlemp->ereturncode = CPR_ERROR_CONNECTION_BUSY;
      s = "Connection already in use";
      break;
    }
    case cpr_could_not_load_lib: {
      sqlemp->ereturncode = -888;
      s = "Could not load libpcr";
      break;
    }
    default: {
      static char tmp[30];
      sqlemp->ereturncode = -899;
      sprintf(tmp, "Error unknown: %d", error);
      s = tmp;
      break;
    }
    }
    len = (int)strlen (s);

    sqlemp->etextlen = (tsp00_Int2) len;
    if (sqlemp->elzu != sp1ce_ok && len > 0) {
      char  lzutext [70];

      lent = s30lnr1 (sqlemp->etext, bsp_c1, 1, mxsp_c70);
      if (lent > mxsp_c70 - len - 3)
	lent = mxsp_c70 - len - 3;
      memcpy (&lzutext, sqlemp->etext, lent);
      sqlemp->etext[len + 0] = ' ';
      sqlemp->etext[len + 1] = '(';
      memcpy (&(sqlemp->etext[len + 2]), lzutext, lent);
      sqlemp->etext[len + 2 + lent] = ')';
      sqlemp->etextlen = (tsp00_Int2) (len + 3 + lent);
    }
    memcpy (sqlemp->etext, s, len);
    M90TRACE (M90_TR_SWORD, "ereturncode  ", &sqlemp->ereturncode);
    M90TRACE (M90_TR_SWORD, "etextlen  ", &sqlemp->etextlen);
    M90TRACEBUF (" etext", &sqlemp->etext, 0, (len == 0) ? 6 :
		 sqlemp->etextlen );
    M90TRACEBUF (" ewarn", &sqlemp->ewarning, 0, 16);
  }
  M90TRACE (M90_TR_EXIT, "p03cseterror", 0);
}     /*--------------------------------------------*/


/*!
  Function:     p03csqlemptosqlca

  see also:

  Description:
  Schreibt, wenn ereturncode bzw. ewarning != 0,
  den tpr_sqlerror Record in den sqlca-area.

  Arguments:
          sqlcap [in/out]
          sqlemp [in]

  return value: void
 */
void
p03csqlemptosqlca (sqlcatype * sqlcap, struct SQLERROR *sqlemp)
{

  M90TRACE (M90_TR_ENTRY, "p03csqlemptosqlca", 0);
  if (sqlemp->ewarning[0] != bsp_c1)
    memcpy (&sqlcap->sqlwarn0, sqlemp->ewarning, 16);
  if ((sqlemp->ereturncode != 0) || (sqlemp->etextlen != 0)) {
    int             len;

    sqlcap->sqlcode = sqlemp->ereturncode;
    sqlcap->sqlerrd[5] = sqlemp->eerrorpos;
    sqlemp->etextlen = s30lnr1 (sqlemp->etext, bsp_c1, 1,
				sqlemp->etextlen);
    sqlcap->sqlerrml = sqlemp->etextlen;
    if (sqlemp->etextlen >= mxsp_c70)
      len = mxsp_c70 - 1;
    else
      len = sqlemp->etextlen;
    memcpy (sqlcap->sqlerrmc, sqlemp->etext, len);
    if (sqlcap->sqlrap->ralang == CPR_LA_C)
      sqlcap->sqlerrmc[len] = '\0';
    memcpy (sqlcap->sqlstate, sqlemp->esqlstate,
	    sizeof (sqlemp->esqlstate));
  }
  M90TRACE (M90_TR_EXIT, "p03csqlemptosqlca", 0);
}     /*--------------------------------------------*/

/*!
  Function:     p03sqlrelease

  see also:

  Description:
  Diese Procedure wird aufgerufen, wenn das Programm beendet
  werden soll.
  L?ost die Verbindung der sqlrap->raactsession zum Kern.
  Sieht nach, ob noch andere Verbindungen zum Kern existieren, wenn
  nein wird beim Runtimelauf f?ur Betriebsysteme Unix, Win32 und Os2
  und nicht Termid "ODB" das Progemm mit sqlfinish (false) beendet.

  Arguments:
          sqlrap [in]
          sqlga [in]
          gaen [in]
          sqlemp [in]

  return value: void
 */
void
p03sqlrelease (sqlratype * sqlrap, sqlgatype * sqlga, sqlgaentry     *gaen,
    struct SQLERROR *sqlemp)
{
#define TERMID_ODBC   "ODB"

    M90TRACE (M90_TR_ENTRY, "p03sqlrelease", 0);
    sqlemp->elzu = sp1ce_ok;
#ifdef SQLODBC
    pa09MTHandler (PA09_ADD);
#endif
    if (gaen->gareference == 0) {
      sqlemp->elzu = sp1ce_notok;
      p03clzuerror (gaen, CPR_LZU_RELEASE, sqlemp);
    }
    else {
      sqlarelease (gaen->gareference);
      gaen->gareqptr = NULL;
      gaen->garecptr = NULL;
    }
#ifdef SQLODBC
    pa09MTHandler (PA09_SUB);
#endif
    if (sqlemp->eprerr != cpr_p_ok)
      p03cseterror (sqlemp, sqlemp->eprerr);
    gaen->gareference = 0;
    p03NologSession(sqlga, cpr_is_false);
#ifndef SQLODBC
    if (sqlga->gaConnection) {
      tpr01_ConContainer *Connection = sqlga->gaConnection;
      if (Connection->GetCount(Connection) == 1) {
	if ((sqlrap->racomponent[0] == 'R')) {
	  if ((sqlrap->rasqlos == os_unix)
	      || (sqlrap->rasqlos == os_os2)
	      || (sqlrap->rasqlos == os_win32)) {
	    sqlfinish (FALSE);
	  }
	  sqlrap->rasqlinit = cpr_is_false;
	}
      }
    }
    else {
      tsp00_Int2 found = 0;
      int      i;
      for (i = 1; i <= MXPR_SQLGA; i++) {
	if (sqlga->gaentry[i-1] != 0 &&
	    sqlga->gaentry[i-1]->gareference != 0)
	  found += 1;
      }
      if ((found == 0)
	  && (sqlrap->racomponent[0] == 'R')) {
        if ((sqlrap->rasqlos == os_unix)
            || (sqlrap->rasqlos == os_os2)
            || (sqlrap->rasqlos == os_win32)) {
	  sqlfinish (FALSE);
	}
        sqlrap->rasqlinit = cpr_is_false;
      }
    }
#endif
    M90TRACE (M90_TR_EXIT, "p03sqlrelease", 0);
}     /*--------------------------------------------*/

/*!
  Function:     p03csqlclock

  see also:

  Description:
  Ist das Profiling eingeschaltet, werden mit dieser Procedure
  Zwischenzeiten genommen und ackumuliert nach sqlra.raprofsec
  gebracht.

  Arguments:
    sqlra [in]
    cmd   [in]

  return value: void
 */
void
p03csqlclock (sqlratype * sqlra, long cmd)
{
    tsp00_Int4        rsec;

    M90TRACE (M90_TR_ENTRY, "p03csqlclock", 0);
    if ((sqlra->rasqlos == os_unix)
        || (sqlra->rasqlos == os_os2)
        || (sqlra->rasqlos == os_win32))
        {
        tsp00_Int4        sec, msec;

        sqlclock (&sec, &msec);
        msec = (msec + 500) / 1000;
        rsec = (sec * 1000) + msec;
        }
    else
        {
        rsec = clock ();
        rsec = (rsec + 500) / 1000;
        }
    /**printf (" cmd = %d, rsec = %d, raprofsec = " , cmd, rsec);**/
    switch (cmd)
        {
        case cpr_is_init:
            sqlra->raprofsec = -rsec;
            break;
        case cpr_is_first:
            sqlra->raprofsec -= rsec;
            break;
        case cpr_is_end:
            sqlra->raprofsec += rsec;
            break;
        }
    /** printf ("%d \n" ,sqlra->raprofsec);  **/
    M90TRACE (M90_TR_EXIT, "p03csqlclock", 0);
    M90TRACE (M90_TR_SDWORD, "cmd       ", &cmd);
    M90TRACE (M90_TR_SDWORD, "rsec      ", &rsec);
    M90TRACE (M90_TR_SDWORD, "raprofsec ", &sqlra->raprofsec);
}     /*--------------------------------------------*/

void
pr03cPutResultName(tpr01_ConDesc *ConDesc,
                   tsp00_KnlIdentifier VAR_ARRAY_REF sqlresn,
                   struct SQLERROR *sqlemp)
{
  sqlratype * sqlrap = ConDesc->sqlca->sqlrap;
  tpr_runtime_errors_Enum Err;
  M90TRACE(M90_TR_ENTRY, "pr03cPutResultName", 0);
  if (sqlresn[0] && memcmp(sqlresn, bsp_c64, sizeof(tsp00_KnlIdentifier))) {
    tsp1_part *partPtr;
    tsp00_Uint4 cbCharLen;
    tsp00_Uint4 cbCursorLen;
    int isTerminated;
    int isCorrupted;
    int isExhausted;
    sp77encodingUTF8->stringInfo(sqlresn, sizeof(tsp00_KnlIdentifier), TRUE,
                                 &cbCharLen, &cbCursorLen, &isTerminated,
                                 &isCorrupted, &isExhausted);
    partPtr = pr03PartFind(sqlrap, sp1pk_resulttablename);
    if (partPtr == NULL) {
      partPtr = pr03SegmentAddPart(ConDesc->SegmDesc, sp1pk_resulttablename);
    }
    if (partPtr) {
      tsp00_Int4 cbPartLen = pr03PartGetFreePartSpace(partPtr);
      tsp00_Int4 Offset = 0;
      tpr05_StringEncoding PacketEncoding = pr03PacketGetEncoding(ConDesc->ga->gareqptr);
      Err = pr03PartConverttoPart(partPtr, &Offset, &cbPartLen,
                                  PacketEncoding,
                                  sqlresn, cbCursorLen, sp77encodingUTF8);
      pr03SegmentFinishPart(ConDesc->SegmDesc);
    }
  }
  M90TRACE (M90_TR_EXIT, "pr03cPutResultName", 0);
}     /*--------------------------------------------*/

void pr03cRelease(tpr01_ConDesc *ConDesc)
{
#ifdef SQLODBC
  static char szReleaseStr[] = "ROLLBACK WORK RELEASE";
#else
  static char szReleaseStr[] = "COMMIT WORK RELEASE";
#endif
  sqlcatype *sqlca = ConDesc->sqlca;
  sqlxatype *sqlxa = ConDesc->sqlxa;
  ConDesc->Connection->InitPacket(ConDesc, sp77encodingAscii, sp1m_dbs);
  if (sqlca->sqlcode == 0) {
    tpr03_SegmDesc *SegmDesc = ConDesc->SegmDesc;
    tsp1_part *partPtr = pr03SegmentAddPart(SegmDesc, sp1pk_command);
    if (partPtr) {
      tpr_runtime_errors_Enum Err;
      void *rawPtr = pr03PartGetRawPtr(partPtr);
      tsp00_Int4 cbPartLen = pr03PartGetFreePartSpace(partPtr);
      tsp00_Int4 Offset = 0;
      Err = pr03PartConverttoPart(partPtr, &Offset, &cbPartLen, sp77encodingAscii, szReleaseStr, sizeof(szReleaseStr)-1, sp77encodingAscii);
      pr03SegmentFinishPart(SegmDesc);
      pr03PacketReqRec(ConDesc, sqlca->sqlemp);
      p03sqlrelease(sqlca->sqlrap, sqlca->sqlgap, ConDesc->ga, sqlca->sqlemp);

    }
    else {
      p08runtimeerror (sqlca, sqlxa, cpr_area_overflow);
    }
  }
}

void
p03cpparsid(sqlratype * sqlrap, sqlgaentry * gaent, tpr00_ParseId VAR_ARRAY_REF parsid, struct SQLERROR *sqlemp)
{
  M90TRACE(M90_TR_ENTRY, "p03cpparsid", 0);
  p03cputpart(sqlrap, gaent, sp1pk_parsid, parsid, ParseIdKnl_mxpr00,
	      sqlemp);
  M90TRACE (M90_TR_EXIT, "p03cpparsid", 0);
}     /*--------------------------------------------*/

void
p03cputpart(sqlratype * sqlrap, sqlgaentry * gaent, char part_kind,
	    void *ptr, tsp00_Int4 len, struct SQLERROR *sqlemp)
{
  tsp1_part_ptr part_ptr;
  tsp1_packet    *pck = NULL;
  M90TRACE(M90_TR_ENTRY, "p03cputpart", 0);
  if (gaent && gaent->gareqptr)
    pck = gaent->gareqptr;
  if (pck) {
    if(s26size_new_part(pck, sqlrap->rasegptr) > len)
      s26new_part_init (pck, sqlrap->rasegptr, &part_ptr);
    else
      p03cseterror (sqlemp, cpr_request_area_overflow);      
    if (part_ptr) {
      part_ptr->sp1p_part_header.sp1p_part_kind = part_kind;
      sqlrap->rasegmpartptr [part_kind] = part_ptr;
      if (ptr && len > 0) {
	memcpy(part_ptr->sp1p_buf, ptr, len);
      }
      else
	len = 0;
      part_ptr->sp1p_part_header.sp1p_buf_len = len;
      s26finish_part (pck, part_ptr);
    }
    else
      if (sqlemp->eprerr != cpr_p_ok)
	p03cseterror (sqlemp, cpr_reflex_start_required);
    if (part_kind == sp1pk_command)
      pr03PacketSetEncoding(pck, sp77encodingAscii);
  }
  else
    if (sqlemp->eprerr != cpr_p_ok)
      p03cseterror (sqlemp, cpr_reflex_start_required);

  M90TRACE (M90_TR_EXIT, "p03cputpart", 0);
}     /*--------------------------------------------*/

void p03getpidtrace (sqlhostname_t  pidtracenam)
{
   tsp00_Int4 pid;
   int strl;

   M90TRACE (M90_TR_ENTRY, "p03getpidtrace", 0);
   strcpy ((char*)pidtracenam, "pid");
   p03getpid (&pid);
   M90TRACE (M90_TR_SDWORD, "pid       ", &pid );
   sprintf ((char*)&pidtracenam[strlen((char*)pidtracenam)], "%d", pid);
   strcat ((char*)pidtracenam, ".pct");
   strl = (int)strlen((char*)pidtracenam);
   memset (&pidtracenam[strl], ' ', sizeof (sqlhostname_t) - strl);
   M90TRACE (M90_TR_EXIT, "p03getpidtrace", 0);
   M90TRACE(M90_TR_STRING, "pidtracenam", pidtracenam);
}     /*--------------------------------------------*/

void p03getpid   (tsp00_Int4 *pid)
{
  sqlgetpid (pid);

}     /*--------------------------------------------*/


void p03NologSession(sqlgatype * sqlga, tsp00_Int2 ganolog)
{
  tsp00_Int2 i;
  M90TRACE (M90_TR_ENTRY, "p03NologSession", 0);
  if (sqlga->gaConnection) {
    tpr01_ConContainer *Connection = sqlga->gaConnection;
    tpr01_ConDesc *ConDesc;
    Connection->OpenCont(Connection);
    Connection->NoLog = ganolog;
    while(ConDesc = Connection->EnumDesc(Connection)) {
      if (Connection->IsConnected(ConDesc)) {
	ConDesc->ga->ganolog = ganolog;
      }
    }
  }
  else {
    sqlgaentry *gaen = sqlga->gaentry[0];
    for(i=0;i<sqlgamax;i++)   {
      gaen=sqlga->gaentry[i];
      if (gaen != NULL)
	gaen->ganolog = ( gaen->gareference ) ? gaen->ganolog : ganolog;
    }
  }
  M90TRACE (M90_TR_EXIT, "p03NologSession", 0);
}

void pr03cSetErrorExt(struct SQLERROR *sqlemp, tpr_runtime_errors_Enum error, char* szErrExt)
{
  sqlemp->elzu = sp1ce_notok;
  pr07C2P(sqlemp->etext, szErrExt, sizeof(sqlemp->etext));
  p03cseterror (sqlemp, error);
}

extern tpr05_StringEncoding pr04cPacketEncoding;

tpr05_StringEncoding pr03cGetPacketEncoding()
{
#ifdef SQLODBC
    const tsp77encoding * pa01GetPacketEncoding ();
    return pa01GetPacketEncoding();
#else
  return pr04cPacketEncoding;
#endif
}

/*#define DEBUGBREAK*/
void p03DebugBreak()
{
#ifdef DEBUGBREAK
  int ret = IDYES;
  ret = MessageBox(0, "DebugBreak ?", "pcr62mt", MB_ICONEXCLAMATION | MB_YESNO);
  if (ret == IDYES)
    DebugBreak();
#endif
}
