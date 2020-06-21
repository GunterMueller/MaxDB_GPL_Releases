/*!
  @file           veo100.c
  @author         
  @special area   
  @brief          RTE - get_page_from_save
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



#include <stdlib.h>
#ifndef _WIN32
#include <unistd.h>
#else
#include <io.h>
#define F_OK 0
#endif

#include <fcntl.h>
#include <stdio.h>
#include "SAPDBCommon/SAPDB_string.h"
#include <ctype.h>

#define DEVSPACE_ID "SQL-DB DEVSPACE" 
#define PAGE_SIZE 4096
#define TRUE 1
#define FALSE 0
#define DEBUG

static int get_page ( int argc, char **argv ) ;
static int check_params ( char **params, int num_params, FILE **inp_file, int *pno, int *block_count, char **buf );
static int check_pno_and_bufcount ( char *pno, char *buf_count );
static int check_headerpage ( FILE *inp_file, int block_count, char **buf );
static int search_page ( FILE *infile, char *buf, int block_count, int pno, long *found_pos, int *blck_pos ) ;
static int read_one_block ( char *buf, FILE *infile, int bytes_to_read );
static int write_page ( int pno, char *buf, int blck_pos) ;
static int put_page ( int num_params, char **params ) ;
static int replace_page ( FILE *devsp, int devsp_pos, int pno, char *new_page) ;
static void sys_err     (const char *, char *);

static int debug = FALSE ;

#define LOCALDBG(x) if ( debug ) { printf x; }

/*----------------------------------------------------------------*/
int main ( int argc, char *argv[] ) 
{
  int  i, rc ;
  if ( (debug = ( argc > 1 && strcmp(argv[1], "-D" ) == 0) ) ) 
    { for ( i=2; i<argc; argv[i-1] = argv[i], i++ ) ;
      argc-- ;
    }
  if ( strstr(argv[0], "get_page") != (char *)0 )
    rc = get_page ( argc, argv ) ;
  else
  if ( strstr(argv[0], "put_page") != (char *)0 )
    rc = put_page ( argc, argv ) ;
  else
    printf("\nBad progname!\n");
  return ( rc ) ;  
}

/*----------------------------------------------------------------*/
static int get_page ( int argc, char **argv ) 
{
  FILE *infile;
  char *buf ;
  int pno, block_count, num_found = 0, blck_pos ;
  long pos ;
  int  rc = check_params ( argv, argc, &infile, &pno, &block_count, &buf ) ;
  if ( rc )
    { char *inp_fname = argc > 3 ? argv[3] : "stdin";					 
      printf("\nsearching for pno <%d> in file <%s> ...\n", 
               pno, inp_fname ) ;
      while ( !num_found && 
              search_page ( infile, buf, block_count, pno, &pos, &blck_pos ) && 
              write_page  ( pno, buf, blck_pos ) )
        { num_found++;
/*
	  printf("\nPage %d found, searching continued\n",pno) ;
*/
	}
      if ( ! ( rc = fclose ( infile ) == 0 ) )
        perror ( "can't close infile" ) ;
      if ( !num_found )
        printf("\npno <%d> not found in file <%s>\n", 
                 pno, inp_fname ) ;
      else
        printf("\npage <%d> found at pos %d and written to <%d.pno>\n", 
               pno, pos, pno ) ;

    }
  return ( rc == 1 ? 0 : rc ) ;  
}

/*----------------------------------------------------------------*/
static int check_params ( char **params, int num_params, FILE **inp_file, int *pno, int *block_count, char **buf )
{ 
  int check_ok = num_params > 2 ;
  if ( check_ok ) 
    {  if ( num_params == 4 )
         *inp_file = fopen ( params[3], "rb" ) ;
       else
         { *inp_file = stdin ;
#ifdef _WIN32
           check_ok  = setmode ( _fileno ( *inp_file ) , _O_BINARY ) != -1 
	                && setvbuf(	*inp_file, NULL, _IOFBF, PAGE_SIZE ) == 0;
#endif
         }
      if ( ( check_ok = *inp_file != NULL ) )
        { char f_name[20] ;
          check_ok = check_pno_and_bufcount ( params [1] , params [2] ) ;
          if ( check_ok ) 
            { *block_count = atoi ( params [2] ) ;
              check_ok = check_headerpage ( *inp_file, *block_count, buf ) ;
              if ( check_ok ) 
                { *pno = atoi ( params [1] ) ;
                  sprintf ( f_name, "%d.pno", *pno ) ;
                  if ( *inp_file != stdin && access ( f_name , F_OK ) == 0 )
                   { char c ;
                     printf ("\nFile %s exists, overwrite? ( Y / N ): ",f_name);
                     fflush ( stdout ) ;
                     c = getchar () ;
                     check_ok = c == 'Y' || c == 'y' ;
                   }  
               }  
            }
        }
      else
        perror ( "can't open file for reading" ) ;
    }
  else
    printf("\nUSAGE: %s <page_no> <block_cnt> <savefile>\n", params[0] ) ; 
  return ( check_ok ) ;
}

/*----------------------------------------------------------------*/
static int check_pno_and_bufcount ( char *pno, char *buf_count )
{
  int check_ok = TRUE;
  int i ;

  for ( i=0; pno[i] != '\0' && check_ok; i++ )
    check_ok = isdigit ( pno[i] ) ;

  if ( check_ok ) 
    for ( i=0; buf_count[i] != '\0' && check_ok; i++ )
      check_ok = isdigit ( buf_count[i] ) ;

  if ( !check_ok ) 
    printf("\n<pno> and <block_cnt> must be integers\n" ) ; 

  return ( check_ok ) ;
}

/*----------------------------------------------------------------*/
static int check_headerpage ( FILE *inp_file, int block_count, char **buf )
{
  int check_ok = block_count > 0 ;
 
  if ( check_ok )
    { int blocksize = block_count * PAGE_SIZE ;
      *buf = (char *) malloc ( blocksize ) ;
      if ( ( check_ok = buf != NULL ) )
        { check_ok = read_one_block ( *buf, inp_file, blocksize ) == blocksize ;
          if ( check_ok ) 
            { int blocksize_wanted = 
                      (unsigned ) ( ((*buf)[2] << 8 ) + (*buf)[3] ) ;
              if ( !(check_ok = blocksize_wanted == blocksize ))
                printf("invalid blocksize! ( given = %d, wanted = %d )\n", 
                        blocksize, blocksize_wanted ) ;
                 
            }
          else
            printf("error during reading headerpage with blocksize %d\n", 
                    blocksize ) ;
        }
      else
        perror ( "can't allocate memory for given blocksize" ) ;
    }
  else
    printf("invalid blocksize %d\n", block_count ) ;

  return ( check_ok ) ;
}

/*----------------------------------------------------------------*/
static int search_page ( FILE *infile, char *buf, int block_count, int pno, long *found_pos, int *blck_pos ) 
{
  int found=FALSE, read_ok, found_pno, num_read, last_pno_found=0  ;
  long tot_pages = 0 ;
  int is_pno_valid ;
  int num_to_read = block_count * PAGE_SIZE ;
  char *buf_beg = buf ;
  do
    { num_read = read_one_block ( buf_beg, infile, num_to_read ) ;	 
      if ( (read_ok = num_read == num_to_read) )
        for ( buf = buf_beg, *blck_pos=0 ; 
             !found && *blck_pos < block_count; )
          { found_pno    = *((int *)buf) ;
            is_pno_valid = buf[4] == 0x08 && *((short *) & buf[6]) != 0 ;
            found        = found_pno == pno && is_pno_valid ;
            if ( !found )
  	      { if ( is_pno_valid && (found_pno > last_pno_found) )
  	          last_pno_found = found_pno ;
          
                if ( (++tot_pages & 0xfff) == 0xfff )
  	          { printf(".");
#ifndef WIN32
                    fflush(stdout) ;
#endif
                  }
                buf += PAGE_SIZE ;
                (*blck_pos)++ ;
              }
  	    if ( is_pno_valid )
              LOCALDBG(("\nPNO found: %d", found_pno )) ;
          }
    }
  while ( !found && (read_ok || ( num_read == 0 && !feof (infile) ))) ;
  if ( !found )
    switch ( num_read )
      { case -1 : perror("error during reading savefile"); break ;
        case  0 : printf("\nhighest valid pno found: %d!\n", last_pno_found ) ;
		          break ;
        default : printf("\nbad savefile size (%d) !\n", num_read ) ;
  			  break ;
      }			 
  else
    *found_pos = tot_pages ;
  return ( read_ok && found ) ;
}

/*----------------------------------------------------------------*/
static int read_one_block ( char *buf, FILE *infile, int bytes_to_read )
{                       
  int read_ret = (int)fread ( buf, sizeof (char), bytes_to_read, infile ) ;
  int num_read = read_ret, num_to_read ;
  int End = read_ret < 0 || feof (infile) ;
  while ( !End && num_read < bytes_to_read )
    { num_to_read = bytes_to_read - num_read ;
      read_ret = (int)fread ( buf+num_read, sizeof(char), num_to_read, infile ) ;
      if ( read_ret > 0 ) 
        { num_read += read_ret ;
          LOCALDBG(("\nnum_read = %d, read_ret = %d",num_read, read_ret)) ;
        }
      else
          End = read_ret < 0 || feof (infile) ;
    }
  return ( num_read ) ;
}
             
/*----------------------------------------------------------------*/
static int write_page ( int pno, char *buf, int blck_pos) 
{
  int write_ok, block_size=PAGE_SIZE ;
  FILE *outp_file ;
  char f_name[20] ;

  sprintf ( f_name, "%d.pno", pno ) ;
  LOCALDBG(("\nwrite_page: pno = %d, blck_pos = %d",pno , blck_pos)) ;
  outp_file = fopen ( f_name, "wb" ) ;
  if ( !( write_ok = outp_file != NULL ) )
    perror ( "can't open outputfile for writing" ) ;
  else
    { char header_page[PAGE_SIZE] ;
      SAPDB_memset ( header_page, '\0', PAGE_SIZE ) ;
      header_page[2] = (char) (block_size >> 8);
      header_page[3] = (char) (block_size & 0xff);
      SAPDB_memcpy ( header_page + 8 , DEVSPACE_ID , strlen (DEVSPACE_ID));
       
      write_ok = fwrite ( header_page, sizeof(char), PAGE_SIZE, outp_file ) 
                 == PAGE_SIZE ;
      if ( !write_ok )  
        perror ( "can't write headerpage to outputfile" ) ;
      else
        { write_ok = fwrite ( buf + PAGE_SIZE * blck_pos , 
                     sizeof(char), PAGE_SIZE, outp_file ) == PAGE_SIZE ;
          if ( ! write_ok ) 
            perror ( "can't write page to outputfile" ) ;
          else
            if ( fclose ( outp_file ) != 0 )
              perror ( "can't close outputfile" ) ;
        }
    }
  return ( write_ok ) ;
}

/*----------------------------------------------------------------*/
static int put_page ( int num_params, char **params ) 
{ 
  int check_ok = num_params == 4 ;
  if ( check_ok ) 
    { FILE *page_file, *devsp_file = fopen ( params[1], "r+b" ) ;
      if ( ( check_ok = devsp_file != NULL ) )
        {  char f_name[20], new_page[PAGE_SIZE] ;
           int pno_pos = atoi ( params [2] ) ;
           int pno     = atoi ( params [3] ) ;
           sprintf ( f_name, "%d.pno", pno ) ;
           page_file = fopen ( f_name, "rb" ) ;
           if ( ( check_ok = page_file != NULL ) )
             { check_ok = fread ( new_page, sizeof (char ), 
                                  PAGE_SIZE, page_file ) == PAGE_SIZE ;
               if ( check_ok ) 
                 if ( *(( int *) new_page) != pno )
                   printf("\nbad pno(%d) in %s\n", *(( int *) new_page) , f_name ) ;
                 else
                   replace_page (devsp_file, pno_pos, pno, new_page ) ;
               else
                 sys_err ( "can't read new_page from file", f_name ) ;
               fclose ( page_file ) ;
             }
           else
             sys_err ( "can't open file for reading: ", f_name ) ;
        }
      else
        sys_err ( "can't open file for reading and writing:", params[1] ) ;
    }
  else
    printf("\nUSAGE: %s <db_devspace> <page_pos> <page_no>\n", params[0] ) ; 
  return ( !check_ok ) ;  
}

/*----------------------------------------------------------------*/
static int replace_page ( FILE *devsp, int devsp_pos, int pno, char *new_page) 
{
  long pos_to_seek = devsp_pos * PAGE_SIZE; 
  int seek_ok      = fseek ( devsp, pos_to_seek, SEEK_SET ) == 0 ;
  if ( !seek_ok ) 
    perror ( "can't seek to specified position" ) ;
  else
    { char buf[PAGE_SIZE] ;
      seek_ok = fread ( buf, sizeof (char ), PAGE_SIZE, devsp ) == PAGE_SIZE ;
      if ( !seek_ok ) 
        perror ( "can't reading from specified pos" ) ;
      else
        if ( !(seek_ok = *((int *)buf) == pno ) )
          printf ( "\ninvalid page on specified position!(%d)\n",
                    *((int *)buf) );
        else 
          { seek_ok = fseek ( devsp, pos_to_seek, 0 ) == 0 &&
                      fwrite( new_page , sizeof(char), PAGE_SIZE, devsp ) 
                      == PAGE_SIZE &&
                      fclose ( devsp ) == 0 ;
            if ( !seek_ok ) 
              perror ( "can't write new page to specified position" ) ;
          }
    }
  return ( seek_ok ) ;
}

/*----------------------------------------------------------------*/
static void sys_err ( const char *err_str, char *err_para )
{
  char err_line[80+256];
  sprintf(err_line, "%s %s", err_str, err_para ) ;
  perror ( err_line ) ;
  return ;
}
  

