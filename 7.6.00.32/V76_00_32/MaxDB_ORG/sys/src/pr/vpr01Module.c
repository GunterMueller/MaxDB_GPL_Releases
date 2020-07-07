/****************************************************************************

  module:    vpr01Module.c

  -------------------------------------------------------------------------

  responsible: BurkhardD

  special area:   CPCDrv | Client | Moduleverwaltung
  description : Verwaltung von Precompilerten Uebersetzungseinheiten

  last changed: 2000-08-07  11:01 Marco Paskamp     unicode enhancement
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


/* next moved to gpr00.h */

#define MAX_MODULE_CMD_GPR00 MinUniqueID_cpr01

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/
#include "gmt90.h"
#include "vpr01Module.h"
#include "vpr01SQL.h"
#include "vpr01Env.h"
#include "vpr01Con.h"
#include "vpr01Precom.h"
#include "vpr01Cursor.h"
#include "vpr01StmtName.h"
#include "gpr03m.h"

#include <stdio.h>

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

static void pr01ModuleInitCont(tpr01_ModuleContainer *Module, tpr01_EnvDesc *EnvDesc);
static tsp00_Int4 pr01ModuleOpenCont(tpr01_ModuleContainer *Module);
static tsp00_Int4 pr01ModuleGetCount(tpr01_ModuleContainer *Module);
static void pr01ModuleCloseCont(tpr01_ModuleContainer *Module);
static tpr01_ModuleDesc *pr01ModuleEnumDesc(tpr01_ModuleContainer *Module);
static tpr01_ModuleDesc *pr01ModuleAddDesc(tpr01_ModuleContainer *Module, sqlxatype *sqlxa);
static void pr01ModuleDeleteDesc(tpr01_ModuleDesc *Desc);
static tpr01_ConContainer *pr01ModuleGetConnectionCont (struct tpr01_ModuleDesc *Desc);
static tpr01_PrecomDesc *pr01ModuleGetPrecomDesc (struct tpr01_ModuleDesc *Desc, sqlxatype *sqlxa, tsp00_Int4 kano);
static tpr01_StmtNameDesc *pr01ModuleFindStmtNameDesc(struct tpr01_ModuleDesc *ModuleDesc, int CmdNo);

static void pr01ModuleStatementBind(tpr01_ModuleDesc *ModuleDesc, tpr05_String *pSQLStatement, int kano, tpr01_SQLStatementOptionEnum fOption);
static void pr01ModuleUniqueName(tpr01_ModuleDesc *ModuleDesc, int CmdNo, char *szStr, int cbLen);
static tsp00_Int4 pr01ModuleUniqueID(tpr01_ModuleDesc *ModuleDesc, int CmdNo);
/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

static void pr01ModuleInitCont(tpr01_ModuleContainer *Module, tpr01_EnvDesc *EnvDesc)
{
  Module->Desc       = pr09NewDescriptor(3, sizeof(tpr01_ModuleDesc));
  Module->EnvDesc    = EnvDesc;
  Module->InitCont   = pr01ModuleInitCont;
  Module->OpenCont   = pr01ModuleOpenCont;
  Module->CloseCont  = pr01ModuleCloseCont;
  Module->GetCount   = pr01ModuleGetCount;
  Module->EnumDesc   = pr01ModuleEnumDesc;
  Module->AddDesc    = pr01ModuleAddDesc;
  Module->DeleteDesc = pr01ModuleDeleteDesc;
  Module->GetPrecomDesc = pr01ModuleGetPrecomDesc;
  Module->GetConnectionCont = pr01ModuleGetConnectionCont;
  Module->FindStmtNameDesc = pr01ModuleFindStmtNameDesc;
  Module->StatementBind = pr01ModuleStatementBind;
  Module->UniqueName    = pr01ModuleUniqueName;
  Module->UniqueID      = pr01ModuleUniqueID;
  Module->ModuleNo = 0;
}

void pr01ModuleDeleteCont(tpr01_ModuleContainer *Module)
{
  if (Module) {
    if (pr01ModuleOpenCont(Module)) {
      /* liefert <> 0 wenn Module Descriptoren vorhanden sind */
      tpr01_ModuleDesc *Desc;
      while(Desc = Module->EnumDesc(Module))
	pr01ModuleDeleteDesc(Desc);
    }
    pr09DeleteDescriptor(Module->Desc);
    Module->Desc = NULL;
    pr03mFree((tsp00_Uint1*)Module, "Module");
  }
}

tpr01_ModuleContainer *pr01ModuleNewCont(struct tpr01_EnvDesc *EnvDesc)
{
  tpr01_ModuleContainer *Module;
  Module = pr03mAllocat(sizeof(tpr01_ModuleContainer), "Module");
  if (Module)
    pr01ModuleInitCont(Module, EnvDesc);
  return Module;
}


static tsp00_Int4 pr01ModuleOpenCont(tpr01_ModuleContainer *Module)
{
  if (Module->Desc) {
    pr09SetIndex(Module->Desc, -1);
  }
  return(Module->GetCount(Module));
}

static tsp00_Int4 pr01ModuleGetCount(tpr01_ModuleContainer *Module)
{
  return(pr09GetItemCount(Module->Desc));
}

static tpr01_ModuleDesc *pr01ModuleEnumDesc(tpr01_ModuleContainer *Module)
{
  tpr01_ModuleDesc *Desc=pr09GetNextItem(Module->Desc);
  return(Desc);
}

static void pr01ModuleCloseCont(tpr01_ModuleContainer *Module)
{
  pr09CloseAllItems(Module->Desc);
}

static tpr01_ModuleDesc *pr01ModuleAddDesc(tpr01_ModuleContainer *Module, sqlxatype *sqlxa)
{
  tpr01_ModuleDesc *ModuleDesc = pr09AddItem(Module->Desc);
  ModuleDesc->DescType         = ModuleDesc_epr01;
  ModuleDesc->Module           = Module;
  ModuleDesc->sqlxa            = sqlxa;
  ModuleDesc->ModuleNo         = Module->ModuleNo++;
  ModuleDesc->Precom           = pr01PrecomNewCont(ModuleDesc);
  ModuleDesc->Cursor           = pr01CursorNewCont(ModuleDesc);
  ModuleDesc->StmtName         = pr01StmtNameNewCont(NULL);
  ModuleDesc->PrecompiledforUnicode = 0;
  return (ModuleDesc);
}

static void pr01ModuleDeleteDesc(tpr01_ModuleDesc *ModuleDesc)
{
  int cbIndex = pr09ItemFindIndex(ModuleDesc->Module->Desc, ModuleDesc);
  if (cbIndex >= 0) {
    pr01PrecomDeleteCont(ModuleDesc->Precom);
    ModuleDesc->Precom = NULL;
    pr01StmtNameDeleteCont(ModuleDesc->StmtName);
    ModuleDesc->StmtName = NULL;
    pr01CursorDeleteCont(ModuleDesc->Cursor);
    ModuleDesc->Cursor = NULL;
    pr09CloseItemEx(ModuleDesc->Module->Desc, cbIndex);
  }
}

static tpr01_ConContainer *pr01ModuleGetConnectionCont (struct tpr01_ModuleDesc *Desc)
{
  M90TRACE(M90_TR_ENTRY, "pr01ModuleGetConnectionCont", 0);
  {
    tpr01_EnvDesc *EnvDesc = Desc->Module->EnvDesc;
    tpr01_EnvContainer *Env = EnvDesc->Env;
    tpr01_ConContainer *Connection = Env->GetConnectionCont(EnvDesc);
    M90TRACE(M90_TR_EXIT, "pr01ModuleGetConnectionCont", 0);
    M90TRACE(M90_TR_HANDLE, "Connection", &Connection);
    return(Connection);
  }
}

static tpr01_PrecomDesc *pr01ModuleGetPrecomDesc (struct tpr01_ModuleDesc *ModuleDesc, sqlxatype *sqlxa, tsp00_Int4 kano)
{
  M90TRACE(M90_TR_ENTRY, "pr01ModuleGetPrecomDesc", 0);
  PR07_CHECKDESC(ModuleDesc, ModuleDesc_epr01);
  {
    tpr01_PrecomContainer *Precom = ModuleDesc->Precom;
    tpr01_PrecomDesc *PrecomDesc = Precom->FindDesc(Precom, kano);
    if (!PrecomDesc) 
      PrecomDesc = Precom->AddDesc(Precom, kano);
    if (!PrecomDesc) {
      Precom->PushError(Precom, ErrDescNotFound);
    }
    M90TRACE(M90_TR_EXIT, "pr01ModuleGetPrecomDesc", 0);
    M90TRACE(M90_TR_HANDLE, "PrecomDesc", &PrecomDesc);
    return(PrecomDesc);
  }
}

static tpr01_StmtNameDesc *pr01ModuleFindStmtNameDesc(struct tpr01_ModuleDesc *ModuleDesc, int CmdNo)
{
  M90TRACE(M90_TR_ENTRY, "pr01ModuleFindStmtNameDesc", 0);
  {
    tpr01_StmtNameContainer *StmtName = ModuleDesc->StmtName;
    tpr01_StmtNameDesc *StmtNameDesc;
    tpr01_UniqueID UniqueID;
    tpr00_StmtNameStruct StmtNameStruct;
    UniqueID = pr01ModuleUniqueID(ModuleDesc, CmdNo);
    StmtName->InitStmtNameStruct (&StmtNameStruct);
    
    StmtNameDesc = StmtName->FindDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID);
    M90TRACE(M90_TR_EXIT, "pr01ModuleFindStmtNameDesc", 0);
    M90TRACE(M90_TR_HANDLE, "StmtNameDesc", &StmtNameDesc);
    return (StmtNameDesc);
  }
}

/*!
  Function:     pr01ModuleStatementBind

  see also: 

  Description:  Binds statement to module. If no statement exists a
                new statement-Descriptor will be created 
    
  Arguments:
    *ModuleDesc     [in] pointer to Modul Descriptor
    *pSQLStatement  [in] pointer to a tpr05_String which contains the sql-statement.
    kano            [in] kommando nummer
    fOption         [in] controls processing of *szSQLStatement
    <UL>
        <LI>fOption == First_epr01     => a new sqlstatement, overwrite buffer 
        <LI>fOption == Append_epr01    => next part of sqlstatement, append on buffer 
        <LI>fOption == Reference_epr01 => only Referenz to sql-statement will be stored 
    </UL>

  return value: void 
 */
static void pr01ModuleStatementBind(tpr01_ModuleDesc *ModuleDesc, 
                                    tpr05_String     *pSQLStatement, 
                                    int              kano, 
                                    tpr01_SQLStatementOptionEnum fOption)
{
  M90TRACE(M90_TR_ENTRY, "pr01ModuleStatementBind", 0);
  M90TRACE(M90_TR_INT, "kano", &kano);
  {
    tpr01_StmtNameContainer *StmtName = ModuleDesc->StmtName;
    tpr01_StmtNameDesc *StmtNameDesc;
    sqlkaentry *ka = ModuleDesc->sqlxa->sqlkap+kano-1;
    tpr01_UniqueID UniqueID;
    tpr00_StmtNameStruct StmtNameStruct;
    M90TRACE(M90_TR_WORD, "ka->kaprindex", &ka->kaprindex);
    UniqueID = pr01ModuleUniqueID(ModuleDesc, kano);
    StmtName->InitStmtNameStruct (&StmtNameStruct);
    StmtNameDesc = StmtName->FindDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID);
    if (!StmtNameDesc) {
      /* Neuen anlegen */
      StmtNameDesc = StmtName->AddDesc(StmtName, &(StmtNameStruct.stStmtName), UniqueID, ka);
    }
    StmtName->PutSQL(StmtNameDesc, pSQLStatement, fOption);
  }
  M90TRACE(M90_TR_EXIT, "pr01ModuleStatementBind", 0);
}

static void pr01ModuleUniqueName(tpr01_ModuleDesc *ModuleDesc, int CmdNo, char *szStr, int cbLen)
{
  if (cbLen > sizeof(tpr01_UniqueName)) {
    tsp00_Int4 UniNumber = ModuleDesc->ModuleNo * MAX_MODULE_CMD_GPR00 + CmdNo;
    sprintf(szStr, ":%#08lx", UniNumber);  
    M90TRACE(M90_TR_STRING, "pr01ModuleUniqueName", szStr);
  }
  else {
    PR07_ASSERT(0);    
  }
}

static tsp00_Int4 pr01ModuleUniqueID(tpr01_ModuleDesc *ModuleDesc, int CmdNo)
{
    return ModuleDesc->ModuleNo * MAX_MODULE_CMD_GPR00 + CmdNo;
}
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/

