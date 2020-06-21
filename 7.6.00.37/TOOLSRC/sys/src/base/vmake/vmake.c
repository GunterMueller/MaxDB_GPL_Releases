/*      vmake.c         

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

#include        "vmake.h"
#include        "vmakvars.h"
#include        "vmakcoll.h"
#include        "vmakmake.h"
#include        "vmakcomm.h"
#include        "vmakmisc.h"
#include        "vmakdist.h"

/*==========================================================================*/
#ifndef linux
char    *USAGE[] =
#else
static  char    *USAGE[] =
#endif
{
"usage: vmake [options] target[+<globalvariant>][//option...] ... \n",
"    -Dl, -Y, -C debug level (l=0..9), debug concom, don't debug collection \n",
"    -i| -w| -b  interactive | background and watch | background            \n",
"    -c| -m      print collection only | print module list only             \n",
"    -x          print commands before execution                            \n",
"    -n| -N      no execution | no execution and no protocol                \n",
"    -r, -s      retrieve unpacked files only, suppress lib and link phases \n",
"    -L| -S      lint | sizeof                                              \n",
"    -l| -g      local make | global make (supersede VMAKE_OPTION=lg)       \n",
"    -p, -e| -E  profile , debug all targets, debug with optimizing         \n",
"    -d          do not make dependencies                                   \n",
"    +e          deactivate global debug                                    \n",
"    +c          print collection additionally                              \n",
"    -u          unconditionally make named targets (no date check)         \n",
"    -U          unconditionally make  ALL  targets (except includes)       \n",
"    -I          together with -U: unconditionally make includes too        \n",
"    -k| -K      keep temporary files | -k + set keep option for tools      \n",
"    -M          display the manual entry                                   \n",
"    -R          make 'REQUIREMENTS'                                        \n",
"    -v          display vmake version                                      \n",
"    -F          create a list of files for distribution                    \n",
0};

/*==========================================================================*/


/* local functions */

static  int     create_directories (void);
static  int     create_dir      (char*,char*,char*);
static  int     get_options     (int,char**,int*);
static  int     get_environment (void);
static  int     analyze_env_opt (void);
static  int     get_vmake_path  (void);
static  int     append_path     (char*,struct path_info**,struct path_info**);
static  void    reduce_mod_type (void);
static  void    lint_files      (void);
static  void    print_cache     (void);
static  void    print_com_list  (struct mod_com_info*);
static  void    print_collection (void);
static  void    print_target    (struct trg_info*);
static  void    print_modlist   (void);
static  void    print_manual    (void);
static  void    print_dirs      (struct dir_info*);
static  void    print_direntry  (struct dir_entry*);
static int print_statistics (struct trg_info*,char*);

#if unix
static  int         fork_vmake  (void);
static  SIG_RTRN    catch_signal (int);
#else

#ifdef WIN32
static BOOL WINAPI catch_event ( DWORD);
static  DWORD WINAPI checkforStopRequest (pid_t);
static int createStopRequestThread (void);
static char EVENT_String [16] = "VMAKE_STOP_";
#endif
#endif


/* local data */

static  pid_t   vmakepid ;



/*==========================================================================*/

global  int     main 
(
    int     argc ,
    char    ** argv 
)
{
    int             rc=0 ;
    int             errcnt ;
    int             argn ;
    int             pfd ;
    int             olddbglvl ;
    struct trg_info *tip ;
    struct dep_info *dip ;
    char            *p ;
    char            trgline [ LINESIZE ] ;
    extern int      dbgdbglvl;
	int             found_option_separator;
	int				makerc = 0;


	/*
     *  The variable 'dbgdbglvl' is set one number higher than
     *  required to support the extended protocol (xprot).
     */
    dbgdbglvl ++ ;

    /*
     *  Save the argument vector.
     */
    opt_argv = argv ;

    /*
     *  Don't buffer stdout/stderr.
     */
    setbuf ( stdout , (char*)0 );
    setbuf ( stderr , (char*)0 );

#ifdef  WIN32
    (void) putenv ( "TZ=" );
#endif
    tzset ();

    rc =  get_options( argc , argv , & argn );
    rc += get_environment();

	if (opt_check_for_running_vmake)
	{
		rc = checkforPIDFile(0);
		DBG0 (("vmake with pid %d is running\n", rc ))
		if (rc > 0)
			rc = 1;
		return (rc);
	}

	if (( opt_distrib ) && ( *env_VMAKE_VERSION != 'f' ))
	{
		DBG0 (( "Error: Distribution runs only in fast mode\n" ));
		return( -1 );
	}

    if ( opt_man )
    {
        (void) get_vmake_path ( );
        print_manual ();
        return ( 0 );
    }

    rc += analyze_env_opt( );
	rc += get_vmake_path( );

    if ( rc != 0  ||  ! (  argc - argn >= 1 ) )
    {
        DBG0 (( "%s\n", vmake_version + 4 ))
        for ( rc = 0 ; USAGE[rc] ; rc ++ )
            dowrite ( USAGE[rc] , (int) strlen(USAGE[rc]) );
        return ( 1 );
    }

    /* clear unused file descriptors */
    for ( pfd = 3 ; pfd < 1024 ; pfd ++ ) (void) close ( pfd );

	/* initialize working directories (CTS 1105673) */
	rc = init_wrk ();
    if ( rc ) return ( rc );

	rc = init_outdir ();
    if ( rc ) return ( rc );

	rc = create_directories ();
    if ( rc ) exit_vmake (rc, TRUE);

	vmakepid = getpid ();
	rc = createPIDFile(vmakepid);
	if ( rc < 0)
	{
		exit_vmake (rc, FALSE);
	}
    
#ifdef  WIN32

	_itoa (vmakepid, EVENT_String + strlen(EVENT_String), 10);
	/* DBG0 (( "Waiting for EVENT for stop request: '%s'\n", EVENT_String ))*/
	hStopRequest = CreateEvent( 
            NULL,     // no security attributes
            TRUE,    // auto-reset event
            FALSE,     // initial state is signaled
            EVENT_String);    // object not named

	if (! hStopRequest)
	{
		DBG0 (("ERROR: Can't create eventhandle for stop request\n"))
		exit_vmake (-1, TRUE);
	}	
	
	/* DBG0 (( "Init Ctrl handler\n" ))*/
	if (! SetConsoleCtrlHandler( (PHANDLER_ROUTINE) catch_event, TRUE ))
	{
		DBG0 (("ERROR: SetControlHandler failed\n"))
		exit_vmake (-1, TRUE);
	}

	/* DBG0 (( "start thread for stop request handling\n" ))*/
	if (createStopRequestThread() != 0)
	{
		DBG0 (("ERROR: Can't create thread for stop request waiting\n"))
		exit_vmake (-1, TRUE);
	}

#endif

    /*
     *  Open startup protocol.
     */
    if ( ! opt_modlist &&  opt_collection != 1 )
    {
        set_protocol ( vmake_coll , vmakeversion );
    }

    rc = set_work_dir ( vmakeversion );
    if ( rc < 0 )
    {
        DBG8 (( "vmake: setwrkdir error %d \n", rc ))
        if ( ! opt_modlist && opt_collection != 1 )
        {
            /* save startup protocol */
            set_protocol ( argv[argc-1] , vmakeversion );
            set_protocol ( EMPTY , ' ' );
        }
		exit_vmake(2, TRUE);
        
    }
    DBG8 (( "vmake: set work dir '%s' \n", tmp_dir ))

    DBG7 (( "vmake: MAXFNMLGT %d \n", MAXFNMLGT ))
    DBG7 (( "vmake: NAMESIZE  %d \n", NAMESIZE  ))
    DBG7 (( "vmake: FILENMSIZ %d \n", FILENAMESIZE ))

    if ( opt_nodebug_col )
    {
        olddbglvl = dbgdbglvl ;
        dbgdbglvl = 1 ;
    }

    if( ( ( argc - argn ) == 1 ) || opt_nodependencies ) opt_nodircache = 1;
    errcnt = 0 ;

	for ( ; argn < argc ; argn ++ )
    {
        p = argv [ argn ] ;
        rc = strlen ( p );
        if ( rc >= sizeof(trgline) )
        {
            DBG0 (( "argument too long '%.56s...' \n" , p ))
            if ( ! opt_modlist && opt_collection != 1 )
            {
                /* save startup protocol */
                set_protocol ( argv[argc-1] , vmakeversion );
                set_protocol ( EMPTY , ' ' );
            }
            exit_vmake (1, TRUE); ;
        }
        COPY ( trgline , p , rc );
        trgline [ rc ] = '\0' ;

		found_option_separator = 0;
        for ( p = trgline ; * p != '\0' ; p ++ )
        {
			if ( * p == ' ' ) break;
			if ( * p == OPTION_SEPARATOR )
			{
				if ( (found_option_separator) || ( trgline[0] != INDEX_SYMBOL ) ||  ( * (p+1) == '-' ) )
					* p = ' ' ;
				if ( * (p+1) == OPTION_SEPARATOR )
				{
					found_option_separator = 1;
					* p = ' ';
					* (p+1) = ' ';
				}
			}
        }

        if ( opt_collection )
        {
            voutwrite ( "start of vmake collection data\n" );
        }

		rc = collect ( trgline , vmakeversion , getdefvers( vmakeversion ) , DT_INCDEP,
            LEVEL_UNKNOWN, (struct trg_info *) NULL , & tip );
        if ( rc == 0 ) continue ; /* nothing to make */
        if ( rc < 0 )
        {
            errcnt ++ ;
            DBG0 (( "error collecting '%s' \n", argv [ argn ] ))
        }
        else    DBG8 (( "vmake: collected '%s' \n", argv [ argn ] ))

        rc = insert_dependency ( tip , & make_list );
        if ( rc < 0 )
        {
            errcnt ++ ;
            DBG0 (( "error inserting '%s' \n", argv [ argn ] ))
        }
        else    DBG8 (( "vmake: inserted '%s' \n", argv [ argn ] ))
    }

    if ( errcnt )
    {
        DBG8 (( "target collect errors %d \n", errcnt ))
        if ( ! opt_modlist && opt_collection != 1 )
        {
            /* save startup protocol */
			/*
            if ( tip && *tip->name ) 
                set_protocol ( tip->name , vmakeversion );
            else
			*/
                set_protocol ( argv[argc-1] , vmakeversion );
			
			eprotwrite ("------------------------------------------------------------\n");
			eprotwrite ("END: MAKE      TARGET: %s   RESULT: ERROR\n", argv[argc-1] );
			eprotwrite ("------------------------------------------------------------\n\n");
            
            set_protocol ( EMPTY , ' ' );
        }
        exit_vmake (2, TRUE);
    }

#ifndef RELOC_LIB
    if ( opt_nodep && ! vopt_mod_only && ! vopt_reloc_lib )
    {
        /* change TT_MOD to TT_MEM if module is only in libraries */
        reduce_mod_type ( );
    }
#endif  /*RELOC_LIB*/

    if ( opt_nodebug_col )
    {
        dbgdbglvl = olddbglvl ;
    }

    if ( opt_collection )
    {
        if ( dbgdbglvl > 1 ) print_cache ( );
        print_collection ( );
        voutwrite ( "end of vmake collection data\n" );
		if ( opt_collection  == 1)
		{
			DBG4 (( "vmake: returning %d \n", 0 ))
			exit_vmake (0, TRUE);;
		}
    }

	/* statistics */
	for ( dip = make_list ; dip ; dip = dip->next )
	{
		count_dependencies(dip->target);
	}

    if ( opt_modlist )
    {
        print_modlist ( );
        DBG4 (( "vmake: returning %d \n", 0 ))
        exit_vmake (0, TRUE);;
    }

#if unix
    if ( opt_interactive == 0 )
    {
        rc = fork_vmake ();
        if ( rc < 0 )
        {
            DBG8 (( "vmake: fork_vmake error %d \n", rc ))
            /* save startup protocol */
			/*
            if ( tip && *tip->name ) 
                set_protocol ( tip->name , vmakeversion );
            else
			*/
                set_protocol ( argv[argc-1] , vmakeversion );
			eprotwrite ("------------------------------------------------------------\n");
			eprotwrite ("END: MAKE      TARGET: %s   RESULT: ERROR\n", argv[argc-1] );
			eprotwrite ("------------------------------------------------------------\n\n");
            set_protocol ( EMPTY , ' ' );
            exit_vmake (-2, TRUE);;
        }
        DBG8 (( "vmake: running with pid %ld \n", (long) vmakepid ))
		vmakepid = getpid ();
		if ( env_SUPPRESS_VMAKE_PIDFILE )
		{
			unlinkPIDFile(1);
			rc = createPIDFile(vmakepid);
			if ( rc < 0)
			{
				exit_vmake (rc, FALSE);
			}
		}
		else
		{
			writePIDtoFile(vmakepid);
		}
    }
   /*  else */  

#endif /*unix*/
	/* vmakepid = getpid ();*/
	
	if ( rc < 0)
	{
		/*
		if ( tip && *tip->name ) 
            set_protocol ( tip->name , vmakeversion );
        else
		*/
        set_protocol ( argv[argc-1] , vmakeversion );
		eprotwrite ("------------------------------------------------------------\n");
		eprotwrite ("END: MAKE      TARGET: %s   RESULT: ERROR\n", argv[argc-1] );
		eprotwrite ("------------------------------------------------------------\n\n");
        set_protocol ( EMPTY , ' ' );
        exit_vmake (2, TRUE);;
	}

    /*
     *  The trap signal will be used to pause the execution of vmake.
     */
#if unix
    (void) signal ( SIGTRAP , catch_signal );
#endif

	date_make = time ( (time_t *) 0 );
	date_make |= 1 ; /* force an odd value for vmake generated files */

    for ( dip = make_list ; dip ; dip = dip->next )
    {
        /*
         *  If a target is named on the command line and it is
         *  additionally a dependency of something else,
         *  don't make it explicitely, so you reduce the number
         *  of protocols generated.
         *  If, however, the target is alone on the command line,
         *  make it anyway. This unlikely case occurs, if there
         *  is a cycle in the dependencies (e.g. a target includes
         *  itself).
         */
        if ( dip->target->callers && make_list->next )
        {
            DBG1 (( "vmake: made as dependency '%s' \n",
                        dip->target->name ))
            continue ;
        }
        DBG4 (( "vmake: callers 0x%08lx \n", (long) dip->target->callers ))
        DBG4 (( "vmake: next    0x%08lx \n", (long) make_list->next ))

        if ( opt_lint )
            set_protocol ( "lint" , vmakeversion );
        else
        {
            char    prot[ FILENAMESIZE ];

            if ( is_oldstyled_source( dip->target->name, NULL, NULL ) || 
                is_mapped_source( dip->target->name, NULL, NULL ) )
                sprintf( prot, "%s" , dip->target->name );
            else if (
					/*strchr( dip->target->own_layer, '/' ) ||
                      ( dip->target->type >= TT_LIB && 
                      dip->target->type <= TT_MAC && 
					  PTS 1102210 */
					  (dip->target->own_layer && * dip->target->own_layer )
                    )
                /* its a multistage layer */
                sprintf( prot, "%s/%s" , dip->target->own_layer, dip->target->name );
            else
                    sprintf( prot, "%s" , dip->target->name );
            set_protocol ( prot , dip->target->version );
        }

        DBG7 (( "vmake: making '%s' \n", dip->target->name ))

		dip->target->act_caller=(struct trg_info *) NULL;
        rc = make ( dip->target , (struct trg_info *) NULL , MK_FULL );
                   /* ^ calling target info pointer */

		if (dip->target->report & VMAKE_REPORT_MODE_PERCENT)
		{
				DBG0 (("VMAKE REPORT:  %d %% of targets finished\n" , (int) ((100*finished_target_counter)/all_target_counter)))			
				DBG0 (("VMAKE REPORT:  %d of %d finished\n" , finished_target_counter, all_target_counter))			
		}

		if (opt_silent_mode && rc > 0)
			dowrite_to_console ("\n", 1);
        if ( rc <= 0 )
        {
            errcnt ++ ;
            DBG8 (( "error making '%s' \n", dip->target->name ))
        }
        else    
		{
			if ( rc > makerc ) makerc = rc;
			DBG8 (( "vmake: made '%s' \n", dip->target->name ))
		}

		/* &gar CTS 1000237 -> */
		
		if ( opt_distrib )
		{
			if ( create_distlist(tip->name) < 0)
				errcnt ++ ;
		}
		/* <- &gar CTS 1000237 */

		/* CTS ERRROFILE */
		eprotwrite ("------------------------------------------------------------\n");
		eprotwrite ("END: MAKE      TARGET: %s   RESULT: %s\n", dip->target->name,
						rc<=0 ? "ERROR": (rc== 1 ? "NO ACTION" : "OK" ));
		eprotwrite ("------------------------------------------------------------\n\n");
		/* <- CTS 1112006 */
    }

	/* CTS 1111841 -> */
	if ( ! opt_distrib )
	{
		struct com_info         * cip ;
		for ( cip=com_lists; cip; cip = cip->next)
			mprotwrite ( "%s desc/%s.com\n", ascdate(cip->date) , cip->layer);
		if ( xtr_date ) mprotwrite ( "%s desc/Extra\n", ascdate(xtr_date) );
		if ( map_date ) mprotwrite ( "%s desc/DirectoryMapping\n", ascdate(map_date) );
		if ( lng_date ) mprotwrite ( "%s desc/Langinfo\n", ascdate(lng_date) );
		if ( lngext_date ) mprotwrite ( "%s desc/LangextInfo\n", ascdate(lngext_date) );
	}
	/* <- CTS 1111841 */


    if ( opt_lint )
        lint_files ();

    rc = 0 ;
#if ! unix
    if ( warn_target_link )
    {
        DBG0 (( "WARNING: %d targets could not be copied! \n", 
                        warn_target_link ))
        rc = 2 ;
    }
#endif

    set_protocol ( EMPTY , ' ' ); /* write termination time */


    if ( opt_noexec  || env_VMAKE_PRINT_STATISTICS )
    {
        /* print statistics */

        voutwrite ( "\n" );
        voutwrite ( "statistics: \n\n" );
		print_statistics(macros, "macros");
		print_statistics(programs, "programs");
		print_statistics(dynlinklibs, "dynlinklibs");
		print_statistics(relocs, "relocs");
		print_statistics(libraries, "libraries");
		print_statistics(modules, "modules");
		print_statistics(includes, "includes");
		print_statistics(files, "files");
		print_statistics(command_list, "commands");
		print_statistics(extra_list, "extras");

/*        DBG0 (( "macros:      %d \n", i )) 
        for ( i = 0, tip = programs ; tip ; tip = tip->next )i++;
        DBG0 (( "programs:    %d \n", i ))
        for ( i = 0, tip = dynlinklibs ; tip ; tip = tip->next ) i++;
        DBG0 (( "dynlinklibs: %d \n", i ))
        for ( i = 0, tip = relocs ; tip ; tip = tip->next ) i++;
        DBG0 (( "relocs:      %d \n", i ))
        for ( i = 0, tip = libraries ; tip ; tip = tip->next ) i++;
        DBG0 (( "libraries:   %d \n", i ))
        for ( i = 0, tip = modules ; tip ; tip = tip->next ) i++;
        DBG0 (( "modules:     %d \n", i ))
        for ( i = 0, tip = includes ; tip ; tip = tip->next ) i++;
        DBG0 (( "includes:    %d \n", i ))
        for ( i = 0, tip = files ; tip ; tip = tip->next ) i++;
        DBG0 (( "files:       %d \n", i ))
        for ( i = 0, tip = command_list ; tip ; tip = tip->next ) i++;
        DBG0 (( "commands:    %d \n", i ))
        for ( i = 0, tip = extra_list ; tip ; tip = tip->next ) i++;
        DBG0 (( "extras:      %d \n", i ))
		*/
    }

	(void) unlinkPIDFile(1);

    if ( errcnt )
    {
        DBG8 (( "target make errors %d \n", errcnt ))
        return ( 2 );
    }

	if (makerc > 1 )
		rc=1;


    DBG4 (( "vmake: returning %d \n", rc ))
    return ( rc );
}

static int print_statistics
(
	struct trg_info *stat_tip,
	char *tipname
)
{
	int i;
	int errcnt = 0;
	int mdcnt = 0;
	int finishedcnt = 0;
	char info_string[FILENAMESIZE];
	char tmp[FILENAMESIZE]; 
	struct trg_info *tip;

	for ( i = 0, tip = stat_tip ; tip ; tip = tip->next ) 
	{
		i++;
		if (tip->made < 0 && ! tip->stat.deperror)
			errcnt++;
		if (tip->made > 1)
			mdcnt++;
	}

	strcpy (tmp, tipname);
	strcat (tmp, ":");
	sprintf (info_string, "%-12.12s%5.1d", tmp, i);
	if (env_VMAKE_PRINT_STATISTICS)
	{
		sprintf (tmp, "            made:%5.1d", mdcnt);
		strcat (info_string, tmp);
		sprintf (tmp, "            errors:%5.1d", errcnt);
		strcat (info_string, tmp);
	}
	
	DBG0 (( "%s\n", info_string ))

	return (i);
}

/*==========================================================================*/

static  int     get_options 
(
    int     argc ,
    char    ** argv ,
    int     * argn 
)
{
    int                     rc ;
	char                    * sign;  /* CTS 1104286 */
    char                    * p ;


    DBG2 (( "getopt: called \n" ))

    rc = 0 ;

    for ( * argn = 1 ; * argn < argc ; ( * argn ) ++ )
    {
        p = argv [ * argn ] ;
		/* CTS 1104286     vvvvvvvvvvvvvvvvv */
		if (( * p != '-' ) && ( * p != '+' ) ) break ;
		sign = p; /* CTS 1104286 */

        for ( p ++ ; * p != '\0' ; p ++ )
        {
			/* CTS 1104286 -> */
			if ( *sign == '+' )
			{
				switch ( * p )
				{
				case 'e':
					if ( opt_debug != 0 )
					{
						DBG0 (( "WARNING: debug option already set. Switch debug off.\n" ))  
					}
					DBG7 (( "opt_debug -1 \n" ))
					opt_debug = -1 ;
					break ;

				case 'c':
					opt_collection = 2 ;
					break ;

				case 'C':
					opt_check_for_running_vmake = 1 ;
					break ;


				default :
					DBG0 (( "illegal option '%c' with +\n", * p ))
					rc ++ ;
					break ;
				}
			}
			/* <- CTS 1104286 */
			else
			{

				switch ( * p )
				{
				case 'v' :
					DBG0 (( "%s\n", vmake_version ))
					exit ( 0 );
				case '?' :
					rc ++ ;
					break ;
				case 'C' :
					DBG7 (( "opt_nodebug_col \n" ))
					opt_nodebug_col = 1 ;
					break ;
				case 'E' :
					/*
					CTS 1110663
					DBG7 (( "opt_errorinc \n" ))
					opt_errorinc = 1 ;
					*/
					DBG7 (( "opt_optdebug\n" ));
					opt_optdebug = 1;
					break ;
				case 'F' :
					DBG7 (( "opt_distrib \n" ))
					opt_distrib = 1 ;
					break ;

				/* CTS DEPS */
				case 'H' :
					DBG7 (( "opt_showdeps \n" ))
					opt_showdeps = 1;
					break ;

				case 'I' :
					DBG7 (( "opt_uncondincl \n" ))
					opt_uncondincl = 1 ;
					break ;
				case 'K' :
					opt_keep |= 2 ;
					DBG7 (( "opt_keep = %d \n", opt_keep ))
					break ;
				case 'L' :
					opt_lint = 1 ;
					DBG7 (( "opt_lint \n" ))
					break ;
				case 'M' :
					DBG7 (( "opt_man \n" ))
					opt_man = 1 ;
					break ;
				case 'N' :
					DBG7 (( "opt_noprot \n" ))
					opt_noexec = 1 ;
					opt_noprot = 1 ;
					opt_interactive = 1 ;
					break ;
				case 'S' :
					opt_sizeof = 1 ;
					DBG7 (( "opt_sizeof \n" ))
					break ;
				case 'R' : 
					opt_require = 1 ;
					DBG7 (( "opt_require \n" ))
					break ;
				case 'U' :
					DBG7 (( "opt_uncond \n" ))
					opt_uncond = 1 ;
					break ;
				case 'Y' :
					yydebug = 1 ;
					break ;

				case 'b' :
					DBG7 (( "opt_background \n" ))
					opt_background = 1 ;
					break ;
				case 'c' :
					DBG7 (( "opt_collection \n" ))
					opt_collection = 1 ;
					break ;
				case 'd' :
					DBG7 (( "opt_nodep \n" ))
					opt_nodep = 1 ;
					break ;
				case 'e' :
					if ( opt_debug != 0 )
					{
						DBG0 (( "WARNING: debug option already set. Switch debug on.\n" ))  
					}
					DBG7 (( "opt_debug \n" ))
					opt_debug = 1 ;
					break ;
				case 'g' :
					DBG7 (( "opt_global \n" ))
					opt_global = 1 ;
					break ;

				/* CTS DEPS */
				case 'h' :
					DBG7 (( "opt_countdeps \n" ))
					opt_countdeps = 1;
					break ;

				case 'i' :
					DBG7 (( "opt_interactive \n" ))
					opt_interactive = 1 ;
					break ;
				case 'k' :
					opt_keep |= 1 ;
					DBG7 (( "opt_keep = %d \n", opt_keep ))
					break ;
				case 'l' :
					DBG7 (( "opt_local \n" ))
					opt_local = 1 ;
					break ;
				case 'm' : 
					DBG7 (( "opt_modlist \n" ))
					opt_modlist = 1 ;
					break ;
				case 'n' :
					DBG7 (( "opt_noexec \n" ))
					opt_noexec = 1 ;
					break ;
				case 'p' :
					DBG7 (( "opt_profile \n" ))
					opt_profile = 1 ;
					break ;
				case 'r' :
					DBG7 (( "opt_retrieve \n" ))
					opt_retrieve = 1 ;
					break ;
				case 's' :
					DBG7 (( "opt_nolink \n" ))
					opt_nolink = 1 ;
					break ;
				case 'u' :
					DBG7 (( "opt_nameduncond \n" ))
					opt_nameduncond = 1 ;
					break ;
				case 'w' :
					DBG7 (( "opt_watch \n" ))
					opt_watch = 1 ;
					break ;
				case 'x' :
					DBG7 (( "opt_print \n" ))
					opt_print = 1 ;
					break ;
				case 'z' :
					DBG7 (( "vopt_break_after_first_error \n" ))
					vopt_break_after_first_error = 1 ;
					break ;
				case 'D' :
					if ( p [ 1 ] >= '0' && p [ 1 ] <= '9' )
					{
						extern int dbgdbglvl;
						dbgdbglvl = p [ 1 ] - '0' ;
						p ++ ;
						DBG0 (( "debug level %d \n", dbgdbglvl ))
						/* select one level higher for xprot */
						dbgdbglvl ++ ;
						break ;
					}
					/*FALLTHROUGH*/
				default :
					DBG0 (( "illegal option '%c' \n", * p ))
					rc ++ ;
					break ;
				}
			}
        }
    }

    DBG4 (( "getopt: returning %d \n", rc ))
    return ( rc );
}

/*==========================================================================*/

static  int     get_environment 
(
)
{
    int     rc ;
    char    * ptr ;
    char *ctmp = NULL;
	char VersionString[]=VMAKE_VERSION; /* CTS 1105925 */

    DBG2 (( "getenv: called \n" ))

    rc = 0 ;

    ptr = getenv ("RELVER");
    if ( ptr )
    {
        env_RELVER = ptr ;
        DBG7 (( "env_RELVER '%s' \n", env_RELVER ))
    }
    
    ptr = getenv ("CORRECTION_LEVEL");
    if ( ptr )
    {
        env_CORRECTION_LEVEL = ptr ;
        DBG7 (( "env_CORRECTION_LEVEL '%s' \n", env_CORRECTION_LEVEL ))
    }

	ptr = getenv ("BUILDPRAEFIX");
    if ( ptr )
    {
        env_BUILDPRAEFIX = ptr ;
        DBG7 (( "env_BUILDPRAEFIX '%s' \n", env_BUILDPRAEFIX ))
    }


	ptr = getenv ("OSSPEC");
    if ( ptr )
    {
        env_OSSPEC = ptr ;
        DBG7 (( "env_OSSPEC '%s' \n", env_OSSPEC ))
    }

	ptr = getenv ("MACH");
    if ( ptr )
    {
        env_MACH = ptr ;
        DBG7 (( "env_MACH '%s' \n", env_MACH ))
    }

	ptr = getenv ("BIT64");
    if ( ptr )
    {
        env_BIT64 = ptr ;
        DBG7 (( "env_BIT64 '%s' \n", env_BIT64 ))
    }

    ptr = getenv ( "VMAKE_VERSION" );
    if ( ptr )
    {
        env_VMAKE_VERSION = ptr ;
        DBG7 (( "env_VMAKE_VERSION '%s' \n", env_VMAKE_VERSION ))
    }

    ptr = getenv ( "VMAKE_DEFAULT" );
    if ( ptr )
    {
        env_VMAKE_DEFAULT = ptr ;
        DBG7 (( "env_VMAKE_DEFAULT '%s' \n", env_VMAKE_DEFAULT ))
    }

    ptr = getenv ( "VMAKE_OPTION" );
    if ( ptr )
    {
        env_VMAKE_OPTION = ptr ;
        DBG7 (( "env_VMAKE_OPTION '%s' \n", env_VMAKE_OPTION ))
    }

    ptr = getenv ( "VMAKE_PATH" );
    if ( ptr )
    {
        env_VMAKE_PATH = ptr ;
        DBG7 (( "env_VMAKE_PATH '%s' \n", env_VMAKE_PATH ))
    }

    ptr = getenv ( "INSTROOT" );
    if ( ptr )
    {
        env_INSTROOT = ptr ;
        DBG7 (( "env_INSTROOT '%s' \n", env_INSTROOT ))
    }

    /*
     *  IST 1997-04-17, On request of Gert, TOOLSHELL will be used
     *  for the tools called by vmake, while SHELL will be used
     *  for commands within macro descriptions.
     *  If TOOLSHELL is not set, the value of SHELL will be used.
     */
    ptr = getenv ( "SHELL" );
    if ( ptr )
    {
        env_SHELL = ptr ;
        DBG7 (( "env_SHELL '%s' \n", env_SHELL ))
    }
    else    env_SHELL = CMD_SHELL ;

    ptr = getenv ( "TOOLSHELL" );
    if ( ptr )
    {
        env_TOOLSHELL = ptr ;
        DBG7 (( "env_TOOLSHELL '%s' \n", env_TOOLSHELL ))
    }
    else    env_TOOLSHELL = env_SHELL ;

    ptr = getenv ( "TOOL" );
    if ( ptr )
    {
        env_TOOL = ptr ;
        DBG7 (( "env_TOOL '%s' \n", env_TOOL ))
    }

    ptr = getenv ( "TOOLEXT" );
    if ( ptr )
    {
        env_TOOLEXT = ptr ;
        DBG7 (( "env_TOOLEXT '%s' \n", env_TOOLEXT ))
    }

    ptr = getenv ( "TOOLOPT" );
    if ( ptr )
    {
        env_TOOLOPT = ptr ;
        DBG7 (( "env_TOOLOPT '%s' \n", env_TOOLOPT ))
    } else env_TOOLOPT = NULL;

    ptr = getenv ( "DLL_TYPE" );
    if ( ptr )
    {
        env_DLL_TYPE = ptr ;
        DBG7 (( "env_DLL_TYPE '%s' \n", env_DLL_TYPE ))
    }

    ptr = getenv ( "ARC_TYPE" );
    if ( ptr )
    {
        env_ARC_TYPE = ptr ;
        DBG7 (( "env_ARC_TYPE '%s' \n", env_ARC_TYPE ))
    }

	ptr = getenv ( "BUILDSTRING" ); /* CTS 1102333 */
    if ( ptr )
    {
        env_BUILDSTRING = ptr ;
        DBG7 (( "env_BUILDSTRING '%s' \n", env_BUILDSTRING ))
    }

    /* CTS 1103482 */
	ptr = getenv ( "INSTLIB" );
	if ( ptr )
    {
        env_INSTLIB = ptr ;
        DBG7 (( "env_INSTLIB '%s' \n", env_INSTLIB ))
    }
	
	/* CTS 1105503 */
	ptr = getenv ( "OWN" );
    if ( ptr )
    {
        env_OWN = ptr ;
        DBG7 (( "env_OWN '%s' \n", env_OWN ))
    }
	/* <- CTS 1105503 */

	/* CTS 1111841 -> */
	ptr = getenv ( "BUILDPID" );
	if ( ptr )    
	{        
		env_BUILDPID = ptr ;        
		DBG7 (( "env_BUILDPID '%s' \n", env_BUILDPID ))    
	}
	else
	{
		env_BUILDPID=NULL;
	}
	/* <- CTS 1111841 */

	/* CTS 1112496 */
	ptr = getenv ( "CMD_OUTPUT_LENGTH" );
	if ( ptr )    
	{        
		env_CMD_OUTPUT_LENGTH = atoi (ptr) ;        
		DBG7 (( "env_CMD_OUTPUT_LENGTH '%d' \n", env_CMD_OUTPUT_LENGTH ))    
	}
	else
	{
		env_CMD_OUTPUT_LENGTH = MAXLINELENGTH;
	}
	/* <- CTS 1112496 */

	ptr = getenv ( "SUPPRESS_CONCOM" );
	if ( ptr )    
	{        
		suppress_all_concom = 1 ;        
		DBG1 (( "INFO: Conditional Compiling is switched off\n" ))    
	}

	ptr = getenv ( "VMAKE_GLOBALS" );
	if ( ptr )    
	{        
		env_VMAKE_GLOBALS = ptr ;        
		DBG7 (( "env_VMAKE_GLOBALS '%s' \n", env_VMAKE_GLOBALS ))    
	}

	ptr = getenv ( "VMAKE_DEFAULT_VARIANT" );
	if ( ptr )    
	{        
		env_VMAKE_DEFAULT_VARIANT = ptr ;        
		DBG7 (( "env_VMAKE_DEFAULT_VARIANT '%s' \n", env_VMAKE_DEFAULT_VARIANT ))    
	}

	/* CTS 1105925 -> */
	/* vmake_version => beetween the first and the second " " */
	ptr = strchr(VersionString, ' '); /* search for first space */
	if ( ptr == NULL || ptr++ ==NULL )
	{
		DBG0 (( "ERROR: ivalid vmake version string \n" ))
		rc++;
	}
	else
	{
		ctmp = strchr(ptr, ' ');
		if ( ptr == NULL )
		{
			DBG0 (( "ERROR: ivalid vmake version string \n" ))
			rc++;
		}
		else
		{
			*ctmp = '\0';
			ctmp = (char*) palloc( 14 + (ctmp - ptr) +1 );
			if ( ctmp == NULL )
			{
				DBG0 (( "ERROR: insufficient dynamic memory \n" ))
				rc++;
			}
			else
			{
				(void) sprintf( ctmp, "VMAKE_RELEASE=%s", ptr  );
				if ( putenv( ctmp ) != 0 )
				{
					rc++;
					DBG0(( "make: error propagating VMAKE_RELEASE to environment \n" ))
				}
			}
		}
	}
	/* <- CTS 1105925 */

	ptr = getenv ( "SUPPRESS_VMAKE_PIDFILE" );
	if ( ptr )    
	{        
		env_SUPPRESS_VMAKE_PIDFILE = 1;       
		DBG7 (( "env_SUPPRESS_VMAKE_PIDFILE '%s' \n", env_SUPPRESS_VMAKE_PIDFILE ))    
	}
	else
	{
		env_SUPPRESS_VMAKE_PIDFILE = 0;        
		DBG7 (( "env_SUPPRESS_VMAKE_PIDFILE '%d' \n", env_SUPPRESS_VMAKE_PIDFILE ))    
	}

	ptr = getenv ( "VMAKE_SILENT_MODE" );
	if ( ptr )    
	{
		ptr = getenv ( "VMAKE_SILENT_STRING" );
		if ( ptr )    
		{
			if (strcmp(ptr, "ROTATE") == 0)
			{
				opt_silent_mode = VMAKE_SILENT_MODE_ROTATE;
				silent_char_string = silent_char_string_rotate;
			}
			else
				if (strcmp(ptr, "NOTHING") == 0)
					opt_silent_mode = VMAKE_SILENT_MODE_NOTHING;
				else
				{
					silent_char_string = ptr;						
					opt_silent_mode = VMAKE_SILENT_MODE_FREE;
				}
		}
		else
		{
			opt_silent_mode = VMAKE_SILENT_MODE_DOT;
			silent_char_string = silent_char_string_dot;
		}

		/* opt_silent_mode = 1;       */
		current_silent_char_count = 0;
	}
	else
		opt_silent_mode = 0;       
	/*
	opt_silent_mode = VMAKE_SILENT_MODE_DOT;
	silent_char_string = silent_char_string_dot;
	*/

	DBG7 (( "opt_silent_mode=%d \n", opt_silent_mode ))    

	ptr = getenv ( "VMAKE_PRINT_STATISTICS" );
	if ( ptr )    
	{        
		env_VMAKE_PRINT_STATISTICS = 1;       
		DBG7 (( "env_VMAKE_PRINT_STATISTICS '%d' \n", env_VMAKE_PRINT_STATISTICS ))    
	}
	else
	{
		env_VMAKE_PRINT_STATISTICS = 0;        
		DBG7 (( "env_VMAKE_PRINT_STATISTICS '%d' \n", env_VMAKE_PRINT_STATISTICS ))    
	}

	ptr = getenv ( "VMAKE_NO_DIRCACHE" );

	if ( ptr )    
		env_VMAKE_NO_DIRCACHE = 1;       
	else
		env_VMAKE_NO_DIRCACHE = 0;

	DBG7 (( "env_VMAKE_NO_DIRCACHE '%s' \n", env_VMAKE_NO_DIRCACHE ))    

		
	ptr = getenv ( "VMAKE_WARNING_PROTOCOL" );

	if ( ptr )    
		opt_warning_protocol = 1;       
	else
		opt_warning_protocol = 0;

	DBG7 (( "opt_warning_protocol '%d' \n", opt_warning_protocol ))    


	ptr = getenv ( "VMAKE_REPORT_PERCENT" );
	if ( ptr )    
	{
		env_VMAKE_REPORT_PERCENT = atoi (ptr) ;        
		DBG7 (( "env_VMAKE_REPORT_PERCENT '%d' \n", env_VMAKE_REPORT_PERCENT ))    
	}
	else
	{
		env_VMAKE_REPORT_PERCENT = 0;
	}
	/* env_VMAKE_REPORT_PERCENT = 5; */
	next_report_percent = env_VMAKE_REPORT_PERCENT;


	ptr = getenv ( "VMAKE_REPORT_TIME" );
	if ( ptr )    
	{
		env_VMAKE_REPORT_TIME = atoi (ptr) ;        
		DBG7 (( "env_VMAKE_REPORT_TIME '%d' \n", env_VMAKE_REPORT_TIME ))    
	}
	else
	{
		env_VMAKE_REPORT_TIME = 0;
	}

	ptr = getenv ( "VMAKE_TEST_READDIR" );
	if ( ptr )    
		env_VMAKE_TEST_READDIR = ptr;        
	else
        env_VMAKE_TEST_READDIR = NULL;

	
    DBG4 (( "getenv: returning %d \n", rc ))
    return ( rc );
}

/*==========================================================================*/

static  int     analyze_env_opt
(
)
{
    int                     ec ;
    char                    * ptr ;


    DBG2 (( "anaopt: called \n" ))

    ec = 0 ;

    if ( env_VMAKE_VERSION )
    {
        /* use the first character only */
        vmakeversion = * env_VMAKE_VERSION ;
    }
    else
    {
        DBG0 (( "unspecified VMAKE_VERSION \n" ))
        ec ++ ;
    }
    switch ( vmakeversion )
    {
    case 'f' :
    case 'q' :
    case 's' :
        DBG7 (( "anaopt: vmakeversion '%c' \n", vmakeversion ))
        break ;
    default :
        DBG0 (( "unknown VMAKE_VERSION '%c' \n",
            ((vmakeversion < ' ') || (vmakeversion > '~')) ?
                        '.' : vmakeversion ))
        ec ++ ;
    }

    if ( ! env_INSTROOT )
    {
        DBG0 (( "please specify INSTROOT \n" ))
        ec ++ ;
    }
    else
        if ( ! env_INSTLIB )
        {
            char    * ctmp;

		    ctmp = (char*) palloc( 13 + strlen( env_INSTROOT ));
		    if ( ctmp == NULL )
		    {
			    DBG0 (( "ERROR: insufficient dynamic memory \n" ))
			    ec++;
			} 
		    else
		    {
			    (void) sprintf( ctmp, "INSTLIB=%s/lib", env_INSTROOT  );
			    if ( putenv( ctmp ) == 0 )
			    {
				    ptr = getenv ( "INSTLIB" );
				    if (ptr)
				    {
					    env_INSTLIB = ptr ;
					    DBG7 (( "env_INSTLIB '%s' \n", env_INSTLIB ))
				    }
				    else
				    {
					    DBG0 (( "ERROR: INSTLIB cannot set ! \n" ));
					    ec++;
				    }
			    }
			    else
			    {
				    ec++;
				    DBG0(( "make: error propagating INSTLIB to environment \n" ))
			    }
            }
	    }

	if ( ! env_DLL_TYPE )
	{
        env_ARC_TYPE = (char *) palloc (strlen("dll") + 1 );
		strcpy(env_ARC_TYPE, "dll");
	}

	if ( ! env_ARC_TYPE )
    {
#if unix
        env_ARC_TYPE = (char *) palloc (strlen("a") + 1 );
		strcpy(env_ARC_TYPE, "a");
#else
		env_ARC_TYPE = (char *) palloc (strlen("lib") + 1 );
		strcpy(env_ARC_TYPE, "lib");
#endif
    }

	if ( ! env_VMAKE_GLOBALS )
	{
		env_VMAKE_GLOBALS = (char *) palloc (strlen(DEFAULT_GLOBALS) + 1 );
		strcpy(env_VMAKE_GLOBALS, DEFAULT_GLOBALS);
	}
	else
	{
		ptr = get_extension(env_VMAKE_GLOBALS);
		if ( ! ptr || ( strcmp (ptr, COM_LST) != 0) )
		{
			ptr = (char *) palloc (strlen(env_VMAKE_GLOBALS) + strlen(COM_LST) + 2);
			strcpy (ptr, env_VMAKE_GLOBALS );
			strcat (ptr, ".");
			strcat (ptr, COM_LST );
		}
	}

    if ( opt_interactive + opt_watch + opt_background > 1 )
    {
        DBG0 (( "please specify only one of -i -w -b \n" ))
        ec ++ ;
    }

    if ( ! opt_uncond ) opt_uncondincl = 0 ;

    if ( ! env_VMAKE_OPTION ) env_VMAKE_OPTION = EMPTY ;
    for ( ptr = env_VMAKE_OPTION ; * ptr != '\0' ; ptr ++ )
    {
        switch ( * ptr )
        {
        case 'a' :
				vopt_no_case_sens_error = 1;
		case 'A' :
                vopt_local_objcopy = 1 ;
                break ;
		case 'b' :
                vopt_c_includes_p = 1 ;
                break ;
        case 'c' :
                vopt_level_c = 1 ;
                break ;
        case 'C' :
                vopt_level_c = 2 ;
                break ;
        case 'D' :
                if ( vopt_local )
                {
                    DBG1(( "local make: we don't use dates files!\n" ))
                }
                else
                    vopt_dates = 1 ;
                break ;

		case 'd' :  vopt_current_date = 1;  /* CTS 1110368 */
					break;
        case 'e' :  opt_debug ++ ; /* opt_debug = 1 ; CTS 1104286 */
                    break ;
		case 'E' :  opt_optdebug = 1; /* CTS 1110663 */
					break;

		/* CTS 1112006 */ 
		case 'f' :  vopt_fulloutput =1;  /* write to errorfile if messages found */
					break;

        case 'g' :
                vopt_global = 1 ;
                break ;
        case 'h' :
                vopt_prefer_shm = 1 ;
                break ;
		case 'H' :
                vopt_no_auto_hdr = 1 ;
                break ;
        case 'i' :
                vopt_level_i = 1 ;
                break ;
        case 'I' :
                vopt_level_i = 2 ;
                break ;
		case 'j' :
				vopt_desc_in_src = 1;
		case 'K' :
				opt_keep |= 2 ;
				break ;
		case 'k' :
				opt_keep |= 1 ;
				break ;
		case 'L' :
                vopt_reloc_lib = 1 ;
                vopt_mod_only = 1 ; /* implicitly set */
                break ;
        case 'l' :
                vopt_local = 1 ;
                vopt_dates = 0 ;
                DBG1(( "local make: disable dates files!\n" ))
                break ;
		case 'n' :
				vopt_new_variant_handling = 1;
				break;
		case 'M' :
                vopt_mod_only = 1 ;
                break ;
		case 'o' :
                vopt_level_o = 1 ;
                break ;
        case 'O' :
                vopt_level_o = 2 ;
                break ;
        case 'p' :
                vopt_level_p = 1 ;
                break ;
        case 'P' :
                vopt_level_p = 2 ;
                break ;
        case 'R' :
                vopt_ignore_shrglob = 1 ;
                break ;
        case 's' :
                vopt_level_s = 1 ;
                break ;
        case 'S' :
                vopt_level_s = 2 ;
                break ;
        case 't' :
                vopt_timestamp = 1 ;
                break ;
        case 'T' :
                vopt_timestamp = 2 ;
                break ;
		case 'u' :
				vopt_make_in_objdir = 1;
				break;
        case 'v' :
                vopt_source_warn = 1 ;
                break ;
        case 'V' :
                vopt_source_copy = 1 ;
                break ;
        case 'w' :
                vopt_object_warn = 1 ;
                break ;
        case 'W' :
                vopt_object_copy = 1 ;
                break ;
		/* CTS XML */
		case 'x' :
				vopt_xml_prot = 1;
				break;
        /* CTS 1103400 */
		case 'X' :  vopt_rw_permission = 1 ;
                    break ;
		case 'Z' :
                vopt_no_ext_map = 1 ;
                break ;
		
		case 'z' :
                vopt_break_after_first_error = 1 ;
                break ;


        default :
            DBG0 (( "illegal character in VMAKE_OPTION '%c' \n",
                                    * ptr ))
            ec ++ ;
        }
    }
    if ( vopt_level_s == 0 && vopt_level_p == 0 &&
         vopt_level_c == 0 && vopt_level_o == 0    )
    {
        vopt_level_s = 2 ;
        vopt_level_o = 2 ;
    }
    if ( vopt_level_s == 0 && vopt_level_o == 0 )
    {
        DBG0 (( "senseless vmake option '%s' \n", env_VMAKE_OPTION ))
        ec ++ ;
    }
    DBG7 (( "anaopt: level_i %d \n", vopt_level_i ))
    DBG7 (( "anaopt: level_s %d \n", vopt_level_s ))
    DBG7 (( "anaopt: level_p %d \n", vopt_level_p ))
    DBG7 (( "anaopt: level_c %d \n", vopt_level_c ))
    DBG7 (( "anaopt: level_o %d \n", vopt_level_o ))

    if ( vopt_local && vopt_global )
    {
        DBG0 (( "specify only one of 'l'(local) or 'g'(global) " ))
        DBG0 (( "in VMAKE_OPTION \n" ))
        ec ++ ;
    }
    else
    {
        if ( ! vopt_local && ! vopt_global ) vopt_global = 1 ; /* default */
    }
    if ( opt_local && opt_global )
    {
        DBG0 (( "specify only one of '-l'(local) or '-g'(global) " ))
        DBG0 (( "in command line \n" ))
        ec ++ ;
    }
    else
    {
        if ( opt_local )
        {
            vopt_local = 1 ;
            vopt_global = 0 ;
            vopt_dates = 0 ;
            DBG1(( "local make: disable dates files!\n" ))
        }
        if ( opt_global )
        {
            vopt_local = 0 ;
            vopt_global = 1 ;
        }
    }
    if ( vopt_c_includes_p && vopt_no_auto_hdr )
    {
        DBG0 (( "specify only one of 'b'(C use P hdr) or " ))
        DBG0 (( "'H'(no P->C hdr) in VMAKE_OPTION \n" ))
        ec ++ ;
    }

	/* CTS 1104286 */
	/* opt_y <=0 -> 0 ; opt_debug >=1 -> 1 */
	opt_debug = opt_debug <= 0 ? 0: 1;


    DBG4 (( "anaopt: returning %d \n", ec ))
    return ( ec );
}

/*==========================================================================*/

static  int     get_vmake_path 
(
)
{
    int                     rc=0 ;
    int                     lgt ;
    struct path_info        * pip ;
    char                    * ptr ;
    char                    path [ FILENAMESIZE ] ;


    DBG2 (( "get_vmake_path: called \n" ))

    ptr = env_VMAKE_PATH ;
    for ( ; ptr && * ptr ; )
    {
        for ( lgt = 0 ; lgt < sizeof(path) ; lgt ++ )
        {
            if ( * ptr == PATH_DELIMITER || * ptr == '\0' ) break ;
            path [ lgt ] = * ptr ;
            ptr ++ ;
        }
        if ( lgt >= sizeof(path) )
        {
            DBG0 (( "path name too long in VMAKE_PATH: '%s' \n", ptr ))
            return ( -2 );
        }
        path [ lgt ] = '\0' ;
        if ( lgt == 0 )
        {
            if ( * ptr != '\0' ) ptr ++ ; /* skip delimiter */
            continue ;
        }
        rc = append_path ( path , & vmake_path , & pip );
        if ( rc < 0 )
        {
            DBG8 (( "get_vmake_path: append_path error %d \n", rc ))
            return ( rc );
        }
        else    DBG8 (( "get_vmake_path: appended path '%s' \n", path ))
    }

    if ( vmake_path )
    {
        for ( pip = vmake_path ; pip != NULL ; pip = pip->next )
            DBG7 (( "get_vmake_path: vmake_path '%s' \n", pip->path ))

		if ( strcmp (env_OWN, vmake_path->path) != 0)
		{
			DBG0 (( "Error: Differences between OWN (%s) and first VMAKE_PATH (%s)\n\n",
				env_OWN, vmake_path->path ))
			return( -2 );
		}

        return ( 0 );
    }

    DBG0 (( "please specify VMAKE_PATH \n" ))

    DBG4 (( "get_vmake_path: returning -2 \n" ))
    return ( -2 );
}

/*==========================================================================*/

static  int create_directories
(
)
{
    int         rc ;
    char            * own ;

    DBG2(( "create_directories: called \n" ))

    if ( vmake_path == NULL )
    {
        DBG0 (( "vmake_path is NULL !!! \n" ))
        return ( 2 );
    }
    own = vmake_path->path ;

    rc = create_dir ( DIR_DBIN , env_INSTROOT , EMPTY );
    if ( rc ) return ( rc ); 
    rc = create_dir ( DIR_DPGM , env_INSTROOT , EMPTY );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_DLIB , env_INSTROOT , EMPTY );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_DINC , env_INSTROOT , EMPTY );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_DCONF, env_INSTROOT , EMPTY );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_DENV , env_INSTROOT , EMPTY );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_DINC , env_INSTROOT , EMPTY );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_DLIB , env_INSTROOT , EMPTY );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_DWRK , env_INSTROOT , EMPTY );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_DDEMO, env_INSTROOT , EMPTY );
    if ( rc ) return ( rc ); 

	/* &gar  PTS 1102197 */
	rc = create_dir ( DIR_OWNTMP  , own , EMPTY );
    if ( rc ) return ( rc );
    
    rc = create_dir ( DIR_INC  , own , EMPTY );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_SIZE , own , EMPTY );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_PROT , own , EMPTY );
    if ( rc ) return ( rc );
    if ( vopt_dates )
    {
        rc = create_dir ( DIR_DATES , own , EMPTY );
        if ( rc ) return ( rc );
    }

    rc = create_dir ( DIR_TMP  , own , "fast" );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_OBJ  , own , "fast" );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_PROT , own , "fast" );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_GSIZ , own , "fast" );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_PSRC , own , "fast" );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_CSRC , own , "fast" );
	if ( rc ) return ( rc );
    if ( vopt_dates )
    {
        rc = create_dir ( DIR_DATES , own , "fast" );
        if ( rc ) return ( rc );
    }

    rc = create_dir ( DIR_TMP  , own , "quick" );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_OBJ  , own , "quick" );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_PROT , own , "quick" );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_GSIZ , own , "quick" );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_PSRC , own , "quick" );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_CSRC , own , "quick" );
	if ( rc ) return ( rc );
    if ( vopt_dates )
    {
        rc = create_dir ( DIR_DATES , own , "quick" );
        if ( rc ) return ( rc );
    }

    rc = create_dir ( DIR_TMP  , own , "slow" );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_OBJ  , own , "slow" );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_PROT , own , "slow" );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_GSIZ , own , "slow" );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_PSRC , own , "slow" );
    if ( rc ) return ( rc );
    rc = create_dir ( DIR_CSRC , own , "slow" );
	if ( rc ) return ( rc );
    if ( vopt_dates )
    {
        rc = create_dir ( DIR_DATES , own , "slow" );
        if ( rc ) return ( rc );
    }

    return ( 0 );
}

/*==========================================================================*/

static  int create_dir 
(
    char    * template ,
    char    * base ,
    char    * version 
)
{
    int         rc ;
    char            path [ FILENAMESIZE ];

    DBG2(( "create_dir: called \n" ))

    (void) sprintf ( path , template , base , version , EMPTY );
    (void) strcat  ( path , "." ); /* create a file name */

    rc = check_dir ( path ); /* takes a file name (not a directory name) */

    return ( rc );
}

/*==========================================================================*/
#if unix

static  int     fork_vmake 
(
)
{
    int                     rc ;
    int                     status ;
    pid_t           sonpid ;
    int                     fd [ 2 ] ;
    char                    line [ LINESIZE ] ;


    DBG2 (( "fork_vmake: called \n" ))

    (void) signal ( SIGPIPE , SIG_IGN );

    rc = pipe ( fd );
    if ( rc < 0 )
    {
        DBG0 (( "cannot create pipe: %s \n", serrno() ))
        return ( -2 );
    }
    DBG8 (( "fork_vmake: pipe created \n" ))

    sonpid = fork ();
    if ( sonpid == (pid_t) -1 )
    {
        DBG0 (( "cannot fork: %s \n", serrno() ))
        return ( -2 );
    }
    DBG8 (( "fork_vmake: forked %ld \n", (long) sonpid ))

    if ( sonpid == 0 )  /* vmake process */
    {
#ifdef SETPGRP_VOID
        vmakepid = setpgrp ( );
#else
        vmakepid = getpid ();
        sonpid = setpgrp ( vmakepid , vmakepid );
        if ( sonpid )
        {
            DBG0 (( "cannot set process group: %s \n", serrno() ))
            return ( -2 );
        }
#endif
        DBG8 (( "fork_vmake: process group %ld \n", (long) vmakepid ))

        (void) close ( 0 );           /* close stdin */
        (void) open ( "/dev/null" , O_RDONLY , 0 ); /* reopen stdin */
        (void) close ( 1 );           /* close stdout */
        (void) dup ( fd [ 1 ] );      /* reopen stdout */
        (void) close ( 2 );           /* close stderr */
        (void) dup ( fd [ 1 ] );      /* reopen stderr */
        (void) close ( fd [ 0 ] );    /* don't read from pipe */
        (void) close ( fd [ 1 ] );    /* unused file descriptor */

        DBG2 (( "%s vmake: starting make phase \n",
                        ascdate(time((time_t*) 0)) ))

        DBG8 (( "fork_vmake: returning %ld \n", (long) vmakepid ))
        return ( vmakepid );
    }

    /* controller process */

    DBG8 (( "fork_vmake: controller continuing \n" ))

    close_connections ();

    if ( opt_background )
    {
        DBG8 (( "fork_vmake: controller exiting \n" ))
        exit_vmake ( 0, FALSE);
        DBG0 (( "exit returned !!! \n" ))
        return ( - 3 ); /* should never be executed !!! */
    }

    vmakepid = sonpid ;
    DBG8 (( "fork_vmake: controller vmake pid %ld \n", (long) vmakepid ))

    (void) signal ( SIGHUP  , catch_signal );
    (void) signal ( SIGINT  , catch_signal );
    (void) signal ( SIGQUIT , catch_signal );
    (void) signal ( SIGTRAP , catch_signal );
    (void) signal ( SIGTERM , catch_signal );
	
    (void) close ( fd [ 1 ] );            /* don't write to pipe */

	
    for ( ; ; )
    {
        rc = read ( fd [ 0 ] , line , sizeof(line) );
        if ( rc < 0 )
        {
            if ( geterrno() == EINTR ) continue ;
            DBG0 (( "read error: %s \n", serrno() ))
            break ;
        }
        if ( rc == 0 ) break ;
        (void) write ( 1 , line , (unsigned) rc );
    }

    (void) close ( fd [ 0 ] );

    rc = wait ( & status ); 
    if ( rc < 0 )
    {
        DBG0 (( "wait error: %s \n", serrno() ))
        return ( -2 );
    }
	
	if ( status == 0 )
    {
		exit_vmake ( 0, TRUE );
        DBG0 (( "exit returned !!! \n" ))
        return ( - 3 ); /* should never be executed !!! */
    }

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
            DBG0 (( "vmake died due to signal %d", status & 0x7F ))
            if ( status & 0x80 )
                DBG0 (( ", core dumped to tmp directory \n" ))
            else
                DBG0 (( "\n" ))
        }
        else
        {
            /* process stopped */
            rc = status >> 8 & 0xFF ;
            DBG0 (( "vmake stopped due to signal %d", rc ))
        }
    }

    DBG4 (( "vmake error %d \n", rc ))
    exit_vmake ( ( rc > 0 ) ? ( rc ) : ( - rc ), TRUE );
    DBG0 (( "exit returned !!! \n" ))
    return ( - 3 ); /* should never be executed !!! */
}

#endif /*unix*/
/*==========================================================================*/

static  int     append_path 
(
    char                    * path ,
    struct path_info        ** listpp ,
    struct path_info        ** pipp 
)
{
    int					rc = 0 ;
    int					lgt ;
    int					status ;
    char				* ptr ;
    struct path_info    ** spipp , * pip ;


    DBG2 (( "apdpth: called \n" ))

    /* search end of list */
    status = 0 ;
    for ( spipp = listpp ; (*spipp) != NULL ; spipp = & (*spipp)->next )
    {
        status ++ ;
    }
    DBG5 (( "apdpth: level %d \n", status ))

    pip = (struct path_info *) palloc ( sizeof(struct path_info) );
    if ( pip == NULL )
    {
        DBG0 (( "insufficient dynamic memory \n" ))
        return ( - 3 );
    }
    FILL ( pip , 0 , sizeof(struct path_info) );

    lgt = strlen ( path );

    /* search for a node separator */
    ptr = strchr ( path , NODE_SEPARATOR );
    if ( ptr && ((ptr - path) >= 2) ) /* treat '[A-Za-z]:' as drive name */
    {
        /* this is a remote path */
        pip->path_status = status | PATH_REMOTE ;
        DBG5 (( "apdpth: remote \n" ))

        /* without vmake_option 'W' don't use objects from a remote path */
        if ( ! vopt_object_copy )
        {
            pip->host_permission |= HOST_OBJECTS_DENIED ;
            DBG5 (( "apdpth: objects denied \n" ))
        }

        /* never use tools/libs from a remote path */
        pip->host_permission |= HOST_TOOLS_DENIED ;
        DBG5 (( "apdpth: tools denied \n" ))
    }
    else
    {
        /* this is a local path */
        if ( spipp == listpp )
			pip->path_status = status | PATH_OWN ;
        else
        if ( vopt_local_objcopy )
            pip->path_status = status | PATH_REMOTE ;
        else {
#ifdef WIN32        
            char szTestPath[ MAX_PATH ];

            (void) strcpy ( szTestPath, path );
            (void) strcat ( szTestPath, "\\" );

            if ( GetDriveType( szTestPath ) == DRIVE_REMOTE ) 
            {
                pip->path_status = status | PATH_REMOTE ;
                /* never use tools/libs from a remote path */
                pip->host_permission |= HOST_TOOLS_DENIED ;
                DBG5 (( "apdpth: tools denied \n" ))
            }
            else
#endif
                pip->path_status = status | PATH_LOCAL ;
        }
        DBG5 (( "apdpth: local \n" ))
    }

    /* objects (and tools) denied on path with '//' suffix */
    if ( ( path[ lgt - 1 ] == '/' ) && ( path [ lgt - 2 ] == '/' ) )
    {
        pip->host_permission |= HOST_OBJECTS_DENIED ;
        pip->host_permission |= HOST_TOOLS_DENIED ;
        DBG5 (( "apdpth: objects denied \n" ))
        DBG5 (( "apdpth: tools denied \n" ))
    }

	pip->path = (char *) palloc( strlen( path ) + 1 );
    (void) strcpy ( pip->path , path );

    /* append to chain */
    * spipp = pip ;
    * pipp  = * spipp ;

    rc = 1 ;

    DBG4 (( "apdpth: returning %d \n", rc ))
    return ( rc );
}

/*==========================================================================*/

static  void    reduce_mod_type 
(
)
{
    struct trg_info         * tip ;
    struct dep_info         * dip ;

    DBG2 (( "reduce_mod_type: called \n" ))

    /*
        With the 'no dependencies' option, modules must be named
        explicitely, if they are to rebuilt.
        Therefore, if they are found in libraries only,
        it is assumed, that their object file is not needed
        in the objects directory, but put into the libraries only.
    */

    for ( tip = modules ; tip != NULL ; tip = tip->next )
    {
        /* if it is already a library member, skip this module */
        if ( tip->type == TT_MEM ) continue ;
        /* if there is no caller, the object is required */
        if ( tip->callers == NULL ) continue ;
        /* preliminary change to TT_MEM */
        tip->type = TT_MEM ;
        /* scan the callers list for callers other than libraries */
        for ( dip = tip->callers ; dip != NULL ; dip = dip->next )
        {
            if ( dip->target->type != TT_LIB )
            {
                /* undo the preliminary change */
                tip->type = TT_MOD ;
                /* don't need to search for more callers */
                break ;
            }
        }
        if ( tip->type == TT_MEM )
        {
            DBG8 (( "reduce_mod_type: changed to TT_MEM '%s' \n",
                                tip->name ))
        }
    }

    DBG4 (( "reduce_mod_type: returning \n" ))
}

/*==========================================================================*/

static  void    lint_files 
(
)
{
    int                     rc ;
    int         count ;
    int         status_dummy ;
    time_t          date ;
    struct trg_info         * tip ;
    char            name [ FILENAMESIZE ];
    valid_bit_arr       vbits ;

    DBG2(( "lint_files: called \n" ))

    VALID_BIT_FILL ( vbits );
    rc = cmdpgm ( env_TOOLSHELL );
    if ( env_TOOLOPT ) rc |= cmdarg( 1, env_TOOLOPT);
    rc = cmdscript ( CMD_COMPLINT );
    count = 0 ;
    for ( tip = modules ; tip ; tip = tip->next )
    {
        char    * file = NULL;

        if ( tip->language != 'c' ) continue ;
        (void) sprintf ( name , "%s.ln" , tip->name );
        rc = get_file ( name , DIR_TMP , versname(tip->version) , EMPTY ,
            FK_OBJECT | FK_LEVEL , vbits ,
            &file , & date , & status_dummy, 0 );
        if ( (rc <= 0) && ! opt_noexec ) continue ;
        rc = cmdarg( 1,  file );
        FREE_AND_SET_TO_NULL(file);
        if ( rc ) break ;
        count ++ ;
    }
    if ( count ) 
	{
		rc = cmdexe ();
		/* CTS 1112006 */
		flush_outputlist (tip, rc, 0);
	}
}

/*==========================================================================*/

static  void    print_cache 
(
)
{
    struct com_info         * cip ;
    struct inc_info         * iip ;
    struct dsc_info         * sip ;
    struct dep_info         * dip ;
    struct extra_info       * eip ;
    struct lang_info        * lip ;

    voutwrite ( "\n" );
    print_dirs(dir_lists);

    for ( cip = com_lists ; cip != NULL ; cip = cip->next )
    {
        if ( cip->com_fast )
        {
            voutwrite ( "com list  '%s'  version 'f' \n", cip->layer );
            print_com_list ( cip->com_fast );
        }
        if ( cip->com_quick )
        {
            voutwrite ( "com list  '%s'  version 'q' \n", cip->layer );
            print_com_list ( cip->com_quick );
        }
        if ( cip->com_slow )
        {
            voutwrite ( "com list  '%s'  version 's' \n", cip->layer );
            print_com_list ( cip->com_slow );
        }
    }
    voutwrite ( "\n" );

    for ( eip = xtr_lists ; eip != NULL ; eip = eip->next )
    {
        voutwrite ( "extra '%s' layer '%s' dir '%s' \n",
            eip->name , eip->layer , eip->dir );
        for ( sip = eip->descriptions ; sip != NULL ; sip = sip->next )
        {
            voutwrite ( "               dsc  '%s'\t'%s' \n",
                    sip->descriptor , sip->value );
        }
        for ( iip = eip->includes ; iip != NULL ; iip = iip->next )
        {
            voutwrite ( "               inc  '%s' language '%c' \n",
                    iip->name , iip->language );
        }
        voutwrite ( "\n" );
    }
    voutwrite ( "\n" );

    for ( lip = lng_lists ; lip != NULL ; lip = lip->next )
    {
        voutwrite ( "language '%c' '%s' '%s' '.%s' '-%%%c' '%s' \n",
        lip->language , lip->unpackinc , lip->unpackmod ,
        lip->suffix , lip->optchar , lip->compiler );
    }
    voutwrite ( "\n" );

    voutwrite ( "make_list \n" );
    for ( dip = make_list ; dip != NULL ; dip = dip->next )
    {
        voutwrite ( "          '%s' \n", dip->target->name );
    }
    voutwrite ( "\n" );

    DBG4 (( "print_cache: returning \n" ))
}

/*==========================================================================*/

static  void    print_dirs 
(
    struct dir_info * dirp 
)
{
    if ( dirp != NULL )
    {
        voutwrite ( "directory '%s' \n", dirp->dirpath );
        print_direntry(dirp->entries);
        voutwrite ( "\n" );
        print_dirs ( dirp->left  );
        print_dirs ( dirp->right );
    }

}

/*==========================================================================*/

static  void    print_direntry 
(
    struct dir_entry    * direp
)
{

    if ( direp != NULL )
    {
        voutwrite ( "          '%s' \n", direp->name );
        print_direntry ( direp->left  );
        print_direntry ( direp->right );
    }
}

/*==========================================================================*/

static  void    print_com_list 
(
    struct mod_com_info * mcip
)
{
    struct inc_info         * iip ;
    struct dsc_info         * sip ;

    for ( ; mcip ; mcip = mcip->next )
    {
        voutwrite ( "          name '%s' \n", mcip->name );
        if ( mcip->special_target_flag != ' ' )
            voutwrite ( "               special_target_flag '%c' \n",
                            mcip->special_target_flag );
        if ( mcip->version != ' ' )
            voutwrite ( "               version '%c' \n", mcip->version );
        if ( mcip->versdef != ' ' )
            voutwrite ( "               versdef '%c' \n", mcip->versdef );
        if ( mcip->stat.level_i || mcip->stat.level_s || mcip->stat.level_p ||
         mcip->stat.level_c || mcip->stat.level_o )
        {
            voutwrite ( "               level_i %d \n", mcip->stat.level_i );
            voutwrite ( "               level_s %d \n", mcip->stat.level_s );
            voutwrite ( "               level_p %d \n", mcip->stat.level_p );
            voutwrite ( "               level_c %d \n", mcip->stat.level_c );
            voutwrite ( "               level_o %d \n", mcip->stat.level_o );
        }
        if ( mcip->stat.shrglob )
            if ( mcip->stat.shrglob == (char) 0xff )
                voutwrite ( "               noshrglob \n" );
            else
                voutwrite ( "               shrglob \n" );
        if ( mcip->stat.debug )
            voutwrite ( "               debug \n" );
        if ( mcip->stat.profile )
            voutwrite ( "               profile \n" );
        for ( sip = mcip->descriptions ; sip != NULL ; sip = sip->next )
        {
            voutwrite ( "               dsc  '%s'\t'%s' \n",
                    sip->descriptor , sip->value );
        }
        for ( iip = mcip->includes ; iip != NULL ; iip = iip->next )
        {
            voutwrite ( "               inc  '%s' language '%c' \n",
                    iip->name , iip->language );
        }
        voutwrite ( "\n" );
    }
}

/*==========================================================================*/

static  void    print_collection 
(
)
{
    struct trg_info         * tip ;

    voutwrite ( "\n" );

    for ( tip = macros ; tip != NULL ; tip = tip->next )
    {
        voutwrite ( "macro   \n" );
        print_target ( tip );
    }

    if ( macros )
        voutwrite ( "\n" );

    for ( tip = programs ; tip != NULL ; tip = tip->next )
    {
        voutwrite ( "program \n" );
        print_target ( tip );
    }

    if ( programs )
        voutwrite ( "\n" );

    for ( tip = dynlinklibs ; tip != NULL ; tip = tip->next )
    {
        voutwrite ( "relocatable \n" );
        print_target ( tip );
    }
    
    if ( dynlinklibs )
        voutwrite ( "\n" );

    for ( tip = relocs ; tip != NULL ; tip = tip->next )
    {
        voutwrite ( "relocatable \n" );
        print_target ( tip );
    }

    if ( relocs )
        voutwrite ( "\n" );

    for ( tip = libraries ; tip != NULL ; tip = tip->next )
    {
        voutwrite ( "library \n" );
        print_target ( tip );
    }

    if ( libraries )
        voutwrite ( "\n" );

    for ( tip = javaprojects ; tip != NULL ; tip = tip->next )
    {
        voutwrite ( "java project \n" );
        print_target ( tip );
    }

    if ( javaprojects )
        voutwrite ( "\n" );

    for ( tip = modules ; tip != NULL ; tip = tip->next )
    {
        voutwrite ( "module  \n" );
        print_target ( tip );
    }

    if ( modules )
        voutwrite ( "\n" );

    for ( tip = includes ; tip != NULL ; tip = tip->next )
    {
        voutwrite ( "include \n" );
        print_target ( tip );
    }

    if ( includes )
        voutwrite ( "\n" );

    for ( tip = files ; tip != NULL ; tip = tip->next )
    {
        voutwrite ( "file    \n" );
        print_target ( tip );
    }

    if ( files )
        voutwrite ( "\n" );

    for ( tip = command_list ; tip != NULL ; tip = tip->next )
    {
        voutwrite ( "command \n" );
        print_target ( tip );
    }

    if ( command_list )
        voutwrite ( "\n" );

    for ( tip = extra_list ; tip != NULL ; tip = tip->next )
    {
        voutwrite ( "extra \n" );
        print_target ( tip );
    }

    voutwrite ( "\n" );

    DBG4 (( "print_collection: returning \n" ))
}

/*==========================================================================*/

static  void    print_target
(
    struct trg_info * tip
)
{
    struct dep_info         * dip ;
    struct extdep_info      * eip ;
    struct dsc_info         * sip ;
    struct obj_info         * oip ;
    struct file_info        * fip ;
    char                    * p ;
    struct dem_info         * dmip ;

    voutwrite ( "        name            '%s' \n", tip->name );

    switch ( tip->type )
    {
    case TT_OPTION :
        p = "TT_OPTION" ;
        break ;
    case TT_FILE :
        p = "TT_FILE" ;
        break ;
    case TT_INC :
        p = "TT_INC" ;
        break ;
    case TT_MEM :
        p = "TT_MEM" ;
        break ;
    case TT_MOD :
        p = "TT_MOD" ;
        break ;
    case TT_LIB :
        p = "TT_LIB" ;
        break ;
    case TT_REL :
        p = "TT_REL" ;
        break ;
    case TT_SHR :
        p = "TT_SHR" ;
        break ;
    case TT_PGM :
        p = "TT_PGM" ;
        break ;
    case TT_SHM :
        p = "TT_SHM" ;
        break ;
    case TT_MAC :
        p = "TT_MAC" ;
        break ;
    case TT_PRJ :
        p = "TT_PRJ" ;
        break ;
    case TT_CMD :
        p = "TT_CMD" ;
        break ;
    case TT_EXTRA :
        p = "TT_EXTRA" ;
        break ;
    case TT_DLL :
        p = "TT_DLL";
        break;
    default :
        p = "ILLEGAL" ;
        break ;
    }
    voutwrite ( "        type            %s   \n", p );
    voutwrite ( "        version         '%c' \n", tip->version );
    voutwrite ( "        language        '%c' \n", tip->language );
	if (tip->localvariant || tip->globalvariants)
	{
		char *name_s = palloc(strlen(tip->name)+1);
		(void) get_basename( tip->name, name_s );
		voutwrite ( "        source          '%s' \n", name_s ); 
		if (tip->localvariant)
			voutwrite ( "        localvariant    '%s' \n", tip->localvariant ); 
		if (tip->globalvariants)
		{
			struct variant_info *tmp_globalvariants = tip->globalvariants;
			voutwrite ( "        globalvariants  '" );
			
			while (tmp_globalvariants)
			{
				voutwrite ( "'%s'", tmp_globalvariants->name);
				if (tmp_globalvariants->next)
                    voutwrite ( ", ");
				tmp_globalvariants = tmp_globalvariants->next;
			}
			voutwrite ( "\n ");
		}
	}
    if ( tip->layer )
        voutwrite ( "        layer           '%s' \n", tip->layer );
    else
        voutwrite ( "        layer           '%s' \n", "" );
    voutwrite ( "        level_i         %d   \n", tip->stat.level_i );
    voutwrite ( "        level_s         %d   \n", tip->stat.level_s );
    voutwrite ( "        level_p         %d   \n", tip->stat.level_p );
    voutwrite ( "        level_c         %d   \n", tip->stat.level_c );
    voutwrite ( "        level_o         %d   \n", tip->stat.level_o );
    voutwrite ( "        shrglob         %d   \n", tip->stat.shrglob );
    voutwrite ( "        debug           %d   \n", tip->stat.debug );
    voutwrite ( "        profile         %d   \n", tip->stat.profile );
    voutwrite ( "        uncond          %d   \n", tip->stat.uncond );
    voutwrite ( "        forcemake       %d   \n", tip->stat.forcemake );
    voutwrite ( "        binary          %d   \n", tip->stat.binary );
	voutwrite ( "        ascii           %d   \n", tip->stat.ascii ); /* CTS 1105829 */
    voutwrite ( "        definition      %d   \n", tip->stat.definition );
    voutwrite ( "        noobject        %d   \n", tip->stat.noobject );
    voutwrite ( "        noobjcopy       %d   \n", tip->stat.noobjcopy );
    switch ( tip->stat.deptype )
    {
        case DT_INCDEP :
            voutwrite ( "        deptype         %s   \n", "incdep" );
            break;
        case DT_OBJDEP :
            voutwrite ( "        deptype         %s   \n", "objdep" );
            break;
        case DT_OBJDEMDEP :
            voutwrite ( "        deptype         %s   \n", "objdemdep" );
            break;
        case DT_SRCDEMDEP :
            voutwrite ( "        deptype         %s   \n", "srcdemdep" );
            break;
        case DT_NODEP :
            voutwrite ( "        deptype         %s   \n", "nodep" );
            break;
        default :
            voutwrite ( "        deptype         %s   \n", "?" );
            break;

    }
	voutwrite ( "        nodebug         %d   \n", tip->stat.nodebug ); /* CTS 1102466 */
	voutwrite ( "        file \n" );
	
	
    if ( tip->fixed_file )
    {
        for ( fip = &tip->file ; fip != NULL ; fip = fip->next )
        {
            if ( fip->file && *fip->file ) 
                voutwrite ( "                 '%s' \n", fip->file );
        }
        if ( tip->type == TT_INC || tip->type == TT_MOD || 
             tip->type == TT_MOD || tip->type == TT_EXTRA )
        {
            if ( tip->afile && * tip->afile )
                voutwrite ( "                 '%s' \n", tip->afile );
        }
    }
    voutwrite ( "        descriptions \n" );
    for ( sip = tip->descriptions ; sip != NULL ; sip = sip->next )
    {
        voutwrite ( "                 '%s'\t'%s' \n",
                       sip->descriptor , sip->value );
    }
    voutwrite ( "        options \n" );
    for ( dip = tip->options ; dip != NULL ; dip = dip->next )
    {
		if (tip->file.file )
			voutwrite ( "                 '%s' \n", dip->target->file.file );
    }
    voutwrite ( "        dependencies \n" );
    for ( dip = tip->dependencies ; dip != NULL ; dip = dip->next )
    {
		char * tmp_layer = NULL;
        if ( dip->target->type == TT_FILE )
            p = dip->target->file.file ;
        else  
		{
			p = dip->target->name ;
			if (dip->target->layer)
				tmp_layer = dip->target->layer;
		}
		if (p)
		{
			if (tmp_layer)
			{
				voutwrite ( "                 '%s/%s'\t'%c' \n",
							tmp_layer, p , dip->target->version );
			}
			else
			{
					voutwrite ( "                 '%s'\t'%c' \n",
							p , dip->target->version );
			}

		}
    }
    voutwrite ( "        objdemands \n" );
    for ( dmip = tip->objdemands ; dmip != NULL ; dmip = dmip->next )
    {
        if ( dmip->directory && * dmip->directory )
			voutwrite ( "            dir '%s' \n", dmip->directory );
		else 
			voutwrite ( "            dir '' \n");
        for ( dip = dmip->list ; dip != NULL ; dip = dip->next )
        {
            if ( dip->target->type == TT_FILE )
                p = dip->target->file.file ;
            else    
                p = dip->target->name ;
			if (p)
				voutwrite ( "                 '%s'\t'%c' \n",
                            p , dip->target->version );
        }
    }
    voutwrite ( "        srcdemands \n" );
    for ( dmip = tip->srcdemands ; dmip != NULL ; dmip = dmip->next )
    {
        if ( dmip->directory && * dmip->directory )
			voutwrite ( "            dir '%s' \n", dmip->directory );
		else 
			voutwrite ( "            dir '' \n");
        for ( dip = dmip->list ; dip != NULL ; dip = dip->next )
        {
            if ( dip->target->type == TT_FILE )
                p = dip->target->file.file ;
            else    
                p = dip->target->name ;
			if (p)
				voutwrite ( "                 '%s'\t'%c' \n",
                            p , dip->target->version );
        }
    }
    if ( tip->type == TT_MOD )
    {
        voutwrite ( "        objects \n" );
        for ( oip = tip->objects ; oip != NULL ; oip = oip->next )
            voutwrite ( "                 '%s' \n", oip->name_o );
    }
    voutwrite ( "        external dependencies\n" );
    for ( eip = tip->ext_dependencies ; eip != NULL ; eip = eip->next )
    {
        voutwrite ( "                 '%s' \n", eip->file );
        voutwrite ( "                 '%s' \n", ascdate( eip->date ) );
    }
    voutwrite ( "        callers \n" );
    for ( dip = tip->callers ; dip != NULL ; dip = dip->next )
    {
		if (dip->target->layer)
		{
				voutwrite ( "                 '%s/%s'\t'%c' \n",
					dip->target->layer, dip->target->name , dip->target->version );
		}
		else
		{
			voutwrite ( "                 '%s'\t'%c' \n",
                dip->target->name , dip->target->version );
		}
    }
    if ( tip->type == TT_FILE && tip->file.file )
        voutwrite ( "        file     '%s' \n", tip->file.file );
    voutwrite ( "\n" );
}

/*==========================================================================*/

static  void    print_modlist
(
)
{
    struct dsc_info     * dip ;
    struct trg_info     * etip ;
    struct trg_info     * itip ;
    struct trg_info     * mtip ;
    char                output [ NAMESIZE ];

    dip  = desc_list ;
    etip = extra_list ;
    itip = includes ;
    mtip = modules ;

    while ( dip )
    {
        voutwrite ( "%s\n", dip->descriptor );
        dip = dip->next ;
    }

    while ( etip )
    {
        voutwrite ( "src/%s/%s\n", etip->layer , etip->name );
        etip = etip->next ;
    }

    while ( itip )
    {
        /* voutwrite ( "src/%s/%.*s\n", itip->layer , itip->basenamelen , itip->name ); */
        voutwrite ( "src/%s/%s\n", itip->layer , get_basename( itip->name, output) );
        itip = itip->next ;
    }

    while ( mtip )
    {
        /* voutwrite ( "src/%s/%.*s\n", mtip->layer , mtip->basenamelen , mtip->name ); */
		if (mtip->local_source)
			voutwrite ( "src/%s/%s (%s)\n", mtip->layer , get_basename( mtip->name, output), mtip->local_source );
		else
			voutwrite ( "src/%s/%s\n", mtip->layer , get_basename( mtip->name, output) );
        mtip = mtip->next ;
    }

    DBG4 (( "print_modlist: returning \n" ))
}

/*==========================================================================*/

static  void    print_manual
(
)
{
    int         rc ;
    int         is_tty ;
    int         status_dummy ;
    time_t          date ;
    VFILE           * vfp ;
    struct stat     statbuf ;
    char            * file = NULL;
    char            line [ LINESIZE ];
    valid_bit_arr       vbits ;

    VALID_BIT_FILL ( vbits );
    rc = get_file ( "vmakeman" , DIR_TSRCVMAK , EMPTY , EMPTY ,
        FK_TOOL | FK_LEVEL , vbits ,
        &file , & date , & status_dummy, 1 );
    if ( rc <= 0 )
    {
        rc = get_file ( "vmakeman" , DIR_TLIB , EMPTY , EMPTY ,
            FK_TOOL | FK_LEVEL , vbits ,
            &file , & date , & status_dummy, 1 );
    }
    if ( (rc <= 0) && env_TOOL )
    {
        file = (char *) repalloc( file, strlen( env_TOOL ) + 
                strlen( "/src/vmake/vmakeman" ) + 1 );
        (void) sprintf ( file , "%s/src/vmake/vmakeman" , env_TOOL );
        rc = stat ( file , &statbuf );
        if ( rc < 0 )
            rc = 0 ; /* not found */
        else
            rc = 1 ; /* found */
    }
    if ( (rc <= 0) && env_TOOL )
    {
        file = (char *) repalloc( file, strlen( env_TOOL ) + 
                strlen( "/src/lib/vmakeman" ) + 1 );
        (void) sprintf ( file , "%s/lib/vmakeman" , env_TOOL );
        rc = stat ( file , &statbuf );
        if ( rc < 0 )
            rc = 0 ; /* not found */
        else
            rc = 1 ; /* found */
    }
    if ( rc <= 0 )
    {
        DBG0 (( "missing manual file $TOOL/lib/vmakeman \n" ))
        return ;
    }

    is_tty = isatty ( 1 );

    vfp = fileopen ( file , "r" );
    if ( ! vfp )
    {
        DBG0 (( "cannot open manual file '%s' \n", file ))
        return ;
    }

    while ( filegets ( line , sizeof(line) , vfp ) )
    {
        if ( (line [ 0 ] == '\f') && is_tty )
        {
            (void) fputs ( "-more- " , stdout );
            (void) fflush ( stdout );
            (void) fgets ( line , sizeof(line) , stdin );
            continue ;
        }
        (void) fputs ( line , stdout );
    }

    (void) fflush ( stdout );
    (void) fileclose ( vfp );
    FREE_AND_SET_TO_NULL(file);
}


static int set_main_layer ()
{
	char buffer[DESCRIPTIONSIZE];
	char *p;

    /* Get the current working directory: */
    if( getcwd( buffer, DESCRIPTIONSIZE ) == NULL )
       return 1;
   
	for ( p = buffer; *p != '\0'; p++ )
	{
		if ( *p == '\\' )
			*p = '/';
	}

	if ( (p = strstr(buffer, "sys/src") ))
	{
		p += 8;
	}
	else 
		if  (p = strstr(buffer,"sys/desc")) /* ) && ( p - buffer + 9 < strlen(buffer) )) */
		{
			p += 9;
		}
		else
		{
			p = NULL;
		}

	if (p && (p < buffer + strlen(buffer)))
	{
		main_layer = palloc(strlen(p)+1);
		strcpy (main_layer, p);
		printf ("layer is %s\n", p);
	}

	return 0;
}


/*==========================================================================*/
#if unix

static  SIG_RTRN        catch_signal 
(
    int sig 
)
{
    int                     lgt ;
    char                    line [ LINESIZE ] ;


   (void) signal ( sig , SIG_IGN );

    if ( sig == SIGTERM )
    {
		 exit_vmake ( sig, TRUE );
        DBG0 (( "exit returned !!! \n" ))
        return ; /* should never be executed !!! */
    }

    if ( sig == SIGTRAP )
    {
        (void) signal ( SIGTRAP , catch_signal );
        if ( getpid() == vmakepid )
        {
            static  int     pausing ;

            if ( pausing ) return ;
            pausing = 1 ;
            (void) pause ();
            pausing = 0 ;
        }
        else
        {
            (void) sprintf ( line ,
                "Don't try to pause the vmake controller \n" );
            lgt = strlen ( line );
            (void) write ( 1 , line , (unsigned) lgt );
        }
        return ;
    }

    (void) sprintf ( line , "do you want to cancel vmake ?   " );
    lgt = strlen ( line );
    (void) write ( 1 , line , (unsigned) lgt );
    (void) read ( 0 , line , sizeof(line) );
    if ( line [ 0 ] == 'y' || line [ 0 ] == 'Y' )
    {
        (void) kill ( (pid_t) (- vmakepid) , sig );
		exit_vmake ( sig, TRUE );
        DBG0 (( "exit returned !!! \n" ))
        return ; /* should never be executed !!! */
    }

    (void) sprintf ( line ,
        "do you want to continue vmake in the background ?   " );
    lgt = strlen ( line );
    (void) write ( 1 , line , (unsigned) lgt );
    (void) read ( 0 , line , sizeof(line) );
    if ( line [ 0 ] == 'y' || line [ 0 ] == 'Y' )
    {
		exit_vmake ( sig, TRUE );
        DBG0 (( "exit returned !!! \n" ))
        return ; /* should never be executed !!! */
    }

    (void) signal ( sig , catch_signal );
}
#else

static BOOL WINAPI catch_event ( DWORD dwCtrlType )
{
	switch ( dwCtrlType )
	{
		case CTRL_C_EVENT:
			DBG0 (("request vmake stop by CTRL+C\n"));
			SetEvent(hStopRequest);
			exit_vmake(1,TRUE);
			return (TRUE);
			break;

		case CTRL_BREAK_EVENT:
			DBG0 (("request vmake stop by CTRL+BREAK\n"))
            SetEvent(hStopRequest);
			exit_vmake(1,TRUE);
			return (TRUE);
			break;

		case CTRL_CLOSE_EVENT:
			DBG0 (("request vmake stop by CLOSE\n"))
            SetEvent(hStopRequest);
			exit_vmake(1,TRUE);
			return (TRUE);
			break;

		case CTRL_SHUTDOWN_EVENT:
			DBG0 (("request vmake stop by SHUTDOWN\n"))
            SetEvent(hStopRequest);
			exit_vmake(1,TRUE);
			return (FALSE);
			break;
	}
	return (FALSE);
}

DWORD WINAPI checkforStopRequest (pid_t pid)
{
	if ( WaitForSingleObject ( hStopRequest , INFINITE ) == WAIT_OBJECT_0 )
    {
	    DBG1 (( "vmake get a external stop request\n" ))
		fflush( stdout );
		exit_vmake ( 1, TRUE );
    }
	return (0);
}

static int createStopRequestThread ()
{
	DWORD dwThreadId, dwThrdParam = 1; 
    
    hStopRequestThread = CreateThread( 
        NULL,                        // default security attributes 
        0,                           // use default stack size  
        (LPTHREAD_START_ROUTINE)checkforStopRequest,         // thread function 
        &dwThrdParam,                // argument to thread function 
        0,                           // use default creation flags 
        &dwThreadId);                // returns the thread identifier 
 
   // Check the return value for success. 
 
   if (hStopRequestThread == NULL) 
   {
      DBG0 (("Error while starting stop request thread (%d)", GetLastError()))
		  return (-1);
   }
   return (0);
}


#endif /*unix*/

