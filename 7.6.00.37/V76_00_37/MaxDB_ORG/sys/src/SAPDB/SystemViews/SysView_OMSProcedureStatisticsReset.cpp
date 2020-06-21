/****************************************************************************/
/*!

  @file        SysView_OMSProcedureStatisticsReset.cpp

  -------------------------------------------------------------------------

  @author      ElkeZ

  @ingroup     SystemViews
  @brief       This module implements the "OMSProcedureStatisticsReset" view class.

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
#include "SystemViews/SysView_OMSProcedureStatisticsReset.hpp"


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

void SysView_OMSProcedureStatisticsReset::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_OMSProcedureStatisticsReset::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_RESETDATE, 0);
	m_Table->AppendCol (ITOCT_CHARUNICODE ,SV_PACKAGENAME, 32);
	m_Table->AppendCol (ITOCT_CHAR ,SV_PROCEDURENAME, 32);
	m_Table->AppendCol (ITOCT_FIXED ,SV_CALLCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_READCACHEDOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_READOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_READCACHEDOBJECTVIAKEYCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_READOBJECTVIAKEYCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_UPDATECACHEDOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_UPDATEOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_DELETECACHEDOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_DELETEOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_LOCKOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SCANOBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_READOBJECTHISTORYACCESSCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SCANOBJECTHISTORYACCESSCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_CPPEXCEPTIONCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_OUTOFDATAEXCEPTIONCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_TIMEOUTEXCEPTIONCOUNT,  20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_OUTOFMEMORYEXCEPTIONCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_TERMINATECOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_ROLLBACKSUBTRANSCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_COMMITSUBTRANSCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_SUBTRANSACTIONLEVEL, 5);
	m_Table->AppendCol (ITOCT_FIXED ,SV_READVAROBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_UPDATEVAROBJECTCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_AVGVAROBJECTLENGTH, 5);
	m_Table->AppendCol (ITOCT_FIXED ,SV_AVGWAITTIMEFORNEWCONSISTENTVIEW, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MINWAITTIMEFORNEWCONSISTENTVIEW, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MAXWAITTIMEFORNEWCONSISTENTVIEW, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_AVGREADSTREAMBUFFERCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MINREADSTREAMBUFFERCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MAXREADSTREAMBUFFERCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_AVGWRITESTREAMBUFFERCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MINWRITESTREAMBUFFERCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MAXWRITESTREAMBUFFERCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_AVGREADSTREAMROWCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MINREADSTREAMROWCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MAXREADSTREAMROWCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_AVGWRITESTREAMROWCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MINWRITESTREAMROWCOUNT, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MAXWRITESTREAMROWCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED ,SV_MEMORYREQUESTCOUNT,        10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_AVGREQUESTMEMORYCHUNKSIZE, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MINREQUESTMEMORYCHUNKSIZE, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MAXREQUESTMEMORYCHUNKSIZE, 10);
    m_Table->AppendCol (ITOCT_FIXED ,SV_MEMORYRELEASECOUNT,        10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_AVGRELEASEMEMORYCHUNKSIZE, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MINRELEASEMEMORYCHUNKSIZE, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MAXRELEASEMEMORYCHUNKSIZE, 10);
	m_Table->AppendCol (ITOCT_FIXED ,SV_AVGMEMORYDELTA, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MINMEMORYDELTA, 20);
	m_Table->AppendCol (ITOCT_FIXED ,SV_MAXMEMORYDELTA, 20);
    m_Table->AppendCol (ITOCT_FIXED ,SV_LOCKOBJECTCOLLISIONCOUNT, 20);
    m_Table->AppendCol (ITOCT_FIXED ,SV_AVGLOCKOBJECTWAITTIME , 20);

}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_OMSProcedureStatisticsReset::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_OMSProcedureStatisticsReset::GetColCount", SysView_Trace, 5);

    return SV_CC_OMSPROCEDURESTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_OMSProcedureStatisticsReset::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_OMSProcedureStatisticsReset::EstimateRows", SysView_Trace, 5);

    return SV_ER_OMSPROCEDURESTATISTICSRESET;
}

/*---------------------------------------------------------------------------*/

void SysView_OMSProcedureStatisticsReset::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_OMSProcedureStatisticsReset::Execute", SysView_Trace, 5);

    SysView_SysViewBase::Execute(); // throws e_not_implemented
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
