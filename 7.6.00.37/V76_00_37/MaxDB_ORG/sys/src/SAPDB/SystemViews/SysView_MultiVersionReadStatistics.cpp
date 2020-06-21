/****************************************************************************/
/*!

  @file        SysView_MultiVersionReadStatistics.cpp

  -------------------------------------------------------------------------

  @author      ThomasA

  @ingroup     SystemViews
  @brief       This module implements the "MULTIVERSIONREADSTATISTICS" view class.

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
#include "gsp100.h"
#include "hsp100.h"
#include "SystemViews/SysView_Defines.hpp"
#include "SystemViews/SysView_ITableObj.hpp"
#include "SystemViews/SysView_MultiVersionReadStatistics.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "hkb51.h"   // kb51IsConsistUsedTransId

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

void SysView_MultiVersionReadStatistics::Create()
{
    m_Table->AppendCol (ITOCT_FIXED, "MINTRANSACTIONID", 20);
	m_Table->AppendCol (ITOCT_CHAR  ,"RETAINHISTORY", 3);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_MultiVersionReadStatistics::GetColCount()
{
	return 5;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_MultiVersionReadStatistics::EstimateRows()
{
	return 1;
}

/*---------------------------------------------------------------------------*/

void SysView_MultiVersionReadStatistics::Execute()
{
    m_Table->GetCatalogTable();

    if(m_Context.IsOk())
    {
        tgg91_TransNo   MinTransactionid;
        pasbool         RetainHistory = 0;
        kb51GetMinTrans ( m_Context.GetTaskId(), MinTransactionid, RetainHistory );
        m_Table->MoveToCol (ITOVT_TRANSNO, &MinTransactionid, 0);     /* MINTRANSACTIONID */
        m_Table->MoveToCol (ITOVT_YESNO,   &RetainHistory, 0);        /* RETAINHISTORY    */
    }
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SysView_MultiVersionReadStatistics::ResetCounter(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_MultiVersionReadStatistics::ResetCounter", SysView_Trace, 5);
    
    return ( true );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
