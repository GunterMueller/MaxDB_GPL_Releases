#ifdef DEBUG
#else
#line 66 "vpa00enc"
#endif

/* Set ODBC-Version to 2.5 */
/* #define ODBCVER 0x0250 */

#ifndef APENV
#define APENV
/* check that one of the known platform macros is set */
#ifndef WIN32
#ifndef OS2
#ifndef WIN
#ifndef SCO
#ifndef HPUX
#ifndef AIX
#ifndef OSF1
#ifndef NMP
#ifndef SUN
#ifndef SOLARIS
#ifndef LINUX
#ifndef FREEBSD 
#if !(defined(SVR4) && defined(I386))
   ERROR - ERROR - ERROR: known platform macro not set!!!!!!
#endif      
#endif      
#endif      
#endif      
#endif      
#endif
#endif      
#endif
#endif
#endif
#endif
#endif
#endif

#if defined (WIN32) || defined (SOLARIS) || defined (SUN) || (defined(SVR4) && defined(I386)) || defined (LINUX)  /* PTS 1120833 */
#define ASYNCENABLED
#endif

#ifdef WIN32
#define WIN
#define EXPORT
#else
#ifdef WIN
#define EXPORT _export
#else
#define EXPORT
#endif
#endif

/* from sqlext.h */
#define SQL_TYPE_DRIVER_END (-97)

#if !defined(WIN32)
/* taken from sqlucode.h */
#define SQL_WCHAR		 	(-8)
#define SQL_WVARCHAR	 	(-9)
#define SQL_WLONGVARCHAR 	(-10)
#define SQL_C_WCHAR			SQL_WCHAR
#endif

#if 0
#ifndef _UCHAR
#define _UCHAR
typedef unsigned char       UCHAR;
#endif

#ifndef _CHAR
#define _CHAR
#if !defined(WIN32)
#define CHAR char
#endif
#endif

#if defined (WIN32) || defined (OSF1)
#ifndef _SDWORD
#define _SDWORD
typedef  long int           SDWORD;
#endif
#ifndef _UDWORD
#define _UDWORD
typedef unsigned long int   UDWORD;
#endif
#ifndef _LDOUBLE
#define _LDOUBLE
typedef      double         LDOUBLE;
#endif
#endif 
#endif /* if 0 */
#if defined(SVR4) && defined(I386)
#ifndef _LDOUBLE
#define _LDOUBLE
typedef      double         LDOUBLE;
#endif
#endif
#define I_ [
#define _I ]

#define API_AUTO_TABLESPACE

/* decide if we want to use the real <windows.h> or if he are on */
/* a non-windows platform. Setting WININC means use <windows.h>  */

#ifdef WIN
#define WININC
#endif

#ifdef WINTEST
#define WININC
#endif


/* for those platforms that support function prototypes,        */
/* turn on the PROTOTYPE macro                                  */

#undef PROTOTYPE
#ifdef WIN
#define PROTOTYPE
#endif
#ifdef WINTEST
#define PROTOTYPE
#endif
#ifdef PC
#define PROTOTYPE
#endif
#ifdef __STDC__
#define PROTOTYPE
#endif
/* for the ibm mainframe we must use cryptic (short) function   */
/* names, turning on CRYPTIC_NAMES enables short names          */

#undef CRYPTIC_NAMES
#ifdef IBM
#define CRYPTIC_NAMES
#endif


/* determine which include files should be used */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <limits.h>

#ifndef IBM 
#ifndef FREEBSD
#include <malloc.h>
#endif
#include <memory.h>
#endif


/* set special macros that are used to isolate the normal C     */
/* functions since Windows DLLs demand the use of special C     */
/* functions.                                                   */

#ifndef WIN
#define API_EXIT(x) exit(x)
#define API_EXIT_MSG(msg) {printf(" %s\n", msg); exit(16);}
#define API_SPRINTF sprintf
#define CALL_ODBC( proc, parm ) pa##proc parm
#define ODBC_PROC(proc, parm, arg) SQLRETURN SQL_API pa##proc parm;\
SQLRETURN SQL_API proc parm \
{ return(pa##proc arg); } \
SQLRETURN SQL_API pa##proc parm
#else
#define API_EXIT(x) FatalAppExit(0,"API_EXIT Called.") 
#define API_EXIT_MSG(msg) FatalAppExit(0,msg)
#define API_SPRINTF wsprintf
#define CALL_ODBC( proc, parm ) proc parm 
#define ODBC_PROC(proc, parm, arg) RETCODE SQL_API proc parm
#endif

#define API_MEMCPY memcpy
#define API_MEMSET memset
#define API_MEMCMP memcmp
#define API_MEMCHR memchr
#define API_STRCPY(s1,s2) strcpy((char*) s1, (const char*) s2)
#define API_STRNCPY(s1,s2,n) strncpy((char*) s1, (const char*) s2,n)
#define API_STRCMP(s1,s2) strcmp((const char*) s1, (const char*) s2)
#define API_STRNCMP(s1,s2,n) strncmp((char*) s1, (char*) s2,n)
#define API_STRLEN(s) strlen((const char*) s)
#define API_STRCAT(s1,s2) strcat((char*)s1,(char*)s2)
#define API_STRNCAT(s1,s2,n) strncat((char*) s1, (char*) s2,n)
#define API_STRCHR(s,c) strchr((char*) s, (char) c)
#define API_STRRCHR strrchr
#define API_STRCSPN(s1,s2) strcspn((char*) s1,(char*) s2)
#define API_NEAR(p) p
#ifndef FREEBSD
#define API_ECVT ecvt
#else
// i think we should port this correctly for a "RELEASE"
// FreeBSD does not natively knows ecvt, but i was not sure
// where to put the code for that ...
#define API_ECVT(a,b,c,d) 0 
#endif

/* Access is needed to some of the the SAG development includes  */
/* that contain things like bits per byte, max size of int, etc. */

#ifndef sql_sqldb
#define sql_sqldb
#endif

/* constant related to        */
/* floating point arithmetic  */
#define SIGN_NEGATIV 1
#define SIGN_POSITIV 0

#define PRINT_SHORT  "%d"
#define PRINT_LONG   "%d"           /* PTS 1119281 */
#define PRINT_FLOAT  "%-1e"
#define PRINT_DOUBLE  "%-1.18e"  /*depending on API_DOUBLE_PREC */
#ifdef WIN
#define CRLF "\x0D\x0A"
#else
#define CRLF "\x0A"
#endif
#endif /* ifdef APENV */

/* end of apenv.h */
