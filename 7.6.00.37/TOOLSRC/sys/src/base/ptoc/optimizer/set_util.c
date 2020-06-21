/* @(#)optimizer/set_util.c

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

 */

/* set_builder.c */
#include <string.h>

#include "../common/symtab.ci"
#include "../common/_strings.ch"
#ifdef TRACE
#include <stdio.h>
#include "../optimizer/alloc_comdef.i"
#endif

# define SetSize SymSize
# define NoWords ((SetSize + 31) / 32)

typedef int Set [NoWords];

extern max_index;
extern max_value;
extern NewWords;
extern NewWordsPlus;
extern rel_code[];
extern sym_range[];

Union_Sets (a, b, c)
   Set a, b, c; {

   int i;
   int bis;

   bis = NewWords;
   for (i = 0; i < bis; i++) {
      a [i] = b [i] | c [i];
      }
   }


Intersect_Sets (a, b, c)
   Set a, b, c; {

   int i;
   int bis;

   bis = NewWords;
   for (i = 0; i < bis; i++) {
      a [i] = b [i] & c [i];
      }
   }


Insert_val_into_Set (value, a)
   int value;
   Set a; {

   int word_index;
   int rel;

   if (value > max_value)
	value = max_value;
   if (value >= 0) {
	rel = rel_code[value];
	word_index = rel / 32;
	a [word_index] = a [word_index] | (1 << (rel & 31));
      }
   else
      { cgenerror("bad argument - insert\n");
      }
   }


int
Member2_in_Set (value, a)
   int value;
   Set a;
{
   if (value >= 0) {
      return (a [value / 32] & (1 << (value & 31))) != 0;
      }
   else {
      cgenerror("bad argument - member\n");
      }
}


int
Member_in_Set (value, a)
   int value;
   Set a;
{
   int rel;

   if (value > max_value)
	value = max_value;
   if (value >= 0) {
      rel = rel_code[value];
      return (a [rel / 32] & (1 << (rel & 31))) != 0;
      }
   else {
      cgenerror("bad argument - member\n");
      }
   }


Assign_Set_to_Set (a, b)
   Set a,b;
{
   int i;
   int bis;

   bis = NewWords;
   for(i = 0; i < bis; i++) {
      a[i] = b[i];
      }
}


int
Empty_check_Set (a)
   Set a;
{
   int i;
   int bis;

   bis = NewWords;
   for(i = 0; i < bis; i++) {
      if (a[i])
	 return(0);
      }
   return(1);
}


int
Equal_two_Sets (a,b)
   Set a,b;
{
   int i;
   int bis;

   bis = NewWords;
   for(i = 0; i < bis; i++) {
      if (a[i] != b[i])
	 return(0);
      }
   return(1);
}


Clear_this_Set (a)
   Set a;
{
   int i;
   int bis;

   bis = NewWordsPlus;
   for(i = 0; i < bis; i++) {
      a[i] = 0;
      }
}


ClearFull (a)
   Set a;
{
   int i;
   int bis;

   bis = 64;
   for(i = 0; i < bis; i++) {
      a[i] = 0;
      }
}


Fill_whole_Set (a)
   Set a;
{
   int i;
   int bis;

   bis = NewWordsPlus;
   for(i = 0; i < bis; i++) {
      a[i] = -1;
      }
}


PrintSet (a)
   Set a;
{
   int i, first, last;
   int bis;

	 bis = max_index;
	 first = -1;
	 last = -1;
	 for (i = 0; i < bis ;i++)  {
	    if ( Member2_in_Set (i, a) ) {
		   if (first == -1) {
		      first = i;
		      last = i;
		      printf (" %d", sym_range[i]);
		   }
		   else  {
		      if (i == last + 1)
			   last = i;
		      else {
			   if (first != last) {
			      if (first + 1 == last) {
				  printf ( " %d", sym_range[last]);
				  }
			      else {
				  printf ( "..%d", sym_range[last]);
				  }
			      }
			   printf ( " %d", sym_range[i]);
			   first = i;
			   last = i;
			   }
		   }
	    }  /* if */
	 }  /* for */
	 if (first != last) {
	    if (first + 1 == last) {
	       printf ( " %d", sym_range[last]);
	    }
	    else {
	       printf ( "..%d", sym_range[last]);
	    }
	 }
      }  /* PrintSet */


   SymbolTableRange
   Ent_Id_only(name,type,kind)           /* function */
   /*******************************************/
   int name;
   SymbolTableRange type;
   KINDTYPE kind;
   {
	BumpSymPtr();

	/* Setting the three dummy fields will take care of all the kinds. */
	STab[SymPtr].DummyInfo.dummy1 = -1;
	STab[SymPtr].DummyInfo.dummy2 = -1;
	STab[SymPtr].DummyInfo.dummy3 = -1;

	STab[SymPtr].SName = name;
	STab[SymPtr].SCard = -1;
	STab[SymPtr].SFile = '\0';
	STab[SymPtr].SType = type;
	STab[SymPtr].SLevel = 0;
	STab[SymPtr].SKind = kind;
	STab[SymPtr].SLink = -1;
	STab[SymPtr].SFlags = 0;
	return(SymPtr);
   }


   give_MrX()                           /* function */
   /*******************************************/
   {
	STRING name;
	int    id;
	int    sym;
	static int MrX_done = 0;
	extern int IntegerPtr;

	if (!MrX_done)  {
	   sprintf(name,"MrX");
	   id = EnterString (name, strlen(name));
	   sym = Ent_Id_only(id, IntegerPtr, VarDcl);
	   MrX_done = sym;
	   }
	return (MrX_done);
   }


   int
   print_name(i)                        /* function */
   /*******************************************/
   int i;
   {
	char str[256];
	GetString(STab[i].SName, str);
	if ( str[0] != '\0' ) {
	   printf("%s" ,str);
	}
	else {
	   printf("<%d>" ,i);
	}
	return(i);
   }


#ifdef TRACE

   c_freopen(i)                         /* function */
   /*******************************************/
   int i;
   {
	switch (i)  {
	case TRACE_FILE:
		freopen("trace" , "a" , stdout);
		break;
	case REG_FILE:
		freopen("regs" , "a" , stdout);
		break;
	case TERMINAL:
		freopen("/dev/tty" , "a" , stdout);
		break;
	case KEY:
		freopen("/dev/tty" , "r", stdin);
		break;
	case DEVNULL:
		freopen("/dev/null" , "a", stdout);
		break;
	case OUT_FILE:
		freopen("outs" , "a", stdout);
		break;
	case LS_FILE:
		freopen("loads" , "a", stdout);
		break;
	case CONV_FILE:
		freopen("conv" , "a", stdout);
		break;
	}
	fflush(stdout);
   }

#endif


