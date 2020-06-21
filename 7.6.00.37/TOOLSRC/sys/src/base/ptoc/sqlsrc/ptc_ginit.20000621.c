/*


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
#include        <stdio.h>

main ( argc , argv )
int     argc ;
char    ** argv ;
    {
    char    line   [400] ;
    char    oldvar [100] ;
    char    var    [100] ;
    int     size ;

    FILE    *fin ;

    oldvar [0] = 0 ;

    if ( ( fin = fopen ( argv [1] , "r" ) ) == NULL )
    {
	  perror ("cginit : open input file") ;
	  exit (1) ;
    }

    printf ("#include <stdio.h>\n") ;

    while ( fgets ( line , sizeof ( line ) , fin ) != NULL )
    {
	if ( sscanf ( line , "%s%d" , var , &size ) != 2 )
	    continue ;

	printf ("\tchar   *%s;\n" , var ) ;
	if ( strcmp ( oldvar , var ) == 0 )
	{
	    fprintf ( stderr , "cginit : duplicate global variable '%s'\n",
		      var ) ;
	    exit (2) ;
	}
	strcpy ( oldvar , var ) ;
    }

    printf ( "char *init_kglobals ( pstart )\n") ;
    printf ( "char  *pstart ;\n" ) ;
    printf ( "{\n" ) ;
    printf ( "\tchar        *p = pstart ;\n" ) ;
    printf ( "\n" ) ;
    printf ( "\tp  = p + 8 - ( (unsigned ptoc_long) p %% 8 );\n" )  ;
    printf ( "\n" ) ;

    rewind ( fin ) ;
    while ( fgets ( line , sizeof ( line ) , fin ) != NULL )
    {
	sscanf ( line , "%s%d" , var , &size ) ;
	printf ("\t%s = p;\n" , var ) ;
#ifdef	DEBUG
	printf ("\tfprintf (stderr,\"%-30s : 0x%%p\\n\", p ) ;\n", var );
#endif
	printf ("\tp  = p + %d ;\n" , size ) ;
	printf ("\tp  = p + 8 - ( (unsigned ptoc_long) p %% 8 );\n" ) ;
	printf ("\n" ) ;
    }
    printf ("\treturn (p);\n" ) ;
    printf ("}\n" ) ;
    return (0) ;
}
