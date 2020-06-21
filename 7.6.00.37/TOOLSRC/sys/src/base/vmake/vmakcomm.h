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

#ifndef     _VMAKCOMM_H
#define     _VMAKCOMM_H

#include    "vmake.h"

/* exported functions */

/*
 * filestat
 * fileopen
 * fileclose
 * fileread
 * fileseek
 * filegets
 * diropen
 * dirclose
 * dirread
 * fileincopy
 * fileoutcopy
 * close_connection
 * makedir
 * renamefile
 * unlinkfile
 *
*/

/*==========================================================================*/

global	int	    filestat    
(
    char        * name ,
    struct stat * statbuf
);

/*==========================================================================*/

global	VFILE	* fileopen  
(
    char    * name,
    char    * type
);

/*==========================================================================*/

global	int		fileclose   
(
    VFILE   * vfp
);

/*==========================================================================*/

global	int		fileread    
(
    char    * buf,
    int     siz,
    int     cnt,
    VFILE   * vfp
);

/*==========================================================================*/

global	int		fileseek    
(
    long    offset,
    int     type,
    VFILE   * vfp
);

/*==========================================================================*/

global	char	* filegets  
(
    char    * buf,
    int     siz,
    VFILE   * vfp
);

/*==========================================================================*/

global	VFILE	* diropen   
(
    char    * path
);

/*==========================================================================*/

global	int		dirclose    
(
    VFILE   * vfp
);

/*==========================================================================*/

global	int		dirread     
(
    char    * fname,
    VFILE   * vfp
);

/*==========================================================================*/

global	int		fileincopy  
(
    char    * src,
    char    * dst,
    int     filetype
);

/*==========================================================================*/

global	int		fileoutcopy
(
    char    * src,
    char    * dst,
    int     filetype
);

/*==========================================================================*/

global	void	close_connections 
(
    void
);

/*==========================================================================*/

global	int		makedir     
(
    char    * node,
    char    * path
);

/*==========================================================================*/

global	int		renamefile    
(
    char    * node,
    char    * file,
    char    * newfile
);

/*==========================================================================*/

global	int		unlinkfile  
(
    char    * node,
    char    * file
);


#endif
