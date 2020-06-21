/*
 * @(#) iendlos.cpc	2005-03-04

	Initialization of users A1 to A6, runs 6 times (one time for each user)

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

#include "jtest/mutsqldbc/mut.h"

#define  DBPROCS_IMPLEMENTED

#ifdef   WIN32
#include "jtest/mutsqldbc/flushbuf.h" /*nocheck*/
#endif

#undef   CE
#define  CE(x,y,z,a)   { if (z != 0 ) pe ( x, y, z, a ) ; }

#undef   ROLLBERR
#define  ROLLBERR(z)  ( ( ( z >= 400 ) && ( z <= 800 ) ) || \
		   ( z == -1 ) || ( z == -106 ) || ( z == -1000 ) )

void create_table_tab1 () ;
void init_table_tab1m () ;
void create_table_tab_small () ;
void create_table_tab2 () ;
void create_view_tabv1 () ;
void init_tab1 () ;
void init_tab2 () ;

void CreateSqlDbProcs () ;
int rollberr (int, int);
void movep (int, int, const char *, ...);

typedef char    c20 [20] ;
typedef char    c100 [100] ;

long    k1 , k2 , k3 ;
char    c1 [20] , c2[20] , c3[20] ;
long    i1 , i2 , i3 , i4 , i5 , i6 ;

#ifdef _WIN32
char    buf [256] ;
#else
FILE    *fo = NULL;
#endif

int     rolled_back = 0 ;

int main (int argc , char **argv)
{
	char    head_line [80]={0};

#ifndef _WIN32
    fo = fopen ( "/dev/tty" , "w" ) ;
    if ( fo )
      if ( ! isatty(fileno(fo)) )
      {
        fclose(fo);
        fo = NULL ;
      }
#endif
	pid = getpid () ;
	srand(pid);

	check_params(argc, argv);
	sprintf(actfile, "%s%s.act", callfile, id);
	{
		char filename [100]={0};
		sprintf ( filename , "iendlos%s.prot" , id ) ;

		if ( ( fout = fopen ( filename , "w" ) ) == NULL )
		{
			char outtext [100]={0};
			sprintf ( outtext , "Error opening file '%s'\n" , filename ) ;
			perror ( outtext ) ;
			exit (-99) ;
		}
	}

	prot ("===== User = %s =====\n" , id ) ;
	prot ("===== %s with pid = %d =====\n", callfile, pid);
	print_params(argc, argv);
	if (!STRICMP("", DBNODE))
	{
		sprintf ( head_line, "===== DBNODE = '%s', DBNAME = '%s' =====\n" , DBNODE, DBNAME) ;
	}
	else
	{
		sprintf ( head_line, "===== DBNAME = '%s' =====\n" , DBNAME) ;
	}
    prot ( head_line, "" ) ;

	movep (line-1 , col , "Initialising '%s'" , id ) ;

	env = new  SQLDBC_Environment (getRuntime());
	conn = create_conn(env);

	//the 'CHOPBLANK' property removes blanks that will be automatically returned for varchar types;
	//	otherwise every returned varchar value has the full length defined with create table and every
	//	comparison will failed
	CP.setProperty("CHOPBLANKS", "1");
	CP.setProperty("ISOLATIONLEVEL", "0");
	CP.setProperty("SQL", "TRUE");
#ifndef _WIN32
	CP.setProperty("FILENAME", "./pid%p.pct");
#else
	CP.setProperty("FILENAME", ".\\pid%p.pct");
#endif
	CP.setProperty("FILESIZE", "25000000");	//25.000.000 = 25 MB
	env->setTraceOptions(CP);

	movep ( line , col , "CONNECT , ERR = %d" , conn->ERRORCODE ) ;
	do_connect(conn, DBNODE, DBNAME, id, pw, CP);

	is_database_unicode();

	stmt = conn->createStatement();	
	CE("main: createStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

	database_info_own();

	do
	{
		movep ( line , col , "DROP TABLE TAB1" ) ;
		returncode = stmt->execute("DROP TABLE TAB1");
		CE("main: DROP TABLE TAB1", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (2, stmt->ERRORCODE) ) ;

	if ( stmt->ERRORCODE != SQLDBC_OK )
	{
		if ( stmt->ERRORCODE != -4004 )
			CE ("main: DROP TABLE TAB1",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;
	}
	else
		commit();

	do
	{
		movep ( line , col , "DROP TABLE TAB2" ) ;
		returncode = stmt->execute("DROP TABLE TAB2");
		CE("main: DROP TABLE TAB2", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (2, stmt->ERRORCODE) ) ;

	if ( stmt->ERRORCODE != SQLDBC_OK )
	{
		if ( stmt->ERRORCODE != -4004 )
			CE ("main: DROP TABLE TAB2",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;
	}
	else
		commit();

	do
	{
		movep ( line , col , "DROP TABLE TABSMALL" ) ;
		returncode = stmt->execute("DROP TABLE TABSMALL");
		CE("main: DROP TABLE TABSMALL", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (2, stmt->ERRORCODE) ) ;

	if ( stmt->ERRORCODE != SQLDBC_OK )
	{
		if ( stmt->ERRORCODE != -4004 )
			CE ("main: DROP TABLE TABSMALL",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;
	}
	else
		commit();

	do
	{
		movep ( line , col , "DROP SYNONYM PUBVIEW" ) ;
		returncode = stmt->execute("DROP SYNONYM PUBVIEW");
		CE("main: DROP SYNONYM PUBVIEW", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (1, stmt->ERRORCODE) ) ;

	if ( stmt->ERRORCODE != SQLDBC_OK )
	{
		if ( stmt->ERRORCODE != -4007 )
			CE ("main: DROP SYNONYM PUBVIEW",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;
	}
	else
		commit();

	create_table_tab1 () ;

	init_table_tab1m () ;

	create_table_tab2 () ;

	create_view_tabv1 () ;

	create_table_tab_small () ;

	do
	{
		returncode = stmt->execute("CREATE SYNONYM PUBVIEW FOR SUPERU.TABV1");
		CE("main: CREATE SYNONYM PUBVIEW FOR SUPERU.TABV1", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	    if ( stmt->ERRORCODE == 600 )
	    {
			rollback();
			movep (line,col,"CREATE SYNONYM ERR 600 : before sleep") ;
			sleep ( 30 + pid % 30 ) ;
			movep (line,col,"CREATE SYNONYM ERR 600 : after  sleep") ;
	    }
	} while ( rollberr (1, stmt->ERRORCODE) ) ;

	CE ("main: CREATE SYNONYM PUBVIEW FOR SUPERU.TABV1",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;

	commit();

	movep ( line , col , "GRANT ... ON TAB1 TO SUPERU" ) ;

	do
	{
		returncode = stmt->execute("GRANT REFERENCES,SELECT,UPDATE,INSERT,DELETE ON TAB1 "
			"TO SUPERU WITH GRANT OPTION");
		CE("main: GRANT ... ON TAB1 TO SUPERU", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (1, stmt->ERRORCODE) ) ;

	CE ("main: GRANT ... ON TAB1 TO SUPERU",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;

	commit();

	movep ( line , col , "CREATE VIEW TABU14" ) ;

	do
	{
		returncode = stmt->execute("CREATE VIEW TABU14 "
			"AS "
			"SELECT TAB1.K1 , TAB1.I1 , TAB1.I2 , "
			"TAB1.C1 , TAB1.C2 , TAB1.I3 , T4.I4 , T4.C3 "
			"FROM TAB1 , SUPERU.TAB4 T4 "
			"WHERE TAB1.I3 = T4.I3 "
			"WITH CHECK OPTION");
		CE("main: CREATE VIEW TABU14", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (1, stmt->ERRORCODE) ) ;

	CE ("main: CREATE VIEW TABU14",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;

	commit();

	do
	{
		returncode = stmt->execute("GRANT REFERENCES,SELECT,UPDATE,INSERT,DELETE ON TAB1 TO SUPERU");
		CE("main: GRANT ... ON TAB1", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (1, stmt->ERRORCODE) ) ;

	CE ("main: GRANT ... ON TAB1",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;

	commit();

	movep ( line , col , "CREATE INDEX TAB1(I3)" ) ;

	do
	{
		returncode = stmt->execute("CREATE INDEX TAB1_I3 ON TAB1(I3)");
		CE("main: CREATE INDEX TAB1.I3", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (1, stmt->ERRORCODE) ) ;

	CE ( "main: CREATE INDEX TAB1.I3",1, stmt->ERRORCODE, stmt->ERRORTEXT);

	commit();

	init_tab2 () ;
	init_tab1 () ;

#ifdef DBPROCS_IMPLEMENTED
	CreateSqlDbProcs () ;
#endif

	movep ( line , col , "***** FINISHED *****" ) ;
	prot ( "***** FINISHED user = %s *****\n", id ) ;

#ifndef _WIN32
	if ( fo )
	{
		fclose(fo);
		fo = NULL ;
	}
#endif

	finish();

	return 0;
}
/*------------------------------------------------------------------------*/
void create_table_tab1 ()
/*
	CREATE TABLE tab1
	ALTER TABLE tab1 DYNAMIC
	NOTE: 'ALTER TABLE <tablename> [NOT] DYNAMIC' is an auto-commited-statement
	COMMIT
*/
{
	movep ( line , col , "CREATE TABLE TAB1"  ) ;

	do
	{
		returncode = stmt->execute("CREATE TABLE TAB1 "
			"(K1     FIXED (10) KEY , "
			"I1     FIXED (10) , "
			"I2     FIXED (10) , "
			"C1     CHAR  (20) , "
			"C2     CHAR  (20) , "
			"I3     FIXED (10) NOT NULL , "
			"FOREIGN KEY ( I3 ) REFERENCES SUPERU.TAB4 "
			"ON DELETE CASCADE )");
		CE("create_table_tab1: CREATE TABLE TAB1", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (2, stmt->ERRORCODE) ) ;

	CE ("create_table_tab1: CREATE TABLE TAB1",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;

    if ( rand () % 2 )
	{
        do
	    {
			returncode = stmt->execute("ALTER TABLE TAB1 DYNAMIC");
			CE("create_table_tab1: ALTER TABLE TAB1 DYNAMIC", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
        } while ( rollberr (2, stmt->ERRORCODE) ) ;

	    CE ("create_table_tab1: ALTER TABLE TAB1 DYNAMIC", 0, stmt->ERRORCODE, stmt->ERRORTEXT) ;
		prot ("TAB1 is DYNAMIC\n", 0 ) ;
	}
	commit();
}
/*------------------------------------------------------------------------*/
void init_table_tab1m ()
/*
	DROP TABLE tab1m
	CREATE TABLE tab1m LIKE tab1
	GRANT REFERENCES to superu
	CREATE INDEX ON i3
	COMMIT
*/
{
	movep ( line , col , "DROP TABLE TAB1M" ) ;
	do
	{
		returncode = stmt->execute("DROP TABLE TAB1M");
		CE("main: DROP TABLE TAB1M", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (2, stmt->ERRORCODE) ) ;

	if ( stmt->ERRORCODE != SQLDBC_OK )
	{
		if ( stmt->ERRORCODE != -4004 )
			CE ("main: DROP TABLE TAB1M",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;
	}
	else
		commit();

	movep ( line , col , "CREATE TABLE TAB1M"  ) ;
	do
	{
		returncode = stmt->execute("CREATE TABLE TAB1M LIKE TAB1 ");
		CE("create_table_tab1: CREATE TABLE TAB1M", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (2, stmt->ERRORCODE) ) ;
	CE ("create_table_tab1: CREATE TABLE TAB1M",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;

	movep ( line , col , "ALTER TABLE TAB1M MODIFY"  ) ;
	do
	{
		returncode = stmt->execute("ALTER TABLE TAB1M MODIFY C2 CHAR(500)");
		CE("create_table_tab1: ALTER TABLE TAB1M MODIFY ...", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (2, stmt->ERRORCODE) ) ;
	CE ("create_table_tab1: ALTER TABLE TAB1M MODIFY ...",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;

	movep ( line , col , "GRANT ... ON TAB1M TO SUPERU" ) ;
	do
	{
		returncode = stmt->execute("GRANT REFERENCES,SELECT,UPDATE,INSERT,DELETE ON TAB1M "
			"TO SUPERU WITH GRANT OPTION");
		CE("main: GRANT ... ON TAB1M TO SUPERU", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (1, stmt->ERRORCODE) ) ;
	CE ("main: GRANT ... ON TAB1M TO SUPERU",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;

	movep ( line , col , "CREATE INDEX TAB1M(I3)" ) ;
	do
	{
		returncode = stmt->execute("CREATE INDEX TAB1M_I3 ON TAB1M(I3)");
		CE("main: CREATE INDEX TAB1M(I3)", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (1, stmt->ERRORCODE) ) ;
	CE ( "main: CREATE INDEX TAB1M(I3)",1, stmt->ERRORCODE, stmt->ERRORTEXT);

	commit();
}
/*------------------------------------------------------------------------*/
void create_table_tab_small ()
/*
	CREATE TABLE tabsmall
	INSERT fixed values into tabsmall
	COMMIT
*/
{
	movep ( line , col , "CREATE TABLE TABSMALL"  ) ;

	do
	{
		returncode = stmt->execute("CREATE TABLE TABSMALL (S CHAR (1) NOT NULL )");
		CE("create_table_tab_small: CREATE TABLE TABSMALL", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (2, stmt->ERRORCODE) ) ;
	CE ("create_table_tab_small: CREATE TABLE TABSMALL",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;

	returncode = stmt->execute("INSERT TABSMALL VALUES ( '1' )");
	CE("create_table_tab_small: INIT TABSMALL ( INSERT )", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	commit();
}
/*------------------------------------------------------------------------*/
void create_table_tab2 ()
/*
	CREATE TABLE tab2
	COMMIT
*/
{
	movep ( line , col , "CREATE TABLE TAB2"  ) ;

	do
	{
		returncode = stmt->execute("CREATE TABLE TAB2 "
			"(I3     FIXED (10) KEY , "
			"I4     FIXED (10) , "
			"C3     CHAR(20) NOT NULL , "
			"CL     LONG )");
		CE("create_table_tab2: CREATE TABLE TAB2", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (2, stmt->ERRORCODE) ) ;

	CE ("create_table_tab2: CREATE TABLE TAB2",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;

	commit();
}
/*------------------------------------------------------------------------*/
void create_view_tabv1 ()
/*
	ALTER TABLE tab1 FOREIGN KEY ...
	CREATE VIEW tabv1
	COMMIT
*/
{
	movep ( line , col , "ALTER TABLE TAB1"  ) ;

	do
	{
		returncode = stmt->execute("ALTER TABLE TAB1 FOREIGN KEY LTABV1( I3 ) REFERENCES TAB2");
		CE("create_view_tabv1: ALTER TABLE TAB1", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (2, stmt->ERRORCODE) ) ;

	CE ("create_view_tabv1: ALTER TABLE TAB1",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;

	movep ( line , col , "CREATE VIEW TABV1"  ) ;

	do
	{
		returncode = stmt->execute("CREATE VIEW TABV1 "
			"AS "
			"SELECT K1 , I1 , I2 , C1 , C2 , TAB2.I3 , I4 , C3 "
			"FROM TAB1 , TAB2 "
			"WHERE TAB1.I3 = TAB2.I3 "
			"WITH CHECK OPTION");
		CE("create_view_tabv1: CREATE VIEW TABV1", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (1, stmt->ERRORCODE) ) ;

	CE ("create_view_tabv1: CREATE VIEW TABV1",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;

	commit();
}
/*------------------------------------------------------------------------*/
void init_tab1 ()
/*
	INSERT from array in tab1 ... until successful execution
	COMMIT
*/
{
    int     loop_cnt = 0 ;
    long    k1[50]={0};
    c20     c1[50]={0}, c2[50]={0};
    long    i1[50]={0}, i2[50]={0}, i3[50]={0};

	SQLDBC_Length indi1[50]={0}, indi2[50]={0};

	int i = 0, j = 0;

    movep ( line , col , "INSERT TAB1"  ) ;

    rolled_back = 0 ;

    for ( i = 1 ; i < rows / 4 ; i += 50 )
    {
		for ( loop_cnt = 0 , j = i ; loop_cnt < 50 && j < rows / 4 ; j++ , loop_cnt++ )
		{
		    indi1[loop_cnt]=sizeof(long);
			indi2[loop_cnt]=SQLDBC_NTS;

			k1 [loop_cnt] = j ;
			i1 [loop_cnt] = j * 100 ;
			i2 [loop_cnt] = j % 10 ;
			sprintf ( c1 [loop_cnt] , "Name%s_%d" , id , j ) ;
			sprintf ( c2 [loop_cnt] , "Name%s.%d" , id , j / 10 ) ;
			i3 [loop_cnt] = j / 10 ;
		}

		movep ( line , col , "INSERT TAB1, n = %d" , i - 1 + loop_cnt ) ;

		if ( loop_cnt == 0 ) return ;

		pstmt = conn->createPreparedStatement();
		CE("init_tab1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

		returncode = checkPreparedStatement(pstmt, "INSERT TAB1 VALUES ( ?, ?, ?, ?, ?, ?)");
		CE("init_tab1: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, k1, &indi1[0], sizeof(long)); 
		if (returncode != SQLDBC_OK) CE("error with bindParameter in init_tab1 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, MY_HOSTTYPE_LONG, i1, &indi1[0], sizeof(long)); 
		if (returncode != SQLDBC_OK) CE("error with bindParameter in init_tab1 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, MY_HOSTTYPE_LONG, i2, &indi1[0], sizeof(long)); 
		if (returncode != SQLDBC_OK) CE("error with bindParameter in init_tab1 (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, c1, &indi2[0], sizeof(c20)); 
		if (returncode != SQLDBC_OK) CE("error with bindParameter in init_tab1 (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_ASCII, c2, &indi2[0], sizeof(c20)); 
		if (returncode != SQLDBC_OK) CE("error with bindParameter in init_tab1 (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (6, MY_HOSTTYPE_LONG, i3, &indi1[0], sizeof(long)); 
		if (returncode != SQLDBC_OK) CE("error with bindParameter in init_tab1 (col 6)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		
		returncode = pstmt->setBatchSize(loop_cnt);
		do
		{
			returncode = pstmt->execute();
			CE ("init_tab1: INIT TAB1 ( INSERT )" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
		}
		while ( rollberr (1, pstmt->ERRORCODE) && rolled_back == 0 ) ;

		loop_cnt = 0 ;

		if ( pstmt->ERRORCODE != 350 && pstmt->ERRORCODE != 200 )
		{
			CE ("init_tab1: INSERT TAB1",1, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
		}
		conn->releaseStatement(pstmt);
		pstmt = 0;

		commit();
	}
}
/*------------------------------------------------------------------------*/
void init_tab2 ()
/*
	INSERT from array in tab2 ... until successful execution
	COMMIT
*/
{
    c20     c3[40]={0};
    long    i3[40]={0}, i4[40]={0};
    int     loop_cnt = 0;

	//VARCHAR cl [40][500] ;
	struct myvarchar{/*unsigned short len; */char arr [/*40*/51];} cl[500]={0};

	SQLDBC_Length indi1[40]={0}, indi2[40]={0};

	int i = 0, j = 0;

    rolled_back = 0 ;

    movep ( line , col , "INSERT TAB2" ) ;

    for ( i = 0 ; i < rows / 8 ; i+=40 )
    {
		for ( loop_cnt = 0 , j = i ; loop_cnt < 40 && j < rows / 8 ; j++ , loop_cnt++ )
		{
		    indi1[loop_cnt]=sizeof(long);
			indi2[loop_cnt]=SQLDBC_NTS;

			i3 [loop_cnt] = j ;
			i4 [loop_cnt] = 10 - j ;
			sprintf ( c3 [loop_cnt] , "tab2%d" , j ) ;
 			strcpy ( cl [loop_cnt].arr , "Langer String                             bis hier") ;
/*			cl [loop_cnt].len = (int)strlen ( cl [loop_cnt].arr ) ;*/
		}

		movep ( line , col , "INSERT TAB2, n = %d\n" , i + loop_cnt ) ;

		pstmt = conn->createPreparedStatement();
		CE("init_tab2: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
		returncode = checkPreparedStatement(pstmt, "INSERT TAB2 VALUES ( ?, ?, ?, ?)");
		CE("init_tab2: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
			
		returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, i3, &indi1[0], sizeof(long)); 
		if (returncode != SQLDBC_OK) CE("error with bindParameter in init_tab2 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, MY_HOSTTYPE_LONG, i4, &indi1[0], sizeof(long)); 
		if (returncode != SQLDBC_OK) CE("error with bindParameter in init_tab2 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_ASCII, c3, &indi2[0], sizeof(c20)); 
		if (returncode != SQLDBC_OK) CE("error with bindParameter in init_tab2 (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, cl, &indi2[0], sizeof(myvarchar)); 
		if (returncode != SQLDBC_OK) CE("error with bindParameter in init_tab2 (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->setBatchSize(loop_cnt);

		do
		{
			returncode = pstmt->execute();
			CE ("init_tab2: INSERT TAB2",0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
		} while ( rollberr (1, pstmt->ERRORCODE) && rolled_back == 0 ) ;

		if ( pstmt->ERRORCODE != 350 && pstmt->ERRORCODE != 200 ) 
			CE ("init_tab2: INSERT TAB2",1, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;

		conn->releaseStatement(pstmt);
		pstmt = 0;
		commit();
    }
}
/*------------------------------------------------------------------------*/
void CreateSqlDbProcs ()
/*
	CREATE various DBProcedures needed for the users execution in rendlos.cpp
*/
{
		char sql1[500]={0}, sql2[500]={0}, sql3[500]={0};
//see old MutDbProcsObj.cpp in test\port\mut\MutDbProcs
		strcpy (sql1, "CREATE DBPROC MUTUPD100000 (IN myc1 char) AS VAR i integer; TRY SET i = 0; WHILE (i < 20000) DO BEGIN UPDATE ");
		strcat (sql1, id);
		strcat (sql1, ".TABSMALL SET S = '2'; SET i = i + 1; END; CATCH IF $rc <> 100 THEN STOP ($rc, 'unexpected error'); IF $rc = 0 THEN TRY SET i = 0; WHILE (i < 80000) DO BEGIN UPDATE ");
		strcat (sql1, id);
		strcat (sql1, ".TABSMALL SET S = :myc1; SET i = i + 1; END; CATCH IF $rc <> 100 THEN STOP ($rc, 'unexpected error');");
		returncode = stmt->execute(sql1);
		if ( stmt->ERRORCODE != SQLDBC_OK && stmt->ERRORCODE != -6006 )
			CE ("CreateSqlDbProcs: CREATE DBPROC MUTUPD100000" , 1, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
		returncode = stmt->execute("GRANT EXECUTE ON MUTUPD100000 TO PUBLIC");
		CE("CreateSqlDbProcs: GRANT EXECUTE ON MUTUPD100000 TO PUBLIC", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
		commit();

		strcpy (sql2, "CREATE DBPROC MUTUPDTAB1 (IN i1 int, IN i2 int, IN c1 char(21), IN c2 char(21), IN k1 int) AS TRY UPDATE ");
		strcat (sql2, id);
		strcat (sql2, ".TAB1 SET I1 = :i1 , I2 = :i2 , C1 = :c1 , C2 = :c2 WHERE K1 = :k1; CATCH IF $rc <> 100 THEN STOP ($rc, 'unexpected error');");
		returncode = stmt->execute(sql2);
		if ( stmt->ERRORCODE != SQLDBC_OK && stmt->ERRORCODE != -6006 )
			CE ("CreateSqlDbProcs: CREATE DBPROC MUTUPDTAB1" , 1, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
		returncode = stmt->execute("GRANT EXECUTE ON MUTUPDTAB1 TO PUBLIC");
		CE("CreateSqlDbProcs: GRANT EXECUTE ON MUTUPDTAB1 TO PUBLIC", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
		commit();
		
		strcpy (sql3, "CREATE DBPROC MUTDELSUPTAB1 (IN k1 int) AS TRY DELETE SUPERU.TAB1 WHERE K1 = :k1; CATCH IF $rc <> 100 THEN STOP ($rc, 'unexpected error');");
		returncode = stmt->execute(sql3);
		if ( stmt->ERRORCODE != SQLDBC_OK && stmt->ERRORCODE != -6006 )
			CE ("CreateSqlDbProcs: CREATE DBPROC MUTDELSUPTAB1" , 1, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
		returncode = stmt->execute("GRANT EXECUTE ON MUTDELSUPTAB1 TO PUBLIC");
		CE("CreateSqlDbProcs: GRANT EXECUTE ON MUTDELSUPTAB1 TO PUBLIC", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
		commit();
}
/*------------------------------------------------------------------------*/
void pe (const char *s , int leave, int errorcode, const char *errortext )
/*
	pe = "print error"
	checks the error from the last command
	with "leave"-flag or some special errors stop kernel (sqladump)
*/
{
	char    zeile [200]={0};

#ifdef DEBUG
	fprintf ( stderr , "pid %d , %s :  ERR = %d\n" , pid , s , errorcode ) ;
#endif

	sprintf ( zeile , "%s :  ERR = %d" , s , errorcode ) ;
	movep ( line , col , zeile) ;

	if ( ( errorcode != 100 ) && ( errorcode != 200 ) )
		print_f ( s, errorcode, errortext ) ;

/*NEW because sqladump breaks down connection and with connection break down no further statement should be sent*/
	if (errorcode == -10807 || errorcode == -10709)
	{
		print_f(s, errorcode, errortext);
		remove( actfile );
		exit(1);
	}
/*ENDNEW*/

	if ( leave || errorcode == -108 || errorcode == -6000 || errorcode <= -9000 ||
		errorcode == -7500 /*|| ( errorcode != -809 && ( errorcode <= -800 && errorcode > -900 ) ) precompiler */)
		{
			print_f ( s, errorcode, errortext ) ;
			Sqldump (s, errorcode, errortext, conn) ;
		}
}
/*------------------------------------------------------------------------*/
int rollberr (int kind, int errorcode)
/*
	large switch for deciding whether to rollback or try again if commands fail with execution
*/
{
	int sqlc = 0;
	if ( ROLLBERR(errorcode) )
	{
	    switch ( kind )
		{
			case 0 : 
				sleep (3) ; 
				return ( 1 ) ; 
				break ;
			case 1 :
				if ( ( rand () % 10 ) == 0 )
				{
						rollback () ;
						prot ("Transaction rolled back, loop again\n","") ;
						rolled_back = 1 ;
						sleep ( 3 ) ;
				}
				else if ( errorcode == -1000 )
					sleep ( 3 ) ;
				return ( 1 ) ;
				break ;
			case 2 :
				rollback () ;
				if ( errorcode == -1000 )
					sleep ( 3 ) ;
				else
					sleep ( 1 ) ;
				prot ("Transaction rolled back, loop again\n","") ;
				rolled_back = 1 ;
				return ( 1 ) ;
				break ;
			case 3 :
				sqlc = errorcode ;
				rollback () ;
				prot ("Transaction rolled back, no more loop\n","") ;
				errorcode = sqlc ;
				rolled_back = 1 ;
				return ( 0 ) ;
				break ;
			default :
				return -2 ;
		}
	}
	else
		return (0) ;
}
/*------------------------------------------------------------------------*/
void movep (int line, int col, const char *s1, ...)
/*
	writes the given text to the console at a given line and column
*/
{
	char    zeile [200]={0};

	va_list paramlist;
	va_start(paramlist, s1);

    vsprintf ( zeile , s1 , paramlist ) ;
	
	va_end(paramlist);
    
	if ( line > 22 ) line = 22 ;

#ifdef _WIN32
    sprintf (buf , "\033[%d;%dH\033[K%s\r",line,col,zeile);
    sflushf (buf) ;
    sprintf (buf , "\033[21;1H") ;
    sflushf (buf) ;
#else
    if ( fo )
    {
		fflush (fo) ;
		fprintf (fo , "\033[%d;%dH\033[K%s\r",line,col,zeile);
		fflush (fo) ;
		fprintf (fo , "\033[21;1H") ;
		fflush (fo) ;
    }
#endif
}
