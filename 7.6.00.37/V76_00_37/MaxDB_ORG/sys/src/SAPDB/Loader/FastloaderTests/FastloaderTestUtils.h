/*!================================================================
 module:    FastloaderTestUtils.h

 responsible: D019882

 special area:
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
#ifndef FASTLOADERTESTUTILS_H
#define FASTLOADERTESTUTILS_H

#include <stdio.h>
#include <stdlib.h>
#include "SAPDB/Interfaces/sapdbifr.h"
#include "Interfaces/tests/Util.h"
#include "SAPDB/Loader/FastloaderTests/FastloaderTestMain.h"
//#include "SAPDB/Interfaces/SQLDBC/SQLDBC_ClientRuntime.h"
//#include "Interfaces/SQLDBC/SQLDBC.h"
#include "Loader/Loader_Loader.hpp"

//using namespace SQLDBC;

class FastloaderTestFrame;

/**
 * Single test. Derive from this class and implement the
 * <code>run()</code> method for a new test.  A test should setup
 * itself (drop and create the respective tables etc.) before running.
 */
class FastloaderTestCase
{
public:
    /**
     * Creates new test case.
     * @param name name that will appear in the log
     */
    FastloaderTestCase(const char *name);
    
    virtual ~FastloaderTestCase();

    /**
     * Runs the test. This method does nothing.
     * @return <code>true</code> if the test succeeded. <i>Use
     * the member <code>m_success</code> in derived classes
     * to check the error status.</i>
     */
    virtual LOADER_Bool run();
    
    /**
     * Internal method called on insert into test frame.
     * @param fmt format string (printf)
     * @param ... variable arguments (printf)
     */
    void setFrame(FastloaderTestFrame *frame);
    
    /**
     * Info message (preceeded with INFO).
     */
    void loginfo(const char *fmt, ...);

    /**
     * Error message (preceeded with ERROR). Calling this method sets the error status.
     * @param fmt format string (printf)
     * @param ... variable arguments (printf)
     */
    void logerror(const char *fmt, ...);

    /**
     * 'Protocol' message (preceeded with PROT). 
     * May be useful for tests based on reference protocol
     * difference (similar to GREPWEG).
     * @param fmt format string (printf)
     * @param ... variable arguments (printf)
     */
    void logprot(const char *fmt, ...);

    /**
     * Gets the single connection.
     * @return the database connection of the frame, or 0.
     */
    SQLDBC_Connection *pSQLDBCConnection() const; 

    /**
     * Gets the environment object.
     * @return the runtime environment.
     */
    SQLDBC_Environment *environment() const; 

    /**
     * Gets the single Loader connection.
     * @return the database connection of the frame, or 0.
     */
    Loader_Connection *pLOADERConnection() const; 


    /**
     * Gets the hostname from the testenvironment.
     * @return the runtime environment.
     */
    char *hostname() const; 

    /**
     * Gets the instancename from the testenvironment.
     * @return the runtime environment.
     */
    char *database() const; 

    /**
     * Gets the username from the testenvironment.
     * @return the runtime environment.
     */
    char *username() const; 

    /**
     * Gets the password from the testenvironment.
     * @return the runtime environment.
     */
    char *password() const; 
    
    /** 
     * Gets the connection properties from the test environment.
     * @return The connection properties.
     */
    const SQLDBC_ConnectProperties& connectproperties() const; 

    /**
     * Gets the test case utility statement instance.
     * @return the utility statement of the test case, or 0.
     */
    SQLDBC_Statement  *statement();

    
    /**
     * Gets the test case utility Fastload command instance.
     * @return the utility Fastload command of the test case, or 0.
     */
    Loader_FastloadCommand* FastloadCommand();


    /**
     * Gets the test case utility Fastload command instance.
     * @return the utility Fastload command of the test case, or 0.
     */
    void releaseFLCommand();



    /**
     * Utility method for the trace to retrieve the hierarchic name.
     */
    inline const char *name() const 
    {
        return m_frame ? m_fullname : m_name;
    }

    SQLDBC_Connection* getSQLDBCConnection() const;

    Loader_Connection* getLoaderConnection() const;

    /**
     * Drop a table. The error is ignored.
     * @param tablename the name of the table to drop.
     */
    void dropTable(const char *tablename);

    /**
     * Executes an SQL statement (using the test case's statement).
     * @param sql the statement
     * @return <code>true</code> if the statement was successfully executed.
     */
    LOADER_Bool execSQL(const char *sql);
    
    /**
     * Determine the size of the order interface packet of the connected 
     * session.
     * @param cbPacketSize [out] The size in bytes for the request packet
     * @return IFR_TRUE if the size was determined successfull.
     */
    //LOADER_Retcode getPacketSize(IFR_Int4 &cbPacketSize);
    
//    inline SAPDBMem_IRawAllocator& getAllocator();
    
protected:
    char                    m_name[1024];
    char                    m_fullname[1024];
    FastloaderTestFrame*    m_frame;
    Loader_FastloadCommand* m_pFLCmd;
    SQLDBC_Statement*       m_pStatement;
//    Loader_Connection*      m_pLoaderConnection;
    LOADER_Bool             m_success;
};


/**
 * Test frame. Groups several test cases together, which may be added
 * with <code>addTestCase</code>. Suggested is only one instance
 * in a program, initialised with the <code>argc</code> and <code>argv</code>.
 */
class FastloaderTestFrame : public FastloaderTestCase
{
public:
    /**
     * Constructor.
     * @param argc the argument count
     * @param argv the argument values
     */
    FastloaderTestFrame(int argc, char**argv);
    
    virtual ~FastloaderTestFrame();
    
    /**
     * Dump small usage info.
     */
    void usage();
    
    /**
     * Returns a pointer to the info output.
     */
    FILE *infostream();

    /**
     * Returns a pointer to the error output.
     */
    FILE *errorstream();

    /**
     * Returns a pointer to the 'protocol' output.
     */
    FILE *protstream();

    /**
     * Gets the status.
     * @return <code>true</code> if the arguments were ok,
     *  resp. the connect succeeded.
     */
    LOADER_Bool good() { return m_good; }
    
    /**
     * Connects to the database.
     */
    void connect();

    SQLDBC_Connection* sqldbcConnection() const { return m_pSQLDBCConnection; }

    Loader_Connection* loaderConnection() const { return m_pLoaderConnection; }

    /**
     * Runs all added tests sequentially.
     */
    LOADER_Bool run();

    /**
     * Disconnects the session by sending a <code>COMMIT WORK
     * RELEASE</code> to the database.
     */
    void disconnect();
    
    /**
     * Adds a test case. 
     * @param tc the test case, which should be on the heap.
     */
    void addTestCase(FastloaderTestCase* tc);

    
    inline char *hostname() const { return m_host; }
    inline char *database() const { return m_database; }
    inline char *username() const { return m_username; }
    inline char *password() const { return m_password; }
    inline const SQLDBC_ConnectProperties& connectproperties() const { return m_properties; }
    
    inline SQLDBC_Environment *environment() const { return m_environment; }

    // internal methods called by the testcase methods
    void info(char *who, char *fmt, va_list ap);
    void error(char *who, char *fmt, va_list ap);
    void prot(char *who, char *fmt, va_list ap);

//    inline SAPDBMem_IRawAllocator& getAllocator() { return m_environment->allocator; }

private:
    /**
     * Initialisation method called by the constructor.
     */
    LOADER_Bool init(int argc, char **argv);
    
    LOADER_Bool m_good;
    FILE    *m_stream;
    char    *m_host;
    char    *m_database;
    char    *m_username;
    char    *m_password;
    LOADER_Bool                     m_packettrace;
    SQLDBC_Environment*             m_environment;
    SQLDBC_ConnectProperties        m_properties;
    SQLDBC_Connection*              m_pSQLDBCConnection;
    Loader*                         m_pLoader;
    Loader_Connection*              m_pLoaderConnection;
    IFRUtil_Vector<FastloaderTestCase*>   m_tc;
    LOADER_Int4                     m_testloopcount;
};


/**
 * Internal template class used for error checking.
 * The template argument <code>C</code> is required to
 * be something that implements an <code>error()</code> method, which
 * returns an <code>IFR_ErrorHndl</code> object to inspect the 
 * status. Use it together with the <code>IFR_errorcheck</code>
 * template method.
 */
template <class C>
class IFR_ErrorCheckClass
{
public:
    inline IFR_ErrorCheckClass(C* ptr, FastloaderTestCase *tc, const char *file=0, int line=0)
    {
        m_ptr=ptr;
        m_tc=tc;
        m_file=file;
        m_line=line;
    }
    
    inline IFR_ErrorCheckClass(const IFR_ErrorCheckClass<C>& copy)
    :m_ptr(copy.m_ptr),
     m_tc(copy.m_tc),
     m_file(copy.m_file),
     m_line(copy.m_line)
    {}

    inline ~IFR_ErrorCheckClass()
    {
        if(m_ptr->error()) {
            if(m_file==0) {
                m_tc->logerror("Error: %d sql state %5s, %s", 
                               m_ptr->error().getErrorCode(),
                               m_ptr->error().getSQLState(),
                               m_ptr->error().getErrorText());
            } else {
                m_tc->logerror("Error: %d sql state %5s, %s in file %s:%d", 
                               m_ptr->error().getErrorCode(),
                               m_ptr->error().getSQLState(),
                               m_ptr->error().getErrorText(),
                               m_file,
                               m_line);
            }
        }
    }

    inline C* operator->()
    {
        return m_ptr;
    }

    inline const C* operator->() const
    {
        return m_ptr;
    }
    
    inline C& operator *()
    {
        return *m_ptr;
    }

    inline const C& operator *() const
    {
        return *m_ptr;
    }
        
protected:
    C*            m_ptr;
    FastloaderTestCase* m_tc;

    const char *  m_file;
    int           m_line;
};

#endif  // FASTLOADERTESTUTILS_H
