/*


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
#include "vmakdist.h"
#include "vmakvars.h"
#include "vmakmisc.h"
#include <stdio.h>
#include <string.h>

/***********************************************/
/* int insert_dist (char *)                    */
/* ------------------------------------------- */
/* insert File (full path name) in distlist    */
/* 1. Check for Path in INST_ROOT              */
/* 2. Cut INST_ROOT from path                  */
/* 3. Insert in distlist (insert_distributes)  */
/* ------------------------------------------- */
/* return: 1=inserted 0=not inserted <0=Error  */
/***********************************************/
global int insert_dist 
(
	char *fullfilename
)
{
	char *distname;
	int rc=0;
	
#ifdef WIN32
	char *p;
	int len_INSTROOT= strlen(env_INSTROOT);
/* covert / to \ if it in env_INSTROOT */ 
	for ( p = fullfilename; p < fullfilename + len_INSTROOT; p++ )
	{
		if ( *p == '/' )
			*p = '\\';
	}
#endif
	
	/* 1. Test for existence */
	/* if directory not in INST_ROOT => not distribute */
	if ( strncmp(fullfilename, env_INSTROOT, strlen(env_INSTROOT)) != 0 )
	{
		DBG0 (( "Warning: targetdirectory of  %s not in INST_ROOT\n", fullfilename ))
		DBG0 (( "File %s ignored\n", fullfilename ))
	}
	else
	{
		/* 2. Cut INST_ROOT */
		distname=fullfilename+strlen(env_INSTROOT)+1;
		rc=insert_distributes(distname);
	}
	return rc;
}

/************************************************/
/* int insert_distributes (char *)              */
/* -------------------------------------------- */
/* insert File in distlist - INST_ROOT/filename */
/* ist the full filename                        */
/* 1. Test for existence                        */
/* 2. Search for entry in distlist              */
/* 3. Insert if not existence                   */
/* -------------------------------------------- */
/*  return: 1=inserted 0=not inserted <0=Error  */
/************************************************/
global int insert_distributes
(
	char *filename
)
{
	struct dist_info **distpp, *distp;
	char fullfilename [ FILENAMESIZE ];
	int rc=0;
	struct stat		statbuf ;
#ifdef WIN32
	char *p, *f_last;
#endif

	DBG2 (( "insdstb: called \n" ))
	DBG3 (( "insdstb: target  '%s' \n", filename  ))

	if ( ( filename[0] == '/' )
	#ifdef WIN32
			|| ( isalpha(filename[ 0 ] ) && ( filename[1] == ':' ) )
	#endif
			)
	{
		(void) strcpy ( fullfilename, filename );		
		if ( strncmp (env_INSTROOT , fullfilename, strlen(env_INSTROOT)) == 0 )
		{
			strcpy (filename, fullfilename + strlen(env_INSTROOT) + 1);
		}
		else
		{
			DBG0 (( "Can't file %s in INSTROOT '%s' \n", fullfilename, env_INSTROOT ))
			return -3;
		}
	}
	else
		(void) sprintf ( fullfilename, "%s/%s", env_INSTROOT, filename );

	

#ifdef WIN32
/* covert / to \ if it in env_INSTROOT */ 
	f_last = fullfilename + strlen( fullfilename );
	for ( p = fullfilename; p <= f_last; p++ )
	{
		if ( *p == '/' )
			*p = '\\';
	}
#endif
	

	if ( stat ( fullfilename , &statbuf ) != 0)
	{
		DBG0 (( "Warning: targetfile %s for distribute not found\n", fullfilename ))
		/* return 0; */
	}
#ifdef WIN32
	f_last = filename + strlen( filename );
	for ( p = filename; p <= f_last; p++ )
	{
		if ( *p == '\\' )
			*p = '/';
	}
#endif

	rc = 1;
	for ( distpp = &dist_list ; *distpp!= NULL ; distpp = &(*distpp)->next )
	{
		rc= strcmp ( (*distpp)->name , filename );
		if ( rc >= 0 ) break ;
	}
	if ( rc==0 )
	{
		DBG4 (( "insdstb: filename '%s' already in list \n", (*distpp)->name ))
	}
	else
	{
		distp = (struct dist_info *) palloc ( sizeof(struct dist_info) );
	    if ( distp == NULL )
	    {
		    DBG0 (( "insdistb: insufficient dynamic memory \n" ))
		    return ( -3 );
	    }
		FILL (  distp , 0 , sizeof(struct dist_info) );

		distp->name = (char *) palloc( strlen( filename ) + 1 );
		(void) strcpy ( distp->name, filename );

		distp->next = *distpp;
		*distpp = distp;
		DBG0 (( "%s\n", filename )) 
		rc=1;
	}
	DBG4 (( "insdstb: returning %d \n", rc ))
	return rc;
}


global int create_distlist
(
	char *distname
)
{
	struct dist_info *distp;
	char basename[ FILENAMESIZE ] ;
	char filename[ FILENAMESIZE ] ;
	int rc;
	FILE* fp;

	DBG2 (( "crdstb: called \n" ))
	DBG3 (( "crdstb: target  '%s' \n", distname  ))

	(void*) get_basename(distname,basename);
	(void*)skip_extension(basename);
	(void) sprintf ( filename , "%s/etc/%s.lst", env_INSTROOT, basename);

	/* PTS 1108423 */
	rc = check_dir ( filename );
	if ( rc ) return ( rc );

	DBG0 (( "Creating distribution file '%s'\n", filename ))
	fp = fopen( filename, "w" );
	if ( ! fp )
	{
		DBG0 (( "error opening '%s' \n", filename ))
		return -3;
	}
	for ( distp = dist_list ; distp != NULL ; distp = distp->next )
	{
		fprintf( fp, "%s\n", distp->name );
	}
    fclose( fp );
	return 0;
}
