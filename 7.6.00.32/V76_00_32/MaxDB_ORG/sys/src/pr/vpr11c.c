/*!**************************************************************************

  module      : vpr11c.h

  -------------------------------------------------------------------------

  responsible : BurkhardD

  special area:
  description : Contains functions for execute SQL comming from ODBC.
                replaces most of PASCAL coded in vpr11.

  last changed: 
  see also    :

  change history : 
        
        D025086 08-11-2000 Created for UNICODE Support. New parser etc.

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

#include "vpr11c.h"
#include "vpr05c.h"
#include "vpr08c.h"
#include "vpr01Con.h"
#include "vpr03Part.h"
#include "vpr05AnalyzeSQL.h"
#include "vpr01SQL.h"
#include "hsp78_0.h"

static void pr11cComtypetoXakano(sqlcatype *sqlca, tpr05_StmtAnalysisDesc *AnalyzeDesc, int *intopos)
{
  sqlcxatype *sqlcxap = sqlca->sqlcxap;
  sqlratype *sqlra = sqlca->sqlrap;
  if ((int ) sqlcxap->xakano != cpr_com_command) {
    *intopos = 0;
    sqlcxap->xakano = cpr_com_sql;

    switch (AnalyzeDesc->StmtType) {
    case (KeywordCOMMIT_epr01) : {
      if (AnalyzeDesc->StmtOption == KeywordRELEASE_epr01)
        sqlcxap->xakano = cpr_com_commit_release;
      else
        sqlcxap->xakano = cpr_com_commit;
      break;
    }
    case (KeywordROLLBACK_epr01) : {
      if (AnalyzeDesc->StmtOption == KeywordRELEASE_epr01)
        sqlcxap->xakano = cpr_com_rollback_release;
      else
        sqlcxap->xakano = cpr_com_rollback;
      break;
    }
    case (KeywordDESCRIBE_epr01) : {
      sqlcxap->xakano = cpr_com_describe;
      *intopos = AnalyzeDesc->UsingPos;
      break;
    }
    case (KeywordDECLARE_epr01) : {
      sqlca->sqlrap->rakamode = cpr_kind_internal;
      break;
    }
    case (KeywordFETCH_epr01) : {}
    case (KeywordMFETCH_epr01) : {}
    case (KeywordFFETCH_epr01) : {
      if (AnalyzeDesc->StmtType == KeywordMFETCH_epr01 )
        sqlcxap->xakano = cpr_com_mfetch;
      else if (AnalyzeDesc->StmtType == KeywordFFETCH_epr01)
        sqlcxap->xakano = cpr_com_mfetch;

      if (sqlcxap->xakano == cpr_com_mfetch)
        pr03SegmentSetAttribute(sqlra->rasegptr, Mass_Cmd_epr03, TRUE);

      if (AnalyzeDesc->UsingPos > 0) {
        if (sqlcxap->xakano == cpr_com_mfetch)
          sqlcxap->xakano = cpr_com_mfetch_describe;
        else
          sqlcxap->xakano = cpr_com_fetch_describe;
      }
      *intopos = AnalyzeDesc->UsingPos;
      sqlcxap->xacfetspec = pr08MapFetchSpec(AnalyzeDesc);
      break;
    }
    default : {
      break;
    }
    }
  }
}

static tpr_runtime_errors_Enum 
pr11cPutSQLStatement(tpr05_String *SQLStatement,
                     tpr05_StmtAnalysisDesc *AnalyzeDesc,
                     tsp1_part *partPtr, 
                     tpr05_StringEncoding encodingPartType)
{
  tpr_runtime_errors_Enum Err = cpr_p_ok;
  if (partPtr) {
    void *rawPtr = pr03PartGetRawPtr(partPtr);
    tsp00_Uint4 cbPartLen = (tsp00_Uint4)pr03PartGetFreePartSpace(partPtr);
    tsp00_Int4 Offset = 0;
    tsp00_Int4 char_size;

    if (((SQLStatement->encodingType == sp77encodingUCS2) ||
	 (SQLStatement->encodingType == sp77encodingUCS2Swapped)) &&
	encodingPartType == sp77encodingAscii)
      char_size = sizeof (tsp81_UCS2Char);
    else
      char_size = 1;

    if ((SQLStatement->cbLen / char_size) > cbPartLen) {
      Err = cpr_cmd_too_long;
    }
    else {
      Offset = 0; /* Startposition in Part */
      switch (AnalyzeDesc->StmtType) {
      case (KeywordFFETCH_epr01) :{}
      case (KeywordMFETCH_epr01) :{
        /* find out position of FFETCH or MFETCH Keyword 
           and ignore the first letter */
        tpr05_KeywordIterator KIter;
        tsp00_Int4 nFetchPos;
        tpr01_KeywordTypeEnum StmtType;
        pr05AnalyseSQLKeywordInitIterator(SQLStatement, &KIter);
        /* gets the first keyword */
        pr05AnalyseSQLKeywordEnum(&KIter);
        /* on SymbolPos starts the 'F'FETCH Command */
        nFetchPos = KIter.SymbolPos;
        if (SQLStatement->encodingType == sp77encodingUCS2 ||
            SQLStatement->encodingType == sp77encodingUCS2Swapped)
          nFetchPos *= sizeof (tsp81_UCS2Char);
        Offset = 0;
        Err = pr03PartConverttoPart(partPtr, &Offset, (tsp00_Int4*)&cbPartLen, encodingPartType, &SQLStatement->rawString[nFetchPos], SQLStatement->cbLen-nFetchPos, SQLStatement->encodingType);
        break;
      }      
      default: {
        Err = pr03PartConverttoPart(partPtr, &Offset, (tsp00_Int4*)&cbPartLen, encodingPartType, SQLStatement->rawString, SQLStatement->cbLen, SQLStatement->encodingType);
        break;
      }
      }
    }
  }
  else {
    Err = cpr_valbuf_too_short;
  }
  return Err;
}

void pr11cGetsqcstm(sqlcatype *sqlca, 
                   struct tpr01_ConDesc *ConDesc, 
                   tpr05_StringEncoding PacketEncoding,
                   tsp1_cmd_mess_type cmdtyp,
                   tsp00_Int4 cbBufLen,
                   tsp00_MoveObj sqcstmbuf,
                   tpr05_StringEncoding encodingType,
                   int *intopos)
{
  tpr03_SegmDesc *SegmDesc = ConDesc->SegmDesc;
  sqlcxatype *sqlcxap = sqlca->sqlcxap;
  sqlratype *sqlrap = sqlca->sqlrap;
  sqltatype *sqltap = sqlrap->rasqltap;
  tpr_runtime_errors_Enum Err = cpr_p_ok;
  sqlrap->ralang = CPR_LA_CALL;
  if (sqltap->tatrout != cpr_trace_off || cmdtyp != sp1m_execute) {
    if (sqlca->sqlcode == cpr_p_ok) {
      tpr03_SegmDesc *SegmDesc;
      tsp00_Int4 Offset, cbPartLen;
      tpr05_String SQLStatement;
      tpr05_StmtAnalysisDesc AnalyzeDesc;
      SQLStatement.allocationType = constant_epr05;
      pr05IfCom_String_InitString(&SQLStatement, sqcstmbuf, cbBufLen, encodingType, constant_epr05);		        
      pr05cAnalyseSQLStatement(&SQLStatement, &AnalyzeDesc);
      p03csqlcaareainit (sqlca);
      ConDesc->Connection->InitPacket (ConDesc, PacketEncoding, cmdtyp);
      SegmDesc = ConDesc->SegmDesc;
      if (SegmDesc) {
        tsp1_part *partPtr = pr03SegmentAddPart(SegmDesc, sp1pk_command);
        Err = pr11cPutSQLStatement(&SQLStatement, &AnalyzeDesc, partPtr, PacketEncoding);
        if (Err == cpr_p_ok) {
          pr03SegmentFinishPart(SegmDesc);
          pr11cComtypetoXakano(sqlca, &AnalyzeDesc, intopos);
        }
        else 
          p08runtimeerror(sqlca, sqlcxap, Err);         
      }
    }
  }
}
  
void pr11cDescribe  (sqlcatype *sqlca,
                     struct tpr01_ConDesc *ConDesc,
                     tpr05_StringEncoding PacketEncoding,
                     sqldatype *sqlda,
                     tpr00_ParseId *sqcparsid,
                     boolean again)
{ 
  sqlcxatype *sqlcxap = sqlca->sqlcxap;
  if (sqlcxap->xakano == cpr_com_fetch_describe
    || sqlcxap->xakano == cpr_com_mfetch_describe) {
    sqlgaentry *gae = ConDesc->ga; 
    tpr03_SegmDesc *SegmDesc = ConDesc->SegmDesc;
    p03csqlcaareainit (sqlca);
    ConDesc->Connection->InitPacket(ConDesc, PacketEncoding, sp1m_dbs);
    SegmDesc = ConDesc->SegmDesc;
    if (SegmDesc) {
      tsp1_part *partPtr = pr03SegmentAddPart(SegmDesc, sp1pk_command);
      tpr_runtime_errors_Enum Err;
      if (partPtr) {
        static char szDescribeCMD[] = "DESCRIBE ";
        tsp00_Int4 cbPartLen = pr03PartGetFreePartSpace(partPtr);
        tsp00_Int4 Offset = 0;
        Err = pr03PartConverttoPart(partPtr, &Offset, &cbPartLen, PacketEncoding, szDescribeCMD, sizeof(szDescribeCMD)-1, sp77encodingAscii);
        pr03SegmentFinishPart(SegmDesc);
        if (!again)
          p03cmdtrace(sqlca->sqlrap, gae, 1, cpr_com_empty, NULL);
        p11againtrace(sqlca,  3, again);
        pr03PacketReqRec(ConDesc, sqlca->sqlemp);
        p03gparsid(sqlca->sqlrap, sqlca->sqlemp, sqcparsid);
        p03returncodeget(sqlca, sqlcxap);
        p11shortfieldparameterput (sqlca, gae, sqlda, again);
      }
    }
  }
} 

void pr11cSyntax  (sqlcatype *sqlca,
                   struct tpr01_ConDesc *ConDesc,
                   /*                   sqldatype *sqlda,*/
                   tpr05_String *sqlCmd)
{ 
  sqlcxatype *sqlcxap = sqlca->sqlcxap;
  sqlgaentry *gae = ConDesc->ga;
  tpr03_SegmDesc *SegmDesc = ConDesc->SegmDesc;
  tpr05_StringEncoding PacketEncoding = sqlca->sqlcxap->xaSQLDesc->PacketEncoding;
  p03csqlcaareainit (sqlca);
  ConDesc->Connection->InitPacket(ConDesc, PacketEncoding, sp1m_syntax);
    SegmDesc = ConDesc->SegmDesc;
    if (SegmDesc) {
      tsp1_part *partPtr = pr03SegmentAddPart(SegmDesc, sp1pk_command);
      tpr_runtime_errors_Enum Err;
      if (partPtr) {
        tsp00_Int4 cbPartLen = pr03PartGetFreePartSpace(partPtr);
        tsp00_Int4 Offset = 0;

        Err = pr03PartConverttoPart(partPtr, &Offset, &cbPartLen, PacketEncoding, sqlCmd->rawString, sqlCmd->cbLen, sqlCmd->encodingType);
        
        pr03SegmentFinishPart(SegmDesc);
        p03cmdtrace(sqlca->sqlrap, gae, 1, cpr_com_empty, NULL);
        pr03PacketReqRec(ConDesc, sqlca->sqlemp);
        p03returncodeget(sqlca, sqlcxap);
        /*        p11shortfieldparameterput (sqlca, gae, sqlda, again); */
      }
    }
} 

void pr11cFetchDescribe(sqlcatype *sqlca,
                        struct tpr01_ConDesc *ConDesc, 
                        tpr05_StringEncoding PacketEncoding,
                        sqldatype *sqlda,
                        tpr05_StmtAnalysisDesc *AnalyzeDesc)
{
  int i;
  tpr_runtime_errors_Enum Err = cpr_p_ok;
  if (sqlca->sqlcode == 0) {
    static char szIntoCMD[] = "INTO ";
    sqlcxatype *sqlcxap = sqlca->sqlcxap;
    sqlratype *sqlrap = sqlca->sqlrap;
    tpr03_SegmDesc *SegmDesc = ConDesc->SegmDesc;
    tsp1_part *partPtr = pr03PartFind(sqlrap, sp1pk_command);
    void *rawPtr = pr03PartGetRawPtr(partPtr);
    tsp00_Int4 cbPartLen = pr03PartGetPartLength(partPtr);
    tsp00_Int4 cbOffset, nParam;
    tpr05_String SQLStatement;
    SQLStatement.allocationType = constant_epr05;
    if (sqlcxap->xakano == cpr_com_mfetch_describe) {
      pr03SegmentSetAttribute(sqlrap->rasegptr, Mass_Cmd_epr03, TRUE);
    }
    
    pr05IfCom_String_InitString(&SQLStatement, rawPtr, cbPartLen, PacketEncoding, constant_epr05);		        
    pr05cAnalyseSQLStatement(&SQLStatement, AnalyzeDesc);
    
    cbPartLen = cbOffset = AnalyzeDesc->UsingPos;
    pr03PartSetPartLength(partPtr, cbPartLen);
    Err = pr03PartConverttoPart(partPtr, &cbOffset, &cbPartLen, PacketEncoding, szIntoCMD, sizeof(szIntoCMD)-1, sp77encodingAscii);
    
    switch(sqlrap->radbmode) {
    case CPR_KIND_ORACLE: {}
    case CPR_KIND_SAPR3: {
      SQLDA *da=sqlca->sqlcxap->xasqldap;
      nParam = da->F;
      break;
    }
    default: {
      sqldatype *da = sqlca->sqlcxap->xasqldap;
      nParam = da->sqln;
      break;
    }
    }
    for (i=0; i < nParam; i++) {
      static char szParamName[] = "A:";
      static char szParamName1[] = ", A:";
      if (i == 1)
        Err = pr03PartConverttoPart(partPtr, &cbOffset, &cbPartLen, PacketEncoding, szParamName, sizeof(szParamName)-1, sp77encodingAscii);
      else
        Err = pr03PartConverttoPart(partPtr, &cbOffset, &cbPartLen, PacketEncoding, szParamName1, sizeof(szParamName1)-1, sp77encodingAscii);
    }
    pr03SegmentFinishPart(SegmDesc);
  } 
}
