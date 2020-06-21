/*!
  @file           datatrunc.cpp
  @author         D025086
  @ingroup        Tests
  @brief          1) Tests if truncation works for all non integral datatypes
  @brief          2) Tests if truncation is signaled for numeric datatypes
  @keywords       empty string 

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
#include "hsp78_0.h"
#include <stdlib.h>
#include <string.h>
class HostVar
{
public:
  //-------------------------------------------------
  HostVar(SQLDBC_HostType hosttype, SQLDBC_Length hostsize, char *c = NULL, SQLDBC_Bool isvarlen=false)
  {
    switch(hosttype) {
    case SQLDBC_HOSTTYPE_ASCII: {
      m_hostsize = hostsize;
      m_data = new char[m_hostsize];
      fillAsciiHostVar((c) ? c : (char*)"ASCII", isvarlen);
      m_ind = m_hostsize;
      break;
    }
    case SQLDBC_HOSTTYPE_UCS2:{}
    case SQLDBC_HOSTTYPE_UCS2_SWAPPED: {
      int swapped = (SQLDBC_HOSTTYPE_UCS2_SWAPPED == hosttype);
      m_hostsize = hostsize*2;
      m_data = new char[m_hostsize];
      fillUCS2HostVar((c) ? c : (char*)"UCS2", isvarlen);
      m_ind = m_hostsize;
      break;
    }
    case SQLDBC_HOSTTYPE_UTF8: {
      m_hostsize = hostsize*6;
      m_data = new char[m_hostsize];
      fillUTF8HostVar((c) ? c : (char*)"UTF8", isvarlen);

      unsigned int charLength, byteLength;
      int  isTerminated, isCorrupted, isExhausted;
      if (sp77encodingUTF8->stringInfo(m_data, m_hostsize, true, 
                                       &charLength,
                                       &byteLength,
                                       &isTerminated,
                                       &isCorrupted,
                                       &isExhausted))        
        exit(-1);
      m_ind = byteLength;                                     
      break;
    }
    default: {
      m_data = NULL;
      m_ind = SQLDBC_NULL_DATA;
    }
    }
  }

  template <class T>
  HostVar(SQLDBC_HostType hosttype, SQLDBC_Length hostsize, T value)
  {
    switch(hosttype) {
    case SQLDBC_HOSTTYPE_INT2: {
      m_hostsize = sizeof(T);
      m_data = new T;
      memcpy(m_data, &value, sizeof(value));
      m_ind = m_hostsize;
      break;
    }
    case SQLDBC_HOSTTYPE_FLOAT: {
      m_hostsize = sizeof(T);
      m_data = new T;
      memcpy(m_data, &value, sizeof(value));
      m_ind = m_hostsize;
      break;
    }
    default: {
      m_data = NULL;
      m_ind = SQLDBC_NULL_DATA;
    }
    }
  }

  ~HostVar() {
    delete m_data;
  }

  //-------------------------------------------------
  void *getRawPtr() { return m_data; };

  //-------------------------------------------------
  SQLDBC_Length *getIndPtr() { return &m_ind; };

  //-------------------------------------------------
  SQLDBC_Length getSize() { return m_hostsize; };

private:
  //-------------------------------------------------
  void fillAsciiHostVar(char *c, SQLDBC_Bool isvarlen=false)
  {
    SQLDBC_Length i;
    int l = (int)strlen(c);
    int filllen = (isvarlen) ? l : m_hostsize;
    for (i=0; i < filllen; i++) {
      ((char*)m_data)[i] = c[i%l];
    }
    for (; i < m_hostsize; i++) {
      ((char*)m_data)[i] = ' ';
    }
  };

  void fillUCS2HostVar(char *c, SQLDBC_Bool swapped=false, SQLDBC_Bool isvarlen = false)
  {
    
    SQLDBC_Length i;
    int firstIndex = ( swapped ? 0 : 1 );
    int l = (int)strlen(c);
    int filllen = (isvarlen) ? l*2 : m_hostsize;
    for (i=0; i < filllen; i+=2) {
      ((char*)m_data)[i+firstIndex] = c[(i/2)%l];
      ((char*)m_data)[i+1-firstIndex] = '\0';
    }
    for (; i < m_hostsize; i+=2) {
      ((char*)m_data)[i+firstIndex] = ' ';
      ((char*)m_data)[i+1-firstIndex] = '\0';
    }
  };

  void fillUTF8HostVar(char *c, SQLDBC_Bool isvarlen = false)
  {    
    SQLDBC_UInt4 destbyteswritten = 0;
    SQLDBC_UInt4 srcbytesparsed = 0;
    HostVar tmp(SQLDBC_HOSTTYPE_ASCII, this->getSize()/6, c, isvarlen);
    tsp78ConversionResult rc = sp78convertString(sp77encodingUTF8, 
                                                 this->getRawPtr(), this->getSize(), &destbyteswritten, 
                                                 false,
                                                 sp77encodingAscii, 
                                                 tmp.getRawPtr(), tmp.getSize(), &srcbytesparsed);
    if (rc != sp78_Ok)
      exit(-1);
    m_hostsize = destbyteswritten;
  };

  SQLDBC_Length m_hostsize;  //! Size of databuffer <var>m_data</var> in bytes
  void *m_data;
  SQLDBC_Length m_ind;
};

class DataTrunc : public SQLDBC_TestCase
{

public:
  DataTrunc ()
    : SQLDBC_TestCase ("DataTrunc")
  {}

  virtual SQLDBC_Bool run () {
        m_success |= runtest(true,  false);  // test with zero terminated strings
        m_success |= runtest(false, false);  // test with unterminated strings
        m_success |= runtest(true,  true);   // as bove but with unicode communication
        m_success |= runtest(false, true);
    return m_success;
  }

  SQLDBC_Bool runtest(SQLDBC_Bool terminate, SQLDBC_Bool unicode) {
    m_success |= Test(SQLDBC_HOSTTYPE_ASCII, terminate, SQLDBC_SQLTYPE_CHA, 10, unicode);
    m_success |= Test(SQLDBC_HOSTTYPE_UCS2, terminate, SQLDBC_SQLTYPE_CHA, 10, unicode);
    m_success |= Test(SQLDBC_HOSTTYPE_UTF8, terminate, SQLDBC_SQLTYPE_CHA, 10, unicode);
    m_success |= Test(SQLDBC_HOSTTYPE_ASCII, terminate, SQLDBC_SQLTYPE_VARCHARA, 10, unicode);
    m_success |= Test(SQLDBC_HOSTTYPE_UCS2, terminate, SQLDBC_SQLTYPE_VARCHARA, 10, unicode);
    m_success |= Test(SQLDBC_HOSTTYPE_UTF8, terminate, SQLDBC_SQLTYPE_VARCHARA, 10, unicode);
    if (connection()->isUnicodeDatabase()) {
      m_success |= Test(SQLDBC_HOSTTYPE_ASCII, terminate, SQLDBC_SQLTYPE_UNICODE, 10, unicode);
      m_success |= Test(SQLDBC_HOSTTYPE_UCS2, terminate, SQLDBC_SQLTYPE_UNICODE, 10, unicode);
      m_success |= Test(SQLDBC_HOSTTYPE_UTF8, terminate, SQLDBC_SQLTYPE_UNICODE, 10, unicode);      
      m_success |= Test(SQLDBC_HOSTTYPE_ASCII, terminate, SQLDBC_SQLTYPE_VARCHARUNI, 10, unicode);
      m_success |= Test(SQLDBC_HOSTTYPE_UCS2, terminate, SQLDBC_SQLTYPE_VARCHARUNI, 10, unicode);
      m_success |= Test(SQLDBC_HOSTTYPE_UTF8, terminate, SQLDBC_SQLTYPE_VARCHARUNI, 10, unicode);
    }
    return m_success;
  }

  SQLDBC_Bool Test(SQLDBC_HostType hosttype, SQLDBC_Bool terminate, SQLDBC_SQLType sqltype, int colsize, SQLDBC_Bool unicode ) {

    SQLDBC_Retcode rc; 
    char szColumnType[200];
    char szHostType[200];
    HostVar *cdata;

    cdata = new HostVar(hosttype, colsize );

    switch(sqltype) {
    case (SQLDBC_SQLTYPE_CHA) : {
      sprintf(szColumnType, "CHAR(%d) ASCII", colsize);
      break;
    }
    case (SQLDBC_SQLTYPE_VARCHARA) : {
      sprintf(szColumnType, "VARCHAR(%d) ASCII", colsize);
      break;
    }
    case (SQLDBC_SQLTYPE_UNICODE) : {
      sprintf(szColumnType, "CHAR(%d) UNICODE", colsize);
      break;
    }
    case (SQLDBC_SQLTYPE_VARCHARUNI) : {
      sprintf(szColumnType, "VARCHAR(%d) UNICODE", colsize);
      break;
    }
    default:
      logerror("Unsupported sqltype(%d)", sqltype);
      return false;
    }

    switch(hosttype) {
    case SQLDBC_HOSTTYPE_ASCII: {
      sprintf(szHostType ,"Ascii");
      break;
    }
    case SQLDBC_HOSTTYPE_UCS2: {
      sprintf(szHostType ,"UCS2");
      break;
    }
    case SQLDBC_HOSTTYPE_UTF8: {
      sprintf(szHostType ,"UTF8");
      break;
    }
    default : {
      logerror("Unsupported Hosttype");
      return m_success;
    }
    }
    loginfo("%s <-> %s %s %s", szHostType, szColumnType, 
            (terminate) ? "zero terminated" : "unterminated",
            (unicode) ? "unicode" : "");

    dropTable("DATATRUNC");

    char szSQL[500];
    sprintf(szSQL, "CREATE TABLE DATATRUNC ( c %s )", szColumnType);
    SQLDBC_Statement *stmt = connection()->createStatement();
    tc_errorcheckrc(rc, stmt)->execute(szSQL);
    if (rc != SQLDBC_OK)
      return m_success;
    connection()->releaseStatement(stmt);

    SQLDBC_PreparedStatement *stmt1 = connection()->createPreparedStatement();

    sprintf(szSQL, "INSERT INTO DATATRUNC VALUES (?)");
    if (unicode) {
      char szUCS2[sizeof(szSQL)*2];
      SQLDBC_UInt4 destbyteswritten = 0;
      SQLDBC_UInt4 srcbytesparsed = 0;
      tsp78ConversionResult rc = sp78convertString(sp77encodingUCS2, 
                                                   szUCS2, sizeof(szUCS2), &destbyteswritten, 
                                                   true,
                                                   sp77encodingAscii, 
                                                   szSQL, (int)strlen(szSQL), &srcbytesparsed);
      tc_errorcheck(stmt1)->prepare(szUCS2, SQLDBC_StringEncodingUCS2);
    }
    else {
      tc_errorcheck(stmt1)->prepare(szSQL);
    }
    tc_errorcheck(stmt1)->bindParameter(1, hosttype, cdata->getRawPtr(), cdata->getIndPtr(), cdata->getSize());
    tc_errorcheckrc(rc, stmt1)->execute();
    if (rc != SQLDBC_OK)
      return m_success;

    stmt = connection()->createStatement();

    tc_errorcheckrc(rc, stmt)->execute("SELECT c from DATATRUNC");
    
    SQLDBC_ResultSet *res = stmt->getResultSet();
    if(!res)
      logerror("No resultset returned");

    char *par1 = new char[colsize*10];
    memset (par1, 'X', colsize*10);

    SQLDBC_Length sizeofpar1;

    SQLDBC_Length ind1=0x0BAD0BAD;

    switch(hosttype) {
    case SQLDBC_HOSTTYPE_ASCII: {
      sizeofpar1 = 5;
      break;
    }
    case SQLDBC_HOSTTYPE_UCS2: {
      sizeofpar1 = 10;
      break;
    }
    case SQLDBC_HOSTTYPE_UTF8: {
      sizeofpar1 = 5;
      break;
    }
    default : {
      logerror("Unsupported Datatype");
      return m_success;
    }
    }

    tc_errorcheck(res)->bindColumn(1, hosttype, par1, &ind1, sizeofpar1, terminate);
    tc_errorcheck(res)->setRowSetSize(1);
    tc_errorcheck(res)->next();
        
    SQLDBC_RowSet *rset = res->getRowSet();
    if(!rset)
      logerror("No rowtset returned");
    
    tc_errorcheckrc(rc,rset)->fetch(); 
    if (rc == SQLDBC_NO_DATA_FOUND){
       logerror("Got SQLDBC_NO_DATA_FOUND where a result was expected.\n"); 
    }
    
    SQLDBC_Int4 numrows = rset->getRowsAffected();
    if (numrows != 1) {
       logerror("More than one row affected"); 
    }

    const SQLDBC_Int4 *rowstatus = rset->getRowStatus();
    if (!rowstatus) {
       logerror("No rowstatus returned");       
    }
    else {
      if (rowstatus[0] != SQLDBC_DATA_TRUNC) {
        logerror("rowstatus[0]=%d expected rowstatus[0]=%d", rowstatus[0], SQLDBC_DATA_TRUNC);       
      }
    }
    
    char *col = (char*)cdata->getRawPtr();
    SQLDBC_Length actcolsize = colsize;
    
    m_success |= CheckColumn(par1, hosttype, sizeofpar1, ind1, terminate, (SQLDBC_Retcode)rowstatus[0], &col, &actcolsize);

    col = (char*)cdata->getRawPtr();
    actcolsize = colsize;
    do {
      int loop=0;
      // getObject starts at position 0 independently of the data read from bindColumn
      // ODBC GD_BOUND isn't not possible with this behaviour 

      memset (par1, 'X', colsize*10);
      ind1=0x0BAD0BAD;
      tc_errorcheckrc(rc, res)->getObject(1, hosttype, par1, &ind1, sizeofpar1, terminate);

      if (rc != SQLDBC_NO_DATA_FOUND)
        m_success |= CheckColumn(par1, hosttype, sizeofpar1, ind1, terminate, rc, &col, &actcolsize);

      if(loop++ > 100)  //prevents an endless loop
        break; 
      
    } while(rc != SQLDBC_NO_DATA_FOUND);

    delete par1;
    delete cdata;

    connection()->releaseStatement(stmt);
    return m_success;
  }

  SQLDBC_Bool CheckColumn(char *par, 
                          SQLDBC_HostType hosttype, 
                          SQLDBC_Length sizeofpar, 
                          SQLDBC_Length ind,
                          SQLDBC_Bool terminate,
                          SQLDBC_Retcode rc,
                          char **col,
                          SQLDBC_Length *colsize)
  {
    int terminationBytes = (!terminate) ? 0 : (hosttype == SQLDBC_HOSTTYPE_UCS2) ? 2 : 1;

    SQLDBC_Length expectedlen = *colsize;

    if (hosttype == SQLDBC_HOSTTYPE_UCS2)
      expectedlen *=2;

    SQLDBC_Length bytesreturned = (rc == SQLDBC_DATA_TRUNC) ? sizeofpar - terminationBytes : expectedlen;

    // check return length
    if (ind != expectedlen){
      logerror("Invalid length(%d) returned expected length(%d).", ind, expectedlen); 
    }

    // check number of termination bytes
    for (int j=0; j < terminationBytes; j++) {
      if (par[bytesreturned+j] != 0) {
        logerror("The result string is not terminated"); 
        break;
      }
    }
    // check content
    if (memcmp(par, *col, bytesreturned)) {
      logerror("Wrong result '%.*s' expected '%.*s'", ind, par, bytesreturned, col); 
    }
    // check overwrite
    if (par[expectedlen+terminationBytes] != 'X') {
      logerror("The result buffer is filled out of bounds"); 
    }

    *col += bytesreturned;
    if (hosttype == SQLDBC_HOSTTYPE_UCS2)
      *colsize -= bytesreturned/2;
    else
      *colsize -= bytesreturned;
    return m_success;
  }
};

class NumericTrunc : public SQLDBC_TestCase
{
public:
  NumericTrunc ()
    : SQLDBC_TestCase ("NumericTrunc")
  {}
  virtual SQLDBC_Bool run () {
    m_success |= NumericTest(SQLDBC_HOSTTYPE_INT2, SQLDBC_SQLTYPE_FLOAT, 20, false);
    m_success |= NumericTest(SQLDBC_HOSTTYPE_FLOAT, SQLDBC_SQLTYPE_INTEGER, 0, true);
    return m_success;
  }
  SQLDBC_Bool NumericTest( SQLDBC_HostType hosttype, SQLDBC_SQLType sqltype, int colsize, SQLDBC_Bool truncated )
  {
    SQLDBC_Retcode rc; 
    char szColumnType[200];
    char szHostType[200];
    HostVar *cdata;

    switch(sqltype) {
    case (SQLDBC_SQLTYPE_FLOAT) : {
      sprintf(szColumnType, "FLOAT(%d)", colsize);
      break;
    }
    case (SQLDBC_SQLTYPE_INTEGER) : {
      sprintf(szColumnType, "INTEGER");
      break;
    }
    case (SQLDBC_SQLTYPE_SMALLINT) : {
      sprintf(szColumnType, "SMALL");
      break;
    }
    default:
      logerror("Unsupported sqltype(%d)", sqltype);
      return false;
    }

    switch(hosttype) {
    case SQLDBC_HOSTTYPE_INT2: {
      cdata = new HostVar(hosttype, colsize, 1234 );
      sprintf(szHostType ,"2 byte int");
      break;
    }
    case SQLDBC_HOSTTYPE_FLOAT: {
      cdata = new HostVar(hosttype, colsize, (float)1234.12 );
      sprintf(szHostType ,"float");
      break;
    }
    default : {
      logerror("Unsupported Hosttype");
      return m_success;
    }
    }

    dropTable("DATATRUNC");

    char szSQL[500];
    sprintf(szSQL, "CREATE TABLE DATATRUNC ( c %s )", szColumnType);
    SQLDBC_Statement *stmt = connection()->createStatement();
    tc_errorcheckrc(rc, stmt)->execute(szSQL);
    if (rc != SQLDBC_OK)
      return m_success;
    connection()->releaseStatement(stmt);

    loginfo("%s <-> %s", szHostType, szColumnType);

    SQLDBC_PreparedStatement *stmt1 = connection()->createPreparedStatement();

    sprintf(szSQL, "INSERT INTO DATATRUNC VALUES (?)");

    tc_errorcheck(stmt1)->prepare(szSQL);

    tc_errorcheck(stmt1)->bindParameter(1, hosttype, cdata->getRawPtr(), cdata->getIndPtr(), cdata->getSize());

    tc_errorcheckrc(rc, stmt1)->execute();
    if (truncated && rc != SQLDBC_DATA_TRUNC)
      logerror("SQLDBC_DATA_TRUNC expected but rc was %d", rc);
    
    return m_success;
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new DataTrunc());
  testframe.addTestCase (new NumericTrunc());
  if (testframe.run ()) 
    return 0;

  return 1;
}


