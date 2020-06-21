/*	mfscan.c		

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
 *	module frame tools pascal source scanner
 */


#include	"mf.h"

static int     skipwhtspc ( );   
static int     skipstring ( );   
static int     skipbrace ( );    
static int     skipparcomtdr ( );
static int     skipparenth ( );  
static int     skipcomtdr ( );   
static int     skiptdr ( );      
static int     skiptdrange ( );  
int	skipcomment ( );
int	nextalpha ( );
int	nextsymbol ( );
int	nextchar ( );

/*============================================================================*/


int	copyall ( )

{
	int		rc;


	DEBUG ( 3, "copyall started \n", 0 )

	do
	{
		rc = nextchar ( );
	}
	while ( rc == 0 && c != EOF );

	DEBUG ( 3, "copyall complete, return code = %d \n", rc )
	if ( rc != ERREOF )
		return ( rc );
	else	return ( 0 );
}


/*============================================================================*/


int	nextkeyword ( )

{
	int		rc, i;


	DEBUG ( 3, "nextkeyword started \n" , 0 )

	keywrdtyp = TYPOTH; 

	while ( keywrdtyp == TYPOTH )
	{
		rc = nextsymbol ( );
		if ( rc ) return ( rc );
		DEBUG ( 3, "nextkeyword: symbol '%s' found \n", symbol )

		for ( i=0; i < sizeof(keyword)-1; i++ )
		{
			if ( symbol[i] == 0 ) break;
			keyword[i] = toupper ( symbol[i] );
		}
		keyword[i] = 0;

		switch ( keyword[0] )
		{
		case 'C':
				if ( strcmp(keyword,"CONST") == 0 )
				{
					keywrdtyp = TYPCON;
                    keyword_record = 0;
                    break;
				}
				if ( (strcmp(keyword,"CASE") == 0) && !keyword_record )
                {
                    mfscan_nesting_level++;
                    keyword_record = 0;
                }
				break;
		case 'T':
				if ( strcmp(keyword,"TYPE") == 0 )
				{
					keywrdtyp = TYPTYP;
                    keyword_record = 0;
				}
				break;
		case 'V':
				if ( strcmp(keyword,"VAR") == 0 )
				{
					keywrdtyp = TYPVAR;
                    keyword_record = 0;
				}
				break;
		case 'F':
				if ( strcmp(keyword,"FUNCTION") == 0 )
				{
					keywrdtyp = TYPFUN;
                    keyword_record = 0;
				}
				break;
		case 'P':
				if ( strcmp(keyword,"PROCEDURE") == 0 )
				{
					keywrdtyp = TYPPRC;
                    keyword_record = 0;
					break;
				}
				if ( strcmp(keyword,"PROGRAM") == 0 )
				{
					keywrdtyp = TYPPRG;
                    keyword_record = 0;
					break;
				}
				break;
		case 'B':
				if ( strcmp(keyword,"BEGIN") == 0 )
				{
                    mfscan_nesting_level++;
                    keyword_record = 0;
					keywrdtyp = TYPBEG;
				}
				break;
        case 'E':
                if (trace_step_out)
                {
                    if ( (strcmp(keyword, "END") == 0) && --mfscan_nesting_level == 0 )
                    {
                       keywrdtyp = TYPSTEPOUT;
                       keyword_record = 0;
                    }
                }
                break;
        case 'R':
                if (trace_step_out)
                {
                    if ( strcmp(keyword,"RECORD") == 0 )
                    {
                        mfscan_nesting_level++;
                        keyword_record = 1;
                    }
                }
                break;
		}

		if ( keywrdtyp != TYPOTH )
		{
			DEBUG ( 3, "nextkeyword: keyword '%s' found, ", keyword)
			DEBUG ( 3, "type = %d \n", keywrdtyp )
		}
		else
		{
			DEBUG ( 3, "nextkeyword: no keyword '%s' \n", symbol )
			if ( outfp != NULL ) fputs ( symbol, outfp );
		}
	}

	DEBUG ( 3, "nextkeyword complete, found '%s' \n", keyword )
	return ( 0 );
}


/*============================================================================*/


int	nextsymbol ( )

{
	int		rc, i;


	DEBUG ( 4, "nextsymbol started \n", 0 )

	rc = nextalpha ( );
	if ( rc ) return ( rc );
	DEBUG ( 4, "nextsymbol: alpha '%c' found \n", c )

	for ( i=0; i < sizeof(symbol)-1; i++ )
	{
		if ( ! (isalnum(c)||c=='_') ) break; 
		symbol[i] = c;
		c = fgetc ( infp );
	}
	symbol[i] = 0;

	DEBUG ( 4, "nextsymbol complete, found '%s' \n", symbol )
	return ( 0 );
}


/*============================================================================*/


int     nextword ( )

{
	int             i, rc;


	DEBUG ( 4, "nextword started \n", 0 )

	for ( rc=ERROTH; rc == ERROTH; )
	{
		switch ( c )
		{
		case EOF:
				rc = ERREOF;
				break;
		case ' ':
		case '\t':
		case '\r':
		case '\n':
		case '\013':
		case '\014':
				skipwhtspc ( );
				break;
		case '\'':
				skipstring ( );
				break;
		case '{':
				skipbrace ( );
				break;
		case '(':
				c = getc ( infp );
				if ( c == '*' )
				{
					skipcomtdr ( );
				}
				else
				{
					ungetc ( c, infp );
					c = '(';
					rc = 0;
				}
				break;
		default:
				rc = 0;
				break;
		}
	}
	if ( rc )
	{
		if ( rc != ERREOF )
			DEBUG ( 4, "nextword: error %d \n", rc )
		return ( rc );
	}

	if ( isalnum(c)||c=='_' )
	{
		for ( i=0; i < WRDSIZ && (isalnum(c)||c=='_'); i++ )
		{
			if ( outfp != NULL ) fputc (c,outfp);
			word[i] = c;
			c = fgetc ( infp );
		}
		word[i] = '\0';
	}
	else
	{
		if ( outfp != NULL ) fputc (c,outfp);
		word[0] = c;
		word[1] = '\0';
		c = fgetc ( infp );
	}

	DEBUG ( 4, "nextword complete: '%s' \n", word )
	return ( 0 );
}


/*============================================================================*/


int	nextalpha ( )

{
	int		rc;


	DEBUG ( 5, "nextalpha started \n", 0 )

	while ( c != EOF  &&  !(isalpha(c)||c=='_')   )
	{
		rc = nextchar ( );
		if ( rc ) return ( rc );
	}

	if ( c == EOF )
	{
		DEBUG ( 5, "nextalpha: end of file \n", 0 )
		return ( ERREOF );
	}
	else
	{
		DEBUG ( 5, "nextalpha complete, found '%c' \n", c )
		return ( 0 );
	}
}


/*============================================================================*/


int	nextsemic ( )

{
	int		rc;


	DEBUG ( 5, "nextsemic started \n", 0 )

	while ( c != EOF  &&  c != ';' )
	{
		rc = nextchar ( );
		if ( rc ) return ( rc );
	}

	if ( c == EOF )
	{
		DEBUG ( 5, "nextsemic: end of file \n", 0 )
		return ( ERREOF );
	}
	else
	{
		DEBUG ( 5, "nextsemic complete, found '%c' \n", c )
		return ( 0 );
	}
}


/*============================================================================*/


int	nextchar ( )

{
	int		rc;


	DEBUG ( 6, "nextchar started \n", 0 )

	switch ( c )
	{
	case ' ':
	case '\t':
	case '\n':
			rc = skipwhtspc ( );
			break;
	case '\'':
			rc = skipstring ( );
			break;
	case '{':
			rc = skipbrace ( );
			break;
	case '(':
			rc = skipparcomtdr ( );
			break;
	default:
			rc = 0;
			if ( outfp != NULL ) fputc ( c, outfp );
			c = fgetc ( infp ); 
			break;
	}
	if ( rc ) return ( rc );

	DEBUG ( 6, "nextchar complete, ", 0 )
	if ( c == EOF )
		DEBUG ( 6, "found 'EOF' \n", 0 )
	else	DEBUG ( 6, "found '%c' \n", c )
	return ( 0 );
}

/*============================================================================*/


static	int	skipwhtspc ( )

{


	DEBUG ( 7, "skipwhtspc started \n", 0 )

	while ( c != EOF && ( c == ' ' || c == '\t' || c == '\n' ) )
	{
		if ( outfp != NULL ) fputc ( c, outfp );
		c = fgetc ( infp ); 
	}

	DEBUG ( 7, "skipwhtspc complete, ", 0 )
	if ( c == EOF )
		DEBUG ( 7, "found character 'EOF' \n", 0 )
	else	DEBUG ( 7, "found character '%c' \n", c )
	return ( 0 );
}


/*============================================================================*/


static	int	skipstring ( )

{


	DEBUG ( 7, "skipstring started \n", 0 )

	if ( outfp != NULL ) fputc ( c, outfp );
	c = fgetc ( infp ); 

	while ( c != EOF && c != '\'' )
	{
		if ( outfp != NULL ) fputc ( c, outfp );
		c = fgetc ( infp ); 
	}

	if ( c == EOF )
	{
		DEBUG ( 0, "skipstring: end of file \n", 0 )
		return ( ERREOF );
	}
	else
	{
		if ( outfp != NULL ) fputc ( c, outfp );
		c = fgetc ( infp ); 
		DEBUG ( 7, "skipstring complete, ", 0 )
		if ( c == EOF )
			DEBUG ( 7, "found character 'EOF' \n", 0 )
		else	DEBUG ( 7, "found character '%c' \n", c )
		return ( 0 );
	}
}


/*============================================================================*/


static	int	skipbrace ( )

{


	DEBUG ( 7, "skipbrace started \n", 0 )

	while ( c != EOF && c != '}' )
	{
		if ( outfp != NULL ) fputc ( c, outfp );
		c = fgetc ( infp ); 
	}

	if ( c == EOF )
	{
		DEBUG ( 0, "skipbrace: end of file \n", 0 )
		return ( ERREOF );
	}
	else
	{
		if ( outfp != NULL ) fputc ( c, outfp );
		c = fgetc ( infp ); 
		DEBUG ( 7, "skipbrace complete, ", 0 )
		if ( c == EOF )
			DEBUG ( 7, "found character 'EOF' \n", 0 )
		else	DEBUG ( 7, "found character '%c' \n", c )
		return ( 0 );
	}
}


/*============================================================================*/


static	int	skipparcomtdr ( )

{
	int		rc;


	DEBUG ( 7, "skipparcomtdr started \n", 0 )

	if ( outfp != NULL ) fputc ( c, outfp );
	c = fgetc ( infp );
	if ( c == EOF )
	{
		DEBUG ( 0, "skipparcomtdr: end of file \n", 0 )
		return ( ERREOF );
	}

	if ( c == '*' )
	{
		rc = skipcomtdr ( );
		if ( rc ) return ( rc );
	}
	else
	{
		rc = skipparenth ( );
		if ( rc ) return ( rc );
	}

	DEBUG ( 7, "skipparcomtdr complete, ", 0 )
	if ( c == EOF )
		DEBUG ( 7, "found character 'EOF' \n", 0 )
	else	DEBUG ( 7, "found character '%c' \n", c )
	return ( 0 );
}


/*============================================================================*/


static	int	skipparenth ( )

{
	int		rc;


	DEBUG ( 8, "skipparenth started \n", 0 )

	while ( c != EOF && c != ')' )
	{
		rc = nextchar ( );
		if ( rc ) return ( rc );
	}

	if ( c == EOF )
	{
		DEBUG ( 0, "skipparenth: end of file \n", 0 )
		return ( ERREOF );
	}
	else
	{
		if ( outfp != NULL ) fputc ( c, outfp );
		c = fgetc ( infp ); 
		DEBUG ( 8, "skipparenth complete, ", c )
		if ( c == EOF )
			DEBUG ( 8, "found character 'EOF' \n", 0 )
		else	DEBUG ( 8, "found character '%c' \n", c )
		return ( 0 );
	}
}


/*============================================================================*/


static	int	skipcomtdr ( )

{
	int		rc;


	DEBUG ( 8, "skipcomtdr started \n", 0 )

	if ( outfp != NULL ) fputc ( c, outfp );
	c = fgetc ( infp ); 
	if ( c == EOF )
	{
		DEBUG ( 0, "skipcomtdr: end of file \n", 0 )
		return ( ERREOF );
	}

	if ( tdskipflag  &&  toupper(c) == 'T' )
	{
		DEBUG ( 8, "skipcomtdr: possibly tdrange \n", 0 )
		rc = skiptdr ( );
		if ( rc ) return ( rc );
	}
	else
	{
		DEBUG ( 8, "skipcomtdr: simple comment \n", 0 )
		rc = skipcomment ( );
		if ( rc ) return ( rc );
	}

	DEBUG ( 8, "skipcomtdr complete, ", 0 )
	if ( c == EOF )
		DEBUG ( 8, "found character 'EOF' \n", 0 )
	else	DEBUG ( 8, "found character '%c' \n", c )
	return ( 0 );
}


/*============================================================================*/


	int	skipcomment ( )

{
	int		lastc;


	DEBUG ( 8, "skipcomment started \n", 0 )

	do
	{
		lastc = c;
		if ( outfp != NULL ) fputc ( c, outfp );
		c = fgetc ( infp ); 
	}
	while ( c != EOF && !( lastc == '*' && c == ')' )  );

	if ( c == EOF )
	{
		DEBUG ( 0, "skipcomment: end of file \n", 0 )
		return ( ERREOF );
	}
	else
	{
		if ( outfp != NULL ) fputc ( c, outfp );
		c = fgetc ( infp ); 
		DEBUG ( 8, "skipcomment complete, ", c )
		if ( c == EOF )
			DEBUG ( 8, "found character 'EOF' \n", 0 )
		else	DEBUG ( 8, "found character '%c' \n", c )
		return ( 0 );
	}
}


/*============================================================================*/


static	int	skiptdr ( )

{
	int		type, rc, i;


	DEBUG ( 8, "skiptdr started \n", 0 )

	for ( i=0; i < sizeof(word)-1; i++ )
	{
		if ( c == EOF || !(isalpha(c)) ) break;
		word[i] = toupper(c);
		c = fgetc ( infp );
	}
	word[i] = 0;
	DEBUG ( 8, "skiptdr: found word '%s' \n", word )

	if ( c == EOF )
	{
		DEBUG ( 0, "skiptdr: end of file \n", 0 )
		return ( ERREOF );
	}

	type = TYPOTH;
	if ( strcmp(word,"TDBEGIN") == 0 ) type = TYPTDB;
	if ( strcmp(word,"TDEND")   == 0 ) type = TYPTDE;

	switch ( type )
	{
	case TYPTDB:
			DEBUG ( 9, "skiptdr: (*TDBEGIN found, ", 0 )
			DEBUG ( 9, "tdskipflag = %d \n", tdskipflag )
			if ( tdskipflag == 1 )
			{
				tdskipflag = 2;
				DEBUG ( 9, "skiptdr: tdskipflag ", 0 )
				DEBUG ( 9, "= %d \n", tdskipflag )
				rc = skiptdrange ( );
				if ( rc ) return ( rc );
				DEBUG ( 9, "skiptdr: tdskipflag ", 0 )
				DEBUG ( 9, "= %d \n", tdskipflag )
				if ( outfp != NULL )
					fputs ( "TDRANGE SKIPPED", outfp );
				rc = skipcomment ( );
				if ( rc ) return ( rc );
			}
			else
			{
				DEBUG ( 0, "(*TDBEGIN without (*TDEND \n", 0 )
				return ( ERRTDB );
			}
			break;
	case TYPTDE:
			DEBUG ( 9, "skiptdr: (*TDEND found, ", 0 )
			DEBUG ( 9, "tdskipflag = %d \n", tdskipflag )
			if ( tdskipflag == 2 )
			{
				tdskipflag = 1;
				DEBUG ( 9, "skiptdr: tdskipflag ", 0 )
				DEBUG ( 9, "= %d \n", tdskipflag )
			}
			else
			{
				DEBUG ( 0, "(*TDEND without (*TDBEGIN \n", 0 )
				return ( ERRTDE );
			}
			break;
	case TYPOTH:
			DEBUG ( 9, "skiptdr: no tdrange: (*%s \n", word )
			if ( outfp != NULL ) fputs ( word, outfp );
			rc = skipcomment ( );
			if ( rc ) return ( rc );
			break;
	}

	DEBUG ( 8, "skiptdr complete: '%c' \n", c )
	return ( 0 );
}


/*============================================================================*/


static	int	skiptdrange ( )

{
	int		rc;



	DEBUG ( 9, "skiptdrange started \n", 0 )

	tdsavfp = outfp;
	outfp = NULL;
	DEBUG ( 9, "skiptdrange: output suppressed \n", 0 )

	while ( c != EOF && tdskipflag == 2 )
	{
		rc = nextchar ( );
		if ( rc ) return ( rc );
	}

	if ( c == EOF )
	{
		DEBUG ( 0, "skiptdrange: end of file \n", 0 )
		return ( ERREOF );
	}
	else
	{
		DEBUG ( 9, "skiptdrange: output resumed \n", 0 )
		outfp = tdsavfp;

		DEBUG ( 9, "skiptdrange complete: '%c' \n", c )
		return ( 0 );
	}
}


