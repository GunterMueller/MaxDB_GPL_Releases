/*      stkset_prep.c     

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

	Create calls to 'stkset_run' behind the build up of the stack frame
	of each subroutine.

	stkset_run (the runtime procedure) fills the stack of the calling
	routine from the location above the saved instruction pointer
	up to the position beyond the last saved register with a
	test pattern. Additionally, the volatile registers are loaded
	with the test pattern, as well as the saved nonvolatile registers.

	stkset_run is given a parameter which describes the saved nonvolatile
	registers as a bit mask which is the bitwise or of the following
	values:

		1       %ebx
		2       %esi
		4       %edi

	A stack frame is built up as follows:

			<%eip>          / 'call' instruction pushes
					/ instruction pointer
		pushl   %ebp            / save base pointer
		movl    %esp,%ebp       / base pointer now points to current
					/ current stack entry (which is
					/ old base pointer)
		subl    $<size>,%esp    / allocate stack space for local
					/ variables of size <size>
	/ or:
					/ if <size> is 4,
		pushl   %eax            / will be used in place of 'subl'
					/ because of better performance
	/ optional:
					/ If nonvolatile registers are used
					/ in a routine, they are to be
					/ saved:
		pushl   %ebx            / and/or:
		pushl   %esi            / and/or:
		pushl   %edi            /

	The remaining registers are free to use by any routine
	without saving them (so called volatile registers):

		%eax
		%ecx
		%edx

 */

char  * USAGE = "usage: stkset_prep [-s] infile outfile \n";
#define ARGS(x) (x == 2)

#include        "stdio.h"
#include        "string.h"

void                    err ();

int			opt_silent ;
int                     calls ;
int                     reg_mask ;
FILE                    * infp ;
FILE                    * outfp ;

/*==========================================================================*/

int     main ( argc , argv )

int                     argc ;
char                    ** argv ;

{
	int                     argn , rc ;
	char                    * p ;


	rc = 0 ;
	for ( argn = 1 ; argn < argc ; argn ++ )
	{
	    p = argv [ argn ];
	    if ( * p != '-' ) break ;
	    p ++ ;
	    switch ( * p )
	    {
	    case 's' :
		opt_silent = 1 ;
		break ;
	    default :
		(void) fprintf ( stderr , "illegal option '%c' \n", * p );
		rc ++ ;
	    }
	}
	if ( rc || ! ARGS ( argc - argn ) )
	{
		(void) write ( 2 , USAGE , (unsigned) strlen ( USAGE ) );
		return ( 1 );
	}

	p = argv [ argn ];
	infp = fopen ( p , "r" );
	if ( ! infp )
	{
		err ( "error opening input file '%s' \n", p );
		return ( 2 );
	}
	argn ++ ;

	p = argv [ argn ];
	outfp = fopen ( p , "w" );
	if ( ! outfp )
	{
		err ( "error opening output file '%s' \n", p );
		(void) fclose ( infp );
		return ( 2 );
	}
	argn ++ ;

	rc = insert ();

	(void) fclose ( infp );
	(void) fclose ( outfp );

	if ( ! opt_silent )
	    (void) printf ( "inserted %d calls to 'stkset_run' \n", calls );

	return ( rc );
}

/*==========================================================================*/

int     insert ( )

{
	int                     matchlevel , match , rc ;
	char                    line [ 256 ];


	matchlevel = 0 ;

	while ( fgets ( line , sizeof(line) , infp ) )
	{
		switch ( matchlevel )
		{
		case 0 :
			match = match_pushl ( line , "%ebp" );
			break ;

		case 1 :
			match = match_movl_esp_ebp ( line );
			if ( ! match )
			{
				err ( "Warning: incomplete call link! \n" );
			}
			break ;

		case 2 :
			match = match_subl_esp ( line );
			if ( ! match ) match = match_pushl ( line , "%eax" );
			if ( match ) break ;
			matchlevel ++ ; /* disallow second subl %esp */

		case 3 :
			match = match_pushl ( line , "%ebx" );
			if ( match ) reg_mask |= 1 ;
			if ( ! match )
			{
				match = match_pushl ( line , "%esi" );
				if ( match ) reg_mask |= 2 ;
			}
			if ( ! match )
			{
				match = match_pushl ( line , "%edi" );
				if ( match ) reg_mask |= 4 ;
			}
			if ( match )
			{
				rc = fputs ( line , outfp );
				if ( rc == EOF )
				{
				    err ( "error writing output file \n" );
				    return ( 2 );
				}
				continue ;
			}
			rc = fprintf ( outfp , "\tpushl\t$%d\n", reg_mask );
			if ( rc <= 0 )
			{
				err ( "error writing output file \n" );
				return ( 2 );
			}
			rc = fputs ( "\tcall\tstkset_run\n" , outfp );
			if ( rc == EOF )
			{
				err ( "error writing output file \n" );
				return ( 2 );
			}
			rc = fputs ( "\taddl\t$4,%esp\n" , outfp );
			if ( rc == EOF )
			{
				err ( "error writing output file \n" );
				return ( 2 );
			}
			rc = fputs ( line , outfp );
			if ( rc == EOF )
			{
				err ( "error writing output file \n" );
				return ( 2 );
			}
			calls ++ ;
			reg_mask = 0 ;
			matchlevel = 0 ;
			continue ;

		default :
			err ( "program error, matchlevel %d \n", matchlevel );
			return ( 2 );
		}

		if ( match )
			matchlevel ++ ;
		else    matchlevel = 0 ;


		rc = fputs ( line , outfp );
		if ( rc == EOF )
		{
			err ( "error writing output file \n" );
			return ( 2 );
		}
	}

	return ( 0 );
}

/*==========================================================================*/

int     match_pushl ( line , reg )

char                    * line ;
char                    * reg ;

{
	char                    * p , * q ;


	for ( p = line ; * p == ' ' || * p == '\t' ; p ++ );

	if ( * ( p ++ ) != 'p' ) return ( 0 );
	if ( * ( p ++ ) != 'u' ) return ( 0 );
	if ( * ( p ++ ) != 's' ) return ( 0 );
	if ( * ( p ++ ) != 'h' ) return ( 0 );
	if ( * ( p ++ ) != 'l' ) return ( 0 );

	if ( * p != ' '  &&
	     * p != '\t'    ) return ( 0 );

	for ( ; * p == ' ' || * p == '\t' ; p ++ );

	for ( q = reg ; * q ; q ++ )
	{
		if ( * ( p ++ ) != * q ) return ( 0 );
	}

	if ( * p != ' '  &&
	     * p != '\t' &&
	     * p != '\n'    ) return ( 0 );

	return ( 1 );
}

/*==========================================================================*/

int     match_movl_esp_ebp ( line )

char                    * line ;

{
	char                    * p ;


	for ( p = line ; * p == ' ' || * p == '\t' ; p ++ );

	if ( * ( p ++ ) != 'm' ) return ( 0 );
	if ( * ( p ++ ) != 'o' ) return ( 0 );
	if ( * ( p ++ ) != 'v' ) return ( 0 );
	if ( * ( p ++ ) != 'l' ) return ( 0 );

	if ( * p != ' '  &&
	     * p != '\t'    ) return ( 0 );

	for ( ; * p == ' ' || * p == '\t' ; p ++ );

	if ( * ( p ++ ) != '%' ) return ( 0 );
	if ( * ( p ++ ) != 'e' ) return ( 0 );
	if ( * ( p ++ ) != 's' ) return ( 0 );
	if ( * ( p ++ ) != 'p' ) return ( 0 );

	if ( * p != ' '  &&
	     * p != '\t' &&
	     * p != ','     ) return ( 0 );

	for ( ; * p == ' ' || * p == '\t' ; p ++ );

	if ( * ( p ++ ) != ',' ) return ( 0 );

	for ( ; * p == ' ' || * p == '\t' ; p ++ );

	if ( * ( p ++ ) != '%' ) return ( 0 );
	if ( * ( p ++ ) != 'e' ) return ( 0 );
	if ( * ( p ++ ) != 'b' ) return ( 0 );
	if ( * ( p ++ ) != 'p' ) return ( 0 );

	if ( * p != ' '  &&
	     * p != '\t' &&
	     * p != '\n'    ) return ( 0 );

	return ( 1 );
}

/*==========================================================================*/

int     match_subl_esp ( line )

char                    * line ;

{
	char                    * p ;


	for ( p = line ; * p == ' ' || * p == '\t' ; p ++ );

	if ( * ( p ++ ) != 's' ) return ( 0 );
	if ( * ( p ++ ) != 'u' ) return ( 0 );
	if ( * ( p ++ ) != 'b' ) return ( 0 );
	if ( * ( p ++ ) != 'l' ) return ( 0 );

	if ( * p != ' '  &&
	     * p != '\t'    ) return ( 0 );

	for ( ; * p != ',' && * p != '\0' ; p ++ );

	if ( * ( p ++ ) != ',' ) return ( 0 );

	for ( ; * p == ' ' || * p == '\t' ; p ++ );

	if ( * ( p ++ ) != '%' ) return ( 0 );
	if ( * ( p ++ ) != 'e' ) return ( 0 );
	if ( * ( p ++ ) != 's' ) return ( 0 );
	if ( * ( p ++ ) != 'p' ) return ( 0 );

	if ( * p != ' '  &&
	     * p != '\t' &&
	     * p != '\n'    ) return ( 0 );

	return ( 1 );
}

/*==========================================================================*/
 
 
/*VARARGS1*/
void    err ( text , val1 , val2 , val3 , val4 )
 
char                    * text ;
int                     val1 ;
int                     val2 ;
int                     val3 ;
int                     val4 ;
 
{
	char                    line [ 256 ] ;
 
 
	(void) sprintf ( line , text , val1 , val2 , val3 , val4 );
 
	(void) write ( 2 , line , (unsigned) strlen ( line ) );
}
 
 
