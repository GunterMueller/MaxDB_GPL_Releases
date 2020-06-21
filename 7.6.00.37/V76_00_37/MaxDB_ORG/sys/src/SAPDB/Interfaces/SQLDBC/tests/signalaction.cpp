/*!
  @file           signalaction.cpp
  @author         D039759
  @ingroup        Tests
  @brief          Tests features against mis-use within a signal
                  handler.                 
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

#include <sys/timeb.h>
#include <time.h>
#include <signal.h>
#include <stdlib.h>

#ifndef WIN32
#include <unistd.h>
#else
#include <process.h>
#endif

#ifdef WIN32
#define FTIME _ftime
#define TIMEB _timeb
#else
#define FTIME ftime
#define TIMEB timeb
#endif

long _fdifftime (struct TIMEB tStart1, struct TIMEB   tEnde1){
    return ((tEnde1.time-tStart1.time)*1000+(tEnde1.millitm-tStart1.millitm));
}

static SQLDBC_Connection *the_connection;
static char *program_name;
static int   test_success = 0;

#ifdef WIN32
extern "C" void __cdecl raise_signal_thread( void * )
{
    Sleep(8000);
    raise(2);
}
#endif


extern "C" void signalhandler(int number)
{
    printf("INFO: PERFORMING ROLLBACK ON SIGNAL\n");
    SQLDBC_Retcode rc = the_connection->rollback();
    if(rc == SQLDBC_OK) {
        fprintf(stderr, "ERROR: ROLLBACK SUCCEEDED");
    } else {
        printf("INFO: ERROR MESSAGE: %s\n", the_connection->error().getErrorText()); 
    }
    if(the_connection->error().getErrorCode() == -10810) {
        test_success = 1;
    }
}

class SignalAction
    : public SQLDBC_TestCase
{
public:
    SignalAction()
    :SQLDBC_TestCase("SignalAction")
    {}

    SQLDBC_Bool run()
    {
        // install the signal handler ...
        signal(2, &signalhandler);
        // now do something ...
        the_connection = connection();
        execSQL("DROP DBPROC LOOPING");
        execSQL("CREATE DBPROC LOOPING(IN A INTEGER) AS "
                "  VAR C INTEGER; "
                "  SET C = 0; "
                "  WHILE C < A DO BEGIN "
                "    SELECT * FROM DOMAIN.COLUMNS; "
                "    SET C = C + 1; "
                "  END; "
                );
        connection()->commit();
#ifndef _WIN32        
        int pid = getpid();
        loginfo("PID IS %d", pid);
        char executor[200];
        sprintf(executor, "%s KILL 8 %d 2 &", program_name, pid);
        loginfo("RUNNING |%s|", executor);
        system(executor);
#else
        _beginthread(raise_signal_thread, 0, 0);
#endif
        loginfo("Running procedure.");
        execSQL("CALL LOOPING(200)");
        if(test_success == 1) {
            loginfo("Correct error in signal handler received.");
            m_success = true;
        }
        return m_success;
    }
};

void run_as_killer(char *sleepstr, char *pidstr, char *sig)
{
#ifndef WIN32
    int sleeptime = atoi(sleepstr);
    int pid = atoi(pidstr);
    int signalnum = atoi(sig);
    sleep(sleeptime);
    kill(pid, signalnum);
#endif
    return;
}





int main (int argc, char **argv)
{
    program_name = argv[0];
    
    if(strcmp(argv[1], "KILL")==0) {
        run_as_killer(argv[2], argv[3], argv[4]);
        return 0;
    }
    
    // Initalize from command line params
    SQLDBC_TestFrame testframe (argc, argv);
    testframe.addTestCase (new SignalAction());
    if (testframe.run ())
        return 0;
    
  return 1;
}
