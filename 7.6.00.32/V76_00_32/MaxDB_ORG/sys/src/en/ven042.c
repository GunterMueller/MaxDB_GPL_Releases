/*!
  @file           ven042.c
  @author         JoergM
  @brief          virtual_terminal_subroutines Common Functions for the Virtual Terminal Modules
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
*/

#define MOD__ "ven042.c:"

#include "geo002.h"
#include "geo001.h"
#include "geo57.h"
#include "geo007_1.h"
#include "heo01.h"
#include "gen04.h"
#include "hen40.h"

/* exported functions */

	char			*sql04_key_name ();
	int			sql04_get_environment ();
	int			sql04_make_key_list ();
	int			sql04_ins_keyinfo ();
	char			*sql04_put_sequence ();
	int			sql04_cat ();
	int			sql04_read_chrclass ();
	int			sql04_isprint ();
	int			sql04_init_graphic_table ();

/* imported functions */

extern	char			*getenv ();
extern	int			putenv ();
extern	long			strtol ();

/* local functions */

static	int			sql04_get_curses_keys ();
static	int			sql04_read_hifkeys ();
static	int			sql04_ins_hifinfo ();
static	void			sql04_get_sequence ();

/* exported data */

	char			* sql04_env_DBTERM ;
	char			* sql04_env_DBHIF ;
	char			* sql04_env_DBCHRCLASS ;
	char			* sql04_env_DBTERMRESET ;
	char			* sql04_env_DBTERMREDRAW ;
				/* TERM and TERMINFO from environment */
	char			* sql04_env_TERM_extern ;
	char			* sql04_env_TERMINFO_extern ;
				/* TERM and TERMINFO as used internally */
	char			* sql04_env_TERM ;
	char			* sql04_env_TERMINFO ;
				/* buffers for putenv */
	char			* sql04_envdef_TERM_extern ;
	char			* sql04_envdef_TERMINFO_extern ;
	char			* sql04_envdef_TERM ;
	char			* sql04_envdef_TERMINFO ;

	UCHAR			sql04_chrclass [ 256 ];
	chtype			sql04_graphic [ MAXGRAPHIC ];

/*
 *  Keys used by the virtual terminal.
 *  NOTE: The capnames for the key functions MUST begin with 'k'!
 *        Otherwise they would be interpreted as a special function.
 */
	int			sql04_key_value ;
	struct key_info		* sql04_key_list ;
	struct capname_info	sql04_cap_table [] =
	{
		/*
		 *  The following list is sorted by VTKEY-Values.
		 *  This is done for the human reader.
		 *  A sort by capnames would allow a better search
		 *  algorithm in ins_hifinfo. Since this is called
		 *  only during initialization, the slightly worse
		 *  performance can be accepted for the advantage of
		 *  easier maintenance of this list.
		 */
#ifdef	SCRLIB
		{ VTKEY_CLEAR        ,  "kclr"  },
		{ VTKEY_DO           ,  "kdo"   },
		{ VTKEY_SK01         ,  "ksk1"  },
		{ VTKEY_SK02         ,  "ksk2"  },
		{ VTKEY_SK03         ,  "ksk3"  },
		{ VTKEY_SK04         ,  "ksk4"  },
		{ VTKEY_SK05         ,  "ksk5"  },
		{ VTKEY_SK06         ,  "ksk6"  },
		{ VTKEY_SK07         ,  "ksk7"  },
		{ VTKEY_SK08         ,  "ksk8"  },
		{ VTKEY_SK09         ,  "ksk9"  },
		{ VTKEY_HELP         ,  "khlp"  },
		{ VTKEY_UP           ,  "kpup"  },
		{ VTKEY_DOWN         ,  "kpd"   },
		{ VTKEY_LEFT         ,  "kpl"   },
		{ VTKEY_RIGHT        ,  "kpr"   },
		{ VTKEY_CMD          ,  "kcmd"  },
		{ VTKEY_PICK         ,  "kpick" },
		{ VTKEY_PUT          ,  "kput"  },
		{ VTKEY_MARK         ,  "kmrk"  },
		{ VTKEY_SELECT       ,  "ksel"  },
		{ VTKEY_MOVE         ,  "kmov"  },
		{ VTKEY_COPY         ,  "kcpy"  },
		{ VTKEY_INSERT       ,  "kil1"  },
		{ VTKEY_DELETE       ,  "kdl1"  },
		{ VTKEY_TOP          ,  "kpuv"  },
		{ VTKEY_BOTTOM       ,  "kpdv"  },
		{ VTKEY_CANCEL       ,  "kcan"  },
		{ VTKEY_UNDO         ,  "kund"  },
		{ VTKEY_END          ,  "kend"  },
		{ VTKEY_PRINT        ,  "kprt"  },
		{ VTKEY_CURSOR_UP    ,  "kcuu1" },
		{ VTKEY_CURSOR_DOWN  ,  "kcud1" },
		{ VTKEY_CURSOR_RIGHT ,  "kcuf1" },
		{ VTKEY_CURSOR_LEFT  ,  "kcub1" },
		{ VTKEY_CURSOR_HOME  ,  "khome" },
		{ VTKEY_CURSOR_END   ,  "kll"   },
		{ VTKEY_NEXT_FIELD   ,  "knxt"  },
		{ VTKEY_PREV_FIELD   ,  "krvrs" },
		{ VTKEY_INSERT_CHAR  ,  "kich1" },
		{ VTKEY_DELETE_CHAR  ,  "kdch1" },
		{ VTKEY_RUBOUT       ,  "kbs"   },
		{ VTKEY_DEL_EOF      ,  "kctst" },
		{ VTKEY_CURSOR_BOL   ,  "kbeg"  },
		{ VTKEY_CURSOR_EOL   ,  "kBEG"  },
		{ VTKEY_BACK_TAB     ,  "kcbt"  },
		{ VTKEY_REFRESH      ,  "krfr"  },
#else	/*SCRLIB*/
		{ VTKEY_PF01         ,  "kf1"   },
		{ VTKEY_PF02         ,  "kf2"   },
		{ VTKEY_PF03         ,  "kf3"   },
		{ VTKEY_PF04         ,  "kf4"   },
		{ VTKEY_PF05         ,  "kf5"   },
		{ VTKEY_PF06         ,  "kf6"   },
		{ VTKEY_PF07         ,  "kf7"   },
		{ VTKEY_PF08         ,  "kf8"   },
		{ VTKEY_PF09         ,  "kf9"   },
		{ VTKEY_PF10         ,  "kf10"  },
		{ VTKEY_PF11         ,  "kf11"  },
		{ VTKEY_PF12         ,  "kf12"  },
		{ VTKEY_CLEAR        ,  "kclr"  },
		{ VTKEY_DO           ,  "kent"  },
		{ VTKEY_SK01         ,  "ksk1"  },
		{ VTKEY_SK02         ,  "ksk2"  },
		{ VTKEY_SK03         ,  "ksk3"  },
		{ VTKEY_SK04         ,  "ksk4"  },
		{ VTKEY_SK05         ,  "ksk5"  },
		{ VTKEY_SK06         ,  "ksk6"  },
		{ VTKEY_SK07         ,  "ksk7"  },
		{ VTKEY_SK08         ,  "ksk8"  },
		{ VTKEY_SK09         ,  "ksk9"  },
		{ VTKEY_HELP         ,  "khlp"  },
		{ VTKEY_UP           ,  "kpp"   },
		{ VTKEY_DOWN         ,  "knp"   },
		{ VTKEY_LEFT         ,  "kLFT"  },
		{ VTKEY_RIGHT        ,  "kRIT"  },
		{ VTKEY_CMD          ,  "kcmd"  },
		{ VTKEY_PICK         ,  "kCPY"  },
		{ VTKEY_PUT          ,  "kMOV"  },
		{ VTKEY_MARK         ,  "kmrk"  },
		{ VTKEY_SELECT       ,  "kslt"  },
		{ VTKEY_MOVE         ,  "kmov"  },
		{ VTKEY_COPY         ,  "kcpy"  },
		{ VTKEY_INSERT       ,  "kil1"  },
		{ VTKEY_DELETE       ,  "kdl1"  },
		{ VTKEY_TOP          ,  "kPRV"  },
		{ VTKEY_BOTTOM       ,  "kNXT"  },
		{ VTKEY_CANCEL       ,  "kcan"  },
		{ VTKEY_UNDO         ,  "kund"  },
		{ VTKEY_END          ,  "kend"  },
		{ VTKEY_EXIT         ,  "kext"  },
		{ VTKEY_PRINT        ,  "kprt"  },
		{ VTKEY_FIND         ,  "kfnd"  },
		{ VTKEY_REPLACE      ,  "krpl"  },
		{ VTKEY_CURSOR_UP    ,  "kcuu1" },
		{ VTKEY_CURSOR_DOWN  ,  "kcud1" },
		{ VTKEY_CURSOR_RIGHT ,  "kcuf1" },
		{ VTKEY_CURSOR_LEFT  ,  "kcub1" },
		{ VTKEY_CURSOR_HOME  ,  "khome" },
		{ VTKEY_CURSOR_END   ,  "kll"   },
		{ VTKEY_NEXT_FIELD   ,  "knxt"  },
		{ VTKEY_PREV_FIELD   ,  "kprv"  },
		{ VTKEY_INSERT_CHAR  ,  "kich1" },
		{ VTKEY_DELETE_CHAR  ,  "kdch1" },
		{ VTKEY_RUBOUT       ,  "kbs"   },
		{ VTKEY_DEL_EOF      ,  "kel"   },
		{ VTKEY_CURSOR_BOL   ,  "kbeg"  },
		{ VTKEY_CURSOR_EOL   ,  "kBEG"  },
		{ VTKEY_BACK_TAB     ,  "kcbt"  },
		{ VTKEY_REFRESH      ,  "krfr"  },
#endif	/*SCRLIB*/
		{ 0                  ,  ""      }
	};
	int			sql04_key_count =
		    sizeof(sql04_cap_table) / sizeof(struct capname_info) - 1 ;

/*
 *  Additional functions, which are not covered by all curses implementations.
 *  Each function has a global pointer to char, initially NULL.
 *  In sql04_hif_table the addresses of these pointers are set
 *  together with the capability names.
 *  NOTE: The capnames for the additional functions MUST NOT begin with 'k'!
 *        Otherwise they would be interpreted as a key.
 */
	char			*sql04_hif_softkeys ;
	struct hifcap_info	sql04_hif_table [] =
	{
		{ & sql04_hif_softkeys , "softk" },
		{ 0                    , "" }
	};

/*==========================================================================*/

static	int	sql04_get_curses_keys ( )

{
    int				rc = 0 ;


    DBG1 (( MF__,"get_curses_keys: called \n" ))

    /*
     *  The following list is sorted by VTKEY-Values.
     *  This is done for the human reader.
     *  The key sequence pointers (key_cle,key_clear,...)
     *  cannot be used as initializers for an array,
     *  since their values are assigned during initscr.
     *  Thus, we have to use lots of function calls
     *  instead of a loop.
     */

#ifdef	SCRLIB
    rc |= sql04_ins_keyinfo ( VTKEY_CLEAR        ,  key_cle       ); /*kcle*/
    rc |= sql04_ins_keyinfo ( VTKEY_CLEAR        ,  key_clear     ); /*kclr*/
    rc |= sql04_ins_keyinfo ( VTKEY_DO           ,  key_do        ); /*kdo*/
    rc |= sql04_ins_keyinfo ( VTKEY_DO           ,  key_enter     ); /*kent*/
    rc |= sql04_ins_keyinfo ( VTKEY_SK01         ,  key_sk1       ); /*ksk1*/
    rc |= sql04_ins_keyinfo ( VTKEY_SK02         ,  key_sk2       ); /*ksk2*/
    rc |= sql04_ins_keyinfo ( VTKEY_SK03         ,  key_sk3       ); /*ksk3*/
    rc |= sql04_ins_keyinfo ( VTKEY_SK04         ,  key_sk4       ); /*ksk4*/
    rc |= sql04_ins_keyinfo ( VTKEY_SK05         ,  key_sk5       ); /*ksk5*/
    rc |= sql04_ins_keyinfo ( VTKEY_SK06         ,  key_sk6       ); /*ksk6*/
    rc |= sql04_ins_keyinfo ( VTKEY_SK07         ,  key_sk7       ); /*ksk7*/
    rc |= sql04_ins_keyinfo ( VTKEY_SK08         ,  key_sk8       ); /*ksk8*/
    rc |= sql04_ins_keyinfo ( VTKEY_SK09         ,  key_sk9       ); /*ksk9*/
    rc |= sql04_ins_keyinfo ( VTKEY_HELP         ,  key_help      ); /*khlp*/
    rc |= sql04_ins_keyinfo ( VTKEY_UP           ,  key_ppage     ); /*kpp*/
    rc |= sql04_ins_keyinfo ( VTKEY_UP           ,  key_pup       ); /*kpup*/
    rc |= sql04_ins_keyinfo ( VTKEY_DOWN         ,  key_npage     ); /*knp*/
    rc |= sql04_ins_keyinfo ( VTKEY_DOWN         ,  key_pd        ); /*kpd*/
    rc |= sql04_ins_keyinfo ( VTKEY_LEFT         ,  key_pl        ); /*kpl*/
    rc |= sql04_ins_keyinfo ( VTKEY_RIGHT        ,  key_pr        ); /*kpr*/
    rc |= sql04_ins_keyinfo ( VTKEY_CMD          ,  key_cmd       ); /*kcmd*/
    rc |= sql04_ins_keyinfo ( VTKEY_PICK         ,  key_pick      ); /*kpick*/
    rc |= sql04_ins_keyinfo ( VTKEY_PUT          ,  key_put       ); /*kput*/
    rc |= sql04_ins_keyinfo ( VTKEY_MARK         ,  key_mark      ); /*kmrk*/
    rc |= sql04_ins_keyinfo ( VTKEY_SELECT       ,  key_sel       ); /*ksel*/
    rc |= sql04_ins_keyinfo ( VTKEY_MOVE         ,  key_move      ); /*kmov*/
    rc |= sql04_ins_keyinfo ( VTKEY_COPY         ,  key_copy      ); /*kcpy*/
    rc |= sql04_ins_keyinfo ( VTKEY_INSERT       ,  key_il        ); /*kil1*/
    rc |= sql04_ins_keyinfo ( VTKEY_DELETE       ,  key_dl        ); /*kdl1*/
    rc |= sql04_ins_keyinfo ( VTKEY_TOP          ,  key_puv       ); /*kpuv*/
    rc |= sql04_ins_keyinfo ( VTKEY_BOTTOM       ,  key_pdv       ); /*kpdv*/
    rc |= sql04_ins_keyinfo ( VTKEY_CANCEL       ,  key_cancel    ); /*kcan*/
    rc |= sql04_ins_keyinfo ( VTKEY_UNDO         ,  key_undo      ); /*kund*/
    rc |= sql04_ins_keyinfo ( VTKEY_END          ,  key_end       ); /*kend*/
    rc |= sql04_ins_keyinfo ( VTKEY_PRINT        ,  key_print     ); /*kprt*/
    rc |= sql04_ins_keyinfo ( VTKEY_CURSOR_UP    ,  key_up        ); /*kcuu1*/
    rc |= sql04_ins_keyinfo ( VTKEY_CURSOR_DOWN  ,  key_down      ); /*kcud1*/
    rc |= sql04_ins_keyinfo ( VTKEY_CURSOR_RIGHT ,  key_right     ); /*kcuf1*/
    rc |= sql04_ins_keyinfo ( VTKEY_CURSOR_LEFT  ,  key_left      ); /*kcub1*/
    rc |= sql04_ins_keyinfo ( VTKEY_CURSOR_HOME  ,  key_home      ); /*khome*/
    rc |= sql04_ins_keyinfo ( VTKEY_CURSOR_END   ,  key_ll        ); /*kll*/
    rc |= sql04_ins_keyinfo ( VTKEY_NEXT_FIELD   ,  key_next      ); /*knxt*/
    rc |= sql04_ins_keyinfo ( VTKEY_NEXT_FIELD   ,  key_wr        ); /*kwr*/
    rc |= sql04_ins_keyinfo ( VTKEY_PREV_FIELD   ,  key_reverse   ); /*krvrs*/
    rc |= sql04_ins_keyinfo ( VTKEY_PREV_FIELD   ,  key_wl        ); /*kwl*/
    rc |= sql04_ins_keyinfo ( VTKEY_INSERT_CHAR  ,  key_ic        ); /*kich1*/
    rc |= sql04_ins_keyinfo ( VTKEY_DELETE_CHAR  ,  key_dc        ); /*kdch1*/
    rc |= sql04_ins_keyinfo ( VTKEY_RUBOUT       ,  key_backspace ); /*kbs*/
    rc |= sql04_ins_keyinfo ( VTKEY_DEL_EOF      ,  key_ctaste    ); /*kctst*/
    rc |= sql04_ins_keyinfo ( VTKEY_DEL_EOF      ,  key_eol       ); /*kel*/
    rc |= sql04_ins_keyinfo ( VTKEY_BACK_TAB     ,  key_backtab   ); /*kcbt*/
    rc |= sql04_ins_keyinfo ( VTKEY_REFRESH      ,  key_refr      ); /*krfr*/
#else	/*SCRLIB*/
    rc |= sql04_ins_keyinfo ( VTKEY_PF01         ,  key_f1        ); /*kf1*/
    rc |= sql04_ins_keyinfo ( VTKEY_PF02         ,  key_f2        ); /*kf2*/
    rc |= sql04_ins_keyinfo ( VTKEY_PF03         ,  key_f3        ); /*kf3*/
    rc |= sql04_ins_keyinfo ( VTKEY_PF04         ,  key_f4        ); /*kf4*/
    rc |= sql04_ins_keyinfo ( VTKEY_PF05         ,  key_f5        ); /*kf5*/
    rc |= sql04_ins_keyinfo ( VTKEY_PF06         ,  key_f6        ); /*kf6*/
    rc |= sql04_ins_keyinfo ( VTKEY_PF07         ,  key_f7        ); /*kf7*/
    rc |= sql04_ins_keyinfo ( VTKEY_PF08         ,  key_f8        ); /*kf8*/
    rc |= sql04_ins_keyinfo ( VTKEY_PF09         ,  key_f9        ); /*kf9*/
    rc |= sql04_ins_keyinfo ( VTKEY_PF10         ,  key_f10       ); /*kf10*/
    rc |= sql04_ins_keyinfo ( VTKEY_PF11         ,  key_f11       ); /*kf11*/
    rc |= sql04_ins_keyinfo ( VTKEY_PF12         ,  key_f12       ); /*kf12*/
    rc |= sql04_ins_keyinfo ( VTKEY_CLEAR        ,  key_clear     ); /*kclr*/
    rc |= sql04_ins_keyinfo ( VTKEY_DO           ,  key_enter     ); /*kent*/
    rc |= sql04_ins_keyinfo ( VTKEY_HELP         ,  key_help      ); /*khlp*/
    rc |= sql04_ins_keyinfo ( VTKEY_UP           ,  key_ppage     ); /*kpp*/
    rc |= sql04_ins_keyinfo ( VTKEY_DOWN         ,  key_npage     ); /*knp*/
    rc |= sql04_ins_keyinfo ( VTKEY_LEFT         ,  key_sleft     ); /*kLFT*/
    rc |= sql04_ins_keyinfo ( VTKEY_RIGHT        ,  key_sright    ); /*kRIT*/
    rc |= sql04_ins_keyinfo ( VTKEY_CMD          ,  key_command   ); /*kcmd*/
    rc |= sql04_ins_keyinfo ( VTKEY_PICK         ,  key_scopy     ); /*kCPY*/
    rc |= sql04_ins_keyinfo ( VTKEY_PUT          ,  key_smove     ); /*kMOV*/
    rc |= sql04_ins_keyinfo ( VTKEY_MARK         ,  key_mark      ); /*kmrk*/
    rc |= sql04_ins_keyinfo ( VTKEY_SELECT       ,  key_select    ); /*kslt*/
    rc |= sql04_ins_keyinfo ( VTKEY_MOVE         ,  key_move      ); /*kmov*/
    rc |= sql04_ins_keyinfo ( VTKEY_COPY         ,  key_copy      ); /*kcpy*/
    rc |= sql04_ins_keyinfo ( VTKEY_INSERT       ,  key_il        ); /*kil1*/
    rc |= sql04_ins_keyinfo ( VTKEY_DELETE       ,  key_dl        ); /*kdl1*/
    rc |= sql04_ins_keyinfo ( VTKEY_TOP          ,  key_sprevious ); /*kPRV*/
    rc |= sql04_ins_keyinfo ( VTKEY_BOTTOM       ,  key_snext     ); /*kNXT*/
    rc |= sql04_ins_keyinfo ( VTKEY_CANCEL       ,  key_cancel    ); /*kcan*/
    rc |= sql04_ins_keyinfo ( VTKEY_UNDO         ,  key_undo      ); /*kund*/
    rc |= sql04_ins_keyinfo ( VTKEY_END          ,  key_end       ); /*kend*/
    rc |= sql04_ins_keyinfo ( VTKEY_EXIT         ,  key_exit      ); /*kext*/
    rc |= sql04_ins_keyinfo ( VTKEY_PRINT        ,  key_print     ); /*kprt*/
    rc |= sql04_ins_keyinfo ( VTKEY_FIND         ,  key_find      ); /*kfnd*/
    rc |= sql04_ins_keyinfo ( VTKEY_REPLACE      ,  key_replace   ); /*krpl*/
    rc |= sql04_ins_keyinfo ( VTKEY_CURSOR_UP    ,  key_up        ); /*kcuu1*/
    rc |= sql04_ins_keyinfo ( VTKEY_CURSOR_DOWN  ,  key_down      ); /*kcud1*/
    rc |= sql04_ins_keyinfo ( VTKEY_CURSOR_RIGHT ,  key_right     ); /*kcuf1*/
    rc |= sql04_ins_keyinfo ( VTKEY_CURSOR_LEFT  ,  key_left      ); /*kcub1*/
    rc |= sql04_ins_keyinfo ( VTKEY_CURSOR_HOME  ,  key_home      ); /*khome*/
    rc |= sql04_ins_keyinfo ( VTKEY_CURSOR_END   ,  key_ll        ); /*kll*/
    rc |= sql04_ins_keyinfo ( VTKEY_NEXT_FIELD   ,  key_next      ); /*knxt*/
    rc |= sql04_ins_keyinfo ( VTKEY_PREV_FIELD   ,  key_previous  ); /*kprv*/
    rc |= sql04_ins_keyinfo ( VTKEY_INSERT_CHAR  ,  key_ic        ); /*kich1*/
    rc |= sql04_ins_keyinfo ( VTKEY_DELETE_CHAR  ,  key_dc        ); /*kdch1*/
    rc |= sql04_ins_keyinfo ( VTKEY_RUBOUT       ,  key_backspace ); /*kbs*/
    rc |= sql04_ins_keyinfo ( VTKEY_DEL_EOF      ,  key_eol       ); /*kel*/
    rc |= sql04_ins_keyinfo ( VTKEY_BACK_TAB     ,  key_btab      ); /*kcbt*/
    rc |= sql04_ins_keyinfo ( VTKEY_REFRESH      ,  key_refresh   ); /*krfr*/
#endif	/*SCRLIB*/

    DBG1 (( MF__,"get_curses_keys: returning %d \n", rc ))
    return ( rc );
}

/*==========================================================================*/

	char	*sql04_key_name ( key )

int			key ;

{
    char			* ptr ;
static	char			ctrl_key_name [ 16 ];
static	char			char_key_name [ 16 ];


    switch ( key )
    {
	case VTKEY_UNKNOWN      : ptr = "Unknown"      ; break ;
	case VTKEY_CHARACTER    : ptr = char_key_name  ; break ;
	case VTKEY_ENTER        : ptr = "Enter"        ; break ;
	case VTKEY_PF01         : ptr = "F1"           ; break ;
	case VTKEY_PF02         : ptr = "F2"           ; break ;
	case VTKEY_PF03         : ptr = "F3"           ; break ;
	case VTKEY_PF04         : ptr = "F4"           ; break ;
	case VTKEY_PF05         : ptr = "F5"           ; break ;
	case VTKEY_PF06         : ptr = "F6"           ; break ;
	case VTKEY_PF07         : ptr = "F7"           ; break ;
	case VTKEY_PF08         : ptr = "F8"           ; break ;
	case VTKEY_PF09         : ptr = "F9"           ; break ;
	case VTKEY_PF10         : ptr = "F10"          ; break ;
	case VTKEY_PF11         : ptr = "F11"          ; break ;
	case VTKEY_PF12         : ptr = "F12"          ; break ;
	case VTKEY_CLEAR        : ptr = "Clear"        ; break ;
	case VTKEY_DO           : ptr = "Do"           ; break ;
	case VTKEY_SK01         : ptr = "Sk1"          ; break ;
	case VTKEY_SK02         : ptr = "Sk2"          ; break ;
	case VTKEY_SK03         : ptr = "Sk3"          ; break ;
	case VTKEY_SK04         : ptr = "Sk4"          ; break ;
	case VTKEY_SK05         : ptr = "Sk5"          ; break ;
	case VTKEY_SK06         : ptr = "Sk6"          ; break ;
	case VTKEY_SK07         : ptr = "Sk7"          ; break ;
	case VTKEY_SK08         : ptr = "Sk8"          ; break ;
	case VTKEY_SK09         : ptr = "Sk9"          ; break ;
	case VTKEY_HELP         : ptr = "Help"         ; break ;
	case VTKEY_UP           : ptr = "Page_Up"      ; break ;
	case VTKEY_DOWN         : ptr = "Page_Down"    ; break ;
	case VTKEY_LEFT         : ptr = "Page_Left"    ; break ;
	case VTKEY_RIGHT        : ptr = "Page_Right"   ; break ;
	case VTKEY_CMD          : ptr = "Command"      ; break ;
	case VTKEY_PICK         : ptr = "Pick"         ; break ;
	case VTKEY_PUT          : ptr = "Put"          ; break ;
	case VTKEY_MARK         : ptr = "Mark"         ; break ;
	case VTKEY_SELECT       : ptr = "Select"       ; break ;
	case VTKEY_MOVE         : ptr = "Move"         ; break ;
	case VTKEY_COPY         : ptr = "Copy"         ; break ;
	case VTKEY_INSERT       : ptr = "Insert_Line"  ; break ;
	case VTKEY_DELETE       : ptr = "Delete_Line"  ; break ;
	case VTKEY_TOP          : ptr = "Top"          ; break ;
	case VTKEY_BOTTOM       : ptr = "Bottom"       ; break ;
	case VTKEY_CANCEL       : ptr = "Cancel"       ; break ;
	case VTKEY_UNDO         : ptr = "Undo"         ; break ;
	case VTKEY_END          : ptr = "End"          ; break ;
	case VTKEY_ESCAPE       : ptr = "Escape"       ; break ;
	case VTKEY_FILL         : ptr = "Fill"         ; break ;
	case VTKEY_FILE_OPEN    : ptr = "File_Open"    ; break ;
	case VTKEY_FILE_SAVE    : ptr = "File_Save"    ; break ;
	case VTKEY_EXIT         : ptr = "Exit"         ; break ;
	case VTKEY_PRINT        : ptr = "Print"        ; break ;
	case VTKEY_FIND         : ptr = "Find"         ; break ;
	case VTKEY_REPLACE      : ptr = "Replace"      ; break ;
	case VTKEY_CURSOR_UP    : ptr = "Cursor_Up"    ; break ;
	case VTKEY_CURSOR_DOWN  : ptr = "Cursor_Down"  ; break ;
	case VTKEY_CURSOR_RIGHT : ptr = "Cursor_Right" ; break ;
	case VTKEY_CURSOR_LEFT  : ptr = "Cursor_Left"  ; break ;
	case VTKEY_CURSOR_HOME  : ptr = "Cursor_Home"  ; break ;
	case VTKEY_CURSOR_END   : ptr = "Cursor_End"   ; break ;
	case VTKEY_NEXT_FIELD   : ptr = "Next_Field"   ; break ;
	case VTKEY_PREV_FIELD   : ptr = "Prev_Field"   ; break ;
	case VTKEY_INSERT_CHAR  : ptr = "Insert_Char"  ; break ;
	case VTKEY_DELETE_CHAR  : ptr = "Delete_Char"  ; break ;
	case VTKEY_RUBOUT       : ptr = "Rubout"       ; break ;
	case VTKEY_DEL_EOF      : ptr = "Del_To_Eof"   ; break ;
	case VTKEY_MOUSE_DOWN   : ptr = "Mouse_Down"   ; break ;
	case VTKEY_MOUSE_UP     : ptr = "Mouse_Up"     ; break ;
	case VTKEY_MOUSE_MOVE   : ptr = "Mouse_Move"   ; break ;
	case VTKEY_MOUSE_DBL    : ptr = "Mouse_Double" ; break ;
	case VTKEY_HSCROLL      : ptr = "Hscroll"      ; break ;
	case VTKEY_VSCROLL      : ptr = "Vscroll"      ; break ;
	case VTKEY_CONTROL      : ptr = ctrl_key_name  ; break ;
	case VTKEY_CURSOR_BOL   : ptr = "Cursor_Bol"   ; break ;
	case VTKEY_CURSOR_EOL   : ptr = "Cursor_Eol"   ; break ;
	case VTKEY_TAB          : ptr = "Tab"          ; break ;
	case VTKEY_BACK_TAB     : ptr = "Back_Tab"     ; break ;
	case VTKEY_REFRESH      : ptr = "Refresh"      ; break ;
	default                 : ptr = "Illegal!!!!!" ;
    }

    if ( key == VTKEY_CHARACTER )
	spri ( char_key_name , "Character(%c)" , sql04_key_value );
    else
	if ( key == VTKEY_CONTROL )
	    spri ( ctrl_key_name , "Control(%c)" , sql04_key_value );

    return ( ptr );
}

/*========================================================================*/

	int	sql04_get_environment ( )

{
   #undef  MF__
   #define MF__ MOD__"sql04_get_environment"
    int				exists ;
    char			* ptr ;
    tsp00_Pathc     TerminfoPath;
    tsp01_RteError  RteError ;


    DBG1 (( MF__,"get_environment: called \n" ))

    /*
     *  If already called before, do nothing.
     */
    if ( sql04_env_TERM || sql04_env_DBTERM ) return ( 0 );

    /*
     *  Get all required environment strings.
     */
    sql04_env_DBTERM          = getenv ( "DBTERM" );
    sql04_env_DBHIF           = getenv ( "DBHIF" );
    sql04_env_DBCHRCLASS      = getenv ( "DBCHRCLASS" );
    sql04_env_DBTERMRESET     = getenv ( "DBTERMRESET" );
    sql04_env_DBTERMREDRAW    = getenv ( "DBTERMREDRAW" );
    sql04_env_TERM_extern     = getenv ( "TERM" );
#ifndef	SCRLIB
    sql04_env_TERMINFO_extern = getenv ( "TERMINFO" );
#else
    sql04_env_TERMINFO_extern = getenv ( "SCREENINFO" );
#endif

    /*
     *  Prepare environment strings for 'putenv'.
     */
    if ( sql04_env_TERM_extern )
    {
		/* TERM= + $TERM + '\0' */
		/*   5   +  ???  +   1  */
	    if ( ALLOC_MEM_EO57( (void **)&ptr, 6 + (unsigned) strlen(sql04_env_TERM_extern) )
              != NO_ERROR_EO001 )
        {
            return ( -3 );
        }
        else
        {
            spri ( ptr , "TERM=%s" , sql04_env_TERM_extern );
    	}
	    sql04_envdef_TERM_extern = ptr ;
    }

    if ( sql04_env_TERMINFO_extern )
    {
		/* SCREENINFO= + $TERMINFO + '\0' */
		/*     11      +    ???    +   1  */
    	if ( ALLOC_MEM_EO57( (void **)&ptr, 12 + (unsigned) strlen(sql04_env_TERMINFO_extern) )
              != NO_ERROR_EO001 )
        {
            return ( -3 );
        }
        else
        {
#ifndef	SCRLIB
	        spri ( ptr , "TERMINFO=%s" , sql04_env_TERMINFO_extern );
#else
	        spri ( ptr , "SCREENINFO=%s" , sql04_env_TERMINFO_extern );
#endif
    	}
	    sql04_envdef_TERMINFO_extern = ptr ;
    }

    /*
     *  DBTERM overrides TERM.
     *  To make this to affect curses, put it as TERM to the environment.
     */
    if ( sql04_env_DBTERM && *sql04_env_DBTERM )
    {
		/* TERM= + $DBTERM + '\0' */
		/*   5   +   ???   +   1  */
    	if ( ALLOC_MEM_EO57( (void **)&ptr, 6 + (unsigned) strlen(sql04_env_DBTERM) ) 
            != NO_ERROR_EO001 )
        {
    	    return ( -3 );
	    }
        else
        {
            spri ( ptr , "TERM=%s" , sql04_env_DBTERM );
            putenv(ptr);
    	}
	    sql04_envdef_TERM = ptr ;
	    sql04_env_TERM    = ptr + 5 ; /* point to the TERM name */
    }
    else
    {
    	sql04_env_DBTERM  = 0 ;
	    sql04_envdef_TERM = sql04_envdef_TERM_extern ;
	    sql04_env_TERM    = sql04_env_TERM_extern ;
    }

    /*
     *  We need TERM or DBTERM.
     */
    if ( ! sql04_env_TERM || ! *sql04_env_TERM )
    {
    	MSGD (( -11987, ERR_TYPE, "VIRTUALS", "TERM or DBTERM must be set \n" ));
	    return ( -2 );
    }

    /*
     *  Without IndependentTerminfoPath we may run with standard terminfo.
     *  However none of the extended features are available.
     */

	/* PTS 1107043 */
    if ( !sqlGetIndependentTerminfoPath( TerminfoPath, TERM_WITHOUT_DELIMITER_EO01, &RteError ) )
    {
    	sql04_envdef_TERMINFO = sql04_envdef_TERMINFO_extern ;
	    sql04_env_TERMINFO    = sql04_env_TERMINFO_extern ;
    	return ( 0 );
    }

    DBG1 (( MF__,"sqlGetIndependentTerminfoPath          = '%s'\n", TerminfoPath ));

    /*
     *  DBTERM enables the use of DB-private terminfo files.
     *  To make this to affect curses, put TERMINFO to the environment.
     *  This will be done only, if the terminfo file
     *  $DBROOT/terminfo/?/$DBTERM exists.
     */
    if ( sql04_env_DBTERM && *sql04_env_DBTERM )
    {
		    /* SCREENINFO= + TerminfoPath /?/ + $DBTERM + '\0' */
		    /*    12       +   ???        +   ???   +   1  */
	    if ( ALLOC_MEM_EO57( (void**)&ptr, 
            12 + (unsigned) strlen(TerminfoPath) + (unsigned) strlen(sql04_env_TERM) )
                != NO_ERROR_EO001 )
        {
    	    return ( -3 );
        }
	    /* build the file name */
	    spri ( ptr , "%s/%c/%s" , TerminfoPath ,  *sql04_env_TERM , sql04_env_TERM );
	    /* check for existence */
	    exists = ( access(ptr , 0) >= 0 );
	    if ( exists )
        {
		/* build the environment string */
#ifndef	SCRLIB
	    	spri ( ptr , "TERMINFO=%s" , TerminfoPath );
#else
    		spri ( ptr , "SCREENINFO=%s" , TerminfoPath );
#endif
        }
	    else
        {
		    FREE_MEM_EO57( ptr );
        }
	    if ( exists && putenv(ptr) )
    	{
    	    return ( -3 );
	    }
    	if ( exists )
	    {
    	    sql04_envdef_TERMINFO = ptr ;
#ifndef	SCRLIB
	        sql04_env_TERMINFO    = ptr + 9 ; /* point to the TERMINFO name */
#else
	        sql04_env_TERMINFO    = ptr + 11 ; /* point to the TERMINFO name */
#endif
    	}
	    else
    	{
	        sql04_envdef_TERMINFO = sql04_envdef_TERMINFO_extern ;
	        sql04_env_TERMINFO    = sql04_env_TERMINFO_extern ;
    	}
    }
    else
    {
	    sql04_envdef_TERMINFO = sql04_envdef_TERMINFO_extern ;
    	sql04_env_TERMINFO    = sql04_env_TERMINFO_extern ;
    }

    /*
     *  DBHIF need to be set only if it differs from DBTERM/TERM.
     *  The .hif file, if existent, is used anyway.
     */
    if ( ! sql04_env_DBHIF || ! *sql04_env_DBHIF )
    {
	sql04_env_DBHIF = sql04_env_TERM ;
    }

    /*
     *  There is a default for the character class.
     */
    if ( ! sql04_env_DBCHRCLASS ) sql04_env_DBCHRCLASS = "german" ;

    DBG1 (( MF__,"DBTERM          = '%s'\n",
		sql04_env_DBTERM          ? sql04_env_DBTERM          : "" ))
    DBG1 (( MF__,"DBHIF           = '%s'\n",
		sql04_env_DBHIF           ? sql04_env_DBHIF           : "" ))
    DBG1 (( MF__,"DBCHRCLASS      = '%s'\n",
		sql04_env_DBCHRCLASS      ? sql04_env_DBCHRCLASS      : "" ))
    DBG1 (( MF__,"TERM            = '%s'\n",
		sql04_env_TERM            ? sql04_env_TERM            : "" ))
    DBG1 (( MF__,"TERMINFO        = '%s'\n",
		sql04_env_TERMINFO        ? sql04_env_TERMINFO        : "" ))
    DBG1 (( MF__,"TERM_extern     = '%s'\n",
		sql04_env_TERM_extern     ? sql04_env_TERM_extern     : "" ))
    DBG1 (( MF__,"TERMINFO_extern = '%s'\n",
		sql04_env_TERMINFO_extern ? sql04_env_TERMINFO_extern : "" ))
    return ( 1 );
}

/*==========================================================================*/

	int	sql04_make_key_list ( )

{
    int				rc ;


    DBG1 (( MF__,"make_key_list: called \n" ))

    /*
     *  If already called before, do nothing.
     */
    if ( sql04_key_list ) return ( 0 );

    /*
     *  Try to read the hif key file.
     */
    rc = sql04_read_hifkeys ();
    if ( rc < 0 ) return ( rc );

    /*
     *  If no hif keys were loaded, use the curses keys.
     */
    if ( ! rc ) rc = sql04_get_curses_keys ();
    if ( rc < 0 ) return ( rc );

    if ( ! rc )
    {
	MSGD (( -11987, WRN_TYPE, "VIRTUALS", "WARNING: no keys are defined!" ))
    }

    return ( rc );
}

/*==========================================================================*/

static	int	sql04_read_hifkeys ( )


{
    int					lno = 0 ;
    int					errcnt = 0 ;
    int					lgt ;
    int					idx ;
    FILE				* fp ;
    char				* capname ;
    char				* value ;
    tsp00_Pathc 		file ;
    char				line [ 256 ];
    tsp01_RteError      RteError ;


    DBG1 (( MF__,"read_hifkeys: called \n" ))

    /*
     *  Hif-Key files can only be used, if DBROOT is available.
     *  Note, that DBHIF is copied from TERM or DBTERM if not set.
     */

	/* PTS 1107043 */
    if ( !sqlGetIndependentTerminfoPath( file, TERM_WITHOUT_DELIMITER_EO01, &RteError ) )
	{
        MSGD (( -11987, ERR_TYPE, "VIRTUALS","Could not determine terminfo path!\n" ))
        MSGD (( -11987, ERR_TYPE, "VIRTUALS","RTE_ERROR: %s\n", RteError.RteErrText ))
        MSGD (( -11987, ERR_TYPE, "VIRTUALS","OS_ERROR : %s\n", RteError.OsErrText ))
	    return ( 0 );
	}

    if (! sql04_env_DBHIF )
    {
    	MSGD (( -11987, ERR_TYPE, "VIRTUALS", "No Hif-Key-File: DBHIF not set" ));
	    return ( 0 );
    }

    strcat( file , "/term/" ) ;
    strcat( file , sql04_env_DBHIF ) ;
    strcat( file , ".hif" ) ;

    /*
     *  Open the file.
     */
    fp = fopen ( file , "r" );
    if ( ! fp )
    {
    	if ( errno != ENOENT )
        {
            MSGD (( -11987, ERR_TYPE, "VIRTUALS", "cannot open '%s': %s", file , sqlerrs() ));
        }
	    return ( 0 );
    }
    DBG1 (( MF__,"opened hifkey file '%s' \n", file ))

    /*
     *  Read the file, each capability on a separate line.
     */
    while ( fgets(line , sizeof(line) , fp) )
    {
	lno ++ ;
	lgt = strlen ( line );
	if ( line[lgt-1] == '\n' ) lgt -- ;
	line[lgt] = 0 ;
	DBGG (( "line %d: '%s' \n", lno , line ))

	/*
	 *  Skip white space.
	 */
	for ( idx = 0 ; isspace(line[idx]) ; idx ++ );

	/*
	 *  Ignore comments, which are defined form '#' to end of line.
	 *  Ignore empty lines.
	 */
	if ( line[idx] == '#' ) continue ;
	if ( line[idx] == 0   ) continue ;

	/*
	 *  Capability names must begin with a letter.
	 */
	if ( ! isalpha(line[idx]) )
	{
	    MSGD (( -11987, ERR_TYPE, "VIRTUALS", 
    	    "syntax error in '%s', line %d: illegal capname '%s'",
					    file , lno , line ));
	    errcnt ++ ;
	    continue ;
	}
	capname = line + idx ;

	/*
	 *  Search for the end of the capability name.
	 *  Capability names consists of letters and digits.
	 */
	while ( isalnum(line[idx]) ) idx ++ ;

	/*
	 *  Only string capabilities are currently allowed.
	 */
	if ( line[idx] != '=' )
	{
	    MSGD (( -11987, ERR_TYPE, "VIRTUALS",
	    "syntax error in '%s', line %d: no string capability '%s'",
					    file , lno , line ))
	    errcnt ++ ;
	    continue ;
	}

	/*
	 *  Terminate the capability name.
	 */
	line[idx ++] = 0 ;
	value = line + idx ;

	/*
	 *  Capability fields can be terminated by ','.
	 */
	while ( (line[idx] != ',') && (line[idx] != 0) )
	{
	    /* backslash may escape ',' */
	    if ( (line[idx] == '\\') && (line[idx+1] != 0) ) idx ++ ;
	    idx ++ ;
	}

	/*
	 *  Terminate the capability value.
	 */
	line[idx ++] = 0 ;

	/*
	 *  Convert the ascii string value to binary.
	 *  This can be done in place, because the binary
	 *  representation can never be longer than the original.
	 */
	sql04_get_sequence ( value );

	/*
	 *  Insert the key definition into the sorted key_list.
	 *  The list is sorted by key sequences (capability values).
	 */
	if ( sql04_ins_hifinfo ( capname , value , file , lno ) < 0 )
							    errcnt ++ ;

	/*
	 *  Proceed with the next input line.
	 */
    }

    /*
     *  Returning > 0 for success.
     *  Returning   0 for nothing done (already returned above).
     *  Returning < 0 for error.
     */
    idx = (errcnt > 0) ? -1 : 1 ;
    DBG1 (( MF__,"read_hifkeys: returning %d \n", idx ))
    return ( idx );
}

/*========================================================================*/

static	int	sql04_ins_hifinfo (
char				* capname ,
char				* value ,
char				* file ,
int				lno )

{
    int				rc ;
    register char		*ptr1 ;
    register char		*ptr2 ;
    struct capname_info		*cip ;
    struct hifcap_info		*hip ;


    DBGG (( "ins_hifinfo:  capname '%s' value '%s' \n",
			capname , sql04_put_sequence(value) ))

    if ( *capname == 'k' )
    {
	for ( cip = sql04_cap_table ; cip->ci_function ; cip ++ )
	{
	    /*
	     *  The call for strcmp(3S) seems to be too expensive
	     *  for the short strings.
	     */
	    for ( ptr1 = capname , ptr2 = cip->ci_capname ;
		  *ptr1 && (*ptr1 == *ptr2) ;
		  ptr1 ++ , ptr2 ++ );
	    if ( *ptr2 == *ptr1 ) break ;
	}
	if ( (*ptr1 != *ptr2) || ! cip->ci_function )
	{
	    MSGD (( -11987, WRN_TYPE, "VIRTUALS", "WARNING: unknown capname '%s' in '%s', line %d \n",
						    capname , file , lno ))
	    return ( 0 );
	}

	rc = sql04_ins_keyinfo ( cip->ci_function , value );
    }
    else
    {
	for ( hip = sql04_hif_table ; hip->hi_sequence ; hip ++ )
	{
	    /*
	     *  The call for strcmp(3S) seems to be too expensive
	     *  for the short strings.
	     */
	    for ( ptr1 = capname , ptr2 = hip->hi_capname ;
		  *ptr1 && (*ptr1 == *ptr2) ;
		  ptr1 ++ , ptr2 ++ );
	    if ( *ptr2 == *ptr1 ) break ;
	}
	if ( (*ptr1 != *ptr2) || ! hip->hi_sequence )
	{
	    MSGD (( -11987, WRN_TYPE, "VIRTUALS", "WARNING: unknown capname '%s' in '%s', line %d \n",
						    capname , file , lno ))
	    return ( 0 );
	}

	if ( ALLOC_MEM_EO57( (void **)hip->hi_sequence, strlen(value)+1 ) != NO_ERROR_EO001 )
    {
	    return ( -3 );
	}
	(void) strcpy ( *(hip->hi_sequence) , value );
	DBG1 (( MF__,"found '%s' = '%s' in '%s', line %d \n",
					capname , value , file , lno ))
    }

    return ( rc );
}

/*==========================================================================*/

	int	sql04_ins_keyinfo ( 
int				function ,
char				* sequence )
{
    int				lgt ;
    register char		* ptr1 ;
    register char		* ptr2 ;
    struct key_info		* kip ;
    struct key_info		** skipp ;


    /*
     *  Insert only defined keys.
     */
    if ( ! sequence || ! *sequence )
    {
	DBG1 (( MF__,"ins_keyinfo: %3d (%s) '%s' \n", function ,
	    sql04_key_name(function) , sequence?sequence:"(null)" ))
	return ( 0 );
    }
    DBG1 (( MF__,"%3d (%s) '%s' \n", function ,
	sql04_key_name(function) , sql04_put_sequence(sequence) ))

    /*
     *  The sequence is assumed NOT to contain '\0'.
     *  '\0' is mapped to '\200' by tic(1M).
     *  Allocate a key_info struct to hold the key information.
     */
    lgt = strlen ( sequence );
    if ( ALLOC_MEM_EO57( (void **)&kip, sizeof(struct key_info) + lgt ) != NO_ERROR_EO001 )
    {
    	return ( -3 );
    }
    kip->ki_function = function ;
    SAPDB_memcpy ( kip->ki_sequence , sequence , lgt + 1 ); /* include '\0' */

    /*
     *  The key_list is sorted by key sequence.
     */
    /* initialize the pointers to compare unequal (the list may be empty) */
    ptr1 = sequence ;
    ptr2 = "" ;
    for ( skipp = & sql04_key_list ; *skipp ; skipp = &(*skipp)->ki_next )
    {
	/* don't use strcmp(3S), it may perform signed character comparison */
	for ( ptr1 = sequence , ptr2 = (*skipp)->ki_sequence ;
	      *ptr1 && (*ptr1 == *ptr2) ;
	      ptr1 ++ , ptr2 ++ );
	if ( (*ptr2 & 0xff) >= (*ptr1 & 0xff) ) break ;
    }
    if ( *ptr1 == *ptr2 )
    {
	    MSGD (( -11987, WRN_TYPE, "VIRTUALS",
	        "WARNING: multiple keys for sequence '%s' (%s=%s) \n",
	        sql04_put_sequence(sequence) ,
	        sql04_env_DBTERM ? "DBTERM" : "TERM" , sql04_env_TERM ))
	    FREE_MEM_EO57 ( kip );
	    (void) sleep ( 1 );
	    return ( 0 );
    }
    kip->ki_next = *skipp ;
    *skipp = kip ;

    return ( 1 );
}

/*==========================================================================*/

/*
 *  Convert a sequence to a string.
 *  The sequence consists of:
 *  x		x (unescaped character (except \ and ^) is taken as itself)
 *  ^x		CTRL-x (control-x, x & 0x1f, where x is any character)
 *  \n		newline
 *  \r		carriage return
 *  \b		backspace
 *  \f		form feed
 *  \t		tab
 *  \e		ESC (escape, 0x1b)
 *  \E		ESC (escape, 0x1b)
 *  \x		x (where x is any character which is taken as itself)
 *  \\		backslash
 *  \^		^ (circumflex, 0x5e)
 *  \0		NUL converted to \200 (like tic(1M) does)
 *  \377	(exactly three octal digits form a character)
 *  The sequence is converted in place. The result cannot be longer
 *  than the original.
 */

static	void	sql04_get_sequence ( sequence )

char			* sequence ;

{
    register char		* src ;
    register char		* dst ;


    for ( src = dst = sequence ; *src ; src ++ )
    {
	switch ( *src )
	{
	case '\\' :
	    if ( ! src[1] ) break ;
	    switch ( *(++src) )
	    {
	    case 'n' :
		*(dst++) = '\n' ;
		break ;
    
	    case 'r' :
		*(dst++) = '\r' ;
		break ;
    
	    case 'b' :
		*(dst++) = '\b' ;
		break ;
    
	    case 'f' :
		*(dst++) = '\f' ;
		break ;
    
	    case 't' :
		*(dst++) = '\t' ;
		break ;
    
	    case 'e' :
	    case 'E' :
		*(dst++) = '\033' ;
		break ;
    
	    case '0' :
		*(dst++) = '\200' ; /* tic(1M) does this conversion */
		break ;
    
	    default :
		if ( (src[0] >= '0') && (src[0] <= '7') &&
		     (src[1] >= '0') && (src[1] <= '7') &&
		     (src[2] >= '0') && (src[2] <= '7')    )
		{
		    *(dst++) = (src[0] - '0') << 6 |
			       (src[1] - '0') << 3 |
			       (src[2] - '0');
		    src += 2 ;
		}
		else
		{
		    *(dst++) = *src ;
		}
	    }
	    break ;

	case '^' :
	    if ( ! src[1] ) break ;
	    *(dst++) = *(++src) & 0x1f ;
	    break ;

	default :
	    *(dst++) = *src ;
	    break ;
	}
    }
    *dst = 0 ;
}

/*==========================================================================*/

/*
 *  Convert a string to a sequence.
 *  The sequence consists of:
 *  x		where x is any printable character (except \ and ^)
 *  \n		for a newline
 *  \r		for a carriage return
 *  \b		for a backspace
 *  \f		for a form feed
 *  \t		for a tab
 *  \e		for a ESC (escape, 0x1b)
 *  \\		for a backslash
 *  \^		for a ^ (circumflex, 0x5e)
 *  ^x		where x is a control character in the range of 0x01 to 0x1f
 *  \377	where x is any other character (377 means three octal digits)
 */

	char	* sql04_put_sequence ( sequence )

char			* sequence ;

{
    register char		* src ;
    register char		* dst ;
static	char			buf [ 256 ];


    for ( src = sequence , dst = buf ; *src ; src ++ )
    {
	if ( sql04_isprint(*src) &&
	     (*src != '\\') && (*src != '^') && (*src != ' ') )
	{
	    *(dst++) = *src ;
	    continue ;
	}

	switch ( *src )
	{
	case '\n' :
	    *(dst++) = '\\' ;
	    *(dst++) = 'n' ;
	    break ;

	case '\r' :
	    *(dst++) = '\\' ;
	    *(dst++) = 'r' ;
	    break ;

	case '\b' :
	    *(dst++) = '\\' ;
	    *(dst++) = 'b' ;
	    break ;

	case '\f' :
	    *(dst++) = '\\' ;
	    *(dst++) = 'f' ;
	    break ;

	case '\t' :
	    *(dst++) = '\\' ;
	    *(dst++) = 't' ;
	    break ;

	case '\033' :
	    *(dst++) = '\\' ;
	    *(dst++) = 'E' ;
	    break ;

	case '\\' :
	    *(dst++) = '\\' ;
	    *(dst++) = '\\' ;
	    break ;

	case '^' :
	    *(dst++) = '\\' ;
	    *(dst++) = '^' ;
	    break ;

	default :
	    if ( ! (*src & ~0x1f) )
	    {
		*(dst++) = '^' ;
		*(dst++) = *src | '@' ;
	    }
	    else
	    {
		*(dst++) = '\\' ;
		*(dst++) = (((*src) >> 6) & 03) + '0' ;
		*(dst++) = (((*src) >> 3) & 07) + '0' ;
		*(dst++) = (((*src) >> 0) & 07) + '0' ;
	    }
	}
    }
    *dst = 0 ;

    return ( buf );
}

/*==========================================================================*/

	int	sql04_cat ( file )

char			* file ;

{
	int			ich ;
	FILE			* fp ;


	errno = 0 ;
	fp = fopen ( file , "r" );
	if ( ! fp )
	{
	    if ( errno == ENOENT )
		return ( 0 );
	    else
		return ( -2 );
	}

	while ( (ich = getc(fp)) != EOF ) (void) putchar ( ich );

	(void) fclose ( fp );

	return ( 1 );
}

/*==========================================================================*/

	int	sql04_read_chrclass ( )

{
    int			fd ;
    int			lgt ;
    int			idx ;
    int			ich ;
    int			in_number ;
    int			numlgt ;
    tsp00_Pathc file ;
    char		line [ 256 ];
    char		number [ 16 ];
    tsp01_RteError  RteError ;


    DBG1 (( MF__,"read_chrclass: called \n" ))

	/* PTS 1107043 */
    if ( !sqlGetIndependentTerminfoPath( file, TERM_WITHOUT_DELIMITER_EO01, &RteError ) ||
		 !sql04_env_DBCHRCLASS )
		 return ( 0 );

    (void) strcat ( file , "/chrclass/") ;
    (void) strcat ( file , sql04_env_DBCHRCLASS ) ;

    fd = open ( file , O_RDONLY , 0 );
    if ( fd < 0 )
    {
    	if ( errno == ENOENT ) return ( 0 );
	    MSGD (( -11987, ERR_TYPE, "VIRTUALS", "cannot open '%s': %s \n", file , sqlerrs() ))
    	return ( -2 );
    }
    DBG1 (( MF__,"chrclass: opened '%s'\n", file ))

    in_number = FALSE ;
    numlgt = 0 ;
    for ( ; ; )
    {
	lgt = read ( fd , line , sizeof(line) );
	if ( lgt <= 0 ) break ;

	for ( idx = 0 ; idx < lgt ; idx ++ )
	{
	    ich = line[idx] & 0xff ;
	    if ( ! isspace(ich) )
	    {
		DBGG (( "chrclass: found char 0x%02x\n", ich ))
		sql04_chrclass[ich] = 1 ;

		if ( in_number )
		{
		    if ( numlgt < sizeof(number)-1 )
			number[numlgt++] = (char) ich ;
		}
		else
		{
		    if ( isdigit(ich) )
		    {
			in_number = TRUE ;
			numlgt = 0 ;
			number[numlgt++] = (char) ich ;
		    }
		}
	    }
	    else
	    {
		if ( in_number )
		{
		    in_number = FALSE ;
		    number[numlgt] = 0 ;
		    ich = (int) strtol ( number , (char**)0 , 0 );
		    if ( (ich > 0) && (ich < 256) ) sql04_chrclass[ich] = 1 ;
		}
	    }
	}
    }

    (void) close ( fd );

#ifdef	DEBUG_RTE
    DBG1 (( MF__,"chrclass values: \n" ))
    for ( idx = 0 ; idx < 16 ; idx ++ )
    {
	for ( lgt = 0 ; lgt < 16 ; lgt ++ )
	{
	    line[lgt*2  ] = sql04_chrclass[(idx<<4)|lgt] ? '1' : '0' ;
	    line[lgt*2+1] = ' ' ;
	}
	line[lgt*2] = 0 ;
	DBG1 (( MF__,"0x%02x:  %s \n", idx<<4 , line ))
    }
#endif

    return ( 1 );
}

/*==========================================================================*/

	int	sql04_isprint ( ich )

int			ich ;

{
    if ( (ich < 0) || (ich > 255) )
    {
        return ( FALSE );
    }
    else
    {
#if defined(NMP)
        return ( isgraph(ich) || isspace(ich) || sql04_chrclass[ich] );
#else
        return ( isprint(ich) || sql04_chrclass[ich] );
#endif
    }
}

/*==========================================================================*/

	int	sql04_init_graphic_table ( )

{
    int				idx ;
    int				has_graphic ;


    DBG1 (( MF__,"gratab: called \n" ))

    /*
     *  Graphic Table initialized with question marks.
     */
    for ( idx = 0 ; idx < MAXGRAPHIC ; idx ++ )
    {
	sql04_graphic[idx] = '?' ;
    }

    /*
     *  Different versions of curses have different methods to
     *  display graphic characters.
     */
#if defined(ACS_PLUS) && ! OSF1
    /* curses decides whether to use graphic mode or replacement chars */
    has_graphic = TRUE ;
    DBG1 (( MF__,"gratab: ACS_ characters are defined \n" ))
#else
#if _IBMR2 || OSF1
    /* these machines use the 'box1' feature */
    has_graphic = ((box_chars_1 != 0) && (*box_chars_1 != 0));
    DBG1 (( MF__,"gratab: box1 is %s present \n", has_graphic?"":"not"  ))
#else
    /* test if the 'smacs' feature is present */
    has_graphic = (enter_alt_charset_mode != 0);
    DBG1 (( MF__,"gratab: smacs is %s present \n", has_graphic?"":"not" ))
#endif
#endif

    if ( ! has_graphic )
    {
	/*
	 *  Graphic Characters initialized with replacement characters.
	 */
	sql04_graphic [ CH_VERTICAL_BAR   ] = '|' ;
	sql04_graphic [ CH_L_RIGHT_CORNER ] = '*' ;
	sql04_graphic [ CH_U_RIGHT_CORNER ] = '*' ;
	sql04_graphic [ CH_L_LEFT_CORNER  ] = '*' ;
	sql04_graphic [ CH_U_LEFT_CORNER  ] = '*' ;
	sql04_graphic [ CH_LEFT_T         ] = '|' ;
	sql04_graphic [ CH_RIGHT_T        ] = '|' ;
	sql04_graphic [ CH_HORIZONTAL_BAR ] = '-' ;
	sql04_graphic [ CH_BOTTOM_T       ] = '-' ;
	sql04_graphic [ CH_TOP_T          ] = '-' ;
	sql04_graphic [ CH_CROSS          ] = '+' ;

	return ( FALSE );	/* no graphic */
    }

    /*
     *  Graphic Characters initialized with curses specific values.
     */
#if defined(ACS_PLUS) && ! OSF1
    sql04_graphic [ CH_L_RIGHT_CORNER ] = ACS_LRCORNER ;
    sql04_graphic [ CH_U_RIGHT_CORNER ] = ACS_URCORNER ;
    sql04_graphic [ CH_U_LEFT_CORNER  ] = ACS_ULCORNER ;
    sql04_graphic [ CH_L_LEFT_CORNER  ] = ACS_LLCORNER ;
    sql04_graphic [ CH_CROSS          ] = ACS_PLUS     ;
    sql04_graphic [ CH_HORIZONTAL_BAR ] = ACS_HLINE    ;
    sql04_graphic [ CH_LEFT_T         ] = ACS_LTEE     ;
    sql04_graphic [ CH_RIGHT_T        ] = ACS_RTEE     ;
    sql04_graphic [ CH_BOTTOM_T       ] = ACS_BTEE     ;
    sql04_graphic [ CH_TOP_T          ] = ACS_TTEE     ;
    sql04_graphic [ CH_VERTICAL_BAR   ] = ACS_VLINE    ;
#else	/*ACS_PLUS*/
#if	_IBMR2 || OSF1
    sql04_graphic [ CH_U_LEFT_CORNER  ] = box_chars_1 [  0 ] | A_ALTCHARSET ;
    sql04_graphic [ CH_HORIZONTAL_BAR ] = box_chars_1 [  1 ] | A_ALTCHARSET ;
    sql04_graphic [ CH_U_RIGHT_CORNER ] = box_chars_1 [  2 ] | A_ALTCHARSET ;
    sql04_graphic [ CH_VERTICAL_BAR   ] = box_chars_1 [  3 ] | A_ALTCHARSET ;
    sql04_graphic [ CH_L_RIGHT_CORNER ] = box_chars_1 [  4 ] | A_ALTCHARSET ;
    sql04_graphic [ CH_L_LEFT_CORNER  ] = box_chars_1 [  5 ] | A_ALTCHARSET ;
    sql04_graphic [ CH_TOP_T          ] = box_chars_1 [  6 ] | A_ALTCHARSET ;
    sql04_graphic [ CH_RIGHT_T        ] = box_chars_1 [  7 ] | A_ALTCHARSET ;
    sql04_graphic [ CH_BOTTOM_T       ] = box_chars_1 [  8 ] | A_ALTCHARSET ;
    sql04_graphic [ CH_LEFT_T         ] = box_chars_1 [  9 ] | A_ALTCHARSET ;
    sql04_graphic [ CH_CROSS          ] = box_chars_1 [ 10 ] | A_ALTCHARSET ;
#else	/*_IBMR2 || OSF1*/
    /* use DEC graphic characters */
    sql04_graphic [ CH_L_RIGHT_CORNER ] = 'j' | A_ALTCHARSET ;
    sql04_graphic [ CH_U_RIGHT_CORNER ] = 'k' | A_ALTCHARSET ;
    sql04_graphic [ CH_U_LEFT_CORNER  ] = 'l' | A_ALTCHARSET ;
    sql04_graphic [ CH_L_LEFT_CORNER  ] = 'm' | A_ALTCHARSET ;
    sql04_graphic [ CH_CROSS          ] = 'n' | A_ALTCHARSET ;
    sql04_graphic [ CH_HORIZONTAL_BAR ] = 'q' | A_ALTCHARSET ;
    sql04_graphic [ CH_LEFT_T         ] = 't' | A_ALTCHARSET ;
    sql04_graphic [ CH_RIGHT_T        ] = 'u' | A_ALTCHARSET ;
    sql04_graphic [ CH_BOTTOM_T       ] = 'v' | A_ALTCHARSET ;
    sql04_graphic [ CH_TOP_T          ] = 'w' | A_ALTCHARSET ;
    sql04_graphic [ CH_VERTICAL_BAR   ] = 'x' | A_ALTCHARSET ;
#endif	/*_IBMR2 || OSF1*/
#endif	/*ACS_PLUS*/

    DBG1 (( MF__,"gratab: VERT_BAR  0x%08lx \n", sql04_graphic[CH_VERTICAL_BAR  ] ))
    DBG1 (( MF__,"gratab: L_R_CRNR  0x%08lx \n", sql04_graphic[CH_L_RIGHT_CORNER] ))
    DBG1 (( MF__,"gratab: U_R_CRNR  0x%08lx \n", sql04_graphic[CH_U_RIGHT_CORNER] ))
    DBG1 (( MF__,"gratab: L_L_CRNR  0x%08lx \n", sql04_graphic[CH_L_LEFT_CORNER ] ))
    DBG1 (( MF__,"gratab: U_L_CRNR  0x%08lx \n", sql04_graphic[CH_U_LEFT_CORNER ] ))
    DBG1 (( MF__,"gratab: LEFT_T    0x%08lx \n", sql04_graphic[CH_LEFT_T        ] ))
    DBG1 (( MF__,"gratab: RIGHT_T   0x%08lx \n", sql04_graphic[CH_RIGHT_T       ] ))
    DBG1 (( MF__,"gratab: HORZ_BAR  0x%08lx \n", sql04_graphic[CH_HORIZONTAL_BAR] ))
    DBG1 (( MF__,"gratab: BOTTOM_T  0x%08lx \n", sql04_graphic[CH_BOTTOM_T      ] ))
    DBG1 (( MF__,"gratab: TOP_T     0x%08lx \n", sql04_graphic[CH_TOP_T         ] ))
    DBG1 (( MF__,"gratab: CROSS     0x%08lx \n", sql04_graphic[CH_CROSS         ] ))

    return ( TRUE );	/* has graphic */
}
