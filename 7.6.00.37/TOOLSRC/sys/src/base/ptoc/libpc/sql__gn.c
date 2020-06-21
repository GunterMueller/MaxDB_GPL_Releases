/* @(#)sql__gn.c		2.1.1  2001-01-15  14:07 SAP DB */
/*

    ========== licence begin LGPL
    Copyright (C) 2000 SAP AG

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

#ifdef  SVR4 || defined SOLARIS
#if     defined(__STDC__)
#define _iob    __iob   
#endif
#endif

#if defined ( __PTOCLIB__ )
# include "h00vars.h"   /* nocheck */
# include "libpc.h"     /* nocheck */
#else
# include "libptc/h00vars.h"
# include "libptc/libpc.h"
#endif

#if defined(_WIN32)
 #include <windows.h>
#endif

/*
 * file record variables
 */
long            sql__filefre = PREDEF;
struct iorechd  sql__fchain = {
        0, 0, 0, 0,             /* only use fchain field */
        INPUT                      /* fchain  */
};
struct iorec    *sql__actfile[MAXFILES] = {
        INPUT,
        OUTPUT,
        ERR
};

/*
 * standard files
 */
static char            sql__inwin , sql__outwin , sql__errwin ;
struct iorechd  sql__input = {
        &sql__inwin ,           /* fileptr */
        0,                      /* lcount  */
        0x7fffffff,             /* llimit  */
#if defined(_WIN32) || defined(LINUX) || defined(FREEBSD)
        (FILE*) -1,             /* fbuf    */
#else
        &_iob[0],               /* fbuf    */
#endif
        OUTPUT,                    /* fchain  */
        STDLVL,                 /* flev    */
        "standard input",       /* pfname  */
        FTEXT|FREAD|SYNC|EOLN,  /* funit   */
        0,                      /* fblk    */
        1                       /* fsize   */
};
struct iorechd  sql__output = {
        &sql__outwin ,          /* fileptr */
        0,                      /* lcount  */
        0x7fffffff,             /* llimit  */
#if defined(_WIN32) || defined(LINUX) || defined(FREEBSD)
        (FILE*) -1,             /* fbuf    */
#else
        &_iob[1],               /* fbuf    */
#endif
        ERR,                    /* fchain  */
        STDLVL,                 /* flev    */
        "standard output",      /* pfname  */
        FTEXT | FWRITE | EOFF,  /* funit   */
        1,                      /* fblk    */
        1                       /* fsize   */
};
struct iorechd  sql__err = {
        &sql__errwin ,          /* fileptr */
        0,                      /* lcount  */
        0x7fffffff,             /* llimit  */
#if defined(_WIN32) || defined(LINUX) || defined(FREEBSD)
        (FILE*) -1,             /* fbuf    */
#else
        &_iob[2],               /* fbuf    */
#endif
        FILNIL,                 /* fchain  */
        STDLVL,                 /* flev    */
        "Message file",         /* pfname  */
        FTEXT | FWRITE | EOFF,  /* funit   */
        2,                      /* fblk    */
        1                       /* fsize   */
};

/*
 * sql__gn - activate a file
 *
 * takes a name, name length, element size, and variable
 * level and returns a pointer to a file structure.
 *
 * a new file structure is initialized if necessary.
 * temporary names are generated, and given
 * names are blank trimmed.
 */

static char *tmpname = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

struct iorec *
sql__gn(struct iorec            *filep,
        char                    *name,
        long                    namlim,
        long                    datasize )
{
        long            maxnamlen = namlim;
        struct iorec    *prev;
        struct iorec    *next;
        register int    cnt;
        struct iorec    locvar;

        if (filep->fblk < MAXFILES && sql__actfile[filep->fblk] == filep) {
                /*
                 * Close and immediately reactivate the file.
                 */
                sql__closep(filep, name != NULL);
                sql__actfile[filep->fblk] = filep;
                filep->funit &= (TEMP | FTEXT);
        } else {
                /*
                 * Initialize a new file record.
                 */
                filep->funit = 0;
                if (datasize == 0) {
                        filep->funit |= FTEXT;
                        datasize = 1;
                }
                filep->fsize = datasize;
                filep->fbuf = 0;
                filep->lcount = 0;
                filep->llimit = 0x7fffffff;
                filep->fname [0] = (char) 0 /* J.Bruehe: not " NULL " */ ;
                filep->fileptr = &filep->window[0];
                /*
                 * Check to see if file is global, or allocated in
                 * the stack by checking its address against the
                 * address of one of our routine's local variables.
                 */
                if ((filep < &locvar) || (filep > GLVL))
                        filep->flev = GLVL;
                else
                        filep->flev = filep;
                for (sql__filefre++; sql__filefre < MAXFILES; sql__filefre++)
                        if (sql__actfile[sql__filefre] == FILNIL)
                                goto gotone;
                for (sql__filefre = PREDEF + 1; sql__filefre < MAXFILES; sql__filefre++)
                        if (sql__actfile[sql__filefre] == FILNIL)
                                goto gotone;
                sql__perrorp ( "File table overflow\n" , 0L , 0L );
                /*NOTREACHED*/
gotone:
                filep->fblk = sql__filefre;
                sql__actfile[sql__filefre] = filep;
                /*
                 * Link the new record into the file chain.
                 */
                prev = (struct iorec *)&sql__fchain;
                next = sql__fchain.fchain;
                while (filep->flev > next->flev) {
                        prev = next;
                        next = next->fchain;
                }
                if (filep->flev == GLVL)
                        /*
                         * Must order global files so that all dynamic files
                         * within a record are grouped together.
                         */
                        while ((next != FILNIL) &&
                               (next->flev == GLVL) &&
                               ((struct iorec *)filep > next)) {
                                prev = next;
                                next = next->fchain;
                        }
                filep->fchain = next;
                prev->fchain = filep;
        }
        /*
         * Get the filename associated with the buffer.
         */
        if (name == NULL) {
/* J.Bruehe: nonsense   if (*filep->fname != NULL) {    */
                if ( filep->fname [ 0 ] != (char) 0 ) {
                        return(filep);
                }
                /*
                 * No name given and no previous name, so generate
                 * a new one of the form #tmp.xxxxxx.
                 */
                filep->funit |= TEMP;
#if defined(_WIN32)
// ---- few new lines for Windows NT... (Ramo)
                /*File name must match DOS naming conventions!*/
                /*Windows temp file names usually contains a '~'.*/
                sprintf(filep->fname, "%lx.~%ct",
                    GetCurrentProcessId(), tmpname[filep->fblk]);
#else
                sprintf(filep->fname, "#tmp.%c%d", tmpname[filep->fblk],
                    getpid());
#endif
                filep->pfname = &filep->fname[0];
                return(filep);
        }
        /*
         * Trim trailing blanks, and insure that the name
         * will fit into the file structure.
         */
        for (cnt = 0; cnt < maxnamlen; cnt++)
                if (name[cnt] == '\0' || name[cnt] == ' ')
                        break;
        if (cnt >= NAMSIZ) {
                sql__perrorp ( "%s: File name too long\n" , (_ptoc_long) name , 0L );
                /*NOTREACHED*/
        }
        maxnamlen = cnt;
        filep->funit &= ~TEMP;
        /*
         * Put the new name into the structure.
         */
        for (cnt = 0; cnt < maxnamlen; cnt++)
                filep->fname[cnt] = name[cnt];
        filep->fname[cnt] = '\0';
        filep->pfname = &filep->fname[0];
        return(filep);
}
