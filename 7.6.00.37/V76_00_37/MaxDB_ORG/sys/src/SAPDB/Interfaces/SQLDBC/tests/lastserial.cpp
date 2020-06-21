/*!
  @file           lastserial.cpp
  @author         D039759
  @ingroup        Tests
  @brief          Checks retrieval of last inserted serial id.
  

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

/*
class XXX
    : public SQLDBC_TestCase
{
public:
    XXX()
    :SQLDBC_TestCase("XXX")
    {}

    SQLDBC_Bool run()
    {
        return m_success;
    }
};
*/

class LastSerial
    : public SQLDBC_TestCase
{
public:
    LastSerial()
    :SQLDBC_TestCase("LastSerial")
    {}

    SQLDBC_Bool run()
    {
        dropTable("TEST_LASTSERIAL");
        execSQL("CREATE TABLE TEST_LASTSERIAL(A INTEGER DEFAULT SERIAL, B INTEGER DEFAULT 0)");
        SQLDBC_Statement *s = connection()->createStatement();
        tc_errorcheck(s)->execute("INSERT INTO TEST_LASTSERIAL VALUES (DEFAULT, 1)");

        char value[20];
        SQLDBC_Length value_ind = 0;
        
        tc_errorcheck(s)->getLastInsertedKey(SQLDBC_FIRST_INSERTED_SERIAL, 
                                             SQLDBC_HOSTTYPE_ASCII,
                                             value,
                                             &value_ind,
                                             sizeof(value));
        
        loginfo("FIRST KEY WAS %s ", value);
        
        
        connection()->releaseStatement(s);
        return m_success;
    }
};


int main (int argc, char **argv)
{
    // Initalize from command line params
    SQLDBC_TestFrame testframe (argc, argv);
    testframe.addTestCase (new LastSerial());
    if (testframe.run ()) 
        return 0;
    return 1;
}


