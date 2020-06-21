/*!================================================================
 module:    IFR_TestUtils.h

 responsible: D039759

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
#ifndef IFR_TESTUTILS_H
#define IFR_TESTUTILS_H

#include <stdio.h>
#include "SAPDB/Interfaces/sapdbifr.h"
#include "SAPDB/Interfaces/tests/Util.h"
#include "SAPDB/Interfaces/tests/TestMain.h"
#include "SAPDB/Interfaces/SQLDBC/SQLDBC_ClientRuntime.h"

class IFR_TestFrame;

/**
 * Single test. Derive from this class and implement the
 * <code>run()</code> method for a new test.  A test should setup
 * itself (drop and create the respective tables etc.) before running.
 */
class IFR_TestCase
{
public:
    /**
     * Creates new test case.
     * @param name name that will appear in the log
     */
    IFR_TestCase(const char *name);
    
    virtual ~IFR_TestCase();

    /**
     * Runs the test. This method does nothing.
     * @return <code>true</code> if the test succeeded. <i>Use
     * the member <code>m_success</code> in derived classes
     * to check the error status.</i>
     */
    virtual IFR_Bool run();
    
    /**
     * Internal method called on insert into test frame.
     * @param fmt format string (printf)
     * @param ... variable arguments (printf)
     */
    void setFrame(IFR_TestFrame *frame);
    
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
    IFR_Connection *connection() const; 

    /**
     * Gets the environment object.
     * @return the runtime environment.
     */
    IFR_Environment *environment() const; 

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
    const IFR_ConnectProperties& connectproperties() const; 

    /**
     * Gets the test case utility statement instance.
     * @return the utility statement of the test case, or 0.
     */
    IFR_Statement  *statement();

    /**
     * Utility method for the trace to retrieve the hierarchic name.
     */
    inline const char *name() const 
    {
        return m_frame ? m_fullname : m_name;
    }

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
    IFR_Bool execSQL(const char *sql);
    
    /**
     * Determine the size of the order interface packet of the connected 
     * session.
     * @param cbPacketSize [out] The size in bytes for the request packet
     * @return IFR_TRUE if the size was determined successfull.
     */
  IFR_Retcode getPacketSize(IFR_Int4 &cbPacketSize);
    
    inline SAPDBMem_IRawAllocator& getAllocator();
    
protected:
    char m_name[1024];
    char m_fullname[1024];
    IFR_TestFrame *m_frame;
    IFR_Statement *m_statement;
    IFR_Bool       m_success;
};


/**
 * Test frame. Groups several test cases together, which may be added
 * with <code>addTestCase</code>. Suggested is only one instance
 * in a program, initialised with the <code>argc</code> and <code>argv</code>.
 */
class IFR_TestFrame
    : public IFR_TestCase
{
public:
    /**
     * Constructor.
     * @param argc the argument count
     * @param argv the argument values
     */
    IFR_TestFrame(int argc, char**argv);
    
    virtual ~IFR_TestFrame();
    
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
    IFR_Bool good() { return m_good; }
    
    /**
     * Connects to the database.
     */
    void connect();

    /**
     * Runs all added tests sequentially.
     */
    IFR_Bool run();

    /**
     * Disconnects the session by sending a <code>COMMIT WORK
     * RELEASE</code> to the database.
     */
    void disconnect();
    
    IFR_Connection *connection() const { return m_connection; }

    /**
     * Adds a test case. 
     * @param tc the test case, which should be on the heap.
     */
    void addTestCase(IFR_TestCase* tc);

    
    inline char *hostname() const { return m_host; }
    inline char *database() const { return m_database; }
    inline char *username() const { return m_username; }
    inline char *password() const { return m_password; }
    inline const IFR_ConnectProperties& connectproperties() const { return m_properties; }
    
    inline IFR_Environment *environment() const { return m_environment; }

    // internal methods called by the testcase methods
    void info(char *who, char *fmt, va_list ap);
    void error(char *who, char *fmt, va_list ap);
    void prot(char *who, char *fmt, va_list ap);

    inline SAPDBMem_IRawAllocator& getAllocator() { return m_environment->allocator; }
private:
    /**
     * Initialisation method called by the constructor.
     */
    IFR_Bool init(int argc, char **argv);
    
    IFR_Bool m_good;
    FILE    *m_stream;
    char    *m_host;
    char    *m_database;
    char    *m_username;
    char    *m_password;
    IFR_Bool m_packettrace;
    IFR_Environment               *m_environment;
    IFR_ConnectProperties         m_properties;
    IFR_Connection                *m_connection;
    IFRUtil_Vector<IFR_TestCase*>  m_tc;
    IFR_Int4                       m_testloopcount;


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
    inline IFR_ErrorCheckClass(C* ptr, IFR_TestCase *tc, const char *file=0, int line=0)
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
    IFR_TestCase* m_tc;

    const char *  m_file;
    int           m_line;
};

inline SAPDBMem_IRawAllocator&
IFR_TestCase::getAllocator() 
{
    return m_frame->getAllocator();
}

/**
 * Template class for negative test (expected error).
 */
template <class C>
class IFR_ExpectedErrorCheckClass
    : public IFR_ErrorCheckClass<C>
{
public:
    /**
     * Constructor.
     * @param ptr pointer to object to be checked (must have <code>error()</code> method.
     * @param tc current test case
     * @param expectederrorcode the error code that is exepected.
     * @param file the file name (use <code>__FILE__</code>)
     * @param line the line number (use <code>__LINE__</code>)
     */
    inline IFR_ExpectedErrorCheckClass(C *ptr, IFR_TestCase* tc, int expectederrorcode, const char *file=0, int line=0)
    :IFR_ErrorCheckClass<C>(ptr, tc, file, line),
     m_expectederrorcode(expectederrorcode),
     m_checkdisabled(false)
    {}


    /**
     * Copy constructor. We need this as only the last
     * instance in a 'copy chain' should actually barf if
     * the error is not set.
     * @param copy instance to copy. In this instance the error check
     *        will be disabled.
     */
    inline IFR_ExpectedErrorCheckClass(const IFR_ExpectedErrorCheckClass& copy)
    :IFR_ErrorCheckClass<C>(copy),
     m_expectederrorcode(copy.m_expectederrorcode),
     m_checkdisabled(false)
    {
        const_cast<IFR_ExpectedErrorCheckClass&>(copy).m_checkdisabled=true;
    }
    
    inline ~IFR_ExpectedErrorCheckClass()
    {
        if(!m_checkdisabled) {
            if(this->m_ptr->error()) {
                if(this->m_ptr->error().getErrorCode() != m_expectederrorcode) {
                    if(this->m_file==0) {
                        this->m_tc->logerror("Unexpected Error: %d sql state %5s, %s, expected was %d.", 
                                       this->m_ptr->error().getErrorCode(),
                                       this->m_ptr->error().getSQLState(),
                                       this->m_ptr->error().getErrorText(),
                                       m_expectederrorcode);
                    } else {
                        this->m_tc->logerror("Unexpected Error: %d sql state %5s, %s in file %s:%d, expected was %d.", 
                                       this->m_ptr->error().getErrorCode(),
                                       this->m_ptr->error().getSQLState(),
                                       this->m_ptr->error().getErrorText(),
                                       this->m_file,
                                       this->m_line,
                                       m_expectederrorcode);
                    }
                } else {
                    this->m_ptr->error().clear(); // otherwise the inherited dtor will jump on it
                }
            } else {
                if(this->m_file==0) {
                    this->m_tc->logerror("Expected Error %d, no error found.", m_expectederrorcode);
                } else {
                    this->m_tc->logerror("Expected Error %d, no error found in file %s:%d.", m_expectederrorcode, this->m_file, this->m_line);   
                }
            }
        }
    }
private:
    int       m_expectederrorcode;
    IFR_Bool  m_checkdisabled;
};


/**
 * Template method for checked execution. The temporary object
 * created by this function is destroyed as soon as the expression
 * containing this call is completely evaluated, hereby checking
 * and reporting any runtime error happened.
 * @param ptr pointer to the handle to check
 * @param tc current test case
 * @param file source file
 * @param line source line.
 */
template <class C> 
inline IFR_ErrorCheckClass<C> 
IFR_errorcheck(C* ptr, IFR_TestCase *tc, const char* file=0, int line=0)
{
    return IFR_ErrorCheckClass<C>(ptr, tc, (char *) file, line);
}

/**
 * Template method for checked execution. The temporary object
 * created by this function is destroyed as soon as the expression
 * containing this call is completely evaluated, hereby checking
 * and reporting any runtime error happened.
 * @param ptr pointer to the handle to check
 * @param tc current test case
 * @param file source file
 * @param line source line.
 */
template <class C>
inline IFR_ExpectedErrorCheckClass<C>
IFR_expectederrorcheck(C* ptr, IFR_TestCase *tc, int expectederrorcode, 
                       const char *file, const int line)
{
    return IFR_ExpectedErrorCheckClass<C>(ptr, tc, expectederrorcode, file, line);
}

/**
 * Simple macro for use only inside a test case.
 * @param x pointer to the handle to check
 */
#define tc_errorcheck(x) IFR_errorcheck(x, this, __FILE__, __LINE__)

/**
 * Simple macro for use only inside a test case.
 * @param x pointer to the handle to check
 * @param exp expected error code after the operation
 */
#define tc_expectederrorcheck(x, exp) IFR_expectederrorcheck(x, this, exp, __FILE__, __LINE__)

/**
 * Simple macro to execute a method / expression and let the result influence
 * the test result. Due to shortcut logic, the test is executed only if the
 * test is not yet messed up.
 * @param x The function / expression to evaluate.
 */
#define tc_try(x) m_success = m_success && (x)
#endif
