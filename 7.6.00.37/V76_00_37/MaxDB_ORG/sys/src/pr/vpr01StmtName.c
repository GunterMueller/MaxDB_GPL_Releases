/*!**************************************************************************

  module:    vpr01StmtName.c

  -------------------------------------------------------------------------

  responsible: BurkhardD, MarcoP

  special area:   CPCDrv | Client | Statementnamen 
  description : Verwaltung von Statementnamen

  last changed: 2000-09-08  9:05
  see also    :

 change history:

        2001-06-22 D025086 PTS 1110677 Do not drop ParseIdNull

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
#include <ctype.h>
#include "vpr07Macro.h"
#include "gpr03.h"
#include "vpr01StmtName.h"
#include "vpr01Con.h"
#include "vpr01SQL.h"
#include "vpr06ParseId.h"
#include "gpr03.h"
#include "vpr01ParseInfo.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

static void pr01StmtNameInitCont(tpr01_StmtNameContainer *Cont);

static tsp00_Int4 pr01StmtNameOpenCont(tpr01_StmtNameContainer *Cont);

static void pr01StmtNameCloseCont(tpr01_StmtNameContainer *Cont);

static tpr01_StmtNameDesc *pr01StmtNameEnumDesc(tpr01_StmtNameContainer *Cont);

static void pr01StmtNameResetIterator(tpr01_StmtNameContainer *Cont);

static tpr01_StmtNameDesc *pr01StmtNameAddDesc(tpr01_StmtNameContainer *StmtName, tpr05_String *stStmtName, tpr01_UniqueID UniqueID, sqlkaentry *ka);

static void pr01StmtNameDeleteDesc(tpr01_StmtNameDesc *StmtNameDesc);

static tpr01_StmtNameDesc *pr01StmtNameFindDesc(tpr01_StmtNameContainer *StmtName, tpr05_String *stStmtName, tpr01_UniqueID UniqueID);

static tsp00_Int4 pr01StmtNameSetIndex(tpr01_StmtNameContainer *Cont, tsp00_Int4 cbIndex);

static int pr01StmtNameGetSQL(tpr01_StmtNameDesc *StmtNameDesc, tpr05_String **pSQLStatement, tpr01_SQLStatementOptionEnum fOption);

static int pr01StmtNamePutSQL(tpr01_StmtNameDesc *StmtNameDesc, tpr05_String *pSQLStatement, tpr01_SQLStatementOptionEnum fOption);

static void pr01StmtNameTraceStmtName (tpr01_StmtNameDesc *StmtNameDesc, sqlcatype * sqlca);

/*static void pr01StmtNameSetStmtType(struct tpr01_StmtNameDesc *StmtNameDesc);*/

static void pr01StmtNameSetState(struct tpr01_StmtNameDesc *StmtNameDesc, tpr01_SQLStateBinary State);

static void pr01StmtNameDropParsId(tpr01_StmtNameDesc *StmtNameDesc, tpr01_SQLDesc *SQLDesc);

static void pr01StmtName_InitStmtNameStruct (tpr00_StmtNameStruct *StmtNameStruct);

#ifdef DYNAHASH  
static tpr09HashValue pr01StmtName_GetKey (const tpr09HashValue data_p, tsp00_Uint4* HashKeyLen);

static tsp00_Int4 pr01StmtName_CompareKey (const tpr09HashKey Item1,tsp00_Uint4 Item1Len, const tpr09HashKey Item2, tsp00_Uint4 Item2Len);

static void pr01StmtName_printKey (const tpr09HashValue data_p, FILE* datei);

static tpr09HashIndex pr01StmtName_hashFunc (const tpr09HashKey HashKey, tsp00_Uint4 cbHashKeyLen, tsp00_Uint4 cbHashTabLen);
/*
static void dumpKey ( tpr09HashValue data_p, char* praefix){
  tpr01_StmtNameDesc_StmtID *stmtID = (struct tpr01_StmtNameDesc_StmtID *)data_p;
  if ( stmtID->UniqueID == EmptyUniqueID_cpr01) {
    printDump("%s \"%.*s\"\n", praefix, stmtID->stmtName->cbLen, stmtID->stmtName->rawString);
  } else {
    printDump("%s \":%#08lx\"\n", praefix, stmtID->UniqueID);
  }
}
*/
#endif 
/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

/*!
  Function:     pr01StmtNameInitCont

  see also: 

  Description:  initializes StmtName Container 
    
  Arguments:
    *Cont [in|out] pointer to container
    return value: void
 */
static void pr01StmtNameInitCont(tpr01_StmtNameContainer *Cont)
{
#ifdef DYNAHASH  
  Cont->Desc = pr09NewHashDescriptor(20, 
                                     sizeof(tpr01_StmtNameDesc), 
                                     pr01StmtName_CompareKey, 
                                     pr01StmtName_hashFunc, 
                                     pr01StmtName_GetKey,
                                     pr01StmtName_printKey);
#else
  Cont->Desc = pr09NewDescriptor(3, sizeof(tpr01_StmtNameDesc));
#endif 
                                     
  Cont->cachedStmtID.UniqueID = EmptyUniqueID_cpr01;
  Cont->cachedStmtID.stmtName = NULL;     
  Cont->InitCont           = pr01StmtNameInitCont;
  Cont->OpenCont           = pr01StmtNameOpenCont;
  Cont->CloseCont          = pr01StmtNameCloseCont;
/*  Cont->GetCount           = NULL;*/
  Cont->SetIndex           = pr01StmtNameSetIndex;
  Cont->ResetIterator      = pr01StmtNameResetIterator;
  Cont->EnumDesc           = pr01StmtNameEnumDesc;
  Cont->DeleteDesc         = pr01StmtNameDeleteDesc;
  Cont->AddDesc            = pr01StmtNameAddDesc;
  Cont->FindDesc           = pr01StmtNameFindDesc;
  Cont->PutSQL             = pr01StmtNamePutSQL;
  Cont->GetSQL             = pr01StmtNameGetSQL;
  Cont->TraceStmtName      = pr01StmtNameTraceStmtName;
  Cont->DropParsId         = pr01StmtNameDropParsId;
  Cont->SetState           = pr01StmtNameSetState;
  Cont->InitStmtNameStruct = pr01StmtName_InitStmtNameStruct;
}


/*!
  Function:     pr01StmtNameDeleteCont

  see also: 

  Description:  deletes a StmtName Container 
    
  Arguments:
    *StmtName [in|out] pointer to a container
    return value: void
 */
void pr01StmtNameDeleteCont(tpr01_StmtNameContainer *StmtName)
{
  if (StmtName) {
    if (pr01StmtNameOpenCont(StmtName)) {
      /* liefert <> 0 wenn StmtName Descriptoren vorhanden sind */
      tpr01_StmtNameDesc *StmtNameDesc;
      while(StmtNameDesc = pr01StmtNameEnumDesc(StmtName))
	pr01StmtNameDeleteDesc(StmtNameDesc);
    }
    if (StmtName->Desc){
#ifdef DYNAHASH  
      pr09DeleteHashDescriptor(StmtName->Desc);
#else
      pr09DeleteDescriptor(StmtName->Desc);
#endif 
    }  
    pr03mFree((tsp00_Uint1*)StmtName, "StmtName");
  }
}

tpr01_StmtNameContainer *pr01StmtNameNewCont()
{
  tpr01_StmtNameContainer *StmtName;
  StmtName = pr03mAllocat(sizeof(tpr01_StmtNameContainer), "StmtName");
  if (StmtName)
    pr01StmtNameInitCont(StmtName);
  return StmtName;
}


static tsp00_Int4 pr01StmtNameOpenCont(tpr01_StmtNameContainer *StmtName)
{
  if (StmtName->Desc) {
#ifdef DYNAHASH  
    pr09ResetHashItemIterator(StmtName->Desc);
#else
    pr09SetIndex(StmtName->Desc, -1);
#endif 
    return 1;
  } else {
    return 0;
  }
}

/*!
  Function:     pr01StmtNameAddDesc

  see also: 

  Description:  adds a new Statement Descriptor to a Statement Container
    
  Arguments:
    *StmtName  [in] pointer to StmtContainer 
    stStmtName [in] Name of the new Stmt Descriptor. If StmtNameStruct points to a
                    NULL Value the Parameter UniqueID must set. 
    UniqueID   [in] uniqueID of the Statement generated by precompiling.
                    If you pass an unique value not equal to 
		    EmptyUniqueID_cpr01 the function will generates a StmtName
                    by it selfs. 
    
    *ka        [in] pointer to an entry of sqlka-area. If ka points to NULL an empty
                    kaentry will generated autimaticly.

  return value: tpr01_StmtNameDesc - pointer to the new Statement Descriptor 
                which was inserted into the Statement Container

  Remarks: pr01StmtNameAddDesc dosn't check if an StmtName already is in use. A new entry will generate at all. 
 */
static tpr01_StmtNameDesc *pr01StmtNameAddDesc(tpr01_StmtNameContainer *StmtName, tpr05_String *stStmtName, tpr01_UniqueID UniqueID, sqlkaentry *ka)
{
  M90TRACE(M90_TR_ENTRY, "pr01StmtNameAddDesc", 0);
  M90TRACE(M90_TR_WORD, "kalineno", &ka->kalineno);
  {
    tpr01_StmtNameDesc *StmtNameDesc;  
#ifdef DYNAHASH  
    tpr09_HashTableError hTabErr; 
    StmtNameDesc = pr09GetNewHashItem(StmtName->Desc);
#else
    StmtNameDesc = pr09AddItem(StmtName->Desc);
#endif 
    StmtNameDesc->DescType = StmtNameDesc_epr01;
    StmtNameDesc->StmtName = StmtName;
    StmtNameDesc->sn = pr03mAllocat(sizeof(sqlsnentry), "StmtNameDesc->sn");
    pr01StmtNameSnEntryInit(StmtNameDesc->sn);
    StmtNameDesc->StmtName->InitStmtNameStruct(&(StmtNameDesc->StmtNameStruct));
    if (UniqueID != EmptyUniqueID_cpr01) {
      /*generate an implizit statement name*/
      StmtNameDesc->StmtNameStruct.stStmtName.cbLen = 
      sp77sprintfUnicode (StmtNameDesc->StmtNameStruct.stStmtName.encodingType,
                          StmtNameDesc->StmtNameStruct.stStmtName.rawString,
                          StmtNameDesc->StmtNameStruct.stStmtName.cbMaxLen,
                          ":%#08lx", 
                          UniqueID);
    }
    else {
      /*take the given statement name*/  
            /*take the given cursor name*/  
      StmtNameDesc->StmtNameStruct.stStmtName.cbLen = stStmtName->cbLen;
      StmtNameDesc->StmtNameStruct.stStmtName.encodingType = stStmtName->encodingType;
      memcpy(StmtNameDesc->StmtNameStruct.stStmtName.rawString, stStmtName->rawString, stStmtName->cbLen);
    }
    StmtNameDesc->StmtID.UniqueID = UniqueID;
    StmtNameDesc->StmtID.stmtName = &(StmtNameDesc->StmtNameStruct.stStmtName);
    /*add the descriptor to the statement container hash index*/
    
#ifdef DYNAHASH  
    hTabErr = pr09AddHashItemToHash(StmtName->Desc, 
                                    &(StmtNameDesc->StmtID), 
                                    sizeof(tpr01_StmtNameDesc_StmtID),
                                    (tpr09HashValue*)StmtNameDesc);
#endif
    
    /* alloc & init new SQLStatetement */
    StmtNameDesc->SQLStatement = pr05IfCom_String_NewDynString(0, NULL);
    StmtNameDesc->fOption      = First_epr01;   /* type of SQLStatement is new Statement */
    StmtNameDesc->State        = Empty_bpr01;
   
    StmtNameDesc->StmtTypeDesc.StmtType      = KeywordNOT_ANALYZED_epr01;
    StmtNameDesc->StmtTypeDesc.StmtOption    = KeywordNOT_FOUND_epr01;
    StmtNameDesc->StmtTypeDesc.CursorPos     = 0;
    StmtNameDesc->StmtTypeDesc.CursorLength  = 0;
    StmtNameDesc->StmtTypeDesc.CursorType    = CPR_S_UNKNOWN;
    StmtNameDesc->StmtTypeDesc.FetchPosType  = CPR_S_UNKNOWN;
    StmtNameDesc->StmtTypeDesc.UsingPos      = 0;
    StmtNameDesc->StmtTypeDesc.Cacheable     = notcacheable_epr01;
    
    StmtNameDesc->ParseInfoDesc = NULL;

    memset(&StmtNameDesc->KnlSessionID, '\0', sizeof(tpr00_SessionID));

    StmtNameDesc->ka = pr01cDupKa(ka);
    StmtNameDesc->ore = pr03mAllocat(sizeof(sqlorentry), "StmtNameDesc->ore");
    pr03cOrInit(StmtNameDesc->ore);    
    StmtNameDesc->cue = pr03mAllocat(sizeof(sqlcuentry), "StmtNameDesc->cue");
    pr03cCuInit(StmtNameDesc->cue);
    
    M90TRACE(M90_TR_EXIT, "pr01StmtNameAddDesc", 0);    
    M90TRACE(M90_TR_HANDLE, "StmtNameDesc", &StmtNameDesc);    
    return(StmtNameDesc);
  }
}

static void pr01StmtNameDeleteDesc(tpr01_StmtNameDesc *StmtNameDesc)
{
#ifdef DYNAHASH  
  if (NULL != pr09FindHashItem(  StmtNameDesc->StmtName->Desc, 
                                 &(StmtNameDesc->StmtID), 
                                 sizeof(tpr01_StmtNameDesc_StmtID)
                               )){
#else
  int cbIndex = pr09ItemFindIndex(StmtNameDesc->StmtName->Desc, StmtNameDesc);
  if (cbIndex >= 0) {
#endif
    PR07_HEAPCHECK(NULL);
    if (StmtNameDesc->sn)
      pr03mFree(StmtNameDesc->sn, "StmtNameDesc->sn");
    PR07_HEAPCHECK(NULL);
    if (StmtNameDesc->ka)
      pr01cFreeKa(StmtNameDesc->ka);
    PR07_HEAPCHECK(NULL);
    if (StmtNameDesc->ore)
      pr03mFree(StmtNameDesc->ore, "StmtNameDesc->ore");
    PR07_HEAPCHECK(NULL);
    if (StmtNameDesc->cue)
      pr03mFree(StmtNameDesc->cue, "StmtNameDesc->cue");
      
    /* deletes SQLStatement and frees memory, but only
       if SQLStatement is not a reference */
    PR07_HEAPCHECK(NULL);
    if ( StmtNameDesc->fOption != Reference_epr01 )
      pr05IfCom_String_DeleteString(StmtNameDesc->SQLStatement);

    /*delete statement name and release memory*/
    pr05IfCom_String_DeleterawString(&(StmtNameDesc->StmtNameStruct.stStmtName));
    PR07_HEAPCHECK(NULL);

    if ( StmtNameDesc->ParseInfoDesc )
      StmtNameDesc->ParseInfoDesc->ParseInfo->DeleteDesc(StmtNameDesc->ParseInfoDesc, DROP_PARSEID);
    PR07_HEAPCHECK(NULL);

#ifdef DYNAHASH  
    pr09DeleteHashItem(  StmtNameDesc->StmtName->Desc, 
                         &(StmtNameDesc->StmtID), 
                         sizeof(tpr01_StmtNameDesc_StmtID)
                       );
#else
    pr09CloseItemEx(StmtNameDesc->StmtName->Desc, cbIndex);
#endif 
  }
}


/*!
  Function:     pr01StmtNameEnumDesc

  see also: 

  Description:  Iterator Function for Statement Container
    
  Arguments:
    *StmtName [in] pointer to Statement Container

  return value: 
    <UL>
        <LI>tpr01_StmtNameDesc* : pointer to then next StatementDescriptor 
                in the Statement Container.
        <LI>null pointer : if no next item in Statement Container exists
    </UL>

 */
static tpr01_StmtNameDesc *pr01StmtNameEnumDesc(tpr01_StmtNameContainer *StmtName)
{
#ifdef DYNAHASH  
  tpr01_StmtNameDesc *StmtNameDesc= pr09HashItemIteratorNextItem(StmtName->Desc);
#else
  tpr01_StmtNameDesc *StmtNameDesc=pr09GetNextItem(StmtName->Desc);
#endif 
  return(StmtNameDesc);
}

static void pr01StmtNameCloseCont(tpr01_StmtNameContainer *StmtName)
{
#ifdef DYNAHASH  
  pr09DeleteAllHashItems(StmtName->Desc);
#else
  pr09CloseAllItems(StmtName->Desc);
#endif 
}

static tsp00_Int4 pr01StmtNameSetIndex(tpr01_StmtNameContainer *StmtName, tsp00_Int4 cbIndex)
{
#ifdef DYNAHASH  
  return 0;
#else
  return(pr09SetIndex(StmtName->Desc, cbIndex));
#endif 
}

static void pr01StmtNameResetIterator(tpr01_StmtNameContainer *Cont){
  if (Cont && Cont->Desc) {
#ifdef DYNAHASH  
    pr09ResetHashItemIterator(Cont->Desc);
#else
    pr09SetIndex(Cont->Desc, -1);
#endif 
  }
}

#ifdef DYNAHASH  
static tpr01_StmtNameDesc *pr01StmtNameFindDesc(tpr01_StmtNameContainer *StmtName, tpr05_String *stStmtName, tpr01_UniqueID UniqueID)
{
  M90TRACE(M90_TR_ENTRY, "pr01StmtNameFindDesc", 0);
  {
    tpr01_StmtNameDesc *StmtNameDesc=NULL; 
    
    StmtName->cachedStmtID.stmtName = stStmtName;
    StmtName->cachedStmtID.UniqueID = UniqueID;
  
    StmtNameDesc = (tpr01_StmtNameDesc *)pr09FindHashItem(  StmtName->Desc, 
                                                            &(StmtName->cachedStmtID), 
                                                            sizeof(tpr01_StmtNameDesc_StmtID)
                                                          );    
    M90TRACE(M90_TR_EXIT, "pr01StmtNameFindDesc", 0);
    M90TRACE(M90_TR_HANDLE, "StmtNameDesc", &StmtNameDesc);
    return(StmtNameDesc);    
  }
}
#else
static tpr01_StmtNameDesc *pr01StmtNameFindDesc(tpr01_StmtNameContainer *StmtName, tpr05_String *stStmtName, tpr01_UniqueID UniqueID)
{
  M90TRACE(M90_TR_ENTRY, "pr01StmtNameFindDesc", 0);
  {
    tsp00_Int4 cbOldIndex = StmtName->SetIndex(StmtName, -1);
    tpr01_StmtNameDesc *StmtNameDesc=NULL;
    if ( UniqueID == EmptyUniqueID_cpr01) {
      while(StmtNameDesc=StmtName->EnumDesc(StmtName)) {
        if (! pr05IfCom_String_strcmp(stStmtName,&(StmtNameDesc->StmtNameStruct.stStmtName))){
	  break;
	}
      }
    }
    else {
      while(StmtNameDesc=StmtName->EnumDesc(StmtName)) {
	if (UniqueID == StmtNameDesc->StmtID.UniqueID) {
	  break;
	}
      }
    }

    if (!StmtNameDesc)
      StmtName->SetIndex(StmtName, cbOldIndex);
    M90TRACE(M90_TR_EXIT, "pr01StmtNameFindDesc", 0);
    M90TRACE(M90_TR_HANDLE, "StmtNameDesc", &StmtNameDesc);
    return(StmtNameDesc);    
  }
}
#endif 


void pr01StmtNameSnEntryInit(sqlsnentry *sn)
{
  memset(sn->snname, 0, sizeof(sn->snname));
  sn->sntype    = cpr_is_statement;
  sn->snstmtp   = NULL;
  sn->sncount   = 0;
  sn->snindex   = 0;
  sn->snbuflen  = 0;
  sn->snstmtlen = 0;
  sn->snsession = ActualSession_cpr00;
  pr06ParseIdCopy(&sn->snParseInfo.ParseId, ParseIdNull_cpr00);
}

/*!
  Function:     pr01StmtNamePutSQL

  see also: 

  Description:  Sets sql statement, length and encoding type 
                in statement descriptor.
    
  Arguments:
    StmtNameDesc  [in] pointer to Stmt Descriptor
    pSQLStatement [in] tpr05_String which contains the sql-statement.
    fOption       [in] controls processing of pSQLStatement->rawString
    <UL>
        <LI>fOption == First_epr01     => a new sqlstatement, overwrite buffer 
        <LI>fOption == Append_epr01    => next part of sqlstatement, append on buffer 
        <LI>fOption == Reference_epr01 => only Referenz to sql-statement will be stored 
    </UL>

  return value: int 
    <UL>
        <LI>0 : error
        <LI>1 : sql-statement processing succeeds
    </UL>
 */
static int pr01StmtNamePutSQL(tpr01_StmtNameDesc *StmtNameDesc, tpr05_String* pSQLStatement, tpr01_SQLStatementOptionEnum fOption)
{
  M90TRACE(M90_TR_ENTRY, "pr01StmtNamePutSQL", 0);
  M90TRACE(M90_TR_HANDLE, "StmtNameDesc", &StmtNameDesc);
  M90TRACE(M90_TR_ENUM, "fOption", &fOption);
  PR07_CHECKDESC(StmtNameDesc, StmtNameDesc_epr01);  
  {
    if (pSQLStatement) {
      tsp00_Uint4 Len, Offset;
      if (pSQLStatement->cbLen != 0) 
        {
          Len = pSQLStatement->cbLen;
        }
      else 
        {
          /*Error nach Bufferumstellung ohne Nullterminator Länge nicht entscheidbar
               
            if (encodingType == sp77encodingAscii)
            {
            Len = strlen(szSQLStatement);
            }
            else
            {
            Len = 2 * sp81UCS2strlen((tsp81_UCS2Char*) szSQLStatement);	
            }*/
          M90TRACE(M90_TR_EXIT, "pr01StmtNamePutSQL", 0);
          return(0);
        }
      /* Reset the analysed state from the Statement Desc */
      StmtNameDesc->State &= ~Analysed_bpr01;
      if (fOption == First_epr01 || fOption == Append_epr01) 
        {

	  if (fOption == First_epr01) 
            {
	      Offset = 0;
            }
	  else 
            {
	      Offset = StmtNameDesc->SQLStatement->cbLen;
            }
	  if (Len+Offset > StmtNameDesc->SQLStatement->cbMaxLen) 
            {
	      if (  StmtNameDesc->SQLStatement->allocationType == allocated_epr05
                    &&StmtNameDesc->SQLStatement->cbMaxLen > 0)
                {
	          StmtNameDesc->SQLStatement->rawString = pr03mReallocat(StmtNameDesc->SQLStatement->rawString, Len+Offset, "StmtNameDesc->SQLStatement->rawString");
                }
	      else 
                {
	          StmtNameDesc->State |= Allocated_bpr01;
	          StmtNameDesc->SQLStatement->rawString = pr03mAllocat(Len, "StmtNameDesc->SQLStatement->rawString");
                  StmtNameDesc->SQLStatement->allocationType = allocated_epr05;
                }
	    
	      if (!StmtNameDesc->SQLStatement->rawString) 
                {
	          pr07CheckAssert(TRUE);
	          return 0;
                }	    
	      StmtNameDesc->SQLStatement->cbMaxLen = Len+Offset;
            }
	  StmtNameDesc->SQLStatement->cbLen = Len+Offset;
	  StmtNameDesc->SQLStatement->encodingType = pSQLStatement->encodingType;
	  StmtNameDesc->fOption = fOption;
	  /* copy SQL-Statement to StmtNameDesc*/
	  memcpy(&StmtNameDesc->SQLStatement->rawString[Offset], pSQLStatement->rawString, Len);
        }
      else 
        {
	  if (fOption == Reference_epr01) 
            {
	      StmtNameDesc->SQLStatement->cbLen = Len;
	      /* if a Statement already allocated free it */
	      /* but don't free it if the new address is same to the old one 
	         or if Statement is only a referenz */
	      if (   StmtNameDesc->fOption != Reference_epr01
                     && StmtNameDesc->SQLStatement->rawString != pSQLStatement->rawString) 
                {
                  /* deletes SQLStatement and frees memory*/
	          pr05IfCom_String_DeleteString(StmtNameDesc->SQLStatement);
                }
	      StmtNameDesc->SQLStatement = pSQLStatement;      
              StmtNameDesc->fOption = Reference_epr01;
            }
	  else
            pr07CheckAssert(TRUE);
        }
      pSQLStatement = StmtNameDesc->SQLStatement;
    }
    M90TRACE(M90_TR_EXIT, "pr01StmtNamePutSQL", 0);
    return(1);
  }
}
/*!
  Function:     pr01StmtNameGetSQL

  see also: 

  Description:  Gets SQL-Command, length and encoding typ for a given 
                SQL-Command. Depending on fOption a copy or a reference 
                of StmtDesc->SQLStatement will be stored in pSQLStatement.

  Arguments:
    *StmtNameDesc [in]       pointer to a statement descriptor
    *pSQLStatement[in | out] pointer to a tpr05_String for output sql-statement
    fOption       [in]       controls type of pSQLStatement->rawString;
    <UL>
        <LI>fOption == Reference_epr01 => only Referenz to sql-statement will be stored
        <LI>fOption == someone else    => copy off sql-statement will be created
    </UL>

  return value: int 
    <UL>
        <LI>0 : error
        <LI>1 : processing succeeds
    </UL>
 */
static int pr01StmtNameGetSQL(tpr01_StmtNameDesc *StmtNameDesc, tpr05_String **pSQLStatement, tpr01_SQLStatementOptionEnum fOption)
{
  M90TRACE(M90_TR_ENTRY, "pr01StmtNameGetSQL", 0);
  M90TRACE(M90_TR_HANDLE, "StmtNameDesc", &StmtNameDesc);
  PR07_CHECKDESC(StmtNameDesc, StmtNameDesc_epr01);  
  {
    if (! *pSQLStatement || fOption == Reference_epr01) 
      /* copy by reference */
      {
        *pSQLStatement = StmtNameDesc->SQLStatement;
         
        M90TRACE(M90_TR_EXIT, "pr01StmtNameGetSQL", 0);
        M90TRACE(M90_TR_STRING, "SQLStatement", StmtNameDesc->SQLStatement->rawString);
        return(1);
      }
    if ((*pSQLStatement)->cbMaxLen >= StmtNameDesc->SQLStatement->cbLen)
      /* space is enough for copying */
      /* copy by value */ 
      {
        (*pSQLStatement)->cbLen        = StmtNameDesc->SQLStatement->cbLen;   
        (*pSQLStatement)->encodingType = StmtNameDesc->SQLStatement->encodingType;   
        memcpy((*pSQLStatement)->rawString, StmtNameDesc->SQLStatement->rawString, StmtNameDesc->SQLStatement->cbLen)
          M90TRACE(M90_TR_EXIT, "pr01StmtNameGetSQL", 0);
        return(1);
      }
    return (0);
  }
}

/*!
  Function:     pr01StmtNameTraceStmtName

  see also: 
  
  Description: Puts statement name to trace  
  
  Arguments:
    StmtNameDesc [in] pointer to tpr01_StmtNameDesc
    sqlca        [in] sqlca area
    
  return value: void
 */
static void pr01StmtNameTraceStmtName (struct tpr01_StmtNameDesc *StmtNameDesc, sqlcatype *sqlca)
{
  sqlratype *sqlrap = sqlca->sqlrap;
  sqltatype      *tap = sqlrap->rasqltap;  
  short           *ll = &sqlrap->rasqltap->tastr80l;
  struct SQLERROR errmsg;
  *ll = sp77sprintfUnicode (sp77encodingUTF8,
                            tap->tastr80,
                            sizeof(tap->tastr80),
                            "Statement Name : %=.*S",
                            StmtNameDesc->StmtNameStruct.stStmtName.encodingType,
                            StmtNameDesc->StmtNameStruct.stStmtName.cbLen, 
                            StmtNameDesc->StmtNameStruct.stStmtName.rawString);  
  p03tvfwritetrace (sqlrap, &errmsg);
}

static void pr01StmtNameDropParsId(tpr01_StmtNameDesc *StmtNameDesc, tpr01_SQLDesc *SQLDesc)
{
  if (SQLDesc->ConDesc) {
    sqlcatype *sqlca = SQLDesc->ConDesc->sqlca;
    sqlxatype *sqlxa = SQLDesc->ConDesc->sqlxa;
    sqlgaentry *ga   = SQLDesc->ConDesc->ga;
    sqlkaentry *ka = StmtNameDesc->ka;  
    if (!pr06ParseIdIsEqual(&ka->kaParseInfo.ParseId, ParseIdNull_cpr00)) {    
      p01bdrop_parsid (sqlca, sqlxa, ka->kaParseInfo.ParseId);
      pr06ParseIdDrop(sqlca, sqlxa, ga, (tpr06_ParseIDCache *)SQLDesc->ConDesc->ParseIdCache, &ka->kaParseInfo.ParseId);
      StmtNameDesc->ka->kastate = cpr_state_empty;
      StmtNameDesc->ka->kaParseInfo.sp1rFunctionCode = csp1_nil_fc;  
    }
  }
}

static void pr01StmtNameSetState(struct tpr01_StmtNameDesc *StmtNameDesc, tpr01_SQLStateBinary State)
{
  if (State != Empty_bpr01)
    StmtNameDesc->State |= State;
  else
    StmtNameDesc->State = State;
}

/*!
  Function:     pr01StmtName_InitStmtNameStruct

  see also: 
  
  Description:  initialized a tpr00_StmtNameStruct

  Arguments:
    StmtNameStruct [in] tpr00_StmtNameStruct
 
  return value: void
 */
static void pr01StmtName_InitStmtNameStruct (tpr00_StmtNameStruct *StmtNameStruct)
{
  StmtNameStruct->stStmtName.allocationType = constant_epr05;
  pr05IfCom_String_InitString(&(StmtNameStruct->stStmtName), 
                              &(StmtNameStruct->private_StrBuf), 
                              sizeof(StmtNameStruct->private_StrBuf), 
                              sp77encodingAscii, 
                              constant_epr05);
}
#ifdef DYNAHASH  
static tpr09HashValue pr01StmtName_GetKey (const tpr09HashValue data_p, tsp00_Uint4* HashKeyLen){
  tpr01_StmtNameDesc_StmtID* stmtID=&((tpr01_StmtNameDesc *)data_p)->StmtID;
  *HashKeyLen = sizeof(tpr01_StmtNameDesc_StmtID);
  return stmtID;
}

static tsp00_Int4 pr01StmtName_CompareKey (const tpr09HashKey Item1,tsp00_Uint4 Item1Len, const tpr09HashKey Item2, tsp00_Uint4 Item2Len){
  tpr01_StmtNameDesc_StmtID *stmtName1 = (tpr01_StmtNameDesc_StmtID*) Item1;
  tpr01_StmtNameDesc_StmtID *stmtName2 = (tpr01_StmtNameDesc_StmtID*) Item2;

  if (stmtName1->UniqueID == EmptyUniqueID_cpr01){ 
    if (stmtName2->UniqueID == EmptyUniqueID_cpr01){
       return  (! pr05IfCom_String_strcmp(stmtName1->stmtName, stmtName2->stmtName));
    } else {
      return 0;
    }       
  } else if (stmtName2->UniqueID != EmptyUniqueID_cpr01){
    return (stmtName1->UniqueID == stmtName2->UniqueID)?1:0; 
  }
  return 0;
}

static void pr01StmtName_printKey (const tpr09HashValue data_p, FILE* datei){
  tpr01_StmtNameDesc_StmtID *stmtID = &((struct tpr01_StmtNameDesc *)data_p)->StmtID;
  if ( stmtID->UniqueID == EmptyUniqueID_cpr01) {
    fprintf(datei, "\"%.*s\"\n", data_p, stmtID->stmtName->cbLen, stmtID->stmtName->rawString);
  } else {
    fprintf(datei,"\":%#08lx\"\n",stmtID->UniqueID);
  }
}

static tpr09HashIndex pr01StmtName_hashFunc (const tpr09HashKey HashKey, tsp00_Uint4 cbHashKeyLen, tsp00_Uint4 cbHashTabLen)
{
  tpr01_StmtNameDesc_StmtID *stmtID = (tpr01_StmtNameDesc_StmtID*) HashKey; 
  if ( stmtID->UniqueID == EmptyUniqueID_cpr01) {
    tsp00_Uint4 i;
    tpr09HashIndex HashVal=0;
    const char*	myKey = (char*) stmtID->stmtName->rawString;
    cbHashKeyLen = stmtID->stmtName->cbLen;
    for(i=0; i < cbHashKeyLen; i++)
        if (myKey[i]) HashVal = (HashVal<<5) - HashVal + myKey[i];
    return ((tpr09HashIndex)HashVal%cbHashTabLen);
  } else {
    return ((tpr09HashIndex)stmtID->UniqueID%cbHashTabLen);
  }  
}
#endif 

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
