/*!
  @file           CrashMe.cpp
  @author         D025086
  @ingroup        Tests
  @brief          Stress test utility.
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



\endif
*/
#include <stdlib.h>
#include "Interfaces/tests/CrashMe.h"
#include "SAPDBCommon/SAPDB_ReuseLib.h"
#include "SAPDB/Interfaces/Runtime/IFR_Datatypes.h"
#define private public
#include "SAPDB/Interfaces/Runtime/IFR_PreparedStmt.h"
#include "SAPDB/Interfaces/Runtime/IFR_Connection.h"

class CrashMe_Check {
public:
  CrashMe_Check(IFR_Connection *connection);
  bool CheckConnection();
  void BreakConnection();
  IFR_Connection *Reconnect(char *hostname, char *database, char *username, char *password);
private:
  IFR_Connection *m_conn;
  IFR_PreparedStmt *m_stmt;
  IFR_UInt8 m_stamp;
  IFR_String m_databasename;
  IFR_String m_hostname;
  IFR_String m_username;
};

void CrashMe_Check::BreakConnection()
{
   IFRPacket_RequestPacket& rp = *m_conn->m_requestpacket;
   tsp1_packet *packet = rp.GetRawPacket ();
   if (!packet)
     return;
   tsp1_packet_header *header = &packet->sp1_header;
   if (!header)
     return;
   /* save the content of the packet */
   int len = header->sp1h_varpart_size;
   void *p = malloc(len);
  memcpy(p, packet, len);

  /* close the connection */
  rp.Reset();
  PIn_RequestSegment segment = rp.AddSegment(sp1m_dbs, sp1sm_internal);
  PIn_Part part = segment.AddPart (sp1pk_command);
  part.AddArgument("COMMIT WORK RELEASE", 19);
  segment.ClosePart(part);
  rp.CloseSegment(segment);  
  tsp1_packet * rawRequest = rp.GetRawPacket();
  tsp00_ErrText errText;
  tsp01_CommErr commErr;
  sqlarequest ((tsp00_Int4)m_conn->m_connectionid, rawRequest, rp.Length (),
               errText, commErr);
  void        * rawPointer = NULL;
  tsp00_Int4    replyLen;
  if (commErr == commErrOk_esp01) {
      sqlareceive ((tsp00_Int4)m_conn->m_connectionid, &rawPointer, replyLen, errText, commErr);
  }

  /* restore the content of the packet */
  memcpy(packet, p, len);
  free(p);

}


IFR_Connection *CrashMe_Check::Reconnect(char *hostname, char *database, char *username, char *password)
{

  IFR_Connection *conn = m_conn->getConnection();
  IFR_Bool memory_ok=true;
  IFR_ConnectProperties emptyProperties(conn->allocator, memory_ok);
  conn->connect(hostname, IFR_NTS, database, IFR_NTS, username, IFR_NTS, password, IFR_NTS, IFR_StringEncodingAscii );
  return conn;
}


CrashMe::CrashMe(IFR_Connection *connection)
{
  m_chk = new CrashMe_Check(connection);
}

CrashMe::~CrashMe()
{
    delete m_chk;
}


bool CrashMe::CheckConnection()
{
  return m_chk->CheckConnection();
}

IFR_Connection *CrashMe::Reconnect(char *hostname, char *database, char *username, char *password)
{
  return m_chk->Reconnect(hostname, database, username, password);
}

void CrashMe::BreakConnection()
{
    m_chk->BreakConnection();
    return;
}

static IFR_Bool memory_ok_ignored=true;

CrashMe_Check::CrashMe_Check(IFR_Connection *connection)
  : m_stmt(NULL),
    m_conn(connection),
    m_stamp(0),
    m_hostname(connection->m_hostname, memory_ok_ignored),
    m_databasename(connection->m_databasename, memory_ok_ignored),
    m_username(connection->m_username, memory_ok_ignored)
{
    CheckConnection(); 
};

bool CrashMe_Check::CheckConnection()
{
  SQL_TIMESTAMP_STRUCT ts;
  IFR_Retcode rc;
  if (!m_stmt) {      
    m_stmt = m_conn->createPreparedStatement ();
  }
  else if (m_stamp == 0) {
    return false;
  }
  rc = m_stmt->prepare("SELECT TIMESTAMP INTO ? FROM DUAL", IFR_NTS, IFR_StringEncodingAscii);
  if (rc != IFR_OK)
    return false;
  rc = m_stmt->bindParameter(1, IFR_HOSTTYPE_ODBCTIMESTAMP, &ts, NULL, sizeof(ts));
  if (rc != IFR_OK)
    return false;
  
#ifdef WIN32
  // on W2k smallest timestamp resolution is milliseconds
  // but let the client wait 1 second
  Sleep(1000);
#else
  sleep(1);
#endif  
  rc = m_stmt->execute();
  if (rc != IFR_OK) {
    return false;
  }

  IFR_UInt8 stamp = ts.fraction / IFR_INT64_CONSTANT(1000000)
      + ts.second * IFR_INT64_CONSTANT(1000)
      + ts.minute * IFR_INT64_CONSTANT(1000)
      + ts.hour   * IFR_INT64_CONSTANT(1000) * 60 * 60
      + ts.day    * IFR_INT64_CONSTANT(1000) * 60 * 60 * 24
      + ts.month  * IFR_INT64_CONSTANT(1000) * 60 * 60 * 24 * 31       // crude approximation
      + ts.year   * IFR_INT64_CONSTANT(1000) * 60 * 60 * 24 * 31 * 12;

  if (stamp > m_stamp) {    
    m_stamp = stamp;
    return true;
  }
  return false;
};

