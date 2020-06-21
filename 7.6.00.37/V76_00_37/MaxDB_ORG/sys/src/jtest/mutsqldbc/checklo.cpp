/*
 * @(#) checklo.cpp	2005-01-21



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

//comment: precompiler automatic cast, watch cpc.h in C:\sapdevelop\V76\develop\usr\sdk\7600\incl
//VARCHAR cll [MAXLONGRAW] ;
struct {SQLDBC_Length len; char arr [MAXLONGRAW];} cll;
int	cllen ;
SQLDBC_Length icllen;
int	locnt ;


int main (int argc, char **argv)
{
	int	kk1 = 0;
	int	lc = 0 ;
	cll.len = MAXLONGRAW ;

	check_params(argc, argv);
	sprintf(actfile, "%s.act", callfile);
	fout = stdout;
	print_params(argc, argv);

	env = new  SQLDBC_Environment (getRuntime());
	//Create a new connection object and open a session to the database. 
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

	do_connect(conn, DBNODE, DBNAME, id, pw, CP);

	stmt = conn->createStatement();	
	CE("createStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);
	pstmt = conn->createPreparedStatement();
	CE("createPreparedStatement", 1, conn->ERRORCODE, conn->ERRORTEXT);

	returncode = checkPreparedStatement(pstmt, "SELECT COUNT (*) INTO ? FROM superu.TAB1");
	CE("prepare", 0, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	SQLDBC_Length indi1=sizeof(locnt);
	returncode = pstmt->bindParameter (1, SQLDBC_HOSTTYPE_INT4, &locnt, &indi1, sizeof(locnt)); 
	if (returncode != SQLDBC_OK) CE("error with bindParameter in main (col 1)", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);
	returncode = pstmt->execute();
	CE("SELECT COUNT ...", 1, pstmt->ERRORCODE, pstmt->ERRORTEXT);

	printf ("checking %d rows\n" , locnt ) ;
	conn->releaseStatement(pstmt);
	pstmt = 0;

	returncode = stmt->execute("SELECT K1, CLLEN, CLB FROM superu.TAB1");
	CE("SELECT K1, CLLEN, CLB FROM superu.TAB1", 1, stmt->ERRORCODE, stmt->ERRORTEXT);

	rs = stmt->getResultSet();
	while ( rs->next() == SQLDBC_OK )
	{
		SQLDBC_Length l;
		returncode = rs->getObject(1, SQLDBC_HOSTTYPE_INT4, &kk1, &l, sizeof(kk1), true);
		if (returncode != SQLDBC_OK) CE("error with getObject in main (col 1)", 1, rs->ERRORCODE, rs->ERRORTEXT);
		returncode = rs->getObject(2, SQLDBC_HOSTTYPE_INT4, &cllen, &icllen, sizeof(cllen), true);
		if (returncode != SQLDBC_OK) CE("error with getObject in main (col 2)", 1, rs->ERRORCODE, rs->ERRORTEXT);
		returncode = rs->getObject(3, SQLDBC_HOSTTYPE_ASCII, &cll.arr, &cll.len, sizeof(cll.arr), true);
		if (returncode != SQLDBC_OK) CE("error with getObject in main (col 3)", 1, rs->ERRORCODE, rs->ERRORTEXT);

		lc++ ;

		if ( icllen == 4 )
		{
			if ( cll.len != cllen )
			{
				printf ( "Illegal length : KEY = %d\n" , kk1 ) ;
				printf ( "cll.len              = %d\n" , cll.len ) ;
				printf ( "cllen                = %d\n" , cllen ) ;
			}
			else if ( cllen > 2 && ( cll.arr [2] != cll.arr[cllen-1] ))
			{
				printf ( "Illegal start/endmark of long raw : k1 = %d, chr (%d) <> chr (%d)\n" , kk1,cll.arr [2] , cll.arr[cllen-1]) ;
			}
		}
/*		else if ( icllen == 0 )
		{
			printf ("icllen = 0\n") ;
			printf ( "    cll.len             = %d\n" , cll.len ) ;
			printf ( "    cllen               = %d\n" , cllen ) ;
		}
*/
	}
	printf ("%d rows checked successfully\n" , lc ) ;
	conn->releaseStatement(pstmt);
	pstmt = 0;

	finish();

	return 0;
}
/*------------------------------------------------------------------------*/
void pe (const char *s, int leave, int errorcode, const char *errortext)
{
	fprintf ( stdout, "%s :  ERR = %d\n%s" , s , errorcode, errortext ) ;
	if (leave == 1)
	{
		remove( actfile );
		exit (99);
	}
}