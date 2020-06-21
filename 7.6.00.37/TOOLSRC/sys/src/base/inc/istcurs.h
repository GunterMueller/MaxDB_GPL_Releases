/*	istcurs.h	

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

#ifndef	_CURSES_DEFINITIONS
#define	_CURSES_DEFINITIONS

/* defined in curses.h */
#undef          OK
#undef          ERR
#undef          TRUE
#undef          FALSE

/*==========================================================================*/
#if unix
/*==========================================================================*/

#ifdef	ULTRIX
#  include	<cursesX.h>
#else
#  include	<curses.h>
#endif
#include	<term.h>

/*
 *  Targon/3300 seems to have the full color feature except of 'start_color'.
 *  Omitting only this call leaves the screen in black/white...
 *  Thus it should enhance the performance to exclude colors at all.
 */
#ifdef	T33
#undef	A_COLOR
#endif
/*
 *  A_COLOR must be present for later use.
 *  A true color curses defines this as non-zero.
 */
#ifndef	A_COLOR
#define	A_COLOR			0
#endif

/*
 *  On some systems the colors are incorrectly defined.
 *  (RED (1) and BLUE (4) exchanged, YELLOW (3) and CYAN (6) exchanged)
 */
#if A_COLOR
#undef  COLOR_BLACK
#undef  COLOR_RED
#undef  COLOR_GREEN
#undef  COLOR_YELLOW
#undef  COLOR_BLUE
#undef  COLOR_MAGENTA
#undef  COLOR_CYAN
#undef  COLOR_WHITE

#define COLOR_BLACK	0
#define COLOR_RED	1
#define COLOR_GREEN	2
#define COLOR_YELLOW	3
#define COLOR_BLUE	4
#define COLOR_MAGENTA	5
#define COLOR_CYAN	6
#define COLOR_WHITE	7
#endif /*A_COLOR*/

/*
 *  Some definitions which are not covered by all curses implementations.
 */
#ifndef	key_btab
#define	key_btab	((char*)0)
#endif
#ifndef	key_cancel
#define	key_cancel	((char*)0)
#endif
#ifndef	key_command
#define	key_command	((char*)0)
#endif
#ifndef	key_copy
#define	key_copy	((char*)0)
#endif
#ifndef	key_end
#define	key_end		((char*)0)
#endif
#ifndef	key_enter
#define	key_enter	((char*)0)
#endif
#ifndef	key_exit
#define	key_exit	((char*)0)
#endif
#ifndef	key_f11
#define	key_f11		((char*)0)
#endif
#ifndef	key_f12
#define	key_f12		((char*)0)
#endif
#ifndef	key_find
#define	key_find	((char*)0)
#endif
#ifndef	key_help
#define	key_help	((char*)0)
#endif
#ifndef	key_mark
#define	key_mark	((char*)0)
#endif
#ifndef	key_move
#define	key_move	((char*)0)
#endif
#ifndef	key_next
#define	key_next	((char*)0)
#endif
#ifndef	key_previous
#define	key_previous	((char*)0)
#endif
#ifndef	key_print
#define	key_print	((char*)0)
#endif
#ifndef	key_refresh
#define	key_refresh	((char*)0)
#endif
#ifndef	key_replace
#define	key_replace	((char*)0)
#endif
#ifndef	key_scopy
#define	key_scopy	((char*)0)
#endif
#ifndef	key_select
#define	key_select	((char*)0)
#endif
#ifndef	key_sleft
#define	key_sleft	((char*)0)
#endif
#ifndef	key_smove
#define	key_smove	((char*)0)
#endif
#ifndef	key_snext
#define	key_snext	((char*)0)
#endif
#ifndef	key_sprevious
#define	key_sprevious	((char*)0)
#endif
#ifndef	key_sright
#define	key_sright	((char*)0)
#endif
#ifndef	key_undo
#define	key_undo	((char*)0)
#endif

/*==========================================================================*/
#else /*unix*/
/*==========================================================================*/
#if MSDOS
/*==========================================================================*/

/*
 *  This replaces <curses.h> for MSDOS.
 */

/*
 *  <curses.h> includes <stdio.h>
 */
#include	<stdio.h>

typedef	unsigned long		chtype ;
typedef	void			*WINDOW ;

#define A_CHARTEXT	000000000377L	/* 0x000000FF */
#define A_ATTRIBUTES	037777600000L	/* 0xFFFF0000 */
#define A_COLOR		017600000000L	/* 0x7e000000 */

#define A_NORMAL	000000000000L	/* 0x00000000 */
#define A_STANDOUT	000000200000L	/* 0x00010000 */
#define A_UNDERLINE	000000400000L	/* 0x00020000 */
#define A_REVERSE	000001000000L	/* 0x00040000 */
#define A_BLINK		000002000000L	/* 0x00080000 */
#define A_DIM		000004000000L	/* 0x00100000 */
#define A_BOLD		000010000000L	/* 0x00200000 */
#define A_ALTCHARSET	000100000000L	/* 0x01000000 */

/* The next two are subject to change so don't depend on them */
#define A_INVIS		000020000000L
#define A_PROTECT	000040000000L

#define COLOR_PAIR(n)	(((chtype)(n)) << 25)
#define PAIR_NUMBER(n)	((int)((((chtype)(n)) & A_COLOR) >> 25))

#define	COLOR_BLACK	0
#define	COLOR_BLUE	1
#define	COLOR_GREEN	2
#define	COLOR_CYAN	3
#define	COLOR_RED	4
#define	COLOR_MAGENTA	5
#define	COLOR_YELLOW	6
#define	COLOR_WHITE	7
#define	COLOR_LIGHT	8

#define	ACS_LRCORNER	((chtype)0xd9)
#define	ACS_URCORNER	((chtype)0xbf)
#define	ACS_ULCORNER	((chtype)0xda)
#define	ACS_LLCORNER	((chtype)0xc0)
#define	ACS_PLUS	((chtype)0xc5)
#define	ACS_HLINE	((chtype)0xc4)
#define	ACS_LTEE	((chtype)0xc3)
#define	ACS_RTEE	((chtype)0xb4)
#define	ACS_BTEE	((chtype)0xc1)
#define	ACS_TTEE	((chtype)0xc2)
#define	ACS_VLINE	((chtype)0xb3)

#define	OK		(0)
#define	ERR		(-1)
#define	TRUE		(1)
#define	FALSE		(0)

/* functions from doscurs.c */

extern	WINDOW		*initscr ( void );
extern	int		endwin ( void );
extern	int		delwin ( WINDOW * );
extern	int		start_color ( void );
extern	int		has_colors ( void );
extern	int		init_pair ( short , short , short );
extern	int		cbreak ( void );
extern	int		raw ( void );
extern	int		nonl ( void );
extern	int		noecho ( void );
extern	int		meta ( WINDOW * , int );
extern	int		typeahead ( int fd );
extern	int		intrflush ( WINDOW * , int );
extern	int		move ( int , int );
extern	int		attrset ( chtype );
extern	int		attron ( chtype );
extern	int		attroff ( chtype );
extern	int		printw ();
extern	int		addstr ( char * );
extern	int		addch ( chtype );
extern	int		beep ( void );
extern	int		flash ( void );
extern	int		clear ( void );
extern	int		erase ( void );
extern	int		clrtobot ( void );
extern	int		clrtoeol ( void );
extern	int		clearok ( WINDOW * , int );
extern	int		refresh ( void );
extern	char		*tparm ( char * , long int , long int , long int ,
					  long int , long int , long int ,
					  long int , long int , long int );
extern	int		putp ( char * );

/* data from doscurs.c */

extern	int		LINES ;
extern	int		COLS ;
extern	WINDOW		*stdscr ;

extern	int		COLORS ;
extern	int		COLOR_PAIRS ;
extern	char		*tab ;
extern	char		*set_tab ;
extern	char		*back_tab ;
extern	char		*clear_all_tabs ;
extern	char		*init_prog ;
extern	char		*init_file ;
extern	char		*init_1string ;
extern	char		*init_2string ;
extern	char		*init_3string ;
extern	char		*reset_file ;
extern	char		*reset_1string ;
extern	char		*reset_2string ;
extern	char		*reset_3string ;
extern	char		*keypad_xmit ;
extern	char		*keypad_local ;
extern	char		*cursor_address ;
extern	char		*enter_alt_charset_mode ;
extern	char		*exit_alt_charset_mode ;

/*==========================================================================*/
#endif /*MSDOS*/
/*==========================================================================*/
#endif /*unix*/
/*==========================================================================*/

#ifndef TRUE
# define	TRUE		(1)
#endif
#ifndef FALSE
# define	FALSE		(0)
#endif

/* This macro allows to use undefined features, which are NULL */
#define	PUTP(x)			{if(x) (void)putp(x);}

/* functions from cursinit.c */

extern	int			enter_curses ();
extern	int			leave_curses ();

/* data from cursinit.c */

extern	chtype			graph_lrcrn ;
extern	chtype			graph_urcrn ;
extern	chtype			graph_ulcrn ;
extern	chtype			graph_llcrn ;
extern	chtype			graph_plus ;
extern	chtype			graph_hline ;
extern	chtype			graph_ltee ;
extern	chtype			graph_rtee ;
extern	chtype			graph_btee ;
extern	chtype			graph_ttee ;
extern	chtype			graph_vline ;

#endif /*_CURSES_DEFINITIONS*/

