/*!
 * \file    SQL_SessionContext.cpp
 * \author  PeterG (GoertzP)
 * \brief   C++ SQL Class: Session context
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

#if !defined (EXT_CLIENT_SQL)
#include "Oms/OMS_Globals.hpp"
#include "Oms/OMS_GlobalAllocatorWrapper.hpp"
#include "liveCache/LVC_KernelInterface.hpp"
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

#ifndef HIN204_H
#include "PacketInterface/PIn_ReplyParser.h"
#endif

#ifndef HIN205_H
#include "PacketInterface/PIn_RequestWriter.h"
#endif

#ifndef SQL_REPLYPARSER_HPP
#include "CppSQL/SQL_ReplyParser.hpp"
#endif

#if defined (EXT_CLIENT_SQL)
#include "gsp01.h"
#include "heo02.h"
#include "heo03.h"
#include "hsp02.h"
#include <new.h>
#endif

#include "SAPDBCommon/SAPDB_MemCopyMove.hpp"
#include "hsp81.h"
#include "hsp100.h"    /* PTS 1112481 */

/* PTS 1130629 */
#if !defined (EXT_CLIENT_SQL)
#include "hsp78_0.h"
#endif


/************************************************************************/
/* Constants                                                            */
/************************************************************************/

#if !defined (EXT_CLIENT_SQL)
const int C_MIN_REPLY_SIZE = 0; /* 1024; */
#endif

const short C_SQL_PARSE_AGAIN = -8;   /* PTS 1112690 */

const pasbool C_PARSING_AGAIN = true; /* PTS 1112690 */

/************************************************************************/
/* Auto pointer                                                         */
/************************************************************************/

template<class T> class auto_ptr 
{
    public:
        explicit auto_ptr(T *p = 0)  { pointee = p; };
        auto_ptr<T>& operator=(T *p) { pointee = p; };
        T* release() 
        {
            T* p = pointee;
            pointee = NULL;
            return p;
        };
        ~auto_ptr() { 
            if (pointee)
              pointee->release(); 
        };
    private:
        T* pointee;
};


/************************************************************************/
/* Helper functions                                                     */
/************************************************************************/

/* PTS 1112481 */
static void SQL_InitVersion(PIn_RequestPacket & reqPacket) {
  tsp00_C5c vers;
  sp100_GetSenderIDVersion(vers);
# if defined (EXT_CLIENT_SQL)
    reqPacket.InitVersion( "CPP", vers );
# else
    reqPacket.InitVersion( "C++", vers );
# endif
}


/************************************************************************/
/* Implementation of class SQL_SessionContext                           */
/************************************************************************/

#define REGISTER_ALLOCATOR true

SQL_SessionContext::SessionHeap::SessionHeap(tsp00_TaskId taskId) 
# if !defined (EXT_CLIENT_SQL)
: m_alloc( (const SAPDB_UTF8*)"", OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper,
            32 * 1024, 32 * 1024, SAPDBMem_RawAllocator::FREE_RAW_EXTENDS, 
            SAPDBMEM_ALLOCATOR_UNLIMITED, !REGISTER_ALLOCATOR)
# endif
{
# if !defined (EXT_CLIENT_SQL)
  SAPDB_UTF8 heapName[41];
  sp77sprintf (REINTERPRET_CAST(char*, &heapName[0]), sizeof(heapName), "SqlCls   T%03d", taskId);
  m_alloc.SetIdentifier (&heapName[0]);

  OMS_Globals::m_globalsInstance->GetKernelInterface()->RegisterAllocator(m_alloc.GetAllocatorInfo());
# endif
}

/*----------------------------------------------------------------------*/

SQL_SessionContext::SessionHeap::~SessionHeap() {
# if !defined (EXT_CLIENT_SQL)
  OMS_Globals::m_globalsInstance->GetKernelInterface()->DeregisterAllocator(m_alloc.GetAllocatorInfo());
# endif
}

/*----------------------------------------------------------------------*/

SQL_SessionContext::SQL_SessionContext(SqlHandle & pHandle, tsp00_TaskId taskId) :
  m_heap(taskId)
, m_CSqlStmtDir(&m_heap)
, m_sqlhandle(pHandle)
, m_request_packet_size(0)
, m_request_packet(0)
, m_reply_packet_size(0) /* PTS 1133793 */
, m_reply_packet(0)
, m_deleteSelf(false)
, m_sqlhandler(NULL)
/* PTS 1120794 */
, m_ref_count(0)
# if !defined(EXT_CLIENT_SQL)
, m_pSink(co10_GetLcSink()) /* PTS 1122076 */
, m_data_len(0)
, m_inMethodCallEpilog(false)
{
  //SAPDBMem_IRawAllocator * m_alloc;
  //m_pSink->GetSqlClsAllocator(m_alloc); /* PTS 1122076 */
  m_curr_method = (tsp00_KnlIdentifier*)allocate(sizeof(tsp00_KnlIdentifier));
  m_hasUnicodeCatalog = OMS_Globals::KernelInterfaceInstance->IsUnicodeInstance(); /* PTS 1130629 */
  short error = 0;                                          /* PTS 1121342 */
  m_pSink->IsUnicodeInstance( m_isUnicodeInstance, error ); /* PTS 1121342 */
# else
, m_pSink(NULL) /* PTS 1122076 */
{
# endif
  m_sqlhandle.m_session_context = this; /* PTS 1122076 */
# if !defined(EXT_CLIENT_SQL)
    m_sqlhandle.dbpAdviseLcSink(m_pSink); /* PTS 1130629 */
# endif
  m_sqlmode.becomes(sp1sm_internal);
}

/*----------------------------------------------------------------------*/

void SQL_SessionContext::sqlSetMode(const SqlOpt::SqlSessionMode mode) {
  switch (mode) {
    case SqlOpt::SQLMODE_NIL :
      m_sqlmode.becomes(sp1sm_nil);
      break;
    case SqlOpt::SQLMODE_SESSION :
      m_sqlmode.becomes(sp1sm_session_sqlmode);
      break;
    case SqlOpt::SQLMODE_INTERNAL :
      m_sqlmode.becomes(sp1sm_internal);
      break;
    case SqlOpt::SQLMODE_ANSI :
      m_sqlmode.becomes(sp1sm_ansi);
      break;
    case SqlOpt::SQLMODE_DB2 :
      m_sqlmode.becomes(sp1sm_db2);
      break;
    case SqlOpt::SQLMODE_ORACLE :
      m_sqlmode.becomes(sp1sm_oracle);
      break;
  }
}

/*----------------------------------------------------------------------*/

void SQL_SessionContext::deleteSelf() {
/* PTS 1120794 */
# if !defined (EXT_CLIENT_SQL)
    decRefCnt();
# endif  
    if (0 == m_ref_count) {
      m_deleteSelf = true;
      if (m_CSqlStmtDir.empty()) {
        m_heap.~SessionHeap();
#       if defined (EXT_CLIENT_SQL)
          free(this);
#       else
          //SAPDBMem_IRawAllocator * m_alloc;
          //co10_GetLcSink()->GetSqlClsAllocator(m_alloc);
          //m_alloc->Deallocate(m_curr_method);
          OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Deallocate(this);
          co10_GetLcSink()->SetSqlSessionContext(NULL);
#       endif
      }
  }
}

/*----------------------------------------------------------------------*/

void * SQL_SessionContext::operator new (size_t sz) {
# if defined (EXT_CLIENT_SQL)
    void * p = malloc(sz);
    if (!p) {
      throw DbpError(e_new_failed,"new failed");
    }
    return p;
# else
#   if defined (OMSTST)
      co10_GetKernelInterface()->TestBadAlloc();
#   endif
    //SAPDBMem_IRawAllocator * m_alloc;
    //co10_GetLcSink()->GetSqlClsAllocator(m_alloc);
    return OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Allocate(sz);
# endif
}

/*----------------------------------------------------------------------*/

void SQL_SessionContext::operator delete (void * p) {
# if defined (EXT_CLIENT_SQL)
  free(p);
# else
  OMS_Globals::m_globalsInstance->m_omsAllocatorWrapper.Deallocate(p);
# endif
}

/*----------------------------------------------------------------------*/

#if defined (EXT_CLIENT_SQL)

int SQL_SessionContext::connect( SQL_Statement * pSqlStmt,
                                 const char * hostname,
                                 const char * dbname,
                                 const char * user,
                                 const char * pwd,
                                 const char * isolevel) {
  PIn_Part * part;
  char       l_stm[256];
  int        rc = 0;

  m_sqlca.sqlCAInit();

  tsp00_ErrTextc errtext;
  tsp01_CommErr_Enum commErr =
    SqlDBConnect( CONST_CAST(char*,hostname),
                  REFCAST_MEO00(tsp00_DbNamec)dbname,
                  srvUser_esp01, 1, m_connection,
                  m_reply_packet_size,
                  m_request_packet_size, (void**)&m_request_packet,
                  errtext );

  if (commErr) {
    switch (commErr) {
      case commErrShutdown_esp01 :
        setRtError( e_dbshutdown, dbname, hostname, NULL );
        break;
      case commErrTasklimit_esp01 :
        setRtError( e_too_many_users, dbname, hostname, NULL );
        break;
      case commErrNotOk_esp01 :
      case commErrTimeout_esp01 :
      case commErrCrash_esp01 :
      case commErrStartRequired_esp01 :
      case commErrSendLineDown_esp01 :
      case commErrReceiveLineDown_esp01 :
      case commErrPacketLimit_esp01 :
      case commErrReleased_esp01 :
      case commErrWouldBlock_esp01 :
      case commErrUnknownRequest_esp01 :
      case commErrServerOrDBUnknown_esp01 :
      default :
        setRtError( e_dbconnect_notok, dbname, hostname, NULL );
    }
    m_connection          = -1;
    m_request_packet_size = 0;
    m_reply_packet_size   = 0; /* PTS 1133793 */
    m_pSink = NULL; /* PTS 1122076 */
    return 0;
  }

  m_pSink = (IliveCacheSink*)m_connection; /* PTS 1122076 */

  IliveCacheSink * pILCSink = initSession();
  if (NULL != pILCSink) {
    PIn_RequestPacket reqPacket( m_request_packet, m_request_packet_size,
                                 pSqlStmt->isUnicode() );
    SQL_InitVersion(reqPacket);                         /* PTS 1112481 */
    PIn_RequestWriter reqWriter(reqPacket);
    reqWriter.Reset();
    /*
    * connect statement
    */
    strcpy( l_stm, "connect \"" );
    strcat( l_stm, user );
    strcat( l_stm,
            "\" identified by ? sqlmode internal isolation level " );
    strcat( l_stm, isolevel );
    reqWriter.InitDbs( m_sqlmode, l_stm );  /* SQL-Mode PTS 1108446 */
    /*
    * convert pwd
    */
    tsp00_Name      pasString;
    tsp00_CryptPw   stCrypt;
    s02applencrypt( pasString.c2p(pwd), stCrypt );
    part = reqWriter.AddDataPart();
    part->AddParameterArg( stCrypt, 1, sizeof(stCrypt), 50, 'x' );
    tsp00_TermId termid;
    // sqltermid (termid); later: use module var initialized by environment
    SAPDB_MemCopyNoCheck( termid, "<unknown><unknown>", sizeof (termid) );
    part->AddParameterArg( termid, 51, sizeof(termid),
                           sizeof(termid) + 1, ' ');
    reqWriter.Close();
    rc = executeDBRequest( pILCSink, reqPacket );
    if (rc) {
      SQL_ReplyParser rplyParser( *this, *pSqlStmt );
      rc = rplyParser.ParseReplyData(m_reply_packet);
    }
  }

  if ( (m_sqlhandler) && (0 != m_sqlca.sqlcode) ) {     /* PTS 1120794 */
    m_sqlhandler(&m_sqlhandle);                         /* PTS 1120794 */
  }

  return (m_sqlca.sqlcode == 0);
}

#endif

/*----------------------------------------------------------------------*/

void SQL_SessionContext::createSqlStmt( SQL & sql, const char * stmt) {
  sql.m_sqlstatement = new SQL_Statement( this, stmt );
  auto_ptr<SQL_Statement> autoCSqlStmt(sql.m_sqlstatement);
  m_CSqlStmtDir.push_front(sql.m_sqlstatement);
  autoCSqlStmt.release();
}

/*----------------------------------------------------------------------*/

void SQL_SessionContext::createSqlStmt( SQL & sql,
                                        const DbpTypeUnicode * stmt) {
  sql.m_sqlstatement = new SQL_Statement( this, stmt );
  auto_ptr<SQL_Statement> autoCSqlStmt(sql.m_sqlstatement);
  m_CSqlStmtDir.push_front(sql.m_sqlstatement);
  autoCSqlStmt.release();
}

/*----------------------------------------------------------------------*/

void SQL_SessionContext::dropSqlStmt(SQL_Statement * pSqlStmt) {
  for (cgg251dclIterator<SQL_Statement*,SessionHeap> iter = 
         m_CSqlStmtDir.begin();
       iter; ++iter) {
    if (pSqlStmt == *iter()) {
      m_CSqlStmtDir.remove(iter);
      break;
    }
  }
  if (pSqlStmt->getPreparedFlag()) {
    dropParsid(pSqlStmt);
  }
  m_heap.deallocate(pSqlStmt);
  pSqlStmt = NULL;
  if (m_deleteSelf) {
    deleteSelf();
  }
}

/*----------------------------------------------------------------------*/

IliveCacheSink * SQL_SessionContext::initSession(const unsigned char reqType) {
  m_sqlca.sqlCAInit(reqType);
  IliveCacheSink * pILCSink = m_pSink; /* PTS 1122076 */

  if (NULL == pILCSink) {
    setRtError( e_dbconnection_down, NULL, NULL, NULL ); 
    return NULL;
  }

# if !defined (EXT_CLIENT_SQL)
    if (NULL == m_request_packet) {
      pILCSink->AllocPacket( (unsigned char**)&m_request_packet,
                             &m_request_packet_size );
      if ( (NULL == m_request_packet) || (0 == m_request_packet_size) ) {
        setRtError( e_dbconnection_down, NULL, NULL, NULL );
        return NULL;
      }
      m_request_packet_size = m_request_packet_size - C_MIN_REPLY_SIZE;
    }
# endif

  return pILCSink;
}

/*----------------------------------------------------------------------*/

int SQL_SessionContext::executeImmediateSQL( IliveCacheSink * pILCSink,
                                             SQL_Statement * pSqlStmt ) {
  int rc = 0;

  PIn_RequestPacket reqPacket( m_request_packet, m_request_packet_size,
                               pSqlStmt->isUnicode() );
  SQL_InitVersion(reqPacket);                         /* PTS 1112481 */
  PIn_RequestWriter reqWriter(reqPacket);
  reqWriter.Reset();
  if (pSqlStmt->isUnicode()) {
    /* PTS 1117853 FF 2002-Sep-16 */
    if (NULL == reqWriter.InitDbs(m_sqlmode,pSqlStmt->getUniStatement())) { /* SQL-Mode PTS 1108446 */
      setRtError( e_cmd_too_long, NULL, NULL, NULL );
      return rc;
    }
  }
  else {
    /* PTS 1117853 FF 2002-Sep-16 */
    if (NULL == reqWriter.InitDbs(m_sqlmode,pSqlStmt->getStatement())) { /* SQL-Mode PTS 1108446 */
      setRtError( e_cmd_too_long, NULL, NULL, NULL);
      return rc;
    }
  }
  reqWriter.Close();

  pSqlStmt->setResetParms();                  /* PTS 1109560 */
  rc = executeDBRequest( pILCSink, reqPacket );
  if (rc) {
    SQL_ReplyParser rplyParser( *this, *pSqlStmt );
    rc = rplyParser.ParseReplyData(m_reply_packet);
    /* PTS 1120794 */
#   if !defined (EXT_CLIENT_SQL)
      monitorRequest(m_reply_packet->sp1_segm().sp1r_function_code());
#   endif
  }
  return rc;
}

/*----------------------------------------------------------------------*/

int SQL_SessionContext::executePreparedSQL( IliveCacheSink * pILCSink,
                                            SQL_Statement * pSqlStmt ) {
  int rc = 1;
  PIn_Part * part;
  const tin02_ParseId * parseId;
  int len = pSqlStmt->getParsid(&parseId);

  PIn_RequestPacket reqPacket( m_request_packet, m_request_packet_size,
                               pSqlStmt->isUnicode() );
  SQL_InitVersion(reqPacket);                         /* PTS 1112481 */
  PIn_RequestWriter reqWriter(reqPacket);
  
  reqWriter.Reset();
  reqWriter.InitExecute( m_sqlmode, parseId ); /* SQL-Mode PTS 1108446 */
  if (pSqlStmt->hasInputParms()) {
    part = reqWriter.AddDataPart();
    rc = pSqlStmt->addInputParms(part);
    if (!rc) {
      return rc;
    }
#   if !defined(EXT_CLIENT_SQL)        /* PTS 1120704 */
    else {                             /* PTS 1120794 */
      m_data_len = part->Length();     /* PTS 1120794 */
    }                                  /* PTS 1120794 */
#   endif                              /* PTS 1120794 */
  }
  reqWriter.Close();

  pSqlStmt->setResetParms();                  /* PTS 1109560 */
  rc = executeDBRequest( pILCSink, reqPacket );
  if (rc) {
    SQL_ReplyParser rplyParser( *this, *pSqlStmt );
    rc = rplyParser.ParseReplyData(m_reply_packet);
    /* PTS 1120794 */
#   if !defined (EXT_CLIENT_SQL)
      monitorRequest(m_reply_packet->sp1_segm().sp1r_function_code());
#   endif
  }
  if (0 == m_sqlca.sqlcode) {
    if ( pSqlStmt->hasLongInput() && rc ) {
      rc = executeLongInput( pILCSink, reqPacket, pSqlStmt );
    }
    if ( pSqlStmt->hasLongOutput() && rc ) {
      rc = executeLongOutput( pILCSink, reqPacket, pSqlStmt );
    }
  }
  return rc;
}

/*------ PTS 1119960 ---------------------------------------------------*/

int SQL_SessionContext::executePreparedSQL( IliveCacheSink * pILCSink,
                                            SQL_Statement * pSqlStmt,
                                            int offset ) {
  int rc = 1;
  PIn_Part * part;
  const tin02_ParseId * parseId;
  int len = pSqlStmt->getParsid( &parseId );

  PIn_RequestPacket reqPacket( m_request_packet, m_request_packet_size,
                               pSqlStmt->isUnicode() );
  SQL_InitVersion(reqPacket);                         /* PTS 1112481 */
  PIn_RequestWriter reqWriter(reqPacket);

  reqWriter.Reset();
  reqWriter.InitExecute( m_sqlmode, parseId ); /* SQL-Mode PTS 1108446 */
  if (pSqlStmt->hasInputParms()) {
    part = reqWriter.AddDataPart();
    if (offset > 0) {
      for ( int i = 0; i < pSqlStmt->getSqln(); i++ ) {
        pSqlStmt->shiftSqlCols( offset, i );
      }
      pSqlStmt->createLongDescriptors(offset);
      rc = pSqlStmt->addInputParms( offset, part );
    }
    else {
      rc = pSqlStmt->addInputParms( part );
    }
    if (!rc) {
      return rc;
    }
  }
  reqWriter.Close();

  pSqlStmt->setResetParms();                  /* PTS 1109560 */
  rc = executeDBRequest( pILCSink, reqPacket );
  if (rc) {
    SQL_ReplyParser rplyParser( *this, *pSqlStmt );
    rc = rplyParser.ParseReplyData(m_reply_packet);
  }
  if (0 == m_sqlca.sqlcode) {
    if ( pSqlStmt->hasLongInput(offset) && rc ) {
      rc = executeLongInput( pILCSink, reqPacket, pSqlStmt, offset );
    }
    if ( pSqlStmt->hasLongOutput() && rc ) {
      rc = executeLongOutput( pILCSink, reqPacket, pSqlStmt );
    }
  }
  return rc;
}

/*----- PTS 1119960 ----------------------------------------------------*/

int SQL_SessionContext::executePreparedMassSQL( IliveCacheSink * pILCSink,
                                                SQL_Statement * pSqlStmt ) {
  int rc = 1;
  PIn_Part * part;
  PIn_Part * rescnt_part;
  const tin02_ParseId * parseId;
  int len = pSqlStmt->getParsid(&parseId);
  int rowcnt = 0, max_rowcnt_in_datapart = 0, reqcnt = 0,
      start_row = 0, rows_handled = 0;

  if ( pSqlStmt->getParsidOfSelectInfo1() == csp1_p_mselect_found || //pSqlStmt->getParseFlag2() == 114 || 
       pSqlStmt->getParsidInfo1()         == csp1_p_mfetch_found ) {  //pSqlStmt->getParseFlag1() == 43) {
    /* mass fetch */
    PIn_RequestPacket reqPacket( m_request_packet, m_request_packet_size,
                                 pSqlStmt->isUnicode() );
    SQL_InitVersion(reqPacket);
    PIn_RequestWriter reqWriter(reqPacket);
    reqWriter.Reset();
    reqWriter.InitMassExecute( m_sqlmode, parseId );
    rescnt_part = reqWriter.AddPart(sp1pk_resultcount);    
    rc = pSqlStmt->addResCntParm(rescnt_part,1,0);
    if (!rc) {
      return rc;
    }
    reqWriter.Close();
    pSqlStmt->setResetParms();
    rc = executeDBRequest( pILCSink, reqPacket );
    if (rc) {
      SQL_ReplyParser rplyParser( *this, *pSqlStmt );
      rc = rplyParser.ParseReplyData(m_reply_packet);
    }
    if (0 == m_sqlca.sqlcode) {                                      /* PTS 1123241 */
      if ( pSqlStmt->hasLongOutputMass() && rc ) {                   /* PTS 1123241 */
        rc = executeLongOutputMass( pILCSink, reqPacket, pSqlStmt ); /* PTS 1123241 */
      }                                                              /* PTS 1123241 */
    }                                                                /* PTS 1123241 */
  }
  else {
    /* mass select, mass insert */
    if ( pSqlStmt->hasInputParms()) {
      rowcnt = pSqlStmt->getInpColIdx() / pSqlStmt->getSqln();
      max_rowcnt_in_datapart = (m_request_packet_size - 144) / 
                               pSqlStmt->getOrigFixBufLength( );
      reqcnt = (rowcnt + max_rowcnt_in_datapart -1) / max_rowcnt_in_datapart;
      for (int i = 1; i <= reqcnt; i++) {
        PIn_RequestPacket reqPacket( m_request_packet, m_request_packet_size,
                                     pSqlStmt->isUnicode() );
        SQL_InitVersion( reqPacket );
        PIn_RequestWriter reqWriter( reqPacket );
        if (i == reqcnt) {
          max_rowcnt_in_datapart = rowcnt - rows_handled;
        }
        reqWriter.Reset();
        reqWriter.InitMassExecute ( m_sqlmode, parseId );
        rescnt_part = reqWriter.AddPart(sp1pk_resultcount);    
        if (i == 1) {
          rc = pSqlStmt->addResCntParm(rescnt_part,0,0);
        }
        else {
          rc = pSqlStmt->addResCntParm(rescnt_part,2,rows_handled);
        }
        if (!rc) {
          return rc;
        }
        part = reqWriter.AddDataPart();
        rc = pSqlStmt->addMassInputParms( part, rows_handled, max_rowcnt_in_datapart );
        if (!rc) {
          return rc;
        }
        part->AdjArgCntForMass(max_rowcnt_in_datapart);
        tsp1_part_attr attr;
        attr.clear();
        if (i == 1) {
          attr.addElement(sp1pa_first_packet);
        }
        if ( i > 1 && i < reqcnt) {
          attr.addElement(sp1pa_next_packet);
        }
        if ( i == reqcnt) {
          attr.addElement(sp1pa_last_packet);
        }
        part->setAttributes(attr);
        reqWriter.Close();
        pSqlStmt->setResetParms();
        rc = executeDBRequest( pILCSink, reqPacket );
        if ( rc ) {
          SQL_ReplyParser rplyParser( *this, *pSqlStmt );
          rc = rplyParser.ParseReplyData(m_reply_packet);
        }
        rows_handled = rows_handled + max_rowcnt_in_datapart;
        start_row = start_row + rows_handled;
      }
    }
    else {
      /* remaining commands */
      PIn_RequestPacket reqPacket( m_request_packet, m_request_packet_size,
                                   pSqlStmt->isUnicode() );
      SQL_InitVersion(reqPacket);
      PIn_RequestWriter reqWriter(reqPacket);
      reqWriter.Reset();
      reqWriter.InitMassExecute ( m_sqlmode, parseId );
      rescnt_part = reqWriter.AddPart(sp1pk_resultcount);
      rc = pSqlStmt->addResCntParm( rescnt_part, 0, 0 );
      if (!rc) {
        return rc;
      }
      reqWriter.Close();
      pSqlStmt->setResetParms();
      rc = executeDBRequest( pILCSink, reqPacket );
      if (rc) {
        SQL_ReplyParser rplyParser( *this, *pSqlStmt );
        rc = rplyParser.ParseReplyData( m_reply_packet );
      }
    }
  }

  return rc;

}

/*----------------------------------------------------------------------*/

int SQL_SessionContext::executeLongInput( IliveCacheSink* pILCSink,
                                          PIn_RequestPacket & reqPacket,
                                          SQL_Statement * pSqlStmt ) {
  int rc = 1;
  PIn_RequestWriter reqWriter(reqPacket);
  do { 
    reqWriter.Reset();
    reqWriter.AddSegment( sp1m_putval, m_sqlmode ); /* SQL-Mode PTS 1108446 */
    PIn_Part * part = reqWriter.AddPart(sp1pk_longdata);
    rc = pSqlStmt->addOpenLongDataInput(part);
    reqWriter.Close();
    rc = executeDBRequest( pILCSink, reqPacket );
    if (rc) {
      SQL_ReplyParser rplyParser( *this, *pSqlStmt );
      rc = rplyParser.ParseReplyData(m_reply_packet);
      /* PTS 1120794 */
#     if !defined (EXT_CLIENT_SQL)
        monitorRequest(m_reply_packet->sp1_segm().sp1r_function_code());
#     endif
    }
  } while ( pSqlStmt->hasLongInput() && rc );

  reqWriter.Reset();
  reqWriter.AddSegment(sp1m_putval, m_sqlmode); /* SQL-Mode PTS 1108446 */
  PIn_Part * part = reqWriter.AddPart(sp1pk_longdata);
  rc = pSqlStmt->addInputFinishLongData(part);
  reqWriter.Close();
  rc = executeDBRequest( pILCSink, reqPacket );
  if (rc) {
    SQL_ReplyParser rplyParser( *this, *pSqlStmt );
    rc = rplyParser.ParseReplyData(m_reply_packet);
    /* PTS 1120794 */
#   if !defined (EXT_CLIENT_SQL)
      monitorRequest(m_reply_packet->sp1_segm().sp1r_function_code());
#   endif
  }

  return rc;
}

/*----- PTS 1119960 ----------------------------------------------------*/

int SQL_SessionContext::executeLongInput( IliveCacheSink* pILCSink,
                                          PIn_RequestPacket & reqPacket,
                                          SQL_Statement * pSqlStmt,
                                          int offset) {
  int rc = 1;
  PIn_RequestWriter reqWriter(reqPacket);
  do { 
    reqWriter.Reset();
    reqWriter.AddSegment( sp1m_putval, m_sqlmode ); /* SQL-Mode PTS 1108446 */
    PIn_Part * part = reqWriter.AddPart(sp1pk_longdata);
    rc = pSqlStmt->addOpenLongDataInput( part, offset );
    reqWriter.Close();
    rc = executeDBRequest( pILCSink, reqPacket );
    if (rc) {
      SQL_ReplyParser rplyParser( *this, *pSqlStmt );
      rc = rplyParser.ParseReplyData(m_reply_packet);
    }
  } while ( pSqlStmt->hasLongInput(offset) && rc );

  reqWriter.Reset();
  reqWriter.AddSegment( sp1m_putval, m_sqlmode ); /* SQL-Mode PTS 1108446 */
  PIn_Part * part = reqWriter.AddPart(sp1pk_longdata);
  rc = pSqlStmt->addInputFinishLongData( part, offset );
  reqWriter.Close();
  rc = executeDBRequest( pILCSink, reqPacket );
  if (rc) {
    SQL_ReplyParser rplyParser( *this, *pSqlStmt );
    rc = rplyParser.ParseReplyData(m_reply_packet);
  }

  return rc;
}

/*----------------------------------------------------------------------*/

int SQL_SessionContext::executeLongOutput( IliveCacheSink * pILCSink,
                                           PIn_RequestPacket & reqPacket,
                                           SQL_Statement * pSqlStmt ) {
  int rc = 1;
  PIn_RequestWriter reqWriter(reqPacket);
  do { 
    reqWriter.Reset();
    reqWriter.AddSegment( sp1m_getval, m_sqlmode ); /* SQL-Mode PTS 1108446 */
    PIn_Part * part = reqWriter.AddPart(sp1pk_longdata);
    rc = pSqlStmt->addOpenLongDataOutput(part);
    reqWriter.Close();
    rc = executeDBRequest( pILCSink, reqPacket );
    if (rc) {
      SQL_ReplyParser rplyParser( *this, *pSqlStmt );
      rc = rplyParser.ParseReplyData(m_reply_packet);
      /* PTS 1120794 */
#     if !defined (EXT_CLIENT_SQL)
        monitorRequest(m_reply_packet->sp1_segm().sp1r_function_code());
#     endif
    }
  } while ( pSqlStmt->hasLongOutput() && rc );

  return rc;
}

/*----- PTS 1123241 ----------------------------------------------------*/

int SQL_SessionContext::executeNextLongOutput(SQL_Statement * pSqlStmt) {
  PIn_RequestPacket reqPacket( m_request_packet, m_request_packet_size,
                               pSqlStmt->isUnicode() );
  SQL_InitVersion(reqPacket);
  return executeLongOutputMass( m_pSink, reqPacket, pSqlStmt );
}

/*----- PTS 1123241 ----------------------------------------------------*/

int SQL_SessionContext::executeLongOutputMass( IliveCacheSink * pILCSink,
                                               PIn_RequestPacket & reqPacket,
                                               SQL_Statement * pSqlStmt ) {
  int rc = 1;
  PIn_RequestWriter reqWriter(reqPacket);
  do { 
    reqWriter.Reset();
    reqWriter.AddSegment( sp1m_getval, m_sqlmode ); /* SQL-Mode PTS 1108446 */
    PIn_Part * part = reqWriter.AddPart(sp1pk_longdata);
    rc = pSqlStmt->addOpenLongDataOutputMass(part);
    reqWriter.Close();
    rc = executeDBRequest( pILCSink, reqPacket );
    if (rc) {
      SQL_ReplyParser rplyParser( *this, *pSqlStmt );
      rc = rplyParser.ParseReplyData(m_reply_packet);
      /* PTS 1120794 */
#     if !defined (EXT_CLIENT_SQL)
        monitorRequest(m_reply_packet->sp1_segm().sp1r_function_code());
#     endif
    }
  } while ( pSqlStmt->hasLongOutputMass() && rc );

  return rc;
}

/*----------------------------------------------------------------------*/

int SQL_SessionContext::executeDBRequest( IliveCacheSink * pILCSink,
                                          PIn_RequestPacket & reqPacket) {
  unsigned char	warnset[2];
  
# if defined (EXT_CLIENT_SQL)
    tsp01_CommErr      commErr;
    tsp00_Int4         outLen;
    tsp00_ErrText      errtext;
    sqlarequest( m_connection, reqPacket.GetRawPacket(),
                 reqPacket.Length(), errtext, commErr );
    if (commErr) {
      setRtError( e_notok, (char*)"sqlarequest", NULL, NULL );
      return 0;
    }
    sqlareceive( m_connection, REINTERPRET_CAST(void**,&m_reply_packet), /* PTS 1131403 */
                 outLen, errtext, commErr );
    if (commErr) {
      setRtError( e_notok, (char*)"sqlareceive", NULL, NULL );
      return 0;
    }
# else
    m_request_packet->sp1_header.sp1h_varpart_size = 
      m_request_packet->sp1_header.sp1h_varpart_size + C_MIN_REPLY_SIZE;
    m_reply_packet = reqPacket.GetRawPacket();
    pILCSink->SQLRequest( reqPacket.Length(),
                          (unsigned char*)reqPacket.GetRawPacket() );
    m_request_packet->sp1_header.sp1h_varpart_size = 
      m_request_packet->sp1_header.sp1h_varpart_size - C_MIN_REPLY_SIZE;
# endif

  m_sqlca.sqlcode   = m_reply_packet->sp1_segm().sp1r_returncode();
  m_sqlca.sqlerrpos = m_reply_packet->sp1_segm().sp1r_errorpos();
  SAPDB_MemCopyNoCheck( m_sqlca.sqlstate, 
          &m_reply_packet->sp1_segm().sp1r_sqlstate(),
          sizeof(tsp00_SqlState) );
  SAPDB_MemCopyNoCheck( &warnset,
          &m_reply_packet->sp1_segm().sp1s_segm_header().sp1r_extern_warning(),
          sizeof(warnset) );
  if (warnset[0] != 0) {
    short i;
    char warnb = 0x01;
    for ( i = 0; i < 8; warnset[0]=warnset[0]>>1, i++ ) {
      if (warnset[0] & warnb) {
        m_sqlca.sqlwarn [i] = 'W';
      }
      else {
        m_sqlca.sqlwarn [i] = ' ';
      }
    }
    for ( i = 8; i < 16; warnset[1]=warnset[1]>>1, i++ ) {
      if (warnset[1] & warnb) {
        m_sqlca.sqlwarn [i] = 'W';
      }
      else {
        m_sqlca.sqlwarn [i] = ' ';
      }
    }
  }
  return 1;
}

/*----------------------------------------------------------------------*/

int SQL_SessionContext::executeSqlStmt(SQL_Statement * pSqlStmt) {
  IliveCacheSink * pILCSink = initSession(pSqlStmt->isUnicode());

  if (NULL != pILCSink) {

    if (true /* PTS 1111883 pSqlStmt->getNumVars() || pSqlStmt->getPreparedFlag()*/) {
      int rc = 1;
      if (!pSqlStmt->getPreparedFlag()) {
        /* PTS 1111883 rc = prepareSqlStmt( pSqlStmt ); */
        rc = pSqlStmt->prepare();  /* PTS 1111883 */
      }
      if ( ( (pSqlStmt->hasLongInput()) && 
             (pSqlStmt->getParsidInfo1() == csp1_p_mass_command /* 70 */) ) ||
           (pSqlStmt->multSingInsert()) ) {
        if ( (pSqlStmt->hasLongInput()) &&
             (pSqlStmt->getParsidInfo1() == csp1_p_mass_command /* 70 */) ) {
          pSqlStmt->removeMassCmdFlag();
          pSqlStmt->setMultiSingInsertFlag();
          rc = pSqlStmt->prepare();
        }
        if ( (rc) && 
             (csp1_p_command_executed != pSqlStmt->getParsidInfo1()) ) {
          int execCnt = pSqlStmt->getInpColIdx() / pSqlStmt->getSqln();
          for ( int i = 0; i < execCnt; i++ ) {
            executePreparedSQL( pILCSink, pSqlStmt, i * pSqlStmt->getSqln() );
            if (C_SQL_PARSE_AGAIN == m_sqlca.sqlcode) {
              m_sqlca.sqlCAInit( pSqlStmt->isUnicode() );
              if ( (NULL != pILCSink) && 
                   pSqlStmt->prepare(C_PARSING_AGAIN) ) {
                      executePreparedSQL( pILCSink, pSqlStmt,
                                          i * pSqlStmt->getSqln() );
              }
            }
          }
        }
      }
      else {
        /* 'prepared sql statement' -> request buffer  */
        if ( (rc) &&
             (csp1_p_command_executed != pSqlStmt->getParsidInfo1()) ) { /* PTS 1116114 FF 2002-06-10 */
          if (pSqlStmt->isMassCmd()) {
            executePreparedMassSQL( pILCSink, pSqlStmt );
          }
          else {
            executePreparedSQL( pILCSink, pSqlStmt );
          }
        }
      }
    }
    else { /* never passed: if (true) ... */
      /* 'immediate sql statement' -> request buffer */
      executeImmediateSQL( pILCSink, pSqlStmt );
    }

    /* +++ **** PTS 1112690 **** +++ */
    if (C_SQL_PARSE_AGAIN == m_sqlca.sqlcode) {
      m_sqlca.sqlCAInit(pSqlStmt->isUnicode());
      if ( (NULL != pILCSink) && pSqlStmt->prepare(C_PARSING_AGAIN) ) {
        if (pSqlStmt->isMassCmd()) {
          executePreparedMassSQL( pILCSink, pSqlStmt );
        }
        else {
          executePreparedSQL( pILCSink, pSqlStmt );
        }
      }
    }
    /* --- **** PTS 1112690 **** --- */

  }

  if ( (m_sqlhandler) && (0 != m_sqlca.sqlcode) ) {    /* PTS 1120794 */
    m_sqlhandler(&m_sqlhandle);                        /* PTS 1120794 */
  }
  return (m_sqlca.sqlcode == 0);
}

/*----- PTS 1119960 ----------------------------------------------------*/

int SQL_SessionContext::executeMassSqlStmt(SQL_Statement * pSqlStmt) {
  pSqlStmt->setMassCmdFlag();
  return executeSqlStmt(pSqlStmt);
}

/*----------------------------------------------------------------------*/

/* PTS 1112690 parsingAgain added */
int SQL_SessionContext::prepareSqlStmt( SQL_Statement * pSqlStmt,
                                        const pasbool parsingAgain ) {
  bool do_parsing = true;     /* PTS 1112690  */
  pasbool l_parsingAgain = parsingAgain;  /* PTS 1112690  */
  
  IliveCacheSink * pILCSink = initSession(pSqlStmt->isUnicode());
  if (NULL != pILCSink) {
    /* PTS 1112690 with 7.4.2.00 the parseid is dropped by the kernel   * 
     *                                              when -8 is signaled */
    if ( pSqlStmt->getPreparedFlag() && !parsingAgain ) {
      dropParsid(pSqlStmt);
    }
    if (!pSqlStmt->dropLongDescriptors()) {
      return 0;
    }

    while (do_parsing) {         /* PTS 1112690  */

      PIn_RequestPacket reqPacket( m_request_packet, m_request_packet_size,
                                   pSqlStmt->isUnicode() );
      SQL_InitVersion(reqPacket);          /* PTS 1112481 */
      PIn_RequestWriter reqWriter(reqPacket);
      if (ECO521_FETCH_DESCRIBE == pSqlStmt->getStmtType()) {
        reqWriter.Reset();
        /* SQL-Mode PTS 1108446 & PTS 1112690 parsingAgain added */
        reqWriter.InitDbs ( m_sqlmode, "DESCRIBE", -1, l_parsingAgain );
        reqWriter.Close();
        if (executeDBRequest( pILCSink, reqPacket )) {
#         if !defined (EXT_CLIENT_SQL)     /* PTS 1120794 */
            m_monitor.IncPrepare();        /* PTS 1120794 */
#         endif                            /* PTS 1120794 */
          SQL_ReplyParser rplyParser( *this, *pSqlStmt );
          rplyParser.ParseReplyData(m_reply_packet);  
        }
        if (!pSqlStmt->dropLongDescriptors()) {
          return 0;
        }
      }
      
      reqWriter.Reset();
      /* SQL-Mode PTS 1108446 & PTS 1112690 parsingAgain added */
      if (pSqlStmt->isUnicode()) {
        /* PTS 1117853 FF 2002-Sep-16 */
        if (NULL == reqWriter.InitParse( m_sqlmode,
                                         pSqlStmt->getUniStatement(),
                                         -1, l_parsingAgain) ) {
          setRtError( e_cmd_too_long, NULL, NULL, NULL );
          return 0;
        }
      }
      else {
        /* PTS 1117853 FF 2002-Sep-16 */
        // BEGIN: MassCmd2
        if (pSqlStmt->isMassCmd()) {
          if (NULL == reqWriter.InitMassParse( m_sqlmode,
                                               pSqlStmt->getStatement(),
                                               -1, l_parsingAgain) ) {
            setRtError( e_cmd_too_long, NULL, NULL, NULL );
            return 0;
          }
        }
        else {
        // END: MassCmd2
          if (NULL == reqWriter.InitParse( m_sqlmode,
                                           pSqlStmt->getStatement(),
                                           -1, l_parsingAgain) ) {
            setRtError( e_cmd_too_long, NULL, NULL, NULL );
            return 0;
          } 
        } // else MassCmd2
      }
      reqWriter.Close();

      pSqlStmt->setResetParms();                  /* PTS 1109560 */
      if (executeDBRequest( pILCSink, reqPacket )) {
        /* PTS 1120794 */
#       if !defined (EXT_CLIENT_SQL)
          m_monitor.IncPrepare();
#       endif
        SQL_ReplyParser rplyParser( *this, *pSqlStmt );
        rplyParser.ParseReplyData(m_reply_packet);
      }
      pSqlStmt->setOrigFixBufLength(pSqlStmt->getFixBufLength());
      pSqlStmt->createLongDescriptors();
      /* +++ **** PTS 1112690 **** +++ */
      if ( C_SQL_PARSE_AGAIN == m_sqlca.sqlcode && !l_parsingAgain ) {
        l_parsingAgain = true;
      }
      else {
        do_parsing = false;
      }
      /* --- **** PTS 1112690 **** --- */

    } /* while (do_parsing) */

  } /* if (NULL != pILCSink) */
  
  if ( (m_sqlhandler) && ( 0 != m_sqlca.sqlcode) ) {    /* PTS 1120794 */
    m_sqlhandler(&m_sqlhandle);                         /* PTS 1120794 */
  }
  return (m_sqlca.sqlcode == 0);
}

/*----------------------------------------------------------------------*/

void * SQL_SessionContext::allocate(size_t sz) {
  return m_heap.allocate(sz);
}

/*----------------------------------------------------------------------*/

void SQL_SessionContext::deallocate(void * p) {
  m_heap.deallocate(p);
}

/*----------------------------------------------------------------------*/

int SQL_SessionContext::dropParsid(SQL_Statement * pSqlStmt) {
  /* drop parseid MUST NOT destroy previous error */
  structSqlCa ca_save = m_sqlca;   // PTS 1111883
  int rc = 0;
  if ( pSqlStmt->getPreparedFlag() && (!m_deleteSelf) &&
       (csp1_p_command_executed != pSqlStmt->getParsidInfo1()) ) { // PTS 1116114 FF 2002-06-10)
    IliveCacheSink * pILCSink =  m_pSink; /* PTS 1122076 */
    if (NULL == pILCSink) {
      m_sqlca = ca_save;          // PTS 1111883
      return 0;
    }

    PIn_RequestPacket reqPacket( m_request_packet, m_request_packet_size,
                                 pSqlStmt->isUnicode() );
    SQL_InitVersion(reqPacket);             /* PTS 1112481 */
    PIn_RequestWriter reqWriter(reqPacket);

    reqWriter.Reset();
    reqWriter.InitDbs( m_sqlmode, "DROP PARSEID" ); /* SQL-Mode PTS 1108446 */
    PIn_Part * part = reqWriter.AddPart(sp1pk_parsid);
    const tin02_ParseId * data;
    int len = pSqlStmt->getParsid(&data);
    part->AddArgument( REINTERPRET_CAST(const void *,data), len );  
    reqWriter.Close();
    rc = executeDBRequest( pILCSink, reqPacket );
    if (rc) {
      /* PTS 1120794 */
#     if !defined (EXT_CLIENT_SQL)
        m_monitor.IncDropParseid();
#     endif
      SQL_ReplyParser rplyParser( *this, *pSqlStmt );
      rc = rplyParser.ParseReplyData(m_reply_packet);
    }
  }

    m_sqlca = ca_save;           // PTS 1111883
    return rc;
}

/*----------------------------------------------------------------------*/

void SQL_SessionContext::setRtError( tgg00_BasisError e, const char * p1,
                                     const char * p2, const char * p3 ) {
  const char * msg = getErrorDesc(e);
  m_sqlca.sqlcode = e;
  m_sqlca.sqlerrml  = (short)strlen(msg);
  SAPDB_MemCopyNoCheck( m_sqlca.sqlerrmc, msg, m_sqlca.sqlerrml );

  if (p1) {
    if ((m_sqlca.sqlerrml + 1 + strlen(p1)) < sizeof(m_sqlca.sqlerrmc)) {
      strcat( m_sqlca.sqlerrmc, "\255" );
      strcat( m_sqlca.sqlerrmc, p1 );
      m_sqlca.sqlerrml = m_sqlca.sqlerrml + 1 + (short)strlen(p1);
    }
  }

  if (p2) {
    if ((m_sqlca.sqlerrml + 1 + (short)strlen(p2)) < 
        sizeof(m_sqlca.sqlerrmc)) {
      strcat( m_sqlca.sqlerrmc, "\255" );
      strcat( m_sqlca.sqlerrmc, p2 );
      m_sqlca.sqlerrml = m_sqlca.sqlerrml + 1 + (short)strlen(p1);
    }
  }

  if (p3) {
    if ((m_sqlca.sqlerrml + 1 + (short)strlen(p3)) < 
        sizeof(m_sqlca.sqlerrmc)) {
      strcat( m_sqlca.sqlerrmc, "\255" );
      strcat( m_sqlca.sqlerrmc, p3 );
      m_sqlca.sqlerrml = m_sqlca.sqlerrml + 1 + (short)strlen(p1);
    }
  }
}

/*----------------------------------------------------------------------*/

void SQL_SessionContext::setError( tgg00_BasisError e, const char * msg ) {
  m_sqlca.sqlcode = e;
  m_sqlca.sqlerrml = (short)strlen(msg);
  SAPDB_MemCopyNoCheck( m_sqlca.sqlerrmc, msg, m_sqlca.sqlerrml );
}

/*----------------------------------------------------------------------*/

void SQL_SessionContext::setErrortext( int argCount, int len,
                                       const char * data ) {
  unsigned int   converted;
  int            UCS2Swap;
  tsp81_UCS2Char UCS2Blank;
  UCS2Blank.s = 32;
  UCS2Swap = (UCS2Blank.c[0] ? 1 : 0); 

  if (m_sqlca.sqlerrmt) {
    if (sizeof(m_sqlca.sqlerrmc) <= len) {
      len = sizeof(m_sqlca.sqlerrmc) - 2;
    }
    SAPDB_MemCopyNoCheck( m_sqlca.sqlerrmuc, data, len );
    m_sqlca.sqlerrml  = len / 2;
    sp81UCS2toASCII( (tsp00_Byte*)(m_sqlca.sqlerrmc),
                     sizeof(m_sqlca.sqlerrmc)-1, &converted,
                     (tsp81_UCS2Char*)data, len/2, UCS2Swap );
  }
  else {
    if (sizeof(m_sqlca.sqlerrmc) <= len) {
      len = sizeof(m_sqlca.sqlerrmc) - 1;
    }
    m_sqlca.sqlerrml = len;
    SAPDB_MemCopyNoCheck( m_sqlca.sqlerrmc, data, len );
    sp81ASCIItoUCS2( (tsp81_UCS2Char*)(m_sqlca.sqlerrmuc), len , UCS2Swap,
                     &converted, (const unsigned char*)data, len );
  }
}

/*----------------------------------------------------------------------*/

void SQL_SessionContext::setResultCount(int resultCnt) {
  m_sqlca.sqlrescount = resultCnt;
}

/*----------------------------------------------------------------------*/

void SQL_SessionContext::setWarnTrunc( ) {
  m_sqlca.sqlwarn[0] = 'W';
  m_sqlca.sqlwarn[1] = 'W';
}

/*----------------------------------------------------------------------*/

short SQL_SessionContext::sqlCode() const {
  return m_sqlca.sqlcode;
}

/*----------------------------------------------------------------------*/

const char * SQL_SessionContext::sqlErrorMsg() const {
  return &m_sqlca.sqlerrmc[0];
}

/*----------------------------------------------------------------------*/

int SQL_SessionContext::sqlIsUnicodeErrMsg() const {
  return m_sqlca.sqlerrmt;
}

/*----------------------------------------------------------------------*/

const DbpTypeUnicode * SQL_SessionContext::sqlUnicodeErrorMsg() const {
  return &m_sqlca.sqlerrmuc[0];
}

/*----------------------------------------------------------------------*/

const char * SQL_SessionContext::sqlState() const {
  return &m_sqlca.sqlstate[0];
}

/*----------------------------------------------------------------------*/

char SQL_SessionContext::sqlWarning(int i) const {
  return m_sqlca.sqlwarn[i];
}

/*----------------------------------------------------------------------*/

long SQL_SessionContext::sqlErrorPos() const {
  return m_sqlca.sqlerrpos;
}

/*----------------------------------------------------------------------*/

long SQL_SessionContext::sqlResultCount() const {
  return m_sqlca.sqlrescount;
}

/*----- PTS 1120794 ----------------------------------------------------*/

void SQL_SessionContext::setHandler(sqlHandler p) {
  m_sqlhandler = p;
}

/*----- PTS 1120794 ----------------------------------------------------*/

sqlHandler SQL_SessionContext::getHandler() {
  return m_sqlhandler;
}

#if !defined (EXT_CLIENT_SQL)

/*----- PTS 1122076 ----------------------------------------------------*/

IliveCacheSink * SQL_SessionContext::getSink() {
  return m_pSink;
}

/*----- PTS 1122076 ----------------------------------------------------*/

void SQL_SessionContext::setSink(IliveCacheSink * pSink) {
  m_pSink = pSink;
}

/*----- PTS 1121342 ----------------------------------------------------*/

bool SQL_SessionContext::isUnicodeInstance() {
  return m_isUnicodeInstance;
}

/*----- PTS 1120794 ----------------------------------------------------*/

void SQL_SessionContext::monitorRequest(int fc) {
  switch (fc) {
    case csp1_insert_fc :
      m_monitor.IncInsert();
      if (m_data_len > 0) {
        m_monitor.AddInsertDataLen(m_data_len);
        m_data_len = 0;
      }
      break;
    case csp1_select_fc :
      m_monitor.IncSelect();
      break;
    case csp1_update_fc :
      m_monitor.IncUpdate();
      if (m_data_len > 0) {
        m_monitor.AddUpdateDataLen(m_data_len);
        m_data_len = 0;
      }
      break;
    case csp1_delete_fc :
      m_monitor.IncDelete();
      if (m_data_len > 0) {
        m_monitor.AddDeleteDataLen(m_data_len);
        m_data_len = 0;
      }
      break;
    case csp1_select_direct_fc :
      m_monitor.IncSelect();
      break;
    case csp1_select_first_fc :
      m_monitor.IncSelect();
      break;
    case csp1_select_last_fc :
      m_monitor.IncSelect();
      break;
    case csp1_select_next_fc :
      m_monitor.IncSelect();
      break;
    case csp1_select_prev_fc :
      m_monitor.IncSelect();
      break;
    case csp1_fetch_first_fc :
      m_monitor.IncFetch();
      if (m_data_len > 0) {
        m_monitor.AddFetchDataLen(m_data_len);
        m_data_len = 0;
      }
      break;
    case csp1_fetch_last_fc :
      m_monitor.IncFetch();
      if (m_data_len > 0) {
        m_monitor.AddFetchDataLen(m_data_len);
        m_data_len = 0;
      }
      break;
    case csp1_fetch_next_fc :
      m_monitor.IncFetch();
      if (m_data_len > 0) {
        m_monitor.AddFetchDataLen(m_data_len);
        m_data_len = 0;
      }
      break;
    case csp1_fetch_prev_fc :
      m_monitor.IncFetch();
      if (m_data_len > 0) {
        m_monitor.AddFetchDataLen(m_data_len);
        m_data_len = 0;
      }
      break;
    case csp1_fetch_pos_fc :
      m_monitor.IncFetch();
      if (m_data_len > 0) {
        m_monitor.AddFetchDataLen(m_data_len);
        m_data_len = 0;
      }
      break;
    case csp1_fetch_same_fc :
      m_monitor.IncFetch();
      if (m_data_len > 0) {
        m_monitor.AddFetchDataLen(m_data_len);
        m_data_len = 0;
      }
      break;
    case csp1_select_into_fc :
      m_monitor.IncSelect();
      break;
    case csp1_fetch_relative_fc :
      m_monitor.IncFetch();
      break;
    default :
      m_monitor.IncOther();
  }
}

/*----- PTS 1120794 ----------------------------------------------------*/

void SQL_SessionContext::incRefCnt() {
  ++m_ref_count;
}

/*----- PTS 1120794 ----------------------------------------------------*/

void SQL_SessionContext::decRefCnt() {
  --m_ref_count;
}

/*----- PTS 1120794 ----------------------------------------------------*/

SQL_Monitor *  SQL_SessionContext::getMonitor() {
  return &m_monitor;
}

/*----- PTS 1120794 ----------------------------------------------------*/

void SQL_SessionContext::setMethodCallEpilog() {
  m_inMethodCallEpilog = true;
}

/*----- PTS 1120794 ----------------------------------------------------*/

void SQL_SessionContext::resetMethodCallEpilog() {
  m_inMethodCallEpilog = false;
}

/*----- PTS 1120794 ----------------------------------------------------*/

void SQL_SessionContext::setCurrMethod(unsigned char * name) {
  SAPDB_MemCopyNoCheck( &m_curr_method[0], name, sizeof(tsp00_KnlIdentifier) );
}

/*----- PTS 1120794 ----------------------------------------------------*/

void SQL_SessionContext::resetCurrMethod() {
  memset( &m_curr_method[0], ' ', sizeof(tsp00_KnlIdentifier) );
}

/*----- PTS 1120794 ----------------------------------------------------*/

bool SQL_SessionContext::getMethodCallMonitorInfo(SqlMonitorData & info) const {
  if (m_inMethodCallEpilog) {
    m_monitor.GetCallStatistics(info);
    SAPDB_MemCopyNoCheck( &info.m_MethodName[0], m_curr_method[0], sizeof(info.m_MethodName) );
    return true;
  }
  return false;
}

/*----- PTS 1120794 ----------------------------------------------------*/

void SQL_SessionContext::setDataLen(int DataLen) {
  m_data_len = DataLen;
}

#endif

/*----- PTS 1123241 ----------------------------------------------------*/

int SQL_SessionContext::getRequestPacketSize() {
  return m_request_packet_size;
}

/*----- PTS 1133793 ----------------------------------------------------*/

int SQL_SessionContext::getReplyPacketSize() {
# if defined (EXT_CLIENT_SQL)
    return m_reply_packet_size;
# else
    return m_request_packet_size;
# endif
}

/*----- PTS 1130629 ----------------------------------------------------*/

const SQL_Statement * SQL_SessionContext::getSqlStmtFromDirectory(const tin02_ParseId * parseId) {
  SQL_Statement * stmt = NULL;
  const tin02_ParseId * parseId2 = NULL;
  int len = 0;

  for ( cgg251dclIterator<SQL_Statement*,SessionHeap> iter = m_CSqlStmtDir.begin();
        iter; ++iter ) {
    stmt = *iter();
    len = stmt->getParsid(&parseId2);
    if (memcmp( parseId, parseId2, len )) {
      break;
    }
  }

  return stmt;
}

#if !defined (EXT_CLIENT_SQL)

/*----- PTS 1130629 ----------------------------------------------------*/

int SQL_SessionContext::getCurrMethodAscii( char * name, int & len ) {
  int rc = 0;
  char * name2 = name;
  int len2 = 0;

  if (m_hasUnicodeCatalog) {
    tsp00_Uint4           destBytesWritten;
    tsp00_Uint4           srcBytesParsed;
    tsp78ConversionResult error;
    error = sp78convertString( sp77encodingAscii,            /* destEndcoding           */
                               name,                         /* destBuffer              */
                               len,                          /* destBufferLengthInBytes */
                               &destBytesWritten,            /* destBytesWritten        */
                               false,                        /* addZeroTerminator       */
                               sp77encodingUCS2,             /* srcEncoding             */
                               &m_curr_method[0],            /* srcBuffer               */
                               sizeof(tsp00_KnlIdentifier),  /* srcBufferLengthInBytes  */
                               &srcBytesParsed );            /* srcBytesParsed          */
    if (sp78_Ok == error) {
      len = 0;
      len2 = destBytesWritten;
      for ( int i = 0; i < len2; i++ ) {
        if (' ' == *name2) {
          break;
        }
        else {
          len++;
          name2++;
        }
      }
      rc = 1;
    }
  }
  else {
    rc = 1;
  }

  return rc;

}

# endif

