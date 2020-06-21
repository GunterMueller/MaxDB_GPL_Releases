/*!
  @file           EmptyString.cpp
  @author         D025086, D042427
  @ingroup        Tests
  @brief          Tests the correct conversion of empty strings
                  Tests fetching an empty string with bound parameters
  @keywords       empty string 
  @bugs           aaa: see PTS

  @see            @link http://pwww.bea.sap.corp:1111/Interfaces/SAPDB_8.0/SQLDBC_apidoc/EmptyString.html Hello world example @endlink 

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

class EmptyString : public SQLDBC_TestCase
{

public:
  EmptyString ()
    : SQLDBC_TestCase ("EmptyString")
  {}

  virtual SQLDBC_Bool run () {
    m_success |= Test(SQLDBC_HOSTTYPE_ASCII, SQLDBC_FALSE); // without termination
    m_success |= Test(SQLDBC_HOSTTYPE_UCS2, SQLDBC_FALSE); // without termination
    m_success |= Test(SQLDBC_HOSTTYPE_UTF8, SQLDBC_FALSE); // without termination
    m_success |= Test(SQLDBC_HOSTTYPE_ASCII, SQLDBC_TRUE); // with termination
    m_success |= Test(SQLDBC_HOSTTYPE_UCS2, SQLDBC_TRUE); // with termination
    m_success |= Test(SQLDBC_HOSTTYPE_UTF8, SQLDBC_TRUE); // with termination
    if (connection()->isUnicodeDatabase()) {
      
    }
    return m_success;
  }

  SQLDBC_Bool Test(SQLDBC_HostType hosttype, SQLDBC_Bool terminate) {

    SQLDBC_Retcode rc; 
    switch(hosttype) {
      case SQLDBC_HOSTTYPE_ASCII: {
        loginfo("Ascii");
        break;
      }
      case SQLDBC_HOSTTYPE_UCS2: {
        loginfo("UCS2");
        break;
      }
      case SQLDBC_HOSTTYPE_UTF8: {
        loginfo("UTF8");
        break;
      }
    }
    SQLDBC_Statement *stmt = connection()->createStatement();
    tc_errorcheckrc(rc, stmt)->execute("SELECT '' as ABC from DUAL");
    
    SQLDBC_ResultSet *res = stmt->getResultSet();
    if(!res)
      logerror("No resultset returned");

    char          par1[5];
    SQLDBC_Length ind1=-10;
    memset (&par1[0],'X',sizeof (par1));
    
    res->bindColumn(1, hosttype, &par1[0], &ind1, sizeof(par1), terminate);
    tc_errorcheck(res)->setRowSetSize(1);
    tc_errorcheck(res)->next();
        
    SQLDBC_RowSet *rset = res->getRowSet();
    if(!rset)
      logerror("No rowtset returned");
    
    tc_errorcheckrc(rc,rset)->fetch(); 
    if (rc == SQLDBC_NO_DATA_FOUND){
       logerror("Got SQLDBC_NO_DATA_FOUND where a result was expected.\n"); 
    }
    
    if (ind1 != 0){
      logerror("Wrong indicator value %d expected 0.\n", ind1); 
    } else {     
      loginfo("ind = %d\n", ind1);
    }
    if (!terminate) {
      if (strncmp(&par1[0], "XXXXXXXXXX", sizeof(par1))){
        logerror("Wrong result \"%.*s\" expected \"XXXXXXXXXX\".\n", sizeof(par1), par1); 
      } else {
        loginfo("result = >%.*s<\n", sizeof(par1), par1);
      }
    }
    else {
      // check numer of termination bytes
      int nz = (hosttype == SQLDBC_HOSTTYPE_UCS2) ? 2 : 1;
      for (int j=0; j < nz; j++) {
        if (par1[j] != 0) {
          logerror("Wrong result \"%.*s\" expected an emptytring", sizeof(par1), par1); 
          break;
        }
      }
      if (strncmp(&par1[nz], "XXXXXXXXXX", sizeof(par1)-nz)) {
        logerror("Wrong result the buffer is filled out of bounds\n"); 
      } else {
        loginfo("result is the empty string\n");
      }
    }
    connection()->releaseStatement(stmt);
    return m_success;
  }
};

class ESBind : public SQLDBC_TestCase
{

public:
  ESBind ()
    : SQLDBC_TestCase ("ESBind")
  {}
  
  virtual SQLDBC_Bool prepare ()
  {
    dropTable("TESTTABLE");
    execSQL("CREATE TABLE TESTTABLE (id integer key, text VARCHAR(255))");
    return m_success;
  }
  
  virtual SQLDBC_Bool release ()
  {
    dropTable("TESTTABLE");
    return m_success;
  }

  virtual SQLDBC_Bool run ()
  {
    m_success |= Test(SQLDBC_HOSTTYPE_ASCII, SQLDBC_SQLTYPE_VARCHARA);
    m_success |= Test(SQLDBC_HOSTTYPE_UCS2, SQLDBC_SQLTYPE_VARCHARA);
    m_success |= Test(SQLDBC_HOSTTYPE_UTF8, SQLDBC_SQLTYPE_VARCHARA);
    m_success |= Test(SQLDBC_HOSTTYPE_ASCII, SQLDBC_SQLTYPE_CHA);
    m_success |= Test(SQLDBC_HOSTTYPE_UCS2, SQLDBC_SQLTYPE_CHA);
    m_success |= Test(SQLDBC_HOSTTYPE_UTF8, SQLDBC_SQLTYPE_CHA);
    if (connection()->isUnicodeDatabase()) {
      m_success |= Test(SQLDBC_HOSTTYPE_ASCII, SQLDBC_SQLTYPE_VARCHARUNI);
      m_success |= Test(SQLDBC_HOSTTYPE_UCS2, SQLDBC_SQLTYPE_VARCHARUNI);
      m_success |= Test(SQLDBC_HOSTTYPE_UTF8, SQLDBC_SQLTYPE_VARCHARUNI);
      m_success |= Test(SQLDBC_HOSTTYPE_ASCII, SQLDBC_SQLTYPE_UNICODE);
      m_success |= Test(SQLDBC_HOSTTYPE_UCS2, SQLDBC_SQLTYPE_UNICODE);
      m_success |= Test(SQLDBC_HOSTTYPE_UTF8, SQLDBC_SQLTYPE_UNICODE);
    }
    return m_success;
  }

  SQLDBC_Bool Test(SQLDBC_HostType hosttype, SQLDBC_SQLType sqltype) 
  {
#define COLLEN (255)    
    SQLDBC_Retcode rc;
    static SQLDBC_Int2 id=0;
    id++;
    switch(hosttype) {
      case SQLDBC_HOSTTYPE_ASCII: {
        loginfo("Ascii");
        break;
      }
      case SQLDBC_HOSTTYPE_UCS2: {
        loginfo("UCS2");
        break;
      }
      case SQLDBC_HOSTTYPE_UTF8: {
        loginfo("UTF8");
        break;
      }
    }
    SQLDBC_PreparedStatement *pstmt = connection()->createPreparedStatement();

    dropTable("TESTTABLE");
    switch(sqltype) {
      case SQLDBC_SQLTYPE_VARCHARA: {
        m_success = execSQL("CREATE TABLE TESTTABLE (id integer key, text VARCHAR(255) ASCII)");
        loginfo("VARCHAR ASCII");
        break;
      }
      case SQLDBC_SQLTYPE_VARCHARUNI: {
        m_success = execSQL("CREATE TABLE TESTTABLE (id integer key, text VARCHAR(255) UNICODE)");
        loginfo("VARCHAR UNICODE");
        break;
      }
      case SQLDBC_SQLTYPE_CHA: {
        m_success = execSQL("CREATE TABLE TESTTABLE (id integer key, text CHAR(255) ASCII)");
        loginfo("CHAR ASCII");
        break;
      }
      case SQLDBC_SQLTYPE_UNICODE: {
        m_success = execSQL("CREATE TABLE TESTTABLE (id integer key, text CHAR(255) UNICODE)");
        loginfo("CHAR UNICODE");
        break;
      }
      default: {
        logerror("Unsupported sqltype in testcase");
        return m_success;
      }
    }
    if (!m_success) {
      logerror("Creation of test table failed.");
        return m_success;
    }
      
    tc_errorcheck(pstmt)->prepare("INSERT INTO TESTTABLE VALUES (?, ?)");

    char data[COLLEN*2+50];
    memset(&data, ' ', sizeof(data));
    strcpy(data, "  abc");
    SQLDBC_Length ind=0;
    tc_errorcheck(pstmt)->bindParameter(1, SQLDBC_HOSTTYPE_INT2, &id, NULL, sizeof(id));
    tc_errorcheck(pstmt)->bindParameter(2, hosttype, data, &ind, COLLEN);

    tc_errorcheck(pstmt)->execute();

    tc_errorcheck(pstmt)->prepare("SELECT text FROM TESTTABLE WHERE id = ?");
    tc_errorcheck(pstmt)->execute();
    SQLDBC_ResultSet *rs = tc_errorcheck(pstmt)->getResultSet();
    if (!rs) {
      logerror("No result set was returned");
      return m_success;
    }

    tc_errorcheck(rs)->next();
    ind=224;
    
    char ref[sizeof(data)];
    memset(ref, 0xBA, sizeof(ref));
    memcpy(data, ref, sizeof(ref));
    tc_errorcheckrc(rc, rs)->getObject(1, hosttype, data, &ind, sizeof(data));

    if (sqltype == SQLDBC_SQLTYPE_VARCHARA
        || sqltype == SQLDBC_SQLTYPE_VARCHARUNI) {
      short offs = 0;
      if (data[offs] != 0)
        logerror("Wrong data returned. Should have return an empty string.");
      offs++;
      if (hosttype == SQLDBC_HOSTTYPE_UCS2) {
        if (data[offs] != 0)
          logerror("Wrong data returned. Should have return an empty string.");
        offs++;
      }      
      if (memcmp(data+offs, ref+offs, sizeof(ref)-offs))
        logerror("Data was written out of bounds.");
    }
    else {
      short offs = 0;
      int collen = COLLEN;
      if (hosttype == SQLDBC_HOSTTYPE_UCS2)
        collen*=2;
      memset(ref, ' ', collen);
      if (hosttype == SQLDBC_HOSTTYPE_UCS2) {
        int i;
        for(i=0;i<collen;i+=2) {
          ref[i] = '\0';          
        }
      }
      ref[collen] = '\0';
      if (hosttype == SQLDBC_HOSTTYPE_UCS2) {
        offs++;
        ref[collen+offs] = '\0';
      }
      if (ind != collen)
        logerror("Wrong length returned.");
      if (hosttype != SQLDBC_HOSTTYPE_UCS2) {
        if (data[collen] != '\0')
          logerror("String not terminated.");
      }
      else
        if (data[collen] != '\0' || data[collen+offs] != '\0') {
          logerror("UCS2 String not terminated.");
        }
      if (memcmp(data, ref, collen+offs))
        logerror("Wrong data returned. Should return only spaces.");
      if (memcmp(data+collen+offs, ref+collen+offs, sizeof(ref)-collen-offs))
        logerror("Data was written out of bounds.");
    }
    return m_success;
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new EmptyString());
  testframe.addTestCase (new ESBind());
  if (testframe.run ()) 
    return 0;

  return 1;
}


