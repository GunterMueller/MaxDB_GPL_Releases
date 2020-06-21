/*	vmakprot.c	

    ========== licence begin LGPL
    Copyright (C) 2002 SAP AG

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
    ========== licence end

*/

static	char * USAGE = "usage: vmakprot [-Dl] [-h number] version target \n\
\n\
    -Dl         debug level (l = 0 .. 9) \n\
    -h number   history number (0 = most recent protocol) \n\
    -x          list extended protocol \n\
";
#define ARGS(x)	(x == 2)

#include  "vmake.h"
#include  "vmakmisc.h"

/* exported functions */

global	int		main ();
global	char    * serrno ();

/* imported functions */


/* local functions */

static void filetest(char*,char*,char,struct stat*,char*);


/* exported data */

extern char * vmake_version;

/* imported data */

/* local data */
static int      hist_no = 0 ;
static char	    path [ FILENAMESIZE ];
static char	    * version ;

/* CTS 1112006 */
#define NORMAL_PROT		1
#define EXTENDED_PROT	2
#define ERROR_PROT		3

/*==========================================================================*/

global	int     main ( argc , argv )

int                     argc ;
char                    ** argv ;

{
	int                     rc ;
	int                     argn ;
	int			extended = FALSE ;
	int			lgt ;
	char			* ptr ;
	char			* target ;
	char			pname [ NAMESIZE ];
	char			pfile [ FILENAMESIZE ] = "";
	char			name  [ NAMESIZE ] = "";
	char			* layer = NULL;
	char			protname [ NAMESIZE ] = "";
	struct stat		pstatbuf ;
    short           knowntype = 0;
	char			tmppfile [ FILENAMESIZE ];

    extern int      dbgdbglvl;
	int				protocoltype = NORMAL_PROT;  /* CTS 1112006 */
	char            protext;
	
    dbgdbglvl++;
	rc = 0 ;
	for ( argn = 1 ; argn < argc ; argn ++ )
	{
	    ptr = argv[argn];
	    if ( *ptr != '-' ) break ;

	    for ( ptr ++ ; *ptr ; ptr ++ )
	    {
		switch ( * ptr )
		{
		case '?' :
			rc ++ ;
			break ;
		case 'h' :
			ptr ++ ;
			if ( ! *ptr ) ptr = argv[++argn];
			if ( ! ptr || (*ptr < '0') || (*ptr > '9') )
			    rc ++ ;
			else
			{
			    hist_no = atoi ( ptr );
			    while ( *ptr ) ptr ++ ;
			    ptr -- ; /* point to end of current argument */
			}
			break ;
		case 'x' :
			protocoltype = EXTENDED_PROT;
			break ;
		case 'e' :
			protocoltype = ERROR_PROT;
			break ;

		case 'D' :
			if ( ptr[1] >= '0' && ptr[1] <= '9' )
			{
                extern int dbgdbglvl;
			    dbgdbglvl = ptr[1] - '0' ;
			    ptr ++ ;
                dbgdbglvl++;
			    DBG0 (( "debug level %d \n", dbgdbglvl ))
			    break ;
			}
			/*FALLTHROUGH*/
		default :
			DBG0 (( "illegal option '%c' \n", *ptr ))
			rc ++ ;
			break ;
		}
	    }
	}
	if ( rc ||  ! ARGS (argc - argn) )
	{
	    DBG0 (( "%s\n", vmake_version + 4 ))
	    DBG0 (( "%s\n", USAGE ))
	    return ( 1 );
	}
	version = argv[argn++];
	target = argv[argn++];
    (void) strcpy( protname, target);
    (void) strcpy( pfile, target);

	DBG7 (( "vmakprot: MAXFNMLGT %d \n", MAXFNMLGT ))
	DBG7 (( "vmakprot: NAMESIZE  %d \n", NAMESIZE  ))
	DBG7 (( "vmakprot: FILENMSIZ %d \n", FILENAMESIZE ))

	ptr = getenv ( "VMAKE_PATH" );
	if ( ! ptr )
	{
	    DBG0 (( "VMAKE_PATH is not set \n" ))
	    return ( 2 );
	}
	
	/* initialize working directories (CTS 1105673) */
	rc = init_wrk ();
    if ( rc ) return ( rc );

	for ( lgt = 0 ; lgt < sizeof(path) ; lgt ++ )
	{
	    if ( * ptr == PATH_DELIMITER || * ptr == '\0' ) break ;
	    path [ lgt ] = * ptr ;
	    ptr ++ ;
	}
	if ( lgt >= sizeof(path) )
	{
	    DBG0 (( "path name too long in VMAKE_PATH: '%s' \n", ptr ))
	    return ( 2 );
	}
	path [ lgt ] = '\0' ;

	DBG4 (( "vmakprot: path '%s' \n", path ))
	DBG4 (( "vmakprot: hist_no %d \n", hist_no ))

    if ( is_oldstyled_source( target, (char **)NULL, NULL ) )
    {
        sprintf( protname, "%s" , target );
        knowntype = 1;
    }
    else if ( is_qualified_source( target, &layer, name ) > 0 ) /* PTS 1106891 */
    {
        sprintf( protname, "%s/%s" , layer, name );
        knowntype = 1;
    }
    else if ( is_valid_descname( target, &layer, name ) )
	{
        char * ext;

        if ( layer )
            sprintf( protname, "%s/%s" , layer, name );
        else
            sprintf( protname, "%s" , name );

        ext = get_extension( protname );
        /* check for known types */
        if ( !strcmp( ext, MAC_LST ) ||
             !strcmp( ext, PRJ_LST ) ||
             !strcmp( ext, LNK_LST ) ||
             !strcmp( ext, SHM_LST ) ||
             !strcmp( ext, DLL_LST ) ||
             !strcmp( ext, SHR_LST ) ||
             !strcmp( ext, REL_LST ) ||
             !strcmp( ext, LIB_LST ) )
            knowntype = 1;
        else
        {
        }
    }

    FREE_AND_SET_TO_NULL(layer);

	

	FILL ( &pstatbuf , 0 , sizeof(pstatbuf) );
	/* FILL ( &xstatbuf , 0 , sizeof(xstatbuf) ); */


	switch (protocoltype)
	{
		case NORMAL_PROT:
			protext='p';
			break;
		case EXTENDED_PROT:
			protext='x';
			break;
		case ERROR_PROT:
			protext='e';
			break;
	}

	if ( knowntype )
    {
	    (void) sprintf ( pname , "%.*s.%c%d", MAXFNMLGT-3 , protname ,protext, hist_no );
	    (void) sprintf ( pfile , DIR_PROT , path , version );
	    (void) strcat  ( pfile , pname );

        rc = stat ( pfile , &pstatbuf );
	    if ( rc < 0 )
	    {
		    if ( (geterrno() != ENOENT) && (geterrno() != ENOTDIR) )
		    {
		        DBG0 (( "cannot stat '%s': %s \n", pfile , serrno() ))
		    }
		    FILL ( &pstatbuf , 0 , sizeof(pstatbuf) );
		}
    }
	else
    {
        /* look for the youngest protocol */
        time_t savedate = 0;

        filetest( protname, "", protext, &pstatbuf, tmppfile );
        if ( savedate < pstatbuf.st_mtime )
        {
            savedate = pstatbuf.st_mtime;
            sprintf( pfile, tmppfile );
        }

        filetest( protname, "mac", protext, &pstatbuf, tmppfile );
        if ( savedate < pstatbuf.st_mtime )
        {
            savedate = pstatbuf.st_mtime;
            sprintf( pfile, tmppfile );
        }

        filetest( protname, "lnk", protext, &pstatbuf, tmppfile );
        if ( savedate < pstatbuf.st_mtime )
        {
            savedate = pstatbuf.st_mtime;
            sprintf( pfile, tmppfile );
        }

        filetest( protname, "shm", protext, &pstatbuf, tmppfile );
        if ( savedate < pstatbuf.st_mtime )
        {
            savedate = pstatbuf.st_mtime;
            sprintf( pfile, tmppfile );
        }

        filetest( protname, "dld", protext, &pstatbuf, tmppfile );
        if ( savedate < pstatbuf.st_mtime )
        {
            savedate = pstatbuf.st_mtime;
            sprintf( pfile, tmppfile );
        }

        filetest( protname, "shr", protext, &pstatbuf, tmppfile );
        if ( savedate < pstatbuf.st_mtime )
        {
            savedate = pstatbuf.st_mtime;
            sprintf( pfile, tmppfile );
        }

        filetest( protname, "rel", protext, &pstatbuf, tmppfile );
        if ( savedate < pstatbuf.st_mtime )
        {
            savedate = pstatbuf.st_mtime;
            sprintf( pfile, tmppfile );
        }

        filetest( protname, "lib", protext, &pstatbuf, tmppfile );
        if ( savedate < pstatbuf.st_mtime )
        {
            savedate = pstatbuf.st_mtime;
            sprintf( pfile, tmppfile );
        }
		if ( savedate == 0 ) FILL ( &pstatbuf , 0 , sizeof(pstatbuf) );
    }
	
	if ( pstatbuf.st_mode )
	{
	    (void) printf ( "%s\n", pfile );
	}
    else
	{
	    DBG0 (( "No protocol '%s'\n", pfile ))
	    return ( 1 );
	}

	return ( 0 );
}

/*==========================================================================*/

static void filetest
(
    char        * protname,
    char        * ext,
    char        pkind,
    struct stat * statbuf,
    char        * file
)
{
	char		tmppname [ NAMESIZE ];
    int         rc;

    if ( * ext )
        (void) sprintf ( tmppname , "%.*s.%s.%c%d", MAXFNMLGT-3 ,
                protname , ext, pkind, hist_no );
    else
        (void) sprintf ( tmppname , "%.*s.%c%d", MAXFNMLGT-3 ,
                protname , pkind, hist_no );
	(void) sprintf ( file , DIR_PROT , path , version );
	(void) strcat  ( file , tmppname );

    rc = stat ( file , statbuf );
    if ( rc < 0 )
    {
	    if ( (geterrno() != ENOENT) && (geterrno() != ENOTDIR) )
	    {
		    DBG0 (( "cannot stat '%s': %s \n", file , serrno() ))
	    }
	    FILL ( statbuf , 0 , sizeof(statbuf) );
    }
}


/**VARARGS1*/
/*PRINTFLIKE1*/
/*
global	void	dbgdbg
(
    char    * fmt,
    ...
)
{
	va_list			args ;

	va_start ( args, fmt );
	(void) vfprintf ( stderr , fmt , args );
	(void) fflush ( stderr );
	va_end ( args );
}
*/
/*==========================================================================*/

