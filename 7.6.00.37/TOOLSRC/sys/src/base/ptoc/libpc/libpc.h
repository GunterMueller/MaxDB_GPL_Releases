/* @(#)libpc.h		2.1.1  2001-01-15  13:52 SAP DB */
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
#ifndef LIBPC_H_
# define LIBPC_H_

# include <stdio.h>

# if !defined (_ptoc_long)
#  if defined (_M_IA64)
#   define _ptoc_long     __int64
#  else
#   define _ptoc_long     long
#  endif
# endif

# if defined(_WIN32)
#  define _CDECL          __cdecl
# else
#  define _CDECL
# endif


# ifdef FALSE    /* at least on _IBMR2 , from <sys/types.h> */
#  undef FALSE
# endif
# ifdef TRUE     /* at least on _IBMR2 , from <sys/types.h> */
#  undef TRUE
# endif

 typedef enum {FALSE, TRUE} bool;

 /*
  *
  * prototypes 
  *
  */
 extern double             sql__atan(double );
 extern long               sql__ca(unsigned char *,long );
 extern void               sql__caseerr( char *, int );
 extern long               sql__ckl(void);
 extern long               sql__clock(void);
 extern int                sql__compare(unsigned char *,unsigned char *,int ,int );
 extern double             sql__cos(double );
 extern void               sql__date(char *);
 extern void               sql__del(char *,long );
 extern void               sql__dispse(char **,long );
 extern void               sql__env(char *,long ,char *,long );
 extern bool               sql__erq(long ,char *,char *);
 extern void               sql__exit(int );
 extern double             sql__exp(double );
 extern long               sql__expp(double );
 extern void               sql__flp(void);
 extern int                sql__gc(void);
 extern bool               sql__ger(long ,long *,long *);
 extern void               sql__goto(int *,int *);
 extern bool               sql__gtl(long ,long *,long *);
 extern bool _CDECL        sql__ic(long,long,long,...);
 extern bool               sql__lel(long ,long *,long *);
 extern double             sql__ln(double );
 extern bool               sql__ltl(long ,long *,long *);
 extern long               sql__max(long ,long ,long );
 extern long             * sql__multi(long *,long *,long *,long );
 extern void               sql__new(char **,long );
 extern char             * sql__nil(char *,char *, int );
 extern char             * sql__nname(long ,void *);
 extern bool               sql__nre(long ,char *,char *);
 extern void               sql__pack(long ,char *,char *,long ,long ,long ,long );
 extern unsigned char    * sql__pcnst(unsigned char *,long,long,long,long,...);
 extern void               sql__peer(char *,char *);
 extern void  _CDECL       sql__perrorp(char *, _ptoc_long , _ptoc_long );
 extern void  _CDECL       sql__pmatherrorp(char *, double);
 extern void               sql__pmain(int );
 extern long               sql__pred(long ,long ,long, char *, int );
 extern unsigned char    * sql__psetadd(unsigned char *,unsigned char *,unsigned char *,long );
 extern unsigned char    * sql__psetco(unsigned char *,int ,int ,unsigned char *,int ,int );
 extern bool               sql__psetge(long ,unsigned char *,unsigned char *);
 extern bool               sql__psetgt(long ,unsigned char *,unsigned char *);
 extern bool               sql__psetle(long ,unsigned char *,unsigned char *);
 extern bool               sql__psetlt(long ,unsigned char *,unsigned char *);
 extern unsigned char    * sql__psetm(unsigned char *,unsigned char *,unsigned char *,long );
 extern unsigned char    * sql__pstrcat(unsigned char *,int ,unsigned char *,unsigned char *);
 extern int                sql__pstrcomp(unsigned char *,unsigned char *);
 extern unsigned char    * sql__pstrcpy(unsigned char *,int ,unsigned char *,int ,int );
 extern double             sql__rd();
 extern void               sql__rgv(long ,char *,long );
 extern long               sql__round(double );
 extern long * _CDECL      sql__scnst(long *, long, long, long, long, ... );
 extern long             * sql__setadd(long *,long *,long *,long );
 extern char             * sql__setco(char *,int ,int ,char *,int ,int );
 extern long             * sql__setmin(long *,long *,long *,long );
 extern long               sql__setrang(long ,long ,long );
 extern unsigned char    * sql__setsub(unsigned char *,unsigned char *,unsigned char *,long );
 extern double             sql__sin(double );
 extern double             sql__sqrt(double );
 extern long               sql__srand(long );
 extern void               sql__stopp(int );
 extern long               sql__succ(long ,long ,long, char *, int );
 extern void               sql__time(char *);
 extern long               sql__trunc(double );
 extern int                sql__ucmp(unsigned char *,unsigned char *,int );
 extern void               sql__unpack(long ,char *,char *,long ,long ,long ,long );
 extern int                sql__strlen ( unsigned char * );

# if defined ( _IBMR2 ) || defined ( SVR4 ) || defined(_WIN32) || defined(LINUX) || defined ( SOLARIS )
   extern void  _CDECL     sql__catchae(int );
# else
   extern void  _CDECL     sql__catchae(int, int);
# endif


#ifdef H00VARS_H_
  extern char              sql__1r(struct iorec *);
  extern void              sql__close(struct iorec *);
  extern void              sql__closea(struct iorec *);
  extern struct iorec    * sql__closep(struct iorec *,bool );
  extern void              sql__dfn(struct iorec *,char *,long ,long );
  extern bool              sql__endof(struct iorec *);
  extern bool              sql__endol(struct iorec *);
  extern long              sql__er(struct iorec *,void *);
  extern void              sql__fflush(struct iorec *);
  extern void              sql__get(struct iorec *);
  extern struct iorec    * sql__gn(struct iorec *,char *,long ,long );
  extern long              sql__intr(struct iorec *);
  extern void              sql__lexe(struct iorec *,long );
  extern double            sql__lor(struct iorec *);
  extern char            * sql__nlf(struct iorec *);
  extern void              sql__npa(struct iorec *);
  extern void              sql__pr(struct iorec *, char *,long );
  extern void              sql__put(struct iorec *);
  extern void              sql__readln(struct iorec *);
  extern void              sql__reset(struct iorec *,char *,long ,long );
  extern void              sql__rewrite(struct iorec *,char *,long ,long );
  extern void              sql__sync(struct iorec *);
  extern void              sql__uncs(struct iorec *);
  extern void              sql__unget(struct iorec *);
  extern struct iorec    * sql__ut(struct iorec *);
  extern void              sql__writeln(struct iorec *);
#endif /* H00VARS_H_ */

#endif /* LIBPC_H_ */

