(* @(#)optimizer/optm1.h


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

(* optm1.h *)

procedure gl_InitSym;
external;

procedure PrintDescriptor
   (NodeIndex: HdgRange;
    msg: STRING;
    var desc1: NodeDescriptorType);
external;

procedure ListDescTab;
external;

function AvailableDescriptor
   : DescriptorRange;
external;

procedure FreeDescriptor
   (desc: DescriptorRange);
external;

procedure ClearDescriptor
   (var descriptor : NodeDescriptorType);
external;

procedure ClearDescriptorFull
   (var descriptor : NodeDescriptorType);
external;

procedure ClearEntryDescriptors;
external;

procedure ClearExitDescriptors;
external;

procedure UnionDescriptors
   (var desc,
    desc1,
    desc2: NodeDescriptorType);
external;

procedure AssignDescriptors
   (var desc1,
    desc2: NodeDescriptorType);
external;

function  EqualDescriptors
   (var desc1,
    desc2: NodeDescriptorType)
    : boolean;
external;

function DescriptorFlags
   (var desc1: NodeDescriptorType)
   : boolean;
external;

function IsOptimizible
   (NodeIndex: HdgRange)
   : boolean;
external;

function IsLvalOp
   (NodeIndex: HdgRange)
   : boolean;
external;

function CreateAddrOfLval
   (NodeIndex: HdgRange)
   : HdgRange;
external;

function CreateAssignmentStatement
   (PtrToExpr,
    NextStmnt: HdgRange;
    target: SymbolTableRange;
    IndexExp: HdgRange)
   : HdgRange;
external;

function CreateLoadNode
   (source: SymbolTableRange)
   : HdgRange;
external;
