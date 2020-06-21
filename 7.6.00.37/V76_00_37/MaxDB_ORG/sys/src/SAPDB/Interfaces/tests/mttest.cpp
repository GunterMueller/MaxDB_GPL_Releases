/****************************************************************************

  module      : mttest.cpp

  -------------------------------------------------------------------------

  responsible : D039759

  special area: Interface Runtime
  description : Multi-Thread Test.


  last changed: 2001-01-20
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2001-2005 SAP AG


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




*****************************************************************************/

#include "SAPDB/Interfaces/sapdbifr.h"
#include "SAPDB/Interfaces/tests/TestUtils.h"
#include "heo07.h"

static int THREAD_COUNT=10;
static int REPEAT_COUNT=5;

IFR_Bool memory_ok_ignored=true;

#ifdef WIN32 
#define SLEEP(seconds) Sleep(seconds*1000)
#define CLOCK_T DWORD
#define CLOCK GetTickCount
#else
#include <unistd.h>
#include <time.h>
#define SLEEP(seconds) sleep(seconds)
#define CLOCK_T clock_t
#define CLOCK clock
#endif

class MultiTest
    : public IFR_TestCase
{
public:
    MultiTest()
    :IFR_TestCase("MultiTest")
    {
        tsp00_ErrTextc errtext;
        teo07_ThreadErr err;
        sqlcreatesem(&m_readysem,
                     1,
                     errtext,
                     &err);
        sqlwaitsem(m_readysem); // block it
        sqlcreatemutex(&m_readycounter_mutex);
    }

    ~MultiTest()
    {
        sqldestroysem(m_readysem);
        sqlbeginmutex(&m_readycounter_mutex);
        sqldestroymutex(&m_readycounter_mutex);
    }

    IFR_Bool setup()
    {
//        __asm { int 3 };
        dropTable("MULTITHREAD_TEST");
        m_success = m_success && execSQL("CREATE TABLE MULTITHREAD_TEST (K INT PRIMARY KEY, V VARCHAR(256))");
        IFR_PreparedStmt *pinsert=connection()->createPreparedStatement();
        int k=0;
        pinsert->bindParameter(1, IFR_HOSTTYPE_INT4, &k, NULL, sizeof(IFR_Int4));
        pinsert->prepare("INSERT INTO MULTITHREAD_TEST VALUES(?, RFILL(\'\',\'A\', 256))", IFR_StringEncodingAscii);
        for(k=1; k<=500; ++k) {
            pinsert->execute();
        }
        connection()->releaseStatement(pinsert);
        execSQL("COMMIT WORK");
        return m_success;
    }
    
    static void * threadfunction(void *argument)
    {
        MultiTest *self=(MultiTest*)argument;
        for(int j=0; j<REPEAT_COUNT; ++j) {
            IFR_PreparedStmt *ps=self->connection()->createPreparedStatement();
            IFR_String statement("SELECT * FROM MULTITHREAD_TEST WHERE "
                                 ,IFR_StringEncodingAscii, self->connection()->allocator, memory_ok_ignored);
            for(int x=1; x<100; ++x) {
                statement.append("K < ? AND ", IFR_StringEncodingAscii, IFR_NTS, memory_ok_ignored);
            }
            statement.append("TRUE", IFR_StringEncodingAscii, IFR_NTS, memory_ok_ignored);
            IFR_Retcode parse_rc = ps->prepare(statement.getBuffer(), statement.getEncoding());
        
            IFR_Int4 klimit=450;
            for(int y=1; y<100; ++y) {
                ps->bindParameter(y, IFR_HOSTTYPE_INT4, &klimit, NULL, sizeof(IFR_Int4));
            }
            ps->setFetchSize(50);
            IFR_Retcode execute_rc = ps->execute();
            if(execute_rc != IFR_OK) {
                self->logerror("EXECUTE ERROR: %d/%s", ps->error().getErrorCode(), ps->error().getErrorText());
            }
            IFR_ResultSet* rs=ps->getResultSet();
            if(rs != 0) {
                int k=1;
                while(rs->next() == IFR_OK) {
                    ++k;
                }
            } 
            self->connection()->releaseStatement(ps);
        }
        self->setReady();
        return NULL;
    }


    IFR_Bool run()
    {
        loginfo("Starting %d threads for %d loops.", THREAD_COUNT, REPEAT_COUNT);
        CLOCK_T starttime=CLOCK();
        teo07_Thread *test_threads = new teo07_Thread[THREAD_COUNT];
        m_success = m_success && setup();
        m_readycounter=0;
        tsp00_ErrTextc errtext;
        teo07_ThreadErr err;
        for(int i=0; i<THREAD_COUNT; ++i) {
            sqlbeginthread(0, // will be increased to min
                           MultiTest::threadfunction,
                           this,
                           0,
                           &test_threads[i],
                           errtext,
                           &err);
            if(err != THR_OK_EO07) {
                logerror("%s", errtext.asCharp());
                exit(1);
            }
        }
        sqlwaitsem(m_readysem);
        CLOCK_T endtime=CLOCK();
        loginfo("Clock ticks elapsed: %d", endtime - starttime);
        delete [] test_threads;
        return m_success;
    }
    
    
    int setReady()
    {
        int result;
        sqlbeginmutex(&m_readycounter_mutex);
        result=m_readycounter++;
        loginfo("THREAD NO %d FINISHED.", result);
        if(m_readycounter == THREAD_COUNT) {
            sqlsignalsem(m_readysem);
        }
        sqlendmutex(&m_readycounter_mutex);
        return result;
    }

    teo07_Mutex           m_readycounter_mutex;
    teo07_ThreadSemaphore m_readysem;
    IFR_Int4              m_readycounter;
};


int main(int argc, char **argv)
{
    for(int i=0; i<argc; ++i) {
        if((strcmp(argv[i], "--threads")==0) && i!=argc-1) {
            THREAD_COUNT=atoi(argv[i+1]);
            if(THREAD_COUNT<=0) {
                THREAD_COUNT=5;
            }
            for(int k=i; k<argc-2; ++k) {
                argv[k]=argv[k+2];
            }
            argc-=2;
        } 
        if((strcmp(argv[i], "--loops")==0) && i!=argc-1) {
            REPEAT_COUNT=atoi(argv[i+1]);
            if(REPEAT_COUNT<=0) {
                REPEAT_COUNT=5;
            }
            for(int j=i; j<argc-2; ++j) {
                argv[j]=argv[j+2];
            }
            argc-=2;
        } 
    }
    IFR_TestFrame testframe(argc, argv);
    testframe.addTestCase(new MultiTest());
    if(testframe.run()) return 0;
    return 1;
}
