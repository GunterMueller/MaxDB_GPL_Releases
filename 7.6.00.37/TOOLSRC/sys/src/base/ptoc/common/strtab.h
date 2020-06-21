/* @(#)common/strtab.h

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

/* stab.c */

#define StabLength 32768

#define StringSemanticsOffset	3
#define StringSemantics(StrNdx) StrTable [StrNdx + StringSemanticsOffset]

#ifdef CC

#define StringNextOffset	0
#define StringLenOffset		1
#define StringNoOffset		2
#define StringCharactersOffset	4

#define GetStringPtr(StrNdx) ((char *) (&StrTable [StrNdx + StringCharactersOffset]))
#define GetStringLength(StrNdx) StrTable [StrNdx + StringLenOffset]
#define GetStringNo(StrNdx) StrTable [StrNdx + StringNoOffset]
#define GetStringChar(StrNdx, i) (GetStringPtr (StrNdx) [i])
#define GetFirstChar(StrNdx) GetStringChar (StrNdx, 0)

extern int	StrTable [];
extern int      StrHash [];

extern
StabI ();

extern	int
EnterString ();

extern	int
FindString ();

extern
GetString ();

#else

#define GetStringPtr(StrNdx) _GetStringPtr (StrNdx)
#define GetStringLength(StrNdx) _GetStringLength (StrNdx)
#define GetStringNo(StrNdx) _GetStringNo (StrNdx)
#define GetStringChar(StrNdx, i) _GetStringChar (StrNdx, i)
#define GetFirstChar(StrNdx) _GetFirstChar (StrNdx)

const
   StabLimit = StabLength - 1;

type
   STRINGPTRTYPE = ^char;

var
   StrTable: array [0..StabLimit] of integer;
   StrHash: array [0..255] of integer;

procedure StabI;
external;

function EnterString
   (var strn: char;
    len: integer)
   : integer;
external;

procedure GetString
   (StrNdx: integer;
    var c: char);
external;

function _GetStringPtr
   (StrNdx: integer)
   : STRINGPTRTYPE;
external;

function _GetStringLength
   (StrNdx: integer)
   : integer;
external;

function _GetStringNo
   (StrNdx: integer)
   : integer;
external;

function _GetStringChar
   (StrNdx: integer;
    i: integer)
   : char;
external;

function _GetFirstChar
   (StrNdx: integer)
   : char;
external;

#endif
