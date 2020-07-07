/* @lastChanged: "1999-02-02  10:49"
 * @filename:    what.c
 * @purpose:     "what UNIX compatible what string parsing"
 * @release:     7.1.0
 * @see:         ""
 *
 * @Copyright (c) 1998-2005 SAP AG"



    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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




 */


/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "precom/what.h"
#if defined (SOLARIS)
#include <unistd.h>
#endif

/*==================================================================*
 *  DECLARATIONS                                                    *
 *==================================================================*/

#define TEST_SIZE 4
#define LIST_SIZE 64
#define BUFF_SIZE 65536

static  char   *test_text = "@(#)";
extern  void    exit( int code );

/*===========================================================================*
 *  CODE                                                                     *
 *===========================================================================*/

void PrecomWhat(char *szProgName,  int argc , char **argv )
{ int   case_typ = 0 , char_typ = 0;    /* case and character switch */
int   argv_pos = 0 , file_des;        /* position in argument list and filedescriptor */
int   buff_pos = 0 , buff_len;        /* positions in I/O-buffer for inputfile */
int   text_pos = 0 , text_len;        /* positions in extracted text of inputfile */
int   list_len = 0 , list_pos;        /* positions in list of searchstrings */
int   uniq_len = 0 , uniq_pos;        /* positions in list of uniqueness */
char  buff_ptr[TEST_SIZE + BUFF_SIZE];/* I/O-buffer for inputfile */
char *name_ptr , *text_ptr;           /* name and extracted text of inputfile */
char **list_ptr = NULL;               /* list of searchstrings */
char **uniq_ptr = NULL;               /* list of uniqueness of printed text */
#ifndef _WIN32
char *line_ptr;                       /* converted text to uppercase */
#endif
/*
  //      Get first argument = filename
  */
if ( ++ argv_pos >= argc )
  { fprintf( stderr , "Usage: %s filename [-c] [-i] [string ...]\n" , szProgName );
  exit( 1 );
  } else name_ptr = argv[argv_pos];
/*
  //      Get next optional arguments : -c, -i or searchstrings
  */
while ( ++ argv_pos < argc )
  { switch ( argv[argv_pos][0] )
    { default:
      if ( (list_len % LIST_SIZE) == 0 )
	{ list_pos = LIST_SIZE * sizeof( char * );
	/*
	  //      Allocate new items in the list of searchstrings
	  */
        if ( list_len )
	  { list_pos += list_len * sizeof( char * );
          list_ptr = (char **)realloc( list_ptr , list_pos );
	  } else
	    list_ptr = (char **)malloc( list_pos );
        if ( list_ptr == NULL )
	  { perror( szProgName );
          exit( 2 );
	  }
	}
      list_ptr[list_len ++] = argv[argv_pos];
      break;
    case '-':
    case '/':
      switch ( argv[argv_pos][1] )
	{ default:
	  fprintf( stderr , "%s: Invalid option %s\n" , szProgName , argv[argv_pos] );
	  exit( 3 );
	  break;
	case 'c':
	case 'C':
	  char_typ = 1;
	  break;
	case 'i':
	case 'I':
	  case_typ = 1;
	}
    }
  }
/*
  //      Open the inputfile
  */
#ifndef _WIN32
file_des = open( name_ptr , O_RDONLY );
#else
file_des = open( name_ptr , O_RDONLY | O_BINARY );
#endif
if ( file_des == -1 )
  { perror( szProgName );
  exit( 4 );
  }
#ifndef _WIN32
if ( case_typ )
  /*
    //      Convert searchstrings to uppercase
    */
  for ( list_pos = 0 ; list_pos < list_len ; list_pos ++ )
    for ( text_len = 0 ; text_len < strlen( list_ptr[list_pos] ) ; text_len ++ )
      list_ptr[list_pos][text_len] = toupper( list_ptr[list_pos][text_len] );
#endif
/*
  //      The searchbuffer overlaps by the size of the teststring, so clear it
  */
memset( buff_ptr , ' ' , TEST_SIZE );
do
  /*
    //      Read the next block from the file into the buffer
    */
  { int skip_txt = 0 , scan_len;
  buff_len = read( file_des , &buff_ptr[TEST_SIZE] , BUFF_SIZE );
  switch ( buff_len )
    { default:
      for ( buff_pos = 0 ; buff_pos < buff_len ; buff_pos ++ )
	/*
	  //      Scan the buffer for the occurrence of the teststring
	  */
	{ if ( strncmp( &buff_ptr[buff_pos] , test_text , TEST_SIZE ) == 0 )
	  { buff_pos += TEST_SIZE;
	  /*
	    //      Scan for the trailing zero, carriage return or linefeed
	    */
          for ( text_len = buff_pos ; text_len < buff_len ; text_len ++ )
	    { if ( buff_ptr[text_len] == '\0' ) break;
            if ( char_typ )
	      { if ( buff_ptr[text_len] == '\r' ) break;
              if ( buff_ptr[text_len] == '\n' ) break;
	      }
	    }
          text_len -= buff_pos;
	  /*
	    //      Copy the text into the allocated text memory
	    */
          text_ptr = (char *)malloc( text_len + 1 );
          if ( text_ptr == NULL )
	    { perror( szProgName );
            (void)close( file_des );
            exit( 2 );
	    }
          (void)memcpy( text_ptr , &buff_ptr[buff_pos] , text_len );
          buff_pos += text_len;
	  /*
	    //      If the trailing zero was not found succeed with the next blocks
	    */
          while ( buff_pos == buff_len )
	    /*
	      //      Move the overlapped part from the end to the front of the block
	      */
	    { (void)memcpy( buff_ptr , &buff_ptr[buff_pos] , TEST_SIZE );
	    /*
	      //      Read the next block from the file into the buffer
	      */
            buff_len = read( file_des , &buff_ptr[TEST_SIZE] , BUFF_SIZE );
            switch ( buff_len )
	      { default:
		/*/
		  //      Scan for the trailing zero, carriage return or linefeed
		  */
		  for ( buff_pos = 0 ; buff_pos < buff_len ; buff_pos ++ )
		  { if ( buff_ptr[buff_pos] == '\0' ) break;
		  if ( char_typ )
		  { if ( buff_ptr[text_len] == '\r' ) break;
                  if ( buff_ptr[text_len] == '\n' ) break;
		  }
		  }
		  if ( buff_pos > 0 )
		  /*
		         Append the next part to the reallocated text memory
		    */
		    { text_ptr = (char *)realloc( text_ptr , text_len + buff_pos + 1 );
		    if ( text_ptr == NULL )
		    { perror( szProgName );
		    (void)close( file_des );
		    exit( 2 );
		    }
		    (void)memcpy( &text_ptr[text_len] , buff_ptr , buff_pos );
		    text_len += buff_pos;
		    }
		    case 0:
		    /*
		      End-of-file found
		      */
break;
	      case -1:
		/*
		  //      I/O-error encountered
		  */
		perror( szProgName );
		exit( 5 );
	      }
	    }
          text_ptr[text_len] = '\0';
#ifndef _WIN32
          if ( case_typ )
	    /*
	      //      Convert text memory to uppercase in the allocated line memory
	      */
	    { line_ptr = (char *)malloc( text_len + 1 );
            if ( line_ptr == NULL )
	      { perror( szProgName );
              (void)close( file_des );
              exit( 2 );
	      }
            for ( text_pos = 0 ; text_pos < text_len ; text_pos ++ )
              line_ptr[text_pos] = toupper( text_ptr[text_pos] );
            line_ptr[text_len] = '\0';
	    } else line_ptr = text_ptr;
#endif
	  /*
	    //      Compare the line memory with the searchstrings
	    */
          for ( list_pos = 0 ; list_pos < list_len ; list_pos ++ )
	    { skip_txt = 1;
            scan_len = strlen( list_ptr[list_pos] );
            for ( text_pos = 0 ; text_pos < text_len - scan_len ; text_pos ++ )
#ifndef _WIN32
	      { skip_txt = strncmp( &line_ptr[text_pos] , list_ptr[list_pos] , scan_len );
#else
	      { if ( case_typ )
                skip_txt = strnicmp( &text_ptr[text_pos] , list_ptr[list_pos] , scan_len );
              else
                skip_txt = strncmp( &text_ptr[text_pos] , list_ptr[list_pos] , scan_len );
#endif
              if ( !skip_txt ) break;
	      }
	      if ( !skip_txt ) break;
	      }
	    if ( !skip_txt )
	      /*
		      Searchstring found: test the text memory for uniqueness
		      */
	      { for ( uniq_pos = 0 ; uniq_pos < uniq_len ; uniq_pos ++ )
		if ( strcmp( text_ptr , uniq_ptr[uniq_pos] ) == 0 )
		  skip_txt = 1;
	      if ( !skip_txt )
		/*
		      Unique Searchstring found: print out the text
		*/
		{ fprintf( stdout , "%s\n" , text_ptr );
		if ( (uniq_len % LIST_SIZE) == 0 )
		  { uniq_pos = LIST_SIZE * sizeof( char * );
		  /*
		        Allocate new items in the list of uniqueness
		  */
		  if ( uniq_len )
		    { uniq_pos += uniq_len * sizeof( char * );
		    uniq_ptr = (char **)realloc( uniq_ptr , uniq_pos );
		    } else
		      uniq_ptr = (char **)malloc( uniq_pos );
		  if ( uniq_ptr == NULL )
		    { perror( szProgName );
		    (void)close( file_des );
		    exit( 2 );
		    }
		  }
		/*
		      Append the text memory to the list of uniqueness
		*/
		uniq_ptr[uniq_len ++] = text_ptr;
		text_ptr = NULL;
		}
	      }
	    /*
	          Free the allocated text and line memory
	    */
	    if ( text_ptr != NULL )
	      free( text_ptr );
#ifndef _WIN32
	    if ( case_typ )
	      free( line_ptr );
#endif
	    }
	  }
	/*
	      Move the overlapped part from the end to the front of the block
	*/
	(void)memcpy( buff_ptr , &buff_ptr[buff_pos] , TEST_SIZE );
	break;
	case 0:
	  /*
	        End-of-file found
	  */
	  break;
	case -1:
	  /*
	        I/O-error encountered
	  */
	  perror( szProgName );
	  exit( 5 );
	}
    } while ( buff_len > 0 );
    /*
          Free the list of uniqueness
    */
    for ( uniq_pos = 0 ; uniq_pos < uniq_len ; uniq_pos ++ )
      free( uniq_ptr[uniq_pos] );
    if ( uniq_ptr != NULL )
      free( uniq_ptr );
    /*
          Free the list of searchstrings
    */
    if ( list_ptr != NULL )
      free( list_ptr );
    /*
          Close the inputfile
    */
    if ( close( file_des ) == -1 )
      { perror( szProgName );
      exit( 6 );
      }
    exit( 0 );
  }
