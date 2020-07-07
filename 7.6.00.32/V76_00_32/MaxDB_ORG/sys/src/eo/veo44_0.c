/*!
  @file           veo44_0.c
  @author         DanielD
  @special area   
  @brief          RTE
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



#include <ctype.h>
#include <stdarg.h>

#include  "gsp00.h" 
#include  "vsp009c.h" 
#include  "heo05.h" 

#if defined (_WIN32)    /*&if $OSSPEC = WIN32*/ 
#include  "gos00.h"     /* nocheck */
#else                   /*&else*/
#include  "gen00.h"     /* nocheck */
#endif                  /*&endif*/


/**\
--------------------------------------- MF__ MOD__ (xerror)
\**/

    #undef MF__
    #define MF__ MOD__"eo44initError"
void
eo44initError (
    tsp9_rte_xerror* xerror)
{
    xerror->xe_result = csp9_xrte_ok;
    xerror->xe_native_error = UNDEF;
    xerror->xe_text [0] = '\0';
}

    #undef MF__
    #define MF__ MOD__"eo44anyError"
void
eo44anyError (
    tsp9_rte_xerror* xerror,
    tsp9_cstr msg)
{
    xerror->xe_result = csp9_xrte_notok;
    if (msg == NULL) {
        msg = "Unspecified error";
    }
    strncpy (xerror->xe_text, msg, sizeof(xerror->xe_text) );
    xerror->xe_text[sizeof(xerror->xe_text) - 1];
}

    #undef MF__
    #define MF__ MOD__"eo44sysError"
void
eo44sysError (
    tsp9_rte_xerror* xerror,
    int              rc )
{
    xerror->xe_result = csp9_xrte_notok;
    sprintf (xerror->xe_text, "OS Error: [%d]", rc);
}

    #undef MF__
    #define MF__ MOD__"eo44eoshowError"
void
eo44eoshowError (
    tsp9_rte_xerror* xerror)
{
    xerror->xe_result = csp9_xrte_atend;
    strcpy (xerror->xe_text, "End of show data");
}

    #undef MF__
    #define MF__ MOD__"eo44printfError"
void
eo44printfError (
    tsp9_rte_xerror* xerror,
    tsp00_Int4        msg_no ,
    signed char     msg_type ,
    char           *msg_label ,
    char           *msg_format ,
    ...)    
{
  va_list          args;
  static char     *msg_type_txt[] =
  { "INTERNAL ERROR", "ERROR", "WARNING", "INFO" };
  
  USEVAR_LINT (msg_label);
  if (xerror != NULL) 
    {
      if ((msg_type == 0) || (msg_type == 1)) 
	{
	  /* internal error or error */
	  xerror->xe_result = csp9_xrte_notok; 
	}
      else 
	{
	  /* warning or info */
	  xerror->xe_result = csp9_xrte_ok;
	}
      xerror->xe_native_error = msg_no;
      va_start (args, msg_format);
      vsprintf (xerror->xe_text, msg_format, args);
      va_end (args);
    }
  else 
    {
      if ( msg_type <= ( sizeof ( msg_type_txt ) / sizeof ( char *) ) )
	{
	  printf ( "%s ", msg_type_txt[msg_type] );
	}
      printf ( "%d: ", msg_no );
    
      va_start ( args, msg_format ) ;
      vprintf ( msg_format, args ) ;
      va_end ( args ) ;
    
      printf ( "\n" );
      fflush ( stdout );
    }
}	

    #undef MF__
    #define MF__ MOD__"eo44printfSimpleError"
void
eo44printfSimpleError (
    tsp9_rte_xerror* xerror,
    tsp00_Int4        msg_no ,
    char           *msg_format ,
    ...)    
{
    va_list          args;

    if (xerror != NULL) {
        xerror->xe_result = msg_no;
        va_start (args, msg_format);
        vsprintf (xerror->xe_text, msg_format, args);
        va_end (args);
    }
    else {
        printf ( "%d: ", msg_no );

        va_start ( args, msg_format ) ;
        vprintf ( msg_format, args ) ;
        va_end ( args ) ;

        printf ( "\n" );
        fflush ( stdout );
    }
}

/**\
--------------------------------------- MF__ MOD__ (parsing)
\**/

    #undef MF__
    #define MF__ MOD__"eo44skipSpace"
tsp9_cstr
eo44skipSpace (
    tsp9_cstr str)
{
    while (isspace (str[0])) {
        ++str;
    }
    return str;
}

    #undef MF__
    #define MF__ MOD__"eo44skipIdentifier"
tsp9_cstr
eo44skipIdentifier (
    tsp9_cstr str)
{
    while (isalpha (str[0])) {
        ++str;
    }
    return str;
}

    #undef MF__
    #define MF__ MOD__"eo44parseInt"
int
eo44parseInt (
    tsp9_cstr str)
{
    str = eo44skipSpace (str);
    return atoi (str);
}

    #undef MF__
    #define MF__ MOD__"eo44strcont"
int     
eo44strcont (
    tsp9_cstr string1, 
    tsp9_cstr string2, 
    int minlgt)
{
    int i;
    int lgt;

    /*
     *  Does string1 start with string2 of at least minlgt characters ?
     */
    if ( (int) strlen(string1) < (lgt = (int)strlen(string2)) ) 
        return ( 0 );
    if ( lgt < minlgt ) 
        return ( 0 );
    if ( minlgt == 0  ) 
        return ( 1 );

    for ( i = 0 ; i < lgt ; i ++ )
    {
        if ( toupper (string1 [i]) != toupper (string2 [i]) ) 
            break ;
    }
    if ( i < lgt ) 
        return ( 0 );
    return ( 1 );
}

/**\
--------------------------------------- MF__ MOD__ (formatting)
\**/

    #undef MF__
    #define MF__ MOD__"eo44printString"
void
eo44printString (
    char** outbuf,
    int* bufSize,
    tsp9_cstr str,
    tsp9_bool quoted)
{
    int len;
    tsp9_cstr formatString;
    int quoteLen;
    
    len = (int)strlen (str);
    if (len < (*bufSize + 4)) { /* add quotes, stop and zero */
        if (quoted) {
            formatString = "\"%s\",";
            quoteLen = 2;
        }
        else {
            formatString = "%s,";
            quoteLen = 0;
        }
        sprintf (*outbuf, formatString, str);
        *outbuf += len + quoteLen + 1;
        *bufSize -= len + quoteLen + 1;
    }
}

    #undef MF__
    #define MF__ MOD__"eo44printLong"
void
eo44printLong (
    char** outbuf,
    int* bufSize,
    long val)
{
    char buf[20];
    
    sprintf (buf, "%d", val);
    eo44printString (outbuf, bufSize, buf, FALSE);
}

    #undef MF__
    #define MF__ MOD__"eo44printChar"
void
eo44printChar (
    char** outbuf,
    int* bufSize,
    int ch)
{
    if (*bufSize >= 2) {
        (*outbuf)[0] = (char) ch;
        (*outbuf)[1] = '\0';
    }
}

