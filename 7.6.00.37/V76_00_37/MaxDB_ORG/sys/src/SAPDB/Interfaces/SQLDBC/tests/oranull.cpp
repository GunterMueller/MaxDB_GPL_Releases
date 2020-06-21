/*!
  @file           oranull.cpp
  @author         D039759
  @ingroup        Tests
  @brief          Test special behaviour for empty strings in ORACLE mode.
 
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

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"

class ORANULL : public SQLDBC_TestCase
{

public:
	ORANULL ()
		: SQLDBC_TestCase ("ORANULL")
	{}


	virtual SQLDBC_Bool run ()
	{
      dropTable("ORANULL");
      execSQL("CREATE TABLE ORANULL(E1 CHAR (10), E2 VARCHAR(10))");
      SQLDBC_PreparedStatement * ps=connection()->createPreparedStatement();
      tc_errorcheck(ps)->prepare("INSERT INTO ORANULL VALUES(?, ?)");
      
      char *v1 = "";
      char *v2 = "";
      SQLDBC_Length i1=SQLDBC_NTS;
      SQLDBC_Length i2=SQLDBC_NTS;
      tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, v1, &i1, 1, true);
      tc_errorcheck(ps)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, v2, &i2, 1, true);
      tc_errorcheck(ps)->execute();
      
      tc_errorcheck(ps)->prepare("SELECT COUNT(*) FROM ORANULL WHERE E1 IS NULL AND E2 IS NULL");
      tc_errorcheck(ps)->execute();
      SQLDBC_ResultSet *rs = ps->getResultSet();
      tc_errorcheck(rs)->next();
      SQLDBC_Int4 n;
      tc_errorcheck(rs)->getObject(1, SQLDBC_HOSTTYPE_INT4, &n, 0, sizeof(SQLDBC_Int4));
      if(n != 1) {
          logerror("NULL VALUES NOT FOUND.");
      }
      connection()->releaseStatement(ps);
      return m_success;
	}
};

int main (int argc, char **argv)
{
	// Initalize from command line params
	SQLDBC_TestFrame testframe (argc, argv);

	/* TODO: 
	 *
	 * add all test cases that you have described above
	 *
	 */
	testframe.addTestCase (new ORANULL());

	if (testframe.run ()) 
		return 0;

	return 1;
}


