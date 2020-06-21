/*!
  @file           vos04vu.c
  @author         RaymondR
  @brief          Virtual Terminal for Win NT
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




/*
 * INCLUDE FILES
 */
#include "gos00.h"
#include "heo46.h"
#include "geo007_1.h"
#include "gos00ds.h"
#include "hos04u.h"


//
// DEFINES
//

#define MOD__  "VOS04VU : "
#define MF__   MOD__"UNDEFINED"


// size defines

#if defined(_WIN32)
  #define VTD_CELL_SIZE             (sizeof(CHAR_INFO))
#else
  #define VTD_CELL_SIZE             2   // number of bytes for a
                                        // character/attribute cell
#endif

#define VTD_KEYB_EVENT              0
#define VTD_MOUSE_EVENT             1

#define VTD_MOUSE_DBL_DEFAULT       "500" // msec

#define VTD_MOUSE_DOWN              0x0004
#define VTD_MOUSE_UP                0x0000
#define VTD_MOUSE_DBL               0x8000

#define VTD_MAX_ATTR_COLOR_ENTRIES  16  // attribute/color table with 16 entries
                                        // 0 - 15


//
// display parameters
//
#define VTD_ROWS_DEFAULT            40  // lines in VTerm presentation space
#define VTD_COLUMNS_DEFAULT         80  // columns in VTerm presentation space
#define VTD_MAX_LINES               70
#define VTD_FIRST_LINE              0   // first line for logical screen
#define VTD_FIRST_COLUMN            0   // first column for logical screen


#if defined ( SOFTKEY_SYSLINE )
 #define VTD_SYSLINE                -1  // system line number
 #define VTD_SKLINE                 -1  // softkey line number
#endif


// field delimiters in screen and attribute buffer

#define VTD_FIELD_DELIMITER_EOF     0x00    // end of field
#define VTD_FIELD_DELIMITER_SONCF   0x01    // start of non-contiguous field
#define VTD_FIELD_DELIMITER_SOCF    0x02    // start of contiguous field



#define VTD_KEYSTROKE_UNDEF         (BYTE)0xFF  // "undefined" value for
                                                // vt_keystroke fields


// RTE-VT attribute definitions
#define VTD_RTE_ATTR_INPUT          0x80    // input field
#define VTD_RTE_ATTR_CHANGED        0x10    // attr. for a changed screen item


// IBM display attributes for the attributes in the attribute/color table
#define VTD_ATTR_UNDEF              0x00
#define VTD_ATTR_BRIGHT             0x01
#define VTD_ATTR_INVERSE            0x02
#define VTD_ATTR_BLINK              0x04
#define VTD_ATTR_INVISIBLE          0x08

#define VTD_ATTR_NORMAL             0x07    // normale darstellung wei� auf
                                            // schwarz

#define VTD_ATTR_BLACK              0x00    // colors
#define VTD_ATTR_BLUE               0x01
#define VTD_ATTR_GREEN              0x02
#define VTD_ATTR_LIGHT_BLUE         0x03
#define VTD_ATTR_RED                0x04
#define VTD_ATTR_PINK               0x05
#define VTD_ATTR_YELLOW             0x06
#define VTD_ATTR_WHITE              0x07


// IBM scan codes for graphic characters
#define DBTERM_ANSI                      1
#define DBTERM_ANSI_COL                  2

#define PLUS                             43
#define MINUS                            45
#define PIPE                             124

#define VTD_GC_ANSI_VERTICAL_BAR         PIPE
#define VTD_GC_ANSI_HORIZONTAL_BAR       MINUS
#define VTD_GC_ANSI_L_RIGHT_CORNER       PLUS
#define VTD_GC_ANSI_U_RIGHT_CORNER       PLUS
#define VTD_GC_ANSI_L_LEFT_CORNER        PLUS
#define VTD_GC_ANSI_U_LEFT_CORNER        PLUS
#define VTD_GC_ANSI_LEFT_T               PLUS
#define VTD_GC_ANSI_RIGHT_T              PLUS
#define VTD_GC_ANSI_BOTTOM_T             PLUS
#define VTD_GC_ANSI_TOP_T                PLUS
#define VTD_GC_ANSI_CROSSING_LINES       PLUS

#define VTD_GC_ANSI_COL_VERTICAL_BAR     179
#define VTD_GC_ANSI_COL_HORIZONTAL_BAR   196
#define VTD_GC_ANSI_COL_L_RIGHT_CORNER   217
#define VTD_GC_ANSI_COL_U_RIGHT_CORNER   191
#define VTD_GC_ANSI_COL_L_LEFT_CORNER    192
#define VTD_GC_ANSI_COL_U_LEFT_CORNER    218
#define VTD_GC_ANSI_COL_LEFT_T           195
#define VTD_GC_ANSI_COL_RIGHT_T          180
#define VTD_GC_ANSI_COL_BOTTOM_T         193
#define VTD_GC_ANSI_COL_TOP_T            194
#define VTD_GC_ANSI_COL_CROSSING_LINES   197

// IBM keyboard scan codes for use with KbdCharIn ()

#define VTD_ESCAPE                  0x1B    // ASCII value, not the scan code
#define VTD_SCAN_CODE               0xE0    // Key is in the scan code
#define VTD_SHIFT_FLAGS             0x03    // Shift flags

#if defined(_WIN32)
  #define VTD_KBD_F1                  VK_F1
  #define VTD_KBD_F2                  VK_F2
  #define VTD_KBD_F3                  VK_F3
  #define VTD_KBD_F4                  VK_F4
  #define VTD_KBD_F5                  VK_F5
  #define VTD_KBD_F6                  VK_F6
  #define VTD_KBD_F7                  VK_F7
  #define VTD_KBD_F8                  VK_F8
  #define VTD_KBD_F9                  VK_F9
  #define VTD_KBD_F10                 VK_F10
  #define VTD_KBD_F11                 VK_F11
  #define VTD_KBD_F12                 VK_F12
  #define VTD_KBD_UP                  VK_UP
  #define VTD_KBD_DOWN                VK_DOWN
  #define VTD_KBD_LEFT                VK_LEFT
  #define VTD_KBD_RIGHT               VK_RIGHT
  #define VTD_KBD_HOME                VK_HOME
  #define VTD_KBD_END                 VK_END
  #define VTD_KBD_PAGEUP              VK_PRIOR
  #define VTD_KBD_PAGEDOWN            VK_NEXT
  #define VTD_KBD_INSERT              VK_INSERT
  #define VTD_KBD_DELETE              VK_DELETE
  #define VTD_KBD_CTRLDELETE          (VK_DELETE | 0x100 /*control*/)
  #define VTD_KBD_CTRLINSERT          (VK_INSERT | 0x100 /*control*/)
  #define VTD_KBD_BACKTAB             (VK_TAB    | 0x200 /*shift*/  )
  #define VTD_KBD_CTRLHOME            (VK_HOME   | 0x100 /*control*/)
  #define VTD_KBD_CTRLEND             (VK_END    | 0x100 /*control*/)
  #define VTD_KBD_CTRL_F1             (VK_F1     | 0x100 /*control*/)
  #define VTD_KBD_CTRL_F3             (VK_F3     | 0x100 /*control*/)
  #define VTD_KBD_CTRL_F9             (VK_F9     | 0x100 /*control*/)
  #define VTD_KBD_CTRL_F10            (VK_F10    | 0x100 /*control*/)
  #define VTD_KBD_SHIFT_HOME          (VK_HOME   | 0x200 /*shift*/)
  #define VTD_KBD_SHIFT_END           (VK_END    | 0x200 /*shift*/)
#else
  #define VTD_KBD_F1                  0x3B
  #define VTD_KBD_F2                  0x3C
  #define VTD_KBD_F3                  0x3D
  #define VTD_KBD_F4                  0x3E
  #define VTD_KBD_F5                  0x3F
  #define VTD_KBD_F6                  0x40
  #define VTD_KBD_F7                  0x41
  #define VTD_KBD_F8                  0x42
  #define VTD_KBD_F9                  0x43
  #define VTD_KBD_F10                 0x44
  #define VTD_KBD_F11                 0x85
  #define VTD_KBD_F12                 0x86
  #define VTD_KBD_UP                  0x48
  #define VTD_KBD_DOWN                0x50
  #define VTD_KBD_LEFT                0x4B
  #define VTD_KBD_RIGHT               0x4D
  #define VTD_KBD_HOME                0x47
  #define VTD_KBD_END                 0x4F
  #define VTD_KBD_PAGEUP              0x49
  #define VTD_KBD_PAGEDOWN            0x51
  #define VTD_KBD_INSERT              0x52
  #define VTD_KBD_DELETE              0x53
  #define VTD_KBD_CTRLDELETE          0x93
  #define VTD_KBD_CTRLINSERT          0x92
  #define VTD_KBD_BACKTAB             0x0F
  #define VTD_KBD_CTRLHOME            0x77
  #define VTD_KBD_CTRLEND             0x75
  #define VTD_KBD_CTRL_F1             0x5E
  #define VTD_KBD_CTRL_F3             0x60
  #define VTD_KBD_CTRL_F9             0x66
  #define VTD_KBD_CTRL_F10            0x67
  #define VTD_KBD_SHIFT_HOME          0x47
  #define VTD_KBD_SHIFT_END           0x4F
#endif


#define VTD_CLEAR_BLINK_BIT         0x7F    // To clear the blink attribute bit
#define VTD_VIDEO_INTENSITY         0x0000  // To set the video intensity
#define VTD_HIDE_HW_CURSOR          0xFFFF  // To hide the hardware cursor
#define VTD_NORMAL_CURSOR_START     -30     // insert cursor start line
#define VTD_NORMAL_CURSOR_END       -100    // insert cursor end line
#define VTD_INSERT_CURSOR_START     -90     // normal cursor start line
#define VTD_INSERT_CURSOR_END       -100    // normal cursor end line


#define VTD_CTRL_A                  0x01    // ASCII for control-a
#define VTD_CTRL_C                  0x03    // ASCII for control-c
#define VTD_CTRL_D                  0x04    // ASCII for control-d
#define VTD_CTRL_F                  0x06    // ASCII for control-f
#define VTD_CTRL_Z                  0x1A    // ASCII for control-z


#define VTD_TERMFIELD_UNDEF         -1      // "undefined" value for
                                            // VTT_TERMFIELD_REC

//
// MACROS
//


// VT common macros

#define LOW_BYTE(byte)          (BYTE) ( (byte) & 0x0F )
#define HI_BYTE(byte)           (BYTE) ( (byte) & 0xF0 )


// attribute macros based on the virtual terminal attribute/color definition

#define IS_ATTR(attribute,is_attr)                              \
( BOOL ) ( attribute & is_attr )

#define SET_ATTR(attribute,set_attr)                            \
( BOOL ) ( attribute |= set_attr )

#define IS_ANY_AC_ATTR_SET(index)                               \
( BOOL ) (( arcAttrColorTable [ index ].fAttribute )        ||  \
          ( arcAttrColorTable [ index ].fForegroundColor )  ||  \
          ( arcAttrColorTable [ index ].fBackgroundColor ))

#define IS_AC_ATTR(index,is_attr)                               \
( BOOL ) (( arcAttrColorTable [ index ].fAttribute ) & is_attr )

#define INIT_AC_ATTR(index)                                     \
( arcAttrColorTable [ index ].fAttribute = VTD_ATTR_UNDEF )

#define SET_AC_ATTR(index,attr)                                 \
( arcAttrColorTable [ index ].fAttribute |= attr )

#define SET_AC_FOREGROUND_COLOR(index,color)                    \
( arcAttrColorTable [ index ].fForegroundColor = color )

#define SET_AC_BACKGROUND_COLOR(index,color)                    \
( arcAttrColorTable [ index ].fBackgroundColor = color )

#define GET_AC_ATTR(index)                                      \
( arcAttrColorTable [ index ].fForegroundColor )

#define GET_AC_FOREGROUND_COLOR(index)                          \
( arcAttrColorTable [ index ].fForegroundColor )

#define GET_AC_BACKGROUND_COLOR(index)                          \
( arcAttrColorTable [ index ].fBackgroundColor )


// attribute macros based on the OS2 V2.00 cell attribute definition

#define SET_CA_FOREGROUND_ATTR(cell,attr)                       \
( cell |= attr )

#define SET_CA_BACKGROUND_ATTR(cell,attr)                       \
( cell |= ( attr << 4 ))

#define SET_CA_INVERSE_ATTR(cell)                               \
( cell = (  cell & 0x08 )        |                              \
         (  cell & 0x80 )        |                              \
         (( cell & 0x07 ) << 4 ) |                              \
         (( cell & 0x70 ) >> 4 ))

#define SET_CA_INVISIBLE_ATTR(cell)                             \
( cell = (  cell & 0x08 )        |                              \
         (  cell & 0x80 )        |                              \
         (( cell & 0x70 ) >> 4 ) |                              \
         (( cell & 0x70 )))

#define SET_CA_BRIGHT_ATTR(cell)                                \
( cell |= 0x08 )

#define SET_CA_BLINK_ATTR(cell)                                 \
( cell |= 0x80 )

#define SET_CA_NOBLINK_ATTR(cell)                               \
( cell &= VTD_CLEAR_BLINK_BIT )

//
// LOCAL TYPE AND STRUCT DEFINITIONS
//

#if !defined(_WIN32)
  // pragma definitions for 16-bit types

  #pragma seg16 ( KBDKEYINFO      )
  #pragma seg16 ( PKBDKEYINFO     )

  #pragma seg16 ( KBDTYPE         )
  #pragma seg16 ( PKBDTYPE        )

  #pragma seg16 ( VIOMODEINFO     )
  #pragma seg16 ( PVIOMODEINFO    )

  #pragma seg16 ( VIOCURSORINFO   )
  #pragma seg16 ( PVIOCURSORINFO  )

  #pragma seg16 ( VIOINTENSITY    )
  #pragma seg16 ( PVIOINTENSITY   )

  #pragma seg16 ( VIOCONFIGINFO   )
  #pragma seg16 ( PVIOCONFIGINFO  )
#endif

// common virtual terminal types (VTT)

typedef char*                   VTT_RETURNKEYS;
typedef VTT_RETURNKEYS*         VTT_PRETURNKEYS;
typedef short                   VTT_RETURNKEYSSIZE;
typedef VTT_RETURNKEYSSIZE*     VTT_PRETURNKEYSSIZE;

typedef BYTE                    VTT_AC_INDEX;
typedef VTT_AC_INDEX*           VTT_PAC_INDEX;
typedef BYTE                    VTT_ATTRIBUTE;
typedef VTT_ATTRIBUTE*          VTT_PATTRIBUTE;
typedef VTT_PATTRIBUTE          VTT_ATTRIBUTES;
typedef VTT_ATTRIBUTES*         VTT_PATTRIBUTES;
typedef short                   VTT_ATTRIBUTESSIZE;
typedef VTT_ATTRIBUTESSIZE*     VTT_PATTRIBUTESSIZE;

typedef char*                   VTT_COLORS;
typedef VTT_COLORS*             VTT_PCOLORS;
typedef BYTE                    VTT_COLOR;
typedef VTT_COLOR*              VTT_PCOLOR;
typedef short                   VTT_COLORSIZE;
typedef VTT_COLORSIZE*          VTT_PCOLORSIZE;

typedef BOOLEAN                 VTT_HASSYSTEMLINE;
typedef VTT_HASSYSTEMLINE*      VTT_PHASSYSTEMLINE;

typedef BOOLEAN                 VTT_HASLABEL;
typedef VTT_HASLABEL*           VTT_PHASLABEL;

typedef short                   VTT_LINE;
typedef VTT_LINE*               VTT_PLINE;

typedef short                   VTT_COLUMN;
typedef VTT_COLUMN*             VTT_PCOLUMN;

typedef char*                   VTT_SCREENBUFFER;
typedef VTT_SCREENBUFFER*       VTT_PSCREENBUFFER;

typedef short                   VTT_SCREENINDEX;
typedef VTT_SCREENINDEX*        VTT_PSCREENINDEX;

typedef VTT_ATTRIBUTES          VTT_ATTRIBUTEBUFFER;
typedef VTT_ATTRIBUTEBUFFER*    VTT_PATTRIBUTEBUFFER;

typedef BOOLEAN                 VTT_SUPPORTGRAPHIC;
typedef VTT_SUPPORTGRAPHIC*     VTT_PSUPPORTGRAPHIC;

typedef BOOLEAN                 VTT_SUPPORTMARK;
typedef VTT_SUPPORTMARK*        VTT_PSUPPORTMARK;

typedef BOOLEAN                 VTT_SUPPORTWINDOWS;
typedef VTT_SUPPORTWINDOWS*     VTT_PSUPPORTWINDOWS;

typedef char                    VTT_SCREENCHAR;
typedef VTT_SCREENCHAR*         VTT_PSCREENCHAR;

typedef unsigned char           VTT_KEY;
typedef VTT_KEY*                VTT_PKEY;

typedef INT4                    VTT_TERMREF;
typedef VTT_TERMREF*            VTT_PTERMREF;

typedef C80C                    VTT_MESSAGE;
typedef VTT_MESSAGE*            VTT_PMESSAGE;

typedef char*                   VTT_SOFTKEYTEXT;
typedef VTT_SOFTKEYTEXT*        VTT_PSOFTKEYTEXT;

typedef CHAR                    VTT_SYSTEMLINETEXT[ sizeof ( tsp00_Line ) + 1 ];
typedef VTT_SYSTEMLINETEXT*     VTT_PSYSTEMLINETEXT;

typedef VTT_ATTRIBUTES          VTT_SYSTEMLINEATTR;
typedef VTT_SYSTEMLINEATTR*     VTT_PSYSTEMLINEATTR;

typedef VT_USAGE_MODE           VTT_USAGE_MODE;
typedef VTT_USAGE_MODE*         VTT_PUSAGE_MODE;

typedef VT_LABELC               VTT_INSERTLABELTEXT;
typedef VTT_INSERTLABELTEXT*    VTT_PINSERTLABELTEXT;

typedef short                   VTT_INSERTLABELPOS [ 2 ];
typedef VTT_INSERTLABELPOS*     VTT_PINSERTLABELPOS;

typedef VTT_ATTRIBUTE           VTT_INSERTLABELATTR;
typedef VTT_INSERTLABELATTR*    VTT_PINSERTLABELATTR;

typedef char*                   VTT_OPTIONS;
typedef VTT_OPTIONS*            VTT_POPTIONS;

typedef char*                   VTT_REJECTKEYS;
typedef VTT_REJECTKEYS*         VTT_PREJECTKEYS;

typedef short                   VTT_MARK;
typedef VTT_MARK*               VTT_PMARK;

typedef BOOLEAN                 VTT_SCREENCHANGED;
typedef VTT_SCREENCHANGED*      VTT_PSCREENCHANGED;

typedef BOOLEAN                 VTT_REFRESH;
typedef VTT_REFRESH*            VTT_PREFRESH;

typedef short                   VTT_TABLEINDEX;
typedef VTT_TABLEINDEX*         VTT_PTABLEINDEX;

typedef short                   VTT_FIELDLENGTH;
typedef VTT_FIELDLENGTH*        VTT_PFIELDLENGTH;

typedef short                   VTT_FIELDINDEX;
typedef VTT_FIELDINDEX*         VTT_PFIELDINDEX;

typedef char                    VTT_LABEL [ VT_LABEL_SIZE + 1 ];
typedef VTT_LABEL*              VTT_PLABEL;

typedef short                   VTT_LABELINDEX;
typedef VTT_LABELINDEX*         VTT_PLABELINDEX;

typedef BYTE                    VTT_DISPLAYCELL [ VTD_CELL_SIZE ];
typedef VTT_DISPLAYCELL*        VTT_PDISPLAYCELL;



// pascal - c interface types

typedef VTT_RETURNKEYS          PASCAL_RETURNKEYS;
typedef VTT_PRETURNKEYSSIZE     PASCAL_RETURNKEYSIZE;
typedef VTT_ATTRIBUTES          PASCAL_ATTRIBUTES;
typedef VTT_PATTRIBUTESSIZE     PASCAL_ATTRIBUTESSIZE;
typedef VTT_COLORS              PASCAL_COLORS;
typedef VTT_COLOR               PASCAL_COLOR;
typedef VTT_PCOLORSIZE          PASCAL_COLORSIZE;
typedef VTT_PHASSYSTEMLINE      PASCAL_HASSYSTEMLINE;
typedef VTT_PHASLABEL           PASCAL_HASLABEL;
typedef VTT_PLINE               PASCAL_LINES;
typedef VTT_PCOLUMN             PASCAL_COLUMNS;
typedef VTT_PSCREENBUFFER       PASCAL_SCREENBUFFER;
typedef VTT_PATTRIBUTEBUFFER    PASCAL_ATTRIBUTEBUFFER;
typedef VTT_PSUPPORTGRAPHIC     PASCAL_SUPPORTGRAPHIC;
typedef VTT_PSUPPORTMARK        PASCAL_SUPPORTMARK;
typedef VTT_PSUPPORTWINDOWS     PASCAL_SUPPORTWINDOWS;
typedef VTT_TERMREF             PASCAL_TERMREF;
typedef VTT_SOFTKEYTEXT         PASCAL_SOFTKEYTEXT;
typedef VTT_SYSTEMLINEATTR      PASCAL_SYSTEMLINEATTR;
typedef VTT_INSERTLABELPOS      PASCAL_INSERTLABLEPOS;
typedef VTT_PINSERTLABELATTR    PASCAL_INSERTLABELATTR;
typedef VTT_OPTIONS             PASCAL_OPTIONS;
typedef VTT_REJECTKEYS          PASCAL_REJECTKEYS;
typedef VTT_PMARK               PASCAL_MARK;
typedef VTT_PSCREENCHANGED      PASCAL_SCREENCHANGED;
typedef VTT_REFRESH             PASCAL_REFRESH;
typedef VTT_TABLEINDEX          PASCAL_TABLEINDEX;

typedef tsp00_Line              PASCAL_SYSTEMLINETEXT;
typedef VT_LABEL                PASCAL_INSERTLABELTEXT;
typedef char*                   PASCAL_LABEL;
typedef BYTE                    PASCAL_ATTRIBUTE;
typedef C80                     PASCAL_MESSAGE;
typedef PBOOLEAN                PASCAL_RETURNCODE;

typedef struct {
    UCHAR           ucVTKey;       // Virtual key
    UCHAR           ucCh;          // Character
    INT4            MouseX;
    INT4            MouseY;
}  PASCAL_KEYSTROKE;


// io options structure

typedef struct {
    BOOL            bWaitForInput;
    BOOL            bReturnOnLast;
    BOOL            bReturnOnFirst;
    BOOL            bIsBell;
    BOOL            bMark;
    BOOL            bWaitForChar;
    VTT_USAGE_MODE  eUsageMode;
} VTT_OPTIONS_REC;

typedef VTT_OPTIONS_REC*        VTT_POPTIONS_REC;


// keystroke structure
typedef struct {
    VTT_SCREENCHAR  ucCharacter;    // Character
    VTT_KEY         ucVKey;         // Virtual key
    ULONG           ulMouseX;
    ULONG           ulMouseY;
} VTT_KEYSTROKE_REC;

typedef VTT_KEYSTROKE_REC*      VTT_PKEYSTROKE_REC;


typedef struct {
    KBDKEYINFO      rcKeyboardKeyInfo;
    MOUEVENTINFO    rcMouseEventInfo;
} VTT_MOUSE_KEYBORD_REC;

typedef VTT_MOUSE_KEYBORD_REC*  VTT_PMOUSE_KEYBORD_REC;


// screen data communication structure

typedef struct {
    VTT_SCREENBUFFER    psScreenBuffer;     // Bildschirm Puffer
    VTT_ATTRIBUTEBUFFER psAttributeBuffer;  // Attribut Puffer
    VTT_LINE            kCursorLine;        // Zeile des Cursors
    VTT_COLUMN          kCursorColumn ;     // Spalte des Cursors
    VTT_SOFTKEYTEXT     psSkText;           // Softkey Text ( output )
    VTT_SYSTEMLINETEXT  psSysText;          // Text f�r die Statuszeile
    VTT_SYSTEMLINEATTR  psSysAttr;          // Attr f�r die Statuszeile
    VTT_INSERTLABELTEXT pszInsertText;      // Text f�r das INSERT-L.
    VTT_LINE            kInsertLine;        // Zeile des INSERT-L.
    VTT_COLUMN          kInsertColumn;      // Spalte des INSERT-L.
    VTT_INSERTLABELATTR cInsertAttr;        // Attribut f�r INSERT-L.
    VTT_OPTIONS_REC     rcOptions;          // Optionen
    VTT_RETURNKEYS      psReturnKeys;       // Returnkeys
    VTT_REJECTKEYS      psRejectKeys;       // Rejectkeys
    VTT_LINE            kNumberOfLines;     // Anzahl von Zeilen
    VTT_COLUMN          kNumberOfColumns;   // Anzahl von Spalten
    VTT_MARK            kMarkBegin;         // Markierungsanfang
    VTT_MARK            kMarkEnd;           // Markierungsende
    VTT_SCREENCHANGED   bScreenChanged;     // Bildschirm�nderung
    VTT_KEYSTROKE_REC   rcKeystroke;        // Ausl�setaste
    VTT_REFRESH         bRefresh;           // Refresh vor Ausgabe
} VTT_TERMDATA_REC;

typedef VTT_TERMDATA_REC*       VTT_PTERMDATA_REC;


// field description -- used with VTermQueryField (), VTermQueryNextField ()

typedef struct {
    VTT_LINE        kDelimiterStartLine;   // row of start delimiter
    VTT_COLUMN      kDelimiterStartColumn; // column of start delimiter
    VTT_LINE        kDelimiterEndLine;     // row of end delimiter
    VTT_COLUMN      kDelimiterEndColumn;   // row of end delimiter
    VTT_LINE        kFirstUsableLine;      // row of first usable position
    VTT_COLUMN      kFirstUsableColumn;    // column of first usable position
    VTT_SCREENINDEX kIndexToScreenBuffer;  // index into screen buffers
    VTT_FIELDLENGTH kLengthOfField;        // length of field
    VTT_FIELDINDEX  kCursorPosition;       // position of cursor -- or -1
} VTT_TERMFIELD_REC;

typedef VTT_TERMFIELD_REC*      VTT_PTERMFIELD_REC;


// attribute/color table

typedef struct {
    VTT_ATTRIBUTE   fAttribute;
    VTT_COLOR       fForegroundColor;
    VTT_COLOR       fBackgroundColor;
} VTT_ATTR_COLOR_TABLE;

typedef VTT_ATTR_COLOR_TABLE*   VTT_PATTR_COLOR_TABLE;


//
// EXPORTED VARIABLES
//
extern BOOLEAN  bRuntimeComponent;
extern PBOOLEAN pbGlobalCancelAddress;


//
// LOCAL VARIABLES
//

//static KBDKEYINFO            rcDbgKbdKeyInfo;// only while debugging
static ULONG                   ulCodePages[2] = { 437, 850 };
#if 0
static BOOL                    bEnhancedKbd;   // enhanced keyboard or AT-style
static BOOL                    bBlinkSupport;  // blinking supported
#endif

static USHORT                  ukColorStatus;  // store in sqlcton to set back in
                                               // sqltoff
static BOOL                    bMonochrome;    // monochrome screen?
static BOOL                    bInsert;        // insert mode?
static BOOL                    bCursorHidden;  // is cursor hidden?


static KBDINFO                 OldKeyStat;     // old keyboard status

#if defined(_WIN32)
 static KBDINFO                KeyStat;        // keyboard status
#endif

static VIOCURSORINFO           rcVideoCursorInfoSave,
                               rcInitialCursorInfo;
static VTT_MOUSE_KEYBORD_REC   MouKeybEvent;

static VTT_ATTR_COLOR_TABLE    arcAttrColorTable [ VTD_MAX_ATTR_COLOR_ENTRIES ];
static PCHAR                   pcLineBuffer;

static VTT_LINE                kMxRows          = VTD_ROWS_DEFAULT;
static VTT_COLUMN              kMxColumns       = VTD_COLUMNS_DEFAULT;
static BOOL                    fVtIfInitialized = FALSE;
static ULONG                   ulDBTerm         = DBTERM_ANSI;

static VTT_LINE                _kMxRows         = -1;
static VTT_COLUMN              _kMxColumns      = -1;

#if !defined(_WIN32)
 static TID                    TidKey           = 0;
 static TID                    TidMou           = 0;
 static HMOU                   MouHandle        = 0;
 static HEV                    hevKeybEvent;
 static HEV                    hevMouEvent;
 static HEV                    hevTrigKeybEvent;
 static HEV                    hevTrigMouEvent;
 static HMUX                   hmuxKeybMouEvent;
 HANDLE                        hThrdMou;
 HANDLE                        hThrdKey;
 NOPTRRECT                     PtrArea;
#endif

//
// LOCAL FUNCTION PROTOTYPES
//

//=============================================================================
//==========     VIRTUAL TERMINAL SUBSYSTEM INTERFACE PROTOTYPES     ==========
//=============================================================================

static BOOL VtIfStartup         ( VOID );
static BOOL VtIfShutdown        ( VOID );
static BOOL VtIfRepaint         ( VTT_PTERMDATA_REC );
static BOOL VtIfKeyLoop         ( VTT_PTERMDATA_REC );
static BOOL VtIfKbdMapKey       ( VTT_PMOUSE_KEYBORD_REC, VTT_PKEYSTROKE_REC,
                                  VTT_PKEY );
static BOOL VtIfMouMapKey       ( VTT_PMOUSE_KEYBORD_REC, VTT_PKEYSTROKE_REC,
                                  VTT_PKEY );


//=============================================================================
//=============     VIRTUAL TERMINAL VIO INTERFACE PROTOTYPES     =============
//=============================================================================

static BOOL VTermProcessKey     ( HVPS, VTT_PTERMDATA_REC, VTT_KEY );
static BOOL VTermCursorUp       ( HVPS, VTT_PTERMDATA_REC );
static BOOL VTermCursorDown     ( HVPS, VTT_PTERMDATA_REC );
static BOOL VTermCursorLeft     ( HVPS, VTT_PTERMDATA_REC );
static BOOL VTermSubCursorLeft  ( HVPS, VTT_PTERMDATA_REC,
                                  VTT_PTERMFIELD_REC, VTT_FIELDLENGTH );
static BOOL VTermCursorRight    ( HVPS, VTT_PTERMDATA_REC );
static BOOL VTermSubCursorRight ( HVPS, VTT_PTERMDATA_REC,
                                  VTT_PTERMFIELD_REC, VTT_FIELDLENGTH );
static BOOL VTermNextField      ( HVPS, VTT_PTERMDATA_REC );
static BOOL VTermPrevField      ( HVPS, VTT_PTERMDATA_REC, BOOL );
static BOOL VTermFirstField     ( HVPS, VTT_PTERMDATA_REC );
static BOOL VTermLastField      ( HVPS, VTT_PTERMDATA_REC );
static BOOL VTermDeleteChar     ( HVPS, VTT_PTERMDATA_REC );
static BOOL VTermBackSpace      ( HVPS, VTT_PTERMDATA_REC );
static BOOL VTermDeleteEof      ( HVPS, VTT_PTERMDATA_REC );
static BOOL VTermToggleInsert   ( HVPS, VTT_PTERMDATA_REC );
static BOOL VTermDoChar         ( HVPS, VTT_PTERMDATA_REC, CHAR );
static VOID VTermBeep           ( VOID );

static BOOL VTermPaintAVioLines ( HVPS, VTT_PTERMDATA_REC, VTT_LINE, VTT_LINE);
static VOID VTermMapVioCell     ( VTT_AC_INDEX, VTT_PATTRIBUTE, VTT_PSCREENCHAR );
static CHAR VTermMapChar        ( VTT_SCREENCHAR );
static VOID VTermMarkField      ( VTT_PTERMDATA_REC, VTT_PTERMFIELD_REC );
static BOOL VTermQueryField     ( VTT_PTERMDATA_REC, VTT_LINE, VTT_COLUMN,
                                  VTT_PTERMFIELD_REC, VTT_PTERMFIELD_REC );
static BOOL VTermQueryNextField ( VTT_PTERMDATA_REC, VTT_PTERMFIELD_REC );
static VOID VTermSetCursor      ( HVPS, VTT_PTERMDATA_REC, VTT_LINE,
                                  VTT_COLUMN, VTT_PTERMFIELD_REC,
                                  VTT_PTERMFIELD_REC );
static VOID VTermHideCursor     ( HVPS, VTT_PTERMDATA_REC, VTT_PTERMFIELD_REC);
static VOID VTermShowCursor     ( HVPS, VTT_PTERMDATA_REC, VTT_PTERMFIELD_REC);
static VOID VTermDrawCursor     ( HVPS, VTT_PTERMDATA_REC, VTT_PTERMFIELD_REC);
static BOOL VTermToggleCursor   ( BOOL, PVIOCURSORINFO );


// other local function prototypes
#if 0
static BOOL IsEnhancedKbd       (   VOID    );
#endif

#if defined ( SOFTKEY_SYSLINE )
static BOOL GetLabel            (   VTT_LABEL, PASCAL_LABEL,
                                    VTT_LABELINDEX    );
#endif

static BOOL GetPresentationSpaceSizes
                                (   PSHORT, PSHORT    );
static BOOL IsMonochrome        (   VOID    );
static VOID GetOptions          (   VTT_POPTIONS_REC, PASCAL_OPTIONS    );
static VOID GetKeystroke        (   VTT_PKEYSTROKE_REC, PASCAL_KEYSTROKE*   );
static VOID PutKeystroke        (   PASCAL_KEYSTROKE*, VTT_KEYSTROKE_REC    );
static BOOL IsReturnkey         (   VTT_PTERMDATA_REC, VTT_KEY    );
static BOOL IsRejectkey         (   VTT_REJECTKEYS, VTT_KEY    );
static LONG GetKeyboardStatus   (   PKBDINFO    );
static LONG SetKeyboardStatus   (   PKBDINFO    );
static LONG TriggerNextEvent    (   ULONG   );
static LONG GetEvent            (   VTT_PMOUSE_KEYBORD_REC*, PULONG   );

#if !defined(_WIN32)
 static VOID _System KeybThread  (   VTT_PMOUSE_KEYBORD_REC    );
 static VOID _System MouThread   (   VTT_PMOUSE_KEYBORD_REC    );
 static ULONG GetDblClckSpeed    (   VOID    );
#endif

//
// ========================== GLOBAL FUNCTIONS ================================
//


// sqltermvar returns the terminal-type (TERM or DBTERM) to the caller.
// If nothing is set, a blank-string is returned in term_type


VOID    sqltermvar  (   char*   psTerminalType      )
{
    #undef  MF__
    #define MF__ MOD__"sqltermvar"


    DBGPAS;

    memset ( psTerminalType, ' ', sizeof ( tsp00_C18 ));
}



/*------------------------------*/

// initialize the virtual terminal


VOID    sqlcton ( register  PASCAL_RETURNKEYS       psReturnKeys,
                            PASCAL_RETURNKEYSIZE    pkReturnKeysSize,
                  register  PASCAL_ATTRIBUTES       psAttributes,
                            PASCAL_ATTRIBUTESSIZE   pkAttributesSize,
                  register  PASCAL_COLORS           psColors,
                            PASCAL_COLORSIZE        pkColorSize,
                            PASCAL_HASSYSTEMLINE    pbHasSysLine,
                            PASCAL_HASLABEL         pbLabel,
                            PASCAL_LINES            pkMaxLines,
                            PASCAL_COLUMNS          pkMaxCols,
                            PASCAL_SCREENBUFFER     ppsScreenBuffer,
                            PASCAL_ATTRIBUTEBUFFER  ppsAttributeBuffer,
                            PASCAL_SUPPORTGRAPHIC   pbGraphic,
                            PASCAL_SUPPORTMARK      pbMark,
                            PASCAL_SUPPORTWINDOWS   pbWindows,
                            PASCAL_RETURNCODE       pbOk                )
{
    #undef  MF__
    #define MF__ MOD__"sqlcton"


    APIRET     rc ;

    #if !defined(_WIN32)
     SEMRECORD SemRec[2];
     USHORT    usEventMask;
    #endif


    DBGIN;

    *pbOk               = FALSE;
    *ppsScreenBuffer    = NULL;
    *ppsAttributeBuffer = NULL;
    pcLineBuffer        = NULL;


    if ( !VtIfStartup ()) {
        DBGOUT;
        return;
    }


    if (( *pkColorSize      != VT_LAST_COLOR  ) ||
        ( *pkReturnKeysSize != VT_LAST_KEY    ) ||
        ( *pkAttributesSize != VT_LAST_MODE   )) {
        DBG1  (( MF__, "INTERNAL ERROR: Wrong map size" ));
        MSGCD  (( ERR_VT_WRONG_MAP_SIZE ));

        DBGOUT;

        return;
      }

    memset ( psReturnKeys,  0, VT_LAST_KEY);
    memset ( psAttributes,  0, VT_LAST_MODE);
    memset ( psColors,      0, VT_LAST_COLOR);

    // set attributes
    psAttributes [ VT_INVERSE   ] = TRUE;
    psAttributes [ VT_INVISIBLE ] = TRUE;
    psAttributes [ VT_BRIGHT    ] = TRUE;
    psAttributes [ VT_MIXED     ] = TRUE;

//    if ( bBlinkSupport )
        psAttributes [ VT_BLINK ] = TRUE;

    // set colors
    psColors [ VT_DEFAULT_COLOR  ] = TRUE;
    psColors [ VT_BLACK          ] = TRUE;
    psColors [ VT_WHITE          ] = TRUE;

    if ( !bMonochrome ) {
        psColors [ VT_RED        ] = TRUE;
        psColors [ VT_GREEN      ] = TRUE;
        psColors [ VT_YELLOW     ] = TRUE;
        psColors [ VT_BLUE       ] = TRUE;
        psColors [ VT_LIGHT_BLUE ] = TRUE;
        psColors [ VT_PINK       ] = TRUE;
    }


    // set returnkeys
    psReturnKeys [ VT_CHARACTER_KEY   ] = TRUE;
    psReturnKeys [ VT_PF01            ] = TRUE;
    psReturnKeys [ VT_PF02            ] = TRUE;
    psReturnKeys [ VT_PF03            ] = TRUE;
    psReturnKeys [ VT_PF04            ] = TRUE;
    psReturnKeys [ VT_PF05            ] = TRUE;
    psReturnKeys [ VT_PF06            ] = TRUE;
    psReturnKeys [ VT_PF07            ] = TRUE;
    psReturnKeys [ VT_PF08            ] = TRUE;
    psReturnKeys [ VT_PF09            ] = TRUE;
    psReturnKeys [ VT_PF10            ] = TRUE;
    psReturnKeys [ VT_PF11            ] = TRUE;
    psReturnKeys [ VT_PF12            ] = TRUE;
    psReturnKeys [ VT_UP_KEY          ] = TRUE;
    psReturnKeys [ VT_DOWN_KEY        ] = TRUE;
    psReturnKeys [ VT_CLEAR           ] = TRUE;
    psReturnKeys [ VT_ENTER           ] = TRUE;
    psReturnKeys [ VT_CANCEL_KEY      ] = FALSE;

    psReturnKeys [ VT_CURSOR_UP       ] = TRUE;
    psReturnKeys [ VT_CURSOR_DOWN     ] = TRUE;
    psReturnKeys [ VT_CURSOR_RIGHT    ] = TRUE;
    psReturnKeys [ VT_CURSOR_LEFT     ] = TRUE;
    psReturnKeys [ VT_CURSOR_HOME     ] = TRUE;
    psReturnKeys [ VT_CURSOR_END      ] = TRUE;
    psReturnKeys [ VT_NEXT_FIELD      ] = TRUE;
    psReturnKeys [ VT_PREV_FIELD      ] = TRUE;
    psReturnKeys [ VT_INSERT_CHAR     ] = TRUE;
    psReturnKeys [ VT_DELETE_CHAR     ] = TRUE;
    psReturnKeys [ VT_RUBOUT          ] = TRUE;

    psReturnKeys [ VT_INSERT_KEY      ] = TRUE;
    psReturnKeys [ VT_DELETE_KEY      ] = TRUE;
    psReturnKeys [ VT_TOP_KEY         ] = TRUE;
    psReturnKeys [ VT_BOTTOM_KEY      ] = TRUE;
    psReturnKeys [ VT_DEL_EOF         ] = TRUE;
    psReturnKeys [ VT_CMD_KEY         ] = TRUE;
    psReturnKeys [ VT_CONTROL_KEY     ] = TRUE;
    psReturnKeys [ VT_CURSOR_EOL      ] = TRUE;
    psReturnKeys [ VT_CURSOR_BOL      ] = TRUE;

    psReturnKeys [ VT_MOUSE_DOWN      ] = TRUE;
    psReturnKeys [ VT_MOUSE_UP        ] = TRUE;
    psReturnKeys [ VT_MOUSE_MOVE      ] = TRUE;
    psReturnKeys [ VT_MOUSE_DBL       ] = TRUE;


    #if defined ( SOFTKEY_SYSLINE )
     *pbHasSysLine = ( BOOLEAN ) ( VTD_SYSLINE != -1 );
     *pbLabel      = ( BOOLEAN ) ( VTD_SKLINE == -1 ) ? NO_SK_LABELS :
                                                        SHORT_SK_LABELS;
    #else
     *pbHasSysLine = FALSE;
     *pbLabel      = FALSE;
    #endif

    *pkMaxLines   = kMxRows;
    *pkMaxCols    = kMxColumns;

    *pbGraphic    = TRUE;
    *pbMark       = TRUE;
    *pbWindows    = TRUE;

    //
    // --- allocate memmory
    //
    rc = ALLOC_MEM (( PPVOID ) ppsScreenBuffer,
                    ( ULONG ) ( kMxRows * kMxColumns ));

    if ( rc == NO_ERROR )
        rc = ALLOC_MEM (( PPVOID ) ppsAttributeBuffer,
                        ( ULONG ) ( kMxRows * kMxColumns ));

    if ( rc == NO_ERROR )
        rc = ALLOC_MEM ((PPVOID)&pcLineBuffer,
                        kMxColumns * VTD_CELL_SIZE * kMxRows );

    if ( rc ) {
        if ( *ppsScreenBuffer )
            FREE_MEM ( *ppsScreenBuffer );

        if ( *ppsAttributeBuffer )
            FREE_MEM ( *ppsAttributeBuffer );

        *ppsScreenBuffer    = NULL;
        *ppsAttributeBuffer = NULL;
        pcLineBuffer        = NULL;

        VtIfShutdown ();

        DBGOUT;
        return;
    }


    #if !defined(_WIN32)
     //
     // --- create threads
     //
     rc = CREATE_THREAD( &TidKey, &hThrdKey, KeybThread,
                         (ULONG)&MouKeybEvent,
                         CREATE_THRD_SUSPENDED, 8192 );


     if ( rc == NO_ERROR )
         rc = CREATE_THREAD( &TidMou, &hThrdMou, MouThread,
                             (ULONG)&MouKeybEvent,
                             CREATE_THRD_SUSPENDED, 8192 );

     if ( rc ) {
         MSGCD  (( ERR_VT_CREATE_THREAD ));
         DBG1  (( MF__, "Could not create thread, rc = %ul", rc ));

         FREE_MEM ( *ppsAttributeBuffer );
         *ppsAttributeBuffer = NULL;

         FREE_MEM ( *ppsScreenBuffer );
         FREE_MEM ( pcLineBuffer );
         *ppsScreenBuffer = NULL;
         pcLineBuffer     = NULL;

         VtIfShutdown ();

         DBGOUT;
         return;
     }


     rc = MouOpen ( 0L, &MouHandle );

     if ( rc == NO_ERROR )  {
         // --- report button 1 press/release events, without mouse motion
         usEventMask = 0x4;
         rc = MouSetEventMask( &usEventMask, MouHandle );

         if ( rc == NO_ERROR )  {
             rc = MouDrawPtr ( MouHandle );
         }
     }

     if ( rc ) {
         MSGCD  (( ERR_VT_OPEN_MOUSE ));
         DBG1  (( MF__, "Could not open mouse, rc = %ul", rc ));

         FREE_MEM ( *ppsAttributeBuffer );
         *ppsAttributeBuffer = NULL;

         FREE_MEM ( *ppsScreenBuffer );
         FREE_MEM ( pcLineBuffer );
         *ppsScreenBuffer = NULL;
         pcLineBuffer     = NULL;

         VtIfShutdown ();

         DBGOUT;
         return;
     }

     //
     // --- create event semaphores
     //
     rc = sql41c_create_event_sem ( &hevKeybEvent, NULL, "KeybEvent",
                                    0, FALSE, NULL );

     if ( rc == NO_ERROR )
       rc = sql41c_create_event_sem ( &hevMouEvent, NULL, "MouEvent",
                                      0, FALSE, NULL );

     if ( rc == NO_ERROR )
         rc = sql41c_create_event_sem ( &hevTrigKeybEvent, NULL, "TrigKeyb",
                                        0, FALSE, NULL );

     if ( rc == NO_ERROR )
         rc = sql41c_create_event_sem ( &hevTrigMouEvent, NULL, "TrigMou",
                                        0, FALSE, NULL );

     if ( rc ) {
         FREE_MEM ( *ppsAttributeBuffer );
         *ppsAttributeBuffer = NULL;

         FREE_MEM ( *ppsScreenBuffer );
         FREE_MEM ( pcLineBuffer );
         *ppsScreenBuffer = NULL;
         pcLineBuffer     = NULL;

         VtIfShutdown ();

         DBGOUT;
         return;
     }

     SemRec[0].hsemCur = (HSEM)hevKeybEvent;
     SemRec[0].ulUser  = VTD_KEYB_EVENT;
     SemRec[1].hsemCur = (HSEM)hevMouEvent;
     SemRec[1].ulUser  = VTD_MOUSE_EVENT;

     rc = DosCreateMuxWaitSem((PSZ) NULL, &hmuxKeybMouEvent,
                              2, SemRec, DCMW_WAIT_ANY);

     if ( rc ) {
         MSGD (( ERR_CREATING_SEM, "Mutex", rc ));
         DBG1 ((MF__, "Error on creating semaphore 'Mutex', rc = %u", rc));

         FREE_MEM ( *ppsAttributeBuffer );
         *ppsAttributeBuffer = NULL;

         FREE_MEM ( *ppsScreenBuffer );
         FREE_MEM ( pcLineBuffer );
         *ppsScreenBuffer = NULL;
         pcLineBuffer     = NULL;

         VtIfShutdown ();

         DBGOUT;
         return;
     }

     //
     // --- resume thread
     //
     rc = RESUME_THREAD( TidKey, hThrdKey );

     if ( rc == NO_ERROR )
         rc = RESUME_THREAD( TidMou, hThrdMou );

     if ( rc ) {
         MSGCD  (( ERR_VT_RESUME_THREAD ));
         DBG1  (( MF__, "Could not resume thread, rc = %ul", rc ));

         FREE_MEM ( *ppsAttributeBuffer );
         *ppsAttributeBuffer = NULL;

         FREE_MEM ( *ppsScreenBuffer );
         FREE_MEM ( pcLineBuffer );
         *ppsScreenBuffer = NULL;
         pcLineBuffer     = NULL;

         VtIfShutdown ();

         DBGOUT;
         return;
     }

    #endif

    *pbOk = TRUE;

    DBGOUT;
}


//-----------------------------------------------------------------------------


// shut off the virtual terminal

VOID    sqltoff (   PASCAL_TERMREF          lTermRef,
                    PASCAL_SCREENBUFFER     ppsScreenBuffer,
                    PASCAL_ATTRIBUTEBUFFER  ppsAttributeBuffer,
                    PASCAL_MESSAGE          psMsg                   )
{
    #undef  MF__
    #define MF__ MOD__"sqltoff"

    C80C    szMsg;
    APIRET  rc;


    DBGIN;

    VtIfShutdown ();

    if ( *ppsScreenBuffer != NULL ) {
        rc = FREE_MEM ( *ppsScreenBuffer );
        *ppsScreenBuffer = NULL;

        if ( rc ) {
            DBG1  (( MF__, "Freeing of Screenbuffer failed" ));
            ABORT();
        }
    }

    if ( *ppsAttributeBuffer != NULL ) {
        rc = FREE_MEM ( *ppsAttributeBuffer );
        *ppsAttributeBuffer = NULL;

        if ( rc ) {
            DBG1  (( MF__, "Freeing of Attributebuffer failed" ));
            ABORT();
        }
    }

    if ( pcLineBuffer != NULL ) {
        rc = FREE_MEM ( pcLineBuffer );
        pcLineBuffer = NULL;

        if ( rc ) {
            DBG1  (( MF__, "Freeing of Linebuffer failed" ));
            ABORT();
        }
    }


    if ( psMsg && *psMsg != ' ' ) {
        eo46PtoC ( szMsg,  psMsg, sizeof ( C80 ));

        if ( *szMsg ) {
            sql60_enable_console ();
            MSGCD (( INFO_COMPONENT_MESSAGE, szMsg ));
            sql60_disable_console ();
        }
    }

    #if !defined(_WIN32)
     //
     // --- kill threads
     //
     rc = KILL_THREAD( TidKey, hThrdKey );

     if ( rc == NO_ERROR )
         rc = KILL_THREAD( TidMou, hThrdMou );

     if ( rc ) {
         MSGCD  (( ERR_VT_KILL_THREAD ));
         DBG1  (( MF__, "Could not kill thread, rc = %ul", rc ));
         ABORT();
     }

     //
     // --- close mouse
     //
     rc = MouClose( MouHandle );

#if 0
     if ( rc ) {
         MSGCD  (( ERR_VT_CLOSE_MOUSE, rc ));
         DBG1  (( MF__, "Could not close mouse, rc = %ul", rc ));
         ABORT();
         }
#endif

     //
     // --- close event semaphores
     //
     rc = sql41c_close_event_sem ( hevKeybEvent, "KeybEvent" );

     if ( rc )
       rc = sql41c_close_event_sem ( hevMouEvent, "MouEvent" );

     if ( rc )
         rc = sql41c_close_event_sem ( hevTrigKeybEvent, "TrigKeyb" );

     if ( rc )
         rc = sql41c_close_event_sem ( hevTrigMouEvent, "TrigMou" );


     if ( rc ) {
         DBG1  (( MF__, "closing mouse /keyboard semaphore" ));
         ABORT();
     }

     rc = DosCloseMuxWaitSem ( hmuxKeybMouEvent );

     if ( rc ) {
         MSGD (( ERR_CLOSING_SEM, "Mutex", rc));
         DBG1 ((MF__, "Error on closing semaphore: 'Mutex', rc = %u\n", rc));
         ABORT();
     }
    #endif

    DBGOUT;
}



/*------------------------------*/


// sqlctio zur Ausgabe eines kompletten Bildschirms und Einlesen von
// Zeichen vom Bildschirm.

void    sqlctio (

    PASCAL_SCREENBUFFER     ppsScreenBuffer,    // Bildschirm Puffer
    PASCAL_ATTRIBUTEBUFFER  ppsAttributeBuffer, // Attribut Puffer
    PASCAL_LINES            pkCursorLine,       // Zeile des Cursors
    PASCAL_COLUMNS          pkCursorColumn ,    // Spalte des Cursors
    PASCAL_SOFTKEYTEXT      psSkText,           // Softkey Text
    PASCAL_SYSTEMLINETEXT   psSysText,          // Text f�r die Statuszeile
    PASCAL_SYSTEMLINEATTR   psSysAttr,          // Attr f�r die Statuszeile
    PASCAL_INSERTLABELTEXT  pszInsertText,      // Text f�r das INSERT-L.
    PASCAL_INSERTLABLEPOS   akInsertPos,        // Position des INSERT-L.
    PASCAL_INSERTLABELATTR  pcInsertAttr,       // Attribute f�r INSERT-L.
    PASCAL_OPTIONS          psOptions,          // Optionen
    PASCAL_RETURNKEYS       psReturnKeys,       // Returnkeys
    PASCAL_REJECTKEYS       psRejectKeys,       // Rejectkeys
    PASCAL_LINES            pkNumberOfLines,    // Anzahl von Zeilen
    PASCAL_COLUMNS          pkNumberOfColumns,  // Anzahl von Spalten
    PASCAL_MARK             pkMarkBegin,        // Markierungsanfang
    PASCAL_MARK             pkMarkEnd,          // Markierungsende
    PASCAL_SCREENCHANGED    pbScreenChanged,    // Bildschirm�nderung
    PASCAL_KEYSTROKE*       psKeystroke,        // Ausl�setaste
    PASCAL_REFRESH          bRefresh,           // Refresh vor Ausgabe
    PASCAL_RETURNCODE       pbOk                )

{
    #undef  MF__
    #define MF__ MOD__"sqlctio"


    VTT_TERMDATA_REC   rcTermData;     // lokales terminal


    DBGIN;

    // aktuallisiere lokales terminal
    rcTermData.psScreenBuffer       = *ppsScreenBuffer;
    rcTermData.psAttributeBuffer    = *ppsAttributeBuffer;
    rcTermData.kCursorLine          = *pkCursorLine;
    rcTermData.kCursorColumn        = *pkCursorColumn;
    rcTermData.psSkText             = psSkText;

    rcTermData.psSysAttr            = psSysAttr;
    eo46PtoC ( rcTermData.psSysText,     psSysText,     sizeof ( tsp00_Line ));

    eo46PtoC ( rcTermData.pszInsertText, pszInsertText, sizeof ( VT_LABEL));

    rcTermData.kInsertLine          = akInsertPos [ 0 ];
    rcTermData.kInsertColumn        = akInsertPos [ 1 ];
    rcTermData.cInsertAttr          = *pcInsertAttr;

    GetOptions ( &( rcTermData.rcOptions ), psOptions );

    rcTermData.psReturnKeys         = psReturnKeys;
    rcTermData.psRejectKeys         = psRejectKeys;
    rcTermData.kNumberOfLines       = kMxRows;
    rcTermData.kNumberOfColumns     = kMxColumns;
    rcTermData.kMarkBegin           = *pkMarkBegin;
    rcTermData.kMarkEnd             = *pkMarkEnd;
    rcTermData.bScreenChanged       = FALSE;

    *pkNumberOfLines   = kMxRows;
    *pkNumberOfColumns = kMxColumns;
    *pbScreenChanged   = FALSE;

    GetKeystroke ( &( rcTermData.rcKeystroke ), psKeystroke );

    rcTermData.bRefresh = bRefresh;

    VtIfRepaint ( &rcTermData );    // IF bRefresh == TRUE THEN
                                    //      screen has not been changed

    if ( rcTermData.rcOptions.bWaitForInput ) {
        VtIfKeyLoop ( &rcTermData );

        *pkCursorLine       = rcTermData.kCursorLine;
        *pkCursorColumn     = rcTermData.kCursorColumn;
        akInsertPos [ 0 ]   = rcTermData.kInsertLine;
        akInsertPos [ 1 ]   = rcTermData.kInsertColumn;
        *pcInsertAttr       = rcTermData.cInsertAttr;
        *pkNumberOfLines    = rcTermData.kNumberOfLines;
        *pkNumberOfColumns  = rcTermData.kNumberOfColumns;
        *pkMarkBegin        = rcTermData.kMarkBegin;
        *pkMarkEnd          = rcTermData.kMarkEnd;
        *pbScreenChanged    = rcTermData.bScreenChanged;
        bRefresh            = rcTermData.bRefresh;

        PutKeystroke ( psKeystroke, rcTermData.rcKeystroke );
    }

    *pbOk = TRUE;
    DBGOUT;
}


//-----------------------------------------------------------------------------


// modify the attribute/color table

VOID    sqlcttable  (   PASCAL_TABLEINDEX   kTableIndex,
                        PASCAL_ATTRIBUTES   psAttributes,
                        PASCAL_COLOR        ucForegroundColor,
                        PASCAL_COLOR        ucBackgroundColor   )
{
    #undef  MF__
    #define MF__ MOD__"sqlcttable"


    DBGIN;

    INIT_AC_ATTR ( kTableIndex );

    if ( psAttributes [ VT_BRIGHT ] ) {
        SET_AC_ATTR ( kTableIndex, VTD_ATTR_BRIGHT );
    }

    if ( psAttributes [ VT_INVERSE ] ) {
        SET_AC_ATTR ( kTableIndex, VTD_ATTR_INVERSE );
    }

    if ( psAttributes [ VT_BLINK ] ) {
        SET_AC_ATTR ( kTableIndex, VTD_ATTR_BLINK );
    }

    if ( psAttributes [ VT_INVISIBLE ] ) {
        SET_AC_ATTR ( kTableIndex, VTD_ATTR_INVISIBLE );
    }

    SET_AC_FOREGROUND_COLOR ( kTableIndex, ucForegroundColor );
    SET_AC_BACKGROUND_COLOR ( kTableIndex, ucBackgroundColor );

    DBGOUT;
}

/*------------------------------*/

VOID sql04u_vt_crash_cleanup ( VOID )
{
    #undef  MF__
    #define MF__ MOD__"sql04u_vt_crash_cleanup"

    VtIfShutdown();

    return;
}


//
// ========================== LOCAL FUNCTIONS =================================
//


// start up the virtual terminal subsystem

static BOOL VtIfStartup (   VOID    )
{
    #undef  MF__
    #define MF__ MOD__"VtIfStartup"


    VIOCURSORINFO   rcVideoCursorInfo;
    VIOINTENSITY    rcVideoIntensity;
    USHORT          ukError;
    PSZ             pszDBTerm;
    ULONG           ulCodePage = ulCodePages[0];
    INT             i;
    #if !defined (_WIN32)
     ULONG          aulCpList[20] = {0};
     ULONG          ulBufSize     = sizeof(aulCpList);
     ULONG          ulListSize    = 0;
     APIRET         rc            = NO_ERROR;
    #endif


    DBGIN;

#if 0
    bEnhancedKbd    = IsEnhancedKbd ();
    bBlinkSupport   = FALSE;
#endif

    bMonochrome     = IsMonochrome  ();
    bInsert         = FALSE;
    bCursorHidden   = TRUE;

    if ( GetPresentationSpaceSizes ( &kMxRows, &kMxColumns ) == FALSE)
      {
      COORD dwSize;

      _kMxRows    = kMxRows;
      _kMxColumns = kMxColumns;

      dwSize.X = kMxColumns = VTD_COLUMNS_DEFAULT;
      dwSize.Y = kMxRows    = VTD_ROWS_DEFAULT;

      if ( !SetConsoleScreenBufferSize( GetStdHandle(STD_OUTPUT_HANDLE), dwSize ))
        {
        DBG1  (( MF__, ERRMSG_VT_WRONG_WINDOW_SIZE ));
        MSGCD (( ERR_VT_WRONG_WINDOW_SIZE  ));

        DBGOUT;
        return FALSE;
        }
      }

    // --- get keybord get
    if ( GetKeyboardStatus ( &OldKeyStat ) != NO_ERROR ) {
        DBGOUT;
        return FALSE;
     }

    // --- set keyboard mode
    #if defined(_WIN32)
     KeyStat         = OldKeyStat;
     KeyStat.fsMask &= ~( ENABLE_LINE_INPUT |
                          ENABLE_ECHO_INPUT |
                          ENABLE_WINDOW_INPUT );

     KeyStat.fsMask |= ENABLE_MOUSE_INPUT;

     if ( SetKeyboardStatus ( &KeyStat ) != NO_ERROR ) {
         DBGOUT;
         return FALSE;
     }
    #else
     PtrArea.row  = 0;
     PtrArea.col  = 0;
     PtrArea.cRow = kMxRows    - 1;
     PtrArea.cCol = kMxColumns - 1;
    #endif


    VioGetCurType ( &rcVideoCursorInfo, 0 );
    rcInitialCursorInfo      = rcVideoCursorInfo;

    rcVideoCursorInfo.yStart = (USHORT) VTD_NORMAL_CURSOR_START;
    rcVideoCursorInfo.cEnd   = (USHORT) VTD_NORMAL_CURSOR_END;
    rcVideoCursorInfoSave    = rcVideoCursorInfo;

    rcVideoCursorInfo.attr   = VTD_HIDE_HW_CURSOR;
    ukError = VioSetCurType ( &rcVideoCursorInfo, 0 );

    if ( ukError ) {
        DBG1  (( MF__, ERRMSG_VT_CANT_CHANGE_CURSOR ));
        MSGCD (( ERR_VT_CANT_CHANGE_CURSOR, ukError  ));

        DBGOUT;

        return FALSE;
    }

    rcVideoIntensity.cb    = sizeof ( VIOINTENSITY );
    rcVideoIntensity.type  = 2;

    ukError = VioGetState ( &rcVideoIntensity, 0 );

    if ( ukError == NO_ERROR ) {
        ukColorStatus       = rcVideoIntensity.fs;
        rcVideoIntensity.fs = VTD_VIDEO_INTENSITY;

        ukError = VioSetState ( &rcVideoIntensity, 0 );
    }

    #if defined (_WIN32)
     ulCodePage = (ULONG) GetConsoleOutputCP();
    #else
     rc = DosQueryCp( ulBufSize, aulCpList, &ulListSize );


     if (( rc == NO_ERROR ) && ( ulListSize > 0 )) {
         ulCodePage = aulCpList[0];
         }
    #endif


    for ( i = 0; i < (sizeof(ulCodePages) /  sizeof(ulCodePages[0])); i++ ) {
      if ( ulCodePage == ulCodePages[i] ) {
          ulDBTerm = DBTERM_ANSI_COL;
          break;
          }
      }

    ukError = (USHORT)GETENV ( DBTERM_ENV_VAR, &pszDBTerm );

    if (( ukError == NO_ERROR )  && ( *pszDBTerm )  &&
        ( ! strcmp ( "ANSI", strupr ( pszDBTerm )))) {
        ulDBTerm = DBTERM_ANSI;
        }
#if 0
    bBlinkSupport = ukError == NO_ERROR;
#endif

    fVtIfInitialized = TRUE;

    DBGOUT;

    return TRUE;
}



/*------------------------------*/


// shut down the virtual terminal subsystem

static BOOL VtIfShutdown    (   VOID    )
{
    #undef  MF__
    #define MF__ MOD__"VtIfShutdown"


    VIOINTENSITY    rcVideoIntensity;
	char			Buffer[2] = { ' ', '\x07' };	


    DBGIN;

    if ( fVtIfInitialized == TRUE ) {
        fVtIfInitialized = FALSE;
        rcVideoIntensity.cb   = sizeof ( VIOINTENSITY );
        rcVideoIntensity.type = 2;
        rcVideoIntensity.fs   = ukColorStatus;

        VioSetState     ( &rcVideoIntensity, 0 );
        VioSetCurPos    ( 0, 0, 0 );
        VioSetCurType   ( &rcInitialCursorInfo, 0 );

        #if !defined(_WIN32)
         MouRemovePtr( &PtrArea, MouHandle );
        #endif

        if ( !bRuntimeComponent ) {

	  	    Buffer[1] = (char)ukColorStatus; 
            VioScrollUp ( 0, 0, (USHORT)-1, (USHORT)-1, (USHORT)-1, Buffer, 0 );
        }

        #if defined(_WIN32)
         if ( SetKeyboardStatus ( &OldKeyStat ) != NO_ERROR ) {
             DBGOUT;
             return FALSE;
         }


         if ( _kMxRows != -1 )
           {
           COORD dwSize;

           dwSize.X = _kMxColumns;
           dwSize.Y = _kMxRows;

           if ( !SetConsoleScreenBufferSize( GetStdHandle(STD_OUTPUT_HANDLE), dwSize ))
             {
             DBGOUT;
             return FALSE;
             }
           }

        #endif
    }

    DBGOUT;

    return TRUE;
}



/*------------------------------*/


// repaint the virtual terminal screen

static BOOL VtIfRepaint (   VTT_PTERMDATA_REC   prcTermData     )
{
    #undef  MF__
    #define MF__ MOD__"VtIfRepaint"


    BOOL    bOk;


    DBGIN;

    bOk = VTermPaintAVioLines ( 0, prcTermData, 0, (VTT_LINE) (kMxRows - 1));

    DBGOUT;

    return bOk;
}



/*------------------------------*/


// process keystrokes until a termination key is hit

static BOOL VtIfKeyLoop (   VTT_PTERMDATA_REC        prcTermData    )
{
    #undef  MF__
    #define MF__ MOD__"VtIfKeyLoop"


    VTT_KEY                 kLocal;
    BOOL                    bTerm;
    APIRET                  rc;
    VTT_PMOUSE_KEYBORD_REC  pMouKeybEvent;
    ULONG                   ulEvent;


    DBGIN;


    // caller's coordinates are relative to one, ours are
    // relative to zero, so adjust now

    VTermSetCursor(0, prcTermData,
                   (VTT_LINE) (VTD_FIRST_LINE + prcTermData -> kCursorLine - 1),
                   (VTT_COLUMN) (prcTermData->kCursorColumn - 1), NULL, NULL );

    // process keystrokes until termination key hit
    bTerm = FALSE;
    while ( !bTerm ) {

        rc = GetEvent( &pMouKeybEvent, &ulEvent );

        if ( rc != NO_ERROR )  {
            DBGOUT;
            return FALSE;
        }

        switch ( ulEvent ) {
            case VTD_KEYB_EVENT:

                if ( VtIfKbdMapKey ( pMouKeybEvent,
                                     &( prcTermData -> rcKeystroke),
                                     &kLocal ))
                    bTerm = VTermProcessKey ( 0, prcTermData, kLocal );

                break;

            case VTD_MOUSE_EVENT:
                if ( VtIfMouMapKey ( pMouKeybEvent,
                                     &( prcTermData -> rcKeystroke),
                                     &kLocal ))
                    bTerm = VTermProcessKey ( 0, prcTermData, kLocal );

                break;

        }

        rc = TriggerNextEvent( ulEvent );

        if ( rc != NO_ERROR )  {
            DBGOUT;
            return FALSE;
        }
    }

    VTermHideCursor ( 0, prcTermData, NULL );

    // adjust cursor coordinates back to caller's frame of
    // reference (relative to one)

    prcTermData -> kCursorLine++;
    prcTermData -> kCursorColumn++;

    DBGOUT;

    return TRUE;
}


/*------------------------------*/


// map a KbdCharIn () character to a virtual terminal keystroke

static BOOL VtIfKbdMapKey   (   VTT_PMOUSE_KEYBORD_REC  pMouKeybEvent,
                                VTT_PKEYSTROKE_REC      prcKeystroke,
                                VTT_PKEY                pkLocal )
{
    #undef  MF__
    #define MF__ MOD__"VtIfKbdMapKey"
    KBDKEYINFO*     prcKeyboardKeyInfo;


    DBGIN;

    prcKeyboardKeyInfo = &pMouKeybEvent->rcKeyboardKeyInfo;

    prcKeystroke -> ucCharacter = VTD_KEYSTROKE_UNDEF;
    prcKeystroke -> ucVKey      = VTD_KEYSTROKE_UNDEF;
    *pkLocal                    = VTD_KEYSTROKE_UNDEF;

    if (( prcKeyboardKeyInfo -> chChar ) == 0 ) {   // not an actual char key
        switch ( prcKeyboardKeyInfo -> chScan ) {
            case VTD_KBD_F1:
                prcKeystroke -> ucVKey = VT_PF01;
            break;

            case VTD_KBD_F2:
                prcKeystroke -> ucVKey = VT_PF02;
            break;

            case VTD_KBD_F3:
                prcKeystroke -> ucVKey = VT_PF03;
            break;

            case VTD_KBD_F4:
                prcKeystroke -> ucVKey = VT_PF04;
            break;

            case VTD_KBD_F5:
                prcKeystroke -> ucVKey = VT_PF05;
            break;

            case VTD_KBD_F6:
                prcKeystroke -> ucVKey = VT_PF06;
            break;

            case VTD_KBD_F7:
                prcKeystroke -> ucVKey = VT_PF07;
            break;

            case VTD_KBD_F8:
                prcKeystroke -> ucVKey = VT_PF08;
            break;

            case VTD_KBD_F9:
                prcKeystroke -> ucVKey = VT_PF09;
            break;

            case VTD_KBD_F10:
                prcKeystroke -> ucVKey = VT_PF10;
            break;

            case VTD_KBD_F11:
                prcKeystroke -> ucVKey = VT_PF11;
            break;

            case VTD_KBD_F12:
                prcKeystroke -> ucVKey = VT_PF12;
            break;

            case VTD_KBD_PAGEUP:
                prcKeystroke -> ucVKey = VT_UP_KEY;
            break;

            case VTD_KBD_PAGEDOWN:
                prcKeystroke -> ucVKey = VT_DOWN_KEY;
            break;

            case VTD_KBD_UP:
                prcKeystroke -> ucVKey = VT_CURSOR_UP;
                *pkLocal  = VT_CURSOR_UP;
            break;

            case VTD_KBD_DOWN:
                prcKeystroke -> ucVKey = VT_CURSOR_DOWN;
                *pkLocal  = VT_CURSOR_DOWN;
            break;

            case VTD_KBD_LEFT:
                prcKeystroke -> ucVKey = VT_CURSOR_LEFT;
                *pkLocal  = VT_CURSOR_LEFT;
            break;

            case VTD_KBD_RIGHT:
                prcKeystroke -> ucVKey = VT_CURSOR_RIGHT;
                *pkLocal  = VT_CURSOR_RIGHT;
            break;

            case VTD_KBD_INSERT:
                prcKeystroke -> ucVKey = VT_INSERT_CHAR;
                *pkLocal  = VT_INSERT_CHAR;
            break;

            case VTD_KBD_DELETE:
                prcKeystroke -> ucVKey = VT_DELETE_CHAR;
                *pkLocal  = VT_DELETE_CHAR;
            break;

            case VTD_KBD_CTRLDELETE:
                prcKeystroke -> ucVKey = VT_DELETE_KEY;
                *pkLocal  = VT_DELETE_KEY;
            break;

            case VTD_KBD_CTRLINSERT:
                prcKeystroke -> ucVKey = VT_INSERT_KEY;
                *pkLocal  = VT_INSERT_KEY;
            break;

            case VTD_KBD_HOME:
                prcKeystroke -> ucVKey = VT_CURSOR_HOME;
                *pkLocal  = VT_CURSOR_HOME;
            break;

            case VTD_KBD_END:
                prcKeystroke -> ucVKey = VT_CURSOR_END;
                *pkLocal  = VT_CURSOR_END;
            break;

            case VTD_KBD_BACKTAB:
                prcKeystroke -> ucVKey = VT_PREV_FIELD;
                *pkLocal  = VT_PREV_FIELD;
            break;

            case VTD_KBD_CTRL_F1:
                prcKeystroke -> ucVKey = VT_CMD_KEY;
                *pkLocal  = VT_CMD_KEY;
            break;

            case VTD_KBD_CTRL_F3:
                prcKeystroke -> ucVKey = VT_DEL_EOF;
                *pkLocal  = VT_DEL_EOF;
            break;

            case VTD_KBD_CTRL_F9:
                prcKeystroke -> ucVKey = VT_PF11;
            break;

            case VTD_KBD_CTRL_F10:
                prcKeystroke -> ucVKey = VT_PF12;
            break;


            default:
                return FALSE;
        }
    }
    else if (( prcKeyboardKeyInfo -> chChar  == VTD_SCAN_CODE ) &&
             (( (UCHAR)prcKeyboardKeyInfo -> chScan < 0x30 ) ||
              ( (UCHAR)prcKeyboardKeyInfo -> chScan > 0x5A )))  {

        if (( prcKeyboardKeyInfo -> fsState ) & VTD_SHIFT_FLAGS ) {
            switch ( prcKeyboardKeyInfo -> chScan ) {
                case VTD_KBD_SHIFT_HOME:
                    prcKeystroke -> ucVKey = VT_CURSOR_BOL;
                    *pkLocal  = VT_CURSOR_BOL;
                break;

                case VTD_KBD_SHIFT_END:
                    prcKeystroke -> ucVKey = VT_CURSOR_EOL;
                    *pkLocal  = VT_CURSOR_EOL;
                break;

                default:
                    return FALSE;
            }
        }
        else {
            switch ( prcKeyboardKeyInfo -> chScan ) {
                case VTD_KBD_PAGEUP:
                    prcKeystroke -> ucVKey = VT_UP_KEY;
                break;

                case VTD_KBD_PAGEDOWN:
                    prcKeystroke -> ucVKey = VT_DOWN_KEY;
                break;

                case VTD_KBD_UP:
                    prcKeystroke -> ucVKey = VT_CURSOR_UP;
                    *pkLocal  = VT_CURSOR_UP;
                break;

                case VTD_KBD_DOWN:
                    prcKeystroke -> ucVKey = VT_CURSOR_DOWN;
                    *pkLocal  = VT_CURSOR_DOWN;
                break;

                case VTD_KBD_LEFT:
                    prcKeystroke -> ucVKey = VT_CURSOR_LEFT;
                    *pkLocal  = VT_CURSOR_LEFT;
                break;

                case VTD_KBD_RIGHT:
                    prcKeystroke -> ucVKey = VT_CURSOR_RIGHT;
                    *pkLocal  = VT_CURSOR_RIGHT;
                break;

                case VTD_KBD_INSERT:
                    prcKeystroke -> ucVKey = VT_INSERT_CHAR;
                    *pkLocal  = VT_INSERT_CHAR;
                break;

                case VTD_KBD_DELETE:
                    prcKeystroke -> ucVKey = VT_DELETE_CHAR;
                    *pkLocal  = VT_DELETE_CHAR;
                break;

                case VTD_KBD_CTRLDELETE:
                    prcKeystroke -> ucVKey = VT_DELETE_KEY;
                    *pkLocal  = VT_DELETE_KEY;
                break;

                case VTD_KBD_CTRLINSERT:
                    prcKeystroke -> ucVKey = VT_INSERT_KEY;
                    *pkLocal  = VT_INSERT_KEY;
                break;

                case VTD_KBD_HOME:
                    prcKeystroke -> ucVKey = VT_CURSOR_HOME;
                    *pkLocal  = VT_CURSOR_HOME;
                break;

                case VTD_KBD_END:
                    prcKeystroke -> ucVKey = VT_CURSOR_END;
                    *pkLocal  = VT_CURSOR_END;
                break;

                case VTD_KBD_CTRLHOME:
                    prcKeystroke -> ucVKey = VT_TOP_KEY;
                    *pkLocal  = VT_TOP_KEY;
                break;

                case VTD_KBD_CTRLEND:
                    prcKeystroke -> ucVKey = VT_BOTTOM_KEY;
                    *pkLocal  = VT_BOTTOM_KEY;
                break;

                default:
                    return FALSE;
            }
        }
    }
    else {
        switch ( prcKeyboardKeyInfo -> chChar ) {
            case '\r':
                prcKeystroke -> ucVKey = VT_ENTER;
                *pkLocal  = VT_ENTER;
            break;

            case '\b':
                prcKeystroke -> ucVKey = VT_RUBOUT;
                *pkLocal  = VT_RUBOUT;
            break;

            case '\t':
                prcKeystroke -> ucVKey = VT_NEXT_FIELD;
                *pkLocal  = VT_NEXT_FIELD;
            break;

            case VTD_CTRL_C:
                prcKeystroke -> ucVKey = VT_CANCEL_KEY;
                *pkLocal  = VT_CANCEL_KEY;
            break;

            case VTD_CTRL_F:
                prcKeystroke -> ucCharacter = prcKeyboardKeyInfo -> chChar + 'A' - 1;
                prcKeystroke -> ucVKey      = VT_CONTROL_KEY;
                *pkLocal                    = VT_CLEAR;
            break;

            case VTD_CTRL_D:
                prcKeystroke -> ucCharacter = prcKeyboardKeyInfo -> chChar + 'A' - 1;
                prcKeystroke -> ucVKey      = VT_CONTROL_KEY;
                *pkLocal                    = VT_ENTER;
            break;

            default:
                if(( prcKeyboardKeyInfo -> chChar >= VTD_CTRL_A ) &&
                   ( prcKeyboardKeyInfo -> chChar <= VTD_CTRL_Z )) {
                    prcKeystroke -> ucCharacter = prcKeyboardKeyInfo -> chChar + 'A' - 1;
                    prcKeystroke -> ucVKey      = VT_CONTROL_KEY;
                    *pkLocal                    = VT_CONTROL_KEY;
                }
                else {
                    if ( (prcKeyboardKeyInfo -> chChar < ' ')  ||
                         (prcKeyboardKeyInfo -> chChar > 255) )
                        return FALSE;
                    else {
                        prcKeystroke -> ucCharacter = prcKeyboardKeyInfo -> chChar;
                        prcKeystroke -> ucVKey      = VT_CHARACTER_KEY;

                        *pkLocal  = VT_CHARACTER_KEY;
                    }
                }
            break;
        }
    }

    DBGOUT;

    return TRUE;
}

/*------------------------------*/

static BOOL VtIfMouMapKey   (   VTT_PMOUSE_KEYBORD_REC  pMouKeybEvent,
                                VTT_PKEYSTROKE_REC      prcKeystroke,
                                VTT_PKEY                pkLocal )
{
    #undef  MF__
    #define MF__ MOD__"VtIfMouMapKey"


    DBGIN;
    prcKeystroke -> ucCharacter = VTD_KEYSTROKE_UNDEF;
    prcKeystroke -> ucVKey      = VTD_KEYSTROKE_UNDEF;
    *pkLocal                    = VTD_KEYSTROKE_UNDEF;

    switch ( pMouKeybEvent->rcMouseEventInfo.fs ) {
        case VTD_MOUSE_DOWN:
            prcKeystroke -> ucVKey   = VT_MOUSE_DOWN;
            prcKeystroke -> ulMouseX = pMouKeybEvent->rcMouseEventInfo.col + 1;
            prcKeystroke -> ulMouseY = pMouKeybEvent->rcMouseEventInfo.row + 1;
            break;
        case VTD_MOUSE_UP  :
            prcKeystroke -> ucVKey   = VT_MOUSE_UP;
            prcKeystroke -> ulMouseX = pMouKeybEvent->rcMouseEventInfo.col + 1;
            prcKeystroke -> ulMouseY = pMouKeybEvent->rcMouseEventInfo.row + 1;
            break;
        case VTD_MOUSE_DBL :
            prcKeystroke -> ucVKey   = VT_MOUSE_DBL;
            prcKeystroke -> ulMouseX = pMouKeybEvent->rcMouseEventInfo.col + 1;
            prcKeystroke -> ulMouseY = pMouKeybEvent->rcMouseEventInfo.row + 1;
            break;
    }

    DBGOUT;

    return TRUE;
}


// ============================================================================


// paint a section of the screen using AVIO

static BOOL VTermPaintAVioLines (   HVPS                hVPS,
                                    VTT_PTERMDATA_REC   prcTermData,
                                    VTT_LINE            kBegin,
                                    VTT_LINE            kEnd            )
{
    #undef  MF__
    #define MF__ MOD__"VTermPaintAVioLines"


    PCHAR               pcBuf         = NULL;
    PCHAR               pcLine        = NULL;
    USHORT              usLen;
    VTT_LINE            kLine;
    VTT_COLUMN          kColumn;
    VTT_SCREENINDEX     kIndex;
    VTT_ATTRIBUTE       fInsertAttr;
//  BOOL                bScrLines;
    #if defined ( SOFTKEY_SYSLINE )
     VTT_LABEL          acLabel;
     VTT_LABELINDEX     kLabel;
     VTT_ATTRIBUTE      fSysAttr;
     BOOL               bSysLine, bSkLine;
    #endif


    DBGIN;

    #if !defined(_WIN32)
     MouRemovePtr( &PtrArea, MouHandle );
    #endif

    kBegin = ( kBegin < VTD_FIRST_LINE ) ? VTD_FIRST_LINE : kBegin;
    kEnd   = ( kEnd   > kMxRows - 1  ) ? kMxRows - 1  : kEnd;

//  if ( kEnd > kMxRows - 1 )
//     kEnd = kMxRows - 1;
//
//
//  bScrLines = ( kBegin <= kMxRows && kEnd >= VTD_FIRST_LINE );
//
//  if ( !bScrLines ) {
//      kBegin = ( kBegin < VTD_FIRST_LINE ) ? VTD_FIRST_LINE : kBegin;
//      kEnd   = ( kEnd   > kMxRows  ) ? kMxRows  : kEnd;
//  }

    if ( bInsert ) {
        VTermMapVioCell  ( LOW_BYTE ( prcTermData -> cInsertAttr ),
                           &fInsertAttr, NULL );
    }

    #if defined ( SOFTKEY_SYSLINE )
     bSysLine  = ( kBegin <= VTD_SYSLINE   && kEnd >= VTD_SYSLINE    );
     bSkLine   = ( kBegin <= VTD_SKLINE    && kEnd >= VTD_SKLINE     );

     if (( bSysLine ) || ( bSkLine )) {
         VTermMapVioCell  ( LOW_BYTE ( prcTermData -> psSysAttr [ 0 ] ),
                            &fSysAttr, NULL );
     }

     if ( bSysLine ) {     // do system line
         VioWrtCharStrAtt ( prcTermData -> psSysText, kMxColumns,
                            (USHORT)VTD_SYSLINE, 0, &fSysAttr, hVPS );
     }

     if ( bSkLine ) {  // do softkey line, only short softkeys are supported
         for ( kLabel = 0; kLabel < VT_SK_NUMBER; kLabel++ ) {
             GetLabel ( acLabel, prcTermData -> psSkText, kLabel );
             VioWrtCharStrAtt ( acLabel, (USHORT)VT_LABEL_SIZE,
                                (USHORT)VTD_SKLINE,
                                (USHORT)(kLabel * ( VT_LABEL_SIZE + 1 )),
                                &fSysAttr, hVPS );
         }
     }
    #endif

    for ( kLine = kBegin, pcLine  = pcLineBuffer;
          kLine <= kEnd;
          kLine++, pcLine += kMxColumns * VTD_CELL_SIZE ) {
        kIndex  = (kLine - VTD_FIRST_LINE) * kMxColumns;

        for ( kColumn = 0, pcBuf = pcLine;
              kColumn < kMxColumns;
              kColumn++, kIndex++, pcBuf += VTD_CELL_SIZE ) {

             #if defined(_WIN32)
              ((CHAR_INFO*)pcBuf)->Char.AsciiChar =
                  VTermMapChar(prcTermData->psScreenBuffer[kIndex]);

              ((CHAR_INFO*)pcBuf)->Attributes = 0;

              VTermMapVioCell(
                      LOW_BYTE(prcTermData->psAttributeBuffer[kIndex]),
                      (VTT_PATTRIBUTE)&((CHAR_INFO*)pcBuf)->Attributes,
                      (VTT_PSCREENCHAR)&((CHAR_INFO*)pcBuf)->Char.AsciiChar );
             #else
              pcBuf [ 0 ] = VTermMapChar ( prcTermData ->
                                           psScreenBuffer [ kIndex ] );
              VTermMapVioCell ( LOW_BYTE ( prcTermData ->
                                           psAttributeBuffer [ kIndex ] ),
                                &pcBuf[ 1 ], & pcBuf [ 0 ] );
             #endif
        }



        if ( bInsert &&  kLine == ( prcTermData -> kInsertLine - 1 )) {

            pcBuf = pcLine + (prcTermData->kInsertColumn - 1) * VTD_CELL_SIZE;
            usLen = (USHORT)strlen ( prcTermData->pszInsertText );

            for ( kColumn = 0;
                  kColumn < usLen;
                  kColumn++, pcBuf += VTD_CELL_SIZE ) {
                pcBuf[ 1 ] = (UCHAR)fInsertAttr;
                pcBuf[ 0 ] = prcTermData->pszInsertText[kColumn];
            }
            pcBuf[ 1 ] = (UCHAR)fInsertAttr;
            pcBuf[ 0 ] = ' ';
        }

    }

    if ( kBegin <= kEnd ) {
        VioWrtCellStr ( pcLineBuffer,
                        (USHORT)(kMxColumns * VTD_CELL_SIZE * (kEnd-kBegin+1)),
                        kBegin, 0, hVPS );
    }

    #if !defined(_WIN32)
     MouDrawPtr ( MouHandle );
    #endif

    DBGOUT;

    return TRUE;
}



/*------------------------------*/


// set the cursor position

static VOID VTermSetCursor  (   HVPS                hVPS,
                                VTT_PTERMDATA_REC   prcTermData,
                                VTT_LINE            kLine,
                                VTT_COLUMN          kColumn,
                                VTT_PTERMFIELD_REC  prcTermFieldFrom,
                                VTT_PTERMFIELD_REC  prcTermFieldTo      )
{
    #undef  MF__
    #define MF__ MOD__"VTermSetCursor"


    DBGIN;

    VTermHideCursor ( hVPS, prcTermData, prcTermFieldFrom  );

    prcTermData -> kCursorLine    = kLine;
    prcTermData -> kCursorColumn  = kColumn;

    VTermShowCursor ( hVPS, prcTermData, prcTermFieldTo );

    DBGOUT;
}



/*------------------------------*/


// hide the cursor

static VOID VTermHideCursor (   HVPS                hVPS,
                                VTT_PTERMDATA_REC   prcTermData,
                                VTT_PTERMFIELD_REC  prcTermField    )
{
    #undef  MF__
    #define MF__ MOD__"VTermHideCursor"


    VIOCURSORINFO   rcCursor;


    DBGIN;

    if ( !bCursorHidden ) {
        VioGetCurType ( &rcCursor, 0 );
        rcCursor.attr = VTD_HIDE_HW_CURSOR;         // hide hardware cursor
        VioSetCurType ( &rcCursor, 0 );
        VTermDrawCursor ( hVPS, prcTermData, prcTermField );
        bCursorHidden = TRUE;
    }

    DBGOUT;
}


/*------------------------------*/


// show the cursor

static VOID VTermShowCursor (   HVPS                hVPS,
                                VTT_PTERMDATA_REC   prcTermData,
                                VTT_PTERMFIELD_REC  prcTermField    )
{
    #undef  MF__
    #define MF__ MOD__"VTermShowCursor"

    DBGIN;

    if ( bCursorHidden ) {
        if (( prcTermData -> kCursorLine   >= 0 )      &&
            ( prcTermData -> kCursorLine   < kMxRows ) &&
            ( prcTermData -> kCursorColumn >= 0 )      &&
            ( prcTermData -> kCursorColumn < kMxColumns )) {
            VioSetCurType ( &rcVideoCursorInfoSave, 0 );
            VTermDrawCursor ( hVPS, prcTermData, prcTermField );
            bCursorHidden = FALSE;
        }  else {
        prcTermData -> kCursorLine   = -1;
        prcTermData -> kCursorColumn = -1;
        }
    }

    DBGOUT;
}



/*------------------------------*/


// toggles the cursor from normal to insert and insert to normal type

static BOOL VTermToggleCursor   (   BOOL            bInsert,
                                    PVIOCURSORINFO  prcActualCursor )
{
    #undef  MF__
    #define MF__ MOD__"VTermToggleCursor"


    APIRET16    rc;


    DBGIN;

    if ( bInsert ) {
        prcActualCursor -> yStart = (USHORT) VTD_INSERT_CURSOR_START;
        prcActualCursor -> cEnd   = (USHORT) VTD_INSERT_CURSOR_END;

        rc = VioSetCurType ( prcActualCursor, 0 );
        if ( rc ) {
            DBG1  (( MF__, ERRMSG_VT_CANT_CHANGE_CURSOR ));
            MSGCD (( ERR_VT_CANT_CHANGE_CURSOR, rc       ));

            DBGOUT;

            return FALSE;
        }
    }
    else {
        prcActualCursor -> yStart = (USHORT) VTD_NORMAL_CURSOR_START;
        prcActualCursor -> cEnd   = (USHORT) VTD_NORMAL_CURSOR_END;

        rc = VioSetCurType ( prcActualCursor, 0 );
        if ( rc ) {
            DBG1  (( MF__, ERRMSG_VT_CANT_CHANGE_CURSOR ));
            MSGCD (( ERR_VT_CANT_CHANGE_CURSOR, rc       ));

            DBGOUT;

            return FALSE;
        }
    }

    DBGOUT;

    return TRUE;
}



/*------------------------------*/


// invert the cursor position

static VOID VTermDrawCursor (   HVPS                hVPS,
                                VTT_PTERMDATA_REC   prcTermData,
                                VTT_PTERMFIELD_REC  prcTermField    )
{
    #undef  MF__
    #define MF__ MOD__"VTermDrawCursor"


    VTT_LINE            kLine;
    VTT_COLUMN          kColumn;
    VTT_TERMFIELD_REC   rcTermField;


    DBGIN;

    kLine   = prcTermData -> kCursorLine;
    kColumn = prcTermData -> kCursorColumn;

    if (prcTermField == NULL)
        if ( VTermQueryField ( prcTermData, kLine, kColumn,
                              &rcTermField, NULL ))
            prcTermField = &rcTermField;

    VioSetCurPos ( kLine, kColumn, hVPS );

    DBGOUT;
}



/*------------------------------*/


// process a keystroke -- returns TRUE if termination key

static BOOL VTermProcessKey (   HVPS                hVPS,
                                VTT_PTERMDATA_REC   prcTermData,
                                VTT_KEY             kLocal          )
{
    #undef  MF__
    #define MF__ MOD__"VTermProcessKey"


    BOOL    bTerm;


    DBGIN;

    bTerm = FALSE;

    if ( prcTermData -> rcKeystroke.ucVKey != VTD_KEYSTROKE_UNDEF ) {

        if ( IsReturnkey ( prcTermData, prcTermData -> rcKeystroke.ucVKey ))
            bTerm = TRUE;
        else if ( IsRejectkey ( prcTermData -> psRejectKeys,
                                           prcTermData -> rcKeystroke.ucVKey ))
            bTerm = FALSE;
        else if ( kLocal != VTD_KEYSTROKE_UNDEF ) {
            if ( IsReturnkey ( prcTermData, kLocal )) {
                prcTermData -> rcKeystroke.ucVKey = ( unsigned char ) kLocal;
                bTerm = TRUE;
            }
            else if ( IsRejectkey ( prcTermData -> psRejectKeys, kLocal ))
                bTerm = FALSE;
            else {
                switch ( kLocal ) {
                    case VT_CHARACTER_KEY:
                        bTerm = VTermDoChar ( hVPS, prcTermData,
                                      prcTermData -> rcKeystroke.ucCharacter );
                    break;

                    case VT_CURSOR_UP:
                        bTerm = VTermCursorUp ( hVPS, prcTermData );
                    break;

                    case VT_CURSOR_DOWN:
                        bTerm = VTermCursorDown ( hVPS, prcTermData );
                    break;

                    case VT_CURSOR_RIGHT:
                        bTerm = VTermCursorRight ( hVPS, prcTermData );
                    break;

                    case VT_CURSOR_LEFT:
                        bTerm = VTermCursorLeft ( hVPS, prcTermData );
                    break;

                    case VT_CURSOR_HOME:
                        bTerm = VTermFirstField ( hVPS, prcTermData );
                    break;

                    case VT_CURSOR_END:
                        bTerm = VTermLastField ( hVPS, prcTermData );
                    break;

                    case VT_NEXT_FIELD:
                    case VT_ENTER:
                        bTerm = VTermNextField ( hVPS, prcTermData );
                    break;

                    case VT_PREV_FIELD:
                        bTerm = VTermPrevField ( hVPS, prcTermData, FALSE);
                    break;

                    case VT_INSERT_CHAR:
                        bTerm = VTermToggleInsert ( hVPS, prcTermData );
                    break;

                    case VT_DELETE_CHAR:
                        bTerm = VTermDeleteChar ( hVPS, prcTermData );
                    break;

                    case VT_RUBOUT:
                        bTerm = VTermBackSpace ( hVPS, prcTermData );
                    break;

                    case VT_DEL_EOF:
                        bTerm = VTermDeleteEof ( hVPS, prcTermData );
                    break;

                    case VT_CANCEL_KEY:
                        raise( SIGINT );
                    break;

                    default:
                        bTerm = FALSE;
                    break;
                }
            }
        }
    }

    DBGOUT;

    return bTerm;
}



/*------------------------------*/


// map a Virtual Terminal color byte to an AVIO color byte

static VOID VTermMapVioCell (   VTT_AC_INDEX    fIndexToAttrColorTable,
                                VTT_PATTRIBUTE  pfCellAttribute,
                                VTT_PSCREENCHAR pcScreenChar )
{
    #undef  MF__
    #define MF__ MOD__"VTermMapVioCell"


    DBGIN;

    // Attribute/Color?
    if ( !IS_ANY_AC_ATTR_SET ( fIndexToAttrColorTable ))    // no attribute set
        // then set white on black (normal attribute)
        *pfCellAttribute = VTD_ATTR_NORMAL;
    else {                                                  // attribute set
        *pfCellAttribute = 0;

        // get and set color attributes
        switch ( GET_AC_FOREGROUND_COLOR ( fIndexToAttrColorTable )) {
            case VT_DEFAULT_COLOR:
                *pfCellAttribute = VTD_ATTR_NORMAL;
            break;

            case VT_WHITE:
                SET_CA_FOREGROUND_ATTR ( *pfCellAttribute, VTD_ATTR_WHITE );
            break;

            case VT_BLACK:
                SET_CA_FOREGROUND_ATTR ( *pfCellAttribute, VTD_ATTR_BLACK );
            break;

            case VT_RED:
                SET_CA_FOREGROUND_ATTR ( *pfCellAttribute, VTD_ATTR_RED );
            break;

            case VT_GREEN:
                SET_CA_FOREGROUND_ATTR ( *pfCellAttribute, VTD_ATTR_GREEN );
            break;

            case VT_YELLOW:
                SET_CA_FOREGROUND_ATTR ( *pfCellAttribute, VTD_ATTR_YELLOW );
            break;

            case VT_BLUE:
                SET_CA_FOREGROUND_ATTR ( *pfCellAttribute, VTD_ATTR_BLUE );
            break;

            case VT_PINK:
                SET_CA_FOREGROUND_ATTR ( *pfCellAttribute, VTD_ATTR_PINK );
            break;

            case VT_LIGHT_BLUE:
                SET_CA_FOREGROUND_ATTR ( *pfCellAttribute, VTD_ATTR_LIGHT_BLUE );
            break;

            default:
                *pfCellAttribute = VTD_ATTR_NORMAL;
            break;
        }

        switch ( GET_AC_BACKGROUND_COLOR ( fIndexToAttrColorTable )) {
            case VT_DEFAULT_COLOR:
                *pfCellAttribute = VTD_ATTR_NORMAL;
            break;

            case VT_WHITE:
                SET_CA_BACKGROUND_ATTR ( *pfCellAttribute, VTD_ATTR_WHITE );
            break;

            case VT_BLACK:
                SET_CA_BACKGROUND_ATTR ( *pfCellAttribute, VTD_ATTR_BLACK );
            break;

            case VT_RED:
                SET_CA_BACKGROUND_ATTR ( *pfCellAttribute, VTD_ATTR_RED );
            break;

            case VT_GREEN:
                SET_CA_BACKGROUND_ATTR ( *pfCellAttribute, VTD_ATTR_GREEN );
            break;

            case VT_YELLOW:
                SET_CA_BACKGROUND_ATTR ( *pfCellAttribute, VTD_ATTR_YELLOW );
            break;

            case VT_BLUE:
                SET_CA_BACKGROUND_ATTR ( *pfCellAttribute, VTD_ATTR_BLUE );
            break;

            case VT_PINK:
                SET_CA_BACKGROUND_ATTR ( *pfCellAttribute, VTD_ATTR_PINK );
            break;

            case VT_LIGHT_BLUE:
                SET_CA_BACKGROUND_ATTR ( *pfCellAttribute,
                                          VTD_ATTR_LIGHT_BLUE );
            break;

            default:
                *pfCellAttribute = VTD_ATTR_NORMAL;
            break;
        }

        // set other attributes

        if ( IS_AC_ATTR ( fIndexToAttrColorTable, VTD_ATTR_INVERSE )) {
            SET_CA_INVERSE_ATTR ( *pfCellAttribute );
        }

        if ( IS_AC_ATTR ( fIndexToAttrColorTable, VTD_ATTR_INVISIBLE )) {
            if ( pcScreenChar != NULL) {
                *pcScreenChar = ' ';
            }
            else {
                SET_CA_INVISIBLE_ATTR ( *pfCellAttribute );
            }
        }

        if ( IS_AC_ATTR ( fIndexToAttrColorTable, VTD_ATTR_BRIGHT )) {
            SET_CA_BRIGHT_ATTR ( *pfCellAttribute );
        }

        if ( IS_AC_ATTR ( fIndexToAttrColorTable, VTD_ATTR_BLINK )) {
            SET_CA_BLINK_ATTR ( *pfCellAttribute );
        }
    }

    DBGOUT;
}



/*------------------------------*/


// map a virtual terminal character, handle graphics and field delimiters

static CHAR VTermMapChar    (   VTT_SCREENCHAR  cScreenChar     )
{
    #undef  MF__
    #define MF__ MOD__"VTermMapChar"


    DBGIN;

    switch ( cScreenChar ) {
        case '\0':
        case '\1':
        case '\2':
            DBGOUT;

            return (' ');

        case VTG_VERTICAL_BAR:
            DBGOUT;

            if ( ulDBTerm == DBTERM_ANSI )
                return (( CHAR ) VTD_GC_ANSI_VERTICAL_BAR );
            else
                return (( CHAR ) VTD_GC_ANSI_COL_VERTICAL_BAR );

        case VTG_HORIZONTAL_BAR:
            DBGOUT;

            if ( ulDBTerm == DBTERM_ANSI )
                return (( CHAR ) VTD_GC_ANSI_HORIZONTAL_BAR );
            else
                return (( CHAR ) VTD_GC_ANSI_COL_HORIZONTAL_BAR );

        case VTG_L_RIGHT_CORNER:
            DBGOUT;

            if ( ulDBTerm == DBTERM_ANSI )
                return (( CHAR ) VTD_GC_ANSI_L_RIGHT_CORNER );
            else
                return (( CHAR ) VTD_GC_ANSI_COL_L_RIGHT_CORNER );

        case VTG_U_RIGHT_CORNER:
            DBGOUT;

            if ( ulDBTerm == DBTERM_ANSI )
                return (( CHAR ) VTD_GC_ANSI_U_RIGHT_CORNER );
            else
                return (( CHAR ) VTD_GC_ANSI_COL_U_RIGHT_CORNER );

        case VTG_L_LEFT_CORNER:
            DBGOUT;

            if ( ulDBTerm == DBTERM_ANSI )
                return (( CHAR ) VTD_GC_ANSI_L_LEFT_CORNER );
            else
                return (( CHAR ) VTD_GC_ANSI_COL_L_LEFT_CORNER );

        case VTG_U_LEFT_CORNER:
            DBGOUT;

            if ( ulDBTerm == DBTERM_ANSI )
                return (( CHAR ) VTD_GC_ANSI_U_LEFT_CORNER );
            else
                return (( CHAR ) VTD_GC_ANSI_COL_U_LEFT_CORNER );

        case VTG_LEFT_T:
            DBGOUT;

            if ( ulDBTerm == DBTERM_ANSI )
                return (( CHAR ) VTD_GC_ANSI_LEFT_T );
            else
                return (( CHAR ) VTD_GC_ANSI_COL_LEFT_T );

        case VTG_RIGHT_T:
            DBGOUT;

            if ( ulDBTerm == DBTERM_ANSI )
                return (( CHAR ) VTD_GC_ANSI_RIGHT_T );
            else
                return (( CHAR ) VTD_GC_ANSI_COL_RIGHT_T );

        case VTG_BOTTOM_T:
            DBGOUT;

            if ( ulDBTerm == DBTERM_ANSI )
                return (( CHAR ) VTD_GC_ANSI_BOTTOM_T );
            else
                return (( CHAR ) VTD_GC_ANSI_COL_BOTTOM_T );

        case VTG_TOP_T:
            DBGOUT;

            if ( ulDBTerm == DBTERM_ANSI )
                return (( CHAR ) VTD_GC_ANSI_TOP_T );
            else
                return (( CHAR ) VTD_GC_ANSI_COL_TOP_T );

        case VTG_CROSSING_LINES:
            DBGOUT;

            if ( ulDBTerm == DBTERM_ANSI )
                return (( CHAR ) VTD_GC_ANSI_CROSSING_LINES );
            else
                return (( CHAR ) VTD_GC_ANSI_COL_CROSSING_LINES );

    }

    /*NOTREACHED*/

    DBGOUT;
    return ( cScreenChar );
}



/*------------------------------*/


// mark a field as changed

static VOID VTermMarkField  (   VTT_PTERMDATA_REC   prcTermData,
                                VTT_PTERMFIELD_REC  prcTermField    )
{
    #undef  MF__
    #define MF__ MOD__"VTermMarkField"


    VTT_FIELDLENGTH kIndex;
    VTT_SCREENINDEX kIndexToScreen;


    DBGIN;

    kIndexToScreen = ( prcTermField -> kFirstUsableLine - VTD_FIRST_LINE ) *
                     kMxColumns + prcTermField -> kFirstUsableColumn;

    for ( kIndex = 0; kIndex < prcTermField -> kLengthOfField;
                                                   kIndex++, kIndexToScreen++ )
        SET_ATTR ( prcTermData ->
                   psAttributeBuffer [ kIndexToScreen ], VTD_RTE_ATTR_CHANGED);

    prcTermData -> bScreenChanged = TRUE;

    DBGOUT;
}



/*------------------------------*/


// find out about the field the cursor is on

static BOOL VTermQueryField (   VTT_PTERMDATA_REC   prcTermData,
                                VTT_LINE            kLine,
                                VTT_COLUMN          kColumn,
                                VTT_PTERMFIELD_REC  prcTermField,
                                VTT_PTERMFIELD_REC  prcTermFieldPrevious    )
{
    #undef  MF__
    #define MF__ MOD__"VTermQueryField"


    VTT_TERMFIELD_REC   rcTermFieldTemp;


    DBGIN;

    if ( prcTermFieldPrevious ) {
        prcTermFieldPrevious -> kFirstUsableLine     = VTD_TERMFIELD_UNDEF;
        prcTermFieldPrevious -> kFirstUsableColumn   = VTD_TERMFIELD_UNDEF;
        prcTermFieldPrevious -> kIndexToScreenBuffer = VTD_TERMFIELD_UNDEF;
        prcTermFieldPrevious -> kLengthOfField       = VTD_TERMFIELD_UNDEF;
        prcTermFieldPrevious -> kCursorPosition      = VTD_TERMFIELD_UNDEF;
    }

    rcTermFieldTemp.kFirstUsableLine   = VTD_FIRST_LINE;
    rcTermFieldTemp.kFirstUsableColumn = VTD_FIRST_COLUMN;

    while ( VTermQueryNextField ( prcTermData, &rcTermFieldTemp )) {
        if ( rcTermFieldTemp.kFirstUsableLine == kLine  &&
                    rcTermFieldTemp.kFirstUsableColumn <= kColumn  &&
                        kColumn <  rcTermFieldTemp.kFirstUsableColumn +
                                             rcTermFieldTemp.kLengthOfField ) {
            *prcTermField = rcTermFieldTemp;
            prcTermField -> kCursorPosition =
                                  kColumn - rcTermFieldTemp.kFirstUsableColumn;

            DBGOUT;

            return TRUE;
        }
        else if ( rcTermFieldTemp.kFirstUsableLine   <  kLine   ||
                ( rcTermFieldTemp.kFirstUsableLine   == kLine   &&
                  rcTermFieldTemp.kFirstUsableColumn <  kColumn )) {

            if ( prcTermFieldPrevious )
                *prcTermFieldPrevious = rcTermFieldTemp;

            if ( ++rcTermFieldTemp.kFirstUsableColumn >= kMxColumns ) {
                rcTermFieldTemp.kFirstUsableLine++;
                rcTermFieldTemp.kFirstUsableColumn = VTD_FIRST_COLUMN;
            }
            // run next loop
        }
        else
            break;
    }

    DBGOUT;

    return FALSE;
}



/*------------------------------*/


// find the next unprotected field

static BOOL VTermQueryNextField (   VTT_PTERMDATA_REC   prcTermData,
                                    VTT_PTERMFIELD_REC  prcTermField    )
{
    #undef  MF__
    #define MF__ MOD__"VTermQueryNextField"


    VTT_LINE        kFirstUsableLine, kDelimiterLine, kStartLine;
    VTT_COLUMN      kFirstUsableColumn, kDelimiterColumn, kStartColumn;
    VTT_SCREENINDEX kIndexToScreenBuffer, kStartIndex;
    VTT_FIELDLENGTH kLength;


    DBGIN;

    kFirstUsableLine     = prcTermField -> kFirstUsableLine;
    kFirstUsableColumn   = prcTermField -> kFirstUsableColumn;
    kIndexToScreenBuffer = ( kFirstUsableLine - VTD_FIRST_LINE) * kMxColumns +
                           kFirstUsableColumn;

    DBG3 (( MF__, "kFirstUsableLine:     %d", kFirstUsableLine     ));
    DBG3 (( MF__, "kFirstUsableColumn:   %d", kFirstUsableColumn   ));
    DBG3 (( MF__, "kIndexToScreenBuffer: %d", kIndexToScreenBuffer ));


    while ( kFirstUsableLine < kMxRows ) {
        while ( kFirstUsableColumn < kMxColumns ) {
            if ( prcTermData -> psScreenBuffer [ kIndexToScreenBuffer ] ==
                                                  VTD_FIELD_DELIMITER_SOCF  ||
                  prcTermData -> psScreenBuffer [ kIndexToScreenBuffer] ==
                                                  VTD_FIELD_DELIMITER_SONCF ) {

                kDelimiterLine   = kFirstUsableLine;
                kDelimiterColumn = kFirstUsableColumn;

                DBG3 (( MF__, "Field delimiter: \"%s\"",
                      ( prcTermData ->
                        psScreenBuffer [ kIndexToScreenBuffer ] ==
                        VTD_FIELD_DELIMITER_SOCF ) ? "SOCF" : "SONCF" ));

                DBG3 (( MF__, "kDelimiterStartLine: %d", kDelimiterLine   ));
                DBG3 (( MF__, "kDelimiterColumn:    %d", kDelimiterColumn ));

                if ( ++kFirstUsableColumn >= kMxColumns ) {
                    if ( ++kFirstUsableLine >= kMxRows ) {
                        DBG3 (( MF__, "Field possible out of screen!?" ));
                        break;
                    }

                    kFirstUsableColumn = VTD_FIRST_COLUMN;
                }

                kStartLine   = kFirstUsableLine;
                kStartColumn = kFirstUsableColumn;

                DBG3 (( MF__, "kFieldStartLine:   %d", kStartLine   ));
                DBG3 (( MF__, "kFieldStartColumn: %d", kStartColumn ));

                kStartIndex  = ++kIndexToScreenBuffer;

                DBG3 (( MF__, "kFieldStartScreenIndex: %d",
                        kIndexToScreenBuffer ));

                kLength = 0;

                while ( prcTermData ->
                        psScreenBuffer [ kIndexToScreenBuffer] !=
                                                   VTD_FIELD_DELIMITER_SONCF &&
                        prcTermData ->
                        psScreenBuffer [ kIndexToScreenBuffer] !=
                                                   VTD_FIELD_DELIMITER_SOCF  &&
                        prcTermData ->
                        psScreenBuffer [ kIndexToScreenBuffer] !=
                                                   VTD_FIELD_DELIMITER_EOF ) {

                    kIndexToScreenBuffer++;

                    if ( ++kFirstUsableColumn >= kMxColumns ) {
                        if ( ++kFirstUsableLine >= kMxRows ) {
                            DBG3 (( MF__,
                                    "Field end possible out of screen!?" ));
                            break;
                        }

                        kFirstUsableColumn = VTD_FIRST_COLUMN;
                    }

                    kLength++;
                }

                DBG3 (( MF__, "Field length: %d", kLength ));
                DBG3 (( MF__, "kDelimiterEndLine:   %d",
                        kFirstUsableLine   ));
                DBG3 (( MF__, "kDelimiterEndColumn: %d",
                        kFirstUsableColumn ));


                if ( IS_ATTR ( prcTermData ->
                               psAttributeBuffer [ kStartIndex ],
                               VTD_RTE_ATTR_INPUT )) {

                    DBG3 (( MF__, "Field is an input field" ));

                    prcTermField -> kDelimiterStartLine   = kDelimiterLine;
                    prcTermField -> kDelimiterStartColumn = kDelimiterColumn;
                    prcTermField -> kDelimiterEndLine     = kFirstUsableLine;
                    prcTermField -> kDelimiterEndColumn   = kFirstUsableColumn;
                    prcTermField -> kFirstUsableLine      = kStartLine;
                    prcTermField -> kFirstUsableColumn    = kStartColumn;
                    prcTermField -> kIndexToScreenBuffer  = kStartIndex;
                    prcTermField -> kLengthOfField        = kLength;

                    DBGOUT;

                    return TRUE;
                }
            }
            else {
                DBG3 (( MF__, "No field delimiter" ));

                kFirstUsableColumn++;
                kIndexToScreenBuffer++;
            }
        }

        kFirstUsableLine++;
        kFirstUsableColumn = VTD_FIRST_COLUMN;
    }

    DBGOUT;

    DBG3 (( MF__, "The cursor point is not an input field!" ));

    return FALSE;
}



/*------------------------------*/


// move the cursor up

static BOOL VTermCursorUp   (   HVPS                hVPS,
                                VTT_PTERMDATA_REC   prcTermData     )
{
    #undef  MF__
    #define MF__ MOD__"VTermCursorUp"


    VTT_TERMFIELD_REC   rcTermFieldCurrent;
    VTT_TERMFIELD_REC   rcTermField;
    VTT_TERMFIELD_REC   rcTermFieldNext;
    VTT_TERMFIELD_REC   rcTermFieldPrevious;
    VTT_LINE            ukLastLine;
    VTT_COLUMN          ukEndCol;


    DBGIN;

    if ( prcTermData -> kCursorLine == VTD_FIRST_LINE ) {
        if ( prcTermData -> rcOptions.bReturnOnFirst ) {
            DBGOUT;
            return TRUE;
        }
        DBGOUT;
        return FALSE;
    }

    if ( !VTermQueryField ( prcTermData, prcTermData -> kCursorLine,
                   prcTermData -> kCursorColumn, &rcTermFieldCurrent, NULL )) {
        VTermSetCursor ( hVPS, prcTermData,
                         (VTT_LINE)  (prcTermData->kCursorLine - 1),
                         (VTT_COLUMN) prcTermData->kCursorColumn, NULL, NULL );

        DBGOUT;

        return FALSE;
    }

    ukEndCol = rcTermFieldCurrent.kFirstUsableColumn +
                                         rcTermFieldCurrent.kLengthOfField - 1;

    while ( ukEndCol >= kMxColumns )
        ukEndCol -= kMxColumns;

    rcTermFieldNext.kFirstUsableLine   = VTD_FIRST_LINE;
    rcTermFieldNext.kFirstUsableColumn = 0;
    ukLastLine = VTD_TERMFIELD_UNDEF;

    while ( VTermQueryNextField ( prcTermData, &rcTermFieldNext )) {
        if ( rcTermFieldNext.kFirstUsableLine >=
                                          rcTermFieldCurrent.kFirstUsableLine )
            break;

        if ( rcTermFieldNext.kFirstUsableLine != ukLastLine )
            ukLastLine = rcTermFieldNext.kFirstUsableLine;

        rcTermField = rcTermFieldNext;
    }

    if ( ukLastLine == VTD_TERMFIELD_UNDEF ) {
        if ( prcTermData -> rcOptions.bReturnOnFirst ) {
            DBGOUT;
            return TRUE;
        }
        DBGOUT;

        return FALSE;
    }

    if ( VTermQueryField ( prcTermData, ukLastLine,
            prcTermData -> kCursorColumn, &rcTermField, &rcTermFieldPrevious ))

        VTermSetCursor ( hVPS, prcTermData, rcTermField.kFirstUsableLine,
             prcTermData -> kCursorColumn, &rcTermFieldCurrent, &rcTermField );

    else if ( rcTermFieldPrevious.kFirstUsableLine == ukLastLine )

        VTermSetCursor ( hVPS, prcTermData,
                         (VTT_LINE)    rcTermFieldPrevious.kFirstUsableLine,
                         (VTT_COLUMN) (rcTermFieldPrevious.kFirstUsableColumn +
                                       rcTermFieldPrevious.kLengthOfField - 1),
                         &rcTermFieldCurrent, &rcTermFieldPrevious );

    else {
        VTermQueryNextField ( prcTermData, &rcTermField );
        VTermSetCursor ( hVPS, prcTermData, rcTermField.kFirstUsableLine,
           rcTermField.kFirstUsableColumn, &rcTermFieldCurrent, &rcTermField );
    }

    DBGOUT;

    return FALSE;
}



/*------------------------------*/


// move the cursor down

static BOOL VTermCursorDown (   HVPS                hVPS,
                                VTT_PTERMDATA_REC   prcTermData     )
{
    #undef  MF__
    #define MF__ MOD__"VTermCursorDown"


    VTT_TERMFIELD_REC   rcTermFieldCurrent;
    VTT_TERMFIELD_REC   rcTermField;
    VTT_TERMFIELD_REC   rcTermFieldPrevious;
    VTT_COLUMN          ukEndCol;
    VTT_LINE            ukNextLine;


    DBGIN;

    if ( !VTermQueryField ( prcTermData, (VTT_LINE) prcTermData -> kCursorLine,
                            (VTT_COLUMN) prcTermData->kCursorColumn,
                            &rcTermFieldCurrent, NULL )) {
        if ( prcTermData -> kCursorLine < kMxRows - 1 ) {
            VTermSetCursor (hVPS, prcTermData,
                            (VTT_LINE) (prcTermData -> kCursorLine + 1),
                            (VTT_COLUMN)prcTermData -> kCursorColumn,
                            NULL, NULL );

            DBGOUT;

            return FALSE;
        }
    }

    ukEndCol = rcTermFieldCurrent.kFirstUsableColumn +
                                         rcTermFieldCurrent.kLengthOfField - 1;
    while ( ukEndCol >= kMxColumns )
        ukEndCol -= kMxColumns;

    rcTermField.kFirstUsableLine   = prcTermData -> kCursorLine;
    rcTermField.kFirstUsableColumn = prcTermData -> kCursorLine;

    while ( rcTermField.kFirstUsableLine == prcTermData -> kCursorLine )
        if ( !VTermQueryNextField ( prcTermData, &rcTermField )) {
            if ( prcTermData -> rcOptions.bReturnOnLast ) {
                DBGOUT;
                return TRUE;
            }

            DBGOUT;
            return FALSE;
        }

    ukNextLine = rcTermField.kFirstUsableLine;

    if ( VTermQueryField ( prcTermData, ukNextLine,
            prcTermData -> kCursorColumn, &rcTermField, &rcTermFieldPrevious ))

        VTermSetCursor ( hVPS, prcTermData, ukNextLine,
             prcTermData -> kCursorColumn, &rcTermFieldCurrent, &rcTermField );

    else if ( rcTermFieldPrevious.kFirstUsableLine == ukNextLine )

        VTermSetCursor ( hVPS, prcTermData,
                         (VTT_LINE) ukNextLine,
                         (VTT_COLUMN) (rcTermFieldPrevious.kFirstUsableColumn +
                                       rcTermFieldPrevious.kLengthOfField - 1),
                         &rcTermFieldCurrent, &rcTermFieldPrevious );

    else {
        VTermQueryNextField ( prcTermData, &rcTermField );
        VTermSetCursor ( hVPS, prcTermData, rcTermField.kFirstUsableLine,
           rcTermField.kFirstUsableColumn, &rcTermFieldCurrent, &rcTermField );
    }

    DBGOUT;

    return FALSE;
}



/*------------------------------*/


// move the cursor left

static BOOL VTermCursorLeft (   HVPS                hVPS,
                                VTT_PTERMDATA_REC   prcTermData     )
{
    #undef  MF__
    #define MF__ MOD__"VTermCursorLeft"


    VTT_TERMFIELD_REC   rcTermField;


    DBGIN;

    if ( !VTermQueryField ( prcTermData, prcTermData -> kCursorLine,
                          prcTermData -> kCursorColumn, &rcTermField, NULL )) {
        if ( prcTermData -> kCursorColumn > 1 )
            VTermSetCursor ( hVPS, prcTermData,
                             (VTT_LINE)   prcTermData -> kCursorLine,
                             (VTT_COLUMN)(prcTermData -> kCursorColumn - 1),
                             NULL, NULL );

        DBGOUT;

        return FALSE;
    }
    else {
        DBGOUT;

        return VTermSubCursorLeft ( hVPS, prcTermData, &rcTermField, 1 );
    }
}



/*------------------------------*/


// move the cursor left -- when we know we are already in a field

static BOOL VTermSubCursorLeft  (   HVPS                hVPS,
                                    VTT_PTERMDATA_REC   prcTermData,
                                    VTT_PTERMFIELD_REC  prcTermField,
                                    VTT_FIELDLENGTH     kDistance       )
{
    #undef  MF__
    #define MF__ MOD__"VTermSubCursorLeft"


    DBGIN;

    if ( prcTermField -> kCursorPosition - kDistance < 0 ) {
        DBGOUT;

        return ( VTermPrevField ( hVPS, prcTermData, TRUE ));
    }
    else {
        if ( prcTermData -> kCursorColumn == 0 )
            VTermSetCursor ( hVPS, prcTermData,
                             (VTT_LINE) (prcTermData -> kCursorLine - 1),
                             (VTT_COLUMN) (kMxColumns - 1),
                             prcTermField, prcTermField );
    else
        VTermSetCursor ( hVPS, prcTermData,
                         (VTT_LINE)   prcTermData -> kCursorLine,
                         (VTT_COLUMN)(prcTermData -> kCursorColumn - kDistance),
                         prcTermField, prcTermField );
    }

    DBGOUT;

    return FALSE;
}



/*------------------------------*/


// move the cursor right

static BOOL VTermCursorRight    (   HVPS                hVPS,
                                    VTT_PTERMDATA_REC   prcTermData     )
{
    #undef  MF__
    #define MF__ MOD__"VTermCursorRight"


    VTT_TERMFIELD_REC   rcTermField;


    DBGIN;

    if ( !VTermQueryField ( prcTermData, prcTermData -> kCursorLine,
                          prcTermData -> kCursorColumn, &rcTermField, NULL )) {
        if ( prcTermData -> kCursorColumn < kMxColumns - 1 )
            VTermSetCursor ( hVPS, prcTermData,
                             (VTT_LINE)   prcTermData -> kCursorLine,
                             (VTT_COLUMN)(prcTermData -> kCursorColumn + 1),
                             NULL, NULL );

        DBGOUT;

        return FALSE;
    }
    else {
        DBGOUT;

        return VTermSubCursorRight ( hVPS, prcTermData, &rcTermField, 1 );
    }
}



/*------------------------------*/


// move the cursor to next tab -- when we know we are already in a field

static BOOL VTermSubCursorRight (   HVPS                hVPS,
                                    VTT_PTERMDATA_REC   prcTermData,
                                    VTT_PTERMFIELD_REC  prcTermField,
                                    VTT_FIELDLENGTH     kDistance       )
{
    #undef  MF__
    #define MF__ MOD__"VTermSubCursorRight"


    DBGIN;

    if ( prcTermField -> kCursorPosition + kDistance + 1 >
                                             prcTermField -> kLengthOfField ) {
        DBGOUT;

        return VTermNextField ( hVPS, prcTermData );
    }
    else {
        if ( prcTermData -> kCursorColumn == kMxColumns - 1 )
            VTermSetCursor ( hVPS, prcTermData,
                             (VTT_LINE) (prcTermData -> kCursorLine + 1),
                             (VTT_COLUMN) 0, prcTermField, prcTermField );
        else
            VTermSetCursor ( hVPS, prcTermData,
                             (VTT_LINE)   prcTermData -> kCursorLine,
                             (VTT_COLUMN)(prcTermData -> kCursorColumn +
                                          kDistance),
                             prcTermField, prcTermField );
    }

    DBGOUT;

    return FALSE;
}



/*------------------------------*/


// move the cursor to the first field

static BOOL VTermFirstField (   HVPS                hVPS,
                                VTT_PTERMDATA_REC   prcTermData     )
{
    #undef  MF__
    #define MF__ MOD__"VTermFirstField"


    VTT_TERMFIELD_REC   rcTermField;


    DBGIN;

    rcTermField.kFirstUsableLine   = VTD_FIRST_LINE;
    rcTermField.kFirstUsableColumn = 0;

    if ( VTermQueryNextField ( prcTermData, &rcTermField ))
        VTermSetCursor ( hVPS, prcTermData, rcTermField.kFirstUsableLine,
                          rcTermField.kFirstUsableColumn, NULL, &rcTermField );

    DBGOUT;

    return FALSE;
}



/*------------------------------*/


// move the cursor to the last field

static BOOL VTermLastField  (   HVPS                hVPS,
                                VTT_PTERMDATA_REC   prcTermData )
{
    #undef  MF__
    #define MF__ MOD__"VTermLastField"


    BOOL                bFound;
    VTT_TERMFIELD_REC   rcTermField;


    DBGIN;


    rcTermField.kFirstUsableLine   = VTD_FIRST_LINE;
    rcTermField.kFirstUsableColumn = 0;
    bFound = FALSE;

    while ( VTermQueryNextField ( prcTermData, &rcTermField ))
        bFound = TRUE;

    if ( bFound )
        VTermSetCursor ( hVPS, prcTermData, rcTermField.kFirstUsableLine,
                          rcTermField.kFirstUsableColumn, NULL, &rcTermField );

    DBGOUT;

    return FALSE;
}



/*------------------------------*/


// move the cursor to the next field

static BOOL VTermNextField  (   HVPS                hVPS,
                                VTT_PTERMDATA_REC   prcTermData )
{
    #undef  MF__
    #define MF__ MOD__"VTermNextField"


    VTT_TERMFIELD_REC   rcTermField;


    DBGIN;

    rcTermField.kFirstUsableLine   = prcTermData -> kCursorLine;
    rcTermField.kFirstUsableColumn = prcTermData -> kCursorColumn;

    if ( VTermQueryNextField ( prcTermData, &rcTermField ))
        VTermSetCursor ( hVPS, prcTermData, rcTermField.kFirstUsableLine,
                          rcTermField.kFirstUsableColumn, NULL, &rcTermField );
    else if ( prcTermData -> rcOptions.bReturnOnLast ) {
        prcTermData -> rcKeystroke.ucVKey      = VT_NEXT_FIELD;
        prcTermData -> rcKeystroke.ucCharacter = 0;

        DBGOUT;

        return TRUE;                            // terminate user entry
    }
    else
        VTermFirstField ( hVPS, prcTermData );

    DBGOUT;

    return FALSE;
}



/*------------------------------*/


// move the cursor to the previous field

static BOOL VTermPrevField  (   HVPS                hVPS,
                                VTT_PTERMDATA_REC   prcTermData,
                                BOOL                bAtEnd          )
{
    #undef  MF__
    #define MF__ MOD__"VTermPrevField"


    VTT_TERMFIELD_REC   rcTermField, rcTermFieldPrevious;


    DBGIN;

    if ( VTermQueryField ( prcTermData, prcTermData -> kCursorLine,
                                prcTermData ->kCursorColumn, &rcTermField,
                                                      &rcTermFieldPrevious )) {
        if ( rcTermFieldPrevious.kFirstUsableLine != VTD_TERMFIELD_UNDEF )
            if ( bAtEnd )
                VTermSetCursor ( hVPS, prcTermData,
                           (VTT_LINE)   rcTermFieldPrevious.kFirstUsableLine,
                           (VTT_COLUMN)(rcTermFieldPrevious.kFirstUsableColumn +
                                        rcTermFieldPrevious.kLengthOfField - 1),
                           &rcTermField, &rcTermFieldPrevious );
            else
                VTermSetCursor ( hVPS, prcTermData,
                            (VTT_LINE)  rcTermFieldPrevious.kFirstUsableLine,
                            (VTT_COLUMN)rcTermFieldPrevious.kFirstUsableColumn,
                            &rcTermField, &rcTermFieldPrevious );

        else if ( prcTermData -> rcOptions.bReturnOnFirst ) {
            prcTermData -> rcKeystroke.ucVKey      = VT_PREV_FIELD;
            prcTermData -> rcKeystroke.ucCharacter = 0;

            DBGOUT;

            return TRUE;                    // terminate user entry
        }
        else
            VTermLastField ( hVPS, prcTermData );
    }

    DBGOUT;

    return FALSE;
}



/*------------------------------*/


// delete the current character

static BOOL VTermDeleteChar (   HVPS                hVPS,
                                VTT_PTERMDATA_REC   prcTermData )
{
    #undef  MF__
    #define MF__ MOD__"VTermDeleteChar"


    VTT_SCREENINDEX     kIndex;
    VTT_TERMFIELD_REC   rcTermField;


    DBGIN;

    if ( VTermQueryField ( prcTermData , prcTermData -> kCursorLine,
                          prcTermData -> kCursorColumn, &rcTermField, NULL )) {
        kIndex = rcTermField.kIndexToScreenBuffer +
                                                   rcTermField.kCursorPosition;

        SAPDB_memmove ( &( prcTermData -> psScreenBuffer [ kIndex ]),
                    &prcTermData -> psScreenBuffer [ kIndex + 1 ],
                                        rcTermField.kLengthOfField -
                                            rcTermField.kCursorPosition - 1 );

        prcTermData -> psScreenBuffer [ kIndex + rcTermField.kLengthOfField -
                                       rcTermField.kCursorPosition - 1 ] = ' ';

        VTermHideCursor     ( hVPS, prcTermData, &rcTermField );
        VTermPaintAVioLines ( hVPS, prcTermData,
                                        rcTermField.kDelimiterStartLine,
                                               rcTermField.kDelimiterEndLine );
        VTermShowCursor     ( hVPS, prcTermData, &rcTermField );

        VTermMarkField      ( prcTermData, &rcTermField );
    }
    else
        VTermBeep ();

    DBGOUT;

    return FALSE;
}



/*------------------------------*/


// delete the character to the left of the cursor

static BOOL VTermBackSpace  (   HVPS                hVPS,
                                VTT_PTERMDATA_REC   prcTermData )
{
    #undef  MF__
    #define MF__ MOD__"VTermBackSpace"


    VTT_SCREENINDEX     kIndex;
    VTT_TERMFIELD_REC   rcTermField;


    DBGIN;

    if ( VTermQueryField ( prcTermData , prcTermData -> kCursorLine,
                           prcTermData -> kCursorColumn, &rcTermField,
                                   NULL ) && rcTermField.kCursorPosition > 0) {
        kIndex = rcTermField.kIndexToScreenBuffer +
                                                   rcTermField.kCursorPosition;

        SAPDB_memmove ( &( prcTermData -> psScreenBuffer [ kIndex - 1 ] ),
                  &prcTermData -> psScreenBuffer [ kIndex ],
                   rcTermField.kLengthOfField - rcTermField.kCursorPosition );

        prcTermData -> psScreenBuffer [ kIndex + rcTermField.kLengthOfField -
                                       rcTermField.kCursorPosition - 1 ] = ' ';

        VTermHideCursor     ( hVPS, prcTermData, &rcTermField );
        VTermPaintAVioLines ( hVPS, prcTermData,
                              rcTermField.kDelimiterStartLine,
                              rcTermField.kDelimiterEndLine );
        VTermShowCursor     ( hVPS, prcTermData, &rcTermField );

        VTermMarkField      ( prcTermData, &rcTermField );

        VTermSubCursorLeft  ( hVPS, prcTermData, &rcTermField, 1 );
    }
    else
        VTermBeep ();

    DBGOUT;

    return FALSE;
}



/*------------------------------*/


// delete to the end of the current field

static BOOL VTermDeleteEof  (   HVPS                hVPS,
                                VTT_PTERMDATA_REC   prcTermData )
{
    #undef  MF__
    #define MF__ MOD__"VTermDeleteEof"


    VTT_SCREENINDEX     kIndex;
    VTT_TERMFIELD_REC   rcTermField;


    DBGIN;

    if ( VTermQueryField ( prcTermData, prcTermData -> kCursorLine,
                          prcTermData -> kCursorColumn, &rcTermField, NULL )) {
        kIndex = rcTermField.kIndexToScreenBuffer +
                                                   rcTermField.kCursorPosition;

        memset ( &( prcTermData -> psScreenBuffer [ kIndex ]), ' ',
                    rcTermField.kLengthOfField - rcTermField.kCursorPosition );

        VTermHideCursor     ( hVPS, prcTermData, &rcTermField );
        VTermPaintAVioLines ( hVPS, prcTermData,
                              rcTermField.kDelimiterStartLine,
                              rcTermField.kDelimiterEndLine );
        VTermShowCursor     ( hVPS, prcTermData, &rcTermField );

        VTermMarkField      ( prcTermData, &rcTermField );
    }
    else
        VTermBeep ();

    DBGOUT;

    return FALSE;
}



/*------------------------------*/


static BOOL VTermToggleInsert   (   HVPS                hVPS,
                                    VTT_PTERMDATA_REC   prcTermData     )
{
    #undef  MF__
    #define MF__ MOD__"VTermToggleInsert"


    DBGIN;

    bInsert = !bInsert;

    VTermHideCursor ( hVPS, prcTermData, NULL );

    VTermPaintAVioLines ( hVPS, prcTermData,
                          (VTT_LINE) (prcTermData -> kInsertLine - 1),
                          (VTT_LINE) (prcTermData -> kInsertLine - 1) );

    VTermShowCursor ( hVPS, prcTermData, NULL );

    if ( !VTermToggleCursor ( bInsert, &rcVideoCursorInfoSave ))
        return TRUE;

    DBGOUT;

    return FALSE;
}



/*------------------------------*/


// process a character key

static BOOL VTermDoChar (   HVPS                hVPS,
                            VTT_PTERMDATA_REC   prcTermData,
                            VTT_SCREENCHAR      cCharacter      )
{
    #undef  MF__
    #define MF__ MOD__"VTermDoChar"


    VTT_SCREENINDEX     kIndex;
    VTT_TERMFIELD_REC   rcTermField;


    DBGIN;

    DBG3 (( MF__, "Line of cursor:   %d", prcTermData -> kCursorLine   ));
    DBG3 (( MF__, "Column of cursor: %d", prcTermData -> kCursorColumn ));

    if ( VTermQueryField ( prcTermData, prcTermData -> kCursorLine,
                          prcTermData -> kCursorColumn, &rcTermField, NULL )) {
        kIndex = rcTermField.kIndexToScreenBuffer +
                                                   rcTermField.kCursorPosition;
        DBG3 (( MF__, "Screen insert index: %d", kIndex ));

        if ( bInsert ) {
            SAPDB_memmove ( &( prcTermData -> psScreenBuffer [ kIndex + 1 ] ),
                      &prcTermData -> psScreenBuffer [ kIndex ],
                      rcTermField.kLengthOfField -
                      rcTermField.kCursorPosition - 1 );
            prcTermData -> psScreenBuffer [ kIndex ] = cCharacter;
        }
        else
            prcTermData -> psScreenBuffer [ kIndex ] = cCharacter;

        VTermHideCursor     ( hVPS, prcTermData, &rcTermField );
        VTermPaintAVioLines ( hVPS, prcTermData,
                              rcTermField.kDelimiterStartLine,
                              rcTermField.kDelimiterEndLine );
        VTermShowCursor     ( hVPS, prcTermData, &rcTermField );

        VTermMarkField      ( prcTermData, &rcTermField );

        DBGOUT;

        return VTermSubCursorRight ( hVPS, prcTermData, &rcTermField, 1 );
    }
    else
        VTermBeep ();

    DBGOUT;

    return FALSE;
}



/*------------------------------*/


// beep the user

static VOID VTermBeep   (   VOID    )
{
    #undef  MF__
    #define MF__ MOD__"VTermBeep"


    BEEP( 880, 100 );

    DBGPAS;
}


// ============================================================================

#if 0
static BOOL IsEnhancedKbd   (   VOID    )
{
    #undef  MF__
    #define MF__ MOD__"IsEnhancedKbd"


    #if defined(_WIN32)

      static PSZ Key      ="HARDWARE\\DESCRIPTION\\System\\MultifunctionAdapter"
                           "\\0\\KeyboardController\\0\\KeyboardPeripheral\\0";
      static PSZ ValueName="Identifier";
      static PSZ Enhanced ="PCAT_ENHANCED";
      static CHAR Buf[20];
      static HKEY hKey;
      DWORD  Err, Type, Size = sizeof(Buf);

      DBGPAS;
      if (!(Err = RegOpenKey(HKEY_LOCAL_MACHINE, Key, &hKey))) {
        Err = RegQueryValueEx(hKey, ValueName, NULL, &Type, Buf, &Size);
        RegCloseKey(hKey);
        if (!Err && Type==REG_SZ) return (!lstrcmpi(Buf, Enhanced));
      }
      DBG1  (( MF__, ERRMSG_VT_OPEN_KEYBOARD, Err ));
      MSGCD (( ERR_VT_OPEN_KEYBOARD, Err ));
      return (FALSE);

    #else

      APIRET  rc                  = 0;
      ULONG   ulAction            = 0;
      HANDLE  hKbd                = 0;
      ULONG   ulCategory          = IOCTL_KEYBOARD;
      ULONG   ulFunction          = KBD_GETKEYBDTYPE;
      PVOID   pvParmList          = NULL;
      ULONG   ulParmLengthMax     = 0;
      ULONG   ulParmLengthInOut   = 0;
      KBDTYPE stKbdType;
      ULONG   ulDataLengthMax     = sizeof ( KBDTYPE );
      ULONG   ulDataLengthInOut   = sizeof ( KBDTYPE );


      DBGIN;

      rc = DosOpen ( "KBD$", &hKbd, &ulAction, 0, FILE_NORMAL, FILE_OPEN,
                     OPEN_ACCESS_READONLY | OPEN_SHARE_DENYNONE, NULL );
      if ( rc ) {
        DBG1  (( MF__, ERRMSG_VT_OPEN_KEYBOARD, rc ));
        MSGCD (( ERR_VT_OPEN_KEYBOARD, rc           ));

        DBGOUT;

        return ( FALSE );
      }

      rc = DosDevIOCtl ( hKbd, ulCategory, ulFunction, pvParmList,
                         ulParmLengthMax, &ulParmLengthInOut, &stKbdType,
                                         ulDataLengthMax, &ulDataLengthInOut );
      DosClose ( hKbd );

      if ( rc ) {
        DBG1  (( MF__, ERRMSG_VT_OPEN_KEYBOARD, rc ));
        MSGCD (( ERR_VT_OPEN_KEYBOARD, rc           ));

        DBGOUT;

        return FALSE;
      }

    DBGOUT;

    return stKbdType.usType;

    #endif
}
#endif


/*------------------------------*/


static BOOL IsMonochrome    (   VOID    )
{
    #undef  MF__
    #define MF__ MOD__"IsMonochrome"


    VIOCONFIGINFO   rcVideoConfigInfo;


    DBGIN;

    rcVideoConfigInfo.cb = sizeof ( VIOCONFIGINFO );
    VioGetConfig ( 0, &rcVideoConfigInfo, 0 );

    if ( rcVideoConfigInfo.adapter == DISPLAY_MONOCHROME ||
         rcVideoConfigInfo.display == MONITOR_MONOCHROME ) {

        DBGOUT;

        return TRUE;
    }
    else {
        DBGOUT;

        return FALSE;
    }
}



/*------------------------------*/


static VOID GetOptions  (   VTT_POPTIONS_REC    prcOptions,
                            PASCAL_OPTIONS      psOptions       )
{
    #undef  MF__
    #define MF__ MOD__"GetOptions"


    DBGIN;

    prcOptions -> bWaitForInput     = ( BOOL ) psOptions [ 0 ];
    prcOptions -> bReturnOnLast     = ( BOOL ) psOptions [ 2 ];
    prcOptions -> bReturnOnFirst    = ( BOOL ) psOptions [ 3 ];
    prcOptions -> bIsBell           = ( BOOL ) psOptions [ 4 ];
    prcOptions -> bMark             = ( BOOL ) psOptions [ 5 ];
    prcOptions -> bWaitForChar      = ( BOOL ) psOptions [ 6 ];

    prcOptions -> eUsageMode        = ( VTT_USAGE_MODE ) psOptions [ 1 ];

    DBGOUT;
}



/*------------------------------*/


static void GetKeystroke    (   VTT_PKEYSTROKE_REC  prcKeystroke,
                                PASCAL_KEYSTROKE*   psKeystroke     )
{
    #undef  MF__
    #define MF__ MOD__"GetKeystroke"


    DBGIN;

    prcKeystroke -> ucVKey      = ( VTT_KEY        ) psKeystroke->ucVTKey;
    prcKeystroke -> ucCharacter = ( VTT_SCREENCHAR ) psKeystroke->ucCh;
    prcKeystroke -> ulMouseX    = ( ULONG )          psKeystroke->MouseX;
    prcKeystroke -> ulMouseY    = ( ULONG )          psKeystroke->MouseY;

    DBGOUT;
}



/*------------------------------*/


static void PutKeystroke    (   PASCAL_KEYSTROKE*   psKeystroke,
                                VTT_KEYSTROKE_REC   rcKeystroke     )
{
    #undef  MF__
    #define MF__ MOD__"PutKeystroke"


    DBGIN;

    psKeystroke->ucVTKey = (UCHAR)rcKeystroke.ucVKey;
    psKeystroke->ucCh    = (UCHAR)rcKeystroke.ucCharacter;
    psKeystroke->MouseX  = (INT4 )rcKeystroke.ulMouseX;
    psKeystroke->MouseY  = (INT4 )rcKeystroke.ulMouseY;

    DBGOUT;
}



/*------------------------------*/


// �berpr�fung, ob die letzte Taste ein Returnkey war. Wenn Ja, dann wird TRUE,
// sonst FALSE zurueckgeliefert.

static BOOL IsReturnkey (   VTT_PTERMDATA_REC   prcTermData,
                            VTT_KEY             ucVKey          )
{
    #undef  MF__
    #define MF__ MOD__"IsReturnkey"


    VTT_RETURNKEYS  psReturnKeys;

    DBGPAS;

    switch (ucVKey) {
        case VT_ENTER:
            if ( prcTermData -> rcOptions.eUsageMode == VT_EDIT ) {
                return FALSE;
            }
            break;

        case VT_CANCEL_KEY:
            return FALSE;

        default:
            break;
    }

    psReturnKeys = prcTermData -> psReturnKeys;

    for (; *psReturnKeys; psReturnKeys++ ) {
        if ( ucVKey == *psReturnKeys ) {
            return TRUE;
        }
    }

    return FALSE;
}


/*------------------------------*/


// �berpr�fung, ob die letzte Taste ein Rejectkey war. Wenn ja, wird TRUE,
// sonst FALSE zurueckgeliefert.

static BOOL IsRejectkey (   VTT_REJECTKEYS psRejectkeys,
                            VTT_KEY        ucVKey          )
{
    #undef  MF__
    #define MF__ MOD__"IsRejectkey"


    DBGIN;

    for (; *psRejectkeys; psRejectkeys++ ) {
        if ( ucVKey == *psRejectkeys ) {
            VTermBeep () ;

            DBGOUT;

            return TRUE;
        }
    }

    DBGOUT;

    return FALSE;
}



/*------------------------------*/


#if defined ( SOFTKEY_SYSLINE )

// Aus dem String mit s�mtlichen Labels das gew�nschte extrahieren.

static BOOL GetLabel    (   VTT_LABEL      pszLabel,
                            PASCAL_LABEL   psLabels,
                            VTT_LABELINDEX kNumber     )
{
    #undef  MF__
    #define MF__ MOD__"GetLabel"


    DBGIN;

    strncpy ( pszLabel, psLabels + ( kNumber * VT_LABEL_SIZE * 2 ),
                                                               VT_LABEL_SIZE );
    pszLabel [ VT_LABEL_SIZE ] = 0;

    DBG3 (( MF__, "Extrahiertes Label: \"%s\"", pszLabel ));

    DBGOUT;

    return TRUE;
}

#endif

/*------------------------------*/


static BOOL GetPresentationSpaceSizes   (   PSHORT  pkRows,
                                            PSHORT  pkColumns   )
{
    #undef  MF__
    #define MF__ MOD__"GetPresentationSpaceSizes"


    VIOMODEINFO rcVideoModeData;
    USHORT      ukError;


    DBGIN;

    rcVideoModeData.cb = sizeof ( VIOMODEINFO );

    ukError = VioGetMode ( &rcVideoModeData, ( HVIO ) NULL );
    if ( !ukError ) {
        *pkRows    = rcVideoModeData.row;
        *pkColumns = rcVideoModeData.col;

        if (( rcVideoModeData.col > sizeof (tsp00_Line) ) ||
            ( rcVideoModeData.row > VTD_MAX_LINES ))
          {
          DBGOUT;
          return FALSE;
          }

        DBG3 (( MF__, "Anzahl der Zeilen  : %d", *pkRows    ));
        DBG3 (( MF__, "Anzahl der Spalten : %d", *pkColumns ));
    }
    else {
        *pkRows    = VTD_ROWS_DEFAULT;
        *pkColumns = VTD_COLUMNS_DEFAULT;

        DBG1  (( MF__, ERRMSG_VT_GET_VIDEO_MODE, ukError ));
        MSGCD (( ERR_VT_GET_VIDEO_MODE, ukError           ));

        DBGOUT;
        return FALSE;
    }

    DBGOUT;
    return TRUE;
}

/*------------------------------*/

static LONG SetKeyboardStatus (  PKBDINFO   pKeyStat    )
{
    #undef  MF__
    #define MF__ MOD__"SetKeyboardStatus"
    APIRET    rc  = NO_ERROR;

    DBGIN;

    rc = KbdSetStatus ( pKeyStat, 0 );

    if ( rc ) {
        DBG1  (( MF__, "Could not set keyboard status, rc = %d", rc ));
        MSGCD (( ERR_VT_SET_KBD_STATUS, rc ));

        DBGOUT;

        return rc ;
    }

    DBGOUT;

    return ( rc );
}

/*------------------------------*/

static LONG GetKeyboardStatus (   PKBDINFO   pKeyStat    )
{
    #undef  MF__
    #define MF__ MOD__"GetKeyboardStatus"
    APIRET    rc  = NO_ERROR;

    DBGIN;

    (*pKeyStat).cb = sizeof ( KBDINFO );

    rc = KbdGetStatus ( pKeyStat, 0 );

    if ( rc ) {
        DBG1  (( MF__, "Could not get keyboard status, rc = %d", rc ));
        MSGCD (( ERR_VT_GET_KBD_STATUS, rc ));

        DBGOUT;

        return rc ;
    }


    DBGOUT;

    return ( rc );
}

/*------------------------------*/

#if defined(_WIN32)

 static LONG GetEvent(   VTT_PMOUSE_KEYBORD_REC  *ppMouKeybEvent,
                         PULONG                  pulEvent   )
 {
     #undef  MF__
     #define MF__ MOD__"GetEvent"
     INPUT_RECORD            InpRecord;
     DWORD                   NumberRead;
     PKBDKEYINFO             KeyData;
     PMOUEVENTINFO           MouseData;
     static HANDLE           Input             = INVALID_HANDLE_VALUE;
     static BOOL             fButtonDbl        = FALSE;
     static DWORD            dwPrevButtonState = 0;

     DBGIN;
     *ppMouKeybEvent = &MouKeybEvent;
     KeyData        = &MouKeybEvent.rcKeyboardKeyInfo;
     MouseData      = &MouKeybEvent.rcMouseEventInfo;

     if ( Input == INVALID_HANDLE_VALUE )
         Input = GetStdHandle(STD_INPUT_HANDLE);

     if ( Input == INVALID_HANDLE_VALUE ) {
         DBGOUT;
         return ( GetLastError() );
     }

     SetKeyboardStatus ( &KeyStat );

     for (;;) {
         if ( !ReadConsoleInput(Input, &InpRecord, 1, &NumberRead) ) {
             DBGOUT;
             return ( GetLastError() );
         }
         if ( NumberRead < 1 )
             continue;

         switch ( InpRecord.EventType ) {
             case KEY_EVENT :

                 if ( !InpRecord.Event.KeyEvent.bKeyDown )
                     break;

                 KeyData->chChar  = InpRecord.Event.KeyEvent.uChar.AsciiChar;
                 KeyData->chScan  = InpRecord.Event.KeyEvent.wVirtualKeyCode;
                 KeyData->fsState = 0;

                 if (InpRecord.Event.KeyEvent.dwControlKeyState & ENHANCED_KEY &&
                         InpRecord.Event.KeyEvent.uChar.AsciiChar != '\r')
                     KeyData->chChar = (BYTE)VTD_SCAN_CODE;
                 if (InpRecord.Event.KeyEvent.dwControlKeyState &
                         LEFT_CTRL_PRESSED ) {
                     KeyData->chScan  |= 0x100;
                     KeyData->fsState |= 0x104;
                 }
                 if (InpRecord.Event.KeyEvent.dwControlKeyState &
                         RIGHT_CTRL_PRESSED) {
                     KeyData->chScan  |= 0x100;
                     KeyData->fsState |= 0x404;
                 }
                 if (InpRecord.Event.KeyEvent.dwControlKeyState &
                        LEFT_ALT_PRESSED ) {
                     KeyData->chScan  |= 0x400;
                     KeyData->fsState |= 0x208;
                 }
                 if (InpRecord.Event.KeyEvent.dwControlKeyState &
                        RIGHT_ALT_PRESSED) {
                     KeyData->chScan  |= 0x400;
                     KeyData->fsState |= 0x408;
                 }
                 if (InpRecord.Event.KeyEvent.dwControlKeyState &
                        SHIFT_PRESSED) {
                     if (KeyData->chScan == 9)   // backtab
                       KeyData->chChar = 0;
                     KeyData->chScan  |= 0x200;
                     KeyData->fsState |= 0x03;
                 }
                 if (InpRecord.Event.KeyEvent.dwControlKeyState &
                        CAPSLOCK_ON) {
                     KeyData->fsState |= 0x40;
                 }
                 if (InpRecord.Event.KeyEvent.dwControlKeyState &
                        NUMLOCK_ON) {
                     KeyData->fsState |= 0x20;
                 }
                 if (InpRecord.Event.KeyEvent.dwControlKeyState &
                        SCROLLLOCK_ON) {
                     KeyData->fsState |= 0x10;
                 }
                 DBG3((MF__, "Returning character %d, scan=%#x",
                     KeyData->chChar, KeyData->chScan));

                 *pulEvent = VTD_KEYB_EVENT;

                 DBGOUT;
                 return ( NO_ERROR );


             case MOUSE_EVENT :

                 if ((InpRecord.Event.MouseEvent.dwEventFlags == MOUSE_MOVED) ||
                     ((InpRecord.Event.MouseEvent.dwButtonState & 1) ==
                      dwPrevButtonState)) {
                     break;
                 }

                 switch ( dwPrevButtonState ) {
                     case 1:  // -- prev. state : VTD_MOUSE_DOWN

                         if (( InpRecord.Event.MouseEvent.dwEventFlags ==
                               DOUBLE_CLICK       ) ||
                             ( fButtonDbl == TRUE ))
                             MouseData->fs = VTD_MOUSE_DBL;
                         else
                             MouseData->fs = VTD_MOUSE_UP;

                         fButtonDbl = FALSE;
                     break;

                     case 0: // -- prev. state : VTD_MOUSE_UP

                         if ( InpRecord.Event.MouseEvent.dwEventFlags ==
                              DOUBLE_CLICK )
                             fButtonDbl = TRUE;
                         else
                             fButtonDbl = FALSE;

                         MouseData->fs = VTD_MOUSE_DOWN;
                     break;
                 }

                 MouseData->col =
                     InpRecord.Event.MouseEvent.dwMousePosition.X;
                 MouseData->row =
                     InpRecord.Event.MouseEvent.dwMousePosition.Y;

                 *pulEvent          = VTD_MOUSE_EVENT;
                 dwPrevButtonState ^= 1;

                 DBGOUT;
                 return ( NO_ERROR );
         }
     }
 }

/*------------------------------*/

 static LONG TriggerNextEvent(   ULONG  ulEvent   )
 {
     #undef  MF__
     #define MF__ MOD__"TriggerNextEvent"

     DBGOUT;
     return ( NO_ERROR );
 }

/*------------------------------*/

#else

 static LONG GetEvent(   VTT_PMOUSE_KEYBORD_REC  *ppMouKeybEvent,
                         PULONG                  pulEvent   )
 {
     #undef  MF__
     #define MF__ MOD__"GetEvent"
     APIRET                  rc;

     *ppMouKeybEvent = &MouKeybEvent;

     do {
        rc = DosWaitMuxWaitSem( hmuxKeybMouEvent, SEM_INDEFINITE_WAIT,
                                pulEvent );

     } while ( rc == ERROR_INTERRUPT );

     if ( rc ) {
         MSGD (( ERR_WAITING_SEM, "Mutex", rc ));
         DBG1 ((MF__, "Error on waiting semaphore 'Mutex', rc = %u", rc));
          DBGOUT;
          return ( rc );
     }

     DBGOUT;
     return ( NO_ERROR );
 }

/*------------------------------*/

 static LONG TriggerNextEvent(   ULONG  ulEvent   )
 {
     #undef  MF__
     #define MF__ MOD__"TriggerNextEvent"
     APIRET                  rc;
     ULONG                   ulPostCnt;

     switch ( ulEvent ) {
         case VTD_KEYB_EVENT:

             DosResetEventSem( hevKeybEvent, &ulPostCnt );

             rc = sql41c_post_event_sem ( hevTrigKeybEvent, "TrigKeyb" );

             if (( rc  != NO_ERROR ) && ( rc != ERROR_ALREADY_POSTED )) {
                 ABORT();
             }
             break;

         case VTD_MOUSE_EVENT:

             DosResetEventSem( hevMouEvent, &ulPostCnt );

             rc = sql41c_post_event_sem ( hevTrigMouEvent, "TrigMou" );

             if (( rc  != NO_ERROR ) && ( rc != ERROR_ALREADY_POSTED )) {
                 ABORT();
             }
             break;

     }

     DBGOUT;
     return ( NO_ERROR );
 }

/*------------------------------*/


 static VOID _System KeybThread  (   VTT_PMOUSE_KEYBORD_REC  pMouKeybEvent    )
 {
     #undef  MF__
     #define MF__ MOD__"KeybThread"
     APIRET      rc  = NO_ERROR;
     KBDINFO     TmpKeyStat;

     DBGIN;

     for ( ;; ) {

         // --- set keyboard raw mode
         TmpKeyStat         = OldKeyStat;
         TmpKeyStat.fsMask |= 0x0004;
         TmpKeyStat.fsMask &= 0xFFF7;

         if ( SetKeyboardStatus ( &TmpKeyStat ) != NO_ERROR ) {
             ABORT();
         }

         KbdCharIn ( &(pMouKeybEvent->rcKeyboardKeyInfo), IO_WAIT, 0 );

         // --- set keyboard cocked mode
         if ( SetKeyboardStatus ( &OldKeyStat ) != NO_ERROR ) {
             ABORT();
         }

         rc = sql41c_post_event_sem ( hevKeybEvent, "KeybEvent" );

         if (( rc  != NO_ERROR ) && ( rc != ERROR_ALREADY_POSTED )) {
             ABORT();
         }

         do {
             rc = sql41c_wait_event_sem ( hevTrigKeybEvent, UNDEF, "TrigKeyb" );
         } while ( rc == ERROR_INTERRUPT );

         if ( rc ) {
             ABORT();
         }

     }

     DBGOUT;
     return;
 }

 /*------------------------------*/

 static VOID _System MouThread   (   VTT_PMOUSE_KEYBORD_REC  pMouKeybEvent    )
 {
     #undef  MF__
     #define MF__ MOD__"MouThread"
     APIRET    rc                 = NO_ERROR;
     ULONG     ulDblClck          = GetDblClckSpeed ();
     ULONG     ulLastReleaseEvent = 0;
     USHORT    ulReadType         = 1;  // WAIT

     DBGIN;

     DBGIN;

     for ( ;; ) {
         MouReadEventQue ( &(pMouKeybEvent->rcMouseEventInfo),
                           &ulReadType, MouHandle );

         if ( pMouKeybEvent->rcMouseEventInfo.fs == VTD_MOUSE_UP ) {
             if ( ulDblClck > pMouKeybEvent->rcMouseEventInfo.time -
                  ulLastReleaseEvent ) {
             pMouKeybEvent->rcMouseEventInfo.fs = VTD_MOUSE_DBL;
             }
             else {
                 ulLastReleaseEvent = pMouKeybEvent->rcMouseEventInfo.time;
             }
         }

         rc = sql41c_post_event_sem ( hevMouEvent, "MouEvent" );

         if (( rc  != NO_ERROR ) && ( rc != ERROR_ALREADY_POSTED )) {
             ABORT();
         }

         do {
             rc = sql41c_wait_event_sem ( hevTrigMouEvent, UNDEF, "TrigMou" );
         } while ( rc == ERROR_INTERRUPT );

         if ( rc ) {
             ABORT();
         }
     }


     DBGOUT;
     return;
 }

/*------------------------------*/

 static ULONG GetDblClckSpeed ( VOID )
 {
     #undef  MF__
     #define MF__ MOD__"GetDblClckSpeed"
     CHAR     szSpeed[21];


     DBGIN;

     PrfQueryProfileString( HINI_USER,
                            (PSZ)"PM_ControlPanel",
                            (PSZ)"DoubleClickSpeed",
                            VTD_MOUSE_DBL_DEFAULT,
                            (PSZ)szSpeed,
                            (ULONG)sizeof(20));


     DBGOUT;
     return ( (ULONG)atol(szSpeed) );
 }

#endif


//
// =============================== END ========================================
//
