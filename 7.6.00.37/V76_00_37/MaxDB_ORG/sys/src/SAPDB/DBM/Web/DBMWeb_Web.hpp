/*! 
  -----------------------------------------------------------------------------
  module: DBMWeb_Web.h
  -----------------------------------------------------------------------------
  responsible:  BerndV
  special area: Database Manager WebAgent Extension
  description:  Common declarations for Database Manager WebAgent Extension
  version:      7.3 and higher 
  -----------------------------------------------------------------------------
                          Copyright (c) 2000-2005 SAP AG
  -----------------------------------------------------------------------------


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

#ifndef _DBMWeb_Web_HPP_
#define _DBMWeb_Web_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */


/*
  -----------------------------------------------------------------------------
  specification public macros
  -----------------------------------------------------------------------------
 */

// stuff
#define DBMWEB_COMPID              "DBMWeb"
#define DBMWEB_STRINGTERM          '\0'

#define DBMWEB_WA_TRACE(s)         wa.WriteLogMsg(s)
#define DBMWEB_TXT_EXPIRE          "Sat, 01 Jan 1990 00:00:00 GMT"

// dbmserver commands
#define DBMWEB_CMD_DBMVERSION      "dbm_version"
#define DBMWEB_CMD_DBENUM          "db_enum"
#define DBMWEB_CMD_DBSTATE         "db_state"

#define DBMWEB_CMD_SQLCONNECT      "sql_connect"
#define DBMWEB_CMD_SQLRELEASE      "sql_release"

#define DBMWEB_CMD_UTILCONNECT     "util_connect"
#define DBMWEB_CMD_UTILRELEASE     "util_release"

#define DBMWEB_CMD_INFO            "info"

#if defined (_WIN32)
  #define strncasecmp  _strnicmp
  #define strcasecmp   _stricmp
#endif

#endif // _DBMWeb_Web_HPP_

