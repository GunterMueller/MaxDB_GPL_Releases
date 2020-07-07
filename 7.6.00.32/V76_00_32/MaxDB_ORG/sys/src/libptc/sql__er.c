/* @(#)sql__er.c		2.1.1  2001-01-15  14:08 SAP DB */
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
# include "h00vars.h"   /* nocheck */
# include "libpc.h"     /* nocheck */
#else
# include "libptc/h00vars.h"
# include "libptc/libpc.h"
#endif

static char *sql__FSC_STR =
        "%*[ \t\n]%74[abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789]";
extern char sql__readbf [];             /* sql__readbf is defined in sql__sync.c */


long
sql__er(struct iorec   *curfile, void  *block)
{
        register int     len;
        register long    i = 0;
        register int     cnt;
        char             namebuf[NAMSIZ];
        int              retval;
        enum_type_names  *name_infos = block;

/**
 *      if (curfile->funit & FWRITE) {
 *              sql__perrorp("%s: Attempt to read, but open for writing\n",
 *                      (long) curfile->pfname, 0L );
 *              / *NOTREACHED* /
 *      }
 **/
        sql__uncs(curfile);
        retval = fscanf(curfile->fbuf, sql__FSC_STR+7, namebuf);
        if (retval == 0) {
            retval = fscanf(curfile->fbuf, sql__FSC_STR, namebuf);
        }
        if (retval == EOF) {
                sql__perrorp (sql__readbf, (_ptoc_long) curfile->pfname, 0L );
                                /* %s: Tried to read past end of file */
                /*NOTREACHED*/
        }
        if (retval == 0)
                goto ename;
        curfile->funit &= ~EOLN;
        curfile->funit |= SYNC;
        for (len = 0; len < NAMSIZ && namebuf[len]; len++) {;}
        len++;
        cnt = name_infos->cnt;
        for (i = 0; i < cnt; i++) {
                if (sql__erq (len, name_infos->names[i], namebuf)) {
                        return i;
                }
        }
ename:
        sql__perrorp("Unknown name \"%s\" found on enumerated type read\n",
                     (_ptoc_long) namebuf, 0L );
        /*NOTREACHED*/
        return 0L;
}
