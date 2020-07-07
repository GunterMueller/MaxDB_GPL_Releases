/*!================================================================
 module:    vpr04Param.c

 responsible: BurkhardD
 
 special area:   CPCDrv | Client | Parameterverwaltung
 
 description: Verwaltung von Hostvariablen
 
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

#include "vpr04Param.h"
#include "vpr01SQL.h"
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

static void pr04InitParamContainer (tpr04_ParamContainer *pCont, sqlxatype *sqlxap);
static void pr04Free(tpr04_ParamContainer *pCont, sqlxatype *sqlxap);
static tsp00_Int4 pr04OpenParam(tpr04_ParamContainer *pCont);
static tsp00_Int4 pr04GetCount(tpr04_ParamContainer *pCont);
static void pr04CloseParam(tpr04_ParamContainer *pCont);
static tpr04_ParamDesc *pr04GetNextParam(tpr04_ParamContainer *pCont);
static void *pr04GetHostVarAddr(tpr04_ParamDesc *pDesc);
static tsp00_Int2 pr04GetHostVarType(tpr04_ParamDesc *pDesc);

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

static void pr04InitParamContainer(tpr04_ParamContainer *pCont, sqlxatype *sqlxap)
{
  pCont->cbCount             = 0;
  pCont->pDesc               = NULL;
  pCont->sqlxap              = sqlxap;
  pCont->InitCont            = pr04InitParamContainer;
  pCont->OpenCont            = pr04OpenParam;
  pCont->CloseCont           = pr04CloseParam;
  pCont->GetCount            = pr04GetCount;
  pCont->EnumDesc            = pr04GetNextParam;
  pCont->GetHostVarAddr      = pr04GetHostVarAddr;
  pCont->GetHostVarType      = pr04GetHostVarType;  
}

void pr04DeleteParamContainer(tpr04_ParamContainer *Param)
{
  if (Param) {
    if (Param->pDesc)
      pr09DeleteDescriptor(Param->pDesc);
    pr03mFree((tsp00_Uint1*)Param, "Param");
  }
}

tpr04_ParamContainer *pr04NewParamContainer(sqlxatype *sqlxap)
{
  tpr04_ParamContainer *Param;
  Param = pr03mAllocat(sizeof(tpr04_ParamContainer), "Param");
  if (Param)
    pr04InitParamContainer(Param, sqlxap);
  return Param;
}

static tsp00_Int4 pr04OpenParam(tpr04_ParamContainer *Param)
{
  sqlxatype   *sqlxa = Param->sqlxap;
  sqlkaentry  *kap = sqlxa->xaSQLDesc->ka;
  sqlparpointer  pap;
  if (!Param->pDesc) {
    Param->pDesc = pr09NewDescriptor(20, sizeof(tpr04_ParamDesc));
  }
  else {
    pr09CloseAllItems(Param->pDesc);
  }
  Param->cbCount = kap->kapacount - 1;
  pap = sqlxa->sqlpap+kap->kapaindex - 1;
  if (pap) {
    int i;
    for (i=0; i < Param->cbCount; i++) {
      tpr04_ParamDesc *p;      
      p = pr09AddItem(Param->pDesc);
      p->sqlv1p=sqlxa->sqlv1p;
      p->sqlv2p=sqlxa->sqlv2p;
      p->HostVarDesc.va1mn=NULL;
      p->HostVarDesc.va1mx=NULL;
      p->pParam = pap++;
    }
  }
  pr09SetIndex(Param->pDesc, -1);
  return(Param->cbCount);
}

static tsp00_Int4 pr04GetCount(tpr04_ParamContainer *Param)
{
  return(Param->cbCount);
}

static tpr04_ParamDesc *pr04GetNextParam(tpr04_ParamContainer *Param)
{
  sqlv1pointer va1;
  tsp00_Int2 elcnt=0;
  tpr04_ParamDesc *pDesc=pr09GetItem(Param->pDesc);

  if (pDesc && 
      pDesc->HostVarDesc.va1 < pDesc->HostVarDesc.va1mx) {
    pDesc->HostVarDesc.va1++;
    return(pDesc);
  }
  else {
    pDesc=pr09GetNextItem(Param->pDesc);
    if (!pDesc)
      return pDesc;
    pDesc->HostVarDesc.va1mx = 0;
    pDesc->HostVarDesc.va1mn = 0;
    pDesc->HostVarDesc.va1   = 0;
  }
  if (pDesc) {
    sqlv1pointer sqlv1p;
    do {
      sqlv1p = pDesc->sqlv1p;    
      if (pDesc->pParam->st.pakind == SQLPARST) {
	va1 = sqlv1p + pDesc->pParam->st.pavarst - 1;
	elcnt = pDesc->pParam->st.paelcnt;
      }
      if (pDesc->pParam->st.pakind == SQLPAREL)
	pDesc=pr09GetNextItem(Param->pDesc);
      else
	break;
    } while (pDesc && pDesc->pParam->st.pakind != SQLPAREL);
    if (!pDesc) {
      return pDesc;
    }
    va1 = sqlv1p + pDesc->pParam->el.pavarno - 1;
    if (va1->cm.va1indi == SQLVACM) {
      pDesc->HostVarDesc.va1mn = va1;
      pDesc->HostVarDesc.va1mx = va1 + elcnt - 1;
    }
    else {
      if (va1->pt.va1indi == SQLVAPT)
	va1 = sqlv1p + va1->pt.va1ix - 1;
      if (va1->sc.va1indi== SQLVAST) {
	pDesc->HostVarDesc.va1mn = sqlv1p + va1->st.va1ix - 1;
	pDesc->HostVarDesc.va1mx = pDesc->HostVarDesc.va1mn + va1->st.va1cmpcnt - 1;
      }
      else {
	pDesc->HostVarDesc.va1mn = va1;
	pDesc->HostVarDesc.va1mx = va1;
      }
    }
    pDesc->HostVarDesc.va1 = va1;
  }
  return(pDesc);
}

static void pr04CloseParam(tpr04_ParamContainer *Param)
{
  pr09CloseAllItems(Param->pDesc);
}

static void *pr04GetHostVarAddr(tpr04_ParamDesc *pDesc)
{
  sqlparpointer pa=pDesc->pParam;
  sqlv1pointer  sqlv1p=pDesc->sqlv1p;
  if (pa->st.pakind == SQLPARST) {
    sqlv1pointer  va = sqlv1p + pa->st.pavarst - 1;
    switch(va->pt.va1indi) {
    case (SQLVAPT) : {
      return *((char **) va->pt.va1addr);
    }
    case (SQLVAST) : {
      return (char *) va->st.va1addr;
    }
    case (SQLVASC) : {
      return (char *) va->sc.va1addr;
    }
    }
  }
  return NULL;  
}

static tsp00_Int2 pr04GetHostVarType(tpr04_ParamDesc *pDesc)
{
  sqlv1pointer va1 = pDesc->HostVarDesc.va1;
  sqlv2pointer va2 = pDesc->sqlv2p + va1->sc.va1indva2 - 1;
  return(va2->va2typ);
}

