/*	istdir.h		

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

#ifndef	_DIRECTORY_DEFINITIONS
#define	_DIRECTORY_DEFINITIONS

#if MSDOS

    /*
     *  Structure returned by DOS interrupt 21h functions 4eh and 4fh.
     */
    struct dir_info
    {
	    unsigned char		di_reserved [21];
	    unsigned char		di_attrib;
	    unsigned short		di_mtime;
	    unsigned short		di_mdate;
	    unsigned long		di_size;
	    char			di_name [13];
    };

    /*
     *  Handle from opendir().
     */
    typedef	struct
		{
		    int				dir_entryno ;
		    struct dir_info		dir_data ;
		    char			dir_name [1];
		}
					DIR ;

    /*
     *  The following structure defines the file
     *  system independent directory entry
     *  as returned from readdir().
     */
    struct dirent
    {
	    long		d_ino;		/* inode number of entry */
	    off_t		d_off;		/* offset of disk dir entry */
	    unsigned short	d_reclen;	/* length of this record */
	    char		d_name[13];	/* name of file */
    };

    /* dirdos.c */
    extern DIR			*opendir();
    extern struct dirent	*readdir();
    extern int			closedir();
    /* Not yet implemented
     * extern long		telldir();
     * extern void		seekdir();
     * #define rewinddir( dirp )	seekdir( dirp, 0L )
     */

#else /*MSDOS*/
# if WIN32

#   include	<windows.h>
#   include	<winnt.h>
#   include	<io.h>

    /*
     *  Structure returned by _findfirst and _findnext.
     *
     *  struct _finddata_t
     *  {
     *      unsigned		attrib;
     *      time_t		time_create;	/ * -1 for FAT file systems * /
     *      time_t		time_access;	/ * -1 for FAT file systems * /
     *      time_t		time_write;
     *      _fsize_t		size;
     *      char		name[260];
     *  };
     */

    /*
     *  Handle from opendir().
     */
    typedef	struct
		{
		    long			dir_handle ;
			//HANDLE			dir_handle ;
		    int				dir_entryno ;
			//WIN32_FIND_DATA		dir_data;
		    struct _finddata_t		dir_data ;
		    char			dir_name [1];
		}
					DIR ;

    /*
     *  The following structure defines the file
     *  system independent directory entry
     *  as returned from readdir().
     */
    struct dirent
    {
	    long		d_ino;		/* inode number of entry */
	    off_t		d_off;		/* offset of disk dir entry */
	    unsigned short	d_reclen;	/* length of this record */
	    char		d_name[_MAX_FNAME];	/* name of file */
    };

    /* dirwinnt.c */
    extern DIR			*opendir();
    extern struct dirent	*readdir();
    extern int			closedir();
    /* Not yet implemented
     * extern long		telldir();
     * extern void		seekdir();
     * #define rewinddir( dirp )	seekdir( dirp, 0L )
     */

# else /*WIN32*/
#   include	<dirent.h>
# endif /*WIN32*/
#endif /*MSDOS*/

#endif /*_DIRECTORY_DEFINITIONS*/

