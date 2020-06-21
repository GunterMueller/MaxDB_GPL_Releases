/*!
  @file           Catalog_SessionCacheInline.hpp
  @author         ThomasA
  @brief          Catalog Session Cache inline methods
  @ingroup        Catalog

\if EMIT_LICENCE

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



\endif
*/

#ifndef CATALOG_SESSIONCACHEINLINE_HPP 
#define CATALOG_SESSIONCACHEINLINE_HPP 

#include "SQLManager/Catalog/Catalog_SessionCache.hpp"
#include "SQLManager/SQLMan_Context.hpp"

#include "hak10.h"

inline Catalog_SessionCache::LruIterator::operator bool() const
{
    return (m_curr != m_context.a_sysdir.ci_cache_dir_trailor.cdir_ptr());
}

//------------------------------------------------------------------------------------

inline void Catalog_SessionCache::LruIterator::operator++()
{
    m_curr = m_next;
	m_next = m_curr->cd_lru_next().cdir_ptr();
}

//------------------------------------------------------------------------------------

inline tak_cache_dir_ptr Catalog_SessionCache::LruIterator::operator()() const
{
    return m_curr;
}

//------------------------------------------------------------------------------------

inline Catalog_Object* Catalog_SessionCache::LruIterator::GetCatalogObject() const
{
    return REINTERPRET_CAST(Catalog_Object*, &m_curr->cd_sysinfo());
}

//------------------------------------------------------------------------------------

inline const Catalog_Object* Catalog_SessionCache::GetObject(const void* p)
{
	if (REINTERPRET_CAST(const Catalog_Object*, p)->b_kl() != 0)
	{
		// pointer already a key or catalog object
		return REINTERPRET_CAST(const Catalog_Object*, p);
	}
	else
	{   // pointer to a frame containing a catalog record 
		return REINTERPRET_CAST(const Catalog_Object*, REINTERPRET_CAST(const char*, p) + ObjectHeader);
	}
}

#endif