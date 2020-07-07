/*!***************************************************************************

module:       OMS_MonitorDirectory.cpp

------------------------------------------------------------------------------

responsible:  ThomasA

special area: OMS

description:  OMS Monitor

see also:     

------------------------------------------------------------------------------

copyright:            Copyright (c) 2000-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#include "Oms/OMS_MonitorDirectory.hpp"
#include "Oms/OMS_Globals.hpp"
#include "geo573.h"
#include "Oms/OMS_DbpError.hpp"

/*===========================================================================*
*  GLOBAL VARIABLES                                                         *
*===========================================================================*/

static OMS_CompareMonitorDirectoryKey glob_MonitorKeyComparator;

/*---------------------------------------------------------------------------*/

int OMS_CompareMonitorDirectoryKey::Compare(const LVC_MonitorDirectoryKey& k1, const LVC_MonitorDirectoryKey& k2) {
	int cmp = memcmp(&k1.m_iid, &k2.m_iid, sizeof(k1.m_iid));
	if (0 != cmp) {
		if (cmp < 0) {
			return -1;
		}
		else {
			return 1;
		}
	}
	else {
		if (k1.m_dispid == k2.m_dispid) {
			return 0;
		}
		else {
			if (k1.m_dispid < k2.m_dispid) {
				return -1;
			}
			else {
				return 1;
			}
		}
	}
}

/*---------------------------------------------------------------------------*/

OMS_MonitorDirectory::OMS_MonitorDirectory(OMS_GlobalAllocatorWrapper *alloc) : m_monitorDir()
{  
  m_monitorDir.AdviseAllocator(alloc);
  m_monitorDir.AdviseCompare(&glob_MonitorKeyComparator);
}

/*---------------------------------------------------------------------------*/

void OMS_MonitorDirectory::Insert(const CLSID& iid, short dispId, tgg01_COMMonitorInfo& info)
{
	LVC_MonitorDirectoryKey monKey(iid, dispId);
	tgg01_COMMonitorInfo* found = m_monitorDir.Find(monKey);
	if (NULL != found) {
		found->cmiCallCnt_gg00+= info.cmiCallCnt_gg00;
		for (int monEnum = cmiRunTime; monEnum <= cmiCacheSize; ++monEnum) {
			switch (monEnum) {
      case cmiRunTimeMin :
      case cmiMinWaitNewConsView :
      case cmiMallocMin :
      case cmiFreeMin :
      case cmiDeltaMin :
      case cmiMinReadApplCallbacks :
      case cmiMinStreamRowsRead :
      case cmiMinWriteApplCallbacks :
      case cmiMinStreamRowsWrite :
	      if (info.cmiCounters_gg00[monEnum] < found->cmiCounters_gg00[monEnum]) {
		      found->cmiCounters_gg00[monEnum] = info.cmiCounters_gg00[monEnum];
	      }
	      break;
      case cmiRunTimeMax :
      case cmiMaxSubtransLevel :
      case cmiMaxWaitNewConsView :
      case cmiCacheSize :
      case cmiMallocMax :
      case cmiFreeMax :
      case cmiDeltaMax :
      case cmiMaxStreamRowsRead :
      case cmiMaxReadApplCallbacks :
      case cmiMaxWriteApplCallbacks :
      case cmiMaxStreamRowsWrite :
      case cmiOmsMaxHashChainLen :
	      if (info.cmiCounters_gg00[monEnum] > found->cmiCounters_gg00[monEnum]) {
		      found->cmiCounters_gg00[monEnum] = info.cmiCounters_gg00[monEnum];
	      }
	      break;
      default :
	      {
		      found->cmiCounters_gg00[monEnum] += info.cmiCounters_gg00[monEnum];
	      }
			}
		}
	}
	else {
		tgg00_BasisError rc;
		try
		{
			found = m_monitorDir.Insert(monKey, rc);
			if (0 == rc) {
				SAPDB_MemCopyNoCheck(found, &info, sizeof(info));
			}
		}
		catch (BAD_ALLOC_GEO573&)
		{
			// if no memory is available, monitoring info cannot be saved
			// ignore it !

      DbpErrorCallbackInterface *pCBInterface = DbpError::dbpGetCallback();  // PTS 1127871
      if (pCBInterface){
        pCBInterface->dbpCaughtBadAlloc();
      }
		}
	}
}

/*---------------------------------------------------------------------------*/

void OMS_MonitorDirectory::Insert(OMS_COMMonitor* monitor) /* PTS 1107731 */
{
	for (OMS_COMMonitor::Iterator iter = monitor->begin(); iter; ++iter) {
		this->Insert(monitor->getIID(), iter.getDispId(), iter().m_monInfo);
	}
}

/*---------------------------------------------------------------------------*/

void OMS_MonitorDirectory::Reset()
{
	m_monitorDir.DeleteAll();
}

/*---------------------------------------------------------------------------*/

