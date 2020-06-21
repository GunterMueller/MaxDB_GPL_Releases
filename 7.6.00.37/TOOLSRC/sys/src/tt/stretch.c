/*!================================================================
 module:    vpr01Precom.c

 Autor: Burkhard Diesing
 Original Autor: Gerald Arnold

 description: Renames a File that is already in use.
 


    ========== licence begin  GPL
    Copyright (C) 2001 SAP AG

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

 ===================================================================*/

#include  <stdio.h>
#include <stdlib.h>
#include <string.h>

static int stretch_file(char *file);

int main(int argc, char ** argv)
{
  char *file;
  int i;
  if ( argc == 2 ) {
    for (i=1; i < argc;) {
      char *opt=argv[i];
      char *parm=argv[i+1];
      if (opt) {
	switch (*opt) {
	case ('-'):{}	
	case ('/'):{
	  opt++;
	  switch(*opt) {
	  case ('\0'): {
	    break;
	  }
	  case ('?'): {}
	  case ('h'): {
	    return(PrintHelp());
	    break;
	  }
	  default: {
	    return(PrintInvOption());
	  }		
	  } /* if (opt) */
	  break;
	}
	default: {
	  file = opt;
	  i++;
	  break;
	}
	}
      }
    } /* for */
    return(stretch_file(file));
  }
  else {
    return(PrintUsage());
  }
}

static int    stretch_file(char *file)
{
  int          rc=0 ;
  char        *fname;
  char        *newfile = NULL;
#ifndef WIN32
  char PathSep = '/';
#else
  char PathSep = '\\';
#endif
  
  newfile = malloc(strlen(file)+2);
  if(!newfile) {
    fprintf(stderr, "Memory allocation failed.\n" );
    return ( -2 );
  }
  if ( !( fname = strrchr( file, PathSep )) ) {
    /* no path information */
    fname = file;
  } else {
    fname++;
    (void) strncpy( newfile, file, fname-file );
    newfile[ fname-file ] = '\0';
  }
  strcat(strcat(newfile, "_"), fname);
  if ( access(newfile, 0) != -1 ) {
    /* try to unlink a existent stretched file */
    if ( unlink( newfile ) )
      stretch_file( newfile );
  }
  rc = rename( file, newfile );
  free(newfile);
  if ( rc ) {
    fprintf(stderr, "Stretch '%s' failed.\n", file, newfile );
    return ( rc );
  }
  return ( 0 );
}

int PrintUsage()
{
  fprintf(stderr, "Usage : stretch [-h] <file>\n\n");
  return(-1);
}

int PrintHelp()
{
  PrintUsage();
  fprintf(stdout, "        Renames a file that is already in use to _<file>.\n"
	          "        If a file with the name _<file> already exists\n"
                  "        stretch will call himself and so on.");
  return(0);
}
PrintInvOption()
{
  fprintf(stderr,  "Invalid option.\n");
  return(-2);
}
