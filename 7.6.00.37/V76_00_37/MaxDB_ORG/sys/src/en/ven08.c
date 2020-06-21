/*!
  @file           ven08.c
  @author         JoergM
  @brief          Client RunTime: sqlread/sqlwrite
  @see            


  Read/Write single lines from/to Stdin/out

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

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



\endif
*/

#define MOD__ "ven08.c:"
#include "gen00.h"

/*
 * ===========================================================================
 */

#undef MF__
#define MF__ MOD__"sqlread"
void    sqlread ( 
tsp00_C132                line ,
tsp00_Bool                 * ok )
{
	char            tmp_line [ 133 ] ;      /* length of line + 1 */
	int             len ;
	int             i ;
	char            * rc ;
	int             max_line_len = 132 ;
	int             max_tmp_line_len = sizeof ( tmp_line ) ;

	DBGIN;

	/* ***** clear in/out vars and local vars ***** */
	/* ***** initialize in/out vars           ***** */

	SAPDB_memset ( line ,     ' ' , max_line_len ) ;
	SAPDB_memset ( tmp_line , ' ' , max_tmp_line_len ) ;

	* ok = TRUE ;


	/* ***** read string from stdin ***** */

	rc = fgets ( tmp_line , max_tmp_line_len , stdin ) ;

	if ( rc == (char *) NULL )
	{
		* ok = FALSE ;
	}
	else
	{
		/* ***** transform local string to pascal string ***** */

		len = strlen ( tmp_line ) - 1 ;

		DBG1 (( MF__,"sqlread: len                     = %d\n" , len ))

		for ( i = 0 ; i < len ; i ++ )
			line [ i ] = tmp_line [ i ] ;


		/* ***** fill up pascal string with blanks ***** */

		for ( i = len ; i < max_line_len ; i ++ )
			line [ i ] = ' ' ;
	}

	DBG1 (( MF__,"sqlread: * ok                    = %d\n" , * ok ))
	DBG1 (( MF__,"sqlread: line                    = %.132s\n" , line ))

    DBGOUT;
}
 
/*
 * ===========================================================================
 */

#undef MF__
#define MF__ MOD__"sqlwrite"
void    sqlwrite ( 
tsp00_C132                line )
{
	char            tmp_line [ 133 ] ;      /* length of line + 1 */
	int             len ;
	int             i ;
	int             max_line_len = 132 ;
	int             max_tmp_line_len = sizeof ( tmp_line ) ;

	DBGIN;

	/* ***** clear local vars ***** */

	SAPDB_memset ( tmp_line , ' ' , max_tmp_line_len ) ;


	/* ***** cut ending blanks ***** */

	for ( i = max_line_len - 1 ; ( ( i >= 0 ) && ( line [ i ] == ' ' ) ) ;
								      i -- ) ;

	DBG1 (( MF__,"sqlwrite: i ( last sign in line ) = %d\n" , i ))


	/* ***** transform pascal string to local string ***** */

	len = i + 1 ;
	for ( i = 0 ; i < len ; i ++ )
		tmp_line [ i ] = line [ i ] ;

	tmp_line [ len ] = '\0' ;

	DBG1 (( MF__,"sqlwrite: line                    = %.132s\n" , line ))
	DBG1 (( MF__,"sqlwrite: tmp_line                = %s\n" , tmp_line ))


	/* ***** write string to stdout ***** */

	(void) puts ( tmp_line ) ;

	DBGOUT;
}
