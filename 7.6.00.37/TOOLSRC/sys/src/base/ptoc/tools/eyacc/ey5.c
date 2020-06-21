/* @(#)tools/eyacc/ey5.c
 */
/*
    ========== licence begin  other

 * Copyright (c) 1979 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.

    ========== licence end
 */

#ifndef lint
static char sccsid[] = "@(#)ey5.c	5.1 (Berkeley) 4/29/85";
#endif not lint

/* fake portable I/O routines, for those
    sites so backward as to not have the
     port. library */
/* munged for standard i/o library: peter and louise 5 may 80 */
#include <stdio.h>
#include <stdlib.h>

extern void error(char *s);

FILE *cin, *cout;

FILE *copen( char *s, char c )
  {
  FILE *f;

	  if( c == 'r' ){
	    f = fopen( s, "r" );
  } else  if( c == 'a' ){
	    f = fopen( s, "a" );
	    fseek( f, 0L, 2 );
  } else {  /* c == w */
	    f = fopen( s, "w" );
  }

  return( f );
  }

void cflush(FILE *x) { /* fake! sets file to x */
  fflush( cout );
  cout = x;
  }

void system_ptoc(){
  error( "The function \"system_ptoc\" is called" );
  }

void cexit(int i){
  fflush( cout );
  if ( i != 0 ) {
    abort();
  }
  exit(i);
  }
