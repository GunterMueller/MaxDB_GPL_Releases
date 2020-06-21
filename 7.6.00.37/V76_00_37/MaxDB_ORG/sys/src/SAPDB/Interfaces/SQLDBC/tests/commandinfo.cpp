/*!
  @file           commandinfo.cpp
  @author         D031096
  @ingroup        Tests
  @brief          Check info command part.
                  
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

class commandInfoSAP : public SQLDBC_TestCase
{
public:
    commandInfoSAP()
    :SQLDBC_TestCase("commandInfoSAP")
    {}

    SQLDBC_Bool run()
    {
        execSQL("DIAGNOSE MONITOR PARSEID OFF");
        
        SQLDBC_PreparedStatement *ps = connection()->createPreparedStatement();
        ps->prepare("SELECT 1 FROM DUAL");
        ps->setCommandInfo("TEST1",
                           5,
                           4711);
        ps->execute();
        execSQL("DIAGNOSE MONITOR READ 0");
        execSQL("DIAGNOSE MONITOR PARSEID ON");
        execSQL("DELETE FROM SYSPARSEID");
        
        ps->execute();
        SQLDBC_Statement *s = connection()->createStatement();
        
        tc_errorcheck(s)->execute("SELECT LINE FROM SYSPARSEID WHERE JOB = 'TEST1'");
        SQLDBC_ResultSet *rs = s->getResultSet();
        if(rs) {
            if(rs->next() != SQLDBC_OK) {
                logerror("Did not find info.");
            }
        } else {
            logerror("Query did not work.");
        }
        
        execSQL("DIAGNOSE MONITOR OFF");
        return m_success;
    }
    
};


class commandinfo : public SQLDBC_TestCase
{
public:
  commandinfo ()
    : SQLDBC_TestCase ("commandinfo")
  {}
  
  SQLDBC_Bool checkSysParseid(char* cmdinfo, SQLDBC_Int4 lineno, char* sqlStmt){
    SQLDBC_Retcode rc;
    SQLDBC_Length ind1 = SQLDBC_NTS;
    SQLDBC_Length ind2 = SQLDBC_NTS;
    SQLDBC_Length ind3 = SQLDBC_NTS;
    const char* szSQL = "Select 'HOMER' from sysparseid where sql_Statement=? and job=? and line=?";

    SQLDBC_PreparedStatement *stmt = connection()->createPreparedStatement();
    tc_errorcheckrc(rc, stmt)->prepare(szSQL);
    tc_errorcheckrc(rc, stmt)->bindParameter  (1, SQLDBC_HOSTTYPE_ASCII, sqlStmt, &ind1, strlen (sqlStmt));  
    tc_errorcheckrc(rc, stmt)->bindParameter  (2, SQLDBC_HOSTTYPE_ASCII, cmdinfo, &ind2, strlen (cmdinfo));  
    tc_errorcheckrc(rc, stmt)->bindParameter  (3, SQLDBC_HOSTTYPE_INT4,  &lineno,  &ind3, sizeof(SQLDBC_Int4));  
    tc_errorcheckrc(rc, stmt)->execute();
    SQLDBC_ResultSet *res = stmt->getResultSet();

    if(!res)
      logerror("No resultset returned");

    rc =res->next();

    if (rc == SQLDBC_NO_DATA_FOUND || rc == SQLDBC_NOT_OK){
      connection()->releaseStatement (stmt); 
      return SQLDBC_FALSE;
    }

    SQLDBC_Length ind;
    char cdata[50];
    tc_errorcheckrc(rc, res)->getObject(1, SQLDBC_HOSTTYPE_ASCII, cdata, &ind, sizeof(cdata));
    
    if (strcmp(cdata, "HOMER")) {
      logerror("Wrong data returned");
    }
    loginfo("cdata=%s", cdata);
    connection()->releaseStatement (stmt); 
    return SQLDBC_TRUE;
  }

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_Retcode rc;
    char cmdinfo[]="Eat my shorts";
    SQLDBC_Int4 lineno = 42;
    tc_errorcheckrc(rc, statement())->execute("DIAGNOSE MONITOR PARSEID ON");
    tc_errorcheckrc(rc, statement())->execute("DELETE FROM SYSPARSEID");
    
    SQLDBC_PreparedStatement *stmt = connection()->createPreparedStatement();
    char szSQL[200]; 
    sprintf (szSQL,"SELECT 'HOMER' FROM DUAL");

    tc_errorcheckrc(rc, stmt)->setCommandInfo(cmdinfo, 
                               sizeof(cmdinfo), 
                               lineno);     
                                             
    tc_errorcheckrc(rc, stmt)->prepare(szSQL);
    tc_errorcheckrc(rc, stmt)->execute();
    
    //checkSysParseid();
    SQLDBC_Bool checkCmdInfo = checkSysParseid(&cmdinfo[0], lineno, (char*)szSQL);

    if (! checkCmdInfo) {
      logerror("Command info not found in system table");
    }
    loginfo("correct command info found");


    sprintf (szSQL,"SELECT 'BART' FROM DUAL");
    tc_errorcheckrc(rc, stmt)->prepare(szSQL);
    tc_errorcheckrc(rc, stmt)->execute();
    
    //checkSysParseid();
    checkCmdInfo = checkSysParseid(&cmdinfo[0], lineno, (char*)szSQL);

    if (checkCmdInfo) {
      logerror("Unexpected command info found in system table");
    }else {
        loginfo("no command info found");
    }
    tc_errorcheckrc(rc, statement())->execute("DIAGNOSE MONITOR PARSEID OFF");
    tc_errorcheckrc(rc, statement())->execute("DELETE FROM SYSPARSEID");
    return m_success;
  }

};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new commandinfo());
  testframe.addTestCase (new commandInfoSAP());
  if (testframe.run ()) 
    return 0;

  return 1;
}


