/*!
  @file           gos00ds.h
  @author         RaymondR
  @brief          NT to OS2 API
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



#ifndef GOS00DS_H
#define GOS00DS_H


#if defined(_WIN32)
 #define IO_WAIT 0
 #define DISPLAY_MONOCHROME 0
 #define MONITOR_MONOCHROME 0

 typedef unsigned short                            APIRET16;
 typedef unsigned short                            HMOU;
 typedef unsigned short                            *PHMOU;
 typedef struct {USHORT cb; USHORT fsMask;}        KBDINFO, *PKBDINFO;
 typedef struct {UCHAR  fsState;
                 UCHAR  chChar;
                 USHORT chScan;}     KBDKEYINFO, *PKBDKEYINFO;
                 // --- Warning: chScan (in KBDKEYINFO) had
                 //     originally been a UCHAR. The change
                 //     to 16 bits is intended!
 typedef struct {USHORT fs;
                 ULONG  time;
                 SHORT  row;
                 SHORT  col; } MOUEVENTINFO, *PMOUEVENTINFO;

 typedef struct {USHORT yStart; USHORT cEnd; USHORT attr;} VIOCURSORINFO;
 typedef VIOCURSORINFO                             *PVIOCURSORINFO;
 typedef struct {USHORT cb; USHORT type; USHORT fs;} VIOINTENSITY;
 typedef struct {PID idpid; ULONG ulData;}         REQUESTDATA;
 typedef struct {USHORT usType;}                   KBDTYPE;
 typedef struct {USHORT cb; USHORT adapter, display;} VIOCONFIGINFO;
 typedef struct {USHORT cb; USHORT col, row;}      VIOMODEINFO;
 typedef struct X_HVIO                             *HVIO, *HVPS;

 // +-------------------------------------------------------+
 // |  FUNCTION PROTOTYPES (and function specific defines)  |
 // +-------------------------------------------------------+
 APIRET16 KbdCharIn(PKBDKEYINFO, USHORT, HANDLE);
 APIRET16 KbdSetStatus(PKBDINFO, HANDLE);
 APIRET16 KbdGetStatus(PKBDINFO, HANDLE);
 APIRET16 VioGetCurType(PVIOCURSORINFO, HVIO);
 APIRET16 VioGetConfig(USHORT, VIOCONFIGINFO*, HVIO);
 APIRET16 VioGetMode(VIOMODEINFO*, HVIO);
 APIRET16 VioGetState(PVOID, HVIO);
 APIRET16 VioSetCurType(PVIOCURSORINFO, HVIO);
 APIRET16 VioSetState(PVOID, HVIO);
 APIRET16 VioSetCurPos(USHORT, USHORT, HVIO);
 APIRET16 VioScrollUp(USHORT, USHORT, USHORT, USHORT, USHORT, PBYTE, HVIO);
 APIRET16 VioWrtCharStrAtt(PBYTE, USHORT, USHORT, USHORT, PBYTE, HVIO);
 APIRET16 VioWrtCellStr(PBYTE, USHORT, USHORT, USHORT, HVIO);

#endif


#endif  /* GOS00DS_H */
