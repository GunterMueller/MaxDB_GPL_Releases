/*!================================================================

  module:       vpr02Con.c

 -------------------------------------------------------------------

  responsible:  BurkhardD, ThomasS

  special area: Connect Syntax Analyse

  description:  


  last changed: 2001-07-23  17:00 Thomas Simenec  Unicode Analyze Connect
  see also:

  change history: 

    2001-09-04 d025086 No implicite connect. 
    2001-05-30  18:23 Thomas Simenec  Unicode Analyze Connect

  -------------------------------------------------------------------------





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


/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gmt90.h"
#include "vpr01Con.h"
#include "vpr01SQL.h"
#include "vpr01StmtName.h"
#include "vpr07Macro.h"
#include "gpr03.h"
#include "gpr04.h"
#include "gpr05.h"
#include "gpr08.h"
#include "hsp78_0.h"
#include "vpr02Con.h"
#include "vpr03Con.h"
#include "vpr01Trace.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

static pr02ConKeywordTabInitialized = cpr_is_empty;

static pr05cKeywordTableStruct pr02ConKeywordTab [ ] = {
  { KeywordADABAS_epr01,     {{"ADABAS"    , 0}, {"", 0}, {"", 0}} },
  { KeywordANSI_epr01,       {{"ANSI"      , 0}, {"", 0}, {"", 0}} },
  { KeywordANSIORA_epr01,    {{"ANSIORA"   , 0}, {"", 0}, {"", 0}} },
  { KeywordAT_epr01,         {{"AT"        , 0}, {"", 0}, {"", 0}} },
  { KeywordCACHELIMIT_epr01, {{"CACHELIMIT", 0}, {"", 0}, {"", 0}} },
  { KeywordCHARACTER_epr01,  {{"CHARACTER" , 0}, {"", 0}, {"", 0}} },
  { KeywordCONNECT_epr01,    {{"CONNECT"   , 0}, {"", 0}, {"", 0}} },
  { KeywordCONNECTC_epr01,   {{"CONNECT:"  , 0}, {"", 0}, {"", 0}} },
  { KeywordDB2_epr01,        {{"DB2"       , 0}, {"", 0}, {"", 0}} },
  { KeywordEXPLICIT_epr01,   {{"EXPLICIT"  , 0}, {"", 0}, {"", 0}} },
  { KeywordIDENTIFIED_epr01, {{"IDENTIFIED", 0}, {"", 0}, {"", 0}} },
  { KeywordIMPLICIT_epr01,   {{"IMPLICIT"  , 0}, {"", 0}, {"", 0}} },
  { KeywordINTERNAL_epr01,   {{"INTERNAL"  , 0}, {"", 0}, {"", 0}} },
  { KeywordISOLATION_epr01,  {{"ISOLATION" , 0}, {"", 0}, {"", 0}} },
  { KeywordKEY_epr01,        {{"KEY"       , 0}, {"", 0}, {"", 0}} },
  { KeywordLEVEL_epr01,      {{"LEVEL"     , 0}, {"", 0}, {"", 0}} },
  { KeywordLOCK_epr01,       {{"LOCK"      , 0}, {"", 0}, {"", 0}} },
  { KeywordNOLOG_epr01,      {{"NOLOG"     , 0}, {"", 0}, {"", 0}} },
  { KeywordNORMAL_epr01,     {{"NORMAL"    , 0}, {"", 0}, {"", 0}} },
  { KeywordORACLE_epr01,     {{"ORACLE"    , 0}, {"", 0}, {"", 0}} },
  { KeywordSAPR3_epr01,      {{"SAPR3"     , 0}, {"", 0}, {"", 0}} },
  { KeywordSQLDB_epr01,      {{"SQL-DB"    , 0}, {"", 0}, {"", 0}} },
  { KeywordSQLMODE_epr01,    {{"SQLMODE"   , 0}, {"", 0}, {"", 0}} },
  { KeywordTIMEOUT_epr01,    {{"TIMEOUT"   , 0}, {"", 0}, {"", 0}} },
  { KeywordUSING_epr01,      {{"USING"     , 0}, {"", 0}, {"", 0}} }
};

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

int s30len(tsp00_MoveObj VAR_ARRAY_REF str, unsigned char val, tsp00_Int4 cnt);
void pr03cSetErrorExt(struct SQLERROR *sqlemp, tpr_runtime_errors_Enum error, char* szErrExt);

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

/*!
  Function:     pr02ConGetConParameter

  see also: 

  Description:  Gets username/password/userkey from hostvariables.
                If userpassw points to NULL only param will increment.
                You can use this to enumerate over the hostvars in the
                CONNECT command.
    
  Arguments:
    sqlca           [in]
    sqlxa           [in]
    ga              [in]
    ka              [in]
    param           [in|out] Index in va area
    userpassw       [out]    username/password from hostvariable
    maxlen          [in]     bufferlength for userpassw
    encodingType    [in]     Unicode/Ascii
    
  return value:
 */
void pr02ConGetConParameter (sqlcatype *sqlca, sqlxatype *sqlxa, 
                             sqlgaentry *ga, sqlkaentry *ka, int *param, 
                             char *userpassw, int maxlen,
                             tpr05_StringEncoding* encodingType)
{
  M90TRACE(M90_TR_ENTRY, "pr02ConGetConParameter", 0);
  {
    int paind, cnt, vaind, indind, len, pos;
    int va1indva2;
    tsp00_Int2 sint;
    void *vaddr;
    void *vaaddr;
    void *iaddr;
    sqlva2en sqlv2;

    if (sqlca->sqlrap->ralang != CPR_LA_CALL) {
      paind = ka->kapaindex + *param;
      p03getparameteraddr (sqlca, sqlxa, &paind,
                           &cnt, &vaddr, &vaind,
                           &iaddr, &indind);
      paind++;
      *param = paind - ka->kapaindex;
      if (!userpassw)
        return;
      vaaddr = vaddr;
      va1indva2 = sqlxa->sqlv1p[vaind-1].sc.va1indva2;
      sqlv2 = sqlxa->sqlv2p[va1indva2-1];
      len = sqlv2.va2size;
      if (sqlv2.va2typ == cpr_vcharc) {
        if (sqlv2.va2size == cpr_pointerlen)
          len = mxsp_buf;
        len = s30len ((unsigned char *) vaaddr, '\0', len);
      } else if (sqlv2.va2typ == cpr_vucs2 || sqlv2.va2typ == cpr_vunicodec) {
        void *userpasswbuf;
        tsp00_Uint4 userlen;

        len = 2 * sp81UCS2strlen (vaaddr);
        userpasswbuf = userpassw;
        userlen = maxlen;       
        sp77encodingUCS2Native->fillString (&userpasswbuf, 
                                            &userlen,
                                            userlen / 2,
                                            ' ');

        if(sqlLocalMachineSwapKind == sw_normal) {
          *encodingType = sp77encodingUCS2;
        } else if (sqlLocalMachineSwapKind == sw_full_swapped) {
          *encodingType = sp77encodingUCS2Swapped;
        } else {
          /* Error unsupported swap type */
          pr01TraceRuntimeError (sqlca, sqlxa, cpr_paramlist_wrong);
        }
        
      }
      if (len > maxlen)
        len = maxlen;
      if ((sqlv2.va2typ == cpr_vchar) ||
          (sqlv2.va2typ == cpr_vbchar) ||
          (sqlv2.va2typ == cpr_vcharc) ||
          (sqlv2.va2typ == cpr_vbuf) ||
          (sqlv2.va2typ == cpr_vucs2)||
          (sqlv2.va2typ == cpr_vunicodec)) {
        memcpy (userpassw, vaaddr, len);
      } else {
        if ((sqlv2.va2typ == cpr_vstring) ||
            (sqlv2.va2typ == cpr_vstring1)) {
          if (sqlv2.va2typ != cpr_vstring1) {
            memcpy (&sint, vaaddr, mxsp_c2);
            pos = 3;
          } else {
            sint = ((char *) vaaddr)[0];
            pos = 2;
          }
          len = sint;
          if (len > maxlen) {
            len = maxlen;
          }
          memcpy (userpassw, (char *) vaaddr+pos, len);
        } else {
          pr01TraceRuntimeError (sqlca, sqlxa, cpr_paramlist_wrong);
        }
      }
    }
  }
  M90TRACE(M90_TR_EXIT, "pr02ConGetConParameter", 0);
}

void p08batdbanalyse (sqlcatype*, sqlxatype*, tsp00_Int4*, tsp00_Int4*,
                      tpr05_Symbol*, tsp00_Int4, char*);

void p08busingdbanalyse (sqlcatype*, sqlxatype*, tsp00_Int4*, tsp00_Int4*,
                         tpr05_Symbol*, tsp00_Int4, char*);

/*!
  Function:     pr02ConAnalyzeSQLMODE

  see also: 

  Description:  Analyzes SQLMODE argument of the connect statement
    
  Arguments:
    sqlca           [in]
    sqlxa           [in]
    gae             [in]
    pIter           [in] Statementiterator to actual analyse pos.

    return value: sets sqlcode in case of an error.

 */
static void pr02ConAnalyzeSQLMODE (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae, tpr05_SymbolIterator *pIter)
{
  tsp00_Int2 ansi = cpr_kind_empty;
  tsp00_Int2 mode = cpr_kind_empty;
  tpr01_KeywordTypeEnum StmtKeyword = 
    pr05cSymbolGetKeyword (pIter, pr02ConKeywordTab, 
                           PR07NUMELEM (pr02ConKeywordTab));
  switch (StmtKeyword) {
  case KeywordSQLDB_epr01:
    mode = cpr_kind_internal;
    break;
  case KeywordSAPR3_epr01:
    mode = cpr_kind_sapr3;
    break;
  case KeywordDB2_epr01:
    mode = cpr_kind_db2;
    break;
  case KeywordADABAS_epr01:
  case KeywordINTERNAL_epr01:
    mode = cpr_kind_internal;
    break;
  case KeywordANSI_epr01:
    mode = cpr_kind_internal;
    ansi = cpr_kind_ansi;
    break;
  case KeywordANSIORA_epr01:
    mode = cpr_kind_oracle;
    ansi = cpr_kind_ansi;
    break;
  case KeywordORACLE_epr01:
    mode = cpr_kind_oracle;
    break;
  default:
    mode = cpr_kind_empty;
    break;
  }
  if (sqlca->sqldbmode == cpr_kind_empty) {
    sqlca->sqldbmode = mode;
    sqlca->sqlrap->rasqlansi = ansi;
    if (sqlca->sqlrap->rasqlansi == CPR_KIND_ANSI)
      gae->gaxuserrec.xu_isolation = CPR_LO_ISOLEV3;
    else
      gae->gaxuserrec.xu_isolation = CPR_LO_ISOLEV10;
  } else {
    if ((sqlca->sqldbmode != mode) ||
        (sqlca->sqlrap->rasqlansi != ansi) ||
        (mode == cpr_kind_empty))
      p08runtimeerror (sqlca, sqlxa, cpr_differ_sqlmode_not_allowed);
  }
}

/*!
  Function:     pr02ConAnalyzeLOCK

  see also: 

  Description:  Analyzes LOCK argument of the connect statement
    
  Arguments:
    sqlca           [in]
    sqlxa           [in]
    gae             [in]
    pIter           [in] Statementiterator to actual analyse pos.

    return value: sets sqlcode in case of an error.

 */
static void pr02ConAnalyzeLOCK (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae, tpr05_SymbolIterator *pIter)
{
  tpr01_KeywordTypeEnum StmtKeyword; 
  pr05cSymbolEnum (pIter);
  StmtKeyword = pr05cSymbolGetKeyword (pIter, pr02ConKeywordTab, 
                                       PR07NUMELEM (pr02ConKeywordTab));
  switch (StmtKeyword) {
  case KeywordNORMAL_epr01:
    gae->gaxuserrec.xu_isolation = CPR_LO_ISOLEV10;
    break;
  case KeywordEXPLICIT_epr01:
    gae->gaxuserrec.xu_isolation = CPR_LO_ISOLEV0;
    break;
  case KeywordIMPLICIT_epr01:
    gae->gaxuserrec.xu_isolation = CPR_LO_ISOLEV20;
    break;
  default:
    p08runtimeerror (sqlca, sqlxa, cpr_connect_syntax_wrong);
    break;
  }
}

/*!
  Function:     pr02ConAnalyzeISOLEVEL

  see also: 

  Description:  Analyzes ISOLATION LEVEL argument of the connect statement
    
  Arguments:
    sqlca           [in]
    sqlxa           [in]
    gae             [in]
    pIter           [in] Statementiterator to actual analyse pos.

    return value: sets sqlcode in case of an error.

 */
static void pr02ConAnalyzeISOLEVEL (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae, tpr05_SymbolIterator *pIter)
{
  pr05cSymbolEnum (pIter);
  if (pr05cSymbolGetKeyword (pIter, pr02ConKeywordTab, 
                             PR07NUMELEM (pr02ConKeywordTab)) 
      == KeywordLEVEL_epr01) {
    if (sqlca->sqlrap->rasqlansi == cpr_kind_ansi)
      gae->gaxuserrec.xu_isolation = CPR_LO_ISOLEV3;
    else
      gae->gaxuserrec.xu_isolation = CPR_LO_ISOLEV10;
    if (pr05cSymbolEnum (pIter) == CPR_S_UNSIGNED_INTEGER) {
      tsp00_Int2 level;
      level = p05chrtoint2 ((void *) pIter->IString->rawString, 
                            pIter->SymbolPos,
                            pIter->SymbolLength);
      if ((level != CPR_LO_ISOLEV0) &&
          (level != CPR_LO_ISOLEV1) &&
          (level != CPR_LO_ISOLEV2) &&
          (level != CPR_LO_ISOLEV3) &&
          (level != CPR_LO_ISOLEV4) &&
          (level != CPR_LO_ISOLEV9) &&
          (level != CPR_LO_ISOLEV10) &&
          (level != CPR_LO_ISOLEV15) &&
          (level != CPR_LO_ISOLEV20) &&
          (level != CPR_LO_ISOLEV30))
        p08runtimeerror (sqlca, sqlxa, cpr_connect_syntax_wrong);
      else
        gae->gaxuserrec.xu_isolation = level;
    }
    else
      p08runtimeerror (sqlca, sqlxa, cpr_connect_syntax_wrong);
  }
  else 
    p08runtimeerror (sqlca, sqlxa, cpr_connect_syntax_wrong);
}

static void pr02fillxuserset(tsp4_xuser_record *xuserrec, tsp4_xuserset xuserset){
  char blankC[sizeof(tsp00_KnlIdentifier)]=bsp_knl_identifier;    
  char UniblankC[sizeof(tsp00_KnlIdentifier)];
  char zeroBytesC[sizeof(tsp00_KnlIdentifier)];  
  void *fillPointer;
  tsp00_Uint4 fillLength;

  memset (&zeroBytesC[0],'\0',sizeof(tsp00_KnlIdentifier));
  memset (xuserset, '\0', sizeof(tsp4_xuserset));
  fillLength  = sizeof(tsp00_KnlIdentifier);
  fillPointer = &UniblankC[0];
  sp77encodingUCS2Native->fillString( &fillPointer, &fillLength, fillLength/2, ' ');
  
  /*XUserkey is already set*/
  PR07PUTBIT(xuserset,sp4xu_sql_userkey);  

  if (  (strncmp ((const char *) xuserrec->xu_user, &blankC[0],  sizeof(xuserrec->xu_user))
        ||memcmp ( xuserrec->xu_userUCS2,     &UniblankC[0],  sizeof(xuserrec->xu_userUCS2))) 
      &&(memcmp  ( xuserrec->xu_password,     &zeroBytesC[0], sizeof(xuserrec->xu_password))
        ||memcmp ( xuserrec->xu_passwordUCS2, &zeroBytesC[0], sizeof(xuserrec->xu_passwordUCS2))) 
  ) {
    PR07PUTBIT(xuserset,sp4xu_sql_userparms);  
  }
  if (strncmp ((const char *) xuserrec->xu_sqlmode, &blankC[0],  sizeof(xuserrec->xu_sqlmode))) {
    PR07PUTBIT(xuserset,sp4xu_sql_mode);  
  }
  if (strncmp ((const char *) xuserrec->xu_servernode, &blankC[0],  sizeof(xuserrec->xu_servernode))) { 
    PR07PUTBIT(xuserset,sp4xu_sql_servernode);  
  }
  if (strncmp ((const char *) xuserrec->xu_serverdb, &blankC[0],  sizeof(xuserrec->xu_serverdb))) {
    PR07PUTBIT(xuserset,sp4xu_sql_serverdb);  
  }
  if (xuserrec->xu_cachelimit != -1) {
    PR07PUTBIT(xuserset,sp4xu_sql_cachelimit);  
  }
  if (xuserrec->xu_timeout != -1) {
    PR07PUTBIT(xuserset,sp4xu_sql_timeout);  
  }
  if (xuserrec->xu_isolation != -1) {
    PR07PUTBIT(xuserset,sp4xu_sql_isolation);  
  }
}

static void pr02ConOpt2XUser(tsp4_xuser_record *gaxu, 
                             tsp4_xuser_record *opxuserrec, 
                             tsp4_xuserset xuserset)
{
#define PR02MEMCPY(p) memcpy(gaxu->p, opxuserrec->p, sizeof(opxuserrec->p))
  if (PR07TSTBIT(xuserset, sp4xu_sql_userkey)) {
    PR02MEMCPY(xu_key);
  }
  if (PR07TSTBIT(xuserset, sp4xu_sql_userparms)) {
    PR02MEMCPY(xu_user);
    PR02MEMCPY(xu_password);
    PR02MEMCPY(xu_userUCS2);
    PR02MEMCPY(xu_passwordUCS2);
  }
  if (PR07TSTBIT(xuserset, sp4xu_sql_mode)) {
    PR02MEMCPY(xu_sqlmode);
  }
  if (PR07TSTBIT(xuserset, sp4xu_sql_servernode)) {
    PR02MEMCPY(xu_servernode);
  }
  if (PR07TSTBIT(xuserset, sp4xu_sql_serverdb)) {
    PR02MEMCPY(xu_serverdb);
  }
  if (PR07TSTBIT(xuserset, sp4xu_sql_cachelimit)) {
    gaxu->xu_cachelimit = opxuserrec->xu_cachelimit;
  }
  if (PR07TSTBIT(xuserset, sp4xu_sql_timeout)) {
    gaxu->xu_timeout = opxuserrec->xu_timeout;
  }
  if (PR07TSTBIT(xuserset, sp4xu_sql_isolation)) {
    gaxu->xu_isolation = opxuserrec->xu_isolation;
  }
#undef PR02MEMCPY
}


static pr02ConGetFromXUser(sqlcatype *sqlca, sqlgaentry *gae, BOOLEAN OldStyle)
{
  struct SQLERROR *sqlemp = sqlca->sqlemp;
  unsigned char   ok;
  tsp4_xuser_record  *gaxu = &gae->gaxuserrec;
  tsp00_Int4 keyDefault = (!memcmp(gaxu->xu_key, "DEFAULT ", 8) || !memcmp(gaxu->xu_key, "default ", 8));
  tsp00_Int4 keySQLOPT  = (!memcmp(gaxu->xu_key, "SQLOPT  ", 8) || !memcmp(gaxu->xu_key, "sqlopt  ", 8));
  if (keyDefault) {
    /*KEY DEFAULT found*/
    tsp4_xuser_record  tmpxuserrec;
    sqlindexuser(1, &tmpxuserrec, NULL, sqlemp->etext, &ok);
    if (ok) {
      tsp4_xuserset xuserset;
      pr02fillxuserset(&tmpxuserrec, xuserset);
      pr02ConOpt2XUser(gaxu, &tmpxuserrec, xuserset);
    }
    else {
      p04trint4(sqlca->sqlrap, "No XUSER entry found. Continue", sqlca->sqlemp->elzu);
    }  
  }
  if (keyDefault || keySQLOPT){
    tsp4_xuser_record opxuserrec;
    tsp00_Name password;      
    tsp4_xuserset xuserset;
    tsp4_args_options options;
    options.opt_component = sp4co_sql_pc_runtime;
    sqlarg3(&opxuserrec, password, &options, xuserset, sqlemp->etext, &ok);
    if (!ok) {
      pr01TracePrintf(sqlca->sqlrap, " ");
      pr07P2C( sqlemp->etext, sqlemp->etext, sizeof(sqlemp->etext));
      pr01TracePrintf(sqlca->sqlrap, "INFO    : sqlarg3 failed. (%s)", sqlemp->etext);
      if (PR07TSTBIT(xuserset, sp4xu_sql_userkey)) {
        /* option -U is set */
        pr07P2C( opxuserrec.xu_key, opxuserrec.xu_key, sizeof(opxuserrec.xu_key));
        pr01TracePrintf(sqlca->sqlrap, "INFO    : Ignore option USERKEY (%s)", opxuserrec.xu_key);
        PR07CLRBIT(xuserset, sp4xu_sql_userkey);
      }
      /* update gaxu even sqlarg3 failed */
      ok = true;
    }
    if (ok) {
      if (PR07TSTBIT(xuserset, sp4xu_sql_userparms)) {
        p03encrypt(password, opxuserrec.xu_password);
        p03encrypt(password, opxuserrec.xu_passwordUCS2);
      }            
      pr02ConOpt2XUser(gaxu, &opxuserrec, xuserset);
    }
  }
  else {
    /*any key other than DEFAULT and SQLOPT*/    
    sqlgetuser(gaxu, NULL, sqlemp->etext, &ok);
    if (!ok) {
      sqlemp->elzu = sp1ce_notok;
      p03cseterror(sqlemp, cpr_xuser_file_open_error);
    }
  }
}
/*!
  Function:     pr02ConAnalyzeKEY

  see also: 

  Description:  Analyzes KEY argument of the connect statement
    
  Arguments:
    sqlca           [in]
    sqlxa           [in]
    gae             [in]
    pIter           [in] Statementiterator to actual analyse pos.
    param           [in/out] Iterator to hostvariables

    return value: sets sqlcode in case of an error.

 */
static void pr02ConAnalyzeKEY (sqlcatype *sqlca, sqlxatype *sqlxa, sqlkaentry *ka, sqlgaentry *gae, tpr05_SymbolIterator *pIter, int *param, BOOLEAN OldStyle)
{
  tsp4_xuser_record    *gaxu = &gae->gaxuserrec;
  tpr05_StringEncoding   encodingType = pIter->IString->encodingType;
  tpr05_Symbol symb = pr05cSymbolEnum (pIter);
  struct SQLERROR *sqlemp=sqlca->sqlemp;
  BOOLEAN getdefault = FALSE;
  if (symb == CPR_S_PARAMETER_NAME || symb == CPR_S_QUESTION_MARK) {
    memset (&gaxu->xu_key, ' ', sizeof(gaxu->xu_key));
    pr02ConGetConParameter (sqlca, sqlxa, gae, ka, param, (char*)gaxu->xu_key, sizeof(gaxu->xu_key), &encodingType);
  } else if ( symb == CPR_S_IDENTIFIER) {
    int len;
    len = PR07MIN(sizeof(gaxu->xu_key), pIter->SymbolLength);
    memset(gaxu->xu_key, ' ', sizeof(gaxu->xu_key));
    memcpy(gaxu->xu_key, &pIter->IString->rawString[pIter->SymbolPos-1], len);
  } else {
    pr03cSetErrorExt(sqlemp, cpr_connect_syntax_wrong, "KEY clause");
  }
  if (sqlemp->eprerr == 0) {
    pr02ConGetFromXUser(sqlca, gae, OldStyle);
  }
}

/*!
  Function:     pr02ConAnalyzeUSING

  see also: 

  Description:  Analyzes USING argument of the connect statement
    
  Arguments:
    sqlca           [in]
    sqlxa           [in]
    gae             [in]
    pIter           [in] Statementiterator to actual analyse pos.
    param           [in/out] Iterator to hostvariables

    return value: sets sqlcode in case of an error.

 */
static void pr02ConAnalyzeUSING (sqlcatype *sqlca, sqlxatype *sqlxa, sqlkaentry *ka, sqlgaentry *gae, tpr05_SymbolIterator *pIter, int *param, BOOLEAN OldStyle)
{
  tsp4_xuser_record    *gaxu = &gae->gaxuserrec;
  tpr05_StringEncoding   encodingType = pIter->IString->encodingType;
  tpr05_Symbol symb = pr05cSymbolEnum (pIter);
  struct SQLERROR *sqlemp=sqlca->sqlemp;
  BOOLEAN getdefault = FALSE;
  char szDBString[512];
  if (symb == CPR_S_PARAMETER_NAME || symb == CPR_S_QUESTION_MARK) {
    memset (szDBString, '\0', sizeof(szDBString));
    pr02ConGetConParameter (sqlca, sqlxa, gae, ka, param, szDBString, sizeof(szDBString), &encodingType);
  } else if ( symb == CPR_S_IDENTIFIER) {
    int len;
    len = PR07MIN(sizeof(szDBString), pIter->SymbolLength);
    memcpy(szDBString, &pIter->IString->rawString[pIter->SymbolPos-1], len);
    szDBString[len] = '\0';
  } else {
    pr03cSetErrorExt(sqlemp, cpr_connect_syntax_wrong, "USING clause");
  }
  if (sqlemp->eprerr == 0) {
    char *szNetworkName=NULL;
    char *szServerNode=NULL;
    char *szServerName=NULL;
    pr02ConParseDBString(szDBString, &szNetworkName, &szServerNode, &szServerName);
    pr07C2P(gaxu->xu_servernode, szServerNode, sizeof(gaxu->xu_servernode));
    pr07C2P(gaxu->xu_serverdb, szServerName, sizeof(gaxu->xu_serverdb));
  }
}

/*!
  Function:     pr02ConAnalyzeConnectStmt

  see also: 

  Description:  Analyzes SQL connect statement with hostvariables
                unicode or ascii
    
  Arguments:
    sqlca           [in]
    sqlxa           [in]
    sqlga           [in]
    ga              [in]
    ka              [in]
    buflen          [in]     Length of connect statement
    SQLStatement    [in|out] Statement to be analyzed
    
  return value:
 */
void pr02ConAnalyzeConnectStmt (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgatype* sqlga,
                                sqlgaentry *ga, sqlkaentry *ka, tsp00_Int4 buflen,
                                tpr05_String *SQLStatement)
{
  M90TRACE(M90_TR_ENTRY, "pr02ConAnalyzeConnectStmt", 0);
  {

    tpr05_StringEncoding   encodingType = SQLStatement->encodingType; /*encoding type of statement*/ 

    tpr01_KeywordTypeEnum  StmtKeyword;

    tpr05_SymbolIterator   Iter;
    tpr05_SymbolIterator   *pIter;
    tpr05_Symbol symb;
    tpr05_String           userpassw;
    tsp00_KnlIdentifier     rawString;
    tsp00_Name               passw;
    tsp00_Int4             sypos;
    tsp00_Int4             sylen;

    tsp78ConversionResult  ConvRes;
    int                    param;
    int                    passwfound;
    int                    i, j, len;
    tsp00_Uint4            dlen, slen;
    struct SQLERROR        *sqlemp=sqlca->sqlemp;
    sqlxuserrec    *gaxu = &ga->gaxuserrec;
    BOOLEAN                ConnectOnly = true; /* true if no key, user spec or other connect param is found */

    pIter = pr05cSymbolOpen (SQLStatement, &Iter);

    if (pr02ConKeywordTabInitialized != cpr_is_init) {

      pr05cInitKeywordTable (pr02ConKeywordTab, PR07NUMELEM (pr02ConKeywordTab));
      pr02ConKeywordTabInitialized = cpr_is_init;
    }
    
    if (sqlca->sqlcode == 0) {
      BOOLEAN OldStyle = TRUE;
      if (sqlga->gaConnection) {
        tpr01_ConContainer *Connection = sqlga->gaConnection;
        OldStyle = Connection->OldStyle;
      }
      if (ga->gareference != 0) {
        p08runtimeerror (sqlca, sqlxa, cpr_session_already_connected);      
      } else {
        
        if (sqlca->sqlrap->ralang != CPR_LA_CALL) {
          memset (gaxu->xu_key, bsp_c1, sizeof(gaxu->xu_key));
          memset (gaxu->xu_user, bsp_c1, sizeof(gaxu->xu_user));
          memset (gaxu->xu_password, bsp_c1, sizeof(gaxu->xu_password));
          memset (gaxu->xu_sqlmode, bsp_c1, sizeof(gaxu->xu_sqlmode));
          gaxu->xu_cachelimit = -1;
          gaxu->xu_timeout = -1;
          gaxu->xu_isolation = CPR_LO_EMPTY;
          *(unsigned short *) &gaxu->xu_userUCS2[0] = (unsigned short) 0;
          *(unsigned short *) &gaxu->xu_passwordUCS2[0] = (unsigned short) 0;
        }
        p05up1casebuf ((void *) SQLStatement->rawString, 1, SQLStatement->cbLen);
        passwfound = 0;
        pr05cSymbolEnum (pIter);
        if (((StmtKeyword = 
              pr05cSymbolGetKeyword (pIter, pr02ConKeywordTab, 
                                     PR07NUMELEM (pr02ConKeywordTab))) 
             == KeywordCONNECT_epr01) ||
            (StmtKeyword == KeywordCONNECTC_epr01)) {
          /*KEYWORD CONNECT found*/
          if (sqlca->sqlrap->rasqlansi == CPR_KIND_ANSI)
            ga->gaxuserrec.xu_isolation = CPR_LO_ISOLEV3;
          else
            ga->gaxuserrec.xu_isolation = CPR_LO_ISOLEV10;
          param = 0;
          symb = pr05cSymbolEnum(pIter);
          while(symb != CPR_S_EOF && sqlca->sqlcode == 0) {
            StmtKeyword = pr05cSymbolGetKeyword (pIter, pr02ConKeywordTab, 
                                                 PR07NUMELEM (pr02ConKeywordTab));
            if (symb == CPR_S_PARAMETER_NAME || symb == CPR_S_QUESTION_MARK) {
              ConnectOnly = false;
              if (sqlca->sqlrap->ralang != CPR_LA_CALL) {
                /*searching for username or username/passwd from hostvar*/
                memset (&rawString, ' ', sizeof(rawString));
                pr02ConGetConParameter(sqlca, sqlxa, ga, ka, &param, 
                                       (char*)rawString, sizeof(rawString), &encodingType);
                
                userpassw.allocationType = constant_epr05;
                pr05IfCom_String_InitString (&userpassw, &rawString, sizeof (rawString), 
                                             encodingType, constant_epr05);
                
                if (!pr05IfCom_String_AsciiCharCompare (&userpassw, 0, "/") &&
                    !pr05IfCom_String_AsciiCharCompare (&userpassw, 1, " ")) {
                  passwfound = 1;
                  if (OldStyle && ((sqlga->gauseropset == cpr_is_false) ||
                      (sqlca->sqlrap->raactsession != cpr_se_primary)))
                    p03sqlgaentryinit (ga, sqlca->sqlrap->raactsession, sqlca->sqlemp);
                } else {
                  char xuuser[mxsp_c64];
                  
                  /*init xuserrec with spaces*/
                  memcpy (ga->gaxuserrec.xu_user, bsp_knl_identifier,
                          strlen (bsp_knl_identifier));
                  i = 0;
                  
                  ConvRes = sp78convertString (sp77encodingAscii, xuuser, sizeof (xuuser), &dlen, TRUE,			     
                                               encodingType, rawString, sizeof (tsp00_KnlIdentifier), &slen);
                  if (slen < sizeof (tsp00_KnlIdentifier)) {
                    ConvRes = sp78convertString (sp77encodingUCS2Native, xuuser, sizeof (xuuser), &dlen, TRUE,
                                                 encodingType, rawString, sizeof (tsp00_KnlIdentifier), &slen);
                    memcpy (ga->gaxuserrec.xu_userUCS2, xuuser, mxsp_c64);    
                  } else {
                    while (pr05IfCom_String_AsciiCharCompare (&userpassw, i, "/") &&
                           (i < mxsp_c64 / 2)) {
                      ga->gaxuserrec.xu_user[i] = xuuser[i];
                      i++;
                    }
                    if (!pr05IfCom_String_AsciiCharCompare (&userpassw, i, "/")) {
                      /*username/password-syntax found, copy passwd into xuserrec*/    
                      passwfound = 1;
                      i++;
                      j = 0;
                      memcpy (passw, bsp_name, sizeof (bsp_name));
                      while ((i < mxsp_c64) && (j < mxsp_name)) {
                        passw[j] = userpassw.rawString[i];
                        i++; j++;
                      }
                      p03encrypt (passw, ga->gaxuserrec.xu_password);
                    }

                  }
                }
              }
            }

            else {
              switch(StmtKeyword) {
              case (KeywordSQLMODE_epr01) : {
                ConnectOnly = false;
                pr05cSymbolEnum (pIter);
                if (sqlca->sqlrap->ralang != CPR_LA_CALL) {
                  pr03cSetErrorExt(sqlemp, cpr_connect_syntax_wrong, "SQLMODE not allowed");
                } else {
                  pr02ConAnalyzeSQLMODE(sqlca, sqlxa, ga, pIter);
                }
                break;
              } /* SQLMODE */
              case (KeywordNOLOG_epr01): {
                ConnectOnly = false;
                ga->ganolog = cpr_is_true;
                break;
              } /* NOLOG */
              case (KeywordLOCK_epr01): {
                ConnectOnly = false;
                pr02ConAnalyzeLOCK(sqlca, sqlxa, ga, pIter);
                break;
              } /* LOCK */
              case (KeywordISOLATION_epr01): {
                ConnectOnly = false;
                pr02ConAnalyzeISOLEVEL(sqlca, sqlxa, ga, pIter);
                break;
              } /* ISOLEVEL */
              case (KeywordTIMEOUT_epr01): {
                ConnectOnly = false;
                if (pr05cSymbolEnum (pIter) == CPR_S_UNSIGNED_INTEGER) {
                  ga->gaxuserrec.xu_timeout = p05chrtoint2 ((void *) pIter->IString->rawString, 
                                                            pIter->SymbolPos,
                                                            pIter->SymbolLength);
                } else
                  pr03cSetErrorExt(sqlemp, cpr_connect_syntax_wrong, "TIMEOUT clause");
                break;
              } /* TIMEOUT */
              case (KeywordCACHELIMIT_epr01): {
                ConnectOnly = false;
                if (pr05cSymbolEnum (pIter) == CPR_S_UNSIGNED_INTEGER) {
                  ga->gaxuserrec.xu_cachelimit = p05chrtoint2 ((void *) pIter->IString->rawString, 
                                                               pIter->SymbolPos,
                                                               pIter->SymbolLength);
                } else
                  pr03cSetErrorExt(sqlemp, cpr_connect_syntax_wrong, "CACHELIMIT clause");
                break;
              } /* CACHELIMIT */
              case (KeywordAT_epr01): {
                if (((sqlca->sqldbmode == cpr_kind_oracle) ||
                     (sqlca->sqldbmode == cpr_kind_sapr3)) &&
                    (sqlca->sqlrap->racomponent[0] != 'R')) {
                  sqlga->gasqlconntyp = cpr_ci_ora_connect;
                  p08batdbanalyse (sqlca, sqlxa,
                                   &pIter->SymbolPos, &pIter->SymbolLength, 
                                   &pIter->Symbol, buflen, pIter->IString->rawString);
                } else
                  pr03cSetErrorExt(sqlemp, cpr_connect_syntax_wrong, "AT clause");
                break;
              } /* AT */
              case (KeywordUSING_epr01): {
                ConnectOnly = false;
                if (OldStyle && (sqlca->sqldbmode == cpr_kind_oracle) ||
                    (sqlca->sqldbmode == cpr_kind_sapr3)) {
                  sqlga->gasqlconntyp = cpr_ci_ora_connect;
                  p08busingdbanalyse (sqlca, sqlxa,
                                      &pIter->SymbolPos, &pIter->SymbolLength, 
                                      &pIter->Symbol, buflen, pIter->IString->rawString);
                } else {
                  
                  pr02ConAnalyzeUSING(sqlca, sqlxa, ka, ga, pIter, &param, OldStyle);
                }

                break;
              } /* USING */
              case (KeywordKEY_epr01): {
                ConnectOnly = false;
                pr02ConAnalyzeKEY(sqlca, sqlxa, ka, ga, pIter, &param, OldStyle);
                break;
              } /* KEY */
              case (KeywordIDENTIFIED_epr01): {
                ConnectOnly = false;
                if (passwfound) {
                  sqlga->gasqlconntyp = cpr_ci_ora_connect;
                } else {
                  if (pr05cSymbolGetKeyword (pIter, pr02ConKeywordTab, 
                                             PR07NUMELEM (pr02ConKeywordTab))
                      == KeywordIDENTIFIED_epr01) {
                    pr05cSymbolEnum (pIter);
                    symb = pr05cSymbolEnum (pIter);
                    sylen = pIter->SymbolLength;
                    sypos = pIter->SymbolPos;
                    
                    if ((symb == CPR_S_PARAMETER_NAME) ||
                        (symb == CPR_S_QUESTION_MARK)) {
                      if (sqlca->sqlrap->ralang != CPR_LA_CALL) {
                        memset (&rawString, ' ', sizeof (rawString));
                        pr02ConGetConParameter (sqlca, sqlxa, ga, ka, &param, 
                                                (char*)rawString, sizeof(rawString), 
                                                &encodingType);
                        memcpy (passw, rawString, mxsp_name);
                        if (!strncmp ((const char *) ga->gaxuserrec.xu_user, bsp_c64, 
                                      sizeof(ga->gaxuserrec.xu_user)))
                          p03encrypt (passw, ga->gaxuserrec.xu_passwordUCS2);
                        else {
                          tsp00_Pw xupassw;
                          ConvRes = sp78convertString (sp77encodingAscii, xupassw, sizeof (xupassw), &dlen, 
                                                       TRUE, encodingType, rawString, sizeof (xupassw), &slen);
                          p03encrypt (xupassw, ga->gaxuserrec.xu_password);
                        }
                      }
                    } else {
                      if (sylen > mxsp_name)
                        len = mxsp_name;
                      else
                        len = sylen;
                      memcpy (passw, bsp_name, strlen (bsp_name));
                      for (i = 0; i < len; i++)
                        passw[i] = SQLStatement->rawString[sypos+i-1];
                      if (!pr05IfCom_String_AsciiCharCompare (SQLStatement, sypos-1, "\""))
                        sylen++;
                      p03encrypt (passw, ga->gaxuserrec.xu_password);
                    }
                  }
                }
                break;
              } /* IDENTIFIED */
              default: {
                ConnectOnly = false;
                /*username/password form literal*/
                StmtKeyword = pr05cSymbolGetKeyword (pIter, pr02ConKeywordTab, 
                                                     PR07NUMELEM (pr02ConKeywordTab));
              
                sylen = pIter->SymbolLength;
                sypos = pIter->SymbolPos;
                if (sylen > mxsp_c64)
                  len = mxsp_c64;
                else
                  len = sylen;
                memcpy (ga->gaxuserrec.xu_user, bsp_knl_identifier, 
                        strlen (bsp_knl_identifier));
                for (i = 0; i < len; i++) 
                  ga->gaxuserrec.xu_user[i] = SQLStatement->rawString[sypos+i-1];
              
                if (!pr05IfCom_String_AsciiCharCompare (SQLStatement, sypos-1, "\""))
                  sylen++;
                break;

              } /* default */
              } /* switch */
            } /* else PARAMETER */
            symb = pr05cSymbolEnum(pIter);
          } /* while */
        } /* CONNECT */
      } /* gareference != 0 */
      if (ConnectOnly) {
        /* Found EXEC SQL CONNECT; without any other arguments */
        /* get all userdata from the default XUserentry */
        memcpy(ga->gaxuserrec.xu_key, "DEFAULT ", 8);
        pr02ConGetFromXUser(sqlca, ga, OldStyle);
      }
      if (OldStyle) 
        p03conninfoget (sqlca->sqlgap, ga, sqlca->sqlrap->raactsession);
    } /* sqlcode == 0 */
  }
  M90TRACE(M90_TR_EXIT, "pr02ConAnalyzeConnectStmt", 0);
}

void pr02ConAnalyseConnect(struct tpr01_ConDesc *ConDesc, struct tpr01_StmtNameDesc *StmtNameDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr02ConAnalyseConnect", 0);
  {
    sqlcatype *sqlca = ConDesc->Connection->sqlca;
    sqlxatype *sqlxa = ConDesc->sqlxa;
    sqlgaentry *gaen = ConDesc->ga;
    sqlgatype *sqlga = sqlca->sqlgap;

    int cbLen;

    tpr05_String *pConString=NULL;
    tpr01_StmtNameContainer *StmtName = StmtNameDesc->StmtName; 
    
    int ret = StmtName->GetSQL(StmtNameDesc, &pConString, Reference_epr01);

    cbLen = pConString->cbLen;
    
    if (pConString->encodingType == sp77encodingAscii) {
      pr02ConAnalyzeConnectStmt (sqlca, sqlxa, sqlga, gaen, StmtNameDesc->ka, cbLen, pConString); 
    } else {
      /*error*/
    }
  }
  M90TRACE(M90_TR_EXIT, "pr02ConAnalyseConnect", 0);
}

/*!
  Function: pr02ConParseDBString 

  description: Parse einen nach ORACLE Konventionen zusammengesetzten
               Instanznamen.

	       Syntax:   
	              dbstring := [[<networkname>][<servernode>]]<servername>

		      networkname := [<networkstring>]<netsep>
		      servernode  := <nodestring><nodesep>
                      nodestring  := <nodename>:<portnumber>
		      servername  := <dbnamestring>
		      netsep := ':'
		      nodesep := '-'
	       
	       Der Inputstring wird von hinten nach vorne durchsucht um die 
	       Separatoren zu bestimmen. Ist ein <networkname> oder 
	       <servernode> nicht vorhanden oder leer, so zeigen die 
	       Outputwerte auf das Ende des szDBString ('\0').
	       Die Separatoren werden aus szDBString durch '\0' ersetzt.
		      	       
  arguments:
      szDBString   [in/out] Nullterminierter ORACLE Instanzname.
      szNetworName [out] Pointer auf <networkname>.
      szServerNode [out] Pointer auf <servernode>.
      szServerName [out] Pointer auf <servername>.
*/

void pr02ConParseDBString(char *szDBString, char **szNetworkName, char **szServerNode, char **szServerName)
{
  /* points to the end */
  char *szEnd = strrchr(szDBString, '\0');
  char cSep = ':';          /* net separator */
  char *p;
  *szNetworkName = szDBString;
  p = strrchr(szDBString, cSep);
  if (p == NULL) {
    *szServerNode = szDBString;
    *szNetworkName = szEnd;
  }
  else {
    /* drop the separator */    
    *p = '\0';
    szDBString = p+1;
  }    
  cSep = '-';
  *szServerNode = szDBString;
  p = strrchr(szDBString, cSep);
  if (p == NULL) {
    cSep = '@';
    p = strrchr(szDBString, cSep);
    if (p == NULL) {
      *szServerName = szDBString;      
      *szServerNode = szEnd;
    }
    else {
      /* drop the separator */
      *p = '\0';
      *szServerName = p+1;
    }
  }
  else {
    *p = '\0';
    *szServerName = p+1;
  }
  if (**szServerNode != '\0') {
    p = *szServerNode;
    /* check if szServerNode contains only numbers */
    /* only numbers means that the ist the port number so the 
       network name is the server name :<servernode>:<portnumber>-<dbname> */
    for (;*p != '\0';p++) {
      if (*p < '0' || *p > '9') {
        break;
      }      
    }
    /* if so the network name is the server name */
    if (*p == '\0') {
      *szServerNode = *szNetworkName;
      p = strrchr(*szServerNode, ':');
      if (p) {
        *szServerNode = p+1;
      }
      (*szServerNode)[strlen(*szServerNode)] = ':';
    }
  }
}

#ifdef TEST
main()
{
  char *szDBString = "p26235@DB72";
  char *szNetworkName;
  char *szServerNode;
  char *szServerName;
  pr02ConParseDBString(szDBString, &szNetworkName, &szServerNode, &szServerName );
  printf("%s,%s,%s", szNetworkName, szServerNode, szServerName);
}

void p08analyzeconnectstatm ()
{
}
#endif
