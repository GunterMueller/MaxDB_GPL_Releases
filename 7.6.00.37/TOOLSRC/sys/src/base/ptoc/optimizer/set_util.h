(* @(#)optimizer/set_util.h


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

(* set_util.h *)

#ifdef FLEX

procedure Union_Sets (a,b,c: PSet);
external;

procedure Intersect_Sets (a,b,c: PSet);
external;

procedure Insert_val_into_Set (value: SetElementType; a: PSet);
external;

function Member_in_Set (value: SetElementType; a: PSet): boolean;
external;

procedure Assign_Set_to_Set (a,b: PSet);
external;

function Empty_check_Set ( a: PSet): boolean;
external;

function Equal_two_Sets ( a,b: PSet): boolean;
external;

procedure Clear_this_Set ( a: PSet);
external;

procedure ClearFull ( a: PSet);
external;

procedure Fill_whole_Set ( a: PSet);
external;

procedure PrintSet ( a: PSet);
external;


#else

procedure Union_Sets (var a,b,c: Set);
external;

procedure Intersect_Sets (var a,b,c: Set);
external;

procedure Insert_val_into_Set (value: SetElementType; var a: Set);
external;

function Member_in_Set (value: SetElementType; var a: Set): boolean;
external;

procedure Assign_Set_to_Set (var a,b: Set);
external;

function Empty_check_Set (var a: Set): boolean;
external;

function Equal_two_Sets (var a,b: Set): boolean;
external;

procedure Clear_this_Set (var a: Set);
external;

procedure Fill_whole_Set (var a: Set);
external;

procedure PrintSet (var a: Set);
external;

#endif



function   give_MrX            :integer;   external;

procedure print_name (ind:integer) ; external;

procedure  c_freopen       (i:integer);                            external;
