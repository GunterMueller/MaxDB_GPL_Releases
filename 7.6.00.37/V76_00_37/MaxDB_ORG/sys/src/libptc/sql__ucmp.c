/* @(#)sql__ucmp.c		2.1.1  2001-01-15  15:02 SAP DB */
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

#include <string.h>

#if defined ( __PTOCLIB__ )
# include "libpc.h"     /* nocheck */
#else
# include "libptc/libpc.h"
#endif

sql__ucmp ( unsigned char    *s1,
            unsigned char    *s2,
            int              n )
{
#if defined ( __alpha )  || defined ( _WIN32 )
    return (memcmp (s1, s2, n));
#else
#if defined(I386) && !defined(M_I86) && !defined(M_I386)

/* from cc:     pushl   %ebp                  build call frame */
/* from cc:     movl    %esp, %ebp            */
    asm ( "     pushl   %edi          " ); /* save register */
    asm ( "     pushl   %esi          " ); /* save register */

    asm ( "     movl    8(%ebp), %esi " ); /* get destination address */
    asm ( "     movl    12(%ebp), %edi" ); /* get source address      */
    asm ( "     movl    16(%ebp), %ecx" ); /* get length              */

    asm ( "     movl    $0, %eax      " ); /* preset return code to equality */
    asm ( "     cmpl    $0, %ecx      " ); /* test length              */
    asm ( "     je      .Lucmp_ret    " ); /* if length zero, return   */

    asm ( "     cld                   " ); /* set direction to forward */
    asm ( "     repz                  " ); /* repeat the following instruction, */
                                           /* while %ecx != 0 and instruction */
                                           /* result is equal; decrement %ecx */
    asm ( "     scmpb                 " ); /* compare (%esi) with (%edi); */
                                           /* update %esi and %edi      */
    asm ( "     je      .Lucmp_ret    " );
    asm ( "     jb      .Lucmp_neg    " );
    asm ( "     movl    $1, %eax      " ); /* set return code to greater */
    asm ( "     jmp     .Lucmp_ret    " );

    asm ( ".Lucmp_neg:                " );
    asm ( "     movl    $-1, %eax     " ); /* set return code to less */

    asm ( ".Lucmp_ret:                " );
    asm ( "     popl    %esi          " ); /* restore register          */
    asm ( "     popl    %edi          " ); /* restore register          */
/* from cc:     leave                         remove call frame         */
/* from cc:     ret                           return less|equal|greater */

/* ====================================================================== */

#else                   /* no Assembler code produced for this machine */

int i ;

for ( i = 0 ; i < n ; i++ )
        {
        if ( s1[i] != s2[i] )
                return ( s1[i] > s2[i] ? 1 : -1 ) ;
        }
return (0) ;

#endif
#endif

}

