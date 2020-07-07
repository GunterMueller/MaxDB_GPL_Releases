/* @lastChanged: "1998-05-27  11:02"
 * @filename:    OMS_COMMonitor.hpp
 * @purpose:     "DBProcedure: Monitor"
 * @release:     7.1.0.0
 * @see:         ""
 *
 * @Copyright (c) 1998-2005 SAP AG"



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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


#ifndef __OMS_COMMONITOR_HPP
#define __OMS_COMMONITOR_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include "ggg00.h"
#include "ggg01.h"
#include "Oms/OMS_AFX.h"
#include "Oms/OMS_Defines.h"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"

/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

class OmsHandle;

class OMS_COMMonitorInfo 
{
public:
  void Reset() {
    m_monInfo.cmiCallCnt_gg00 = 0;
    for (int ix = cmiRunTime; ix <= cmiCacheSize; ix++) {
      m_monInfo.cmiCounters_gg00[ix] = (tsp00_8ByteCounter) 0.0;
    }
    m_monInfo.cmiCounters_gg00[cmiRunTimeMin           ] = csp_maxint4;
    m_monInfo.cmiCounters_gg00[cmiMinWaitNewConsView   ] = csp_maxint4;
	  m_monInfo.cmiCounters_gg00[cmiMallocMin            ] = csp_maxint4;
    m_monInfo.cmiCounters_gg00[cmiFreeMin              ] = csp_maxint4;
    m_monInfo.cmiCounters_gg00[cmiDeltaMin             ] = csp_maxint4;
	  m_monInfo.cmiCounters_gg00[cmiMinReadApplCallbacks ] = csp_maxint4;
    m_monInfo.cmiCounters_gg00[cmiMinStreamRowsRead    ] = csp_maxint4;
    m_monInfo.cmiCounters_gg00[cmiMinWriteApplCallbacks] = csp_maxint4;
    m_monInfo.cmiCounters_gg00[cmiMinStreamRowsWrite   ] = csp_maxint4;
  }
  OMS_COMMonitorInfo () {
    Reset();    
  }

  tgg01_COMMonitorInfo m_monInfo;
};

class OMS_COMMonitor {
public:
  class Iterator {
  public :
    Iterator(OMS_COMMonitor*);
    inline int getDispId() const;
    inline void operator++();
    inline operator bool() const;
    inline OMS_COMMonitorInfo& operator()() const;
  private :
    OMS_COMMonitor* m_monitor;
    int           m_idx;
  };
 
  OMS_COMMonitor(CLSID& IID, long numEntries) : m_size(numEntries), m_iid(IID),  m_currInfo(NULL) {
    /*PTS 1103516 FF*/
    m_monInfoArray = new OMS_COMMonitorInfo[numEntries];
    if (NULL == m_monInfoArray ) 
      m_size = 0;
  }

  /*PTS 1103516 FF*/
  ~OMS_COMMonitor() {
    delete [] m_monInfoArray;
  }

  void Reset () {
    for (int ix = 0; ix < m_size; ix++) {
      (m_monInfoArray+ix)->Reset();
    }
  }

  /* PTS 1112275 FF                                                    */
  /* function to handle InfoArray with a dimension, which is too small */
  /* this may occurr when dlopen doesn't support RTLD_LOCAL            */
  void increaseInfoArray( long num )
  {
	  if ( num > size() ) {
		  OMS_COMMonitorInfo*  newInfoArray = new OMS_COMMonitorInfo[num];
		  if ( newInfoArray ) {
        int i;
			  for ( i = 0; i < m_size; i++ )
				  newInfoArray[ i ] = m_monInfoArray[ i ];
			  for ( i = m_size; i < num; i++ )
				  newInfoArray[i].Reset();
			  m_size = num;
			  delete [] m_monInfoArray;
			  m_monInfoArray = newInfoArray;
		  }
	  }
  }

  inline Iterator begin();

  inline void callEpilog(int dispId, int runTime);

  tsp00_Int4 size() {
    return m_size;
  }

  inline tgg01_COMMonitorInfo* getCurrent();

  inline const CLSID& getIID() const;

  inline void setCurrent(int dispId, unsigned char* lpMethodName);

  OMS_COMMonitorInfo& operator[](int idx) {
    return m_monInfoArray[idx];
  }

  // helper methods:
  void omsSetCurrentMonitorMethod(OmsHandle& h);
  static void omsMethodCallEpilogWrapper(OmsHandle& h, void* sql_handle, int runTime);     /* PTS 1120794 */

private:
  OMS_COMMonitorInfo*   m_monInfoArray;   /*PTS 1103516 FF*/
  tsp00_Int4            m_size;
  CLSID                 m_iid;
  tgg01_COMMonitorInfo* m_currInfo;
 
  friend class OMS_COMMonitor::Iterator;
};

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

inline OMS_COMMonitor::Iterator OMS_COMMonitor::begin() {
  Iterator iter(this);
  return iter;
}

inline void OMS_COMMonitor::callEpilog(int dispId, int runTime)
{
  if ( dispId < size() ) {
    if (runTime > m_currInfo->cmiCounters_gg00[cmiRunTimeMax]) {
      m_currInfo->cmiCounters_gg00[cmiRunTimeMax] = runTime;
    }
    if (runTime < m_currInfo->cmiCounters_gg00[cmiRunTimeMin]) {
      m_currInfo->cmiCounters_gg00[cmiRunTimeMin] = runTime;
    }
    m_currInfo->cmiCounters_gg00[cmiRunTime] += runTime;
    m_currInfo = NULL;
  }
}

inline tgg01_COMMonitorInfo* OMS_COMMonitor::getCurrent() {
  return m_currInfo;
}

inline const CLSID& OMS_COMMonitor::getIID() const 
{
  return m_iid;
}

inline  void OMS_COMMonitor::setCurrent(int dispId, unsigned char* lpMethodName) {
  if ( dispId <size() ) {
    m_currInfo = &m_monInfoArray[dispId].m_monInfo;
    if (0 == m_currInfo->cmiCallCnt_gg00) {
      SAPDB_MemCopyNoCheck(&m_currInfo->cmiMethodName_gg00[0], lpMethodName,
        sizeof (m_currInfo->cmiMethodName_gg00));
    }
  }
  else {
    m_currInfo = NULL;
  }
}

inline OMS_COMMonitor::Iterator::Iterator(OMS_COMMonitor* monitor) : m_monitor(monitor), m_idx(-1) 
{
  operator++();
}

inline int OMS_COMMonitor::Iterator::getDispId() const {
  return m_idx;
}

inline void OMS_COMMonitor::Iterator::operator++() 
{
  ++m_idx;
  while ((m_idx < m_monitor->size()) && (0 == m_monitor->m_monInfoArray[m_idx].m_monInfo.cmiCallCnt_gg00)) {
    ++m_idx;
  }
}

inline OMS_COMMonitor::Iterator::operator bool() const
{
  return (m_idx < m_monitor->size());
}

inline OMS_COMMonitorInfo& OMS_COMMonitor::Iterator::operator()() const
{
  return m_monitor->m_monInfoArray[m_idx];
}

#endif  /* __OMS_COMMONITOR_HPP */
