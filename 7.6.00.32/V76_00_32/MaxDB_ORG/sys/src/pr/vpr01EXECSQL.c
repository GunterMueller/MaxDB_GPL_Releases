/*!================================================================
 module:    vpi01EXECSQL.c

 responsible: BurkhardD

 special area:   CPCDrv | Client | EXEC SQL Verarbeitung

 description: EXEC SQL Verarbeitung

 see:

 change history: 
         2001-11-08 D025086 Cancel session bugfix PTS 1112515 and PASCAL 
                            port from p01cancel,
         2001-09-04 d025086 No implicite connect. Disable by option.
         2001-05-08 d025086 (PTS 1110208) initialize some variables
         2000-11-16 d025086 (PTS 1108288) pr01EXECSQLVersion 
                            porting from PASCAL
         2000-08-07  17:17 Marco Paskamp  Unicode extensions



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

#ifndef __VPR01_EXECSQL_H__
#define __VPR01_EXECSQL_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr03.h"
#include "gpr01.h"
#include "gpr08.h"
#include "vpr07c.h"
#include "vpr01SQL.h"
#include "vpr01Con.h"
#include "vpr03CheckPoint.h"
#include "vpr01Trace.h"
#include "vpr01EXECSQL.h"
#include <ctype.h>

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

static tsp00_Int4 pr01GetProfileString(char *lpszSection, char *lpszEntry, char *lpszDefault, char *lpszReturnBuffer, tsp00_Int4 cbReturnBuffer, char *szProfileStr, tsp00_Int4 cbLenProfileStr);
static void pr01Put2Upper( char * string_ptr );
static char *pr01CompareStringUntil(char *szSource, char *szString, char UntilChar);

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

tsp00_Int4 pr03PartGetBuf(sqlratype * sqlra, tsp1_part_kind PartKind, tsp00_Uint1 **pBuf)
{
  tsp1_part      *part_ptr;
  p03find_part (sqlra, PartKind, &part_ptr);
  if (part_ptr) {
    *pBuf = part_ptr->variant.C_1.sp1p_buf_F;
    return part_ptr->variant.C_1.sp1p_part_header_F.sp1p_buf_len;
  }
  else {
    *pBuf = NULL;
    return 0;
  }
}

void pr01EXECSQLOptions(tpr01_SQLDesc *SQLDesc)
{
  tpr01_SQLContainer *SQL = SQLDesc->SQL;
  sqlcatype *sqlca = SQL->GetSqlca(SQLDesc);
  sqlxatype *sqlxa = SQL->GetSqlxa(SQLDesc);
  sqlratype    *sqlra = sqlca->sqlrap;
  sqlkaentry *ka   = SQLDesc->ka;
  sqltatype  *sqlta = sqlra->rasqltap;
  sqlgaentry *ga   = SQLDesc->ConDesc->ga;
    struct SQLERROR *sqlemp = sqlca->sqlemp;
  char *szCommandBuffer;
  tpr05_String *pCommand = 0;
  tsp00_Int4 cbLen;
  sqlra->rakamode = ka->kamode;
  p03csqlcaareainit (sqlca);
  p01xtracefilecheck(sqlca, sqlxa);
  SQL->GetSQL(SQLDesc, &pCommand, Reference_epr01);
  cbLen = pCommand->cbLen;
  szCommandBuffer = pCommand->rawString;
  if ( sqlta->tatrout != cpr_trace_off ) {
    p03cmdtrace(sqlra, ga,1, cpr_com_option, pCommand);
  }
  if (szCommandBuffer && cbLen) {
    tsp00_C256 szOption;
    /* Option CONNECT */
    pr01GetProfileString(NULL, "CONNECT", "", (char*)szOption, sizeof(szOption), pCommand->rawString, pCommand->cbLen);
    if (szOption[0] != '\0') {
      pr01Put2Upper( (char*)szOption );
      if (!SQLDesc->ConDesc->Connection->OldStyle) {
        if (!strcmp((char*)szOption, "OLDSTYLE")) {
          BOOLEAN OldStyle=TRUE;
          SQLDesc->ConDesc->Connection->SetAttr(SQLDesc->ConDesc, OldStyleConnect_epr01Con, (void*)&OldStyle, 0);
        } 
        else {
          p08runtimeerror (sqlca, sqlxa, cpr_invalid_option);
          return;
        }
      }
      else if (!strcmp((char*)szOption, "EXPLICIT")) {
        sqlca->sqlgap->gasqlconntyp = cpr_ci_explicit;
      }
      else if (!strcmp((char*)szOption, "IMPLICIT")) {
        sqlca->sqlgap->gasqlconntyp = cpr_ci_implicit;
      }
      else if (!strcmp((char*)szOption, "ORACLE")) {
        sqlca->sqlgap->gasqlconntyp = cpr_ci_ora_connect;
      }
      else {
        p08runtimeerror (sqlca, sqlxa, cpr_invalid_option);
      }
      return;
    }
    /* Option CHECKPOINT */
    pr01GetProfileString(NULL, "CHECKPOINT", "", (char*)szOption, sizeof(szOption), pCommand->rawString, pCommand->cbLen);
    if (szOption[0] != '\0') {
      pr01Put2Upper( (char*)szOption );
      if (!strcmp((char*)szOption, "NOWAIT")) {
        sqlInitCheckPoint(pr03CheckPointNoWait);
      }
      else if (!strcmp((char*)szOption, "WAIT")) {
        sqlInitCheckPoint(NULL);
      }
      else {
        p08runtimeerror (sqlca, sqlxa, cpr_invalid_option);
      }
      return;
    }
    /* Option OPTIMIZED_STREAMS */
    pr01GetProfileString(NULL, "OPTIMIZED_STREAMS", "", (char*)szOption, sizeof(szOption), pCommand->rawString, pCommand->cbLen);
    if (szOption[0] != '\0') {
      pr01Put2Upper( (char*)szOption );
      if (!strcmp((char*)szOption, "ON")) {
        SQLDesc->ConDesc->Connection->OptimizedStreams = 1;
      }
      else if (!strcmp((char*)szOption, "OFF")) {
        SQLDesc->ConDesc->Connection->OptimizedStreams = 0;
      }
      else {
        p08runtimeerror (sqlca, sqlxa, cpr_invalid_option);
      }
      return;
    }
    /* Option PARSEINFOCACHE_SIZE */
    pr01GetProfileString(NULL, "PARSEINFOCACHE_SIZE", "", (char*)szOption, sizeof(szOption), pCommand->rawString, pCommand->cbLen);
    if (szOption[0] != '\0') {
      tsp00_Int4 cbCacheSize = atoi((char*)szOption);
      if (cbCacheSize>=0) {
        SQLDesc->ConDesc->Connection->SetAttr (SQLDesc->ConDesc, ParseInfoCacheSize_epr01Con, (void*)&cbCacheSize, sizeof(tsp00_Int4));
      }
      else {
        p08runtimeerror (sqlca, sqlxa, cpr_invalid_option);
      }
      return;
    }
    /* Option PARSEINFOCACHE=OFF */
    pr01GetProfileString(NULL, "PARSEINFOCACHE", "", (char*)szOption, sizeof(szOption), pCommand->rawString, pCommand->cbLen);
    if (szOption[0] != '\0') {
      pr01Put2Upper( (char*)szOption );
      if (!strcmp((char*)szOption, "OFF")) {
        tsp00_Int4 cbCacheSize = 0;
        SQLDesc->ConDesc->Connection->SetAttr (SQLDesc->ConDesc, ParseInfoCacheSize_epr01Con, (void*)&cbCacheSize, sizeof(tsp00_Int4));
      }
      else {
        p08runtimeerror (sqlca, sqlxa, cpr_invalid_option);
      }
      return;
    }
    /*unknown option -> error*/
    p08runtimeerror (sqlca, sqlxa, cpr_invalid_option);
  }
}

void pr01EXECSQLVersion(tpr01_SQLDesc *SQLDesc)
{
  tpr01_SQLContainer *SQL = SQLDesc->SQL;
  sqlcatype *sqlca = SQL->GetSqlca(SQLDesc);
  sqlxatype *sqlxa = SQL->GetSqlxa(SQLDesc);
  sqlratype    *sqlra = sqlca->sqlrap;
  sqlkaentry *ka   = SQLDesc->ka;
  sqltatype  *sqlta = sqlra->rasqltap;
  sqlgaentry *ga   = SQLDesc->ConDesc->ga;
  struct SQLERROR *sqlemp = sqlca->sqlemp;
  tpr_runtime_errors_Enum err = cpr_p_ok;
  if (ka->kapacount > 1) {
    p01xtracefilecheck (sqlca, sqlxa);
    p01pparsidtrace (sqlca, sqlxa, ka, ka->kaParseInfo.ParseId, TraceParseIDVersion_epr00);
    if (ka->kapaindex > 0) {
      tsp00_C40 version;
      void *pLZUVersion;
      void *pPCRVersion;
      void *pIndAddr;
      int va1ind;
      int indind, cnt, paix;
      paix = ka->kapaindex;
      p03getparameteraddr (sqlca, sqlxa, &paix,
                           &cnt, &pLZUVersion, &va1ind,
                           &pIndAddr, &indind );
      if (!pLZUVersion) {
        p08runtimeerror (sqlca, sqlxa,
                         cpr_missing_variable_addr);
      }
      else {
        sqlversion(version);
        p01verstrace  (sqlca, sqlxa, 1, version);
        p01putversion (sqlca, sqlxa, va1ind, mxsp_c40, version, &err, &pLZUVersion);
      }
      paix = ka->kapaindex+1;
      p03getparameteraddr (sqlca, sqlxa, &paix,
                           &cnt, &pPCRVersion, &va1ind,
                           &pIndAddr, &indind );
      if (!pPCRVersion) {
        p08runtimeerror (sqlca, sqlxa,
                         cpr_missing_variable_addr);
      }
      else {
        p07version (&version);
        p01verstrace  (sqlca, sqlxa, 2, version);
        p01putversion (sqlca, sqlxa, va1ind, mxsp_c40,
                       version, &err, &pPCRVersion);
      }
    }
    if (err != cpr_p_ok)
      pr01TraceRuntimeError(sqlca, sqlxa, err);
    p01xtimetrace(sqlca, sqlxa, ga);
    p01xcmdclose(sqlca, sqlxa, ga);
  }
}

void pr01EXECSQLTraceLine(tpr01_SQLDesc *SQLDesc)
{
  tpr01_SQLContainer *SQL = SQLDesc->SQL;
  sqlcatype *sqlca = SQL->GetSqlca(SQLDesc);
  sqlxatype *sqlxa = SQL->GetSqlxa(SQLDesc);
  sqlratype    *sqlra = sqlca->sqlrap;
  sqlkaentry *ka   = SQLDesc->ka;
  sqltatype  *sqlta = sqlra->rasqltap;
  sqlgaentry *ga   = SQLDesc->ConDesc->ga;
  struct SQLERROR *sqlemp = sqlca->sqlemp;
  tpr05_String *pCommand = 0;
  sqlra->rakamode = ka->kamode;
  p03csqlcaareainit (sqlca);
  p01xtracefilecheck(sqlca, sqlxa);
  SQL->GetSQL(SQLDesc, &pCommand, Reference_epr01);
  if ( sqlta->tatrout != cpr_trace_off ) {
    p03cmdtrace(sqlra, ga, 1, cpr_com_trace, pCommand);
  }
}

/*!
  Function:     pr01EXECSQLCancel

  see also: 

  Description:  Canceled a running sql statement.
  
  Arguments:
    
  return value:
 */
void pr01EXECSQLCancel(struct tpr01_SQLDesc *SQLDesc)
{
  tpr01_SQLContainer *SQL = SQLDesc->SQL;
  sqlcatype *sqlca = SQL->GetSqlca(SQLDesc);
  sqlxatype *sqlxa = SQL->GetSqlxa(SQLDesc);
  sqlratype    *sqlra = sqlca->sqlrap;
  sqlkaentry *ka   = SQLDesc->ka;
  tpr01_ConDesc *ConDesc = NULL;
  tsp00_Int2 sess;
  if (ka->kapacount == 1) {
    if (ka->kapaindex == 0) {
      ConDesc = SQLDesc->ConDesc;
    }
    else {
      if (ka->kapaindex < 0) {
        sess = -ka->kapaindex;        
      }
      else {
        if (ka->kapaindex > 0) {
          tsp00_Uint1* VaAddr;
          tsp00_Int4 vaind;
          vaind = pr01cGetVaIndex(sqlxa, ka->kapaindex, &VaAddr);
          if (!VaAddr) {
            p08runtimeerror(sqlca, sqlxa, cpr_missing_variable_addr);
          }
          else {
            sqlv1pointer va1 = sqlxa->sqlv1p + vaind;
            sqlv2pointer va2 = sqlxa->sqlv2p + va1->sc.va1indva2 - 1;
            switch (va2->va2typ) {
            case CPR_VINT2 : {}
            case CPR_VUNS2 : {
              tsp00_Int2 sint;
              memcpy (&sint, va1->sc.va1addr, sizeof (sint));
              sess = sint;
              break;
            }
            case CPR_VINT4 : {}
            case CPR_VUNS4 : {
              tsp00_Int4 lint;
              memcpy (&lint, va1->sc.va1addr, sizeof (lint));
              sess = lint;
              break;
            }
            default : {
              p08runtimeerror(sqlca, sqlxa, cpr_unknown_datatype);
              break;
            }
            }
          }
        }
      }
      if (sess > 0) {
        tpr00_DatabaseName szDatabaseName;
        tpr01_ConContainer *Connection = SQLDesc->ConDesc->Connection;
        sprintf(szDatabaseName, ":%d", sess);
        ConDesc = Connection->FindDesc(Connection, szDatabaseName, SessionDefault_epr01Con);
      }
    }
  }
  if (ConDesc) {
    sqlgaentry *ga = ConDesc->ga;
    if (ga->gareference != 0) {
      p01canctrace(sqlca, sqlxa, ConDesc->SessionNr);
      p01pparsidtrace (sqlca, sqlxa, ka, *ParseIdNull_cpr00, TraceParseIDSQLStmt_epr00);
      sqlacancel (ga->gareference);
      p01xtimetrace(sqlca, sqlxa, ConDesc->ga);
    }
  }
  else
    p08runtimeerror (sqlca, sqlxa, cpr_db_session_not_allowed);
}

char *strnchr( const char *string, int c, size_t count  )
{
  char *s = (char*)string;
  while(*s != c) {
    if (count > 1) count--;
    else return NULL;
    s++;
  }
  return s;
}

static tsp00_Int4 pr01GetProfileString(
char *lpszSection,
char *lpszEntry,
char *lpszDefault,
char *lpszReturnBuffer,
tsp00_Int4 cbReturnBuffer,
char *szProfileStr,
tsp00_Int4 cbLenProfileStr
)
{
   char szUpper[64];
   char szLine[255];
   tsp00_Int4 l;
   boolean found=false;
   tsp00_Int4 ret = false;
   if (lpszDefault && cbReturnBuffer >= 0 && szProfileStr) {
      char *s, *s1, *x;
      size_t len;
      if (lpszSection) {
	    strncpy(szUpper, lpszSection, sizeof(szUpper)-1);
	    pr01Put2Upper(szUpper);
	    found = false;
      }
      else {
	   if (lpszEntry) {
	     strncpy(szUpper, lpszEntry, sizeof(szUpper)-1);
	     pr01Put2Upper(szUpper);
	   }
      }
      l = 0;
      s1 = szProfileStr;
      while (s1) {
	    s = strnchr(s1, ';', cbLenProfileStr);
	    if (s)
	      len =(size_t) (s - s1);
	    else
	      len = (size_t) cbLenProfileStr;
	    strncpy(szLine, s1, len);
	    szLine[len] = '\0';
	    s1 = (s) ? s+1 : NULL;
	    s = szLine;
	    /* remove trailing spaces and cr nl */
	    for(x = s + strlen(s)-1; x > s; x--) {
	      if (isspace(*x)) *x = '\0';
	      else break;
	    }
	    /* remove leading spaces */
	    while(isspace(*s)) s++;
          switch(*s) {
    	    case '\0': {
    	       goto end_while;
    	    }
    	    case '[': {
    	      if (found) {
    		    goto end_while;
    	      }
    	      else {
    		    s++;
    		    if (lpszSection) {
    		       if (pr01CompareStringUntil(s, szUpper, ']')) {
    			     if (lpszEntry) {
    			       strncpy(szUpper, lpszEntry, sizeof(szUpper)-1);
    			       pr01Put2Upper(szUpper);
    			       s += strlen(szUpper);
    			     }
    			     strncpy(lpszReturnBuffer, lpszDefault, cbReturnBuffer);
    			     found = true;
    		       }
    		    }
    		    else {
    		      s1 = strchr(s, ']');
    		    }
    	      }
              break;
    	    }
    	    default: {
    	       if (found || !lpszSection) {
    		     char *p;
    		     if (lpszEntry) {
    		       if (p=pr01CompareStringUntil(s, szUpper, '=')) {
    			     while(isspace(*p)) p++;
    			     strncpy(lpszReturnBuffer, p, cbReturnBuffer);
    			     found = true;
    			     goto end_while;
    		       }
    		     }
    		     else {
    		       if (lpszSection) {
    			     found = true;
    			     if (l < (cbReturnBuffer-1)) {
    			       if (l == 0)
    			         strcpy(lpszReturnBuffer, "");
    			       strncat(lpszReturnBuffer, s, (tsp00_Int4)(cbReturnBuffer-1)-l);
    			       l = (tsp00_Int4) strlen(lpszReturnBuffer);
    			     }
    			     else
    			       goto end_while;
    		       }
    		     }
    	       }
    	       break;
    	    }
          } /*end switch*/
      } /*end while*/
      end_while:
      if(!found)
	    strncpy(lpszReturnBuffer, lpszDefault, cbReturnBuffer);
      ret = (tsp00_Int4) strlen(lpszReturnBuffer);
   }
   return(ret);
}

static void pr01Put2Upper( char * string_ptr )
{
  tsp00_Uint2 i;
  if (string_ptr != NULL) {
    for (i=0; i < strlen(string_ptr); i++) {
      *(string_ptr + i) = toupper( *(string_ptr + i) );
    }
  }
  return;
}

static char *pr01CompareStringUntil(char *szSource, char *szString, char UntilChar)
{
   char *p;
   char *x;
   for (;isspace(*szSource); szSource++);
   if(p = strchr(szSource, UntilChar)) {
      *p = '\0';
      for (x=p-1; x > szSource && isspace(*x); x--) *x = '\0';
      pr01Put2Upper(szSource);
      if (!strcmp(szSource, szString))
	 return(p+1);
   }
   return(NULL);
}

#endif
