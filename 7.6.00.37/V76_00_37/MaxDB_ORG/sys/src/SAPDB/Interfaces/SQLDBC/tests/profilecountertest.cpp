/*!
  @file           profilecountertest.cpp
  @author         MarcoP
  @ingroup        Tests
  @brief          tests the profile counters of SQLDBC
  @keywords       double linked list




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

#define maxVal 8


class testcase1 : public SQLDBC_TestCase
{
private:
  SQLDBC_IRuntime* m_rt;
  SQLDBC_Connection *myCon[maxVal];
  SQLDBC_Statement *myStmt[maxVal];
  SQLDBC_PreparedStatement *myPStmt[maxVal];
  SQLDBC_Environment *myEnv[maxVal];


public:
	testcase1 ()
		: SQLDBC_TestCase ("testcase1"),
		m_rt(0)
	{
		char errorText[100];
        m_rt = SQLDBC::GetClientRuntime(errorText, sizeof(errorText) );
    }

	~testcase1 ()
	{
    }

	virtual SQLDBC_Bool prepare ()
	{

		return m_success;
	}

	virtual SQLDBC_Bool release ()
	{
		return m_success;
	}

    void initCounter(SQLDBC_Int8* counter, SQLDBC_Bool ignorepacket){
        memset  (counter, 0, sizeof(SQLDBC_Int8)* SQLDBC_PROFILE_MAX);
        counter[SQLDBC_SENDCOUNT] = -1;
        counter[SQLDBC_SENDSIZE]  = -1;
        counter[SQLDBC_RECEIVESIZE]  = -1;
        counter[SQLDBC_SQLERRORCOUNT] = -1;
        counter[SQLDBC_ROWNOTFOUNDCOUNT] = -1;
        counter[SQLDBC_DATAWRITESIZE] = -1;
        counter[SQLDBC_LONGDATAWRITESIZE] = -1;
        counter[SQLDBC_DATAREADSIZE] = -1;
        counter[SQLDBC_LONGDATAREADSIZE] = -1;
        if(ignorepacket) {
            counter[SQLDBC_FETCHCOUNT] = -1;
        }
    }		
    
    
    SQLDBC_Bool parseAgainTest(SQLDBC_Int8* counters){
        SQLDBC_Retcode rc;
		SQLDBC_Statement *myStmtHdl = myCon[0]->createStatement();
		if(!myStmtHdl) {
		  logerror("createStatement() failed");
		  return SQLDBC_FALSE;
		}
		counters[SQLDBC_STATEMENTCOUNT]++;
		rc = myStmtHdl->execute("DROP TABLE PARSEAGAINTAB", SQLDBC_StringEncodingAscii);
        if (rc != SQLDBC_OK){
//          counters[SQLDBC_SQLERRORCOUNT]++;
        }
        counters[SQLDBC_EXECUTECOUNT]++;
		tc_errorcheck(myStmtHdl)->execute("CREATE TABLE PARSEAGAINTAB (a varchar(42))", SQLDBC_StringEncodingAscii);
        counters[SQLDBC_EXECUTECOUNT]++;

		SQLDBC_PreparedStatement * myPStmtHdl = myCon[0]->createPreparedStatement();
		if(!myPStmtHdl) {
		  logerror("createPreparedStatement() failed");
		  return SQLDBC_FALSE;
		}
		counters[SQLDBC_PREPAREDSTATEMENTCOUNT]++;
		tc_errorcheck(myPStmtHdl)->prepare("SELECT * FROM PARSEAGAINTAB", SQLDBC_StringEncodingAscii);
        counters[SQLDBC_PREPARECOUNT]++;
        counters[SQLDBC_PARSINFOMISSCOUNT]++;
        tc_errorcheck(myPStmtHdl)->execute();
        counters[SQLDBC_EXECUTECOUNT]++;
        counters[SQLDBC_SELECTCOUNT]++;

        SQLDBC_ResultSet * rs = myPStmtHdl->getResultSet();
        int rc_count= 0;
        while (SQLDBC_OK == rs->next()){rc_count++;}
//        counters[SQLDBC_FETCHCOUNT]+=6;
        counters[SQLDBC_FETCHROWCOUNT]+=rc_count;
            
        
		tc_errorcheck(myStmtHdl)->execute("DROP TABLE PARSEAGAINTAB", SQLDBC_StringEncodingAscii);
        counters[SQLDBC_EXECUTECOUNT]++;
		tc_errorcheck(myStmtHdl)->execute("CREATE TABLE PARSEAGAINTAB (HOMER varchar(42))", SQLDBC_StringEncodingAscii);
        counters[SQLDBC_EXECUTECOUNT]++;

        tc_errorcheck(myPStmtHdl)->execute();
        counters[SQLDBC_PARSEAGAINCOUNT]++;
        counters[SQLDBC_EXECUTECOUNT]++;
        counters[SQLDBC_SELECTCOUNT]++;
//        counters[SQLDBC_SQLERRORCOUNT]++;
//        counters[SQLDBC_SQLERRORCOUNT]++;  //eigentlich ein Bug, dass der Error zweimal gezählt wird


		tc_errorcheck(myCon[0])->releaseStatement(myPStmtHdl);
		tc_errorcheck(myCon[0])->releaseStatement(myStmtHdl);
		return m_success;
    
    }

    SQLDBC_Bool longTest(SQLDBC_Int8* counters){
        SQLDBC_Retcode rc;
		SQLDBC_Statement *myStmtHdl = myCon[0]->createStatement();
		if(!myStmtHdl) {
		  logerror("createStatement() failed");
		  return SQLDBC_FALSE;
		}
		counters[SQLDBC_STATEMENTCOUNT]++;
		rc = myStmtHdl->execute("DROP TABLE PARSEAGAINTAB", SQLDBC_StringEncodingAscii);
        if (rc != SQLDBC_OK){
//          counters[SQLDBC_SQLERRORCOUNT]++;
        }
        counters[SQLDBC_EXECUTECOUNT]++;
		tc_errorcheck(myStmtHdl)->execute("CREATE TABLE PARSEAGAINTAB (a long ascii)", SQLDBC_StringEncodingAscii);
        counters[SQLDBC_EXECUTECOUNT]++;

		SQLDBC_PreparedStatement * myPStmt = myCon[0]->createPreparedStatement();
		if(!myPStmt) {
		  logerror("createPreparedStatement() failed");
		  return SQLDBC_FALSE;
		}

		counters[SQLDBC_PREPAREDSTATEMENTCOUNT]++;
		tc_errorcheck(myPStmt)->prepare("INSERT INTO PARSEAGAINTAB  VALUES (?)");
        counters[SQLDBC_PREPARECOUNT]++;
       	counters[SQLDBC_PARSINFOMISSCOUNT]++;

        char *pdata = new char[2000000];
        memset(pdata,'a',2000000);
        SQLDBC_Length ind[10];
        for (int k=0; k<10 ; k++){
          ind[k]=200000;
//          counters[SQLDBC_DATAWRITESIZE]+=130457;
//          counters[SQLDBC_LONGDATAWRITESIZE]+=69666; 
        }

        tc_errorcheck(myPStmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, &pdata[0], &ind[0], 200000, SQLDBC_FALSE);
//        counters[SQLDBC_DATAWRITESIZE]+=130481;
		tc_errorcheck(myPStmt)->execute();
//        counters[SQLDBC_LONGDATAWRITESIZE]+=69642; 
        counters[SQLDBC_EXECUTECOUNT]++;
        counters[SQLDBC_INSERTCOUNT]++;

        tc_errorcheck(myPStmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, &pdata[0], &ind[0], 200000, SQLDBC_FALSE);
        tc_errorcheck(myPStmt)->setBatchSize(10);

		tc_errorcheck(myPStmt)->execute();
        counters[SQLDBC_EXECUTECOUNT]++;
        counters[SQLDBC_INSERTCOUNT]++;
        
        tc_errorcheck(myPStmt)->setBatchSize(1);
        tc_errorcheck(myPStmt)->clearParameters();

        // getObject
		tc_errorcheck(myPStmt)->prepare("SELECT * FROM  PARSEAGAINTAB", SQLDBC_StringEncodingAscii);
        counters[SQLDBC_PREPARECOUNT]++;
      	counters[SQLDBC_PARSINFOMISSCOUNT]++;
		tc_errorcheck(myPStmt)->execute();
        counters[SQLDBC_EXECUTECOUNT]++;
        counters[SQLDBC_SELECTCOUNT]++;
//        counters[SQLDBC_DATAREADSIZE]+=130416;
    
        SQLDBC_ResultSet * rs = myPStmt->getResultSet();
        int rc_count= 0;
        while (SQLDBC_OK == rs->next()){
        	rc_count++;
            memset(pdata,'x',2000000);
            tc_errorcheck(rs)->getObject (
                1, 
                SQLDBC_HOSTTYPE_ASCII, 
                pdata, 
                &ind[0], 
                200000, 
                SQLDBC_FALSE);
//             counters[SQLDBC_LONGDATAREADSIZE]+=(69562+130520);
             
      	}
//        counters[SQLDBC_LONGDATAREADSIZE]-=(69562+130520-70076); //substract bytes received at execute
        counters[SQLDBC_FETCHROWCOUNT]+=rc_count;

//        fetch
		tc_errorcheck(myPStmt)->execute();
        counters[SQLDBC_EXECUTECOUNT]++;
        counters[SQLDBC_SELECTCOUNT]++;
//        counters[SQLDBC_DATAREADSIZE]+=130416;
    
        rs = myPStmt->getResultSet();
        rc_count= 0;
        while (SQLDBC_OK == rs->next()){
        	rc_count++;
            memset(pdata,'x',2000000);
            tc_errorcheck(rs)->bindColumn (
                1, 
                SQLDBC_HOSTTYPE_ASCII, 
                pdata, 
                &ind[0], 
                200000, 
                SQLDBC_FALSE);
//             counters[SQLDBC_LONGDATAREADSIZE]+=(69562+130520);
            SQLDBC_RowSet* rset = rs->getRowSet();
            tc_errorcheck(rset)->fetch();
      	}
        counters[SQLDBC_FETCHROWCOUNT]+=rc_count;
//        counters[SQLDBC_LONGDATAREADSIZE]-=(69562+130520-70076); //substract bytes received at execute

        delete pdata; 
        return m_success;
    }

	virtual SQLDBC_Bool run ()
	{
      SQLDBC_Bool ignore_packets=false;
      {
          SQLDBC_PreparedStatement *ps_packetsize = connection()->createPreparedStatement();
          tc_errorcheck(ps_packetsize)->prepare("SELECT VALUE FROM ACTIVECONFIGURATION WHERE PARAMETERNAME = '_PACKET_SIZE'");
          tc_errorcheck(ps_packetsize)->execute();
          if(m_success) {
              SQLDBC_ResultSet *packetsize_rs = ps_packetsize->getResultSet();
              
              SQLDBC_Length result_ind = 0;
              SQLDBC_Int4  result = 0;
              tc_errorcheck(packetsize_rs)->next();
              tc_errorcheck(packetsize_rs)->getObject(1, SQLDBC_HOSTTYPE_INT4, &result, 0, 0);
              if(result != 131072) {
                  loginfo("Found packet size %d, disabling packet based checks.", result);
                  ignore_packets = true;
              }
          }
          if(!m_success) {
              connection()->releaseStatement(ps_packetsize);
              return m_success;
          }
      }

        SQLDBC_Retcode rc;
		SQLDBC_Int8 counters[SQLDBC_PROFILE_MAX];
        
    initCounter(counters, ignore_packets);
      
		for (int i=0; i< maxVal ;i++){

            myEnv[i] = new SQLDBC_Environment(m_rt);
            if(!myEnv[i]) {
              logerror("SQLDBC_Environment_createConnection() failed");
              return SQLDBC_FALSE;
            }

			myCon[i]= myEnv[i]->createConnection();
            tc_errorcheck(myCon[i])->connect(m_frame->hostname(),
                                       SQLDBC_NTS,
                                       m_frame->database(),
                                       SQLDBC_NTS,
                                       m_frame->username(),
                                       SQLDBC_NTS,
                                       m_frame->password(),
                                       SQLDBC_NTS,
                                       SQLDBC_StringEncodingAscii,
                                       connectproperties());
            counters[SQLDBC_CONNECTIONCOUNT]++;
            counters[SQLDBC_STATEMENTCOUNT]++;   //every connection contains an internal statement

          myPStmt[i] = myCon[i]->createPreparedStatement();
          if(!myPStmt[i]) {
              logerror("createPreparedStatement() failed");
              return SQLDBC_FALSE;
          }
          counters[SQLDBC_PREPAREDSTATEMENTCOUNT]++;
          tc_errorcheck(myPStmt[i])->prepare("SELECT * INTO ? FROM DUAL", SQLDBC_StringEncodingAscii);
          counters[SQLDBC_PREPARECOUNT]++;
          counters[SQLDBC_PARSINFOMISSCOUNT]++;
          tc_errorcheck(myPStmt[i])->execute();
          counters[SQLDBC_EXECUTECOUNT]++;
          counters[SQLDBC_SELECTCOUNT]++;
//          counters[SQLDBC_DATAREADSIZE]+=2;


          myStmt[i] = myCon[i]->createStatement();
          if(!myStmt[i]) {
              logerror("createStatement() failed");
              return SQLDBC_FALSE;
          }
          tc_errorcheck(myStmt[i])->execute("SELECT * FROM DUAL", SQLDBC_StringEncodingAscii);
          counters[SQLDBC_STATEMENTCOUNT]++;
          counters[SQLDBC_EXECUTECOUNT]++;
          counters[SQLDBC_SELECTCOUNT]++;
          counters[SQLDBC_FETCHROWCOUNT]++;
//          counters[SQLDBC_DATAREADSIZE]+=2;
		}

        SQLDBC_Profile* prof = myEnv[0]->getProfile();

        if ( checkCounter(prof, &counters[0]) == SQLDBC_FALSE){
          return m_success;
        }

        /*multiple calls to collectCounters should not influence the counter values*/
        if ( checkCounter(prof, &counters[0]) == SQLDBC_FALSE){
          return m_success;
        }

		for (int p=0; p< 100 ;p++){
			char stmt[100];
			SQLDBC_Statement *myStmt = myCon[0]->createStatement();
			if(!myStmt) {
			  logerror("createStatement() failed");
			  return SQLDBC_FALSE;
			}
			counters[SQLDBC_STATEMENTCOUNT]++;
            
			tc_errorcheck(myStmt)->execute("SELECT * FROM DUAL", SQLDBC_StringEncodingAscii);
			counters[SQLDBC_EXECUTECOUNT]++;
            counters[SQLDBC_SELECTCOUNT]++;
            counters[SQLDBC_FETCHROWCOUNT]++;
//            counters[SQLDBC_DATAREADSIZE]+=2;
        
       		rc = myStmt->execute("DROP TABLE PARSEAGAINTAB", SQLDBC_StringEncodingAscii);
            if (rc != SQLDBC_OK){
//              counters[SQLDBC_SQLERRORCOUNT]++;
            }
            counters[SQLDBC_EXECUTECOUNT]++;
	    	tc_errorcheck(myStmt)->execute("CREATE TABLE PARSEAGAINTAB (a varchar (8000) ascii)", SQLDBC_StringEncodingAscii);
            counters[SQLDBC_EXECUTECOUNT]++;

            tc_errorcheck(myStmt)->execute("INSERT INTO PARSEAGAINTAB VALUES ('STARSIGN')", SQLDBC_StringEncodingAscii);
            counters[SQLDBC_EXECUTECOUNT]++;
            counters[SQLDBC_INSERTCOUNT]++;
            tc_errorcheck(myStmt)->execute("UPDATE PARSEAGAINTAB SET A = 'HAPPY ENDINGS'", SQLDBC_StringEncodingAscii);
            counters[SQLDBC_EXECUTECOUNT]++;
            counters[SQLDBC_UPDATECOUNT]++;
            tc_errorcheck(myStmt)->execute("DELETE FROM PARSEAGAINTAB", SQLDBC_StringEncodingAscii);
            counters[SQLDBC_EXECUTECOUNT]++;
            counters[SQLDBC_DELETECOUNT]++;
            tc_errorcheck(myCon[0])->releaseStatement(myStmt);
			
			
			SQLDBC_PreparedStatement * myPStmt = myCon[0]->createPreparedStatement();
			if(!myPStmt) {
			  logerror("createPreparedStatement() failed");
			  return SQLDBC_FALSE;
			}
			counters[SQLDBC_PREPAREDSTATEMENTCOUNT]++;
			tc_errorcheck(myPStmt)->prepare("INSERT INTO PARSEAGAINTAB VALUES (?)");
            counters[SQLDBC_PREPARECOUNT]++;
            if (p==0){
            	counters[SQLDBC_PARSINFOMISSCOUNT]++;
            } else {
            	counters[SQLDBC_PARSEINFOHITCOUNT]++;
            	counters[SQLDBC_PARSEAGAINCOUNT]++;
//                counters[SQLDBC_SQLERRORCOUNT]++;
//                counters[SQLDBC_SQLERRORCOUNT]++; //ist eigentlich ein bug, dass der error zweimal gezählt wird
            }	

            char pdata[100];
            memset(pdata,'a',sizeof(pdata));
            SQLDBC_Length ind[100];
            for (int x=0; x<100 ; x++){
              ind[x]=1;
//              counters[SQLDBC_DATAWRITESIZE]+=8001;
            }

            tc_errorcheck(myPStmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, &pdata[0], &ind[0], 1, SQLDBC_FALSE);
//            counters[SQLDBC_DATAWRITESIZE]+=8001;
            if (p!=0){
//              counters[SQLDBC_DATAWRITESIZE]+=8001; //due to parseagain
            }
			tc_errorcheck(myPStmt)->execute();
            counters[SQLDBC_EXECUTECOUNT]++;
            counters[SQLDBC_INSERTCOUNT]++;

            tc_errorcheck(myPStmt)->bindParameter(1, SQLDBC_HOSTTYPE_ASCII, &pdata[0], &ind[0], 1, SQLDBC_FALSE);
            tc_errorcheck(myPStmt)->setBatchSize(100);

			tc_errorcheck(myPStmt)->execute();
            counters[SQLDBC_EXECUTECOUNT]++;
            counters[SQLDBC_INSERTCOUNT]++;
            
            tc_errorcheck(myPStmt)->setBatchSize(1);
            tc_errorcheck(myPStmt)->clearParameters();


			tc_errorcheck(myPStmt)->prepare("SELECT * FROM PARSEAGAINTAB", SQLDBC_StringEncodingAscii);
            counters[SQLDBC_PREPARECOUNT]++;
            if (p==0){
            	counters[SQLDBC_PARSINFOMISSCOUNT]++;
            } else {
            	counters[SQLDBC_PARSEINFOHITCOUNT]++;
            	counters[SQLDBC_PARSEAGAINCOUNT]++;
//                counters[SQLDBC_SQLERRORCOUNT]++;
            }	
			tc_errorcheck(myPStmt)->execute();
            counters[SQLDBC_EXECUTECOUNT]++;
            counters[SQLDBC_SELECTCOUNT]++;

            SQLDBC_ResultSet * rs = myPStmt->getResultSet();
            int rc_count= 0;
            while (SQLDBC_OK == rs->next()){
            	rc_count++;
//                counters[SQLDBC_DATAREADSIZE]+=8001;
          	}
            if(!ignore_packets) {
                counters[SQLDBC_FETCHCOUNT]+=6;
            }
            counters[SQLDBC_FETCHROWCOUNT]+=rc_count;

  			tc_errorcheck(myPStmt)->prepare("SELECT * FROM PARSEAGAINTAB FOR UPDATE", SQLDBC_StringEncodingAscii);
            counters[SQLDBC_PREPARECOUNT]++;
            if (p==0){
            	counters[SQLDBC_PARSINFOMISSCOUNT]++;
            } else {
            	counters[SQLDBC_PARSEINFOHITCOUNT]++;
            	counters[SQLDBC_PARSEAGAINCOUNT]++;
//                counters[SQLDBC_SQLERRORCOUNT]++;
            }	
			tc_errorcheck(myPStmt)->execute();
            counters[SQLDBC_EXECUTECOUNT]++;
            counters[SQLDBC_SELECTCOUNT]++;
            counters[SQLDBC_NOTSELECTFETCHOPTIMIZED]++;
        
			sprintf (stmt, "SELECT 'HOMER_%03d' FROM DUAL", p);
			tc_errorcheck(myPStmt)->prepare(stmt, SQLDBC_StringEncodingAscii);
            counters[SQLDBC_PREPARECOUNT]++;
            counters[SQLDBC_PARSINFOMISSCOUNT]++;
			tc_errorcheck(myPStmt)->execute();
            counters[SQLDBC_EXECUTECOUNT]++;
            counters[SQLDBC_SELECTCOUNT]++;
            counters[SQLDBC_FETCHROWCOUNT]++;
//            counters[SQLDBC_DATAREADSIZE]+=10;
    		tc_errorcheck(myCon[0])->releaseStatement(myPStmt);
        }
    
        if ( checkCounter(prof, &counters[0]) == SQLDBC_FALSE){
          return m_success;
        }

        /*test reset counter*/
        prof->resetCounters();
   	    initCounter(counters, ignore_packets);

        if ( checkCounter(prof, &counters[0]) == SQLDBC_FALSE){
          return m_success;
        }

        /*test connection counters*/
		for (int m=0; m< maxVal ;m++){
           tc_errorcheck(myCon[m])->commit();
           counters[SQLDBC_COMMITCOUNT]++;
           tc_errorcheck(myCon[m])->rollback();
           counters[SQLDBC_ROLLBACKCOUNT]++;
           tc_errorcheck(myCon[m])->setTransactionIsolation(2);
           counters[SQLDBC_SETISOLATIONCOUNT]++;
           tc_errorcheck(myCon[m])->cancel();
           counters[SQLDBC_CANCELCOUNT]++;
        }
        if ( checkCounter(prof, &counters[0]) == SQLDBC_FALSE){
          return m_success;
        }


        parseAgainTest(counters);
        if ( checkCounter(prof, &counters[0]) == SQLDBC_FALSE){
          return m_success;
        }

        longTest(counters);
        if ( checkCounter(prof, &counters[0]) == SQLDBC_FALSE){
          return m_success;
        }

        /*release SQLDBC handles*/
		for (int n=0; n< maxVal ;n++){

           tc_errorcheck(myCon[n])->releaseStatement(myStmt[n]);
           tc_errorcheck(myCon[n])->releaseStatement(myPStmt[n]);

           myEnv[n]->releaseConnection(myCon[n]);
           delete myEnv[n];
        }

		return m_success;
	}

	SQLDBC_Bool checkCounter(SQLDBC_Profile* prof, SQLDBC_Int8* counter){
	    SQLDBC_Bool erg = SQLDBC_TRUE;
        prof->collectCounters  (    );
        for (int i=0; i < SQLDBC_PROFILE_MAX; i++){
          SQLDBC_UInt8	profVal = prof->getCounter((SQLDBC_ProfileCountValues)i);
          if (counter[i] >= 0 && profVal != counter[i]){
#ifdef _WIN32
            logerror("Wrong counter value for counter index %d - counter retrieved %I64d expected %I64d", i, profVal, (SQLDBC_UInt8)(counter[i]));
#else
            logerror("Wrong counter value for counter index %d - counter retrieved %lu expected %lu", i, (SQLDBC_UInt8)profVal, (SQLDBC_UInt8)counter[i]);
#endif
            erg = SQLDBC_FALSE;
          } else {
#ifdef VERBOSE
#ifdef _WIN32
            loginfo("counter value %d <%I64d>", i, profVal);
#else
            loginfo("counter value %d <%lu>", i, profVal);
#endif
#endif
          }
        }
        return erg;
     }
};

class CallDBProcProfile
    : public SQLDBC_TestCase
{
public:
    CallDBProcProfile()
    :SQLDBC_TestCase("CallDBProcProfile")
    {}

    SQLDBC_Bool run()
    {
        execSQL("DROP DBPROC CALLDBPROCPROFILE");
        execSQL("CREATE DBPROC CALLDBPROCPROFILE AS SELECT 1 FROM SYSDBA.DUAL;");
        SQLDBC_Profile *p = environment()->getProfile();
        if(!p) {
            logerror("NO PROFILE");
            return false;
        }
        p->resetCounters();
        SQLDBC_PreparedStatement *ps=connection()->createPreparedStatement();
        ps->prepare("CALL CALLDBPROCPROFILE");
        ps->execute();
        connection()->releaseStatement(ps);
        p->collectCounters();
        
        SQLDBC_Int4 count = (SQLDBC_Int4) p->getCounter(SQLDBC_CALLDBPROCEDURECOUNT);
        if(count == 1) {
            loginfo("PROCEDURE CALL COUNT IS %d", count);
        } else {
            logerror("PROCEDURE CALL COUNT IS %d, expected 1", count);
        }
        return m_success;
    }
};

int main (int argc, char **argv)
{
	// Initalize from command line params
	SQLDBC_TestFrame testframe (argc, argv);

	/* TODO:
	 *
	 * add all test cases that you have described above
	 *
	 */
	testframe.addTestCase (new testcase1());
  testframe.addTestCase(new CallDBProcProfile());
//	testframe.addTestCase (new testcase2());
//	testframe.addTestCase (new testcase3());

	if (testframe.run ())
		return 0;

	return 1;
}


