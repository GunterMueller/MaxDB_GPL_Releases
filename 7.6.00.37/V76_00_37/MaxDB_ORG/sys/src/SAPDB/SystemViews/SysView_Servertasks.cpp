/****************************************************************************/
/*!

  @file        SysView_Servertasks.cpp

  -------------------------------------------------------------------------

  @author      UweH

  @ingroup     SystemViews
  @brief       This module implements the "SERVERTASKS" view class.

  @see        

*/
/*-------------------------------------------------------------------------

  copyright:    Copyright (c) 2002-2005 SAP AG


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_Servertasks.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "ServerTasks/SrvTasks_ICoordinator.hpp"
#include "RunTime/Tasking/RTETask_ITask.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/



/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/



/*===========================================================================*
 *  LOCAL CLASSES, STRUCTURES, TYPES, UNIONS ...                             *
 *===========================================================================*/



/*===========================================================================*
 *  STATIC/INLINE FUNCTIONS  (PROTOTYPES)                                     *
 *===========================================================================*/



/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

void SysView_Servertasks::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Servertasks::Create", SysView_Trace, 5);

    m_Table->AppendCol (ITOCT_FIXED, SV_TASKID,             10);
    m_Table->AppendCol (ITOCT_CHAR,  SV_ISASSIGNED,          3);
    m_Table->AppendCol (ITOCT_CHAR,  SV_ISRESERVED,          3);
    m_Table->AppendCol (ITOCT_CHAR,  SV_LASTJOBDESCRIPTION, 32);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_Servertasks::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Servertasks::GetColCount", SysView_Trace, 5);

    return SV_CC_SERVERTASKS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_Servertasks::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Servertasks::EstimateRows", SysView_Trace, 5);

    return SV_ER_SERVERTASKS;
}

/*---------------------------------------------------------------------------*/

void SysView_Servertasks::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_Servertasks::Execute", SysView_Trace, 5);

    m_Table->GetCatalogTable();

    if ( ! m_Context.IsOk() )
        return;

    RTETask_ITask &task = *RTETask_ITask::ByTaskID(m_Context.GetTaskId());

    SAPDB_UInt           controlNo = 0;
    SAPDB_UInt           servertaskid;
    bool                 isAssigned;
    bool                 isReserved;
    RTE_TaskDisplayName  description;
    
    while ( SrvTasks_ICoordinator::Instance().GetInfo ( task,
                                                        controlNo,
                                                        servertaskid,
                                                        isAssigned,
                                                        isReserved,
                                                        description) )
    {
        for (int c = 0; c < sizeof(RTE_TaskDisplayName)-1; c++)
            description[c] = toupper(description[c]);

        AddColumn (servertaskid);
        AddColumn (isAssigned?"YES":"NO");
        AddColumn (isReserved?"YES":"NO");
        AddColumn (description);
        ++controlNo;
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
