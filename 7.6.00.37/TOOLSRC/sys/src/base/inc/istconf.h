/*	istconf.h	

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

#ifndef	_CONFIG_DESCRIPTION
#define	_CONFIG_DESCRIPTION

/*
 *  Parameter type definitions
 */
#define		CFGTYP_FILE		1
#define		CFGTYP_STRING		2
#define		CFGTYP_INT		3
#define		CFGTYP_BOOL		4

/*
 *  Parameter description structure
 */
struct config_descr
{
    int				cfd_type ;
    int				cfd_mandatory ;
    char			*cfd_name ;
    char			*cfd_value ;
};
typedef	struct config_descr	config_descr ;

/*
 *  Function prototypes
 */
extern	int			get_config ();
extern	long			get_confval_long ();
extern	char			*get_confval_string ();

/*
 * --------------------------------------------------------
 *         EXAMPLE
 * --------------------------------------------------------
 *  Initialize as follows:
 *
 *  static  char  *config_files [] =
 *	    {
 *		"./myprog.ini" ,
 *		"~/myprog.ini" ,
 *		0
 *	    };
 *
 *  #define		RES_LINES	"lines"
 *  #define		RES_COLS	"columns"
 *  #define		RES_SORT	"sort"
 *  #define		RES_HEADER	"header"
 *  #define		RES_WORKDIR	"workdir"
 *  #define		RES_HISTFILE	"historyfile"
 *
 *  static  config_descr  config_desc [] =
 *	    {
 *		/ *   type    mandatory    name      value
 *		* /
 *		{CFGTYP_INT    , FALSE , RES_LINES    ,0},
 *		{CFGTYP_INT    , FALSE , RES_COLS     ,0},
 *		{CFGTYP_BOOL   , FALSE , RES_SORT     ,0},
 *		{CFGTYP_STRING , TRUE  , RES_HEADER   ,0},
 *		{CFGTYP_FILE   , TRUE  , RES_WORKDIR  ,0},
 *		{CFGTYP_FILE   , TRUE  , RES_HISTFILE ,0},
 *		{0             , 0     , 0            ,0}
 *	    };
 *
 * --------------------------------------------------------
 */

#endif /*_CONFIG_DESCRIPTION*/

