/*!
 * \file    SQL_Handle.hpp
 * \author  PeterG (GoertzP)
 * \brief   C++ SQL Class: Handle
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


#ifndef SQL_HANDLE_HPP
#define SQL_HANDLE_HPP


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "Oms/OMS_DbpBase.hpp"

#ifndef SQL_OPTION_HPP
#include "CppSQL/SQL_Option.hpp"
#endif


/************************************************************************/
/* Forward declarations                                                 */
/************************************************************************/

class SQL;
class SQL_SessionContext;


/************************************************************************/
/* Class SqlHandle                                                      */
/************************************************************************/

class AFX_EXT_CLASS SqlHandle
#if !defined (EXT_CLIENT_SQL)
: public DbpBase
#endif
{
# if defined (SQLCLS_IMPL)
  friend class SQL;
  friend class SQL_SessionContext;
  friend class SqlSession;
# endif
protected:
  SqlHandle();
  ~SqlHandle();
  void sqlInitSession(IUnknown ** s);
public :
  SQL                   sql(const char * stmt);
  SQL                   sql(const DbpTypeUnicode* stmt);
  typedef void (__cdecl * sqlHandler) (SqlHandle * p_sqlhandle);
  sqlHandler            sqlSetHandler(sqlHandler p); /* PTS 1120794 */
# if defined (SQLCLS_IMPL)
  void                  sqlSetDefaultHandler(); /* PTS 1120794 */
# endif
  short                 sqlCode() const; // returns sql error code
  const char *          sqlState() const; // returns a copy of sqlstate
  const char *          sqlErrorMsg() const; // returns a copy of sqlerrmc
  const DbpTypeUnicode* sqlUnicodeErrorMsg() const; // returns a copy of sqlerrmc_uni
  int                   sqlIsUnicodeErrMsg() const;
  char                  sqlWarning(int i) const; // returns sqlwarn [i] )
  long                  sqlErrorPos() const; // returns sql error position sqlerrd[5]
  long                  sqlResultCount() const; // returns sql result count sqlerrd[2]
  void                  sqlSetMode(const SqlOpt::SqlSessionMode mode);
  void                  sqlRaiseError( short msgno, const char * errmsg );
  void                  sqlRaiseError( short msgno, const DbpTypeUnicode * errmsg );
# if !defined (EXT_CLIENT_SQL)
  void                  sqlExceptionHandler(DbpError & e);
  bool                  sqlGetMethodCallMonitorInfo(SqlMonitorData& info) const; /* PTS 1120794 */
# if defined (SQLCLS_IMPL)
  HRESULT               AdviseSqlSession();
  HRESULT               UnAdviseSqlSession();
  void                  sqlSetMethodCallEpilog(); /* PTS 1120794 */
  void                  sqlResetMethodCallEpilog(); /* PTS 1120794 */
  void                  sqlSetCurrMethod(unsigned char* name); /* PTS 1120794 */
  void                  sqlResetCurrMethod(); /* PTS 1120794 */
  void                  sqlResetMonitor(); /* PTS 1120794 */
  void                  sqlSetRuntime(unsigned int Runtime); /* PTS 1120794 */
  const IliveCacheSink* getSinkPtr() const {return getLcSink();} /* PTS 1130629 */
# endif
# else
  inline void           dbpDebugBreak();
# endif
# if defined (SQLCLS_IMPL)
  void                  sqlResetSession();
# endif
  void *                m_str_assign;
  void *                m_str_append;
private:
  SQL_SessionContext *  m_session_context;
};


/************************************************************************/
/* Default error handler                                                */
/************************************************************************/


void
#if defined(WIN32)
__cdecl
#endif
sqlDefaultHandler(SqlHandle * p_sqlhandle);


#endif /* SQL_HANDLE_HPP */

