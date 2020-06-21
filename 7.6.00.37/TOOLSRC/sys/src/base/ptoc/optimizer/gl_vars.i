(* @(#)optimizer/gl_vars.i

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

(* gl_vars.i *)

var
   gl_IntegerPtr,
   gl_AnyPtrPtr: SymbolTableRange;

   BusyDescriptorList,
   FreeDescriptorList: DescriptorRange;

   ProcedureName : SymbolTableRange;

   OptimizedIf , OptimizedLoop , ExprFlag , StmntFlag : boolean;

   ProcTable : ProcTableArray;

   StmntChain,
   StmntLink,
   ExprChain,
   ExprLink: array [TableRange] of HdgRange;
   ExprStmnt: array [TableRange] of TableRange;

   NoStmnt,NoExpr : TableRange;

   ExcludedNode , IncludedNode , HdgEnd ,
	    OptimizeNode : HdgRange ; (* !!!!!!!!!!! mds *)

   opt_glob	:	boolean;
   NoProc	:	integer;

   RedundantStmnts , RedundantExpr , LoopsConverted , FoldExp ,
	    StmntsOutsideLoop , ExprOutsideLoop ,
	    LongStmntOpt , OptIndex , InitialSize ,
	    NoNodesTraversed , PropagatedExp , ExprReduced : integer;
            (* statistics counters *)


   rel_code, sym_range  : RelCodeArray;
   max_index            : SymbolTableRange;
   max_value            : SymbolTableRange;
   NewWords             : SetLimit;
   NewWordsPlus         : SetLimit;
   NewVars              : SymbolTableRange;

   first_time		: boolean;

#ifdef FLEX
   ExitRef              : integer;
   ExitDef              : integer;
   EntryRef             : integer;
   EntryDef             : integer;
#endif

	x_used          : integer;
	x_aktadr        : integer;
	x_heap          : Pmolloch;
	x_aktheap       : Pmolloch;

	used_space      : integer;
	Maxused_space   : integer;
	rest		: integer;
	Maxrest		: integer;

	s_desc		: integer;
	Anz_desc	: integer;
	MaxAnz_desc	: integer;
