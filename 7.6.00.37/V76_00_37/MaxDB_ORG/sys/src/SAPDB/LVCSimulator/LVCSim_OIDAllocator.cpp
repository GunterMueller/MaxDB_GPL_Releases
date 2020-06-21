/*!***************************************************************************

  module      : LVCSim_OIDAllocator.cpp

  -------------------------------------------------------------------------

  responsible : IvanS

  special area: liveCache Simulator
  description : allocator for memory and OIDs

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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


#include "LVCSimulator/LVCSim_Internal.hpp"
#include "LVCSimulator/LVCSim_OIDAllocator.hpp"
#include "LVCSimulator/LVCSim_ObjectHandle.hpp"


#define	ALIGN				sizeof(int)
#define	OBJ_SIZE_BITS		(MAX_OBJ_SIZE_BITS - MIN_OBJ_SIZE_BITS)

#define	BYTES_PER_RANGE		(64*1024)	// allocate 64kB blocks


void LVCSim_OIDObject::getCurrentOid(OMS_ObjectId8 *lpoid) const
{
	lpoid->m_oid.OidAsStruct.pno        = m_oid.OidAsStruct.m_pagenr;
	lpoid->m_oid.OidAsStruct.pagePos    = m_oid.OidAsStruct.m_pagepos;
	lpoid->m_oid.OidAsStruct.generation = m_oid.OidAsStruct.m_generation;
}

void LVCSim_OIDObject::getRef(tgg91_PageRef *lpref) const
{
	lpref->gg91BuildRef(m_oid.OidAsStruct.m_pagenr, m_oid.OidAsStruct.m_pagepos);
}


LVCSim_OIDAllocator::LVCSim_OIDAllocator(unsigned int idx, size_t size)
	: m_idx(idx), m_size(((size + ALIGN - 1) / ALIGN) * ALIGN), m_freeptr(-1),
	  m_endptr(0), m_mapcnt(0)
{
	m_objpage = BYTES_PER_RANGE / m_size;
}

LVCSim_OIDAllocator::~LVCSim_OIDAllocator()
{
	// delete all page maps
	// TODO: use opposite to allocate map
    for (size_t i = 0; i < m_mapcnt; ++i) {
        delete[] m_maps[i];
    }
}



void *LVCSim_OIDAllocator::allocate()
{
	RTESync_LockedScope l(m_lock);

	if (m_freeptr >= 0) {
		// reuse address
		LVCSim_OIDObject *addr = (LVCSim_OIDObject*)
			(m_maps[m_freeptr / m_objpage] + (m_freeptr % m_objpage) * m_size);
		int newfreeptr = *((int*) addr);
		addr->setOid((unsigned int) m_idx, m_freeptr + 1, addr->getGeneration());
		memset(addr + 1, 0, m_size - sizeof(LVCSim_OIDObject));
		m_freeptr = newfreeptr;
		return addr;
	}

	// must create new object
	if (m_endptr == m_mapcnt * m_objpage) {
		// no space in last block, get new block
		if (m_mapcnt == LVCSIM_ALLOC_MAX_MAP) {
			// TODO: throw exception?
			return NULL;
		}
		// TODO: do not use new, do mmap()
		m_maps[m_mapcnt++] = new char[BYTES_PER_RANGE];
	}

	LVCSim_OIDObject *addr = (LVCSim_OIDObject*)
		(m_maps[m_mapcnt-1] + (m_endptr++ % m_objpage) * m_size);
	addr->setOid((unsigned int) m_idx, (unsigned int) m_endptr, 1);
	memset(addr + 1, 0, m_size - sizeof(LVCSim_OIDObject));
	return addr;
}

void LVCSim_OIDAllocator::deallocate(unsigned int oid)
{
	--oid;
	size_t page = oid / m_objpage;
    LVCSIM_ASSERT((page < m_mapcnt));

	RTESync_LockedScope l(m_lock);

	LVCSim_OIDObject *addr = (LVCSim_OIDObject*)
		(m_maps[page] + (oid % m_objpage) * m_size);
	LVCSIM_ASSERT(addr->isValid());
	addr->nextGeneration();
	*((int*) addr) = m_freeptr;
	m_freeptr = oid;
}

void *LVCSim_OIDAllocator::map(unsigned int oid) const
{
	--oid;
	size_t page = oid / m_objpage;
	if (page >= m_mapcnt) return NULL;
	return (m_maps[page] + (oid % m_objpage) * m_size);
}

unsigned int LVCSim_OIDAllocator::getNextOid(unsigned int oid) const
{
	// since external OIDs are internal + 1, it's already incremented
	size_t page = oid / m_objpage;
	if (page >= m_mapcnt) return 0;

	LVCSim_OIDObject *addr = (LVCSim_OIDObject*)
		(m_maps[page] + (oid % m_objpage) * m_size);

	while (oid < m_endptr) {
		++oid;
		if (addr->isValid()) {
			// OK, this is not deleted, return it
			return oid;
		}
		if (oid % m_objpage) {
			// same page - seek to next object
			addr = (LVCSim_OIDObject*) (((char*)addr) + m_size);
		} else {
			// new page
			addr = (LVCSim_OIDObject*) m_maps[++page];
		}
	}
	return 0;	// no more objects
}


LVCSim_OIDAllocator::LVCSim_OIDAllocator(LVCSim_CheckpointReader &inp, size_t objsize)
{
	inp.read(this, ((char*) &m_maps) - ((char*) this));

	for (unsigned int i = 0; i < m_mapcnt; ++i)
	{
		m_maps[i] = new char[BYTES_PER_RANGE];
	}

	// read objects
	unsigned int oid;
	inp.read(&oid, sizeof(oid));
	while (oid) {
		// read object
		new(this, oid) LVCSim_ObjectHandle(inp, objsize);
		inp.read(&oid, sizeof(oid));
	}

	// read & reconnect freelist
	inp.read(&m_freeptr, sizeof(m_freeptr));
	int fptr = m_freeptr;
	while (fptr != -1)
	{
		int ofptr = fptr;
		inp.read(&fptr, sizeof(fptr));
		LVCSim_ObjectHandle *h = reinterpret_cast<LVCSim_ObjectHandle*>
			(m_maps[ofptr / m_objpage] + (ofptr % m_objpage) * m_size);
		int *addr = reinterpret_cast<int*>(h);
		h->deactivateObject();
		*addr = fptr;
	}
}

void LVCSim_OIDAllocator::writeToStream(LVCSim_CheckpointWriter &o, size_t objsize) const
{
	o.write(this, ((char*) &m_maps) - ((char*) this));

	// dump objects as such
	unsigned int oid = 0;
	while (oid = getNextOid(oid)) {
		o.write(&oid, sizeof(oid));
		// dump an object
		const LVCSim_ObjectHandle *obj = (const LVCSim_ObjectHandle *) map(oid);
		obj->writeToStream(o, objsize);
	}
	o.write(&oid, sizeof(oid));

	// dump freelist
	int fptr = m_freeptr;
	while (fptr != -1) 
	{
		o.write(&fptr, sizeof(fptr));
		int *addr = (int*) (m_maps[fptr / m_objpage] + (fptr % m_objpage) * m_size);
		fptr = *addr;
	}
	o.write(&fptr, sizeof(fptr));
}

