/*	istutime.h	

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

#ifndef	_UTIME_HEADERS
#define	_UTIME_HEADERS

#if T31 || T32 || T33 || T35 || MSDOS
# define	STRUCT_UTIMBUF	struct {time_t actime,modtime;}
#else
# ifdef WIN32
#   include	<sys/utime.h>
#   define	STRUCT_UTIMBUF	struct _utimbuf
#   define	utime		_utime
# else
#   include	<utime.h>
#   define	STRUCT_UTIMBUF	struct utimbuf
# endif
#endif

#endif /*_UTIME_HEADERS*/

