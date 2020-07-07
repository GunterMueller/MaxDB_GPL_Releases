/*!
  @file           vos94.cpp
  @author         RobinW, RaymondR
  @brief          SAPDB Instance and Database Registration and Management Interfaces
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



\endif
*/




/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gos00.h"

#include "RunTime/RTE_MessageList.hpp"
#include "RunTime/RTE_Messages.hpp"
#include "RunTime/RTE_SpeedInfo.hpp"
#include "RunTime/RTE_DBRegister.hpp"
#include "RunTime/RTE_NTService.hpp"

#include "gsp09.h"

extern "C"
{
#include "heo44.h"
}


/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/
#define MOD__  "VOS94C : "
#define MF__   MOD__"UNDEFINED"


/*------------------------------*/


externC APIRET sql94_service_entry (  ULONG             ulRequest,
                                      ULONG             ulKernelType,
                                      BOOL              fAutoStart,
                                      PSZ               pszNodeName,
                                      PSZ               pszDBRoot,
                                      PSZ               pszServerDB,
                                      PSZ               pszAccountName,
                                      PSZ               pszPassword,
                                      PSZ               pszParameter,
                                      PSZ               pszValue,
                                      PSZ               pszPgmName,
                                      tsp09_rte_xerror *xerror)
{
    RTE_SpeedInfo speed = speedFast;
    switch(ulKernelType)
    {
    case KERNEL_TYPE_SLOW:
        speed = speedSlow;
        break;
    case KERNEL_TYPE_QUICK:
        speed = speedQuick;
        break;
    case KERNEL_TYPE_TEST:
        speed = speedTest;
        break;
    }
    SAPDBErr_MessageList errList;


    SAPDB_Bool returnValue;
    RTE_NTService service(pszServerDB,pszDBRoot,pszPgmName,pszNodeName,speed);
    if( REMOVE_SAPDB_SERVICE == ulRequest)
    {
        returnValue = service.RemoveServiceEntry (errList);
    }
    else
    {
        returnValue = service.UpdateServiceEntry(
            (0 != fAutoStart) ? RTE_StartAutomatic : RTE_StartOnDemand,
            pszAccountName,pszPassword,
            pszParameter,pszValue,
            errList);
    }
    if(!returnValue)
    {
        RTE_FillXErrorFromErrList(xerror, errList);
    }
    return returnValue;
}

