/* @(#)pgram/f.decls.h

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


# define push_value\
	++sp,\
	val [sp] = yylval,\
	ColNo [sp] = yyecol,\
	CardCount = CardNum [sp] = yyeline

#define check_call(x,col)\
	if (((NodeIndex = x) != -1) && (MinorOperator == CallOp) && (ListSizeOf (Ls2operand) != Ls2parameter)) ErrorMsg (8, col)

#define PushProcType(x)  ProcTypeStack[ProcTypeCounter++]=(long int)x;
#define PopProcType      (ProcTypeStack[--ProcTypeCounter])

	int sp;			/* semantics stack index */
	STRING ss;		/* temp string for semantics code */
	int t1, t2, t3;		/* temps for semantics */
	int NodeIndex;
	int ProcTypeCounter = 0;
	long int ProcTypeStack[100] = { 0 };
	int pushed = 0;
