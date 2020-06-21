/*
 * @(#) mut.h	2005-05-10

	Header file for the MUT cpp-files, contains several often used functions

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

#define private public

#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <string.h>
#include "heo03.h"
#include "gsp01.h"
#include "SAPDB/Interfaces/SQLDBC/SQLDBC_ClientRuntime.h"
#include "SAPDB/Interfaces/Runtime/IFR_ConnectionItem.h"
#include "SAPDB/Interfaces/Runtime/IFR_Connection.h"
#include "SAPDB/Interfaces/SQLDBC/SQLDBC.h"
#include "jtest/mutsqldbc/allocator.h"

#ifdef   WIN32
#include "SAPDB/RunTime/System/RTESys_MSWindows.h" /*nocheck*/
#define  getpid    GetCurrentProcessId
#define  sleep(x)  Sleep (x * 1000)
#define  STRICMP _stricmp
#else
#define  STRICMP strcasecmp
#endif

#ifdef _WIN32
#define SIGFUNC	(void (__cdecl *)(int))
#else
#define SIGFUNC	(void (*)(int))
#endif

using namespace SQLDBC;

#define  CE(x,y,z,a)   {if ( protocol && ( z == 0 ) ) pe ( x, 0, z, a ) ;\
		    if ( z != 0 ) pe ( x, y, z, a ) ;}
#define  SE(x,y,z,a)   {if ((z != 0) && (z != 100)) pe ( x , y, z, a ) ; }
#define  DE(x,y,z,a)   {pe ( x , y , z , a) ;}
#define  ROLLBERR(z)  ( ( ( z >= 400  ) && ( z <=  800  ) ) \
	|| ( z == -106 ) || ( z == -1000 ) || ( z == -9205 ) || (z == -7067) || (z == -51) )
#define MAXLONGRAW 32760
#define MAXSCHEMAS 50
#define MAXMAXSCHEMAS 400
//for definition of HOSTTYPE long on windows (long=4byte) and linux (long=8byte)
#define MY_HOSTTYPE_LONG ((sizeof(long)>4)?SQLDBC_HOSTTYPE_INT8:SQLDBC_HOSTTYPE_INT4)

static SQLDBC_IRuntime *runtime;
static SQLDBC_Environment *env=0;
static SQLDBC_Connection *conn;
static SQLDBC_Statement *stmt;
static SQLDBC_Retcode returncode;
static SQLDBC_ConnectProperties CP;
static SQLDBC_PreparedStatement *pstmt;
static SQLDBC_PreparedStatement *pstmt2;
static SQLDBC_PreparedStatement *pstmt3;
static SQLDBC_ResultSet *rs;
static SQLDBC_ResultSet *rs2;
#define ERRORCODE error().getErrorCode()
#define ERRORTEXT error().getErrorText()

void	check_params(int myargc, char **myargv);
void	print_params(int myargc, char **myargv);
void	print_f (const char *, int, const char *);
void	prot (const char *, ...);
void	rollback();
void	commit();
void	cwr();
void	close_outputfilehandle();
void	finish();
void	Sqldump(const char *, int, const char *, SQLDBC_Connection*);
bool	FileExist(const char*);
void	is_database_unicode();
void	database_info_own();
SQLDBC_Retcode checkPreparedStatement(SQLDBC_PreparedStatement *, const char *);
SQLDBC_Connection* create_conn(SQLDBC_Environment *);
void	do_connect(SQLDBC_Connection *, char *, char *, char *, char *, SQLDBC_ConnectProperties &);
void	check_memory();
void	get_current_time(char *);
const char*	getProfileCounterName(SQLDBC_ProfileCountValues);
void	getSQLDBCCounter();

void	pe (const char *, int, int, const char *) ;

SQLDBC_Bool use_unicode = SQLDBC_FALSE;

FILE	*fout;
int		pid = 0;
int		cycle = 0;
int		DumpOnLongCheck = 0;
int		protocol = 0;
int		col = 1;

int		check_longs = 0;
int		superusernumber = 0;
int		maxcycle = 1;
int		line = 2;
int		maxlongraw = 19000;
int		rows = 3000;
int		noinit = 0;
int		maxRuntime = 0;
int		use_cluster = 0;
int		use_random = 1;
char	da[40];
char	callfile[40];
char	actfile[15];
char	DBNAME[10];
char	id[40];
char	pw[40];
char	DBM_id[40];
char	DBM_pw[40];
char	DBA_id[40];
char	DBA_pw[40];
char	DBNODE[40];
/* ATTENTION: edit MAXIGNORE, if you edit the call from the pl-script for rendlos.exe with more than two "-ignore"-values */
#define MAXIGNORE 2
int     ignore [MAXIGNORE];

char	DATABASE_SESSIONID[40];
char	DATABASE_APPLICATIONPROCESS[40];
char	DATABASE_USERNAME[40];
char	DATABASE_TASKID[40];
char	DATABASE_UID[40];

int		prepareErrors = 0;

MyAllocator *myAllocator=0;
/*------------------------------------------------------------------------*/
void check_params(int myargc, char **myargv)
/*
	checks the given parameters for the executables, all the same way
*/
{
/* ATTENTION: edit number of local arrays, if you edit the call from the pl-script for rendlos.exe with more than two "-ignore"-values */
	char		ignore1[10]={0};
	char		ignore2[10]={0};

	strcpy(callfile, myargv[0]);
	strcpy(DBNODE, "");

	for ( int argn = 2 ; argn <= myargc ; argn++ )
	{
		if ( strcmp ( myargv [argn-1] , "-noinit" ) == 0 )
		{
			noinit = 1 ;
		}
		if ( strcmp ( myargv [argn-1] , "-checklongrows" ) == 0 )
			check_longs = 1 ;

		if ( strcmp ( myargv [argn-1] , "-superusernumber" ) == 0 )
		{
			superusernumber = atoi ( myargv [argn] ) ;
			argn++ ;
		}
		if ( strcmp ( myargv [argn-1] , "-maxruntime" ) == 0 )
		{
			maxRuntime = atoi ( myargv [argn] ) ;
			argn++ ;
		}
		if ( strcmp ( myargv [argn-1] , "-database" ) == 0 )
		{
			strcpy(DBNAME, myargv [argn]);
			argn++ ;
		}
		if ( strcmp ( myargv [argn-1] , "-user" ) == 0 )
		{
			sscanf ( myargv[argn] , "%[A-Za-z0-9]%*[,]%[A-Za-z0-9]" , id , pw ) ;
			argn++ ;
		}
		if ( strcmp ( myargv [argn-1] , "-dbm" ) == 0 )
		{
			sscanf ( myargv[argn] , "%[A-Za-z0-9]%*[,]%[A-Za-z0-9]" , DBM_id , DBM_pw ) ;
			argn++ ;
		}
		if ( strcmp ( myargv [argn-1] , "-dba" ) == 0 )
		{
			sscanf ( myargv[argn] , "%[A-Za-z0-9]%*[,]%[A-Za-z0-9]" , DBA_id , DBA_pw ) ;
			argn++ ;
		}
		if ( strcmp ( myargv [argn-1] , "-ignore" ) == 0 )
		{
/* ATTENTION: edit number of arguments, if you edit the call from the pl-script for rendlos.exe with more than two "-ignore"-values */
			sscanf ( myargv[argn] , "%[A-Za-z0-9]%*[,]%[A-Za-z0-9]" , ignore1 , ignore2 ) ;
			ignore[0]=atoi(ignore1);
			ignore[1]=atoi(ignore2);
			argn++ ;
		}
		if ( strcmp ( myargv [argn-1] , "-maxcycle" ) == 0 )
		{
			maxcycle = atoi ( myargv [argn] ) ;
			argn++ ;
		}
		if ( strcmp ( myargv [argn-1] , "-rows" ) == 0 )
		{
			rows = atoi ( myargv [argn] ) ;
			argn++ ;
		}
		if ( strcmp ( myargv [argn-1] , "-line" ) == 0 )
		{
			line = atoi ( myargv [argn] ) ;
			argn++ ;
		}
		if ( strcmp ( myargv [argn-1] , "-maxlongraw" ) == 0 )
		{
			maxlongraw = atoi ( myargv [argn] ) ;
			argn++ ;
		}
		if ( strcmp ( myargv [argn-1] , "-use_cluster" ) == 0 )
		{
			use_cluster = atoi ( myargv [argn] ) ;
			argn++ ;
		}
		if ( strcmp ( myargv [argn-1] , "-use_random" ) == 0 )
		{
			use_random = atoi ( myargv [argn] ) ;
			argn++ ;
		}
		if ( strcmp ( myargv [argn-1] , "-node" ) == 0 )
		{
			if (strcmp(myargv[argn], "emptynode") == 0 )		//this option is set in mutrun.pl and means that there is no node set
			{
				strcpy(DBNODE, "");
			}
			else
			{
				strcpy(DBNODE, myargv [argn]);
			}
			argn++ ;
		}
	}
}
/*------------------------------------------------------------------------*/
void print_params(int myargc, char **myargv)
/*
	prints the arguments in the protocol file
*/
{
	char cmdline[1024]={0};
	strcat(cmdline, "-- arguments: ");
	for(int i=1; i<myargc; ++i)
	{
		strcat(cmdline, myargv[i]);
		if(i != myargc -1)
		{
			strcat(cmdline, " ");
		}
	}
	prot("%s\n", cmdline);
}
/*------------------------------------------------------------------------*/
void print_f (const char *s, int errorcode, const char *errortext)
/*
	prints the given text in the protocol file (only in case of errors [from function "pe"] or before sqldump)
*/
{
	get_current_time(da);

	fprintf ( fout , "pid %d , %.19s , cycle %3d , %s :  ERR = %d, TXT = %s\n" ,
				pid , da , cycle , s , errorcode, errortext) ;
}
/*------------------------------------------------------------------------*/
void prot (const char *s1 , ... )
/*
	prints the given text in the protocol file
*/
{
	char	da [40]={0} ;
	time_t	tm ;

	tm = time ( (time_t *) 0) ;
	strncpy ( da , ctime ( &tm ) , 39 ) ;
	da [ 39 ] = 0 ;

 	va_list paramlist;
	va_start(paramlist, s1);
	fprintf ( fout , "pid %d , %.19s , cycle %3d , " , pid , da , cycle ) ;
	vfprintf ( fout , s1 , paramlist ) ;
	va_end(paramlist);
	fflush ( fout ) ;
}
/*------------------------------------------------------------------------*/
void rollback ()
/*
	explicit ROLLBACK-statement for faster use of command in source code
*/
{
	returncode = conn->rollback();
	CE("rollback: ROLLBACK WORK", 1, conn->ERRORCODE, conn->ERRORTEXT);
}
/*------------------------------------------------------------------------*/
void commit ()
/*
	explicit COMMIT-statement for faster use of command in source code
*/
{
	returncode = conn->commit();
	CE("commit: COMMIT WORK", 1, conn->ERRORCODE, conn->ERRORTEXT);
}
/*------------------------------------------------------------------------*/
void cwr()
/*
	CommitWorkRelease
*/
{
	returncode = conn->commitWorkRelease();
	CE("cwr: CommitWorkRelease", 1, conn->ERRORCODE, conn->ERRORTEXT);
}
/*------------------------------------------------------------------------*/
void close_outputfilehandle()
/*
	closes the file handle for the output to the protocol file
*/
{
	if ( fout )
		fclose ( fout ) ;
}
/*------------------------------------------------------------------------*/
void finish()
/*
	finish for the application
*/
{
	cwr();

	getSQLDBCCounter();

	conn->releaseStatement(stmt);

	env->releaseConnection(conn);

	delete env;

	check_memory();

	close_outputfilehandle();

	remove( actfile );
}
/*------------------------------------------------------------------------*/
void sqladump (SQLDBC_Connection *conn ){
	IFR_Connection *ifrconn = conn->m_citem->m_item->getConnection();
    tsp00_ErrTextc cError;
	fprintf(fout, "Error with Client-ConnID: %d\n", (tsp00_Int4) ifrconn->m_connectionid);
	SQLDBC_Statement *dumpStmt = conn->createStatement();
	CE("main: createStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	returncode = dumpStmt->execute("SQLDUMP"); /* to find dumping task in vtrace */
    tsp01_CommErr_Enum err = SqlDBDump (  (tsp00_Int4) ifrconn->m_connectionid,  cError );
    if (err != commErrOk_esp01)
	{
		fprintf(fout, "Error with sqladump: %s\n", cError.asCharp());
		if (ifrconn->m_connectionid == -1)
		{
			fprintf(fout, "Try to open new connection to repeat SqlDBDump()\n");
			conn = create_conn(env);
			do_connect(conn, DBNODE, DBNAME, id, pw, CP);
			dumpStmt = conn->createStatement();
			CE("main: createStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
			returncode = dumpStmt->execute("SQLDUMP"); /* to find dumping task in vtrace */
			ifrconn = conn->m_citem->m_item->getConnection();
			err = SqlDBDump (  (tsp00_Int4) ifrconn->m_connectionid,  cError );
			if (err != commErrOk_esp01)
			{
				fprintf(fout, "Error with second sqladump: %s\n", cError.asCharp());
			}
		}
	}
	conn->releaseStatement(dumpStmt);
	env->releaseConnection(conn);
	delete env;
}
/*------------------------------------------------------------------------*/
SQLDBC_IRuntime* getRuntime(){
	//Every application has to initialize the SQLDBC library by getting a
	//reference to the ClientRuntime and calling the SQLDBC_Environment constructor.
	char errorText[200]={0};

	SQLDBC_IRuntime* runtime = //SQLDBC::GetClientRuntime
		SQLDBC::GetSingleThreadClientRuntime
		(errorText, sizeof(errorText) );
	if (!runtime)
	{
		print_f("Getting instance of the ClientRuntime failed", -42, errorText);
		close_outputfilehandle();
		remove( actfile );
		exit (1);
	}
	return runtime;
}
/*------------------------------------------------------------------------*/
void Sqldump (const char* origin, int errorcode, const char *errortext, SQLDBC_Connection *conn=0)
/*
	stops the database kernel in case of emergency
*/
{
	FILE *fdump = NULL;
	char dumpfile [100]={0} ;
	prot ("** calling sqldump **") ;
	print_f (origin, errorcode, errortext) ;
	sprintf ( dumpfile , "dump.prt" ) ;

	if (!FileExist(dumpfile))
	{
		if ( ( errorcode != -10709 ) && ( errorcode != -10807 ) /* && ( errorcode != 800 ) */ )
		{
			if ( ( fdump = fopen ( dumpfile , "w" ) ) == NULL )
			{
				char outtext [100]={0} ;
				sprintf ( outtext , "Error opening file '%s'\n" , dumpfile ) ;
				perror ( outtext ) ;
				close_outputfilehandle();
				remove( actfile );
				exit (-99) ;
			}

			fprintf(fdump, "The task with the following processID causes the SQLADUMP: %d\n", pid);
			if (!STRICMP("superu", id))
			{
				fprintf(fdump, "This is the superuser connected as %s %d\n", id, superusernumber);
			}
			else
			{
				fprintf(fdump, "This is the user connected as %s\n", id);
			}
			fprintf(fdump, "The origin was: %s\n", origin);
			fprintf(fdump, "The reason was the errorcode: %d,\n", errorcode);
			fprintf(fdump, "with errortext: %s\n", errortext);
			fprintf(fdump, "The DATABASE_TASKID was: %s\n", DATABASE_TASKID);
			fprintf(fdump, "\nsqladump will be called.\n");

			if ( fdump )
				fclose ( fdump ) ;

			if (conn==0)
			{
				//this only possible, because only in check_memory() we call Sqldump without conn, 
				// because before call of check_memory all old things were released
				env = new SQLDBC_Environment (getRuntime());
				conn = create_conn(env);
				do_connect(conn, DBNODE, DBNAME, id, pw, CP);
			}

			sqladump(conn);
		}
	}

	close_outputfilehandle();
	remove( actfile );
	exit (1) ;
}
/*------------------------------------------------------------------------*/
bool FileExist(const char* FileName)
/*
	checks whether a file exists or not
*/
{
	struct stat my_stat;
	return (stat(FileName, &my_stat) == 0);
}
/*------------------------------------------------------------------------*/
void is_database_unicode()
/*
	checks whether the database is unicode or not and writes an output to the prot-file
*/
{
	use_unicode = conn->isUnicodeDatabase();

	if (use_unicode == SQLDBC_TRUE)
	{
		prot("Unicode will be used in the database.\n");
	}
	else
	{
		prot("Unicode will NOT be used in the database.\n");
	}
}
/*------------------------------------------------------------------------*/
void database_info_own()
/*
	gives an output of the database parameters for the current connection to the prot-file
*/
{
	pstmt = conn->createPreparedStatement();
	CE("database_info_own: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

	returncode = checkPreparedStatement(pstmt, "SELECT SESSIONID, APPLICATIONPROCESS, USERNAME, TASKID, UID INTO ?, ?, ?, ?, ? FROM SESSIONS WHERE OWN='YES'");
	CE("database_info_own: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

	SQLDBC_Length indi1=sizeof(DATABASE_SESSIONID), indi2=sizeof(DATABASE_APPLICATIONPROCESS), indi3=sizeof(DATABASE_USERNAME),
		indi4=sizeof(DATABASE_TASKID), indi5=sizeof(DATABASE_UID);

	returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &DATABASE_SESSIONID, &indi1, sizeof(DATABASE_SESSIONID));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in database_info_own (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_ASCII, &DATABASE_APPLICATIONPROCESS, &indi2, sizeof(DATABASE_APPLICATIONPROCESS));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in database_info_own (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_ASCII, &DATABASE_USERNAME, &indi3, sizeof(DATABASE_USERNAME));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in database_info_own (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &DATABASE_TASKID, &indi4, sizeof(DATABASE_TASKID));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in database_info_own (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_ASCII, &DATABASE_UID, &indi5, sizeof(DATABASE_UID));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in database_info_own (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

	returncode = pstmt->execute();
	CE("database_info_own: SELECT ... FROM SESSIONS WHERE OWN='YES'", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

	prot(" SESSIONID = %s, APPLICATIONPROCESS = %s, USERNAME = %s, TASKID = %s, UID = %s\n", DATABASE_SESSIONID, DATABASE_APPLICATIONPROCESS, DATABASE_USERNAME, DATABASE_TASKID, DATABASE_UID);

	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
SQLDBC_Retcode checkPreparedStatement(SQLDBC_PreparedStatement *ps, const char *st)
/*
	retry to prepare a statement until no error 500 - lock request timeout occurs
		that is necessary because in the kernel it is possible that a prepare failed
		e.g. when an insert has a long duration and a create index try to access that exclusiv lock
	a counter requests a sqldump if 500er errors are too much
*/
{
	SQLDBC_Retcode rc;
	const int maxPrepareErrors = 5;

	do
	{
		rc = ps->prepare(st);
		if ((ps->ERRORCODE == 500) || (ps->ERRORCODE == 400))
		{
			prepareErrors = prepareErrors + 1;
			prot("st = %s, rc = %d, ec = %d, prepareErrors = %d\n", st, rc, ps->ERRORCODE, prepareErrors);
		}
	}while((ps->ERRORCODE == 500) || (ps->ERRORCODE == 400));

	if (prepareErrors > maxPrepareErrors)
	{
		prot("More than %d 'lock request timeout' (500) or 'lock collision' (400) occured: SQLDUMP is called\n", maxPrepareErrors);
		Sqldump("checkPreparedStatement()", 500, "Too many lock request timeouts or lock collisions!", conn);
	}

	return rc;
}
/*------------------------------------------------------------------------*/
SQLDBC_Connection* create_conn(SQLDBC_Environment *envi)
/*
	creates connection with given parameters
*/
{
	if (!myAllocator)
	{
		myAllocator = new  MyAllocator(&(getRuntime()->getGlobalAllocator()));
	}
	SQLDBC_Connection *connection;
	connection = envi->createConnection(*myAllocator);
	return connection;
}
/*------------------------------------------------------------------------*/
void do_connect(SQLDBC_Connection *connection, char *node, char *db, char *user, char *pass, SQLDBC_ConnectProperties &prop)
/*
	creates connection with given parameters
*/
{
	returncode = connection->connect(node, db, user, pass, prop);
	CE ("do_connect: CONNECT" , 1, connection->ERRORCODE, connection->ERRORTEXT ) ;
}
/*------------------------------------------------------------------------*/
void check_memory()
/*
	allocator object will be used with create connection to check whether all allocatded objects are deallocated
*/
{
	SQLDBC_Int8 alloc_count = myAllocator->getAllocCount();
	SQLDBC_Int8 alloc_bytes = myAllocator->getAllocBytes();
	SQLDBC_Int8 dealloc_count = myAllocator->getDeAllocCount();
	SQLDBC_Int8 dealloc_bytes = myAllocator->getDeAllocBytes();

	prot("Checking for memory leaks:\n");
#ifdef WIN32
	prot("%I64d allocated (%I64d Bytes) - %I64d deallocated (%I64d Bytes)\n", alloc_count, alloc_bytes, dealloc_count, dealloc_bytes);
#else
#ifndef BIT64
	prot("%lld allocated (%lld Bytes) - %lld deallocated (%lld Bytes)\n", alloc_count, alloc_bytes, dealloc_count, dealloc_bytes);
#else
	prot("%ld allocated (%ld Bytes) - %ld deallocated (%ld Bytes)\n", alloc_count, alloc_bytes, dealloc_count, dealloc_bytes);
#endif
#endif

	if (alloc_count != dealloc_count || alloc_bytes != dealloc_bytes)
	{
#ifdef WIN32
		prot("ERROR: %I64d memoryleaks found (%I64d Bytes)\n", alloc_count-dealloc_count, alloc_bytes-dealloc_bytes);
#else
#ifndef BIT64
		prot("ERROR: %lld memoryleaks found (%lld Bytes)\n", alloc_count-dealloc_count, alloc_bytes-dealloc_bytes);
#else
		prot("ERROR: %ld memoryleaks found (%ld Bytes)\n", alloc_count-dealloc_count, alloc_bytes-dealloc_bytes);
#endif
#endif
		Sqldump("check_memory()", 12345, "Memory leaks found!");
	}
	else
	{
		prot("No memory leaks found.\n");
	}
}
/*------------------------------------------------------------------------*/
void get_current_time(char *da)
/*
	get current time
*/
{
	time_t tm ;
	tm = time ( (time_t *) 0) ;
	strncpy(da , ctime ( &tm ) , 39) ;
	da[39] = 0 ;
}
/*------------------------------------------------------------------------*/
const char* getProfileCounterName(SQLDBC_ProfileCountValues value)
/*
	gets the name for the profilCounter, unfortunately there's no other possibility
	[before you need a 'dirty' cast from loop counter to SQLDBC_ProfileCountValues]
*/
{
	switch(value)
	{
		case (SQLDBC_ALLOCATECOUNT              ): { return "ALLOCATECOUNT           ";}
		case (SQLDBC_DEALLOCATECOUNT            ): { return "DEALLOCATECOUNT         ";}
		case (SQLDBC_CONNECTIONCOUNT            ): { return "CONNECTIONCOUNT         ";}
		case (SQLDBC_COMMITCOUNT                ): { return "COMMITCOUNT             ";}
		case (SQLDBC_ROLLBACKCOUNT              ): { return "ROLLBACKCOUNT           ";}
		case (SQLDBC_SETISOLATIONCOUNT          ): { return "SETISOLATIONCOUNT       ";}
		case (SQLDBC_SENDCOUNT                  ): { return "SENDCOUNT               ";}
		case (SQLDBC_SENDSIZE                   ): { return "SENDSIZE                ";}
		case (SQLDBC_RECEIVESIZE                ): { return "RECEIVESIZE             ";}
		case (SQLDBC_CANCELCOUNT                ): { return "CANCELCOUNT             ";}
		case (SQLDBC_STATEMENTCOUNT             ): { return "STATEMENTCOUNT          ";}
		case (SQLDBC_PREPAREDSTATEMENTCOUNT     ): { return "PREPAREDSTATEMENTCOUNT  ";}
		case (SQLDBC_NOTSELECTFETCHOPTIMIZED    ): { return "NOTSELECTFETCHOPTIMIZED ";}
		case (SQLDBC_DROPPARSEIDCOUNT           ): { return "DROPPARSEIDCOUNT        ";}
		case (SQLDBC_DROPCURSORCOUNT            ): { return "DROPCURSORCOUNT         ";}
		case (SQLDBC_DROPLONGDESCCOUNT          ): { return "DROPLONGDESCCOUNT       ";}
		case (SQLDBC_PREPARECOUNT               ): { return "PREPARECOUNT            ";}
		case (SQLDBC_PARSEINFOHITCOUNT          ): { return "PARSEINFOHITCOUNT       ";}
		case (SQLDBC_PARSINFOMISSCOUNT          ): { return "PARSINFOMISSCOUNT       ";}
		case (SQLDBC_PARSEAGAINCOUNT            ): { return "PARSEAGAINCOUNT         ";}
		case (SQLDBC_PARSEINFOSQUEEZECOUNT      ): { return "PARSEINFOSQUEEZECOUNT   ";}
		case (SQLDBC_EXECUTECOUNT               ): { return "EXECUTECOUNT            ";}
		case (SQLDBC_INSERTCOUNT                ): { return "INSERTCOUNT             ";}
		case (SQLDBC_UPDATECOUNT                ): { return "UPDATECOUNT             ";}
		case (SQLDBC_SELECTCOUNT                ): { return "SELECTCOUNT             ";}
		case (SQLDBC_CALLDBPROCEDURECOUNT       ): { return "CALLDBPROCEDURECOUNT    ";}
		case (SQLDBC_DELETECOUNT                ): { return "DELETECOUNT             ";}
		case (SQLDBC_FETCHCOUNT                 ): { return "FETCHCOUNT              ";}
		case (SQLDBC_SQLERRORCOUNT              ): { return "SQLERRORCOUNT           ";}
		case (SQLDBC_ROWNOTFOUNDCOUNT           ): { return "ROWNOTFOUNDCOUNT        ";}
		case (SQLDBC_FETCHROWCOUNT              ): { return "FETCHROWCOUNT           ";}
		case (SQLDBC_DATAREADSIZE               ): { return "DATAREADSIZE            ";}
		case (SQLDBC_DATAWRITESIZE              ): { return "DATAWRITESIZE           ";}
		case (SQLDBC_LONGDATAREADSIZE           ): { return "LONGDATAREADSIZE        ";}
		case (SQLDBC_LONGDATAWRITESIZE          ): { return "LONGDATAWRITESIZE       ";}
  }
  return "UNKNOWN PROFILE COUNTER";
}
/*------------------------------------------------------------------------*/
void getSQLDBCCounter()
/*
	gets all collected SQLDBC counter values from the Environment Profile
		and write them to a specified file
	[you need a dirty cast from loop counter to SQLDBC_ProfileCountValues before]
*/
{
	SQLDBC_Profile* prof = env->getProfile();
	prof->collectCounters();
	SQLDBC_UInt8 profVal = 0;

	FILE *fcounter = NULL;
	char counterfile [100]={0} ;
	sprintf ( counterfile , "pid%d_SQLDBCcounter.prt", pid ) ;

	if ( ( fcounter = fopen ( counterfile , "w" ) ) == NULL )
	{
		char outtext [100]={0} ;
		sprintf ( outtext , "Error opening file '%s'\n" , counterfile ) ;
		perror ( outtext ) ;
		exit (-99) ;
	}

	fprintf(fcounter, "These are all counters collected from SQLDBC\n");
	for (int i=0; i < SQLDBC_PROFILE_MAX; i++)
	{
		profVal = prof->getCounter((SQLDBC_ProfileCountValues)i);
#ifdef WIN32
		fprintf(fcounter, "%s= %I64d\n", getProfileCounterName((SQLDBC_ProfileCountValues)i), profVal);
#else
#ifndef BIT64
		fprintf(fcounter, "%s= %lld\n", getProfileCounterName((SQLDBC_ProfileCountValues)i), profVal);
#else
		fprintf(fcounter, "%s= %ld\n", getProfileCounterName((SQLDBC_ProfileCountValues)i), profVal);
#endif
#endif
	}

	if ( fcounter )
		fclose ( fcounter ) ;
}
