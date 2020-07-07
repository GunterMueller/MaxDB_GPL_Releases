/*!---------------------------------------------------------------------
  @file           RTE_Database.cpp
  @author         JoergM, RobinW
  @brief          DBM Support: SAPDB Instance and Database Registration and Management Interfaces
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
---------------------------------------------------------------------*/

#ifdef WIN32
/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "gos00.h"                      /* nocheck */
#include "RunTime/RTE_NTService.hpp"    /* nocheck */
#include "RunTime/RTE_Database.hpp"     /* nocheck */

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

 /*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/


SAPDB_Bool RTE_Database::UpdateUserAccount( SAPDB_Char *newAccount,SAPDB_Char *newPassword, SAPDBErr_MessageList &errList )
{
    SAPDB_Bool success=false;
    Container_List<RTE_SpeedInfo>::Iterator speedIterator;
    speedIterator = m_speedList.Begin();
    while(!(speedIterator ==  0))
    {
        RTE_NTService service( m_dbName,m_dbRoot,NULL,NULL,*speedIterator);
        success |= service.UpdateServiceEntry((RTE_StartOption)RTE_SERVICE_ENTRY_UNCHANGED,newAccount,newPassword,NULL,NULL,errList);
        ++speedIterator;
    }
    return success;
}

SAPDB_Bool RTE_Database::UpdateAutoStartFlag( SAPDB_Bool autoStart, SAPDBErr_MessageList &errList )
{
    Container_List<RTE_SpeedInfo>::Iterator speedIterator;
    speedIterator = m_speedList.Begin();
    SAPDB_Bool success = false;
    while(!(speedIterator ==  0))
    {
        RTE_NTService service( m_dbName,m_dbRoot,NULL,NULL,*speedIterator);
        if(service.UpdateServiceEntry(autoStart ? RTE_StartAutomatic : RTE_StartOnDemand,(SAPDB_Char *)RTE_SERVICE_ENTRY_UNCHANGED,NULL,NULL,NULL,errList))
        {
            if(autoStart)   // Set the autoStart flag only for the first speed, reset for all
            {
                return true;
            }
            else
            {
                success = true;
            }
        }
        ++speedIterator;
    }
    if(autoStart)
    {
        return false;   // if we should have set the auto start flag and come here, update must have gone wrong...
    }
    else
    {
        return success; // if we should have reset the auto start flag and come here, we have been successful if resetting the flag for at least one speed succeeded
    }
}


#endif
