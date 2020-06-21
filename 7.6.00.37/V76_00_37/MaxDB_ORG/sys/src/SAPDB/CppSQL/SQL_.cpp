/*!
 * \file    SQL_.cpp
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


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "geo00.h"

#ifndef __OMS_DEFINES_HPP
#include "Oms/OMS_Defines.h"
#endif

#include "liveCache/LVC_IliveCacheSink.hpp"

#ifndef SQL_HANDLE_HPP
#include "CppSQL/SQL_Handle.hpp"
#endif

#ifndef SQL_HPP
#include "CppSQL/SQL_.hpp"
#endif

#ifndef SQL_SESSIONCONTEXT_HPP
#include "CppSQL/SQL_SessionContext.hpp"
#endif

#ifndef SQL_STATEMENT_HPP
#include "CppSQL/SQL_Statement.hpp"
#endif

#ifndef SQL_COLUMN_HPP
#include "CppSQL/SQL_Column.hpp"
#endif


/************************************************************************/
/* Implementation of class SQL                                          */
/************************************************************************/

SQL::SQL( SqlHandle* handle, const char *stmt ) {
  m_sqlstatement = NULL;
  handle->m_session_context->createSqlStmt( *this, stmt );
}

/*----------------------------------------------------------------------*/

SQL::SQL( SqlHandle* handle, const DbpTypeUnicode *stmt ) {
  m_sqlstatement = NULL;
  handle->m_session_context->createSqlStmt( *this, stmt );
}

/*----------------------------------------------------------------------*/

#if defined(EXT_CLIENT_SQL)

int SQL::connect( const char* hostname, const char* dbname, const char *user, 
                  const char *passwd, const char* isolevel) {
  if (m_sqlstatement) {
    return m_sqlstatement->connect( hostname, dbname, user, 
                                    passwd, isolevel);
  }
  else {
    return 0;
  }
}

#endif

/*----------------------------------------------------------------------*/

SQL::SQL( ) {
  m_sqlstatement = NULL;
}

/*----------------------------------------------------------------------*/

SQL::~SQL() {
  if (NULL != m_sqlstatement) {
    m_sqlstatement->release();
    m_sqlstatement = NULL;
  }
}

/*----------------------------------------------------------------------*/

SQL::SQL(const SQL& sql) {
  m_sqlstatement = sql.m_sqlstatement;
  if (m_sqlstatement) {
    m_sqlstatement->addRef();
  }
}

/*----------------------------------------------------------------------*/

short SQL::sqlNumCols() const {
  if (m_sqlstatement) {
    return m_sqlstatement->getNumCols();
  }
  else {
    return 0;
  }
}

/*----------------------------------------------------------------------*/

short SQL::sqlNumOutCols() const {
  if (m_sqlstatement) {
    return m_sqlstatement->getNumOutCols();
  }
  else {
    return 0;
  }
}

/*----------------------------------------------------------------------*/

int SQL::sqlExecute() {
  if (m_sqlstatement) {
    return m_sqlstatement->execute();
  }
  else {
    return 0;
  }
}

/*----- PTS 1119960 ----------------------------------------------------*/

int SQL::sqlExecuteMass() {
  if (m_sqlstatement) {
    return m_sqlstatement->executeMass();
  }
  else {
    return 0;
  }
}

/*----------------------------------------------------------------------*/

int SQL::sqlPrepare() {
  if (m_sqlstatement) {
    return m_sqlstatement->prepare();
  }
  else {
    return 0;
  }
}

/*----------------------------------------------------------------------*/

int SQL::operator() () {
  return sqlExecute ();
}

/*----------------------------------------------------------------------*/

#if defined (SUN) || defined(SOLARIS)
SQL &SQL::operator<< (const SqlCol p)
#else
SQL &SQL::operator<< (const SqlCol& p)
#endif
{
  if (m_sqlstatement) {
    m_sqlstatement->appendNewColDesc(p);
  }
  return *this;
}

/*----------------------------------------------------------------------*/

SqlCol &SQL::operator[] (int i) {
  if ( m_sqlstatement ) {
    return (*m_sqlstatement)[i];
  }
  else {
    return *new(SqlCol);
  }
}

/*----------------------------------------------------------------------*/

const SQL & SQL::operator= (const SQL& right) {
  if (NULL != m_sqlstatement) {
    m_sqlstatement->release();
  }
  m_sqlstatement = right.m_sqlstatement;
  if (NULL != m_sqlstatement) {
    m_sqlstatement->addRef();
  }
  return *this;
}

/*----------------------------------------------------------------------*/

void* SQL::operator new (size_t sz) {
# if defined (EXT_CLIENT_SQL)
    void *p = malloc(sz);
    if (!p) {
      throw DbpError( e_new_failed, "new failed" );
    }
    return p;
# else
#   if defined (OMSTST)
      co10_GetKernelInterface()->TestBadAlloc();
#   endif
    //SAPDBMem_IRawAllocator *m_alloc;
    //co10_GetLcSink()->GetSqlClsAllocator(m_alloc);
    SQL_SessionContext *pSessionContext = NULL;
    tsp00_TaskId taskId;
    co10_GetLcSink()->GetSqlSessionContext(REINTERPRET_CAST(void**,&pSessionContext), taskId);
    return pSessionContext->allocate(sz);     
# endif
}

/*----------------------------------------------------------------------*/

void SQL::operator delete (void* p) {
# if defined (EXT_CLIENT_SQL)
    free(p);
# else
    //SAPDBMem_IRawAllocator *m_alloc;
    //co10_GetLcSink()->GetSqlClsAllocator(m_alloc);
    SQL_SessionContext *pSessionContext = NULL;
    tsp00_TaskId taskId;
    co10_GetLcSink()->GetSqlSessionContext(REINTERPRET_CAST(void**,&pSessionContext), taskId);
    pSessionContext->deallocate(p);
# endif
}

/*----------------------------------------------------------------------*/

void* SQL::operator new[] (size_t sz) {
# if defined (EXT_CLIENT_SQL)
    void *p = malloc(sz);
    if (!p) {
      throw DbpError(e_new_failed,"new failed");
    }
    return p;
# else
#   if defined (OMSTST)
      co10_GetKernelInterface()->TestBadAlloc();
#   endif
    //SAPDBMem_IRawAllocator *m_alloc;
    //co10_GetLcSink()->GetSqlClsAllocator(m_alloc);
    SQL_SessionContext *pSessionContext = NULL;
    tsp00_TaskId taskId;
    co10_GetLcSink()->GetSqlSessionContext(REINTERPRET_CAST(void**,&pSessionContext), taskId);
    return pSessionContext->allocate(sz);  
# endif
}

/*----------------------------------------------------------------------*/

void SQL::operator delete[] (void* p) {
# if defined (EXT_CLIENT_SQL)
    free(p);
# else
    //SAPDBMem_IRawAllocator *m_alloc;
    //co10_GetLcSink()->GetSqlClsAllocator(m_alloc);
    SQL_SessionContext *pSessionContext = NULL;
    tsp00_TaskId taskId;
    co10_GetLcSink()->GetSqlSessionContext(REINTERPRET_CAST(void**,&pSessionContext), taskId);
    pSessionContext->deallocate(p);
# endif
}

