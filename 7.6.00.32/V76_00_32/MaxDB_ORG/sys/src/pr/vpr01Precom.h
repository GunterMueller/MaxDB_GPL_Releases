/*!================================================================
 module:    vpr01Precom.h

 responsible: BurkhardD
 
 special area:   CPCDrv | Client | Precompiler Kommandos 
 description: Verwaltung von EXEC SQL Statements
 
 see:  

 change history: 
         2001-11-08 D025086 Cancel session bugfix PTS 1112515 and PASCAL 
                    port from p01cancel,
         2000-08-07  11:01 Marco Paskamp     unicode enhancement
         2000-11-16 d025086 (PTS 1108288) pr01PrecomVersion new


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

#ifndef __VPR01_PRECOM_H__
#define __VPR01_PRECOM_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"
#include "gpr01.h"
#include "vpr09DynaHash.h"
#include "vpr09DynaDesc.h"
/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

typedef struct tpr01_PrecomDesc {
  tpr01_DescTypeEnum      DescType;
  struct tpr01_PrecomContainer  *Precom;
  sqlkaentry             *ka;
  sqlprentry             *pr;
  tsp00_Int4              CmdNo;
  struct tpr01_StmtNameDesc     *StmtNameDesc;
  struct tpr01_SQLDesc     *SQLDesc;
  struct tpr01_ErrorDesc        *ErrorDesc;
  tpr00_ComTypeEnum ComType;
} tpr01_PrecomDesc;

typedef struct tpr01_PrecomContainer {
  struct tpr01_ModuleDesc  *ModuleDesc;
  struct tpr01_SQLContainer     *SQL;
#ifdef DYNAHASH  
  tpr09DynamicHashDescriptor *Desc;
#else
  tpr09DynamicDescriptor *Desc;
#endif 
  struct tpr01_ErrorDesc        *ErrorDesc;  
  void           (*InitCont)  (struct tpr01_PrecomContainer *Cont, struct tpr01_ModuleDesc *ModuleDesc, sqlcatype *sqlca, sqlxatype  *sqlxa);
  tsp00_Int4     (*OpenCont)  (struct tpr01_PrecomContainer *Cont);
  void           (*CloseCont) (struct tpr01_PrecomContainer *Cont);
  void           (*ResetIterator)   (struct tpr01_PrecomContainer *Cont);
  tpr01_PrecomDesc * (*EnumDesc)  (struct tpr01_PrecomContainer *Cont);
  tpr01_PrecomDesc * (*AddDesc)   (struct tpr01_PrecomContainer *Cont, tsp00_Int4 CmdNo);
  void           (*DeleteDesc) (tpr01_PrecomDesc *PrecomDesc);
  tpr01_PrecomDesc * (*FindDesc)   (struct tpr01_PrecomContainer *Cont, tsp00_Int4 CmdNo);
  struct tpr01_SQLContainer * (*GetSQLCont) (struct tpr01_PrecomContainer *Cont);
  struct tpr01_ConDesc  *(*GetConnection) (tpr01_PrecomDesc *PrecomDesc, struct tpr01_ModuleDesc *ModuleDesc, tsp00_Int4 nSession);
  void           (*PrepareStmtName) (tpr01_PrecomDesc *PrecomDesc, struct tpr01_ModuleDesc *ModuleDesc, struct tpr01_ConDesc *ConDesc);
  struct tpr01_CursorDesc * (*DeclareCursor)   (tpr01_PrecomDesc *Desc, struct tpr01_ModuleDesc *ModuleDesc, struct tpr01_ConDesc *ConDesc);
  void           (*Describe) (tpr01_PrecomDesc *Desc, struct tpr01_ModuleDesc *ModuleDesc, struct tpr01_ConDesc *ConDesc);
  void           (*Open)     (tpr01_PrecomDesc *Desc, struct tpr01_ModuleDesc *ModuleDesc, struct tpr01_ConDesc *ConDesc);
  void           (*TryExecute)    (tpr01_PrecomDesc *Desc, struct tpr01_ModuleDesc *ModuleDesc, struct tpr01_ConDesc *ConDesc);
  void           (*Execute)       (tpr01_PrecomDesc *Desc, struct tpr01_ModuleDesc *ModuleDesc, struct tpr01_ConDesc *ConDesc);
  void           (*Prepare)       (tpr01_PrecomDesc *Desc, struct tpr01_ModuleDesc *ModuleDesc, struct tpr01_ConDesc *ConDesc);
  void           (*ExecDirect)    (tpr01_PrecomDesc *Desc);
  boolean        (*Connect)       (tpr01_PrecomDesc *Desc, struct tpr01_ConDesc *ConDesc, struct tpr01_StmtNameDesc *StmtNameDesc);
  void           (*StatementBind) (tpr01_PrecomDesc *Desc, tpr05_String *pSQLStatement, tpr01_SQLStatementOptionEnum fOption, struct tpr01_ModuleDesc *ModuleDesc, struct tpr01_ConDesc *ConDesc);
  tpr01_UniqueID (*GetStmtName)   (tpr01_PrecomDesc *Desc, tpr05_String *stStmtName);
  tpr01_UniqueID (*GetCursorName) (tpr01_PrecomDesc *Desc, tpr00_CursorNameDesc *CursorNameDesc);
  tsp00_Int4     (*GetError)      (tpr01_PrecomDesc *Desc, struct tpr01_ErrorDesc *ErrorDesc);
  tsp00_Int4     (*SetError)      (tpr01_PrecomDesc *Desc, tpr_runtime_errors_Enum ErrorEnum);
  void           (*PushError)     (struct tpr01_PrecomContainer *Cont, tpr00_ErrorKeyEnum ErrorKey);
  void           (*Option)        (struct tpr01_PrecomDesc *PrecomDesc, struct tpr01_ModuleDesc *ModuleDesc, struct tpr01_ConDesc *ConDesc);
  void           (*TraceLine)        (struct tpr01_PrecomDesc *PrecomDesc, struct tpr01_ModuleDesc *ModuleDesc, struct tpr01_ConDesc *ConDesc);
  void           (*Version)        (struct tpr01_PrecomDesc *PrecomDesc, struct tpr01_ModuleDesc *ModuleDesc, struct tpr01_ConDesc *ConDesc);
  void           (*Cancel)        (struct tpr01_PrecomDesc *PrecomDesc, struct tpr01_ModuleDesc *ModuleDesc, struct tpr01_ConDesc *ConDesc);
  void           (*AbortSession)        (struct tpr01_PrecomDesc *PrecomDesc, struct tpr01_ModuleDesc *ModuleDesc, struct tpr01_ConDesc *ConDesc);
  sqlxatype  *sqlxa;
  sqlcatype  *sqlca;
} tpr01_PrecomContainer;

/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

tpr01_PrecomContainer *pr01PrecomNewCont(struct tpr01_ModuleDesc *ModuleDesc);

void pr01PrecomDeleteCont(tpr01_PrecomContainer *Cont);

#endif
