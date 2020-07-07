/****************************************************************************/
/*!

  @file        SysView_BackTrace.cpp

  -------------------------------------------------------------------------

  @author      ThomasA

  @ingroup     SystemViews
  @brief       This module implements the SYSDD.BACKTRACE table.

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
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_BackTrace.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "SQLManager/DBProc/DBProc_Debugger.hpp"
#include "hak262.h"

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

void SysView_BackTrace::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_BackTrace::Create", SysView_Trace, 5);

	// SYSDD.BACKTRACE is only for internal use of debugger --> no SYSINFO-view
	// in this view is a symbolic backtrace, 1 row per backtrace-level of 
	// dbproc, function or trigger
	// TYPE       : DBPROC, FUNCTION or TRIGGER
	// SCHEMANAME : Schema of DBPROC, function or trigger
	// TABLENAME  : tablename of table the trigger is defined for (for TYPE=TRIGGER)
	// NAME       : name of DBPROC, function or trigger

	m_Table->AppendCol (ITOCT_CHAR,        SV_TYPE, 8);
    m_Table->AppendCol (ITOCT_CHARUNICODE, SV_SCHEMANAME, 32);
    m_Table->AppendCol (ITOCT_CHARUNICODE, SV_TABLENAME,  32);
    m_Table->AppendCol (ITOCT_CHARUNICODE, SV_NAME, 32);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_BackTrace::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_BackTrace::GetColCount", SysView_Trace, 5);

    return SV_CC_BACKTRACE;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_BackTrace::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_BackTrace::EstimateRows", SysView_Trace, 5);

    return SV_ER_BACKTRACE;
}

/*---------------------------------------------------------------------------*/

void SysView_BackTrace::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_BackTrace::Execute", SysView_Trace, 5);
    m_Table->GetCatalogTable();
    DBProc_Debugger* pDebugger = m_Context.GetDBProcDebugger();
    if (m_Context.IsOk() && pDebugger)
    {
        int level = -1;
        do
        {
            SQLMan_Surrogate methodId;
            int              methodKind;
            int              linkage;
            level = pDebugger->nextBackTrace(level, methodId, methodKind, linkage);
            if (level >= 0)
            {
                tsp00_C8            methodTyp;
                tsp00_KnlIdentifier schema;
                tsp00_KnlIdentifier objName;
                tsp00_KnlIdentifier triggerName;
                a262GetObjectName (m_Context, methodId, methodKind, linkage, methodTyp, schema, objName, triggerName);
                if (m_Context.IsOk())
                {
                    m_Table->MoveToCol (ITOVT_CHARPTR,       &methodTyp, sizeof(methodTyp));
                    m_Table->MoveToCol (ITOVT_KNLIDENTIFIER, &schema, 0);
                    if (cak_i_trigger == methodKind)
                    {
                        m_Table->MoveToCol (ITOVT_KNLIDENTIFIER, &objName, 0);
                        m_Table->MoveToCol (ITOVT_KNLIDENTIFIER, &triggerName, 0);
                    }
                    else
                    {
                        m_Table->MoveToCol (ITOVT_KNLIDENTIFIER, &triggerName, 0); // blankname
                        m_Table->MoveToCol (ITOVT_KNLIDENTIFIER, &objName, 0);
                    }
                    continue;
                }
            }
            break;
        }
        while (level > 0);
    }
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
