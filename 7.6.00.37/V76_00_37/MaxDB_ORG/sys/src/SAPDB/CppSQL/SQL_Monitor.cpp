/*!
 * \file    SQL_Monitor.cpp
 * \author  PeterG (GoertzP)
 * \brief   C++ SQL Class: Monitor
 */

/*

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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



*/


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef SQL_MONITOR_HPP
#include "CppSQL/SQL_Monitor.hpp"
#endif

#include "livecachetypes.h" // nocheck


/************************************************************************/
/* Implementation of class SQL_Monitor                                  */
/************************************************************************/

SQL_Monitor::SQL_Monitor() 
: m_Runtime(0)
, m_cntPrepare(0)
, m_cntInsert(0)
, m_cntSelect(0)
, m_cntUpdate(0)
, m_cntDelete(0)
, m_cntFetch(0)
, m_cntDropParseid(0)
, m_cntOther(0)
, m_lenInsertData(0)
, m_lenUpdateData(0)
, m_lenDeleteData(0)
, m_lenFetchData(0)
{
  this->Reset();
}

/*----------------------------------------------------------------------*/

void SQL_Monitor::GetCallStatistics(SqlMonitorData & info) const {
  info.m_Runtime        = m_Runtime;
  info.m_cntPrepare     = m_cntPrepare;
  info.m_cntInsert      = m_cntInsert;
  info.m_cntSelect      = m_cntSelect;
  info.m_cntUpdate      = m_cntUpdate;
  info.m_cntDelete      = m_cntDelete;
  info.m_cntFetch       = m_cntFetch;
  info.m_cntDropParseid = m_cntDropParseid;
  info.m_cntOther       = m_cntOther;
  info.m_lenInsertData  = m_lenInsertData;
  info.m_lenUpdateData  = m_lenUpdateData;
  info.m_lenDeleteData  = m_lenDeleteData;
  info.m_lenFetchData   = m_lenFetchData;
} 

