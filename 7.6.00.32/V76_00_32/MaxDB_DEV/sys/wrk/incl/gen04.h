/*!
  @file           gen04.h
  @author         JoergM
  @special area   virtual_terminal_definitions
  @brief          Constants and Types for the Virtual Terminal
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



\endif
//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//
*/

/*==========================================================================*/
/*----------------------    from vsp00c          ---------------------------*/
/*==========================================================================*/

#include	<stdlib.h>
#include	<unistd.h>
#include	<stdio.h>
#include	<string.h>
#if !(defined SUN || defined SOLARIS)
#include	<memory.h>
#endif /* SUN || SOLARIS */
#include	<errno.h>
#include	<sys/types.h>

#define		MAXVFNLENGTH		64

#   if OSF1
#   define	CINT			size_t
#   else
#   define	CINT			int
#   endif

#define		global			/* clarifies the scope of symbols */

/*==========================================================================*/
/*----------------------    from ven00c          ---------------------------*/
/*==========================================================================*/

#include	<fcntl.h>
#include	<signal.h>
#   define	SIGNALFUNCTYPE	void

extern	char			*sqlerrs ();

#ifdef	DEBUG_RTE
#  define		LOCALDBG(x)		{sqldbgvt x;}
   extern		void		sqldbgvt ();
#else
#  define		LOCALDBG(x)		{}
#endif
#ifdef	DEBUG_DEBUG
#define	DBGG			LOCALDBG
#else
#define	DBGG(x)			{}
#endif

/*==========================================================================*/
/*---------------------       INCLUDES        ------------------------------*/
/*==========================================================================*/

#ifdef _IBMR2		/* 91-Aug-20 J.Bruehe */
#define NLS	1	/* "curses" seems generated using "NLS", */
			/* this influences the key values used */
#endif

#include	<ctype.h>

#ifdef OSF1
/*
 *  OSF1 is an exception, because it defines everything for termios
 *  but curses includes termio, which redefines VINTR,... for use with ioctl!
 *  The tricks below disable the inclusion of termio.h.
 */
# define _SYS_TERMIO_H_
# define termio termios
#endif
#ifndef FREEBSD
#include        <termio.h>
#endif

# include	<termios.h>

#if	defined(NCCS) && defined(TCSADRAIN)
# define	TERMIO_INFO		struct termios
# define	CC_SIZE			NCCS
# define	GET_TERMIO(f,t)		tcgetattr(f,t)
# define	SET_TERMIO(f,t)		tcsetattr(f,TCSADRAIN,t)
#else
# define	TERMIO_INFO		struct termio
# define	CC_SIZE			NCC
# define	GET_TERMIO(f,t)		ioctl(f,TCGETA,t)
# define	SET_TERMIO(f,t)		ioctl(f,TCSETAW,t)
#endif

/* defined in curses.h */
#undef          OK
#undef          TRUE
#undef          FALSE
#if !(defined LINUX && defined IA64)
#define bool __bool
#endif
#ifdef SCRLIB
#    ifdef NMP
#        include "/opt/xms/incl/curses.h" /* nocheck */
#        include "/opt/xms/incl/term.h" /* nocheck */
#    else
#        include <curses.h>
#        include <term.h>
#    endif /* NMP */
#else /* ! SCRLIB */
#    ifdef sun
#            include "curses.h" /* nocheck */
#            include "term.h" /* nocheck */
#    else
#            include <curses.h>
#        include <term.h>
#    endif
#endif /* ! SCRLIB */
#undef bool

#ifdef SCRLIB
#define	ACS_BTEE	ACS_BTTEE
#define	ACS_LTEE	ACS_LTTEE
#define	ACS_RTEE	ACS_RTTEE
#define	ACS_TTEE	ACS_TTTEE
#endif /*SCRLIB*/

/*
 *  A_COLOR must be present for later use.
 *  A true color curses defines this as non-zero.
 *  To differentiate between curses and screenlib,
 *  use A_BGCOLOR wich is only defined by screenlib.
 */
#ifndef	A_COLOR
#define	A_COLOR		0
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

/* This macro allows to use undefined features, which are NULL */
#define	PUTP(x)			{if(x) (void)putp(x);}

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
/*--------------             CONSTANTS                   -------------------*/
/*==========================================================================*/

/* Definition des Tasten-Arrays  ( vt_key ) */

#ifdef ENUM_ARRAY
enum
{
	VTKEY_UNKNOWN ,
	VTKEY_CHARACTER ,
	VTKEY_ENTER ,
	VTKEY_PF01 ,
	VTKEY_PF02 ,
	VTKEY_PF03 ,
	VTKEY_PF04 ,
	VTKEY_PF05 ,
	VTKEY_PF06 ,
	VTKEY_PF07 ,
	VTKEY_PF08 ,
	VTKEY_PF09 ,
	VTKEY_PF10 ,
	VTKEY_PF11 ,
	VTKEY_PF12 ,
	VTKEY_CLEAR ,
	VTKEY_DO ,
	VTKEY_SK01 ,
	VTKEY_SK02 ,
	VTKEY_SK03 ,
	VTKEY_SK04 ,
	VTKEY_SK05 ,
	VTKEY_SK06 ,
	VTKEY_SK07 ,
	VTKEY_SK08 ,
	VTKEY_SK09 ,
	VTKEY_HELP ,
	VTKEY_UP ,
	VTKEY_DOWN ,
	VTKEY_LEFT ,
	VTKEY_RIGHT ,
	VTKEY_CMD ,
	VTKEY_PICK ,
	VTKEY_PUT ,
	VTKEY_MARK ,
	VTKEY_SELECT ,
	VTKEY_MOVE ,
	VTKEY_COPY ,
	VTKEY_INSERT ,
	VTKEY_DELETE ,
	VTKEY_TOP ,
	VTKEY_BOTTOM ,
	VTKEY_CANCEL ,
	VTKEY_UNDO ,
	VTKEY_END ,
	VTKEY_ESCAPE ,
	VTKEY_FILL ,
	VTKEY_FILE_OPEN ,
	VTKEY_FILE_SAVE ,
	VTKEY_EXIT ,
	VTKEY_PRINT ,
	VTKEY_FIND ,
	VTKEY_REPLACE ,
	VTKEY_CURSOR_UP ,
	VTKEY_CURSOR_DOWN ,
	VTKEY_CURSOR_RIGHT ,
	VTKEY_CURSOR_LEFT ,
	VTKEY_CURSOR_HOME ,
	VTKEY_CURSOR_END ,
	VTKEY_NEXT_FIELD ,
	VTKEY_PREV_FIELD ,
	VTKEY_INSERT_CHAR ,
	VTKEY_DELETE_CHAR ,
	VTKEY_RUBOUT ,
	VTKEY_DEL_EOF ,
	VTKEY_MOUSE_DOWN ,
	VTKEY_MOUSE_UP ,
	VTKEY_MOUSE_MOVE ,
	VTKEY_MOUSE_DBL ,
	VTKEY_HSCROLL ,
	VTKEY_VSCROLL ,
	VTKEY_CONTROL ,
	VTKEY_CURSOR_BOL ,
	VTKEY_CURSOR_EOL ,
	VTKEY_LAST ,		/* enter new keys before this one */
	/* internal key definitions */
	VTKEY_TAB ,
	VTKEY_BACK_TAB ,
	VTKEY_REFRESH
};
#else	/*!ENUM_ARRAY*/
/**/
#define VTKEY_UNKNOWN             0
#define VTKEY_CHARACTER           1
#define VTKEY_ENTER               2
#define VTKEY_PF01                3
#define VTKEY_PF02                4
#define VTKEY_PF03                5
#define VTKEY_PF04                6
#define VTKEY_PF05                7
#define VTKEY_PF06                8
#define VTKEY_PF07                9
#define VTKEY_PF08                10
#define VTKEY_PF09                11
#define VTKEY_PF10                12
#define VTKEY_PF11                13
#define VTKEY_PF12                14
#define VTKEY_CLEAR               15
#define VTKEY_DO                  16
#define VTKEY_SK01                17
#define VTKEY_SK02                18
#define VTKEY_SK03                19
#define VTKEY_SK04                20
#define VTKEY_SK05                21
#define VTKEY_SK06                22
#define VTKEY_SK07                23
#define VTKEY_SK08                24
#define VTKEY_SK09                25
#define VTKEY_HELP                26
#define VTKEY_UP                  27
#define VTKEY_DOWN                28
#define VTKEY_LEFT                29
#define VTKEY_RIGHT               30
#define VTKEY_CMD                 31
#define VTKEY_PICK                32
#define VTKEY_PUT                 33
#define VTKEY_MARK                34
#define VTKEY_SELECT              35
#define VTKEY_MOVE                36
#define VTKEY_COPY                37
#define VTKEY_INSERT              38
#define VTKEY_DELETE              39
#define VTKEY_TOP                 40
#define VTKEY_BOTTOM              41
#define VTKEY_CANCEL              42
#define VTKEY_UNDO                43
#define VTKEY_END                 44
#define VTKEY_ESCAPE              45
#define VTKEY_FILL                46
#define VTKEY_FILE_OPEN           47
#define VTKEY_FILE_SAVE           48
#define VTKEY_EXIT                49
#define VTKEY_PRINT               50
#define VTKEY_FIND                51
#define VTKEY_REPLACE             52
#define VTKEY_CURSOR_UP           53
#define VTKEY_CURSOR_DOWN         54
#define VTKEY_CURSOR_RIGHT        55
#define VTKEY_CURSOR_LEFT         56
#define VTKEY_CURSOR_HOME         57
#define VTKEY_CURSOR_END          58
#define VTKEY_NEXT_FIELD          59
#define VTKEY_PREV_FIELD          60
#define VTKEY_INSERT_CHAR         61
#define VTKEY_DELETE_CHAR         62
#define VTKEY_RUBOUT              63
#define VTKEY_DEL_EOF             64
#define VTKEY_MOUSE_DOWN          65
#define VTKEY_MOUSE_UP            66
#define VTKEY_MOUSE_MOVE          67
#define VTKEY_MOUSE_DBL           68
#define VTKEY_HSCROLL             69
#define VTKEY_VSCROLL             70
#define VTKEY_CONTROL             71
#define VTKEY_CURSOR_BOL          72
#define VTKEY_CURSOR_EOL          73
#define VTKEY_LAST                74 /* enter new keys before this one */
/* internal key definitions */
#define VTKEY_TAB                 VTKEY_LAST+1
#define VTKEY_BACK_TAB            VTKEY_LAST+2
#define VTKEY_REFRESH             VTKEY_LAST+3
#endif	/*!ENUM_ARRAY*/

/* new CR : Definition des CR- und TAB-Verhaltens ( vt_usage_mode ) */
#define VT_FORM         0
#define VT_EDIT         1

/* Definition des vt_mark-Arrays */

#ifdef ENUM_ARRAY
enum { VTM_MARK_OFF, VTM_MARK_BLOCK , VTM_MARK_LINE , VTM_MARK_CONTINUE };
#else /* ! ENUM_ARRAY */
#define VTM_MARK_OFF            0
#define VTM_MARK_BLOCK          1
#define VTM_MARK_LINE           2
#define VTM_MARK_CONTINUE       3
#endif /* ! ENUM_ARRAY */

/* Definition des vton-Attribut-Arrays ( vt_mode ) */

#ifdef ENUM_ARRAY
enum
{
	VTA_BRIGHT,
	VTA_INVERSE,
	VTA_UNDERLINE,
	VTA_BLINK,
	VTA_INVISIBLE,
	VTA_MIXED,
	VTA_GRAYED,
	VTA_LAST
};
#else /* ! ENUM_ARRAY */
#define	VTA_BRIGHT               0
#define	VTA_INVERSE              1
#define	VTA_UNDERLINE            2
#define	VTA_BLINK                3
#define	VTA_INVISIBLE            4
#define	VTA_MIXED                5
#define	VTA_GRAYED               6
#define	VTA_LAST                 7
#endif /* ! ENUM_ARRAY */


/* Definition des Color-Arrays ( vt_color ) */

#ifdef ENUM_ARRAY
enum
{
	VTC_MONOCHROME,
	VTC_WHITE,
	VTC_BLACK,
	VTC_RED,
	VTC_GREEN,
	VTC_YELLOW,
	VTC_BLUE,
	VTC_PINK,
	VTC_LIGHT_BLUE,
	VTC_LAST
};
#else /* ! ENUM_ARRAY */
#define VTC_MONOCHROME           0
#define VTC_WHITE                1
#define VTC_BLACK                2
#define VTC_RED                  3
#define VTC_GREEN                4
#define VTC_YELLOW               5
#define VTC_BLUE                 6
#define VTC_PINK                 7
#define VTC_LIGHT_BLUE           8
#define	VTC_LAST                 9
#endif /* ! ENUM_ARRAY */


/* Definition der Attribut-Belegung im vt-Attribut-Byte */

#define BIT_CHANGED            16
#define BIT_MIXED              32
#define BIT_KANJI              64
#define BIT_UNPROTECTED       128

/* Definition der Platzhalter-Character fuer Graphik-Zeichen */

#define CH_VERTICAL_BAR       0x8
#define CH_L_RIGHT_CORNER     0xb
#define CH_U_RIGHT_CORNER     0xc
#define CH_L_LEFT_CORNER      0x10
#define CH_U_LEFT_CORNER      0x11
#define CH_LEFT_T             0x12
#define CH_RIGHT_T            0x13
#define CH_HORIZONTAL_BAR     0x14
#define CH_BOTTOM_T           0x1c
#define CH_TOP_T              0x1d
#define CH_CROSS              0x1e
#define	MAXGRAPHIC            0x20

/*==========================================================================*/
/*--------------                TYPES                    -------------------*/
/*==========================================================================*/

/*
 *  This struct holds the assiciation of a key function (e.g. VTKEY_HOME)
 *  to a character sequence which will be received when the key is pressed.
 *  The complete key information is represented by a chain of key_info
 *  structs, sorted by key sequence, and pointed to by 'sql04_key_list'.
 */
struct key_info
{
	struct key_info		*ki_next ;
	int			ki_function ;
	char			ki_sequence [ 1 ];
				/* allocated with required size */
};

/*
 *  This struct holds the association of key functions (e.g. VTKEY_HOME)
 *  to terminfo capability names. This information is required for
 *  analyzing the human interface files (.hif).
 *  The complete capname information consists of an initialized array
 *  of these structs (sql04_cap_table).
 */
struct capname_info
{
	int			ci_function ;
	char			*ci_capname ;
};

/*
 *  This struct holds the association of additional features to their
 *  capability names. These capabilities can only be used in the
 *  human interface files (.hif).
 *  The complete additional features information consists of an
 *  initialized array of these structs (sql04_hif_table).
 */
struct hifcap_info
{
	char			**hi_sequence ;
	char			*hi_capname ;
};

#define	UCHAR			unsigned char

/*==========================================================================*/
/*--------------                MACROS                   -------------------*/
/*==========================================================================*/

#undef	CTRL
#define	CTRL(x)			((x)&0x1f)
#define	spri			(void) sprintf

#define smaller(x,y)            ( ( (x) < (y) ) ? (x) : (y) )
#define greater(x,y)            ( ( (x) > (y) ) ? (x) : (y) )

/*==========================================================================*/
/*----------                    IMPORTED FUNCTIONS                ----------*/
/*==========================================================================*/

extern	char			*sql04_key_name ();
extern	int			sql04_get_environment ();
extern	int			sql04_make_key_list ();
extern	int			sql04_ins_keyinfo ();
extern	char			*sql04_put_sequence ();
extern	int			sql04_cat ();
extern	int			sql04_read_chrclass ();
extern	int			sql04_isprint ();
extern	int			sql04_init_graphic_table ();

/*==========================================================================*/
/*----------                    IMPORTED DATA                     ----------*/
/*==========================================================================*/

extern	char			*sql04_env_DBROOT ;
extern	char			*sql04_env_DBTERM ;
extern	char			*sql04_env_DBHIF ;
extern	char			*sql04_env_DBCHRCLASS ;
extern	char			*sql04_env_DBTERMRESET ;
extern	char			*sql04_env_DBTERMREDRAW ;
				/* TERM and TERMINFO from environment */
extern	char			*sql04_env_TERM_extern ;
extern	char			*sql04_env_TERMINFO_extern ;
				/* TERM and TERMINFO as used internally */
extern	char			*sql04_env_TERM ;
extern	char			*sql04_env_TERMINFO ;
				/* buffers for putenv */
extern	char			*sql04_envdef_TERM_extern ;
extern	char			*sql04_envdef_TERMINFO_extern ;
extern	char			*sql04_envdef_TERM ;
extern	char			*sql04_envdef_TERMINFO ;

extern	UCHAR			sql04_chrclass [ 256 ];
extern	chtype			sql04_graphic [ MAXGRAPHIC ];

extern	int			sql04_key_count ;
extern	int			sql04_key_value ;
extern	struct key_info		*sql04_key_list ;
extern	struct capname_info	sql04_cap_table [];

extern	char			*sql04_hif_softkeys ;
extern	struct hifcap_info	sql04_hif_table [];
