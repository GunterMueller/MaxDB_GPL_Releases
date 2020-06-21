/*!================================================================

 module:        applparamtest.cpp

 -------------------------------------------------------------------

 responsible:   D039759

 special area:  SQL

 description:   A test that executes a prepared statement which needs 
                application-supplied parameters when being parsed,
                to be executed correctly.

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



//----------------------------------------------------------------------
class ApplParamTest : public IFR_TestCase
{
public:
    ApplParamTest()
    :IFR_TestCase ("ApplParamTest")
    {}
    
    virtual IFR_Bool run ()
    {
        SQL_TIMESTAMP_STRUCT buffer;
        IFR_Length   lengthindicator;
        lengthindicator = IFR_NULL_DATA;
        
        IFR_PreparedStmt *ps=tc_errorcheck(connection())->createPreparedStatement();
        tc_errorcheck(ps)->bindParameter(1, IFR_HOSTTYPE_ODBCTIMESTAMP, &buffer, &lengthindicator, 20);
        tc_errorcheck(ps)->prepare("SELECT 1, ? FROM DUAL", IFR_StringEncodingAscii);
        tc_errorcheck(ps)->execute();
        
        return m_success;
    }
};

//----------------------------------------------------------------------
class ApplParamTest2 : public IFR_TestCase
{
public:
    ApplParamTest2()
    :IFR_TestCase ("ApplParamTest2")
    {}
    
    virtual IFR_Bool run ()
    {
        IFR_Length   lengthindicator;
        lengthindicator = IFR_NULL_DATA;
        
        IFR_PreparedStmt *ps=tc_errorcheck(connection())->createPreparedStatement();
        char buffer1[100];
        char buffer2[100];
        IFR_Length buffer_ind=IFR_NTS;
        strcpy(buffer1, "1");
        strcpy(buffer2, "1");
        
        tc_errorcheck(ps)->bindParameter(1, IFR_HOSTTYPE_ASCII, buffer1, &buffer_ind, 100);
        tc_errorcheck(ps)->bindParameter(2, IFR_HOSTTYPE_ASCII, buffer2, &buffer_ind, 100);
        tc_errorcheck(ps)->prepare("SELECT SIN(?) + SIN(?) FROM DUAL", IFR_StringEncodingAscii);
        tc_errorcheck(ps)->execute();
        
        return m_success;
    }
};

//----------------------------------------------------------------------
int main (int argc, char **argv)
{
    
    // Initalize from command line params
    IFR_TestFrame testframe (argc, argv);
    testframe.addTestCase (new ApplParamTest2());
    testframe.addTestCase (new ApplParamTest());
    if (testframe.run ()) 
        return 0;
    
    return 1;
}
