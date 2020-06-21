/*      vmakmake.c       

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

#include "devconfig.h"

#include        "vmakmake.h"
#include        "vmake.h"
#include        "vmakvars.h"
#include        "vmakmisc.h"
#include        "vmakcomm.h"
#include        "vmakdist.h"


/* local functions */

#ifndef	RELOC_LIB
static	int			search_library  (struct trg_info*);
static	int			make_library    (struct trg_info*);
#endif	/*RELOC_LIB*/
static	int			insert_member   (char*,time_t,struct trg_info*,int);

static	int			make_include    (struct trg_info*,time_t,int, int);
static	int			make_pascal_inc (struct trg_info*,time_t,int, int);	
static	int			make_other_inc  (struct trg_info*,time_t,int, int);
static	int			make_module     (struct trg_info*,struct trg_info*,time_t,int, int);
static	int			make_reloc_library (struct trg_info*);
static	int			make_reloc      (struct trg_info*);
static	int			make_dll        (struct trg_info*);
static	int			make_program    (struct trg_info*);
static	int			make_extra      (struct trg_info*,time_t,int);
static	int			make_javaproject(struct trg_info*, time_t);
static	int			make_libpgm_dates_file (struct trg_info*);
static  int         make_dirlist (struct trg_info *tip);

static	int			unpack_module_frame (struct trg_info*,char*,time_t,time_t,int,char*);
static	int			compile_file    (struct trg_info*,char*,int,char*,int,time_t,time_t);
static	int			issue_command   (struct trg_info*,struct trg_info*,int);
static	int			search_file     (struct trg_info*);
static	int			remove_temp_obj (struct dep_info*);
static	int			copy_shrglob    (struct trg_info*,char*);
static	int			list_shrglobs   (struct trg_info*,FILE*);
static  int			list_dates      (struct trg_info*,FILE*);
static	int			read_libpgm_dates_file (struct trg_info*,time_t*);
static	char		* suffix        (struct trg_info*);
static	char	    * csuffix       (struct trg_info*);
static	char		* unpackinc     (struct trg_info*);
static	char		* unpackexp     (struct trg_info*);
static	char		* unpackmod     (struct trg_info*);
static	char		* compiler      (struct trg_info*);
static	char		optchar         (struct trg_info*);

static	int			remove_demands  (struct trg_info*);
static	int			copy_demands    (struct trg_info*);
static	char	    * get_objectext (struct trg_info*);  /* CTS 1104953 */
global  int         set_exec_permissions (char *);       /* CTS 1110662 */


/*==========================================================================*/

global	int     make 
(
    struct trg_info * tip ,
    struct trg_info * ctip ,        /* calling target info pointer */
    int             make_option 
)
{
	int             rc = 0 ;
	int             errcnt ;
	int             tmp_option ;
	int             dep_option ;
	int				found_recursion = 0;
	struct dep_info * dip ;
	struct dep_info * sdip ;
	struct dep_info ** sdipp ;
	struct trg_info * dtip ;
	struct dep_info * recdip;
	struct trg_info * rectip;
	struct dep_info * on_success_dip;
	char   tmp_version_output [14];
	int current_report_percent;
	struct trg_info * savetip;
	
	DBG2 (( "make: called \n" ))
	DBG3 (( "make: target '%s' option %d \n", tip->name , make_option ))

	/* tip->act_caller = ctip; */
	if (strcmp (tip->name, "convert_ptoc_types.pl") == 0 )
	{
			DBG3 (( "convert_ptoc_types.pl found\n" ))
	}

	if (tip->made != 0 && tip->made == INIT_STATE)
	{
		if ( (tip->type == TT_MOD ) || (tip->type == TT_INC) )
		{
		    DBG0 (( "WARNING: target '%s/%s' dependent\n", tip->layer, tip->name )) 
		    DBG0 (( "         from itself over '%s/%s'\n",ctip->layer, ctip->name ))	
			
		    recdip = (struct dep_info *) palloc ( sizeof(struct dep_info) );
			if ( recdip == NULL )
				return (ewrite_all (tip, 0, -3, "ERROR: insufficient dynamic memory \n" ));
			FILL (  recdip , 0 , sizeof(struct dep_info) );
			if ( ctip != tip )
			{
				int count = 0;
				for ( rectip = ctip; rectip && rectip->act_caller != tip && count++ < 10 ; rectip = rectip->act_caller)
				{
					DBG0 (("                     over '%s/%s'\n", rectip->act_caller->layer, rectip->act_caller->name )) 
				}
			}
			else
			{
				rectip = ctip;	
				DBG0 ((" ctip == tip (%s) \n", tip->name ))
			}
			if ( rectip )
			{
				for ( sdipp = & tip->rec_callers ; *sdipp && (*sdipp)->target != rectip ;sdipp = &(*sdipp)->next );
				if ( ! (*sdipp))
				{
					recdip->target = rectip;
					*sdipp = recdip ;
				}
				else
				{	FREE_AND_SET_TO_NULL(recdip); }

			}
			else
			{
				DBG0 ((" Internal Problem: found open recursion branch \n" ))
			}
		}
		else
		   DBG0 (( "WARNING: target '%s' dependent from itself over '%s'\n",tip->name, ctip->name ))
		found_recursion=1;
	}

	if ( tip->made != 0 &&  !( tip->stat.forcemake && tip->made < 2) && ! found_recursion )
	{
	    DBG8 (( "make: already made %d \n", tip->max_depmade > tip->made ? tip->max_depmade : tip->made ))
	    /* PTS 1103633 */
	    return ( tip->max_depmade > tip->made ? tip->max_depmade : tip->made );
	}

	/*
	 *  For bigger targets write a time stamp to let the user know,
	 *  when this target started.
	 *  This might facilitate to estimate the duration of building
	 *  this target.
	 */
	if ( (tip->type >= TT_LIB) && (tip->type <= TT_MAC) )
	{
	    if ( vopt_timestamp )
    		outstamp_add ( "\n%s (%s started)\n", ascdate(time((time_t*)0)) , tip->name );
	    else
    		DBG1 (( "\n%s (%s started)\n", ascdate(time((time_t*)0)) , tip->name ))
	}

	/*
	    The insertion times of the library members have to be
	    collected before the respective members are made
	    (as dependencies). This is true because the modification
	    times of the module's source and includes have to be
	    compared against the modification time of the module's
	    object, which in the case of a library member is it's
	    insertion time.
	*/

/* 	open_xml_tag (XML_TAG_MAKE, tip); */

#ifndef	RELOC_LIB
	if ( (tip->type == TT_LIB) && ! vopt_reloc_lib )
	{
	    DBG8 (( "make: collecting library members ... \n" ))
	    rc = search_library ( tip );
	    if ( rc < 0 )
	    {
		    tip->made = rc ;
			return (ewrite_all (tip, 0, tip->made, "ERROR: error collecting members of '%s' \n", tip->name ));
	    }
	}
#endif	/*RELOC_LIB*/

	switch ( make_option )
	{
	case MK_FULL :
		dep_option = MK_FULL ;
		if ( opt_nodep && tip->type != TT_MAC )
		{
		    if ( 
             tip->type == TT_REL ||
			 tip->type == TT_SHR ||
			 tip->type == TT_PGM ||
			 tip->type == TT_SHM    )
			    /* link needs dependency object file names */
			    dep_option = MK_NAME ;
		    else
			    dep_option = MK_NONE ;
		}
		break ;
	case MK_NAME :
		/* if only object name requested,
		   do not work on dependencies */
		dep_option = MK_NONE ;
		break ;
	case MK_NONE :
		dep_option = MK_NONE ;
		break ;
	default :
		return (ewrite_all (tip, 0, -1, "ERROR: illegal make option %d \n", make_option ));
	}


	/*
	 *  If there is a cycle in the dependencies
	 *  (e.g. a target includes itself),
	 *  avoid an endless recursion, by initializing
	 *  the 'made' flag.
	 */
	tip->made = INIT_STATE ;

	tip->minpathlvl = PATH_LEVEL ;
	DBG5 (( "make: target '%s' minpathlvl %d ini \n", tip->name , tip->minpathlvl ))
	errcnt = 0 ;
	dtip   = tip ; /* remark newest dependency */
    if ( tip->propagates ) 
    {
        rc = propagate_env( tip->propagates );
		if ( rc ) 
		{ /*close_last_xml_tag(); */ return( rc );}
    }

    if ( tip->dependencies && ( tip->type != TT_INC || tip->stat.level_i ))
	{
		DBG5 (( "Making dependencies for '%s' \n", tip->name ))
	    for ( dip = tip->dependencies ; dip != NULL ; dip = dip->next )
	    {
			if (found_recursion)
			{
				for ( sdipp = & tip->rec_callers ; *sdipp ; sdipp = &(*sdipp)->next )
				{
					DBG1 (("recursion check %s (%p) - %s (%p) \n", (*sdipp)->target->name, (*sdipp)->target,
						dip->target->name, dip->target))
					if ( (*sdipp)->target == dip->target )
					{   
						DBG1 (("ignoring %s because recursion \n", dip->target->name))
						break;
					}
				}
				if ( *sdipp )
					continue;
			}

			if ( errcnt>0) 
			{
				for ( on_success_dip=tip->on_success; on_success_dip != NULL; on_success_dip=on_success_dip->next )
				{
					if ( on_success_dip->target == dip->target )
						break;
				}

				if ( on_success_dip != NULL || dip->target->stat.on_success)
				{
					if (tip->report & (VMAKE_REPORT_MODE_BEGIN|VMAKE_REPORT_MODE_END) )
						DBG0 (("VMAKE REPORT: Make of '%s' skipped because of previous errors (%s)\n" , dip->target->name, ascdate(time((time_t*)0))))
					else
						DBG0 (("'%s' skipped because of previous errors (on_success)\n" , dip->target->name ))
					continue;
				}
			}

			if (tip->report & VMAKE_REPORT_MODE_BEGIN)
			{
				if (dip->target->type == TT_CMD || dip->target->type == TT_EXECUTE)
					DBG0 (("VMAKE REPORT: Command '%s' started (%s)\n" , dip->target->name, ascdate(time((time_t*)0))))
				else
					DBG0 (("VMAKE REPORT: Make of '%s' started (%s)\n" , dip->target->name, ascdate(time((time_t*)0))))
			}

			/* if dependency is in make list (cmd. line), make it full */
			tmp_option = dep_option ;
			if ( tmp_option != MK_FULL )
            {
				for ( sdip = make_list ; sdip != NULL ; sdip = sdip->next )
                {
				    if ( sdip->target == dip->target )
				    {
					    tmp_option = MK_FULL ;
					    break ;
				    }
                }
            }
savetip=dip->target->act_caller;
			dip->target->act_caller=tip;
		    rc = make( dip->target , tip , tmp_option );
dip->target->act_caller=savetip;	
		    if ( rc == INIT_STATE )
		    {
		        DBG0 (( "WARNING: target dependent from itself '%s' \n",
							    dip->target->name ))
		        continue ;
		    }

			
		    if ( rc <= 0 )
		    {
		        DBG1 (( "Error %d making '%s' \n", rc , dip->target->name ))
				if (tip->report & VMAKE_REPORT_MODE_END)
					if ((dip->target->type == TT_CMD) || (dip->target->type == TT_EXECUTE))
						DBG0 (("VMAKE REPORT: Command '%s' started (%s)\n" , dip->target->name, ascdate(time((time_t*)0))))
					else
						DBG0 (("VMAKE REPORT: Make of '%s' failed (%s)\n" , dip->target->name, ascdate(time((time_t*)0))))
					

		        errcnt ++ ;
				if (vopt_break_after_first_error)
					break;
		    }
		    else
		    {
		        DBG8 (( "make: made '%s' \n", dip->target->name ))
		        DBG5 (( "make: target '%s' minpathlvl %d dip \n",
			        dip->target->name , dip->target->minpathlvl ))
		        if ( dip->target->minpathlvl < tip->minpathlvl )
		        {
			        tip->minpathlvl = dip->target->minpathlvl ;
			        DBG5 (( "make: target '%s' minpathlvl %d now \n",
				        tip->name , tip->minpathlvl ))
		        }
				if (dip->target->stat.no_dep_remake)
				{
					DBG1 (("remake check for callers of '%s' is switched off (no_dep_remake)\n", dip->target->name ))
				}
				else
				{
					if ( rc > tip->max_depmade ) tip->max_depmade = rc ;
					/* set dtip to the newest dependency */
					if ( dtip->date < dip->target->date )
			    	    dtip = dip->target ; /* remark newest dependency */
				}
				if (tip->report & VMAKE_REPORT_MODE_END)
					if ((dip->target->type == TT_CMD) || (dip->target->type == TT_EXECUTE))
						DBG0 (("VMAKE REPORT: Command '%s' successfully finished (%s)\n" , dip->target->name, ascdate(time((time_t*)0))))
					else
						DBG0 (("VMAKE REPORT: Make of '%s' successfully finished (%s)\n" , dip->target->name, ascdate(time((time_t*)0))))
		    }
			if ( (env_VMAKE_REPORT_PERCENT > 0) || (tip->report & VMAKE_REPORT_MODE_PERCENT))
			{
				current_report_percent = (int) (((100*finished_target_counter)/all_target_counter)+ 0.5);
				if ((env_VMAKE_REPORT_PERCENT > 0) && (current_report_percent >= next_report_percent))
                {
					printf ("VMAKE REPORT:  %d %% of targets finished (%s)\n" , (int) ((100*finished_target_counter)/all_target_counter), ascdate(time((time_t*)0)) );			                
					while (next_report_percent <= current_report_percent)
						next_report_percent += env_VMAKE_REPORT_PERCENT;
				}
				else	
					if (tip->report & VMAKE_REPORT_MODE_PERCENT)
					{
						DBG0 (("VMAKE REPORT:  %d %% of targets finished\n" , current_report_percent))			
						DBG1 (("VMAKE REPORT:  %d of %d targets finished\n" , finished_target_counter, all_target_counter))			
					}
			}
		}

	}
/*
	if (tip->made > 0 )
		return ( tip->max_depmade > tip->made ? tip->max_depmade : tip->made );
*/
    /* retract propagates */
    if ( tip->propagates ) 
    {
        rc = propagate_env( tip->propagates );
        if ( rc ) 
		{ /* close_last_xml_tag(); */ return( rc );}
    }

	DBG1 (( "%s:\n", tip->name ))

	(void) sprintf ( vmakelayer_env , "CURR_LAYER=%s" , (tip->layer ? tip->layer : "") );
	rc = putenv ( vmakelayer_env );
	if ( rc )
	{
		DBG0 (( "cannot putenv '%s' \n", vmakelayer_env ))
		return ( rc );
	}

	/* CTS 1111662 */
	switch ( tip->version )
	{
		case 'f' :	strcpy (tmp_version_output, "fast"); break;
		case 'q' :	strcpy (tmp_version_output, "quick"); break;
		case 's' :	strcpy (tmp_version_output, "slow"); break;
		default  :  strcpy (tmp_version_output, "independent"); break;
	}
	DBG1 (( "targetversion: %s\n", tmp_version_output ))
	DBG5 (( "make: target '%s' minpathlvl %d dep \n",
		tip->name , tip->minpathlvl ))
	if ( dtip->date )
		DBG1 (( "%s dependencies (%s) \n", ascdate(dtip->date) ,
			(dtip->file.file ? dtip->file.file : dtip->name) ))

	if ( errcnt )
	{
#ifndef	RELOC_LIB
	    if ( (tip->type == TT_LIB) && ! vopt_reloc_lib )
	    {
		    /*
		     *  If the target is a library, rebuild it even if
		     *  not all modules are successfully built.
		     *  This has the effect that the successfully made
		     *  objects are included into the library and need
		     *  not be built again during a later vmake run.
		     */
		    DBG0 (( "WARNING: not all members made for '%s' \n",
							    tip->name ))
	    }
	    else
#endif	/*RELOC_LIB*/
	    {
		    tip->made = -2 ;
		    DBG0 (( "'%s' not made because of dependency errors \n",
							    tip->name ))
		    DBG1 (( "\n" ))
			ewrite_targetheader (tip, -2);
			eprotwrite ( "not made because of dependency errors of: \n"); 
			for ( dip = tip->dependencies ; dip != NULL ; dip = dip->next )
			{
				if ( dip->target->made < 0 )
					eprotwrite ( "        %s\n", dip->target->name); 
			}
			eprotwrite ( EPROT_SEPARATOR ); 
			/*	close_last_xml_tag(); */
			tip->stat.deperror = 1;
			set_finished(tip);

		    return ( tip->made );
	    }
	}

	switch ( tip->type )
	{
	case TT_INC :
			rc = make_include( tip , dtip->date , make_option, found_recursion );

			/* PTS 1104596 */
			if ( tip->other_dep_tip)
			{
				tip->other_dep_tip->made = rc;
				tip->other_dep_tip->status = tip->status;
				if (tip->file.file)
				{
					/* copy first filename and references to others */
					tip->other_dep_tip->file.file = (char *) repalloc( tip->other_dep_tip->file.file, 
						strlen( tip->file.file ) + 1 );
					(void) strcpy( tip->other_dep_tip->file.file, tip->file.file );
					tip->other_dep_tip->file.next = tip->file.next;
				}
				tip->other_dep_tip->date = tip->date;
				tip->other_dep_tip->minpathlvl = tip->minpathlvl; /* PTS 1104596/2  */

				set_finished(tip->other_dep_tip);

				DBG4 (( "make: found other deptype %s\n", tip->name )) 
			}

			break ;
	case TT_MEM :
	case TT_MOD :
		/* PTS 1103379 */
		rc = make_module( tip , ctip , dtip->date , make_option, found_recursion);
		break ;
	case TT_LIB :
#ifndef	RELOC_LIB
		if ( ! vopt_reloc_lib )
		    rc = make_library ( tip);
		else
#endif	/*RELOC_LIB*/
		    rc = make_reloc_library( tip);
		break ;
	case TT_REL :
	case TT_SHR :
		rc = make_reloc( tip );
		break ;
	case TT_DLL :
		rc = make_dll( tip );
		break ;
	case TT_PGM :
	case TT_SHM :
		rc = make_program( tip );
		break ;
    case TT_JP :
        rc = make_javaproject( tip, dtip->date );
        break;
	case TT_MAC :
    case TT_PRJ :
		if ( ! tip->dependencies )
		{
		    DBG1 (( "Warning: macro/project '%s' is empty! \n", tip->name ))
		    rc = 1 ;
		}
		else
			rc = tip->max_depmade;
		tip->date = dtip->date ;
		 
		break ;
	case TT_FILE :
		rc = search_file( tip );
		break ;
	case TT_CMD :
	case TT_EXECUTE :
		rc = issue_command( tip , ctip , make_option );
		break ;
	case TT_DIRLIST :
		rc = make_dirlist( tip );
		break ;
	case TT_EXTRA :
		rc = make_extra( tip , dtip->date , make_option );
		break ;
	case TT_OPTION :
		rc = 1 ;
		tip->date = 1 ; /* very old, but not zero */
		break ;
	default :
		rc = -2 ;
		DBG0 (( "ERROR: illegal target type in '%s' \n", tip->name ))
		break ;
	}
	/* &gar PTS 1102705 */
	if ( rc == ERROR_NO_CASE_SENSITIVE )
		DBG0 (( "Error: Described file '%s' is not case sensitive\n",tip->name ))			

	tip->made = rc ;
	if ( errcnt ) 
	{
		tip->made = -1 ; /* can only be true for TT_LIB */
        allerror_target_counter++;
	}

	set_finished (tip);

	/*
	 *  Remove objects from the tmp directory if they are not needed
	 *  for further libraries. Objects which go not or not exclusively
	 *  into libraries have already been copied into the objects
	 *  directory by 'make_module'.
	 */
#ifndef	RELOC_LIB
	if ( (tip->type == TT_LIB) && ! vopt_reloc_lib )
	{
	    DBG8 (( "make: removing temporary objects ... \n" ))
	    (void) remove_temp_obj( tip->dependencies );
	}
#endif	/*RELOC_LIB*/

	if ( tip->made > 1 || tip->type == TT_MAC )
	{
	    /*
	     *  After the target has successfully been made, delete
	     *  a eventually existing protocol from the last make of
	     *  this target. This is to prevent confusion with the
	     *  protocols. No obsolete protocols should mess up the
	     *  protocol's directory.
	     *  This function deletes the protocol only if it is
	     *  not the current protocol.
	     */
	    DBG8 (( "make: removing old protocol ... \n" ))
	    delete_protocol( tip->name , tip->version );
	}

	if ( tip->made > 1 )
		DBG1 (( "%s %s\n", ascdate(tip->date) ,
		    (tip->file.file ? tip->file.file : tip->name) ))
	

	if ( (tip->type >= TT_LIB) && (tip->type <= TT_MAC) )
	{
	    if ( vopt_timestamp )
    		outstamp_add ( "\n%s (%s finished)\n", ascdate(time((time_t*)0)) , tip->name );
	    else
    		DBG1 (( "\n%s (%s finished)\n", ascdate(time((time_t*)0)) , tip->name ))
	}

	DBG1 (( "\n" ))
	DBG5 (( "make: target '%s' minpathlvl %d trg \n",
		tip->name , tip->minpathlvl ))

	DBG4 (( "make: returning %d \n", tip->made ))
	
	/*close_last_xml_tag();  */

	return ( tip->made < 0 ? tip->made : 
		  ( tip->max_depmade > tip->made ? tip->max_depmade : tip->made ) );
}


/*==========================================================================*/


#ifndef	RELOC_LIB
static  int     search_library 
(
    struct trg_info         * tip 
)
{
	int                     rc=0 ;
	time_t                  date , size ;
	char                    * p ;
	VFILE                   * fp ;
	struct stat		        statbuf ;
	struct ar_hdr           hdr ;
	char                    * trgfile = NULL;
	char                    name [ FILENAMESIZE ] ;
	char                    line [ LINESIZE ] ;
	valid_bit_arr		    vbits ;
#ifdef	HAVE_STRUCT_FL_HDR_FL_FSTMOFF
	/* J.Bruehe 91-Jul-31  On IBM RS 6000, archive format is not
	 * sequential put pointer-linked with a header struct, 
	 * and names are variable size.
	 */
	struct fl_hdr           fil_hdr ;
	long                    next_member_offs ;
	int	                    name_var_size ;
#endif


	DBG4 (( "srchlib: called \n" ))
	VALID_BIT_FILL ( vbits );

	if ( opt_sizeof || opt_lint || opt_retrieve || ! tip->stat.level_o )
	{
	    DBG8 (( "srchlib: no objects \n" ))
	    return ( 0 );
	}

	/* find target */

	(void) sprintf ( name , "%s.%s", tip->name, env_ARC_TYPE );

	if ( tip->fixed_file )
	{
	    /* tip->file.file contains the target file name. Save it. */
		if ( tip->file.file )
		{
			trgfile = strdup( tip->file.file );
			if ( ! trgfile )
				return (ewrite_all (tip, 0, -3, "ERROR: insufficient dynamic memory \n" ));
		}
	    /* vmake creates a fixed target with version suffix */
        tip->file.file = (char *) repalloc( tip->file.file, (tip->file.file ? strlen( tip->file.file ): 0) + 3);
	    (void) sprintf ( tip->file.file , "%s.%c", trgfile , tip->version );

	    tip->status = PATH_OWN ;
	    rc = stat ( tip->file.file , &statbuf );
	    if ( rc >= 0 )
	    {
		    tip->date = statbuf.st_mtime ;
		    rc = 1 ; /* means: file found */
	    }
	    if (    (rc < 0)
		 && ((geterrno() == ENOENT) || (geterrno() == ENOTDIR)) )
		rc = 0 ; /* means: file not found */
	}
	else
	{
	    /* the target file is always in the OWN tree */
	    (void) new_file ( name , DIR_OBJ , versname(tip->version) ,
						tip->layer , &trgfile );

	    rc = get_file ( name , DIR_OBJ , versname(tip->version) , tip->layer ,
			FK_LIB | FK_LEVEL , vbits ,
			    &tip->file.file , & tip->date , & tip->status , 0 );
	}
	if ( rc < 0 )
		return (ewrite_all (tip, 1, rc, "ERROR: file read error for '%s' (%d)\n" ,name, rc));
	if ( rc == 0 )
	{
	    if ( tip->fixed_file )
	    {
		    /* version suffix may have been appended: restore orig name */
            tip->file.file = (char *) repalloc( tip->file.file, strlen( trgfile ) + 1 );
		    (void) strcpy( tip->file.file, trgfile );
	    }
	    DBG4 (( "srchlib: new library       '%s' \n", tip->file.file ))
	    return ( 0 );
	}
	DBG4 (( "srchlib: %s ", ascdate(tip->date) ))
	DBG4 (( "srchlib: %s library \n", tip->file.file ))
	/*
	 *  "tip->file.file" now contains the path to an existing file (incl. .f).
	 *  "trgfile" contains the path for the new file.
	 */

	if ( opt_uncond && (tip->stat.level_s|tip->stat.level_p|tip->stat.level_c) )
	{
	    /* 'trgfile' is the local target, while 'tip->file.file' may be remote */
	    if ( stat ( trgfile , &statbuf ) != -1 )
	    {
		    /* unlink a locally existing library */
		    rc = unlink_file ( trgfile );
		    if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while unlinking %s \n", trgfile));
	    }
	    if ( tip->fixed_file )
	    {
            tip->file.file = (char *) repalloc( tip->file.file, strlen( trgfile ) + 1 );
		    (void) strcpy ( tip->file.file , trgfile );
		    tip->date = 0 ;
		    tip->status = 0 ;
	    }
	    /*
	     *  Returning here with 'tip->file.file', 'tip->date', 'tip->status'
	     *  set to the designated target or a remote library.
	     */
	    DBG8 (( "srchlib: unconditionally rebuild library \n" ))
	    return ( 0 );
	}

	/*
	 *  If the most current library is on a remote node,
	 *  copy it here, which is necessary anyway.
	 *  This avoids to transfer it twice (first for date analyze
	 *  of its members, and then to use it).
	 */
	if ( (tip->status & PATH_REMOTE) && ! tip->stat.uncond && vopt_object_copy )
	{
		outstamp ( "%-9s %-5s <%s>\n",
			tip->name , versname(tip->version) , tip->file.file ? tip->file.file : "" );

	    rc = vcopy_file ( tip->file.file , trgfile , tip->date , FT_BIN );
	    if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", tip->file.file , trgfile));

	    if ( tip->stat.shrglob == 1 )
	    {
		    rc = copy_shrglob ( tip , (char*) 0 );
		    if ( rc ) return ( rc );
	    }

	    /*
	     *  With the "no execution" option, the copy didn't work,
	     *  so we don't adapt the file name, because the
	     *  date analyze of the members has still to work.
	     *  Thus, we defer this until after the analyzation is done.
	     */
	    if ( ! opt_noexec )
	    {
            tip->file.file = (char *) repalloc( tip->file.file, strlen( trgfile ) + 1 );
		    (void) strcpy ( tip->file.file , trgfile );
		    tip->status = PATH_OWN ;
	    }
	}

	/*
	    Open the archive file !
	    This might not be sucessful on all machines!
	    One alternative would be to analyze the output of
	    the 'ar tv ...' command. But because of the complicated
	    output, the probably different format on different
	    machines, and the difficulty of computing a long integer
	    time value from the character representation, this has not
	    been implemented here. Moreover, the time resolution
	    would be no better than a minute.
	    Another alternative is to avoid libraries on such machines
	    and to use relocatables instead.
	*/

	fp = fileopen ( tip->file.file , "rb" );
	if ( fp == NULL )
	{
		return (ewrite_all (tip, 1, -2, "ERROR: cannot open '%s' \n", tip->file.file ));
	}
	DBG8 (( "srchlib: opened '%s' \n", tip->file.file ))

#ifdef HAVE_STRUCT_FL_HDR_FL_FSTMOFF
	rc = fileread ( (char *) & fil_hdr , sizeof(fil_hdr) , 1 , fp );	
	if ( rc != 1 )
	{
		(void) fileclose ( fp );
		return (ewrite_all (tip, 1, -2, "ERROR: cannot read '%s' \n", tip->file.file ));
	}
	if ( strncmp ( fil_hdr.fl_magic , AIAMAG , SAIAMAG ) != 0 )
	{
		(void) fileclose ( fp );
		return (ewrite_all (tip, 0, -2, "ERROR: no archive '%s' \n", tip->file.file ));
	}
	COPY ( line , fil_hdr.fl_fstmoff , sizeof(fil_hdr.fl_fstmoff) );
	line [ sizeof(fil_hdr.fl_fstmoff) ] = '\0' ;
	next_member_offs = atol ( line );
	rc = fileseek ( next_member_offs , 0, fp );
	if ( rc != 0 )
	{
	    (void) fileclose ( fp );
		return (ewrite_all (tip, 1, -2, "ERROR: cannot fseek on '%s' \n", tip->file.file ));
	}
#else
	p = filegets ( line , sizeof(line) , fp );
	if ( p == NULL )
	{
		(void) fileclose ( fp );
		return (ewrite_all (tip, 1, -2, "ERROR: cannot read '%s' \n", tip->file.file ));
	}

	if ( strcmp ( line , ARMAG ) != 0 )
	{
		(void) fileclose ( fp );
		return (ewrite_all (tip, 1, -2, "ERROR: no archive '%s' \n", tip->file.file ));
	}
#endif	/* HAVE_STRUCT_FL_HDR_FL_FSTMOFF */

	for ( ; ; )
	{
	    /*
		Read the header of the library member.
	    */
#ifdef	HAVE_STRUCT_FL_HDR_FL_FSTMOFF	/* IBM RS6000 (AIX) has its own way, see their "ar.h" */
	    DBG4 (( "srchlib: loop: member at %d \n", next_member_offs ))
	    rc = fileread ( (char *) & hdr , sizeof(hdr) - sizeof (hdr._ar_name) , 1 , 	fp );	
		/* do not read the first two chars of the member name */
	    if ( rc != 1 )
	    {	
		    (void) fileclose ( fp );
			return (ewrite_all (tip, 1, -2, "ERROR: error reading member header in '%s' \n", tip->file.file ));
	    }
#else
	    rc = fileread ( (char*) & hdr , sizeof(hdr) , 1 , fp );
	    if ( rc != 1 ) break ;
#endif

#ifdef  ARFMAG	/* IBM AIX has no magic string for a single element */
	    if ( strncmp ( hdr.ar_fmag , ARFMAG , sizeof(hdr.ar_fmag) ) )
	    {
		    (void) fileclose ( fp );
			return (ewrite_all (tip, 0, -2, "ERROR: illegal library format of '%s' \n", tip->file.file ));
	    }
#endif

	    COPY ( line , hdr.ar_date , sizeof(hdr.ar_date) );
	    line [ sizeof(hdr.ar_date) ] = '\0' ;
	    date = atol ( line );
	    DBG4 (( "srchlib: member date %s \n", line ))

#ifdef	HAVE_STRUCT_FL_HDR_FL_FSTMOFF	/* IBM RS6000 (AIX) has its own way, see their "ar.h" */
	    COPY ( line , hdr.ar_nxtmem , sizeof(hdr.ar_nxtmem) );
	    line [ sizeof(hdr.ar_nxtmem) ] = '\0' ;
	    next_member_offs = atol ( line );
	    /*
		The member name is variable length
	    */
	    COPY ( line , hdr.ar_namlen , sizeof(hdr.ar_namlen) );
	    line [ sizeof(hdr.ar_namlen) ] = '\0' ;
	    name_var_size = atol ( line );
	    DBG4 (( "srchlib: member name length %s \n", line ))
	    /*
		Read the name of the library member.
	    */
	    rc = fileread ( line , sizeof(char) , name_var_size , fp );
	    if ( rc != name_var_size )
	    {
			(void) fileclose ( fp );
			return (ewrite_all (tip, 1, -2, "ERROR: error reading member name in '%s' \n", tip->file.file ));
	    }
	    line [ name_var_size ] = '\0' ; /* make zero terminated name */
	    DBG4 (( "srchlib: member name %s \n", line ))
#else	/* not HAVE_STRUCT_FL_HDR_FL_FSTMOFF */
	    COPY ( line , hdr.ar_size , sizeof(hdr.ar_size) );
	    line [ sizeof(hdr.ar_size) ] = '\0' ;
	    size = atol ( line );
	    DBG4 (( "srchlib: member size %s \n", line ))
	    if ( size & 0x01 ) size ++ ; /* make it even */

	    /*
		The member name is '/' terminated (not on T/35)
		and blank padded (even on T/35).
	    */
	    COPY ( line , hdr.ar_name , sizeof(hdr.ar_name) );
	    for ( p = line + sizeof(hdr.ar_name) - 1 ;
		  * p == ' ' && p > line ;
		  p -- );
	    if ( * p != ' ' && * p != '/' ) p ++ ;
	    * p = '\0' ; /* make zero terminated name */
#endif	/* HAVE_STRUCT_FL_HDR_FL_FSTMOFF */

	    /*
		Don't insert the archive's symbol table into
		the member list.
	    */
	    if ( line [ 0 ] != '\0'  &&                       /* empty name */
		 strcmp ( line , "__.SYMDEF"       ) != 0 &&  /* T/35 */
		 strcmp ( line , "__________ELEL_" ) != 0 &&  /* ULTRIX */
		 strcmp ( line , "________64ELEL_" ) != 0 &&  /* OSF1 */
		 strcmp ( line , "__________EBEB_" ) != 0 )   /* SGW */
	    {
		    rc = insert_member ( line , date , tip , TRUE );
		    if ( rc < 0 )
		    {
		        DBG8 (( "insert_member error on '%s' \n", tip->file.file ))
		        (void) fileclose ( fp );
		        return ( rc );
		    }
	    }

#ifdef	HAVE_STRUCT_FL_HDR_FL_FSTMOFF
	    /*
		Follow pointer to next member, if valid.
	    */
	    if ( next_member_offs <= 0 )
	    {
	        DBG4 (( "srchlib: leaving, offset %d \n", next_member_offs ))
    		break ;
	    }
	    rc = fileseek ( next_member_offs , 0, fp ); /* pointer chain */
#else
	    /*
		Skip the member's contents and position to the next header.
	    */
	    rc = fileseek ( size , 1, fp ); /* contents is sequential */
#endif	/* HAVE_STRUCT_FL_HDR_FL_FSTMOFF */
	    if ( rc != 0 )
	    {
		    (void) fileclose ( fp );
			return (ewrite_all (tip, 1, -2, "ERROR: cannot fseek on '%s' \n", tip->file.file ));
	    }
	}

	(void) fileclose ( fp );

	/*
	 *  In case of the "no execution" option the file name of the
	 *  library is adapted to the name it would have got if the
	 *  copy operation could have been done.
	 *  The name must also be reset if fixed_file is true.
	 */
	if ( (opt_noexec && (tip->status & PATH_REMOTE)) || tip->fixed_file )
	{
        tip->file.file = (char *) repalloc( tip->file.file, strlen( trgfile ) + 1 );
	    (void) strcpy ( tip->file.file , trgfile );
	    tip->status = PATH_OWN ;
	}

    FREE_AND_SET_TO_NULL(trgfile);

	DBG4 (( "srchlib: returning %d \n", 1 ))
	return ( 1 );
}
#endif	/*RELOC_LIB*/


/*==========================================================================*/


static  int     insert_member 
(
    char                    * target ,
    time_t                  date ,
    struct trg_info         * tip ,
    int			            is_lib  /* is relocatable lib */
)
{
	int                     lgt , rc=0 ;
	struct mem_info         ** smipp , * mip ;
	struct dep_info         ** sdipp ;
	char                    name [ NAMESIZE ] ;

    DBG2 (( "insmem: called \n" ))
	DBG3 (( "insmem: target  '%s' \n", target  ))

	lgt = strlen ( target );
	if ( is_lib )
	{
	    /* strip file extension */

	    if ( target [ lgt - 1 ] != 'o' &&
		 target [ lgt - 2 ] != '.'    )
	    {
		    DBG1 (( "WARNING: inconsistent contents of target '%s' \n",
							        tip->name ))
		    DBG1 (( "         no object module '%s' \n", target ))
		    return ( 0 );
	    }
	    lgt -= 2 ;
	}
	if ( lgt >= sizeof(name) )
		return (ewrite_all (tip, 0, -1, "ERROR: target member name too long '%s' \n", target ));

	COPY ( name , target , lgt );
	name [ lgt ] = '\0' ;

	/* check occurrence of module in target list */

	rc = - 1 ;
	for ( sdipp = & tip->dependencies ;
	      (*sdipp) != NULL ;
	      sdipp = & (*sdipp)->next )
	{
	    if ( (*sdipp)->target->type == TT_FILE )
		    rc = strcmp ( (*sdipp)->target->name , target );
	    else    
            rc = strcmp ( (*sdipp)->target->name , name );
	    if ( rc == 0 ) break ;
	}
	if ( rc )
	{
	    DBG1 (( "WARNING: inconsistent contents of target '%s' \n",
							tip->name ))
	    DBG1 (( "         module not in target list '%s' \n", name ))
	    return ( 0 );
	}

	/* insert into member list */

	rc = - 1 ;
	for ( smipp = & tip->members ;
	      (*smipp) != NULL ;
	      smipp = & (*smipp)->next )
	{
	    rc = strcmp ( (*smipp)->name , name );
	    if ( rc >= 0 ) break ;
	}

	if ( rc == 0 )
	{
	    DBG0 (( "WARNING: invalid contents of target '%s' \n",
							tip->name ))
	    DBG0 (( "         duplicate module '%s' \n", name ))
	    (*smipp)->date = 0l ; /* force rebuild of target */
	}
	else
	{
	    mip = (struct mem_info *) palloc ( sizeof(struct mem_info) );
	    if ( mip == NULL )
		    return (ewrite_all (tip, 0, -3, "ERROR: insufficient dynamic memory \n" ));

	    FILL (  mip , 0 , sizeof(struct mem_info) );

		mip->name = (char *) palloc( strlen( name ) + 1 );
		(void) strcpy( mip->name, name );

	    mip->date = date ;

	    mip->next = * smipp ;
	    * smipp = mip ;

	    DBG7 (( "insmem: inserted '%s' \n", name ))
	    DBG7 (( "insmem: date     '%s' \n", ascdate(date) ))
	    rc = 1 ;
	}

	DBG4 (( "insmem: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/


static  int     make_include 
(
    struct trg_info         * tip ,
    time_t                  date_dep ,      /* date of newest dependency */
    int                     make_option, 
	int                     found_recursion

)
{
	int                     rc=0 ;

    DBG2 (( "makinc: called \n" ))
	DBG3 (( "makinc: making '%s' \n", tip->name ))

/*
   Always compute the path name of the include file. IST ??.??.??
   Whatfor? I don't see a sense in it. IST 24.05.91
   This was necessary for modules of a certain layer with level_s
   including a include module of another layer without level_s.
   Now implemented in 'collect_include_list' by setting level_s
   of the required include module. IST 15.07.91
*/
	if ( ! tip->stat.level_s )
	{
	    DBG8 (( "makinc: no source level \n" ))
	    return ( 1 );
	}

	/* &gar CTS 1000237 (no Make if -F ) -> */
	if (opt_distrib)
	{
		if (! tip->stat.distribute)
			return 1;
	}
	/* <- &gar CTS 1000237  */

	if ( opt_sizeof && tip->language != LANG_PASCAL_P [ 0 ] )
	{
	    DBG8 (( "makinc: sizeof and no PASCAL include \n" ))
	    return ( 1 );
	}

	if ( opt_lint && ! ( tip->language == LANG_C   [ 0 ] || 
						 tip->language == LANG_CPP [ 0 ] ) )
	{
	    DBG8 (( "makinc: lint and no C/C++ include \n" ))
	    return ( 1 );
	}

	if ( tip->language == LANG_PASCAL_P [ 0 ] )
	{
	    rc = make_pascal_inc ( tip , date_dep , make_option, found_recursion );
	}
	else
	{
	    rc = make_other_inc  ( tip , date_dep , make_option, found_recursion );
	}

	DBG4 (( "makinc: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/

static  int     make_pascal_inc 
(
    struct trg_info         * tip ,
    time_t                  date_dep ,      /* date of newest dependency */
    int                     make_option,
	int                     found_recursion
)
{
	int                     lgt , rc=0 ;
	int                     status_con ;
	int                     status_typ ;
	int                     status_src ;
	int                     status_hdr ;
	time_t                  date_con ;
	time_t                  date_typ ;
	time_t                  date_src ;
	time_t                  date_hdr ;
	struct dep_info		    * dip ;
	struct dsc_info         * sip ;
	struct stat		        statbuf ;
	char                    name_src [ FILENAMESIZE ] ;
	char                    name_con [ FILENAMESIZE ] ;
	char                    name_typ [ FILENAMESIZE ] ;
	char                    name_siz [ FILENAMESIZE ] ;
	char                    name_hdr [ FILENAMESIZE ] ;
	char			        * file     = NULL;
	char                    * file_con = NULL ;
	char                    * file_typ = NULL ;
	char                    * file_siz = NULL ;
	char                    * file_src = NULL ;
	char                    * file_hdr = NULL ;
	valid_bit_arr		    vbits ;


	DBG2 (( "makpasinc: called \n" ))
	DBG3 (( "makpasinc: making '%s' \n", tip->name ))

	VALID_BIT_FILL ( vbits );

	/* find source file */

	status_src = PATH_LEVEL ;
	if ( tip->stat.level_s && (make_option == MK_FULL || tip->stat.uncond) )
	{
	    /* strip the variant identifier */
        (void) get_basename( tip->name, name_src );

		if ( tip->local_source )
		{
			struct    stat fileinfo;
			int       missing_slash;
			rc = stat( tip->local_source, & fileinfo );
			missing_slash = tip->local_source[ strlen( tip->local_source ) - 1 ] == '/' ? 0 : 1;
            if  ( (rc == 0 && ( fileinfo.st_mode & S_IFDIR ) )  ||  ( missing_slash == 0 ) )
			{
				char    * tmp_source;

				tmp_source = palloc (strlen(tip->local_source) + missing_slash + strlen(name_src) + 1);
				(void) strcpy (tmp_source, tip->local_source);
				if ( missing_slash )
					(void) strcat (tmp_source, "/" );
				(void) strcat ( tmp_source, name_src );

				FREE_AND_SET_TO_NULL(tip->local_source);
				tip->local_source = tmp_source;
				rc = stat( tip->local_source, & fileinfo );
			}
			if ( rc == 0 ) /* found file */
				date_src = fileinfo.st_mtime;
			else
			{
				return (ewrite_all (tip, 0, -2, "ERROR: missing source    '%s' \n" ,tip->local_source));
			}
			status_src = PATH_OWN;
			file_src = tip->local_source;
			rc = 1; 
		}
		else
		{
			rc = get_file ( name_src , DIR_SRC , tip->layer , EMPTY ,
				FK_SOURCE | FK_LEVEL , vbits ,
				&file_src , & date_src , & status_src, 0 );
			if ( rc < 0 )
				return (ewrite_all (tip, 1, rc, "ERROR: file read error for '%s' (%d)\n" ,name_src, rc));
		}

	    if ( rc == 0 )
	    {
			rc = get_file ( name_src , DIR_SRC , tip->layer , EMPTY , FK_SOURCE | FK_LEVEL , vbits ,
			                &file_src , & date_src , & status_src, 1 );
			if ( rc == 0 )
				return (ewrite_all (tip, 0, -2, "ERROR: missing source    '%s' \n" ,name_src));
			/*
		    DBG0 (( "ERROR: missing source    '%s' \n", name_src ))
			ewrite_targetheader (tip, -2);
			eprotwrite ( "missing source \n");
			eprotwrite ( EPROT_SEPARATOR ); 
		    return ( - 2 );
			*/
	    }
	    DBG1 (( "%s %s \n", ascdate(date_src) , file_src ))
		/* CTS 1111841 -> */
	    mprotwrite ( "%s src/%s/%s,%d\n", ascdate(date_src) , tip->layer, tip->name, 
					status_src & PATH_LEVEL );
		/* <- CTS 1111841 */

	    if ( (status_src & PATH_LEVEL) < tip->minpathlvl )
	    {
		    tip->minpathlvl = status_src & PATH_LEVEL ;
		    DBG5 (( "makpasinc: target '%s' minpathlvl %d src \n",
			    tip->name , tip->minpathlvl ))
	    }

	    /* find dates file */

	    if ( vopt_dates )
	    {
		    rc = get_dates_file ( tip,
			    FK_OBJECT | (tip->minpathlvl & FK_LEVEL) ,
			    date_src , vbits );
		    if ( rc < 0 )
		    {
			    DBG4 (( "get file error, returning %d \n", rc ))
			    return ( rc );
		    }
	    }

	    /*
	     *  With VMAKE_OPTION=V get all sources into OWN tree.
	     */
	    if ( vopt_source_copy && ! (status_src & PATH_OWN) )
	    {
			/*
			CTS 1110663
		    if ( opt_errorinc )
		    {
		        DBG0 (( "Error: trying to make '%s' with option -E \n",
								    tip->name ))
		        return ( -2 );
		    }
			*/
		    outstamp ( "%-9s %-5s <%s>\n", EMPTY , EMPTY , file_src );
		    (void) new_file ( tip->name , DIR_SRC , tip->layer ,
					        EMPTY , &file );
		    rc = vcopy_file ( file_src , file , date_src , FT_TXT );
			if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", file_src , file));

		    file_src = (char *) repalloc( file_src, strlen( file ) + 1 );
            (void) strcpy ( file_src , file );

		    status_src = PATH_OWN | (status_src & PATH_LEVEL);
	    }
	    if ( ! tip->stat.level_p && ! tip->stat.level_c && ! tip->stat.level_o )
	    {
		    /* 'S' or 'SV' specified, everything ok */
		    DBG4 (( "makpasinc: stopping after name generation \n" ))
		    return ( 1 );
	    }
	}

	/* find constants file (target) */
    status_con = tip->minpathlvl & PATH_LEVEL ;
	DBG5 (( "makpasinc: target '%s' minpathlvl %d srch \n",
		tip->name , status_con ))
	(void) sprintf ( name_con , "%s.con", tip->name );

	rc = get_file ( name_con , DIR_INC , 
        is_oldstyled_source( tip->name, NULL, NULL ) ? EMPTY : tip->layer ,
        versname(tip->version) ,
		FK_OBJECT | (status_con & FK_LEVEL) , vbits ,
		&file_con , & date_con , & status_con, 0 );
	if ( rc < 0 )
		return (ewrite_all (tip, 1, rc, "ERROR: file read error for '%s' (%d)\n" ,name_con, rc));
	DBG1 (( "%s %s \n", ascdate(date_con) , file_con ))

	/* find types file (target as well) */

    status_typ = tip->minpathlvl & PATH_LEVEL ;
	DBG5 (( "makpasinc: target '%s' minpathlvl %d srch \n",
		tip->name , status_typ ))
	(void) sprintf ( name_typ , "%s.typ", tip->name );

	rc = get_file ( name_typ , DIR_INC , 
        is_oldstyled_source( tip->name, NULL, NULL ) ? EMPTY : tip->layer ,
        versname(tip->version) , 
		FK_OBJECT | (status_typ & FK_LEVEL) , vbits ,
		&file_typ , & date_typ , & status_typ, 0 );
	if ( rc < 0 )
		return (ewrite_all (tip, 1, rc, "ERROR: file read error for '%s' (%d)\n" ,name_typ, rc));

	DBG1 (( "%s %s \n", ascdate(date_typ) , file_typ ))

	/* find C-header file (target as well) */

	if ( ! vopt_no_auto_hdr )
	{
		status_hdr = tip->minpathlvl & PATH_LEVEL ;
	    DBG5 (( "makpasinc: target '%s' minpathlvl %d srch \n",
		    tip->name , status_hdr ))
		/* PTS 1104605 */
		if ( extension_exists(tip->name) )
			(void) sprintf ( name_hdr , "%s", tip->name );
		else
			(void) sprintf ( name_hdr , "%s.h", tip->name );

	    rc = get_file ( name_hdr , DIR_INC ,
                is_oldstyled_source( tip->name, NULL, NULL ) ? EMPTY : tip->layer ,
			    versname(tip->version) ,
			    FK_OBJECT | (status_hdr & FK_LEVEL) , vbits ,
			    &file_hdr , & date_hdr , & status_hdr, 0 );
		if ( rc < 0 )
			return (ewrite_all (tip, 1, rc, "ERROR: file read error for '%s' (%d)\n" ,name_hdr, rc));
	    
	    DBG1 (( "%s %s \n", ascdate(date_hdr) , file_hdr ))
	}
	else
	{
	    status_hdr = status_typ ;
	    date_hdr   = date_typ ;
	    COPY ( name_hdr , name_typ , sizeof(name_hdr) );

        file_hdr = (char *) repalloc( file_hdr, strlen( file_typ ) + 1 );
	    (void) strcpy( file_hdr , file_typ );
	}

	/* test wether target files are in same directory */

	if ( date_con > 0 && date_typ > 0 && date_hdr > 0 )
	{
	    rc = strncmp ( file_con , file_typ , (CINT)(strlen(file_con) - 4) );
	    if ( rc == 0 )
	    {
		    rc = strncmp ( file_con , file_hdr ,
				    (CINT)(strlen(file_con) - 4) );
	    }
	    if ( rc != 0 )
	    {
		    DBG8 (( "makpasinc: includes not consistent '%s' \n",
							    tip->name ))
		    date_con = 0l ; /* force remake */
		    date_typ = 0l ;
	    }
	}

	/* insert newest date into target information struct */

	if ( date_con > date_typ )
		tip->date = date_con ;
	else    
        tip->date = date_typ ;
	if ( date_hdr > tip->date )
		tip->date = date_hdr ;

	/* get filename without extension */

	lgt = strlen ( file_con ) - 4 ;
    tip->file.file = (char *) repalloc( tip->file.file, lgt + 1 );
	(void) strncpy( tip->file.file , file_con , lgt );
	tip->file.file [ lgt ] = '\0' ;
	tip->status = status_con ;

	if ( make_option != MK_FULL && ! tip->stat.uncond )
	{
	    DBG8 (( "makpasinc: no full make \n" ))
	    return ( 1 );
	}

	/*
	 *  During a local make, local objects are used regardless of their
	 *  age relative to the source, except that the source is local too.
	 *  The normal make algorithm may be used if the dates of the
	 *  unused remote files are cleared.
	 */
	if ( vopt_local )
	{
	    if ( status_src & PATH_OWN )
	    {
	        DBG1 (( "makpasinc: Source local: normal operation \n" ))
	    }
	    else
	    {
		    if ( tip->status & PATH_OWN )
		    {
		        DBG1 (( "makpasinc: Objects local: ignore src date \n" ))
		        date_src = 0 ;
		    }
		    else
		    {
		        DBG1 (( "makpasinc: nothing local: normal operation \n" ))
		    }
	    }
	}

	/* compare dates */

	if ( ! tip->stat.uncond &&
	     date_con >= date_src - TOL &&
	     date_typ >= date_src - TOL &&
	     date_hdr >= date_src - TOL &&
	     date_con >= date_dep - TOL &&
	     date_typ >= date_dep - TOL &&
	     date_hdr >= date_dep - TOL &&
	     VALID_BIT_TST(status_con&PATH_LEVEL,vbits) )
	{	/* PTS 1102287 copy also if in local path (not OWN) */
	    if ( (status_con & PATH_REMOTE) || (status_con & PATH_LOCAL) ||
		 (status_typ & PATH_REMOTE) || (status_typ & PATH_LOCAL) ||
		 (status_hdr & PATH_REMOTE) || (status_hdr & PATH_LOCAL) )
	    {
			/*
			CTS 1110663
		    if ( opt_errorinc )
		    {
		        DBG0 (( "Error: trying to make '%s' with option -E \n",
								    tip->name ))
		        return ( -2 );
		    }
			*/
			
			/* CTS 1112727 */
			/* if recursion found the module will be maked later once again */
			/* return 2 is necessary for dependencies */
			/*
			if (found_recursion) 
			{
				DBG1 (( "found recursion - make module later\n" ))
				FREE_AND_SET_TO_NULL(file);
				FREE_AND_SET_TO_NULL(file_con);
				FREE_AND_SET_TO_NULL(file_typ);
				FREE_AND_SET_TO_NULL(file_siz);
				FREE_AND_SET_TO_NULL(file_src);
				FREE_AND_SET_TO_NULL(file_hdr);
				return (2); 		
			}
			*/
			
		    outstamp ( "%-9s %-5s <%s>\n",
			    tip->name , versname(tip->version) , file_con );
		    rc = new_file ( name_con , DIR_INC , 
                is_oldstyled_source( tip->name, NULL, NULL ) ? EMPTY : tip->layer ,
                EMPTY , &file );
		    if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while creating %s \n", name_con));
		    rc = vcopy_file ( file_con , file , date_con , FT_TXT );
		    if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", file_con , file));

		    outstamp ( "%-9s %-5s <%s>\n",
			    tip->name , versname(tip->version) , file_typ );
		    rc = new_file ( name_typ , DIR_INC , 
                is_oldstyled_source( tip->name, NULL, NULL ) ? EMPTY : tip->layer ,
                EMPTY , &file );
			if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while creating %s \n", name_typ));
		    rc = vcopy_file ( file_typ , file , date_typ , FT_TXT );
			if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", file_typ , file));
		    

		    /* get filename without extension */
		    lgt = strlen ( file ) - 4 ;
            tip->file.file = (char *) repalloc( tip->file.file, lgt + 1 );
		    (void) strncpy( tip->file.file , file , lgt );
		    tip->file.file [ lgt ] = '\0' ;
		    tip->status = PATH_OWN ;

		    if ( ! vopt_no_auto_hdr )
		    {
		        outstamp ( "%-9s %-5s <%s>\n",
			        tip->name , versname(tip->version) , file_hdr );
		        rc = new_file ( name_hdr , DIR_INC , 
                    is_oldstyled_source( tip->name, NULL, NULL ) ? EMPTY : tip->layer ,
                    EMPTY , &file );
				if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while creating %s \n", name_hdr));
		        rc = vcopy_file ( file_hdr , file , date_hdr , FT_TXT );
				if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", file_hdr , file));
		    }

		    if ( vopt_dates )
		    {
		        rc = make_dates_file ( tip, date_src);
				if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while creating date file\n"));
		    }
	        FREE_AND_SET_TO_NULL(file);
	        FREE_AND_SET_TO_NULL(file_con);
	        FREE_AND_SET_TO_NULL(file_typ);
	        FREE_AND_SET_TO_NULL(file_siz);
	        FREE_AND_SET_TO_NULL(file_src);
	        FREE_AND_SET_TO_NULL(file_hdr);

		    return ( 2 );
	    }

	    FREE_AND_SET_TO_NULL(file);
	    FREE_AND_SET_TO_NULL(file_con);
	    FREE_AND_SET_TO_NULL(file_typ);
	    FREE_AND_SET_TO_NULL(file_siz);
	    FREE_AND_SET_TO_NULL(file_src);
	    FREE_AND_SET_TO_NULL(file_hdr);

        DBG2 (( "up to date        %s \n", tip->name ))
	    return ( 1 );
	}

	/* CTS 1112727 */
	/* if recursion found the module will be maked later once again */
	/* return 2 is necessary for dependencies                       */
	/* we do so as the module was maked without errors              */
	if (found_recursion) 
	{
		DBG1 (( "found recursion - make module later\n" ))
		if (vopt_current_date)
		{	
			tip->date = time ( (time_t *) 0 );
			tip->date |= 1 ; /* force an odd value for vmake generated files */
		}
		else
		{
			tip->date = date_make > date_src ? date_make : date_src;
			if ( date_dep > tip->date )
				tip->date = date_dep;
		}

		FREE_AND_SET_TO_NULL(file);
		FREE_AND_SET_TO_NULL(file_con);
		FREE_AND_SET_TO_NULL(file_typ);
		FREE_AND_SET_TO_NULL(file_siz);
		FREE_AND_SET_TO_NULL(file_src);
		FREE_AND_SET_TO_NULL(file_hdr);
		return (2); 		
	}



	if ( tip->stat.uncond )
	    DBG1 (( "unconditional     %s \n", tip->name ))

	/* make target files */
	/*
	CTS 1110663
	if ( opt_errorinc )
	{
	    DBG0 (( "Error: trying to make '%s' with option -E \n",
							tip->name ))
	    return ( -2 );
	}
	*/

	/* echo target name and version */

	if (tip->protstyle == PROTSTYLE_QUALIFIED_VALUE)
		outstamp ( "%s/%s %-5s", tip->layer, tip->name , versname(tip->version) );
	else
		outstamp ( "%-9s %-5s", tip->name , versname(tip->version) );
	
	if ( opt_sizeof ) outstamp_add ( " %s", "sizeof" );
	outstamp_add ( "\n" );

	/* change to proper directory */
	rc = set_work_dir ( tip->version );
	
	if ( rc < 0 )
	{
		DBG8 (( "makpasinc: setwrkdir error %d \n", rc ))
		return ( - 3 );
	}
	DBG8 (( "makpasinc: set work dir '%s' \n", tmp_dir ))

    /* get local target file name */

	tip->date = 0l ;
	(void) new_file ( tip->name , DIR_INC , 
        is_oldstyled_source( tip->name, NULL, NULL ) ? EMPTY : tip->layer ,
        EMPTY , &tip->file.file );
    
    file_con = (char *) repalloc( file_con, strlen( tip->file.file ) + 4 + 1 );
    file_typ = (char *) repalloc( file_typ, strlen( tip->file.file ) + 4 + 1 );
	(void) sprintf ( file_con , "%s.con", tip->file.file );
	(void) sprintf ( file_typ , "%s.typ", tip->file.file );

	/* PTS 1104605 */
	if ( extension_exists(tip->file.file) )
    {
        file_hdr = (char *) repalloc( file_hdr, strlen( tip->file.file ) + 1 );
		(void) sprintf ( file_hdr , "%s", tip->file.file );
    }
	else
    {
        file_hdr = (char *) repalloc( file_hdr, strlen( tip->file.file ) + 3 );
		(void) sprintf ( file_hdr , "%s.h", tip->file.file );
    }
	tip->status = PATH_OWN ;

	if ( opt_uncondincl && (stat ( file_con , &statbuf ) != -1) )
	{
	    rc = unlink_file ( file_con );
		if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while unlinking %s \n", file_con));
	}
	if ( opt_uncondincl && (stat ( file_typ , &statbuf ) != -1) )
	{
	    rc = unlink_file ( file_typ );
		if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while unlinking %s \n", file_typ));
	}
	if ( ! vopt_no_auto_hdr )
	{
	    if ( opt_uncondincl && (stat ( file_hdr , &statbuf ) != -1) )
	    {
		    rc = unlink_file ( file_hdr );
			if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while unlinking %s \n", file_hdr));
	    }
	}

	/* if necessary, copy remote source */

	if ( status_src & PATH_REMOTE )
	{
	    rc = vcopy_file ( file_src , tip->name , date_src , FT_TXT );
		if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", file_src, tip->name));
	}
	else
	{
		if ( test_for_variants( tip->name ) )
	    {
		    /* if it is a variant, get a link (or copy) here */
			rc = vcopy_file ( file_src , tip->name , date_src , FT_TXT );
			if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", file_src, tip->name));
			/* use tip->name, unlink after use */
		    status_src = PATH_REMOTE | (status_src & PATH_LEVEL);
	    }
	}

	/* unpack module frame */

	rc = cmdpgm ( env_TOOLSHELL );
	if ( env_TOOLOPT ) rc |= cmdarg( 1, env_TOOLOPT);
	rc |= cmdscript ( unpackinc(tip) );
	if ( opt_keep & 2 ) rc |= cmdarg( 1,  "-k" );
	/* if ( tip->stat.debug   || opt_debug ) CTS 1102466 */
	if ( opt_optdebug)
		rc |= cmdarg( 1,  "-optdebug%c1", DSC_DELIMITER );
	else
		if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) )
				rc |= cmdarg( 1,  "-debug%c1", DSC_DELIMITER );

	if (tip->stat.noextension) rc |= cmdarg( 1,  "-noextension" ); /* CTS 1112700 */

	if ( rc ) return ( rc );
	for ( sip = tip->descriptions ; sip != NULL ; sip = sip->next )
	{
	    /* descriptors beginning alnum are concom options */
		/* if ( ! isalnum(sip->descriptor[0]) ) continue ; */
		/* now: always "-" before - parse by mf_opts.pl */
	    rc |= cmdarg( 1,  "-%s%s", sip->descriptor , sip->value );
	    if ( rc ) return ( rc );
	}
	if ( status_src & PATH_REMOTE )
		rc |= cmdarg( 1,  tip->name );
	else    
        rc |= cmdarg( 1,  file_src );
	if ( rc ) return ( rc );

	/* all demands will be copied to work directory */
	rc = copy_demands ( tip );
	if ( rc ) return ( -2 );

    rc = cmdexe ( );
	/* CTS 1112006 */
	flush_outputlist (tip, rc, 1);
	if (rc)
		return ( rc );
	
	if ( status_src & PATH_REMOTE ) (void) unlink_file ( tip->name );

	if ( ! vopt_no_auto_hdr )
	{
	    /* create the C-header file */

		rc = cmdpgm ( env_TOOLSHELL );
		if ( env_TOOLOPT ) rc |= cmdarg( 1, env_TOOLOPT);
	    rc |= cmdscript ( CMD_MF00CCC );
	    if ( opt_keep & 2 ) rc |= cmdarg( 1,  "-k" );
	    for ( sip = tip->descriptions ; sip != NULL ; sip = sip->next )
	    {
		    /* descriptors beginning alnum are concom options */
		    /* if ( ! isalnum(sip->descriptor[0]) ) continue ; */
			/* now: always "-" before - parse by mf_opts.pl */
		    rc |= cmdarg( 1,  "-%s%s", sip->descriptor , sip->value );
		    if ( rc ) return ( rc );
	    }
	    rc |= cmdarg( 1,  "%s.con" , tip->name );
	    rc |= cmdarg( 1,  "%s.typ" , tip->name );
	    for ( dip = tip->dependencies ; dip != NULL ; dip = dip->next )
	    {
		    if ( dip->target->stat.deptype == DT_INCDEP )
		    {
		        /* PASCAL include file name is without .con or .typ */
		        rc |= cmdarg( 1,  "%s.con" , dip->target->file.file );
		        rc |= cmdarg( 1,  "%s.typ" , dip->target->file.file );
		    }
	    }
	    if ( rc ) return ( rc );
		rc = cmdexe ( );
	    /* CTS 1112006 */
		flush_outputlist (tip, rc, 1);
		if (rc)
			return ( rc );
	}

	/* if include level 'I' is required, create size information */

	if ( tip->stat.level_i > 1 )
	{
		rc = cmdpgm ( env_TOOLSHELL );
		if ( env_TOOLOPT ) rc |= cmdarg( 1, env_TOOLOPT);
	    rc |= cmdscript ( CMD_MFSIZE );
	    if ( opt_keep & 2 ) rc |= cmdarg( 1,  "-k" );
	    for ( sip = tip->descriptions ; sip != NULL ; sip = sip->next )
	    {
		    /* descriptors beginning alnum are concom options */
		    /* if ( ! isalnum(sip->descriptor[0]) ) continue ; */
			/* now: always "-" before - parse by mf_opts.pl */
		    rc |= cmdarg( 1,  "-%s%s", sip->descriptor , sip->value );
		    if ( rc ) return ( rc );
	    }
	    for ( dip = tip->dependencies ; dip != NULL ; dip = dip->next )
	    {
		    if ( dip->target->stat.deptype == DT_INCDEP )
		    {
		        /* PASCAL include file name is without .con or .typ */
		        rc |= cmdarg( 1,  "%s.con" , dip->target->file.file );
		        rc |= cmdarg( 1,  "%s.typ" , dip->target->file.file );
	        }
		}
	    rc |= cmdarg( 1,  "%s.con" , tip->name );
	    rc |= cmdarg( 1,  "%s.typ" , tip->name );
	    if ( rc ) return ( rc );
		rc = cmdexe ( );
	    /* CTS 1112006 */
		flush_outputlist (tip, rc, 1);
		if (rc)
			return ( rc );
		

	    (void) sprintf ( name_siz , "%s.siz", tip->name );
	    (void) new_file ( name_siz , DIR_SIZE , EMPTY , EMPTY , &file_siz );
	    rc = move_file ( name_siz , file_siz );
		if ( rc ) 
			return ( ewrite_all (tip, 1, rc, "ERROR: error while moving %s to %s \n", name_siz , file_siz));

	    /*
	     *  With option -S and this module on the command line
	     *  and not needed for other modules (callers == NULL),
	     *  don't create files in DIR_INC directory.
	     */
	    if ( opt_sizeof && ! tip->callers )
	    {
		DBG8 (( "makpasinc: sizeof and no callers, returning 1 \n"))
		return ( 1 );
	    }
	}

	/* remove copied demand objects */
	rc = remove_demands ( tip );
	if ( rc ) return rc ;

	/* touch targets, both should have the same date */
	/* CTS 1110368 */
	if (vopt_current_date)
	{
		tip->date = time ( (time_t *) 0 );
		tip->date |= 1 ; /* force an odd value for vmake generated files */
	}
	else
	{
		tip->date = date_make > date_src ? date_make : date_src;
		if ( date_dep > tip->date )
			tip->date = date_dep;
	}

	rc = touch_file ( tip, name_con , tip->date , "" , TRUE );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while touching %s\n", name_con));
	rc = touch_file ( tip, name_typ , tip->date , "" , TRUE );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while touching %s\n", name_typ));
	if ( ! vopt_no_auto_hdr )
	{
	    rc = touch_file ( tip, name_hdr , tip->date , "" , TRUE );
		if ( rc ) 
			return ( ewrite_all (tip, 1, rc, "ERROR: error while touching %s\n", name_hdr));
	}

	/* move files */

	rc = move_file ( name_con , file_con );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while moving %s to %s \n", name_con , file_con));
	rc = move_file ( name_typ , file_typ );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while moving %s to %s \n", name_typ , file_typ ));

	if ( ! vopt_no_auto_hdr )
	{
	    rc = move_file ( name_hdr , file_hdr );
	    if ( rc ) 
			return ( ewrite_all (tip, 1, rc, "ERROR: error while moving %s to %s \n", name_hdr , file_hdr ));
	}

	if ( vopt_dates )
	{
	    rc = make_dates_file ( tip, date_src);
	    if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while creating date file\n"));
	}

    rc = 2 ; /* really made */

	FREE_AND_SET_TO_NULL(file);
	FREE_AND_SET_TO_NULL(file_con);
	FREE_AND_SET_TO_NULL(file_typ);
	FREE_AND_SET_TO_NULL(file_siz);
	FREE_AND_SET_TO_NULL(file_src);
	FREE_AND_SET_TO_NULL(file_hdr);

    DBG4 (( "makpasinc: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/

static  int     make_other_inc 
(
    struct trg_info         * tip ,
    time_t                  date_dep ,      /* date of newest dependency */
    int                     make_option, 
	int                     found_recursion
)
{
	int                     rc=0 ;
	int                     status_src;
	time_t                  date_src;
	struct dep_info		    * dip ;
	struct dsc_info         * sip ;
	struct file_info        * fip;
	struct stat		statbuf ;
	char                    name     [ FILENAMESIZE ] ;
	char                    name_src [ FILENAMESIZE ] ;
	char                    * file_src = NULL ;
	char                    * file     = NULL ;
	valid_bit_arr		vbits;


	DBG2 (( "makothinc: called \n" ))
	DBG3 (( "makothinc: making '%s' \n", tip->name ))

	VALID_BIT_FILL ( vbits );
	
	if ( tip->language != ' ' && ! (tip->stat.noextension) && ! extension_exists(tip->name) )
		(void) sprintf ( name , "%s.h", tip->name );
	else
		(void) sprintf ( name , "%s", tip->name );

	/* set filename to afile if afile ends with / */
	if (tip->fixed_file )
	{	
		for (fip = tip->file.next; fip; fip = fip->next)
		{
			if ( fip->file[ strlen( fip->file ) - 1 ] == '/')
			{
				fip->file = (char*) repalloc ( fip->file, strlen (fip->file) + strlen( name ) + 1 );
				strcat (fip->file, name);
			}
		}
	}
	
    /* &gar CTS 1000237 -> */
	if ( opt_distrib )
	{
		rc = 0;
		for (fip = tip->file.next; fip; fip = fip->next)
		{
			if (! fip->nodistrib)
				rc += insert_dist( fip->file );
		}
		return rc < 0 ? rc : 1;
	}
	/* <- &gar CTS 1000237 */

	/* find source */

	status_src = PATH_LEVEL ;
	if ( tip->stat.level_s && (make_option == MK_FULL || tip->stat.uncond) )
	{
	    /* strip the variant identifier */
		if (tip->stat.realname)
			strcpy (name_src, tip->name);
		else
			(void) get_basename( tip->name, name_src );

		if ( tip->local_source )
		{
			struct    stat fileinfo;
			int       missing_slash;
			rc = stat( tip->local_source, & fileinfo );
			missing_slash = tip->local_source[ strlen( tip->local_source ) - 1 ] == '/' ? 0 : 1;
            if  ( (rc == 0 && ( fileinfo.st_mode & S_IFDIR ) )  ||  ( missing_slash == 0 ) )
			{
				char    * tmp_source;

				tmp_source = palloc (strlen(tip->local_source) + missing_slash + strlen(name_src) + 1);
				(void) strcpy (tmp_source, tip->local_source);
				if ( missing_slash )
					(void) strcat (tmp_source, "/" );
				(void) strcat ( tmp_source, name_src );

				FREE_AND_SET_TO_NULL(tip->local_source);
				tip->local_source = tmp_source;
				rc = stat( tip->local_source, & fileinfo );
			}
			if ( rc == 0 ) /* found file */
				date_src = fileinfo.st_mtime;
			else
			{
				return (ewrite_all (tip, 0, -2, "ERROR: missing source    '%s' \n" ,tip->local_source));
			}
			status_src = PATH_OWN;
			file_src = tip->local_source;
			rc = 1; 
		}
		else
		{
			rc = get_file ( name_src , DIR_SRC , tip->layer , EMPTY ,
				FK_SOURCE | FK_LEVEL , vbits ,
				&file_src , & date_src , & status_src, 0 );
			if ( rc < 0 )
				return (ewrite_all (tip, 1, rc, "ERROR: file read error for '%s' (%d)\n" ,name_src, rc));

			if ( rc == 0 )
				if (get_file ( name_src , DIR_SRC , tip->layer , EMPTY , FK_SOURCE | FK_LEVEL , vbits ,
								&file_src , & date_src , & status_src, 1 ) <= 0)
					return (ewrite_all (tip, 0, -2, "ERROR: missing source    '%s' \n" ,name_src));
		}
	    
	    DBG1 (( "%s %s \n", ascdate(date_src) , file_src ))
		/* CTS 1111841 -> */
	    mprotwrite ( "%s src/%s/%s,%d\n", ascdate(date_src) , tip->layer, tip->name, 
					status_src & PATH_LEVEL );
		/* <- CTS 1111841 */

	    if ( (status_src & PATH_LEVEL) < tip->minpathlvl )
	    {
		    tip->minpathlvl = status_src & PATH_LEVEL ;
		    DBG5 (( "makothinc: target '%s' minpathlvl %d src \n",
			    tip->name , tip->minpathlvl ))
	    }

	    /* find dates file */

	    if ( vopt_dates )
	    {
		    rc = get_dates_file ( tip,
			    FK_OBJECT | (tip->minpathlvl & FK_LEVEL) ,
			    date_src , vbits );
		    if ( rc < 0 )
		    	return (ewrite_all (tip, 1, rc, "ERROR: date file error (%d) \n", rc));
	    }

	    /*
	     *  With VMAKE_OPTION=V get all sources into OWN tree.
	     */
	    if ( vopt_source_copy && ! (status_src & PATH_OWN) )
	    {
			/*
			CTS 1110663
		    if ( opt_errorinc )
		    {
		        DBG0 (( "Error: trying to make '%s' with option -E \n",
								    tip->name ))
		        return ( -2 );
		    }
			*/
		    outstamp ( "%-9s %-5s <%s>\n", EMPTY , EMPTY , file_src );
		    (void) new_file ( tip->name , DIR_SRC , tip->layer ,
					        EMPTY , &file );
		    rc = vcopy_file ( file_src , file , date_src , FT_TXT );
		    if ( rc ) 
				return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", file_src , file));

            file_src = (char *) repalloc( file_src, strlen(file ) + 1 );
		    (void) strcpy( file_src , file );

		    status_src = (status_src & PATH_LEVEL) | PATH_OWN ;
	    }
	    if ( ! tip->stat.level_p && ! tip->stat.level_c && ! tip->stat.level_o )
	    {
		    /* 'S' or 'SV' specified, everything ok */
		    DBG4 (( "makothinc: stopping after name generation \n" ))
		    return ( 1 );
	    }
	}

	/* find target */

    tip->status = tip->minpathlvl & PATH_LEVEL ;
	DBG5 (( "makothinc: target '%s' minpathlvl %d srch \n",
		tip->name , tip->status ))

	/*
	if ( tip->language != ' ' && ! (tip->stat.noextension) && ! extension_exists(tip->name) )
		(void) sprintf ( name , "%s.h", tip->name );
	else
		(void) sprintf ( name , "%s", tip->name );

  */

	rc = get_file ( name , DIR_INC , 
        is_oldstyled_source( tip->name, NULL, NULL ) ? EMPTY : tip->layer ,
        versname(tip->version) , 
		FK_OBJECT | (tip->status & FK_LEVEL) , vbits ,
		&tip->file.file , & tip->date , & tip->status, 0 );
	if ( rc < 0 )
		return (ewrite_all (tip, 1, rc, "ERROR: file read error for '%s' (%d)\n" ,name, rc));

	DBG1 (( "%s %s \n", ascdate(tip->date) , tip->file.file ))

	if ( make_option != MK_FULL && ! tip->stat.uncond )
	{
		DBG8 (( "makothinc: no full make \n" ))
		return ( 1 );
	}

	/*
	 *  During a local make, local objects are used regardless of their
	 *  age relative to the source, except that the source is local too.
	 *  The normal make algorithm may be used if the dates of the
	 *  unused remote files are cleared.
	 */
	if ( vopt_local )
	{
	    if ( status_src & PATH_OWN )
	    {
	        DBG1 (( "makothinc: Source local: normal operation \n" ))
	    }
	    else
	    {
		    if ( tip->status & PATH_OWN )
		    {
		        DBG1 (( "makothinc: Object local: ignore src date \n" ))
		        date_src = 0 ;
		    }
		    else
		    {
		        DBG1 (( "makothinc: nothing local: normal operation \n" ))
		    }
	    }
	}

	/* compare dates */

	if ( ! tip->stat.uncond &&
	     tip->date >= date_src - TOL &&
	     tip->date >= date_dep - TOL &&
	     VALID_BIT_TST(tip->status&PATH_LEVEL,vbits) )
	{
		/* PTS 1102287 copy also if in local path (not OWN) */
	    if ( (tip->status & PATH_REMOTE) || (tip->status & PATH_LOCAL) )
	    {

			/* CTS 1112727 */
			/* if recursion found the module will be maked later once again */
			/* return 2 is necessary for dependencies */
			/*
			if (found_recursion) 
			{
				DBG1 (( "found recursion - make module later\n" ))
				FREE_AND_SET_TO_NULL(file);
				FREE_AND_SET_TO_NULL(file_src);
				return (2); 		
			}
			*/

			/*
			CTS 1110663
		    if ( opt_errorinc )
		    {
		        DBG0 (( "Error: trying to make '%s' with option -E \n",
								    tip->name ))
		        return ( -2 );
		    }
			*/
		    outstamp ( "%-9s %-5s <%s>\n",
				tip->name , versname(tip->version) , tip->file.file ? tip->file.file : "" );
		    rc = new_file ( name , DIR_INC , 
                is_oldstyled_source( tip->name, NULL, NULL ) ? EMPTY : tip->layer ,
                EMPTY , &file );
		    if ( rc ) 
				return (ewrite_all (tip, 1, rc, "ERROR: file creation error '%s' (%d)\n" ,name, rc));

		    rc = vcopy_file ( tip->file.file , file , tip->date , FT_TXT );
		    if ( rc ) 
				return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", tip->file.file , file));

            tip->file.file = (char *) repalloc( tip->file.file, strlen( file ) + 1 );
		    (void) strcpy ( tip->file.file , file );

		    tip->status = PATH_OWN ;

            /* fixed_file */
            if ( tip->fixed_file )
            {
				rc = 0;
				for (fip = tip->file.next; fip; fip = fip->next)
				{
					if (vcopy_file ( tip->file.file , fip->file , tip->date , tip->stat.binary ? FT_BIN : FT_TXT  ))
					{
						rc--;
						ewrite_all (tip, 1, -1, "ERROR: error while copying %s to %s\n", tip->file.file , fip->file);
					}
				}
    	        if ( rc ) return ( rc );
            }

            if ( vopt_dates )
		    {
		        rc = make_dates_file ( tip, date_src);
				if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while creating date file\n"));
		    }
            FREE_AND_SET_TO_NULL(file);
            FREE_AND_SET_TO_NULL(file_src);

		    return ( 2 );
	    }

        /* fixed_file */
        if ( tip->fixed_file )
        {
			rc = 0;
			for (fip = tip->file.next; fip; fip = fip->next)
			{
				rc += vcopy_file ( tip->file.file , fip->file , tip->date , tip->stat.binary ? FT_BIN : FT_TXT  );
			}
    	    if ( rc ) return ( rc );
        }

        FREE_AND_SET_TO_NULL(file);
        FREE_AND_SET_TO_NULL(file_src);

        DBG2 (( "up to date        %s \n", tip->name ))
	    return ( 1 );
	}
	if ( tip->stat.uncond )
	    DBG1 (( "unconditional     %s \n", tip->name ))

	/* make target */
	/*
	CTS 1110663
	if ( opt_errorinc )
	{
	    DBG0 (( "Error: trying to make '%s' with option -E \n", tip->name ))
	    return ( -2 );
	}
	*/


	/* CTS 1112727 */
	/* if recursion found the module will be maked later once again */
	/* return 2 is necessary for dependencies                       */
	/* we do so as the module was maked without errors              */
	if (found_recursion) 
	{
		if (vopt_current_date)
		{
			tip->date = time ( (time_t *) 0 );
			tip->date |= 1 ; /* force an odd value for vmake generated files */
		}
		else
		{
			tip->date = date_make > date_src ? date_make : date_src;
			if ( date_dep > tip->date )
				tip->date = date_dep;
		}
	    FREE_AND_SET_TO_NULL(file);
	    FREE_AND_SET_TO_NULL(file_src);
		DBG1 (( "found recursion - make module later\n" ))
		return (2); 		
	}

	/* echo target name and version */
	if (tip->protstyle == PROTSTYLE_QUALIFIED_VALUE)
		outstamp ( "%s/%s %-5s", tip->layer, tip->name , versname(tip->version) );
	else
		outstamp ( "%-9s %-5s", tip->name , versname(tip->version) );

	/* if ( tip->stat.debug   || opt_debug   ) outstamp_add ( " %s", "debug" ); CTS 1102466 */
	if ( opt_optdebug)
		outstamp_add ( " %s", "debug (optimized)" ); 
	else
		if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) )
			outstamp_add ( " %s", "debug" ); 
	if ( tip->stat.profile || opt_profile ) outstamp_add ( " %s", "profile" );
	if ( tip->stat.definition ) outstamp_add ( " %s", "definition" );
	if ( tip->stat.binary && ! tip->stat.definition ) outstamp_add ( " %s", "binary" );
	if ( tip->stat.ascii) outstamp_add ( " %s", "ascii" );
	if ( tip->stat.text) outstamp_add ( " %s", "text" );
	outstamp_add ( "\n" );

		/* change to proper directory */
	rc = set_work_dir ( tip->version );
	
	if ( rc < 0 )
	{
		DBG8 (( "makothinc: setwrkdir error %d \n", rc ))
		return ( - 3 );
	}
	DBG8 (( "makothinc: set work dir '%s' \n", tmp_dir ))

	/* get local target file name */

	tip->date = 0l ;
	(void) new_file ( name , DIR_INC , 
        is_oldstyled_source( tip->name, NULL, NULL ) ? EMPTY : tip->layer ,
        EMPTY , &tip->file.file );
	tip->status = PATH_OWN ;

	if ( opt_uncondincl && (stat ( tip->file.file , &statbuf ) != -1) )
	{
	    rc = unlink_file ( tip->file.file );
		if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while unlinking %s \n", tip->file.file));
	}

	/* copy always sources to tmp  */
	rc = vcopy_file ( file_src , tip->name , date_src , 
				tip->stat.binary ? FT_BIN : FT_TXT );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", file_src , tip->name));

	/* CTS 1102268 */
	if (! (tip->stat.binary || tip->stat.text) ) /* CTS 1105829 */
	{
		/* unpack module frame */
		rc = cmdpgm ( env_TOOLSHELL );
		if ( env_TOOLOPT ) 
                rc |= cmdarg( 1, env_TOOLOPT);
		if ( is_global_include( tip->name ) || tip->stat.ascii )   /* CTS 1110365 */
			rc |= cmdscript ( unpackinc(tip) );
		else
			rc |= cmdscript ( unpackexp(tip) );

		if ( opt_keep & 2 ) 
            rc |= cmdarg( 1,  "-k" );
		/* if ( tip->stat.debug   || opt_debug ) CTS 1102466 */
		if ( opt_optdebug)
			rc |= cmdarg( 1,  "-optdebug%c1", DSC_DELIMITER );
		else
			if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) )
						rc |= cmdarg( 1,  "-debug%c1", DSC_DELIMITER );

		if (tip->stat.ascii) rc |= cmdarg( 1,  "-ascii" ); /* CTS 1110365 */

		if (tip->stat.noextension) rc |= cmdarg( 1,  "-noextension" ); /* CTS 1112700 */

		rc |= cmdarg( 1,  "-language%c%c", DSC_DELIMITER, tip->language );
		if ( rc ) return ( rc );
		for ( sip = tip->descriptions ; sip != NULL ; sip = sip->next )
		{
			/* descriptors beginning alnum are concom options */
			/* if ( ! isalnum(sip->descriptor[0]) ) continue ; */
			/* now: always "-" before - parse by mf_opts.pl */
			rc |= cmdarg( 1,  "-%s%s", sip->descriptor , sip->value );
			if ( rc ) return ( rc );
		}
		/*
		if ( status_src & PATH_REMOTE )
		*/
			rc |= cmdarg( 1,  tip->name );
		/*
		else    
		    rc |= cmdarg( 1,  file_src );
		*/

		for ( dip = tip->dependencies ; dip != NULL ; dip = dip->next )
		{
			if ( dip->target->stat.deptype == DT_INCDEP )
			{
				/*
				if (dip->target->fixed_file && strlen(dip->target->afile) > 0 )
					if ( dip->target->stat.noextension)
						rc |= cmdarg( 1,  "%s." , dip->target->afile );  
					else
						rc |= cmdarg( 1,  "%s" , dip->target->afile );
				else
				*/
					if (dip->target->file.file)
						if ( dip->target->stat.noextension)
							rc |= cmdarg( 1,  "%s." , dip->target->file.file );  /* CTS 1112700 */
						else
							rc |= cmdarg( 1,  "%s" , dip->target->file.file );
			}
		}
		if ( rc ) return ( rc );

		/* all demands will be copied to work directory */
		rc = copy_demands ( tip );
		if ( rc ) 
			return ( ewrite_all (tip, 1, -2, "ERROR: error while copying demands\n"));
		    
		rc = cmdexe ( );
		/* CTS 1112006 */
		flush_outputlist (tip, rc, 1);
		if (rc)
			return ( rc );

		/* remove copied demand objects */
		rc = remove_demands ( tip );
		if (rc) 
			return ( ewrite_all (tip, 1, -2, "ERROR: error while deleting demands\n"));
	}
	else
	{
		/* if not remote tools copy to tmp */
		/* if binary or ascii -> no tools -> copy here */
		/* CTS 1102268 */
		if ( ! (status_src & PATH_REMOTE) )
		{
			rc = vcopy_file ( file_src , tip->name , date_src , 
					tip->stat.binary ? FT_BIN : FT_TXT );
			if (rc) 
				return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", file_src , tip->file));
		}
	}

    if ( ( status_src & PATH_REMOTE ) && strcmp(tip->name, name) ) 
		(void) unlink_file ( tip->name );

	/* now touch after move */
	/* CTS 1110368 */
	if (vopt_current_date)
	{
		tip->date = time ( (time_t *) 0 );
		tip->date |= 1 ; /* force an odd value for vmake generated files */
	}
	else
	{
		tip->date = date_make > date_src ? date_make : date_src;
		if ( date_dep > tip->date )
			tip->date = date_dep;
	}

	rc = touch_file ( tip, name, tip->date , "" , TRUE );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while touching %s\n", name));

	
	/* move file */
	rc = move_file ( name , tip->file.file );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while moving %s to %s \n", name, tip->file.file));

    /* fixed_file */
    if ( tip->fixed_file )
    {
		rc = 0;
		for (fip = tip->file.next; fip; fip = fip->next)
		{
			if (vcopy_file ( tip->file.file , fip->file , tip->date , tip->stat.binary ? FT_BIN : FT_TXT ))
			{
				rc--;
				ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", tip->file.file , fip->file);
			}
		}
    	if ( rc ) return ( rc );
    }

    if ( vopt_dates )
	{
	    rc = make_dates_file ( tip, date_src );
		if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while creating date file\n"));
	}

    rc = 2 ; /* really made */

    FREE_AND_SET_TO_NULL(file);
    FREE_AND_SET_TO_NULL(file_src);

	DBG4 (( "makothinc: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/

static  int     make_module 
(
    struct trg_info         * tip ,
    struct trg_info         * ctip ,        /* calling target info pointer */
    time_t                  date_dep ,      /* date of newest dependency */
    int                     make_option,
	int                     found_recursion
	
)
{
	int				rc=0 ;
	int				status_c , status_p , status_s ;
	int				status_g ;
				/* status_o is tip->status */
	int				need_s , need_p , need_c , need_o ;
	int				found_s , found_p , found_c , found_o ;
	int				remake=0 , old_p , old_c , old_o ;
	int				use_s , use_p , use_c ;
	int				got_gsize = 0 ;
	time_t			date_g ;
	time_t			date_s , date_p , date_c , date_o ;
	/* date_o is tip->date */
	
	long			size_o ;
	struct mem_info * mip ;
	struct obj_info	* oip ;
    struct extdep_info * edip, *eldest_extdep, *newest_extdep;
	struct stat		statbuf ;
	struct file_info * fip;
	
	char            name_s [ FILENAMESIZE ] ;
	char            name_p [ FILENAMESIZE ] ;
	char            name_c [ FILENAMESIZE ] ;
	char            name_o [ FILENAMESIZE ] ;
	char            name_g [ FILENAMESIZE ] ;
	char			* file   = NULL;
	char            * file_s = NULL ;
	char            * file_p = NULL ;
	char            * file_c = NULL ;
	char            * file_g = NULL ;
			/* file_o is tip->file.file */

	char			*p;
	valid_bit_arr	vbits ;
	

	DBG2 (( "makmod: called \n" ))
	DBG3 (( "makmod: target '%s' \n", tip->name ))
	DBG3 (( "makmod: caller '%s' \n", ctip ? ctip->name : "" ))
	DBG3 (( "makmod: depend '%s' \n", ascdate(date_dep) ))
	DBG3 (( "makmod: option  %d  \n", make_option ))

    VALID_BIT_FILL ( vbits );
	
    /*
	The following code is very complicated. It was much simpler in
	former times. But then the need of the "C-port" arose. With the
	C-port, PASCAL modules are made by converting the PASCAL code to
	C code and then compiling the C code. The main problem is that
	the intermediate PASCAL and C files should be kept. They could be
	manually modified. Of course, their dates have to be considered,
	and the make operation has to be adapted to the current situation.

	Another problem is that the code has to work with PASCAL ports too.
	One step more is to let it work with only PASCAL files or only
	C files kept. Other variations could be to start the make process
	at the C file level or at the pascal level or to do a pure
	object port or to stop at the pascal, C, or object level.

	The several files with the languages PASCAL, C, and Assembler
	are to be understood to exist on four levels:

	    - module frame level (source level)
		Here, all sources are packed in the "module frame".
		They have to be unpacked before they can be given
		to the respective compiler.

	    - pascal level
		Here, all sources are unpacked from their module frame.
		This level exists if manual modifications have to be made
		to these files. The name "pascal level" has been selected
		because most of the modules are written in PASCAL. So the
		majority of files on this level will be PASCAL files.
		But note that files of each language will be kept on this
		level. With this concept, all compilable files can be
		found on this level. Hence other tools, relying on these
		files need not make differences between the modules (take
		PASCAL files directly and unpack module frames for other
		languages).

	    - C level
		The remarks to the previous paragraph principally apply to
		this also. The only difference is that no PASCAL files
		can exist at this level.
		If the C level AND the pascal level are used, the make
		operation for non-PASCAL modules at this level is to
		simply copy them from the pascal level to the C level.

	    - object level
		This level contains object code files, which can be linked
		to form executable programs.

	Each of the above levels may be used or not used. This is
	determined by the "vmake option".
	Each of the levels has it's own 'level character', which can be
	given in lowercase or uppercase characters. Generally, a lowercase
	character means that this level's file will be used and updated,
	if already existent. An uppercase character means that this level's
	file must be present before or will be generated.
	*/

    /*
     * prepare environment
     */

	need_s = 0 ;
	need_p = 0 ;
	need_c = 0 ;
	need_o = 0 ;
	if ( tip->stat.level_s > 1 ) need_s = 1 ; /* 'S' */
	if ( tip->stat.level_p > 1 ) need_p = 1 ; /* 'P' */
	if ( tip->stat.level_c > 1 ) need_c = 1 ; /* 'C' */
	if ( tip->stat.level_o     ) need_o = 1 ; /* 'o' or 'O' */
	DBG8 (( "makmod: make option '" ))
	if ( tip->stat.level_s ) DBG8 (( "%c" , (tip->stat.level_s > 1) ? 'S' : 's' ))
	if ( tip->stat.level_p ) DBG8 (( "%c" , (tip->stat.level_p > 1) ? 'P' : 'p' ))
	if ( tip->stat.level_c ) DBG8 (( "%c" , (tip->stat.level_c > 1) ? 'C' : 'c' ))
	if ( tip->stat.level_o ) DBG8 (( "%c" , (tip->stat.level_o > 1) ? 'O' : 'o' ))
	DBG8 (( "' \n" ))

	/*
	 *  Try to locate all of the files and get their dates.
	 *  The start file (regarding to vmake option) must be present.
	 */

	if (opt_distrib)
	{
		/* PTS 1108421 */
		if ( tip->stat.distribute )
		{
			struct stat fileinfo;
			for (fip = tip->file.next; fip; fip = fip->next)
			{
				if ( !fip->nodistrib )
				{
					rc = stat( fip->file, & fileinfo );
					if ( ( ( rc == 0 && ( fileinfo.st_mode & S_IFDIR )) ||  
						( fip->file[ strlen( fip->file ) - 1 ] == '/' ))            /* CTS 1107469 */
						&& ctip )
					{
						fip->file = (char *) repalloc( fip->file, strlen( fip->file ) + strlen (tip->name) +
								( fip->file[ strlen( fip->file ) - 1 ] == '/' ? 1 : 2) );
						if ( fip->file[ strlen( fip->file ) - 1 ] != '/' )
							strcat (fip->file, "/");
						strcat (fip->file, tip->name);
					}

					rc = insert_dist( fip->file );
					if ( rc < 0 ) return rc;
				}
			}
		}
		return( 1 );
	}		

	found_s = 0 ;
	status_s = PATH_LEVEL ;
	if ( tip->stat.level_s && ( make_option == MK_FULL || tip->stat.uncond ) )
	{
	    /* strip the variant identifier */
		if (tip->stat.realname)
			strcpy (name_s, tip->name);
		else
			(void) get_basename( tip->name, name_s );

	    DBG8 (( "makmod: searching source file '%s' \n", name_s ))

		if ( tip->local_source )
		{
			struct    stat fileinfo;
			int       missing_slash;
			rc = stat( tip->local_source, & fileinfo );
			missing_slash = tip->local_source[ strlen( tip->local_source ) - 1 ] == '/' ? 0 : 1;
            if  ( (rc == 0 && ( fileinfo.st_mode & S_IFDIR ) )  ||  ( missing_slash == 0 ) )
			{
				char    * tmp_source;

				tmp_source = palloc (strlen(tip->local_source) + missing_slash + strlen(name_s) + 1);
				(void) strcpy (tmp_source, tip->local_source);
				if ( missing_slash )
					(void) strcat (tmp_source, "/" );
				(void) strcat ( tmp_source, name_s );

				FREE_AND_SET_TO_NULL(tip->local_source);
				tip->local_source = tmp_source;
				rc = stat( tip->local_source, & fileinfo );
			}
			if ( rc == 0 ) /* found file */
				date_s = fileinfo.st_mtime;
			else
			{
				return (ewrite_all (tip, 0, -2, "ERROR: missing source    '%s' \n" ,tip->local_source));
			}
			status_s = PATH_OWN;
			file_s = tip->local_source;
			rc = 1; 
		}
		else
		{
		    rc = get_file( name_s , DIR_SRC , tip->layer , EMPTY ,
				FK_SOURCE | FK_LEVEL , vbits ,
				&file_s , & date_s , & status_s, 0 );
			if ( rc < 0 )
				return (ewrite_all (tip, 1, rc, "ERROR: file read error for '%s' (%d)\n" ,name_s, rc));
			if ( rc == 0 && need_s )
			{
				DBG1 (("Missing file (%s) - look for it agian ...\n",  name_s)) 
				rc = get_file( name_s , DIR_SRC , tip->layer , EMPTY , FK_SOURCE | FK_LEVEL , vbits ,
				               &file_s , & date_s , & status_s, 1 );
			}
		}
	    if ( rc == 0 && need_s )
	    {
			return (ewrite_all (tip, 0, -2, "ERROR: missing source    '%s' \n" ,name_s));
	    }
	    if ( rc > 0 )
	    {
			DBG1 (( "%s %s \n", ascdate(date_s) , file_s ))
            mprotwrite ( "%s src/%s/%s,%d\n", ascdate(date_s) , tip->layer, tip->name, 
                status_s & PATH_LEVEL );
			found_s = 1 ;
			if ( ( status_s & PATH_LEVEL ) < tip->minpathlvl )
			{
				tip->minpathlvl = status_s & PATH_LEVEL ;
				DBG5 (( "makmod: target '%s' minpathlvl %d src \n",
							tip->name , tip->minpathlvl ))
			}

			/* find dates file */
			if ( vopt_dates )
			{
				rc = get_dates_file ( tip,
					FK_OBJECT | (tip->minpathlvl & FK_LEVEL) ,
					date_s , vbits );
				if ( rc < 0 )
				{
					DBG4 (( "get file error, returning %d \n", rc ))
					return( rc );
				}
			}

			/*
			 *  With VMAKE_OPTION=V get all sources into OWN tree.
			 */
			if ( vopt_source_copy && ! (status_s & PATH_OWN) )
			{
				outstamp ( "%-9s %-5s <%s>\n", EMPTY , EMPTY , file_s );
				(void) new_file ( name_s , DIR_SRC , tip->layer ,
								EMPTY , &file );
				rc = vcopy_file ( file_s , file , date_s , tip->stat.binary ? FT_BIN : FT_TXT );
				if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", file_s , file));

				file_s = (char *) repalloc( file_s, strlen(file ) + 1 );
				(void) strcpy ( file_s , file );
				status_s = (status_s & PATH_LEVEL) | PATH_OWN ;
			}
	    }
	}

	found_p = 0 ;
	status_p = tip->minpathlvl & PATH_LEVEL ;
	if ( tip->stat.level_p && (make_option == MK_FULL || tip->stat.uncond) )
	{
		if ( extension_exists(tip->name) || tip->stat.ascii || tip->stat.binary || tip->stat.text )
		    (void) sprintf ( name_p , "%s", tip->name);
		else
		    (void) sprintf ( name_p , "%s.%s", tip->name , suffix(tip) );

		DBG8 (( "makmod: searching P level file '%s' \n", name_p ))

	    rc = get_file ( name_p , DIR_PSRC , versname(tip->version) ,
			tip->layer , FK_OBJECT | (status_p & FK_LEVEL) , vbits ,
			&file_p , & date_p , & status_p, 0 );

	    if ( rc < 0 )
			return (ewrite_all (tip, 1, rc, "ERROR: file read error for '%s' (%d)\n" ,name_p, rc));
		
	    if ( rc == 0 && need_p && ! tip->stat.level_s )
			return (ewrite_all (tip, 0, -2, "ERROR: missing pascal level file '%s' \n", name_p ));
	    if ( rc > 0 || need_p )
    		DBG1 (( "%s %s \n", ascdate(date_p) , file_p ))
	    if ( rc > 0 )
	    	found_p = 1 ;
	}

	found_c = 0 ;
	status_c = status_p & PATH_LEVEL ;
	if ( tip->stat.level_c && (make_option == MK_FULL || tip->stat.uncond) )
	{

		if ( extension_exists(tip->name) )
			(void) sprintf ( name_c , "%s", tip->name );
		else
			(void) sprintf ( name_c , "%s.%s", tip->name , csuffix(tip) );

		DBG8 (( "makmod: searching C level file '%s' \n", name_c ))

	    rc = get_file ( name_c , DIR_CSRC , versname(tip->version) ,
			tip->layer , FK_OBJECT | (status_c & FK_LEVEL) , vbits ,
			&file_c , & date_c , & status_c, 0 );
	    if ( rc < 0 )
			return (ewrite_all (tip, 1, rc, "ERROR: file read error for '%s' (%d)\n" ,name_c, rc));

	    if ( rc == 0 && need_c && ! tip->stat.level_p && ! tip->stat.level_s )
			return (ewrite_all (tip, 0, -2, "ERROR: missing C level file '%s' \n", name_c ));

	    if ( rc > 0 || need_c )
    		DBG1 (( "%s %s \n", ascdate(date_c) , file_c ))
	    if ( rc > 0 )
	    	found_c = 1 ;
	}

	/*
	 *  With the "no dependencies option" or with a pure object port
	 *  modules may not really to be made. Nevertheless, in these cases
	 *  the name of the object file must be built.
	 *  Though the name is needed anyway, we need to know where the
	 *  source is located when searching for the object. Thus, we cannot
	 *  move the following code to the top.
	 *  For 'noobject' we simulate a fruitless object
	 *  search altought is isn't neccessary
	 */

	found_o = 0 ;
    tip->status = status_c & PATH_LEVEL ;

	if ( tip->stat.level_o )
	{
		if ( tip->stat.binary || tip->stat.ascii || tip->stat.text || tip->stat.checkdate ) /* CTS 1105829 */
			(void) sprintf ( name_o , "%s", tip->name );
		else
		{
			if ( p = extension_exists(tip->name) )
			{	
				(void) strncpy( name_o, tip->name, p - tip->name );
				name_o [ p - tip->name ] = '\0';
				(void) strcat( name_o, get_objectext(tip));
			}
			else
				(void) sprintf ( name_o , "%s%s", tip->name, get_objectext(tip) );
			
		}

		DBG8 (( "makmod: searching object file '%s' \n", name_o ))

	   /*
	    * Try to locate independent object file.
	    * Even if a module is to be built into libraries only
	    * (during this run), an eventually existing object file
	    * will be updated too.
	    * Therefore, if this object file exists, change the
	    * module's target type to TT_MOD. This forces to keep the
	    * object file in the object's directory.
	    */

		if ( ! ( tip->stat.noobject || tip->objects ) )
		{
			if ( ! tip->stat.noobjcopy  ) 
			    rc = get_file ( name_o , DIR_OBJ , versname(tip->version) , tip->layer ,
	    		    FK_OBJECT | (tip->status & FK_LEVEL) , vbits ,
			    &tip->file.file , & tip->date , & tip->status, 0 );
			else
			    /* only local search */
			    rc = get_file ( name_o , DIR_OBJ , versname(tip->version) , tip->layer ,
	    		    FK_LIB | FK_LEVEL, vbits ,
			        &tip->file.file , & tip->date , & tip->status, 0 );
	        if ( rc < 0 )
				return (ewrite_all (tip, 1, rc, "ERROR: file read error for '%s' (%d)\n" ,name_o, rc));

	        if ( rc > 0 )
	        {
				DBG1 (( "%s %s \n", ascdate(tip->date) , tip->file.file ? tip->file.file : tip->name ))
		        found_o = 1 ;
		        DBG7 (( "changing to TT_MOD '%s' \n", tip->name ))
		        tip->type = TT_MOD ;
	        }
	        else
	        {
	         /*
		      *  At this point, no object file was found.
		      *  If the target type is TT_MEM,
		      *  it can also be a library member.
		      */
		        if ( tip->type == TT_MEM )
	            {
		            DBG7 (( "makmod: library member '%s' \n", name_o ))

		            (void) new_file ( name_o , DIR_TMP ,
					        versname(tip->version) ,
					        EMPTY , &tip->file.file );
		            tip->status = PATH_OWN ;

		            /* try to locate library member */
	        
		            rc = - 1 ;
		            if ( ctip != NULL )
		            {
				        for ( mip = ctip->members ;
					          mip != NULL ;
					          mip = mip->next )
				        {
		    			        rc = strcmp ( mip->name , tip->name );
		    			        if ( rc >= 0 ) break ;
				        }
		            }
		            if ( rc == 0 )
		            {
				        found_o = 1 ;
		    	        tip->date = mip->date ;
		            }
		            else
		            {
				        mip = NULL ;
		    	        tip->date = 0l ;
		            }
		            DBG1 (( "%s %s:%s \n", ascdate(tip->date) ,
						        ctip->file.file  ? ctip->file.file : ctip->name, name_o ))
	            }
	        }
		}

		if ( ( oip = tip->objects ) && ! ( tip->stat.noobject ) && ! ( tip->stat.checkdate ) )
		{
			found_o = 1 ;
			for ( ; oip ; oip = oip->next )
			{
			    if ( ! tip->stat.noobjcopy )
					rc = get_file ( oip->name_o , DIR_OBJ , versname(tip->version) , tip->layer ,
	    				FK_OBJECT | (tip->status & FK_LEVEL) , vbits ,
				        &oip->file , & date_o , & tip->status, 0 );
			    else
			    /* only local search */
					rc = get_file ( oip->name_o , DIR_OBJ , versname(tip->version) , tip->layer ,
	    			    FK_LIB | FK_LEVEL , vbits ,
				        &oip->file , & date_o , & tip->status, 0 );
				
				if ( (tip->date < date_o ) && found_o ) tip->date = date_o;

				if ( rc < 0 )
					return (ewrite_all (tip, 1, rc, "ERROR: file read error for '%s' (%d)\n" ,name_o, rc));
			    
			    if ( rc > 0 )
			    {
    			    found_o = found_o & 1 ;
			    }
			    else
			    {
			        found_o = 0;
			        tip->date = 0l;
			    }
			    DBG1 (( "%s %s \n", ascdate(tip->date) , oip->file ))
			}
		}

	    if ( ! tip->stat.level_c && ! tip->stat.level_p && ! tip->stat.level_s && ! found_o )
			return (ewrite_all (tip, 0, -2, "ERROR: missing object '%s' \n", name_o ));

	    if ( need_o && ! found_o && ! ( tip->stat.noobject || tip->stat.noobjcopy ) )
				DBG1 (( "%s %s \n", ascdate(tip->date) , tip->file.file ? tip->file.file : tip->name ))

	    if ( tip->stat.shrglob == 1 )
	    {
		    (void) sprintf ( name_g , "%s.gsz", tip->name );
		    DBG8 (( "makmod: searching gsize  file '%s' \n", name_g ))
		    rc = get_file ( name_g , DIR_GSIZ ,
				    versname(tip->version) , EMPTY ,
				    FK_OBJECT | (tip->status & FK_LEVEL) , vbits ,
				    &file_g , & date_g , & status_g, 0 );
		    if ( rc < 0 )
				return (ewrite_all (tip, 1, rc, "ERROR: get file error, returning %d \n", rc ));

		    if ( rc == 0 )
		        if ( ! tip->stat.level_c && ! tip->stat.level_p && ! tip->stat.level_s )
					return (ewrite_all (tip, 0, -2, "ERROR: missing gsize  '%s' \n", name_g ));

		    if ( rc > 0 || need_o )
		    {
		        DBG1 (( "%s %s \n", ascdate(date_g) , file_g ))
		    }
	    }
	}
	DBG8 (( "makmod: found all files needed \n" ))


	if ( make_option != MK_FULL && ! tip->stat.uncond )
	{
	    if ( tip->stat.level_o )
	    {
		    DBG8 (( "makmod: no full make \n" ))
		    if ( tip->status & PATH_REMOTE )
		    {
			    if ( oip = tip->objects )
			        for ( ; oip ; oip = oip->next )
			        {
		                outstamp ( "%-9s %-5s <%s>\n",
			            tip->name , versname(tip->version) , oip->file );
		                rc = new_file ( oip->name_o , DIR_OBJ ,
				            versname(tip->version) , tip->layer , &file );
						if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while creating %s \n", oip->name_o));
		                rc = vcopy_file ( oip->file , file , tip->date , FT_BIN );
		                if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", oip->file , file));
			        }
			    else
			        if ( ! tip->stat.noobject )
			        {
		                outstamp ( "%-9s %-5s <%s>\n",
			            tip->name , versname(tip->version) , tip->file.file  ? tip->file.file : tip->name );
		                rc = new_file ( name_o , DIR_OBJ ,
				            versname(tip->version) , tip->layer , &file );
		                if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while creating %s \n", name_o));

		                rc = vcopy_file ( tip->file.file , file , tip->date , FT_BIN );
						if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", tip->file.file , file));
			        }

			    if ( tip->stat.shrglob == 1 )
		        {
			        rc = copy_shrglob ( tip , (char*) 0 );
			        if ( rc ) return ( rc );
		        }

			    if ( ! tip->objects )
			    {
                    tip->file.file = (char *) repalloc( tip->file.file, strlen( file ) + 1 );
		            (void) strcpy ( tip->file.file , file );
			    }
		        tip->status = PATH_OWN ;

		    }

            /* fixed_file */
            if ( tip->fixed_file )
            {
				rc = 0;
				for (fip = tip->file.next; fip; fip = fip->next)
				{
					if (vcopy_file ( tip->file.file , fip->file , tip->date , FT_BIN ))
					{
						rc--;
						ewrite_all (tip, 1, -1, "ERROR: error while copying %s to %s\n", tip->file.file , fip->file);
					}
				}
    	        if ( rc ) return ( rc );
            }
		    return ( 1 );
	    }
	    else
	    {
		    DBG0 (( "WARNING: vmake option excludes objects '%s' \n",
						    tip->name ))
		    return ( 0 );
	    }
	}

	/*
	    If vmake option indicates that only source level is concerned,
	    this can be used to test the existence of the source files.
	    If vmake option indicates that only object level is concerned,
	    this can be used for a pure object port.
	    Other single level options are of no sense.

	    In either case the make process can stop at this point.
	*/

	rc = 0 ;
	if ( tip->stat.level_s > 1 ) rc ++ ;
	if ( tip->stat.level_p > 1 ) rc ++ ;
	if ( tip->stat.level_c > 1 ) rc ++ ;
	if ( tip->stat.level_o     ) rc ++ ;
	DBG8 (( "makmod: %d levels required \n", rc ))

	if ( rc < 2 )
	{
	    /* only one level required, stop at this point */

	    if ( tip->stat.level_s > 1 || tip->stat.level_o > 1 )
	    {
			/* 'S' or 'O' specified, everything ok */
			DBG4 (( "makmod: stopping after name generation \n" ))
			return ( 1 );
	    }
	    else
			return (ewrite_all (tip, 0, -2, "ERROR: senseless vmake option, module '%s' \n", tip->name ));
	}

	/*
	 *  At this point we have the file names and dates of all concerned
	 *  files. Non-existent files got date zero (oldest date possible).
	 *  The next operation is to compare the dates of all these files
	 *  to determine which files to remake.
	 *  To simplify this algorithm, the dates of files which are not
	 *  concerned are set so that they seem up to date.
	 *
	 *  If unconditional make is requested, all concerned files are made.
	 *
	 *  The dates of the dependencies (include files) matter only if
	 *  the source is of concern, because their code is already included
	 *  in the files during unpack of the module frame. Or, in other
	 *  words, there are no include files on pascal or C level except
	 *  of the UNIX system include files.
	 */

	DBG8 (( "makmod: computing which actions to perform \n" ))
	DBG8 (( "makmod: need_s  %d \n", need_s ))
	DBG8 (( "makmod: need_p  %d \n", need_p ))
	DBG8 (( "makmod: need_c  %d \n", need_c ))
	DBG8 (( "makmod: need_o  %d \n", need_o ))
	DBG8 (( "makmod: found_s %d \n", found_s ))
	DBG8 (( "makmod: found_p %d \n", found_p ))
	DBG8 (( "makmod: found_c %d \n", found_c ))
	DBG8 (( "makmod: found_o %d \n", found_o ))

	if ( ! found_s && ! need_s ) date_s = 0 ;
	if ( ! found_p && ! need_p ) date_p = date_s ;
	if ( ! found_c && ! need_c ) date_c = date_p ;
	if ( ! found_o && ! need_o ) tip->date = date_c ;

	DBG8 (( "makmod: date_s  %s \n", ascdate(date_s) ))
	DBG8 (( "makmod: date_p  %s \n", ascdate(date_p) ))
	DBG8 (( "makmod: date_c  %s \n", ascdate(date_c) ))
	DBG8 (( "makmod: date_o  %s \n", ascdate(tip->date) ))

	/*
	    All files, which are already present, need to be maintained.
	*/

	if ( found_p ) need_p = 1 ;
	if ( found_c ) need_c = 1 ;
	if ( found_o ) need_o = 1 ;

	DBG4 (( "makmod: all files present are needed \n" ))
	DBG4 (( "makmod: need_s  %d \n", need_s ))
	DBG4 (( "makmod: need_p  %d \n", need_p ))
	DBG4 (( "makmod: need_c  %d \n", need_c ))
	DBG4 (( "makmod: need_o  %d \n", need_o ))

	remake = 0 ;
	old_p = 0 ;
	old_c = 0 ;
	old_o = 0 ;

    if ( opt_sizeof && tip->language != LANG_PASCAL_P[ 0 ] )
	{
		DBG8 (( "makmod: sizeof and no PASCAL module \n" ))
		return ( 1 );
	}

	if ( opt_lint && ! ( tip->language == LANG_C   [ 0 ] || 
						 tip->language == LANG_CPP [ 0 ] ) )
	{
		DBG8 (( "makmod: lint and no C/C++ module \n" ))
		return ( 1 );
	}

    /* check for external dependencies */
	if ( tip->ext_dependencies )
    {
        /* look for eldest depedency */
        eldest_extdep = tip->ext_dependencies;
		newest_extdep = tip->ext_dependencies;
        for ( edip = tip->ext_dependencies ; edip != NULL ; edip = edip->next )
        {
            if ( eldest_extdep->date > edip->date ) eldest_extdep = edip;
			if ( newest_extdep->date < edip->date )  newest_extdep = edip;
        }
        if ( tip->stat.noobject )
        {
            if ( date_p > eldest_extdep->date + 30*TOL ) 
            {
                tip->stat.uncond = 1;
				DBG1 (( "%s %s (extdep)\n", ascdate( eldest_extdep->date ), eldest_extdep->file ))
            }
        }
        else
        {
            if ( tip->date > eldest_extdep->date + 30*TOL )
            {
                tip->stat.uncond = 1;
				DBG1 (( "%s %s (extdep)\n", ascdate( eldest_extdep->date ), eldest_extdep->file ))
            }
        }
		/* CTS 1110368 */
		if ( vopt_current_date )
		{	if ( tip->date < eldest_extdep->date )	tip->date = eldest_extdep->date; }
		else
		{   if ( tip->date < newest_extdep->date )  tip->date = newest_extdep->date; }
    }


	/*
	 *  During a local make, local objects are used regardless of their
	 *  age relative to the source, except that the source is local too.
	 *  The normal make algorithm may be used if the dates of the
	 *  unused remote files are cleared.
	 */
	if ( vopt_local )
	{
	    if ( status_s & PATH_OWN )
	    {
	      DBG1 (( "makmod: Source local: normal operation \n" ))
	    }
	    else
	    {
            if ( status_p & PATH_OWN )
            {
                DBG1 (( "makmod: P file local: ignore Source date \n" ))
                date_s = 0 ;
            }
            else
            {
                if ( status_c & PATH_OWN )
                {
                    DBG1 (( "makmod: C file local: ignore Source and P date \n" ))
                    date_p = 0 ;
                    date_s = 0 ;
                }
                else
                {
                    if ( tip->status & PATH_OWN )
                    {
                        DBG1 (( "makmod: Object local: ignore S, P, C date \n" ))
                        date_c = 0 ;
                        date_p = 0 ;
                        date_s = 0 ;
                    }
                    else
                    {
                        DBG1 (( "makmod: nothing local: normal operation \n" ))
                    }
                }
            }
	    }
	}

	if ( tip->stat.uncond || opt_retrieve /* done by vmakcoll.c 
					    || opt_sizeof || opt_lint */ )
	{
	    DBG1 (( "unconditional     %s \n", tip->name ))
	    remake = 1 ;
	    if ( need_p && found_s ) old_p = 1 ;
	    if ( need_c && ( found_s || found_p ) ) old_c = 1 ;
	    if ( need_o && ( found_s || found_p || found_c ) ) old_o = 1 ;
	}
	/*
	    Check PASCAL level file, if file is required or existent.
	*/
	if ( need_p && ! remake )
	{
	    if ( date_p < date_s - TOL )
	    {
			remake = 1 ;
			old_p  = 1 ;
	        if ( need_c ) old_c = 1 ;
	        if ( need_o ) old_o = 1 ;
	    }
	    /*
		Date checking against the include modules is sensful
		only, if the source file exists.
	    */
	    if ( found_s && ! remake )
	    {
	        if ( date_p < date_dep - TOL ||
		     ! VALID_BIT_TST(status_p&PATH_LEVEL,vbits) )
	        {
			    remake = 1 ;
	            old_p  = 1 ;
	            if ( need_c ) old_c = 1 ;
	            if ( need_o ) old_o = 1 ;
	        }
	    }
	}
	/*
	    Check C level file, if file is required or existent.
	*/
	if ( need_c && ! remake )
	{
	    if ( date_c < date_p - TOL )
	    {
	        remake = 1 ;
			old_c  = 1 ;
	        if ( need_o ) old_o = 1 ;
	    }
	    /*
		Date checking against the include modules is meaningful
		only, if the source file exists.
	    */
	    if ( found_s && ! remake )
	    {
	        if ( date_c < date_dep - TOL ||
		     ! VALID_BIT_TST(status_c&PATH_LEVEL,vbits) )
	        {
				remake = 1 ;
	            old_c  = 1 ;
	            if ( need_o ) old_o = 1 ;
	        }
	    }
	}
	/*
	    Check object file, if file is required or existent.
	*/
	if ( need_o && ! remake )
	{
		/* 
		 * Don't remake target if it has a valid date
		 * but there are no object 
		 */
		if ( tip->stat.noobject && VALID_BIT_TST(tip->status&PATH_TYPE,vbits) && (tip->date < date_c) )
			tip->date = date_c;

	    if ( tip->date < date_c - TOL )
	    {
	        remake = 1 ;
			old_o  = 1 ;
	    }
	    /*
		Date checking against the include modules is meaningful
		only, if the source file exists.
	    */
		/* PTS 1104635 */
	    if ( found_s && ! remake )
	    {
		    if ( tip->date < date_dep - TOL )
			{
			    remake = 1 ;
	            old_o  = 1 ;
	        }
			else 
				if ( ! VALID_BIT_TST(tip->status&PATH_LEVEL,vbits)  && ! tip->stat.noobject )
				{
					remake = 1 ;
					old_o  = 1 ;
				}	            
	    }
	    if ( (tip->stat.shrglob == 1) && ! remake )
	    {
		    if ( tip->date != date_g )
	        {
			    remake = 1 ;
	            old_o = 1 ;
	        }
	    }
		/* &gar PTS 1103379-1 + PTS 1103379 */ 
		/*only PTS 1103379-1: if (( max_made > 1 ) && ( tip->stat.noobjcopy ) )*/
		/* if any dependencies made or copy and ( noobjcopy or any dependency local ) => remake */
		if (( tip->max_depmade > 1 ) && (( tip->stat.noobjcopy ) || (tip->minpathlvl<1)) )
		{
			remake = 1;
			old_o = 1 ;
		}
	}

	
	if ( ! remake )
	{
        /*
         * all is ok
         * get newest objects files, C level files or P level Files
         * if these files are remote files
         */

	    if ( (tip->status & PATH_REMOTE) ||
	         (status_c & PATH_REMOTE) ||
	         (status_p & PATH_REMOTE) || 
			 /* &gar PTS 1103379 */
			 ( tip->max_depmade > 1 ) && ( ! tip->stat.noobjcopy ))
	    {
			
			/* CTS 1112727 */
			/* if recursion found the module will be maked later once again */
			/* return 2 is necessary for dependencies */
			if (found_recursion) 
			{
				DBG1 (( "found recursion - make module later\n" ))
				FREE_AND_SET_TO_NULL(file);
				FREE_AND_SET_TO_NULL(file_s);
				FREE_AND_SET_TO_NULL(file_p);
				FREE_AND_SET_TO_NULL(file_c);
				FREE_AND_SET_TO_NULL(file_g);
				return (2); 
					
			}

		    if (( tip->status & PATH_REMOTE ) || ( tip->max_depmade > 1 ))
		    {
			    if ( oip = tip->objects )
			        for ( ; oip ; oip = oip->next )
			        {
		                outstamp ( "%-9s %-5s <%s>\n",
			            tip->name , versname(tip->version) , oip->file );
		                rc = new_file ( oip->name_o , DIR_OBJ ,
				            versname(tip->version) , tip->layer , &file );
						if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while creating %s \n", oip->name_o));

						rc = vcopy_file ( oip->file , file , tip->date , FT_BIN );
						if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", oip->file , file));                

                        oip->file = (char *) repalloc( oip->file, strlen( file ) + 1 );
		                (void) strcpy ( oip->file , file );
			        }
			    else
			    {
		            outstamp ( "%-9s %-5s <%s>\n",
			        tip->name , versname(tip->version) , tip->file.file  ? tip->file.file : tip->name);
		            rc = new_file ( name_o , DIR_OBJ ,
				        versname(tip->version) , tip->layer , &file );
		            if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while creating %s \n", name_o));

		            rc = vcopy_file ( tip->file.file , file , tip->date , FT_BIN );
					if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", tip->file.file , file));		            

					/* PTS 1110662 */ 
					if (tip->stat.exec)
					{	
						rc = set_exec_permissions (file);
						if (rc) 
							return (ewrite_all (tip, 1, rc, "ERROR: can't set execute permissions for '%s'\n", file ));
					}

                    tip->file.file = (char *) repalloc( tip->file.file, strlen( file ) + 1 );
		            (void) strcpy ( tip->file.file , file );
					
					/* fixed_file */
                    if ( tip->fixed_file )
                    {
						rc = 0;
						for (fip = tip->file.next; fip; fip = fip->next)
						{
							if (vcopy_file ( tip->file.file , fip->file , tip->date , FT_BIN ))
							{
								rc--;
								ewrite_all (tip, 1, -1, "ERROR: error while copying %s to %s\n", tip->file.file , fip->file);
							}
						}
    					if ( rc ) return ( rc );
                    }
			    }

		        if ( tip->stat.shrglob == 1 )
		        {
			        rc = copy_shrglob ( tip , (char*) 0 );
			        if ( rc ) return ( rc );
		        }

		        tip->status = PATH_OWN ;
		    }
		    if ( status_c & PATH_REMOTE )
		    {
		        outstamp ( "%-9s %-5s <%s>\n",
			    name_c , versname(tip->version) , file_c );
		        rc = new_file ( name_c , DIR_CSRC ,
			        versname(tip->version) , tip->layer , &file );
				if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while creating %s \n", name_c));
		        
		        rc = vcopy_file ( file_c , file , date_c , FT_TXT );
				if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", file_c , file));
		    }
		    if ( status_p & PATH_REMOTE )
		    {
		        outstamp ( "%-9s %-5s <%s>\n",
			    name_p , versname(tip->version) , file_p );
		        rc = new_file ( name_p , DIR_PSRC ,
			        versname(tip->version) , tip->layer , &file );
				if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while creating %s \n", name_p));

		        rc = vcopy_file ( file_p , file , date_p , FT_TXT );
				if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", file_p , file));
		    }
		    if ( vopt_dates )
		    {
		        rc = make_dates_file ( tip, date_s );
		        if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while creating date file\n"));
		    }

            FREE_AND_SET_TO_NULL(file);
            FREE_AND_SET_TO_NULL(file_s);
            FREE_AND_SET_TO_NULL(file_p);
            FREE_AND_SET_TO_NULL(file_c);
            FREE_AND_SET_TO_NULL(file_g);
		    return ( 2 );
	    }

        /* fixed_file */
        if ( tip->fixed_file )
        {
            if ( oip = tip->objects )
            {
                for ( ; oip ; oip = oip->next )
                {
					rc = 0;
					for (fip = tip->file.next; fip; fip = fip->next)
					{
						if (vcopy_file ( oip->file , fip->file , tip->date , FT_BIN ))
						{
							rc--;
							ewrite_all (tip, 1, -1, "ERROR: error while copying %s to %s\n", oip->file , fip->file);
						}
					}
    				if ( rc ) return ( rc );
                }
            }
            else
            {
				rc = 0;
				for (fip = tip->file.next; fip; fip = fip->next)
				{
					if (vcopy_file ( tip->file.file , fip->file , tip->date , FT_BIN ))
					{
						rc--;
						ewrite_all (tip, 1, -1, "ERROR: error while copying %s to %s\n", tip->file.file , fip->file);
					}
				}
    	        if ( rc ) return ( rc );
            }
        }

        FREE_AND_SET_TO_NULL(file);
        FREE_AND_SET_TO_NULL(file_s);
        FREE_AND_SET_TO_NULL(file_p);
        FREE_AND_SET_TO_NULL(file_c);
        FREE_AND_SET_TO_NULL(file_g);

	    DBG2 (( "up to date        %s \n", tip->name ))
	    return ( 1 );
	}

	DBG4 (( "makmod: the following files are old and to rebuild \n" ))
	DBG4 (( "makmod: old_p   %d \n", old_p ))
	DBG4 (( "makmod: old_c   %d \n", old_c ))
	DBG4 (( "makmod: old_o   %d \n", old_o ))

	/*
	 *  At this point we know which files are out of date.
	 *  The last paragraph influenced the file age top down.
	 *  This means, if a file was found out of date, all files which
	 *  depend on it will be set out of date too.
	 *
	 *  Now, determine at which level to begin the make operation.
	 *  This has to be done bottom up because of the influence of the
	 *  dependencies. For instance, if an object is old in respect to the
	 *  dependencies and the C and pascal levels are not concerned,
	 *  we have to begin with the module frame even if the object is
	 *  up to date in respect to the module frame.
	 */

	if (tip->stat.checkdate)
	{
		outstamp ( "%-9s checkdate\n", tip->name );

		if (vopt_current_date)
		{
			tip->date = time ( (time_t *) 0 );
			tip->date |= 1 ; /* force an odd value for vmake generated files */
		}
		else
		{
			tip->date = date_make > date_s ? date_make : date_s;
			if ( date_dep > tip->date )
				tip->date = date_dep;
		}
		rc = touch_file ( tip, tip->file.file, tip->date , "" , FALSE );
		if ( vopt_dates )
		{
			rc = make_dates_file ( tip, date_s );
			if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while creating date file\n"));
		}
	}
	else
	{
		use_s = 0 ;
		use_p = 0 ;
		use_c = 0 ;

		if ( old_o )
		{
			if ( need_c )
				use_c = 1 ;
			else if ( need_p )
				use_p = 1 ;
			else
				use_s = 1 ;
		}
		if ( old_c )
		{
			if ( need_p )
				use_p = 1 ;
			else
				use_s = 1 ;
		}
		if ( old_p ) use_s = 1 ;

		DBG4 (( "makmod: the following files are used for rebuild \n" ))
		DBG4 (( "makmod: use_s   %d \n", use_s ))
		DBG4 (( "makmod: use_p   %d \n", use_p ))
		DBG4 (( "makmod: use_c   %d \n", use_c ))

		/*
		 *  Ok, everything is clear now. Do as requested.
		 */

		 /* change to proper directory */
		if ( vopt_make_in_objdir )
			rc = set_work_dir_in_objdir ( tip->version, tip->layer );
		else
			rc = set_work_dir ( tip->version );
		
		if ( rc < 0 )
		{
			DBG8 (( "makmod: setwrkdir error %d \n", rc ))
			return ( - 3 );
		}
		DBG8 (( "makmod: work dir now '%s' \n", tmp_dir ))

		/* all demands will be copied to work directory */
		rc = copy_demands ( tip );
		if ( rc ) return ( -2 );

		/* 1994-Dec-19 J.Bruehe  I want the source date in a 'what' string */
		whatmod ( tip->name , date_s ); /* environment 'MOD_DAT' */

		if ( opt_uncond )
		{
			DBG8 (( "makmod: remove all files to remake \n" ))
			if ( need_p )
			{
				(void) new_file ( name_p , DIR_PSRC , versname(tip->version) ,
								tip->layer , &file );
				if ( (stat ( file , &statbuf ) != -1) && found_s )
				{
					DBG8 (( "makmod: need P and found S \n" ))
					rc = unlink_file ( file );
					if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while unlinking %s \n", file));					
				}
			}

			if ( need_c )
			{
				(void) new_file ( name_c , DIR_CSRC , versname(tip->version) ,
									tip->layer , &file );
				if ( (stat ( file , &statbuf ) != -1) && (found_s || found_p) )
				{
					DBG8 (( "makmod: need C and found S or P \n" ))
					rc = unlink_file ( file );
					if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while unlinking %s \n", file));
				}
			}

			if ( need_o )
			{
				if ( oip = tip->objects )
					for ( ; oip ; oip = oip->next )
					{
						(void) new_file ( oip->name_o , DIR_OBJ , versname(tip->version) ,
											tip->layer , &file );
						if ( (stat ( file , &statbuf ) != -1) &&
							 ( found_s || found_p || found_c ) )
						{
							DBG8 (( "makmod: need O and found S or P or C \n" ))
							rc = unlink_file ( file );
							if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while deleting %s \n", file));
						}
					}
				else
				{
					(void) new_file ( name_o , DIR_OBJ , versname(tip->version) ,
									tip->layer , &file );
					if ( (stat ( file , &statbuf ) != -1) &&
					 ( found_s || found_p || found_c ) )
					{
						DBG8 (( "makmod: need O and found S or P or C \n" ))
						rc = unlink_file ( file );
						if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while deleting %s \n", file));
					}
				}
			}
		}

		/* CTS 1112727 */
		/* if recursion found the module will be maked later once again */
		/* return 2 is necessary for dependencies                       */
		/* we do so as the module was maked without errors              */
		if (found_recursion) 
		{
			tip->date = time ( (time_t *) 0 );
			tip->date |= 1 ; /* force an odd value for vmake generated files */
			FREE_AND_SET_TO_NULL(file);
			FREE_AND_SET_TO_NULL(file_s);
			FREE_AND_SET_TO_NULL(file_p);
			FREE_AND_SET_TO_NULL(file_c);
			FREE_AND_SET_TO_NULL(file_g);
			DBG1 (( "found recursion - make module later\n" ))
			return (2); 		
		}

		if ( use_s )
		{
			/* echo target name and version */
			if (tip->protstyle == PROTSTYLE_QUALIFIED_VALUE)
				outstamp ( "%s/%s %-5s", tip->layer, tip->name , versname(tip->version) );
			else
				outstamp ( "%-9s %-5s", tip->name , versname(tip->version) );

			if ( tip->stat.shrglob == 1 ) outstamp_add ( " %s", "shrglob" );
			/* if ( tip->stat.debug   || opt_debug   ) outstamp_add ( " %s", "debug" ); CTS 1102466 */
			if (opt_optdebug)  /* CTS 1110663 */
				outstamp_add ( " %s", "debug (optimized)" ); 
			else
				if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) )
					outstamp_add ( " %s", "debug" ); 
			if ( tip->stat.profile || opt_profile ) outstamp_add ( " %s", "profile" );
			if ( tip->stat.definition ) outstamp_add ( " %s", "definition" );
			if ( tip->stat.binary && ! tip->stat.definition ) outstamp_add ( " %s", "binary" );
			if ( tip->stat.ascii ) outstamp_add ( " %s", "ascii" );
			if ( tip->stat.text ) outstamp_add ( " %s", "text" );
			if ( opt_lint ) outstamp_add ( " %s", "lint" );
			if ( opt_sizeof ) outstamp_add ( " %s", "sizeof" );
			outstamp_add ( "\n" );

			if ( ! need_p )
			{
				/* if not need_p, name_p might not be set */
				if ( extension_exists(tip->name) || tip->stat.ascii || tip->stat.text || tip->stat.binary )
					(void) sprintf ( name_p , "%s", tip->name );
				else
					(void) sprintf ( name_p , "%s.%s",
							tip->name , suffix(tip) );

				file_p = (char *) repalloc( file_p, strlen( name_p ) + 1 );
				(void) strcpy ( file_p , name_p );
				use_p = 1 ;
			}

			rc = unpack_module_frame ( tip , file_s , date_s , date_dep, status_s ,
									name_p );
			if ( rc ) return ( rc );

			if ( need_p )
			{
				/* get local target name */
				(void) new_file ( name_p , DIR_PSRC , versname(tip->version) ,
	        			tip->layer , &file_p );
				rc = move_file ( name_p , file_p );
				if ( rc ) 
					return ( ewrite_all (tip, 1, rc, "ERROR: error while moving %s to %s \n", name_p , file_p ));
			}
		}

		if ( ! need_o && ! need_c ) return ( 1 ); /* 2 ? */

		if ( use_p )
		{
			if ( ! use_s && (status_p & PATH_REMOTE) )
			{
				outstamp ( "%-9s %-5s <%s>\n",
					name_p , versname(tip->version) , file_p );
				rc = new_file ( name_p , DIR_PSRC ,
					versname(tip->version) , tip->layer , &file );
				if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while creating %s \n", name_p));

				rc = vcopy_file ( file_p , file , date_p , FT_TXT );
				if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", file_p , file));

				file_p = (char *) repalloc( file_p, strlen( file ) + 1 );
				(void) strcpy ( file_p , file );
			}

			if ( need_p )
			{
				/* echo target name and version */

					if ( extension_exists(tip->name) || tip->stat.ascii || tip->stat.text || tip->stat.binary )
						outstamp ( "%s %s", tip->name, versname(tip->version) );
					else
						outstamp ( "%s.%s %s", tip->name , suffix(tip) ,
									versname(tip->version) );

				if ( tip->stat.shrglob == 1 ) outstamp_add ( " %s", "shrglob" );
				/* if ( tip->stat.debug   || opt_debug   ) CTS 1102466 */
				if (opt_optdebug)  /* CTS 1110663 */
					outstamp_add ( " %s", "debug (optimized)" ); 
				else
					if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) )
									outstamp_add ( " %s", "debug" );
				if ( tip->stat.profile || opt_profile )
								outstamp_add ( " %s", "profile" );
				if ( opt_lint ) outstamp_add ( " %s", "lint" );
				outstamp_add ( "\n" );
			}

			if ( tip->stat.level_c )
			{
				if ( ! need_c )
				{
					/* if not need_c, name_c might not be set */
					int size_of_file_c = sizeof(file_c);

					if ( extension_exists(tip->name) )
						(void) sprintf ( name_c , "%s", tip->name );
					else
						(void) sprintf ( name_c , "%s.%s",
							tip->name , csuffix(tip) );
					
					

					(void) strncpy ( file_c , name_c , strlen(file_c) );
					use_c = 1 ;
				}
				if ( tip->language == LANG_PASCAL_P[ 0 ] )
				{
					rc = compile_file ( tip , file_p , need_p , name_c , FALSE, date_s, date_dep);
					if ( rc ) return ( rc );
					if ( tip->stat.shrglob == 1 ) got_gsize = 1 ;
				}
				if ( need_c )
				{
					/* get local target name */
					(void) new_file ( name_c , DIR_CSRC ,
							versname(tip->version) ,
							tip->layer , &file_c );
					if ( tip->language == LANG_PASCAL_P[ 0 ] )
					{
						/* just created above */
						rc = move_file ( name_c , file_c );
						if ( rc ) 
							return ( ewrite_all (tip, 1, rc, "ERROR: error while moving %s to %s \n", name_c , file_c ));
					}
					else
					{
						rc = link_file ( file_p , file_c );
						if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while linking %s to %s\n", file_p , tip->name));		            


						if (    ! need_p && ! tip->stat.debug
							 && ! opt_debug && ! opt_retrieve && ! opt_optdebug )
						{
							rc = unlink_file ( file_p );
							if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while deleting %s \n", file_p));
						}
					}
				}
			}
			else
			{
				rc = compile_file ( tip , file_p , need_p , name_o , 
					tip->objects ? TRUE : FALSE, date_s, date_dep );
    			if ( rc ) return ( rc );
	    		if ( tip->stat.shrglob == 1 ) got_gsize = 1 ;
			}
		}

		/*IST 1995-11-03 || opt_retrieve: build C file too. */
		if ( ! need_o || opt_retrieve ) return ( 1 );

		if ( use_c )
		{
			if ( ! use_s && ! use_p && (status_c & PATH_REMOTE) )
			{
				outstamp ( "%-9s %-5s <%s>\n",
					name_c , versname(tip->version) , file_c );
				rc = new_file ( name_c , DIR_CSRC ,
					versname(tip->version) , tip->layer , &file );
				if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while creating %s \n", name_c));
				
				rc = vcopy_file ( file_c , file , date_c , FT_TXT );
				if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", file_c , file));

				file_c = (char *) repalloc( file_c, strlen( file ) + 1 );
				(void) strcpy ( file_c , file );
			}

			if ( need_c )
			{
    			/* echo target name and version */
				if ( extension_exists(tip->name) )
					outstamp ( "%s %s", tip->name, versname(tip->version) );
				else
					outstamp ( "%s.%s %s", tip->name , csuffix(tip) ,
								versname(tip->version) );
				if ( tip->stat.shrglob == 1 ) outstamp_add ( " %s", "shrglob" );
				if (opt_optdebug)  /* CTS 1110663 */
					outstamp_add ( " %s", "debug (optimized)" ); 
				else
					if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) )
								outstamp_add ( " %s", "debug" );
				if ( tip->stat.profile || opt_profile )
							outstamp_add ( " %s", "profile" );
				if ( opt_lint ) outstamp_add ( " %s", "lint" );
				outstamp_add ( "\n" );
			}

			if ( tip->language == LANG_PASCAL_P[ 0 ] )
			{
				tip->language = LANG_C[ 0 ] ;
				rc = compile_file ( tip , file_c ,
						tip->stat.level_c ? need_c : need_p , name_o , 
						tip->objects ? TRUE : FALSE, date_s, date_dep );
				tip->language = LANG_PASCAL_P[ 0 ] ;
			}
			else
			{
				rc = compile_file ( tip , file_c , need_c , name_o ,
					tip->objects ? TRUE : FALSE, date_s, date_dep );
			}
			if ( rc ) return ( rc );
		}
		
		/*
		 *  At this point, we have built a new object file. This is the
		 *  prerequisite for further make operations (libraries,
		 *  relocatables, programs).
		 *  If the object file is needed for other than libraries, move it
		 *  to the object files directory.
		 *  If the shrglob is set then a .gsz file has to exist.
		 *  Set the date of the gsize file to the same date
		 *  as the object file and move it to the proper directory.
		 */
		if ( ! opt_noexec ) 
		{
			if ( oip = tip->objects )
				for ( ; oip ; oip = oip->next )
				{
					rc = get_date_and_size ( oip->name_o , & tip->date , & size_o , TRUE);
					if ( rc ) return ( rc );

					if ( size_o == 0 )
						DBG0 (( "WARNING: target file is empty '%s' \n", oip->name_o  ))

					if ( size_o < 0 )
						return (ewrite_all (tip, 0, -2, "ERROR: target file is empty '%s' \n", oip->name_o ));
				}
			else
				if ( ! tip->stat.noobject )
				{
					rc = get_date_and_size ( name_o , & tip->date , & size_o , TRUE);
					if ( rc ) return ( rc );

					if ( size_o == 0 )
						DBG0 (( "WARNING: target file is empty '%s' \n", name_o ))

					if ( size_o < 0 )
						return (ewrite_all (tip, 0, -2, "ERROR: target file is empty '%s' \n", name_o ));
				}
		

			if ( got_gsize )
			{
				/* get gsize file name */
				(void) sprintf ( name_g , "%s.gsz" , tip->name );
				if ( tip->objects )
					rc = touch_file (tip, name_g , tip->date , tip->objects->name_o , TRUE );
				else
					rc = touch_file (tip, name_g , 
						tip->stat.noobject ? date_s : tip->date , name_o , TRUE );
				if ( rc ) 
					return ( ewrite_all (tip, 1, rc, "ERROR: error while touching %s\n", name_g));
				/* must exist */
			}

			if ( tip->type == TT_MOD )
			{
				/* get local target file name */
				if ( (oip = tip->objects  ) && ! vopt_make_in_objdir)
				{
					for ( ; oip ; oip = oip->next )
					{
						(void) new_file ( oip->name_o , DIR_OBJ , versname(tip->version) ,
										tip->layer , &oip->file );
						rc = move_file ( oip->name_o , oip->file );
						if ( rc ) 
							return ( ewrite_all (tip, 1, rc, "ERROR: error while moving %s to %s \n", oip->name_o , oip->file ));
					}
					tip->status = PATH_OWN ;
				}
				else
				if ( ! tip->stat.noobject && ! vopt_make_in_objdir )
				{
					(void) new_file ( name_o , DIR_OBJ , versname(tip->version) ,
									tip->layer , &tip->file.file );
					tip->status = PATH_OWN ;
					rc = move_file ( name_o , tip->file.file );
					if ( rc ) 
						return ( ewrite_all (tip, 1, rc, "ERROR: error while moving %s to %s \n", name_o , tip->file.file  ));
				}
				

				/* if exec-flag  CTS 1102283 */ 
				if (tip->stat.exec)
				{	
					rc = set_exec_permissions (tip->file.file);
					if (rc) 
						return (ewrite_all (tip, 1, rc, "ERROR: can't set execute permissions for '%s'\n", tip->file.file ));
				}
				/* fixed_file */
				if ( tip->fixed_file )
				{
					if ( oip = tip->objects )
					{
						for ( ; oip ; oip = oip->next )
						{
							rc = 0;
							for (fip = tip->file.next; fip; fip = fip->next)
							{
								if (vcopy_file ( oip->file , fip->file , tip->date , FT_BIN ))
								{
									rc--;
									ewrite_all (tip, 1, -1, "ERROR: error while copying %s to %s\n", oip->file , fip->file);
								}
							}
    						if ( rc ) return ( rc );
						}
					}
					else
					{
						rc = 0;
						for (fip = tip->file.next; fip; fip = fip->next)
						{
							if (vcopy_file ( tip->file.file , fip->file , tip->date , FT_BIN ))
							{
								rc--;
								ewrite_all (tip, 1, -1, "ERROR: error while copying %s to %s\n", tip->file.file , fip->file);
							}
						}
    					if ( rc ) return ( rc );
					}
				}
			}

			if ( got_gsize )
			{
				/* get local gsize file name */
				(void) new_file ( name_g , DIR_GSIZ , versname(tip->version) ,
									EMPTY , &file );
				rc = move_file ( name_g , file );
				if ( rc ) 
					return ( ewrite_all (tip, 1, rc, "ERROR: error while moving %s to %s \n", name_g , file  ));
			}

			if ( vopt_dates )
			{
				rc = make_dates_file ( tip, date_s );
				if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while creating date file\n"));
			}

			/* touch extdep's with the make timestamp (CTS 1110368) */
			if ( tip->ext_dependencies &&  ! vopt_current_date )
			{
				for ( edip = tip->ext_dependencies ; edip != NULL ; edip = edip->next )
				{
					rc = touch_file ( tip, edip->file, tip->date , "" , TRUE );
					if ( rc ) 
						return ( ewrite_all (tip, 1, rc, "ERROR: error while touching %s\n", edip->file));
				}

			}
		}
		
		/* remove copied demand objects */
		rc = remove_demands ( tip );
		if ( rc ) return rc ;
	}
    rc = 2 ; /* really made */
	

    FREE_AND_SET_TO_NULL(file);
    FREE_AND_SET_TO_NULL(file_s);
    FREE_AND_SET_TO_NULL(file_p);
    FREE_AND_SET_TO_NULL(file_c);
    FREE_AND_SET_TO_NULL(file_g);

	DBG4 (( "makmod: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/

static  int     unpack_module_frame 
(
    struct trg_info * tip ,
    char			* file_src ,
    time_t          date_src ,
	time_t          date_dep,
    int			    status_src ,
    char			* name_obj
)
{
	int			rc=0 ;
	/* time_t			date ; CTS 1110368 */
	struct dep_info         * dip ;
	struct dsc_info         * sip ;
	char			        * file = NULL;

	DBG2 (( "unpackframe: called \n" ))
	DBG3 (( "unpackframe: target '%s' \n", tip->name ))
    
    /* if necessary, copy remote source */

	if ( status_src & PATH_REMOTE )
	{
	    rc = vcopy_file ( file_src , tip->name , date_src , 
				tip->stat.binary ? FT_BIN : FT_TXT);
		if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", file_src , tip->name));		            
	    if ( vopt_source_copy ) /* normally already done!??? */
	    {			    /* this code could be removed later!??? */
			DBG0 (( "WARNING: possibly copied file with variant name!\n" ))
			(void) new_file ( tip->name , DIR_SRC , tip->layer , EMPTY , &file );
			rc = link_file ( tip->name , file );
			if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while linking %s to %s\n", tip->name , file));		            		
	    }
	}
	else
	{
	    if ( ( test_for_variants( tip->name ) ) || tip->stat.binary || tip->stat.text || tip->stat.ascii) /* CTS 1105829 */
	    {
			/* if it is a variant or local binary , get a link (or copy) here */
			/* link only if not variant (PTS 1002016) */
			if ( test_for_variants( tip->name ) || tip->stat.ascii || tip->stat.text || tip->stat.binary  ) 
			{
				rc = vcopy_file ( file_src , tip->name , date_src , 
					tip->stat.binary ? FT_BIN : FT_TXT);
				if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", file_src , tip->name));		            
			}	
			else
			{
				rc = link_file ( file_src , tip->name );
				if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while linking %s to %s\n", file_src , tip->name));		            
			}
			/* use tip->name, unlink later */
			status_src = PATH_REMOTE | (status_src & PATH_LEVEL);
	    }
	}

	if ( tip->stat.binary || tip->stat.text )	
	{
		/* CTS 1110368 */
		if (vopt_current_date)
		{
			tip->date = time ( (time_t *) 0 );
			tip->date |= 1 ; /* force an odd value for vmake generated files */
		}
		else
		{
			tip->date = date_make > date_src ? date_make : date_src;
			if ( date_dep > tip->date )
				tip->date = date_dep;
		}

		rc = touch_file ( tip, tip->name , tip->date , "" , TRUE );
		if ( rc ) 
			return ( ewrite_all (tip, 1, rc, "ERROR: error while touching %s\n", tip->name));
		return ( 0 );/* CTS 1105829 */
	}

	if ( opt_sizeof && ! ( tip->stat.ascii || tip->stat.text)  )
	{
	    /* extract const and type sections */

		rc = cmdpgm ( env_TOOLSHELL );
		if ( env_TOOLOPT ) rc |= cmdarg( 1, env_TOOLOPT);
	    rc |= cmdscript ( unpackinc(tip) );
	    if ( opt_keep & 2 ) rc |= cmdarg( 1,  "-k" );
	    /* if ( tip->stat.debug   || opt_debug ) CTS 1102466 */
		if (opt_optdebug)  /* CTS 1110663 */ 
			rc |= cmdarg( 1,  "-optdebug%c1", DSC_DELIMITER );
		else
			if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) )
				rc |= cmdarg( 1,  "-debug%c1", DSC_DELIMITER );
	    if ( rc ) return ( rc );
	    for ( sip = tip->descriptions ; sip != NULL ; sip = sip->next )
	    {
			/* descriptors beginning alnum are concom options */
			/* if ( ! isalnum(sip->descriptor[0]) ) continue ; */
			/* now: always "-" before - parse by mf_opts.pl */
			rc |= cmdarg( 1,  "-%s%s", sip->descriptor , sip->value );
			if ( rc ) return ( rc );
	    }
	    if ( status_src & PATH_REMOTE )
		    rc |= cmdarg( 1,  tip->name );
	    else    rc |= cmdarg( 1,  file_src );
		rc = cmdexe ( );
	    /* CTS 1112006 */
		flush_outputlist (tip, rc, 1);
		if (rc)
			return ( rc );
	    if ( status_src & PATH_REMOTE ) (void) unlink_file ( tip->name );

	    /* create size file */

		rc = cmdpgm ( env_TOOLSHELL );
		if ( env_TOOLOPT ) rc |= cmdarg( 1, env_TOOLOPT);
	    rc |= cmdscript ( CMD_MFSIZE );
	    if ( opt_keep & 2 ) rc |= cmdarg( 1,  "-k" );
	    for ( dip = tip->dependencies ; dip != NULL ; dip = dip->next )
	    {
		/* PASCAL include file name is without .con or .typ */
			if (dip->target->file.file)
			{
				rc |= cmdarg( 1,  "%s.con" , dip->target->file.file );
				rc |= cmdarg( 1,  "%s.typ" , dip->target->file.file );
			}
	    }
	    rc |= cmdarg( 1,  "%s.con" , tip->name );
	    rc |= cmdarg( 1,  "%s.typ" , tip->name );
		rc = cmdexe ( );
	    /* CTS 1112006 */
		flush_outputlist (tip, rc, 1);
		if (rc)
			return ( rc );
		file = (char *) repalloc( file, strlen( tip->name ) + strlen( ".siz" ) + 1 );
        (void) sprintf ( file , "%s.siz" , tip->name );

		if (vopt_current_date)
		{
			tip->date = time ( (time_t *) 0 );
			tip->date |= 1 ; /* force an odd value for vmake generated files */
		
		}
		else
		{
			tip->date = date_make > date_src ? date_make : date_src;
			if ( date_dep > tip->date )
				tip->date = date_dep;
		}

	    rc = touch_file ( tip, file , tip->date , "" , TRUE );
		if ( rc ) 
			return ( ewrite_all (tip, 1, rc, "ERROR: error while touching %s\n", file));

		FREE_AND_SET_TO_NULL(file);

	    DBG8 (( "makmod: sizeof, returning 1 \n" ))
	    return ( 1 );
	}

	/* unpack module frame */
	rc = cmdpgm ( env_TOOLSHELL );
	if ( env_TOOLOPT ) rc |= cmdarg( 1, env_TOOLOPT);
	rc |= cmdscript ( unpackmod(tip) );
	rc |= cmdarg( 1,  "-%c"  , 	(tip->version == ' ' || tip->version == 'n') ? vmakeversion : tip->version );
	if ( opt_keep & 2 ) rc |= cmdarg( 1,  "-k" );
	if (tip->stat.ascii) rc |= cmdarg( 1,  "-ascii" ); /* CTS 1110365 */
	if (tip->stat.noextension) rc |= cmdarg( 1,  "-noextension" ); /* CTS 1112700 */
	/* if ( tip->stat.debug   || opt_debug ) CTS 1102466 */
	if (opt_optdebug)  /* CTS 1110663 */ 
		rc |= cmdarg( 1,  "-optdebug%c1", DSC_DELIMITER );
	else
		if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) )
			rc |= cmdarg( 1,  "-debug%c1", DSC_DELIMITER );
	rc |= cmdarg( 1,  "-language%c%c", DSC_DELIMITER, tip->language );
	if ( rc ) return ( rc );
	for ( sip = tip->descriptions ; sip != NULL ; sip = sip->next )
	{
			/* always "-" before  */
			rc |= cmdarg( 1,  "-%s%s", sip->descriptor , sip->value );
			if ( rc ) return ( rc );
	}
	if ( status_src & PATH_REMOTE )
		rc |= cmdarg( 1,  tip->name );
	else    rc |= cmdarg( 1,  file_src );
	for ( dip = tip->dependencies ; dip != NULL ; dip = dip->next )
	{
		if ( dip->target->made < 0 )
		{
		    DBG0 (( "'%s' not made because of dependency errors \n",
							    tip->name ))
			ewrite_targetheader (tip, -2);
			eprotwrite ( "not made because of dependency errors of: \n"); 
			for ( dip = tip->dependencies ; dip != NULL ; dip = dip->next )
			{
				if ( dip->target->made < 0 )
					eprotwrite ( "        %s\n", dip->target->name); 
			}
			eprotwrite ( EPROT_SEPARATOR ); 

		    DBG8 (( "makmod: dependency not made '%s' \n",
							    dip->target->name ))
		    return ( - 2 );
		}
		if ( dip->target->stat.deptype != DT_INCDEP ) 
            continue;
		if (dip->target->file.file)
		{
			/*
			if (dip->target->fixed_file && ( strlen(dip->target->afile) >0 ) )
				if ( dip->target->stat.noextension)
					rc |= cmdarg( 1,  "%s." , dip->target->afile );  
				else
					rc |= cmdarg( 1,  dip->target->afile );
			else
				*/
				if ( dip->target->stat.noextension)
					rc |= cmdarg( 1,  "%s." , dip->target->file.file );  /* CTS 1112700 */
				else
					if ( dip->target->language == LANG_PASCAL_P[ 0 ] )
					{
						if ( tip->language == LANG_PASCAL_P[ 0 ] )
						{
							rc |= cmdarg( 1,  "%s.con", dip->target->file.file );
							rc |= cmdarg( 1,  "%s.typ", dip->target->file.file );
						}
						else
						{
							/*IST 1997-04-08 */
							if ( extension_exists(dip->target->file.file) )
								rc |= cmdarg( 1,  "%s", dip->target->file.file );
							else
								rc |= cmdarg( 1,  "%s.h", dip->target->file.file );
						}

					}
					else
					{
		   				rc |= cmdarg( 1,  dip->target->file.file );
					}
		}
		if ( rc ) return ( rc );
	}
	if ( rc ) return ( rc );
	rc = cmdexe ( );
	flush_outputlist (tip, rc, 1);
	if ( rc ) return ( rc );
	if ( (status_src & PATH_REMOTE) && 
			strcmp( tip->name, name_obj ) ) 
		(void) unlink_file ( tip->name );

	/* CTS 1110368 */
	if (vopt_current_date)
	{
		tip->date = time ( (time_t *) 0 );
		tip->date |= 1 ; /* force an odd value for vmake generated files */
	}
	else
	{
		tip->date = date_make > date_src ? date_make : date_src;
		if ( date_dep > tip->date )
			tip->date = date_dep;
	}
	

	rc = touch_file ( tip, name_obj , tip->date , "" , TRUE );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while touching %s\n", name_obj));

	FREE_AND_SET_TO_NULL(file);

    return ( 0 );
}


/*==========================================================================*/

static  int     compile_file 
(
    struct trg_info * tip ,
    char			* file_src ,
    int             need_src ,
    char			* name_obj ,
    int				use_tip_obj,
	time_t          date_src,
	time_t          date_dep
)
{
	int                     rc=0 ;
	int                     offset ;
	int                     optc ;
	/* time_t			date ; CTS 1110368 */
	struct dsc_info         * sip ;
	struct obj_info			* oip ;

	DBG2 (( "compfil: called \n" ))
	DBG3 (( "compfil: target '%s' \n", tip->name ))

    if ( tip->stat.binary || tip->stat.ascii || tip->stat.text)	return ( 0 ); /* CTS 1105829 */

	rc = cmdpgm ( env_TOOLSHELL );
	if ( env_TOOLOPT ) rc |= cmdarg( 1, env_TOOLOPT);
	rc |= cmdscript ( opt_lint ? CMD_COMPLINT : compiler(tip) );
	if ( opt_lint ) rc |= cmdarg( 1,  "-c" );
	if ( opt_keep & 2 ) rc |= cmdarg( 1,  "-k" );
	if ( tip->stat.shrglob == 1 ) rc |= cmdarg( 1,  "-Y" );
	/* if ( tip->stat.debug   || opt_debug ) CTS 1102466 */
	if (opt_optdebug)  /* CTS 1110663 */ 
		rc |= cmdarg( 1,  "-optdebug%c1", DSC_DELIMITER );
	else
		if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) ) 
			rc |= cmdarg( 1,  "-g" );
	if ( tip->stat.profile || opt_profile ) rc |= cmdarg( 1,  "-p" );
	rc |= cmdarg( 1,  "-language%c%c", DSC_DELIMITER, tip->language );
	optc = optchar ( tip );
	for ( sip = tip->descriptions ; sip != NULL ; sip = sip->next )
	{
	    /* descriptors beginning alnum are concom options */
	    if ( isalnum(sip->descriptor[0]) ) continue ;

	    /*
	     *  Descriptors beginning with '%X' are meant for the language X
	     *  only (specified by the character after the '%').
	     *  '%%' is for all languages, transformed to '%'.
	     */
	    if ( sip->descriptor[0] == '%' )
	    {
		if ( sip->descriptor[1] == '%' )
		{
		    /* found %% mapping to % */
		    offset = 1 ;
		}
		else
		{
		    if ( sip->descriptor[1] != optc ) continue ;
		    /* found %X where X is the char for this compiler */
		    offset = 2 ; /* skip '%X' of the descriptor string */
		}
	    }
	    else
	    {
		    offset = 0 ;
        }
	    rc |= cmdarg( 1,  "%s%s", sip->descriptor + offset , sip->value );
	    if ( rc ) return ( rc );
	}
    if ( use_tip_obj && tip->objects )
	    rc |= cmdarg( 1,  "-output%c%s", DSC_DELIMITER, tip->objects->name_o );
	rc |= cmdarg( 1,  file_src );
	if ( rc ) return ( rc );
	rc = cmdexe ( );

	/* CTS ERRROFILE */
	flush_outputlist (tip, rc, 1);
	if (rc)
		return ( rc );
	
	/* CTS 1110368 */
	if (vopt_current_date)
	{
		tip->date = time ( (time_t *) 0 );
		tip->date |= 1 ; /* force an odd value for vmake generated files */
	}
	else
	{
		tip->date = date_make > date_src ? date_make : date_src;
		if ( date_dep > tip->date )
			tip->date = date_dep;
	}

	/* use_tip_obj == TRUE use name_obj , otherwise use tip->objects */
	if ( ! use_tip_obj && ! tip->stat.noobject )
	{
		rc = touch_file ( tip, name_obj , tip->date , "" , TRUE ); 
		if ( rc ) 
			return ( ewrite_all (tip, 1, rc, "ERROR: error while touching %s\n", name_obj));
	}
	else
	{
		for ( oip = tip->objects ; oip ; oip = oip->next )
		{
		    rc = touch_file ( tip, oip->name_o , tip->date , "" , TRUE ); 
			if ( rc ) 
				return ( ewrite_all (tip, 1, rc, "ERROR: error while touching %s\n", oip->name_o));
		}
	}

	if ( opt_lint ) return ( 1 );

	if ( ! need_src && ! opt_keep && ! opt_retrieve &&
				! tip->stat.debug && ! opt_debug && ! opt_optdebug)
	{
	    rc = unlink_file ( file_src );
		if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while deleting %s \n", file_src));
	}

	return ( 0 );
}


/*==========================================================================*/

#ifndef	RELOC_LIB
static  int     make_library 
(
    struct trg_info         * tip 
)
{
	int                     remake=0 , lgt , rc=0 , ec ;
	int			end_of_options ;
	struct dep_info         * dip ;
	struct dep_info         * dip_nobind ;  /* CTS 1105891 */
	struct dsc_info         * sip ;
	struct mem_info         * mip ;
	struct obj_info         * oip ;
	struct stat		statbuf ;
	char                    * trgfile = NULL;
	char                    name [ FILENAMESIZE ] ;


	DBG4 (( "maklib: called \n" ))

	if ( opt_sizeof || opt_lint || opt_retrieve || ! tip->stat.level_o ||
							    opt_nolink )
	{
		DBG8 (( "maklib: no objects \n" ))
		return ( 1 );
	}

	/* CTS 1111841 -> */
	if (tip->layer)
		mprotwrite ( "%s desc/%s/%s,%d\n", ascdate(tip->date) , tip->layer , tip->name, 
			(tip->stat.level_s - 1) & PATH_LEVEL );
	else
		mprotwrite ( "%s desc/%s,%d\n", ascdate(tip->date) , tip->name, 
			(tip->stat.level_s - 1) & PATH_LEVEL );
	/* <- CTS 1111841 */

	/* target file already searched and copied by "search_library" */
	/* get local target file name */

	if ( tip->fixed_file )
	{
	    /* tip->file.file contains the target file name. Save it. */
	    trgfile = strdup( tip->file.file ) ;
	    if ( ! trgfile )
			return (ewrite_all (tip, 0, -3, "ERROR: insufficient dynamic memory \n" ));
	    /* vmake creates a fixed target with version suffix */
        tip->file.file = (char *) repalloc( tip->file.file, (tip->file.file ? strlen( tip->file.file ) : 0)  +  3 );
	    (void) sprintf ( tip->file.file , "%s.%c", trgfile , tip->version );
	}
	else
	{
	    /* the target file is always in the OWN tree */
	    (void) sprintf ( name , "%s.%s", tip->name, env_ARC_TYPE );
	    (void) new_file ( name , DIR_OBJ , versname(tip->version) ,
						tip->layer , &trgfile );
	}

	/*
	 *  At this point we have to decide whether the library is
	 *  up to date or not. For normal targets, we compare the
	 *  target's date against other dates. The date of the
	 *  library file, however, is meaningless. Instead, the
	 *  insertion dates of the library members have to be
	 *  compared against the dates of the already made dependencies.
	 */
	remake = 0 ;
	for ( dip = tip->dependencies ; dip != NULL ; dip = dip->next )
	{
	    if ( dip->target->made <= 0 ) continue ; /* target not made */
	    /* find member in library */
	    lgt = strlen ( dip->target->name );
	    /* strip '.o' from a file name */
	    if ( dip->target->type == TT_FILE ) lgt -= 2 ;
	    FILL ( name , '\0' , sizeof(name) );
	    COPY ( name , dip->target->name , lgt );
	    ec = - 1 ;
	    for ( mip = tip->members ; mip != NULL ; mip = mip->next )
	    {
		ec = strcmp ( mip->name , name );
		if ( ec >= 0 ) break ;
	    }
	    /*
	     *  If member is not in library or
	     *  object is newer than member or
	     *  object is nearer than library then
	     *  remake is necessary
	     */
	    if ( ec || (dip->target->date > mip->date + TOL) ||
		 ((dip->target->status & PATH_LEVEL) <
				(tip->status & PATH_LEVEL)) ||
		 (dip->target->made > 1) /* IST 1997-04-14 */
	       )
	    {
		remake ++ ;
	    }
	}

	if ( ! tip->stat.uncond && ! remake )
	{
	    DBG2 (( "up to date        %s \n", tip->name ))
	    if ( tip->fixed_file )
	    {
		/* vmake links the existing target to the real target file */
		rc = link_file ( tip->file.file , trgfile );
		if ( rc ) 
			return ( ewrite_all (tip, 1, rc, "ERROR: error while linking %s to %s\n", tip->file.file , trgfile));

		if ( rc ) return ( rc );

	    }

		if ( tip->variantfile )
		{
			rc = link_file ( tip->file.file , tip->variantfile->file );
			if ( rc ) 
				return ( ewrite_all (tip, 1, rc, "ERROR: error while linking %s to %s\n", tip->file.file , tip->variantfile->file));
		}


        FREE_AND_SET_TO_NULL(trgfile);

	    return ( 1 );
	}
	else
    for ( dip = tip->remakes ; dip ; dip = dip->next )
        if ( dip->target->made != 2 )
		{
            DBG4(("make_lib: remake %s \n", dip->target->name))
			dip->target->made = 0;
			dip->target->stat.uncond = 1;
			make ( dip->target, tip, MK_FULL );
		}
    
    if ( tip->stat.uncond )
	    DBG1 (( "unconditional     %s \n", tip->name ))

	/* echo target and options */

	outstamp ( "%-9s %-5s", tip->name , versname(tip->version) );
	if ( tip->stat.shrglob == 1 ) outstamp_add ( " %s", "shrglob" );
	/* if ( tip->stat.debug   || opt_debug   ) outstamp_add ( " %s", "debug" ); CTS 1102466 */
	if (opt_optdebug)  /* CTS 1110663 */
			outstamp_add ( " %s", "debug (optimized)" ); 
	else
		if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) ) 
			outstamp_add ( " %s", "debug" );
	if ( tip->stat.profile || opt_profile ) outstamp_add ( " %s", "profile" );
	outstamp_add ( "\n" );

	/* change to proper directory */

	rc = set_work_dir ( tip->version );
	if ( rc < 0 )
	{
	    DBG8 (( "maklib: setwrkdir error %d \n", rc ))
	    return ( -3 );
	}
	DBG8 (( "maklib: set work dir '%s' \n", tmp_dir ))

	if ( opt_uncond && (tip->stat.level_s|tip->stat.level_p|tip->stat.level_c) &&
		(stat ( trgfile , &statbuf ) != -1) )
	{
	    rc = unlink_file ( trgfile );
		if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while deleting %s \n", trgfile));
	}

	/*
	 *  If "searched" library is not in local tree,
	 *  copy it into the local tree.
	 *  This copies all modules within that library and thus avoids
	 *  to build them all anew.
	 */
	if ( tip->status & PATH_LEVEL )
	{
	    if ( ! strcmp(tip->file.file, trgfile) )
	    {
			outstamp ( "%-9s %-5s <%s>\n", EMPTY , EMPTY , tip->file.file );
			rc = vcopy_file ( tip->file.file ? tip->file.file : tip->name , trgfile , tip->date , FT_BIN );
			if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while copying %s to %s\n", tip->file.file ? tip->file.file : tip->name , trgfile));			
		}

	    if ( tip->stat.shrglob == 1 )
	    {
			rc = copy_shrglob ( tip , (char*) 0 );
			if ( rc ) return ( rc );
	    }

        tip->file.file = (char *) repalloc( tip->file.file, strlen( trgfile ) + 1 );
	    (void) strcpy ( tip->file.file , trgfile );
	    tip->status = PATH_OWN ;
	}

	/* build command */

	rc = cmdpgm ( env_TOOLSHELL );
	if ( env_TOOLOPT ) rc |= cmdarg( 1, env_TOOLOPT);
	rc |= cmdscript ( CMD_ARCHIVE );
	if ( opt_keep & 2 ) rc |= cmdarg( 1,  "-k" );
	if ( tip->stat.shrglob == 1 ) rc |= cmdarg( 1,  "-Y" );
	
	/* if ( tip->stat.debug        || opt_debug   ) rc |= cmdarg( 1,  "-g" ); CTS 1102466 */
	if (opt_optdebug)  /* CTS 1110663 */ 
		rc |= cmdarg( 1,  "-optdebug", DSC_DELIMITER );
	else
		if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) ) 
			rc |= cmdarg( 1,  "-g" );
	if ( tip->stat.profile == 1 || opt_profile ) rc |= cmdarg( 1,  "-p" );

	for ( sip = tip->descriptions ; sip != NULL ; sip = sip->next )
	{
	    /* descriptors beginning alnum are concom options */
	    if ( isalnum(sip->descriptor[0]) ) continue ;
	    rc |= cmdarg( 1,  "%s%s", sip->descriptor , sip->value );
	    if ( rc ) return ( rc );
	}

	/*
	 *  Put all leading options from the dependencies list
	 *  in front of the target file name.
	 */
	for ( dip = tip->options ; dip != NULL ; dip = dip->next )
	{
		rc |= cmdarg( 1,  dip->target->file.file );
	}
	end_of_options = FALSE ; 
	for ( dip = tip->dependencies ; dip != NULL ; dip = dip->next )
	{
	    if ( dip->target->made <= 0 )
	    {
			DBG7 (( "dependency not made '%s' \n", dip->target->name ))
			continue ;
	    }

	    /* find member in library */
	    ec = - 1 ;
	    for ( mip = tip->members ; mip != NULL ; mip = mip->next )
	    {
			ec = strcmp ( mip->name , dip->target->name );
			if ( ec >= 0 ) break ;
	    }
	    if ( ec == 0 && dip->target->date <= mip->date - TOL )
	    {
			DBG7 (( "maklib: dependency older '%s' \n", dip->target->name ))
			continue ;
	    }

        /* CTS 1105891 -> */
		for ( dip_nobind=tip->nobinds; dip_nobind !=NULL; dip_nobind=dip_nobind->next )
		{
			if ( dip_nobind->target == dip->target )
				break;
		}
		if ( dip_nobind != NULL )
			continue;
		/* <- CTS 1105891 */

		/* PTS 1108170 */
		if ( (dip->target->type == TT_MAC ) )
		{
			DBG0 (( "Warning: mac description %s called in %s without 'nobind'\n",
								dip->target->name, tip->name ))
			continue;
		}


	    if ( ! end_of_options && (dip->target->type != TT_OPTION) )
	    {
			/*
			 *  Found first non-option argument.
			 *  Output the target file first.
			 *  Options following non-option arguments are position
			 *  dependent options (e.g. library options to the linker).
			 */
			end_of_options = TRUE ;
			rc |= cmdarg( 1,  tip->file.file );
	    }
#if ! unix
	    if ( dip->target->type == TT_DLL )
			rc |= cmdarg( 1,  dip->target->afile );
	    else
#endif	
			if ( oip = dip->target->objects )
				for ( ; oip ; oip = oip->next )
					rc |= cmdarg_obj( 1, tip->version, oip->file ); /* CTS 1105495 */
			else
				if ( ! dip->target->stat.noobject )
					rc |= cmdarg_obj ( 1, tip->version, dip->target->file.file ); /* CTS 1105495 */
	    
		if ( rc ) return ( rc );
	}
	if ( rc ) return ( rc );

	/* execute comand, built above */

	rc = cmdexe ( );
	/* CTS 1112006 */
	flush_outputlist (tip, rc, 1);
	if (rc)
		return ( rc );


	tip->date = time ( (time_t *) 0 );
	tip->date |= 1 ; /* force an odd value for vmake generated files */
	rc = touch_file ( tip, tip->file.file , tip->date , "", TRUE );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while touching %s\n",  tip->file.file));


	/* no move; library is rebuilt in place */

	/* set link to correctly named target */

	if ( tip->fixed_file )
	{
	    rc = link_file ( tip->file.file , trgfile );
		if ( rc ) 
			return ( ewrite_all (tip, 1, rc, "ERROR: error while linking %s to %s\n", tip->file.file , trgfile));
	}

	if ( tip->variantfile )
	{
		rc = link_file ( tip->file.file , tip->variantfile->file );
		if ( rc ) 
			return ( ewrite_all (tip, 1, rc, "ERROR: error while linking %s to %s\n", tip->file.file , tip->variantfile->file));
	}


	if ( vopt_dates )
	{
	    rc = make_libpgm_dates_file ( tip );
	    if ( rc ) return ( rc );
	}

	rc = 2 ; /* really made */

    FREE_AND_SET_TO_NULL(trgfile);

	DBG4 (( "maklib: returning %d \n", rc ))
	return ( rc );
}
#endif	/*RELOC_LIB*/


/*==========================================================================*/

static  int     make_reloc_library 
(
    struct trg_info         * tip 
)
{
	int                     remake=0 , rc=0, rc1=0 ;
	int			ec ;
	int			end_of_options ;
	time_t			date_dat ;
	struct dep_info         * dip ;
	struct dep_info         * dip_nobind ;  /* CTS 1105891 */
	struct dsc_info         * sip ;
	struct mem_info         * mip ;
	struct obj_info         * oip ;
    struct file_info        * fip ;
	struct stat		statbuf ;
	char                    * trgfile = NULL;
	char                    name [ FILENAMESIZE ] ;
	char                    targetname [ FILENAMESIZE ] ;
	valid_bit_arr		vbits ;
	
	DBG2 (( "makrellib: called \n" ))

    VALID_BIT_FILL ( vbits );

	if ( opt_sizeof || opt_lint || opt_retrieve || tip->stat.level_o < 2 ||
								opt_nolink )
	{
		DBG8 (( "makrellib: no objects \n" ))
		return ( 1 );
	}

	/* CTS 1111841 -> */
	if (tip->layer)
		mprotwrite ( "%s desc/%s/%s,%d\n", ascdate(tip->date) , tip->layer , tip->name, 
			(tip->stat.level_s - 1) & PATH_LEVEL );
	else
		mprotwrite ( "%s desc/%s,%d\n", ascdate(tip->date) , tip->name, 
			(tip->stat.level_s - 1) & PATH_LEVEL );
	/* <- CTS 1111841 */


	/* find target */

    remove_ext( tip->name, targetname );
	(void) sprintf ( name , "%s.%s", targetname, env_ARC_TYPE );

	if ( tip->fixed_file )
	{
	    /* tip->file.file contains the target file name. Save it. */
	    trgfile = strdup( tip->file.file );
	    if ( ! trgfile )
			return (ewrite_all (tip, 0, -3, "ERROR: insufficient dynamic memory \n" ));

	    /* vmake creates a fixed target with version suffix */
        tip->file.file = (char *) repalloc( tip->file.file, (tip->file.file ? strlen( tip->file.file ) : 0 ) + 3 ); 
	    (void) sprintf ( tip->file.file , "%s.%c", trgfile , tip->version );

	    tip->status = PATH_OWN ;
	    rc = stat ( tip->file.file , &statbuf );
	    if ( rc >= 0 )
	    {
		date_dat = statbuf.st_mtime ;
		rc = 1 ; /* means: file found */
	    }
	    if ( (rc < 0) &&
		 ((geterrno() == ENOENT) || (geterrno() == ENOTDIR)) )
		{
			rc = 0 ; /* means: file not found */
			date_dat = 0;
		}
	}
	else
	{
	    /* the target file is always in the OWN tree */
	    (void) new_file ( name , DIR_OBJ , versname(tip->version) ,
						tip->layer , &trgfile );
	    rc = get_file ( name , DIR_OBJ , versname(tip->version) , tip->layer ,
		    FK_LIB | FK_LEVEL , vbits ,
		    &tip->file.file , & date_dat , & tip->status, 0 );
	}
	if ( rc < 0 )
		return (ewrite_all (tip, 1, rc, "ERROR: file read error for '%s' (%d)\n" ,name, rc));

	remake = ( date_dat < tip->date );
	tip->date = date_dat;
	DBG1 (( "%s %s \n", ascdate(tip->date) , tip->file.file ? tip->file.file : tip->name ))

	/*
	 *  "tip->file.file" now contains the path to an existing file (incl. .f).
	 *  "trgfile" contains the path for the new file.
	 */
    
	 /* &gar CTS 1000237 -> */
	if ( opt_distrib )
	{
		if ( tip->stat.distribute )
		{
			if (! tip->file.nodistrib)
			{
				rc = insert_dist(trgfile);
				if (rc <0) return rc;
			}
			fip = tip->file.next;
			while( fip )
			{
				if (! fip->nodistrib)
				{
					rc = insert_dist(fip->file);
					if (rc <0) return rc;
				}
				fip = fip->next;
			}
		}
		return 1;
	}
	/* <- &gar CTS 1000237 */

	/* build command, while checking dates */

	if ( tip->dependencies == NULL )
	{
	    DBG0 (( "WARNING: library list empty '%s' \n", tip->name ))
	    return ( 0 );
	}

	if ( vopt_dates )
	{
	    rc = read_libpgm_dates_file ( tip , &date_dat );
	    if ( rc ) return ( rc );
	}
	if (date_dat != tip->date )
		remake = 1;

	for ( dip = tip->dependencies ; dip != NULL ; dip = dip->next )
	{
	    ec = -1 ;
	    for ( mip = tip->members ; mip != NULL ; mip = mip->next )
	    {
		ec = strcmp ( mip->name , dip->target->name );
		if ( ec >= 0 ) break ;
	    }
	    /*
	     *  If member is not in target or
	     *  object is newer than target or
	     *  object is newer than member or
	     *  object is older than member or
	     *  object is nearer than library then
	     *  remake is necessary
	     */
	    if (    ec
	         || (dip->target->date > tip->date + TOL)
		 || (dip->target->date > mip->date + TOL)
		 || (dip->target->date < mip->date - TOL)
		 || ((dip->target->status & PATH_LEVEL) <
				(tip->status & PATH_LEVEL))
		 || (dip->target->made > 1) /* IST 1997-04-14 */
	       )
	    {
		DBG1 (( "%s !%s! \n", mip?ascdate(mip->date):ascdate(0) ,
							dip->target->name ))
		remake ++ ;
	    }
    }

	if ( ! tip->stat.uncond && ! remake )
	{
	    DBG2 (( "up to date        %s \n", tip->name ))
	    if ( tip->fixed_file )
	    {
		    /* vmake links the existing target to the real target file */
		    rc = link_file ( tip->file.file , trgfile );
			if (rc) 
				ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , trgfile);
			
			fip = tip->file.next;
            while( fip )
            {
				rc1 = link_file ( tip->file.file , fip->file );
				if (rc1)
				{
					rc |= rc1;
					ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , fip->file);
				}
                fip = fip->next;
            }
			for (fip = tip->variantfile; fip; fip=fip->next)
			{
				rc1 =  link_file ( tip->file.file, fip->file  );
				if (rc1)
				{
					rc |= rc1;
					ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , fip->file);
				}
			}

		    if ( rc ) return ( rc );
	    }

        FREE_AND_SET_TO_NULL(trgfile);
	    return ( 1 );
	}
	else
    for ( dip = tip->remakes ; dip ; dip = dip->next )
        if ( dip->target->made != 2 )
		{
            DBG4(("make_rel_lib: remake %s \n", dip->target->name))
			dip->target->made = 0;
			dip->target->stat.uncond = 1;
			make ( dip->target, tip, MK_FULL );
		}

    if ( tip->stat.uncond )
	    DBG1 (( "unconditional     %s \n", tip->name ))

    rc = cmdpgm ( env_TOOLSHELL );
	if ( env_TOOLOPT ) rc |= cmdarg( 1, env_TOOLOPT);
	rc |= cmdscript ( CMD_ARCHIVE );
	if ( opt_keep & 2 ) rc |= cmdarg( 1,  "-k" );
	if ( tip->stat.shrglob == 1 ) rc |= cmdarg( 1,  "-Y" );
	/* if ( tip->stat.debug        || opt_debug   ) rc |= cmdarg( 1,  "-g" ); CTS 1102466 */
	if (opt_optdebug)  /* CTS 1110663 */ 
		rc |= cmdarg( 1,  "-optdebug%c1", DSC_DELIMITER );
	else
		if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) ) 
			rc |= cmdarg( 1,  "-g" );
	if ( tip->stat.profile == 1 || opt_profile ) rc |= cmdarg( 1,  "-p" );

	for ( sip = tip->descriptions ; sip != NULL ; sip = sip->next )
	{
	    /* descriptors beginning alnum are concom options */
	    if ( isalnum(sip->descriptor[0]) ) continue ;
	    rc |= cmdarg( 1,  "%s%s", sip->descriptor , sip->value );
	    if ( rc ) return ( rc );
	}

	/*
	 *  Put all leading options from the dependencies list
	 *  in front of the target file name.
	 */
	for ( dip = tip->options ; dip != NULL ; dip = dip->next )
	{
		rc |= cmdarg( 1,  dip->target->file.file );
	}
	end_of_options = FALSE ;
	for ( dip = tip->dependencies ; dip != NULL ; dip = dip->next )
	{
	    DBG8 (( "makrellib: dependency found '%s' \n", dip->target->name ))
	    if ( dip->target->made == INIT_STATE )
	    {
			DBG8 (( "WARNING: target dependent from itself '%s' \n",
								dip->target->name ))
			continue ;
	    }
	    if ( dip->target->made < 0 )
		{
				DBG0 (( "'%s' not made because of dependency errors\n",
							tip->name ))
				ewrite_targetheader (tip, -2);
				eprotwrite ( "not made because of dependency errors of: \n"); 
				eprotwrite ( "        %s (remake) \n", dip->target->name );
				eprotwrite ( EPROT_SEPARATOR ); 
				return ( -2 );
		}


	    DBG7 (( "makrellib: dependency date value %ld \n",
							dip->target->date ))

        /* CTS 1105891 -> */
		for ( dip_nobind=tip->nobinds; dip_nobind !=NULL; dip_nobind=dip_nobind->next )
		{
			if ( dip_nobind->target == dip->target )
				break;
		}
		if ( dip_nobind != NULL )
			continue;
		/* <- CTS 1105891 */

		/* PTS 1108170 */
		if ( (dip->target->type == TT_MAC ) )
		{
			DBG0 (( "Warning: mac description %s called in %s without 'nobind'\n",
								dip->target->name, tip->name ))
			continue;
		}

	    
	    if ( ! end_of_options && (dip->target->type != TT_OPTION) )
	    {
		/*
		 *  Found first non-option argument.
		 *  Output the target file first.
		 *  Options following non-option arguments are position
		 *  dependent options (e.g. library options to the linker).
		 */
			end_of_options = TRUE ;
			rc |= cmdarg( 1,  name ); /* create library in tmp dir */
	    }
		if ( dip->target->stat.deptype == DT_OBJDEMDEP || dip->target->stat.deptype == DT_SRCDEMDEP ) continue ;
#if ! unix
	    if ( dip->target->type == TT_DLL )
			rc |= cmdarg( 1,  dip->target->afile );
	    else
#endif
			if ( oip = dip->target->objects )
				for ( ; oip ; oip = oip->next )
					rc |= cmdarg_obj ( 1, tip->version,  oip->file ); /* CTS 1105495 */
			else
				if ( ! dip->target->stat.noobject )
					rc |= cmdarg_obj ( 1, tip->version,  dip->target->file.file ); /* CTS 1105495 */
	    if ( rc ) return ( rc );
	}
	if ( rc ) return ( rc );

	/* echo target and options */

	outstamp ( "%-9s %-5s", tip->name , versname(tip->version) );
	if ( tip->stat.shrglob == 1 ) outstamp_add ( " %s", "shrglob" );
	/* if ( tip->stat.debug   || opt_debug   ) outstamp_add ( " %s", "debug" ); CTS 1102466 */
	if (opt_optdebug)  /* CTS 1110663 */
		outstamp_add ( " %s", "debug (optimized)" );
	else
		if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) ) 
			outstamp_add ( " %s", "debug" );
	if ( tip->stat.profile || opt_profile ) outstamp_add ( " %s", "profile" );
	outstamp_add ( "\n" );

	/* change to proper directory */

	rc = set_work_dir ( tip->version );
	if ( rc < 0 )
	{
		DBG8 (( "makrellib: setwrkdir error %d \n", rc ))
		return ( - 3 );
	}
	DBG8 (( "makrellib: set work dir '%s' \n", tmp_dir ))

	/* get local target file name */

	tip->date = 0l ;
	if ( ! tip->fixed_file )
	{
        tip->file.file = (char *) repalloc( tip->file.file, strlen( trgfile ) + 1 );
	    (void) strcpy ( tip->file.file , trgfile );
	}
	tip->status = PATH_OWN ;

	if ( opt_uncond && (stat ( tip->file.file , &statbuf ) != -1) )
	{
	    rc = unlink_file ( tip->file.file );
		if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while deleting %s \n", tip->file.file));
	}

	/*
	 *  There may be a library from a former run in the directory.
	 *  Avoid that 'ar' tries to use it.
	 */
	if ( stat ( name , &statbuf ) != -1 )
	{
	    rc = unlink_file ( name );
		if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while deleting %s \n", name));
	}

	/* all demands will be copied to work directory */
	rc = copy_demands ( tip );
	if ( rc ) return ( -2 );

	/* execute command, built above */

	rc = cmdexe ( );
	/* CTS 1112006 */
	flush_outputlist (tip, rc, 1);
	if (rc)
		return ( rc );


	/* remove copied demand objects */
	rc = remove_demands ( tip );
	if ( rc ) return rc ;

	tip->date = time ( (time_t *) 0 );
	tip->date |= 1 ; /* force an odd value for vmake generated files */
	rc = touch_file ( tip, name , tip->date , "" , TRUE );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while touching %s\n", name));

	/* move library */

	rc = move_file ( name , tip->file.file );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while moving %s to %s \n", name , tip->file.file  ));

	/* set link to correctly named target */

	if ( tip->fixed_file )
	{

		rc = link_file ( tip->file.file , trgfile );
		if (rc) 
			ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , trgfile);
		
		fip = tip->file.next;
        while( fip )
        {
			rc1 = link_file ( tip->file.file , fip->file );
			if (rc1)
			{
				rc |= rc1;
				ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , fip->file);
			}
            fip = fip->next;
        }
		for (fip = tip->variantfile; fip; fip=fip->next)
		{
			rc1 =  link_file ( tip->file.file, fip->file  );
			if (rc1)
			{
				rc |= rc1;
				ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , fip->file);
			}
		}

	    if ( rc ) return ( rc );
	}

	if ( vopt_dates )
	{
		rc = make_libpgm_dates_file ( tip );
	    if ( rc ) return ( rc );
	}

	rc = 2 ; /* really made */

    FREE_AND_SET_TO_NULL(trgfile);

	DBG4 (( "makrellib: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/

static  int     make_reloc 
(
    struct trg_info         * tip 
)
{
	int                     remake=0 , rc=0, rc1 = 0 ;
	int			ec ;
	int			end_of_options ;
	time_t			date_dat ;
	struct dep_info         * dip ;
	struct dep_info         * dip_nobind ;  /* CTS 1105891 */
	struct dsc_info         * sip ;
	struct mem_info         * mip ;
	struct obj_info			* oip ;
    struct file_info        * fip ;
	struct stat		statbuf ;
	char                    * trgfile = NULL ;
	char                    name [ FILENAMESIZE ] ;
	char                    targetname [ FILENAMESIZE ] ;
	valid_bit_arr		vbits ;
	char				*p;

	DBG2 (( "makrel: called \n" ))

    VALID_BIT_FILL ( vbits );

	if ( opt_sizeof || opt_lint || opt_retrieve || tip->stat.level_o < 2 ||
								opt_nolink )
	{
		DBG8 (( "makrel: no objects \n" ))
		return ( 1 );
	}

	/* CTS 1111841 -> */
	if (tip->layer)
		mprotwrite ( "%s desc/%s/%s,%d\n", ascdate(tip->date) , tip->layer , tip->name, 
			(tip->stat.level_s - 1) & PATH_LEVEL );
	else
		mprotwrite ( "%s desc/%s,%d\n", ascdate(tip->date) , tip->name, 
			(tip->stat.level_s - 1) & PATH_LEVEL );
	/* <- CTS 1111841 */


	/* find target */
    remove_ext( tip->name, targetname );
	if ( p = extension_exists( targetname ) )
	{	
        STRCPY( name, targetname, p - targetname )
		strcat( name, ".o");
	}
	else
		(void) sprintf ( name , "%s.o", tip->name );

	if ( tip->fixed_file )
	{
	    /* tip->file.file contains the target file name. Save it. */
	    trgfile = strdup( tip->file.file );
	    if ( ! trgfile )
			return (ewrite_all (tip, 0, -3, "ERROR: insufficient dynamic memory \n" ));

        tip->file.file = (char *) repalloc( tip->file.file, (tip->file.file ? strlen( tip->file.file ) : 0) + 3);
	    /* vmake creates a fixed target with version suffix */
	    (void) sprintf ( tip->file.file , "%s.%c", trgfile , tip->version );

	    tip->status = PATH_OWN ;
	    rc = stat ( tip->file.file , &statbuf );
	    if ( rc >= 0 )
	    {
		    date_dat = statbuf.st_mtime ;
		    rc = 1 ; /* means: file found */
	    }
	    if ( (rc < 0) && ((geterrno() == ENOENT) || (geterrno() == ENOTDIR)) )
		{
		    rc = 0 ; /* means: file not found */
			date_dat=0;
		}
	}
	else
	{
	    /* the target file is always in the OWN tree */
	    (void) new_file ( name , DIR_OBJ , versname(tip->version) ,
						tip->layer , &trgfile );
	    rc = get_file ( name , DIR_OBJ , versname(tip->version) , tip->layer ,
		    FK_LIB | FK_LEVEL , vbits ,
		    &tip->file.file , &date_dat , & tip->status, 0 );
	}
	if ( rc < 0 )
		return (ewrite_all (tip, 1, rc, "ERROR: file read error for '%s' (%d)\n" , name, rc));
	
	remake = ( date_dat < tip->date );
	tip->date = date_dat;
	DBG1 (( "%s %s \n", ascdate(tip->date) , tip->file.file ? tip->file.file : tip->name ))

	/*
	 *  "tip->file.file" now contains the path to an existing file (incl. .f).
	 *  "trgfile" contains the path for the new file.
	 */
	 /* &gar CTS 1000237 -> */
	if ( opt_distrib )
	{
		if ( tip->stat.distribute )
		{
			if (! tip->file.nodistrib)
			{
				rc = insert_dist(trgfile);
				if (rc <0) return rc;
			}
			fip = tip->file.next;
			while( fip )
			{
				if (! fip->nodistrib)
				{
					rc = insert_dist(fip->file);
					if (rc <0) return rc;
				}
				fip = fip->next;
			}
		}
		return 1;
	}
	/* <- &gar CTS 1000237 */

	/* build command, while checking dates */

	if ( tip->dependencies == NULL )
	{
	    DBG0 (( "WARNING: reloc list empty '%s' \n", tip->name ))
	    return ( 0 );
	}

	if ( vopt_dates )
	{
	    rc = read_libpgm_dates_file ( tip , &date_dat );
	    if ( rc ) return ( rc );
	}
	if (date_dat != tip->date )
		remake = 1;

	for ( dip = tip->dependencies ; dip != NULL ; dip = dip->next )
    {
	    ec = -1 ;
	    for ( mip = tip->members ; mip != NULL ; mip = mip->next )
	    {
		ec = strcmp ( mip->name , dip->target->name );
		if ( ec >= 0 ) break ;
	    }
	    /*
	     *  If member is not in target or
	     *  object is newer than target or
	     *  object is newer than member or
	     *  object is older than member or
	     *  object is nearer than library then
	     *  remake is necessary
	     */
	    if (    ec
	         || (dip->target->date > tip->date + TOL)
		 || (dip->target->date > mip->date + TOL)
		 || (dip->target->date < mip->date - TOL)
		 || ((dip->target->status & PATH_LEVEL) <
				(tip->status & PATH_LEVEL))
		 || (dip->target->made > 1) /* IST 1997-04-14 */
	       )
	    {
		DBG1 (( "%s !%s! \n", mip?ascdate(mip->date):ascdate(0) ,
							dip->target->name ))
		remake ++ ;
	    }
	}

	if ( ! tip->stat.uncond && ! remake )
	{
	    DBG2 (( "up to date        %s \n", tip->name ))
	    if ( tip->fixed_file )
	    {
		    /* vmake links the existing target to the real target file */
			rc = link_file ( tip->file.file , trgfile );
			if (rc) 
				ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , trgfile);
			
			fip = tip->file.next;
            while( fip )
            {
				rc1 = link_file ( tip->file.file , fip->file );
				if (rc1)
				{
					rc |= rc1;
					ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , fip->file);
				}
                fip = fip->next;
            }
			for (fip = tip->variantfile; fip; fip=fip->next)
			{
				rc1 =  link_file ( tip->file.file, fip->file  );
				if (rc1)
				{
					rc |= rc1;
					ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , fip->file);
				}
			}
			if ( rc ) return ( rc );
	    }
        FREE_AND_SET_TO_NULL(trgfile);
	    return ( 1 );
	}
	else
    for ( dip = tip->remakes ; dip ; dip = dip->next )
        if ( dip->target->made != 2 )
		{
            DBG4(("make_rel: remake %s \n", dip->target->name))
			dip->target->made = 0;
			dip->target->stat.uncond = 1;
			make ( dip->target, tip, MK_FULL );
		}

    if ( tip->stat.uncond )
	    DBG1 (( "unconditional     %s \n", tip->name ))

	rc = cmdpgm ( env_TOOLSHELL );
	if ( env_TOOLOPT ) rc |= cmdarg( 1, env_TOOLOPT);
	if ( tip->type == TT_SHR )
	{
		rc |= cmdscript ( CMD_LINKSHR );
	}
	else
	{
		rc |= cmdscript ( CMD_LINKREL );
	}
	if ( opt_keep & 2 ) rc |= cmdarg( 1,  "-k" );
	if ( tip->stat.shrglob == 1 ) rc |= cmdarg( 1,  "-Y" );
	/* if ( tip->stat.debug        || opt_debug   ) rc |= cmdarg( 1,  "-g" ); CTS 1102466 */
	if (opt_optdebug)  /* CTS 1110663 */ 
		rc |= cmdarg( 1,  "-optdebug", DSC_DELIMITER );
	else
		if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) ) 
			rc |= cmdarg( 1,  "-g" );
	if ( tip->stat.profile == 1 || opt_profile ) rc |= cmdarg( 1,  "-p" );

	for ( sip = tip->descriptions ; sip != NULL ; sip = sip->next )
	{
	    /* descriptors beginning alnum are concom options */
	    if ( isalnum(sip->descriptor[0]) ) continue ;
	    rc |= cmdarg( 1,  "%s%s", sip->descriptor , sip->value );
	    if ( rc ) return ( rc );
	}

	/*
	 *  Put all leading options from the dependencies list
	 *  in front of the target file name.
	 */
	for ( dip = tip->options ; dip != NULL ; dip = dip->next )
	{
		rc |= cmdarg( 1,  dip->target->file.file );
	}
	end_of_options = FALSE ;
	for ( dip = tip->dependencies ; dip != NULL ; dip = dip->next )
	{
	    DBG8 (( "makrel: dependency found '%s' \n", dip->target->name ))
	    if ( dip->target->made == INIT_STATE )
	    {
			DBG8 (( "WARNING: target dependent from itself '%s' \n",
								dip->target->name ))
			continue ;
	    }
	    if ( dip->target->made < 0 )
		{
				DBG0 (( "'%s' not made because of dependency errors\n",
							tip->name ))
				ewrite_targetheader (tip, -2);
				eprotwrite ( "not made because of dependency errors of: \n"); 
				eprotwrite ( "        %s (remake) \n", dip->target->name );
				eprotwrite ( EPROT_SEPARATOR ); 
				return ( -2 );
		}

	    DBG7 (( "makrel: dependency date value %ld \n",
							dip->target->date ))

        /* CTS 1105891 -> */
		for ( dip_nobind=tip->nobinds; dip_nobind !=NULL; dip_nobind=dip_nobind->next )
		{
			if ( dip_nobind->target == dip->target )
				break;
		}
		if ( dip_nobind != NULL )
			continue;
		/* <- CTS 1105891 */
	    
		/* PTS 1108170 */
		if ( (dip->target->type == TT_MAC ) )
		{
			DBG0 (( "Warning: mac description %s called in %s without 'nobind'\n",
								dip->target->name, tip->name ))
			continue;
		}

	    if ( ! end_of_options && (dip->target->type != TT_OPTION) )
	    {
		/*
		 *  Found first non-option argument.
		 *  Output the target file first.
		 *  Options following non-option arguments are position
		 *  dependent options (e.g. library options to the linker).
		 */
		end_of_options = TRUE ;
		rc |= cmdarg( 1,  name ); /* create reloc in tmp dir */
	    }
		if ( dip->target->stat.deptype == DT_OBJDEMDEP || dip->target->stat.deptype == DT_SRCDEMDEP ) continue ;
#if ! unix
	    if ( dip->target->type == TT_DLL )
			rc |= cmdarg( 1,  dip->target->afile );
	    else
#endif
			if ( oip = dip->target->objects )
				for ( ; oip ; oip = oip->next ) 
					rc |= cmdarg_obj( 1, tip->version, oip->file ); /* CTS 1105495 */
			else
				if ( ! dip->target->stat.noobject ) 
					rc |= cmdarg_obj( 1, tip->version, dip->target->file.file ); /* CTS 1105495 */
	    if ( rc ) return ( rc );
	}
	if ( rc ) return ( rc );

	/* echo target and options */

	outstamp ( "%-9s %-5s", tip->name , versname(tip->version) );
	if ( tip->stat.shrglob == 1 ) outstamp_add ( " %s", "shrglob" );
	/* if ( tip->stat.debug   || opt_debug   ) outstamp_add ( " %s", "debug" ); CTS 1102466 */
	if (opt_optdebug)  /* CTS 1110663 */
			outstamp_add ( " %s", "debug (optimized)" ); 
	else
		if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) ) 
			outstamp_add ( " %s", "debug" );
	if ( tip->stat.profile || opt_profile ) outstamp_add ( " %s", "profile" );
	outstamp_add ( "\n" );

	/* change to proper directory */

	rc = set_work_dir ( tip->version );
	if ( rc < 0 )
	{
		DBG8 (( "makrel: setwrkdir error %d \n", rc ))
		return ( - 3 );
	}
	DBG8 (( "makrel: set work dir '%s' \n", tmp_dir ))

	/*
	 *  If the relocatable contains shared globals,
	 *  create a file with the names of all global size files
	 *  used by the relocatable.
	 *  This is probably not necessary, because linkshr does not
	 *  use this file. It may however be used for later extensions.
	 */
	if ( (tip->stat.shrglob == 1) && (tip->type == TT_SHR) && ! opt_noexec )
	{
	    FILE		* fp ;

	    (void) unlink ( "gsize" ); /* remove if already existent */
	    fp = fopen ( "gsize" , "w" );
	    if ( ! fp )
			return (ewrite_all (tip, 1, -2, "ERROR: cannot create 'gsize' \n" ));
		
	    rc = list_shrglobs ( tip , fp );
	    (void) fclose ( fp );
	    if ( rc ) return ( rc );
	}

	/* get local target file name */

	tip->date = 0l ;
	if ( ! tip->fixed_file )
	{
        tip->file.file = (char *) repalloc( tip->file.file, strlen( trgfile ) + 1 );
	    (void) strcpy ( tip->file.file , trgfile );
	}
	tip->status = PATH_OWN ;

	if ( opt_uncond && (stat ( tip->file.file , &statbuf ) != -1) )
	{
	    rc = unlink_file ( tip->file.file );
		if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while deleting %s \n", tip->file.file));
	}

	/* execute command, built above */

	rc = cmdexe ( );
	/* CTS 1112006 */
	flush_outputlist (tip, rc, 1);
	if (rc)
		return ( rc );
	

	tip->date = time ( (time_t *) 0 );
	tip->date |= 1 ; /* force an odd value for vmake generated files */
	rc = touch_file ( tip, name , tip->date , "" , TRUE );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while touching %s\n",  name));

	/* move reloc */

	rc = move_file ( name , tip->file.file );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while moving %s to %s \n", name , tip->file.file  ));

	/* set link to correctly named target */

	if ( tip->fixed_file )
	{
		rc = link_file ( tip->file.file , trgfile );
		if (rc) 
			ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , trgfile);
		
		fip = tip->file.next;
        while( fip )
        {
			rc1 = link_file ( tip->file.file , fip->file );
			if (rc1)
			{
				rc |= rc1;
				ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , fip->file);
			}
            fip = fip->next;
        }
		for (fip = tip->variantfile; fip; fip=fip->next)
		{
			rc1 =  link_file ( tip->file.file, fip->file  );
			if (rc1)
			{
				rc |= rc1;
				ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , fip->file);
			}
		}
		if ( rc ) return ( rc );
	}

	if ( vopt_dates )
	{
	    rc = make_libpgm_dates_file ( tip );
	    if ( rc ) return ( rc );
	}

	rc = 2 ; /* really made */

    FREE_AND_SET_TO_NULL(trgfile);

	DBG4 (( "makrel: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/

static  int     make_dll 
(
    struct trg_info         * tip 
)
{
	int                     remake=0 , rc=0, rc1=0 ;
	int	                    ec ;
	int	                    end_of_options ;
	int                     astatus ;
	time_t                  adate = 0 ; /* PTS 1104836 */
	time_t                  date_dat = 0 ; /* PTS 1104836 */
	struct dep_info         * dip ;
	struct dep_info         * dip_nobind ;  /* CTS 1105891 */
	struct dsc_info         * sip ;
	struct mem_info         * mip ;
	struct obj_info         * oip ;
    struct file_info        * fip ;
	struct opt_info         * optp;
	struct stat		        statbuf ;
	char                    trgfile [ FILENAMESIZE ] ;
	char                    atrgfile [ FILENAMESIZE ] ;
	char                    name [ FILENAMESIZE ] ;
	char                    aname [ FILENAMESIZE ] ;
	char                    file [ FILENAMESIZE ] ;
	char                    targetname [ FILENAMESIZE ] ;
	char                    dfile [ NAMESIZE + 5 ] ;
	char                    globaldir [NAMESIZE ];
	char                    *filep = NULL; 
	char                    *variantatrgfile=NULL;
	char                    *varianttrgfile = NULL;


	valid_bit_arr		vbits ;

    DBG2 (( "makdll: called \n" ))

	VALID_BIT_FILL ( vbits );
	FILL ( globaldir , 0 , sizeof(globaldir)-1 );

	if ( opt_sizeof || opt_lint || opt_retrieve || tip->stat.level_o < 2 ||
								opt_nolink )
	{
		DBG8 (( "makdll: no objects \n" ))
		return ( 1 );
	}

	/* CTS 1111841 -> */
	if (tip->layer)
		mprotwrite ( "%s desc/%s/%s,%d\n", ascdate(tip->date) , tip->layer , tip->name, 
			(tip->stat.level_s - 1) & PATH_LEVEL );
	else
		mprotwrite ( "%s desc/%s,%d\n", ascdate(tip->date) , tip->name, 
			(tip->stat.level_s - 1) & PATH_LEVEL );
	/* <- CTS 1111841 */


	/* vmake creates a DLL with version suffix */
    remove_ext( tip->name, targetname );
	cut_global_variants(targetname);
	(void) sprintf ( name , "%s.%s.%c", targetname , env_DLL_TYPE ,
							tip->version );
	(void) sprintf ( aname , "%s.%s.%c", targetname , env_ARC_TYPE ,
							tip->version );

	/* find target */
	if ( tip->fixed_file )
	{
		
		/* tip->file.file contains the target file name. Save it. */
		FILL ( trgfile , 0 , sizeof(trgfile)-1 );
		(void) strncpy ( trgfile , tip->file.file , sizeof(trgfile)-1 );

		if (tip->variantfile)
		{
			/* set initial file to file in variant directory with version */
			tip->file.file = (char *) repalloc( tip->file.file, strlen( tip->variantfile->file ) + 3 );
			(void) sprintf ( tip->file.file , "%s.%c", tip->variantfile->file, tip->version );
		}
		else
		{
			/* vmake creates a fixed target with version suffix */
			tip->file.file = (char *) repalloc( tip->file.file, strlen( trgfile ) + 3 );
			(void) sprintf ( tip->file.file , "%s.%c", trgfile , tip->version );
		}

		tip->status = PATH_OWN ;
		rc = stat ( tip->file.file , &statbuf );
		if ( rc >= 0 )
		{
			date_dat = statbuf.st_mtime ;
			rc = 1 ; /* means: file found */
		}
		if ( (rc < 0) &&
		 ((geterrno() == ENOENT) || (geterrno() == ENOTDIR)) )
		{
			rc = 0 ; /* means: file not found */
			date_dat=0;
		}
	}
	else
	{
	    /* the target file is always in the DBROOT tree */
		/* CTS 1103482 */
		
#if unix
	    (void) sprintf ( trgfile , "%s/%s.%s" , env_INSTLIB
#else
	    (void) sprintf ( trgfile , "%s/pgm/%s.%s" , env_INSTROOT
#endif
			 ,targetname , env_DLL_TYPE );
#if unix
	    (void) sprintf ( file  , "%s/" , env_INSTLIB );
#else
	    (void) sprintf ( file  , "%s/pgm/" , env_INSTROOT );
#endif
		
		if (tip->globalvariants)
		{
			if (create_globalvariant_filename (file, &filep, tip->globalvariants) < 0)
				return -3;
			if (create_globalvariant_filename (trgfile, &varianttrgfile, tip->globalvariants) < 0)
				return -3;
		}
		else
			filep = file;
		
		rc = get_file ( name , filep , EMPTY , EMPTY ,
					FK_LIB | FK_LEVEL , vbits ,
					&tip->file.file , & date_dat , & tip->status, 0 );
	}
    remake = ( date_dat < tip->date );
	tip->date = date_dat;
	if ( rc < 0 )
		return (ewrite_all (tip, 1, rc, "ERROR: file read error for '%s' (%d)\n" ,name, rc));

	DBG1 (( "%s %s \n", ascdate(tip->date) , tip->file.file ))
	/*
	 *  "tip->file.file" now contains the path to an existing file (incl. .f).
	 *  "trgfile" contains the path for the new file.
	 */

	/*
	 *  'linkdll' creates a dynamic link library and a static library.
	 */
#if unix
	if ( ! tip->stat.nostlib )
	{
#endif
		if ( tip->fixed_file )
		{
			/* replace the DLL's extension by the archive's one */
			remove_ext( trgfile, atrgfile ); 
			/* PTS 1106573 -> */
			(void) strcat ( atrgfile , "." ); 
			(void) strcat ( atrgfile , env_ARC_TYPE ); 
			/* <- PTS 1106573 */

			if (tip->variantfile)
			{
				if ( create_globalvariant_filename (atrgfile, &variantatrgfile, tip->globalvariants) < 0 )
					return -3;
				/* set initial file to file in variant directory with version */
				tip->afile = (char *) repalloc( tip->afile, strlen( variantatrgfile ) + 3 );
				(void) sprintf ( tip->afile , "%s.%c", variantatrgfile , tip->version );
			}
			else
			{
				/* vmake creates a fixed target with version suffix */
				tip->afile = (char *) repalloc( tip->afile, strlen( atrgfile ) + 3 );
				(void) sprintf ( tip->afile , "%s.%c", atrgfile , tip->version );
			}

			astatus = PATH_OWN ;
			rc = stat ( tip->afile , &statbuf );
			if ( rc >= 0 )
			{
				adate =statbuf.st_mtime ;
				rc = 1 ; /* means: file found */
			}
			if ( (rc < 0) &&
			 ((geterrno() == ENOENT) || (geterrno() == ENOTDIR)) )
			rc = 0 ; /* means: file not found */
		}
		else
		{
			/* the target file is always in the DBROOT tree */
			/* CTS 1103482 */
			(void) sprintf ( atrgfile , "%s/%s.%s" ,
						env_INSTLIB , targetname , env_ARC_TYPE );
			(void) sprintf ( file  , "%s/" , env_INSTLIB );

			if (tip->globalvariants)
			{
				if (create_globalvariant_filename (file, &filep, tip->globalvariants) < 0)
					return -3;
			}
			else
				filep = file;
		
			rc = get_file ( aname , filep , EMPTY , EMPTY ,
					FK_LIB | FK_LEVEL , vbits ,
					&tip->afile , &adate , &astatus, 0 );
			if ( rc < 0 )
				return (ewrite_all (tip, 1, rc, "ERROR: file read error for '%s' (%d)\n" ,aname, rc));

			rc = create_globalvariant_filename (atrgfile, &variantatrgfile, tip->globalvariants);
			if ( rc < 0 )
				return (ewrite_all (tip, 1, -3, "ERROR: can't create global variant file name\n"));
		}
		if ( rc < 0 )
			return (ewrite_all (tip, 1, rc, "ERROR: file read error for '%s' (%d)\n" ,aname, rc));

		DBG1 (( "%s %s \n", ascdate(adate) , tip->afile ))
		/*
		 *  "tip->afile" now contains the path to an existing file (incl. .f).
		 *  "atrgfile" contains the path for the new file.
		 */
#if unix
	}

	else
		DBG1 (( "static lib ignored because of option %s\n", TRGDSC_NOSTLIBOPTION ))
#endif

	 /* &gar CTS 1000237 -> */
	if ( opt_distrib )
	{
		if ( tip->stat.distribute )
		{
			if (! tip->file.nodistrib)
			{
				rc = insert_dist(trgfile);
				if (rc<0) return rc;
				if ( ! tip->stat.nostlib)
				{
					rc = insert_dist(atrgfile);		
					if (rc <0) return rc;
				}
			}
			fip = tip->file.next;
			while( fip )
			{
				if (! fip->nodistrib)
				{
					rc = insert_dist(fip->file);
					if (rc <0) return rc;
				}
				fip = fip->next;
			}
		}
		return 1;
	}
	/* <- &gar CTS 1000237 */
	
	/* build command, while checking dates */

	if ( tip->dependencies == NULL )
	{
	    DBG0 (( "WARNING: link list empty '%s' \n", tip->name ))
	    return ( 0 );
	}

	if ( vopt_dates )
	{
	    rc = read_libpgm_dates_file ( tip , &date_dat );
	    if ( rc ) return ( rc );
	}
	
	/* The dates of the dynamic and static libs must match. */
	if ( ((adate != tip->date) && ! (tip->stat.nostlib)) || date_dat != tip->date )
		remake = 1;


	for ( dip = tip->dependencies ; dip != NULL ; dip = dip->next )
	{
	    ec = -1 ;
	    for ( mip = tip->members ; mip != NULL ; mip = mip->next )
	    {
			ec = strcmp ( mip->name , dip->target->name );
			if ( ec >= 0 ) break ;
	    }
	    /*
	     *  If member is not in target or
	     *  object is newer than target or
	     *  object is newer than member or
	     *  object is older than member or
	     *  object is nearer than library then
	     *  remake is necessary
	     */
	    if (    ec
	         || (dip->target->date > tip->date + TOL)
		 || (dip->target->date > mip->date + TOL)
		 || (dip->target->date < mip->date - TOL)
		 || ((dip->target->status & PATH_LEVEL) <
				(tip->status & PATH_LEVEL))
		 || (dip->target->made > 1) /* IST 1997-04-14 */
	       )
	    {
			DBG1 (( "%s !%s! \n", mip?ascdate(mip->date):ascdate(0) ,
								dip->target->name ))
			remake ++ ;
	    }
    }

	if ( ! tip->stat.uncond && ! remake )
	{
	    DBG2 (( "up to date        %s \n", tip->name ))

	    /* vmake links the created target to the real target file */
	    rc = link_file ( tip->file.file , trgfile );
	
		if (rc) 
			ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , trgfile);
		
		fip = tip->file.next;
        while( fip )
        {
			rc1 = link_file ( tip->file.file , fip->file );
			if (rc1)
			{
				rc |= rc1;
				ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , fip->file);
			}
            fip = fip->next;
        }
		for (fip = tip->variantfile; fip; fip=fip->next)
		{
			rc1 =  link_file ( tip->file.file, fip->file  );
			if (rc1)
			{
				rc |= rc1;
				ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , fip->file);
			}
		}
		if ( rc ) return ( rc );

#if unix
		if ( ! tip->stat.nostlib )
		{
#endif
			rc = link_file ( tip->afile , atrgfile );
			if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while linking %s to %s\n", tip->afile , atrgfile));		            
			if ( variantatrgfile )
			{
				rc = link_file ( tip->afile , variantatrgfile );
				if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while linking %s to %s\n", tip->afile , variantatrgfile));		            
			}
#if unix
		}
#endif
		if ( varianttrgfile )
		{
			rc = link_file ( tip->file.file , varianttrgfile );
			if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while linking %s to %s\n", tip->file.file , varianttrgfile));		            
		}
		return ( 1 );
	}
	else
    for ( dip = tip->remakes ; dip ; dip = dip->next )
        if ( dip->target->made != 2 )
		{
            DBG4(("make_dll: remake %s \n", dip->target->name))
			dip->target->made = 0;
			dip->target->stat.uncond = 1;
			rc = make ( dip->target, tip, MK_FULL );
			if ( rc < 0 ) 
			{
				DBG0 (( "'%s' not made because of dependency errors (remake)\n",
							tip->name ))
				ewrite_targetheader (tip, -2);
				eprotwrite ( "not made because of dependency errors of: \n"); 
				eprotwrite ( "        %s (remake) \n", dip->target->name );
				eprotwrite ( EPROT_SEPARATOR ); 
				return ( rc );
			}
		}

    if ( tip->stat.uncond )
	    DBG1 (( "unconditional     %s \n", tip->name ))


    rc = cmdpgm ( env_TOOLSHELL );
	if ( env_TOOLOPT ) rc |= cmdarg( 1, env_TOOLOPT);
	rc |= cmdscript ( CMD_LINKDLL );
	if ( opt_keep & 2 ) rc |= cmdarg( 1,  "-k" );
	if ( tip->stat.shrglob == 1 ) rc |= cmdarg( 1,  "-Y" );
#if unix 
	if ( tip->stat.nostlib ) rc |= cmdarg( 1,  "-nostaticlib" );
#endif
	/* if ( tip->stat.debug        || opt_debug   ) rc |= cmdarg( 1,  "-g" ); CTS 1102466 */
	if (opt_optdebug)  /* CTS 1110663 */ 
		rc |= cmdarg( 1,  "-optdebug", DSC_DELIMITER );
	else
		if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) ) 
			rc |= cmdarg( 1,  "-g" );
	if ( tip->stat.profile == 1 || opt_profile ) rc |= cmdarg( 1,  "-p" );
	if ( rc ) return ( rc );

	for ( sip = tip->descriptions ; sip != NULL ; sip = sip->next )
	{
	    /* descriptors beginning alnum are concom options */
	    if ( isalnum(sip->descriptor[0]) ) continue ;
	    rc = cmdarg( 1,  "%s%s", sip->descriptor , sip->value );
	    if ( rc ) return ( rc );
		/* sip->value starts with '=' */
	}

	/*
	 *  Put all leading options from the dependencies list
	 *  in front of the target file name.
	 */
	for ( dip = tip->options ; dip != NULL ; dip = dip->next )
	{
		rc |= cmdarg( 1,  dip->target->file.file );
	}

	for ( optp = tip->linkoptions ; optp != NULL ; optp = optp->next )
	{
		rc |= cmdarg( 1,  optp->option );
	}

	end_of_options = FALSE ;
	for ( dip = tip->dependencies ; dip != NULL ; dip = dip->next )
	{
	    DBG8 (( "makdll: dependency found '%s' \n", dip->target->name ))
	    if ( dip->target->made == INIT_STATE )
	    {
			DBG8 (( "WARNING: target dependent from itself '%s' \n",
								dip->target->name ))
			continue ;
	    }
		
		if ( dip->target->made < 0 )
		{
				DBG0 (( "'%s' not made because of dependency errors\n",
							tip->name ))
				ewrite_targetheader (tip, -2);
				eprotwrite ( "not made because of dependency errors of: \n"); 
				eprotwrite ( "        %s (remake) \n", dip->target->name );
				eprotwrite ( EPROT_SEPARATOR ); 
				return ( -2 );
		}
	    DBG7 (( "makdll: dependency date value %ld \n",
							dip->target->date ))

		/* CTS 1105891 -> */
		for ( dip_nobind=tip->nobinds; dip_nobind !=NULL; dip_nobind=dip_nobind->next )
		{
			if ( dip_nobind->target == dip->target )
				break;
		}
		if ( dip_nobind != NULL )
			continue;
		/* <- CTS 1105891 */

	    rc = 0 ;
	    if ( ! end_of_options && (dip->target->type != TT_OPTION) )
	    {
		/*
		 *  Found first non-option argument.
		 *  Output the target file first.
		 *  Options following non-option arguments are position
		 *  dependent options (e.g. library options to the linker).
		 */
			end_of_options = TRUE ;
			rc |= cmdarg( 1,  name ); /* create target in tmp dir */
#if unix
			if ( ! tip->stat.nostlib )
#endif
				rc |= cmdarg( 1,  aname ); /* create target in tmp dir */
	    }

		/* PTS 1108170 */
		if ( (dip->target->type == TT_MAC ) )
		{
			DBG0 (( "Warning: mac description %s called in %s without 'nobind'\n",
								dip->target->name, tip->name ))
			continue;
		}
		
#if ! unix
		if ( dip->target->type == TT_DLL )
			rc |= cmdarg( 1,  dip->target->afile );
		else
#endif
			if ( dip->target->stat.definition )
			{
				strcpy ( dfile, "-def:" );
				strcat ( dfile, dip->target->name );
				rc |= cmdarg( 1,  dfile );
			}
			else
				if ( oip = dip->target->objects )
					for ( ; oip ; oip = oip->next )
						rc |= cmdarg_obj( 1, tip->version, oip->file ); /* CTS 1105495 */
				else
					if ( ! dip->target->stat.noobject )
						rc |= cmdarg_obj( 1, tip->version, dip->target->file.file ); /* CTS 1105495 */
	

		if ( rc ) return ( rc );
	}
	
	/* echo target and options */

	outstamp ( "%-9s %-5s", tip->name , versname(tip->version) );
	if ( tip->stat.shrglob == 1 ) outstamp_add ( " %s", "shrglob" );
	/* if ( tip->stat.debug   || opt_debug   ) outstamp_add ( " %s", "debug" ); CTS 1102466 */
	if (opt_optdebug)  /* CTS 1110663 */
			outstamp_add ( " %s", "debug (optimized)" ); 
	else
		if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) ) 
			outstamp_add ( " %s", "debug" );
	if ( tip->stat.profile || opt_profile ) outstamp_add ( " %s", "profile" );
	outstamp_add ( "\n" );

	/* change to proper directory */

	rc = set_work_dir ( tip->version );
	if ( rc < 0 )
	{
	    DBG8 (( "makdll: setwrkdir error %d \n", rc ))
	    return ( - 3 );
	}
	DBG8 (( "makdll: set work dir '%s' \n", tmp_dir ))

	/* get local target file name */

	tip->date = 0l ;
	tip->status = PATH_OWN ;

	if (tip->globalvariants)
	{
		if ( create_globalvariant_filename (trgfile, &filep, tip->globalvariants) < 0 )
			return -3;
	}
	else
		filep = trgfile;

	
	tip->file.file = (char *) repalloc( tip->file.file, strlen( filep ) + 3 );
	(void) sprintf ( tip->file.file , "%s.%c" , filep , tip->version );
	


	if ( opt_uncond && (stat ( tip->file.file , &statbuf ) != -1) )
	{
	    rc = unlink_file ( tip->file.file );
		if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while deleting %s \n", tip->file.file));
	}

	/* all demands will be copied to work directory */
	rc = copy_demands ( tip );
	if ( rc ) return ( -2 );

	/* execute command, built above */
	rc = cmdexe ( );
	/* CTS 1112006 */
	flush_outputlist (tip, rc, 1);
	if (rc)
		return ( rc );

	/* remove copied demand objects */
	rc = remove_demands ( tip );
	if ( rc ) return rc ;

	tip->date = time ( (time_t *) 0 );
	tip->date |= 1 ; /* force an odd value for vmake generated files */
	rc = touch_file ( tip, name , tip->date , "" , TRUE );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while touching %s\n",  name));
#if unix
	if ( ! tip->stat.nostlib )
#endif
		rc = touch_file ( tip, aname , tip->date , "" , TRUE );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while touching %s\n",  aname));

	/* move DLL and archive */

	rc = move_file ( name , tip->file.file );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while moving %s to %s \n", name , tip->file.file  ));

#if unix
	if ( ! tip->stat.nostlib )
#endif
		rc = move_file ( aname , tip->afile );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while moving %s to %s \n", aname , tip->afile  ));

	
	/* set link to correctly named target */

	rc = link_file ( tip->file.file , trgfile );
	if (rc) 
		ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , trgfile);
	
	fip = tip->file.next;
    while( fip )
    {
		rc1 = link_file ( tip->file.file , fip->file );
		if (rc1)
		{
			rc |= rc1;
			ewrite_all (tip, 1,  -1, "ERROR: error while linking %s to %s\n", tip->file.file , fip->file);
		}
        fip = fip->next;
    }
	for (fip = tip->variantfile; fip; fip=fip->next)
	{
		rc1 =  link_file ( tip->file.file, fip->file  );
		if (rc1)
		{
			rc |= rc1;
			ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , fip->file);
		}
	}
	if ( rc ) return ( rc );

#if unix
	if ( ! tip->stat.nostlib )
	{
#endif
		rc = link_file ( tip->afile , atrgfile );
		if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while linking %s to %s\n", tip->afile , atrgfile));		            
		if ( variantatrgfile )
		{
			rc = link_file ( tip->afile , variantatrgfile );
			if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while linking %s to %s\n", tip->afile , variantatrgfile));		            
		}
#if unix
	}
#endif
	if ( varianttrgfile )
	{
		rc = link_file ( tip->file.file , varianttrgfile );
		if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while linking %s to %s\n", tip->file.file , varianttrgfile));		            
	}

	if ( vopt_dates )
	{
	    rc = make_libpgm_dates_file ( tip );
	    if ( rc ) return ( rc );
	}

	rc = 2 ; /* really made */

	DBG4 (( "makdll: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/

static  int     make_program 
(
    struct trg_info         * tip 
)
{
	int                     remake=0 , rc=0, rc1=0 ;
	int			ec ;
	time_t			date_dat ;
	struct dep_info         * dip ;
	struct dep_info         * dip_nobind ;  /* CTS 1105891 */
	struct dsc_info         * sip ;
	struct mem_info         * mip ;
	struct obj_info			* oip ;
	struct opt_info         * optp;
    struct file_info        * fip ;
	struct stat		statbuf ;
	char                    * trgfile = NULL ;
	char                    * file    = NULL;
	char                    name [ FILENAMESIZE ] ;
	char                    targetname [ FILENAMESIZE ] ;
	char                    * filep = NULL;
	char					* varianttrgfile = NULL;
	valid_bit_arr		vbits ;

	DBG2 (( "makpgm: called \n" ))

    VALID_BIT_FILL ( vbits );

	if ( opt_sizeof || opt_lint || opt_retrieve || tip->stat.level_o < 2 ||
								opt_nolink )
	{
		DBG8 (( "makpgm: no objects \n" ))
		return ( 1 );
	}

	/* CTS 1111841 -> */
	if (tip->layer)
		mprotwrite ( "%s desc/%s/%s,%d\n", ascdate(tip->date) , tip->layer , tip->name, 
			(tip->stat.level_s - 1) & PATH_LEVEL );
	else
		mprotwrite ( "%s desc/%s,%d\n", ascdate(tip->date) , tip->name, 
			(tip->stat.level_s - 1) & PATH_LEVEL );
	/* <- CTS 1111841 */


	/* vmake creates a program with version suffix */
    remove_ext( tip->name, targetname );
	cut_global_variants(targetname);
#if unix
	(void) sprintf ( name , "%s.%c", targetname , tip->version );
#else
	(void) sprintf ( name , "%s.exe.%c", targetname , tip->version );
#endif
	DBG5 (( "makpgm: name '%s' \n", name ))

	/* find target */
	if ( tip->fixed_file )
	{
	    /* tip->file.file contains the target file name. Save it. */
        trgfile = strdup( tip->file.file );
        if ( ! trgfile )
			return (ewrite_all (tip, 0, -3, "ERROR: insufficient dynamic memory \n" ));
		
		if (tip->globalvariants)
		{
			if (create_globalvariant_filename (trgfile, &filep, tip->globalvariants) < 0)
				return -3;
		}
		else
			filep = trgfile;


	    /* vmake creates a fixed target with version suffix */
		tip->file.file = (char *) repalloc( tip->file.file, strlen( filep ) + 3 );
	    (void) sprintf ( tip->file.file , "%s.%c", filep , tip->version );

	    DBG5 (( "makpgm: trgfile   '%s' \n", trgfile ))
	    DBG5 (( "makpgm: tip->file.file '%s' \n", tip->file.file ))

	    tip->status = PATH_OWN ;
	    rc = stat ( tip->file.file , &statbuf );
	    if ( rc >= 0 )
	    {
		    date_dat = statbuf.st_mtime ;
		    rc = 1 ; /* means: file found */
	    }
	    if ( (rc < 0) && ((geterrno() == ENOENT) || (geterrno() == ENOTDIR)) )
		{
		    rc = 0 ; /* means: file not found */
			date_dat = 0;
		}
	}
	else
	{
	    /* the target file is always in the DBROOT tree */
		trgfile = (char *) repalloc( trgfile, strlen( env_INSTROOT ) 
                + strlen( "/pgm/" ) + strlen( targetname ) + 1 );
	    (void) sprintf ( trgfile , "%s/pgm/%s" , env_INSTROOT , targetname );
#if ! unix
		trgfile = (char *) repalloc( trgfile, strlen( trgfile ) + 4 + 1 );
	    (void) strcat ( trgfile , ".exe" );
#endif
		file = (char *) palloc( strlen( env_INSTROOT ) 
                + strlen( "/pgm/" ) + 1 );
	    (void) sprintf ( file  , "%s/pgm/" , env_INSTROOT );

		if (tip->globalvariants)
		{
			if (create_globalvariant_filename (file, &filep, tip->globalvariants) < 0)
				return -3;
			if (create_globalvariant_filename (trgfile, &varianttrgfile, tip->globalvariants) < 0)
				return -3;
		}
		else
			filep = file;

		rc = get_file ( name , filep , EMPTY , EMPTY ,
		    FK_LIB | FK_LEVEL , vbits ,
		    &tip->file.file , & date_dat , & tip->status, 0 );
	}
	if ( rc < 0 )
	{
	    DBG4 (( "get file error, returning %d \n", rc ))
	    return ( rc );
	}
	remake = ( date_dat < tip->date );
	tip->date = date_dat;
	DBG1 (( "%s %s \n", ascdate(tip->date) , tip->file.file ))


	
	/*
	 *  "tip->file.file" now contains the path to an existing file (incl. .f).
	 *  "trgfile" contains the path for the new file.
	 */

	 /* &gar CTS 1000237 -> */
	if ( opt_distrib )
	{
		if ( tip->stat.distribute )
		{
			if (! tip->file.nodistrib)
			{
				rc = insert_dist(trgfile);
				if (rc <0) return rc;
			}
			fip = tip->file.next;
			while( fip )
			{
				if (! fip->nodistrib)
				{
					rc = insert_dist(fip->file);
					if (rc <0) return rc;
				}
				fip = fip->next;
			}
		}
		return 1;
	}
	/* <- &gar CTS 1000237 */

	/* build command, while checking dates */

	if ( tip->dependencies == NULL )
	{
	    DBG0 (( "WARNING: link list empty '%s' \n", tip->name ))
	    return ( 0 );
	}

	if ( vopt_dates )
	{
	    rc = read_libpgm_dates_file ( tip , &date_dat );
	    if ( rc ) return ( rc );
	}
	if (date_dat != tip->date )
		remake = 1;
	
    for ( dip = tip->dependencies ; dip != NULL ; dip = dip->next )
	{
        ec = -1 ;
        for ( mip = tip->members ; mip != NULL ; mip = mip->next )
        {
	        ec = strcmp ( mip->name , dip->target->name );
		    if ( ec >= 0 ) break ;
        }
        /*
         *  If member is not in target or
         *  object is newer than target or
         *  object is newer than member or
         *  object is older than member or
         *  object is nearer than library then
         *  remake is necessary
         */
        if (    ec
             || (dip->target->date > tip->date + TOL)
         || (dip->target->date > mip->date + TOL)
         || (dip->target->date < mip->date - TOL)
         || ((dip->target->status & PATH_LEVEL) <
                (tip->status & PATH_LEVEL))
         || (dip->target->made > 1) /* IST 1997-04-14 */
           )
        {
			DBG1 (( "%s !%s! \n", mip?ascdate(mip->date):ascdate(0) ,
                            dip->target->name ))
			remake ++ ;
        }
    }

    if ( ! tip->stat.uncond && ! remake )
    {
        DBG2 (( "up to date        %s \n", tip->name ))

        /* vmake links the existing target to the real target file */
        rc = link_file ( tip->file.file , trgfile );
		if (rc) 
			ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , trgfile);
		
		fip = tip->file.next;
		while( fip )
		{
			rc1 = link_file ( tip->file.file , fip->file );
			if (rc1)
			{
				rc |= rc1;
				ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , fip->file);
			}
			fip = fip->next;
		}
		for (fip = tip->variantfile; fip; fip=fip->next)
		{
			rc1 =  link_file ( tip->file.file, fip->file  );
			if (rc1)
			{
				rc |= rc1;
				ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , fip->file);
			}
		}
		if ( rc ) return ( rc );

    	if ( varianttrgfile )
		{
			rc = link_file ( tip->file.file , varianttrgfile );
		    if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while linking %s to %s\n", tip->file.file , varianttrgfile));		            
		}

        FREE_AND_SET_TO_NULL(trgfile);
        FREE_AND_SET_TO_NULL(file);
        return ( 1 );
    }
	else
    for ( dip = tip->remakes ; dip ; dip = dip->next )
        if ( dip->target->made != 2 )
		{
            DBG4(("make_prog: remake %s \n", dip->target->name))
			dip->target->made = 0;
			dip->target->stat.uncond = 1;
			make ( dip->target, tip, MK_FULL );
		}

    rc = cmdpgm ( env_TOOLSHELL );
	if ( env_TOOLOPT ) rc |= cmdarg( 1, env_TOOLOPT);
	if ( tip->type == TT_SHM )
	{
		rc |= cmdscript ( CMD_LINKSHM );
	}
	else
	{
		rc |= cmdscript ( CMD_LINKLNK );
	}
	if ( opt_keep & 2 ) rc |= cmdarg( 1,  "-k" );
	if ( tip->stat.shrglob == 1 ) rc |= cmdarg( 1,  "-Y" );
	/* if ( tip->stat.debug        || opt_debug   ) rc |= cmdarg( 1,  "-g" ); CTS 1102466 */
	if (opt_optdebug)  /* CTS 1110663 */ 
		rc |= cmdarg( 1,  "-optdebug", DSC_DELIMITER );
	else
		if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) ) 
			rc |= cmdarg( 1,  "-g" );
	if ( tip->stat.profile == 1 || opt_profile ) rc |= cmdarg( 1,  "-p" );

	for ( sip = tip->descriptions ; sip != NULL ; sip = sip->next )
	{
	    /* descriptors beginning alnum are concom options */
	    if ( isalnum(sip->descriptor[0]) ) continue ;
	    rc |= cmdarg( 1,  "%s%s", sip->descriptor , sip->value );
	    if ( rc ) return ( rc );
	}
	rc |= cmdarg( 1,  "-o" );
	rc |= cmdarg( 1,  name ); /* create program in tmp dir */

	for ( dip = tip->options ; dip != NULL ; dip = dip->next )
	{
		rc |= cmdarg( 1,  dip->target->file.file );
	}
	for ( optp = tip->linkoptions ; optp != NULL ; optp = optp->next )
	{
		rc |= cmdarg( 1,  optp->option );
	}
	for ( dip = tip->dependencies ; dip != NULL ; dip = dip->next )
	{
		DBG8 (( "makpgm: dependency found '%s' \n", dip->target->name ))
	    if ( dip->target->made == INIT_STATE )
	    {
			DBG8 (( "WARNING: target dependent from itself '%s' \n",
								dip->target->name ))
			continue ;
	    }
	    if ( dip->target->made < 0 )
	    {
			DBG0 (( "'%s' not made because of dependency errors \n",
								tip->name ))
			DBG8 (( "makpgm: dependency not made '%s' \n",
								dip->target->name ))
			return ( - 2 );
	    }
	    DBG7 (( "makpgm: dependency date value %ld \n",
							dip->target->date ))

        /* CTS 1105891 -> */
		for ( dip_nobind=tip->nobinds; dip_nobind !=NULL; dip_nobind=dip_nobind->next )
		{
			if ( dip_nobind->target == dip->target )
				break;
		}
		if ( dip_nobind != NULL )
			continue;
		/* <- CTS 1105891 */
	    
		if ( dip->target->stat.deptype == DT_OBJDEMDEP || dip->target->stat.deptype == DT_SRCDEMDEP ) continue;

		/* PTS 1108170 */
		if ( (dip->target->type == TT_MAC ) )
		{
			DBG0 (( "Warning: mac description %s called in %s without 'nobind'\n",
								dip->target->name, tip->name ))
			continue;
		}

#if ! unix
	    if ( dip->target->type == TT_DLL )
			rc |= cmdarg( 1,  dip->target->afile );
	    else
#endif
			if ( oip = dip->target->objects )
				for ( ; oip ; oip = oip->next )
					rc |= cmdarg_obj( 1, tip->version, oip->file ); /* CTS 1105495 */
			else
				if ( ! dip->target->stat.noobject )
					rc |= cmdarg_obj( 1, tip->version, dip->target->file.file ); /* CTS 1105495 */
	    if ( rc ) return ( rc );
	}
	if ( rc ) return ( rc );

	if ( tip->stat.uncond )
	    DBG1 (( "unconditional     %s \n", tip->name ))

	/* echo target and options */

	outstamp ( "%-9s %-5s", tip->name , versname(tip->version) );
	if ( tip->stat.shrglob == 1 ) outstamp_add ( " %s", "shrglob" );
	/* if ( tip->stat.debug   || opt_debug   ) outstamp_add ( " %s", "debug" ); CTS 1102466 */
	if (opt_optdebug)  /* CTS 1110663 */
			outstamp_add ( " %s", "debug (optimized)" ); 
	else
		if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) ) 
			outstamp_add ( " %s", "debug" );
	if ( tip->stat.profile || opt_profile ) outstamp_add ( " %s", "profile" );
	outstamp_add ( "\n" );

	/* change to proper directory */

	rc = set_work_dir ( tip->version );
	if ( rc < 0 )
	{
		DBG8 (( "makpgm: setwrkdir error %d \n", rc ))
		return ( - 3 );
	}
	DBG8 (( "makpgm: set work dir '%s' \n", tmp_dir ))

	/*
	 *  If the program contains shared globals,
	 *  create a file with the names of all global size files
	 *  used by the program.
	 */
	if ( (tip->stat.shrglob == 1) && (tip->type == TT_SHM) && ! opt_noexec )
	{
	    FILE		* fp ;

	    (void) unlink ( "gsize" ); /* remove if already existent */
	    fp = fopen ( "gsize" , "w" );
	    if ( ! fp )
			return (ewrite_all (tip, 1, -2, "ERROR: cannot create 'gsize' \n" ));

		rc = list_shrglobs ( tip , fp );
	    (void) fclose ( fp );
	    if ( rc ) return ( rc );
	}

	/* get local target file name */

	tip->date = 0l ;
	tip->status = PATH_OWN ;
	if (tip->globalvariants)
	{
		if ( create_globalvariant_filename (trgfile, &filep, tip->globalvariants) < 0 )
			return -3;
	}
	else
		filep = trgfile;

    tip->file.file = (char *) repalloc( tip->file.file, strlen( filep ) + 3 );
	(void) sprintf ( tip->file.file , "%s.%c" , filep , tip->version );

	if ( opt_uncond && (stat ( tip->file.file , &statbuf ) != -1) )
	{
	    rc = unlink_file ( tip->file.file );
		if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while deleting %s \n", tip->file.file));
	}

	/* all demands will be copied to work directory */
	rc = copy_demands ( tip );
	if ( rc ) return ( -2 );

	/* execute command, built above */

	rc = cmdexe ( );
	/* CTS 1112006 */
	flush_outputlist (tip, rc, 1);
	if (rc)
		return ( rc );

	/* remove copied demand objects */
	rc = remove_demands ( tip );
	if ( rc ) return rc ;

	tip->date = time ( (time_t *) 0 );
	tip->date |= 1 ; /* force an odd value for vmake generated files */
	rc = touch_file ( tip, name , tip->date , "" , TRUE );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while touching %s\n",  name));

	/* move program */

	rc = move_file ( name , tip->file.file );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while moving %s to %s \n", name , tip->file.file  ));

	/* set link to correctly named program */

	rc = link_file ( tip->file.file , trgfile );
	if (rc) 
		ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , trgfile);
	
	fip = tip->file.next;
    while( fip )
    {
		rc1 = link_file ( tip->file.file , fip->file );
		if (rc1)
		{
			rc |= rc1;
			ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , fip->file);
		}
        fip = fip->next;
    }
	for (fip = tip->variantfile; fip; fip=fip->next)
	{
		rc1 =  link_file ( tip->file.file, fip->file  );
		if (rc1)
		{
			rc |= rc1;
			ewrite_all (tip, 1, -1, "ERROR: error while linking %s to %s\n", tip->file.file , fip->file);
		}
	}
	
	if ( rc ) return ( rc );

	if ( varianttrgfile )
	{
		rc = link_file ( tip->file.file , varianttrgfile );
		if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while linking %s to %s\n", tip->file.file , varianttrgfile));		            
	}

	if ( vopt_dates )
	{
	    rc = make_libpgm_dates_file ( tip );
	    if ( rc ) return ( rc );
	}

	rc = 2 ; /* really made */

    FREE_AND_SET_TO_NULL(trgfile);
    FREE_AND_SET_TO_NULL(file);

	DBG4 (( "makpgm: returning %d \n", rc ))
	return ( rc );
}

/*==========================================================================*/

static  int     make_dirlist 
(
    struct trg_info         * tip 
)
{
	int                     errcnt=0 , rc=0 ;
	char					*directory = NULL;
	valid_bit_arr	        vbits ;
	int                     filetype = 0;
	struct file_info        *pfip;
	struct dsc_info         *dip;
	int                     silent = 0;
    	
    VALID_BIT_FILL ( vbits );

	return (1);

	DBG2 (( "maklst: called \n" ))

	filetype = tip->stat.binary ? FT_BIN : FT_TXT;

	/* PTS 1108150 */
	if (opt_distrib)
	{
		DBG3 (( "maklst:  ignored (-F)\n" ))
		return ( 1 );
	}

	for (dip=tip->descriptions; dip; dip=dip->next)
	{
		if (strcmp(dip->descriptor, "silent") == 0)
			silent = 1;
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

		for (pfip = &(tip->file); pfip; pfip=pfip->next)
		{
			if (pfip->file)
			{
				rc = recursive_dircopy ( directory , pfip->file, filetype, silent );
				if ( rc != 0 )
					return (ewrite_all (tip, 0, -1, "ERROR: Copying of '%s' failed\n", tip->name)) ;
			}
		}
	}
	else
	{
		ewrite_all (tip, 0, -1, "ERROR: Can't handle only qualified directory names %s\n", tip->name);
	}
	return (1);
}

/*==========================================================================*/

static  int     make_extra 
(
    struct trg_info         * tip ,
    time_t                  date_dep ,      /* date of newest dependency */
    int                     make_option 
)
{
	int             rc=0 ;
	int			    status_src ;
	time_t			date_src ;
/*	struct dep_info * dip ; */
	struct dsc_info * sip ;
	char			* p ;
	struct stat		statbuf ;
	char			name [NAMESIZE];

	char            dir [ FILENAMESIZE ] ;
	char            * file_src = NULL ;
	char            * file     = NULL;
	char            * file_o   = NULL;
	int             date_file;
	valid_bit_arr	vbits ;
	int             ret=0,i ;

	DBG2 (( "makxtr: called \n" ))
	DBG3 (( "makxtr: making '%s' \n", tip->name ))

    VALID_BIT_FILL ( vbits );

	/* find source */

	status_src = PATH_LEVEL ;
	
	for (i=0; i<1; i++)
	{
		/* if create distriblist then not look for source in herarchy */ 
		if ( ! opt_distrib )
		{
			if ( tip->stat.level_s && (make_option == MK_FULL || tip->stat.uncond) )
			{
				rc = get_file ( tip->name , DIR_SRC , tip->layer , EMPTY ,
					FK_SOURCE | FK_LEVEL , vbits ,
					&file_src , & date_src , & status_src, 0 );
				if ( rc < 0 )
				{
					DBG4 (( "get file error, returning %d \n", rc ))
					ret = rc; break; /* return ret */
				}
				if ( rc == 0 )
					rc = get_file ( tip->name , DIR_SRC , tip->layer , EMPTY , FK_SOURCE | FK_LEVEL , vbits ,
					                &file_src , & date_src , & status_src, 1 );
				if ( rc == 0 )
				{
					if ( tip->stat.level_s > 1 )
					{
						ret = ewrite_all (tip, 0, -2, "ERROR: missing source    '%s' \n" ,tip->name);
						break; /* return ret */

					}
					else
					{
						DBG2 (( "no source         '%s' \n", tip->name ))
						ret = 1; break; /* return ret */
					}
				}
				DBG1 (( "%s %s \n", ascdate(date_src) , file_src ))
				mprotwrite ( "%s src/%s/%s,%d\n", ascdate(date_src) , tip->layer, tip->name, status_src & PATH_LEVEL );
				if ( (status_src & PATH_LEVEL) < tip->minpathlvl )
				{
					tip->minpathlvl = status_src & PATH_LEVEL ;
					DBG5 (( "makxtr: target '%s' minpathlvl %d src \n",
						tip->name , tip->minpathlvl ))
				}

				/* find dates file */

				if ( vopt_dates )
				{
					rc = get_dates_file ( tip,
						FK_OBJECT | (tip->minpathlvl & FK_LEVEL) ,
						date_src , vbits );
					if ( rc < 0 )
					{
						DBG4 (( "get file error, returning %d \n", rc ))
						ret = rc; break;  /* return ret */
					}
				}

				/*
				 *  With VMAKE_OPTION=V get all sources into OWN tree.
				 */
				if ( vopt_source_copy && ! (status_src & PATH_OWN) )
				{
					outstamp ( "%-9s %-5s <%s>\n", EMPTY , EMPTY , file_src );
					(void) new_file ( tip->name , DIR_SRC , tip->layer ,
								EMPTY , &file );
					rc = vcopy_file ( file_src , file , date_src ,
							tip->stat.binary ? FT_BIN : FT_TXT );
					if ( rc ) {	ret = rc; break;}  /* return ret */

					file_src = (char *) repalloc( file_src, strlen( file ) + 1 );
					(void) strcpy ( file_src , file );

					status_src = (status_src & PATH_LEVEL) | PATH_OWN ;
				}
			}

			if ( opt_sizeof || opt_lint || opt_retrieve || tip->stat.level_o < 2 )
			{
				DBG8 (( "makxtr: no objects \n" ))
				ret = 1; break;  /* return ret */
			}
		}

		/* find target */

		FILL ( name , 0 , sizeof(name) );
		(void) strncpy ( name, tip->name , sizeof(name)-1 );
		rc = strlen ( name );
		for ( sip = tip->descriptions ; sip != NULL ; sip = sip->next )
		{
			if ( ! strcmp ( sip->descriptor , "ext" ) )
			{
				p = extension_exists ( name );
				if ( ! p ) p = name + rc ;
				rc = p - name ;
				(void) strncpy ( p , (sip->value[0] == '=') ? sip->value+1 :
							sip->value , sizeof(name)-1 - rc );
				continue ;
			}
		}
		if ( tip->language == '.' )
		{
			/* nodot option */
			p = extension_exists( name );
			if ( p ) * p = '\0' ;
		}



		if ( ( tip->file.file[0] == '/' )
	#ifdef WIN32
			|| ( isalpha(tip->file.file[ 0 ] ) && ( tip->file.file[1] == ':' ) )
	#endif
		   )
		{
			/* directory name is absolute */
			(void) strcpy ( dir , tip->file.file );
			status_src &= PATH_TYPE ; /* search only in local tree */
		}
		else
		{
			/* directory name is relative to $OWN */
			(void) strcpy ( dir , "%s/" );
			(void) strcat( dir , tip->file.file );
		}
		
		if ( dir[ strlen( dir ) - 1 ] != '/' )   /* PTS 1108163 */
			(void) strcat ( dir , "/" );

		tip->status = tip->minpathlvl & PATH_LEVEL ;
		DBG5 (( "makxtr: target '%s' minpathlvl %d srch \n",
			tip->name , tip->status))
		DBG5 (( "makxtr: target '%s' minpathlvl %d srch \n",
			tip->name , tip->status))


		(void) new_file ( name , dir , EMPTY ,
								EMPTY , &tip->file.file );

		rc = stat ( tip->file.file , &statbuf );
		if ( rc >= 0 )
			date_file = statbuf.st_mtime ;  /* file found */
		else
			date_file = 0 ; /* means: file not found */
		

		(void) new_file ( tip->name , DIR_OBJ , EMPTY , tip->layer , &file_o);
			   
		rc = get_file ( tip->name, DIR_OBJ , EMPTY , tip->layer ,
			FK_OBJECT | (tip->status & FK_LEVEL) , vbits ,
			&file , & tip->date , & tip->status, 0 );
		if ( rc < 0 )
		{
			DBG4 (( "get file error, returning %d \n", rc ))
			ret = rc; break;  /* return ret */
		}
		DBG1 (( "%s %s \n", ascdate(tip->date) , file ))
		DBG1 (( "%s %s \n", ascdate(date_file) , tip->file.file ))

		/* &gar CTS 1000237 -> */
		if ( opt_distrib )
		{
			if ( tip->stat.distribute )
				rc = insert_dist(tip->file.file);
			ret = rc < 0 ? rc : 1; break;  /* return ret */
		}
		/* <- &gar CTS 1000237 */

		if ( make_option != MK_FULL && ! tip->stat.uncond )
		{
			DBG8 (( "makxtr: no full make \n" ))
			ret = rc; break;  /* return ret */
		}

		/*
		 *  By request of Lutz Jungnickel at 1993-08-17:
		 *  Target gets date of source.
		 *  The condition for rebuild is unequal dates of source and target.
		 *  Since exactly this date-comparison is done for dates files
		 *  and Lutz's request is meaningless with dates files,
		 *  we can use the date's files date variable to hold
		 *  the target's date.
		 */

		/* compare dates */

		if (    ! tip->stat.uncond
			 && tip->date >= date_src - TOL
			 && tip->date >= date_dep - TOL
			 && VALID_BIT_TST(tip->status&PATH_LEVEL,vbits)
		   )
		{
			/* PTS 1102287 copy also if in local path (not OWN) */
			if ( (tip->status & PATH_REMOTE) || (tip->status & PATH_LOCAL) )
			{
				outstamp ( "%-9s %-5s <%s>\n",
					name , versname(tip->version) , file );
				
				rc = vcopy_file ( file , file_o , tip->date ,
							tip->stat.binary ? FT_BIN : FT_TXT );
				if ( rc ) { ret = rc; break; }  /* return ret */
			}
			else
			{
				if ( tip->date <= date_file)
				{
					DBG2 (( "up to date        %s \n", tip->name ))
					ret = 1; break;
				}
			}
		}
		else
		{
			if ( tip->stat.uncond )
				DBG1 (( "unconditional     %s \n", tip->name ))

			/* make target */

			/* echo target name */

			outstamp ( "%-9s\n", tip->name );

			/* change to proper directory */

			rc = set_work_dir ( tip->version );
			if ( rc < 0 )
			{
				DBG8 (( "makxtr: setwrkdir error %d \n", rc ))
				ret = rc; break; /* return ret */
			}
			DBG8 (( "makxtr: set work dir '%s' \n", tmp_dir ))

			/* get local target file name */

			tip->date = 0l ;
			tip->status = PATH_OWN ;
		
			/*
			 *  Create the target directory, if not existent.
			 *  CTS 1102465 
			 */
			rc = check_dir ( file_o );
			if ( rc )  { ret = rc; break; } /* return ret */

			/* if necessary, copy remote source */

			if ( status_src & PATH_REMOTE )
			{
				rc = vcopy_file ( file_src , tip->name , date_src ,
							tip->stat.binary ? FT_BIN : FT_TXT );
				if ( rc ) { ret = rc; break; } /* return ret */
			}

			/* unpack module frame */

			rc = cmdpgm ( env_TOOLSHELL );
			if ( env_TOOLOPT ) rc |= cmdarg( 1, env_TOOLOPT);
			rc |= cmdscript ( CMD_MFEXTRA );
			if ( opt_keep & 2 ) rc |= cmdarg( 1,  "-k" );
			if (tip->stat.exec) /* CTS 1110662 */
				rc |= cmdarg( 1,  "-exec%c1", DSC_DELIMITER );
			if (opt_optdebug)  /* CTS 1110663 */ 
				rc |= cmdarg( 1,  "-optdebug%c1", DSC_DELIMITER );
			else
				if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) )
						rc |= cmdarg( 1,  "-debug%c1", DSC_DELIMITER );
			if ( rc ) { ret = rc;	break; }  /* return ret */
			for ( sip = tip->descriptions ; sip != NULL ; sip = sip->next )
			{
				if ( ! isalnum(sip->descriptor[0]) )
				{
	    			continue ;
				}
				/* descriptors beginning alnum are concom options */
				if ( ! strcmp ( sip->descriptor , "ext" ) )
				{
    				continue ;
				}
				rc |= cmdarg( 1,  "-%s%s", sip->descriptor , sip->value );
				if ( rc ) {	ret = rc; break;}  /* return ret */
			}
			if ( ret != 0)
				break;  /* return ret */
			if ( status_src & PATH_REMOTE )
				rc |= cmdarg( 1,  tip->name );
			else    rc |= cmdarg( 1,  file_src );
			rc |= cmdarg( 1,  file_o );
			rc = cmdexe ( );
			/* CTS 1112006 */
			flush_outputlist (tip, rc, 1);
			if (rc) { ret = rc; break; } /* return ret */

			if ( status_src & PATH_REMOTE ) (void) unlink_file ( tip->name );

			if ( vopt_dates )
			{
				tip->date = time ( (time_t *) 0 );
				tip->date |= 1 ; /* force an odd value for vmake generated files */
				rc = touch_file ( tip, file_o , tip->date , "" , TRUE );
				if (rc)	{ ret = rc; break;}  /* return ret */
			}
			else
			{
				/*
				 *  By request of Lutz Jungnickel at 1993-08-17:
				 *  Target gets date of source.
				 */
				tip->date = date_src ;
				rc = touch_file ( tip, file_o , date_src , file_src , TRUE );
				if (rc)	{ ret = rc; break; }  /* return ret */
			}
		}
		if ( tip->stat.exec && ! opt_noexec )
		{
			rc = set_exec_permissions (file_o);
			if (rc)
				return (ewrite_all (tip, 1, rc, "ERROR: can't set execute permissions for '%s'\n", file_o));
		}

		tip->status = PATH_OWN ;

		if ( vopt_dates )
		{
			rc = make_dates_file ( tip, date_src );
			if (rc)	{ ret = rc; break;}  /* return ret */
		}
		
		rc = vcopy_file ( file_o , tip->file.file , tip->date ,
					tip->stat.binary ? FT_BIN : FT_TXT );

		if ( rc ) { ret = rc; break; }  /* return ret */
		ret = 2;
	}

    FREE_AND_SET_TO_NULL(file);
	FREE_AND_SET_TO_NULL(file_o);
	FREE_AND_SET_TO_NULL(file_src);
	DBG4 (( "makxtr: returning %d \n", ret ))
	return ( ret );
}


/*==========================================================================*/

static  int     issue_command 
(
    struct trg_info         * tip ,
    struct trg_info         * ctip ,        /* calling target info pointer */
    int                     make_option 
)
{
	int                     made_dep , made , rc=0 ;
	struct dep_info         * dip ;
	char *command;

	DBG2 (( "isscmd: called \n" ))

	command = (tip->type == TT_EXECUTE) ? tip->afile : tip->name;

	/* PTS 1108150 */
	if (opt_distrib)
	{
		DBG3 (( "isscmd: commands ignored (-F)\n" ))
		return ( 1 );
	}

	if ( opt_sizeof || opt_lint || opt_retrieve )
	{
		DBG8 (( "isscmd: no objects \n" ))
		return ( 1 );
	}

	if ( make_option != MK_FULL )
	{
	    DBG8 (( "isscmd: no full make \n" ))
	    return ( 1 );
	}



	if ( ! tip->stat.forcemake && ctip )
	{
	    DBG6 (( "isscmd: checking dependencies of '%s' \n", ctip->name ))
	    made_dep = 0 ;
		for ( dip = ctip->dependencies ; dip != NULL ; dip = dip->next )
	    {
			/* PTS 1103539 */
			if ( dip->target == tip ) break;
			made = dip->target->made ;
			if ( tip->stat.uncond && ( made < 0 ) )
				return (ewrite_all (tip, 0, 1, "command '%s...' not executed\n because of errors in dependencies made of '%s'\n", tip->name, ctip->name ));
		    if ( made > made_dep ) made_dep = made ;
		    DBG6 (( "isscmd: made %d (%s) \n", made , dip->target->name ))
	    }
	    if ( made_dep < 2 )
	    {
		    DBG2 (( "no other dependencies made of '%s', \n", ctip->name ))
		    DBG2 (( "so command not executed '%s...' \n", tip->name ))
		    return ( 1 );
	    }
	}

	/* echo command */

	outstamp ( "!%s\n"  , tip->name );

	/* change to proper directory */

	rc = set_work_dir ( tip->version );
	if ( rc < 0 )
	{
		DBG8 (( "isscmd: setwrkdir error %d \n", rc ))
		return ( - 3 );
	}
	DBG8 (( "isscmd: set work dir '%s' \n", tmp_dir ))

	/* execute command */
	if ( opt_noexec || vopt_source_copy ) return ( 2 );

	rc = cmdpgm ( env_SHELL );
#if unix
	rc |= cmdarg( 1,  "-c" );
	rc |= cmdarg( 1,  "%s", command );
#else
	if ( strcmp(env_SHELL, CMD_SHELL) ) 
            rc |= cmdarg( 1,  "-c" );
	rc |= cmdarg( 0,  "\"%s\"", command );
#endif
	rc = cmdexe ( );
	if ( tip->stat.ignore_err)
	{
		if (rc)
			DBG0 (( "error of command \"%s\" ignored \n", tip->name )) 
		rc=0;
	}
	/* CTS 1112006 */
	flush_outputlist (tip, rc, 0);
	if (rc)
		return ( rc );

	tip->date = time ( (time_t *) 0 );
	rc = 2 ;

	DBG4 (( "isscmd: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/

static  int     search_file 
(
    struct trg_info		* tip 
)
{
	int	                rc=0 ;
	char                * name = NULL;
	char                * path = NULL;
	valid_bit_arr       vbits ;

	DBG2 (( "srchfil: called \n" ))
	DBG3 (( "srchfil: file or option '%s' \n", tip->file.file ))

    VALID_BIT_FILL ( vbits );

	if ( (tip->file.file[0] == '/')
#ifdef WIN32
		|| ( isalpha(tip->file.file[0]) && (tip->file.file[1] == ':') )
#endif
	   )
	{
	    /* absolute path name */

	    rc = get_date_and_size ( tip->file.file , & tip->date , (long*)0 , TRUE );
	    if ( rc ) return ( rc );
	}
	else
	{
	    /* relative path name, search in object directories */

	    name = strdup( tip->file.file );
        if ( ! name )
			return (ewrite_all (tip, 0, -3, "ERROR: insufficient dynamic memory \n" ));

	    rc = get_file ( name , DIR_OBJ , versname(tip->version) , tip->layer ,
		    FK_LIB | FK_LEVEL , vbits ,
		    &tip->file.file , & tip->date , & tip->status, 0 );
	    if ( rc < 0 )
	    {
		    DBG4 (( "get file error, returning %d \n", rc ))
		    return ( rc );
	    }
	    if ( rc == 0 )
			return (ewrite_all (tip, 0, -2, "ERROR: file not found '%s' \n", tip->file.file ));

	    DBG1 (( "%s %s \n", ascdate(tip->date) , tip->file.file ))

	    if ( tip->status & PATH_REMOTE )
	    {
		    outstamp ( "%-9s %-5s <%s>\n", EMPTY , EMPTY , tip->file.file );
		    (void) new_file ( name , DIR_OBJ ,
			    versname(tip->version) , tip->layer , &path );
		    rc = vcopy_file ( tip->file.file , path , tip->date , FT_BIN );
		    if ( rc ) return ( rc );

		    tip->file.file = (char *) repalloc( tip->file.file, strlen( path ) + 1 );
            (void) strcpy ( tip->file.file , path );
		    tip->status = PATH_OWN ;
	    }
	}

    FREE_AND_SET_TO_NULL(path);
    FREE_AND_SET_TO_NULL(name);

	DBG4 (( "srchfil: returning %d \n", rc ))
	return ( 1 );
}


/*==========================================================================*/

static	char	* suffix 
(
    struct trg_info			* tip 
)
{
	register char			language ;
	register struct lang_info	* lip ;

    DBG2 (( "suffix: called \n" ))
	DBG3 (( "suffix: target '%s' \n", tip->name ))

	language = tip->language ;
	if ( (language == LANG_PASCAL_P[0]) && tip->stat.level_c ) language = LANG_PASCAL_T[0] ;
	for ( lip = lng_lists ; lip ; lip = lip->next )
	{
	    if ( lip->language == language ) break ;
	}
	if ( ! lip )
	{
		if ( ! tip->stat.binary && ! tip->stat.ascii && ! tip->stat.text ) /* CTS 1105829 */
			DBG0 (( "ERROR: no information for language '%c' \n", language ))
	    return ( "" );
	}

	return ( lip->suffix );
}

/*==========================================================================*/

static	char	* csuffix 
(
    struct trg_info			* tip 
)
{
	register char			language ;
	register struct lang_info	* lip ;

    DBG2 (( "csuffix: called \n" ))
	DBG3 (( "csuffix: target '%s' \n", tip->name ))

	language = tip->language ;
	if ( (language == LANG_PASCAL_P[0]) && tip->stat.level_c ) language = LANG_PASCAL_T[0] ;
	for ( lip = lng_lists ; lip ; lip = lip->next )
	{
	    if ( lip->language == language ) break ;
	}
	if ( ! lip )
	{	
		if ( ! tip->stat.binary && ! tip->stat.ascii  && ! tip->stat.text) /* CTS 1105829 */
			DBG0 (( "ERROR: no information for language '%c' \n", language ))
	    return ( "" );
	}

	if ( language == LANG_PASCAL_T[0] ) return ( LANG_C );
	return ( lip->suffix );
}

/*==========================================================================*/

static	char	* unpackinc 
(
    struct trg_info			* tip 
)
{
	register char			language ;
	register struct lang_info	* lip ;

	DBG2 (( "unpackinc: called \n" ))
	DBG3 (( "unpackinc: target '%s' \n", tip->name ))

	if (tip->stat.ascii)
		language = LANG_RC[0];
	else
		language = tip->language ;

	if ( (language == LANG_PASCAL_P[ 0 ]) && tip->stat.level_c ) language = LANG_PASCAL_T[ 0 ] ;
	for ( lip = lng_lists ; lip ; lip = lip->next )
	{
	    if ( lip->language == language ) break ;
	}
	if ( ! lip )
	{
		if ( ! tip->stat.binary && ! tip->stat.ascii && ! tip->stat.text ) /* CTS 1105829 */
			DBG0 (( "ERROR: no information for language '%c' \n", language ))
	    return ( "" );
	}

	return ( lip->unpackinc );
}

/*==========================================================================*/

static	char	* unpackexp 
(
    struct trg_info			* tip 
)
{
	register char			language ;
	register struct lang_info	* lip ;

	DBG2 (( "unpackexp: called \n" ))
	DBG3 (( "unpackexp: target '%s' \n", tip->name ))

	language = tip->language ;
	if ( (language == LANG_PASCAL_P[ 0 ]) && tip->stat.level_c ) language = LANG_PASCAL_T[ 0 ] ;
	for ( lip = lng_lists ; lip ; lip = lip->next )
	{
	    if ( lip->language == language ) break ;
	}
	if ( ! lip )
	{
		if ( ! tip->stat.binary && ! tip->stat.ascii && ! tip->stat.text) /* CTS 1105829 */
			DBG0 (( "ERROR: no information for language '%c' \n", language ))
	    return ( "" );
	}

	return ( lip->unpackexp );
}

/*==========================================================================*/

static	char	* unpackmod 
(
    struct trg_info			* tip 
)
{
	register char			language ;
	register struct lang_info	* lip ;

	DBG2 (( "unpackmod: called \n" ))
	DBG3 (( "unpackmod: target '%s' \n", tip->name ))

	if (tip->stat.ascii)
		language = LANG_RC[0];
	else
		language = tip->language ;
	
	if ( (language == LANG_PASCAL_P[ 0 ]) && tip->stat.level_c ) language = LANG_PASCAL_T[ 0 ] ;
	for ( lip = lng_lists ; lip ; lip = lip->next )
	{
	    if ( lip->language == language ) break ;
	}
	if ( ! lip )
	{
		if ( ! (tip->stat.binary || tip->stat.text)) /* CTS 1105829 */
			DBG0 (( "ERROR: no information for language '%c' \n", language ))
	    return ( "" );
	}

	return ( lip->unpackmod );
}

/*==========================================================================*/

static	char	* compiler 
(
    struct trg_info			* tip 
)
{
	register char			language ;
	register struct lang_info	* lip ;

	DBG2 (( "compiler: called \n" ))
	DBG3 (( "compiler: target '%s' \n", tip->name ))

	if (tip->compiler)
		return (tip->compiler);
	language = tip->language ;
	if ( (language == LANG_PASCAL_P[ 0 ]) && tip->stat.level_c ) language = LANG_PASCAL_T[ 0 ] ;
	for ( lip = lng_lists ; lip ; lip = lip->next )
	{
	    if ( lip->language == language ) break ;
	}
	if ( ! lip )
	{
		if ( ! tip->stat.binary && ! tip->stat.ascii && ! tip->stat.text ) /* CTS 1105829 */
			DBG0 (( "ERROR: no information for language '%c' \n", language ))
	    return ( "" );
	}

	return ( lip->compiler );
}

/*==========================================================================*/

static	char	optchar 
(
    struct trg_info			* tip 
)
{
	register char			language ;
	register struct lang_info	* lip ;

	DBG2 (( "optchr: called \n" ))
	DBG3 (( "optchr: target '%s' \n", tip->name ))

	language = tip->language ;
	if ( (language == LANG_PASCAL_P[ 0 ]) && tip->stat.level_c ) language = LANG_PASCAL_T[ 0 ] ;
	for ( lip = lng_lists ; lip ; lip = lip->next )
	{
	    if ( lip->language == language ) break ;
	}
	if ( ! lip )
	{
		if ( ! tip->stat.binary && ! tip->stat.ascii && ! tip->stat.text ) /* CTS 1105829 */
			DBG0 (( "ERROR: no information for language '%c' \n", language ))
	    return ( ' ' );
	}

	return ( lip->optchar );
}

/*==========================================================================*/

static  int     remove_temp_obj 
(
    struct dep_info         * listp 
)
{
	int                     rc=0 ;
	struct dep_info         * dip , * cip ;
	struct trg_info         * tip ;

	DBG2 (( "rmtmpobj: called \n" ))

	rc = 0 ;
	for ( dip = listp ; dip != NULL ; dip = dip->next )
	{
	    tip = dip->target ;

	    /* execute the following code only for objects,
	       which are only contained in libraries */
	    if ( tip->type != TT_MEM ) continue ;
	    DBG8 (( "rmtmpobj: removing dependency '%s' \n", tip->name ))

	    /* scan the callers list for callers with make errors and
	       for callers not yet made */
	    rc = 0 ;
	    for ( cip = tip->callers ; cip != NULL ; cip = cip->next )
	    {
		if ( cip->target != NULL &&
		     cip->target->made == 0 ) rc ++ ;
	    }
	    if ( rc == 0 ) /* all callers made */
	    {
		/* if module is no longer needed and
		   it was really made, remove it */
		if ( tip->made > 1 ) 
		{
		    DBG8 (( "rmtmpobj: last ref, removing \n" ))
		    DBG5 (( "removing dependency '%s' \n", tip->name ))
		    (void) unlink_file ( tip->file.file );
		}
		else
		{
		    DBG8 (( "rmtmpobj: last ref, unmade \n" ))
		}
	    }
	    else
	    {
		/* if another library needs the module and
		   it was not really made, mark it as not made.
		   this enables another date checking against
		   the other library */
		if ( tip->made == 1 ) 
		{
		    DBG8 (( "rmtmpobj: more ref, unmade \n" ))
		    tip->made = 0 ;
		}
		else
		{
		    DBG8 (( "rmtmpobj: more ref, made \n" ))
		}
	    }
	}

	DBG4 (( "rmtmpobj: returning %d \n", rc ))
	return ( rc );
}


/*==========================================================================*/

static  int	copy_shrglob 
(
    struct trg_info	    * tip ,
    char                * caller_obj 
)
{
	int			rc=0 ;
	int			l ;
	char			* p ;
	struct dep_info		* dip ;
	struct stat		statbuf ;
	char			name [ FILENAMESIZE ];
	char			file [ FILENAMESIZE ];
	char			* lfile = NULL;

	DBG2 (( "copyshrglob: called \n" ))
	DBG3 (( "copyshrglob: target '%s' \n", tip->name ))

	if ( tip->type >= TT_LIB )
	{
	    rc = 0 ;
	    for ( dip = tip->dependencies ; dip ; dip = dip->next )
	    {
		if ( dip->target->made == INIT_STATE ) continue ;
		if ( dip->target->stat.shrglob != 1 ) continue ;
		rc |= copy_shrglob ( dip->target , tip->file.file );
	    }
	    return ( rc );
	}

	(void) sprintf ( name , "%s.gsz", tip->name );
	p = caller_obj ? caller_obj : tip->file.file ;
	l = strlen ( p );
	COPY ( file , p , l + 1 );

	/* strip basename */
	for ( p = file + l ; (p > file) && (*p != '/') ; p -- );

	/* strip obj directory */
	if ( p > file ) for ( p -- ; (p > file) && (*p != '/') ; p -- );

	/* strip version directory */
	if ( p > file ) for ( p -- ; (p > file) && (*p != '/') ; p -- );

	l = p - file ;
	if ( l >= sizeof(file)-32 )
		return (ewrite_all (tip, 0, -2, "ERROR: file name too long '%.40s|\n", file ));

	(void) sprintf ( file + l , "/%s/gsize/%s" ,
			versname(tip->version) , name );

	rc = filestat ( file , & statbuf );
	if ( rc < 0 )
	{
	    DBG0 (( "WARNING: missing file '%s' \n", file ))
		eprotwrite ( "WARNING: missing file '%s' \n", file );
	    return ( 0 );
	}

	rc = new_file ( name , DIR_GSIZ , versname(tip->version) , EMPTY , &lfile );
	if ( rc ) return ( rc );

	rc = vcopy_file ( file , lfile , statbuf.st_mtime , FT_TXT );
	if ( rc ) return ( rc );

    FREE_AND_SET_TO_NULL(lfile);

    return ( 0 );
}


/*==========================================================================*/

static  int	list_shrglobs 
(
    struct trg_info		* target ,
    FILE			* fp 
)
{
	int	                rc=0 ;
	int	                l ;
	char                * p ;
	struct dep_info		* dip ;
	struct trg_info		* tip ;
	char                file [ FILENAMESIZE ];

    DBG2 (( "listshrglob: called \n" ))
	DBG3 (( "listshrglob: target '%s' \n", target->name ))

	rc = 0 ;
	for ( dip = target->dependencies ; dip ; dip = dip->next )
	{
		tip = dip->target ;

	        if ( tip->made == INIT_STATE )
		{
		    DBG4 (( "list_shrglobs: dep   '%s' recursive \n",
							    tip->name ))
		    continue ;
		}
		if ( tip->stat.shrglob != 1 )
		{
		    DBG4 (( "list_shrglobs: dep   '%s' no shrglob \n",
							    tip->name ))
		    continue ;
		}
		if ( tip->type >= TT_LIB )
		{
		    DBG4 (( "list_shrglobs: dep   '%s' library \n",
							    tip->name ))
		    rc |= list_shrglobs ( tip , fp );
		    continue ;
		}
		DBG4 (( "list_shrglobs: dep   '%s' \n", tip->name ))

		l = strlen ( tip->file.file );
		if ( l >= sizeof(file) ) l = sizeof(file)-1 ;
		FILL ( file , 0 , sizeof(file) );
	        COPY ( file , tip->file.file , l );

	        p = strrchr ( file , '/' );
	        if ( ! p ) p = file ;
		if ( * p == '/' ) p ++ ;
		l = p - file ;

		if ( l + strlen(tip->name) >= sizeof(file)-16 )
			return (ewrite_all (tip, 0, -2, "ERROR: file name too long '%.40s|\n", file ));

		(void) sprintf ( p , "../../gsize/%s.gsz\n", tip->name );
		(void) fputs ( file , fp );
	}

	return ( rc );
}


/*==========================================================================*/

static	int	make_libpgm_dates_file 
(
    struct trg_info		*tip 
)
{
    int				rc=0 ;
    FILE			*fp ;
    char			* trgfile = NULL ;
    char			name [ FILENAMESIZE ];

	DBG2 (( "maklibpgmdat: called \n" ))
	DBG3 (( "maklibpgmdat: making '%s' \n", tip->name ))

	if ( opt_noexec ) return ( 0 );

    (void) sprintf ( name , "%s.dts", tip->name );
    (void) unlink ( name ); /* remove if already existent */

    fp = fopen ( name , "w" );
    if ( ! fp )
		return (ewrite_all (tip, 1, -2, "ERROR: cannot create '%s' \n", name ));

    rc = list_dates ( tip , fp );

    (void) fclose ( fp );
    if ( rc ) return ( rc );

    (void) new_file ( name , DIR_DATES , versname(tip->version) ,
					tip->layer , &trgfile );

    rc = move_file ( name , trgfile );
	if ( rc ) 
		return ( ewrite_all (tip, 1, rc, "ERROR: error while moving %s to %s \n", name , trgfile  ));

    rc = touch_file ( tip, trgfile , tip->date , tip->name , TRUE );

    FREE_AND_SET_TO_NULL(trgfile);

    return ( rc );
}


/*==========================================================================*/

static  int	list_dates 
(
    struct trg_info		*target ,
    FILE			*fp 
)
{
    int				    rc = 0 ;
    struct dep_info		*dip ;
    struct trg_info		*tip ;
    char			line [ FILENAMESIZE ];

    DBG2 (( "listdates: called \n" ))

    for ( dip = target->dependencies ; dip ; dip = dip->next )
    {
		tip = dip->target ;
		if ( tip->made == INIT_STATE ) continue ;

		(void) sprintf ( line , "%08lx %s\n", (long) tip->date , tip->name );
		rc = fputs ( line , fp );
		if ( rc < 0 )
			return (ewrite_all (tip, 1, -2, "ERROR: cannot write dates file for '%s': %s \n",
						tip->name , serrno() ));
    }

    return ( 0 );
}


/*==========================================================================*/

static	int	read_libpgm_dates_file 
(
    struct trg_info		*tip ,
    time_t			    *datep 
)
{
    int				rc=0 ;
    unsigned long		ldat ;
    time_t			date ;
    FILE			*fp ;
    struct stat			statbuf ;
    char			* trgfile = NULL;
    char			line [ FILENAMESIZE ];
    char			name [ FILENAMESIZE ];

	DBG2 (( "readlibpgmdates: called \n" ))
	DBG3 (( "readlibpgmdates: target '%s' \n", tip->name ))

    (void) new_file ( tip->name , DIR_DATES , versname(tip->version) ,
					tip->layer , &trgfile );
    trgfile = (char *) repalloc( trgfile, strlen( trgfile) + strlen(".dts") + 1 );
    ( void ) strcat ( trgfile, ".dts");
    rc = stat ( trgfile , &statbuf );
    if ( rc == -1 )
    {
	    if ( errno == ENOENT )
	    {
	        *datep = 0 ;
	        DBG1 (( "%s %s \n", ascdate(*datep) , trgfile ))
	        return ( 0 );
	    }
	    else
			return (ewrite_all (tip, 1, -2, "ERROR: cannot stat '%s' \n", trgfile ));

    }
    *datep = statbuf.st_mtime ;
    DBG1 (( "%s %s \n", ascdate(*datep) , trgfile ))

    fp = fopen ( trgfile , "r" );
    if ( ! fp )
		return (ewrite_all (tip, 1, -2, "ERROR: cannot open '%s' \n", trgfile ));

    while ( fgets(line , sizeof(line) , fp) )
    {
	    rc = sscanf ( line , "%08lx %s" , &ldat , name );
	    if ( rc != 2 )
	    {
	        DBG0 (( "ERROR: syntax error in '%s' \n", trgfile ))
	        rc = 0 ;
	        continue ;
	    }
	    date = ldat ;
	    DBG2 (( "%s %s \n", ascdate(date) , name ))

	    rc = insert_member ( name , date , tip , FALSE );
	    if ( rc < 0 )
	    {
	        break ;
	    }
	    rc = 0 ;
    }

    (void) fclose ( fp );

    FREE_AND_SET_TO_NULL(trgfile);

    return ( rc );
}


/*==========================================================================*/

static int remove_demands 
(
    struct trg_info		*tip 
)
{
	struct dep_info		* dip ;
    struct dem_info     * dmip ;
	char                * file_tmp = NULL;
	char				* tmp_o ;
	int					rc = 0;
	struct obj_info		* oip ;
	char				version;

	DBG2 (( "remdem: called \n" ))
	DBG3 (( "remdem: target '%s' \n", tip->name ))

    if ( opt_keep ) return( 0 );

	/* CTS 1110258 */
	version = (tip->version == ' ' || tip->version == 'n') ? vmakeversion : tip->version ; 

    
	/* remove copied demand objects */
    for ( dmip = tip->objdemands ; dmip != NULL ; dmip = dmip->next )
    {
        char    outfile [ FILENAMESIZE ] ;

        for ( dip = dmip->list ; dip != NULL ; dip = dip->next )
        {
		    if ( ! dip->target->stat.noobject )
            {
		        if ( ! dip->target->objects )
		        {
                    if ( * dmip->directory )
                    {
                        (void) strcpy( outfile , dmip->directory ) ;
                        (void) strcat( outfile , "/" ) ;
                    }
                    else 
                        outfile[ 0 ] = '\0';

			        tmp_o = strrchr ( dip->target->file.file, '/' );
				    if   ( tmp_o != NULL )  
                        tmp_o++;
				    else                    
                        tmp_o = dip->target->file.file;
                    (void) strcat( outfile, tmp_o );
			        
                    (void) new_file ( outfile , DIR_TMP , versname(version) ,
							        EMPTY , &file_tmp );
			        rc =  unlink_file ( file_tmp );
                    if ( rc )
                    {
                        DBG1 (( "remdem: can't remove demand '%s' (%s) \n", file_tmp, serrno() ))
                    }
		        }
		        else
		        {
			        for ( oip = dip->target->objects; oip ; oip = oip->next )
			        {
                        if ( * dmip->directory )
                        {
                            (void) strcpy( outfile , dmip->directory ) ;
                            (void) strcat( outfile , "/" ) ;
                        }
				        tmp_o = strrchr ( oip->file, '/' );
				        if   ( tmp_o != NULL )  
                            tmp_o++;
				        else                    
                            tmp_o = oip->file;
                        (void) strcat( outfile, tmp_o );

				        (void) new_file ( outfile , DIR_TMP , versname(version) ,
								        EMPTY , &file_tmp );
				        rc =  unlink_file ( file_tmp );
				        if ( rc ) 
                        {
                            DBG1 (( "remdem: can't remove demand '%s' (%s) \n", file_tmp, serrno() ))
                        }
			        }
		        }
            }
	    }
        /* remove directory */
        if ( * dmip->directory )
        {
            char    p [ NAMESIZE ];
            char    * q;

            (void) strcpy( p , dmip->directory );
            while ( q = strrchr( p , '/' ) )
            {
                if ( rmdir( p ) )
                {
                    DBG1 (( "remdem: can't remove directory '%s' (%s) \n", p, serrno() ))
                }
                * q  = '\0';
            }
            if ( rmdir( p ) )
            {
                DBG1 (( "remdem: can't remove directory '%s' (%s) \n", p, serrno() ))
            }
        }
    }

    for ( dmip = tip->srcdemands ; dmip != NULL ; dmip = dmip->next )
    {
        char    outfile [ FILENAMESIZE ] ;
        
        for ( dip = dmip->list ; dip != NULL ; dip = dip->next )
        {
            if ( * dmip->directory )
            {
                (void) strcpy( outfile , dmip->directory ) ;
                (void) strcat( outfile , "/" ) ;
            }
            else outfile[ 0 ] = '\0';
            tmp_o = strrchr( dip->target->file.file, '/' );
            if   ( tmp_o != NULL )  
                tmp_o++;
            else                    
                tmp_o = dip->target->file.file;

            (void) strcat( outfile, tmp_o );
            (void) new_file ( outfile , DIR_TMP , versname(version) ,
                EMPTY , &file_tmp );
            
            rc =  unlink_file ( file_tmp );
            if ( rc )
            {
                DBG1 (( "remdem: can't remove demand '%s' (%s) \n", file_tmp, serrno() ))
            }
        }
        /* remove directory */
        if ( * dmip->directory )
        {
            char    p [ NAMESIZE ];
            char    * q;

            (void) strcpy( p , dmip->directory );
            while ( q = strrchr( p , '/' ) )
            {
                if ( rmdir( p ) )
                {
                    DBG1 (( "remdem: can't remove directory '%s' (%s) \n", p, serrno() ))
                }
                * q  = '\0';
            }
            if ( rmdir( p ) )
            {
                DBG1 (( "remdem: can't remove directory '%s' (%s) \n", p, serrno() ))
            }
        }
    }

    FREE_AND_SET_TO_NULL(file_tmp);

	return ( 0 );
}


/*==========================================================================*/

static int copy_demands 
(
    struct trg_info		*tip 
)
{
	struct dep_info		* dip ;
    struct dem_info     * dmip ;
	char				* tmp_o ;
	int					rc = 0;
	struct obj_info		* oip ;
	char                name_s [ FILENAMESIZE ] ;
	char                * file_tmp = NULL;
	valid_bit_arr	    vbits ;
	char				version;

	DBG2 (( "copydem: called \n" ))
	DBG3 (( "copydem:target '%s' \n", tip->name ))

    VALID_BIT_FILL ( vbits );

	/* CTS 1110258 */
	version = (tip->version == ' ' || tip->version == 'n') ? vmakeversion : tip->version ; 

	/* all demands will be copied to work directory */
    for ( dmip = tip->objdemands ; dmip != NULL ; dmip = dmip->next )
    {
        char    outfile [ FILENAMESIZE ] ;

        for ( dip = dmip->list ; dip != NULL ; dip = dip->next )
        {
		    if ( ! dip->target->stat.noobject )
            {
		        if ( ! dip->target->objects )
		        {
                    if ( dmip->directory && * dmip->directory )
                    {
                        (void) strcpy( outfile , dmip->directory ) ;
                        (void) strcat( outfile , "/" ) ;
                    }
                    else 
                         outfile[0] = '\0';
					
					if ( dip->target->made == 0)
					{
						DBG4(("copy_demands: %s \n", dip->target->name))
						rc = make ( dip->target, tip, MK_FULL );
						if ( rc < 0 ) 
						{
							DBG0 (( "'%s' not made because of demand errors\n",
										tip->name ))
							ewrite_targetheader (tip, -2);
							eprotwrite ( "not made because of demand errors of: \n"); 
							eprotwrite ( "        %s (remake) \n", dip->target->name );
							eprotwrite ( EPROT_SEPARATOR ); 
							return ( rc );
						}
					}

					if ( ! dip->target->file.file )
					{
						DBG0 (( "%s\n", tip->name))
						DBG0 (( "ERROR: demand file '%s' not created before\n", dip->target->name ))
						DBG0 (( "       (missing dependency information)\n" ))
						ewrite_targetheader (tip, -2);
						eprotwrite ( "ERROR: demand file '%s' not created before\n", dip->target->name );
						eprotwrite ( "       (missing dependency information)\n" );
						eprotwrite ( EPROT_SEPARATOR ); 
						return ( - 2 );
					}
			        tmp_o = strrchr ( dip->target->file.file, '/' );

			        if   ( tmp_o != NULL )  
                        tmp_o++;
			        else                    
                        tmp_o = dip->target->file.file ;

                    (void) strcat( outfile, tmp_o );

                    if ( dmip->directory && * dmip->directory ) 
                        check_dir( outfile );

			        (void) new_file ( outfile , DIR_TMP , versname(version)  ,
							        EMPTY , &file_tmp );
			        rc = vcopy_file ( dip->target->file.file , file_tmp , 
					        dip->target->date , FT_BIN );
			        if ( rc ) return ( -1 );
		        }
		        else
		        {
			        for ( oip = dip->target->objects; oip ; oip = oip->next )
			        {
                        if ( dmip->directory && * dmip->directory )
                        {
                            (void) strcpy( outfile , dmip->directory ) ;
                            (void) strcat( outfile , "/" ) ;
                        }
				        tmp_o = strrchr ( oip->file, '/' );
				        if ( tmp_o != NULL )    
                            tmp_o++;
				        else                    
                            tmp_o = oip->file;
                        (void) strcat( outfile, tmp_o );
                        if ( dmip->directory && * dmip->directory ) 
                            check_dir( outfile );
				        (void) new_file ( outfile , DIR_TMP , versname(version) ,
								        EMPTY , &file_tmp );
				        rc = vcopy_file ( oip->file , file_tmp , 
						        dip->target->date , FT_BIN );
				        if ( rc ) return ( -1 );
			        }
		        }
            }
	    }
    }

    for ( dmip = tip->srcdemands ; dmip != NULL ; dmip = dmip->next )
    {
        char    outfile [ FILENAMESIZE ] ;

        for ( dip = dmip->list ; dip != NULL ; dip = dip->next )
        {
            if ( dmip->directory && * dmip->directory )                
            {
                (void) strcpy( outfile , dmip->directory ) ;
                (void) strcat( outfile , "/" ) ;
            }
            else 
                outfile[ 0 ] = '\0';
            
            if ( ! dip->target->file.file )
            {
                /* strip the variant identifier */
		        (void) get_basename( dip->target->name, name_s );
	            DBG8 (( "makmod: searching source file '%s' \n", name_s ))
        	    dip->target->status = PATH_LEVEL ;

	            rc = get_file ( name_s , DIR_SRC , dip->target->layer , EMPTY ,
			        FK_SOURCE | FK_LEVEL , vbits ,
			        &dip->target->file.file , &dip->target->date , &dip->target->status, 0 );
	            if ( rc < 0 )
	            {
			        DBG4 (( "get file error, returning %d \n", rc ))
			        return ( rc );
	            }
				if ( rc == 0 )
					rc = get_file ( name_s , DIR_SRC , dip->target->layer , EMPTY ,
			                        FK_SOURCE | FK_LEVEL , vbits ,
			                        &dip->target->file.file , &dip->target->date , &dip->target->status, 1 );

	            if ( rc == 0 )
					return (ewrite_all (tip, 0, -2, "ERROR: missing source    '%s' \n" ,dip->target->file.file));
                else
			        DBG8 (( "%s %s \n", ascdate(dip->target->date) , dip->target->file.file ))
            }
            
            tmp_o = strrchr( dip->target->file.file, '/' );
            
            if   ( tmp_o != NULL )  
                tmp_o++;
            else                    
                tmp_o = dip->target->file.file ;
            
            (void) strcat( outfile, tmp_o );
            
            if ( dmip->directory && * dmip->directory ) 
                check_dir( outfile );
            
            (void) new_file ( outfile , DIR_TMP , versname(version) ,
                EMPTY , &file_tmp );
            rc = vcopy_file ( dip->target->file.file , file_tmp , 
                dip->target->date , FT_BIN );
            if ( rc ) return ( -1 );
        }
    }

    FREE_AND_SET_TO_NULL(file_tmp);

	return ( 0 );
}


/*==========================================================================*/

static	char	* get_objectext
(
    struct trg_info			* tip 
)
{
	register char			language ;
	register struct lang_info	* lip ;

	DBG2 (( "optchr: called \n" ))
	DBG3 (( "optchr: target '%s' \n", tip->name ))

	language = tip->language ;
	if ( (language == LANG_PASCAL_P[ 0 ]) && tip->stat.level_c ) language = LANG_PASCAL_T[ 0 ] ;
	for ( lip = lng_lists ; lip ; lip = lip->next )
	{
	    if ( lip->language == language ) break ;
	}
	if ( ! lip )
	{
		if ( ! tip->stat.binary && ! tip->stat.ascii && ! tip->stat.text ) /* CTS 1105829 */
			DBG0 (( "ERROR: no information for language '%c' \n", language ))
	    return ( std_objectext );
	}

	return ( lip->objectext );
}

/*==========================================================================*/

static  int     make_javaproject 
(
    struct trg_info         * tip, 
	time_t                  date_dep
)
{
    int                 rc   = 0;
    int                 move_project = 1;
    char                basedir[ FILENAMESIZE ];
    char                * src = NULL;
    char                * dst = NULL;
	struct dsc_info     * sip ;
	struct trg_info     * dtip ;
	struct dem_info     * dmip ;
	struct dep_info		* dip ;
    struct obj_info     * oip ;
	valid_bit_arr		vbits ;
	
	DBG2 (( "makjavp: called \n" ))

    VALID_BIT_FILL ( vbits );

    if ( opt_noexec ) return( 0 );

	rc = read_libpgm_dates_file( tip , &tip->date );
	if ( rc ) return ( rc );

    dtip = tip;
    for ( dmip = tip->srcdemands ; dmip != NULL ; dmip = dmip->next )
    {
        for ( dip = dmip->list ; dip != NULL ; dip = dip->next )
        {
	        /* strip the variant identifier */
            
            src = (char *) repalloc( src, strlen( dip->target->name ) + 1 );
		    (void) get_basename( dip->target->name, src );

	        rc = get_file( src , DIR_SRC , dip->target->layer , EMPTY ,
			    FK_SOURCE | FK_LEVEL , vbits ,
                &dip->target->file.file , & dip->target->date , & dip->target->status, 0 );
	        if ( rc < 0 )
	        {
			    DBG4 (( "get file error, returning %d \n", rc ))
			    return ( rc );
			}
			if ( rc == 0 )
				rc = get_file( src , DIR_SRC , dip->target->layer , EMPTY ,
			                   FK_SOURCE | FK_LEVEL , vbits ,
                               &dip->target->file.file , & dip->target->date , & dip->target->status, 1 );
	        if ( rc == 0 )
	        {
				return (ewrite_all (tip, 0, -2, "ERROR: missing source    '%s' \n" ,src));
	        }
            if ( dtip->date < dip->target->date )
	    	    dtip = dip->target ; /* remark newest dependency */
        }
    }

    FREE_AND_SET_TO_NULL(src);

    if ( tip->date >= dtip->date && ! tip->stat.uncond )
    {
	    DBG2 (( "up to date        %s \n", tip->name ))
		return( 1 );
    }

	rc = set_work_dir( tip->version );
	if ( rc < 0 )
	{
	    DBG8 (( "makjavaproj: setwrkdir error %d \n", rc ))
	    return ( - 3 );
	}
	DBG8 (( "makjavaproj: work dir now '%s' \n", tmp_dir ))

    rc = copy_demands( tip );
    if ( rc ) return( rc );

    /* compile files */
    for ( dmip = tip->srcdemands ; dmip != NULL ; dmip = dmip->next )
    {
        for ( dip = dmip->list ; dip != NULL ; dip = dip->next )
        {
            char    * p, * oext;

            if ( ! dip->target->objects )
            {
		        dip->target->objects = (struct obj_info *) palloc ( sizeof(struct obj_info) );
		        if ( dip->target->objects == NULL )
		        	return (ewrite_all (tip, 0, -3, "ERROR: insufficient dynamic memory \n" ));
		        FILL (  dip->target->objects , 0 , sizeof(struct obj_info) );

                oext = get_objectext( dip->target );
                if ( p = extension_exists( dip->target->name ) )
			    {	
                    dip->target->objects->file = (char *) palloc( strlen( dmip->directory ) + 1 + 
                        p - dip->target->name + strlen( oext ) + 1 );
                    if ( * dmip->directory )
                    {
                        (void) strcpy( dip->target->objects->file, dmip->directory );
                        (void) strcat( dip->target->objects->file, "/" );
                    }
				    (void) strncat( dip->target->objects->file, dip->target->name, p - dip->target->name );
				    dip->target->objects->file[ strlen( dmip->directory ) + 1 + p - dip->target->name ] = '\0';
				    (void) strcat( dip->target->objects->file, oext );
			    }
			    else
                {
                    dip->target->objects->file = (char *) palloc( strlen( dmip->directory ) + 1 +
                        strlen( dip->target->name ) + strlen( oext ) + 1 );
                    if ( * dmip->directory )
                    {
				        (void) sprintf( dip->target->objects->file , "%s/%s%s", dmip->directory, dip->target->name, oext );
                    }
                    else
                    {
				        (void) sprintf( dip->target->objects->file , "%s%s", dip->target->name, oext );
                    }
                }
            }
            else
            {
                for ( oip = dip->target->objects; oip ; oip = oip->next )
                {
                    oip->file = (char *) repalloc( oip->file, strlen( dmip->directory ) + 1 +
                        strlen( oip->name_o ) + 1 );
                    if ( * dmip->directory )
                    {
				        (void) sprintf( oip->file , "%s/%s", dmip->directory, oip->name_o );
                    }
                    else
                    {
				        (void) sprintf( oip->file , "%s", oip->name_o );
                    }
                }
            }
			
            dip->target->afile = (char *) repalloc( dip->target->afile, strlen( dmip->directory ) + 
                strlen( dip->target->name ) + 2 );
            (void) sprintf( dip->target->afile, "%s/%s", dmip->directory, dip->target->name );

	        outstamp ( "%-9s %-5s", dip->target->name , versname(dip->target->version) );
			if (opt_optdebug)  /* CTS 1110663 */
				outstamp_add ( " %s", "debug (optimized)" ); 
			else
				if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) )
					outstamp_add ( " %s", "debug" ); 
	        outstamp_add ( "\n" );

            rc = compile_file( dip->target, dip->target->afile, TRUE, 
                dip->target->objects->file, tip->objects ? TRUE : FALSE, (time_t) 0, date_dep );
            if ( rc ) return( rc );
        }
    }

	outstamp ( "%-9s %-5s", tip->name , versname(tip->version) );
	if (opt_optdebug)  /* CTS 1110663 */
			outstamp_add ( " %s", "debug (optimized)" ); 
	else
		if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) ) 
			outstamp_add ( " %s", "debug" );
	outstamp_add ( "\n" );

    if ( tip->fixed_file )
    {
        if ( ! strcmp( get_extension( tip->file.file ), "jar" ) )
        {
            FILE        * fp;
            char        jarfile  [ NAMESIZE ];
            char        jarfile_c[ NAMESIZE ];

            /* create JAR file */
            remove_ext( tip->name, jarfile );
            (void) strcat( jarfile, ".jar");

            move_project = 0;
            rc = cmdpgm ( env_TOOLSHELL );
	        if ( env_TOOLOPT ) rc |= cmdarg( 1, env_TOOLOPT );
	        rc |= cmdscript ( CMD_LINKJAR );
	        if ( opt_keep & 2 ) rc |= cmdarg( 1,  "-k" );
			if (opt_optdebug)  /* CTS 1110663 */ 
				rc |= cmdarg( 1,  "-optdebug", DSC_DELIMITER );
			else
				if ( tip->stat.debug || ( opt_debug && ! tip->stat.nodebug ) ) 
					rc |= cmdarg( 1,  "-g" );
	        if ( tip->stat.profile == 1 || opt_profile ) rc |= cmdarg( 1,  "-p" );
	        if ( rc ) return ( rc );

	        for ( sip = tip->descriptions ; sip != NULL ; sip = sip->next )
	        {
	            /* descriptors beginning alnum are concom options */
	            if ( isalnum( sip->descriptor[ 0 ] ) ) continue ;
	            rc |= cmdarg( 1,  "%s%s", sip->descriptor , sip->value );
	            if ( rc ) return ( rc );
	        }

	        for ( dip = tip->options ; dip != NULL ; dip = dip->next )
	        {
		        rc |= cmdarg( 1,  dip->target->file.file );
	        }
            rc |= cmdarg( 1, jarfile );


            /* create content file */
            (void) strcpy( jarfile_c, jarfile );
            (void) strcat( jarfile_c, ".lst" );
            fp = fopen ( jarfile_c , "w" );
            if ( ! fp )
				return (ewrite_all (tip, 0, -2, "ERROR: cannot create '%s' \n", jarfile_c ));

            for ( dmip = tip->srcdemands ; dmip != NULL ; dmip = dmip->next )
            {
                for ( dip = dmip->list ; dip != NULL ; dip = dip->next )
                {
                    for ( oip = dip->target->objects; oip ; oip = oip->next )
                    {
		                if ( ( fputs( oip->file , fp ) < 0 ) || ( fputs( "\n", fp ) < 0 ) )
							return (ewrite_all (tip, 0, -2, "ERROR: cannot write dates file for '%s': %s \n",
						                tip->name , serrno() ));
                    }
                }
            }
            (void) fclose ( fp );

        	rc |= cmdarg( 1, jarfile_c );
            if ( rc ) return ( rc );

            rc = cmdexe();
            
			/* CTS 1112006 */
			flush_outputlist (tip, rc, 1 );
			if (rc)
				return ( rc );
			
            rc = unlink_file( jarfile_c );
            if ( rc )
            {
                DBG1 (( "makjavaproj: can't remove '%s' (%s) \n", jarfile_c, serrno() ))
            }
            
   			tip->date = time ( (time_t *) 0 );
			tip->date |= 1 ; /* force an odd value for vmake generated files */

	        rc = touch_file ( tip, jarfile , tip->date , "" , TRUE );
			if ( rc ) 
				return ( ewrite_all (tip, 1, rc, "ERROR: error while touching %s\n",  jarfile));

            rc = move_file( jarfile, tip->file.file );
			if ( rc ) 
				return ( ewrite_all (tip, 1, rc, "ERROR: error while moving %s to %s \n", jarfile, tip->file.file ));
        }
        else
        {
            (void) strcpy( basedir, tip->file.file );
        }
    }
    else
    {
        (void) strcpy( basedir, "%s/projects/java/%s/" );
    }

    /* move/remove project */
    for ( dmip = tip->srcdemands ; dmip != NULL ; dmip = dmip->next )
    {
        for ( dip = dmip->list ; dip != NULL ; dip = dip->next )
        {
            for ( oip = dip->target->objects; oip ; oip = oip->next )
            {
                if ( move_project )
                {
                    (void) new_file ( oip->file , basedir , tip->layer , EMPTY , &dst );
                    rc = move_file( oip->file, dst );
					if ( rc ) 
						return ( ewrite_all (tip, 1, rc, "ERROR: error while moving %s to %s \n", oip->file, dst  ));
                }
                else
                {
                    rc = unlink( oip->file );
					if ( rc ) return ( ewrite_all (tip, 1, rc, "ERROR: error while deleting %s \n", oip->file));
                }
            }
        }
    }
    FREE_AND_SET_TO_NULL(dst);


    rc = remove_demands( tip );
    if ( rc ) return( rc );

    tip->date = dtip->date;
	rc = make_libpgm_dates_file( tip );
	if ( rc ) return ( rc );
    
	rc = 2;  /* really made */
    DBG4 (( "makjavp: returning %d \n", rc ))
	return ( rc );
}

global int  set_exec_permissions (char *filename)
{
	int rc;
	struct stat		        statbuf ;
	
	rc = stat ( filename , &statbuf );
	if ( rc == -1 )
	{
		DBG0 (( "cannot stat '%s': %s \n", filename , serrno() ))
		return ( rc );
	}
	rc = chmod ( filename , statbuf.st_mode | 0111 );
	if ( rc == -1 )
	{
		DBG0 (( "cannot chmod '%s': %s \n", filename , serrno() ))
		return ( rc );
	}
	return (0);
}

