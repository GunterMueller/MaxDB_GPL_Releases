/*


    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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



*/
#include "SAPDB/Interfaces/sapdbifr.h"
#include "SAPDB/Interfaces/tests/TestUtils.h"
#include <limits.h>
#include <stdio.h>
#include <math.h>

IFR_Bool memory_ok_ignored=true;

//----------------------------------------------------------------------
class InvalidDataTest
    : public IFR_TestCase
{
public:
  InvalidDataTest()
    :IFR_TestCase("InvalidDataTest")
  {
  }
  
  void CheckRowStatus(IFR_PreparedStmt *stmt, int row, SQLDBC_Rowstatus code)
  {
      const IFR_Int4 *rowstat = stmt->getRowStatus();
    if (!rowstat)
      logerror("getRowStatus return nothing.\n");
    if (rowstat && rowstat[row] != code)
      logerror("rowstatus[0] contains a wrong status (%d). Rowstat should be (%d).\n", rowstat[row], code);
  }

  void Init()
  {
    dropTable("INVDATA");
    execSQL("CREATE TABLE INVDATA (C CHAR (10) ASCII, V VARCHAR(10) ASCII)");
  }

  void Free()
  {
    dropTable("INVDATA");
  }

  IFR_Bool Truncation(IFR_Bool unicode, IFR_Bool varchar)
  {
    
    IFR_Retcode rc;
    IFR_PreparedStmt *stmt=tc_errorcheck(connection())->createPreparedStatement();
    if (varchar) {
        IFR_String cmdInsert ("insert into INVDATA set V = ?"
                              , IFR_StringEncodingAscii, connection()->allocator, memory_ok_ignored);
      rc = tc_errorcheck (stmt)->prepare (cmdInsert.getBuffer(), cmdInsert.getEncoding());
    }
    else {
        IFR_String cmdInsert ("insert into INVDATA set C = ?"
                            , IFR_StringEncodingAscii, connection()->allocator, memory_ok_ignored);
      rc = tc_errorcheck (stmt)->prepare (cmdInsert.getBuffer(), cmdInsert.getEncoding());
    }
    
    // check truncation of databuffer
    char buffer[256];
    unsigned short buffer_w[sizeof(buffer)];

    strcpy(buffer, "1234567890");

    if (unicode)
      convertToUCS2(buffer_w, sizeof(buffer_w), buffer);
      
    IFR_Length bufferlen = IFR_NTS;    
    if (unicode)
      tc_errorcheck(stmt)->bindParameter(1, IFR_HOSTTYPE_UCS2_NATIVE, buffer_w, &bufferlen, sizeof(buffer_w));
    else
      tc_errorcheck(stmt)->bindParameter(1, IFR_HOSTTYPE_ASCII, buffer, &bufferlen, sizeof(buffer));

    tc_errorcheck(stmt)->execute();

    bufferlen = IFR_NTS;
    strcpy(buffer, "12345678901");        
    if (unicode)
      convertToUCS2(buffer_w, sizeof(buffer_w), buffer);
    { rc = tc_expectederrorcheck(stmt, -10401)->execute(); }
    if (rc != IFR_NOT_OK) 
      logerror("Wrong returncode (%d) has been returned. Error code should be (IFR_NOT_OK).\n", rc);      
    else {
      CheckRowStatus(stmt, 0, SQLDBC_EXECUTE_FAILED);
    }

    // check if rowstat will update during next call

    bufferlen = 10;

    if(unicode)
      bufferlen *= 2;

    rc = tc_errorcheck(stmt)->execute();
    CheckRowStatus(stmt, 0, SQLDBC_SUCCESS_NO_INFO);

    // check if truncation is reported for CHAR data if only 
    // trailing blanks are followed

    strcpy(buffer, "1234567890   ");        
    if (unicode)
      convertToUCS2(buffer_w, sizeof(buffer_w), buffer);

    bufferlen = strlen(buffer);

    if(unicode)
      bufferlen *= 2;
    if (varchar) {
      rc = tc_errorcheck(stmt)->execute();
      CheckRowStatus(stmt, 0, SQLDBC_SUCCESS_NO_INFO);
    }
    else {
      rc = tc_expectederrorcheck(stmt, -10401)->execute();      
      if (rc != IFR_NOT_OK) 
        logerror("Wrong returncode (%d) has been returned. Error code should be (IFR_NOT_OK).\n", rc);      
      else {
        CheckRowStatus(stmt, 0, SQLDBC_EXECUTE_FAILED);
      }
    }

    if (m_success)
      loginfo("Truncation unicode=%d, varchar=%d SUCCESS\n", unicode, varchar);
    else
      loginfo("Truncation unicode=%d, varchar=%d FAILED\n", unicode, varchar);

    return m_success;
  }

  IFR_Bool WrongData()
  {    
    IFR_Retcode rc;
    IFR_String cmdInsert ("insert into INVDATA set C = ?", IFR_StringEncodingAscii, connection()->allocator, memory_ok_ignored);
    IFR_PreparedStmt *stmt=tc_errorcheck(connection())->createPreparedStatement();
    rc = tc_errorcheck (stmt)->prepare (cmdInsert.getBuffer(), cmdInsert.getEncoding());
    
    // check truncation of databuffer
    char buffer[256];
    unsigned short buffer_w[sizeof(buffer)];

    strcpy(buffer, "1234567890");

    convertToUCS2(buffer_w, sizeof(buffer_w), buffer);

    
    IFR_Length bufferlen = IFR_NTS;    
    tc_errorcheck(stmt)->bindParameter(1, IFR_HOSTTYPE_UCS2_NATIVE, buffer_w, &bufferlen, sizeof(buffer_w));
    
    tc_errorcheck(stmt)->execute();

    // invalidate the unicode data so that is imposible to convert to ascii
    int ucs2=0x0BADF00D;
    memcpy(&buffer_w[3], &ucs2, sizeof(unsigned short));

    { tc_expectederrorcheck(stmt, -10405)->execute(); }
    CheckRowStatus(stmt, 0, SQLDBC_EXECUTE_FAILED);    
    
    if (m_success)
      loginfo("WrongData SUCCESS\n");
    else
      loginfo("WrongData FAILED\n");
    return m_success;
  }

  IFR_Bool run()
  {    
    this->Init();
    // CHAR
    this->Truncation(false, false);
    this->Truncation(true, false); // UNICODE
    // VARCHAR
    this->Truncation(false, true);
    this->Truncation(true, true); // UNICODE
    this->WrongData();
    this->Free();
    return m_success;
  }

};


int main(int argc, char **argv)
{
    IFR_TestFrame testframe(argc, argv);
    testframe.addTestCase(new InvalidDataTest());
    if(testframe.run()) return 0;
    return 1;
}
    
