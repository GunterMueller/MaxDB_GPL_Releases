/*!
  @file           hos04u.h
  @author         RaymondR
  @brief          virtual terminal
  @see            

\if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

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


#ifndef HOS04U_H
#define HOS04U_H


// --- virtual terminal constants ---------------------------------------------

#define VT_MAX_SCREEN_CHARS     32767   // 0x7FFF
#define VT_MAX_SCREEN_LINES     32767   // 0x7FFF
#define VT_MAX_LINELENGTH       32767   // 0x7FFF

#define VT_LABEL_SIZE           8
#define VT_SK_NUMBER            9

// --- virtual terminal graphic constants -------------------------------------

#define VTG_VERTICAL_BAR        0x08
#define VTG_HORIZONTAL_BAR      0x14
#define VTG_L_RIGHT_CORNER      0x0B
#define VTG_U_RIGHT_CORNER      0x0C
#define VTG_L_LEFT_CORNER       0x10
#define VTG_U_LEFT_CORNER       0x11
#define VTG_LEFT_T              0x12
#define VTG_RIGHT_T             0x13
#define VTG_BOTTOM_T            0x1C
#define VTG_TOP_T               0x1D
#define VTG_CROSSING_LINES      0x1E
#define VTG_BLANK               ' '

// --- virtual terminal keystrokes -- pascal type VT_KEY ----------------------
enum {
  VT_UNKNOWN_KEY,
  VT_CHARACTER_KEY,
  VT_ENTER,
  VT_PF01,
  VT_PF02,
  VT_PF03,
  VT_PF04,
  VT_PF05,
  VT_PF06,
  VT_PF07,
  VT_PF08,
  VT_PF09,
  VT_PF10,
  VT_PF11,
  VT_PF12,
  VT_CLEAR,
  VT_DO_KEY,
  VT_SK01,
  VT_SK02,
  VT_SK03,
  VT_SK04,
  VT_SK05,
  VT_SK06,
  VT_SK07,
  VT_SK08,
  VT_SK09,
  VT_HELP_KEY,
  VT_UP_KEY,
  VT_DOWN_KEY,
  VT_LEFT_KEY,
  VT_RIGHT_KEY,
  VT_CMD_KEY,
  VT_PICK_KEY,
  VT_PUT_KEY,
  VT_MARK_KEY,
  VT_SELECT_KEY,
  VT_MOVE_KEY,
  VT_COPY_KEY,
  VT_INSERT_KEY,
  VT_DELETE_KEY,
  VT_TOP_KEY,
  VT_BOTTOM_KEY,
  VT_CANCEL_KEY,
  VT_UNDO_KEY,
  VT_END_KEY,
  VT_ESCAPE_KEY,
  VT_FILL,
  VT_FILE_OPEN,
  VT_FILE_SAVE,
  VT_EXIT,
  VT_PRINT,
  VT_FIND,
  VT_REPLACE,
  VT_CURSOR_UP,
  VT_CURSOR_DOWN,
  VT_CURSOR_RIGHT,
  VT_CURSOR_LEFT,
  VT_CURSOR_HOME,
  VT_CURSOR_END,
  VT_NEXT_FIELD,
  VT_PREV_FIELD,
  VT_INSERT_CHAR,
  VT_DELETE_CHAR,
  VT_RUBOUT,
  VT_DEL_EOF,
  VT_MOUSE_DOWN,
  VT_MOUSE_UP,
  VT_MOUSE_MOVE,
  VT_MOUSE_DBL,
  VT_HSCROLL,
  VT_VSCROLL,
  VT_CONTROL_KEY,
  VT_CURSOR_BOL,
  VT_CURSOR_EOL,
  VT_LAST_KEY
  };

// --- virtual terminal attributes -- pascal type VT_MODE ---------------------

enum {
  VT_BRIGHT,
  VT_INVERSE,
  VT_UNDERLINE,
  VT_BLINK,
  VT_INVISIBLE,
  VT_MIXED,
  VT_GRAYED,
  VT_LAST_MODE
  };

// --- virtual terminal colors -- pascal type VT_COLOR ------------------------

enum {
  VT_DEFAULT_COLOR,
  VT_WHITE,
  VT_BLACK,
  VT_RED,
  VT_GREEN,
  VT_YELLOW,
  VT_BLUE,
  VT_PINK,
  VT_LIGHT_BLUE,
  VT_LAST_COLOR
  };

// --- virtual terminal usage mode -- pascal type VT_USAGE_MODE ---------------

enum {
  VT_FORM,
  VT_EDIT
  };

// --- virtual terminal softkey types -- pascal type SK_LABEL_TYPE ------------

enum {
  NO_SK_LABELS,
  SHORT_SK_LABELS,
  LONG_SK_LABELS
  };

// --- Virtual terminal -------------------------------------------------------

typedef enum1                   VT_KEY;
typedef enum1                   VT_LOCAL_FUNCTION;
typedef enum1                   VT_MODE;
typedef enum1                   VT_USAGE_MODE;
typedef enum1                   VT_COLOR;
typedef enum1                   VT_MARK;
typedef enum1                   VT_SK_LABEL_TYPE;
typedef enum1                   VT_DBCS_TYPE;

typedef char                    VT_LABEL  [ VT_LABEL_SIZE ];
typedef char                    VT_LABELC [ VT_LABEL_SIZE + 1 ];

typedef char                    C80   [ 80  ];
typedef char                    C80C  [ 81  ];

#endif
