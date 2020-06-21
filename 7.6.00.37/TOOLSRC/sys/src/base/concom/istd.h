/*	istd.h		

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

/* conditional integer, used for string and file functions */
#if	SCO || pyr
#define		CINT		unsigned
#else
#define		CINT		int
#endif

/* type of return value of signal-catching handler */
#if	T32
#define		SIG_RTRN	int
#else
#define		SIG_RTRN	void
#endif

#undef		TRUE
#define		TRUE		1
#undef		FALSE
#define		FALSE		0
#undef		NULL
#define		NULL		0L

#define	global	/* empty in C but clearifies the use of symbols */

#define	COPY(d,s,l)	(void) memcpy ( (char*)(d) , (char*)(s) , (CINT)(l) )
#define	FILL(d,v,l)	(void) memset ( (char*)(d) ,   (int)(v) , (CINT)(l) )

/* debug routines are called like: DBG3 (( "blabla %d '%s' \n", i, txt )) */
#ifdef	DEBUG
extern	void		dbgdbg ();
extern	int		dbgdbglvl ;
#define	DBG0(x)		{dbgdbg x;}
#define	DBG1(x)		{if(dbgdbglvl>=1) dbgdbg x;}
#define	DBG2(x)		{if(dbgdbglvl>=2) dbgdbg x;}
#define	DBG3(x)		{if(dbgdbglvl>=3) dbgdbg x;}
#define	DBG4(x)		{if(dbgdbglvl>=4) dbgdbg x;}
#define	DBG5(x)		{if(dbgdbglvl>=5) dbgdbg x;}
#define	DBG6(x)		{if(dbgdbglvl>=6) dbgdbg x;}
#define	DBG7(x)		{if(dbgdbglvl>=7) dbgdbg x;}
#define	DBG8(x)		{if(dbgdbglvl>=8) dbgdbg x;}
#define	DBG9(x)		{if(dbgdbglvl>=9) dbgdbg x;}
#else
#define	DBG0(x)		{}
#define	DBG1(x)		{}
#define	DBG2(x)		{}
#define	DBG3(x)		{}
#define	DBG4(x)		{}
#define	DBG5(x)		{}
#define	DBG6(x)		{}
#define	DBG7(x)		{}
#define	DBG8(x)		{}
#define	DBG9(x)		{}
#endif

