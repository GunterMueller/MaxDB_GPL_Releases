/*!
  @file           unterm.cpp
  @author         D025086
  @ingroup        Tests
  @brief          Check if no terminated Strings are possible.
  @keywords       SQLDBC_HOSTTYPE_ASCII, terminated, C style, getObject

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
#include <string.h>

class applparam : public SQLDBC_TestCase
{
public:
  applparam ()
    : SQLDBC_TestCase ("applparam")
  {}

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_PreparedStatement *stmt = connection()->createPreparedStatement();

    const char *szSQL = "SELECT ? from DUAL ";

    loginfo("prepare without bind (-7016): '%s'", szSQL);
    tc_expectederrorcheck(stmt, -7016)->prepare(szSQL);

    char pdata[10];
    sprintf(pdata, "abcdef");

    char ppos[10];
    sprintf(ppos, "2");

    SQLDBC_Length ind=SQLDBC_NTS;

    tc_errorcheck(stmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, pdata, &ind, sizeof(pdata), SQLDBC_TRUE);
    
    loginfo("prepare with bind: '%s'", szSQL);
    tc_errorcheck(stmt)->prepare(szSQL);

    loginfo("execute");
    tc_errorcheck(stmt)->execute();

    char szCursorName[30];
    tc_errorcheck(stmt)->getCursorName(szCursorName, 
                                       SQLDBC_StringEncodingAscii,  
                                       sizeof(szCursorName),
                                       NULL);  

    loginfo("cursorname=%s", szCursorName);

    ///////////////////////////////////////////////

    szSQL = "SELECT ?, 1+? from DUAL ";

    loginfo("clearParameters");
    tc_errorcheck(stmt)->clearParameters();
    
    loginfo("prepare without bind: '%s'", szSQL);
    tc_expectederrorcheck(stmt, -7016)->prepare(szSQL);

    loginfo("bindParameter");
    tc_errorcheck(stmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, pdata, &ind, sizeof(pdata), SQLDBC_TRUE);
    
    loginfo("bindParameter");
    tc_errorcheck(stmt)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, ppos, &ind, sizeof(ppos), SQLDBC_TRUE);
    
    loginfo("prepare with bind: '%s'", szSQL);
    tc_errorcheck(stmt)->prepare(szSQL);

    loginfo("execute");
    tc_errorcheck(stmt)->execute();
    
    return m_success;
  }

};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new applparam());
  if (testframe.run ()) 
    return 0;
  return 1;
}


