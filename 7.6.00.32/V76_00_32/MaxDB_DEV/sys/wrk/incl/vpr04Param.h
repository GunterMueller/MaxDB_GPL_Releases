/*!================================================================
 module:    vpr04Param.h

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

#ifndef __VPR04_PARAM_H__
#define __VPR04_PARAM_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"
#include "vpr09DynaDesc.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

typedef struct tpr04_HostVarDesc {
  sqlv1pointer  va1;
  sqlv1pointer  va1mn;
  sqlv1pointer  va1mx;
} tpr04_HostVarDesc;

typedef struct tpr04_ParamDesc {
  sqlparpointer     pParam;
  sqlv1pointer      sqlv1p;
  sqlv2pointer      sqlv2p;
  tpr04_HostVarDesc HostVarDesc;
} tpr04_ParamDesc;

typedef struct tpr04_ParamContainer tpr04_ParamContainer;

struct tpr04_ParamContainer {
  tpr09DynamicDescriptor *pDesc;
  tsp00_Int4 cbCount;
  sqlxatype  *sqlxap;
  void             (*InitCont) (tpr04_ParamContainer *pCont, sqlxatype *sqlxap);
  tsp00_Int4       (*OpenCont) (tpr04_ParamContainer *pCont);
  void             (*CloseCont) (tpr04_ParamContainer *pCont);
  tsp00_Int4       (*GetCount) (tpr04_ParamContainer *pCont);
  tpr04_ParamDesc *(*EnumDesc) (tpr04_ParamContainer *pCont);
  void *           (*GetHostVarAddr) (tpr04_ParamDesc *pDesc);
  tsp00_Int2       (*GetHostVarType) (tpr04_ParamDesc *pDesc);
};

/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

tpr04_ParamContainer *pr04NewParamContainer(sqlxatype *sqlxap);

void pr04DeleteParamContainer(tpr04_ParamContainer *pCont);

#endif
