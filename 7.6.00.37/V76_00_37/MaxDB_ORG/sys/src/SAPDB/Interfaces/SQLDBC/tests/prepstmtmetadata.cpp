/*!================================================================

 module:        prepstmtmetadata.cpp

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
enum Mode { 
    In = 1,
    InOut = 2,
    Out = 4
};

enum nullable {
    nullVal   = 1,
    nonullVal = 0
};

class MetadataTestCase {
public:
  
  const char *sqlcmd;
  int         paramcount;
  const char        **paramname;
  SQLDBC_SQLType *partype;
  int         *parlength;
  int         *parphysLength;
  Mode        *parmode;
  nullable    *parnullval;
  int         *parprecison;
  int         *parscale;
  int         columncount;
  const char **colname;
  SQLDBC_SQLType *coltype;
  int         *collength;
  int         *colphysLength;
  nullable    *colnullval;
  int         *colprecison;
  int         *colscale;
  int          nocolumns;

  MetadataTestCase (  const char        *asqlcmd,
                      int         aparamcount,
                      const char        **aparamname,
                      SQLDBC_SQLType *apartype,
                      int         *aparlength,
                      int         *aparphysLength,
                      Mode        *aparmode,
                      nullable    *aparnullval,
                      int         *aparprecison,
                      int         *aparscale,
                      int         acolumncount,
                      const char        **acolname,
                      SQLDBC_SQLType *acoltype,
                      int         *acollength,
                      int         *acolphysLength,
                      nullable    *acolnullval,
                      int         *acolprecison,
                      int         *acolscale,
                      int         anocolumns = 0
                   ):
 sqlcmd (asqlcmd), 
 paramcount(aparamcount),
 paramname(aparamname),
 partype(apartype),
 parlength(aparlength),
 parphysLength(aparphysLength),
 parmode(aparmode),
 parnullval(aparnullval),
 parprecison(aparprecison),
 parscale(aparscale),
 columncount(acolumncount),                          
 colname(acolname),
 coltype(acoltype),
 collength(acollength),
 colphysLength(acolphysLength),
 colnullval(acolnullval),
 colprecison(acolprecison),
 colscale(acolscale),
 nocolumns(anocolumns)
  {}
};

class PrepStmtMetaData : public SQLDBC_TestCase
{
public:
    PrepStmtMetaData()
    :SQLDBC_TestCase ("PrepStmtMetaData")
    {}
    
    void checkMetaData (SQLDBC_PreparedStatement* ps, MetadataTestCase& mdTest){
      SQLDBC_ParameterMetaData* psmd = tc_errorcheck(ps)->getParameterMetaData();
    
    loginfo("Check Parameter: %s", mdTest.sqlcmd);       
    if (psmd->getParameterCount()!= mdTest.paramcount)
      logerror("Invalid Number of parameters: %d expected %d", psmd->getParameterCount(), mdTest.paramcount);        
    if (psmd->getParameterCount())
      for (int i = 1; i <= psmd->getParameterCount(); ++i) {
        /*check pramDataType*/ 
         if ( psmd->getParameterType(i)!= mdTest.partype[i-1]){
           logerror("Invalid parameter type - Parameter: %02d Parametertype: >%d< expected >%d<", 
                        i, 
                        psmd->getParameterType(i),
                        mdTest.partype[i-1]
                    );
          }
         /*check Mode*/ 
         if ( psmd->getParameterMode(i)!= mdTest.parmode[i-1]){
           logerror("Invalid parameter mode - Parameter: %02d ParameterMode: >%d< expected >%d<", 
                        i, 
                        psmd->getParameterMode(i),
                        mdTest.parmode[i-1]
                    );
          }        

         char       parametername_buffer[512];
         SQLDBC_Length buflen;

         psmd->getParameterName(i, parametername_buffer, SQLDBC_StringEncodingAscii, 512, &buflen);
         
         /*check paramnames*/ 
         if ( ! (*(mdTest.paramname[i-1]))){
             
           if( buflen != 0){
             logerror("Invalid parameter name - Parameter: %02d ParameterName: >%.*s<expected >%s<", 
                      i, 
                      buflen,
                      parametername_buffer,
                      mdTest.paramname[i-1]);          
           }           
         } else {
           if ( strncmp(parametername_buffer, mdTest.paramname[i-1], strlen(mdTest.paramname[i-1]))){
             logerror("Invalid parameter name - Parameter: %02d ParameterName: >%.*s< expected >%s<", 
                        i, 
                        buflen,
                        parametername_buffer,
                        mdTest.paramname[i-1]);
          }
        }
        //check parlength;
        if ( psmd->getParameterLength(i)!= mdTest.parlength[i-1]){
           logerror("Invalid parameter length - Parameter: %02d ParameterLength: >%d< expected >%d<", 
                        i, 
                        psmd->getParameterLength(i),
                        mdTest.parlength[i-1]
                    );
          }        
       //check *parphysLength;
       if ( psmd->getPhysicalLength(i)!= mdTest.parphysLength[i-1]){
           logerror("Invalid physical parameter length - Parameter: %02d Length: >%d< expected >%d<", 
                        i, 
                        psmd->getPhysicalLength(i),
                        mdTest.parphysLength[i-1]
                    );
          }        

       //check parnullval;
       if ( psmd->isNullable(i)!= mdTest.parnullval[i-1]){
           logerror("Invalid parameter nullvalue - Parameter: %02d nullvalue: >%d< expected >%d<", 
                        i, 
                        psmd->isNullable(i),
                        mdTest.parnullval[i-1]
                    );
        }        
        //check parprecison;
        if ( psmd->getPrecision(i)!= mdTest.parprecison[i-1]){
           logerror("Invalid parameter precision - Parameter: %02d ParameterPrecision: >%d< expected >%d<", 
                        i, 
                        psmd->getPrecision(i),
                        mdTest.parprecison[i-1]
                    );
        }        
        //check parscale;
        if ( psmd->getScale(i)!= mdTest.parscale[i-1]){
           logerror("Invalid parameter scale - Parameter: %02d ParameterScale: >%d< expected >%d<", 
                        i, 
                        psmd->getScale(i),
                        mdTest.parscale[i-1]
                    );
          }        
      }
    }

    void checkResultSetMetaData (SQLDBC_PreparedStatement* ps, MetadataTestCase& mdTest){
      SQLDBC_ResultSetMetaData* psmd = tc_errorcheck(ps)->getResultSetMetaData();
      
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
         /*check paramnames*/
         char       columname_buffer[512];
         SQLDBC_Length bufferlen;

         psmd->getColumnName(i, columname_buffer, SQLDBC_StringEncodingAscii, 512, &bufferlen);

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
        this->dropTable("PrepStmtMetaData");
        this->execSQL("CREATE TABLE PrepStmtMetaData"
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
//                 "  C_LONGEB       Long EBCDIC NOT NULL"
                 ")");
        
        this->execSQL("DROP DBPROCEDURE  PrepStmtMetaDataProc");
        this->execSQL("CREATE DBPROCEDURE  PrepStmtMetaDataProc"
                  "(IN C_CHARASCII    Char (1) ,"
                  " OUT C_CHARBYTE     Char (1) BYTE,"
                  " OUT C_VARCHARASCII Varchar (1) ,"
                  " IN C_VARCHARBYTE  Varchar (1) BYTE,"
                  " INOUT C_INT          Integer,"
                  " OUT C_SMALINT      Smallint,"
                  " IN C_FLOAT        Float (5),"
                  " OUT C_FIXED        Fixed (5),"
                  " IN C_FIXED1       Fixed (5,5),"
                  " OUT C_BOOLEAN      Boolean,"
                  " IN C_DATE         Date,"
                  " OUT C_TIME         Time,"
                  " INOUT C_TIMESTAMP    Timestamp) AS BEGIN"
                  " SELECT * from sysdba.dual;"
                  " END; ");
               
               
        this->execSQL("DROP DBPROCEDURE  PrepStmtMetaDataProc2");
        this->execSQL("CREATE DBPROC PrepStmtMetaDataProc2 (IN numval INTEGER) RETURNS CURSOR AS "
                      " BEGIN"
                      " $CURSOR = 'MARCO';"
                      " DECLARE :$CURSOR CURSOR FOR SELECT :numval FROM SYSDBA.DUAL;"
                      " END;");
    }
    
    virtual SQLDBC_Bool run ()
    {
        
        const char        *sqlcmd0        = "PING";
        int         paramcount0     = 0;
        const char        *paramname0[]   = {""}; 
        SQLDBC_SQLType *partype0       = 0;
        int         *parlength0     = 0;
        int         *parphysLength0 = 0;
        Mode        *parmode0       = 0;
        nullable    *parnullval0    = 0;
        int         *parprecison0   = 0;
        int         *parscale0      = 0;
        int         columncount0    = 0;
        const char        *colname0[]     = {""};
        SQLDBC_SQLType *coltype0       = 0;
        int         *collength0     = 0;
        int         *colphysLength0 = 0;
        nullable    *colnullval0    = 0;
        int         *colprecison0   = 0;
        int         *colscale0      = 0;
        
        const char        *sqlcmd1         = "INSERT into PrepStmtMetaData values (?,?,:A,?,:B,:C,?,?,?,?,?,?,?,?,?)";
        int         paramcount1     = 15;
        const char        *paramname1[]   = {"","","","","","","","","","","","","","",""}; 
        SQLDBC_SQLType partype1[]      = {SQLDBC_SQLTYPE_CHA,
                                          SQLDBC_SQLTYPE_CHB,
                                          SQLDBC_SQLTYPE_VARCHARA,
                                          SQLDBC_SQLTYPE_VARCHARB,
                                          SQLDBC_SQLTYPE_INTEGER,
                                          SQLDBC_SQLTYPE_SMALLINT,
                                          SQLDBC_SQLTYPE_FLOAT,
                                          SQLDBC_SQLTYPE_FIXED,
                                          SQLDBC_SQLTYPE_FIXED,
                                          SQLDBC_SQLTYPE_BOOLEAN,
                                          SQLDBC_SQLTYPE_DATE,
                                          SQLDBC_SQLTYPE_TIME,
                                          SQLDBC_SQLTYPE_TIMESTAMP,
                                          SQLDBC_SQLTYPE_STRA,
                                          SQLDBC_SQLTYPE_STRB};                                
        int         parlength1[]    = {1,1,1,1,10,5,5,5,5,1,10,8,26,40,40};
        int         parphysLength1[]= {1,1,1,1,6,4,4,4,4,1,10,8,26,40,40};
        Mode        parmode1[]      = {In,In,In,In,In,In,In,In,In,In,In,In,In,In,In}; 
        nullable    parnullval1[]   = {nonullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal};
        int         parprecison1[]  = {1,1,1,1,10,5,5,5,5,1,10,8,26,40,40};
        int         parscale1[]     = {0,0,0,0,0,0,0,0,5,0,0,0,0,0,0};
        int         columncount1    = 0;
        const char *colname1[]     = {""};
        SQLDBC_SQLType *coltype1       = 0;
        int         *collength1     = 0;
        int         *colphysLength1 = 0;
        nullable    *colnullval1    = 0;
        int         *colprecison1   = 0;
        int         *colscale1      = 0;

        const char *sqlcmd2        = "SELECT * FROM PrepStmtMetaData WHERE C_CHARASCII = ? AND C_DATE = :A";
        int         paramcount2     = 2;
        const char        *paramname2[]   = {"",""}; 
        SQLDBC_SQLType partype2[]      = {SQLDBC_SQLTYPE_CHA,SQLDBC_SQLTYPE_DATE};                                
        int         parlength2[]    = {1,10};
        int         parphysLength2[]= {1,10};
        Mode        parmode2[]      = {In,In}; 
        nullable    parnullval2[]   = {nullVal,nullVal};
        int         parprecison2[]  = {1,10};
        int         parscale2[]     = {0,0};
        int         columncount2    = 15;
        const char  *colname2[]     = { "C_CHARASCII","C_CHARBYTE","C_VARCHARASCII","C_VARCHARBYTE","C_INT","C_SMALINT","C_FLOAT","C_FIXED","C_FIXED1","C_BOOLEAN","C_DATE","C_TIME","C_TIMESTAMP","C_LONGASCII","C_LONGBYTE"}; 
        SQLDBC_SQLType coltype2[]      = {SQLDBC_SQLTYPE_CHA,SQLDBC_SQLTYPE_CHB,SQLDBC_SQLTYPE_VARCHARA,SQLDBC_SQLTYPE_VARCHARB,SQLDBC_SQLTYPE_INTEGER,SQLDBC_SQLTYPE_SMALLINT,SQLDBC_SQLTYPE_FLOAT,SQLDBC_SQLTYPE_FIXED ,SQLDBC_SQLTYPE_FIXED ,SQLDBC_SQLTYPE_BOOLEAN,SQLDBC_SQLTYPE_DATE,SQLDBC_SQLTYPE_TIME,SQLDBC_SQLTYPE_TIMESTAMP,SQLDBC_SQLTYPE_STRA,SQLDBC_SQLTYPE_STRB};
        int         collength2[]    = {1,1,1,1,10,5,5,5,5,1,10,8,26,40,40};
        int         colphysLength2[]= {1,1,1,1,6,4,4,4,4,1,10,8,26,40,40};
        nullable    colnullval2[]   = {nonullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal};
        int         colprecison2[]  = {1,1,1,1,10,5,5,5,5,1,10,8,26,40,40};
        int         colscale2[]     = {0,0,0,0,0,0,0,0,5,0,0,0,0,0,0};

        const char *sqlcmd3        = "SELECT C_CHARASCII||:A INTO :B FROM PrepStmtMetaData WHERE C_TIME = :C";
        int         paramcount3     = 3;
        const char *paramname3[]   = {"","EXPRESSION1",""}; 
        SQLDBC_SQLType partype3[]      = {SQLDBC_SQLTYPE_CHA,SQLDBC_SQLTYPE_CHA,SQLDBC_SQLTYPE_TIME};                                
        int         parlength3[]    = {254,255,8};
        int         parphysLength3[]= {254,255,8};
        Mode        parmode3[]      = {In,Out,In}; 
        nullable    parnullval3[]   = {nullVal,nullVal,nullVal};
        int         parprecison3[]  = {254,255,8};
        int         parscale3[]     = {0,0,0};
        int         columncount3    = 0;
        const char *colname3[]     = {""}; 
        SQLDBC_SQLType *coltype3       = 0;
        int         *collength3     = 0;
        int         *colphysLength3 = 0;
        nullable    *colnullval3    = 0;
        int         *colprecison3   = 0;
        int         *colscale3      = 0;

        const char *sqlcmd4        = "CALL PrepStmtMetaDataPROC (?,?,?,?,?,?,?,?,?,?,?,?,?)";
        int         paramcount4     = 13;
        const char *paramname4[]   = {"C_CHARASCII","C_CHARBYTE","C_VARCHARASCII","C_VARCHARBYTE","C_INT","C_SMALINT","C_FLOAT","C_FIXED","C_FIXED1","C_BOOLEAN","C_DATE","C_TIME","C_TIMESTAMP" };
        SQLDBC_SQLType partype4[]      = {SQLDBC_SQLTYPE_CHA,SQLDBC_SQLTYPE_CHB,SQLDBC_SQLTYPE_CHA,SQLDBC_SQLTYPE_CHB,SQLDBC_SQLTYPE_INTEGER,SQLDBC_SQLTYPE_SMALLINT,SQLDBC_SQLTYPE_FLOAT,SQLDBC_SQLTYPE_FIXED ,SQLDBC_SQLTYPE_FIXED ,SQLDBC_SQLTYPE_BOOLEAN,SQLDBC_SQLTYPE_DATE,SQLDBC_SQLTYPE_TIME,SQLDBC_SQLTYPE_TIMESTAMP};
        int         parlength4[]    = {1,1,1,1,10,5,5,5,5,1,10,8,26};
        int         parphysLength4[]= {1,1,1,1,6,4,4,4,4,1,10,8,26};
        Mode        parmode4[]      = {In,Out,Out,In,InOut,Out,In,Out,In,Out,In,Out,InOut};
        nullable    parnullval4[]   = {nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal,nullVal};
        int         parprecison4[]  = {1,1,1,1,10,5,5,5,5,1,10,8,26};
        int         parscale4[]     = {0,0,0,0, 0,0,0,0,5,0, 0,0, 0,};
        int         columncount4    = 0;
        const char *colname4[]     = {""};
        SQLDBC_SQLType *coltype4       = 0;
        int         *collength4     = 0;
        int         *colphysLength4 = 0;
        nullable    *colnullval4    = 0;
        int         *colprecison4   = 0;
        int         *colscale4      = 0;
        
        if (connection()->isUnicodeDatabase()){
          partype4[0]   = SQLDBC_SQLTYPE_UNICODE;
          parphysLength4[0] = 2;
          partype4[2]   = SQLDBC_SQLTYPE_UNICODE;
          parphysLength4[2] = 2;
        }        

        const char *sqlcmd5        = "CALL PrepStmtMetaDataPROC2 (?)";
        int         paramcount5     = 1;
        const char *paramname5[]   = {"NUMVAL" };
        SQLDBC_SQLType partype5[]      = {SQLDBC_SQLTYPE_INTEGER};
        int         parlength5[]    = {10};
        int         parphysLength5[]= {6};
        Mode        parmode5[]      = {In};
        nullable    parnullval5[]   = {nullVal};
        int         parprecison5[]  = {10};
        int         parscale5[]     = {0};
        int         columncount5    = -1;
        const char  *colname5[]     = {""};
        SQLDBC_SQLType *coltype5       = 0;
        int         *collength5     = 0;
        int         *colphysLength5 = 0;
        nullable    *colnullval5    = 0;
        int         *colprecison5   = 0;
        int         *colscale5      = 0;
        
        MetadataTestCase sqlcmds []= {
            MetadataTestCase(sqlcmd0,paramcount0,paramname0,partype0,parlength0,parphysLength0,parmode0,parnullval0,parprecison0,parscale0,columncount0,colname0,coltype0,collength0,colphysLength0,colnullval0,colprecison0,colscale0, 1),
            MetadataTestCase(sqlcmd1,paramcount1,paramname1,partype1,parlength1,parphysLength1,parmode1,parnullval1,parprecison1,parscale1,columncount1,colname1,coltype1,collength1,colphysLength1,colnullval1,colprecison1,colscale1, 1),
          MetadataTestCase(sqlcmd2,paramcount2,paramname2,partype2,parlength2,parphysLength2,parmode2,parnullval2,parprecison2,parscale2,columncount2,colname2,coltype2,collength2,colphysLength2,colnullval2,colprecison2,colscale2),
          MetadataTestCase(sqlcmd3,paramcount3,paramname3,partype3,parlength3,parphysLength3,parmode3,parnullval3,parprecison3,parscale3,columncount3,colname3,coltype3,collength3,colphysLength3,colnullval3,colprecison3,colscale3),
          MetadataTestCase(sqlcmd4,paramcount4,paramname4,partype4,parlength4,parphysLength4,parmode4,parnullval4,parprecison4,parscale4,columncount4,colname4,coltype4,collength4,colphysLength4,colnullval4,colprecison4,colscale4, 1),
          MetadataTestCase(sqlcmd5,paramcount5,paramname5,partype5,parlength5,parphysLength5,parmode5,parnullval5,parprecison5,parscale5,columncount5,colname5,coltype5,collength5,colphysLength5,colnullval5,colprecison5,colscale5, 1)
        };                                                         
                                                                   
        initTables();                                              
                                                                   
        SQLDBC_PreparedStatement *ps=tc_errorcheck(connection())->createPreparedStatement();
                                                                   
        for (int i=0; i<sizeof(sqlcmds)/sizeof(MetadataTestCase); ++i) {
            tc_errorcheck(ps)->prepare(sqlcmds[i].sqlcmd, SQLDBC_StringEncodingAscii);           
          checkMetaData (ps, sqlcmds[i]);                          
          if(!sqlcmds[i].nocolumns) {
              checkResultSetMetaData (ps, sqlcmds[i]);                 
          }
        }                                                          
        return m_success;                                          
   }      
        
};

//----------------------------------------------------------------------
int main (int argc, char **argv)
{
    
    // Initalize from command line params
    SQLDBC_TestFrame testframe (argc, argv);
    testframe.addTestCase (new PrepStmtMetaData());
    if (testframe.run ()) 
        return 0;
    
    return 1;
}
