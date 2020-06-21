/*!
  @file           vos09u.c
  @author         RaymondR
  @brief          OS/2 console functions adapted to Windows NT
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
#include "gos00ds.h"

/*
 * DEFINES
 */
#define MOD__  "VOS09UC : "
#define MF__   MOD__"UNDEFINED"

/*
 * MACROS
 */


/*
 * LOCAL TYPE AND STRUCT DEFINITIONS
 */


/*
 * EXTERNAL VARIABLES
 */


/*
 * EXPORTED VARIABLES
 */


/*
 * LOCAL VARIABLES
 */

static HANDLE Input  = INVALID_HANDLE_VALUE;
static HANDLE Output = INVALID_HANDLE_VALUE;

/*
 * LOCAL FUNCTION PROTOTYPES
 */


/*
 * ========================== GLOBAL FUNCTIONS ================================
 */

APIRET16 KbdCharIn(PKBDKEYINFO KeyData, USHORT Wait, HANDLE Handle)
{
    #undef  MF__
    #define MF__ MOD__"KbdCharIn"
    INPUT_RECORD  InpRecord;
    DWORD         NumberRead;

    DBGIN;
    if (Input==INVALID_HANDLE_VALUE) Input = GetStdHandle(STD_INPUT_HANDLE);
    if (Input==INVALID_HANDLE_VALUE) {
        DBGOUT;
        return ((APIRET16)GetLastError());
    }
    for (;;) {
        if (!ReadConsoleInput(Input, &InpRecord, 1, &NumberRead)) {
            DBGOUT;
            return ((APIRET16)GetLastError());
        }
        if (NumberRead==1 && InpRecord.EventType==KEY_EVENT) {
            if (InpRecord.Event.KeyEvent.bKeyDown) {
                KeyData->chChar  = InpRecord.Event.KeyEvent.uChar.AsciiChar;
                KeyData->chScan  = InpRecord.Event.KeyEvent.wVirtualKeyCode;
                KeyData->fsState = 0;

                if (InpRecord.Event.KeyEvent.dwControlKeyState & ENHANCED_KEY &&
                        InpRecord.Event.KeyEvent.uChar.AsciiChar != '\r')
                    KeyData->chChar = (BYTE)0xe0;
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
                DBGOUT;
                return ((APIRET16)NO_ERROR);
            }
        }
    }
}

/*------------------------------*/

APIRET16 KbdGetStatus(PKBDINFO Status, HANDLE Handle)
{
    #undef  MF__
    #define MF__ MOD__"KbdGetStatus"
    DWORD Mask;

    DBGPAS;
    if (Input==INVALID_HANDLE_VALUE) Input = GetStdHandle(STD_INPUT_HANDLE);
    if (Input==INVALID_HANDLE_VALUE) return ((APIRET16)GetLastError());
    if (!GetConsoleMode(Input, &Mask)) return ((APIRET16)GetLastError());
    Status->fsMask = (WORD)Mask;
    return ((APIRET16)NO_ERROR);
}

/*------------------------------*/

APIRET16 KbdSetStatus(PKBDINFO Status, HANDLE Handle)
{
    #undef  MF__
    #define MF__ MOD__"KbdSetStatus"
    DBGPAS;
    if (Input==INVALID_HANDLE_VALUE) Input = GetStdHandle(STD_INPUT_HANDLE);
    if (Input==INVALID_HANDLE_VALUE) return ((APIRET16)GetLastError());
    if (!SetConsoleMode(Input, Status->fsMask))
        return ((APIRET16)GetLastError());
    return ((APIRET16)NO_ERROR);
}

/*------------------------------*/

APIRET16 VioGetCurType(PVIOCURSORINFO CursorInfo, HVIO Handle)
{
    #undef  MF__
    #define MF__ MOD__"VioGetCurType"
    CONSOLE_CURSOR_INFO NtCursorInfo;

    DBGPAS;
    if (Output==INVALID_HANDLE_VALUE) Output = GetStdHandle(STD_OUTPUT_HANDLE);
    if (Output==INVALID_HANDLE_VALUE) return ((APIRET16)GetLastError());
    if (!GetConsoleCursorInfo(Output, &NtCursorInfo))
        return ((APIRET16)GetLastError());
    if (NtCursorInfo.bVisible) {
        CursorInfo->yStart = (USHORT)(NtCursorInfo.dwSize - 110);
        CursorInfo->cEnd   = (USHORT)-100;
        CursorInfo->attr   = 0;
    } else CursorInfo->attr = 0xffff;
    return ((APIRET16)NO_ERROR);
}

/*------------------------------*/

APIRET16 VioSetCurType(PVIOCURSORINFO CursorInfo, HVIO Handle)
{
    #undef  MF__
    #define MF__ MOD__"VioSetCurType"
    CONSOLE_CURSOR_INFO NtCursorInfo;

    DBGPAS;
    if (Output==INVALID_HANDLE_VALUE) Output = GetStdHandle(STD_OUTPUT_HANDLE);
    if (Output==INVALID_HANDLE_VALUE) return ((APIRET16)GetLastError());
    if (CursorInfo->attr == 0xffff) NtCursorInfo.bVisible = FALSE;
    else NtCursorInfo.bVisible = TRUE;
    if ((NtCursorInfo.dwSize = (USHORT)(CursorInfo->yStart + 110)) > 100)
         NtCursorInfo.dwSize = 100;  /*maximum value*/
    if (!SetConsoleCursorInfo(Output, &NtCursorInfo))
        return ((APIRET16)GetLastError());
    return ((APIRET16)NO_ERROR);
}

/*------------------------------*/

APIRET16 VioGetConfig(USHORT Reserved, VIOCONFIGINFO *Info, HVIO Handle)
{
    #undef  MF__
    #define MF__ MOD__"VioGetConfig"
    HDC hDC;
    LONG Col;

    DBGPAS;
    if (!(hDC = CreateCompatibleDC(NULL))) return ((APIRET16)1);
    Col = GetDeviceCaps(hDC, PLANES) * GetDeviceCaps(hDC, BITSPIXEL);
    DeleteDC(hDC);
    if (Col > 1) Info->adapter = Info->display = 1;
    else {
        Info->adapter = DISPLAY_MONOCHROME;     //constants (both 0)
        Info->display = MONITOR_MONOCHROME;     //  defined in os2.h
    }
    return ((APIRET16)NO_ERROR);
}

/*------------------------------*/

APIRET16 VioGetMode(VIOMODEINFO *Data, HVIO Handle)
{
    #undef  MF__
    #define MF__ MOD__"VioGetMode"
    CONSOLE_SCREEN_BUFFER_INFO BufInfo;

    DBGPAS;
    if (Output==INVALID_HANDLE_VALUE) Output = GetStdHandle(STD_OUTPUT_HANDLE);
    if (Output==INVALID_HANDLE_VALUE) return ((APIRET16)GetLastError());
    if (!GetConsoleScreenBufferInfo(Output, &BufInfo))
        return ((APIRET16)GetLastError());
    Data->row = BufInfo.dwSize.Y;
    Data->col = BufInfo.dwSize.X;
    return ((APIRET16)NO_ERROR);
}

/*------------------------------*/

APIRET16 VioGetState(VIOINTENSITY *Intensity, HVIO Handle)
    //called for determining the intensity of console output operations
{
    #undef  MF__
    #define MF__ MOD__"VioGetState"

    CONSOLE_SCREEN_BUFFER_INFO BufInfo;

    DBGPAS;

    if (Output==INVALID_HANDLE_VALUE) Output = GetStdHandle(STD_OUTPUT_HANDLE);
    if (Output==INVALID_HANDLE_VALUE) return ((APIRET16)GetLastError());

    if (!GetConsoleScreenBufferInfo(Output, &BufInfo))
        return ((APIRET16)GetLastError());

	Intensity->fs = BufInfo.wAttributes;

    return ((APIRET16)NO_ERROR);
}

/*------------------------------*/

APIRET16 VioSetState(VIOINTENSITY *Intensity, HVIO Handle)
    //called for changing the intensity of console output operations
{
    #undef  MF__
    #define MF__ MOD__"VioSetState"
    DBGPAS;
    return ((APIRET16)NO_ERROR);
}

/*------------------------------*/

APIRET16 VioSetCurPos(USHORT Row, USHORT Column, HVIO Handle)
{
    #undef  MF__
    #define MF__ MOD__"VioSetCurPos"
    COORD Pos;

    DBGPAS;
    if (Output==INVALID_HANDLE_VALUE) Output = GetStdHandle(STD_OUTPUT_HANDLE);
    if (Output==INVALID_HANDLE_VALUE) return ((APIRET16)GetLastError());
    Pos.X = Column;
    Pos.Y = Row;
    if (!SetConsoleCursorPosition(Output, Pos))
        return ((APIRET16)GetLastError());
    return ((APIRET16)NO_ERROR);
}

/*------------------------------*/

APIRET16 VioScrollUp (USHORT Top, USHORT Left, USHORT Bottom, USHORT Right,
                      USHORT Lines, PBYTE Address, HVIO Handle)
    //function used for clearing screen only
{
    #undef  MF__
    #define MF__ MOD__"VioScrollUp"
    static COORD                Start = {0,0};
    DWORD                       Written;
    CONSOLE_SCREEN_BUFFER_INFO  BufInfo;

    DBGPAS;
    if (Output==INVALID_HANDLE_VALUE) Output = GetStdHandle(STD_OUTPUT_HANDLE);
    if (Output==INVALID_HANDLE_VALUE) return ((APIRET16)GetLastError());

    if (!GetConsoleScreenBufferInfo(Output, &BufInfo))
        return ((APIRET16)GetLastError());

    if ( Lines == (USHORT)-1 )
      Lines = (USHORT)BufInfo.dwSize.Y;

    if (!FillConsoleOutputCharacter(Output, Address[0],
                                    (DWORD)BufInfo.dwSize.X * Lines,
                                    Start, &Written))
        return ((APIRET16)GetLastError());

    FillConsoleOutputAttribute(Output, Address[1], Written, Start, &Written);


    return ((APIRET16)NO_ERROR);
}

/*------------------------------*/

APIRET16 VioWrtCharStrAtt
    (PCHAR Str, USHORT Len, USHORT Row, USHORT Col, PBYTE Attr, HVIO Handle)
{
    #undef  MF__
    #define MF__ MOD__"VioWrtCharStrAtt"
    DWORD Written;
    WORD  wAttr;
    COORD Coord;

    DBGPAS;
    if (Output==INVALID_HANDLE_VALUE) Output = GetStdHandle(STD_OUTPUT_HANDLE);
    if (Output==INVALID_HANDLE_VALUE) return ((APIRET16)GetLastError());
    Coord.X = Col;
    Coord.Y = Row;
    wAttr = *Attr;
    if (!FillConsoleOutputAttribute (Output, wAttr, Len, Coord, &Written))
        return ((APIRET16)GetLastError());
    if (!WriteConsoleOutputCharacter(Output, Str,   Len, Coord, &Written))
        return ((APIRET16)GetLastError());
    return ((APIRET16)NO_ERROR);
}

/*------------------------------*/

APIRET16 VioWrtCellStr
    (PCHAR Str, USHORT Len, USHORT Row, USHORT Col, HVIO Handle)
    //display Str as pairs of character and attribute...
{
    #undef  MF__
    #define MF__ MOD__"VioWrtCellStr"
    SMALL_RECT                 DestRect;
    static COORD               Coord = {0,0};
    COORD                      Size;
    CONSOLE_SCREEN_BUFFER_INFO BufInfo;

    DBGPAS;
    if (Output==INVALID_HANDLE_VALUE) Output = GetStdHandle(STD_OUTPUT_HANDLE);
    if (Output==INVALID_HANDLE_VALUE) return ((APIRET16)GetLastError());

    if (!GetConsoleScreenBufferInfo(Output, &BufInfo))
        return ((APIRET16)GetLastError());

    Len /= sizeof(CHAR_INFO);

    if ( BufInfo.dwSize.X > Len)
      Size.X = Len;
    else
      Size.X = BufInfo.dwSize.X;

    Size.Y = (Len / BufInfo.dwSize.X);

    if (Len % BufInfo.dwSize.X)
      Size.Y++;

    DestRect.Left   = Col;
    DestRect.Top    = Row;
    DestRect.Right  = Col + Size.X - 1;
    DestRect.Bottom = Row + Size.Y - 1;
    if (!WriteConsoleOutput(Output, (CHAR_INFO*)Str, Size, Coord, &DestRect))
        return ((APIRET16)GetLastError());
    return ((APIRET16)NO_ERROR);
}


/*
 * ========================== LOCAL FUNCTIONS =================================
 */


/*
 * =============================== END ========================================
 */
