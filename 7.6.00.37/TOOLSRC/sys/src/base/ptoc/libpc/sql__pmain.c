/* @(#)sql__pmain.c		2.1.1  2001-01-15  14:05 SAP DB */
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

#if defined(_WIN32)
#include <windows.h>
#endif

#define asm(x)                 /*IST 07.08.91*/

#include <signal.h>

#if defined ( __PTOCLIB__ )
# include "h00vars.h"   /* nocheck */
# include "libpc.h"     /* nocheck */
#else
# include "libptc/h00vars.h"
# include "libptc/libpc.h"
#endif

#ifdef  __WATCOMC__
#define _iob    __iob         /*RR 09.09.92*/
#endif

/*
 * program variables
 */
#if defined(_WIN32)
  int                                _argc;
  char                               **_argv;
  char                               **_envp;
  struct iorechd                     *input;
  struct iorechd                     *output;
#else
  extern int                         _argc;
  extern char                        **_argv;
  extern char                        **_envp;
#endif


long            sql__stlim = 500000;
long            sql__stcnt = 0;
long            sql__seed_v = 7774755;
#ifdef I386
long            _main_cfp_;             /* base pointer of main */
#endif

#ifdef ORI      /*JH sql__pmain not used for precomiled programs ! */
char            *sql__minptr = (char *)MIN_POINTER;
char            *sql__maxptr = (char *)MAX_POINTER;

/*
 * file record variables
 */
long            sql__filefre = PREDEF;
struct iorechd  sql__fchain = {
        0, 0, 0, 0,             /* only use fchain field */
        INPUT                   /* fchain  */
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
        &sql__inwin ,                            /* fileptr */
        0,                                       /* lcount  */
        0x7fffffff,                              /* llimit  */
#if defined(_WIN32) || defined(LINUX)
        (FILE*) -1,                              /* fbuf    */
#else
        &_iob[0],                                /* fbuf    */
#endif
        OUTPUT,                                  /* fchain  */
        STDLVL,                                  /* flev    */
        "standard input",                        /* pfname  */
        FTEXT|FREAD|SYNC|EOLN,                   /* funit   */
        0,                                       /* fblk    */
        1                                        /* fsize   */
};
struct iorechd  sql__output = {
        &sql__outwin ,                           /* fileptr */
        0,                                       /* lcount  */
        0x7fffffff,                              /* llimit  */
#if defined(_WIN32) || defined(LINUX)
        (FILE*) -1,                              /* fbuf    */
#else
        &_iob[1],                                /* fbuf    */
#endif
        ERR,                                     /* fchain  */
        STDLVL,                                  /* flev    */
        "standard output",                       /* pfname  */
        FTEXT | FWRITE | EOFF,                   /* funit   */
        1,                                       /* fblk    */
        1                                        /* fsize   */
};
struct iorechd  sql__err = {
        &sql__errwin ,                           /* fileptr */
        0,                                       /* lcount  */
        0x7fffffff,                              /* llimit  */
#if defined(_WIN32) || defined(LINUX)
        (FILE*) -1,                              /* fbuf    */
#else
        &_iob[2],                                /* fbuf    */
#endif
        FILNIL,                                  /* fchain  */
        STDLVL,                                  /* flev    */
        "Message file",                          /* pfname  */
        FTEXT | FWRITE | EOFF,                   /* funit   */
        2,                                       /* fblk    */
        1                                        /* fsize   */
};

#else  /* ORI */
extern struct iorec    *sql__actfile[MAXFILES] ;
#endif /* ORI */

void
sql__pmain(int mode)
{
        /*
         * necessary only on systems which do not initialize
         * memory to zero
         */

        struct iorec    **ip;
#if defined(_WIN32)
        HMODULE hm;            /*type defined in windows.h*/
        extern int    __argc;  /*set by system on module entry*/
        extern char **__argv;
        int        *win_argc;  /*import from DLL at runtime, set here...*/
        char     ***win_argv;  /*import from DLL at runtime, set here...*/
#endif

#if defined(_WIN32)

        if (  (hm = GetModuleHandle("SqlUser"))  ||
              (hm = GetModuleHandle("SqlUser72"))||
              (hm = GetModuleHandle("SqlUser73"))||
              (hm = GetModuleHandle("SqlUser74"))||
              (hm = GetModuleHandle("SqlUser74n"))||
              (hm = GetModuleHandle("SqlUser75"))||
              (hm = GetModuleHandle("SqlUser76"))||
              (hm = GetModuleHandle("SqlUser77"))||
              (hm = GetModuleHandle("SqlUser78"))||
              (hm = GetModuleHandle("SqlUser79"))||
              (hm = GetModuleHandle("SqlUser80"))||
              (hm = GetModuleHandle("SqlUser81"))||
              (hm = GetModuleHandle("SqlUser81"))||
              (hm = GetModuleHandle("SqlUser82"))||
              (hm = GetModuleHandle("SqlUser83"))||
              (hm = GetModuleHandle("SqlUser84"))||
              (hm = GetModuleHandle("SqlUser85"))||
              (hm = GetModuleHandle("SqlUser86"))||
              (hm = GetModuleHandle("SqlUser87"))||
              (hm = GetModuleHandle("SqlUser88"))||
              (hm = GetModuleHandle("SqlUser89")))
        {
           win_argc = (int    *)GetProcAddress(hm, "WinArgc");
           win_argv = (char ***)GetProcAddress(hm, "WinArgv");

           if (win_argc && win_argv) {
              *win_argc = __argc;
              *win_argv = __argv;
           }
        }

#else

        /*
         * if running with runtime tests enabled, give more
         * coherent error messages for FPEs
         */
#ifdef PYRAMID
        if (mode) {
                asm ("bispsw    $0x480");
                signal(SIGFPE, sql__catchae);
        }
#endif

#ifdef M68020
        asm ("fmov.l    %control,%d0    ");
        asm ("and.l     &0xff,%d0       ");
        asm ("or.l      &0x3400,%d0     ");
        asm ("fmov.l    %d0,%control    ");
        signal(SIGFPE, sql__catchae);
#endif

#ifdef I386
        asm("   movl    (%ebp),%eax");          /* save main base pointer */
        asm("   movl    %eax,_main_cfp_");

        signal(SIGFPE, sql__catchae);
#endif
#endif

#if defined(_WIN32) || defined(LINUX)
        if (sql__input.fbuf == (FILE*) -1L)
          {
          sql__input.fbuf  = stdin;
          sql__output.fbuf = stdout;
          sql__err.fbuf    = stderr;
          }

#if !defined(LINUX)
        input  = &sql__input;
        output = &sql__output;
#endif /* !LINUX */
#endif

        for (ip = &sql__actfile[3]; ip < &sql__actfile[MAXFILES]; *ip++ = FILNIL)
          {;}
}

/* return frame pointer of caller */


/*
 * Use this function to prevent from  
 * "local variable 'a' used without having been initialized warnings" 
 */
int 
__array( int * a, int pos )
{
 return a[pos];
}

/*
 * Return frame pointer of calling function
 */
int
sql__gc()
{
        /*
         * This function should return the frame pointer of its calling
         * function; the current implementation should be portable
         * across many architectures, but there might be some archi-
         * tectures on which this won't work; in the latter case
         * this function must be implemented using "asm" directives
         * (note that someone defined "asm" as an empty macro at the
         * beginning of this file);
         *
         * TODO: for each new architecture, that PTOC is ported to,
         * one must check that this implementation really works; if not
         * a workaround uing "asm" directive must be implemented for
         * that architecture.
         *
         * NOTE: this function is called only when "sql_closea" is
         * called, which gets the return value of "sql_gc" (i.e. the
         * frame pointer) as an input parameter, using it to determine
         * which files are local to the calling function and must be
         * closed and unlinked from the file chain;
         */

        int     a[16];          /* allocate some space on the stack; */


        /* return the word above the local variables, */
        /* on most architectures the frame pointer of */
        /* the calling function                       */
        return __array(a, 16);
}
