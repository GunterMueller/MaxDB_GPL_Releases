/*!================================================================

 module:        QueryStmt.cpp

 -------------------------------------------------------------------

 responsible:   D030044

 special area:  SQL

 description:   Example: Test for Resultset

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

class QueryStmt : public IFR_TestCase
{
public:
  QueryStmt ()
    : IFR_TestCase ("QueryStmt")
  {}

  virtual IFR_Bool run ()
  {
    IFR_Retcode rc;
    IFR_ResultSet *ResultSet = 0;
    IFR_RowSet *rowset = 0;
    char *cmdSelect=(char *) "select J, I from PACKET4";
    IFR_Statement *stmt = tc_errorcheck (connection ())->createStatement ();
    stmt->setResultSetType(IFR_Statement::SCROLL_SENSITIVE);
    rc = tc_errorcheck (stmt)->execute (cmdSelect, IFR_StringEncodingAscii);
    if (rc != IFR_OK)
      logerror (stmt->error().getErrorText ());
    ResultSet = tc_errorcheck(stmt)->getResultSet ();
    rowset = ResultSet->getRowSet ();
    char s[16];
    IFR_Length ind;

    rc = ResultSet->absolute (1);
    strcpy(s, "               ");
    if (rowset->getObject (1, s, sizeof(s), IFR_HOSTTYPE_ASCII, &ind, true) != IFR_OK)
      logerror ("%s\n", ResultSet->error().getErrorText());
    else {
      printf ("s: %s\n", s);
      if (strcmp(s, "J00001"))
        logerror ("Wrong value returned %s expected J00001\n", s);          
    }
    strcpy(s, "               ");
    if (rowset->getObject (2, s, sizeof(s), IFR_HOSTTYPE_ASCII, &ind, true) != IFR_OK)
      logerror ("%s\n", ResultSet->error().getErrorText());
    else {
      if (strcmp(s, "1"))
        logerror ("Wrong value returned %s expected 1\n", s);          
      printf ("s: %s\n", s);
    }
    rc = ResultSet->absolute (MAX_IFR_INT2+1);
    strcpy(s, "               ");
    if (rowset->getObject (1, s, sizeof(s), IFR_HOSTTYPE_ASCII, &ind, true) != IFR_OK)
      logerror ("%s\n", ResultSet->error().getErrorText());
    else {
      if (strcmp(s, "J32768"))
        logerror ("Wrong value returned %s expected J32768\n", s);          
      printf ("s: %s\n", s);
    }
    strcpy(s, "               ");
    if (rowset->getObject (2, s, sizeof(s), IFR_HOSTTYPE_ASCII, &ind, true) != IFR_OK)
      logerror ("%s\n", ResultSet->error().getErrorText());
    else {
      if (strcmp(s, "32768"))
        logerror ("Wrong value returned %s expected 32768\n", s);          
      printf ("s: %s\n", s);
    }
    strcpy(s, "               ");
    rc = ResultSet->absolute (1);
    if (rowset->getObject (1, s, sizeof(s), IFR_HOSTTYPE_ASCII, &ind, true) != IFR_OK)
      logerror ("%s\n", ResultSet->error().getErrorText());
    else {
      if (strcmp(s, "J00001"))
        logerror ("Wrong value returned %s expected J00001\n", s);          
      printf ("s: %s\n", s);
    }
    strcpy(s, "               ");
    if (rowset->getObject (2, s, sizeof(s), IFR_HOSTTYPE_ASCII, &ind, true) != IFR_OK)
      logerror ("%s\n", ResultSet->error().getErrorText());
    else {
      if (strcmp(s, "1"))
        logerror ("Wrong value returned %s expected 1\n", s);          
      printf ("s: %s\n", s);
    }

    execSQL ("COMMIT WORK");
    return m_success;
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  IFR_TestFrame testframe (argc, argv);
  testframe.addTestCase (new QueryStmt ());
  if (testframe.run ()) 
    return 0;

  return 1;
}
