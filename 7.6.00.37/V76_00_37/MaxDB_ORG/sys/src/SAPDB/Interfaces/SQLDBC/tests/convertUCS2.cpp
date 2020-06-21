/*!
  @file           convertUCS2.cpp
  @author         D025086
  @ingroup        Tests
  @brief          Check the dtatypes UCS2 and UCS2 swapped datatypes
  @todo           
                  
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

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"
#include "SAPDB/SAPDBCommon/SAPDB_sprintf.h"
#include <string.h>

class Convert : public SQLDBC_TestCase
{
public:
  Convert ()
    : SQLDBC_TestCase ("Convert")
  {
  }

  virtual SQLDBC_Bool prepare ()
  {
    dropTable("Convert");
    m_success = m_success && execSQL("CREATE TABLE Convert (A VARCHAR(10))");
    return m_success;
  }
   
  virtual SQLDBC_Bool release ()
  {
    dropTable("Convert");
    return m_success;
  }

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_PreparedStatement *stmt = connection()->createPreparedStatement();

    const char *szSQL = "INSERT INTO Convert values(?)";

    tc_errorcheck(stmt)->prepare(szSQL);
    
    char cdata[22];
    const char *UCS2 = "UCS2";

    SQLDBC_Length ind = SQLDBC_NTS;
    
    tc_errorcheck(stmt)->bindParameter(1, SQLDBC_HOSTTYPE_UCS2, cdata, &ind, sizeof(cdata));

    SAPDB_sprintfUnicode(SAPDB_EncodingUCS2, cdata, sizeof(cdata), UCS2);

    SQLDBC_Retcode rc = tc_errorcheck(stmt)->execute();
    
    tc_errorcheck(stmt)->bindParameter(1, SQLDBC_HOSTTYPE_UCS2_SWAPPED, cdata, &ind, sizeof(cdata));
    
    SAPDB_sprintfUnicode(SAPDB_EncodingUCS2Swapped, cdata, sizeof(cdata), UCS2);
    rc = tc_errorcheck(stmt)->execute();
    
    SQLDBC_Statement *select = connection()->createStatement();
    
    tc_errorcheck(select)->execute("SELECT * FROM convert");

    SQLDBC_ResultSet *resultset = select->getResultSet();
    if(!resultset) {
      logerror("getResultSet failed\n");
    }
    
    while(resultset->next() == SQLDBC_OK) {
      tc_errorcheck(resultset)->getObject(1, SQLDBC_HOSTTYPE_ASCII, &cdata, &ind, sizeof(cdata));
      loginfo("%s\n", cdata);
      if (strcmp(UCS2, cdata)) {
        logerror("Wrong data returned %s,%s\n", cdata, UCS2);
      }
      memset(cdata, '\0', sizeof(cdata));
    }    
    
    return m_success;
  }

};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new Convert());
  if (testframe.run ()) 
    return 0;

  return 1;
}


