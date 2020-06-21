/* @(#)sql__setrang.c		2.1.1  2001-01-15  14:34 SAP DB */
/*


    ========== licence begin  GPL
    Copyright (c) 2000-2005 SAP AG

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version 2
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
    ========== licence end




 */

#if defined ( __PTOCLIB__ )
# include "libpc.h"     /* nocheck */
#else
# include "libptc/libpc.h"
#endif

char sql__ERANGL[] = "(Set?) Range violation: value %ld , lower bound %ld \n";
char sql__ERANGU[] = "(Set?) Range violation: value %ld , upper bound %ld \n";

#if defined(WIN32)
#pragma warning( disable: 4716 ) // must return a value
#endif

long
sql__setrang( long    value,
              long    lower,
              long    upper )
{

#if defined(I386) && !defined(M_I86) && !defined(M_I386) &&  !defined (WIN32)

/* from cc:     pushl   %ebp                 build call frame */
/* from cc:     movl    %esp, %ebp           */
    asm ( "    movl    8(%ebp), %eax " ); /* get value to check */

    asm ( "    cmpl    %eax, 12(%ebp)" ); /* check for minimum  */
    asm ( "    jg      .L01_range    " );

    asm ( "    cmpl    %eax, 16(%ebp)" ); /* check for maximum  */
    asm ( "    jge     .L01_rangeok  " );

    asm ( ".L01_range:               " );
    asm ( "    hlt                   " ); /* reserved instruction trap */
    asm ( "    xorl    %ecx, %ecx    " ); /* set to zero        */
    asm ( "    movl    %ecx, (%ecx)  " ); /* memory fault trap  */

    asm ( ".L01_rangeok:             " );
/* from cc:    leave                         ok, remove call frame  */
/* from cc:    ret                           return value       */

/* ====================================================================== */

#else                   /* no Assembler code produced for this machine */

        if (value < lower ){
                sql__perrorp ( sql__ERANGL , value , lower );
                /*NOTREACHED*/
                return -1L;
        }
        if (value > upper ){
                sql__perrorp ( sql__ERANGU , value , upper );
                /*NOTREACHED*/
                return -1L;
        }

        return  value;

#endif

}
