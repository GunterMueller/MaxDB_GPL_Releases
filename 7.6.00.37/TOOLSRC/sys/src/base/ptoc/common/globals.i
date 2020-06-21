(* @(#)common/globals.i

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

(* globals.i *)

#include "_strings.h"
#include "symtab.i"
#include "trees.i"

var
   ErrorCount: integer;
   control: array [char] of boolean;
   control2: array [char] of boolean;
#ifdef RAS /*RAS00 def. own glob.vars */
   RAS_control: array [char] of boolean;
   MacNames: STRING;
#endif
   FillByte: char;
   FillWord: integer;
   FillForVar: integer;
   WOptSize: integer;
