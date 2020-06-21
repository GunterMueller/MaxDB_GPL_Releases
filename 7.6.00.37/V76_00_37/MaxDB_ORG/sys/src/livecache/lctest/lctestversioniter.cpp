/*



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





*/
#if defined(IMF)
#include "livecache/lctest/lctestversioniter.h"
#include "livecache/lctest/lctesterrors.h"
#else
#include "lctestversioniter.h" // nocheck
#include "lctesterrors.h" // nocheck
#endif
CVersionIterator::CVersionIterator(SqlHandle* handle) 
: m_handle(handle)
, m_sqlCode(0)
, m_resCnt(0)
, m_nullTerminator(0)
{
  SQL sel = m_handle->sql("select oms_version_id from oms_versions order by create_date, create_time");
  sel.sqlExecute();
  m_sqlCode = sqlCode();
  m_resCnt  = m_handle->sqlResultCount();
  if (sqlOk()) {
    m_fetch = m_handle->sql("fetch into ?");
    m_fetch << SqlCol(&m_version[0], SqlCol::VCHAR, sizeof(m_version));
    m_fetch.sqlExecute();
  }
  else {
    if (100 != sqlCode()) {
      ThrowError (e_SQL_ERROR, sqlCode(), 0);
    }
  }
};

void CVersionIterator::operator++() {
  m_fetch.sqlExecute();
  m_sqlCode = sqlCode();
  if (!sqlOk()) {
    if (100 != m_sqlCode) {
      ThrowError (e_SQL_ERROR, sqlCode(), 0);
    }
  }
}