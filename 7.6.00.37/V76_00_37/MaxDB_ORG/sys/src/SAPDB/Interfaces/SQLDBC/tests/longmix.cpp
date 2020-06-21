/*!
  @file           longmix.cpp
  @author         D039759
  @ingroup        Tests
  @brief          Tests weird ordering of LONG columns on INSERT and UPDATE.
  

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

// http://pts:1080/webpts?wptsdetail=yes&ErrorType=0&ErrorID=1127276

static char bigbuf[200000];

class LongMix
    : public SQLDBC_TestCase
{
public:
    LongMix()
    : SQLDBC_TestCase("LONGMIX")
    {}

    SQLDBC_Bool run()
    {
        dropTable("LONGMIX");
        execSQL("CREATE TABLE LONGMIX(K INT PRIMARY KEY, L1 LONG ASCII, L2 LONG ASCII, L3 LONG ASCII)");
        execSQL("INSERT INTO LONGMIX VALUES(1, NULL, NULL, NULL)");
        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
        ps->prepare("UPDATE LONGMIX SET L3 = ?, L2 = ?, L1 =  ? WHERE K = 1");
        SQLDBC_Length ind=SQLDBC_NTS;
        const char *data="ABCDEF";
        memset(bigbuf, 'A', sizeof(bigbuf));
        SQLDBC_Length indbigbuf=sizeof(bigbuf);
        tc_errorcheck(ps)->bindParameter(1,SQLDBC_HOSTTYPE_ASCII, (void *)data, &ind, 7);
        tc_errorcheck(ps)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, bigbuf, &indbigbuf, sizeof(bigbuf), false);
        tc_errorcheck(ps)->bindParameter(3, SQLDBC_HOSTTYPE_ASCII, (void *)data, &ind, 7);
        tc_errorcheck(ps)->execute();
        connection()->commit();
        
        char          result[256];
        memset(result, 0, sizeof(result));
        SQLDBC_Length result_length;
        tc_errorcheck(ps)->prepare("SELECT L3 FROM LONGMIX WHERE K = 1");
        tc_errorcheck(ps)->execute();
        SQLDBC_ResultSet *rs=tc_errorcheck(ps)->getResultSet();
        tc_errorcheck(rs)->next();
        tc_errorcheck(rs)->getObject(1, SQLDBC_HOSTTYPE_ASCII, result, &result_length, sizeof(result));
        if(strcmp(result, "ABCDEF") != 0) {
            logerror("LONG DATA NOT CORRECTLY INSERTED.");
        }
        connection()->releaseStatement(ps);
        return m_success;
    }
};



int main (int argc, char **argv)
{
    // Initalize from command line params
    SQLDBC_TestFrame testframe (argc, argv);
    testframe.addTestCase (new LongMix());
    if (testframe.run ()) 
        return 0;
    return 1;
}


