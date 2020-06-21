/*!================================================================
 module:    FastloaderTestUtils.cpp

 responsible: D019882

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
#define LOADER_NO_MAIN_FUNC
//#include "Interfaces/tests/TestUtils.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>
#include "Loader/Loader_Loader.hpp"
#include "Loader/FastloaderTests/FastloaderTestUtils.h"
#include "RunTime/MemoryManagement/RTEMem_RawAllocator.hpp"             // ???

//----------------------------------------------------------------------
static char *error_text(SQLDBC_ErrorHndl& error)
{
    static char buffer[1024];
    sp77sprintfUnicode(sp77encodingUTF8, buffer, 1024, "Error %d, sql state %5s, %s",
                       error.getErrorCode(), 
                       error.getSQLState(),
                       error.getErrorText());
    return buffer;
}

//----------------------------------------------------------------------
FastloaderTestCase::FastloaderTestCase(const char* name)
:m_pFLCmd(NULL),
m_pStatement(NULL)
{
    strcpy(m_name, name);
    *m_fullname = '\0';
    m_frame     = 0;
    m_success   = true;
}

//----------------------------------------------------------------------
FastloaderTestCase::~FastloaderTestCase()
{
    if(m_pFLCmd) {
        m_frame->loaderConnection()->releaseFastloadCommand(m_pFLCmd);
    }

    if (m_pStatement)
    {
        m_frame->sqldbcConnection()->releaseStatement(m_pStatement);
    }
            
}

//----------------------------------------------------------------------
Loader_Connection*
FastloaderTestCase::getLoaderConnection() const
{
    if(m_frame)
    {
        return m_frame->loaderConnection();
    }
    else
    {
        return 0;
    }
}

//----------------------------------------------------------------------
SQLDBC_Connection*
FastloaderTestCase::getSQLDBCConnection() const
{
    if(m_frame)
    {
        return m_frame->sqldbcConnection();
    }
    else
    {
        return 0;
    }
}


//----------------------------------------------------------------------
SQLDBC_Environment*
FastloaderTestCase::environment() const
{
    if(m_frame)
    {
        return m_frame->environment();
    }
    else
    {
        return 0;
    }
}

//----------------------------------------------------------------------
char *
FastloaderTestCase::database() const
{
    if(m_frame)
    {
        return m_frame->database();
    }
    else
    {
        return 0;
    }
}

//----------------------------------------------------------------------
char *
FastloaderTestCase::hostname() const
{
    if(m_frame) {
        return m_frame->hostname();
    } else {
        return 0;
    }
}

//----------------------------------------------------------------------
char *
FastloaderTestCase::username() const
{
    if(m_frame) {
        return m_frame->username();
    } else {
        return 0;
    }
}

//----------------------------------------------------------------------
char *
FastloaderTestCase::password() const
{
    if(m_frame) {
        return m_frame->password();
    } else {
        return 0;
    }
}


//----------------------------------------------------------------------
const SQLDBC_ConnectProperties&
FastloaderTestCase::connectproperties() const
{
    return m_frame->connectproperties();
}


//----------------------------------------------------------------------
SQLDBC_Statement*
FastloaderTestCase::statement() 
{
    if (0 == m_pStatement)
    {
        if (0 == getSQLDBCConnection())
        {
            return 0;
        }
        else
        {
            m_pStatement = getSQLDBCConnection()->createStatement();
        }
    }
    return m_pStatement;
}


//----------------------------------------------------------------------
Loader_FastloadCommand *
FastloaderTestCase::FastloadCommand() 
{
    if (0 == m_pFLCmd)
    {
        if (0 == getLoaderConnection())
        {
            return 0;
        }
        else
        {
            m_pFLCmd = getLoaderConnection()->createFastloadCommand();
        }
    }
    return m_pFLCmd;
}


void
FastloaderTestCase::releaseFLCommand()
{
    if (0 != m_pFLCmd)
    {
        Loader_Connection *_pLoaderConn = getLoaderConnection();
        if (0 != _pLoaderConn)
        {
            _pLoaderConn->releaseFastloadCommand(m_pFLCmd);
            m_pFLCmd = 0;
        }
    }
}

//----------------------------------------------------------------------
LOADER_Bool
FastloaderTestCase::run()
{
    // virtual function - nothing to do
    return m_success = false;
}

//----------------------------------------------------------------------
void
FastloaderTestCase::setFrame(FastloaderTestFrame *f)
{
    strcpy(m_fullname, f->m_fullname);
    strcat(m_fullname, ".");
    strcat(m_fullname, m_name);
    m_frame=f;
}

//----------------------------------------------------------------------
void 
FastloaderTestCase::loginfo(const char *fmt, ...)
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
FastloaderTestCase::logerror(const char *fmt, ...)
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
FastloaderTestCase::logprot(const char *fmt, ...)
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
FastloaderTestCase::dropTable(const char *tablename)
{
    char dropstatement[1024];
    sprintf(dropstatement, "DROP TABLE %s", tablename);
    execSQL(dropstatement);
}

//----------------------------------------------------------------------
LOADER_Bool
FastloaderTestCase::execSQL(const char *sql)
{
    if(0 == statement())
    {
        logerror("Cannot execute statement %s", sql);
    }
    statement()->execute(sql, IFR_NTS, IFR_StringEncodingAscii);
    if(statement()->error())
    {
        loginfo("Execute %s", sql);
        loginfo("%s", error_text(statement()->error()));
        statement()->clearError();
        return false;
    }
    return true;
}

//LOADER_Retcode FastloaderTestCase::getPacketSize(IFR_Int4 &cbPacketSize)
//{
//  IFR_Retcode rc;
//  IFR_Bool memory_ok_ignored=true;
//  char selinto[] = "SELECT VALUE INTO ? FROM DBPARAMETERS WHERE DESCRIPTION = 'PACKET_SIZE' OR DESCRIPTION = '_PACKET_SIZE'";
//  IFR_PreparedStmt *stmt = tc_errorcheck (connection())->createPreparedStatement ();
//  IFR_String sql(selinto, IFR_StringEncodingAscii, connection()->allocator, memory_ok_ignored);
//  rc = tc_errorcheck (stmt)->prepare(sql.getBuffer(), sql.getEncoding());
//  if (rc == IFR_OK) {
//    tc_errorcheck (stmt)->bindParameter (1, IFR_HOSTTYPE_INT4, &cbPacketSize, NULL, sizeof(cbPacketSize));
//    cbPacketSize = 99999;
//  }
//  if (rc == IFR_OK)
//    rc = tc_errorcheck (stmt)->execute();
//  if (rc != IFR_OK) {
//    m_success = false;
//  }
//  if (cbPacketSize == 99999)
//    logerror ("PackeSize not returned\n");      
//  loginfo("PacketSize = %d\n", cbPacketSize);    
//  connection()->releaseStatement(stmt);
//  return rc;
//} 


// TestFrame
static LOADER_Bool glbl_memory_ok_ignored=true;

//----------------------------------------------------------------------
FastloaderTestFrame::FastloaderTestFrame(int argc, char **argv)
:FastloaderTestCase(argv[0]),
 m_stream(0),
 m_packettrace(false),
 m_environment(0),
 m_properties(),
 m_tc(RTEMem_RawAllocator::Instance()),
 m_testloopcount(1),
 m_pLoader(0)
{
    m_frame = this;             // base class member
    m_good  = init(argc, argv);
}


//----------------------------------------------------------------------
FastloaderTestFrame::~FastloaderTestFrame()
{
    if(m_stream)
        fclose(m_stream);
    if(m_password) {
        free(m_password);
        m_password = 0;
    }
    if(m_username) {
        free(m_username);
        m_username = 0;
    }
    if(m_host) {
        free(m_host);
        m_host = 0;
    }

    if (NULL != m_pLoader)
    {
        delete m_pLoader;
    }
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
namespace SQLDBC {
    SQLDBC_IRuntime *GetSingleThreadClientRuntime(char *errorText, const SQLDBC_Int4 errorTextSize );
};

//----------------------------------------------------------------------
LOADER_Bool
FastloaderTestFrame::init(int argc, char **argv)
{
    // strip leading path
    char *last_slash     = strrchr(argv[0], '/');
    char *last_backslash = strrchr(argv[0], '\\');
    
    char *used_name = last_slash > last_backslash ? last_slash : last_backslash;
    if(used_name == 0)
    {
        used_name = argv[0];
    }
    else
    {
        ++used_name;
    }

    strcpy(m_name, used_name);
    strcpy(m_fullname, used_name);
    int single_threaded = 1;
    m_host = m_database = m_username = m_password = 0;

    // Parse options
    int i=1;
    while(i < argc)
    {
        if (0 == strcmp("-n", argv[i]))
        {
            ++i;
            if (i == argc)
            {
                logerror("Option -n requires an argument.\n");
                exit(1);
            }
            m_host = strdup(argv[i]);
            ++i;
            continue;
        } 
        if (0 == strcmp("-d", argv[i]))
        {
            ++i;
            if (i == argc)
            {
                logerror("Option -d requires an argument.\n");
                exit(1);
            }
            m_database = strupper(strdup(argv[i]));
            ++i;
            continue;
        } 
        if (0 == strcmp("-u", argv[i]))
        {
            ++i;
            if (i == argc)
            {
                logerror("Option -u requires an argument.\n");
                exit(1);
            }
            char *comma = strchr(argv[i], ',');
            if (0 == comma)
            {
                logerror("Option -u requires username,password.\n");
                exit(1);
            }
            m_username = (char*)malloc(comma - argv[i] +1);
            memcpy(m_username, argv[i], comma - argv[i]);
            m_username[comma - argv[i]] = '\0';
            m_password = (char*)malloc(strlen(comma));
            strcpy(m_password, comma+1);
            strupper(m_password);
            strupper(m_username);
            ++i;
            continue;
        } 
        if(0 == strcmp("-t", argv[i]))
        {
            ++i;
            if (i == argc)
            {
                logerror("Option -t requires an argument.\n");
                exit(1);
            }
            m_stream = fopen(argv[i], "w");
            if (0 == m_stream)
            {
                logerror("Could not open trace file %s.\n", argv[i]);
                exit(1);
            }
            ++i;
            continue;
        }
        if (0 == strcmp("-p", argv[i]))
        {
            ++i;
            if (i == argc)
            {
                logerror("Option -p requires an argument.\n");
                exit(1);
            }
            char *s  = argv[i];
            char *eq = strchr(s, '=');
            if (eq == 0)
            {
                logerror("Syntax error in property argument %s.\n", s);
            }
            *eq = 0;
            m_properties.setProperty(s, eq+1);
            *eq = '=';
            ++i;
            continue;
        }
        if (0 == strcmp("-lc", argv[i]))
        {
            ++i;
            if (i == argc)
            {
                logerror("Option -lc requires an argument.\n");
                exit(1);
            }
            m_testloopcount = atoi(argv[i]);
            ++i;
            continue;
        } 
        // We are single threaded!
/*        if (0 == strcmp("-mt", argv[i]))
        {
            ++i;
            single_threaded = 0;
            continue;
        }*/
        if (0 == strcmp("-x", argv[i]))
        {
            ++i;
            m_packettrace = true;
            continue;
        }
        ++i;
    }

    if (0 == m_host)
    {
        m_host = strdup("");
    }
    
    SQLDBC_IRuntime *rt = GetSingleThreadClientRuntime(0, 0);

    m_environment = new SQLDBC_Environment(rt);

    m_pLoader     = new Loader;
    
    if (!(m_username && m_password && m_database))
    {
        usage();
    }
    return true;
}

//----------------------------------------------------------------------
void
FastloaderTestFrame::usage()
{
    fprintf(stderr, "Usage: %s [options]\n", m_fullname);
    fprintf(stderr, "Where options are: \n");
    fprintf(stderr, "   -n <node>            the hostname of the database server\n");
    fprintf(stderr, "   -d <dbname>          the name of the database\n");
    fprintf(stderr, "   -u <user>,<password> username and password\n");
    fprintf(stderr, "   -p <key>=<value>     connect property key is set to value\n");
    fprintf(stderr, "   -t <filename>        stream trace file\n");
    fprintf(stderr, "   -lc <count>          run tests <count> times\n");
    exit(1);
}

//----------------------------------------------------------------------
FILE*
FastloaderTestFrame::infostream()
{
    if(m_stream) {
        return m_stream;
    } else {
        return stdout;
    }
}

//----------------------------------------------------------------------
FILE*
FastloaderTestFrame::errorstream()
{
    if(m_stream) {
        return m_stream;
    } else {
        return stderr;
    }
}

//----------------------------------------------------------------------
FILE*
FastloaderTestFrame::protstream()
{
    return m_stream;
}



//----------------------------------------------------------------------
void
FastloaderTestFrame::info(char *who, char *format, va_list ap)
{
    char prefix[1024];
    char buffer[1024];
    strcpy(prefix, "INFO:");
    strcat(prefix, who);
    strcat(prefix, ":");
    sp77vsprintfUnicode(sp77encodingAscii, buffer, 1024, format, ap);
    fputs(prefix, infostream());
    fputs(buffer, infostream());
    if(buffer[strlen(buffer)-1]!='\n') {
        fputs("\n", infostream());
    }
} 

//----------------------------------------------------------------------
void
FastloaderTestFrame::error(char *who, char *format, va_list ap)
{
    char prefix[1024];
    char buffer[1024];
    strcpy(prefix, "ERROR:");
    strcat(prefix, who);
    strcat(prefix, ":");
    sp77vsprintfUnicode(sp77encodingAscii, buffer, 1024, format, ap);
    fputs(prefix, errorstream());
    fputs(buffer, errorstream());
    if(buffer[strlen(buffer)-1]!='\n') {
        fputs("\n", errorstream());
    }
    m_success=false;
} 

//----------------------------------------------------------------------
void
FastloaderTestFrame::prot(char *who, char *format, va_list ap)
{
    if(protstream()) {
        char prefix[1024];
        char buffer[1024];
        strcpy(prefix, "PROT:");
        strcat(prefix, who);
        strcat(prefix, ":");
        sp77vsprintfUnicode(sp77encodingAscii, buffer, 1024, format, ap);
        fputs(prefix, protstream());
        fputs(buffer, protstream());
        if(buffer[strlen(buffer)-1]!='\n') {
            fputs("\n", protstream());
        }
    } 
}


//----------------------------------------------------------------------
LOADER_Bool
FastloaderTestFrame::run()
{
    connect();
    if(!m_good)
    {
        logerror("Connect failed.");
    }
    else
    {
        loginfo("Running %d tests\n", m_tc.GetSize());
        LOADER_Int4 loopcount = m_testloopcount > 1 ? m_testloopcount : 1;
        if (loopcount != 1)
        {
            loginfo("Running %d loops", loopcount);
        }
        for (LOADER_Int4 j=0; j<loopcount; ++j)
        {
            for (IFRUtil_Vector<FastloaderTestCase*>::Iterator i=m_tc.Begin(); i!=m_tc.End(); ++i)
            {
                if((*i)->run())
                {
                    loginfo("Test %s SUCCESS.\n", (*i)->name());
                }
                else
                {
                    loginfo("Test %s FAIL.\n", (*i)->name());
                    m_success = false;
                }
            }
        }
        disconnect();
    }
    if (m_success)
      loginfo("SUCCESS.\n");
    else
      logerror("FAIL.\n");      
    return m_success;
}

//----------------------------------------------------------------------
// Connect to DB over SQLDBC and to Loader
void
FastloaderTestFrame::connect()
{
    loginfo("----------------------------------------------------------------------\n");
    loginfo("Connect to host \'%s\', database \'%s\'.\n", m_host, m_database);
    loginfo("        as user %s with password %s\n", m_username, m_password);
    loginfo("----------------------------------------------------------------------\n");

    m_pSQLDBCConnection = m_environment->createConnection();

// Deprectated function was removed.
//     if(m_packettrace) 
//         m_connection->enablePacketTrace();
    
    m_good = LOADER_OK == m_pSQLDBCConnection->connect(m_host, LOADER_NTS,
                                                       m_database, LOADER_NTS,
                                                       m_username, LOADER_NTS,
                                                       m_password, LOADER_NTS,
                                                       IFR_StringEncodingAscii, m_properties);

    if (m_good)
    {
        m_pLoaderConnection = m_pLoader->createConnection(m_pSQLDBCConnection);
//TODOTODO check connection
    }

    if (!m_good)
    {
        logerror("Error: %d sql state %5s, %s", m_pSQLDBCConnection->error().getErrorCode(),
                                                m_pSQLDBCConnection->error().getSQLState(),
                                                m_pSQLDBCConnection->error().getErrorText());        
    }
}
// connect()


//----------------------------------------------------------------------
void
FastloaderTestFrame::disconnect()
{
    for(IFRUtil_Vector<FastloaderTestCase*>::Iterator i=m_tc.Begin(); i!=m_tc.End(); ++i)
    {
        FastloaderTestCase* todelete=*i;
        delete todelete;
    }

    m_pLoader->releaseConnection(m_pLoaderConnection);
    
    m_environment->releaseConnection(m_pSQLDBCConnection);
    delete m_environment;
    m_pSQLDBCConnection = 0;
    m_pLoaderConnection = 0;
    m_environment       = 0;
    loginfo("----------------------------------------------------------------------\n");
    loginfo("Disconnect from database. \n");
    loginfo("----------------------------------------------------------------------\n");
}


//----------------------------------------------------------------------
void
FastloaderTestFrame::addTestCase(FastloaderTestCase *tc)
{
    tc->setFrame(this);
    m_tc.InsertEnd(tc, glbl_memory_ok_ignored);
}
