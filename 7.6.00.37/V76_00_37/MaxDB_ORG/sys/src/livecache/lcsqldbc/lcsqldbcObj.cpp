/***************************************************************************


    ========== licence begin  GPL
    Copyright (c) 2002-2005 SAP AG

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
/* lcsqldbcObj.cpp : Implementation of Clcsqldbc                            */
/****************************************************************************/

#include "lcbasetypes.h"      /* no check */
#include "livecachetypes.h"   /* no check */
#include "lcsqldbc.h"         /* no check */
#include "livecache/lcsqldbc/lcsqldbcObj.h"

#if defined(WIN32)
#include <new>
#else
#include <new.h>
#endif

/****************************************************************************/
/* Register is the very first Method called after the creation of the       */
/* Component-Object.                                                        */
/* (within the "constructor" of the Component-Object no communication with  */
/*  the liveCache has been activated yet!)                                  */

STDMETHODIMP Clcsqldbc::Register()
{
    try {
    co90CheckVersion ((IUnknown**)&this->m_pIUnknown, 
                      ILIVECACHESINK_VERSION, 
                      LIVECACHETYPES_VERSION, LIVECACHEOMS_VERSION, 
                      LIVECACHESQL_VERSION );
    omsControlBadAllocExceptions(-4711);
    LCSQLDBC_TEST1(); // check if sql can be done within Register
  }
  catch (DbpError &e) {
    omsExceptionHandler (e);
  }
  return S_OK;
};

/****************************************************************************/
STDMETHODIMP Clcsqldbc::LCSQLDBC_TEST1()
{
  char  actStmt[256];
  SQL_SQLDBCConnection con;
  
  SQLDBC_Statement *stmt = con.createStatement();
  char cPar[256];
  strcpy(cPar,"create table LCSQLDBC_TAB1 (s1 int, s2 varchar(256))");
  stmt->execute(cPar);
  if ( -6000 == con.error().getErrorCode())
    stmt->execute("delete LCSQLDBC_TAB1");
  con.releaseStatement(stmt);
  
  SQLDBC_PreparedStatement *pstmt = con.createPreparedStatement();
  
  pstmt->prepare("insert into LCSQLDBC_TAB1 values( :a1, :a2 )");
  
  SQLDBC_Int4 i1 = 1;
  SQLDBC_Length li1 = 0;
  SQLDBC_Length lcPar = SQLDBC_NTS;
  SqlCol stmt_par = SqlCol( &cPar[0], SqlCol::VSTRING, strlen(cPar) );
  strcpy(actStmt,"insert into LCSQLDBC_TAB1 values( :a1, :a2 )");
  pstmt->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &i1, &li1, sizeof(i1));
  pstmt->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, cPar, &lcPar, sizeof(cPar));
  pstmt->execute();
  if (pstmt->error()) {
    dbpTrace("LCSQLDBC_TEST1() ERROR stmt:%s", actStmt );
    return S_OK;
  }
  pstmt->prepare("select s1, s2 INTO :a1, :a2 FROM LCSQLDBC_TAB1");
  
  i1 = 0;
  pstmt->execute();
  
  if ( ( i1 != 1 ) || strcmp(cPar, "create table LCSQLDBC_TAB1 (s1 int, s2 varchar(256))"))
    throw DbpError(-1, "Error within 1. Test of LCSQLDBC_TEST1");
  
  dbpTrace("Clcsqldbc::LCSQLDBC_TEST1()  OK!");
  return S_OK;
}

STDMETHODIMP Clcsqldbc::OUTCOPY_MESSAGES2(StreamHandle * ostream, StreamHandle * ostream2)
{
  char errmsg[600];
  SQL_SQLDBCConnection con;
  try {
    TMessageRow messageRow;
    OmsOStream<StreamHandle, TMessageRow> outStream(this, *ostream);
    OmsOStream<StreamHandle, TMessageRow> outStream2(this, *ostream2);
    
    SQLDBC_Statement *stmt = con.createStatement();
    
    stmt->execute("SELECT MSGNO, LANGUAGE, MSGTEXT FROM SYSDBA.MESSAGES");
    if (stmt->error()) {
      sprintf(errmsg, "ERROR: Can't execute statement (%d):(%s) in OUTCOPY_MESSAGES2", 
              stmt->error().getErrorCode(), stmt->error().getErrorText());
      throw DbpError(-1, errmsg);
    }
    SQLDBC_ResultSet *rs = stmt->getResultSet();
    if (!rs) {
      sprintf(errmsg, "ERROR: creating SQLDBC_ResultSet (%d):(%s) in OUTCOPY_MESSAGES2", 
              stmt->error().getErrorCode(), stmt->error().getErrorText());
      throw DbpError(-1, errmsg);
    }
    rs->bindColumn(1, SQLDBC_HOSTTYPE_INT4, &messageRow.msgNo, NULL, sizeof(messageRow.msgNo));
    rs->bindColumn(2, SQLDBC_HOSTTYPE_ASCII, messageRow.language, NULL, sizeof(messageRow.language));
    rs->bindColumn(3, SQLDBC_HOSTTYPE_ASCII, messageRow.msg, NULL, sizeof(messageRow.msg));

    memset(messageRow.msg, ' ', sizeof(messageRow.msg));
    memset(messageRow.language, ' ', sizeof(messageRow.language));
    while (rs->next() == SQLDBC_OK) {
      rs->getRowSet()->fetch();
      outStream.omsWrite(messageRow);
      outStream2.omsWrite(messageRow);
      memset(messageRow.language, ' ', sizeof(messageRow.language));
      memset(messageRow.msg, ' ', sizeof(messageRow.msg));
    }
    if (rs->error()) {
      sprintf(errmsg, "ERROR: On fetch data (%d):(%s) in OUTCOPY_MESSAGES2", 
              stmt->error().getErrorCode(), stmt->error().getErrorText());
      throw DbpError(-1, errmsg);
    }
    rs->close();
    outStream.omsClose();
    outStream2.omsClose();
  }
  catch (DbpError sqle)
    {
      sqlExceptionHandler(sqle);
    }
  return S_OK;
}

STDMETHODIMP Clcsqldbc::INCOPY_MESSAGES2(StreamHandle * istream, StreamHandle * istream2)
{
  char errmsg[600];
  SQL_SQLDBCConnection con;
  try {
    TMessageRow *pMessageRow=NULL;
    TMessageRow *pMessageRow2=NULL;
    boolean count=0; // prevents endless loop;

    SQLDBC_PreparedStatement *pstmt = con.createPreparedStatement();
    pstmt->prepare("INSERT INTO NEWMESSAGES VALUES (?,?,?)");
    if (pstmt->error()) {
      sprintf(errmsg, "ERROR: Can't prepare statement (%d):(%s) in OUTCOPY_MESSAGES2", 
              pstmt->error().getErrorCode(), pstmt->error().getErrorText());
      throw DbpError(-1, errmsg);
    }

    SQLDBC_ParameterMetaData* pdata = pstmt->getParameterMetaData();
    SQLDBC_Int4 msglen = pdata->getParameterLength(3);

    OmsIStream<StreamHandle, TMessageRow> inStream(this, *istream);
    OmsIStream<StreamHandle, TMessageRow> inStream2(this, *istream2);

    do {
      if (!count || pMessageRow)
        pMessageRow = inStream.omsRead();
      if (!count || pMessageRow2) 
        pMessageRow2 = inStream2.omsRead();
      count++;
      if (pMessageRow) {
        pstmt->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &pMessageRow->msgNo, NULL, sizeof(pMessageRow->msgNo));
        pstmt->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, pMessageRow->language, NULL, sizeof(pMessageRow->language));
        pstmt->bindParameter(3, SQLDBC_HOSTTYPE_ASCII, pMessageRow->msg, NULL, msglen);
        pstmt->execute();
        if (pstmt->error()) {
          sprintf(errmsg, "ERROR: Can't insert MessageRow (%d):(%s) in INCOPY_MESSAGES2", 
                  pstmt->error().getErrorCode(), pstmt->error().getErrorText());
          throw DbpError(-1, errmsg);
        }
      }
      if (pMessageRow2) {
        pstmt->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &pMessageRow2->msgNo, NULL, sizeof(pMessageRow2->msgNo));
        pstmt->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, pMessageRow2->language, NULL, sizeof(pMessageRow2->language));
        pstmt->bindParameter(3, SQLDBC_HOSTTYPE_ASCII, pMessageRow2->msg, NULL, msglen);
        pstmt->execute();
        if (pstmt->error()) {
          sprintf(errmsg, "ERROR: Can't insert MessageRow2 (%d):(%s) in INCOPY_MESSAGES2", 
                  pstmt->error().getErrorCode(), pstmt->error().getErrorText());
          throw DbpError(-1, errmsg);
        }
      }          
    } while (count > 10000 || pMessageRow || pMessageRow2);
  }
  catch (DbpError sqle)
    {
      sqlExceptionHandler(sqle);
    }
  return S_OK;
}
