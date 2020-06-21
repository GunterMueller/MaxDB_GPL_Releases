/*	mf.h		

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

/*
 *	module frame tools declaration file
 */


#include	<errno.h>
#include	<stdio.h>
#include	<ctype.h>
#include	<string.h>
#include	<stdlib.h>
#include        <memory.h>
#include	"debug.h"

#undef toupper
#define toupper(x) (x>='a'&&x<='z'?x-'a'+'A':x)
#define		ERREOF		1000

#define		ERROTH		1100
#define		ERRCON		1101
#define		ERRTYP		1102
#define		ERRVAR		1103
#define		ERRFUN		1104
#define		ERRPRC		1105
#define		ERRPRG		1106
#define		ERRBEG		1107

#define		ERRTDB		1131
#define		ERRTDE		1132


#define		TYPOTH		0
#define		TYPCON		1
#define		TYPTYP		2
#define		TYPVAR		3
#define		TYPFUN		4
#define		TYPPRC		5
#define		TYPPRG		6
#define		TYPBEG		7
#define		TYPSTEPOUT	8

#define		TYPTDB		31
#define		TYPTDE		32


#define		LINSIZ		8192
#define		WRDSIZ		256
#define		SYMSIZ		256
#define		KWDSIZ		256


char		line[LINSIZ];
char		word[WRDSIZ];
char		symbol[SYMSIZ];
char		lastproc[SYMSIZ];
char		keyword[KWDSIZ];
int		    keywrdtyp;
int		    c;

FILE		*infp;
FILE		*outfp;

int		tdskipflag;
int		trace_step_out;
FILE		*tdsavfp;

char		lastproc[SYMSIZ];
int         mfscan_nesting_level;
int         keyword_record;

int                     dbgdbglvl ;

#define	DBG0(x)		{err x;}
#define	DBG1(x)		{if(dbgdbglvl>=1) {err x;}}
#define	DBG2(x)		{if(dbgdbglvl>=2) {err x;}}
#define	DBG3(x)		{if(dbgdbglvl>=3) {err x;}}
#define	DBG4(x)		{if(dbgdbglvl>=4) {err x;}}
#define	DBG5(x)		{if(dbgdbglvl>=5) {err x;}}
#define	DBG6(x)		{if(dbgdbglvl>=6) {err x;}}
#define	DBG7(x)		{if(dbgdbglvl>=7) {err x;}}
#define	DBG8(x)		{if(dbgdbglvl>=8) {err x;}}
#define	DBG9(x)		{if(dbgdbglvl>=9) {err x;}}

#define	IFDBG(dbglvl)	if ( dbglvl <= dbgdbglvl )

#define	SPRI	(void) sprintf

#define         SIG_RTRN        void
#define		global		/* empty in C but clearifies declarations */


#define	COPY(d,s,l)	(void) memcpy ( (char*)(d) , (char*)(s) , (l) )
#define	FILL(d,v,l)	(void) memset ( (char*)(d) ,   (int)(v) , (l) )

#undef	FALSE
#define	FALSE		0
#undef	TRUE
#define	TRUE		1
#undef	NULL
#define	NULL		0L

