/*!================================================================

 module:        InsStmtExe.cpp

 -------------------------------------------------------------------

 responsible:   D030044

 special area:  SQL

 description:   Example: Insert with Parameters

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

class InsStmtExe : public IFR_TestCase
{
public:
  InsStmtExe ()
    : IFR_TestCase ("InsStmtExe")
  {}

  virtual IFR_Bool run ()
  {
    bool hasResult = false;
    IFR_Retcode rc;

    IFR_Int4 cbPacketSize;
    if (getPacketSize(cbPacketSize) != IFR_OK)
      return m_success;
    IFR_Bool memory_ok=true;
    IFR_String cmdInsert ("INSERT INTO PACKET4 VALUES (?, ?)"
                          , IFR_StringEncodingAscii, connection()->allocator, memory_ok);
    IFR_Statement *stmt = tc_errorcheck (connection ())->createStatement ();
    IFR_PreparedStmt *stmtPrepare = tc_errorcheck (connection ())->createPreparedStatement ();

    dropTable ("PACKET4");
    m_success = m_success && execSQL ("CREATE TABLE PACKET4 (I INTEGER, J CHAR(6))");

    /* The table should be filled with as much as 32K rows and as much as 4 packets 
       to fill the whole table. 
    */
    /* determine the numer of rows needed */
    int nrows = cbPacketSize*4 / (10+1+6+1);
    nrows = (nrows < MAX_IFR_INT2+1) ? MAX_IFR_INT2+1 : nrows;
    
    loginfo("Fill table with %d rows", nrows);

    IFR_Int4 I;
    char *testStr2 = new char[7];

    IFR_Length Ilen=sizeof(I);
    IFR_Length t2len=IFR_NTS;
    tc_errorcheck (stmtPrepare)->bindParameter (1, IFR_HOSTTYPE_INT4, &I, NULL, sizeof(I));
    tc_errorcheck (stmtPrepare)->bindParameter (2, IFR_HOSTTYPE_ASCII, testStr2, &t2len, 10);
    tc_errorcheck (stmtPrepare)->prepare (cmdInsert.getBuffer(), cmdInsert.getEncoding());
    int i;
    for (i = 0; i < nrows; i++) {
      I = i+1;
      sprintf (testStr2, "J%.5d", i+1);
      rc = tc_errorcheck (stmtPrepare)->execute ();
      if (rc != IFR_OK) {
          logerror (stmt->error().getErrorText ());
          break;
      }
      if(i % 5000 == 0) { loginfo("Inserted %d rows ... ", i); }
    }
    if(i % 5000 != 0) { loginfo("Inserted %d rows ... ", i); }

    execSQL ("COMMIT WORK");

    connection()->releaseStatement(stmt);
    connection()->releaseStatement(stmtPrepare);

    return m_success;
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  IFR_TestFrame testframe (argc, argv);
  testframe.addTestCase (new InsStmtExe ());
  if (testframe.run ()) 
    return 0;

  return 1;
}
