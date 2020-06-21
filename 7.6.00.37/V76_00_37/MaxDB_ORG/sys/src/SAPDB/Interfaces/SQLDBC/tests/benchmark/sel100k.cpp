/*!================================================================
 module:    updinsert.cpp

 responsible: TTK

 special area: updatable rowset
 description:  insert by updatable rowset

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

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"
#include "heo02.h"
#include <string.h>

#define MAXLINES 10000

long id[MAXLINES];
char text1[MAXLINES][25], text2[MAXLINES][225];


class sqlbench : public SQLDBC_TestCase
{
public:
  sqlbench ()
    : SQLDBC_TestCase ("sqlbench")
  {}

  void printErrorText (SQLDBC_ErrorHndl& error)
  {
    char buffer[1024];
    sprintf (buffer, "Error %d, sql state %5s, %s",
             error.getErrorCode(), 
             error.getSQLState(),
             error.getErrorText());
    logerror (buffer);
  }

  void SelectTable ()
  {
    SQLDBC_PreparedStatement      *pStmt;
    SQLDBC_ResultSetMetaData *resultMetaData;
    SQLDBC_ResultSet         *resultset;

    pStmt = tc_errorcheck(connection())->createPreparedStatement ();

    tc_errorcheck(pStmt)->prepare("select * from phpbench",
                                  SQLDBC_StringEncodingType::Ascii);

    tc_errorcheck(pStmt)->execute();

  // read meta data from result set
  if (pStmt->isQuery()) {
    resultset = pStmt->getResultSet();

    resultMetaData = resultset->getResultSetMetaData();

    tc_errorcheck(resultset)->bindColumn(1, SQLDBC_HOSTTYPE_INT4, id, NULL, 0);  // integral data type need no length info
    tc_errorcheck(resultset)->bindColumn(2, SQLDBC_HOSTTYPE_ASCII, &text1, NULL, sizeof(text1[0]));

    tc_errorcheck(resultset)->bindColumn(3, SQLDBC_HOSTTYPE_ASCII, &text2, NULL, sizeof(text2[0]));

    // read data with MAXLINES rows per call
    resultset->setRowSetSize(MAXLINES);

    // position to the start of the resultset
    tc_errorcheck(resultset)->first();

    tc_errorcheck(resultset)->getRowSet()->fetch();

    printf ("%d\t%s\t%s\n", id[0], text1[0], text2[0]);
    printf ("%d\t%s\t%s\n", id[9900], text1[9900], text2[9900]);

    // close result set
    resultset->close();
  }
  }

  virtual SQLDBC_Bool run ()
  {

    SQLDBC_Int4 startsec, startusec;
    SQLDBC_Int4 endsec, endusec;
    sqlclock(&startsec , &startusec);
    for (int j=0; j<10; j++)
      SelectTable ();
    sqlclock(&endsec , &endusec);
    
    printf("sqlbench runs %u.%u seconds\n", endsec-startsec, endusec-startusec);    

  return m_success;
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe(argc, argv);
  testframe.addTestCase(new sqlbench());
  if (testframe.run()) 
    return 0;

  return 1;
}
