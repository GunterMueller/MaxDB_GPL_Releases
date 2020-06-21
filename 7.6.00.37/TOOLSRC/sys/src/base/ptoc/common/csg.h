(* @(#)common/csg.h


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

(* csg.h *)

procedure SetMinor
   (NodeIndex: HdgRange;
    opcode: integer);
external;

function node3
   (op, typ, p1, p2: integer)
   : HdgRange;
external;

function UnaryExpression
   (op, typ, op1, p1, p2: integer)
   : HdgRange;
external;

function BinaryExpression
   (op, typ, op1, op2: integer)
   : HdgRange;
external;

procedure SetBinaryParameters
   (NodeIndex: HdgRange;
    p1,
    p2: integer);
external;

procedure AppendList
   (major, typ, exp1, exp2: integer);
external;

procedure InsertList
   (major, typ, exp1, exp2: integer);
external;

procedure LinkLists
   (L1,
    L2: integer);
external;

function MakeList
   (major, typ, op1, op2, op3: integer)
   : HdgRange;
external;

function ExtractList
   (list: HdgRange)
   : HdgRange;
external;

function ComplexStmnt
   (major, typ, op1, op2, op3, LineNo: integer)
   : HdgRange;
external;

function CaseStmnt
   (major, minor, typ, low, high, LineNo: integer)
   : HdgRange;
external;

function special
   (op, typ, p1, LineNo: integer)
   : HdgRange;
external;

function ShortStatement
   (op, typ, op1, LineNo: integer)
   : HdgRange;
external;

function LongStatement
   (op, typ, op1, op2, p1, p2, LineNo: integer)
   : HdgRange;
external;

function ProcNode
   (major, typ, op1, op2, Label, LineNo: integer)
   : HdgRange;
external;

function LinkChain
   (l1, l2: integer)
   : HdgRange;
external;
