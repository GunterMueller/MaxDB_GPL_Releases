/* @(#)common/commonalloc.c

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

#ifdef WIN32
#pragma warning (disable : 4033) /*RASE err. 1a: warnings.level 2*/
#endif

#include <sys/types.h>
#include <stdlib.h>
#include "globals.ci"
#include "strtab.h"

#if OLD
extern	caddr_t	sbrk ();
#endif

long int * MALLOC ();

#ifndef PYRAMID_DOUBTFUL

struct HdgArray {
	int *head;
	struct HdgArray *next;
	};

#define HDGNULL (struct HdgArray *)0
struct HdgArray *current_Hdg = HDGNULL;
struct HdgArray *current_Hdg_head = HDGNULL;

InitHdg(){
	current_Hdg = (struct HdgArray *) MALLOC (HdgSize * sizeof (int) , 3);
	current_Hdg->next = HDGNULL;
	current_Hdg->head = (int *)current_Hdg + sizeof (struct HdgArray);
	current_Hdg_head = current_Hdg;
}

FreeHdgArrays(array_ptr)
struct HdgArray *array_ptr;
{
	if ( array_ptr != HDGNULL ) {
	   FreeHdgArrays(array_ptr->next);
	   FREE(array_ptr,3);
	}
}

ResetHdgArray(){
	FreeHdgArrays( current_Hdg_head );
	current_Hdg = HDGNULL;
	current_Hdg_head = HDGNULL;
}
#endif /* PYRAMID_DOUBTFUL */

TestOvflw (node_size)
	int	node_size;
{
	static	int	HdgLimit = HdgSize;
	int	*new_space;

#ifdef PYRAMID_DOUBTFUL
	if (ptrg >= HdgLimit - node_size) {
		new_space = (int *) sbrk (HdgSize * sizeof (int));
		if ((int) new_space != -1) {
			ptrg = new_space - field;
			HdgLimit = ptrg + HdgSize;
		}
		else {
			cerror ("out of memory - TestOvflw\n");
		}
	}
#else
   	if (current_Hdg_head == HDGNULL) {
      	   InitHdg();
	   ptrg = current_Hdg->head - field + 1;
#   if BIT64
	   /* field[ptrg+5] might be cast to a long or a double, so it must be
	    * 8-byte aligned
	    */
	   if ((long)&field[ptrg+5] & 0x7) ptrg++;
#   endif
	   HdgLimit = ptrg + HdgSize - 1 - sizeof(struct HdgArray);
	   return;
	}
#   if BIT64
	/* field[ptrg+5] might be cast to a long or a double, so it must be
	 * 8-byte aligned
	 */
	if ((long)&field[ptrg+5] & 0x7) ptrg++;
#   endif
	if (ptrg >= HdgLimit - node_size) {
	   if ( current_Hdg->next == HDGNULL ) {
		current_Hdg->next =
			(struct HdgArray *) MALLOC (HdgSize * sizeof (int) , 3);
		current_Hdg = current_Hdg->next;
		current_Hdg->next = HDGNULL;
		current_Hdg->head = (int *)current_Hdg +sizeof(struct HdgArray);
	   }
	   else {
		current_Hdg = current_Hdg->next;
	   }
	   ptrg = current_Hdg->head - field + 1;
	   HdgLimit = ptrg + HdgSize - 1 - sizeof(struct HdgArray);
	}
#endif /* PYRAMID_DOUBTFUL */
}


BumpSymPtr ()
{
	static	int	SymLimit = SymLast;
	static	int	NextSym = 0;
	SYMTAB	*new_space;

	SymPtr = NextSym;
	if (NextSym < SymLimit) {
		NextSym += 1;
	}
	else {
#if OLD
		new_space = (SYMTAB *) sbrk ((SymSize + 1) * sizeof (SYMTAB));
		if ((int) new_space != -1) {
#else
		new_space = (SYMTAB *) calloc ((SymSize + 1) , sizeof (SYMTAB));
		if ( new_space ) {
#endif
			/*
			 *  IST 1995-01-19
			 *  The symbol table 'STab' will have big gaps!
			 *  Therefore we MUST NOT scan the list by increasing
			 *  indexes. Instead, we have to use the 'SNext' field,
			 *  which is the index of the next entry and may be
			 *  much bigger than the current one
			 *  (Not always current+1).
			 */
			NextSym = (new_space - STab) + 1;
			SymLimit = NextSym + SymSize - 1;
		}
		else {
			cerror ("out of memory - BumpSymPtr\n");
		}
	}
	STab [SymPtr].SNext = NextSym;
}


static long int *free_big = (long int *)0;
static long int *free_small = (long int *)0;

long int * MALLOC ( size , from_where )
int size;
int from_where;
{
	long int *ret_val = (long int*) 0;

	switch (from_where) {
	   case ( 1 ) : {
/*
	      printf ("Symboltabelle\t");
*/
	      break;
	   }
	   case ( 2 ) : {
/*
	      printf ("CodeSequences\t");
*/
	      if (free_big) {
		 ret_val = free_big;
		 free_big = (long int *) *free_big;
	      }
	      break;
	   }
	   case ( 3 ) : {
/*
	      printf ("Hdg          \t");
*/
	      if (free_small) {
		 ret_val = free_small;
		 free_small = (long int *) *free_small;
	      }
	      break;
	   }
	   case ( 4 ) : {
/*
	      printf ("datafile     \t");
*/
	      break;
	   }
	   case ( 5 ) : {
/*
	      printf ("Optimizer    \t");
*/
	      if (free_big) {
		 ret_val = free_big;
		 free_big = (long int *) *free_big;
	      }
	      break;
	   }
	}

	if (!ret_val)
	   ret_val = (long int *) malloc(size);
	if (!ret_val)
	   cerror("MALLOC could not get space\n");
/*
	printf("   %d  (adr = %lx)\n" , size, ret_val );
*/
	return ( ret_val );
}


FREE ( p, from_where )
long int *p;
{
/*
	printf("FREE  ");
*/
	switch (from_where) {
	   case ( 1 ) : {
/*
	      printf ("Symboltabelle\t");
*/
	      break;
	   }
	   case ( 2 ) : {
/*
	      printf ("CodeSequences\t");
*/
	      *p = (long int) free_big; /* J.Bruehe 91-Aug-01  Was cast "int *" */
	      free_big = p;
	      break;
	   }
	   case ( 3 ) : {
/*
	      printf ("Hdg          \t");
*/
	      *p = (long int) free_small; /* J.Bruehe 91-Aug-01  Was cast "int *" */
	      free_small = p;
	      break;
	   }
	   case ( 4 ) : {
/*
	      printf ("datafile     \t");
*/
	      break;
	   }
	   case ( 5 ) : {
/*
	      printf ("Optimizer    \t");
*/
	      *p = (long int) free_big; /* J.Bruehe 91-Aug-01  Was cast "int *" */
	      free_big = p;
	      break;
	   }
	 }
/*
	 printf("(adr = %lx)\n" , p);
*/
}
