/*      vmakmisc.c

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

#include        "vmakmisc.h"
#include        "vmake.h"
#include        "vmakvars.h"
#include        "vmakcomm.h"
#include        "regex.h"
#include        "vmakcoll.h"

# if defined(WIN32)
#   include	<sys/utime.h>
# else
#   include	<utime.h>
# endif

#ifndef WIN32
#define strnicmp strncasecmp
#endif

#define		FRAGMENT_SIZE	64	/* minimum bytes allocated */

#define REGEXP_VALID_SOURCEINDEX "^:([^:/\\][^:\\]+)/([^:/\\]+)$"
#define REGEXP_VALID_DESCINDEX   "^::(([^:/\\][^:\\]+)/)?([^:/\\]+)$"
#define REGEXP_OLDSTYLED_MODBODY "^(v|g|h|i)([a-z]{2,})([0-9]{2,})"

/* local functions */

static	int		filelink        (char*,char*);
static	int		fileunlink      (char*);
static	int		test_is_dir     (char*);
#ifdef	MKDIR
static	int     mkdir           (char*,int);
#endif
static	void	xprotwrite      (char*, ...);
static	int		get_dir_info    (char*,struct dir_info**);
static	int		get_dir_entry   (char*,struct dir_info*);
static	int		insert_dir_entry (char*,struct dir_info*);
static  void	print_env_opt   (void);
static	int		stretch_file    (char*);
static	int		force_unlink    (char*);

static  int	write_descriptions (struct trg_info*,FILE*);
static  int	compare_descriptions (char*, struct trg_info*);
global char * get_description_line(struct trg_info* );


/* imported data */

/* local data */

static	FILE    * sprotfp ;
static	FILE    * sxprotfp ;
static	FILE    * protfp ;
static	FILE    * xprotfp ;
static	FILE    * mprotfp ;

static	FILE    * eprotfp ;  /* CTS 1112006 */
static	FILE    * seprotfp;  /* CTS 1112006 */

static	FILE    * swprotfp;  
static	FILE    * wprotfp;  

static	char    protname [ FILENAMESIZE ] ;
static	char    protvers ;
static	int		terminating ;

/* exported data */

global  regex_t     * sourceindex_comp = NULL;
global  regex_t     * oldstyledmodule_comp = NULL;
global  regex_t     * descindex_comp = NULL;
global  regmatch_t  regexmatches[10];

global  char        * vmake_version = VMAKE_VERSION ;
global  char        * vmake_coll = "collection phase" ;

int     dbgdbglvl;
int     dbgcurlvl;

/*==========================================================================*/


global	int     get_file
(
/* in  */ char            * name ,
/* in  */ char            * dir ,
/* in  */ char            * subdir1 ,
/* in  */ char            * subdir2 ,
/* in  */ int             file_kind ,   /* FK_XXXXX */
/* in  */ valid_bit_arr   vbits ,
/* out */ char            ** file ,
/* out */ time_t          * date ,
/* out */ int			  * path_status,
/* in  */ int             no_dircache
)
{
	int                     rc = 0 ;
	int                     lgt ;
	int			            level ;
	int			            stop_level ;
	struct path_info        * pip ;
	struct dir_info         * dirp ;
	struct stat             statbuf ;
	char			        path [ LINESIZE ] ;
			/* LINESIZE: sprintf should not run out of space */

	DBG4 (( "get_file: name    '%s' \n", name ))
	DBG4 (( "get_file: dir     '%s' \n", dir ))
	DBG4 (( "get_file: subdir1 '%s' \n", subdir1 ? subdir1 : "" ))
	DBG4 (( "get_file: subdir2 '%s' \n", subdir2 ? subdir2 : "" ))

	* date = 0l;
	stop_level = file_kind & FK_LEVEL ;
	* path_status = stop_level ; /* if file not found */
	DBG4 (( "get_file: stoplvl %d \n", stop_level ))

    for ( pip = vmake_path ; pip != NULL ; pip = pip->next )
	{
		level = pip->path_status & PATH_LEVEL ;
	    DBG5 (( "get_file: level %d \n", level ))

	    /*
	     *  If neither the warning "Newer file on later tree"
	     *  nor a download is required, we can shortcut the loop.
	     */
	    if (( (file_kind & FK_SOURCE) && ! vopt_source_warn && ! vopt_source_copy ) ||
		     ((file_kind & FK_OBJECT) && ! vopt_object_warn ) )
	    {
		    if ( level > stop_level )
		    {
		        DBG8 (( "get_file: stop level exceeded \n" ))
		        break ;
		    }
		    if ( ! VALID_BIT_TST(level,vbits) )
		    {
		        DBG8 (( "get_file: no valid dates file in '%s' \n",
							        pip->path ))
		        continue ;
		    }
	    }

	    if ( ( (file_kind & FK_OBJECT) &&
	           (pip->host_permission & HOST_OBJECTS_DENIED) ) ||
	         ( (file_kind & FK_TOOL) &&
	           (pip->host_permission & HOST_TOOLS_DENIED) )      )
	    {
		    DBG8 (( "get_file: skipping tree '%s' \n", pip->path ))
		    continue ;
	    }

	    /*
	     *  Build the directory path.
	     */
	    (void) sprintf ( path , dir , pip->path , subdir1 ? subdir1 : "" ,
            subdir2 ? subdir2 : "" );
	    lgt = strlen ( path );
	    if ( lgt >= FILENAMESIZE )
	    {
		    DBG0 (( "get_file: file name too long '%s' \n", path ))
		    return ( - 2 );
	    }

        if ( no_dircache + env_VMAKE_NO_DIRCACHE == 0  )
        {
	        /*
	         *  Fetch the directory information.
	         */
            rc = get_dir_info ( path , & dirp );
	        if ( rc < 0 )
	        {
				/* &gar PTS 1102705 */
				if ( rc == ERROR_NO_CASE_SENSITIVE )
				{	
					return ( ERROR_NO_CASE_SENSITIVE );
				}
				 /* &gar PTS 1102705 */
		        DBG4 (( "get_file: get_dir_info error %d \n", rc ))
		        return ( rc );
	        }
	        if ( rc == 0 )
	        {
		        DBG5 (( "get_file: no such directory '%s' \n", path ))
		        continue ; /* no such directory */
	        }

	        /*
	         *  Check for the existence of the file in that directory.
	         */
	        rc = get_dir_entry ( name , dirp );
	        if ( rc <= 0 )
	        {
				 /* &gar PTS 1102705 */
				if ( rc == ERROR_NO_CASE_SENSITIVE )
				{
					return ( ERROR_NO_CASE_SENSITIVE );
				}
				 /* &gar PTS 1102705 */
		        DBG5 (( "get_file: no such file '%s' \n", name ))
		        continue ; /* no such file */
	        }
        }

	    /*
	     *  Build the complete path name of the file.
	     */
	    (void) strncpy  ( path + lgt , name , (CINT)(sizeof(path) - lgt) );
	    if ( (int) strlen ( path + lgt ) >= FILENAMESIZE - lgt )
	    {
		    DBG0 (( "get_file: file name too long '%s' \n", path ))
		    return ( - 2 );
	    }

	    /*
	     *  Stat the file.
	     */
	    rc = filestat ( path , & statbuf );
	    if ( rc < 0 )
	    {
    		if ( (geterrno() == ENOENT) || (geterrno() == ENOTDIR) )
		    {
		        DBG2 (( "file not found    '%s'\n", path ))
		    }
		    else
		    {
		        DBG0 (( "get_file: cannot stat '%s': %s \n",
						    path , serrno() ))
		        return ( - 2 );
		    }
	    }
	    else  /* file found */
	    {
		    /*
		     *  If the stop level is not exceeded
		     *  AND a file was not yet found (*date == 0)
		     *      OR  the currently examined file is newer
		     *          AND it is no local make
		     *          AND the copy option is active
		     *          AND the last found file is in the local tree
		     *              (means: get the first non-local file)
		     *  AND the currently examined file has a valid dates file
		     *  then this file will be used.
		     */

			 if (
                (level <= stop_level) &&
                ( (*date <= 0 ) ||
                 (
				 /* (statbuf.st_mtime > *date + TOL) && ! vopt_local &&  PTS GERT*/
                    (statbuf.st_mtime > *date + TOL) && ! vopt_local &&                     (
                     ( (file_kind & FK_SOURCE) && vopt_source_copy) ||
                      (
                       (file_kind & FK_OBJECT) && vopt_object_copy
                      )
                    ) && ((*path_status & PATH_LEVEL) == 0)
                 )
                ) && VALID_BIT_TST(level,vbits)
               )
		    {
		        if ( *date > 0 )
			        DBG1 (( "%s (%s) \n", ascdate(*date) , *file ))

                *file = (char *) repalloc( *file, strlen( path ) + 1 );
                (void) strcpy ( *file , path );

		        * date = statbuf.st_mtime ;
		        * path_status = pip->path_status ;
		        DBG5 (( "get_file: %s %s 0x%04x \n",
			        ascdate(*date) , file , *path_status ))

		        if ( ((file_kind & FK_SOURCE) &&
			      ! vopt_source_warn && ! vopt_source_copy) ||
		             ((file_kind & FK_OBJECT) &&
			      ! vopt_object_warn && ! vopt_object_copy)    )
		        {
			        DBG8 (( "get_file: search completed \n" ))
			        break ;
		        }
		    }
		    else
		    {
		        DBG5 (( "get_file: stoplvl||older||local||!copy \n" ))
		        /*
		         *  If a file was already found (*date != 0)
		         *  and the currently examined file is newer
		         *  and the warning option is active
		         *  then warn.
		         */
		        if ( ( ( *date>0 ) || (level > stop_level)) &&
			     (statbuf.st_mtime > * date) &&
			     ( ((file_kind & FK_SOURCE) && vopt_source_warn) ||
			       ((file_kind & FK_OBJECT) && vopt_object_warn)    ) )
		        {
			        if ( * date )
			            DBG0 (( "WARNING: newer file on later VMAKE_PATH \n(%s on %s) \n",
									        name, path ))
						/* &gar CTS 1000199 */
			        else
			            DBG1 (( "WARNING: file exists on later VMAKE_PATH '%s'\n",
									        name ))
			        DBG1 (( "%s (%s)\n", ascdate(statbuf.st_mtime) , path ))
		        }
		    }
	    }/*end if file found*/
	
	} /* end - for ( pip = vmake_path ; pip != NULL ; pip = pip->next ) */
	if ( ! pip ) DBG8 (( "get_file: VMAKE_PATH exhausted \n" ))

	if ( * date > 0 )  /* if file found */
	{
	    DBG4 (( "get_file: returning %d \n", 1 ))
	    return ( 1 );
	}

	/* in absence of a file create a new file name in home root */

	rc = new_file ( name , dir , subdir1 , subdir2 , file );

	DBG4 (( "get_file: file not found, returning %d \n", rc ))
	return ( rc );
}

/*==========================================================================*/
global	int     get_directory
(
/* in  */ char            * dirname ,
/* in  */ int             file_kind ,   /* FK_XXXXX */
/* in  */ valid_bit_arr   vbits ,
/* out */ char            ** directory 
)
{
	int                     rc = 0 ;
	int                     lgt ;
	int			            level ;
	int			            stop_level ;
	struct path_info        * pip ;
	struct dir_info         * dirp ;
	/* struct stat             statbuf ;*/
	char			        path [ LINESIZE ] ;
			/* LINESIZE: sprintf should not run out of space */

	DBG4 (( "get_dir: dirname    '%s' \n", dirname ))
	
	stop_level = file_kind & FK_LEVEL ;
	

    for ( pip = vmake_path ; pip != NULL ; pip = pip->next )
	{
		level = pip->path_status & PATH_LEVEL ;
	    DBG5 (( "get_file: level %d \n", level ))

	    /*
	     *  If neither the warning "Newer file on later tree"
	     *  nor a download is required, we can shortcut the loop.
	     */
	    if (( (file_kind & FK_SOURCE) && ! vopt_source_warn && ! vopt_source_copy ) ||
		     ((file_kind & FK_OBJECT) && ! vopt_object_warn ) )
	    {
		    if ( level > stop_level )
		    {
		        DBG8 (( "get_file: stop level exceeded \n" ))
		        break ;
		    }
		    if ( ! VALID_BIT_TST(level,vbits) )
		    {
		        DBG8 (( "get_file: no valid dates file in '%s' \n",
							        pip->path ))
		        continue ;
		    }
	    }

	    if ( ( (file_kind & FK_OBJECT) &&
	           (pip->host_permission & HOST_OBJECTS_DENIED) ) ||
	         ( (file_kind & FK_TOOL) &&
	           (pip->host_permission & HOST_TOOLS_DENIED) )      )
	    {
		    DBG8 (( "get_file: skipping tree '%s' \n", pip->path ))
		    continue ;
	    }

	    /*
	     *  Build the directory path.
	     */
	    (void) sprintf ( path , DIR_SRC , pip->path , dirname );
	    lgt = strlen ( path );
	    if ( lgt >= FILENAMESIZE )
	    {
		    DBG0 (( "get_file: file name too long '%s' \n", path ))
		    return ( - 2 );
	    }

        if ( env_VMAKE_NO_DIRCACHE == 0  )
        {
	        /*
	         *  Fetch the directory information.
	         */
            rc = get_dir_info ( path , & dirp );
            if ( rc < 0 )
	        {
				/* &gar PTS 1102705 */
				if ( rc == ERROR_NO_CASE_SENSITIVE )
				{	
					return ( ERROR_NO_CASE_SENSITIVE );
				}
				 /* &gar PTS 1102705 */
		        DBG4 (( "get_dir: get_dir_info error %d \n", rc ))
		        return ( rc );
	        }
			if ( (rc == 0) || (dirp->entries == NULL) )
	        {
		        DBG5 (( "get_dir: no such directory '%s' \n", path ))
		        continue ; /* no such directory */
	        }
			else
			{
				*directory = (char *) repalloc( *directory, strlen( path ) + 1 );
                (void) strcpy ( *directory , path );
				return (1);
			}
        }
	}
	rc = new_file ( dirname , DIR_SRC , "" , "" , directory );

	DBG4 (( "get_dir: file not found, returning %d \n", rc ))
	return ( rc );
}



/*==========================================================================*/

global	int     get_dates_file
(
    struct trg_info * tip ,	/* IN  */
    int             file_kind ,	/* IN  */
    time_t          srcdate ,	/* IN  */
    valid_bit_arr   vbits 		/* OUT */
)
{
	int                     rc=0 ;
	int                     lgt ;
	int			level ;
	int			stop_level ;
	struct path_info        * pip ;
	struct dir_info         * dirp ;
	struct stat             statbuf ;
	char			path [ LINESIZE ] ;
	char			dates_name [ NAMESIZE ] ;

	char            * name;	/* IN  */
    char            * subdir1;	/* IN  */
    char            * subdir2;	/* IN  */
	
	name = tip->name;
	subdir1 = versname(tip->version);
	subdir2 = tip->layer;
	/* LINESIZE: sprintf should not run out of space */


	DBG4 (( "get_dates_file: name    '%s' \n", name ))
	DBG4 (( "get_dates_file: dir     '%s' \n", DIR_DATES ))
	DBG4 (( "get_dates_file: subdir1 '%s' \n", subdir1 ? subdir1 : "" ))
	DBG4 (( "get_dates_file: subdir2 '%s' \n", subdir2 ? subdir2 : "" ))

	VALID_BIT_CLEAR ( vbits );
	stop_level = file_kind & FK_LEVEL ;
	DBG4 (( "get_dates_file: stoplvl %d \n", stop_level ))
	( void ) strcpy ( dates_name, name ) ;
	( void ) strcat ( dates_name, ".dat" ) ;

	for ( pip = vmake_path ; pip != NULL ; pip = pip->next )
	{
	    if ( ( (file_kind & FK_OBJECT) &&
	           (pip->host_permission & HOST_OBJECTS_DENIED) ) ||
	         ( (file_kind & FK_TOOL) &&
	           (pip->host_permission & HOST_TOOLS_DENIED) )      )
	    {
		    DBG8 (( "get_dates_file: skipping tree '%s' \n", pip->path ))
		    continue ;
	    }
	    level = pip->path_status & PATH_LEVEL ;
	    DBG5 (( "get_dates_file: level %d \n", level ))

	    /*
	     *  Dates file cannot exceed stop_level.
	     */
	    if ( level > stop_level )
	    {
		    DBG8 (( "get_file: stop level exceeded \n" ))
		    break ;
	    }

	    /*
	     *  Build the directory path.
	     */
        (void) sprintf ( path , DIR_DATES , pip->path , subdir1 ? subdir1 : "",
            subdir2 ? subdir2 : "" );
	    lgt = strlen ( path );
	    if ( lgt >= FILENAMESIZE )
	    {
		    DBG0 (( "get_dates_file: file name too long '%s' \n", path ))
		    return ( - 2 );
	    }

	    /*
	     *  Fetch the directory information.
	     */
	    rc = get_dir_info ( path , & dirp );
	    if ( rc < 0 )
	    {
		    DBG4 (( "get_dates_file: get_dir_info error %d \n", rc ))
		    return ( rc );
	    }
	    if ( rc == 0 )
	    {
		    DBG5 (( "get_dates_file: no such directory '%s' \n", path ))
		    continue ; /* no such directory */
	    }

	    /*
	     *  Check for the existence of the file in that directory.
	     */
	    rc = get_dir_entry ( dates_name , dirp );
	    if ( rc <= 0 )
	    {
			if (rc == ERROR_NO_CASE_SENSITIVE)
			{
				DBG5 (( "get_dates_file: date file '%s' ignored \n", dates_name ))	
				break;
			}
		    DBG5 (( "get_dates_file: no such file '%s' \n", dates_name ))
		    continue ; /* no such file */
	    }

	    /*
	     *  Build the complete path name of the file.
	     */
	    (void) strncpy  ( path + lgt , dates_name , (CINT)(sizeof(path) - lgt) );
	    if ( (int) strlen ( path + lgt ) >= FILENAMESIZE - lgt )
	    {
		    DBG0 (( "get_dates_file: file name too long '%s' \n", path ))
		    return ( - 2 );
	    }

	    /*
	     *  Stat the file.
	     */
	    rc = filestat ( path , & statbuf );
	    if ( rc < 0 )
	    {
		    if ( (geterrno() == ENOENT) || (geterrno() == ENOTDIR) )
		    {
		        DBG2 (( "file not found    '%s'\n", path ))
		    }
		    else
		    {
		        DBG0 (( "get_dates_file: cannot stat '%s': %s \n",
						    path , serrno() ))
		        return ( - 2 );
		    }
	    }
	    else
	    {
		    DBG5 (( "get_dates_file: %s %s\n",
			    ascdate(statbuf.st_mtime) , path ))
		    /*
		     *  If the dates file matches the sources date,
		     *  then this file will be used.
		     *  Select an own dates file regardless of its date,
		     *  just in case that no other one is present.
		     */
			DBG1 (( "%s %s \n", ascdate(statbuf.st_mtime) , path ))
		    if (    (statbuf.st_mtime >= srcdate - TOL)
		         && (statbuf.st_mtime <= srcdate + TOL) )
			{
				/* start the check of descriptions */
				rc = 0; 
/*
				if (tip->type != TT_INC) 
				{
					rc = compare_descriptions( path, tip);
				}
				*/
				if (rc == 0 )
					VALID_BIT_SET ( level , vbits );
				else
					if ( rc < 0 )
					{
						DBG0 (( "get_dates_file: cannot read '%s': %s \n",
									path , serrno() ))
						return ( - 2 );
					}
			}
			
			if (statbuf.st_mtime > srcdate + TOL)
					DBG0 (( "WARNING: found newer datefile (%s)\n", path ));
		

	    }
	}
	if ( ! pip ) DBG8 (( "get_dates_file: VMAKE_PATH exhausted \n" ))

	DBG4 (( "get_dates_file: returning %d \n", 0 ))
	return ( 0 );
}


/*==========================================================================*/

global	int     new_file
(
    char    * name ,
    char    * dir ,
    char    * subdir1 ,
    char    * subdir2 ,
    char    ** file
)
{
	int                     lgt ;
	char			path [ LINESIZE ] ;
			/* LINESIZE: sprintf should not run out of space */


	DBG4 (( "new_file: name    '%s' \n", name ))
	DBG4 (( "new_file: dir     '%s' \n", dir ))
	DBG4 (( "new_file: subdir1 '%s' \n", subdir1 ? subdir1 : "" ))
	DBG4 (( "new_file: subdir2 '%s' \n", subdir2 ? subdir2 : "" ))

	if ( vmake_path == NULL )
	{
		/* don't change this output or change it also in iprot.pm (PTS 1108883) */
		DBG0 (( "vmake_path is NULL !!! \n" ))
		return ( - 2 );
	}
    (void) sprintf ( path , dir , vmake_path->path ,
        subdir1 ? subdir1 : "", subdir2 ? subdir2 : "" );
	lgt = strlen ( path );

    (void) strncpy  ( path + lgt , name , (CINT)(sizeof(path) - lgt) );
	lgt += strlen ( path + lgt );
	
    *file = (char *) repalloc( *file, lgt + 1 );
    COPY ( *file , path , lgt );
	(*file) [ lgt ] = '\0' ;

    DBG4 (( "new_file: new file          '%s' \n", *file ))

	DBG4 (( "new_file: returning %d \n", 0 ))
	return ( 0 );
}


/*==========================================================================*/

static  int     get_dir_info
(
    char                    * path ,
    struct dir_info         ** dirpp
)
{
	int                     rc=0 ;
	struct dir_info         ** sdirpp , * dirp ;
	VFILE                   * fp ;
	char                    fname [ NAMESIZE ] = "";

    DBG2 (( "getdirinf: called \n" ))
	DBG3 (( "getdirinf: path '%s' \n", path ))

    * dirpp = NULL ;

	rc = - 1 ;
	sdirpp = & dir_lists;

	while ( ( (*sdirpp) != NULL ) && ( rc != 0 ) )
	{
#ifdef unix
		rc = strcmp ( (*sdirpp)->dirpath , path );
#else
		rc = stricmp ( (*sdirpp)->dirpath , path );
#endif
		if (rc < 0 )
			sdirpp = & (*sdirpp)->right;
		else
    		if (rc > 0 ) sdirpp = & (*sdirpp)->left;
	}

	if ( rc == 0 )
	{
	    DBG8 (( "getdirinf: found directory '%s' \n", (*sdirpp)->dirpath ))
	}
	else
	{
	    dirp = (struct dir_info *) palloc ( sizeof(struct dir_info) );
	    if ( dirp == NULL )
	    {
		    DBG0 (( "insufficient dynamic memory \n" ))
		    return ( - 3 );
	    }

	    FILL (  dirp , 0 , sizeof(struct dir_info) );

		dirp->dirpath = (char *) palloc( strlen( path ) + 1 );
		(void) strcpy( dirp->dirpath, path );


		* sdirpp = dirp ;
	    DBG7 (( "getdirinf: inserted '%s' \n", dirp->dirpath ))

	    fp = diropen ( path ); /* open directory !!! */
	    if ( fp == NULL )
	    {
		    DBG4 (( "getdirinf: cannot open directory      '%s' \n", path ))
		    return ( 0 );
	    }
	    DBG4 (( "getdirinf: opened directory '%s' \n", path ))

	    while ( dirread ( fname , fp ) == 1 )
	    {
		    DBG7 (( "getdirinf: dirent '%.16s' \n", fname ))

		    rc = insert_dir_entry ( fname , dirp );
		    if ( rc < 0 )
		    {
		        DBG8 (( "getdirinf: insert error %d \n", rc ))
		        (void) dirclose ( fp );
		        return ( rc );
		    }
	    }

	    (void) dirclose ( fp );
	}

	* dirpp = * sdirpp ;

	DBG4 (( "getdirinf: returning %d \n", 1 ))
	return ( 1 );
}

/*==========================================================================*/

static  int     get_dir
(
    char                    * path ,
    struct dir_info         ** dirpp
)
{
	int                     rc=0 ;
	struct dir_info         ** sdirpp , * dirp ;
	VFILE                   * fp ;
	char                    fname [ NAMESIZE ] = "";

    DBG2 (( "getdirinf: called \n" ))
	DBG3 (( "getdirinf: path '%s' \n", path ))

    * dirpp = NULL ;

	rc = - 1 ;
	sdirpp = & dir_lists;

	while ( ( (*sdirpp) != NULL ) && ( rc != 0 ) )
	{
#ifdef unix
		rc = strcmp ( (*sdirpp)->dirpath , path );
#else
		rc = stricmp ( (*sdirpp)->dirpath , path );
#endif
		if (rc < 0 )
			sdirpp = & (*sdirpp)->right;
		else
    		if (rc > 0 ) sdirpp = & (*sdirpp)->left;
	}

	if ( rc == 0 )
	{
	    DBG8 (( "getdirinf: found directory '%s' \n", (*sdirpp)->dirpath ))
	}
	else
	{
	    dirp = (struct dir_info *) palloc ( sizeof(struct dir_info) );
	    if ( dirp == NULL )
	    {
		    DBG0 (( "insufficient dynamic memory \n" ))
		    return ( - 3 );
	    }

	    FILL (  dirp , 0 , sizeof(struct dir_info) );

		dirp->dirpath = (char *) palloc( strlen( path ) + 1 );
		(void) strcpy( dirp->dirpath, path );


		* sdirpp = dirp ;
	    DBG7 (( "getdirinf: inserted '%s' \n", dirp->dirpath ))

	    fp = diropen ( path ); /* open directory !!! */
	    if ( fp == NULL )
	    {
		    DBG4 (( "getdirinf: cannot open directory      '%s' \n", path ))
		    return ( 0 );
	    }
	    DBG4 (( "getdirinf: opened directory '%s' \n", path ))

	    while ( dirread ( fname , fp ) == 1 )
	    {
		    DBG7 (( "getdirinf: dirent '%.16s' \n", fname ))

		    rc = insert_dir_entry ( fname , dirp );
		    if ( rc < 0 )
		    {
		        DBG8 (( "getdirinf: insert error %d \n", rc ))
		        (void) dirclose ( fp );
		        return ( rc );
		    }
	    }

	    (void) dirclose ( fp );
	}

	* dirpp = * sdirpp ;

	DBG4 (( "getdirinf: returning %d \n", 1 ))
	return ( 1 );
}


/*==========================================================================*/

static  int     get_dir_entry
(
    char                    * name ,
    struct dir_info         * dirp
)
{
	int                     rc=0 ;
	struct dir_entry        * sdirep ;
	char                    name_src [ FILENAMESIZE ] ;
	char                    name_desc [ FILENAMESIZE ] ;
	
	DBG4 (( "getdirent: name '%s' \n", name ))

	rc = - 1 ;
/*	for ( sdirep = dirp->entries ; sdirep != NULL ;
	      sdirep = sdirep->next )
	{
#ifdef unix
		rc = strcmp ( sdirep->name , name );
#else
		rc = stricmp ( sdirep->name , name );
#endif
		if ( rc >= 0 ) break ;
	}
*/
	sdirep = dirp->entries ;
	while ( ( sdirep != NULL ) && ( rc != 0 ) )
	{
#ifdef unix
		rc = strcmp ( sdirep->name , name );
#else
		rc = stricmp ( sdirep->name , name );
		 /* &gar PTS 1102705 -> */
		if ( ( rc==0 ) && ( strcmp (sdirep->name , name ) != 0) )
		{
			(void) sprintf ( name_src, DIR_SRC, vmake_path->path, "" );
			(void) sprintf ( name_desc, DIR_DSCR, vmake_path->path, "" );
			/* change only to right upper/lower in $OWN and if not in desc or src */
			/* for date files and local maked files */
			if  ( (strncmp (dirp->dirpath, vmake_path->path, strlen(vmake_path->path)) == 0)
				/* length -1 because last // from DIR_... not found */
				&& ( strncmp (dirp->dirpath, name_src, strlen(name_src)-1 ) != 0) &&
					( strncmp (dirp->dirpath, name_desc, strlen(name_desc)-1 ) != 0) )
			{	
				rc = rename_file_case_sensitive ( dirp->dirpath, sdirep->name , name );
				return rc;
			}
			else
			{
				if (vopt_no_case_sens_error)
				{
					DBG0 (( "Warning: There are different ways of writings \n         of the target '%s' in the hierarchy \n", name ))
				}
				else
				{
					DBG0 (( "Error: There are different ways of writings \n       of the target '%s' in the hierarchy \n", name ))
					return ( ERROR_NO_CASE_SENSITIVE );
				}
			}
		}
		/*  <- &gar PTS 1102705 */
#endif	
		if (rc < 0 )
			sdirep = sdirep->right;
		else
		    if (rc > 0 ) sdirep = sdirep->left;
	}
	
	if ( rc != 0 )
	{
		DBG4 (( "getdirent: returning %d \n", 0 ))
		return ( 0 );
	}

	DBG4 (( "getdirent: returning %d \n", 1 ))
	return ( 1 );
}


/*==========================================================================*/

static  int     insert_dir_entry
(
    char                    * filnam ,
    struct dir_info         * listp
)
{
	int                     rc=0 ;
	struct dir_entry        ** sdirepp , * direp ;


	DBG4 (( "insdirent: filnam '%s' \n", filnam ))

	rc = - 1 ;
/*	for ( sdirepp = & listp->entries ; (*sdirepp) != NULL ;
	      sdirepp = & (*sdirepp)->next )
	{
#ifdef unix
	    rc = strcmp ( (*sdirepp)->name , filnam );
#else
	    rc = stricmp ( (*sdirepp)->name , filnam );
#endif
	    if ( rc >= 0 ) break ;
	}
*/
	sdirepp = & listp->entries ;
	while ( ( (*sdirepp) != NULL ) && ( rc != 0 ) )
	{
#ifdef unix
		rc = strcmp ( (*sdirepp)->name , filnam );
#else
		rc = stricmp ( (*sdirepp)->name , filnam );
#endif
		if (rc < 0 )
			sdirepp = & (*sdirepp)->right;
		else
		if (rc > 0 )
			sdirepp = & (*sdirepp)->left;
	}

	if  ( rc == 0 )
	{
	    DBG8 (( "insdirent: entry found '%s' \n", (*sdirepp)->name ))
	}
	else
	{
	    direp = (struct dir_entry *) palloc ( sizeof(struct dir_entry) );
	    if ( direp == NULL )
	    {
		    DBG0 (( "insufficient dynamic memory \n" ))
		    return ( - 3 );
	    }

	    FILL (  direp , 0 , sizeof(struct dir_entry) );

		direp->name = (char *) palloc( strlen( filnam ) + 1 );
	    (void) strcpy ( direp->name , filnam );

	    * sdirepp = direp ;

	    DBG8 (( "insdirent: inserted '%s' \n", direp->name ))
	    rc = 1 ;
	}

	DBG4 (( "insdirent: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/

global	int	vcopy_file
(
    char			* src ,
    char			* dst ,
    time_t			date ,
    int			filetype
)
{
	int			    rc=0 ;
	struct utimbuf	utim ;
	struct stat		statbuf ;
	char			line [ LINESIZE ];
    char            fname[ FILENAMESIZE ];
	char            * p;


	DBG4 (( "vcopy_file: src '%s' \n", src ))
	DBG4 (( "vcopy_file: dst '%s' \n", dst ))

	/* PTS 1109460 */
	if ( dst[ strlen( dst ) - 1 ] == '/')
	{
		p = strrchr( src, '/' );
		if ( p )
		{
            (void) strcpy( fname, dst );
			(void) strcat( fname, p + 1 );
		}
		else
		{
            (void) strcpy( fname, dst );
			(void) strcat( fname, src );
		}
        dst = fname;
	}

	(void) sprintf ( line , "%s %s %s \n", CMD_COPY , src , dst );
	if ( opt_print ) voutwrite ( line );
	xprotwrite ( line );
	if ( opt_noexec ) return ( 0 );

	/*
	 *  Create the destination directory, if not existent.
	 */
	rc = check_dir ( dst );
	if ( rc ) return ( rc );

	/*
	 *  If the date is not known, stat the file.
	 */
	if ( ! date )
	{
	    rc = filestat ( src , & statbuf );
	    if ( rc < 0 )
	    {
		    DBG0 (( "cannot stat '%s': %s \n", src , serrno() ))
		    return ( -2 );
	    }
	    date = statbuf.st_mtime ;
	}
	/* check for exsitence of the same file */

	rc = filestat ( dst , & statbuf );
	if ((rc == 0) &&  (date == statbuf.st_mtime))
	{
		DBG1 (("vcopy_file skipped - same time stamp\n"))
		DBG1 (("(%s -> %s)\n", src, dst))
		return (0);
	}

	/*
	 *  Create a temporary file name.
	 *  If interrupted, vcopy_file should not leave an
	 *  incomplete file in the source directory.
	 */
	(void) sprintf ( line , "VM%06d.TMP" , (int) getpid() );

	/*
	 *  Unlink the temp file, if present.
	 */
	(void) unlink ( line );

	rc = fileincopy ( src , line , filetype );

	if ( ! rc )
	{
	    /*
	     *  Reset the original date.
	     */
	    utim.actime = date ;
	    utim.modtime = date ;

	    rc = utime ( line , & utim );
	    if ( rc < 0 )
	    {
		    DBG0 (( "cannot utime '%s': %s \n", line , serrno() ))
		    rc = -3 ;
	    }
	}

	if ( ! rc ) rc = filelink ( line , dst );

	(void) unlink ( line );

	DBG4 (( "vcopy_file: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/

global	int	get_date_and_size
(
    char    * src ,
    time_t	* datep ,
    long	* sizep ,
    int     src_exists
)
{
	int			rc=0 ;
	struct stat		statbuf ;


	if ( opt_collection ) /* PTS 1110752 */
	{
	    statbuf.st_mtime = time ( (time_t*)0 );
	    statbuf.st_size  = 1 ;
	}
	else
	{
	    rc = filestat ( src , & statbuf );
	    if ( rc < 0 )
	    {
            if ( src_exists )
            {
		        DBG0 (( "cannot stat '%s': %s \n", src , serrno() ))
		        return ( -2 );
            }
            else
            {
	            statbuf.st_mtime = 0 ;
	            statbuf.st_size  = 1 ;
            }
	    }
	}

	if ( datep ) *datep = statbuf.st_mtime ;
	if ( sizep ) *sizep = statbuf.st_size ;

	return ( 0 );
}


/*==========================================================================*/

global	int	touch_file
(
	struct trg_info *tip,
    char			* dst ,
    time_t			date ,
    char			* src ,
    int             file_exists
)
{
	int			    rc=0, fd ;
	struct utimbuf	utim ;
	char			line [ LINESIZE ];


	if ( *src )
	    (void) sprintf ( line , "touch %s (from %s) \n", dst , src );
	else
	    (void) sprintf ( line , "touch %s \n", dst );
	if ( opt_print ) voutwrite ( line );
	xprotwrite ( line );
	if ( opt_noexec ) return ( 0 );

	utim.actime  = date ;
	utim.modtime = date ;

    if ( ! file_exists )
    {
	    /*
	     *  Unlink an old dates file.
	     *  This is necessary, if the current make-run is done by a different
	     *  user than the last one.
	     *  utime(2) works only for the owner of the file.
	     */
	    (void) unlink ( dst );

        fd = open ( dst , O_WRONLY | O_CREAT , 0666 );
        if ( fd < 0 )
        {
			DBG0 (( "cannot create file '%s': %s \n", dst , serrno() ))
			return ( -3 );
        }
        (void) close ( fd );
    }

	rc = utime ( dst , & utim );
	if ( rc < 0 )
	{
		DBG0 (( "cannot utime '%s': %s \n", dst , serrno() ))
		return ( -3 );
	}

	return ( 0 );
}


/*==========================================================================*/

global	int	make_dates_file
(
	struct trg_info        * tip,
	time_t			         date
)
{
	int			rc ;
	char		* file = NULL;
	FILE			*fp ;
	char		name [ FILENAMESIZE ];

	DBG2 (( "makdatfile: called \n" ))
	DBG3 (( "makdatfile: making '%s' \n", tip->name ))


   
	if ( opt_noexec ) return ( 0 );

	(void) sprintf ( name , "%s.dat", tip->name );
  
    fp = fopen ( name , "w" );
    if ( ! fp )
		return (ewrite_all (tip, 1, -2, "ERROR: cannot create '%s' \n", name ));


    rc = write_descriptions ( tip , fp );
	if ( rc ) return ( rc );

    (void) fclose ( fp );
    
	rc = new_file ( tip->name , DIR_DATES , versname(tip->version) , tip->layer , &file );
	if ( rc ) return ( rc );

	/* a target may named like a layer! Therefore notice this as a file */
    file = (char *) repalloc( file, strlen( file ) + strlen( ".dat" ) + 1 );
	( void ) strcat ( file , ".dat" );

	rc = check_dir ( file );
	if ( rc ) return ( rc );

    rc = move_file ( name , file );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while moving %s to %s \n", name , file  ));

    rc = touch_file ( tip, file , date , tip->name , TRUE );
    FREE_AND_SET_TO_NULL(file);
    return ( rc );
}


/*==========================================================================*/

static  int	write_descriptions 
(
    struct trg_info		*tip ,
    FILE			*fp 
)
{
    int				    rc = 0 ;
	char                *description_line = 0;

    DBG2 (( "listdates: called \n" ))

	description_line = get_description_line(tip);
	
	rc = fputs ( description_line , fp );
	if ( rc < 0 )
		return (ewrite_all (tip, 1, -2, "ERROR: cannot write dates file for '%s': %s \n",
	 			tip->name , serrno() ));

    return ( 0 );
}

/*==========================================================================*/

static  int	compare_descriptions 
(
    char 		        *path ,
    struct trg_info		*tip 
)
{
    int				    rc = 0 ;
    VFILE               * fp ;
    char *file_line;
	char * description_line;
	char *statinfo_line = NULL;
	char *p;

	DBG2 (( "comp_dsc: called \n" ))

	description_line = get_description_line (tip);

    fp = fileopen ( path , "r" );
	if ( ! fp )
	{
	    DBG0 (( "cannot open '%s' \n", path ))
		eprotwrite ( "cannot open '%s' \n", path );
	    return ( -2 );
	}
	DBG8 (( "com_dsc opened '%s' \n", path ))

	file_line = (char*) palloc (strlen(description_line)+ 20);

	p = filegets ( file_line , strlen(description_line)+ 19 , fp );
	(void) fileclose ( fp );

	if ( p == NULL )
	{
		DBG1 (( "cannot read '%s' \n", path ))
	    rc = 1;
	}
	else
		if (strcmp (description_line, file_line) != 0)
		{
			DBG1 (("Found different descriptions:\n%s (target)\n%s (datefile)\n", description_line, file_line ))
			rc = 1;
		}
    return (rc);
}

global char * get_description_line
(
	struct trg_info *tip
)
{
	char description_line[ LINESIZE ] ;
	char *ret_description_line = NULL;
	struct dsc_info *dip;
	char *p = description_line;

	DBG2 (( "get_dsc_line: called \n" ))

	if (! tip->description_line)
	{
		description_line[0] = '\0';

		for (dip=tip->descriptions; dip; dip=dip->next)
		{
			strcat (description_line, " && ");
			strcat (description_line, dip->descriptor);
			if (dip->value)
				strcat (description_line, dip->value);
		}

		if (tip->stat.ascii)
			strcat (description_line, " && ascii");
		if (tip->stat.binary)
			strcat (description_line, " && binary");
		if (tip->stat.checkdate)
			strcat (description_line, " && checkdate");
		if (tip->stat.debug)
			strcat (description_line, " && debug");
		if (tip->stat.definition)
			strcat (description_line, " && definition");
		if (tip->stat.exec)
			strcat (description_line, " && exec");
		if (tip->stat.forcemake)
			strcat (description_line, " && forcemake");
		if (tip->stat.ignore_err)
			strcat (description_line, " && ignore_err");
		if (tip->stat.nodebug)
			strcat (description_line, " && nodebug");
		if (tip->stat.noextension)
			strcat (description_line, " && noextension");
		if (tip->stat.profile)
			strcat (description_line, " && profile");
		if (tip->stat.realname)
			strcat (description_line, " && realname");
		if (tip->stat.shrglob)
			strcat (description_line, " && shrglob");
		if (tip->stat.text)
			strcat (description_line, " && text");
	    
		/* cut first ' && ' */
		if (description_line[0] != '\0')
			p = p + 4;

		ret_description_line = palloc (strlen (p) +1);
		strcpy (ret_description_line, p);
	    
		tip->description_line = ret_description_line;
	}
	return (tip->description_line);
}


/*==========================================================================*/

global	int	check_buffer
(
    char			** bufp ,
    unsigned		* sizp ,
    unsigned		minsiz
)
{
	unsigned		newsiz ;
	char			* newbuf ;


	if ( minsiz < * sizp ) return ( 0 );
	/*
	 *  buffer size must be at least one greater for the '\0'.
	 */
	newsiz = ( minsiz / FRAGMENT_SIZE + 1 ) * FRAGMENT_SIZE ;
	if ( * bufp && * sizp )
		newbuf = realloc ( * bufp , newsiz );
	else	newbuf = malloc ( newsiz );
	if ( newbuf == NULL )
	{
		DBG0 (( "\007\r\ninsufficient memory \r\n" ))
		return ( 3 );
	}
	* bufp = newbuf ;
	* sizp = newsiz ;
	return ( 0 );
}

/*==========================================================================*/

global	int	allocate_string
(
    char			** dstp ,
    char			* src
)
{
	unsigned		lgt ;
	char			* p ;


	if ( * dstp && ! strcmp ( * dstp , src ) ) return ( 0 );

	lgt = strlen ( src ) + 1 ; /* include '\0' */
	p = malloc ( lgt );
	if ( p == NULL )
	{
		DBG0 (( "insufficient memory for string \n" ))
		return ( 3 );
	}

	COPY ( p , src , lgt );

	if ( * dstp )
	{
		free ( * dstp );
	}
	* dstp = p ;
	return ( 0 );
}

/*==========================================================================*/

global	char	* repalloc
(
    char            * mem_ptr,
    unsigned		size
)
{
	char			* retval ;


	mem_ptr = (char*) realloc ( mem_ptr, size );
	
	if ( mem_ptr == NULL )
	{
	    DBG0 (( "FATAL: insufficient dynamic memory \n" ))
	    exit ( 3 );
	}

	if ( (unsigned long) mem_ptr % (unsigned long) sizeof(char **) )
	{
	    DBG0 (( "FATAL: malloc returned unaligned pointer 0x%08lx \n",
							(long) mem_ptr ))
	    exit ( 3 );
	}

#ifdef	lint
	retval = (char *) 0 ; /* suppress lint message:
				  'possible pointer alignment problem' */
#else
	retval = (char *) mem_ptr ;
#endif

	return ( retval );
}


/*==========================================================================*/
global	char	* palloc
(
    unsigned		size
)
{
	char	* retval ;


	void * mem_ptr = malloc ( size );

	
	if ( mem_ptr == NULL )
	{
	    DBG0 (( "FATAL: insufficient dynamic memory \n" ))
	    exit ( 3 );
	}

	if ( (unsigned long) mem_ptr % (unsigned long) sizeof(char **) )
	{
	    DBG0 (( "FATAL: malloc returned unaligned pointer 0x%08lx \n",
							(long) mem_ptr ))
	    exit ( 3 );
	}

#ifdef	lint
	retval = (char *) 0 ; /* suppress lint message:
				  'possible pointer alignment problem' */
#else
	retval = (char *) mem_ptr ;
#endif

	return ( retval );
}


/*==========================================================================*/

global	char    *  ascdate
(
    time_t                  clock
)
{
	struct tm               * tim ;
static  char                    line [ LINESIZE ] ;


	if ( clock == 0l ) /* PTS DATEOUPUT */
	{
		FILL  ( line , 0 ,                    sizeof(line)   );
		(void) strncpy ( line , "remake necessary ", sizeof(line)-1 );
		return ( line );
	}

	tim = localtime ( & clock );
	
	(void) sprintf ( line , "%02d-%02d-%02d %02d:%02d:%02d",
		 tim->tm_mday , tim->tm_mon+1 , tim->tm_year <100 ? tim->tm_year : tim->tm_year - 100 ,
		 tim->tm_hour , tim->tm_min , tim->tm_sec );


	return ( line );
}


/*==========================================================================*/

global	char    * versname
(
    char                    version
)
{
	char                    * p ;


	DBG4 (( "versname: version '%c' \n", version ))

	switch ( version )
	{
	case 's' :
			p = stringSLOW ;
			break ;
	case 'q' :
			p = stringQUICK ;
			break ;
	case 'f' :
			p = stringFAST ;
			break ;
	default :
			p = stringEMPTY ;
			break ;
	}

	DBG4 (( "versname: returning '%s' \n", p ))
	return ( p );
}


/*==========================================================================*/

global	int     move_file
(
    char                    * file ,
    char                    * newfile
)
{
	int                     rc ;
	char			line [ LINESIZE ];


	DBG4 (( "move_file: file    '%s' \n", file ))
	DBG4 (( "move_file: newfile '%s' \n", newfile ))

	(void) sprintf ( line , "%s %s %s \n", CMD_MOVE , file , newfile );
	if ( opt_print )
	{
		voutwrite ( line );
	}
	xprotwrite ( line );

	if ( opt_noexec ) return ( 0 );


	/*
	 *  Create the destination directory, if not existent.
	 */
	rc = filelink ( file , newfile );

	if ( opt_keep ) return ( rc );

	if ( ! rc ) rc = fileunlink ( file );

	DBG4 (( "move_file: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/

global	int     link_file
(
    char                    * file ,
    char                    * newfile
)
{
	int                     rc ;
	char			line [ LINESIZE ];
	struct stat fileinfo;
	time_t date;



	DBG4 (( "link_file: file    '%s' \n", file ))
	DBG4 (( "link_file: newfile '%s' \n", newfile ))

	(void) sprintf ( line , "%s %s %s \n", CMD_LINK , file , newfile );
	if ( opt_print )
	{
		voutwrite ( line );
	}
	xprotwrite ( line );

	if ( opt_noexec ) return ( 0 );

#ifdef	unix
	rc = filelink ( file , newfile );
#else
	rc = filestat ( file , & fileinfo );
	if ( rc < 0 )
	{
		DBG0 (( "cannot stat '%s': %s \n", file , serrno() ))
		return ( -2 );
	}
	date = fileinfo.st_mtime ;
	rc = filestat ( newfile , & fileinfo );
	if ((rc != 0) || (fileinfo.st_mtime != date))
	{
		DBG4 (("Force link (%d): %d - %d \n", rc, fileinfo.st_mtime, date))
        rc = filelink ( file , newfile );
	}
#endif

	DBG4 (( "link_file: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/

global	int     unlink_file
(
    char                    * file
)
{
	int                     rc ;
	char			line [ LINESIZE ];


	DBG4 (( "unlink_file: file '%s' \n", file ))

	if ( opt_keep ) return ( 0 );

	(void) sprintf ( line , "%s %s \n", CMD_REMOVE , file );
	if ( opt_print )
	{
		voutwrite ( line );
	}
	xprotwrite ( line );

	if ( opt_noexec ) return ( 0 );

	rc = fileunlink ( file );

	DBG4 (( "unlink_file: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/

static	int     filelink
(
    char                    * file ,
    char                    * newfile
)
{
	int                  rc ;
	struct stat		statbuf ;

	DBG4 (( "filelink: file    '%s' \n", file ))
	DBG4 (( "filelink: newfile '%s' \n", newfile ))

	rc = force_unlink( newfile );
	if ( rc ) return ( rc );

	/*
	 *  Create the destination directory, if not existent.
	 */
	rc = check_dir ( newfile );
	if ( rc ) return ( rc );

#ifdef	unix
	if ( ! is_variant ) /* if variant -> copy (PTS 1002016) */
	{
		rc = link ( file , newfile );
		if ( rc < 0 )
		{
			if ( geterrno() != EXDEV )
			{
				DBG0 (( "cannot link '%s' to '%s': %s \n", file , newfile , serrno() ))
				return ( -2 );		
			}
		}
		else
		{
			DBG8 (( "filelink: linked '%s' to '%s' \n", file , newfile ))
		}
	}
	if (( rc < 0 ) || ( is_variant ))
	{
#endif		
		    /*
		     *  Get the original date.
		     */
		    rc = stat ( file , & statbuf );
		    if ( rc < 0 )
		    {
		        DBG0 (( "cannot stat '%s': %s \n", file , serrno() ))
		        return ( -2 );
		    }

		    rc = copy_file ( file , newfile , &statbuf );
		    if ( rc )
		    {
		        return ( rc );
		    }
		    DBG8 (( "filelink: copied '%s' to '%s' \n", file , newfile ))
#ifdef	unix
	}
#endif

	DBG4 (( "filelink: returning %d \n", 0 ))
	return ( 0 );
}


/*==========================================================================*/

static	int     fileunlink
(
    char                    * file
)
{
	int                     rc ;


	DBG4 (( "fileunlink: file '%s' \n", file ))

	rc = unlink ( file );
	if ( rc < 0 )
	{
	    DBG0 (( "cannot unlink '%s': %s \n", file , serrno() ))
	    return ( -2 );
	}

	DBG4 (( "fileunlink: returning %d \n", 0 ))
	return ( 0 );
}


/*==========================================================================*/

global	int     cmdpgm
(
    char                    * program
)
{


	DBG4 (( "cmdpgm: '%s' \n", program ))

	cmdargc = 0 ;
	cmdlgt = 0 ;
	FILL ( cmdname , 0 , sizeof(cmdname) );
	FILL ( cmd     , 0 , sizeof(cmd)     );

	/* program path */

	(void) strncpy ( cmdname , program , sizeof(cmdname)-1 );

	/* argv [ 0 ] */

	cmdargv [ cmdargc ++ ] = cmd + cmdlgt ;
	(void) sprintf ( cmd + cmdlgt , program );
	cmdlgt += strlen ( cmd + cmdlgt );
	cmdlgt ++ ; /* save null byte */

	return ( 0 );
}


/*==========================================================================*/

global	int     cmdscript
(
    char    * script
)
{
	int             rc=0 ;
	int			    status_dummy ;
	time_t          date ;
	/* va_list			args ; */
	struct stat		statbuf ;
	char            name [ FILENAMESIZE ] ;
	char            * file = NULL;
	valid_bit_arr	vbits ;


	if ( ( strcmp ( script + strlen (script) - strlen(env_TOOLEXT), env_TOOLEXT) != 0) && env_TOOLEXT )
	{
		/* script will be extended by env_TOOLEXT */
		(void) sprintf( name, "%s%s", script, env_TOOLEXT );
	}
	else
		(void) strcpy(name, script);

	DBG4 (( "cmdscr: '%s' \n", name ))
	VALID_BIT_FILL ( vbits );


	if ( env_TOOL )
	{
	    file = (char *) repalloc( file, strlen( env_TOOL ) +
                strlen( name ) + strlen("/bin/") + 1 );
        (void) sprintf ( file , "%s/bin/%s" , env_TOOL , name );
	    rc = stat ( file , &statbuf );
	    if ( rc < 0 )
		    rc = 0 ; /* not found */
	    else
		    rc = 1 ; /* found */
	}

	if ( rc == 0 )
	{
		if ( stat ( script , &statbuf ) >= 0 )
		{	
			file = (char *) repalloc (file, strlen (name) + 1);
			strcpy (file, name);
			rc = 1;
		}
	}

	/*
	 *  If command script not found in $TOOL/bin,
	 *  try VMAKE_PATH.
	 */
	if ( rc == 0 )
	{
		rc = get_file ( name , DIR_TBIN , EMPTY , EMPTY ,
			FK_TOOL | FK_LEVEL , vbits ,
            &file , & date , & status_dummy, 0 );
		if ( rc < 0 )
		{
			DBG4 (( "get file error, returning %d \n", rc ))
			return ( rc );
		}
	}

	if ( rc == 0 )
	{
	    DBG0 (( "missing command script '%s' \n", name ))
	    return ( -2 );
	}

	cmdargv [ cmdargc ++ ] = cmd + cmdlgt ;
	(void) sprintf ( cmd + cmdlgt , file );
	cmdlgt += strlen ( cmd + cmdlgt );
	cmdlgt ++ ; /* save null byte */
	return ( 0 );
}


/* CTS 1105495 -> */
/* cmdarg_obj - replace $OWN/wrk/ with ../../ */
global int cmdarg_obj
( int adjust_doublequotes,
  char version,
  char * obj_file )
{
	static char relative_filename[FILENAMESIZE];
	static char dir_wrk[FILENAMESIZE];
	int         dir_wrk_size;
	char        * pos_obj_file = obj_file;
	char        * vn;

	switch ( version )
	{
	case 'f' :
	case 'q' :
	case 's' :
		break ;
	default :
		version = vmakeversion ;
	}
	vn = versname ( version );

	FILL ( relative_filename, 0 , FILENAMESIZE );
	sprintf (dir_wrk, DIR_WRK, env_OWN);
	dir_wrk_size=strlen(dir_wrk);

	if ( strncmp(dir_wrk,obj_file,dir_wrk_size) == 0)
	{
		strcpy(relative_filename, "../");
		pos_obj_file += dir_wrk_size;
		
		/* add vmake_version for working directory */
		strcat (dir_wrk, vn);

		/* if not the same version */
		if ( strncmp(dir_wrk,obj_file,dir_wrk_size + strlen(vn)) != 0  )
			strcat(relative_filename, "../");
		else
			pos_obj_file += strlen(vn) + 1; /* version + '/' */
	}
	strcat (relative_filename,pos_obj_file);

	return (cmdarg ( adjust_doublequotes, relative_filename));
}
/* <- CTS 1105495 */



/*==========================================================================*/

/**VARARGS1*/
/*PRINTFLIKE1*/
global	int     cmdarg
(
	/* CTS 1103620 */
	int adjust_doublequotes,
    char    * argument ,
    ...
)
{
	int     i , lgt ;
	va_list args ;
	char    line [ LINESIZE ], *c ;

	DBG4 (( "cmdarg: '%s' \n", argument ))

	if ( cmdargc >= MAXCMDARGS || cmdlgt >= CMDBUFFERSIZE || cmdlgt >= WRNCMDBUFFERSIZE )
	{
		for ( i = lgt = 0 ; i < 6 && i < cmdargc && lgt < 40 ; i ++ )
		{
			(void) sprintf ( line + lgt , "%s ", cmdargv [ i ] );
			lgt += strlen ( line + lgt );
		}
		/* PTS 1104090 */
		if ( cmdargc < MAXCMDARGS && cmdlgt < CMDBUFFERSIZE )
			DBG0 (( "Warning: command very long (%d) '%.40s ...' \n", cmdlgt, line ))
		else
		{
			DBG0 (( "command too long '%.40s ...' \n", line ))
			return ( -3 );
		}
	}

	cmdargv [ cmdargc ++ ] = cmd + cmdlgt ;
	
	va_start ( args, argument );
#if unix	
	(void) vsprintf ( cmd + cmdlgt , argument , args );
#else
	/* CTS 1103620 */
	FILL ( line , 0 , sizeof(line) );
    /* it's now in line  */
	(void) vsprintf ( line , argument , args );
	
	for (i=0, c = cmd + cmdlgt ; line[i] ; i++)
	{
		if (( line[i] == '"' ) && adjust_doublequotes)
		{
			*c='\\';
			c++;
		}
		*c=line[i];
		c++;
	}
/*(void) vsprintf ( cmd + cmdlgt , argument , args ); */
#endif
	va_end ( args );
	cmdlgt += strlen ( cmd + cmdlgt );
	cmdlgt ++ ; /* save null byte */
	return ( 0 );
}


/*==========================================================================*/

global	int     cmdexe
(
)
{
    int				rc=0 ;
    int				idx ;
    int				lgt ;
    int				len ;
#if unix
    int				status ;
    int				fd [ 2 ];
#endif
    char			* p ;
    char			line [ LINESIZE + 2 ];
    char			tempc ;
	
	cmdargv [ cmdargc ] = NULL ;

    if ( cmdargc == 0 )
    {
	    DBG0 (( "no command argument for '%s' \n", cmdname ))
	    return ( - 2 );
    }

    for ( idx = 0 ; idx < cmdargc ; idx ++ )
    {
	    if ( idx < 2 )
	    {
		    if ( opt_print )
			    voutwrite ( "%s " , cmdargv [ idx ] );
		    else	
                xprotwrite ( "%s " , cmdargv [ idx ] );
	    }
	    else
	    {
		    if ( opt_print )
			    voutwrite ( "\\\n\t%s " , cmdargv [ idx ] );
		    else	
                xprotwrite ( "\\\n\t%s " , cmdargv [ idx ] );
	    }
    }
    if ( opt_print )
    {
	    voutwrite ( "\n" );
    }
    xprotwrite ( "\n" );

    if ( opt_noexec ) return ( 0 );

#if unix
    rc = pipe ( fd );
    if ( rc < 0 )
    {
	    DBG0 (( "cannot create pipe: %s \n", serrno() ))
	    return ( -2 );
    }

    rc = fork ( );
    if ( rc < 0 )
    {
	    DBG0 (( "cannot fork: %s \n", serrno() ))
	    return ( -2 );
    }

    if ( rc == 0 )  /* son process */
    {
	    (void) close ( 1 );        /* close stdout */
	    (void) close ( 2 );        /* close stderr */
	    (void) dup ( fd [ 1 ] );   /* reopen stdout (returns smallest number available =1 )*/
	    (void) dup ( fd [ 1 ] );   /* reopen stderr (returns smallest number available =2 )*/
	    (void) close ( fd [ 0 ] ); /* don't read from pipe */
	    close_connections ( );     /* don't need server connections */
	    for ( idx = 3 ; idx < 20 ; idx ++ )
	    {
		    (void) close ( idx );  /* close all but stdin,out,err */
	    }

	    (void) execvp ( cmdname , cmdargv );
	    DBG0 (( "execvp error: %s \n", serrno() ))
	    exit_vmake ( 4, TRUE );
	    DBG0 (( "exit returned !!! \n" ))
	    return ( -4 );
    }

    /* father process */

    (void) close ( fd [ 1 ] );            /* don't write to pipe */

    for ( len = 0 ; ; )
    {
	    lgt = read ( fd [ 0 ] , line , sizeof(line)-2 );
	    if ( lgt < 0 )
	    {
	        if ( geterrno() == EINTR ) continue ;
	        voutwrite ( "read error: %s \n", serrno() );
	        break ;
	    }
	    if ( lgt == 0 ) break ;

	    /*
	     *  Break lines after env_CMD_OUTPUT_LENGTH characters.
	     */
	    for ( p = line , idx = 0 ; idx < lgt ; )
		{
	        for ( ; (idx < lgt) && (( env_CMD_OUTPUT_LENGTH <= 0) || (len < env_CMD_OUTPUT_LENGTH))  ; idx ++ , len ++ )
	        {
		        if ( line[idx] == '\n' ) len = 0 ;
	        }
			if ( (len >= env_CMD_OUTPUT_LENGTH)  && ( env_CMD_OUTPUT_LENGTH > 0) )
	        {
				tempc = line[idx] ;
				line[idx] = '\\' ;
				if (opt_silent_mode)
				{
					/* outstamp_add("%s%s",p, "\n");*/
					dowrite_to_protocols ( p , line + idx + 1 - p );
					dowrite_to_protocols ( "\n" , 1 );  
				}
				else
				{
			        dowrite ( p , line + idx + 1 - p );
					dowrite ( "\n" , 1 );  
				}
				
				add_to_outputlist (p, line + idx + 1 - p );
				add_to_outputlist ("\n", 1 );
				line[idx] = tempc ;
		        len = 0 ;
	        }
	        else
	        {
				add_to_outputlist (p, line + idx - p );
				if (!opt_silent_mode)
					dowrite ( p , line + idx - p );
				else
					dowrite_to_protocols ( p , line + idx - p );

				/*
				else
					outstamp_add("%s",p);
				*/
	        }
	        p = line + idx ;
	    }
    }

    (void) close ( fd [ 0 ] );

    rc = wait ( &status );
    if ( rc < 0 )
    {
	    DBG0 (( "wait error: %s \n", serrno() ))
	    return ( -2 );
    }

    if ( status == 0 ) 	return ( 0 );

    if ( ( status & 0xFF ) == 0 )
    {
	    /* normal process exit */
	    rc = status >> 8 & 0xFF ; /* get high order eight bits */
    }
    else
    {
	    if ( ( status >> 8 & 0xFF ) == 0 )
	    {
		    /* terminated due to a signal */
		    rc = status & 0xFF ;
	    }
	    else
	    {
		    /* process stopped */
		    rc = status >> 8 & 0xFF ;
	    }
    }
#else /*unix*/

    {
    DWORD			dwRead ;
    HANDLE			hChildStdoutRd ;
    HANDLE			hChildStdoutWr ;
    HANDLE			hChildStderrWr ;
    HANDLE			hSaveStdout ;
    HANDLE			hSaveStderr ;
    SECURITY_ATTRIBUTES		saAttr ;
    PROCESS_INFORMATION		piProcInfo ;
    STARTUPINFO			siStartInfo ;
	DWORD EventCount ;
	HANDLE          WaitForEndHandles[2];

	WaitForEndHandles[0] = hStopRequest;
    /*
     *  Remove the nul characters from the command string.
     */
    for ( idx = 1 ; idx < cmdargc ; idx ++ )
    {
    	*(cmdargv[idx] - 1) = ' ' ;
    }

    /*
     *  Set the bInheritHandle flag so pipe handles are inherited.
     */
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = TRUE;
    saAttr.lpSecurityDescriptor = NULL;

    /*
     * The steps for redirecting child's STDOUT:
     *     1.  Save current STDOUT, to be restored later.
     *     2.  Create anonymous pipe to be STDOUT for child.
     *     3.  Set STDOUT of parent to be write handle of pipe, so
     *         it is inherited by child.
     */

    /*
     *  Create a pipe for the child's STDOUT.
     */
    if (!CreatePipe(&hChildStdoutRd, &hChildStdoutWr, &saAttr, 0))
    {
	    DBG0 (( "cannot create pipe: %s \n", serrno() ))
	    return ( -2 );
    }

    /*
     *  Create a duplicated pipe for the child's STDERR (duplicated from child's STDOUT)
     */
	if (! DuplicateHandle(
				GetCurrentProcess(),	// handle to process with handle to duplicate
				hChildStdoutWr,	// handle to duplicate
				GetCurrentProcess(),	// handle to process to duplicate to
				&hChildStderrWr,	// pointer to duplicate handle
				DUPLICATE_SAME_ACCESS,	// access for duplicate handle
				TRUE,	// handle inheritance flag
				DUPLICATE_SAME_ACCESS 	// optional actions
				) )
	{
	    DBG0 (( "cannot duplicate pipe: %s \n", serrno() ))
	    (void) CloseHandle(hChildStdoutWr);
	    (void) CloseHandle(hChildStdoutRd);
	    return ( -2 );
	}


    /*
     *  Save the handle to the current STDOUT.
     */
    hSaveStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    hSaveStderr = GetStdHandle(STD_ERROR_HANDLE);

    /*
     *  Set a write handle to the pipe to be STDOUT and STDERR.
     */
    if (!SetStdHandle(STD_OUTPUT_HANDLE, hChildStdoutWr))
    {
	    DBG0 (( "cannot redirect stdout: %s \n", serrno() ))
	    (void) CloseHandle(hChildStdoutWr);
	    (void) CloseHandle(hChildStdoutRd);
	    (void) CloseHandle(hChildStderrWr);
	    return ( -2 );
    }
    if (!SetStdHandle(STD_ERROR_HANDLE, hChildStderrWr))
    {
	    DBG0 (( "cannot redirect stderr: %s \n", serrno() ))
	    (void) CloseHandle(hChildStdoutWr);
	    (void) CloseHandle(hChildStdoutRd);
	    (void) CloseHandle(hChildStderrWr);
	    return ( -2 );
    }

    /*
     *  Set up members of STARTUPINFO structure.
     */
    FILL ( &siStartInfo , 0 , sizeof(STARTUPINFO) );
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.lpReserved = NULL;
    siStartInfo.lpReserved2 = NULL;
    siStartInfo.cbReserved2 = 0;
    siStartInfo.lpDesktop = NULL;
    siStartInfo.dwFlags = 0;

    /*
     *  Now create the child process.
     */
    rc = CreateProcess( NULL ,		/* program                            */
			cmd ,		/* command line                       */
			NULL ,		/* process security attributes        */
			NULL ,		/* primary thread security attributes */
			TRUE ,	/*	handles are inherited              */
			0 ,		/* creation flags                     */
			NULL ,		/* use parent's environment           */
			NULL ,		/* use parent's current directory     */
			&siStartInfo ,	/* STARTUPINFO pointer                */
			&piProcInfo ); 	/* receives PROCESS_INFORMATION       */

    /*
     *  After process creation, restore the saved STDOUT and STDERR.
     */

	hChildProcess = piProcInfo.hProcess;

    if (!SetStdHandle(STD_ERROR_HANDLE, hSaveStderr))
    {
	    DBG0 (( "cannot redirect stderr: %s \n", serrno() ))
	    (void) CloseHandle(hChildStdoutWr);
	    (void) CloseHandle(hChildStdoutRd);
	    (void) CloseHandle(hChildStderrWr);
	    return ( -2 );
    }
    if (!SetStdHandle(STD_OUTPUT_HANDLE, hSaveStdout))
    {
	    DBG0 (( "cannot redirect stdout: %s \n", serrno() ))
	    (void) CloseHandle(hChildStdoutWr);
	    (void) CloseHandle(hChildStdoutRd);
	    (void) CloseHandle(hChildStderrWr);
	    return ( -2 );
    }

    /*
     *  If the process creation failed, give an error message
     *  AFTER stderr has been restored!
     */
    if ( ! rc )
    {
	    DBG0 (( "cannot create process: %s \n", serrno() ))
	    DBG0 (( "commandline was: %s \n", cmd ))
	    (void) CloseHandle(hChildStdoutWr);
	    (void) CloseHandle(hChildStdoutRd);
	    (void) CloseHandle(hChildStderrWr);
	    return ( -2 );
    }

    /*
     *  Close the write end of the pipe before reading from the
     *  read end of the pipe.
     */
    (void) CloseHandle(hChildStdoutWr);
    (void) CloseHandle(hChildStderrWr);

    /*
     *  Read from pipe that is the standard output for child process.
     */
    for ( len = 0 ; ; )
    {
		if (!ReadFile(hChildStdoutRd, (CHAR*) line , LINESIZE, &dwRead, NULL))
	    {
	        if ( GetLastError() != ERROR_BROKEN_PIPE )
    		    voutwrite ( "read error: %s \n", serrno() );
	        else
	    	    dwRead = 0 ;
	        break ;
	    }
	    lgt = dwRead ;
	    if ( lgt == 0 ) break ;

	    /*
	     *  Break lines after env_CMD_OUTPUT_LENGTH characters.
	     */
	    for ( p = line , idx = 0 ; idx < lgt ; )
	    {
	        for ( ; (idx < lgt) && (( env_CMD_OUTPUT_LENGTH <= 0) || (len < env_CMD_OUTPUT_LENGTH)); idx ++ , len ++ )
	        {
		        if ( line[idx] == '\n' )
                {
                    len = 0 ;
#ifdef WIN32
                    /*line[idx-1] = '\n'; */ /* 0x0A */
                    line[idx-1] = ' '; /* 0x0A */
#endif
                }
	        }
			if ( (len >= env_CMD_OUTPUT_LENGTH)  && ( env_CMD_OUTPUT_LENGTH > 0) )
	        {
				tempc = line[idx] ;
				line[idx] = '\\' ;
				if (opt_silent_mode)
				{
				/*	outstamp_add("%s%s",p, "\n"); */
					dowrite_to_protocols ( p , line + idx + 1 - p );
					dowrite_to_protocols ( "\n" , 1 );  
				}
				else
				{
			        dowrite ( p , line + idx + 1 - p );
					dowrite ( "\n" , 1 );  
				}
				
				add_to_outputlist (p, line + idx + 1 - p );
				add_to_outputlist ("\n", 1 );
				line[idx] = tempc ;
		        len = 0 ;
	        }
	        else
	        {
				add_to_outputlist (p, line + idx - p );
				if (!opt_silent_mode)
					dowrite ( p , line + idx - p );
				else
					dowrite_to_protocols ( p , line + idx - p );
				/*
				else
					outstamp_add("%s",p);
				*/	
	        }
	        p = line + idx ;
	    }
    }

    /*
     *  Close the read end of the pipe.
     */
    (void) CloseHandle(hChildStdoutRd);
    
	WaitForEndHandles[1] = piProcInfo.hProcess;

	EventCount = WaitForMultipleObjects( 2, WaitForEndHandles, FALSE, INFINITE );
	if ( EventCount == WAIT_FAILED )
	{
		DBG0 (( "cannot wait for process: %s \n", serrno() ))
		hChildProcess = NULL;
	    return ( -2 );
    }
	if (EventCount == 0)
	{
/*	    DBG1 (("Termination requested by extern event. Waiting for end of called system command\n"))
		if ( WaitForSingleObject ( piProcInfo.hProcess , INFINITE ) == WAIT_FAILED )
		{
			DBG0 (( "cannot wait for process: %s \n", serrno() ))
		}
		*/
		Sleep (500);
		ExitThread(0);
	}
	else
	{
		hChildProcess = NULL;
		DBG3 (("system call finished\n"))
	}

    if ( ! GetExitCodeProcess ( piProcInfo.hProcess , &dwRead ) )
    {
	    DBG0 (( "cannot get process exit code: %s \n", serrno() ))
	    return ( -2 );
    }
    if ( ! CloseHandle ( piProcInfo.hThread ) )
    {
    	DBG0 (( "cannot close thread handle: %s \n", serrno() ))
    }
    if ( ! CloseHandle ( piProcInfo.hProcess ) )
    {
    	DBG0 (( "cannot close process handle: %s \n", serrno() ))
    }
    if ( dwRead == 0 ) return ( 0 );
	rc = dwRead ;
    }

#endif /*unix*/

	if (opt_silent_mode)
		outstamp_add("COMMAND error %d \n", rc);
	else
		DBG0 (( "COMMAND error %d \n", rc ))

    return ( -2 );
}


/*==========================================================================*/

global	void	set_protocol
(
    char                    * trgname ,
    char                    version
)
{
	int			        cnt ;
	time_t              clock ;
	char		        * pdate ;
	char		        * pname ;
	char		        * pvers ;
	char                name [ FILENAMESIZE ] ;
	char                * file = NULL;
	char		        copybuf [ BUFSIZ ];


	DBG4 (( "setprot: trgname '%s' \n", trgname ))
	DBG4 (( "setprot: version '%c' \n", version ))
	
	for (;*trgname==':';trgname++); /* cut ':' on the protocolname  PTS 1002076*/

	if ( version == ' ' || version == 'n' ) version = vmakeversion ; /* CTS 1110258 */

	if ( ! strcmp (trgname , protname) && (version == protvers) )
	{
	    DBG4 (( "setprot: already opened '%s' \n", trgname ))
	    return ;
	}

	/*
	 *  Close current protocol, if open.
	 */
	if ( protfp || xprotfp || xprotfp )
	{
	    clock = time ( (time_t*) 0 );
	    pdate = ascdate(clock);
	    pname = protname ;
	    pvers = versname(protvers);
	    if ( xprotfp == sxprotfp )
	    {
		    pname = vmake_coll ;
		    pvers = "" ;
	    }
	    if ( terminating )
		    voutwrite (
		        "%s terminating  %s  because of communication errors \n",
		        pdate , pname );
	    else
		voutwrite ( "%s end    of    %s   %s \n",
		    pdate , pname , pvers );

	    if ( protfp && (protfp != sprotfp) ) (void) fclose ( protfp );
	    if ( xprotfp && (xprotfp != sxprotfp) ) (void) fclose ( xprotfp );
		if ( eprotfp && (eprotfp != seprotfp) ) (void) fclose ( eprotfp );
		if ( opt_warning_protocol && wprotfp && (wprotfp != swprotfp) ) (void) fclose ( wprotfp );
	    protfp = NULL ;
	    xprotfp = NULL ;
		eprotfp = NULL ;
		wprotfp = NULL ;
	}

	if ( ! * trgname )
	{
	    /*
	     *  Empty target name: close all protocols.
	     *  Normal and extended prots are closed above.
	     *  Startprot is open during the whole vmake run.
	     */
	    if ( sprotfp )
	    {
		    DBG4 (( "setprot: empty protocol name, closing startprot \n" ))
		    (void) fclose ( sprotfp );
		    sprotfp = NULL ;
	    }
	    if ( sxprotfp )
	    {
		    DBG4 (( "setprot: empty protocol name, closing xstartprot \n" ))
		    (void) fclose ( sxprotfp );
		    sxprotfp = NULL ;
	    }
	    if ( mprotfp )
	    {
		    DBG4 (( "setprot: empty protocol name, closing mprot \n" ))
		    (void) fclose ( mprotfp );
		    mprotfp = NULL ;
	    }

		/* CTS 1112006 -> */
		if ( seprotfp )
	    {
		    DBG4 (( "setprot: empty protocol name, closing errorprot \n" ))
		    (void) fclose ( seprotfp );
		    seprotfp = NULL ;
	    }
		/* <- CTS 1112006 */

		if ( swprotfp  && opt_warning_protocol)
	    {
		    DBG4 (( "swtprot: empty protocol name, closing wrnprot \n" ))
		    (void) fclose ( swprotfp );
		    swprotfp = NULL ;
	    }

	    return ;
	}

	if ( opt_noprot )
	{
	    if ( trgname == vmake_coll ) /* pointer comparison */
	    {
    		print_env_opt ();
	    }
	    DBG4 (( "setprot: no execution \n" ))
	    return ;
	}

	/*
	 *  Save old protocol.
	 */
	delete_protocol ( trgname , version );
	FILL ( protname , 0 , sizeof(protname) );

	clock = time ( (time_t*) 0 );

	if ( trgname == vmake_coll ) /* pointer comparison */
	{

        sprotfp = get_tmp_file ();
		
	    if ( ! sprotfp )
	    {
		    DBG0 (( "error opening tempfile for startprot \n" ))
		    return ;
	    }
	    (void) setvbuf ( sprotfp , (char*)0 , _IOFBF , BUFSIZ );
	    protfp = sprotfp ;
	    DBG7 (( "setprot: opened tempfile for startprot \n" ))

		/* CTS 1112006 -> */
		seprotfp = get_tmp_file ();
	    if ( ! seprotfp )
	    {
		    DBG0 (( "error opening tempfile for estartprot \n" ))
		    return ;
	    }
	    (void) setvbuf ( seprotfp , (char*)0 , _IOFBF , BUFSIZ );
	    eprotfp = seprotfp ;
	    DBG7 (( "setprot: opened tempfile for estartprot \n" ))
		/* <- CTS 1112006 */

		if (opt_warning_protocol)
		{
			swprotfp = get_tmp_file ();
			if ( ! swprotfp )
			{
				DBG0 (( "error opening tempfile for wstartprot \n" ))
				return ;
			}
			(void) setvbuf ( swprotfp , (char*)0 , _IOFBF , BUFSIZ );
			wprotfp = swprotfp ;
			DBG7 (( "setprot: opened tempfile for wstartprot \n" ))
		}	

	    sxprotfp = get_tmp_file ();
	    if ( ! sxprotfp )
	    {
		    DBG0 (( "error opening tempfile for xstartprot \n" ))
		    return ;
	    }
	    (void) setvbuf ( sxprotfp , (char*)0 , _IOFBF , BUFSIZ );
	    xprotfp = sxprotfp ;
	    DBG7 (( "setprot: opened tempfile for xstartprot \n" ))

	    print_env_opt ();
	    voutwrite ( "%s start  of    %s \n", ascdate(clock) , trgname );

		eprotwrite ("<html><head><meta http-equiv=\"Content-Type\" content=\"text/html\"></head><body ><PRE><PLAINTEXT>\n");
		eprotwrite ("------------------------------------------------------------\n");
		eprotwrite ("PHASE: COLLECTING (%s started)\n", ascdate(clock));
		eprotwrite ("------------------------------------------------------------\n\n");
	}
	else
	{
	    (void) strncpy ( protname , trgname , sizeof(protname)-1 );

	    /* normal protokoll */
        (void) sprintf ( name , "%.*s.p0", MAXFNMLGT-3 , protname );
	    (void) new_file ( name , DIR_PROT , versname(version) ,
							EMPTY , &file );
	    DBG7 (( "setprot: normal prot file '%s' \n", file ))
        check_dir( file );
        protfp = fopen ( file , "w" );
	    if ( ! protfp )
	    {
		    DBG0 (( "error opening '%s' \n", file ))
		    return ;
	    }
	    setbuf ( protfp , (char*)0 );
	    DBG7 (( "setprot: opened '%s' \n", file ))

	    /* extended protokoll */
	    (void) sprintf ( name , "%.*s.x0", MAXFNMLGT-3 , protname );
	    (void) new_file ( name , DIR_PROT , versname(version) ,
							EMPTY , &file );
	    DBG7 (( "setprot: extend prot file '%s' \n", file ))
	    xprotfp = fopen ( file , "w" );
	    if ( ! xprotfp )
	    {
		    DBG0 (( "error opening '%s' \n", file ))
		    return ;
	    }
	    setbuf ( xprotfp , (char*)0 );
	    DBG7 (( "setprot: opened '%s' \n", file ))

	    /* module protokoll + CTS 1111841 */
		(void) sprintf ( name , "%.*s.%s.m0", MAXFNMLGT-12 , "Modules", env_BUILDPID ? env_BUILDPID : "" );
		(void) new_file ( name , DIR_PROT , EMPTY /* versname(version) */ ,
							EMPTY , &file );
	    DBG7 (( "setprot: module prot file '%s' \n", file ))
	    mprotfp = fopen ( file , "a" );
	    if ( ! mprotfp )
	    {
		    DBG0 (( "error opening '%s' \n", file ))
		    return ;
	    }
	    setbuf ( mprotfp , (char*)0 );
	    DBG7 (( "setprot: opened '%s' \n", file ))

		/* CTS 1112006 -> */
		(void) sprintf ( name , "%.*s.e0", MAXFNMLGT-3 , protname );
	    (void) new_file ( name , DIR_PROT , versname(version) ,
							EMPTY , &file );
	    DBG7 (( "setprot: error prot file '%s' \n", file ))
	    eprotfp = fopen ( file , "w" );
	    if ( ! eprotfp )
	    {
		    DBG0 (( "error opening '%s' \n", file ))
		    return ;
	    }
	    setbuf ( eprotfp , (char*)0 );
	    DBG7 (( "setprot: opened '%s' \n", file ))
		/* <- CTS 1112006 */

		if (opt_warning_protocol)
		{
			(void) sprintf ( name , "%.*s.w0", MAXFNMLGT-3 , protname );
			(void) new_file ( name , DIR_PROT , versname(version) ,
								EMPTY , &file );
			DBG7 (( "setprot: warning prot file '%s' \n", file ))
			wprotfp = fopen ( file , "w" );
			if ( ! wprotfp )
			{
				DBG0 (( "error opening '%s' \n", file ))
				return ;
			}
			setbuf ( wprotfp , (char*)0 );
			DBG7 (( "setprot: opened '%s' \n", file ))
		}
		
	    protvers = version ;

	    if ( sprotfp )
	    {
		    DBG6 (( "setprot: copying protocol header ...\n" ))
		    rewind ( sprotfp );
		    while ( (cnt = fread(copybuf , 1 , BUFSIZ , sprotfp)) != 0 )
		        (void) fwrite ( copybuf , 1 , cnt , protfp );

		    (void) fprintf ( mprotfp , "%s\n", vmake_version );
		    (void) fprintf ( mprotfp , "%s %s %c\n", ascdate(clock),
                                          trgname , version);
            (void) fprintf ( mprotfp, "%s\n", env_VMAKE_PATH);
	    }
	    if ( sxprotfp )
	    {
		    DBG6 (( "setprot: copying xprotocol header ...\n" ))
		    rewind ( sxprotfp );
		    while ( (cnt = fread(copybuf , 1 , BUFSIZ , sxprotfp)) != 0 )
		        (void) fwrite ( copybuf , 1 , cnt , xprotfp );
	    }
        if ( seprotfp )
	    {
		    DBG6 (( "setprot: copying eprotocol header ...\n" ))
		    rewind ( seprotfp );
		    while ( (cnt = fread(copybuf , 1 , BUFSIZ , seprotfp)) != 0 )
		        (void) fwrite ( copybuf , 1 , cnt , eprotfp );
	    }

		if ( opt_warning_protocol && swprotfp )
	    {
		    DBG6 (( "setprot: copying eprotocol header ...\n" ))
		    rewind ( swprotfp );
		    while ( (cnt = fread(copybuf , 1 , BUFSIZ , swprotfp)) != 0 )
		        (void) fwrite ( copybuf , 1 , cnt , wprotfp );
	    }

        voutwrite ( "%s start  of    %s   %s \n",
			ascdate(clock) , protname , versname(protvers) );

		eprotwrite ("------------------------------------------------------------\n");
		eprotwrite ("PHASE: MAKE    TARGET: %s   VERSION:%s\n", protname , versname(protvers) );
		eprotwrite ("------------------------------------------------------------\n\n");
	}

    FREE_AND_SET_TO_NULL(file);

	DBG4 (( "setprot: returning \n" ))
	return ;
}


/*==========================================================================*/

global	void	delete_protocol
(
    char                    * trgname ,
    char                    version
)
{
	int			idx ;
	char                    name [ FILENAMESIZE ] ;
	char                    * file = NULL ;
	char                    oldname [ FILENAMESIZE ] ;
	char                    * oldfile = NULL;


	DBG4 (( "delprot: trgname '%s' \n", trgname ))

	if ( opt_noprot )
	{
		DBG4 (( "delprot: no execution \n" ))
		return ;
	}

	if ( version == ' ' ) version = vmakeversion ;

	if ( strcmp ( trgname , protname ) == 0 && version == protvers )
	{
		DBG4 (( "delprot: current protocol '%s' \n", protname ))
		return ;
	}

	if ( trgname [ 0 ] == '\0' )
	{
		DBG4 (( "delprot: no protocol \n" ))
		return ;
	}

	for ( idx = 9 ; idx > 0 ; idx -- )
	{
        /* normal protokoll */
	    (void) sprintf ( name    , "%.*s.p%d", MAXFNMLGT-3 ,
						trgname , idx - 1 );
	    (void) sprintf ( oldname , "%.*s.p%d", MAXFNMLGT-3 ,
						trgname , idx );
	    (void) new_file ( name    , DIR_PROT , versname ( version ) ,
							    EMPTY , &file );
	    (void) new_file ( oldname , DIR_PROT , versname ( version ) ,
							    EMPTY , &oldfile );
	    DBG7 (( "delprot: move '%s' to '%s' \n", file , oldfile ))
	    (void) unlink ( oldfile );
	    (void) link ( file , oldfile );
	    (void) unlink ( file );

        /* extended protokoll */
        (void) sprintf ( name    , "%.*s.x%d", MAXFNMLGT-3 ,
						trgname , idx - 1 );
	    (void) sprintf ( oldname , "%.*s.x%d", MAXFNMLGT-3 ,
						trgname , idx );
	    (void) new_file ( name    , DIR_PROT , versname ( version ) ,
							    EMPTY , &file );
	    (void) new_file ( oldname , DIR_PROT , versname ( version ) ,
							    EMPTY , &oldfile );
	    DBG7 (( "delprot: move '%s' to '%s' \n", file , oldfile ))
	    (void) unlink ( oldfile );
	    (void) link ( file , oldfile );
	    (void) unlink ( file );

		/* error protokoll */
        (void) sprintf ( name    , "%.*s.e%d", MAXFNMLGT-3 ,
						trgname , idx - 1 );
	    (void) sprintf ( oldname , "%.*s.e%d", MAXFNMLGT-3 ,
						trgname , idx );
	    (void) new_file ( name    , DIR_PROT , versname ( version ) ,
							    EMPTY , &file );
	    (void) new_file ( oldname , DIR_PROT , versname ( version ) ,
							    EMPTY , &oldfile );
	    DBG7 (( "delprot: move '%s' to '%s' \n", file , oldfile ))
	    (void) unlink ( oldfile );
	    (void) link ( file , oldfile );
	    (void) unlink ( file );

		/* warning protokoll */
		(void) sprintf ( name    , "%.*s.w%d", MAXFNMLGT-3 ,
						trgname , idx - 1 );
	    (void) sprintf ( oldname , "%.*s.w%d", MAXFNMLGT-3 ,
						trgname , idx );
	    (void) new_file ( name    , DIR_PROT , versname ( version ) ,
							    EMPTY , &file );
	    (void) new_file ( oldname , DIR_PROT , versname ( version ) ,
							    EMPTY , &oldfile );
	    DBG7 (( "delprot: move '%s' to '%s' \n", file , oldfile ))
	    (void) unlink ( oldfile );
	    (void) link ( file , oldfile );
	    (void) unlink ( file );


        if ( ( trgname != vmake_coll ) && ! mprotfp )
        {
            /* module protokoll */
            (void) sprintf ( name    , "%.*s.%s.m%d", MAXFNMLGT-12 ,
							"Modules", env_BUILDPID ? env_BUILDPID : "", idx - 1 );

			(void) sprintf ( oldname    , "%.*s.%s.m%d", MAXFNMLGT-12 ,
							"Modules", env_BUILDPID ? env_BUILDPID : "", idx );
			
	        (void) new_file ( name    , DIR_PROT , EMPTY /* versname ( version ) */ ,
							        EMPTY , &file );
	        (void) new_file ( oldname , DIR_PROT , EMPTY /* versname ( version ) */ ,
							        EMPTY , &oldfile );
	        DBG7 (( "delprot: move '%s' to '%s' \n", file , oldfile ))
	        (void) unlink ( oldfile );
	        (void) link ( file , oldfile );
	        (void) unlink ( file );
        }

	}

    FREE_AND_SET_TO_NULL(file);
    FREE_AND_SET_TO_NULL(oldfile);

	DBG4 (( "delprot: returning \n" ))
	return ;
}


/*==========================================================================*/

static  void	print_env_opt
(
)
{
    struct path_info		* pip ;
    char			** argv ;
    char			* p ;
    char			opt [ 100 ];

    /* version */

    DBG0 (( "%s\n", vmake_version + 4 ))
    DBG1 (( "\n" ))

    /* environment */
	DBG1 (( "BUILDSTRING     =%s\n", env_BUILDSTRING     ? env_BUILDSTRING     : "" )) /* CTS 1102333 */
	DBG1 (( "RELVER          =%s\n", env_RELVER          ? env_RELVER          : "" ))
    DBG1 (( "CORRECTION_LEVEL=%s\n", env_CORRECTION_LEVEL? env_CORRECTION_LEVEL: "" ))
	DBG1 (( "BUILDPRAEFIX    =%s\n", env_BUILDPRAEFIX    ? env_BUILDPRAEFIX    : "" ))
    DBG1 (( "TOOL            =%s\n", env_TOOL            ? env_TOOL            : "" ))
    DBG1 (( "INSTROOT        =%s\n", env_INSTROOT        ? env_INSTROOT        : "" ))
    DBG1 (( "SHELL           =%s\n", env_SHELL           ? env_SHELL           : "" ))
    DBG1 (( "TOOLSHELL       =%s\n", env_TOOLSHELL       ? env_TOOLSHELL       : "" ))
	DBG1 (( "OSSPEC          =%s\n", env_OSSPEC          ? env_OSSPEC          : "" ))
	DBG1 (( "MACH            =%s\n", env_MACH            ? env_MACH            : "" ))
	DBG1 (( "BIT64           =%s\n", env_BIT64           ? env_BIT64           : "0"))
	DBG1 (( "DLL_TYPE        =%s\n", env_DLL_TYPE        ? env_DLL_TYPE        : "" ))
	DBG1 (( "ARC_TYPE        =%s\n", env_ARC_TYPE        ? env_ARC_TYPE        : "" ))
    DBG1 (( "VMAKE_VERSION   =%s\n", env_VMAKE_VERSION   ? env_VMAKE_VERSION   : "" ))
    DBG1 (( "VMAKE_DEFAULT   =%s\n", env_VMAKE_DEFAULT   ? env_VMAKE_DEFAULT   : "" ))
    DBG1 (( "VMAKE_OPTION    =%s\n", env_VMAKE_OPTION    ? env_VMAKE_OPTION    : "" ))
    DBG0 (( "VMAKE_PATH      =%s\n", env_VMAKE_PATH      ? env_VMAKE_PATH      : "" ))

    DBG1 (( "\n" ))

    /* command line */

    for ( argv = opt_argv ; * argv ; argv ++ ) DBG1 (( "%s ", * argv ))
    DBG1 (( "\n" ))

    /* command line options */

    p = opt ;

	if ( opt_background  ) * (p ++) = 'b' ;
    if ( opt_collection  ) * (p ++) = 'c' ;
    if ( dbgdbglvl > 1   ) * (p ++) = 'D' ;
	if ( opt_nodep       ) * (p ++) = 'd' ;
    if ( dbgdbglvl > 1   ) * (p ++) = '0' + dbgdbglvl - 1 ;
	if ( opt_optdebug    ) * (p ++) = 'E' ;
	if ( opt_debug       ) * (p ++) = 'e' ;
	if ( opt_distrib     ) * (p ++) = 'F' ;
    if ( opt_global      ) * (p ++) = 'g' ;
    if ( opt_uncondincl  ) * (p ++) = 'I' ;
    if ( opt_interactive ) * (p ++) = 'i' ;
    if ( opt_lint        ) * (p ++) = 'L' ;
    if ( opt_local       ) * (p ++) = 'l' ;
    if ( opt_keep & 1    ) * (p ++) = 'k' ;
    if ( opt_keep & 2    ) * (p ++) = 'K' ;
    if ( opt_man         ) * (p ++) = 'M' ;
	if ( opt_modlist     ) * (p ++) = 'm' ;
    if ( opt_noprot      ) * (p ++) = 'N' ;
	if ( opt_noexec      ) * (p ++) = 'n' ;
	if ( opt_profile     ) * (p ++) = 'p' ;
	if ( opt_retrieve    ) * (p ++) = 'r' ;
	if ( opt_sizeof      ) * (p ++) = 'S' ;
	if ( opt_nameduncond ) * (p ++) = 'u' ;
    if ( opt_uncond      ) * (p ++) = 'U' ;
	if ( opt_watch       ) * (p ++) = 'w' ;
	if ( opt_print       ) * (p ++) = 'x' ;
	if ( yydebug         ) * (p ++) = 'Y' ;
	
    * p = '\0' ;
    DBG1 (( "vmake command line options: -%s\n", opt ))

    DBG1 (( "vmake version: %c\n", vmakeversion ))
    DBG1 (( "vmake default: %c\n", getdefvers( vmakeversion ) ))

    /* data computed from options and environment */

    p = opt ;

    if ( vopt_local_objcopy ) * (p ++) = 'A' ;
	if ( vopt_no_case_sens_error ) * (p ++) = 'a' ;
	if ( vopt_c_includes_p  ) * (p ++) = 'b' ;
	if ( vopt_level_c & 1   ) * (p ++) = 'c' ;
    if ( vopt_level_c & 2   ) * (p ++) = 'C' ;
	if ( vopt_current_date  ) * (p ++) = 'd' ;
    if ( vopt_dates         ) * (p ++) = 'D' ;
	if ( opt_debug          ) * (p ++) = 'e' ;
	if ( vopt_fulloutput    ) * (p ++) = 'f' ;
	if ( vopt_global        ) * (p ++) = 'g' ;
    if ( vopt_prefer_shm    ) * (p ++) = 'h' ;
	if ( vopt_no_auto_hdr   ) * (p ++) = 'H' ;
	if ( vopt_level_i & 1   ) * (p ++) = 'i' ;
    if ( vopt_level_i & 2   ) * (p ++) = 'I' ;
    if ( vopt_mod_only      ) * (p ++) = 'M' ;
	if ( vopt_reloc_lib     ) * (p ++) = 'L' ;
    if ( vopt_local         ) * (p ++) = 'l' ;
    if ( vopt_level_p & 1   ) * (p ++) = 'p' ;
    if ( vopt_level_p & 2   ) * (p ++) = 'P' ;
	if ( vopt_level_o & 1   ) * (p ++) = 'o' ;
    if ( vopt_level_o & 2   ) * (p ++) = 'O' ;
	if ( vopt_ignore_shrglob) * (p ++) = 'R' ;
    if ( vopt_level_s & 1   ) * (p ++) = 's' ;
    if ( vopt_level_s & 2   ) * (p ++) = 'S' ;
    if ( vopt_timestamp & 1 ) * (p ++) = 't' ;
    if ( vopt_timestamp & 2 ) * (p ++) = 'T' ;
	if ( vopt_source_warn   ) * (p ++) = 'v' ;
    if ( vopt_source_copy   ) * (p ++) = 'V' ;
    if ( vopt_object_warn   ) * (p ++) = 'w' ;
    if ( vopt_object_copy   ) * (p ++) = 'W' ;
    /* CTS 1103400 */
	if ( vopt_rw_permission ) * (p ++) = 'X' ;
	if ( vopt_no_ext_map    ) * (p ++) = 'Z' ;
	
	* p = '\0' ;
    DBG1 (( "vmake option: %s\n", opt ))

    DBG1 (( "vmake path:\n" ))
    for ( pip = vmake_path ; pip ; pip = pip->next )
    {
	switch ( pip->path_status & PATH_TYPE )
	{
	case PATH_OWN :
	    opt[0] = 'O' ;
	    break ;
	case PATH_LOCAL :
	    opt[0] = 'L' ;
	    break ;
	case PATH_REMOTE :
	    opt[0] = 'R' ;
	    break ;
	default :
	    opt[0] = '?' ;
	    break ;
	}
	opt[1] = 'd' ;
	opt[2] = 's' ;
	if ( pip->host_permission & HOST_OBJECTS_DENIED )
	    opt[3] = '-' ;
	else
	    opt[3] = 'o' ;
	if ( pip->host_permission & HOST_TOOLS_DENIED )
	    opt[4] = '-' ;
	else
	    opt[4] = 't' ;
	opt[5] = 0 ;
	DBG1 (( "\t%s\t%s\n", opt , pip->path ))
    }
    DBG1 (( "\n" ))

}

/*==========================================================================*/

global	int     set_work_dir
(
    char                    version
)
{
	int                     rc=0 ;
	char                    * vn , line [ LINESIZE ];

	DBG4 (( "setwrkdir: version '%c' \n", version ))

	switch ( version )
	{
	case 'f' :
	case 'q' :
	case 's' :
		break ;
	default :
		version = vmakeversion ;
	}
	vn = versname ( version );

	/*
	 *  put VMAKE_VERSION to the environment.
	 *  WARNING: don't use a automatic variable with 'putenv'!
	 */
	(void) sprintf ( vmakeversion_env , "CURR_VERSION=%s" , vn );
	rc = putenv ( vmakeversion_env );
	if ( rc )
	{
		DBG0 (( "cannot putenv '%s' \n", vmakeversion_env ))
		return ( rc );
	}


    ENV_TO_SET = (char*) palloc (strlen("SUPPRESS_VMAKE_PIDFILE=y")+1);
    strcpy( ENV_TO_SET, "SUPPRESS_VMAKE_PIDFILE=y");
    rc = putenv( ENV_TO_SET );
    if ( rc )
    {
        DBG0 (( "cannot set environment variable (%s): %s \n",
                    ENV_TO_SET , serrno() ))
        return ( - 2 );
    }

	(void) sprintf ( line , DIR_TMP , vmake_path->path , vn );
	if ( strcmp ( line , tmp_dir ) == 0 )
	{
		DBG4 (( "setwrkdir: work dir already correct '%s' \n", line ))
		return ( 0 );
	}
	(void) strncpy ( tmp_dir , line , sizeof(tmp_dir)-1 );
	DBG7 (( "setwrkdir: tmp_dir '%s' \n", tmp_dir ))

	(void) sprintf ( line , "%s %s \n", CMD_CHDIR , tmp_dir );
	if ( opt_print )
	{
		voutwrite ( line );
	}
	xprotwrite ( line );

	if ( opt_noexec ) return ( 0 );

	rc = chdir ( tmp_dir );
	if ( rc < 0 )
	{
		DBG0 (( "cannot chdir to '%s' \n", tmp_dir ))
		return ( rc );
	}
	DBG8 (( "%s %s \n", CMD_CHDIR , tmp_dir ))

	DBG4 (( "setwrkdir: returning %d \n", 0 ))
	return ( 0 );
}

/*==========================================================================*/

global	int     set_work_dir_in_objdir
(
    char                    version,
	char					*layer

)
{
	int                     rc=0 ;
	char                    * vn , line [ LINESIZE ];


	DBG4 (( "setwrkdir: version '%c' \n", version ))

	switch ( version )
	{
	case 'f' :
	case 'q' :
	case 's' :
	case 'n' :
		break ;
	default :
		version = vmakeversion ;
	}
	vn = versname ( version );

	/*
	 *  put VMAKE_VERSION to the environment.
	 *  WARNING: don't use a automatic variable with 'putenv'!
	 */
	(void) sprintf ( vmakeversion_env , "CURR_VERSION=%s" , vn );
	rc = putenv ( vmakeversion_env );
	if ( rc )
	{
		DBG0 (( "cannot putenv '%s' \n", vmakeversion_env ))
		return ( rc );
	}

	(void) sprintf ( line , DIR_OBJ , vmake_path->path , vn, layer );
	
	if ( strcmp ( line , tmp_dir ) == 0 )
	{
		DBG4 (( "setwrkdir: work dir already correct '%s' \n", line ))
		return ( 0 );
	}
	(void) strncpy ( tmp_dir , line , sizeof(tmp_dir)-1 );
	
	rc = check_dir ( tmp_dir );
	if ( rc )  { return rc; }

	DBG7 (( "setwrkdir: tmp_dir '%s' \n", tmp_dir ))

	(void) sprintf ( line , "%s %s \n", CMD_CHDIR , tmp_dir );
	if ( opt_print )
	{
		voutwrite ( line );
	}
	xprotwrite ( line );

	if ( opt_noexec ) return ( 0 );

	rc = chdir ( tmp_dir );
	if ( rc < 0 )
	{
		DBG0 (( "cannot chdir to '%s' \n", tmp_dir ))
		return ( rc );
	}
	DBG8 (( "%s %s \n", CMD_CHDIR , tmp_dir ))

	DBG4 (( "setwrkdir: returning %d \n", 0 ))
	return ( 0 );
}



static	int	test_is_dir
(
    char			* path
)
{
	int			rc=0 ;
	char			*ptr ;
	struct	stat		statbuf ;
#ifdef WIN32
static	char			drive []= "c:/" ;
#endif

	/* PTS 1103276 */
	for (ptr = path + strlen (path); *(ptr-1) == '/'; *ptr--);
	*ptr=0;

	ptr = path ;
#ifdef WIN32
	/*
	 *  The drive identifier must have a '/' to be
	 *  recognized as a directory.
	 */
	if ( isalpha(ptr[0]) && (ptr[1] == ':') && ! ptr[2] )
	{
	    drive[0] = *ptr ;
	    ptr = drive ;
	}
#endif
	/* rc = stat ( ptr , & statbuf ); */
    rc = filestat( ptr , & statbuf ); /* GG 08.07.98 use function also for remote files */
	if ( rc < 0 )
	{
	    if ( (geterrno() != ENOENT) && (geterrno() != ENOTDIR) )
	    {
		    DBG0 (( "cannot stat '%s': %s \n", ptr , serrno() ))
		    return ( 2 );
	    }
	    DBG4 (( "testisdir: directory does not exist '%s' \n", ptr ))
	    return ( 1 );
	}

	/* path exists */

	if ( ! (statbuf.st_mode & S_IFDIR) )
	{
	    DBG4 (( "'%s' isn't a directory \n", ptr ))
	    return ( 2 );
	}

	DBG4 (( "testisdir: directory exists '%s' \n", ptr ))
	return ( 0 );
}

/*==========================================================================*/

#ifdef	MKDIR
static	int     mkdir
(
    char                    * dir ,
    int                     mode
)
{
	int                     rc=0 ;
	char                    * p , * cmd ;
	SIG_RTRN                (* oldsigcld)();


	p = "mkdir %s;chmod 0%03o %s";
	cmd = malloc ( strlen(p) + 2 * strlen(dir) );
	if ( ! cmd )
	{
		DBG0 (( "insufficient memory \n" ))
		seterrno(ENOMEM) ;
		return ( -1 );
	}
	(void) sprintf ( cmd , p , dir , mode , dir );

	oldsigcld = signal ( SIGCLD , SIG_DFL ); /* sh needs it */

	rc = system ( line );

	(void) signal ( SIGCLD , oldsigcld );
	FREE_AND_SET_TO_NULL(cmd);

	if ( rc )
	{
	    seterrno ( rc >> 8 );
	    return ( rc );
	}
	return ( 0 );
}
#endif	/*MKDIR*/

/*==========================================================================*/

/**VARARGS1*/
/*PRINTFLIKE1*/
global	void	dbgdbg
(
    char    * fmt ,
    ...
)
{
	int     lgt, rc ;
	va_list	args ;
	char    line [ LINESIZE ] ;

	/*
	    This function is entered only, if 'dbgdbglvl' is
	    greater or equal to the message's level 'dbgcurlvl'.
	*/

	va_start ( args, fmt );
#ifdef WIN32
	rc = _vsnprintf ( line , LINESIZE-1, fmt , args );
#else
	rc = vsprintf ( line , fmt , args );
#endif
	va_end ( args );
#ifdef WIN32
	if ( rc == -1 )
/*	 line to short for args */
			line [ LINESIZE -1 ] = '\0';
#endif
	lgt = strlen ( line );

	/*
	 *  'dbgdbglvl' is always one higher than required.
	 *  This higher level goes into the extended protocol only.
	 */
	if ( dbgdbglvl > dbgcurlvl )
	{
		/* write to both protocols */
		dowrite ( line , lgt );
	}
	else
	{
		/* write to extended protocol only */
		if ( xprotfp )
		{
			(void) fwrite ( line , 1 , (CINT) lgt , xprotfp );
			(void) fflush ( xprotfp );
		}
	}
}


/*==========================================================================*/

/**VARARGS1*/
/*PRINTFLIKE1*/
global	void	outstamp
(
    char    * fmt ,
    ...
)
{
	int     lgt, rc ;
	time_t	clock ;
	va_list	args ;
	char    line [ LINESIZE ] ;

	if ( vopt_timestamp > 1 )
	{
		clock = time ( (time_t*)0 );
		(void) strcpy ( line , ascdate ( clock ) );
		lgt = strlen ( line );
		line [ lgt ++ ] = ' ' ;
	}
	else	lgt = 0 ;

	va_start ( args, fmt );
#ifdef WIN32
	rc = _vsnprintf ( line + lgt , LINESIZE - lgt, fmt , args );
#else
	rc = vsprintf ( line + lgt , fmt , args );
#endif
	va_end ( args );
#ifdef WIN32
	if ( rc == -1 )
/*	 line to short for args */
			line [ LINESIZE-1 ] = '\0';
#endif
	lgt += strlen ( line + lgt );
	if (opt_silent_mode)
	{
/* -> new */
		if (opt_silent_mode != VMAKE_SILENT_MODE_NOTHING)
		{
			if (opt_silent_mode == VMAKE_SILENT_MODE_ROTATE) 
			{
				if (current_silent_char >= 0)
					(void) fwrite ( "\b" , 1 , (CINT) 1, stdout );		
			}
			else
				if (++current_silent_char_count > 79)
				{
					(void) fwrite ( "\n" , 1 , (CINT) 1, stdout );		
					current_silent_char_count = 1;
					/*current_silent_char = 0;*/
				}
			current_silent_char++;
			if (current_silent_char > (int) strlen(silent_char_string) - 1 )
				current_silent_char = 0;
			(void) fwrite ( silent_char_string + current_silent_char, 1 , (CINT) 1, stdout );
			
	/* <- new */
			/* (void) fwrite ( "." , 1 , (CINT) 1, stdout ); */
			(void) fflush ( stdout );
		}
		
		strcpy (outstampline, line);
		dowrite_to_protocols ( line , lgt );
	}
	else
        dowrite ( line , lgt );
}

/* =================================== */

global	void	outstamp_add
(
    char    * fmt ,
    ...
)
{
	int    rc ;
	va_list	args ;
	char    line [ LINESIZE ] ;


	va_start ( args, fmt );
#ifdef WIN32
	rc = _vsnprintf ( line , LINESIZE, fmt , args );
#else
	rc = vsprintf ( line , fmt , args );
#endif
	va_end ( args );
#ifdef WIN32
	if ( rc == -1 )
/*	 line to short for args */
			line [ LINESIZE-1 ] = '\0';
#endif
	if (opt_silent_mode)
	{
		strcat (outstampline, line);
		dowrite_to_protocols ( line , strlen(line) );
	}
	else
        dowrite ( line , strlen(line) );
}



/*==========================================================================*/


/**VARARGS1*/
/*PRINTFLIKE1*/
global	void	voutwrite
(
    char    * fmt ,
    ...
)
{
	int     lgt, rc ;
	va_list	args ;
	char    line [ LINESIZE ] ;

	va_start ( args, fmt );
#ifdef WIN32
	rc = _vsnprintf ( line , LINESIZE-1, fmt , args );
#else
	rc = vsprintf ( line , fmt , args );
#endif
	va_end ( args );
#ifdef WIN32
	if ( rc == -1 )
/*	 line to short for args */
			line [ LINESIZE-1 ] = '\0';
#endif
	lgt = strlen ( line );
	dowrite ( line , lgt );
}


/*==========================================================================*/

/* This is for Ingo's sources like imalloc.c */
/**VARARGS1*/
/*PRINTFLIKE1*/
global	void	err
(
    char    * fmt ,
    ...
)
{
	int     lgt,rc ;
	va_list	args ;
	char    line [ LINESIZE ] ;

	va_start ( args, fmt );
#ifdef WIN32
	rc = _vsnprintf ( line , LINESIZE-1, fmt , args );
#else
	rc = vsprintf ( line , fmt , args );
#endif
	va_end ( args );
#ifdef WIN32
	if ( rc == -1 )
/*	 line to short for args */
			line [ LINESIZE-1 ] = '\0';
#endif
	lgt = strlen ( line );
	dowrite ( line , lgt );
}


/*==========================================================================*/

global	void	dowrite
(
    char                    * line ,
    int                     length
)
{

	(void) dowrite_to_console (line, length);
	(void) dowrite_to_protocols (line, length);
}

/*==========================================================================*/

global	void	dowrite_to_console
(
    char                    * line ,
    int                     length
)
{

	(void) fwrite ( line , 1 , (CINT) length , stdout );
	(void) fflush ( stdout );
}


/*==========================================================================*/

global	void	dowrite_to_protocols
(
    char                    * line ,
    int                     length
)
{
	if ( protfp != NULL )
	{
		(void) fwrite ( line , 1 , (CINT) length , protfp );
		(void) fflush ( protfp );
	}

	if ( xprotfp != NULL )
	{
		(void) fwrite ( line , 1 , (CINT) length , xprotfp );
		(void) fflush ( xprotfp );
	}
}



/*==========================================================================*/

/**VARARGS1*/
/*PRINTFLIKE1*/
static	void	xprotwrite
(
    char    * fmt ,
    ...
)
{
	int     lgt,rc ;
	va_list	args ;
	char    line [ LINESIZE ] ;

	if ( xprotfp != NULL )
	{
    	va_start ( args, fmt );

#ifdef WIN32
		rc = _vsnprintf ( line , LINESIZE-1, fmt , args );
#else
		rc = vsprintf ( line , fmt , args );
#endif
		va_end ( args );
#ifdef WIN32
		if ( rc == -1 )
		/*	 line to short for args */
				line [ LINESIZE-1 ] = '\0';
#endif
		lgt = strlen ( line );
		(void) fwrite ( line , 1 , (CINT) lgt , xprotfp );
		(void) fflush ( xprotfp );
	}
}

/*==========================================================================*/

/**VARARGS1*/
/*PRINTFLIKE1*/
global	void	mprotwrite
(
    char    * fmt ,
    ...
)
{
	int     lgt,rc ;
	va_list	args ;
	char    line [ LINESIZE ] ;

	if ( mprotfp != NULL )
	{
    	va_start ( args, fmt );
#ifdef WIN32
		rc = _vsnprintf ( line , LINESIZE-1, fmt , args );
#else
		rc = vsprintf ( line , fmt , args );
#endif
		va_end ( args );
#ifdef WIN32
		if ( rc == -1 )
		/*	 line to short for args */
				line [ LINESIZE-1 ] = '\0';
#endif
		lgt = strlen ( line );
		(void) fwrite ( line , 1 , (CINT) lgt , mprotfp );
		(void) fflush ( mprotfp );
	}
}

/*=================================================================*/
/* CTS 1112006 -> */
global	void	eprotwrite
(
    char    * fmt ,
    ...
)
{
	int     lgt,rc ;
	va_list	args ;
	char    line [ LINESIZE ] ;

	if ( eprotfp != NULL )
	{
    	va_start ( args, fmt );

#ifdef WIN32
		rc = _vsnprintf ( line , LINESIZE-1, fmt , args );
#else
		rc = vsprintf ( line , fmt , args );
#endif
		va_end ( args );
#ifdef WIN32
		if ( rc == -1 )
		/*	 line to short for args */
				line [ LINESIZE-1 ] = '\0';
#endif
		lgt = strlen ( line );
		(void) fwrite ( line , 1 , (CINT) lgt , eprotfp );
		(void) fflush ( eprotfp );
	}
}
/* <- CTS 1112006 */


/*=================================================================*/
global	void	eprotwriteline
(
    char    * line
)
{
	int     lgt;
	lgt = strlen ( line );
	(void) fwrite ( line , 1 , (CINT) lgt , eprotfp );
	(void) fflush ( eprotfp );
}

/*=================================================================*/
/* CTS 1112006 -> */
global	void	wprotwrite
(
    char    * fmt ,
    ...
)
{
	int     lgt,rc ;
	va_list	args ;
	char    line [ LINESIZE ] ;

	if ( wprotfp != NULL )
	{
    	va_start ( args, fmt );

#ifdef WIN32
		rc = _vsnprintf ( line , LINESIZE-1, fmt , args );
#else
		rc = vsprintf ( line , fmt , args );
#endif
		va_end ( args );
#ifdef WIN32
		if ( rc == -1 )
		/*	 line to short for args */
				line [ LINESIZE-1 ] = '\0';
#endif
		lgt = strlen ( line );
		(void) fwrite ( line , 1 , (CINT) lgt , wprotfp );
		(void) fflush ( wprotfp );
	}
}
/* <- CTS 1112006 */


/*=================================================================*/
global	void	wprotwriteline
(
    char    * line
)
{
	int     lgt;
	lgt = strlen ( line );
	(void) fwrite ( line , 1 , (CINT) lgt , wprotfp );
	(void) fflush ( wprotfp );
}


/*==========================================================================*/

global	char	*serrno
(
)
{
    int				len ;
    int				err ;
    static char			line [ 128 ];
    err = geterrno  ();
    (void) sprintf ( line , "%d, ", err );
    len = strlen ( line );
    (void) sprintf ( line + len , "%.*s",
			sizeof(line) - len - 1 , strerror(err) );
    return ( line );
}

/*==========================================================================*/

global	void     vmake_term
(
)
{
	terminating = TRUE ;
	set_protocol ( EMPTY , ' ' );
#ifdef WIN32
	close_connections ( );
#else
	exit_vmake ( 2, TRUE );
#endif
}


/*==========================================================================*/

global	void     exit_vmake
(
    int                     code ,
	int                     unlink_pidfile
)
{
/*	SetEvent(hStopRequest);*/
#ifdef WIN32
	if (hChildProcess)
	{
		DBG1 (( "Terminating child process ... " ))
		if ( TerminateProcess(hChildProcess, code))
			DBG1 (( "OK" ))
		else
			DBG1 (( "FAILED!" ))
		Sleep (500);
	}


#endif
	close_connections ( );
	if (unlink_pidfile)
		(void) unlinkPIDFile(1);

	exit ( code );
	/*NOTREACHED*/
}


/* This function generates a string like the following pattern
	ven81c dated 1994-12-16 16:38:50 used 1994-12-17 17:29:48
 * containing the module name (with variant), its date, and current date.
 *
 * This string is exported as shell varianle 'MOD_DAT' and used in the
 * shell scripts 'mfp' and 'mfc' where it is wrapped in a 'what' pattern.
 * So any program contains these dates, accessible to 'what'.
 * It is important that 'fgrep' and 'sort' can be applied!
 */

static	char 		*the_string ;


global	void	whatmod
(
    char			* modname ,
    time_t			modtime
)
{
    time_t			now_time ;
    struct tm			mod , now ;

    now_time = time ( (time_t *) 0 );
    COPY ( &mod , localtime(&modtime) , sizeof(struct tm) );
    COPY ( &now , localtime(&now_time) , sizeof(struct tm) );

	the_string =   repalloc(the_string, strlen(modname)+60);
    (void) sprintf ( the_string ,
       "MOD_DAT=%s dated %.4d-%.2d-%.2d %.2d:%.2d:%.2d used %.4d-%.2d-%.2d %.2d:%.2d:%.2d" ,
             modname ,
             mod.tm_year+1900, mod.tm_mon+1, mod.tm_mday,
             mod.tm_hour, mod.tm_min , mod.tm_sec ,
             now.tm_year+1900, now.tm_mon+1, now.tm_mday,
             now.tm_hour, now.tm_min , now.tm_sec );

    if ( putenv ( the_string ) )
    {
        DBG0 (( "putenv fails - no memory - ABORT\n" ))
        exit ( 3 );
    }
}

/*==========================================================================*/

global int	copy_file
(
    char			* srcfile ,
    char			* dstfile ,
    struct stat		* statbufp
)
{
    int				rc ;
    int				lgt ;
    int				ifd ;
    int				ofd ;
    struct utimbuf	utimbuf ;
    char			buf [ 16384 ];

    ifd = open ( srcfile , O_RDONLY
#if ! defined(unix) || defined(CYGNUS)
			    | O_BINARY
#endif
			    , 0 );
    if ( ifd < 0 )
    {
	    err ( "copy: cannot open '%s': %s \n", srcfile , serrno() );
	    return ( -2 );
    }

    rc = read ( ifd , buf , sizeof(buf) );
    if ( rc == -1 )
    {
	    err ( "copy: error reading '%s': %s \n", srcfile , serrno() );
	    (void) close ( ifd );
	    return ( -2 );
    }
    lgt = rc ;

    ofd = open ( dstfile , O_WRONLY | O_CREAT | O_TRUNC
#if ! defined(unix) || defined(CYGNUS)
			    | O_BINARY
#endif
			    , 0200 );
    if ( ofd < 0 )
    {
	    (void) close ( ifd );
	    err ( "copy: cannot create '%s': %s \n", dstfile , serrno() );
	    return ( -2 );
    }

    while ( lgt > 0 )
    {
	    while ( lgt > 0 )
	    {
	        rc = write ( ofd , buf , (unsigned) lgt );
	        if ( rc == -1 )
	        {
		    err ( "copy: error writing '%s': %s \n", dstfile , serrno() );
		    (void) close ( ifd );
		    (void) close ( ofd );
		    return ( -2 );
	        }
	        lgt -= rc ;
	    }

	    lgt = read ( ifd , buf , sizeof(buf) );
	    if ( lgt == -1 )
	    {
	        err ( "copy: error reading '%s': %s \n", srcfile , serrno() );
	        (void) close ( ifd );
	        (void) close ( ofd );
	        return ( -2 );
	    }
    }
    /* while lgt > 0 */

    rc = close ( ifd );
    rc = close ( ofd );

    utimbuf.actime  = statbufp->st_atime ;
    utimbuf.modtime = statbufp->st_mtime ;
    rc = utime ( dstfile , &utimbuf );
	
	/* &gar CTS 1103400 */
	if (vopt_rw_permission)
#if defined(WIN32)
		rc = chmod ( dstfile , _S_IREAD | _S_IWRITE );
#else
		/* under UNIX + write permissions for user and group */
		rc = chmod ( dstfile, statbufp->st_mode | S_IWUSR | S_IWGRP);
#endif
	else
		rc = chmod ( dstfile , statbufp->st_mode );

    return ( 0 );
}

/*==========================================================================*/

static int	stretch_file
(
    char			* file
)
{
	int			rc=0 ;
	char		* fname;
	char		newfile  [ FILENAMESIZE ];

	DBG4 (( "stretch_file: file '%s' \n", file ))

	if ( !( fname = strrchr( file, '/' )) ) {
		/* no path information */
		fname = file;
	} else {
		fname++;
		(void) strncpy( newfile, file, fname-file );
		newfile[ fname-file ] = '\0';
	}
	strcat(strcat(newfile, "_"), fname);
	if ( access(newfile, 0) != -1 ) {
		/* try to unlink a existent stretched file */
		if ( unlink( newfile ) )
			stretch_file( newfile );
	}
	rc = rename( file, newfile );
		
	if ( rc )
	{
	    DBG4 (( "rename '%s' to '%s' failed \n", file, newfile ))
	    return ( rc );
	}

	DBG4 (( "stretch_file: returning %d \n", 0 ))
	return ( 0 );
}

/*==========================================================================*/

static int	force_unlink
(
    char			* file
)
{
	int		rc=0;

	DBG4 (( "force_unlink: file '%s' \n", file ))

    if ( test_is_dir( file ) == 0 )
    {
    	DBG4 (( "force_unlink: %s is directory  \n", file ))
        rc = 0;
    }
    else
    {
	    rc = unlink ( file );
	    if ( rc < 0 )
	    {
	        if ( (geterrno() != ENOENT) && (geterrno() != ENOTDIR) )
	        {
			    DBG4 (( "cannot unlink old '%s': %s \n", file , serrno() ))
			    DBG4 (( "try to stretch '%s' \n", file ))
			    if ( stretch_file(file) ) {
				    DBG0 (( "force_unlink of '%s' failed : %s \n", file, serrno() ))
				    return ( -2 );
			    }
	        }
	        else
	        {
			    DBG8 (( "force_unlink: no old '%s' \n", file ))
	        }
	    }
    }
	DBG4 (( "force_unlink: returning %d \n", 0 ))
	return ( 0 );
}

/*==========================================================================*/

global char * get_basename
(
    char			* target ,
    char			* basename
)
{
    char * firstvariant_index, *p ;

    firstvariant_index = strchr ( target, '-' );
	p = strchr ( target, '+' );
	if  ( p > firstvariant_index )
	{
		if (! firstvariant_index)
			firstvariant_index = p;
	}
	else
	{
		if ( p )
			firstvariant_index = p;
	}
	/* firstvariant_index to first variant */
	p = extension_exists ( target );

	/* if variant found */
	if (firstvariant_index)
	{
		(void) strncpy( basename, target, firstvariant_index - target );
		basename[ firstvariant_index - target ] = '\0';
		if ( p )
			(void) strcat( basename, p );
	}
	else
		(void) strcpy ( basename , target );

	p= strstr(basename, NEW_VARIANT_DEFINITION);
	if (p)
		*p = '\0';
	
    return basename;
}

/*==========================================================================*/
int  test_for_variants
(
    char			* target
/* if variant == NULL we would only regonize, that a variant exist */
)
{
	if (strchr ( target, '-' ) || strchr ( target, '+' ) )
		return 1;
	else
		return 0;
}


/* ======================== */
/* return:					*/
/* ERROR			< 0		*/
/* no variants		= 0		*/
/* found variants	> 0		*/
/* ======================== */

global int get_variants
(
    char					* target ,
    char					** localvariant,
	struct variant_info		** globalvariants
)
{
    char *found_local_variant= NULL;
	char *found_global_variant=NULL;
	char *p;
	char save_char;
	int rc = 0;


	/* check for already initialized variants */
	if ( *localvariant != NULL )
	{
		DBG0 (( "get_var: Error: local variant already initialized %s \n", target ))
		return -2;
	}
		
	if ( *globalvariants != NULL )
	{
		DBG0 (( "get_var: Error: global variants already initialized %s \n", target ))
		return -2;
	}
	

	for ( p = target ; *p ; p ++  )
	{
		if ((*p == '-') || (*p == '+') || (*p == '.') )
		{
			save_char = *p;
			*p = '\0';
			if (found_local_variant)
			{
				if (* localvariant)
				{
					DBG0 (( "ERROR: more than one local variant in target %s \n", target )) ;
					return -2;
				}
				if ( ! strlen (found_local_variant))
				{
					DBG0 (( "ERROR: bad variant definition for %s \n", target )) ;
					return -2;
				}

				* localvariant = (char *) palloc (strlen (found_local_variant) + 1);
				strcpy(* localvariant, found_local_variant);
				found_local_variant = NULL;
				rc ++;
			}
			if (found_global_variant)
			{
				if ( ! strlen (found_global_variant))
				{
					DBG0 (( "ERROR: bad variant definition for %s \n", target )) ;
					return -2;
				}
				if ( insert_variant(globalvariants, found_global_variant, 1) < 0)
					return -3;
				found_global_variant=NULL;
				rc ++;
			}
			*p = save_char;
			if (*p == '.')
				break;
		}
				
		if (*p == '-')
		{	found_local_variant = ++p; }
		if (*p == '+')
		{	found_global_variant = ++p;}
	}

	if (found_local_variant)
	{
		if (* localvariant)
		{
			DBG0 (( "ERROR: more local than one variant in target %s \n", target )) ;
			return -2;
		}
		if ( ! strlen (found_local_variant))
		{
			DBG0 (( "ERROR: bad variant definition for %s \n", target )) ;
			return -2;
		}

		* localvariant = (char *) palloc (strlen (found_local_variant) + 1);
		strcpy(* localvariant, found_local_variant);
		rc ++;
	}
	if (found_global_variant)
	{
		if ( ! strlen (found_global_variant))
		{
			DBG0 (( "ERROR: bad variant definition for %s \n", target )) ;
			return -2;
		}
		if (insert_variant(globalvariants, found_global_variant, 1) < 0)
			return -3;
		rc ++;
	}
	return rc;
}

global int check_variants
(
	char					* local_variant,
	struct variant_info		* global_variants
)
{
	struct variant_info		* temp_vip;
	int rc ;
	
	if (local_variant)
		for (temp_vip = global_variants ; temp_vip; temp_vip = temp_vip->next )
		{
			if ( strcmp ( temp_vip->name , local_variant) == 0)
			{
				DBG0 (( "ERROR: variant %s defined as global and local \n", local_variant ))
				return -3;
			}
		}
	rc= 1;
	return 1;
}

global int insert_variant
(
	struct variant_info		** vipp,
	char                     * variantname,
	int                        dupplicate_warning
)
{
	struct variant_info		** temp_vipp;
	struct variant_info      * temp_vip;
	int                     rc ;

	for (temp_vipp = vipp; *temp_vipp; temp_vipp = &(*temp_vipp)->next )
	{

        /* compare variant names */
	    rc = strcmp (variantname, (*temp_vipp)->name );
	    if ( rc > 0 ) break ;
	    if ( rc < 0 ) continue ;
		if ( dupplicate_warning )
			DBG0 (( "WARNING: dupplicate variant definition for %s (ignored)\n", variantname )) ;
		return 0;
	}
	temp_vip =  (struct variant_info *) palloc ( sizeof (struct variant_info));
	temp_vip->name = (char *) palloc ( strlen(variantname) + 1 );
	strcpy ( temp_vip->name, variantname );
	temp_vip->next = *temp_vipp;
	*temp_vipp = temp_vip;
	return 1;
}

global int insert_variants
(
	struct variant_info		** target_vipp,
	struct variant_info		* src_vip
)
{
	struct variant_info		* temp_vip;
	
	/* if first list without elements set list to second list */
	if ( ! *target_vipp)
	{
		*target_vipp = src_vip;
	}
	else
		for (temp_vip = src_vip; temp_vip; temp_vip = temp_vip->next )
		{
			if (insert_variant( target_vipp, src_vip->name, 0) < 0)
				return -3;
		}
	return 1;
}


global int build_full_variant_name
(
	char				** targetname,
	char                * local_variant,
	struct variant_info * global_variants
)
{
	char			    line [ LINESIZE ] ;
	char                *extension= NULL;

	struct variant_info * temp_vip ;
	char                * p;
	int    variant_count = 0;

	(void) get_basename( *targetname, line );

	p = get_extension( line );
	if (*p != '\0' )
	{
		extension = (char *) palloc (strlen (p) + 2 );
		strcpy (extension, ".");
		strcat (extension, p);
	}
	
	skip_extension(line);
	if (local_variant)
	{
		strcat(line, "-");
		strcat(line, local_variant);
	}
	for (temp_vip = global_variants; temp_vip; temp_vip = temp_vip->next )
	{
		strcat(line, "+");
		strcat(line, temp_vip->name);
	}
	/* append extension if exists */	
	if (extension)
		strcat(line, extension);
	

	*targetname = (char *) repalloc( *targetname, strlen(line) + 1 );
	strcpy(*targetname, line );

	FREE_AND_SET_TO_NULL(extension);
	return 1;
}

int append_global_variants
(
	struct variant_info		** variant_list,
	struct variant_info		* to_append_list
)
{
	/* remove dupplicates from the first list (variant_list) and not from the appended list */
	struct variant_info     ** temp_vipp;
	struct variant_info     * vipp_to_remove;
	struct variant_info     * ap_temp_vip;
	int found_dupplicate;

	for ( temp_vipp = variant_list; * temp_vipp; )
	{
		found_dupplicate = 0;
		for ( ap_temp_vip = to_append_list;
			 ap_temp_vip;
			ap_temp_vip = ap_temp_vip->next)
		{
				if (strcmp (ap_temp_vip->name, (*temp_vipp)->name) == 0 )
				{
					vipp_to_remove = (*temp_vipp);
					(*temp_vipp) = (*temp_vipp)->next;
					FREE_AND_SET_TO_NULL(vipp_to_remove->name);
					FREE_AND_SET_TO_NULL(vipp_to_remove);
					found_dupplicate = 1;
					continue;
				}
		}
		/* dupplicate is ignored */
		if (! found_dupplicate)
			temp_vipp = &(*temp_vipp)->next;
	}

	* temp_vipp = to_append_list;
	return 1;
}

int get_variants_save
(
    char					* target ,
    char					* localvariant,
	char					* globalvariant
/* if variant == NULL we would only regonize, that a variant exist */
)
{
    char * check_local_variant,* end, * check_global_variant;
	int rc = 0;

	
    check_local_variant = strchr ( target, '-' );
	check_global_variant = strchr ( target, '+' );
	
	if ( ! check_local_variant && ! check_global_variant )
	{
		localvariant[ 0 ] = '\0';
		globalvariant[ 0 ] = '\0';
	}
	else
	{
		end = extension_exists ( target );
		if ( ! end )
		{
			end = target + strlen(target);
		}
		/* local variant to extension (end) */
		if (check_local_variant > check_global_variant)
		{
			(void) strncpy( localvariant, check_local_variant + 1,
				end - check_local_variant - 1 );
			localvariant[ end - check_local_variant - 1 ] = '\0';
			if ( check_global_variant )
			{
				(void) strncpy( globalvariant, check_global_variant + 1,
				check_local_variant - check_global_variant - 1 );
				globalvariant[ check_local_variant - check_global_variant - 1 ] = '\0';

			}
			else
				check_global_variant [0] = '\0';
		}
		else
		{
			(void) strncpy( globalvariant, check_global_variant + 1,
				end - check_global_variant - 1 );
			globalvariant[ end - check_global_variant - 1 ] = '\0';
			if ( check_local_variant )
			{
				(void) strncpy( localvariant, check_local_variant + 1,
				check_global_variant - check_local_variant - 1 );
				localvariant[ check_global_variant - check_local_variant - 1 ] = '\0';
			}
			else
				check_local_variant [0] = '\0';
		}
	}
	if ( check_local_variant )
		rc ++;
	if ( check_global_variant )
		rc ++;

	return rc;
}

global int create_globalvariant_filename
(
	char				*filename,
	char				**variant_filename,
	struct variant_info *globalvariants
)
{
	char *p;
	char globaldir[NAMESIZE];
	
	DBG3 (("create_globalvariant_filename: name '%s'\n", filename))
	if (! globalvariants)
		return 0;
	if ( strnicmp(filename, env_OWN, strlen(env_OWN)) != 0 )
	{
		DBG0 (( "Error: global variants outside OWN not allowed (%s)\n", filename ))
		eprotwrite ( "Error: global variants outside OWN not allowed (%s)\n", filename );
		return -3;
	}
	
	(void) get_global_variant_dir (globalvariants, globaldir);
	
	p = filename + strlen (env_OWN) ;
	if (*variant_filename)
		*variant_filename = (char*) repalloc (*variant_filename, strlen(pDIR_OUT) + strlen(globaldir) + strlen (p) + 1 );
	else
		*variant_filename = (char*) palloc (strlen(pDIR_OUT) + strlen(globaldir) + strlen (p) + 1 );

	strcpy (*variant_filename, pDIR_OUT );
	strcat (*variant_filename, globaldir );
	strcat (*variant_filename, p );
	DBG4 (("create_globalvariant_filename: return '%s'\n", *variant_filename))

	return 1;

}

global int get_global_variant_dir
(
 struct variant_info	*globalvariants,
 char					*globaldir
)
{	if (! globalvariants)
	 return 0;
	
	strcpy(globaldir, globalvariants->name);
	while ( globalvariants->next)
	{
		globalvariants = globalvariants->next;
		strcat (globaldir, "+");
		strcat (globaldir, globalvariants->name);
	}
	return 1;
}

global int cut_global_variants
(
	char				*name
)
{
	char *ext, *p;
	char *variant;

	DBG3 (( "cut_global_variants: name '%s' \n", name ))
	
	variant= strchr(name, '+');
	if ( ! variant )
		return 0;
	ext = get_extension(name);

	*variant = '\0';

	if ( *ext != '\0' )
	{
		/* to do because:
		The behavior of strcat is undefined
		if the source and destination strings overlap ...
		*/
		p = (char*) palloc (strlen(ext) + 2);
		strcpy (p, ".");
		strcat (p, ext);
		strcat (name, p);
		FREE_AND_SET_TO_NULL(p);
	}

	DBG4 (( "cut_global_variants returns: '%s' \n", name ))

	return 1;
}

/*==========================================================================*/
/*  substitute:                                                             */
/*--------------------------------------------------------------------------*/
/*  substitute enviroment variables with the real values                    */
/*==========================================================================*/
global  int     substitute_internal_variables
(
	char					**subst_string ,
	struct trg_info			*tip
)
{
	unsigned        i = 0, j, actlen ;
	char			* p , * q;
	char			* int_var;
	char			var [ FILENAMESIZE ];
	char			*path = NULL;
	int rc;
	int bracketcount; 

	DBG2 (( "substitute: called \n" ))
	DBG3 (( "substitute: name  '%s' \n", *subst_string ))

    actlen = strlen( *subst_string );
    path = (char *) palloc( actlen + 1 );

	/* expand environment variables */
	for ( p = *subst_string ; *p ; p ++ , i ++ )
	{
		bracketcount = 0;
	    /* environment substitution starts with '$' */
	    if ( * p == '%' )
	    {
            unsigned varlen;

		    p ++ ;
            /* treat "$$" as "$" */
            if ( * p == '%' )
            {
    		    path[ i ] = * p ;
            }
            else
            {
                if ( * p == LBRACKET )
                {
					bracketcount = 1;
                    /* variable *subst_string is enclosed by braces {} */
                    p ++ ;
                    for ( j = 0 ; ((*p != RBRACKET) || bracketcount >1)  && j < sizeof(var); p ++ , j ++ )
                    {
                        var [ j ] = * p ;
						if ( * p == LBRACKET )
							bracketcount ++;
						else 
							if ( * p == RBRACKET )
								bracketcount --;
                    }
                    if ( * p == RBRACKET ) p ++ ;
                }
				else
					if (* p == LBRACKET2 )
					{
						bracketcount = 1;
						/* variable *subst_string is enclosed by braces () */
						p ++ ;
						for ( j = 0 ; ((*p != RBRACKET2) || bracketcount >1)  && j < sizeof(var); p ++ , j ++ )
						{
							if ( * p == LBRACKET2 )
								bracketcount ++;
							else 
								if ( * p == RBRACKET2 )
									bracketcount --;
							var [ j ] = * p ;
						}
						if ( * p == RBRACKET2 ) p ++ ;
					}
					else
					{
						if ( *p == '+' || *p == '-' || *p == '*' )
						{
							var [ 0 ] = * p;
							j=1; p++;
						}
						else
						{
							/* variable *subst_string is terminated by non-symbol-character */
							for ( j = 0 ; 
							( isalnum ( * p ) || * p == '_' || * p == LBRACKET2 ) && j < sizeof(var); 
							p ++ , j ++ 
								)
							{
								/* found function arguments */
								if (* p == LBRACKET2 )
								{
                                    for (  ; (*p != RBRACKET2)  && j < sizeof(var); p ++ , j ++ )
									{
										var [ j ] = * p ;
									}
									if ( * p == RBRACKET2 ) var [ j ] = * p ; 
								}
								else
									var [ j ] = * p ;
							}
						}
					}
                if ( j >= sizeof(var) )
                {
                    DBG0 (( "Error: variable name too long '%.*s' \n", sizeof(var) , var ))
					eprotwrite ( "Error: variable name too long '%.*s' \n", sizeof(var) , var );
                    return ( - 2 );
                }
                var [ j ] = '\0' ;
				int_var  = NULL;
				               
                rc = get_intern_variables ( var, &int_var,  tip );
                if ( int_var == NULL )
                {
                    DBG0 (( "Error: cannot substitute internal variable '%s' \n", var ))
					eprotwrite ( "Error: cannot substitute internal variable '%s' \n", var );
                    return ( - 2 );
                }
                varlen = strlen( int_var );
                
                path = (char *) repalloc( path, actlen - ( strlen( var ) + 1 ) 
                    + varlen + 1 );
				q = int_var;
                for ( ; * q != '\0' ; q ++ , i ++ )
                {
                    path [ i ] = * q ;
                }
                actlen = actlen - ( strlen( var ) + 1 ) + varlen;
                p -- , i -- ; /* compensate the surrounding loop's increment */
				FREE_AND_SET_TO_NULL(int_var);
            }
        }
	    else
	    {
		    (path) [ i ] = * p ;
	    }
	}

	path [ i ] = '\0' ;
	*subst_string = (char *) repalloc( *subst_string,  strlen( path ) + 1 ) ;
	strcpy (*subst_string, path);
	DBG4 (( "substitute: path '%s' \n", *subst_string ))
	FREE_AND_SET_TO_NULL(path);
	return ( 1 );
}

global int get_global_variant_name
(
	char	                **new_string,
	struct trg_info			*tip,
	char                    *prefix,
	char                    *separator
)
{
	int length = 0;
	int sep_length = separator ? strlen(separator) : 0;
	struct variant_info     * vip;
    	
	if (tip->globalvariants)
	{
		for (vip = tip->globalvariants; vip; vip = vip->next )
			length += strlen(vip->name) + sep_length;
	}
	
	if (length > 0)
	{
		if (prefix)
			length += strlen(prefix);

		*new_string  = (char *) repalloc(*new_string, length + 1 );
		*new_string[0] = '\0';
		if (prefix)
			strcpy (*new_string, prefix); 
		for (vip = tip->globalvariants; vip; vip = vip->next )
		{
			strcat(*new_string, vip->name);
			if ((vip->next) && (separator))
			{
				strcat (*new_string, separator); 
			}
		}
	}
	else
	{
		*new_string  = (char *) repalloc(*new_string, 1 );
		*new_string[0] = '\0';
	}
	return (0);
}

global int get_intern_variables
(
	char	*internal_variable,
	char    ** out,
    struct trg_info			* tip 
)
{
	char					*local_variant = NULL;
	struct variant_info		*global_variants = NULL;
	int length = 0;
	char *p;
	char *tmp = NULL;
	
	*out = (char *)  palloc( 1 );
	*out[0] = '\0';

	if (tip)
	{
		local_variant = tip->localvariant;
		global_variants = tip->globalvariants;
	}
	else
	{
		local_variant = NULL;
		global_variants = NULL;
	}		

	if (strcmp (internal_variable, DSC_INTERNVAR_OLD_LVARIANT) == 0 )
	{
		if (local_variant)
		{
			*out = (char *) repalloc(*out, strlen(local_variant)+2  );
			strcpy(*out, "-");
			strcat(*out, local_variant);
		}
	}
	else
	if (strcmp (internal_variable, DSC_INTERNVAR_OLD_GVARIANT )==0)
	{
		get_global_variant_name(out, tip, "+","+");
	}
	else
	if (strcmp (internal_variable, DSC_INTERNVAR_OLD_ALLVARIANTS ) == 0)
	{
        get_global_variant_name(&tmp, tip, "+","+");
		if (local_variant)
		{
			*out = (char *) repalloc( *out,  strlen(local_variant) + 2 + strlen(tmp) );
			strcpy(*out, "-");
			strcat(*out, local_variant);
		}
		else
		{
			*out = (char *) repalloc( *out, strlen(tmp) + 1 );
			*out[0] = '\0';
		}
		if (strlen(tmp)>0)
			strcat(*out, tmp);
	}
	else
	if (strcmp (internal_variable, DSC_INTERNVAR_LAYER ) == 0)
	{
		if (tip->layer)
		{
			*out = (char *)  repalloc( *out, strlen(tip->layer) + 1 );
			strcpy (*out, tip->layer);
		}
	}
	else
	{
		*out = (char *)  repalloc( *out, strlen(tip->name) + 1 );
		if (strcmp (internal_variable, DSC_INTERNVAR_BASENAME ) == 0)
		{
			*out = (char *)  repalloc( *out, strlen(tip->name) + 1 );
			remove_ext( tip->name, *out );
		}
		else
        if ((strcmp (internal_variable, DSC_INTERNVAR_PUREBASENAME ) == 0)
			|| (strcmp (internal_variable, DSC_INTERNVAR_PUREFILENAME ) == 0))
		{
			*out = (char *)  repalloc( *out, strlen(tip->name) + 1 );
			get_basename(tip->name, *out);
			if (strcmp (internal_variable, DSC_INTERNVAR_PUREBASENAME ) == 0)
			{
				if ( tmp = extension_exists( *out ) )
				{
					*tmp = '\0';
					tmp = NULL;
				}
			}
		}
		else
		if (strcmp (internal_variable, DSC_INTERNVAR_FILENAME ) == 0)
		{
			strcpy ( *out, tip->name);
		}
		else
		if (strcmp (internal_variable, DSC_INTERNVAR_EXTENSION ) == 0)
		{
			if ( tmp = extension_exists( tip->name ) )
			{
				strcpy ( *out, tmp);
				tmp = NULL;
			}
		}
		else
		if (strstr (internal_variable, DSC_INTERNVAR_LVARIANT ) == internal_variable )
		{
			tmp = NULL;
			if (strlen(internal_variable) > strlen (DSC_INTERNVAR_LVARIANT))
			{
				p = internal_variable + strlen (DSC_INTERNVAR_LVARIANT);
				if ((p[0] == LBRACKET2) && (p[strlen(p)-1] == RBRACKET2 ))
				{
					if (strlen(p) > 2)
					{
						tmp  =  (char *)  palloc( strlen(p) - 1 );
						strncpy (tmp, p+1, strlen(p) - 2 );
						tmp[strlen(p) - 2] = '\0';
					}
				}
				else
				{
					*out = NULL;
					return (-2);
				}
			}
			if (tip->localvariant)
			{
				*out = (char *)  repalloc( *out, strlen(tip->localvariant) + ( tmp ? strlen(tmp) + 1 : 1 )) ;
				if (tmp)
					strcpy (*out, tmp);
				else
					(*out)[0] = '\0';
				strcat (*out, tip->localvariant);
			}
		}
		else
		if (strstr (internal_variable, DSC_INTERNVAR_GVARIANTS ) == internal_variable )
		{
			tmp = NULL;
			if (strlen(internal_variable) > strlen (DSC_INTERNVAR_GVARIANTS))
			{
				p = internal_variable + strlen (DSC_INTERNVAR_GVARIANTS);
				if ((p[0] == LBRACKET2) && (p[strlen(p)-1] == RBRACKET2 ))
				{
					if (strlen(p) > 2)
					{
						tmp  =  (char *)  palloc( strlen(p) - 1 );
						strncpy (tmp, p+1, strlen(p) - 2 );
						tmp[strlen(p) - 2] = '\0';
						if (p = strchr(tmp, ','))
						{
							/* tmp = prefix, p = separator */
                            *p = '\0';
							p++;
						}
						else
							p = default_variant_separator;
					}
				}
				else
				{
					*out = NULL;
					return (-2);
				}
			}
			if (tip->globalvariants)
				get_global_variant_name(out, tip, tmp,p);
		}
		else
		{
			*out = NULL;
			return (-2);
		}
	}
	FREE_AND_SET_TO_NULL(tmp);
	return (0);
}

/* old */
global int substitute_internalx_variables
(
	char					**subst_string ,
	struct trg_info			*tip
)
{
	char *p,save;
	char *start;
	struct variant_info * vip;
	char line[LINESIZE];
	char					*local_variant;
	struct variant_info		*global_variants;

	if (tip)
	{
		local_variant = tip->localvariant;
		global_variants = tip->globalvariants;
	}
	else
	{
		local_variant = NULL;
		global_variants = NULL;
	}

	line[0] = '\0';

	for ( start = *subst_string ; *start ; start = ++p )
	{
		p = strchr (start, INTERNAL_VARPREFIX);
		if ( ! p )
		{
			strcat (line, start);
			break;
		}
		
		save = *p;
		*p = '\0';
		strcat (line,start);
		*p = save;
		p++;


		switch ( *p )
		{
		case '*' :
		case '-' :	
				if (local_variant)
				{
					strcat(line, "-");
					strcat(line, local_variant);
				}
				if (*p == '-')
					break;
		case '+' :
				for (vip = global_variants; vip; vip = vip->next )
				{
					strcat(line, "+");
					strcat(line, vip->name);
				}
				break;
		/* treat "%%" as "%" */
		case  INTERNAL_VARPREFIX :
				strcat(line, "%");
				break;

        default  :
				DBG0 (( "error in internal variable substitution syntax '%s' \n", *subst_string ))
				eprotwrite ( "error in internal variable substitution syntax '%s' \n", *subst_string );
                return ( - 2 );
		}
	}
	if (line[0] != '\0')
	{
		/* found variant prefix */
		*subst_string = (char*) repalloc(*subst_string, strlen(line) + 1 );
		(void) strcpy(*subst_string, line);
		return 1;
	}
	else
		return 0;

}


/*==========================================================================*/

global int cmp_layer
(
    char	        * layer1 ,
    char	        * layer2
)
{
    if ( layer1 && layer2 )
    {
        return strcmp( layer1, layer2 );
    }
    else
    {
        if ( layer1 && ! layer2 )
            return 1;
        else
        {
            if ( ! layer1 && layer2 )
                return -1;
            else
                /* layer1 == NULL, layer2 == NULL */
                return 0;
        }
    }
}

/*==========================================================================*/

global void get_layer
(
    char	        * modname ,
    char	        ** layer
)
{
    /* int  layer_len; */

    if ( is_oldstyled_source( modname, layer, NULL ) )
    {
        /* is_oldstyled_source() set layer in regexmatches[ 2 ] */
        return ;
    }

    if ( is_qualified_source( modname, layer, NULL  ) )
    {

        /* is_qualified_source() set layer in regexmatches[ 1 ] */
        return ;
    }

	if ( is_qualified_desc( modname, layer, NULL  ) )
    {

        /* is_qualified_desc() set layer in regexmatches[ 1 ] */
        return ;
    }


    DBG2 (( "getlayer: layer not found for '%s' \n", modname ))
	return ;
}

/*==========================================================================*/

global int is_global_include
(
    char			* filename
)
{
	/* check if filename is a global include ( 00 include ) */

    int     rc = 0;
    char    * modname;

    modname = filename ;

	/* &gar -> */
	if ( vopt_no_ext_map )
    {
		char *ext ;
		ext	= get_extension (modname);
  	
		if ( !strcmp(ext, EXT_RC ) ||
			 !strcmp(ext, EXT_ICO) ||
			 !strcmp(ext, EXT_DLG) ||
			 !strcmp(ext, EXT_DEF) ||
			 !strcmp(ext, EXT_MC ) ||
			 !strcmp(ext, EXT_IDL )||
			 !strcmp(ext, EXT_YCC )||
			 !strcmp(ext, EXT_LEX )||
			 !strcmp(ext, EXT_RGS ) )
		return 0;
	}
	/* &gar */

    /* is_oldstyled_source() set numbers in regexmatches[ 3 ] */
    if ( is_oldstyled_source( modname, NULL, NULL ) )
    {
        if ( *(modname) == 'g' ) rc = 1;
        else if (   modname[ regexmatches[3].rm_so     ] == '0' &&
                    modname[ regexmatches[3].rm_so + 1 ] == '0' )
             rc = 1;
    }

    if ( rc )   return( 1 );
    else        return( 0 );
}

/*==========================================================================*/

global int is_include
(
    char	* filename
)
{
    int     rc = 0;
    char    * modname;

    modname = ( filename[0] == INDEX_SYMBOL ) ? skip_index( filename ) : filename;

	/* &gar -> */
	if ( vopt_no_ext_map )
    {
		char *ext ;
		ext	= get_extension (modname);
  	
		if ( !strcmp(ext, EXT_RC ) ||
			 !strcmp(ext, EXT_ICO) ||
			 !strcmp(ext, EXT_DLG) ||
			 !strcmp(ext, EXT_DEF) ||
			 !strcmp(ext, EXT_MC ) ||
			 !strcmp(ext, EXT_IDL )||
			 !strcmp(ext, EXT_YCC )||
			 !strcmp(ext, EXT_LEX )||
			 !strcmp(ext, EXT_RGS ) )
		return 0;
	}
	/* &gar */

    if ( * modname == 'h' && is_oldstyled_source( modname, NULL, NULL ) )
    {
		rc = 1;
    }
    else if ( is_global_include( modname ) )
    {
        rc = 1;
    }
	if ( rc ||
         ! strcmp( get_extension( modname ), EXT_H ) ||
         ! strcmp( get_extension( modname ), EXT_HPP ))
        rc = 1;

    if ( rc )
		return ( 1 );
	else
		return ( 0 );
}

/*==========================================================================*/

global char * get_extension
(
    char	* modname
)
{
	char	* p, * variant;

	if ( variant = strchr(modname, '-') )
		p = extension_exists (variant);
	else
		p = extension_exists(modname);

	if ( p )
		return ++p;
	else
		return modname + strlen(modname);
}

/*  &gar CTS 1000237 */
/*==========================================================================*/
global char * skip_extension
(
	char * modname
)
{
	char *p;

	if (p = extension_exists(modname))
		*p='\0';
	return modname;
}

/*===========================================================================*/

global char * skip_index
(
    char    * target
)
{
    char * modname;

    /* skip to  :.../.... */
    /*               ^    */

    modname = strrchr( target , '/' );
    if ( modname = strrchr( target , '/' ) )
    {
        return( ++modname );
    }
    /* remove a maximum of 2 INDEX_SYMBOLs */
    else
    {
        modname = target ;
        if ( modname[0] == INDEX_SYMBOL ) modname++;
        if ( modname[0] == INDEX_SYMBOL ) modname++;
        return ( modname );
    }
}

/*==========================================================================*/

global int is_qualified_source
(
    char    * target,
    char    **layer ,
    char    name [ NAMESIZE ]
)
{
    int rc=0;

    /* set layer in regexmatches[2] */
    if ( sourceindex_comp == NULL )
    {
        sourceindex_comp = (struct re_pattern_buffer *) palloc( sizeof(regex_t) );
        if ( rc = regcomp( sourceindex_comp, REGEXP_VALID_SOURCEINDEX, REG_EXTENDED ) )
        {
	        char error [ FILENAMESIZE ];
	        regerror( rc, sourceindex_comp, error, FILENAMESIZE-1 );
	        DBG0(( "vmake: error compile regular expression: %s\n", error ));
            exit( 3 );
        }
    }

    if ( regexec( sourceindex_comp, target, 10, regexmatches, 0 ) )
		return (is_mapped_source(target, layer, name)); /* ( PTS 1105037 ) */
    else
    {
        int len;

        if ( layer )
        {
            /* is_qualified_source() set layer in regexmatches[ 1 ] */
            len = regexmatches[1].rm_eo - regexmatches[1].rm_so;

            *layer = (char *) repalloc( *layer, len + 1 );
            (void) strncpy( *layer, target + regexmatches[ 1 ].rm_so, len );

            (*layer)[ len ] = '\0';
        }
        if ( name )
        {
            /* is_qualified_source() set name in regexmatches[ 2 ] */
            len = regexmatches[ 2 ].rm_eo - regexmatches[ 2 ].rm_so;
            (void) strncpy( name, target + regexmatches[ 2 ].rm_so,
                (len < NAMESIZE) ? len : NAMESIZE-1 );
            name[ (len < NAMESIZE) ? len : NAMESIZE-1 ] = '\0';
        }
        return( 1 );
    }
}


/* =================================================== */
/* test for mapping of targetname                      */
/* set layer and name if found and reference not NULL  */
/* return 1 if found else return 0                     */
/* ( PTS 1105037 )                                     */
/* =================================================== */

global int is_mapped_source
(
    char    * target,            /* IN */
    char    **layer , /* OUT */
    char    name [ NAMESIZE ]    /* OUT */
)
{
	struct dirmap_info *dmip;
	int rc = 0;
	char *p	= strchr(target, '_');
	
	DBG4 (( "is_mapped_source: target '%s' \n", target ))
	if ( p )
	{
		/* set '_' to '\0' */
		*p = '\0';
		if ( ! map_lists )
		{
			rc = get_map_info ();
			if ( rc <= 0 )
			{
				*p = '_';
				return ( rc );
			}
		}
		rc = -1 ;

		for ( dmip = map_lists ; dmip ; dmip = dmip->next )
		{
			rc = strcmp ( dmip->shortform , target );
			if ( rc >= 0 ) break ;
		}
		/* reset to '_' */
		*p = '_';
		/* mapping info found */
		if (rc == 0)
		{
			if (name)
				(void) strcpy( name , target);
			if (layer)
            {
				DBG4 (( "map %s to %s\n", target, dmip->layer))
                *layer = (char *) repalloc( *layer, strlen( dmip->layer ) + 1 );
				(void) strcpy( *layer, dmip->layer );
            }
			return 1;
		}
	}
	return 0;
}

/*==========================================================================*/

/* =================================================== */
/* test for mapped source with full path               */
/* -> path can cut because vmake found it with dirmaps */
/* return 1 if found else return 0                     */
/* ( PTS 1105037 )                                     */
/* =================================================== */

global int is_mapped_source_with_full_path
(
    char    * target,            /* IN */
    char    **layer ,            /* OUT */
    char    name [ NAMESIZE ]    /* OUT */
)
{
	struct dirmap_info  * dmip;
	int                 rc = 0;
	char                * p;
	char                * map_name;

	DBG4 (( "is_mapped_source_with_full_path: target '%s' \n", target ))
		
	map_name = strrchr( target, '/' );  /* cut the last name */
	if ( ! map_name )
		return 0;                    /* without layer */
	if ( ! (p = strchr( ++map_name, '_' )) )
		return 0;                    /* not mapped file */
	
	
	/* set '_' to '\0' */
	*p = '\0';
	if ( ! map_lists )
	{
		rc = get_map_info ();
		if ( rc <= 0 )
		{
			*p = '_';
			return ( rc );
		}
	}
	rc = -1 ;

	for ( dmip = map_lists ; dmip ; dmip = dmip->next )
	{
		rc = strcmp ( dmip->shortform , map_name);
		if ( rc >= 0 ) break ;
	}
	/* reset to '_' */
	*p = '_';
	/* mapping info found */
	if (rc == 0)
	{
		if (name)
			(void) strcpy( name , map_name);
		if (layer)
        {
            *layer = (char *) repalloc( *layer, strlen( dmip->layer ) + 1 );
			(void) strcpy( *layer, dmip->layer );
			DBG4 (( "map %s to %s\n", target, dmip->layer))
        }
		return 1;
	}
	return 0;
}


/*==========================================================================*/

global int is_oldstyled_source
(
    char    * modname,
    char    ** layer,
    char    name [ NAMESIZE ]
)
{
    int rc=0;

    if ( oldstyledmodule_comp == NULL )
    {
        oldstyledmodule_comp = (struct re_pattern_buffer *) palloc( sizeof(regex_t) );
        if ( rc = regcomp( oldstyledmodule_comp, REGEXP_OLDSTYLED_MODBODY, REG_EXTENDED ) )
        {
	        char error [ FILENAMESIZE ];
	        regerror( rc, oldstyledmodule_comp, error, FILENAMESIZE-1 );
	        DBG0(( "vmake: error compile regular expression: %s\n", error ));
            exit( 3 );
        }
    }

    if ( regexec( oldstyledmodule_comp, modname, 10, regexmatches, 0 ) )
    {

        if ( vopt_no_ext_map )
        {
            char    * ext;

            ext = get_extension( modname );

	        if ( !strcmp(ext, EXT_RC ) ||
		         !strcmp(ext, EXT_ICO) ||
		         !strcmp(ext, EXT_DLG) ||
		         !strcmp(ext, EXT_DEF) ||
		         !strcmp(ext, EXT_MC ) ||
		         !strcmp(ext, EXT_IDL )||
		         !strcmp(ext, EXT_YCC )||
		         !strcmp(ext, EXT_LEX )||
		         !strcmp(ext, EXT_RGS ) )
            {
                if ( layer )
                {
                    *layer = (char *) repalloc( *layer, strlen( "Resource" ) + 1 );
                    (void) strcpy( *layer, "Resource" );
                }
                if ( name )
                    (void) strcpy( name, modname );

		        return ( 1 );
            }
	        else
		        return ( 0 );
        }
        return( 0 );
    }
    else
    {
        int len;
        if ( layer )
        {
            /* is_oldstyled_source() set layer in regexmatches[ 2 ] */
            len = regexmatches[ 2 ].rm_eo - regexmatches[ 2 ].rm_so;

            *layer = (char *) repalloc( *layer, len + 1 );
            (void) strncpy( *layer, modname + regexmatches[ 2 ].rm_so, len );
            (*layer)[ len ] = '\0';
        }
        if ( name )
            (void) strcpy( name, modname );

        return( 1 );
    }

}

/*==========================================================================*/

global int is_valid_sourcename
(
    char	* modname ,
    char    **layer ,
    char    name [ NAMESIZE ]
)
{

	if ( is_qualified_source( modname, layer, name ) ||
         is_oldstyled_source( modname, layer, name ) )
	{
		DBG4 (( "isvalmod: valid module name '%s' \n", modname ))
		return ( 1 );
	}
    else
    {
		DBG4 (( "isvalmod: no valid module name '%s' \n", modname ))
    	return ( 0 );
    }
}

/*==========================================================================*/

global int is_qualified_desc
(
    char    * target,
    char    ** layer ,
    char    name [ NAMESIZE ]
)
{
    int rc=0;

    /* set layer in regexmatches[2] */
    if ( descindex_comp == NULL )
    {
        descindex_comp = (struct re_pattern_buffer *) palloc( sizeof(regex_t) );
        if ( rc = regcomp( descindex_comp, REGEXP_VALID_DESCINDEX, REG_EXTENDED ) )
        {
	        char error [ FILENAMESIZE ];
	        regerror( rc, descindex_comp, error, FILENAMESIZE-1 );
	        DBG0(( "vmake: error compile regular expression: %s\n", error ));
            exit( 3 );
        }
    }

    if ( regexec( descindex_comp, target, 10, regexmatches, 0 ) )
    {
        DBG4(( "isqualdesc: '%s' isn't valid index \n", target ))
        return( 0 );
    }
    else
    {
        int len;
        DBG4(( "isqualdesc: '%s' is valid index \n", target ))
        if ( layer )
        {
            /* is_qualified_desc() set layer in regexmatches[ 2 ] */
            len = regexmatches[ 2 ].rm_eo - regexmatches[ 2 ].rm_so;

            *layer = (char *) repalloc( *layer, len + 1 );
            (void) strncpy( *layer, target + regexmatches[ 2 ].rm_so, len );
            (*layer)[ len ] = '\0';
        }
        if ( name )
        {
            /* is_qualified_desc() set name in regexmatches[ 3 ] */
            len = regexmatches[ 3 ].rm_eo - regexmatches[ 3 ].rm_so;

            (void) strncpy( name, target + regexmatches[ 3 ].rm_so, len );
            name[ len ] = '\0';
        }
        return( 1 );
    }
}

/*==========================================================================*/

global int is_valid_descname
(
    char	* modname,
    char    ** layer ,
    char    name [ NAMESIZE ]
)
{
    char    * p ;
    int     len;
    int     knowntype = 0;

    p = get_extension( modname );
    if ( name )
    {
        len = strlen( modname );
        (void) strncpy( name, modname , NAMESIZE - 1 );
        name[ (len < NAMESIZE) ? len : NAMESIZE - 1 ] = '\0';
    }

    if ( is_qualified_desc( modname, layer, name )  )
        return( 1 );

    if ( *p )
    {
        /* check for known types */
        if ( !strcmp( p, MAC_LST ) ||
             !strcmp( p, PRJ_LST ) ||
             !strcmp( p, LNK_LST ) ||
             !strcmp( p, SHM_LST ) ||
             !strcmp( p, DLL_LST ) ||
             !strcmp( p, SHR_LST ) ||
             !strcmp( p, REL_LST ) ||
             !strcmp( p, LIB_LST ) ||
             !strcmp( p, JP_LST ) ||
             !strcmp( p, COM_LST ) )
            knowntype = 1;
    }

    if ( knowntype && modname[0] != ':' && ( !strrchr( modname , '/' ) || isalpha (modname[0])) )
        return( 1 );

    return( 0 );
}

/*==========================================================================*/

global	int	check_dir /* takes a file name (not a directory) */
(
    char			* file
)
{
	int			    lgt , c , rc=0 ;
	char			* p , *q;
	char			path [ FILENAMESIZE ];
	char			dirbuf [ FILENAMESIZE ];

	(void) strcpy ( path , file );
	p = strrchr ( path , '/' );
	if ( ! p ) return ( 0 ); /* filename only (in current directory) */
	*p = 0 ; /* terminate the directory path */

	rc = test_is_dir ( path );
	if ( rc == 0 ) return ( 0 ); /* exists already */

	(void) sprintf ( dirbuf , "mkdir %s \n", path );
	if ( opt_print )
	{
		voutwrite ( dirbuf );
	}
	xprotwrite ( dirbuf );
	if ( opt_noexec ) return ( 0 );

	(void) strcpy ( dirbuf , path );

	for ( p = dirbuf ; * p ; p ++ )
	{
	    while ( * p == '/' ) p ++ ;
	    while ( (* p != '/') && (* p != '\\') && (* p != '\0') ) p ++ ;
	    c = * p ;
	    * p = '\0' ;

	    rc = test_is_dir ( dirbuf );
	    if ( rc )
	    {
		    if ( opt_print ) voutwrite ( "mkdir %s\n", dirbuf );
		    if ( ! opt_noexec )
		    {
	            q = strchr ( path , NODE_SEPARATOR );
	            if ( ! q || ((q - path) < 2) ) /* C: is drive name */
	            {
		            rc = mkdir ( dirbuf , 0777 );
		            if ( rc == -1 )
		            {
		                DBG0 (( "cannot create directory '%s': %s \n",
						        dirbuf , serrno() ))
		                return ( 2 );
		            }
                }
                else
                {
                    char    node[ NAMESIZE ];
	                DBG7 (( "check_dir: remote access \n" ))

	                FILL ( node , 0 , sizeof(node) );
	                lgt = p - path ;
	                if ( lgt >= sizeof(node) )
	                {
		                DBG0 (( "check_dir: node name too long: '%s' \n", path ))
		                seterrno(EINVAL) ;
		                return ( -1 );
	                }
	                COPY ( node , path , lgt );
	                DBG6 (( "check_dir: node  '%s' \n", node  ))
                    rc = makedir( node, path + lgt + 1);
                    if ( rc ) return( rc );
                }
		    }
	    }

	    if ( c == '\0' ) break ;
	    * p = (char) c ;
	}

	if ( opt_noexec ) return ( 0 );
	rc = test_is_dir ( path );
	if ( rc )
	{
	    DBG0 (( "failed to create directory '%s' \n", path ))
	    return ( rc );
	}

	return ( 0 );
}

/*==========================================================================*/

global	char	getdefvers
(
    char    version
)
{
    if ( env_VMAKE_DEFAULT )
        return( * env_VMAKE_DEFAULT );

    switch ( version )
    {
    case 'f' :
        return 'f' ;
    case 'q' :
    case 's' :
        return 'q' ;
    default  :
        return ' ' ;
    }
}
/*==========================================================================*/

void remove_ext
(
    char    * target,
    char    * name
)
{
    char    * p;
	DBG4(( "remext: name '%s'\n", target ))
    if ( p = extension_exists( target ) )
        STRCPY( name, target, p - target + 1 )
    else
        STRCPY( name, target, (int) strlen( target ) + 1 )
	DBG4(( "remext: name '%s'\n", name ))
}

/*==========================================================================*/

global int propagate_value
(
/* in */ struct env_info    * eip
)
{
    char    * p, * q;
	char    * value = NULL;

    /* eip->value contains the new value */
    p = getenv( eip->variable );
	/* &gar PTS 1002090 -> */
	if ( p )
	{
		value = (char*) palloc (strlen( p ) + 1 );
		if ( value == NULL )
		{
			DBG0 (( "ERROR: insufficient dynamic memory \n" ))
			return ( - 3 );
		}
		strcpy (value,p);
	}
	/* <- PTS 1002090 */
    q = (char*) palloc( strlen( eip->value ? eip->value : EMPTY ) +
    strlen( eip->variable ) + 2 );
	if ( q == NULL )
	{
		DBG0 (( "ERROR: insufficient dynamic memory \n" ))
		return ( - 3 );
	}
	(void) sprintf( q, "%s=%s", eip->variable, eip->value ? eip->value : EMPTY  );
	/*
	if (eip->value)
        (void) sprintf( q, "%s=%s", eip->variable, eip->value );
	else
		(void) sprintf( q, "%s", eip->variable  );
	*/
	if ( putenv( q ) )
	{
		DBG0(( "make: error propagating environment \n" ))
	}
	/* CTS 1103623 FREE_AND_SET_TO_NULL(q); */
	if ( eip->value ) FREE_AND_SET_TO_NULL(eip->value);
	eip->value = value;
	/* eip->value contains now the old value */
    return( 0 );
}


/*==========================================================================*/

/*==========================================================================*/

global int propagate_env
(
/* in */ struct env_info    * eip
)
{
    /* char    * p, * q; */
	int rc=0;

    for( ; eip ; eip = eip->next )
    {
		
		rc=propagate_value(eip);
		if ( rc<0 ) return rc;
    }
    return( 0 );
}


/*==========================================================================*/

int rename_file_case_sensitive
(
/* in */ char *directory,
/* in */ char *source,
/* in */ char* target
)
{
	char name_src [ FILENAMESIZE ];
	char name_trg [ FILENAMESIZE ];
	int rc;

	sprintf(name_src, "%s%s", directory, source);
	sprintf(name_trg, "%s%s", directory, target);

	DBG0 (( "rename '%s' to '%s' \n", name_src, name_trg ))
	if ( opt_noexec ) return ( 0 );

	rc = rename( name_src, name_trg);
	
	DBG4 (( "rfcs: returning %d \n", rc ))
	return rc;
}


/*==========================================================================*/
/* PTS 1104605 */
char * extension_exists
(
/* in */  char * filename
)
{
	char *pos;
	
	if (! filename) return (char*) 0;
	pos = strrchr (filename, '.');
	if ( ( pos < strrchr (filename, '/')) || ( pos < strrchr (filename, '\\')) )
		return (char* ) 0;

	return pos;
}

/* ------------------------------------------- */
/* CTS 1105673                                 */
/* for different wrk-directories               */
/* ------------------------------------------- */
global int     init_wrk (void)
{

	if (! DIR_WRK)
		set_DIR_WRK();
	
    pDIR_INC = (char *) repalloc( pDIR_INC, strlen( pDIR_WRK ) + 15 + 1 );
    sprintf(pDIR_INC,   "%sincl/%%s/",      pDIR_WRK);

    pDIR_SIZE = (char *) repalloc( pDIR_SIZE, strlen( pDIR_WRK ) + 15 + 1 );
	sprintf(pDIR_SIZE,  "%ssize/",          pDIR_WRK);

    pDIR_TMP = (char *) repalloc( pDIR_TMP, strlen( pDIR_WRK ) + 15 + 1 );
	sprintf(pDIR_TMP,   "%s%%s/tmp/",        pDIR_WRK);

    pDIR_OBJ = (char *) repalloc( pDIR_OBJ, strlen( pDIR_WRK ) + 15 + 1 );
	sprintf(pDIR_OBJ,   "%s%%s/obj/%%s/",    pDIR_WRK);

    pDIR_PROT = (char *) repalloc( pDIR_PROT, strlen( pDIR_WRK ) + 15 + 1 );
	sprintf(pDIR_PROT,  "%s%%s/prot/",      pDIR_WRK);

    pDIR_GSIZ = (char *) repalloc( pDIR_GSIZ, strlen( pDIR_WRK ) + 15 + 1 );
	sprintf(pDIR_GSIZ,  "%s%%s/gsize/",     pDIR_WRK);

    pDIR_PSRC = (char *) repalloc( pDIR_PSRC, strlen( pDIR_WRK ) + 15 + 1 );
	sprintf(pDIR_PSRC,  "%s%%s/psrc/",      pDIR_WRK);

    pDIR_CSRC = (char *) repalloc( pDIR_CSRC, strlen( pDIR_WRK ) + 15 + 1 );
	sprintf(pDIR_CSRC,  "%s%%s/csrc/",      pDIR_WRK);

    pDIR_DATES = (char *) repalloc( pDIR_DATES, strlen( pDIR_WRK ) + 15 + 1 );
	sprintf(pDIR_DATES, "%s%%s/dates/%%s/", pDIR_WRK);

    pDIR_BROWSE = (char *) repalloc( pDIR_BROWSE, strlen( pDIR_WRK ) + 15 + 1 );
	sprintf(pDIR_BROWSE,"%s%%s/browse/%%s/",pDIR_WRK);

    return 0;
}

global void set_DIR_WRK()
{
	char * env_WRK_NAME = getenv ("WRK_NAME");

	if (env_WRK_NAME)
	{
        pDIR_WRK = (char *) repalloc( pDIR_WRK, strlen( env_WRK_NAME ) + 15 + 1 );
		sprintf(pDIR_WRK,"%%s/sys/%s/", env_WRK_NAME);
	}
	else
	{
        pDIR_WRK = (char *) repalloc( pDIR_WRK, strlen( STD_WRK_NAME ) + 15 + 1 );
		sprintf(pDIR_WRK,"%%s/sys/%s/", STD_WRK_NAME);
	}
}
	

/* ------------------------------------------- */
/* CTS 1105673                                 */
/* for different wrk-directories               */
/* ------------------------------------------- */
global int     init_outdir (void)
{

	char * env_VMAKE_OUTPUT;
	
	env_VMAKE_OUTPUT = getenv ("VMAKE_OUTPUT_DIR");


	if (env_VMAKE_OUTPUT)
        pDIR_OUT = env_VMAKE_OUTPUT;
	else
	{
        pDIR_OUT = (char *) palloc( strlen(env_OWN) + strlen( STD_OUT_DIR ) +  2 );
		strcpy ( pDIR_OUT, env_OWN);
		strcat ( pDIR_OUT, "/");
		strcat ( pDIR_OUT, STD_OUT_DIR);
	}

    return 0;
}


/* CTS 1112006 -> */

global int clear_outputlist ()
{
	struct output_info *pOutput = output_list;

	while ( pOutput )
	{
		free ( pOutput->line );
		output_list = pOutput->next;
		free (pOutput);
		pOutput = output_list;
	}
	return 0;	
}

global int add_to_outputlist (char *line, int line_len)
{
	struct output_info *pOutput = (struct output_info *) palloc ( sizeof(struct output_info) );
	struct output_info **oipp;

	for ( oipp = &output_list ; *oipp ; oipp = &(*oipp)->next );

	*oipp = pOutput;
	pOutput->line = (char *) palloc ( line_len +1 );
	pOutput->next = NULL;
	strncpy (pOutput->line, line, line_len);
	pOutput->line[line_len]='\0';
	return (0);
}

global int flush_outputlist (struct trg_info * tip, int status, int wrnout )
{
	struct output_info *pOutput = output_list;
	int writeOut = status  || ( output_list && opt_warning_protocol && wrnout );

	if (opt_silent_mode && output_list)
	{
		if (opt_silent_mode == VMAKE_SILENT_MODE_ROTATE) 
            dowrite_to_console("\b\n", 1);
		else
			if (opt_silent_mode != VMAKE_SILENT_MODE_NOTHING) 
				dowrite_to_console("\n", 1);
		dowrite_to_console(outstampline, strlen(outstampline));
		current_silent_char = -1;
		current_silent_char_count = 0;
	}

	if ( writeOut)
		if (status)
			ewrite_targetheader(tip, status);
		else
			wwrite_targetheader(tip, status);
	while ( pOutput )
	{
		if ( writeOut)
			if (status)
				eprotwriteline ( pOutput->line );
			else
				wprotwriteline ( pOutput->line );
			
		if (opt_silent_mode)
		{
			dowrite_to_console(pOutput->line, strlen(pOutput->line));
		}
		free ( pOutput->line );
		output_list = pOutput->next;
		free (pOutput);
		pOutput = output_list;
	}
	if ( writeOut)
		if (status)
			eprotwrite ( EPROT_SEPARATOR );
		else
			wprotwrite ( EPROT_SEPARATOR );

	return 0;	
}

/* =========================================== */

global	int ewrite_all 
(
	struct trg_info * tip, 
	int errno_output,
	int status,
    char    * fmt ,
    ...
)
{
	int     lgt, rc ;
	va_list	args ;
	char    line [ LINESIZE ] ;

	va_start ( args, fmt );
#ifdef WIN32
	rc = _vsnprintf ( line , LINESIZE-1, fmt , args );
#else
	rc = vsprintf ( line , fmt , args );
#endif
	va_end ( args );
#ifdef WIN32
	if ( rc == -1 )
/*	 line to short for args */
			line [ LINESIZE-1 ] = '\0';
#endif
	lgt = strlen ( line );
	
	if (opt_silent_mode)
	{
		dowrite_to_console("\n", 1);
		dowrite_to_console(outstampline, strlen(outstampline));
	}

	DBG0 (( "%s", line ))
	ewrite_targetheader (tip, status);
	eprotwrite ( "%s", line );
	eprotwrite ( "\n=>%s", serrno() );
	eprotwrite ( EPROT_SEPARATOR ); 
	return (status);
}


global int ewrite_targetheader( struct trg_info * tip, int status)
{
	int rc=0;
	char name_without_variants[LINESIZE];
	
	(void) get_basename( tip->name, name_without_variants );
	switch (tip->type)
	{
		case TT_INC :
		case TT_MEM :
		case TT_MOD :
			eprotwrite ("TARGET: ':%s/%s'", tip->layer, name_without_variants );
			break;

		case TT_LIB :
		case TT_REL :
		case TT_SHR :
		case TT_DLL :
		case TT_PGM :
		case TT_SHM :
		case TT_JP  :
		case TT_MAC :
		case TT_PRJ :
			if (tip->layer)
				eprotwrite ("TARGET: '::%s/%s'", tip->layer, name_without_variants );
			else
				eprotwrite ("TARGET: '%s'", name_without_variants );
			break ;
		case TT_FILE :
			eprotwrite ("TARGET: file '%s' in %s", name_without_variants, tip->act_caller->name );
			break ;
		case TT_CMD :
		case TT_EXECUTE :
			eprotwrite ("TARGET: command '%s' in %s", name_without_variants, tip->act_caller->name );
			break ;
		case TT_DIRLIST :
			eprotwrite ("TARGET: filelist '%s'", tip->name );
			break ;
		case TT_EXTRA :
			eprotwrite ("TARGET: extra file '%s'", name_without_variants );
			break ;
		case TT_OPTION :
			eprotwrite ("TARGET: '%s' in %s", name_without_variants, tip->act_caller->name );
			break ;
		default :
			eprotwrite ("TARGET: '%s' - illegal target type", name_without_variants );
			rc = -2 ;
			DBG0 (( "ERROR: illegal target type in '%s' ", tip->name ))
			break ;
	}
	
	if (status)
		eprotwrite ("     STATUS: ERROR");
	else
		eprotwrite ("     STATUS: OUTPUT");

	if (tip->responsible)
	{
		eprotwrite ("     RESPONSIBLE: %s", tip->responsible);
	}
	else
	if ( tip->type == TT_CMD && tip->act_caller && tip->act_caller->responsible )
	{
		eprotwrite ("     RESPONSIBLE: %s", tip->act_caller->responsible);
		eprotwrite ("     CALLER:      %s", tip->act_caller->responsible);
	}
	eprotwrite ("     DATE: %s", ascdate(time ((time_t*) 0 )));		
	eprotwrite ("\n\n");
	return rc;	
}

/* <- CTS 1112006 */


global int wwrite_targetheader( struct trg_info * tip, int status)
{
	int rc=0;
	char name_without_variants[LINESIZE];
	
	(void) get_basename( tip->name, name_without_variants );
	switch (tip->type)
	{
		case TT_INC :
		case TT_MEM :
		case TT_MOD :
			wprotwrite ("TARGET: ':%s/%s'", tip->layer, name_without_variants );
			break;

		case TT_LIB :
		case TT_REL :
		case TT_SHR :
		case TT_DLL :
		case TT_PGM :
		case TT_SHM :
		case TT_JP  :
		case TT_MAC :
		case TT_PRJ :
			if (tip->layer)
				wprotwrite ("TARGET: '::%s/%s'", tip->layer, name_without_variants );
			else
				wprotwrite ("TARGET: '%s'", name_without_variants );
			break ;
		case TT_FILE :
			wprotwrite ("TARGET: file '%s' in %s", name_without_variants, tip->act_caller->name );
			break ;
		case TT_CMD :
		case TT_EXECUTE :
			wprotwrite ("TARGET: command '%s' in %s", name_without_variants, tip->act_caller->name );
			break ;
		case TT_DIRLIST :
			wprotwrite ("TARGET: filelist '%s'", tip->name );
			break ;
		case TT_EXTRA :
			wprotwrite ("TARGET: extra file '%s'", name_without_variants );
			break ;
		case TT_OPTION :
			wprotwrite ("TARGET: '%s' in %s", name_without_variants, tip->act_caller->name );
			break ;
		default :
			wprotwrite ("TARGET: '%s' - illegal target type", name_without_variants );
			rc = -2 ;
			DBG0 (( "ERROR: illegal target type in '%s' ", tip->name ))
			break ;
	}
	
	if (status)
		wprotwrite ("     STATUS: ERROR");
	else
		wprotwrite ("     STATUS: WARNING");

	if (tip->responsible)
	{
		wprotwrite ("     RESPONSIBLE: %s", tip->responsible);
	}
	else
	if ( tip->type == TT_CMD && tip->act_caller && tip->act_caller->responsible )
	{
		wprotwrite ("     RESPONSIBLE: %s", tip->act_caller->responsible);
		wprotwrite ("     CALLER:      %s", tip->act_caller->responsible);
	}
	wprotwrite ("     DATE: %s", ascdate(time ((time_t*) 0 )));		
	wprotwrite ("\n\n");
	return rc;	
}




global int     createPIDFile (pid_t pid)
{
	struct stat statbuf;
	int rc;
	char buffer[15];
	int len;
	
	if (  opt_noexec + opt_collection + opt_modlist> 0  )
		return (0);

	PIDFilename = (char*) palloc ( strlen(VMAKE_PIDFILE) + strlen (DIR_WRK) 
		           + strlen(env_OWN) + ( env_SUPPRESS_VMAKE_PIDFILE ? 8 : 1  ));
	sprintf (PIDFilename, pDIR_WRK, env_OWN);
	strcat (PIDFilename, VMAKE_PIDFILE);
	if ( env_SUPPRESS_VMAKE_PIDFILE )
	{
		(void) sprintf ( buffer , "%ld", (long) pid );
		strcat(PIDFilename,buffer);
	}

	rc = filestat ( PIDFilename , & statbuf );
	if ( rc < 0 )
	{
		if ( (geterrno() == ENOENT) || (geterrno() == ENOTDIR) )
		{
			DBG8 (( "Pidfile not found    '%s'\n", PIDFilename ))
		}
		else
		{
			DBG0 (( "createPIDFile: cannot stat '%s': %s \n",
						PIDFilename , serrno() ))
			return ( - 2 );
		}
	}
	else
	{	
		DBG0 (("Warning: vmake pidfile (%s) already exists !\n", PIDFilename ))
		fd_PIDFile = open ( PIDFilename, O_RDONLY, 0644 );	
		if ( fd_PIDFile < 0 )
		{
			DBG0 (( "cannot read old pid file: %s \n", serrno() ))
			return ( -2 );
		}
		
		if (testLock(fd_PIDFile) != 0)
		{
			len = read (fd_PIDFile, buffer, 15);
			if ( len <= 0 )
			{
				DBG0 (( "cannot read pid from pidfile: %s \n", serrno() ))
				return (-2);
			}
			buffer [len] = '\0';

			DBG0 (( "Error: an other make (PID %s) is running in this area !\n", buffer ));
			return (-3);
		}
		close (fd_PIDFile);
	}

	fd_PIDFile = open ( PIDFilename, O_WRONLY|O_CREAT|O_TRUNC, 0644 );
	if ( fd_PIDFile < 0 )
	{
		DBG0 (( "cannot create pid file: %s \n", serrno() ))
		return ( -2 );
	}

	(void) sprintf ( buffer , "%ld", (long) pid );
	len = strlen(buffer);
	rc = write ( fd_PIDFile , buffer , (unsigned) len );
	if ( rc != len )
	{
		DBG0 (( "error while writing pid file: %s \n", serrno() ))
		return ( -2 );
	}
	return( setLock(fd_PIDFile));
}	


global int checkforPIDFile (int info)
{
	int PID;
	struct stat statbuf;
	int rc;
	char buffer[15];
	int len;

	if (!DIR_WRK)
		set_DIR_WRK();

	if (PIDFilename == NULL)
	{
		PIDFilename = (char*) palloc ( strlen(VMAKE_PIDFILE) + strlen (DIR_WRK) + strlen(env_OWN) +1 );
		sprintf (PIDFilename, DIR_WRK, env_OWN);
		strcat (PIDFilename, VMAKE_PIDFILE);
	}

	rc = filestat ( PIDFilename , & statbuf );
	if ( rc < 0 )
	{
		if ( (geterrno() == ENOENT) || (geterrno() == ENOTDIR) )
		{
			DBG8 (( "Pidfile not found    '%s'\n", PIDFilename ))
			return (0);
		}
		else
		{
			DBG0 (( "createPIDFile: cannot stat '%s': %s \n",
						PIDFilename , serrno() ))
			return ( - 2 );
		}
	}
	else
	{	
		if (info)
			DBG0 (("Warning: vmake pidfile (%s) already exists !\n", PIDFilename )) 
		fd_PIDFile = open ( PIDFilename, O_RDONLY, 0644 );	
		if ( fd_PIDFile < 0 )
		{
			DBG0 (( "cannot read old pid file: %s \n", serrno() ))
			return ( -2 );
		}
		
		if (testLock(fd_PIDFile) != 0)
		{
			len = read (fd_PIDFile, buffer, 15); 
			if ( len <= 0 )
			{
				if (info)
					DBG0 (( "cannot read pid from pidfile: %s \n", serrno() ))
				return (-2);
			}
			buffer [len] = '\0';
			PID = atoi(buffer);
			if (info)
				DBG0 (( "Error: an other make (PID %s) is running in this area !\n", buffer ));
			return (PID);
		}
		close (fd_PIDFile);
		unlinkPIDFile(0);
	}
	return (0);
}


global int     unlinkPIDFile (int unlockfile)
{
	if (  opt_noexec + opt_collection + opt_modlist > 0 )
		return (0);
	if (unlockfile)
		(void) unlock (fd_PIDFile); 

/*(void) unlock (fd_PIDFile); */
	close (fd_PIDFile);
	return unlink (PIDFilename);
}	

global int writePIDtoFile(pid_t pid)
{
	char buffer[15];
	int len;

	if ( env_SUPPRESS_VMAKE_PIDFILE  || ( opt_noexec + opt_collection + opt_modlist > 0 ) )
		return (0);
	
	if ( lseek( fd_PIDFile, 0L, SEEK_SET ) != 0)
	{
		DBG0 (( "error while rewriting pid file (seek): %s \n", serrno() ))
		return ( -2 );
	}
	
	(void) sprintf ( buffer , "%ld", (long) pid );
	len = strlen(buffer) + 1;

	if ( write ( fd_PIDFile , buffer , (unsigned) len ) != len )
	{
		DBG0 (( "error while writing pid file: %s \n", serrno() ))
		return ( -2 );
	}
	return( 0);
}



/* Lock functions */

#ifdef unix

int setLock (int fd)
{
	int rc;
	struct flock lock[1];
	lock->l_type=F_WRLCK;
	lock->l_whence=SEEK_SET;
	lock->l_start=0;
	lock->l_len=0;		/* whole file */
	rc = fcntl(fd,F_SETLK,lock);
	if(rc != 0)
	{
		DBG0(( "Error: set lock failed (%s)\n",serrno() ))
		eprotwrite ("Error: set lock failed (%s)\n",serrno() );	
		return -1;
	
	}
	return 1;
}



int testLock (int fd)
{
	/*
        	file unlocked:  return 0
        	file locked:    return pid
        	error:          return -1
	*/
	int rc;
	struct flock lock[1];
	
	lock->l_type=F_WRLCK;
    lock->l_whence=SEEK_SET;
	lock->l_start=0;
    lock->l_len=0;		/* whole file */
	rc=fcntl(fd,F_GETLK,lock);
	if ( rc != 0 )
	{
		DBG0(( "Error: test lock failed (%s)\n",serrno() ))
		return -1;
	}
	if ( lock->l_type == F_UNLCK ){
		return 0;
	}
	
    return (int) lock->l_pid;
}

int unlock (int fd)
{
	int rc;
	struct flock lock[1];
	lock->l_type=F_UNLCK;
	lock->l_whence=SEEK_SET;
	lock->l_start=0;
	lock->l_len=0;		/* whole file */
	rc=fcntl(fd,F_SETLK,lock);
	if ( rc != 0 )
	{
		DBG1(( "Error: unlock failed (%s)\n",serrno() ))
		return -1;
	}
	return 0;
}

void checkforStopRequest (char * current_activity)
{
}


#else
/* WINDOWS*/

int setLock (int fd){
    BOOL rc;
	OVERLAPPED ovlpd[1];
	ovlpd->Offset=0;
	ovlpd->OffsetHigh=0;
	ovlpd->hEvent=NULL;
	rc = LockFileEx (
		(HANDLE) ((long) _get_osfhandle(fd)),
		LOCKFILE_FAIL_IMMEDIATELY |
		0,
		0,
		0xffffffff,
		0xffffffff,		/* whole file */
		ovlpd
	);		
	if (!rc)
	{
		DBG0(( "Error: set lock failed - %s\n",serrno()))
		eprotwrite ("Error: set lock failed - %s\n",serrno());	
		return -1;
	}	
	return 0;
}

int unlock (int fd)
{
	BOOL rc;
	OVERLAPPED ovlpd[1];
	ovlpd->Offset=0;
	ovlpd->OffsetHigh=0;
	ovlpd->hEvent=NULL;

	rc = UnlockFileEx(
		(HANDLE) ((long) _get_osfhandle(fd)),
		0,
		0xffffffff,
		0xffffffff,		/* whole file */
		ovlpd
	);

	if (!rc)
	{
		DBG1(( "Error: unlock failed - %s\n",serrno()))
		return -1;
	}
	return 0;
}



int testLock (int fd)
{
    /*
          file unlocked:  return 0
          file locked:    return 1
          error:          return -1
    */
	BOOL rc;
	OVERLAPPED ovlpd[1];
	ovlpd->Offset=0;
	ovlpd->OffsetHigh=0;
	ovlpd->hEvent=NULL;
	rc = LockFileEx(
		(HANDLE) ((long) _get_osfhandle(fd)),
		LOCKFILE_FAIL_IMMEDIATELY |
		LOCKFILE_EXCLUSIVE_LOCK,
		0,
		0xffffffff,
		0xffffffff,	/* whole file */
		ovlpd
	);
	if(!rc)
	{
		if ( GetLastError() == ERROR_LOCK_VIOLATION || GetLastError() == ERROR_SHARING_VIOLATION){
			return 1;	
		}
		else{
			DBG0(( "Error: testlock failed - %s\n",serrno()))
			eprotwrite ("Error: testlock failed - %s\n",serrno());	
			return -1;
		}
	}
	return ( unlock(fd));

}


#endif


global int set_finished (struct trg_info *tip)
{
	if (tip->finished > 1 )
	{
		DBG2 (( "State finished already set for target '%s'\n", tip->name))
	}
	else
	{
		finished_target_counter++;
		tip->finished = 2;
	}

	return (0);
}

global	int		read_dir_info    (char* path, struct dir_info** dipp)
{
	return (get_dir_info(path, dipp));
}

global int recursive_dircopy
(
	char *src,
	char *dest,
	int   filetype, 
	int   silent
)
{
	struct dir_info         * dirp = NULL;
	struct dir_entry        * direp;
	int                     rc = 0;
	int                     errcnt = 0;

    DBG3 (("rec_dircp %s to %s \n", src, dest))

	DBG0 (("%s/... (%s)\n", src, (filetype == FT_BIN) ? "binary" : "ascii"  ))
	rc = read_dir_info ( src , & dirp );
	if (rc >= 0)
	{
		direp = dirp->entries;
		if (direp)
		{
			DBG8 (( "Parse entry '%s' ...\n", direp->name ));

			errcnt += allcopy (direp, src, dest, filetype, silent);
			/* if (direp->left)
				DBG0 (( "rdcp: Parse entry '%s' (leftt)...\n", direp->left->name ));
			errcnt += allcopy (direp->left, src, dest, filetype, silent);
			if (direp->right)
				DBG0 (( "rdcp:      Parse entry '%s' (right) ...\n", direp->right->name ));
			errcnt += allcopy (direp->right, src, dest, filetype, silent);
			*/
		}
	}
	else
	{
		errcnt++;
	}
	return (errcnt);
}


global int allcopy
(
	struct dir_entry        * direp,
	char                    * src,
	char                    * dest,
	int                       filetype, 
	int                       silent
)
{
	char                    *p_src=NULL, *p_dest=NULL;
	int                     errcnt = 0;
	int	                    rc;
	struct stat fileinfo;
	
	if (direp)
	{
		/* copy itself */
		p_src = (char*) palloc (strlen(src) + strlen(direp->name) + 2);
		strcpy (p_src, src);
		strcat (p_src, "/");
		strcat (p_src, direp->name);
		p_dest = (char*) palloc (strlen(dest) + strlen(direp->name) + 2);
		strcpy (p_dest, dest);
		strcat (p_dest, "/");
		strcat (p_dest, direp->name);

		rc = filestat ( p_src , & fileinfo );
		if ( rc < 0 )
	    {
    		DBG8 (( "rec_dircp: cannot stat '%s': %s \n", p_src , serrno() ))
			errcnt++;
	    }
		if ( rc == 0 )
		{
			if ( fileinfo.st_mode & S_IFDIR )
			{
				rc = recursive_dircopy (p_src, p_dest, filetype, silent);
				errcnt += rc;
			}
			else
			{
				if (! silent)
					DBG0 (("%s (%s)\n", p_dest, (filetype == FT_BIN) ? "binary" : "ascii"  ))
				rc = vcopy_file ( p_src , p_dest , 0 , filetype );
				if ( rc ) 
					errcnt ++;
			}
		}

		/* copy left */
		DBG8 (( "Found entry '%s' on the left site of '%s' ...\n", (direp->left) ? direp->left->name : "NOTHING", direp->name ));
		errcnt += allcopy (direp->left, src, dest, filetype, silent);
		/*copy right */
		DBG8 (( "           Found entry '%s' on the right site of '%s' ...\n", (direp->right) ? direp->right->name : "NOTHING", direp->name ));
		errcnt += allcopy (direp->right, src, dest, filetype, silent);
	}
	return (errcnt);
}

global FILE* get_tmp_file ()
{

	FILE    * myfile = NULL;
	char    * tempdir = NULL; 


#ifdef unix
	myfile  = tmpfile ();
#else
	if (tempdir = getenv("TEMP"))
	{
		char* myfilename = NULL;

	   if( ( myfilename = _tempnam( tempdir, "vmake_" ) ) != NULL )
	   {
           myfile = fopen( myfilename, "w+D" );
	   }
	   else
		   DBG0 (("ERROR: can't create a unique filename under %s\n", tempdir))
	}
#endif
	return (myfile);
}


/*
global int get_filelist_of_directory
(
	char *directory,
	char *filter
)
{
	struct dir_info         * dirp = NULL;
	struct dir_entry        * direp;
	int                       rc;

    rc = read_dir_info ( directory , & dirp );
	if (rc >= 0)
	{
		direp = dirp->entries;
        if (direp)
		{

*/
/* 
struktur directory ->entries
                   ->directory
				   -> dep_info
layer und filter muss passen
*/

/*
int get_left_side
{
	struct file_info 

*sfipp = (struct file_info *) palloc (sizeof (struct file_info));
					FILL (  *sfipp , 0 , sizeof(struct file_info) );
}
*/


