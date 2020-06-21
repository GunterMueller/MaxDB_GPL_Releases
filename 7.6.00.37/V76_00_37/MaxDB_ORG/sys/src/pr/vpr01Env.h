/*!*************************************************************************

  module      : MyModule.cpp

  -------------------------------------------------------------------------

  responsible: BurkhardD

  special area:   CPCDrv | Client | Environment

  description: Verwaltung des Interface Environments


  last changed: 2000-08-11  10:17 Marco Paskamp - unicode enhancement
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


#ifndef __VPR01_ENV_H__
#define __VPR01_ENV_H__

/*==================================================================*
 *  INCLUDES                                                        *
 *==================================================================*/

#include "gpr00.h"
#include "gpr01.h"
#include "vpr09DynaDesc.h"

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

typedef struct tpr01_EnvDesc {
  tpr01_DescTypeEnum      DescType;
  struct tpr01_EnvContainer *Env;        
  struct tpr01_ModuleContainer *Module;  /* contains all modules */ 
  struct tpr01_ConContainer *Connection; /* connection all connections */ 
  sqlcatype *sqlca;               /* corresponding sqlca area */
  tsp00_SwapKind_Enum swapKind;   /* stores swap kind of local maschine*/
} tpr01_EnvDesc;

typedef struct tpr01_EnvContainer {
  tpr09DynamicDescriptor *Desc;
  void           (*InitCont)   (struct tpr01_EnvContainer *Cont);
  tsp00_Int4     (*OpenCont)   (struct tpr01_EnvContainer *Cont);
  tpr01_EnvDesc *(*AddDesc)    (struct tpr01_EnvContainer *Cont, sqlcatype *sqlca);
  void           (*DeleteDesc) (tpr01_EnvDesc *Desc);
  tpr01_EnvDesc *(*GetDesc)    (struct tpr01_EnvContainer *Cont);
  void           (*CloseDesc)  (struct tpr01_EnvContainer *Cont);
  tsp00_Int4     (*GetCount)   (struct tpr01_EnvContainer *Cont);
  tpr01_EnvDesc *(*EnumDesc)   (struct tpr01_EnvContainer *Cont);
  struct tpr01_ConContainer *(*GetConnectionCont)   (struct tpr01_EnvDesc *Desc);
} tpr01_EnvContainer;


/*==================================================================*
 *  EXTERNAL MEMBERS                                                *
 *==================================================================*/

#ifdef __cplusplus
extern "C"
#endif

/*==================================================================*
 *  PROTOTYPES                                                      *
 *==================================================================*/

tpr01_EnvContainer *pr01EnvNewCont();

void pr01EnvDeleteCont(tpr01_EnvContainer *Cont);

extern char *pr01EnvIsSwapped;

#endif
/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
