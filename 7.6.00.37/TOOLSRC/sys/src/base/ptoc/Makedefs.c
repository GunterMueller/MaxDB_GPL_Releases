/* @(#)Makedefs.c		2.0.6  1997-05-12  SQL DBS
/*                       Definitions given to 'make' for PTOC
/*                       to be processed by 'cpp' into 'Makedefs'


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

/*                                                                      */
/* This file contains all definitions needed in the several 'Makefile's */
/* used to 'make' PTOC (Pascal-TO-C-translator).                        */
/* It is called '.c' only to make the C compiler accept it as input.    */
/*                                                                      */
/* The distinction for the several machines should be made using only   */
/* predefined symbols of 'cpp' so that no manual intervention is needed.*/
/* These predefined symbols ara available only via 'cc', so the file    */
/* should resemble legal C syntax. It may be difficult to get the       */
/* comments into the output.                                            */
/*                                                                      */
/* In the 'master' version, comments are C-style which is translated    */
/* into the at-sign '#' needed by 'make' AFTER 'cpp' has extracted      */
/* the specific version.                                                */
/*                                                                      */
/* Note that for purposes of checking, every branch should contain      */
/* exactly one comment line with the sequence 'spe cif ic:' (without    */
/* the blanks). A check might be introduced that 'fgrep' finds          */
/* exactly one such line.                                               */
/*                                                                      */
/* For each environment, we 'define' to 'cpp' the CPU (variable 'MACH') */
/* and the UNIX variant ('OSSPEC')                                      */
/* for BOTH C and Pascal files ('ptoc' uses 'cpp' for '.P' files !).    */
/* However, if our value is identical to the predefined symbol, this    */
/* is 'undef'ined first so that 'cpp' does not replace the string by    */
/* its ('cpp'-) value, normally '1'.                                    */
/* In all cases, we define 'SYSV' as well - if an environment is        */
/* added for which this is not valid, this master file must be changed. */
/* For C files, 'CC' is also defined (look into some of the include     */
/* files why).                                                          */
/*                                                                      */
/* If the C compiler needs any additional options (including 'define's  */
/* which are not relevant for Pascal code), these must be given in      */
/* 'CCOPTS'.                                                            */
/*                                                                      */

/* These are initial values which should be reset in the respective     */
/* branch. Note that the 'CHECK' macro should fail in both the default  */
/* and any non-matching case so that 'make' stops.                      */
	MACH = undefined
	OSSPEC = undefined
	CHECK = uname -a ; false

/* Standard name of the C compiler + other tools (if not changed below). */
/* $(RANLIB) is called to update archives, 'echo' is used if not needed  */
	CC = cc
	AS = as
	LD = cc
	AR = ar
	RANLIB = ranlib


OPTIMIZE = -O
CCOPTS =
SHL_CCOPTS =

#if i386
/* any machine with iAPX386                                             */
MACH = I386
#if defined ( sinix ) || defined ( SINIX )
/* specific: Unexpected sinix / SINIX                                   */
#endif
#if defined ( MX300I ) || defined ( MX500I )
/* specific: Unexpected MX 300 i / MX 500 i                             */
	OSSPEC = sinix
	RANLIB = echo
#endif
#if M_XENIX     /* we are looking for SCO, but they don't confess openly */
/* 'i386' is ensured above - keep to the nested if's                    */
/* specific: PC, iAPX386, SCO                                           */
	OSSPEC = SCO
	CCOPTS = -D_SVID -D_NO_PROTOTYPE -DNO_PROTOTYPE
	CC = cc
	RANLIB = echo
	CHECK = uname -a ; ls -ld /etc/copyrights/01.sco
#else
#  if seq || dynix || DYNIX     /* NOT YET TESTED - none available */
/* specific: SEQUENT, iAPX386, DYNIX    Unexpected / Untested           */
#undef DYNIX
	OSSPEC = DYNIX
	OPTIMIZE = -Wc,-O3
	CCOPTS = -W0,-Nn4096
	RANLIB = ar ts
#  else
	OSSPEC = LINUX
	RANLIB = echo
	CCOPTS = -g
	CC = cc
	CHECK = uname -s | fgrep -x Linux
/* 'i386' is ensured above - any Non-SCO Non-Dynix is treated as System V */
/* specific: LINUX */
#if 0
/* specific: PC, iAPX386, UNIX V (Interactive, SNI MX..i, NCR, ...)     */
	OSSPEC = SVR4
	SHL_CCOPTS = -K PIC
	RANLIB = ar ts
	CHECK = uname -a ; what '/unix' | fgrep 'UNIX System V/386' | head -3
#endif
#  endif
#endif
#endif

#if T35 || pyr
/* specific: TARGON/35                                                  */
	MACH = PYRAMID
	OSSPEC = TOS41
	RANLIB = ucb ranlib
	CHECK = uname -m | fgrep TARGON/35
#endif

#if T31 || TARGON31 || __T31__ || m68k
/* Attention: none of the '..31' symbols is defined.                    */
/* ANY 68k CPU will get here, 'T31' is correct by pure chance.          */
#undef T31
	MACH = T31
#  if sinix
/* specific: TARGON/31, SINIX                                           */
	OSSPEC = SINIX
#  else
/* specific: TARGON/31, TOS                                             */
	OSSPEC = TOS
	CHECK = uname -s | fgrep -x TARGON31
#endif
	RANLIB = echo
#endif

#if _IBMR2
/* specific: IBM RS/6000                                                */
#undef _IBMR2
	MACH = _IBMR2
	OSSPEC = AIX
	CCOPTS = -D_NO_PROTO -U__STR__
	CHECK = uname -s | fgrep -x AIX
#endif

#if __alpha
/* specific: DEC Alpha                                                  */
	MACH = ALPHA
	OSSPEC = OSF1
	OPTIMIZE = -O2 -Olimit 1500
	CCOPTS =
	LDFLAGS = $(OPTIMIZE)
	RANLIB = ar ts
	CHECK = uname -m | fgrep alpha
/* All programs use 64-bit addrs except pgenc, which still uses 32-bit addrs */
	PGENC_CCFLAGS= -taso
	PGENC_LDFLAGS= -taso
#endif

#if __hp9000s700 || __hp9000s800
/* specific: HP 9000 series 700 or 800                                  */
	MACH = HP9
	OSSPEC = HPUX
	OPTIMIZE = +O1    /* ehemals +O3 */
	CCOPTS = +DA1.1 +DS1.1 -Ae +Z
/*	SHL_CCOPTS = +Z */
	RANLIB = ar ts
	CHECK = uname -s | fgrep HP-UX
#endif

#if __ia64
/* specific: HP-UX IA64 */
        MACH = HP_IA64
        OSSPEC = HPUX
        OPTIMIZE = +O1    /* ehemals +O3 */
        CCOPTS = -Ae +Z +DD32
        LDFLAGS = -L/usr/lib/hpux32
        RANLIB = ar ts
        CHECK = uname -s | fgrep HP-UX
#endif
    
#if mips
#  if sinix
/* specific: SNI RM 600 and similar                                     */
	MACH = RM6
	OSSPEC = SVR4
	CCOPTS = -K signed -K PIC
	CC = /usr/ccs/bin/cc
	AS = /usr/ccs/bin/as
	LD = /usr/ccs/bin/cc
	AR = /usr/ccs/bin/ar
	CHECK = uname -m | fgrep RM600
	RANLIB = echo
#  else
/* specific: MIPS machine other than RM 600                             */
	MACH = some_mips
	OSSPEC = some_os
	CHECK = uname -s | fgrep TARGON3300
#  endif
#endif

#if sun
	MACH = SUN
#  if sunos || SUNOS
/* specific: SUN SPARC, running SUN-OS                                  */
#undef SUNOS
	OSSPEC = SUNOS
	OPTIMIZE = -O4
	CHECK = uname -s | fgrep -x SunOS && uname -r | fgrep 4.
#  else         /* SOLARIS */
/* specific: SUN SPARC, running SOLARIS                                 */
	OSSPEC = SVR4
	OPTIMIZE = -xO4
	CCOPTS = -Xt
	RANLIB = ar ts
	CHECK = uname -s | fgrep -x SunOS && uname -r | fgrep 5.
#  endif
#endif

#if NATSEMI || ns32000
/* specific: Unexpected National Semiconductor                          */
#endif
#if AT386
/* specific: Unexpected AT 386                                          */
#endif
#if defined ( mc68000 ) || defined ( is68k )
/* specific: Unexpected Motorola CPU (any)                              */
#endif
#if defined ( M68000 ) || defined ( M68010 ) || defined ( M68020 )
/* specific: Unexpected Motorola CPU (0/1/2)                            */
#endif
#if defined ( M68030 ) || defined ( M68040 )
/* specific: Unexpected Motorola CPU (3/4)                              */
#endif
#if defined ( svr3 ) || defined ( SVR3 ) || defined ( SVR32 )
/* specific: Unexpected System V Release 3                              */
#endif
#if defined ( svr4 ) || defined ( SVR4 ) || defined ( SVR42 )
/* specific: Unexpected System V Release 4                              */
#endif
#ifdef SYSV
/* specific: Unexpected System V                                        */
#endif


CCFLAGS = -D$(OSSPEC) -D$(MACH) -DSYSV -DCC $(OPTIMIZE) $(CCOPTS) -D__PTOCLIB__
PCFLAGS = -D$(OSSPEC) -D$(MACH) -DSYSV -O  -e
/* test, original was with             -O G     option */


/* From README.m (sqlall1:/u/all30/tool/bin, 1993-May-04)               */
/*                                                                      */
/*              &if $MACH  =  SUN AND $OSSPEC  =  SUNOS                 */
/*              #DEFS = "\                                              */
/*              # -I/usr/include/net -I/usr/include -I/usr/5include "   */
/*              &endif                                                  */
/*                                                                      */
/*      &if $OSSPEC in [ MI_ULT ]                                       */
/*      OPTIMIZE = "-O2 -Olimit 1500"                                   */
/*      NO_OPTIMIZE = "-O0"                                             */
/*      &else                                                           */
/*      &endif                                                          */
/*                                                                      */
/*      ASFLAGS =                                                       */
/*                                                                      */
/*      &if $OSSPEC in [ HPUX , TOS41 ]                                 */
/*      AS = as                                                         */
/*      &else                                                           */
/*      AS = $CC                                                        */
/*      &endif                                                          */
/*                                                                      */
/*      ==== =                                                          */
/*      6071 Dec  2 11:54 libpc/Makefile.sun                            */
/*      91c91                                                           */
/*      <       sql__ucmp.c                                             */
/*      ---                                                             */
/*      >       sql__ucmp.s                                             */
/*      ========= =                                                     */
/*                                                                      */

