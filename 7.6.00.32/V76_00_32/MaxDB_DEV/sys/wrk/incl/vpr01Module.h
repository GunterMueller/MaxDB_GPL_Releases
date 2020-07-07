/*!================================================================
 module:    vpr01Module.h

 responsible: BurkhardD

 special area:   CPCDrv | Client | Moduleverwaltung

 description: Verwaltung von Precompilerten Uebersetzungseinheiten

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

#ifndef __VPR01_MODULE_H__
#define __VPR01_MODULE_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"
#include "gpr01.h"
#include "vpr09DynaDesc.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

typedef struct tpr01_ModuleDesc {
  tpr01_DescTypeEnum      DescType;
  struct tpr01_ModuleContainer *Module;
  struct tpr01_PrecomContainer *Precom;
  struct tpr01_StmtNameContainer *StmtName;
  struct tpr01_CursorContainer *Cursor;
  tsp00_Int4 ModuleNo;
  tpr01_Precompiledfor PrecompiledforUnicode;  /* = 1 if application was precompiled with switch -G unicode */
  sqlxatype *sqlxa;
} tpr01_ModuleDesc;

typedef struct tpr01_ModuleContainer {
  struct tpr01_EnvDesc *EnvDesc;
  tpr09DynamicDescriptor *Desc;
  tsp00_Int4 ModuleNo;
  void              (*InitCont)   (struct tpr01_ModuleContainer *Cont, struct tpr01_EnvDesc *EnvDesc);
  tsp00_Int4        (*OpenCont)   (struct tpr01_ModuleContainer *Cont);
  void              (*CloseCont)  (struct tpr01_ModuleContainer *Cont);
  tpr01_ModuleDesc *(*AddDesc)    (struct tpr01_ModuleContainer *Cont, sqlxatype *sqlxa);
  void              (*DeleteDesc) (struct tpr01_ModuleDesc *Desc);
  tsp00_Int4        (*GetCount)   (struct tpr01_ModuleContainer *Cont);
  tpr01_ModuleDesc *(*EnumDesc)   (struct tpr01_ModuleContainer *Cont);
  struct tpr01_ConContainer *(*GetConnectionCont)   (struct tpr01_ModuleDesc *Desc);
  struct tpr01_PrecomDesc *(*GetPrecomDesc) (struct tpr01_ModuleDesc *Desc, sqlxatype *sqlxa, tsp00_Int4 kano);
  struct tpr01_StmtNameDesc *(*FindStmtNameDesc) (struct tpr01_ModuleDesc *ModuleDesc, int CmdNo);
  void (*StatementBind) (struct tpr01_ModuleDesc *ModuleDesc, tpr05_String *pSQLStatement, int kano, tpr01_SQLStatementOptionEnum fOption);
  void (*UniqueName)    (struct tpr01_ModuleDesc *ModuleDesc, int CmdNo, char *szStr, int cbLen);
  tsp00_Int4 (*UniqueID) (struct tpr01_ModuleDesc *ModuleDesc, int CmdNo);
} tpr01_ModuleContainer;

/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

tpr01_ModuleContainer *pr01ModuleNewCont(struct tpr01_EnvDesc *EnvDesc);

void pr01ModuleDeleteCont(tpr01_ModuleContainer *Cont);

#endif
