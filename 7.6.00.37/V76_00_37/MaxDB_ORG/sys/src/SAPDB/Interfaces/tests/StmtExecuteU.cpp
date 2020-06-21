/*!================================================================

 module:        StmtExecute.cpp

 -------------------------------------------------------------------

 responsible:   D025086

 special area:  SQL

 description:   Example: Simple Execute

 ------------------------------------------------------------------


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




 ===================================================================*/

#include <stdio.h>
#include "Interfaces/sapdbifr.h"
#include "SAPDB/Interfaces/tests/TestUtils.h"

enum UCS2ByteOrder {
  normal,
  swapped,
  native
};

class StmtExecute : public IFR_TestCase
{
public:
  StmtExecute(UCS2ByteOrder byteorder)
    : IFR_TestCase("StmtExecute")
  {
    switch(byteorder) {
    case normal: {
      m_encoding = IFR_StringEncodingUCS2;
      m_swapped = false;
      break;
    }
    case swapped: {
      m_encoding = IFR_StringEncodingUCS2Swapped;
      m_swapped = true;
      break;
    }
    case native: {
      m_encoding = IFR_StringEncodingUCS2Native;
      m_swapped = (m_encoding == IFR_StringEncodingUCS2Swapped) ? true : false;
      break;
    }
    }
  }

  virtual IFR_Bool run()
  {
    IFR_Retcode rc;

    static char *Cmd = (char*)"SELECT 'ONE' FROM DUAL";
    unsigned short buffer_w[100];
    char *buffer = (char*)buffer_w;
    convertToUCS2(buffer_w, sizeof(buffer_w), m_swapped, Cmd);
    
    IFR_Statement *stmt = tc_errorcheck(connection())->createStatement ();
    
    rc = tc_errorcheck (stmt)->execute (buffer, m_encoding);
    if (rc == IFR_OK) {
      loginfo("Statement successfull executed\n");
    }
    else {
      logerror("Execution failed\n");
      logerror(stmt->error().getErrorText());      
      return m_success;
    }
    IFR_ResultSet *resultset = tc_errorcheck(stmt)->getResultSet();
    if (resultset != NULL) {
      loginfo("Resultset exists.\n");
    } else {
      logerror("No resultset exists.\n");
      return m_success;
    }
    tc_errorcheck(resultset)->next();
    IFR_RowSet *rowset = resultset->getRowSet ();

    char s[10];
    IFR_Length ind;
    rowset->getObject (1, s, sizeof(s), IFR_HOSTTYPE_ASCII, &ind, true);
    tc_errorcheck(resultset);
    if (!strcmp("ONE", s))
      loginfo("'%s' fetched from result", s);
    else
      logerror("Wrong value fetched from result", s);      
    return m_success;
  }
private:
  IFR_StringEncoding m_encoding;
  IFR_Bool m_swapped;
};

class StmtPrepare : public IFR_TestCase
{
public:
  StmtPrepare(UCS2ByteOrder byteorder)
    : IFR_TestCase("StmtPrepare")
  {
    switch(byteorder) {
    case normal: {
      m_encoding = IFR_StringEncodingUCS2;
      m_swapped = false;
      break;
    }
    case swapped: {
      m_encoding = IFR_StringEncodingUCS2Swapped;
      m_swapped = true;
      break;
    }
    case native: {
      m_encoding = IFR_StringEncodingUCS2Native;
      m_swapped = (m_encoding == IFR_StringEncodingUCS2Swapped) ? true : false;
      break;
    }
    }
  }

  virtual IFR_Bool run()
  {
    IFR_Retcode rc;

    static char *Cmd = (char*)"SELECT 'ONE' FROM DUAL";
    unsigned short buffer_w[100];
    char *buffer = (char*)buffer_w;

    convertToUCS2(buffer_w, sizeof(buffer_w), m_swapped, Cmd);
    
    IFR_PreparedStmt *stmt = tc_errorcheck(connection())->createPreparedStatement ();
    IFR_Bool memory_ok_ignored=true;
    IFR_String sql(buffer, m_encoding, connection()->allocator, memory_ok_ignored);

    rc = tc_errorcheck(stmt)->prepare(sql.getBuffer(), sql.getEncoding());
    if (rc == IFR_OK) {
      loginfo("Statement successfull prepared\n");
    }
    else {
      printf("error : preparation failed\n");
      logerror (stmt->error().getErrorText());      
      return m_success;
    }

    rc = tc_errorcheck(stmt)->execute();
    if (rc == IFR_OK) {
      loginfo("Statement successfull execute\n");
    }
    else {
      logerror("execution failed\n");
      logerror(stmt->error().getErrorText());      
      return m_success;
    }
    
    IFR_ResultSet *resultset = tc_errorcheck(stmt)->getResultSet();
    if (resultset != NULL) {
      loginfo("Resultset exists.\n");
    } else {
      logerror("No resultset exists.\n");
      return m_success;
    }
    tc_errorcheck(resultset)->next();
    IFR_RowSet *rowset = resultset->getRowSet ();

    char s[10];
    IFR_Length ind;
    rowset->getObject(1, s, sizeof(s), IFR_HOSTTYPE_ASCII, &ind, true);
    tc_errorcheck(resultset);
    if (!strcmp("ONE", s))
      loginfo("'%s' fetched from result", s);
    else
      logerror("Wrong value fetched from result", s);      
    return m_success;
  }
private:
  IFR_StringEncoding m_encoding;
  IFR_Bool m_swapped;
};

int main(int argc, char **argv)
{
  // Initalize from command line params
  IFR_TestFrame testframe (argc, argv);
  testframe.addTestCase(new StmtExecute(native));
  testframe.addTestCase(new StmtPrepare(native));
  testframe.addTestCase(new StmtExecute(normal));
  testframe.addTestCase(new StmtPrepare(normal));
  testframe.addTestCase(new StmtExecute(swapped));
  testframe.addTestCase(new StmtPrepare(swapped));
  if (testframe.run()) 
    return 0;

  return 1;
}




