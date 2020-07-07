/*!
  @file        Trans_ExtendedMonitoringContext.cpp
  @author      Ferdi
  @ingroup     Transaction
  @brief       extended monitoring for long running transaction
  @since       2004-09-18

\if EMIT_LICENCE
  copyright:    Copyright (c) 2001-2005 SAP AG


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

#include "Transaction/Trans_ExtendedMonitoringContext.hpp"
#include "RunTime/System/RTESys_Time.h"

///
const int Trans_ExtendedMonitoringContext::ExtMon_CommentLength = 2048;

///
const SAPDB_Char*
Trans_ExtendedMonitoringContext::GetTransactionStartTime(RTESys_SQLTimestamp& timeBuffer)
{
    return RTESys_BuildSQLTimeStamp( this->m_startTimeUTC * 1000 * 1000, timeBuffer);
};

///
const SAPDB_Char*
Trans_ExtendedMonitoringContext::GetTransactionEndTime(RTESys_SQLTimestamp& timeBuffer)
{
    memset(&timeBuffer[0], ' ', sizeof(RTESys_SQLTimestamp));
    if (this->m_inUse != SAPDB_TRUE)
        RTESys_BuildSQLTimeStamp( this->m_endTimeUTC * 1000 * 1000, timeBuffer);
    return reinterpret_cast<SAPDB_Char*>(&timeBuffer[0]);
};




