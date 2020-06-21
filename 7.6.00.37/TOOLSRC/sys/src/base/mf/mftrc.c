/*      mftrc.c         

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

/*
 *      module frame trace insertion
 *
 *      add call to 'testproc' with module and procedure name of 'strlgt'
 */

#define USAGE "usage: %s [-Dl] [-t] infile outfile testproc module strlgt \n"
#define ARGS    == 5


#include	"mf.h"

static  int     trc_section ( );
static  int     nextbody ( );

static  char            testproc[SYMSIZ];
static  char            module[SYMSIZ];
static  int             stringlgt;


/*============================================================================*/


int	main ( argc, argv )

int		argc;
char		**argv;

{
	int		argn, rc, i, j;
	char		*p, modnam[SYMSIZ];


	DEBUGINI 
	tdskipflag = 1;

	for ( argn=1; argn < argc; argn++ )
	{
		if ( argv[argn][0] != '-' ) break;
		rc = mfoption ( argv[argn] );
		if ( rc ) return ( rc );
	}

	if ( ! (argc-argn ARGS) )
	{
		DEBUG ( 0, USAGE, argv[0] )
		return ( 1 );
	}

	infp = fopen ( argv[argn], "r" );
	if ( infp == NULL )
	{
		DEBUG ( 0, "input file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "input file '%s' opened \n", argv[argn] )
	argn++;

	outfp = fopen ( argv[argn], "w" );
	if ( outfp == NULL )
	{
		DEBUG ( 0, "output file open error %d \n", errno )
		return ( errno );
	}
	DEBUG ( 1, "output file '%s' opened \n", argv[argn] )
	argn++;

	strncpy ( testproc, argv[argn], sizeof(testproc) );
	DEBUG ( 1, "trace procedure is '%s' \n", testproc )
	argn++;

	for ( i=0; i < sizeof(modnam); i++ )
	{
		modnam[i] = toupper(argv[argn][i]);
		if ( modnam[i] == 0 ) break;
	}
#ifdef WIN32
	{
		char *ptmp;
		while( ptmp = strchr(modnam, '\\')) { *ptmp = '/';}
	}
#endif
	p = strrchr ( modnam, '/' );
	if ( p == NULL ) p = modnam; else p++;
	if ( !isdigit(p[2]) ) p++;
	if ( ! (isalpha(p[0]) && isalpha(p[1]) &&
		isdigit(p[2]) && isdigit(p[3])	)  )
	{
		DEBUG ( 0, "illegal module name '%s' \n", p );
		return ( 1 );
	}
	(void) sprintf ( module , "%-6.5s" , p );
	DEBUG ( 1, "module is '%s' \n", module )
	argn++;

	(void) sscanf ( argv[argn], "%d", &stringlgt );
	if ( stringlgt < 4 || stringlgt > WRDSIZ )
	{
		DEBUG ( 0, "illegal string length %d \n", stringlgt );
		return ( 1 );
	}
	DEBUG ( 1, "string length is %d \n", stringlgt );
	argn++;

	if ( tdskipflag )
	{
		c = fgetc ( infp );
		rc = copyall ( );
		DEBUG ( 1, "copy section complete, return code = %d \n", rc )
	}
	else
	{
		rc = trc_section ( );
		DEBUG ( 1, "trc_section complete, return code = %d \n", rc )
	}

	fclose ( infp );
	DEBUG ( 1, "input file closed \n", 0 )
	fclose ( outfp );
	DEBUG ( 1, "output file closed \n", 0 )

	return ( rc );
}


/*============================================================================*/


static  int     trc_section ( )

{
	int             rc, i;
	char            tststr[WRDSIZ];
    int             trc;
    /*
    char            * except_list;
    char            * dummy;
    */


	DEBUG ( 2, "trc_section started \n", 0 )

	c = fgetc(infp);
    /*
    except_list = getenv("MFTRC_EXCEPT");
    DEBUG ( 2, "exception list: %s \n", except_list )
    */
      
	while (  ( rc = nextkeyword ( ) )  ==  0  )
	{
		DEBUG ( 2, "trc_section: keyword found: %s, ", keyword )
		DEBUG ( 2, "keyword type: %d \n", keywrdtyp )
        trc = TRUE;

		switch ( keywrdtyp )
		{
		case TYPFUN:
		case TYPPRC:
				fputs ( keyword, outfp );
				rc = nextsymbol ( );
				if ( rc ) return ( rc );
				strcpy ( tststr, ">" ); 
                strcpy ( tststr, module );
                strcpy ( lastproc, symbol ); 
				strncat ( tststr, symbol, sizeof(tststr)-
							strlen(tststr) );
				fputs ( symbol, outfp );
                /*if ( except_list )
                {
                    char * except_proc;
                    
                    dummy = strdup ( except_list );
                    except_proc = strtok ( dummy, ", " );
                    while ( except_proc )
                    {
                        if ( !strcmp ( except_proc, symbol ) ) 
                        {
                            trc = FALSE;
                            break;
                        }
                        except_proc = strtok ( NULL, ", " );
                    }
                    free ( dummy );
                }
				*/
                rc = nextbody ( );
				if ( rc ) return ( rc );
				fputs ( keyword, outfp );
                if ( trc )
    				fprintf ( outfp, "\n%s('%-*.*s');", testproc,
	    					stringlgt, stringlgt, tststr);
				break;
        case TYPSTEPOUT:
                if (trace_step_out)
                {
                    strcpy ( tststr, "<" );
                    strncat ( tststr, lastproc, sizeof(tststr)-
                                strlen(tststr) );
                    fprintf ( outfp, ";%s('%-*.*s');\n", testproc,
                            stringlgt, stringlgt, tststr);
                }
				fputs ( keyword, outfp );
                break;
		default:
				fputs ( keyword, outfp );
				break;
		}
	}

	if ( rc != ERREOF )
	{
		DEBUG ( 2, "trc_section: error code %d \n", rc )
		return ( rc );
	}
	else
	{
		DEBUG ( 2, "trc_section complete \n", 0 )
		return ( 0 );
	}
}


/*============================================================================*/


static  int     nextbody ( )

{
	int             rc, i;
	char            prcnam[SYMSIZ];


	DEBUG ( 2, "nextbody started \n", 0 )

	while (  ( rc = nextkeyword ( ) )  ==  0  )
	{
		DEBUG ( 2, "nextbody: keyword found: %s, ", keyword )
		DEBUG ( 2, "keyword type: %d \n", keywrdtyp )

		if ( keywrdtyp == TYPBEG ) break;

		fputs ( keyword, outfp );
	}

	if ( rc != 0  &&  rc != ERREOF )
	{
		DEBUG ( 2, "nextbody: error code %d \n", rc )
		return ( rc );
	}
	else
	{
		DEBUG ( 2, "nextbody complete \n", 0 )
		return ( 0 );
	}
}


