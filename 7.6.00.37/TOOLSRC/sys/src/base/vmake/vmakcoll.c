/*      vmakcoll.c       

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
#include "vmakcoll.h"
#include "vmake.h"
#include "vmakvars.h"
#include "vmakmisc.h"
#include "vmakcomm.h"
#include "vmakdist.h"
#include <stdio.h>
#include <string.h>
#include "regex.h"
#include <assert.h>

/* imported functions */

extern int		concom (char*, char*);

/* local functions */

static	int		collect_target  (char*,char,char,int,int,struct mod_com_info*,struct trg_info*,struct trg_info**, char *, struct variant_info *);
static	int		collect_module  (char*,char,int,struct mod_com_info*,struct trg_info*,struct trg_info**, char *, struct variant_info *);
static	int		collect_file    (char*,char*,char,struct trg_info*,struct trg_info**);
static	int		collect_option  (char*,char*,struct trg_info*,struct trg_info**);
static	int		collect_command (char*,char,struct trg_info*,struct trg_info**);
static	int		collect_descriptions (struct trg_info*,struct mod_com_info_list*,struct mod_com_info*);
static	int		collect_include_list (char*,struct trg_info*,struct inc_info*, int);
static  int		collect_demand_list (char*,struct trg_info*,struct dem_mod_info*,struct dem_info**,unsigned int, int);
static  int		collect_dep_list (char*,struct trg_info*,struct inc_info*, int);
static  int		collect_obj_list (char*,struct trg_info*,struct obj_info*,char*,struct variant_info*);
static  int		collect_extdep_list (char*,struct trg_info*,struct extdep_info*);
static	int		collect_extra   (char*,struct mod_com_info*,struct trg_info*,struct trg_info**);
static  int     collect_target_option (char*,struct trg_info*,struct mod_com_info*);
static  int     collect_parentlinkoptions (char*,struct trg_info*,struct opt_info*);
static  int     collect_list (char*, char, char, int, struct mod_com_info*,struct trg_info*,struct trg_info**);

global  int     collect_directory (char*, char*, char*, char, char, int, struct mod_com_info*, struct trg_info*, int, int);
global int      collect_directory_entries (char*, char*, struct dir_entry*,char*, char, char, int, struct mod_com_info*, struct trg_info*,int, int);


global	int		get_com_info    (char*,char,char*, struct mod_com_info**);
global	int		get_com_info_list (char*,char,char*, struct mod_com_info_list**);
static	char	get_language    (char*);
static	int		get_lang_info   (void);
static	int		get_langext_info(void);
static	int		get_dsc_file    (char*,char,char*,char*, time_t*, int);
static	int		get_dsc_line    (char*,int,VFILE*,char*);
static	int		get_extra_info  (void);

static	int		insert_target   (int,char,unsigned,char*,struct trg_info*,struct trg_info**,struct mod_com_info*);
static	int		move_target   (struct trg_info*,struct trg_info**,struct trg_info**);
static	int		insert_description (struct dsc_info*,struct dsc_info**,struct trg_info*, int);
static	int		insert_mod_inc  (char*,struct mod_com_info*);
static	int		insert_mod_opt  (char*,struct mod_com_info*);
static	int		insert_mod_dsc  (char*,char*,struct mod_com_info*);
static  int		insert_mod_demands (char*,char*,struct mod_com_info*,struct dem_mod_info**);
static  int		insert_mod_dep  (char*,struct mod_com_info*);
static  int		insert_mod_extdep  (char*,struct mod_com_info*);
static	int		insert_mod_output  (char*,struct mod_com_info*);
static  int		insert_mod_obj  (char*,struct mod_com_info*);
static  int     insert_mod_parentlinkoption (char*,struct mod_com_info*);
static	int		insert_mod_com_info (struct mod_com_info*,struct mod_com_info**);
static  int     append_mod_com_info (struct mod_com_info**,struct mod_com_info*);
static  int     insert_file (struct file_info*,char[ FILENAMESIZE ],char);
static  int     insert_file_to_filelist(struct file_info**,char*,char);
static  int     insert_parentlinkoptions (struct trg_info *,struct opt_info*,int);
static  int     insert_options ( struct opt_info **, struct opt_info *);



static  int		insert_demands (struct trg_info*,char*,struct dem_info**);

static  char    *insert_to_optionlist (char*);

static  int		analyze_extra_line  (char*);
static	int		analyze_target_line (char*,struct mod_com_info*,char);
static	int		analyze_description (char*,struct mod_com_info*);
static  int     analyze_lang_line   (char*);
static  int     analyze_langext_line(char*);
static  int     analyze_comtype (struct	mod_com_info* );
static  int     analyze_com_variants (struct mod_com_info*);


static  int     target_exists   (char*,char,unsigned,struct trg_info *,struct trg_info**,struct mod_com_info*);
static  int     find_target     (int,char,unsigned,char*,struct trg_info *,struct trg_info**,struct mod_com_info*);
static	int     remove_dependency (char*,struct trg_info *, int);
static	int     remove_from_callers(struct trg_info *,struct trg_info *);

static	int     remove_demands (char*,char*,struct dem_info**);
static	int    	clear_dependencies (struct trg_info*, int);
static	int    	clear_demands (char*,struct dem_info**);
static  int     clear_descriptions (struct trg_info *);

static	int		substitute      (char*,char**);
static  int     is_std_ext      (char*);

/* PTS 1105037 for DirMapping*/
static  int     analyze_map_line (char *);
/* CTS 1110367 */
char check_nodistrib (char *);

#ifdef	NOCONCOM
static	int		concom          (char*,char*);
#endif	/*NOCONCOM*/

#ifndef WIN32
#define strnicmp strncasecmp
#endif

/* local data */

static char newversion = '?', newdefault = '?';

/*==========================================================================*/


global	int     collect 
(   
    char                * trgline   ,
    char                defvers     ,
    char                subdflt     ,
    int                 dep_type    ,
    int                 level       ,
    struct trg_info     * ctip      ,
    struct trg_info     ** tipp		
)
{
	int                     rc=0 ;
	struct mod_com_info     ov_mci ;
	char                    version, savvers, savdef ;
	char                    versdef ;
	char                    * target ;
    int                     trgtype;
    int                     gtt;
    int                     stop_level;
	/* PTS 1104596 */
	int						target_found; 
	/*0= not found, 1=found , 2= found with other deptype */
	struct trg_info			* other_deptype_tip;
	struct variant_info     * global_variants = NULL;
	char                    * local_variant = NULL;
	char                    * tmp_target;
	int                     ctip_layer_already_added = 0;
	char					*caller_layer = NULL; 
	char					*subst_string = NULL;


    DBG2 (( "collect: called \n" ))
	DBG3 (( "collect: trgline  '%s' \n", trgline ))
	DBG3 (( "collect: version  '%c' \n", defvers ))
	DBG3 (( "collect: level     %d  \n", level ))


	* tipp     = NULL ;
    stop_level = GTT_UNKNOWN;

    /* save information */
    savvers = newversion; savdef = newdefault ;

    /* initialize the module compile information */
	FILL ( &ov_mci , 0 , sizeof(struct mod_com_info) );
    
	ov_mci.special_target_flag = ' ' ;
	ov_mci.version = ' ' ;
	ov_mci.versdef = ' ' ;

	rc = analyze_target_line ( trgline, & ov_mci , defvers );
	if ( ( rc > 0 ) && ov_mci.name && strchr(ov_mci.name, INTERNAL_VARPREFIX) )
		rc = substitute_internal_variables(&(ov_mci.name), ctip);

	if ( ( rc > 0 ) && ov_mci.file && strchr(ov_mci.file->file, INTERNAL_VARPREFIX) )
		rc = substitute_internal_variables(&(ov_mci.file->file), ctip);

	if ( rc <= 0 )
	{
		FREE_AND_SET_TO_NULL(ov_mci.name);
		FREE_AND_SET_TO_NULL(ov_mci.file);
		FREE_AND_SET_TO_NULL(ov_mci.defaultlayer);
		FREE_AND_SET_TO_NULL(ov_mci.objdemanddir);
		FREE_AND_SET_TO_NULL(ov_mci.srcdemanddir);
		DBG4 (( "collect: analyze target line error %d \n", rc ))
	    return ( rc );
	}
	
	if ( opt_distrib && ov_mci.nodistrib)
	{
		FREE_AND_SET_TO_NULL(ov_mci.name);
		FREE_AND_SET_TO_NULL(ov_mci.file);
		FREE_AND_SET_TO_NULL(ov_mci.defaultlayer);
		FREE_AND_SET_TO_NULL(ov_mci.objdemanddir);
		FREE_AND_SET_TO_NULL(ov_mci.srcdemanddir);
		return 0;
	}

	/* CTS 1110258 */
	if ( ov_mci.version == 'n' )
	{
		DBG0 (( "Error: description error in %s\n", ctip ? ctip->name : trgline )) 
		DBG0 (( "       option 'noversion' for target %s found\n", ov_mci.name))
		DBG0 (( "       (the option 'noversion' is allowed in compile descriptions only)\n" ))

		eprotwrite( "Error: description error in %s\n", ctip ? ctip->name : trgline );
		eprotwrite( "       option 'noversion' for target %s found\n", ov_mci.name);
		eprotwrite( "       (the option 'noversion' is allowed in compile descriptions only)\n" );
		return -1;
	}

    target = ov_mci.name ;
	DBG7 (( "collect: target         '%s' \n", target ))

	if ( ov_mci.version != ' ' )
	    version = ov_mci.version ;
	else
	    version = defvers ;

    if ( newversion != '?' && ov_mci.version == ' ') 
    {
        version = newversion;
        DBG7 (( "collect: explicit target version '%c' \n", version ))
    }
    DBG7 (( "collect: target version '%c' \n", version ))

	if ( ov_mci.versdef != ' ' )
	    versdef = ov_mci.versdef ;
	else
	    versdef = subdflt ;
    if ( newversion != '?' && ov_mci.versdef == ' ') 
    {
        versdef = newdefault;
        DBG7 (( "collect: explicit target version '%c' \n", versdef ))
    }
	DBG7 (( "collect: target default '%c' \n", versdef ))

	/*
	 *  The default version must not be slower than the target version.
	 *  Otherwise there could be slower objects in faster
	 *  object directories!
	 */
	switch ( version )
	{
	case 'f' :
	    versdef = 'f' ;
	    break ;
	case 'q' :
	    if ( versdef == 's' ) versdef = 'q' ;
	    break ;
	}

	if (ctip && ctip->layer)
			caller_layer = ctip->layer;


    if ( ov_mci.special_target_flag == '?' )
    {
        rc = collect_target_option( trgline , ctip , &ov_mci );
	    if ( rc < 0 )
	    {
			DBG4 (( "collect: internal option  error %d \n", rc ))
            return ( rc );
	    }
	    return ( rc );
    }

    if ( ov_mci.special_target_flag == CMD_PREFIX )
	{
	    if ( ctip )
    		version = ctip->version ;
	    else
	    	version = defvers ;
	    DBG8 (( "collect command '%s' \n", trgline ))
	    rc = collect_command ( trgline , version , ctip , tipp );
	    if ( rc <= 0 )
	    {
			DBG4 (( "collect: command error %d \n", rc ))
	    }
	    return ( rc );
	}
	/* is file */
	if (( ov_mci.special_target_flag == '/' ) && ov_mci.file)
	{
	    DBG8 (( "collect file '%s' \n", ov_mci.file->file ))
	    rc = collect_file ( ov_mci.name , ov_mci.file->file , version , ctip , tipp );
	    if ( rc <= 0 )
	    {
			DBG4 (( "collect: file error %d \n", rc ))
			return ( rc );
	    }
	    return ( rc );
	}

	if ( ov_mci.special_target_flag == '-' )
	{
	    if ( ov_mci.file && (ov_mci.file->file[0] == '-') && (ov_mci.file->file[1] == '>') )
	    {
		    if ( ! ov_mci.file->file[2] )
		    {
		        DBG0 (( "WARNING: no target redirection '%s'\n", trgline ))
				eprotwrite ( "WARNING: no target redirection '%s'\n", trgline );

		    }
		    else
		    {
                struct stat fileinfo;
                char        name [ NAMESIZE ];
				/* CTS 1110367 */
				char        nodistrib = check_nodistrib (ov_mci.file->file);


		        if (ctip) ctip->fixed_file = TRUE ;
                rc = stat( ov_mci.file->file + 2, & fileinfo );
			
                if ( ( ( rc == 0 && ( fileinfo.st_mode & S_IFDIR )) ||  
					( ov_mci.file->file[ strlen( ov_mci.file->file ) - 1 ] == '/' ))            /* CTS 1107534 */
					&& ctip )
                {
					char temp_ext[ EXTENSIONSIZE ];
					FILL ( temp_ext, 0, EXTENSIONSIZE );
					
                    remove_ext( ctip->name, name );
					switch ( ctip->type )
					{
					case TT_DLL :
						(void) strcat( temp_ext, "." );
						(void) strcat( temp_ext, env_DLL_TYPE );
						break;
					case TT_LIB :
						(void) strcat( temp_ext, "." );
						(void) strcat( temp_ext, env_ARC_TYPE );
						break;
					case TT_PGM :
					case TT_SHM :
#ifdef WIN32
						(void) strcat( temp_ext, ".exe" );
#endif
						break;
					case TT_REL :
					case TT_SHR :
					default :
						(void) strcat( temp_ext, "." );
						(void) strcat( temp_ext, get_extension (ctip->name) );
						break;
					}

					(void) cut_global_variants (name);

					if ( ov_mci.file->file[ strlen( ov_mci.file->file ) - 1 ] != '/' )   /* CTS 1107469 */
                    {
    					ov_mci.file->file = (char *) repalloc( ov_mci.file->file, strlen( ov_mci.file->file ) + strlen( name ) + strlen(temp_ext) + 1 + 1 );
						(void) strcat( ov_mci.file->file, "/" );
                        (void) strcat( ov_mci.file->file, name );
						(void) strcat( ov_mci.file->file, temp_ext );
                    }
                    else
                    {
    					ov_mci.file->file = (char *) repalloc( ov_mci.file->file, strlen( ov_mci.file->file ) + strlen( name ) + strlen(temp_ext)+ 1 );
                        (void) strcat( ov_mci.file->file, name );
						(void) strcat( ov_mci.file->file, temp_ext );
                    }
                }
				else
				{
					if ( ov_mci.file->file[ strlen( ov_mci.file->file ) - 1 ] == '*' )            /* CTS 1107534 */
					{
						char temp_ext[ EXTENSIONSIZE ];
						FILL ( temp_ext, 0, EXTENSIONSIZE );

						ov_mci.file->file[ strlen( ov_mci.file->file ) - 1 ] = '\0';
						/* remove . from extension */
						if (  ov_mci.file->file[ strlen( ov_mci.file->file ) - 1 ] == '.' )
						{
							ov_mci.file->file[ strlen( ov_mci.file->file ) - 1 ] = '\0';
						}

						if (ctip)
						{
							switch ( ctip->type )
							{
							case TT_DLL :
								(void) strcat( temp_ext, "." );
								(void) strcat( temp_ext, env_DLL_TYPE );
								break;
							case TT_LIB :
								(void) strcat( temp_ext, "." );
								(void) strcat( temp_ext, env_ARC_TYPE );
								break;
							case TT_PGM :
							case TT_SHM :
	#ifdef WIN32
								(void) strcat( temp_ext, ".exe" );
	#endif
								break;
							case TT_REL :
							case TT_SHR :
							default :
								(void) strcat( temp_ext, "." );
								(void) strcat( temp_ext, get_extension (ctip->name) );
								break;
							}
						}
                        ov_mci.file->file = (char *) repalloc( ov_mci.file->file, strlen( ov_mci.file->file ) + strlen( temp_ext ) + 1 );
					    (void) strcat( ov_mci.file->file, temp_ext );
					}
                    
					/* <- CTS 1107534 */
				}
				DBG8 (( "explicit target file name '%s' \n", ov_mci.file->file + 2 ))
				if (ctip)
				{
					rc = insert_file( &ctip->file, ov_mci.file->file + 2, nodistrib );
					if (rc < 0)
						return rc;
					if (ctip->globalvariants)
					{
						char *variantfile = NULL;
			
						if ( create_globalvariant_filename(ov_mci.file->file + 2, &variantfile, ctip->globalvariants) < 0 )
							return -3;
						if ( insert_file_to_filelist( &ctip->variantfile, variantfile, 1 ) < 0 )
							return -3;
					}
				}
				return( rc); /* CTS 1110367 */
		    }
	    }
		DBG8 (( "collect: option '%s' \n", ov_mci.file->file ))
		rc = collect_option ( ov_mci.name , ov_mci.file->file , ctip , tipp );
		if ( rc <= 0 )
	    {
			DBG4 (( "collect: file error %d \n", rc ))
			return ( rc );
	    }
	    return ( rc );
	}

	if ( trgline[0] == '.') 
	{
		int count = 0;
		char *pstart, *pslash, *p;
		if (!caller_layer)
		{
			DBG0 (( "can't find layer to interprete '%s'\n", trgline ))
			eprotwrite ( "can't find layer to interprete '%s'\n", trgline );
			return -2;
		}
		tmp_target = (char*) palloc (strlen(target) + strlen(caller_layer) + 2);
		strcpy(tmp_target,caller_layer);
		/* cut the last "/" if exists */
		if (tmp_target[strlen(tmp_target)-1]== '/')
			tmp_target[strlen(tmp_target)-1] ='\0';
		
		pstart = target;
		pslash = pstart;
		while (*pstart)
		{
		
			while (*pslash && *pslash != '/')
			{	pslash++; }
			if ( (strncmp (pstart, "..", pslash - pstart)==0) && (pslash - pstart == 2))
			{
				p = strrchr(tmp_target,'/');
				if ( ! p )
				{
					if ( *tmp_target)
					{
						*tmp_target = '\0';
					}
					else
					{
						DBG0 (( "can't use target '%s' with default layer '%s'\n", trgline, caller_layer ))
						eprotwrite ( "can't use target '%s' with default layer '%s'\n", trgline, caller_layer );
						return -2;
					}
				}
				else
				{
					*p = '\0';
				}
			}
			else
				if (strncmp (pstart, ".", pslash - pstart))
				{
					if (*tmp_target)
						strcat (tmp_target,"/");
					strncat (tmp_target, pstart, pslash - pstart);
				}
	
			if ( *pslash == '/') pslash++;
			pstart = pslash;
		}
		FREE_AND_SET_TO_NULL(target);
		target=tmp_target;
		ctip_layer_already_added = 1;
		
	}


	gtt = analyze_source( &target, &trgtype, 
			&local_variant, &global_variants,
			((ctip) && ((ctip->type) >= TT_LIB) && ! ov_mci.novariant ) ? 
					ctip->globalvariants : NULL);

	if ( gtt < 0 )
	{
		DBG0 (( "error while analyzing '%s'\n", trgline ))
		eprotwrite ( "error while analyzing '%s'\n", trgline );
		return gtt;
	}

	if ((isalpha(*trgline) || (trgline[0] == '.')) &&  (strchr(target,'/')) )
	{
		if ( ( gtt == GTT_DESC ) || ( gtt == GTT_KNOWN_DESC ) )
		{
			if (caller_layer && ! ctip_layer_already_added)
			{
				tmp_target = (char*) palloc ( strlen(target) + strlen(caller_layer) + 4);
				(void) sprintf ( tmp_target, "::%s/%s", caller_layer, target );
			}
			else
			{
				tmp_target = (char*) palloc ( strlen(target) + 4);
				(void) sprintf ( tmp_target, "::%s", target );
			}
			FREE_AND_SET_TO_NULL(target);
			target=tmp_target;

		}
		else
		{
			if (caller_layer && ! ctip_layer_already_added)
			{
				tmp_target = (char*) palloc ( strlen(target) + strlen(caller_layer) + 3);
				(void) sprintf ( tmp_target, ":%s/%s", caller_layer, target );
			}
			else
			{
				tmp_target = (char*) palloc ( strlen(target) + 2);
				(void) sprintf ( tmp_target, ":%s", target );
			}
			FREE_AND_SET_TO_NULL(target);
			target=tmp_target;
		}
	}


	ov_mci.name=target;

    /* reset optimization if a big target */
    if ( opt_nodircache && gtt != GTT_SRC && ! opt_nodependencies ) 
        opt_nodircache = 0;
	target_found = target_exists( target , version , dep_type , ctip , tipp, &ov_mci );
	
	if ( target_found == 2 )
	{
		other_deptype_tip = *tipp; 
		*tipp = 0;
	}

    if ( target_found == 1 )
	{
		if ( ctip ) 
        {
			if ( ov_mci.stat.deptype == DT_OBJDEMDEP ) /* 'demand' in description */
            {
			    if ( insert_demands ( *tipp , "", &ctip->objdemands ) < 0 )
			    {
				    DBG4 (( "collect: insert_dem error %d \n", rc ))
				    return ( rc );
			    }
            }
			if ( ov_mci.stat.deptype == DT_SRCDEMDEP ) /* 'demand' in description */
            {
			    if ( insert_demands ( *tipp , "", &ctip->srcdemands ) < 0 )
			    {
				    DBG4 (( "collect: insert_dem error %d \n", rc ))
				    return ( rc );
			    }
            }
            if ( ov_mci.remake ) 
            {
    			if ( insert_dependency ( *tipp , &ctip->remakes ) < 0 )
                {
				    DBG4 (( "collect: insert_dep error %d \n", rc ))
				    return ( rc );
                }
            }
			/* CTS 1105891 -> */
			if ( ov_mci.nobind ) 
            {
    			if ( insert_dependency ( *tipp , &ctip->nobinds ) < 0 )
                {
				    DBG4 (( "collect: insert_dep error %d \n", rc ))
				    return ( rc );
                }
            }
            /* <- CTS 1105891 */
			/* CTS 1105891 -> */
			if ( ov_mci.on_success ) 
            {
    			if ( insert_dependency ( *tipp , &ctip->on_success ) < 0 )
                {
				    DBG4 (( "collect: insert_dep error %d \n", rc ))
				    return ( rc );
                }
            }
            /* <- CTS 1105891 */

			rc = insert_dependency ( ctip , & (*tipp)->callers );
		}
		else 
    		rc = 1 ;
        
        /* ad ->file to files */
        if ( ov_mci.file && ov_mci.file->file && *ov_mci.file->file )
            rc = insert_file( &(*tipp)->file, ov_mci.file->file, 0 );
		/* PTS 1108150 */
		if ( ov_mci.stat.distribute)
			(*tipp)->stat.distribute = ov_mci.stat.distribute ;

	    DBG4 (( "collect: returning %d \n", rc ))

		return ( rc );
	}

	if ( gtt == GTT_LIST )
	{
		DBG4 (( "collect: explicit list ('%d') \n", trgtype ))
		rc = collect_list ( target , version , versdef , trgtype , & ov_mci , ctip , tipp);
		if ( rc < 0 )
		{
			DBG4 (( "collect: DLL error %d \n", rc ))
			return ( rc );
		}
	}
	else
	{
		if ( gtt == GTT_KNOWN_DESC )
		{
			DBG4 (( "collect: explicit target description ('%d') \n", trgtype ))
			rc = collect_target ( target , version , versdef , dep_type ,
						trgtype , & ov_mci , ctip , tipp, local_variant, global_variants );
			if ( rc < 0 )
			{
				DBG4 (( "collect: error %d \n", rc ))
				return ( rc );
			}
			if ( rc == 0 )
			{
				DBG0 (( "unknown target '%s', ", trgline ))
				DBG0 (( "version '%c' \n", version ))
				eprotwrite ( "unknown target '%s' (%s)\n", trgline, versname(version) );
				if ( ctip != NULL )
				{
					DBG0 (( "called by target '%s', ", ctip->name ))
					DBG0 (( "version '%c' \n", ctip->version ))
					eprotwrite ( "   called by target '%s' (%s) \n", ctip->name, versname(ctip->version) );
				}
				rc = - 1 ;
			}
		}
		else
		{
			if ( gtt == GTT_SRC )
			{
				DBG4(( "collect: switch search level to LEVEL_MODULE for '%s' \n", target ))
				level = LEVEL_MODULE;
			}
			/* CTS 1105891 -> */
			else 
				if (ov_mci.nobind)
				{
					DBG0 (( "Warning: nobind used without known extension for %s\n", target ))
					DBG0 (( "         in the description %s \n", ctip->name ))
					eprotwrite ( "Warning: nobind used without known extension for %s\n", target );
					eprotwrite ( "         in the description %s \n", ctip->name );
					level=GTT_UNKNOWN; /* reset to unknown */
				}
			/* <- CTS 1105891 */

			if ( gtt == GTT_DESC ) 
				stop_level = GTT_DESC;
			do
			{
				switch ( level )
				{
				default :
					DBG8 (( "try to interpret target as a macro \n" ))
					rc = collect_target ( target , version , versdef , dep_type ,
								TT_MAC , & ov_mci , ctip , tipp, local_variant, global_variants );
					if ( rc < 0 )
					{
						DBG4 (( "collect: macro error %d \n", rc ))
						return ( rc );
					}
					if ( rc > 0 ) break ;

				case LEVEL_PROJECT :
					DBG8 (( "try to interpret target as a project \n" ))
					rc = collect_target ( target , version , versdef , dep_type ,
								TT_PRJ , & ov_mci , ctip , tipp, local_variant, global_variants );
					if ( rc < 0 )
					{
						DBG4 (( "collect: project error %d \n", rc ))
						return ( rc );
					}
					if ( rc > 0 ) break ;
					/*FALLTHROUGH*/

				case LEVEL_PROGRAM :
					if ( vopt_prefer_shm )
					{
						DBG8 (( "try to interpret target as a shm program \n" ))
						rc = collect_target ( target , version , versdef , dep_type ,
									TT_SHM , & ov_mci , ctip , tipp, local_variant, global_variants );
						if ( rc < 0 )
						{
							DBG4 (( "collect: program error %d \n", rc ))
							return ( rc );
						}
						if ( rc > 0 ) break ;

						DBG8 (( "try to interpret target as a program \n" ))
						rc = collect_target ( target , version , versdef , dep_type ,
									TT_PGM , & ov_mci , ctip , tipp, local_variant, global_variants );
						if ( rc < 0 )
						{
							DBG4 (( "collect: program error %d \n", rc ))
							return ( rc );
						}
						if ( rc > 0 ) break ;
					}
					else
					{
						DBG8 (( "try to interpret target as a program \n" ))
						rc = collect_target ( target , version , versdef , dep_type ,
									TT_PGM , & ov_mci , ctip , tipp, local_variant, global_variants );
						if ( rc < 0 )
						{
							DBG4 (( "collect: program error %d \n", rc ))
							return ( rc );
						}
						if ( rc > 0 ) break ;

						DBG8 (( "try to interpret target as a shm program \n" ))
						rc = collect_target ( target , version , versdef , dep_type ,
									TT_SHM , & ov_mci , ctip , tipp, local_variant, global_variants );
						if ( rc < 0 )
						{
							DBG4 (( "collect: program error %d \n", rc ))
							return ( rc );
						}
						if ( rc > 0 ) break ;
					}
					/*FALLTHROUGH*/

				case LEVEL_RELOC :
					DBG8 (( "try to interpret target as a DLL \n" ))
					rc = collect_target ( target , version , versdef , dep_type ,
								TT_DLL , & ov_mci , ctip , tipp, local_variant, global_variants );
					if ( rc < 0 )
					{
						DBG4 (( "collect: DLL error %d \n", rc ))
						return ( rc );
					}
					if ( rc > 0 ) break ;
					/*FALLTHROUGH*/

					if ( vopt_prefer_shm )
					{
						DBG8 (( "try to interpret target as a shm reloc \n" ))
						rc = collect_target ( target , version , versdef , dep_type ,
									TT_SHR , & ov_mci , ctip , tipp, local_variant, global_variants );
						if ( rc < 0 )
						{
							DBG4 (( "collect: reloc error %d \n", rc ))
							return ( rc );
						}
						if ( rc > 0 ) break ;

						DBG8 (( "try to interpret target as a reloc \n" ))
						rc = collect_target ( target , version , versdef , dep_type ,
									TT_REL , & ov_mci , ctip , tipp, local_variant, global_variants );
						if ( rc < 0 )
						{
							DBG4 (( "collect: reloc error %d \n", rc ))
							return ( rc );
						}
						if ( rc > 0 ) break ;
					}
					else
					{
						DBG8 (( "try to interpret target as a reloc \n" ))
						rc = collect_target ( target , version , versdef , dep_type ,
									TT_REL , & ov_mci , ctip , tipp, local_variant, global_variants );
						if ( rc < 0 )
						{
							DBG4 (( "collect: reloc error %d \n", rc ))
							return ( rc );
						}
						if ( rc > 0 ) break ;

						DBG8 (( "try to interpret target as a shm reloc \n" ))
						rc = collect_target ( target , version , versdef , dep_type ,
									TT_SHR , & ov_mci , ctip , tipp, local_variant, global_variants );
						if ( rc < 0 )
						{
							DBG4 (( "collect: reloc error %d \n", rc ))
							return ( rc );
						}
						if ( rc > 0 ) break ;
					}
					/*FALLTHROUGH*/

				case LEVEL_LIBRARY :
					DBG8 (( "try to interpret target as a library \n" ))
					rc = collect_target ( target , version , versdef , dep_type ,
								TT_LIB , & ov_mci , ctip , tipp, local_variant, global_variants );
					if ( rc < 0 )
					{
						DBG4 (( "collect: library error %d \n", rc ))
						return ( rc );
					}
					if ( rc > 0 ) break ;
					/*FALLTHROUGH*/

				case LEVEL_JAVAPROJECT :
					DBG8 (( "try to interpret target as a java project \n" ))
					rc = collect_target ( target , version , versdef , dep_type ,
								TT_JP , & ov_mci , ctip , tipp, local_variant, global_variants );
					if ( rc < 0 )
					{
						DBG4 (( "collect: java project error %d \n", rc ))
						return ( rc );
					}
					if ( rc > 0 ) break ;
					/*FALLTHROUGH*/

					if ( stop_level == GTT_DESC ) 
					{
						/* nothing found */
						DBG0 (( "unknown target '%s', ", target ))
						DBG0 (( "version '%c'\n", version ))
						eprotwrite ( "unknown target '%s' (%s)\n", target, versname(version) );
						if ( ctip != NULL )
						{
							DBG0 (( "called by target '%s', ", ctip->name ))
							DBG0 (( "version '%c' \n", ctip->version ))
							eprotwrite ( "   called by target '%s' (%s) \n", ctip->name, versname(ctip->version) );
						}
						rc = - 1 ;
						break;
					}

				case LEVEL_MODULE :
					DBG8 (( "try to interpret target as a module \n" ))
					rc = collect_module ( target , version , dep_type, & ov_mci , 
											ctip , tipp, local_variant, global_variants );
					if ( rc < 0 )
					{
						DBG4 (( "collect: module error %d \n", rc ))
						return ( rc );
					}

					/* PTS 1104596 */
					if ( target_found == 2 )
					{
						other_deptype_tip->other_dep_tip = *tipp;
						(*tipp)->other_dep_tip = other_deptype_tip;
					}

					if ( rc > 0 ) break ;
					DBG8 (( "try to interpret target as an extra target \n" ))
					rc = collect_extra ( target , & ov_mci , ctip , tipp );
					if ( rc < 0 )
					{
						DBG4 (( "collect: extra error %d \n", rc ))
						return ( rc );
					}
					if ( rc > 0 ) break ;

					/*FALLTHROUGH*/

					/* nothing found */
					DBG0 (( "unknown target '%s', ", target ))
					DBG0 (( "version '%c' - missing com description?\n", version ))
					eprotwrite ( "unknown target '%s' (%s) - missing com description?\n", target, versname(version) );
					if ( ctip != NULL )
					{
						DBG0 (( "called by target '%s', ", ctip->name ))
						DBG0 (( "version '%c' \n", ctip->version ))
						eprotwrite ( "   called by target '%s' (%s) \n", ctip->name, versname(ctip->version) );
					}
					rc = - 1 ;
				}
				/* &gar PTS 1002142 */
				/* if not module (description with old module format */
				/* try o interprete as description -> rc=-10*/
				if (( level == LEVEL_MODULE ) && ( rc < 1 ))  
				{
					level = 0; 
					rc    = -10;
					DBG0 (( "try to interprete '%s' as description\n", trgline )) 
					eprotwrite ( "try to interprete '%s' as description\n", trgline );
				}
			}
			while ( rc == -10); /* if module and not found PTS 1002142 */
		}
	}
    /* give some states to the parent target */
    if ( * tipp != NULL )
	{
	    if ( (*tipp)->stat.shrglob == 0 ) (*tipp)->stat.shrglob = ov_mci.stat.shrglob ;
	    if ( (*tipp)->stat.debug   == 0 ) (*tipp)->stat.debug   = ov_mci.stat.debug ;
		if ( (*tipp)->stat.nodebug == 0 ) (*tipp)->stat.nodebug = ov_mci.stat.nodebug ;  /* CTS 1102466 */
		if ( (*tipp)->stat.no_dep_remake == 0 ) (*tipp)->stat.no_dep_remake = ov_mci.stat.no_dep_remake ;  /* CTS 1102466 */
	    if ( (*tipp)->stat.profile == 0 ) (*tipp)->stat.profile = ov_mci.stat.profile ;
		if ( (*tipp)->stat.noextension == 0 ) (*tipp)->stat.noextension = ov_mci.stat.noextension ;  /* CTS 1112700 */

		/* &gar CTS 1000237 */
		if ( (*tipp)->stat.distribute == 0 ) (*tipp)->stat.distribute = ov_mci.stat.distribute ;

		if (ov_mci.stat.ascii)
		{
			/*
			DBG0 (( "WARNING: found ascii option for target '%s'\n", (*tipp)->name ))
			DBG0 (( "         in the description '%s'\n", ctip->name ))
			DBG0 (( "         It should be used only in com descriptions !\n" ))
			*/
			if ( (*tipp)->stat.ascii == 0 ) (*tipp)->stat.ascii = ov_mci.stat.ascii ;
		}

		if (ov_mci.stat.text)
		{
			/*
			DBG0 (( "WARNING: found ascii option for target '%s'\n", (*tipp)->name ))
			DBG0 (( "         in the description '%s'\n", ctip->name ))
			DBG0 (( "         It should be used only in com descriptions !\n" ))
			*/
			if ( (*tipp)->stat.text == 0 ) (*tipp)->stat.text = ov_mci.stat.text ;
		}
		
		if (ov_mci.stat.binary)
		{
			/*
			DBG0 (( "WARNING: found binary option for target '%s'\n", (*tipp)->name))
			DBG0 (( "         in the description '%s'\n", ctip->name ))
			DBG0 (( "         It should be used only in com descriptions !\n" ))
			*/
			if ( (*tipp)->stat.binary == 0 ) (*tipp)->stat.binary = ov_mci.stat.binary ;
		}

		
		if (ov_mci.stat.exec)
		{
			if ( (*tipp)->stat.exec == 0 ) (*tipp)->stat.exec = ov_mci.stat.exec ;
			/*
			DBG0 (( "WARNING: found exec option for target '%s'\n", (*tipp)->name ))
			DBG0 (( "         in the description '%s'\n", ctip->name ))
			DBG0 (( "         It should be used only in com descriptions !\n" ))

			eprotwrite ( "WARNING: found exec option for target '%s'\n", (*tipp)->name );
			eprotwrite ( "         in the description '%s'\n", ctip->name );
			eprotwrite ( "         It should be used only in com descriptions !\n" );
			*/
		}

		if ( ov_mci.nobind )
		{
			rc = insert_dependency ( *tipp , & ctip->nobinds );
			if ( rc < 0 )
			{
				DBG8 (( "coldesc: insert_dep error %d \n", rc ))
				return ( rc );
			}
			DBG8 (( "coldesc: nobind '%s' \n", (*tipp)->name ))
		}

		if ( ov_mci.on_success )
		{
			rc = insert_dependency ( *tipp , & ctip->on_success );
			if ( rc < 0 )
			{
				DBG8 (( "coldesc: insert_dep error %d \n", rc ))
				return ( rc );
			}
			DBG8 (( "coldesc: on_success '%s' \n", (*tipp)->name ))
		}

		if (ov_mci.protstyle)
		{
			/*
			DBG0 (( "WARNING: found binary option for target '%s'\n", (*tipp)->name))
			DBG0 (( "         in the description '%s'\n", ctip->name ))
			DBG0 (( "         It should be used only in com descriptions !\n" ))
			*/
			(*tipp)->protstyle = ov_mci.protstyle ;
		}

		

	    DBG7 (( "collect: (target)    '%s' \n", (*tipp)->name ))
	    DBG7 (( "collect: shrglob    now  %d  \n", (*tipp)->stat.shrglob ))
	    DBG7 (( "collect: debug      now  %d  \n", (*tipp)->stat.debug ))
	    DBG7 (( "collect: profile    now  %d  \n", (*tipp)->stat.profile ))
		DBG7 (( "collect: distribute now  %d  \n", (*tipp)->stat.distribute ))
	}

	/* if propagates then react old propagates and copy new progagates to tip */
	if ((*tipp) && ((*tipp)->propagates) ) 
    {
        rc = propagate_env( (*tipp)->propagates );
        if ( rc ) return( rc );
		rc=1;
    }

    /* restore newversion and newdefault ! */
    newversion = savvers; 
    newdefault = savdef;

    FREE_AND_SET_TO_NULL(ov_mci.name);
    FREE_AND_SET_TO_NULL(ov_mci.file);
    FREE_AND_SET_TO_NULL(ov_mci.defaultlayer);
    FREE_AND_SET_TO_NULL(ov_mci.objdemanddir);
    FREE_AND_SET_TO_NULL(ov_mci.srcdemanddir);
    
    DBG4 (( "collect: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/


static  int     collect_target 
(   
    char                    * target   ,
    char                    version     ,
    char                    versdef     ,
    int                     dep_type    ,
    int                     trgtype     ,
    struct mod_com_info     * ov_mcip   ,
    struct trg_info         * ctip      ,
    struct trg_info         ** tipp     ,
	char					* local_variant,
	struct variant_info		* global_variants
)
{
	int                 next_level , errcnt=0 , rc=0 , lgt;
	struct trg_info     * tip , * stip ;
	VFILE               * fp ;
	struct list_info	* lip ;
	struct list_info	* head = 0 ;
	struct list_info	** tailp = & head ;
	char			    * file_type ;
	char                * name = NULL;
	char                * lname = NULL;
	char                list [ FILENAMESIZE ] ;
	char			    line [ LINESIZE ] ;
	char			    found_vers ;
    extern char         concomversion;
	time_t				filedate=0;       /* CTS DESCDATE */
	extern char         *concom_variant_local;
	extern struct variant_info   *concom_variants_global;
	char *tmp_name;
	char * caller_layer=NULL;
	char * plus_name = NULL;


	struct dscfile_info *dfip = NULL;
	struct dscfile_info **dfipp;
/*	int			lgt = 0 ; */
	struct dscline_info *dlip; 
	struct dscline_info **dlipp; 
	char* new_line = NULL;
	int size = sizeof(line);
	char *p,*q;
	int continued_line = 0;

	


/* &gar  dep
	struct dem_info *dem_tmp;
struct dep_info *dep_tmp;
int dem_found=0;
 &gar */

    DBG2 (( "coltrg: called \n" ))
	DBG3 (( "coltrg: target  '%s' \n", target ))
	DBG3 (( "coltrg: version '%c' \n", version ))
	DBG3 (( "coltrg: versdef '%c' \n", versdef ))
	DBG3 (( "coltrg: trgtype %d   \n", trgtype ))

	* tipp = NULL ; /* initialize the result pointer */

	switch ( trgtype )
	{
	case TT_MAC :
			file_type = MAC_LST ;
			next_level = LEVEL_MACRO ;
			break ;
	case TT_PRJ :
			file_type = PRJ_LST ;
			next_level = LEVEL_PROJECT ;
			break ;
	case TT_PGM :
			file_type = LNK_LST ;
			next_level = LEVEL_RELOC ;
			break ;
	case TT_SHM :
			file_type = SHM_LST ;
			next_level = LEVEL_RELOC ;
			break ;
	case TT_DLL :
			file_type = DLL_LST ;
			next_level = LEVEL_RELOC ;
			break ;
	case TT_REL :
			file_type = REL_LST ;
			next_level = LEVEL_RELOC ;
			break ;
	case TT_SHR :
			file_type = SHR_LST ;
			next_level = LEVEL_RELOC ;
			break ;
	case TT_LIB :
			file_type = LIB_LST ;
			next_level = LEVEL_RELOC ;
			break ;
	case TT_JP :
			file_type = JP_LST ;
			next_level = LEVEL_MODULE ;
			break ;
	default :
		DBG0 (( "collect_target entered with illegal type %d \n",trgtype ))
		eprotwrite( "collect_target entered with illegal type %d \n",trgtype );
		return ( - 1 );
	}

	if (ctip && ctip->layer)
			caller_layer = ctip->layer;
	

    /* misuse of line */
    (void) get_basename( target, line );
    if ( strcmp( file_type, get_extension( line ) ) )
    {
        lname = (char *) repalloc( lname, strlen( line ) + strlen( file_type ) + 1 + 1 );
        name  = (char *) repalloc( name,  strlen( target ) + strlen( file_type ) + 1 + 1 );
	    (void) sprintf ( lname , "%s.%s", line   , file_type );
	    (void) sprintf ( name ,  "%s.%s", target , file_type );
    }
    else
    {
        lname = (char *) repalloc( lname, strlen( line ) + 1 );
        name  = (char *) repalloc( name,  strlen( target ) + 1 );
	    (void) sprintf ( lname , "%s", line );
	    (void) sprintf ( name  , "%s", target );
    }

	DBG8 (( "search description file '%s' \n", lname ))

	rc = find_target ( trgtype , version , dep_type , name , ctip , & tip, ov_mcip );
	if ( rc < 0 )
	{
		DBG4 (( "coltrg: find error %d \n", rc ))
		return ( rc );
	}
	if ( rc > 0 )
	{
		/* PTS 1107143 -> */
		if ( ov_mcip->nobind )
		{
			rc = insert_dependency ( tip , & ctip->nobinds );
			if ( rc < 0 )
			{
				DBG8 (( "coldesc: insert_dep error %d \n", rc ))
				return ( rc );
			}
			DBG8 (( "coldesc: nobind '%s' \n", tip->name ))
		}
		if ( ov_mcip->on_success )
		{
			rc = insert_dependency ( tip , & ctip->on_success );
			if ( rc < 0 )
			{
				DBG8 (( "coldesc: insert_dep error %d \n", rc ))
				return ( rc );
			}
			DBG8 (( "coldesc: on_success '%s' \n", tip->name ))
		}
		/* <- PTS 1107143 */
		DBG8 (( "coltrg: target already present '%s' \n", target ))
		* tipp = tip ;
		return ( 1 );
	}

    rc = get_dsc_file( lname , version , list , & found_vers, &filedate, 0 );

	if ( (rc == 0)  && (lname[0] != INDEX_SYMBOL) && caller_layer )
	{ 
		
		tmp_name = (char *) palloc( strlen( lname) + strlen( caller_layer ) + 4 );
		(void) sprintf ( tmp_name , "::%s/%s", caller_layer, lname );
		FREE_AND_SET_TO_NULL(lname);
		lname = tmp_name;
		tmp_name = (char *) palloc( strlen( name) + strlen( caller_layer ) + 4 );
		(void) sprintf ( tmp_name , "::%s/%s", caller_layer, name );
		FREE_AND_SET_TO_NULL(name);
		name = tmp_name;
		rc = find_target ( trgtype , version , dep_type , name , ctip , & tip, ov_mcip );
		if ( rc < 0 )
		{
			DBG4 (( "coltrg: find error %d \n", rc ))
			return ( rc );
		}
		if ( rc > 0 )
		{
			/* PTS 1107143 -> */
			if ( ov_mcip->nobind )
			{
				rc = insert_dependency ( tip , & ctip->nobinds );
				if ( rc < 0 )
				{
					DBG8 (( "coldesc: insert_dep error %d \n", rc ))
					return ( rc );
				}
				DBG8 (( "coldesc: nobind '%s' \n", tip->name ))
			}
			/* <- PTS 1107143 */
			if ( ov_mcip->on_success )
			{
				rc = insert_dependency ( tip , & ctip->on_success );
				if ( rc < 0 )
				{
					DBG8 (( "coldesc: insert_dep error %d \n", rc ))
					return ( rc );
				}
				DBG8 (( "coldesc: on_success '%s' \n", tip->name ))
			}
			DBG8 (( "coltrg: target already present '%s' \n", target ))
			* tipp = tip ;
			return ( 1 );
		}

		rc = get_dsc_file( lname , version , list , & found_vers, &filedate, 0 );
	}

	if ( rc <= 0 )
	{
		/* &gar PTS 1102705 */
		if ( rc == ERROR_NO_CASE_SENSITIVE )
		{
			DBG0 (( "Error : Descriptionfile '%s' is not case sensitive described \n", lname ))
			eprotwrite ( "Error : Descriptionfile '%s' is not case sensitive described \n", lname );
		}
		else	
			if ( rc == 0 )
				DBG4 (( "coltrg: description file not found '%s' \n", name ))
			else    
				DBG8 (( "coltrg: get_dsc_file error %d \n", rc ))
		return ( rc );
	}
	DBG4 (( "coltrg: description file found     '%s' \n", name ))

	rc = insert_target( trgtype , version , dep_type , name , ctip , & tip, ov_mcip );
	
	tip->localvariant = local_variant;
	tip->globalvariants = global_variants;

	concom_variants_global = global_variants; 
	concom_variant_local = local_variant;

	if ( rc < 0 )
	{
		DBG4 (( "coltrg: insert error %d \n", rc ))
		return ( rc );
	}

	/* CTS DSCDATE */
	tip->date = filedate;

	if ( rc == 0 )
	{
		DBG8 (( "coltrg: target already present '%s' \n", target ))
		* tipp = tip ;
		return ( 1 );
	}

    is_qualified_desc( name, &tip->own_layer, NULL );
    if ( tip->own_layer )
    {
        tip->layer     = (char *) repalloc( tip->layer, strlen( tip->own_layer ) + 1 );
	    (void) strcpy( tip->layer , tip->own_layer ); /* PTS 1102210 */
    }

    if ( ! opt_nodependencies )
    {
	    fp = fileopen ( list , "r" );
	    if ( ! fp )
	    {
	        DBG0 (( "cannot open '%s' \n", list ))
			eprotwrite( "cannot open '%s' \n", list );
	        return ( - 2 );
	    }
	    DBG8 (( "coltrg: opened '%s' \n", list ))
    
        concomversion = version; /* set conditional compiling for versions */
	    concom_debug = ov_mcip->stat.debug || opt_debug ;  /* &gar (CTS 1105643) */
		/* should now set at of collect_target */
		/*
        concom_variant_local = tip->localvariant;
		concom_variants_global = tip->globalvariants;
		*/

				

        for ( ; ; )
	    {
	        rc = get_dsc_line ( line , sizeof(line) , fp , list );
	        if ( rc <  0 )
	        {
		        DBG4 (( "coltrg: get_line error %d \n" , rc ))
		        (void) concom ( (char*) 0 , list );
		        (void) fileclose ( fp );
		        return ( rc ) ;
	        }
	        if ( rc == 0 ) break ;

	        lgt = strlen ( line );
	        if ( ! lgt ) continue ;

	        lip = (struct list_info*) palloc ( sizeof(struct list_info) + lgt );
	        if ( ! lip )
	        {
		        DBG0 (( "insufficient dynamic memory \n" ))
				eprotwrite ( "insufficient dynamic memory \n" );
		        (void) concom ( (char*) 0 , list );
		        (void) fileclose ( fp );
		        return ( -3 );
	        }
	        FILL ( lip , 0 , sizeof(struct list_info) + lgt );
	        COPY ( lip->line , line , lgt );

	        * tailp = lip ;
	        tailp = & lip->next ;
	        DBG4 (( "coltrg: read line '%s' \n", lip->line ))
	    }

		errcnt = 0 ;

	    rc = concom ( (char*) 0 , list );
	    (void) fileclose ( fp );

		if (rc < 0 )
			errcnt++;

/* insert extension.plus */
/* append to tailp */
		
		/* rc = get_plus_info ( file_type, &lip ); */
		
		dfipp = &plus_descriptions;
		for (dfip = plus_descriptions; dfip; dfip = dfip->next)
		{
			if (strcmp(dfip->type,file_type) == 0) 
				break; /* found */
			dfipp = &(dfip->next);
		}
		if ( ! dfip )
		{
			dfip = (struct dscfile_info *) palloc( sizeof (struct dscfile_info) ); 
			dfip->type = (char*) palloc (strlen (file_type) + 1);
			strcpy (dfip->type, file_type);
			dfip->next = NULL;
			plus_name = (char *) repalloc( plus_name, 3 + strlen( PLUS_LAYER ) + strlen (PLUS_NAME) + strlen( file_type ) + 2 ); 
		
			(void) sprintf ( plus_name , "::%s/%s.%s" , PLUS_LAYER, PLUS_NAME, file_type );
			rc = get_dsc_file( plus_name, version , list , & found_vers, &filedate, 0 );
			

			if (rc == 0)
			{
				dfip->lines = NULL;
				dfip->name = NULL;
			}
			else
			{
				dfip->name = (char*) palloc (strlen (list) + 1);
				strcpy (dfip->name, list);
				fp = fileopen ( list , "r" );
				if ( ! fp )
				{
					DBG0 (( "cannot open '%s' \n", list ))
					eprotwrite( "cannot open '%s' \n", list );
					return ( - 2 );
				}
				DBG8 (( "coltrg: opened '%s' \n", list ))
			
				dlipp = &(dfip->lines);
				FILL (  line , 0 , size );
				
				while ( filegets( line + lgt , size - lgt , fp ) )
				{
					dlip = (struct dscline_info*) palloc (sizeof(struct dscline_info));
					dlip->next = NULL;
					dlip->text = (char *) palloc (strlen(line+lgt) + 2);
					strcpy (dlip->text, line+lgt);
					if (dlip->text[strlen(dlip->text)-1] == '\n')
						dlip->text[strlen(dlip->text)-1] = '\0'; /* cut \n */
					*dlipp = dlip;
					dlipp = &dlip->next;
				}
				*dfipp = dfip;
				(void) fileclose ( fp );
			}
/* 
in dfip->lines steht jetzt alles drin */
		}

		for (dlip = dfip->lines; dlip; dlip=dlip->next)
		{
			if ( ! concom(dlip->text , dfip->name ) ) continue ;
			p = dlip->text;
			
			while( isspace( * p ) ) p++;

			if ( continued_line )
			{
                new_line = (char*) repalloc (new_line, strlen(new_line) + strlen(p));
				q = new_line + strlen(new_line) - 1;
			}
			else
			{
				/* empty lines with spaces */
				if (strlen(p) == 0)
					continue;
				new_line = (char*) palloc (strlen(p));
/*				new_line = (char*) palloc (strlen(dlip->text)); */
				q = new_line;
			}
			strcpy (q, dlip->text);
			if (dlip->text[strlen(dlip->text)] == '\\')
			{
				continued_line = 1;
			}
			else
			{
				continued_line = 0;
				lip = (struct list_info*) palloc ( sizeof(struct list_info) + strlen(dlip->text) );
				if ( ! lip )
				{
					DBG0 (( "insufficient dynamic memory \n" ))
					eprotwrite ( "insufficient dynamic memory \n" );
					(void) concom ( (char*) 0 , list );
					return ( -3 );
				}
				FILL ( lip , 0 , sizeof(struct list_info) + strlen(dlip->text));
				COPY ( lip->line , dlip->text , strlen(dlip->text) + 1 );
				* tailp = lip ;
				tailp = &(lip->next);
			}
		}
		rc = concom ( (char*) 0 , list );  
		if (rc < 0 )
			errcnt++;

   
	    for ( ; head ; head = lip->next , free((char*)lip) )
	    {
	        lip = head ;
	        DBG4 (( "coltrg: targtline '%s' \n", lip->line ))

	        rc = collect( lip->line , versdef , versdef , DT_INCDEP,
			           next_level , tip , & stip );
	        if ( rc < 0 )
	        {
				errcnt ++ ;
		        continue ;
	        }
	        if ( rc == 0 )
	        {
		        /*if ( ! stip ) continue ;  empty lines or comments */
                continue ;
		        /*
		         *  This 'fallthrough' is for targets which are already present;
		         *  they should be inserted into the dependencies list anyway.
		         *  On the other hand, they should return rc=1. So complain.
		         */
		        /* DBG0 (( "BUGCHECK: collect returned 0 for '%s'\n", lip->line ))  */
	        }

			if (stip->parentlinkoptions)
				if ( insert_parentlinkoptions (tip, stip->parentlinkoptions, TRUE) < 0 )
					return (-1);

				/*
			if ( stip->parentlinkoptions ) 
			{
				if ( tip->type != TT_MAC ) 
				{
					if ( ( tip->type == TT_PGM ) || ( tip->type == TT_DLL ) || ( tip->type == TT_SHM ) )
					{ 
						rc = insert_options (&(tip->linkoptions), stip->parentlinkoptions );	
						if ( rc < 0 )
						{
							DBG4 (( "coltrg: linkoption error %d \n", rc ))
							return ( rc );
						}
					}
		
					if ( ( tip->type != TT_PGM ) && ( tip->type != TT_SHM ) )
					{
						rc = insert_options (&(tip->parentlinkoptions), stip->parentlinkoptions);
						if ( rc < 0 )
						{
							DBG4 (( "coltrg: parentlinkoption error %d \n", rc ))
							return ( rc );
						}
					}
				}
			}
			*/

		
		    if ( ! ( stip->stat.deptype == DT_SRCDEMDEP || trgtype == TT_JP ))
            {
                if ( stip->type != TT_OPTION )
			    {
                    rc = insert_dependency ( stip , & tip->dependencies );
			    }
		        else
                {
			        rc = insert_dependency ( stip , & tip->options );
                }
                if ( rc < 0 )
                {
                    DBG8 (( "coltrg: insert_dep error %d \n", rc ))
                        return ( rc );
                }
            }
            else
            {
                if ( stip->stat.deptype != DT_SRCDEMDEP && trgtype == TT_JP )
				{
    	            DBG0 (( "coltrg: accept only source demands '%s' \n", stip->name ))
					eprotwrite ( "coltrg: accept only source demands '%s' \n", stip->name);
				}
            }

	        /*
	         *  If any one of the modules uses the debug or profile
	         *  option, this applies to a using program too.
	         *  Therefore, these options are delivered upwards
	         *  from level to level.
	         */

	        if ( stip->stat.shrglob == 1 ) tip->stat.shrglob = 1 ;
	        if ( stip->stat.debug        ) tip->stat.debug   = 1 ;
	        if ( stip->stat.profile      ) tip->stat.profile = 1 ;
	        DBG7 (( "coltrg: (target)    '%s' \n", tip->name ))
	        DBG7 (( "coltrg: shrglob now  %d  \n", tip->stat.shrglob ))
	        DBG7 (( "coltrg: debug   now  %d  \n", tip->stat.debug ))
	        DBG7 (( "coltrg: profile now  %d  \n", tip->stat.profile ))
	    }

        if ( errcnt )
	    {
	        DBG8 (( "target collect errors %d \n", errcnt ))
			/* &gar PTS 1102705 */
			DBG0 (( "Error while collecting '%s'\n", target ));  	
			eprotwrite( "Error while collecting '%s'\n", target );  		
	        return ( - 1 );
	    }

	    rc = collect_descriptions( tip , (struct mod_com_info_list *) 0  , ov_mcip );
	    if ( rc < 0 )
	    {
	        DBG4 (( "coltrg: description error %d \n", rc ))
	        return ( rc );
	    }

	    DBG8 (( "coltrg: collected descriptions for '%s' \n", target ))
    }

    /* restore layer PTS 1102210 */
    if ( cmp_layer( tip->own_layer, tip->layer ) )
    {
        if ( ! tip->own_layer )
        {
            FREE_AND_SET_TO_NULL(tip->layer);
            tip->layer = NULL;
        }
        else
        {
            tip->layer     = (char *) repalloc( tip->layer, strlen( tip->own_layer ) + 1 );
	        (void) strcpy( tip->layer , tip->own_layer );
        }
    }

    * tipp = tip ; /* set the result pointer */

    FREE_AND_SET_TO_NULL(name);
    FREE_AND_SET_TO_NULL(lname);

	DBG4 (( "coltrg: returning %d \n", 1 ))
	return ( 1 );
}

/*==========================================================================*/


static  int     collect_list 
(   
    char                    * name   ,
    char                    version     ,
    char                    versdef     ,
    int                     trgtype     ,
    struct mod_com_info     * ov_mcip   ,
    struct trg_info         * ctip      ,
    struct trg_info         ** tipp    
)
{
	int                     rc ;
	struct trg_info         *tip = NULL;
	char                    *p;
	char					*directory = NULL;
	struct dir_info         * dirp = NULL;
	valid_bit_arr	vbits ;
	int                     errcnt = 0;
	int                     recursive = 0;
	char                    *filter = NULL;

    DBG2 (( "collst: called \n" ))
	DBG3 (( "collst: name     '%s' \n", name ))
	

	rc = insert_target ( trgtype , version , DT_NODEP , name , ctip , & tip, ov_mcip );
	if ( rc <= 0 )
	{
	    DBG4 (( "collst: insert error %d \n", rc ))
	    return ( rc );
	}
	if ( rc == 0 )
	{
		DBG8 (( "collst: target already present '%s' \n", name ))
		* tipp = tip ;
		return ( 1 );
	}
	if ( rc > 0 )
	{
		/* PTS 1107143 -> */
		if ( ov_mcip->nobind )
		{
			rc = insert_dependency ( tip , & ctip->nobinds );
			if ( rc < 0 )
			{
				DBG8 (( "coldesc: insert_dep error %d \n", rc ))
				return ( rc );
			}
			DBG8 (( "coldesc: nobind '%s' \n", tip->name ))
		}
		if ( ov_mcip->on_success )
		{
			rc = insert_dependency ( tip , & ctip->on_success );
			if ( rc < 0 )
			{
				DBG8 (( "coldesc: insert_dep error %d \n", rc ))
				return ( rc );
			}
			DBG8 (( "coldesc: on_success '%s' \n", tip->name ))
		}
		/* <- PTS 1107143 */
	}
	rc = collect_descriptions( tip , (struct mod_com_info_list *) 0  , ov_mcip );
	tip->layer = NULL;
	get_layer(tip->name, &tip->layer);
	
	if (! tip->layer)
		tip->layer = ctip->layer;

	if (!tip->layer)
	{
 		/* &gar PTS 1102705 */
		DBG0 (( "Found filelist without layer specification '%s'\n", tip->name ));  	
		eprotwrite( "Found filelist without layer specification '%s'\n", tip->name );  		
	    return ( - 1 );
	}

	p = strchr(tip->layer, '*');
	if (p)
	{
		if (*(p-1) == '/' )
			p--;
		*p = '\0';
        recursive = 1;
	}

	p = strrchr(name, '/');

	if (p)
		p++;
	else
		p = name;
   
	if (p)
	{
		filter = (char*) palloc (strlen(p));
		strcpy (filter, p);
	}
	else
	{
		DBG0 (( "Wrong filter definition '%s'\n", name ));  	
		eprotwrite( "Wrong filter definition '%s'\n", name );  		
		return ( - 1 );
	}

	if (tip->layer)
	{
		rc = get_directory ( tip->layer , FK_SOURCE | FK_LEVEL , vbits , &directory);
		if ( rc < 0 )
		{
			DBG4 (( "get dir error, returning %d \n", rc ))
			return (rc);  /* return ret */
		}
        if ( rc == 0 )
			return (ewrite_all (tip, 0, -1, "ERROR: Can't find files for %s\n", tip->name)) ;

		rc = collect_directory (directory, filter, tip->layer, version, versdef, trgtype, ov_mcip, tip, recursive, 1);
		if (rc)
			errcnt++;
		else
			if ( ! insert_dependency ( tip , & ctip->dependencies ))
				errcnt++;
	}
	else
	{
		DBG0 (( "collst: can't determine layer of %s \n", tip->name ))
		return (-1);
	}

	*tipp = tip;

	return (errcnt);
}

global int collect_directory
(
	char                    * directory , /* direct path */
	char                    * filter    ,
	char                    * layer     ,
	char                    version     ,
    char                    versdef     ,
    int                     trgtype     ,
    struct mod_com_info     * ov_mcip   ,
	struct trg_info         * ctip      ,
	int                     recursive   ,
	int                     silent
)
{
	struct dir_info         * dirp = NULL;
	struct dir_entry        * direp;
	int                     rc = 0;
	int                     errcnt = 0;

    DBG3 (("coldir %s\n", directory))

	/*DBG0 (("%s/... (%s)\n", directory, (filetype == FT_BIN) ? "binary" : "ascii"  )) */
	rc = read_dir_info ( directory , & dirp );
	if (ov_mcip->file &&  
		ov_mcip->file->file && 
		(ov_mcip->file->file[strlen(ov_mcip->file->file)-1] != '/'))
	{
		char *orgfilename = ov_mcip->file->file;
		DBG0 (("WARNING: missing final '/' after destination directory for '%s'\n (found in '%s')\n", ctip->name, ctip->act_caller->name  ))
		ov_mcip->file->file = (char *) palloc (strlen(ov_mcip->file->file)+2);
        strcpy (ov_mcip->file->file, orgfilename);
        strcat (ov_mcip->file->file, "/");
		FREE_AND_SET_TO_NULL(orgfilename);
	}
	
	
	if (rc >= 0)
	{
		direp = dirp->entries;
		if (direp)
		{
			DBG4 (( "Parse entry '%s' under %s...\n", direp->name, directory ));
			/*
			if (ov_mcip->file &&  
				ov_mcip->file->file && 
				(ov_mcip->file->file[strlen(ov_mcip->file->file)-2] != '/'))
			{
					char *orgfilename = ov_mcip->file->file;
                    DBG0 (("WARNING: missing final '/' after target for '%s'\n", directory ))
					ov_mcip->file->file = (char *) palloc (strlen(ov_mcip->file->file)+2);
                    strcpy (ov_mcip->file->file, orgfilename);
                    strcat (ov_mcip->file->file, "/");
					FREE_AND_SET_TO_NULL(orgfilename);
			}
			*/
			errcnt += collect_directory_entries (directory, filter, direp, layer, version, versdef, trgtype, ov_mcip, ctip, recursive, silent );
		}
	}
	else
	{
		errcnt++;
	}
	return (errcnt);
}


global int collect_directory_entries
(
	char                    * directory , 
	char                    * filter    ,
	struct dir_entry        * direp     ,
	char                    * layer     ,
	char                    version     ,
    char                    versdef     ,
    int                     trgtype     ,
	struct mod_com_info     * ov_mcip   ,
	struct trg_info         * ctip      ,
	int                     recursive   , 
	int                     silent
)
{
	char                    *p_src=NULL;
	int                     errcnt = 0;
	int	                    rc;
	char                    *tmplayer = NULL;
	char                    *tmpmodule = NULL;
	struct trg_info         * tip = NULL;
	char                    * p;

	struct stat fileinfo;
	
	if (direp)
	{
		/* copy itself */
		p_src = (char*) palloc (strlen(directory) + strlen(direp->name) + 2);
		strcpy (p_src, directory);
		strcat (p_src, "/");
		strcat (p_src, direp->name);
		
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
				struct file_info    * orgfile;

				if ( recursive )
				{
					/* neuer layer und neues Unterverzeichnis */
					tmplayer = (char*) palloc( strlen(layer) + strlen(direp->name) + 2);
					strcpy (tmplayer, layer);
					strcat (tmplayer, "/");
					strcat (tmplayer, direp->name);
					/* special handling for file pathes - add new subdirectory to destination dir*/
					orgfile = ov_mcip->file;
					
					if (orgfile)
					{
						ov_mcip->file = (struct file_info *) palloc (sizeof (struct file_info));
						FILL (  ov_mcip->file , 0 , sizeof(struct file_info) );
						ov_mcip->file->file = (char *) palloc( strlen( orgfile->file) + strlen(direp->name) + 2 );
						(void) strcpy( ov_mcip->file->file, orgfile->file );
						(void) strcat ( ov_mcip->file->file, direp->name );
						(void) strcat ( ov_mcip->file->file, "/" );
					}

					rc = collect_directory (p_src, filter, tmplayer, version, versdef, trgtype, ov_mcip, ctip, recursive, 1);
					errcnt += rc;
					ov_mcip->file = orgfile;
				}
				else
				{
					DBG4 (( "coll_dir_ent: Found directory but recursive collecting is not allowed\n"))
				}
			}
			else
			{
				int filter_matched = 0;
				if (filter)
				{
					p = strchr (filter, '*');
                    if ( (p == filter) ||  (strncmp(direp->name , filter, p - filter) == 0) )
					{
						/* first matched */
						p++;
						/* compare the end */
                        if (strcmp (direp->name + strlen (direp->name) - strlen (p), p ) == 0)
							filter_matched = 1;
					}
				}
				else
					filter_matched = 1;
				if ( filter_matched )
				{
					tmpmodule = (char*) palloc( strlen(layer) + strlen(direp->name) + 3);
					strcpy (tmpmodule, ":");
					strcat (tmpmodule, layer);
					strcat (tmpmodule, "/");
					strcat (tmpmodule, direp->name);
					DBG6 (( "Collect module '%s' ...\n", tmpmodule ));

					rc = collect_module ( tmpmodule , version , DT_OBJDEP, ov_mcip , 
												ctip , &tip, NULL, NULL );

					if (tip)
					{
						tip->stat.realname = 1;

						/* if (! silent)
							DBG0 (("%s (%s)\n", p_dest, (filetype == FT_BIN) ? "binary" : "ascii"  ))
						*/
						if ( rc ) 
							rc = insert_dependency ( tip , & ctip->dependencies );
						if ( ! rc ) 
							errcnt ++;
					}
				}
			}
		}

		/* copy left */
		DBG8 (( "Found entry '%s' on the left site of '%s' ...\n", (direp->left) ? direp->left->name : "NOTHING", direp->name ));
		errcnt += collect_directory_entries (directory, filter, direp->left, layer, version, versdef, trgtype, ov_mcip, ctip, recursive, silent );
		/*copy right */
		DBG8 (( "           Found entry '%s' on the right site of '%s' ...\n", (direp->right) ? direp->right->name : "NOTHING", direp->name ));
		errcnt += collect_directory_entries (directory, filter, direp->right, layer, version, versdef, trgtype, ov_mcip, ctip, recursive, silent );
	}
	return (errcnt);
}

/*==========================================================================*/


static  int     collect_module 
(   
    char                    * target    ,
    char                    version     ,
    int                     dep_type    ,
    struct mod_com_info     * ov_mcip   ,
    struct trg_info         * ctip      ,
    struct trg_info         ** tipp     ,
	char					* local_variant,
	struct variant_info		* global_variants
)
{
	int                          trgtype , rc;
	struct trg_info              * tip ;
	struct mod_com_info_list     * mcilp = NULL;
	char                         trgvers ; 
    char                         * layer = NULL;
	/* &gar PTS  1103486 */
	struct trg_info * stip=NULL;
	char tmp_version = version;
	int temp_count;
	char * caller_layer=NULL;


    DBG2 (( "colmod: called \n" ))
	DBG3 (( "colmod: target  '%s' \n", target ))
	DBG3 (( "colmod: version '%c' \n", version ))

	* tipp = NULL ;

	/*
      Check whether the target is a module. The target is
      assumed to be a module, if two conditions are met:
      A module's name needs to begin with {v|g|h|i} followed 
      by letters (the layer) and at least 
      two digits. Or a moduls name is a qualificated name 
      ( :<layer>/<source> ). There must be compilation information 
      present for the module's layer. 
	*/

	if (ctip && ctip->layer)
			caller_layer = ctip->layer;

	if ( ! is_valid_sourcename( target, &layer, NULL ) )
	{
        if ( caller_layer && *caller_layer )
        {
		    DBG4 (( "colmod: no valid module name '%s' but present caller layer \n", target ))
        }
        else
        {
		    DBG4 (( "colmod: no valid module name       '%s' \n", target ))
		    return ( 0 );
        }
	}
	DBG4 (( "colmod: valid module name          '%s' \n", target ))

	
    if ( ! layer && caller_layer ) 
    {
        DBG4 (( "colmod: no layer found, use callers default layer \n" ))
        layer = (char *) repalloc( layer, strlen( caller_layer ) + 1 );
        (void) strcpy( layer , caller_layer );
    }

    
	/* look for a caller version for conditional compiling of com-description */
	/* &gar PTS  1103486 -> */
    if ( version == ' ')
	{
	    DBG6 (("--------------\nlook for caller of '%s'\n", target)) ;
		temp_count=0;
		/* check of 'n' for noversion (CTS 1110258) */
		for (stip = ctip; stip && ((stip->version == ' ') || (stip->version == 'n')); stip=stip->act_caller)
		{
			/* PTS 1104691 */
			if (temp_count++ > 30) 
			{
				DBG0 (( "Warning (colmod): Found act_caller recursion \n" ))
				eprotwrite ( "Warning (colmod): Found act_caller recursion \n" );
				break; 
			} 
			
			DBG6 (("found caller '%s' with version '%c'\n", stip->name, stip->version)) 
		}
		if (stip)
			DBG6 (("found caller '%s' with version '%c'\n", stip->name, stip->version)) 
		else
			DBG6 (("use default vmakeversion 'c%'\n", vmakeversion)) ;
		tmp_version = (stip && stip->version != ' ') ? stip->version : vmakeversion;
		/* tmp_version = stip ? stip->version : vmakeversion;  */
	}
	/* <- PTS 1103486 */

	/* find compile informations for this layer */
	if ( ! ov_mcip->nocomfile )
	{
		rc = get_com_info_list ( layer , tmp_version , ov_mcip->comfilename, & mcilp);
		if ( rc <= 0 )
		{
			/* &gar PTS 1102705 */
			if ( rc == ERROR_NO_CASE_SENSITIVE )
			{
				DBG0 (( "Error: Com-Description for '%s' not case sensitive\n",target ))
				eprotwrite ( "Error: Com-Description for '%s' not case sensitive\n",target );
			}	
			else 	
				if ( rc == 0 )
					DBG8 (( "colmod: com info not found '%s' \n", layer ))
				else    	
					DBG8 (( "colmod: get_com_info error for %s (%d) \n", layer, rc ))
			return ( rc );
		}
		DBG8 (( "colmod: com info found '%s' \n", layer ))
	}

	/*
	 *  determine the type of the module:
	 *      All modules with the digits '00' or first char 'g' 
	 *      are include modules.
	 *      Modules, which are contained in libraries only are treated
	 *              as library members. Their '.o' files will be deleted
	 *              after beeing put into the libraries.
	 *      All other modules are normal modules, which '.o' files go
	 *              into the object ditrectories.
	 */
	if ( (version == ' ') || is_include( target ) )
	{

	    trgtype = TT_INC ;
	    trgvers = ' ' ; /* includes don't have a version */
	    DBG8 (( "colmod: include module '%s' \n", target ))
	}
	else
	{
	    /*
	     *  Decide whether the compiled object is to put
	     *  as '.o' file in the objects directory (TT_MOD),
	     *  or whether it is to be put into a library (TT_MEM).
	     */

#ifndef	RELOC_LIB
	    if ( (ctip == NULL) || vopt_mod_only || vopt_reloc_lib )
	    {
#endif	/*RELOC_LIB*/
    		trgtype = TT_MOD ;
#ifndef	RELOC_LIB
	    }
	    else
	    {
		    if ( ctip->type == TT_LIB )
			    trgtype = TT_MEM ;
		    else    trgtype = TT_MOD ;
	    }
#endif	/*RELOC_LIB*/
	    trgvers = version ;
	    if ( trgtype == TT_MOD )
		    DBG8 (( "colmod: independent module '%s' \n", target ))
	    else    DBG8 (( "colmod: library member '%s' \n", target ))
	}

	/* now try to insert the module into the module list */

    rc = insert_target ( trgtype , trgvers , dep_type , target , ctip , & tip, ov_mcip );
	if ( rc < 0 )
	{
	    DBG4 (( "colmod: insert error %d \n", rc ))
	    return ( rc );
	}
	if ( rc == 0 )
	{
	    DBG8 (( "colmod: target already present '%s' \n", target ))
	    * tipp = tip ;
	    return ( 1 );
	}
	DBG8 (( "colmod: inserted '%s' \n", target ))

	/*
	 *  At this point we successfully inserted a new module.
	 *  Now fill in all the target information.
	 */

	tip->localvariant = local_variant;
	tip->globalvariants = global_variants;
    tip->language = get_language ( tip->name );

	DBG7 (( "colmod: module language '%c' \n", tip->language ))

    tip->layer     = (char *) repalloc( tip->layer,     strlen( layer ) + 1 );
    tip->own_layer = (char *) repalloc( tip->own_layer, strlen( layer ) + 1 );
	(void) strcpy ( tip->layer , layer );
	(void) strcpy ( tip->own_layer , layer ); /* PTS 1102210 */
	
    if ( ! opt_nodependencies )
    {
		char old_language = tip->language;
		
	    rc = collect_descriptions ( tip , mcilp , ov_mcip );
		/* interprete again with an other language */
		if ( (tip->language != old_language) && ( rc >= 0 ) && ( old_language != LANG_RC [0] ) )
		{
			rc = clear_descriptions ( tip );
			if ( rc >= 0 )
				rc = collect_descriptions ( tip , mcilp , ov_mcip );
		}
		if ( rc < 0 )
	    {
		    DBG4 (( "colmod: description error %d \n", rc ))
		    return ( rc );
	    }
	    DBG8 (( "colmod: collected descriptions for '%s' \n", target ))
    }

	* tipp = tip ;

    FREE_AND_SET_TO_NULL(layer);

	DBG4 (( "colmod: returning %d \n", 1 ))
	return ( 1 );
}


/*==========================================================================*/


static  int     collect_file 
(   
    char            * basename  ,
    char            * file      ,
    char            version     ,
    struct trg_info * ctip      ,
    struct trg_info ** tipp     
)
{
	int                     rc ;

    DBG2 (( "colfil: called \n" ))
	DBG3 (( "colfil: basename '%s' \n", basename ))
	DBG3 (( "colfil: file     '%s' \n", file ))
	DBG3 (( "colfil: version  '%c' \n", version ))

	rc = insert_target ( TT_FILE , version , DT_NODEP , basename , ctip , tipp, NULL );
	if ( rc <= 0 )
	{
	    DBG4 (( "colfil: insert error %d \n", rc ))
	    return ( rc );
	}

    (*tipp)->file.file = (char *) palloc( strlen( file ) + 1 );
	(void) strcpy ( (*tipp)->file.file , file );

	DBG4 (( "colfil: returning %d \n", 1 ))
	return ( 1 );
}


/*==========================================================================*/


static  int     collect_option 
(   
    char                    * name      ,
    char                    * option    ,
    struct trg_info         * ctip      ,
    struct trg_info         ** tipp     
)
{
	int                     rc ;

    DBG2 (( "colopt: called \n" ))
	DBG3 (( "colopt: name     '%s' \n", name ))
	DBG3 (( "colopt: option   '%s' \n", option ))

	rc = insert_target ( TT_OPTION , ' ' , DT_NODEP , name , ctip , tipp, NULL );
	if ( rc <= 0 )
	{
	    DBG4 (( "colopt: insert error %d \n", rc ))
	    return ( rc );
	}

    (*tipp)->file.file = (char *) palloc( strlen( option ) + 1 );
	(void) strcpy ( (*tipp)->file.file , option );

	DBG4 (( "colopt: returning %d \n", 1 ))
	return ( 1 );
}


/*==========================================================================*/


static  int     collect_command
(   
    char            * command   ,
    char			version     ,
    struct trg_info * ctip      ,
    struct trg_info ** tipp     
)
{
	int                     rc=0 ;
	int                     unconditional, forcemake;
	int                     ignore_error;  /* CTS 1112286 */
	struct trg_info         * tip ;
	char                    * p ;


    DBG2 (( "colcmd: called \n" ))
	DBG3 (( "colcmd: command '%s' \n", command ))
	DBG3 (( "colcmd: version '%c' \n", version ))

	* tipp = NULL ;

	if ( ctip == NULL || ctip->type != TT_MAC )
	{
		DBG0 (( "commands are allowed in macros only \n" ))
		eprotwrite ("commands are allowed in macros only \n" );
	    if ( ctip == NULL )
		    DBG0 (( "command found on command line \n" ))
	    else    
            DBG0 (( "command found in '%s' \n", ctip->name ))
			eprotwrite ( "command found in '%s' \n", ctip->name );
	    return ( - 2 );
	}
	
	/*
	 *  Store the command into cmdlin.
	 *  Cmdlin is separated into two parts.
	 *  The first is of NAMESIZE-1 to be put into the name field
	 *  of the target info struct.
	 *  The second is of FILENAMESIZE-1 to be put into the file field
	 *  of the target info struct.
	 */

	for ( p = command ; isspace(*p) ; p ++ );
	if ( *p != CMD_PREFIX )
	{
	    DBG0 (( "incorrect command prefix on command '%s' \n", p ))
		eprotwrite ( "incorrect command prefix on command '%s' \n", p );
	    return ( - 1 );
	}
	p ++ ;

	/*
	 *  !!command  means: execute command always.
	 */
	unconditional = FALSE ;
	forcemake     = FALSE ; /* &gar CTS 1102522 */
	ignore_error  = FALSE ;
	if ( *p == '?' )
	{
		unconditional = TRUE ; 
	    p ++ ;
	}
	if ( *p == CMD_PREFIX )
	{
		forcemake = TRUE;
        /* unconditional = TRUE ; */
	    p ++ ;
	}

	for ( ; isspace(*p) ; p ++ );

	/* CTS 1112286 */
	if ( *p == '+' )
	{
		ignore_error = TRUE ; 
	    p ++ ;
	}
	for ( ; isspace(*p) ; p ++ );
	/* <- CTS 1112286 */

	if ( strlen( p ) == 0 )
	{
	    DBG7 (( "colcmd: empty command \n" ))
	    return ( 0 );
	}

	rc = insert_target ( TT_CMD , version , DT_NODEP , p , ctip , & tip, NULL );
	if ( rc < 0 )
	{
	    DBG4 (( "colcmd: insert error %d \n", rc ))
	    return ( rc );
	}

	tip->stat.uncond = unconditional ;
	tip->stat.forcemake = forcemake ; /* &gar PTS 1102522 */
	tip->stat.ignore_err = ignore_error;  /* CTS 1112286 */

	* tipp = tip ;

	DBG4 (( "colcmd: returning %d \n", 1 ))
	return ( 1 );
}

/*==========================================================================*/


static  int     collect_execution
(   
    char            * command   ,
	char            * options   ,
    struct trg_info * ctip      ,
    struct trg_info ** tipp     
)
{
	int                     rc=0 ;
	struct trg_info         * tip ;
	char                    * subst_command = NULL;
	char                    * p, *p_last;
    

    DBG2 (( "colexe: called \n" ))
	DBG3 (( "colexe: command '%s' \n", command ))
	
	* tipp = NULL ;

	if ( ctip == NULL || ctip->type != TT_MAC )
	{
		DBG0 (( "commands are allowed in macros only \n" ))
		eprotwrite ("commands are allowed in macros only \n" );
	    if ( ctip == NULL )
		    DBG0 (( "command found on command line \n" ))
	    else    
            DBG0 (( "command found in '%s' \n", ctip->name ))
			eprotwrite ( "command found in '%s' \n", ctip->name );
	    return ( - 2 );
	}
	
	rc = substitute( command, &subst_command );
	if ( rc != 0 )
	{
		DBG0 (( "ERROR: environment substitution error for %s (%d) \n", command, rc ))
		return ( rc );
	}
	if ( strchr(subst_command, INTERNAL_VARPREFIX) )
	{
		rc = substitute_internal_variables(&subst_command, ctip);
		if ( rc <= 0 )
		{
			DBG0 (( "ERROR: variant substitution error for %s (%d) \n", subst_command, rc ))
			return ( rc );
		}
	}

#ifdef WIN32
/* covert / to \ if  don't find option beginning with ' /'*/ 
	p_last = subst_command + strlen( subst_command );
	for ( p = subst_command; p <= p_last; p++ )
	{
		if ( *p == '/' )
		{
			if ((p > subst_command) && (*(p-1) != ' '))
				*p = '\\';
		}
	}
#endif

	tip = (struct trg_info *) palloc ( sizeof(struct trg_info) );
	if ( tip == NULL )
	{
		DBG0 (( "insufficient dynamic memory \n" ))
		eprotwrite ( "insufficient dynamic memory \n" );
		return ( -3 );
	}

	FILL (  tip , 0 , sizeof(struct trg_info) );

	tip->type = TT_EXECUTE;
    tip->name = (char *) palloc( strlen( command ) + 1 );
    (void) strcpy ( tip->name , command );
	tip->afile = subst_command;

    if ( ctip )
	{
	    rc = insert_dependency ( ctip , & tip->callers );
	    if ( rc < 0 ) return ( rc );
	}
    
	if (options)
	{
		char    * token;
		char    token_sep[] = ", ";

		p = options;
		p++;
		if (options[strlen(options)-1] != ')')
		{
			DBG0 (( "wrong option format for execute '%s' \n", options ))
			eprotwrite ( "wrong option format for execute '%s' \n", options );
			return (-1);
		}
		else
		{
			options[strlen(options)-1] = '\0';
		}
		token = strtok( options+1 , token_sep );
        while( token != NULL )
        {
            if (strcmp(token, "always")==0)
			{
				tip->stat.uncond = 1;
				tip->stat.forcemake = 1;
			}
			else
			if (strcmp(token, "on_success")==0)
			{
				tip->stat.on_success = 1;
			}
			else
			if (strcmp(token, "ignore_error")==0)
			{
				tip->stat.ignore_err = 1;
			}
			else
			{
				DBG0 (( "wrong option for execute '%s' \n", token ))
				eprotwrite ( "wrong option for execute '%s' \n", token );
				return (-1);
			}

			token = strtok( NULL , token_sep );
		}
	}

	* tipp = tip ;

	DBG4 (( "colcmd: returning %d \n", 1 ))
	return ( 1 );
}


/*==========================================================================*/


static  int     collect_descriptions
(   
    struct trg_info         * tip       ,
    struct mod_com_info_list     * mcilp      ,  
    struct mod_com_info     * ov_mcip   
)
{
	int                     rc ;
	struct dsc_info         * sip, si;
    struct extdep_info      * edip;
	struct inc_info         * iip, * dip ;
	struct dem_mod_info     * dmip ;
	struct obj_info			* oip;
	struct mod_com_info     * mcip;
	struct file_info        * fip;
	struct file_info        ** sfipp;
	struct mod_com_info_list * matched_mci_list, *next_matched_mci, **mcilpp;
	char			        * target , * mci_name ;
	char                    var_default     [ NAMESIZE ] ;
	char			        lang_default    [ NAMESIZE ] ;
	char			        langvar_default [ NAMESIZE ] ;
    char			        name_default        [ NAMESIZE ] ;
	char			        namevar_default     [ NAMESIZE ] ;
	char			        namelang_default    [ NAMESIZE ] ;
	char			        namelangvar_default [ NAMESIZE ] ;
	char                    extension_default [ NAMESIZE ] ;
	
	/* CTS VARIANT */
	char			        target_default [ NAMESIZE ] ;
	char			        target_without_variants  [ NAMESIZE ] ;
	char                    target_without_global_variants  [ NAMESIZE ] ;

	char			        * p;
    int                     regexmatch;
	struct variant_info     * vip_tip;
	struct variant_info     * vip_mci;
	int						found_exact_target;
	int						found_target_without_variants;
	int						found_target_without_global_variants;
	int                     suppress_uncond = 0;
	int                     need_local_variant = 0;
	int                     variants_not_found = 0;


    DBG2 (( "coldsc: called \n" ))
	DBG3 (( "coldsc: target '%s' \n", tip->name ))

	target = tip->name ;
	rc=0;

	/* targets with extensions get the description 'unpack=no' */
	if ( ( tip->type == TT_MOD || tip->type == TT_INC ) &&
			*(p = get_extension(tip->name)) != '\0' )
	{	
		si.next = NULL;

		si.descriptor = "unpack";
		si.value      = "=no";

		rc = insert_description ( &si , & tip->descriptions, NULL, MCI_LAYER );
		if ( rc < 0 )
		{
			DBG4 (( "coldsc: insert error %d \n", rc ))
			return ( rc );
		}
		DBG8 (( "coldsc: inserted '%s' because module with extension\n", 
			si.descriptor ))
	}

	/* recognize variants */
   /* (void) get_variants( tip->name , localvariant, globalvariant ); */

	/* CTS VARIANT */
	(void) get_basename( tip->name , target_default );
	(void) get_basename( tip->name , target_without_variants );
	(void) strcpy(target_without_global_variants, tip->name);
	(void) cut_global_variants (target_without_global_variants);
	
	
	
	/*
	 *  A variant default line begins with '*-<variant>'.
     *
     *  check for max. lenght of a default name (1 char for first module char +
     *  1 char for languge + length of DSC_DEFAULT + length of variant )
     *
	 */

	if ( (unsigned) strlen ( DSC_DEFAULT ) + 
		( tip->localvariant ? (unsigned) strlen ( tip->localvariant ) : 0 ) > NAMESIZE - 3 )
	{
	    DBG0 (( "WARNING: variant with name and language too long \
                for default line '%s' \n", tip->name ));
		eprotwrite ( "WARNING: variant with name and language too long for default line '%s' \n", tip->name );
	    var_default         [ 0 ] = '\0' ;
	    lang_default        [ 0 ] = '\0' ;
        name_default        [ 0 ] = '\0' ;
        namevar_default     [ 0 ] = '\0' ;
		namelang_default    [ 0 ] = '\0' ;
        namelangvar_default [ 0 ] = '\0' ;
		target_default      [ 0 ] = '\0' ;
		target_without_variants [ 0 ] = '\0';
		extension_default [ 0 ] = '\0';
    }
	else
	{
        /* var_default '*' */
	    (void) strcpy ( var_default , DSC_DEFAULT );
		/* var_default '*-<variant>'*/
        if ( tip->localvariant )
        {
			(void) strcat ( var_default , "-" );
	        (void) strcat ( var_default , tip->localvariant );
		}

        /* lang_default '*<langchar>' */
    	(void) strcpy  ( lang_default, DSC_DEFAULT );
        (void) strncat ( lang_default, &tip->language, sizeof(char) );

        /* langvar_default '*<langchar>-<variant>' */
        (void) strcpy  ( langvar_default, DSC_DEFAULT );
        (void) strncat ( langvar_default, &tip->language, sizeof(char) );
		if ( tip->localvariant )
        {
			(void) strcat ( langvar_default , "-" );
	        (void) strcat ( langvar_default , tip->localvariant );
		}
		
	    FILL ( name_default        , 0 , NAMESIZE );
	    FILL ( namevar_default     , 0 , NAMESIZE );
	    FILL ( namelang_default    , 0 , NAMESIZE ) ;
	    FILL ( namelangvar_default , 0 , NAMESIZE ) ;

        * name_default = * tip->name ;
        /* name_default '<char>*' */
        (void) strcat  ( name_default, DSC_DEFAULT );

        * namevar_default = * tip->name ;
        /* namevar_default '<char>*-<variant>' */
    	(void) strcat  ( namevar_default, DSC_DEFAULT );
        if ( tip->localvariant )
        {
	        (void) strcat  ( namevar_default , "-" );
    	    (void) strcat  ( namevar_default, tip->localvariant );
        }

        * namelang_default = * tip->name ;
        /* namelang_default '<char>*<langchar>' */
        (void) strcat  ( namelang_default, DSC_DEFAULT );
        (void) strncat ( namelang_default, &tip->language, sizeof(char) );

        * namelangvar_default = * tip->name ;
        /* namelangvar_default '<char>*<langchar>-<variant>' */
    	(void) strcat  ( namelangvar_default, DSC_DEFAULT );
        (void) strncat ( namelangvar_default, &tip->language, sizeof(char) );
        if ( tip->localvariant )
        {
    	    (void) strcat  ( namelangvar_default, "-" );
    	    (void) strcat  ( namelangvar_default, tip->localvariant );
        }

		/* CTS VARIANT */
        /* target_default '*.<extension>' or '*' */
        if (*(p = get_extension(tip->name)) != '\0')
    	{
	    	skip_extension (target_default);
    		strcat (target_default, "*.");
	    	strcat (target_default, p );
			strcpy (extension_default, "*.");
			strcat (extension_default, p );

    	}
    	else
	    {
		    strcat (target_default, "*");
			extension_default [ 0 ] = '\0';
	    }
		/* <- CTS VARIANT */		
    }


	/* the following description information stems from the compile list */
	/* find compile information, which refer to this module: */
	/* (a) default information, or (b) module specific information */
	iip = NULL ;
	for ( ; mcilp ; mcilp = mcilp->next )
	{
		found_exact_target=0;
		found_target_without_variants=0;
		found_target_without_global_variants=0;
	
		matched_mci_list = (struct mod_com_info_list *) NULL;

		mcilpp= &matched_mci_list;
		
		for ( mcip=mcilp->mcip; mcip ; mcip = mcip->next )
		{
			DBG4 (( "coldsc: dscr mod in list   '%s' \n", mcip->name ))

			regexmatch = -1;

			if ( mcip->special_target_flag == 'R' )
			{
				int     err;

				DBG4(( "coldsc: compile regular expression '%s' \n", mcip->name ))

				if ( ! mcip->comp_pattern )
				{
					mcip->comp_pattern = 
						(struct re_pattern_buffer *) palloc( sizeof(regex_t) );
					if ( mcip->comp_pattern == NULL )
					{
						DBG0 (( "insufficient dynamic memory \n" ))
						eprotwrite ( "insufficient dynamic memory \n" );
						return ( -3 );
					}

					if ( err = regcomp( mcip->comp_pattern, mcip->name, REG_EXTENDED) )
					{
						char error [ 256 ];
						regerror( err, mcip->comp_pattern, error, 255 );
						DBG0(( "Error in regcomp: %s\n", error ));
						eprotwrite ( "Error in regcomp: %s\n", error );
						continue;
					}
				}
				regexmatch = regexec( mcip->comp_pattern, target, 10, regexmatches, 0 );
				mci_name = EMPTY;
				if (regexmatch)
					continue;
			}
			else
			{
				mci_name = ( mcip->name[0] == INDEX_SYMBOL ) ? 
					skip_index( mcip->name ) : mcip->name;

				rc = 1;
				/* old check */
				
				rc  = strcmp ( mci_name , DSC_DEFAULT         ) != 0;
				rc &= strcmp ( mci_name , var_default         ) != 0;
				rc &= strcmp ( mci_name , lang_default        ) != 0;
				rc &= strcmp ( mci_name , name_default        ) != 0;
				rc &= strcmp ( mci_name , namevar_default     ) != 0;
				rc &= strcmp ( mci_name , namelang_default    ) != 0;
				rc &= strcmp ( mci_name , langvar_default     ) != 0;
				rc &= strcmp ( mci_name , namelangvar_default ) != 0;
				rc &= strcmp ( mci_name , var_default         ) != 0;
				
				/* CTS VARIANT */
 				rc &= stricmp ( mci_name , target_default      ) != 0;
				if ((vopt_new_variant_handling) && ( stricmp ( mci_name , target_without_variants ) == 0 ))
				{
					rc &= 0;
					found_target_without_variants = 1;

				} 
				else
					if ( stricmp ( mci_name , target_without_global_variants ) == 0 )
					{
						rc &= 0;
						found_target_without_global_variants = 1;

					};

				if ( stricmp ( mci_name , target ) == 0 )
				{
					rc &= 0;
					found_exact_target = 1;

				};
				rc &= stricmp ( mci_name , extension_default      ) != 0;

				/* new parsing for all variant combinations */
				/* if nothing found with old method */
				if ( rc )
				{
					need_local_variant = 0;
					variants_not_found = 0;
					
					if ( ( strcmp ( mcip->purename, target_without_variants ) == 0 ) || 
						 (strcmp ( mcip->purename, target_default ) == 0) ||
							( strcmp ( mcip->purename, DSC_DEFAULT ) == 0 )             ||
							( strcmp ( mcip->purename, extension_default ) == 0 )       ||
							( strcmp ( mcip->purename, lang_default ) == 0 ) )
					{
						rc = 0;
						if ( mcip->localvariant )
						{
							if(   ! ( tip->localvariant && 
								( strcmp (mcip->localvariant, tip->localvariant) == 0 )) )
								rc = 1;
						}
						else
							if ( ! vopt_new_variant_handling && tip->localvariant && 
								( strcmp ( mcip->purename, target_default ) ) )
								need_local_variant = 1;
							/* if need_local_variant not set back to 0 -> no match */	
						
 						if (! rc)
							for ( vip_mci = mcip->allvariants; vip_mci; vip_mci=vip_mci->next)
							{
								/* found local variant */
								if (tip->localvariant)
									if ( strcmp ( vip_mci->name, tip->localvariant) == 0 )
									{
										need_local_variant = 0;
										continue;
									}
								for (vip_tip = tip->globalvariants; vip_tip; vip_tip=vip_tip->next)
									if ( strcmp (vip_tip->name, vip_mci->name) == 0 )
										break;
								/* mci variant not found */
								if (! vip_tip)
								{
									rc = 1;
									variants_not_found = 1;
									break;
								}
							}

						if (need_local_variant || variants_not_found)
							rc = 1;
						else
						{
							/* check for no variants */
							for ( vip_mci = mcip->novariants; vip_mci; vip_mci=vip_mci->next)
							{
							/* found local variant */
								if (tip->localvariant)
									if ( strcmp ( vip_mci->name, tip->localvariant) == 0 )
									{ 
										rc = 1; 
										continue;
									}
								for (vip_tip = tip->globalvariants; vip_tip; vip_tip=vip_tip->next)
									if ( strcmp (vip_tip->name, vip_mci->name) == 0 )
										break;
								/* mci variant not found */
								if (vip_tip)
								{
									rc = 1;
									break;
								}
							}
						}							
					}
						
					/* test for new variant definitions */ 
					if (rc)
						continue ;
				}
				
			}
			*mcilpp = (struct mod_com_info_list *) palloc (sizeof (struct mod_com_info_list));
			(*mcilpp)->next = 0;
			(*mcilpp)->mcip = mcip;
			mcilpp = &(*mcilpp)->next;
		}



		/* check for combination of old variants in new variant handling */
		if ( vopt_new_variant_handling && 
			( found_exact_target || found_target_without_global_variants) &&
			  found_target_without_variants  && strchr (target, '-' ))
		{
			DBG0 (( "WARNING: found old variant definition and normal name of target \n" ))
			DBG0 (( "         ('%s' and '%s' in '%s.com')\n", target_without_variants, target, tip->layer ))
			
			for (mcilpp = &matched_mci_list; *mcilpp; )
			{
				if( stricmp ( (*mcilpp)->mcip->name, target_without_variants) == 0)
				{
					next_matched_mci = (*mcilpp)->next;
					FREE_AND_SET_TO_NULL (*mcilpp);
					*mcilpp = next_matched_mci;
				}
				else
				{
					mcilpp = &(*mcilpp)->next;
				}
			}
		}

		while (matched_mci_list)
		{
			mcip = matched_mci_list->mcip;
			/* at this point we have some descriptions */
			DBG8 (( "coldsc: description module '%s' \n", mcip->name ))

			/* includes */
			if ( mcip->includes )
			{
				iip = mcip->includes ;
				rc = collect_include_list ( target , tip , iip, mcilp->mci_type );
				if ( rc < 0 )
				{
					DBG8 (( "coldsc: collect_inc error %d \n", rc ))
					return ( rc );
				}
				DBG8 (( "coldsc: collected include list for    '%s' \n",
										tip->name ))
			}

			/* objdemands */
			if ( mcip->objdemands )
			{
				dmip = mcip->objdemands ;
				rc = collect_demand_list ( target , tip , dmip, &tip->objdemands, DT_OBJDEMDEP, mcilp->mci_type );
				if ( rc < 0 )
				{
					DBG8 (( "coldsc: collect_demand error %d \n", rc ))
					return ( rc );
				}
				DBG8 (( "coldsc: collected objdemand list for    '%s' \n",
										tip->name ))
			}
			
			/* srcdemands */
			if ( mcip->srcdemands )
			{
				dmip = mcip->srcdemands ;
				rc = collect_demand_list ( target , tip , dmip, &tip->srcdemands, DT_SRCDEMDEP, mcilp->mci_type );
				if ( rc < 0 )
				{
					DBG8 (( "coldsc: collect_demand error %d \n", rc ))
					return ( rc );
				}
				DBG8 (( "coldsc: collected srcdemand list for    '%s' \n",
										tip->name ))
			}

			/* &gar ??? */
			if ( ( mcip->stat.deptype == DT_OBJDEMDEP ) && tip->callers )
			{
				/* set tip as demand for the calling target */
				tip->stat.deptype = mcip->stat.deptype;
				rc = insert_demands ( tip , mcip->objdemanddir, & tip->callers->target->objdemands );
				if ( rc < 0 )
				{
					DBG8 (( "coldemand: insert_dep error %d \n", rc ))
					return ( rc );
				}
				DBG8 (( "coldsc: objdemand '%s' \n", mcip->name ))

			}

			if ( ( mcip->stat.deptype == DT_SRCDEMDEP ) && tip->callers )
			{
				/* set tip as demand for the calling target */
				tip->stat.deptype = mcip->stat.deptype;
				rc = insert_demands ( tip , mcip->srcdemanddir, & tip->callers->target->srcdemands );
				if ( rc < 0 )
				{
					DBG8 (( "coldemand: insert_dep error %d \n", rc ))
					return ( rc );
				}
				DBG8 (( "coldsc: srcdemand '%s' \n", mcip->name ))

			}

			/* dependencies */
			if ( mcip->dependencies )
			{
				iip = mcip->dependencies ;
				rc = collect_dep_list ( target , tip , iip, mcilp->mci_type );
				if ( rc < 0 )
				{
					DBG8 (( "coldsc: collect_dep error %d \n", rc ))
					return ( rc );
				}
				DBG8 (( "coldsc: collected dependency list for    '%s' \n",
										tip->name ))
			}

			/* objects */
			if ( mcip->objects )
			{
				oip = mcip->objects ;
				rc = collect_obj_list ( target , tip , oip, tip->localvariant, tip->globalvariants );
				if ( rc < 0 )
				{
					DBG8 (( "coldsc: collect_obj error %d \n", rc ))
					return ( rc );
				}
				DBG8 (( "coldsc: collected object list for    '%s' \n",
										tip->name ))
			}

			/* extrenal dependencies */

			if ( mcip->ext_dependencies )
			{
				edip = mcip->ext_dependencies ;
				rc = collect_extdep_list ( target , tip , edip );
				if ( rc < 0 )
				{
					DBG8 (( "coldsc: collect_extdep error %d \n", rc ))
					return ( rc );
				}
				DBG8 (( "coldsc: collected external dependency list for    '%s' \n",
										tip->name ))
			}
			
			if ( mcip->parentlinkoptions )
			{
				rc = insert_options ( &(tip->parentlinkoptions), mcip->parentlinkoptions );
				if ( rc < 0 )
				{
					DBG8 (( "coldesc: insert_options error %d \n", rc ))
					return ( rc );
				}
				DBG8 (( "coldesc: parentlinkoption '%s' \n", tip->name ))
			}

			/* allow nobind in com descriptions also -> */
			if ( mcip->nobind && tip->act_caller )
			{
				rc = insert_dependency ( tip , & tip->act_caller->nobinds );
				if ( rc < 0 )
				{
					DBG8 (( "coldesc: insert_dep error %d \n", rc ))
					return ( rc );
				}
				DBG8 (( "coldesc: nobind '%s' \n", tip->name ))
			}
			/* <- nobind in com descriptions */

			for ( sip = mcip->descriptions ; sip != NULL ; sip = sip->next )
			{
				rc = insert_description ( sip , & tip->descriptions, tip, mcilp->mci_type );
				if ( rc < 0 )
				{
					DBG4 (( "coldsc: insert error %d \n", rc ))
					return ( rc );
				}
				DBG8 (( "coldsc: inserted '%s' \n", sip->descriptor ))
			}

			if ( mcip->version != ' ' ) tip->version = mcip->version ;
			if ( mcip->stat.shrglob ) tip->stat.shrglob = ((mcip->stat.shrglob == 1) &&
					(tip->stat.shrglob != 2 )) ? 1 : 2 ;
			if ( mcip->stat.debug && ! ( tip->stat.nodebug && (mcilp->mci_type == MCI_GLOBALS ))) 
				tip->stat.debug   = mcip->stat.debug ; 

			if ( mcip->stat.nodebug  && ! ( tip->stat.debug && (mcilp->mci_type == MCI_GLOBALS ))) 
				tip->stat.nodebug   = mcip->stat.nodebug ;  /*CTS 1102466 */
			if ( mcip->stat.profile ) tip->stat.profile = mcip->stat.profile ;
			if ( mcip->stat.noextension ) tip->stat.noextension = mcip->stat.noextension ; /* CTS 1112700 */
			if ( mcip->stat.uncond ) tip->stat.uncond = mcip->stat.uncond ; 
			if ( mcip->stat.no_dep_remake ) tip->stat.no_dep_remake = mcip->stat.no_dep_remake ; /* CTS 1112700 */
			if ( mcip->stat.exec ) tip->stat.exec = mcip->stat.exec ; 
			
			if ( mcip->stat.binary && ! ( tip->stat.ascii && (mcilp->mci_type == MCI_GLOBALS )))
			{
				tip->stat.binary = mcip->stat.binary ;
				tip->stat.ascii = 0 ;  /* reset the komplement */
				tip->stat.text = 0 ;
				tip->language = ' ';
			}
			if ( mcip->stat.checkdate && ! ( tip->stat.checkdate && (mcilp->mci_type == MCI_GLOBALS ))) 
				tip->stat.checkdate   =  mcip->stat.checkdate; 

			/* CTS 1105829 -> */
			if ( mcip->stat.ascii && ! ( tip->stat.binary && (mcilp->mci_type == MCI_GLOBALS )))
			{
				tip->stat.ascii = mcip->stat.ascii ;
				tip->stat.binary = 0 ;  /* reset the komplement */
				tip->language = ' ';
			} /* <- CTS 1105829 */

			if ( mcip->stat.text && ! ( tip->stat.binary && (mcilp->mci_type == MCI_GLOBALS )))
			{
				tip->stat.text = mcip->stat.text ;
				tip->stat.ascii = 0 ;
				tip->stat.binary = 0 ;  /* reset the komplement */
				tip->language = ' ';
			}

			if ( mcip->protstyle)
			{
				tip->protstyle = mcip->protstyle ;
			}

			if ( mcip->stat.definition ) tip->stat.definition =  mcip->stat.definition ;
			if ( mcip->compiler && ! ( tip->compiler && (mcilp->mci_type == MCI_GLOBALS )))
				tip->compiler = mcip->compiler ;
			if ( mcip->stat.noobject )   
			{
				rc = collect_obj_list ( target , tip , NULL, NULL, NULL );
				tip->stat.noobject   =  mcip->stat.noobject ;
			}
			if ( mcip->stat.noobjcopy ) tip->stat.noobjcopy =  mcip->stat.noobjcopy ;

			if ( mcip->type  && (mcip->type != tip->type) )
			{
				if ( mcip->type == TT_INC )
				{
					tip->version= ' ';
					if ( (tip->type == TT_MOD) || (tip->type == TT_MEM) )
					{
						move_target ( tip, &modules, &includes);
						tip->type = mcip->type;
					}
				}
				else
				{
					DBG0 (( "WARNING: type redefinition only to include allowed \n" ))
					DBG0 (( "         type redefinition for %s/%s ignored \n", tip->layer, tip->name  ))
				}
				/*
				if (( mcip->type == TT_MOD) || (mcip->type == TT_MEM) )
				{
					tip->version= mcip->version;
					if  (tip->type == TT_INC )
					{
						move_target ( tip, &includes, &modules);
						tip->type = mcip->type;
					}
				}
				*/
			}

			if ( mcip->file && mcip->file->file && *mcip->file->file )
			{
				for ( sfipp = &(tip->file.next) ; *sfipp ; sfipp = &(*sfipp)->next );
				
				for (fip = mcip->file; fip; fip = fip->next)
				{
					/* use tip->afile for TT_MOD */
					tip->fixed_file = TRUE;
					*sfipp = (struct file_info *) palloc (sizeof (struct file_info));
					FILL (  *sfipp , 0 , sizeof(struct file_info) );
					(*sfipp)->nodistrib = fip->nodistrib;
					(*sfipp)->file = (char *) palloc( strlen( fip->file) + 1 );
					(void) strcpy( (*sfipp)->file, fip->file );
					rc = substitute_internal_variables(&(*sfipp)->file, tip);
					if ( rc < 0 )
					{
						DBG4 (( "coldsc: subst_variant error %d \n", rc ))
						return ( rc );
					}

					/* if in com-decription for all files (PTS 1102241) */
					/* PTS 1109460
					if (mcip->file[strlen(mcip->file)-1]=='/')
						strcat(tip->afile,tip->name);
					*/
					sfipp = &(*sfipp)->next;
				}
			}

			/* CTS 1112700 */
			if ( mcip->language )
				tip->language = mcip->language;

			if (mcip->local_source)
			{
				tip->local_source = (char *) repalloc( tip->local_source,strlen( mcip->local_source) + 1 );
				(void) strcpy (tip->local_source, mcip->local_source);
                rc = substitute_internal_variables(&(tip->local_source), tip);
				if ( rc < 0 )
				{
					DBG4 (( "coldsc: subst_variant error %d \n", rc ))
					return ( rc );
				}
			}

			if ( tip->stat.exec == 0 ) 
				tip->stat.exec = mcip->stat.exec; /* CTS 1110662 */

			DBG7 (( "coldsc: version       now '%c' \n", tip->version ))
			DBG7 (( "coldsc: shrglob       now  %d  \n", tip->stat.shrglob ))
			DBG7 (( "coldsc: debug         now  %d  \n", tip->stat.debug ))
			DBG7 (( "coldsc: profile       now  %d  \n", tip->stat.profile ))
			DBG7 (( "coldsc: forcemake     now  %d  \n", tip->stat.forcemake ))
			DBG7 (( "coldsc: uncond        now  %d  \n", tip->stat.uncond ))
			DBG7 (( "coldsc: binary        now  %d  \n", tip->stat.binary ))
			DBG7 (( "coldsc: asci          now  %d  \n", tip->stat.ascii )) /* CTS 1105829 */
			DBG7 (( "coldsc: text          now  %d  \n", tip->stat.text ))
			DBG7 (( "coldsc: definition    now  %d  \n", tip->stat.definition ))
			DBG7 (( "coldsc: noobject      now  %d  \n", tip->stat.noobject ))
			DBG7 (( "coldsc: noobjcopy     now  %d  \n", tip->stat.noobjcopy ))
			DBG7 (( "coldsc: nodebug       now  %d  \n", tip->stat.nodebug )) /* CTS 1102466 */
			DBG7 (( "coldsc: exec          now  %d  \n", tip->stat.exec )) /* CTS 1110662 */
			DBG7 (( "coldsc: noextension   now  %d  \n", tip->stat.noextension )) /* CTS 1112700 */
			DBG7 (( "coldsc: no_dep_remake now  %d  \n", tip->stat.no_dep_remake )) /* CTS 1112700 */


			/* if options specified, override the VMAKE_OPTION */

			if ( mcip->stat.level_i != 0 ||
			 mcip->stat.level_s != 0 || mcip->stat.level_p != 0 ||
				 mcip->stat.level_c != 0 || mcip->stat.level_o != 0    )
			{
				tip->stat.level_i = mcip->stat.level_i ;
				tip->stat.level_s = mcip->stat.level_s ;
				tip->stat.level_p = mcip->stat.level_p ;
				tip->stat.level_c = mcip->stat.level_c ;
				tip->stat.level_o = mcip->stat.level_o ;
				DBG7 (( "coldsc: vmake option changed \n" ))
			}
			
			if (mcip->suppress_uncond)
				suppress_uncond = mcip->suppress_uncond;
			
			
			next_matched_mci = matched_mci_list->next;
			FREE_AND_SET_TO_NULL( matched_mci_list);
			matched_mci_list = next_matched_mci;
		}
	}
	DBG8 (( "coldsc: collected compile info for    '%s' \n", tip->name ))

	/* if present, use override module compile info */
	/* it stems from a target list or from the command line */
	/* it always overrides the information from the complie list */

	if ( ov_mcip )
	{
	    for ( sip = ov_mcip->descriptions ; sip != NULL ; sip = sip->next )
	    {
		    rc = insert_description ( sip , & tip->descriptions, tip, MCI_OVERWRITE );
		    if ( rc < 0 )
		    {
		        DBG4 (( "coldsc: insert error %d \n", rc ))
		        return ( rc );
		    }
		    DBG8 (( "coldsc: inserted '%s' \n", sip->descriptor ))
	    }

	    if ( ov_mcip->version != ' ' ) tip->version = ov_mcip->version ;
	    if ( ov_mcip->stat.shrglob ) tip->stat.shrglob = ((ov_mcip->stat.shrglob == 1) &&
				(tip->stat.shrglob != 2 )) ? 1 : 2 ;
	    if ( ov_mcip->stat.debug   ) tip->stat.debug   = ov_mcip->stat.debug ;
		if ( ov_mcip->stat.nodebug   ) tip->stat.nodebug   = ov_mcip->stat.nodebug ; /* CTS 1102466 */
	    if ( ov_mcip->stat.profile ) tip->stat.profile = ov_mcip->stat.profile ;
		if ( ov_mcip->stat.noextension ) tip->stat.noextension = ov_mcip->stat.noextension ; /* CTS 1112700 */
		if ( ov_mcip->stat.no_dep_remake ) tip->stat.no_dep_remake = ov_mcip->stat.no_dep_remake ; 
		if ( ov_mcip->stat.checkdate ) tip->stat.checkdate = ov_mcip->stat.checkdate ; 
		if ( ov_mcip->stat.distribute ) tip->stat.distribute = ov_mcip->stat.distribute ; 
		if ( ov_mcip->stat.exec ) tip->stat.exec = ov_mcip->stat.exec ; 

	    if ( ov_mcip->stat.binary || ov_mcip->stat.ascii || ov_mcip->stat.text ) /* CTS 1105829 -> */ 
        {
			tip->stat.binary = ov_mcip->stat.binary ;
			/* every binary before ascii */
			tip->stat.ascii = ov_mcip->stat.binary ? 0 : ( ov_mcip->stat.text ? 0 : 1 );
			tip->stat.text = ov_mcip->stat.text; 
            tip->language = ' ';
			if (tip->type != TT_DIRLIST)
				tip->type = TT_MOD;
        }
	    if ( ov_mcip->stat.definition ) tip->stat.definition =  ov_mcip->stat.definition;
	    if ( ov_mcip->stat.noobject )   
		{
			rc = collect_obj_list ( target , tip , NULL, NULL, NULL );
			tip->stat.noobject = ov_mcip->stat.noobject ;
		}
	    if ( ov_mcip->stat.noobjcopy ) tip->stat.noobjcopy =  ov_mcip->stat.noobjcopy ;
	    if ( ov_mcip->stat.forcemake ) 
        {
            tip->stat.uncond =  ov_mcip->stat.uncond ;
            tip->stat.forcemake =  ov_mcip->stat.forcemake ;
        }

		if ( ov_mcip->remake && tip->type == TT_MOD )
		{

			rc = insert_dependency ( tip , & tip->callers->target->remakes );
			if ( rc < 0 )
			{
				DBG8 (( "coldesc: insert_dep error %d \n", rc ))
				return ( rc );
			}
			DBG8 (( "coldesc: remake '%s' \n", tip->name ))
		}

		/* CTS 1105891 -> */
		if ( ov_mcip->nobind )
		{

			rc = insert_dependency ( tip , & tip->act_caller->nobinds );
			if ( rc < 0 )
			{
				DBG8 (( "coldesc: insert_dep error %d \n", rc ))
				return ( rc );
			}
			DBG8 (( "coldesc: nobind '%s' \n", tip->name ))
		}
		/* <- CTS 1105891 */

		if ( ov_mcip->on_success )
		{
			rc = insert_dependency ( tip , & tip->act_caller->on_success );
			if ( rc < 0 )
			{
				DBG8 (( "coldesc: insert_dep error %d \n", rc ))
				return ( rc );
			}
			DBG8 (( "coldesc: on_success '%s' \n", tip->name ))
		}


        if ( ov_mcip->file && ov_mcip->file->file && *ov_mcip->file->file)
        {
            while ( tip->file.next )
            {
                struct file_info * fip;
                fip = tip->file.next;
                tip->file.next = fip->next;
				FREE_AND_SET_TO_NULL(fip->file);
                FREE_AND_SET_TO_NULL(fip);
            }
			if (tip->variantfile)
			{
				struct file_info * fip = tip->variantfile;
				while (fip) 
				{
					tip->variantfile = fip;
					fip = tip->variantfile->next;
					FREE_AND_SET_TO_NULL (tip->variantfile);
				}
			}
			if (ov_mcip->variantfile)
			{
				tip->variantfile = (struct file_info *) palloc (sizeof(struct file_info));
				if ( strchr(ov_mcip->variantfile, INTERNAL_VARPREFIX) )
				{
					tip->variantfile->file = palloc (strlen(ov_mcip->variantfile) +1);
					strcpy (tip->variantfile->file, ov_mcip->variantfile );
					rc = substitute_internal_variables(&tip->file.file, tip);
					if ( rc < 0 )
					{
						DBG4 (( "coldsc: subst_variant error %d \n", rc ))
						return ( rc );
					}
				}
				else
					tip->variantfile->file = ov_mcip->variantfile;
				tip->variantfile->nodistrib = 1;
			}

            tip->fixed_file = TRUE;
			if  (tip->type >= TT_LIB && tip->type <= TT_SHM) 
            {
                tip->file.file = (char *) repalloc( tip->file.file, strlen( ov_mcip->file->file ) + 1 );
                (void) strcpy( tip->file.file, ov_mcip->file->file );
				rc = substitute_internal_variables(&tip->file.file, tip);
				if ( rc < 0 )
				{
					DBG4 (( "coldsc: subst_variant error %d \n", rc ))
					return ( rc );
				}
            }
            else
            {
				
				sfipp = &(tip->file.next) ;
				
				for (fip = ov_mcip->file; fip; fip = fip->next)
				{
					/* use tip->afile for TT_MOD */
					tip->fixed_file = TRUE;
					*sfipp = (struct file_info *) palloc (sizeof (struct file_info));
					FILL (  *sfipp , 0 , sizeof(struct file_info) );
					(*sfipp)->nodistrib = fip->nodistrib;
					(*sfipp)->file = (char *) palloc( strlen( fip->file) + 1 );
					(void) strcpy( (*sfipp)->file, fip->file );
					rc = substitute_internal_variables(&(*sfipp)->file, tip);
					if ( rc < 0 )
					{
						DBG4 (( "coldsc: subst_variant error %d \n", rc ))
						return ( rc );
					}

					/* if in com-decription for all files (PTS 1102241) */
					/* PTS 1109460
					if (mcip->file[strlen(mcip->file)-1]=='/')
						strcat(tip->afile,tip->name);
					*/
					sfipp = &(*sfipp)->next;
				}
            }
        }

	    /* if options specified, override the VMAKE_OPTION */

		if ( ov_mcip->stat.level_i != 0 ||
		 ov_mcip->stat.level_s != 0 || ov_mcip->stat.level_p != 0 ||
		 ov_mcip->stat.level_c != 0 || ov_mcip->stat.level_o != 0    )
	    {
		    tip->stat.level_i = ov_mcip->stat.level_i ;
		    tip->stat.level_s = ov_mcip->stat.level_s ;
		    tip->stat.level_p = ov_mcip->stat.level_p ;
		    tip->stat.level_c = ov_mcip->stat.level_c ;
		    tip->stat.level_o = ov_mcip->stat.level_o ;
		    DBG7 (( "coldsc: vmake option changed \n" ))
	    }

		if (ov_mcip->local_source)
		{
			tip->local_source = (char *) repalloc( tip->local_source,strlen( ov_mcip->local_source) + 1 );
			(void) strcpy (tip->local_source, ov_mcip->local_source);
			rc = substitute_internal_variables(&(tip->local_source), tip);
			if ( rc < 0 )
			{
				DBG4 (( "coldsc: subst_variant error %d \n", rc ))
				return ( rc );
			}
		}

	    /* if a new include list specified, override a previous one */

	    if ( ov_mcip->includes )
	    {
		    iip = ov_mcip->includes ;
		    rc = collect_include_list ( target , tip , iip, MCI_OVERWRITE );
		    if ( rc < 0 )
		    {
		        DBG8 (( "coldsc: collect_inc error %d \n", rc ))
		        return ( rc );
		    }
		    DBG8 (( "coldsc: collected include list for    '%s' \n",
							        tip->name ))
	    }

	    /* if a new demand list specified, override a previous one */
	    if ( ov_mcip->objdemands )
	    {
		    dmip = ov_mcip->objdemands ;
		    rc = collect_demand_list ( target , tip , dmip, &tip->objdemands, DT_OBJDEMDEP, MCI_OVERWRITE );
		    if ( rc < 0 )
		    {
		        DBG8 (( "coldsc: collect_dem error %d \n", rc ))
		        return ( rc );
		    }
		    DBG8 (( "coldsc: collected objdemand list for    '%s' \n",
							        tip->name ))
	    }

	    if ( ov_mcip->srcdemands )
	    {
		    dmip = ov_mcip->srcdemands ;
		    rc = collect_demand_list ( target , tip , dmip, &tip->srcdemands, DT_SRCDEMDEP, MCI_OVERWRITE );
		    if ( rc < 0 )
		    {
		        DBG8 (( "coldsc: collect_dem error %d \n", rc ))
		        return ( rc );
		    }
		    DBG8 (( "coldsc: collected srcdemand list for    '%s' \n",
							        tip->name ))
	    }

		if ( ( ov_mcip->stat.deptype == DT_OBJDEMDEP ) && tip->callers )
		{
			/* set tip as demand for the calling target */
			tip->stat.deptype = ov_mcip->stat.deptype;
			rc = insert_demands ( tip , ov_mcip->objdemanddir, & tip->callers->target->objdemands );
			if ( rc < 0 )
			{
				DBG8 (( "coldemand: insert_dep error %d \n", rc ))
				return ( rc );
			}
			DBG8 (( "coldsc: objdemand '%s' \n", ov_mcip->name ))

		}

		if ( ( ov_mcip->stat.deptype == DT_SRCDEMDEP ) && tip->callers )
		{
			/* set tip as demand for the calling target */
			tip->stat.deptype = ov_mcip->stat.deptype;
			rc = insert_demands ( tip , ov_mcip->srcdemanddir, & tip->callers->target->srcdemands );
			if ( rc < 0 )
			{
				DBG8 (( "coldemand: insert_dep error %d \n", rc ))
				return ( rc );
			}
			DBG8 (( "coldsc: srcdemand '%s' \n", ov_mcip->name ))

		}

		/* if a dependency list specified append this list to a previous one */
	    if ( ov_mcip->dependencies )
	    {
		    dip = ov_mcip->dependencies ;
		    rc = collect_dep_list ( target , tip , dip, MCI_OVERWRITE );
		    if ( rc < 0 )
		    {
		        DBG8 (( "coldsc: collect_dep error %d \n", rc ))
		        return ( rc );
		    }
		    DBG8 (( "coldsc: collected dependency list for    '%s' \n",
							        tip->name ))
	    }

		/* objects */
	    if ( ov_mcip->objects )
	    {
		    tip->stat.noobject = 0;
		    oip = ov_mcip->objects ;
		    rc = collect_obj_list ( target , tip , oip, tip->localvariant, tip->globalvariants  );
		    if ( rc < 0 )
		    {
		        DBG8 (( "coldsc: collect_obj error %d \n", rc ))
		        return ( rc );
		    }
		    DBG8 (( "coldsc: collected object list for    '%s' \n",
							        tip->name ))
	    }
        /* extrenal dependencies */

	    if ( ov_mcip->ext_dependencies )
	    {
		    edip = ov_mcip->ext_dependencies ;
		    rc = collect_extdep_list ( target , tip , edip );
		    if ( rc < 0 )
		    {
		        DBG8 (( "coldsc: collect_extdep error %d \n", rc ))
		        return ( rc );
		    }
		    DBG8 (( "coldsc: collected external dependency list for    '%s' \n",
							        tip->name ))
	    }

		if ( ov_mcip->protstyle ) tip->protstyle = ov_mcip->protstyle ; 

		if (ov_mcip->suppress_uncond)
				suppress_uncond = ov_mcip->suppress_uncond;		

        DBG7 (( "coldsc: version       now '%c' \n", tip->version ))
	    DBG7 (( "coldsc: shrglob       now  %d  \n", tip->stat.shrglob ))
	    DBG7 (( "coldsc: debug         now  %d  \n", tip->stat.debug ))
		DBG7 (( "coldsc: nodebug       now  %d  \n", tip->stat.nodebug )) /* CTS 1102466 */
	    DBG7 (( "coldsc: profile       now  %d  \n", tip->stat.profile ))
	    DBG7 (( "coldsc: forcemake     now  %d  \n", tip->stat.forcemake ))
	    DBG7 (( "coldsc: uncond        now  %d  \n", tip->stat.uncond ))
	    DBG7 (( "coldsc: binary        now  %d  \n", tip->stat.binary ))  
		DBG7 (( "coldsc: ascii         now  %d  \n", tip->stat.ascii )) /* CTS 1105829 */
		DBG7 (( "coldsc: text          now  %d  \n", tip->stat.text )) 
	    DBG7 (( "coldsc: definition    now  %d  \n", tip->stat.definition ))
	    DBG7 (( "coldsc: noobject      now  %d  \n", tip->stat.noobject ))
	    DBG7 (( "coldsc: noobjcopy     now  %d  \n", tip->stat.noobjcopy ))
		DBG7 (( "coldsc: noextension   now  %d  \n", tip->stat.noextension )) /* CTS 1112700 */
		DBG7 (( "coldsc: no_dep_remake now  %d  \n", tip->stat.no_dep_remake ))


		if ( ( tip->stat.noobject || tip->stat.noobjcopy || tip->objects ) && 
            (tip->stat.binary || tip->stat.ascii || tip->stat.text ))
		{
		    DBG0 (( "inconsistent descriptions for %s\n", tip->name ))
			DBG0 ((" ('(binary || ascii || text ) && (noobjcopy || noobject || objects)')\n" ))
			eprotwrite( "inconsistent descriptions for %s\n", tip->name );
			eprotwrite (" ('(binary || ascii || text) && (noobjcopy || noobject || objects)')\n" );
			return ( -1 );
		}
    }

	if ( opt_uncond && 
		(	( (tip->type >= TT_MOD) && (! suppress_uncond )) ||
			 ( (tip->type == TT_INC) && opt_uncondincl)
	    )           &&
		( ! opt_nodep || ! tip->act_caller  ||  ! tip->act_caller->act_caller )
	   )
	{
		tip->stat.uncond = 1 ;
		DBG7 (( "coldsc: uncond      now  %d  \n", tip->stat.uncond ))
    }

	DBG8 (( "coldsc: collected target descriptions '%s' \n", tip->name ))

	DBG4 (( "coldsc: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/


static  int     collect_include_list
(   
    char            * target,
    struct trg_info * tip   ,
    struct inc_info * iip,
	int               mci_type
)
{
	int                     errcnt , rc ;
	struct trg_info         * stip ;

    DBG2 (( "colinc: called \n" ))
	DBG3 (( "colinc: target '%s' \n", target ))

	rc = errcnt = 0 ;
	DBG7 (( "colinc: target language '%c' \n", tip->language ))

	for ( ; iip != NULL ; iip = iip->next )
	{
		if ( iip->disposition == '-' )
		{
			rc = remove_dependency ( iip->name , tip, DT_INCDEP );
		    if ( rc < 0 )
		    {
			    DBG8 (( "colinc: remove_dep error %d \n", rc ))
			    return ( rc );
		    }
			continue ;
		}
		if ( iip->disposition == 0 )
		{
			if ( mci_type == MCI_GLOBALS )
			{
				DBG0 (( "error in %s.com: use only includes('%s') with + or -\n", env_VMAKE_GLOBALS, iip->name ))
				eprotwrite ( "error in %s.com: use only includes('%s') with + or -\n", env_VMAKE_GLOBALS, iip->name  );
			    return ( -3 );
			}
		    rc = clear_dependencies ( tip, DT_INCDEP);
		    if ( rc < 0 )
		    {
			    DBG8 (( "colinc: clear_dep error %d \n", rc ))
			    return ( rc );
		    }
		}

		/* if default include list was overwritten by an empty
		   list, an empty inc_info is in the list */
		if ( iip->name [ 0 ] == '\0' )
		{
		    continue ;
		}

		DBG7 (( "colinc: include module '%s' \n", iip->name ))
		if ( (tip->version == ' ') && ! strcmp ( iip->name , target ) )
		{
			break ;
		}

		if ( iip->language != ' ' )
		{
		    DBG7 (( "colinc: include language '%c' \n", iip->language ))
			/*IST 1997-04-08 */
		    /* avoid collecting other languages include files for pascal files */

			/* GG 1997-11-28 allow C includes in C++		 */
			/* GG 1997-11-28 vopt_c_includes_p valid for C++ */
		    if ( ( iip->language != tip->language ) )
		    {
				switch ( tip->language )
				{
				case 'a':
                case 'p': 
					continue;
				case 'x':
				case 'c':
					if ( ! vopt_c_includes_p && iip->language == LANG_PASCAL_P[ 0 ] ) 
						continue;
					break;
				}
		    }
		}
		if (strcmp (iip->name, "[s") == 0)
			rc=1;
        
		rc = collect( iip->name , ' ' , ' ' , DT_INCDEP, LEVEL_MODULE , tip , & stip );
		
		if ( tip == stip )
		{
			DBG3 (( "colinc: found direct refernce to itself for '%s'\n", iip->name ))
			continue ;
		}

		if ( rc <= 0 )
		{
			DBG0 (( "error in include list of '%s' ('%s')\n", tip->name, iip->name ))
			eprotwrite ( "error in include list of '%s' ('%s')\n", tip->name, iip->name );
		    errcnt ++ ;
		    continue ;
		}

		if (stip->parentlinkoptions)
			if (insert_parentlinkoptions(tip,stip->parentlinkoptions, FALSE) < 0)
			{
				DBG0 (( "error in parentlinkoption for %s from %s \n", tip->name, stip->name ))
				eprotwrite ( "error in parentlinkoption for %s from %s \n", tip->name, stip->name );
				errcnt ++ ;
			}
		


		/* binary includes needs a version and type TT_MOD */ 
		if ( stip->stat.binary || stip->stat.ascii || stip->stat.text  ) /* CTS 1105829 */
		{
			stip->version = tip->version;
			stip->type = TT_MOD ;
            stip->stat.deptype = DT_OBJDEMDEP;
			/* collect binary includes as a demand */
			rc = insert_demands ( stip , "", & tip->objdemands );
			if ( rc < 0 )
			{
				DBG8 (( "colinc: insert_dep error %d \n", rc ))
				return ( rc );
			}
		}

		if ( rc == 0 )
		{
		    if ( ! stip ) continue ; /* empty lines or comments */

		    /*
		     *  This 'fallthrough' is for targets which are already
		     *  present; they should be inserted into the dependencies
		     *  list anyway. On the other hand, they should return
		     *  rc=1. So complain.
		     */
		    DBG0 (( "BUGCHECK: collect returned 0 for '%s'\n",
							    iip->name ))
		}

		rc = insert_dependency ( stip , & tip->dependencies );
		if ( rc < 0 )
		{
			DBG8 (( "colinc: insert_dep error %d \n", rc ))
			return ( rc );
		}
		DBG8 (( "colinc: dependent '%s' \n", stip->name ))

		if ( tip->stat.level_s && ! stip->stat.level_s )
		{
			stip->stat.level_s = 1 ;
			DBG7 (( "colinc: set include level_s \n" ))
		}

		/*
		 *  IST 1994-03-09 modules can include their own includes.
		 *  The include list is ordered by dependency.
		 *  Include modules behind the current one itself
		 *  can never be include modules for it.
		 */
		if ( ! strcmp ( iip->name , target ) ) break ;
	}

	if ( errcnt )
	{
		DBG8 (( "target collect errors %d \n", errcnt ))
		return ( - 3 );
	}

	DBG4 (( "colinc: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/


static  int     collect_demand_list
(   
    char                    * target,
    struct trg_info         * tip   ,
    struct dem_mod_info     * dip ,
    struct dem_info         ** demand_list ,
    unsigned int            depkind,
	int                     mci_type

)
{
	int                     errcnt , rc;
	struct trg_info         * stip ;


    DBG2 (( "coldemand: called \n" ))
	DBG3 (( "coldemand: target '%s' \n", target ))

	rc = errcnt = 0 ;

	for ( ; dip != NULL ; dip = dip->next )
	{
		if ( dip->disposition == '-' )
		{
			/* Can't remove dependency dip->name from &tip->dependencies 
			   because dip->name could be another dependency */
		    rc = remove_demands ( dip->name , dip->directory , &(*demand_list) );
		    if ( rc < 0 )
		    {
			    DBG8 (( "coldemand: remove_dep error %d \n", rc ))
			    return ( rc );
		    }
		    continue ;
		}
		if ( dip->disposition == 0 )
		{
			if ( mci_type == MCI_GLOBALS )
			{
				DBG0 (( "error in %s.com: use only demands('%s') with + or -\n", env_VMAKE_GLOBALS, dip->name ))
				eprotwrite ( "error in %s.com: use only demands('%s') with + or -\n", env_VMAKE_GLOBALS, dip->name  );
			    return ( -3 );
			}
		    /* rc = clear_demands ( dip->directory , &tip->demands ); */
            rc = clear_demands ( dip->directory , &(*demand_list) );
		    if ( rc < 0 )
		    {
			    DBG8 (( "coldemand: clear_dep error %d \n", rc ))
			    return ( rc );
		    }
		}

		/* if default demand list was overwritten by an empty
		   list, an empty inc_info is in the list */
		if ( dip->name [ 0 ] == '\0' )
		    continue ;

		DBG7 (( "coldemand: demand module '%s' \n", dip->name ))

		if ( (tip->version == ' ') && ! strcmp ( dip->name , target ) )
			break ;

        if ( is_include( dip->name ) )
        {
		    rc = collect ( dip->name , ' ' , ' ', 
                    depkind, LEVEL_MODULE , tip , & stip );
        }
        else
        if (  tip->version == ' ' )
        {
            struct trg_info * tmp_tip = NULL;
            /* look for a caller version */
			
			/* &gar PTS 1103112 + 1103486->
			for ( sdip = tip->callers; sdip->next && sdip->target->version == ' '; 
                    sdip = sdip->next );
            */
			
			for ( tmp_tip = tip->act_caller; tmp_tip && ( tmp_tip->version == ' '); 
				tmp_tip = tmp_tip->act_caller);
			/* <- PTS 1103112 */
			
            if ( tmp_tip )
		        rc = collect ( dip->name , tmp_tip->version , tmp_tip->version, 
                        depkind, LEVEL_MODULE , tip , & stip );
            else
				/* &gar PTS 1103112 - if nothing -> vmakeversion */
		        /*rc = collect ( dip->name , ' ' , ' ',  */
				rc = collect ( dip->name , vmakeversion , vmakeversion,
                        depkind, LEVEL_MODULE , tip , & stip );
        }
        else
		    rc = collect ( dip->name , tip->version , tip->version, 
                    depkind, LEVEL_MODULE , tip , & stip );

		if ( rc <= 0 )
		{
			DBG0 (( "error in demand list of '%s' ('%s')\n", tip->name, dip->name ))
			eprotwrite  ("error in demand list of '%s' ('%s')\n", tip->name, dip->name );
		    errcnt ++ ;
		    continue ;
		}

		/*
		if ( rc == 0 )
		{
		    if ( ! stip ) continue ; * empty lines or comments *

		    *
		     *  This 'fallthrough' is for targets which are already
		     *  present; they should be inserted into the dependencies
		     *  list anyway. On the other hand, they should return
		     *  rc=1. So complain.
		     *
		    DBG0 (( "BUGCHECK: collect returned 0 fo '%s'\n", dip->name ))
		}
		*/
		if ( tip == stip )
		{
			DBG3 (( "colinc: found direct refernce to itself for '%s'\n", dip->name ))
			continue ;
		}

		rc = insert_demands ( stip , dip->directory , &(*demand_list) );
		if ( rc < 0 )
		{
			DBG8 (( "coldemand: insert_dep error %d \n", rc ))
			return ( rc );
		}
		DBG8 (( "coldemand: demand '%s' \n", stip->name ))

		if ( depkind == DT_OBJDEMDEP )
        {
            rc = insert_dependency ( stip , & tip->dependencies );
		    if ( rc < 0 )
		    {
			    DBG8 (( "coldemand: insert_dep error %d \n", rc ))
			    return ( rc );
		    }
		    DBG8 (( "coldemand: dependent '%s' \n", stip->name ))
        }

		/*
		 *  modules can include their own demands.
		 *  The demand list is ordered by dependency.
		 *  Demand modules behind the current one itself
		 *  can never be demand modules for it.
		 */
		if ( ! strcmp ( dip->name , target ) ) break ;
	}

	if ( errcnt )
	{
		DBG8 (( "target collect errors %d \n", errcnt ))
		return ( - 3 );
	}

	DBG4 (( "coldemand: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/


static  int     collect_dep_list 
(   
    char            * target,
    struct trg_info * tip   ,
    struct inc_info * dip,   
	int mci_type 
)
{
	int                     errcnt , rc;
	struct trg_info         * stip ;


    DBG2 (( "coldep: called \n" ))
	DBG3 (( "coldep: target '%s' \n", target ))

	rc = errcnt = 0 ;

	for ( ; dip != NULL ; dip = dip->next )
	{

		if ( dip->disposition == '-' )
		{
			rc = remove_dependency ( dip->name , tip, DT_OBJDEP );
		    if ( rc < 0 )
		    {
			    DBG8 (( "colinc: remove_dep error %d \n", rc ))
			    return ( rc );
		    }
			continue ;
		}
		if ( dip->disposition == 0 )
		{

			if ( mci_type == MCI_GLOBALS )
			{
				DBG2 (( "warning in %s.com: use only dependencies('%s') with + or -\n", env_VMAKE_GLOBALS, dip->name ))
			}
/*			else
			{
				rc = clear_dependencies ( &tip->dependencies, DT_OBJDEP);
				if ( rc < 0 )
				{
					DBG8 (( "colinc: clear_dep error %d \n", rc ))
					return ( rc );
				}
			}
*/
		}

		if ( dip->name [ 0 ] == '\0' )
		{
		    continue ;
		}

		DBG7 (( "coldep: demand module '%s' \n", dip->name ))
		if ( (tip->version == ' ') && ! strcmp ( dip->name , target ) )
		{
			break ;
		}


        if ( is_include( dip->name ) )
        {
		    rc = collect ( dip->name , ' ' , ' ', 
                    DT_OBJDEP , LEVEL_MODULE , tip , & stip );
        }
        else
        if (  tip->version == ' ' )
        {
            struct trg_info * tmp_tip = NULL;
			int temp_count=0;
            
			/* look for a caller version */
            /* &gar PTS 1103112 + 1103486 ->
			for ( sdip = tip->callers; sdip->next && sdip->target->version == ' '; 
                    sdip = sdip->next );
            */
			for ( tmp_tip = tip->act_caller; tmp_tip && 
				( (tmp_tip->version == ' ') || (tmp_tip->version == 'n') ); 
				tmp_tip = tmp_tip->act_caller )
			{ 
				if ( temp_count++ > 30 ) 
				{
					DBG2 (( "WARNING: found act_caller recursion for %s\n", tip->name ))
					break;
				}
				DBG3 (("look for act_caller:%s(%c)\n", tmp_tip->name, tmp_tip->version))
			}
			if (tmp_tip && ( tmp_tip->version == ' ' || tmp_tip->version == 'n' ) ) 
				tmp_tip = NULL;
			/* <- PTS 1103112 */
			
            if ( tmp_tip )
		        rc = collect ( dip->name , tmp_tip->version , tmp_tip->version, 
                        DT_OBJDEP , LEVEL_MODULE , tip , & stip );
            else
				/* &gar PTS 1103112 - if nothing -> vmakeversion */
		        /*rc = collect ( dip->name , ' ' , ' ',  */
				rc = collect ( dip->name , vmakeversion , vmakeversion,
                        DT_OBJDEP , LEVEL_MODULE , tip , & stip );
        }
        else
		    rc = collect ( dip->name , tip->version , tip->version, 
                    DT_OBJDEP , LEVEL_MODULE , tip , & stip );

		if ( rc <= 0 )
		{
			DBG0 (( "error in dependency list of '%s' ('%s')\n", tip->name, dip->name ))
			eprotwrite ( "error in dependency list of '%s' ('%s')\n", tip->name, dip->name );
		    errcnt ++ ;
		    continue ;
		}
		
/*
	collect return 1 if already collected 
		if ( rc == 0 )
		{
			DBG0 (( "error in dependency list of '%s' ('%s')\n", tip->name, dip->name ))
		    if ( ! stip ) continue ; * empty lines or comments *

		
		     *  This 'fallthrough' is for targets which are already
		     *  present; they should be inserted into the dependencies
		     *  list anyway. On the other hand, they should return
		     *  rc=1. So complain.
		     *
		    DBG0 (( "BUGCHECK: collect returned 0 for '%s'\n",
							    dip->name ))
		}
	*/
		if ( tip == stip )
		{
			DBG3 (( "coldep: found direct refernce to itself for '%s'\n", dip->name ))
			continue ;
		}

        /* stip->stat.depkind = DT_OBJDEP; */
        rc = insert_dependency ( stip , & tip->dependencies );
		if ( rc < 0 )
		{
			DBG8 (( "coldep: insert_dep error %d \n", rc ))
			return ( rc );
		}
		DBG8 (( "coldep: dependent '%s' \n", stip->name ))

		if (stip->parentlinkoptions)
			if (insert_parentlinkoptions(tip,stip->parentlinkoptions, FALSE) < 0)
			{
				DBG0 (( "error in parentlinkoption for %s from %s \n", tip->name, stip->name ))
				eprotwrite ( "error in parentlinkoption for %s from %s \n", tip->name, stip->name );
				errcnt ++ ;
			}
		

        /*
		 *  modules can include their own dependencies.
		 *  The dependency list is ordered by dependency.
		 *  Depending modules behind the current one itself
		 *  can never be dependend modules for it.
		 */
		if ( ! strcmp ( dip->name , target ) ) break ;
	}

	if ( errcnt )
	{
		DBG8 (( "target collect errors %d \n", errcnt ))
		return ( - 3 );
	}

	DBG4 (( "coldep: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/

static  int     collect_obj_list
(   
    char            * target,
    struct trg_info * tip   ,
    struct obj_info * oip   ,
	char                    * local_variant,
	struct variant_info     * global_variants
)
{
	struct obj_info			*listp, *ip;
	struct obj_info         **current; 

    DBG2 (( "colobj: called \n" ))
	DBG3 (( "colobj: target '%s' \n", target ))

	/* clear an existing object list */
	listp = tip->objects;
	while ( listp )
	{
		ip = listp ;
		listp = ip->next ;
		FREE_AND_SET_TO_NULL(ip->file);
		FREE_AND_SET_TO_NULL(ip->name_o);
		FREE_AND_SET_TO_NULL(ip);
	}

	current = &tip->objects;
	/* all objects are copied */
	for (listp = oip; listp; listp=listp->next)
	{
		ip = (struct obj_info *) palloc (sizeof (struct obj_info));
		ip->name_o = strdup(listp->name_o);
		if ( ! ip->name_o )
	    {
		    DBG0 (( "insufficient dynamic memory \n" ))
		    return ( - 3 );
	    }
		if ( substitute_internal_variables(&(ip->name_o), tip) < 0)
			return -2;
		ip->file = NULL;
		ip->next = NULL;
		(*current) = ip;
		current = &(ip->next);
	}
	
	DBG4 (( "colobj: returning %d \n", 0 ))
	return ( 0 );
}


/*==========================================================================*/

static  int     collect_extdep_list
(   
    char            * target,
    struct trg_info * tip   ,
    struct extdep_info * dip
)
{
	struct extdep_info  * listp, * ip, ** current ;
    int                 rc ;
	
    DBG2 (( "colextdep: called \n" ))
	DBG3 (( "colextdep: target '%s' \n", target ))
	rc = 0 ;

	/* clear an existing object list */
	listp = tip->ext_dependencies;
	while ( listp )
	{
		ip = listp ;
		listp = ip->next ;
		FREE_AND_SET_TO_NULL(ip->file);
		FREE_AND_SET_TO_NULL(ip);
	}

	current = &tip->ext_dependencies;
	/* all objects are copied */
	for (listp = dip; listp; listp=listp->next)
	{
		ip = (struct extdep_info *) palloc (sizeof (struct extdep_info));
		ip->file = strdup(listp->file);
		if ( ! ip->file )
	    {
		    DBG0 (( "insufficient dynamic memory \n" ))
		    return ( - 3 );
	    }
		if ( substitute_internal_variables(&(ip->file), tip) < 0)
			return (-2);
		ip->next = NULL;
		rc = get_date_and_size ( ip->file, &(ip->date), (long*)0, FALSE );
		if ( rc ) return ( rc );
		(*current) = ip;
		current = &(ip->next);
	}
	
	DBG4 (( "colextdep: returning %d \n", 0 ))
	return ( 0 );
}


/*==========================================================================*/

static  int     collect_target_option 
(   
    char                    * trgline   ,
    struct trg_info         * ctip      ,
    struct mod_com_info     * mcip
)
{
	int     rc = 0, i, colon = 0 ;
    char    option [ NAMESIZE ] = "" ;
    char    * token;
    char    token_sep[] = ", ";
    int     offset = 0;
	char    * sep;
	int		token_len;

    DBG2 (( "coltrgopt: called \n" ))
    DBG3 (( "coltrgopt: trgline '%s' \n", trgline ))
	DBG3 (( "coltrgopt: caller  '%s' \n", ctip ? ctip->name:"none" ))


    FILL ( option , 0 , NAMESIZE );
    for ( ; isspace(*trgline) ; trgline++ );
    trgline++; /* skip '?' */

    token = strtok( trgline, " ");
    while( token )
    {
		sep = strchr(token, ':');
		token_len=strlen(token);
		if (sep) 
			token_len = sep-token;

		if ( strlen(option) + token_len + 1 > NAMESIZE )
		{
			DBG0(( "coltrgopt: option \"? %s...\" to long \n", option ))
			eprotwrite ( "coltrgopt: option \"? %s...\" to long \n", option );
			return( -1 );
		}
		
		strncat (option, token, token_len);

		if (sep)
		{
			token=sep;
			colon=1;
			break;
		}
		token = strtok( NULL, " " );
    }
    if ( ! colon )
    {
        DBG0(( "coltrgopt: ':' not found in target option \n" ))
		eprotwrite ( "coltrgopt: ':' not found in target option \n" );
        return( -1 );
    }

    /* set another token separator (but only one!)  with :[char]: */
    if ( (token_len > 1) && (*(token+2) == ':') )
    {
        token_sep[0] = *(token+1);
        token_sep[1] = '\0';
        offset = 2;
    }

    if ( *(token + 1 + offset ) != '\0' )
        trgline += token - trgline + 1 + offset;
    else
        trgline += token - trgline + 2 + offset;

    DBG4 (( "coltrgopt: option     '%s' \n", option ))

	
    
    /* skip over [whitspaces] */
    for ( ; *trgline && ( isspace(*trgline) || (*trgline == ':')) ; trgline++ );

	if ( !strnicmp( option , TRGDSC_REQU_VERSION, strlen(TRGDSC_REQU_VERSION) ) )
    {
		DBG3 (("required vmake version: %s", trgline))
		if (strncmp (VMAKE_VERSION_NUMBER, trgline, strlen(VMAKE_VERSION_NUMBER)) < 0)
		{
			DBG0 (( "%s require a newer vmake version  (%s) \n", ctip->name, trgline   ))
			eprotwrite ( "%s require a newer vmake version  (%s) \n", ctip->name, trgline  );
            return ( -1 );
		}
	}
	else
		/*
	if (ctip->type == TT_COM)
	{
		DBG0 (("Warning: option '%s' in com description '%s' doesn't have effect\n", option, ctip->name))
	}
	else
	*/
    if ( ! stricmp( option , TRGDSC_DEFLAYER ) )
    {
        char    * layer = trgline;
        for ( i = 0 ; !isspace(*trgline) && *trgline; trgline++, i++ )
            ;
        mcip->defaultlayer = (char *) repalloc( mcip->defaultlayer, i + 1 );
        COPY( mcip->defaultlayer, layer, i );
        mcip->defaultlayer[ i ] = '\0';

        DBG4 (( "coltrgopt: value for option '%s' \n", mcip->defaultlayer ))
        if ( ctip && (ctip->type != TT_COM)) 
        {
            if ( (ctip->type > TT_MOD) && (ctip->type <= TT_MAC) )
            {
                ctip->layer = (char *) repalloc( ctip->layer, strlen( mcip->defaultlayer ) + 1 );
                (void) strcpy( ctip->layer , mcip->defaultlayer );
                DBG4 (( "coltrgopt: ctip layer now: '%s' \n", ctip->layer ))
            }
			else
			{
				DBG4 (( "coltrgopt: no effect '%s' for '%s'\n", TRGDSC_DEFLAYER , ctip->name ))
				return ( -1 );
			}
        }
        rc = 0;
    }
    else
	if ( ! stricmp( option , TRGDSC_PARENTLINKOPTION ) )
    {
		if (ctip->type == TT_COM)
			DBG0 (("Warning: parentlink option in com description '%s' doesn't have effect", ctip->name))
		else
		{
			rc = insert_mod_parentlinkoption (trgline, mcip);
			if (rc < 0 )
			{
				DBG0 (( "error in parentlinkoption '%s' \n", trgline ))
				eprotwrite ( "error in parentlinkoption '%s' \n", trgline );
				return ( rc );
			}

			if ( ctip ) 
			{
				rc = insert_parentlinkoptions (ctip, mcip->parentlinkoptions, FALSE);
				if (rc < 0 )
				{
					DBG0 (( "error in parentlinkoption '%s' \n", trgline ))
					eprotwrite ( "error in parentlinkoption '%s' \n", trgline );
					return ( rc );
				}        
			}
		}
        rc = 0;
    }
    else	
	if ( !strnicmp( option , TRGDSC_ERROR, strlen(TRGDSC_ERROR) ) )
    {
		DBG0 (( "Error: %s \n", trgline ))
		eprotwrite ( "Error: %s \n", trgline );
        return ( -1 );
	}
	else
	if ( !strnicmp( option , TRGDSC_EXECUTE, strlen(TRGDSC_EXECUTE) ) )
    {
		char *exe_options;
		struct trg_info * tipp;

		if (ctip->type == TT_COM)
		{
			DBG0 (( "ERROR: Wrong option '%s' in com description %s \n", TRGDSC_EXECUTE, ctip->name ))
			eprotwrite ( "ERROR: Wrong option '%s' in com description %s \n", TRGDSC_EXECUTE, ctip->name );
            return ( -2 );
        }


		/* parsed by collect_execution */
		exe_options = strchr( option, '(' );
        
		rc = collect_execution (trgline, exe_options, ctip, &tipp);
        if ( rc < 0 )
        {
            DBG0 (( "error collecting execution of '%s' \n", trgline ))
			eprotwrite ( "error collecting execution of '%s' \n", trgline );
            return ( rc );
        }
        else    
            DBG8 (( "vmake: collected '%s' \n", trgline ))

		rc = insert_dependency ( tipp , &(ctip->dependencies) );
        if ( rc < 0 )
        {
            DBG0 (( "error inserting '%s' \n", trgline ))
			eprotwrite ( "error inserting '%s' \n", trgline );
            return ( rc );
        }
        else
            DBG8 (( "vmake: inserted execution '%s' \n", trgline ))
		rc = 0;
	}
	else
	if ( !strnicmp( option , TRGDSC_REQUIRE, strlen(TRGDSC_REQUIRE) ) )
    {
        char            target[ NAMESIZE ];
        struct trg_info * tipp;
        
        if ( opt_require && (ctip != NULL) && (ctip->type != TT_COM) ) 
		{
            for ( i = 0 ; ! isspace( * trgline ) && * trgline; trgline++, i++ )
                if ( i < NAMESIZE - 1 ) target[ i ] = * trgline;
            target [ i ] = '\0';
            DBG4 (( "coltrgopt: value for option '%s' \n", target ))

	        rc = collect ( target , vmakeversion , getdefvers( vmakeversion ) , 
                    DT_INCDEP, LEVEL_MACRO, (struct trg_info*)NULL , &tipp );
            if ( rc < 0 )
            {
                DBG0 (( "error collecting '%s' \n", target ))
				eprotwrite ( "error collecting '%s' \n", target );
                return ( rc );
            }
            else    
                DBG8 (( "vmake: collected '%s' \n", target ))

            rc = insert_dependency ( tipp , & make_list );
            if ( rc < 0 )
            {
                DBG0 (( "error inserting '%s' \n", target ))
				eprotwrite ( "error inserting '%s' \n", target );
                return ( rc );
            }
            else
                DBG8 (( "vmake: inserted '%s' \n", target ))
        }
        rc = 0;
    }
    else
    if ( !strnicmp( option , TRGDSC_DEFVERSION, strlen(TRGDSC_DEFVERSION) ) )
    {
        if ( *trgline )
        {
            switch ( *trgline )
            {
            case 'f' :
            case 'q' :
            case 's' :
                newversion = *trgline;
                DBG4 (( "coltrgopt: version '%c' \n", newversion ))
                break;
            default :
                DBG4 (( "coltrgopt: unknown version '%c' \n", *trgline ))
            }
        }
        else
        {
            newversion = '?'; newdefault = '?';
            DBG4 (( "coltrgopt: reset version \n" ))
        }
        if ( *(trgline+1) )
        {
            switch ( *trgline )
            {
            case 'f' :
            case 'q' :
            case 's' :
                newdefault = *(trgline+1);
                DBG4 (( "coltrgopt: default version '%c' \n", newdefault ))
                break;
            default :
                DBG4 (( "coltrgopt: unknown version '%c' \n", *(trgline+1) ))
            }
        }
        rc = 0;
    }
    else
    if ( !strnicmp( option , TRGDSC_OUTPUT, strlen(TRGDSC_OUTPUT) ) )
    {
        token = strtok( trgline , token_sep );
        while( token != NULL )
        {
            char        * file = NULL;
            struct stat fileinfo;
            char        name [ NAMESIZE ];

			/* CTS 1110367 */
			char         nodistrib = check_nodistrib (token);

			ctip->fixed_file = TRUE ;

            substitute( token, &file );
			if ( strchr(file, INTERNAL_VARPREFIX) )
			{
				rc = substitute_internal_variables(&file, ctip);
				if ( rc <= 0 )
				{
					DBG0 (( "ERROR: variant substitution error for %s (%d) \n", file, rc ))
					return ( rc );
				}
			}


            rc = stat( file, & fileinfo );

            if ( ( ( rc == 0 && ( fileinfo.st_mode & S_IFDIR )) ||  
				( file[ strlen( file ) - 1 ] == '/' ))            /* CTS 1107469 */
				&& ctip && (ctip->type != TT_COM) )
            {
                remove_ext( ctip->name, name );
                switch ( ctip->type )
                {
                case TT_DLL :
                    (void) strcat( name, "." );
                    (void) strcat( name, env_DLL_TYPE );
                    break;
                case TT_LIB :
                    (void) strcat( name, "." );
                    (void) strcat( name, env_ARC_TYPE );
                    break;
                case TT_PGM :
                case TT_SHM :
#ifdef WIN32
                    (void) strcat( name, ".exe" );
#endif
					break;
                case TT_REL :
                case TT_SHR :
                default :
                    (void) strcpy( name, ctip->name );
                }

				(void) cut_global_variants (name);

				if ( file[ strlen( file ) - 1 ] != '/')   /* CTS 1107469 */
                {
					file = (char *) repalloc( file, strlen( file ) + strlen( name ) + 1 + 1 );
                    (void) strcat( file, "/" );
                    (void) strcat( file, name );
                }
                else
                {
					file = (char *) repalloc( file, strlen( file ) + strlen( name ) + 1 );
                    (void) strcat( file, name );
                }
            }
			else
			{
				if ( file[ strlen( file ) - 1 ] == '*' )            /* CTS 1107534 */
				{
					char temp_ext[ EXTENSIONSIZE ];

					FILL ( temp_ext, 0, EXTENSIONSIZE );
					file[ strlen( file ) - 1 ] = '\0';

					/* remove . from extension */
					if (  file[ strlen( file ) - 1 ] == '.' )
					{
						file[ strlen( file ) - 1 ] = '\0';
					}

					switch ( ctip->type )
					{
					case TT_DLL :
						(void) strcat( temp_ext, "." );
						(void) strcat( temp_ext, env_DLL_TYPE );
						break;
					case TT_LIB :
						(void) strcat( temp_ext, "." );
						(void) strcat( temp_ext, env_ARC_TYPE );
						break;
					case TT_PGM :
					case TT_SHM :
#ifdef WIN32
						(void) strcat( temp_ext, ".exe" );
#endif						
						break;
					case TT_REL :
					case TT_SHR :
					default :
						(void) strcat( temp_ext, "." );
						(void) strcat( temp_ext, get_extension (ctip->name) );
						break;
					}
       				file = (char *) repalloc( file, strlen( file ) + strlen( temp_ext ) + 1 );
					(void) strcat( file, temp_ext );
				}

			}
			DBG8 (( "explicit target file name '%s' \n", file ))
            
			rc = insert_file( &ctip->file, file, nodistrib );
			
			if ((rc >= 0) && (ctip->globalvariants))
			{
				char *variantfile;
				if ( create_globalvariant_filename(file, &variantfile, ctip->globalvariants) < 0 )
					rc = insert_file_to_filelist ( & ctip->variantfile, variantfile, 1 );
			}

            FREE_AND_SET_TO_NULL(file);
            if ( rc ) return( rc );
            DBG4 (( "coltrgopt: value for option '%s' \n", file ))
            token = strtok( NULL , token_sep );
        }
    }
    else
	if ( !strnicmp( option , TRGDSC_VMAKEOPTION, strlen(TRGDSC_VMAKEOPTION) ) )
    {

		token = strtok( trgline, token_sep );
        while( token )
        {
			if ( !strnicmp( token , TRGDSC_NOSTLIBOPTION, strlen(TRGDSC_NOSTLIBOPTION) ) )
				if (ctip->type == TT_DLL )
					ctip->stat.nostlib = 1;
				else
				{
					DBG0 (( "vmakeoption \"%s\" is only allowed in dld descriptions  \n", TRGDSC_NOSTLIBOPTION ))
					eprotwrite ( "vmakeoption \"? %s\" is only allowed in dld descriptions  \n", TRGDSC_NOSTLIBOPTION );
					return ( -2 );
				}
			else
			{
				DBG0 (( "unknown vmakeoption \"%s\"\n", token ))
				eprotwrite ( "unknown vmakeoption \"%s\"\n", token );
				return ( -2 );
			}
			token = strtok( NULL, token_sep );
		}
	}
	else
	if ( !strnicmp( option , TRGDSC_REPORT, strlen(TRGDSC_REPORT) ) )
    {
		token = strtok( trgline, token_sep );
        while( token )
        {
			if (! stricmp(token, VMAKE_REPORT_BEGIN))
			{
				ctip->report |= VMAKE_REPORT_MODE_BEGIN;
			}
			else
				if (! stricmp(token, VMAKE_REPORT_END))
				{
					ctip->report |= VMAKE_REPORT_MODE_END;
				}
				else
					if (! stricmp(token, VMAKE_REPORT_PERCENT))
					{
						ctip->report |= VMAKE_REPORT_MODE_PERCENT;
					}
					else
						if (! stricmp(token, VMAKE_REPORT_ALL))
						{
							ctip->report |= VMAKE_REPORT_MODE_ALL;
						}
						else
						{   
							DBG0 (( "ERROR: %s not allowed as option for %s\n", token, TRGDSC_REPORT ))
							return ( -1 );
						}
			token = strtok( NULL, token_sep );
		}
        rc = 0;
	}
	else
	if ( !strnicmp( option , TRGDSC_LINKWITH, strlen(TRGDSC_LINKWITH) ) )
    {
        struct trg_info * tipp;
        char            file [ FILENAMESIZE ];

        token = strtok( trgline, token_sep );
        while( token )
        {
            char            * tmp = NULL;

		    rc = substitute ( token , &tmp );
		    if ( rc ) return ( rc );
			if ( strchr(tmp, INTERNAL_VARPREFIX) )
			{
				rc = substitute_internal_variables(&tmp, ctip);
				if ( rc <= 0 )
				{
					DBG0 (( "ERROR: variant substitution error for %s (%d) \n", tmp, rc ))
					return ( rc );
				}
			}

            (void) sprintf( file , "-%9s%c%s%c%s", TRGDSC_LINKOPTION, DSC_DELIMITER, "lib", 
                    DSC_DELIMITER, tmp );
		    rc = collect_option ( file , file , ctip , &tipp );
		    if ( rc <= 0 )
	        {
			    DBG4 (( "collect: file error %d \n", rc ))
			    return ( -1 );
	        }
			rc = insert_dependency ( tipp , & ctip->options );
	        if ( rc < 0 )
	        {
		        DBG8 (( "coltrgopt: insert_dep error %d \n", rc ))
		        return ( -1 );
	        }
            token = strtok( NULL, token_sep );
            FREE_AND_SET_TO_NULL(tmp);
        }
        rc = 0;
    }
	else
	if ( !strnicmp( option , TRGDSC_RESPONSIBLE, strlen(TRGDSC_RESPONSIBLE) ) )
    {
        char            responsible [ FILENAMESIZE ];
		int res_len=0;
		
		for ( ; *trgline && ( *trgline != '#' ); trgline++ )
		{
			responsible [ res_len ] = *trgline;
			res_len++;
		};
		responsible [ res_len ] = '\0';
		if (ctip->responsible)
		{
			DBG0 (( "WARNING: duplicate responsibe option in %s \n", \
					ctip->name))
			FREE_AND_SET_TO_NULL(ctip->responsible);
		}
		ctip->responsible = (char*) palloc(strlen (responsible));
		strcpy (ctip->responsible, responsible);
        rc = 0;
    }
    else
    if ( 
        !strnicmp( option , TRGDSC_LINKOPTION, strlen(TRGDSC_LINKOPTION) ) || 
        !strnicmp( option , TRGDSC_TOOLOPTION, strlen(TRGDSC_TOOLOPTION) )
       )
    {
        struct trg_info * tipp;
        char            file [ FILENAMESIZE ];

        file[ FILENAMESIZE - 1 ] = '\0';
        token = strtok( trgline, token_sep );
        while( token )
        {
		    char			* tmp = NULL;
			substitute ( token , &tmp ); /* &gar PTS 1001820 */
			if ( strchr(tmp, INTERNAL_VARPREFIX) )
			{
				rc = substitute_internal_variables(&tmp, ctip);
				if ( rc <= 0 )
				{
					DBG0 (( "ERROR: variant substitution error for %s (%d) \n", tmp, rc ))
					return ( rc );
				}
			}

            (void) strcpy( file, "-" );
            (void) strncat( file, option, 10 );
            (void) strcat( file, "=" );
            (void) strncat( file, tmp, FILENAMESIZE - 13 );
            FREE_AND_SET_TO_NULL(tmp);
		    rc = collect_option ( file , file , ctip , &tipp );
		    if ( rc <= 0 )
	        {
			    DBG4 (( "collect: file error %d \n", rc ))
			    return ( -1 );
	        }
			rc = insert_dependency ( tipp , & ctip->options );
	        if ( rc < 0 )
	        {
		        DBG8 (( "coltrgopt: insert_dep error %d \n", rc ))
		        return ( -1 );
	        }
            token = strtok( NULL, token_sep );
        }
        rc = 0;
    }
    else
    if ( ! stricmp( option , TRGDSC_PROPAGATE ) )
    {
        struct env_info ** seip;
        struct env_info * eip;
        char            variable [ NAMESIZE ];
        char            value    [ FILENAMESIZE ];

		/* PTS 1103622 */
		if (ctip != NULL && (ctip->type != TT_COM))
		{
			for ( i=0 ; !isspace(*trgline) && *trgline && *trgline != DSC_DELIMITER ; 
					trgline++, i++ )
				if ( i < NAMESIZE-1 ) variable[ i ] = *trgline;
			variable[ i ] = '\0';

			rc = -1;
			for( seip = & ctip->propagates; *seip; seip=&(*seip)->next )
			{
				rc = strcmp( (*seip)->variable, variable );
				if ( rc >= 0 ) break ;
			}
			if ( rc != 0 )
			{
                char            * subst = NULL;

				eip = (struct env_info *) palloc ( sizeof(struct env_info) );
				if ( eip == NULL )
				{
					DBG0 (( "insufficient dynamic memory \n" ))
					eprotwrite ( "insufficient dynamic memory \n" );
					return ( -3 );
				}

				FILL (  eip , 0 , sizeof(struct env_info) );

				eip->variable = (char *) palloc( strlen( variable ) + 1 );

				(void) strcpy( eip->variable, variable );
				eip->next = *seip ;          /* &gar PTS 1001613 */
				*seip = eip;
				for ( ; isspace(*trgline) || *trgline == DSC_DELIMITER ; trgline++ );
				for ( i=0 ; !isspace(*trgline) && *trgline ; trgline++, i++ )
					if ( i < FILENAMESIZE - 1 ) value[ i ] = *trgline;
				value[ i ] = '\0';

				rc = substitute( value, &subst );
				if ( rc ) return( rc );
				if ( strchr(subst, INTERNAL_VARPREFIX) )
				{
					rc = substitute_internal_variables(&subst, ctip);
					if ( rc <= 0 )
					{
						DBG0 (( "ERROR: variant substitution error for %s (%d) \n", subst, rc ))
						return ( rc );
					}
				}
				eip->value = subst; 

				propagate_value (eip); /* &gar PTS 1002090-1 */
			}
			else
			{
				/* &gar PTS 1002090-2 if exists for this description -> WARNING */
				DBG0 (( "WARNING: duplicate propagate in %s for %s \n", \
					ctip->name, (*seip)->variable ))
				DBG0 (( "         value ignored - actual propagate is %s=%s\n", \
						(*seip)->variable, (*seip)->value ? (*seip)->value : "" ))
				eprotwrite ( "WARNING: duplicate propagate in %s for %s \n",ctip->name, (*seip)->variable );
				eprotwrite ( "         value ignored - actual propagate is %s=%s\n",(*seip)->variable, 
					(*seip)->value ? (*seip)->value : "" );
			}
		}
        rc = 0;
    }
	else
	/* &gar CTS 1000237 */
		if ( ! stricmp( option , TRGDSC_DISTRIB ) )
		{
			
			if (! opt_distrib)
			{
				DBG4 (( "coltrgopt: distrib option ignored\n" ))
				return ( 0 );
			}
			else
			{
				token = strtok( trgline, token_sep );
				while( token != NULL )
				{
    				char        * tmp = NULL;
					rc = substitute ( token , &tmp );
					if ( rc ) return( rc );
					if ( strchr(tmp, INTERNAL_VARPREFIX) )
					{
						rc = substitute_internal_variables(&tmp, ctip);
						if ( rc <= 0 )
						{
							DBG0 (( "ERROR: variant substitution error for %s (%d) \n", tmp, rc ))
							return ( rc );
						}
					}
					rc = insert_distributes( tmp );
                    FREE_AND_SET_TO_NULL(tmp);
					if ( rc < 0 )
					{
						DBG8 (( "coltrgopt: insert_distrib error %d \n", rc ))
						return ( -1 );
					}
					token = strtok( NULL, token_sep );
				}
				rc=0;
			}
		}
		else
		{
			DBG0 (( "coltrgopt: unknown target option '%s' \n", option ))
			eprotwrite ( "coltrgopt: unknown target option '%s' \n", option );
			rc = -1;
		}

	DBG4 (( "coltrgopt: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/

static  int     target_exists 
(   
    char            * target    ,
    char            version     ,
    unsigned        dep_type    ,
    struct trg_info * ctip      ,
    struct trg_info ** tipp     ,
    struct mod_com_info *mcip
)
{
	int             rc = -1 ;
	int			    i ;
	struct trg_info * tip ;
	struct trg_info * list [ 16 ];
    char            * layer = NULL;
    char            * puretarget;
	int				found_other_dep=0;
	struct trg_info * other_dep_tip ;
	int             desc_end;
	char            * caller_layer=NULL;

	DBG2 (( "trgexi: called \n" ))
	DBG3 (( "trgexi: target  '%s' \n", target  ))
	DBG3 (( "trgexi: version '%c' \n", version ))

	* tipp = 0 ;
    puretarget = ( target[0] == INDEX_SYMBOL ) ? skip_index( target ) : target ;

	i = 0 ;
	list[i++] = files ;
	list[i++] = command_list ;
	list[i++] = macros ;
	list[i++] = programs ;
	list[i++] = extra_list ;
	list[i++] = libraries ;
	list[i++] = javaprojects ;
	list[i++] = dynlinklibs ;
	list[i++] = projects ;
	list[i++] = relocs ;
    desc_end = i-1;
	list[i++] = includes ;
	list[i++] = modules ;

	if (ctip && ctip->layer)
			caller_layer = ctip->layer;

	for ( tip = list[--i] ; i ; tip = list[--i] )
	{
	    for ( ; tip ; tip = tip->next )
	    {
		    /* compare target names */
		    rc = strcmp ( tip->name , puretarget );
		    if ( rc > 0 ) break ;
		    if ( rc < 0 ) continue ;

	        /* same target can be present with different versions */
	        /* same target can be present with different dependencies */
            /* (include, no include)                                  */
	        /* if ( (tip->version == version  || tip->version == 'n' || version == ' ') )  CTS 1110368 */ 
			if ( tip->version == version  || tip->version == 'n' ) /* CTS 1110368 */ 
	        {
                /* it seems to be the same target */ 
                /* use enhanced checks */
                get_layer ( target , &layer );
                if ( ! layer && i > desc_end && caller_layer ) 
                {
                    /* get layer from caller if there's no layer */
                    layer = (char *) repalloc( layer, strlen( caller_layer )  + 1 );
                    (void) strcpy( layer , caller_layer );
                }
                if ( cmp_layer( layer , tip->layer ) )
                    /* not the same target */
                    rc = 1;
				else
					if (mcip && (tip->stat.deptype   != dep_type ))
					{
						if ( tip->other_dep_tip && ( tip->other_dep_tip->stat.deptype  == dep_type ))
						{ 
							rc  = 0;
							tip = tip->other_dep_tip;
						}
						else
						{
							rc = 1 ;  /* look for other file in the list */
							/* PTS 1104691 */
							found_other_dep = 1;
							other_dep_tip = tip;
						}
					}
			}
	        else
	            rc = 1 ;
            if ( rc == 0 ) 
                break;
	    }
	    if ( rc == 0 )
            break ;
	}

	if ( rc == 0 )
	{
	    DBG4 (( "trgexi: target exists '%s' %s \n",
				target , versname(version) ))
			/*
		if ( ( version = ' ' )  && (tip->version != 'n') )
			tip->version = ' ';
			*/
	    * tipp = tip ;
	}
	else
		if (found_other_dep)
		{
			DBG4 (( "trgexi: target exists with other deptype '%s' %s \n",
	 				target , versname(version) ))
			* tipp = other_dep_tip ;
			return (2);
		}

    FREE_AND_SET_TO_NULL(layer);

	DBG4 (( "trgexi: returning %d \n", ! rc ))
	/* PTS 1104596 */
	return ( rc ? 0:1);
}

/*==========================================================================*/

/*===========================================================================*/
/* find_target: look for target in the tip-list ( macros,includes, files)    */
/*===========================================================================*/

static  int     find_target 
(   
    int                 trgtype ,
    char                version ,
    unsigned            dep_type,
    char                * target,
    struct trg_info     * ctip  ,
    struct trg_info     ** tipp ,
    struct mod_com_info * mcip
)
{
	int             rc = -1 ;
	struct trg_info * tip ;
    char            * layer = NULL;
    char            * puretarget;
	char            * caller_layer=NULL;



    DBG2 (( "instrg: called \n" ))
	DBG3 (( "instrg: target  '%s' \n", target  ))
	DBG3 (( "instrg: version '%c' \n", version ))

	* tipp = NULL ;
    puretarget = ( target[0] == INDEX_SYMBOL ) ? skip_index( target ) : target ;

	if (ctip && ctip->layer)
			caller_layer = ctip->layer;

	switch ( trgtype )
	{
	case TT_OPTION :
	case TT_FILE :
			tip = files ;
			break ;
	case TT_INC :
			tip = includes ;
			break ;
	case TT_MEM :
	case TT_MOD :
			tip = modules ;
			break ;
	case TT_LIB :
			tip = libraries ;
			break ;
	case TT_JP :
			tip = javaprojects ;
			break ;
	case TT_REL :
	case TT_SHR :
			tip = relocs ;
			break ;
	case TT_DLL :
			tip = dynlinklibs ;
			break ;
	case TT_PGM :
	case TT_SHM :
			tip = programs ;
			break ;
	case TT_MAC :
			tip = macros ;
			break ;
    case TT_PRJ :
            tip = projects ;
            break;
	case TT_CMD :
			tip = command_list ;
			break ;
	case TT_EXTRA :
			tip = extra_list ;
			break ;
	default :
			DBG0 (( "findtrg: illegal target type %d \n", trgtype ))
			eprotwrite ( "findtrg: illegal target type %d \n", trgtype );
			return ( -1 );
	}

	for ( ; tip ; tip = tip->next )
	{
	    /* always collect a command, file or option as a new target */
	    if ( (trgtype == TT_CMD) || (trgtype == TT_FILE) || (trgtype == TT_OPTION)  ) 
            break;

	    /* compare target names */
	    rc = strcmp ( tip->name , puretarget );
	    if ( rc > 0 ) break ;
	    if ( rc < 0 ) continue ;

	    /* same target can be present with different versions */
	    /* same target can be present with different dependencies */
        /* (include, no include)                                  */
		if ( tip->version == version )
		{
            /* it seems to be the same target */
            /* use enhanced checks */
            (void) get_layer ( target , &layer );
            if ( ! layer && trgtype >= TT_LIB && caller_layer ) 
            {
                /* get layer from caller if there's no layer */
                layer = (char *) repalloc( layer, strlen( caller_layer ) + 1 );
                (void) strcpy( layer , caller_layer );
            }
            if ( cmp_layer( layer , tip->layer ) )
                /* not the same target */
                rc = 1 ;
            if ( tip->stat.deptype   != dep_type  )
                rc = 1 ;
		}
		else
		{
	        rc = 1 ;
		}

	    if ( rc == 0 ) break ;
	}

	if ( rc )
	{
	    DBG4 (( "findtrg: target not found '%s' \n", target ))
	    return ( 0 );
	}

	* tipp = tip ;

    FREE_AND_SET_TO_NULL(layer);

	DBG4 (( "findtrg: target found '%s' \n", tip->name ))
	return ( 1 );
}

/*==========================================================================*/


static  int     insert_target 
(   
    int                 trgtype ,
    char                version ,
    unsigned            dep_type,
    char                * target,
    struct trg_info     * ctip  ,
    struct trg_info     ** tipp ,
    struct mod_com_info *mcip
)
{
	int                 ec , rc = -1;
	struct trg_info     ** stipp , * tip = NULL;
    char                * layer = NULL;
    char                * puretarget;
	char                * caller_layer = NULL;

	DBG2 (( "instrg: called \n" ))
	DBG3 (( "instrg: target  '%s' \n", target  ))
	DBG3 (( "instrg: version '%c' \n", version ))


	* tipp = NULL ;
    puretarget = ( (target[0] == INDEX_SYMBOL) && (trgtype != TT_DIRLIST) )  ? skip_index( target ) : target ;

	if (ctip && ctip->layer)
			caller_layer = ctip->layer;

    if ( ! opt_nodependencies ) 
    {
		switch ( trgtype )
		{
		case TT_OPTION :
		case TT_FILE :
				stipp = &files ;
				break ;
		case TT_INC :
				stipp = &includes ;
				break ;
		case TT_MEM :
		case TT_MOD :
				stipp = &modules ;
				break ;
		case TT_LIB :
				stipp = &libraries ;
				break ;
		case TT_JP :
				stipp = &javaprojects ;
				break ;
		case TT_REL :
		case TT_SHR :
				stipp = &relocs ;
				break ;
		case TT_DLL :
				stipp = &dynlinklibs ;
				break ;
		case TT_PGM :
		case TT_SHM :
				stipp = &programs ;
				break ;
		case TT_MAC :
				stipp = &macros ;
				break ;
		case TT_PRJ :
				stipp = &projects ;
				break ;
		case TT_CMD :
				stipp = &command_list ;
				break ;
		case TT_EXTRA :
				stipp = &extra_list ;
				break ;
		case TT_DIRLIST:
				stipp = &dirlist ;
				break ;
		default :
				DBG0 (( "instrg: illegal target type %d \n", trgtype ))
				eprotwrite ( "instrg: illegal target type %d \n", trgtype );
				return ( -1 );
		}

        for ( ; *stipp != NULL ; stipp =  &(*stipp)->next )
	    {
	        /* always collect a command, file or option as a new target */
	        if ( (trgtype == TT_CMD) || (trgtype == TT_FILE) || (trgtype == TT_OPTION)  ) 
                break;

	        /* compare target names */
	        rc = strcmp ( (*stipp)->name , puretarget );
	        if ( rc > 0 ) break ;
	        if ( rc < 0 ) continue ;

	        /* same target can be present with different versions */
	        /* same target can be present with different dependencies */
            /* (include, no include)                                  */
		    if ( (*stipp)->version == version )
		    {
                /* it seems to be the same target */
                /* use enhanced checks */
                (void) get_layer ( target , &layer );
                if ( ! layer && caller_layer ) 
                {
                    /* get layer from caller if there's no layer */
                    layer = (char *) repalloc( layer, strlen( caller_layer ) + 1 );
                    (void) strcpy( layer , caller_layer );
                }
                if ( cmp_layer( layer , (*stipp)->layer ) ) 
                    /* not the same target */
                    rc = 1 ;
			    if ( (*stipp)->stat.deptype	 != dep_type )
                    rc = 1;
		    }
		    else
		    {
	            rc = 1 ;
		    }

	        if ( rc == 0 ) break ;
	    }
    }
    else
        rc = -1;

    if ( rc == 0 )
	{
	    DBG8 (( "instrg: target found '%s' \n", (*stipp)->name ))
	    /*
		TT_MOD overrides TT_MEM, because '.o' file has to be made.
	    */
	    if ( trgtype == TT_MOD )
	    {
		    (*stipp)->type = TT_MOD ;
		    DBG7 (( "instrg: changed to TT_MOD '%s' \n", (*stipp)->name ))
	    }
	}
	else
	{
	    tip = (struct trg_info *) palloc ( sizeof(struct trg_info) );
	    if ( tip == NULL )
	    {
		    DBG0 (( "insufficient dynamic memory \n" ))
			eprotwrite ( "insufficient dynamic memory \n" );
		    return ( -3 );
	    }

		FILL (  tip , 0 , sizeof(struct trg_info) );

	    if ( (trgtype == TT_CMD) || (trgtype == TT_FILE) || (trgtype == TT_OPTION) || (trgtype == TT_DIRLIST)) 
        {
            tip->name = (char *) palloc( strlen( target ) + 1 );
            (void) strcpy ( tip->name , target );
        }
        else
        {
            tip->name = (char *) palloc( strlen( puretarget ) + 1 );
            (void) strcpy ( tip->name , puretarget );
        }

        tip->stat.deptype = dep_type;
	    tip->type = trgtype ;
	    tip->version = version ;
	    tip->language = ' ' ;
	    /* use default VMAKE_OPTION, until overriden */
		tip->stat.level_i = vopt_level_i ;
	    tip->stat.level_s = vopt_level_s ;
	    tip->stat.level_p = vopt_level_p ;
	    tip->stat.level_c = vopt_level_c ;
	    tip->stat.level_o = vopt_level_o ;
		
        if ( ! opt_nodependencies ) 
        { 
        	tip->next = *stipp ;
            *stipp = tip ;
        }

	    DBG7 (( "instrg: inserted '%s' \n", tip->name ))
	    rc = 1 ;
	}

    if ( ( ! opt_nodependencies ) || ( ! tip ) )
        * tipp = *stipp ;
    else
        * tipp = tip;

	if ( ctip )
	{
	    ec = insert_dependency ( ctip , & (*tipp)->callers );
	    if ( ec < 0 ) return ( ec );
	}

	/* handling for link descriptions */
	
	if ( opt_uncond && (trgtype > TT_MOD) && 
         ( ! opt_nodep || ! ctip  ||  ! ctip->act_caller )
	   )
	{
	    (*tipp)->stat.uncond = 1 ;
	}

	
	/*
	 *  Lint and sizeof are to be applied to all modules unconditionally.
	 */
	if ( (opt_lint || opt_sizeof) &&
	     (((*tipp)->type == TT_MOD) || ((*tipp)->type == TT_MEM)) )
	{
	    (*tipp)->stat.uncond = 1 ;
	}

	/*
	 *  ctip == NULL means that target was given on command line.
	 *  If macros have to be made unconditionally,
	 *  this applies to all their dependencies too.
	 *  'opt_sizeof' includes uncond for named include modules too.
	 */
	/* not uncond if the calling mac descriiption have a caller (PTS 1110170) */
    if ( (! ctip || ((ctip->type == TT_MAC) && !ctip->callers)) &&
	     (opt_nameduncond || opt_sizeof) )
	{
		(*tipp)->stat.uncond = 1 ;
		if ( opt_sizeof && (*tipp)->type == TT_INC )
		{
			(*tipp)->stat.level_i = 2 ;
		}
	}

    /* PTS 1103486 -> active caller */
	(*tipp)->act_caller = ctip; 

    FREE_AND_SET_TO_NULL(layer);

	DBG4 (( "instrg: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/

static  int     move_target 
(
    struct trg_info     * tip  ,
    struct trg_info     ** srclist ,
	struct trg_info     ** destlist
)
{
	struct trg_info     ** stipp;
	struct trg_info     * prevtip = NULL;
	char                error_msg[40];
	int                 rc;

	error_msg[0]='\0';
	if ( *srclist == includes )
		strcpy(error_msg, " from includes" );
	else
		if ( *srclist == modules )
			strcpy(error_msg, " from modules" );

	if ( *destlist == includes )
		strcat(error_msg, " to includes" );
	else
		if ( *destlist == modules )
			strcat(error_msg, " to modules" );

	/* remove from first source list */
	for (stipp = srclist; *stipp != NULL ; stipp =  &(*stipp)->next )
	{
		if (*stipp == tip)
			break;
		prevtip = *stipp;
	}
	
	if ( *stipp != tip )
		DBG0 (( "Internal Warning(movetrg): Can't move target %s%s\n", tip->name, error_msg ))
	else
	{
		if (prevtip)
			prevtip->next = (*stipp)->next;
		else
			*stipp = (*stipp)->next;
	}


	/* insert to destination list */
	prevtip = NULL;
	for (stipp = destlist; *stipp != NULL ; stipp =  &(*stipp)->next )
	{
		/* compare target names */
		if ( *stipp == tip )
		{
			DBG8 (( "movetrg: target already exists in the list \n" ))
			return 0;
		}

		rc = strcmp ( (*stipp)->name ,tip->name );
		if ( rc > 0 ) break ;
		prevtip = *stipp;
		if ( rc == 0 )
		{
			/* same target can be present with different versions */
			/* same target can be present with different layers */
			/* (include, no include)                                  */
			if ( ((*stipp)->version == tip->version ) && 
				! cmp_layer( tip->layer , (*stipp)->layer ) &&
				((*stipp)->stat.deptype	 == tip->stat.deptype) )
			{
				DBG4 (( "Internal Warning(movetrg): Found %s in dest list with other pointer\n", tip->name ))
				DBG4 (( "                           while moving%s (ignored)\n",  error_msg ))
			}
		}
	} 

	tip->next = *stipp;

	if (prevtip)
		prevtip->next = tip;
	else
		*stipp = tip;

	return 1;

}

/*==========================================================================*/


static  int     insert_description 
(   
    struct dsc_info         * descp     ,
    struct dsc_info         ** listpp   ,
    struct trg_info			*tip,
	int                     mci_type
)
{
	int                     rc ;
	struct dsc_info         ** sipp , * sip ;
	char                    * descriptor = NULL;
	
	DBG2 (( "insdsc: called \n" ))
	if ( strchr (descp->descriptor, INTERNAL_VARPREFIX ) )
	{
		descriptor = (char *) palloc( strlen( descp->descriptor ) + 1 );
		if ( descriptor == NULL )
		{
			DBG0 (( "insufficient dynamic memory \n" ))
			eprotwrite ( "insufficient dynamic memory \n" );
			return ( - 3 );
		}

		strcpy (descriptor, descp->descriptor);
		if ( substitute_internal_variables (&descriptor, tip ) < 0 )
			return -3;
	}
	else
		descriptor = descp->descriptor;

	rc = - 1 ;
	for ( sipp = listpp ; (*sipp) != NULL ; sipp = & (*sipp)->next )
	{
		rc = strcmp ( (*sipp)->descriptor, descriptor );
		if ( rc >= 0 ) break ;
	}

	if  ( rc == 0 )
	{
		if (mci_type == MCI_GLOBALS)
		{
			DBG3 (( "insdsc: descriptor '%s' not overwrite by globals definitions\n", descriptor  ))
			return 0; 
		}
		DBG8 (( "insdsc: descriptor found '%s' \n",
							(*sipp)->descriptor ))
        FREE_AND_SET_TO_NULL((*sipp)->value);
	}
	else
	{
		sip = (struct dsc_info *) palloc ( sizeof(struct dsc_info) );
		if ( sip == NULL )
		{
			DBG0 (( "insufficient dynamic memory \n" ))
			eprotwrite ( "insufficient dynamic memory \n" );
			return ( - 3 );
		}

		FILL (  sip , 0 , sizeof(struct dsc_info) );

        sip->descriptor = (char *) palloc( strlen( descriptor ) + 1 );
        (void) strcpy ( sip->descriptor , descriptor );
		
		sip->next = * sipp ;
		* sipp = sip ;

		DBG7 (( "insdsc: inserted '%s' \n", descriptor ))
		rc = 1 ;
	}

    (*sipp)->value = (char *) palloc( strlen( descp->value ) + 1 );
    (void) strcpy ( (*sipp)->value , descp->value );
	if ( substitute_internal_variables (&((*sipp)->value), tip ) < 0 )
			return -3;

	DBG7 (( "insdsc: value '%s' \n", (*sipp)->value ))

	DBG4 (( "insdsc: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/


global	int     insert_dependency
(   
    struct trg_info         *tip    ,
    struct dep_info         **listpp
)
{
	int                     rc = 0 ;
	struct dep_info         **sdipp ;
	struct dep_info         *dip ;

    DBG2 (( "insdep: called \n" ))
	DBG3 (( "insdep: target '%s' \n", tip ? tip->name : "NULL" ))

	if ( tip == NULL )
	{
		DBG4 (( "insdep: target is NULL \n" ))
		return ( 0 );
	}

	for ( sdipp = listpp ; *sdipp ; sdipp = &(*sdipp)->next );

	/* end of dependency list */

	dip = (struct dep_info*) palloc ( sizeof(struct dep_info) );
	if ( dip == NULL )
	{
		DBG0 (( "insufficient dynamic memory \n" ))
		eprotwrite ( "insufficient dynamic memory \n" );
		return ( - 3 );
	}

	FILL (  dip , 0 , sizeof(struct dep_info) );
	dip->target = tip ;

	*sdipp = dip ;

	DBG7 (( "insdep: appended '%s' \n", dip->target->name ))
	rc = 1 ;

	DBG4 (( "insdep: returning %d \n", rc ))
	return ( rc );
}

/*==========================================================================*/


static	int     insert_demands
(   
    struct trg_info         * tip    ,	   /* pointer to insert in demands */
    char                    * directory ,  /* directory name for copy */
    struct dem_info         ** listpp	   /* pointer to actual demands */
)
{
	int                     rc=0 ;
	struct dep_info         **sdipp ;
	struct dem_info         **sdmipp ;
	struct dep_info         *dip ;
	struct dem_info         *dmip ;

	if ( ! directory )
	{
		directory = (char *) palloc( 1 );
		directory[ 0 ] = '\0';
	}

	DBG2 (( "insdem: called \n" ))
	DBG3 (( "insdem: target '%s' \n", tip ? tip->name : "NULL" ))
	DBG3 (( "insdem: directory '%s' \n", directory ))

	if ( tip == NULL )
	{
		DBG4 (( "insdem: target is NULL \n" ))
		return ( 0 );
	}

    rc = -1;
    /* search for the directory in the mcip->demands->directory*/
	for ( sdmipp = listpp ; *sdmipp ; sdmipp = &(*sdmipp)->next )
    {
#ifdef WIN32
        rc = stricmp( (*sdmipp)->directory , directory );
#else
        rc = strcmp( (*sdmipp)->directory , directory );
#endif
        if ( rc == 0 ) break;
    }

	/* end of dem_info list or directory found */
	/* if directory entry in the list not exists -> create and append */
    if ( rc != 0 )
    {
	    DBG7 (( "insdem appended '%s' \n", directory ))

	    dmip = (struct dem_info*) palloc ( sizeof(struct dem_info) );
	    if ( dmip == NULL )
	    {
		    DBG0 (( "insufficient dynamic memory \n" ))
			eprotwrite ( "insufficient dynamic memory \n" );
		    return ( - 3 );
	    }
    	*sdmipp = dmip ;

    	FILL (  dmip , 0 , sizeof(struct dem_info) );

		dmip->directory = (char *) palloc( strlen( directory ) + 1 );
        (void) strcpy( dmip->directory , directory );
    }
    else
        dmip = (*sdmipp);

    /* to the end of the list (dep_info) of the directory */
	for ( sdipp = &dmip->list ; *sdipp ; sdipp = &(*sdipp)->next )
	{
		if ( (*sdipp)->target==tip )
		{
			DBG4 (( "insdem: %s already in list of demands\n", tip->name ))
			rc = 0;
			return ( rc );
		}
	}

    dip = (struct dep_info*) palloc ( sizeof(struct dep_info) );
	if ( dip == NULL )
	{
		DBG0 (( "insufficient dynamic memory \n" ))
		eprotwrite ( "insufficient dynamic memory \n" );
		return ( - 3 );
	}
    FILL (  dip , 0 , sizeof(struct dep_info) );
    dip->target = tip ;
    *sdipp = dip ;

	DBG7 (( "insdem appended '%s' \n", dip->target->name ))
	rc = 1 ;

	DBG4 (( "insdem: returning %d \n", rc ))
	return ( rc );
}
/*==========================================================*/

static int insert_parentlinkoptions
(   
    struct trg_info         * tip    ,	   /* pointer to insert linkoptions and parentoptions */
	struct opt_info         * parentlinkoptions,
	int                       save_also_as_linkoption
)
{
	int rc = 0;

	DBG2 (( "insplo: called \n" ))

	if ( parentlinkoptions ) 
	{
		if ( tip->type != TT_MAC ) /* mac ignored */
		{
			if (  save_also_as_linkoption && 
				(( tip->type == TT_PGM ) || ( tip->type == TT_DLL ) || ( tip->type == TT_SHM )) )
			{ 
				rc = insert_options (&(tip->linkoptions), parentlinkoptions );	
				if ( rc < 0 )
				{
					DBG0 (( "coltrg: linkoption error %d \n", rc ))
					return ( rc );
				}
			}

			if (( tip->type != TT_PGM ) && ( tip->type != TT_SHM ) )
			{
				rc = insert_options (&(tip->parentlinkoptions), parentlinkoptions);
				if ( rc < 0 )
				{
					DBG0 (( "coltrg: parentlinkoption error %d \n", rc ))
					return ( rc );
				}
			}
		}
		rc = 1;
	}

	DBG4 (( "insplo: returning %d \n", rc ))
	return (rc);

}

/*==========================================================================*/
/* both list must be sorted ! */
static  int     insert_options 
(
	struct opt_info         ** trg_options,
	struct opt_info         * src_options
)
{
	struct opt_info         * soip;
	struct opt_info         ** toipp, *toip;
	
	int rc; 

	DBG2 (( "insopt: called \n" ))
	if ( ! src_options )
	{	return (0);	}

	soip = src_options;

	for ( toipp = trg_options ; soip ; toipp =  &(*toipp)->next )
	{
	    /* compare options */
		if ( *toipp )
			rc = strcmp ( (*toipp)->option, soip->option );
		else /* append at the end */
			rc = 1;
		/* option already exists */
		if ( rc < 0 ) continue ;

		if ( rc > 0 )
		{
			/* look for option in global option list */
			toip = (struct opt_info *) palloc ( sizeof(struct opt_info) );
			if ( toip == NULL )
			{
				DBG0 (( "insufficient dynamic memory \n" ))
				eprotwrite ( "insufficient dynamic memory \n" );
				return ( -1 );
			}
			toip->next = *toipp;		
			toip->option = soip->option;
			*toipp = toip;
		}
		soip = soip->next;
	}

	DBG4 (( "insopt: returning %d \n", 1 ))
	return ( 1 );
}

/*==========================================================================*/

static	int     remove_dependency
(   
    char                * target ,
	struct trg_info     * tip,
	int					dep_type
    /* struct dep_info     **listpp */
)
{
	int                     rc ;
	struct dep_info         **sdipp ;
	struct dep_info         *dip ;
	/* PTS 1104466 */
	char    * layer = NULL;	
    char    name [ NAMESIZE ];	
	char    * ptarget;

	DBG2 (( "remdep: called \n" ))
	DBG3 (( "remdep: target '%s' \n", target ))

	rc = 0 ;

	/* PTS 1104466 */
	/* new conventions :layer/name */
	if ( target[ 0 ] == INDEX_SYMBOL )
		if ( ! is_valid_sourcename( target, &layer, name ) )
		{
			DBG0 (( "remdep: '%s' is not valid source name in dependencies of '%s'\n", target, tip->name   ))
			eprotwrite ( "remdep: '%s' is not valid source name in dependencies of '%s'\n", target, tip->name   );
			return -3;
		}
		else
		{
			ptarget = name;
		}
	else
	{
		ptarget = target;
	}

	for ( sdipp = &tip->dependencies ; *sdipp ; )
	{
	    dip = *sdipp ;
	    if ( ! strcmp ( dip->target->name , ptarget ) )
	    {
			/* PTS 1104466 */ 
			/* same filename but different layer */
			if ( (layer && cmp_layer ( dip->target->layer , layer )) ||
				( dip->target->stat.deptype != dep_type ) ) 
			{
				sdipp = &dip->next ;
			}
			else
			{
				*sdipp = dip->next ;
				DBG7 (( "remdep: removing '%s' from dependencies of '%s'\n", dip->target->name, tip->name ))
				remove_from_callers(tip, dip->target);
				FREE_AND_SET_TO_NULL(dip);
				rc = 1 ;
			}
	    }
	    else
	    {
		    sdipp = &dip->next ;
	    }
	}

	if ( ! rc )
	{
		DBG0 (( "remdep: dependency of '%s' not found: '%s' \n", tip->name, target ))
		eprotwrite ( "remdep: dependency of '%s' not found: '%s' \n", tip->name, target );
	}

    FREE_AND_SET_TO_NULL(layer);

	DBG4 (( "remdep: returning %d \n", rc ))
	return ( rc );
}

/*==========================================================================*/

static	int     remove_demands
(   
    char                * target ,
    char                * directory ,
    struct dem_info     ** listpp
)
{
	int                     rc ;
	struct dep_info         **sdipp ;
	struct dem_info         **sdmipp ;
	struct dep_info         *dip ;

	DBG2 (( "remdem: called \n" ))
	DBG3 (( "remdem: target '%s' \n", target ))
	DBG3 (( "remdem: directory '%s' \n", directory ))

    rc = -1;
	for ( sdmipp = listpp ; *sdmipp ; sdmipp = &(*sdmipp)->next )
    {
        rc = strcmp( (*sdmipp)->directory , directory );
        if ( rc == 0 ) break;
    }
    if ( rc ) 
    {
        DBG0 (( "target directory '%s' for demands don't exist \n ", directory ))
		DBG0 (( "remdep: dependency not found '%s' \n", target ))
		eprotwrite ( "target directory '%s' for demands don't exist \n ", directory );
		eprotwrite ( "remdep: dependency not found '%s' \n", target );
        return ( ! rc );
    }

    rc = 0 ;
	for ( sdipp = &(*sdmipp)->list ; *sdipp ; )
	{
	    dip = *sdipp ;
	    if ( ! strcmp ( dip->target->name , target ) )
	    {
		    *sdipp = dip->next ;
		    DBG7 (( "remdep: removing '%s' \n", dip->target->name ))
		    
		    FREE_AND_SET_TO_NULL(dip);
		    rc = 1 ;
	    }
	    else
	    {
		    sdipp = &dip->next ;
	    }
	}

	if ( ! rc )
	{
		DBG0 (( "remdep: dependency not found '%s' \n", target ))
		eprotwrite ( "remdep: dependency not found '%s' \n", target );
	}

	DBG4 (( "remdem: returning %d \n", rc ))
	return ( rc );
}

/*==========================================================================*/

static	int     clear_dependencies 
(   
    struct trg_info         *tip,
	int                 dep_type
)
{
	struct dep_info         *dip ;
	struct dep_info         **sdipp ;
	

	DBG2 (( "clrdep: called \n" ))

	for ( sdipp = &tip->dependencies ; *sdipp ; )
	{
	    dip = *sdipp ;
		if ((dep_type == DT_ALLDEP) || ( dip->target->stat.deptype == dep_type ))
		{
			*sdipp = dip->next ;
			DBG7 (( "clrdep: removing '%s' from dependencies\n", dip->target->name ))
			remove_from_callers(tip, dip->target);
			FREE_AND_SET_TO_NULL(dip);
		}
		else
		{
			sdipp = &dip->next ;
		}		
	}

	DBG4 (( "clrdep: returning %d \n", 0 ))
	return ( 0 );
}

/*==========================================================================*/

static	int  remove_from_callers
(   
	struct trg_info         *tip,
	struct trg_info         *deltip
)
{
	struct dep_info         *dip ;
	struct dep_info         **sdipp ;
	


	DBG2 (( "rmclls called \n" ))

	for ( sdipp = &(deltip->callers) ; *sdipp ; )
	{
		dip = *sdipp ;
		if (dip->target == tip)
		{
			*sdipp = dip->next ;
			DBG7 (( "clrdep: removing '%s' from dependencies\n", dip->target->name ))
			FREE_AND_SET_TO_NULL(dip);
		}
		else
		{
			sdipp = &dip->next ;
		}		
	}
	
	return (0);
}


/*==========================================================================*/

static	int     clear_demands 
(   
    char                * directory ,
    struct dem_info     ** listpp 
)
{
    int     rc ;
    struct dep_info         *dip ;
	struct dem_info         **sdmipp ;

	DBG2 (( "clrdep: called \n" ))

    rc = -1;
	for ( sdmipp = listpp ; *sdmipp ; sdmipp = &(*sdmipp)->next )
    {
        rc = strcmp( (*sdmipp)->directory , directory );
        if ( rc == 0 ) break;
    }
    if ( rc ) 
    {
        DBG4 (( "target directory '%s' for demands don't exist \n ", directory ))
    }
    else
        while ( (*sdmipp)->list )
	    {
		    dip = (*sdmipp)->list ;
		    (*sdmipp)->list = dip->next ;
		    DBG7 (( "clrdem: removing '%s' \n", dip->target->name ))
		    FREE_AND_SET_TO_NULL(dip);
	    }

	DBG4 (( "clrdem: returning %d \n", 0 ))
	return ( 0 );
}

/*==========================================================================*/

global  int     get_com_info_list 
(   
    char                         * layer         ,
    char                         version         ,
	char                         * spec_comfile  , 
    struct mod_com_info_list     ** rmcilpp	
)
{
	struct mod_com_info_list     *mcilp;
	int rc =0;
	
	mcilp = (struct mod_com_info_list *) palloc ( sizeof(struct mod_com_info_list) );

	mcilp->mci_type=MCI_LAYER;
	
	*rmcilpp = mcilp;
	rc = get_com_info(layer,version, spec_comfile, &(mcilp->mcip));
	if ( rc > 0 )
	{
		mcilp->next=(struct mod_com_info_list *) palloc ( sizeof(struct mod_com_info_list) );
		mcilp->next->mci_type=MCI_GLOBALS;
		rc+=get_com_info ( env_VMAKE_GLOBALS, version, NULL, &(mcilp->next->mcip));
		mcilp->next->next = (struct mod_com_info_list*) 0;
	}
	return (rc);

}


/*==========================================================================*/

global  int     get_com_info 
(   
    char                    * layer         ,
    char                    version         ,
	char                    * comfile       ,
    struct mod_com_info     ** rmcipp	
)
{
	int                     rc = 0 ;
	int                     errcnt ;
	int                     lgt ;
	struct com_info         * cip ;
	struct com_info         ** scipp ;
	VFILE                   * fp ;
	struct mod_com_info     mci ;
	struct mod_com_info     * mcip ;
	struct mod_com_info     ** mcipp ;
	struct list_info	    * lip ;
	struct list_info	    * head = 0 ;
	struct list_info	    ** tailp = & head ;
	char                    name [ FILENAMESIZE ] ;
	char                    list [ FILENAMESIZE ] ;
	char                    line [ LINESIZE ] ;
	char                    * modlayer = NULL ;
	char                    * mci_defaultlayer = NULL ;
	char			        tmpvers ;
	char			        found_vers ;
	char                    * search_string;
    extern char     concomversion;
	time_t			filedate=0;
	struct trg_info         * dummy_tip;

    assert( layer );
    
    DBG2 (( "getcominf: called \n" ))
	DBG3 (( "getcominf: layer   '%s' \n", layer ))
	DBG3 (( "getcominf: version '%c' \n", version ))

	* rmcipp = 0 ;

	tmpvers = ((version == ' ') || (version == 'n')) ? getdefvers( vmakeversion ) : version ; 
	
	mci_defaultlayer = (char *) palloc( strlen( layer ) + 1 );
	(void) strcpy( mci_defaultlayer , layer ); 

	if (0)
	{
		valid_bit_arr	vbits ;
		char            * comfilepath = NULL;
		int			    status ;
		
		VALID_BIT_FILL ( vbits );     

		rc = get_file ( comfile , DIR_SRC , EMPTY , EMPTY ,
			FK_SOURCE | FK_LEVEL , vbits ,
			&comfilepath , &filedate , & status, opt_nodircache );
		
		if ( rc )
		{
            strcpy (list, comfilepath);
			FREE_AND_SET_TO_NULL (comfile);
		}
		else
		{
            DBG0 (( "cannot find specified comfile '%s' \n", comfile ))
			eprotwrite ( "cannot find specified comfile '%s' \n", comfile );
			return ( - 2 );
		}
	}
	else
	{
		/*
		*  Find the required compile info in the com_lists queue,
		*  or find the position where to insert it.
		*/
		rc = - 1 ;
		search_string = comfile ? comfile : layer;

        for ( scipp = & com_lists ; *scipp ; scipp = & (*scipp)->next )
		{
			rc = cmp_layer ( (*scipp)->layer , search_string );
			if ( rc >= 0 ) break ;
		}

		if ( rc == 0 )
		{
			cip = * scipp ;
			switch ( tmpvers )
			{
			case 'f' :
	    		mcip = cip->com_fast ;
				break ;
			case 'q' :
    			mcip = cip->com_quick ;
	    		break ;
			case 's' :
    			mcip = cip->com_slow ;
	    		break ;
			default :
    			mcip = 0 ;
			}
			if ( mcip )
			{
				DBG8 (( "getcominf: found info '%s' version '%c' \n",
							(*scipp)->layer , tmpvers ))
				*rmcipp = mcip ;
				DBG8 (( "getcominf: returning %d \n", 1 ))
				return ( 1 );
			}
		}
		else
		{
			cip = 0 ;
		}

		/*
		*  This layers/versions com info has not yet been collected
		*  The layer's compile info is contained in a description file
		*  '<layer>.com'. Try to find the file.
		*/

		if (comfile)
		{
			valid_bit_arr	vbits ;
			char            * comfilepath = NULL;
			int			    status ;
			char            * comfile_layer = NULL;
			
			VALID_BIT_FILL ( vbits );     

			get_layer(comfile, &comfile_layer);

			rc = get_file ( skip_index(comfile) , DIR_SRC , comfile_layer, EMPTY , 
				FK_SOURCE | FK_LEVEL , vbits ,
				&comfilepath , &filedate , & status, opt_nodircache );
			
			if ( rc )
			{
				strcpy(list, comfilepath);
				FREE_AND_SET_TO_NULL (comfilepath);
			}
			else
			{
				DBG0 (( "cannot find specified comfile '%s' \n", comfile ))
				eprotwrite ( "cannot find specified comfile '%s' \n", comfile );
				return ( - 2 );
			}
			found_vers = 'f';
		}
		else
		{
			if ( strrchr( layer, '/' ) )
			{
				/* make a qualified description name */
    			(void) sprintf ( name , "::%s.%s", layer , COM_LST );
			}
			else
    			(void) sprintf ( name , "%s.%s", layer , COM_LST );
			
			DBG8 (( "getcominf: search compile list '%s' version '%c' \n",
									name , tmpvers ))

			rc = get_dsc_file ( name , tmpvers , list , & found_vers, &filedate, 1 );
			if ( rc <= 0 )
			{
				if ( rc == 0 )
					DBG4 (( "getcominf: compile list not found     '%s' \n", name ))
				else 
					DBG8 (( "getcominf: get_dsc_file error %d \n", rc ))
				return ( rc );
			}

			DBG4 (( "getcominf: compile list found         '%s' version '%c' \n",
							name , found_vers ))
		}

		if ( opt_nodependencies ) return( 1 );

		/*
		*  The description file might not have the requested version.
		*  This is because 'get_dsc_file' searches for other version's
		*  descriptions if the requested are not present.
		*/
		if ( cip )
		{
			mcip = 0 ;
			switch ( found_vers )
			{
			case 'f' :
    			mcip = cip->com_fast ;
	    		if ( tmpvers == 'f' ) break ;
    			/* wanted quick or slow but did not find it */
				cip->com_quick = cip->com_fast ;
	    		/*FALLTHROUGH*/
			case 'q' :
				mcip = cip->com_quick ;
				if ( tmpvers == 'q' ) break ;
				/* wanted slow but did not find it */
				cip->com_slow = cip->com_quick ;
				/*FALLTHROUGH*/
			case 's' :
    			mcip = cip->com_slow ;
			case 'n' :
			case ' ' :
				switch ( tmpvers )
				{
				case 'f' :
					mcip = cip->com_fast ;
	    			break ;
				case 'q' :
					mcip = cip->com_quick ;
					break ;
				case 's' :
        			mcip = cip->com_slow ;
				}
			}

			if ( mcip )
			{
				DBG8 (( "getcominf: found info '%s' version '%c' \n",
							(*scipp)->layer , found_vers ))
				*rmcipp = mcip ;
				DBG8 (( "getcominf: returning %d \n", 1 ))
				return ( 1 );
			}
		}
		else
		{
			/*
			*  Allocate a new com_info struct.
			*/
			cip = (struct com_info *) palloc ( sizeof(struct com_info) );
			if ( cip == NULL )
			{
				DBG0 (( "insufficient dynamic memory \n" ))
				eprotwrite ( "insufficient dynamic memory \n" );
				return ( - 3 );
			}

			FILL ( cip , 0 , sizeof(struct com_info) );
			cip->layer = (char *) palloc( strlen( search_string ) + 1 );
			(void) strcpy( cip->layer, search_string );

			/*
			*  Insert the com_info into the com_lists queue
			*  at the position found above.
			*/
			cip->next = * scipp ;
			* scipp = cip ;
			DBG7 (( "getcominf: inserted '%s' \n", cip->layer ))
		}

		cip->date = filedate;

		/*
		*  Put the information to the found version's list.
		*/
		switch ( found_vers )
		{
		case 'f' :
			mcipp = & cip->com_fast ;
			break ;
		case 'q' :
			mcipp = & cip->com_quick ;
			break ;
		case 's' :
			mcipp = & cip->com_slow ;
			break ;
		case ' ' :
		case 'n' :
			switch ( tmpvers )
			{
			case 'f' :
    			mcipp = & cip->com_fast ;
	    		break ;
			case 'q' :
    			mcipp = & cip->com_quick ;
				break ;
			case 's' :
    			mcipp = & cip->com_slow ;
			}
			break;
		default :
			DBG0 (( "getcominf: illegal found_version '%c' \n", found_vers ))
			eprotwrite ( "getcominf: illegal found_version '%c' \n", found_vers );
			return ( -4 ) ;
		}
		DBG8 (( "getcominf: collecting info '%s' version '%c' \n",
						cip->layer , found_vers ))

		/*
		*  Now, read the file
		*/
		if ( found_vers == ' ' ) concomversion = tmpvers;
	}

	fp = fileopen ( list , "r" );
	if ( ! fp )
	{
	    DBG0 (( "cannot open '%s' \n", list ))
		eprotwrite ( "cannot open '%s' \n", list );
	    return ( - 2 );
	}
	DBG8 (( "getcominf: opened '%s' \n", list ))

	/* read com-file and insert every line in list lip */
	for ( ; ; )
	{
	    rc = get_dsc_line ( line , sizeof(line) , fp , list );
	    if ( rc <  0 )
	    {
		    DBG4 (( "getcominf: get_line error %d \n" , rc ))
		    (void) concom ( (char*) 0 , list );
		    (void) fileclose ( fp );
		    return ( rc ) ;
	    }
	    if ( rc == 0 ) break ;

	    lgt = strlen ( line );
	    if ( ! lgt ) continue ;

	    lip = (struct list_info*)
		    palloc ( sizeof(struct list_info) + lgt );
	    if ( ! lip )
	    {
		    DBG0 (( "insufficient dynamic memory \n" ))
			eprotwrite ( "insufficient dynamic memory \n" );
		    (void) concom ( (char*) 0 , list );
		    (void) fileclose ( fp );
		    return ( -3 );
	    }
	    FILL ( lip , 0 , sizeof(struct list_info) + lgt );
	    COPY ( lip->line , line , lgt );

	    * tailp = lip ;
	    tailp = & lip->next ;
	    DBG4 (( "getcominf: read line '%s' \n", lip->line ))
	}

	errcnt = 0 ;
	rc = concom ( (char*) 0 , list );
	(void) fileclose ( fp );
	if (rc < 0 )
		errcnt++ ;

	lgt = strlen ( layer );

	dummy_tip = (struct trg_info *) palloc ( sizeof(struct trg_info) );
	if ( dummy_tip == NULL )
	{
		DBG0 (( "insufficient dynamic memory \n" ))
		eprotwrite ( "insufficient dynamic memory \n" );
		return ( -3 );
	}

	FILL (  dummy_tip , 0 , sizeof(struct trg_info) );
	dummy_tip->name = name;
	dummy_tip->type = TT_COM;

	for ( ; head ; head = lip->next , free((char*)lip) )
	{
	    lip = head ;
	    DBG4 (( "getcominf: targtline '%s' \n", lip->line ))

	    /* initialize the module compile information */

	    FILL ( &mci , 0 , sizeof(struct mod_com_info) );
        
		mci.special_target_flag = ' ' ;

	    mci.version = ' ' ;
	    mci.versdef = ' ' ;
		mci.language = 0;

        mci.defaultlayer = (char *) repalloc( mci.defaultlayer, strlen( mci_defaultlayer ) + 1 );
        (void) strcpy (mci.defaultlayer , mci_defaultlayer );
	    rc = analyze_target_line ( lip->line , & mci, tmpvers );
	    if ( rc < 0 )
	    {
		    DBG8 (( "getcominf: analyze error %d \n", rc ))
		    errcnt ++ ;
	    }
	    if ( rc == 0 ) continue ;
        if ( mci.special_target_flag == '?' )
        {
            rc = collect_target_option( lip->line , dummy_tip , &mci );
	        if ( rc < 0 )
	        {
			    DBG4 (( "collect: internal option  error %d \n", rc ))
    		    errcnt ++ ;
	        }
            mci_defaultlayer = (char *) repalloc( mci_defaultlayer, strlen( mci.defaultlayer ) + 1 );
            (void) strcpy( mci_defaultlayer , mci.defaultlayer ); 
	        continue;
        }

	    /*
	     *  Only modules of the same layer may be contained.
	     *  Exceptions are the default targets (beginning with '*').
	     */
		/* GG 13.02.98 new defaults like h* or g*x */
	    if ( (mci.name[0] != DSC_DEFAULT[0]) && (mci.name[1] != DSC_DEFAULT[0])
                && mci.special_target_flag != 'R'
           )
		{
			get_layer( mci.name, &modlayer );
            if ( ! modlayer )
            {
                /* use layer, not mci.defaultlayer! */
                modlayer = (char *) repalloc( modlayer, strlen( layer ) + 1 );
                (void) strcpy( modlayer , layer );
            }
            else
            {
		        if ( cmp_layer ( modlayer , layer ) ) 
		        {
			        DBG0 (( "WARNING: no effect: '%s' in '%s' \n",
							        mci.name , name ))
					eprotwrite ( "WARNING: no effect: '%s' in '%s' \n",	mci.name , name );
			        continue ;
		        }
            }
		}

	    rc = insert_mod_com_info ( & mci , mcipp );
	    if ( rc < 0 )
	    {
		    DBG8 (( "getcominf: insert error %d \n", rc ))
		    errcnt ++ ;
	    }
	}

	if ( ! *mcipp)
	{
		DBG2 (( "found empty com description %s \n", layer ))
	}

	if ( errcnt )
	{
	    DBG0 (( "com info analyze errors %d \n", errcnt ))
		eprotwrite ( "com info analyze errors %d \n", errcnt );
	    return ( - 2 );
	}

	/*
	 *  The search algorithm for slow descriptions is as follows:
	 *  1. Search the description in the $DESC/slow  directory.
	 *  2. Search the description in the $DESC/quick directory.
	 *  3. Search the description in the $DESC/fast  directory.
	 *
	 *  The search algorithm for quick descriptions is as follows:
	 *  1. Search the description in the $DESC/quick directory.
	 *  2. Search the description in the $DESC/fast  directory.
	 *
	 *  The search algorithm for fast descriptions is as follows:
	 *  1. Search the description in the $DESC/fast  directory.
	 *
	 *  If the description was not found in the first step,
	 *  collect the descriptions for the other involved steps too.
	 *  This avoids collecting them again if the other version's
	 *  description is required.
	 */
	switch ( found_vers )
	{
	case 'f' :
	    if ( tmpvers == 'f' ) break ;
	    /* wanted quick or slow but did not find it */
	    cip->com_quick = cip->com_fast ;
	    /*FALLTHROUGH*/
	case 'q' :
	    if ( tmpvers == 'q' ) break ;
	    /* wanted slow but did not find it */
	    cip->com_slow = cip->com_quick ;
	    /*FALLTHROUGH*/
	}
	*rmcipp = *mcipp ;

    FREE_AND_SET_TO_NULL(mci_defaultlayer);
    FREE_AND_SET_TO_NULL(modlayer);

	DBG4 (( "getcominf: returning %d \n", 1 ))
	return ( 1 );
}

/*==========================================================================*/
static  int     append_mod_com_info 
(   
    struct mod_com_info     ** mcipp  ,
    struct mod_com_info     * mcip_to_append
)
{
	struct mod_com_info     ** temp_mcip ;
	int rc = 1;

	DBG2 (( "appmodcom: called \n" ))
	DBG3 (( "appmodcom: modnam '%s' \n", (*mcipp) ? (*mcipp)->name : "none"  ))

	for (temp_mcip = mcipp ; *temp_mcip ; temp_mcip = & (*temp_mcip)->next );
	
	*temp_mcip = mcip_to_append;

	DBG4 (( "appmodcom: returning %d \n", 1 ))
	return ( rc );
}
 

/*==========================================================================*/


static  int     insert_mod_com_info 
(   
    struct mod_com_info     * mcip  ,
    struct mod_com_info     ** mcipp
)
{
	struct mod_com_info     * nmcip ;
	int rc = 1;

	DBG2 (( "insmodcom: called \n" ))
	DBG3 (( "insmodcom: modnam '%s' \n", mcip->name ))

	/*
	 *  Mod_com_info has to be appended to end of com_in	fo
	 */
	for ( ; *mcipp ; mcipp = & (*mcipp)->next );

	/*
	 *  Allocate a new mod_com_info struct
	 */
	nmcip = (struct mod_com_info *)
			palloc ( sizeof(struct mod_com_info) );
	if ( nmcip == NULL )
	{
	    DBG0 (( "insufficient dynamic memory \n" ))
		eprotwrite ( "insufficient dynamic memory \n" );
	    return ( - 3 );
	}

	/* copy the collected module compile info */

	COPY ( nmcip , mcip , sizeof(struct mod_com_info) );
	
	rc = analyze_comtype ( nmcip );
	if ( ! rc )
		return ( - 3 );

	rc = analyze_com_variants ( nmcip );
	if ( rc < 0)
	{
		DBG0 (( "error while analyzing variant definition for '%s'\n", mcip->name ))
		eprotwrite ( "error while analyzing variant definition for '%s'\n", mcip->name  );
		return -3;
	}
	
	/* append this mod_com_info to queue */

	* mcipp = nmcip ;
	DBG7 (( "insmodcom: appended '%s' \n", nmcip->name ))

	DBG4 (( "insmodcom: returning %d \n", 1 ))
	return ( rc );
}
 

/*==========================================================================*/

static	int	collect_extra 
(   
    char			* target            ,
    struct mod_com_info     * ov_mcip   ,
    struct trg_info		* ctip          ,
    struct trg_info		** tipp         
)
{
	int		            rc = 0 ;
	struct dsc_info     * sip ;
	struct extra_info	* eip ;
	struct inc_info     * iip ;
	char			    * p ;
	

    DBG2 (( "colxtr: called \n" ))
	DBG3 (( "colxtr: target '%s' \n", target ))

	/*
	 *  Find the target's extension name.
	 *  If it has none, it is no extra target.
	 */

	/* PTS 1104605 */
	p = extension_exists (target);
	/* p = strrchr ( target , '.' ); */
	if ( ! p )
	{
	    DBG4 (( "colxtr: target '%s' has no dot: no extra \n", target ))
	    return ( 0 );
	}

	/*
	 *  If not already done,
	 *  read the extra-accessories-targets description file.
	 */
	if ( ! xtr_lists )
	{
	    rc = get_extra_info ();
	    if ( rc <= 0 ) return ( rc );
	}

	/*
	 *  Search for the extension name in the extra target's list.
	 */
	rc = -1 ;
	for ( eip = xtr_lists ; eip ; eip = eip->next )
	{
	    rc = strcmp ( eip->name , p );
	    if ( rc >= 0 ) break ;
	}
	if ( rc ) return ( 0 );	/* extra target not found */

	cut_global_variants(target);
		
	rc = insert_target ( TT_EXTRA , ' ' , DT_INCDEP , target , ctip , tipp, ov_mcip );
	if ( rc < 0 )
	{
		DBG4 (( "colxtr: insert error %d \n", rc ))
		return ( rc );
	}
	if ( rc == 0 )
	{
		DBG8 (( "colxtr: target already present '%s' \n", target ))
		return ( 1 );
	}
	DBG8 (( "colxtr: inserted '%s' \n", target ))

	/*
	 *  Copy the extra-target's info to the target struct.
	 */
    (*tipp)->layer = (char *) repalloc( (*tipp)->layer, strlen( eip->layer ) + 1 );
    (void) strcpy( (*tipp)->layer, eip->layer );

	(*tipp)->language    = eip->nodot ;
	(*tipp)->stat.binary = eip->binary ;
	(*tipp)->stat.ascii  = eip->ascii ;
	(*tipp)->stat.text  = eip->text ;
	(*tipp)->stat.exec   = eip->exec ; /* CTS 1110662 */

    if ( ! opt_nodependencies )
    {
	    /* expand environment variables */
	    rc = substitute ( eip->dir , &(*tipp)->file.file );
	    if ( rc ) return ( rc );

	    iip = NULL ;
	    for ( sip = eip->descriptions ; sip ; sip = sip->next )
	    {
	        rc = insert_description ( sip , & (*tipp)->descriptions, NULL, MCI_EXTRA );
	        if ( rc < 0 )
	        {
		        DBG4 (( "colxtr: insert error %d \n", rc ))
		        return ( rc );
	        }
	        DBG8 (( "colxtr: inserted '%s' \n", sip->descriptor ))
	    }
	    if ( eip->includes )
	    {
	        iip = eip->includes ;
	        rc = collect_include_list ( target , * tipp , iip, MCI_EXTRA );
	        if ( rc < 0 )
	        {
		        DBG8 (( "colxtr: collect_inc error %d \n", rc ))
		        return ( rc );
	        }
	        DBG8 (( "colxtr: collected include list for    '%s' \n",
						        (*tipp)->name ))
	    }

	    for ( sip = ov_mcip->descriptions ; sip ; sip = sip->next )
	    {
	        rc = insert_description ( sip , & (*tipp)->descriptions, NULL, MCI_EXTRA );
	        if ( rc < 0 )
	        {
		        DBG4 (( "colxtr: insert error %d \n", rc ))
		        return ( rc );
	        }
	        DBG8 (( "colxtr: inserted '%s' \n", sip->descriptor ))
	    }
	    if ( ov_mcip->includes )
	    {
	        iip = ov_mcip->includes ;
	        rc = collect_include_list ( target , * tipp , iip, MCI_EXTRA );
	        if ( rc < 0 )
	        {
		        DBG8 (( "colxtr: collect_inc error %d \n", rc ))
		        return ( rc );
	        }
	        DBG8 (( "colxtr: collected include list for    '%s' \n",
						        (*tipp)->name ))
	    }
    }
	DBG4 (( "colxtr: returning %d \n", 1 ))
	return ( 1 );
}

/*==========================================================================*/

static int	get_extra_info 
( 
)
{
	int		rc = 0 ;
	int		errcnt ;
	VFILE   * fp ;
	char	* name = EXTRA_LST ;
	char    list [ FILENAMESIZE ] ;
	char	line [ LINESIZE ] ;
	char	found_vers ;


	DBG2 (( "get_extra: called \n" ))

	/*
	 *  Create a dummy information struct.
	 *  Thus, xtr_lists is never empty, even if no
	 *  extra description file is available,
	 *  and the file will not be searched twice.
	 */
	(void) strcpy          ( line , ".~~~ ~~ /dummy/" );
	rc = analyze_extra_line ( line );
	if ( rc <= 0 ) return ( rc );

	DBG8 (( "search description file '%s' \n", name ))
	rc = get_dsc_file ( name , vmakeversion , list , & found_vers, &xtr_date, 0 );
	if ( rc <= 0 )
	{
	    if ( rc == 0 )
		    DBG4 (( "get_extra: description file not found '%s' \n", name ))
	    else    DBG8 (( "get_extra: get_dsc_file error %d \n", rc ))
	    return ( rc );
	}
	DBG4 (( "get_extra: description file found     '%s' \n", list ))

	fp = fileopen ( list , "r" );
	if ( ! fp )
	{
	    DBG0 (( "cannot open '%s' \n", list ))
		eprotwrite ( "cannot open '%s' \n", list );
	    return ( -2 );
	}
	DBG8 (( "get_extra: opened '%s' \n", list ))

	errcnt = 0 ;
	for ( ; ; )
	{
	    rc = get_dsc_line ( line , sizeof(line) , fp , list );
	    if ( rc < 0 )
	    {
		    DBG4 (( "get_extra: get_line error %d \n" , rc ))
		    (void) concom ( (char*) 0 , list );
		    (void) fileclose ( fp );
		    return ( rc ) ;
	    }
	    if ( rc == 0 ) break ;

	    rc = analyze_extra_line ( line );
	    if ( rc < 0 ) errcnt ++ ;
	}

	rc = concom ( (char*) 0 , list );
	(void) fileclose ( fp );
	if ( rc < 0 ) 
		errcnt++ ;

	if ( errcnt )
	{
	    DBG8 (( "extra file list collect errors %d \n", errcnt ))
	    return ( -1 );
	}

	DBG4 (( "get_extra: returning %d \n", 1 ))
	return ( 1 );
}

/*==========================================================================*/


static  int     analyze_extra_line
(   
    char                    * xtrline 
)
{
	int                 i , ec , rc=0 ;
	char				ch ;
	struct extra_info	* eip ;
	struct extra_info	** seipp ;
	char                * ext ;
	char                * lay ;
	char                * dir ;
	char				* p ;
	struct mod_com_info	mci ;
	char                description [ FILENAMESIZE ] ;
	char				quote ;


    DBG2 (( "anaxtrlin: called \n" ))
	DBG3 (( "anaxtrlin: xtrline '%s' \n", xtrline ))

	/* find the file name extension */
	for ( ext = xtrline ; isspace(*ext) ; ext ++ );
	if ( * ext == '#' )
	{
	    DBG7 (( "anaxtrlin: comment line \n" ))
	    return ( 0 );
	}

	/* find the layer name */
	for ( lay = ext ; * lay && ! isspace(*lay) ; lay ++ );
	if ( ! * lay )
	{
	    DBG0 (( "syntax error in Extra file: '%s' \n", ext ))
		eprotwrite ( "syntax error in Extra file: '%s' \n", ext );
	    return ( -1 );
	}
	* lay = '\0' ; /* terminate 'ext' */
	for ( lay ++ ; isspace(*lay) ; lay ++ );

	/* find the directory name */
	for ( dir = lay ; * dir && ! isspace(*dir) ; dir ++ );
	if ( ! * dir )
	{
	    DBG0 (( "syntax error in Extra file: '%s' '%s' \n", ext , lay ))
		eprotwrite ( "syntax error in Extra file: '%s' '%s' \n", ext , lay );
	    return ( -1 );
	}
	* dir = '\0' ; /* terminate 'lay' */
	for ( dir ++ ; isspace(*dir) ; dir ++ );

	/* terminate the directory name */
	for ( p = dir ; * p && ! isspace(*p) ; p ++ );
	ch = * p ; /* save the terminator */
	* p = '\0' ; /* terminate 'dir' */

	/* allocate an information struct */
	eip = (struct extra_info *) palloc ( sizeof(struct extra_info) );
	if ( ! eip )
	{
		DBG0 (( "insufficient dynamic memory \n" ))
		eprotwrite ( "insufficient dynamic memory \n" );
		return ( - 3 );
	}
	FILL ( eip , 0 , sizeof(struct extra_info) );

	eip->name  = (char *) palloc( strlen( ext ) + 1 );
	eip->layer = (char *) palloc( strlen( lay ) + 1 );
	eip->dir   = (char *) palloc( strlen( dir ) + 1 );

	(void) strcpy ( eip->name  , ext );
	(void) strcpy ( eip->layer , lay );
	(void) strcpy ( eip->dir   , dir );
    eip->nodot = ' ';

	/* insert it into the list */
	rc = -1 ;
	for ( seipp = & xtr_lists ; * seipp ; seipp = & (*seipp)->next )
	{
	    rc = strcmp ( (*seipp)->name , ext );
	    if ( rc >= 0 ) break ;
	}
	if ( rc == 0 )
	{
	    DBG0 (( "multiply declared in extra list: '%s' \n", ext ))
		eprotwrite ( "multiply declared in extra list: '%s' \n", ext );
	    return ( -1 );
	}
	eip->next = * seipp ;
	*seipp = eip ;

	/* extract all the target descriptions from the target line */

	* p = ch ; /* restore the terminator */
	FILL ( & mci , 0 , sizeof(mci) );
	
	mci.name = "";

	ec = 0 ;
	while ( * p != '\0' )
	{
	    for ( ; isspace(*p) ; p ++ ) ;	/* skip spaces */
	    if ( * p == '#' ) break ;		/* rest of line is comment */

	    /* copy the description */

	    quote = 0 ;
	    for ( i = 0 ; *p && (i < sizeof(description)) ; i ++ )
	    {
		    if ( (*p == '\\') && p[1] )
		    {
		        p ++ ;
		    }
		    else
		    {
		        if ( quote )
		        {
			        if ( *p == quote )
			        {
			            quote = 0 ;
			            p ++ ;
			            i -- ; /* compensate loop's increment */
			            continue ;
			        }
		        }
		        else
		        {
			        if ( (*p == '\'') || (*p == '"') )
			        {
			            quote = *p ;
			            p ++ ;
			            i -- ; /* compensate loop's increment */
			            continue ;
			        }
			        else
			            if ( isspace(*p) ) break ;
		        }
		    }
		    description [ i ] = * p ;
		    p ++ ;
	    }
	    if ( i >= sizeof(description) )
	    {
		    DBG0 (( "description too long on line '%s' \n", xtrline ))
			eprotwrite ( "description too long on line '%s' \n", xtrline );
		    return ( - 1 );
	    }
	    description [ i ] = '\0' ;
	    if ( i == 0 ) continue ;	/* empty description, e.g. line end */
	    DBG7 (( "anaxtrlin: target description '%s' \n", description ))

	    /* now analyze the description */
	    if ( ! strcmp(description , "nodot") )
	    {
		    eip->nodot = '.' ;
		    continue ;
	    }
	    if ( ! strcmp(description , "binary") )
	    {
		    eip->binary = TRUE ;
			continue ;
	    }
		/* CTS 1105829 -> */
		if ( ! strcmp(description , "ascii") )
	    {
		    eip->ascii = TRUE ;
		    continue ;
	    }
		if ( ! strcmp(description , "text") )
	    {
		    eip->text = TRUE ;
		    continue ;
	    }
		if ((eip->ascii || eip->text) && eip->binary)
		{
			 DBG0 (( "binary and ascii|text cannot define for the same extra target: '%s' \n", ext ))
			 eprotwrite ( "binary and ascii|text cannot define for the same extra target: '%s' \n", ext );
			 return ( -1 );
		}
		/* <- CTS 1105829 */
		if ( ! strcmp(description , "exec") )
	    {
		    eip->exec = TRUE ;
		    continue ;
	    }

	    /* now analyze a general description */
	    /* put the results into the mod_com_info struct */

	    rc = analyze_description ( description , & mci );
	    if ( rc < 0 )
	    {
		    DBG4 (( "anaxtrlin: analyze error %d \n", rc ))
		    DBG0 (( "unidentified target description '%s' \n", description ))
		    DBG0 (( "             target '%s' \n", eip->name ))
			eprotwrite ( "unidentified target description '%s' \n", description );
			eprotwrite ( "             target '%s' \n", eip->name );
		    ec ++ ;
	    }
	}

	/*
	 *  Link the descriptions to the extra info.
	 */
	eip->descriptions = mci.descriptions ;
	eip->includes     = mci.includes ;

	if ( ec != 0 )
		rc = - 1 ;
	else    rc = 1 ;

	DBG4 (( "anaxtrlin: returning %d \n", rc ))
	return ( rc );
}
/*==========================================================================*/
/* PTS 1105037 for DirMapping*/

global int	get_map_info 
( 
)
{
	int		rc = 0 ;
	int		errcnt ;
	VFILE   * fp ;
	char	* name = DIRMAP_LST ;
	char    list [ FILENAMESIZE ] ;
	char	line [ LINESIZE ] ;
	char	found_vers ;

	DBG2 (( "get_map: called \n" ))

	/*
	 *  Create a dummy information struct.
	 *  Thus, xtr_lists is never empty, even if no
	 *  extra description file is available,
	 *  and the file will not be searched twice.
	 */
	(void) strcpy          ( line , "~~~,~~~  #dummy" );
	rc = analyze_map_line ( line );
	if ( rc <= 0 ) return ( rc );

	DBG8 (( "search description file '%s' \n", name ))
	rc = get_dsc_file ( name , vmakeversion , list , & found_vers, & map_date, 0 );
	if ( rc <= 0 )
	{
	    if ( rc == 0 )
		    DBG4 (( "get_extra: description file not found '%s' \n", name ))
	    else    DBG8 (( "get_extra: get_dsc_file error %d \n", rc ))
	    return ( rc );
	}
	DBG4 (( "get_extra: description file found     '%s' \n", list ))

	fp = fileopen ( list , "r" );
	if ( ! fp )
	{
	    DBG0 (( "cannot open '%s' \n", list ))
		eprotwrite ( "cannot open '%s' \n", list );
	    return ( -2 );
	}
	DBG8 (( "get_extra: opened '%s' \n", list ))

	errcnt = 0 ;
	for ( ; ; )
	{
	    rc = get_dsc_line ( line , sizeof(line) , fp , list );
	    if ( rc < 0 )
	    {
		    DBG4 (( "get_map: get_line error %d \n" , rc ))
		    (void) concom ( (char*) 0 , list );
		    (void) fileclose ( fp );
		    return ( rc ) ;
	    }
	    if ( rc == 0 ) break ;

	    rc = analyze_map_line ( line );
	    if ( rc < 0 ) errcnt ++ ;
	}

	rc = concom ( (char*) 0 , list );
	(void) fileclose ( fp );
	if ( rc < 0 ) 
		errcnt++ ;

	if ( errcnt )
	{
	    DBG8 (( "map file list collect errors %d \n", errcnt ))
	    return ( -1 );
	}

	DBG4 (( "get_map: returning %d \n", 1 ))
	return ( 1 );
}



/*==========================================================================*/
/* PTS 1105037 for DirMapping*/

static  int     analyze_map_line
(   
    char                    * mapline 
)
{
	int                     rc=0 ;
	
	struct dirmap_info	* dmip ;
	struct dirmap_info	** dmipp ;
	char                    * mapp ;
	char					* shortform;
	char                    * lay ;
	int                     sep_found = 0;
	
    DBG2 (( "anaxtrlin: called \n" ))
	DBG3 (( "anaxtrlin: mapline '%s' \n", mapline ))

	/* find the file name extension */
	for ( mapp = mapline ; isspace(*mapp) ; mapp ++ );
	if ( ! *mapp )
	{
	    DBG7 (( "anamaplin: empty line \n" ))
	    return ( 0 );
	}
	
	if ( * mapp == COMMENT )
	{
	    DBG7 (( "anamaplin: comment line \n" ))
	    return ( 0 );
	}
	
	/* first word => short form */
	shortform = mapp;
	for ( mapp++ ; 
		  *mapp && ! isspace(*mapp) && ( *mapp != MAP_SEPARATOR ) && ( *mapp != COMMENT );
		  mapp ++ );
	
    if ( ! *mapp || ( *mapp == COMMENT ) )
	{
		DBG0 (( "syntax error in Mapping file: '%s' \n", mapline ))
		eprotwrite ( "syntax error in Mapping file: '%s' \n", mapline );
		return ( -1 );
	}
	if ( *mapp == MAP_SEPARATOR )
			sep_found = 1;	
	
	/* end of shortform */
	*mapp='\0';
	/* found the mapping layer and check the MAP_SEPARATOR */
	for ( mapp++ ; *mapp && ( isspace(*mapp) || ( *mapp == COMMENT ) || 
		    ((*mapp == MAP_SEPARATOR) && (sep_found==0)));mapp ++ )
		if ( *mapp == MAP_SEPARATOR )
			sep_found = 1;
	if ( ! *mapp || ( *mapp == COMMENT ) || ( *mapp == MAP_SEPARATOR ) || ( sep_found != 1 ) )
	{
	   DBG0 (( "syntax error in Mapping file: '%s' \n", mapline ))
	   eprotwrite ( "syntax error in Mapping file: '%s' \n", mapline );
	   return ( -1 );
	}
	/* layer */
	lay = mapp;
	for ( mapp++ ; 
		  *mapp && ! isspace(*mapp) && ( *mapp != MAP_SEPARATOR ) && ( *mapp != COMMENT );
		  mapp ++ );
	
    if ( ( *mapp == MAP_SEPARATOR ) )
	{
	   DBG0 (( "syntax error in Mapping file: '%s' \n", mapline ))
	   eprotwrite ( "syntax error in Mapping file: '%s' \n", mapline );
	   return ( -1 );
	}
	/* end of layer */
	*mapp='\0';

	/* allocate an information struct */
	dmip = (struct dirmap_info *) palloc ( sizeof(struct dirmap_info) );
	if ( ! dmip )
	{
		DBG0 (( "insufficient dynamic memory \n" ))
		eprotwrite ( "insufficient dynamic memory \n" );
		return ( - 3 );
	}	
	FILL ( dmip , 0 , sizeof(struct dirmap_info) );

    dmip->shortform = (char *) palloc( strlen( shortform ) + 1 );
    dmip->layer     = (char *) palloc( strlen( DIR_SRC_SAPDB ) + strlen( lay ) + 1 );
	
	(void) strcpy ( dmip->shortform , shortform );
	(void) strcpy ( dmip->layer , DIR_SRC_SAPDB );
	(void) strcat ( dmip->layer , lay );

	/* insert it into the list */
	rc = -1 ;
	for ( dmipp = & map_lists ; * dmipp ; dmipp = & (*dmipp)->next )
	{
	    rc = strcmp ( (*dmipp)->shortform , shortform );
	    if ( rc >= 0 ) break ;
	}

	if ( rc == 0 )
	{
	    DBG0 (( "multiply declared in mapping list: '%s' \n", shortform ))
		eprotwrite ( "multiply declared in mapping list: '%s' \n", shortform );
	    return ( -1 );
	}
	dmip->next = * dmipp ;
	*dmipp = dmip ;

	rc = 1 ;

	DBG4 (( "anamaplin: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/


static  int     analyze_target_line 
(   
    char                    * trgline   ,
    struct mod_com_info     * mcip      ,   
    char                    version
)
{
	int                     isfile , i , j , l , ec , rc=0 ;
	char                    *p ;
	char                    description [ DESCRIPTIONSIZE ];
	char                    token [ DESCRIPTIONSIZE ];
	char			        quote ;
    /* suppress for block '[{f|q|s} ...  {f|q|s}]' in a line */
    int                     suppress = 0, ignore_token = 0;
    int                     found, block = 0;
    int                     token_len = 0;
    int                     analyze_name = TRUE;

    DBG2 (( "anatrglin: called \n" ))
	DBG3 (( "anatrglin: trgline '%s' \n", trgline ))

	/* find the first non white space character */
/*
trgline=(char*) malloc (100);
strcpy (trgline,"abc -scrpt='\"das ist ein test\"'");
*/

	for ( p = trgline ; isspace(*p) ; p ++ ) {}
	if ( ! *p )
	{
	    DBG7 (( "anatrglin: empty line \n" ))
	    return ( 0 );
	}
	if ( *p == '#' )
	{
	    mcip->special_target_flag = '#' ;
	    DBG7 (( "anatrglin: comment line \n" ))
	    return ( 0 );
	}

	/* if it is a command we don't analyze anything */

	if ( *p == CMD_PREFIX )
	{
	    mcip->special_target_flag = CMD_PREFIX ;
	    DBG7 (( "anatrglin: command found \n" ))
	    return ( 1 );
	}

	if ( *p == '?' )
	{
        /* internal target option */
	    mcip->special_target_flag = '?' ;
	    DBG7 (( "anatrglin: internal option found \n" ))
	    return ( 1 );
	}

    /* only in a com list */
    if ( !strnicmp( p, "regex(", strlen( "regex(" ) ) )
    {
        char    * end;
        int     len;
        
        end = strrchr( p , ')' );
        p += 6;
        if ( ! end )
        {
	        mcip->special_target_flag = '#' ;
	        DBG0 (( "anatrglin: missing ')' after \"regex(\" or empty regex \n" ))
			eprotwrite ( "anatrglin: missing ')' after \"regex(\" or empty regex \n" );
	        return( 0 );
        }
        len = end - p;

		mcip->name = (char *) palloc( len + 1 ) ;
        STRCPY( mcip->name, p, len + 1 )        

        mcip->special_target_flag = 'R';
		mcip->comtype = COM_REGEXP;
        analyze_name = FALSE;
        p = ++end;
    }

    if ( analyze_name )
    {
    /*
	 *  A line with a slash ('/') in the first token
	 *  describes a file.
	 *  A line starting with a non-alnum (except a slash)
	 *  is an option, not a file!
	 *  Because file names can be longer than NAMESIZE-1
	 *  we cannot copy it into the mod_com_info struct.
	 *  Nevertheless we have to analyze the descriptions.
	 *  Though almost the version descriptions (fast,...)
	 *  are of some interest, we do a full analyze for the
	 *  reason of symmetry.
	 *  Therefore, we first evaluate the length of the first
	 *  token and determine whether it has a slash in it.
	 */
	isfile = 0 ;
	quote = 0 ;
    found = 0;
    while( !( found && !block ) )
    {
        if ( ignore_token ) ignore_token = 0;
	    for ( i = 0 ; *p && (i < sizeof(description)) ; i ++ )
	    {
            /* skip escape sequence */
	        if ( (*p == '\\') && p[1] ) p ++ ;
            /* else ... */
	        else
	        {
		        if ( *p == '/' ) isfile = 1 ;
		        if ( quote )
		        {
		            if ( *p == quote )
		            {
			            quote = 0 ;
			            p ++ ;
			            i -- ; /* compensate loop's increment */
			            continue ;
		            }
		        }
		        else
		        {
		            if ( (*p == '\'') || (*p == '"') )
		            {
			            quote = *p ;
			            p ++ ;
			            i -- ; /* compensate loop's increment */
			            continue ;
		            }
		            else
    			        if ( isspace(*p) ) break ;
		        }
	        }
            token[i] = *p ;
	        p ++ ;
	    }

        if ( i >= sizeof(token) )
	    {
	        DBG0 (( "target too long on line '%s' \n", trgline ))
			eprotwrite ( "target too long on line '%s' \n", trgline );
	        return ( - 1 );
	    }
	    if ( i == 0 )
	    {
	        DBG7 (( "anatrglin: empty target name \n" ))
	        return ( 0 );
	    }
        token[i] = 0 ;

    
        /* look for block '[{f|q|s} ...  {f|q|s}]' */
        if ( token[ 0 ] == '[' )
        {
            switch ( token[ 1 ] )
            {
            case 'f' :
                if ( version != 'f' ) 
                    suppress = 1;
                break;
            case 'q' :
                if ( version != 'q' ) 
                    suppress = 2;
                break;
            case 's' :
                if ( version != 's' ) 
                    suppress = 3;
                break;
            default  :
                DBG0 (( "wrong block start '%s' \n", token ))
				eprotwrite ( "wrong block start '%s' \n", token );
                return( -1 );
            }
            block = 1;
            DBG5(( "anatrgline: start of block '[{f|q|s} ...  {f|q|s}]' \n" ))
            DBG5(( "anatrgline: found now '%d' \n", found ))
        }
        else
        if ( token[ 1 ] == ']' )
        {
            if ( suppress == 0 && block )
            {
                block = 0;
            }
            else
            {
                switch ( token[ 0 ] )
                {
                case 'f' :
                    if ( suppress != 1 )
                    {
                        DBG0 (( "parse error, faulty end of block '[{f|q|s}' \n" ))
						eprotwrite ( "parse error, faulty end of block '[{f|q|s}' \n" );
                        return ( -1 );
                    }
                    suppress = 0;
                    break;
                case 'q' :
                    if ( suppress != 2 )
                    {
                        DBG0 (( "parse error, faulty end of block '[{f|q|s}' \n" ))
						eprotwrite ( "parse error, faulty end of block '[{f|q|s}' \n" );
                        return ( -1 );
                    }
                    suppress = 0;
                    break;
                case 's' :
                    if ( suppress != 3 )
                    {
                        DBG0 (( "parse error, faulty end of block '[{f|q|s}' \n" ))
						eprotwrite ( "parse error, faulty end of block '[{f|q|s}' \n" );
                        return ( -1 );
                    }
                    suppress = 0;
                    break;
                default  :
                    DBG0 (( "wrong block end '%s' \n", description ))
					eprotwrite ( "wrong block end '%s' \n", description );
                    return( -1 );
                }
            }
            block = 0;
            DBG5(( "anatrgline: end of block '[{f|q|s} ...  {f|q|s}]' \n" ))
            DBG5(( "anatrgline: suppress now '%d' \n", suppress ))
        }
        else
        if ( ! suppress )
        {
            (void) strcpy( description , token );
            token_len = i;
            found = 1;
            for ( ; isspace(*p) ; p ++ );
        }
        for ( ; isspace(*p) ; p ++ );
    }
    
	DBG5 (( "anatrglin: target '%s' \n", description ))

	/*
	 *  A line starting with a non-alnum (except a slash)
	 *  is an option, not a file!
	 */
	if ( ! isalnum(description[0]) &&
	     (description[0] != '/') &&
	     (description[0] != '$') &&
		 (description[0] != '_') ) /* PTS 1106608 */
	{
	    isfile = 0 ;
	}
	
	if ( isalpha(description[0] ) || (description[0] == '.' ) ) /* PTS 1106608 */
	{
	    isfile = 0 ;
	}

	/*
	 *  If it is a file, note it in the special flag. The file name
	 *  is not copied. It has to be extracted from the target line
	 *  by the function 'collect_file'.
	 *  If it is no file, the target name is copied into the
	 *  mod_com_info struct after it is checked for the correct length.
	 */
	if ( isfile )
	{
	    DBG7 (( "anatrglin: file found \n" ))
	    mcip->special_target_flag = '/' ;

		if (! mcip->file )
		{
			mcip->file = (struct file_info *) palloc (sizeof(struct file_info));
			FILL (  mcip->file , 0 , sizeof(struct file_info) );
		}
	    rc = substitute ( description , &mcip->file->file );
	    if ( rc ) return ( rc );
	    DBG5 (( "anatrglin: target '%s' \n", mcip->file->file ))

	    /* eliminate trailing slashes */
	    for ( i = strlen(mcip->file->file)-1 ; 
             (i > 0) && (mcip->file->file[ i ] == '/') ; 
             i -- )
	    {
		    mcip->file->file[i] = 0 ;
	    }
	    i ++ ;

	    /* put the base name into the 'name' field */
	    for ( j = i - 2 ; j >= 0 ; j -- )
    		if ( mcip->file->file[ j ] == '/' ) break ;
	    if ( j >= 0 )
		    if ( mcip->file->file[ j ] == '/' ) 
                j ++ ;
	        else
    		    j = 0 ;
	    l = i - j ;

		mcip->name = (char *) palloc( l + 1 );
	    STRCPY ( mcip->name , mcip->file->file + j , l + 1 );

	}
	else
	{
	    /*
	     *  A target starting with a non-alnum character is an option.
	     *  It is treated like a file.
	     */
	    
		/*
		if ( (! isalnum(description[0])) && 
             (description[0] != INDEX_SYMBOL ) &&
             (description[0] != '*' ) )
			PTS 1106608 */
		if ( (! isalnum(description[0]) && (description[0] !='_') && (description[0] !='.')) && 
             (description[0] != INDEX_SYMBOL ) &&
             (description[0] != '*' ) )
	    {
			DBG7 (( "anatrglin: option found \n" ))
			mcip->special_target_flag = '-' ;
			if (! mcip->file )
			{
				mcip->file = (struct file_info *) palloc (sizeof(struct file_info));
				FILL (  mcip->file , 0 , sizeof(struct file_info) );
			}
			rc = substitute ( description , &mcip->file->file );
			
			if ( rc ) return ( rc );
			DBG7 (( "anatrglin: target '%s' \n", mcip->file ))

			/*
			 *  Use the begin of the option as target name.
			 */

	    }

		mcip->name = (char *) palloc( token_len + 1 );
        STRCPY ( mcip->name , description, token_len + 1 );
		
	    DBG7 (( "anatrglin: target name '%s' \n", mcip->name ))
	}
    } /* analyze_name */

	/* extract all the target descriptions from the target line */
	ec = 0 ;
	while ( *p != '\0' )
	{
        /* reset marker of block */
        if ( ignore_token ) ignore_token = 0;

        for ( ; isspace(*p) ; p ++ );	/* skip spaces */
	    if ( * p == '#' ) break ;		/* rest of line is comment */

	    /* copy the description */

	    quote = 0 ;
		
		for ( i = 0 ; *p && (i < sizeof(description)) ; i ++ )
	    {
		    if ( (*p == '\\') && p[1] )
		    {
		        p ++ ;
				if (*p == '"') 
			    	quote = (quote == '\\') ? 0 : '\\';
		    }
		    else
		    {
		        if ( quote )
		        {
			        if ( *p == quote )
			        {
			            quote = 0 ;
			            p ++ ;
			            i -- ; /* compensate loop's increment */
			            continue ;
			        }
		        }
		        else
		        {
			        if ( (*p == '\'') || (*p == '"') )
			        {
			            quote = *p ;
			            p ++ ;
			            i -- ; /* compensate loop's increment */
			            continue ;
			        }
			        else
			            if ( isspace(*p) ) break ;
		        }
		    }
		    description [ i ] = * p ;
		    p ++ ;
	    }
	    if ( i >= sizeof(description) )
	    {
		    DBG0 (( "description too long on line '%s' \n", trgline ))
			eprotwrite ( "description too long on line '%s' \n", trgline );
		    return ( - 1 );
	    }
	    description [ i ] = '\0' ;
	    if ( i == 0 ) continue ;	/* empty description, e.g. line end */

        /* look for block '[{f|q|s} ...  {f|q|s}]' */
        if ( description[ 0 ] == '[' )
        {
            switch ( description[ 1 ] )
            {
            case 'f' :
                if ( version != 'f' ) 
                    suppress = 1; else ignore_token = 1;
                break;
            case 'q' :
                if ( version != 'q' ) 
                    suppress = 2; else ignore_token = 1;
                break;
            case 's' :
                if ( version != 's' ) 
                    suppress = 3; else ignore_token = 1;
                break;
            default  :
                DBG0 (( "wrong block start '%s' \n", description ))
				eprotwrite ( "wrong block start '%s' \n", description );
                return( -1 );
            }
            DBG5(( "anatrgline: start of block '[{f|q|s} ...  {f|q|s}]' \n" ))
            DBG5(( "anatrgline: suppress now '%d' \n", suppress ))
        }
        else
        if ( description[ 1 ] == ']' )
        {
            if ( suppress == 0 ) 
            {
                ignore_token = 1;
            }
            else
            {
                switch ( description[ 0 ] )
                {
                case 'f' :
                    if ( suppress != 1 )
                    {
                        DBG0 (( "parse error, faulty end of block '[{f|q|s}' \n" ))
						eprotwrite ( "parse error, faulty end of block '[{f|q|s}' \n" );
                        return ( -1 );
                    }
                    break;
                case 'q' :
                    if ( suppress != 2 )
                    {
                        DBG0 (( "parse error, faulty end of block '[{f|q|s}' \n" ))
						eprotwrite ( "parse error, faulty end of block '[{f|q|s}' \n" );
                        return ( -1 );
                    }
                    break;
                case 's' :
                    if ( suppress != 3 )
                    {
                        DBG0 (( "parse error, faulty end of block '[{f|q|s}' \n" ))
						eprotwrite ( "parse error, faulty end of block '[{f|q|s}' \n" );
                        return ( -1 );
                    }
                    break;
                default  :
                    DBG0 (( "wrong block end '%s' \n", description ))
					eprotwrite ( "wrong block end '%s' \n", description );
                    return( -1 );
                }
                suppress = 0; ignore_token = 1;
            }
            DBG5(( "anatrgline: end of block '[{f|q|s} ...  {f|q|s}]' \n" ))
            DBG5(( "anatrgline: suppress now '%d' \n", suppress ))
        }  

        if ( ! suppress && ! ignore_token )
        {
        	char        * subst_desc  = NULL;

            DBG7 (( "anatrglin: target description '%s' \n", description ))
	        /* now analyze the description */
	        /* put the results into the mod_com_info struct */
			rc = substitute ( description , &subst_desc );
		    if ( rc ) return ( rc );

		    DBG7 (( "anatrglin: target '%s' \n", subst_desc ))

		    rc = analyze_description ( subst_desc , mcip );
	        if ( rc < 0 )
	        {
		        DBG4 (( "anatrglin: analyze error %d \n", rc ))
		        DBG0 (( "unidentified target description '%s' \n",
							        description ))
		        DBG0 (( "             target '%s' \n", mcip->name ))
				eprotwrite ( "unidentified target description '%s' \n",description );
				eprotwrite ( "             target '%s' \n", mcip->name );
		        ec ++ ;
	        }
            FREE_AND_SET_TO_NULL(subst_desc);
        }
	}

	if ( ec != 0 )
		rc = - 1 ;
	else    
        rc = 1 ;

	DBG4 (( "anatrglin: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/


static  int     analyze_description
(   
    char                    * moddsc,
    struct mod_com_info     * mcip  
)
{
	int                     dsclgt , vallgt , rc=0 ;
	char                    * p ;
	char                    descr [ FILENAMESIZE ] ;
	char                    value [ DESCRIPTIONSIZE ] ;

	/* &gar PTS 1001577 -> */
	char            * puretarget;
	/* filename without layer*/ 
	puretarget = ( mcip->name[0] == INDEX_SYMBOL ) ? skip_index( mcip->name ) : mcip->name ;
	/* <- &gar PTS 1001577 */

    DBG2 (( "anadsc: called \n" ))
	DBG3 (( "anadsc: moddsc '%s' \n", moddsc ))

	/* test for version descriptions */
	/* f, f[fqs], q, q[fqs], s, s[fqs] */
	/* first char is version, second char is default version */

	switch ( moddsc[0] )
	{
	case 'f' :
	case 'q' :
	case 's' :

	    switch ( moddsc[1] )
	    {
	    case '\0' :
	    case 'f' :
	    case 'q' :
	    case 's' :

		if ( ! moddsc[1] || ! moddsc[2] )
		{
		    mcip->version = moddsc[0] ;
		    DBG7 (( "anadsc: version %s \n", versname(moddsc[0]) ))
		    if ( moddsc[1] )
		    {
			    mcip->versdef = moddsc[1] ;
			    DBG7 (( "anadsc: default %s \n", versname(moddsc[1]) ))
		    }
		    return ( 1 );
		}
	    }
	}

	/* test for special descriptions */

	switch ( moddsc[0] )
	{
	/* CTS 1105829 */
	case 'a' :
		if ( strcmp ( moddsc , "ascii"     ) == 0 )
		{
			mcip->stat.ascii  = 1;
			DBG7 (( "anadsc: ascii \n" ))
		    return ( 1 );
		}
		break;

	/* CTS 1105829 */
	case 'b' :
		if ( strcmp ( moddsc , "binary"     ) == 0 )
		{
			mcip->stat.binary  = 1;
			DBG7 (( "anadsc: binary \n" ))
		    return ( 1 );
		}
		break;

	case 'c' :
		if ( strcmp ( moddsc , "checkdate"     ) == 0 )
		{
			mcip->stat.checkdate  = 1;
			mcip->version  = 'n';
			DBG7 (( "anadsc: checkdate \n" ))
		    return ( 1 );
		}

		if ( strncmp ( moddsc , DSC_SPEC_COMFILE, strlen( DSC_SPEC_COMFILE ) ) == 0 )
		{
            /* language pointer */
            char    * q ;
			char    * spec_comfile;

			q = moddsc + strlen( DSC_SPEC_COMFILE );
			if ( *q == '=' )
			{
				/* get directory */
				q++;
				
				/* while ((*q != '\0') && (*q == ':'))
                    q++;
					*/

				spec_comfile = (char*) palloc(strlen(q)+1);
				strcpy(spec_comfile, q);
				mcip->comfilename = spec_comfile;
			}
			else
			{
				 DBG0 (( "anadsc: missing '=' in comfile definition  \n" ))
				 eprotwrite ( "anadsc: missing '=' in comfile definition  \n" );
                 return( -1 );
			}
			DBG8 (( "anadsc: comfile '%s'\n", spec_comfile ))
			return ( 1 );
		}
		break;
			
	case 'd' :
		if ( strcmp ( moddsc , "d"     ) == 0 ||
		     strcmp ( moddsc , "debug" ) == 0    )
		{
			mcip->stat.debug = 1 ;
			DBG7 (( "anadsc: debug \n" ))
			return ( 1 );
		}
		if ( strncmp ( moddsc , DSC_OBJDEMAND2, strlen( DSC_OBJDEMAND2 ) ) == 0 && 
             ! strchr( moddsc , DSC_DELIMITER ) )
		{
            /* module is a demandee */
            char    * q, *ddir ;
            
            if ( * ( q = moddsc + strlen( DSC_OBJDEMAND2 ) ) == '{' )
            {
                /* get directory */
		    	ddir = moddsc + strlen( DSC_OBJDEMAND2 ) + 1;
                q = strchr( ddir, '}' );
                if ( q == NULL )
                {
                    DBG0 (( "anadsc: missing '}' in demand module \n" ))
					eprotwrite ( "anadsc: missing '}' in demand module \n" );
                    return( -1 );
                }
				else
				{
                    * q = '\0';
					mcip->objdemanddir = (char *) palloc( strlen( ddir ) + 1 );
					(void) strcpy( mcip->objdemanddir, ddir );
                }
            }
			mcip->stat.deptype  = DT_OBJDEMDEP;
			DBG7 (( "anadsc: demand \n" ))
			return ( 1 );
		}
		
		if ( strcmp ( moddsc , "definition" ) == 0 )
		{
			mcip->stat.definition = 1;
			mcip->stat.binary  = 1;
			mcip->stat.deptype  = DT_OBJDEMDEP;
			DBG7 (( "anadsc: definition \n" ))
			return ( 1 );
		}
		/* &gar CTS 1000237 -> */
		if ( strcmp ( moddsc , "distribute") == 0 ||
			strcmp ( moddsc , "distrib" ) == 0 )
		{
			mcip->stat.distribute = 1;
			DBG7 (( "anadsc: distribute \n" ))
			return ( 1 );
		}
		/* <- &gar CTS 1000237 */
		break ;

	case 'e' :
		/* CTS 1110258 */
		if ( strcmp ( moddsc , "exec" ) == 0 )
		{
			mcip->stat.exec = 1;
			DBG7 (( "anadsc: exec \n" ))
			return ( 1 );
		}

	/* CTS 1112700 */
	case 'l' :
		if ( strncmp ( moddsc , DSC_LANGUAGE, strlen (DSC_LANGUAGE) ) == 0 )
		{
			/* language pointer */
            char    * q ;
			struct  lang_info    * lip ;

			q = moddsc + strlen( DSC_LANGUAGE );
			if ( *q == '=' )
			{
				mcip->language = *(q+1);
				/* check language for existence */
				if ( ! lng_lists ) (void) get_lang_info();
				for ( lip = lng_lists ; lip ; lip = lip->next )
				{
					if (lip->language == mcip->language) break;
				}
				if (! lip)
				{
					DBG0 (( "anadsc: unknown language character for  %s\n", mcip->name ))
					eprotwrite ( "anadsc: unknown language character for  %s\n", mcip->name );
					return( -1 );
				}
			}
			else
			{
				 DBG0 (( "anadsc: missing '=' in lang definition  \n" ))
				 eprotwrite ( "anadsc: missing '=' in lang definition  \n" );
                 return( -1 );
			}
			DBG7 (( "anadsc: lang \n" ))
			return ( 1 );
		}

	case 'n' :
		/* CTS 1110258 */
		if ( strcmp ( moddsc , "noversion" ) == 0 )
		{
			mcip->version='n';
			return(1);
		}

		/* CTS 1110367 */
		if ( strcmp ( moddsc , "nodistribute") == 0  ||
			strcmp ( moddsc , "nodistrib" ) == 0 )
		{
			mcip->nodistrib = 1;
			DBG7 (( "anadsc: nodistribute \n" ))
			return ( 1 );
		}

		if ( strcmp ( moddsc , "noshrglob" ) == 0 )
		{
		    if ( vopt_ignore_shrglob )
		    {
		    	DBG2 (( "anadsc: ignoring noshrglob \n" ))
		    }
		    else
		    {
			    mcip->stat.shrglob = 2 ;
			    DBG7 (( "anadsc: noshrglob \n" ))
		    }
		    return ( 1 );
		}
		if ( strcmp ( moddsc , "noobjcopy" ) == 0 )
		{
			mcip->stat.noobjcopy = 1 ;
			DBG7 (( "anadsc: noobjcopy \n" ))
		    return ( 1 );
		}
		/* &gar CTS 1102466 -> */
		if ( strcmp ( moddsc , "nodebug" ) == 0 )
		{
			mcip->stat.nodebug = 1 ;
			DBG7 (( "anadsc: nodebug \n" ))
		    return ( 1 );
		}
		/* <- &gar CTS  1102466 */ 
		if ( strcmp ( moddsc , "no_dep_remake" ) == 0 )
		{
			mcip->stat.no_dep_remake = 1 ;
			DBG7 (( "anadsc: no_dep_remake \n" ))
		    return ( 1 );
		}
		/* CTS 1105891 -> */
		if ( strcmp ( moddsc , "nobind"  ) == 0 )
		{
			mcip->nobind = 1 ;
			DBG7 (( "anadsc: nobind\n" ))
			return ( 1 );
		}
		/* <-CTS 1105891 */
		/* CTS 1112700 */
		if ( strcmp ( moddsc , "noextension" ) == 0 )
		{
			mcip->stat.noextension = 1 ;
			DBG7 (( "anadsc: noextension \n" ))
		    return ( 1 );
		}
		/* <- CTS 1112700 */ 

		if ( strcmp ( moddsc , "novariant" ) == 0 )
		{
			mcip->novariant=1;
			return(1);
		}
		
		if ( strcmp ( moddsc , DSC_NOCOMINFO ) == 0 )
		{
			mcip->nocomfile=1;
			mcip->comfilename = (char*) repalloc(mcip->comfilename, 1);
			mcip->comfilename [0] = '\0';
			return(1);
		}

		break ;

    case 'o' :
		if ( strncmp ( moddsc , DSC_OBJDEMAND1, strlen( DSC_OBJDEMAND1 ) ) == 0 && 
             ! strchr( moddsc , DSC_DELIMITER ) )
		{
            /* module is a demandee */
            char    * q, *ddir ;
            
            if ( * ( q = moddsc + strlen( DSC_OBJDEMAND1 ) ) == '{' )
            {
                /* get directory */
		    	ddir = moddsc + strlen( DSC_OBJDEMAND1 ) + 1;
                q = strchr( ddir, '}' );
                if ( q == NULL )
                {
                    DBG0 (( "anadsc: missing '}' in demand module \n" ))
					eprotwrite ( "anadsc: missing '}' in demand module \n" );
                    return( -1 );
                }
				else
				{
                    * q = '\0';
					mcip->objdemanddir = (char *) palloc( strlen( ddir ) + 1 );
					(void) strcpy( mcip->objdemanddir, ddir );
                }
            }
			mcip->stat.deptype  = DT_OBJDEMDEP;
			DBG7 (( "anadsc: demand \n" ))
			return ( 1 );
		}
		if ( strcmp ( moddsc , DSC_ON_SUCCESS ) == 0 )
		{
			mcip->on_success = 1;
			return(1);
		}
        break;
    case 'p' :
		if ( strcmp ( moddsc , "p"       ) == 0 ||
		     strcmp ( moddsc , "profile" ) == 0    )
		{
			mcip->stat.profile = 1 ;
			DBG7 (( "anadsc: profile \n" ))
			return ( 1 );
		}
		if ( strncmp ( moddsc , DSC_PROTSTYLE, strlen (DSC_PROTSTYLE) ) == 0 )
		{
			/* language pointer */
            char    * q ;

			q = moddsc + strlen( DSC_PROTSTYLE );
			if ( *q == '=' )
			{
				q++;
				if ( stricmp ( q , PROTSTYLE_QUALIFIED_TEXT ) == 0 )
				{
					mcip->protstyle = PROTSTYLE_QUALIFIED_VALUE;
				}
				else
				if ( stricmp ( q , PROTSTYLE_SILENT_TEXT ) == 0 )
				{
					mcip->protstyle = PROTSTYLE_SILENT_VALUE;
				}
				else
				{
					DBG0 (( "anadsc: unknown protstyle '%s' (allowed: %s|%s)\n", q, PROTSTYLE_QUALIFIED_TEXT, PROTSTYLE_SILENT_TEXT ))
					eprotwrite ( "anadsc: unknown protstyle '%s' (allowed: %s|%s)\n", q, PROTSTYLE_QUALIFIED_TEXT, PROTSTYLE_SILENT_TEXT );
                    return( -1 );
				}
			}
			else
			{
				 DBG0 (( "anadsc: missing '=' in lang definition  \n" ))
				 eprotwrite ( "anadsc: missing '=' in lang definition  \n" );
                 return( -1 );
			}
			DBG7 (( "anadsc: lang \n" ))
			return ( 1 );
		}
		break ;

	case 'r' :
		if ( strcmp ( moddsc , "remake"  ) == 0 )
		{
			mcip->remake = 1 ;
			mcip->stat.noobjcopy = 1 ;
			DBG7 (( "anadsc: remake \n" ))
			return ( 1 );
		}
		break ;

	case 's' :
		if ( strcmp ( moddsc , "shrglob" ) == 0 )
		{
		    if ( vopt_ignore_shrglob )
		    {
    			DBG2 (( "anadsc: ignoring shrglob \n" ))
		    }
		    else
		    {
    			mcip->stat.shrglob = 1 ;
	    		DBG7 (( "anadsc: shrglob \n" ))
		    }
		    return ( 1 );
		}
        else
		if ( strncmp ( moddsc , DSC_SRCDEMAND, strlen( DSC_SRCDEMAND ) ) == 0 && 
             ! strchr( moddsc , DSC_DELIMITER ) )
		{
            /* module is a demandee */
            char    * q, *ddir ;
            
            if ( * ( q = moddsc + strlen( DSC_SRCDEMAND ) ) == '{' )
            {
                /* get directory */
		    	ddir = moddsc + strlen( DSC_SRCDEMAND ) + 1;
                q = strchr( ddir, '}' );
                if ( q == NULL )
                {
                    DBG0 (( "anadsc: missing '}' in demand module \n" ))
					eprotwrite ( "anadsc: missing '}' in demand module \n" );
                    return( -1 );
                }
				else
				{
                    * q = '\0';
					mcip->srcdemanddir = (char *) palloc( strlen( ddir ) + 1 );
					(void) strcpy( mcip->srcdemanddir, ddir );
                }
            }
			mcip->stat.deptype  = DT_SRCDEMDEP;
			DBG7 (( "anadsc: demand \n" ))
			return ( 1 );
		}
		else
		if ( strcmp ( moddsc , "suppress_uncond" ) == 0 )
		{
			mcip->suppress_uncond = 1;
			return(1);
		}
		break ;

	/* case 't': */
	/* CTS TYPE */
	case 't' :
		if ( strncmp ( moddsc , DSC_TYPE, strlen (DSC_TYPE) ) == 0 )
		{
			/* type pointer */
            char    * q ;
			
			q = moddsc + strlen( DSC_TYPE );
			if ( *q == '=' )
			{
				if (! strncmp(q+1, "inc", 3))
				{
					mcip->version= ' ';
					mcip->type= TT_INC;
				}
				else
					if (! strncmp(q+1, "mod", 3))
						mcip->type= TT_MOD;
					else
					{
						DBG0 (( "anadsc: unknown type defintion for %s\n", mcip->name ))
						eprotwrite ( "anadsc: unknown type defintion for %s\n", mcip->name );
						return( -1 );
					}
			}
			DBG7 (( "anadsc: type \n" ))
			return ( 1 );
		}
		else
		{
			if ( strcmp ( moddsc , "text"     ) == 0 )
			{
				mcip->stat.text  = 1;
				DBG7 (( "anadsc: text \n" ))
				return ( 1 );
			}
		}
		break;

	case 'u' :
		if ( strcmp ( moddsc , "uncond" ) == 0 )
		{
			mcip->stat.forcemake = 1 ;
			mcip->stat.uncond = 1 ;
	    	DBG7 (( "anadsc: forcemake and as a result uncond\n" ))
		    return ( 1 );
		}
		break ;

	case '<' :
		if ( moddsc[1] == '-' )
		{
			if (moddsc[2] == '\0')
			{
				DBG0 (( "anadsc: error in source location for %s\n", mcip->name ))
				eprotwrite ( "anadsc: error in source location for %s\n", mcip->name );
				return( -1 );
			}

			mcip->local_source = (char *) repalloc( mcip->local_source, strlen( moddsc + 2 ) + 1 );
			(void) strcpy( mcip->local_source, moddsc + 2 ); 		
			return ( 1 );
		}
		break;


    case '-' :
		if ( strncmp ( moddsc , "->", 2 ) == 0 )
		{
            struct stat fileinfo;
			char nodistrib = 0;

			if (moddsc[2] == '\0')
			{
				DBG0 (( "anadsc: error in in file destination for %s\n", mcip->name ))
				eprotwrite ( "anadsc: error in file destination for %s\n", mcip->name );
				return( -1 );
			}

			if (! mcip->file )
			{
				mcip->file = (struct file_info *) palloc (sizeof(struct file_info));
				FILL (  mcip->file , 0 , sizeof(struct file_info) );
			}

            mcip->file->file = (char *) repalloc( mcip->file->file, strlen( moddsc + 2 ) + 1 );
            (void) strcpy( mcip->file->file, moddsc + 2 ); 

			/* CTS 1110367 */
			mcip->file->nodistrib = check_nodistrib (mcip->file->file);

            rc = stat( mcip->file->file, & fileinfo );
            if  ( (rc == 0 && ( fileinfo.st_mode & S_IFDIR ))  ||  
                  (mcip->file->file[ strlen( mcip->file->file ) - 1 ] == '/') )
            {
				/* PTS 1102241 none if com-description for all */
				/* if Regular Expr. or select more files -> not append filename */

		        if ( (mcip->special_target_flag != 'R') && 
					!strstr ( mcip->name, DSC_DEFAULT ) ) 
				{
					if ( mcip->file->file[ strlen( mcip->file->file ) - 1 ] != '/')
                    {
                        mcip->file->file = (char *) repalloc( mcip->file->file, strlen( mcip->file->file ) + 1 + 1 );
						(void) strcat( mcip->file->file, "/" );
    					/* (void) strcat( mcip->file, puretarget );  new */
                    }
					/* PTS 1110251
                    else
                    {
					     &gar PTS 1001577 -> 
                        mcip->file = (char *) repalloc( mcip->file, strlen( mcip->file ) + 
                                strlen( puretarget ) + 1 );
					    (void) strcat( mcip->file, puretarget );  
					    <- &gar PTS 1001577 
                    }
					*/
				}
				else    /* CTS 1107469 */
				{
					if (mcip->file->file[ strlen( mcip->file->file ) - 1 ] != '/')
                    {
                        mcip->file->file = (char *) repalloc( mcip->file->file, strlen( mcip->file->file ) + 
                                strlen( puretarget ) + 1 );
						(void) strcat( mcip->file->file, "/" );
                    }
				}      /* <- CTS 1107469 */
            }
            DBG7 (( "anadsc: output file '%s' \n", mcip->file ))
		    return ( 1 );
		}
		break ;
    }
	/* CTS 1105829 
	if ( mcip->stat.binary && mcip->stat.ascii)
	{
		DBG0 (( "binary and ascii cannot define in the same description: '%s' \n"))
		return ( -1 );
	}
	*/
	
	
	/*
	 *  Other descriptions are of the format <descriptor>[=<value>].
	 *  First try to locate the end of the descriptor ('=' or
	 *  null byte).
	 */

	for ( p = moddsc ; * p != DSC_DELIMITER && * p != '\0' ; p ++ );
	dsclgt = p - moddsc ;
	DBG8 (( "anadsc: descriptor length %d \n", dsclgt ))

	if ( dsclgt <= 0 )
	{
		DBG8 (( "anadsc: nothing to analyze \n" ))
		return ( 0 );
	}
	if ( dsclgt > sizeof(descr)-1 )
	{
		DBG0 (( "illegal descriptor size '%s' \n", moddsc ))
		eprotwrite ( "illegal descriptor size '%s' \n", moddsc );
		return ( - 2 );
    }

	/* description is not neccessarily null terminated */
	COPY ( descr , moddsc , dsclgt );
	descr [ dsclgt ] = '\0' ;
	DBG7 (( "anadsc: descriptor '%s' \n", descr ))

	/*
	 *  If there was a '=' found, there is a value too.
	 *  IST 1994-03-24: The value may be empty (e.g. -TRACE=).
	 */
	/* if ( * p == DSC_DELIMITER ) p ++ ;	IST 1994-03-24 */
	vallgt = strlen ( p ) ;
	if ( vallgt > sizeof(value)-1 )
	{
	    DBG0 (( "illegal value size '%s' \n", moddsc ))
		eprotwrite ( "illegal value size '%s' \n", moddsc );
	    return ( - 2 );
	}

	/* value is null terminated and of correct length */
	(void) strcpy ( value , p );

	/*
	 *  IST 1994-03-24
	 *  Special descriptions will not have the '=' in the value.
	 */
	p = value ;				/* IST 1994-03-24 */
	if ( * p == DSC_DELIMITER ) p ++ ;	/* IST 1994-03-24 */

    /* values like 'inc=..' found */
	if ( strcmp ( descr , DSC_INC ) == 0 )
	{
		rc = insert_mod_inc ( p , mcip );	/* IST 1994-03-24 */
	    if ( rc < 0 )
	    {
		    DBG8 (( "anadsc: insert_mod_inc error %d \n", rc ))
		    return ( rc );
	    }
	}
	else
	if ( strcmp ( descr , DSC_COMPILER ) == 0 )
	{
		
		mcip->compiler = (char*) palloc (strlen(p)+1);
		strcpy (mcip->compiler, p);
	}
	else
	if ( strncmp( descr , DSC_OBJDEMAND1, ( rc = strlen( DSC_OBJDEMAND1 ) ) ) == 0 || 
         strncmp( descr , DSC_OBJDEMAND2, ( rc = strlen( DSC_OBJDEMAND2 ) ) ) == 0 )
	{
        /* module is a demander */
		char    * q;
		char    * ddir;  /* PTS 1103000 */
		
		if ( * ( q = descr + rc ) == '{' )
		{
			/* get directory */
			ddir = descr + rc + 1;
			q = strchr (ddir,'}');
			if ( q == NULL )
			{
				DBG0 (( "anadsc: missing '}' in demand list \n" ))
				eprotwrite ( "anadsc: missing '}' in demand list \n" );
				return( -1 );
			}
			else
			{
				*q='\0';  /* end of string (PTS 1103000) */
			}
		}
		else if ( *q != '\0' )
		{
			DBG0 (( "anadsc: wrong 'demand' description '%s' \n", descr ))
			eprotwrite ( "anadsc: wrong 'demand' description '%s' \n", descr );
			return( -1 );
		}
		else 
            ddir = q  /* set to '\0' PTS 1103000 */;

		rc = insert_mod_demands ( p , ddir , mcip, &mcip->objdemands );	/* PTS 1103000 */
		if ( rc < 0 )
		{
		    DBG8 (( "anadsc: insert_mod_demands error %d \n", rc ))
		    return ( rc );
		}
	}
	else
    if ( strncmp( descr , DSC_SRCDEMAND, ( rc = strlen( DSC_SRCDEMAND ) ) ) == 0 )
	{
        /* module is a source demander */
		char    * q;
		char    * ddir;
		
		if ( * ( q = descr + rc ) == '{' )
		{
			/* get directory */
			ddir = descr + rc + 1;
			q = strchr (ddir,'}');
			if ( q == NULL )
			{
				DBG0 (( "anadsc: missing '}' in demand list \n" ))
				eprotwrite ( "anadsc: missing '}' in demand list \n" );
				return( -1 );
			}
			else
			{
				*q='\0';
			}
		}
		else if ( *q != '\0' )
		{
			DBG0 (( "anadsc: wrong 'demand' description '%s' \n", descr ))
			eprotwrite ( "anadsc: wrong 'demand' description '%s' \n", descr );
			return( -1 );
		}
		else 
            ddir = q;

		rc = insert_mod_demands ( p , ddir , mcip, &mcip->srcdemands );
		if ( rc < 0 )
		{
		    DBG8 (( "anadsc: insert_mod_demands error %d \n", rc ))
		    return ( rc );
		}
	}
	else
	if ( strcmp ( descr , DSC_DEP ) == 0 )
	{
	    rc = insert_mod_dep ( p , mcip );
	    if ( rc < 0 )
	    {
		    DBG8 (( "anadsc: insert_mod_dep error %d \n", rc ))
		    return ( rc );
	    }
	}
	else
	if ( strcmp ( descr , DSC_EXTDEP ) == 0 )
	{
	    rc = insert_mod_extdep ( p , mcip );
	    if ( rc < 0 )
	    {
		    DBG8 (( "anadsc: insert_mod_extdep error %d \n", rc ))
		    return ( rc );
	    }
	}
	else
	if ( strcmp ( descr , DSC_OBJ ) == 0 )
	{
	    rc = insert_mod_obj ( p , mcip );
	    if ( rc < 0 )
	    {
		    DBG8 (( "anadsc: insert_mod_obj error %d \n", rc ))
		    return ( rc );
	    }
	}
    else
	if ( strcmp ( descr , DSC_OPT ) == 0 )
	{
		rc = insert_mod_opt ( p , mcip );	/* IST 1994-03-24 */
	    if ( rc < 0 )
	    {
		    DBG8 (( "anadsc: insert_mod_opt error %d \n", rc ))
		    return ( rc );
	    }
	}
	else
	if ( strcmp ( descr , DSC_OUTPUT ) == 0 )
	{
		rc = insert_mod_output ( p , mcip );	
	    if ( rc < 0 )
	    {
		    DBG8 (( "anadsc: insert_mod_output error %d \n", rc ))
		    return ( rc );
	    }
	}
	else
	if ( strcmp ( descr , DSC_PARENTLINKOPTION ) == 0 )
	{
		rc = insert_mod_parentlinkoption ( p , mcip );	
	    if ( rc < 0 )
	    {
		    DBG8 (( "anadsc: insert_mod_parentlinkoption error %d \n", rc ))
		    return ( rc );
	    }
	}
	else
	{
		/* &gar (PTS 1001838) -> also '+...' without append '=1' */
		if ( (vallgt == 0) && ((descr[0] != '-') && (descr[0] != '+')))
		{
		    /* supply a default value for conditional compilation */
		    /* (void) strcpy ( value , "1" ); IST 1994-03-24 */
		    (void) strcpy ( value , "=1" );
		}
		DBG7 (( "anadsc: value      '%s' \n", value ))

		rc = insert_mod_dsc ( descr , value , mcip );
		if ( rc < 0 )
		{
		    DBG8 (( "anadsc: insert_mod_dsc error %d \n", rc ))
		    return ( rc );
		}
	}

	DBG4 (( "anadsc: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/


static  int     insert_mod_inc
(   
    char                    * inclist   ,
    struct mod_com_info     * mcip      
)
{
	int			    i ;
	struct inc_info ** siipp , * iip ;
	char            * p ;
	char            incnam [ FILENAMESIZE ] ;
	char            * layer  = NULL;
	char            name   [ NAMESIZE ];
	char			disposition ;

	DBG2 (( "insmodinc: called \n" ))
	DBG3 (( "insmodinc: inclist '%s' \n", inclist ))

	for ( siipp = &mcip->includes ; *siipp ; siipp = &(*siipp)->next );

	disposition = 0 ; /* replace an existing include list */
	for ( p = inclist ; * p != '\0' ; )
	{
		for ( ; *p == ',' ; p ++ );
		if ( (*p == '+') || (*p == '-') )
		{
			disposition = *p ;
			p ++ ;
		}
		for ( i = 0 ; i < sizeof(incnam) ; i ++ )
		{
			if ( * p == ',' || * p == '\0' ) break ;
			incnam [ i ] = * p ;
			p ++ ;
		}
		if ( i >= sizeof(incnam) )
		{
			DBG0 (( "include module name too long '%s' \n",
								inclist ))
			eprotwrite ( "include module name too long '%s' \n",inclist );
			return ( - 1 );
		}
		incnam [ i ] = '\0' ;
		if ( i == 0 ) continue ; /* e.g. two commas (,,) */
		DBG7 (( "insmodinc: incnam '%s' \n", incnam ))

		iip = (struct inc_info *) palloc ( sizeof(struct inc_info) );
		if ( iip == NULL )
		{
			DBG0 (( "insufficient dynamic memory \n" ))
			eprotwrite ( "insufficient dynamic memory \n" );
			return ( - 3 );
		}
		FILL (  iip , 0 , sizeof(struct inc_info) );


		/* CTS long names */
		if ( is_mapped_source_with_full_path (incnam, &layer, name) )
		{
			iip->name = (char *) palloc( strlen( name ) + 1 );
			(void) strcpy ( iip->name , name );
		}
		else
		{
			get_layer( incnam , &layer );
			if ( ! layer && mcip->defaultlayer && (*mcip->defaultlayer) )
			{
				iip->name = (char *) palloc( strlen( incnam ) + strlen( mcip->defaultlayer ) + 2 + 1 );
				(void) sprintf ( iip->name, ":%s/" , mcip->defaultlayer );
				(void) strcat  ( iip->name , incnam );

			}
			else
			{
				iip->name = (char *) palloc( strlen( incnam ) + 1 );
				(void) strcpy ( iip->name , incnam );

			}
		}

		iip->disposition = disposition ;

		/* don't set language for extra targets */
		if ( is_valid_sourcename( iip->name, (char **)NULL, NULL ) )
		{
		    iip->language = get_language ( iip->name );
		}
		else
		    iip->language = ' ' ;

		* siipp = iip ;
		siipp = & iip->next ;

		DBG7 (( "insmodinc: inserted '%s' \n", iip->name ))
		DBG7 (( "insmodinc: language '%c' \n", iip->language ))

		/*
		 *  Let only the first module replace the include list.
		 *  Add all further modules per default.
		 */
		if ( disposition == 0 ) disposition = '+' ;
	}

	/*
	    If include list is empty, this means that nothing is to
	    include. To overwrite a default include list, a non empty
	    include list must be given with a module. Therefore,
	    an empty element is to be inserted here.
	*/

	if ( mcip->includes == NULL )
	{
		iip = (struct inc_info *) palloc ( sizeof(struct inc_info) );
		if ( iip == NULL )
		{
			DBG0 (( "insufficient dynamic memory \n" ))
			eprotwrite ( "insufficient dynamic memory \n" );
			return ( - 3 );
		}

		FILL (  iip , 0 , sizeof(struct inc_info) );

		iip->name = (char *) palloc( 1 );
		iip->name[ 0 ] = '\0';

		iip->disposition = 0 ; /* clear an existing list */
		iip->language = ' ' ;
		mcip->includes = iip ;
	}

    FREE_AND_SET_TO_NULL(layer);

	DBG4 (( "insmodinc: returning %d \n", 1 ))
	return ( 1 );
}


/*==========================================================================*/


static  int     insert_mod_opt 
(   
    char                    * options   ,
    struct mod_com_info     * mcip      
)
{
	int                     ec , rc=0 ;
	char			* p ;

	DBG2 (( "insmodopt: called \n" ))
	DBG3 (( "insmodopt: options '%s' \n", options ))

	ec = 0 ;
	for ( p = options ; * p != '\0' ; p ++ )
	{
		switch ( * p )
		{
		case 'i' :
				mcip->stat.level_i = 1 ;
				break ;
		case 'I' :
				mcip->stat.level_i = 2 ;
				break ;
		case 's' :
				mcip->stat.level_s = 1 ;
				break ;
		case 'S' :
				mcip->stat.level_s = 2 ;
				break ;
		case 'p' :
				mcip->stat.level_p = 1 ;
				break ;
		case 'P' :
				mcip->stat.level_p = 2 ;
				break ;
		case 'c' :
				mcip->stat.level_c = 1 ;
				break ;
		case 'C' :
				mcip->stat.level_c = 2 ;
				break ;
		case 'o' :
				mcip->stat.level_o = 1 ;
				break ;
		case 'O' :
				mcip->stat.level_o = 2 ;
				break ;
		default :
			DBG0 (( "illegal character in options '%c' \n", * p ))
			DBG0 (( "module '%s' \n", mcip->name ))
			eprotwrite ( "illegal character in options '%c' \n", * p );
			eprotwrite ( "module '%s' \n", mcip->name );
			ec -- ;
		}
	}

	/*
	    If no level characters are present, set default.
	*/

	if ( ( mcip->stat.level_s == 0 ) && ( mcip->stat.level_p == 0 ) &&
	     ( mcip->stat.level_c == 0 ) && ( mcip->stat.level_o == 0 )    )
	{
		mcip->stat.level_s = 2 ;
		mcip->stat.level_o = 2 ;
	}

	/*
	    If vmake option indicates that only source level is concerned,
	    this can be used to test the existence of the source files.
	    If vmake option indicates that only object level is concerned,
	    this can be used for a pure object port.
	    Other single level options are of no sense.
	*/

	rc = 0 ;
	if ( mcip->stat.level_s ) rc ++ ;
	if ( mcip->stat.level_p ) rc ++ ;
	if ( mcip->stat.level_c ) rc ++ ;
	if ( mcip->stat.level_o ) rc ++ ;
	if ( rc < 2 )
	{
	    /* only one level activated, allowed are 'S' or 'O' */

	    if ( mcip->stat.level_s == 0 && mcip->stat.level_o == 0 )
	    {
		    DBG0 (( "senseless vmake option '%s' \n", options ))
			eprotwrite ( "senseless vmake option '%s' \n", options );
		    ec -- ;
	    }
	}
	DBG7 (( "insmodopt: level_i %d \n", mcip->stat.level_i ))
	DBG7 (( "insmodopt: level_s %d \n", mcip->stat.level_s ))
	DBG7 (( "insmodopt: level_p %d \n", mcip->stat.level_p ))
	DBG7 (( "insmodopt: level_c %d \n", mcip->stat.level_c ))
	DBG7 (( "insmodopt: level_o %d \n", mcip->stat.level_o ))

	DBG4 (( "insmodopt: returning %d \n", ec ))
	return ( ec );
}


/*==========================================================================*/

static  int     insert_mod_dsc 
(   
    char                    * descr ,
    char                    * value ,
    struct mod_com_info     * mcip  
)
{
	int                     rc ;
	struct dsc_info         ** ssipp , * sip ;

	DBG2 (( "insmoddsc: called \n" ))
	DBG3 (( "insmoddsc: descriptor '%s' \n", descr ))
	DBG3 (( "insmoddsc: value      '%s' \n", value ))

	rc = - 1 ;
	for ( ssipp = & mcip->descriptions ; *(ssipp) != NULL ;
	      ssipp = & (*ssipp)->next )
	{
		rc = strcmp ( (*ssipp)->descriptor , descr );
		if ( rc >= 0 ) break ;
	}

	if  ( rc == 0 )
	{
		DBG8 (( "insmoddsc: descriptor found '%s' \n",
							(*ssipp)->descriptor ))
        FREE_AND_SET_TO_NULL((*ssipp)->value);
	}
	else
	{
		sip = (struct dsc_info *) palloc ( sizeof(struct dsc_info) );
		if ( sip == NULL )
		{
			DBG0 (( "insufficient dynamic memory \n" ))
			eprotwrite ( "insufficient dynamic memory \n" );
			return ( - 3 );
		}

		FILL (  sip , 0 , sizeof(struct dsc_info) );

        sip->descriptor = (char *) palloc( strlen( descr ) + 1 );
		(void) strcpy ( sip->descriptor , descr );

		sip->next = * ssipp ;
		* ssipp = sip ;

		DBG7 (( "insmoddsc: new descriptor '%s' \n",
							sip->descriptor ))
		rc = 1 ;
	}

    (*ssipp)->value = (char *) palloc( strlen( value ) + 1 );
    (void) strcpy ( (*ssipp)->value , value );

	DBG7 (( "insmoddsc: inserted value '%s' \n", (*ssipp)->value ))

	DBG4 (( "insmoddsc: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/


static  int     insert_mod_demands 
(
    char                    * demandlist,
    char                    * directory,
    struct mod_com_info     * mcip ,
    struct dem_mod_info     ** demand_list
)
{
	int			        i ;
	struct dem_mod_info ** sdipp , * dip ;
	char                * p ;
	char                demandnam [ NAMESIZE ] ;
    char                * layer = NULL;
	char				disposition ;

	DBG2 (( "insmoddemands: called \n" ))
	DBG3 (( "insmoddemands: demandlist '%s' \n", demandlist ))

	for ( sdipp = &(*demand_list) ; *sdipp ; sdipp = &(*sdipp)->next );

	disposition = 0 ; /* replace an existing demand list */
	for ( p = demandlist ; * p != '\0' ; )
	{
		for ( ; *p == ',' ; p ++ );
		if ( (*p == '+') || (*p == '-') )
		{
			disposition = *p ;
			p ++ ;
		}
		for ( i = 0 ; i < sizeof(demandnam) ; i ++ )
		{
			if ( * p == ',' || * p == '\0' ) break ;
			demandnam [ i ] = * p ;
			p ++ ;
		}
		if ( i >= sizeof(demandnam) )
		{
			DBG0 (( "demand module name too long '%s' \n", demandlist ))
			eprotwrite ( "demand module name too long '%s' \n", demandlist );
			return ( - 1 );
		}
		demandnam [ i ] = '\0' ;
		if ( i == 0 ) continue ; /* e.g. two commas (,,) */
		DBG7 (( "insmoddemand: demandnam '%s' \n", demandnam ))

		dip = (struct dem_mod_info *) palloc ( sizeof(struct dem_mod_info) );
		if ( dip == NULL )
		{
			DBG0 (( "insufficient dynamic memory \n" ))
			eprotwrite ( "insufficient dynamic memory \n" );
			return ( - 3 );
		}
		FILL (  dip , 0 , sizeof(struct dem_mod_info) );

        get_layer( demandnam , &layer );

        if ( ! layer && mcip->defaultlayer && (*mcip->defaultlayer) )
        {
	        dip->name = (char *) palloc( strlen( mcip->defaultlayer ) + strlen( demandnam ) + 2 + 1 );
            (void) sprintf ( dip->name, ":%s/" , mcip->defaultlayer );
		    (void) strcat  ( dip->name , demandnam );
        }
        else
		{
	        dip->name = (char *) palloc( strlen( demandnam ) + 1 );
		    (void) strcpy ( dip->name , demandnam );
		}

		dip->disposition = disposition ;

		* sdipp = dip ;
		sdipp = & dip->next ;

		DBG7 (( "insmoddemand: inserted '%s' \n", dip->name ))

		/*
		 *  Let only the first module replace the include list.
		 *  Add all further modules per default.
		 */
		if ( disposition == 0 ) disposition = '+' ;

        if ( is_valid_sourcename( dip->name, (char **)NULL, NULL ) )
		{
		    dip->language = get_language ( dip->name );
		}
		else
		    dip->language = ' ' ;

        dip->directory = (char *) palloc( strlen( directory ) + 1 );
		(void) strcpy( dip->directory , directory );
	}

	/*
	    If include list is empty, this means that nothing is to
	    include. To overwrite a default include list, a non empty
	    include list must be given with a module. Therefore,
	    an empty element is to be inserted here.
	*/

	if ( (*demand_list) == NULL )
	{
		dip = (struct dem_mod_info *) palloc ( sizeof(struct dem_mod_info) );
		if ( dip == NULL )
		{
			DBG0 (( "insufficient dynamic memory \n" ))
			eprotwrite ( "insufficient dynamic memory \n" );
			return ( - 3 );
		}

		FILL (  dip , 0 , sizeof(struct inc_info) );

		dip->disposition = 0 ; /* clear an existing list */

        dip->directory = (char *) palloc( strlen( directory ) + 1 );
        (void) strcpy( dip->directory, directory );
		(*demand_list) = dip ;
	}

    FREE_AND_SET_TO_NULL(layer);

	DBG4 (( "insmoddemands: returning %d \n", 1 ))
	return ( 1 );
}


/*==========================================================================*/

static  int     insert_mod_dep 
(
    char                    * deplist   ,
    struct mod_com_info     * mcip 
)
{
	int			            i ;
	struct inc_info         ** siipp , * iip ;
	char                    * p ;
    char                    depnam [ FILENAMESIZE ] ;
	char                    * layer  = NULL ;
	char                    name   [ NAMESIZE ];
	char			        disposition ;

	DBG2 (( "insmoddep: called \n" ))
	DBG3 (( "insmoddep: deplist '%s' \n", deplist ))

	for ( siipp = &mcip->dependencies ; *siipp ; siipp = &(*siipp)->next );

	disposition = 0 ; /* a dependencies can't be removed */
	for ( p = deplist ; * p != '\0' ; )
	{
		for ( ; *p == ',' ; p ++ );
		if ( (*p == '+') || (*p == '-') )
		{
			disposition = *p ;
			p ++ ;
		}
		for ( i = 0 ; i < sizeof(depnam) ; i ++ )
		{
			if ( * p == ',' || * p == '\0' ) break ;
			depnam [ i ] = * p ;
			p ++ ;
		}

		if ( i >= sizeof(depnam) )
		{
			DBG0 (( "dependency module name too long '%s' \n",
								deplist ))
			eprotwrite ( "dependency module name too long '%s' \n",	deplist );
			return ( - 1 );
		}
		depnam [ i ] = '\0' ;
		if ( i == 0 ) continue ; /* e.g. two commas (,,) */
		DBG7 (( "insmoddep: depnam '%s' \n", depnam ))

		iip = (struct inc_info *) palloc ( sizeof(struct inc_info) );
		if ( iip == NULL )
		{
			DBG0 (( "insufficient dynamic memory \n" ))
			eprotwrite ( "insufficient dynamic memory \n" );
			return ( - 3 );
		}
		FILL (  iip , 0 , sizeof(struct inc_info) );

		/* CTS long names */
		if ( is_mapped_source_with_full_path (depnam, &layer, name) )
		{
			iip->name = (char *) palloc( strlen( name ) + 1 );
			(void) strcpy ( iip->name , name );
		}
		else
		{
			get_layer( depnam, &layer );
			if ( ! layer && mcip->defaultlayer && (*mcip->defaultlayer) )
			{
				iip->name = (char *) palloc( strlen( depnam ) + strlen( mcip->defaultlayer ) + 2 + 1 );
				(void) sprintf ( iip->name, ":%s/" , mcip->defaultlayer );
				(void) strcat  ( iip->name , depnam );
			}
			else
			{
				iip->name = (char *) palloc( strlen( depnam ) + 1 );
				(void) strcpy ( iip->name , depnam );
			}
		}


		iip->disposition = disposition ;

		/* don't set language for extra targets */
		if ( is_valid_sourcename( iip->name, (char **)NULL, NULL ) )
		{
		    iip->language = get_language ( iip->name );
		}
		else
		    iip->language = ' ' ;

		* siipp = iip ;
		siipp = & iip->next ;

		/*
		 *  Let only the first module replace the include list.
		 *  Add all further modules per default.
		 */
		if ( disposition == 0 ) disposition = '+' ;


		DBG7 (( "insmoddep: inserted '%s' \n", iip->name ))
		DBG7 (( "insmoddep: language '%c' \n", iip->language ))

	}

	/*
	    If dependency list is empty, this means that this module
		isn't dependend from nothing special module.
	    To overwrite a default dependency list, a non empty
	    dependency list must be given with a module. Therefore,
	    an empty element is to be inserted here.
	*/

	if ( mcip->dependencies == NULL )
	{
		iip = (struct inc_info *) palloc ( sizeof(struct inc_info) );
		if ( iip == NULL )
		{
			DBG0 (( "insufficient dynamic memory \n" ))
			eprotwrite ( "insufficient dynamic memory \n" );
			return ( - 3 );
		}

		FILL (  iip , 0 , sizeof(struct inc_info) );

		iip->name = (char *) palloc( 1 );
		iip->name[ 0 ] = '\0';

		iip->disposition = 0 ; /* clear an existing list */
		iip->language = ' ' ;
		mcip->dependencies = iip ;
	}

    FREE_AND_SET_TO_NULL(layer);

	DBG4 (( "insmoddep: returning %d \n", 1 ))
	return ( 1 );
}

/*==========================================================================*/

static  int     insert_mod_output 
(
    char                    * outputlist   ,
    struct mod_com_info     * mcip 
)
{
	int			        i,rc ;
	struct file_info  ** sfipp , * fip ;
	char                * p ;
	char                output [ FILENAMESIZE ] ;
    struct stat fileinfo;

	DBG2 (( "insmodoutput: called \n" ))
	DBG3 (( "insmodoutput: outputlist '%s' \n", outputlist ))

	for ( sfipp = &mcip->file ; *sfipp ; sfipp = &(*sfipp)->next );

	for ( p = outputlist ; * p != '\0' ; )
	{
		for ( ; *p == ',' ; p ++ );
		for ( i = 0 ; i < sizeof(output) ; i ++ )
		{
			if ( * p == ',' || * p == '\0' ) break ;
			output [ i ] = * p ;
			p ++ ;
		}
		if ( i >= sizeof(output) )
		{
			DBG0 (( "output module name too long '%s' \n",
								outputlist ))
			eprotwrite ( "output module name too long '%s' \n", outputlist );
			return ( - 1 );
		}
		output [ i ] = '\0' ;
		if ( i == 0 ) continue ; /* e.g. two commas (,,) */
		DBG7 (( "insmodoutput: output '%s' \n", output ))

		fip = (struct file_info *) palloc ( sizeof(struct file_info) );
		if ( fip == NULL )
		{
			DBG0 (( "insufficient dynamic memory \n" ))
			eprotwrite ( "insufficient dynamic memory \n" );
			return ( - 3 );
		}
		FILL (  fip , 0 , sizeof(struct file_info) );

		fip->file = (char *) palloc( strlen(output) + 1 );
		(void) strcpy( fip->file, output );

		fip->nodistrib = check_nodistrib (fip->file);
        rc = stat( fip->file, & fileinfo );
        if  ( (rc == 0 && ( fileinfo.st_mode & S_IFDIR ))  ||  
              (fip->file[ strlen( fip->file ) - 1 ] == '/') )
        {
		    if ( (mcip->special_target_flag != 'R') && 
				!strstr ( mcip->name, DSC_DEFAULT ) ) 
			{
				if ( fip->file[ strlen( fip->file ) - 1 ] != '/')
                {
                    fip->file = (char *) repalloc( fip->file, strlen( fip->file ) +  2 );
					(void) strcat( fip->file, "/" );
    			}
			}
			else   
			{
				if (fip->file[ strlen( fip->file ) - 1 ] != '/')
                {
                    fip->file = (char *) repalloc( fip->file, strlen( fip->file ) + 1);
                    (void) strcat( fip->file, "/" );
                }
			}      
        }

		* sfipp = fip ;
		sfipp = & fip->next ;

		DBG7 (( "insmodoutput: inserted '%s' \n", fip->file ))
	}

	DBG4 (( "insmodoutput: returning %d \n", 1 ))
	return ( 1 );
}

/*==========================================================================*/

static  int     insert_mod_extdep 
(
    char                    * deplist   ,
    struct mod_com_info     * mcip 
)
{
	int			        i ;
	struct extdep_info  ** sdipp , * dip ;
	char                * p ;
	char                depnam [ FILENAMESIZE ] ;

	DBG2 (( "insmodextdep: called \n" ))
	DBG3 (( "insmodextdep: deplist '%s' \n", deplist ))

	for ( sdipp = &mcip->ext_dependencies ; *sdipp ; sdipp = &(*sdipp)->next );

	for ( p = deplist ; * p != '\0' ; )
	{
		for ( ; *p == ',' ; p ++ );
		for ( i = 0 ; i < sizeof(depnam) ; i ++ )
		{
			if ( * p == ',' || * p == '\0' ) break ;
			depnam [ i ] = * p ;
			p ++ ;
		}
		if ( i >= sizeof(depnam) )
		{
			DBG0 (( "dependency module name too long '%s' \n",
								deplist ))
			eprotwrite ( "dependency module name too long '%s' \n", deplist );
			return ( - 1 );
		}
		depnam [ i ] = '\0' ;
		if ( i == 0 ) continue ; /* e.g. two commas (,,) */
		DBG7 (( "insmodextdep: depnam '%s' \n", depnam ))

		dip = (struct extdep_info *) palloc ( sizeof(struct extdep_info) );
		if ( dip == NULL )
		{
			DBG0 (( "insufficient dynamic memory \n" ))
			eprotwrite ( "insufficient dynamic memory \n" );
			return ( - 3 );
		}
		FILL (  dip , 0 , sizeof(struct extdep_info) );

		dip->file = (char *) palloc( strlen( depnam ) + 1 );
		(void) strcpy( dip->file, depnam );

		* sdipp = dip ;
		sdipp = & dip->next ;

		DBG7 (( "insmodextdep: inserted '%s' \n", dip->file ))

	}

	DBG4 (( "insmodextdep: returning %d \n", 1 ))
	return ( 1 );
}


/*==========================================================================*/

static  int     insert_mod_parentlinkoption 
(
	char                    * linkoption,
	struct mod_com_info     * mcip
)
{
	struct opt_info         ** soipp, * oip;
	int rc = 1; 

	DBG2 (( "insmodplo: called \n" ))
	DBG3 (( "insmodplo: parentlinkoption '%s' \n", linkoption ))


	for ( soipp = &mcip->parentlinkoptions ; *soipp; soipp =  &(*soipp)->next )
	{
	    /* compare options */
	    rc = strcmp ( (*soipp)->option , linkoption );
	    if ( rc >= 0 ) break ;
	    if ( rc < 0 ) continue ;
	}
	
	if ( rc == 0 ) /* option already defined */
	{
	    DBG8 (( "insmodplo: option found '%s' \n", (*soipp)->option ))
	}
	else
	{
		/* look for option in global option list */
		oip = (struct opt_info *) palloc ( sizeof(struct opt_info) );
	    if ( oip == NULL )
	    {
		    DBG0 (( "insufficient dynamic memory \n" ))
			eprotwrite ( "insufficient dynamic memory \n" );
		    return ( -1 );
	    }
		oip->next = *soipp ;
		oip->option = insert_to_optionlist (linkoption);
		if ( oip->option == NULL )
	    {	
			return (-1);
		}
        *soipp = oip ;
	}
	DBG4 (( "insmodplo: returning %d \n", 1 ))
	return ( 1 );
}

/*===================================================================*/

static  char * insert_to_optionlist 
(
	char * option
)
{
	struct opt_info   ** soipp;
	struct opt_info   * oip;
	int rc = 1;

	DBG2 (( "insoptlst: called \n" ))
	DBG3 (( "insoptlst: option '%s' \n", option ))

	for ( soipp = &option_list ; *soipp; soipp =  &(*soipp)->next )
	{
	    /* compare options */
	    rc = strcmp ( (*soipp)->option , option );
	    if ( rc >= 0 ) break ;
	    if ( rc < 0 ) continue ;
	}
	
    if ( rc == 0 )
	{
	    DBG8 (( "insoptlst: option found '%s' \n", (*soipp)->option ))
	}
	else
	{
	    oip = (struct opt_info *) palloc ( sizeof(struct opt_info) );
	    if ( oip == NULL )
	    {
		    DBG0 (( "insufficient dynamic memory \n" ))
			eprotwrite ( "insufficient dynamic memory \n" );
		    return ( NULL );
	    }
		oip->option = (char*) palloc (strlen(option)+1);
		if ( oip == NULL )
	    {
		    DBG0 (( "insufficient dynamic memory \n" ))
			eprotwrite ( "insufficient dynamic memory \n" );
		    return ( NULL );
	    }
		strcpy (oip->option, option);
        oip->next = *soipp ;
        *soipp = oip ;
	}
	return ((*soipp)->option);
}

/*==========================================================================*/

static  int     insert_mod_obj 
(
    char                    * objlist,
    struct mod_com_info     * mcip 
)
{
	int			i ;
	struct obj_info         ** soipp , * oip ;
	char                    * p ;
	char                    objnam [ NAMESIZE ] ;

	DBG2 (( "insmodobj: called \n" ))
	DBG3 (( "insmodobj: objlist '%s' \n", objlist ))

	if ( ! *objlist )
	{
		DBG4 (( "insmodobj: object list is empty \n" ))
		mcip->stat.noobject = 1;
	}
	else
		mcip->stat.noobject = 0;

	for ( soipp = &mcip->objects ; *soipp ; soipp = &(*soipp)->next );

	for ( p = objlist ; * p != '\0' ; )
	{
		for ( ; *p == ',' ; p ++ );
		for ( i = 0 ; i < sizeof(objnam) ; i ++ )
		{
			if ( * p == ',' || * p == '\0' ) break ;
			objnam [ i ] = * p ;
			p ++ ;
		}
		if ( i >= sizeof(objnam) )
		{
			DBG0 (( "dependency module name too long '%s' \n",
								objlist ))
			eprotwrite ( "dependency module name too long '%s' \n",	objlist );
			return ( - 1 );
		}
		objnam [ i ] = '\0' ;
		if ( i == 0 ) continue ; /* e.g. two commas (,,) */
		DBG7 (( "insmodobj: objnam '%s' \n", objnam ))

		oip = (struct obj_info *) palloc ( sizeof(struct obj_info) );
		if ( oip == NULL )
		{
			DBG0 (( "insufficient dynamic memory \n" ))
			eprotwrite ( "insufficient dynamic memory \n" );
			return ( - 3 );
		}
		FILL (  oip , 0 , sizeof(struct obj_info) );

	    if ( ! extension_exists (objnam))
			strcat ( objnam , ".o" );

		oip->name_o = (char *) palloc( strlen( objnam ) + 1 );
		(void) strcpy( oip->name_o, objnam );

		* soipp = oip ;
		soipp = & oip->next ;

		DBG7 (( "insmodobj: inserted '%s' \n", oip->name_o ))

	}

	DBG4 (( "insmodobj: returning %d \n", 1 ))
	return ( 1 );
}


/*==========================================================================*/

static	char	get_language
(
    char		* target 
)
{
	struct  langext_info    * leip ;
	struct  lang_info       * lip ;
	char	                last_char ='\0';
	char	                * ext;
    char                    modname [ NAMESIZE ];
    int                     rc = 0;

    DBG2 (( "getlang: called \n" ))
    DBG3 (( "getlang: target '%s' \n", target ))

    if ( *(ext = get_extension( get_basename( target , modname ) ) ) != '\0' ) 
	{
        if ( ! lngext_lists ) (void) get_langext_info();
        for ( leip = lngext_lists ; leip ; leip = leip->next )
        {
            rc = strcmp( leip->ext , ext );
        	if ( rc >= 0 ) break ;
        }
        if ( rc == 0 ) last_char = leip->language ;
	}
	else
		last_char = modname [ strlen(modname)-1 ] ;

	DBG4 (( "get_language: last_char         '%c' \n", last_char ))

	if ( ! lng_lists ) (void) get_lang_info ();

	for ( lip = lng_lists ; lip ; lip = lip->next )
	{
	    if ( lip->language == last_char ) break ;
	}
	if ( ! lip )
	{
	    DBG4 (( "get_language: default language  '%c' \n", LANG_PASCAL_P[ 0 ] ))
	    return ( 'p' );
	}

	DBG4 (( "get_language: language          '%c' \n", lip->language ))
	return ( lip->language );
}

/*==========================================================================*/

static	int	get_lang_info 
(
)
{
	int		rc=0 ;
	int		errcnt ;
	VFILE   * fp ;
	char	* name = LANG_LST ;
	char    list [ FILENAMESIZE ] ;
	char	line [ LINESIZE ] ;
	char	found_vers ;


	DBG2 (( "getlnginf: called \n" ))

	/*
	 *  Create default information.
	 */
	(void) strcpy          ( line , LANG_PASCAL_T);
	(void) strcat          ( line , " mfpinc mfpexp mfp .p %T comppc" );
	rc = analyze_lang_line ( line );
	if ( rc <= 0 )  return ( rc );
	(void) strcpy          ( line , LANG_PASCAL_P);
	(void) strcat          ( line , " mfpinc mfpexp mfp .p %P compp" );
	rc = analyze_lang_line ( line );
	if ( rc <= 0 )  return ( rc );
	(void) strcpy          ( line , LANG_C);
	(void) strcat          ( line , " mfcinc mfcexp mfc .c %C compc" );
	rc = analyze_lang_line ( line );
	if ( rc <= 0 )  return ( rc );
	(void) strcpy          ( line , LANG_ASM);
	(void) strcat          ( line , " mfainc mfaexp mfa .s %A compa" );
	rc = analyze_lang_line ( line );
	if ( rc <= 0 )  return ( rc );
	(void) strcpy          ( line , LANG_CPP);
	(void) strcat          ( line , " mfcinc mfcexp mfc .cpp %X compc" );
	rc = analyze_lang_line ( line );
	if ( rc <= 0 )  return ( rc );

	(void) strcpy          ( line , LANG_J);
	(void) strcat          ( line , " mfcinc mfcexp mfrc .java .class %J compj" );
	rc = analyze_lang_line ( line );
	if ( rc <= 0 )  return ( rc );

    (void) strcpy          ( line , LANG_RC);
	(void) strcat          ( line , " mfcinc mfcexp mfrc .rc %R comprc" );
	rc = analyze_lang_line ( line );
	if ( rc <= 0 )  return ( rc );

	DBG8 (( "getlnginf: search description file '%s' \n", name ))
	rc = get_dsc_file ( name , vmakeversion , list , & found_vers, &lng_date, 0 );
	if ( rc <= 0 )
	{
	    if ( rc == 0 )
		DBG4 (( "getlnginf: description file not found '%s' \n", name ))
	    else
		DBG8 (( "getlnginf: get_dsc_file error %d \n", rc ))
	    return ( rc );
	}
	DBG4 (( "getlnginf: description file found     '%s' \n", list ))

	fp = fileopen ( list , "r" );
	if ( ! fp )
	{
	    DBG0 (( "cannot open '%s' \n", list ))
		eprotwrite ( "cannot open '%s' \n", list );
	    return ( -2 );
	}
	DBG8 (( "getlnginf: opened '%s' \n", list ))

	errcnt = 0 ;
	for ( ; ; )
	{
	    rc = get_dsc_line ( line , sizeof(line) , fp , list );
	    if ( rc < 0 )
	    {
		    DBG4 (( "getlnginf: get_line error %d \n" , rc ))
		    (void) concom ( (char*) 0 , list );
		    (void) fileclose ( fp );
		    return ( rc ) ;
	    }
	    if ( rc == 0 ) break ;

	    rc = analyze_lang_line ( line );
	    if ( rc < 0 ) errcnt ++ ;
	}

	rc = concom ( (char*) 0 , list );
	(void) fileclose ( fp );
	if ( rc < 0 ) 
		errcnt++ ;

	if ( errcnt )
	{
	    DBG8 (( "getlnginf: lang file list collect errors %d \n", errcnt ))
	    return ( -1 );
	}

	DBG4 (( "getlnginf: returning %d \n", 1 ))
	return ( 1 );
}

/*==========================================================================*/

static	int	get_langext_info 
(
)
{
	int		rc=0 ;
	int		errcnt ;
	VFILE   * fp ;
	char	* name = LANGEXT_LST ;
	char    list [ FILENAMESIZE ] ;
	char	line [ LINESIZE ] ;
	char	found_vers ;

	DBG2 (( "getlnginf: called \n" ))

	/*
	 *  Create default information.
	 */
	(void) strcpy          ( line , "x(C++) cpp,hpp" );
	rc = analyze_langext_line ( line );
	if ( rc <= 0 )  return ( rc );
	(void) strcpy          ( line , "c(C) c,h " );
	rc = analyze_langext_line ( line );
	if ( rc <= 0 )  return ( rc );
	(void) strcpy          ( line , "t(Pascal) t" );
	rc = analyze_langext_line ( line );
	if ( rc <= 0 )  return ( rc );
	(void) strcpy          ( line , "p(Pascal) p" );
	rc = analyze_langext_line ( line );
	if ( rc <= 0 )  return ( rc );
	(void) strcpy          ( line , "a(Assembler) s" );
	rc = analyze_langext_line ( line );
	if ( rc <= 0 )  return ( rc );
	(void) strcpy          ( line , "r(Resources) rc,ico,def,mc,dlg,idl,ycc,lex,rgs" );
	rc = analyze_langext_line ( line );
	if ( rc <= 0 )  return ( rc );
	(void) strcpy          ( line , "j(Java) java" );
	rc = analyze_langext_line ( line );
	if ( rc <= 0 )  return ( rc );

    DBG8 (( "getlngextinf: search description file '%s' \n", name ))
	rc = get_dsc_file ( name , vmakeversion , list , & found_vers, &lngext_date, 0 );
	if ( rc <= 0 )
	{
	    if ( rc == 0 )
		DBG4 (( "getlngextinf: description file not found '%s' \n", name ))
	    else
		DBG8 (( "getlngextinf: get_dsc_file error %d \n", rc ))
	    return ( rc );
	}
	DBG4 (( "getlngextinf: description file found     '%s' \n", list ))

	fp = fileopen ( list , "r" );
	if ( ! fp )
	{
	    DBG0 (( "cannot open '%s' \n", list ))
		eprotwrite ( "cannot open '%s' \n", list );
	    return ( -2 );
	}
	DBG8 (( "getlnginf: opened '%s' \n", list ))

	errcnt = 0 ;
	for ( ; ; )
	{
	    rc = get_dsc_line ( line , sizeof(line) , fp , list );
	    if ( rc < 0 )
	    {
		    DBG4 (( "getlnginf: get_line error %d \n" , rc ))
		    (void) concom ( (char*) 0 , list );
		    (void) fileclose ( fp );
		    return ( rc ) ;
	    }
	    if ( rc == 0 ) break ;
	    rc = analyze_langext_line ( line );
	    if ( rc < 0 ) errcnt ++ ;
	}

	rc = concom ( (char*) 0 , list );
	(void) fileclose ( fp );
	if ( rc < 0 ) 
		errcnt++ ;

	if ( errcnt )
	{
	    DBG8 (( "getlngextinf: langext file list collect errors %d \n", errcnt ))
	    return ( -1 );
	}

	DBG4 (( "getlngextinf: returning %d \n", 1 ))
	return ( 1 );
}

/*==========================================================================*/

static  int     analyze_lang_line 
(
    char                    * lngline 
)
{
	int                 rc=0 ;
	struct lang_info	* lip ;
	struct lang_info    ** slipp ;
	char                * lng ;
	char                * ext ;
	char                * opt ;
	char                * inc ;
	char                * exp ;
	char                * mod ;
	char                * cmp ;
	register char		* p ;

	char                * obj = std_objectext;   /* CTS 1104953 */

    DBG2 (( "analnglin: called \n" ))
	DBG3 (( "analnglin: lngline '%s' \n", lngline ))

	/*
	 *  Find the language character
	 */
	for ( p = lngline ; isspace(*p) ; p ++ );
	if ( * p == '#' )
	{
	    DBG7 (( "analnglin: comment line \n" ))
	    return ( 0 );
	}
	if ( ! isalpha ( * p ) )
	{
	    DBG0 (( "syntax error in lang file: '%s' \n", lngline ))
		eprotwrite ( "syntax error in lang file: '%s' \n", lngline );
	    return ( -1 );
	}
	lng = p ;
	for ( ; *p && ! isspace(*p) ; p ++ );
	if ( ! * p )
	{
	    DBG0 (( "syntax error in lang file: '%s' \n", lngline ))
		eprotwrite ( "syntax error in lang file: '%s' \n", lngline );
	    return ( -1 );
	}
	* p = '\0' ; /* terminate 'lng' */

	/*
	 *  Find the unpacker for includes
	 */
	for ( p ++ ; isspace(*p) ; p ++ );
	inc = p ;
	for ( ; * p && ! isspace(*p) ; p ++ );
	if ( ! * p )
	{
	    DBG0 (( "syntax error in lang file: '%s' \n", lngline ))
		eprotwrite ( "syntax error in lang file: '%s' \n", lngline );
	    return ( -1 );
	}
	* p = '\0' ; /* terminate 'inc' */

	/*
	 *  Find the unpacker for exports
	 */
	for ( p ++ ; isspace(*p) ; p ++ );
	exp = p ;
	for ( ; * p && ! isspace(*p) ; p ++ );
	if ( ! * p )
	{
	    DBG0 (( "syntax error in lang file: '%s' \n", lngline ))
		eprotwrite ( "syntax error in lang file: '%s' \n", lngline );
	    return ( -1 );
	}
	* p = '\0' ; /* terminate 'inc' */

	/*
	 *  Find the unpacker for modules
	 */
	for ( p ++ ; isspace(*p) ; p ++ );
	mod = p ;
	for ( ; * p && ! isspace(*p) ; p ++ );
	if ( ! * p )
	{
	    DBG0 (( "syntax error in lang file: '%s' \n", lngline ))
		eprotwrite ( "syntax error in lang file: '%s' \n", lngline );
	    return ( -1 );
	}
	* p = '\0' ; /* terminate 'mod' */

	/*
	 *  Find the file name extension
	 */
	for ( p ++ ; isspace(*p) ; p ++ );
	if ( * p != '.' )
	{
	    DBG0 (( "syntax error in lang file: '%s' \n", lngline ))
		eprotwrite ( "syntax error in lang file: '%s' \n", lngline );
	    return ( -1 );
	}
	p ++ ;
	ext = p ;
	for ( ; * p && ! isspace(*p) ; p ++ );
	if ( ! * p )
	{
	    DBG0 (( "syntax error in lang file: '%s' \n", lngline ))
		eprotwrite ( "syntax error in lang file: '%s' \n", lngline );
	    return ( -1 );
	}
	* p = '\0' ; /* terminate 'ext' */

	/*
	 *  Find the option name
	 */
	for ( p ++ ; isspace(*p) ; p ++ );

	/* CTS 1104953 -> */
	if ( *p == '.' )
	{
		obj = p;
		for ( p ++ ; ! isspace(*p) ; p ++ );
		if ( ! * p )
		{
			DBG0 (( "syntax error in lang file: '%s' \n", lngline ))
			eprotwrite ( "syntax error in lang file: '%s' \n", lngline );
			return ( -1 );
		}
		* p = '\0' ; /* terminate 'obj' */

		for ( p ++ ; isspace(*p) ; p ++ );
	}
	/* <- CTS 1104953 */

	if ( * p != '%' )
	{
	    DBG0 (( "syntax error in lang file: '%s' \n", lngline ))
		eprotwrite ( "syntax error in lang file: '%s' \n", lngline );
	    return ( -1 );
	}
	p ++ ;
	if ( ! isalpha ( * p ) )
	{
	    DBG0 (( "syntax error in lang file: '%s' \n", lngline ))
		eprotwrite ( "syntax error in lang file: '%s' \n", lngline );
	    return ( -1 );
	}
	opt = p ;
	for ( ; * p && ! isspace(*p) ; p ++ );
	if ( ! * p )
	{
	    DBG0 (( "syntax error in lang file: '%s' \n", lngline ))
		eprotwrite ( "syntax error in lang file: '%s' \n", lngline );
	    return ( -1 );
	}
	* p = '\0' ; /* terminate 'opt' */

	/*
	 *  Find the compiler
	 */
	for ( p ++ ; isspace(*p) ; p ++ );
	cmp = p ;
	for ( ; * p && ! isspace(*p) ; p ++ );
	* p = '\0' ; /* terminate 'cmp' */

	DBG7 (( "analnglin: language  '%c' \n", * lng ))
	DBG7 (( "analnglin: suffix    '.%s' \n", ext ))
	DBG7 (( "analnglin: optchar   '%%%c' \n", *opt ))
	DBG7 (( "analnglin: unpackinc '%s' \n", inc ))
	DBG7 (( "analnglin: unpackexp '%s' \n", exp ))
	DBG7 (( "analnglin: unpackmod '%s' \n", mod ))
	DBG7 (( "analnglin: compiler  '%s' \n", cmp ))
	DBG7 (( "analnglin: objectext  %s' \n", obj ))  /* CTS 1104953 */

	/*
	 *  Find the position for this language in the list.
	 */
	rc = -1 ;
	for ( slipp = & lng_lists ; * slipp ; slipp = & (*slipp)->next )
	{
	    rc = (*slipp)->language - *lng ;
	    if ( rc >= 0 ) break ;
	}
	if ( rc == 0 )
	{
	    DBG8 (( "analnglin: replace old language info \n" ))
	    lip = * slipp ;
		FREE_AND_SET_TO_NULL(lip->unpackinc);
		FREE_AND_SET_TO_NULL(lip->unpackexp);
		FREE_AND_SET_TO_NULL(lip->unpackmod);
		FREE_AND_SET_TO_NULL(lip->compiler);
		FREE_AND_SET_TO_NULL(lip->suffix);
		FREE_AND_SET_TO_NULL(lip->objectext);
	}
	else
	{
	    DBG8 (( "analnglin: allocate new language info \n" ))
	    lip = (struct lang_info *) palloc ( sizeof(struct lang_info) );
	    if ( ! lip )
	    {
		    DBG0 (( "insufficient dynamic memory \n" ))
			eprotwrite ( "insufficient dynamic memory \n" );
		    return ( - 3 );
	    }
	    FILL ( lip , 0 , sizeof(struct lang_info) );

	    /* insert it into the list */
	    lip->next = * slipp ;
	    * slipp = lip ;
	}

	lip->unpackinc = (char *) palloc( strlen( inc ) + 1 );
	lip->unpackexp = (char *) palloc( strlen( exp ) + 1 );
	lip->unpackmod = (char *) palloc( strlen( mod ) + 1 );
	lip->compiler  = (char *) palloc( strlen( cmp ) + 1 );
	lip->suffix    = (char *) palloc( strlen( ext ) + 1 );
	lip->objectext = (char *) palloc( strlen( obj ) + 1 );

	(void) strcpy ( lip->unpackinc , inc );
	(void) strcpy ( lip->unpackexp , exp );
	(void) strcpy ( lip->unpackmod , mod );
	(void) strcpy ( lip->compiler  , cmp );
	(void) strcpy ( lip->suffix    , ext );
	(void) strcpy ( lip->objectext , obj );

	lip->optchar  = *opt ;
	lip->language = *lng ;

	DBG4 (( "analnglin: returning %d \n", 1 ))
	return ( 1 );
}

/*==========================================================================*/

static  int     analyze_langext_line 
(
    char                    * lngline 
)
{
	int                 rc=0 ;
	struct langext_info	* lip ;
	struct langext_info ** slipp ;
	char                lng ;
	char                * extensions ;
    char                * ext;
	char		        * p ;

    DBG2 (( "analngextlin: called \n" ))
	DBG3 (( "analngextlin: lngline '%s' \n", lngline ))

	/*
	 *  Find the language character
	 */
	for ( p = lngline ; isspace(*p) ; p ++ );
	if ( * p == '#' )
	{
	    DBG7 (( "analngextlin: comment line \n" ))
	    return ( 0 );
	}
	if ( ! isalpha ( * p ) )
	{
	    DBG0 (( "syntax error in lang file: '%s' \n", lngline ))
		eprotwrite ( "syntax error in lang file: '%s' \n", lngline );
	    return ( -1 );
	}
	lng = * p ;
	for ( ; *p && ! isspace(*p) ; p ++ );
	if ( ! * p )
	{
	    DBG0 (( "syntax error in lang file: '%s' \n", lngline ))
	    eprotwrite ( "syntax error in lang file: '%s' \n", lngline );
		return ( -1 );
	}

	for ( p ++ ; isspace(*p) ; p ++ );
	extensions = p ;

	DBG7 (( "analngextlin: language  '%c' \n", lng ))
	DBG7 (( "analngextlin: extensions '%s' \n", extensions ))

    ext = strtok( extensions, ", " );
    while ( ext != NULL && ext[ 0 ] != '#' )
    {
	    /*
	     *  Find the position for this language in the list.
	     */
	    rc = -1 ;
	    for ( slipp = & lngext_lists ; * slipp ; slipp = & (*slipp)->next )
	    {
	        rc = strcmp( (*slipp)->ext , ext ) ;
	        if ( rc >= 0 ) break ;
	    }
	    if ( rc == 0 )
	    {
		    lip = * slipp ;
	        DBG8 (( "analngextlin: replace old extension info '%s' \n",
                lip->ext ))
	    }
	    else
	    {
	        DBG8 (( "analnglin: allocate new language info \n" ))
	        lip = (struct langext_info *) palloc ( sizeof(struct langext_info) );
	        if ( ! lip )
	        {
		        DBG0 (( "insufficient dynamic memory \n" ))
				eprotwrite ( "insufficient dynamic memory \n" );
		        return ( - 3 );
	        }
	        FILL ( lip , 0 , sizeof(struct langext_info) );

            lip->ext = (char *) palloc( strlen( ext ) + 1 );
            (void) strcpy( lip->ext , ext );

    	    DBG7 (( "analngextlin: extension '%s' added \n", lip->ext ))

            /* insert it into the list */
	        lip->next = * slipp ;
	        * slipp = lip ;
	    }
	    lip->language = lng ;

        /* get next extensions */
        ext = strtok( NULL , ", " );
    }

	DBG4 (( "analngextlin: returning %d \n", 1 ))
	return ( 1 );
}

/*==========================================================================*/

static  int     get_dsc_file 
(
    char    * name  ,
    char    version ,
    char    * file  ,
    char	* found_vers, 
	time_t  * filedate,
	int     is_com_description
)
{
	int             rc=0 ;
	int			    status ;
	time_t			date ;
	struct dsc_info	*dip ;
	struct dsc_info	**sdipp ;
	char			* vers = EMPTY ;
	char			* myfile = NULL;
	char			path   [ FILENAMESIZE ];
    char            * layer = NULL;
    char            name_dsc [ NAMESIZE ] = "";
    valid_bit_arr	vbits ;
    char            * gfile = NULL;
	int             qual_desc;
	int             found_in_src = 0;
	

    DBG2 (( "getdscfil: called \n" ))
	DBG3 (( "getdscfil: name    '%s' \n", name    ))
	DBG3 (( "getdscfil: version '%c' \n", version ))

    VALID_BIT_FILL ( vbits );

    FILL( name_dsc , 0 , NAMESIZE );
    FILL( path , 0 , FILENAMESIZE );
    qual_desc = is_qualified_desc( name, &layer, name_dsc );
	if ( qual_desc )
    {
        if ( layer )
        {
            (void) sprintf( path, DIR_NEWDSCR, "%s", "%s", layer );
        }
        else
            (void) strcpy( path , DIR_DSCR );
    }
    else
    {
        (void) strcpy( name_dsc , name );
        (void) strcpy( path , DIR_DSCR );
    }

    /* new behavior */
    /* look first in %/sys/desc/ if its was found take it */
	* found_vers = ' ' ;
	rc = get_file ( name_dsc , path , EMPTY , EMPTY ,
		FK_SOURCE | FK_LEVEL , vbits ,
		&gfile , & date , & status, opt_nodircache );

    if ( ! rc ) switch ( version )
	{
	case 's' :
		* found_vers = 's' ;
		vers = SLOW ;
		rc = get_file ( name_dsc , path , vers , EMPTY ,
			FK_SOURCE | FK_LEVEL , vbits ,
			&gfile , & date , & status, opt_nodircache );
        if ( rc )
        {
            DBG1(( "info: obsolete directory 'slow' for description '%s'\n", name_dsc ))
            DBG1(( "      use only one description in $DESC \n" ))
        }
		if ( rc ) break ;
		/*FALLTHROUGH*/

	case 'q' :
		* found_vers = 'q' ;
		vers = QUICK ;
		rc = get_file ( name_dsc , path , vers , EMPTY ,
			FK_SOURCE | FK_LEVEL , vbits ,
			&gfile , & date , & status, opt_nodircache );
        if ( rc )
        {
            DBG1(( "info: obsolete directory 'quick' for description '%s'\n", name_dsc ))
            DBG1(( "      use only one description in $DESC \n" ))
        }
		if ( rc ) break ;
		/*FALLTHROUGH*/

	default :
		* found_vers = 'f' ;
		vers = FAST ;
		rc = get_file ( name_dsc , path , vers , EMPTY ,
			FK_SOURCE | FK_LEVEL , vbits ,
			&gfile , & date , & status, opt_nodircache );
        if ( rc )
        {
            DBG1(( "info: obsolete directory 'fast' for description '%s'\n", name_dsc ))
            DBG1(( "      use only one description in $DESC \n" ))
        }
		if ( rc ) break ;
		/*FALLTHROUGH*/
	}

	if ( !rc )
	{
		/* look in src path */
		FILL( path , 0 , FILENAMESIZE );
		if ( qual_desc )

			(void) sprintf( path, DIR_SRC, "%s", layer ? layer : "" );	
		else
			(void) strcpy( path , DIR_SRC );


		if ( ! strcmp (name_dsc + ( strlen (name_dsc) - 4 ) , ".com"))
		{
			strncat(path, name_dsc, strlen (name_dsc) - 4);
			strcat(path, "/");
		}

		rc = get_file ( name_dsc , path , EMPTY , EMPTY ,
			FK_SOURCE | FK_LEVEL , vbits ,
			&gfile , & date , & status, opt_nodircache );

		found_in_src = 1;
		/* add com-name to layer */
		if (is_com_description )
		{
			char            *p,*ptemp ;

			ptemp = strchr(name_dsc, '.');
			p = (char *) palloc ( (layer ? strlen(layer)+ 1 : 0 ) + (ptemp - name_dsc) + 1); 
			if (layer)
			{
				strcpy(p, layer);
				strcat(p, "/");
			}
			else
				*p='\0';
			strncat(p, name_dsc, (ptemp - name_dsc));
			FREE_AND_SET_TO_NULL(layer);
			layer = p;
		}
	}

	if ( rc > 0 )
	{
	    if ( strlen( gfile ) > FILENAMESIZE - 1 )
        {
	        DBG0 (( "filename too long '%s' \n", gfile ))
			eprotwrite ( "filename too long '%s' \n", gfile );
	        return ( - 2 );
        }
        
        (void) strcpy( file, gfile );

        DBG1 (( "%s %s \n", ascdate(date) , file ))
		*filedate = date;
		

		if (found_in_src)
		{
			if ( layer && * layer )
            {
                myfile = (char *) palloc( strlen( layer ) + 
                        strlen( name_dsc ) + 2  + 4 );
				(void) sprintf ( myfile , "src/%s/%s" , layer, name_dsc );
            }
			else
            {
                myfile = (char *) palloc( strlen( name_dsc ) + 1 +4  );
				(void) sprintf ( myfile , "src/%s" , name_dsc );
            }
		}
		else
		{

			if ( * found_vers != ' ' )
			{
				/* &gar PTS 1103893 */
				if ( layer && * layer )
				{
					myfile = (char *) palloc( strlen( vers ) + strlen( layer ) + 
							strlen( name_dsc ) + 2 + 1 + 5 );
					(void) sprintf ( myfile , "desc/%s/%s/%s" , vers , layer, name_dsc );
				}
				else
				{
					myfile = (char *) palloc( strlen( vers ) + 
							strlen( name_dsc ) + 1 + 1 +5  );
					(void) sprintf ( myfile , "desc/%s/%s" , vers , name_dsc );
				}
			}		
			else
			{
				if ( layer && * layer )
				{
					myfile = (char *) palloc( strlen( layer ) + 
							strlen( name_dsc ) + 1 + 1 + 5 );
					(void) sprintf ( myfile , "desc/%s/%s" , layer, name_dsc );
				}
				else
				{
					myfile = (char *) palloc( strlen( name_dsc ) + 1 + 5  );
					(void) sprintf ( myfile , "desc/%s" , name_dsc );
				}
			}
		}

	    rc = -1 ;
	    for ( sdipp = & desc_list ; *sdipp ; sdipp = & (*sdipp)->next )
	    {
		    rc = strcmp ( (*sdipp)->descriptor , myfile );
		    if ( rc >= 0 ) break ;
	    }
	    if ( rc )
	    {
            /* not found in internal list -> insert */
		    dip = (struct dsc_info *) palloc ( sizeof(struct dsc_info) );
		    if ( dip == NULL )
		    {
		        DBG0 (( "insufficient dynamic memory \n" ))
				eprotwrite ( "insufficient dynamic memory \n" );
		        return ( -3 );
		    }
		    FILL ( dip , 0 , sizeof(struct dsc_info) );

			dip->descriptor = (char *) palloc( strlen( myfile ) + 1 );
		    (void) strcpy ( dip->descriptor , myfile );
			dip->value      = (char *) palloc( 1 );
            dip->value[ 0 ] = '\0';

		    dip->next = * sdipp ;
		    * sdipp = dip ;
	    }

	    /*
	     *  With VMAKE_OPTION=V get all sources into OWN tree.
	     */
	    if ( vopt_source_copy && ! (status & PATH_OWN) )
	    {
		    outstamp ( "%-9s %-5s <%s>\n", EMPTY , EMPTY , file );
            if ( (* found_vers != ' ') || found_in_src  )
				(void) new_file ( name_dsc , path , EMPTY , EMPTY , &myfile );
            else
    		    (void) new_file ( name_dsc , path , vers , EMPTY , &myfile );
		    rc = vcopy_file ( file , myfile , date , FT_TXT );
		    if ( rc ) return ( rc );

            if ( strlen( myfile ) > FILENAMESIZE - 1 )
            {
	            DBG0 (( "filename too long '%s' \n", myfile ))
				eprotwrite ( "filename too long '%s' \n", myfile );
	            return ( - 2 );
            }
        
		    (void) strcpy ( file , myfile );
	    }

	    rc = 1 ;
	}

    FREE_AND_SET_TO_NULL(gfile);
    FREE_AND_SET_TO_NULL(myfile);
    FREE_AND_SET_TO_NULL(layer);

	DBG4 (( "getdscfil: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/


static  int     get_dsc_line 
(
    char    * line  ,
    int		size    ,
    VFILE	* fp    ,
    char	* filename 
)
{
	int			lgt = 0 ;
    short       continued_line = 0;
    char        * p, * q;

    DBG2 (( "getdsclin: called \n" ))

	FILL (  line , 0 , size );
	while ( filegets( line + lgt , size - lgt , fp ) )
	{
	    if ( ! concom(line + lgt , filename ) ) continue ;

        if ( continued_line )
        {
            p = line + lgt;
            while( isspace( * p ) )
            {
                q = p;
                while( * q != '\0' )
                {
                    *q = *(q+1);
                    q++;
                }
            }
        }
	    /*
	     *  Filegets returns the line terminated with
	     *  the newline character and a null byte.
	     *  If the line was too long, it is truncated but still
	     *  null terminated.
	     *  Now, decrement the line length to strip the newline.
	     *  If the resulting length does not point to a newline
	     *  character, then the line was truncated.
	     */
	    lgt += strlen ( line + lgt );

		/* PTS 1105219 -> */
		if ( ( lgt < size ) && (line [ lgt - 1 ] != '\n') ) /* without "\n" */
			line [ lgt ] ='\n';
		else
	 		lgt -- ;
		/* <- PTS 1105219 */

	    if ( line [ lgt ] != '\n' )
	    {
		    DBG0 (( "line too long in '%s': %.40s...\n", filename , line ))
			eprotwrite ( "line too long in '%s': %.40s...\n", filename , line );
		    return ( - 1 );
	    }

	    /*
	     *  Empty lines are of no interest.
	     */
	    if ( lgt < 1 ) continue ;

	    /*
	     *  Continuation lines are introduced by a backslash
	     *  as the very last character of the preceeding line.
	     *  However, we can read the next line only, if there
	     *  are at least 3 bytes left in the buffer:
	     *      At least one character,
	     *      a newline,
	     *      a nullbyte.
	     */
	    if ( line [ lgt - 1 ] == '\\' )
	    {
		    lgt -- ;
		    if ( lgt < size - 3 ) 
            {
                continued_line = 1;
                continue ;
            }
		    DBG0 (( "line too long in '%s': %.40s...\n", filename , line ))
			eprotwrite ( "line too long in '%s': %.40s...\n", filename , line );
		    return ( - 1 );
	    }

	    /*
	     *  At this point we encountered the end of the line.
	     *  No further continuation lines follow.
	     *  The terminating newline has been read, so the line
	     *  is completely in the buffer.
	     */
	    line [ lgt ] = '\0' ;
	    break ;
	}

	return ( lgt ) ;
}


/*==========================================================================*/
/*  substitute:                                                             */
/*--------------------------------------------------------------------------*/
/*  substitute enviroment variables with the real values                    */
/*==========================================================================*/
static  int     substitute 
(
    char                    * name , /* IN  */
    char                    ** path   /* OUT */
)
{
	unsigned        i = 0, j, actlen ;
	char			* p , * q ;
	char			var [ FILENAMESIZE ];

	DBG2 (( "substitute: called \n" ))
	DBG3 (( "substitute: name  '%s' \n", name ))

    actlen = strlen( name );
    *path = (char *) palloc( actlen + 1 );

	/* expand environment variables */
	for ( p = name ; *p ; p ++ , i ++ )
	{
	    /* environment substitution starts with '$' */
	    if ( * p == '$' )
	    {
            unsigned varlen;

		    p ++ ;
            /* treat "$$" as "$" */
            if ( * p == '$' )
            {
    		    (*path) [ i ] = * p ;
            }
            else
            {
                if ( * p == LBRACKET )
                {
                    /* variable name is enclosed by braces {} */
                    p ++ ;
                    for ( j = 0 ; * p != RBRACKET && j < sizeof(var); p ++ , j ++ )
                    {
                        var [ j ] = * p ;
                    }
                    if ( * p == RBRACKET ) p ++ ;
                }
				else
					if (* p == LBRACKET2 )
					{
						/* variable name is enclosed by braces () */
						p ++ ;
						for ( j = 0 ; * p != RBRACKET2 && j < sizeof(var); p ++ , j ++ )
						{
							var [ j ] = * p ;
						}
						if ( * p == RBRACKET2 ) p ++ ;
					}
					else
					{
						/* variable name is terminated by non-symbol-character */
						for ( j = 0 ; 
						( isalnum ( * p ) || * p == '_' ) && j < sizeof(var); 
						p ++ , j ++ 
							)
						{
							var [ j ] = * p ;
						}
					}
                if ( j >= sizeof(var) )
                {
                    DBG0 (( "Error: variable name too long '%.*s' \n", sizeof(var) , var ))
					eprotwrite ( "Error: variable name too long '%.*s' \n", sizeof(var) , var );
                    return ( - 2 );
                }
                var [ j ] = '\0' ;
                
                q = getenv ( var );
                if ( q == NULL )
                {
                    DBG0 (( "Error: cannot substitute variable '%s' \n", var ))
					eprotwrite ( "Error: cannot substitute variable '%s' \n", var );
                    return ( - 2 );
                }
                varlen = strlen( q );
                
                *path = (char *) repalloc( *path, actlen - ( strlen( var ) + 1 ) 
                    + varlen + 1 );
                for ( ; * q != '\0' ; q ++ , i ++ )
                {
                    (*path) [ i ] = * q ;
                }
                actlen = actlen - ( strlen( var ) + 1 ) + varlen;
                
                p -- , i -- ; /* compensate the surrounding loop's increment */
            }
        }
	    else
	    {
		    (*path) [ i ] = * p ;
	    }
	}

	(*path) [ i ] = '\0' ;
	DBG4 (( "substitute: path '%s' \n", *path ))

	return ( 0 );
}

/*==========================================================================*/


#ifdef	NOCONCOM
/*ARGSUSED*/
static	int	concom 
(
    char			* line  ,
    char			* filename 
)
{
	if ( line [ 0 ] != '&' ) return ( 1 );

	return ( 0 );
}
#endif	/*NOCONCOM*/

/*==========================================================================*/

static int insert_file
(
    struct file_info    * sfip,
    char                file [ FILENAMESIZE ],
	char                nodistrib
)
{
	DBG2 (( "insfile: called \n" ))

    while( sfip->next ) sfip = sfip->next;
    if ( sfip->file && *(sfip->file) )
    {
        sfip->next = (struct file_info *) palloc ( sizeof(struct file_info) );
	    if ( sfip->next == NULL )
	    {
		    DBG0 (( "insufficient dynamic memory \n" ))
			eprotwrite ( "insufficient dynamic memory \n") ;
		    return ( -3 );
	    }
        FILL (  sfip->next , 0 , sizeof(struct file_info) );

        sfip->next->file = (char *) palloc( strlen( file ) + 1 );

        (void) strcpy( sfip->next->file, file);
		sfip->next->nodistrib = nodistrib; 	/* CTS 1110367 */
    }
    else
    {
        sfip->file = (char *) palloc( strlen( file ) + 1 );
        (void) strcpy( sfip->file, file );
		sfip->nodistrib = nodistrib;  /* CTS 1110367 */
    }
    return( 0 );
}

static int insert_file_to_filelist
(
	struct file_info    ** sfip,
    char                * file,
	char                nodistrib
)
{
	struct file_info		** fipp;
	
	/* if first list without elements set list to second list */
	DBG2 (( "insfile: called \n" ))
	for ( fipp = sfip ; *fipp ; fipp = &(*fipp)->next );
	*fipp =  (struct file_info *) palloc ( sizeof (struct file_info)); 
	(*fipp)->next = NULL;
	(*fipp)->file = file;  
	(*fipp)->nodistrib = nodistrib;  

	return 1;
}

/*==========================================================================*/

global int analyze_source
(
    char	             ** source,				 /* IN  */
    int                   * found_trgtype,		 /* OUT */
	char                 ** local_variant,       /* OUT */
	struct  variant_info ** global_variants,     /* OUT */
	struct variant_info   * caller_variants     /* IN  */
	/* char                 ** basename,             OUT */
)
{
    char                * p, * q ;
    int                 rc;
    int                 found_type;
	char                * source_subst = NULL;
	

	substitute( *source, &source_subst );   /* PTS 1002075 */
	
	rc = get_variants( source_subst, local_variant, global_variants);
	if (rc < 0)
		return rc;
	if (caller_variants)
	{
		/* rc = append_global_variants( global_variants, caller_variants );*/
		rc = insert_variants( global_variants, caller_variants );
		if (rc < 0)
			return rc;
	}
	/* local variant already in the string */
	if ( * global_variants )
	{
		
		rc = check_variants (*local_variant, *global_variants) ;
		if (rc < 0)
			return -3;
		rc = build_full_variant_name ( &source_subst, *local_variant, *global_variants );
		if (rc < 0)
			return rc;
	}

	/* now source_subst should be the full name (with all variants in it) */
	/* -> at first the local variant and then global variants in alphabetical order */

	FREE_AND_SET_TO_NULL(*source);
	* source = source_subst;

    /* len = strlen( *source ); */
    p = get_extension( *source );
    found_type = TT_UNKNOWN;
	/*
    if ( name )
    {
        (void) strncpy( name, *source , NAMESIZE - 1 );
        name[ (len < NAMESIZE) ? len : NAMESIZE - 1 ] = '\0';
    }
	*/
/*
    if ( is_valid_sourcename( *source, layer, name ) )
        return( GTT_SRC );
		*/
	if (((*source)[strlen(*source)-1] == '*') || strstr(*source, "/*.") || strstr(*source, "_*") || strchr(*source, '*') ) 
	{
		*found_trgtype = TT_DIRLIST;
		return (GTT_LIST);
	}

	if ( is_valid_sourcename( *source, NULL, NULL ) )
        return( GTT_SRC );

    if ( ( q = strrchr( *source, '/' ) ) == NULL && is_std_ext( p ) )
        return( GTT_SRC );

	if ( ( q = strrchr( *source, '/' ) ) && isalpha(**source) && is_std_ext( p ) )
		return( GTT_SRC );

    if ( *p )
    {
        /* check for known types */
        if      ( !strcmp( p, MAC_LST ) )
        {
            found_type = TT_MAC;
        }
        else if ( !strcmp( p, PRJ_LST ) )
        {
            found_type = TT_PRJ;
        }
        else if ( !strcmp( p, LNK_LST ) )
        {
            found_type = TT_PGM;
        }
        else if ( !strcmp( p, SHM_LST ) )
        {
            found_type = TT_SHM;
        }
        else if ( !strcmp( p, DLL_LST ) )
        {
            found_type = TT_DLL;
        }
        else if ( !strcmp( p, SHR_LST ) )
        {
            found_type = TT_SHR;
        }
        else if ( !strcmp( p, REL_LST ) )
        {
            found_type = TT_REL;
        }
        else if ( !strcmp( p, LIB_LST ) )
        {
            found_type = TT_LIB;
        }
        else if ( !strcmp( p, JP_LST ) )
        {
            found_type = TT_JP;
        }
        else if ( !strcmp( p, COM_LST ) )
        {
            found_type = TT_COM;
        }
    }

    if ( found_trgtype ) * found_trgtype = found_type;

   /* if ( is_valid_descname( *source, layer, name )  ) */
	if ( is_valid_descname( *source, NULL, NULL )  )
    {
        int rc = GTT_DESC;
        if ( found_type != TT_UNKNOWN ) 
        {
            rc = GTT_KNOWN_DESC;
        }
        return( rc );
    }

    return( GTT_UNKNOWN );
}

/*==========================================================================*/

static int is_std_ext
(
    char	* ext
)
{
    int rc = -1;
    struct langext_info * leip;

    if ( ! lngext_lists ) (void) get_langext_info();
    for ( leip = lngext_lists ; leip ; leip = leip->next )
    {
        rc = strcmp( leip->ext , ext );
        if ( rc >= 0 ) break ;
    }
    if ( rc == 0 ) 
        return ( 1 );
    else
        return( 0 );
}

/*==========================================================================*/
/* CTS 1110367 */
/* check for (nodistrib) in the filename -> return 1 */
/* and insert \0 after the filename - before (nodistrib) */
/*==========================================================================*/
char check_nodistrib 
(
	char *  filename
)
{
	char      * p;
	p = strstr (filename, TRGDSC_FILE_NODISTRIB);
	if ( ! p )
		return 0;
	*p = '\0';
	return 1;
}

/*==============================================================*/
/* initialize for mci: local_variant, allvariants, pure_target */
static int analyze_com_variants 
( 
	struct mod_com_info     * mcip 
)
{
	char			    line [ LINESIZE ] ;
	char              * found_variant, *p;
	int				    rc = 0;
		
	/* init pure name without variants */
	(void) get_basename(mcip->name, line);
	mcip->purename = (char *) palloc ( strlen(line) + 1 );
	strcpy (mcip->purename, line);
	mcip->name_without_global_variants = (char *) palloc ( strlen(line) + 1 );
	strcpy (mcip->name_without_global_variants, line);
	cut_global_variants (mcip->name_without_global_variants);

	/* init local variant */
	found_variant = strchr (mcip->name, '-'); 
	if ( found_variant )
	{
		/*
		if (vopt_new_variant_handling) 
			DBG0 (( "Warning: found old variant definition '%s' in '%s.com'\n", mcip->name, mcip->defaultlayer))  
			*/
		found_variant++; 
		for ( p = found_variant; *p; p++)
			if ( ! (isalnum (*p) || (*p=='_')) )
				break;
		mcip->localvariant = (char *) palloc ( p - found_variant + 1);
		strncpy ( mcip->localvariant, found_variant, p - found_variant);
		mcip->localvariant [p - found_variant] = '\0';
	}
	else
		mcip->localvariant = NULL;

	/* init all variants in form ":variant(...)" */
	found_variant = strstr(mcip->name, NEW_VARIANT_DEFINITION);
	if ( found_variant )
	{
		found_variant += strlen(NEW_VARIANT_DEFINITION);
		if (*found_variant != LBRACKET2)
			return -3;
		for ( p = ++found_variant;  p && ( *p != RBRACKET2 ) ; p++)
		{
			if ( ! (isalnum (*p) || (*p=='_') || (*p=='!') )  )
				if ( *p != VARIANT_SEPARATOR )
					return -3;
				else
				{
					if ( found_variant == p )
						return -3;
					*p = '\0';
					if ( *found_variant == '!')
						rc = insert_variant(& (mcip->novariants), found_variant + 1, 1);
					else
						rc = insert_variant(& (mcip->allvariants), found_variant, 1);
					*p = VARIANT_SEPARATOR;
					p++;
					found_variant = p;
				}
		}
		if (rc >= 0)
			if ( *p != RBRACKET2 )
				return -3;
			else
			{
				if ( found_variant == p )
					return -3;
				*p = '\0';
				if ( *found_variant == '!')
					rc = insert_variant(& (mcip->novariants), found_variant + 1, 1);
				else
					rc = insert_variant(& (mcip->allvariants), found_variant, 1);
				*p = RBRACKET2;
			}
	}

	return rc;
}

/* update comtypes for lines in com descriptions :
COM_NORMAL_MODULE			ven70c (matches all variants of it if there is'nt a xxx-<variant> definition 
COM_OLDVARIANT_MODULE		ven70c-m (match only local variant 
COM_NEWVARIANT_MODULE	    ven70c:variant(...) 
COM_GLOBAL					globals.com 
COM_WILDCARD				match all 
COM_OLDVARIANT_WILDCARD     match only local variants
COM_NEWVARIANT_WILDCARD     match local and gloab variants 
COM_REGEXP					regular expression
*/
static int analyze_comtype
(
	struct mod_com_info     * mcip
)
{	
	/* Wildcard */
	if ( ! mcip->comtype )
		if (strchr(mcip->name, '*'))
			if (strchr(mcip->name, '-'))
				mcip->comtype = COM_OLDVARIANT_WILDCARD;
			else
				if (strstr(mcip->name, ":variant("))
					mcip->comtype = COM_NEWVARIANT_WILDCARD;
				else
					mcip->comtype = COM_WILDCARD;
		else
			if (strchr(mcip->name, '-'))
				mcip->comtype = COM_OLDVARIANT_MODULE;
			else
				if (strstr(mcip->name, ":variant("))
					mcip->comtype = COM_NEWVARIANT_MODULE;
				else
					mcip->comtype = COM_NORMAL_MODULE;
	return mcip->comtype;
}


static int clear_descriptions 
( 
 struct trg_info       * tip 
)
{
	int rc;
    struct dep_info         *dip ;
	struct dem_info         *sdmip ;
	struct file_info        *fip;


	/* clear dependencies */
	rc = clear_dependencies ( tip, DT_ALLDEP );
	if ( rc < 0 )
	{
		DBG8 (( "colinc: clear_dep error %d \n", rc ))
		return ( rc );
	}
	/* clear demands */
	while ( tip->objdemands )
    {
        while ( tip->objdemands->list )
	    {
		    dip = tip->objdemands->list ;
		    tip->objdemands->list = dip->next ;
		    DBG7 (( "clrdem: removing '%s' \n", dip->target->name ))
		    FREE_AND_SET_TO_NULL(dip);
	    }
		sdmip = tip->objdemands->next;
		FREE_AND_SET_TO_NULL(sdmip);
    }

	while ( tip->file.next )
    {
        
        fip = tip->file.next;
        tip->file.next = fip->next;
		FREE_AND_SET_TO_NULL(fip->file);
        FREE_AND_SET_TO_NULL(fip);
    }
	return 0;
	
}

global int count_dependencies ( struct trg_info *tip )
{
	struct dep_info * dip = tip->dependencies;
	if (tip->finished)
		return (0);

	tip->finished = 1;
	all_target_counter++;
    for ( dip = tip->dependencies ; dip != NULL ; dip = dip->next )
	{
		count_dependencies (dip->target);
	}
	return (0);
}

/* include_description_lines with condidional compiling 
static int include_description_file
( 
	char                 *filename,    
	struct dscfile_info **dfipp	
)
{
    
		dfipp = &plus_descriptions;
		for (dfip = plus_descriptions; dfip; dfip = dfip->next)
		{
			if (strcmp(dfip->type,file_type) == 0) 
				break; 
			dfipp = &(dfip->next);
		}
		if ( ! dfip )
		{
			dfip = (struct dscfile_info *) palloc( sizeof (struct dscfile_info) ); 
			
			dfip->next = NULL;
			plus_name = (char *) repalloc( filename + 2 ); 

		
			(void) sprintf ( plus_name , "::%s/%s.%s" , PLUS_LAYER, PLUS_NAME, file_type );
			rc = get_dsc_file( plus_name, version , list , & found_vers, &filedate, 0 );
			

			if (rc == 0)
			{
				dfip->lines = NULL;
				dfip->name = NULL;
			}
			else
			{
				dfip->name = (char*) palloc (strlen (list) + 1);
				strcpy (dfip->name, list);
				fp = fileopen ( list , "r" );
				if ( ! fp )
				{
					DBG0 (( "cannot open '%s' \n", list ))
					eprotwrite( "cannot open '%s' \n", list );
					return ( - 2 );
				}
				DBG8 (( "coltrg: opened '%s' \n", list ))
			}
			dlipp = &(dfip->lines);
			FILL (  line , 0 , size );
			
            while ( filegets( line + lgt , size - lgt , fp ) )
			{
				dlip = (struct dscline_info*) palloc (sizeof(struct dscline_info));
				dlip->next = NULL;
				dlip->text = (char *) palloc (strlen(line+lgt) + 2);
				strcpy (dlip->text, line+lgt);
				if (dlip->text[strlen(dlip->text)-1] == '\n')
					dlip->text[strlen(dlip->text)-1] = '\0'; 
				*dlipp = dlip;
				dlipp = &dlip->next;
			}
			*dfipp = dfip;
			(void) fileclose ( fp );
		}

		for (dlip = dfip->lines; dlip; dlip=dlip->next)
		{
			if ( ! concom(dlip->text , dfip->name ) ) continue ;
			p = dlip->text;
			
			while( isspace( * p ) ) p++;

			if ( continued_line )
			{
                new_line = (char*) repalloc (new_line, strlen(new_line) + strlen(p));
				q = new_line + strlen(new_line) - 1;
			}
			else
			{
				
				if (strlen(p) == 0)
					continue;
				new_line = (char*) palloc (strlen(p));
				q = new_line;
			}
			strcpy (q, dlip->text);
			if (dlip->text[strlen(dlip->text)] == '\\')
			{
				continued_line = 1;
			}
			else
			{
				continued_line = 0;
				lip = (struct list_info*) palloc ( sizeof(struct list_info) + strlen(dlip->text) );
				if ( ! lip )
				{
					DBG0 (( "insufficient dynamic memory \n" ))
					eprotwrite ( "insufficient dynamic memory \n" );
					(void) concom ( (char*) 0 , list );
					return ( -3 );
				}
				FILL ( lip , 0 , sizeof(struct list_info) + strlen(dlip->text));
				COPY ( lip->line , dlip->text , strlen(dlip->text) + 1 );
				* tailp = lip ;
				tailp = &(lip->next);
			}
		}
		rc = concom ( (char*) 0 , list );  
		if (rc < 0 )
			errcnt++;


*/