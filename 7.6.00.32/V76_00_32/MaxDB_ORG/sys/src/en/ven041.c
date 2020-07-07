/*!
  @file           ven041.c
  @author         JoergM
  @brief          virtual_terminal Unique Interface to Various Types of Terminals
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

#define MOD__ "ven041.c:"

#include "geo002.h"
#include "gen04.h"
#include "gen500.h"
#include "geo001.h"
#include "geo57.h"
#include "hen40.h"
/* includes ven00tc */

#define	VT_EOF			0	/* end of field */
#define	VT_SONCF		1	/* start of non-continuation field */
#define	VT_SOCF			2	/* start of continuation field */
#define	FIELD_DELIMITERS	3	/* number of field delimiter types */

#define LINEVALUES              20	/* debug line hex bytes */
#define VT_LABEL_SIZE           8
#define SK_NUMBER               9
#define BLANK                   (' ')
#define CHBLANK                 ((chtype)' ')
#define DELTA_TAB               8

#ifdef FREEBSD
 #define TABDLY OXTABS
#endif

typedef unsigned char tsp8_uni_error;
/* Pascal file function without prototypes */
extern int s80uni_charlayout(
tsp00_MoveObjPtr src_ptr,
tsp00_Int4 src_len,
tsp00_Int2 src_codeset,
tsp00_MoveObjPtr dest_ptr,
tsp8_uni_error *rc,
tsp00_Int4 *err_char_no);
extern tsp8_uni_error s80uni_chartype(
tsp00_MoveObjPtr str_ptr,
tsp00_Int4 *str_len,
tsp00_Int2 str_codeset);
typedef
  unsigned char tsp8_uni_load_error;
typedef
  unsigned char tsp8_mapc1values[65536];
typedef
  tsp00_C2 tsp8_mapvalues[65536];
typedef
  short tsp8_mapindexes[65536];
typedef
  unsigned char tsp8_value_typ;
typedef
  unsigned char tsp8_flags[1];
typedef
  unsigned char tsp8_descr_string[60];
typedef
  unsigned char tsp8_vendor_string[60];
typedef
  tsp8_mapvalues *tsp8_values_table_ptr;
typedef
  tsp8_mapindexes *tsp8_index_table_ptr;
typedef
  tsp8_mapc1values *tsp8_c1_values_table_ptr;
typedef
  struct ptoc_Type1
    {
    union 
      {
      struct
        {
        tsp8_values_table_ptr values;
        tsp8_value_typ value_typ;
        tsp00_Uint1 value_len;
        tsp00_Int2 mt_fill2;
        tsp00_Int4 mt_fill3;
        } C_1;
      struct
        {
        tsp8_index_table_ptr indexes;
        tsp8_value_typ imt_fill0;
        tsp00_Uint1 imt_fill1;
        tsp00_Int2 imt_fill2;
        tsp00_Int4 imt_fill3;
        } C_2;
      struct
        {
        tsp8_c1_values_table_ptr c1_values;
        tsp8_value_typ ic1_fill0;
        tsp00_Uint1 ic1_fill1;
        tsp00_Int2 ic1_fill2;
        tsp00_Int4 ic1_fill3;
        } C_3;
      } integer;
    } tsp8_mapping_table;
typedef
  tsp8_mapping_table *tsp8_mapping_table_ptr;
typedef
  struct ptoc_Type2
    {
    tsp00_KnlIdentifier descr;
    tsp00_Int2 subspace_cnt;
    tsp8_flags flags;
    char sub_c_filler;
    struct ptoc_Type3
      {
      tsp00_Int4 start_ix;
      tsp00_Int4 end_ix;
      tsp00_Int2 dimension;
      tsp00_Int2 ss_filler1;
      tsp00_Int4 ss_filler2;
      struct ptoc_Type4
        {
        unsigned char low;
        unsigned char high;
        tsp00_Int2 delta;
        tsp00_Int4 br_fill2;
        } byteranges[3];
      } subspace[4];
    } tsp8_subcode;
typedef
  tsp8_subcode *tsp8_subcode_ptr;
typedef
  tsp8_subcode_ptr tsp8_subcodes[20];
typedef
  tsp8_subcodes *tsp8_subcodes_ptr;
typedef
  struct ptoc_Type5
    {
    tsp8_mapping_table_ptr mapping1;
    tsp8_mapping_table_ptr mapping2;
    tsp8_subcodes subcode1;
    tsp8_subcodes subcode2;
    tsp8_vendor_string vendor;
    tsp8_descr_string descr;
    tsp00_KnlIdentifier iso_locale_str;
    tsp00_Int2 subcode_cnt1;
    tsp00_Int2 subcode_cnt2;
    tsp00_Int2 enc_filler1;
    tsp00_C4 fillcharacter;
    tsp00_Uint1 fillchar_len;
    tsp00_Uint1 max_codewidth;
    unsigned char typ;
    tsp00_Uint1 enc_filler2;
    } tsp8_encodings[5];
typedef
  tsp8_encodings *tsp8_encodings_ptr;
extern int s82uni_fload(
tsp8_encodings_ptr *encodings_ptr,
tsp00_Int2 *encoding_cnt,
char read_dblang_flag,
tsp00_KnlIdentifier iso_locale_str,
tsp00_Int2 *codeset,
tsp8_uni_load_error *rc);
extern int s82uni_load_error(
tsp8_uni_load_error rc,
tsp00_C40 msg);

/*
 *  Virtual Terminal Attributes contain an index into an array
 *  of curses attributes (blink,bold,colors,...).
 *  The index within the attribute byte implies a certain table size.
 *  The algorithm for the extraction of the index from the
 *  attribute byte must match with the table size.
 */
#define	MAXATTRIBUTES		16
#define	ATTR_TO_IDX(x)		( (x) & 15 )
#define	ATTR(x)			( sql04_attrib_table [ ATTR_TO_IDX(x) ].att )

#define is_invisible(x)		( sql04_attrib_table [ ATTR_TO_IDX(x) ].invis )
#define is_protect(x)		( ( (x) & BIT_UNPROTECTED ) == 0 )
#define is_unprot(x)		( ( (x) & BIT_UNPROTECTED ) != 0 )
#define is_mixed(x)		( ( (x) & BIT_MIXED ) != 0 )

#define LINE(x)                 ((x) / sql04_cols)
#define COLUMN(x)               ((x) % sql04_cols)

#ifdef  DEBUG_RTE
static	char			*DEBUGFILE = "/tmp/vt.diag" ;
static	FILE			*debugfp = NULL;
#define	DBGSCREEN(_s,_a,_l,_t)	{debug_screen((UCHAR*)(_s), \
				(UCHAR*)(_a),(int)(_l),(_t));}
#else	/*DEBUG_RTE*/
#define	DBGSCREEN(_s,_a,_l,_t)	{}
#endif	/*DEBUG_RTE*/

/*--------------------------------------------------------------------------*/
/*------------------    GLOBAL FUNCTION PROTOTYPES     ---------------------*/
/*--------------------------------------------------------------------------*/

global	void			sqlcton ();
global	void			sqlctoff ();
global	void			sqlctio ();
global	void			sqlcttable ();
global	void			sqlctgtable ();
global	void			sqltrelease ();
global	void			sqlthold ();
global	void			sqltermvar ();
#ifdef	OLD
global	void			sqltmap ();
#endif	/*OLD*/
global	void			sqldbgvt (char *, ...);

/*--------------------------------------------------------------------------*/
/*------------------  EXTERNAL FUNCTION PROTOTYPES     ---------------------*/
/*--------------------------------------------------------------------------*/

extern	char			*sqlerrs ();
extern	void			sqlabort ();
extern	void			sql47_ctop ();

extern	void			exit ();

/*--------------------------------------------------------------------------*/
/*------------------     LOCAL FUNCTION PROTOTYPES     ---------------------*/
/*--------------------------------------------------------------------------*/

static	void			sql04_retkey_array ();
static	void			sql04_attrib_array ();
static	void			sql04_colors_array ();
static	void			sql04_update_colors ();
static	chtype			sql04_convert_color ();

static	void			sql04_get_options ();
static	void			sql04_full_screen ();
static	void			sql04_part_screen ();
static	void			sql04_write_sysline ();
static	void			sql04_write_softkeys ();
static	int			sql04_get_key ();
static	int			sql04_input_character ();
static	void			sql04_put_charstring ();

static	void			sql04_init_ins ();
static	void			sql04_reset_ins ();
static	void			sql04_insert_on ();
static	void			sql04_insert_off ();

static	void			sql04_local_keys ();
static	void			sql04_delete ();
static	int			sql04_rubout ();
static	void			sql04_erase_to_eof ();
static	int			sql04_shift_left ();
static	int			sql04_shift_right (int start);
static	void			sql04_next_unprot ();
static	int			sql04_next_field ();
static	int			sql04_prev_field ();
static	void			sql04_position_changed ();

static	int			sql04_in_field (int start);
static	int			sql04_find_next ();
static	int			sql04_find_prev ();
static	int			sql04_find_unprot ();
static	int			sql04_find_eof ();

#ifdef	MARK_NOCH_ZU_TUN
static	void			sql04_write_marked ();
static	boolean			sql04_in_marked_area ();
#endif
static	void			sql04_addch ();
static	void			sql04_beep_flash ();

#ifdef DEBUG_RTE
static	void			debug_init ();
static	SIGNALFUNCTYPE		debug_catch ();
static	void			debug_screen ();
static	void			debug_hexline ();
static	void			debug_charline ();
static	void			debug_key_array ();
#endif

/*--------------------------------------------------------------------------*/
/*----------------     EXTERNAL DATA DEFINITIONS     -----------------------*/
/*--------------------------------------------------------------------------*/

extern	int			sql01_vt_clear ;
#ifdef KANJI
extern	char			*sql01_dblang ;
#endif

/*--------------------------------------------------------------------------*/
/*------------------     LOCAL DATA DEFINITIONS     ------------------------*/
/*--------------------------------------------------------------------------*/

struct attrib_info
{
	chtype		att ;           /* curses attribute */
	boolean		invis ;         /* invisible attribute is set */
	boolean		changed ;       /* modified by sqlttable */
	enum1		fg ;            /* foreground color */
	enum1		bg ;            /* background color */
};
static	struct attrib_info	sql04_attrib_table [ MAXATTRIBUTES ];

static	int	sql04_turned_on ;        /* 'sqlton' successfully executed */
static	int	sql04_released ;         /* 'sqltrelease' executed */
static	int	sql04_has_colors ;       /* the terminal has colors */
static	int	sql04_screen_size ;      /* size of screen in bytes */
static	int	sql04_lines ;            /* LINES modified by sqlton */
static	int	sql04_cols ;             /* COLS at sqlton */
static	int	sql04_tio_count ;        /* number of calls to 'sqltio' */
static	int	sql04_term_initialized ; /* terminal initialization done */
static	int	sql04_attrib_changed ;   /* attributes changed by 'sqlttable' */
static	int	sql04_first_linechg ;    /* first line with screen modific. */
static	int	sql04_last_linechg ;     /* last line with screen modific. */
static	int	sql04_ring_bell ;        /* sound terminal's bell */
static	int	sql04_mark_option ;      /* specifies, how to mark text */
static	int	sql04_wait_for_input ;   /* wait for input after output */
static	int	sql04_usage_mode ;       /* VT_FORM/VT_EDIT, for CR and TAB */
static	int	sql04_return_on_last ;   /* if cursor moves beyond last field */
static	int	sql04_return_on_first ;  /* if cursor moves ahead first field */
static	int	sql04_enable_charkey ;   /* return after printable character */
static	int	sql04_enable_ctrlkey ;   /* return after control character */
#ifndef	SAFE_CTRLKEY
static	int	sql04_old_enable_ctrlkey ;
#endif
static	int	sql04_position ;         /* cursor position index in buffer */
static	int	sql04_displpos ;         /* cursor position index on display */
static	int	sql04_insert_mode ;      /* if in insert mode */
static	int	sql04_insert_save ;      /* insert_mode at begin of sqlctio */
static	int	sql04_before_first ;     /* if before first unprotected char */
static	int	sql04_behind_last ;      /* if behind last unprotected char */
static	int	sql04_screen_changed ;   /* screen has been modified */

static	TERMIO_INFO   sql04_termio_extern ;	/* extern modes */
static	TERMIO_INFO   sql04_termio_intern ;	/* intern modes */
static	TERMIO_INFO   sql04_termio_ctlkey ;	/* modes for VTKEY_CONTROL */

static	UCHAR	*sql04_tio_screen ;             /* screen argument of sqltio */
static	UCHAR	*sql04_tio_attrib ;             /* attrib argument of sqltio */

static	UCHAR   *sql04_save_screen ;            /* saved screen contents */
static	UCHAR   *sql04_save_attrib ;            /* saved attributes contents */
#ifdef	PRINT_LINES_CHANGED
static	UCHAR	*sql04_check_screen ;           /* last input screen contents */
#endif
#ifdef KANJI
#define		uni_ok			0
#define		uni_no_such_encoding	1
#define		uni_not_translatable	2
#define		uni_dest_too_short	3
#define		uni_src_too_short	4
#define		uni_is_singlebyte	5
#define		uni_is_multibyte	6
#define		uni_is_incomplete	7

#define		uni_load_ok		0

static	void			*sql04_encoding_ptr ;  /* uni_trans */
static	tsp00_Int2		sql04_encoding_cnt ;   /* uni_trans */
static	tsp00_Int2		sql04_codeset ;        /* uni_trans */
static	UCHAR			*sql04_charmodes ;     /* character modes */
#endif

#ifdef	SCRLIB
static	int	sql04_sk_count ;                /* number of sofkeys */
#endif
static	UCHAR	sql04_save_sys_text [ 132 ];	/* saved message line */
static	UCHAR	sql04_save_sys_attr [ 132 ];	/* saved message attributes */
static	char	sql04_save_sk_text [ 2 * VT_LABEL_SIZE * SK_NUMBER ];

static	boolean	sql04_marked ;           /* if mark key pressed */
static	boolean	sql04_mark_on ;          /* if 'sqltio' requested mark */
static	int	sql04_mark_apos ;        /* startpos of marked field */
static	int	sql04_mark_epos ;        /* endpos of marked field */
static	chtype	sql04_mark_attrib ;      /* curses attribute for marked areas */

#ifdef	DEBUG_RTE
static	char	*sql04_color_name [] = { "b/w" , "white" , "black" , "red" ,
			  "green" , "yellow" , "blue" , "magenta" , "cyan" };
#endif

/*--------------------------------------------------------------------------*/
/*-----------------------     EXECUTABLE CODE     --------------------------*/
/*==========================================================================*/

/*
 *  'sqlton' initializes the virtual terminal.
 */

#undef MF__
#define MF__ MOD__"sqlcton"
global	void	sqlcton (
char			*returnkeys_array ,
tsp00_Int2		*size_returnkeys ,
char			*attributes_array ,
tsp00_Int2		*size_attributes ,
char			*colors_array ,
tsp00_Int2		*size_colors ,
boolean			*c_has_sysline ,
char			*c_label ,
tsp00_Int2		*maxlines ,
tsp00_Int2		*maxcols ,
UCHAR			**cscreen_buf ,
UCHAR			**cattrib_buf ,
boolean			*cgraphic ,
boolean			*cmark ,
boolean			*cwindows ,
boolean			*c_ok )
{

    int                 rc ;
    int                 idx ;
    SIGNALFUNCTYPE	(* oldsigint)();


    DBGIN;

    /*
     *  Initialize the return code.
     */
    *c_ok = FALSE ;

    if ( sql04_turned_on )
    {
		DBG1 (( MF__,"sqlton already performed\n" ));
		return ;
    }

    /*
     *  Check, if the PASCAL<->C mapper (ven04) is compatible.
     */
    rc = 0 ;
    if ( *size_returnkeys != VTKEY_LAST )
    {
	DBG1(( MF__, "sqlton internal: wrong no of key set elems\n" ))
	DBG1(( MF__, "size_returnkeys      = %d\n", *size_returnkeys ))
	DBG1(( MF__, "VTKEY_LAST           = %d\n", VTKEY_LAST ))
	rc ++ ;
    }
    if ( *size_attributes != VTA_LAST )
    {
	DBG1(( MF__, "sqlton internal: wrong no of attrib set elems\n" ))
	DBG1(( MF__, "size_attributes      = %d\n", *size_attributes ))
	DBG1(( MF__, "VTA_LAST             = %d\n", VTA_LAST ))
	rc ++ ;
    }
    if ( *size_colors != VTC_LAST )
    {
	DBG1(( MF__, "sqlton internal: wrong no of color set elems\n" ))
	DBG1(( MF__, "size_colors          = %d\n", *size_colors ))
	DBG1(( MF__, "VTC_LAST             = %d\n", VTC_LAST ))
	rc ++ ;
    }
    if ( rc ) return ;

    /*
     *  Get all required environment strings
     *  and, if necessary, export TERM and TERMINFO.
     */
    rc = sql04_get_environment ();
    if ( rc < 0 ) return ;

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
    if ( rc < 0 ) return ;

    /*
     *  Allow interrupts during initialization.
     */
    oldsigint = signal ( SIGINT , SIG_DFL );

    /*
     *  Save the termio values as they were outside of curses.
     */
    (void) GET_TERMIO ( 0 , &sql04_termio_extern );

#if OSF1
    /*
     *  Prepare a termio structure for use inside curses
     */
    SAPDB_memcpy ( &sql04_termio_intern , &sql04_termio_extern ,
				    sizeof(sql04_termio_intern) );
    /*
     *  Adjust the termio flags which are not equally set by all
     *  curses implementations.
     */
    sql04_termio_intern.c_iflag &= ~(ISTRIP|INLCR|IGNCR|ICRNL);
    sql04_termio_intern.c_oflag &= ~(ONLCR|OCRNL|TABDLY);
    sql04_termio_intern.c_lflag &= ~(ICANON|ECHO|ECHOE|ECHOK);

    /* sql04_termio_intern.c_iflag &= ~(IXON); don't catch Ctrl-Q and Ctrl-S */
    for ( idx = 0 ; idx < CC_SIZE ; idx ++ ) sql04_termio_intern.c_cc[idx] = 0 ;
    sql04_termio_intern.c_cc[VINTR ] = ('C'&0x1f);
    sql04_termio_intern.c_cc[VQUIT ] = ('Y'&0x1f);
    sql04_termio_intern.c_cc[VMIN  ] = 1 ;
    sql04_termio_intern.c_cc[VTIME ] = 1 ;

#if defined(VSTART) && (VSTART < CC_SIZE)
    sql04_termio_intern.c_cc[VSTART] = ('Q'&0x1f);
#endif
#if defined(VSTOP) && (VSTOP < CC_SIZE)
    sql04_termio_intern.c_cc[VSTOP ] = ('S'&0x1f);
#endif
#endif /*OSF1*/
#if OSF1
    /* make this mode always active for OSF1, switching is impossible on OSF1 */
    (void) SET_TERMIO ( 0 , &sql04_termio_intern );
#endif

    /*
     *  Initialize curses.
     *  'initscr' will never return unsuccessful.
     *  On error it writes a message and exits.
     *  If successful it returns stdscr.
     *  Verify this here, just in case...
     */
    if ( (initscr() != stdscr) || (stdscr == 0) )
    {
	DBG1(( MF__, "sqlton: error in curses-initscr() \n" ))
	(void) SET_TERMIO ( 0 , &sql04_termio_extern );
	DBG1 (( MF__,"restored external terminal settings \n" ))
	(void) signal ( SIGINT , oldsigint );
	return ;
    }

    /*
     *  Avoid using tabs.
     *  On some systems tabs are unreliably expanded by terminal driver.
     */
    init_tabs = 0 ;
    tab = 0 ;
    set_tab = 0 ;
    back_tab = 0 ;
    clear_all_tabs = 0 ;

#if ! OSF1 /* already taken care of -- see SET_TERMIO ( above ) */
    /*
     *  Set input modes.
     */
    (void) noecho ();
    (void) cbreak ();
    (void) nonl ();
#endif

#if ! OSF1 /* can't call GET_TERMIO() during curses mode */
    /*
     *  Save the termio values as they are inside of curses.
     */
    (void) GET_TERMIO ( 0 , &sql04_termio_intern );
#endif

#if ! OSF1 /* can't call SET_TERMIO() during curses mode */
    /*
     *  Adjust the termio flags which are not equally set by all
     *  curses implementations.
     */
    sql04_termio_intern.c_iflag &= ~(ISTRIP|INLCR|IGNCR|ICRNL);
    sql04_termio_intern.c_oflag &= ~(ONLCR|OCRNL|TABDLY);
    sql04_termio_intern.c_lflag &= ~(ICANON|ECHO|ECHOE|ECHOK);
    (void) SET_TERMIO ( 0 , &sql04_termio_intern );
    DBG1 (( MF__,"restored internal terminal settings \n" ))

    /*
     *  Prepare a termio structure for later use with VTKEY_CONTROL.
     */
    SAPDB_memcpy ( &sql04_termio_ctlkey , &sql04_termio_intern ,
				sizeof(sql04_termio_ctlkey) );
    /* sql04_termio_ctlkey.c_iflag &= ~(IXON); don't catch Ctrl-Q and Ctrl-S */
    for ( idx = 0 ; idx < CC_SIZE ; idx ++ ) sql04_termio_ctlkey.c_cc[idx] = 0 ;
    sql04_termio_ctlkey.c_cc[VINTR ] = ('C'&0x1f);
    sql04_termio_ctlkey.c_cc[VQUIT ] = ('Y'&0x1f);
    sql04_termio_ctlkey.c_cc[VMIN  ] = 1 ;
    sql04_termio_ctlkey.c_cc[VTIME ] = 1 ;
#if defined(VSTART) && (VSTART < CC_SIZE)
    sql04_termio_ctlkey.c_cc[VSTART] = ('Q'&0x1f);
#endif
#if defined(VSTOP) && (VSTOP < CC_SIZE)
    sql04_termio_ctlkey.c_cc[VSTOP ] = ('S'&0x1f);
#endif
#endif /*! OSF1*/

    /*
     *  Initialize the graphic characters conversion table.
     */
    *cgraphic = sql04_init_graphic_table ();
    DBG1 (( MF__,"has_graphic %d \n", *cgraphic ))

    /*
     *  Fill the available attributes table.
     */
    sql04_attrib_array ( attributes_array );

    /*
     *  Initialize the color feature.
     */
#if A_COLOR
#ifndef A_BGCOLOR
    (void) start_color ();
#endif /*A_BGCOLOR*/
#endif /*A_COLOR*/

    /*
     *  Fill the available colors table.
     */
    sql04_colors_array ( colors_array );

    /*
     *  Initialize the key list.
     */
    rc = sql04_make_key_list ();
    if ( rc < 0 )
    {
	(void) endwin ();
	(void) SET_TERMIO ( 0 , &sql04_termio_extern );
	DBG1 (( MF__,"restored external terminal settings \n" ))
	(void) signal ( SIGINT , oldsigint );
	return ;
    }

    /*
     *  Fill the available returnkeys table.
     */
    sql04_retkey_array ( returnkeys_array );

    /*
     *  After initialization we can return to the normal signal handling.
     */
    (void) signal ( SIGINT , oldsigint );

    /*
     *  if the keyboard has a MARK-Key and the terminal is able to display
     *  inverse- or blink-alltributes, then the virtual terminal provides
     *  the mark-functionalities
     */
    if ( enter_reverse_mode )
	sql04_mark_attrib = A_REVERSE ;
    else if ( enter_blink_mode )
	    sql04_mark_attrib = A_BLINK ;
	else
	    sql04_mark_attrib = 0 ;
/*JH
    *cmark = ( key_mark && sql04_mark_attrib ) ? 1 : ( sql04_mark_attrib = 0 );
*/
    *cmark = 0 ;        /* later */
    DBG1 (( MF__,"marking possible %d \n", *cmark ))

    /*
     *  Windows are not yet implemented.
     */
    *cwindows = 0 ;

    /*
     *  Get the lines and columns of the terminal.
     *  If the application runs in a window, the current window size
     *  will be used throughout the whole session.
     */
    sql04_lines = LINES ;
    sql04_cols  = COLS ;

#ifdef SCRLIB
    *c_has_sysline = TRUE ;
    *c_label = 1 ; /* short soft key labels */
    sql04_sk_count = SK_NUMBER ;
    /* Falls es keine hardwaremaessige Softkeyzeile gibt , vermindern sich die
       nutzbaren Zeilen um 1 */
    if ( ! f_skzex() ) sql04_lines -- ;
#else /*SCRLIB*/
    *c_has_sysline = has_status_line && to_status_line && from_status_line ;
    if ( sql04_hif_softkeys )
	*c_label = 1 ; /* short soft key labels */
    else
	*c_label = 0 ; /* no soft key labels */
#endif /*SCRLIB*/
    DBG1 (( MF__,"has_sysline %d \n", *c_has_sysline ))

    *maxlines = (tsp00_Int2) sql04_lines ;
    *maxcols  = (tsp00_Int2) sql04_cols ;
    sql04_screen_size = sql04_lines * sql04_cols ;
    DBG1 (( MF__,"maxlines    %d \n", *maxlines ))
    DBG1 (( MF__,"maxcols     %d \n", *maxcols ))
    DBG1 (( MF__,"screen_size %d \n", sql04_screen_size ))

    /*
     *  Allocate buffers for screen and attribute contents.
     */
    ALLOC_MEM_EO57( (void **)cscreen_buf, sql04_screen_size );
    ALLOC_MEM_EO57( (void **)cattrib_buf, sql04_screen_size );
    ALLOC_MEM_EO57( (void **)&sql04_save_screen, sql04_screen_size );
    ALLOC_MEM_EO57( (void **)&sql04_save_attrib, sql04_screen_size );
#ifdef	PRINT_LINES_CHANGED
    ALLOC_MEM_EO57( (void **)&sql04_check_screen, sql04_screen_size );
#endif
#ifdef KANJI
    ALLOC_MEM_EO57( (void **)&sql04_charmodes, sql04_screen_size );
#endif

    if (    !*cscreen_buf
	 || !*cattrib_buf
         || !sql04_save_screen
	 || !sql04_save_attrib
#ifdef	PRINT_LINES_CHANGED
         || ! sql04_check_screen
#endif
#ifdef KANJI
         || ! sql04_charmodes
#endif
       )
    {
	(void) endwin ();
	(void) SET_TERMIO ( 0 , &sql04_termio_extern );
	DBG1 (( MF__,"restored external terminal settings \n" ))
	return ;
    }
    DBG1 (( MF__,"cscreen_buf     0x%08lx \n", (long) *cscreen_buf ))
    DBG1 (( MF__,"cattrib_buf     0x%08lx \n", (long) *cattrib_buf ))
    DBG1 (( MF__,"save_screen     0x%08lx \n", (long) sql04_save_screen ))
    DBG1 (( MF__,"save_attrib     0x%08lx \n", (long) sql04_save_attrib ))
#ifdef	PRINT_LINES_CHANGED
    DBG1 (( MF__,"check_screen    0x%08lx \n", (long) sql04_check_screen ))
#endif
#ifdef KANJI
    DBG1 (( MF__,"charmodes       0x%08lx \n", (long) sql04_charmodes   ))
#endif

    SAPDB_memset ( *cscreen_buf        , ' '  , sql04_screen_size );
    SAPDB_memset ( *cattrib_buf        , 0    , sql04_screen_size );
    SAPDB_memset ( sql04_save_screen   , ' '  , sql04_screen_size );
    SAPDB_memset ( sql04_save_attrib   , 0    , sql04_screen_size );
#ifdef	PRINT_LINES_CHANGED
    SAPDB_memset ( sql04_check_screen  , 0xff , sql04_screen_size );
#endif
#ifdef KANJI
    SAPDB_memset ( sql04_charmodes     , 0    , sql04_screen_size );
#endif
    SAPDB_memset ( sql04_save_sys_text , ' '  , sizeof(sql04_save_sys_text) );
    SAPDB_memset ( sql04_save_sys_attr , 0    , sizeof(sql04_save_sys_attr) );
    SAPDB_memset ( sql04_save_sk_text  , ' '  , sizeof(sql04_save_sk_text)  );

#ifdef KANJI
    /*
     *  Load UNICODE tables.
     */
    {
	tsp8_uni_load_error	load_ret ;
	tsp00_C40	    msg;
	tsp00_KnlIdentifier	dblang;

	if ( sql01_dblang && *sql01_dblang )
	{
	    sql47_ctop ( dblang , sql01_dblang , sizeof(dblang) );
	    s82uni_fload ( (tsp8_encodings_ptr *)&sql04_encoding_ptr , &sql04_encoding_cnt , FALSE ,
					   dblang , &sql04_codeset , &load_ret );
	    if ( load_ret != uni_load_ok )
	    {
		s82uni_load_error ( load_ret , msg );
		DBG1(( MF__, "sqlton: %.40s\n", msg ))
		(void) endwin ();
		(void) SET_TERMIO ( 0 , &sql04_termio_extern );
		DBG1 (( MF__,"restored external terminal settings \n" ))
		return ;
	    }
	}
	else
	{
	    sql04_encoding_ptr = 0 ;
	    sql04_encoding_cnt = 0 ;
	    sql04_codeset      = 0 ;
	}
    }
#endif

    /*
     *  Force a full screen write at the first call to 'sqltio'.
     */
    sql01_vt_clear = TRUE ;

    /*
     *  Force rebuild of attributes and screen.
     */
    for ( idx = 0 ; idx < MAXATTRIBUTES ; idx++ )
	sql04_attrib_table[idx].changed = TRUE ;

    /*
     *  Anything is perfect.
     */
    sql04_tio_count = 1 ; /* F.H. for sqlthold */
    sql04_turned_on = TRUE ;
    *c_ok = TRUE ;

#ifdef DEBUG_DEBUG
    DBG1 (( MF__,"isprint values: \n" ))
    for ( idx = 0 ; idx < 16 ; idx ++ )
    {
	int fillchar;
	DBG1 (( MF__,"0x%02x:  ", idx<<4 ))
	for ( fillchar = 0 ; fillchar < 16 ; fillchar ++ )
	    DBG1 (( MF__,"%2d", isprint((idx<<4)|fillchar)?1:0 ))
	DBG1 (( MF__,"\n" ))
    }
    DBG1 (( MF__,"chrclass values: \n" ))
    for ( idx = 0 ; idx < 16 ; idx ++ )
    {
	int fillchar;
	DBG1 (( MF__,"0x%02x:  ", idx<<4 ))
	for ( fillchar = 0 ; fillchar < 16 ; fillchar ++ )
	    DBG1 (( MF__,"%2d", sql04_chrclass[(idx<<4)|fillchar]?1:0 ))
	DBG1 (( MF__,"\n" ))
    }
#endif
}

/*==========================================================================*/

/*
 *  'sqltoff' releases the virtual terminal.
 *  If 'msg' is all blanks, it is not written.
 */
#undef MF__
#define MF__ MOD__"sqlctoff"
global	void	sqlctoff (
char           **cscreen_buf ,
char           **cattrib_buf ,
char           *msg )
{
    int             i ;
#ifdef	SCRLIB
    extern VS       vs_fd ;
#endif


    DBGIN;
    DBGSCREEN ( msg , msg , 80 , "sqltoff message" )

    if ( ! sql04_turned_on )
    {
    	DBG1(( MF__, "sqlton never called\n" ))
	    return ;
    }

    /*
     *  Clear screen only if something has been written to it.
     *  Some applications may misbehave in the way that they
     *  suppress 'sqltio' when running in batch mode but
     *  calling 'sqlton'-'sqltoff' anyway.
     *  In this case we suppress the clearing of the screen.
     */
    (void) attrset ( A_NORMAL );
    (void) clear ();
    if ( sql04_tio_count > 1 /* sqlton ! */ )
    {
	(void) refresh ();
#ifdef	SCRLIB
	(void) d_sftkys (&vs_fd);    /* erase softkeys */
	(void) msg_mode ();          /* enter message mode (to_status_line) */
	(void) cbol ();		 /* cursor begin of line */
	(void) attrset ( A_NORMAL );
	/* Overwrite status line with blanks */
	for ( i = 0 ; i < sql04_cols ; i ++ ) (void) addch ( CHBLANK );
	(void) cbol ();		 /* cursor begin of line */
	(void) screen_mode ();       /* enter screen mode (from_status_line) */
#else	/*SCRLIB*/
	SAPDB_memset ( sql04_save_sys_text , ' ' , sizeof(sql04_save_sys_text) );
	SAPDB_memset ( sql04_save_sys_attr , 0   , sizeof(sql04_save_sys_attr) );
	SAPDB_memset ( sql04_save_sk_text  , ' ' , sizeof(sql04_save_sk_text)  );
	sql04_write_sysline ( sql04_save_sys_text , sql04_save_sys_attr );
	sql04_write_softkeys ( sql04_save_sk_text );
#endif	/*SCRLIB*/
    }

    /*
     *  Reset the screen to the 'outside curses' state.
     */
    (void) endwin ();

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
    (void) SET_TERMIO ( 0 , &sql04_termio_extern );
    DBG1 (( MF__,"restored external terminal settings \n" ))

    /*
     *  If the terminal initialization strings have been sent,
     *  the terminal must be reset.
     */
    if ( sql04_term_initialized )
    {
	/*
	 *  Reset the terminal.
	 */
	PUTP ( keypad_local );
	if ( init_prog  ) (void) system ( init_prog );
	if ( reset_file ) (void) sql04_cat ( reset_file );
	PUTP ( reset_1string );
	PUTP ( reset_2string );
	PUTP ( reset_3string );
	(void) fflush ( stdout );

	sql04_term_initialized = FALSE ;

	/*
	 *  Issue the user's reset command.
	 */
	if ( sql04_env_DBTERMRESET )
	    (void) system ( sql04_env_DBTERMRESET );
    }

    /*
     *  'endwin' arranges for a later call to 'refresh'.
     *  None of the curses data structures is released.
     *  To finally leave curses, 'delwin' is called
     *  which frees all data allocated by curses.
     */
    (void) delwin ( stdscr );

    /*
     *  Print the message.
     *  Terminate message with '\0'.
     *  Strip trailing spaces.
     *  Print only, if not empty.
     */
    for ( i = 0 ; (i < 80-1) && sql04_isprint(msg[i]&0xff) ; i ++ );
    for (       ; (i >  0)   &&       isspace(msg[i]&0xff) ; i -- );
    if ( sql04_isprint(msg[i]&0xff) && ! isspace(msg[i]&0xff) ) i ++ ;
    if ( i > 0 )
    {
	(void) fwrite ( msg , 1 , i , stdout );
	(void) fputc ( '\n' ,stdout );
	(void) fflush  ( stdout );
    }

    /*
     *  Free buffers.
     */
    DBG1 (( MF__,"free *cscreen_buf 0x%08lx \n", (long) *cscreen_buf ))
    FREE_MEM_EO57 ( *cscreen_buf );
    *cscreen_buf = 0 ;

    DBG1 (( MF__,"free *cattrib_buf 0x%08lx \n", (long) *cattrib_buf ))
    FREE_MEM_EO57 ( *cattrib_buf );
    *cattrib_buf = 0 ;

    DBG1 (( MF__,"free save_screen  0x%08lx \n", (long) sql04_save_screen ))
    FREE_MEM_EO57 ( sql04_save_screen );
    sql04_save_screen = 0 ;

    DBG1 (( MF__,"free save_attrib  0x%08lx \n", (long) sql04_save_attrib ))
    FREE_MEM_EO57 ( sql04_save_attrib );
    sql04_save_attrib = 0 ;

#ifdef	PRINT_LINES_CHANGED
    DBG1 (( MF__,"free check_screen 0x%08lx \n", (long) sql04_check_screen ))
    FREE_MEM_EO57 ( sql04_check_screen );
    sql04_check_screen = 0 ;
#endif

#ifdef KANJI
    DBG1 (( MF__,"free charmodes    0x%08lx \n", (long) sql04_charmodes ))
    FREE_MEM_EO57 ( sql04_charmodes );
    sql04_charmodes = 0 ;
#endif

    sql04_turned_on = FALSE ;
    sql04_released  = FALSE ;
    sql04_tio_count = 0 ;
}

/*==========================================================================*/

/*
 *  'sqltio' writes a screen image
 *  and, if 'wait_for_input', reads keys and characters from the keyboard.
 */

/*ARGSUSED*/	/* c_sk_text only for screenlib */

#undef MF__
#define MF__ MOD__"sqlctio"
global	void	sqlctio ( screen ,
			  attrib ,
			  cursorline ,
			  cursorcolumn ,
			  c_sk_text ,
			  c_sys_text ,
			  c_sys_attr ,
			  insert_text ,
			  insert_pos ,
			  insert_opt ,
			  c_vt_options ,
			  c_returnkeys ,
			  c_rejectkeys ,
			  num_of_lines ,
			  num_of_cols ,
			  mark_begin ,
			  mark_end ,
			  screen_changed ,
			  c_keystroke ,
			  c_refresh ,
			  c_ok )

UCHAR			**screen ;        /* screen buffer                */
UCHAR			**attrib ;        /* attributes buffer            */
tsp00_Int2		*cursorline ;     /* cursorposition (line)        */
tsp00_Int2		*cursorcolumn ;   /* cursorposition (column)      */
UCHAR			*c_sk_text ;      /* text for softkeys            */
UCHAR			*c_sys_text ;     /* text for status line         */
UCHAR			*c_sys_attr ;     /* attributes for status line   */
UCHAR			*insert_text ;    /* text for insert label        */
tsp00_Int2		*insert_pos ;     /* position for insert label    */
UCHAR			*insert_opt ;     /* attributes for insert label  */
char			*c_vt_options ;   /* options                      */
char			*c_returnkeys ;   /* table of returnkeys          */
char			*c_rejectkeys ;   /* table of rejectkeys          */
boolean			c_refresh ;       /* refresh before screen output */
tsp00_Int2		*num_of_lines ;   /* returns screen lines         */
tsp00_Int2		*num_of_cols ;    /* returns screen columns       */
tsp00_Int2		*mark_begin ;     /* returns begin of marked area */
tsp00_Int2		*mark_end ;       /* returns end of marked area   */
boolean			*screen_changed ; /* returns if screen changed    */
char			*c_keystroke ;    /* returns release key          */
boolean			*c_ok ;           /* returncode                   */

{
    int             keyfunc ;


    if ( ! sql04_turned_on )
    {
	DBG1(( MF__, "sqlton never called\n" ))
	*c_ok = FALSE ;
	return ;
    }

    if ( sql04_released )
    {
	DBG1(( MF__,"sqltio after sqltrelease \n" ))
	*c_ok = FALSE ;
	return ;
    }

    sql04_tio_count ++ ;			/* count the calls to sqltio */
    sql04_tio_screen = *screen ;		/* get a pointer value */
    sql04_tio_attrib = *attrib ;		/* get a pointer value */
    *c_ok            = TRUE ;			/* initialize output argument */
    *num_of_lines    = (tsp00_Int2) sql04_lines ;	/* initialize output argument */
    *num_of_cols     = (tsp00_Int2) sql04_cols ;	/* initialize output argument */
    *screen_changed  = FALSE ;			/* initialize output argument */

    DBG1 (( MF__,"\n" ))
    DBG1 (( MF__,"called +++++ %d +++++\n", sql04_tio_count ))
    DBG1 (( MF__,"screen       0x%08lx \n", (long) sql04_tio_screen ))
    DBG1 (( MF__,"attrib       0x%08lx \n", (long) sql04_tio_attrib ))
    DBG1 (( MF__,"screen_size  %d \n", sql04_screen_size ))
    DBG1 (( MF__,"screen_lines %d \n", sql04_lines ))
    DBG1 (( MF__,"screen_cols  %d \n", sql04_cols ))

    /*
     *  At the first call to sqltio send the terminal initialization strings.
     */
    if ( ! sql04_term_initialized )
    {
	if ( init_prog ) (void) system ( init_prog );
	if ( init_file ) (void) sql04_cat ( init_file );
#if OSF1 /* must resume curses mode before calling curses routines */
        (void) refresh ();
#endif
	PUTP ( init_1string );
	PUTP ( init_2string );
	PUTP ( init_3string );
	PUTP ( keypad_xmit );
	sql04_term_initialized = TRUE ;
    }

    /*
     *  Extract the options from the options array.
     */
    sql04_get_options ( c_vt_options );
    DBG1 (( MF__,"refresh            = %d\n", c_refresh ))

    /*
     *  Update the attributes table if sqlttable has been called.
     *  Using 'init_pair' refreshes the screen immediately.
     *  To avoid this clear the screen and update the colors
     *  immediately before writing the new screen.
     */
    if ( sql04_has_colors && sql04_attrib_changed ) sql04_update_colors ();

    DBGSCREEN ( sql04_tio_screen , sql04_tio_attrib ,
		sql04_screen_size , "input screen" )

#ifdef KANJI
    /*
     *  Create the charmodes array from the screen buffer.
     */
    {
	tsp8_uni_error		retcode ;
	tsp00_Int4		err_char_no ;

	if ( sql04_encoding_ptr && sql04_encoding_cnt )
	{
	    s80uni_charlayout ( (tsp00_MoveObjPtr)sql04_tio_screen ,
				(tsp00_Int4) sql04_screen_size ,
				sql04_codeset , (tsp00_MoveObjPtr)sql04_charmodes ,
				&retcode , &err_char_no );
	    if ( retcode != uni_ok )
	    {
		DBG1 (( MF__,"s80uni_charlayout error %d pos %d \n",
				    (int) retcode , (int) err_char_no ))
		DBG1(( MF__,"s80uni_charlayout error %d pos %d \n",
					    (int) retcode , (int) err_char_no ))
		*c_ok = FALSE ;
		return ;
	    }
	}
    }
#endif

    /*
     *  Write the new screen image.
     *  'full_screen' clears and rewrites the complete screen.
     *  It also (re-)initializes the save buffers.
     *  'part_screen' writes only the changed lines.
     */
#ifdef	FORCE_REFRESH
    sql01_vt_clear = TRUE ;
#endif
    if ( sql01_vt_clear || c_refresh )
	sql04_full_screen ();
    else
	sql04_part_screen ();

    /*
     *  Initialize the insert mode and write the insert label.
     */
    sql04_init_ins ( insert_pos , insert_text , insert_opt );

    /*
     *  Mark the marked characters.
     */
#ifdef MARK_NOCH_ZU_TUN
    sql04_mark_on = ( sql04_mark_option && (*mark_begin < *mark_end) );
#else
    sql04_mark_on = FALSE ;
#endif
    if ( sql04_mark_on )
	sql04_mark_apos = *mark_begin - 1 ;
    else
	sql04_mark_apos = -1 ;
    DBG1 (( MF__,"mark_begin %d \n", *mark_begin ))
    DBG1 (( MF__,"mark_end   %d \n", *mark_end   ))
    DBG1 (( MF__,"mark_apos  %d \n", sql04_mark_apos   ))

    /*
     *  Compute the current cursor position in the screen buffer.
     *  The virtual terminal interface counts from 1 to lines,
     *  curses counts from 0 to lines-1.
     */
    DBG1 (( MF__,"cursorline   %d \n", *cursorline   ))
    DBG1 (( MF__,"cursorcolumn %d \n", *cursorcolumn ))
    sql04_position = (*cursorline - 1) * sql04_cols + (*cursorcolumn - 1);
    if ( (sql04_position < 0) || (sql04_position >= sql04_screen_size) )
    {
	sql04_position = 0 ;
    }
#ifdef KANJI
    while ( (sql04_position > 0) && sql04_charmodes[sql04_position] )
	sql04_position -- ;
#endif
    sql04_displpos = sql04_position ;

    /*
     *  Move the cursor to the requested position
     *  and output the screen.
     */
    (void) move ( LINE(sql04_displpos) , COLUMN(sql04_displpos) );
    if ( sql04_env_DBTERMREDRAW ) (void) clearok ( stdscr , TRUE );
    (void) refresh ();

    /*
     *  If changed, write the new message line.
     */
    if ( sql01_vt_clear ||
	 memcmp ( (char*) c_sys_text , (char*) sql04_save_sys_text ,
					sizeof(sql04_save_sys_text) ) ||
	 memcmp ( (char*) c_sys_attr , (char*) sql04_save_sys_attr ,
					sizeof(sql04_save_sys_attr) )    )
    {
	DBGSCREEN ( c_sys_text , c_sys_attr , sql04_cols , "Message Line" );
	sql04_write_sysline ( c_sys_text , c_sys_attr );

	SAPDB_memcpy ( sql04_save_sys_text , c_sys_text , sizeof(sql04_save_sys_text) );
	SAPDB_memcpy ( sql04_save_sys_attr , c_sys_attr , sizeof(sql04_save_sys_attr) );
    }

    /*
     *  If changed, write the new softkey labels.
     */
    if ( sql01_vt_clear ||
	 memcmp ( (char*) c_sk_text , (char*) sql04_save_sk_text ,
					sizeof(sql04_save_sk_text) ) )
    {
	DBGSCREEN ( c_sk_text , c_sk_text , 153 , "Softkeys" );
	sql04_write_softkeys ( c_sk_text );
	SAPDB_memcpy ( sql04_save_sk_text , c_sk_text , sizeof(sql04_save_sk_text) );
    }

    /*
     *  Sound terminal's bell, if requested.
     *  Some terminals can't generate an audible signal, but may be able
     *  to flash the screen (light it up for a short time).
     */
    if ( sql04_ring_bell )
    {
	sql04_beep_flash ();
    }

    /*
     *  If 'sqlttable' was called, all steps for updating the screen
     *  have now been taken. Reset the flag.
     *  Applies also to the clear flag.
     */
    sql04_attrib_changed = FALSE ;
    sql01_vt_clear = FALSE ;

    /*
     *  Initialize global flag.
     */
    sql04_screen_changed = FALSE ;

    /*
     *  If requested, wait for keyboard input.
     */
    if ( sql04_wait_for_input )
    {
#ifdef DEBUG_DEBUG
	debug_key_array ( "return:" , c_returnkeys );
	debug_key_array ( "reject:" , c_rejectkeys );
#endif

#if ! OSF1 /* can't call SET_TERMIO() during curses mode */
	/*
	 *  If VTKEY_CONTROL is in the returnkeys,
	 *  'sql04_enable_ctrlkey' has been set by 'get_options'.
	 */
#ifdef	SAFE_CTRLKEY
        if ( sql04_enable_ctrlkey )
	{
	    DBG1 (( MF__,"sqltio(%d): control_key enabled \n", sql04_tio_count ))
	    /* set special input mode */
	    (void) SET_TERMIO ( 0 , &sql04_termio_ctlkey );
	}
#else
	/*
	 *  Change the termio values only, if this is a change to the
	 *  last setting of 'sql04_enable_ctrlkey'.
	 *  ('ioctl'/'tcsetattr' consumes some time).
	 */
        if ( sql04_enable_ctrlkey )
	{
	    if ( !sql04_old_enable_ctrlkey )
	    {
		DBG1 (( MF__,"sqltio(%d): control_key enabled \n", sql04_tio_count ))
		/* set special input mode */
		(void) SET_TERMIO ( 0 , &sql04_termio_ctlkey );
	    }
	}
	else
	{
	    if ( sql04_old_enable_ctrlkey )
	    {
		DBG1 (( MF__,"sqltio(%d): control_key disabled\n", sql04_tio_count ))
		/* reset special input mode */
		(void) SET_TERMIO ( 0 , &sql04_termio_intern );
	    }
	}
	sql04_old_enable_ctrlkey = sql04_enable_ctrlkey ;
#endif	/*SAFE_CTRLKEY*/
#endif /*! OSF1*/

	/*
	 *  Initialize the end of input flags.
	 */
	sql04_before_first = FALSE ;
	sql04_behind_last = FALSE ;

	/*
	 *  Loop until
	 *  - a returnkey is pressed,
	 *  - option "return_on_last" is active and the cursor is behind
	 *    the last unprotected character
	 *  - option "return_on_first" is active and the cursor is before
	 *    the first unprotected character
	 */
	while ( ! sql04_behind_last && ! sql04_before_first )
	{
	    if ( sql04_enable_charkey )
	    {
		/*
		 *  return on any printable character.
		 */
		keyfunc = sql04_get_key ();
		if ( (keyfunc == VTKEY_UNKNOWN)   ||
		     (keyfunc == VTKEY_CHARACTER) ||
		     (keyfunc == VTKEY_CONTROL)      ) break ;
	    }
	    else /* sql04_enable_charkey */
	    {
		/*
		 *  Change input fields. Enter printable characters.
		 *  Leave the loop, if a non-printable character or key
		 *  was received or after the last unprotected character
		 *  has been changed.
		 */
		while ( ! sql04_behind_last )
		{
		    keyfunc = sql04_input_character ();
		    if ( keyfunc != VTKEY_CHARACTER ) break ;

		    sql04_part_screen ();
		    (void) move ( LINE(sql04_displpos) ,
				COLUMN(sql04_displpos) );
		    (void) refresh ();
		}
		/* while !behind_last */
	    }
	    /* sql04_enable_charkey */

	    DBG1 (( MF__,"Key: %s return=%d reject=%d \n",
		    sql04_key_name(keyfunc) , c_returnkeys[keyfunc] ,
					      c_rejectkeys[keyfunc] ))

	    if ( sql04_behind_last || c_returnkeys[keyfunc] ) break ;
	    if ( c_rejectkeys[keyfunc] )
	    {
		sql04_beep_flash ();
		continue ;
	    }

	    switch ( keyfunc )
	    {
	    case VTKEY_MARK :
#ifdef MARK_NOCH_ZU_TUN
		sql04_mark_apos = sql04_position ;
		sql04_marked    = TRUE ;
		sql04_mark_on   = sql04_mark_on ? FALSE : TRUE ;
		*mark_begin     = (tsp00_Int2) (sql04_position + 1);
		sql04_write_marked ();
		DBG1 (( MF__,"vtinout : MARK_KEY pressed, mark_on now = %d\n",
							 sql04_mark_on))
#endif
		break ;

	    case VTKEY_INSERT_CHAR :
		if ( sql04_insert_mode )
		    sql04_insert_off ( insert_pos );
		else
		    sql04_insert_on  ( insert_pos , insert_text , insert_opt );
		break ;

	    case VTKEY_REFRESH :
		sql04_full_screen ();
		if ( sql04_insert_mode )
		    sql04_insert_on ( insert_pos , insert_text , insert_opt );
		(void) move ( LINE(sql04_displpos) , COLUMN(sql04_displpos) );
		(void) refresh ();
		sql04_write_sysline ( c_sys_text , c_sys_attr );
		sql04_write_softkeys ( c_sk_text );
		SAPDB_memcpy ( sql04_save_sys_text , c_sys_text ,
			sizeof(sql04_save_sys_text) );
		SAPDB_memcpy ( sql04_save_sys_attr , c_sys_attr ,
			sizeof(sql04_save_sys_attr) );
		SAPDB_memcpy ( sql04_save_sk_text , c_sk_text ,
			sizeof(sql04_save_sk_text) );
		break ;

	    default :
		sql04_local_keys ( keyfunc );
		sql04_part_screen ();
	    }
	    /* switch */

	    (void) move ( LINE(sql04_displpos) , COLUMN(sql04_displpos) );
	    (void) refresh ();
	}
	/* while !behind_last && !before_first */

#if ! OSF1 /* can't call SET_TERMIO() during curses mode */
#ifdef	SAFE_CTRLKEY
        if ( sql04_enable_ctrlkey )
	{
	    DBG1 (( MF__,"sqltio(%d): control_key disabled\n", sql04_tio_count ))
	    /* reset special input mode */
	    (void) SET_TERMIO ( 0 , &sql04_termio_intern );
	}
#endif	/*SAFE_CTRLKEY*/
#endif /*OSF1*/

    }
    /* wait for input */

    /*
     *  Return the key function and value.
     */
    c_keystroke[0] = (char) keyfunc ;
    c_keystroke[1] = (char) sql04_key_value ;

    /*
     *  Return the mark positions.
     */
    if ( sql04_marked )
    {
	*mark_end = (tsp00_Int2) (sql04_position + 1);
	if ( *mark_end < *mark_begin )
	{
	    int save_mark ;
	    save_mark = *mark_begin ;
	    *mark_begin = *mark_end ;
	    *mark_end = (tsp00_Int2) save_mark ;
	}
    }

    /*
     *  Return the current cursor position.
     */
    *cursorline   = (tsp00_Int2) (LINE(sql04_displpos) + 1);
    *cursorcolumn = (tsp00_Int2) (COLUMN(sql04_displpos) + 1);

    /*
     *  Return the screen change status.
     */
    *screen_changed = (boolean) sql04_screen_changed ;

    /*
     *  Remove the insert label from the screen since it might be
     *  elsewhere at the next call to 'sqltio'.
     */
    sql04_reset_ins ( insert_pos );

    DBG1 (( MF__,"returning +++++++++++++\n" ))
}

/*==========================================================================*/

/*
 *  'sqlttable' sets the attributes table.
 */

global	void	sqlcttable ( elem , att_array , fg , bg )

tsp00_Int2		elem ;
char			*att_array ;
enum1			fg ;
enum1			bg ;

{
    if ( (elem < 0) || (elem >= MAXATTRIBUTES) )
    {
		MSGCD (( -11987, ERR_TYPE, "VTERM   ",
			"sqlttable: illegal attribute index %d \n", elem ));
		sqlabort ();
		return ;
    }

    DBG1 (( MF__,"sqlttable: %2d fg/bg %d/%d (%s/%s) %s %s %s %s %s \n" ,
	elem , fg , bg , sql04_color_name[fg] , sql04_color_name[bg] ,
	att_array[0]?"BRIGHT":""    , att_array[1]?"REVERSE":"" ,
	att_array[2]?"UNDERLINE":"" , att_array[3]?"BLINK":""   ,
	att_array[4]?"INVISIBLE":"" ))


    /*
     *  We have to redraw the complete screen if any of the attributes
     *  changed. Otherwise the optimization would only redraw those parts
     *  of the screen which changed their character contents.
     */
    sql01_vt_clear = TRUE ;
    sql04_attrib_changed = TRUE ;
    sql04_attrib_table[elem].changed = TRUE ;
    sql04_attrib_table[elem].invis   = FALSE ;
    sql04_attrib_table[elem].att     = 0 ;

#ifdef SCRLIB_BOLD     /* Probleme mit Screenlib und bold */
    if ( att_array[0] ) sql04_attrib_table[elem].att |= A_BOLD ;
#endif
#ifndef	SCRLIB
#if ! sun /*&& ! T31*/
    if ( att_array[0] ) sql04_attrib_table[elem].att |= A_BOLD ;
#endif
#endif

    if ( att_array[1] ) sql04_attrib_table[elem].att |= A_REVERSE ;
    if ( att_array[2] ) sql04_attrib_table[elem].att |= A_UNDERLINE ;
    if ( att_array[3] ) sql04_attrib_table[elem].att |= A_BLINK ;
    if ( att_array[4] ) sql04_attrib_table[elem].invis = TRUE ;

    sql04_attrib_table[elem].fg = fg ;
    sql04_attrib_table[elem].bg = bg ;
    /*
     *  Defer the conversion of the colors to the attribute until the next
     *  sqltio is called: init_pair refreshes the screen immediately !
     */

    DBGG (( "sqlttable: curses attr 0x%08lx \n",
	    (long) sql04_attrib_table[elem].att ))
}

/*==========================================================================*/

/*
 *  'sqltgtable' reads the attributes table.
 */

global	void	sqlctgtable ( elem , att_array , fg , bg )

tsp00_Int2		elem ;
char			*att_array ;
enum1			*fg ;
enum1			*bg ;

{
    if ( (elem < 0) || (elem >= MAXATTRIBUTES) )
    {
		MSGCD (( -11987, ERR_TYPE, "VTERM   ",
			"sqltgtable: illegal attribute index %d \n", elem ));
		sqlabort ();
		return ;
    }

    SAPDB_memset ( att_array , 0 , VTA_LAST );

    if ( sql04_attrib_table[elem].att & A_BOLD      ) att_array[0] = TRUE ;
    if ( sql04_attrib_table[elem].att & A_REVERSE   ) att_array[1] = TRUE ;
    if ( sql04_attrib_table[elem].att & A_UNDERLINE ) att_array[2] = TRUE ;
    if ( sql04_attrib_table[elem].att & A_BLINK     ) att_array[3] = TRUE ;
    if ( sql04_attrib_table[elem].invis             ) att_array[4] = TRUE ;

    *fg = sql04_attrib_table[elem].fg ;
    *bg = sql04_attrib_table[elem].bg ;
}

/*==========================================================================*/

/*
 *  'sqltrelease' suspends curses mode
 *  and temporarily restores shell mode.
 *  Curses mode will be resumed through a call to 'sqlthold'.
 */

/*ARGSUSED*/ /* term_ref unused */
global	void	sqltrelease ( term_ref )

tsp00_Int4		term_ref ;

{
#ifdef SCRLIB
    register int    i ;
    extern VS       vs_fd ;
#endif


    if ( ! sql04_turned_on ) return ;
    sql04_released = TRUE ;
    DBG1 (( MF__,"sqltrelease: called \n" ))

    /*
     *  Clear screen only if something has been written to it.
     *  Some applications may misbehave in the way that they
     *  suppress 'sqltio' when running in batch mode but
     *  calling 'sqlton'-'sqltoff' anyway.
     *  In this case we suppress the clearing of the screen.
     */
    (void) attrset ( A_NORMAL );
    (void) clear ();
    if ( sql04_tio_count > 0 )
    {
	(void) refresh ();
#ifdef	SCRLIB
	(void) d_sftkys (&vs_fd);    /* erase softkeys */
	(void) msg_mode ();          /* enter message mode (to_status_line) */
	(void) cbol ();		 /* cursor begin of line */
	(void) attrset ( A_NORMAL );
	/* Overwrite status line with blanks */
	for ( i = 0 ; i < sql04_cols ; i ++ ) (void) addch ( CHBLANK );
	(void) cbol ();		 /* cursor begin of line */
	(void) screen_mode ();       /* enter screen mode (from_status_line) */
#else	/*SCRLIB*/
	SAPDB_memset ( sql04_save_sys_text , ' ' , sizeof(sql04_save_sys_text) );
	SAPDB_memset ( sql04_save_sys_attr , 0   , sizeof(sql04_save_sys_attr) );
	SAPDB_memset ( sql04_save_sk_text  , ' ' , sizeof(sql04_save_sk_text)  );
	sql04_write_sysline ( sql04_save_sys_text , sql04_save_sys_attr );
	sql04_write_softkeys ( sql04_save_sk_text );
#endif	/*SCRLIB*/
    }
#if OSF1 /* must call curses routines during curses mode */
    if ( sql04_term_initialized )
    {
        /*
         *  Reset the terminal.
         */
        PUTP ( keypad_local );
        if ( init_prog  ) (void) system ( init_prog );
        if ( reset_file ) (void) sql04_cat ( reset_file );
        PUTP ( reset_1string );
        PUTP ( reset_2string );
        PUTP ( reset_3string );
        (void) fflush ( stdout );

        sql04_term_initialized = FALSE ;

        /*
         *  Issue the user's reset command.
         */
        if ( sql04_env_DBTERMRESET )
            (void) system ( sql04_env_DBTERMRESET );
    }
#endif

    /*
     *  Reset the screen to the 'outside curses' state.
     *  'endwin' arranges for a later call to 'refresh'.
     *  None of the curses data structures is released.
     *  Calling 'refresh' again restores curses mode and
     *  redraws the last screen contents
     *  (here it will be cleared, since we called 'clear' some lines above).
     */
    (void) endwin ();

    /*
     *  On some systems 'endwin' leaves stdio in full buffered mode.
     */
    (void) setvbuf ( stdout , (char*) 0 , _IOLBF , BUFSIZ );
    (void) setvbuf ( stderr , (char*) 0 , _IONBF , BUFSIZ );

    /*
     *  Since we modified the termio values by ourself,
     *  we restore them completely to their previous state.
     */
    (void) SET_TERMIO ( 0 , &sql04_termio_extern );
    DBG1 (( MF__,"restored external terminal settings \n" ))
#ifndef	SAFE_CTRLKEY
    sql04_enable_ctrlkey = FALSE ;
#endif

#if ! OSF1 /* must call curses routines during curses mode */
    if ( sql04_term_initialized )
    {
	/*
	 *  Reset the terminal.
	 */
	PUTP ( keypad_local );
	if ( init_prog  ) (void) system ( init_prog );
	if ( reset_file ) (void) sql04_cat ( reset_file );
	PUTP ( reset_1string );
	PUTP ( reset_2string );
	PUTP ( reset_3string );
	(void) fflush ( stdout );

	sql04_term_initialized = FALSE ;

	/*
	 *  Issue the user's reset command.
	 */
	if ( sql04_env_DBTERMRESET )
	    (void) system ( sql04_env_DBTERMRESET );
    }
#endif

    /*
     *  Restore the original environment.
     */
    if ( sql04_envdef_TERM_extern )
	(void) putenv ( sql04_envdef_TERM_extern );
    if ( sql04_envdef_TERMINFO_extern )
	(void) putenv ( sql04_envdef_TERMINFO_extern );
}

/*==========================================================================*/

/*
 *  'sqlthold' resumes curses mode after a call to 'sqltrelease'.
 */

/*ARGSUSED*/ /* term_ref unused */
global	void	sqlthold ( term_ref , manual_input )

tsp00_Int4		term_ref ;
boolean			manual_input ;

{
    char            dummy [2];
#ifdef SCRLIB
    extern VS       vs_fd ;
#endif


    if ( ! sql04_released ) return ;
    sql04_released = FALSE ;
    DBG1 (( MF__,"sqlthold: called, manual_input %d \n", manual_input ))

    if ( manual_input && (sql04_tio_count > 0) )
    {
	(void) fprintf ( stdout , "Hit <CR> to return " );
	(void) fflush  ( stdout );
	(void) read ( 0 , dummy , 1 );
    }

    /*
     *  We released the terminal by calling 'endwin'.
     *  'endwin' restores shell mode and prepares the curses data
     *  for resuming curses mode after the next call to 'refresh'.
     *  This next call to refresh will be done with the next 'sqltio'.
     *  We suppress it here, since the component may run in batch mode.
     *  In this case we won't output anything.
     */

    /*
     *  Since we modified the termio values by ourself,
     *  we restore them completely to their previous state.
     */
    (void) SET_TERMIO ( 0 , &sql04_termio_intern );
    DBG1 (( MF__,"restored internal terminal settings \n" ))

    /*
     *  Force a refresh of the screen.
     */
    sql01_vt_clear = TRUE ;

    /*
     *  Set the internal environment.
     */
    if ( sql04_envdef_TERM )
	(void) putenv ( sql04_envdef_TERM );
    if ( sql04_envdef_TERMINFO )
	(void) putenv ( sql04_envdef_TERMINFO );
}

/*==========================================================================*/

/*
 *  'sqltermvar' returns the terminal-type (TERM or DBTERM) to the caller.
 *  If nothing is set, a blank-string is returned in term_type.
 */

global	void	sqltermvar ( term_type )

char    *term_type ;

{
    if ( ! sql04_env_TERM )
	SAPDB_memset ( term_type , ' ' , sizeof(tsp00_C18) );
    else
	sql47_ctop ( term_type , sql04_env_TERM , sizeof(tsp00_C18) );

}

/*==========================================================================*/

#ifdef	OLD
/*
 *  'sqltmap' maps all non-printable characters in a character array
 *  to question marks.
 */

/*ARGSUSED*/ /* term_ref unused */
global	void	sqltmap ( term_ref , buf , startpos , length )

tsp00_Int4		term_ref ;
char			*buf ;
tsp00_Int2		startpos ;
tsp00_Int2		length ;

{
    register int             i ;
    register char            *spos ;


    spos = buf + startpos - 1 ;

    for ( i = 0 ; i < length ; i ++ , spos ++ )
    {
	 if ( ! sql04_isprint(*spos) ) *spos = '?' ;
    }
}
#endif	/*OLD*/

/*==========================================================================*/

/*
 *  Fill the table of available returnkeys.
 */

static	void	sql04_retkey_array ( returnkeys_array )

register char  *returnkeys_array ;

{
    struct key_info		*kip ;


    SAPDB_memset ( returnkeys_array , 0 , VTKEY_LAST );

    /*
     *  The following keys are always available:
     *  some are available on all keyboards (e.g. RETURN/ENTER),
     *  others can be simulated by control sequences (e.g. Ctrl-L),
     *  others are generated by interrupt or special terminal modes.
     */
    returnkeys_array [ VTKEY_CHARACTER ]    = TRUE ; /* characters, e.g. 'a' */
    returnkeys_array [ VTKEY_ENTER ]        = TRUE ; /* Ctrl-D */
    returnkeys_array [ VTKEY_CLEAR ]        = TRUE ; /* Ctrl-F */
    returnkeys_array [ VTKEY_DO ]           = TRUE ; /* RETURN with VT_FORM */
    returnkeys_array [ VTKEY_CMD ]          = TRUE ; /* Ctrl-R */
    returnkeys_array [ VTKEY_CANCEL ]       = TRUE ; /* interrupt */
    returnkeys_array [ VTKEY_END ]          = TRUE ; /* Ctrl-E and/or Ctrl-Z */
    returnkeys_array [ VTKEY_NEXT_FIELD ]   = TRUE ; /* TAB with VT_FORM
							RETURN with VT_EDIT */
    returnkeys_array [ VTKEY_PREV_FIELD ]   = TRUE ; /* Ctrl-K with VT_FORM */
    returnkeys_array [ VTKEY_DEL_EOF ]      = TRUE ; /* Ctrl-X */
    returnkeys_array [ VTKEY_CONTROL ]      = TRUE ; /* Ctrl-A to Ctrl-Z when
							enabled as returnkey */

    for ( kip = sql04_key_list ; kip ; kip = kip->ki_next )
    {
	if ( kip->ki_function < VTKEY_LAST )
	    returnkeys_array [ kip->ki_function ] = TRUE ;
    }

#ifdef DEBUG_RTE
    debug_key_array ( "Returnkey:" , returnkeys_array );
#endif
}

/*==========================================================================*/

/*
 *  Fill the table of available attributes.
 */

static	void	sql04_attrib_array ( attributes_array )

register char  *attributes_array ;

{
    SAPDB_memset ( attributes_array , FALSE , VTA_LAST );

    if ( enter_bold_mode      ) attributes_array[VTA_BRIGHT]    = TRUE ;
    if ( enter_reverse_mode   ) attributes_array[VTA_INVERSE]   = TRUE ;
    if ( enter_underline_mode ) attributes_array[VTA_UNDERLINE] = TRUE ;
    if ( enter_blink_mode     ) attributes_array[VTA_BLINK]     = TRUE ;

    attributes_array[VTA_INVISIBLE] = TRUE ;
    attributes_array[VTA_MIXED]     = TRUE ;

    DBG1 (( MF__,"bright      %d \n", attributes_array[VTA_BRIGHT]    ))
    DBG1 (( MF__,"inverse     %d \n", attributes_array[VTA_INVERSE]   ))
    DBG1 (( MF__,"underline   %d \n", attributes_array[VTA_UNDERLINE] ))
    DBG1 (( MF__,"blink       %d \n", attributes_array[VTA_BLINK]     ))
    DBG1 (( MF__,"invisible   %d \n", attributes_array[VTA_INVISIBLE] ))
    DBG1 (( MF__,"mixed       %d \n", attributes_array[VTA_MIXED]     ))
}

/*==========================================================================*/

/*
 *  Fill the table of available colors.
 */

static	void	sql04_colors_array ( colors_array )

register char  *colors_array ;

{
    int				idx ;


    colors_array[VTC_MONOCHROME] = TRUE ;
    for ( idx = VTC_WHITE ; idx < VTC_LAST ; idx ++ )
	colors_array[idx] = FALSE ;

#if A_COLOR
#ifdef A_BGCOLOR

    if ( bg_color0 && fg_color0 ) colors_array [ VTC_BLACK      ] = TRUE ;
    if ( bg_color1 && fg_color1 ) colors_array [ VTC_RED        ] = TRUE ;
    if ( bg_color2 && fg_color2 ) colors_array [ VTC_GREEN      ] = TRUE ;
    if ( bg_color3 && fg_color3 ) colors_array [ VTC_YELLOW     ] = TRUE ;
    if ( bg_color4 && fg_color4 ) colors_array [ VTC_BLUE       ] = TRUE ;
    if ( bg_color5 && fg_color5 ) colors_array [ VTC_PINK       ] = TRUE ;
    if ( bg_color6 && fg_color6 ) colors_array [ VTC_LIGHT_BLUE ] = TRUE ;
    if ( bg_color7 && fg_color7 ) colors_array [ VTC_WHITE      ] = TRUE ;

    for ( idx = VTC_WHITE ;
	  (idx <= VTC_LIGHT_BLUE) && ! colors_array[idx] ;
	  idx ++ );
    sql04_has_colors = (idx <= VTC_LIGHT_BLUE);

#else /*A_BGCOLOR*/

    DBG1 (( MF__,"COLORS     %d \n" , COLORS ))
    if ( COLORS > COLOR_BLACK   ) colors_array [ VTC_BLACK      ] = TRUE ;
    if ( COLORS > COLOR_RED     ) colors_array [ VTC_RED        ] = TRUE ;
    if ( COLORS > COLOR_GREEN   ) colors_array [ VTC_GREEN      ] = TRUE ;
    if ( COLORS > COLOR_YELLOW  ) colors_array [ VTC_YELLOW     ] = TRUE ;
    if ( COLORS > COLOR_BLUE    ) colors_array [ VTC_BLUE       ] = TRUE ;
    if ( COLORS > COLOR_MAGENTA ) colors_array [ VTC_PINK       ] = TRUE ;
    if ( COLORS > COLOR_CYAN    ) colors_array [ VTC_LIGHT_BLUE ] = TRUE ;
    if ( COLORS > COLOR_WHITE   ) colors_array [ VTC_WHITE      ] = TRUE ;

    sql04_has_colors = has_colors ();

#endif /*A_BGCOLOR*/
#endif /*A_COLOR*/

#ifdef DEBUG_RTE
    for ( idx = VTC_WHITE ; idx <= VTC_LIGHT_BLUE ; idx ++ )
	DBG1 (( MF__,"color[%d]: %d \n", idx , colors_array[idx] ))
    DBG1 (( MF__,"has colors: %d \n" , sql04_has_colors ))
#endif

}

/*==========================================================================*/

/*
 *  Clear screen and update curses's color map.
 */

static	void	sql04_update_colors ( )

{
    int				idx ;


    /*
     *  We need to clear the screen before using init_pair
     *  since init_pair immediately updates the screen with the new colors.
     *  This can lead to multiple screen updates just before a new
     *  screen layout is to be output.
     *  Clearing the screen completly (refresh is required!)
     *  results in a single re-write of the new colored screen layout.
     */
    (void) clear ();
    (void) refresh ();

    for ( idx = 0 ; idx < MAXATTRIBUTES ; idx ++ )
    {
	sql04_attrib_table[idx].att &= ~A_COLOR ;
	sql04_attrib_table[idx].att |= sql04_convert_color ( idx ,
	    sql04_attrib_table[idx].fg, sql04_attrib_table[idx].bg );
    }

    /*
     *  We need a complete re-write of the screen, since we cleared it.
     */
    sql01_vt_clear = TRUE ;
}

/*==========================================================================*/

/*
 *  Convert a foreground/background color pair to a curses attribute.
 */

/*ARGSUSED*/	/* elem unused with screenlib */

static	chtype	sql04_convert_color ( elem , fg , bg )

int			elem ;
enum1   		fg ;
enum1   		bg ;

{
    chtype                      color_attr = 0 ;
    short                       fg_color ;
    short                       bg_color ;


    if ( ! sql04_has_colors )
    {
	DBG1 (( MF__,"cvtcol: no colors, returning attr 0x%08lx\n",
						(long) color_attr ))
	return ( color_attr );
    }

    /*
     *  Use default b/w if all is monochrome
     */
    if ( (bg == VTC_MONOCHROME) && (fg == VTC_MONOCHROME) )
    {
	DBG1 (( MF__,"cvtcol: b/w requested, returning attr 0x%08lx\n",
						(long) color_attr ))
	return ( color_attr );
    }

#if A_COLOR
#ifdef A_BGCOLOR

    switch ( bg )
    {
    case VTC_WHITE      : color_attr = BG_COL7 ; break ;
    case VTC_BLACK      : color_attr = BG_COL0 ; break ;
    case VTC_RED        : color_attr = BG_COL1 ; break ;
    case VTC_GREEN      : color_attr = BG_COL2 ; break ;
    case VTC_YELLOW     : color_attr = BG_COL3 ; break ;
    case VTC_BLUE       : color_attr = BG_COL4 ; break ;
    case VTC_PINK       : color_attr = BG_COL5 ; break ;
    case VTC_LIGHT_BLUE : color_attr = BG_COL6 ; break ;
    }

    switch ( fg )
    {
    case VTC_WHITE      : color_attr |= FG_COL0 ; break ;
    case VTC_BLACK      : color_attr |= FG_COL7 ; break ;
    case VTC_RED        : color_attr |= FG_COL1 ; break ;
    case VTC_GREEN      : color_attr |= FG_COL2 ; break ;
    case VTC_YELLOW     : color_attr |= FG_COL3 ; break ;
    case VTC_BLUE       : color_attr |= FG_COL4 ; break ;
    case VTC_PINK       : color_attr |= FG_COL5 ; break ;
    case VTC_LIGHT_BLUE : color_attr |= FG_COL6 ; break ;
    }

#else /*A_BGCOLOR*/

    switch ( fg )
    {
    case VTC_WHITE      : fg_color = COLOR_WHITE   ; break ;
    case VTC_BLACK      : fg_color = COLOR_BLACK   ; break ;
    case VTC_RED        : fg_color = COLOR_RED     ; break ;
    case VTC_GREEN      : fg_color = COLOR_GREEN   ; break ;
    case VTC_YELLOW     : fg_color = COLOR_YELLOW  ; break ;
    case VTC_BLUE       : fg_color = COLOR_BLUE    ; break ;
    case VTC_PINK       : fg_color = COLOR_MAGENTA ; break ;
    case VTC_LIGHT_BLUE : fg_color = COLOR_CYAN    ; break ;
    }

    switch ( bg )
    {
    case VTC_WHITE      : bg_color = COLOR_WHITE   ; break ;
    case VTC_BLACK      : bg_color = COLOR_BLACK   ; break ;
    case VTC_RED        : bg_color = COLOR_RED     ; break ;
    case VTC_GREEN      : bg_color = COLOR_GREEN   ; break ;
    case VTC_YELLOW     : bg_color = COLOR_YELLOW  ; break ;
    case VTC_BLUE       : bg_color = COLOR_BLUE    ; break ;
    case VTC_PINK       : bg_color = COLOR_MAGENTA ; break ;
    case VTC_LIGHT_BLUE : bg_color = COLOR_CYAN    ; break ;
    }

    if ( fg_color <  0      ) fg_color = 0 ;
    if ( fg_color >  COLORS ) fg_color = (short) COLORS ;
    if ( bg_color <  0      ) bg_color = 0 ;
    if ( bg_color >  COLORS ) bg_color = (short) COLORS ;

    /*
     *  Curses uses pair number 0 for its own purposes.
     */
    elem ++ ;
    if ( elem <  1           ) elem = 1 ;
    if ( elem >= COLOR_PAIRS ) elem = COLOR_PAIRS - 1 ;

    (void) init_pair ( (short) elem , fg_color , bg_color );
    color_attr = COLOR_PAIR ( elem );

#endif /*A_BGCOLOR*/
#endif /*A_COLOR*/

    DBG1 (( MF__,"cvtcol: %2d fg/bg %d/%d (%s/%s) 0x%08lx \n", elem , fg , bg ,
	sql04_color_name[fg] , sql04_color_name[bg] , (long) color_attr ))
    return ( color_attr );
}

/*==========================================================================*/

/*
 *  Extract several options from an options arry.
 */

static	void	sql04_get_options ( c_vt_options )

char			*c_vt_options ;

{
#ifdef	APPL_BUG
    int				i ;
#endif


    sql04_wait_for_input  = (int) c_vt_options [0] ;
    sql04_usage_mode      = (int) c_vt_options [1] ;
    sql04_return_on_last  = (int) c_vt_options [2] ;
    sql04_return_on_first = (int) c_vt_options [3] ;
    sql04_ring_bell       = (int) c_vt_options [4] ;
    sql04_mark_option     = (int) c_vt_options [5] ;
    sql04_mark_option     =  1 ; /*TEST !!!!*/
    sql04_enable_charkey  = (int) c_vt_options [6] ;

    sql04_first_linechg   = ((c_vt_options [7]      ) & 0x00ff) |
			    ((c_vt_options [8]  << 8) & 0xff00) ;
    sql04_last_linechg    = ((c_vt_options [9]      ) & 0x00ff) |
			    ((c_vt_options [10] << 8) & 0xff00) ;

    sql04_enable_ctrlkey  = (int) c_vt_options [11] ;
#ifdef	TEST_CTRLKEY
    sql04_enable_ctrlkey  = TRUE ;
#endif

#ifdef	PRINT_LINES_CHANGED
    if ( (sql04_first_linechg != 1) || (sql04_last_linechg != sql04_lines) )
    {
	int			fst ;
	int			lst ;


	DBG1(( MF__,"(%3d) first %2d, last %2d, lines %2d \n",
	    sql04_tio_count , sql04_first_linechg , sql04_last_linechg ,
	    sql04_lines ))
	lst = sql04_lines * sql04_cols ;
	for ( fst = 0 ; fst < lst ; fst ++ )
	    if ( sql04_tio_screen[fst] != sql04_check_screen[fst] ) break ;
	for ( lst -- ; lst > fst ; lst -- )
	    if ( sql04_tio_screen[lst] != sql04_check_screen[lst] ) break ;
	if ( fst <= lst )
	{
	    DBG1(( MF__,
		"sqltio: first %2d, col %2d ; last %2d, col %2d \n",
		fst/sql04_cols , fst%sql04_cols ,
		lst/sql04_cols , lst%sql04_cols ))
	    DBG1(( MF__,
		"sqltio: screen f %02x l %02x, old f %02x l %02x \n",
		sql04_tio_screen[fst]&0xff, sql04_tio_screen[lst]&0xff,
		sql04_check_screen[fst]&0xff, sql04_check_screen[lst]&0xff ))
	}
    }
    SAPDB_memcpy ( sql04_check_screen , sql04_tio_screen , sql04_lines * sql04_cols );
#endif /*PRINT_LINES_CHANGED*/

#ifdef	GET_ALL_LINES
    sql04_first_linechg = 1 ;
    sql04_last_linechg  = sql04_lines ;
#else /*GET_ALL_LINES*/
#ifdef	APPL_BUG
    if ( sql04_first_linechg < 1 ) sql04_first_linechg = 1 ;
    if ( sql04_first_linechg > sql04_lines ) sql04_first_linechg = sql04_lines ;
    if ( sql04_last_linechg < 1 ) sql04_last_linechg = 1 ;
    if ( sql04_last_linechg > sql04_lines ) sql04_last_linechg = sql04_lines ;
#else /*APPL_BUG*/
    if ( sql04_first_linechg < 1        ||
	 sql04_first_linechg > sql04_lines ||
	 sql04_last_linechg  < 1        ||
	 sql04_last_linechg  > sql04_lines    )
    {
	(void) endwin ();
	(void) SET_TERMIO ( 0 , &sql04_termio_extern);
	DBG1 (( MF__,"restored external terminal settings \n" ))
	DBG1(( MF__,
	    "sqltio: first/last line changed: %2d, %2d, lines: %2d \n",
		sql04_first_linechg , sql04_last_linechg , sql04_lines ))
	sqlabort ();
    }
#endif /*APPL_BUG*/
#endif /*GET_ALL_LINES*/

    DBG1 (( MF__,"wait_for_input     = %d\n", sql04_wait_for_input ))
    DBG1 (( MF__,"usage_mode         = %d\n", sql04_usage_mode ))
    DBG1 (( MF__,"return_on_last     = %d\n", sql04_return_on_last ))
    DBG1 (( MF__,"return_on_first    = %d\n", sql04_return_on_first ))
    DBG1 (( MF__,"ring_bell          = %d\n", sql04_ring_bell ))
    DBG1 (( MF__,"mark_option        = %d\n", sql04_mark_option ))
    DBG1 (( MF__,"enable_charkey     = %d\n", sql04_enable_charkey ))
    DBG1 (( MF__,"first_line_changed = %d\n", sql04_first_linechg ))
    DBG1 (( MF__,"last_line_changed  = %d\n", sql04_last_linechg ))
    DBG1 (( MF__,"enable_ctrlkey     = %d\n", sql04_enable_ctrlkey ))
}

/*==========================================================================*/

/*
 *  Write the full output screen.
 *  To optimize the output, a save screen is created,
 *  which contains the screen text and the correct attribute
 *  for each screen position. (sqltio gets a screen with the end of field
 *  delimiters determining the attribute for the following gap between
 *  the fields.)
 */

static	void	sql04_full_screen ( )

{
    register int		pos ;
    register int		eps ;
    register int		in_field ;
    register UCHAR		nscr ;
    register UCHAR		natt ;


    /*
     *  Get the positions in the screen/attrib buffers
     *  where the first and last changed characters are.
     */
    pos = 0 ;
    eps = sql04_screen_size ;
    in_field = FALSE ;
    natt = sql04_tio_attrib[0];

    /*
     *  Reset the line change values
     *  since 'part_screen' might be called later during input.
     *  It then should only update the modified positions.
     *  The modifying functions will lower 'first_linechg' and
     *  raise 'last_linechg' to the correct values.
     */
    sql04_first_linechg = sql04_lines ;
    sql04_last_linechg  = 1 ;

    /*
     *  Clear the curses buffers.
     */
    (void) clear ();


    for ( ; pos < eps ; pos ++ )
    {
	/*
	 *  Get the current character.
	 */
	nscr = sql04_tio_screen[pos];

	/*
	 *  Check for a field delimiter.
	 */
	if ( nscr < FIELD_DELIMITERS )
	{
	    /*
	     *  A field delimiter changes the in-field state.
	     */
	    if ( nscr == VT_EOF )
	    {
		/*
		 *  End of field delimiter gives the attribute for all
		 *  following out of field characters.
		 *  The delimiter is replaced by a blank.
		 */
		natt = sql04_tio_attrib[pos] ;
		in_field = FALSE ;
	    }
	    else
	    {
		/*
		 *  Start of field delimiter (VT_SONCF || VT_SOCF).
		 *  If previously in field, tis behaves like an eof delimiter.
		 *  Otherwise, it gets the attribute of the leading
		 *  eof delimiter.
		 */
		if ( in_field ) natt = sql04_tio_attrib[pos];
		in_field = TRUE ;
	    }
	    /*
	     *  A field delimiter is displayed as blank.
	     */
	    nscr = BLANK ;
	}
	else
	{
	    /*
	     *  No field delimiter.
	     *  Each character within a field has its own attribute.
	     *  Characters within fields might have been changed.
	     *  The corresponding bit is not set in the save buffers.
	     *  To compare equal, if only the bit is changed,
	     *  strip this bit from the register variable.
	     *  Outside a field, only blanks are displayed.
	     */
	    if ( in_field )
		natt = sql04_tio_attrib[pos] & ~BIT_CHANGED ;
	    else
		nscr = BLANK ;
	}
	/* if nscr < FIELD_DELIMITERS */

	sql04_save_screen[pos] = nscr ;
	sql04_save_attrib[pos] = natt ;
	sql04_addch ( nscr , natt );
    }
    /* for pos */
}

/*==========================================================================*/

/*
 *  Write a partial output screen.
 *  Only those screen positions with changes against the previous screen
 *  are output to curses.
 *  To optimize further output, the save screen is updated.
 *  (see the comments to 'full_screen').
 */

static	void	sql04_part_screen ( )

{
    register int		pos ;
    register int		eps ;
    register int		in_field ;
    register UCHAR		nscr ;
    register UCHAR		natt ;
    register UCHAR		*ptr ;


    /*
     *  Get the positions in the screen/attrib buffers
     *  where the first and last changed characters might be.
     */
    pos = (sql04_first_linechg - 1) * sql04_cols ;
    eps = sql04_last_linechg * sql04_cols ;
    DBG1 (( MF__,"part_screen: pos %d \n", pos ))
    DBG1 (( MF__,"part_screen: eps %d \n", eps ))

    /*
     *  If we start at screen begin, the first character's attribute
     *  is always correct and we are not in a field.
     *  If we don't start with the first character,
     *  we have to determine if we are in a field or not.
     *  In a field each position has its own valid attribute,
     *  while outside a field the end of field delimiter determines
     *  the attribute for all following positions.
     */
    if ( pos == 0 )
    {
	in_field = FALSE ;
	natt = sql04_tio_attrib[0];
    }
    else
    {
	/*
	 *  Start checking the position before the first changed position.
	 */
	pos -- ;
	ptr = sql04_tio_screen + pos ;
	while ( (pos > 0) && (*ptr >= FIELD_DELIMITERS) )
	{
	    pos -- ;
	    ptr -- ;
	}
	/*
	 *  If at a start of field, the starting attribute is insignificant
	 *  since each field position has its own attribute.
	 *  At an end of field, the attribute is correct for the new line.
	 */
	if ( (*ptr == VT_SONCF) || (*ptr == VT_SOCF) )
	    in_field = TRUE ;
	else
	    in_field = FALSE ;
	natt = sql04_tio_attrib[pos];
	DBG1 (( MF__,"part_screen: pos %d: 0x%02x %s in field \n",
		    pos , (*ptr)&0xff , in_field?"":"not" ))
	/*
	 *  Restart at the beginning of the new line.
	 */
	pos = (sql04_first_linechg - 1) * sql04_cols ;
    }
    DBG1 (( MF__,"part_screen: atb 0x%02x \n", natt&0xff ))

    /*
     *  Reset the line change values
     *  since 'part_screen' might be called later during input.
     *  It then should only update the modified positions.
     *  The modifying functions will lower 'first_linechg' and
     *  raise 'last_linechg' to the correct values.
     */
    sql04_first_linechg = sql04_lines ;
    sql04_last_linechg  = 1 ;


    for ( ; pos < eps ; pos ++ )
    {
	/*
	 *  Get the current character.
	 */
	nscr = sql04_tio_screen[pos];

	/*
	 *  Check for a field delimiter.
	 */
	if ( nscr < FIELD_DELIMITERS )
	{
	    /*
	     *  A field delimiter changes the in-field state.
	     */
	    if ( nscr == VT_EOF )
	    {
		/*
		 *  End of field delimiter gives the attribute for all
		 *  following out of field characters.
		 *  The delimiter is replaced by a blank.
		 */
		natt = sql04_tio_attrib[pos] ;
		in_field = FALSE ;
	    }
	    else
	    {
		/*
		 *  Start of field delimiter (VT_SONCF || VT_SOCF).
		 *  If previously in field, tis behaves like an eof delimiter.
		 *  Otherwise, it gets the attribute of the leading
		 *  eof delimiter.
		 */
		if ( in_field ) natt = sql04_tio_attrib[pos];
		in_field = TRUE ;
	    }
	    /*
	     *  A field delimiter is displayed as blank.
	     */
	    nscr = BLANK ;
	}
	else
	{
	    /*
	     *  No field delimiter.
	     *  Each character within a field has its own attribute.
	     *  Characters within fields might have been changed.
	     *  The corresponding bit is not set in the save buffers.
	     *  To compare equal, if only the bit is changed,
	     *  strip this bit from the register variable.
	     *  Outside a field, only blanks are displayed.
	     */
	    if ( in_field )
		natt = sql04_tio_attrib[pos] & ~BIT_CHANGED ;
	    else
		nscr = BLANK ;
	}
	/* if nscr < FIELD_DELIMITERS */

	if ( (sql04_save_screen[pos] != nscr) ||
	     (sql04_save_attrib[pos] != natt)    )
	{
#if DEBUG_DEBUG
	    DBG1 (( MF__,"part_screen: line %02d col %02d from '%c' to '%c' \n",
	          LINE(pos) , COLUMN(pos) , sql04_save_screen[pos] , nscr ))
#endif /*DEBUG_DEBUG*/
	    sql04_save_screen[pos] = nscr ;
	    sql04_save_attrib[pos] = natt ;
	    (void) move ( LINE(pos) , COLUMN(pos) );
	    sql04_addch ( nscr , natt );
	}
    }
    /* for pos */
}

/*==========================================================================*/

/* Ausgabe der Statuszeile */

static	void	sql04_write_sysline ( c_sys_text , c_sys_attr ) /* screenlib */

UCHAR  *c_sys_text ;
UCHAR  *c_sys_attr ;

{
    int				i ;
    int				syslgt ;


    /* Falls keine hardwaremaessige Message-Zeile existiert, wird diese hinter
       der letzten Terminal-Zeile ausgegeben. Die Position muss gegebenfalls
       noch geaendert werden, falls die Softkeys in dieser Zeile ausgegeben
       werden */

#ifdef SCRLIB

    chtype			blank_attrib ;


    (void) move (LINES - 2, 0);
    (void) refresh ();                  /* Macke in Scrlib f. vt220 ... */
    msg_mode ();                        /* In den Message-Mode schalten */
    (void) attrset ( A_NORMAL );        /* sure is sure */

    if ( sql04_has_colors )
	(void) addch ( CHBLANK );       /* otherwise we may have a colored
					   first character (very very bad) in
					   report ; my problem ??? */

    else
	(void) addch ( CHBLANK | ATTR(c_sys_attr[0]) );

    for (i = 1 ; (c_sys_text[i-1] != 1) && (i < sql04_cols - 1); i++)
	(void) addch ( CHBLANK | ATTR(c_sys_attr[0]) );

    while (i < sql04_cols /* - 1 */ )
    {

	for (; (i < sql04_cols - 1) && ( c_sys_text[i] & 0xfe ); i++)
	{
	    sql04_addch ( c_sys_text[i] , c_sys_attr[i] );
	}

	blank_attrib = ATTR(c_sys_attr[i]);

	if ( i >= sql04_cols - 1 )
	    break ;
	else
	    if (c_sys_text[i] == 1)
		(void) addch ( CHBLANK | ATTR(c_sys_attr[i++]) );
	    else  /* == 0 */
		for (; (c_sys_text[i] != 1) && (i < sql04_cols /* - 1 */ ); i++)
		    (void) addch ( CHBLANK | blank_attrib );
    }

    (void) attrset ( blank_attrib );
    clrtoeol ();
    (void) attrset ( A_NORMAL );      /* sure is sure */

    cbol ();
    (void) refresh ();
    screen_mode ();

#else /*SCRLIB*/

    if ( ! has_status_line || ! to_status_line || ! from_status_line ) return ;
    if ( (int) (width_status_line) > 0 )
	syslgt = width_status_line < sql04_cols ?
		 width_status_line : sql04_cols ;
    else
	syslgt = sql04_cols ;

    (void) vidattr ( A_NORMAL );
    (void) putp ( tparm ( to_status_line , 0L , /* column */
		    /* dummies: */ 0L , 0L , 0L , 0L , 0L , 0L , 0L , 0L ) );
    (void) vidattr ( ATTR(c_sys_attr[0]) );
    for ( i = 0 ; (i < syslgt) && (c_sys_text[i] != 1) ; i ++ )
	(void) putchar ( ' ' );
    if ( i < syslgt )
    {
	(void) putchar ( ' ' );	/* for the SOF character */
	i ++ ;			/* skip the SOF character */
    }

    while ( i < syslgt )
    {
				/* end if 0 or 1 found (EOF or SOF) */
	for ( ; (i < syslgt) && (c_sys_text[i] & 0xfe) ; i ++ )
	{
	    if ( c_sys_attr[i] != c_sys_attr[i-1] )
		(void) vidattr ( ATTR(c_sys_attr[i]) );
	    (void) putchar ( c_sys_text[i] );
	}

	if ( i >= syslgt ) break ;
	if ( c_sys_text[i] == 1 )	/* SOF */
	{
	    if ( c_sys_attr[i] != c_sys_attr[i-1] )
		(void) vidattr ( ATTR(c_sys_attr[i]) );
	    (void) putchar ( ' ' );
	    i ++ ;
	}
	else				/* == 0 (EOF) */
	{
	    if ( c_sys_attr[i] != c_sys_attr[i-1] )
		(void) vidattr ( ATTR(c_sys_attr[i]) );
	    for ( ; (i < syslgt) && (c_sys_text[i] != 1) ; i ++ )
		(void) putchar ( ' ' );
	}
    }

    if ( (i < syslgt) && (c_sys_attr[i] != c_sys_attr[i-1]) )
	(void) vidattr ( ATTR(c_sys_attr[i]) );
    for ( ; i < syslgt ; i ++ )
	(void) putchar ( ' ' );
    (void) vidattr ( A_NORMAL );
    (void) putp ( from_status_line );
    (void) fflush ( stdout );

#endif /*SCRLIB*/
}

/*==========================================================================*/

/* Schreiben der Softkeys */

static	void	sql04_write_softkeys ( c_sk_text )

UCHAR           *c_sk_text ;

{
#ifdef SCRLIB

    extern VS       vs_fd ;
    int             skanz ;
    int             sklen ;
    char            i ;
    struct softkeys sks ;


    (void) attrset ( A_NORMAL ); /* Softkeys mit eingeschalteten Attr. gibt
				  * offenbar Murks auf vt220 etc ... */

    (void) refresh ();           /* wegen color-pc etc. , Softkeys werden
				  * sonst sofort wieder geloescht */

    skanz = f_skanz ();
    sklen = f_sk_length ();

    DBG1 (( MF__,"write_softkeys :         skanz = %d\n", skanz))
    DBG1 (( MF__,"write_softkeys :         sklen = %d\n", sklen))

    sks.anzahl = (short) skanz ;

    for ( i = 0 ; i < skanz ; i++ )
    {
	sks.sk[i].softkeyno = (short) i ;

	/* zum Testen , laueft sonst nicht mit scrlib-Vers. > 1.2 */
	SAPDB_memset ((char *) sks.sk[i].text, ' ', 17);

	(void) strncpy (sks.sk[i].text, c_sk_text + 16 * i, sklen);

	DBG1 (( MF__,"write_softkeys :         sks.sk[i].text = %s\n", sks.sk[i].text))
    }

    w_sftkys ( &vs_fd , &sks );
    (void) attrset ( A_NORMAL );      /* sure is sure */

#else /*SCRLIB*/

    if ( ! sql04_hif_softkeys ) return ;
    (void) putp ( tparm ( sql04_hif_softkeys ,
	    (long)c_sk_text    , (long)c_sk_text+16 , (long)c_sk_text+32 ,
	    (long)c_sk_text+48 , (long)c_sk_text+64 , (long)c_sk_text+80 ,
	    (long)c_sk_text+96 , (long)c_sk_text+112 , (long)c_sk_text+128 ) );
    (void) fflush ( stdout );

#endif /*SCRLIB*/
}

/*==========================================================================*/

/*
 *  Read a key or character from the keyboard.
 */

static	int	sql04_get_key ( )

{
    int				rc ;
    int				keyfunc ;
    int				ich ;
    int				seqpos ;
    struct key_info		**first_match ;
    struct key_info		**last_match ;
    struct key_info		**cur ;
    char			buf [ 2 ];


    DBG1 (( MF__,"get_key: called \n" ))

    /*
     *  -1 is an illegal key function value. A key sequence is
     *  not matched as long as this value is negative.
     *  key_value is a global variable holding the secondary value
     *  for the key functions VTKEY_CHARACTER and VTKEY_CONTROL.
     */
    keyfunc         = -1 ;
    sql04_key_value = 0 ;

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
    for ( seqpos = 0 ; ; seqpos ++ )
    {
	DBGG (( "get_key: seqpos    %d \n", seqpos ))

	/*
	 *  Read a character from STDIN.
	 */
	rc = read ( 0 , buf , 1 );
	if ( rc < 0 )
	{
	    DBG1(( MF__, "get_key: read error: %s \n", sqlerrs() ))
	    if ( errno == EINTR )
		keyfunc = VTKEY_CANCEL ;
	    else
		keyfunc = VTKEY_END ;
	    break ;
	}
	if ( rc == 0 )
	{
	    DBG1 (( MF__,"get_key: end of file during read \n" ))
	    keyfunc = VTKEY_END ;
	    break ;
	}
	ich = buf[0] & 0xff ;
	DBGG (( "get_key: got character 0x%02x \n", ich ))

	/*
	 *  tic(1M) converts '\0' to '\200'.
	 */
	if ( ich == 0 ) ich = 0200 ;

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

    /*
     *  If the key has already been determined in the loop,
     *  the work is finished.
     */
    if ( keyfunc >= 0 )
    {
	if ( sql04_usage_mode != VT_EDIT )
	{
	    switch ( keyfunc )
	    {
	    case VTKEY_TAB :
		keyfunc = VTKEY_NEXT_FIELD ;
		break ;
	    case VTKEY_BACK_TAB :
		keyfunc = VTKEY_PREV_FIELD ;
		break ;
	    }
	}
	DBG1 (( MF__,"get_key: returning key: %s \n", sql04_key_name(keyfunc) ))
	return ( keyfunc );
    }

    /*
     *  We received a character that did not match any of the
     *  keys in the list.
     *  If it was NOT the first character, the begin of one or more
     *  key sequences had been matched by previously
     *  received characters. The current characters belongs to a
     *  key sequence similar to those in the list.
     *  Thus the received sequence comes from an undefined key.
     */
    if ( seqpos != 0 )
    {
	keyfunc = VTKEY_UNKNOWN ;
	DBG1 (( MF__,"get_key: returning key: %s \n", sql04_key_name(keyfunc) ))
	return ( keyfunc );
    }

/* MAP CHARACTERS HERE (IBM -> ISO) */

    /*
     *  We received a character, not a key.
     *  If it is printable, return it in the global variable key_value.
     */
    if ( sql04_isprint(ich) )
    {
	keyfunc = VTKEY_CHARACTER ;
	sql04_key_value = ich ;
	DBG1 (( MF__,"get_key: returning key: %s \n", sql04_key_name(keyfunc) ))
	return ( keyfunc );
    }

    if ( sql04_enable_ctrlkey && (ich >= CTRL('A')) && (ich <= CTRL('Z')) )
    {
	DBG1 (( MF__,"get_key: control_key '%c' \n", ich + 'A' - CTRL('A') ))
	switch ( ich )
	{
	case CTRL('C') :
	case CTRL('Y') :
	    (void) kill ( getpid() , (ich == CTRL('C')) ? SIGINT : SIGQUIT );
	    keyfunc = VTKEY_UNKNOWN ;
	    DBG1 (( MF__,"get_key: returning key: %s \n", sql04_key_name(keyfunc) ))
	    return ( keyfunc );

	case CTRL('H') :	/* identical with BACKSPACE key */
	case CTRL('I') :	/* identical with TAB key */
	case CTRL('M') :	/* identical with RETURN key */
	    break ;

	default :
	    keyfunc = VTKEY_CONTROL ;
	    sql04_key_value = ich - CTRL('A') + 'A' ;
	    DBG1 (( MF__,"get_key: returning key: %s \n", sql04_key_name(keyfunc) ))
	    return ( keyfunc );
	}
    }

    keyfunc = VTKEY_UNKNOWN ;

    switch ( ich )
    {
    /*
     *  Some Control Characters have natural equivalences
     *  in the set of VTKEYs.
     */

    case '\b' :                     /* BACKSPACE */
	keyfunc = VTKEY_RUBOUT ;
	break ;

    case '\t' :                     /* TAB */
	if ( sql04_usage_mode == VT_EDIT )
	    keyfunc = VTKEY_TAB ;
	else
	    keyfunc = VTKEY_NEXT_FIELD ;
	break ;

    case '\n' :                     /* NEWLINE */
    case '\r' :                     /* RETURN */
	if ( sql04_usage_mode == VT_EDIT )
	    keyfunc = VTKEY_NEXT_FIELD ;
	else
	    keyfunc = VTKEY_DO ;
	break ;

    case 0x7f :                     /* DEL */
	keyfunc = VTKEY_DELETE_CHAR ;
	break ;

    /*
     *  The Control Characters below simulate some important keys
     *  which are not present on all keyboards.
     */

    case CTRL('D') :
	keyfunc = VTKEY_ENTER ;
	break ;

    case CTRL('E') :		/* see also CTRL('Z') */
	keyfunc = VTKEY_END ;
	break ;

    case CTRL('F') :
	keyfunc = VTKEY_CLEAR ;
	break ;

    case CTRL('K') :
	if ( sql04_usage_mode == VT_EDIT )
	    keyfunc = VTKEY_BACK_TAB ;
	else
	    keyfunc = VTKEY_PREV_FIELD ;
	break ;

    case CTRL('L') :
	keyfunc = VTKEY_REFRESH ;
	break ;

    case CTRL('R') :
	keyfunc = VTKEY_CMD ;
	break ;

    case CTRL('X') :
	keyfunc = VTKEY_DEL_EOF ;
	break ;

    case CTRL('Z') :		/* see also CTRL('E') */
	keyfunc = VTKEY_END ;
	break ;

    }

    DBG1 (( MF__,"get_key: returning key: %s \n", sql04_key_name(keyfunc) ))
    return ( keyfunc );
}

/*==========================================================================*/

/*
 *  Get a character and put it to the screen buffer.
 */

static	int	sql04_input_character ( )

{
    int				keyfunc ;
    int				lgt ;
    char			buf [ 16 ];


    keyfunc = sql04_get_key ();
    if ( keyfunc != VTKEY_CHARACTER )
    {
	return ( keyfunc );
    }

    lgt = 0 ;
    buf[lgt ++] = (char) sql04_key_value ;

#ifdef KANJI
    /*
     *  Collect a complete (Multi-Byte-) character.
     */
    {
	tsp00_Int4		len ;

	if ( sql04_encoding_ptr && sql04_encoding_cnt )
	{
	    while (    (lgt < sizeof(buf))
		    && (len = lgt , s80uni_chartype ( (tsp00_MoveObjPtr)buf , &len ,
					sql04_codeset ) == uni_is_incomplete) )
	    {
		keyfunc = sql04_get_key ();
		if ( keyfunc != VTKEY_CHARACTER )
		{
		    MSGD (( -11004, ERR_TYPE, "VTERM   ",
				"inputchr: key within kanji sequence \n" ));
		    sql04_beep_flash ();
		    return ( keyfunc );
		}
		buf[lgt ++] = (char) sql04_key_value ;
	    }
	    if ( lgt >= sizeof(buf) )
	    {
		    MSGD (( -11004, ERR_TYPE, "VTERM   ",
				 "inputchr: kanji sequence too long %d \n", lgt ));
			sql04_beep_flash ();
			return ( keyfunc );
	    }
	}
    }
#endif

    sql04_put_charstring ( buf , lgt );

    return ( VTKEY_CHARACTER );
}

/*==========================================================================*/

/*
 *  Put a character which may be built by multiple bytes (e.g. Kanji).
 */

static	void	sql04_put_charstring ( buf , lgt )

char			*buf ;
int			lgt ;

{
    int				idx ;
    int				high_pos ;


#ifdef DEBUG_RTE
    DBG1 (( MF__,"putstr: at pos %d put 0x%02x ", sql04_position , buf[0]&0xff ))
    for ( idx = 1 ; idx < lgt ; idx ++ ) DBG1 (( MF__,"0x%02x ", buf[idx]&0xff ))
    DBG1 (( MF__,"\n" ))
#endif

    if ( ! sql04_in_field(sql04_position) )
    {
	DBG1 (( MF__,"putstr: not in field \n" ))
	sql04_beep_flash ();
	return ;
    }

    /*
     *  Cannot write to protected positions.
     */
    if ( is_protect(sql04_save_attrib[sql04_position]) )
    {
	DBG1 (( MF__,"putstr: at protected position \n" ))
	sql04_beep_flash ();
	/*
	 *  In a mixed field advance cursor to next unprotected position.
	 */
	if ( is_mixed(sql04_save_attrib[sql04_position]) )
	{
	    sql04_next_unprot ();
	    DBG1 (( MF__,"putstr: (mixed) found position %d \n", sql04_position ))
	}
	return ;
    }

    /*
     *  If in overtype mode, the character at the current position
     *  is removed before inserting the new one.
     */
    if ( ! sql04_insert_mode )
    {
	sql04_delete ();
    }

    /*
     *  Shift characters to the right.
     */
    for ( idx = 0 ; idx < lgt ; idx ++ )
    {
	high_pos = sql04_shift_right ( sql04_position );
	if ( high_pos < 0 )
	{
	    /*
	     *  The input field is already full.
	     */
	    while ( idx -- ) sql04_shift_left ( sql04_position );
	    /*IST XXX insert label should be switched to blink !*/
	    sql04_beep_flash ();
	    return ;
	}
    }
    /*
     *  This is the highest position, where a character has been modified.
     */
    sql04_position_changed ( high_pos );

    /*
     *  Update buffers.
     */
    for ( idx = 0 ; idx < lgt ; idx ++ )
    {
	sql04_tio_screen[sql04_position+idx] = (UCHAR) buf[idx];
	sql04_tio_attrib[sql04_position+idx] |= BIT_CHANGED ;
#ifdef KANJI
	sql04_charmodes[sql04_position+idx] = idx ? 1 : 0 ;
#endif
    }
    sql04_screen_changed = TRUE ;
    sql04_position_changed ( sql04_position );
#ifdef KANJI
    /*
     *  Position on the last byte of the character.
     */
    sql04_position += lgt - 1 ;
#endif

    /*
     *  Automatically advance the cursor to the next unprotected
     *  position. This may move "behind last".
     */
    sql04_next_unprot ();
    DBG1 (( MF__,"putstr: returning position %d \n", sql04_position ))
}

/*==========================================================================*/

/*
 *  Determine the insert mode for the current 'sqltio'.
 *  'insert_mode' is normally saved over several calls to 'sqltio'.
 *  If, however, 'sqltio' is called with an empty insert label,
 *  insert is initially switched off for this call.
 */

static	void	sql04_init_ins ( insert_pos , insert_text , insert_opt )

tsp00_Int2		*insert_pos ;
UCHAR			*insert_text ;
UCHAR			*insert_opt ;

{
    register int		pos ;


    sql04_insert_save = sql04_insert_mode ;
    pos = (insert_pos[0] - 1) * sql04_cols + (insert_pos[1] - 1);

    /*
     *  If there is no insert label, temporarily switch insert mode off.
     */
    if ( (pos <= 0) ||
	 (pos + VT_LABEL_SIZE >= sql04_screen_size) )
    {
	sql04_insert_mode = FALSE ;
    }
    DBG1 (( MF__,"iniins: old %d, pos %d, new %d \n",
	sql04_insert_save , pos , sql04_insert_mode ))

    /*
     *  If insert mode is active, display the insert label.
     */
    if ( sql04_insert_mode )
    {
	sql04_insert_on ( insert_pos , insert_text , insert_opt );
    }
}

/*==========================================================================*/

/*
 *  Save the insert mode for the next 'sqltio'.
 *  'insert_mode' is normally saved over several calls to 'sqltio'.
 *  If, however, 'sqltio' has been called with an empty insert label,
 *  insert has been switched off for this call and will be reset to its
 *  original state.
 */

static	void	sql04_reset_ins ( insert_pos )

tsp00_Int2		*insert_pos ;

{
    register int		pos ;


    pos = (insert_pos[0] - 1) * sql04_cols + (insert_pos[1] - 1);
    DBG1 (( MF__,"resins: new %d, pos %d, old %d \n",
	sql04_insert_mode , pos , sql04_insert_save ))

    /*
     *  If there is no insert label, reset insert mode.
     */
    if ( (pos <= 0) ||
	 (pos + VT_LABEL_SIZE >= sql04_screen_size) )
    {
	sql04_insert_mode = sql04_insert_save ;
    }
    else
    {
	/*
	 *  If insert mode is active, clear the insert label.
	 *  (It might be displayed elsewhere at the next 'sqltio')
	 */
	if ( sql04_insert_mode )
	{
	    sql04_insert_off ( insert_pos );
	    /* insert_mode is cleared by sql04_insert_off */
	    sql04_insert_mode = TRUE ;
	}
    }
}

/*==========================================================================*/

/*
 *  Switch the insert mode on and display the insert label.
 */

static	void	sql04_insert_on ( insert_pos , insert_text , insert_opt )

tsp00_Int2		*insert_pos ;
UCHAR			*insert_text ;
UCHAR			*insert_opt ;

{
    register int		pos ;
    register int		i ;
    register UCHAR		*ptr ;
    register UCHAR		atb ;


    sql04_insert_mode = TRUE ;

    pos = (insert_pos[0] - 1) * sql04_cols + (insert_pos[1] - 1);
    DBG1 (( MF__,"inson:  pos %d, label '%.8s' \n", pos , insert_text ))

    if ( (pos > 0) &&
	 (pos + VT_LABEL_SIZE < sql04_screen_size) )
    {
	ptr = insert_text ;
	atb = *insert_opt ;
	(void) move ( LINE(pos) , COLUMN(pos) );

	for ( i = 0 ; i < VT_LABEL_SIZE ; i ++ )
	{
	    sql04_addch ( *(ptr++) , atb );
	}
    }
}

/*==========================================================================*/

/*
 *  Switch the insert mode on and replace the insert label
 *  with the original screen contents.
 */

static	void	sql04_insert_off ( insert_pos )

tsp00_Int2		*insert_pos ;

{
    register int		pos ;
    register int		i ;
    register UCHAR		*ptr ;
    register UCHAR		*atp ;


    sql04_insert_mode = FALSE ;

    pos = (insert_pos[0] - 1) * sql04_cols + (insert_pos[1] - 1);
    DBG1 (( MF__,"insoff: pos %d, oldscreen '", pos ))

    if ( (pos > 0) &&
	 (pos + VT_LABEL_SIZE < sql04_screen_size) )
    {
	ptr = sql04_save_screen + pos ;
	atp = sql04_save_attrib + pos ;
	(void) move ( LINE(pos) , COLUMN(pos) );

	for ( i = 0 ; i < VT_LABEL_SIZE ; i ++ )
	{
	    DBG1 (( MF__,"%c" , *ptr ))
	    sql04_addch ( *(ptr++) , *(atp++) );
	}
    }
    DBG1 (( MF__,"' \n" ))
}

/*==========================================================================*/

static	void	sql04_local_keys ( keyfunc )

int			keyfunc ;

{
    register int		pos ;
    register int		col ;


    pos = sql04_position ;

    switch ( keyfunc )
    {
    case VTKEY_CURSOR_UP:
	pos -= sql04_cols ;
	break ;

    case VTKEY_CURSOR_DOWN:
	pos += sql04_cols ;
	break ;

    case VTKEY_CURSOR_RIGHT:
	pos ++ ;
	break ;

    case VTKEY_CURSOR_LEFT:
	pos -- ;
	break ;

    case VTKEY_CURSOR_HOME:
	col = sql04_position ;	/* save old position */
	sql04_position = 0 ;
	pos = sql04_next_field ();
	sql04_position = col ;	/* restore old position */
	break ;

    case VTKEY_CURSOR_END:
	col = sql04_position ;	/* save old position */
	sql04_position = sql04_screen_size-1 ;
	pos = sql04_prev_field ();
	sql04_position = col ;	/* restore old position */
	break ;

    case VTKEY_NEXT_FIELD:
	pos = sql04_next_field ();
	break ;

    case VTKEY_PREV_FIELD :
	pos = sql04_prev_field ();
	break ;

    case VTKEY_DELETE_CHAR :
	sql04_delete ();
	break ;

    case VTKEY_RUBOUT:
	pos = sql04_rubout ();
	break ;

    case VTKEY_DEL_EOF:
	sql04_erase_to_eof ();
	break ;

    case VTKEY_TAB:
	col = COLUMN(pos);
	if ( col + DELTA_TAB >= sql04_cols )
	    pos += sql04_cols - col ; /* start of next line */
	else
	    pos += (col / DELTA_TAB + 1) * DELTA_TAB - col ;
	break ;

    case VTKEY_BACK_TAB:
	col = COLUMN(pos);
	if ( col - DELTA_TAB < 0 )
	{
	    pos -= col ;
	    col = sql04_cols ; /* past end of previous line */
	}
	pos -= col - (col - 1) / DELTA_TAB * DELTA_TAB ;
	break ;

    default:
	sql04_beep_flash ();
	break ;
    }

#ifdef KANJI
    /*
     *  Except when trying to move one character right,
     *  position at the beginning of a character by moving left.
     *  This prevents curses from optimizing by outputting only
     *  a part of the character.
     */
    if ( pos == sql04_position + 1 )
	col = 1 ;
    else
	col = -1 ;
    while ( (pos > 0) && (pos <= sql04_screen_size) && sql04_charmodes[pos] )
    {
	pos += col ;
    }
#endif

    /*
     *  Don't allow movements behind the last field,
     *  if 'return_on_last' is requested.
     */
    if ( sql04_return_on_last )
    {
	if ( pos >= sql04_screen_size )
	{
	    /* tried to move behind screen */
	    sql04_behind_last = TRUE ;
	    return ;
	}
	if ( (pos >= 0) &&
	     !sql04_in_field(pos) &&
	     (sql04_find_next(pos , sql04_screen_size) < 0) )
	{
	    /* tried to move behind last field */
	    sql04_behind_last = TRUE ;
	    return ;
	}
    }

    /*
     *  Don't allow movements before the first field,
     *  if 'return_on_first' is requested.
     */
    if ( sql04_return_on_first )
    {
	if ( pos < 0 )
	{
	    /* tried to move before screen */
	    sql04_before_first = TRUE ;
	    return ;
	}
	if ( (pos < sql04_screen_size) &&
	     !sql04_in_field(pos) &&
	     (sql04_find_prev(pos , -1 ) < 0) )
	{
	    /* tried to move before first field */
	    sql04_before_first = TRUE ;
	    return ;
	}
    }

    /*
     *  Some local key functions may wrap around the screen.
     */
    if ( pos < 0 ) pos += sql04_screen_size ;
    if ( pos >= sql04_screen_size ) pos -= sql04_screen_size ;

    /*
     *  If the position changed, we have to update the global 'position'
     *  and probably 'displpos'.
     */
    if ( pos != sql04_position )
    {
	/*
	 *  If both positions are the same,
	 *  or if the new position is outside a field,
	 *  or if the new position is visible,
	 *  or if the new position is in a different field
	 *  then set the display cursor to the same new position
	 *  as the buffer cursor.
	 */
	if ( (sql04_displpos == sql04_position) ||
	     ! sql04_in_field(pos) ||
	     ! is_invisible(sql04_tio_attrib[pos]) ||
	     (sql04_find_eof(sql04_displpos) != sql04_find_eof(pos)) )
	{
	    sql04_displpos = pos ;
	}
	sql04_position = pos ;
    }
}

/*==========================================================================*/

/*
 *  Erase the character at the cursor position and
 *  shift the characters right from the cursor position left.
 */

static	void	sql04_delete ( )

{
    register int		pos ;
    register UCHAR		*atp ;
#ifdef KANJI
    int				cnt ;
#endif


    pos = sql04_position ;
    atp = sql04_tio_attrib + pos ;

    if ( !sql04_in_field(pos) || is_protect(*atp) )
    {
	sql04_beep_flash ();
	return ;
    }

    /*
     *  This position is the lowest one to be modified by 'delete'.
     */
    sql04_position_changed ( pos );

#ifdef KANJI
    cnt = 0 ;
    do
    {
	pos = sql04_position ;
#endif

	/*
	 *  Shift the characters right from this position to the left.
	 */
	pos = sql04_shift_left ( pos );

	/*
	 *  This position is the highest one to be modified by 'delete'.
	 */
	sql04_position_changed ( pos );

	/*
	 *  Blank the specified position out.
	 */
	sql04_tio_screen[pos] = BLANK ;
	sql04_tio_attrib[pos] |= BIT_CHANGED ;
	sql04_screen_changed = TRUE ;

#ifdef KANJI
	sql04_charmodes[pos] = 0 ;
    }
    while (    (++cnt < sql04_screen_size) /* avoid infinite loop */
	    && sql04_charmodes[sql04_position] );
#endif
}

/*==========================================================================*/

/*
 *  Erase the character to the left of the cursor.
 *  This is also allowed if cursor is at the end of field delimiter
 *  or at the first position of a continuation field.
 *  If in insert mode, the characters right from the new cursor
 *  position are shifted left.
 */

static	int	sql04_rubout ( )

{
    register int		pos ;
    register int		eps ;
    register UCHAR		*ptr ;
    register UCHAR		*atp ;
    int				rubout_pos ;


    pos = sql04_position ;
    eps = -1 ;

#ifdef KANJI
    /*
     *  Find the first byte of the current character.
     */
    do
    {
#endif

	pos -- ;
	ptr = sql04_tio_screen + pos ;
	atp = sql04_tio_attrib + pos ;

	if ( pos <= eps )
	{
	    sql04_beep_flash ();
	    return ( sql04_position );
	}

	/*
	 *  If the cursor was at the first position of a continuation field,
	 *  delete the last character of the previous field.
	 */
	if ( (*ptr == VT_SOCF) &&
	     (*(ptr+1) >= FIELD_DELIMITERS) &&
	     is_unprot(*(atp+1)) )
	{
	    pos = sql04_find_prev ( pos , eps );
	    if ( pos < eps )
	    {
		sql04_beep_flash ();
		return ( sql04_position );
	    }
	    pos = sql04_find_eof ( pos );
	    ptr = sql04_tio_screen + pos ;
	    atp = sql04_tio_attrib + pos ;
	}

	/*
	 *  If neither before nor now in field, there is nothing to delete.
	 */
	if ( ! sql04_in_field(pos) || is_protect(*atp) )
	{
	    sql04_beep_flash ();
	    return ( sql04_position );
	}

#ifdef KANJI
    }
    while ( sql04_charmodes[pos] );
#endif

    /*
     *  This is the lowest position to be modified by 'rubout'.
     */
    sql04_position_changed ( pos );

    /*
     *  Position at the first byte of the character
     *  and delete the whole character.
     */
    rubout_pos = pos ;
    pos = sql04_position ;
    sql04_position = rubout_pos ;
    sql04_delete ();

    /*
     *  If not in insert mode, the current position is overtyped
     *  with a blank instead of being removed.
     *  After having removed a character, insert a blank here.
     */
    if ( ! sql04_insert_mode )
    {
	sql04_insert_mode = TRUE ;
	sql04_put_charstring ( " " , 1 );
	sql04_insert_mode = FALSE ;
    }

    sql04_position = pos ;
    return ( rubout_pos );
}

/*==========================================================================*/

/*
 *  Fill the rest of the current field with blanks.
 *  If a mixed field contains a protected position, stop at it.
 *  Don't erase continuation fields.
 */

static	void	sql04_erase_to_eof ( )

{
    register int		pos ;
    register int		eps ;
    register UCHAR		*atp ;
    register UCHAR		*ptr ;


    pos = sql04_position ;
    eps = sql04_screen_size ;
    ptr = sql04_tio_screen + pos ;
    atp = sql04_tio_attrib + pos ;

    if ( ! sql04_in_field(pos) || is_protect(*atp) )
    {
	sql04_beep_flash ();
	return ;
    }

    sql04_position_changed ( pos );
    while ( (pos < eps) && (*ptr >= FIELD_DELIMITERS) && is_unprot(*atp) )
    {
	*ptr = BLANK ;
	*atp |= BIT_CHANGED ;
#ifdef KANJI
	sql04_charmodes[pos] = 0 ;
#endif
	pos ++ ;
	ptr ++ ;
	atp ++ ;
    }
    sql04_screen_changed = TRUE ;
    sql04_position_changed ( pos );
}

/*==========================================================================*/

/*
 *  Shift the characters right of 'position' one position to the left.
 *  Shift through all continuation fields.
 *  Stop at a protected position.
 *  Update the 'tio_screen' buffer only.
 *  Return the position of the last unprotected character of the fields.
 */

static	int	sql04_shift_left ( start_position )

int			start_position ;

{
    register int		pos ;
    register int		eps ;
    register UCHAR		*ptr ;
    register UCHAR		*atp ;
    int				lastpos ;


    pos = start_position ;
    eps = sql04_screen_size ;
    ptr = sql04_tio_screen + pos ;
    atp = sql04_tio_attrib + pos ;

    /*
     *  We may have to shift characters through several continuation fields.
     */
    for ( ; ; )
    {
	/*
	 *  Start with the first character which is to be shifted left.
	 */
	pos ++ ;
	ptr ++ ;
	atp ++ ;
	lastpos = pos ;

	/*
	 *  Shift all characters in the field left.
	 */
	while ( (pos < eps) && (*ptr >= FIELD_DELIMITERS) && is_unprot(*atp) )
	{
	    *(ptr-1) = *ptr ;
	    *(atp-1) |= BIT_CHANGED ;
#ifdef KANJI
	    sql04_charmodes[pos-1] = sql04_charmodes[pos] ;
#endif
	    pos ++ ;
	    ptr ++ ;
	    atp ++ ;
	}
	if ( pos != lastpos ) sql04_screen_changed = TRUE ;

	/*
	 *  Stop at screen end or at a protected character in the field.
	 */
	if ( (pos >= eps) || ((*ptr >= FIELD_DELIMITERS) && is_protect(*atp)) )
	{
	    return ( pos - 1 );
	}

	/*
	 *  We found an end of field delimiter.
	 *  Store the position of the last unprotected character.
	 *  Look for a non-empty continuation field.
	 */
	lastpos = pos - 1 ;
	pos = sql04_find_next ( pos , eps );
	ptr = sql04_tio_screen + pos ;
	atp = sql04_tio_attrib + pos ;

	/*
	 *  Return, if there are no more fields or
	 *  if at a non-continuation field
	 *  or at a protected character at the first position.
	 */
	if ( (pos < 0) || (*(ptr-1) != VT_SOCF) || is_protect(*atp) )
	{
	    return ( lastpos );
	}

	/*
	 *  Move the first character from the continuation field
	 *  to the last position of the previous field.
	 */
	sql04_tio_screen[lastpos] = *ptr ;
	sql04_tio_attrib[lastpos] |= BIT_CHANGED ;
#ifdef KANJI
	sql04_charmodes[lastpos] = sql04_charmodes[pos] ;
#endif
	sql04_screen_changed = TRUE ;

	/*
	 *  Continue with shifting characters within this continuation field.
	 */
    }
    /* for(;;) */
    /*NOTREACHED*/
}

/*==========================================================================*/

/*
 *  Shift the characters from 'position' on right one position to the right.
 *  Shift through all continuation fields.
 *  Stop at a protected position.
 *  Update the 'tio_screen' buffer only.
 *  Return the position of the last unprotected character of the fields.
 */

static	int	sql04_shift_right ( int start_position )
{
    register int		pos ;
    register int		eps ;
    register UCHAR		*ptr ;
    register UCHAR		*atp ;
    int				lastpos ;
    int				end_pos ;


    DBG1 (( MF__,"shftrigt: called    %d \n", start_position ))

    pos = start_position ;
    eps = sql04_screen_size ;
    ptr = sql04_tio_screen + pos ;
    atp = sql04_tio_attrib + pos ;

    /*
     *  Search for the last unprotected position in the (continued) field.
     */
    for ( ; ; )
    {
	/*
	 *  Find the end of the current field or a protected character.
	 */
	while ( (pos < eps) && (*ptr >= FIELD_DELIMITERS) && is_unprot(*atp) )
	{
	    pos ++ ;
	    ptr ++ ;
	    atp ++ ;
	}

	/*
	 *  The current position is one character position behind
	 *  the last writable position of this field.
	 */
	end_pos = pos ;

	/*
	 *  Stop at screen end
	 *  or at a protected character in the field.
	 */
	if ( (pos >= eps) || ((*ptr >= FIELD_DELIMITERS) && is_protect(*atp)) )
	{
	    break ;
	}

	/*
	 *  Look for a non-empty continuation field.
	 */
	pos = sql04_find_next ( pos , eps );
	ptr = sql04_tio_screen + pos ;
	atp = sql04_tio_attrib + pos ;

	/*
	 *  Stop, if there are no more fields or
	 *  if at a non-continuation field
	 *  or at a protected character at the first position.
	 */
	if ( (pos < 0) || (*(ptr-1) != VT_SOCF) || is_protect(*atp) )
	{
	    break ;
	}
    }
    /* for(;;) */

    /*
     *  Get pointers to the last writable position in the field.
     */
    pos = end_pos - 1 ;
    eps = start_position ;
    ptr = sql04_tio_screen + pos ;
    DBG1 (( MF__,"shftrigt: end pos   %d \n", end_pos ))

    /*
     *  Insert is possible only if there is at least one space
     *  at the end of the field.
     */
    if ( *ptr != ' ' )
    {
	DBG1 (( MF__,"shftrigt: not blank %d \n", end_pos ))
	return ( -1 );
    }

    /*
     *  Go to the first character to be shifted right.
     */
    pos -- ;
    ptr -- ;

    /*
     *  Shift the characters in the field(s) to the right.
     */
    while ( pos >= eps )
    {
	/*
	 *  Shift the characters within the current field.
	 */
	atp = sql04_tio_attrib + pos ;
	while ( (pos >= eps) && (*ptr >= FIELD_DELIMITERS) )
	{
	    *(ptr+1) = *ptr ;
	    *(atp+1) |= BIT_CHANGED ;
#ifdef KANJI
	    sql04_charmodes[pos+1] = sql04_charmodes[pos] ;
#endif
	    pos -- ;
	    ptr -- ;
	    atp -- ;
	}

	/*
	 *  Stop at insert position.
	 */
	if ( pos < eps ) break ;

	/*
	 *  Save the position, where to put the last character from
	 *  the previous continuation field.
	 */
	lastpos = pos + 1 ;
	DBG1 (( MF__,"shftrigt: lastpos   %d \n", lastpos ))

	/*
	 *  Find the end of the previous field.
	 */
	pos = sql04_find_prev ( pos , -1 );
	if ( pos < 0 ) break ; /* MUST NEVER HAPPEN ! */
	pos = sql04_find_eof ( pos );
	ptr = sql04_tio_screen + pos ;

	/*
	 *  Move the last character of this field to the saved position.
	 */
	sql04_tio_screen[lastpos] = *ptr ;
	sql04_tio_attrib[lastpos] |= BIT_CHANGED ;
#ifdef KANJI
	sql04_charmodes[lastpos] = sql04_charmodes[pos] ;
#endif
	sql04_screen_changed = TRUE ;
	DBG1 (( MF__,"shftrigt: movepos   %d \n", pos ))

	/*
	 *  Go to the first character of this field to be shifted right.
	 */
	pos -- ;
	ptr -- ;
    }
    /* while */

    DBG1 (( MF__,"shftrigt: returning %d \n", end_pos ))
    return ( end_pos );
}

/*==========================================================================*/

/*
 *  Set 'sql04_position' to the first unprotected position
 *  of the current or of the next following field.
 *  Wrap around the end of the screen.
 *  If no unprotected position exists, beep and leave 'sql04_position'
 *  unchanged.
 */

static	void	sql04_next_unprot ( )

{
    register int		pos ;
    register int		went_next_field = FALSE ;


    /*
     *  Try the next unprotected position in the current field.
     */
    pos = sql04_find_unprot ( sql04_position );

    /*
     *  If none, search for the next field with an unprotected position.
     */
    if ( pos < 0 )
    {
	pos = sql04_next_field ();
	if ( pos != sql04_position ) went_next_field = TRUE ;
    }

    /*
     *  The search might have exceeded the last field.
     */
    if ( pos >= sql04_screen_size )
    {
	sql04_behind_last = TRUE ;
	return ;
    }

    /*
     *  In invisible fields, the display cursor remains on the first
     *  character of the field when typing characters.
     *  If the new position is visible,
     *  or the old position was visible,
     *  or the position changed to a new field
     *  then the display cursor should follow the buffer cursor.
     */
    if ( ! is_invisible(sql04_tio_attrib[pos]) ||
	 ! is_invisible(sql04_tio_attrib[sql04_displpos]) ||
	 went_next_field )
    {
	sql04_displpos = pos ;
    }

    sql04_position = pos ;
}

/*==========================================================================*/

/*
 *  Return the first unprotected position of the next following field.
 *  Wrap around the end of the screen.
 *  If no unprotected position exists, beep and return 'sql04_position'.
 */

static	int	sql04_next_field ( )

{
    register int		fpos ;
    register int		pos ;


    /*
     *  Go to the first unprotected position of the next field.
     */
    for ( pos = sql04_position ; ; )
    {
	pos = sql04_find_next ( pos , sql04_screen_size );
	if ( (pos < 0) || is_unprot(sql04_tio_attrib[pos]) ) break ;
	fpos = pos ;
	pos = sql04_find_unprot ( pos );
	if ( pos > 0 ) break ;
	pos = fpos ;
    }

    /*
     *  If none found, restart from the beginning of the screen.
     *  If, however, 'return_on_last' is active, return a position
     *  past the end of the screen.
     */
    if ( pos < 0 )
    {
	if ( sql04_return_on_last ) return ( sql04_screen_size );
	for ( pos = 0 ; ; )
	{
	    pos = sql04_find_next ( pos , sql04_position + 1 );
	    if ( (pos < 0) || is_unprot(sql04_tio_attrib[pos]) ) break ;
	    fpos = pos ;
	    pos = sql04_find_unprot ( pos );
	    if ( pos > 0 ) break ;
	    pos = fpos ;
	}
    }

    if ( pos >= 0 ) return ( pos );
    sql04_beep_flash ();
    return ( sql04_position );
}

/*==========================================================================*/

/*
 *  Return the first unprotected position of the next previous field.
 *  Wrap around the beginning of the screen.
 *  If no unprotected position exists, beep and return 'sql04_position'.
 */

static	int	sql04_prev_field ( )

{
    register int		fpos ;
    register int		pos ;


    /*
     *  Go to the first unprotected position of the previous field.
     */
    for ( pos = sql04_position ; ; )
    {
	pos = sql04_find_prev ( pos , -1 );
	if ( (pos < 0) || is_unprot(sql04_tio_attrib[pos]) ) break ;
	fpos = pos ;
	pos = sql04_find_unprot ( pos );
	if ( pos > 0 ) break ;
	pos = fpos ;
    }

    /*
     *  If none found, restart from the end of the screen.
     *  If, however, 'return_on_first' is active, return a position
     *  before the beginning of the screen.
     */
    if ( pos < 0 )
    {
	if ( sql04_return_on_first ) return ( -1 );
	for ( pos = sql04_screen_size-1 ; ; )
	{
	    pos = sql04_find_prev ( pos , sql04_position );
	    if ( (pos < 0) || is_unprot(sql04_tio_attrib[pos]) ) break ;
	    fpos = pos ;
	    pos = sql04_find_unprot ( pos );
	    if ( pos > 0 ) break ;
	    pos = fpos ;
	}
    }

    if ( pos >= 0 ) return ( pos );
    sql04_beep_flash ();
    return ( sql04_position );
}

/*==========================================================================*/

static	void	sql04_position_changed ( position )

int			position ;

{
    register int		lno ;


    /*
     *  'first_linechg' and 'last_linechg' count from 1 to sql04_lines.
     */
    lno = LINE(position) + 1 ;

    if ( lno < sql04_first_linechg ) sql04_first_linechg = lno ;
    if ( lno > sql04_last_linechg  ) sql04_last_linechg  = lno ;
}

/*==========================================================================*/

/*
 *  We are in a field, if not at a field delimiter and the
 *  next previous delimiter is a start of field and the
 *  current position is either unprotected or mixed.
 */

static	int	sql04_in_field ( int start_position )
{
    register int		pos ;
    register UCHAR		*ptr ;
    register UCHAR		*atp ;


    /*
     *  Get a pointer to the current character.
     */
    pos = start_position ;
    ptr = sql04_tio_screen + pos ;

    /*
     *  If the current character is a field delimiter,
     *  we are not in a field.
     */
    if ( *ptr < FIELD_DELIMITERS ) return ( FALSE );

    /*
     *  Step from the current character backwards
     *  until a field delimiter is found.
     */
    do
    {
	pos -- ;
	ptr -- ;
    }
    while ( (pos >= 0) && (*ptr >= FIELD_DELIMITERS) );

    /*
     *  If no field delimiter found or
     *  the current character is a end of field delimiter,
     *  we are not in a field.
     */
    if ( (pos < 0) || (*ptr == VT_EOF) ) return ( FALSE );

    /*
     *  Step into the field and get its attribute.
     */
    pos ++ ;
    ptr ++ ;
    atp = sql04_tio_attrib + pos ;

    if ( is_unprot(*atp) || is_mixed(*atp) )
	return ( TRUE );
    else
	return ( FALSE );
}

/*==========================================================================*/

/*
 *  Return the position of the first character
 *  of the next following unprotected or mixed field.
 *  If no field follows, return -1.
 */

static	int	sql04_find_next ( start_position , stop_position )

int			start_position ;
int			stop_position ;

{
    register int		pos ;
    register int		eps ;
    register UCHAR		*ptr ;
    register UCHAR		*atp ;


    DBG1 (( MF__,"find_next: start     %d \n", start_position ))
    DBG1 (( MF__,"find_next: stop      %d \n", stop_position ))

    pos = start_position ;
    eps = stop_position ;
    ptr = sql04_tio_screen + pos ;

    /*
     *  Loop until an unprotected or mixed field is found.
     */
    for ( ; pos < eps ; )
    {
	/*
	 *  Step from the current character forward
	 *  until a start of field delimiter is found.
	 */
	while ( (pos < eps) &&
		(*ptr != VT_SONCF) &&
		(*ptr != VT_SOCF) )
	{
	    pos ++ ;
	    ptr ++ ;
	}

	/*
	 *  If no field found, give up.
	 */
	if ( pos >= eps ) break ;

	/*
	 *  We found a start of field delimiter.
	 *  Move into that field and get its attribute.
	 */
	pos ++ ;
	ptr ++ ;
	atp = sql04_tio_attrib + pos ;

	/*
	 *  If we found an unprotected or mixed field, return.
	 */
	if ( (pos < eps) &&
	     (*ptr >= FIELD_DELIMITERS) &&
	     (is_unprot(*atp) || is_mixed(*atp)) )
	{
	    DBG1 (( MF__,"find_next: returning %d \n", pos ))
	    return ( pos );
	}
    }

    DBG1 (( MF__,"find_next: returning %d \n", -1 ))
    return ( -1 );
}

/*==========================================================================*/

/*
 *  Return the position of the first character
 *  of the next previous unprotected or mixed field.
 *  If no field leads, return -1.
 */

static	int	sql04_find_prev ( start_position , stop_position )

int			start_position ;
int			stop_position ;

{
    register int		pos ;
    register int		eps ;
    register UCHAR		*ptr ;
    register UCHAR		*atp ;


    DBG1 (( MF__,"find_prev: start     %d \n", start_position ))
    DBG1 (( MF__,"find_prev: stop      %d \n", stop_position ))

    pos = start_position ;
    eps = stop_position ;
    ptr = sql04_tio_screen + pos ;

    /*
     *  Step from the current character backwards
     *  until a field delimiter is found.
     */
    while ( (pos > eps) && (*ptr >= FIELD_DELIMITERS) )
    {
	pos -- ;
	ptr -- ;
    }

    /*
     *  The field delimiter may be an end of field or a start of field.
     *  In the first case we were outside a field and have to find
     *  the beginning of the field.
     *  In the second case we were inside a field and found its own
     *  beginning and have to find the beginning of the previous field.
     */
    if ( pos > eps )
    {
	pos -- ;
	ptr -- ;
    }

    /*
     *  Loop until an unprotected or mixed field is found.
     */
    for ( ; pos > eps ; )
    {
	/*
	 *  Find the next previous start of field.
	 */
	while ( (pos > eps) && (*ptr != VT_SONCF) && (*ptr != VT_SOCF) )
	{
	    pos -- ;
	    ptr -- ;
	}

	/*
	 *  If no field found, give up.
	 */
	if ( pos <= eps ) break ;

	/*
	 *  We found a start of field delimiter.
	 *  Move into that field and get its attribute.
	 */
	pos ++ ;
	ptr ++ ;
	atp = sql04_tio_attrib + pos ;

	/*
	 *  If we found an unprotected or mixed field, return.
	 */
	if ( (*ptr >= FIELD_DELIMITERS) &&
	     (is_unprot(*atp) || is_mixed(*atp)) )
	{
	    DBG1 (( MF__,"find_prev: returning %d \n", pos ))
	    return ( pos );
	}

	/*
	 *  We are at the first character of a protected field.
	 *  Move before its start of field delimiter so that the next loop
	 *  execution does not find the same field again.
	 */
	pos -= 2 ;
	ptr -= 2 ;
    }

    DBG1 (( MF__,"find_prev: returning %d \n", -1 ))
    return ( -1 );
}

/*==========================================================================*/

/*
 *  Return the next following unprotected position within the same field.
 *  If no unprotected position follows in this field, return -1.
 */

static	int	sql04_find_unprot ( start_position )

int			start_position ;

{
    register int		pos ;
    register int		eps ;
    register UCHAR		*ptr ;
    register UCHAR		*atp ;


    pos = start_position ;
    pos ++ ; /* don't find the current position */
    eps = sql04_screen_size ;
    ptr = sql04_tio_screen + pos ;
    atp = sql04_tio_attrib + pos ;

    /*
     *  We start always in a field.
     */
    while ( (pos < eps) && (*ptr >= FIELD_DELIMITERS) && is_protect(*atp) )
    {
	pos ++ ;
	ptr ++ ;
	atp ++ ;
    }
    if ( (pos < eps) && (*ptr >= FIELD_DELIMITERS) && is_unprot(*atp) )
    {
	return ( pos );
    }

    return ( -1 );
}

/*==========================================================================*/

/*
 *  Return the position of the last character of the field.
 */

static	int	sql04_find_eof ( start_position )

int			start_position ;

{
    register int		pos ;
    register int		eps ;
    register UCHAR		*ptr ;


    pos = start_position ;
    eps = sql04_screen_size ;
    ptr = sql04_tio_screen + pos ;

    /*
     *  We start always in a field.
     */
    while ( (pos < eps) && (*ptr >= FIELD_DELIMITERS) )
    {
	pos ++ ;
	ptr ++ ;
    }

    return ( pos - 1 );
}

#ifdef MARK_NOCH_ZU_TUN
/*==========================================================================*/

/*
 *   Display a full (normalized) vt-screen on the terminal.
 *   Used ONLY(!) for the mark-functionality
 */

static	void	sql04_write_marked ( )

{
    register int		pos ;
    register UCHAR		*ptr = sql04_save_screen ;
    register UCHAR		*atp = sql04_save_attrib ;


    DBGSCREEN ( ptr , atp , sql04_screen_size , "write_marked" )

    (void) attron ( sql04_mark_attrib );
    for ( pos = 0 ; pos < sql04_screen_size ; pos ++ , ptr ++ , atp ++ )
    {
	/* don't change graphics and non-input fields */
	if ( (*ptr >= ' ') && sql04_in_field(pos) )
	{
	    sql04_addch ( *ptr , *atp );
	}
    }
    (void) attroff ( sql04_mark_attrib );

}
#endif /* MARK_NOCH_ZU_TUN */

#ifdef MARK_NOCH_ZU_TUN
/*==========================================================================*/

static	boolean	sql04_in_marked_area (pos)
int     pos ;
{

    return FALSE ;      /* bis zur ausgetesteten Version ! */

    if ( pos == sql04_mark_apos )
	return TRUE ;

    switch ( sql04_mark_option )
    {
	case VTM_MARK_BLOCK :
	{

	    int     acol ;
	    int     aline ;
	    int     ecol ;
	    int     eline ;

	    int     poscol ;
	    int     posline ;

	    int     links,rechts,oben,unten ;

	    acol    = sql04_mark_apos % sql04_cols ;
	    aline   = sql04_mark_apos / sql04_cols ;
	    ecol    = sql04_mark_epos % sql04_cols ;
	    eline   = sql04_mark_epos / sql04_cols ;

	    poscol  = pos % sql04_cols ;
	    posline = pos / sql04_cols ;

	    links   = smaller ( acol , ecol );
	    rechts  = greater ( acol , ecol );
	    oben    = smaller ( aline , eline );
	    unten   = greater ( aline , eline );

	    return ( poscol >= links && poscol <= links &&
		 posline <= unten && posline >= oben );

	}

	case VTM_MARK_LINE :
	    return ( ( sql04_mark_apos / sql04_cols ) == ( pos / sql04_cols ) );

	case VTM_MARK_CONTINUE :
	    if ( sql04_mark_apos <= sql04_mark_epos )
		return ( pos >= sql04_mark_apos && pos <= sql04_mark_epos );
	    else
		return ( pos >= sql04_mark_epos && pos <= sql04_mark_apos );

	 default :
	    return  ( FALSE );

    }
}
#endif /* MARK_NOCH_ZU_TUN */

#ifdef MARK_NOCH_ZU_TUN
/*==========================================================================*/

/* Markieren eines Bildschirmbereichs durch Darstellung der ersten
   Markierungs-Position durch Blink und Underline (wie Rand-Editor) */

#endif /* MARK_NOCH_ZU_TUN */

/*==========================================================================*/

static	void	sql04_addch ( uchr , uatt )

register UCHAR		uchr ;
register UCHAR		uatt ;

{
    register chtype		cchr ;
    register chtype		catt ;


    catt = ATTR(uatt);

    if ( uchr < MAXGRAPHIC )
    {
	/*
	 *  Graphic character. Strip all non-color attributes.
	 *  Some terminals get crazy from graphic with attributes.
	 */
	cchr = sql04_graphic[uchr];
	catt &= A_COLOR ;
	DBGG (( "addch: graphic 0x%02x -> 0x%08lx \n", uchr , cchr | catt ))
    }
    else
    {
	if ( is_invisible(uatt) )
	{
	    cchr = CHBLANK ;
	}
	else
	{
	    if ( sql04_isprint((int) uchr) )
	    {
		cchr = (chtype) uchr ;
	    }
	    else
	    {
		cchr = (chtype) '?' ;
	    }
	    /* if is print */
	}
	/* if is invisible */
    }
    /* if is graphic */

#if SCO || SVR4 || SOLARIS || LINUX || FREEBSD
    /*
     *  On some systems 'addch' fails with characters > 0x7f.
     */
    if ( (uchr < ' ') || (uchr > '~') )
    {
	register int		curlin ;
	register int		curcol ;
	chtype			cstr [ 1 ];

	cstr[0] = cchr | catt ;
	(void) addchnstr ( cstr , 1 );
	getyx ( stdscr , curlin , curcol );
	curcol ++ ;
	if ( curcol >= sql04_cols )
	{
	    curcol = 0 ;
	    curlin ++ ;
	    if ( curlin >= sql04_lines ) curlin = 0 ;
	}
	(void) move ( curlin , curcol );
    }
    else
#endif	/*SCO || SVR4 || SOLARIS */
    {
	(void) addch ( cchr | catt );
    }
}

/*==========================================================================*/

static	void	sql04_beep_flash ( )

{
    /*
     *  Sound terminal's bell.
     *  Some terminals can't generate an audible signal, but may be able
     *  to flash the screen (light it up for a short time).
     */
    (void) beep ();
    (void) flash ();
}

/*==========================================================================*/

#ifdef DEBUG_RTE
static	void	debug_init ()

{
    /*
     *  In the DEBUG mode, the virtual terminal can be left by Ctrl-C.
     */
    (void) signal ( SIGINT , debug_catch );

    debugfp = fopen ( DEBUGFILE , "w" );
    if ( ! debugfp )
    {
	DBG1(( MF__, "cannot open '%s': %s \n", DEBUGFILE , sqlerrs() ))
	exit ( 1 );
    }
}
#endif /* DEBUG_RTE */

/*==========================================================================*/

#ifdef DEBUG_RTE
/*
 *  In the DEBUG mode, the virtual terminal can be left by Ctrl-C.
 */

/*ARGSUSED*/
static	SIGNALFUNCTYPE	debug_catch ( sig )

int			sig ;

{
    char           *msg = "" ;


    sqlctoff ( &msg , &msg , msg );
    (void) fclose ( debugfp ); /* 91-Aug-20 J.Bruehe: force a buffer flush */
    exit ( 0 );
}
#endif /* DEBUG_RTE */

/*==========================================================================*/

#ifdef	DEBUG_RTE
/*PRINTFLIKE1*/
global	void	sqldbgvt ( char *text , ... )
{
    va_list			args ;



    if ( ! debugfp ) debug_init ();

    va_start ( args, text );
    (void) vfprintf ( debugfp , text , args );
    (void) fflush ( debugfp );
    va_end ( args );
}
#endif /* DEBUG_RTE */

/*==========================================================================*/

#ifdef	DEBUG_RTE
static	void	debug_screen ( screen , attrib , length , header )

UCHAR			*screen ;
UCHAR			*attrib ;
int			length ;
char			*header ;

{
    int             i ;



    if ( ! debugfp ) debug_init ();


    (void) fprintf ( debugfp , "\n\n%s\n", header );
    for ( i = 0 ; header[i] ; i ++ ) (void) putc ( '=' , debugfp );
    (void) fprintf ( debugfp , "\n\n" );

    for ( i = 0 ; i < length / LINEVALUES ; i++ )
    {
	debug_hexline  ( attrib , i * LINEVALUES , LINEVALUES , 'a' );
	debug_hexline  ( screen , i * LINEVALUES , LINEVALUES , 's' );
	debug_charline ( screen , i * LINEVALUES , LINEVALUES , 's' );
	(void) putc ( '\n', debugfp );
    }
    debug_hexline  ( attrib , i * LINEVALUES , length % LINEVALUES , 'a' );
    debug_hexline  ( screen , i * LINEVALUES , length % LINEVALUES , 's' );
    debug_charline ( screen , i * LINEVALUES , length % LINEVALUES , 's' );
    (void) putc ( '\n', debugfp );
    (void) fflush (debugfp);
}
#endif /* DEBUG_RTE */

/*==========================================================================*/

#ifdef	DEBUG_RTE
static	void	debug_hexline ( buf , startpos , length , label )

UCHAR			*buf ;
int			startpos ;
int			length ;
char			label ;

{
    register int		i ;


    (void) fprintf ( debugfp , " %c : %4d   ", label , startpos );
    for ( i = 0 ; i < length ; i ++ )
    {
	if ( i % 4 == 0 ) (void) putc ( ' ' , debugfp );
	(void) fprintf ( debugfp , "%02x ", (unsigned) buf[i + startpos] );
    }
    (void) putc ( '\n', debugfp );
}
#endif /* DEBUG_RTE */

/*==========================================================================*/

#ifdef	DEBUG_RTE
static	void	debug_charline ( buf , startpos , length , label )

UCHAR			*buf ;
int			startpos ;
int			length ;
char			label ;

{
    register int		i ;
    register UCHAR		uch ;


    (void) fprintf ( debugfp , " %c : %4d   ", label , startpos );
    for ( i = 0 ; i < length ; i++ )
    {
	if ( i % 4 == 0 ) (void) putc ( ' ' , debugfp );
	(void) putc ( ' ' , debugfp );
	uch = buf[i+startpos];
	if ( sql04_isprint((int) uch) )
	    (void) putc ( uch , debugfp );
	else
	    (void) putc ( ' ' , debugfp );
	(void) putc ( ' ' , debugfp );
    }
    (void) putc ( '\n', debugfp );
}
#endif /* DEBUG_RTE */

/*==========================================================================*/

#ifdef	DEBUG_RTE
static	void	debug_key_array ( name , keyarr )

char			*name ;
char			*keyarr ;

{
    int				idx ;


    for ( idx = 0 ; idx < VTKEY_LAST ; idx ++ )
	DBG1 (( MF__,"%s %-18s=%d \n", name , sql04_key_name(idx) , keyarr[idx] ))
}
#endif /* DEBUG_RTE */

/*==========================================================================*/

#ifdef	lint
static	void	sql04_lint_dummy ()
{
    if( sql04_mark_apos && sql04_mark_epos && sql04_mark_attrib )
	sql04_lint_dummy ();
}
#endif	/*lint*/
