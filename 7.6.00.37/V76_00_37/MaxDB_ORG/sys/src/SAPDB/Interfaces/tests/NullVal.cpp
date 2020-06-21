/*!
  @file           NullVal.cpp
  @author         D025086
  @ingroup        Tests
  @brief          Test returning Null values.
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
#include <stdio.h>
#include <string.h>
#include "Interfaces/sapdbifr.h"
#include "SAPDB/Interfaces/tests/TestUtils.h"

IFR_Bool memory_ok_ignored=true;

class NullVal : public IFR_TestCase
{
public:
  NullVal()
    :IFR_TestCase("NullVal")
  {}

  IFR_Bool NullColumns() {
    IFR_Retcode rc;

    IFR_StringEncoding encoding = (connection()->isUnicodeDatabase()) ? 
      IFR_StringEncodingUCS2Native : IFR_StringEncodingAscii;
      
    IFR_String sql(encoding, connection()->allocator);

    sql.append("SELECT * FROM NULLVAL", IFR_StringEncodingAscii, IFR_NTS, memory_ok_ignored);

    IFR_PreparedStmt *stmt = tc_errorcheck (connection ())->createPreparedStatement ();

    rc = tc_errorcheck (stmt)->prepare(sql.getBuffer(), sql.getEncoding());
    rc = tc_errorcheck (stmt)->execute();
    if (rc != IFR_OK)
      logerror (stmt->error().getErrorText ());
    
    IFR_ResultSet *res = tc_errorcheck (stmt)->getResultSet();
    IFR_RowSet *rowset = res->getRowSet ();

    char s[11];
    IFR_Length ind;

    rowset->getObject(1, (IFR_Byte*)s, sizeof(s), IFR_HOSTTYPE_ASCII, &ind, false);
    tc_expectederrorcheck(res, -10501);
    
    IFR_ResultSetMetaData *rsd = stmt->getResultSetMetaData();
    int colnum = rsd->getColumnCount();
    
    { tc_errorcheck(res)->next(); }

    const char refstr[] = "xxxxxxxxxx";
    for (;colnum > 0; colnum--) {
      ind = 99;
      strcpy(s, refstr);      
      loginfo("Check column %d", colnum);
      if (rowset->getObject(colnum, s, sizeof(s), IFR_HOSTTYPE_ASCII, &ind, true) != IFR_OK)
        logerror (res->error().getErrorText ());
      
      // A '\0' is written into the first char, this is because of ODBC behaviour.
      if(strcmp(s+1, &(refstr[1]))!=0 || *s != 0)
          logerror ("getObject attached data for column %d.", colnum);
      
      if(ind != IFR_NULL_DATA)
          logerror ("Indicator value wasn't set  for column %d.", colnum);
    }
    return m_success;
  }

  virtual IFR_Bool run()
  {
    dropTable ("NULLVAL");

    m_success = m_success && execSQL ("CREATE TABLE NULLVAL (C CHAR (10), CB CHAR (10) BYTE, V VARCHAR(10), VB VARCHAR(10) BYTE, F2 FIXED(2,0), I INTEGER, F FLOAT, L LONG, LB LONG BYTE, D DATE, T TIME, TS TIMESTAMP)");
      
    m_success = m_success && execSQL ("INSERT INTO NULLVAL VALUES (NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL )");

    m_success = m_success && NullColumns();

    return m_success;
  } // end run
};

int main(int argc, char **argv)
{
  // Initalize from command line params
  IFR_TestFrame testframe(argc, argv);
  testframe.addTestCase(new NullVal());
  if(testframe.run()) 
    return 0;

  return 1;
}


