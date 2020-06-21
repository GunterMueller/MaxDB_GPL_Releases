/*!================================================================

 module:        canceltest.cpp

 -------------------------------------------------------------------

 responsible:   D039759, D025086

 special area:  SQL

 description:   Test for statement cancellation.

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

#include <stdlib.h>
#include <stdio.h>
#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"
#include "heo07.h"
#include "heo02.h"
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

//----------------------------------------------------------------------
class CancelTest : public SQLDBC_TestCase
{
public:
    CancelTest ()
    : SQLDBC_TestCase ("CancelTest")
    {}

    virtual SQLDBC_Bool prepare()
    {
        dropTable("SQLDBC_SLEEP");
        m_success = m_success && execSQL("CREATE TABLE SQLDBC_SLEEP (c integer)");
        connection()->commit();
        teo07_Thread thread;
        tsp00_ErrTextc errtext;
        teo07_ThreadErr err;
        sqlinit_multithreading(&thread, errtext, &err);
        if(err != THR_OK_EO07) {
            logerror("%s", errtext.asCharp());
            m_success = false;
        }
        return m_success;
    }    

    virtual SQLDBC_Bool release ()
    {
      dropTable("SQLDBC_SLEEP");
      return m_success;
    }
    
    virtual SQLDBC_Bool run ()
    {
        SQLDBC_Environment env(runtime());
        this->conn = env.createConnection();
        tc_errorcheck(conn)->connect(hostname(), 
                                     database(), 
                                     username(),
                                     password());
        tc_errorcheck(conn)->setAutoCommit(SQLDBC_FALSE);
        SQLDBC_Statement *stm1=conn->createStatement();
        tc_errorcheck(stm1)->execute("INSERT INTO SQLDBC_SLEEP VALUES(1)", 
                                     SQLDBC_StringEncodingAscii);        

        SQLDBC_Statement *stm=connection()->createStatement();

        teo07_Thread cancelthread;
        tsp00_ErrTextc errtext;
        teo07_ThreadErr err;
        sqlbeginthread(0, 
                       (teo07_ThreadCallback)CancelTest::threadfunction, 
                       this,
                       0,
                       &cancelthread,
                       errtext,
                       &err);
        if(err != THR_OK_EO07) {
            logerror("%s", errtext.asCharp());
            exit(1);
        }
        ready=false;

        stm->execute("SELECT * FROM SQLDBC_SLEEP", SQLDBC_StringEncodingAscii);

        if(stm->error()) {
          if(stm->error().getErrorCode() == -102) {
            ready=true;
          } else {
            logerror("Error: %d sql state %5s, %s", 
                     stm->error().getErrorCode(),
                     stm->error().getSQLState(),
                     stm->error().getErrorText());        
            
          }
        }
        tsp00_Int4 status;
        sqljointhread(cancelthread, &status, errtext, &err);
        if(err != THR_OK_EO07) {
            logerror("%s", errtext.asCharp());
            exit(1);
        }

        m_success = m_success && ready;
        
        return m_success;
    }

    static void * threadfunction(void *ptr)
    {
        CancelTest *self=(CancelTest*)ptr;
        SQLDBC_Retcode rc;
        SLEEP(5);
        rc=self->connection()->cancel();
        if (rc != SQLDBC_OK)
          self->logerror("cancel doesn't return SQLDBC_OK");
        rc = self->conn->commit();
        if (rc != SQLDBC_OK)
          self->logerror("commit doesn't return SQLDBC_OK");
        return 0;
    }
    
    SQLDBC_Bool ready;
    SQLDBC_Connection *conn;
};


//----------------------------------------------------------------------
int main (int argc, char **argv)
{
    SQLDBC_TestFrame testframe (argc, argv);
    testframe.addTestCase (new CancelTest());
    testframe.run ();
    return 0;
}
