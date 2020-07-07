/*!================================================================
 module:    gpr01.h

 responsible: BurkhardD

 special area:   CPCDrv | Client | Prototypes fuer vpr01

 description:

 see:




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

#ifndef __GPR01_H__
#define __GPR01_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"
#include "gpr01b.h"
#include "gpr01e.h"
#include "gpr01s.h"
#include "gpr01s.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/* This Enumeration describe all Dynamicdescriptors used by the
   Interfaceruntime. It can be use to evaluate the correctness of a Descriptor
   passing to a function.
*/
typedef enum tpr01_DescTypeEnum {
  UnknownDesc_epr01,
  EnvDesc__epr01,
  ModuleDesc_epr01,
  ConDesc_epr01,
  PrecomDesc_epr01,
  StmtNameDesc_epr01,
  SQLDesc_epr01,
  CursorDesc_epr01,
  CommandDesc_epr01,
  ErrorDesc_epr01,
  SegDesc_epr01,
  ParseInfoDesc_epr01
} tpr01_DescTypeEnum;

/* This enumeration describes the state of a SQL-Command.
   The state is bitwise coded so combinations of the states are possible.
   To test if a state is set use the & operator or the |= operator to set
   one of the state.
*/

typedef enum tpr01_SQLStateBinary {
  Empty_bpr01     = 0x00,
  Allocated_bpr01 = 0x01,          /* SQL-Command is allocated */
  Prepared_bpr01  = 0x02,          /* SQL-Command is parsed */
  Executed_bpr01  = 0x04,          /* SQL-Command is executed */
  Opened_bpr01    = 0x08,          /* A cursor is open and fetches are possible */
  Closed_bpr01    = 0x10,          /* The cursor has been closed */
  Analysed_bpr01  = 0x20,          /* The analyse function has been worked and
				      the StmtTypeDesc is correctly set */
  StatusAll_bpr01 = (Empty_bpr01-1) /* This member is the bitwise empty mask of the state. You can use this as a negation operator */
} tpr01_SQLStateBinary;

typedef enum tpr01_ConnectOptionBinary {
  NoSelectDirectFast_bpr01 = 0x01
} tpr01_ConnectOptionBinary;

typedef enum tpr01_SQLStatementOptionEnum {
  First_epr01,
  Append_epr01,
  Reference_epr01
} tpr01_SQLStatementOptionEnum;

typedef enum tpr01_KeywordTypeEnum {
  KeywordABSOLUTE_epr01,
  KeywordADABAS_epr01,
  KeywordALTER_epr01,
  KeywordANSI_epr01,
  KeywordANSIORA_epr01,
  KeywordAPPEND_epr01,
  KeywordAT_epr01,
  KeywordCACHELIMIT_epr01,
  KeywordCHARACTER_epr01,
  KeywordCLOSE_epr01,
  KeywordCOMMIT_epr01,
  KeywordCONNECT_epr01,
  KeywordCONNECTC_epr01,
  KeywordCREATE_epr01,
  KeywordDB2_epr01,
  KeywordDDL_epr01,
  KeywordDECLARE_epr01,
  KeywordDEFAULT_epr01,
  KeywordDELETE_epr01,
  KeywordDESCRIBE_epr01,
  KeywordDESCRIPTOR_epr01,
  KeywordDIRECT_epr01,
  KeywordDISTINCT_epr01,
  KeywordDROP_epr01,
  KeywordEXIST_epr01,
  KeywordEXPLICIT_epr01,
  KeywordFETCH_epr01,
  KeywordFFETCH_epr01,
  KeywordFIRST_epr01,
  KeywordFOR_epr01,
  KeywordFROM_epr01,
  KeywordGETVAL_epr01,
  KeywordGRANT_epr01,
  KeywordIDENTIFIED_epr01,
  KeywordIMPLICIT_epr01,
  KeywordINDEX_epr01,
  KeywordINSERT_epr01,
  KeywordINTERNAL_epr01,
  KeywordINTO_epr01,
  KeywordISOLATION_epr01,
  KeywordKEY_epr01,
  KeywordLAST_epr01,
  KeywordLEVEL_epr01,
  KeywordLOCK_epr01,
  KeywordMFETCH_epr01,
  KeywordNEXT_epr01,
  KeywordNOLOG_epr01,
  KeywordNORMAL_epr01,
  KeywordOPEN_epr01,
  KeywordORACLE_epr01,
  KeywordPOS_epr01,
  KeywordPREPARE_epr01,
  KeywordPREV_epr01,
  KeywordPROC_epr01,
  KeywordPUTVAL_epr01,
  KeywordRELATIVE_epr01,
  KeywordRELEASE_epr01,
  KeywordREVOKE_epr01,
  KeywordROLLBACK_epr01,
  KeywordSAME_epr01,
  KeywordSAPR3_epr01,
  KeywordSELECT_epr01,
  KeywordSET_epr01,
  KeywordSETISOLEVEL_epr01,
  KeywordSHOW_epr01,
  KeywordSQLDB_epr01,
  KeywordSQLMODE_epr01,
  KeywordTIMEOUT_epr01,
  KeywordUNIQUE_epr01,
  KeywordUPDATE_epr01,
  KeywordUSING_epr01,
  KeywordWORK_epr01,
  KeywordNOT_FOUND_epr01 = -1,
  KeywordNOT_ANALYZED_epr01 = -2
} tpr01_KeywordTypeEnum;

/*flag that marks if parse infos of the sql statement are cacheable or not*/
typedef enum tpr01_Cacheable {
  notcacheable_epr01   = 0,
  cacheable_epr01      = 1
} tpr01_Cacheable;

typedef enum tpr01_Precompiledfor {
  PrecompiledForAscii_epr01   = 0,
  PrecompiledForUnicode_epr01 = 1
} tpr01_Precompiledfor;

/* to register an internal use StmtName use this list to create an ID that is
   only used once */
enum UniqueID_epr01 {
  EmptyUniqueID_cpr01 = (0x00000000),
  MinUniqueID_cpr01 = (0x00010000), /* Each Module has an Unique Offset,
                                       but this limits the number of stmtname
                                       per module */
  ReservedUniqueIDStart_cpr01 = (0x7FFF0000),
  ODBCConStmtID_cpr01=(ReservedUniqueIDStart_cpr01+1),
  ODBCDescribeStmtID_cpr01
} UniqueID_epr01;

typedef tsp00_Int4 tpr01_UniqueID;
typedef char tpr01_UniqueName[9];
/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

void p01varpart_get(sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, sqlkaentry *ka);
void p01pparsidtrace (sqlcatype *sqlca, sqlxatype *sqlxa, sqlkaentry *ka, tpr00_ParseId VAR_ARRAY_REF parsid, tpr00_TraceParseID_Enum inout);
void p01cmd_trace (sqlcatype *sqlca, sqlxatype *sqlxa, sqlkaentry *ka, sqlgaentry *ga);
void p01prepstmtname(sqlcatype *sqlca, sqlxatype *sqlxa);
void p01begi(sqlcatype *sqlca, sqlxatype *sqlxa, tsp00_Int2 sess);
void p01execute(sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, sqlkaentry *ka);
void p01searchgroupfunc (sqlcatype *sqlca, sqlxatype *sqlxa, BOOLEAN *massopen, BOOLEAN *parspre);
void p01resallocinfo(sqlcatype *sqlca, sqlxatype *sqlxa, sqlkaentry *ka, sqlorentry *ore);
boolean pr01cParsePrepare(struct tpr01_SQLDesc *SQLDesc);
tsp00_Int4 pr01cParse(struct tpr01_SQLDesc *SQLDesc, tsp00_Int2 *cmdfetch, tsp00_Int4 *datafound);
void p01cCmdParseExecute(sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *gae, sqlkaentry * kae, sqlprentry *pre, sqlorentry *ore, sqlcuentry * cue);
void pr01cOpen(struct tpr01_SQLDesc *SQLDesc);
void pr01cBeg(struct tpr01_SQLDesc *SQLDesc);
void pr01cEnd(struct tpr01_SQLDesc *SQLDesc);
void pr01cExecu(struct tpr01_SQLDesc *SQLDesc);
void pr01cOpenCursor(struct tpr01_SQLDesc *SQLDesc);
void pr01cDynmfetch (struct tpr01_SQLDesc *SQLDesc);
void pr01cDescribe (struct tpr01_SQLDesc *SQLDesc);
boolean pr01cIsComType(struct tpr01_SQLDesc *SQLDesc, tpr00_ComTypeEnum ComType);
void p01colinitsfinfo(sqlcatype *sqlca, sqlxatype *sqlxa, sqlkaentry *ka, sqlorentry *ore);
void p01colmovesfinfo (sqlcatype *sqlca, sqlxatype *sqlxa, sqlkaentry *ka, sqlorentry *ore);
void p01nammovesfinfo(sqlcatype *sqlca, sqlxatype *sqlxa, sqlkaentry *ka, sqlorentry *ore, tpr00_ComTypeEnum getkatyp);
void p01resinitsfinfo (sqlcatype *sqlca, sqlxatype *sqlxa, sqlkaentry *ka, sqlorentry *ore, BOOLEAN *prepars);
void p01resmovesfinfo (sqlcatype *sqlca, sqlxatype *sqlxa, sqlkaentry *ka, sqlorentry *ore, BOOLEAN *prepars);
void p01adbsexecute (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, sqlkaentry *ka, sqlorentry *ore, sqlcuentry *cu, tsp00_Int2 *cmdfetch, tsp1_cmd_mess_type *cmdtyp, BOOLEAN proc_begin);
void p01oradescribe  (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga);
void p01makefetchinto (sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, int intopos);
void p01dynmfetch (sqlcatype *sqlca, sqlxatype *sqlxa, sqlkaentry *ka);
void p01cmd_execute_immediate(sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, sqlkaentry *ka);
void p01execu(sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, sqlkaentry *ka);
void p01check(sqlcatype *sqlca, sqlxatype *sqlxa, tsp00_Int4 connplen);
void p01ma2bi(sqlcatype *sqlca, sqlxatype *sqlxa, tsp00_Int2 ma_no, tsp00_Int2 ma_len, tpr_macroline VAR_ARRAY_REF val);
struct tpr01_PrecomDesc *pr01cGetPrecomDesc(sqlxatype *sqlxa, tsp00_Int4 kano);
void p01beg(sqlcatype *sqlca, sqlxatype *sqlxa);
void p01end(sqlcatype *sqlca, sqlxatype *sqlxa);
void p01descr(sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, sqlkaentry *ka);
void p01command(sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, sqlkaentry *ka, tsp00_Int2 kano);
void p01stop(sqlcatype *sqlca, sqlxatype *sqlxa);
void p01exit(sqlcatype *sqlca, sqlxatype *sqlxa);
void p01cancel(sqlcatype *sqlca, sqlxatype *sqlxa, sqlgaentry *ga, tsp00_Int2 kano);
void p01version(sqlcatype *sqlca, sqlxatype *sqlxa, tsp00_Int2 kano);
void p01_katyp_get(sqlcatype *sqlca, sqlxatype *sqlxa);
void p01usenewparsinfo (sqlcatype *sqlca, sqlxatype *sqlxa, sqlkaentry *ka, sqlorentry *ore, sqlcuentry *cu);
sqlkaentry *pr01cDupKa(sqlkaentry *kae);
void pr01cFreeKa(sqlkaentry *kae);
void p01verstrace (sqlcatype *sqlca, sqlxatype *sqlxa, tsp00_Int2 art, tsp00_C40 VAR_ARRAY_REF vers);
void p01putversion (sqlcatype *sqlca, sqlxatype *sqlxa, int vaind, tsp00_Int4 len, tsp00_C40 VAR_ARRAY_REF vers,  tpr_runtime_errors_Enum *error, void *ptr);
tsp00_Int4 pr01cResultNameGet(sqlratype *sqlrap, sqlgaentry *gae, tsp00_Byte *szResultName, int cbLen);
void pr01cSetLastDataReceived (struct tpr01_SQLDesc *SQLDesc, tsp00_Int4 aValue);
tsp00_Int4 pr01cGetVaIndex(sqlxatype *sqlxa, tsp00_Int4 index, tsp00_Uint1** pAddr);
void p01canctrace(sqlcatype *sqlca, sqlxatype *sqlxa, tsp00_Int2 Session);
void pr01AbortSession (struct tpr01_SQLDesc *SQLDesc);
#endif
