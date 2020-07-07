/*!
  @file        Trans_ExtendedMonitoringIter.cpp
  @author      FerdiF
  @ingroup     Transaction
  @brief       implementation of class Trans_ExtendedMonitoringIter
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
#include "Transaction/Trans_ExtendedMonitoringIter.hpp"

///
const SAPDB_UInt4 Trans_ExtendedMonitoringIter::cInvalidPos = 4294967295;

/// returns the next available global transentry if false is returned an error occured
Trans_ExtendedMonitoringIter::Trans_ExtendedMonitoringIter(tsp00_TaskId TaskId) 
    : m_next(cInvalidPos)
    , m_taskId(TaskId)
    , m_firstDone(false)
{
}

/// 
bool Trans_ExtendedMonitoringIter::Next()
{
    if (m_firstDone)
    {
        Trans_ExtendedMonitoring::GetNextHistoryContext(m_taskId, m_ctx, m_next);
    }
    else
    {
        Trans_ExtendedMonitoring::GetFirstHistoryContext(m_taskId, m_ctx, m_next);
        m_firstDone = true;
    }
    return IsValid();    
}

///
Trans_ExtendedMonitoringContext* 
Trans_ExtendedMonitoringIter::GetMonitoringContext() 
{
    return static_cast<Trans_ExtendedMonitoringContext*>(&m_ctx);
};



