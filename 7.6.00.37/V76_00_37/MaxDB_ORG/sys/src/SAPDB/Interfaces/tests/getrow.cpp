/*!
  @file           getrow.cpp
  @author         D025086
  @ingroup        Tests
  @brief          Simple Resultset test.
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

#define MAXROWS (2000)
#define ROWSETSIZE (21)
#define COLSIZE (30)
#define ROWFMT "ROW%d"

#define UNUSEDFMT "NOROW%d"

IFR_Bool memory_ok_ignored=true;


class GetRow : public IFR_TestCase
{
public:
  GetRow()
    :IFR_TestCase("GetRow")
  {}

  void fillRowSet(void *rowset, int elemsize, int rowsetsize, IFR_Length *ind)
  {
    for (int i=0; i < rowsetsize; i++) {
      char *p = (char*)rowset+elemsize*i;
      memset(p, ' ', elemsize);
      int len = sprintf(p, UNUSEDFMT, i);
      memset(&p[len], ' ', elemsize-len-1);
      p[elemsize-1] = '\0';
      ind[i] = 0x0BAD;
    }
  } 

  boolean CheckRows(int rownum, int numrows, int rowsetsize, 
                    void *rowset, int elemsize, IFR_Length *ind)
  {
    CheckRowSet(rownum, numrows, rowsetsize, 
                (char*)ROWFMT, rowset, elemsize, ind);
    /* check if other rows are untouched */
    CheckRowSet(numrows, rowsetsize-numrows, rowsetsize, 
                (char*)UNUSEDFMT, (char*)rowset+elemsize*(numrows), elemsize, NULL);
    return m_success;    
  }

  boolean CheckRowSet(int rownum, int numrows, int rowsetsize, 
                   char *rowfmt, void *rowset, int elemsize, IFR_Length *ind)
  { 
    char *col = new char[elemsize];
    for (int i=0; i < numrows; i++) {
      memset(col, ' ', elemsize);
      int len = sprintf(col, rowfmt, i+rownum);
      memset(&col[len], ' ', elemsize-len-1);
      col[elemsize-1] = '\0';
      if(memcmp(col, (char*)rowset+elemsize*i, elemsize))
        logerror ("Invalid column content '%s' expected '%s'\n", (char*)rowset+elemsize*i, col);
      if (ind && ind[i] != COLSIZE)
        logerror ("Invalid indicator value (%d) expected (%d)\n", ind[i], COLSIZE);
    }
    delete col;
    return m_success;
  }
  

  IFR_Bool Test1() {
    IFR_Retcode rc;
    IFR_RowSet *rowset;

    IFR_PreparedStmt *stmt = tc_errorcheck (connection ())->createPreparedStatement ();

    IFR_StringEncoding encoding = (connection()->isUnicodeDatabase()) ? 
      IFR_StringEncodingUCS2Native : IFR_StringEncodingAscii;
    
    IFR_String sql(encoding, connection()->allocator);

    sql.append("SELECT * FROM ABC", IFR_StringEncodingAscii, IFR_NTS, memory_ok_ignored);

    rc = tc_errorcheck (stmt)->prepare(sql.getBuffer(), sql.getEncoding());
    tc_errorcheck(stmt)->setResultSetType(IFR_Statement::SCROLL_SENSITIVE);
    if (rc != IFR_OK)
      logerror (stmt->error().getErrorText ());
    
    rc = tc_errorcheck (stmt)->execute();
    if (rc != IFR_OK)
      logerror (stmt->error().getErrorText ());
    
    IFR_ResultSet *resultset = tc_errorcheck (stmt)->getResultSet();

    if (!resultset) {
      logerror ("No Resultset returned\n", __FILE__, __LINE__);
      return false;
    }

    /* bind the row array */

    tc_errorcheck(resultset)->setRowSetSize(ROWSETSIZE);

    char abc[ROWSETSIZE][COLSIZE+1];
    IFR_Length *ind = new IFR_Length[ROWSETSIZE];
    tc_errorcheck(resultset)->bindColumn (1, IFR_HOSTTYPE_ASCII, 
			  abc[0], ind, sizeof(abc[0]));
    if (resultset->getResultCount() == 0) {
      printf("No resultset exists.\n");
    }

    /****** set to the last row ************************************/
    tc_errorcheck(resultset)->absolute(-1);
    int rownum = tc_errorcheck(resultset)->getRow();
    if (rownum != MAXROWS) {
      logerror ("Invalid position (%d) expected %d\n", rownum, MAXROWS);
    }

    rowset = resultset->getRowSet ();

    fillRowSet(abc, sizeof(abc[0]), ROWSETSIZE, ind);
    rowset->fetch();
    tc_errorcheck(resultset);
    
    int numrows = resultset->getFetchedRows();
    if (numrows != 1) {
      logerror ("Invalid number of rows fetched (%d) expected %d\n", numrows, 1);
    }
    printf("absolute(-1) rownum=%d, numrows=%d\n", rownum, numrows);
    CheckRows(rownum, numrows, ROWSETSIZE, abc, sizeof(abc[0]), ind);

    /****** set to the last 3 rows **********************************/
    tc_errorcheck(resultset)->absolute(-3);
    rownum = tc_errorcheck(resultset)->getRow();
    if (rownum != MAXROWS-2) {
      logerror ("Invalid position (%d) expected %d\n", rownum, MAXROWS-2);
    }

    fillRowSet(abc, sizeof(abc[0]), ROWSETSIZE, ind);
    rowset->fetch();
    tc_errorcheck(resultset);
    
    numrows = resultset->getFetchedRows();
    if (numrows != 3) {
      logerror ("Invalid number of rows fetched (%d) expected %d\n", numrows, ROWSETSIZE);
    }
    printf("absolute(-3) rownum=%d, numrows=%d\n", rownum, numrows);
    CheckRows(rownum, numrows, ROWSETSIZE, abc, sizeof(abc[0]), ind);

    /****** set to the first row *************************************/
    tc_errorcheck(resultset)->absolute(MAXROWS*-1);
    rownum = tc_errorcheck(resultset)->getRow();
    if (rownum != 1) {
      logerror ("Invalid position (%d) expected %d\n", rownum, 1);      
    }

    fillRowSet(abc, sizeof(abc[0]), ROWSETSIZE, ind);
    rowset->fetch();
    tc_errorcheck(resultset);
    
    numrows = resultset->getFetchedRows();
    if (numrows != ROWSETSIZE) {
      logerror ("Invalid number of rows fetched (%d) expected %d\n", numrows, ROWSETSIZE);
    }
    printf("absolute(%d) rownum=%d, numrows=%d\n", (MAXROWS*-1), rownum, numrows);
    CheckRows(rownum, numrows, ROWSETSIZE, abc, sizeof(abc[0]), ind);

    /***** fetch next rowset ******************************************/
    tc_errorcheck(resultset)->next();
    rownum = tc_errorcheck(resultset)->getRow();
    if (rownum != ROWSETSIZE+1) {
      logerror ("Invalid position (%d) expected %d\n", rownum, ROWSETSIZE);   
    }

    fillRowSet(abc, sizeof(abc[0]), ROWSETSIZE, ind);
    rowset->fetch();
    tc_errorcheck(resultset);
        
    numrows = resultset->getFetchedRows();
    if (numrows != ROWSETSIZE) {
      logerror ("Invalid number of rows fetched (%d) expected %d\n", numrows, ROWSETSIZE);
    }
    printf("next rownum=%d, numrows=%d\n", rownum, numrows);
    CheckRows(rownum, numrows, ROWSETSIZE, abc, sizeof(abc[0]), ind);
    /****************************************************/

    return m_success;
  }

  virtual IFR_Bool run()
  {
    char szSQL[512];
    m_success = true;
    dropTable ("ABC");

    sprintf(szSQL, "CREATE TABLE ABC (C CHAR (%d))", COLSIZE);
    m_success = m_success && execSQL (szSQL);
    if (m_success)
      for (int i=1;i<=MAXROWS;i++) {
        char szSQL[512];
        sprintf(szSQL, "INSERT INTO ABC VALUES ('"ROWFMT"')", i);
        m_success = m_success && execSQL(szSQL);
      }
    
    if(m_success) {
      execSQL("COMMIT WORK");
      m_success = true;
      m_success = m_success && Test1();
    }
    
    dropTable ("ABC");
    execSQL("COMMIT WORK");
    return m_success;
  } // end run
};

int main(int argc, char **argv)
{
  // Initalize from command line params
  IFR_TestFrame testframe(argc, argv);
  testframe.addTestCase(new GetRow());
  if(testframe.run()) 
    return 0;

  return 1;
}


