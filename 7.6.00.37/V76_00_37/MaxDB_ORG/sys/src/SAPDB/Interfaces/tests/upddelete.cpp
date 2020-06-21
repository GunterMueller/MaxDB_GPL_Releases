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

#include "SAPDB/Interfaces/sapdbifr.h"
#include "SAPDB/Interfaces/tests/TestUtils.h"

#define MAX_ROWS 100

class updinsert : public IFR_TestCase
{
public:
  updinsert ()
    : IFR_TestCase ("Updatable RowSet: delete")
  {}

void printErrorText (IFR_ErrorHndl& error)
{
    char buffer[1024];
    sp77sprintfUnicode(sp77encodingUTF8, buffer, 1024, "Error %d, sql state %5s, %s",
                       error.getErrorCode(), 
                       error.getSQLState(),
                       error.getErrorText());
    logerror (buffer);
}

  virtual IFR_Bool run ()
  {
    IFR_PreparedStmt      *pStmt, *pStmt1;

    IFR_ResultSetMetaData *resultMetaData;
    IFR_ResultSet         *resultset;
    IFR_UpdatableRowSet   *updRowSet;

    IFR_Retcode            retcode;

    IFR_Int4 id[MAX_ROWS];
    char     name[MAX_ROWS][30];

    int counter;
    int i;

    connection()->setAutoCommit (IFR_TRUE);

    dropTable ("UPDDELETE");
    m_success = m_success && execSQL ("CREATE TABLE UPDDELETE (id int, name char(30))");

    //aaaaaaaaaaaaaaaaa", IFR_StringEncodingAscii, connection()->allocator);

    pStmt = tc_errorcheck(connection ())->createPreparedStatement ();

    tc_errorcheck(pStmt)->prepare("insert into UPDDELETE values (?, ?)",
                                  IFR_StringEncodingType::Ascii);

    tc_errorcheck(pStmt)->bindParameter (1, IFR_HOSTTYPE_INT4, id, NULL, 0);  // integral data type need no length info
    tc_errorcheck(pStmt)->bindParameter (2, IFR_HOSTTYPE_ASCII, &name[0], NULL, sizeof(name[0]));

    counter = 1;
    for (i=0; i<MAX_ROWS; i++) {
      id[0] = counter;
      sprintf (name[0], "Fritz %d", counter);
      counter++;
      // execute
      tc_errorcheck(pStmt)->execute ();
    }

    // insert data with MAX_ROWS rows per call
    tc_errorcheck(pStmt)->setRowArraySize (MAX_ROWS);

    for (i=0; i<MAX_ROWS; i++) {
      id[i] = counter;
      sprintf (name[i], "Fritz %d", counter);
      counter++;
    }

    // execute batch command
    tc_errorcheck(pStmt)->executeBatch ();


    tc_errorcheck(connection())->commit();

    pStmt->setResultSetConcurrencyType (IFR_Statement::UPDATABLE);
    pStmt->setResultSetType(IFR_Statement::SCROLL_SENSITIVE);
    tc_errorcheck(pStmt)->prepare("select * from UPDDELETE where name like '%7'",
                                  IFR_StringEncodingType::Ascii);

    tc_errorcheck(pStmt)->execute ();

  // read meta data from result set
  if (pStmt->isQuery()) {
    resultset = pStmt->getResultSet ();
    updRowSet = resultset->getUpdatableRowSet ();

    resultMetaData = resultset->getResultSetMetaData ();

    printf ("Resultset has %d columns\n", resultMetaData->getColumnCount());

    // read column names
    for (i=1; i<=resultMetaData->getColumnCount(); i++) {
      char buf[50];
      IFR_Length length;
      resultMetaData->getColumnName(i, buf, IFR_StringEncodingType::Ascii,
                                    sizeof(buf), &length);
      printf("Col %d has the name %s\n", i, buf);
    }

    // read data with 7 rows per call
    resultset->setRowSetSize (7);

    // bind columns
    tc_errorcheck(resultset)->bindColumn (1, IFR_HOSTTYPE_INT4, id, NULL, sizeof(id[0]));
    tc_errorcheck(resultset)->bindColumn (2, IFR_HOSTTYPE_ASCII, &name[0], NULL, sizeof(name[0]));

    // position to the start of the resultset
    tc_errorcheck(resultset)->first ();

    for (i=0; i<7; i++) {
      id[i] = 997 + i;
      sprintf (name[i], "%d haha", i+1);
    }

    if (updRowSet->insertRow(0) != IFR_OK) {
      logerror ("Error: insertAllRows.");
      printErrorText (resultset->error());
    }

    id[0] = 444;
    strcpy (name[0], "444 huhu");

    if (updRowSet->insertRow(1) != IFR_OK) {
      logerror ("Error: insertOneRow.");
      printErrorText (resultset->error());
    }

    id[0] = 444;
    strcpy (name[0], "444 huhu");
    //    tc_errorcheck(resultset)->updateRow(1);

    if (updRowSet->deleteRow(4) != IFR_OK) {
      logerror ("Error: updateRow(4).");
      printErrorText (resultset->error());
    }

    if (updRowSet->deleteRow(5) != IFR_OK) {
      logerror ("Error: updateRow(5).");
      printErrorText (resultset->error());
    }

    pStmt1 = tc_errorcheck(connection ())->createPreparedStatement ();

    tc_errorcheck(pStmt1)->prepare("select * from UPDDELETE where name like '%7' and id < 160",
                                  IFR_StringEncodingType::Ascii);
    pStmt1->setResultSetType(IFR_Statement::SCROLL_SENSITIVE);
    tc_errorcheck(pStmt1)->execute ();
    resultset = pStmt1->getResultSet ();

    // read data with 7 rows per call
    resultset->setRowSetSize (7);

    // bind columns
    tc_errorcheck(resultset)->bindColumn (1, IFR_HOSTTYPE_INT4, id, NULL, sizeof(id[0]));
    tc_errorcheck(resultset)->bindColumn (2, IFR_HOSTTYPE_ASCII, &name[0], NULL, sizeof(name[0]));

    // position to the start of the resultset
    tc_errorcheck(resultset)->first ();

    do {
      static int first = true;

      // read data
      if (first) {
        tc_errorcheck(resultset)->getRowSet()->fetch ();
        first = false;
      } else
        tc_errorcheck(resultset)->getRowSet()->fetch ();

      // print data
      for (unsigned int i=0; i<resultset->getFetchedRows(); i++)
        printf("%d\t%s\n", id[i], name[i]);

      // position to the next part of the resultset
    } while ((retcode = resultset->next ()) == IFR_OK);

    // some error code should be printed, since no data could be read
    //    CheckRetcode (retcode, resultset);

    // close result set
    resultset->close();
  }
  return m_success;
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  IFR_TestFrame testframe (argc, argv);
  testframe.addTestCase (new updinsert ());
  if (testframe.run ()) 
    return 0;

  return 1;
}
