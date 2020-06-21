(* @(#)optimizer/gl_consts.i

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

(* gl_consts.i *)

const

   TableSize = 1000;

   SetSize = SymSize;
   SetSizeMinusOne = SetSize - 1;

#ifdef FLEX
   NoWords = (SetSize + 31) div 32;
#else
   NoWords = ((SetSize + 31) div 32 ) div 2;
#endif

   ProcTableSize = 100;

   DELTA = 2;




#define MaxHeap 262136
#if defined (__alpha)   /* DEC 3000/4000 ALPHA , running OSF1 */
#define Overhead 16
#else
#define Overhead 8
#endif
			(* Einheit der Speicher-Verwaltung *)

#ifdef TRACE
#define COUNT_REC(size) count_rec(size)
#else
#define COUNT_REC(size)
#endif

#define newspace 4
#define print_space(x) for i:= 0 to x do write(' ')


#define X_NEW(h,size)       begin h := ptr(x_aktadr);\
			    x_aktadr := x_aktadr + size;\
			    COUNT_REC(size);\
			    if x_aktadr > x_used\
			       then h := ptr(x_new(size)) end

(* 1.5.94
 * #define NEW(h,typ,size)   begin if typ = nil \
 * 			     then X_NEW(h,size)\
 * 			     else begin \
 * 				  h := typ; typ := h^.next;\
 * 				  end ;\
 * 			  h^.next := nil end
 * 1.5.94 *)

(* 1.5.94
 * #define DISPOSE(h,typ)    if h<>nil \
 * 			     then begin \
 * 				  h^.next:=typ; typ:=h; h:=nil;\
 * 				  end;
 * 1.5.94 *)


#ifdef FLEX

#define set_ExitRef(t)  t^.ExitDescriptor.ref  := ptr ( ord4(t) + sizeof(NodeDescriptorRecord) + ExitRef  )
#define set_ExitDef(t)  t^.ExitDescriptor.def  := ptr ( ord4(t) + sizeof(NodeDescriptorRecord) + ExitDef  )
#define set_EntryRef(t) t^.EntryDescriptor.ref := ptr ( ord4(t) + sizeof(NodeDescriptorRecord) + EntryRef )
#define set_EntryDef(t) t^.EntryDescriptor.def := ptr ( ord4(t) + sizeof(NodeDescriptorRecord) + EntryDef )
#define set_Descript(t) set_ExitRef(t);set_ExitDef(t);set_EntryRef(t);set_EntryDef(t)

#define set_Ref(x,y)    x.ref := @y
#define set_Def(x,y)    x.def := @y

#endif


