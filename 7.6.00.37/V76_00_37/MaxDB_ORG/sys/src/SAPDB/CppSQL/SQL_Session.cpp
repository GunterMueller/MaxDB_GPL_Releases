/*!
 * \file    SQL_Session.cpp
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


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "heo02.h"
#include "heo03.h"
#include "hsp02.h"
#include "Oms/OMS_Defines.h"

#ifndef SQL_SESSION_HPP
#include "CppSQL/SQL_Session.hpp"
#endif

#ifndef SQL_HPP
#include "CppSQL/SQL_.hpp"
#endif

#ifndef SQL_SESSIONCONTEXT_HPP
#include "CppSQL/SQL_SessionContext.hpp"
#endif


/************************************************************************/
/* Implementation of class SqlSession                                   */
/************************************************************************/

SqlSession::SqlSession() {
  tsp00_CompName compname;
  compname.SetBlank();
  sqlinit( compname, NULL );
}

/*----------------------------------------------------------------------*/

SqlSession::SqlSession( const char *   user,
                        const char *   pwd,
                        const char *   dbname,
                        const char *   host,
                        const IsoLevel isolevel,
                        const char *   version ) {
  tsp00_CompName compname;
  compname.SetBlank();
  sqlinit( compname, NULL );
  this->connect( user, pwd, dbname, host, isolevel, version );
}

/*----------------------------------------------------------------------*/

SqlSession::~SqlSession() {
  this->sqlSetHandler(NULL);
  this->sql("ROLLBACK WORK RELEASE").sqlExecute();
  sqlfinish(false);
}

/*----------------------------------------------------------------------*/

bool SqlSession::connect( const char *   user,
                          const char *   pwd,
                          const char *   dbname,
                          const char *   host,
                          const IsoLevel isolevel,
                          const char *   version) {
  int    rc = 0;
  char   cIsoLvl[3];
  char * pcIsoLvl = &cIsoLvl[0];

  if (0 == strcmp(version,LIVECACHESQL_VERSION)) {
    switch (isolevel) {
      case ISO_LEVEL_0 :
        strcpy(pcIsoLvl,"0");
        break;
      case ISO_LEVEL_1 :
        strcpy(pcIsoLvl,"1");
        break;
      case ISO_LEVEL_15 :
        strcpy(pcIsoLvl,"15");
        break;
      case ISO_LEVEL_2 :
        strcpy(pcIsoLvl,"2");
        break;
      case ISO_LEVEL_3 :
        strcpy(pcIsoLvl,"3");
        break;
    }
    this->sql((char*)NULL).connect( host, dbname, user, pwd, pcIsoLvl );
  }
  else {
    STATIC_CAST(SqlHandle*,this)->m_session_context->setError( -8048, "Incompatible Version" );
  }
  return (this->sqlCode() == 0);
}

