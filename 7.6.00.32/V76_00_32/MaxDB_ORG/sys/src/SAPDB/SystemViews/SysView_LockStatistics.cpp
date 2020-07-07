/****************************************************************************/
/*!

  @file        SysView_LockStatistics.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "LockStatistics" view class.

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
#include "SystemViews/SysView_LockStatistics.hpp"
#include "SQLManager/SQLMan_Context.hpp"


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

void SysView_LockStatistics::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LockStatistics::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_FIXED ,SV_ENTRYCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_USEDENTRYCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_USEDENTRYCOUNTPERCENTAGE, 3);
	m_Table->AppendCol (ITOCT_FIXED ,SV_AVGUSEDENTRYCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_AVGUSEDENTRYCOUNTPERCENTAGE, 3);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MAXUSEDENTRYCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MAXUSEDENTRYCOUNTPERCENTAGE, 3);
	m_Table->AppendCol (ITOCT_FIXED ,SV_TABLELOCKCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_ROWLOCKCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_HOLDERCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_REQUESTORCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_LOCKESCALATIONVALUE, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_LOCKESCALATIONCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SQLLOCKCOLLISIONCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_OMSLOCKCOLLISIONCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_DEADLOCKCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SQLREQUESTTIMEOUTCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_OMSREQUESTTIMEOUTCOUNT, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_TRANSACTIONREGIONCOUNT, 5);
	m_Table->AppendCol (ITOCT_FIXED ,SV_TABLEREGIONCOUNT, 5);
	m_Table->AppendCol (ITOCT_FIXED ,SV_ROWREGIONCOUNT, 5);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_LockStatistics::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LockStatistics::GetColCount", SysView_Trace, 5);

    return SV_CC_LOCKSTATISTICS;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_LockStatistics::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LockStatistics::EstimateRows", SysView_Trace, 5);

    return SV_ER_LOCKSTATISTICS;
}

/*---------------------------------------------------------------------------*/

void SysView_LockStatistics::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LockStatistics::Execute", SysView_Trace, 5);

    SysView_SysViewBase::Execute(); // throws e_not_implemented
}

/*---------------------------------------------------------------------------*/

SAPDB_Bool SysView_LockStatistics::ResetCounter(void)
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_LockStatistics::ResetCounter", SysView_Trace, 5);

    m_Context.ThrowError (e_not_implemented, 2);
    return ( true );
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
