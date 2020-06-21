/*	isthdr.h	

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

#ifndef	_GENERAL_HEADERS
#define	_GENERAL_HEADERS

/* generic includes */

#include	<stdlib.h>

#if unix
# include	<unistd.h>
#endif

#if SCO
# include	<prototypes.h>
#endif

#if T31
  extern	char		*getcwd ();
  extern	char		*getenv ();
  extern	char		*getlogin ();
  extern	char		*getpass ();
  extern	char		*sbrk ();
  extern	char		*shmat ();
  extern	char		*malloc ();
  extern	char		*realloc ();
  extern	char		*calloc ();
  extern	char		*ctermid ();
  extern	char		*ctime ();
  extern	char		*asctime ();
  extern	char		*cuserid ();
  extern	char		*tmpnam ();
  extern	char		*tempnam ();
  extern	char		*ttyname ();
#endif

#ifdef	MSDOS
# include	<io.h>		/* open,close,read,write,... */
# include	"istdos.h"
#endif

#ifdef	WIN32
# include	<windows.h>
# include	<windef.h>	/* CONST,DWORD,LPVOID,winnt.h,HANDLE,... */
# include	<stdarg.h>	/* needed by winbase.h */
# include	<winbase.h>	/* GetLastError !!! */
# include	<io.h>		/* open,close,read,write,... */
# include	<direct.h>	/* _chdir,_getcwd,_mkdir,... */
# include	"istwinnt.h"
#else
  typedef	unsigned long		DWORD ;
  typedef	void			*LPVOID ;
#endif

#include        <string.h>
#include        <memory.h>
#include        <sys/types.h>

#endif /*_GENERAL_HEADERS*/

