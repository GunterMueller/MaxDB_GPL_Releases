(* @(#)pascl/sysprocs.h


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


*)

(* sysprocs.h *)

const
   SysFirst	= 0;
   SysReset	= 0;
   SysRewrite	= 1;
   SysRead	= 2;
   SysWrite	= 3;
   SysFlush	= 4;
   SysRemove	= 5;
   SysMessage	= 6;
   SysOrd	= 7;
   SysChr	= 8;
   SysReadln	= 9;
   SysWriteln	= 10;
   SysEoln	= 11;
   SysEof	= 12;
   SysArgc	= 13;
   SysArgv	= 14;
   SysNew	= 15;
   SysDispose	= 16;
   SysPut	= 17;
   SysGet	= 18;
   SysPage	= 19;
   SysAbs	= 20;
   SysArctan	= 21;
   SysCos	= 22;
   SysExp	= 23;
   SysLn	= 24;
   SysOdd	= 25;
   SysPack	= 26;
   SysPred	= 27;
   SysRound	= 28;
   SysSin	= 29;
   SysSqr	= 30;
   SysSqrt	= 31;
   SysSucc	= 32;
   SysTrunc	= 33;
   SysUnpack	= 34;
   SysHalt	= 35;
   SysClock	= 36;
   SysSysclock	= 37;
   SysWallclock	= 38;
   SysPtr	= 39;
   SysDate	= 40;
   SysTime	= 41;
   SysLinelimit	= 42;
   SysNull	= 43;
   SysCard	= 44;
   SysExpo	= 45;
   SysRandom	= 46;
   SysSeed	= 47;
   SysUndefined	= 48;
   SysLength	= 49;
   SysConcat	= 50;
   SysCopy	= 51;
   SysOrd2	= 52;
   SysOrd4	= 53;
   SysSizeof	= 54;
   SysClose	= 55;
   SysEnvp	= 56;
   SysAsm 	= 57;
   SysLast	= 57;

var
   SysSproc: array [SysFirst..SysLast] of packed array [0..19] of char;

#ifdef PYRAMID_DOUBTFUL
#else
var SaveIndex: HdgRange;
#endif

procedure SysprocsI;
external;

function CallPcstart
   (mode,
    card: integer)
   : HdgRange;
external;

function CallPcexit
   (card: integer)
   : HdgRange;
external;

function CallFnil
   (NodeIndex: HdgRange;
    ComponentType: SymbolTableRange)
   : HdgRange;
external;

function CallCompare
   (p1,
    p2: SymbolTableRange)
   : HdgRange;
external;

function CallSetOp
   (op: integer;
    typ: SymbolTableRange;
    res,
    left,
    right: HdgRange)
   : HdgRange;
external;

function CallSetRel
   (op: integer;
    typ: SymbolTableRange;
    left,
    right: HdgRange)
   : HdgRange;
external;

function CallInct
   (element,
    ct: HdgRange)
   : HdgRange;
external;

function CallIntor
   (element,
    NodeIndex: HdgRange)
   : HdgRange;
external;

function CallInt
   (element: HdgRange;
    typ: SymbolTableRange;
    right: HdgRange)
   : HdgRange;
external;

function CallCttot
   (typ: SymbolTableRange;
    res,
    ct: HdgRange)
   : HdgRange;
external;

function CallConvertt
   (res,
    src: HdgRange)
   : HdgRange;
external;

function CallStringCopy
   (result,
    source,
    first,
    size: HdgRange)
   : HdgRange;
external;

function CallStringConcat
   (result,
    left,
    right: HdgRange)
   : HdgRange;
external;

function CallStringCompare
   (Exp1,
    Exp2: HdgRange)
   : HdgRange;
external;

function CallRange
   (value: HdgRange;
    lower,
    upper: integer)
   : HdgRange;
external;

function CallNilCheck
   (p1 : ParseStackRange)
   : HdgRange;
external;

function EvalGoto
   (LabIndex: SymbolTableRange)
   : HdgRange;
external;

function EvalDefname
   (id: SymbolTableRange)
   : HdgRange;
external;

function IsProcedure
   (sysproc: integer)
   : boolean;
external;

procedure EvalSys
   (ProcOrFunc,
    sysproc,
    p1,
    p2: integer);
external;
