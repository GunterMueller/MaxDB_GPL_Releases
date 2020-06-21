(* @(#)pascl/semutil.h


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

(* semutil.h *)

procedure StackError
   (p: ParseStackRange);
external;

function IsOrdinal
   (t: SymbolTableRange)
   : boolean;
external;

function IsIntegral
   (t: SymbolTableRange)
   : boolean;
external;

function IsEnum
   (t: SymbolTableRange)
   : boolean;
external;

function IsFile
   (t: SymbolTableRange)
   : boolean;
external;

function IsTextFile
   (t: SymbolTableRange)
   : boolean;
external;

function IsPac
   (t: SymbolTableRange)
   : boolean;
external;

function IsCharFile
   (t: SymbolTableRange)
   : boolean;
external;

function IsCharArray
   (t: SymbolTableRange)
   : boolean;
external;

function LengthOfCharArray
   (t: SymbolTableRange)
   : integer;
external;

function IsLval
   (NodeIndex: HdgRange)
   : boolean;
external;

function IsSet
   (typ: SymbolTableRange)
   : boolean;
external;

function IsPointer
   (typ: SymbolTableRange)
   : boolean;
external;

function IsStructured
   (typ: SymbolTableRange)
   : boolean;
external;

function IsCallRange
   (NodeIndex: HdgRange)
   : boolean;
external;

function SkipCallRange
   (NodeIndex: HdgRange)
   : HdgRange;
external;

procedure CharToPac
   (NodeIndex: HdgRange);
external;

function IsString
   (typ: SymbolTableRange)
   : boolean;
external;

function LengthOfString
   (t: SymbolTableRange)
   : integer;
external;

procedure PacToString
   (NodeIndex: HdgRange);
external;

procedure CharToString
   (NodeIndex: HdgRange);
external;

function EvalConvert
   (NewType: SymbolTableRange;
    NodeIndex: HdgRange)
   : HdgRange;
external;

#define check_call(x,col)\
   if (x <> -1) and (MinorOperatorOf (x) = CallOp) then CheckCall (x, col)

procedure CheckCall
   (NodeIndex: HdgRange;
    col: integer);
external;

function CompatibleTypes
   (ptyp1,
    ptyp2: SymbolTableRange)
   : boolean;
external;

function ContainsFile
   (p : SymbolTableRange) : boolean;
external;

function FileInRec
   (p : SymbolTableRange) : boolean;
external;

function IsNegative
   (NodeIndex : HdgRange) : boolean;
external;

procedure FillVar
   (six: SymbolTableRange);
external;
