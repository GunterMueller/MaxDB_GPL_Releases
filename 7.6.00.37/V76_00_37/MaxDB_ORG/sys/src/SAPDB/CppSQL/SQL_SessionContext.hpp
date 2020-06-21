/*!
 * \file    SQL_SessionContext.hpp
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


#ifndef SQL_SESSIONCONTEXT_HPP
#define SQL_SESSIONCONTEXT_HPP


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#ifndef VSP001_H
#include "vsp001.h"
#endif

#ifndef GGG00_H
#include "ggg00.h"
#endif

#ifndef __OMS_DEFINES_HPP
#include "Oms/OMS_Defines.h"
#endif

#include "liveCache/LVC_IliveCacheSink.hpp"

#ifndef GGG251_H
#include "ggg251.h"
#endif

#ifndef SQL_OPTION_HPP
#include "CppSQL/SQL_Option.hpp"
#endif

#ifndef SQL_ERRORHANDLER_HPP
#include "CppSQL/SQL_ErrorHandler.hpp"
#endif

#ifndef __vco03_h__
#include "livecachetypes.h" /* no check*/
#endif

#ifndef HSP81_H
#include "hsp81.h"
#endif

#if !defined (EXT_CLIENT_SQL)
#include "Oms/OMS_Globals.hpp"
#include "liveCache/LVC_KernelInterface.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_IBlockAllocator.hpp"
#include "SAPDBCommon/MemoryManagement/SAPDBMem_RawAllocator.hpp"
#include "CppSQL/SQL_Monitor.hpp" /* PTS 1120794 */
#endif

/* PTS 1122076 */
#ifndef SQL_HANDLE_HPP
#include "CppSQL/SQL_Handle.hpp"
#endif

/* PTS 1130629 */
#ifndef PIN_H
#include "PacketInterface/PIn.h"
#endif


/************************************************************************/
/* Forward declarations                                                 */
/************************************************************************/

class SQL_Statement;
class PIn_RequestPacket;
struct IliveCacheSink;
class SQL;


/************************************************************************/
/* Type definitions                                                     */
/************************************************************************/

typedef void (__cdecl * sqlHandler ) ( SqlHandle* p_sqlhandle ); /* PTS 1120794 */

struct structSqlCa {
  structSqlCa()	{
    this->sqlCAInit();
  }
  void sqlCAInit(const unsigned char reqType = '\0') {
    sqlrescount = 0;
    sqlerrpos   = 0;
    sqlcode     = 0;
    memset(sqlwarn,' ',sizeof(sqlwarn));
    sqlerrml    = 0;
    sqlerrmt    = reqType;
    memset(sqlerrmc,'\0',sizeof(sqlerrmc));
    memset(sqlerrmuc,'\0',sizeof(sqlerrmuc));
    memset(sqlstate,' ',sizeof(sqlstate) - 1);
    sqlstate[sizeof(sqlstate) - 1] = '\0';
  };
  int            sqlrescount;
  int            sqlerrpos;
  short          sqlcode;
  char           sqlwarn[16];
  short          sqlerrml;
  unsigned char  sqlerrmt;           // 0=Ascii; 1=Unicode 
  char           sqlerrmc[81];       // 0 terminated char
  DbpTypeUnicode sqlerrmuc[162];     // 0x0000 terminated char
  char           sqlstate[6];        // 0 terminated
};


/************************************************************************/
/* Class SQL_SessionContext                                             */
/************************************************************************/

class SQL_SessionContext : public SQL_ErrorHandler {
private :
  class SessionHeap {
# if !defined (EXT_CLIENT_SQL)
    private:
      SAPDBMem_RawAllocator m_alloc;
# endif
    public:
      SessionHeap(tsp00_TaskId);
      ~SessionHeap();
      void * allocate(size_t sz) {
#       if defined (EXT_CLIENT_SQL)
          return malloc(sz);
#       else
#         if defined (OMSTST)
            co10_GetKernelInterface()->TestBadAlloc();
#         endif
          return m_alloc.Allocate(sz);
#       endif
      }
      void deallocate(void * p) {
#       if defined (EXT_CLIENT_SQL)
          free(p);
#       else
          m_alloc.Deallocate(p);
#       endif
      }
  };
  SessionHeap            m_heap;
  cgg251DCList<SQL_Statement*,SessionHeap> m_CSqlStmtDir;
  SqlHandle              m_sqlhandle; /* PTS 1122076 */
  IliveCacheSink *       m_pSink;     /* PTS 1122076 */
  structSqlCa            m_sqlca;
  tsp00_Int4             m_request_packet_size;
  tsp1_packet *          m_request_packet;
  tsp00_Int4             m_reply_packet_size; /* PTS 1133793 */
  tsp1_packet *          m_reply_packet;
  tsp1_sqlmode           m_sqlmode;
  boolean                m_deleteSelf;
  int                    m_ref_count;          /* PTS 1120794 */
  sqlHandler             m_sqlhandler;         /* PTS 1120794 */
# if !defined(EXT_CLIENT_SQL)
  bool                   m_hasUnicodeCatalog;  /* PTS 1130629 */
  bool                   m_isUnicodeInstance;  /* PTS 1121342 */
  int                    m_data_len;           /* PTS 1120794 */
  boolean                m_inMethodCallEpilog; /* PTS 1120794 */
  SQL_Monitor            m_monitor;            /* PTS 1120794 */
  tsp00_KnlIdentifier *  m_curr_method;        /* PTS 1120794 */
# endif
# if defined (EXT_CLIENT_SQL)
  int                    m_connection;
  //  char*                 m_hostname;
  //  char*                 m_dbname;
# endif
  
  IliveCacheSink * initSession(const unsigned char reqType = '\0');
  int executeDBRequest( IliveCacheSink* pILCSink,
                        PIn_RequestPacket & reqPacket );
  int executeImmediateSQL( IliveCacheSink * pILCSink,
                           SQL_Statement * pSqlStmt );
  int executePreparedSQL( IliveCacheSink * pILCSink,
                          SQL_Statement * pSqlStmt );
  int executePreparedSQL( IliveCacheSink * pILCSink,
                          SQL_Statement * pSqlStmt, int offset ); // PTS 1119960
  int executePreparedMassSQL( IliveCacheSink * pILCSink,
                              SQL_Statement * pSqlStmt ); // PTS 1119960
  int executeLongInput( IliveCacheSink * pILCSink,
                        PIn_RequestPacket & reqPacket,
                        SQL_Statement * pSqlStmt );
  int executeLongInput( IliveCacheSink * pILCSink,
                        PIn_RequestPacket & reqPacket,
                        SQL_Statement * pSqlStmt, int offset );  // PTS 1119960
  int executeLongOutput( IliveCacheSink * pILCSink,
                         PIn_RequestPacket &reqPacket,
                         SQL_Statement * pSqlStmt );
  int executeLongOutputMass( IliveCacheSink * pILCSink,     /* PTS 1123241 */
                             PIn_RequestPacket & reqPacket, /* PTS 1123241 */
                             SQL_Statement * pSqlStmt );    /* PTS 1123241 */
# if !defined (EXT_CLIENT_SQL) /* PTS 1120794 */
  void monitorRequest(int fc); /* PTS 1120794 */
# endif                        /* PTS 1120794 */

public:
  SQL_SessionContext(SqlHandle & pHandle, tsp00_TaskId taskId);
  ~SQL_SessionContext();
  void   createSqlStmt( SQL & sql, const char * stmt );
  void   createSqlStmt( SQL & sql, const DbpTypeUnicode * stmt );
  void   dropSqlStmt(SQL_Statement * pSqlStmt);
  int    executeSqlStmt(SQL_Statement * pSqlStmt);
  int    executeMassSqlStmt(SQL_Statement * pSqlStmt); // PTS 1119960
  int    prepareSqlStmt( SQL_Statement * pSqlStmt,
                         const pasbool parsingAgain = false );
  void * operator new (size_t sz);
  void   operator delete (void * p);
# if defined (EXT_CLIENT_SQL)
  int    connect( SQL_Statement * pSqlStmt, const char * hostname,
                  const char * dbname, const char * user,
                  const char * pwd, const char * isolevel );
# endif
  void   sqlSetMode(const SqlOpt::SqlSessionMode mode);
  void * allocate(size_t sz);
  void   deallocate(void * p);
  void   deleteSelf( );
  int    dropParsid(SQL_Statement * pSqlStmt);
  void   setErrortext( int argCount, int len, const char * data );
  void   setResultCount(int data);
  void   setWarnTrunc();
  short  sqlCode() const;
  char   sqlWarning(int) const;
  long   sqlErrorPos () const;
  long   sqlResultCount() const;
  const  char * sqlErrorMsg() const;
  int    sqlIsUnicodeErrMsg() const;
  const  DbpTypeUnicode * sqlUnicodeErrorMsg() const;
  const  char * sqlState() const;
  void   setRtError( tgg00_BasisError e, const char * p1,
                     const char * p2, const char * p3 );
  void   setError( tgg00_BasisError e, const char * p1 );
  void   setHandler(sqlHandler p);                              /* PTS 1120794 */
  sqlHandler getHandler();                                      /* PTS 1120794 */
# if !defined (EXT_CLIENT_SQL)                                  /* PTS 1120794 */
  IliveCacheSink * getSink();                                   /* PTS 1122076 */
  void             setSink(IliveCacheSink * pSink);             /* PTS 1122076 */
  bool   isUnicodeInstance();                                   /* PTS 1121342 */
  void   incRefCnt();                                           /* PTS 1120794 */
  void   decRefCnt();                                           /* PTS 1120794 */
  SQL_Monitor * getMonitor();                                   /* PTS 1120794 */
  void   setMethodCallEpilog();                                 /* PTS 1120794 */
  void   resetMethodCallEpilog();                               /* PTS 1120794 */
  void   setCurrMethod(unsigned char* name);                    /* PTS 1120794 */
  void   resetCurrMethod();                                     /* PTS 1120794 */
  bool   getMethodCallMonitorInfo(SqlMonitorData & info) const; /* PTS 1120794 */
  void   setDataLen(int DataLen);                               /* PTS 1120794 */
# endif                                                         /* PTS 1120794 */
  int    executeNextLongOutput(SQL_Statement * pSqlStmt); /* PTS 1123241 */
  int    getRequestPacketSize();                          /* PTS 1123241 */
  int    getReplyPacketSize();                            /* PTS 1133793 */
  const SQL_Statement * getSqlStmtFromDirectory(const tin02_ParseId * parseId); /* PTS 1130629 */
  SqlHandle * getSqlHandle() {return &m_sqlhandle;}                             /* PTS 1130629 */
# if !defined (EXT_CLIENT_SQL) 
  tsp00_KnlIdentifier * getCurrMethod() {return m_curr_method;}                 /* PTS 1130629 */
  int                   getCurrMethodAscii( char * name, int & len );           /* PTS 1130629 */
# endif
};


#endif /* SQL_SESSIONCONTEXT_HPP */

