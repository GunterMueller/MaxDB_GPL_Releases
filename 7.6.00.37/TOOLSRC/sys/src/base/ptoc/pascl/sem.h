(* @(#)pascl/sem.h


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

(* sem.h *)

procedure EvalBinary
   (p1,
    p2: ParseStackRange;
    op: integer);
external;

procedure EvalDivide
   (p1,
    p2: ParseStackRange);
external;

procedure EvalModDiv
   (p1,
    p2: ParseStackRange;
    op: integer);
external;

procedure EvalRelational
   (p1,
    p2: ParseStackRange;
    op: integer);
external;

procedure EvalIn
   (p1,
    p2: ParseStackRange);
external;

function AssgnmntCompatible
   (typ1,
    exp2: SymbolTableRange)
   : boolean;
external;

procedure EvalUnary
   (p1: ParseStackRange;
    op: integer);
external;

function EvalSetExp
   (ResultIndex,
    ExprIndex: HdgRange)
   : HdgRange;
external;

function EvalStringExp
   (ResultIndex,
    ExprIndex: HdgRange)
   : HdgRange;
external;

procedure EvalSetConstructor
   (p1,
    p2: integer);
external;

function RangeCheck
   (typ: SymbolTableRange;
    NodeIndex: HdgRange)
   : HdgRange;
external;

function GenStore
   (lhs: HdgRange;
    ltype,
    lstype: SymbolTableRange;
    lcol: integer;
    rhs: HdgRange;
    rtype,
    rstype: SymbolTableRange;
    rcol: integer;
    card: integer)
   : HdgRange;
external;

procedure GenCall
   (symp: SymbolTableRange;
    p1,
    p2: ParseStackRange);
external;

procedure GenAddress
   (p1,
    p2: ParseStackRange);
external;

procedure AccessVariable
   (symp: SymbolTableRange;
    p1,
    p2: ParseStackRange);
external;

procedure IndexedVar
   (p1,
    p2: integer);
external;

procedure EvalWith
   (p1,
    p2: integer);
external;

procedure EndWith
   (p1: integer);
external;

procedure QualifiedVar
   (p1,
    p2: integer);
external;

procedure RefVar
   (p1: integer);
external;

procedure DerefVar
   (p1: integer);
external;

procedure BuildCaseConst
   (p1,
    p2: integer);
external;

function BuildCase
   (p1,
    p2: integer)
   : HdgRange;
external;

function BuildDefault
   (p1,
    p2: integer)
   : HdgRange;
external;

function LabDef
   (p1: integer)
   : integer;
external;

function LabRef
   (p1: integer)
   : integer;
external;

procedure EvalTo
   (f,
    v,
    e1,
    e2: ParseStackRange);
external;

procedure EvalDownto
   (f,
    v,
    e1,
    e2: ParseStackRange);
external;

procedure EvalFor
   (p1,
    p2: HdgRange);
external;

function EvalProgramParameters
   (p1: ParseStackRange)
   : HdgRange;
external;

function EvalAssert
   (p1, p2: ParseStackRange)
   : HdgRange;
external;
