/*
 * @(#) rendlos.cpc	2005-03-08

	Simulates the behavior of the users, one file for each user

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
#include "jtest/mutsqldbc/fdircheck.h"

#include <signal.h>
#include <ctype.h>

#define  DBPROCS_IMPLEMENTED

#ifdef   WIN32
#include "jtest/mutsqldbc/flushbuf.h" /*nocheck*/
#endif

void check_long_raw (const char *, int, int, int) ;
void alt_tab1_dynamic () ;
void alt_tab1_nondynamic () ;
void update100000 () ;
void release_connect () ;
void ins_upd_tab1 (int) ;
void del_intersect_superu () ;
void select_and_check_superu_tab1sl () ;
void update_superu_tab1sl () ;
void count_tab1 () ;
void delete_tab1 () ;
void sel_upd_sup () ;
void union_all_tab1 () ;
void sel_ins_sup () ;
void check_lr_su () ;
void del_sup () ;
void update_stat_sample () ;
void update_stat () ;
void ins_tabu2 () ;
void upd_tabv1 () ;
void sel_distinct_tab1 () ;
void sel_temp_inv_tab1 () ;
void sel_first_stab1_ins_tab1 () ;
void ins_stab1_sel_first_tab1 () ;
void complex_select () ;
void between_sbview () ;
void like_bview () ;
void equal_sbview () ;
void equal_bview_only_index () ;
void between_sbview_16 () ;
void equal_sbview_16 () ;
void like_bview_16 () ;
void equal_bview_only_index_16 () ;
void insert_array_tab1 (int) ;
void mass_insert_array_tab1m () ;
void insert_tab20k () ;
void ins_except_tab1 () ;
void delete_tab20k () ;
void delete_tab20k_2 () ;
void del_avg_tab1 (int) ;
void delete_between_tab20k () ;
void update_key_tab20k_1 () ;
void update_key_tab20k_2 () ;
void insert_tab04sk () ;
void delete_tab04sk () ;
void delete_range_tab04sk () ;
void update_tab04sk () ;
void cre_idx_tab1 () ;
void drop_idx_tab1 () ;
void use_tab_cluster_1();
void use_tab_cluster_2();
int rollberr (int, int );
void cwr_ok (int) ;

typedef char    c20 [21] ;
typedef char    c40 [41] ;
typedef char	c100 [101] ;
typedef char	c500 [501] ;

long    k1 , k2 , k3 ;
long    k1m [10] ;
c20     c1 , c2 , c3, c4 ;
SQLDBC_Length c1ind   , c2ind  , c3ind  ;
c20     c1m [10] , c2m [10] ;
c100	c5 ;
long    i1 , i2 , i3 , i4 , i5 , i6 ;
SQLDBC_Length i1ind , i2ind , i3ind ;
long    i1m [10] , i2m [10] , i3m [10] ;
long    ii ;
long    nr , avg ;
int     cnt ;
SQLDBC_Length cllen , icllen ;
struct {SQLDBC_Length len; char arr [MAXLONGRAW];} cll;
char    k0 [240] ;
char    c6 [1501] ;
char	char1a [2], char1b [2], char1c [2];
char	char5 [6] ;
char	char400 [401] ;
char	char400_2 [401] ;
int		rc ;
int		ResultCount ;
struct  {long len; char arr[100000];} scl;
long    lnll , nrll ;
int		mytime, mypid ;

#ifdef DBPROCS_IMPLEMENTED
int		DbProcUsed = 1 ;
#else
int		DbProcUsed = 0 ;
#endif

#ifdef _WIN32
char    buf [256] ;
#else
FILE    *fo = NULL;
#endif

int     rolled_back = 0 ;
long	endTime = 0 ;
int		endTimeReached = 0 ;
int		function_called_counter = 0;
int		function_called_counter_all = 0;


int main (int argc , char **argv)
{
	int		i = 0;
	char	head_line[80]={0};

#ifndef _WIN32
    fo = fopen ( "/dev/tty" , "w" ) ;
    if ( fo )
      if ( ! isatty(fileno(fo)) )
      {
        fclose(fo);
        fo = NULL ;
      }
#endif

	mypid = pid = getpid () ;
	srand(pid);

	check_params(argc, argv);
	sprintf(actfile, "rendlos%s.act", id);
	{
		char filename [100]={0} ;
		sprintf ( filename , "rendlos%s.prot" , id ) ;

		if ( ( fout = fopen ( filename , "w" ) ) == NULL )
		{
			char outtext [100]={0} ;
			sprintf ( outtext , "Error opening file '%s'\n" , filename ) ;
			perror ( outtext ) ;
			exit (-99) ;
		}
	}

	if ( maxRuntime > 0 )
	{
		endTime = (long) time ((time_t *) 0 ) + maxRuntime  ;
		maxcycle = 0x7fffffff ;		//7fffffff = 2147483647
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

	if ( maxRuntime > 0 )
		prot ("max. runtime : %d sec\n" , maxRuntime ) ;
	else
		prot ("max. cylces : %d\n" , maxcycle ) ;

	env = new  SQLDBC_Environment (getRuntime());
	conn = create_conn(env);

	//the 'CHOPBLANK' property removes blanks that will be automatically returned for varchar types;
	//	otherwise every returned varchar value has the full length defined with create table and every
	//	comparison will failed
	CP.setProperty("CHOPBLANKS", "1");
	if (id == "A1" || id == "A3" || id == "A5")
		CP.setProperty("ISOLATIONLEVEL", "1");
	else
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

	signal ( SIGTERM , SIGFUNC cwr_ok ) ;

	for (i = 0; i < MAXIGNORE; i++)
	{
		prot ("No Dump Request for Fatal Error = %d\n" , ignore[i] ) ;
	}

	pstmt = conn->createPreparedStatement();
	CE("main: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	returncode = checkPreparedStatement(pstmt, "SELECT maxr INTO ? FROM superu.maxraw WHERE ROWNO <= 1");
	CE("main: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	SQLDBC_Length indi1=sizeof(maxlongraw);
	returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &maxlongraw, &indi1, sizeof(maxlongraw));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->execute();
	CE("main: SELECT MAXRAW ...", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	conn->releaseStatement(pstmt);
	pstmt = 0;

	prot ("maxlongraw = %d\n" , maxlongraw ) ;

	memset ( k0 , 'x' , 240 ) ;
	k0[239] = 0 ;
	memset ( c6 , 'y' , 1500 ) ;
	c6 [1499] = 0 ;

	if ( DbProcUsed )
	{
		char sql1[500]={0}, sql2[500]={0};

		/*
			CREATE DBPROC MUTUPD100000 (IN myc1 char) AS
			VAR i integer;
			TRY
				SET i = 0;
				WHILE (i < 20000) DO BEGIN
					UPDATE <id>.TABSMALL SET S = '2';
					SET i = i + 1;
				END;
			CATCH
				IF $rc <> 100 THEN STOP ($rc, 'unexpected error');
				IF $rc = 0 THEN
					TRY
					SET i = 0;
					WHILE (i < 80000) DO BEGIN
						UPDATE <id>.TABSMALL SET S = :myc1;
						SET i = i +1;
					END;
					CATCH
						IF $rc <> 100 THEN STOP ($rc, 'unexpected error');
		*/
		strcpy (sql1, "CREATE DBPROC MUTUPD100000 (IN myc1 char) AS VAR i integer; TRY SET i = 0; WHILE (i < 20000) DO BEGIN UPDATE ");
		strcat (sql1, id);
		strcat (sql1, ".TABSMALL SET S = '2'; SET i = i + 1; END; CATCH IF $rc <> 100 THEN STOP ($rc, 'unexpected error'); IF $rc = 0 THEN TRY SET i = 0; WHILE (i < 80000) DO BEGIN UPDATE ");
		strcat (sql1, id);
		strcat (sql1, ".TABSMALL SET S = :myc1; SET i = i + 1; END; CATCH IF $rc <> 100 THEN STOP ($rc, 'unexpected error');");

		returncode = stmt->execute(sql1);
		if ( stmt->ERRORCODE != 0 && stmt->ERRORCODE != -6006 )
			CE ("main: CREATE DBPROC MUTUPD100000" , 1, stmt->ERRORCODE, stmt->ERRORTEXT ) ;

		returncode = stmt->execute("GRANT EXECUTE ON MUTUPD100000 TO PUBLIC");
		CE("main: GRANT EXECUTE ON MUTUPD100000 TO PUBLIC", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

		commit();


		/*
			CREATE DBPROC MUTUPDTAB1 (IN i1 int, IN i2 int, IN c1 char(21), IN c2 char(21), IN k1 int) AS
			TRY
				UPDATE <id>.TAB1 SET I1 = :i1 , I2 = :i2 , C1 = :c1 , C2 = :c2 WHERE K1 = :k1;
			CATCH
				IF $rc <> 100 THEN STOP ($rc, 'unexpected error');
		*/
		strcpy (sql2, "CREATE DBPROC MUTUPDTAB1 (IN i1 int, IN i2 int, IN c1 char(21), IN c2 char(21), IN k1 int) AS TRY UPDATE ");
		strcat (sql2, id);
		strcat (sql2, ".TAB1 SET I1 = :i1 , I2 = :i2 , C1 = :c1 , C2 = :c2 WHERE K1 = :k1; CATCH IF $rc <> 100 THEN STOP ($rc, 'unexpected error');");

		returncode = stmt->execute(sql2);
		if ( stmt->ERRORCODE != 0 && stmt->ERRORCODE != -6006 )
			CE ("main: CREATE DBPROC MUTUPDTAB1" , 1, stmt->ERRORCODE, stmt->ERRORTEXT ) ;

		returncode = stmt->execute("GRANT EXECUTE ON MUTUPDTAB1 TO PUBLIC");
		CE("main: GRANT EXECUTE ON MUTUPDTAB1 TO PUBLIC", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

		commit();
	}


	for ( cycle = 1 ; cycle <= maxcycle && endTimeReached == 0 ; cycle++ )
	{
		function_called_counter = 0;

		sprintf ( head_line , "RESOURCE loc '%s', user '%s' : cycle %d" , DBNAME, id , cycle ) ;
		movep (line-1 , col , "%s" , head_line ) ;
		switch ( cycle % 5 )
		{
			case 0 :
				drop_idx_tab1 () ;
				del_avg_tab1 (0) ;
				alt_tab1_dynamic () ;
				verify_fdir_table_info();
				verify_fdir_index_info();
				break ;
			case 1 :
				cre_idx_tab1 () ;
				del_avg_tab1 (1) ;
				break ;
			case 2 :
				drop_idx_tab1 () ;
				del_avg_tab1 (0) ;
				update_stat () ;
				verify_fdir_table_info();
				verify_fdir_index_info();
				break ;
			case 3 :
				update100000 () ;
				alt_tab1_nondynamic () ;
				break ;
			case 4 :
				cre_idx_tab1 () ;
				del_avg_tab1 (1) ;
				break ;
		}

		for ( i = 1 ; ( i < 400) && ( endTimeReached == 0 ) ; i++ )
		{
			movep ( line-1 , 60 , "%d" , i ) ;

			if ((i % 10) == 0) prot("i = %d\n", i);

			ins_upd_tab1 (i) ;

			if ( (( rand ()  % 2 ) * use_random) == 0 )
				delete_tab1 () ;

			if ( (( rand ()  % 4 ) * use_random) == 0 )
				insert_tab04sk () ;

			if ( (( rand ()  % 5 ) * use_random) == 0 )
				insert_tab20k () ;

			if ( (( rand ()  % 5 ) * use_random) == 0 )
				sel_upd_sup () ;

			if ( (( rand ()  % 5 ) * use_random) == 0 )
				sel_ins_sup () ;

			if ( (( rand ()  % 5 ) * use_random) == 0 )
				del_sup () ;

			if ( (( rand ()  % 6 ) * use_random) == 0 )
				delete_tab20k_2 () ;

			if ( (( rand ()  % 8 ) * use_random) == 0 )
				delete_tab20k () ;

			if ( (( rand ()  % 9 ) * use_random) == 0 )
				ins_tabu2 () ;

			if ( (( rand ()  % 12 ) * use_random) == 0 )
				check_lr_su () ;

			if ( (( rand ()  % 13 ) * use_random) == 0 )
				sel_distinct_tab1 () ;

			if ( (( rand ()  % 15 ) * use_random) == 0 )
				sel_temp_inv_tab1 () ;

			if ( (use_cluster == 1) && (( rand ()  % 15 ) * use_random) == 0 )
				use_tab_cluster_1();

			if ( (use_cluster == 1) && ( (rand ()  % 15 ) * use_random) == 0 )
				use_tab_cluster_2();

			if ( (( rand ()  % 17 ) * use_random) == 0 )
				sel_first_stab1_ins_tab1 () ;

			if ( (( rand ()  % 18 ) * use_random) == 0 )
				delete_tab04sk () ;

			if ( (( rand ()  % 21 ) * use_random) == 0 )
				update_key_tab20k_1 () ;

			if ( (( rand ()  % 23 ) * use_random) == 0 )
				update_superu_tab1sl () ;

			if ( (( rand ()  % 29 ) * use_random) == 0 )
				update_key_tab20k_2 () ;

			if ( (( rand ()  % 35 ) * use_random) == 0 )
				ins_except_tab1 () ;

			if ( (( rand ()  % 21 ) * use_random) == 0 )
				update_tab04sk() ;

			if ( (( rand ()  % 37 ) * use_random) == 0 )
				mass_insert_array_tab1m () ;

			if ( (( rand ()  % 37 ) * use_random) == 0 )
				ins_stab1_sel_first_tab1 () ;

			if ( (( rand ()  % 37 ) * use_random) == 0 )
			{
				int iii ;
				for ( iii = 0 ; iii < 4 ; iii++ )
				{
					union_all_tab1 () ;
				}
			}

			if ( (( rand ()  % 51 ) * use_random) == 0 )
				update_stat_sample () ;

			if ( (( rand ()  % 59 ) * use_random) == 0 )
				insert_array_tab1 ( cycle + 2 ) ;

			if ( (( rand ()  % 71 ) * use_random) == 0 )
				upd_tabv1 () ;

			if ( (( rand ()  % 73 ) * use_random) == 0 )
				complex_select () ;

			if ( (( rand ()  % 90 ) * use_random) == 0 )
				count_tab1 () ;

			if ( (( rand () % 93 ) * use_random) == 0 )
				select_and_check_superu_tab1sl  ()  ;

			if ( (( rand () % 97 ) * use_random) == 0 )
				delete_between_tab20k () ;

			if ( (( rand () % 117 ) * use_random) == 0 )
				delete_range_tab04sk () ;

/*	in the meantime no test for joins over 64 tables necessary; 16 is enough
#ifdef JOIN64
			if ( (( rand ()  % 171 ) * use_random) == 0 )
				between_sbview () ;

			if ( (( rand ()  % 172 ) * use_random) == 0 )
				like_bview () ;

			if ( (( rand ()  % 173) * use_random) == 0 )
				equal_sbview () ;

			if ( (( rand ()  % 174) * use_random) == 0 )
				equal_bview_only_index () ;
#else
*/
			if ( (( rand ()  % 121 ) * use_random) == 0 )
				between_sbview_16 () ;

			if ( (( rand ()  % 122 ) * use_random) == 0 )
				like_bview_16 () ;

			if ( (( rand ()  % 123) * use_random) == 0 )
				equal_sbview_16 () ;

			if ( (( rand ()  % 124) * use_random) == 0 )
				equal_bview_only_index_16 () ;
/*
#endif
*/
			if ( i == 1 && cycle > 2 )
				del_intersect_superu () ;

			if ( (( rand ()  % 117 ) * use_random) == 0 )
				release_connect () ;

			if ( maxRuntime > 0 && endTime < (long) time ((time_t *) 0 ) )
			{
				prot ("Endtime reached: %ld\n", (long) time ((time_t *)0) );
				endTimeReached = 1 ;
				/* break ; */
			}

			if (use_random == 0 )
			{
				break;
			}
		}
		prot("*** %d functions were called in cycle %d ***\n", function_called_counter, cycle);
		function_called_counter_all = function_called_counter_all + function_called_counter;
	}

	prot("***** %d functions were called at all *****\n", function_called_counter_all);

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
void release_connect ()
/*
	CommitWorkRelease
	ReleaseConnection (not Environment)
	try to connect until succesfully connected
	create Statement (stmt)
*/
{
	function_called_counter++;

	movep ( line , col , "COMMIT WORK RELEASE , ERR = %d" , conn->ERRORCODE ) ;
	cwr();

	env->releaseConnection(conn);

	sleep ( rand () % 5 ) ;

	do
	{
		conn = create_conn(env);

		//the 'CHOPBLANK' property removes blanks that will be automatically returned for varchar types;
		//	otherwise every returned varchar value has the full length defined with create table and every
		//	comparison will failed
		CP.setProperty("CHOPBLANKS", "1");
		if (id == "A1" || id == "A3" || id == "A5")
			CP.setProperty("ISOLATIONLEVEL", "1");
		else
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
	} while ( rollberr (0, conn->ERRORCODE) ) ;

    CE ("release_connect: CONNECT" , 1, conn->ERRORCODE, conn->ERRORTEXT ) ;

//	is_database_unicode();

	stmt = conn->createStatement();
	CE("release_connect: createStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

	prot("RECONNECT ... done \n");

	database_info_own();
}
/*------------------------------------------------------------------------*/
void ins_upd_tab1 (int ii)
/*
	initialize some values
	INSERT TAB1
	if duplicate key error, try to UPDATE TAB1 manual or by DBProcedure MUTUPDTAB1
*/
{
	function_called_counter++;

	int i = 0;

	i = ii ;
	k1 = rand () % rows ;
	i1 = ( i * 133 ) % 1113 ;
	i2 = ( k1 % 13 ) * 47 + ( i % 10 ) * 9 ;
	sprintf ( c1 , "Name_%s_%d" , id , k1 ) ;
	sprintf ( c2 , "Name.%s_%d" , id , k1 / 10 ) ;
	i3 = i / 10 ;

	movep ( line , col , "INSERT TAB1 , K1 = %d\r" , k1 ) ;

	pstmt = conn->createPreparedStatement();
	CE("ins_upd_tab1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
	{
		returncode = checkPreparedStatement(pstmt, "INSERT TAB1 VALUES ( ? , ( ? * 133 ) MOD 1113, "
			"( ? MOD 13 ) * 47 + ( ? MOD 10 ) * 9 , ? , ? , ? )");
		CE("ins_upd_tab1: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		SQLDBC_Length indi1=sizeof(long), indi2=sizeof(i), indi3=SQLDBC_NTS;
		returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &k1, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_upd_tab1 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &i, &indi2, sizeof(i));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_upd_tab1 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, MY_HOSTTYPE_LONG, &k1, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_upd_tab1 (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_INT4, &i, &indi2, sizeof(i));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_upd_tab1 (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_ASCII, &c1, &indi3, sizeof(c1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_upd_tab1 (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (6, SQLDBC_HOSTTYPE_ASCII, &c2, &indi3, sizeof(c2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_upd_tab1 (col 6)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (7, MY_HOSTTYPE_LONG, &i3, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_upd_tab1 (col 7)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
/*
	'Name.' || substr (:id,1,2) || '_' || chr ( :k1 ) ,
	'Name.' || substr (:id,1,2) || '.' || chr ( :k1 DIV 10 ) ,
*/
	CE ( "ins_upd_tab1: INSERT TAB1" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
	} while ( rollberr (1, pstmt->ERRORCODE) ) ;

	if ( pstmt->ERRORCODE == 200 )  /* duplicate primary key */
	{
		pstmt2 = conn->createPreparedStatement();
		CE("ins_upd_tab1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
		if ( DbProcUsed == 0 || ( rand () % 5 ) )
		{
			movep ( line , col , "UPDATE TAB1 , K1 = %d\r" , k1 ) ;

			do
			{
				returncode = checkPreparedStatement(pstmt2, "UPDATE TAB1 SET I1 = ? , I2 = ? , "
					"C1 = ? , C2 = ? WHERE K1 = ?");
				CE("ins_upd_tab1: prepare", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
				SQLDBC_Length indi1=sizeof(long), indi3=SQLDBC_NTS;
				returncode = pstmt2->bindParameter (1, MY_HOSTTYPE_LONG, &i1, &indi1, sizeof(long));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_upd_tab1 (col 8)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
				returncode = pstmt2->bindParameter (2, MY_HOSTTYPE_LONG, &i2, &indi1, sizeof(long));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_upd_tab1 (col 9)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
				returncode = pstmt2->bindParameter (3, SQLDBC_HOSTTYPE_ASCII, &c1, &indi3, sizeof(c1));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_upd_tab1 (col 10)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
				returncode = pstmt2->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &c2, &indi3, sizeof(c2));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_upd_tab1 (col 11)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
				returncode = pstmt2->bindParameter (5, MY_HOSTTYPE_LONG, &k1, &indi1, sizeof(long));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_upd_tab1 (col 12)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
				returncode = pstmt2->execute();
				CE ("ins_upd_tab1: UPDATE TAB1",0, pstmt2->ERRORCODE, pstmt2->ERRORTEXT) ;
			} while ( rollberr (1, pstmt2->ERRORCODE) ) ;

		}
		else
		{
			movep ( line , col , "UPDATE TAB1 (DBPROC), K1 = %d\r" , k1 ) ;
			do
			{
				returncode = checkPreparedStatement(pstmt2, "CALL MUTUPDTAB1(?, ?, ?, ?, ?)");
				CE("ins_upd_tab1: prepare", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);

				SQLDBC_Length indi1=sizeof(long), indi3=SQLDBC_NTS;

				returncode = pstmt2->bindParameter (1, MY_HOSTTYPE_LONG, &i1, &indi1, sizeof(long));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_upd_tab1 (col 13)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
				returncode = pstmt2->bindParameter (2, MY_HOSTTYPE_LONG, &i2, &indi1, sizeof(long));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_upd_tab1 (col 14)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
				returncode = pstmt2->bindParameter (3, SQLDBC_HOSTTYPE_ASCII, &c1, &indi3, sizeof(c1));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_upd_tab1 (col 15)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
				returncode = pstmt2->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &c2, &indi3, sizeof(c2));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_upd_tab1 (col 16)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
				returncode = pstmt2->bindParameter (5, MY_HOSTTYPE_LONG, &k1, &indi1, sizeof(long));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_upd_tab1 (col 17)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
				returncode = pstmt2->execute();
				CE ("ins_upd_tab1: UPDATE TAB1 (DBPROC)",0, pstmt2->ERRORCODE, pstmt2->ERRORTEXT) ;
			} while (rollberr (1, pstmt2->ERRORCODE) ) ;
		}
		conn->releaseStatement(pstmt2);
		pstmt2 = 0;
	}
	CE ("ins_upd_tab1: UPDATE TAB1",0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;

	commit () ;

	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void update100000 ()
/*
	UPDATE TABSMALL manual or by DBProcedure MUTUPD100000
*/
{
	function_called_counter++;

	long i ;
	char myc1[2]={0};
	sprintf ( myc1 , "3") ;

	if ( ( rand ()  % 6 ) != 0 )
		return ;

	pstmt = conn->createPreparedStatement();
	CE("update100000: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	if ( DbProcUsed && ( rand () % 2 == 0 ) )
	{
		movep ( line , col , "UPDATE 100000 TABSMALL (DBPROC)") ;
		returncode = checkPreparedStatement(pstmt, "CALL MUTUPD100000(?)");
		CE("update100000: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi3=SQLDBC_NTS;
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &myc1, &indi3, sizeof(myc1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update100000 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
		CE ("update100000: UPDATE TABSMALL (DBPROC)",1, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
	}
	else
	{
		movep ( line , col , "UPDATE 100000 TABSMALL") ;
		for ( i = 0 ; i < 100000 ; i++ )
		{
			returncode = stmt->execute("UPDATE TABSMALL SET S = '2'");
			CE ("update100000: UPDATE TABSMALL",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;
		}
	}
	commit () ;

	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void del_intersect_superu ()
/*
	DELETE FROM TAB1 with an INTERSECT of superu.TAB1
	COMMIT or ROLLBACK
*/
{
	function_called_counter++;

	do
	{
		movep ( line , col , "DELETE ... INTERSECT ... SUPERU.TAB1");

		returncode=stmt->execute("DELETE FROM TAB1 WHERE K1 IN "
			"( SELECT K1 FROM superu.TAB1 INTERSECT SELECT K1 FROM TAB1 )");
		if ( stmt->ERRORCODE == -6000 )
		{
			prot ( "DELETE ... INTERSECT ... SUPERU.TAB1 %d", stmt->ERRORCODE);
			rollback () ;
		}

		CE ( "del_intersect_superu: DELETE ... INTERSECT ... SUPERU.TAB1",0, stmt->ERRORCODE, stmt->ERRORTEXT);

	} while ( rollberr (1, stmt->ERRORCODE) ) ;
	if ( stmt->ERRORCODE == -3009 )
	{
		CE ( "del_intersect_superu: DELETE ... INTERSECT ... SUPERU.TAB1",1, stmt->ERRORCODE, stmt->ERRORTEXT);
	}

	if ( rand () % 2 )
	{
		commit () ;
	}
	else
	{
		movep ( line , col , "DELETE ... INTERSECT ... SUPERU.TAB1 (ROLLBACK)");
		rollback () ;
	}
}
/*------------------------------------------------------------------------*/
void select_and_check_superu_tab1sl  ()
/*
	SELECT randomized value from superu.TAB1 with LOCK ISOLATION LEVEL 1
	fetch through ResultSet and check whether values are equal
*/
{
	function_called_counter++;

	int loclen = 0 ;
	char loclong[100001]={0};

	nrll = rand () % 20 ;

	pstmt = conn->createPreparedStatement();
	CE("select_and_check_superu_tab1sl: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
	{
        movep ( line ,col , "====== SELECT TAB1SL, nr = %d" , nrll ) ;

		returncode = checkPreparedStatement(pstmt, "SELECT lnll, clll FROM superu.tab1sl WHERE nrll = ? WITH LOCK ISOLATION LEVEL 1");
		CE("select_and_check_superu_tab1sl: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(long);

		returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &nrll, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in select_and_check_superu_tab1sl (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
        CE ("select_and_check_superu_tab1sl: SELECT TAB1SL" ,0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;

        if ( pstmt->ERRORCODE == 0 )
        {
            scl.len = 100000 ;

			rs = pstmt->getResultSet();
			while (rs->next() == SQLDBC_OK)
			{
				SQLDBC_Length l;
				returncode = rs->getObject(1, MY_HOSTTYPE_LONG, &lnll, &l, sizeof(lnll), true);
				if (returncode != SQLDBC_OK) CE("error with getObject in select_and_check_superu_tab1sl (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
				returncode = rs->getObject(2, SQLDBC_HOSTTYPE_ASCII, &scl.arr, &l, sizeof(scl.arr), true);
				if (returncode != SQLDBC_OK) CE("error with getObject in select_and_check_superu_tab1sl (col 2)", 1, rs->ERRORCODE, rs->ERRORTEXT);

				/* In error situations no 0 delimiter is returned by DB. */
				/* So first copy to local array, then perform strlen()  */
				memcpy ( loclong , scl.arr , 100000 ) ;
				loclen = (int) strlen ( scl.arr ) ;

				if ( loclen != lnll )
				{
					prot ( "=== Longlong Table TAB1SL ===\n"             ) ;
					prot ( "Illegal length : nr = %d\n" , nrll ) ;
					prot ( "stored length       = %d\n" , lnll ) ;
					prot ( "strlen (result)     = %d\n" , loclen   ) ;
				}
			}
			rs->close();
		}

	} while ( rollberr (1, pstmt->ERRORCODE) ) ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void  update_superu_tab1sl  ()
/*
	UPDATE superu.tabsl with randomized value
	COMMIT or ROLLBACK
*/
{
	function_called_counter++;

	nrll = rand () % 20 ;

	pstmt = conn->createPreparedStatement();
	CE("update_superu_tab1sl: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
	{
		movep ( line ,col , "====== UPDATE TAB1SL, nr = %d" , nrll ) ;

		lnll = 50000 + ( rand () % 6 ) * 10000 - 1 ;

		memset ( scl.arr , 's' , lnll ) ;
		scl.arr[0] = scl.arr[lnll-2] = 't' ;
		scl.arr[lnll-1]= (char) 0 ;
		scl.len = lnll = (int) strlen ( scl.arr ) ;

		returncode = checkPreparedStatement(pstmt, "UPDATE superu.tab1sl SET CLLL = ? , LNLL = ? WHERE  nrll = ?");
		CE("update_superu_tab1sl: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(long), indi2=SQLDBC_NTS;

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &scl.arr, &indi2, sizeof(scl.arr));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_superu_tab1sl (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, MY_HOSTTYPE_LONG, &lnll, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_superu_tab1sl (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, MY_HOSTTYPE_LONG, &nrll, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_superu_tab1sl (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
        CE ("update_superu_tab1sl: UPDATE TAB1SL" ,0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
	} while ( rollberr (1, pstmt->ERRORCODE) ) ;

	if ( pstmt->ERRORCODE == 0 )
	{
		if ( rand () % 2 == 0 )
			commit() ;
		else
			rollback();
	}
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void ins_except_tab1 ()
/*
	INSERT TAB1 with a nested SELECT query
	COMMIT
*/
{
	function_called_counter++;

	if ( rand () % 3 ) return ;
	do
	{
		movep ( line , col , "INSERT ... EXCEPT ... SUPERU.TAB1");
		returncode=stmt->execute("INSERT TAB1 SELECT * FROM "
			"(SELECT K1 , I1 , I2 , C1 , C2 , I3 FROM SUPERU.TAB1 EXCEPT "
			"SELECT K1 , I1 , I2 , C1 , C2 , I3 FROM TAB1) IGNORE DUPLICATES");
		CE ("ins_except_tab1: INSERT ... EXCEPT ... SUPERU.TAB1" , 0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
	} while ( rollberr (1, stmt->ERRORCODE) ) ;

	commit () ;
}
/*------------------------------------------------------------------------*/
void count_tab1 ()
/*
	count number of records of tab1 into "cnt"
	COMMIT
*/
{
	function_called_counter++;

	movep (line,col,"SELECT COUNT(*) FROM TAB1") ;

	pstmt = conn->createPreparedStatement();
	CE("count_tab1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
	{
		returncode = checkPreparedStatement(pstmt, "SELECT COUNT (*) INTO ? FROM TAB1");
		CE("count_tab1: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		SQLDBC_Length indi1=sizeof(cnt);
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &cnt, &indi1, sizeof(cnt));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in count_tab1 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
		CE ("count_tab1: SELECT COUNT(*) FROM TAB1",0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
	} while ( rollberr (1, pstmt->ERRORCODE) ) ;

	movep (line,col,"SELECT COUNT(*) FROM TAB1, cnt = %d", cnt) ;
	sleep (1) ;
	prot ( "SELECT COUNT(*) FROM TAB1, cnt = %d\n" , cnt ) ;
	commit () ;
}
/*------------------------------------------------------------------------*/
void delete_tab1 ()
/*
	DELETE randomized KEY rows
	COMMIT
*/
{
	function_called_counter++;

	k1 = rand () % rows ;

	movep ( line , col , "DELETE TAB1 , K1 = %d\r" , k1 ) ;

	pstmt = conn->createPreparedStatement();
	CE("delete_tab1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
	{
		returncode = checkPreparedStatement(pstmt, "DELETE TAB1 KEY K1 = ?");
		CE("delete_tab1: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		SQLDBC_Length indi1=sizeof(long);
		returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &k1, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in delete_tab1 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
		CE ( "delete_tab1: DELETE TAB1 KEY = :k1" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
	} while ( rollberr (1, pstmt->ERRORCODE) ) ;

	commit () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void sel_upd_sup ()
/*
	SELECT one randomized row
	try to UPDATE superu.tab1
*/
{
	function_called_counter++;

	k1 = rand () % rows ;

	movep (line,col,"SELECT FROM TAB1") ;

	pstmt = conn->createPreparedStatement();
	CE("sel_upd_sup: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
	{
		//SELECT ... INTO only possible because K1 is KEY of TAB1!!! otherwise it should be fetched with rs->getObject ...
		returncode = checkPreparedStatement(pstmt, "SELECT I1 , I2 , C1 , C2 INTO ?, ?, ?, ? FROM TAB1 WHERE K1 = ?");
		CE("sel_upd_sup: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(long);

		returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &i1, &i1ind, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_upd_sup (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, MY_HOSTTYPE_LONG, &i2, &i2ind, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_upd_sup (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_ASCII, &c1, &c1ind, sizeof(c1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_upd_sup (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &c2, &c2ind, sizeof(c2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_upd_sup (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (5, MY_HOSTTYPE_LONG, &k1, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_upd_sup (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();

		CE ("sel_upd_sup: SELECT TAB1" ,0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
	} while ( rollberr (0, pstmt->ERRORCODE) ) ;

	SE ("sel_upd_sup: SELECT DIRECT FROM TAB1" ,0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
	if (pstmt->ERRORCODE == 0 && c1ind == 0 && c2ind == 0 && i1ind == 0 && i2ind ==0)
	{
		movep (line,col,"==== UPDATE SUPERU.TAB1 , K1 = %d\r" , k1);

		pstmt2 = conn->createPreparedStatement();
		CE("sel_upd_sup: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
		do
		{
			returncode = checkPreparedStatement(pstmt2, "UPDATE SUPERU.TAB1 SET I1 = ?, I2 = ?, C1 = ?, C2 = ? WHERE K1 = ?");
			CE("sel_upd_sup: prepare", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);

			SQLDBC_Length indi1=sizeof(long), indi2=SQLDBC_NTS;

			returncode = pstmt2->bindParameter (1, MY_HOSTTYPE_LONG, &i1, &indi1, sizeof(long));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_upd_sup (col 6)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
			returncode = pstmt2->bindParameter (2, MY_HOSTTYPE_LONG, &i2, &indi1, sizeof(long));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_upd_sup (col 7)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
			returncode = pstmt2->bindParameter (3, SQLDBC_HOSTTYPE_ASCII, &c1, &indi2, sizeof(c1));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_upd_sup (col 8)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
			returncode = pstmt2->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &c2, &indi2, sizeof(c2));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_upd_sup (col 9)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
			returncode = pstmt2->bindParameter (5, MY_HOSTTYPE_LONG, &k1, &indi1, sizeof(long));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_upd_sup (col 10)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);

			returncode = pstmt2->execute();

			CE ("sel_upd_sup: UPDATE SUPERU.TAB1" ,0, pstmt2->ERRORCODE, pstmt2->ERRORTEXT) ;
		} while ( rollberr (1, pstmt2->ERRORCODE) ) ;

		if (pstmt2->ERRORCODE != 0)
			movep(line,col,"===== UPDATE SUPERU.TAB1 , ERR = %d\r" , pstmt2->ERRORCODE ) ;
		else
		{
			commit () ;
		}
		conn->releaseStatement(pstmt2);
		pstmt2 = 0;
	}

	conn->releaseStatement(pstmt);
	pstmt = 0;
	check_long_raw ( "UPDATE SUPERU.TAB1" , k1 , -1 , -1) ;
}
/*------------------------------------------------------------------------*/
void union_all_tab1 ()
/*
	large SELECT ... UNION ALL SELECT ... for randomized values
*/
{
	function_called_counter++;

	int u1 = rand () % rows ;
	int u2 = rand () % rows ;
	int u3 = rand () % rows ;
	int u4 = rand () % rows ;
	int u5 = rand () % rows ;
	int u6 = rand () % rows ;
	int u7 = rand () % rows ;
	int u8 = rand () % rows ;
	int u9 = rand () % rows ;
	int u10 = rand () % rows ;
	int u11 = rand () % rows ;
	int u12 = rand () % rows ;
	int u13 = rand () % rows ;
	int u14 = rand () % rows ;
	int u15 = rand () % rows ;
	int u16 = rand () % rows ;
	int u17 = rand () % rows ;
	int u18 = rand () % rows ;
	int u19 = rand () % rows ;
	int u20 = rand () % rows ;

	pstmt = conn->createPreparedStatement();
	CE("union_all_tab1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
	{
		movep (line,col,"SELECT ... FROM TAB1 UNION ALL ...") ;

		returncode = checkPreparedStatement(pstmt, "SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ? "
			"UNION ALL SELECT I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE K1 = ?");
		CE("union_all_tab1: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(u1), indi2=sizeof(u2), indi3=sizeof(u3), indi4=sizeof(u4), indi5=sizeof(u5),
			indi6=sizeof(u6), indi7=sizeof(u7), indi8=sizeof(u8), indi9=sizeof(u9), indi10=sizeof(u10),
			indi11=sizeof(u11), indi12=sizeof(u12), indi13=sizeof(u13), indi14=sizeof(u14), indi15=sizeof(u15),
			indi16=sizeof(u16), indi17=sizeof(u17), indi18=sizeof(u18), indi19=sizeof(u19), indi20=sizeof(u20);
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &u1, &indi1, sizeof(u1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &u2, &indi2, sizeof(u2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_INT4, &u3, &indi3, sizeof(u3));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_INT4, &u4, &indi4, sizeof(u4));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_INT4, &u5, &indi5, sizeof(u5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (6, SQLDBC_HOSTTYPE_INT4, &u6, &indi6, sizeof(u6));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 6)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (7, SQLDBC_HOSTTYPE_INT4, &u7, &indi7, sizeof(u7));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 7)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (8, SQLDBC_HOSTTYPE_INT4, &u8, &indi8, sizeof(u8));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 8)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (9, SQLDBC_HOSTTYPE_INT4, &u9, &indi9, sizeof(u9));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 9)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (10, SQLDBC_HOSTTYPE_INT4, &u10, &indi10, sizeof(u10));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 10)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (11, SQLDBC_HOSTTYPE_INT4, &u11, &indi11, sizeof(u11));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 11)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (12, SQLDBC_HOSTTYPE_INT4, &u12, &indi12, sizeof(u12));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 12)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (13, SQLDBC_HOSTTYPE_INT4, &u13, &indi13, sizeof(u13));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 13)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (14, SQLDBC_HOSTTYPE_INT4, &u14, &indi14, sizeof(u14));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 14)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (15, SQLDBC_HOSTTYPE_INT4, &u15, &indi15, sizeof(u15));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 15)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (16, SQLDBC_HOSTTYPE_INT4, &u16, &indi16, sizeof(u16));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 16)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (17, SQLDBC_HOSTTYPE_INT4, &u17, &indi17, sizeof(u17));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 17)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (18, SQLDBC_HOSTTYPE_INT4, &u18, &indi18, sizeof(u18));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 18)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (19, SQLDBC_HOSTTYPE_INT4, &u19, &indi19, sizeof(u19));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 19)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (20, SQLDBC_HOSTTYPE_INT4, &u20, &indi20, sizeof(u20));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 20)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (21, SQLDBC_HOSTTYPE_INT4, &u2, &indi2, sizeof(u2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 21)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (22, SQLDBC_HOSTTYPE_INT4, &u3, &indi3, sizeof(u3));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 22)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (23, SQLDBC_HOSTTYPE_INT4, &u4, &indi4, sizeof(u4));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 23)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (24, SQLDBC_HOSTTYPE_INT4, &u5, &indi5, sizeof(u5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 24)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (25, SQLDBC_HOSTTYPE_INT4, &u6, &indi6, sizeof(u6));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 25)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (26, SQLDBC_HOSTTYPE_INT4, &u7, &indi7, sizeof(u7));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 26)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (27, SQLDBC_HOSTTYPE_INT4, &u8, &indi8, sizeof(u8));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 27)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (28, SQLDBC_HOSTTYPE_INT4, &u9, &indi9, sizeof(u9));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 28)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (29, SQLDBC_HOSTTYPE_INT4, &u10, &indi10, sizeof(u10));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 29)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (30, SQLDBC_HOSTTYPE_INT4, &u11, &indi11, sizeof(u11));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 30)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (31, SQLDBC_HOSTTYPE_INT4, &u12, &indi12, sizeof(u12));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 31)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (32, SQLDBC_HOSTTYPE_INT4, &u13, &indi13, sizeof(u13));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 32)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (33, SQLDBC_HOSTTYPE_INT4, &u14, &indi14, sizeof(u14));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 33)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (34, SQLDBC_HOSTTYPE_INT4, &u15, &indi15, sizeof(u15));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 34)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (35, SQLDBC_HOSTTYPE_INT4, &u16, &indi16, sizeof(u16));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 35)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (36, SQLDBC_HOSTTYPE_INT4, &u17, &indi17, sizeof(u17));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 36)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (37, SQLDBC_HOSTTYPE_INT4, &u18, &indi18, sizeof(u18));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 37)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (38, SQLDBC_HOSTTYPE_INT4, &u19, &indi19, sizeof(u19));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 38)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (39, SQLDBC_HOSTTYPE_INT4, &u20, &indi20, sizeof(u20));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in union_all_tab1 (col 39)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
		CE ("union_all_tab1: SELECT ... FROM TAB1 UNION ALL ...",0, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		if ( pstmt->ERRORCODE == 0 )
		{
			rs = pstmt->getResultSet();
			while ( rs->next() == SQLDBC_OK )
			{
				SQLDBC_Length l;
				returncode = rs->getObject(1, MY_HOSTTYPE_LONG, &i1, &l, sizeof(i1), true);
				if (returncode != SQLDBC_OK) CE("error with getObject in union_all_tab1 (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
				returncode = rs->getObject(2, MY_HOSTTYPE_LONG, &i2, &l, sizeof(i2), true);
				if (returncode != SQLDBC_OK) CE("error with getObject in union_all_tab1 (col 2)", 1, rs->ERRORCODE, rs->ERRORTEXT);
				returncode = rs->getObject(3, SQLDBC_HOSTTYPE_ASCII, &c1, &l, sizeof(c1), true);
				if (returncode != SQLDBC_OK) CE("error with getObject in union_all_tab1 (col 3)", 1, rs->ERRORCODE, rs->ERRORTEXT);
				returncode = rs->getObject(4, SQLDBC_HOSTTYPE_ASCII, &c2, &l, sizeof(c2), true);
				if (returncode != SQLDBC_OK) CE("error with getObject in union_all_tab1 (col 4)", 1, rs->ERRORCODE, rs->ERRORTEXT);
				returncode = rs->getObject(5, MY_HOSTTYPE_LONG, &i3, &l, sizeof(i3), true);
				if (returncode != SQLDBC_OK) CE("error with getObject in union_all_tab1 (col 5)", 1, rs->ERRORCODE, rs->ERRORTEXT);
			}
			rs->close();
		}
		else if ( ROLLBERR (pstmt->ERRORCODE))
		{
			CE ("union_all_tab1: SELECT ... FROM TAB1 UNION ALL ...",0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
		}
		else if ( pstmt->ERRORCODE != 100 )
		{
			CE ("union_all_tab1: SELECT ... FROM TAB1 UNION ALL ...",1, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
		}
	} while ( rollberr (1, pstmt->ERRORCODE) ) ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void sel_ins_sup ()
/*
	SELECT one randomized row
	try to INSERT superu.tab1
*/
{
	function_called_counter++;

	k1 = rand () % rows ;

	movep (line,col,"SELECT DIRECT FROM TAB1" ) ;

	cll.len = 17000 + 2 ;

	pstmt = conn->createPreparedStatement();
	CE("sel_ins_sup: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

	//SELECT ... INTO only possible because K1 is KEY of TAB1!!! otherwise it should be fetched with rs->getObject ...
	returncode = checkPreparedStatement(pstmt, "SELECT  I1 , I2 , C1 , C2 , I3	INTO ?, ?, ?, ?, ? FROM TAB1 WHERE  K1 = ?");
	CE("sel_ins_sup: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

	SQLDBC_Length indi1=sizeof(long);

	returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &i1, &i1ind, sizeof(long));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_ins_sup (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (2, MY_HOSTTYPE_LONG, &i2, &i2ind, sizeof(long));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_ins_sup (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_ASCII, &c1, &c1ind, sizeof(c1));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_ins_sup (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &c2, &c2ind, sizeof(c2));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_ins_sup (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (5, MY_HOSTTYPE_LONG, &i3, &i3ind, sizeof(long));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_ins_sup (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (6, MY_HOSTTYPE_LONG, &k1, &indi1, sizeof(long));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_ins_sup (col 6)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

	returncode = pstmt->execute();
	CE ("sel_ins_sup: SELECT FROM TAB1" ,0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
	SE ("sel_ins_sup: SELECT FROM TAB1" ,0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;

	if ( pstmt->ERRORCODE == 0 && c1ind == 0 && c2ind == 0 && i1ind == 0
		&& i2ind == 0 && i3ind == 0 && icllen == 0 )
	{
		movep (line,col,"==== INSERT SUPERU.TAB1 , K1 = %d\r" , k1);

		cll.len = rand () % maxlongraw + 4 ;
		cllen = cll.len ;
		memset ( cll.arr , '1' , cll.len ) ;

		memset ( c6 , 'y' , 1500 ) ;
		c6 [(rand()%1250)+250] = 0 ;

		cll.arr[0] = ( cll.len >> 8 ) & 0x7f  ;
		cll.arr[1] = cll.len & 0x7f ;
		cll.arr[2] = '+' ;
		cll.arr[cll.len-1] = '+' ;
		pstmt2 = conn->createPreparedStatement();
		CE("sel_ins_sup: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

		returncode = checkPreparedStatement(pstmt2, "INSERT SUPERU.TAB1 "
				"SET K0 = ?, K1 = ? , "
				"I1 = ?, I2 = ?, C1 = ?, C2 = ?, "
				"I3 = ? , CLLEN = ?, CF0 = ? , CLB = ?");
		CE("sel_ins_sup: prepare", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(long), indi2=SQLDBC_NTS, indi3=sizeof(cllen), indi4=cll.len;

		returncode = pstmt2->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &k0, &indi2, sizeof(k0));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_ins_sup (col 7)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
		returncode = pstmt2->bindParameter (2, MY_HOSTTYPE_LONG, &k1, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_ins_sup (col 8)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
		returncode = pstmt2->bindParameter (3, MY_HOSTTYPE_LONG, &i1, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_ins_sup (col 9)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
		returncode = pstmt2->bindParameter (4, MY_HOSTTYPE_LONG, &i2, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_ins_sup (col 10)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
		returncode = pstmt2->bindParameter (5, SQLDBC_HOSTTYPE_ASCII, &c1, &indi2, sizeof(c1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_ins_sup (col 11)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
		returncode = pstmt2->bindParameter (6, SQLDBC_HOSTTYPE_ASCII, &c2, &indi2, sizeof(c2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_ins_sup (col 12)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
		returncode = pstmt2->bindParameter (7, MY_HOSTTYPE_LONG, &i3, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_ins_sup (col 13)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
		returncode = pstmt2->bindParameter (8, SQLDBC_HOSTTYPE_INT4, &cllen, &indi3, sizeof(cllen));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_ins_sup (col 14)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
		returncode = pstmt2->bindParameter (9, SQLDBC_HOSTTYPE_ASCII, &c6, &indi2, sizeof(c6));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_ins_sup (col 15)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
		returncode = pstmt2->bindParameter (10, SQLDBC_HOSTTYPE_ASCII, &cll.arr, &indi4, sizeof(cll.arr));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_ins_sup (col 16)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);

		returncode = pstmt2->execute();

		CE ("sel_ins_sup: INSERT SUPERU.TAB1",0, pstmt2->ERRORCODE, pstmt2->ERRORTEXT) ;

		if ( pstmt2->ERRORCODE != 0 )
		{
			movep (line,col,"===== INSERT SUPERU.TAB1 , ERR = %d\r" , pstmt2->ERRORCODE ) ;
			rollback () ;
		}
		else
		{
			if ( rand () % 3 )
				commit () ;
			else
				rollback () ;
		}
	}
	conn->releaseStatement(pstmt);
	pstmt = 0;
	check_long_raw ( "INSERT SUPERU.TAB1" , k1 , -1 , -1) ;
}
/*------------------------------------------------------------------------*/
void check_long_raw (const char *c, int k1, int i1, int i2)
/*
	various SELECT ... WITH LOCK ISOLATION LEVEL 1 depending on function-parameters
	fetch through ResultSet and check values
	in case of discrepancies stop kernel (sqladump)
*/
{
	function_called_counter++;

	int	kk1 = 0;
	int	ii1 = 0;
	int	ii2 = 0;
	int ok = 0;

	if ( check_longs == 0 ) return ;

	movep (line,col,"===== CHECK long raw >%s<" ,c) ;

	cll.len = MAXLONGRAW ;

	pstmt = conn->createPreparedStatement();
	CE("check_long_raw: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

	for (int i = 0; i < 3; i++)
	{
		if ( k1 >= 0 )
		{
			kk1 = k1 ;

			returncode = checkPreparedStatement(pstmt, "SELECT K1, CLLEN, CLB FROM superu.TAB1 WHERE K1 = ? WITH LOCK ISOLATION LEVEL 1");

			CE("check_long_raw: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

			SQLDBC_Length indi1=sizeof(kk1);

			returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &kk1, &indi1, sizeof(kk1));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in check_long_raw (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

			returncode = pstmt->execute();

			SE ("check_long_raw: SELECT CLLEN , CLB FROM TAB1, K1 =" ,0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
			if (pstmt->ERRORCODE == 0) {ok = 1; break;}
		}
		else
		{
			ii1 = i1 ;
			ii2 = i2 ;

			returncode = checkPreparedStatement(pstmt, "SELECT K1, CLLEN, CLB FROM superu.TAB1	WHERE I1 BETWEEN ? AND ? WITH LOCK ISOLATION LEVEL 1");

			CE("check_long_raw: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

			SQLDBC_Length indi1=sizeof(ii1), indi2=sizeof(ii2);

			returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &ii1, &indi1, sizeof(ii1));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in check_long_raw (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
			returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &ii2, &indi2, sizeof(ii2));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in check_long_raw (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

			returncode = pstmt->execute();
			SE ("check_long_raw: SELECT CLLEN , CLB FROM TAB1, BETWEEN ..." ,0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
			if (pstmt->ERRORCODE == 0) {ok = 1; break;}
		}
	}

	if (ok == 1)
	{
		rs = pstmt->getResultSet();

		while ( rs->next() == SQLDBC_OK )
		{
			cll.len = MAXLONGRAW ;

			SQLDBC_Length l;
			returncode = rs->getObject(1, SQLDBC_HOSTTYPE_INT4, &kk1, &l, sizeof(kk1), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in check_long_raw (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
			returncode = rs->getObject(2, SQLDBC_HOSTTYPE_INT4, &cllen, &icllen, sizeof(cllen), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in check_long_raw (col 2)", 1, rs->ERRORCODE, rs->ERRORTEXT);
			returncode = rs->getObject(3, SQLDBC_HOSTTYPE_ASCII, &cll.arr, &cll.len, sizeof(cll.arr), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in check_long_raw (col 3)", 1, rs->ERRORCODE, rs->ERRORTEXT);

			if ( icllen == 4 )
			{
				if ( cll.len != cllen )
				{
					prot ( "=== %s ===\n" 		    , c ) ;
					prot ( "Illegal length : k1 = %d\n" , kk1 ) ;
					prot ( "cll.len             = %d\n" , cll.len ) ;
					prot ( "cllen               = %d\n" , cllen ) ;
					if ( DumpOnLongCheck )
					{
						Sqldump ("check_long_raw()", 444, "Error with cll.len and cllen!", conn) ;
					}
				}
				else if  ( cllen > 1000 )
				{
					int	i = 0;

					if ( cll.arr [2] != cll.arr[cllen-1] )
					{
						prot ( "=== %s ===\n" 		    , c ) ;
						prot ( "Illegal start/endmark of long raw : k1 = %d\n" , kk1 ) ;
						if ( DumpOnLongCheck )
						{
							Sqldump ("check_long_raw()", 444, "Error with cll.arr[2]!", conn) ;
						}
					}

					for ( i = 3 ; i < cllen - 2 ; i++ )
					{
						if ( cll.arr[3] != cll.arr[i] )
						{
							prot ( "=== %s ===\n" 		    , c ) ;
							prot ( "Illegal char in long raw : k1 = %d\n" , kk1 ) ;
	        				if ( DumpOnLongCheck )
							{
								Sqldump ("check_long_raw()", 444, "Error with cll.arr[3]!", conn) ;
							}
						}
					}
				}
			}
		}
		rs->close();
	}
	else
	{
		prot("check_long_raw not executed!\n");
	}
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void check_lr_su ()
/*
	SELECT one row FROM superu.tab1
	try to check values
	in case of discrepancies stop kernel (sqladump)
*/
{
	function_called_counter++;

	k1 = rand () % rows ;

	movep (line,col,"===== SELECT NEXT long raw FROM TAB1" ) ;

	cll.len = MAXLONGRAW ;

	pstmt = conn->createPreparedStatement();
	CE("check_lr_su: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	//SELECT ... INTO only possible because K1 and K0 are KEY of superu.TAB1!!! otherwise it should be fetched with rs->getObject ...
	returncode = checkPreparedStatement(pstmt, "SELECT CLLEN , CLB INTO ?, ? FROM superu.TAB1 WHERE K1 = ? AND K0 = ?");
	CE("check_lr_su: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

	SQLDBC_Length indi1=sizeof(long), indi2=SQLDBC_NTS;

	returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &cllen, &icllen, sizeof(cllen));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in check_lr_su (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_ASCII, &cll.arr, &cll.len, sizeof(cll.arr));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in check_lr_su (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (3, MY_HOSTTYPE_LONG, &k1, &indi1, sizeof(long));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in check_lr_su (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &k0, &indi2, sizeof(k0));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in check_lr_su (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

	returncode = pstmt->execute();
	CE ("check_lr_su: SELECT DIRECT CLLEN , CLB FROM TAB1" ,0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
	SE ("check_lr_su: SELECT DIRECT CLLEN , CLB FROM TAB1" ,0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;

	if ( returncode == SQLDBC_OK && icllen == 4 )
	{
		movep (line,col,"===== CHECK long raw SUPERU.TAB1 , K1 = %d\r" , k1);

		if ( cll.len != cllen )
		{
			prot ( "Illegal length : k1 = %d\n" , k1 ) ;
			prot ( "cll.len             = %d\n" , cll.len ) ;
			prot ( "cllen               = %d\n" , cllen ) ;
			if ( DumpOnLongCheck )
			{
				Sqldump ("check_lr_su()", pstmt->ERRORCODE, pstmt->ERRORTEXT, conn) ;
			}
		}
		else if  ( cllen > 4 )
		{
			int	i ;

			if ( ( cllen > 1000 ) && ( cll.arr [2] != cll.arr[cllen-1]) )
			{
				prot ( "Illegal start/endmark of long raw : k1 = %d\n" , k1 ) ;
				if ( DumpOnLongCheck )
				{
					Sqldump ("check_lr_su()", pstmt->ERRORCODE, pstmt->ERRORTEXT, conn) ;
				}
			}

			for ( i = 3 ; i < cllen-1 ; i++ )
			{
				if ( cll.arr[3] != cll.arr[i] )
				{
					prot ( "Illegal char in long raw : k1 = %d\n" , k1 ) ;
					if ( DumpOnLongCheck )
					{
						Sqldump ("check_lr_su()", pstmt->ERRORCODE, pstmt->ERRORTEXT, conn) ;
					}
				}
			}
		}
	}
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void del_sup ()
/*
	DELETE from superu.tab1 manual or by DBProcedure MUTDELSUPTAB1
	COMMIT
*/
{
	function_called_counter++;

 	int udb = 0;
	k1 = rand () % rows ;

	udb = ( DbProcUsed && ( rand () % 2 ) ) ;

	if ( udb )
		movep ( line , col , "===== DELETE SUPERU.TAB1 , K1 = %d (DBPROC)\r" , k1 ) ;
	else
		movep ( line , col , "===== DELETE SUPERU.TAB1 , K1 = %d\r" , k1 ) ;

	pstmt = conn->createPreparedStatement();
	CE("del_sup: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
	{
		if ( udb )
		{
			returncode = checkPreparedStatement(pstmt, "CALL MUTDELSUPTAB1(?)");
			CE("del_sup: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

			SQLDBC_Length indi1=sizeof(long);

			returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &k1, &indi1, sizeof(long));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in del_sup (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

			returncode = pstmt->execute();
			CE ( "del_sup: ===== DELETE SUPERU.TAB1 (DBPROC)" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
		}
		else
		{
			returncode = checkPreparedStatement(pstmt, "DELETE SUPERU.TAB1 WHERE K1 = ?");
			CE("del_sup: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

			SQLDBC_Length indi1=sizeof(long);

			returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &k1, &indi1, sizeof(long));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in del_sup (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

			returncode = pstmt->execute();
			CE ( "del_sup: ===== DELETE SUPERU.TAB1" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
		}

	} while ( rollberr (1, pstmt->ERRORCODE) ) ;

	commit () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void update_stat_sample ()
/*
	UPDATE STATISTICS TAB1 ... SAMPLE
	COMMIT
*/
{
	function_called_counter++;

	movep (line,col,"UPDATE STATISTICS ... SAMPLE \n") ;
	do
	{
		returncode = stmt->execute("UPDATE STATISTICS TAB1 ESTIMATE SAMPLE 1000 ROWS");
		CE("update_stat_sample: UPDATE STATISTICS ... SAMPLE", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (2, stmt->ERRORCODE) ) ;

	CE ("update_stat_sample: UPDATE STATISTICS ... SAMPLE" , 1, stmt->ERRORCODE, stmt->ERRORTEXT ) ;

	commit () ;

	movep (line,col,"UPDATE STATISTICS COLUMN ... SAMPLE \n") ;
	do
	{
		returncode = stmt->execute("UPDATE STATISTICS COLUMN (*) FOR TAB1 ESTIMATE SAMPLE 1000 ROWS");
		CE("update_stat_sample: UPDATE STATISTICS COLUMN ... SAMPLE", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (2, stmt->ERRORCODE) ) ;

	CE ("update_stat_sample: UPDATE STATISTICS COLUMN ... SAMPLE" , 1, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
	commit () ;
}
/*------------------------------------------------------------------------*/
void update_stat ()
/*
	UPDATE STATSTICS
	COMMIT
*/
{
	function_called_counter++;

	movep (line,col,"UPDATE STATISTICS *\n") ;
	do
	{
		returncode=stmt->execute("UPDATE STATISTICS *");
		CE ("update_stat: UPDATE STATISTICS *" , 0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
	} while ( rollberr (2, stmt->ERRORCODE) ) ;

	CE ("update_stat: UPDATE STATISTICS *" , 1, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
	commit () ;
}
/*------------------------------------------------------------------------*/
void del_avg_tab1 (int rollback_request)
/*
	DELETE FROM TAB1 with SELECT FROM PUBVIEW (view)
	COMMIT or ROLLBACK
*/
{
	function_called_counter++;

	if ( rand () % 2 ) return ;

	movep ( line , col , "DELETE TAB1 WHERE ... SELECT AVG ()" ) ;
	pstmt = conn->createPreparedStatement();
	CE("del_avg_tab1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

	do
	{
		returncode = checkPreparedStatement(pstmt, "DELETE TAB1 WHERE ( ? + ? ) > ( SELECT AVG ( ? + ? ) FROM PUBVIEW )");
		CE("del_avg_tab1: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		SQLDBC_Length indi1=sizeof(long);
		returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &i1, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in del_avg_tab1 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, MY_HOSTTYPE_LONG, &i2, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in del_avg_tab1 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, MY_HOSTTYPE_LONG, &i1, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in del_avg_tab1 (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, MY_HOSTTYPE_LONG, &i2, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in del_avg_tab1 (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
		CE("del_avg_tab1: DELETE TAB1 WHERE ... SELECT AVG ()", 0, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		SE("del_avg_tab1: DELETE TAB1 WHERE ... SELECT AVG ()", 0, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	} while ( rollberr (1, pstmt->ERRORCODE) ) ;
	if ( rollback_request )
	{
		CE ( "del_avg_tab1: DELETE TAB1 WHERE ... SELECT AVG : ROLLBACK (ok)",0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
		rollback () ;
	}
	else
	{
		commit () ;
	}
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void ins_tabu2 ()
/*
	INSERT TABU14 with randomized values
	COMMIT or ROLLBACK
*/
{
	function_called_counter++;

	k1 = rand () % rows ;
	i1 = rand () % rows ;
	i2 = rand () % rows ;
	i3 = ( rand () % rows ) / 10 ;
	i4 = ( rand () % rows ) / 100 ;
	sprintf ( c1 , "ins_%s_%d" , id , i1 ) ;
	sprintf ( c2 , "ins_%s_%d" , id , i2 / 10 ) ;
	sprintf ( c3 , "abu_%s_%d" , id , i2 / 10 ) ;

	pstmt = conn->createPreparedStatement();
	CE("ins_tabu2: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
	{
		movep ( line , col ,"===== INSERT TABU14" ) ;

		returncode = checkPreparedStatement(pstmt, "INSERT TABU14 VALUES ( ?, ?, ?, ?, ?, ?, ?, ?)");
		CE("ins_tabu2: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(long), indi2=SQLDBC_NTS;

		returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &k1, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_tabu2 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, MY_HOSTTYPE_LONG, &i1, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_tabu2 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, MY_HOSTTYPE_LONG, &i2, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_tabu2 (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &c1, &indi2, sizeof(c1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_tabu2 (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_ASCII, &c2, &indi2, sizeof(c2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_tabu2 (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (6, MY_HOSTTYPE_LONG, &i3, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_tabu2 (col 6)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (7, MY_HOSTTYPE_LONG, &i4, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_tabu2 (col 7)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (8, SQLDBC_HOSTTYPE_ASCII, &c3, &indi2, sizeof(c3));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_tabu2 (col 8)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
		if ( pstmt->ERRORCODE != 360 )
			CE ("ins_tabu2: INSERT TABU14",0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
	} while ( rollberr (1, pstmt->ERRORCODE) ) ;

	if ( pstmt->ERRORCODE == 0 )
		commit () ;
	else
		rollback () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void upd_tabv1 ()
/*
	SELECT ... FROM TABV1 (view)
	SELECT ... TAB1 with previously selected value
	UPDATE ... TABV1 (view) with previously selected value
*/
{
	function_called_counter++;

	nr = rand () % rows ;

	pstmt = conn->createPreparedStatement();
	CE("upd_tabv1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
	{
		returncode = checkPreparedStatement(pstmt, "SELECT K1 , I1 FROM TABV1 WHERE K1 = ?");
		CE("upd_tabv1: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(long);

		returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &nr, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in upd_tabv1 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
		CE ("upd_tabv1: SELECT K1 , I1 FROM TABV1 (upd_tabv1)" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
	} while ( rollberr (2, pstmt->ERRORCODE) ) ;

	if ( pstmt->ERRORCODE == 0 )
	{
		rs = pstmt->getResultSet();
		while ( rs->next() == SQLDBC_OK )
		{
			SQLDBC_Length l;
			returncode = rs->getObject(1, MY_HOSTTYPE_LONG, &k1, &l, sizeof(k1), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in upd_tabv1 (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
			returncode = rs->getObject(1, MY_HOSTTYPE_LONG, &i1, &l, sizeof(i1), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in upd_tabv1 (col 2)", 1, rs->ERRORCODE, rs->ERRORTEXT);
		}
		movep ( line , col ,"SELECT DIRECT I3 " ) ;
		pstmt2 = conn->createPreparedStatement();
		CE("upd_tabv1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

		returncode = checkPreparedStatement(pstmt2, "SELECT I3 INTO ? FROM TAB1 WHERE K1 = ?");
		CE("upd_tabv1: prepare", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(long);

		returncode = pstmt2->bindParameter (1, MY_HOSTTYPE_LONG, &i3, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in upd_tabv1 (col 2)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
		returncode = pstmt2->bindParameter (2, MY_HOSTTYPE_LONG, &k1, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in upd_tabv1 (col 3)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);

		returncode = pstmt2->execute();
		CE ("upd_tabv1: SELECT DIRECT I3 ",0, pstmt2->ERRORCODE, pstmt2->ERRORTEXT) ;
		SE ("upd_tabv1: SELECT DIRECT I3 ",0, pstmt2->ERRORCODE, pstmt2->ERRORTEXT) ;

		if (pstmt2->ERRORCODE == 0)
		{
		    movep ( line , col ,"UPDATE TABV1" ) ;

			pstmt3 = conn->createPreparedStatement();
			CE("upd_tabv1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
			do
			{
				returncode = checkPreparedStatement(pstmt3, "UPDATE TABV1 SET I1 = 100 WHERE I3 = ?");
				CE("upd_tabv1: prepare", 1, pstmt3->ERRORCODE, pstmt3->ERRORTEXT);

				SQLDBC_Length indi1=sizeof(long);

				returncode = pstmt3->bindParameter (1, MY_HOSTTYPE_LONG, &i3, &indi1, sizeof(long));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in upd_tabv1 (col 4)", 1, pstmt3->ERRORCODE, pstmt3->ERRORTEXT);

				returncode = pstmt3->execute();
				CE ("upd_tabv1: UPDATE TABV1",0, pstmt3->ERRORCODE, pstmt3->ERRORTEXT) ;
			} while ( rollberr (1, pstmt3->ERRORCODE) ) ;

			commit () ;

			conn->releaseStatement(pstmt3);
			pstmt3 = 0;
		}
		else
			rollback () ;

		conn->releaseStatement(pstmt2);
		pstmt2 = 0;
		rs->close();
	}
	else
		CE ("upd_tabv1: SELECT TABV1",0, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void sel_distinct_tab1 ()
/*
	SELECT DISTINCT FROM superu.tab1
*/
{
	function_called_counter++;

	sprintf ( c1 , "%%%s%%" , id ) ;

	movep ( line , col , "===== SELECT DISTINCT C1,I3 FROM SUPERU.TAB1");

	pstmt = conn->createPreparedStatement();
	CE("sel_distinct_tab1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	returncode = checkPreparedStatement(pstmt, "SELECT DISTINCT C1, I3 FROM SUPERU.TAB1 WHERE C1 LIKE ?");
	CE("sel_distinct_tab1: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

	SQLDBC_Length indi2=SQLDBC_NTS;

	returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &c1, &indi2, sizeof(c1));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_distinct_tab1 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

	returncode = pstmt->execute();
	CE ( "sel_distinct_tab1: SELECT DISTINCT C1,I3 FROM SUPERU.TAB1",0, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	SE ( "sel_distinct_tab1: SELECT DISTINCT C1,I3 FROM SUPERU.TAB1",0, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void sel_temp_inv_tab1 ()
/*
	SELECT ... FROM superu.tab1 with randomized values
	fetch through ResultSet
*/
{
	function_called_counter++;

	char k00 [240]={0};
	long k01=0,i01=0;

	nr = rand () % rows ;

	movep ( line , col , "===== SELECT temp inv FROM SUPERU.TAB1");

	pstmt = conn->createPreparedStatement();
	CE("sel_temp_inv_tab1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
	{
		returncode = checkPreparedStatement(pstmt, "SELECT K0, K1, I1 FROM SUPERU.TAB1 WHERE K1 BETWEEN ? and (? + 100) "
			"ORDER BY K0, K1");
		CE("sel_temp_inv_tab1: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(long);

		returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &nr, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_temp_inv_tab1 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, MY_HOSTTYPE_LONG, &nr, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_temp_inv_tab1 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
	} while ( rollberr (1, pstmt->ERRORCODE) ) ;

	CE ( "sel_temp_inv_tab1: SELECT temp inv FROM SUPERU.TAB1",0, pstmt->ERRORCODE, pstmt->ERRORTEXT);

	rs = pstmt->getResultSet();
	while ( rs->next() == SQLDBC_OK )
	{
		SQLDBC_Length l;

		returncode = rs->getObject(1, SQLDBC_HOSTTYPE_ASCII, &k00, &l, sizeof(k00), false);	//true sorgt für 'trunc' (RC=2)
		if (returncode != SQLDBC_OK) CE("error with getObject in sel_temp_inv_tab1 (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
		returncode = rs->getObject(2, MY_HOSTTYPE_LONG, &k01, &l, sizeof(k01), true);
		if (returncode != SQLDBC_OK) CE("error with getObject in sel_temp_inv_tab1 (col 2)", 1, rs->ERRORCODE, rs->ERRORTEXT);
		returncode = rs->getObject(3, MY_HOSTTYPE_LONG, &i01, &l, sizeof(i01), true);
		if (returncode != SQLDBC_OK) CE("error with getObject in sel_temp_inv_tab1 (col 3)", 1, rs->ERRORCODE, rs->ERRORTEXT);
	}
	rs->close();
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void sel_first_stab1_ins_tab1 ()
/*
	SELECT one row FROM superu.tab1 until successful execution
	INSERT selected values in tab1
*/
{
	function_called_counter++;

    nr = rand () % rows ;

    movep (line,col,"===== SELECT ... FROM SUPERU.TAB1 WHERE ... ROWNO <= 1");

	pstmt = conn->createPreparedStatement();
	CE("sel_first_stab1_ins_tab1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
    do
    {
		returncode = checkPreparedStatement(pstmt, "SELECT K1 , I1 , I2 , C1 , C2 , I3 INTO  ?, ?, ?, ?, ?, ?"
			"FROM SUPERU.TAB1 WHERE I1  > ? AND ROWNO <= 1");
		CE("sel_first_stab1_ins_tab1: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(long), indi2=SQLDBC_NTS;

		returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &k1, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_first_stab1_ins_tab1 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, MY_HOSTTYPE_LONG, &i1, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_first_stab1_ins_tab1 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, MY_HOSTTYPE_LONG, &i2, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_first_stab1_ins_tab1 (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &c1, &indi2, sizeof(c1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_first_stab1_ins_tab1 (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_ASCII, &c2, &indi2, sizeof(c2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_first_stab1_ins_tab1 (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (6, MY_HOSTTYPE_LONG, &i3, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_first_stab1_ins_tab1 (col 6)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (7, MY_HOSTTYPE_LONG, &nr, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_first_stab1_ins_tab1 (col 7)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
		CE ( "sel_first_stab1_ins_tab1: SELECT ... FROM SUPERU.TAB1 WHERE ... ROWNO <= 1" ,0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
    } while ( rollberr (1, pstmt->ERRORCODE) ) ;

    SE ( "sel_first_stab1_ins_tab1: SELECT ... FROM SUPERU.TAB1 WHERE ... ROWNO <= 1" ,0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
    if ( pstmt->ERRORCODE != 100 )
    {
		movep ( line , col , "INSERT TAB1 ... " ) ;

		pstmt2 = conn->createPreparedStatement();
		CE("sel_first_stab1_ins_tab1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

		do
		{
			returncode = checkPreparedStatement(pstmt2, "INSERT TAB1 VALUES ( ?, ?, ?, ?, ?, ?)");
			CE("sel_first_stab1_ins_tab1: prepare", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);

			SQLDBC_Length indi1=sizeof(long), indi2=SQLDBC_NTS;

			returncode = pstmt2->bindParameter (1, MY_HOSTTYPE_LONG, &k1, &indi1, sizeof(long));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_first_stab1_ins_tab1 (col 8)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
			returncode = pstmt2->bindParameter (2, MY_HOSTTYPE_LONG, &i1, &indi1, sizeof(long));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_first_stab1_ins_tab1 (col 9)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
			returncode = pstmt2->bindParameter (3, MY_HOSTTYPE_LONG, &i2, &indi1, sizeof(long));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_first_stab1_ins_tab1 (col 10)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
			returncode = pstmt2->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &c1, &indi2, sizeof(c1));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_first_stab1_ins_tab1 (col 11)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
			returncode = pstmt2->bindParameter (5, SQLDBC_HOSTTYPE_ASCII, &c2, &indi2, sizeof(c2));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_first_stab1_ins_tab1 (col 12)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
			returncode = pstmt2->bindParameter (6, MY_HOSTTYPE_LONG, &i3, &indi1, sizeof(long));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in sel_first_stab1_ins_tab1 (col 13)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);

			returncode = pstmt2->execute();
			CE ("sel_first_stab1_ins_tab1: INSERT TAB1" , 0, pstmt2->ERRORCODE, pstmt2->ERRORTEXT ) ;
		} while ( rollberr (0, pstmt2->ERRORCODE) ) ;

		commit () ;
		conn->releaseStatement(pstmt2);
		pstmt2 = 0;
    }
    else
    {
		rollback () ;
    }
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void ins_stab1_sel_first_tab1 ()
/*
	INSERT from tab1 selected values INTO superu.tab1 until successful execution
	COMMIT
*/
{
	function_called_counter++;

    nr = rand () % rows ;

    movep(line,col,"===== INSERT SUPERU.TAB1 SELECT ... UPDATE DUPL.");

	pstmt = conn->createPreparedStatement();
	CE("ins_stab1_sel_first_tab1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
    {
		returncode = checkPreparedStatement(pstmt, "INSERT INTO SUPERU.TAB1 ( K0 , K1 , I1 , I2 , C1 , C2 , I3 ) "
			"SELECT ? , K1 , I1 , I2 , C1 , C2 , I3 FROM TAB1 WHERE "
			"K1 = ( SELECT MIN (K1) FROM TAB1 WHERE I1 > ? ) UPDATE DUPLICATES");
		CE("ins_stab1_sel_first_tab1: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(long), indi2=SQLDBC_NTS;

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &k0, &indi2, sizeof(k0));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_stab1_sel_first_tab1 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, MY_HOSTTYPE_LONG, &nr, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in ins_stab1_sel_first_tab1 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
		CE ( "ins_stab1_sel_first_tab1: ===== INSERT SUPERU.TAB1 SELECT ... UPDATE DUPL.",0, pstmt->ERRORCODE, pstmt->ERRORTEXT);
    }	while ( rollberr (1, pstmt->ERRORCODE) ) ;

    commit () ;

	conn->releaseStatement(pstmt);
	pstmt = 0;
    check_long_raw ( "INSERT SUPERU.TAB1 SELECT ..." , -1 , 1  , rows ) ;
}
/*------------------------------------------------------------------------*/
void complex_select ()
/*
	SELECT FROM superu.tab1 with complex nested query until successful execution
	COMMIT
*/
{
	function_called_counter++;

    nr = rand () % rows ;

	pstmt = conn->createPreparedStatement();
	CE("complex_select: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	if ( rand () % 2 )
    {
		movep (line,col,"===== SELECT FROM SUPERU.TAB1 WHERE ... SELECT ... AND") ;
		do
		{
			returncode = checkPreparedStatement(pstmt, "SELECT K1 , I1 FROM SUPERU.TAB1 "
				"WHERE ( K1 > ( SELECT MIN (K1) "
						"FROM TAB1 "
						"WHERE K1 > ? AND I3 IS NOT NULL ) "
					"AND I1 > ( SELECT AVG (I1) "
						"FROM PUBVIEW "
						"WHERE PUBVIEW.K1 > ? ))");
			CE("complex_select: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

			SQLDBC_Length indi1=sizeof(long);

			returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &nr, &indi1, sizeof(long));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in complex_select (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
			returncode = pstmt->bindParameter (2, MY_HOSTTYPE_LONG, &nr, &indi1, sizeof(long));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in complex_select (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

			returncode = pstmt->execute();
			CE ("complex_select: ===== SELECT FROM SUPERU.TAB1 WHERE ... SELECT ... AND" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
		} while ( rollberr (1, pstmt->ERRORCODE) ) ;
    }
    else
    {
		movep (line,col,"===== SELECT FROM PUBVIEW WHERE ... SELECT ... AND") ;
		do
		{
			returncode = checkPreparedStatement(pstmt, "SELECT K1, I1, I3 FROM PUBVIEW "
				"WHERE ( K1 > ( SELECT MIN (K1) "
						"FROM TAB1 WHERE K1 > ? AND I3 IS NOT NULL "
					"AND I1 > ( SELECT AVG (I1) "
						"FROM TAB1 WHERE ( K1 BETWEEN ? AND 99999 ) AND C1 IS NOT NULL ))) "
				"AND I1 IS NOT NULL");
			CE("complex_select: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

			SQLDBC_Length indi1=sizeof(long);

			returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &nr, &indi1, sizeof(long));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in complex_select (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
			returncode = pstmt->bindParameter (2, MY_HOSTTYPE_LONG, &nr, &indi1, sizeof(long));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in complex_select (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

			returncode = pstmt->execute();
			CE ("complex_select: ===== SELECT FROM PUBVIEW WHERE ... SELECT ... AND" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
		} while ( rollberr (1, pstmt->ERRORCODE) ) ;
	}
    commit () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void between_sbview ()
/*
	count number of records of superu.sbview_i_p into "cnt" with randomized "between"-condition
	COMMIT
*/
{
	function_called_counter++;

    int v1 = ( rand () % rows ) / 10 ;
    int v2 = v1 + 10 ;

	pstmt = conn->createPreparedStatement();
	CE("between_sbview: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
    {
        movep ( line , col , "SELECT count (*) INTO :cnt FROM SUPERU.SBVIEW_I_P, BETWEEN");

		returncode = checkPreparedStatement(pstmt, "SELECT count (*) INTO ? FROM SUPERU.SBVIEW_I_P WHERE I3 BETWEEN ? and ?");
		CE("between_sbview: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(cnt), indi3=sizeof(v1), indi4=sizeof(v2);

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &cnt, &indi1, sizeof(cnt));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in between_sbview (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &v1, &indi3, sizeof(v1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in between_sbview (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_INT4, &v2, &indi4, sizeof(v2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in between_sbview (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
        CE ( "between_sbview: SELECT count (*) INTO :cnt FROM SUPERU.SBVIEW_I_P, BETWEEN",0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
    } while ( rollberr (1, pstmt->ERRORCODE) ) ;

    commit () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void equal_sbview ()
/*
	count number of records of superu.sbview_i_p into "cnt" with randomized "equal"-condition
	COMMIT
*/
{
	function_called_counter++;

	int v1 = ( rand () % rows )  ;

	pstmt = conn->createPreparedStatement();
	CE("equal_sbview: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
    {
		movep ( line , col , "SELECT count (*) INTO :cnt FROM SUPERU.SBVIEW_I_P, EQUAL");

		returncode = checkPreparedStatement(pstmt, "SELECT count (*) INTO ? FROM SUPERU.SBVIEW_I_P WHERE K1 = ?");
		CE("equal_sbview: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(cnt), indi3=sizeof(v1);

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &cnt, &indi1, sizeof(cnt));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in equal_sbview (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &v1, &indi3, sizeof(v1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in equal_sbview (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
		CE ( "equal_sbview: SELECT count (*) INTO :cnt FROM SUPERU.SBVIEW_I_P, EQUAL",0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
	} while ( rollberr (1, pstmt->ERRORCODE) ) ;

	commit () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void like_bview ()
/*
	count number of records of superu.bview_i_p into "cnt" with randomized "like"-condition
	COMMIT
*/
{
	function_called_counter++;

	int v1 = ( rand () % rows ) / 10 ;

	pstmt = conn->createPreparedStatement();
	CE("like_bview: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
    {
		movep ( line , col , "SELECT I6, C5 FROM SUPERU.BVIEW_I_P, LIKE");

		returncode = checkPreparedStatement(pstmt, "SELECT I6, C5 FROM SUPERU.BVIEW_I_P WHERE I5 = ? AND C4 like 'tab%'");
		CE("like_bview: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi3=sizeof(v1);

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &v1, &indi3, sizeof(v1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in like_bview (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
		CE ( "like_bview: SELECT I6, C5 FROM SUPERU.BVIEW_I_P, LIKE",0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;

	} while ( rollberr (1, pstmt->ERRORCODE) ) ;

	if ( pstmt->ERRORCODE == 0 )
	{
		rs = pstmt->getResultSet();
		if (rs->next() == SQLDBC_OK)
		{
			SQLDBC_Length l;
			//do
			//{
			movep ( line , col , "FETCH I6, C5 FROM SUPERU.BVIEW_I_P, LIKE");
			returncode = rs->getObject(1, MY_HOSTTYPE_LONG, &i6, &l, sizeof(i6), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in like_bview (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
			returncode = rs->getObject(2, SQLDBC_HOSTTYPE_ASCII, &c5, &l, sizeof(c5), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in like_bview (col 2)", 1, rs->ERRORCODE, rs->ERRORTEXT);

				//CE ( "FETCH I6, C5 FROM SUPERU.BVIEW_I_P WHERE, LIKE",0 ) ;
			//} while ( rollberr (1) ) ;
		}
		rs->close();
	}
	commit () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void equal_bview_only_index ()
/*
	count number of records of superu.bview_i_p into "cnt" with randomized "equal"-condition
	COMMIT
*/
{
	function_called_counter++;

	int v1 = ( rand () % rows ) / 10 ;

	sprintf ( c4 , "tab2%d" , v1 ) ;

	pstmt = conn->createPreparedStatement();
	CE("equal_bview_only_index: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
    {
		movep ( line , col , "SELECT C4 FROM SUPERU.BVIEW_I_P, EQUAL");

		returncode = checkPreparedStatement(pstmt, "SELECT C4 FROM SUPERU.BVIEW_I_P WHERE C4 = ?");
		CE("equal_bview_only_index: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi2=SQLDBC_NTS;

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &c4, &indi2, sizeof(c4));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in equal_bview_only_index (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
		CE ( "equal_bview_only_index: SELECT C4 FROM SUPERU.BVIEW_I_P, EQUAL",0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
	} while ( rollberr (1, pstmt->ERRORCODE) ) ;

	if ( pstmt->ERRORCODE == 0 )
	{
		rs = pstmt->getResultSet();
		if (rs->next() == SQLDBC_OK)
		{
			SQLDBC_Length l;
			//do
			//{
			movep ( line , col , "FETCH C4 FROM SUPERU.BVIEW_I_P, EQUAL");
			returncode = rs->getObject(1, SQLDBC_HOSTTYPE_ASCII, &c4, &l, sizeof(c4), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in equal_bview_only_index (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);

			//CE ( "FETCH C4 FROM SUPERU.BVIEW_I_P WHERE C4 = :c4",0 ) ;
			//} while ( rollberr (1) ) ;
		}
		rs->close();
	}
	commit () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void between_sbview_16 ()
/*
	count number of records of superu.sbview_i_p_16 into "cnt" with randomized "between"-condition
	COMMIT
*/
{
	function_called_counter++;

    int v1 = ( rand () % rows ) / 10 ;
    int v2 = v1 + 10 ;

	pstmt = conn->createPreparedStatement();
	CE("between_sbview_16: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
    {
        movep ( line , col , "SELECT count (*) INTO :cnt FROM SUPERU.SBVIEW_I_P_16, BETWEEN");

		returncode = checkPreparedStatement(pstmt, "SELECT count (*) INTO ? FROM SUPERU.SBVIEW_I_P_16 WHERE I3 BETWEEN ? and ?");
		CE("between_sbview_16: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(cnt), indi3=sizeof(v1), indi4=sizeof(v2);

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &cnt, &indi1, sizeof(cnt));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in between_sbview_16 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &v1, &indi3, sizeof(v1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in between_sbview_16 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_INT4, &v2, &indi4, sizeof(v2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in between_sbview_16 (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
        CE ( "between_sbview_16: SELECT count (*) INTO :cnt FROM SUPERU.SBVIEW_I_P_16, BETWEEN",0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;

    } while ( rollberr (3, pstmt->ERRORCODE) ) ; /* TS 2003-10-21 Change rollberr from 1 to 3, because of -8 errors */

    commit () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void equal_sbview_16 ()
/*
	count number of records of superu.sbview_i_p_16 into "cnt" with randomized "equal"-condition
	COMMIT
*/
{
	function_called_counter++;

	int v1 = ( rand () % rows )  ;

	pstmt = conn->createPreparedStatement();
	CE("equal_sbview_16: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
    {
		movep ( line , col , "SELECT count (*) INTO :cnt FROM SUPERU.SBVIEW_I_P_16, EQUAL");

		returncode = checkPreparedStatement(pstmt, "SELECT count (*) INTO ? FROM SUPERU.SBVIEW_I_P_16 WHERE K1 = ?");
		CE("equal_sbview_16: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(cnt), indi3=sizeof(v1);

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &cnt, &indi1, sizeof(cnt));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in equal_sbview_16 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &v1, &indi3, sizeof(v1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in equal_sbview_16 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
		CE ( "equal_sbview_16: SELECT count (*) INTO :cnt FROM SUPERU.SBVIEW_I_P_16, EQUAL",0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;

	} while ( rollberr (1, pstmt->ERRORCODE) ) ;

	commit () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void like_bview_16 ()
/*
	count number of records of superu.bview_i_p_16 into "cnt" with randomized "like"-condition
	COMMIT
*/
{
	function_called_counter++;

	int v1 = ( rand () % rows ) / 10 ;

	pstmt = conn->createPreparedStatement();
	CE("like_bview_16: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
    {
		movep ( line , col , "SELECT I6, C5 FROM SUPERU.BVIEW_I_P_16, LIKE");

		returncode = checkPreparedStatement(pstmt, "SELECT I6, C5 FROM SUPERU.BVIEW_I_P_16 WHERE I5 = ? AND C4 like 'tab%'");
		CE("like_bview_16: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi3=sizeof(v1);

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &v1, &indi3, sizeof(v1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in like_bview_16 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
		CE ( "like_bview_16: SELECT I6, C5 FROM SUPERU.BVIEW_I_P_16, LIKE",0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;

	} while ( rollberr (1, pstmt->ERRORCODE) ) ;

	if ( pstmt->ERRORCODE == 0 )
	{
		rs = pstmt->getResultSet();
		if (rs->next() == SQLDBC_OK)
		{
			SQLDBC_Length l;
			//do
			//{
			movep ( line , col , "FETCH I6, C5 FROM SUPERU.BVIEW_I_P_16, LIKE");
			returncode = rs->getObject(1, MY_HOSTTYPE_LONG, &i6, &l, sizeof(i6), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in like_bview_16 (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
			returncode = rs->getObject(2, SQLDBC_HOSTTYPE_ASCII, &c5, &l, sizeof(c5), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in like_bview_16 (col 2)", 1, rs->ERRORCODE, rs->ERRORTEXT);

			//CE ( "FETCH I6, C5 FROM SUPERU.BVIEW_I_P_16 WHERE, LIKE",0 ) ;
			//} while ( rollberr (1) ) ;
		}
		rs->close();
	}
	commit () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void equal_bview_only_index_16 ()
/*
	count number of records of superu.bview_i_p_16 into "cnt" with randomized "equal"-condition
	COMMIT
*/
{
	function_called_counter++;

	int v1 = ( rand () % rows ) / 10 ;

	sprintf ( c4 , "tab2%d" , v1 ) ;

	pstmt = conn->createPreparedStatement();
	CE("equal_bview_only_index_16: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
    {
		movep ( line , col , "SELECT C4 FROM SUPERU.BVIEW_I_P_16, EQUAL");

		returncode = checkPreparedStatement(pstmt, "SELECT C4 FROM SUPERU.BVIEW_I_P_16 WHERE C4 = ?");
		CE("equal_bview_only_index_16: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi2=SQLDBC_NTS;

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &c4, &indi2, sizeof(c4));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in equal_bview_only_index_16 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
		CE ( "equal_bview_only_index_16: SELECT C4 FROM SUPERU.BVIEW_I_P_16, EQUAL",0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
	} while ( rollberr (1, pstmt->ERRORCODE) ) ;

	if ( pstmt->ERRORCODE == 0 )
	{
		rs = pstmt->getResultSet();
		if (rs->next() == SQLDBC_OK)
		{
			SQLDBC_Length l;
			//do
			//{
			movep ( line , col , "FETCH C4 FROM SUPERU.BVIEW_I_P_16, EQUAL");
			returncode = rs->getObject(1, SQLDBC_HOSTTYPE_ASCII, &c4, &l, sizeof(c4), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in equal_bview_only_index_16 (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);

			//CE ( "FETCH C4 FROM SUPERU.BVIEW_I_P_16 WHERE C4 = :c4",0 ) ;
			//} while ( rollberr (1) ) ;
		}
		rs->close();
	}
	commit () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void insert_array_tab1 ( int rws )
/*
	INSERT from array in tab1 ... UPDATE DUPLICATES until successful execution
	COMMIT
*/
{
	function_called_counter++;

    int i = 0;
    int rows_to_insert = rws > 10 ? 10 : rws ;

	movep ( line , col , "ARRAY INSERT TAB1 (%d rows)" , rows_to_insert ) ;

    rolled_back = 0 ;

	SQLDBC_Length indi1[10]={0}, indi2[10]={0};
    for ( i = 0 ; i < rows_to_insert ; i++ )
    {
	    indi1[i]=sizeof(long);
		indi2[i]=SQLDBC_NTS;
		k1m[i] = rand () % rows ;
		i1m[i] = ( rand () % rows ) * 100 ;
		i2m[i] = ( rand () % rows ) % 10 ;
		sprintf ( c1m [i] , "Name%s_%d" , id , ( rand () % rows )  ) ;
		sprintf ( c2m [i] , "Name%s.%d" , id , ( rand () % rows )  / 10 ) ;
		i3m [i] = ( rand () % rows )  / 10 ;
    }

	pstmt = conn->createPreparedStatement();
	CE("insert_array_tab1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
    do
    {
		returncode = checkPreparedStatement(pstmt, "INSERT TAB1 VALUES ( ?, ?, ?, ?, ?, ?) UPDATE DUPLICATES");
		CE("insert_array_tab1: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &k1m, &indi1[0], sizeof(k1m));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_array_tab1 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, MY_HOSTTYPE_LONG, &i1m, &indi1[0], sizeof(i1m));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_array_tab1 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, MY_HOSTTYPE_LONG, &i2m, &indi1[0], sizeof(i2m));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_array_tab1 (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &c1m, &indi2[0], sizeof(c20));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_array_tab1 (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_ASCII, &c2m, &indi2[0], sizeof(c20));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_array_tab1 (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (6, MY_HOSTTYPE_LONG, &i3m, &indi1[0], sizeof(i3m));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_array_tab1 (col 6)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->setBatchSize(rows_to_insert);
		CE ("insert_array_tab1: setBatchSize" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;

		returncode = pstmt->execute();
		CE ("insert_array_tab1: ARRAY INSERT TAB1" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;

    } while ( rollberr (1, pstmt->ERRORCODE) && rolled_back == 0 ) ;

    CE ("insert_array_tab1: ARRAY INSERT TAB1" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;

    commit () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void mass_insert_array_tab1m ()
/*
	INSERT from array in TAB1M ... UPDATE DUPLICATES until successful execution
	COMMIT or ROLLBACK
	FETCH values
	SELECT COUNT *
	if rowcount greater than rows-1000 delete some values
	COMMIT or ROLLBACK
*/
{
	function_called_counter++;

	int i = 0;
	int rws = rows/3;
	int rows_to_insert = rws > 1000 ? 1000 : rws ;
	long k1m[1000]={0};
	long i1m[1000]={0}, i2m[1000]={0}, i3m[1000]={0};
	c20 c1m[1000]={0};
	c500 *c2m = new c500[1000];
	long k1m_f=0;
	c500 c2m_f={0};
	
	movep ( line , col , "Massen-INSERT TAB1M (%d rows)" , rows_to_insert ) ;

	rolled_back = 0 ;

	SQLDBC_Length indi1[1000]={0}, indi2[1000]={0};
	for ( i = 0 ; i < rows_to_insert ; i++ )
	{
	    indi1[i]=sizeof(long);
		indi2[i]=SQLDBC_NTS;
		k1m[i] = rand () % rows ;
		i1m[i] = ( rand () % rows ) * 100 ;
		i2m[i] = ( rand () % rows ) % 1000 ;
		sprintf ( c1m [i] , "Name%s_%d" , id , ( rand () % rows )  ) ;
		sprintf ( c2m [i] , "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789" , id , ( rand () % rows )  / 1000 ) ;
		i3m [i] = ( rand () % rows )  / 100 ;
	}

	pstmt = conn->createPreparedStatement();
	CE("mass_insert_array_tab1m: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
	{
		returncode = checkPreparedStatement(pstmt, "INSERT TAB1M VALUES ( ?, ?, ?, ?, ?, ?) UPDATE DUPLICATES");
		CE("mass_insert_array_tab1m: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &k1m, &indi1[0], sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in mass_insert_array_tab1m (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, MY_HOSTTYPE_LONG, &i1m, &indi1[0], sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in mass_insert_array_tab1m (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, MY_HOSTTYPE_LONG, &i2m, &indi1[0], sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in mass_insert_array_tab1m (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &c1m, &indi2[0], sizeof(c20));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in mass_insert_array_tab1m (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_ASCII, c2m, &indi2[0], sizeof(c500));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in mass_insert_array_tab1m (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (6, MY_HOSTTYPE_LONG, &i3m, &indi1[0], sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in mass_insert_array_tab1m (col 6)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->setBatchSize(rows_to_insert);
		CE ("mass_insert_array_tab1m: setBatchSize" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;

		returncode = pstmt->execute();
		CE ("mass_insert_array_tab1m: Massen-INSERT TAB1M" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;

	} while ( rollberr (1, pstmt->ERRORCODE) && rolled_back == 0 ) ;
	CE ("mass_insert_array_tab1m: Massen-INSERT TAB1M" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;

	if ( ((rand () % 4) * use_random) == 1 )
	{
		rollback () ;
	}
	else
	{
		commit () ;

		returncode = stmt->execute("SELECT K1, C2 FROM TAB1M");
		CE("mass_insert_array_tab1m: SELECT K1, C2 FROM TAB1M" , 0, stmt->ERRORCODE, stmt->ERRORTEXT) ;

		if (returncode == SQLDBC_OK)
		{
			rs = stmt->getResultSet();
			i = 0;
			while (rs->next() == SQLDBC_OK)
			{
				i++;
				SQLDBC_Length l;
				returncode = rs->getObject(1, MY_HOSTTYPE_LONG, &k1m_f, &l, sizeof(long), true);
				if (returncode != SQLDBC_OK) CE("error with getObject in mass_insert_array_tab1m (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
				returncode = rs->getObject(2, SQLDBC_HOSTTYPE_ASCII, &c2m_f, &l, sizeof(c500), true);
				if (returncode != SQLDBC_OK) CE("error with getObject in mass_insert_array_tab1m (col 2)", 1, rs->ERRORCODE, rs->ERRORTEXT);
			}
			rs->close();
		}
	}

	pstmt2 = conn->createPreparedStatement();
	CE("mass_insert_array_tab1m: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
	{
		returncode = checkPreparedStatement(pstmt2, "SELECT COUNT (*) INTO ? FROM TAB1M");
		CE("count_tab1: prepare", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
		SQLDBC_Length indi3=sizeof(cnt);
		returncode = pstmt2->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &cnt, &indi3, sizeof(cnt));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in count_tab1 (col 1)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
		returncode = pstmt2->execute();
		CE ("count_tab1: SELECT COUNT(*) FROM TAB1M",0, pstmt2->ERRORCODE, pstmt2->ERRORTEXT) ;
	} while ( rollberr (1, pstmt->ERRORCODE) && rolled_back == 0 ) ;

	if (cnt > rows-1000)
	{
		returncode = stmt->execute("DELETE TAB1M WHERE I3 MOD 2 = 1");
		CE ( "mass_insert_array_tab1m: DELETE TAB1M WHERE I3 MOD 2 = 1" , 0, stmt->ERRORCODE, stmt->ERRORTEXT) ;
	}

	if ( rand () % 4 )
	{
		rollback () ;
	}
	else
	{
		commit () ;
	}

	conn->releaseStatement(pstmt2);
	pstmt2 = 0;
	conn->releaseStatement(pstmt);
	pstmt = 0;
	delete[] c2m;
}
/*------------------------------------------------------------------------*/
void insert_tab20k ()
/*
	INSERT superu.tab20k with randomized values until successful execution
	COMMIT or ROLLBACK
*/
{
	function_called_counter++;

	movep (line,col,"INSERT SUPERU.TAB20K") ;

	int i = rand () % rows ;
	int l = rand () % 400 ;

	i3 = i ;
	i4 = rows - i ;
	sprintf ( char5 , "%d" , i3 ) ;

	memset ( char400 , 'a'+ (rand() % 25) , l ) ;
	char400 [l] = 0 ;
	char400 [100] = (char) ( 'a' + (rand() % 26) ) ;

	memset ( char400_2 , 'a'+ (rand() % 25) , l ) ;
	char400_2 [399] = 0 ;
	char400_2[(l/2)] = (char) ( 'a' + (rand() % 26) ) ;

    mytime = (int) time ( (time_t *) 0) ;

	pstmt = conn->createPreparedStatement();
	CE("insert_tab20k: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
	{
		returncode = checkPreparedStatement(pstmt, "INSERT SUPERU.TAB20K set "
			"VC1 = ?, C2  = ?, I3  = ?, "
			"VC4  = ?, C5  = ?, I6  = ?, "
			"VC7  = ?, C8  = ?, I9  = ?, "
			"VC10  = ?, C11  = ?, I12  = ?, "
			"VC13  = ?, C14  = ?, I15  = ?, "
			"VC16  = ?, C17  = ?, I18  = ?, "
			"VC19  = ?, C20  = ? , "
			"MYPID = ? , MYTIME = ? , "
			"I21 = ?, C22 = ?, VC23 = ?, C24 = ?, C25 = ?, "
			"I26 = ?, C27 = ?, VC28 = ?, C29 = ?, C30 = ?, "
			"I31 = ?, C32 = ?, VC33 = ?, C34 = ?, C35 = ?, "
			"I36 = ?, C37 = ?, VC38 = ?, C39 = ?, C40 = ?");
		CE("insert_tab20k: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(long), indi2=SQLDBC_NTS, indi3=sizeof(mypid), indi4=sizeof(mytime);

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, MY_HOSTTYPE_LONG, &i3, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (6, MY_HOSTTYPE_LONG, &i4, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 6)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (7, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 7)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (8, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 8)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (9, MY_HOSTTYPE_LONG, &i3, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 9)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (10, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 10)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (11, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 11)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (12, MY_HOSTTYPE_LONG, &i4, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 12)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (13, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 13)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (14, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 14)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (15, MY_HOSTTYPE_LONG, &i3, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 15)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (16, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 16)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (17, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 17)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (18, MY_HOSTTYPE_LONG, &i4, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 18)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (19, SQLDBC_HOSTTYPE_ASCII, &char400, &indi2, sizeof(char400));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 19)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (20, SQLDBC_HOSTTYPE_ASCII, &char400_2, &indi2, sizeof(char400_2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 20)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (21, SQLDBC_HOSTTYPE_INT4, &mypid, &indi3, sizeof(mypid));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 21)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (22, SQLDBC_HOSTTYPE_INT4, &mytime, &indi4, sizeof(mytime));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 22)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (23, MY_HOSTTYPE_LONG, &i3, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 23)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (24, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 24)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (25, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 25)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (26, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 26)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (27, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 27)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (28, MY_HOSTTYPE_LONG, &i4, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 28)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (29, SQLDBC_HOSTTYPE_ASCII, &char400, &indi2, sizeof(char400));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 29)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (30, SQLDBC_HOSTTYPE_ASCII, &char400_2, &indi2, sizeof(char400_2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 30)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (31, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 31)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (32, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 32)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (33, MY_HOSTTYPE_LONG, &i4, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 33)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (34, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 34)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (35, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 35)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (36, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 36)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (37, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 37)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (38, MY_HOSTTYPE_LONG, &i4, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 38)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (39, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 39)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (40, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 40)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (41, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 41)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (42, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab20k (col 42)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();

		if ( pstmt->ERRORCODE != 250 )
			{ CE ("insert_tab20k: INSERT TAB20K",0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ; }

	} while ( rollberr (1, pstmt->ERRORCODE) ) ;

	if ( rand () % 4 )
		commit () ;
	else
		rollback () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void delete_tab20k ()
/*
	DELETE superu.tab20k with randomized value ("equal"-condition)
	COMMIT or ROLLBACK
*/
{
	function_called_counter++;

    k1 = rand () % rows ;
	sprintf ( char5 , "%d" , k1 ) ;

    movep ( line , col , "DELETE SUPERU.TAB20K WHERE VC1 = ") ;

	pstmt = conn->createPreparedStatement();
	CE("delete_tab20k: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
    do
    {
		returncode = checkPreparedStatement(pstmt, "DELETE SUPERU.TAB20K WHERE VC1 = ?");
		CE("delete_tab20k: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi2=SQLDBC_NTS;

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in delete_tab20k (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();

		CE ( "delete_tab20k: DELETE SUPERU.TAB20K WHERE VC1=" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;

    } while ( rollberr (1, pstmt->ERRORCODE) ) ;

	if ( rand () % 4 )
		commit () ;
	else
		rollback () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void delete_tab20k_2 ()
/*
	DELETE superu.tab20k with randomized value ("equal"-condition)
	COMMIT or ROLLBACK
*/
{
	function_called_counter++;

    k1 = rand () % rows ;

    movep ( line , col , "DELETE SUPERU.TAB20K WHERE I36 = ") ;

	pstmt = conn->createPreparedStatement();
	CE("delete_tab20k_2: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
    do
    {
		returncode = checkPreparedStatement(pstmt, "DELETE SUPERU.TAB20K WHERE I36 = ?");
		CE("delete_tab20k_2: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(long);

		returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &k1, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in delete_tab20k_2 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();

		CE ( "delete_tab20k_2: DELETE SUPERU.TAB20K WHERE I36 =" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
    } while ( rollberr (1, pstmt->ERRORCODE) ) ;

	if ( rand () % 4 )
		commit () ;
	else
		rollback () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void delete_between_tab20k ()
/*
	DELETE superu.tab20k with randomized value ("between"-condition)
	COMMIT or ROLLBACK
*/
{
	function_called_counter++;

	i4 = rows - ( rand () % rows ) ;
	i3 = i4 + 10 ;

    movep ( line , col , "DELETE SUPERU.TAB20K WHERE I36 between ... ") ;

	pstmt = conn->createPreparedStatement();
	CE("delete_between_tab20k: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
    {
		returncode = checkPreparedStatement(pstmt, "DELETE SUPERU.TAB20K WHERE I36 between ? and ?");
		CE("delete_between_tab20k: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(long);

		returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &i4, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in delete_between_tab20k (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, MY_HOSTTYPE_LONG, &i3, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in delete_between_tab20k (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
		CE ( "delete_between_tab20k: DELETE SUPERU.TAB20K WHERE I36 between ... " , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;

    }	while ( rollberr (1, pstmt->ERRORCODE) ) ;

	if ( rand () % 4 )
		commit () ;
	else
		rollback () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void update_key_tab20k_1 ()
/*
	UPDATE superu.tab20k until successful execution ("equal"-condition)
	COMMIT or ROLLBACK
*/
{
	function_called_counter++;

	movep (line,col,"UPDATE KEY SUPERU.TAB20K 1") ;

	int l = rand () % 400 ;

	memset ( char400 , 'a'+ (rand() % 25) , l ) ;
	char400 [l] = 0 ;
	char400 [100] = (char) ( 'a' + (rand() % 26) ) ;

	memset ( char400_2 , 'a'+ (rand() % 25) , l ) ;
	char400_2 [399] = 0 ;
	char400_2[(l/2)] = (char) ( 'a' + (rand() % 26) ) ;

	i3 = rand () % rows ;
	i4 = rows - ( rand () % rows ) / 10 ;
    mytime = (int) time ( (time_t *) 0) ;

	pstmt = conn->createPreparedStatement();
	CE("update_key_tab20k_1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
	{
		returncode = checkPreparedStatement(pstmt, "UPDATE SUPERU.TAB20K SET I18  = ?, MYPID = ?, MYTIME = ?, "
			"VC19 = ?, C20 = ?, vc28 = ? WHERE I36 =  ?");
		CE("update_key_tab20k_1: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(long), indi2=SQLDBC_NTS, indi3=sizeof(mypid), indi4=sizeof(mytime);

		returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &i3, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_key_tab20k_1 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &mypid, &indi3, sizeof(mypid));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_key_tab20k_1 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_INT4, &mytime, &indi4, sizeof(mytime));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_key_tab20k_1 (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &char400, &indi2, sizeof(char400));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_key_tab20k_1 (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_ASCII, &char400_2, &indi2, sizeof(char400_2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_key_tab20k_1 (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (6, SQLDBC_HOSTTYPE_ASCII, &char400, &indi2, sizeof(char400));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_key_tab20k_1 (col 6)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (7, MY_HOSTTYPE_LONG, &i4, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_key_tab20k_1 (col 7)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();

		CE ("update_key_tab20k_1: UPDATE SUPERU.TAB20K 2",0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
	} while ( rollberr (1, pstmt->ERRORCODE) ) ;

	if ( rand () % 4 )
		commit () ;
	else
		rollback () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void update_key_tab20k_2 ()
/*
	UPDATE superu.tab20k until successful execution ("between"-condition)
	count numer of updated rows
	if any row was updated, select values from superu.tab20k and check selected and updated values
	COMMIT or ROLLBACK
*/
{
	function_called_counter++;

	int l = 1 + ( rand () % 399 );
	int theloop = 0;
	int updated = 0;
	char mychar ;
	char mychar400[401]={0};

    mytime = (int) time ( (time_t *) 0) ;
	movep (line,col,"UPDATE KEY SUPERU.TAB20K 2") ;

	i3 = rand () % rows ;
	i4 = rows - ( rand () % rows ) ;
	sprintf ( char5 , "%d" , i3 ) ;

	mychar = '0' + (char)( mypid % 10 ) ;

	memset ( char400 , (char)('a' + (rand () % 4)) , l ) ;
	char400 [l] = 0 ;
	char400 [0] = char400 [l-1] = mychar ;

	memset ( char400_2 , 'a'+ (rand() % 25) , l ) ;
	char400_2 [399] = 0 ;
	char400_2[(l/2)] = (char) ( 'a' + (rand() % 26) ) ;

	strcpy ( mychar400 , char400 ) ;
	pstmt = conn->createPreparedStatement();
	CE("update_key_tab20k_2: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
	{
		returncode = checkPreparedStatement(pstmt, "UPDATE SUPERU.TAB20K SET VC19 = ?, C20 = ?, C40 = ?, "
			"MYPID = ?, MYTIME = ?, VC28 = ? WHERE I21 BETWEEN ? AND ( ? + 10 )");
		CE("update_key_tab20k_2: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(long), indi2=SQLDBC_NTS, indi3=sizeof(mypid), indi4=sizeof(mytime);

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &char400, &indi2, sizeof(char400));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_key_tab20k_2 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_ASCII, &char400, &indi2, sizeof(char400));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_key_tab20k_2 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_key_tab20k_2 (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_INT4, &mypid, &indi3, sizeof(mypid));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_key_tab20k_2 (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_INT4, &mytime, &indi4, sizeof(mytime));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_key_tab20k_2 (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (6, SQLDBC_HOSTTYPE_ASCII, &char400_2, &indi2, sizeof(char400_2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_key_tab20k_2 (col 6)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (7, MY_HOSTTYPE_LONG, &i4, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_key_tab20k_2 (col 7)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (8, MY_HOSTTYPE_LONG, &i4, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_key_tab20k_2 (col 8)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();

		CE ("update_key_tab20k_2: UPDATE KEY SUPERU.TAB20K 2",0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
	} while ( rollberr (1, pstmt->ERRORCODE) ) ;

	updated = pstmt->getRowsAffected();

	if ( updated > 0 )
	{
		pstmt2 = conn->createPreparedStatement();
		CE("update_key_tab20k_2: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

		returncode = checkPreparedStatement(pstmt2, "SELECT I21, VC19, C20 FROM SUPERU.TAB20K WHERE I21 BETWEEN ? "
			"AND ( ? + 10 ) AND MYPID = ? and MYTIME = ?");
		CE("update_key_tab20k_2: prepare", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(long), indi2=SQLDBC_NTS, indi3=sizeof(mypid), indi4=sizeof(mytime);

		returncode = pstmt2->bindParameter (1, MY_HOSTTYPE_LONG, &i4, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_key_tab20k_2 (col 9)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
		returncode = pstmt2->bindParameter (2, MY_HOSTTYPE_LONG, &i4, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_key_tab20k_2 (col 10)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
		returncode = pstmt2->bindParameter (3, SQLDBC_HOSTTYPE_INT4, &mypid, &indi3, sizeof(mypid));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_key_tab20k_2 (col 11)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
		returncode = pstmt2->bindParameter (4, SQLDBC_HOSTTYPE_INT4, &mytime, &indi4, sizeof(mytime));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_key_tab20k_2 (col 12)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);

		returncode = pstmt2->execute();

		CE ("update_key_tab20k_2: SELECT VC19 , VC20 WHERE I21 ...",0, pstmt2->ERRORCODE, pstmt2->ERRORTEXT) ;

		rs = pstmt2->getResultSet();

		for ( theloop = 0 ; theloop < updated ; theloop++ )
		{
			if (rs->next() == SQLDBC_OK)
			{
				SQLDBC_Length l;

				returncode = rs->getObject(1, MY_HOSTTYPE_LONG, &i3, &l, sizeof(i3), true);
				if (returncode != SQLDBC_OK) CE("error with getObject in update_key_tab20k_2 (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
				returncode = rs->getObject(2, SQLDBC_HOSTTYPE_ASCII, &char400, &l, sizeof(char400), false);
				if (returncode != SQLDBC_OK) CE("error with getObject in update_key_tab20k_2 (col 2)", 1, rs->ERRORCODE, rs->ERRORTEXT);
				//false + true works! true + true works
				returncode = rs->getObject(3, SQLDBC_HOSTTYPE_ASCII, &char400_2, &l, sizeof(char400_2), true);
				if (returncode != SQLDBC_OK) CE("error with getObject in update_key_tab20k_2 (col 3)", 1, rs->ERRORCODE, rs->ERRORTEXT);

				char myline[600]={0};
				if ( strcmp (mychar400,char400) )
				{
					sprintf ( myline,"SELECT TAB20K, I21= %d, corrupted VC19 : %s",i3,char400 ) ;
					print_f ( myline, 445, "check error manually") ;
					sprintf ( myline,"should be: %s",mychar400 ) ;
					print_f ( myline, 445, "check error manually") ;
					Sqldump ("update_key_tab20k_2()", 445, "check error with mychar400 and char400", conn) ;
				}

				if ( strcmp (mychar400,char400_2) )
				{
					sprintf ( myline,"SELECT TAB20K, I21= %d, corrupted C20 : '%s'",i3,char400_2 ) ;
					print_f ( myline, 445, "check error manually") ;
					sprintf ( myline,"should be: '%s'",mychar400 ) ;
					print_f ( myline, 445, "check error manually") ;
					Sqldump ("update_key_tab20k_2()", 445, "check error with mychar400 and char400_2", conn) ;
				}
			}
		}
		if ( theloop != updated )
		{
			char myline [200]={0};
			sprintf ( myline,"SELECT TAB20K, updated (%d) <> (%d) selected",updated,theloop) ;
			print_f ( myline, 445, "check error manually") ;
			Sqldump ("update_key_tab20k_2()", 445, "check error with theloop and updated", conn) ;
		}

		rs->close();
		conn->releaseStatement(pstmt2);
		pstmt2 = 0;
	}

	if ( rand () % 4 )
		commit () ;
	else
		rollback () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void insert_tab04sk ()
/*
	INSERT superu.tab04sk until successful execution
	COMMIT or ROLLBACK
*/
{
	function_called_counter++;

	int i = rand () % rows ;
	int l = rand () % 400 ;

	i3 = ( i >> 16 ) & 0xff ;
	i4 = rows - i ;

	char1a[1] = char1b[1] = char1c[1] = (char) 0 ;

	char1a[0] = ( i ) & 0xff ;
	char1b[0] = ( ( i >> 8 ) ) & 0xff ;
	char1c[0] = ( 'a' + (char) (rand()%13) ) ;

	sprintf ( char5 , "%d" , i ) ;

	movep (line,col,"INSERT SUPERU.TAB04SK") ;

    mytime = (int) time ( (time_t *) 0) ;

	pstmt = conn->createPreparedStatement();
	CE("insert_tab04sk: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
	{
		returncode = checkPreparedStatement(pstmt, "INSERT SUPERU.TAB04SK set "
			"VC1  = ?, C2  = ?, I3  = ?, "
			"VC4  = ?, C5  = ?, I6  = ?, "
			"VC7  = ?, C8  = ?, I9  = ?, "
			"VC10  = ?, C11  = ?, I12  = ?, "
			"VC13  = ?, C14  = ?, I15  = ?, "
			"VC16  = ?, C17  = ?, I18  = ?, "
			"VC19  = ?, C20  = ? , "
			"MYPID = ?, MYTIME = ?, "
			"I21 = ?, C22 = ?, VC23 = ?, C24 = ?, C25 = ?, "
			"I26 = ?, C27 = ?, VC28 = ?, C29 = ?, C30 = ?, "
			"I31 = ?, C32 = ?, VC33 = ?, C34 = ?, C35 = ?, "
			"I36 = ?, C37 = ?, VC38 = ?, C39 = ?, C40 = ?");
		CE("insert_tab04sk: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(long), indi2=SQLDBC_NTS, indi3=sizeof(mypid), indi4=sizeof(mytime);

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &char1a, &indi2, sizeof(char1a));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_ASCII, &char1b, &indi2, sizeof(char1b));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, MY_HOSTTYPE_LONG, &i3, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &char1c, &indi2, sizeof(char1c));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (6, MY_HOSTTYPE_LONG, &i4, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 6)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (7, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 7)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (8, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 8)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (9, MY_HOSTTYPE_LONG, &i3, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 9)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (10, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 10)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (11, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 11)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (12, MY_HOSTTYPE_LONG, &i4, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 12)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (13, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 13)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (14, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 14)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (15, MY_HOSTTYPE_LONG, &i3, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 15)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (16, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 16)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (17, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 17)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (18, MY_HOSTTYPE_LONG, &i4, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 18)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (19, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 19)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (20, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 20)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (21, SQLDBC_HOSTTYPE_INT4, &mypid, &indi3, sizeof(mypid));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 21)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (22, SQLDBC_HOSTTYPE_INT4, &mytime, &indi4, sizeof(mytime));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 22)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (23, MY_HOSTTYPE_LONG, &i3, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 23)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (24, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 24)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (25, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 25)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (26, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 26)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (27, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 27)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (28, MY_HOSTTYPE_LONG, &i4, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 28)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (29, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 29)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (30, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 30)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (31, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 31)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (32, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 32)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (33, MY_HOSTTYPE_LONG, &i4, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 33)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (34, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 34)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (35, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 35)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (36, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 36)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (37, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 37)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (38, MY_HOSTTYPE_LONG, &i4, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 38)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (39, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 39)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (40, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 40)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (41, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 41)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (42, SQLDBC_HOSTTYPE_ASCII, &char5, &indi2, sizeof(char5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab04sk (col 42)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();

		CE ("insert_tab04sk: INSERT tab04sk",0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
	} while ( rollberr (1, pstmt->ERRORCODE) ) ;

	if ( rand () % 4 )
		commit () ;
	else
		rollback () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void delete_tab04sk ()
/*
	DELETE superu.tab04sk with randomized value
	COMMIT or ROLLBACK
*/
{
	function_called_counter++;

    i3 = rand () % 256 ;
	char1a[0] = (char) i3 ;

    movep ( line , col , "DELETE SUPERU.TAB04SK WHERE VC1 = ") ;

	pstmt = conn->createPreparedStatement();
	CE("delete_tab04sk: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
    do
    {
		returncode = checkPreparedStatement(pstmt, "DELETE SUPERU.TAB04SK WHERE VC1 = ?");
		CE("delete_tab04sk: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi2=SQLDBC_NTS;

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &char1a, &indi2, sizeof(char1a));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in delete_tab04sk (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();

		CE ( "delete_tab04sk: DELETE SUPERU.TAB04SK WHERE VC1 = " , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
    } while ( rollberr (1, pstmt->ERRORCODE) ) ;

	if ( rand () % 4 )
		commit () ;
	else
		rollback () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void delete_range_tab04sk ()
/*
	DELETE superu.tab04sk with randomized value
	COMMIT or ROLLBACK
*/
{
	function_called_counter++;

	char1c[0] = ( 'a' + (char) (rand()%13) ) ;
	char1c[1] = 0 ;

	movep ( line , col , "DELETE SUPERU.TAB04SK WHERE VC4 = ... ") ;

	pstmt = conn->createPreparedStatement();
	CE("delete_range_tab04sk: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
    {
		returncode = checkPreparedStatement(pstmt, "DELETE SUPERU.TAB04SK WHERE VC4 = ?");
		CE("delete_range_tab04sk: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi2=SQLDBC_NTS;

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &char1c, &indi2, sizeof(char1c));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in delete_range_tab04sk (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
		CE ( "delete_range_tab04sk: DELETE SUPERU.TAB04SK WHERE VC4 = ..." , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
    }	while ( rollberr (1, pstmt->ERRORCODE) ) ;

	if ( rand () % 4 )
		commit () ;
	else
		rollback () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void update_tab04sk ()
/*
	UPDATE superu.tab04sk with randomized value until successful execution
	COMMIT or ROLLBACK
*/
{
	function_called_counter++;

	movep (line,col,"UPDATE KEY SUPERU.TAB04SK 1") ;

	i3 = rand () % rows ;
	i4 = rows - ( rand () % rows ) / 10 ;

    mytime = (int) time ( (time_t *) 0) ;

	pstmt = conn->createPreparedStatement();
	CE("update_tab04sk: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
	{
		returncode = checkPreparedStatement(pstmt, "UPDATE SUPERU.TAB04SK SET I18  = ?, MYPID = ? , "
			"MYTIME = ? WHERE I36 =  ?");
		CE("update_tab04sk: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(long), indi2=SQLDBC_NTS, indi3=sizeof(mypid), indi4=sizeof(mytime);

		returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &i3, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab04sk (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &mypid, &indi3, sizeof(mypid));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab04sk (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_INT4, &mytime, &indi4, sizeof(mytime));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab04sk (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, MY_HOSTTYPE_LONG, &i4, &indi1, sizeof(long));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab04sk (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();

		CE ("update_tab04sk: UPDATE SUPERU.TAB04SK 2",0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
	} while ( rollberr (1, pstmt->ERRORCODE) ) ;

	if ( rand () % 4 )
		commit () ;
	else
		rollback () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void cre_idx_tab1 ()
/*
	CREATE INDEX ... on tab1 until successful execution
	COMMIT
*/
{
	function_called_counter++;

    do
    {
		movep ( line , col , "CREATE INDEX TAB1_I1 ON TAB1(I1)" ) ;

		returncode=stmt->execute("CREATE INDEX TAB1_I1 ON TAB1(I1)");
		CE ("cre_idx_tab1: CREATE INDEX TAB1_I1 ON TAB1(I1)" , 0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
    } while ( rollberr (1, stmt->ERRORCODE) ) ;
    commit () ;
}
/*------------------------------------------------------------------------*/
void alt_tab1_dynamic ()
/*
	make tab1 DYNAMIC until successful execution
	NOTE: 'ALTER TABLE <tablename> [NOT] DYNAMIC' is an auto-commited-statement
*/
{
	function_called_counter++;

    do
    {
        movep ( line , col , "ALTER TABLE TAB1 DYNAMIC") ;
		returncode=stmt->execute("ALTER TABLE TAB1 DYNAMIC");
		CE ("alt_tab1_dynamic: ALTER TABLE TAB1 DYNAMIC" , 0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
    } while ( rollberr (1, stmt->ERRORCODE) ) ;
}
/*------------------------------------------------------------------------*/
void alt_tab1_nondynamic ()
/*
	make tab1 NOT DYNAMIC until successful execution
	NOTE: 'ALTER TABLE <tablename> [NOT] DYNAMIC' is an auto-commited-statement
*/
{
	function_called_counter++;

    do
    {
        movep ( line , col , "ALTER TABLE TAB1 NOT DYNAMIC") ;
		returncode=stmt->execute("ALTER TABLE TAB1 NOT DYNAMIC");
		CE ("alt_tab1_nondynamic: ALTER TABLE TAB1 NOT DYNAMIC" , 0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
    } while ( rollberr (1, stmt->ERRORCODE) ) ;
}
/*------------------------------------------------------------------------*/
void drop_idx_tab1 ()
/*
	DROP INDEX ... on tab1 until successful execution
	COMMIT
*/
{
	function_called_counter++;

    do
    {
		movep ( line , col , "DROP INDEX TAB1(I1)") ;
		returncode=stmt->execute("DROP INDEX TAB1_I1 ON TAB1");
		CE ("drop_idx_tab1: DROP INDEX TAB1_I1 ON TAB1" , 0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
    } while ( rollberr (1, stmt->ERRORCODE) ) ;
    commit () ;
}
/*------------------------------------------------------------------------*/
void use_tab_cluster_1()
/*
	SELECT * FROM one of three table randomized
	SELECT details FROM TABLESTORAGEDETAILS
	if TAB3_CL is used, SELECT also details from INDEXSTORAGEDETAILS
*/
{
	int tab_used = (rand() % 3) + 1;
	char table_name[100]={0};
	char clearString[400]={0};
	sprintf(table_name, "TAB%d_CL", tab_used);
	int i1_local = 0, i2_local = 0;
	SQLDBC_Length l;

	sprintf(clearString , "SELECT * FROM SUPERU.%s", table_name);
	movep(line, col, clearString);
	returncode = stmt->execute(clearString);
	CE ("use_tab_cluster_1: SELECT * FROM ...", 1, stmt->ERRORCODE, stmt->ERRORTEXT) ;

	sprintf(clearString , "SELECT FILES.treeleavessize/8 leaves, TABLESTORAGEDETAILS.clustercount "
        "FROM FILES, TABLESTORAGEDETAILS, TABLES "
        "WHERE FILES.fileid = TABLES.tableid AND TABLES.schemaname='SUPERU' AND TABLES.tablename='%s' "
		"AND TABLESTORAGEDETAILS.schemaname = 'SUPERU' AND TABLESTORAGEDETAILS.tablename = '%s'", table_name, table_name);
	movep(line, col, "SELECT ... FROM TABLESTORAGEDETAILS ...");
	returncode = stmt->execute(clearString);
	CE ("use_tab_cluster_1: SELECT ... FROM TABLESTORAGEDETAILS ...", 1, stmt->ERRORCODE, stmt->ERRORTEXT) ;

	if (returncode == SQLDBC_OK)
	{
		rs = stmt->getResultSet();

		while (rs->next() == SQLDBC_OK)
		{
			returncode = rs->getObject(1, SQLDBC_HOSTTYPE_INT4, &i1_local, &l, sizeof(i1_local), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in use_tab_cluster_1 (col 1.1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
			returncode = rs->getObject(2, SQLDBC_HOSTTYPE_INT4, &i2_local, &l, sizeof(i2_local), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in use_tab_cluster_1 (col 1.2)", 1, rs->ERRORCODE, rs->ERRORTEXT);
prot("table = %s, leaves = %d, clustercount = %d\n", table_name, i1_local, i2_local);
		}
		rs->close();
	}

	if (tab_used == 3)
	{
		sprintf(clearString , "SELECT FILES.treeleavessize/8 leaves, INDEXSTORAGEDETAILS.clustercount "
            "FROM FILES, INDEXSTORAGEDETAILS, INDEXES "
            "WHERE FILES.fileid = INDEXES.fileid AND INDEXES.schemaname='SUPERU' AND INDEXES.indexname='TAB3_CL_I4' "
            "AND INDEXSTORAGEDETAILS.schemaname = 'SUPERU' AND INDEXSTORAGEDETAILS.indexname = 'TAB3_CL_I4'");
		movep(line, col, "SELECT ... FROM INDEXSTORAGEDETAILS ...");
		returncode = stmt->execute(clearString);
		CE ("use_tab_cluster_1: SELECT ... FROM INDEXSTORAGEDETAILS ...", 1, stmt->ERRORCODE, stmt->ERRORTEXT) ;

		if (returncode == SQLDBC_OK)
		{
			rs = stmt->getResultSet();
			while (rs->next() == SQLDBC_OK)
			{
				returncode = rs->getObject(1, SQLDBC_HOSTTYPE_INT4, &i1_local, &l, sizeof(i1_local), true);
				if (returncode != SQLDBC_OK) CE("error with getObject in use_tab_cluster_1 (col 2.1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
				returncode = rs->getObject(2, SQLDBC_HOSTTYPE_INT4, &i2_local, &l, sizeof(i2_local), true);
				if (returncode != SQLDBC_OK) CE("error with getObject in use_tab_cluster_1 (col 2.2)", 1, rs->ERRORCODE, rs->ERRORTEXT);
prot("table = %s, leaves = %d, clustercount = %d\n", table_name, i1_local, i2_local);
			}
			rs->close();
		}
	}

	commit();
}
/*------------------------------------------------------------------------*/
void use_tab_cluster_2()
/*
	SELECT * FROM randomized table WHERE randomized value
	FETCH values from some columns
*/
{
	int tab_used = (rand() % 3) + 1;
	int val1 = 0;
	int val2 = 0;
	SQLDBC_Length indi1=sizeof(val1), indi2=sizeof(val2);

	pstmt = conn->createPreparedStatement();
	CE("use_tab_cluster_2: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

	if (tab_used == 1)
	{
		movep ( line , col , "SELECT * FROM SUPERU.TAB1_CL WHERE ...") ;
		val1 = rand() % 20000;
		val2 = val1 + (rand() % 20000);
		returncode = checkPreparedStatement(pstmt, "SELECT * FROM SUPERU.TAB1_CL WHERE I1 BETWEEN ? AND ?");
		CE("use_tab_cluster_2: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &val1, &indi1, sizeof(val1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in use_tab_cluster_2 (col 1.1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &val2, &indi2, sizeof(val2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in use_tab_cluster_2 (col 1.2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
		CE ("use_tab_cluster_2: SELECT * FROM SUPERU.TAB1_CL WHERE I1 BETWEEN ...",0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
	}
	if (tab_used == 2)
	{
		movep ( line , col , "SELECT * FROM SUPERU.TAB2_CL WHERE ...") ;
		val1 = rand() % 45000;
		val2 = val1 + 10000;
		returncode = checkPreparedStatement(pstmt, "SELECT * FROM SUPERU.TAB2_CL WHERE I2 BETWEEN ? AND ?");
		CE("use_tab_cluster_2: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &val1, &indi1, sizeof(val1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in use_tab_cluster_2 (col 2.1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &val2, &indi2, sizeof(val2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in use_tab_cluster_2 (col 2.2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
		CE ("use_tab_cluster_2: SELECT * FROM SUPERU.TAB2_CL WHERE I2 BETWEEN ...",0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
	}
	if (tab_used == 3)
	{
		movep ( line , col , "SELECT * FROM SUPERU.TAB3_CL WHERE ...") ;
		val1 = rand() % 20;
		returncode = checkPreparedStatement(pstmt, "SELECT * FROM SUPERU.TAB3_CL WHERE I4 < ?");
		CE("use_tab_cluster_2: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &val1, &indi1, sizeof(val1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in use_tab_cluster_2 (col 3.1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
		CE ("use_tab_cluster_2: SELECT * FROM SUPERU.TAB3_CL WHERE I4 < ...",0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
	}

	if (returncode == SQLDBC_OK)
	{
		rs = pstmt->getResultSet();
		int loop_counter = 0;
		int i1_local = 0, i2_local = 0, i3_local = 0;
		while ((rs->next() == SQLDBC_OK) && (loop_counter < 150))
		{
			SQLDBC_Length l;
			returncode = rs->getObject(1, SQLDBC_HOSTTYPE_INT4, &i1_local, &l, sizeof(i1_local), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in use_tab_cluster_2 (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
			returncode = rs->getObject(2, SQLDBC_HOSTTYPE_INT4, &i2_local, &l, sizeof(i2_local), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in use_tab_cluster_2 (col 2)", 1, rs->ERRORCODE, rs->ERRORTEXT);
			returncode = rs->getObject(3, SQLDBC_HOSTTYPE_INT4, &i3_local, &l, sizeof(i3_local), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in use_tab_cluster_2 (col 3)", 1, rs->ERRORCODE, rs->ERRORTEXT);

			loop_counter++;

			int jump_for_next_fetch = (rand() % 20) + 1;
			while (rs->next() == SQLDBC_OK && jump_for_next_fetch != 0)
			{
				jump_for_next_fetch--;
			}
		}
		rs->close();
	}

	commit();
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void pe (const char *s , int leave, int errorcode, const char *errortext )
/*
	pe = "print error"
	checks the error from the last command
	with "leave"-flag or some special errors stop kernel (sqladump)
*/
{
	char	zeile[200]={0};

#ifdef DEBUG
    fprintf ( stderr , "pid %d , %s :  ERR = %d\n" , pid , s , errorcode ) ;
#endif

    sprintf ( zeile , "%s :  ERR = %d" , s , errorcode ) ;
    movep ( line , col , zeile ) ;

    if ( ( errorcode != 100 ) && ( errorcode != 200 ) )
		print_f ( s, errorcode, errortext ) ;

    if ( errorcode == 650 || errorcode == 710 )      /* Location down / Shutdown */
		finish();

    /*
	if ( SQLC == -8 / * || SQLC == -9405 * /)
		return ;
	*/

/*NEW because sqladump breaks down connection and with connection break down no further statement should be sent*/
	if (errorcode == -10807 || errorcode == -10709)
	{
		print_f(s, errorcode, errortext);
		remove( actfile );
		exit(1);
	}
/*ENDNEW*/

    if ( leave || errorcode == -108 || errorcode == -106 || errorcode == -6000 ||
		( errorcode <= -9000 && errorcode >= -10000 ) || errorcode == -7500 ||
		errorcode == -8 || ( errorcode <= -980 && errorcode > -999 ) /*||
		( errorcode != -809 && ( errorcode <= -800 && errorcode > -900 ) )*/ )
    {
		print_f ( s, errorcode, errortext ) ;
		{
			for ( int i = 0; i < MAXIGNORE; i++)
			{
				if ( ignore[i] == -errorcode )
				{
					print_f ("** sqldump ignored ==> release ", errorcode, errortext) ;
					finish();
				}
			}
		}
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
				return 1 ;
				break ;
			case 1 :
				if ( ( rand () % 4 ) == 0 )
				{
					rollback () ;
					prot ("Transaction rolled back, loop again\n","") ;
					rolled_back = 1 ;
					sleep ( 1 ) ;
				}
				else if ( errorcode == -1000 )
					sleep ( 3 ) ;
#ifdef _WIN32
				Sleep (100 * ( rand () % 10 ) ) ;
#else
				sleep (1) ;
#endif
				return 1 ;
				break ;
			case 2 :
				rollback () ;
				if ( errorcode == -1000 )
					sleep ( 3 ) ;
				else
					sleep ( 1 ) ;
#ifdef _WIN32
				Sleep (100) ;
#else
				sleep (1) ;
#endif
				prot ("Transaction rolled back, loop again\n","") ;
				rolled_back = 1 ;
				return 1;
				break ;
			case 3 :
				sqlc = errorcode ;
				rollback () ;
				prot ("Transaction rolled back, no more loop\n","") ;
				errorcode = sqlc ;
				rolled_back = 1 ;
				return 0 ;
				break ;
			case 4 :
				rollback () ;
				prot ("Transaction rolled back, loop again\n","") ;
				sleep (1) ;
				return 1 ;
				break ;
			default:
				return -2 ;
		}
	}
    else
		return (0) ;
}
/*------------------------------------------------------------------------*/
void cwr_ok (int sig)
{
    print_f ("Terminated (SIGTERM), O.K.\n", 0, "") ;
    finish();
}
/*------------------------------------------------------------------------*/
void movep (int line, int col, const char *s1, ...)
/*
	writes the given text to the console at a given line and column
*/
{
	char    zeile [200]={0} ;

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
