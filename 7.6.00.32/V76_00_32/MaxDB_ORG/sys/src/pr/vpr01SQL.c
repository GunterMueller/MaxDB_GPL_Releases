/*!================================================================
 module:    vpr01SQL.c

 responsible: BurkhardD

 special area:   CPCDrv | Client | SQLStatment Verwaltung

 description: SQL Verwaltung

 see:
last changed: 2000-08-07  11:01 Marco Paskamp     unicode enhancement




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

#include "gmt90.h"
#include "vpr01SQL.h"
#include "vpr01Env.h"
#include "vpr01StmtName.h"
#include "vpr01Cursor.h"
#include "vpr01Error.h"
#include "vpr03Segment.h"
#include "gpr03.h"
#include "gpr08.h"
#include "vpr04Long.h"


/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

static void pr01SQLInitCont(tpr01_SQLContainer *Cont, tpr01_EnvDesc *EnvDesc);

static tsp00_Int4 pr01SQLOpenCont(tpr01_SQLContainer *Cont);

static tsp00_Int4 pr01SQLGetCount(tpr01_SQLContainer *Cont);

static void pr01SQLCloseCont(tpr01_SQLContainer *Cont);

static tpr01_SQLDesc *pr01SQLEnumDesc(tpr01_SQLContainer *Cont);

static tpr01_SQLDesc *pr01SQLAddDesc(tpr01_SQLContainer *Cont);

static void pr01SQLDeleteDesc(tpr01_SQLDesc *SQLDesc);

static tsp00_Int4 pr01SQLSetIndex(tpr01_SQLContainer *Cont, tsp00_Int4 cbIndex);

static int pr01SQLPutSQL(tpr01_SQLDesc *SQLDesc, tpr05_String *pSQLStatement, tpr01_SQLStatementOptionEnum fOption);

static int pr01SQLGetSQL(tpr01_SQLDesc *SQLDesc, tpr05_String **pSQLStatement, tpr01_SQLStatementOptionEnum fOption);

static void pr01SQLOpen (struct tpr01_SQLDesc *SQLDesc);

static void pr01SQLPrepare (struct tpr01_SQLDesc *SQLDesc);

static boolean pr01SQLExecute (struct tpr01_SQLDesc *SQLDesc);

static boolean pr01SQLTryExecute (struct tpr01_SQLDesc *SQLDesc);

static void pr01SQLClose (struct tpr01_SQLDesc *SQLDesc);

static void pr01SQLDescribe (struct tpr01_SQLDesc *SQLDesc);

static void pr01SQLRelease (struct tpr01_SQLDesc *SQLDesc);

static tpr01_SQLStateBinary pr01SQLState (struct tpr01_SQLDesc *SQLDesc);

static sqlcatype * pr01SQLGetSqlca (struct tpr01_SQLDesc *SQLDesc);

static sqlxatype *pr01SQLGetSqlxa (struct tpr01_SQLDesc *SQLDesc);

static tsp00_Int4 pr01SQLSetError(struct tpr01_SQLDesc *SQLDesc, tpr_runtime_errors_Enum ErrorEnum);

static void pr01SQLInitDesc (struct tpr01_SQLDesc *SQLDesc, sqlxatype *sqlxa);

static void pr01SQLSetState(struct tpr01_SQLDesc *SQLDesc);

static void pr01SQLBeforeExecute(struct tpr01_SQLDesc *SQLDesc);

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

static void pr01SQLInitCont(tpr01_SQLContainer *SQL, tpr01_EnvDesc *EnvDesc)
{
  SQL->Desc = pr09NewDescriptor(3, sizeof(tpr01_SQLDesc));
  SQL->EnvDesc = EnvDesc;
  SQL->InitCont   = pr01SQLInitCont;
  SQL->OpenCont   = pr01SQLOpenCont;
  SQL->CloseCont  = pr01SQLCloseCont;
  SQL->GetCount   = pr01SQLGetCount;
  SQL->EnumDesc   = pr01SQLEnumDesc;
  SQL->AddDesc    = pr01SQLAddDesc;
  SQL->DeleteDesc    = pr01SQLDeleteDesc;
  SQL->SetIndex   = pr01SQLSetIndex;
  SQL->PutSQL     = pr01SQLPutSQL;
  SQL->GetSQL     = pr01SQLGetSQL;
  SQL->Close      = pr01SQLClose;
  SQL->Prepare    = pr01SQLPrepare;
  SQL->Execute    = pr01SQLExecute;
  SQL->TryExecute = pr01SQLTryExecute;
  SQL->Open       = pr01SQLOpen;
  SQL->Describe   = pr01SQLDescribe;
  SQL->State      = pr01SQLState;
  SQL->SetError   = pr01SQLSetError;
  SQL->GetSqlca   = pr01SQLGetSqlca;
  SQL->GetSqlxa   = pr01SQLGetSqlxa;
  SQL->InitDesc   = pr01SQLInitDesc;
  SQL->Release    = pr01SQLRelease;
}

void pr01SQLDeleteCont(tpr01_SQLContainer *SQL)
{
  if (SQL) {
    if (pr01SQLOpenCont(SQL)) {
      tpr01_SQLDesc *SQLDesc;
      while(SQLDesc = SQL->EnumDesc(SQL))
	SQL->DeleteDesc(SQLDesc);
    }
    pr09DeleteDescriptor(SQL->Desc);
    SQL->Desc = NULL;
    pr03mFree((tsp00_Uint1*)SQL, "SQL");
  }
}

tpr01_SQLContainer *pr01SQLNewCont(tpr01_EnvDesc *EnvDesc)
{
  tpr01_SQLContainer *SQL;
  SQL = pr03mAllocat(sizeof(tpr01_SQLContainer), "SQL");
  if (SQL)
    pr01SQLInitCont(SQL, EnvDesc);
  return SQL;
}

static tsp00_Int4 pr01SQLOpenCont(tpr01_SQLContainer *SQL)
{
  if (!SQL->Desc) {
    SQL->Desc = pr09NewDescriptor(10, sizeof(tpr01_SQLDesc));
  }
  pr09SetIndex(SQL->Desc, -1);
  return(SQL->GetCount(SQL));
}

static tsp00_Int4 pr01SQLGetCount(tpr01_SQLContainer *SQL)
{
  return(pr09GetItemCount(SQL->Desc));
}

static tpr01_SQLDesc *pr01SQLEnumDesc(tpr01_SQLContainer *SQL)
{
  tpr01_SQLDesc *Desc=pr09GetNextItem(SQL->Desc);
  return(Desc);
}

static void pr01SQLCloseCont(tpr01_SQLContainer *SQL)
{
  pr09CloseAllItems(SQL->Desc);
}

static tpr01_SQLDesc *pr01SQLAddDesc(tpr01_SQLContainer *SQL)
{
  M90TRACE(M90_TR_ENTRY, "pr01SQLAddDesc", 0);
  {
    tpr01_SQLDesc *SQLDesc;
    SQLDesc = pr09AddItem(SQL->Desc);
    SQLDesc->DescType = SQLDesc_epr01;
    SQLDesc->SQL = SQL;
    pr01SQLInitDesc(SQLDesc, NULL);
    SQLDesc->ErrorDesc    = pr01NewErrorDesc();
    M90TRACE(M90_TR_EXIT, "pr01SQLAddDesc", 0);
    M90TRACE(M90_TR_HANDLE, "SQLDesc", &SQLDesc);
    return(SQLDesc);
  }
}

static void pr01SQLDeleteDesc(tpr01_SQLDesc *SQLDesc)
{
  int cbIndex = pr09ItemFindIndex(SQLDesc->SQL->Desc, SQLDesc);
  if (cbIndex >= 0) {
    pr01DeleteErrorDesc(SQLDesc->ErrorDesc);
    pr09CloseItemEx(SQLDesc->SQL->Desc, cbIndex);
  }
}


static tsp00_Int4 pr01SQLSetIndex(tpr01_SQLContainer *SQL, tsp00_Int4 cbIndex)
{
  return(pr09SetIndex(SQL->Desc, cbIndex));
}

static void pr01SQLPrepare (struct tpr01_SQLDesc *SQLDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01SQLPrepare", 0);
  PR07_CHECKDESC(SQLDesc, SQLDesc_epr01);
  {
    sqlcatype *sqlca = pr01SQLGetSqlca(SQLDesc);
    sqlxatype *sqlxa = pr01SQLGetSqlxa(SQLDesc);
    tpr01_StmtNameDesc *StmtNameDesc = SQLDesc->StmtNameDesc;
    sqlxa->xaSQLDesc = SQLDesc;
    if (StmtNameDesc) {
      if (StmtNameDesc->State & Prepared_bpr01
          && ! pr06ParseIdKnlSessionIDCompare(&(StmtNameDesc->KnlSessionID), &(SQLDesc->ConDesc->ga->gaKnlSessionID)) ) {
        if(SQLDesc->ConDesc->ParseInfoCache){
          tpr01_ParseInfoContainer *ParseInfo = SQLDesc->ConDesc->ParseInfoCache;
          ParseInfo->DeleteDesc(StmtNameDesc->ParseInfoDesc, DROP_PARSEID);
          StmtNameDesc->ParseInfoDesc = NULL;
        }
        else {
          tpr01_StmtNameContainer *StmtName = StmtNameDesc->StmtName;
          StmtName->DropParsId(StmtNameDesc, SQLDesc);
        }  
        StmtNameDesc->State &= ~Prepared_bpr01;
      }
      if ( !(StmtNameDesc->State & Analysed_bpr01)) {
        if (StmtNameDesc->SQLStatement->rawString) {
          pr05cAnalyseSQLStatement (StmtNameDesc->SQLStatement, &(StmtNameDesc->StmtTypeDesc));
          StmtNameDesc->State |= Analysed_bpr01;
        }
      }
    }
    if (pr01cParsePrepare(SQLDesc)) {
      StmtNameDesc->State |= Prepared_bpr01;
      M90TRACE(M90_TR_ENUM, "State", &StmtNameDesc->State);
    }
  }
  M90TRACE(M90_TR_EXIT, "pr01SQLPrepare", 0);
}

static void pr01SQLOpen (struct tpr01_SQLDesc *SQLDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01SQLOpen", 0);
  PR07_CHECKDESC(SQLDesc, SQLDesc_epr01);
  {
    sqlcatype *sqlca = pr01SQLGetSqlca(SQLDesc);
    sqlxatype *sqlxa = pr01SQLGetSqlxa(SQLDesc);
    sqlxa->xaSQLDesc = SQLDesc;
    pr01CursorAlterParseid(SQLDesc);
    pr01SQLBeforeExecute(SQLDesc);
    pr01cOpen(SQLDesc);
    if (sqlca->sqlcode == 0 || sqlca->sqlcode == 100) {
      pr01SQLSetState(SQLDesc);
    }
  }
  M90TRACE(M90_TR_EXIT, "pr01SQLOpen", 0);
}

static void pr01SQLClose (struct tpr01_SQLDesc *SQLDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01SQLClose", 0);
  {
    pr01SQLBeforeExecute(SQLDesc);
  }
  M90TRACE(M90_TR_EXIT, "pr01SQLClose", 0);
}

static tpr01_SQLStateBinary pr01SQLState (struct tpr01_SQLDesc *SQLDesc)
{
  PR07_CHECKDESC(SQLDesc, SQLDesc_epr01);
  M90TRACE(M90_TR_ENUM, "pr01SQLState", &SQLDesc->StmtNameDesc->State);
  return SQLDesc->StmtNameDesc->State;
}

static sqlcatype *pr01SQLGetSqlca (struct tpr01_SQLDesc *SQLDesc)
{
  tpr01_EnvDesc *EnvDesc = SQLDesc->SQL->EnvDesc;
  return EnvDesc->sqlca;
}

static sqlxatype *pr01SQLGetSqlxa (struct tpr01_SQLDesc *SQLDesc)
{
  return SQLDesc->sqlxa;
}

static tsp00_Int4 pr01SQLSetError(tpr01_SQLDesc *SQLDesc, tpr_runtime_errors_Enum ErrorEnum)
{
  tsp00_Int4 ErrorCode;
  tpr01_ErrorText szErrorText;
  pr01ErrorGetErrorText(ErrorEnum, &ErrorCode, szErrorText);
  SQLDesc->ErrorDesc->Error->SetError(SQLDesc->ErrorDesc, ErrorCode, (char*)szErrorText, CPR_NTS);
  return ErrorCode;
}
/*!
  Function:     pr01SQLPutSQL

  see also:

  Description:  ???

  Arguments:
    *SQLDesc      [in] pointer to SQL Descriptor
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
static int pr01SQLPutSQL(tpr01_SQLDesc *SQLDesc, tpr05_String *pSQLStatement, tpr01_SQLStatementOptionEnum fOption)
{
  M90TRACE(M90_TR_ENTRY, "pr01SQLPutSQL", 0);
  {
    int ret = 0;
    if (SQLDesc->StmtNameDesc) {
      tpr01_StmtNameContainer *StmtName = SQLDesc->StmtNameDesc->StmtName;
      ret = StmtName->PutSQL(SQLDesc->StmtNameDesc, pSQLStatement, fOption);
    }
    M90TRACE(M90_TR_EXIT, "pr01SQLPutSQL", 0);
    return ret;
  }
}
/*!
  Function:     pr01SQLGetSQL

  see also:

  Description:  ???

  Arguments:
    *SQLDesc      [in]  pointer to SQL Descriptor
    pSQLStatement [out] tpr05_String which contains the sql-statement.
    fOption       [in]  controls processing of pSQLStatement->rawString
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
static int pr01SQLGetSQL(tpr01_SQLDesc *SQLDesc, tpr05_String **pSQLStatement, tpr01_SQLStatementOptionEnum fOption)
{
  M90TRACE(M90_TR_ENTRY, "pr01SQLGetSQL", 0);
  M90TRACE(M90_TR_HANDLE, "StmtNameDesc", &SQLDesc->StmtNameDesc);
  {
    int ret = 0;
    if (SQLDesc->StmtNameDesc) {
      tpr01_StmtNameContainer *StmtName = SQLDesc->StmtNameDesc->StmtName;
      ret = StmtName->GetSQL(SQLDesc->StmtNameDesc, pSQLStatement, fOption);
    }
    M90TRACE(M90_TR_EXIT, "pr01SQLGetSQL", 0);
    return ret;
  }
}

static boolean pr01SQLExecute(struct tpr01_SQLDesc *SQLDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01SQLExecute", 0);
  {
    sqlcatype *sqlca = pr01SQLGetSqlca(SQLDesc);
    sqlxatype *sqlxa = pr01SQLGetSqlxa(SQLDesc);
    sqlkaentry *OpenKa;
    tsp00_Bool failed = FALSE;
    sqlxa->xaSQLDesc = SQLDesc;
    OpenKa = pr01sIsExecuteAtFetch(sqlxa);
    M90TRACE(M90_TR_PTR, "OpenKa", &OpenKa);
    if (OpenKa != NULL) {
      pr01sExecuteAtFetch(SQLDesc);
      if (sqlca->sqlcode < 0) {
        failed = TRUE;
      }
    }
    while (!failed && sqlxa->xacmdinit == sqlIsTrue) {
      pr01cExecu(SQLDesc);
      pr01cEnd(SQLDesc);
    }
    if (sqlca->sqlcode == 0 || sqlca->sqlcode == 100) {
      tpr01_StmtNameDesc *StmtNameDesc = SQLDesc->StmtNameDesc;
      StmtNameDesc->State |= Executed_bpr01;
      M90TRACE(M90_TR_ENUM, "State", &StmtNameDesc->State);
    }
    M90TRACE(M90_TR_EXIT, "pr01SQLExecute", 0);
    return true;
  }
}

static boolean pr01SQLTryExecute (struct tpr01_SQLDesc *SQLDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01SQLTryExecute", 0);
  PR07_CHECKDESC(SQLDesc, SQLDesc_epr01);
  M90TRACE(M90_TR_HANDLE, "SQLDesc", &SQLDesc);
  M90TRACE(M90_TR_WORD, "kalineno", &SQLDesc->ka->kalineno);
  {
    boolean ret = false;
    sqlcatype *sqlca = pr01SQLGetSqlca(SQLDesc);
    sqlxatype *sqlxa = pr01SQLGetSqlxa(SQLDesc);
    sqlxa->xaSQLDesc = SQLDesc;
    pr01SQLBeforeExecute(SQLDesc);
    pr01cBeg(SQLDesc);
    if (!SQLDesc->StmtNameDesc) {
      sqlxa->xacmdinit = sqlIsTrue;
    }
    if ((sqlca->sqlcode == 0 || sqlca->sqlcode == 100)
	&& sqlxa->xacmdinit != sqlIsTrue) {
      pr01SQLSetState(SQLDesc);
      ret = true;
    }
    M90TRACE(M90_TR_EXIT, "pr01SQLTryExecute", 0);
    M90TRACE(M90_TR_UCHAR, "ret", &ret);
    return ret;
  }
}

static void pr01SQLDescribe (struct tpr01_SQLDesc *SQLDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01SQLDescribe", 0);
  {
    sqlcatype *sqlca = pr01SQLGetSqlca(SQLDesc);
    sqlxatype *sqlxa = pr01SQLGetSqlxa(SQLDesc);
    sqlxa->xaSQLDesc = SQLDesc;
    pr01cDescribe(SQLDesc);
    M90TRACE(M90_TR_EXIT, "pr01SQLDescribe", 0);
  }
}

static void pr01SQLRelease (struct tpr01_SQLDesc *SQLDesc)
{
  M90TRACE(M90_TR_ENTRY, "pr01SQLRelease", 0);
  {
    sqlcatype *sqlca = pr01SQLGetSqlca(SQLDesc);
    tpr01_SQLContainer *SQL = SQLDesc->SQL;
    tpr01_ConContainer *Connection = SQLDesc->ConDesc->Connection;
    Connection->Release(SQLDesc->ConDesc);
    p03EnvClear( sqlca );
    M90TRACE(M90_TR_EXIT, "pr01SQLRelease", 0);
  }
}

static void pr01SQLInitDesc(struct tpr01_SQLDesc *SQLDesc, sqlxatype *sqlxa)
{
  M90TRACE(M90_TR_ENTRY, "pr01SQLInitDesc", 0);
  PR07_CHECKDESC(SQLDesc, SQLDesc_epr01);
  {
    SQLDesc->sqlxa = sqlxa;
    SQLDesc->ka = NULL;
    SQLDesc->ore = NULL;
    SQLDesc->pr = NULL;
    SQLDesc->cu = NULL;
    SQLDesc->StmtNameDesc = NULL;
    SQLDesc->CursorDesc = NULL;
    SQLDesc->ConDesc = NULL;
    SQLDesc->ComType = cpr_com_empty;
    SQLDesc->MessType = sp1m_nil;
    SQLDesc->Attribute.massopen = FALSE;
    SQLDesc->Attribute.parspre  = FALSE;
    SQLDesc->PacketEncoding  = sp77encodingAscii;
  }
  M90TRACE(M90_TR_EXIT, "pr01SQLInitDesc", 0);
}

static void pr01SQLBeforeExecute(struct tpr01_SQLDesc *SQLDesc)
{
  sqlcatype *sqlca = SQLDesc->ConDesc->sqlca;
  sqlkaentry *ka   = SQLDesc->ka;

  if(! pr06ParseIdCheckKnlSessionID(&SQLDesc->ConDesc->ga->gaKnlSessionID, &ka->kaParseInfo.ParseId)) {
    pr06ParseIdCopy(&ka->kaParseInfo.ParseId, (tpr00_ParseId*) CPR_PARSIDNULL);
    ka->kastate = cpr_state_parse;
  }
  if (sqlca->sqlrap->rasqlldp) {
    if ((sqlca->sqlrap->rasqlldp->ldsparr_mdesc.descNelem > 0) ||
        (sqlca->sqlrap->rasqlldp->ldmaxo > 0)) {
      if ((SQLDesc->ComType != cpr_com_sql_putval) &&
          (SQLDesc->ComType != cpr_com_sql_getval)) {
        /*no Getval and no Putval then close long columns*/
        pr04LongCloseDescriptors(SQLDesc->ConDesc);
      }
    }
  }
}

static void pr01SQLSetState(struct tpr01_SQLDesc *SQLDesc)
{
  tpr01_StmtNameDesc *StmtNameDesc = SQLDesc->StmtNameDesc;
  sqlkaentry *ka = SQLDesc->ka;
  M90TRACE(M90_TR_WORD,"sp1rFunctionCode", &ka->kaParseInfo.sp1rFunctionCode);
  switch(ka->kaParseInfo.sp1rFunctionCode) {
  case (csp1_insert_fc) : {}
  case (csp1_minsert_fc) : {}
  case (csp1_select_fc) : {}
  case (csp1_mselect_fc) : {
    StmtNameDesc->State |= Opened_bpr01;
    break;
  }
  default:{
    StmtNameDesc->State |= Executed_bpr01;
  }
  }
  M90TRACE(M90_TR_ENUM, "State", &StmtNameDesc->State);
}

/*!
  Function:     pr01SQLIsSelectForUpdate

  see also:

  Description:  Checks if the correspondending cursor is a SELECT FOR UPDATE.

  Arguments:
    SQLDesc [in] a pointer to a SQL descriptor

  return value: true if the cursor was generated by an SELECT FOR UPDATE
                false if not
 */
static boolean pr01SQLIsSelectForUpdate(tpr01_SQLDesc *SQLDesc)
{  
  if (SQLDesc && SQLDesc->CursorDesc && SQLDesc->CursorDesc->OpenKa) {
    sqlkaentry *OpenKa   = SQLDesc->CursorDesc->OpenKa;
    if (OpenKa->kaParseInfo.ParseId[10] ==  csp1_p_select_for_update_found
        || OpenKa->kaParseInfo.ParseId[10] ==  csp1_p_reuse_update_sel_found
        || OpenKa->kaParseInfo.ParseId[10] ==  csp1_p_for_upd_mselect_found
        || OpenKa->kaParseInfo.ParseId[10] ==  csp1_p_reuse_upd_mselect_found)
      return true;
  }
  return false;
}


/*!
  Function:     pr01SQLgetAllAttributes

  see also:

  Description:  Sets SQLDesc->Attributes depending on analyze of
                sql satement

  Arguments:
    SQLDesc [in] a pointer to a SQL descriptor

  return value: void
 */
void pr01SQLgetAllAttributes(struct tpr01_SQLDesc *SQLDesc)
{
  if (SQLDesc->StmtNameDesc)
    {

      switch (SQLDesc->StmtNameDesc->StmtTypeDesc.StmtType)
        {
        case (KeywordINSERT_epr01) :
        case (KeywordUPDATE_epr01) :
        case (KeywordDELETE_epr01)  :
          {
            SQLDesc->Attribute.massopen = TRUE;
            SQLDesc->Attribute.parspre  = TRUE;
            break;
          }
        case (KeywordDECLARE_epr01):
        case (KeywordDESCRIBE_epr01):
          {
            SQLDesc->Attribute.massopen = FALSE;
            SQLDesc->Attribute.parspre  = FALSE;
            break;
          }
        case (KeywordFETCH_epr01)  :
          {
            if (pr01SQLIsSelectForUpdate(SQLDesc))
              SQLDesc->Attribute.massopen = FALSE;              
            else
              SQLDesc->Attribute.massopen = TRUE;
            SQLDesc->Attribute.parspre  = TRUE;
            if (SQLDesc->StmtNameDesc->StmtTypeDesc.UsingPos > 0)
              {
                /* fetch using descriptor nicht prepare parsen */
                SQLDesc->Attribute.parspre  = FALSE;

              }
            break;
          }
        case (KeywordSHOW_epr01):
          {
            SQLDesc->Attribute.massopen = FALSE;
            SQLDesc->Attribute.parspre  = FALSE;
            break;
          }
        case (KeywordSELECT_epr01):
          {
            sqlcatype *sqlca = pr01SQLGetSqlca(SQLDesc);
            SQLDesc->Attribute.massopen = TRUE;
            SQLDesc->Attribute.parspre  = TRUE;
            if ( SQLDesc->StmtNameDesc->StmtTypeDesc.StmtOption != KeywordNOT_FOUND_epr01
                || (   SQLDesc->StmtNameDesc->StmtTypeDesc.UsingPos > 0         /*SELECT INTO*/
                    && sp1sm_oracle > pr03cMapSqlMode(sqlca->sqlrap->rakamode, sqlca->sqlrap->rasqlansi)
                    )
               )
            /*a SELECT ... INTO can handled by database kernel in oracle mode only*/
              {
                SQLDesc->Attribute.massopen = FALSE;
                SQLDesc->Attribute.parspre  = TRUE;

              }
            break;
          }
        case (KeywordCREATE_epr01):
        case (KeywordDROP_epr01)  :
        case (KeywordDDL_epr01)  :
        case (KeywordSETISOLEVEL_epr01)  :
          {
            /* create , drop, alter, grant, revoke, exist */
            /* kein prepare parsen da ddl-kommandos       */
            SQLDesc->Attribute.massopen = FALSE;
            SQLDesc->Attribute.parspre  = FALSE;
            break;
          }
        default :
          {
            SQLDesc->Attribute.massopen = FALSE;
            SQLDesc->Attribute.parspre  = TRUE;
          }
        } /*end switch*/
    }
}
