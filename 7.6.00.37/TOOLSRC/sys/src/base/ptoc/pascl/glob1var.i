(* @(#)pascl/glob1var.i

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

(* glob1var.i *)

var
   IntegerPtr, BooleanPtr, CharPtr, TextPtr, FilePtr, PacPtr: SymbolTableRange;
   LongPtr, FloatPtr, DoublePtr: SymbolTableRange;
   UbytePtr,
   UshortPtr: SymbolTableRange;
   BytePtr, ShortPtr, StringPtr: SymbolTableRange;
   WFormatPtr: SymbolTableRange;			/* EF4: 01-11-86 WB */
   IntsetIndex: integer;

   (* pointers used to maintain scope info in symbol table *)
   ProcName: ProcNameType;
   ProcMark: integer;
   ProcIndex: integer;
   parct: integer;
   ReturnedType: integer;
   LevelNo: integer;
   mp:               integer;    (* points into parse stacks *)
   StartCnt:         ParseStack; (* start of IdList *)
   cnt:              ParseStack; (* no. of elements in list *)
   g1 :              ParseStack; (* points to hdg *)
   g2 :              ParseStack; (* auxillary g1 stack *)
   val:              ParseStack; (* value of token *)
   TypeType:         ParseStack; (**)
   SubType:          ParseStack; (**)
   SaveReturnedType: ParseStack; (**)
   SaveParct:        ParseStack; (* save info between scope levels *)
   SaveProcmark:     ParseStack; (* '' *)
   SaveProcindex:    ParseStack; (* '' *)		(* EG5: 02-07-86 WB *)
   CardNum:          ParseStack; (* card on which token was found *)
   ColNo:            ParseStack; (* token column *)
   WithRecs:         ParseStack;
   NameofType:	     ParseStack;	
   NameofConst:	     ParseStack;	

   ParmPtr: integer;
   ParmId: ParmTable;        (* points into string table *)
   ParmType: ParmTable;      (* specifies parameter type *)
   ParmNameofType:ParmTable; (* Name of parameter type: PTOC *)
   ParmPassedBy: array [ParmTableRange] of char;  (* specifies how parameters are passed *)

   ExpPtr:          integer;
   ExpType:         IntExpList;
   ExpStype:        IntExpList;
   ExpColNo:        IntExpList;
   ExpVal:          IntExpList;

   IdPtr: integer;
   IdList: array[0..IdListSize] of integer;  (* points to string table *)

   (* pointers used to link fields of a record in a symbol table *)

   HeadFieldLink : integer;
   TailFieldLink : integer;

   CardCount: integer;
   CurrentFile: integer;

   CaseRoot: HdgRange;

   ConstVal: IntConstList;
   TagType, StartConst: IntConstList;
   ConstPtr, TagPtr: integer;
   LabelStack: LabelStackType;
   LabelPtr, LabelScopePtr, ForWithLevel: integer;

   asm_esc: boolean;
