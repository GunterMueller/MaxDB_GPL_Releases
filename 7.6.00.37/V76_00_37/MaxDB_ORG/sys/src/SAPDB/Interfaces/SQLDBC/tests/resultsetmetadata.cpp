/*!================================================================

 module:        resultretmetadata.cpp
 -------------------------------------------------------------------

 responsible:   MarcoP

 special area:  SQL

 description:   A test that checks the getParameterMetaData/getResultSetMetaData of
                of a Prepared Statement.

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

#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"
#include <string.h>

SQLDBC_Bool memory_ok_ignored=true;

//----------------------------------------------------------------------



//----------------------------------------------------------------------

enum nullable {
    nullVal   = 1,
    nonullVal = 0
};

class MetadataTestCase {
public:
  
  const char *sqlcmd;
  int         columncount;
  const char **colname;
  SQLDBC_SQLType *coltype;
  int         *collength;
  int         *colphysLength;
  nullable    *colnullval;
  int         *colprecison;
  int         *colscale;
  
 MetadataTestCase (  const char        *asqlcmd,
                      int         acolumncount,
                      const char  **acolname,
                      SQLDBC_SQLType *acoltype,
                      int         *acollength,
                      int         *acolphysLength,
                      nullable    *acolnullval,
                      int         *acolprecison,
                      int         *acolscale
                   ):
 sqlcmd (asqlcmd), 
 columncount(acolumncount),                          
 colname(acolname),
 coltype(acoltype),
 collength(acollength),
 colphysLength(acolphysLength),
 colnullval(acolnullval),
 colprecison(acolprecison),
 colscale(acolscale)
  {}
};

class ResultSetMetaData : public SQLDBC_TestCase
{
public:
    ResultSetMetaData()
    :SQLDBC_TestCase ("ResultSetMetaData")
    {}

    void checkResultSetMetaData (SQLDBC_ResultSetMetaData* psmd, MetadataTestCase& mdTest){
    
    loginfo("Check Columns: %s", mdTest.sqlcmd);       
    if (psmd->getColumnCount()!= mdTest.columncount)
      logerror("Invalid Number of columns: %d expected %d", psmd->getColumnCount(), mdTest.columncount);        
    if (psmd->getColumnCount())
      for (int i = 1; i <= psmd->getColumnCount(); ++i) {
        /*check pramDataType*/ 
         if ( psmd->getColumnType(i)!= mdTest.coltype[i-1]){
           logerror("Invalid column type - column: %02d columntype: >%d< expected >%d<", 
                        i, 
                        psmd->getColumnType(i),
                        mdTest.coltype[i-1]
                    );
          }

         char       columname_buffer[512];
         SQLDBC_Length bufferlen;

         psmd->getColumnName(i, columname_buffer, SQLDBC_StringEncodingAscii, 512, &bufferlen);

         /*check paramnames*/
         if ( ! (*(mdTest.colname[i-1]))){
           if( bufferlen != 0){
             logerror("Invalid column name - column: %02d columnName: >%.*s<expected >%s<", 
                      i, 
                      bufferlen,
                      columname_buffer,
                      mdTest.colname[i-1]);          
           }           
         } else {
           if ( strncmp(columname_buffer, mdTest.colname[i-1], strlen(mdTest.colname[i-1]))){
             logerror("Invalid column name - column: %02d columnName: >%.*s< expected >%s<", 
                        i, 
                        bufferlen,
                        columname_buffer,
                        mdTest.colname[i-1]);
          }
        }
        //check parlength;
        if ( psmd->getColumnLength(i)!= mdTest.collength[i-1]){
           logerror("Invalid column length - column: %02d columnLength: >%d< expected >%d<", 
                        i, 
                        psmd->getColumnLength(i),
                        mdTest.collength[i-1]
                    );
          }        
       //check *parphysLength;
       if ( psmd->getPhysicalLength(i)!= mdTest.colphysLength[i-1]){
           logerror("Invalid physical column length - column: %02d Length: >%d< expected >%d<", 
                        i, 
                        psmd->getPhysicalLength(i),
                        mdTest.colphysLength[i-1]
                    );
          }        

       //check parnullval;
       if ( psmd->isNullable(i)!= mdTest.colnullval[i-1]){
           logerror("Invalid column nullvalue - column: %02d nullvalue: >%d< expected >%d<", 
                        i, 
                        psmd->isNullable(i),
                        mdTest.colnullval[i-1]
                    );
        }        
        //check parprecison;
        if ( psmd->getPrecision(i)!= mdTest.colprecison[i-1]){
           logerror("Invalid column precision - column: %02d columnPrecision: >%d< expected >%d<", 
                        i, 
                        psmd->getPrecision(i),
                        mdTest.colprecison[i-1]
                    );
        }        
        //check parscale;
        if ( psmd->getScale(i)!= mdTest.colscale[i-1]){
           logerror("Invalid column scale - column: %02d columnScale: >%d< expected >%d<", 
                        i, 
                        psmd->getScale(i),
                        mdTest.colscale[i-1]
                    );
          }        
      }
    }
    
    void initTables(){
        this->dropTable("ResultSetMetaData");
        this->execSQL("CREATE TABLE ResultSetMetaData"
                 " (C_CHARASCII    Char (1) ASCII NOT NULL,"
                 "  C_CHARBYTE     Char (1) BYTE,"
//                 "  C_CHAREBCDIC   Char (1) EBCDIC,"
//                "  C_CHAREU       Char (1) UNICODE,"
                 "  C_VARCHARASCII  Varchar (1) ASCII,"
                 "  C_VARCHARBYTE   Varchar (1) BYTE,"
//                 "  C_VARCHAREBCDIC Varchar (1) EBCDIC,"
                 "  C_INT          Integer,"
                 "  C_SMALINT      Smallint,"
                 "  C_FLOAT        Float (5),"
                 "  C_FIXED        Fixed (5),"
                 "  C_FIXED1       Fixed (5,5),"
                 "  C_BOOLEAN      Boolean,"
                 "  C_DATE         Date,"
                 "  C_TIME         Time,"
                 "  C_TIMESTAMP    Timestamp,"
                 "  C_LONGASCII    Long ASCII,"
                 "  C_LONGBYTE     Long BYTE"
//                  "  C_LONGUNICODE  Long UNICODE,"
//                 "  C_LONGEB       Long EBCDIC"
                 ")");
        this->execSQL("INSERT INTO ResultSetMetaData (C_CHARASCII) VALUES('H')" );      
               
        this->execSQL("DROP DBPROCEDURE  ResultSetMetaDataProc2");
        this->execSQL("CREATE DBPROC ResultSetMetaDataProc2 RETURNS CURSOR AS "
                      " BEGIN"
                      " $CURSOR = 'MARCO';"
                      " DECLARE :$CURSOR CURSOR FOR SELECT 'Homer' As Simpson FROM SYSDBA.DUAL;"
                      " END;");
    }
    
    virtual SQLDBC_Bool run ()
    {

        const char *sqlcmd0        = "SELECT * FROM ResultSetMetaData";
        int         columncount0   = 15;
        const char *colname0[]     = { "C_CHARASCII","C_CHARBYTE","C_VARCHARASCII","C_VARCHARBYTE","C_INT","C_SMALINT","C_FLOAT","C_FIXED","C_FIXED1","C_BOOLEAN","C_DATE","C_TIME","C_TIMESTAMP","C_LONGASCII","C_LONGBYTE","C_LONGEB"}; 
        SQLDBC_SQLType coltype0[]      = {SQLDBC_SQLTYPE_CHA,SQLDBC_SQLTYPE_CHB,SQLDBC_SQLTYPE_VARCHARA,SQLDBC_SQLTYPE_VARCHARB,SQLDBC_SQLTYPE_INTEGER,SQLDBC_SQLTYPE_SMALLINT,SQLDBC_SQLTYPE_FLOAT,SQLDBC_SQLTYPE_FIXED ,SQLDBC_SQLTYPE_FIXED ,SQLDBC_SQLTYPE_BOOLEAN,SQLDBC_SQLTYPE_DATE,SQLDBC_SQLTYPE_TIME,SQLDBC_SQLTYPE_TIMESTAMP,SQLDBC_SQLTYPE_STRA,SQLDBC_SQLTYPE_STRB,SQLDBC_SQLTYPE_STRE};
        int         collength0[]    = {1,1,1,1,10,5,5,5,5,1,10,8,26,40,40};
        int         colphysLength0[]= {1,1,1,1,6,4,4,4,4,1,10,8,26,40,40};
        nullable    colnullval0[]   = {nonullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal};
        int         colprecison0[]  = {1,1,1,1,10,5,5,5,5,1,10,8,26,40,40};
        int         colscale0[]     = {0,0,0,0,0,0,0,0,5,0,0,0,0,0,0};


        const char *sqlcmd1         = "CALL ResultSetMetaDataPROC2 ";
        int         columncount1     = 1;
        const char *colname1[]      = {"SIMPSON"};
        SQLDBC_SQLType coltype1[]       = {SQLDBC_SQLTYPE_CHA};
        int         collength1[]     = {5};
        int         colphysLength1[] = {5};
        nullable    colnullval1[]    = {nonullVal};
        int         colprecison1[]   = {5};
        int         colscale1[]      = {0};

        if (connection()->isUnicodeDatabase()){
          coltype1[0]       = SQLDBC_SQLTYPE_UNICODE;
          colphysLength1[0] = 10;
        }        
        
        MetadataTestCase sqlcmds []= {
          MetadataTestCase(sqlcmd0,columncount0,colname0,coltype0,collength0,colphysLength0,colnullval0,colprecison0,colscale0),
          MetadataTestCase(sqlcmd1,columncount1,colname1,coltype1,collength1,colphysLength1,colnullval1,colprecison1,colscale1),
        };                                                         
                                                                   
        initTables();                                              
                                                                   
        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
                                                                   
        for (int i=0; i<sizeof(sqlcmds)/sizeof(MetadataTestCase); ++i) {
          tc_errorcheck(ps)->prepare(sqlcmds[i].sqlcmd, SQLDBC_StringEncodingAscii); 
          tc_errorcheck(ps)->execute(); 
          SQLDBC_ResultSet *rs = tc_errorcheck(ps)->getResultSet(); 
          SQLDBC_ResultSetMetaData *psmd = tc_errorcheck(rs)->getResultSetMetaData();   
          checkResultSetMetaData (psmd, sqlcmds[i]);                 
        }                                                          
        return m_success;                                          
   }      
        
};

class UnionSelect
    : public SQLDBC_TestCase
{
public:
    UnionSelect()
    :SQLDBC_TestCase("UnionSelect")
    {}

    SQLDBC_Bool run()
    {
        dropTable("UNION_TEST");
        execSQL("CREATE TABLE UNION_TEST (A INT, B INT)");
        execSQL("INSERT INTO UNION_TEST VALUES (1, 2)");
        execSQL("INSERT INTO UNION_TEST VALUES (3, 4)");
        SQLDBC_PreparedStatement *ps=connection()->createPreparedStatement();
        ps->prepare("SELECT A FROM UNION_TEST WHERE B = ? UNION SELECT B FROM UNION_TEST WHERE A = ?");
        SQLDBC_Int4 b_param = 2;
        SQLDBC_Int4 a_param = 4;
        SQLDBC_Length a_ind = sizeof(SQLDBC_Int4);
        SQLDBC_Length b_ind = sizeof(SQLDBC_Int4);
        tc_errorcheck(ps)->bindParameter(1, SQLDBC_HOSTTYPE_INT4, &b_param, &b_ind, sizeof(SQLDBC_Int4));
        tc_errorcheck(ps)->bindParameter(2, SQLDBC_HOSTTYPE_INT4, &a_param, &a_ind, sizeof(SQLDBC_Int4));

        SQLDBC_ResultSetMetaData *md = tc_errorcheck(ps)->getResultSetMetaData();
        if(md != 0) {
            SQLDBC_Int2 colcount = md->getColumnCount();
        }
        tc_errorcheck(ps)->execute();
        connection()->releaseStatement(ps);
        return m_success;
    }
};


class UpdatableResult
    : public SQLDBC_TestCase
{
public:
    UpdatableResult()
    :SQLDBC_TestCase("UpdatableResult")
    {}

    SQLDBC_Bool run()
    {
        dropTable("UPDATETABLE_TEST");
        execSQL("CREATE TABLE UPDATETABLE_TEST(A INTEGER)");
        SQLDBC_Statement *s = connection()->createStatement();
        tc_errorcheck(s)->setResultSetConcurrencyType(SQLDBC_Statement::CONCUR_UPDATABLE);
        tc_errorcheck(s)->execute("SELECT * FROM UPDATETABLE_TEST");
        char cmptablename[80];
        sprintf(cmptablename, "\"%s\".\"UPDATETABLE_TEST\"", username());
        char tablename[80];
        SQLDBC_Length bufferlength = 0;
        tc_errorcheck(s)->getTableName(tablename, SQLDBC_StringEncodingAscii, 80, &bufferlength);
        if(strcmp(tablename, cmptablename) != 0) {
            logerror("Found table name '%s' instead of %s", tablename, cmptablename);
        }
        return m_success;
    }
};


//----------------------------------------------------------------------
int main (int argc, char **argv)
{
    
    // Initalize from command line params
    SQLDBC_TestFrame testframe (argc, argv);
    testframe.addTestCase (new UnionSelect());
    testframe.addTestCase (new ResultSetMetaData());
    testframe.addTestCase (new UpdatableResult());
    if (testframe.run ()) 
        return 0;
    
    return 1;
}
