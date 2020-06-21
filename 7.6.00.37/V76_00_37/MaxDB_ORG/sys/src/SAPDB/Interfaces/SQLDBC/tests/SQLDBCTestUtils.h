/*!================================================================
 module:    SQLDBC_TestUtils.h

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
#ifndef SQLDBC_TESTUTILS_H
#define SQLDBC_TESTUTILS_H

#include <stdio.h>
#include <stdarg.h>
#include "SAPDB/Interfaces/SQLDBC/SQLDBC.h"
#include "SAPDB/Container/Container_Vector.hpp"
#include "SAPDB/Interfaces/SQLDBC/tests/SQLDBCTestMain.h"

using namespace SQLDBC;

class SQLDBC_TestFrame;

typedef unsigned short UCS2CHAR;

/**
 * Single test. Derive from this class and implement the
 * <code>run()</code> method for a new test.  A test should setup
 * itself (drop and create the respective tables etc.) before running.
 */
class SQLDBC_TestCase
{
public:
    /**
     * Creates new test case.
     * @param name name that will appear in the log
     */
    SQLDBC_TestCase(const char *name);
    
    virtual ~SQLDBC_TestCase();

    /**
     * Init the test. This method does nothing. Applications may
     * implement this to do startup initialisations.
     * @return <code>true</code> if the test succeeded. <i>Use
     * the member <code>m_success</code> in derived classes
     * to check the error status.</i>
     */
    virtual SQLDBC_Bool prepare();
    
    /**
     * Runs the test. This method does nothing.
     * @return <code>true</code> if the test succeeded. <i>Use
     * the member <code>m_success</code> in derived classes
     * to check the error status.</i>
     */
    virtual SQLDBC_Bool run();
    
    /**
     * Release the test. This method does nothing. Applications may
     * implement this free resources like drop table etc.
     * @return <code>true</code> if the test succeeded. <i>Use
     * the member <code>m_success</code> in derived classes
     * to check the error status.</i>
     */
    virtual SQLDBC_Bool release();
    
    /**
     * Internal method called on insert into test frame.
     * @param fmt format string (printf)
     * @param ... variable arguments (printf)
     */
    void setFrame(SQLDBC_TestFrame *frame);
    
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
    SQLDBC_Connection *connection() const; 

    /**
     * Gets the environment object.
     * @return the runtime environment.
     */
    SQLDBC_Environment *environment() const; 

    /**
     * Gets the client runtime object.
     * @return the runtime environment.
     */
    SQLDBC_IRuntime *runtime() const; 

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
    SQLDBC_Bool execSQL(const char *sql);
    
    /**
     * Sets the last error in the test frame.
     */
    void setLastError(SQLDBC_Int4 errorcode, const char *errormessage);
    
    inline SQLDBC_Bool success()
    {
        m_success = true;
        return true;
    }

    inline SQLDBC_Bool failure()
    {
        m_success = false;
        return false;
    }

    inline SQLDBC_Bool& result()
    {
        return m_success;
    }
    
protected:
    char m_name[1024];
    char m_fullname[1024];
    SQLDBC_TestFrame *m_frame;
    SQLDBC_Statement *m_statement;
    SQLDBC_Bool       m_success;
};


/**
 * Test frame. Groups several test cases together, which may be added
 * with <code>addTestCase</code>. Suggested is only one instance
 * in a program, initialised with the <code>argc</code> and <code>argv</code>.
 */
class SQLDBC_TestFrame
    : public SQLDBC_TestCase
{
public:
    /**
     * Constructor.
     * @param argc the argument count
     * @param argv the argument values
     */
    SQLDBC_TestFrame(int argc, char**argv);
    
    /**
     * Constructor.
     * @param argc the argument count
     * @param argv the argument values
     * @param customRuntime A custom SQLDBC runtime.
     * @param silent Whether errors shall be logged.
     */
    SQLDBC_TestFrame(int argc, char**argv, SQLDBC_IRuntime* customRuntime, SQLDBC_Bool silent=false);

    /**
     * Destructor. Mostly called at the end of the program.
     */
    virtual ~SQLDBC_TestFrame();
    
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
    SQLDBC_Bool good() { return m_good; }
    
    /**
     * Connects to the database.
     */
    void connect();

    /**
     * Runs all added tests sequentially.
     */
    SQLDBC_Bool run();

    /**
     * Disconnects the session by sending a <code>COMMIT WORK
     * RELEASE</code> to the database.
     */
    void disconnect();
    
    /**
     * Retrieves the connection.
     */
    SQLDBC_Connection *connection() const { return m_connection; }

    SQLDBC_Int4 getLastError() { return m_lasterrorcode; }

    const char *getLastErrorMessage() { return m_lasterrormessage; }

    void setLastError(SQLDBC_Int4 errorcode, const char *errormessage);
    
    /**
     * Adds a test case. 
     * @param tc the test case, which should be on the heap.
     */
    void addTestCase(SQLDBC_TestCase* tc);

    
    inline char *hostname() const { return m_host; }
    inline char *database() const { return m_database; }
    inline char *username() const { return m_username; }
    inline char *password() const { return m_password; }
    inline char *lockfile() const { return m_lockfile; }
    inline const SQLDBC_ConnectProperties& connectproperties() const { return m_properties; }
    
    inline SQLDBC_Environment *environment() const { return m_environment; }
    inline SQLDBC_IRuntime *runtime() const { return m_runtime; }

    // internal methods called by the testcase methods
    void info(char *who, char *fmt, va_list ap);
    void error(char *who, char *fmt, va_list ap);
    void prot(char *who, char *fmt, va_list ap);
    
    inline void setSilent(SQLDBC_Bool silent)
    {
        m_silent=silent;
    }
    
    void waitForRmLockfile();
    
private:
    /**
     * Initialisation method called by the constructor.
     */
    SQLDBC_Bool init(int argc, char **argv);

    SQLDBC_Bool m_good;
    FILE    *m_stream;
    char    *m_host;
    char    *m_database;
    char    *m_username;
    char    *m_password;
    SQLDBC_ConnectProperties m_properties;
   
    SQLDBC_Environment              *m_environment;
    SQLDBC_Connection               *m_connection;
    SQLDBC_IRuntime                 *m_runtime;
    Container_Vector<SQLDBC_TestCase*> m_tc;
    
    SQLDBC_Int4 m_lasterrorcode;
    char        m_lasterrormessage[512];
    SQLDBC_Bool m_silent;
    SQLDBC_Int4 m_testloopcount;
    SQLDBC_Int4 m_testlooptime;
  /* member for benchmark */
    char    *m_lockfile;
    SQLDBC_Int4 m_userId;
};


/**
 * Internal template class used for error checking.
 * The template argument <code>C</code> is required to
 * be something that implements an <code>error()</code> method, which
 * returns an <code>SQLDBC_ErrorHndl</code> object to inspect the 
 * status. Use it together with the <code>SQLDBC_errorcheck</code>
 * template method.
 */
template <class C>
class SQLDBC_ErrorCheckClass
{
public:
    inline SQLDBC_ErrorCheckClass(C* ptr, SQLDBC_TestCase *tc, SQLDBC_Retcode *rc=0, const char *file=0, int line=0)
    {
        m_ptr=ptr;
        m_tc=tc;
        m_file=file;
        m_line=line;
        m_rcptr = rc;
    }
    
    inline SQLDBC_ErrorCheckClass(const SQLDBC_ErrorCheckClass<C>& copy)
    :m_ptr(copy.m_ptr),
     m_tc(copy.m_tc),
     m_file(copy.m_file),
     m_line(copy.m_line),
     m_rcptr(copy.m_rcptr)
    {}

    inline ~SQLDBC_ErrorCheckClass()
    {
      if(m_rcptr) {
        switch(*m_rcptr) {
        case (SQLDBC_NOT_OK):
        case (SQLDBC_OVERFLOW): {
          if (!m_ptr->error()) {
            if (m_file)
              m_tc->logerror("Error: Returncode was (%d) but no error found in file %s:%d.", *m_rcptr, m_file, m_line);
            else
              m_tc->logerror("Error: Returncode was (%d) but no error found.", *m_rcptr);
          }          
          break;
        }
        case (SQLDBC_NO_DATA_FOUND) : {
          if (m_ptr->error()) {
            SQLDBC_Int4 sqlcode = 
            m_ptr->error().getErrorCode();
            if (sqlcode != 100) 
            if (m_file)
              m_tc->logerror("Error: Returncode was (%d) but getErrorCode wasn set to (%d) in file %s:%d.", *m_rcptr, sqlcode, m_file, m_line);
            else
              m_tc->logerror("Error: Returncode was (%d) but getErrorCode wasn set to (%d).", *m_rcptr, sqlcode);
          }
          break;
        }
        case (SQLDBC_OK) :
        case (SQLDBC_NEED_DATA) :
        case (SQLDBC_DATA_TRUNC) :{
            if (m_ptr->error()) {
            if (m_file)
              m_tc->logerror("Error: An error was found but returncode was (%d) in file %s:%d.", *m_rcptr, m_file, m_line);
            else
              m_tc->logerror("Error: An error was found but returncode was (%d).", *m_rcptr);
          }
          break;
        }
        case (SQLDBC_INVALID_OBJECT) : {
            if(m_ptr == 0) {
                m_tc->logerror("Error: Invalid object found %s:%d.", m_file, m_line);
            }
            if (m_ptr->error()) {
                if (m_file) {
                    m_tc->logerror("Error: An error was found but returncode was (%d) in file %s:%d.", *m_rcptr, m_file, m_line);
                } else {
                    m_tc->logerror("Error: An error was found but returncode was (%d).", *m_rcptr);
                }
            }
            break;
        }
        default: {
          if (m_file)
            m_tc->logerror("Error: Unknown returncode (%d) in file %s:%d.", *m_rcptr, m_file, m_line);
          else
            m_tc->logerror("Error: Unknown returncode (%d).", *m_rcptr);
          break;
        }
        } // switch
      }
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
    SQLDBC_TestCase* m_tc;

    const char *  m_file;
    int           m_line;
    SQLDBC_Retcode *m_rcptr;
};

/**
 * Template class for negative test (expected error).
 */
template <class C>
class SQLDBC_ExpectedErrorCheckClass
    : public SQLDBC_ErrorCheckClass<C>
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
    inline SQLDBC_ExpectedErrorCheckClass(C *ptr, SQLDBC_TestCase* tc, int expectederrorcode, SQLDBC_Retcode *rc=0, const char *file=0, int line=0)
    :SQLDBC_ErrorCheckClass<C>(ptr, tc, rc, file, line),
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
    inline SQLDBC_ExpectedErrorCheckClass(const SQLDBC_ExpectedErrorCheckClass& copy)
    :SQLDBC_ErrorCheckClass<C>(copy),
     m_expectederrorcode(copy.m_expectederrorcode),
     m_checkdisabled(false)
    {
        const_cast<SQLDBC_ExpectedErrorCheckClass&>(copy).m_checkdisabled=true;
    }
    
    inline ~SQLDBC_ExpectedErrorCheckClass()
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
                    this->m_ptr->clearError(); // otherwise the inherited dtor will jump on it
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
    SQLDBC_Bool  m_checkdisabled;
};


/**
 * Template method for checked execution. The temporary object
 * created by this function is destroyed as soon as the expression
 * containing this call is completely evaluated, hereby checking
 * and reporting any runtime error happened.
 * @param ptr pointer to the handle to check
 * @param tc current test case
 * @param rc pointer to the returncode of the called function
 * @param file source file
 * @param line source line.
 */
template <class C> 
inline SQLDBC_ErrorCheckClass<C> 
SQLDBC_errorcheck(C* ptr, SQLDBC_TestCase *tc, SQLDBC_Retcode *rc=0, const char* file=0, int line=0)
{
    return SQLDBC_ErrorCheckClass<C>(ptr, tc, rc, (char *) file, line);
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
inline SQLDBC_ExpectedErrorCheckClass<C>
SQLDBC_expectederrorcheck(C* ptr, SQLDBC_TestCase *tc, int expectederrorcode, 
                       SQLDBC_Retcode *rc, const char *file, const int line)
{
    return SQLDBC_ExpectedErrorCheckClass<C>(ptr, tc, expectederrorcode, rc, file, line);
}

/**
 * Simple macro for use only inside a test case.
 * @param x pointer to the handle to check
 */
#define tc_errorcheckrc(rc, x) SQLDBC_errorcheck(x, this, &rc, __FILE__, __LINE__),rc=x

/**
 * Simple macro for use only inside a test case.
 * @param x pointer to the handle to check
 */
#define tc_errorcheck(x) SQLDBC_errorcheck(x, this, 0, __FILE__, __LINE__)

/**
 * Simple macro for use only inside a test case.
 * @param x pointer to the handle to check
 * @param exp expected error code after the operation
 */
#define tc_expectederrorcheck(x, exp) SQLDBC_expectederrorcheck(x, this, exp, 0,__FILE__, __LINE__)

/**
 * Simple macro to execute a method / expression and let the result influence
 * the test result. Due to shortcut logic, the test is executed only if the
 * test is not yet messed up.
 * @param x The function / expression to evaluate.
 */
#define tc_try(x) m_success = m_success && (x)

void Util_PrintError(const char *str, const SQLDBC_ErrorHndl *err);
void Util_SPrintError(char *buf, const char *str, const SQLDBC_ErrorHndl *err);
/*!
  Function:     convertToUCS2

  see also: 
  
  Description:  Converts Ascii to UCS2
    
  Arguments:
    szTCHAR     [out] output buffer (TCHAR)
    maxLenTCHAR [in]  maximum length of szTCHAR
    szStr       [in]  null terminated input buffer

  return value: error code
    <UL>
        <LI>zero    : Okay
        <LI>not zero: error occurs
    </UL>
 */
int convertToUCS2(UCS2CHAR *szTCHAR, int maxLenTCHAR, char *szStr);

/*!
  Function:     convertToUCS2

  see also: 
  
  Description:  Converts Ascii to UCS2
    
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
int convertToUCS2(UCS2CHAR *szTCHAR, int maxLenTCHAR, SQLDBC_Bool swapped, char *szStr);

/*!
  Function:     convertToASCII

  see also: 
  
  Description:  Converts TCHAR to ASCII depending on compile switch -G unicode. 
  If cbLenszStr is null a UCS2strlen will be used to determine length of szStr.
    
  Arguments:
    szCHAR     [out] output buffer (CHAR)
    maxLenCHAR [in]  maximum length of szCHAR
    swapped    [in]  set to true if the byte order of the output buffer 
                     should be in swapped byte order
    szStr      [in]  null terminated input buffer (TCHAR)
    cbLenszStr [in]  length of szStr

  return value: error code
    <UL>
        <LI>zero    : Okay
        <LI>not zero: error occurs
    </UL>
 */
int convertUCS2ToASCII(char *szCHAR, int maxLenCHAR, UCS2CHAR *szStr, int cbLenszStr);

int convertUCS2ToASCII(char *szCHAR, int maxLenCHAR, SQLDBC_Bool swapped, UCS2CHAR *szStr, int cbLenszStr);

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
int convertToUTF8(char *szCHAR, int maxLenCHAR, char *szStr);

/*!
  Function:     convertUTF8ToASCII

  see also: 
  
  Description:  Converts CHAR to ASCII
    
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
int convertUTF8ToASCII(char *szCHAR, int maxLenCHAR, char *szStr, int cbLenszStr);


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
SQLDBC_Retcode HostTypeToStr(char *szCHAR, SQLDBC_Length maxLenCHAR, SQLDBC_HostType HostType);

#endif
