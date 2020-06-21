/*!
 * \file    SQL_ReplyParser.cpp
 * \author  PeterG (GoertzP)
 * \brief   C++ SQL Class: Reply parser
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

#ifndef SQL_REPLYPARSER_HPP
#include "CppSQL/SQL_ReplyParser.hpp"
#endif

#ifndef SQL_SESSIONCONTEXT_HPP
#include "CppSQL/SQL_SessionContext.hpp"
#endif

#ifndef SQL_STATEMENT_HPP
#include "CppSQL/SQL_Statement.hpp"
#endif


/************************************************************************/
/* Implementation of class SQL_ReplyParser                              */
/************************************************************************/

SQL_ReplyParser::SQL_ReplyParser( SQL_SessionContext & sessionContext,
                                  SQL_Statement & stmt ) :
m_sessCtxt(sessionContext), m_stmt(stmt) {
}

/*----------------------------------------------------------------------*/

int SQL_ReplyParser::ParseReplyData(tsp1_packet * m_reply_packet) {
  PIn_ReplyPacket rplyPacket(m_reply_packet);
  PIn_ReplyParser::AcceptKind  accepted;
  PIn_ReplySegment segment = rplyPacket.GetFirstSegment();
  accepted = unknown_e;

  while ( (segment.IsValid()) && (accepted != abort_e) ) {
    accepted = PIn_ReplyParser::ParseReply(segment);
    segment = rplyPacket.GetNextSegment(segment);
  }
  return (accepted != abort_e);
}

/*----------------------------------------------------------------------*/

PIn_ReplyParser::AcceptKind
SQL_ReplyParser::HandleData( int argCount, int dataLen,
                             const teo00_Byte * dataPtr ) {
  if (m_stmt.isMassCmd()) { /* PTS 1123241 */
    m_stmt.getOutputParmsMass( argCount, dataLen, dataPtr );
  }
  else {
    m_stmt.getOutputParms( argCount, dataLen, dataPtr );
  }
# if !defined(EXT_CLIENT_SQL)           /* PTS 1120794 */
    m_sessCtxt.setDataLen(dataLen);     /* PTS 1120794 */
# endif                                 /* PTS 1120794 */
  return PIn_ReplyParser::handled_e;
}

/*----------------------------------------------------------------------*/

PIn_ReplyParser::AcceptKind
SQL_ReplyParser::HandleErrortext( int argCount, int len,
                                  const char * data) {
  m_sessCtxt.setErrortext( argCount, len, data );
  return PIn_ReplyParser::handled_e;
}

/*----------------------------------------------------------------------*/

PIn_ReplyParser::AcceptKind
SQL_ReplyParser::HandleLongdata( int argCount, int dataLen,
                                 const tsp00_Byte * dataPtr) {
  if (m_stmt.isMassCmd()) { /* PTS 1123241 */
//    if (dataLen == argCount * (int)(sizeof(tsp00_LongDescriptor) + 1)) {
//      m_stmt.getLongDescMass( argCount, dataLen, dataPtr );
//    }
//    else {
      m_stmt.getLongDescAndValueMass( argCount, dataLen, dataPtr );
//    }
  }
  else {
    if (dataLen == argCount * (int)(sizeof(tsp00_LongDescriptor) + 1)) {
      m_stmt.getLongDesc ( argCount, dataLen, dataPtr );
    }
    else {
      m_stmt.getLongDescAndValue ( argCount, dataLen, dataPtr );
    }
  }
  return PIn_ReplyParser::handled_e;
}

/*----------------------------------------------------------------------*/

PIn_ReplyParser::AcceptKind
SQL_ReplyParser::HandleParsid( int argCount, int len,
                               const tin02_ParseId * data) {
  m_stmt.setParsid( len, data );
  return PIn_ReplyParser::handled_e;
}

/*----------------------------------------------------------------------*/

PIn_ReplyParser::AcceptKind 
SQL_ReplyParser::HandleParsidOfSelect( int argCount, int len,
                                       const tin02_ParseId * data) {
  m_stmt.setParsidOfSelect( len, data );
  return PIn_ReplyParser::handled_e;
}

/*----------------------------------------------------------------------*/

PIn_ReplyParser::AcceptKind
SQL_ReplyParser::HandleResultcount( int argCount, int len, long data ) {
  m_sessCtxt.setResultCount(data);
  return PIn_ReplyParser::handled_e;
}

/*----------------------------------------------------------------------*/

PIn_ReplyParser::AcceptKind
SQL_ReplyParser::HandleResulttablename( int argCount, int len,
                                        const char * data ) {
  m_stmt.setResultTablename( len, data );
  return PIn_ReplyParser::handled_e;
}

/*----------------------------------------------------------------------*/

PIn_ReplyParser::AcceptKind
SQL_ReplyParser::HandleShortinfo( int argCount, int len,
                                  const tsp1_param_info * data ) {
  m_stmt.setSqlDA_sqln(argCount);
  for ( int ix=0; ix < argCount; ix++ ) {
    m_stmt.setSqlDA_sfi( ix, data + ix );
  }
  return PIn_ReplyParser::handled_e;
}

