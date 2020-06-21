/* @(#)h00vars.h		2.1.1  2001-01-15  13:49 SAP DB */
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
#ifndef H00VARS_H_
#define H00VARS_H_
#include <stdio.h>

# include <limits.h>

# include "whoami.h"  /* nocheck */

#define BITSPERBYTE     8
#define BITSPERLONG     (BITSPERBYTE * sizeof(long))
#define LG2BITSBYTE     03
#define MSKBITSBYTE     07
#define LG2BITSLONG     05
#define MSKBITSLONG     037
#define NAMSIZ          76
#define MAXFILES        32
#define PREDEF          2

#ifdef ADDR64
# if defined(_WIN64)
#  define  MIN_POINTER      _I64_MAX
# else
#  define  MIN_POINTER      LONG_MAX
# endif
#endif
#ifdef ADDR32
# define   MIN_POINTER      0x7fffffff
#endif

#define   MAX_POINTER       0



#ifdef PYRAMID
#   define STDLVL  ((struct iorec *)(0xc00cc001))
#   define GLVL    ((struct iorec *)(0xc00cc000))
#else
#   ifdef ADDR64
#       define STDLVL  ((struct iorec *)(0xffffffffffffffff))
#       define GLVL    ((struct iorec *)(0xfffffffffffffffe))
#   else
#       define STDLVL  ((struct iorec *)(0xffffffff))
#       define GLVL    ((struct iorec *)(0xfffffffe))
#   endif /* ADDR64 */
#endif /* PYRAMID */

#define FILNIL     ((struct iorec *)(0))
#define INPUT      ((struct iorec *)(&sql__input))
#define OUTPUT     ((struct iorec *)(&sql__output))
#define ERR        ((struct iorec *)(&sql__err))

/*
 * program variables
 */

extern int              _argc;          /* number of passed args */
extern char             **_argv;        /* values of passed args */
extern char             **_envp;        /* values of environment vars */
extern long             sql__stlim;     /* statement limit */
extern long             sql__stcnt;     /* statement count */
extern long             sql__seed_v;    /* random number seed value */
extern char             *sql__maxptr;   /* maximum valid pointer */
extern char             *sql__minptr;   /* minimum valid pointer */

/*
 * file structures
 */
struct iorechd {
        char            *fileptr;       /* ptr to file window */
        int             lcount;         /* number of lines printed */
        int             llimit;         /* maximum number of text lines */
        FILE            *fbuf;          /* FILE ptr */
        struct iorec    *fchain;        /* chain to next file */
        struct iorec    *flev;          /* ptr to associated file variable */
        char            *pfname;        /* ptr to name of file */
        short           funit;          /* file status flags */
        unsigned short  fblk;           /* index into active file table */
        long            fsize;          /* size of elements in the file */
        char            fname[NAMSIZ];  /* name of associated UNIX file */
};

struct iorec {
        char            *fileptr;       /* ptr to file window */
        int             lcount;         /* number of lines printed */
        int             llimit;         /* maximum number of text lines */
        FILE            *fbuf;          /* FILE ptr */
        struct iorec    *fchain;        /* chain to next file */
        struct iorec    *flev;          /* ptr to associated file variable */
        char            *pfname;        /* ptr to name of file */
        short           funit;          /* file status flags */
        unsigned short  fblk;           /* index into active file table */
        long            fsize;          /* size of elements in the file */
        char            fname[NAMSIZ];  /* name of associated UNIX file */
        char            buf[BUFSIZ];    /* I/O buffer */
        char            window[1];      /* file window element */
};

/*
 * unit flags
 */
#define SPEOLN  0x100   /* 1 => pseudo EOLN char read at EOF */
#define FDEF    0x080   /* 1 => reserved file name */
#define FTEXT   0x040   /* 1 => text file, process EOLN */
#define FWRITE  0x020   /* 1 => open for writing */
#define FREAD   0x010   /* 1 => open for reading */
#define TEMP    0x008   /* 1 => temporary file */
#define SYNC    0x004   /* 1 => window is out of sync */
#define EOLN    0x002   /* 1 => at end of line */
#define EOFF    0x001   /* 1 => at end of file */

/*
 * file routines
 */
extern struct iorec     *sql__gn();

/*
 * file record variables
 */
extern struct iorechd   sql__fchain;        /* head of active file chain */
extern struct iorec     *sql__actfile[];    /* table of active files */
extern long             sql__filefre;       /* last used entry in _actfile */

/*
 * standard files
 */
extern struct iorechd   sql__input;
extern struct iorechd   sql__output;
extern struct iorechd   sql__err;


/* enumerated type name descriptor */
typedef struct { 
  int    cnt;
  char * names[1]; 
} enum_type_names;


#endif /* H00VARS_H_ */
