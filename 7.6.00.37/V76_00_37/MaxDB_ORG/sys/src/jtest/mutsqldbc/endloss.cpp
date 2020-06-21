/*
 * @(#) endloss.cpc	2005-01-21

	Simulates the behavior of the super-users, one file for each super-user

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

#ifdef   WIN32
#include "jtest/mutsqldbc/flushbuf.h" /*nocheck*/
#endif

void release_connect ();
void check_table ();
void update_stat ();
void update_stat_column_tab3j ();
void update_stat_sample ();
void crea_index_tab1 ();
void alter_tab3_not_dynamic ( );
void alter_tab3_dynamic ( );
void alter_tab1_not_dynamic ( );
void alter_tab1_dynamic ( );
void drop_index_tab1 ();
void insert_tab1 ();
void update_tab1_lr ();
void update_tab1 ();
void update_tab1_between ();
void update_tab1_key ();
void insert_tab3 ();
void insert_tab3j_1 ();
void insert_tab3j_2 ();
void count_tab1 ();
void delete_tab3 ();
void delete_tab3_key ();
void delete_tab3j_1 ();
void delete_tab3j_2 ();
void select_and_check_tab1sl  () ;
void update_tab1sl  ();
void delete_range_tab3_between ();
void mass_upd_tab1 ();
void delete_rollback_tab4 ();
void sel_from_roots ();
void work_schema();
int rollberr (int, int);

void    check_long_raw (const char *, int, int, int, int) ;
void    update_tabv1 () ;
void    update_tabv1_2 () ;
void    diagnose_analyze_on () ;
void    diagnose_analyze_count_on () ;
void    diagnose_analyze_count_off () ;
void    diagnose_analyze_off () ;
void    diagnose_analyze_clear (int) ;
void	protDiagnoseFiles () ;

typedef unsigned short UCSCHAR;

void update_tab1uc();
void del_ins_tab1uc();

UCSCHAR	uchar1[24+1];
UCSCHAR	uchar2[5+1];
UCSCHAR	uchar3[500+1];
UCSCHAR	uchar4[3+1];
UCSCHAR	uchar5[1500+1];
char	nouchar1[24+1];
char	nouchar3[500+1];
char	nouchar4[3+1];

char	k0 [240] ;
char    c6 [1501] ;
int     k1 , k2 , k3 ;
char    c1 [21] , c2[21] , c3[21] , c4[21] , c5[101] ;
int     i1 , i2 , i3 , i4 , i5 , i6 ;
int     ii ;
int     nr , cnt ;
double	avg;

struct {SQLDBC_Length len; char arr [MAXLONGRAW];} cll;
int	cll_len ;
SQLDBC_Length cll_len_i ;
int	cllen ;
SQLDBC_Length icllen ;

struct  {long len; char arr[100000];} scl;
long	lnll , nrll ;
char	stmnt [1000] ;

#ifdef _WIN32
char    buf [256] ;
#else
FILE    *fo = NULL;
#endif

int     rolled_back = 0 ;
long	endTime = 0 ;
void    cwr_ok () ;
int		function_called_counter = 0;
int		function_called_counter_all = 0;


int main (int argc , char **argv )
{
	int     i = 0;
	char    head_line [80]={0};

#ifdef _WIN32
	signal ( SIGTERM , SIGFUNC cwr_ok ) ;
#else
	signal ( SIGTERM , SIGFUNC cwr_ok ) ;
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
	sprintf(actfile, "endloss%d.act", superusernumber);
	{
		char filename [100]={0};
		sprintf ( filename , "endloss%d.prot" , superusernumber ) ;

		if ( ( fout = fopen ( filename , "w" ) ) == NULL )
		{
			char outtext [100]={0};
			sprintf ( outtext , "Error opening file '%s'\n" , filename ) ;
			perror ( outtext ) ;
			exit (-99) ;
		}
	}

	if (noinit == 1)
		prot ("No init !\n");

	if ( maxRuntime > 0 )
	{
		endTime = (long) time ((time_t *) 0 ) + maxRuntime  ;
		prot ("time now: %ld\n" , (long) time ((time_t *) 0 ) ) ;
		prot ("endtime = %ld\n" , endTime ) ;
		maxcycle = 0x7fffffff ;
	}

	prot ("===== superu = %d =====\n", superusernumber) ;
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
	prot (head_line) ;

	if ( maxRuntime > 0 )
		prot ("max. runtime : %d sec\n" , maxRuntime ) ;
	else
		prot ("max. cylces : %d\n" , maxcycle ) ;


#ifdef PROTOKOLL
	protocol = 1 ;
	movep ( line , col , "writing protocol" ) ;
	sleep (1) ;
#endif

	movep ( line , col , "CONNECT" ) ;

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

	pstmt = conn->createPreparedStatement();
	CE("main: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	returncode = checkPreparedStatement(pstmt, "SELECT maxr INTO ? FROM superu.maxraw WHERE ROWNO <= 1");
	CE("main: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	SQLDBC_Length indi1=sizeof(maxlongraw);
	returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &maxlongraw, &indi1, sizeof(maxlongraw));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->execute();
	CE("main: SELECT ...", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	conn->releaseStatement(pstmt);
	pstmt = 0;

	prot ("maxlongraw = %d\n" , maxlongraw ) ;

	memset ( k0 , 'x' , 240 ) ;
	k0[239] = 0 ;
	memset ( c6 , 'y' , 1500 ) ;
	c6 [1499] = 0 ;

	for ( cycle = 1 ; cycle <= maxcycle ; cycle++ )
	{
		function_called_counter = 0;
		
		movep (line-1 , col , "DBA %d :      cycle %d" , superusernumber, cycle ) ;
		if ( cycle % 25 == 0 )
			release_connect () ;
		if ( ( cycle % 12 == 2 ) && ( superusernumber == 1 ) )
			delete_rollback_tab4 () ;
		if ( superusernumber == 1 )
			update_stat_column_tab3j () ;

		switch ( cycle % 8 )
		{
		case 0 :
		case 1 :
			diagnose_analyze_clear(1) ;
			if (use_unicode == SQLDBC_TRUE)
			{
				update_tab1uc();
				del_ins_tab1uc();
			}
			break ;
		case 2 :
			if (superusernumber == 2)
				drop_index_tab1 () ;
			diagnose_analyze_on () ;
			verify_fdir_table_info();
			verify_fdir_index_info();
			break ;
		case 3 :
			diagnose_analyze_clear (0) ;
			if (superusernumber == 2)
			{
				alter_tab3_not_dynamic () ;
				alter_tab1_dynamic () ;
			}
			diagnose_analyze_count_on () ;
			break ;
		case 4 :
			if (superusernumber == 2)
				check_table () ;
			diagnose_analyze_clear (1) ;
			break ;
		case 5 :
			diagnose_analyze_clear (0) ;
			if (superusernumber == 2)
			{
				alter_tab3_dynamic () ;
				alter_tab1_not_dynamic () ;
			}
			diagnose_analyze_count_off () ;
			break ;
		case 6 :
			diagnose_analyze_clear (0) ;
			if (superusernumber == 2)
				crea_index_tab1 () ;
			diagnose_analyze_off () ;
			verify_fdir_table_info();
			verify_fdir_index_info();
			break ;
		case 7 :
			if (superusernumber == 2)
				update_stat () ;
			diagnose_analyze_clear (1) ;
			break ;
		}

		for ( i = 1 ; i < 200 ; i++ )
		{
			movep ( line-1 , 40 , "%d" , i ) ;

			if ((i % 10) == 0) prot("i = %d\n", i);

			if ( i % 128 == 40 )
				protDiagnoseFiles () ;

			insert_tab3j_1 () ;

			if ( (( rand () % 2 ) * use_random) == 0 )
				insert_tab3j_2 ();

			if ( superusernumber == 1 && (( rand() % 3) * use_random) == 0 )
				work_schema();

			if ( check_longs == 0 || (( rand () % 2 ) * use_random) == 0 )
				insert_tab3 () ;

			if ( check_longs == 0 && (( rand () % 3 ) * use_random) == 0 )
				update_tab1 () ;

			if ( (( rand () % 4 ) * use_random) == 0 )
				insert_tab1 () ;

			if ( (( rand () % 17 ) * use_random) == 0 )
				update_tab1_lr () ;

			if ( (( rand () % 18 ) * use_random) == 0 )
				update_tab1_key () ;

			if ( (( rand () % 19 ) * use_random) == 0 )
				update_tabv1_2 () ;

			if ( (( rand () % 23 ) * use_random) == 0 )
				update_tab1_between () ;

			if ( (( rand () % 29) * use_random) == 0 )
				select_and_check_tab1sl  ()  ;

			if ( (( rand () % 35 ) * use_random) == 0 )
				update_tabv1 () ;

			if ( (( rand () % 51 ) * use_random) == 0 )
				delete_tab3 () ;

			if ( (( rand () % 53 ) * use_random) == 0 )
				delete_tab3_key () ;

			if ( (( rand () % 71 ) * use_random) == 0 )
				update_tab1sl () ;

			if ( (( rand () % 75 ) * use_random) == 0 )
				delete_range_tab3_between () ;

			if ( (( rand () % 77 ) * use_random) == 0 )
				mass_upd_tab1 () ;

			if ( (( rand () % 97 ) * use_random) == 0 )
				sel_from_roots () ;

			if ( (( rand () % 101 ) * use_random) == 0 )
				count_tab1 () ;

			if ( (( rand () % 149 ) * use_random) == 0 )
				update_stat_sample () ;

			if ( (( rand () % 4096 ) * use_random) == 0 )
				delete_tab3j_1 () ;

			if ( (( rand () % 9192 ) * use_random) == 0 )
				delete_tab3j_2 () ;

			if ( ( superusernumber == 1 ) && ( (( rand () % 10 ) * use_random) == 0 ) )
				release_connect () ;

			if ( maxRuntime > 0 && endTime < (long) time ((time_t *) 0 ) )
			{
				prot ("Endtime reached: %ld\n", (long) time ((time_t *)0) );
				break ;
			}
			if (use_random == 0 )
			{
				break;
			}
		}
		if ( maxRuntime > 0 && endTime < (long) time ((time_t *) 0 ) )
			break ;

		prot("*** %d functions were called in cycle %d ***\n", function_called_counter, cycle);
		function_called_counter_all = function_called_counter_all + function_called_counter;
	}
	diagnose_analyze_off () ;
	
	prot("***** %d functions were called at all *****\n", function_called_counter_all);
	
	movep ( line , col , "***** FINISHED *****"  ) ;
	prot ( "***** FINISHED superu = %d *****\n", superusernumber ) ;

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

    movep ( line , col , "COMMIT WORK RELEASE , ERR = %d" , conn->ERRORCODE) ;
	cwr();

	env->releaseConnection(conn);

    sleep ( rand () % 5 ) ;

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

//	is_database_unicode();

	stmt = conn->createStatement();
	CE("release_connect: createStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

	prot("RECONNECT ... done \n");

	database_info_own();
}
/*------------------------------------------------------------------------*/
void check_table ()
/*
	CHECK TABLE tab1 or tab4
*/
{
	function_called_counter++; 

	if ( rand () % 2 )
    {
		movep (line,col,"CHECK TABLE TAB1 EXTENDED\n") ;
		do
		{
			returncode=stmt->execute("CHECK TABLE TAB1 EXTENDED");
			CE ("check_table: CHECK TABLE TAB1 EXTENDED" , 0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
		} while ( rollberr (2, stmt->ERRORCODE) ) ;
    }
	else
	{
		movep (line,col,"CHECK TABLE TAB4 EXTENDED\n") ;
		do
		{
			returncode=stmt->execute("CHECK TABLE TAB4 EXTENDED");
			CE ("check_table: CHECK TABLE TAB4 EXTENDED" , 0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
		} while ( rollberr (2, stmt->ERRORCODE) ) ;
    }
	CE ("check_table: CHECK TABLE EXTENDED" , 1, stmt->ERRORCODE, stmt->ERRORTEXT ) ;

	if ( rand () % 2 )
		commit () ;
	else
		rollback () ;
}
/*------------------------------------------------------------------------*/
void update_stat ()
/*
	UPDATE STATISTICS
*/
{
	function_called_counter++; 

    int z = 0 ;						//To-Do: z can be left out, because it will never be changed => ...
	if ( rand () % 2 )
    {
		movep (line,col,"UPDATE STATISTICS *\n") ;
		do
		{
			returncode=stmt->execute("UPDATE STATISTICS *");
			if ( stmt->ERRORCODE == -3007 && z++ < 2 )		//To-Do: whole if-statement can be left out,
			{												//because -3007 (invalid statement) is not possible
                prot ("UPDATE STATISTICS *\n");				//and z++ is always <2
			}
			else
				CE ("update_stat: UPDATE STATISTICS *" , 0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;

		} while ( rollberr (2, stmt->ERRORCODE) ) ;
		SE ("update_stat: UPDATE STATISTICS *" , 1, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
    }
	else
	{
		movep (line,col,"UPDATE STATISTICS COLUMN (*)\n") ;
		do
		{
			returncode=stmt->execute("UPDATE STATISTICS COLUMN (*) FOR TAB1");
			if ( stmt->ERRORCODE == -3007 && z++ < 2 )		//To-Do: whole if-statement can be left out,
			{												//because -3007 (invalid statement) is not possible
                prot ("UPDATE STATISTICS COLUMN ... \n");	//and z++ is always <2
			}
			else
				CE ("update_stat: UPDATE STATISTICS COLUMN (*) FOR TAB1" , 0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;

		} while ( rollberr (2, stmt->ERRORCODE) ) ;
        SE ("update_stat: UPDATE STATISTICS COLUMN ... "  , 1, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
    }
}
/*------------------------------------------------------------------------*/
void update_stat_column_tab3j ()
/*
	UPDATE STATISTICS for 4 tables until successful execution
	COMMIT
*/
{
	function_called_counter++; 

	int i = 0;
	movep (line,col,"UPDATE STATISTICS COLUMN (*) FOR TAB3J_X\n") ;
	for ( i = 1 ; i <= 4 ; i++ )
	{
	 sprintf ( stmnt ,"UPDATE STATISTICS COLUMN (*) FOR TAB3J_%d", i ) ;
		do
		{
			returncode = stmt->execute(stmnt);
			CE(stmnt, 0, stmt->ERRORCODE, stmt->ERRORTEXT);
		}while ( rollberr (2, stmt->ERRORCODE) ) ;
		SE ( stmnt , 1, stmt->ERRORCODE, stmt->ERRORTEXT) ;
		commit();
	}
}
/*------------------------------------------------------------------------*/
void update_stat_sample ()
/*
	UPDATE STATISTICS TAB1 ... SAMPLE
	COMMIT or ROLLBACK
*/
{
	function_called_counter++; 

    int z = 0 ;						//To-Do: with z and both if error -3007 statements: look above in "update_stat"
    movep (line,col,"UPDATE STATISTICS ... SAMPLE \n") ;
    do
    {
		returncode = stmt->execute("UPDATE STATISTICS TAB1 ESTIMATE SAMPLE 1000 ROWS");
		if ( stmt->ERRORCODE == -3007 && z++ < 2 )
		{
            prot ("UPDATE STATISTICS ... SAMPLE, sqlcode = %d\n", stmt->ERRORCODE);
            rollback () ;
		}
		else
			SE ("update_stat_sample: UPDATE STATISTICS ... SAMPLE" , 0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
    } while ( rollberr (2, stmt->ERRORCODE) ) ;
    SE ("update_stat_sample: UPDATE STATISTICS ... SAMPLE" , 1, stmt->ERRORCODE, stmt->ERRORTEXT  ) ;

    if ( rand () % 2 )
        commit () ;
    else
		rollback () ;

    movep (line,col,"UPDATE STATISTICS COLUMN ... SAMPLE \n") ;

    do
    {
		returncode = stmt->execute("UPDATE STATISTICS COLUMN (*) FOR TAB1 ESTIMATE SAMPLE 1000 ROWS");
		if ( stmt->ERRORCODE == -3007 && z++ < 2 )
		{
			prot ("UPDATE STATISTICS COLUMN ... SAMPLE, sqlcode = %d\n", stmt->ERRORCODE);
			rollback () ;
		}
		else
		SE ("update_stat_sample: UPDATE STATISTICS COLUMN ... SAMPLE" , 0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
    } while ( rollberr (2, stmt->ERRORCODE) ) ;
    SE ("update_stat_sample: UPDATE STATISTICS COLUMN ... SAMPLE" , 1, stmt->ERRORCODE, stmt->ERRORTEXT  ) ;

    commit () ;
}
/*------------------------------------------------------------------------*/
void crea_index_tab1 ()
/*
	CREATE INDEX ... on tab1 until successful execution
	COMMIT
*/
{
	function_called_counter++; 

	movep (line,col,"CREATE INDEX TAB1(I3)\n") ;
	do
	{
		returncode = stmt->execute("CREATE INDEX TAB1_I3 ON TAB1(I3)");
		CE ("crea_index_tab1: CREATE INDEX TAB1_I3 ON TAB1(I3)",0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
		if ( ( stmt->ERRORCODE < 0 ) && ( stmt->ERRORCODE != -1000 ) )
		{
			Sqldump ("crea_index_tab1()", stmt->ERRORCODE, stmt->ERRORTEXT, conn) ;
		}
	} while ( rollberr (2, stmt->ERRORCODE) ) ;
	if ( ! ( noinit && stmt->ERRORCODE == -103 ) )
	{
		CE ( "crea_index_tab1: CREATE INDEX ON TAB1 (I3)",1, stmt->ERRORCODE, stmt->ERRORTEXT);
	}

	commit () ;
}
/*------------------------------------------------------------------------*/
void alter_tab3_not_dynamic ( )
/*
	make tab3, tab3j_1 and tab3j_2 NOT DYNAMIC until successful execution
	NOTE: 'ALTER TABLE <tablename> [NOT] DYNAMIC' is an auto-commited-statement
*/
{
	function_called_counter++; 

	do
	{
		movep (line,col,"ALTER TABLE TAB3 NOT DYNAMIC\n") ;
		prot ("Try to set TAB3 to NOT DYNAMIC\n");
		returncode = stmt->execute("ALTER TABLE TAB3 NOT DYNAMIC");
		CE ("alter_tab3_not_dynamic: ALTER TABLE TAB3 NOT DYNAMIC",0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
	} while ( rollberr(2, stmt->ERRORCODE) ) ;

    CE ("alter_tab3_not_dynamic: ALTER TABLE TAB3 NOT DYNAMIC",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;
    prot ("Set TAB3 to NOT DYNAMIC\n");

	if ( rand () % 2 )
	{
		do
		{
			movep (line,col,"ALTER TABLE TAB3J_1 NOT DYNAMIC\n") ;
			prot ("Try to set TAB3J_1 to NOT DYNAMIC\n");
			returncode = stmt->execute("ALTER TABLE TAB3J_1 NOT DYNAMIC");
			CE ("alter_tab3_not_dynamic: ALTER TABLE TAB3J_1 NOT DYNAMIC",0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
		} while ( rollberr(2, stmt->ERRORCODE) ) ;

		CE ("alter_tab3_not_dynamic: ALTER TABLE TAB3J_1 NOT DYNAMIC",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;
		prot ("Set TAB3J_1 to NOT DYNAMIC\n");
	}

	if ( rand () % 2 )
	{
		do
		{
			movep (line,col,"ALTER TABLE TAB3J_2 NOT DYNAMIC\n") ;
			prot ("Try to set TAB3J_2 to NOT DYNAMIC\n");
			returncode = stmt->execute("ALTER TABLE TAB3J_2 NOT DYNAMIC");
			CE ("alter_tab3_not_dynamic: ALTER TABLE TAB3J_2 NOT DYNAMIC",0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
		} while ( rollberr(2, stmt->ERRORCODE) ) ;

		CE ("alter_tab3_not_dynamic: ALTER TABLE TAB3J_2 NOT DYNAMIC",1, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
		prot ("Set TAB3J_2 to NOT DYNAMIC\n");
	}
}
/*------------------------------------------------------------------------*/
void alter_tab3_dynamic ( )
/*
	make tab3, tab3j_1 and tab3j_2 DYNAMIC until successful execution
	NOTE: 'ALTER TABLE <tablename> [NOT] DYNAMIC' is an auto-commited-statement
*/
{
	function_called_counter++; 

	do
	{
		movep (line,col,"ALTER TABLE TAB3 DYNAMIC\n") ;
        prot ("Try to set TAB3 to DYNAMIC\n");
		returncode = stmt->execute("ALTER TABLE TAB3 DYNAMIC");
		CE ("alter_tab3_dynamic: ALTER TABLE TAB3 DYNAMIC",0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
	} while ( rollberr(2, stmt->ERRORCODE) ) ;

    CE ("alter_tab3_dynamic: ALTER TABLE TAB3 DYNAMIC",1, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
    prot ("Set TAB3 to DYNAMIC\n");
	if ( rand () % 2 )
	{
		do
		{
			movep (line,col,"ALTER TABLE TAB3J_1 DYNAMIC\n") ;
			prot ("Try to set TAB3J_1 to DYNAMIC\n");
			returncode = stmt->execute("ALTER TABLE TAB3J_1 DYNAMIC");
			CE ("alter_tab3_dynamic: ALTER TABLE TAB3J_1 DYNAMIC",0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
		} while ( rollberr(2, stmt->ERRORCODE) ) ;

		CE ("alter_tab3_dynamic: ALTER TABLE TAB3J_1 DYNAMIC",1, stmt->ERRORCODE, stmt->ERRORTEXT  ) ;
		prot ("Set TAB3J_1 to DYNAMIC\n");
	}
	if ( rand () % 2 )
	{
		do
		{
			movep (line,col,"ALTER TABLE TAB3J_2 DYNAMIC\n") ;
			prot ("Try to set TAB3J_2 to DYNAMIC\n");
			returncode = stmt->execute("ALTER TABLE TAB3J_2 DYNAMIC");
			CE ("alter_tab3_dynamic: ALTER TABLE TAB3J_2 DYNAMIC",0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
		} while ( rollberr(2, stmt->ERRORCODE) ) ;

		CE ("alter_tab3_dynamic: ALTER TABLE TAB3J_2 DYNAMIC",1, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
		prot ("Set TAB3J_2 to DYNAMIC\n");
	}
}
/*------------------------------------------------------------------------*/
void alter_tab1_not_dynamic ( )
/*
	make tab1 NOT DYNAMIC until successful execution
	NOTE: 'ALTER TABLE <tablename> [NOT] DYNAMIC' is an auto-commited-statement
*/
{
	function_called_counter++; 

	do
	{
		movep (line,col,"ALTER TABLE TAB1 NOT DYNAMIC\n") ;
		returncode = stmt->execute("ALTER TABLE TAB1 NOT DYNAMIC");
		CE ("alter_tab1_not_dynamic: ALTER TABLE TAB1 NOT DYNAMIC",0, stmt->ERRORCODE, stmt->ERRORTEXT) ;
	} while ( rollberr(2, stmt->ERRORCODE) ) ;

    CE ("alter_tab1_not_dynamic: ALTER TABLE TAB1 NOT DYNAMIC",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;
    prot ("Set TAB1 to NOT DYNAMIC\n");
}
/*------------------------------------------------------------------------*/
void alter_tab1_dynamic ( )
/*
	make tab1 DYNAMIC until successful execution
	NOTE: 'ALTER TABLE <tablename> [NOT] DYNAMIC' is an auto-commited-statement
*/
{
	function_called_counter++; 

	do
	{
		movep (line,col,"ALTER TABLE TAB1 DYNAMIC\n") ;
		returncode = stmt->execute("ALTER TABLE TAB1 DYNAMIC");
		CE ("alter_tab1_dynamic: ALTER TABLE TAB1 DYNAMIC",0, stmt->ERRORCODE, stmt->ERRORTEXT  ) ;
	} while ( rollberr(2, stmt->ERRORCODE) ) ;

    CE ("alter_tab1_dynamic: ALTER TABLE TAB1 DYNAMIC",1, stmt->ERRORCODE, stmt->ERRORTEXT  ) ;
    prot ("Set TAB1 to DYNAMIC\n");
}
/*------------------------------------------------------------------------*/
void drop_index_tab1 ()
/*
	LOCK TAB1 exclusiv
	DROP INDEX ... on tab1 until successful execution
	COMMIT
*/
{
	function_called_counter++; 

	int dump_c = DumpOnLongCheck ;
	do
	{
		do
		{
			movep (line,col,"LOCK TABLE TAB1 IN EXCLUSIVE MODE") ;
			returncode = stmt->execute("LOCK TABLE TAB1 IN EXCLUSIVE MODE");
			CE("drop_index_tab1: LOCK TABLE TAB1 IN EXCLUSIVE MODE", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
		} while ( rollberr (2, stmt->ERRORCODE) ) ;

		CE ( "drop_index_tab1: LOCK TABLE TAB1 IN EXCLUSIVE MODE",1, stmt->ERRORCODE, stmt->ERRORTEXT) ;

		DumpOnLongCheck = 1 ;
		check_long_raw ( "CHECK ALL WITH DUMP" ,  1  , 99999 , -1  , -1 )  ;
		DumpOnLongCheck = dump_c ;
		movep (line,col,"DROP INDEX TAB1_I3 ON TAB1\n") ;

		returncode = stmt->execute("DROP INDEX TAB1_I3 ON TAB1");
		CE("drop_index_tab1: DROP INDEX TAB1_I3 ON TAB1", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	} while ( rollberr (2, stmt->ERRORCODE) ) ;

	if ( ! ( noinit && stmt->ERRORCODE == -104 ) )
	{
		CE ( "drop_index_tab1: DROP INDEX TAB1_I3 ON TAB1",1, stmt->ERRORCODE, stmt->ERRORTEXT);
	}
	commit();
}
/*------------------------------------------------------------------------*/
void insert_tab1 ()
/*
	INSERT tab1 with randomized values
	COMMIT or ROLLBACK
*/
{
	function_called_counter++; 

    int i = rand () % rows ;

    k1 = i ;
    i1 = i * 100 + 1 ;
    i2 = i % 10 + 1 ;
    sprintf ( c1 , "Name_i%d" , i ) ;
    sprintf ( c2 , "Name.i%d" , i / 10 ) ;
    i3 = i / 10 ;

    memset ( c6 , 'y' , 1500 ) ;
    c6 [(rand()%1250)+250] = 0 ;

    cll.len = rand () % maxlongraw + 4 ;
    cll_len = cll.len ;
    memset ( cll.arr , '2' , cll.len ) ;
    /* cll.arr[0] = cll.len / 256 ;
    cll.arr[1] = cll.len & 0xff ; */
    cll.arr[0] = ( cll.len >> 8 ) & 0x7f  ;
    cll.arr[1] = cll.len & 0x7f ;
    cll.arr[2] = '+' ;
    cll.arr[cll_len-1] = '+' ;

    movep ( line , col , "INSERT TAB1, I3 = %d" , i3 ) ;

	pstmt = conn->createPreparedStatement();
	CE("insert_tab1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

	returncode = checkPreparedStatement(pstmt, "INSERT TAB1 SET K0 = ?, K1 = ?, I1 = ?, I2 = ?, C1 = ?, C2 = ?, "
		"I3 = ?, CLLEN = ?, CF0 = ?, CLB = ?");
	CE("insert_tab1: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	SQLDBC_Length indi1=SQLDBC_NTS, indi2=sizeof(k1), indi3=sizeof(i1), indi4=sizeof(i2),
		indi5=SQLDBC_NTS, indi6=SQLDBC_NTS, indi7=sizeof(i3), indi8=sizeof(cll_len),
		indi9=SQLDBC_NTS, indi10=cll.len;
	returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &k0, &indi1, sizeof(k0));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab1 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &k1, &indi2, sizeof(k1));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab1 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_INT4, &i1, &indi3, sizeof(i1));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab1 (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_INT4, &i2, &indi4, sizeof(i2));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab1 (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_ASCII, &c1, &indi5, sizeof(c1));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab1 (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (6, SQLDBC_HOSTTYPE_ASCII, &c2, &indi6, sizeof(c2));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab1 (col 6)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (7, SQLDBC_HOSTTYPE_INT4, &i3, &indi7, sizeof(i3));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab1 (col 7)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (8, SQLDBC_HOSTTYPE_INT4, &cll_len, &indi8, sizeof(cll_len));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab1 (col 8)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (9, SQLDBC_HOSTTYPE_ASCII, &c6, &indi9, sizeof(c6));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab1 (col 9)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (10, SQLDBC_HOSTTYPE_ASCII, &cll.arr, &indi10, sizeof(cll.arr));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab1 (col 10)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->execute();
	CE("insert_tab1: INSERT TAB1 ...", 0, pstmt->ERRORCODE, pstmt->ERRORTEXT);
    if ( pstmt->ERRORCODE == 0 )
	{
		conn->releaseStatement(pstmt);
		pstmt = 0;
		check_long_raw ( "INSERT TAB1" ,  k1  , k1 , -1  , -1 )  ;
	}
    if ( rand () % 4 )
        commit () ;
    else
		rollback () ;
}
/*------------------------------------------------------------------------*/
void update_tab1_lr ()
/*
	UPDATE tab1 with randomized values until successful execution ("equal"-condition)
	COMMIT or ROLLBACK
*/
{
	function_called_counter++; 

    int updated = 0;

	i2 = rand () % rows ;
    i1 = rand () % rows ;
    sprintf ( c2 , "%dtab1_lr%d" , i2 , ( i1 + i2 ) % 100 ) ;

    movep ( line , col , "UPDATE long raws ... BETWEEN" , k1 ) ;

    ii = i1 + 20 ;

    cll.len = rand () % maxlongraw + 4 ;
    cll_len = cll.len ;
    memset ( cll.arr , '0' + ( i1 % 10 ) , cll.len ) ;
    cll.arr[0] = ( cll.len >> 8 ) & 0x7f  ;
    cll.arr[1] = cll.len & 0x7f ;
    cll.arr[2] = '+' ;
    cll.arr[cll_len-1] = '+' ;

	pstmt = conn->createPreparedStatement();
	CE("update_tab1_lr: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

    do
    {
		returncode = checkPreparedStatement(pstmt, "UPDATE TAB1 SET I2 = ?, C2 = ?, CLLEN = ?, CLB = ? WHERE I1 = ?");
		CE("update_tab1_lr: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		SQLDBC_Length indi1=sizeof(i2), indi2=SQLDBC_NTS, indi3=sizeof(cll_len), indi4=cll.len, indi5=sizeof(i1);
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &i2, &indi1, sizeof(i2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1_lr (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_ASCII, &c2, &indi2, sizeof(c2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1_lr (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_INT4, &cll_len, &indi3, sizeof(cll_len));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1_lr (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &cll.arr, &indi4, sizeof(cll.arr));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1_lr (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_INT4, &i1, &indi5, sizeof(i1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1_lr (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
		if ( pstmt->ERRORCODE != -7012 )
		{
			CE ( "update_tab1_lr: UPDATE long raws ... BETWEEN" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
		}
    } while ( rollberr (1, pstmt->ERRORCODE) ) ;

    if ( pstmt->ERRORCODE != 100 && pstmt->ERRORCODE != -7012 )
    {
//		prot ( "UPDATE long raws ... BETWEEN, errorcode = %d\n" , pstmt->ERRORCODE ) ;
		updated = pstmt->getRowsAffected();
//		prot ( "UPDATE long raws ... BETWEEN, count   = %d\n" , updated ) ;

//		if ( pstmt->ERRORCODE == 0 && updated > 1 )
//		{
//prot("test_ut1lr_d1\n");
//prot("test_ut1lr_d2\n");
//			DE ( "update_tab1_lr: UPDATE long raws, too many" , 1, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
//		}

	}
	if ( pstmt->ERRORCODE == 0 )
	{
		if ( rand () % 4 )
			commit () ;
		else
			rollback () ;
	}
	else
		rollback () ;

	conn->releaseStatement(pstmt);
	pstmt = 0;
    check_long_raw ( "UPDATE long raws ... BETWEEN" , -1 , -1 , i1 , i1 )  ;
}
/*------------------------------------------------------------------------*/
void update_tab1 ()
/*
	UPDATE tab1 with randomized values until successful execution ("equal"-condition)
	COMMIT or ROLLBACK
*/
{
	function_called_counter++; 

    k1 = rand () % rows ;
    i2 = rand () % rows ;
    i1 = rand () % rows ;
    sprintf ( c2 , "%dtab1_ut%d" , i2 , ( i1 + i2 ) % 100 ) ;

    movep ( line , col , "UPDATE TAB1" , k1 ) ;

	pstmt = conn->createPreparedStatement();
	CE("update_tab1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
    do
    {
		returncode = checkPreparedStatement(pstmt, "UPDATE TAB1 SET I2 = ? , C2 = ? WHERE K1 = ?");
		CE("update_tab1: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		SQLDBC_Length indi1=sizeof(i2), indi2=SQLDBC_NTS, indi3=sizeof(k1);
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &i2, &indi1, sizeof(i2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_ASCII, &c2, &indi2, sizeof(c2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_INT4, &k1, &indi3, sizeof(k1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1 (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
		CE("update_tab1: UPDATE TAB1 ...", 0, pstmt->ERRORCODE, pstmt->ERRORTEXT);
    } while ( rollberr (1, pstmt->ERRORCODE) ) ;
    if ( rand () % 8 )
		commit () ;
    else
		rollback () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void update_tab1_between ()
/*
	UPDATE tab1 with randomized values until successful execution ("between"-condition)
	COMMIT or ROLLBACK
*/
{
	function_called_counter++; 

    i2 = rand () % rows ;
    i1 = rand () % rows ;
    sprintf ( c2 , "%dtab1_ut%d" , i2 , ( i1 + i2 ) % 100 ) ;

    movep ( line , col , "UPDATE TAB1 ... BETWEEN" , k1 ) ;

    ii = i1 + 20 ;

	pstmt = conn->createPreparedStatement();
	CE("update_tab1_between: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

    do
    {
		returncode = checkPreparedStatement(pstmt, "UPDATE TAB1 SET I2 = ? , C2 = ? WHERE I1 BETWEEN ? AND ?");
		CE("update_tab1_between: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		SQLDBC_Length indi1=sizeof(i2), indi2=SQLDBC_NTS, indi3=sizeof(i1), indi4=sizeof(ii);
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &i2, &indi1, sizeof(i2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1_between (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_ASCII, &c2, &indi2, sizeof(c2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1_between (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_INT4, &i1, &indi3, sizeof(i1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1_between (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_INT4, &ii, &indi4, sizeof(ii));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1_between (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();

		CE ( "update_tab1_between: UPDATE TAB1 ... BETWEEN" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
    } while ( rollberr (1, pstmt->ERRORCODE) ) ;

    if ( ( pstmt->ERRORCODE == 0 ) && ( rand () % 4 ) )
		commit () ;
    else
		rollback () ;

	conn->releaseStatement(pstmt);
	pstmt = 0;

    check_long_raw ( "UPDATE TAB1 ... BETWEEN" ,  -1 , -1  , i1 , ii )  ;
}
/*------------------------------------------------------------------------*/
void update_tab1_key ()
/*
	UPDATE tab1 KEY with randomized values until successful execution ("equal"-condition)
	COMMIT or ROLLBACK
*/
{
	function_called_counter++; 

    int kplus = rand () % rows ;
	k1 = rand () % rows ;

    movep ( line , col , "UPDATE TAB1 KEY" , k1 ) ;

	pstmt = conn->createPreparedStatement();
	CE("update_tab1_key: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

    do
    {
		returncode = checkPreparedStatement(pstmt, "UPDATE TAB1 SET K1 = ?	WHERE K1 = ?");
		CE("update_tab1_key: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		SQLDBC_Length indi1=sizeof(kplus), indi2=sizeof(k1);
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &kplus, &indi1, sizeof(kplus));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1_key (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &k1, &indi2, sizeof(k1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1_key (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
		CE("update_tab1_key: UPDATE TAB1 KEY", 0, pstmt->ERRORCODE, pstmt->ERRORTEXT);
    } while ( rollberr (1, pstmt->ERRORCODE) ) ;
    if ( pstmt->ERRORCODE == 0 )
		commit () ;
    else
		rollback () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void check_long_raw (const char *c, int k1, int k2, int i1, int i2)
/*
	various SELECT ... WITH LOCK ISOLATION LEVEL 1 depending on function-parameters
	fetch through ResultSet and check values
	in case of discrepancies stop kernel (sqladump)
*/
{
	function_called_counter++; 

	int	kk1 = 0;
	int	kk2 = 0;
	int	ii1 = 0;
	int	ii2 = 0;
	int ok = 0;

	if ( check_longs == 0 ) return ;
    movep (line,col,"CHECK long raw >%s<",c) ;

    cll.len = MAXLONGRAW ;

	pstmt = conn->createPreparedStatement();
	CE("check_long_raw: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	
	for (int i = 0; i < 3; i++)
	{
		if ( k1 >= 0 )
		{
			kk1 = k1 ;
			if ( k1 == k2 )
			{
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
				kk2 = k2 ;
				returncode = checkPreparedStatement(pstmt, "SELECT K1, CLLEN, CLB FROM superu.TAB1 WHERE K1 BETWEEN ? AND ? WITH LOCK ISOLATION LEVEL 1");
				CE("check_long_raw: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
				SQLDBC_Length indi1=sizeof(kk1), indi2=sizeof(kk2);
				returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &kk1, &indi1, sizeof(kk1));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in check_long_raw (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
				returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &kk2, &indi2, sizeof(kk2));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in check_long_raw (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
				returncode = pstmt->execute();
				SE ("check_long_raw: SELECT CLLEN , CLB FROM TAB1, K1 =" ,0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
				if (pstmt->ERRORCODE == 0) {ok = 1; break;}
			}
		}
		else
		{
			ii1 = i1 ;
			ii2 = i2 ;

			returncode = checkPreparedStatement(pstmt, "SELECT K1, CLLEN, CLB FROM superu.TAB1 WHERE I1 BETWEEN ? AND ? WITH LOCK ISOLATION LEVEL 1");
			CE("check_long_raw: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
			SQLDBC_Length indi1=sizeof(ii1), indi2=sizeof(ii2);
			returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &ii1, &indi1, sizeof(ii1));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in check_long_raw (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
			returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &ii2, &indi2, sizeof(ii2));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in check_long_raw (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
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
					int	i ;
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
void insert_tab3 ()
/*
	INSERT tab3 with randomized values until successful execution
	COMMIT or ROLLBACK
*/
{
	function_called_counter++; 

    k3 = rand () % rows ;
    if ( ( k3 % 3 ) == 1 ) k3 /= 10 ;
    i5 = rand () % 10 ;
    sprintf ( c4 , "%dttt%d" , k3 , ( k3 + i5 ) % 100 ) ;
    memset ( c5 , 'x' , sizeof(c5) ) ;
    memcpy ( c5 , c4 , strlen(c4) ) ;
    c5 [ sizeof (c5) - 1 ] = 0 ;

    movep ( line , col , "INSERT TAB3 , K3 = %d" , k3 ) ;

	pstmt = conn->createPreparedStatement();
	CE("insert_tab3: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
    do
    {
		returncode = checkPreparedStatement(pstmt, "INSERT TAB3 VALUES ( ? , ? , ? , ? )");
		CE("insert_tab3: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		SQLDBC_Length indi1=sizeof(k3), indi2=sizeof(i5), indi3=SQLDBC_NTS, indi4=SQLDBC_NTS;
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &k3, &indi1, sizeof(k3));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab3 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &i5, &indi2, sizeof(i5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab3 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_ASCII, &c4, &indi3, sizeof(c4));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab3 (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &c5, &indi4, sizeof(c5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab3 (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
		CE("insert_tab3: INSERT TAB3 ...", 0, pstmt->ERRORCODE, pstmt->ERRORTEXT);
    } while ( rollberr (1, pstmt->ERRORCODE) ) ;

    if ( pstmt->ERRORCODE == 0 )
		commit () ;
    else
		rollback () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void insert_tab3j_1 ()
/*
	INSERT tab3j_1 with randomized values until successful execution
	COMMIT or ROLLBACK
*/
{
	function_called_counter++; 

    k3 = rand () % rows ;
    if ( ( k3 % 3 ) == 1 ) k3 /= 10 ;
    i5 = rand () % 10 ;

	if ( (rand() % 3) == 0 )
		sprintf ( c4 , "itab%d" , ( k3 + i5 ) % 100 ) ;
	else
		sprintf ( c4 , "tab%d" , ( k3 + i5 ) % 100 ) ;

    memset ( c5 , 'x' , sizeof(c5) ) ;
    memcpy ( c5 , c4 , strlen(c4) ) ;
    c5 [ sizeof (c5) - 1 ] = 0 ;

	pstmt = conn->createPreparedStatement();
	CE("insert_tab3j_1: createPreparedStatement", 1, conn->error().getErrorCode(), conn->error().getErrorText());
    movep ( line , col , "INSERT TAB3J_1 , K3 = %d" , k3 ) ;

	do
    {
		returncode = checkPreparedStatement(pstmt, "INSERT TAB3J_1 VALUES ( ? , ? , ? , ? )");
		CE("insert_tab3j_1: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		SQLDBC_Length indi1=sizeof(k3), indi2=sizeof(i5), indi3=SQLDBC_NTS, indi4=SQLDBC_NTS;
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &k3, &indi1, sizeof(k3));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab3j_1 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &i5, &indi2, sizeof(i5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab3j_1 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_ASCII, &c4, &indi3, sizeof(c4));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab3j_1 (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &c5, &indi4, sizeof(c5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab3j_1 (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
		CE("insert_tab3j_1: INSERT TAB3J_1 ...", 0, pstmt->ERRORCODE, pstmt->ERRORTEXT);
    } while ( rollberr (1, pstmt->ERRORCODE) ) ;

	if ( rand () % 4 )
		commit () ;
	else
		rollback () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void insert_tab3j_2 ()
/*
	INSERT tab3j_2 with randomized values until successful execution
	COMMIT or ROLLBACK
*/
{
	function_called_counter++; 

    k3 = rand () % rows ;
    if ( ( k3 % 3 ) == 1 ) k3 /= 10 ;
    i5 = rand () % 10 ;

	if ( (rand () % 3) != 0 )
		sprintf ( c4 , "itab%d" , ( k3 + i5 ) % 100 ) ;
	else
		sprintf ( c4 , "tab%d" , ( k3 + i5 ) % 100 ) ;

    memset ( c5 , 'x' , sizeof(c5) ) ;
    memcpy ( c5 , c4 , strlen(c4) ) ;
    c5 [ sizeof (c5) - 1 ] = 0 ;

    movep ( line , col , "INSERT TAB3J_2 , K3 = %d" , k3 ) ;

	pstmt = conn->createPreparedStatement();
	CE("insert_tab3j_2: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
    do
    {
		returncode = checkPreparedStatement(pstmt, "INSERT TAB3J_2 VALUES ( ? , ? , ? , ? )");
		CE("insert_tab3j_2: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		SQLDBC_Length indi1=sizeof(k3), indi2=sizeof(i5), indi3=SQLDBC_NTS, indi4=SQLDBC_NTS;
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &k3, &indi1, sizeof(k3));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab3j_2 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &i5, &indi2, sizeof(i5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab3j_2 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_ASCII, &c4, &indi3, sizeof(c4));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab3j_2 (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &c5, &indi4, sizeof(c5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in insert_tab3j_2 (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
		CE("insert_tab3j_2: INSERT TAB3J_2 ...", 0, pstmt->ERRORCODE, pstmt->ERRORTEXT);
    } while ( rollberr (1, pstmt->ERRORCODE) ) ;
	if ( rand () % 4 )
		commit () ;
	else
		rollback () ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
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
    do
    {
		returncode = stmt->execute("SELECT COUNT (*) FROM TAB1");
		CE("count_tab1: SELECT COUNT(*) FROM TAB1", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
    } while ( rollberr (1, stmt->ERRORCODE) ) ;

	rs = stmt->getResultSet();
    while ( rs->next() == SQLDBC_OK )
    {
		SQLDBC_Length l;

		returncode = rs->getObject(1, SQLDBC_HOSTTYPE_INT4, &cnt, &l, sizeof(cnt), true);
		if (returncode != SQLDBC_OK) CE("error with getObject in count_tab1 (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
	}

    movep (line,col,"SELECT COUNT(*) FROM TAB1, cnt = %d", cnt) ;
    sleep (1) ;
    prot ( "SELECT COUNT(*) FROM TAB1, cnt = %d\n" , cnt ) ;

    commit () ;
}
/*------------------------------------------------------------------------*/
void delete_tab3 ()
/*
	SELECT COUNT * FROM tab3 until successful execution
	fetch result into "cnt"
	if count > 50 DELETE tab3 with nested query ("equal"-condition)
	COMMIT or ROLLBACK
*/
{
	function_called_counter++; 

    movep (line,col,"SELECT COUNT(*) FROM TAB3") ;
    do
    {
		returncode = stmt->execute("SELECT COUNT (*) FROM TAB3");
		CE("delete_tab3: SELECT COUNT(*) FROM TAB3", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
    } while ( rollberr (1, stmt->ERRORCODE) ) ;


	rs = stmt->getResultSet();
    while ( rs->next() == SQLDBC_OK )
    {
		SQLDBC_Length l;
		returncode = rs->getObject(1, SQLDBC_HOSTTYPE_INT4, &cnt, &l, sizeof(cnt), true);
		if (returncode != SQLDBC_OK) CE("error with getObject in delete_tab3 (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
	}

    movep (line,col,"SELECT COUNT(*) FROM TAB3, cnt = %d", cnt) ;
    sleep (1) ;

    if ( cnt > 50 )
    {
		movep ( line , col , "DELETE TAB3 WHERE ... SELECT" ) ;
		do
		{
			returncode = stmt->execute("DELETE TAB3 WHERE I6 > (SELECT AVG (I6) FROM TAB3)");
			CE("delete_tab3: DELETE TAB3 WHERE ... SELECT", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
		} while ( rollberr (1, stmt->ERRORCODE) ) ;

		if ( stmt->ERRORCODE == 0 )
			commit () ;
		else
			rollback () ;

    }
}
/*------------------------------------------------------------------------*/
void delete_tab3_key ()
/*
	SELECT COUNT * FROM tab3 until successful execution
	fetch result into "cnt"
	if count > 50 DELETE KEY tab3 with nested query ("equal"-condition)
	COMMIT or ROLLBACK
*/
{
	function_called_counter++; 

    i5 = rand () % rows ;
    movep (line,col,"SELECT COUNT(*) FROM TAB3") ;
    do
    {
		returncode = stmt->execute("SELECT COUNT (*) FROM TAB3");
		CE("delete_tab3_key: SELECT COUNT(*) FROM TAB3", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
    } while ( rollberr (1, stmt->ERRORCODE) ) ;

	rs = stmt->getResultSet();
    while ( rs->next() == SQLDBC_OK )
    {
		SQLDBC_Length l;
		returncode = rs->getObject(1, SQLDBC_HOSTTYPE_INT4, &cnt, &l, sizeof(cnt), true);
		if (returncode != SQLDBC_OK) CE("error with getObject in delete_tab3_key (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
	}

    movep (line,col,"SELECT COUNT(*) FROM TAB3, cnt = %d", cnt) ;
    sleep (1) ;
    if ( cnt > 50 )
    {
		movep ( line , col , "DELETE RANGE TAB3, KEY " ) ;
		pstmt = conn->createPreparedStatement();
		CE("delete_tab3_key: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

		do
		{
			returncode = checkPreparedStatement(pstmt, "DELETE TAB3 WHERE I5 = ?");
			CE("delete_tab3_key: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
			SQLDBC_Length indi1=sizeof(i5);
			returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &i5, &indi1, sizeof(i5));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in delete_tab3_key (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
			returncode = pstmt->execute();
			CE("delete_tab3_key: DELETE RANGE TAB3 ... SELECT", 0, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		} while ( rollberr (1, pstmt->ERRORCODE) ) ;

		if ( pstmt->ERRORCODE == 0 )
			commit () ;
		else
			rollback () ;

		conn->releaseStatement(pstmt);
		pstmt = 0;
    }
}
/*------------------------------------------------------------------------*/
void delete_tab3j_1 ()
/*
	SELECT COUNT *, ... FROM tab3j_1 until successful execution
	fetch result into "cnt"
	if count > 50 DELETE tab3j_1 with nested query ("equal"-condition)
	COMMIT or ROLLBACK
*/
{
	function_called_counter++; 

    movep (line,col,"SELECT COUNT(*) FROM TAB3J_1") ;
    do
    {
		returncode = stmt->execute("SELECT COUNT (*), AVG (I6) FROM TAB3J_1");
		CE("delete_tab3j_1: SELECT COUNT(*) FROM TAB3J_1", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
    } while ( rollberr (1, stmt->ERRORCODE) ) ;

	rs = stmt->getResultSet();
    while ( rs->next() == SQLDBC_OK )
    {
		SQLDBC_Length l;

		returncode = rs->getObject(1, SQLDBC_HOSTTYPE_INT4, &cnt, &l, sizeof(cnt), true);
		if (returncode != SQLDBC_OK) CE("error with getObject in delete_tab3j_1 (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
		returncode = rs->getObject(2, SQLDBC_HOSTTYPE_DOUBLE, &avg, &l, sizeof(avg), true);
		if (returncode != SQLDBC_OK) CE("error with getObject in delete_tab3j_1 (col 2)", 1, rs->ERRORCODE, rs->ERRORTEXT);
	}

    if ( cnt > 50 )
    {
		movep ( line , col , "DELETE TAB3J_1 WHERE ..." ) ;

		pstmt = conn->createPreparedStatement();
		CE("delete_tab3j_1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

		do
		{
			returncode = checkPreparedStatement(pstmt, "DELETE TAB3J_1 WHERE I6 > ( ? / 10 )");
			CE("delete_tab3j_1: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
			SQLDBC_Length indi1=sizeof(avg);
			returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_DOUBLE, &avg, &indi1, sizeof(avg));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in delete_tab3j_1 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
			returncode = pstmt->execute();
			CE ("delete_tab3j_1: DELETE TAB3J_1 WHERE ... ",0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
		} while ( rollberr (1, pstmt->ERRORCODE) ) ;

		if ( rand () % 4 )
			commit () ;
		else
			rollback () ;

		conn->releaseStatement(pstmt);
		pstmt = 0;
    }
}
/*------------------------------------------------------------------------*/
void delete_tab3j_2 ()
/*
	SELECT COUNT *, ... FROM tab3j_2 until successful execution
	fetch result into "cnt"
	if count > 50 DELETE tab3j_2 with nested query ("equal"-condition)
	COMMIT or ROLLBACK
*/
{
	function_called_counter++; 

    movep (line,col,"SELECT COUNT(*) FROM TAB3J_2") ;
    do
    {
		returncode = stmt->execute("SELECT COUNT (*), AVG (I6) FROM TAB3J_2");
		CE("delete_tab3j_2: SELECT COUNT(*) FROM TAB3J_2", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
    } while ( rollberr (1, stmt->ERRORCODE) ) ;

	rs = stmt->getResultSet();
    while ( rs->next() == SQLDBC_OK )
    {
		SQLDBC_Length l;

		returncode = rs->getObject(1, SQLDBC_HOSTTYPE_INT4, &cnt, &l, sizeof(cnt), true);
		if (returncode != SQLDBC_OK) CE("error with getObject in delete_tab3j_2 (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
		returncode = rs->getObject(2, SQLDBC_HOSTTYPE_DOUBLE, &avg, &l, sizeof(avg), true);
		if (returncode != SQLDBC_OK) CE("error with getObject in delete_tab3j_2 (col 2)", 1, rs->ERRORCODE, rs->ERRORTEXT);
	}

    if ( cnt > 50 )
    {
		movep ( line , col , "DELETE TAB3J_2 WHERE ... " ) ;
		pstmt = conn->createPreparedStatement();
		CE("delete_tab3j_2: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

		do
		{
			returncode = checkPreparedStatement(pstmt, "DELETE TAB3J_2 WHERE I6 > ( ? / 10 )");
			CE("delete_tab3j_2: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
			SQLDBC_Length indi1=sizeof(avg);
			returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_DOUBLE, &avg, &indi1, sizeof(avg));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in delete_tab3j_2 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
			returncode = pstmt->execute();
			CE ("delete_tab3j_2: DELETE TAB3J_2 WHERE ... ",0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
		} while ( rollberr (1, pstmt->ERRORCODE) ) ;

		if ( rand () % 4 )
			commit () ;
		else
			rollback () ;

		conn->releaseStatement(pstmt);
		pstmt = 0;
    }
}
/*------------------------------------------------------------------------*/
void select_and_check_tab1sl  ()
/*
	SELECT randomized value from superu.TAB1 with LOCK ISOLATION LEVEL 1
	fetch through ResultSet and check whether values are equal
*/
{
	function_called_counter++; 

    int loclen = 0;
	char loclong [100001]={0};

    nrll = rand () % 20 ;

	pstmt = conn->createPreparedStatement();
	CE("select_and_check_tab1sl: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

    do
    {
  		movep ( line ,col , "====== SELECT TAB1SL, nr = %d" , nrll ) ;

		returncode = checkPreparedStatement(pstmt, "SELECT lnll , clll FROM tab1sl "
			"WHERE  nrll = ?  WITH LOCK ISOLATION LEVEL 1");
		CE("select_and_check_tab1sl: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		SQLDBC_Length indi1=sizeof(long);
		returncode = pstmt->bindParameter (1, MY_HOSTTYPE_LONG, &nrll, &indi1, sizeof(nrll));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in select_and_check_tab1sl (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
        CE ("select_and_check_tab1sl: SELECT TAB1SL" ,0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;

        if ( pstmt->ERRORCODE == 0 )
        {
			rs = pstmt->getResultSet();
			scl.len = 100000 ;

			while ( rs->next() == SQLDBC_OK )
			{
				cll.len = MAXLONGRAW ;

				SQLDBC_Length l;
				returncode = rs->getObject(1, MY_HOSTTYPE_LONG, &lnll, &l, sizeof(lnll), true);
				if (returncode != SQLDBC_OK) CE("error with getObject in select_and_check_tab1sl (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
				returncode = rs->getObject(2, SQLDBC_HOSTTYPE_ASCII, &scl.arr, &l, sizeof(scl.arr), true);
				if (returncode != SQLDBC_OK) CE("error with getObject in select_and_check_tab1sl (col 2)", 1, rs->ERRORCODE, rs->ERRORTEXT);

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
		}

    } while ( rollberr (1, pstmt->ERRORCODE) ) ;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void update_tab1sl  ()
/*
	UPDATE tab1sl with randomized values until successful execution
	COMMIT or ROLLBACK
*/
{
	function_called_counter++; 

    int i = 0;
    nrll = rand () % 20 ;

	pstmt = conn->createPreparedStatement();
	CE("update_tab1sl: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
    for ( i = 1 ; i < 10 ; i++ )
    do
    {
        movep ( line ,col , "====== UPDATE TAB1SL, nr = %d" , nrll ) ;
        lnll = 50000 + ( rand () % 6 ) * 10000 - 1 ;
        memset ( scl.arr , 's' , lnll ) ;
		scl.arr[0] = scl.arr[lnll-2] = 't' ;
        scl.arr[lnll-1]= (char) 0 ;
		scl.len = lnll = (int) strlen ( scl.arr ) ;

		returncode = checkPreparedStatement(pstmt, "UPDATE tab1sl SET CLLL = ?, LNLL = ? WHERE  nrll = ?");
		CE("update_tab1sl: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		SQLDBC_Length indi1=SQLDBC_NTS, indi2=sizeof(long), indi3=sizeof(long);
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &scl.arr, &indi1, sizeof(scl.arr));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1sl (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, MY_HOSTTYPE_LONG, &lnll, &indi2, sizeof(lnll));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1sl (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, MY_HOSTTYPE_LONG, &nrll, &indi3, sizeof(nrll));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1sl (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
        CE ("update_tab1sl: UPDATE TAB1SL" ,0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;
    } while ( rollberr (1, pstmt->ERRORCODE) ) ;
    if ( rand () % 2 == 0 )
		commit() ;
    else
		rollback();
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void delete_range_tab3_between ()
/*
	SELECT COUNT *, ... FROM tab3 until successful execution
	fetch result into "cnt"
	if count > 50 DELETE KEY tab3 with nested query ("between"-condition)
	COMMIT or ROLLBACK
*/
{
	function_called_counter++; 

    i5 = rand () % rows ;
    movep (line,col,"SELECT COUNT(*) FROM TAB3") ;
    do
    {
		returncode = stmt->execute("SELECT COUNT (*) FROM TAB3");
		CE("delete_range_tab3_between: SELECT COUNT(*) FROM TAB3", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
    } while ( rollberr (1, stmt->ERRORCODE) ) ;

	rs = stmt->getResultSet();
	while ( rs->next() == SQLDBC_OK )
	{
		SQLDBC_Length l;
		returncode = rs->getObject(1, SQLDBC_HOSTTYPE_INT4, &cnt, &l, sizeof(cnt), true);
		if (returncode != SQLDBC_OK) CE("error with getObject in delete_range_tab3_between (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
	}

	movep (line,col,"SELECT COUNT(*) FROM TAB3, cnt = %d", cnt) ;
    sleep (1) ;

    if ( cnt > 50 )
    {
		movep ( line , col , "DELETE RANGE TAB3, KEY " ) ;
		pstmt = conn->createPreparedStatement();
		CE("delete_range_tab3_between: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
		do
		{
			if ( rand () % 2 )
			{
				returncode = checkPreparedStatement(pstmt, "DELETE TAB3 WHERE I5 BETWEEN ? AND ( ? + ( ? / 5 )  )");
				CE("delete_range_tab3_between: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
				SQLDBC_Length indi1=sizeof(i5), indi2=sizeof(rows);
				returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &i5, &indi1, sizeof(i5));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in delete_range_tab3_between (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
				returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &i5, &indi1, sizeof(i5));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in delete_range_tab3_between (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
				returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_INT4, &rows, &indi2, sizeof(rows));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in delete_range_tab3_between (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
				returncode = pstmt->execute();
			}
			else
			{
				returncode = checkPreparedStatement(pstmt, "DELETE TAB3 WHERE I5 BETWEEN ( ? - ( ? / 5 )  ) AND ?");
				CE("delete_range_tab3_between: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
				SQLDBC_Length indi1=sizeof(i5), indi2=sizeof(rows);
				returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &i5, &indi1, sizeof(i5));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in delete_range_tab3_between (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
				returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &rows, &indi2, sizeof(rows));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in delete_range_tab3_between (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
				returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_INT4, &i5, &indi1, sizeof(i5));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in delete_range_tab3_between (col 6)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
				returncode = pstmt->execute();
			}

			CE ("delete_range_tab3_between: DELETE RANGE TAB3 ... BETWEEN",0, pstmt->ERRORCODE, pstmt->ERRORTEXT) ;

		} while ( rollberr (1, pstmt->ERRORCODE) ) ;

		if ( pstmt->ERRORCODE == 0 && ( rand () % 2 ) )
			commit () ;
		else
			rollback () ;
    }
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void update_tabv1 ()
/*
	SELECT one row FROM tab1 until successful execution
	UPDATE tabv1 with selected values
*/
{
	function_called_counter++; 

    nr = rand () % rows ;
    movep (line,col,"SELECT TAB1 ... WHERE ROWNO <= 1" ) ;
	pstmt = conn->createPreparedStatement();
	CE("update_tabv1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

    do
    {
		returncode = checkPreparedStatement(pstmt, "SELECT K1, I3, CLLEN, CLB INTO ?, ?, ?, ? "
			"FROM TAB1 WHERE K0 = ? AND K1 > ? AND ROWNO <= 1");
		CE("update_tabv1: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		SQLDBC_Length indi1=sizeof(k1), indi2=sizeof(i3), indi7=SQLDBC_NTS, indi8=sizeof(nr);
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &k1, &indi1, sizeof(k1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tabv1 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &i3, &indi2, sizeof(i3));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tabv1 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_INT4, &cll_len, &cll_len_i, sizeof(cll_len));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tabv1 (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_ASCII, &cll.arr, &cll.len, sizeof(cll.arr));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tabv1 (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_ASCII, &k0, &indi7, sizeof(k0));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tabv1 (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (6, SQLDBC_HOSTTYPE_INT4, &nr, &indi8, sizeof(nr));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tabv1 (col 6)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();

		if ( pstmt->ERRORCODE == -6000 )
		{
			rollback () ;
			return ;
		}
		CE ( "update_tabv1: SELECT TAB1 ... WHERE K1 > :nr AND ROWNO <= 1" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
    } while ( rollberr (0, pstmt->ERRORCODE) ) ;

    if ( pstmt->ERRORCODE == 0 )
    {
		i1 = rand () % rows ;

		movep (line,col,"UPDATE TABV1" ) ;
		pstmt2 = conn->createPreparedStatement();
		CE("update_tabv1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

		do
		{
			check_long_raw ( "before UPDATE TABV1" , i3 * 10 , i3 * 10 + 10 , -1 , -1 ) ;
			returncode = checkPreparedStatement(pstmt2, "UPDATE TABV1 SET I1 = ? WHERE I3 = ?");
			CE("update_tabv1: prepare", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
			SQLDBC_Length indi1=sizeof(i1), indi2=sizeof(i3);
			returncode = pstmt2->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &i1, &indi1, sizeof(i1));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tabv1 (col 7)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
			returncode = pstmt2->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &i3, &indi2, sizeof(i3));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tabv1 (col 8)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
			returncode = pstmt2->execute();

			CE ("update_tabv1: UPDATE VIEW TABV1",0, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
		} while ( rollberr (3, pstmt2->ERRORCODE) ) ;

		commit () ;
		check_long_raw ( "after UPDATE TABV1",i3*10,i3*10+10,-1,-1);
		conn->releaseStatement(pstmt2);
		pstmt2 = 0;
    }
	else {
		CE ("update_tabv1: SELECT TAB1 ... WHERE ROWNO <= 1",0, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	}
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void update_tabv1_2 ()
/*
	SELECT one row FROM tabv1 until successful execution
	fetch through ResultSet and UPDATE tabv1 with selected values
*/
{
	function_called_counter++; 

    movep (line,col,"SELECT FROM TABV1 WHERE ... SELECT FROM TABV1") ;
	pstmt = conn->createPreparedStatement();
	CE("update_tabv1_2: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
    do
    {
		returncode = checkPreparedStatement(pstmt, "SELECT K1, I3 FROM TABV1 WHERE K1 = "
			"(SELECT MIN (K1) FROM TABV1 WHERE K1 > ? )");
		CE("update_tabv1_2: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		SQLDBC_Length indi1=sizeof(nr);
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &nr, &indi1, sizeof(nr));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tabv1_2 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
		if ( pstmt->ERRORCODE == -6000 )
        {
			rollback () ;
			return ;
		}

		CE ("update_tabv1_2: SELECT FROM TABV1 WHERE ... SELECT FROM TABV1" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
    } while ( rollberr (1, pstmt->ERRORCODE) ) ;

	if ( pstmt->ERRORCODE == 0 )
    {
		rs = pstmt->getResultSet();
	    while ( rs->next() == SQLDBC_OK )
		{
			SQLDBC_Length l;

			returncode = rs->getObject(1, SQLDBC_HOSTTYPE_INT4, &k1, &l, sizeof(k1), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in update_tabv1_2 (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
			returncode = rs->getObject(2, SQLDBC_HOSTTYPE_INT4, &i3, &l, sizeof(i3), true);
			if (returncode != SQLDBC_OK) CE("error with getObject in update_tabv1_2 (col 2)", 1, rs->ERRORCODE, rs->ERRORTEXT);

			i1 = ( k1 + i3 ) % rows ;

			movep (line,col,"UPDATE TABV1" ) ;

			pstmt2 = conn->createPreparedStatement();
			CE("update_tabv1_2: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

			do
			{
				returncode = checkPreparedStatement(pstmt2, "UPDATE TABV1 SET I1 = ? WHERE I3 = ?");
				CE("update_tabv1_2: prepare", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
				SQLDBC_Length indi1=sizeof(i1), indi2=sizeof(i3);
				returncode = pstmt2->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &i1, &indi1, sizeof(i1));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tabv1_2 (col 2)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
				returncode = pstmt2->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &i3, &indi2, sizeof(i3));
				if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tabv1_2 (col 3)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
				returncode = pstmt2->execute();
				CE ("update_tabv1_2: UPDATE VIEW TABV1",0, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
			} while ( rollberr (3, pstmt2->ERRORCODE) ) ;

			commit () ;
			conn->releaseStatement(pstmt2);
			pstmt2 = 0;
		}

		check_long_raw ( "UPDATE VIEW TABV1",-1,-1, i1 , i1 ) ;
	}
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void mass_upd_tab1 ()
/*
	SELECT FROM tabv1 with randomized values until successful execution
	LOCK TABLE tab1 exclusiv until successful execution
	fetch through ResultSet and UPDATE tab1
	COMMIT or ROLLBACK
*/
{
	function_called_counter++; 

    int     rs_open = 0 ;
    int		table_lock = rand () % 2 ;

    nr = rand () % rows ;

    movep (line,col,"SELECT ERGEB1 FROM TABV1" ) ;

	pstmt = conn->createPreparedStatement();
	CE("mass_upd_tab1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
    do
    {
		if ( rs_open )
		{
			rs->close();
			rs_open = 0 ;
		}

		returncode = checkPreparedStatement(pstmt, "SELECT K1, I1, I2, I3 FROM TABV1 WHERE ( ( I1 + I2 ) / 4 ) > ? ORDER BY I1");
		CE("mass_upd_tab1: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		SQLDBC_Length indi1=sizeof(nr);
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &nr, &indi1, sizeof(nr));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in mass_upd_tab1 (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
		CE ( "mass_upd_tab1: SELECT ... FROM TABV1" , 0, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
    } while ( rollberr (1, pstmt->ERRORCODE) ) ;
	rs = pstmt->getResultSet();
    rs_open = 1 ;

    rolled_back = 0 ;

    if ( table_lock || superusernumber == 2 )
    {
		movep (line,col,"LOCK TABLE TAB1 IN EXCLUSIVE MODE" ) ;
		do
		{
			returncode = stmt->execute("LOCK TABLE TAB1 IN EXCLUSIVE MODE");
			CE("mass_upd_tab1: LOCK TABLE TAB1 IN EXCLUSIVE MODE", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
		} while ( rollberr (1, stmt->ERRORCODE) && ( rolled_back == 0 ) ) ;
	}

    movep (line,col,"FETCH ERGEB1 UPDATE TABV1 , ERR = %d" , stmt->ERRORCODE ) ;

	while ( rs->next() == SQLDBC_OK )
    {
		SQLDBC_Length l;
		returncode = rs->getObject(1, SQLDBC_HOSTTYPE_INT4, &k1, &l, sizeof(k1), true);
		if (returncode != SQLDBC_OK) CE("error with getObject in mass_upd_tab1 (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
		returncode = rs->getObject(2, SQLDBC_HOSTTYPE_INT4, &i1, &l, sizeof(i1), true);
		if (returncode != SQLDBC_OK) CE("error with getObject in mass_upd_tab1 (col 2)", 1, rs->ERRORCODE, rs->ERRORTEXT);
		returncode = rs->getObject(3, SQLDBC_HOSTTYPE_INT4, &i2, &l, sizeof(i2), true);
		if (returncode != SQLDBC_OK) CE("error with getObject in mass_upd_tab1 (col 3)", 1, rs->ERRORCODE, rs->ERRORTEXT);
		returncode = rs->getObject(4, SQLDBC_HOSTTYPE_INT4, &i3, &l, sizeof(i3), true);
		if (returncode != SQLDBC_OK) CE("error with getObject in mass_upd_tab1 (col 4)", 1, rs->ERRORCODE, rs->ERRORTEXT);

		movep (line,col,"Massen-UPDATE TAB1 , K1 = %d" , k1 ) ;
		pstmt2 = conn->createPreparedStatement();
		CE("mass_upd_tab1: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
		do
		{
			returncode = checkPreparedStatement(pstmt2, "UPDATE TAB1 SET I1 = ?, I2 = ? KEY K0 = ?, K1 = ?");
			CE("mass_upd_tab1: prepare", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
			SQLDBC_Length indi1=sizeof(i2), indi2=sizeof(i1), indi3=sizeof(k0), indi4=sizeof(k1);
			returncode = pstmt2->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &i2, &indi1, sizeof(i2));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in mass_upd_tab1 (col 2)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
			returncode = pstmt2->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &i1, &indi2, sizeof(i1));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in mass_upd_tab1 (col 3)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
			returncode = pstmt2->bindParameter (3, SQLDBC_HOSTTYPE_INT4, &k0, &indi3, sizeof(k0));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in mass_upd_tab1 (col 4)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
			returncode = pstmt2->bindParameter (4, SQLDBC_HOSTTYPE_INT4, &k1, &indi4, sizeof(k1));
			if (returncode != SQLDBC_OK) CE("error with bindParameter in mass_upd_tab1 (col 5)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
			returncode = pstmt2->execute();
			CE("mass_upd_tab1: Massen-UPDATE TAB1" , 0, pstmt2->ERRORCODE, pstmt2->ERRORTEXT ) ;

		} while ( rollberr (1, pstmt2->ERRORCODE) && ( rolled_back == 0 ) ) ;
    }

    if ( rand () % 3 == 0 )
    {
		movep (line,col,"Massen-UPDATE TAB1 , ROLLBACK (ok) " ) ;
		rollback () ;
    }
    else
    {
		commit () ;
    }

	rs->close();
    rs_open = 0 ;

	conn->releaseStatement(pstmt2);
	pstmt2 = 0;
	conn->releaseStatement(pstmt);
	pstmt = 0;

    check_long_raw ( "Massen Update TAB1" , -1,-1 , 1 , rows ) ;
}
/*------------------------------------------------------------------------*/
void delete_rollback_tab4 ()
/*
	SELECT COUNT * FROM tab4
	DELETE tab4 until successful execution
	ROLLBACK
	COMMIT
	SELECT COUNT * FROM tab4
*/
{
	function_called_counter++; 

	int cnt_local = 0;
	int ok = 0;

	movep (line,col,"SELECT COUNT(*) FROM TAB4") ;

	pstmt2 = conn->createPreparedStatement();
	CE("delete_rollback_tab4: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
    {
		returncode = checkPreparedStatement(pstmt2, "SELECT COUNT (*) INTO ? FROM TAB4");
		CE("delete_rollback_tab4: prepare", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
		SQLDBC_Length indi1=sizeof(cnt);
		returncode = pstmt2->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &cnt, &indi1, sizeof(cnt));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in delete_rollback_tab4 (col 1)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
		returncode = pstmt2->execute();
		CE("delete_rollback_tab4: SELECT COUNT(*) FROM TAB4", 0, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
    }while ( rollberr (1, pstmt2->ERRORCODE )) ;
	conn->releaseStatement(pstmt2);
	pstmt2 = 0;

	cnt_local = cnt;

	movep (line,col,"DELETE TAB4" ) ;

	for (int i = 0; i < 6; i++)
	{
		returncode = stmt->execute("DELETE TAB4");
		CE("delete_rollback_tab4: DELETE TAB4", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
		if (stmt->ERRORCODE == 0) {ok = 1; break;}
    } 

    prot ("DELETE TAB4, ERR = %d\n", stmt->ERRORCODE);

    sleep ( 5 ) ;

    if ( (stmt->ERRORCODE != 350) )
    {
		CE ("delete_rollback_tab4: DELETE TAB4",1, stmt->ERRORCODE, stmt->ERRORTEXT);
    }

    movep (line,col,"DELETE TAB4 : Rollback (ok)" ) ;

    rollback () ;

    prot  ( "DELETE TAB4 (Rollback) : %d\n" , conn->ERRORCODE ) ;
    CE ("delete_rollback_tab4: DELETE TAB4 (Rollback)",1, conn->ERRORCODE, conn->ERRORTEXT);

	commit();

    movep (line,col,"SELECT COUNT(*) FROM TAB4") ;

	pstmt = conn->createPreparedStatement();
	CE("delete_rollback_tab4: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
    {
		returncode = checkPreparedStatement(pstmt, "SELECT COUNT (*) INTO ? FROM TAB4");
		CE("delete_rollback_tab4: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		SQLDBC_Length indi1=sizeof(cnt);
		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &cnt, &indi1, sizeof(cnt));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in delete_rollback_tab4 (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->execute();
		CE("delete_rollback_tab4: SELECT COUNT(*) FROM TAB4", 0, pstmt->ERRORCODE, pstmt->ERRORTEXT);
    }while ( rollberr (1, pstmt->ERRORCODE )) ;

	movep (line,col,"SELECT COUNT(*) FROM TAB4, cnt = %d", cnt) ;

    if ( cnt != cnt_local )
	{
		prot("before ROLLBACK: cnt = %d BUT after ROLLBACK: cnt = %d\n", cnt_local, cnt);
		DE ( "delete_rollback_tab4: SELECT COUNT(*) FROM TAB4 differs before and after ROLLBACK\n" , 1, pstmt->ERRORCODE, pstmt->ERRORTEXT ) ;
	}
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void diagnose_analyze_on ()
/*
	DIAGNOSE ANALYSE ON
	COMMIT
*/
{
	function_called_counter++; 

	movep (line,col,"DIAGNOSE ANALYZE ON" ) ;
    do
    {
		returncode = stmt->execute("DIAGNOSE ANALYZE ON");
		CE ("diagnose_analyze_on: DIAGNOSE ANALYZE ON", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
    }
	while ( rollberr (1, stmt->ERRORCODE) ) ;
	commit();
}
/*------------------------------------------------------------------------*/
void diagnose_analyze_count_on ()
/*
	DIAGNOSE ANALYSE COUNT ON
	COMMIT
*/
{
	function_called_counter++; 

	movep (line,col,"DIAGNOSE ANALYZE COUNT ON" ) ;
    do
    {
		returncode = stmt->execute("DIAGNOSE ANALYZE COUNT ON");
		CE ("diagnose_analyze_count_on: DIAGNOSE ANALYZE COUNT ON",0, stmt->ERRORCODE, stmt->ERRORTEXT);
    }
	while ( rollberr (1, stmt->ERRORCODE) ) ;
	commit () ;
}
/*------------------------------------------------------------------------*/
void diagnose_analyze_count_off ()
/*
	DIAGNOSE ANALYSE COUNT OFF
	COMMIT
*/
{
	function_called_counter++; 

	movep (line,col,"DIAGNOSE ANALYZE COUNT OFF" ) ;

    do
    {
		returncode = stmt->execute("DIAGNOSE ANALYZE COUNT OFF");
		CE ("diagnose_analyze_count_off: DIAGNOSE ANALYZE COUNT OFF",0, stmt->ERRORCODE, stmt->ERRORTEXT);
    }
	while ( rollberr (1, stmt->ERRORCODE) ) ;
	commit () ;
}
/*------------------------------------------------------------------------*/
void diagnose_analyze_off ()
/*
	DIAGNOSE ANALYSE OFF
	COMMIT
*/
{
	function_called_counter++; 

	movep (line,col,"DIAGNOSE ANALYZE OFF" ) ;
    do
    {
		returncode = stmt->execute("DIAGNOSE ANALYZE OFF");
		CE ("diagnose_analyze_off: DIAGNOSE ANALYZE OFF",0, stmt->ERRORCODE, stmt->ERRORTEXT);
    }
	while ( rollberr (1, stmt->ERRORCODE) ) ;
	commit () ;
}
/*------------------------------------------------------------------------*/
void diagnose_analyze_clear(int clAll)
/*
	DIAGNOSE ANLYSE CLEAR DATA/COMMAND/ALL until successful execution
	COMMIT
*/
{
	function_called_counter++; 

	int	clearKind = 0;
	const char *cd[3] = { "DATA","COMMAND","ALL" } ;
	char clearString [100]={0};

	if ( clAll == 0 )
		clearKind = rand() % 3 ;
	else
		clearKind = 2 ;
	sprintf ( clearString , "DIAGNOSE ANALYZE CLEAR %s", cd [clearKind] ) ;
	movep (line, col, clearString);
    do
    {
		switch (clearKind)
		{
		case 0:
			returncode = stmt->execute("DIAGNOSE ANALYZE CLEAR DATA");
			break ;
		case 1:
			returncode = stmt->execute("DIAGNOSE ANALYZE CLEAR COMMAND");
			break ;
		case 2:
			returncode = stmt->execute("DIAGNOSE ANALYZE CLEAR ALL");
			break ;
		}
		CE (clearString,0, stmt->ERRORCODE, stmt->ERRORTEXT);
    }
	while ( rollberr (1, stmt->ERRORCODE) ) ;
	prot ("%s\n", clearString);
	commit();
}
/*------------------------------------------------------------------------*/
void protDiagnoseFiles ()
/*
	SELECT COUNT * FROM various system tables
	COMMIT
*/
{
	function_called_counter++; 

	int	dCnt = 0;

	pstmt = conn->createPreparedStatement();
	CE("protDiagnoseFiles: createPreparedStatement", 1, conn->error().getErrorCode(), conn->error().getErrorText());
	returncode = checkPreparedStatement(pstmt, "SELECT COUNT (*) INTO ? FROM SYSMONITOR");
	CE("protDiagnoseFiles: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	SQLDBC_Length indi1=sizeof(dCnt);
	returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &dCnt, &indi1, sizeof(dCnt));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in protDiagnoseFiles (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->execute();
	prot ("SELECT COUNT (*) FROM SYSMONITOR: %d\n",dCnt);
	conn->releaseStatement(pstmt);
	pstmt = 0;

	pstmt = conn->createPreparedStatement();
	CE("protDiagnoseFiles: createPreparedStatement", 1, conn->error().getErrorCode(), conn->error().getErrorText());
	returncode = checkPreparedStatement(pstmt, "SELECT COUNT (*) INTO ? FROM SYSMONDATA");
	CE("protDiagnoseFiles: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	SQLDBC_Length indi2=sizeof(dCnt);
	returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &dCnt, &indi2, sizeof(dCnt));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in protDiagnoseFiles (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->execute();
	prot ("SELECT COUNT (*) FROM SYSMONDATA: %d\n",dCnt);
	conn->releaseStatement(pstmt);
	pstmt = 0;

	pstmt = conn->createPreparedStatement();
	CE("protDiagnoseFiles: createPreparedStatement", 1, conn->error().getErrorCode(), conn->error().getErrorText());
	returncode = checkPreparedStatement(pstmt, "SELECT COUNT (*) INTO ? FROM SYSPARSEID");
	CE("protDiagnoseFiles: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	SQLDBC_Length indi3=sizeof(dCnt);
	returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &dCnt, &indi3, sizeof(dCnt));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in protDiagnoseFiles (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->execute();
	prot ("SELECT COUNT (*) FROM SYSPARSEID: %d\n",dCnt);
	conn->releaseStatement(pstmt);
	pstmt = 0;

	pstmt = conn->createPreparedStatement();
	CE("protDiagnoseFiles: createPreparedStatement", 1, conn->error().getErrorCode(), conn->error().getErrorText());
	returncode = checkPreparedStatement(pstmt, "SELECT COUNT (*) INTO ? FROM SYSDATA_ANALYZE");
	CE("protDiagnoseFiles: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	SQLDBC_Length indi4=sizeof(dCnt);
	returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &dCnt, &indi4, sizeof(dCnt));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in protDiagnoseFiles (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->execute();
	prot ("SELECT COUNT (*) FROM SYSDATA_ANALYZE: %d\n",dCnt);
	conn->releaseStatement(pstmt);
	pstmt = 0;

	pstmt = conn->createPreparedStatement();
	CE("protDiagnoseFiles: createPreparedStatement", 1, conn->error().getErrorCode(), conn->error().getErrorText());
	returncode = checkPreparedStatement(pstmt, "SELECT COUNT (*) INTO ? FROM SYSCMD_ANALYZE");
	CE("protDiagnoseFiles: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	SQLDBC_Length indi5=sizeof(dCnt);
	returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &dCnt, &indi5, sizeof(dCnt));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in protDiagnoseFiles (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->execute();
	prot ("SELECT COUNT (*) FROM SYSCMD_ANALYZE: %d\n",dCnt);
	conn->releaseStatement(pstmt);
	pstmt = 0;

	commit() ;
}
/*------------------------------------------------------------------------*/
void work_schema()
/*
	SELECT CURRENT_SCHEMA (old_schema)
	CREATE and SET CURRENT_SCHEMA (test_schema)
	CREATE TABLE in test_schema
	INSERT to new table in test_schema
	SELECT from table in test_schema
	check whether inserted values are equal to values from resultSet
	switch back to old_schema
*/
{
	char old_schema[50]={0};
	char new_schema[50]={0};
	sprintf(new_schema, "schema_%s", id);
	char new_schema_tab[50]={0};
	sprintf(new_schema_tab, "schema_tab_%s", id) ;
	char clearString[150]={0};
	SQLDBC_Length indi=SQLDBC_NTS;

	/* Randomized values that will be inserted in the table */
	int ws_k0 = pid;
	int ws_i0 = rand () % pid;
	char ws_c0[20]={0};
	sprintf(ws_c0, "%d", rand()%128);
	long ws_l0 = (rand () % pid) * (rand () % pid);
	char ws_vc0[15]={0};
	sprintf(ws_vc0, "%X", rand()%256);

	/* Variables that contains the values from the resultSet */
	int get_k0 = 0;
	int get_i0 = 0;
	char get_c0[20]={0};
	long get_l0 = 0;
	char get_vc0[15]={0};

	movep (line, col, "WORK SCHEMA ...");
	
	/* Save the current_schema to go back later */
	pstmt2 = conn->createPreparedStatement();
	CE("work_schema: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	returncode = checkPreparedStatement(pstmt2, "SELECT CURRENT_SCHEMA INTO ? FROM DUAL");
	CE("work_schema: prepare", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
	returncode = pstmt2->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &old_schema, &indi, sizeof(old_schema));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in work_schema (col 1)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
	returncode = pstmt2->execute();
	CE("work_schema: SELECT CURRENT_SCHEMA ...", 0, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);

	/* Drop existing schema, create new schema, switch to new schema and create table*/
	sprintf(clearString, "DROP SCHEMA %s", new_schema);
	returncode = stmt->execute(clearString);
	//CE ("work_schema: DROP SCHEMA", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	sprintf(clearString, "CREATE SCHEMA %s", new_schema);
	returncode = stmt->execute(clearString);
	CE ("work_schema: CREATE SCHEMA", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	sprintf(clearString, "SET CURRENT_SCHEMA = %s", new_schema);
	returncode = stmt->execute(clearString);
	CE ("work_schema: SET CURRENT_SCHEMA ...", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
	sprintf(clearString, "CREATE TABLE %s (K0 FIXED(10) KEY, I0 FIXED(10), C0 CHAR(20), L0 LONG, VC0 VARCHAR(15))", new_schema_tab);
	returncode = stmt->execute(clearString);
	CE ("work_schema: CREATE TABLE ...", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	/* Insert values for one row in new table */
	pstmt = conn->createPreparedStatement();
	CE("work_schema: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	sprintf(clearString, "INSERT %s SET K0 = ?, I0 = ?, C0 = ?, L0 = ?, VC0 = ?", new_schema_tab);
	returncode = checkPreparedStatement(pstmt, clearString);
	CE("work_schema: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	SQLDBC_Length indi1=sizeof(ws_k0), indi2=sizeof(ws_i0), indi3=SQLDBC_NTS, indi4=sizeof(long), indi5=SQLDBC_NTS;
	returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &ws_k0, &indi1, sizeof(ws_k0));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in work_schema (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &ws_i0, &indi2, sizeof(ws_i0));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in work_schema (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_ASCII, &ws_c0, &indi3, sizeof(ws_c0));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in work_schema (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (4, MY_HOSTTYPE_LONG, &ws_l0, &indi4, sizeof(ws_l0));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in work_schema (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_ASCII, &ws_vc0, &indi5, sizeof(ws_vc0));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in work_schema (col 6)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->execute();
	CE("work_schema: INSERT test_schema_tab1 ...", 0, pstmt->ERRORCODE, pstmt->ERRORTEXT);

	if (returncode == SQLDBC_OK)
	{
		/* Select values (from resultSet) and check if they are equal with inserted values */
		sprintf(clearString, "SELECT K0, I0, C0, L0, VC0 FROM %s", new_schema_tab);
		returncode = stmt->execute(clearString);
		CE ("work_schema: SELECT ...", 0, stmt->ERRORCODE, stmt->ERRORTEXT);
		
		if (returncode == SQLDBC_OK)
		{
			rs = stmt->getResultSet();
			if (rs->next() == SQLDBC_OK)
			{
				SQLDBC_Length l;

				returncode = rs->getObject(1, SQLDBC_HOSTTYPE_INT4, &get_k0, &l, sizeof(get_k0), true);
				if (returncode != SQLDBC_OK) CE("error with getObject in work_schema (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
				returncode = rs->getObject(2, SQLDBC_HOSTTYPE_INT4, &get_i0, &l, sizeof(get_i0), true);
				if (returncode != SQLDBC_OK) CE("error with getObject in work_schema (col 2)", 1, rs->ERRORCODE, rs->ERRORTEXT);
				returncode = rs->getObject(3, SQLDBC_HOSTTYPE_ASCII, &get_c0, &l, sizeof(get_c0), true);
				if (returncode != SQLDBC_OK) CE("error with getObject in work_schema (col 3)", 1, rs->ERRORCODE, rs->ERRORTEXT);
				returncode = rs->getObject(4, MY_HOSTTYPE_LONG, &get_l0, &l, sizeof(get_l0), true);
				if (returncode != SQLDBC_OK) CE("error with getObject in work_schema (col 4)", 1, rs->ERRORCODE, rs->ERRORTEXT);
				returncode = rs->getObject(5, SQLDBC_HOSTTYPE_ASCII, &get_vc0, &l, sizeof(get_vc0), true);
				if (returncode != SQLDBC_OK) CE("error with getObject in work_schema (col 5)", 1, rs->ERRORCODE, rs->ERRORTEXT);

				if (ws_k0 != get_k0 || ws_i0 != get_i0 || strcmp(ws_c0, get_c0) || ws_l0 != get_l0 || strcmp(ws_vc0, get_vc0))
				{
					prot("There is a differences in written and read objects in work_schema().\n");
					prot("inserted K0 : %d - read K0 : %d\n", ws_k0, get_k0);
					prot("inserted I0 : %d - read I0 : %d\n", ws_i0, get_i0);
					prot("inserted C0 : %s - read C0 : %s\n", ws_c0, get_c0);
					prot("inserted L0 : %d - read L0 : %d\n", ws_l0, get_l0);
					prot("inserted VC0: %s - read VC0: %s\n", ws_vc0, get_vc0);
					Sqldump("work_schema()", -1, "error with differences in resultSet", conn);
				}
			}
			else
			{
				prot("No check for differences in inserted and read objects because no resultSet available.\n");
				Sqldump("work_schema()", 100, "no resultSet available", conn);
			}
		}
		else
		{
			prot("No check for differences in inserted and read objects because no data was selected.\n");
			Sqldump("work_schema()", -1, "no data selected", conn);
		}
	}
	else
	{
		prot("No check for differences in written and read objects because no data was inserted.\n");
		Sqldump("work_schema()", -1, "no data inserted", conn);
	}

	pstmt3 = conn->createPreparedStatement();
	CE("work_schema: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	sprintf(clearString, "SET CURRENT_SCHEMA = %s", old_schema) ;
	returncode = checkPreparedStatement(pstmt3, clearString);
	CE("work_schema: prepare", 1, pstmt3->ERRORCODE, pstmt3->ERRORTEXT);
	returncode = pstmt3->bindParameter (1, SQLDBC_HOSTTYPE_ASCII, &old_schema, &indi, sizeof(old_schema));
	if (returncode != SQLDBC_OK) CE("error with bindParameter in work_schema (col 7)", 1, pstmt3->ERRORCODE, pstmt3->ERRORTEXT);
	returncode = pstmt3->execute();
	CE("work_schema: SET CURRENT_SCHEMA back ...", 0, pstmt3->ERRORCODE, pstmt3->ERRORTEXT);

	sprintf(clearString, "DROP SCHEMA %s", new_schema);
	returncode = stmt->execute(clearString);
	CE ("work_schema: DROP SCHEMA", 0, stmt->ERRORCODE, stmt->ERRORTEXT);

	commit();

	conn->releaseStatement(pstmt2);
	pstmt2 = 0;
	conn->releaseStatement(pstmt);
	pstmt = 0;
	conn->releaseStatement(pstmt3);
	pstmt3 = 0;
}
/*------------------------------------------------------------------------*/
void sel_from_roots ()
/*
	SELECT * FROM ROOTS until successful execution
*/
{
	function_called_counter++; 

    movep (line,col,"SELECT * FROM ROOTS" ) ;
    do
    {
		returncode=stmt->execute("SELECT * FROM ROOTS");
		CE ("sel_from_roots: SELECT * FROM ROOTS" , 0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
    } while ( rollberr (1, stmt->ERRORCODE) ) ;
}
/*------------------------------------------------------------------------*/
void update_tab1uc ()
/*
	UPDATE tab1uc with randomized values until successful execution
	COMMIT
*/
{
	function_called_counter++; 

//prot("USE update_tab1uc\n");
	int y = 0;
	i2 = rand () % 500 + 500;
	i3 = rand () % 100;
	i4 = i3 + 100;
	i6 = (rand () % 8) + 2;
	for (y = 0; y < sizeof(uchar1)/sizeof(UCSCHAR) - 1; y++)
	{
		uchar1[y] = (UCSCHAR)0x2603;	//Unicode 2603 = Schneemann-Symbol (Miscellaneous Symbol)
	}
	uchar1[0] = (UCSCHAR)0x2696;	//Unicode 2696 = Waagen-Symbol für Gerechtigkeit (Miscellaneous Symbol)  // nicht wirklich sichtbar
	uchar1[sizeof(uchar1)/sizeof(UCSCHAR)-1] = 0;
	for (y = 0; y < sizeof(uchar2)/sizeof(UCSCHAR) - 1; y++)
	{
		uchar2[y] = (UCSCHAR)0x1465;	//Unicode 1465 = neg. Smiley als Zeichen 90° gedreht (Unified Canadian Aboriginal Syllabics) 
		// nicht wirklich sichtbar
	}
	uchar2[sizeof(uchar2)/sizeof(UCSCHAR)-1] = 0;
	for (y = 0; y < sizeof(uchar3)/sizeof(UCSCHAR) - 1; y = y+2)
	{
		uchar3[y] = (UCSCHAR)0x4e14;	//Unicode 4e14 = chin. Schriftzeichen (CJK Unified Ideographs)
	}
	uchar3[sizeof(uchar3)/sizeof(UCSCHAR)-1] = 0;

	movep ( line ,col , "UPDATE TAB1UC ..." , "") ;

	pstmt = conn->createPreparedStatement();
	CE("update_tab1uc: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	do
    {
		returncode = checkPreparedStatement(pstmt, "UPDATE TAB1UC SET CU1 = ?, I2 = ?, CU4 = ?, I6 = ?, VCU7 = ? WHERE I3 > ?");
		CE("update_tab1uc: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=SQLDBC_NTS, indi2=sizeof(i2), indi3=SQLDBC_NTS, indi4=sizeof(i6), indi5=SQLDBC_NTS, indi6=sizeof(i3);

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_UCS2_NATIVE, &uchar1, &indi1, sizeof(uchar1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1uc (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &i2, &indi2, sizeof(i2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1uc (col 2)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (3, SQLDBC_HOSTTYPE_UCS2_NATIVE, &uchar2, &indi3, sizeof(uchar2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1uc (col 3)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (4, SQLDBC_HOSTTYPE_INT4, &i6, &indi4, sizeof(i6));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1uc (col 4)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (5, SQLDBC_HOSTTYPE_UCS2_NATIVE, &uchar3, &indi5, sizeof(uchar3));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1uc (col 5)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
		returncode = pstmt->bindParameter (6, SQLDBC_HOSTTYPE_INT4, &i3, &indi6, sizeof(i3));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in update_tab1uc (col 6)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
		CE("update_tab1uc: UPDATE TAB1UC ...", 0, pstmt->ERRORCODE, pstmt->ERRORTEXT);
    }while ( rollberr (1, pstmt->ERRORCODE )) ;

	commit () ;

	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void del_ins_tab1uc ()
/*
	DELETE tab1uc randomized values until successful execution
	COMMIT or ROLLBACK
*/
{
	function_called_counter++; 

//prot("USE del_ins_tab1uc\n");
	int loccnt = 0;
	int diff = 0;
	int nrl = 0;
	int y = 0;

	movep ( line , col , "SELECT COUNT (*) INTO ? FROM TAB1UC", "" ) ;
	pstmt = conn->createPreparedStatement();	
	CE("del_ins_tab1uc: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
    do
    {
		returncode = checkPreparedStatement(pstmt, "SELECT COUNT (*) INTO ? FROM TAB1UC");
		CE("del_ins_tab1uc: prepare", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(cnt);

		returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &cnt, &indi1, sizeof(cnt)); 
		if (returncode != SQLDBC_OK) CE("error with bindParameter in del_ins_tab1uc (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

		returncode = pstmt->execute();
		CE("del_ins_tab1uc: SELECT COUNT (*) INTO ? FROM TAB1UC", 0, pstmt->ERRORCODE, pstmt->ERRORTEXT);
    } while ( rollberr (1, pstmt->ERRORCODE) ) ;
	loccnt = cnt;

	movep ( line , col , "DELETE TAB1UC WHERE I2 > ( SELECT AVG ( I2 + I3 ) FROM TAB1UC )", "" ) ;
	do
	{
		returncode=stmt->execute("DELETE TAB1UC WHERE I2 > ( SELECT AVG ( I2 + I3 ) FROM TAB1UC )");
		CE ("del_ins_tab1uc: DELETE TAB1UC WHERE I2 > ( SELECT AVG ( I2 + I3 ) FROM TAB1UC )" , 0, stmt->ERRORCODE, stmt->ERRORTEXT ) ;
	} while ( rollberr (2, stmt->ERRORCODE) ) ;

	movep ( line , col , "SELECT COUNT (*) INTO ? FROM TAB1UC", "" ) ;
	pstmt2 = conn->createPreparedStatement();	
	CE("del_ins_tab1uc: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
    do
    {
		returncode = checkPreparedStatement(pstmt2, "SELECT COUNT (*) INTO ? FROM TAB1UC");
		CE("del_ins_tab1uc: prepare", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);

		SQLDBC_Length indi1=sizeof(cnt);

		returncode = pstmt2->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &cnt, &indi1, sizeof(cnt)); 
		if (returncode != SQLDBC_OK) CE("error with bindParameter in del_ins_tab1uc (col 2)", 1, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);

		returncode = pstmt2->execute();
		CE("del_ins_tab1uc: SELECT COUNT (*) INTO ? FROM TAB1UC", 0, pstmt2->ERRORCODE, pstmt2->ERRORTEXT);
    } while ( rollberr (1, pstmt2->ERRORCODE) ) ;
	diff = (loccnt - cnt);	

	pstmt3 = conn->createPreparedStatement();	
	CE("del_ins_tab1uc: createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	returncode = checkPreparedStatement(pstmt3, "INSERT TAB1UC VALUES (?, ?, ?, ?, 42, ?, ?, ?, ?)");
	CE("del_ins_tab1uc: prepare", 1, pstmt3->ERRORCODE, pstmt3->ERRORTEXT);

	for (y = 0; y < sizeof(uchar1)/sizeof(UCSCHAR) - 1; y++)
	{
		uchar1[y] = (UCSCHAR)0x2614;	//Unicode 2614 = Regenschirm-Symbol (Miscellaneous Symbol)	// nicht wirklich sichtbar
	}
	uchar1[sizeof(uchar1)/sizeof(UCSCHAR)-1] = 0;

	for (y = 0; y < sizeof(uchar2)/sizeof(UCSCHAR) - 1; y++)
	{
		uchar2[y] = (UCSCHAR)0x1572;	//Unicode 1572 = Zeichen (Unified Canadian Aboriginal Syllabics)	// nicht wirklich sichtbar
	}
	uchar2[sizeof(uchar2)/sizeof(UCSCHAR)-1] = 0;

	for (y = 0; y < sizeof(uchar3)/sizeof(UCSCHAR) - 1; y++)
	{
		uchar3[y] = (UCSCHAR)0x4ed0;	//Unicode 4ed0 = chin. Schriftzeichen (CJK Unified Ideographs)
	}
	uchar3[sizeof(uchar3)/sizeof(UCSCHAR)-1] = 0;

	for (y = 0; y < sizeof(uchar4)/sizeof(UCSCHAR) - 1; y++)
	{
		uchar4[y] = (UCSCHAR)0x263b;	//Unicode 263b = Smiley-Symbol [Negativ-Bild] (Miscellaneous Symbol)
	}
	uchar4[sizeof(uchar4)/sizeof(UCSCHAR)-1] = 0;

	for (y = 0; y < sizeof(uchar5)/sizeof(UCSCHAR) - 1; y++)
	{
		uchar5[y] = (UCSCHAR)0x090b;	//Unicode 090b = Schriftsymbol (Devanagari)
	}
	uchar5[sizeof(uchar5)/sizeof(UCSCHAR)-1] = 0;

	for (nrl = 0 ; nrl < (diff + (rand() % 10)) ; nrl++ ) 
	{
		i3 = rand() % 100;
		i6 = rand() % 2;

		SQLDBC_Length indi1=SQLDBC_NTS, indi2=sizeof(nrl) , indi3=sizeof(i3) , indi4=SQLDBC_NTS,  
			indi5=sizeof(i6), indi6=SQLDBC_NTS, indi7=SQLDBC_NTS, indi8=SQLDBC_NTS;

		returncode = pstmt3->bindParameter (1, SQLDBC_HOSTTYPE_UCS2_NATIVE, &uchar1, &indi1, sizeof(uchar1));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in del_ins_tab1uc (col 3)", 1, pstmt3->ERRORCODE, pstmt3->ERRORTEXT);
		returncode = pstmt3->bindParameter (2, SQLDBC_HOSTTYPE_INT4, &nrl, &indi2, sizeof(nrl));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in del_ins_tab1uc (col 4)", 1, pstmt3->ERRORCODE, pstmt3->ERRORTEXT);
		returncode = pstmt3->bindParameter (3, SQLDBC_HOSTTYPE_INT4, &i3, &indi3, sizeof(i3));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in del_ins_tab1uc (col 5)", 1, pstmt3->ERRORCODE, pstmt3->ERRORTEXT);
		returncode = pstmt3->bindParameter (4, SQLDBC_HOSTTYPE_UCS2_NATIVE, &uchar2, &indi4, sizeof(uchar2));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in del_ins_tab1uc (col 6)", 1, pstmt3->ERRORCODE, pstmt3->ERRORTEXT);
		returncode = pstmt3->bindParameter (5, SQLDBC_HOSTTYPE_INT4, &i6, &indi5, sizeof(i6));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in del_ins_tab1uc (col 7)", 1, pstmt3->ERRORCODE, pstmt3->ERRORTEXT);
		returncode = pstmt3->bindParameter (6, SQLDBC_HOSTTYPE_UCS2_NATIVE, &uchar3, &indi6, sizeof(uchar3));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in del_ins_tab1uc (col 8)", 1, pstmt3->ERRORCODE, pstmt3->ERRORTEXT);
		returncode = pstmt3->bindParameter (7, SQLDBC_HOSTTYPE_UCS2_NATIVE, &uchar4, &indi7, sizeof(uchar4));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in del_ins_tab1uc (col 9)", 1, pstmt3->ERRORCODE, pstmt3->ERRORTEXT);
		returncode = pstmt3->bindParameter (8, SQLDBC_HOSTTYPE_UCS2_NATIVE, &uchar5, &indi8, sizeof(uchar5));
		if (returncode != SQLDBC_OK) CE("error with bindParameter in del_ins_tab1uc (col 10)", 1, pstmt3->ERRORCODE, pstmt3->ERRORTEXT);

		returncode = pstmt3->execute();
		CE("del_ins_tab1uc: INSERT TAB1UC ...", 1, pstmt3->ERRORCODE, pstmt3->ERRORTEXT);			
	}

	commit () ;

	conn->releaseStatement(pstmt3);
	pstmt3 = 0;
	conn->releaseStatement(pstmt2);
	pstmt2 = 0;
	conn->releaseStatement(pstmt);
	pstmt = 0;
}
/*------------------------------------------------------------------------*/
void pe (const char *s, int leave, int errorcode, const char *errortext)
/*
	pe = "print error"
	checks the error from the last command
	with "leave"-flag or some special errors stop kernel (sqladump)
*/
{
    char    zeile [200]={0};

    sprintf ( zeile , "%s :  ERR = %d" , s , errorcode ) ;
    movep ( line , col , zeile ) ;

	if ( protocol || ( errorcode > 200 ) || ( errorcode < 0 ) )
		print_f (s, errorcode, errortext) ;

	if ( errorcode == 650 || errorcode == 710 )      /* Location down / Shutdown */
		finish();

/*NEW because sqladump breaks down connection and with connection break down no further statement should be sent*/
	if (errorcode == -10807 || errorcode == -10709)
	{
		print_f(s, errorcode, errortext);
		remove( actfile );
		exit(1);
	}
/*ENDNEW*/

	if ( leave || errorcode == -108 || errorcode == -6000 || errorcode == -28007 || errorcode == -28804 ||
		( errorcode <= -9000 && errorcode >= -10000 ) || errorcode == -7500 ||
		errorcode == -8 /* || ( errorcode != -809 && ( errorcode <= -800 && errorcode > -900 ) ) precompiler*/ )
    {
		print_f (s, errorcode, errortext) ;
		Sqldump (s, errorcode, errortext, conn) ;
    }
}
/*------------------------------------------------------------------------*/
void cwr_ok ()
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
    char    zeile [200]={0};

	va_list paramlist;
	va_start(paramlist, s1);

    vsprintf ( zeile , s1 , paramlist ) ;

	va_end(paramlist);

#ifdef _WIN32
    sprintf (buf , "\033[%d;%dH\033[K%s\r\r",line,col,zeile);
	sflushf (buf) ;
    sprintf (buf , "\033[21;1H") ;
	sflushf (buf) ;
#else
    if ( fo )
    {
		fflush (fo) ;
		fprintf (fo , "\033[%d;%dH\033[K%s\r\r",line,col,zeile);
		fflush (fo) ;
		fprintf (fo , "\033[21;1H") ;
		fflush (fo) ;
    }
#endif
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
				return ( 1 ) ;
				break ;
			case 1 :
				if ( ( rand () % 4 ) == 0 )
				{
					rollback () ;
					prot ("Transaction rolled back, loop again\n");
					rolled_back = 1 ;
					sleep ( 1 ) ;
				}
				return ( 1 ) ;
				break ;
			case 2 :
				rollback () ;
				sleep ( 1 ) ;
				prot ("Transaction rolled back, loop again\n") ;
				rolled_back = 1 ;
				return ( 1 ) ;
				break ;
			case 3 :
				sqlc = errorcode ;
				rollback () ;
				prot ("Transaction rolled back, no more loop\n") ;
				errorcode = sqlc ;
				rolled_back = 1 ;
				return ( 0 ) ;
				break;
			case 4 :
				rollback () ;
				prot ("Transaction rolled back, loop again\n") ;
				sleep (1) ;
				return (1) ;
				break ;
			default:
				return -2 ;
		}
	}
	else
		return (0) ;
}
