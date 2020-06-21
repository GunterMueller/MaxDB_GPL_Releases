/*      vmakcomm.c       

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

/* we have remote access!! */
/* #define  SOCKETS */

#include "devconfig.h"

#include "vmakcomm.h"
#include "vmake.h"
#include "vmakmisc.h"


# ifdef  SOCKETS
#       if  defined(WIN32)
#           include			<winsock.h>
#           define      EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#       else
#           include			<sys/socket.h>
#           include			<netinet/in.h>
#           include			<arpa/inet.h>
#           include			<netdb.h>
#           ifdef HAVE_SYS_SELECT_H
#               include			<sys/select.h>
#           endif
#       endif
# endif
# ifdef HAVE_DIRENT_H
#  include	<dirent.h>
# else
    typedef	struct
		{
		    long			dir_handle ;
		    int				dir_entryno ;
		    struct _finddata_t		dir_data ;
		    char			dir_name [1];
		}
					DIR ;
    struct dirent
    {
	    long		d_ino;		/* inode number of entry */
	    off_t		d_off;		/* offset of disk dir entry */
	    unsigned short	d_reclen;	/* length of this record */
	    char		d_name[_MAX_FNAME];	/* name of file */
    };
    extern DIR			    * opendir(char*);
    extern struct dirent	* readdir();
    extern int			    closedir();
# endif


/* local functions */

#ifdef	SOCKETS
static	int			open_connection (char*,struct connection_info**);
static	int			get_message (struct connection_info*);
static	int			put_message (struct connection_info*);
static	void		remove_connection (int);
#endif

/* local data */

#ifdef	SOCKETS
static	struct connection_info  * connections ;
static	struct message          remote_msg ;
	/*
	 *  remote_file is an array instead of a chained list 
	 *  because the reference number for the vmakeserver
	 *  is the index of the remote file server.
	 */
static	RFILE   remote_file [ MAXREMOTEFILES ] ;

#if WIN32
static	int	wsa_started ;
#endif	/*WIN32*/

#endif	/*SOCKETS*/


/*==========================================================================*/


global	int     filestat
(
    char                    * name ,
    struct stat             * statbuf 
)
{
	int                     rc=0 ;
	char                    * fname ;
#ifdef	SOCKETS
	int                     lgt ;
	struct connection_info  * cip ;
	char                    * p , node [ NAMESIZE ] ;
#endif

	DBG2 (( "filestat: called \n" ))
	DBG3 (( "filestat: name '%s' \n", name ))

	fname = strchr ( name , NODE_SEPARATOR );
	if ( ! fname || ((fname - name) < 2) ) /* C: is drive name */
	{
		rc = stat ( name , statbuf );
		return ( rc );
	}

#ifndef SOCKETS
	DBG0 (( "filestat: remote access not implemented '%s' \n", name ))
	seterrno(EINVAL) ;
	return ( -1 );
#else
	DBG7 (( "filestat: remote access \n" ))

	FILL ( node , 0 , sizeof(node) );
	lgt = fname - name ;
	if ( lgt >= sizeof(node) )
	{
		DBG0 (( "filestat: node name too long: '%s' \n", name ))
		seterrno(EINVAL) ;
		return ( -1 );
	}
	COPY ( node , name , lgt );
	DBG6 (( "filestat: node  '%s' \n", node  ))

	fname ++ ;
	DBG6 (( "filestat: fname '%s' \n", fname ))

	/* find existing or open new connection to server */

	rc = open_connection ( node , & cip );
	if ( rc )
	{
		DBG0 (( "filestat: cannot access '%s' \n", name ))
		return ( -1 );
	}
	DBG7 (( "filestat: got connection \n" ))

	/* issue stat request */

	lgt = strlen ( fname ) + 1 ; /* include null character ('\0') */
	if ( lgt > MESSAGESIZE )
	{
		DBG0 (( "filestat: file name too long: '%s' \n", fname ))
		seterrno(EINVAL) ;
		return ( -1 );
	}
	DBG6 (( "filestat: fname length %d \n", lgt ))

	COPY (  remote_msg.data , fname , lgt );

	remote_msg.type         = REM_STAT ;
	remote_msg.reference    = (char) 0 ;
	remote_msg.length [ 0 ] = lgt      & 0xFF ;   /* low  byte */
	remote_msg.length [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( cip );
	if ( rc != HEADER_SIZE + lgt )
	{
		DBG0 (( "filestat: request length mismatch %d \n", rc ))
		if ( rc >= 0 ) seterrno(EIO) ;
		return ( -1 );
	}
	DBG6 (( "filestat: sent request to server (%d bytes) \n", rc ))

	lgt = get_message ( cip );
	if ( lgt - HEADER_SIZE != 30 )
	{
		if ( lgt - HEADER_SIZE == 2 )
		{
			/* got errno */
			p = (char*) remote_msg.data ;
			seterrno (  p [  0 ]       & 0x00FF |
				        p [  1 ] <<  8 & 0xFF00 );

		}
		else
		{
			DBG0 (( "filestat: illegal reply size %d \n", lgt ))
			seterrno(EIO) ;
		}
		return ( -1 );
	}
	DBG6 (( "filestat: got reply from server (%d bytes) \n", lgt ))

	p = (char*) remote_msg.data ;
	statbuf->st_mode  =   p [  0 ]       & 0x00FF |
				p [  1 ] <<  8 & 0xFF00 ;
	statbuf->st_ino   =   p [  2 ]       & 0x00FF |
				p [  3 ] <<  8 & 0xFF00 ;
	statbuf->st_dev   =   p [  4 ]       & 0x00FF |
				p [  5 ] <<  8 & 0xFF00 ;
	statbuf->st_rdev  =   p [  6 ]       & 0x00FF |
				p [  7 ] <<  8 & 0xFF00 ;
	statbuf->st_nlink =   p [  8 ]       & 0x00FF |
				p [  9 ] <<  8 & 0xFF00 ;
	statbuf->st_uid   =   p [ 10 ]       & 0x00FF |
				p [ 11 ] <<  8 & 0xFF00 ;
	statbuf->st_gid   =   p [ 12 ]       & 0x00FF |
				p [ 13 ] <<  8 & 0xFF00 ;
	statbuf->st_size  =   p [ 14 ]       & 0x000000FF |
				p [ 15 ] <<  8 & 0x0000FF00 |
				p [ 16 ] << 16 & 0x00FF0000 |
				p [ 17 ] << 24 & 0xFF000000 ;
	statbuf->st_atime =   p [ 18 ]       & 0x000000FF |
				p [ 19 ] <<  8 & 0x0000FF00 |
				p [ 20 ] << 16 & 0x00FF0000 |
				p [ 21 ] << 24 & 0xFF000000 ;
	statbuf->st_mtime =   p [ 22 ]       & 0x000000FF |
				p [ 23 ] <<  8 & 0x0000FF00 |
				p [ 24 ] << 16 & 0x00FF0000 |
				p [ 25 ] << 24 & 0xFF000000 ;
	statbuf->st_ctime =   p [ 26 ]       & 0x000000FF |
				p [ 27 ] <<  8 & 0x0000FF00 |
				p [ 28 ] << 16 & 0x00FF0000 |
				p [ 29 ] << 24 & 0xFF000000 ;

	DBG4 (( "filestat: stated '%s' \n", name ))
	return ( 0 );
#endif
}


/*==========================================================================*/


global	VFILE   * fileopen 
(
    char    * name ,
    char    * type 
)
{
	VFILE                   * vfp ;
	FILE                    * fp ;
	char                    * fname ;
#ifdef	SOCKETS
	int                     fno , lgt , typlgt , rc=0 ;
	RFILE			* rfp ;
	struct connection_info  * cip ;
	struct file_cache       * fcp ;
	char                    node [ NAMESIZE ] ;
#endif

	DBG2 (( "fileopen: called \n" ))
	DBG3 (( "fileopen: name '%s' \n", name ))
	DBG3 (( "fileopen: type '%s' \n", type ))

	fname = strchr ( name , NODE_SEPARATOR );
	if ( ! fname || ((fname - name) < 2) ) /* C: is drive name */
	{
		fp = fopen ( name , type );
		if ( fp == NULL ) return ( NULL );
		vfp = (VFILE*) palloc ( sizeof(VFILE) );
		vfp->type = FT_LOCAL ;
		vfp->v_fp.l_fp = fp ;
		return ( vfp );
	}

#ifndef SOCKETS
	DBG0 (( "ERROR: remote access not implemented '%s' \n", name ))
	return ( NULL );
#else
	DBG7 (( "fileopen: remote access \n" ))

	/* find empty slot in remote files list */

	for ( fno = 0 ; fno < MAXREMOTEFILES ; fno ++ )
	{
		if ( remote_file [ fno ].in_use == 0 ) break ;
	}
	if ( fno >= MAXREMOTEFILES )
	{
	    DBG0 (( "fileopen: no more remote files. in use are %d \n", fno ))
	    return ( NULL );
	}
	DBG6 (( "fileopen: found free file no. %d \n", fno ))

	rfp = & remote_file [ fno ] ;

	FILL ( node , 0 , sizeof(node) );
	lgt = fname - name ;
	if ( lgt >= sizeof(node) )
	{
	    DBG0 (( "fileopen: node name too long: '%s' \n", name ))
	    return ( NULL );
	}
	COPY ( node , name , lgt );
	DBG6 (( "fileopen: node  '%s' \n", node  ))

	fname ++ ;
	DBG6 (( "fileopen: fname '%s' \n", fname ))

	/* find existing or open new connection to server */

	rc = open_connection ( node , & cip );
	if ( rc )
	{
	    DBG0 (( "fileopen: cannot access '%s' \n", name ))
	    return ( NULL );
	}
	DBG7 (( "fileopen: got connection \n" ))

	/* allocate file cache */

	fcp = (struct file_cache *) palloc ( sizeof(struct file_cache) );
	if ( fcp == NULL )
	{
		DBG0 (( "insufficient dynamic memory \n" ))
		return ( NULL );
	}
	FILL ( fcp , 0 , sizeof(struct file_cache) );
	DBG7 (( "fileopen: got file cache \n" ))

	/* issue open request */

	lgt    = strlen ( fname ) + 1 ; /* include null character ('\0') */
	typlgt = strlen ( type  ) + 1 ; /* include null character ('\0') */
	if ( lgt + typlgt > MESSAGESIZE )
	{
		DBG0 (( "fileopen: file name too long: '%s' \n", fname ))
		FREE_AND_SET_TO_NULL(fcp);
		return ( NULL );
	}
	DBG6 (( "fileopen: fname length %d \n", lgt    ))
	DBG6 (( "fileopen: type  length %d \n", typlgt ))

	COPY (  remote_msg.data       , fname , lgt    );
	COPY (  remote_msg.data + lgt , type  , typlgt );
	lgt += typlgt ;

	remote_msg.type         = REM_OPEN ;
	remote_msg.reference    = (char) fno ;
	remote_msg.length [ 0 ] = lgt      & 0xFF ;   /* low byte */
	remote_msg.length [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( cip );
	if ( rc != HEADER_SIZE + lgt )
	{
		DBG0 (( "fileopen: request length mismatch %d \n", rc ))
		FREE_AND_SET_TO_NULL(fcp);
		return ( NULL );
	}
	DBG6 (( "fileopen: sent request to server (%d bytes) \n", rc ))

	lgt = get_message ( cip );
	if ( lgt < HEADER_SIZE )
	{
		DBG0 (( "fileopen: illegal reply size %d \n", lgt ))
		FREE_AND_SET_TO_NULL(fcp);
		return ( NULL );
	}
	DBG6 (( "fileopen: got reply from server (%d bytes) \n", lgt ))

	if ( remote_msg.reference != (char) fno )
	{
		DBG4 (( "fileopen: cannot open '%s' \n", name ))
		FREE_AND_SET_TO_NULL(fcp);
		return ( NULL );
	}

	lgt -= HEADER_SIZE ;
	COPY (  fcp->data ,  remote_msg.data , lgt );
	fcp->remain = lgt ;
	fcp->current = 0 ;
	if ( remote_msg.type == REM_CLOSE ) fcp->eof = 1 ;

	rfp->fcp    = fcp ;
	rfp->cip    = cip ;
	rfp->ref    = fno ;
	rfp->in_use = 1 ;

	vfp = (VFILE*) palloc ( sizeof(VFILE) );
	vfp->type = FT_REMOTE ;
	vfp->v_fp.r_fp = rfp ;

	DBG4 (( "fileopen: opened '%s' \n", name ))
	return ( vfp );
#endif
}


/*==========================================================================*/


global	int     fileclose
(
    VFILE                   * vfp 
)
{
	int                     rc=0 ;
	FILE			* fp ;
#ifdef	SOCKETS
	int                     lgt ;
	RFILE			* rfp ;
	struct connection_info  * cip ;
	struct file_cache       * fcp ;
#endif

	DBG2 (( "fileclose: called \n" ))

	if ( vfp->type == FT_LOCAL )
	{
		fp = vfp->v_fp.l_fp ;
		rc = fclose ( fp );
		if ( ! rc ) FREE_AND_SET_TO_NULL(vfp);
		return ( rc );
	}

#ifndef SOCKETS
	DBG0 (( "ERROR: remote access not implemented \n" ))
	return ( EOF );
#else
	DBG7 (( "fileclose: remote access \n" ))

	rfp = vfp->v_fp.r_fp ;

	if ( ! rfp->in_use )
	{
		DBG0 (( "fileclose: file not open \n" ))
		return ( EOF );
	}
	fcp = rfp->fcp ;

	if ( fcp->eof )
	{
		/* already closed by server */
		FREE_AND_SET_TO_NULL(fcp);
		FILL (  rfp , 0 , sizeof(RFILE) );
		return ( 0 );
	}

	cip = rfp->cip ;

	lgt = 0 ;
	remote_msg.type         = REM_CLOSE ;
	remote_msg.reference    = (char) rfp->ref ;
	remote_msg.length [ 0 ] = lgt      & 0xFF ;   /* low byte */
	remote_msg.length [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( cip );
	if ( rc != HEADER_SIZE )
	{
		DBG0 (( "fileclose: request length mismatch %d \n", rc ))
		FREE_AND_SET_TO_NULL(fcp);
		FILL (  rfp , 0 , sizeof(RFILE) );
		return ( EOF );
	}
	DBG6 (( "fileclose: sent request to server (%d bytes) \n", rc ))

	lgt = get_message ( cip );
	if ( lgt != HEADER_SIZE )
	{
		DBG0 (( "fileclose: illegal reply size %d \n", lgt ))
		FREE_AND_SET_TO_NULL(fcp);
		FILL (  rfp , 0 , sizeof(RFILE) );
		return ( EOF );
	}
	DBG6 (( "fileclose: got reply from server (%d bytes) \n", lgt ))

	FREE_AND_SET_TO_NULL(vfp);
	FREE_AND_SET_TO_NULL(fcp);
	FILL (  rfp , 0 , sizeof(RFILE) );

	DBG4 (( "fileclose: returning \n" ))
	return ( 0 );
#endif
}


/*==========================================================================*/


global	int     fileread  
(
    char                    * buf ,
    int                     siz ,
    int                     cnt ,
    VFILE                   * vfp 
)
{
	int                     rc=0 ;
	FILE			* fp ;
#ifdef	SOCKETS
	int                     bufsiz , lgt ;
	char                    * bufptr ;
	RFILE			* rfp ;
	struct connection_info  * cip ;
	struct file_cache       * fcp ;
#endif

	DBG2 (( "fileread: called \n" ))

	if ( vfp->type == FT_LOCAL )
	{
		fp = vfp->v_fp.l_fp ;
		rc = fread  ( buf , (CINT) siz , (CINT) cnt , fp );
		return ( rc );
	}

#ifndef SOCKETS
	DBG0 (( "ERROR: remote access not implemented \n" ))
	return ( 0 );
#else
	DBG7 (( "fileread: remote access \n" ))

	rfp = vfp->v_fp.r_fp ;

	if ( ! rfp->in_use )
	{
		DBG0 (( "fileread: file not open \n" ))
		return ( 0 );
	}
	fcp = rfp->fcp ;
	cip = rfp->cip ;


	bufptr = buf ;
	bufsiz = siz * cnt ;

	while ( bufsiz > 0 )
	{
		lgt = bufsiz ;
		if ( lgt > fcp->remain ) lgt = fcp->remain ;
		COPY ( bufptr , fcp->data + fcp->current , lgt );
		bufptr += lgt ;
		bufsiz -= lgt ;
		fcp->remain -= lgt ;
		fcp->current += lgt ;
		DBG6 (( "got %d bytes from cache \n", lgt ))
		if ( bufsiz <= 0 ) break ;

		if ( fcp->eof )
		{
			lgt = ( siz * cnt - bufsiz ) / siz ;
			DBG4 (( "fileread: returning %d \n", lgt ))
			return ( lgt );
		}

		lgt = 0 ;
		remote_msg.type         = REM_READ ;
		remote_msg.reference    = (char) rfp->ref ;
		remote_msg.length [ 0 ] = lgt      & 0xFF ;   /* low byte */
		remote_msg.length [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

		rc = put_message ( cip );
		if ( rc != HEADER_SIZE + lgt )
		{
			DBG0 (( "fileread: request size mismatch %d \n", rc ))
			return ( 0 );
		}
		DBG6 (( "fileread: sent request to server (%d bytes) \n", rc ))

		lgt = get_message ( cip );
		if ( lgt < HEADER_SIZE )
		{
			DBG0 (( "fileread: illegal reply size %d \n", lgt ))
			return ( 0 );
		}
		DBG6 (( "fileread: got reply from server (%d bytes) \n", lgt ))

		if ( remote_msg.type == REM_CLOSE ) fcp->eof = 1 ;

		lgt -= HEADER_SIZE ;
		COPY ( fcp->data , remote_msg.data , lgt );
		fcp->remain = lgt ;
		fcp->current = 0 ;
	}

	DBG4 (( "fileread: returning %d \n", cnt ))
	return ( cnt );
#endif
}


/*==========================================================================*/


global	int     fileseek  
(
    long                    offset ,
    int                     type ,
    VFILE                   * vfp 
)
{
	int                     rc=0 ;
	FILE			* fp ;
#ifdef	SOCKETS
	int                     lgt ;
	RFILE			* rfp ;
	char                    * p ;
	struct connection_info  * cip ;
	struct file_cache       * fcp ;
#endif

	DBG2 (( "fileseek: called \n" ))

	if ( vfp->type == FT_LOCAL )
	{
		fp = vfp->v_fp.l_fp ;
		rc = fseek  ( fp , offset , type );
		return ( rc );
	}

#ifndef SOCKETS
	DBG0 (( "ERROR: remote access not implemented \n" ))
	return ( -1 );
#else
	DBG7 (( "fileseek: remote access \n" ))

	rfp = vfp->v_fp.r_fp ;

	if ( ! rfp->in_use )
	{
		DBG0 (( "fileseek: file not open \n" ))
		return ( 0 );
	}
	fcp = rfp->fcp ;
	cip = rfp->cip ;

	p = remote_msg.data ;
	p [  0 ] = offset       & 0xFF ;
	p [  1 ] = offset >>  8 & 0xFF ;
	p [  2 ] = offset >> 16 & 0xFF ;
	p [  3 ] = offset >> 24 & 0xFF ;
	p [  4 ] = type         & 0xFF ;

	lgt = 5 ;
	remote_msg.type         = REM_SEEK ;
	remote_msg.reference    = (char) rfp->ref ;
	remote_msg.length [ 0 ] = lgt      & 0xFF ;   /* low byte */
	remote_msg.length [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( cip );
	if ( rc != HEADER_SIZE + lgt )
	{
		DBG0 (( "fileseek: request length mismatch %d \n", rc ))
		return ( -1 );
	}
	DBG6 (( "fileseek: sent request to server (%d bytes) \n", rc ))

	lgt = get_message ( cip );
	if ( lgt < HEADER_SIZE + 2 )
	{
		DBG0 (( "fileseek: illegal reply size %d \n", lgt ))
		return ( -1 );
	}
	DBG6 (( "fileseek: got reply from server (%d bytes) \n", lgt ))

	lgt -= HEADER_SIZE + 2 ;
	fcp->remain = lgt ;
	fcp->current = 0 ;
	COPY ( fcp->data , remote_msg.data + 2 , lgt );

	p = remote_msg.data ;
	rc =    p [  0 ]       & 0x00FF |
		p [  1 ] <<  8 & 0xFF00 ;

	DBG4 (( "fileseek: returning %d \n", rc ))
	return ( rc );
#endif
}


/*==========================================================================*/


global	char    * filegets  
(
    char    * buf ,
    int     siz ,
    VFILE   * vfp
)
{
	char                    * p ;
	FILE			        * fp ;
#ifdef	SOCKETS
	int                     rc=0 ;
	int                     c , bufsiz , lgt ;
	char                    * bufptr ;
	RFILE			        * rfp ;
	struct connection_info  * cip ;
	struct file_cache       * fcp ;
#endif

	DBG2 (( "filegets: called \n" ))

	if ( vfp->type == FT_LOCAL )
	{
		fp = vfp->v_fp.l_fp ;
		p = fgets  ( buf , siz , fp );
		return ( p );
	}

#ifndef SOCKETS
	DBG0 (( "ERROR: remote access not implemented \n" ))
	return ( NULL );
#else
	DBG7 (( "filegets: remote access \n" ))

	rfp = vfp->v_fp.r_fp ;

	if ( ! rfp->in_use )
	{
		DBG0 (( "filegets: file not open \n" ))
		return ( 0 );
	}
	fcp = rfp->fcp ;
	cip = rfp->cip ;

	bufptr = buf ;
	bufsiz = siz - 1 ; /* save space for null byte */

	while ( bufsiz > 0 )
	{
		for ( c = -1 ;
		      bufsiz > 0 && fcp->remain > 0 && c != '\n' ;
		      bufsiz -- )
		{
			c = fcp->data [ fcp->current ++ ] ;
			fcp->remain -- ;
			if ( c == '\0' ) break ;
			* ( bufptr ++ ) = (char) c ;
		}
		if ( c == '\n' || c == '\0' ) break ;
		if ( bufsiz <= 0 ) break ;
		if ( fcp->eof ) break ;

		lgt = 0 ;
		remote_msg.type         = REM_READ ;
		remote_msg.reference    = (char) rfp->ref ;
		remote_msg.length [ 0 ] = lgt      & 0xFF ;   /* low byte */
		remote_msg.length [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

		rc = put_message ( cip );
		if ( rc != HEADER_SIZE + lgt )
		{
		    DBG0 (( "filegets: request length mismatch %d \n", rc ))
		    return ( NULL );
		}
		DBG6 (( "filegets: sent request to server (%d bytes) \n", rc ))

		lgt = get_message ( cip );
		if ( lgt < HEADER_SIZE )
		{
			DBG0 (( "filegets: illegal reply size %d \n", lgt ))
			return ( NULL );
		}
		DBG6 (( "filegets: got reply from server (%d bytes) \n", lgt ))

		if ( remote_msg.type == REM_CLOSE ) fcp->eof = 1 ;

		lgt -= HEADER_SIZE ;
		COPY ( fcp->data , remote_msg.data , lgt );
		fcp->remain = lgt ;
		fcp->current = 0 ;
	}

	* bufptr = '\0' ; /* must  always be null terminated */

	if ( bufptr == buf && fcp->eof )
	{
		DBG4 (( "filegets: eof \n" ))
		return ( NULL );
	}
	else
	{
		DBG4 (( "filegets: returning \n" ))
		return ( buf );
	}
#endif
}


/*==========================================================================*/


global	VFILE   * diropen 
(
    char    * path 
)
{
	VFILE                   * vfp ;
	char                    * fname ;
#ifdef  NODIR
	FILE                    * fp ;
#else
	DIR                     * fp ;
#endif
#ifdef	SOCKETS
	int                     fno , lgt , rc=0 ;
	RFILE			* rfp ;
	struct connection_info  * cip ;
	struct file_cache       * fcp ;
	char                    node [ NAMESIZE ] ;
#endif
#ifdef  NODIR
	char                    line [ LINESIZE ] ;
#endif

	DBG2 (( "diropen: called \n" ))
	DBG3 (( "diropen: path '%s' \n", path ))

	fname = strchr ( path , NODE_SEPARATOR );
	if ( ! fname || ((fname - path) < 2) ) /* C: is drive name */
	{
#ifndef NODIR
		fp = opendir ( path ); 
#else
		(void) sprintf ( line , "ls %s", path );
		fp = popen ( line , "r" );
#endif
		if ( ! fp ) return ( (VFILE*) 0 );
		vfp = (VFILE*) palloc ( sizeof(VFILE) );
		vfp->type = FT_LOCAL ;
		vfp->v_fp.l_fp = (FILE*) fp ;
		return ( vfp );
	}

#ifndef SOCKETS
	DBG0 (( "ERROR: remote access not implemented '%s' \n", path ))
	return ( NULL );
#else
	DBG7 (( "diropen: remote access \n" ))

	/* find empty slot in remote files list */

	for ( fno = 0 ; fno < MAXREMOTEFILES ; fno ++ )
	{
		if ( remote_file [ fno ].in_use == 0 ) break ;
	}
	if ( fno >= MAXREMOTEFILES )
	{
	    DBG0 (( "diropen: no more remote files. in use are %d \n", fno ))
	    return ( NULL );
	}
	DBG6 (( "diropen: found free file no. %d \n", fno ))

	rfp = & remote_file [ fno ] ;

	FILL ( node , 0 , sizeof(node) );
	lgt = fname - path ;
	if ( lgt >= sizeof(node) )
	{
		DBG0 (( "diropen: node name too long: '%s' \n", path ))
		return ( NULL );
	}
	COPY ( node , path , lgt );
	DBG6 (( "diropen: node  '%s' \n", node  ))

	fname ++ ;
	DBG6 (( "diropen: fname '%s' \n", fname ))

	/* find existing or open new connection to server */

	rc = open_connection ( node , & cip );
	if ( rc )
	{
		DBG0 (( "diropen: cannot access '%s' \n", path ))
		return ( NULL );
	}
	DBG7 (( "diropen: got connection \n" ))

	/* allocate file cache */

	fcp = (struct file_cache *) palloc ( sizeof(struct file_cache) );
	if ( fcp == NULL )
	{
		DBG0 (( "insufficient dynamic memory \n" ))
		return ( NULL );
	}
	FILL ( fcp , 0 , sizeof(struct file_cache) );
	DBG7 (( "diropen: got file cache \n" ))

	/* issue open request */

	lgt    = strlen ( fname ) + 1 ; /* include null character ('\0') */
	if ( lgt > MESSAGESIZE )
	{
		DBG0 (( "diropen: file name too long: '%s' \n", fname ))
		FREE_AND_SET_TO_NULL(fcp);
		return ( NULL );
	}
	DBG6 (( "diropen: fname length %d \n", lgt    ))

	COPY (  remote_msg.data , fname , lgt    );

	remote_msg.type         = REM_DIROPEN ;
	remote_msg.reference    = (char) fno ;
	remote_msg.length [ 0 ] = lgt      & 0xFF ;   /* low byte */
	remote_msg.length [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( cip );
	if ( rc != HEADER_SIZE + lgt )
	{
		DBG0 (( "diropen: request length mismatch %d \n", rc ))
		FREE_AND_SET_TO_NULL(fcp);
		return ( NULL );
	}
	DBG6 (( "diropen: sent request to server (%d bytes) \n", rc ))

	lgt = get_message ( cip );
	if ( lgt < HEADER_SIZE )
	{
		DBG0 (( "diropen: illegal reply size %d \n", lgt ))
		FREE_AND_SET_TO_NULL(fcp);
		return ( NULL );
	}
	DBG6 (( "diropen: got reply from server (%d bytes) \n", lgt ))

	if ( remote_msg.reference != (char) fno )
	{
		DBG4 (( "diropen: cannot open '%s' \n", path ))
		FREE_AND_SET_TO_NULL(fcp);
		return ( NULL );
	}

	lgt -= HEADER_SIZE ;
	if ( lgt > sizeof(fcp->data) )
	{
		DBG0 (( "diropen: illegal data size %d \n", lgt ))
		FREE_AND_SET_TO_NULL(fcp);
		return ( NULL );
	}
	DBG6 (( "diropen: got data from server (%d bytes) \n", lgt ))

	COPY ( fcp->data , remote_msg.data , lgt );
	fcp->remain = lgt ;
	fcp->current = 0 ;
	if ( remote_msg.type == REM_DIRCLOSE ) fcp->eof = 1 ;
	DBG4 (( "diropen: cache pos %d, length %d, eof %d \n",
			fcp->current , fcp->remain , fcp->eof ))

	rfp->fcp    = fcp ;
	rfp->cip    = cip ;
	rfp->ref    = fno ;
	rfp->in_use = 1 ;

	vfp = (VFILE*) palloc ( sizeof(VFILE) );
	vfp->type = FT_REMOTE ;
	vfp->v_fp.r_fp = rfp ;

	DBG4 (( "diropen: opened '%s' \n", path ))
	return ( vfp );
#endif
}


/*==========================================================================*/


global	int     dirclose 
(
    VFILE                   * vfp 
)
{
	int                     rc=0 ;
#ifdef	NODIR
	FILE			* fp ;
#else
	DIR 			* fp ;
#endif
#ifdef	SOCKETS
	int                     lgt ;
	RFILE			* rfp ;
	struct connection_info  * cip ;
	struct file_cache       * fcp ;
#endif

	DBG2 (( "dirclose: called \n" ))

	if ( vfp->type == FT_LOCAL )
	{
#ifndef NODIR
		fp = (DIR*) vfp->v_fp.l_fp ;
		rc = 0 ; (void) closedir ( fp ); /* T31 defines void closedir!*/
#else
		fp = vfp->v_fp.l_fp ;
		rc = pclose ( fp );
#endif
		if ( ! rc ) FREE_AND_SET_TO_NULL(vfp);
		return ( rc );
	}

#ifndef SOCKETS
	DBG0 (( "ERROR: remote access not implemented \n" ))
	return ( EOF );
#else
	DBG7 (( "dirclose: remote access \n" ))

	rfp = vfp->v_fp.r_fp ;

	if ( ! rfp->in_use )
	{
		DBG0 (( "fileclose: file not open \n" ))
		return ( EOF );
	}
	fcp = rfp->fcp ;

	if ( fcp->eof )
	{
		/* already closed by server */
		FREE_AND_SET_TO_NULL(fcp);
		FILL (  rfp , 0 , sizeof(RFILE) );
		return ( 0 );
	}

	cip = rfp->cip ;

	lgt = 0 ;
	remote_msg.type         = REM_DIRCLOSE ;
	remote_msg.reference    = (char) rfp->ref ;
	remote_msg.length [ 0 ] = lgt      & 0xFF ;   /* low byte */
	remote_msg.length [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	rc = put_message ( cip );
	if ( rc != HEADER_SIZE )
	{
		DBG0 (( "dirclose: request length mismatch %d \n", rc ))
		FREE_AND_SET_TO_NULL(fcp);
		FILL (  rfp , 0 , sizeof(RFILE) );
		return ( EOF );
	}
	DBG6 (( "dirclose: sent request to server (%d bytes) \n", rc ))

	lgt = get_message ( cip );
	if ( lgt != HEADER_SIZE )
	{
		DBG0 (( "dirclose: illegal reply size %d \n", lgt ))
		FREE_AND_SET_TO_NULL(fcp);
		FILL (  rfp , 0 , sizeof(RFILE) );
		return ( EOF );
	}
	DBG6 (( "dirclose: got reply from server (%d bytes) \n", lgt ))

	FREE_AND_SET_TO_NULL(vfp);
	FREE_AND_SET_TO_NULL(fcp);
	FILL (  rfp , 0 , sizeof(RFILE) );

	DBG4 (( "dirclose: returning \n" ))
	return ( 0 );
#endif
}


/*==========================================================================*/


global	int     dirread  
(
    char                    * fname ,
    VFILE                   * vfp  
	/* (union between remote File and normal file) + filetype (vmaketypes.h) */
)
{
#ifdef	NODIR
	FILE			* fp ;
#else
	DIR 			* fp ;
#endif
#ifdef	SOCKETS
	int                     c , bufsiz , lgt , rc=0 ;
	char                    * bufptr ;
	RFILE			* rfp ;
	struct connection_info  * cip ;
	struct file_cache       * fcp ;
#endif
#ifndef	NODIR
	int			idx ;
	char			chr ;
	struct dirent           * dp ;
#else
#ifndef SOCKETS
	int                     lgt ;
#endif
	char			line [ LINESIZE ] ;
#endif

	DBG2 (( "dirread: called \n" ))
	DBG3 (( "dirread: file '%s' \n", fname ))

	FILL ( fname , 0 , NAMESIZE );

	if ( vfp->type == FT_LOCAL )
	{
#ifndef NODIR
	    fp = (DIR*) vfp->v_fp.l_fp ;
	    while ( (dp = readdir ( fp )) )
	    {
		if ( !( isalnum(dp->d_name[0]) || (dp->d_name[0] == '_') ||
			 ( dp->d_name[0] == '.' && isalnum(dp->d_name[1]) ) 
			  )
			)
		    continue ;

		for ( idx = 0 ; idx < MAXFNMLGT ; idx ++ )
		{
		    chr = dp->d_name[idx];
		    if ( chr == '\0' ) break ;
		    fname[idx] = chr ;
		}
		fname[idx] = '\0' ;

		DBG4 (( "dirread: returning '%s' \n", fname ))
		return ( 1 );
	    }
#else
	    fp = vfp->v_fp.l_fp ;
	    while ( fgets  ( line , sizeof(line) , fp ) != NULL )
	    {
		if ( !isalnum(line[0]) && (line[0] != '_') ) continue ;

		lgt = strlen ( line );
		if ( line [ lgt - 1 ] == '\n' ) lgt -- ;
		line [ lgt ] = '\0' ;
		(void) strncpy ( fname , line , MAXFNMLGT );

		DBG4 (( "dirread: returning '%s' \n", fname ))
		return ( 1 );
	    }
#endif
	    DBG4 (( "dirread: returning EOF \n" ))
	    return ( 0 );
	}

#ifndef SOCKETS
	DBG0 (( "ERROR: remote access not implemented \n" ))
	return ( 0 );
#else
	DBG7 (( "dirread: remote access \n" ))

	rfp = vfp->v_fp.r_fp ;

	if ( ! rfp->in_use )
	{
		DBG0 (( "dirread: file not open \n" ))
		return ( 0 );
	}
	fcp = rfp->fcp ;
	cip = rfp->cip ;
	DBG4 (( "dirread: cache pos %d, length %d \n",
			fcp->current , fcp->remain ))

	bufptr = fname ;
	bufsiz = MAXFNMLGT ;

	while ( bufsiz > 0 )
	{
		/*
		    Read the next filename until buffer empty or
		    null byte read.
		    If filename longer than MAXFNMLGT, rest is skipped.
		*/
		c = '~' ; /* not '\0' */
		for ( ; fcp->remain > 0 ; )
		{
			c = fcp->data [ fcp->current ++ ] ;
			fcp->remain -- ;
			if ( c == '\0' ) break ;
			if ( bufsiz > 0 )
			{
				* ( bufptr ++ ) = (char) c ;
				bufsiz -- ;
			}
		}
		DBG4 (( "dirread: fname '%.16s', bufsiz %d, c 0x%02x \n",
				fname , bufsiz , c & 0xff ))
		DBG4 (( "dirread: cache pos %d, length %d, eof=%d \n",
				fcp->current , fcp->remain , fcp->eof ))
		if ( c == '\0' ) break ;
		if ( bufsiz <= 0 ) break ;
		if ( fcp->eof ) break ;

		lgt = 0 ;
		remote_msg.type         = REM_DIRREAD ;
		remote_msg.reference    = (char) rfp->ref ;
		remote_msg.length [ 0 ] = lgt      & 0xFF ;   /* low byte */
		remote_msg.length [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

		rc = put_message ( cip );
		if ( rc != HEADER_SIZE )
		{
			DBG0 (( "dirread: request length mismatch %d \n", rc ))
			return ( 0 );
		}
		DBG6 (( "dirread: sent request to server (%d bytes) \n", rc ))

		lgt = get_message ( cip );
		if ( lgt < HEADER_SIZE )
		{
			DBG0 (( "dirread: illegal reply size %d \n", lgt ))
			return ( 0 );
		}
		DBG6 (( "dirread: got reply from server (%d bytes) \n", lgt ))

		if ( remote_msg.type == REM_DIRCLOSE ) fcp->eof = 1 ;

		lgt -= HEADER_SIZE ;
		COPY ( fcp->data , remote_msg.data , lgt );
		fcp->remain = lgt ;
		fcp->current = 0 ;
	}

	if ( bufptr == fname && fcp->eof )
	{
		DBG4 (( "dirread: eof \n" ))
		return ( 0 );
	}
	else
	{
		DBG4 (( "dirread: returning %d \n", 1 ))
		return ( 1 );
	}
#endif
}


/*==========================================================================*/


/*ARGSUSED*/	/* filetype unused in unix */

global	int     fileincopy
(
    char    * src ,
    char    * dst , /* local path */
    int		filetype 
)
{
	int                     rc=0 ;
	int			msgtype ;
	char                    * fname ;
#ifdef	SOCKETS
	int                     lgt ;
	struct connection_info  * cip ;
	FILE                    * fp ;
	char                    node [ NAMESIZE ] ;
#endif
	struct stat		statbuf ;

	DBG2 (( "incopy: called \n" ))
	DBG3 (( "incopy: src     '%s' \n", src     ))
	DBG3 (( "incopy: dst     '%s' \n", dst     ))

	fname = strchr ( src , NODE_SEPARATOR );
	if ( ! fname || ((fname - src) < 2) ) /* C: is drive name */
	{
		/*
		 *  Get the original date.
		 */
		rc = stat ( src , & statbuf );
		if ( rc < 0 )
		{
		    DBG0 (( "cannot stat '%s': %s \n", src , serrno() ))
		    return ( -2 );
		}

		rc = copy_file ( src , dst , &statbuf );
		return ( rc );
	}

#ifndef SOCKETS
	DBG0 (( "incopy: remote access not implemented '%s' \n", src ))
	return ( -1 );
#else
	DBG7 (( "incopy: remote access \n" ))

	FILL ( node , 0 , sizeof(node) );
	lgt = fname - src ;
	if ( lgt >= sizeof(node) )
	{
		DBG0 (( "incopy: node name too long: '%s' \n", src ))
		return ( -1 );
	}
	COPY ( node , src , lgt );
	DBG6 (( "incopy: node  '%s' \n", node  ))

	fname ++ ;
	DBG6 (( "incopy: fname '%s' \n", fname ))

	/* find existing or open new connection to server */

	rc = open_connection ( node , & cip );
	if ( rc )
	{
		DBG0 (( "incopy: cannot access '%s' \n", node ))
		return ( -1 );
	}
	DBG7 (( "incopy: got connection \n" ))

	lgt = strlen ( fname ) + 1 ; /* include null character ('\0') */
	if ( lgt > MESSAGESIZE )
	{
		DBG0 (( "incopy: file name too long: '%s' \n", fname ))
		return ( -1 );
	}
	DBG6 (( "incopy: fname length %d \n", lgt ))

	COPY (  remote_msg.data , fname , lgt );

	msgtype = (filetype == FT_BIN) ? REM_INCOPY_BIN : REM_INCOPY ;
	remote_msg.type         = (char) msgtype ;
	remote_msg.reference    = (char) 0 ;
	remote_msg.length [ 0 ] = lgt      & 0xFF ;   /* low  byte */
	remote_msg.length [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

	/* open destination file */

#if unix
	fp = fopen ( dst , "w" );
#else
	fp = fopen ( dst , (filetype == FT_BIN) ? "wb" : "w" );
#endif
	if ( fp == NULL )
	{
		DBG0 (( "incopy: cannot create '%s': %s \n", dst , serrno() ))
		return ( -1 );
	}
	DBG6 (( "incopy: opened '%s' \n", dst ))

	/* issue incopy request */

	rc = put_message ( cip );
	if ( rc != HEADER_SIZE + lgt )
	{
		DBG0 (( "incopy: request length mismatch %d \n", rc ))
		(void) fclose ( fp );
		(void) unlink ( dst );
		if ( rc >= 0 ) seterrno(EIO) ;
		return ( -1 );
	}
	DBG6 (( "incopy: sent request to server (%d bytes) \n", rc ))

	lgt = get_message ( cip );
	if ( lgt < HEADER_SIZE )
	{
		DBG0 (( "incopy: illegal reply size %d \n", lgt ))
		(void) fclose ( fp );
		(void) unlink ( dst );
		if ( lgt >= 0 ) seterrno(EIO) ;
		return ( -1 );
	}
	DBG6 (( "incopy: got reply from server (%d bytes) \n", lgt ))

	if ( remote_msg.reference != (char) 0 )
	{
		DBG0 (( "incopy: cannot open '%s' \n", src ))
		(void) fclose ( fp );
		(void) unlink ( dst );
		seterrno(EIO) ;
		return ( -1 );
	}

	for ( ; ; )
	{
		lgt -= HEADER_SIZE ;
		if ( lgt == 0 ) break ;

		rc = fwrite ( remote_msg.data , 1 , (CINT) lgt , fp );
		if ( rc != lgt )
		{
			(void) fclose ( fp );
			(void) unlink ( dst );
			/* PTS 1108168 */
			if ( geterrno() == ENOSPC ) 
			{
					DBG0 (( "incopy: No space left on device\n" ))
					vmake_term ();
			}
			else
			{
				DBG0 (( "incopy: file write error: %s\n", serrno() ))
				close_connections();
			}
			
			return ( -1 );
		}

		if ( remote_msg.type != msgtype ) break ;

		lgt = get_message ( cip );
		if ( lgt < HEADER_SIZE )
		{
			DBG0 (( "incopy: illegal reply size %d \n", lgt ))
			(void) fclose ( fp );
			(void) unlink ( dst );
			return ( -1 );
		}
		DBG6 (( "incopy: got reply from server (%d bytes) \n", lgt ))

	}

	(void) fclose ( fp );

	DBG4 (( "incopy: copied '%s' to '%s' \n", src , dst ))
	return ( 0 );
#endif
}

/*==========================================================================*/


/*ARGSUSED*/	/* filetype unused in unix */

global	int     fileoutcopy
(
    char    * src , /* local path */
    char    * dst ,
    int		filetype 
)
{
	int                     rc=0 ;
	int			            msgtype ;
	char                    * fname , * p;
#ifdef	SOCKETS
	int                     lgt ;
	struct connection_info  * cip ;
	FILE                    * fp ;
	char                    node [ NAMESIZE ] ;
#endif
	struct stat		        statbuf ;

	DBG2 (( "outcopy: called \n" ))
	DBG3 (( "outcopy: src      '%s' \n", src      ))
	DBG3 (( "outcopy: dst      '%s' \n", dst      ))
	DBG3 (( "outcopy: filetype '%d' \n", filetype ))

	fname = strchr ( dst , NODE_SEPARATOR );
	if ( ! fname || ((fname - dst) < 2) ) /* C: is drive name */
	{
		/*
		 *  Get the original date.
		 */
		rc = stat ( src , & statbuf );
		if ( rc < 0 )
		{
		    DBG0 (( "cannot stat '%s': %s \n", src , serrno() ))
		    return ( -2 );
		}

		rc = copy_file ( src , dst , &statbuf );
		return ( rc );
	}

#ifndef SOCKETS
	DBG0 (( "outcopy: remote access not implemented '%s' \n", src ))
	return ( -1 );
#else
	DBG7 (( "outcopy: remote access \n" ))

	FILL ( node , 0 , sizeof(node) );
	lgt = fname - dst ;
	if ( lgt >= sizeof(node) )
	{
		DBG0 (( "outcopy: node name too long: '%s' \n", src ))
		return ( -1 );
	}
	COPY ( node , dst , lgt );
	DBG6 (( "outcopy: node  '%s' \n", node  ))

	fname ++ ;
	DBG6 (( "outcopy: fname '%s' \n", fname ))

	rc = stat ( src , & statbuf );
	if ( rc < 0 )
	{
		DBG0 (( "cannot stat '%s': %s \n", src , serrno() ))
		return ( -2 );
	}

	/* find existing or open new connection to server */

	rc = open_connection ( node , & cip );
	if ( rc )
	{
		DBG0 (( "outcopy: cannot access '%s' \n", node ))
		return ( -1 );
	}
	DBG7 (( "outcopy: got connection \n" ))

	lgt = strlen ( fname ) + 1 ; /* include null character ('\0') */
	if ( lgt + 4 > MESSAGESIZE ) /* + 4 bytes for following long integer */
	{
		DBG0 (( "outcopy: file name too long: '%s' \n", fname ))
		return ( -1 );
	}
	DBG6 (( "outcopy: fname length %d \n", lgt ))

	COPY (  remote_msg.data , fname , lgt );

	p = remote_msg.data + strlen( fname ) + 1;
	p [ 0 ] = statbuf.st_mtime       & 0xFF ;
	p [ 1 ] = statbuf.st_mtime >>  8 & 0xFF ;
	p [ 2 ] = statbuf.st_mtime >> 16 & 0xFF ;
	p [ 3 ] = statbuf.st_mtime >> 24 & 0xFF ;
    lgt += 4; /* + 4 bytes for long integer */

	msgtype = (filetype == FT_BIN) ? REM_OUTCOPY_BIN : REM_OUTCOPY ;
	remote_msg.type         = (char) msgtype ;
	remote_msg.reference    = (char) 0 ;
	remote_msg.length [ 0 ] = lgt      & 0xFF ;   /* low  byte */
	remote_msg.length [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

    /* open source file */

#if unix
	fp = fopen ( src , "r" );
#else
	fp = fopen ( src , (filetype == FT_BIN) ? "rb" : "r" );
#endif
	if ( fp == NULL )
	{
		DBG0 (( "outcopy: cannot read '%s': %s \n", src , serrno() ))
		return ( -1 );
	}
	DBG7 (( "outcopy: opened '%s' \n", src ))

	/* issue outcopy request */
	rc = put_message ( cip );
	if ( rc != HEADER_SIZE + lgt )
	{
		DBG0 (( "outcopy: request length mismatch %d \n", rc ))
		(void) fclose ( fp );
		if ( rc >= 0 ) seterrno(EIO) ;
		return ( -1 );
	}
	DBG6 (( "outcopy: sent request to server (%d bytes) \n", rc ))

	lgt = get_message ( cip );
	if ( lgt < HEADER_SIZE )
	{
		DBG0 (( "outcopy: illegal reply size %d \n", lgt ))
		(void) fclose ( fp );
		if ( lgt >= 0 ) seterrno(EIO) ;
		return ( -1 );
	}
	DBG6 (( "outcopy: got reply from server (%d bytes) \n", lgt ))

	if ( remote_msg.reference != (char) 0 )
	{
		DBG0 (( "outcopy: cannot open '%s' \n", dst ))
		(void) fclose ( fp );
		seterrno(EIO) ;
		return ( -1 );
	}

    for ( ; ; )
	{
		for ( lgt = 0 ; lgt < MESSAGESIZE ; lgt += rc )
		{
			rc = fread ( (remote_msg.data + lgt) , 1 ,
					MESSAGESIZE - (CINT) lgt , fp );
			if ( rc == 0 )
			{
				DBG7 (( "outcopy: reached eof \n" ))
				remote_msg.type = 0 ;
				break ;
			}
		}

		p = (char *) remote_msg.length ;
		p [ 0 ] = lgt      & 0xFF ;   /* low  byte */
		p [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

		rc = put_message ( cip );
		DBG6 (( "outcopy: sent reply (%d bytes) \n", rc ))

		if ( remote_msg.type == 0 ) break ;
	}

	lgt = get_message ( cip );
	if ( lgt < HEADER_SIZE )
	{
		DBG0 (( "outcopy: illegal reply size %d \n", lgt ))
		(void) fclose ( fp );
		if ( lgt >= 0 ) seterrno(EIO) ;
		return ( -1 );
	}
	DBG6 (( "outcopy: got reply from server (%d bytes) \n", lgt ))

    if ( remote_msg.reference  == (char) 255 )
    {
	    lgt = remote_msg.length [ 0 ] & 0x00FF | remote_msg.length [ 1 ] <<  8 & 0xFF00 ;
        if ( lgt != 0 )
        {
            char    * msg;
            msg = (char*) palloc ( lgt + 1 );
            if ( msg ) 
            {
                strncpy( msg, (char*) remote_msg.data, lgt );
                msg[ lgt ] = '\0';
            }
            /* on error return 2 */
            DBG0 (( "cannot copy file \n"))
            DBG0 (( "message '%s' \n", msg ))
            if ( msg ) FREE_AND_SET_TO_NULL(msg);
    		(void) fclose ( fp );
	        return ( -1 );
        }
    }

	(void) fclose ( fp );
	DBG4 (( "outcopy: returning %d \n", 0 ))
	return ( 0 );
#endif
}

/*==========================================================================*/


#ifdef  SOCKETS
static 	int     open_connection 
(
    char                    * node ,
    struct connection_info  ** cipp 
)
{
	int                             lgt , sd , rc=0 ;
	time_t                          my_time , srv_time , diff ;
	char                            * p ;
	struct hostent                  * hp ;
	struct servent                  * sp ;
	struct sockaddr                 sa ;
	short                           port ;
	struct connection_info          * cip ;


	for ( cip = connections ; cip != NULL ; cip = cip->next )
	{
		if ( strcmp ( cip->node , node ) == 0 ) break ;
	}
	if ( cip != NULL )
	{
		DBG6 (( "node already connected '%s' \n", node ))
		* cipp = cip ;
		return ( 0 );
	}

	cip = (struct connection_info *)
		palloc ( sizeof(struct connection_info) );
	if ( cip == NULL )
	{
		DBG0 (( "isufficient dynamic memory \n" ))
		return ( ENOMEM );
	}
	FILL ( cip , 0 , sizeof(struct connection_info) );

    cip->node = (char *) palloc( strlen( node ) + 1 );
    (void) strcpy( cip->node, node );

#if WIN32
	if ( ! wsa_started )
	{
	    WSADATA	x;
	    WSAStartup ( 0x101 , &x );
	    wsa_started = TRUE ;
	}
#endif

	seterrno ( 0 );
	sp = getservbyname ( SOCKET_SERVICE, SOCKET_PROTOCOL );
	if ( sp == NULL )
	{
	    /* try a second time */
	    seterrno ( 0 );
	    sp = getservbyname ( SOCKET_SERVICE, SOCKET_PROTOCOL );
	    if ( sp == NULL )
	    {
		DBG0 (( "get service by name error: %s \n", serrno() ))
		DBG0 (( "check /etc/services: %s   7792/%s \n",
				SOCKET_SERVICE, SOCKET_PROTOCOL ))
		FREE_AND_SET_TO_NULL(cip);
		vmake_term ();
		if ( geterrno() == 0 ) seterrno(EINVAL) ;
		return ( -2 );
	    }
	}
	DBG6 (( "got service by name \n" ))

	seterrno ( 0 );
	hp = gethostbyname ( node );
	if ( hp == NULL )
	{
	    /* try a second time */
	    seterrno ( 0 );
	    hp = gethostbyname ( node );
	    if ( hp == NULL )
	    {
		DBG0 (( "get host by name error: %s \n", serrno() ))
		DBG0 (( "check /etc/hosts: nn.nn.nn.nn   %s \n", node ))
		FREE_AND_SET_TO_NULL(cip);
		vmake_term ();
		if ( geterrno() == 0 ) seterrno(EINVAL) ;
		return ( -2 );
	    }
	}
	DBG6 (( "got host by name \n" ))

	sd = socket ( AF_INET, SOCK_STREAM, 0 );
	if ( sd < 0 )
	{
	    DBG0 (( "socket open error: %s \n", serrno() ))
	    FREE_AND_SET_TO_NULL(cip);
	    vmake_term ();
	    return ( -2 );
	}
	cip->sd = sd ;
	DBG6 (( "opened socket %d \n", sd ))

	sa.sa_family = hp->h_addrtype;
	p = (char*)sa.sa_data;
	FILL ( p, 0, sizeof(sa.sa_data) );
	port = sp->s_port;
	COPY ( p,  &port, 2 );
	p += 2;
	lgt = hp->h_length;
	COPY ( p, hp->h_addr, lgt );
	p += lgt;

	rc = connect ( sd, &sa, sizeof(sa) );
	if ( rc )
	{
	    if ( geterrno() == EADDRNOTAVAIL )
	    {
		    DBG0 (( "server not reachable \n" ))
	    }
	    else    DBG0 (( "connect error: %s \n", serrno() ))
	    DBG0 (( "error during connect to '%s' \n", node ))
	    (void) close ( sd );
	    FREE_AND_SET_TO_NULL(cip);
	    vmake_term ();
	    return ( -2 );
	}
	DBG6 (( "connected to '%s' \n", node ))

	lgt = 1 ;
	rc = setsockopt ( sd , SOL_SOCKET , SO_KEEPALIVE ,
				(char*) & lgt , sizeof(lgt) );
	if ( rc < 0 )
	{
	    DBG0 (( "WARNING: setsockopt (KEEPALIVE) error: %s \n", serrno() ))
	}

	remote_msg.type         = REM_TIME ;
	remote_msg.reference    = (char) 0 ;
	remote_msg.length [ 0 ] = 0 ;         /* low byte */
	remote_msg.length [ 1 ] = 0 ;         /* high byte */

	rc = put_message ( cip );
	if ( rc < HEADER_SIZE )
	{
	    (void) close ( sd );
	    FREE_AND_SET_TO_NULL(cip);
	    return ( rc < 0 ? -rc : EIO );
	}
	DBG6 (( "sent time request  to  server (%d bytes) \n", rc ))

	lgt = get_message ( cip );
	if ( lgt <= 0 )
	{
	    (void) close ( sd );
	    FREE_AND_SET_TO_NULL(cip);
	    return ( lgt < 0 ? -lgt : EIO );
	}
	DBG6 (( "got  time message from server (%d bytes) \n", rc ))

	my_time = time ( (time_t *) 0 );

	if ( lgt - HEADER_SIZE < 4 )
	{
	    DBG0 (( "illegal time reply size %d \n", lgt ))
	    (void) close ( sd );
	    FREE_AND_SET_TO_NULL(cip);
	    vmake_term ();
	    return ( EIO );
	}
	p = (char*) remote_msg.data ;
	srv_time = p [ 0 ]       & 0xFF     | 
               p [ 1 ] <<  8 & 0xFF00   |
		       p [ 2 ] << 16 & 0xFF0000 | 
               p [ 3 ] << 24 & 0xFF000000 ;

	diff = my_time - srv_time ;
	if ( diff < 0 ) diff = -diff ;
	if ( diff <= HYSTERESIS )
	{
	    DBG4 (( "server time is %s \n", ascdate(srv_time) ))
	    DBG4 (( "my     time is %s \n", ascdate(my_time) ))
	}
	else
	{
	    DBG0 (( "server time is %s \n", ascdate(srv_time) ))
	    DBG0 (( "my     time is %s \n", ascdate(my_time) ))
	    DBG0 (( "ERROR: won't use server because of time difference \n" ))
		eprotwrite ( "server time is %s \n", ascdate(srv_time) );
		eprotwrite ( "my     time is %s \n", ascdate(my_time) );
	    eprotwrite ( "ERROR: won't use server because of time difference \n" );
	    (void) close ( sd );
	    FREE_AND_SET_TO_NULL(cip);
	    vmake_term ();
	    return ( EIO );
	}

	cip->next = connections ;
	connections = cip ;
	* cipp = cip ;

	return ( 0 );
}
#endif


/*==========================================================================*/


#ifdef  SOCKETS
static	int     get_message 
(
    struct connection_info  * cip 
)
{
	int                     sd , msglgt , lgt , rc=0 ;


	sd = cip->sd ;
	DBG4 (( "getmsg: called %d \n", sd ))

	lgt = cip->cache_lgt ;
	if ( lgt )
	{
	    COPY ( & remote_msg , (char*) cip->cache , lgt );
	    cip->cache_lgt = 0 ;
	}

	for ( rc=0/*lint*/ ; lgt < HEADER_SIZE ; lgt += rc )
	{
	    rc = recv ( sd , (char*) & remote_msg  + lgt ,
				 sizeof(remote_msg) - lgt , 0 );
	    if ( rc <= 0 )
	    {
		if ( rc < 0 )
		{
		    DBG0 (( "socket recv error: %s \n", serrno() ))
		    remove_connection ( sd );
		    vmake_term ();
		    return ( -2 );
		}
		DBG0 (( "connection closed by server \n" ))
		remove_connection ( sd );
		vmake_term ();
		return ( 0 );
	    }
	    DBG6 (( "getmsg: got %d bytes \n", rc ))
	}
	DBG7 (( "getmsg: header complete (got %d bytes) \n", lgt ))

	msglgt = HEADER_SIZE +
		 (remote_msg.length [ 0 ]      & 0x00FF |
		  remote_msg.length [ 1 ] << 8 & 0xFF00   );
	if ( msglgt < HEADER_SIZE || msglgt > sizeof(remote_msg) )
	{
	    DBG0 (( "illegal message length %d \n", msglgt ))
	    vmake_term ();
	    return ( -EIO );
	}
	DBG7 (( "getmsg: complete message length is %d bytes \n", msglgt ))

	for ( ; lgt < msglgt ; lgt += rc )
	{
	    rc = recv ( sd , (char*) & remote_msg  + lgt ,
				 sizeof(remote_msg) - lgt , 0 );
	    if ( rc <= 0 )
	    {
		if ( rc < 0 )
		{
		    DBG0 (( "socket recv error: %s \n", serrno() ))
		    remove_connection ( sd );
		    vmake_term ();
		    return ( -2 );
		}
		DBG0 (( "connection closed by server \n" ))
		remove_connection ( sd );
		vmake_term ();
		return ( 0 );
	    }
	    DBG6 (( "getmsg: got %d bytes \n", rc ))
	}
	DBG7 (( "getmsg: message complete (got %d bytes) \n", lgt ))

	if ( lgt > msglgt )
	{
	    cip->cache_lgt = lgt - msglgt ;
	    COPY ( cip->cache ,  & remote_msg + msglgt , cip->cache_lgt );
	    lgt -= cip->cache_lgt ;
	}

	DBG4 (( "getmsg: type      %d \n", (int) remote_msg.type ))
	DBG4 (( "getmsg: reference %d \n", (int) remote_msg.reference ))
	DBG4 (( "getmsg: returning %d \n", lgt ))
	return ( lgt );
}
#endif


/*==========================================================================*/


#ifdef  SOCKETS
static	int     put_message 
(
    struct connection_info  * cip 
)
{
	int                     sd , msglgt , rc=0 ;


	sd = cip->sd ;
	DBG4 (( "putmsg: sd        %d \n", sd ))
	DBG4 (( "putmsg: type      %d \n", (int) remote_msg.type ))
	DBG4 (( "putmsg: reference %d \n", (int) remote_msg.reference ))

	msglgt = HEADER_SIZE +
		 (remote_msg.length [ 0 ]      & 0x00FF |
		  remote_msg.length [ 1 ] << 8 & 0xFF00   );
	DBG4 (( "putmsg: length    %d \n", msglgt ))
	if ( msglgt < HEADER_SIZE || msglgt > sizeof(remote_msg) )
	{
	    DBG0 (( "illegal message length %d \n", msglgt ))
	    vmake_term ();
	    return ( -EIO );
	}

	rc = send ( sd , (char*) & remote_msg , (unsigned) msglgt , 0 );
	if ( rc != msglgt )
	{
	    if ( rc < 0 )
	    {
		DBG0 (( "socket send error: %s \n", serrno() ))
	    }
	    else
	    {
		DBG0 (( "socket send size error %d \n", rc ))
	    }
	    remove_connection ( sd );
	    vmake_term ();
	    if ( rc >= 0 ) seterrno(EIO) ;
	    return ( -2 );
	}

	DBG4 (( "putmsg: returning %d \n", rc ))
	return ( rc );
}
#endif


/*==========================================================================*/


void	close_connections 
(
)
{
#ifdef  SOCKETS
	struct connection_info  * cip ;

	while ( connections != NULL )
	{
		cip = connections ;
#ifdef unix
		(void) close ( cip->sd );
#else
		(void) closesocket ( cip->sd );
#endif
		connections = cip->next ;
		FREE_AND_SET_TO_NULL(cip);
	}
#ifdef WIN32
    if ( wsa_started )
        WSACleanup();
#endif
#endif
}


/*==========================================================================*/

#ifdef  SOCKETS
static	void	remove_connection 
(
    int     sd    
)
{
	struct connection_info  * cip , ** cipp ;


	for (   cipp = & connections ;
		(* cipp) != NULL ;
		cipp = & (* cipp)->next )
	{
		if ( (* cipp)->sd == sd ) break ;
	}
	cip = * cipp ;
	if ( ! cip ) return ;
	(void) close ( cip->sd );
	(* cipp) = cip->next ;
	FREE_AND_SET_TO_NULL(cip);
}
#endif

/*==========================================================================*/

#ifdef	WIN32
static	struct dirent		dirent ;

global	DIR	* opendir 
(
    char			* path
)
{
	unsigned	lgt ;
	char		*ptr ;
	DIR         *dirp ;

    DBG2 (( "opendir: called \n" ))
    DBG3 (( "opendir: path '%s' \n", path ))

    lgt = strlen ( path ) + 5;
	if ( ! lgt )
	{
	    seterrno ( ENOENT );
	    return ( (DIR*) 0 );
	}
	dirp = (DIR*) malloc ( sizeof(DIR) + lgt );
	if ( ! dirp )
	{
	    seterrno ( ENOMEM );
	    return ( (DIR*) 0 );
	}
	FILL ( dirp , 0 , sizeof(DIR) + lgt );
	dirp->dir_handle = -1 ;
	ptr = dirp->dir_name ;
	if ( (lgt > 1) || ((lgt == 1) && (*path != '.')) )
	{
        (void) strcpy( dirp->dir_name, path );
        while( ptr = strchr( dirp->dir_name, '\\' ) ) *ptr = '/';
        if ( *(dirp->dir_name+strlen(path)-1) != '/' ) 
            (void)strcat( dirp->dir_name, "/" );
	}
    (void)strcat( dirp->dir_name, "*.*" );
	DBG4 (( "opendir: open '%s' \n", dirp->dir_name ))
	dirp->dir_handle = _findfirst ( dirp->dir_name , &dirp->dir_data );
	if ( dirp->dir_handle == -1 )
	{
	    switch ( geterrno() )
	    {
	    case ENOENT :
		dirp->dir_entryno = -1 ;
		break ;

	    default :
		FREE_AND_SET_TO_NULL(dirp);
		return ( (DIR*) 0 );
	    }
	}
	else
	{
	    dirp->dir_entryno = 0 ;
	}
	return ( dirp );
}

/*==========================================================================*/

global	struct dirent	*readdir 
(
    DIR			*dirp 
)
{
	long			lrc=0 ;
	if ( ! dirp )
	{
	    seterrno ( EBADF );
	    return ( (struct dirent*) 0 );
	}
	if ( dirp->dir_entryno == -1 )
	{
	    seterrno ( ENOENT );
	    return ( (struct dirent*) 0 );
	}
	if ( dirp->dir_entryno != 0 )
	{
	    lrc = _findnext ( dirp->dir_handle , &dirp->dir_data );
	    if ( lrc == -1 )
	    {
		dirp->dir_entryno = -1 ;
		return ( (struct dirent*) 0 );
	    }
	}
        dirp->dir_entryno ++ ;
        DBG4 (( "readdir: dir entry '%s' \n", dirp->dir_data.name ))
        FILL ( &dirent , 0 , sizeof(dirent) );
	dirent.d_reclen = sizeof(dirent);
	dirent.d_off    = sizeof(dirent) * dirp->dir_entryno ;
	(void) strncpy ( dirent.d_name , dirp->dir_data.name ,
					sizeof(dirent.d_name)-1 );
        return ( &dirent );
}

/*==========================================================================*/

global	int	closedir 
(
    DIR			*dirp 
)
{
	if ( ! dirp )
	{
	    seterrno ( EBADF );
	    return ( -1 );
	}
	if ( dirp->dir_handle != -1 )
	{
	    (void) _findclose ( dirp->dir_handle );
	}
	FREE_AND_SET_TO_NULL(dirp);
	return ( 0 );
}
#endif /*WIN32*/

/*==========================================================================*/

global int makedir
(
    char    * node,
    char    * path
)
{
    int     rc=0 , lgt;
	struct connection_info  * cip ;
    char    * msg;

    DBG6 (( "makedir: node '%s' \n", node ))
    DBG6 (( "makedir: path '%s' \n", path ))

    /* find existing or open new connection to server */

#ifndef SOCKETS
	DBG0 (( "makedir: remote access not implemented '%s':'%s' \n", node, path ))
	seterrno(EINVAL) ;
	return ( -1 );
#else

    rc = open_connection ( node , & cip );
    if ( rc )
    {
	    DBG0 (( "check_dir: cannot access '%s' \n", node ))
	    return ( -1 );
    }
    DBG7 (( "makedir: got connection \n" ))

    /* issue stat request */

    lgt = strlen ( path ) + 1 ; /* include null character ('\0') */
    if ( lgt > MESSAGESIZE )
    {
	    DBG0 (( "makedir: path name too long: '%s' \n", path ))
	    seterrno(EINVAL) ;
	    return ( -1 );
    }
    DBG6 (( "makedir: path length %d \n", lgt ))

    COPY (  remote_msg.data , path , lgt );

    remote_msg.type         = REM_MKDIR ;
    remote_msg.reference    = (char) 0 ;
    remote_msg.length [ 0 ] = lgt      & 0xFF ;   /* low  byte */
    remote_msg.length [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

    rc = put_message ( cip );
    if ( rc != HEADER_SIZE + lgt )
    {
	    DBG0 (( "makedir: request length mismatch %d \n", rc ))
	    if ( rc >= 0 ) seterrno(EIO) ;
	    return ( -1 );
    }
    DBG6 (( "makedir: sent request to server (%d bytes) \n", rc ))

    lgt = get_message ( cip );

    DBG6 (( "makedir: got reply from server (%d bytes) \n", lgt ))
	lgt = remote_msg.length [ 0 ] & 0x00FF | remote_msg.length [ 1 ] <<  8 & 0xFF00 ;
    if ( lgt != 0 )
    {
        msg = (char*) palloc ( lgt + 1 );
        if ( msg ) 
        {
            strncpy( msg, (char*) remote_msg.data, lgt );
            msg[ lgt ] = '\0';
        }
        /* on error return 2 */
        DBG0 (( "cannot create remote directory \n"))
        DBG0 (( "message '%s' \n", msg ))
        if ( msg ) FREE_AND_SET_TO_NULL(msg);
	    return ( 2 );
    }
    DBG4 (( "makedir: path '%s' created \n", path ))
    return( 0 );
#endif
}

/*==========================================================================*/

global int renamefile
(
    char    * node,
    char    * file,
    char    * newfile
)
{
    int     rc=0 , lgt, msglgt;
	struct connection_info  * cip ;
    char    * msg;

    DBG6 (( "renamefile: node '%s' \n"   , node ))
    DBG6 (( "renamefile: file '%s' \n"   , file ))
    DBG6 (( "renamefile: newfile '%s' \n", newfile ))

    /* find existing or open new connection to server */

#ifndef SOCKETS
	DBG0 (( "renamefile: remote access not implemented '%s' \n", node ))
	seterrno(EINVAL) ;
	return ( -1 );
#else

    rc = open_connection ( node , & cip );
    if ( rc )
    {
	    DBG0 (( "renamefile: cannot access '%s' \n", node ))
	    return ( -1 );
    }
    DBG7 (( "renamefile: got connection \n" ))

    /* issue stat request */

    msglgt = lgt = strlen ( file ) + 1 ; /* include null character ('\0') */
    if ( lgt > MESSAGESIZE )
    {
	    DBG0 (( "renamefile: file name too long: '%s' \n", file ))
	    seterrno(EINVAL) ;
	    return ( -1 );
    }
    DBG6 (( "renamefile: file length %d \n", lgt ))
    (void) strncpy( remote_msg.data , file , lgt ); 
    msglgt += lgt = strlen ( newfile ) + 1 ; /* include null character ('\0') */
    if ( msglgt > MESSAGESIZE )
    {
	    DBG0 (( "renamefile: newfile name too long: '%s' \n", newfile ))
	    seterrno(EINVAL) ;
	    return ( -1 );
    }
    DBG6 (( "renamefile: new file length %d \n", lgt ))
    (void) strncpy( remote_msg.data + strlen( remote_msg.data ) + 1, 
        newfile , lgt ); 

    remote_msg.type         = REM_RENAME ;
    remote_msg.reference    = (char) 0 ;
    remote_msg.length [ 0 ] = msglgt      & 0xFF ;   /* low  byte */
    remote_msg.length [ 1 ] = msglgt >> 8 & 0xFF ;   /* high byte */

    rc = put_message ( cip );
    if ( rc != HEADER_SIZE + msglgt )
    {
	    DBG0 (( "renamefile: request length mismatch %d \n", rc ))
	    if ( rc >= 0 ) seterrno(EIO) ;
	    return ( -1 );
    }
    DBG6 (( "renamefile: sent request to server (%d bytes) \n", rc ))

    lgt = get_message ( cip );

    DBG6 (( "renamefile: got reply from server (%d bytes) \n", lgt ))
	lgt = remote_msg.length [ 0 ] & 0x00FF | remote_msg.length [ 1 ] <<  8 & 0xFF00 ;
    if ( lgt != 0 )
    {
        msg = (char*) palloc ( lgt + 1 );
        if ( msg ) 
        {
            strncpy( msg, (char*) remote_msg.data, lgt );
            msg[ lgt ] = '\0';
        }
        /* on error return 2 */
        DBG0 (( "renamefile: cannot rename files \n"))
        DBG0 (( "renamefile: message '%s' \n", msg ))
        if ( msg ) FREE_AND_SET_TO_NULL(msg);
	    return ( 2 );
    }
    DBG4 (( "renamefile: file '%s' to '%s' renamed \n", file, newfile ))
    return( 0 );
#endif
}

/*==========================================================================*/

global int unlinkfile
(
    char    * node,
    char    * file
)
{
    int     rc=0 , lgt;
	struct connection_info  * cip ;
    char    * msg;

    DBG6 (( "unlinkfile: node '%s' \n", node ))
    DBG6 (( "unlinkfile: file '%s' \n", file ))

    /* find existing or open new connection to server */

#ifndef SOCKETS
	DBG0 (( "unlinkfile: remote access not implemented '%s':'%s' \n", node, file ))
	seterrno(EINVAL) ;
	return ( -1 );
#else

    rc = open_connection ( node , & cip );
    if ( rc )
    {
	    DBG0 (( "unlinkfile: cannot access '%s' \n", node ))
	    return ( -1 );
    }
    DBG7 (( "unlinkfile: got connection \n" ))

    /* issue stat request */

    lgt = strlen ( file ) + 1 ; /* include null character ('\0') */
    if ( lgt > MESSAGESIZE )
    {
	    DBG0 (( "unlinkfile: file name too long: '%s' \n", file ))
	    seterrno(EINVAL) ;
	    return ( -1 );
    }
    DBG6 (( "unlinkfile: file length %d \n", lgt ))

    COPY (  remote_msg.data , file , lgt );

    remote_msg.type         = REM_UNLINK ;
    remote_msg.reference    = (char) 0 ;
    remote_msg.length [ 0 ] = lgt      & 0xFF ;   /* low  byte */
    remote_msg.length [ 1 ] = lgt >> 8 & 0xFF ;   /* high byte */

    rc = put_message ( cip );
    if ( rc != HEADER_SIZE + lgt )
    {
	    DBG0 (( "unlinkfile: request length mismatch %d \n", rc ))
	    if ( rc >= 0 ) seterrno(EIO) ;
	    return ( -1 );
    }
    DBG6 (( "unlinkfile: sent request to server (%d bytes) \n", rc ))

    lgt = get_message ( cip );

    DBG6 (( "unlinkfile: got reply from server (%d bytes) \n", lgt ))
	lgt = remote_msg.length [ 0 ] & 0x00FF | remote_msg.length [ 1 ] <<  8 & 0xFF00 ;
    if ( lgt != 0 )
    {
        msg = (char*) palloc ( lgt + 1 );
        if ( msg ) 
        {
            strncpy( msg, (char*) remote_msg.data, lgt );
            msg[ lgt ] = '\0';
        }
        /* on error return 2 */
        DBG0 (( "cannot unlink file \n"))
        DBG0 (( "message '%s' \n", msg ))
        if ( msg ) FREE_AND_SET_TO_NULL(msg);
	    return ( 2 );
    }
    DBG4 (( "unlinkfile: file '%s' unlinked \n", file ))
    return( 0 );
#endif
}

/*==========================================================================*/

