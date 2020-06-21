/*!**************************************************************************

 module:    vpr01Precom.c

 -------------------------------------------------------------------------

 responsible: BurkhardD

 special area:   CPCDrv | Client | Precompiler Kommandos

 description: Verwaltung von EXEC SQL Statements

 first created: ???
 change history: 

        2001-11-08 D025086 Cancel session bugfix PTS 1112515 and PASCAL 
                           port from p01cancel,
        2001-09-04 d025086 No implicite connect. 
        2001-06-22 d025086 unresolved variable dropped
        2000-11-16 d025086 (PTS 1108288) pr01PrecomVersion neu
        2000-08-07   9:57    Marco Paskamp   unicode enhancement

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


/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "vpr01Precom.h"
#include "gpr03.h"
#include "gpr08.h"
#include "vpr01Con.h"
#include "vpr02Con.h"
#include "vpr01Cursor.h"
#include "vpr01Error.h"
#include "vpr01Module.h"
#include "vpr01Env.h"
#include "vpr01SQL.h"
#include "vpr01StmtName.h"
#include "vpr07Macro.h"
#include "vpr01EXECSQL.h"
#include "vpr01Trace.h"
#include <stdio.h>
#include <string.h>
/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

int s30klen(char *str, char val, int cnt);

/*==================================================================*
 *  EXTERNAL VARIABLES                                              *
 *==================================================================*/

extern tpr01_EnvContainer *sqlEnvCont;

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

static tsp00_Int4 pr01PrecomOpenCont(tpr01_PrecomContainer *Cont);

static void pr01PrecomCloseCont(tpr01_PrecomContainer *Cont);

static void pr01PrecomResetIterator(tpr01_PrecomContainer *Cont);

static tpr01_PrecomDesc *pr01PrecomEnumDesc(tpr01_PrecomContainer *Cont);

static tpr01_PrecomDesc *pr01PrecomAddDesc(tpr01_PrecomContainer *Cont, tsp00_Int4 CmdNo);

static void pr01PrecomDeleteDesc(tpr01_PrecomDesc *PrecomDesc);

static tpr01_PrecomDesc *pr01PrecomFindDesc(tpr01_PrecomContainer *Cont, tsp00_Int4 CmdNo);

static tpr01_UniqueID pr01PrecomGetStmtName(tpr01_PrecomDesc *PrecomDesc, tpr05_String *stStmtName);

static tpr01_UniqueID pr01PrecomGetCursorName(tpr01_PrecomDesc *PrecomDesc, tpr00_CursorNameDesc *CursorNameDesc);

static char *pr01PrecomGetDatabaseName(tpr01_PrecomDesc *PrecomDesc, char *szDatabaseName);

static char *pr01PrecomGetDBString(tpr01_PrecomDesc *PrecomDesc);

static tsp00_Int4 pr01PrecomGetHostVarStringBuf(sqlxatype *sqlxa, tsp00_Int4 index, enum tpr_runtime_errors_Enum *error, tpr05_String *pSQLStatement, tpr01_SQLStatementOptionEnum fOption);

static void pr01PrecomPrepareStmtName(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc);

static tpr01_CursorDesc *pr01PrecomDeclareCursor(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc);

static void pr01PrecomDescribe(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc);

static void pr01PrecomOpen(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc );

static void pr01PrecomPrepare(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc);

static void pr01PrecomExecute(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc);

static void pr01PrecomTryExecute(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc);

static tpr01_StmtNameContainer *pr01PrecomGetStmtNameCont(tpr01_PrecomContainer *Cont);

static tpr01_ConDesc * pr01PrecomGetConnection(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tsp00_Int4 session);

static boolean pr01PrecomConnect(tpr01_PrecomDesc *PrecomDesc, tpr01_ConDesc *ConDesc, tpr01_StmtNameDesc *StmtNameDesc);

static tsp00_Int4 pr01PrecomGetError(tpr01_PrecomDesc *PrecomDesc, tpr01_ErrorDesc *ErrorDesc);

static tsp00_Int4 pr01PrecomSetError(tpr01_PrecomDesc *PrecomDesc, tpr_runtime_errors_Enum ErrorEnum);

static boolean pr01PrecomGetSQL(tpr01_PrecomDesc *PrecomDesc, tpr01_StmtNameDesc *StmtNameDesc);

static boolean pr01PrecomGetStaticSQL(tpr01_PrecomDesc *PrecomDesc, tpr01_StmtNameDesc *StmtNameDesc);

static boolean pr01PrecomGetDynamicSQL(tpr01_PrecomDesc *PrecomDesc, tpr01_StmtNameDesc *StmtNameDesc);

static tpr01_SQLDesc *pr01PrecomMakeSQLDesc(tpr01_PrecomDesc *PrecomDesc, tpr01_SQLDesc *SQLDesc, tpr01_ConDesc *ConDesc, tpr01_CursorDesc *CursorDesc, tpr00_ComTypeEnum ComType);

static void pr01PrecomStatementBind(tpr01_PrecomDesc *PrecomDesc, tpr05_String *pSQLStatement, tpr01_SQLStatementOptionEnum fOption, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc);

static void pr01PrecomPushError(tpr01_PrecomContainer *Cont, tpr00_ErrorKeyEnum ErrorKey);

static void pr01PrecomAfterExecute(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc);

static tpr01_UniqueID pr01PrecomUniqueID(tpr01_PrecomDesc *PrecomDesc);

static void pr01PrecomOption(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc);

static void pr01PrecomTraceLine(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc);

static void pr01PrecomVersion(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc);

static void pr01PrecomCancel(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc);

static void pr01PrecomAbortSession(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc);

static pr01PrecomRuntimeError(tpr01_PrecomDesc *PrecomDesc, tpr01_ConDesc *ConDesc, enum tpr_runtime_errors_Enum error);

#ifdef DYNAHASH  
static tpr09HashValue pr01Precom_GetKey (const tpr09HashValue data_p, tsp00_Uint4* HashKeyLen);

static tsp00_Int4 pr01Precom_CompareKey (const tpr09HashKey Item1,tsp00_Uint4 Item1Len, const tpr09HashKey Item2, tsp00_Uint4 Item2Len);

static void pr01Precom_printKey (const tpr09HashValue data_p, FILE* datei);

static tpr09HashIndex pr01Precom_HashFunc (const tpr09HashKey HashKey, tsp00_Uint4 cbHashKeyLen, tsp00_Uint4 cbHashTabLen);
/*
static void dumpKey ( tpr09HashValue data_p, char* praefix){
  printDump("%s \"CmdNo=%04d\"\n", praefix, *((tsp00_Int4*)data_p));
}
*/
#endif 
/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

void pr01PrecomDeleteCont(tpr01_PrecomContainer *Precom)
{
  if (Precom) {
    if (pr01PrecomOpenCont(Precom)) {
      tpr01_PrecomDesc *PrecomDesc;
      while(PrecomDesc = Precom->EnumDesc(Precom))
	Precom->DeleteDesc(PrecomDesc);
    }
    pr01SQLDeleteCont(Precom->SQL);
    pr01DeleteErrorDesc(Precom->ErrorDesc);
#ifdef DYNAHASH  
    pr09DeleteHashDescriptor(Precom->Desc);
#else
    pr09DeleteDescriptor(Precom->Desc);
#endif 
    Precom->Desc = NULL;
    pr03mFree((tsp00_Uint1*)Precom, "Precom");
  }
}

/*---------------------------------------------------------------------------*/

static void pr01PrecomInitCont(tpr01_PrecomContainer *Precom, tpr01_ModuleDesc *ModuleDesc, sqlcatype *sqlca, sqlxatype *sqlxa)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomInitCont", 0);
#ifdef DYNAHASH  
  Precom->Desc = pr09NewHashDescriptor(20, 
                                       sizeof(tpr01_PrecomDesc), 
                                       pr01Precom_CompareKey, 
                                       pr01Precom_HashFunc, 
                                       pr01Precom_GetKey,
                                       pr01Precom_printKey);
#else
  Precom->Desc = pr09NewDescriptor(3, sizeof(tpr01_PrecomDesc));
#endif 

  Precom->ModuleDesc = ModuleDesc;
  Precom->ErrorDesc  = pr01NewErrorDesc();
  Precom->SQL        = pr01SQLNewCont(ModuleDesc->Module->EnvDesc);
  Precom->sqlxa = sqlxa;
  Precom->sqlca = sqlca;
  Precom->OpenCont        = pr01PrecomOpenCont;
  Precom->CloseCont       = pr01PrecomCloseCont;
  Precom->ResetIterator   = pr01PrecomResetIterator;
  Precom->EnumDesc        = pr01PrecomEnumDesc;
  Precom->AddDesc         = pr01PrecomAddDesc;
  Precom->DeleteDesc      = pr01PrecomDeleteDesc;
  Precom->FindDesc        = pr01PrecomFindDesc;
  Precom->GetStmtName     = pr01PrecomGetStmtName;
  Precom->GetCursorName   = pr01PrecomGetCursorName;
  Precom->PrepareStmtName = pr01PrecomPrepareStmtName;
  Precom->DeclareCursor   = pr01PrecomDeclareCursor;
  Precom->Describe        = pr01PrecomDescribe;
  Precom->Open            = pr01PrecomOpen;
  Precom->TryExecute      = pr01PrecomTryExecute;
  Precom->Execute         = pr01PrecomExecute;
  Precom->Prepare         = pr01PrecomPrepare;
  Precom->GetConnection   = pr01PrecomGetConnection;
  Precom->Connect         = pr01PrecomConnect;
  Precom->GetError        = pr01PrecomGetError;
  Precom->SetError        = pr01PrecomSetError;
  Precom->StatementBind   = pr01PrecomStatementBind;
  Precom->PushError       = pr01PrecomPushError;
  Precom->Option          = pr01PrecomOption;
  Precom->TraceLine       = pr01PrecomTraceLine;
  Precom->Version         = pr01PrecomVersion;
  Precom->Cancel          = pr01PrecomCancel;
  Precom->AbortSession    = pr01PrecomAbortSession;
  M90TRACE(M90_TR_EXIT, "pr01PrecomInitCont", 0);
}

/*---------------------------------------------------------------------------*/

tpr01_PrecomContainer *pr01PrecomNewCont(struct tpr01_ModuleDesc *ModuleDesc)
{
  tpr01_PrecomContainer *Precom;
  M90TRACE(M90_TR_ENTRY, "pr01PrecomNewCont", 0);
  Precom = pr03mAllocat(sizeof(tpr01_PrecomContainer), "Precom");
  if (Precom)
    pr01PrecomInitCont(Precom, ModuleDesc, ModuleDesc->Module->EnvDesc->sqlca, ModuleDesc->sqlxa);
  M90TRACE(M90_TR_EXIT, "pr01PrecomNewCont", 0);
  return Precom;
}

/*---------------------------------------------------------------------------*/

static tsp00_Int4 pr01PrecomOpenCont(tpr01_PrecomContainer *Precom)
{
  if (Precom->Desc) {
#ifdef DYNAHASH  
    pr09ResetHashItemIterator(Precom->Desc);
#else
    pr09SetIndex(Precom->Desc, -1);
#endif 
    return 1;
  } else {
    return 0;
  }
}

/*---------------------------------------------------------------------------*/

static tpr01_PrecomDesc *pr01PrecomAddDesc(tpr01_PrecomContainer *Precom, tsp00_Int4 CmdNo)
{
  tpr01_PrecomDesc *PrecomDesc;
#ifdef DYNAHASH  
  tpr09_HashTableError hTabErr; 
  PrecomDesc = pr09GetNewHashItem(Precom->Desc);
#else
  PrecomDesc = pr09AddItem(Precom->Desc);
#endif 
  M90TRACE(M90_TR_ENTRY, "pr01PrecomAddDesc", 0);
  PrecomDesc->DescType = PrecomDesc_epr01;
  PrecomDesc->Precom   = Precom;
  PrecomDesc->SQLDesc  = NULL;
  PrecomDesc->ka = Precom->sqlxa->sqlkap + CmdNo-1;
  M90TRACE(M90_TR_WORD, "CmdNo", &CmdNo);
  if (PrecomDesc->ka->kaprindex > 0) {
    M90TRACE(M90_TR_WORD, "kaprindex", &PrecomDesc->ka->kaprindex);
    PrecomDesc->pr = Precom->sqlxa->sqlprp + PrecomDesc->ka->kaprindex-1;
  }
  else {
    PrecomDesc->pr = NULL;
  }
  PrecomDesc->CmdNo            = CmdNo;
#ifdef DYNAHASH  
   /*add the descriptor to the statement container hash index*/
  hTabErr = pr09AddHashItemToHash(Precom->Desc, 
                                  &CmdNo, 
                                  sizeof(tsp00_Int4),
                                  (tpr09HashValue*)PrecomDesc);
#endif 

  PrecomDesc->ErrorDesc        = pr01NewErrorDesc();
  PrecomDesc->StmtNameDesc     = NULL;
  PrecomDesc->ComType = PrecomDesc->ka->katyp;
  M90TRACE(M90_TR_EXIT, "pr01PrecomAddDesc", 0);
  return(PrecomDesc);
}

/*---------------------------------------------------------------------------*/

static void pr01PrecomDeleteDesc(tpr01_PrecomDesc *PrecomDesc)
{
#ifdef DYNAHASH  
  if (NULL != pr09FindHashItem(PrecomDesc->Precom->Desc, 
                               &PrecomDesc->CmdNo, 
                               sizeof(tsp00_Int4)) ){
#else
  int cbIndex = pr09ItemFindIndex(PrecomDesc->Precom->Desc, PrecomDesc);
  if (cbIndex >= 0) {
#endif 
    pr01DeleteErrorDesc(PrecomDesc->ErrorDesc);
#ifdef DYNAHASH  
    pr09DeleteHashItem(PrecomDesc->Precom->Desc, 
                               &PrecomDesc->CmdNo, 
                               sizeof(tsp00_Int4));
#else
    pr09CloseItemEx(PrecomDesc->Precom->Desc, cbIndex);
#endif 
  }
}

/*---------------------------------------------------------------------------*/

static void pr01PrecomResetIterator(tpr01_PrecomContainer *Cont){
#ifdef DYNAHASH  
  if (Cont && Cont->Desc) pr09ResetHashItemIterator(Cont->Desc);
#else
  if (Cont && Cont->Desc) pr09SetIndex(Cont->Desc, -1);
#endif 
}
/*---------------------------------------------------------------------------*/

static tpr01_PrecomDesc *pr01PrecomEnumDesc(tpr01_PrecomContainer *Precom)
{
  {
#ifdef DYNAHASH  
    tpr01_PrecomDesc *NextDesc = pr09HashItemIteratorNextItem(Precom->Desc);
#else
    tpr01_PrecomDesc *NextDesc = pr09GetNextItem(Precom->Desc);
#endif 
    M90TRACEIF(M90_TR_DWORD, "pr01PrecomEnumDesc", (NextDesc), &NextDesc->CmdNo);
    return(NextDesc);
  }
}

/*---------------------------------------------------------------------------*/

static void pr01PrecomCloseCont(tpr01_PrecomContainer *Cont)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomCloseCont", 0);
#ifdef DYNAHASH  
  pr09DeleteAllHashItems(Cont->Desc);
#else
  pr09CloseAllItems(Cont->Desc);
#endif 
  M90TRACE(M90_TR_EXIT, "pr01PrecomCloseCont", 0);
}

/*---------------------------------------------------------------------------*/
#ifdef DYNAHASH  
static tpr01_PrecomDesc *pr01PrecomFindDesc(tpr01_PrecomContainer *Cont, tsp00_Int4 CmdNo)
{
  tpr01_PrecomDesc *PrecomDesc = (tpr01_PrecomDesc *)pr09FindHashItem(  Cont->Desc, 
                                                            &CmdNo, 
                                                            sizeof(tsp00_Int4)
                                                          );
  M90TRACE(M90_TR_HANDLE, "PrecomDesc", &PrecomDesc);
  return(PrecomDesc);
}
#else
static tpr01_PrecomDesc *pr01PrecomFindDesc(tpr01_PrecomContainer *Cont, tsp00_Int4 CmdNo)
{
  tsp00_Int4 cbOldIndex = pr09SetIndex(Cont->Desc, -1);
  tpr01_PrecomDesc *PrecomDesc=NULL;
  M90TRACE(M90_TR_ENTRY, "pr01PrecomFindDesc", 0);
  while(PrecomDesc=Cont->EnumDesc(Cont)) {
    if (CmdNo == PrecomDesc->CmdNo) {
      M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
      break;
    }
  }
  if (!PrecomDesc)
    pr09SetIndex(Cont->Desc, cbOldIndex);
  M90TRACE(M90_TR_EXIT, "pr01PrecomFindDesc", 0);
  M90TRACE(M90_TR_HANDLE, "PrecomDesc", &PrecomDesc);
  return(PrecomDesc);
}
#endif 


/*---------------------------------------------------------------------------*/

int s30len(tsp00_MoveObj VAR_ARRAY_REF str, unsigned char val, tsp00_Int4 cnt);

/*---------------------------------------------------------------------------*/
/*!
  Function:     pr01PrecomGetStmtName

  see also: 
  
  Description:  Gets the name of statement
  Arguments:
    PrecomDesc [in] Pointer to the precm descriptor
    CursorName [in|out] name of statement
    cbLen      [in] length of ... in bytes 

  return value: UniqueID of statement or EmptyUniqueID_cpr01 if no UniqueID was found 
 */
static tpr01_UniqueID pr01PrecomGetStmtName(tpr01_PrecomDesc *PrecomDesc, tpr05_String *stStmtName)
{
  tpr01_UniqueID UniqueID = EmptyUniqueID_cpr01;
  M90TRACE(M90_TR_ENTRY, "pr01PrecomGetStmtName", 0);
  PR07_CHECKDESC(PrecomDesc, PrecomDesc_epr01);
  {
    sqlkapointer ka = PrecomDesc->ka;
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    sqlxatype *sqlxa = Precom->sqlxa;
    sqlprpointer pr = PrecomDesc->pr;
    if (pr) {
      if (pr->prStmtNameIndex > 0) 
      {
	    /* statement name ueber hostvariable */
	    /* prStmtNameIndex ist nicht der vaindex sondern ein index in die paarea */
	    /* also ersteinmal vaindex ermitteln */
	    enum tpr_runtime_errors_Enum error = cpr_p_ok;
	    pr01PrecomGetHostVarStringBuf(sqlxa, pr->prStmtNameIndex, &error, stStmtName, First_epr01);
	    if (error != cpr_p_ok ) 
            {
              p08runtimeerror(Precom->sqlca, sqlxa, error);
              stStmtName->cbLen = 0;
	    }
      }
      else 
      {
	    tsp00_Uint4 len;
	    len = (tsp00_Uint4) s30klen ((char*)pr->prStmtName, bsp_c1, sizeof(tsp00_KnlIdentifier));
	    len = PR07MIN(len, stStmtName->cbLen);
	    if (len) 
	    {
	        tpr05_StringError erg = pr05IfCom_String_ConvertP (stStmtName,
                                                                   (char*)pr->prStmtName, 
                                                                   len,
                                                                   sp77encodingAscii);
	    }
	    if (len == 0) 
	    {
	        UniqueID = pr01PrecomUniqueID(PrecomDesc);
	    }
      }
    }
    else {
      UniqueID = pr01PrecomUniqueID(PrecomDesc);
    }
    M90TRACE(M90_TR_EXIT, "pr01PrecomGetStmtName", 0);
    return UniqueID;
  }
}

/*---------------------------------------------------------------------------*/
/*!
  Function:     pr01PrecomGetCursorName

  see also: 
  
  Description:  Gets the name of a cursor from hostvariable
  Arguments:
    PrecomDesc [in] Pointer to the precm descriptor
    CursorName [in|out] name of cursor
    cbLen      [in] length of ... in bytes 

  return value: UniqueID of Cursor EmptyUniqueID_cpr01 null if no UniqueID was found 
 */
static tpr01_UniqueID pr01PrecomGetCursorName(tpr01_PrecomDesc *PrecomDesc, tpr00_CursorNameDesc *CursorNameDesc)
{
  tpr01_UniqueID UniqueID = EmptyUniqueID_cpr01;
  M90TRACE(M90_TR_ENTRY, "pr01PrecomGetCursorName", 0);
  PR07_CHECKDESC(PrecomDesc, PrecomDesc_epr01);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  {
    tpr01_StmtNameDesc *StmtNameDesc=PrecomDesc->StmtNameDesc; 
    tpr01_PrecomContainer *Cont = PrecomDesc->Precom;
    sqlxatype *sqlxa = Cont->sqlxa;
    sqlprpointer pr = PrecomDesc->pr;
    M90TRACE(M90_TR_PTR, "pr", &pr);
    if (pr && pr->prCursorNameIndex > 0) 
    {
      enum tpr_runtime_errors_Enum error = cpr_p_ok;
      tsp00_Int4 len = sizeof(tsp00_KnlIdentifierc);
      M90TRACE(M90_TR_WORD, "prCursorNameIndex", &pr->prCursorNameIndex);
      len = pr01PrecomGetHostVarStringBuf(sqlxa, pr->prCursorNameIndex, &error, &(CursorNameDesc->CursorName), First_epr01);
    }
    else 
    {
      tsp00_Uint4 len=0;
      if (pr) 
      {
	    len = (tsp00_Uint4) s30klen ((char*)pr->prCursorName, bsp_c1, sizeof(tsp00_KnlIdentifier));
	    len = PR07MIN(len, CursorNameDesc->CursorName.cbMaxLen);
	    M90TRACE(M90_TR_DWORD, "len", &len);
	    if (len) 
	    {
	        tpr05_StringError erg = pr05IfCom_String_ConvertP (&(CursorNameDesc->CursorName),
                                                                   (char*)pr->prCursorName, 
                                                                   len,
                                                                   sp77encodingAscii);
	    } 
      }
      if (len == 0) 
      {
	    if (StmtNameDesc) 
	    {
	        UniqueID = pr01PrecomUniqueID(PrecomDesc);
	    }
      }
    }
    M90TRACE(M90_TR_EXIT, "pr01PrecomGetCursorName", 0);
    return UniqueID;
  }
}


/*---------------------------------------------------------------------------*/
/*!
  Function:     pr01PrecomGetDynamicSQL

  see also: 

  Description:    Copies SQl-Command for dynamic SQL
  
  Gets SQL-Statement from Hostvariable and copied it to *StmtNameDesc
  
  Arguments:
    *PrecomDesc   [in] a pointer to a tpr01_PrecomDesc which contains the  
                     SQL-Command
    *StmtNameDesc [out] a pointer to a tpr01_StmtNameDesc which gets the copy 
                        of SQL-Command

  return value: boolean
    <UL>
        <LI>TRUE : processing Okay
        <LI>FALSE: Error
    </UL>
 */
static boolean pr01PrecomGetDynamicSQL(tpr01_PrecomDesc *PrecomDesc, tpr01_StmtNameDesc *StmtNameDesc)
{
  boolean ret = FALSE;
  M90TRACE(M90_TR_ENTRY, "pr01PrecomGetDynamicSQL", 0);
  PR07_CHECKDESC(PrecomDesc, PrecomDesc_epr01);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  {
    tpr01_PrecomContainer *Cont = PrecomDesc->Precom;
    sqlxatype *sqlxa = Cont->sqlxa;
    sqlprpointer pr = PrecomDesc->pr;
    sqlkaentry *ka = StmtNameDesc->ka;
    tsp00_Int4 ind = 0;
    tpr05_String pSQLStatement;
    pSQLStatement.allocationType = constant_epr05;
    M90TRACE(M90_TR_WORD, "prstindex", &pr->prstindex);
    if (pr && pr->prstindex > 0)
      ind = pr->prstindex;
    else 
    {
      M90TRACE(M90_TR_WORD, "kaStindex", &ka->kaStindex);
      M90TRACE(M90_TR_WORD, "kaStcount", &ka->kaStcount);
      if (ka->kaStindex > 0 && ka->kaStcount == 0) 
      {
	    ind = ka->kaStindex;
      }
    }

    M90TRACE(M90_TR_DWORD, "ind", &ind);
    if (ind > 0) 
    {
      enum tpr_runtime_errors_Enum error = cpr_p_ok;
      tsp00_Int4 len;
      len = pr01PrecomGetHostVarStringBuf(sqlxa, ind, &error, &pSQLStatement, Reference_epr01);
      if (error != cpr_p_ok) 
      {
	    pr01PrecomSetError(PrecomDesc, cpr_area_overflow);
      }
      else 
      {
	    tpr01_StmtNameContainer *StmtName = StmtNameDesc->StmtName;
	    if (StmtName->PutSQL(StmtNameDesc, &pSQLStatement, First_epr01))
	    {
	        ka->kastate = cpr_state_empty;
	        ret = TRUE;
	    }
	    else
	    {
            pr01PrecomSetError(PrecomDesc, cpr_area_overflow );
	    }
      }
    }
    else 
    {
      pr01PrecomSetError(PrecomDesc, cpr_area_overflow );
    }
  }
  M90TRACE(M90_TR_EXIT, "pr01PrecomGetDynamicSQL", 0);
  M90TRACE(M90_TR_WORD, "ret", &ret);
  return ret;
}

/*---------------------------------------------------------------------------*/
/*!
  Function:     pr01PrecomGetStaticSQL

  see also: 

  Description:  Static SQL-Statements
  
  Gets the SQL-Statement from Precom Desc and makes a reference to this statement in
  StmtDesc 
    
  Arguments:
    *PrecomDesc   [in] a pointer to a tpr01_PrecomDesc which contains static 
                     SQL-Command
    *StmtNameDesc [out] a pointer to a tpr01_StmtNameDesc which gets the reference 
                        to a SQL-Command
  return value: boolean
    <UL>
        <LI>TRUE : processing succesed
        <LI>FALSE: error
    </UL>
 */
static boolean pr01PrecomGetStaticSQL(tpr01_PrecomDesc *PrecomDesc, tpr01_StmtNameDesc *StmtNameDesc)
{
  boolean ret = FALSE;
  M90TRACE(M90_TR_ENTRY, "pr01PrecomGetStaticSQL", 0);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  {
    tpr01_ModuleDesc *ModuleDesc = PrecomDesc->Precom->ModuleDesc;
    tpr01_StmtNameContainer *StmtName = ModuleDesc->StmtName;
    tpr01_StmtNameDesc *StaticStmtNameDesc;
    tpr01_UniqueID UniqueID;
    tpr00_StmtNameStruct StmtNameStruct;
    UniqueID = pr01PrecomUniqueID(PrecomDesc);
    StmtName->InitStmtNameStruct (&StmtNameStruct);
    StaticStmtNameDesc = StmtName->FindDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID);
    if (StaticStmtNameDesc) 
    {
      int erg;  
      tpr05_String *aSQLStatement = NULL;
      erg = StmtName->GetSQL(StaticStmtNameDesc, &aSQLStatement, Reference_epr01);
      erg = StmtName->PutSQL(StmtNameDesc, aSQLStatement, Reference_epr01);

      ret = TRUE;
    }
    else 
    {
      pr01PrecomSetError(PrecomDesc, cpr_area_overflow );
    }
  }
  M90TRACE(M90_TR_EXIT, "pr01PrecomGetStaticSQL", 0);
  M90TRACE(M90_TR_WORD, "ret", &ret);
  return ret;
}

/*---------------------------------------------------------------------------*/
/*!
  Function:     pr01PrecomGetSQL

  see also: 

  Description:    Gets the SQL-Command from the static sqlareas sqlst or the 
                  dynamic area sqlpr.
		  It also analyses the SQL Command by passing it to pr05cAnalyseSQLStatement.
  
  Arguments:
    *PrecomDesc   [in] a pointer to a tpr01_PrecomDesc which contains the  
                     SQL-Command
    *StmtNameDesc [out] a pointer to a tpr01_StmtNameDesc which gets the copy 
                        of SQL-Command

  return value: boolean
    <UL>
        <LI>TRUE : processing Okay
        <LI>FALSE: Error
    </UL>
 */

static boolean pr01PrecomGetSQL(tpr01_PrecomDesc *PrecomDesc, tpr01_StmtNameDesc *StmtNameDesc)
{
  boolean ret = FALSE;
  M90TRACE(M90_TR_ENTRY, "pr01PrecomGetSQL", 0);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  if (StmtNameDesc->ka->kaprindex > 0) {
    if ( PrecomDesc->pr->prarea == cpr_in_sqlva 
         && (PrecomDesc->pr->prstindex > 0 || StmtNameDesc->ka->kaStindex > 0 && StmtNameDesc->ka->kaStcount == 0))
      {
        /* Statement form sqlva area */
        ret = pr01PrecomGetDynamicSQL(PrecomDesc, StmtNameDesc);
      }
    else {
      if (StmtNameDesc->ka->kaStcount > 0) {
	ret = pr01PrecomGetStaticSQL(PrecomDesc, StmtNameDesc);
      }
    }
  }
  else {
    if (StmtNameDesc->ka->kaStcount > 0) {
      ret = pr01PrecomGetStaticSQL(PrecomDesc, StmtNameDesc);
    }
    else {
      ret = pr01PrecomGetDynamicSQL(PrecomDesc, StmtNameDesc);
    }
  }
  if (ret) {
    if (StmtNameDesc->SQLStatement->rawString) {
      /*pr05IfCom_String_toupper (StmtNameDesc->SQLStatement);*/
      pr05cAnalyseSQLStatement (StmtNameDesc->SQLStatement, &(StmtNameDesc->StmtTypeDesc));
      StmtNameDesc->State |= Analysed_bpr01;
    }
  }
  M90TRACE(M90_TR_EXIT, "pr01PrecomGetSQL", 0);
  M90TRACE(M90_TR_WORD, "ret", &ret);
  return ret;
}


/*---------------------------------------------------------------------------*/

static char *pr01PrecomGetDatabaseName(tpr01_PrecomDesc *PrecomDesc, char *szDatabaseName)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomGetDatabaseName", 0);
  PR07_CHECKDESC(PrecomDesc, PrecomDesc_epr01);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  {
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    sqlxatype *sqlxa = Precom->sqlxa;
    sqlkapointer ka = PrecomDesc->ka;
    tsp00_Int4 atno = ka->kaatindex;
    sqlatpointer at = sqlxa->sqlatp + atno - 1;
    tsp00_Int4 len;

    if (at->atdbnamix > 0) {
      enum tpr_runtime_errors_Enum error = cpr_p_ok;
      tsp00_Int4 vaind;
      tsp00_Uint1* VaAddr;

      vaind = pr01cGetVaIndex(sqlxa, at->atdbnamix, &VaAddr);
      if (vaind >= 0) {
	sqlv1pointer sqlv1p = sqlxa->sqlv1p+vaind;
	sqlv2pointer sqlv2p = sqlxa->sqlv2p+sqlv1p->sc.va1indva2-1;
	tsp00_Int4 va2size = sqlv2p->va2size;
	switch(sqlv2p->va2typ) {
	case (cpr_vbuf):{}
	case (cpr_vchar):{}
	case (cpr_vbchar): {
	  len = s30klen ((char*)VaAddr, bsp_c1, va2size);
	  if (len > NODEID_MXSP00) {
	    len = NODEID_MXSP00;
	    error = cpr_cmd_too_long;
	  }
	  memcpy(szDatabaseName, VaAddr, len);
	  szDatabaseName[len] = '\0';
	  break;
	}
	case (cpr_vstring4): {
	  memcpy(&len, VaAddr, sizeof(len));
	  if (len+4 > NODEID_MXSP00) {
	    len = NODEID_MXSP00 - 4 ;
	    error = cpr_cmd_too_long;
	  }
	  memcpy(szDatabaseName, VaAddr+4, len);
	  break;
	}
	case (cpr_vstring): {
	  tsp00_Int2 in2;
	  memcpy(&in2, VaAddr, sizeof(in2));
	  len = in2;
	  if (len+2 > NODEID_MXSP00) {
	    len = NODEID_MXSP00 - 2;
	    error = cpr_cmd_too_long;
	  }
	  memcpy(szDatabaseName, VaAddr+2, len);
	  break;
	}
	case (cpr_vstring1): {
	  len = (tsp00_Int4) VaAddr[0];
	  memcpy(szDatabaseName, VaAddr+1, len);
	  break;
	}
	case (cpr_vcharc):{}
	case (cpr_vcharz):{
	  if (va2size == cpr_pointerlen) {
	    len = NODEID_MXSP00;
	  }
	  else {
	    len = va2size;
	  }
	  len = (va2size == cpr_pointerlen) ? NODEID_MXSP00 : va2size;
	  len = s30len ((unsigned char*)VaAddr, '\0', len);
	  if ( len > NODEID_MXSP00) {
	    len = NODEID_MXSP00;
	    error = cpr_cmd_too_long;
	  }
	  memcpy(szDatabaseName, VaAddr, len);
	  break;
	}
	default: {
	  error = cpr_unknown_datatype;
	  break;
	}
	}
	if (error != cpr_p_ok) {
          p08runtimeerror(Precom->sqlca, sqlxa, error);
        }
      }
      if (error != cpr_p_ok)
	szDatabaseName = NULL;
    }
    else {
      len = s30klen ((char*)at->atdbname, bsp_c1, sizeof(tsp00_KnlIdentifier));
      memcpy(szDatabaseName, at->atdbname, len);
    }
    if (szDatabaseName)
      szDatabaseName[len] = '\0';
    M90TRACE(M90_TR_EXIT, "pr01PrecomGetDatabaseName", 0);
    return szDatabaseName;
  }
}

/*---------------------------------------------------------------------------*/
/*!
  Function:     pr01PrecomGetDBString

  see also: 
  
  Description:  Gets the oracle conform at-String for Connectivity
    
  Arguments:
    *PrecomDesc [in] a pointer to a tpr01_PrecomDesc with contains ka-area 

  return value: char* a referenz to at-String
 */

static char* pr01PrecomGetDBString(tpr01_PrecomDesc *PrecomDesc)
{
  tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
  sqlxatype *sqlxa = Precom->sqlxa;
  tpr05_String DBString;
  DBString.allocationType = constant_epr05;

  pr05IfCom_String_InitString(&DBString, NULL, 0, sp77encodingAscii,constant_epr05);
  
  if (PrecomDesc->ka->kaatindex > 0) 
  {
    sqlatpointer at = sqlxa->sqlatp + PrecomDesc->ka->kaatindex - 1;
    if (at->atdbindex > 0) 
    {
      enum tpr_runtime_errors_Enum error = cpr_p_ok;
      tsp00_Int4 len;
      len = pr01PrecomGetHostVarStringBuf(sqlxa, at->atdbindex, &error, &DBString, Reference_epr01);

      if (DBString.encodingType == sp77encodingAscii)
      {
        return DBString.rawString;
      }
      else
      {
        /* only ASCII is supported*/  
	    pr01PrecomSetError(PrecomDesc, cpr_conv_from_unicode_impossible);
        return NULL;
      }
    }
    else {
      if (at->atdbindex == 0) {
        pr05IfCom_String_InitString(&DBString, at->atdbstring, sizeof(at->atdbstring), sp77encodingAscii, constant_epr05);
      }
    }
  }
  return DBString.rawString;
}

/*---------------------------------------------------------------------------*/
/*!
  Function:     pr01PrecomGetHostVarStringBuf

  see also: 

  Description:  Gets SQLStatement from sqlxa-area an determines length and encoding type  

  NOTE: swapKind [in] wich contains swap kind of lacal maschine is taken from global
        variable sqlLocalMachineSwapKind  

  Arguments:

  *sqlxa         [in|out] pointer to sqlxa-area
  index          [in]     index of hostvariable
  *error         [in|out] errorhandler
  *pSQLStatement [in|out] pointer to a tpr05_String which gets the referenz to 
                          SQLStatement
  fOption        [in]     processing option of pSQLStatement->rawString
    <UL>
        <LI>fOption == First_epr01     => a new sqlstatement, overwrite buffer 
        <LI>fOption == Append_epr01    => next part of sqlstatement, append on buffer 
        <LI>fOption == Reference_epr01 => only Referenz to sql-statement will be stored 
    </UL>

  return value: int length of in bytes
 */
static tsp00_Int4 pr01PrecomGetHostVarStringBuf(sqlxatype *sqlxa, 
                                                tsp00_Int4 index, 
                                                enum tpr_runtime_errors_Enum *error, 
                                                tpr05_String *pSQLStatement, 
                                                tpr01_SQLStatementOptionEnum fOption)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomGetHostVarStringBuf", 0);
  {
  tsp00_Uint4 len_in_bytes=0;     /* Length of HostVarStringBuf in Bytes without null terminator*/
  tsp00_Uint1* VaAddr;
  tpr05_StringEncoding encType;
  tsp00_Int4 vaindex = pr01cGetVaIndex(sqlxa, index, &VaAddr);

  tsp00_Int2 va2typ = sqlxa->sqlv2p[sqlxa->sqlv1p[vaindex].sc.va1indva2-1].va2typ;
  tsp00_Int4 va2size = sqlxa->sqlv2p[sqlxa->sqlv1p[vaindex].sc.va1indva2-1].va2size;
  char *hostvaraddr = (char*)VaAddr;
  *error = cpr_p_ok;
  switch (va2typ) {			/* va2typ means typ of hostvariable */
  /* determine length and adress for the different string types and stores them into len,hostvaraddr   */
  case (cpr_vbuf):{}
  case (cpr_vchar):{}
  case (cpr_vbchar):{
    len_in_bytes = (tsp00_Uint4)s30klen (hostvaraddr, bsp_c1, va2size);
    encType = sp77encodingAscii;
    break;
  }
  case (cpr_vstring4): {
    tsp00_Int4 in4;
    memcpy(&in4, hostvaraddr, sizeof(in4));
    len_in_bytes = (tsp00_Uint4) in4;
    hostvaraddr += sizeof(in4);
    encType = sp77encodingAscii;
    break;
  }
  case (cpr_vstring): {
    tsp00_Int4 in2;
    memcpy(&in2, hostvaraddr, sizeof(in2));
    len_in_bytes = (tsp00_Uint4) in2;
    hostvaraddr += sizeof(in2);
    encType = sp77encodingAscii;
    break;
  }
  case (cpr_vstring1):{
    len_in_bytes = (char)hostvaraddr[1];
    hostvaraddr += sizeof(char);
    encType = sp77encodingAscii;
    break;
  }
  case (cpr_vcharc):{}
  case (cpr_vcharz):{

    len_in_bytes = (tsp00_Uint4) strlen(hostvaraddr);
    encType = sp77encodingAscii;
    break;
  }
  case (cpr_vunicode):{
  	/* unicode(UCS2)-String with constant length and fillchars*/
    len_in_bytes = (tsp00_Uint4)va2size;
    if(sqlLocalMachineSwapKind == sw_normal)
    {
        encType = sp77encodingUCS2;
    }
    else if (sqlLocalMachineSwapKind == sw_full_swapped)
    {
        encType = sp77encodingUCS2Swapped;
    }
    else
    {
        /* Error unsupported swap type */
        *error =  cpr_unknown_datatype;
       len_in_bytes = 0;
    }
    break;
  }
  case (cpr_vucs2):
  case (cpr_vutf16):
  case (cpr_vunicodec):{
  	/* unicode(UCS2)-String with terminator */
    len_in_bytes = 2 * sp81UCS2strlen((tsp81_UCS2Char*)hostvaraddr); /* a UCS2Char needs 2 byte */
    if(sqlLocalMachineSwapKind == sw_normal)
    {
        encType = sp77encodingUCS2;
    }
    else if (sqlLocalMachineSwapKind == sw_full_swapped)
    {
        encType = sp77encodingUCS2Swapped;
    }
    else
    {
       /* Error unsupported swap type */
       *error =  cpr_unknown_datatype;
       len_in_bytes = 0;
    }
    break;
  }
  default:{
    *error =  cpr_unknown_datatype;
    len_in_bytes = 0;
    break;
  }
  }
  if (*error == cpr_p_ok) 
  {
    if (fOption == Reference_epr01) 
    {
      pr05IfCom_String_InitString(pSQLStatement, hostvaraddr, len_in_bytes, encType,constant_epr05);
    }
    else 
    {
      if (len_in_bytes > pSQLStatement->cbMaxLen) 
      {
	        *error = cpr_inp_string_truncated;
	        len_in_bytes = pSQLStatement->cbMaxLen;
      }
      pSQLStatement->cbLen = len_in_bytes;
      pSQLStatement->encodingType = encType;      
      memcpy(pSQLStatement->rawString, hostvaraddr,pSQLStatement->cbLen);
      M90TRACELEN(M90_TR_STRING, "hostvaraddr", hostvaraddr, pSQLStatement->cbLen);
    }
  }
  }
  M90TRACE(M90_TR_EXIT, "pr01PrecomGetHostVarStringBuf", 0);
  M90TRACE(M90_TR_DWORD, "pSQLStatement->cbLen", &pSQLStatement->cbLen);
  return pSQLStatement->cbLen;
}

/*---------------------------------------------------------------------------*/

static tpr01_CursorDesc *pr01PrecomGetCursorDesc(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomGetCursorDesc", 0);
  PR07_CHECKDESC(PrecomDesc, PrecomDesc_epr01);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  {
    tpr00_CursorNameDesc CursorNameDesc;
    tpr01_CursorDesc *CursorDesc = NULL;
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    tpr01_CursorContainer *Cursor = ModuleDesc->Cursor;
    tpr01_UniqueID UniqueID;
    memset (CursorNameDesc.private_StrBuf, 0, sizeof(CursorNameDesc.private_StrBuf));
    Cursor->InitCursorName(&CursorNameDesc);
    UniqueID = pr01PrecomGetCursorName(PrecomDesc, &CursorNameDesc);
    M90TRACE(M90_TR_DWORD, "UniqueID", &UniqueID);
    CursorDesc = Cursor->FindDesc(Cursor, &(CursorNameDesc.CursorName), UniqueID);
    M90TRACE(M90_TR_EXIT, "pr01PrecomGetCursorDesc", 0);
    M90TRACE(M90_TR_HANDLE, "CursorDesc", &CursorDesc);
    return CursorDesc;
  }
}

/*---------------------------------------------------------------------------*/

static tpr01_StmtNameDesc *pr01PrecomGetStmtNameDesc(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomGetStmtNameDesc", 0);
  PR07_CHECKDESC(PrecomDesc, PrecomDesc_epr01);
  PR07_CHECKDESC(ModuleDesc, ModuleDesc_epr01);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  {
    tpr01_StmtNameDesc *StmtNameDesc = NULL;
    tpr00_StmtNameStruct StmtNameStruct;
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    tpr01_StmtNameContainer *StmtName = ModuleDesc->StmtName;
    tpr01_UniqueID UniqueID;
    StmtName->InitStmtNameStruct (&StmtNameStruct);
    UniqueID = pr01PrecomGetStmtName(PrecomDesc, &(StmtNameStruct.stStmtName));
    M90TRACE(M90_TR_DWORD, "UniqueID", &UniqueID);
    StmtNameDesc = StmtName->FindDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID);
    M90TRACE(M90_TR_EXIT, "pr01PrecomGetStmtNameDesc", 0);
    M90TRACE(M90_TR_HANDLE, "StmtNameDesc", &StmtNameDesc);
    return StmtNameDesc;
  }
}


/*---------------------------------------------------------------------------*/
/*!
  Function:     pr01PrecomPrepareStmtName

  see also: 

  Description:  Builds a new StmtNameDesc given by the Precom/ModuleDesc. 
  Init or create a new SQLDesc for Parsing the SQL-Command by pr01PrecomPrepare
    
  Arguments:
    *PrecomDesc   [in|out] a pointer to a tpr01_PrecomDesc 
    *ModuleDesc   [in|out] a pointer to a tpr01_ModuleDesc 
    *ConDescDesc  [in|out] a pointer to a tpr01_ConDescDesc 
  return value: void
 */

static void
pr01PrecomPrepareStmtName(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomPrepareStmtName", 0);
  PR07_CHECKDESC(PrecomDesc, PrecomDesc_epr01);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  M90TRACE(M90_TR_WORD, "kalineno", &PrecomDesc->ka->kalineno);
  {
    tpr00_StmtNameStruct StmtNameStruct;
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    tpr01_StmtNameDesc *StmtNameDesc;
    tpr01_StmtNameContainer *StmtName = ModuleDesc->StmtName;
    tpr01_UniqueID UniqueID;
    StmtName->InitStmtNameStruct (&StmtNameStruct);
    UniqueID = pr01PrecomGetStmtName(PrecomDesc, &(StmtNameStruct.stStmtName));
    M90TRACE(M90_TR_DWORD, "UniqueID", &UniqueID);
    StmtNameDesc = StmtName->FindDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID);

    if (StmtNameDesc) {
      /*init ore*/
      sqlorentry    *ore = StmtNameDesc->ore;
      if (ore){
        ore->orcolcntacc = 0;
        ore->orrescntacc = 0;
        ore->orresnamacc = 0;
        ore->oropennew = cpr_is_init;
      }  
    }
    else{
      /* Statementname noch nicht in der Liste */
      /* Neuen anlegen */
      StmtNameDesc = StmtName->AddDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID, PrecomDesc->ka);
    }
    PrecomDesc->StmtNameDesc = StmtNameDesc;
    pr01PrecomGetSQL(PrecomDesc, StmtNameDesc);
    PrecomDesc->SQLDesc = pr01PrecomMakeSQLDesc(PrecomDesc, PrecomDesc->SQLDesc, ConDesc, NULL, PrecomDesc->ComType);
  }
  M90TRACE(M90_TR_EXIT, "pr01PrecomPrepareStmtName", 0);
}

/*---------------------------------------------------------------------------*/

static tpr01_CursorDesc *
pr01PrecomDeclareCursor(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomDeclareCursor", 0);
  PR07_CHECKDESC(PrecomDesc, PrecomDesc_epr01);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  M90TRACE(M90_TR_WORD, "kalineno", &PrecomDesc->ka->kalineno);
  {
    tpr00_CursorNameDesc CursorNameDesc;
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    tpr01_CursorDesc *CursorDesc = NULL;
    tpr01_UniqueID CursorUniqueID;
    tpr01_CursorContainer *Cursor = ModuleDesc->Cursor;
    Cursor->InitCursorName(&CursorNameDesc);
    CursorUniqueID = pr01PrecomGetCursorName(PrecomDesc, &CursorNameDesc);
    if (CursorUniqueID != EmptyUniqueID_cpr01) {
      tpr01_StmtNameContainer *StmtName = ModuleDesc->StmtName;
      tpr01_StmtNameDesc *StmtNameDesc = pr01PrecomGetStmtNameDesc(PrecomDesc, ModuleDesc);
      if (!StmtNameDesc) {
	    tpr00_StmtNameStruct StmtNameStruct;
	    tpr01_UniqueID UniqueID;
	    StmtName->InitStmtNameStruct (&StmtNameStruct);
	    UniqueID = pr01PrecomGetStmtName(PrecomDesc, &(StmtNameStruct.stStmtName));
	    if (UniqueID == EmptyUniqueID_cpr01) {
	      tpr01_StmtNameContainer *StmtName = ModuleDesc->StmtName;
	      StmtNameDesc = StmtName->FindDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID);
	      if (!StmtNameDesc) {
	        StmtNameDesc = StmtName->AddDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID, PrecomDesc->ka);
	        pr01PrecomGetSQL(PrecomDesc, StmtNameDesc);
	      }
	    }
	    else {
              tpr01_PrecomContainer *Precom = PrecomDesc->Precom;              
              p08runtimeerror(Precom->sqlca, Precom->sqlxa, cpr_unknown_statement_name);
	    }
      }
      if (StmtNameDesc)
        CursorDesc = Cursor->Declare(Cursor, StmtNameDesc, ConDesc, &(CursorNameDesc.CursorName), CursorUniqueID);
    }
    M90TRACE(M90_TR_EXIT, "pr01PrecomDeclareCursor", 0);
    M90TRACE(M90_TR_HANDLE, "CursorDesc", &CursorDesc);
    return CursorDesc;
  }
}

/*---------------------------------------------------------------------------*/

static void 
pr01PrecomDescribe(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomDescribe", 0);
  PR07_CHECKDESC(PrecomDesc, PrecomDesc_epr01);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  M90TRACE(M90_TR_WORD, "kalineno", &PrecomDesc->ka->kalineno);
  {
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    tpr01_SQLDesc *SQLDesc=NULL;
    tpr00_StmtNameStruct StmtNameStruct;
    tpr01_StmtNameContainer *StmtName = ModuleDesc->StmtName;
    tpr01_CursorContainer *Cursor = ModuleDesc->Cursor;
    tpr01_UniqueID UniqueID;
    StmtName->InitStmtNameStruct (&StmtNameStruct);
    if (ConDesc) {
      tpr01_ConContainer *Connection = ConDesc->Connection;
      tpr01_StmtNameDesc *StmtNameDesc = NULL;
      ConDesc->Connection->SetAttr(ConDesc, ActualSession_epr01Con, ConDesc, 0);
      if (!Connection->IsConnected(ConDesc)) {
        tpr00_StmtNameStruct StmtNameStruct;
        tpr01_StmtNameContainer *StmtName = ModuleDesc->StmtName;
        tpr01_UniqueID UniqueID;
        StmtName->InitStmtNameStruct (&StmtNameStruct);
        UniqueID = pr01PrecomGetStmtName(PrecomDesc, &(StmtNameStruct.stStmtName));
        /*M90TRACE(M90_TR_STRING, "szStmtName", szStmtName);*/
        StmtNameDesc = StmtName->FindDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID);
        if (pr01PrecomConnect(PrecomDesc, ConDesc, StmtNameDesc)) {
          /* autoconnect was running */
          pr01PrecomDescribe(PrecomDesc, ModuleDesc, ConDesc);
        }
      }
      else {
        UniqueID = pr01PrecomGetStmtName(PrecomDesc, &(StmtNameStruct.stStmtName));
        /*M90TRACE(M90_TR_STRING, "szStmtName", szStmtName);*/
        StmtNameDesc = StmtName->FindDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID);

        /* Statementname noch nicht in der Liste */
        if (!StmtNameDesc) {
          /* Neuen anlegen */
          StmtNameDesc = StmtName->AddDesc(StmtName,&(StmtNameStruct.stStmtName), UniqueID, PrecomDesc->ka);
        }
        PrecomDesc->StmtNameDesc = StmtNameDesc;
        SQLDesc = pr01PrecomMakeSQLDesc(PrecomDesc, PrecomDesc->SQLDesc, ConDesc, NULL, PrecomDesc->ComType);
        PrecomDesc->SQLDesc = SQLDesc;
        SQLDesc->ka->kapacount = PrecomDesc->ka->kapacount;
        
        if (   SQLDesc->StmtNameDesc->StmtTypeDesc.CursorPos > 0
            && (  SQLDesc->StmtNameDesc->StmtTypeDesc.CursorType == CPR_S_IDENTIFIER
                ||SQLDesc->StmtNameDesc->StmtTypeDesc.CursorType == CPR_S_STRING_LITERAL)){
          /*fetch command with cursor name which doesn't come from hostvar*/    
          tpr05_String *pCursorName = NULL;
          tpr05_String CursorName;
          CursorName.allocationType = constant_epr05;
          pCursorName = &CursorName;
          pr05IfCom_String_InitString(pCursorName,
                                      &SQLDesc->StmtNameDesc->SQLStatement->rawString[SQLDesc->StmtNameDesc->StmtTypeDesc.CursorPos-1],
                                      SQLDesc->StmtNameDesc->StmtTypeDesc.CursorLength,
                                      SQLDesc->StmtNameDesc->SQLStatement->encodingType,
                                      constant_epr05);
          SQLDesc->CursorDesc = Cursor->FindDesc(Cursor, pCursorName, 0);
        }
        
        SQLDesc->SQL->Describe(SQLDesc);
        pr01PrecomAfterExecute(PrecomDesc, ModuleDesc, ConDesc);
      }
    }
  }
  M90TRACE(M90_TR_EXIT, "pr01PrecomDescribe", 0);
}
/*---------------------------------------------------------------------------*/

static void
pr01PrecomOpen(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc )
{
  M90TRACE(M90_TR_ENTRY,  "pr01PrecomOpen", 0);
  M90TRACE(M90_TR_HANDLE, "PrecomDesc", &PrecomDesc);
  PR07_CHECKDESC(PrecomDesc, PrecomDesc_epr01);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  M90TRACE(M90_TR_WORD, "kalineno", &PrecomDesc->ka->kalineno);
  {
    if (ConDesc) {
      tpr01_ConContainer *Connection = ConDesc->Connection;
      tpr01_StmtNameDesc *StmtNameDesc = NULL;
      ConDesc->Connection->SetAttr(ConDesc, ActualSession_epr01Con, ConDesc, 0);
      if (!Connection->IsConnected(ConDesc)) {
  	    tpr00_StmtNameStruct StmtNameStruct;
	    tpr01_StmtNameContainer *StmtName = ModuleDesc->StmtName;
	    tpr01_UniqueID UniqueID;
	    StmtName->InitStmtNameStruct (&StmtNameStruct);
	    UniqueID = pr01PrecomGetStmtName(PrecomDesc,&(StmtNameStruct.stStmtName));
	    /*M90TRACE(M90_TR_STRING, "szStmtName", szStmtName);*/
	    StmtNameDesc = StmtName->FindDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID);
            if (pr01PrecomConnect(PrecomDesc, ConDesc, StmtNameDesc)) {
	      /* autoconnect was running */
	      /* now execute the Statement */
	      pr01PrecomOpen(PrecomDesc, ModuleDesc, ConDesc);
	    }
      }
      else {
	    tpr01_SQLDesc *SQLDesc;
	    if ((PrecomDesc->ComType == cpr_com_sql_open) ||
	    (PrecomDesc->ComType == cpr_com_sql_open_keep)) {
	      tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
	      tpr01_CursorDesc *CursorDesc = pr01PrecomGetCursorDesc(PrecomDesc, ModuleDesc);
          if (CursorDesc) {
	        tpr01_CursorContainer *Cursor = CursorDesc->Cursor;
	        M90TRACE(M90_TR_HANDLE,  "CursorDesc", &CursorDesc);
	        if (PrecomDesc->ComType == cpr_com_sql_open_keep) {
	          PrecomDesc->ComType = cpr_com_sql_open;
	        }
	        Cursor->PrepareOpen(CursorDesc, PrecomDesc->ka);
	        SQLDesc = pr01PrecomMakeSQLDesc(PrecomDesc, PrecomDesc->SQLDesc, ConDesc, CursorDesc, PrecomDesc->ComType);
	        PrecomDesc->SQLDesc = SQLDesc;
	        SQLDesc->SQL->Open(SQLDesc);
	        pr01PrecomAfterExecute(PrecomDesc, ModuleDesc, ConDesc);
	      }
	      else {
                tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
                p08runtimeerror(Precom->sqlca, Precom->sqlxa, cpr_unknown_cursor_name);
	      }
	    }
	    else {
	      /* exec sql execute <stmtname> */
	      tpr01_StmtNameDesc *StmtNameDesc;
	      tpr01_StmtNameContainer *StmtName = ModuleDesc->StmtName;
	      tpr00_StmtNameStruct StmtNameStruct;
	      tpr01_UniqueID UniqueID;
	      StmtName->InitStmtNameStruct (&StmtNameStruct);
	      UniqueID = pr01PrecomGetStmtName(PrecomDesc, &(StmtNameStruct.stStmtName));
	      /*M90TRACE(M90_TR_STRING, "szStmtName", szStmtName);*/
	      StmtNameDesc = StmtName->FindDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID);

	      if (StmtNameDesc) {
	        PrecomDesc->StmtNameDesc = StmtNameDesc;
	        if (StmtNameDesc->ka->kapacount < 0)
	          StmtNameDesc->ka->kapacount = -1;
	        StmtNameDesc->ka->kapacount = PrecomDesc->ka->kapacount;
	        StmtNameDesc->ka->kapaindex = PrecomDesc->ka->kapaindex;
	        SQLDesc = pr01PrecomMakeSQLDesc(PrecomDesc, PrecomDesc->SQLDesc, ConDesc, NULL, PrecomDesc->ComType);
	        PrecomDesc->SQLDesc = SQLDesc;
	        SQLDesc->SQL->Open(SQLDesc);
	        pr01PrecomAfterExecute(PrecomDesc, ModuleDesc, ConDesc);
	      }
	      else {
                tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
                p08runtimeerror(Precom->sqlca, Precom->sqlxa, cpr_unknown_statement_name);
	      }
	    }
      }
    }
    else {
      pr07CheckAssert(true);
    }
  }
  M90TRACE(M90_TR_EXIT, "pr01PrecomOpen", 0);
}

/*---------------------------------------------------------------------------*/

static void
pr01PrecomTryExecute(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomTryExecute", 0);
  M90TRACE(M90_TR_HANDLE, "PrecomDesc", &PrecomDesc);
  M90TRACE(M90_TR_HANDLE, "ConDesc", &ConDesc);
  PR07_CHECKDESC(PrecomDesc, PrecomDesc_epr01);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  M90TRACE(M90_TR_WORD, "kalineno", &PrecomDesc->ka->kalineno);
  {
    if (ConDesc) {
      tpr01_ConContainer    *Connection = ConDesc->Connection;
      tpr01_PrecomContainer     *Precom = PrecomDesc->Precom;
      tpr01_SQLDesc            *SQLDesc = PrecomDesc->SQLDesc;
      tpr01_CursorDesc      *CursorDesc = NULL;
      tpr01_StmtNameContainer *StmtName = ModuleDesc->StmtName;
      tpr00_StmtNameStruct StmtNameStruct;
      tpr01_StmtNameDesc  *StmtNameDesc = NULL;
      tpr01_UniqueID UniqueID;
      StmtName->InitStmtNameStruct (&StmtNameStruct);
      ConDesc->Connection->SetAttr(ConDesc, ActualSession_epr01Con, ConDesc, 0);
      if (!Connection->IsConnected(ConDesc)) {
        UniqueID = pr01PrecomGetStmtName(PrecomDesc, &(StmtNameStruct.stStmtName));
	    /*M90TRACE(M90_TR_STRING, "szStmtName", szStmtName);*/
	    StmtNameDesc = StmtName->FindDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID);
	    if (pr01PrecomConnect(PrecomDesc, ConDesc, StmtNameDesc)) {
	      /* autoconnect was running */
	      /* now execute the Statement */
	      pr01PrecomTryExecute(PrecomDesc, ModuleDesc, ConDesc);
	    }
      }
      else {
	    UniqueID = pr01PrecomGetStmtName(PrecomDesc, &(StmtNameStruct.stStmtName));
	    /*M90TRACE(M90_TR_STRING, "szStmtName", szStmtName);*/
	    StmtNameDesc = StmtName->FindDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID);
	    if (!StmtNameDesc) {
	      StmtNameDesc = StmtName->AddDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID, PrecomDesc->ka);
	      pr01PrecomGetSQL(PrecomDesc, StmtNameDesc);
	    }
	    if (!SQLDesc) {
	      tpr01_SQLContainer *SQL = Precom->SQL;
	      SQLDesc = SQL->AddDesc(SQL);
	    }
 	    if (SQLDesc) {
	      tpr01_SQLContainer *SQL = SQLDesc->SQL;
	      SQL->InitDesc(SQLDesc, Precom->sqlxa);
	      PrecomDesc->SQLDesc = SQLDesc;
	      SQLDesc->ComType = PrecomDesc->ComType;
	      SQLDesc->ConDesc = ConDesc;
	  
	      if (StmtNameDesc) {
	        SQLDesc->StmtNameDesc = StmtNameDesc;
	        SQLDesc->ka  = StmtNameDesc->ka;
	        SQLDesc->ore = StmtNameDesc->ore;
	        SQLDesc->cu  = StmtNameDesc->cue;
	      }
	      if (StmtNameDesc->State & Analysed_bpr01) {
	        if ( PrecomDesc->ka->katyp == cpr_com_declare_cursor
		        || PrecomDesc->ka->katyp == cpr_com_sql_fetch
		        || PrecomDesc->ka->katyp == cpr_com_sql_putval
		        || PrecomDesc->ka->katyp == cpr_com_sql_getval
		        || PrecomDesc->ka->katyp == cpr_com_sql_close
		        || StmtNameDesc->StmtTypeDesc.StmtType == KeywordDECLARE_epr01
		        || StmtNameDesc->StmtTypeDesc.StmtType == KeywordFETCH_epr01
		        || StmtNameDesc->StmtTypeDesc.StmtType == KeywordCLOSE_epr01
		       ) {
	          tpr00_CursorNameDesc CursorNameDesc;
	          tpr01_CursorContainer *Cursor = ModuleDesc->Cursor;
	          tpr01_UniqueID UniqueID;
	          Cursor->InitCursorName(&CursorNameDesc);

                  UniqueID = pr01PrecomGetCursorName(PrecomDesc, &CursorNameDesc);
	          if (CursorNameDesc.CursorName.cbLen != 0) {
	            CursorDesc = Cursor->FindDesc(Cursor, &(CursorNameDesc.CursorName), UniqueID);
	          }
	          if (CursorDesc && PrecomDesc->ComType == cpr_com_declare_cursor) {
		        Cursor->Close(CursorDesc);
		        CursorDesc = NULL;
	          }
	          SQLDesc->CursorDesc = CursorDesc;
	       }
	       if (CursorDesc) {
	         switch(StmtNameDesc->StmtTypeDesc.StmtType) {
	           case (KeywordDECLARE_epr01): {}
	           case (KeywordSELECT_epr01): {}
	           case (KeywordDESCRIBE_epr01): {}
	           case (KeywordSHOW_epr01): {
		           SQLDesc->ka = CursorDesc->OpenKa;
		           SQLDesc->cu = SQLDesc->CursorDesc->cue;
		           break;
  	         }
  	         case (KeywordFETCH_epr01): {
  		         if (!CursorDesc->FetchKa){
  		           tpr01_StmtNameContainer *StmtName = StmtNameDesc->StmtName;
  		           tpr01_CursorContainer *Cursor = CursorDesc->Cursor;
  		           StmtName->DropParsId(StmtNameDesc, SQLDesc);
  		           if(CursorDesc->FetchKa) 	    	/* PTS 1107213 */
  		           {
  			           pr01cFreeKa(CursorDesc->FetchKa);
  			           CursorDesc->FetchKa = NULL;
  		           }
  		           Cursor->PrepareFetch(CursorDesc, StmtNameDesc, true);
  		           SQLDesc->CursorDesc = CursorDesc;
  		           SQLDesc->ka = CursorDesc->FetchKa;
  		         }
  		         SQLDesc->ka = CursorDesc->FetchKa;
  		         SQLDesc->cu = CursorDesc->cue;
  		         SQLDesc->pr = PrecomDesc->pr;
                         SQLDesc->PacketEncoding = CursorDesc->StmtNameDesc->SQLStatement->encodingType;
  		         break;
  	         }
  	         case (KeywordCLOSE_epr01): {
  		         if (!CursorDesc->CloseKa) {
  		           tpr01_StmtNameContainer *StmtName = StmtNameDesc->StmtName;
  		           tpr01_CursorContainer *Cursor = CursorDesc->Cursor;
  		           StmtName->DropParsId(StmtNameDesc, SQLDesc);
  		           Cursor->PrepareClose(CursorDesc, StmtNameDesc->ka);
  		           SQLDesc->CursorDesc = CursorDesc;
  		           SQLDesc->ka = CursorDesc->CloseKa;
  		           SQLDesc->cu = CursorDesc->cue;
  		         }
  		         break;
  	         }
	         }/*end switch*/
	      }/*end if cursor desc*/
	    }
	    if (SQL->TryExecute(SQLDesc)) {
	      pr01PrecomAfterExecute(PrecomDesc, ModuleDesc, ConDesc);
	    }
	  }
	  else {
	  pr01PrecomSetError(PrecomDesc, cpr_area_overflow );
	}
      }
    }
  }
  M90TRACE(M90_TR_EXIT, "pr01PrecomTryExecute", 0);
}

/*---------------------------------------------------------------------------*/

static void
pr01PrecomExecute(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomExecute", 0);
  M90TRACE(M90_TR_HANDLE, "PrecomDesc", &PrecomDesc);
  M90TRACE(M90_TR_HANDLE, "ConDesc", &ConDesc);
  PR07_CHECKDESC(PrecomDesc, PrecomDesc_epr01);
  M90TRACE(M90_TR_WORD, "kalineno", &PrecomDesc->ka->kalineno);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  {
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    tpr01_StmtNameDesc *StmtNameDesc = NULL;
    M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
    if (ConDesc) {
      tpr01_ConContainer *Connection = ConDesc->Connection;
      ConDesc->Connection->SetAttr(ConDesc, ActualSession_epr01Con, ConDesc, 0);
      if (!Connection->IsConnected(ConDesc)) {
	tpr01_StmtNameContainer *StmtName = ModuleDesc->StmtName;
	tpr00_StmtNameStruct StmtNameStruct;
	tpr01_UniqueID UniqueID;
	StmtName->InitStmtNameStruct (&StmtNameStruct);
	UniqueID = pr01PrecomGetStmtName(PrecomDesc, &(StmtNameStruct.stStmtName));
	/*M90TRACE(M90_TR_STRING, "szStmtName", szStmtName);*/
	StmtNameDesc = StmtName->FindDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID);
	PrecomDesc->StmtNameDesc = StmtNameDesc;
	pr01PrecomGetSQL(PrecomDesc, StmtNameDesc);
	PrecomDesc->SQLDesc = pr01PrecomMakeSQLDesc(PrecomDesc, PrecomDesc->SQLDesc, ConDesc, NULL, PrecomDesc->ComType);
	if (pr01PrecomConnect(PrecomDesc, ConDesc, StmtNameDesc)) {
	  /* now execute the Statement */
	  pr01PrecomExecute(PrecomDesc, ModuleDesc, ConDesc);
	}
      }
      else {
	tpr01_StmtNameContainer *StmtName = ModuleDesc->StmtName;
	tpr00_StmtNameStruct StmtNameStruct;
	tpr01_UniqueID UniqueID;
	StmtName->InitStmtNameStruct (&StmtNameStruct);
	UniqueID = pr01PrecomGetStmtName(PrecomDesc, &(StmtNameStruct.stStmtName));
	/*M90TRACE(M90_TR_STRING, "szStmtName", szStmtName);*/
	StmtNameDesc = StmtName->FindDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID);
	PrecomDesc->StmtNameDesc = StmtNameDesc;
	if (PrecomDesc->SQLDesc ) {
	  tpr01_SQLDesc *SQLDesc = PrecomDesc->SQLDesc;
	  tpr01_SQLContainer *SQL = SQLDesc->SQL;
	  if (StmtNameDesc) {
	    SQLDesc->StmtNameDesc = StmtNameDesc;
	    pr01PrecomGetSQL(PrecomDesc, StmtNameDesc);
	    SQLDesc->ka = StmtNameDesc->ka;
	    SQLDesc->ore = StmtNameDesc->ore;
	    SQLDesc->cu = StmtNameDesc->cue;
	    SQLDesc->pr = PrecomDesc->pr;
	  }
	  else {
	    pr07CheckAssert(true);
	  }
	  if (PrecomDesc->ka->katyp == cpr_com_declare_cursor
	      || StmtNameDesc->StmtTypeDesc.StmtType == KeywordDECLARE_epr01) {
	    tpr00_CursorNameDesc CursorNameDesc;
	    tpr01_CursorContainer *Cursor = ModuleDesc->Cursor;
	    tpr01_CursorDesc *CursorDesc = NULL;
	    tpr01_UniqueID UniqueID;
            Cursor->InitCursorName(&CursorNameDesc);
            UniqueID = pr01PrecomGetCursorName(PrecomDesc, &CursorNameDesc);
            if (CursorNameDesc.CursorName.cbLen != 0) {
	      CursorDesc = Cursor->FindDesc(Cursor, &(CursorNameDesc.CursorName), UniqueID);
	      if (CursorDesc == NULL)
		CursorDesc = Cursor->AddDesc(Cursor, &(CursorNameDesc.CursorName), UniqueID, ConDesc);
	    }
	    CursorDesc = Cursor->Declare(Cursor, StmtNameDesc, ConDesc, &(CursorNameDesc.CursorName), UniqueID);
	  }
	  else {
	    switch (StmtNameDesc->StmtTypeDesc.StmtType) {
	    case (KeywordFETCH_epr01) : {
              tpr00_CursorNameDesc CursorNameDesc;
	      tpr01_CursorContainer *Cursor = ModuleDesc->Cursor;
	      tpr01_UniqueID UniqueID;
	      Cursor->InitCursorName(&CursorNameDesc);
              UniqueID = pr01PrecomGetCursorName(PrecomDesc, &CursorNameDesc);
              SQLDesc->CursorDesc = Cursor->FindDesc(Cursor, &(CursorNameDesc.CursorName), UniqueID);
	      if (SQLDesc->CursorDesc) {
		Cursor->PrepareFetch(SQLDesc->CursorDesc, StmtNameDesc, false);
		SQLDesc->ka = SQLDesc->CursorDesc->FetchKa;
		SQLDesc->cu = SQLDesc->CursorDesc->cue;
                SQLDesc->PacketEncoding = SQLDesc->CursorDesc->StmtNameDesc->SQLStatement->encodingType;
	      }
	      if (SQL->Execute(SQLDesc)) {
		pr01PrecomAfterExecute(PrecomDesc, ModuleDesc, ConDesc);
	      }
	      break;
	    }
            case (KeywordPUTVAL_epr01) : {}
	    case (KeywordGETVAL_epr01) : {
              tpr00_CursorNameDesc CursorNameDesc;
	      tpr01_CursorContainer *Cursor = ModuleDesc->Cursor;
	      tpr01_UniqueID UniqueID;
	      Cursor->InitCursorName(&CursorNameDesc);
              UniqueID = pr01PrecomGetCursorName(PrecomDesc, &CursorNameDesc);
	      SQLDesc->CursorDesc = Cursor->FindDesc(Cursor, &(CursorNameDesc.CursorName), UniqueID);
	      if (SQLDesc->CursorDesc) {
		SQLDesc->PacketEncoding = SQLDesc->CursorDesc->StmtNameDesc->SQLStatement->encodingType;
	      }
	      if (SQL->Execute(SQLDesc)) {
		pr01PrecomAfterExecute(PrecomDesc, ModuleDesc, ConDesc);
	      }
	      break;
	    } 
	    case (KeywordCLOSE_epr01) : {
              tpr00_CursorNameDesc CursorNameDesc;
	      tpr01_CursorContainer *Cursor = ModuleDesc->Cursor;
	      tpr01_UniqueID UniqueID;
	      Cursor->InitCursorName(&CursorNameDesc);
              UniqueID = pr01PrecomGetCursorName(PrecomDesc, &CursorNameDesc);
	      SQLDesc->CursorDesc = Cursor->FindDesc(Cursor, &(CursorNameDesc.CursorName), UniqueID);
	      if (SQLDesc->CursorDesc) {
		Cursor->PrepareClose(SQLDesc->CursorDesc, StmtNameDesc->ka);
		SQLDesc->ka = SQLDesc->CursorDesc->CloseKa;
		SQLDesc->cu = SQLDesc->CursorDesc->cue;
	      }
	      if (SQL->Execute(SQLDesc)) {
		pr01PrecomAfterExecute(PrecomDesc, ModuleDesc, ConDesc);
	      }
	      break;
	    }
	    default : { 
	      PR07_HEAPCHECK(NULL);
	      if (SQL->Execute(SQLDesc))
		pr01PrecomAfterExecute(PrecomDesc, ModuleDesc, ConDesc);
	      PR07_HEAPCHECK(NULL);
	      break;
	    }
	    }
	  }
	}
	else
	  pr07CheckAssert(true);
      }
    }
    else {
      pr07CheckAssert(true);
    }
  }
  M90TRACE(M90_TR_EXIT, "pr01PrecomExecute", 0);
}

/*---------------------------------------------------------------------------*/

static tpr01_ConContainer *pr01PrecomGetConnectionCont(tpr01_PrecomDesc *PrecomDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomGetConnectionCont", 0);
  {
    tpr01_PrecomContainer *Precom=PrecomDesc->Precom;
    tpr01_EnvDesc *EnvDesc = Precom->sqlca->sqlEnvDesc;
    tpr01_EnvContainer *Env = EnvDesc->Env;
    tpr01_ConContainer *Connection = Env->GetConnectionCont(EnvDesc);
    M90TRACE(M90_TR_EXIT, "pr01PrecomGetConnectionCont", 0);
    M90TRACE(M90_TR_HANDLE, "Connection", &Connection);
    return(Connection);
  }
}

/*---------------------------------------------------------------------------*/

static tpr01_ConDesc *pr01PrecomGetFromKa(tpr01_PrecomDesc *PrecomDesc, tsp00_Int4 nSession)
{
  tpr01_ConDesc *ConDesc = NULL;
  tpr00_DatabaseName szDatabaseName;
  tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
  tpr01_ConContainer *Connection = pr01PrecomGetConnectionCont(PrecomDesc);
  epr01Con_SessionType SessionType;
  
  if (nSession == 0)
    SessionType = SessionTemp_epr01Con;
  else if (  Connection->SessionCnt == 0
           ||PrecomDesc->ka->kaatindex <= 1)
    SessionType = SessionPrimary_epr01Con;  
  else    
    SessionType = SessionDefault_epr01Con;  
      
  if (PrecomDesc->ka->kaatindex == 0) {
    sprintf(szDatabaseName, ":%d", nSession);
  }
  else {
    sqlxatype *sqlxa = Precom->sqlxa;
    sqlatpointer at = sqlxa->sqlatp + PrecomDesc->ka->kaatindex - 1;
    if (at->atdbnamix == -1) {
      sprintf(szDatabaseName, ":%d", nSession);
    }
    else {
      pr01PrecomGetDatabaseName(PrecomDesc, szDatabaseName);
    }
  }
  M90TRACE(M90_TR_STRING, "szDatabaseName", szDatabaseName);
  ConDesc = Connection->FindDesc(Connection, szDatabaseName, SessionType);
  if (!ConDesc) {
    /* Neuen anlegen */
    ConDesc = Connection->AddDesc(Connection, szDatabaseName, Precom->sqlxa, SessionType);
    ConDesc->ga->ganolog = Connection->NoLog;
  }
  return ConDesc;
}

/*---------------------------------------------------------------------------*/

static tpr01_ConDesc *
pr01PrecomGetConnection(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tsp00_Int4 nSession)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomGetConnection", 0);
  M90TRACE(M90_TR_HANDLE, "PrecomDesc", &PrecomDesc);
  PR07_CHECKDESC(PrecomDesc, PrecomDesc_epr01);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  M90TRACE(M90_TR_WORD, "kalineno", &PrecomDesc->ka->kalineno);
  {
    tpr01_ConDesc *ConDesc = NULL;
      switch(PrecomDesc->ComType) {
      case (cpr_com_sql_fetch): {}
      case (cpr_com_sql_open): {}
      case (cpr_com_sql_putval): {}
      case (cpr_com_sql_getval): {}
      case (cpr_com_sql_close): {
        tpr01_CursorDesc *CursorDesc = pr01PrecomGetCursorDesc(PrecomDesc, ModuleDesc);
        if (CursorDesc) {
	      tpr01_CursorContainer *Cursor = CursorDesc->Cursor;
	      ConDesc = Cursor->GetConnection(CursorDesc);
        }
        if (!ConDesc) {
	     ConDesc = pr01PrecomGetFromKa(PrecomDesc, nSession);
        }
        break;
      }
      default: {
        ConDesc = pr01PrecomGetFromKa(PrecomDesc, nSession);
        break;
      }
    }
    /*update the sqlxa pointer, because maybe commands comes from different
      modules and will be executed on the same connection. PTS 1109252*/
    ConDesc->sqlxa = ModuleDesc->sqlxa;
    M90TRACE(M90_TR_EXIT, "pr01PrecomGetConnection", 0);
    M90TRACE(M90_TR_HANDLE, "ConDesc", &ConDesc);
    M90TRACE(M90_TR_ENUM, "Status", &ConDesc->Status);
    M90TRACE(M90_TR_PTR, "ConDesc->ga", &ConDesc->ga);
    M90TRACE(M90_TR_DWORD, "gareference", &ConDesc->ga->gareference);
    return ConDesc;
  }
}

/*---------------------------------------------------------------------------*/
/*!
  Function:     pr01PrecomConnect

  see also: 

  Description:  Connects to the database. 
  
    
  Arguments:
    *PrecomDesc   [in|out] a pointer to a tpr01_PrecomDesc 
    *ModuleDesc   [in|out] a pointer to a tpr01_ModuleDesc 
    *ConDescDesc  [in|out] a pointer to a tpr01_ConDescDesc 

  return value: TRUE if an implicite connect was running
                FALSE if an explicite connect was running 
		      or no SQL Stmt was bind no connect was made. 
 */

static boolean pr01PrecomConnect(tpr01_PrecomDesc *PrecomDesc, tpr01_ConDesc *ConDesc, tpr01_StmtNameDesc *StmtNameDesc)
{
  boolean ret = FALSE;
  M90TRACE(M90_TR_ENTRY, "pr01PrecomConnect", 0);
  M90TRACE(M90_TR_HANDLE, "PrecomDesc", &PrecomDesc);
  M90TRACE(M90_TR_HANDLE, "ConDesc", &ConDesc);
  PR07_CHECKDESC(PrecomDesc, PrecomDesc_epr01);
  PR07_CHECKDESC(ConDesc, ConDesc_epr01);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  M90TRACE(M90_TR_WORD, "kalineno", &PrecomDesc->ka->kalineno);
  {
    sqlxatype *sqlxa = PrecomDesc->Precom->sqlxa;
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    tpr01_ConContainer *Connection = ConDesc->Connection;
    Connection->Open(ConDesc, PrecomDesc->ka);
    if (PrecomDesc->ComType == cpr_com_sql_conn
        || (StmtNameDesc && StmtNameDesc->StmtTypeDesc.StmtType == KeywordCONNECT_epr01)) {
      if (PrecomDesc->SQLDesc) {
	char *szDBString=NULL;
	char *szNetworkName=NULL;
	char *szServerNode=NULL;
	char *szServerName=NULL;
	szDBString = pr01PrecomGetDBString(PrecomDesc);
	PrecomDesc->SQLDesc = pr01PrecomMakeSQLDesc(PrecomDesc, PrecomDesc->SQLDesc, ConDesc, NULL, cpr_com_sql_conn );
	if (szDBString) {
	  char *szTmp;
	  szTmp = pr03mAllocat((tsp00_Int4)strlen(szDBString)+1, "szTmp");
	  strcpy(szTmp, szDBString);
	  pr02ConParseDBString(szTmp, &szNetworkName, &szServerNode, &szServerName);
	  Connection->SetAttr(ConDesc, ServerNode_epr01Con, szServerNode, CPR_NTS);
	  Connection->SetAttr(ConDesc, ServerName_epr01Con, szServerName, CPR_NTS);
	  pr03mFree(szTmp, "szTmp");
	}
	Connection->Connect(ConDesc, StmtNameDesc);
	sqlxa->xacmdinit = sqlIsFalse;
	ret = FALSE;
      }
      else {
	sqlxa->xacmdinit = sqlIsTrue;
	ret = FALSE;
      }
    }
    else {
      if (ConDesc->Connection->OldStyle &&
          ConDesc->SessionType == SessionPrimary_epr01Con) {
        PrecomDesc->SQLDesc = pr01PrecomMakeSQLDesc(PrecomDesc, PrecomDesc->SQLDesc, ConDesc, NULL, cpr_com_sql_conn);
        Connection->AutoConnect(ConDesc);
        ret = TRUE;
      }
      else {
        PrecomDesc->SQLDesc = pr01PrecomMakeSQLDesc(PrecomDesc, PrecomDesc->SQLDesc, ConDesc, NULL, PrecomDesc->ComType);
        if (ConDesc->Connection->OldStyle) {
          pr01PrecomRuntimeError(PrecomDesc, ConDesc, cpr_no_session_connected);
        }
        else {
          pr01PrecomRuntimeError(PrecomDesc, ConDesc, cpr_session_not_connected);
        }
      }
    }
  }
  if (PrecomDesc->Precom->sqlca->sqlcode != 0) {
    ret = FALSE;
  }
  M90TRACE(M90_TR_EXIT, "pr01PrecomConnect", 0);
  M90TRACE(M90_TR_BOOL, "ret", &ret);
  return ret;
}

/*---------------------------------------------------------------------------*/

static tsp00_Int4 pr01PrecomGetError(tpr01_PrecomDesc *PrecomDesc, tpr01_ErrorDesc *ErrorDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomGetError", 0);
  M90TRACE(M90_TR_HANDLE, "PrecomDesc", &PrecomDesc);
  M90TRACE(M90_TR_HANDLE, "ErrorDesc", &ErrorDesc);
  PR07_CHECKDESC(PrecomDesc, PrecomDesc_epr01);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  {
    sqlcatype *sqlca = PrecomDesc->Precom->sqlca;
    tsp00_Int4 cbLen;
    tsp00_Int4 cbErrorCode;
    ErrorDesc->Error->GetError(ErrorDesc, &cbErrorCode, sqlca->sqlerrmc, &cbLen, sizeof(sqlca->sqlerrmc));
    sqlca->sqlerrml = cbLen;
    sqlca->sqlcode = cbErrorCode;
    M90TRACE(M90_TR_EXIT, "pr01PrecomGetError", 0);
    M90TRACE(M90_TR_DWORD, "cbErrorCode", &cbErrorCode);
    return cbErrorCode;
  }
}

/*---------------------------------------------------------------------------*/

static tsp00_Int4 pr01PrecomSetError(tpr01_PrecomDesc *PrecomDesc, tpr_runtime_errors_Enum ErrorEnum)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomSetError", 0);
  M90TRACE(M90_TR_HANDLE, "PrecomDesc", &PrecomDesc);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  {
    tsp00_Int4 ErrorCode;
    tpr01_ErrorText szErrorText;
    pr01ErrorGetErrorText(ErrorEnum, &ErrorCode, szErrorText);
    PrecomDesc->ErrorDesc->Error->SetError(PrecomDesc->ErrorDesc, ErrorCode, (char*)szErrorText, CPR_NTS);
    M90TRACE(M90_TR_EXIT, "pr01PrecomSetError", 0);
    return ErrorCode;
  }
}

/*---------------------------------------------------------------------------*/

static tpr01_SQLDesc *pr01PrecomMakeSQLDesc(tpr01_PrecomDesc *PrecomDesc, tpr01_SQLDesc *SQLDesc, tpr01_ConDesc *ConDesc, tpr01_CursorDesc *CursorDesc, tpr00_ComTypeEnum ComType )
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomMakeSQLDesc", 0);
  M90TRACE(M90_TR_HANDLE, "PrecomDesc", &PrecomDesc);
  M90TRACE(M90_TR_HANDLE, "SQLDesc", &SQLDesc);
  M90TRACE(M90_TR_HANDLE, "ConDesc", &ConDesc);
  M90TRACE(M90_TR_HANDLE, "CursorDesc", &CursorDesc);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  {
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    tpr01_SQLContainer *SQL = Precom->SQL;
    if (!SQLDesc) {
      SQLDesc = SQL->AddDesc(SQL);
    }
    SQL->InitDesc(SQLDesc, Precom->sqlxa);
    SQLDesc->ConDesc = ConDesc;
    if (PrecomDesc->StmtNameDesc) {
      SQLDesc->ka = PrecomDesc->StmtNameDesc->ka;
      SQLDesc->ore = PrecomDesc->StmtNameDesc->ore;
      SQLDesc->cu = PrecomDesc->StmtNameDesc->cue;
    }
    SQLDesc->pr = PrecomDesc->pr;
    M90TRACE(M90_TR_INT, "ComType", &ComType);
    switch(ComType) {
    case (cpr_com_sql):{
      if (CursorDesc) {
	SQLDesc->ka = CursorDesc->OpenKa;
	SQLDesc->CursorDesc = CursorDesc;
	SQLDesc->cu = CursorDesc->cue;
	SQLDesc->StmtNameDesc = CursorDesc->StmtNameDesc;
      }
      else {
	SQLDesc->StmtNameDesc = PrecomDesc->StmtNameDesc;
      }
      break;
    }
    case (cpr_com_sql_conn):{
      SQLDesc->ka = ConDesc->ka;
      SQLDesc->StmtNameDesc = PrecomDesc->StmtNameDesc;
      break;
    }
    case (cpr_com_sql_putval): {}
    case (cpr_com_sql_getval): {}
    case (cpr_com_sql_fetch):{
      SQLDesc->CursorDesc = CursorDesc;
      SQLDesc->StmtNameDesc = PrecomDesc->StmtNameDesc;
      if (SQLDesc->StmtNameDesc) {
	SQLDesc->ka = SQLDesc->StmtNameDesc->ka;
      }
      if (CursorDesc) {
	SQLDesc->cu = CursorDesc->cue;
      }
      break;
    }
    case (cpr_com_sql_close):{
      SQLDesc->CursorDesc = CursorDesc;
      SQLDesc->StmtNameDesc = PrecomDesc->StmtNameDesc;
      if (CursorDesc) {
	SQLDesc->ka = CursorDesc->CloseKa;
	SQLDesc->cu = CursorDesc->cue;
      }
      break;
    }
    case (cpr_com_sql_open):{
      SQLDesc->CursorDesc = CursorDesc;
      SQLDesc->StmtNameDesc = (CursorDesc) ? CursorDesc->StmtNameDesc : NULL;
      if (CursorDesc) {
	SQLDesc->ka = CursorDesc->OpenKa;
	SQLDesc->cu = CursorDesc->cue;
      }
      break;
    }
    case (cpr_com_ora_descr_sel):{}
    case (cpr_com_describe):{
      SQLDesc->ka->kapacount = PrecomDesc->ka->kapacount;
      SQLDesc->StmtNameDesc = PrecomDesc->StmtNameDesc;
      break;
    }
    }
    if (CursorDesc) {
      if (ComType == cpr_com_sql_open
	  || ComType == cpr_com_sql_fetch
	  || ComType == cpr_com_sql_putval
	  || ComType == cpr_com_sql_getval
	  || ComType == cpr_com_sql_close ) {
	SQLDesc->ore = CursorDesc->StmtNameDesc->ore;
	SQLDesc->cu = CursorDesc->cue;
      }
    }
    Precom->sqlxa->xaSQLDesc = SQLDesc;
    SQLDesc->ComType = PrecomDesc->ComType;
    if (SQLDesc->StmtNameDesc) {
      tpr05_String *SQLStatement = SQLDesc->StmtNameDesc->SQLStatement;
      SQLDesc->PacketEncoding = SQLStatement->encodingType;
    }
    else {
      SQLDesc->PacketEncoding = sp77encodingAscii;
    }
    M90TRACE(M90_TR_EXIT, "pr01PrecomMakeSQLDesc", 0);
    M90TRACE(M90_TR_HANDLE, "SQLDesc", &SQLDesc);
    M90TRACE(M90_TR_HANDLE, "CursorDesc", &SQLDesc->CursorDesc);
    M90TRACE(M90_TR_HANDLE, "StmtNameDesc", &SQLDesc->StmtNameDesc);
    return SQLDesc;
  }
}

/*---------------------------------------------------------------------------*/
/*!
  Function:     pr01PrecomPrepare

  see also: 

  Description:  Prepares a SQL-Command. 
  
  Prepares a SQL-Command given by the PrecomDesc and ModuleDesc. If the given
  ConDesc is not connected it also connects to the Database by passing the 
  Arguments to pr01PrecomConnect
    
  Arguments:
    *PrecomDesc   [in|out] a pointer to a tpr01_PrecomDesc 
    *ModuleDesc   [in|out] a pointer to a tpr01_ModuleDesc 
    *ConDescDesc  [in|out] a pointer to a tpr01_ConDescDesc 
  return value: void
 */

static void pr01PrecomPrepare(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomPrepare", 0);
  M90TRACE(M90_TR_HANDLE, "PrecomDesc", &PrecomDesc);
  M90TRACE(M90_TR_HANDLE, "ModuleDesc", &ModuleDesc);
  M90TRACE(M90_TR_HANDLE, "ConDesc", &ConDesc);
  if (ConDesc) {
    tpr01_ConContainer *Connection = ConDesc->Connection;
    tpr01_StmtNameDesc *StmtNameDesc = NULL;
    ConDesc->Connection->SetAttr(ConDesc, ActualSession_epr01Con, ConDesc, 0);
    if (!Connection->IsConnected(ConDesc)) {
      tpr00_StmtNameStruct StmtNameStruct;
      tpr01_StmtNameContainer *StmtName = ModuleDesc->StmtName;
      tpr01_UniqueID UniqueID;
      StmtName->InitStmtNameStruct (&StmtNameStruct);
      UniqueID = pr01PrecomGetStmtName(PrecomDesc, &(StmtNameStruct.stStmtName));
      /*M90TRACE(M90_TR_STRING, "szStmtName", szStmtName);*/
      StmtNameDesc = StmtName->FindDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID);
      if (pr01PrecomConnect(PrecomDesc, ConDesc, StmtNameDesc)) {
	/* autoconnect was running */
	/* now prepare the Statement */
	pr01PrecomPrepare(PrecomDesc, ModuleDesc, ConDesc);
      }
    }
    else {
      pr01PrecomPrepareStmtName(PrecomDesc, ModuleDesc, ConDesc);
      if (PrecomDesc->SQLDesc) {
	tpr01_SQLContainer *SQL;
	tpr01_SQLDesc *SQLDesc = PrecomDesc->SQLDesc;
	SQL = SQLDesc->SQL;
	if (SQL->State(SQLDesc) & Prepared_bpr01) {
	  SQL->Close(SQLDesc);
	}
	SQL->Prepare(SQLDesc);
      }
    }
  }
  else {
    pr07CheckAssert(true);
  }
  M90TRACE(M90_TR_EXIT, "pr01PrecomPrepare", 0);
}

/*---------------------------------------------------------------------------*/
/*!
  Function:     pr01PrecomStatementBind

  see also: 

  Description:  Binds SQL-Statement
  
  Puts a SQL-Statement in PrecomDesc depending on fOption 
    
  Arguments:
    *PrecomDesc   [in|out] a pointer to a tpr01_PrecomDesc 
    *pSQLStatement [in]    a pointer to a tpr05_String which contains the  
                           SQL-Command
    fOption        [in]    controls processing of PrecomDesc->SQLStatement->rawString
    <UL>
        <LI>fOption == First_epr01     => a new sqlstatement, overwrite buffer 
        <LI>fOption == Append_epr01    => next part of sqlstatement, append on buffer 
        <LI>fOption == Reference_epr01 => only Referenz to sql-statement will be stored 
    </UL>

  return value: void
 */
static void pr01PrecomStatementBind(tpr01_PrecomDesc *PrecomDesc, tpr05_String *pSQLStatement, tpr01_SQLStatementOptionEnum fOption, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomStatementBind", 0);
  M90TRACE(M90_TR_HANDLE, "PrecomDesc", &PrecomDesc);
  {
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    tpr01_StmtNameContainer *StmtName = ModuleDesc->StmtName;
    tpr01_StmtNameDesc *StmtNameDesc;
    tpr00_StmtNameStruct StmtNameStruct;
    tpr01_UniqueID UniqueID;
    StmtName->InitStmtNameStruct (&StmtNameStruct);
    UniqueID = pr01PrecomGetStmtName(PrecomDesc, &(StmtNameStruct.stStmtName));
    /*M90TRACE(M90_TR_STRING, "szStmtName", szStmtName);*/
    StmtNameDesc = StmtName->FindDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID);
    if (!StmtNameDesc) {
      StmtNameDesc = StmtName->AddDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID, PrecomDesc->ka);
      StmtName->PutSQL(StmtNameDesc, pSQLStatement, First_epr01);
    }
    else
      StmtName->PutSQL(StmtNameDesc, pSQLStatement, fOption);
  }
  M90TRACE(M90_TR_EXIT, "pr01PrecomStatementBind", 0);
}

/*---------------------------------------------------------------------------*/

static void pr01PrecomPushError(tpr01_PrecomContainer *Cont, tpr00_ErrorKeyEnum ErrorKey)
{
  tpr01_ErrorText szErrorText;
  tpr01_ErrorDesc *ErrorDesc = Cont->ErrorDesc;
  tpr01_ErrorMethod *Error = ErrorDesc->Error;
  tpr01_ErrorCode ErrorCode;
  Error->MapError(ErrorDesc, PrecomDesc_epr01, ErrorKey, (char*)szErrorText, &ErrorCode);
  Error->SetError(ErrorDesc, ErrorCode, (char*)szErrorText, CPR_NTS);
}

/*---------------------------------------------------------------------------*/

static void pr01PrecomAfterExecute(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomAfterExecute", 0);
  M90TRACE(M90_TR_HANDLE, "PrecomDesc", &PrecomDesc);
  {
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    tpr01_SQLDesc *SQLDesc = PrecomDesc->SQLDesc;
    M90TRACE(M90_TR_DWORD, "sqlcode", &Precom->sqlca->sqlcode);
    /* Verweis auf die vom USING DESCRIPTOR gebunden SQLDA entfernen */
    Precom->sqlca->sqlcxap->xasqldap = NULL;
    if (Precom->sqlca->sqlcode == 0 || Precom->sqlca->sqlcode == 100) {
      M90TRACE(M90_TR_ENUM, "ComType", &PrecomDesc->ComType);
      switch (SQLDesc->ComType) {
      case (cpr_com_sql): {
	sqlkaentry *ka = SQLDesc->ka;
	M90TRACE(M90_TR_WORD,"sp1rFunctionCode", &ka->kaParseInfo.sp1rFunctionCode);
	switch(ka->kaParseInfo.sp1rFunctionCode) {
	case (csp1_commit_fc) : {
	  /* ADIS 1102890 (Kernel returns wrong function_code) */
	  if (ka->kaParseInfo.ParseId[cpr_p_precom_index] != csp1_p_release_found)
	    break;
	}
	case (csp1_commit_release_fc) : {}
	case (csp1_rollback_release_fc) : {
          tpr01_SQLContainer *SQL = SQLDesc->SQL;
          SQL->Release(SQLDesc);
	  break;
	}
	case (csp1_select_fc) : {}
	case (csp1_mselect_fc) : {
	  if (SQLDesc->CursorDesc) {
	    tpr01_CursorDesc *CursorDesc = SQLDesc->CursorDesc;
	    tpr01_CursorContainer *Cursor = CursorDesc->Cursor;
	    if (Precom->sqlca->sqlresn[0] != '\0') {
              tpr00_CursorNameDesc CursorNameDesc;
	      tpr01_CursorDesc *CursorDesc1;
	      Cursor->InitCursorName(&CursorNameDesc);
              /*Copy cursor name from resultset (received from database kernel after parsing).
                NOTE that the following statements assumes (and only work if) encoding type UTF8 
                for Precom->sqlca->sqlresn*/ 
	      pr07P2C(CursorNameDesc.CursorName.rawString, Precom->sqlca->sqlresn, sizeof(Precom->sqlca->sqlresn));
	      pr07StripChar(CursorNameDesc.CursorName.rawString, " ");
	      CursorNameDesc.CursorName.cbLen = (tsp00_Uint4)strlen(CursorNameDesc.CursorName.rawString);
	      CursorDesc1 = Cursor->FindDesc(Cursor, &(CursorNameDesc.CursorName), EmptyUniqueID_cpr01);
	      Cursor->AlterCursorName(CursorDesc, &(CursorNameDesc.CursorName));
	    }
	  }
	  else {
	    if (Precom->sqlca->sqlresn[0] != '\0') {
	      tpr01_SQLContainer *SQL = SQLDesc->SQL;
	      tpr01_ConDesc *ConDesc = SQLDesc->ConDesc;
	      tpr01_CursorContainer *Cursor = ModuleDesc->Cursor;
	      tpr01_CursorDesc *CursorDesc = NULL;
              tpr00_CursorNameDesc CursorNameDesc;
	      Cursor->InitCursorName(&CursorNameDesc);
	      /*Copy cursor name from resultset (received from database kernel after parsing).
                NOTE that the following statements assumes (and only work if) encoding type UTF8 
                for Precom->sqlca->sqlresn*/ 
	      pr07P2C(CursorNameDesc.CursorName.rawString, Precom->sqlca->sqlresn, sizeof(Precom->sqlca->sqlresn));
              pr07StripChar(CursorNameDesc.CursorName.rawString, " ");
              CursorNameDesc.CursorName.cbLen = (tsp00_Uint4)strlen(CursorNameDesc.CursorName.rawString);
              CursorNameDesc.CursorName.cbLen = (tsp00_Uint4)strlen(CursorNameDesc.CursorName.rawString);
	      CursorDesc = Cursor->Declare(Cursor, SQLDesc->StmtNameDesc, ConDesc, &(CursorNameDesc.CursorName), EmptyUniqueID_cpr01);
	      Cursor->PrepareOpen(CursorDesc, SQLDesc->ka);
	    } 

	  }
	  break;
	}
#if RELVER < R73
	case (csp1_select_direct_fc) : {}
	case (csp1_select_first_fc) : {}
	case (csp1_select_last_fc) : {}
	case (csp1_select_next_fc) : {}
	case (csp1_select_prev_fc) : {}
#endif
	case (csp1_select_into_fc) : {
	  if (SQLDesc->CursorDesc) {
	    tpr01_CursorDesc *CursorDesc = SQLDesc->CursorDesc;
	    tpr01_CursorContainer *Cursor = CursorDesc->Cursor;
	    Cursor->Close(CursorDesc);
	    SQLDesc->CursorDesc = NULL;
	  }
	  break;
	}
	}
	break;
      }
      case (cpr_com_sql_close ): {
	if (SQLDesc->CursorDesc) {
	  tpr01_CursorDesc *CursorDesc = SQLDesc->CursorDesc;
	  tpr01_CursorContainer *Cursor = CursorDesc->Cursor;
	  Cursor->Close(CursorDesc);
	  SQLDesc->CursorDesc = NULL;
	}
	break;
      }
      case (cpr_com_commit_release ): {}
      case (cpr_com_rollback_release ): {
        tpr01_SQLContainer *SQL = SQLDesc->SQL;
        SQL->Release(SQLDesc);
        break;
      }
      }
    }
  }
  M90TRACE(M90_TR_EXIT, "pr01PrecomAfterExecute", 0);
}

/*---------------------------------------------------------------------------*/

static tsp00_Int4 pr01PrecomUniqueID(tpr01_PrecomDesc *PrecomDesc)
{
  tpr01_ModuleDesc *ModuleDesc = PrecomDesc->Precom->ModuleDesc;
  tpr01_ModuleContainer *Module = ModuleDesc->Module;
  return Module->UniqueID(ModuleDesc, PrecomDesc->CmdNo);
}

/*---------------------------------------------------------------------------*/

static void pr01PrecomVersion(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomVersion", 0);
  PR07_CHECKDESC(PrecomDesc, PrecomDesc_epr01);
  M90TRACE(M90_TR_HANDLE, "PrecomDesc", &PrecomDesc);
  M90TRACE(M90_TR_HANDLE, "ConDesc", &ConDesc);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  M90TRACE(M90_TR_WORD, "kalineno", &PrecomDesc->ka->kalineno);
  {
    sqlkapointer ka = PrecomDesc->ka;
    tpr01_PrecomContainer *Cont = PrecomDesc->Precom;
    sqlxatype *sqlxa = Cont->sqlxa;
    tpr00_StmtNameStruct StmtNameStruct;
    tpr01_StmtNameContainer *StmtName = ModuleDesc->StmtName;
    tpr01_StmtNameDesc *StmtNameDesc = NULL;
    tpr01_UniqueID UniqueID;
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    tpr01_SQLDesc *SQLDesc = PrecomDesc->SQLDesc;
    tpr01_SQLContainer *SQL;
    StmtName->InitStmtNameStruct (&StmtNameStruct);
    UniqueID = pr01PrecomGetStmtName(PrecomDesc, &(StmtNameStruct.stStmtName));
    StmtNameDesc = StmtName->FindDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID);
    if (!StmtNameDesc) {
      StmtNameDesc = StmtName->AddDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID, PrecomDesc->ka);
    }
    if (!SQLDesc) {
      SQL = Precom->SQL;
      SQLDesc = SQL->AddDesc(SQL);
    }
    else {
      SQL = SQLDesc->SQL;
    }
    SQL->InitDesc(SQLDesc, Precom->sqlxa);
    Precom->sqlxa->xaSQLDesc = SQLDesc;
    PrecomDesc->SQLDesc = SQLDesc;
    SQLDesc->ConDesc = ConDesc;
    SQLDesc->ComType = PrecomDesc->ComType;
    if (StmtNameDesc) {
      SQLDesc->StmtNameDesc = StmtNameDesc;
      SQLDesc->ka = StmtNameDesc->ka;
      SQLDesc->ore = StmtNameDesc->ore;
      SQLDesc->cu = StmtNameDesc->cue;
    }
    pr01EXECSQLVersion(SQLDesc);
  }
  M90TRACE(M90_TR_EXIT, "pr01PrecomVersion", 0);
}

/*---------------------------------------------------------------------------*/

static void pr01PrecomOption(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomOption", 0);
  PR07_CHECKDESC(PrecomDesc, PrecomDesc_epr01);
  M90TRACE(M90_TR_HANDLE, "PrecomDesc", &PrecomDesc);
  M90TRACE(M90_TR_HANDLE, "ConDesc", &ConDesc);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  M90TRACE(M90_TR_WORD, "kalineno", &PrecomDesc->ka->kalineno);
  {
    sqlkapointer ka = PrecomDesc->ka;
    tpr01_PrecomContainer *Cont = PrecomDesc->Precom;
    sqlxatype *sqlxa = Cont->sqlxa;
    tpr00_StmtNameStruct StmtNameStruct;
    tpr01_StmtNameContainer *StmtName = ModuleDesc->StmtName;
    tpr01_StmtNameDesc *StmtNameDesc = NULL;
    tpr01_UniqueID UniqueID;
    StmtName->InitStmtNameStruct (&StmtNameStruct);
    UniqueID = pr01PrecomGetStmtName(PrecomDesc, &(StmtNameStruct.stStmtName));
    StmtNameDesc = StmtName->FindDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID);
    if (!StmtNameDesc) {
      StmtNameDesc = StmtName->AddDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID, PrecomDesc->ka);
    }
    if (pr01PrecomGetSQL(PrecomDesc, StmtNameDesc)) {
      tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
      tpr01_SQLDesc *SQLDesc = PrecomDesc->SQLDesc;
      tpr01_SQLContainer *SQL;
      if (!SQLDesc) {
	SQL = Precom->SQL;
	SQLDesc = SQL->AddDesc(SQL);
      }
      else {
	SQL = SQLDesc->SQL;
      }
      SQL->InitDesc(SQLDesc, Precom->sqlxa);
      PrecomDesc->SQLDesc = SQLDesc;
      SQLDesc->ConDesc = ConDesc;
      SQLDesc->ComType = PrecomDesc->ComType;
      if (StmtNameDesc) {
	SQLDesc->StmtNameDesc = StmtNameDesc;
	SQLDesc->ka = StmtNameDesc->ka;
	SQLDesc->ore = StmtNameDesc->ore;
	SQLDesc->cu = StmtNameDesc->cue;
      }
      pr01EXECSQLOptions(SQLDesc);
    }
  }
  M90TRACE(M90_TR_EXIT, "pr01PrecomOption", 0);
}

/*---------------------------------------------------------------------------*/

static void pr01PrecomTraceLine(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomSetTraceLine", 0);
  PR07_CHECKDESC(PrecomDesc, PrecomDesc_epr01);
  M90TRACE(M90_TR_HANDLE, "PrecomDesc", &PrecomDesc);
  M90TRACE(M90_TR_HANDLE, "ConDesc", &ConDesc);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  M90TRACE(M90_TR_WORD, "kalineno", &PrecomDesc->ka->kalineno);
  {
    sqlkapointer ka = PrecomDesc->ka;
    tpr01_PrecomContainer *Cont = PrecomDesc->Precom;
    sqlxatype *sqlxa = Cont->sqlxa;
    tpr00_StmtNameStruct StmtNameStruct;
    tpr01_StmtNameContainer *StmtName = ModuleDesc->StmtName;
    tpr01_StmtNameDesc *StmtNameDesc = NULL;
    tpr01_UniqueID UniqueID;
    StmtName->InitStmtNameStruct (&StmtNameStruct);
    UniqueID = pr01PrecomGetStmtName(PrecomDesc, &(StmtNameStruct.stStmtName));
    /*M90TRACE(M90_TR_STRING, "szStmtName", szStmtName);*/
    StmtNameDesc = StmtName->FindDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID);
    if (!StmtNameDesc) {
      StmtNameDesc = StmtName->AddDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID, PrecomDesc->ka);
    }
    if (pr01PrecomGetSQL(PrecomDesc, StmtNameDesc)) {
      tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
      tpr01_SQLDesc *SQLDesc = PrecomDesc->SQLDesc;
      tpr01_SQLContainer *SQL;
      if (!SQLDesc) {
	SQL = Precom->SQL;
	SQLDesc = SQL->AddDesc(SQL);
      }
      else {
	SQL = SQLDesc->SQL;
      }
      SQL->InitDesc(SQLDesc, Precom->sqlxa);
      PrecomDesc->SQLDesc = SQLDesc;
      SQLDesc->ConDesc = ConDesc;
      SQLDesc->ComType = PrecomDesc->ComType;
      if (StmtNameDesc) {
	SQLDesc->StmtNameDesc = StmtNameDesc;
	SQLDesc->ka = StmtNameDesc->ka;
	SQLDesc->ore = StmtNameDesc->ore;
	SQLDesc->cu = StmtNameDesc->cue;
      }
      pr01EXECSQLTraceLine(SQLDesc);
    }
  }
  M90TRACE(M90_TR_EXIT, "pr01PrecomTraceLine", 0);
}

static pr01PrecomRuntimeError(tpr01_PrecomDesc *PrecomDesc, tpr01_ConDesc *ConDesc, enum tpr_runtime_errors_Enum error)
{
  sqlcatype *sqlca = PrecomDesc->Precom->sqlca;
  sqlxatype *sqlxa = PrecomDesc->Precom->sqlxa;  
  sqlkaentry *kae = PrecomDesc->ka;
  p01xtracefilecheck(sqlca, sqlxa);
  p01pparsidtrace(PrecomDesc->Precom->sqlca, PrecomDesc->Precom->sqlxa, kae, kae->kaParseInfo.ParseId, TraceParseIDSQLStmt_epr00);
  p08runtimeerror (sqlca, sqlxa, error);  
}

/*---------------------------------------------------------------------------*/

static void pr01PrecomCancel(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomCancel", 0);
  PR07_CHECKDESC(PrecomDesc, PrecomDesc_epr01);
  M90TRACE(M90_TR_HANDLE, "PrecomDesc", &PrecomDesc);
  M90TRACE(M90_TR_HANDLE, "ConDesc", &ConDesc);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  M90TRACE(M90_TR_WORD, "kalineno", &PrecomDesc->ka->kalineno);
  {
    sqlkapointer ka = PrecomDesc->ka;
    tpr01_PrecomContainer *Cont = PrecomDesc->Precom;
    sqlxatype *sqlxa = Cont->sqlxa;
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    tpr01_SQLDesc *SQLDesc = PrecomDesc->SQLDesc;
    tpr01_SQLContainer *SQL = SQLDesc->SQL;
    SQL->InitDesc(SQLDesc, Precom->sqlxa);
    if (ConDesc)
      ConDesc = ConDesc->Connection->Actual;
    SQLDesc->ConDesc = ConDesc;
    SQLDesc->ka = PrecomDesc->ka;
    SQLDesc->StmtNameDesc = NULL;
    SQLDesc->ComType = PrecomDesc->ComType;
    pr01EXECSQLCancel(SQLDesc);
  }
  M90TRACE(M90_TR_EXIT, "pr01PrecomCancel", 0);
}

static void pr01PrecomAbortSession(tpr01_PrecomDesc *PrecomDesc, tpr01_ModuleDesc *ModuleDesc, tpr01_ConDesc *ConDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01PrecomAbortSession", 0);
  PR07_CHECKDESC(PrecomDesc, PrecomDesc_epr01);
  M90TRACE(M90_TR_HANDLE, "PrecomDesc", &PrecomDesc);
  M90TRACE(M90_TR_HANDLE, "ConDesc", &ConDesc);
  M90TRACE(M90_TR_DWORD, "CmdNo", &PrecomDesc->CmdNo);
  M90TRACE(M90_TR_WORD, "kalineno", &PrecomDesc->ka->kalineno);
  {
    sqlkapointer ka = PrecomDesc->ka;
    tpr01_PrecomContainer *Cont = PrecomDesc->Precom;
    sqlxatype *sqlxa = Cont->sqlxa;
    tpr01_PrecomContainer *Precom = PrecomDesc->Precom;
    tpr01_SQLDesc *SQLDesc = PrecomDesc->SQLDesc;
    tpr01_SQLContainer *SQL;
    if (!SQLDesc) {
      SQL = Precom->SQL;
      SQLDesc = SQL->AddDesc(SQL);
    }
    else {
      SQL = SQLDesc->SQL;
    }
    SQL->InitDesc(SQLDesc, Precom->sqlxa);
    if (ConDesc)
      ConDesc = ConDesc->Connection->Actual;
    SQLDesc->ConDesc = ConDesc;
    SQLDesc->ComType = PrecomDesc->ComType;
    pr01AbortSession(SQLDesc);
  }
  M90TRACE(M90_TR_EXIT, "pr01PrecomAbortSession", 0);
}
#ifdef DYNAHASH  
static tpr09HashValue pr01Precom_GetKey (const tpr09HashValue data_p, tsp00_Uint4* HashKeyLen){
  *HashKeyLen = sizeof(tsp00_Int4);
  return &(((struct tpr01_PrecomDesc *)data_p)->CmdNo);
}

static tsp00_Int4 pr01Precom_CompareKey (const tpr09HashKey Item1,tsp00_Uint4 Item1Len, const tpr09HashKey Item2, tsp00_Uint4 Item2Len){
   tsp00_Int4 val1 = *((tsp00_Int4*)Item1);
   tsp00_Int4 val2 = *((tsp00_Int4*)Item2);
  
   return (val1 == val2)?1:0;
}

static void pr01Precom_printKey (const tpr09HashValue data_p, FILE* datei){
  tsp00_Int4 val1 = ((struct tpr01_PrecomDesc *)data_p)->CmdNo;
  fprintf(datei, "<PrecomDesc addr=0x%p CmdNo=\"%d\" />", data_p, val1);
}

static tpr09HashIndex pr01Precom_HashFunc (const tpr09HashKey HashKey, tsp00_Uint4 cbHashKeyLen, tsp00_Uint4 cbHashTabLen)
{
  tsp00_Int4 val1 = *((tsp00_Int4*)HashKey);
  return ((tpr09HashIndex)val1%cbHashTabLen);
}
#endif 

