/*!================================================================
module:        OMS_MonitorDirectory.hpp

responsible:   ThomasA

special area:  OMS

description:   Oms Monitor   

see:

Copyright (c) 1998-2005 SAP AG 



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




===================================================================*/

#ifndef __OMS_MONITORDIRECTORY_HPP
#define __OMS_MONITORDIRECTORY_HPP

#include "Oms/OMS_GlobalAllocatorWrapper.hpp"
#include "Oms/OMS_COMMonitor.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "ggg250.h"
#include "ggg01.h"

class OMS_CompareMonitorDirectoryKey;
class OMS_MonitorDirectory;

/*!
class :  LVC_MonitorDirectoryKey

description: defines the key of a monitoring info of a method of an instance
of the class identified by its iid. the Method is identified by
its corresponding dispid.
*/
class LVC_MonitorDirectoryKey {
public :
	/*
	Function:     
	description:  LVC_MonitorDirectoryKey
	constructor, initializes the key.

	arguments:
	iid   [in]  : the CLSID (GUID) of the com object.
	dispid[in]  : the index of the method in the vtable
	return value: none
	*/
	LVC_MonitorDirectoryKey(const CLSID& iid, short dispId) : m_iid(iid), m_dispid(dispId) {}
	/*
	Function: GetIid     
	description:  yields the iid.

	arguments: none
	return value: iid
	*/
	const CLSID& GetIid()    const { return m_iid; }
	/*
	Function: GetDispId     
	description:  yields the dispid.

	arguments: none
	return value: dispid 
	*/
	short        GetDispId() const { return m_dispid; }
private :
	friend class OMS_CompareMonitorDirectoryKey;
	CLSID m_iid;
	short m_dispid;
};
/*! endclass: LVC_MonitorDirectoryKey */


/*!
class :  OMS_CompareMonitorDirectoryKey

description: defines comparism of monitor keys
*/
class OMS_CompareMonitorDirectoryKey {
public :
	int Compare(const LVC_MonitorDirectoryKey& k1, const LVC_MonitorDirectoryKey& k2);
};
/*! endclass: OMS_CompareMonitorDirectoryKey */

/*!
class :  OMS_MonitorDirectory

description: implements the monitor dictionary. This dictionary holds monitor data of
com instances, that have already been destructed.
The data is stored as rows of an AVL-Tree using a key structure defined by
LVC_MonitorDirectoryKey. 
*/
class OMS_MonitorDirectory {
public :
	class Iterator : public cgg250AvlInfoTree<LVC_MonitorDirectoryKey,tgg01_COMMonitorInfo,OMS_CompareMonitorDirectoryKey,OMS_GlobalAllocatorWrapper>::Iterator
	{
	public :
		inline void Get(CLSID& iid, tsp00_Int4& dispid, tgg01_COMMonitorInfo& monInfo);
	};
	/*
	Function:      OMS_MonitorDirectory
	description:   constructor

	arguments  :  pointer to global allocator
	*/
	OMS_MonitorDirectory(OMS_GlobalAllocatorWrapper *alloc);

	/*
	Function:     Insert
	description:  Inserts the monitor data into the monitor dictionary

	arguments  :  monitor, holds the monitor data of a com instance
	*/
	void   Insert(OMS_COMMonitor* monitor);
	/*
	Function     : begin
	description  : returns an iterator for the monitor dictionary

	arguments    : none
	return value : iterator
	*/
	inline Iterator begin(); 
	/*
	Function     : Reset
	description  : clears the monitor dictionary

	arguments    : none
	*/
	void   Reset();
private :
	void Insert(const CLSID& iid, short dispId, tgg01_COMMonitorInfo& info);
	cgg250AvlInfoTree<LVC_MonitorDirectoryKey,tgg01_COMMonitorInfo,OMS_CompareMonitorDirectoryKey,OMS_GlobalAllocatorWrapper> m_monitorDir;
};
/*! endclass: OMS_MonitorDirectory */

inline OMS_MonitorDirectory::Iterator  OMS_MonitorDirectory::begin() 
{
	cgg250AvlInfoTree<LVC_MonitorDirectoryKey,tgg01_COMMonitorInfo,OMS_CompareMonitorDirectoryKey,OMS_GlobalAllocatorWrapper>::Iterator
		iter = m_monitorDir.First(); 
	return *REINTERPRET_CAST(Iterator*, &iter);
}

inline void OMS_MonitorDirectory::Iterator::Get(CLSID& iid, tsp00_Int4& dispid, tgg01_COMMonitorInfo& monInfo) {
	iid    = this->operator()()->GetIid();
	dispid = this->operator()()->GetDispId();
	SAPDB_MemCopyNoCheck(&monInfo, this->GetInfo(), sizeof(monInfo));
}

#endif	// __OMS_MONITORDIRECTORY_HPP
