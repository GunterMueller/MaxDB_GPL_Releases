/*	ftree.h		

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

#ifndef	_FTREE_INFO
#define	_FTREE_INFO

#if WIN32
#  include	<windows.h>
#  include	<winnt.h>
#else
   typedef	char*	PSECURITY_DESCRIPTOR ;
#endif

struct ftree_control
{
	int			ftc_recursive ;
	int			ftc_readdirs ;
	int			ftc_symlink ;
	int			ftc_dirs_only ;
	unsigned		ftc_count ;
	struct ftree_info	*ftc_first ;
	struct ftree_info	*ftc_last ;
};
typedef	struct ftree_control	ftree_control ;

struct ftree_info
{
	struct ftree_info	*fti_next ;
	struct ftree_info	*fti_dir_first ;
	struct ftree_info	*fti_dir_last ;
	struct stat		fti_stat ;
	PSECURITY_DESCRIPTOR	fti_secp ;
	char			fti_name [ 1 ];
				/* allocated with required size */
};
typedef	struct ftree_info	ftree_info ;

extern	int			ftree ();
extern	int			ftree_get_dir ();
extern	void			ftree_free ();

#endif	/*_FTREE_INFO*/

