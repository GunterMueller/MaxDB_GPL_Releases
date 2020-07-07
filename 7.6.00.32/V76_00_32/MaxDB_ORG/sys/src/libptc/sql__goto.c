/* @(#)sql__goto.c		2.1.1  2001-01-15  14:07 SAP DB */
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


#if defined(_WIN32) && defined(USE_MSG_PRIO7)
 void          sql60_int_err_msg_prio7 ( char *pszFormatStr, ... );
#endif

#define asm(x)                  /*IST 07.08.91*/

/* 1992-Sep-10 J.Bruehe  This is a dummy, we do not support GOTO */

#if defined ( PYRAMID ) || defined ( I386 ) || defined ( T31 )
static int      *ob_label;
static int      *ob_link;
#endif

# include <stdio.h>

void
sql__goto ( int     *label,
            int     *link )
{

#if defined(_WIN32) && defined(USE_MSG_PRIO7)
        sql60_int_err_msg_prio7( "GOTO not implemented - ABORT \n" );
        {int *x=0; *x=0;}
#else
        fprintf ( stderr , "GOTO not implemented - ABORT \n" );
        {int *x=0; *x=0;}
        /*NOTREACHED*/
#endif

#ifdef GOTO_IS_NOT_SUPPORTED

#ifdef PYRAMID
        int     *i;  /* in lr0 */

        ob_label = label;
        ob_link = link;

        asm ("return.loop:");
        asm ("  rcsp    lr0");
        if (i >= ob_link + 32) {
                asm ("  movw    cfp,lr0");
                sql__closea (i);
                asm ("  movw    $return.loop,pr15");
                return;
        }
        ;  /* dummy statement to work around asm problem */
        asm ("  movw    cfp,lr0");
        sql__closea (i);
        asm ("  movw    _ob_label,pr15");
        return;
#else
#ifdef I386
        extern int *_main_cfp_;

        ob_label = label;
        ob_link = link;
        if (ob_link == 0) ob_link = _main_cfp_;
        asm("_return_loop:");
        asm("   cmpl    ob_link,%ebp");
        asm("   je      .L100");

        asm("   push    %ebp");                 /* close local files */
        asm("   call    sql__closea");
        asm("   movl    $_return_loop,4(%ebp)");
        asm("   leave");
        asm("   ret");

        asm(".L100:");
        asm("   pushl   ob_label");
        asm("   ret");
#else
#ifdef T31
        ob_label = label;
        ob_link = link;
        asm("return%loop:");
        asm("   mov.l   (%fp),%d0");
        asm("   cmp.l   %d0,ob_link");
        asm("   bhs     L%100");
        asm("   mov.l   %d0,-(%sp)");
        asm("   jsr     sql__closea");
        asm("   add.l   &4,%sp");
        asm("   mov.l   &return%loop,4(%fp)");
        asm("   unlk    %fp");
        asm("   rts");
        asm("L%100:");
        asm("   add.l   &4,%sp");
        asm("   mov.l   ob_label,4(%fp)");
        asm("   unlk    %fp");
        asm("   rts");
#else
# include <stdio.h>
        fprintf ( stderr , "No GOTO on this machine - ABORT \n" );
        exit ( 99 );
#endif
#endif
#endif
#endif /* GOTO_IS_NOT_SUPPORTED */
}
