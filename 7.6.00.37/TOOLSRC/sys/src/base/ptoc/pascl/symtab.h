(* @(#)pascl/symtab.h


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

(* symtab.h *)

var
   InputPtr,
   OutputPtr,
   ErrorPtr,
   _ArgcPtr,
   _ArgvPtr,
   _EnvpPtr,
   ArgcPtr,
   ArgvPtr,
   EnvpPtr,
   TypeIorechd,
   TypePtrIorechd,
   TypeFbuf,
   ElementFbuf,
   AnyPtrPtr,
   PtrToCharPtr,
   AnyPacPtr,
   AnyPacLenPtr,
   AnyFilePtr,
   AnySetPtr: SymbolTableRange;

   BlankString,
   OctString,
   HexString: integer;


procedure SymtabI;
external;

function PrimType
   (t: SymbolTableRange)
   : SymbolTableRange;
external;

procedure ChkEqualType (p1, p2: integer);
external;

procedure IdPtrIncr;
external;

procedure ParmPtrIncr;
external;

procedure IncrExpPtr;
external;

procedure IncrConstPtr;
external;

procedure IncrLabelPtr;
external;

procedure MoveStacks (From, To: integer);
external;

procedure PopScope (p: integer);
external;

procedure upscope (p: integer);
external;

procedure CloseRec;
external;

procedure OpenRec;
external;

procedure NewSymbol
   (id, typ: integer;
    kind: KINDTYPE;
    level, card: integer);
external;

function FindSymbol
   (id : integer)
   : integer;
external;

function FindQSymbol
   (rec,
    id : integer)
   : integer;
external;

procedure EntSymbol
   (id,
    typ: integer;
    kind: KINDTYPE;
    card: integer);
external;

function EntModule
   (p: ParseStackRange;
    ModuleKind: KINDTYPE):integer;
external;

procedure EntBlock
   (p: ParseStackRange);
external;

procedure EntConstant (id, p: integer);
external;

procedure EntVariable (p1, p2, p3: integer);
external;

procedure LabelName
   (lab: integer;
    var name: STRING;
    var len: integer);
external;

procedure EntField (p1, p2: integer);
external;

procedure EntVariant (p1, p2: integer);
external;

procedure EntLabel (p1: integer);
external;

procedure InitSystem;
external;

procedure ChkTypeDefn (t:integer; p1, p2, p3: ParseStackRange); /*DK 26.08.88*/
external;

procedure TypName (p1, p2: integer);
external;

procedure CheckType (p: integer);
external;

procedure CheckForwardDefn (p: integer);
external;

function DclProc
   (p1,
    p2: ParseStackRange;
    kind: KINDTYPE;
    result: SymbolTableRange):integer;
external;

function EntParameterList
   (p1,						(* PTOC *)
    p2,
    p3: ParseStackRange;
    col: integer)
   : integer;
external;

procedure FormalProcType
   (p1,
    p2: ParseStackRange;
    prim_type,
    tnam,
    res_typ: SymbolTableRange);
external;

function EntProcParm
   (p1: ParseStackRange)
   : integer;
external;

function IdLookup (p: integer): integer;
external;

function QidLookup (p1, p2: integer): integer;
external;

function FindIntset
   : SymbolTableRange;
external;

procedure CheckCaseConst (p1: integer);
external;
