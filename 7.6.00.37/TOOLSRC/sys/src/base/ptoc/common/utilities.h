(* @(#)common/utilities.h


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

(* utilities.h *)

procedure UtilitiesI;
external;

procedure PasCerror
   (message: STRING);
external;

function BaseType
   (typ: SymbolTableRange)
   : SymbolTableRange;
external;

function CopyTree
   (NodeIndex: HdgRange)
   : HdgRange;
external;

function demorgan
   (NodeIndex: integer)
   : integer;
external;

function GetTemporary
   (ProcedureName: SymbolTableRange;
    TempType: SymbolTableRange;
    TempList: boolean)
   : SymbolTableRange;
external;

procedure FreeTemporary
   (TempIndex: SymbolTableRange);
external;

procedure ClearTemporarys;
external;

(* DK 26.08.88 moved here from optm26.h *)
function BasisTyp
    (index:integer):  integer;
external;

