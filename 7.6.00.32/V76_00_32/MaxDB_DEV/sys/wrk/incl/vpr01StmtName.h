/*!**************************************************************************

  module:    vpr01StmtName.h

  -------------------------------------------------------------------------

  responsible: BurkhardD, MarcoP

  special area:   CPCDrv | Client | Statementnamen
  description : Verwaltung von Statementnamen

  last changed: 2000-08-07  17:35 Marco Paskamp     Unicode-Erweiterungen
  see also    :

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


#ifndef __VPR01_STMTNAME_H__
#define __VPR01_STMTNAME_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"
#include "gpr01.h"
#include "vpr09DynaHash.h"
#include "vpr09DynaDesc.h"
#include "vpr05AnalyzeSQL.h"
#include "vpr01ParseInfo.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/
typedef struct tpr01_StmtNameDesc_StmtID
{
  tpr05_String * stmtName;
  tpr01_UniqueID UniqueID;
}tpr01_StmtNameDesc_StmtID;

/*!
  Declaration: tpr01_StmtNameDesc
  Description: structure for managing SQL-Statements
 */
typedef struct tpr01_StmtNameDesc {
  tpr01_DescTypeEnum      DescType;
  struct tpr01_StmtNameContainer *StmtName;	/* pointer to his container*/
  tpr01_StmtNameDesc_StmtID StmtID;         /* statement identifier*/
  sqlsnentry  *sn;                          /* Name of SQL-Statement */
  tpr00_StmtNameStruct StmtNameStruct;      /* Name of the SQL-Statement */
  tpr05_String *SQLStatement;               /* contains the SQL-Statement */
  tpr01_SQLStatementOptionEnum fOption;     /* state of SQLStatement (copy or reference)*/
  tpr01_SQLStateBinary State;
  tpr01_ParseInfoDesc  *ParseInfoDesc;      /* Descriptor containing parseinfos of SQLStatement */
  tpr00_SessionID   KnlSessionID;         /* SessionID (first 4 Bytes of Parseid)*/
  sqlkaentry *ka;
  sqlorentry *ore;
  sqlcuentry *cue;
  tpr05_StmtAnalysisDesc StmtTypeDesc;	    /*describes SQL-statement (SELECT, UPDATE ...) of SQLStatement*/
} tpr01_StmtNameDesc;

/*!
  Declaration: tpr01_StmtNameContainer
  Description: container for tpr01_StmtNameDesc`s
 */
typedef struct tpr01_StmtNameContainer {
#ifdef DYNAHASH
  tpr09DynamicHashDescriptor *Desc;
#else
  tpr09DynamicDescriptor *Desc;
#endif

  tpr01_StmtNameDesc_StmtID cachedStmtID; /*use for temporary stmt ID'S to avaid malloc/frees*/
  void                (*InitCont)   (struct tpr01_StmtNameContainer *Cont);
  tsp00_Int4          (*OpenCont)   (struct tpr01_StmtNameContainer *Cont);
  void                (*CloseCont)  (struct tpr01_StmtNameContainer *Cont);
  tpr01_StmtNameDesc *(*AddDesc)    (struct tpr01_StmtNameContainer *StmtName, tpr05_String *stStmtName, tpr01_UniqueID UniqueID, sqlkaentry *ka);
  void                (*DeleteDesc) (struct tpr01_StmtNameDesc *Desc);
/*  tsp00_Int4          (*GetCount)   (struct tpr01_StmtNameContainer *Cont);*/
  tpr01_StmtNameDesc *(*EnumDesc)   (struct tpr01_StmtNameContainer *Cont);
  void           (*ResetIterator)   (struct tpr01_StmtNameContainer *Cont);
  tsp00_Int4          (*SetIndex)   (struct tpr01_StmtNameContainer *Cont, tsp00_Int4);
  tpr01_StmtNameDesc *(*FindDesc)   (struct tpr01_StmtNameContainer *StmtName, tpr05_String *stStmtName, tpr01_UniqueID UniqueID);
  int        (*PutSQL)              (struct tpr01_StmtNameDesc *Desc, tpr05_String *pSQLStatement, tpr01_SQLStatementOptionEnum fOption);
  int        (*GetSQL)              (struct tpr01_StmtNameDesc *Desc, tpr05_String **pSQLStatement, tpr01_SQLStatementOptionEnum fOption);
  void       (*TraceStmtName)       (struct tpr01_StmtNameDesc *Desc, sqlcatype *sqlca);
  void       (*SetState)            (struct tpr01_StmtNameDesc *Desc, tpr01_SQLStateBinary State);
  void       (*DropParsId)          (struct tpr01_StmtNameDesc *Desc, struct tpr01_SQLDesc *SQLDesc);
  void       (*InitStmtNameStruct)  (       tpr00_StmtNameStruct *StmtNameStruct);

} tpr01_StmtNameContainer;


tpr01_StmtNameContainer *pr01StmtNameNewCont();

void pr01StmtNameDeleteCont(tpr01_StmtNameContainer *Cont);

void pr01StmtNameSnEntryInit(sqlsnentry *sn);

#endif
