/*!================================================================
 module:    SQLDBCTestUtils.cpp

 responsible: D039759

 special area: Test Utilities
 description:  

 see:
 
 change history:


    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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

#define SQLDBC_NO_MAIN_FUNC
#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestUtils.h"
#include "SAPDB/RunTime/MemoryManagement/RTEMem_Allocator.hpp"
#include "SAPDB/Container/Container_Vector.hpp"
#include "hsp77.h"
#include "hsp78_0.h"
#include "hsp81.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <errno.h>

//----------------------------------------------------------------------
static char *error_text(SQLDBC_ErrorHndl& error)
{
    static char buffer[1024];
    sprintf(buffer, "Error %d, sql state %5s, %s",
            error.getErrorCode(), 
            error.getSQLState(),
            error.getErrorText());
    return buffer;
}

//----------------------------------------------------------------------
SQLDBC_TestCase::SQLDBC_TestCase(const char* name)
:m_statement(0)
{
    strcpy(m_name, name);
    *m_fullname='\0';
    m_frame=0;
    m_success=true;
}

//----------------------------------------------------------------------
SQLDBC_TestCase::~SQLDBC_TestCase()
{
  if(m_frame && m_frame->connection() && m_statement)
    m_frame->connection()->releaseStatement(m_statement);
}

//----------------------------------------------------------------------
SQLDBC_Connection*
SQLDBC_TestCase::connection() const
{
    if(m_frame) {
        return m_frame->connection();
    } else {
        return 0;
    }
}

//----------------------------------------------------------------------
SQLDBC_Environment*
SQLDBC_TestCase::environment() const
{
    if(m_frame) {
        return m_frame->environment();
    } else {
        return 0;
    }
}

//----------------------------------------------------------------------
SQLDBC_IRuntime*
SQLDBC_TestCase::runtime() const
{
    if(m_frame) {
        return m_frame->runtime();
    } else {
        return 0;
    }
}

//----------------------------------------------------------------------
char *
SQLDBC_TestCase::database() const
{
    if(m_frame) {
        return m_frame->database();
    } else {
        return 0;
    }
}

//----------------------------------------------------------------------
char *
SQLDBC_TestCase::hostname() const
{
    if(m_frame) {
        return m_frame->hostname();
    } else {
        return 0;
    }
}

//----------------------------------------------------------------------
char *
SQLDBC_TestCase::username() const
{
    if(m_frame) {
        return m_frame->username();
    } else {
        return 0;
    }
}

//----------------------------------------------------------------------
char *
SQLDBC_TestCase::password() const
{
    if(m_frame) {
        return m_frame->password();
    } else {
        return 0;
    }
}

//----------------------------------------------------------------------
static SQLDBC_ConnectProperties emptyProperties;

//----------------------------------------------------------------------
const SQLDBC_ConnectProperties&
SQLDBC_TestCase::connectproperties() const
{
    if(m_frame) {
        return m_frame->connectproperties();
    } else {
        exit(1);
    }
    return emptyProperties;
}


//----------------------------------------------------------------------
SQLDBC_Statement *
SQLDBC_TestCase::statement() 
{
    if(m_statement==0) {
        if(connection()==0) {
            return 0;
        } else {
            m_statement=connection()->createStatement();
        }
    }
    return m_statement;
}
        



//----------------------------------------------------------------------
SQLDBC_Bool
SQLDBC_TestCase::run()
{
  return m_success=false;
}

//----------------------------------------------------------------------
SQLDBC_Bool
SQLDBC_TestCase::prepare()
{
  return m_success=true;
}

//----------------------------------------------------------------------
SQLDBC_Bool
SQLDBC_TestCase::release()
{
    return m_success;
}

//----------------------------------------------------------------------
void
SQLDBC_TestCase::setFrame(SQLDBC_TestFrame *f)
{
    strcpy(m_fullname, f->m_fullname);
    strcat(m_fullname, ".");
    strcat(m_fullname, m_name);
    m_frame=f;
}

//----------------------------------------------------------------------
void 
SQLDBC_TestCase::loginfo(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    if(m_frame) {
        m_frame->info(m_fullname, (char *) fmt, ap);
    }
    va_end(ap);
}

//----------------------------------------------------------------------
void 
SQLDBC_TestCase::logerror(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    if(m_frame) {
        m_frame->error(m_fullname, (char *) fmt, ap);
    }
    va_end(ap); 
    m_success=false;
}

//----------------------------------------------------------------------
void 
SQLDBC_TestCase::logprot(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    if(m_frame) {
        m_frame->prot(m_fullname, (char *) fmt, ap);
    }
    va_end(ap);
}

//----------------------------------------------------------------------
void
SQLDBC_TestCase::setLastError(SQLDBC_Int4 errorcode, 
                              const char *errormessage)
{
    if(m_frame) {
        m_frame->setLastError(errorcode, errormessage);
    }
}


//----------------------------------------------------------------------
void
SQLDBC_TestCase::dropTable(const char *tablename)
{
    char dropstatement[1024];
    sprintf(dropstatement, "DROP TABLE %s", tablename);
    execSQL(dropstatement);
}

//----------------------------------------------------------------------
SQLDBC_Bool
SQLDBC_TestCase::execSQL(const char *sql)
{
    if(statement()==0) {
        logerror("Cannot execute statement %s", sql);
    }
    statement()->execute(sql, SQLDBC_StringEncodingAscii);
    if(statement()->error()) {
        loginfo("Execute %s", sql);
        loginfo("%s", error_text(statement()->error()));
        return false;
    }
    return true;
}


// TestFrame

//----------------------------------------------------------------------
SQLDBC_TestFrame::SQLDBC_TestFrame(int argc, char **argv)
:SQLDBC_TestCase(argv[0]),
 m_stream(0),
 m_tc(RTEMem_Allocator::Instance()),
 m_properties(),
 m_runtime(0),
 m_lasterrorcode(0),
 m_silent(false)
{
    m_lasterrormessage[0]='\0';
    m_frame=this;
    m_good=init(argc, argv);
}

//----------------------------------------------------------------------
SQLDBC_TestFrame::SQLDBC_TestFrame(int argc, char **argv, 
                                   SQLDBC_IRuntime *customRuntime,
                                   SQLDBC_Bool silent)
:SQLDBC_TestCase(argv[0]),
 m_stream(0),
 m_tc(RTEMem_Allocator::Instance()),
 m_properties(),
 m_runtime(customRuntime),
 m_lasterrorcode(0),
 m_silent(silent)
{
    m_lasterrormessage[0]='\0';
    m_frame=this;
    m_good=init(argc, argv);
}

//----------------------------------------------------------------------
SQLDBC_TestFrame::~SQLDBC_TestFrame()
{
    if(m_stream)
        fclose(m_stream);
}

//----------------------------------------------------------------------
static char*
strupper(char *str)
{
    char *p=str;
    while(*p) {
        *p=toupper(*p);
        ++p;
    }
    return str;
}

//----------------------------------------------------------------------
SQLDBC_Bool
SQLDBC_TestFrame::init(int argc, char **argv)
{
    strcpy(m_name, argv[0]);
    strcpy(m_fullname, argv[0]);
    int single_threaded = 1;
    m_host=m_database=m_username=m_password=0;
    m_lockfile=0;
    m_userId=0;
    m_testloopcount=m_testlooptime=0;
    int i=1;
    while(i<argc) {
        if(strcmp("-s", argv[i])==0) {
            m_silent = true;
        }
        if(strcmp("-h", argv[i])==0) {
          usage();
          continue;
        }
        if(strcmp("-n", argv[i])==0) {
            ++i;
            if(i==argc) { logerror("Option -n requires an argument.\n"); exit(1); }
            m_host=strdup(argv[i]);
            ++i;
            continue;
        } 
        if(strcmp("-d", argv[i])==0) {
            ++i;
            if(i==argc) { logerror("Option -d requires an argument.\n"); exit(1); }
            m_database=strupper(strdup(argv[i]));
            ++i;
            continue;
        } 
        if(strcmp("-u", argv[i])==0) {
            ++i;
            if(i==argc) { logerror("Option -u requires an argument.\n"); exit(1); }
            char *comma=strchr(argv[i], ',');
            if(comma==0) { logerror("Option -u requires username,password.\n"); exit(1); }
            m_username=(char*)malloc(comma - argv[i] +1);
            memcpy(m_username, argv[i], comma - argv[i]);
            m_username[comma - argv[i]]='\0';
            m_password=(char*)malloc(strlen(comma));
            strcpy(m_password, comma+1);
            strupper(m_password);
            strupper(m_username);
            ++i;
            continue;
        } 
        if(strcmp("-t", argv[i])==0) {
            ++i;
            if(i==argc) { logerror("Option -t requires an argument.\n"); exit(1); }
            m_stream=fopen(argv[i], "w");
            if(m_stream==0) { logerror("Could not open trace file %s.\n", argv[i]); exit(1); }
            ++i;
            continue;
        }
        if(strcmp("-p", argv[i])==0) {
            ++i;
            if(i==argc) { logerror("Option -p requires an argument.\n"); exit(1); }
            char *s=argv[i];
            char *eq=strchr(s, '=');
            if(eq==0) {
                logerror("Syntax error in property argument %s.\n", s);
            }
            *eq=0;
            m_properties.setProperty(s, eq+1);
            *eq='=';
            ++i;
            continue;
        }
        if(strcmp("-lc", argv[i])==0) {
            ++i;
            if(i==argc) { logerror("Option -lc requires an <count> argument.\n"); exit(1); }
            m_testloopcount = atoi(argv[i]);
            ++i;
            continue;
        } 
        if(strcmp("-lt", argv[i])==0) {
            ++i;
            if(i==argc) { logerror("Option -lt requires an <seconds> argument.\n"); exit(1); }
            m_testlooptime = atoi(argv[i]);
            ++i;
            continue;
        } 
        if(strcmp("-mt", argv[i])==0) {
            ++i;
            single_threaded = 0;
            continue;
        }
        if(strcmp("-id", argv[i])==0) {
            ++i;
            if(i==argc) { logerror("Option -id requires integer argument.\n"); exit(1); }
            m_userId = atoi(argv[i]);
            ++i;
            continue;
        } 
        if(strcmp("-lock", argv[i])==0) {
            ++i;
            if(i==argc) { logerror("Option -lock requires an filename argument.\n"); exit(1); }
            m_lockfile=strdup(argv[i]);
            ++i;
            continue;
        } 
        ++i;
    }
    if(m_host==0) {
        m_host=strdup("");
    }
    if(m_testloopcount && m_testlooptime) {
      logerror("You can't combine the -lc with -lt argument.\n");
      exit(1);
    }
    if(!(m_username && m_password && m_database)) {
        usage();
    }
    // Initialize the runtime, if the user didn't specify a custom one.
    if(m_runtime == 0) {
        char errorText[512];
        if(single_threaded) {
            m_runtime = SQLDBC::GetSingleThreadClientRuntime(errorText, sizeof(errorText));
        } else {
            m_runtime = SQLDBC::GetClientRuntime(errorText, sizeof(errorText));
        }
        if(!m_runtime) {
            logerror("Error: %s", errorText);
            return false;
        }
    }
    connect();
    return m_good;
}

//----------------------------------------------------------------------
void
SQLDBC_TestFrame::usage()
{
    fprintf(stderr, "Usage: %s [options]\n", m_fullname);
    fprintf(stderr, "Where options are: \n");
    fprintf(stderr, "   -h                   this help\n");
    fprintf(stderr, "   -n <node>            the hostname of the database server\n");
    fprintf(stderr, "   -d <dbname>          the name of the database\n");
    fprintf(stderr, "   -u <user>,<password> username and password\n");
    fprintf(stderr, "   -p <key>=<value>     connect property key is set to value\n");
    fprintf(stderr, "   -t <filename>        stream trace file\n");
    fprintf(stderr, "   -s                   silent\n");
    fprintf(stderr, "   -lc <count>          run all tests <count> times\n");
    fprintf(stderr, "   -lt <seconds>        run the tests n <seconds>\n");
    fprintf(stderr, "   -lock <filename>     don't start test if this file exists\n");
    exit(1);
}

//----------------------------------------------------------------------
FILE*
SQLDBC_TestFrame::infostream()
{
    if(m_stream) {
        return m_stream;
    } else {
        return stdout;
    }
}

//----------------------------------------------------------------------
FILE*
SQLDBC_TestFrame::errorstream()
{
    if(m_stream) {
        return m_stream;
    } else {
        return stderr;
    }
}

//----------------------------------------------------------------------
FILE*
SQLDBC_TestFrame::protstream()
{
    return m_stream;
}



//----------------------------------------------------------------------
void
SQLDBC_TestFrame::info(char *who, char *format, va_list ap)
{
    if(!m_silent) {
        char prefix[1024];
        char buffer[1024];
        strcpy(prefix, "INFO:");
        strcat(prefix, who);
        strcat(prefix, ":");
        vsprintf(buffer, format, ap);
        fputs(prefix, infostream());
        fputs(buffer, infostream());
        if(buffer[strlen(buffer)-1]!='\n') {
            fputs("\n", infostream());
        }
    }
} 

//----------------------------------------------------------------------
void
SQLDBC_TestFrame::error(char *who, char *format, va_list ap)
{
    if(!m_silent) {
        char prefix[1024];
        char buffer[1024];
        strcpy(prefix, "ERROR:");
        strcat(prefix, who);
        strcat(prefix, ":");
        vsprintf(buffer, format, ap);
        fputs(prefix, errorstream());
        fputs(buffer, errorstream());
        if(buffer[strlen(buffer)-1]!='\n') {
            fputs("\n", errorstream());
        }
    }
    m_success=false;
} 

//----------------------------------------------------------------------
void
SQLDBC_TestFrame::prot(char *who, char *format, va_list ap)
{
    if(protstream()) {
        char prefix[1024];
        char buffer[1024];
        strcpy(prefix, "PTOT:");
        strcat(prefix, who);
        strcat(prefix, ":");
        vsprintf(buffer, format, ap);
        fputs(prefix, protstream());
        fputs(buffer, protstream());
        if(buffer[strlen(buffer)-1]!='\n') {
            fputs("\n", protstream());
        }
    } 
}


//----------------------------------------------------------------------
SQLDBC_Bool
SQLDBC_TestFrame::run()
{
    if(!m_good) {
        logerror("Connect failed.");
    } else {
        loginfo("Running %d tests\n", m_tc.GetSize());
        if(lockfile() != 0) { 
          long seconds = time( NULL );
          FILE* tLockfile = fopen(lockfile(), "a");
          fprintf(tLockfile, "%d=%ld\n", m_userId, seconds);
          fclose(tLockfile);
        }
        waitForRmLockfile(); 
        if(m_testloopcount > 1) { 
            loginfo("Repeating the test loop %d times\n", m_testloopcount);
        }
        if(m_testlooptime > 1) { 
            loginfo("Repeating the test loop %d seconds\n", m_testlooptime);
        }
        SQLDBC_Int4 loopcount = m_testloopcount;
        if(loopcount < 1) loopcount = 1;
        SQLDBC_Int4 looptime = m_testlooptime;
        long endtime, begintime;
        begintime=endtime=time(NULL);
        SQLDBC_Int4 l;
        for(l = 0; (l < loopcount || (endtime-begintime) < looptime); ++l) {
            for(Container_Vector<SQLDBC_TestCase*>::Iterator i=m_tc.Begin(); i!=m_tc.End(); ++i) {
                if((*i)->prepare() && (*i)->run() && (*i)->release() ) {
                                      loginfo("Test %s SUCCESS.\n", (*i)->name());
                } else {
                                      loginfo("Test %s FAIL.\n", (*i)->name());
                    m_success=false;
                }
            }
            endtime = time(NULL);
        }
        loginfo("Test loops %d times.\n", l);
        disconnect();
    }
    if (m_success)
      loginfo("SUCCESS.\n");
    else
      logerror("FAIL.\n");      
    waitForRmLockfile(); 
    return m_success;
}

#ifdef WIN32
#define R_OK 0
#define SLEEP(s) Sleep ( s * 1000l )
#else
#define SLEEP(s) sleep ( s )
#endif

//---------------------------------------------------------------------
void
SQLDBC_TestFrame::waitForRmLockfile()
{
  while (lockfile()){
    if (access(lockfile(), R_OK) == -1){
      if (errno != ENOENT){
        logerror( "Try to access lockfile" );
        exit (2);
      }
      break;
    }
    SLEEP(1);
  }
}

//---------------------------------------------------------------------
void
SQLDBC_TestFrame::setLastError(SQLDBC_Int4 errorcode, const char *errormessage)
{
    m_lasterrorcode = errorcode;
    strncpy(m_lasterrormessage, errormessage, 511);
    return;
}

//----------------------------------------------------------------------
void
SQLDBC_TestFrame::connect()
{
    loginfo("----------------------------------------------------------------------\n");
    loginfo("Connect to host \'%s\', database \'%s\'.\n", m_host, m_database);
    loginfo("        as user %s with password %s\n", m_username, m_password);
    loginfo("----------------------------------------------------------------------\n");
    //loginfo("Making new environment.\n");
    
    m_environment=new SQLDBC_Environment(m_runtime);
    
    //loginfo("New environment created.\n");
    m_connection=m_environment->createConnection();
    if(!m_connection) {
        logerror("Setup - SQLDBC_Environment::createConnection()");
        setLastError(-10760,
                     "Memory allocation failed.");
        m_good = false;
        return;
    }
    m_good = 
        m_connection->connect(m_host, m_database, m_username, m_password, m_properties)==SQLDBC_OK;

    if(!m_good) {
        logerror("Error: %d sql state %5s, %s", 
                 m_connection->error().getErrorCode(),
                 m_connection->error().getSQLState(),
                 m_connection->error().getErrorText()); 
        setLastError(m_connection->error().getErrorCode(),
                     m_connection->error().getErrorText());
    }
}

//----------------------------------------------------------------------
void
SQLDBC_TestFrame::disconnect()
{
    for(Container_Vector<SQLDBC_TestCase*>::Iterator i=m_tc.Begin(); i!=m_tc.End(); ++i) {
        SQLDBC_TestCase* todel=*i;
        delete todel;
    }
    m_environment->releaseConnection(m_connection);
    delete m_environment;
    m_connection=0;
    m_environment=0;
    loginfo("----------------------------------------------------------------------\n");
    loginfo("Disconnect from database. \n");
    loginfo("----------------------------------------------------------------------\n");
}


//----------------------------------------------------------------------
void
SQLDBC_TestFrame::addTestCase(SQLDBC_TestCase *tc)
{
    tc->setFrame(this);
    m_tc.InsertEnd(tc);
}

void Util_PrintError(const char *str, const SQLDBC_ErrorHndl *err)
{
  static char buf[1024];
  const char * szText = err->getErrorText();
  sp77sprintfUnicode(sp77encodingUTF8, buf, sizeof(buf), "ERROR: %s (%d):%s\n", str,  
                     err->getErrorCode(), err->getErrorText());
  printf(buf);
};

void Util_SPrintError(char *buf, const char *str, const SQLDBC_ErrorHndl *err)
{
    sp77sprintfUnicode(sp77encodingUTF8, buf, 1024, "ERROR: %s (%d):%s\n", str,  
                       err->getErrorCode(), err->getErrorText());
}

/*!
  Function:     convertToUCS2

  see also: 
  
  Description:  Converts Ascii to TCHAR depending on compile switch -G unicode
    
  Arguments:
    szTCHAR     [out] output buffer (TCHAR)
    maxLenTCHAR [in]  maximum length of szTCHAR
    swapped     [in]  set to true if the byte order of the output buffer 
                      should be in swapped byte order
    szStr       [in]  null terminated input buffer

  return value: error code
    <UL>
        <LI>zero    : Okay
        <LI>not zero: error occurs
    </UL>
 */

int convertToUCS2(UCS2CHAR *szTCHAR, int maxLenTCHAR, char *szStr) {
  SQLDBC_Bool swapped = (sp77encodingUCS2Swapped == sp77encodingUCS2Native) ? true : false;
  return convertToUCS2(szTCHAR, maxLenTCHAR, swapped, szStr);
}

int convertToUCS2(UCS2CHAR *szTCHAR, int maxLenTCHAR, SQLDBC_Bool swapped, char *szStr)
{
  tsp78ConversionResult ConvRes = sp78_Ok; 
  unsigned int   dlen, slen;
  const tsp77encoding *encoding = (swapped) ? sp77encodingUCS2Swapped : sp77encodingUCS2;
  ConvRes = sp78convertString(encoding, (char*) szTCHAR, maxLenTCHAR, &dlen, true,			     
			     sp77encodingAscii, szStr, strlen(szStr), &slen);
  if (sp78_Ok != ConvRes)
  {
    return (-1);
  }
  return (0);
}

/*!
  Function:     convertUCS2ToASCII

  see also: 
  
  Description:  Converts TCHAR to ASCII depending on compile switch -G unicode. 
  If cbLenszStr is null a UCS2strlen will be used to determine length of szStr.
    
  Arguments:
    szCHAR     [out] output buffer (CHAR)
    maxLenCHAR [in]  maximum length of szCHAR
    swapped    [in]  set to true if the byte order of the output buffer 
                     should be in swapped byte order
    szStr      [in]  null terminated input buffer (TCHAR)
    cbLenszStr [in]  length of szStr (in bytes)

  return value: error code
    <UL>
        <LI>zero    : Okay
        <LI>not zero: error occurs
    </UL>
 */
int convertUCS2ToASCII(char *szCHAR, int maxLenCHAR, UCS2CHAR *szStr, int cbLenszStr)
{
  return convertUCS2ToASCII(szCHAR, maxLenCHAR, false, szStr, cbLenszStr);
}

int convertUCS2ToASCII(char *szCHAR, int maxLenCHAR, SQLDBC_Bool swapped, UCS2CHAR *szStr, int cbLenszStr)
{
  tsp78ConversionResult ConvRes = sp78_Ok; 
  unsigned int dlen, slen;
  int cbLen = (cbLenszStr != 0)?cbLenszStr:2*sp81UCS2strlen((const tsp81_UCS2Char*) szStr); 
  const tsp77encoding *encoding = (swapped) ? sp77encodingUCS2Swapped : sp77encodingUCS2;

  ConvRes = sp78convertString(sp77encodingAscii, szCHAR, maxLenCHAR, &dlen, true,			     
			      encoding, (char*) szStr, cbLen, &slen);
  if (sp78_Ok != ConvRes)
  {
    return (-1);
  }
  return (0);
}

/*!
  Function:     convertToUTF8

  see also: 
  
  Description:  Converts Ascii to CHAR
    
  Arguments:
    szCHAR     [out] output buffer (CHAR)
    maxLenCHAR [in]  maximum length of szCHAR
    szStr      [in]  null terminated input buffer

  return value: error code
    <UL>
        <LI>zero    : Okay
        <LI>not zero: error occurs
    </UL>
 */

int convertToUTF8(char *szCHAR, int maxLenCHAR, char *szStr)
{
  tsp78ConversionResult ConvRes = sp78_Ok; 
  unsigned int   dlen, slen;
  ConvRes = sp78convertString(sp77encodingUTF8, szCHAR, maxLenCHAR, &dlen, true,			     
			     sp77encodingAscii, szStr, strlen(szStr), &slen);
  if (sp78_Ok != ConvRes)
  {
    return (-1);
  }
  return (0);
}

/*!
  Function:     convertUTF8ToASCII

  see also: 
  
  Description:  Converts TCHAR to ASCII depending on compile switch -G unicode. 
    
  Arguments:
    szCHAR     [out] output buffer (CHAR)
    maxLenCHAR [in]  maximum length of szCHAR
    szStr      [in]  null terminated input buffer (CHAR)
    cbLenszStr [in]  length of szStr

  return value: error code
    <UL>
        <LI>zero    : Okay
        <LI>not zero: error occurs
    </UL>
 */
int convertUTF8ToASCII(char *szCHAR, int maxLenCHAR, char *szStr, int cbLenszStr)
{
  tsp78ConversionResult ConvRes = sp78_Ok; 
  unsigned int dlen, slen;
//  int cbLen = (cbLenszStr != 0)?cbLenszStr:2*sp81UCS2strlen((const tsp81_UCS2Char*) szStr); 
  int cbLen = cbLenszStr;


  ConvRes = sp78convertString(sp77encodingAscii, szCHAR, maxLenCHAR, &dlen, true,			     
			      sp77encodingUTF8, szStr, cbLen, &slen);
  if (sp78_Ok != ConvRes)
  {
    return (-1);
  }
  return (0);
}


/*!
  Function:     HostTypeToStr

  see also: 
  
  Description:  Converts SQLDBC_HostType to char*
    
  Arguments:
    szCHAR     [out] output buffer (CHAR)
    maxLenCHAR [in]  maximum length of szCHAR
    HostType   [in]  host type

  return value: error code
    <UL>
        <LI>zero    : Okay
        <LI>not zero: buffer too small or unknown host type
    </UL>
 */
SQLDBC_Retcode HostTypeToStr(char *szCHAR, SQLDBC_Length maxLenCHAR, SQLDBC_HostType HostType) {
  switch (HostType) {
    case SQLDBC_HOSTTYPE_MIN: 
      {if (maxLenCHAR >= 20) sprintf(szCHAR, "SQLDBC_HOSTTYPE_MIN"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
//    case SQLDBC_HOSTTYPE_PARAMETER_NOTSET: 
//      {if (maxLenCHAR >= 33) sprintf(szCHAR, "SQLDBC_HOSTTYPE_PARAMETER_NOTSET"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_BINARY: 
      {if (maxLenCHAR >= 23) sprintf(szCHAR, "SQLDBC_HOSTTYPE_BINARY"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_ASCII: 
      {if (maxLenCHAR >= 22) sprintf(szCHAR, "SQLDBC_HOSTTYPE_ASCII"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_UTF8: 
      {if (maxLenCHAR >= 21) sprintf(szCHAR, "SQLDBC_HOSTTYPE_UTF8"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_UINT1: 
      {if (maxLenCHAR >= 22) sprintf(szCHAR, "SQLDBC_HOSTTYPE_UINT1"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_INT1: 
      {if (maxLenCHAR >= 21) sprintf(szCHAR, "SQLDBC_HOSTTYPE_INT1"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_UINT2: 
      {if (maxLenCHAR >= 22) sprintf(szCHAR, "SQLDBC_HOSTTYPE_UINT2"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_INT2: 
      {if (maxLenCHAR >= 21) sprintf(szCHAR, "SQLDBC_HOSTTYPE_INT2"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_UINT4: 
      {if (maxLenCHAR >= 22) sprintf(szCHAR, "SQLDBC_HOSTTYPE_UINT4"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_INT4: 
      {if (maxLenCHAR >= 21) sprintf(szCHAR, "SQLDBC_HOSTTYPE_INT4"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_UINT8: 
      {if (maxLenCHAR >= 22) sprintf(szCHAR, "SQLDBC_HOSTTYPE_UINT8"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_INT8: 
      {if (maxLenCHAR >= 21) sprintf(szCHAR, "SQLDBC_HOSTTYPE_INT8"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_DOUBLE: 
      {if (maxLenCHAR >= 23) sprintf(szCHAR, "SQLDBC_HOSTTYPE_DOUBLE"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_FLOAT: 
      {if (maxLenCHAR >= 22) sprintf(szCHAR, "SQLDBC_HOSTTYPE_FLOAT"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_ODBCDATE: 
      {if (maxLenCHAR >= 25) sprintf(szCHAR, "SQLDBC_HOSTTYPE_ODBCDATE"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_ODBCTIME: 
      {if (maxLenCHAR >= 25) sprintf(szCHAR, "SQLDBC_HOSTTYPE_ODBCTIME"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_ODBCTIMESTAMP: 
      {if (maxLenCHAR >= 30) sprintf(szCHAR, "SQLDBC_HOSTTYPE_ODBCTIMESTAMP"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_ODBCNUMERIC: 
      {if (maxLenCHAR >= 28) sprintf(szCHAR, "SQLDBC_HOSTTYPE_ODBCNUMERIC"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_GUID: 
      {if (maxLenCHAR >= 21) sprintf(szCHAR, "SQLDBC_HOSTTYPE_GUID"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_UCS2: 
      {if (maxLenCHAR >= 21) sprintf(szCHAR, "SQLDBC_HOSTTYPE_UCS2"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_UCS2_SWAPPED: 
      {if (maxLenCHAR >= 29) sprintf(szCHAR, "SQLDBC_HOSTTYPE_UCS2_SWAPPED"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
//    case SQLDBC_HOSTTYPE_UCS2_NATIVE: 
//      {if (maxLenCHAR >= 28) sprintf(szCHAR, "SQLDBC_HOSTTYPE_UCS2_NATIVE"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
    case SQLDBC_HOSTTYPE_USERDEFINED: 
      {if (maxLenCHAR >= 28) sprintf(szCHAR, "SQLDBC_HOSTTYPE_USERDEFINED"); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
#define HOSTTYPE(ht) (ht): \
      {if (maxLenCHAR >= (SQLDBC_Length) strlen(#ht)) sprintf(szCHAR, #ht); else return SQLDBC_NOT_OK;  return SQLDBC_OK;}
 case HOSTTYPE(SQLDBC_HOSTTYPE_BLOB);
 case HOSTTYPE(SQLDBC_HOSTTYPE_ASCII_CLOB);
 case HOSTTYPE(SQLDBC_HOSTTYPE_UCS2_CLOB);
 case HOSTTYPE(SQLDBC_HOSTTYPE_UTF8_CLOB);               
 case HOSTTYPE(SQLDBC_HOSTTYPE_RAWHEX);               
 case HOSTTYPE(SQLDBC_HOSTTYPE_STREAM);               
 case HOSTTYPE(SQLDBC_HOSTTYPE_OMS_PACKED_8_3);               
 case HOSTTYPE(SQLDBC_HOSTTYPE_OMS_PACKED_15_3);               
 case HOSTTYPE(SQLDBC_HOSTTYPE_OMS_TIMESTAMP);               
 case HOSTTYPE(SQLDBC_HOSTTYPE_OMS_ASCII);     
 case HOSTTYPE(SQLDBC_HOSTTYPE_DECIMAL);     
  default: {
   {if (maxLenCHAR >= (SQLDBC_Length) strlen("UNKNOWN")) sprintf(szCHAR, "UNKNOWN"); return SQLDBC_NOT_OK;}
 }
#undef HOSTTYPE
  }
  return SQLDBC_NOT_OK;
}


