/*!
  -----------------------------------------------------------------------------

  module: gin100.h

  -----------------------------------------------------------------------------

  responsible:  BerndV

  special area: Error Messages

  description:  specification of log system error messages

  version:      min 7.2.

  -----------------------------------------------------------------------------

                          Copyright (c) 1998-2005 SAP AG

  -----------------------------------------------------------------------------



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




*/


#ifndef _GIN100_H_
#define _GIN100_H_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
*/
#include "geo200.h"  // teo200_EventList
#include "geo47.h"   // Unicode _TCHAR

/*
  -----------------------------------------------------------------------------
  Definitions
  -----------------------------------------------------------------------------
*/

#define TERR_IN100               teo200_EventList::Error_eeo200
#define TWRN_IN100               teo200_EventList::Warning_eeo200
#define TINF_IN100               teo200_EventList::Info_eeo200

#define N_IN100(_n)              STATIC_CAST( teo200_EventList::teo200_EventID, (-24000-_n))

#define FUNCTION_DBG_MIN100(_s)   const _TCHAR * pName_in100 = _s
#define FUNCTION_NAME_MIN100      pName_in100

inline teo200_EventList::teo200_EventID in100_ErrId
    ( const teo200_EventList::teo200_EventID _a, 
      const teo200_EventList::teo200_EventType _b, 
      const _TCHAR * const _c, 
      const _TCHAR * const _d ) 
{
  return _a;
} // end in100_ErrId

/*
  -----------------------------------------------------------------------------
  Error Codes
  -----------------------------------------------------------------------------
*/
#define  OK_IN100                                    0 , TERR_IN100, _T("")   , _T("everything works fine")

#define  ERR_IN100                        N_IN100(   0), TERR_IN100, _T("LOG"), _T("common error")

// Errors for Logging
#define  ERR_MEMORY_IN100                 N_IN100(   1), TERR_IN100, _T("LOG"), _T("error allocation memory")
#define  ERR_RTE_IN100                    N_IN100(   2), TERR_IN100, _T("RTE"), _T("original message: %s (%d)")
#define  ERR_THREAD_IN100                 N_IN100(   3), TERR_IN100, _T("LOG"), _T("Thread API error: %s (%d)")
#define  ERR_FILE_IN100                   N_IN100(   4), TERR_IN100, _T("LOG"), _T("File API error: %s (%d)")

// Errors for XDB
#define  ERR_XDBCONNECT_IN100             N_IN100( 200), TERR_IN100, _T("XDB"), _T("connect error: %s %s (%d)")
#define  ERR_XDBNOTINST_IN100             N_IN100( 201), TERR_IN100, _T("XDB"), _T("XDB tables not installed")
#define  ERR_XDBSQL_IN100                 N_IN100( 202), TERR_IN100, _T("XDB"), _T("SQL error (%d): %s")
#define  ERR_XDBINVVAL_IN100              N_IN100( 203), TERR_IN100, _T("XDB"), _T("invalid value: %s")
#define  ERR_XDBSEQU_IN100                N_IN100( 204), TERR_IN100, _T("XDB"), _T("wrong call sequence")
#define  ERR_XDBINVFUNC_IN100             N_IN100( 205), TERR_IN100, _T("XDB"), _T("Invalid function id! Entry '%s'/'%s' in DBAFID not found")
#define  ERR_XDBINVTRIAL_IN100            N_IN100( 206), TERR_IN100, _T("XDB"), _T("Invalid operation id! Entry '%s'/'%s'/'%d' in DBATRIAL not found")
#define  ERR_XDBINVERR_IN100              N_IN100( 207), TERR_IN100, _T("XDB"), _T("Invalid error code! Entry '%d' in DBAERR not found")
#define  ERR_XDBNOSESS_IN100              N_IN100( 208), TERR_IN100, _T("XDB"), _T("SQL Session not active!")

// Errors for SQL
#define  ERR_SQLSQL_IN100                 N_IN100( 300), TERR_IN100, _T("SQL"), _T("SQL error (%d): %s")
#define  ERR_SQLNOSESS_IN100              N_IN100( 301), TERR_IN100, _T("SQL"), _T("SQL Session not active!")
#define  ERR_SQLNOSEL_IN100               N_IN100( 302), TERR_IN100, _T("SQL"), _T("Statement is not a SELECT!")

#endif // _GIN100_H_