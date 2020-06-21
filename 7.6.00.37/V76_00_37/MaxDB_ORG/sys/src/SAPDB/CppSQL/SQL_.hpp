/*!
 * \file    SQL_.hpp
 * \author  PeterG (GoertzP)
 * \brief   C++ SQL Class: SQL
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


#ifndef SQL_HPP
#define SQL_HPP


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef __OMS_LiveCacheTypes_h__
#include "livecachetypes.h" /* no check*/
#endif


/************************************************************************/
/* Forward declarations                                                 */
/************************************************************************/

class SqlCol;
class SQL_Statement;
class SqlHandle;


/************************************************************************/
/* Class SQL                                                            */
/************************************************************************/

class AFX_EXT_CLASS SQL {
# if defined (SQLCLS_IMPL)
  friend class SqlHandle;
  friend class SQL_SessionContext;
# endif
private:
# if defined (SQLCLS_IMPL)
  SQL( SqlHandle * handle, const char * stmt );
  SQL( SqlHandle * handle, const DbpTypeUnicode * stmt );
# endif
  SQL_Statement * m_sqlstatement;
public:
# if defined (SQLCLS_IMPL)
# if defined (EXT_CLIENT_SQL)
  int connect( const char* hostname, const char* dbname, 
               const char* user, const char* pwd, const char* isolevel);
# endif
# endif
  SQL();
  ~SQL();
  SQL(const SQL& sql);
  short      sqlNumCols() const; // returns number of input and output parameters
  short      sqlNumOutCols() const; // returns number of output parameters
  int        sqlExecute(); // executes a statement
  int        sqlExecuteMass(); // PTS 1119960, executes a mass statement
  int        sqlPrepare(); // prepares a statement and returns the number
  int        operator() (); // like execute ()
& if $MACH IN [ SUN ] || $OSSPEC == SOLARIS
  SQL &      operator<< (const SqlCol p); // assignes a column
& else
  SQL &      operator<< (const SqlCol & p); // assignes a column
& endif
  SqlCol&    operator[] (int i); // select a column by index
  const SQL& operator= (const SQL& right);
  void*      operator new (size_t sz);
  void       operator delete (void* p);
  void*      operator new[] (size_t sz);
  void       operator delete[] (void* p);
};


#endif /* SQL_HPP */

