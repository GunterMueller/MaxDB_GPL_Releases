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
#include "livecachesql.h" // nocheck

class CVersionIterator {
public :
  CVersionIterator(SqlHandle*);
  operator bool () const { return (0 == m_sqlCode); }
  OmsVersionId& operator()() { return m_version; }
  void operator++ ();
  int IterCnt() { return m_resCnt; }
  void GetVersionId(OmsVersionId& v) { memcpy(&v[0], &m_version[0], sizeof(v)); }
private :
  SqlHandle*   m_handle;
  long         m_sqlCode;
  int          m_resCnt;
  SQL          m_fetch;
  OmsVersionId m_version;
  char         m_nullTerminator;
  long sqlCode () const { return m_handle->sqlCode(); }
  bool sqlOk() const    { return (0 == sqlCode()); }
};


#define STR_IN(P)  SqlCol(P, SqlCol::VSTRING, strlen(P))
#define STR_OUT(P) SqlCol(&P[0], SqlCol::VSTRING, sizeof(P))



