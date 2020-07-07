/*!
 * \file    SQL_Session.hpp
 * \author  PeterG (GoertzP)
 * \brief   C++ SQL Class: Session
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


#ifndef SQL_SESSION_HPP
#define SQL_SESSION_HPP


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef SQL_HANDLE_HPP
#include "CppSQL/SQL_Handle.hpp"
#endif


/************************************************************************/
/* Class SqlSession                                                     */
/************************************************************************/

class AFX_EXT_CLASS SqlSession : public SqlHandle
{
public:
  typedef enum { ISO_LEVEL_0, ISO_LEVEL_1, ISO_LEVEL_15,
                 ISO_LEVEL_2, ISO_LEVEL_3
               } IsoLevel;
  SqlSession();
  SqlSession( const char * user, const char * pwd,
              const char * dbname, const char * host = "",
              const IsoLevel = ISO_LEVEL_1,
              const char * version = LIVECACHESQL_VERSION );
  ~SqlSession();
  bool connect( const char * user, const char * pwd,
                const char * dbname, const char * host = "",
                const IsoLevel = ISO_LEVEL_1,
                const char * version = LIVECACHESQL_VERSION );
};


#endif /* SQL_SESSION_HPP */

