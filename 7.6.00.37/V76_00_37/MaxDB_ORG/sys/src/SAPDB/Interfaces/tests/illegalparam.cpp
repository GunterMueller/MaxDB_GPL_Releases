/*!
  @file           FailedPreparedStmt.cpp
  @author         D025086
  @ingroup        Tests
  @brief          Check wrong execution of SQL Statements.
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
#include "SAPDB/Interfaces/sapdbifr.h"
#include "SAPDB/Interfaces/tests/TestUtils.h"
#include <limits.h>
#include <stdio.h>
#include <math.h>

//----------------------------------------------------------------------
class IllegalParameterTest
    : public IFR_TestCase
{
public:
    IllegalParameterTest()
    :IFR_TestCase("IllegalParameterTest")
    {}

    IFR_Bool run()
    {
        IFR_PreparedStmt *p1=tc_errorcheck(connection())->createPreparedStatement();
        char buffer[256];
        IFR_Length bufferlen;
        // Nullpointer for buffer used
        { tc_expectederrorcheck(p1, -10305)->bindParameter(1, IFR_HOSTTYPE_ASCII, 0, 0, 256); }
        // negative buffer size
        { tc_expectederrorcheck(p1, -10306)->bindParameter(1, IFR_HOSTTYPE_ASCII, buffer, &bufferlen, -1); }
        delete p1;
        return m_success;
    }
};


int main(int argc, char **argv)
{
    IFR_TestFrame testframe(argc, argv);
    testframe.addTestCase(new IllegalParameterTest());
    if(testframe.run()) return 0;
    return 1;
}
    
