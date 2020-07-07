/*



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





*/
#ifdef WIN32
#define ZStringImport
#define OptionsImport
#endif

#include "ContentStorage/ZString.h"
#include "ContentStorage/Options.h"
#include "ContentStorage/TRACE.h"
#include "ContentStorage/SAPCSConHdl.h"

#include <time.h>
#include <stdio.h>

#define DEFAULT_MAX_OPEN_CONN  25 // the default value for m_maxOpenConn. Used if the value MAX_USER_SESSIONS
                                  // can not be retrieved from system tables

#define COMPR_MAX_BLOCK_SIZE   128000

// ******************************************************************************

class DbHdlItem {
public:
    DbHdlItem()
        : compr( COMPR_MAX_BLOCK_SIZE ), compr2( COMPR_MAX_BLOCK_SIZE ),
          decompr(), decompr2()
        { };
    tpa111_ODCompr2    compr;               // used to compress data
    tpa111_ODCompr2    compr2;              // used to compress properties
    tpa111_ODDecompr2  decompr;             // used to decompress data
    tpa111_ODDecompr2  decompr2;            // used to decompress properties
    SQLHENV   henv;
    SQLHDBC   hdbc;
    SQLHSTMT  hstmt[NUMBER_STMT_TYPES];
    bool      prepared;
#ifdef WIN32
    FILETIME  endTransaction;       // end time of transaction              
#else
    time_t    endTransaction;       // end time of transaction
#endif
    bool      hdlError;             // flag indicate invalid handle errors. If hdlError
    // is set, DbHdlItem will be deleted in PutDbHdl()
    bool docIdOnly;
    
    DbHdlItem* next;                
    DbHdlItem* prev;
    int check;                      // check flag, only for tests
}; // class DbHdlItem

// ******************************************************************************

SAPCSConHdl::SAPCSConHdl(Options* options)
    : m_Lock()
{
	m_Options	     = options;
	m_sessionTimeout = 0;
	FreeHdlList      = NULL;
	LatestDbHdl      = NULL;
	DbHdlList        = NULL;
    m_numOpenConn    = 0;
    m_firstConn      = true;
    m_maxOpenConn    = DEFAULT_MAX_OPEN_CONN;
}

// ******************************************************************************

SAPCSConHdl::~SAPCSConHdl()
{
// close all open database connections
	DbDisconnect();
}

// ******************************************************************************

bool SAPCSConHdl::SetMaxOpenConn( long maxOpenConn )
{
	m_maxOpenConn = maxOpenConn; 
	return true;
}

// ******************************************************************************

bool SAPCSConHdl::SetSectionName( const ZString& sectionName)
{
	m_SectionName = CS_CAST(ZString&) sectionName; 
	return true;
}

// ******************************************************************************

bool SAPCSConHdl::SetSessionTimeout(long sessionTimeout)
{
	m_sessionTimeout = sessionTimeout; 
	return true;
}

// ******************************************************************************

#define SHUTDOWN_DELAY 2000 // milliseconds
#define MAX_LOOP_COUNT  20000

bool SAPCSConHdl::DbDisconnect()
{
	DbHdlItem*	dbHdl;

    // give other threads some time to finish
    // threads that request a new DbHdl are blocked!
#ifdef WIN32
    Sleep( SHUTDOWN_DELAY );
#endif
    m_Lock.StartMutex();
    //XTRACE("Begin SAPCSConHdl::DbDisconnect");
#if defined(SAPCSConHdl_XTRY)
	try
	{
#endif
// rollback all open transactions and close the corresponding connections
        int loopCount = 0;
		while (DbHdlList != NULL && loopCount++ < MAX_LOOP_COUNT)
		{
XTRACE("SAPCSConHdl::DbDisconnect: active sessions have to be canceled");
			dbHdl = DbHdlList;
			SQLTransact(dbHdl->henv, dbHdl->hdbc, SQL_ROLLBACK);
			SQLDisconnect(dbHdl->hdbc);
			SQLFreeConnect(dbHdl->hdbc);
			SQLFreeEnv(dbHdl->henv);
			DbHdlList = DbHdlList->next;
			delete dbHdl;
            m_numOpenConn--;
		}; // while
        if (loopCount >= MAX_LOOP_COUNT) {
            XTRACE("DbDisconnect: loop count exceeded 1");
        }; /* if */
		DbHdlList = NULL;
		LatestDbHdl = NULL;
        //XTRACE("starting to disconnect free sessions");
// close all connections not in use
		while (FreeHdlList != NULL && loopCount++ < MAX_LOOP_COUNT)
		{
			dbHdl = FreeHdlList;
			SQLDisconnect(dbHdl->hdbc);
			SQLFreeConnect(dbHdl->hdbc);
			SQLFreeEnv(dbHdl->henv);
			FreeHdlList = FreeHdlList->next;
			delete dbHdl;
            m_numOpenConn--;
		}; // while
        if (loopCount >= MAX_LOOP_COUNT) {
            XTRACE("DbDisconnect: loop count exceeded 2");
        }; /* if */
		FreeHdlList = NULL;
#if defined(SAPCSConHdl_XTRY)
	}
	except(FatalError("DbDisconnect", GetExceptionCode()))
	{
		return false;
	}
#endif
    //XTRACE("End SAPCSConHdl::DbDisconnect");
    m_Lock.EndMutex();
	return true;
}

// ******************************************************************************

// return "LiveCache 7.2" if it exists, else return "LiveCache"
BOOL GetDriverName( SQLHDBC hdbc, ZString &driverName );

#define FIRST_CONN_MAX_TRIES  12
#define CONN_RETRY_WAIT_TIME  5000

bool SAPCSConHdl::GetDbHdl(DbConnection& connection, bool& prepared, ZString& err)
{
	DbHdlItem*	dbHdl = NULL;
	RETCODE		retcode;
	ZString		serverDb = "";
	ZString		serverNode = "";
	ZString		driver = "";
	ZString		userId = "";		
	ZString		password = "";
	ZString		sqlTrace = "";
	ZString		conStrIn = "";
	char*		conStrOut;
	short		maxConStrLen = 300;
	short		conStrLen;
	SQLINTEGER	adaError = 0;
	char		errorMsg[SQL_MAX_MESSAGE_LENGTH];
	short		lenMsg = 0;
	long		cid;
	char		cidStr[18];
	int			i = 1;
	bool		hdlNotFound = true;
#ifdef WIN32
	SYSTEMTIME	syt;
	FILETIME	ft;
#else
    time_t      ft;
#endif
	double		timeDiff;
    int         numTries = 0;
	
    m_Lock.StartMutex();
// first look in freelist for a free connection  
    int loopCount = 0;
	while ((FreeHdlList != NULL) && hdlNotFound
           && loopCount++ < MAX_LOOP_COUNT)
	{
#ifdef WIN32
		GetSystemTime (&syt);
		SystemTimeToFileTime(&syt, &ft);
#else
        time( &ft );
#endif
		dbHdl = FreeHdlList;
		FreeHdlList = FreeHdlList->next;
		if (m_sessionTimeout > 0)
		{
#ifdef WIN32
			timeDiff = ft.dwLowDateTime ;
			timeDiff = (timeDiff - dbHdl->endTransaction.dwLowDateTime) / 10000000;
			timeDiff += (double) (ft.dwHighDateTime - dbHdl->endTransaction.dwHighDateTime) * ULONG_MAX / 10000000;
#else
            timeDiff = ft - dbHdl->endTransaction;
#endif
// compare timeDiff (time between last transaction and new transaction) with DB parameter 
// session timeout. Add 5 seconds to timeDiff for certainty. If timediff is lower as session timeout	 
// then  session is useable else delete dbHdl of the session
			if (timeDiff + 5 < m_sessionTimeout)
				hdlNotFound = false;
			else
			{
				DeleteFreeDbHdl(dbHdl, err);
				dbHdl = NULL;
			}
		}
		else
			hdlNotFound = false;
	}
    if (loopCount >= MAX_LOOP_COUNT) {
        XTRACE("GetDbHdl: loop count exceeded 3");
    }; /* if */
    m_Lock.EndMutex();

// if there is no free connection, instantiate a new one. The connection parameters are in the ContRep profile
    if ((dbHdl == NULL) && (m_numOpenConn < m_maxOpenConn))
	{
		m_Options->GetOption((char*) m_SectionName, "ContentStorageName", serverDb, "");
		m_Options->GetOption((char*) m_SectionName, "ContentStorageHost", serverNode, "");
		m_Options->GetOption((char*) m_SectionName, "driver", driver, "");
		m_Options->GetOption((char*) m_SectionName, "user", userId, "");
		m_Options->GetOption((char*) m_SectionName, "password", password, "");
		m_Options->GetOption((char*) m_SectionName, "sqltrace", sqlTrace, "");

		if (serverDb.GetSize() == 0)
		{
			err.SetBuf("Parameter 'ContentStorageName' not set ");
			return false;
		}

		dbHdl = new DbHdlItem;
		dbHdl->hdlError = false;

		retcode = SQLAllocEnv(&dbHdl->henv);
		if (retcode != SQL_SUCCESS)
		{
			delete dbHdl;
			err.SetBuf("SQLAllocEnv failed ");	
			return false;
		}
		retcode = SQLAllocConnect(dbHdl->henv, &dbHdl->hdbc);
		if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO))
		{
			delete dbHdl;
			err.SetBuf("SQLAllocConnect failed ");
			return false;
		}

        // Map "localhost" to empty string. Because with "localhost"
        // we use TCP/IP for comunication which is much slower than
        // shared memory
		if (serverNode == "localhost")
		{
            serverNode.SetBuf("");
		}
		if (driver.GetSize() == 0) {
			GetDriverName( dbHdl->hdbc, driver );
            m_Options->SetOption( (char*) m_SectionName, "driver", driver );
        };
		if (userId.GetSize() == 0)
			userId = "SAPR3";
		if (password.GetSize() == 0)
			password = "SAP";

		conStrIn.Format("SERVERDB=%s;SERVERNODE=%s;UID=%s;PWD=%s;DRIVER=%s", 
						(char*) serverDb, (char*) serverNode, (char*) userId,
                        (char*) password, (char*) driver);

		if (sqlTrace.GetSize() != 0)
		{
			cid = (long) dbHdl->hdbc;
#ifdef WIN32
			_itoa(cid, cidStr, 10);
#else
            sprintf( cidStr, "%d", cid );
#endif
			conStrIn.Add(";TRACEFILENAME=sqltrace");			
			conStrIn.Add(cidStr);
			conStrIn.Add(".pct");
		}

		retcode = SQLSetConnectOption(dbHdl->hdbc, SQL_AUTOCOMMIT, SQL_AUTOCOMMIT_OFF);
		if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO))
		{
			err.SetBuf("set autocommit_off failed ");
		}
		retcode = SQLSetConnectOption(dbHdl->hdbc, SQL_TXN_ISOLATION, SQL_TXN_READ_COMMITTED);
		if (!(retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO))
		{
			err.Add("set isolation level committed read failed ");
		}		

		conStrOut = new char[maxConStrLen+1];
        numTries = 0;
        retcode = SQLDriverConnect( dbHdl->hdbc,
                                    NULL, (UCHAR*) (char*) conStrIn, SQL_NTS,
                                    (UCHAR*) conStrOut, maxConStrLen,
                                    &conStrLen, SQL_DRIVER_NOPROMPT );
        /* if this is the first connect we may have to wait until the DB is "warm".
         * So try it again after some delay. */
        while ( retcode == SQL_ERROR && m_firstConn && numTries < FIRST_CONN_MAX_TRIES) {
            numTries++;
#ifdef WIN32
            Sleep( CONN_RETRY_WAIT_TIME );
#endif
            retcode = SQLDriverConnect( dbHdl->hdbc,
                                        NULL, (UCHAR*) (char*) conStrIn,
                                        SQL_NTS,
                                        (UCHAR*) conStrOut, maxConStrLen,
                                        &conStrLen, SQL_DRIVER_NOPROMPT );
        }; /* while */
        m_firstConn = false;
		delete conStrOut;
		if ((retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO))
		{
			dbHdl->prepared = false;
           // alloc statements
           for (i = 0; i < NUMBER_STMT_TYPES; i++) {
 		       retcode = SQLAllocStmt(dbHdl->hdbc, &dbHdl->hstmt[i]); 
               if (retcode != SQL_SUCCESS) {
        		   err.SetBuf("SQLAllocStmt failed");
                   return false;
               }; // if
           }; // for
           m_numOpenConn++;
		}
		else
		{
// connect failed
			SQLError(dbHdl->henv, dbHdl->hdbc, SQL_NULL_HSTMT, NULL, &adaError, 
				(UCHAR*) errorMsg, SQL_MAX_MESSAGE_LENGTH -1 , &lenMsg);
			SQLFreeConnect(dbHdl->hdbc);
			SQLFreeEnv(dbHdl->henv);
			delete dbHdl;
			dbHdl = NULL;
            if (adaError != SAPCS_ERR_TO_MANY_USERS) {
                err.Format("SQLConnect failed, %s, Error Code: %d",errorMsg,  adaError);
                return false;
            }; // else
        }; // else
 	} // if (dbHdl == NULL)

    if (dbHdl == NULL) {
        int do_times = 0;
        do {
            m_Lock.StartMutex();
             
            // first look again in freelist for free connection  
            if (FreeHdlList != NULL) {
                dbHdl = FreeHdlList;
                FreeHdlList = FreeHdlList->next;
            }
            m_Lock.EndMutex();
            do_times++;
#ifdef WIN32
            if (dbHdl == NULL)
                Sleep(1000);
#endif
        } while (dbHdl == NULL && do_times <= 10);
        if (dbHdl == NULL) {
            err.SetBuf("too many users connected");
            return false;
        }
    }; // if

	dbHdl->check = CHECK_SUM;


	dbHdl->next = NULL;
	dbHdl->prev = NULL;

    m_Lock.StartMutex();
	if (LatestDbHdl == NULL)
	{
		dbHdl->prev = NULL;
		LatestDbHdl = dbHdl;
	}	
	else
	{
		LatestDbHdl->next = dbHdl;
		dbHdl->prev = LatestDbHdl;
		LatestDbHdl = dbHdl;
	}

	if (DbHdlList == NULL)
	{
		DbHdlList = dbHdl;
	}

    m_Lock.EndMutex();

	connection = (DbConnection) dbHdl;
	prepared = dbHdl->prepared;
	return true;
}

// ******************************************************************************

bool SAPCSConHdl::PutDbHdl(DbConnection connection, ZString& err)
{
#ifdef WIN32
	SYSTEMTIME	syt;
	FILETIME	ft;
#else
    time_t      ft;
#endif
	DbHdlItem*	dbHdl;

	dbHdl = (DbHdlItem*) connection;

	if (dbHdl == NULL)
	{
		err.SetBuf("no connection handle \n");
		return false;
	}
	if (dbHdl->check != CHECK_SUM)
	{
		err.SetBuf("connection handle invalid \n");
		return false;
	}

    m_Lock.StartMutex();
// only for test in single user mode, code has to be removed
/*	m_CntPutHdl++;
	if (m_CntGetHdl != m_CntPutHdl)
	{
		err.Format("m_CntGetHdl: %d  m_CntPutHdl: %d",m_CntGetHdl, m_CntPutHdl);
		ReleaseSemaphore(m_Lock,1,NULL);
		return false;
	}
*/
	if (dbHdl->prev != NULL)
	{
		(dbHdl->prev)->next = dbHdl->next;
	}
	if (dbHdl->next != NULL)
	{
		(dbHdl->next)->prev = dbHdl->prev;
	}
	if (dbHdl == LatestDbHdl)
	{
		LatestDbHdl = dbHdl->prev;
	}
	
	if (dbHdl == DbHdlList)
	{
		DbHdlList = dbHdl->next;
	}

	dbHdl->prev = NULL;
	dbHdl->next = NULL;
	
	if (dbHdl->hdlError == false)
	{
#ifdef WIN32
		GetSystemTime (&syt);
		SystemTimeToFileTime(&syt, &ft);
		dbHdl->endTransaction.dwLowDateTime = ft.dwLowDateTime;
		dbHdl->endTransaction.dwHighDateTime = ft.dwHighDateTime;
#else
        time( &dbHdl->endTransaction );
#endif
		if (FreeHdlList == NULL)
		{		
			FreeHdlList = dbHdl;
		}
		else
		{
			dbHdl->next = FreeHdlList;
			FreeHdlList = dbHdl;
		}
        m_Lock.EndMutex();
	}
	else
	{
        m_Lock.EndMutex();
		SQLDisconnect(dbHdl->hdbc);
		SQLFreeConnect(dbHdl->hdbc);
		SQLFreeEnv(dbHdl->henv);
		delete dbHdl;
        m_numOpenConn--;
	}

	return true;
}

// ******************************************************************************

bool SAPCSConHdl::GetOdbcHdl(DbConnection connection, StmtType stmtType, SQLHENV& henv, SQLHDBC& hdbc, SQLHSTMT& hstmt, 
						   ZString& err)
{

	DbHdlItem* dbHdl;

	dbHdl = (DbHdlItem*) connection;

	if (dbHdl == NULL)
	{
		err.SetBuf("no connection handle \n");
		return false;
	}
	if (dbHdl->check != CHECK_SUM)
	{
		err.SetBuf("connection handle invalid \n");
		return false;
	}

	henv = dbHdl->henv;
	hdbc = dbHdl->hdbc;
	hstmt = dbHdl->hstmt[stmtType];

	return true;
}

// ******************************************************************************

bool SAPCSConHdl::GetOdbcHdl(DbConnection connection, SQLHENV& henv, SQLHDBC& hdbc, ZString& err)
{

	DbHdlItem* dbHdl;

	dbHdl = (DbHdlItem*) connection;

	if (dbHdl == NULL)
	{
		err.SetBuf("no connection handle \n");
		return false;
	}
	if (dbHdl->check != CHECK_SUM)
	{
		err.SetBuf("connection handle invalid \n");
		return false;
	}

	henv = dbHdl->henv;
	hdbc = dbHdl->hdbc;

	return true;
}


// ******************************************************************************

bool SAPCSConHdl::DeleteFreeDbHdl(DbHdlItem* dbHdl, ZString& err)
{

	if (dbHdl == NULL)
	{
		err.SetBuf("no connection handle \n");
		return false;
	}
	if (dbHdl->check != CHECK_SUM)
	{
		XTRACE("DeleteFreeDbHdl: connection handle invalid \n");
		return false;
	}

	SQLDisconnect(dbHdl->hdbc);
	SQLFreeConnect(dbHdl->hdbc);
	SQLFreeEnv(dbHdl->henv);
	delete dbHdl;

	return true;
}


// ******************************************************************************

void SAPCSConHdl::IsPrepared(DbConnection connection)
{
	DbHdlItem* dbHdl;

	dbHdl = (DbHdlItem*) connection;
	dbHdl->prepared = true;
}

// ******************************************************************************

void SAPCSConHdl::SetDocIdOnly(DbConnection connection, bool docIDOnly)
{
	DbHdlItem* dbHdl;

	dbHdl = (DbHdlItem*) connection;
	dbHdl->docIdOnly = docIDOnly;
}

// ******************************************************************************

bool SAPCSConHdl::GetDocIdOnly(DbConnection connection)
{
	DbHdlItem* dbHdl;

	dbHdl = (DbHdlItem*) connection;
	return dbHdl->docIdOnly; 
}

// ******************************************************************************

long SAPCSConHdl::SQLErr(ZString& err, DbConnection connection, StmtType stmtNo)
{
	SQLHSTMT    hstmt;
	SQLHENV		henv;
	SQLHDBC		hdbc;
	char		sqlState[6];
	char		errorMsg[SQL_MAX_MESSAGE_LENGTH];
	short		lenMsg = 0;
	SQLINTEGER	adaError;
	RETCODE		retcode;
	DbHdlItem*		dbHdl;

	GetOdbcHdl(connection, stmtNo, henv, hdbc, hstmt, err);
	dbHdl = (DbHdlItem*) connection;

	if (hstmt == SQL_NULL_HSTMT)
	{
		err.SetBuf("null statement handle");
		return 0;
	}
	else
	{
		retcode = SQLError(henv, hdbc, hstmt, (UCHAR*) sqlState, &adaError, 
			               (UCHAR*) errorMsg, SQL_MAX_MESSAGE_LENGTH -1 , &lenMsg);
		if (retcode == SQL_INVALID_HANDLE)
		{
			dbHdl->hdlError = true;
			err.SetBuf("ODBC handle error");
			
		}
		else
		{
			if (strcmp(sqlState, "01S03") == 0)
				adaError = SAPCS_ERR_NOT_FOUND;
			else
			{
				dbHdl->hdlError = true;
				err.Format("%s ;ODBC State %s; Error Code %d", errorMsg, sqlState, adaError);
			}
		}
		return adaError;
	}
}


// ******************************************************************************

void SAPCSConHdl::FatalError(char* methodName, long exception)
{
	ZString err;

	err = "SAPCSConHdl::";
	err.Add(methodName);
	err.Add(": fatal error:"); 
	switch(exception)
	{
#ifdef WIN32
	case EXCEPTION_ACCESS_VIOLATION:
		err.Add("EXCEPTION_ACCESS_VIOLATION");
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		err.Add("EXCEPTION_DATATYPE_MISALIGNMENT");
		break;
	case EXCEPTION_BREAKPOINT:
		err.Add("EXCEPTION_BREAKPOINT");
		break;
	case EXCEPTION_SINGLE_STEP:
		err.Add("EXCEPTION_SINGLE_STEP");
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		err.Add("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		err.Add("EXCEPTION_FLT_DENORMAL_OPERAND");
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
        err.Add("EXCEPTION_FLT_DIVIDE_BY_ZERO");
		break;
	case EXCEPTION_FLT_INEXACT_RESULT:
        err.Add("EXCEPTION_FLT_INEXACT_RESULT");
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		err.Add("EXCEPTION_FLT_INVALID_OPERATION");
		break;
	case EXCEPTION_FLT_OVERFLOW:
		err.Add("EXCEPTION_FLT_OVERFLOW");
		break;
	case EXCEPTION_FLT_STACK_CHECK:
		err.Add("EXCEPTION_FLT_STACK_CHECK");
		break;
	case EXCEPTION_FLT_UNDERFLOW:
		err.Add("EXCEPTION_FLT_UNDERFLOW");
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		err.Add("EXCEPTION_INT_DIVIDE_BY_ZERO");
        break;
	case EXCEPTION_INT_OVERFLOW:
		err.Add("EXCEPTION_INT_OVERFLOW");
		break;
	case EXCEPTION_PRIV_INSTRUCTION:
		err.Add("EXCEPTION_PRIV_INSTRUCTION");
		break;
	case EXCEPTION_IN_PAGE_ERROR:
		err.Add("EXCEPTION_IN_PAGE_ERROR");
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		err.Add("EXCEPTION_ILLEGAL_INSTRUCTION");
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		err.Add("EXCEPTION_NONCONTINUABLE_EXCEPTION");
		break;
	case EXCEPTION_STACK_OVERFLOW:
		err.Add("EXCEPTION_STACK_OVERFLOW");
		break;
	case EXCEPTION_INVALID_DISPOSITION:
		err.Add("EXCEPTION_INVALID_DISPOSITION");
		break;
	case EXCEPTION_GUARD_PAGE:
		err.Add("EXCEPTION_GUARD_PAGE");
		break;
	case EXCEPTION_INVALID_HANDLE:
		err.Add("EXCEPTION_INVALID_HANDLE");
		break;
#endif /* ifdef WIN32 */
    default:
		err.Add("EXCEPTION");
        break;
	}
XTRACE((char*) err);
}

tpa111_ODCompr2& SAPCSConHdl::GetCompr( DbConnection connection )
{
      DbHdlItem* dbHdl;
      dbHdl = (DbHdlItem*) connection;
      return dbHdl->compr;
}; /* GetCompr */

tpa111_ODDecompr2& SAPCSConHdl::GetDecompr( DbConnection connection )
{
    DbHdlItem* dbHdl;

    dbHdl = (DbHdlItem*) connection;
    return dbHdl->decompr;
}; /* GetDecompr */


tpa111_ODCompr2& SAPCSConHdl::GetCompr2( DbConnection connection )
{
      DbHdlItem* dbHdl;
      dbHdl = (DbHdlItem*) connection;
      return dbHdl->compr2;
}; /* GetCompr */

tpa111_ODDecompr2& SAPCSConHdl::GetDecompr2( DbConnection connection )
{
    DbHdlItem* dbHdl;

    dbHdl = (DbHdlItem*) connection;
    return dbHdl->decompr2;
}; /* GetDecompr */


#define ODBC_LIVECACHE    "LiveCache"
#define ODBC_LIVECACHE_72 "LiveCache 7.2"

BOOL GetDriverName( SQLHDBC hdbc, ZString &driverName )
{
    SQLRETURN   sqlRetcode = SQL_SUCCESS;
    UCHAR       outStr[255];
    SQLSMALLINT outSize = 0;

    // first try "new" 7.2. Driver
    sqlRetcode = SQLDriverConnect( hdbc, NULL,
                                   (UCHAR*) "DRIVER="ODBC_LIVECACHE_72";", SQL_NTS,
                                   outStr, sizeof(outStr),
                                   &outSize,
                                   SQL_DRIVER_NOPROMPT );
    if (sqlRetcode == SQL_ERROR) {
        UCHAR       sqlState[10];
        SQLINTEGER  nativeErr;
        UCHAR       errorMsg[100];
        SQLSMALLINT msgLen;

        sqlRetcode = SQLError( SQL_NULL_HENV, hdbc,
                               SQL_NULL_HSTMT,
                               sqlState,
                               &nativeErr,
                               errorMsg,
                               sizeof(errorMsg),
                               &msgLen );
        if (sqlRetcode != SQL_SUCCESS
            || strcmp( (char*) sqlState, "IM002" ) == 0
            || strcmp( (char*) sqlState, "IM002" ) == 0) {
            // IM002: Datasource name not found. (This is returned by my DM.)
            // IM003: Specified driver could not be loaded. (This should
            // be returned as described in the ODBC manual.)
            // Driver "LiveCache 7.2" does not exist. Use "LiveCache".
            driverName = ODBC_LIVECACHE;
        } else {
            driverName = ODBC_LIVECACHE_72;
        };
    } else {
        // this should never happen
        driverName = ODBC_LIVECACHE_72;
    };

    return TRUE;
}; // GetDriverName
