/*!
  @file           ven48.c
  @author         JoergM
  @brief          RunTime: Miscellaneous String Functions
  @see            

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

#define MOD__ "ven48.c:"

#include "gen00.h"
#include "gen48.h"

#include "SAPDBCommon/SAPDB_string.h"

#define            DOLL      '$'
#define            KLA       '{'
#define            KLZ       '}'

/* exported functions */

#include "gen48.h"

/* imported functions */

extern  char                            * getenv ();

/*
 * --------------------------------------------------------------------
 */

char * sql48_strloc ( 
char                            * s1 ,
char                            * s2 )
{
/*
 * return address in s1 where s2 starts
 */

char                               *xs1;
char                               *xs2;

while ( *s1 )
    {
    if ( *s1 == *s2 )
	{
	xs1 = s1 + 1;
	xs2 = s2 + 1;
	while ( *xs2 && *xs2 == *xs1 )
	    {
	    xs1++;
	    xs2++;
	    }
	if ( ! *xs2 )
	    {
	    return ( s1 );
	    }
	}
    s1++;
    }

DBG1 (( MF__,"strloc: nothing found %s  \n", s2 ));
return (0);
}

/*-------------------------------------------------------*/

#ifdef  REL24
tsp00_Int2    sql48_strblcpy (
char                            * s1 ,
char                            * s2 ,
tsp00_Int2                        n )
{
/*
 * copy max n characters from s2 to s1 up to first space
 * or new_line, and end with NUL
 * Return 0 if OK otherwise -1
 */

while ( n-- > 0 && *s2 != '\0' )
    {
    if ( *s2 == ' ' ||	*s2 == '\n' )
	{
	*s1 = '\0';
        return (0);
        }
    else
	*s1++ = *s2++;
    }

*s1 = '\0';

return ( -1 );

}
#endif  /*REL24*/

/*-------------------------------------------------------*/

void    sql48_change_log ( 
char                                   * in_name ,
char                                   * out_name ,
tsp00_Bool                                * error )
/*---------------------------------------------------------------
 * transforms a logical filenameof the form :
 *
 *  log_name => name         -> @/_od_doll
 *           => / -> name    -> @/_od_doll
 *           => / -> envname -> @/_od_doll
 *           => env_name     -> @/_od_doll
 *
 *  @/_od_doll=> / -> name
 *            => / -> envname
 *            => env_name
 *
 *  env_name => ${ENVIRONMENTVARIABLE}
 *
 *  (@ means Iteration )
 * -------------------------------------------------------------
 *   e.g.: input : ${DBNAME}/datei
 *         output: /usr/vdn/DB/datei
 * --------------------------------------------------------------
 * If the logical name is not existant, this part of the logical
 * file description is ignored.
 *   e.g:  ${GIBTSNICHT}/test/datei -> blankname
 * ---------------------------------------------------------------
 * If the output name is longer than 65 characters, it gets cut.
 *---------------------------------------------------------------
 */
{

	tsp00_Int2                               ch_cnt ;
	tsp00_Int2                               env_len ;
	char                                   * nul_ptr ;
	char                                   * in_ptr ;
	char                                   * klz_ptr ;
	char                                   * env_ptr ;
	char                                   * out_ptr ;
	VF_FILENAMEC                           env_name ;
	VF_FILENAMEC                           tmp_name ;


	DBG1 (( MF__,"sql48_change_log: called \n" ));
	DBG1 (( MF__,"sql48_change_log: in_name                     = >%s<\n",
				 in_name ));


	/* ***** trailing blanks to nulls       ***** */
	/* ***** check the string's correctness ***** */
	/* ***** don't modify input parameter   ***** */

	SAPDB_strncpy_zero_terminated ( (char *)tmp_name, (const char *)in_name, sizeof(VF_FILENAMEC) );

	nul_ptr = (char *) strchr ( (const char *)tmp_name, '\0' );

	if ( ( nul_ptr == NULL ) || ( nul_ptr == (char *)tmp_name ) )
	{
		* error    = TRUE ;
		* out_name = '\0' ;
		return ;
	}

	while ( ( nul_ptr != (char *)tmp_name )    &&
		( * (-- nul_ptr ) == ' ' )     )
		* nul_ptr = '\0';

	if ( * tmp_name == '\0' )       /* all blank */
	{
		* error    = TRUE;
		* out_name = '\0';
		return;
	};


	/* ***** start main substitution loop ***** */

	* error  = FALSE;

	ch_cnt  = MAXVFNLENGTH;        /* count of remaining characters */
	in_ptr  = (char *)tmp_name;    /* input pointer */
	out_ptr = out_name;            /* output pointer */

	while ( ch_cnt > 0 && *in_ptr != '\0' )
	{
		if ( * in_ptr != DOLL          ||
		     * ( in_ptr + 1 ) != KLA   ||
		     ( klz_ptr = (char *) strchr ( in_ptr , KLZ )) == NULL )
		{
			/* ***** normal char, just copy it ***** */

			* out_ptr ++ = * in_ptr ++;
			ch_cnt--;
		}
		else
		{
			/* ***** "valid" logical part found ***** */

			in_ptr += 2;

			SAPDB_memcpy ( env_name , in_ptr , klz_ptr - in_ptr );

			env_name [ klz_ptr - in_ptr ] = '\0';

			DBG1 (( MF__,"sql48_change_log: getenv , env_name           = >%s<\n",
							  env_name ));


			/* ***** if possible expand env var ***** */

			env_ptr = (char *) getenv ( (const char *)env_name );

			if ( env_ptr != 0 )
			{

				/* ***** check remaining length ***** */

				if ( ( env_len = strlen( env_ptr )) > ch_cnt )
					env_len = ch_cnt;


				/* ***** build in expanded env var ***** */

				SAPDB_memcpy ( out_ptr , env_ptr , env_len );

				out_ptr += env_len ;
				ch_cnt  -= env_len ;
			}


			/* ***** skip variable in input ***** */

			in_ptr  =  klz_ptr + 1 ;
		}
	}

	*out_ptr = '\0';

	DBG1 (( MF__,"sql48_change_log: returns out_name            = %s\n",
				 out_name ));
}
