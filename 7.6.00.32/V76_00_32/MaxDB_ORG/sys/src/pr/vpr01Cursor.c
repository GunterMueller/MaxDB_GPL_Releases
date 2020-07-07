/*!================================================================
 module:    vpr01Cursor.c

 responsible: BurkhardD
 
 special area:   CPCDrv | Client | Cursorverwaltung
 
 description: Cursor Verwaltung
 
 see:  
 
 change history:

        2001-06-22 D025086 PTS 1110677 Close the Cursor to drop old ParseIds
        before open a new one


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

#include "vpr07Macro.h"
#include "vpr01Cursor.h"
#include "vpr01Con.h"
#include "vpr01Error.h"
#include "vpr01StmtName.h"
#include "gpr03.h"
#include "gpr03m.h"
#include "gmt90.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

static void pr01CursorInitCont(tpr01_CursorContainer *Cont);

static tsp00_Int4 pr01CursorOpenCont(tpr01_CursorContainer *Cursor);

/*static tsp00_Int4 pr01CursorGetCount(tpr01_CursorContainer *Cursor);*/

static void pr01CursorCloseCont(tpr01_CursorContainer *Cursor);

static tpr01_CursorDesc *pr01CursorEnumDesc(tpr01_CursorContainer *Cursor);

static tpr01_CursorDesc *pr01CursorAddDesc(tpr01_CursorContainer *Cursor, tpr05_String *CursorName, tpr01_UniqueID UniqueID, tpr01_ConDesc *ConDesc);

static void pr01CursorDeleteDesc(tpr01_CursorDesc *CursorDesc);

static tpr01_CursorDesc *pr01CursorFindDesc(tpr01_CursorContainer *Cursor, tpr05_String* CursorName, tpr01_UniqueID UniqueID);

static void pr01CursorResetIterator(tpr01_CursorContainer *Cursor);

static void pr01CursorExecute(tpr01_CursorDesc *CursorDesc);

static tpr01_CursorDesc *pr01CursorDeclare (tpr01_CursorContainer *Cursor, tpr01_StmtNameDesc *StmtNameDesc, tpr01_ConDesc *ConDesc, tpr05_String *CursorName, tpr01_UniqueID UniqueID);

static void pr01CursorPrepareOpen(tpr01_CursorDesc *CursorDesc, sqlkaentry *ka);
static void pr01CursorPrepareFetch(tpr01_CursorDesc *CursorDesc, struct tpr01_StmtNameDesc *StmtNameDesc, boolean tryexec);
static void pr01CursorPrepareClose(tpr01_CursorDesc *CursorDesc, sqlkaentry *ka);
static void pr01CursorClose(tpr01_CursorDesc *CursorDesc);

static void pr01CursorAlterCursorName (tpr01_CursorDesc *CursorDesc,  tpr05_String* CursorName);

static tpr01_ConDesc *pr01CursorGetConnection(tpr01_CursorDesc *CursorDesc);

static BOOLEAN pr01CursorSetAttr (tpr01_CursorDesc *CursorDesc, epr01Cursor_Attr Attr, 
				  void *pAttr, tsp00_Int4 cbLen);

static void pr01CursorGetAttr (tpr01_CursorDesc *CursorDesc, epr01Cursor_Attr Attr, 
			       void *pAttr, tsp00_Int4 *cbLen);

static void pr01Cursor_InitCursorName (tpr00_CursorNameDesc *CursorNameDesc);
#ifdef DYNAHASH  
static tpr09HashValue pr01Cursor_GetKey (const tpr09HashValue data_p, tsp00_Uint4* HashKeyLen);

static tsp00_Int4 pr01Cursor_CompareKey (const tpr09HashKey Item1,tsp00_Uint4 Item1Len, const tpr09HashKey Item2, tsp00_Uint4 Item2Len);

static void pr01Cursor_printKey (const tpr09HashValue data_p, FILE* datei);

static tpr09HashIndex pr01Cursor_hashFunc (const tpr09HashKey HashKey, tsp00_Uint4 cbHashKeyLen, tsp00_Uint4 cbHashTabLen);
/*
static void dumpKey ( tpr09HashValue data_p, char* praefix){
  tpr01_CursorDesc_CursorID *cursorID = (struct tpr01_CursorDesc_CursorID *)data_p;
  if ( cursorID->UniqueID == EmptyUniqueID_cpr01) {
    printDump("%s \"%.*s\"\n", praefix, cursorID->cursorName->cbLen, cursorID->cursorName->rawString);
  } else {
    printDump("%s \":%#08lx\"\n", praefix, cursorID->UniqueID);
  }
}
*/
#endif 

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

static void pr01CursorInitCont(tpr01_CursorContainer *Cursor)
{
#ifdef DYNAHASH  
  Cursor->Desc = pr09NewHashDescriptor(20, 
                                       sizeof(tpr01_CursorDesc), 
                                       pr01Cursor_CompareKey, 
                                       pr01Cursor_hashFunc, 
                                       pr01Cursor_GetKey,
                                       pr01Cursor_printKey);
#else
  Cursor->Desc = pr09NewDescriptor(3, sizeof(tpr01_CursorDesc));
#endif 

  Cursor->cachedCursorID.UniqueID = EmptyUniqueID_cpr01;
  Cursor->cachedCursorID.cursorName = NULL;     
  Cursor->ErrorDesc = pr01NewErrorDesc();
  Cursor->InitCont  = pr01CursorInitCont;
  Cursor->OpenCont  = pr01CursorOpenCont;
  Cursor->CloseCont = pr01CursorCloseCont;
/*  Cursor->GetCount  = NULL; */
  Cursor->EnumDesc  = pr01CursorEnumDesc;
  Cursor->AddDesc   = pr01CursorAddDesc;
  Cursor->DeleteDesc = pr01CursorDeleteDesc;
  Cursor->ResetIterator = pr01CursorResetIterator;
  Cursor->FindDesc  = pr01CursorFindDesc;
  Cursor->Execute   = pr01CursorExecute;
  Cursor->Declare   = pr01CursorDeclare;
  Cursor->Close     = pr01CursorClose;
  Cursor->PrepareOpen     = pr01CursorPrepareOpen;
  Cursor->PrepareFetch    = pr01CursorPrepareFetch;
  Cursor->PrepareClose    = pr01CursorPrepareClose;
  Cursor->AlterCursorName = pr01CursorAlterCursorName;
  Cursor->GetConnection   = pr01CursorGetConnection;
  Cursor->InitCursorName  = pr01Cursor_InitCursorName; 
}

void pr01CursorDeleteCont(tpr01_CursorContainer *Cursor)
{
  if (Cursor) {
    if (pr01CursorOpenCont(Cursor)) {
      tpr01_CursorDesc *CursorDesc;
      while(CursorDesc = Cursor->EnumDesc(Cursor))
	Cursor->DeleteDesc(CursorDesc);
    }
    pr01DeleteErrorDesc(Cursor->ErrorDesc);    
#ifdef DYNAHASH  
    pr09DeleteHashDescriptor(Cursor->Desc);
#else
    pr09DeleteDescriptor(Cursor->Desc);
#endif 
    Cursor->Desc = NULL;
    pr03mFree((tsp00_Uint1*)Cursor, "Cursor");
  }
}

tpr01_CursorContainer *pr01CursorNewCont()
{
  tpr01_CursorContainer *Cursor;
  Cursor = pr03mAllocat(sizeof(tpr01_CursorContainer), "Cursor");
  if (Cursor)
    pr01CursorInitCont(Cursor);
  return Cursor;
}


static tsp00_Int4 pr01CursorOpenCont(tpr01_CursorContainer *Cursor)
{
/*  if (!Cursor->Desc) {
    Cursor->Desc = pr09NewDescriptor(10, sizeof(tpr01_CursorDesc));
  }
*/
    if (Cursor->Desc) {
#ifdef DYNAHASH  
    pr09ResetHashItemIterator(Cursor->Desc);
#else
    pr09SetIndex(Cursor->Desc, -1);
#endif 
    return 1;
  } else {
    return 0;
  }
}

/*
static tsp00_Int4 pr01CursorGetCount(tpr01_CursorContainer *Cursor)
{
  return(pr09GetItemCount(Cursor->Desc));
}
*/
static tpr01_CursorDesc *pr01CursorEnumDesc(tpr01_CursorContainer *Cursor)
{
#ifdef DYNAHASH  
  tpr01_CursorDesc *CursorDesc=pr09HashItemIteratorNextItem(Cursor->Desc);
#else
  tpr01_CursorDesc *CursorDesc=pr09GetNextItem(Cursor->Desc);
#endif 
  return(CursorDesc);
}

static void pr01CursorCloseCont(tpr01_CursorContainer *Cursor)
{
  M90TRACE(M90_TR_HANDLE, "pr01CursorCloseCont", Cursor);
#ifdef DYNAHASH  
  pr09DeleteAllHashItems(Cursor->Desc);
#else
  pr09CloseAllItems(Cursor->Desc);
#endif 
}

void pr01CursorInitAttr (tpr01_CursorDesc *CursorDesc)
{
  BOOLEAN vfalse = FALSE;
  BOOLEAN vtrue = TRUE;
  CursorDesc->SetAttr (CursorDesc, openkeep_epr01Cursor, &vfalse, 1);
  CursorDesc->SetAttr (CursorDesc, useNewParseInfo_epr01Cursor, &vtrue, 1);
  CursorDesc->SetAttr (CursorDesc, lastDataReceived_epr01Cursor, &vfalse, 1);
}

static tpr01_CursorDesc *pr01CursorAddDesc(tpr01_CursorContainer *Cursor, tpr05_String *CursorName, tpr01_UniqueID UniqueID, tpr01_ConDesc *ConDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01CursorAddDesc", 0);
  {
    tpr01_CursorDesc *CursorDesc;      
#ifdef DYNAHASH  
    tpr09_HashTableError hTabErr; 
    CursorDesc = pr09GetNewHashItem(Cursor->Desc);
#else
    CursorDesc = pr09AddItem(Cursor->Desc);
#endif 
    CursorDesc->DescType = CursorDesc_epr01;
    CursorDesc->Cursor=Cursor;
    CursorDesc->ConDesc      = ConDesc;
    CursorDesc->StmtNameDesc = NULL;
    CursorDesc->FetchStmtNameDesc = NULL;
    CursorDesc->OpenKa       = NULL;
    CursorDesc->FetchKa      = NULL;
    CursorDesc->CloseKa      = NULL;
    CursorDesc->ErrorDesc    = pr01NewErrorDesc();
    CursorDesc->cue = pr03mAllocat(sizeof(sqlcuentry), "CursorDesc->cu");
    pr03cCuInit(CursorDesc->cue);
    Cursor->InitCursorName(&(CursorDesc->CursorNameDesc));
    if (UniqueID != EmptyUniqueID_cpr01) {
      /*generate an implizit cursor name*/
      CursorDesc->CursorNameDesc.CursorName.cbLen = 
      sp77sprintfUnicode (CursorDesc->CursorNameDesc.CursorName.encodingType,
                          CursorDesc->CursorNameDesc.CursorName.rawString,
                          CursorDesc->CursorNameDesc.CursorName.cbMaxLen,
                          ":%#08lx", 
                          UniqueID);
    }
    else {
      /*take the given cursor name*/  
      CursorDesc->CursorNameDesc.CursorName.cbLen = CursorName->cbLen;
      CursorDesc->CursorNameDesc.CursorName.encodingType = CursorName->encodingType;
      memcpy(CursorDesc->CursorNameDesc.CursorName.rawString, CursorName->rawString, CursorName->cbLen);
    }
    CursorDesc->CursorID.UniqueID = UniqueID;
    CursorDesc->CursorID.cursorName = &(CursorDesc->CursorNameDesc.CursorName);

#ifdef DYNAHASH  
    /*add the descriptor to the cursor container hash index*/
    hTabErr = pr09AddHashItemToHash(Cursor->Desc, 
                                    &(CursorDesc->CursorID), 
                                    sizeof(tpr01_CursorDesc_CursorID),
                                    (tpr09HashValue*)CursorDesc);
#endif 

    CursorDesc->SetAttr = pr01CursorSetAttr;
    CursorDesc->GetAttr = pr01CursorGetAttr;
    pr01CursorInitAttr (CursorDesc);
         
    M90TRACE(M90_TR_EXIT, "pr01CursorAddDesc", 0);
    return(CursorDesc);
  }
}

static void pr01CursorDeleteDesc(tpr01_CursorDesc *CursorDesc)
{
#ifdef DYNAHASH  
  if (NULL != pr09FindHashItem(CursorDesc->Cursor->Desc, 
                               &(CursorDesc->CursorID), 
                               sizeof(tpr01_CursorDesc_CursorID))) {  

#else
  int cbIndex = pr09ItemFindIndex(CursorDesc->Cursor->Desc, CursorDesc);
  if (cbIndex >= 0) {
#endif 

    tpr05_String* cName = &(CursorDesc->CursorNameDesc.CursorName);                            
    pr01DeleteErrorDesc(CursorDesc->ErrorDesc);    
    if (CursorDesc->cue) {
      pr03mFree(CursorDesc->cue, "CursorDesc->cue");
    }
    /*delete cursor name and release memory*/
#ifdef DYNAHASH  
    pr09DeleteHashItem(CursorDesc->Cursor->Desc, 
                       &(CursorDesc->CursorID), 
                       sizeof(tpr01_CursorDesc_CursorID));
#else
    pr09CloseItemEx(CursorDesc->Cursor->Desc, cbIndex);
#endif 
    if (cName && cName->rawString) pr05IfCom_String_DeleterawString(cName);
  } 
}

static void pr01CursorResetIterator(tpr01_CursorContainer *Cont){
#ifdef DYNAHASH  
  if (Cont && Cont->Desc) {pr09ResetHashItemIterator(Cont->Desc);}    
#else
  if (Cont && Cont->Desc) {pr09SetIndex(Cont->Desc, -1);}
#endif 
}
#ifdef DYNAHASH  
static tpr01_CursorDesc *pr01CursorFindDesc(tpr01_CursorContainer *Cursor, tpr05_String *CursorName, tpr01_UniqueID UniqueID)
{
  M90TRACE(M90_TR_ENTRY, "pr01CursorFindDesc", 0);
  {
    tpr01_CursorDesc *CursorDesc = NULL;
    pr01CursorResetIterator(Cursor);
    Cursor->cachedCursorID.cursorName = CursorName;
    Cursor->cachedCursorID.UniqueID   = UniqueID;
    CursorDesc = (tpr01_CursorDesc *)pr09FindHashItem(  Cursor->Desc, 
                                                        &(Cursor->cachedCursorID), 
                                                        sizeof(tpr01_CursorDesc_CursorID));    
    M90TRACE(M90_TR_EXIT, "pr01CursorFindDesc", 0);
    M90TRACE(M90_TR_HANDLE, "CursorDesc", &CursorDesc);
    return(CursorDesc);    
  }
}
#else
static tpr01_CursorDesc *pr01CursorFindDesc(tpr01_CursorContainer *Cursor, tpr05_String *CursorName, tpr01_UniqueID UniqueID)
{
  M90TRACE(M90_TR_ENTRY, "pr01CursorFindDesc", 0);
  {
    tsp00_Int4 cbOldIndex = pr09SetIndex(Cursor->Desc, -1);
    tpr01_CursorDesc *CursorDesc = NULL;
    if ( UniqueID == EmptyUniqueID_cpr01) {
      while(CursorDesc=Cursor->EnumDesc(Cursor)) {
	    if (! pr05IfCom_String_strcmp(CursorName,&(CursorDesc->CursorNameDesc.CursorName)))
	      break;
      }
    }
    else {
      while(CursorDesc=Cursor->EnumDesc(Cursor)) {
	    if (UniqueID == CursorDesc->CursorID.UniqueID)
	      break;
      }
    }
    if(!CursorDesc)
      pr09SetIndex(Cursor->Desc, cbOldIndex);    
    M90TRACE(M90_TR_EXIT, "pr01CursorFindDesc", 0);
    M90TRACE(M90_TR_HANDLE, "CursorDesc", &CursorDesc);
    return(CursorDesc);    
  }
}
#endif 

static void pr01CursorExecute(tpr01_CursorDesc *CursorDesc)
{
  CursorDesc->ErrorDesc->Error->SetError(CursorDesc->ErrorDesc, 1, "UNKNOWN", CPR_NTS);
  return;
}

static void pr01CursorClose(tpr01_CursorDesc *CursorDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01CursorClose", 0);
  M90TRACE(M90_TR_HANDLE, "CursorDesc", &CursorDesc);
  {
    BOOLEAN myVal = FALSE;
    M90TRACE(M90_TR_HANDLE, "StmtNameDesc", &CursorDesc->StmtNameDesc);
    if (CursorDesc->StmtNameDesc) 
    {
      tpr01_CursorContainer *Cursor = CursorDesc->Cursor;
      if (CursorDesc->OpenKa) 
      {
        pr01cFreeKa(CursorDesc->OpenKa);
        CursorDesc->OpenKa = NULL; 
      }
      if (CursorDesc->FetchKa) 
      {
        /* drop parseid  PTS 1106501 */
        sqlcatype *sqlca = CursorDesc->ConDesc->sqlca;
        sqlxatype *sqlxa = CursorDesc->ConDesc->sqlxa;
        sqlgaentry *ga   = CursorDesc->ConDesc->ga;
        sqlkaentry *ka   = CursorDesc->FetchKa;  
        p01bdrop_parsid (sqlca, sqlxa, ka->kaParseInfo.ParseId);
        pr06ParseIdDrop(sqlca, sqlxa, ga, (tpr06_ParseIDCache *)CursorDesc->ConDesc->ParseIdCache, &ka->kaParseInfo.ParseId);
        pr01cFreeKa(CursorDesc->FetchKa);
	CursorDesc->FetchKa = NULL;
      }
      if (CursorDesc->CloseKa) 
      {
	    CursorDesc->CloseKa = NULL;
      }
    }
    /*reset cursor attribut lastDataReceived to false*/
    CursorDesc->SetAttr (CursorDesc, lastDataReceived_epr01Cursor, &myVal, 1);
  }
  M90TRACE(M90_TR_EXIT, "pr01CursorClose", 0);
}

static void pr01CursorPrepareOpen(tpr01_CursorDesc *CursorDesc, sqlkaentry *ka)
{
  BOOLEAN keep;

  M90TRACE(M90_TR_ENTRY, "pr01CursorPrepareOpen", 0);
  PR07_CHECKDESC(CursorDesc, CursorDesc_epr01);
  {
    tpr01_CursorContainer *Cursor = CursorDesc->Cursor;
    /* PTS 1110677 */
    Cursor->Close(CursorDesc);
    if (!CursorDesc->OpenKa) {
      CursorDesc->OpenKa = pr01cDupKa(ka);
      if (CursorDesc->StmtNameDesc) {
	tpr01_StmtNameDesc *StmtNameDesc = CursorDesc->StmtNameDesc;
	if (StmtNameDesc->ka->kapacount > 0) {
	  /* Command with :hostvars */
	  M90TRACE(M90_TR_WORD, "kapacount", &StmtNameDesc->ka->kapacount);
	  M90TRACE(M90_TR_WORD, "kapaindex", &StmtNameDesc->ka->kapaindex);
	  CursorDesc->OpenKa->kapacount = StmtNameDesc->ka->kapacount;
	  CursorDesc->OpenKa->kapaindex = StmtNameDesc->ka->kapaindex;
	}
	CursorDesc->OpenKa->kafaindex = StmtNameDesc->ka->kafaindex;
        if (StmtNameDesc->ka->kafae) {
          memcpy(CursorDesc->OpenKa->kafae, StmtNameDesc->ka->kafae, sizeof(sqlfaentry));
        }
      }
    }
    if (ka->katyp == cpr_com_sql_open_keep) {
      keep = TRUE;
      CursorDesc->SetAttr (CursorDesc, openkeep_epr01Cursor, &keep, sizeof (keep));
    } else {
      keep = FALSE;
      CursorDesc->SetAttr (CursorDesc, openkeep_epr01Cursor, &keep, sizeof (keep));
    }
  }
  M90TRACE(M90_TR_EXIT, "pr01CursorPrepareOpen", 0);
}

static void pr01CursorPrepareFetch(tpr01_CursorDesc *CursorDesc, struct tpr01_StmtNameDesc *StmtNameDesc, boolean tryexec)
{
  M90TRACE(M90_TR_ENTRY, "pr01CursorPrepareFetch", 0);
  PR07_CHECKDESC(CursorDesc, CursorDesc_epr01);
  PR07_CHECKDESC(StmtNameDesc, StmtNameDesc_epr01);
  {
    if (!CursorDesc->FetchKa) {
      CursorDesc->FetchKa = pr01cDupKa(StmtNameDesc->ka);
    }
    if (CursorDesc->FetchStmtNameDesc != StmtNameDesc) {
        pr01cFreeKa(CursorDesc->FetchKa);
        CursorDesc->FetchKa = pr01cDupKa(StmtNameDesc->ka);
    }
    CursorDesc->FetchStmtNameDesc = StmtNameDesc;
    CursorDesc->FetchKa->kacuindex = CursorDesc->OpenKa->kacuindex;
  }
  M90TRACE(M90_TR_EXIT, "pr01CursorPrepareFetch", 0);
}
  
static void pr01CursorPrepareClose(tpr01_CursorDesc *CursorDesc, sqlkaentry *ka)
{
  M90TRACE(M90_TR_ENTRY, "pr01CursorPrepareClose", 0);
  PR07_CHECKDESC(CursorDesc, CursorDesc_epr01);
  {
    CursorDesc->CloseKa = ka;
  }
  M90TRACE(M90_TR_EXIT, "pr01CursorPrepareClose", 0);
}

static tpr01_CursorDesc *pr01CursorDeclare (tpr01_CursorContainer *Cursor, tpr01_StmtNameDesc *StmtNameDesc, tpr01_ConDesc *ConDesc, tpr05_String *CursorName, tpr01_UniqueID UniqueID)
{
  M90TRACE(M90_TR_ENTRY, "pr01CursorDeclare", 0);
  M90TRACE(M90_TR_HANDLE, "StmtNameDesc", StmtNameDesc);
  {
    tpr01_CursorDesc *CursorDesc = Cursor->FindDesc(Cursor, CursorName, UniqueID);
    if (!CursorDesc) {
      CursorDesc = Cursor->AddDesc(Cursor, CursorName, UniqueID, ConDesc);
    }
    if (!StmtNameDesc) {
      tsp00_Int4 ErrorCode;
      tpr01_ErrorText szErrorText;
      pr01ErrorGetErrorText(cpr_unknown_statement_name, &ErrorCode, szErrorText);
      Cursor->ErrorDesc->Error->SetError(Cursor->ErrorDesc, ErrorCode, (char*)szErrorText, CPR_NTS);
      CursorDesc = NULL;
    }
    if (CursorDesc) {
      BOOLEAN vtrue = TRUE;
      CursorDesc->StmtNameDesc = StmtNameDesc;
      CursorDesc->ConDesc = ConDesc;

      /*set useNewParseInfo flag because the following open command must 
        use the parseinfo from stmtnamedesc*/
      CursorDesc->SetAttr (CursorDesc, useNewParseInfo_epr01Cursor, &vtrue, 1);
    }
    M90TRACE(M90_TR_EXIT, "pr01CursorDeclare", 0);
    M90TRACE(M90_TR_HANDLE, "CursorDesc", &CursorDesc);
    return CursorDesc;
  }
}
/*!
  Function:     pr01CursorAlterCursorName

  see also: 

  Description:  alters the cursor name
   
  If no cursor name is given by application a implicit cursor name will 
  be set. After parsing the command by database kernel the implicit cursor 
  name will be replaced with the result set name received from database kernel.
  
  Arguments:
    CursorDesc [in]  cursor descriptor
    CursorName [out] new name of cursor

  return value: void
 */
static void pr01CursorAlterCursorName (tpr01_CursorDesc *CursorDesc, tpr05_String *CursorName)
{
  PR07_CHECKDESC(CursorDesc, CursorDesc_epr01);
  {
    tpr05_StringError erg;
#ifdef DYNAHASH  
    tpr09HashIndex  hashIndex;
    pr09HTRemoveHashItemByKey ( &(CursorDesc->Cursor->Desc->hTab), 
                                &(CursorDesc->CursorID), 
                                sizeof(tpr01_CursorDesc_CursorID));                   
#endif 
    erg = pr05IfCom_String_Convert (&(CursorDesc->CursorNameDesc.CursorName),CursorName);
    CursorDesc->CursorID.UniqueID = EmptyUniqueID_cpr01;
#ifdef DYNAHASH  
    pr09HTInsertHashItemByValue (&(CursorDesc->Cursor->Desc->hTab), 
                                 &(CursorDesc->CursorID), 
                                 sizeof(tpr01_CursorDesc_CursorID),
                                 CursorDesc, 
                                 &hashIndex);
#endif 
  }
}

/*!
  Function:     pr01CursorAlterParseid

  see also: 

  Description:  alters the parseid of current Openka
   
  If the useNewParseinfo flag is set or the parseid of Openka is zero the
  parseid of ka-area in StmtNameDesc will be copied.
  
  Arguments:
    SQLDesc [in]  sql descriptor

  return value: void
 */
void pr01CursorAlterParseid (tpr01_SQLDesc *SQLDesc)
{
  PR07_CHECKDESC(SQLDesc, SQLDesc_epr01);
  {
    tpr01_CursorDesc   *CursorDesc     = SQLDesc->CursorDesc;
    sqlkaentry         *ka             = SQLDesc->ka;
    tpr01_StmtNameDesc *StmtNameDesc   = SQLDesc->StmtNameDesc;
    BOOLEAN            useNewParseInfo = FALSE;
    tsp00_Int4         sz              = 0;
    
    if (   StmtNameDesc 
        &&(StmtNameDesc->State & Prepared_bpr01)
        ) {
         if ( CursorDesc ) {
           CursorDesc->GetAttr (CursorDesc, useNewParseInfo_epr01Cursor, &useNewParseInfo, &sz);
           if (   pr06ParseIdIsEqual(&ka->kaParseInfo.ParseId, ParseIdNull_cpr00)
               || useNewParseInfo ) {
             pr06ParseInfoCopy(&ka->kaParseInfo, &StmtNameDesc->ka->kaParseInfo);
             useNewParseInfo = FALSE;
             CursorDesc->SetAttr (CursorDesc, useNewParseInfo_epr01Cursor, &useNewParseInfo, 1);
           }  
         }   
         else {
           pr06ParseInfoCopy(&ka->kaParseInfo, &StmtNameDesc->ka->kaParseInfo);
         }
         ka->kastate = cpr_state_execute;
         SQLDesc->ore = StmtNameDesc->ore;
      }
  }
}


static tpr01_ConDesc *pr01CursorGetConnection(tpr01_CursorDesc *CursorDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01CursorGetConnection", 0);
  M90TRACE(M90_TR_EXIT, "pr01CursorGetConnection", 0);
  M90TRACE(M90_TR_HANDLE, "ConDesc", &CursorDesc->ConDesc);
  return CursorDesc->ConDesc;
}
 
static void pr01CursorB2B(void *cout, void *cin, int clen, int len)
{
  if (len == CPR_NTS) 
    len = (int) strlen((char*)cin);
  len = PR07MIN(len, clen);
  memcpy((char*)cout, (char*)cin, len);  
}

static BOOLEAN pr01CursorSetAttr (tpr01_CursorDesc *CursorDesc, epr01Cursor_Attr Attr, 
				  void *pAttr, tsp00_Int4 cbLen)
{
  M90TRACE(M90_TR_ENTRY, "pr01CursorSetAttr", 0);
  {
    BOOLEAN ret = TRUE;
    switch (Attr) {
    case openkeep_epr01Cursor: {
      pr01CursorB2B (&CursorDesc->Attribute.openkeep, pAttr,
		     sizeof (CursorDesc->Attribute.openkeep), cbLen);
      break;
    } 
    case useNewParseInfo_epr01Cursor: {
      pr01CursorB2B (&CursorDesc->Attribute.useNewParseInfo, pAttr,
		     sizeof (CursorDesc->Attribute.useNewParseInfo), cbLen);
      break;
    } 
    case lastDataReceived_epr01Cursor: {
      pr01CursorB2B (&CursorDesc->Attribute.lastDataReceived, pAttr,
		     sizeof (CursorDesc->Attribute.lastDataReceived), cbLen);
      break;
    } 
    default: {
      ret = FALSE;
    }
    }
    M90TRACE(M90_TR_EXIT, "pr01CursorSetAttr", 0);  
    return ret;
  }
}

static void pr01CursorGetAttr (tpr01_CursorDesc *CursorDesc, epr01Cursor_Attr Attr, 
			       void *pAttr, tsp00_Int4 *cbLen)
{
  M90TRACE(M90_TR_ENTRY, "pr01CursorGetAttr", 0);
  {
    switch (Attr) {
    case openkeep_epr01Cursor: {
      *cbLen = sizeof (CursorDesc->Attribute.openkeep);
      pr01CursorB2B (pAttr, &CursorDesc->Attribute.openkeep,
		     sizeof (CursorDesc->Attribute.openkeep), *cbLen);
      break;
    }
    case useNewParseInfo_epr01Cursor: {
      *cbLen = sizeof (CursorDesc->Attribute.useNewParseInfo);
      pr01CursorB2B (pAttr, &CursorDesc->Attribute.useNewParseInfo,
		     sizeof (CursorDesc->Attribute.useNewParseInfo), *cbLen);
      break;
    } 
    case lastDataReceived_epr01Cursor: {
      *cbLen = sizeof (CursorDesc->Attribute.lastDataReceived);
      pr01CursorB2B (pAttr, &CursorDesc->Attribute.lastDataReceived,
		     sizeof (CursorDesc->Attribute.lastDataReceived), *cbLen);
      break;
    } 
    default: {}
    }
  }
  M90TRACE(M90_TR_EXIT, "pr01CursorGetAttr", 0);  
}

/*!
  Function:     pr01Cursor_InitCursorName

  see also: 
  
  Description:  initializes tpr00_CursorNameDesc

  Arguments:
    CursorNameDesc [in] a tpr00_CursorNameDesc
 
  return value: void
 */
static void pr01Cursor_InitCursorName (tpr00_CursorNameDesc *CursorNameDesc)
{
  CursorNameDesc->CursorName.allocationType = constant_epr05;
  pr05IfCom_String_InitString(&(CursorNameDesc->CursorName), 
                              &(CursorNameDesc->private_StrBuf), 
                              sizeof(CursorNameDesc->private_StrBuf), 
                              sp77encodingAscii, 
                              constant_epr05);
  CursorNameDesc->CursorName.cbLen=0;
}

#ifdef DYNAHASH  
static tpr09HashValue pr01Cursor_GetKey (const tpr09HashValue data_p, tsp00_Uint4* HashKeyLen){
  tpr01_CursorDesc_CursorID* cursorID=&((tpr01_CursorDesc *)data_p)->CursorID;
  *HashKeyLen = sizeof(tpr01_CursorDesc_CursorID);
  return cursorID;
}

static tsp00_Int4 pr01Cursor_CompareKey (const tpr09HashKey Item1,tsp00_Uint4 Item1Len, const tpr09HashKey Item2, tsp00_Uint4 Item2Len){
  tpr01_CursorDesc_CursorID *cursorName1 = (tpr01_CursorDesc_CursorID*) Item1;
  tpr01_CursorDesc_CursorID *cursorName2 = (tpr01_CursorDesc_CursorID*) Item2;

  if (cursorName1->UniqueID == EmptyUniqueID_cpr01){ 
    if (cursorName2->UniqueID == EmptyUniqueID_cpr01){
       return  (! pr05IfCom_String_strcmp(cursorName1->cursorName, cursorName2->cursorName));
    } else {
      return 0;
    }       
  } else if (cursorName2->UniqueID != EmptyUniqueID_cpr01){
    return (cursorName1->UniqueID == cursorName2->UniqueID)?1:0; 
  }
  return 0;
}

static void pr01Cursor_printKey (const tpr09HashValue data_p, FILE* datei){
  tpr01_CursorDesc_CursorID *cursorID = &((struct tpr01_CursorDesc *)data_p)->CursorID;
  if ( cursorID->UniqueID == EmptyUniqueID_cpr01) {
    fprintf(datei, "<CursorDesc addr=0x%p name=\"%.*s\" />", data_p, cursorID->cursorName->cbLen, cursorID->cursorName->rawString);
  } else {
    fprintf(datei, "<CursorDesc addr=0x%p uniqueID=\"%d\" />", data_p, cursorID->UniqueID);
  }
}

static tpr09HashIndex pr01Cursor_hashFunc (const tpr09HashKey HashKey, tsp00_Uint4 cbHashKeyLen, tsp00_Uint4 cbHashTabLen)
{
  tpr01_CursorDesc_CursorID *cursorID = (tpr01_CursorDesc_CursorID*) HashKey; 
  if ( cursorID->UniqueID == EmptyUniqueID_cpr01) {
    tsp00_Uint4 i;
    tpr09HashIndex HashVal=0;
    const char*	myKey = (char*) cursorID->cursorName->rawString;
    cbHashKeyLen = cursorID->cursorName->cbLen;
    for(i=0; i < cbHashKeyLen; i++)
        if (myKey[i]) HashVal = (HashVal<<5) - HashVal + myKey[i];
    return ((tpr09HashIndex)HashVal%cbHashTabLen);
  } else {
    return ((tpr09HashIndex)cursorID->UniqueID%cbHashTabLen);
  }  
}
#endif 
