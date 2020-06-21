/*!================================================================

 module:        dataexecute.cpp

 -------------------------------------------------------------------

 responsible:   D039759

 special area:  SQL

 description:   Tests that DATA AT EXECUTE does work.
 
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
// Test data at execute for integral types (int, date, ...)
class IntegralDataTest : public IFR_TestCase
{
public:
    IntegralDataTest()
    :IFR_TestCase ("IntegralDataTest")
    {}
    
    virtual IFR_Bool run()
    {
        dropTable("DEX_INT4_TEST");
        m_success = m_success && execSQL("CREATE TABLE DEX_INT4_TEST (K INTEGER, V INTEGER)");
        IFR_PreparedStmt *p1=tc_errorcheck(connection())->createPreparedStatement();
        IFR_Length indicator=IFR_DATA_AT_EXEC;
        tc_errorcheck(p1)->bindParameter(1, IFR_HOSTTYPE_INT4, (void *)17, &indicator, sizeof(IFR_Int4));
        tc_errorcheck(p1)->prepare("INSERT INTO DEX_INT4_TEST (K,V) VALUES (1, ?)", IFR_StringEncodingAscii);
        IFR_Retcode rc = tc_errorcheck(p1)->execute();
        do {
            if(rc != IFR_NEED_DATA) {
                logerror("Execute did not return NEED DATA");
                break;
            } 

            IFR_Int2 pindex=127;
            void   * paddr=0;
            
            rc = tc_errorcheck(p1)->nextParameter(pindex, paddr);
            if(rc != IFR_NEED_DATA) {
                logerror("Expected NEED DATA from nextParameter");
                break;
            }
            if(pindex != 1) {
                logerror("Expected parameter index 1");
                break;
            }
            if(paddr != (void*)17) {
                logerror("Expected 'pointer' 17");
                break;
            }
            
            IFR_Int4 data=1267;
            IFR_Length dataind=sizeof(IFR_Int4);
            
            rc = tc_errorcheck(p1)->putData(&data, &dataind);
            if(rc != IFR_OK) {
                logerror("Could not put data.");
            }
            
            rc = tc_errorcheck(p1)->nextParameter(pindex, paddr);
            if(rc != IFR_OK) {
                logerror("Execution failed.");
            }
        } while(0);
        connection()->releaseStatement(p1);
        connection()->commit();
        return m_success;
    }
};

//----------------------------------------------------------------------
class CharDataTest : public IFR_TestCase
{
public:
    CharDataTest()
    :IFR_TestCase ("CharDataTest")
    {}
    
    virtual IFR_Bool run()
    {
        dropTable("DEX_CHAR_TEST");
        m_success = m_success && execSQL("CREATE TABLE DEX_CHAR_TEST (K INTEGER, V VARCHAR(255))");
        IFR_PreparedStmt *p1=tc_errorcheck(connection())->createPreparedStatement();
        IFR_Length indicator=IFR_DATA_AT_EXEC;
        tc_errorcheck(p1)->bindParameter(1, IFR_HOSTTYPE_ASCII, (void *)17, &indicator, sizeof(IFR_Int4));
        tc_errorcheck(p1)->prepare("INSERT INTO DEX_CHAR_TEST (K,V) VALUES (1, ?)", IFR_StringEncodingAscii);
        IFR_Retcode rc = tc_errorcheck(p1)->execute();
        do {
            if(rc != IFR_NEED_DATA) {
                logerror("Execute did not return NEED DATA");
                break;
            } 
            
            IFR_Int2 pindex=127;
            void   * paddr=0;
            
            rc = tc_errorcheck(p1)->nextParameter(pindex, paddr);
            if(rc != IFR_NEED_DATA) {
                logerror("Expected NEED DATA from nextParameter");
                break;
            }
            if(pindex != 1) {
                logerror("Expected parameter index 1");
                break;
            }
            if(paddr != (void*)17) {
                logerror("Expected 'pointer' 17");
                break;
            }

            char *data=(char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            IFR_Length dataind=1;
            
            for(int k=0; k<255; ++k) {
                rc = tc_errorcheck(p1)->putData(data+k%26, &dataind);
                if(rc != IFR_OK) {
                    logerror("Could not put data in step %d.", k);
                }
            }
            
            rc = tc_errorcheck(p1)->nextParameter(pindex, paddr);
            if(rc != IFR_OK) {
                logerror("Execution failed.");
            }
        } while(0);
        connection()->releaseStatement(p1);
        connection()->commit();
        return m_success;
    }
};

class LongDataTest : public IFR_TestCase
{
public:
    LongDataTest()
    :IFR_TestCase ("LongDataTest")
    {}
    
    char *makeCharBuffer(IFR_size_t size)
    {
        char *data=(char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        char *result=new char[size];
        for(IFR_size_t i=0; i<size; ++i) {
            result[i]=data[i%26];
        }
        return result;
    }
    
    virtual IFR_Bool run()
    {
        // loginfo("Making data ...");
        IFR_size_t size=1024*1024;
        char *data = makeCharBuffer(size);
        // loginfo("Finished making data.");
        dropTable("DEX_LONGDATA_TEST");
        m_success = m_success && execSQL("CREATE TABLE DEX_LONGDATA_TEST (K INTEGER, V LONG ASCII)");
        IFR_PreparedStmt *p1=tc_errorcheck(connection())->createPreparedStatement();
        IFR_Length indicator=IFR_DATA_AT_EXEC;
        tc_errorcheck(p1)->bindParameter(1, IFR_HOSTTYPE_ASCII, (void *)17, &indicator, size);
        tc_errorcheck(p1)->prepare("INSERT INTO DEX_LONGDATA_TEST (K,V) VALUES (1, ?)", IFR_StringEncodingAscii);
        IFR_Retcode rc = tc_errorcheck(p1)->execute();
        do {
            if(rc != IFR_NEED_DATA) {
                logerror("Execute did not return NEED DATA");
                break;
            } 
            
            IFR_Int2 pindex=127;
            void   * paddr=0;
            
            rc = tc_errorcheck(p1)->nextParameter(pindex, paddr);
            if(rc != IFR_NEED_DATA) {
                logerror("Expected NEED DATA from nextParameter");
                break;
            }
            if(pindex != 1) {
                logerror("Expected parameter index 1");
                break;
            }
            if(paddr != (void*)17) {
                logerror("Expected 'pointer' 17");
                break;
            }


            IFR_Length dataind=size;
            
            for(int j=0; j<20; ++j) {
                loginfo("STEP %d", j);
                rc = tc_errorcheck(p1)->putData(data, &dataind);
                if(rc != IFR_OK) {
                    logerror("Could not put data in step %d.", j);
                    j=10001;
                    break;
                }
                // loginfo("Inserted %d MBytes.", j);
            }
            
            rc = tc_errorcheck(p1)->nextParameter(pindex, paddr);
            if(rc != IFR_OK) {
                logerror("Execution failed.");
            }
        } while(0);
        connection()->releaseStatement(p1);
        connection()->commit();
        return m_success;
    }
}; 


//----------------------------------------------------------------------
class DataExecuteParseAgainTest
    : public IFR_TestCase
{
public:
    DataExecuteParseAgainTest()
    :IFR_TestCase("DataExecuteParseAgainTest")
    {}

    virtual IFR_Bool run()
    {
        dropTable("DEX_PARSEAGAIN");
        m_success = m_success && execSQL("CREATE TABLE DEX_PARSEAGAIN(K INTEGER, V1 CHAR(300) ASCII, V2 LONG BYTE)");
        IFR_PreparedStmt *p1=tc_errorcheck(connection())->createPreparedStatement();
        tc_errorcheck(p1)->prepare("INSERT INTO DEX_PARSEAGAIN VALUES (?, ?, ?)", IFR_StringEncodingAscii);
        
        // bind the parameters
        IFR_Int4   k_value     = 17;
        IFR_Length k_indicator = sizeof(IFR_Int4);
        
        IFR_Length v1_indicator = IFR_DATA_AT_EXEC;
        IFR_Length v2_indicator = IFR_DATA_AT_EXEC;
        
        tc_errorcheck(p1)->bindParameter(1, IFR_HOSTTYPE_INT4, &k_value, &k_indicator, sizeof(IFR_Int4));
        tc_errorcheck(p1)->bindParameter(2, IFR_HOSTTYPE_ASCII, 0, &v1_indicator, 300);
        tc_errorcheck(p1)->bindParameter(3, IFR_HOSTTYPE_BINARY, 0, &v2_indicator, 300); 
        IFR_Retcode rc = tc_errorcheck(p1)->execute();

        dropTable("DEX_PARSEAGAIN");
        m_success = m_success && execSQL("CREATE TABLE DEX_PARSEAGAIN(K INTEGER PRIMARY KEY, V1 CHAR(300) ASCII, V2 LONG BYTE)");
        
        do {
        	if(rc != IFR_NEED_DATA) {
            	logerror("Execute did not return NEED DATA");
				break;
        	} 
       		IFR_Int2 pindex=127;
        	void   * paddr=0;
            
        	rc = tc_errorcheck(p1)->nextParameter(pindex, paddr);
        	if(rc != IFR_NEED_DATA) {
            	logerror("Expected NEED DATA from nextParameter");
            	break;
        	}
        	
        	if(pindex != 2) {
            	logerror("Expected parameter index 2, found %d", pindex);
            	break;
        	}
        	
        	char *data=(char *)"1234567890";
        	IFR_Length indicator=IFR_NTS;
        
        	for(int i=0; i<30; ++i) {
            	rc = tc_errorcheck(p1)->putData(data, &indicator);
            	if(rc != IFR_OK) {
                	logerror("Operation putData failed in step %d for CHAR parameter.", i);
                	i=31;
            	}
        	}
        	if(m_success == false) 
        		break;
        
        	rc = tc_errorcheck(p1)->nextParameter(pindex, paddr);
        	if(rc != IFR_NEED_DATA) {
            	logerror("Expected NEED DATA from nextParameter");
            	break;
        	}
        	if(pindex != 3) {
            	logerror("Expected parameter index 3");
            	break;	
        	}
        
        	char *longdata=(char *)"1234567890";
        	IFR_Length longlength=10;
        	for(int j=0; j<5000; ++j) {
                rc = tc_errorcheck(p1)->putData(longdata, &longlength);
            	if(rc != IFR_OK) {
                	logerror("Operation putData failed in step %d for LONG parameter.", j);
                	j=5001;
            	}
        	}
			if(m_success == false) { 
				break; 
			}
			
        	rc = tc_errorcheck(p1)->nextParameter(pindex, paddr);
        	if(rc != IFR_OK) {
            	logerror("Expected IFR_OK from nextParameter");
            	break;
        	}
        } while(false);
        connection()->commit();
        return m_success;
    }
};


//----------------------------------------------------------------------
class SequenceTest : public IFR_TestCase
{
public:
    SequenceTest()
    :IFR_TestCase ("SequenceTest")
    {}

    virtual IFR_Bool run()
    {
//         m_success = m_success && runStopAfterExecute();
//         m_success = m_success &&  runStopAfterNextParameter();
//         m_success = m_success && runStopAfterPutData();
        m_success = m_success && runStopAfterCompletion();
        return m_success;
    }
    

    IFR_Bool stopDataExecute(IFR_PreparedStmt *ps)
    {
        tc_errorcheck(ps)->prepare("INSERT INTO DEX_SEQ_TEST (K,V) VALUES (1, 'ABC')", IFR_StringEncodingAscii);
        IFR_Retcode rc = tc_errorcheck(ps)->execute();
        if(rc != IFR_OK) {
            logerror("Failed to abort a statement.");
        }
        connection()->releaseStatement(ps);
        connection()->commit();
        return true;
        
    }

    IFR_Bool runStopAfterExecute()
    {
        dropTable("DEX_SEQ_TEST");
        m_success = m_success && execSQL("CREATE TABLE DEX_SEQ_TEST (K INTEGER, V VARCHAR(255))");
        IFR_PreparedStmt *p1=tc_errorcheck(connection())->createPreparedStatement();
        IFR_Length indicator=IFR_DATA_AT_EXEC;
        tc_errorcheck(p1)->bindParameter(1, IFR_HOSTTYPE_ASCII, (void *)17, &indicator, sizeof(IFR_Int4));
        tc_errorcheck(p1)->prepare("INSERT INTO DEX_SEQ_TEST (K,V) VALUES (1, ?)", IFR_StringEncodingAscii);
        IFR_Retcode rc = tc_errorcheck(p1)->execute();
        if(rc != IFR_NEED_DATA) {
            logerror("Execute did not return NEED DATA");
            connection()->releaseStatement(p1);
            return false;
        } 
        
        return stopDataExecute(p1);
    }    

    IFR_Bool runStopAfterNextParameter()
    {
        dropTable("DEX_SEQ_TEST");
        m_success = m_success && execSQL("CREATE TABLE DEX_SEQ_TEST (K INTEGER, V VARCHAR(255))");
        IFR_PreparedStmt *p1=tc_errorcheck(connection())->createPreparedStatement();
        IFR_Length indicator=IFR_DATA_AT_EXEC;
        tc_errorcheck(p1)->bindParameter(1, IFR_HOSTTYPE_ASCII, (void *)17, &indicator, sizeof(IFR_Int4));
        tc_errorcheck(p1)->prepare("INSERT INTO DEX_SEQ_TEST (K,V) VALUES (1, ?)", IFR_StringEncodingAscii);
        IFR_Retcode rc = tc_errorcheck(p1)->execute();
        do {
            if(rc != IFR_NEED_DATA) {
                logerror("Execute did not return NEED DATA");
                break;
            } 
            
            IFR_Int2 pindex=127;
            void   * paddr=0;
            
            rc = tc_errorcheck(p1)->nextParameter(pindex, paddr);
            if(rc != IFR_NEED_DATA) {
                logerror("Expected NEED DATA from nextParameter");
                break;
            }
            if(pindex != 1) {
                logerror("Expected parameter index 1");
                break;
            }
            if(paddr != (void*)17) {
                logerror("Expected 'pointer' 17");
                break;
            }
            
            return stopDataExecute(p1);

        } while(0);

        return true;
    }

    IFR_Bool runStopAfterPutData()
    {
        loginfo("stopaftercomplete");
        dropTable("DEX_SEQ_TEST");
        m_success = m_success && execSQL("CREATE TABLE DEX_SEQ_TEST (K INTEGER, V VARCHAR(255))");
        IFR_PreparedStmt *p1=tc_errorcheck(connection())->createPreparedStatement();
        IFR_Length indicator=IFR_DATA_AT_EXEC;
        tc_errorcheck(p1)->bindParameter(1, IFR_HOSTTYPE_ASCII, (void *)17, &indicator, sizeof(IFR_Int4));
        tc_errorcheck(p1)->prepare("INSERT INTO DEX_SEQ_TEST (K,V) VALUES (1, ?)", IFR_StringEncodingAscii);
        IFR_Retcode rc = tc_errorcheck(p1)->execute();
        do {
            if(rc != IFR_NEED_DATA) {
                logerror("Execute did not return NEED DATA");
                break;
            } 
            
            IFR_Int2 pindex=127;
            void   * paddr=0;
            
            rc = tc_errorcheck(p1)->nextParameter(pindex, paddr);
            if(rc != IFR_NEED_DATA) {
                logerror("Expected NEED DATA from nextParameter");
                break;
            }
            if(pindex != 1) {
                logerror("Expected parameter index 1");
                break;
            }
            if(paddr != (void*)17) {
                logerror("Expected 'pointer' 17");
                break;
            }
                        

            char *data=(char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            IFR_Length dataind=1;
            
            for(int k=0; k<255; ++k) {
                rc = tc_errorcheck(p1)->putData(data+k%26, &dataind);
                if(rc != IFR_OK) {
                    logerror("Could not put data in step %d.", k);
                }
            }
            
            return stopDataExecute(p1);

        } while(0);

        return false;
    }

    IFR_Bool runStopAfterCompletion()
    {
        dropTable("DEX_SEQ_TEST");
        m_success = m_success && execSQL("CREATE TABLE DEX_SEQ_TEST (K INTEGER, V VARCHAR(255))");
        IFR_PreparedStmt *p1=tc_errorcheck(connection())->createPreparedStatement();
        IFR_Length indicator=IFR_DATA_AT_EXEC;
        tc_errorcheck(p1)->bindParameter(1, IFR_HOSTTYPE_ASCII, (void *)17, &indicator, sizeof(IFR_Int4));
        tc_errorcheck(p1)->prepare("INSERT INTO DEX_SEQ_TEST (K,V) VALUES (1, ?)", IFR_StringEncodingAscii);
        IFR_Retcode rc = tc_errorcheck(p1)->execute();
        do {
            if(rc != IFR_NEED_DATA) {
                logerror("Execute did not return NEED DATA");
                break;
            } 
            
            IFR_Int2 pindex=127;
            void   * paddr=0;
            
            rc = tc_errorcheck(p1)->nextParameter(pindex, paddr);
            if(rc != IFR_NEED_DATA) {
                logerror("Expected NEED DATA from nextParameter");
                break;
            }
            if(pindex != 1) {
                logerror("Expected parameter index 1");
                break;
            }
            if(paddr != (void*)17) {
                logerror("Expected 'pointer' 17");
                break;
            }
            
            

            char *data=(char *)"ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            IFR_Length dataind=1;
            
            for(int k=0; k<255; ++k) {
                rc = tc_errorcheck(p1)->putData(data+k%26, &dataind);
                if(rc != IFR_OK) {
                    logerror("Could not put data in step %d.", k);
                }
            }
            
            rc = tc_errorcheck(p1)->nextParameter(pindex, paddr);
            if(rc != IFR_OK) {
                logerror("Execution failed.");
            }

            return stopDataExecute(p1);
        } while(0);
        return false;
    }
    
};

//----------------------------------------------------------------------
class BatchMixTest : public IFR_TestCase
{
public:
    BatchMixTest()
    :IFR_TestCase ("BatchMixTest")
    {}
    
    virtual IFR_Bool run()
    {
        dropTable("DEX_BATCHMIX_TEST");
        m_success = m_success && execSQL("CREATE TABLE BATCHMIX_TEST (K INTEGER, V1 VARCHAR(200), V2 LONG ASCII)");
        
        IFR_PreparedStmt *insertstmt=tc_errorcheck(connection())->createPreparedStatement();

        IFR_Int4 *k_values=new IFR_Int4[4];
        k_values[0]=111111;
        k_values[1]=222222;
        k_values[2]=333333;
        k_values[3]=444444;
        IFR_Length *k_indicator=new IFR_Length[4];
        k_indicator[0]=k_indicator[1]=k_indicator[2]=k_indicator[3]=sizeof(IFR_Int4);
        
        char *v1_values= new char[80];
        IFR_Length* v1_indicator=new IFR_Length[4];
        strcpy(v1_values, "VALUE1");
        strcpy(v1_values + 20, "VALUE2");
        strcpy(v1_values + 40, "VALUE3");
        strcpy(v1_values + 60, "VALUE4");
        v1_indicator[0]=IFR_NTS;
        v1_indicator[1]=IFR_NTS;
        v1_indicator[2]=IFR_DATA_AT_EXEC;
        v1_indicator[3]=IFR_NTS;
        
        char *v2_value=new char[4000000];
        for(int k=0; k<4; ++k) {
            for(int i=0; i<1000000; ++i) {
                v2_value[i+k*1000000]='1' + k;
            }
        }
        IFR_Length *v2_indicator=new IFR_Length[4];
        v2_indicator[0]=100000;
        v2_indicator[1]=10;
        v2_indicator[2]=100000;
        v2_indicator[3]=IFR_DATA_AT_EXEC;
        
        tc_errorcheck(insertstmt)->bindParameter(1, IFR_HOSTTYPE_INT4,  k_values, k_indicator, sizeof(IFR_Int4));
        tc_errorcheck(insertstmt)->bindParameter(2, IFR_HOSTTYPE_ASCII, v1_values, v1_indicator, 20);
        tc_errorcheck(insertstmt)->bindParameter(3, IFR_HOSTTYPE_ASCII, v2_value, v2_indicator, 1000000);
        tc_errorcheck(insertstmt)->prepare("INSERT INTO BATCHMIX_TEST VALUES (?, ?, ?)", IFR_StringEncodingAscii);
        tc_errorcheck(insertstmt)->setRowArraySize(4);
        IFR_Retcode rc=IFR_OK;
        do {
            rc=tc_errorcheck(insertstmt)->executeBatch();

            if(rc != IFR_NEED_DATA) {
                logerror("Expected NEED DATA after batch execute.");
                break;
            }

            IFR_Int2 pindex=127;
            void   * paddr=0;
            
            rc = tc_errorcheck(insertstmt)->nextParameter(pindex, paddr);
            
            if(rc != IFR_NEED_DATA) {
                logerror("Expected NEED DATA after 1st nextParameter.");
                break;
            }
            
            if(pindex != 2) {
                logerror("Expected pindex at 2 found, %d", (IFR_Int4)pindex);
                break;
            }

            if(paddr != v1_values + 40) {
                logerror("Expected 40 bytes offset found, %d", (IFR_Int4) ((char*)paddr - (char*)v1_values));
                break;
            }
            
            IFR_Length ind=IFR_NTS;
            
            rc = tc_errorcheck(insertstmt)->putData((void *)"homer", &ind);
            if(rc != IFR_OK) {
                logerror("Error after 1st putData.");
                break;
            }
            
            pindex = 127;
            // DebugBreak();
            rc = tc_errorcheck(insertstmt)->nextParameter(pindex, paddr);
            if(pindex != 3) {
                logerror("Expected pindex at 3 found, %d", (IFR_Int4)pindex);
                break;
            }
            if(paddr != v2_value + 3000000) {
                logerror("Expected 3000000 bytes offset found, %d", (IFR_Int4) ((char*)paddr - (char *)v2_value));
                break;
            }

            char *c=(char *)"AAAAAAAAAABBBBBBBBBBCCCCCCCCCC";
            for(int i=0; i< 2; ++i) {
                rc = tc_errorcheck(insertstmt)->putData(c, &ind);
                if(rc != IFR_OK) {
                    break;
                }
            }
            if(rc != IFR_OK) {
                break;
            }
            
            rc=tc_errorcheck(insertstmt)->nextParameter(pindex, paddr);

            if(rc != IFR_OK) {
                logerror("Expected OK from nextParameter.");
                break;
            }
            
            
        } while(false);
        
        return m_success;
    }

};

//----------------------------------------------------------------------
class IndicatorLengthTest : public IFR_TestCase
{
public:
    IndicatorLengthTest()
    :IFR_TestCase ("IndicatorLengthTest")
    {}
    
    virtual IFR_Bool run()
    {
        dropTable("INDICATORLENGTH_TEST");
        m_success = m_success && execSQL("CREATE TABLE INDICATORLENGTH_TEST (K INTEGER,  V LONG ASCII)");
        IFR_PreparedStmt *insertstmt=tc_errorcheck(connection())->createPreparedStatement();
        tc_errorcheck(insertstmt)->prepare("INSERT INTO INDICATORLENGTH_TEST VALUES (1, ?)", IFR_StringEncodingAscii);
        IFR_Length ind=IFR_DATA_AT_EXEC;
        insertstmt->bindParameter(1, IFR_HOSTTYPE_ASCII, 0, &ind, 1);

        IFR_Retcode rc = insertstmt->execute();
        void *p;
        IFR_Int2 next;
        IFR_ResultSet *rs;
        IFR_Length putdata_length = 10;
        IFR_Length buffer_ind;
        char buffer[100];
        if(rc != IFR_NEED_DATA) {
            logerror("IFR_NEED_DATA expected after execute.");
            goto fini;
        }
        insertstmt->nextParameter(next, p);

        insertstmt->putData((char *)"AAAAAAAAAA", &putdata_length);
        insertstmt->nextParameter(next, p);
        tc_errorcheck(insertstmt)->clearParameters();
        tc_errorcheck(insertstmt)->prepare("SELECT V FROM INDICATORLENGTH_TEST WHERE K = 1", IFR_StringEncodingAscii);
        tc_errorcheck(insertstmt)->execute();
        rs=insertstmt->getResultSet();
        rs->next();
        memset(buffer, 0, 100);
        rs->getRowSet()->getObject(1, buffer, 100, IFR_HOSTTYPE_ASCII, &buffer_ind, false);
        if(buffer_ind != 10 || strcmp(buffer, "AAAAAAAAAA")) {
            logerror("Found %s with length %d instead of AAAAAAAAAA.", buffer, buffer_ind);
        }
    fini:
        connection()->releaseStatement(insertstmt);
        return m_success;
    }
        
};



//----------------------------------------------------------------------
int main (int argc, char **argv)
{
    // Initalize from command line params
    IFR_TestFrame testframe (argc, argv);
    testframe.addTestCase (new IndicatorLengthTest());   
    testframe.addTestCase (new IntegralDataTest());
    testframe.addTestCase (new CharDataTest());
    testframe.addTestCase (new LongDataTest());
    testframe.addTestCase (new DataExecuteParseAgainTest());
    testframe.addTestCase (new SequenceTest());
    testframe.addTestCase (new BatchMixTest());
    
    if (testframe.run ()) 
        return 0;
    return 1;
}
