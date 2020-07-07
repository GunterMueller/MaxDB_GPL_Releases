/****************************************************************************/
/*!

  @file        SysView_RestartInformation.cpp

  -------------------------------------------------------------------------

  @author      UweH

  @ingroup     SystemViews
  @brief       This module implements the "RestartInformation" view class.

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
#include "SystemViews/SysView_RestartInformation.hpp"
#include "Logging/Log_Savepoint.hpp"
#include "Logging/Log_Volume.hpp"
#include "SQLManager/SQLMan_Context.hpp"
#include "Transaction/Trans_Context.hpp"


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

void SysView_RestartInformation::Create()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_RestartInformation::Create", SysView_Trace, 5);

	m_Table->AppendCol (ITOCT_FIXED,     SV_BADINDEXCOUNT,               5);
	m_Table->AppendCol (ITOCT_FIXED,     SV_RESTARTCONVERTERVERSION,     10);
	m_Table->AppendCol (ITOCT_FIXED,     SV_LASTSAVEPOINTID,             10);
	m_Table->AppendCol (ITOCT_CHAR,      SV_LASTSAVEPOINTREASON,         20);
	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_LASTSAVEPOINTDATE,           0);
	m_Table->AppendCol (ITOCT_FIXED,     SV_OPENTRANSACTIONCOUNT,        10);
	m_Table->AppendCol (ITOCT_FIXED,     SV_OPENTRANSACTIONFILEROOT,     10);
	m_Table->AppendCol (ITOCT_FIXED,     SV_HISTORYDIRECTORYROOT,        10);
	m_Table->AppendCol (ITOCT_FIXED,     SV_FILEDIRECTORYROOT,           10);
	m_Table->AppendCol (ITOCT_FIXED,     SV_LOBDIRECTORYROOT,            10);
	m_Table->AppendCol (ITOCT_FIXED,     SV_RESTARTIOSEQUENCE,           10);
	m_Table->AppendCol (ITOCT_FIXED,     SV_RESTARTOFFSETOFPAGEONVOLUME, 10);
	m_Table->AppendCol (ITOCT_FIXED,     SV_RESTARTOFFSETOFENTRYINPAGE,  10);
	m_Table->AppendCol (ITOCT_CHAR,      SV_RESTARTENTRYTYPE,            20);
	m_Table->AppendCol (ITOCT_TIMESTAMP, SV_RESTARTENTRYDATE,            0);
	m_Table->AppendCol (ITOCT_FIXED,     SV_CYCLERELATEDENDOFFSET,       10);
	m_Table->AppendCol (ITOCT_FIXED,     SV_LASTKNOWNSEQUENCE,           10);
	m_Table->AppendCol (ITOCT_FIXED,     SV_LASTKNOWNOFFSET,             10);
	m_Table->AppendCol (ITOCT_CHAR,      SV_DATABASEIDENTIFIER,          100);
	m_Table->AppendCol (ITOCT_CHAR,      SV_LASTSAVEPOINTRELEASEVERSION, 40);
	m_Table->AppendCol (ITOCT_CHAR,      SV_INSTALLSERVERDBVERSION,      40);
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_RestartInformation::GetColCount()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_RestartInformation::GetColCount", SysView_Trace, 5);

    return SV_CC_RESTARTINFORMATION;
}

/*---------------------------------------------------------------------------*/

SAPDB_Int SysView_RestartInformation::EstimateRows()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_RestartInformation::EstimateRows", SysView_Trace, 5);

    return SV_ER_RESTARTINFORMATION;
}

/*---------------------------------------------------------------------------*/

void SysView_RestartInformation::Execute()
{
    SAPDBTRACE_METHOD_DEBUG ("SysView_RestartInformation::Execute", SysView_Trace, 5);

    m_Table->GetCatalogTable();

    Log_Volume &log = Log_Volume::Instance();

    Log_Savepoint::RestartInformation info;
    Log_Savepoint::GetRestartInformation (m_Context.GetTransContext().Task(), info);

    AddColumn (info.badIndexCount);
    AddColumn (info.previousConverterVersion);
    AddColumn (info.lastSavepointId);
    AddColumn (info.lastSavepointReason);
    AddColumn (info.lastSavepointDate, info.lastSavepointTime);
    AddColumn (info.openTransactionCount);
    AddColumn (info.openTransactionFileRoot);
    AddColumn (info.historyDirectoryFileRoot);
    AddColumn (info.fileDirectoryRoot);
    AddColumn (info.longFileDirectoryRoot);
    AddColumn (info.restartIOSequence);
    AddColumn (info.restartLogVolumeOffset);
	AddColumn (info.restartLogEntryOffset);
	AddColumn (info.restartLogEntryType);
    AddColumn (info.restartEntryDate, info.restartEntryTime);
    AddColumn (log.LogicalLogEndOffset().RawValue());
    AddColumn (log.GetLastKnownIOSeq().RawValue());
    AddColumn (log.GetLastKnownOffset().RawValue());
    AddColumn (info.databaseIdentifier);
    AddColumn (info.currentSoftwareVersion);
    AddColumn (info.installationSoftwareVersion);
}

/*===========================================================================*
 *  END OF CODE                                                              *
 *===========================================================================*/
