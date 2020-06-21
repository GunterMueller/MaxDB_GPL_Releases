(* @(#)optimizer/temp_util.h


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

(* temp_util.h *)


procedure gl_InitTemps;
external;

function gl_TempAvail
   (TempIndex,
    TempType: SymbolTableRange)
   : boolean;
external;

function gl_GetTemporary
   (TempType: SymbolTableRange)
   : SymbolTableRange;
external;

procedure gl_FreeTemp
   (Temp: SymbolTableRange);
external;

procedure gl_PushNestLevel;
external;

procedure gl_PopNestLevel
#ifdef FLEX
   (DefSet: PSet);
#else
   (var DefSet: Set);
#endif
external;


procedure init_optimizer;  	external;
procedure close_optimizer;  	external;
procedure init_proc;  		external;
procedure close_proc;  		external;
procedure reset_heapmanager;	external;

function x_new(size : integer) : integer; external;

function not_SetConst(index: integer): boolean; external;



#ifdef TRACE

procedure count_rec ( size : integer ); external;

procedure new_max;                           external;

procedure print_HDG (NodeIndex : HdgRange);        external;

function ZeilenNr (NodeIndex : HdgRange): integer;           external;

procedure SelectForPrinting
   (NodeIndex : HdgRange; space: integer; nl: boolean);      external;

#endif
