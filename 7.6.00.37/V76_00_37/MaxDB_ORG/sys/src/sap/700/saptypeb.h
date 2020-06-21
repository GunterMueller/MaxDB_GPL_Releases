/* @(#) $Id: //sapdb/V76/c_00/b_37/sys/src/sap/700/saptypeb.h#1 $ SAP*/
/* CCQ_CCU_FILE_OFF */
/**********************************************************************/
/*                                                                    */
/*  SAP  Systeme, Anwendungen und Produkte in der Datenverarbeitung   */
/*                                                                    */
/*  This is a part of the central include file, which should be       */
/*  included by all SAP C software.                                   */
/*                                                                    */
/**********************************************************************/


#ifndef SAPTYPEB_H
#define SAPTYPEB_H  1


/* ANSI concurring includes ..........................................*/

#if defined(CPP_USE_NEW_C_HEADERS)

  #include <cfloat>      /* Characteristics of floating types ..........*/
  #include <climits>     /* Sizes of integral types ....................*/
  #include <cstddef>     /* Common definitions .........................*/
  #include <cctype>

  #ifndef NO_STDIO_H
    #include <cstdio>    /* Input/output ...............................*/
  #endif

  #ifndef NO_STDLIB_H
    #include <cstdlib>   /* General utilities ..........................*/
  #endif

  #ifndef NO_STRING_H
    #include <cstring>   /* String handling ............................*/
  #endif

#else

  #include <float.h>     /* Characteristics of floating types ..........*/
  #include <limits.h>    /* Sizes of integral types ....................*/
  #include <stddef.h>    /* Common definitions .........................*/
  #include <ctype.h>

  #ifndef NO_STDIO_H
    #include <stdio.h>   /* Input/output ...............................*/
  #endif

  #ifndef NO_STDLIB_H
    #include <stdlib.h>  /* General utilities ..........................*/
  #endif

  #ifndef NO_STRING_H
    #include <string.h>  /* String handling ............................*/
  #endif

#endif


/**********************************************************************/
/*                                                                    */
/* Platform identification                                            */
/*                                                                    */
/* SAP_SYSTEMID can be used to uniquely identify the platform, on     */
/* which R/3 is running.                                              */
/*                                                                    */
/* Conventions:                                                       */
/*   0x0???  ASCII platforms                                          */
/*   0x01??   Unix                                                    */
/*   0x0110    HP-UX                                                  */
/*   0x0120    Digital Unix                                           */
/*   0x0140    IBM AIX                                                */
/*   0x0142    SNI Sinix (RM/600)                                     */
/*   0x0144    SGI IRIX                                               */
/*   0x0171    Solaris SPARC 32-bit                                   */
/*   0x0172    Solaris SPARC 64-bit                                   */
/*   0x0173    Solaris X86   32-bit                                   */
/*   0x0174    Solaris X86   64-bit                                   */
/*   0x0210   DOS                                                     */
/*   0x0220   OS/2                                                    */
/*   0x0230   NT                                                      */
/*   0x0310   OpenVMS on VAX (still alive!)                           */
/*   0x0320   OpenVMS on Alpha (still alive!)                         */
/*   0x0400   MacOS <9.x                                              */
/*   0x0401   MacOSX (Darwin)                                         */
/*                                                                    */
/*   0x1???  EBCDIC platforms                                         */
/*   0x11??   AS/400                                                  */
/*                                                                    */
/*                                                                    */
/* SAP_SYSTEMID should be set within the platform specific file       */
/* type<platform>.h                                                   */
/*                                                                    */
/**********************************************************************/


/**********************************************************************/
/*                                                                    */
/* Current SAP release and macro to print it                          */
/*                                                                    */
/* SAP_RELEASE_NO will never be decremented.                          */
/*                                                                    */
/* SAP_RELEASE_S        *internal* macro to enforce that              */
/*                      both SAP_RELEASE and SAP_RELEASE_R have the   */
/*                      same "value"                                  */
/*                      this is the value that must be edited to      */
/*                      change the value from SAP_RELEASE             */
/*                                                                    */
/* SAP_RELEASE_NO       for numeric comparisons                       */
/*                                                                    */
/* SAP_RELEASE          for *normal* SAP_UC use                       */
/*                                                                    */
/* SAP_RELEASE_R        for "what" strings and other char uses        */
/*                                                                    */
/* SAP_RELEASE_AR       for character array initialization            */
/*                                                                    */
/* SAP_RELEASE_NO_STR   for frontend programs (Resources)             */
/*                      because RC has a poor preprocessor we cannot  */
/*                      cNtoS here                                    */
/*                                                                    */
/* SAP_RELEASE_NO_STR_R for "what" strings and other char uses        */
/**********************************************************************/

#define SAP_RELEASE_S           "700"   /* the string value  WARNING: before changing this line contact MakeFactory as it is used by make.unix */
#define SAP_RELEASE_NO           7000   /* for numeric comparisons */
#define SAP_RELEASE_NO_STR   cU("7000") /* for frontend programs   */
#define SAP_RELEASE_NO_STR_R    cR( cNtoS( SAP_RELEASE_NO) ) /* for what strings        */

#define SAP_RELEASE             cU( SAP_RELEASE_S ) /* for *normal* use                 */
#define SAP_RELEASE_R           cR( SAP_RELEASE_S ) /* for "what" strings               */
#define SAP_RELEASE_AR          iU( SAP_RELEASE_S ) /* for char array initialization    */
/* GUID that represents the current release. Must be changed when SAP_RELEASE changes !!!*/
/* Use Microsoft tool "guidgen" to create a new GUID */
#define SAP_RELEASE_GUID	cU("{BA16BBA9-18F4-4fef-866A-F86AC32C674B}")

/* GUID that represents the SAP AG, never change it !!!*/
#define SAP_GUID 		cU("{68EE92D6-C017-4209-95A5-360C0DB40426}")
/* GUID that represents the R/3 Product, never change it !!! */
#define SAP_R3_GUID		cU("{2B2191AB-DB06-4683-A1E6-ED2A3533EBE4}")

#define PRINT_SAP_RELEASE(argc, argv)                        \
{                                                            \
  if ((argc) == 2)                                           \
  {                                                          \
    if (strcmpU(argv[1], cU("-V")) == 0)                     \
    {                                                        \
      printfU( cU("SAP release: %s  SAP release no: %d \n"), \
               SAP_RELEASE, SAP_RELEASE_NO);                 \
      exit (0);                                              \
    }                                                        \
  }                                                          \
}

/**********************************************************************/
/* SAP_CONST:  pre ANSI relic, now use 'const' directly               */
/**********************************************************************/
#define SAP_CONST const

/* Uncomment the following line to test Const-Correctness. In the sources,
 * conZt will be replaced by const soon. (d025846 2004-07-07) */
/* #define conZt const */
#define conZt



/**********************************************************************/
/*                                                                    */
/* SAP_MAX_ALIGN_T:                                                   */
/* structure with maximum possible size, used for alignment           */
/*                                                                    */
/* SAP_MAX_ALIGN:                                                     */
/* maximum alignment required for any integral data type              */
/*                                                                    */
/* If a certain platform contains a type that exceeds the size of     */
/* the C-defined integral data types, define PLATFORM_MAX_T in the    */
/* platform dependent part of saptype.h                               */
/*                                                                    */
/* Example: (in saptype.h)                                            */
/*                                                                    */
/* #ifdef SAPon<platform>                                             */
/* union                                                              */
/* {                                                                  */
/*   longlong my_largest_type;                                        */
/* } PLATFORM_MAX_T                                                   */
/* #endif                                                             */
/*                                                                    */
/**********************************************************************/
#ifndef PLATFORM_MAX_T
typedef void * PLATFORM_MAX_T;
#endif

typedef union
{
  long               align_1;
  double             align_2;
  void *             align_3;
  PLATFORM_MAX_T     align_4;
} SAP_MAX_ALIGN_T;

/* Worst allignment, that may be needed.
 * In bytes.
 */
#define SAP_MAX_ALIGN ( sizeofR( SAP_MAX_ALIGN_T))

/* Allign a size 'i' to a multiple of 'a'.
 * Both parameters and result in bytes.
 */
#ifndef SAP_ALIGN_SIZE_TO
#define SAP_ALIGN_SIZE_TO(i,a) (((i)%(a))?((i)+((a)-((i)%(a)))):(i))
#endif

/* Allign a size 'i' safely.
 * Parameter and result in bytes.
 */
#ifndef SAP_ALIGN_SIZE_TO_MAX
#define SAP_ALIGN_SIZE_TO_MAX(i) (SAP_ALIGN_SIZE_TO(i,SAP_MAX_ALIGN))
#endif

/* Allign a pointer 'p' to a multiple of 'a'.
 * Parameter 'p' should be (void*).
 * Parameter 'a' in bytes.
 * Result is (void*).
 * Note: please try to avoid casting pointers to integral
 * datatypes (AS400, etc...).
 * if absolutely necessary, use (SAP_ULONG_PTR, long with pointer precision)
 * instead of(long)
 * (-> see e.g. HP-UX 64 Bit address model )
 */
#ifndef SAP_ALIGN_PTR_TO

#ifdef SAPonOS400
    #include <mispcobj.h>
    #define SAP_ALIGN_PTR_TO(p,a)                                          \
      ((stsppo((_SPCPTR)(p))%(a))                                          \
       ? ((void*)(((SAP_RAW*)(p))+((a)-(stsppo((_SPCPTR)(p))%(a)))))          \
       : ((void*)(p)))

#else
#define SAP_ALIGN_PTR_TO(p,a)                                          \
  (                                                                    \
    (((SAP_ULONG_PTR)(p))%(a))                                         \
    ? ((void*)                                                         \
	(((SAP_RAW*)(p))  +  ( (a) - (((SAP_ULONG_PTR)(p))%(a)) ))     \
      )                                                                \
    : (p)                                                              \
  )
#endif
#endif

/* Allign a pointer 'p' safely.
 * Parameter 'p' should be (void*).
 * Result is (void*).
 */
#ifndef SAP_ALIGN_PTR_TO_MAX
#define SAP_ALIGN_PTR_TO_MAX(p) (SAP_ALIGN_PTR_TO(p,SAP_MAX_ALIGN))
#endif

/**********************************************************************/
/* SAP_ANYPTR:  pre ANSI relic, now use 'void *' directly             */
/* SAP_VOIDPTR:  pre ANSI relic, now use 'void *' directly            */
/*                                                                    */
/* Use macros BYTE_OFFSET and BYTE_OFFSET_NEG for pointer arithmetic  */
/* with void pointers. BYTE_OFFSET adds the integral offset,          */
/* BYTE_OFFSET_NEG subtracts it.                                      */
/*                                                                    */
/* CAVEAT: bus error (wrong alignment)                                */
/**********************************************************************/
typedef void *  SAP_ANYPTR;
typedef void *  SAP_VOIDPTR;

#define BYTE_OFFSET(ptr, offset)  \
  ((void *)((SAP_RAW *)(void *)(ptr) + (offset)))
#define BYTE_OFFSET_NEG(ptr, offset)  \
  ((void *)((SAP_RAW *)(void *)(ptr) - (offset)))


/**********************************************************************/
/*                                                                    */
/* SAP_DCi ():  pre ANSI relic, now use ANSI declarations directly    */
/* SAP_DFi ():  pre ANSI relic, now use ANSI definitions directly     */
/* SAP_DPi ():  pre ANSI relic, now use ANSI declarations directly    */
/*                                                                    */
/* Macros for parameter lists in                                      */
/*                                                                    */
/*  - function declarations (SAP_DCi) (external or forward) and       */
/*  - function definitions  (SAP_DFi) (header of procedure) and       */
/*  - function pointer declarations (SAP_DPi)                         */
/*                                                                    */
/* where i == number of parameters (0 <= i <= 13).                    */
/*                                                                    */
/* These macros expand to                                             */
/*                                                                    */
/*  - ANSI style declarations and definitions                         */
/*                                                                    */
/* The macros accept only parameter declarations with identifiers in  */
/* the rightmost position. This restriction can always be met by      */
/* declaring new types with typedef, if neccessary.                   */
/*                                                                    */
/* Examples:                                                          */
/*                                                                    */
/* typedef char  * ARRAY_3 [3] ;                                      */
/* typedef int  (* FCT_PTR) SAP_DP1 (SAP_BYTE *, byte_ptr);           */
/*                                                                    */
/* char * test1 SAP_DC0 ();                                           */
/* void   test2 SAP_DC3 (char *, ptr, int, count, SAP_USHORT, length);*/
/* int    test3 SAP_DC2 (ARRAY_3, array, FCT_PTR, fptr);              */
/*                                                                    */
/* char * test1 SAP_DF0 ()                                            */
/* { ... }                                                            */
/*                                                                    */
/* void   test2 SAP_DF3 (char *, ptr, int, count, SAP_USHORT, length) */
/* { ... }                                                            */
/*                                                                    */
/* int    test3 SAP_DF2 (ARRAY_3, array, FCT_PTR, fptr)               */
/* { ... }                                                            */
/*                                                                    */
/**********************************************************************/
#define SAP_DF0()                                                      \
  (void)
#define SAP_DF1(A,a)                                                   \
  (A a)
#define SAP_DF2(A,a,B,b)                                               \
  (A a, B b)
#define SAP_DF3(A,a,B,b,C,c)                                           \
  (A a, B b, C c)
#define SAP_DF4(A,a,B,b,C,c,D,d)                                       \
  (A a, B b, C c, D d)
#define SAP_DF5(A,a,B,b,C,c,D,d,E,e)                                   \
  (A a, B b, C c, D d, E e)
#define SAP_DF6(A,a,B,b,C,c,D,d,E,e,F,f)                               \
  (A a, B b, C c, D d, E e, F f)
#define SAP_DF7(A,a,B,b,C,c,D,d,E,e,F,f,G,g)                           \
  (A a, B b, C c, D d, E e, F f, G g)
#define SAP_DF8(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h)                       \
  (A a, B b, C c, D d, E e, F f, G g, H h)
#define SAP_DF9(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i)                   \
  (A a, B b, C c, D d, E e, F f, G g, H h, I i)
#define SAP_DF10(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i,J,j)              \
  (A a, B b, C c, D d, E e, F f, G g, H h, I i, J j)
#define SAP_DF11(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i,J,j,K,k)          \
  (A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k)
#define SAP_DF12(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i,J,j,K,k,L,l)      \
  (A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l)
#define SAP_DF13(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i,J,j,K,k,L,l,M,m)  \
  (A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m)
#define SAP_DF14(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i,J,j,K,k,L,l,M,m,N,n)  \
  (A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m, N n)
#define SAP_DF15(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i,J,j,K,k,L,l,M,m,N,n,O,o)  \
  (A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m,N n,O o)

#define SAP_DC0()                                                      \
  SAP_DF0()
#define SAP_DC1(A,a)                                                   \
  SAP_DF1(A,a)
#define SAP_DC2(A,a,B,b)                                               \
  SAP_DF2(A,a,B,b)
#define SAP_DC3(A,a,B,b,C,c)                                           \
  SAP_DF3(A,a,B,b,C,c)
#define SAP_DC4(A,a,B,b,C,c,D,d)                                       \
  SAP_DF4(A,a,B,b,C,c,D,d)
#define SAP_DC5(A,a,B,b,C,c,D,d,E,e)                                   \
  SAP_DF5(A,a,B,b,C,c,D,d,E,e)
#define SAP_DC6(A,a,B,b,C,c,D,d,E,e,F,f)                               \
  SAP_DF6(A,a,B,b,C,c,D,d,E,e,F,f)
#define SAP_DC7(A,a,B,b,C,c,D,d,E,e,F,f,G,g)                           \
  SAP_DF7(A,a,B,b,C,c,D,d,E,e,F,f,G,g)
#define SAP_DC8(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h)                       \
  SAP_DF8(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h)
#define SAP_DC9(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i)                   \
  SAP_DF9(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i)
#define SAP_DC10(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i,J,j)              \
  SAP_DF10(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i,J,j)
#define SAP_DC11(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i,J,j,K,k)          \
  SAP_DF11(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i,J,j,K,k)
#define SAP_DC12(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i,J,j,K,k,L,l)      \
  SAP_DF12(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i,J,j,K,k,L,l)
#define SAP_DC13(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i,J,j,K,k,L,l,M,m)  \
  SAP_DF13(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i,J,j,K,k,L,l,M,m)
#define SAP_DC14(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i,J,j,K,k,L,l,M,m,N,n)  \
  SAP_DF14(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i,J,j,K,k,L,l,M,m,N,n)
#define SAP_DC15(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i,J,j,K,k,L,l,M,m,N,n,O,o)  \
  SAP_DF15(A,a,B,b,C,c,D,d,E,e,F,f,G,g,H,h,I,i,J,j,K,k,L,l,M,m,N,n,O,o)

#define SAP_DP0()                                                      \
  SAP_DF0()
#define SAP_DP1(A,a)                                                   \
  SAP_DF1(A,a)
#define SAP_DP2(A,a,B,b)                                               \
  SAP_DF2(A,a,B,b)
#define SAP_DP3(A,a,B,b,C,c)                                           \
  SAP_DF3(A,a,B,b,C,c)
#define SAP_DP4(A,a,B,b,C,c,D,d)                                       \
  SAP_DF4(A,a,B,b,C,c,D,d)
#define SAP_DP5(A,a,B,b,C,c,D,d,E,e)                                   \
  SAP_DF5(A,a,B,b,C,c,D,d,E,e)


/**********************************************************************/
/* For every platform that supports Unicode, one of the preprocessor  */
/* switches                                                           */
/*                                                                    */
/*   SAP_UC_is_wchar    or      SAP_UC_is_UTF16_without_wchar         */
/*                                                                    */
/* must be set here, because SAP_UC and SAP_CHAR depend on it.        */
/* For platforms where string constants must be edited by a special   */
/* parse tool, the switch SAPwithoutUTF16_LITERALS must be set in     */
/* addition.                                                          */
/**********************************************************************/

#ifdef SAPonAIX
  #if defined(_AIX51) && defined(SAPwith64_BIT)
    #define SAPonAIX_wchar_is_4B
  #elif defined(SAPccQ)
    #define SAPonAIX_wchar_is_4B
  #else
    #define SAPonAIX_wchar_is_2B
  #endif
#endif

#if defined(SAPonNT)      || \
    defined(SAPonOS400)   || \
    (defined(SAPonOS390) && !defined(_LP64))  || \
    (defined(SAPonAIX) && defined(SAPonAIX_wchar_is_2B))
  #define WCHAR_is_2B
#else
  #define WCHAR_is_4B
#endif


#ifndef SAPwithUNICODE
  #define SAP_UC_is_1B
#else  /* SAPwithUNICODE */
  #if defined(WCHAR_is_2B)
    #define SAP_UC_is_wchar
  #else
    #define SAP_UC_is_UTF16_without_wchar
  #endif
#endif /* SAPwithUNICODE or not */

#if defined(WCHAR_is_2B) || \
    defined(SAPonHP_UX) || \
    (defined(SAPonLIN) && defined(__i386__) && defined(__GNUC__) && (__GNUC__<3)) || \
    (defined(SAPonSUN) && defined(__cplusplus) && (defined(__sparc) || defined(__sparcv9))) || \
    (defined(SAPonSUN) && (defined(__i386) || defined(__amd64)))
  /* we have literals for UTF-16 */
#else
  #define SAPwithoutUTF16_LITERALS
#endif


/**********************************************************************/
/* SAP_CHAR and SAP_UC:  char or wchar_t or unsigned short            */
/**********************************************************************/
#ifndef SAPwithUNICODE

  typedef char          SAP_CHAR;      /* Range: CHAR_MIN ... CHAR_MAX */
  typedef char          SAP_UC;

  #if defined(SAPonHP_UX) || defined(SAPonSUN)
    #define SAP_CHAR_MIN  (0)
    #define SAP_UC_MIN    (0)
    #define SAP_CHAR_MAX  UCHAR_MAX
    #define SAP_UC_MAX    UCHAR_MAX
  #else
    #define SAP_CHAR_MIN  CHAR_MIN
    #define SAP_UC_MIN    CHAR_MIN
    #define SAP_CHAR_MAX  CHAR_MAX
    #define SAP_UC_MAX    CHAR_MAX
  #endif

#else /* SAPwithUNICODE */

  #if   defined(SAP_UC_is_wchar)
    typedef wchar_t       SAP_CHAR;
    typedef wchar_t       SAP_UC;
  #elif defined(SAP_UC_is_UTF16_without_wchar)
    typedef unsigned short SAP_CHAR;
    typedef unsigned short SAP_UC;
  #endif

  #define SAP_CHAR_MIN  (0)
  #define SAP_UC_MIN    (0)

  #ifdef SAP_UC_is_wchar
    #if defined(SAPonNT)    || \
        defined(SAPonLIN)   || \
        defined(SAPonOSF1)                      /* WCHAR_MAX is defined */
      #define SAP_CHAR_MAX   WCHAR_MAX
      #define SAP_UC_MAX     WCHAR_MAX
    #elif defined(SAPonAIX)    || \
          defined(SAPonOS390)  || \
          defined(SAPonHP_UX)  || \
          defined(SAPonSUN)    || \
          defined(SAPonOS400)               /* WCHAR_MAX is not defined */
      #define SAP_CHAR_MAX   ((wchar_t)~0)
      #define SAP_UC_MAX     ((wchar_t)~0)
    #else
      #error "SAP_CHAR_MAX not yet defined on this platform"
    #endif /* different platforms */
  #elif defined(SAP_UC_is_UTF16_without_wchar)
      #define SAP_CHAR_MAX   ((SAP_CHAR)~0)
      #define SAP_UC_MAX     ((SAP_UC)~0)
  #endif

#endif /* SAPwithUNICODE */

typedef SAP_UC*         SAP_UCS;
#define                 UC_NULL         ((SAP_UC)0)

/**********************************************************************/
/* SAP_BYTE  and  SAP_RAW                                             */
/**********************************************************************/
/* We created a problem here without any good reason:                 */
/* 'SAP_BYTE' was intended to be the basic SAP datatype to handle     */
/* bytes of eight bits each.                                          */
/* In Standard C it was declared (unsigned char) of course.           */
/* But unfortunately it also was documented as "unsigned char".       */
/* During the years of R/3 development most colleagues used SAP_BYTE  */
/* for bytes. But quite a large number of programmes used SAP_BYTE    */
/* when they needed an unsigned character.                            */
/* When moving to Unicode we have to separate: bytes will stay bytes, */
/* but unsigned characters will become Unicode characters.            */
/* So:                                                                */
/*     If you really mean bytes, please use the new type 'SAP_RAW'.   */
/*     If you need unsigned characters, then use 'unsigned char'.     */
/*     If you need a signed byte, use 'SAP_SRAW'.                     */
/*     'SAP_BYTE' shall no longer be used.                            */
/*--------------------------------------------------------------------*/
typedef unsigned char SAP_BYTE;        /* Value range: 0 .. UCHAR_MAX */
#define SAP_BYTE_MAX  UCHAR_MAX

typedef unsigned char   SAP_RAW;       /* A byte. Range 0 ..UCHAR_MAX */
#define SAP_RAW_MAX     UCHAR_MAX
typedef   signed char   SAP_SRAW;      /* A byte. (-128)-127 .. 128   */
#define          	RAW_NULL   ((SAP_RAW)0)

/**********************************************************************/
/* The data types beginning with SAPt... are for the intermediate use */
/* during the Unicode conversion of the sources. They can be used     */
/* to indicate the future data type without destroying coding that    */
/* is not yet converted.                                              */
/**********************************************************************/
#ifndef SAPwithUNICODE

  typedef unsigned char SAPtUC;
  typedef unsigned char SAPtCHAR;
  typedef char          SAPtRAW;

  #define SAPtvoidUC    void
  #define SAPtUCvoid    SAP_UC
  #define SAPtvoidCHAR  void
  #define SAPtCHARvoid  SAP_CHAR
  #define SAPtvoidRAW   void
  #define SAPtBYTEvoid  SAP_RAW  /* must be SAP_RAW, because SAP_BYTE
                                  * and unsigned char would lead to
                                  * a ccU warning
                                  */

#else

  typedef SAP_UC        SAPtUC;
  typedef SAP_CHAR      SAPtCHAR;
  typedef unsigned char SAPtRAW;

  #define SAPtvoidUC    SAP_UC
  #define SAPtUCvoid    void
  #define SAPtvoidCHAR  SAP_CHAR
  #define SAPtCHARvoid  void
  #define SAPtvoidRAW   SAP_RAW
  #define SAPtBYTEvoid  void

#endif

/**********************************************************************/
/* SAP_SHORT:  short  (2 bytes)                                       */
/**********************************************************************/
#if SHRT_MAX == 0x7FFF
  typedef short SAP_SHORT;       /* Value range: SHRT_MIN .. SHRT_MAX */
  #define SAP_SHORT_MIN  SHRT_MIN
  #define SAP_SHORT_MAX  SHRT_MAX
#else
  #error "We need a short type with 2 bytes"
#endif
#define SAP_SHORT_BYTES  2
/* for classifying character length and byte length */
/*SAPUNICODEOK_CHARLEN*/ typedef SAP_SHORT SAP_SHORT_U;
/*SAPUNICODEOK_RAWLEN*/  typedef SAP_SHORT SAP_SHORT_R;


/**********************************************************************/
/* SAP_USHORT:  unsigned short  (2 bytes)                             */
/**********************************************************************/
#if USHRT_MAX == 0xFFFFu
  typedef unsigned short SAP_USHORT;              /* Value range:     */
                                                  /*   0 .. USHRT_MAX */
  #define SAP_USHORT_MAX  USHRT_MAX
#else
  #error "We need a unsigned short type with 2 bytes"
#endif
#define SAP_USHORT_BYTES  2
/* for classifying character length and byte length */
/*SAPUNICODEOK_CHARLEN*/ typedef SAP_USHORT SAP_USHORT_U;
/*SAPUNICODEOK_RAWLEN*/  typedef SAP_USHORT SAP_USHORT_R;


/**********************************************************************/
/* SAP_INT:  signed integer type (4 bytes)                            */
/**********************************************************************/
#if INT_MAX == 0x7FFFFFFF
  typedef int SAP_INT;                        /* Value range:         */
                                              /*   INT_MIN .. INT_MAX */
  #define SAP_INT_MIN  INT_MIN
  #define SAP_INT_MAX  INT_MAX

#elif LONG_MAX == 0x7FFFFFFF
  typedef long SAP_INT;                     /* Value range:           */
                                            /*   LONG_MIN .. LONG_MAX */
  #define SAP_INT_MIN  LONG_MIN
  #define SAP_INT_MAX  LONG_MAX

#else
  #error "We need an int type with 4 bytes"
#endif
#define SAP_INT_BYTES  4
/* for classifying character length and byte length */
/*SAPUNICODEOK_CHARLEN*/ typedef SAP_INT SAP_INT_U;
/*SAPUNICODEOK_RAWLEN*/  typedef SAP_INT SAP_INT_R;


/**********************************************************************/
/* SAP_UINT:  unsigned integer type (4 bytes)                         */
/**********************************************************************/
#if UINT_MAX == 0xFFFFFFFFu
  typedef unsigned int SAP_UINT;                   /* Value range:    */
                                                   /*   0 .. UINT_MAX */
  #define SAP_UINT_MAX  UINT_MAX

#elif ULONG_MAX == 0xFFFFFFFFu
  typedef unsigned long SAP_UINT;                 /* Value range:     */
                                                  /*   0 .. ULONG_MAX */
  #define SAP_UINT_MAX  ULONG_MAX

#else
  #error "We need an unsigned int type with 4 bytes"
#endif
#define SAP_UINT_BYTES  4
/* for classifying character length and byte length */
/*SAPUNICODEOK_CHARLEN*/ typedef SAP_UINT SAP_UINT_U;
/*SAPUNICODEOK_RAWLEN*/  typedef SAP_UINT SAP_UINT_R;


/**********************************************************************/
/* Obsolete: SAP_ULONG:  unsigned integer type (4 bytes)              */
/* (identical with SAP_UINT, shall be replaced)                       */
/**********************************************************************/
typedef SAP_UINT  SAP_ULONG;
#define SAP_ULONG_MAX  UINT_MAX
#define SAP_ULONG_BYTES  SAP_UINT_BYTES

/**********************************************************************/
/* SAP_LLONG:  signed 64 bit integer (8 bytes)                        */
/* SAP_ULLONG: unsigned 64 bit integer                                */
/*                                                                    */
/* !!!!!!!!!!!!!!!!!!!!! ATTENTION !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  */
/* SAP_[U]LLONG2STR, SAP_STR2LLONG are not yet implemented on all     */
/* platforms, but they are available on the kernel platforms.         */
/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  */
/*                                                                    */
/*   SAP_[U]LLONG           basic 64 bit integer data type            */
/*   SAP_[U]LLONG_BYTES     # bytes used for representation in memory */
/*   SAP_[U]LLONG_MAX       largest representable value               */
/*   SAP_[U]LLONG_MIN       least (negative) representable value      */
/*   SAP_[U]LLONG_CONST     wrapper for literal integer constants     */
/*   SAP_[U]LLONG2STR       write decimal repr. to string (Unicode)   */
/*   SAP_STR2LLONG          read decimal repr. from string (Unicode)  */
/*                                                                    */
/*    This data type provides a means of calculating with             */
/*    very large integer numbers: e.g. memory/file sizes > 2GB,       */
/*    nano-second counters etc., on 64-bit as well as 32-bit          */
/*    platforms.                                                      */
/*                                                                    */
/* Example of usage:                                                  */
/*                                                                    */
/*   SAP_UC buf1[22], buf2[80];                                       */
/*   SAP_LLONG x,y = SAP_LLONG_CONST( 100000000000 );                 */
/*   x = y + SAP_LLONG_CONST( 1234567890000 );                        */
/*   SAP_LLONG2STR( buf1, x );                                        */
/*   snprintfU( buf2, 80, cU("Size = %")SAP_Flld cU("\n"), x );       */
/**********************************************************************/

#define SAP_LLONG_BYTES 8
#define SAP_ULLONG_BYTES 8

#if defined SAPwith64_BIT && \
    ! defined SAPonNT && \
    ! defined SAPonOS400 && \
    ! defined SAPccQ

  typedef          long SAP_LLONG;   /* this requires the LP64 data model */
  typedef unsigned long SAP_ULLONG;

  #define SAP_LLONG_CONST( x )  ( CCQ_CAST(SAP_LLONG) x##L )
  #define SAP_ULLONG_CONST( x ) ( CCQ_CAST(SAP_ULLONG) x##UL )
  #define SAP_STR2LLONG( value, str )  (value = atolU(str))

  /* format identifier for printfU....*/
  #define SAP_Flld  CCQ_FMT("Flld") cU("ld")
  #define SAP_Fllu  CCQ_FMT("Fllu") cU("lu")
  #define SAP_Fllx  CCQ_FMT("Fllx") cU("lx")

#elif defined SAPonHPPA   || \
      defined SAPonRS6000 || \
      defined SAPonSUN    || \
      defined SAPonOS390  || \
      defined SAPonOS400  || \
      defined SAPonRM600  || \
      defined SAPonApple  || \
      defined SAPonDARWIN || \
      defined SAPonLIN    || \
      defined SAPonPTX    || \
      defined SAPccQ

  typedef          long long SAP_LLONG;
  typedef unsigned long long SAP_ULLONG;

  #define SAP_LLONG_CONST( x )  ( CCQ_CAST(SAP_LLONG) x##LL )
  #define SAP_ULLONG_CONST( x ) ( CCQ_CAST(SAP_ULLONG) x##ULL )
  externC SAP_LLONG sap_str2llong( const SAP_UC * const str );
  #define sap_str2llong_needed
  #define SAP_STR2LLONG( value, str ) (value = sap_str2llong(str))

  /* format identifier for printfU....*/
  #define SAP_Flld  CCQ_FMT("Flld") cU("lld")
  #define SAP_Fllu  CCQ_FMT("Fllu") cU("llu")
  #define SAP_Fllx  CCQ_FMT("Fllx") cU("llx")

#elif defined SAPonOS2_2x

  typedef long SAP_LLONG;

  /* format identifier for printfU....*/
  #define SAP_Flld  CCQ_FMT("Flld") cU("ld")
  #define SAP_Fllu  CCQ_FMT("Fllu") cU("lu")
  #define SAP_Fllx  CCQ_FMT("Fllx") cU("lx")

#elif defined SAPonNT

  typedef          __int64 SAP_LLONG;
  typedef unsigned __int64 SAP_ULLONG;

  #define SAP_LLONG_CONST( x )  ( CCQ_CAST(SAP_LLONG) x##i64 )
  #define SAP_ULLONG_CONST( x ) ( CCQ_CAST(SAP_ULLONG) x##ui64 )
  externC SAP_LLONG sap_str2llong( const SAP_UC * const str );
  #define sap_str2llong_needed
  #define SAP_STR2LLONG( value, str )  (value = sap_str2llong(str))

  /* format identifier for printfU....*/
  #define SAP_Flld  CCQ_FMT("Flld") cU("I64d")
  #define SAP_Fllu  CCQ_FMT("Fllu") cU("I64u")
  #define SAP_Fllx  CCQ_FMT("Fllx") cU("I64x")

#endif

#define SAP_LLONG2STR( str, value )  sprintfU(str,cU("%") SAP_Flld, value)
#define SAP_ULLONG2STR( str, value ) sprintfU(str,cU("%") SAP_Fllu, value)

#if defined SAP_LLONG_CONST  	  /* may be not be defined on all PFs */

  #define SAP_LLONG_MAX   SAP_LLONG_CONST(  9223372036854775807 )
  #define SAP_LLONG_MIN ( SAP_LLONG_CONST( -9223372036854775807 )  - 1)
  #define SAP_ULLONG_MAX SAP_ULLONG_CONST( 18446744073709551615 )

#endif
/* for classifying character length and byte length */
/*SAPUNICODEOK_CHARLEN*/ typedef SAP_LLONG  SAP_LLONG_U;
/*SAPUNICODEOK_RAWLEN*/  typedef SAP_LLONG  SAP_LLONG_R;

#if !defined(SAPonOS2_2x)
/*SAPUNICODEOK_CHARLEN*/ typedef SAP_ULLONG SAP_ULLONG_U;
/*SAPUNICODEOK_RAWLEN*/  typedef SAP_ULLONG SAP_ULLONG_R;
#endif

/**********************************************************************/
/* SAP_BOOL:   boolean value (TRUE or FALSE)                          */
/**********************************************************************/
#ifdef SAP_BOOL_ENUM
  typedef enum { FALSE = 0, TRUE = 1 } SAP_BOOL;
#else
  typedef unsigned char SAP_BOOL;
  #ifndef FALSE
    #define FALSE  0
  #endif

  #ifndef TRUE
    #define TRUE  1
  #endif
#endif

/* ts: on AS/400, SAP_BOOL must be char, only char is guaranteed to be the
 * same size on ILE and PASE */
#if ( defined(SAPonOS400) || defined(SAPwithPASE400) ) && defined(SAP_BOOL_ENUM)
#error SAP_BOOL must be char on iSeries, do not define SAP_BOOL_ENUM
#endif


/**********************************************************************/
/* SAP_DOUBLE:                                                        */
/**********************************************************************/
#ifndef SAP_DOUBLE_DEFINED
#undef SAP_DOUBLE_DEFINED
  typedef double SAP_DOUBLE;          /* Value range is at least        */
                                      /* +- 10**37, 15 digits precision */
  #define SAP_DOUBLE_MIN  DBL_MIN
  #define SAP_DOUBLE_MAX  DBL_MAX
#endif


/**********************************************************************/
/* SAP_VARSTR:                                                        */
/**********************************************************************/
typedef unsigned char  SAP_VARSTR;  /* Variable string.               */
                                    /* This declares only the first   */
                                    /* byte of the string.            */
                                    /* The following bytes will be    */
                                    /* used for the rest of the       */
                                    /* string. In structs, this must  */
                                    /* be the last field (as long as  */
                                    /* the compiler allocates fields  */
                                    /* in that order).                */


/**********************************************************************/
/* SAP_INTi (i = 1, 2, 4):                                            */
/* machine independent data types to access 1, 2 or 4 byte within a   */
/* structure containing a mainframe image                             */
/**********************************************************************/
/*      current        Name            Explanation                    */
/*      implementation                 what to use for                */
/*--------------------------------------------------------------------*/
typedef SAP_RAW        SAP_INT1;    /* 1 byte binary data             */

typedef SAP_RAW        SAP_INT2[2];
                                    /* 2 bytes binary data            */
                                    /* length, offsets                */
                                    /* ASM: AL2, XL2, Y, H            */
                                    /* For access                     */
                                    /* use ICM03 and STCM03 only !    */

typedef SAP_RAW        SAP_INT4[4];
                                    /* 4 bytes binary data            */
                                    /* length, offsets                */
                                    /* ASM: AL4, XL4, A, F            */
                                    /* For access                     */
                                    /* use ICM15 and STCM15 only !    */



/**********************************************************************/
/* FLAGBYTE:                                                          */
/* machine independent data types to access flag bits within a byte   */
/* within a structure containing a mainframe image                    */
/**********************************************************************/
typedef SAP_RAW        FLAGBYTE;    /* bit structure (8 bits)         */

#define HEX80  0x80                 /* This is the name of the bit,   */
                                    /* which was addressed with       */
                                    /* X'80' on the IBM mainframe.    */
#define HEX40  0x40                 /* The implementation may vary    */
#define HEX20  0x20                 /* on different machines.         */
#define HEX10  0x10                 /* This type is used to access    */
#define HEX08  0x08                 /* data which is moved between    */
#define HEX04  0x04                 /* platforms.                     */
#define HEX02  0x02                 /* For read access use            */
#define HEX01  0x01                 /*    ( aByte & HEX80 )           */
                                    /* For write access use           */
                                    /*    aByte |= HEX80         or   */
                                    /*    aByte &= ~HEX80             */


/**********************************************************************/
/* SAP_TOTAL_SIZE_T:  This type has n bits on an n-bit platform, and  */
/*              should be used when sizes may become larger than 4 GB.*/
/*              size_t may have a smaller range because it is         */
/*              required to hold the size of a single chunk of memory */
/*              only.                                                 */
/*   SAP_TOTAL_SIZE_CONST: Suffix for constants. Needed for constants */
/*              > SAP_INT_MAX. (Necessary because C99 6.4.4.1 has not */
/*              yet been implemented everywhere.)                     */
/**********************************************************************/
#ifdef SAPonOS400

  typedef NS_STD_C_HEADER unsigned long long  SAP_TOTAL_SIZE_T;
  #define SAP_TOTAL_SIZE_T_MAX  ((SAP_TOTAL_SIZE_T) -1)

  /* Suffix for constants. */
  #define SAP_TOTAL_SIZE_CONST( x ) ( x##ULL )

  /* Format identifiers for printfU etc. */
  #define SAP_Ftsu CCQ_FMT("Ftsu") cU("llu")
  #define SAP_Ftsx CCQ_FMT("Ftsx") cU("llx")

  /* Convert string representation (decimal) to SAP_TOTAL_SIZE_T. */
  #define SAP_STR2TOTAL_SIZE_T( value, str )  (value = strtoullU((str), (SAP_UC **)0, 10))

#else

  typedef NS_STD_C_HEADER size_t  SAP_TOTAL_SIZE_T;
  #define SAP_TOTAL_SIZE_T_MAX  ((SAP_TOTAL_SIZE_T) -1)

  /* Suffix for constants. */
  #define SAP_TOTAL_SIZE_CONST( x )   SAP_SIZE_CONST( x )

  /* Format identifier for printfU etc. */
  #define SAP_Ftsu  CCQ_FMT("Ftsu") SAP_Fsu
  #define SAP_Ftsx  CCQ_FMT("Ftsx") SAP_Fsx

  /* Convert string representation (decimal) to SAP_TOTAL_SIZE_T. */
  #define SAP_STR2TOTAL_SIZE_T( value, str )    SAP_STR2size_t( value, str )

#endif


/**********************************************************************/
/* SAP_CHUNK_SIZE_T:                                                  */
/* This type is guaranteed to be a synonym (typedef) for size_t. Thus */
/* it can be used when calling system functions which use size_t. The */
/* difference between size_t and SAP_CHUNK_SIZE_T is that there are   */
/* ccQ warnings if SAP_CHUNK_SIZE_T is assigned to or compared with a */
/* 32-bit type such as int, unsigned, SAP_UINT etc.                   */
/*                                                                    */
/* SAP_SIZE_CONST:   Append the suffix for constants.                 */
/* SAP_Fsu, SAP_Fsx: Formatted output.                                */
/* SAP_STR2size_t:   Convert string representation (decimal) to       */
/*                   SAP_CHUNK_SIZE_T                                 */
/**********************************************************************/
typedef NS_STD_C_HEADER size_t  SAP_CHUNK_SIZE_T;
#define SAP_CHUNK_SIZE_T_MAX  ((SAP_CHUNK_SIZE_T) -1)


/**********************************************************************
 * Obsolete: SAP_SIZE_T
 **********************************************************************/
typedef NS_STD_C_HEADER size_t  SAP_SIZE_T;
#if defined (SAPonNT) && defined (WIN64)
#define SAP_SIZE_T_MAX  ((size_t) 0xffffffffffffffff)
#else
#define SAP_SIZE_T_MAX  ((size_t) ULONG_MAX)
#endif


/**********************************************************************/
/* 'conversion' macros SAP_SIZE_T -> SAP_INT with clipping	      */
/*  needed for 64-bit migration					      */
/**********************************************************************/
#define SAP_SIZE2INT( size ) \
	((size) < SAP_INT_MAX ? (SAP_INT)size : SAP_INT_MAX)
#define SAP_SIZE2UINT( size ) \
	((size) < SAP_UINT_MAX ? (SAP_INT)size : SAP_UINT_MAX)

/**********************************************************************/
/* __w64 is a new Microsoft compiler (>= 13.00) specific keyword. It  */
/* is necessary to detect 64 Bit porting issues using the /wp64       */
/* compiler switch correctly during 32 Bit compilation.	              */
/**********************************************************************/
#if defined(SAPonNT) && (_MSC_VER < 1300)
#define __SAPW64
#else
#define __SAPW64 __w64
#endif

/**********************************************************************/
/* SAP_OFFSET_T:  unsigned integer type for pointer arithmetic        */
/**********************************************************************/
#if defined SAPonOS400
typedef unsigned long long SAP_OFFSET_T;
#else
typedef size_t SAP_OFFSET_T;
#endif


/**********************************************************************/
/* SAP_SOFFSET_T:  signed integer type for pointer arithmetic         */
/**********************************************************************/
#if defined SAPonOS400
typedef long long SAP_SOFFSET_T;
#else
typedef ptrdiff_t SAP_SOFFSET_T;
#endif


/**********************************************************************/
/* SAP_INT_PTR:  signed integer type with pointer precision           */
/**********************************************************************/
#if defined (SAPonNT) && defined (WIN64)
typedef __int64 SAP_INT_PTR;
#elif defined SAPonOS400
typedef long long SAP_INT_PTR;
#elif defined SAPonNT
typedef long __SAPW64 SAP_INT_PTR;
#else
typedef long SAP_INT_PTR;
#endif


/**********************************************************************/
/* SAP_UINT_PTR:  unsigned integer type with pointer precision        */
/**********************************************************************/
#if defined (SAPonNT) && defined (WIN64)
typedef unsigned __int64 SAP_UINT_PTR;
#elif defined SAPonOS400
typedef unsigned long long SAP_UINT_PTR;
#elif defined SAPonNT
typedef unsigned long __SAPW64 SAP_UINT_PTR;
#else
typedef unsigned long SAP_UINT_PTR;
#endif


/**********************************************************************/
/* SAP_LONG_PTR:  signed integer type with pointer precision          */
/**********************************************************************/
#if defined (SAPonNT) && defined (WIN64)
typedef __int64 SAP_LONG_PTR;
#elif defined SAPonOS400
typedef long long SAP_LONG_PTR;
#elif defined SAPonNT
typedef long __SAPW64 SAP_LONG_PTR;
#else
typedef long SAP_LONG_PTR;
#endif


/**********************************************************************/
/* SAP_ULONG_PTR:  unsigned integer type with pointer precision       */
/**********************************************************************/
#if defined (SAPonNT) && defined (WIN64)
typedef unsigned __int64 SAP_ULONG_PTR;
#elif defined SAPonOS400
typedef unsigned long long SAP_ULONG_PTR;
#elif defined SAPonNT
typedef unsigned long __SAPW64 SAP_ULONG_PTR;
#else
typedef unsigned long SAP_ULONG_PTR;
#endif


/**********************************************************************/
/* Format identifiers for special SAP data types  (WIN64)             */
/**********************************************************************/
#if defined (SAPonNT) && defined (WIN64)
/* SAP_ULONG_PTR, SAP_LONG_PTR (WIN64) */
#define SAP_Flpu  cU("I64u")
#define SAP_Flpd  cU("I64d")
#define SAP_Flpx  cU("I64x")

/* size_t (WIN64) */
#define SAP_Fsu  CCQ_FMT("Fsu") cU("I64u")
#define SAP_Fsd  CCQ_FMT("Fsd") cU("I64u")
#define SAP_Fsx  CCQ_FMT("Fsx") cU("I64x")
/* Suffix for constants. */
#define SAP_SIZE_CONST( x ) ( x##ui64 )
/* Convert string representation (decimal) to size_t. */
#define SAP_STR2size_t( value, str )  (value = strtoullU( str, (SAP_UC **)0, 10))

/* SAP_OFFSET_T, offset_t (WIN64) */
#define SAP_Fod  cU("I64u")
#define SAP_Fox  cU("I64x")

/* SAP_SOFFSET_T, soffset_t (WIN64) */
#define SAP_Fsod  cU("I64d")
#define SAP_Fsox  cU("I64x")

/* Structure cache line alignment (WIN64) */
#ifdef _M_IA64
#define SAP_CACHE_ALIGN __declspec(align(128))
#else
#define SAP_CACHE_ALIGN __declspec(align(64))
#endif

/**********************************************************************/
/* Format identifiers for special SAP data types  (WIN32)             */
/**********************************************************************/
#elif defined (SAPonNT) && !defined (WIN64)
/* SAP_ULONG_PTR, SAP_LONG_PTR (WIN32) */
#define SAP_Flpu  cU("lu")
#define SAP_Flpd  cU("ld")
#define SAP_Flpx  cU("lx")

/* size_t (WIN32)  */
#define SAP_Fsu  CCQ_FMT("Fsu") cU("lu")
#define SAP_Fsd  CCQ_FMT("Fsd") cU("lu")
#define SAP_Fsx  CCQ_FMT("Fsx") cU("lx")
/* Suffix for constants. */
#define SAP_SIZE_CONST( x ) ( x##ul )
/* Convert string representation (decimal) to size_t. */
#define SAP_STR2size_t( value, str )  (value = strtoulU( str, (SAP_UC **)0, 10))

/* SAP_OFFSET_T (WIN32) */
#define SAP_Fod  cU("lu")
#define SAP_Fox  cU("lx")

/* SAP_SOFFSET_T (WIN32) */
#define SAP_Fsod  cU("ld")
#define SAP_Fsox  cU("lx")

/* Structure cache line alignment (WIN32) */
#define SAP_CACHE_ALIGN

/**********************************************************************/
/* Format identifiers for special SAP data types  (OS400)             */
/**********************************************************************/
#elif defined (SAPonOS400)
/* SAP_ULONG_PTR, SAP_LONG_PTR (OS400) */
#define SAP_Flpu  cU("llu")
#define SAP_Flpd  cU("lld")
#define SAP_Flpx  cU("llx")

/* size_t (OS400)  */
#define SAP_Fsu  CCQ_FMT("Fsu") cU("lu")
#define SAP_Fsd  CCQ_FMT("Fsd") cU("lu")
#define SAP_Fsx  CCQ_FMT("Fsx") cU("lx")
/* Suffix for constants. */
#define SAP_SIZE_CONST( x ) ( x##UL )
/* Convert string representation (decimal) to size_t. */
#define SAP_STR2size_t( value, str )  (value = strtoulU( str, (SAP_UC **)0, 10))

/* SAP_OFFSET_T (OS400) */
#define SAP_Fod  cU("llu")
#define SAP_Fox  cU("llx")

/* SAP_SOFFSET_T (OS400) */
#define SAP_Fsod  cU("lld")
#define SAP_Fsox  cU("llx")

/* Structure cache line alignment (OS400) */
#define SAP_CACHE_ALIGN

/**********************************************************************/
/* Format identifiers for special SAP data types  (UNIX)              */
/**********************************************************************/
#else
/* SAP_ULONG_PTR, SAP_LONG_PTR (UNIX 32-/64-Bit) */
#define SAP_Flpu  cU("lu")
#define SAP_Flpd  cU("ld")
#define SAP_Flpx  cU("lx")

/* size_t (UNIX 32-/64-Bit)  */
#define SAP_Fsu  CCQ_FMT("Fsu") cU("lu")
#define SAP_Fsd  CCQ_FMT("Fsd") cU("lu")
#define SAP_Fsx  CCQ_FMT("Fsx") cU("lx")
/* Suffix for constants. */
#define SAP_SIZE_CONST( x ) ( x##UL )
/* Convert string representation (decimal) to size_t. */
#define SAP_STR2size_t( value, str )  (value = strtoulU( str, (SAP_UC **)0, 10))

/* SAP_OFFSET_T (UNIX 32-/64-Bit) */
#define SAP_Fod  cU("lu")
#define SAP_Fox  cU("lx")

/* SAP_SOFFSET_T (UNIX 32/64-Bit) */
#define SAP_Fsod  cU("ld")
#define SAP_Fsox  cU("lx")

/* Structure cache line alignment (UNIX 32/64-Bit) */
#define SAP_CACHE_ALIGN

#endif


/**********************************************************************/
/* C standard types                                                   */
/* classified if they count character length or byte length           */
/**********************************************************************/
/*SAPUNICODEOK_CHARLEN*/ typedef int                     intU;
/*SAPUNICODEOK_RAWLEN*/  typedef int                     intR;
/*SAPUNICODEOK_CHARLEN*/ typedef unsigned int            unsigned_intU;
/*SAPUNICODEOK_RAWLEN*/  typedef unsigned int            unsigned_intR;
/*SAPUNICODEOK_CHARLEN*/ typedef short                   shortU;
/*SAPUNICODEOK_RAWLEN*/  typedef short                   shortR;
/*SAPUNICODEOK_CHARLEN*/ typedef unsigned short          unsigned_shortU;
/*SAPUNICODEOK_RAWLEN*/  typedef unsigned short          unsigned_shortR;
/*SAPUNICODEOK_CHARLEN*/ typedef long                    longU;
/*SAPUNICODEOK_RAWLEN*/  typedef long                    longR;
/*SAPUNICODEOK_CHARLEN*/ typedef unsigned long           unsigned_longU;
/*SAPUNICODEOK_RAWLEN*/  typedef unsigned long           unsigned_longR;
/*SAPUNICODEOK_CHARLEN*/ typedef NS_STD_C_HEADER size_t  size_tU;
/*SAPUNICODEOK_RAWLEN*/  typedef NS_STD_C_HEADER size_t  size_tR;

/**********************************************************************/
/* C standard types that convert between character length and byte    */
/* length when used as a factor                                       */
/**********************************************************************/
/* SAPUNICODEOK_BYTEPERCHAR */ typedef int               intUtoR;
/* SAPUNICODEOK_CHARPERBYTE */ typedef int               intRtoU;


/**********************************************************************/
/* macro CCQ_CAST(type)                                               */
/* to classify length information of constants without getting        */
/* problems when the constants are used in #if directives             */
/* e.g.: do classify length information of                            */
/* #define FIELD_LEN 10                                               */
/* as character length, you can use                                   */
/* #define FIELD_LEN (CCQ_CAST(intU)10)                               */
/* this expands to '(10)' for the native compiler                     */
/* and to ((intU)10) for ccQ (outside #if directives)                 */
/**********************************************************************/
#ifndef CCQ_CAST
#define CCQ_CAST(type)
#endif

/**********************************************************************/
/* macro CCQ_FMT(sap_fmt)                                             */
/* make SAP format identifier visible to ccQ to check its proper usage*/
/* e.g.:                                                              */
/* #define SAP_Fsu  CCQ_FMT("Fsu") cU("lu")                           */
/* this expands to 'cU("lu")' for the native compiler                 */
/* and to 'cU("@") cU("Fsu") cU("@") cU("lu")' for ccQ                */
/**********************************************************************/
#ifndef CCQ_FMT
#define CCQ_FMT(sap_fmt)
#endif

/**********************************************************************/
/* C standard type time_t compatible type                             */
/*   for representing time differences.                               */
/*                                                                    */
/* Use this type to distinguish time_t values containing real         */
/* calendar time values and values containing time differences.       */
/*                                                                    */
/* Background: Performing arithmetic operations to calendar time      */
/* values, e.g. the addition of two values cause an overflow from a   */
/* certain date on. ccQ warns if dangerous arithmetic operations are  */
/* performed on time_t calendar time values. Arithmetic operations on */
/* calendar time differences such as processing times are noncritical.*/
/**********************************************************************/
#ifdef SAPccQ
  typedef long timediff_t;
#else
  #define timediff_t time_t
#endif

/**********************************************************************/
/* timeval.tv_sec differs vom time_t on WIN64. Use this macro for     */
/* getting a time_t pointer from a tv_sec:                            */
/*                                                                    */
/*          gettimeofday(&t,NULL);                                    */
/* WRONG:   ctime(&(t.tv_sec));                                       */
/* CORRECT: ctime(TIMEVALSEC_TO_TIME_T_PTR(t.tv_sec));                */
/**********************************************************************/
#if defined(SAPonNT) && defined(WIN64)
__forceinline __int64 *TIMEVALSEC_TO_TIME_T_PTR(long sec)
{
    __declspec(thread) static __int64 t;
    t = sec;
    return &t;
}
#else
    #define TIMEVALSEC_TO_TIME_T_PTR(sec)	(&(sec))
#endif


/**********************************************************************/
/* common definition of global SAP data types                         */
/**********************************************************************/
/* date: YYYYMMDD */
#define SAP_DATE_LN 8
typedef SAP_CHAR  SAP_DATE [SAP_DATE_LN];

/* time: HHMMSS */
#define SAP_TIME_LN 6
typedef SAP_CHAR  SAP_TIME [SAP_TIME_LN];

/* BCD numbers */
typedef SAP_RAW   SAP_BCD;


/**********************************************************************/
/* mode strings for fopen                                             */
/**********************************************************************/
#define FOPEN_R_BIN      cU("rb")
#define FOPEN_W_BIN      cU("wb")
#define FOPEN_A_BIN      cU("ab")
#define FOPEN_RPLUS_BIN  cU("r+b")
#define FOPEN_WPLUS_BIN  cU("w+b")
#define FOPEN_APLUS_BIN  cU("a+b")

#define FOPEN_R_TEXT     cU("r")
#define FOPEN_W_TEXT     cU("w")
#define FOPEN_A_TEXT     cU("a")
#define FOPEN_A_TEXT_AR  iU("a")
#define FOPEN_RPLUS_TEXT cU("r+")
#define FOPEN_WPLUS_TEXT cU("w+")
#define FOPEN_APLUS_TEXT cU("a+")

#ifdef SAPonOS400
#define FOPEN_W_TEXT_UTF8     cU("w,o_ccsid=1208")
#define FOPEN_A_TEXT_UTF8     cU("a,o_ccsid=1208")
#define FOPEN_A_TEXT_AR_UTF8  iU("a")
#define FOPEN_WPLUS_TEXT_UTF8 cU("w+,o_ccsid=1208")
#define FOPEN_APLUS_TEXT_UTF8 cU("a+,o_ccsid=1208")
#else
#define FOPEN_W_TEXT_UTF8     cU("w")
#define FOPEN_A_TEXT_UTF8     cU("a")
#define FOPEN_A_TEXT_AR       iU("a")
#define FOPEN_WPLUS_TEXT_UTF8 cU("w+")
#define FOPEN_APLUS_TEXT_UTF8 cU("a+")
#endif
/**********************************************************************/
/* THREAD_VAR: global variable declaration for thread context only    */
/**********************************************************************/
#if defined(__WIN32) || defined(WIN32)
/*  #define THREAD_VAR	__declspec(thread) disabled 16.9.96 D. Rachal */
  #define THREAD_VAR
#else
  #define THREAD_VAR
#endif

/**********************************************************************/
/* Min/max macros                                                     */
/* To prevent compiler warnings the macros MAX and MIN now have the   */
/* same wording as those in /usr/include/sys/param.h.                 */
/**********************************************************************/
#ifndef MIN
  #define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#ifndef MAX
  #define MAX(a,b) (((a)>(b))?(a):(b))
#endif

/* Allow suppression of definition of min and max
   to avoid conflicts with C++ headers */
#ifndef CPP_NO_LOWERCASE_MIN_MAX

#ifndef min
  #define min(a,b)       (((a) <= (b)) ? (a) : (b))
#endif

#ifndef max
  #define max(a,b)       (((a) <= (b)) ? (b) : (a))
#endif

#endif

/**********************************************************************/
/* USE and UNREF_PARM (two names for the same macro)                  */
/*   avoid warnings about unused formal parameters                    */
/*                                                                    */
/* Caution: USE and UNREF_PARM only work for non-const parameters     */
/*   In the case of const parameters a syntax error will occur,       */
/*   for example: "expression must be a modifiable lvalue"            */
/**********************************************************************/
#define USE(param)           ((param)=(param))
#define UNREF_PARM(param)    ((param)=(param))


/**********************************************************************/
/* STMT macro                                                         */
/*   for the warningless construction of statement-like macros        */
/*                                                                    */
/* Example:                                                           */
/*   #define RSPO_ASSERT()    \                                       */
/*        STMT( if (!(expr))  \                                       */
/*              { ...         \                                       */
/*              } )                                                   */
/**********************************************************************/
#if (defined(SAPonRM600) || defined(lint))
  static int ZERO_VAL = 0;
#else
  #define ZERO_VAL 0
#endif

#define STMT(statement)       \
            do { statement }  \
            while (ZERO_VAL)

/**********************************************************************/
/* SAP_GET_VOLATILE                                                   */
/*                                                                    */
/* Used to read variables defined with type qualifier 'volatile'      */
/* and with immediate cast.                                           */
/*                                                                    */
/* The macro avoids that the volatile behaviour is lost for the read  */
/* access, which would result in an unsave access and a compiler      */
/* warning 'casting from volatile type to non-volatile type'.         */
/* No extra code is generated.                                        */
/*                                                                    */
/* Example:                                                           */
/*                                                                    */
/* <type_1>             x;                                            */
/* volatile <type_2> *  y = <some shm_address>;                       */
/* ...                                                                */
/* x = (<type_1>) *y;                      warning, unsafe access     */
/* x = (<type_1>) SAP_GET_VOLATILE( *y );  no warning, correct access */
/*                                                                    */
/*--------------------------------------------------------------------*/
#define SAP_GET_VOLATILE(x) (1?(x):(x))


/**********************************************************************/
/* ARRAY_LENGTH                                                       */
/**********************************************************************/
#define ARRAY_LENGTH(Array) (sizeofR (Array) / sizeofR (Array[0]))


/**********************************************************************/
/* access macros for SAP_INTi                                         */
/**********************************************************************/
#define ICM03U(x) ((unsigned short) \
                (( ((unsigned short)((x)[0]))           <<  8) | \
                 ( ((unsigned short)((x)[1])) & 0xff)         ))
                                    /* Read access to variables of    */
                                    /* fields of type SAP_INT2        */

#define ICM03(x) ((short) \
                 (( ((short)((x)[0]))           <<  8) | \
                  ( ((short)((x)[1])) & 0xff)         ))
                                    /* Read access to variables of    */
                                    /* fields of type SAP_INT2        */

#define ICM15(x) ((SAP_INT) \
	   ( ( ((SAP_INT )((SAP_SRAW)((x)[0]))        ) << 24) | \
	     ((((SAP_INT )(           (x)[1])) & 0xff ) << 16) | \
	     ((((SAP_INT )(           (x)[2])) & 0xff ) <<  8) | \
	      (((SAP_INT )(           (x)[3])) & 0xff )        ))
                                    /* Read access to variables of    */
                                    /* fields of type SAP_INT4        */

#define STCM03(x, val) \
  ( (x)[0] = (SAP_RAW)(((val) >> 8) & 0xff),\
    (x)[1] = (SAP_RAW)( (val)       & 0xff) )
                                    /* Write access for variables of  */
                                    /* fields of type SAP_INT2        */

#if defined (SAPonNT) && !defined(_OS2DAPIE_)
__inline void STCM15(SAP_RAW *x, unsigned int val)
    {
    (x)[0] = (SAP_RAW)(((val) >>24) & 0xff);
    (x)[1] = (SAP_RAW)(((val) >>16) & 0xff) ;
    (x)[2] = (SAP_RAW)(((val) >>8)  & 0xff);
    (x)[3] = (SAP_RAW)( (val)       & 0xff);
    }
    /* Workaround fuer Compiler Fehler auf NT. Um inlining auf NT zu
       aktivieren sollte mit /Ob1 compiliert werden. Damit wird es fast
       doppelt so schnell.*/
#else
#define STCM15(x, val) \
  ( (x)[0] = (SAP_RAW)(((val) >>24) & 0xff),\
    (x)[1] = (SAP_RAW)(((val) >>16) & 0xff),\
    (x)[2] = (SAP_RAW)(((val) >> 8) & 0xff),\
    (x)[3] = (SAP_RAW)( (val)       & 0xff) )
                                    /* Write access for variables of  */
                                    /* fields of type SAP_INT4        */
                                    /* byte order 3,2,1,0 assumed     */
#endif

#define ACM03(x, y) \
  ( (x)[0] = (y)[0],\
    (x)[1] = (y)[1] )
                                    /* Assign macro for variables of  */
                                    /* fields of type SAP_INT2        */

#define ACM15(x, y) \
  ( (x)[0] = (y)[0],\
    (x)[1] = (y)[1],\
    (x)[2] = (y)[2],\
    (x)[3] = (y)[3] )
                                    /* Assign macro for variables of  */
                                    /* fields of type SAP_INT4        */

/* in order to allow ccQ to check the type of the parameters
   define these macros as functions */
#ifdef SAPccQ
  #undef ICM03U
  #define ICM03U(x) _ccQICM03U(x)
  extern /*SAPUNICODEOK_MIXEDLEN*/ unsigned short _ccQICM03U( const void* );

  #undef ICM03
  #define ICM03(x) _ccQICM03(x)
  extern /*SAPUNICODEOK_MIXEDLEN*/ short _ccQICM03( const void* );

  #undef ICM15
  #define ICM15(x) _ccQICM15(x)
  extern /*SAPUNICODEOK_MIXEDLEN*/ SAP_INT _ccQICM15( const void* );

  #undef STCM03
  #define STCM03(x,y) _ccQSTCM03(x,y)
  extern /*SAPUNICODEOK_MIXEDLEN*/ SAP_RAW _ccQSTCM03( void*, /*SAPUNICODEOK_MIXEDLEN*/ SAP_USHORT );

  #undef STCM15
  #define STCM15(x,y) _ccQSTCM15(x,y)
  extern /*SAPUNICODEOK_MIXEDLEN*/ SAP_RAW _ccQSTCM15( void*, /*SAPUNICODEOK_MIXEDLEN*/ SAP_UINT );

  #undef ACM03
  #define ACM03(x,y) _ccQACM03(x,y)
  extern /*SAPUNICODEOK_MIXEDLEN*/ SAP_RAW _ccQACM03( void*, const void* );

  #undef ACM15
  #define ACM15(x,y) _ccQACM15(x,y)
  extern /*SAPUNICODEOK_MIXEDLEN*/ SAP_RAW _ccQACM15( void*, const void* );

  /* for backward compatibility of ccQ self validation */
  #define _ccQSapintMacrosSet
#endif


/**********************************************************************/
/* common definition of return codes for many procedures.             */
/**********************************************************************/

typedef int SAPRETURN;     /* standard return type                    */

#define SAP_O_K        0   /* This routine has done the work it       */
                           /* should do.  This value is compatible    */
                           /* with 0 and FALSE  !                     */
                           /* Reason: This allows for C-codings like: */
                           /*     rc = some_function(...);            */
                           /*     if (rc) { do error handling }       */

/* <other_values>          :  are defined on a project by project     */
                           /* basis.                                  */
                           /* There names are like:                   */
                           /* - <project name> in capital letters.    */
                           /* - "E"                                   */
                           /* - <name of error> in capital letters.   */


/**********************************************************************/
/* common definition of error handling for many procedures.           */
/**********************************************************************/

typedef int SAPERRMASK;    /* error handling mask.                    */

                           /* This is normally the last param of a    */
                           /* procedure.  Such a procedure should     */
                           /* have the return type "SAPRETURN".       */
                           /* The caller ORs all the return values,   */
                           /* which he wants to handle hinself.       */
                           /* n the case of an error condition not    */
                           /* covered by this, the routine will do    */
                           /* some error handling itself and          */
                           /* will NOT return.                        */

                           /* Here two shorthands are defined:        */

#define SAPNOERROR     0   /* value   == 0                            */
                           /* meaning == I dont want to handle any    */
                           /*            error                        */

#define SAPALLERRORS  ~0   /* value   == ~0                           */
                           /* meaning == I want to handle all errors  */
                           /*            myself                       */


/**********************************************************************/
/* LINE_USING_PROC                                                    */
/* The type for generalized dump routines:                            */
/*   a pointer to a procedure, which will be called for (and with)    */
/*   every line, that shall be output. The parameters of each such    */
/*   procedure must be:                                               */
/*   1st: address of text data.                                       */
/*   2nd: length of that data. Counted in number of basic characters. */
/* LINE_USING_PROC_WITH_CONTEXT                                       */
/*   3rd: void pointer to pass arbitrary context information          */
/**********************************************************************/
BEGIN_externC
typedef void (* LINE_USING_PROC) (SAP_CHAR *  buffer,
                                  int         number_of_chars );
typedef void (* LINE_USING_PROC_WITH_CONTEXT) (
                                  SAP_CHAR *  buffer,
                                  int         number_of_chars,
                                  void *      context );
END_externC

/* Definitions for Unicode Implementation */
#include "sapuc.h"

/* Definitions from SAP data dictionary */
#include "saptypec.h"

#endif  /* SAPTYPEB_H */
