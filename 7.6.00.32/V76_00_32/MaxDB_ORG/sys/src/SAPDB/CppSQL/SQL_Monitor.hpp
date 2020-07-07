/*!
 * \file    SQL_Monitor.hpp
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


#ifndef SQL_MONITOR_HPP
#define SQL_MONITOR_HPP


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "gsp92.h"


/************************************************************************/
/* Forward declarations                                                 */
/************************************************************************/

struct SqlMonitorData;


/************************************************************************/
/* Class SQL_Monitor                                                    */
/************************************************************************/

/*! 
  Class: SQL_Monitor

  Description: counts SQL events
 */

class SQL_Monitor
{
public : 
  SQL_Monitor();

/*!----------------------------------------------------------------------
   function: GetCallStatistics
   description:  transfers the statistics of the last db-procedure call into
                 a SqlMonitorData structure
   arguments:    
     info[in] reference to the SqlMonitorData structure
   return value: none
   ---------------------------------------------------------------------*/
  void GetCallStatistics(SqlMonitorData& info) const;

/*!----------------------------------------------------------------------
   function: Reset
   description:  sets all counters to their initial state
   arguments: none    
   return value: none
  ----------------------------------------------------------------------*/
  inline void Reset();

  inline void SetRuntime(SAPDB_UInt4 Runtime);
  inline void IncPrepare();
  inline void IncInsert();
  inline void IncSelect();
  inline void IncUpdate();
  inline void IncDelete();
  inline void IncFetch();
  inline void IncDropParseid();
  inline void IncOther();
  inline void AddInsertDataLen(SAPDB_UInt4 DataLen);
  inline void AddUpdateDataLen(SAPDB_UInt4 DataLen);
  inline void AddDeleteDataLen(SAPDB_UInt4 DataLen);
  inline void AddFetchDataLen(SAPDB_UInt4 DataLen);
  
private :
  SAPDB_UInt4 m_Runtime;
  SAPDB_UInt4 m_cntPrepare;
  SAPDB_UInt4 m_cntInsert;
  SAPDB_UInt4 m_cntSelect;
  SAPDB_UInt4 m_cntUpdate;
  SAPDB_UInt4 m_cntDelete;
  SAPDB_UInt4 m_cntFetch;
  SAPDB_UInt4 m_cntDropParseid;
  SAPDB_UInt4 m_cntOther;
  SAPDB_UInt4 m_lenInsertData;
  SAPDB_UInt4 m_lenUpdateData;
  SAPDB_UInt4 m_lenDeleteData;
  SAPDB_UInt4 m_lenFetchData;
};

/*! endclass: SQL_Monitor */

/*----------------------------------------------------------------------*/

inline void SQL_Monitor::Reset() {
  memset( this, 0, sizeof(*this) );
}

/*----------------------------------------------------------------------*/

inline void SQL_Monitor::SetRuntime(SAPDB_UInt4 Runtime) {
  m_Runtime = Runtime;
}

/*----------------------------------------------------------------------*/

inline void SQL_Monitor::IncPrepare() {
  ++m_cntPrepare;
}

/*----------------------------------------------------------------------*/

inline void SQL_Monitor::IncInsert() {
  ++m_cntInsert;
}

/*----------------------------------------------------------------------*/

inline void SQL_Monitor::IncSelect() {
  ++m_cntSelect;
}

/*----------------------------------------------------------------------*/

inline void SQL_Monitor::IncUpdate() {
  ++m_cntUpdate;
}

/*----------------------------------------------------------------------*/

inline void SQL_Monitor::IncDelete() {
  ++m_cntDelete;
}

/*----------------------------------------------------------------------*/

inline void SQL_Monitor::IncFetch() {
  ++m_cntFetch;
}

/*----------------------------------------------------------------------*/

inline void SQL_Monitor::IncDropParseid() {
  ++m_cntDropParseid;
}

/*----------------------------------------------------------------------*/

inline void SQL_Monitor::IncOther() {
  ++m_cntOther;
}

/*----------------------------------------------------------------------*/

inline void SQL_Monitor::AddInsertDataLen(SAPDB_UInt4 DataLen) {
  m_lenInsertData += DataLen;
}

/*----------------------------------------------------------------------*/

inline void SQL_Monitor::AddUpdateDataLen(SAPDB_UInt4 DataLen) {
  m_lenUpdateData += DataLen;
}

/*----------------------------------------------------------------------*/

inline void SQL_Monitor::AddDeleteDataLen(SAPDB_UInt4 DataLen) {
  m_lenDeleteData += DataLen;
}

/*----------------------------------------------------------------------*/

inline void SQL_Monitor::AddFetchDataLen(SAPDB_UInt4 DataLen) {
  m_lenFetchData += DataLen;
}


#endif /* SQL_MONITOR_HPP */

