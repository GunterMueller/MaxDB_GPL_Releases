/*!
  @file           ParamBind.cpp
  @author         D025086
  @ingroup        Tests
  @brief          Check binding of parameters before and after prepare.
  @todo           Half done, do some tests with wrong bindings.
                  
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

class BindParameter : public SQLDBC_TestCase
{
public:
  BindParameter ()
    : SQLDBC_TestCase ("BindParameter")
  {}

  virtual SQLDBC_Bool run ()
  {
    SQLDBC_Retcode rc;
    SQLDBC_PreparedStatement *stmt = connection()->createPreparedStatement();

    char pdata[10];
    sprintf(pdata, "1");

    SQLDBC_Length ind=SQLDBC_NTS;
    tc_errorcheckrc(rc, stmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, pdata, &ind, sizeof(pdata), SQLDBC_TRUE);
    
    const char *szSQL = "SELECT 'ONE' from dual where '1' = ?";

    tc_errorcheckrc(rc, stmt)->prepare(szSQL);

    tc_errorcheckrc(rc, stmt)->execute();

    SQLDBC_ResultSet *res = stmt->getResultSet();

    if(!res)
      logerror("No resultset returned");

    tc_errorcheck(res)->next();

    char cdata[50];
    tc_errorcheckrc(rc, res)->getObject(1, SQLDBC_HOSTTYPE_ASCII, cdata, &ind, sizeof(cdata));
    
    if (strcmp(cdata, "ONE")) {
      logerror("Wrong data returned");
    }

    loginfo("cdata=%s", cdata);

    return m_success;
  }

};

class BindParameterColumnWise : public SQLDBC_TestCase
{
public:
  BindParameterColumnWise ()
    : SQLDBC_TestCase ("BindParameterColumnWise")
  {}

  virtual SQLDBC_Bool run ()
  {
    #define BATCH 15
    SQLDBC_Retcode rc;

    int keys[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

    char *names[] = { "Congress",
                      "Long Island",
                      "Regency",
                      "Eight Avenue",
                      "Lake Michigan",
                      "Airport",
                      "Empire State", 
                      "Midtown", 
                      "Sunshine",
                      "Beach",
                      "Atlantic", 
                      "Long Beach",
                      "Star",
                      "River Boat",
                      "Indian Horse"
    };

    char *zips[] = { "20005",
                     "11788",
                     "20037",
                     "10019",
                     "60601",
                     "60018",
                     "12203",
                     "10019",
                     "33575",
                     "32018",
                     "33441",
                     "90804",
                     "90029",
                     "70112",
                     "92262"
    };

    char *addresses[] = { "155 Beechwood Str.",
                          "1499 Grove Street",
                          "477 17th Avenue",
                          "112 8th Avenue",
                          "354 OAK Terrace",
                          "650 C Parkway",
                          "65 Yellowstone Dr.",
                          "12 Barnard Str.",
                          "200 Yellowstone Dr.",
                          "1980 34th Str.",
                          "111 78th Str.",
                          "35 Broadway",
                          "13 Beechwood Place",
                          "788 MAIN STREET",
                          "16 MAIN STREET"
    };

    dropTable("SQLDBC_PARAMBIND");
    m_success = m_success && execSQL("CREATE TABLE SQLDBC_PARAMBIND(HNO INTEGER, NAME CHAR(15), ZIP CHAR(5), ADDRESS CHAR(25))");

    SQLDBC_PreparedStatement *prepstmt = connection()->createPreparedStatement();

    SQLDBC_Length key_ind[BATCH];
    SQLDBC_Length name_ind[BATCH];
    SQLDBC_Length zip_ind[BATCH];
    SQLDBC_Length address_ind[BATCH];

    memset (key_ind, 0, sizeof(key_ind));
    memset (name_ind, 0, sizeof(name_ind));
    memset (name_ind, 0, sizeof(name_ind));
    memset (zip_ind, 0, sizeof(zip_ind));
    memset (address_ind, 0, sizeof(address_ind));

    tc_errorcheckrc(rc, prepstmt)->prepare("insert into sqldbc_parambind values (?, ?,?,?)");
    tc_errorcheckrc(rc, prepstmt)->setBatchSize(BATCH);
    tc_errorcheckrc(rc, prepstmt)->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &keys[0], &key_ind[0], sizeof(SQLDBC_Int4));
    tc_errorcheckrc(rc, prepstmt)->bindParameterAddr(2, SQLDBC_HOSTTYPE_ASCII, &names[0], &name_ind[0], 13);
    tc_errorcheckrc(rc, prepstmt)->bindParameterAddr(3, SQLDBC_HOSTTYPE_ASCII, &zips[0], &zip_ind[0], 5);
    tc_errorcheckrc(rc, prepstmt)->bindParameterAddr(4, SQLDBC_HOSTTYPE_ASCII, &addresses[0], &address_ind[0], 19);

    int i;
    for (i = 0; i < BATCH; i++) {
      key_ind[i]      =  (SQLDBC_Length) sizeof(SQLDBC_Int4);
      name_ind[i]     =  (SQLDBC_Length) strlen((const char *) names[i]);
      zip_ind[i]      =  (SQLDBC_Length) strlen((const char *) zips[i]);
      address_ind[i]  =  (SQLDBC_Length) strlen((const char *) addresses[i]);
    }
    tc_errorcheckrc(rc, prepstmt)->execute ();

    SQLDBC_Statement *stmt = connection()->createStatement();
    stmt->setResultSetType (SQLDBC_Statement::SCROLL_SENSITIVE);
    tc_errorcheckrc(rc, stmt)->execute("SELECT * FROM SQLDBC_PARAMBIND");
    SQLDBC_ResultSet *rs = tc_errorcheck(stmt)->getResultSet();
    tc_errorcheck(rs)->setRowSetSize(BATCH);
    if(tc_errorcheck(rs)->next() == SQLDBC_NO_DATA_FOUND) {
      logerror("Expected data in call to next()");
      connection()->releaseStatement(stmt);  
    }

    SQLDBC_RowSet *rowset = rs->getRowSet();
    for (i = 0; i < BATCH; i++) {
      tc_errorcheckrc(rc, rowset)->setPos(i+1);
      char szString[30];
      SQLDBC_Int4 cbtmp0 = 0;
      SQLDBC_Int4 cbInt = 0;
      char sztmp1[15];
      char sztmp2[5];
      char sztmp3[25];
      SQLDBC_Length ind;

      memcpy (&cbtmp0, &keys[i], sizeof(SQLDBC_Int4));
      tc_errorcheckrc(rc, rowset)->getObject(1, SQLDBC_HOSTTYPE_INT4, &cbInt, &ind, sizeof(SQLDBC_Int4));
      if (cbtmp0 != cbInt) {
        logerror("Error. Data found: %d. Expected: %d.", cbInt, cbtmp0);
      }
      memset (sztmp1, ' ', sizeof(sztmp1));
      memcpy (sztmp1, names[i], strlen(names[i]));
      tc_errorcheckrc(rc, rowset)->getObject(2, SQLDBC_HOSTTYPE_ASCII, szString, &ind, sizeof(szString));
      if (memcmp (szString, sztmp1, 15)) {
        logerror("Error. Data found: %s. Expected: %s.", szString, names[i]);
      }
      memset (sztmp2, ' ', sizeof(sztmp2));
      memcpy (sztmp2, zips[i], strlen(zips[i]));
      tc_errorcheckrc(rc, rowset)->getObject(3, SQLDBC_HOSTTYPE_ASCII, szString, &ind, sizeof(szString));
      if (memcmp (szString, sztmp2, 5)) {
        logerror("Error. Data found: %s. Expected: %s.", szString, zips[i]);
      }
      memset (sztmp3, ' ', sizeof(sztmp3));
      memcpy (sztmp3, addresses[i], strlen(addresses[i]));
      tc_errorcheckrc(rc, rowset)->getObject(4, SQLDBC_HOSTTYPE_ASCII, szString, &ind, sizeof(szString));
      if (memcmp (szString, sztmp3, 25)) {
        logerror("Error. Data found: %s. Expected: %s.", szString, addresses[i]);
      }      
    }

    return m_success;
  }

};

class BindParameterRowWise : public SQLDBC_TestCase
{
public:
  BindParameterRowWise ()
    : SQLDBC_TestCase ("BindParameterRowWise")
  {}

  virtual SQLDBC_Bool run ()
  {
    #define BATCH 15
    SQLDBC_Retcode rc;

    typedef struct {
      SQLDBC_Int4     key;
      char           *name;
      char           *zip;
      char           *address;
      SQLDBC_Length   key_ind;
      SQLDBC_Length   name_ind;
      SQLDBC_Length   zip_ind;
      SQLDBC_Length   address_ind;
    } row;

    row rows[] = { { 1, "Congress", "20005", "155 Beechwood Str.", 0, 0, 0 },
                   { 2, "Long Island", "11788", "1499 Grove Street", 0, 0, 0 },
                   { 3, "Regency", "20037", "477 17th Avenue", 0, 0, 0 },
                   { 4, "Eight Avenue", "10019", "112 8th Avenue", 0, 0, 0 },
                   { 5, "Lake Michigan", "60601", "354 OAK Terrace", 0, 0, 0 },
                   { 6, "Airport", "60018", "650 C Parkway", 0, 0, 0 },
                   { 7, "Empire State", "12203", "65 Yellowstone Dr.", 0, 0, 0 },
                   { 8, "Midtown", "10019", "12 Barnard Str.", 0, 0, 0 },
                   { 9, "Sunshine", "33575", "200 Yellowstone Dr.", 0, 0, 0 },
                   { 10, "Beach", "32018", "1980 34th Str.", 0, 0, 0 },
                   { 11, "Atlantic", "33441", "111 78th Str.", 0, 0, 0 },
                   { 12, "Long Beach", "90804", "35 Broadway", 0, 0, 0 },
                   { 13, "Star", "90029", "13 Beechwood Place", 0, 0, 0 },
                   { 14, "River Boat", "70112", "788 MAIN STREET", 0, 0, 0 },
                   { 15, "Indian Horse", "92262", "16 MAIN STREET", 0, 0, 0 }
    };

    dropTable("SQLDBC_PARAMBIND");
    m_success = m_success && execSQL("CREATE TABLE SQLDBC_PARAMBIND(HNO INTEGER, NAME CHAR(15), ZIP CHAR(5), ADDRESS CHAR(25))");

    SQLDBC_PreparedStatement *prepstmt = connection()->createPreparedStatement();

    tc_errorcheckrc(rc, prepstmt)->prepare("insert into sqldbc_parambind values (?, ?,?,?)");
    tc_errorcheckrc(rc, prepstmt)->setBindingType(sizeof(row));
    tc_errorcheckrc(rc, prepstmt)->setBatchSize(BATCH);
    tc_errorcheckrc(rc, prepstmt)->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &rows[0].key, &rows[0].key_ind, sizeof(SQLDBC_Int4));
    tc_errorcheckrc(rc, prepstmt)->bindParameterAddr(2, SQLDBC_HOSTTYPE_ASCII, &rows[0].name   , &rows[0].name_ind   , rows[0].name_ind    );
    tc_errorcheckrc(rc, prepstmt)->bindParameterAddr(3, SQLDBC_HOSTTYPE_ASCII, &rows[0].zip    , &rows[0].zip_ind    , rows[0].zip_ind     );
    tc_errorcheckrc(rc, prepstmt)->bindParameterAddr(4, SQLDBC_HOSTTYPE_ASCII, &rows[0].address, &rows[0].address_ind, rows[0].address_ind );

    int i;
    for (i = 0; i < BATCH; i++) {
      rows[i].key_ind      =  (SQLDBC_Length) sizeof(SQLDBC_Int4); 
      rows[i].name_ind     =  (SQLDBC_Length) strlen((const char *) rows[i].name);
      rows[i].zip_ind      =  (SQLDBC_Length) strlen((const char *) rows[i].zip);
      rows[i].address_ind  =  (SQLDBC_Length) strlen((const char *) rows[i].address);
    }
    tc_errorcheckrc(rc, prepstmt)->execute ();

    SQLDBC_Statement *stmt = connection()->createStatement();
    stmt->setResultSetType (SQLDBC_Statement::SCROLL_SENSITIVE);
    tc_errorcheckrc(rc, stmt)->execute("SELECT * FROM SQLDBC_PARAMBIND");
    SQLDBC_ResultSet *rs = tc_errorcheck(stmt)->getResultSet();
    tc_errorcheck(rs)->setRowSetSize(BATCH);
    if(tc_errorcheck(rs)->next() == SQLDBC_NO_DATA_FOUND) {
      logerror("Expected data in call to next()");
      connection()->releaseStatement(stmt);  
    }

    SQLDBC_RowSet *rowset = rs->getRowSet();
    for (i = 0; i < BATCH; i++) {
      tc_errorcheckrc(rc, rowset)->setPos(i+1);
      char szString[30];
      SQLDBC_Int4 cbtmp0 = 0;
      SQLDBC_Int4 cbInt = 0;
      char sztmp1[15];
      char sztmp2[5];
      char sztmp3[25];
      SQLDBC_Length ind;

      memcpy (&cbtmp0, &rows[i].key, sizeof(SQLDBC_Int4));
      tc_errorcheckrc(rc, rowset)->getObject(1, SQLDBC_HOSTTYPE_INT4, &cbInt, &ind, sizeof(SQLDBC_Int4));
      if (cbtmp0 != cbInt) {
        logerror("Error. Data found: %d. Expected: %d.", cbInt, cbtmp0);
      }
      memset (sztmp1, ' ', sizeof(sztmp1));
      memcpy (sztmp1, rows[i].name, strlen(rows[i].name));
      tc_errorcheckrc(rc, rowset)->getObject(2, SQLDBC_HOSTTYPE_ASCII, szString, &ind, sizeof(szString));
      if (memcmp (szString, sztmp1, 15)) {
        logerror("Error. Data found: %s. Expected: %s.", szString, rows[i].name);
      }
      memset (sztmp2, ' ', sizeof(sztmp2));
      memcpy (sztmp2, rows[i].zip, strlen(rows[i].zip));
      tc_errorcheckrc(rc, rowset)->getObject(3, SQLDBC_HOSTTYPE_ASCII, szString, &ind, sizeof(szString));
      if (memcmp (szString, sztmp2, 5)) {
        logerror("Error. Data found: %s. Expected: %s.", szString, rows[i].zip);
      }
      memset (sztmp3, ' ', sizeof(sztmp3));
      memcpy (sztmp3, rows[i].address, strlen(rows[i].address));
      tc_errorcheckrc(rc, rowset)->getObject(4, SQLDBC_HOSTTYPE_ASCII, szString, &ind, sizeof(szString));
      if (memcmp (szString, sztmp3, 25)) {
        logerror("Error. Data found: %s. Expected: %s.", szString, rows[i].address);
      }      
    }

    return m_success;
  }

};

class BindParameterByAddr : public SQLDBC_TestCase
{
public:
  BindParameterByAddr ()
    : SQLDBC_TestCase ("BindParameterByAddr")
  {}

  virtual SQLDBC_Bool run ()
  {
    #define BATCHSIZE 2000
    #define MAXLEN 2123123
    SQLDBC_Retcode rc;

    /*raw data*/
    char* rows = "ROW1_SPALTE1ROW1_SPALTE2ROW1_VARIABLE_LONG_SPALTE" \ 
                 "ROW2_SPALTE1ROW2_SPALTE2ROW2_VARIABLE_LONG_SPALTE_ETWAS_LAENGER" \
                 "ROW3_SPALTE1ROW3_SPALTE2ROW3_VARIABLE_LONG_SPALTE_GANZ_GANZ_LANG" ;
                 
    SQLDBC_Length rowLen [3][3]= {{12,12,25},
                                  {12,12,39},
                                  {12,12,40}};

    /*pointer array containing binding*/
    char **col1 = new char* [BATCHSIZE];
    char **col2 = new char* [BATCHSIZE];
    char **col3 = new char* [BATCHSIZE];

    SQLDBC_Length col1_ind[BATCHSIZE];
    SQLDBC_Length col2_ind[BATCHSIZE];
    SQLDBC_Length col3_ind[BATCHSIZE];
                     
    int j,i,pos;
    for (j = 0; j < BATCHSIZE; j++) {
    	i = j%3;
    	if (i == 0) pos = 0;
    	col1[j]      = &rows[pos];
    	col1_ind[j]  = rowLen[i][0];
    	pos += col1_ind[j];
    	
    	col2[j]      = &rows[pos];
    	col2_ind[j]  = rowLen[i][1];
    	pos += col2_ind[j];

    	col3[j]      = &rows[pos];
    	col3_ind[j]  = rowLen[i][2];
    	pos += col3_ind[j];
    }
    
    dropTable("SQLDBC_PARAMBIND2");
    m_success = m_success && execSQL("CREATE TABLE SQLDBC_PARAMBIND2(SPALTE1 CHAR(12), SPALTE2 CHAR(12), LONG_SPALTE LONG ASCII)");

    SQLDBC_PreparedStatement *prepstmt = connection()->createPreparedStatement();

    tc_errorcheckrc(rc, prepstmt)->prepare("insert into sqldbc_parambind2 values (?,?,?)");
    
    /*binding*/
    tc_errorcheckrc(rc, prepstmt)->setBatchSize(BATCHSIZE);
    tc_errorcheckrc(rc, prepstmt)->bindParameterAddr(1, SQLDBC_HOSTTYPE_ASCII, col1, &col1_ind[0], MAXLEN, false );
    tc_errorcheckrc(rc, prepstmt)->bindParameterAddr(2, SQLDBC_HOSTTYPE_ASCII, col2, &col2_ind[0], MAXLEN, false  );
    tc_errorcheckrc(rc, prepstmt)->bindParameterAddr(3, SQLDBC_HOSTTYPE_ASCII, col3, &col3_ind[0], MAXLEN, false );

    /*execute*/
    tc_errorcheckrc(rc, prepstmt)->execute ();
    connection()->commit();

    /*select*/
    SQLDBC_Statement *stmt = connection()->createStatement();
    stmt->setResultSetType (SQLDBC_Statement::SCROLL_SENSITIVE);
    tc_errorcheckrc(rc, stmt)->execute("SELECT * FROM SQLDBC_PARAMBIND2");
    SQLDBC_ResultSet *rs = tc_errorcheck(stmt)->getResultSet();
    tc_errorcheck(rs)->setRowSetSize(BATCHSIZE);
    if(tc_errorcheck(rs)->next() == SQLDBC_NO_DATA_FOUND) {
      logerror("Expected data in call to next()");
      connection()->releaseStatement(stmt);  
    }

    SQLDBC_RowSet *rowset = rs->getRowSet();
    for (i = 0; i < BATCHSIZE; i++) {
      tc_errorcheckrc(rc, rowset)->setPos(i+1);
      char szString[50];
      char sztmp1[15];
      char sztmp2[15];
      char sztmp3[50];
      SQLDBC_Length ind;

      memset (sztmp1, ' ', sizeof(sztmp1));
      memcpy (sztmp1, col1[i], col1_ind[i]);
      tc_errorcheckrc(rc, rowset)->getObject(1, SQLDBC_HOSTTYPE_ASCII, szString, &ind, sizeof(szString));
      if (memcmp (szString, sztmp1, col1_ind[i])) {
        logerror("Error. Data found: %s. Expected: %.*s.", szString, col1_ind[i], col1[i]);
      }
      memset (sztmp2, ' ', sizeof(sztmp2));
      memcpy (sztmp2,  col2[i], col2_ind[i]);
      tc_errorcheckrc(rc, rowset)->getObject(2, SQLDBC_HOSTTYPE_ASCII, szString, &ind, sizeof(szString));
      if (memcmp (szString, sztmp2, col2_ind[i])) {
        logerror("Error. Data found: %s. Expected: %.*s.", szString, col2_ind[i], col2[i]);
      }
      memset (sztmp3, ' ', sizeof(sztmp3));
      memcpy (sztmp3,  col3[i], col3_ind[i]);
      tc_errorcheckrc(rc, rowset)->getObject(3, SQLDBC_HOSTTYPE_ASCII, szString, &ind, sizeof(szString));
      if (memcmp (szString, sztmp3, col3_ind[i])) {
        logerror("Error. Data found: %s. Expected: %.*s.", szString, col3_ind[i], col3[i]);
      }    
    }
    delete col1;
    delete col2;
    delete col3;

    return m_success;
  }

};

class BindParameterDA : public SQLDBC_TestCase
{
public:
  BindParameterDA ()
    : SQLDBC_TestCase ("BindParameterDA")
  {}

  virtual SQLDBC_Bool run ()
  {
    #define BATCHLENSIZE 3
    #define MAXLEN 2123123
    SQLDBC_Retcode rc;

    /*raw data*/
    char* rows = "ROW1_SPALTE1ROW1_SPALTE2_HIHI" \ 
                 "ROW2_SPALTE1ROW2_SPALTE2_HOHO" \
                 "ROW3_SPALTE1ROW3_SPALTE2_HAHA" ;
    
    SQLDBC_Length colLen [2]= {12,17};
    
    SQLDBC_Length recordLen = 29;

    dropTable("SQLDBC_PARAMBIND3");
    m_success = m_success && execSQL("CREATE TABLE SQLDBC_PARAMBIND3(SPALTE1 CHAR(12), SPALTE2 CHAR(17))");

    SQLDBC_PreparedStatement *prepstmt = connection()->createPreparedStatement();

    tc_errorcheckrc(rc, prepstmt)->prepare("insert into sqldbc_parambind3 values (?,?)");
    
    /*binding*/
    tc_errorcheckrc(rc, prepstmt)->setBindingType(recordLen);
    tc_errorcheckrc(rc, prepstmt)->setBatchSize(BATCHLENSIZE);
    tc_errorcheckrc(rc, prepstmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, &rows[0],          NULL, colLen[0], false );
    tc_errorcheckrc(rc, prepstmt)->bindParameter(2, SQLDBC_HOSTTYPE_ASCII, &rows[colLen[0]],  NULL, colLen[1], false  );
 
    /*execute*/
    tc_errorcheckrc(rc, prepstmt)->execute ();
    connection()->commit();

    /*select*/
    SQLDBC_Statement *stmt = connection()->createStatement();
    stmt->setResultSetType (SQLDBC_Statement::SCROLL_SENSITIVE);
    tc_errorcheckrc(rc, stmt)->execute("SELECT * FROM SQLDBC_PARAMBIND3");
    SQLDBC_ResultSet *rs = tc_errorcheck(stmt)->getResultSet();
    tc_errorcheck(rs)->setRowSetSize(BATCHLENSIZE);
    if(tc_errorcheck(rs)->next() == SQLDBC_NO_DATA_FOUND) {
      logerror("Expected data in call to next()");
      connection()->releaseStatement(stmt);  
    }

    SQLDBC_RowSet *rowset = rs->getRowSet();
    for (int i = 0; i < BATCHLENSIZE; i++) {
      tc_errorcheckrc(rc, rowset)->setPos(i+1);
      char szString[50];
      char sztmp1[15];
      char sztmp2[20];
      SQLDBC_Length ind;

      memset (sztmp1, ' ', sizeof(sztmp1));
      memcpy (sztmp1, &rows[i*recordLen], colLen[0]);
      tc_errorcheckrc(rc, rowset)->getObject(1, SQLDBC_HOSTTYPE_ASCII, szString, &ind, sizeof(szString));
      if (memcmp (szString, sztmp1, colLen[0])) {
        logerror("Error. Data found: %s. Expected: %.*s.", szString, colLen[0], &rows[i*recordLen]);
      }
      memset (sztmp2, ' ', sizeof(sztmp2));
      memcpy (sztmp2,  &rows[i*recordLen+colLen[0]], colLen[1]);
      tc_errorcheckrc(rc, rowset)->getObject(2, SQLDBC_HOSTTYPE_ASCII, szString, &ind, sizeof(szString));
      if (memcmp (szString, sztmp2, colLen[1])) {
        logerror("Error. Data found: %s. Expected: %.*s.", szString, colLen[1],  &rows[i*recordLen+colLen[0]]);
      }
    }
    return m_success;
  }
};

int main (int argc, char **argv)
{
  // Initalize from command line params
  SQLDBC_TestFrame testframe (argc, argv);
  testframe.addTestCase (new BindParameter());
  testframe.addTestCase (new BindParameterColumnWise());
  testframe.addTestCase (new BindParameterRowWise());
  testframe.addTestCase (new BindParameterByAddr());
  testframe.addTestCase (new BindParameterDA());
  if (testframe.run ()) 
    return 0;

  return 1;
}


