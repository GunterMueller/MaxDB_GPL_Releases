/*!================================================================
 module:    SimpleNavigation.cpp

 responsible: D030044

 special area: Resultset
 description:  Tests result set cursor navigation for absolute 
               positioning.

 see:
 
 change history:


    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#include "SAPDB/Interfaces/sapdbifr.h"
#include "SAPDB/Interfaces/tests/TestUtils.h"

IFR_Bool memory_ok_ignored=true;

class SimpleNavigation : public IFR_TestCase
{
public:
  SimpleNavigation ()
    : IFR_TestCase ("SimpleNavigation")
  {}

  virtual IFR_Bool run ()
  {
    int i;
    dropTable ("TEST_NAVIGATION");
    m_success = m_success && execSQL ("CREATE TABLE TEST_NAVIGATION (A INTEGER, B CHAR(1000) ASCII)");

    IFR_String a256 ("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\
aaaaaaaaaaaaaaaaa", IFR_StringEncodingAscii, connection()->allocator, memory_ok_ignored);
    IFR_PreparedStmt *p1 = tc_errorcheck(connection ())->createPreparedStatement ();
    IFR_String s1("INSERT INTO TEST_NAVIGATION (A, B) VALUES (?, \'", 
                  IFR_StringEncodingAscii, connection()->allocator, memory_ok_ignored);    
    s1.append(a256, memory_ok_ignored);
    s1.append("\')", IFR_StringEncodingAscii, IFR_NTS, memory_ok_ignored);
    tc_errorcheck(p1)->bindParameter (1, IFR_HOSTTYPE_INT4, &i, 0, 4);
    tc_errorcheck(p1)->prepare (s1.getBuffer (), s1.getEncoding());
    for (i = 1; i <= RECORDS; i++) {
      tc_errorcheck(p1)->execute (); 
    }

    int fetchSizes[] = { 0, 1, 2, 4, 10, 15, 47, 150, 1200, 
			 RECORDS, RECORDS/2, RECORDS/3, RECORDS -1, RECORDS+1 };

    for (i = 0; i < sizeof (fetchSizes) / sizeof (int); i++) {
      testAbsolute (fetchSizes[i]);
    }
    connection()->releaseStatement(p1);
    execSQL ("COMMIT WORK");
    return m_success;
  }

  void testAbsolute (int fetchSize) {
    int testindices[] = { 1, -1,
			  2, -2,
			  3, -3,
			  fetchSize, -fetchSize,
			  fetchSize * 2, -fetchSize * 2,
			  17, 23, 37, 42,
			  RECORDS, -RECORDS,
			  RECORDS + 1, -RECORDS -1,
			  RECORDS - 1, -RECORDS + 1,
			  RECORDS / 2, -RECORDS / 2,
			  RECORDS / 3, -RECORDS / 3
    };

    int *allindices = new int[RECORDS * 2 + sizeof (testindices) / sizeof (int)];
    IFR_size_t i;
    for (i = 1; i <= (unsigned) RECORDS; i++) {
      allindices[i-1] = i;
    }
    for (i = 1; i <= (unsigned) RECORDS; i++) {
      allindices[RECORDS + i - 1] = -(signed)i;
    }
    for (i = 0; i < sizeof (testindices) / sizeof (int); i++) {
      allindices[2*RECORDS + i - 1] = testindices[i];
    }

    char fetchBuf[64];
    char targetBuf[64];
    char valueBuf[64];
    IFR_ResultSet *rs = getQueryResult (fetchSize);
    sprintf (fetchBuf, "%d", fetchSize);
    for (i = 0; i < RECORDS * 2 + sizeof (testindices) / sizeof (int); i++) {
        int targetindex = allindices[i];
        sprintf (targetBuf, "%d", targetindex);
        
        if (targetindex != 0) {
            int pos = checkpos (targetindex);
            int value = invertNumber (targetindex, RECORDS);
            sprintf (valueBuf, "%d", value);
            IFR_Retcode rc = rs->absolute (targetindex);
            if (pos == 0 && rc != IFR_OK) {
                logerror("FETCH ABSOLUTE %s FETCHSIZE %s NOT ON RESULT SET", targetBuf, fetchBuf);
                continue;
            } else if (pos == -1 && !rs->isBeforeFirst ()) {
                logerror("FETCH ABSOLUTE %s FETCHSIZE %s NOT BEFORE RESULT SET", targetBuf, fetchBuf);
                continue;
            } else if (pos == 1 && !rs->isAfterLast ()) {
                logerror("FETCH ABSOLUTE %s FETCHSIZE %s NOT AFTER RESULT SET", targetBuf, fetchBuf);
                continue;
            }

	if (pos == 0) {
	  int val = 0;
          IFR_Length ind;
	  char valBuf[64];
	  IFR_Retcode rc;
	  IFR_RowSet *rowset = rs->getRowSet ();
	  rc = rowset->getObject (1, &val, sizeof(val), IFR_HOSTTYPE_INT4, &ind, false);
	  sprintf (valBuf, "%d", val);
	  if (val != value) {
          logerror("FETCH ABSOLUTE %s FETCHSIZE %s EXPECTED %s RETRIEVED %s ", targetBuf, fetchBuf, valueBuf, valBuf);
      }

	  int row;
	  row = rs->getRow ();
	  if (row != value) {
          logerror("FETCH ABSOLUTE: UNEXPECTED ROW NUM %d INSTEAD OF %s", row, valueBuf);
              }
	}
      }
    }

    delete[] allindices;
  }

  IFR_ResultSet *getQueryResult (int fetchSize) {
      const char *cmdSelect = "SELECT A, B FROM TEST_NAVIGATION ORDER BY A";
    IFR_Statement *stmt = tc_errorcheck(connection ())->createStatement ();
    stmt->setResultSetType(IFR_Statement::SCROLL_SENSITIVE);
    tc_errorcheck (stmt)->setFetchSize (fetchSize);
    IFR_Retcode rc = tc_errorcheck (stmt)->execute (cmdSelect, IFR_StringEncodingAscii);
    if (rc != IFR_OK)
      Util_PrintError ("execute", &(stmt->error()));

    return tc_errorcheck (stmt)->getResultSet ();
  }

  int checkpos (int i) {
      if (i > 0) {
      if (i > RECORDS)
	return 1;
    } else {
      if (invertNumber (i, RECORDS) <= 0) {
	return -1;
      }
    }
    return 0;
  }

  int invertNumber (int number, int end) {
    if (number >= 0)
      return number;
    else
      return end + number + 1;
  }

  static int RECORDS;
};

int SimpleNavigation::RECORDS = 333;

int main (int argc, char **argv)
{
  // Initalize from command line params
  IFR_TestFrame testframe (argc, argv);
  testframe.addTestCase (new SimpleNavigation ());
  if (testframe.run ()) 
    return 0;

  return 1;
}
