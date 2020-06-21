(* @(#)optimizer/gl_types.i

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

(* gl_types.i *)

type

   TableRange = 1..TableSize;

   SetLimit = 1..NoWords;
   SetElementType = 0..SetSizeMinusOne;
   Set = array [SetLimit] of integer;
#ifdef FLEX
   PSet = ^Set;
#endif

   OpcodeRange          = 0 .. LastOp;

   NodeDescriptorType  = record
#ifdef FLEX
      ref       : PSet;
      def       : PSet;
#else
      ref       : Set;
      def       : Set;
#endif
      continue  : boolean;
      break     : boolean;
      return    : boolean;
      goto_label: boolean;
      hash_code : integer;
   end;

   DescriptorRange = ^NodeDescriptorRecord;
   NodeDescriptorRecord = record
      prev,
      next: DescriptorRange;
      essential: boolean;
      ExitDescriptor: NodeDescriptorType;
      EntryDescriptor: NodeDescriptorType;
   end;

   ProcTableType       = record
                                   SymTabInx     : SymbolTableRange;
				/*
                                 * HdgFileOffset : integer;
				 * Size          : integer;
				 */
				   ProcExitDescr : NodeDescriptorType;
                                end;
   ProcTableArray      = array[0..ProcTableSize] of ProcTableType;

   TraversalType = (ExprTraversal, StmntTraversal);


   RelCodeArray         = array [ 0 .. SetSizeMinusOne ] of SymbolTableRange;




	/* heap manager */

	Pchar =
		^char;

	Pmolloch =
		^Tmolloch;

	Tmolloch =
		record
		next         : Pmolloch;
		arr          : array [ 1 .. MaxHeap ] of char;
		molloch_size : integer;
		end;

