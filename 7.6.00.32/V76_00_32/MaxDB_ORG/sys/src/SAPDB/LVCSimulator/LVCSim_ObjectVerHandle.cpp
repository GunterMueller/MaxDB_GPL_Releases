/*!***************************************************************************

  module      : LVCSim_ObjectVerHandle.cpp

  -------------------------------------------------------------------------

  responsible : IvanS

  special area: liveCache Simulator
  description : Internal object version handle 

  -------------------------------------------------------------------------

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

#include "LVCSimulator/LVCSim_ObjectVerHandle.hpp"
#include "LVCSimulator/LVCSim_OIDAllocator.hpp"

/*===========================================================================*
 *  METHODS                                                                  *
 *===========================================================================*/

LVCSim_ObjectVerHandle::~LVCSim_ObjectVerHandle()
{
	delete m_next;
}

void *LVCSim_ObjectVerHandle::operator new(size_t sz, size_t datasize)
{
	// TODO: use better allocator
	return new char[sz + datasize - 1 - sizeof(size_t)];
}

void LVCSim_ObjectVerHandle::operator delete(void *ptr)
{
	delete[] (char*) ptr;
}

void LVCSim_ObjectVerHandle::operator delete(void *ptr, size_t sz)
{
	delete[] (char*) ptr;
}

LVCSim_ObjectVerHandle::LVCSim_ObjectVerHandle(LVCSim_CheckpointReader &i, size_t objsize)
{
	i.read(&m_tid_min, sizeof(m_tid_min));
	if (objsize) {
		i.read(&m_data.fix[0], objsize);
	} else {
		i.read(&m_data.var.size, sizeof(m_data.var.size));
		i.read(&m_data.var.data[0], m_data.var.size);
	}

	size_t nextsize;
	i.read(&nextsize, sizeof(nextsize));
	if (nextsize) {
		m_next = new(nextsize) LVCSim_ObjectVerHandle(i, objsize);
	} else {
		m_next = NULL;
	}
}

void LVCSim_ObjectVerHandle::writeToStream(LVCSim_CheckpointWriter &o, size_t objsize) const
{
	size_t rsize;
	if (objsize) rsize = objsize;
	else rsize = (m_data.var.size + sizeof(size_t));
	o.write(&rsize, sizeof(rsize));

	o.write(&m_tid_min, sizeof(m_tid_min));
	if (objsize) {
		o.write(&m_data.fix[0], objsize);
	} else {
		o.write(&m_data.var.size, sizeof(m_data.var.size));
		o.write(&m_data.var.data[0], m_data.var.size);
	}

	if (m_next) m_next->writeToStream(o, objsize);
	else {
		objsize = 0;
		o.write(&objsize, sizeof(objsize));
	}
}
