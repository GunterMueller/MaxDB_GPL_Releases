/*!================================================================
 module:    vpr01Env.c

 responsible: BurkhardD
 
 special area:   CPCDrv | Client | Environment
 
 description: Verwaltung des Interface Environments
 
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

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "vpr01Env.h"
#include "vpr01Module.h"
#include "vpr01Con.h"
#include "gpr03m.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

static void pr01EnvInitCont(tpr01_EnvContainer *Cont);
static tsp00_Int4 pr01EnvOpenCont(tpr01_EnvContainer *Cont);
static tpr01_EnvDesc *pr01EnvAddDesc(tpr01_EnvContainer *Cont, sqlcatype *sqlca);
static void pr01EnvDeleteDesc(tpr01_EnvDesc *Desc);
static tpr01_EnvDesc *pr01EnvGetDesc(tpr01_EnvContainer *Cont);
static tsp00_Int4 pr01EnvGetCount(tpr01_EnvContainer *Cont);
static tpr01_EnvDesc *pr01EnvEnumDesc(tpr01_EnvContainer *Cont);
static void pr01EnvCloseDesc(tpr01_EnvContainer *Cont);
static tpr01_ConContainer *pr01EnvGetConnectionCont (tpr01_EnvDesc *Desc);

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

static void pr01EnvInitCont(tpr01_EnvContainer *Cont)
{
  Cont->Desc = pr09NewDescriptor(1, sizeof(tpr01_EnvDesc));
  Cont->InitCont   = pr01EnvInitCont;
  Cont->OpenCont   = pr01EnvOpenCont;
  Cont->AddDesc    = pr01EnvAddDesc;
  Cont->DeleteDesc = pr01EnvDeleteDesc;
  Cont->GetDesc    = pr01EnvGetDesc;
  Cont->CloseDesc  = pr01EnvCloseDesc;
  Cont->GetCount   = pr01EnvGetCount;
  Cont->EnumDesc   = pr01EnvEnumDesc;
  Cont->GetConnectionCont = pr01EnvGetConnectionCont;
}

void pr01EnvDeleteCont(tpr01_EnvContainer *Env)
{
  if (Env) {
    if (pr01EnvOpenCont(Env)) {
      tpr01_EnvDesc *Desc;
      while(Desc = pr01EnvEnumDesc(Env))
	pr01EnvDeleteDesc(Desc);
    }
    pr09DeleteDescriptor(Env->Desc);
    Env->Desc = NULL;
    pr03mFree((tsp00_Uint1*)Env, "Env");
  }
}

/*!
  Function:     pr01EnvGetSwapKind

  see also: 
  
  Description:  Determines the swap kind of local maschine
    
  Arguments:

  return value: tsp00_SwapKind_Enum
 */
tsp00_SwapKind_Enum pr01EnvGetSwapKind()
{
  union sw
  {
    char            c[4];
    tsp00_Int4      l;
  }
  swapb;
  
  /* write number 1 in swapb and looking 
     in which byte it could be find */
  swapb.l = 1;
  if (swapb.c[3] == 0x01)
  {
    return sw_normal;
  }
  if (swapb.c[0] == 0x01)
  {
    return sw_full_swapped;
  }
  else
  {
    return sw_part_swapped;
  }
}

tpr01_EnvContainer *pr01EnvNewCont()
{
  tpr01_EnvContainer *Env;
  Env = pr03mAllocat(sizeof(tpr01_EnvContainer), "Env");
  if (Env)
    pr01EnvInitCont(Env);
  return Env;
}

static tsp00_Int4 pr01EnvOpenCont(tpr01_EnvContainer *Cont)
{
  if (Cont->Desc) {
    pr09SetIndex(Cont->Desc, -1);
  }
  return(Cont->GetCount(Cont));
}

static tpr01_EnvDesc *pr01EnvAddDesc(tpr01_EnvContainer *Env, sqlcatype *sqlca)
{
  tpr01_EnvDesc *EnvDesc=pr09AddItem(Env->Desc);
  EnvDesc->DescType = EnvDesc__epr01;
  EnvDesc->Env=Env;
  EnvDesc->sqlca = sqlca;
  EnvDesc->Connection=pr01ConNewCont(EnvDesc);
  EnvDesc->Module=pr01ModuleNewCont(EnvDesc);
  /*set swap kind*/
  EnvDesc->swapKind=pr01EnvGetSwapKind();
  sqlLocalMachineSwapKind = EnvDesc->swapKind;
  sqlca->sqlgap->gaConnection=EnvDesc->Connection;
  return(EnvDesc);
}

static void pr01EnvDeleteDesc(tpr01_EnvDesc *EnvDesc)
{
  int cbIndex = pr09ItemFindIndex(EnvDesc->Env->Desc, EnvDesc);
  
  if (cbIndex >= 0) {
    tpr01_ModuleContainer *Module = EnvDesc->Module;
    tpr01_ConContainer *Connection = EnvDesc->Connection;
    pr01ModuleDeleteCont(Module);
    pr01ConDeleteCont(Connection);
    pr09CloseItemEx(EnvDesc->Env->Desc, cbIndex);
  }    
}
static tpr01_EnvDesc *pr01EnvGetDesc(tpr01_EnvContainer *Cont)
{
  return(pr09GetItem(Cont->Desc));
}

static tsp00_Int4 pr01EnvGetCount(tpr01_EnvContainer *Cont)
{
  return(pr09GetItemCount(Cont->Desc));
}

static tpr01_EnvDesc *pr01EnvEnumDesc(tpr01_EnvContainer *Cont)
{
  tpr01_EnvDesc *Desc=pr09GetNextItem(Cont->Desc);
  return(Desc);
}

static void pr01EnvCloseDesc(tpr01_EnvContainer *Cont)
{
  pr09CloseAllItems(Cont->Desc);
}

static tpr01_ConContainer *pr01EnvGetConnectionCont (tpr01_EnvDesc *Desc)
{
  return(Desc->Connection);
}
