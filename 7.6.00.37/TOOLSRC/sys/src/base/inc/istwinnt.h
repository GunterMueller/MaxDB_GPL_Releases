/*	istwinnt.h		

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

#ifndef	_WINNT_DEFINITIONS
#define	_WINNT_DEFINITIONS

#ifdef	WIN32

#undef		va_start
#undef		va_arg
#undef		va_end

typedef	int			pid_t ;
typedef	int			mode_t ;
typedef	short			uid_t ;
typedef	short			gid_t ;

#undef	link
#define	link			rename
#undef	mkdir
#define	mkdir(dir,mode)		_mkdir(dir)
#undef	pclose
#define	pclose			_pclose
#undef	popen
#define	popen			_popen

/* from istwinnt.c */

#undef				pipe
extern	int			pipe ( int* );
#undef				sleep
extern	unsigned		sleep ( unsigned );
extern	char			*strptime ();

#endif	/*WIN32*/

#endif /*_WINNT_DEFINITIONS*/

