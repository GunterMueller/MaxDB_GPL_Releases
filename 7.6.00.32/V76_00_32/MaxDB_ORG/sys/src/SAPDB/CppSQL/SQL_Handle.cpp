/*!
 * \file    SQL_Handle.cpp
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


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef SQL_HANDLE_HPP
#include "CppSQL/SQL_Handle.hpp"
#endif

#ifndef SQL_HPP
#include "CppSQL/SQL_.hpp"
#endif

#ifndef SQL_SESSIONCONTEXT_HPP
#include "CppSQL/SQL_SessionContext.hpp"
#endif

#include "hsp81.h"


/************************************************************************/
/* Provoke division by zero in case of a range violation                */
/************************************************************************/
/* PG: Do these functions really force divisions by zero? */

#if !defined (EXT_CLIENT_SQL)
#if defined(QUICK) || defined(SLOW)

externC teo00_Longint
sqlk_rangeviolation( // ALL IN
                     teo00_Longint val,
                     teo00_Longint minVal,
                     teo00_Longint maxVal,
                     int           lineno,
                     const char *   fileName )
{ int x = 0; x = 1/10*x; return -1; }

/*----------------------------------------------------------------------*/

externC void
sqlk_assert( // ALL IN
             int          kind,
             int          lineno,
             const char * filename,
             const char * condition )
{ int x = 0; x = x/10*0; }

#endif
#endif


/************************************************************************/
/* Implementation of default error handler                              */
/************************************************************************/

void
#if defined(WIN32)
__cdecl
#endif
sqlDefaultHandler(SqlHandle * p_sqlhandle) {
  if (p_sqlhandle->sqlCode() != 100) {
    if (p_sqlhandle->sqlIsUnicodeErrMsg()) {
      throw DbpError( DbpError::DB_ERROR_UNICODE,
                      p_sqlhandle->sqlCode(),
                      p_sqlhandle->sqlUnicodeErrorMsg(),
                      __FILE__, __LINE__ );
    }
    else {
      throw DbpError( DbpError::DB_ERROR,
                      p_sqlhandle->sqlCode(),
                      p_sqlhandle->sqlErrorMsg(),
                      __FILE__, __LINE__ );
    }
  }
}


/************************************************************************/
/* Implementation of class SqlHandle                                    */
/************************************************************************/

SqlHandle::SqlHandle() {
  m_session_context = NULL;
/* PTS 1120794 */
# if !defined (EXT_CLIENT_SQL)
  if (!OMS_Globals::m_globalsInstance->InSimulator()) {
    tsp00_TaskId taskId;
    co10_GetLcSink()->GetSqlSessionContext(REINTERPRET_CAST(void**,&m_session_context), taskId);
    if (NULL == m_session_context) {
      m_session_context = new SQL_SessionContext(*this, taskId);
      co10_GetLcSink()->SetSqlSessionContext((void*)m_session_context);
      sqlSetHandler(&sqlDefaultHandler);
    }
    m_session_context->incRefCnt();
  }
# else
  m_session_context = new SQL_SessionContext(*this,0);
  sqlSetHandler(&sqlDefaultHandler);
# endif
}

/*----------------------------------------------------------------------*/

SqlHandle::~SqlHandle() {
  if (m_session_context)
    m_session_context->deleteSelf();
}

/*----------------------------------------------------------------------*/

void SqlHandle::sqlInitSession(IUnknown** s) {
  /* m_session = (IliveCacheSink**)s; PTS 1122076 */
}

/*----------------------------------------------------------------------*/

SQL SqlHandle::sql(const char* stmt) {
  return SQL( this, stmt );
}

/*----------------------------------------------------------------------*/

SQL SqlHandle::sql(const DbpTypeUnicode* stmt) {
  return SQL( this, stmt );
}

/*----- PTS 1120704 ----------------------------------------------------*/

typedef void (__cdecl * sqlHandler) (SqlHandle* p_sqlhandle);

sqlHandler SqlHandle::sqlSetHandler(sqlHandler p) {
  sqlHandler p_old = m_session_context->getHandler();
  m_session_context->setHandler(p);
  return (p_old);
}

/*----- PTS 1120794 ----------------------------------------------------*/

void SqlHandle::sqlSetDefaultHandler() {
  sqlSetHandler(&sqlDefaultHandler);
}

/*----------------------------------------------------------------------*/

short SqlHandle::sqlCode() const {
  if (m_session_context) {
    return m_session_context->sqlCode ();
  }
  else {
    return e_unknown_error;
  }
}

/*----------------------------------------------------------------------*/

const char* SqlHandle::sqlState() const {
  if (m_session_context) {
    return m_session_context->sqlState();
  }
  else {
    return NULL;
  }
}

/*----------------------------------------------------------------------*/

const char* SqlHandle::sqlErrorMsg() const {
  if (m_session_context) {
    return m_session_context->sqlErrorMsg ();
  }
  else {
    return NULL;
  }
}

/*----------------------------------------------------------------------*/

const DbpTypeUnicode* SqlHandle::sqlUnicodeErrorMsg() const {
  if (m_session_context) {
    return m_session_context->sqlUnicodeErrorMsg ();
  }
  else {
    return NULL;
  }
}

/*----------------------------------------------------------------------*/

int SqlHandle::sqlIsUnicodeErrMsg() const {
  return m_session_context->sqlIsUnicodeErrMsg();
}

/*----------------------------------------------------------------------*/

char SqlHandle::sqlWarning(int i) const {
  if (m_session_context) {
    return m_session_context->sqlWarning(i);
  }
  else {
    return ' ';
  }
}

/*----------------------------------------------------------------------*/

long SqlHandle::sqlErrorPos() const {
  if (m_session_context) {
    return m_session_context->sqlErrorPos();
  }
  else {
    return -1;
  }
}

/*----------------------------------------------------------------------*/

long SqlHandle::sqlResultCount() const {
  if (m_session_context) {
    return m_session_context->sqlResultCount();
  }
  else {
    return -1;
  }
}

/*----------------------------------------------------------------------*/

void SqlHandle::sqlSetMode(const SqlOpt::SqlSessionMode mode) {
  if (m_session_context) {
    m_session_context->sqlSetMode(mode);
  }
}

/*----------------------------------------------------------------------*/

void SqlHandle::sqlRaiseError( short msgno, const char* errmsg ) {
# if defined(EXT_CLIENT_SQL)
    throw DbpError( msgno, errmsg );
# else
    /* PTS 1122076 */
    IliveCacheSink * pSink = m_session_context->getSink();
    if (pSink) {
      pSink->SetError( msgno, (tsp00_Int4)strlen(errmsg), 
                       (unsigned char*)errmsg, csp_ascii );
    }
# endif
}

/*----------------------------------------------------------------------*/

void SqlHandle::sqlRaiseError( short msgno, const DbpTypeUnicode* errmsg ) {
# if defined(EXT_CLIENT_SQL)
    throw DbpError( msgno, errmsg );
# else
    /* PTS 1122076 */
    IliveCacheSink * pSink = m_session_context->getSink();
    if (pSink) {
      tsp81_UCS2Char UCS2Blank;
      int            UCS2Swap;
      UCS2Blank.s = 32;
      UCS2Swap    = ( UCS2Blank.c[0] ? 1 : 0 );
      int len = sp81UCS2strlen((tsp81_UCS2Char*)errmsg);
      if (UCS2Swap) {
        DbpTypeUnicode l_errmsg[256];
        if (len > 256) {
          len = 256;
        }
        sp81UCS2strncpy( REINTERPRET_CAST(tsp81_UCS2Char*,l_errmsg), 
                         REINTERPRET_CAST(const tsp81_UCS2Char*,errmsg), 
                         256 );
        for ( int i = 0; i < len; i++ ) {
          l_errmsg[i] = l_errmsg[i] << 8;
        }
        pSink->SetError( msgno, len*2, 
                         (unsigned char*) &l_errmsg[0], csp_unicode );
      }
      else {
        pSink->SetError( msgno, len*2, (unsigned char*) errmsg, 
                         csp_unicode );
      }
    }
# endif
}

/*----------------------------------------------------------------------*/

#if !defined (EXT_CLIENT_SQL)

/*----------------------------------------------------------------------*/

void SqlHandle::sqlExceptionHandler(DbpError& e) {
  IliveCacheSink * pSink = m_session_context->getSink(); /* PTS 1122076 */
  if (pSink) {                                           /* PTS 1122076 */
    switch (e.m_errorKind) {
      case DbpError::DB_ERROR :
      case DbpError::USER_DEFINED :
        sqlRaiseError ( (short)e.m_errorNo, &e.m_errorText[0] );
        break;
      case DbpError::DB_ERROR_UNICODE :
      case DbpError::USER_DEFINED_UNICODE :
        sqlRaiseError( (short)e.m_errorNo, 
                       REINTERPRET_CAST(DbpTypeUnicode*,&e.m_errorText[0]) );
        break;
      case DbpError::HRESULT_ERROR :
        pSink->HResultError(e.m_errorNo); /* PTS 1122076 */
        break;
      default :
        dbpOpError( "Invalid DbpError, kind = %d, errno = %d", 
                    e.m_errorKind, e.m_errorNo);
        throw;
    }
  }
}

/*----------------------------------------------------------------------*/

HRESULT SqlHandle::AdviseSqlSession() {
  /* PTS 1122076 */
  IliveCacheSink * pSink = m_session_context->getSink();
  if (pSink) {
    dbpAdviseLcSink(pSink);
  }
  return S_OK;
}

/*----------------------------------------------------------------------*/

HRESULT SqlHandle::UnAdviseSqlSession() {
  dbpAdviseLcSink(NULL);
  if (m_session_context) {
    m_session_context->setSink(NULL); /* PTS 1122076 */
    m_session_context->getSqlHandle()->dbpAdviseLcSink(NULL); /* PTS 1130629 */
  }
  return S_OK;
}

/*----- PTS 1120794 ----------------------------------------------------*/

void SqlHandle::sqlSetMethodCallEpilog() {
  if (m_session_context) {
    m_session_context->setMethodCallEpilog();
  }
}

/*----- PTS 1120794 ----------------------------------------------------*/

void SqlHandle::sqlResetMethodCallEpilog() {
  if (m_session_context) {
    m_session_context->resetMethodCallEpilog();  
  }
}

/*----- PTS 1120794 ----------------------------------------------------*/

void SqlHandle::sqlSetCurrMethod(unsigned char* name) {
  if(m_session_context) {
    m_session_context->setCurrMethod(name);
  }
}

/*----- PTS 1120794 ----------------------------------------------------*/

void SqlHandle::sqlResetCurrMethod() {
  if (m_session_context) {
    m_session_context->resetCurrMethod();
  }
}

/*----- PTS 1120794 ----------------------------------------------------*/

bool SqlHandle::sqlGetMethodCallMonitorInfo(SqlMonitorData& info) const {
  if (m_session_context) {
    return m_session_context->getMethodCallMonitorInfo(info);
  }
  else {
    return false;
  }
}

/*----- PTS 1120794 ----------------------------------------------------*/

void SqlHandle::sqlResetMonitor() {
  if (m_session_context) {
    m_session_context->getMonitor()->Reset();
  }
}

/*----- PTS 1120794 ----------------------------------------------------*/

void SqlHandle::sqlSetRuntime(unsigned int Runtime) {
  if (m_session_context) {
    m_session_context->getMonitor()->SetRuntime(Runtime);
  }
}

/*----------------------------------------------------------------------*/

#endif

/*----------------------------------------------------------------------*/

void SqlHandle::sqlResetSession( ) {
//  m_sqlmsg->m_snd_packet_size = 0;
}

