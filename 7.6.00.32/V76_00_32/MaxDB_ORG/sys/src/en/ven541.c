/*!
  @file           ven541.c
  @author         JoergM
  @brief          RTE: Filenode info
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



#undef MOD__
#define MOD__ "ven541.c:"

#define RAWDEV_ALIGN 8192
#define USE_OLD_CODE /* PTS 1113151 */

#include "gen00.h"
#include "heo00.h"
#include "gsp01.h"
#include "gen500.h"
#include "heo46.h"

/* exported functions */

#include "gen541.h"
#include "hen541.h"
#include "geo001.h"
#include "geo57.h"
#include "hen40.h"

/* PTS 1105707 */
#if !defined(BIT64) && defined(O_LARGEFILE)
#define DEFAULT_OPEN_MODE_EN54 ( O_LARGEFILE )
#else
#define DEFAULT_OPEN_MODE_EN54 ( 0 )
#endif

/*
 * ===========================================================================
 */

void  SqlDevSize ( 
const char                        *devspace,
tsp00_Int4                        *devcapacity ,
tsp00_ErrTextc                     errtext,
tsp00_Bool                        *ok)
{

  #undef  MF__
  #define MF__ MOD__"SqlDevSize"

  tsp00_Int4 dumpyFsize;
  /* XXX SqlDevSize needs modification of interface to become tsp00_Page size independend */
  tsp00_Int4 block_size = sizeof(tsp00_Page) ;

  *ok = e541_get_nodinfo ((char *)devspace, &dumpyFsize) == MOD_CHARSPEC ;
  if ( *ok )
  {  
	   int fd = open( devspace, DEFAULT_OPEN_MODE_EN54 | O_RDONLY) ;
	   *ok = ( fd != -1 );
       if ( !*ok )
       { 
			strncpy ( errtext,"Cannot open devspace", sizeof ( tsp00_ErrTextc ));
            MSGD (( WRN_DEVSZ_OPEN_RAW_FAILED, devspace , sqlerrs() ));
       }
       else
       {
		  struct stat			stat_buf ;
          *ok = ( fstat ( fd, &stat_buf ) != -1 );
          if ( !*ok ) 
          {
			 strncpy ( errtext,"Error during fstat",sizeof ( tsp00_ErrTextc ));
             MSGD (( WRN_DEVSZ_FSTAT_FAILED, devspace, sqlerrs() ));
          }
          else
          {
			   *devcapacity = e541_get_devsize0 ( fd, block_size ) - 1 ;
               *ok = ( *devcapacity > 1 );
               if ( *ok )
               {
				   MSGD (( INFO_DEVSZ_CALCULATED_SIZE, 
						devspace, *devcapacity ));
               }
               else
               { 
			  	   strncpy ( errtext,"illegal devsize on devspace",
	   	               sizeof ( tsp00_ErrTextc ));
			       MSGD (( WRN_DEVSZ_ILLEGAL_DEVSIZE, 
						devspace, *devcapacity ));
               }   
           }
          (void) close (fd);
      }
  }
  else
  {
	  strncpy ( errtext,"device type must be RAW!",sizeof ( tsp00_ErrTextc ));
      MSGD (( WRN_DEVSZ_ILLEGAL_DEVTYPE ));
  }           
  return ; 
}

/*
 * ===========================================================================
 */

  #undef  MF__
  #define MF__ MOD__"e541_get_nodinfo"
/* Gets the filemode of the node specified by fname */
int     e541_get_nodinfo ( 
char     *fname,
tsp00_Int4 *fsize)
{
   int rtc;
   struct stat buf;

   rtc = stat(fname, &buf);

   if (rtc == -1)
   {
       rtc = MOD_UNKNOWN;
       DBG1 (( MF__,"get_nodeinfo: Unknown node-mode: %d \n", buf.st_mode  ));
       MSGD (( WRN_NODEINFO_STAT_ERROR, fname, sqlerrs() ));
   }
   else if
      ((buf.st_mode & MOD_FIFOSPEC) == MOD_FIFOSPEC) rtc = MOD_FIFOSPEC;
   else if
       ((buf.st_mode & MOD_CHARSPEC) == MOD_CHARSPEC) rtc = MOD_CHARSPEC;
   else if
       ((buf.st_mode & MOD_DIR     ) == MOD_DIR     ) rtc = MOD_DIR;
   else if
       ((buf.st_mode & MOD_BLCKSPEC) == MOD_BLCKSPEC) rtc = MOD_BLCKSPEC;
   else if
       ((buf.st_mode & MOD_ORDIFILE) == MOD_ORDIFILE ||
        (buf.st_mode & 0           ) == 0           ) rtc = MOD_ORDIFILE;
   else
   {
      rtc = MOD_UNKNOWN;
      MSGD (( IERR_NODEINFO_UNKNOWN_MODE, buf.st_mode ));
   }
   *fsize = (tsp00_Int4) (buf.st_size / PAGE_MXSP00);
   return rtc;
} /* e541_get_nodinfo */

/*
 * ===========================================================================
 */

int e541_get_devsize0 ( 
int       devfile,
tsp00_Int4  block_size )
{
  #undef  MF__
  #define MF__ MOD__"e541_get_devsize0"
    int		rc ;
    off_t	offseek ;
    off_t	offreturn ;
    off_t	pos ;
    char	*buf_beg;
    char	*buf;

    if ( ALLOC_MEM_EO57( (void **)&buf_beg, block_size + RAWDEV_ALIGN ) != NO_ERROR_EO001 )
    {
        MSGD (( WRN_DEVSZ0_MALLOC_ERROR, sqlerrs() ));
        return ( 0 );
    }

    buf = (char*) ( ( (unsigned long)(buf_beg + RAWDEV_ALIGN-1) / RAWDEV_ALIGN ) 
					* RAWDEV_ALIGN );

    pos = (off_t) 1 ;

#ifndef USE_OLD_CODE

        /* PTS 1109721
           SEEK_END not supported for raw devices... 
         */

        /* This code does not uses seeks in steps of power of 2 to find the maximum possible position.
           The previous tests used positions outside of the allowed range of the device, which has
           lead to disturbing messages in syslog...
         */
    while ( ((off_t)1) == pos ) 
    {
        /* Seek to end of file to get maximum size */
        offreturn = lseek ( devfile, (off_t)0, SEEK_END );
        if ( ((off_t)-1) == offreturn )
        {
            MSGD (( ERR_NEWDEVSIZE_ERR_SEEK, sqlerrs() ));
            break;
        }

        /* Calculate number of pages */
        pos = offreturn/block_size;
        if ( pos <= (off_t)1 )
        {
            MSGD (( ERR_NEWDEVSIZE_ERR_SEEK, "device too small" ));
            break;
        }

        /* Calculate position of last block and test read only access to it */
        offseek = (pos - 1) * block_size;

        offreturn = lseek ( devfile, offseek, SEEK_SET );
        if ( ((off_t)-1) == offreturn )
        {
            MSGD (( ERR_NEWDEVSIZE_ERR_SEEK, sqlerrs() ));
            pos = (off_t)0;
            break;
        }
        rc = read ( devfile, buf, (unsigned) block_size );
	    if ( rc != (int) block_size )
	    {
            MSGD (( ERR_NEWDEVSIZE_ERR_READ, sqlerrs() ));
            pos = (off_t)0;
            break;
        }
    } /* end while (off_t)1 == pos */

#else /* USE_OLD_CODE */ /* PTS 1109721 */

      /* PTS 1109721
         SEEK_END not supported for raw devices... 
       */
    {
        off_t	delta;
        for (;;)
        {

            pos *= (off_t) 2 ;
            DBG1 (( MF__,"pos          %ld", (long) pos ));
            offseek = pos * (off_t) block_size ;
            DBG1 (( MF__,"offseek      %ld", (long) offseek ));
		    if ( ! offseek )
		    {
			    /* The multiplication may exceed the representable range which results in a zero result... */
			    break ;
		    }

            offreturn = lseek ( devfile, offseek, SEEK_SET );
		    if ( offreturn == (off_t) -1 )
		    {
			    break ;
		    }

            rc = read ( devfile, buf, (unsigned) block_size );
		    DBG1 (( MF__,"read returned %d", rc ));
		    if ( rc != (int) block_size )
		    {
			    break ;
		    }
        }

        for ( pos /= (off_t) 2 , delta = pos / (off_t) 2 ;
	      delta >= (off_t) 1 ;
	      delta /= (off_t) 2 )
        {
		    DBG1 (( MF__,"delta        %ld", (long) delta ));
            pos += delta;
            DBG1 (( MF__,"pos          %ld", (long) pos ));
		    offseek = pos * (off_t) block_size ;
            DBG1 (( MF__,"offseek      %ld", (long) offseek ));

		    offreturn = lseek ( devfile, offseek, SEEK_SET );
		    if ( offreturn == (off_t) -1 )
		    {
			    pos -= delta;
			    continue ;
		    }

            rc = read ( devfile, buf, (unsigned) block_size );
		    DBG1 (( MF__,"read returned %d", rc ));
		    if ( rc != (int) block_size )
		    {
			    pos -= delta;
			    continue ;
		    }
        }

        offseek = pos * (off_t) block_size ;
        offreturn = lseek ( devfile, offseek, SEEK_SET );
        DBG1 (( MF__,"offseek %ld -> offreturn %ld", (long) offseek, (long)offreturn ));
        if ( offreturn == (off_t) -1 )
        {
    	    pos -- ;
        }
        else
        {
	        rc = read ( devfile, buf, (unsigned) block_size );
	        DBG1 (( MF__,"vdevsize0: read returned %d \n", rc ));
	        if ( rc != (int) block_size )
	        {
	            pos -- ;
	        }
        }
    }
#endif /* USE_OLD_CODE */ /* PTS 1109721 */

    FREE_MEM_EO57 ( buf_beg );

    DBG1 (( MF__,"devsize = %d\n", (int)pos ));

    if ( ((int)pos) != pos )
    {
        MSGD (( ERR_NEWDEVSIZE_ERR_SEEK, "device to big" ));
        pos = 0x7FFFFFFF;
    }

    return ( (int) pos );
}

/*
 * ===========================================================================
 */


  #undef  MF__
  #define MF__ MOD__"en541_fcErr"
static void en541_fcErr ( char             *FileName,
                          char             *RteTxt  ,
                          tsp01_RteError    *RteError )
{
  sqlos_errcode_and_errtext   ( &RteError->OsErrCode,
                                 RteError->OsErrText,
                                 sizeof ( RteError->OsErrText) ) ;

  RteError->RteErrCode = !RTE_NO_ERROR_SP01 ;
  eo46_rte_errtext_with_filename ( RteTxt,
                                   FileName,
                                   RteError->RteErrText ,
                                   sizeof( RteError->RteErrText ) );
}

/*
 * ===========================================================================
 */
#define ERROR_OPEN_FILE_FOR_READ  "Can't open (read) File: "
#define ERROR_OPEN_FILE_FOR_WRITE "Can't open (write) File: "
#define ERROR_READ_FILE           "Can't read File: "
#define ERROR_WRITE_FILE          "Can't write File: "

int    e541_filecopy  (  char             *SrcName,
                         char             *DestName,
                         tsp01_RteError    *RteError )
{
  #undef  MF__
  #define MF__ MOD__"e541_filecopy"

  int src_stream = open ( SrcName, DEFAULT_OPEN_MODE_EN54|O_RDONLY );
  tsp00_Bool fCopyOk = ( src_stream >= 0 );

  SAPDB_memset(RteError, 0, sizeof(tsp01_RteError) ) ;
  RteError->RteErrCode = RTE_NO_ERROR_SP01 ;
  RteError->OsErrCode  = OS_NO_ERROR_SP01 ;

  DBG3 ((MF__, "filecopy: <%s> ==> <%s> called\n", SrcName, DestName ));
  if ( fCopyOk )
  {
      int dest_stream;
      struct stat orgBuf;
      mode_t createMode = (mode_t)0660;

      if ( 0 == stat(SrcName, &orgBuf) )
      {
          createMode = orgBuf.st_mode;
      }

      dest_stream = open ( DestName, DEFAULT_OPEN_MODE_EN54|O_WRONLY|O_CREAT|O_TRUNC, createMode ) ;
      if ( ! ( fCopyOk = (dest_stream >= 0) ) )
      {
        en541_fcErr ( DestName, ERROR_OPEN_FILE_FOR_WRITE, RteError ) ;
      }
      else
#define COPY_BUFFER_SIZE (32*1024)
      {
          char buf [ COPY_BUFFER_SIZE ] ;
          int num_read, num_written;

          do
          {
              num_read = read ( src_stream, (char *)buf, sizeof ( buf ) );
              if ( num_read < 0 )
			  {
                  en541_fcErr ( SrcName, ERROR_READ_FILE, RteError ) ;
			  }
              else if ( num_read > 0 )
              {
                  num_written = write ( dest_stream, (char *)buf, num_read ) ;
                  if ( num_written < 0 )
                  {
                      en541_fcErr ( DestName, ERROR_WRITE_FILE, RteError ) ;
                  }
              }
          }
          while ( num_read > 0
               && num_read == num_written ) ;

          fCopyOk = ( num_read == 0 ) ;

          close ( dest_stream ) ;
      }
      close ( src_stream ) ;
  }
  else
  {
      en541_fcErr ( SrcName, ERROR_OPEN_FILE_FOR_READ, RteError ) ;
  }

  DBGOUT;
  return((int )fCopyOk) ;
}
