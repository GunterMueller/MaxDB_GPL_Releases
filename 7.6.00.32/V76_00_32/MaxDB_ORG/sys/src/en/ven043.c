/*!
  @file           ven043.c
  @author         JoergM
  @brief          make_terminfo: Create a Terminfo Description for a Terminal
  @see

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2001-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end



//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
*/

#define MOD__ "ven043.c:"

#include "geo002.h"
#include "gen04.h"
#include "geo001.h"
#include "geo57.h"
#include "hen40.h"
#include "heo01.h"

#ifdef FREEBSD
#define TABDLY OXTABS
#endif

static	char * VERSION = "@(#)maketi  7.2  2000-07-25 ";
static	char * USAGE []=
{
"usage: x_maketi [options] ",
"    -d         delete all existing key sequences ",
"    -h         create hif-file instead of ti-file ",
"    -e         use explicit <ESC>-confirmation (e.g. for remote login) ",
"    -u         create ti-file from binary source (untic) ",
"    -I term    specifies source terminal type ",
"    -T term    specifies destination terminal type ",
"    -o file    specifies destination file name ",
0};
#define	ARGS(x)	((x) == 0)

#define		LABELLINE		2	/* line for first label */
#define		LABELSIZE		13	/* one less is displayed */

#define		ATTR_NORMAL		0	/* normal attributes */
#define		ATTR_REVERSE		1	/* reverse video */
#define		ATTR_MESSAGE		2	/* message display */

#define MOVE_LABEL(i)	(void) move (   LABELLINE+(i/label_cols) , \
					LABELSIZE*(i%label_cols) + 1 )
#define	MSG(x)		{msg x;}

/* exported functions */

/* imported functions */

extern	char			*sqlerrs ();

/* local functions */

static	int			maketi_session ();
static	int			open_session ();
static	void			close_session ();
static	int			get_cursor_keys ();
static	void			write_header ();
static	int			read_key ();
static	int			get_key ();
static	void			delete_key ();
static	void			write_label ();
static	void			fill_line ();
static	void			prompt_label ();
static	int			write_output ();
static	void			write_hifkeys ();
static	void			write_terminfo ();
static	void			write_bool ();
static	void			write_number ();
static	void			write_string ();
static	int			create_temp_file ();
static	void			remove_temp_file ();
static	void			free_key_list ();
static	void			msg (char *,...);
static	void			set_attrib ();
static	SIGNALFUNCTYPE		catch_alarm ();
static	SIGNALFUNCTYPE		catch_signal ();

/* local data */

static	int			opt_delete = 0;	/* -d */
static	int			opt_hifkey = 0;	/* -h */
static	int			opt_escape = 0;	/* -e */
static	int			opt_untic = 0;	/* -u */

static	char		*input_TERM = NULL;	/* source term name */
static	char		*output_TERM = NULL;	/* destination term name */
static	char		*output_file = NULL ;	/* destination path name */
static	char		*temp_file = NULL;	/* work file name */
static	FILE		*tempfp = NULL ;	/* pointer to work file */

static	int			cur_label = 0;	/* the current label index */
static	int			label_count = 0;	/* maximum number of labels */
static	int			label_cols = 0;	/* label columns per line */
static	int			label_lines = 0;	/* label lines */
static	int			in_curses = 0;	/* between initscr and endwin */
static	int			accepted_on = 0;	/* the 'accepted' label is on */
static	int			message_on = 0;	/* a message is displayed */

static	TERMIO_INFO		termio_extern ;	    /* termio outside curses */
static	TERMIO_INFO		termio_intern ;	    /* termio inside curses */
static	char			key_buf [ 256 ] = {
'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0',
'\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};    /* sequence of last key */


/*==========================================================================*/

global	int	main (
int			argc ,
char			**argv )
{
    int				rc ;
    int				argn ;
    char			*ptr ;
    tsp00_Pathc     TerminfoPath;
    tsp01_RteError  RteError ;

    /*
     *  Get all needed environment strings.
     */
    rc = sql04_get_environment ();
    if ( rc < 0 ) return ( -rc );

    /*
     *  Get command line options.
     */
    input_TERM  = sql04_env_TERM ;
    output_TERM = sql04_env_TERM ;
    rc = 0 ;
    for ( argn = 1 ; argn < argc ; argn ++ )
    {
	ptr = argv[argn];
	if ( *ptr != '-' ) break ;
	ptr ++ ;
	if ( *ptr == '-' ) break ; /* option -- */

	for ( ; *ptr ; ptr ++ )
	{
	    switch ( *ptr )
	    {
	    case '?':
		rc ++ ;
		break ;

	    case 'd':
		opt_delete = 1 ;
		break ;

	    case 'e':
		opt_escape = 1 ;
		break ;

	    case 'h':
		opt_hifkey = 1 ;
		break ;

	    case 'u':
		opt_untic = 1 ;
		break ;

	    case 'I':
		if ( ptr[1] || (argn+1 < argc) )
		{
		    ptr ++ ;
		    if ( ! *ptr ) ptr = argv[++argn];
		    input_TERM = argv[argn];
		    while ( *ptr ) ptr ++ ; /* skip rest of argument */
		    ptr -- ;
		}
		else
		{
		    MSG (( "argument missing for '-%c' \n", *ptr ))
		    rc ++ ;
		}
		break ;

	    case 'T':
		if ( ptr[1] || (argn+1 < argc) )
		{
		    ptr ++ ;
		    if ( ! *ptr ) ptr = argv[++argn];
		    output_TERM = argv[argn];
		    while ( *ptr ) ptr ++ ; /* skip rest of argument */
		    ptr -- ;
		}
		else
		{
		    MSG (( "argument missing for '-%c' \n", *ptr ))
		    rc ++ ;
		}
		break ;

	    case 'o':
		if ( ptr[1] || (argn+1 < argc) )
		{
		    ptr ++ ;
		    if ( ! *ptr ) ptr = argv[++argn];
		    output_file = ptr ;
		    while ( *ptr ) ptr ++ ; /* skip rest of argument */
		    ptr -- ;
		}
		else
		{
		    MSG (( "argument missing for '-%c' \n", *ptr ))
		    rc ++ ;
		}
		break ;

	    default:
		MSG (( "illegal option '%c' \n", *ptr ))
		rc ++ ;
		break ;
	    }
	}
    }
    if ( rc || !ARGS(argc - argn) )
    {
	for ( rc = 0 ; USAGE[rc] ; rc ++ ) MSG (( "%s\n", USAGE[rc] ))
	return ( 1 );
    }

    /*
     *  If not already set by option -o, build output file name.
     */
    if ( ! output_file )
    {
	if ( opt_hifkey && strcmp(sql04_env_DBHIF , sql04_env_TERM) )
	    ptr = sql04_env_DBHIF ;
	else
	    ptr = output_TERM ;

	/* PTS 1107043 */
    if ( !sqlGetIndependentTerminfoPath( TerminfoPath, TERM_WITHOUT_DELIMITER_EO01, &RteError ) )
	{
        MSG (( "Could not determine terminfo path!\n" ))
        MSG (( "RTE_ERROR: %s\n", RteError.RteErrText ))
        MSG (( "OS_ERROR : %s\n", RteError.OsErrText ))
	    return ( -3 );
	}

				/* TerminfoPath/term/$DBHIF.hif\0 */
				/*       +       6 +  ? +1+3 +1 */
	if ( ALLOC_MEM_EO57( (void **)&output_file,
                    strlen(TerminfoPath) + strlen(ptr) + 20 ) != NO_ERROR_EO001 )
    {
	    return ( -3 );
    }

	if ( opt_hifkey )
	    spri ( output_file , "%s/term/%s.hif", TerminfoPath , ptr );
	else
	    spri ( output_file , "%s/%s.ti", TerminfoPath , ptr );
    }

    /*
     *  If -I term is specified, use this for curses initialization.
     */
    if ( strcmp(input_TERM , sql04_env_TERM) )
    {
	    /* TERM= + input_TERM + '\0' */
	    /*   5   +    ???     +   1  */
    	if ( ALLOC_MEM_EO57( (void **)&ptr, 6 + strlen(input_TERM) ) != NO_ERROR_EO001 )
        {
	        return ( -3 );
        }
        spri ( ptr , "TERM=%s" , input_TERM );
    	if ( putenv(ptr) )
	    {
	        return ( -3 );
    	}
	    sql04_envdef_TERM = ptr ;
	    sql04_env_TERM    = ptr + 5 ; /* point to the TERM name */
    }

    /*
     *  Catch interrupts to perform cleanup.
     */
    (void) signal ( SIGHUP  , (SIGNALFUNCTYPE(*)()) catch_signal );
    (void) signal ( SIGINT  , (SIGNALFUNCTYPE(*)()) catch_signal );
    (void) signal ( SIGTERM , (SIGNALFUNCTYPE(*)()) catch_signal );

    /*
     *  Open a temporary file for use as output file.
     *  This verifies that the directory can be written.
     *  The temporary file will be moved to 'output_file'
     *  after the write operation is complete.
     */
    rc = create_temp_file ();
    if ( rc < 0 ) return ( -rc );

    /*
     *  Start the curses session.
     *  This also reads the terminfo and hifkey files
     *  and initializes the terminfo structues.
     */
    rc = open_session ();

    /*
     *  If requested, delete all key sequences.
     */
    if ( opt_delete )
    {
    	free_key_list ();
    }

    /*
     *  Untic only.
     *  This needs the the initialized terminfo structures.
     */
    if ( (rc >= 0) && opt_untic )
    {
	/*
	 *  'close_session' resets the terminal modes
	 *  but the terminfo structures are not released.
	 */
	close_session ();
	rc = write_output ();
	remove_temp_file ();
	return ( rc < 0 ? -rc : 0 );
    }

    /*
     *  If 'open_session' succeded, perform the maketi session.
     */
    if ( rc >= 0 ) rc = maketi_session ();

    /*
     *  Leave the curses session.
     */
    close_session ();

    /*
     *  If 'maketi_session' returned a positive result, write the output.
     */
    if ( rc > 0 ) rc = write_output ();

    remove_temp_file ();
    return ( rc < 0 ? -rc : 0 );
}

/*==========================================================================*/

#undef MF__
#define MF__ MOD__"maketi_session"
static	int	maketi_session ( )
{
    int				rc ;
    int				fin ;
    int				function ;

    DBGIN;

#if A_COLOR && !defined(A_BGCOLOR)
    (void) init_pair ( 1 , COLOR_WHITE , COLOR_BLUE  );
    (void) init_pair ( 2 , COLOR_BLUE  , COLOR_WHITE );
    (void) init_pair ( 3 , COLOR_WHITE , COLOR_RED   );
    set_attrib ( ATTR_NORMAL );
    /*
     *  Fill the complete screen with the background color.
     */
    for ( fin = 0 ; fin < LINES ; fin ++ ) fill_line ( fin , (chtype) ' ' );
#endif

    /*
     *  Get the number of possible keys,
     *  number of lines for key name labels and
     *  number of key name labels per line.
     */
    label_count = sql04_key_count ;
    label_cols  = COLS / LABELSIZE ;
    label_lines = (label_count + label_cols - 1) / label_cols ;

    /*
     *  Prompt for cursor keys, if not existent.
     */
    rc = get_cursor_keys ();
    if ( rc < 0 ) return ( rc );

    write_header ();

    cur_label = 0 ;
    for ( fin = 0 ; fin == 0 ; )
    {
	function = sql04_cap_table[cur_label].ci_function ;
	prompt_label ( function );

	rc = read_key ( function , FALSE );
	switch ( rc )
	{
	case VTKEY_CHARACTER :
	    switch ( sql04_key_value )
	    {
	    case 'd' :
		delete_key ( function );
		write_label ( cur_label );
		break ;

	    case 'e' :
	    case 'q' :
		fin = sql04_key_value ;
		break ;

	    default :
		MSG (( "unacceptable character '%c', retry \n",
						sql04_key_value ))
	    }
	    break ;

	case VTKEY_CURSOR_RIGHT :
	    cur_label ++ ;
	    if ( cur_label >= label_count ) cur_label = 0 ;
	    break ;

	case VTKEY_CURSOR_LEFT :
	    cur_label -- ;
	    if ( cur_label < 0 ) cur_label = label_count - 1 ;
	    break ;

	case VTKEY_CURSOR_DOWN :
	    cur_label += label_cols ;
	    if ( cur_label >= label_count )
		cur_label -= label_lines * label_cols ;
	    if ( cur_label < 0 )
		cur_label += label_cols ;
	    break ;

	case VTKEY_CURSOR_UP :
	    cur_label -= label_cols ;
	    if ( cur_label < 0 )
		cur_label += label_lines * label_cols ;
	    if ( cur_label >= label_count )
		cur_label -= label_cols ;
	    break ;

	case VTKEY_REFRESH :
	    write_header ();
	    break ;
	}
    }

    if ( fin == 'e' )
	rc = 1 ;
    else
	rc = 0 ;
    return ( rc );
}

/*==========================================================================*/

static	int	open_session ( )

{
    int				rc ;


    DBG1 (( MF__,"open_session: called \n" ))

    /*
     *  Save the termio values as they were outside of curses.
     */
    (void) GET_TERMIO ( 0 , &termio_extern );

#if T31 || T35 || RM6
    /*
     *  Initialize the standard character classification (ctype).
     */
    (void) setchrclass ( (char*) 0 );
#endif

    /*
     *  Read our own character classification file.
     *  The only class of interest is 'isprint'.
     *  All non-space characters in this file are treated as printable.
     */
    rc = sql04_read_chrclass ();
    if ( rc < 0 ) return ( rc );

    /*
     *  Initialize curses.
     *  'initscr' will never return unsuccessful.
     *  On error it writes a message and exits.
     *  If successful it returns stdscr.
     *  Verify this here, just in case...
     */
    if ( (initscr() != stdscr) || (stdscr == 0) )
    {
	MSG (( "error in curses-initscr() \n" ))
	return ( -2 );
    }
    in_curses = TRUE ;

    /*
     *  Avoid using tabs.
     *  On some systems tabs are unreliably expanded by terminal driver.
     */
    tab = 0 ;
    set_tab = 0 ;
    back_tab = 0 ;
    clear_all_tabs = 0 ;

    /*
     *  Set imput modes.
     */
    (void) noecho ();
    (void) cbreak ();

    /*
     *  Initialize the terminal.
     */
    if ( init_prog ) (void) system ( init_prog );
    if ( init_file ) (void) sql04_cat ( init_file );
    PUTP ( init_1string );
    PUTP ( init_2string );
    PUTP ( init_3string );
    PUTP ( keypad_xmit );

    /*
     *  Save the termio values as they are inside of curses.
     */
    (void) GET_TERMIO ( 0 , &termio_intern );

    /*
     *  Adjust the termio flags which are not equally set by all
     *  curses implementations.
     */
    termio_intern.c_iflag &= ~(ISTRIP|INLCR|IGNCR|ICRNL);
    termio_intern.c_oflag &= ~(ONLCR|OCRNL|TABDLY);
    termio_intern.c_lflag &= ~(ICANON|ECHO|ECHOE|ECHOK);
    (void) SET_TERMIO ( 0 , &termio_intern );

    /*
     *  Initialize the graphic characters conversion table.
     */
    (void) sql04_init_graphic_table ();

    /*
     *  Initialize the color feature.
     */
#if A_COLOR && !defined(A_BGCOLOR)
    (void) start_color ();
#endif /*A_COLOR*/

    /*
     *  Initialize the key list.
     */
    rc = sql04_make_key_list ();

    return ( rc );
}

/*==========================================================================*/

static	void	close_session ( )

{
    DBG1 (( MF__,"close_session: called \n" ))

    /*
     *  Reset the screen to the 'outside curses' state.
     */
    (void) endwin ();
    in_curses = FALSE ;

    /*
     *  On some systems 'endwin' leaves stdio in full buffered mode.
     */
    (void) setvbuf ( stdin  , (char*) 0 , _IOLBF , BUFSIZ );
    (void) setvbuf ( stdout , (char*) 0 , _IOLBF , BUFSIZ );
    (void) setvbuf ( stderr , (char*) 0 , _IONBF , BUFSIZ );

    /*
     *  Since we modified the termio values by ourself,
     *  we restore them completely to their previous state.
     */
    (void) SET_TERMIO ( 0 , &termio_extern );

    /*
     *  Reset the terminal.
     */
    PUTP ( keypad_local );
    if ( init_prog  ) (void) system ( init_prog );
    if ( reset_file ) (void) sql04_cat ( init_file );
    PUTP ( reset_1string );
    PUTP ( reset_2string );
    PUTP ( reset_3string );
    (void) fputs ( "\r\n" , stdout );
    (void) fflush ( stdout );
}

/*==========================================================================*/

static	int	get_cursor_keys ( )

{
    int				keyfunc ;
    int				has_up ;
    int				has_down ;
    int				has_left ;
    int				has_right ;
    struct key_info		*kip ;


    DBG1 (( MF__,"get_cursor_keys: called \n" ))

    /*
     *  Check for the existence of all cursor keys.
     */
    has_up = has_down = has_left = has_right = 0 ;
    for ( kip = sql04_key_list ; kip ; kip = kip->ki_next )
    {
	switch ( kip->ki_function )
	{
	case VTKEY_CURSOR_UP :
	    has_up = 1 ;
	    break ;

	case VTKEY_CURSOR_DOWN :
	    has_down = 1 ;
	    break ;

	case VTKEY_CURSOR_LEFT :
	    has_left = 1 ;
	    break ;

	case VTKEY_CURSOR_RIGHT :
	    has_right = 1 ;
	    break ;

	}
    }
    if ( has_up && has_down && has_left && has_right )
    {
	DBG1 (( MF__,"get_cursor_keys: all cursor keys are already present \n" ))
	return ( 0 );
    }

    set_attrib ( ATTR_NORMAL );
    (void) move ( 0 , 1 );
    (void) addstr ( VERSION+4 );
    (void) move ( 0 , 41 );
    set_attrib ( ATTR_REVERSE );
    (void) printw ( " %s " , output_TERM );
    set_attrib ( ATTR_NORMAL );
    fill_line ( LABELLINE - 1 , sql04_graphic[CH_HORIZONTAL_BAR] );
    fill_line ( LINES - 4 , sql04_graphic[CH_HORIZONTAL_BAR] );
    (void) move ( LINES - 3 , 1 );
    (void) addstr ( "'q' - quit without modifications" );
    fill_line ( LINES - 2 , sql04_graphic[CH_HORIZONTAL_BAR] );
    fill_line ( LINES - 1 , (chtype) ' ' );
    (void) move ( LINES - 1 , 1 );
    (void) printw ( "Output File: %s ", output_file );

    (void) move (10, 1);
    (void) addstr ("Press <cursor right> ");
    keyfunc = read_key ( VTKEY_CURSOR_RIGHT , TRUE );
    if ( (keyfunc != VTKEY_CURSOR_RIGHT) &&
	 (keyfunc != VTKEY_UNKNOWN) ) return ( -1 );

    (void) move (10, 1);
    (void) addstr ("Press <cursor left>  ");
    keyfunc = read_key ( VTKEY_CURSOR_LEFT  , TRUE );
    if ( (keyfunc != VTKEY_CURSOR_LEFT) &&
	 (keyfunc != VTKEY_UNKNOWN) ) return ( -1 );

    (void) move (10, 1);
    (void) addstr ("Press <cursor up>    ");
    keyfunc = read_key ( VTKEY_CURSOR_UP    , TRUE );
    if ( (keyfunc != VTKEY_CURSOR_UP) &&
	 (keyfunc != VTKEY_UNKNOWN) ) return ( -1 );

    (void) move (10, 1);
    (void) addstr ("Press <cursor down>  ");
    keyfunc = read_key ( VTKEY_CURSOR_DOWN  , TRUE );
    if ( (keyfunc != VTKEY_CURSOR_DOWN) &&
	 (keyfunc != VTKEY_UNKNOWN) ) return ( -1 );

    DBG1 (( MF__,"get_cursor_keys: returning \n" ))
    return ( 0 );
}

/*==========================================================================*/

static	void	write_header ( )

{
    int			lno ;
    int			cno ;
    int			lab ;


    (void) clear ();
    set_attrib ( ATTR_NORMAL );

#if A_COLOR && !defined(A_BGCOLOR)
    /*
     *  Fill the complete screen with the background color.
     */
    for ( lno = 0 ; lno < LINES ; lno ++ ) fill_line ( lno , (chtype) ' ' );
#endif

    (void) move ( 0 , 1 );
    (void) addstr ( VERSION+4 );
    (void) move ( 0 , 39 );
    set_attrib ( ATTR_REVERSE );
    (void) printw ( " %s " , output_TERM );
    set_attrib ( ATTR_NORMAL );
    fill_line ( LABELLINE - 1 , sql04_graphic[CH_HORIZONTAL_BAR] );

    lab = 0 ;
    for ( lno = 0 ; lno < label_lines ; lno ++ )
    {
	for ( cno = 0 ; cno < label_cols ; cno ++ )
	{
	    if ( lab >= label_count ) break ;
	    write_label ( lab ++ );
	}
	if ( lab >= label_count ) break ;
    }

    fill_line ( LABELLINE + label_lines , sql04_graphic[CH_HORIZONTAL_BAR] );
    (void) move ( LABELLINE + label_lines + 1 , 1 );
    (void) addstr ( "Use the cursor keys or TAB to select a key." );
    (void) move ( LABELLINE + label_lines + 2 , 1 );
    (void) addstr ( "Then press the corresponding key (combination)." );
    fill_line ( LABELLINE + label_lines + 3 , sql04_graphic[CH_HORIZONTAL_BAR]);

    fill_line ( LINES - 5 , sql04_graphic[CH_HORIZONTAL_BAR] );
    (void) move ( LINES - 4 , 1 );
    (void) addstr ( "'d' - delete selected key sequence" );
    (void) move ( LINES - 3 , 1 );
    (void) addstr ( "'e' - exit and save results" );
    (void) move ( LINES - 3 , 41 );
    (void) addstr ( "'q' - quit without modifications" );
    fill_line ( LINES - 2 , sql04_graphic[CH_HORIZONTAL_BAR] );
    fill_line ( LINES - 1 , (chtype) ' ' );
    (void) move ( LINES - 1 , 1 );
    (void) printw ( "Output File: %s ", output_file );
}

/*==========================================================================*/

static	int	read_key ( function , replace_uncond )

int			function ;
int			replace_uncond ;

{
    int				rc ;
    int				keyfunc ;
    int				lab ;
    char			buf [ 2 ];


    DBG1 (( MF__,"read_key: function %d (%s) \n", function ,
			sql04_key_name(function) ))

    for ( ; ; )
    {
	keyfunc = get_key ();
	DBG1 (( MF__,"read_key: get_key returned %d (%s) \n",
			keyfunc , sql04_key_name(keyfunc) ))
	DBG1 (( MF__,"read_key: get_key returned key_buf '%s' \n",
			sql04_put_sequence(key_buf) ))
	if ( keyfunc < 0 ) continue ; /* sequence too long */
	if ( keyfunc == function ) break ; /* everything is fine */

	if ( keyfunc == VTKEY_CHARACTER )
	{
	    switch ( sql04_key_value )
	    {
	    case '\t' :
		/* TAB steps to the next menu item */
		keyfunc = VTKEY_CURSOR_RIGHT ;
		break ;

	    case 'd' :
	    case 'e' :
		if ( ! replace_uncond ) break ; /* accept that character */
		/*FALLTHROUGH*/

	    case 'q' :
		break ; /* accept that character */

	    default :
		MSG (( "unaccaptable character '%c', retry \n",
						sql04_key_value ))
		MOVE_LABEL ( cur_label );
		continue ;
	    }
	    break ;
	}

	if ( keyfunc != VTKEY_UNKNOWN )
	{
	    /*
	     *  The key sequence was assigned to another key.
	     */
	    if ( ! replace_uncond )
	    {
		if ( (keyfunc == VTKEY_CURSOR_UP    ) ||
		     (keyfunc == VTKEY_CURSOR_DOWN  ) ||
		     (keyfunc == VTKEY_CURSOR_RIGHT ) ||
		     (keyfunc == VTKEY_CURSOR_LEFT  )    ) break ;

		MSG (( "This sequence is currently assigned to %s, Replace ? ",
						sql04_key_name(keyfunc) ))
		buf[0] = 'n' ;
		(void) read ( 0 , buf , 1 );
		if ( (buf[0] != 'y') && (buf[0] != 'Y') )
		{
		    MSG (( "Key %s NOT modified \n", sql04_key_name(keyfunc) ))
		    MOVE_LABEL ( cur_label );
		    continue ;
		}
		MSG (( "Key %s deleted \n", sql04_key_name(keyfunc) ))
	    }

	    /*
	     *  Remove the other key from list.
	     */
	    delete_key ( keyfunc );

	    /*
	     *  Rewrite the other key's label.
	     */
	    for ( lab = 0 ; lab < label_count ; lab ++ )
		if ( sql04_cap_table[lab].ci_function == keyfunc ) break ;
	    if ( lab < label_count ) write_label ( lab );

	    keyfunc = VTKEY_UNKNOWN ; /* return UNKNOWN as key did not exist */
	}

	switch ( key_buf[0] )
	{
	case '\t' :
	case '\r' :
	case '\n' :
	    MSG (( "unacceptable start of sequence '%s' \n",
				sql04_put_sequence(key_buf) ))
	    MOVE_LABEL ( cur_label );
	    continue ;
	}

	/*
	 *  Remove the current key from list
	 *  (It might already have a different sequence).
	 */
	delete_key ( function );

	/*
	 *  Insert the current key into list.
	 */
	rc = sql04_ins_keyinfo ( function , key_buf );
	if ( rc < 0 )
	{
	    keyfunc = VTKEY_CHARACTER ;
	    sql04_key_value = 'q' ;
	}
	break ;
    }

    return ( keyfunc );
}

/*==========================================================================*/

static	int	get_key ( )

{
    int				rc ;
    int				keyfunc ;
    int				ich ;
    int				seqpos ;
    int				esc_count ;
    int				cur_line ;
    int				cur_column ;
    struct key_info		**first_match ;
    struct key_info		**last_match ;
    struct key_info		**cur ;


    DBG1 (( MF__,"get_key: called \n" ))

    /*
     *  -1 is an illegal key function value. A key sequence is
     *  not matched as long as this value is negative.
     *  key_value is a global variable holding the secondary value
     *  for the key functions VTKEY_CHARACTER and VTKEY_CONTROL.
     */
    keyfunc         = -1 ;
    sql04_key_value = 0 ;

    getyx ( stdscr , cur_line , cur_column );
    (void) refresh ();
    esc_count = 0 ;

    /*
     *  'first_match' points to the first key that matches all
     *  characters of the key sequence as it is currently received.
     *  'last_match' points to the first key after 'first_match'
     *  that does not match all of the characters received.
     *  'first_match' will never exceed 'last_match'.
     *  If both are equal, there is no key matching the received
     *  sequence.
     *
     *  Initially, 'first_match' points to the first key in the list
     *  and 'last_match' points behind the last key in the list.
     *  In this way, the first character received can match all of
     *  the keys in the list.
     *
     *  'first_match' and 'last_match' are pointers to the
     *  list-linkage pointers 'ki_next'.
     *  The last key in the list points to NULL.
     *  'last_match' will then initially point to the linkage-pointer
     *  of the last key in the list.
     *  'cur' is used as a running variable.
     */
    for ( cur = &sql04_key_list ; *cur ; cur = &(*cur)->ki_next );
    first_match = &sql04_key_list ;
    last_match  = cur ;

    /*
     *  Search the key sequence in the key list.
     *  Key sequences may be multi-byte strings.
     *  The first character read from STDIN is compared against the
     *  first characters of the key sequences in the key list.
     *  Since there may be more list entries starting with the same
     *  character, the list is sorted by key sequences and the variables
     *  'first_match' and 'last_match' reflect the positions in the
     *  list where the first character matched.
     *  The second character read form STDIN is then compared
     *  against the second characters of the key sequences in the
     *  list. During this process the first/last match variables
     *  may narrow the range for the next search.
     *  This process continues until the search range shrinked to
     *  a single list entry.
     */
    for ( seqpos = 0 ; seqpos < sizeof(key_buf)-1 ; seqpos ++ )
    {
	DBG1 (( MF__,"get_key: seqpos    %d \n", seqpos ))
	key_buf[seqpos] = 0 ; /* loop can be left by 'break' */

	/*
	 *  Read a character from STDIN.
	 */
	rc = read ( 0 , key_buf + seqpos , 1 );
	if ( rc <= 0 )
	{
	    if ( rc == 0 )
		    MSG (( "get_key: end of file during read \n" ))
	    else
		    MSG (( "get_key: read error: %s \n", sqlerrs() ))
	    keyfunc = VTKEY_CHARACTER ;
	    sql04_key_value = 'q' ;
	    break ;
	}
	ich = key_buf[seqpos] & 0xff ;
	DBG1 (( MF__,"get_key: got character 0x%02x \n", ich ))

	/*
	 *  tic(1M) converts '\0' to '\200'.
	 */
	if ( ich == 0 )
	{
	    ich = 0200 ;
	    key_buf[seqpos] = (char) ich ;
	}

	/*
	 *  If the 'accepted' label is still on, turn it now off.
	 *  If a message is still displayed, erase it now.
	 */
	if ( accepted_on || message_on )
	{
	    if ( accepted_on )
	    {
		(void) move ( 0 , COLS-9 );
		(void) addstr ( "         " );
		accepted_on = FALSE ;
	    }
	    if ( message_on )
	    {
		fill_line ( LINES-1 , (chtype) ' ' );
		(void) move ( LINES - 1 , 1 );
		(void) printw ( "Output File: %s ", output_file );
		message_on = FALSE ;
	    }
	    (void) move ( cur_line , cur_column );
	    (void) refresh ();
	}

	/*
	 *  If option -e was specified, count the escape characters.
	 */
	if ( opt_escape )
	{
	    (void) move ( 0 , COLS-9 );
	    if ( ich == '\033' )
	    {
		esc_count ++ ;
		if ( esc_count >= 3 ) break ;
		(void) printw ( "   ESC %-2d" , esc_count );
	    }
	    else
	    {
		esc_count = 0 ;
		(void) addstr ( "         " );
	    }
	    (void) move ( cur_line , cur_column );
	    (void) refresh ();
	}

	/*
	 *  Find the first list entry matching the character
	 *  at the current position of the key sequence.
	 */
	for ( cur = first_match ; cur != last_match ; cur = &(*cur)->ki_next )
	{
	    if ( ((*cur)->ki_sequence[seqpos]&0xff) >= ich ) break ;
	}
	first_match = cur ;

	/*
	 *  Find the first list entry after 'first_match' NOT matching
	 *  the character at the current position of the key sequence.
	 */
	for ( ; cur != last_match ; cur = &(*cur)->ki_next )
	{
	    if ( ((*cur)->ki_sequence[seqpos]&0xff) > ich ) break ;
	}
	last_match = cur ;

	/*
	 *  'last_match' points always to the first list entry that does
	 *  not match. If 'first_match' points to the same entry,
	 *  the received sequence is not in the list.
	 */
	if ( first_match == last_match ) break ;

	/*
	 *  If there is more than one list entry left,
	 *  continue the search with the next character.
	 */
	if ( (*first_match)->ki_next != *last_match ) continue ;

	/*
	 *  A single list entry is left and it matches the
	 *  currently received character.
	 *  If this is the last character in the list entry's
	 *  key sequence the key sequence is completely
	 *  received and the match is perfect.
	 */
	cur = first_match ;
	if ( (*cur)->ki_sequence[seqpos+1] == 0 )
	{
	    keyfunc = (*cur)->ki_function ;
	    break ;
	}

	/*
	 *  Continue to read characters until the key sequence
	 *  is complete, or a mismatching character is received.
	 */
    }
    key_buf[++seqpos] = 0 ;

    /*
     *  If the key has already been determined in the loop,
     *  the work is finished.
     */
    if ( keyfunc >= 0 )
    {
	DBG1 (( MF__,"get_key: returning %s \n", sql04_key_name(keyfunc) ))
	return ( keyfunc );
    }

    /*
     *  If we received a character as the first byte,
     *  which is printable, return it in the global variable key_value.
     *  It might be one of the special commands.
     */
    if ( (seqpos == 1) && (sql04_isprint(ich) || (ich == '\t')) )
    {
	keyfunc = VTKEY_CHARACTER ;
	sql04_key_value = ich ;
	DBG1 (( MF__,"get_key: returning %d \n", keyfunc ))
	return ( keyfunc );
    }

    /*
     *  If option -e was specified, and escape was hit three times,
     *  the sequence is to be accepted as complete.
     */
    if ( opt_escape && (esc_count >= 3) )
    {
	(void) move ( 0 , COLS-9 );
	(void) addstr ( "accepted " );
	accepted_on = TRUE ;
	(void) move ( cur_line , cur_column );
	(void) refresh ();
	seqpos -= esc_count ;
	key_buf[seqpos] = 0 ;
	keyfunc = VTKEY_UNKNOWN ;
	DBG1 (( MF__,"get_key: returning %d \n", keyfunc ))
	return ( keyfunc );
    }

    /*
     *  Print a label showing the user that we are waiting for more input.
     *  Don't reset the cursor position. This makes the new lable more
     *  visible.
     */
    (void) move ( 0 , COLS-9 );
    if ( opt_escape )
    {
	(void) addstr ( "   ESC 0 " );
    }
    else
    {
	(void) addstr ( " WAITING " );
    }
    (void) refresh ();

    /*
     *  Catch alarm signals to terminate key entry.
     */
    if ( ! opt_escape )
    {
	(void) signal ( SIGALRM , (SIGNALFUNCTYPE(*)()) catch_alarm );
#if _IBMR2 || SUN_ucb || MI_ULT || LINUX || FREEBSD
	(void) siginterrupt ( SIGALRM , TRUE );
#endif
    }

    /*
     *  We received a character that did not match any of the
     *  keys in the list.
     *  Wait for a short period to get the key sequence complete.
     */
    for ( ; seqpos < sizeof(key_buf)-1 ; seqpos ++ )
    {
	DBG1 (( MF__,"get_key: seqpos    %d \n", seqpos ))
	key_buf[seqpos] = 0 ; /* loop can be left by 'break' */
	if ( ! opt_escape ) (void) alarm ( 2 );

	/*
	 *  Read a character from STDIN.
	 */
	rc = read ( 0 , key_buf + seqpos , 1 );
	if ( rc <= 0 )
	{
	    if ( (rc < 0) && (errno == EINTR) )
	    {
		/*
		 *  Assume that interrupt stems from alarm.
		 */
		keyfunc = VTKEY_UNKNOWN ;
	    }
	    else
	    {
    		if ( rc == 0 )
	    	    MSG (( "get_key: end of file during read \n" ))
		    else
		        MSG (( "get_key: read error: %s\n", sqlerrs() ))
	    	keyfunc = VTKEY_CHARACTER ;
		    sql04_key_value = 'q' ;
	    }
	    break ;
	}
	ich = key_buf[seqpos] & 0xff ;
	DBG1 (( MF__,"get_key: got character 0x%02x \n", ich ))

	/*
	 *  tic(1M) converts '\0' to '\200'.
	 */
	if ( ich == 0 )
	{
	    ich = 0200 ;
	    key_buf[seqpos] = (char) ich ;
	}

	/*
	 *  If option -e was specified, count the escape characters.
	 */
	if ( opt_escape )
	{
	    (void) move ( 0 , COLS-9 );
	    if ( ich == '\033' )
	    {
		esc_count ++ ;
		if ( esc_count >= 3 ) break ;
		(void) printw ( "   ESC %-2d" , esc_count );
	    }
	    else
	    {
		esc_count = 0 ;
		(void) addstr ( "         " );
	    }
	    (void) refresh ();
	}
    }
    if ( ! opt_escape ) (void) alarm ( 0 );
    key_buf[++seqpos] = 0 ;

    (void) move ( 0 , COLS-9 );
    if ( seqpos >= sizeof(key_buf) )
    {
    	(void) addstr ( "         " );
    	MSG (( "key sequence is too long \n" ))
	    (void) sleep ( 2 );
	    return ( keyfunc ); /* -1 */
    }
    (void) addstr ( "accepted " );
    accepted_on = TRUE ;
    (void) refresh ();

    /*
     *  If option -e was specified, and escape was hit three times,
     *  the sequence is to be accepted as complete.
     */
    if ( opt_escape && (esc_count >= 3) )
    {
	seqpos -= esc_count ;
	key_buf[seqpos] = 0 ;
	keyfunc = VTKEY_UNKNOWN ;
    }

    DBG1 (( MF__,"get_key: returning %d \n", keyfunc ))
    return ( keyfunc ); /* VTKEY_UNKNOWN or VTKEY_CARACTER('q') */
}

/*==========================================================================*/

static	void	delete_key ( function )

int			function ;

{
    struct key_info	*kip ;
    struct key_info	**skipp ;


    for ( skipp = & sql04_key_list ; *skipp ; )
    {
	if ( (*skipp)->ki_function == function )
	{
	    kip = *skipp ;
	    *skipp = kip->ki_next ;
	    FREE_MEM_EO57 ( kip );
	}
	else
	    skipp = &(*skipp)->ki_next ;
    }
}

/*==========================================================================*/

static	void	write_label ( pos )

int			pos ;

{
    register int		function ;
    struct key_info		*kip ;


    function = sql04_cap_table[pos].ci_function ;

    for ( kip = sql04_key_list ; kip ; kip = kip->ki_next )
	if ( kip->ki_function == function ) break ;

    MOVE_LABEL ( pos );
    if ( kip ) set_attrib ( ATTR_REVERSE );

    (void) printw ( "%.*s", LABELSIZE-1 , sql04_key_name(function) );

    set_attrib ( ATTR_NORMAL );
}

/*==========================================================================*/

static	void	fill_line ( lno , lch )

int			lno ;
chtype			lch ;

{
    int             idx ;


    (void) move ( lno , 0 );

    for ( idx = 0 ; idx < COLS ; idx ++ ) (void) addch ( lch );
}

/*==========================================================================*/

static	void	prompt_label ( function )

register int		function ;

{
    struct key_info		*kip ;


    for ( kip = sql04_key_list ; kip ; kip = kip->ki_next )
	if ( kip->ki_function == function ) break ;

    fill_line ( LABELLINE + label_lines + 4 , (chtype) ' ' );
    (void) move ( LABELLINE + label_lines + 4 , 1 );
    (void) printw ( "Key: %-12s  Capname: %-8s  Sequence: '%s'" ,
			sql04_key_name(function) ,
			sql04_cap_table[cur_label].ci_capname ,
			kip ? sql04_put_sequence(kip->ki_sequence) : "" );

    write_label ( cur_label );
    MOVE_LABEL ( cur_label );
}

/*==========================================================================*/

static	int	write_output ( )

{
    int				rc ;


    if ( opt_hifkey )
	write_hifkeys ();
    else
	write_terminfo ();
    rc = ferror ( tempfp ) ? -2 : 1 ;
    (void) fclose ( tempfp );
    if ( rc < 0 ) return ( rc );

    /*
     *  Disable interrupts, while shuffling files.
     */
    (void) signal ( SIGHUP  , SIG_IGN );
    (void) signal ( SIGINT  , SIG_IGN );
    (void) signal ( SIGQUIT , SIG_IGN );

    (void) unlink ( output_file );
    (void) link ( temp_file , output_file );
    (void) unlink ( temp_file );

    if ( opt_hifkey )
	    MSG (( "New hifkey file '%s' for TERM='%s'\n" ,
			output_file , output_TERM ))
    else
    	MSG (( "New terminfo file '%s' for TERM='%s'\n" ,
			output_file , output_TERM ))
    return ( 1 );
}

/*==========================================================================*/

static	void	write_hifkeys ( )

{
    register int		function ;
    struct capname_info		*cip ;
    struct key_info		*kip ;


    for ( cip = sql04_cap_table ; cip->ci_function ; cip ++ )
    {
	function = cip->ci_function ;
	for ( kip = sql04_key_list ; kip ; kip = kip->ki_next )
	{
	    if ( kip->ki_function == function )
	    {
		(void) fprintf ( tempfp , "%s=%s,\n", cip->ci_capname ,
				    sql04_put_sequence(kip->ki_sequence) );
	    }
	}
    }
}

/*==========================================================================*/

static	void	write_terminfo ( )

{
    (void) fprintf ( tempfp , "%s,\n", output_TERM );

    /*
     *  The rest of this function was created by the program 'unticcod'
     *  which has been built from 'ven04nc'.
     */

    /*
     *  Booleans
     */

#ifdef auto_right_margin
    write_bool   ( "am"        , auto_right_margin             );
#endif
#ifdef back_color_erase
    write_bool   ( "bce"       , back_color_erase              );
#endif
#ifdef auto_left_margin
    write_bool   ( "bw"        , auto_left_margin              );
#endif
#ifdef can_change
    write_bool   ( "ccc"       , can_change                    );
#endif
#ifdef hard_cursor
    write_bool   ( "chts"      , hard_cursor                   );
#endif
#ifdef cpi_changes_res
    write_bool   ( "cpix"      , cpi_changes_res               );
#endif
#ifdef cr_cancels_micro_mode
    write_bool   ( "crxm"      , cr_cancels_micro_mode         );
#endif
#ifdef memory_above
    write_bool   ( "da"        , memory_above                  );
#endif
#ifdef has_print_wheel
    write_bool   ( "daisy"     , has_print_wheel               );
#endif
#ifdef memory_below
    write_bool   ( "db"        , memory_below                  );
#endif
#ifdef erase_overstrike
    write_bool   ( "eo"        , erase_overstrike              );
#endif
#ifdef status_line_esc_ok
    write_bool   ( "eslok"     , status_line_esc_ok            );
#endif
#ifdef generic_type
    write_bool   ( "gn"        , generic_type                  );
#endif
#ifdef hard_copy
    write_bool   ( "hc"        , hard_copy                     );
#endif
#ifdef hue_lightness_saturation
    write_bool   ( "hls"       , hue_lightness_saturation      );
#endif
#ifdef has_status_line
    write_bool   ( "hs"        , has_status_line               );
#endif
#ifdef tilde_glitch
    write_bool   ( "hz"        , tilde_glitch                  );
#endif
#ifdef insert_null_glitch
    write_bool   ( "in"        , insert_null_glitch            );
#endif
#ifdef has_meta_key
    write_bool   ( "km"        , has_meta_key                  );
#endif
#ifdef lpi_changes_res
    write_bool   ( "lpix"      , lpi_changes_res               );
#endif
#ifdef prtr_silent
    write_bool   ( "mc5i"      , prtr_silent                   );
#endif
#ifdef move_insert_mode
    write_bool   ( "mir"       , move_insert_mode              );
#endif
#ifdef move_standout_mode
    write_bool   ( "msgr"      , move_standout_mode            );
#endif
#ifdef non_dest_scroll_region
    write_bool   ( "ndscr"     , non_dest_scroll_region        );
#endif
#ifdef no_pad_char
    write_bool   ( "npc"       , no_pad_char                   );
#endif
#ifdef non_rev_rmcup
    write_bool   ( "nrrmc"     , non_rev_rmcup                 );
#endif
#ifdef needs_xon_xoff
    write_bool   ( "nxon"      , needs_xon_xoff                );
#endif
#ifdef over_strike
    write_bool   ( "os"        , over_strike                   );
#endif
#ifdef semi_auto_right_margin
    write_bool   ( "sam"       , semi_auto_right_margin        );
#endif
#ifdef transparent_underline
    write_bool   ( "ul"        , transparent_underline         );
#endif
#ifdef eat_newline_glitch
    write_bool   ( "xenl"      , eat_newline_glitch            );
#endif
#ifdef ceol_standout_glitch
    write_bool   ( "xhp"       , ceol_standout_glitch          );
#endif
#ifdef col_addr_glitch
    write_bool   ( "xhpa"      , col_addr_glitch               );
#endif
#ifdef xon_xoff
    write_bool   ( "xon"       , xon_xoff                      );
#endif
#ifdef no_esc_ctlc
    write_bool   ( "xsb"       , no_esc_ctlc                   );
#endif
#ifdef dest_tabs_magic_smso
    write_bool   ( "xt"        , dest_tabs_magic_smso          );
#endif
#ifdef row_addr_glitch
    write_bool   ( "xvpa"      , row_addr_glitch               );
#endif

    /*
     *  Numbers
     */

#ifdef bit_image_entwining
    write_number ( "bitwin"    , bit_image_entwining           );
#endif
#ifdef bit_image_type
    write_number ( "bitype"    , bit_image_type                );
#endif
#ifdef buttons
    write_number ( "btns"      , buttons                       );
#endif
#ifdef buffer_capacity
    write_number ( "bufsz"     , buffer_capacity               );
#endif
#ifdef max_colors
    write_number ( "colors"    , max_colors                    );
#endif
#ifdef columns
    write_number ( "cols"      , columns                       );
#endif
#ifdef print_rate
    write_number ( "cps"       , print_rate                    );
#endif
#ifdef init_tabs
    write_number ( "it"        , init_tabs                     );
#endif
#ifdef label_height
    write_number ( "lh"        , label_height                  );
#endif
#ifdef lines
    write_number ( "lines"     , lines                         );
#endif
#ifdef lines_of_memory
    write_number ( "lm"        , lines_of_memory               );
#endif
#ifdef label_width
    write_number ( "lw"        , label_width                   );
#endif
#ifdef max_attributes
    write_number ( "ma"        , max_attributes                );
#endif
#ifdef max_micro_address
    write_number ( "maddr"     , max_micro_address             );
#endif
#ifdef micro_char_size
    write_number ( "mcs"       , micro_char_size               );
#endif
#ifdef max_micro_jump
    write_number ( "mjump"     , max_micro_jump                );
#endif
#ifdef micro_line_size
    write_number ( "mls"       , micro_line_size               );
#endif
#ifdef no_color_video
    write_number ( "ncv"       , no_color_video                );
#endif
#ifdef num_labels
    write_number ( "nlab"      , num_labels                    );
#endif
#ifdef number_of_pins
    write_number ( "npins"     , number_of_pins                );
#endif
#ifdef output_res_char
    write_number ( "orc"       , output_res_char               );
#endif
#ifdef output_res_horz_inch
    write_number ( "orhi"      , output_res_horz_inch          );
#endif
#ifdef output_res_line
    write_number ( "orl"       , output_res_line               );
#endif
#ifdef output_res_vert_inch
    write_number ( "orvi"      , output_res_vert_inch          );
#endif
#ifdef max_pairs
    write_number ( "pairs"     , max_pairs                     );
#endif
#ifdef padding_baud_rate
    write_number ( "pb"        , padding_baud_rate             );
#endif
#ifdef dot_horz_spacing
    write_number ( "spinh"     , dot_horz_spacing              );
#endif
#ifdef dot_vert_spacing
    write_number ( "spinv"     , dot_vert_spacing              );
#endif
#ifdef virtual_terminal
    write_number ( "vt"        , virtual_terminal              );
#endif
#ifdef wide_char_size
    write_number ( "widcs"     , wide_char_size                );
#endif
#ifdef maximum_windows
    write_number ( "wnum"      , maximum_windows               );
#endif
#ifdef width_status_line
    write_number ( "wsl"       , width_status_line             );
#endif
#ifdef magic_cookie_glitch
    write_number ( "xmc"       , magic_cookie_glitch           );
#endif

    /*
     *  Strings
     */

#ifdef acs_chars
    write_string ( "acsc"      , acs_chars                     );
#endif
#ifdef box_attr_1
    write_string ( "batt1"     , box_attr_1                    );
#endif
#ifdef box_attr_2
    write_string ( "batt2"     , box_attr_2                    );
#endif
#ifdef bell
    write_string ( "bel"       , bell                          );
#endif
#ifdef bit_image_carriage_return
    write_string ( "bicr"      , bit_image_carriage_return     );
#endif
#ifdef bit_image_newline
    write_string ( "binel"     , bit_image_newline             );
#endif
#ifdef bit_image_repeat
    write_string ( "birep"     , bit_image_repeat              );
#endif
#ifdef enter_blink_mode
    write_string ( "blink"     , enter_blink_mode              );
#endif
#ifdef enter_bold_mode
    write_string ( "bold"      , enter_bold_mode               );
#endif
#ifdef box_chars_1
    write_string ( "box1"      , box_chars_1                   );
#endif
#ifdef box_chars_2
    write_string ( "box2"      , box_chars_2                   );
#endif
#ifdef back_tab
    write_string ( "cbt"       , back_tab                      );
#endif
#ifdef change_res_horz
    write_string ( "chr"       , change_res_horz               );
#endif
#ifdef cursor_invisible
    write_string ( "civis"     , cursor_invisible              );
#endif
#ifdef clear_screen
    write_string ( "clear"     , clear_screen                  );
#endif
#ifdef command_character
    write_string ( "cmdch"     , command_character             );
#endif
#ifdef cursor_normal
    write_string ( "cnorm"     , cursor_normal                 );
#endif
#ifdef color_names
    write_string ( "colornm"   , color_names                   );
#endif
#ifdef change_char_pitch
    write_string ( "cpi"       , change_char_pitch             );
#endif
#ifdef carriage_return
    write_string ( "cr"        , carriage_return               );
#endif
#ifdef code_set_init
    write_string ( "csin"      , code_set_init                 );
#endif
#ifdef char_set_names
    write_string ( "csnm"      , char_set_names                );
#endif
#ifdef change_scroll_region
    write_string ( "csr"       , change_scroll_region          );
#endif
#ifdef parm_left_cursor
    write_string ( "cub"       , parm_left_cursor              );
#endif
#ifdef cursor_left
    write_string ( "cub1"      , cursor_left                   );
#endif
#ifdef parm_down_cursor
    write_string ( "cud"       , parm_down_cursor              );
#endif
#ifdef cursor_down
    write_string ( "cud1"      , cursor_down                   );
#endif
#ifdef parm_right_cursor
    write_string ( "cuf"       , parm_right_cursor             );
#endif
#ifdef cursor_right
    write_string ( "cuf1"      , cursor_right                  );
#endif
#ifdef cursor_address
    write_string ( "cup"       , cursor_address                );
#endif
#ifdef parm_up_cursor
    write_string ( "cuu"       , parm_up_cursor                );
#endif
#ifdef cursor_up
    write_string ( "cuu1"      , cursor_up                     );
#endif
#ifdef change_res_vert
    write_string ( "cvr"       , change_res_vert               );
#endif
#ifdef cursor_visible
    write_string ( "cvvis"     , cursor_visible                );
#endif
#ifdef create_window
    write_string ( "cwin"      , create_window                 );
#endif
#ifdef parm_dch
    write_string ( "dch"       , parm_dch                      );
#endif
#ifdef delete_character
    write_string ( "dch1"      , delete_character              );
#endif
#ifdef display_clock
    write_string ( "dclk"      , display_clock                 );
#endif
#ifdef define_bit_image_region
    write_string ( "defbi"     , define_bit_image_region       );
#endif
#ifdef define_char
    write_string ( "defc"      , define_char                   );
#endif
#ifdef device_type
    write_string ( "devt"      , device_type                   );
#endif
#ifdef dial_phone
    write_string ( "dial"      , dial_phone                    );
#endif
#ifdef enter_dim_mode
    write_string ( "dim"       , enter_dim_mode                );
#endif
#ifdef display_pc_char
    write_string ( "dispc"     , display_pc_char               );
#endif
#ifdef parm_delete_line
    write_string ( "dl"        , parm_delete_line              );
#endif
#ifdef delete_line
    write_string ( "dl1"       , delete_line                   );
#endif
#ifdef these_cause_cr
    write_string ( "docr"      , these_cause_cr                );
#endif
#ifdef dis_status_line
    write_string ( "dsl"       , dis_status_line               );
#endif
#ifdef erase_chars
    write_string ( "ech"       , erase_chars                   );
#endif
#ifdef clr_eos
    write_string ( "ed"        , clr_eos                       );
#endif
#ifdef clr_eol
    write_string ( "el"        , clr_eol                       );
#endif
#ifdef clr_bol
    write_string ( "el1"       , clr_bol                       );
#endif
#ifdef ena_acs
    write_string ( "enacs"     , ena_acs                       );
#endif
#ifdef end_bit_image_region
    write_string ( "endbi"     , end_bit_image_region          );
#endif
#ifdef form_feed
    write_string ( "ff"        , form_feed                     );
#endif
#ifdef flash_screen
    write_string ( "flash"     , flash_screen                  );
#endif
#ifdef label_format
    write_string ( "fln"       , label_format                  );
#endif
#ifdef font_0
    write_string ( "font0"     , font_0                        );
#endif
#ifdef font_1
    write_string ( "font1"     , font_1                        );
#endif
#ifdef font_2
    write_string ( "font2"     , font_2                        );
#endif
#ifdef font_3
    write_string ( "font3"     , font_3                        );
#endif
#ifdef font_4
    write_string ( "font4"     , font_4                        );
#endif
#ifdef font_5
    write_string ( "font5"     , font_5                        );
#endif
#ifdef font_6
    write_string ( "font6"     , font_6                        );
#endif
#ifdef font_7
    write_string ( "font7"     , font_7                        );
#endif
#ifdef from_status_line
    write_string ( "fsl"       , from_status_line              );
#endif
#ifdef get_mouse
    write_string ( "getm"      , get_mouse                     );
#endif
#ifdef down_half_line
    write_string ( "hd"        , down_half_line                );
#endif
#ifdef cursor_home
    write_string ( "home"      , cursor_home                   );
#endif
#ifdef flash_hook
    write_string ( "hook"      , flash_hook                    );
#endif
#ifdef column_address
    write_string ( "hpa"       , column_address                );
#endif
#ifdef tab
    write_string ( "ht"        , tab                           );
#endif
#ifdef set_tab
    write_string ( "hts"       , set_tab                       );
#endif
#ifdef up_half_line
    write_string ( "hu"        , up_half_line                  );
#endif
#ifdef hangup
    write_string ( "hup"       , hangup                        );
#endif
#ifdef parm_ich
    write_string ( "ich"       , parm_ich                      );
#endif
#ifdef insert_character
    write_string ( "ich1"      , insert_character              );
#endif
#ifdef init_file
    write_string ( "if"        , init_file                     );
#endif
#ifdef parm_insert_line
    write_string ( "il"        , parm_insert_line              );
#endif
#ifdef insert_line
    write_string ( "il1"       , insert_line                   );
#endif
#ifdef scroll_forward
    write_string ( "ind"       , scroll_forward                );
#endif
#ifdef parm_index
    write_string ( "indn"      , parm_index                    );
#endif
#ifdef initialize_color
    write_string ( "initc"     , initialize_color              );
#endif
#ifdef initialize_pair
    write_string ( "initp"     , initialize_pair               );
#endif
#ifdef enter_secure_mode
    write_string ( "invis"     , enter_secure_mode             );
#endif
#ifdef insert_padding
    write_string ( "ip"        , insert_padding                );
#endif
#ifdef init_prog
    write_string ( "iprog"     , init_prog                     );
#endif
#ifdef init_1string
    write_string ( "is1"       , init_1string                  );
#endif
#ifdef init_2string
    write_string ( "is2"       , init_2string                  );
#endif
#ifdef init_3string
    write_string ( "is3"       , init_3string                  );
#endif
#ifdef key_sbeg
    write_string ( "kBEG"      , key_sbeg                      );
#endif
#ifdef key_scancel
    write_string ( "kCAN"      , key_scancel                   );
#endif
#ifdef key_scommand
    write_string ( "kCMD"      , key_scommand                  );
#endif
#ifdef key_scopy
    write_string ( "kCPY"      , key_scopy                     );
#endif
#ifdef key_screate
    write_string ( "kCRT"      , key_screate                   );
#endif
#ifdef key_sdc
    write_string ( "kDC"       , key_sdc                       );
#endif
#ifdef key_sdl
    write_string ( "kDL"       , key_sdl                       );
#endif
#ifdef key_send
    write_string ( "kEND"      , key_send                      );
#endif
#ifdef key_seol
    write_string ( "kEOL"      , key_seol                      );
#endif
#ifdef key_sexit
    write_string ( "kEXT"      , key_sexit                     );
#endif
#ifdef key_sfind
    write_string ( "kFND"      , key_sfind                     );
#endif
#ifdef key_shelp
    write_string ( "kHLP"      , key_shelp                     );
#endif
#ifdef key_shome
    write_string ( "kHOM"      , key_shome                     );
#endif
#ifdef key_sic
    write_string ( "kIC"       , key_sic                       );
#endif
#ifdef key_sleft
    write_string ( "kLFT"      , key_sleft                     );
#endif
#ifdef key_smove
    write_string ( "kMOV"      , key_smove                     );
#endif
#ifdef key_smessage
    write_string ( "kMSG"      , key_smessage                  );
#endif
#ifdef key_snext
    write_string ( "kNXT"      , key_snext                     );
#endif
#ifdef key_soptions
    write_string ( "kOPT"      , key_soptions                  );
#endif
#ifdef key_sprint
    write_string ( "kPRT"      , key_sprint                    );
#endif
#ifdef key_sprevious
    write_string ( "kPRV"      , key_sprevious                 );
#endif
#ifdef key_sredo
    write_string ( "kRDO"      , key_sredo                     );
#endif
#ifdef key_srsume
    write_string ( "kRES"      , key_srsume                    );
#endif
#ifdef key_sright
    write_string ( "kRIT"      , key_sright                    );
#endif
#ifdef key_sreplace
    write_string ( "kRPL"      , key_sreplace                  );
#endif
#ifdef key_ssave
    write_string ( "kSAV"      , key_ssave                     );
#endif
#ifdef key_ssuspend
    write_string ( "kSPD"      , key_ssuspend                  );
#endif
#ifdef key_sundo
    write_string ( "kUND"      , key_sundo                     );
#endif
#ifdef key_a1
    write_string ( "ka1"       , key_a1                        );
#endif
#ifdef key_a3
    write_string ( "ka3"       , key_a3                        );
#endif
#ifdef key_b2
    write_string ( "kb2"       , key_b2                        );
#endif
#ifdef key_beg
    write_string ( "kbeg"      , key_beg                       );
#endif
#ifdef key_backspace
    write_string ( "kbs"       , key_backspace                 );
#endif
#ifdef key_c1
    write_string ( "kc1"       , key_c1                        );
#endif
#ifdef key_c3
    write_string ( "kc3"       , key_c3                        );
#endif
#ifdef key_cancel
    write_string ( "kcan"      , key_cancel                    );
#endif
#ifdef key_btab
    write_string ( "kcbt"      , key_btab                      );
#endif
#ifdef key_close
    write_string ( "kclo"      , key_close                     );
#endif
#ifdef key_clear
    write_string ( "kclr"      , key_clear                     );
#endif
#ifdef key_command
    write_string ( "kcmd"      , key_command                   );
#endif
#ifdef key_copy
    write_string ( "kcpy"      , key_copy                      );
#endif
#ifdef key_create
    write_string ( "kcrt"      , key_create                    );
#endif
#ifdef key_ctab
    write_string ( "kctab"     , key_ctab                      );
#endif
#ifdef key_left
    write_string ( "kcub1"     , key_left                      );
#endif
#ifdef key_down
    write_string ( "kcud1"     , key_down                      );
#endif
#ifdef key_right
    write_string ( "kcuf1"     , key_right                     );
#endif
#ifdef key_up
    write_string ( "kcuu1"     , key_up                        );
#endif
#ifdef key_dc
    write_string ( "kdch1"     , key_dc                        );
#endif
#ifdef key_dl
    write_string ( "kdl1"      , key_dl                        );
#endif
#ifdef key_eos
    write_string ( "ked"       , key_eos                       );
#endif
#ifdef key_eol
    write_string ( "kel"       , key_eol                       );
#endif
#ifdef key_end
    write_string ( "kend"      , key_end                       );
#endif
#ifdef key_enter
    write_string ( "kent"      , key_enter                     );
#endif
#ifdef key_exit
    write_string ( "kext"      , key_exit                      );
#endif
#ifdef key_f0
    write_string ( "kf0"       , key_f0                        );
#endif
#ifdef key_f1
    write_string ( "kf1"       , key_f1                        );
#endif
#ifdef key_f10
    write_string ( "kf10"      , key_f10                       );
#endif
#ifdef key_f11
    write_string ( "kf11"      , key_f11                       );
#endif
#ifdef key_f12
    write_string ( "kf12"      , key_f12                       );
#endif
#ifdef key_f13
    write_string ( "kf13"      , key_f13                       );
#endif
#ifdef key_f14
    write_string ( "kf14"      , key_f14                       );
#endif
#ifdef key_f15
    write_string ( "kf15"      , key_f15                       );
#endif
#ifdef key_f16
    write_string ( "kf16"      , key_f16                       );
#endif
#ifdef key_f17
    write_string ( "kf17"      , key_f17                       );
#endif
#ifdef key_f18
    write_string ( "kf18"      , key_f18                       );
#endif
#ifdef key_f19
    write_string ( "kf19"      , key_f19                       );
#endif
#ifdef key_f2
    write_string ( "kf2"       , key_f2                        );
#endif
#ifdef key_f20
    write_string ( "kf20"      , key_f20                       );
#endif
#ifdef key_f21
    write_string ( "kf21"      , key_f21                       );
#endif
#ifdef key_f22
    write_string ( "kf22"      , key_f22                       );
#endif
#ifdef key_f23
    write_string ( "kf23"      , key_f23                       );
#endif
#ifdef key_f24
    write_string ( "kf24"      , key_f24                       );
#endif
#ifdef key_f25
    write_string ( "kf25"      , key_f25                       );
#endif
#ifdef key_f26
    write_string ( "kf26"      , key_f26                       );
#endif
#ifdef key_f27
    write_string ( "kf27"      , key_f27                       );
#endif
#ifdef key_f28
    write_string ( "kf28"      , key_f28                       );
#endif
#ifdef key_f29
    write_string ( "kf29"      , key_f29                       );
#endif
#ifdef key_f3
    write_string ( "kf3"       , key_f3                        );
#endif
#ifdef key_f30
    write_string ( "kf30"      , key_f30                       );
#endif
#ifdef key_f31
    write_string ( "kf31"      , key_f31                       );
#endif
#ifdef key_f32
    write_string ( "kf32"      , key_f32                       );
#endif
#ifdef key_f33
    write_string ( "kf33"      , key_f33                       );
#endif
#ifdef key_f34
    write_string ( "kf34"      , key_f34                       );
#endif
#ifdef key_f35
    write_string ( "kf35"      , key_f35                       );
#endif
#ifdef key_f36
    write_string ( "kf36"      , key_f36                       );
#endif
#ifdef key_f37
    write_string ( "kf37"      , key_f37                       );
#endif
#ifdef key_f38
    write_string ( "kf38"      , key_f38                       );
#endif
#ifdef key_f39
    write_string ( "kf39"      , key_f39                       );
#endif
#ifdef key_f4
    write_string ( "kf4"       , key_f4                        );
#endif
#ifdef key_f40
    write_string ( "kf40"      , key_f40                       );
#endif
#ifdef key_f41
    write_string ( "kf41"      , key_f41                       );
#endif
#ifdef key_f42
    write_string ( "kf42"      , key_f42                       );
#endif
#ifdef key_f43
    write_string ( "kf43"      , key_f43                       );
#endif
#ifdef key_f44
    write_string ( "kf44"      , key_f44                       );
#endif
#ifdef key_f45
    write_string ( "kf45"      , key_f45                       );
#endif
#ifdef key_f46
    write_string ( "kf46"      , key_f46                       );
#endif
#ifdef key_f47
    write_string ( "kf47"      , key_f47                       );
#endif
#ifdef key_f48
    write_string ( "kf48"      , key_f48                       );
#endif
#ifdef key_f49
    write_string ( "kf49"      , key_f49                       );
#endif
#ifdef key_f5
    write_string ( "kf5"       , key_f5                        );
#endif
#ifdef key_f50
    write_string ( "kf50"      , key_f50                       );
#endif
#ifdef key_f51
    write_string ( "kf51"      , key_f51                       );
#endif
#ifdef key_f52
    write_string ( "kf52"      , key_f52                       );
#endif
#ifdef key_f53
    write_string ( "kf53"      , key_f53                       );
#endif
#ifdef key_f54
    write_string ( "kf54"      , key_f54                       );
#endif
#ifdef key_f55
    write_string ( "kf55"      , key_f55                       );
#endif
#ifdef key_f56
    write_string ( "kf56"      , key_f56                       );
#endif
#ifdef key_f57
    write_string ( "kf57"      , key_f57                       );
#endif
#ifdef key_f58
    write_string ( "kf58"      , key_f58                       );
#endif
#ifdef key_f59
    write_string ( "kf59"      , key_f59                       );
#endif
#ifdef key_f6
    write_string ( "kf6"       , key_f6                        );
#endif
#ifdef key_f60
    write_string ( "kf60"      , key_f60                       );
#endif
#ifdef key_f61
    write_string ( "kf61"      , key_f61                       );
#endif
#ifdef key_f62
    write_string ( "kf62"      , key_f62                       );
#endif
#ifdef key_f63
    write_string ( "kf63"      , key_f63                       );
#endif
#ifdef key_f7
    write_string ( "kf7"       , key_f7                        );
#endif
#ifdef key_f8
    write_string ( "kf8"       , key_f8                        );
#endif
#ifdef key_f9
    write_string ( "kf9"       , key_f9                        );
#endif
#ifdef key_find
    write_string ( "kfnd"      , key_find                      );
#endif
#ifdef key_help
    write_string ( "khlp"      , key_help                      );
#endif
#ifdef key_home
    write_string ( "khome"     , key_home                      );
#endif
#ifdef key_stab
    write_string ( "khts"      , key_stab                      );
#endif
#ifdef key_ic
    write_string ( "kich1"     , key_ic                        );
#endif
#ifdef key_il
    write_string ( "kil1"      , key_il                        );
#endif
#ifdef key_sf
    write_string ( "kind"      , key_sf                        );
#endif
#ifdef key_ll
    write_string ( "kll"       , key_ll                        );
#endif
#ifdef key_mouse
    write_string ( "kmous"     , key_mouse                     );
#endif
#ifdef key_move
    write_string ( "kmov"      , key_move                      );
#endif
#ifdef key_mark
    write_string ( "kmrk"      , key_mark                      );
#endif
#ifdef key_message
    write_string ( "kmsg"      , key_message                   );
#endif
#ifdef key_npage
    write_string ( "knp"       , key_npage                     );
#endif
#ifdef key_next
    write_string ( "knxt"      , key_next                      );
#endif
#ifdef key_open
    write_string ( "kopn"      , key_open                      );
#endif
#ifdef key_options
    write_string ( "kopt"      , key_options                   );
#endif
#ifdef key_ppage
    write_string ( "kpp"       , key_ppage                     );
#endif
#ifdef key_print
    write_string ( "kprt"      , key_print                     );
#endif
#ifdef key_previous
    write_string ( "kprv"      , key_previous                  );
#endif
#ifdef key_redo
    write_string ( "krdo"      , key_redo                      );
#endif
#ifdef key_reference
    write_string ( "kref"      , key_reference                 );
#endif
#ifdef key_resume
    write_string ( "kres"      , key_resume                    );
#endif
#ifdef key_refresh
    write_string ( "krfr"      , key_refresh                   );
#endif
#ifdef key_sr
    write_string ( "kri"       , key_sr                        );
#endif
#ifdef key_eic
    write_string ( "krmir"     , key_eic                       );
#endif
#ifdef key_replace
    write_string ( "krpl"      , key_replace                   );
#endif
#ifdef key_restart
    write_string ( "krst"      , key_restart                   );
#endif
#ifdef key_save
    write_string ( "ksav"      , key_save                      );
#endif
#ifdef key_select
    write_string ( "kslt"      , key_select                    );
#endif
#ifdef key_suspend
    write_string ( "kspd"      , key_suspend                   );
#endif
#ifdef key_catab
    write_string ( "ktbc"      , key_catab                     );
#endif
#ifdef key_undo
    write_string ( "kund"      , key_undo                      );
#endif
#ifdef lab_f0
    write_string ( "lf0"       , lab_f0                        );
#endif
#ifdef lab_f1
    write_string ( "lf1"       , lab_f1                        );
#endif
#ifdef lab_f10
    write_string ( "lf10"      , lab_f10                       );
#endif
#ifdef lab_f2
    write_string ( "lf2"       , lab_f2                        );
#endif
#ifdef lab_f3
    write_string ( "lf3"       , lab_f3                        );
#endif
#ifdef lab_f4
    write_string ( "lf4"       , lab_f4                        );
#endif
#ifdef lab_f5
    write_string ( "lf5"       , lab_f5                        );
#endif
#ifdef lab_f6
    write_string ( "lf6"       , lab_f6                        );
#endif
#ifdef lab_f7
    write_string ( "lf7"       , lab_f7                        );
#endif
#ifdef lab_f8
    write_string ( "lf8"       , lab_f8                        );
#endif
#ifdef lab_f9
    write_string ( "lf9"       , lab_f9                        );
#endif
#ifdef cursor_to_ll
    write_string ( "ll"        , cursor_to_ll                  );
#endif
#ifdef change_line_pitch
    write_string ( "lpi"       , change_line_pitch             );
#endif
#ifdef print_screen
    write_string ( "mc0"       , print_screen                  );
#endif
#ifdef prtr_off
    write_string ( "mc4"       , prtr_off                      );
#endif
#ifdef prtr_on
    write_string ( "mc5"       , prtr_on                       );
#endif
#ifdef prtr_non
    write_string ( "mc5p"      , prtr_non                      );
#endif
#ifdef parm_left_micro
    write_string ( "mcub"      , parm_left_micro               );
#endif
#ifdef micro_left
    write_string ( "mcub1"     , micro_left                    );
#endif
#ifdef parm_down_micro
    write_string ( "mcud"      , parm_down_micro               );
#endif
#ifdef micro_down
    write_string ( "mcud1"     , micro_down                    );
#endif
#ifdef parm_right_micro
    write_string ( "mcuf"      , parm_right_micro              );
#endif
#ifdef micro_right
    write_string ( "mcuf1"     , micro_right                   );
#endif
#ifdef parm_up_micro
    write_string ( "mcuu"      , parm_up_micro                 );
#endif
#ifdef micro_up
    write_string ( "mcuu1"     , micro_up                      );
#endif
#ifdef clear_margins
    write_string ( "mgc"       , clear_margins                 );
#endif
#ifdef micro_column_address
    write_string ( "mhpa"      , micro_column_address          );
#endif
#ifdef mouse_info
    write_string ( "minfo"     , mouse_info                    );
#endif
#ifdef cursor_mem_address
    write_string ( "mrcup"     , cursor_mem_address            );
#endif
#ifdef micro_row_address
    write_string ( "mvpa"      , micro_row_address             );
#endif
#ifdef newline
    write_string ( "nel"       , newline                       );
#endif
#ifdef orig_colors
    write_string ( "oc"        , orig_colors                   );
#endif
#ifdef orig_pair
    write_string ( "op"        , orig_pair                     );
#endif
#ifdef pad_char
    write_string ( "pad"       , pad_char                      );
#endif
#ifdef fixed_pause
    write_string ( "pause"     , fixed_pause                   );
#endif
#ifdef pc_term_options
    write_string ( "pctrm"     , pc_term_options               );
#endif
#ifdef pkey_key
    write_string ( "pfkey"     , pkey_key                      );
#endif
#ifdef pkey_local
    write_string ( "pfloc"     , pkey_local                    );
#endif
#ifdef pkey_xmit
    write_string ( "pfx"       , pkey_xmit                     );
#endif
#ifdef pkey_plab
    write_string ( "pfxl"      , pkey_plab                     );
#endif
#ifdef plab_norm
    write_string ( "pln"       , plab_norm                     );
#endif
#ifdef order_of_pins
    write_string ( "porder"    , order_of_pins                 );
#endif
#ifdef enter_protected_mode
    write_string ( "prot"      , enter_protected_mode          );
#endif
#ifdef pulse
    write_string ( "pulse"     , pulse                         );
#endif
#ifdef quick_dial
    write_string ( "qdial"     , quick_dial                    );
#endif
#ifdef stop_bit_image
    write_string ( "rbim"      , stop_bit_image                );
#endif
#ifdef restore_cursor
    write_string ( "rc"        , restore_cursor                );
#endif
#ifdef stop_char_set_def
    write_string ( "rcsd"      , stop_char_set_def             );
#endif
#ifdef repeat_char
    write_string ( "rep"       , repeat_char                   );
#endif
#ifdef req_mouse_pos
    write_string ( "reqmp"     , req_mouse_pos                 );
#endif
#ifdef enter_reverse_mode
    write_string ( "rev"       , enter_reverse_mode            );
#endif
#ifdef reset_file
    write_string ( "rf"        , reset_file                    );
#endif
#ifdef req_for_input
    write_string ( "rfi"       , req_for_input                 );
#endif
#ifdef scroll_reverse
    write_string ( "ri"        , scroll_reverse                );
#endif
#ifdef parm_rindex
    write_string ( "rin"       , parm_rindex                   );
#endif
#ifdef exit_italics_mode
    write_string ( "ritm"      , exit_italics_mode             );
#endif
#ifdef exit_leftward_mode
    write_string ( "rlm"       , exit_leftward_mode            );
#endif
#ifdef exit_alt_charset_mode
    write_string ( "rmacs"     , exit_alt_charset_mode         );
#endif
#ifdef exit_am_mode
    write_string ( "rmam"      , exit_am_mode                  );
#endif
#ifdef remove_clock
    write_string ( "rmclk"     , remove_clock                  );
#endif
#ifdef exit_ca_mode
    write_string ( "rmcup"     , exit_ca_mode                  );
#endif
#ifdef exit_delete_mode
    write_string ( "rmdc"      , exit_delete_mode              );
#endif
#ifdef exit_micro_mode
    write_string ( "rmicm"     , exit_micro_mode               );
#endif
#ifdef exit_insert_mode
    write_string ( "rmir"      , exit_insert_mode              );
#endif
#ifdef keypad_local
    write_string ( "rmkx"      , keypad_local                  );
#endif
#ifdef label_off
    write_string ( "rmln"      , label_off                     );
#endif
#ifdef meta_off
    write_string ( "rmm"       , meta_off                      );
#endif
#ifdef char_padding
    write_string ( "rmp"       , char_padding                  );
#endif
#ifdef exit_pc_charset_mode
    write_string ( "rmpch"     , exit_pc_charset_mode          );
#endif
#ifdef exit_scancode_mode
    write_string ( "rmsc"      , exit_scancode_mode            );
#endif
#ifdef exit_standout_mode
    write_string ( "rmso"      , exit_standout_mode            );
#endif
#ifdef exit_underline_mode
    write_string ( "rmul"      , exit_underline_mode           );
#endif
#ifdef exit_xon_mode
    write_string ( "rmxon"     , exit_xon_mode                 );
#endif
#ifdef reset_1string
    write_string ( "rs1"       , reset_1string                 );
#endif
#ifdef reset_2string
    write_string ( "rs2"       , reset_2string                 );
#endif
#ifdef reset_3string
    write_string ( "rs3"       , reset_3string                 );
#endif
#ifdef exit_shadow_mode
    write_string ( "rshm"      , exit_shadow_mode              );
#endif
#ifdef exit_subscript_mode
    write_string ( "rsubm"     , exit_subscript_mode           );
#endif
#ifdef exit_superscript_mode
    write_string ( "rsupm"     , exit_superscript_mode         );
#endif
#ifdef exit_upward_mode
    write_string ( "rum"       , exit_upward_mode              );
#endif
#ifdef exit_doublewide_mode
    write_string ( "rwidm"     , exit_doublewide_mode          );
#endif
#ifdef set0_des_seq
    write_string ( "s0ds"      , set0_des_seq                  );
#endif
#ifdef set1_des_seq
    write_string ( "s1ds"      , set1_des_seq                  );
#endif
#ifdef set2_des_seq
    write_string ( "s2ds"      , set2_des_seq                  );
#endif
#ifdef set3_des_seq
    write_string ( "s3ds"      , set3_des_seq                  );
#endif
#ifdef start_bit_image
    write_string ( "sbim"      , start_bit_image               );
#endif
#ifdef save_cursor
    write_string ( "sc"        , save_cursor                   );
#endif
#ifdef alt_scancode_esc
    write_string ( "scesa"     , alt_scancode_esc              );
#endif
#ifdef scancode_escape
    write_string ( "scesc"     , scancode_escape               );
#endif
#ifdef set_clock
    write_string ( "sclk"      , set_clock                     );
#endif
#ifdef set_color_pair
    write_string ( "scp"       , set_color_pair                );
#endif
#ifdef select_char_set
    write_string ( "scs"       , select_char_set               );
#endif
#ifdef start_char_set_def
    write_string ( "scsd"      , start_char_set_def            );
#endif
#ifdef enter_draft_quality
    write_string ( "sdrfq"     , enter_draft_quality           );
#endif
#ifdef set_a_background
    write_string ( "setab"     , set_a_background              );
#endif
#ifdef set_a_foreground
    write_string ( "setaf"     , set_a_foreground              );
#endif
#ifdef set_background
    write_string ( "setb"      , set_background                );
#endif
#ifdef set_color_band
    write_string ( "setcolor"  , set_color_band                );
#endif
#ifdef set_foreground
    write_string ( "setf"      , set_foreground                );
#endif
#ifdef set_attributes
    write_string ( "sgr"       , set_attributes                );
#endif
#ifdef exit_attribute_mode
    write_string ( "sgr0"      , exit_attribute_mode           );
#endif
#ifdef enter_italics_mode
    write_string ( "sitm"      , enter_italics_mode            );
#endif
#ifdef set_page_length
    write_string ( "slines"    , set_page_length               );
#endif
#ifdef enter_leftward_mode
    write_string ( "slm"       , enter_leftward_mode           );
#endif
#ifdef enter_alt_charset_mode
    write_string ( "smacs"     , enter_alt_charset_mode        );
#endif
#ifdef enter_am_mode
    write_string ( "smam"      , enter_am_mode                 );
#endif
#ifdef enter_ca_mode
    write_string ( "smcup"     , enter_ca_mode                 );
#endif
#ifdef enter_delete_mode
    write_string ( "smdc"      , enter_delete_mode             );
#endif
#ifdef set_bottom_margin
    write_string ( "smgb"      , set_bottom_margin             );
#endif
#ifdef set_bottom_margin_parm
    write_string ( "smgbp"     , set_bottom_margin_parm        );
#endif
#ifdef set_left_margin
    write_string ( "smgl"      , set_left_margin               );
#endif
#ifdef set_left_margin_parm
    write_string ( "smglp"     , set_left_margin_parm          );
#endif
#ifdef set_lr_margin
    write_string ( "smglr"     , set_lr_margin                 );
#endif
#ifdef set_right_margin
    write_string ( "smgr"      , set_right_margin              );
#endif
#ifdef set_right_margin_parm
    write_string ( "smgrp"     , set_right_margin_parm         );
#endif
#ifdef set_top_margin
    write_string ( "smgt"      , set_top_margin                );
#endif
#ifdef set_tb_margin
    write_string ( "smgtb"     , set_tb_margin                 );
#endif
#ifdef set_top_margin_parm
    write_string ( "smgtp"     , set_top_margin_parm           );
#endif
#ifdef enter_micro_mode
    write_string ( "smicm"     , enter_micro_mode              );
#endif
#ifdef enter_insert_mode
    write_string ( "smir"      , enter_insert_mode             );
#endif
#ifdef keypad_xmit
    write_string ( "smkx"      , keypad_xmit                   );
#endif
#ifdef label_on
    write_string ( "smln"      , label_on                      );
#endif
#ifdef meta_on
    write_string ( "smm"       , meta_on                       );
#endif
#ifdef enter_pc_charset_mode
    write_string ( "smpch"     , enter_pc_charset_mode         );
#endif
#ifdef enter_scancode_mode
    write_string ( "smsc"      , enter_scancode_mode           );
#endif
#ifdef enter_standout_mode
    write_string ( "smso"      , enter_standout_mode           );
#endif
#ifdef enter_underline_mode
    write_string ( "smul"      , enter_underline_mode          );
#endif
#ifdef enter_xon_mode
    write_string ( "smxon"     , enter_xon_mode                );
#endif
#ifdef enter_near_letter_quality
    write_string ( "snlq"      , enter_near_letter_quality     );
#endif
#ifdef enter_normal_quality
    write_string ( "snrmq"     , enter_normal_quality          );
#endif
#ifdef enter_shadow_mode
    write_string ( "sshm"      , enter_shadow_mode             );
#endif
#ifdef enter_subscript_mode
    write_string ( "ssubm"     , enter_subscript_mode          );
#endif
#ifdef enter_superscript_mode
    write_string ( "ssupm"     , enter_superscript_mode        );
#endif
#ifdef subscript_characters
    write_string ( "subcs"     , subscript_characters          );
#endif
#ifdef enter_upward_mode
    write_string ( "sum"       , enter_upward_mode             );
#endif
#ifdef superscript_characters
    write_string ( "supcs"     , superscript_characters        );
#endif
#ifdef enter_doublewide_mode
    write_string ( "swidm"     , enter_doublewide_mode         );
#endif
#ifdef clear_all_tabs
    write_string ( "tbc"       , clear_all_tabs                );
#endif
#ifdef tone
    write_string ( "tone"      , tone                          );
#endif
#ifdef to_status_line
    write_string ( "tsl"       , to_status_line                );
#endif
#ifdef user0
    write_string ( "u0"        , user0                         );
#endif
#ifdef user1
    write_string ( "u1"        , user1                         );
#endif
#ifdef user2
    write_string ( "u2"        , user2                         );
#endif
#ifdef user3
    write_string ( "u3"        , user3                         );
#endif
#ifdef user4
    write_string ( "u4"        , user4                         );
#endif
#ifdef user5
    write_string ( "u5"        , user5                         );
#endif
#ifdef user6
    write_string ( "u6"        , user6                         );
#endif
#ifdef user7
    write_string ( "u7"        , user7                         );
#endif
#ifdef user8
    write_string ( "u8"        , user8                         );
#endif
#ifdef user9
    write_string ( "u9"        , user9                         );
#endif
#ifdef underline_char
    write_string ( "uc"        , underline_char                );
#endif
#ifdef row_address
    write_string ( "vpa"       , row_address                   );
#endif
#ifdef wait_tone
    write_string ( "wait"      , wait_tone                     );
#endif
#ifdef set_window
    write_string ( "wind"      , set_window                    );
#endif
#ifdef goto_window
    write_string ( "wingo"     , goto_window                   );
#endif
#ifdef xoff_character
    write_string ( "xoffc"     , xoff_character                );
#endif
#ifdef xon_character
    write_string ( "xonc"      , xon_character                 );
#endif
#ifdef zero_motion
    write_string ( "zerom"     , zero_motion                   );
#endif
}

/*==========================================================================*/

static	void	write_bool ( capname , var )

char			*capname ;
char			var ;

{
    if ( var )
    {
	(void) fprintf ( tempfp , "\t%s,\n", capname );
    }
}

/*==========================================================================*/

static	void	write_number ( capname , var )

char			*capname ;
int			var ;

{
    if ( var >= 0 )
    {
	(void) fprintf ( tempfp , "\t%s#%d,\n", capname , var );
    }
}

/*==========================================================================*/

static	void	write_string ( capname , var )

char			*capname ;
char			*var ;

{
    register int			function ;
    register struct capname_info	* cip ;
    register struct key_info		* kip ;


    /*
     *  IF this capability name is one of our key names,
     *  use its sequence.
     */
    for ( cip = sql04_cap_table ; cip->ci_function ; cip ++ )
    {
	if ( !strcmp(cip->ci_capname , capname) ) break ;
    }
    if ( cip->ci_function )
    {
	function = cip->ci_function ;
	for ( kip = sql04_key_list ; kip ; kip = kip->ki_next )
	    if ( kip->ki_function == function ) break ;
	if ( kip )
	    var = kip->ki_sequence ;
	else
	    var = 0 ;
    }

    if ( var && *var )
    {
	(void) fprintf ( tempfp , "\t%s=%s,\n", capname ,
						sql04_put_sequence(var) );
    }
}

/*==========================================================================*/

static	SIGNALFUNCTYPE	catch_alarm ( )

{
    (void) signal ( SIGALRM , (SIGNALFUNCTYPE(*)()) catch_alarm );
}

/*==========================================================================*/

static	SIGNALFUNCTYPE	catch_signal ( )

{
    char			*ptr ;


    close_session ();
    remove_temp_file ();
    ptr = "maketi canceled, no output written \n";
    (void) write ( 2 , ptr , (unsigned) strlen(ptr) );
    exit ( 1 );
}

/*==========================================================================*/

static	int	create_temp_file ( )

{
    unsigned			dirlgt ;
    char			*ptr ;


    /*
     *  Locate the end of the directory path name in 'output_file'.
     */
    ptr = strrchr ( output_file , '/' );
    if ( ! ptr ) ptr = output_file ;
    if ( *ptr == '/' ) ptr ++ ;
    dirlgt = (unsigned) ( ptr - output_file );

    if ( ALLOC_MEM_EO57( (void **)&temp_file, dirlgt + 32 ) != NO_ERROR_EO001 )
    {
    	return ( -3 );
    }

    /*
     *  If there is any directory path name included in 'output_file',
     *  then 'dirlgt' includes the terminating '/'.
     */
    spri ( temp_file , "%.*sMAKETI.%ld" , dirlgt , output_file ,
						(long) getpid() );

    tempfp = fopen ( temp_file , "w" );
    if ( ! tempfp )
    {
    	MSG (( "cannot open temp file '%s': %s \n", temp_file , sqlerrs() ))
	    return ( -2 );
    }

    return ( 1 );
}

/*==========================================================================*/

static	void	remove_temp_file ( )

{
    if ( temp_file ) (void) unlink ( temp_file );
}

/*==========================================================================*/

#undef MF__
#define MF__ MOD__"free_key_list"
static	void	free_key_list ( )

{
    struct key_info		* kip ;


    DBGIN;

    while ( sql04_key_list )
    {
    	kip = sql04_key_list ;
	    sql04_key_list = kip->ki_next ;
	    FREE_MEM_EO57 ( kip );
    }
    DBGOUT;
}

/*==========================================================================*/

static	void	set_attrib ( attribute )

int			attribute ;

{
    switch ( attribute )
    {
    default :
    case ATTR_NORMAL :
#if A_COLOR && !defined(A_BGCOLOR)
	if ( has_colors() )
	    (void) attrset ( COLOR_PAIR(1) );
	else
#endif
	    (void) attrset ( A_NORMAL );
	break ;

    case ATTR_REVERSE :
#if A_COLOR && !defined(A_BGCOLOR)
	if ( has_colors() )
	    (void) attron ( COLOR_PAIR(2) );
	else
#endif
	    (void) attron ( A_REVERSE );
	break ;

    case ATTR_MESSAGE :
#if A_COLOR && !defined(A_BGCOLOR)
	if ( has_colors() )
	    (void) attron ( COLOR_PAIR(3) );
#endif
	(void) attron ( A_STANDOUT );
	break ;
    }
}

/*==========================================================================*/

/**VARAGRS1*/
/*PRINTFLIKE1*/
static	void	msg ( char *text , ... )
{
    int				lgt ;
    va_list			args ;
    char			line [ 256 ];


    if ( *text )
    {
	va_start ( args, text );
	(void) vsprintf ( line , text , args );
	va_end ( args );
	lgt = strlen ( line );
    }
    else
	lgt = 0 ; /* clear message line, if in_curses */

    if ( in_curses )
    {
	if ( lgt > COLS-3 ) lgt = COLS-3 ;
	while ( (lgt > 0) && (line[lgt-1] == '\n') ) lgt -- ;
	line[lgt] = 0 ;

	fill_line ( LINES-1 , (chtype) ' ' );
	(void) move ( LINES-1 , 1 );
	if ( lgt > 0 )
	{
	    set_attrib ( ATTR_MESSAGE );
	    (void) addch ( ' ' );
	    (void) addstr ( line );
	    (void) addch ( ' ' );
	    set_attrib ( ATTR_NORMAL );
	    (void) beep ();
	    (void) flash ();
	    message_on = TRUE ;
	}
	else
	    message_on = FALSE ;
	(void) refresh ();
    }
    else
    {
	if ( lgt > 0 ) (void) write ( 2 , line , (unsigned) lgt );
    }
}
