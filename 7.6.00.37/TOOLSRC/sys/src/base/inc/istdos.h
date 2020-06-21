/*	istdos.h		

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

#ifndef	_DOS_DEFINITIONS
#define	_DOS_DEFINITIONS

#ifdef	MSDOS

#undef		va_start
#undef		va_arg
#undef		va_end

typedef	int			pid_t ;
typedef	int			mode_t ;
typedef	int			uid_t ;
typedef	int			gid_t ;

#define	link(_src_,_dst_)	(rename((_src_),(_dst_)))

#endif	/*MSDOS*/

#endif /*_DOS_DEFINITIONS*/

