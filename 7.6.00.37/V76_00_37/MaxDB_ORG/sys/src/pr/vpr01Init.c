/*!================================================================
 module:    vpr01Init.c

 responsible: BurkhardD, MarcoP
 
 special area:   CPCDrv | Client | Runtime System Init
 
 description: Runtime System Init  (former VPR01E)

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

#include "gpr00.h"
#include "vpr01Env.h"
#include "vpr01Module.h"
#include "vpr01Trace.h"
/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

/*==================================================================*
 *  EXTERNAL PROTOTYPES                                             *
 *==================================================================*/

/*==================================================================*
 *  EXPORTED FUNCTIONS                                              *
 *==================================================================*/

/*==================================================================*
 *  FUNCTION PROTOTYPES                                             *
 *==================================================================*/

void p01xcheck (sqlcatype *sqlca, sqlxatype *sqlxa);

/*==================================================================*
 *  CODE                                                            *
 *==================================================================*/

#define cpr_is_true (0)
#define cpr_is_false (1)

void p01xcheck (sqlcatype *sqlca, sqlxatype *sqlxa)
{
  tpr01_ModuleContainer *ModuleCont;
  if (sqlxa->xalcheck[0] != 'C') {
    if (sqlca->sqlext[0] != 'H') {
      sqlca->sqlcxap->xalang = sqlxa->xalang;
      p01_g_init (sqlca);
    }
    sqlxa->xainit = cpr_is_true;
    p01_l_init (sqlca, sqlxa);
  }
  else {
    sqlxa->xainit = cpr_is_false;
  }
  /*check if trace options changed during the runtime*/
  pr01traceParameterRefresh(sqlca, sqlxa);
}
 
