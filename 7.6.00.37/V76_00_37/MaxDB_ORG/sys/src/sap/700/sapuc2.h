/* @(#) $Id: //sapdb/V76/c_00/b_37/sys/src/sap/700/sapuc2.h#1 $ SAP*/     
/*--------------------------------------------------------------------*/
/* (c) Copyright SAP AG, Walldorf, 1996, 1997                         */
/*                                                                    */
/* This is an include file with special definitions and should be     */
/* used only for a very limited number of modules !                   */
/*                                                                    */
/*--------------------------------------------------------------------*/

/*-HISTORY-------------------------
 * 12.02.2004 kkr      Add UcnToUtf16nCPGD, UcnToUtf16nCP,
 *                         Utf16nToUcnCPGD, Utf16nToUcnCP
 *
 * 19.11.2003 bs       Stop using types SAP_UCS, SAP_U2S, SAP_U4S, 
 *                     SAP_UTF16S, SAP_UTF8S to simplify the usage of 
 *                     'const' type qualifiers
 * 14.07.2003 mit
 *                     Add range definition for control codes.
 * 11.11.2002 mit
 *                     Define boundaries to split and decode UTF-16.
 *                     (Had been in saputfi.h before.)
 * 10.06.2002 mit
 *                     Add Utf8nToUcnCP_4 and UcnToUtf8nCP_4 for RFC,
 *                     which are more strict than Utf8nToUcnCP and
 *                     Utf8nToUcnCP.
 * 26.11.2001 meb      add macro IS_LOW_SURROGATE and IS_HIGH_SURROGATE
 * 09.08.2001 bs       add more SAPUNICODEOK_MIXEDLEN comments 
 * 29.10.2000 bs       add some SAPUNICODEOK_MIXEDLEN in order to avoid 
 *                     unnecessary ccQ length trace warnings
 *-
 * 04.10.2000 bs       use intU in conversion function declarations
 *-
 * 31.08.2000 meb      Switch off Unicode checks with comment 
 *                     CCQ_CCU_FILE_OFF
 *-
 * 28.08.2000 mit
 *                     Add Utf8nToUcn_2prep, Utf8nToUcn_2,
 *                     UcnToUtf8n_3prep, UcnToUtf8n_3
 *-
 * 23.08.2000 meb      Add SAP_UC <-> SAP_UC_MB
 *-
 * 11.05.2000 bs       Add NeutralUcnToUcn() and UcnTo NeutralUcn()
 *-
 * 09.03.2000 hm       Add casts (char *) to Utf8sCpy
 *-
 * 04.08.1999 mit      Add A7 <-> E8 .
 *-
 * 27.05.1999 mit      Add UcnToA7n,
 *-
 * 18.05.1999 mit      Add A7sCat, A7sPrintF, A7sToLong
 *- 
 * 23.02.1999 bs       enhance parts for ccU 
 * 08.02.1999 mit      Add A7ToE8, E8ToA7, A7nToE8nInPlace and
 *                     E8nToA7nInPlace.
 *-
 * 08.02.1999 mit      'E8ToB8' for Unicode-AS/400
 *-
 * 22.12.1998 mit      'A7ToUc' for AS/400
 *-
 * 19.11.1998 mit      Renaming some more functions.
 *-
 * 16.11.1998 mit      Move alternative character types from sapuc2.h to
 *                     sapuc.h . Now they can be used in other include
 *                     files for prototypes or field definitions.
 *                     For using data of these types, you still have to
 *                     include sapuc2.h .
 *-
 * 16.11.1998 mit      AIX does not like macro redefines.
 *-
 * 13.11.1998 mit      Delete UcsToA7s_SetDefault(),
 *                     UcsToB7s_SetDefault(), UcsToE8s_SetDefault.
 * 12.11.1998 mit      Renaming some types and many functions:
 *                     __old__          __new__
 *                     SAP_MB           SAP_B8
 *                     A7aToUca         A7nToUcnVola
 *                     ...              ... 
 *-
 * 30.10.1998 mit      Corrections for B7 prototypes and macros.
 *-
 * 08.09.1998 mit      Make more parameters  const
 *                     Many comments added.
 *-
 * 03.09.1998 mit      Make more parameters  const
 *-
 * 02.09.1998 mit      Several new ..B7.. functions.
 *-
 * 01.09.1998 mit      Against warnings oc ccU -u
 *-
 * 11.06.1998 mit      SAP_UTF7 added.
 *                     'char' removed whereeven not neccessary.
 *                     EBCDIC: IBM's code page number is 500 (docu only)
 *-
 * 29.05.1998 mit      SAP_A7 and SAP_E8 added.
 *                     Many procedures renamed:
 *                     + Short name is in-line function.
 *                     + Other procedures have suffixes:
 *                         _E  returning error code
 *                         _A  taking address
 *                         _N  taking length information
 *                         _S  taking substitution character
 *                     i.e.: E8nToUcn(...)
 *-
 * 03.06.1997 bs       reorganized preprocessor switches
 * 03.03.1997 bs       definition of SAP_B7 moved 
 *                     from sapuc.h to sapuc2.h
 * 02.10.1996 mit      SAP_B8: unsigned char -> char
 *                     to become compatible with wcstombs in <stdlib.h>
 *-
 * 24.09.1996 mit      Add UCS-4 and UTF-16.
 *-
 * 12.07.1996 mit      First creation.
 *-END-HISTORY--------------------*/

/* CCQ_CCU_FILE_OFF */

#ifndef SAPUC2_H
#define SAPUC2_H "$Id: //sapdb/V76/c_00/b_37/sys/src/sap/700/sapuc2.h#1 $"

#if !defined(SAPwithUNICODE)
#define SAPUC2_ID1 "this SAP system for single byte only"
#else
#define SAPUC2_ID1 "SAP universal character with wide range"
#endif

#if defined(SAP_UC_is_wchar)
#define SAPUC2_ID2 "SAP universal character == wchar_t"
#else
#define SAPUC2_ID2 "SAP universal character differs from wchar_t"
#endif

#if defined(SAPwithUNICODE)
#define SAPUC2_ID3 "SAP universal character == UTF-16"
#else
#define SAPUC2_ID3 "SAP universal character differs from UTF-16"
#endif

#if defined(SAP_UC_is_wchar)
#define SAPUC2_ID4 "wchar_t uses Unicode"
#else
#define SAPUC2_ID4 "wchar_t uses proprietary coding"
#endif

#ifdef WCHAR_is_2B
#define SAPUC2_ID5 "wchar_t has 2 bytes"
#elif defined(WCHAR_is_4B)
#define SAPUC2_ID5 "wchar_t has 4 bytes"
#else
#define SAPUC2_ID5 "wchar_t has other size"
#endif

#ifndef SAPEINTERN
#define SAPEINTERN   1
#endif
#ifndef SAPEINVAL
#define SAPEINVAL   16
#endif
#ifndef SAPETOOLONG
#define SAPETOOLONG 32
#endif
#ifndef RSCPETOOLONG
#define RSCPETOOLONG 32
#endif
#ifndef RSCPENOCONV
#define RSCPENOCONV    2048
#endif
#ifndef SAPENOLANG
#define SAPENOLANG 128
#endif
#ifndef SAPENOCP
#define SAPENOCP   128
#endif
#ifndef SAPEUNIQ
#define SAPEUNIQ  512
#endif
#ifndef RSCPEUNIQ
#define RSCPEUNIQ  512
#endif
#ifndef SAPEODATA
#define SAPEODATA 8192
#endif
#ifndef RSCPEODATA
#define RSCPEODATA 8192
#endif

#if defined(SAP_UC_is_1B)
 #define UC_MASK 0xFF

#elif defined(SAP_UC_is_2B)
 #define UC_MASK 0xFFFF

#elif defined(SAP_UC_is_4B)
 #define UC_MASK 0xFFFFFFFF

#else
 #error strange SAP_UC_LN
#endif


/**********************************************************************/
/*                                                                    */
/* If you need C types to handle single characters or strings with a  */
/* large character set, you have the choice between six of them.      */
/* Please handle them distinctly! Some pairs may be compatible on     */
/* one platform but not on the others.                                */
/*                                                                    */
/* Each type has some advantages and some disadvantages. That is the  */
/* reason, why we could not drop any one.                             */
/*                                                                    */
/*                                                                    */
/*                                                                    */
/*                                                                    */
/*                        .---------.                                 */
/*                        | wchar_t |                                 */
/*                       .'         `----------.                      */
/*                       | XPG4 wide char      |                      */
/*                       | + "all" characters  |                      */
/*                       | + fast              |                      */
/*                       | - unknown coding    |                      */
/*                       | + many functions    |                      */
/*                       | - unknown size      |                      */
/*                       | + but size is fixed |                      */
/*                       `---------------------'                      */
/*                                                                    */
/*                                                                    */
/*                                                                    */
/*   .---------.                              .--------.              */
/*   |  char   |                              |   UC   |              */
/*  .'         `----------.                  .'        `-----------.  */
/*  | Standard C character|                  | universal character |  */
/*  | - 1 byte            |                  | + "all" characters  |  */
/*  | - variing coding    |                  | + fast              |  */
/*  | + many functions    |                  | - platform dependent|  */
/*  | - also used for     |                  | - unknown size      |  */
/*  |   multi-byte strings|                  | + but size is fixed |  */
/*  | - also used for     |                  | . zero-terminated   |  */
/*  |   raw bytes         |                  |   strings           |  */
/*  `---------------------'                  `---------------------'  */
/*                                                                    */
/*                                                                    */
/*                                                                    */
/*                                                                    */
/*   .---------.                              .--------.              */
/*   |   B8    |                              | CHAR   |              */
/*  .'         `----------.                  .'        `-----------.  */
/*  | multibyte           |                  | universal character |  */
/*  | - variable size     |                  | + "all" characters  |  */
/*  | - variing coding    |                  | + fast              |  */
/*  | + many functions    |                  | - platform dependent|  */
/*  | - cannot single char|                  | - unknown size      |  */
/*  | + 'char' compatible |                  | + but size is fixed |  */
/*  `---------------------'                  | . blank-padded      |  */
/*                                           |   strings           |  */
/*                                           `---------------------'  */
/*                                                                    */
/*                                                                    */
/**********************************************************************/


/**********************************************************************/
/*                                                                    */
/*  .----.  .------.  .------.  .---------.  .----.                   */
/*  | UC |  | CHAR |  | char |  | wchar_t |  | B8 |                   */
/*  `----'  `------'  `------'  `---------'  `----'                   */
/*                                                                    */
/* Beside of these three types there are some more character types:   */
/*                                                                    */
/*                                                                    */
/*   .--------.                                                       */
/*   |   U2   |                                                       */
/*  .'        `----------------.                                      */
/*  | ISO Unicode UCS-2        |            The real UCS-2            */
/*  | + 60000 characters       |                                      */
/*  | + coding = UCS-2         |                                      */
/*  | + size   = 2 B           |                                      */
/*  | - unknown byte order     |                                      */
/*  `--------------------------'                                      */
/*                                                                    */
/*                                                                    */
/*   .--------.                                                       */
/*   |  UTF8  |                                                       */
/*  .'        `----------------.                                      */
/*  | Unicode Transfer Format  |       Used within some database.     */
/*  | - variable size          |       Used in some files.            */
/*  | - cannot single char     |       Can be used for filenames.     */
/*  | + 2000000000 characters  |                                      */
/*  | + 'char' compatible      |                                      */
/*  | + "file system safe"     |                                      */
/*  | + easy UCS-2 convertable |                                      */
/*  `--------------------------'                                      */
/*                                                                    */
/*                                                                    */
/*   .--------.                                                       */
/*   |  UTF7  |                                                       */
/*  .'        `----------------.                                      */
/*  | Unicode Transfer Format  |       Used on some data stream.      */
/*  | - variable size          |       i.e. e-mail                    */
/*  | - cannot single char     |                                      */
/*  | + 2000000000 characters  |                                      */
/*  | + 'char' compatible      |                                      */
/*  | + 7-Bit ASCII compatible |                                      */
/*  | + "file system safe"     |                                      */
/*  `--------------------------'                                      */
/*                                                                    */
/*                                                                    */
/*   .--------.                                                       */
/*   |  UTF16 |                                                       */
/*  .'        `----------------.                                      */
/*  | Unicode Transfer Format  |       Unicode 2.0 (with surrogates)  */
/*  | - variable (even) size   |       ISO: UTF-16                    */
/*  | - cannot single char     |       Used in JAVA.                  */
/*  | + 1060000 characters     |                                      */
/*  | + UCS-2 compatible       |                                      */
/*  | + easy UCS-4 convertable |                                      */
/*  `--------------------------'                                      */
/*                                                                    */
/*                                                                    */
/*   .--------.                                                       */
/*   |   U4   |                                                       */
/*  .'        `----------------.                                      */
/*  | Unicode 4 byte Format    |       Unicode 2.0  &  ISO            */
/*  | + 2000000000 characters  |                                      */
/*  | + coding = UCS-4         |                                      */
/*  | + size   = 4 B           |                                      */
/*  `--------------------------'                                      */
/*                                                                    */
/*                                                                    */
/*   .------.                                                         */
/*   |  B7  |                                                         */
/*  .'      `------------------.                                      */
/*  | Character from 7-Bit set |       "Old C", US-ASCII              */
/*  | - single byte chars      |                                      */
/*  | . zero terminated strings|       Can be 8 bit if used on        */
/*  | + 'char' compatible      |       EBCDIC machine !               */
/*  `--------------------------'                                      */
/*                                                                    */
/*                                                                    */
/*   .------.                                                         */
/*   |  A7  |                                                         */
/*  .'      `------------------.                                      */
/*  | 7-bit-US-ASCII           |       US-ASCII                       */
/*  | - single byte chars      |                                      */
/*  | . zero terminated strings|       Is always ASCII. Even on an    */
/*  | + 'char' compatible      |       EBCDIC machine !               */
/*  `--------------------------'                                      */
/*                                                                    */
/*                                                                    */
/*   .------.                                                         */
/*   |  E8  |                                                         */
/*  .'      `------------------.                                      */
/*  | 8-bit-IBM-500-EBCDIC     |       "international" EBCDIC         */
/*  | - single byte chars      |                                      */
/*  | . IBM-500 coding always  |       even in other counties         */
/*  | + 'char' compatible      |                                      */
/*  `--------------------------'                                      */
/*                                                                    */
/*                                                                    */
/*   .------.                                                         */
/*   |  UM  |                                                         */
/*  .'      `------------------.                                      */
/*  | Universal multibyte      |       multibyte representation       */
/*  | - variable size          |       of SAP_UC                      */
/*  | - cannot single char     |       Utf8 in Unicode mode           */
/*  | + 'char' compatible      |       active codepage in             */
/*  | + datatype of textfiles  |       non-Unicode mode               */
/*  `--------------------------'                                      */
/*                                                                    */
/*                                                                    */
/**********************************************************************/


/**********************************************************************/
/*      Some general information about the conversion routines        */
/*--------------------------------------------------------------------*/
/* There are functions which convert strings from one buffer into a   */
/* different buffer.                                                  */
/* These functions have .._dst parameter which point to a destination */
/* area. The caller has to make sure, that this is long enough.       */
/*                                                                    */
/* "...Check..." functions can fail with an error return code, if the */
/* source contains characters, that are illegal either for the source */
/* or for the destination.                                            */
/*                                                                    */
/* "...Subst..." functions never fail, because they use a             */
/* substitution character to replace any bad data.                    */
/*                                                                    */
/* "....Vola" functions convert strings on the fly. They return       */
/* pointers to some static internal memory.                           */
/* So please don't wait too long before using the values. But there   */
/* is enough memory to handle at least fife strings at the same time. */
/*                                                                    */
/* These procedures can be used to build complicated expressions:     */
/*                                                                    */
/* if ( 0==memcmpR( ascii_command,                                    */
/*         ( netProt == USE_IP_NET ? UcsToA7sVola( command1 )         */
/*                                 : UcsToA7sVola( long_command ) )   */
/* {                                                                  */
/*   handle_cmd( A7sToUcsVola( ascii_command ) );                     */
/* }                                                                  */
/*                                                                    */
/**********************************************************************/


/**********************************************************************/
/* wchar_t :   wide character for fast processing                     */
/*             with platform dependent coding.                        */
/*--------------------------------------------------------------------*/
/* This is the C data type as defined by XPG4.                        */
/* A variable of this type can store a wide character.                */
/* The size of this type is platform dependent: often 2 or 4 bytes.   */
/* The coding used is platform dependent and OS release specific.     */
/*--------------------------------------------------------------------*/
/* This type gives direct access to many functions, as has been       */
/* defined in XPG4 and should be implemented everywhere.              */
/*    Classification of a single character:                           */
/*       iswalnum                                                     */
/*       iswalpha                                                     */
/*       iswcntrl                                                     */
/*       iswupper      (Note: most scripts do not have cases.)        */
/*       iswlower                                                     */
/*       iswdigit                                                     */
/*       iswprint                                                     */
/*       wcwidth                                                      */
/*    Simple handling of w-zero terminated strings:                   */
/*       wcscat / wcsncat                                             */
/*       wcscmp / wcsncmp                                             */
/*       wcscpy / wcsncpy                                             */
/*       wcslen                                                       */
/*       wcschr / wcsrchr                                             */
/*       wcspbrk                                                      */
/*       wcsspn / wcscspn                                             */
/*       wcswcs                                                       */
/*       wcstok / wcstok_r                                            */
/*       wcswidth                                                     */
/*    Locale dependent handling of w-zero terminated strings:         */
/*       wcscoll                                                      */
/*       wcsxfrm                                                      */
/*    Conversion functions:                                           */
/*       wcstod / wcstol / wcstoul                                    */
/*       mbtowc / mbstowcs / wctomb / wcstombs                        */
/* Note:                                                              */
/*       wcsftime cannot be used!                                     */
/*--------------------------------------------------------------------*/

#if defined (SAPonDARWIN)
	/*
	 * #warning "wchar.h is missing for SAPonDARWIN !!!"
	 */
#else
#include  <wchar.h>
#endif

#define wcharNull ((wchar_t)0)


/**********************************************************************/
/* SAP_UC:   universal character                                      */
/*--------------------------------------------------------------------*/
/* The size of an universal character is constant and platform        */
/* dependent.                                                         */
/* The coding is platfrom dependent also.                             */
/* It is implementations dependent, wether the coding can vary during */
/* runtime ! ( see setlocale() ).                                     */
/* It is implementations dependent, how many distinct characters can  */
/* be stored. But often all important characters of the world are     */
/* possible.                                                          */
/* To give an idea, why there is this distinction between declared    */
/* features and those which are left 'implementations dependent':     */
/* A specific implementation may map 'SAP_UC' onto                    */
/*  + UCS-2  (16-Bit Unicode, most likely)                            */
/*  + UTF-16 in 32-Bit                                                */
/*  + wchar_t with some propietary coding                             */
/*  + Latin-1 or some other ISO-8859-1 single byte code               */
/*                                                                    */
/* This type gives access to many SAP specific functions.             */
/*--------------------------------------------------------------------*/
/* Declaration and implementation in 'sapuc.h'.                       */
/*--------------------------------------------------------------------*/
#ifndef SAPUC_H
#include "sapuc.h"
#endif

BEGIN_externC

/**********************************************************************/
/* SAP_U2:   character for Unicode UCS-2                              */
/*--------------------------------------------------------------------*/
/* A variable of this type can store a Unicode UCS-2 character.       */
/* The size of this type is 16 bit.                                   */
/* The coding used is UCS-2 (ISO 10646). The release of that          */
/* ISO norm is not specified here. It is the most recent release that */
/* is supported by SAP.                                               */
/* If not explicitelly labeled UTF-16 or SAP_UTF16, surrogates are    */
/* neither expected nor handled.                                      */
/* Comparison to SAP_UTF16: SAP_U2 should be used when surrogate      */
/* support is explicitely exluded. Otherwise, use SAP_UTF16.          */
/*--------------------------------------------------------------------*/
/* This type gives access to some generic functions, which are        */
/* handled differently on the different platforms, but give the same  */
/* result to all SAP applications.                                    */
/*    Classification of a single character:                           */
/*       <none. Processing is done using SAP_UC.>                     */
/*    Simple handling of zero terminated strings:                     */
/*       <none. Processing is done using SAP_UC.>                     */
/*    Conversion functions:                                           */
/*       SAP_U2_...                                                   */
/*--------------------------------------------------------------------*/

/* sapuc.h :
 * 
 * #define U2Null ((SAP_U2)0)
 * 
 * #if defined(WCHAR_is_2B)
 *   typedef wchar_t   SAP_U2;
 * #else 
 *   typedef SAP_USHORT   SAP_U2 ;
 * #endif 
 */

#if defined(SAP_UC_is_UCS2) && defined(SAP_UC_is_2B)

#define U2sLen(aU2s)           strlenU(aU2s)
#define U2sCpy(dest,src)       strcpyU(dest,src)
#define UcToU2(aUc)            (aUc)
#define UcsToU2s(aU2s,aUcs)    strcpyU((aU2s),(aUcs))
#define UcnToU2n(aU2s,aUcs,n) \
        memcpyR((void*)(aU2s),(const void*)(aUcs),(n)*sizeofR(SAP_UC))
#define U2ToUc(aU2)            ((const SAP_UC *)(aU2))
#define U2sToUcs(aUcs,aU2s) strcpyU((aU2s),(aUcs))

#else /* not SAP_UC: UCS2 and 2B */

int            U2sLen(          const SAP_U2 * aU2        );
const SAP_U2 * U2sCpy(                SAP_U2 * dest       ,
				const SAP_U2 * src        );

#ifdef SAPwithCHAR_EBCDIC
SAP_U2         UcToU2(          const SAP_UC   aUc        );

#else /* ASCII or UCS-4 */
#define        UcToU2(aUc)          ((SAP_U2) (aUc))

#endif /* EBCDIC / ASCII */

void           UcsToU2s(              SAP_U2 * dest       , 
                                const SAP_UC * src        );
void           UcnToU2n(              SAP_U2 * dest       , 
                                const SAP_UC * src        , 
                                const intU     noof_chars );
SAP_UC         U2ToUc(          const SAP_U2   aU2        );
void           U2sToUcs(              SAP_UC * dest       , 
                                const SAP_U2 * src        );

#endif /* +/- SAP_UC_is_UCS2 and 2B */


/**********************************************************************/
/* SAP_U4:   character for Unicode UCS-4 (UTF-32)                     */
/*--------------------------------------------------------------------*/
/* A variable of this type can store a Unicode UCS-4 or UTF-32        */
/* character. UTF-32 is a subset of Ucs-4 that contains exacly the    */
/* same characters as UTF-16. We do not explicitely distinguish       */
/* between UTF-32 and UCS-4 in the C code.                            */
/* The size of this type is 32 bit.                                   */
/* The coding used is UCS-4 (ISO 10646). The release of that          */
/* ISO norm is not specified here. It is the most recent release that */
/* is supported by SAP. UTF-32 is the subset of UCS-4 that contains   */
/* exactly the same characters as UTF-16. We do not explicitely       */
/* distinguish between UTF-32 and UCS-4 in the C code.                */
/*--------------------------------------------------------------------*/
/* This type gives access to some generic functions, which are        */
/* handled differently on the different platforms, but give the same  */
/* result to all SAP applications.                                    */
/*    Classification of a single character:                           */
/*       <none. Processing is done using SAP_UC.>                     */
/*    Simple handling of zero terminated strings:                     */
/*       <none. Processing is done using SAP_UC.>                     */
/*    Conversion functions:                                           */
/*       SAP_U4_...                                                   */
/*--------------------------------------------------------------------*/

/* sapuc.h :
 * 
 * #define U4Null ((SAP_U4)0)
 * 
 * #if defined(WCHAR_is_4B)
 *   typedef wchar_t   SAP_U4;
 * #else 
 *   typedef SAP_UINT   SAP_U4 ;
 * #endif 
 */

#if defined(SAP_UC_is_UCS4) && defined(SAP_UC_is_4B)

#define U4sLen(aU4s)           strlenU(aU4s)
#define U4sCpy(dest,src)       strcpyU(dest,src)
#define UcToU4(aUc)            (aUc)
#define UcsToU4s(aU4s,aUcs)    strcpyU((aU4s),(aUcs))
#define U4ToUc(aU4)            (aU4)
#define U4sToUcs(aUcs,aU4s)    strcpyU((aU4s),(aUcs))

#else /* not SAP_UC_is_UCS4 and 4B */

int            U4sLen(          const SAP_U4 * aU4s );
const SAP_U4 * U4sCpy(                SAP_U4 * dest ,
				const SAP_U4 * src  );

#ifdef SAPwithCHAR_EBCDIC
SAP_U4         UcToU4(          const SAP_UC   aUc  );

#else /* ASCII or UCS-4 */
#define        UcToU4(aUc)          ((SAP_U4) (aUc))

#endif /* EBCDIC / ASCII */

void           UcsToU4s(              SAP_U4 * dest , 
                                const SAP_UC * src  );
SAP_UC         U4ToUc(          const SAP_U4   aU4  );
void           U4sToUcs(              SAP_UC * dest , 
                                const SAP_U4 * src  );

#endif /* +/- SAP_UC_is_UCS_4 and 4B */


/**********************************************************************/
/* SAP_UTF16:   character for Unicode UTF16                           */
/*--------------------------------------------------------------------*/
/* This type cannot be used to define a variable for a single         */
/* character !                                                        */
/* The size of this type is 16 bit.                                   */
/* The coding used is UTF16 (ISO 10646). The release of that          */
/* ISO norm is not specified here. It is the most recent release that */
/* is supported by SAP.                                               */
/* Comparison to SAP_U2: SAP_U2 should be used when surrogate support */
/* is explicitely exluded. Otherwise, use SAP_UTF16.                  */
/*--------------------------------------------------------------------*/
/* This type gives access to some generic functions, which are        */
/* handled differently on the different platforms, but give the same  */
/* result to all SAP applications.                                    */
/*    Classification of a single character:                           */
/*       <none. Processing is done using SAP_UC.>                     */
/*    Simple handling of zero terminated strings:                     */
/*       <none. Processing is done using SAP_UC.>                     */
/*    Conversion functions:                                           */
/*       SAP_UTF16_...                                                */
/*--------------------------------------------------------------------*/

/* sapuc.h :
 * 
 * #define UTF16Null ((SAP_UTF16)0)
 * 
 * #if defined(WCHAR_is_2B)
 *   typedef wchar_t   SAP_UTF16;
 * #else 
 *   typedef SAP_USHORT   SAP_UTF16 ;
 * #endif 
 */

/*
 *  Constants for handling of UTF-16.
 *
 *  Please do not use directly. Instead call mblenU() or other 
 *  rscp...-functions.
 */
#define SURROGATE_HALF_SHIFT       (int)           10
#define SURROGATE_HALF_BASE        (SAP_U4) 0x0010000UL
#define SURROGATE_HALF_MASK        (SAP_U4)     0x3FFUL
#define SURROGATE_HIGH_START       (SAP_U4)    0xD800UL
#define SURROGATE_HIGH_START_B     (SAP_RAW)   0xD8
#define SURROGATE_HIGH_END         (SAP_U4)    0xDBFFUL
#define SURROGATE_HIGH_END_B       (SAP_RAW)   0xDB
#define SURROGATE_LOW_START        (SAP_U4)    0xDC00UL
#define SURROGATE_LOW_START_B      (SAP_RAW)   0xDC
#define SURROGATE_LOW_END          (SAP_U4)    0xDFFFUL
#define SURROGATE_LOW_END_B        (SAP_RAW)   0xDF
#define SURROGATE_HIGH_PUA_START   (SAP_U4)    0xDB80UL
#define SURROGATE_HIGH_PUA_START_B (SAP_RAW)   0xDB
#define SURROGATE_HIGH_PUA_END     (SAP_U4)    0xDBFFUL

/* Private Use Area */
#define UTF16_PUA1_HIGH_START      (SAP_U4)    0xE000UL
#define UTF16_PUA1_HIGH_END        (SAP_U4)    0xF8FFUL

/* Control codes (which are subject to SAP's control code filters).
 *
 * For performance reasons U+007F is joint to the C1 area.
 * For performance reasons application my know, that C0 starts at
 * U+0000 and that there is no need for a test like
 *  ch >= CONTROLCODE_C0_START
 */
#define CONTROLCODE_C0_START       (SAP_U4)    0x0000UL
#define CONTROLCODE_C0_END         (SAP_U4)    0x001FUL
#define CONTROLCODE_C1_START       (SAP_U4)    0x007FUL
#define CONTROLCODE_C1_END         (SAP_U4)    0x009FUL


#define Utf16sCpy(dest,src)       U2sCpy(dest,src)
/* Don't mind about surrogates. */

#if defined(SAP_UC_is_UCS2) && defined(SAP_UC_is_2B)

#define Utf16sLen(aUtf16s)           strlenU(aUtf16s)
#define UcToUTF16(aUc)               (aUc)
#define UcsToUTF16sNolim(aUTF16s,aUcs)    strcpyU((aUTF16s),(aUcs))

#else /* not SAP_UC_is_UCS2  and 2B */

int          Utf16sLen(         const SAP_UTF16 * aUtf16s    );
#define      UcToUTF16(aUc)         ((SAP_UTF16)(aUc))
void         UcsToUTF16sNolim(        SAP_UTF16 * dest       , 
                                const SAP_UC    * src        );

#endif /* +/- SAP_UC_is_UCS2 and 2B */

SAP_UC       Utf16ToUc(         const SAP_UTF16   aUTF16     );
void         Utf16sToUcs(             SAP_UC    * dest       , 
                                const SAP_UTF16 * src        );
void         Utf16nToUcn(             SAP_UC    * dest       , 
                                const SAP_UTF16 * src        ,
                                const intU        noof_chars );

#define IS_HIGH_SURROGATE(ch)   ( ((ch) & 0xFC00) == 0xD800)
#define IS_LOW_SURROGATE(ch)    ( ((ch) & 0xFC00) == 0xDC00)


SAPRETURN SwapUnicodeBuffer(    const void      * p_cp       ,
				      void      * p_buffer   ,
			        const intU        noof_chars );
/*----------------------------------------------------------------------
 * SwapUnicodeBuffer  -  Swap the contents of a buffer with Unicode
 *                       data in-place, if neccessary.
 *
 *                       This routine looks at the code page number to
 *                       decide wether swapping is neccessary or not.
 *                       This routine can handle code page numbers even
 *                       when they are swapped themselves.
 * Params:
 *
 *   p_cp        IN    REF  Code page number of the data.
 *
 *   p_buffer    INOUT REF  Address of buffer with the data
 *
 *   noof_chars  IN    VAL  Number of characters in that buffer.
 *                          (Counted in base units of that code pages.
 *                           In case of UTF-16, count number of
 *                           16-bit-units.)
 * Return:
 *   
 *   SAP_O_K     I swapped.
 *   SAP_O_K     I think, that swapping is not neccessary.
 *
 *   SAPEUNIQ    (warn) I was not sure about swapping or not, but I
 *               decided internally.
 *
 *   SAPENOCP    I cannot understand the code page number.
 */


/**********************************************************************
 * Neutral UC format:  
 * Platform independent character format, stored in SAP_RAW arrays. 
 * In the non-Unicode System: contains unchanged characters
 * In Unicode systems: contains Unicode Characters in
 *                     big endian format 
 * The neutral UC format is platform independent like UTF-8, but has a 
 * fixed length per character like SAP_UC. Tables of arrays containing 
 * neutral format characters can be sorted using memcmpR(). 
 *
 * There is no own data type for such data, but SAP_RAW arrays should 
 * be used instead. Use SAP_UC_LN to define the length of the SAP_RAW
 * fields: The SAP_RAW arrays must have SAP_UC_LN times as many units 
 * as the corresponding SAP_UC/SAP_CHAR arrays.
 * 
 * There are two conversion functions to convert between SAP_UC/SAP_CHAR
 * and the neutral UC format: NeutralUcnToUcn() and UcnToNeutralUcn(). 
 * In both conversion functions, the third parameter counts the 
 * SAP_CHAR/SAPUC units.
 * Source and target pointer may have the same adress, resulting in an 
 * in-place conversion. If they do not have the same address, then the 
 * source and target areas must not overlap at all. 
 */
void NeutralUcnToUcn( SAP_CHAR *, const SAP_RAW *,  size_tU );
void UcnToNeutralUcn( SAP_RAW *,  const SAP_CHAR *, size_tU );


/**********************************************************************/
/* SAP_B8:   bytes storing multibyte characters                       */
/*--------------------------------------------------------------------*/
/* This type cannot be used to define a variable for a single         */
/* character !                                                        */
/* A string of this type can store characters using one or more bytes */
/* for each character.                                                */
/* The coding varies during runtime ! ( see setlocale() ).            */
/* The type SAP_B8 is compatible to char. It is used for              */
/* documentation purposes. It marks char[] which may contain          */
/* multibyte characters.                                              */
/*--------------------------------------------------------------------*/
/* This type gives direct access to many functions, as has been       */
/* defined in ANSI C and XPG4 and should be implemented everywhere.   */
/*    Classification of a single character:                           */
/*       mblen                                                        */
/*    Simple handling of zero terminated strings:                     */
/*       strcat                                                       */
/*       strcmp                                                       */
/*       strcpy                                                       */
/*       strlen    (not number of chars, but number of bytes!)        */
/*    Conversion functions:                                           */
/*       strcoll / strxfrm                                            */
/*       mbtowc / mbstowcs / wctomb / wcstombs                        */
/* Note:                                                              */
/* The character classification functions isprint, isalpha,.. are     */
/* only valid, if(mblen(ptr)==1)                                      */
/*--------------------------------------------------------------------*/

/* sapuc.h :
 * 
 * typedef char   SAP_B8 ;
 * #define        B8Null ((SAP_B8)0)
 */
typedef char * SAP_B8s;

/* Number of used bytes, not number of characters. */
#ifndef SAPccQ /* normal */
  #define B8sLen(a_multibyte_string) strlen(a_multibyte_string)
  #define B8sCpy(dest,src)           strcpy(dest,src)
#else /* for the ccU checker */
  extern size_tR   B8sLen( const SAP_B8 * );
  extern SAP_B8  * B8sCpy(       SAP_B8 *, const SAP_B8 * );
#endif


/*--------------------------------------------------------------------*/
/* SAP_UC  -  Conversions from and to multibyte                       */
/*--------------------------------------------------------------------*/
/* void     UcsToB8sNolim(       SAP_B8 * dest       ,
 *                         const SAP_UC * src        );
 * size_t   B8sToUcsNolim(       SAP_UC * dest       ,
 *                         const SAP_B8 * src        );
 *
 *
 * UcsToB8sNolim
 * --------
 * Convert a string of universal characters into a multibyte string.
 * `src` must be zero-terminated.
 * `dest` will be zero-terminated.
 * Hope there is enough space.
 * 
 * B8sToUcsNolim
 * --------
 * Convert a multibyte string into a string of universal characters.
 * `src` must be zero-terminated.
 * `dest` will be zero-terminated.
 * Hope there is enough space.
 */

/*--------------------------------------------------------------------*/
/* SAP_UC  -  Conversions from and to multibyte                  impl */
/*--------------------------------------------------------------------*/
#if defined(SAP_UC_is_1B) && ! defined(SAPccQ) 
  #define UcsToB8sNolim(dest,src)       strcpy(dest,src)
  #define B8sToUcsNolim(dest,src)       (void)strcpy(dest,src)

#elif defined(SAP_UC_is_wchar) && ! defined(SAPccQ)
  #define UcsToB8sNolim(dest,src)       wcstombs(dest,src,999999)
  #define B8sToUcsNolim(dest,src)       (void)mbstowcs(dest,src,999999)

#else /* SAPs own SAP_UC implementation or SAPccQ */
  void     UcsToB8sNolim(       SAP_B8 * dest       ,
                          const SAP_UC * src        );
  size_t   B8sToUcsNolim(       SAP_UC * dest       ,
                          const SAP_B8 * src        );

#endif /* SAP_UC_is_... */


/**********************************************************************/
/* SAP_UTF8:   bytes storing multibyte characters with UTF-8 coding   */
/*--------------------------------------------------------------------*/
/* This type cannot be used to define a variable for a single         */
/* character !                                                        */
/* A string of this type can store characters using one or more bytes */
/* for each character.                                                */
/* The coding is defined with Unicode.                                */
/* The type SAP_UTF8 is compatible to char. It is used for            */
/* documentation purposes. It marks char[] which may contain          */
/* UTF-8 multibyte characters.                                        */
/*--------------------------------------------------------------------*/
/* This type gives direct access to some functions, as has been       */
/* defined in ANSI C and XPG4 and should be implemented everywhere.   */
/*    Simple handling of zero terminated strings:                     */
/*       strcat                                                       */
/*       strcmp                                                       */
/*       strcpy                                                       */
/*       strlen    (not number of chars, but number of bytes!)        */
/*--------------------------------------------------------------------*/

/* sapuc.h :
 * 
 * typedef unsigned char   SAP_UTF8 ;
 * #define                 UTF8Null ((SAP_UTF8)0)
 */
#define                 UTF8Space ((SAP_UTF8)0x20)

/* maximum number of Utf-8 bytes produces by a SAP_UC 
   can be used to determine size of utf8-buffer */
#define SAP_UC_MAX_UTF8_LN  3

/* Number of used bytes, not number of characters.
 * Implementing Len,Cpy by macros with casts hides all type errors!!!
 * Len already had this cast, added it for Cpy.
 */
#ifndef SAPccQ /* normal */
  #define Utf8sLen(a_UTF8_string) strlen((char*)a_UTF8_string)
  #define Utf8sCpy(dest,src)      strcpy((char*)dest,(char*)src)
#else /* for the ccU checker */
  extern size_tR    Utf8sLen( const SAP_UTF8 * );
  extern SAP_UTF8 * Utf8sCpy(       SAP_UTF8 *, const SAP_UTF8 * ); 
#endif

/* 
 * Convert between UTF-8 and process characters
 *                           ( char or Unicode ) .
 */
SAPRETURN Utf8nToUcnCP(     SAP_S_CODEPAGE * system_codepage
		      ,     SAP_CHAR      ** pp_out
		      ,     SAP_CHAR       * p_outend
		      ,     SAP_UTF8      ** pp_in 
		      ,     SAP_UTF8       * p_inend );
SAPRETURN Utf8nToUcnCPFast( SAP_S_CODEPAGE * system_codepage
			  , SAP_CHAR      ** pp_out
			  , SAP_CHAR       * p_outend
			  , SAP_UTF8      ** pp_in 
			  , SAP_UTF8       * p_inend );
SAPRETURN UcnToUtf8nCP(     SAP_S_CODEPAGE * system_codepage
		      ,     SAP_UTF8      ** pp_out
		      ,     SAP_UTF8       * p_outend
		      ,     SAP_CHAR      ** pp_in 
		      ,     SAP_CHAR       * p_inend );
SAPRETURN UcnToUtf8nCPFast( SAP_S_CODEPAGE * system_codepage
			  , SAP_UTF8      ** pp_out
			  , SAP_UTF8       * p_outend
			  , SAP_CHAR      ** pp_in 
			  , SAP_CHAR       * p_inend );

SAPRETURN Utf8nToUcn_2prep( void );
SAPRETURN Utf8nToUcn_2( SAP_CHAR      ** pp_out
		      , SAP_CHAR       * p_outend
		      , SAP_UTF8      ** pp_in 
		      , SAP_UTF8       * p_inend );
SAPRETURN UcnToUtf8n_2prep( void );
SAPRETURN UcnToUtf8n_2( SAP_UTF8      ** pp_out
		      , SAP_UTF8       * p_outend
		      , SAP_CHAR      ** pp_in 
		      , SAP_CHAR       * p_inend );

SAPRETURN Utf8nToUcn_3prep( void );
SAPRETURN Utf8nToUcn_3( SAP_CHAR      ** pp_out
		      , SAP_CHAR       * p_outend
		      , SAP_UTF8      ** pp_in 
		      , SAP_UTF8       * p_inend );
SAPRETURN UcnToUtf8n_3prep( void );
SAPRETURN UcnToUtf8n_3( SAP_UTF8      ** pp_out
		      , SAP_UTF8       * p_outend
		      , SAP_CHAR      ** pp_in 
		      , SAP_CHAR       * p_inend );

SAPRETURN Utf8nToUcnCP_4(   SAP_S_CODEPAGE * system_codepage
		        ,   SAP_CHAR      ** pp_out
		        ,   SAP_CHAR       * p_outend
		        ,   SAP_UTF8      ** pp_in 
		        ,   SAP_UTF8       * p_inend );
SAPRETURN UcnToUtf8nCP_4(   SAP_S_CODEPAGE * system_codepage
		        ,   SAP_UTF8      ** pp_out
		        ,   SAP_UTF8       * p_outend
		        ,   SAP_CHAR      ** pp_in 
		        ,   SAP_CHAR       * p_inend );

BEGIN_externC
typedef SAPRETURN (* Utf8nToUcnCP_PROC)( SAP_S_CODEPAGE* system_codepage
				       , SAP_CHAR     ** pp_out
				       , SAP_CHAR      * p_outend
				       , SAP_UTF8     ** pp_in 
				       , SAP_UTF8      * p_inend );
typedef SAPRETURN (* UcnToUtf8nCP_PROC)( SAP_S_CODEPAGE* system_codepage
				       , SAP_UTF8     ** pp_out
				       , SAP_UTF8      * p_outend
				       , SAP_CHAR     ** pp_in 
				       , SAP_CHAR      * p_inend );
END_externC


 
SAPRETURN UcsToUtf8s(           SAP_UTF8 * p_out,
		          const intR       p_maxoutbytes,
                                intR     * pp_outusedbytes,
		          const SAP_UC   * p_in           );
/* UcsToUtf8sFast() is 'dirty': only 7bit characters are correctly
 * converted. Therefore the length parameters are not marked as byte 
 * counters (cf. A7sLen). 
 */
SAPRETURN UcsToUtf8sFast(         SAP_UTF8 * p_out, 
/* SAPUNICODEOK_MIXEDLEN */ const int        p_maxoutbytes,
/* SAPUNICODEOK_MIXEDLEN */       int      * pp_outusedbytes, 
                            const SAP_UC   * p_in           );
/*----------------------------------------------------------------------
 * UcsToUtf8s  -  Convert from zero-terminated universal charater string
 *                to zero-terminated UTF-8 string.
 *                With output length limitation.
 *
 * Parameters:
 *
 *    p_in            IN  Zero-terminated string of SAP_UCs.
 *
 *    p_out           IN  Pointer to destination area.
 *                    OUT *p_out receives the data.
 *
 *                        'p_in' and 'p_out' must point to
 *                        non-overlapping areas.
 *
 *    p_maxoutbytes   IN  Number of bytes, that may be written into
 *                        'p_out'. Note, that this includes space
 *                        for the trailing null-byte.
 *
 *    pp_outusedbytes IN  Null-pointer
 *                        or
 *                        pointer to variable.
 *                    OUT Number of data bytes written into 'p_out'.
 *                        Actually the routine has written
 *                        (*pp_outusedbytes +1) bytes because of the
 *                        trailing null-byte.
 * Return:
 *
 *    SAP_O_K             Conversion was possible and output area
 *                        was long enough.
 *
 *    SAPETOOLONG         Conversion was possible, but output area was
 *                        too short.
 *                        Only (*pp_outusedbytes +1) bytes have been
 *                        written. This may be less than 'p_maxoutbytes'
 *                        because a multi-byte UTF-8 characters could
 *                        fall onto the boundary.
 *                        A trailing null-byte is always written.
 */

SAPRETURN UcsToUtf8sOverlap(           SAP_UTF8 * p_out
			   ,     const intR       p_maxoutbytes
			   ,           intR     * pp_outusedbytes
			   ,     const SAP_UC   * p_in           );
SAPRETURN UcsToUtf8sFastOverlap(       SAP_UTF8 * p_out
			       , const intR       p_maxoutbytes
			       ,       intR     * pp_outusedbytes
			       , const SAP_UC   * p_in           );
/*----------------------------------------------------------------------
 * UcsToUtf8sOverlap  -  Convert from zero-terminated universal charater
 *                       string to zero-terminated UTF-8 string.
 *                       With output length limitation.
 *                       Output area and input area may overlap.
 *
 * The semantic is defined by doing the whole conversion like
 * UcsToUtf8s, but without writing any byte into 'p_out' before reading
 * all bytes from 'p_in'.
 *
 * Return:
 *
 *    SAP_O_K             See UcsToUtf8s()
 *    SAPETOOLONG         See UcsToUtf8s()
 *
 *    SAPEINTERNAL        Internal error. May be caused by memory
 *                        shortage.
 */


SAPRETURN UcnToUtf8nOverlap_2(           SAP_UTF8 * p_out
			     ,     const intR       p_maxoutbytes
			     ,           intR     * pp_outusedbytes
			     ,     const SAP_CHAR * p_in
			     ,     const intU       p_in_noofchars );
SAPRETURN UcnToUtf8nFastOverlap_2(       SAP_UTF8 * p_out
				 , const intR       p_maxoutbytes
				 ,       intR     * pp_outusedbytes
				 , const SAP_CHAR * p_in
				 , const intU       p_in_noofchars );
/*----------------------------------------------------------------------
 * UcnToUtf8nOverlap_2  -  Convert from fixed-sized universal string
 *                         to fixed-sized UTF-8 string.
 *                         With output length limitation.
 *                         Allowing overlapping areas.
 * Parameters:
 *
 *    p_in            IN  Array of 'p_in_noofchars' characters.
 *    p_in_noofchars  IN  Length of 'p_in' counted in basic characters.
 *
 *    p_out           IN  Pointer to destination area.
 *                    OUT *p_out receives the data.
 *
 *                        'p_in' and 'p_out' may point to overlapping
 *                        areas.
 *                        The semantic is defined by doing the whole
 *                        conversion without writing any byte into
 *                        'p_out' before reading all bytes from 'p_in'.
 *
 *    p_maxoutbytes   IN  Number of bytes, that may be written into
 *                        'p_out'.
 *
 *    pp_outusedbytes IN  Null-pointer
 *                        or
 *                        pointer to variable.
 *                    OUT Number of data bytes written into 'p_out'.
 *                        If the routine wanted to write a smaller
 *                        number of bytes, the rest is filled with
 *                        spaces.
 * Return:
 *
 *    SAP_O_K             Conversion was possible and output area
 *                        was long enough.
 *    SAPETOOLONG         Conversion was possible, but output area was
 *                        too short.
 *                        Only (*pp_outusedbytes) data bytes have been
 *                        written. This may be less than 'p_maxoutbytes'
 *                        because a multi-byte UTF-8 characters could
 *                        fall onto the boundary.
 *                        If (*pp_outusedbytes) is smaller than
 *                        p_maxoutbytes, the rest area is filled with
 *                        spaces.
 *
 *    SAPEINTERNAL        Internal error. May be caused by memory
 *                        shortage.
 */


SAPRETURN Utf8sToUcs(           SAP_UC   * p_out
		    ,     const intU       p_maxoutchars
		    ,           intU     * pp_outusedchars
		    ,     const SAP_UTF8 * p_in           );
SAPRETURN Utf8sToUcsFast(       SAP_UC   * p_out
			, const intU       p_maxoutchars
			,       intU     * pp_outusedchars
			, const SAP_UTF8 * p_in           );
/*----------------------------------------------------------------------
 * Utf8sToUcs  -  Convert from zero-terminated UTF-8 string to
 *                zero-terminated universal charater string;
 *                With output length limitation.
 *
 *                If SAP_UC is not Unicode and if the input data 
 *                contains characters, which cannot be converted,
 *                substitution characters ('#') are written into *p_out.
 *
 * Parameters:
 *
 *    p_in            IN  Zero-terminated UTF-8 string.
 *
 *    p_out           IN  Pointer to destination area.
 *                    OUT *p_out receives the data.
 *
 *                        'p_in' and 'p_out' must point to
 *                        non-overlapping areas.
 *
 *    p_maxoutchars   IN  Number of (basic) characters, that may be
 *                        written into 'p_out'. Note, that this includes
 *                        space for the trailing null-character.
 *
 *    pp_outusedchars IN  Null-pointer
 *                        or
 *                        pointer to variable.
 *                    OUT Number of (basic) characters written into
 *                        'p_out'.
 *                        Actually the routine has written
 *                        (*pp_outusedchars +1) characters because of
 *                        the trailing null-char.
 * Return:
 *
 *    SAP_O_K             Conversion was possible and output area
 *                        was long enough.
 *    SAPETOOLONG         Conversion was possible, but output area was
 *                        too short.
 *                        Only (*pp_outusedchars +1) chars have been
 *                        written. This may be less than 'p_maxoutchars'
 *                        once a special UTF-16 surrogate protection is
 *                        activated.
 *                        A trailing null-char is always written.
 */

SAPRETURN Utf8sToUcsOverlap(           SAP_UC   * p_out
			   ,     const intU       p_maxoutchars
			   ,           intU     * pp_outusedchars
			   ,     const SAP_UTF8 * p_in           );
SAPRETURN Utf8sToUcsFastOverlap(       SAP_UC   * p_out
			       , const intU       p_maxoutchars
			       ,       intU     * pp_outusedchars
			       , const SAP_UTF8 * p_in           );
/*----------------------------------------------------------------------
 * Utf8sToUcsOverlap  -  Convert from zero-terminated UTF-8 string to
 *                       zero-terminated universal charater string;
 *                       With output length limitation.
 *
 *                       Output area and input area may overlap.
 *
 * The semantic is defined by doing the whole conversion like
 * Utf8sToUcs, but without writing any byte into 'p_out' before reading
 * all bytes from 'p_in'.
 *
 * Return:
 *
 *    SAP_O_K             See Utf8sToUcs()
 *    SAPETOOLONG         See Utf8sToUcs()
 *
 *    SAPEINTERNAL        Internal error. May be caused by memory
 *                        shortage.
 */

SAPRETURN Utf8nToUcnOverlap_2(           SAP_CHAR * p_out
			     ,     const intU       p_maxoutchars
			     ,           intU     * pp_outusedchars
			     ,     const SAP_UTF8 * p_in
			     ,     const intR       p_in_noofbytes );
SAPRETURN Utf8nToUcnFastOverlap_2(       SAP_CHAR * p_out
				 , const intU       p_maxoutchars
				 ,       intU     * pp_outusedchars
				 , const SAP_UTF8 * p_in
				 , const intR       p_in_noofbytes );
/*----------------------------------------------------------------------
 * Utf8nToUcnOverlap_2  -  Convert from fixed-sized UTF-8 string to
 *                         fixed-sized universal character string.
 *                         With output length limitation.
 *                         Allowing overlapping areas.
 *
 *                         If SAP_UC is not Unicode and if the input
 *                         data contains characters, which cannot be
 *                         converted, substitution characters ('#') are
 *                         written into *p_out.
 * Parameters:
 *
 *    p_in            IN  Array of 'p_in_noofbytes' bytes containing
 *                        UTF-8 data.
 *    p_in_noofbytes  IN  Length of 'p_in' counted in bytes.
 *
 *    p_out           IN  Pointer to destination area.
 *                    OUT *p_out receives the data.
 *
 *                        'p_in' and 'p_out' may point to overlapping
 *                        areas.
 *                        The semantic is defined by doing the whole
 *                        conversion without writing any byte into
 *                        'p_out' before reading all bytes from 'p_in'.
 *
 *
 *    p_maxoutchars   IN  Number of (basic) characters, that may be
 *                        written into 'p_out'.
 *
 *    pp_outusedchars IN  Null-pointer
 *                        or
 *                        pointer to variable.
 *                    OUT Number of (basic) characters written into
 *                        'p_out'.
 *                        If the routine wanted to write a smaller
 *                        number of chars, the rest is filled with
 *                        spaces.
 * Return:
 *
 *    SAP_O_K             Conversion was possible and output area
 *                        was long enough.
 *    SAPETOOLONG         Conversion was possible, but output area was
 *                        too short.
 *                        Only (*pp_outusedchars) (basic) characters
 *                        have been written. This may be less than
 *                        'p_maxoutchars' once a UTF-16 surogate
 *                        protection will be enabled.
 *                        If (*pp_outusedchars) is smaller than
 *                        p_maxoutchars, the rest of the 'p_out' area
 *                        is filled with spaces.
 *
 *    SAPEINTERNAL        Internal error. May be caused by memory
 *                        shortage.
 */

/**********************************************************************/
/*                                                                    */ 
/*         Conversion functions between SAP_UC and UTF-16             */
/*         ----------------------------------------------             */
/*         -> implementedt in rscpuc3.c                               */
/*                                                                    */
/**********************************************************************/

/**********************************************************************/
/*                                                                    */ 
/* UcnToUtf16nCP   - Convert from universal SAP_UC character string   */
/*                   to universal UTF-16 string.                      */                              
/*                                                                    */ 
/*		     If SAP_UC is Unicode and if the input            */
/*                   data contains characters, which cannot be        */
/*                   converted, substitution characters ('#') are     */
/*                   written into *pp_out.                            */
/*                                                                    */
/* Parameters:                                                        */
/*                                                                    */
/*    pp_out         IN  begin of target buffer                       */
/*                   OUT points behind the last converted character   */
/*                       in the target buffer                         */
/*                                                                    */
/*    p_outend       IN  points behind end of source string           */
/*                                                                    */
/*    pp_in          IN  begin of source string                       */
/*                   OUT points behind the last converted characters  */
/*                       in the source string                         */
/*                                                                    */
/*    p_inend        IN  ponts behind end of source string            */
/*                                                                    */
/* Return:                                                            */
/*                                                                    */
/*    SAP_O_K        Conversion was possible and output area          */
/*                   was long enough.                                 */
/*                                                                    */
/*    SAPETOOLONG    Conversion was possible, but output area was     */
/*                   too short.                                       */
/*                                                                    */
/*    SAPEINTERNAL   Internal error. May be caused by memory          */
/*                   shortage.                                        */
/**********************************************************************/
SAPRETURN UcnToUtf16nCP(     SAP_S_CODEPAGE * system_codepage
		       ,     SAP_UTF16     ** pp_out
		       ,     SAP_UTF16      * p_outend
		       ,     SAP_UC        ** pp_in 
		       ,     SAP_UC         * p_inend );


/**********************************************************************/
/*                                                                    */ 
/* Utf16nToUcnCP   - Convert from universal UTF-16 character string   */
/*                   to universal SAP_UC string.                      */
/*                                                                    */
/*		     If SAP_UC is Unicode and if the input            */
/*                   data contains characters, which cannot be        */
/*                   converted, substitution characters ('#') are     */
/*                   written into *pp_out.                            */
/*                                                                    */
/*                                                                    */
/*   Parameters:       see UcnToUtf16nCP_F                            */
/*   Return    :       see UcnToUtf16nCP_F                            */
/*                                                                    */
/**********************************************************************/
SAPRETURN Utf16nToUcnCP(     SAP_S_CODEPAGE * system_codepage
		       ,     SAP_UC        ** pp_out
		       ,     SAP_UC         * p_outend
		       ,     SAP_UTF16     ** pp_in 
	               ,     SAP_UTF16      * p_inend );


/**********************************************************************/
/*                                                                    */ 
/* UcnToUtf16nCPGD   - Convert from universal SAP_UC character string */
/*                     to universal UTF-16 string with filter         */
/*                     functionality. It is assumed that data are     */
/*                     correct ("good data") and can be passed through*/
/*                     without further checks.                        */
/*                                                                    */
/*	       	       If SAP_UC is Unicode,  input characters, which */
/*                     cannot be converted, will be psassed through   */
/*                     unchagend.                                     */
/*                                                                    */
/*                                                                    */
/*   Parameters:       see UcnToUtf16nCP_F                            */
/*   Return    :       see UcnToUtf16nCP_F                            */
/*                                                                    */
/**********************************************************************/
SAPRETURN UcnToUtf16nCPGD(     SAP_S_CODEPAGE * system_codepage
			 ,     SAP_UTF16     ** pp_out
		         ,     SAP_UTF16      * p_outend
		         ,     SAP_UC        ** pp_in 
		         ,     SAP_UC         * p_inend );


/**********************************************************************/
/*                                                                    */ 
/* Utf16nToUcnCPGD   - Convert from universal UTF-16 character string */
/*                     to universal SAP_UC string with filter         */
/*                     functionality                                  */
/*                                                                    */
/*	       	       If SAP_UC is Unicode,  input characters, which */
/*                     cannot be converted, will be passed through    */
/*                     unchagend.                                     */
/*                                                                    */
/*                                                                    */
/*   Parameters:       see UcnToUtf16nCP_F                            */
/*   Return    :       see UcnToUtf16nCP_F                            */
/*                                                                    */
/**********************************************************************/
SAPRETURN Utf16nToUcnCPGD(     SAP_S_CODEPAGE * system_codepage
		         ,     SAP_UC        ** pp_out
		         ,     SAP_UC         * p_outend
		         ,     SAP_UTF16     ** pp_in 
		         ,     SAP_UTF16      * p_inend );




/**********************************************************************/
/* SAP_UTF7:   bytes storing multibyte characters with UTF-7 coding   */
/*--------------------------------------------------------------------*/
/* This type cannot be used to define a variable for a single         */
/* character !                                                        */
/* A string of this type can store characters using one or more bytes */
/* for each character.                                                */
/* The coding is defined with Unicode.                                */
/* The type SAP_UTF7 is compatible to char. It is used for            */
/* documentation purposes. It marks char[] which may contain          */
/* UTF-7 multibyte characters.                                        */
/* Note: UTF-7 encoding has modes! You always have to convert a whole */
/*       chunck of data (i.e. whole lines)                            */
/*--------------------------------------------------------------------*/
/* This type gives direct access to some functions, as has been       */
/* defined in ANSI C and XPG4 and should be implemented everywhere.   */
/*    Simple handling of zero terminated strings:                     */
/*       strlen    (not number of chars, but number of bytes!)        */
/*--------------------------------------------------------------------*/

/* sapuc.h :
 * 
 * typedef unsigned char   SAP_UTF7 ;
 * #define                 UTF7Null ((SAP_UTF7)0)
 */

#ifndef UTF7Null

typedef unsigned char   SAP_UTF7 ;
typedef unsigned char * SAP_UTF7S;

/* Number of used bytes, not number of characters. */
#ifndef SAPccQ /* normal */
  #define Utf7sLen(a_UTF7_string) strlen(a_UTF7_string)
  #define Utf7sCpy(dest,src)      strcpy(dest,src)
#else 
  extern size_tR    Utf7sLen( const SAP_UTF7 * );
  extern SAP_UTF7 * Utf7sCpy(       SAP_UTF7 *, const SAP_UTF7 * ); 
#endif 

#endif

/* Procedures are defined in rscputf.h */


/**********************************************************************/
/* SAP_A7:   a byte storing a 7-bit-US-ASCII character                */
/*--------------------------------------------------------------------*/
/* The coding is defined as 7-Bit US-ASCII. This is platform          */
/* independent. Even on an EBCDIC machine, variables of type SAP_A7   */
/* contain ASCII. Also it never contains national characters.         */
/* SAP_A7 values are compatible to UTF-8. (Only one direction.)       */
/* When text fields of SAP_A7 are defined, we expect a terminating    */
/* null.                                                              */
/* SAP_A7_A has same implementation as SAP_A7. It shall be used, when */
/* blank-padded non-terminated text fields are defined.               */
/*--------------------------------------------------------------------*/
/* Types:                                                             */
/*                  SAP_A7                                            */
/*                  SAP_A7_A                                          */
/*                                                                    */
/* Constants:                                                         */
/*                  A7Null                                            */
/*                                                                    */
/* Converting single characters:                                      */
/*                                                                    */
/* SAP_A7           UcToA7(          const SAP_UC      a_uc     )     */
/* SAP_A7           UcToA7Fast(      const SAP_UC      a_uc     )     */
/* SAP_A7           charToA7(        const char        a_char   )     */
/* SAP_A7           U2ToA7(          const SAP_U2      a_u2     )     */
/* SAP_A7           Utf8ToA7(        const SAP_UTF8    a_utf8   )     */
/*                                                                    */
/* SAP_UC           A7ToUc(          const SAP_A7      a_a7     )     */
/* char             A7ToB8(          const SAP_A7      a_a7     )     */
/* SAP_U2           A7ToU2(          const SAP_A7      a_a7     )     */
/* SAP_UTF8         A7ToUtf8(        const SAP_A7      a_a7     )     */
/*                                                                    */
/* Converting strings:                                                */
/*                                                                    */
/* const SAP_UC *   A7sToUcsVola(  const SAP_A7   * a7_val )          */
/* const SAP_CHAR * A7nToUcnVola(  const SAP_A7   * a7_val            */
/*                          ,      const int        noof_char )       */
/* const SAP_A7 *   UcsToA7sVola(  const SAP_UC   * uc_val )          */
/* const SAP_A7 *   UcnToA7nVola(  const SAP_CHAR * uc_val            */
/*                          ,      const int        noof_char )       */
/*                                                                    */
/* void             A7sToUcs(            SAP_UC    * uc_dst     ,     */
/*                                 const SAP_A7    * a7_src     )     */
/* void             A7nToUcn(            SAP_CHAR  * uc_dst     ,     */
/*                                 const SAP_A7    * a7_src     ,     */
/*                                 const int         noof_char  )     */
/* SAPRETURN        UcsToA7sCheck(       SAP_A7    * a7_dst     ,     */
/*                                 const SAP_UC    * uc_src     )     */
/* SAPRETURN        UcnToA7nCheck(       SAP_A7    * a7_dst     ,     */
/*                                 const SAP_CHAR  * uc_src     ,     */
/*                                 const int         noof_char  )     */
/* void             UcsToA7sSubst(       SAP_A7    * a7_dst     ,     */
/*                                 const SAP_UC    * uc_src     ,     */
/*                                 const SAP_UC      subst      )     */
/* void             UcnToA7nSubst(       SAP_A7    * a7_dst     ,     */
/*                                 const SAP_CHAR  * uc_src     ,     */
/*                                 const int         noof_char  ,     */
/*                                 const SAP_UC      subst      )     */
/* void             UcnToA7n(            SAP_A7    * a7_dst     ,     */
/*                                 const SAP_CHAR  * uc_src     ,     */
/*                                 const int         noof_char  )     */
/*                                                                    */
/*                                                                    */
/* Utilities:                                                         */
/*                                                                    */
/* int              A7sLen(        const SAP_A7    * a_a7s      )     */
/* SAP_A7 *         A7sCpy(              SAP_A7    * dest       ,     */
/*                                 const SAP_A7    * src        )     */
/* SAP_A7 *         A7sCat(              SAP_A7    * dest       ,     */
/*                                 const SAP_A7    * src        )     */
/* int              A7sCmp(        const SAP_A7    * left       ,     */
/*                                 const SAP_A7    * right      )     */
/*                                                                    */
/*                  A7sPrintF(           SAP_A7    * dest       ,     */
/*                                 const SAP_A7    * format           */
/*                             [ , const ?       ?   value ]... )     */
/* long             A7sToLong(     const SAP_A7    * value_str  )     */
/*                                                                    */
/*                                                                    */
/*                                                                    */
/*                                                                    */
/* int              UcsToA7s_ErrCnt (void)                            */
/* void             UcsToA7s_ErrReset (void)                          */
/*--------------------------------------------------------------------*/


/* sapuc.h :
 * 
 * typedef char   SAP_A7;
 * #define        A7Null ((SAP_A7)0)
 */

typedef SAP_A7 SAP_A7_A ;
#ifndef SAPccQ /* normal */
  #define A7sLen(a_7BitASCII_string) strlen(a_7BitASCII_string)
  #define A7sCpy(dest,src)           strcpy(dest,src)
  #define A7sCat(dest,src)           strcat(dest,src)
  #define A7sCmp(left,right)         strcmp(left,right)
#else /* for the ccU checker */
  /* A7sLen may be used as character length, too */
  /* SAPUNICODEOK_MIXEDLEN */
  extern size_t    A7sLen( const SAP_A7 * );
  extern SAP_A7 *  A7sCpy(       SAP_A7 *, const SAP_A7 * ); 
  extern SAP_A7 *  A7sCat(       SAP_A7 *, const SAP_A7 * ); 
  extern int       A7sCmp( const SAP_A7 *, const SAP_A7 * ); 
#endif

#ifdef SAPwithCHAR_EBCDIC
 /* Sorry, no A7sPrintF(), A7sToLong(). */
#else /* ASCII */
#ifndef SAPccQ /* normal */
  #define A7sPrintF                  sprintf
  #define A7sToLong(value_str)       atol(value_str)
#else /* for the ccU checker */
  extern int       A7sPrintF(       SAP_A7 * dest, 
			      const SAP_A7 * format    , ...);
  extern long      A7sToLong( const SAP_A7 * value_str );
#endif
#endif /* EBCDIC / ASCII */


/* Converting single characters
 */
#ifdef SAPwithCHAR_EBCDIC
extern SAP_B8  A7ToB8(const SAP_A7 a_7Bit);

#else /* ASCII */
#define        A7ToB8(a_7Bit) ((SAP_B8)(a_7Bit))

#endif /* EBCDIC / ASCII */

#if defined(SAP_UC_is_1B)
#ifdef SAPwithCHAR_EBCDIC
/* SAP_UC == EBCDIC */
#define        A7ToUc(a_7Bit)   ((SAP_UC)(A7ToB8(a_7Bit)))
#define        UcToA7Fast(a_uc) UcToA7(a_uc)


#else /* ASCII */
  #ifndef SAPccQ /* normal */
    #define    A7ToUc(a_7Bit)   ((SAP_UC)(a_7Bit))
    #define    UcToA7Fast(a_uc) ((SAP_A7)(a_uc))
  #else /* for the ccU checker */
    SAP_UC     A7ToUc( const SAP_A7 a_a7 );
    #define    UcToA7Fast(a_uc) UcToA7(a_uc)
  #endif
#endif /* EBCDIC / ASCII */

#else /* not UC_is_1B */
  #ifndef SAPccQ /* normal */
    #define    A7ToUc(a_7Bit)   ((SAP_UC)(a_7Bit))
    #define    UcToA7Fast(a_uc) ((SAP_A7)(a_uc))
  #else /* for the ccU checker */
    SAP_UC     A7ToUc( const SAP_A7 a_a7 );
    #define    UcToA7Fast(a_uc) UcToA7(a_uc)
  #endif
#endif /* +/- SAP_UC_is_1B */

#define        A7ToU2(a_7Bit)   ((SAP_U2)(a_7Bit))
#define        A7ToUtf8(a_7Bit) ((SAP_UTF8)(a_7Bit))

extern SAP_A7 B8ToA7( const SAP_B8 a_char );

SAP_A7         UcToA7(          const SAP_UC      a_uc       );
SAP_A7         U2ToA7(          const SAP_U2      a_u2       );
SAP_A7         Utf8ToA7(        const SAP_UTF8    a_utf8     );

/* Converting strings on the fly.
 * -----------------------------
 * A7sToUcsVola() and UcsToA7sVola() return pointers to some static
 * internal memory. So please don't wait too long before using
 * the values. But there is enough memory to handle at least fife
 * strings at the same time.
 *
 * UcsToA7sVola() will never fail. If there is a Unicode character
 * in the input, which does not belong to the ASCII range, it is
 * replaced by a default character.
 * For control of this substitution, there are three auxilliary
 * functions:
 *
 * UcsToA7s_ErrReset() clears the error counter.
 *
 * UcsToA7s_ErrCnt() returns the number of substituted characters
 * since the last call to UcsToA7s_ErrReset()
 *
 * These procedures can be used to build complicated expressions:
 *
 * if ( 0==memcmpR( ascii_command,
 *         ( netProt == USE_IP_NET ? UcsToA7sVola( command1 )
 *                                 : UcsToA7sVola( long_command ) )
 * {
 *   handle_cmd( A7sToUcsVola( ascii_command ) );
 * }
 *
 *
 * This set of procedures is  not  thread-safe:
 *   UcsToA7s_ErrCnt, UcsToA7s_ErrReset,
 *   A7sToUcsVola, A7nToUcnVola
 *   UcsToA7sVola, UcnToA7nVola .
 *
 * All the other procedures for converting from or to SAP_A7 are
 * designed to be thread-safe.
 */
const SAP_UC *   A7sToUcsVola( const SAP_A7   * a7_val );
const SAP_CHAR * A7nToUcnVola( const SAP_A7   * a7_val,  
/* SAPUNICODEOK_MIXEDLEN */    const int        noof_char );
const SAP_A7 *   UcsToA7sVola( const SAP_UC   * uc_val );
const SAP_A7 *   UcnToA7nVola( const SAP_CHAR * uc_val,  
/* SAPUNICODEOK_MIXEDLEN */    const int        noof_char );
int              UcsToA7s_ErrCnt (void);
void             UcsToA7s_ErrReset (void);

/* Converting strings from one buffer into a different buffer.
 * -----------------------------------------------------------
 * .._dst must point to an destination area, which is large enough.
 *
 * The routines UcsToA7sCheck and UcnToA7nCheck can fail
 * with the return code 1 if the source contains illegal characters.
 *
 * The routines UcsToA7sSubst and UcnToA7nSubst never fail,
 * because they will use the given substitute character to replace any
 * invalid input character. If the substitution character itself is
 * not valied, '#' is used.
 *
 * The routine UcnToA7n never fails,
 * because it will use '#' to replace any invalid input character.
 * 
 */
void             A7sToUcs(      SAP_UC          * uc_dst     ,
				const SAP_A7    * a7_src     );
void             A7nToUcn(      SAP_CHAR        * uc_dst     ,
				const SAP_A7    * a7_src     ,
/* SAPUNICODEOK_MIXEDLEN */     const int         noof_char  );
SAPRETURN        UcsToA7sCheck(       SAP_A7    * a7_dst     ,
/* SAPUNICODEOK_MIXEDLEN */     const SAP_UC    * uc_src     );
SAPRETURN        UcnToA7nCheck(       SAP_A7    * a7_dst     ,
			        const SAP_CHAR  * uc_src     ,
/* SAPUNICODEOK_MIXEDLEN */     const int         noof_char  );
void             UcsToA7sSubst(       SAP_A7    * a7_dst     , 
				const SAP_UC    * uc_src     ,
				const SAP_UC      subst      );
void             UcnToA7nSubst(       SAP_A7    * a7_dst     , 
				const SAP_CHAR  * uc_src     ,
/* SAPUNICODEOK_MIXEDLEN */     const int         noof_char  , 
			        const SAP_UC      subst      );
void             UcnToA7n(            SAP_A7    * a7_dst     , 
				const SAP_CHAR  * uc_src     ,
/* SAPUNICODEOK_MIXEDLEN */     const int         noof_char  );


/**********************************************************************/
/* SAP_B7:   byte storing an English character or simple symbol       */
/*--------------------------------------------------------------------*/
/* The coding is defined either 7-Bit US-ASCII or a subset of EBCDIC. */
/* The type SAP_B7 is compatible to char. It is used for              */
/* documentation purposes. It marks char[] which never contain        */
/* national characters.                                               */
/*--------------------------------------------------------------------*/
/* Types:                                                             */
/*                  SAP_B7                                            */
/*                                                                    */
/* Constants:                                                         */
/*                  B7Null                                            */
/*                                                                    */
/* Converting single characters:                                      */
/*                                                                    */
/* char             B7toB8(       const SAP_B7     a_b7      )        */
/* SAP_UC           B7ToUc(       const SAP_B7     a_b7      )        */
/* SAP_B7           UcToB7(       const SAP_UC     a_uc      )        */
/* SAP_B7           UcToB7Fast(   const SAP_UC     a_uc      )        */
/*                                                                    */
/* Converting strings:                                                */
/*                                                                    */
/* const SAP_UC *   B7sToUcsVola( const SAP_B7   * b7_src    )        */
/*                  B7sToUcs(           SAP_UC   * uc_dest   ,        */
/*                                const SAP_B7   * b7_src    )        */
/*                  B7nToUcn(           SAP_CHAR * uc_dest   ,        */
/*                                const SAP_B7   * b7_src    ,        */
/*                                const int        noof_char )        */
/* const SAP_B7 *   UcsToB7sVola( const SAP_UC   * uc_src    )        */
/*                  UcsToB7s(           SAP_B7   * b7_dest   ,        */
/*                                const SAP_UC   * uc_src    )        */
/*                  UcnToB7n(           SAP_B7   * b7_dest   ,        */
/*                                const SAP_CHAR * uc_src    ,        */
/*                                const int        noof_char )        */
/*                                                                    */
/* Utilities:                                                         */
/*                                                                    */
/* int              B7sLen(       const SAP_B7    * b7_src   )        */
/*                  B7sCpy(             SAP_B7    * b7_dest  ,        */
/*                                const SAP_B7    * b7_src   )        */
/* int              B7sCmp(       const SAP_B7    * b7_left  ,        */
/*                                const SAP_B7    * b7_right )        */
/*--------------------------------------------------------------------*/

/* sapuc.h :
 * 
 * typedef char SAP_B7;
 * #define      B7Null      ((SAP_B7)0)
 */

/* B7toB8 : */
#define B7toB8(a_7Bit)      ((char)(a_7Bit))

/* B7ToUc : */
#if defined(SAP_UC_is_1B)
  #define B7ToUc(a_7Bit) ((SAP_UC)(a_7Bit))
#else /* not UC_is_1B */
  #ifdef SAPwithCHAR_EBCDIC
    #define         B7ToUc(a_b7) E8ToUc((SAP_E8)a_b7)
  #else /* ASCII */
    #ifdef SAPccQ
      SAP_UC B7ToUc(const SAP_B7 a_b7);
    #else
      #define B7ToUc(a_7Bit) ((SAP_UC)(a_7Bit))
    #endif
  #endif /* EBCDIC / ASCII */
#endif /* +/- SAP_UC_is_1B */

/* UcToB7 : */
#if defined(SAP_UC_is_1B) && ! defined(SAPccQ)
  #define UcToB7(a_uc) (a_uc)
#else /* not UC_is_1B or SAPccQ */
  SAP_B7        UcToB7( const SAP_UC a_uc );
#endif /* +/- SAP_UC_is_1B resp -/+ SAPccQ */

/* UcToB7Fast : */
#if defined(SAPccQ)
  SAP_B7        UcToB7Fast( const SAP_UC a_uc );
#elif defined(SAP_UC_is_1B)
  #define UcToB7Fast(a_uc) (a_uc)
#elif defined(SAPwithCHAR_EBCDIC) /* under Unicode */
  #define UcToB7Fast(a_uc) UcToB7(a_uc)
#else /* Unicode on ASCII */
  #define UcToB7Fast(a_uc) ((SAP_B7)(a_uc))
#endif

/* B7sToUcsVola : B7sToUcs : B7nToUcn : */
#if defined(SAP_UC_is_1B) && ! defined(SAPccQ)
  #define B7sToUcsVola( b7_src )       ((const SAP_UC *)(b7_src))
  #define B7sToUcs( uc_dest, b7_src )  strcpy( uc_dest, b7_src )
  #define B7nToUcn( uc_dest, b7_src, noof_char) \
		                       memcpyR( uc_dest, b7_src, noof_char )
#else /* not UC_is_1B or SAPccQ */
  const SAP_UC * B7sToUcsVola( const SAP_B7   * b7_src    );
  void           B7sToUcs(           SAP_UC   * uc_dest   ,
                               const SAP_B7   * b7_src    );
  void           B7nToUcn(           SAP_CHAR * uc_dest   ,
                               const SAP_B7   * b7_src    , 
/* SAPUNICODEOK_MIXEDLEN */    const int        noof_char );
#endif /* +/- SAP_UC_is_1B resp -/+ SAPccQ */

const SAP_B7 * UcsToB7sVola( const SAP_UC   * uc_src    );
void           UcsToB7s(           SAP_B7   * b7_dest   , 
                             const SAP_UC   * uc_src    );
void           UcnToB7n(           SAP_B7   * b7_dest   , 
                             const SAP_CHAR * uc_src    , 
/* SAPUNICODEOK_MIXEDLEN */  const int        noof_char );

/* B7sLen : B7sCpy : B7sCmp : */
#ifndef SAPccQ /* normal */
  #define B7sLen(a_7Bit_string) strlen(a_7Bit_string)
  #define B7sCpy(dest,src)      strcpy(dest,src)
  #define B7sCmp(left,right)    strcmp(left,right)
#else /* for the ccU checker */
  /* B7sLen may be used as character length, too */
  /* SAPUNICODEOK_MIXEDLEN */
  extern size_t    B7sLen( const SAP_B7 * );
  extern SAP_B7 *  B7sCpy(       SAP_B7 *, const SAP_B7 * );
  extern int       B7sCmp( const SAP_B7 *, const SAP_B7 * );
#endif


#ifdef SAP_UC_is_1B
#else  /* non 1B */
#ifdef SAPwithCHAR_EBCDIC
/* not yet: extern SAP_U2   B7ToU2( SAP_B7 a_7Bit ); */
/* not yet: extern SAP_UTF8 B7ToUtf8( SAP_B7 a_7Bit ); */
#else /* ASCII */
/* not yet: #define B7ToU2(a_7Bit) ((SAP_U2)(a_7Bit)) */
/* not yet: #define B7ToUtf8(a_7Bit) ((SAP_UTF8)(a_7Bit)) */
#endif /* EBCDIC / ASCII */
#endif /* +/- 1B */
  
/* Converting strings from one buffer into a different buffer.
 * -----------------------------------------------------------
 * .._dst must point to an destination area, which is large enough.
 *
 * In case of B7nToUcn,.. the destination has to have exactly two 
 * (or 4) times as many bytes as the source.
 * In case of UcnToB7n,.. the destination has to have exactly half
 * (or 1/4) that many bytes as the source.
 *
 * The routines UcnToB7n,.. never fail,
 * because they will use the given substitute character to replace any
 * invalid input character. If the substitution character itself is
 * not valid, '#' is used.
 */


/**********************************************************************/
/* SAP_E8:   a byte storing a 8-Bit IBM 500 EBCDIC character          */
/*--------------------------------------------------------------------*/
/* The coding is defined as 8-Bit IBM 500 EBCDIC. This is platform    */
/* independent. Even on an ASCII machine, variables of type SAP_E8    */
/* contain EBCDIC. If it contains national characters, these are from */
/* the western European area.                                         */
/* When text fields of SAP_E8 are defined, we expect a terminating    */
/* null.                                                              */
/* SAP_E8_A has same implementation as SAP_E8. It shall be used, when */
/* blank-padded non-terminated text fields are defined.               */
/*--------------------------------------------------------------------*/
/* Types:                                                             */
/*                  SAP_E8                                            */
/*                  SAP_E8_A                                          */
/*                                                                    */
/* Constants:                                                         */
/*                  E8Null                                            */
/*                  E8_MASK                                           */
/*                                                                    */
/* Converting single characters:                                      */
/*                                                                    */
/* SAP_E8           UcToE8(       const SAP_UC     a_uc      )        */
/* SAP_E8           U2ToE8(       const SAP_U2     a_u2      )        */
/* SAP_E8           charToE8(     const SAP_CHAR   a_char    )        */
/*                                                                    */
/* SAP_UC           E8ToUc(       const SAP_E8     a_e8      )        */
/* SAP_U2           E8ToU2(       const SAP_E8     a_e8      )        */
/* char             E8tochar(     const SAP_E8     a_e8      )        */
/*                                                                    */
/* SAP_E8           A7ToE8(       const SAP_A7     a_a7      )        */
/* SAP_A7           E8ToA7(       const SAP_E8     a_e8      )        */
/*                                                                    */
/* Converting strings:                                                */
/*                                                                    */
/* const SAP_UC *   E8sToUcsVola( const SAP_E8   * e8_val    )        */
/* const SAP_CHAR * E8nToUcnVola( const SAP_E8   * e8_val             */
/*                              , const int        noof_char )        */
/* const SAP_E8 *   UcsToE8sVola( const SAP_UC   * uc_val    )        */
/* const SAP_E8 *   UcnToE8nVola( const SAP_CHAR * uc_val             */
/*                              , const int        noof_char )        */
/*                                                                    */
/* void             E8sToUcs(           SAP_UC   * uc_dst    ,        */
/*                                const SAP_E8   * e8_src    )        */
/* void             E8nToUcn(           SAP_CHAR * uc_dst    ,        */
/*                                const SAP_E8   * e8_src    ,        */
/*                                const int        noof_char )        */
/* SAPRETURN        UcsToE8sCheck(      SAP_E8   * e8_dst    ,        */
/*                                const SAP_UC   * uc_src    )        */
/* SAPRETURN        UcnToE8nCheck(      SAP_E8   * e8_dst    ,        */
/*                                const SAP_CHAR * uc_src    ,        */
/*                                const int        noof_char )        */
/* void             UcsToE8sSubst(      SAP_E8   * e8_dst    ,        */
/*                                const SAP_UC   * uc_src    ,        */
/*                                const SAP_UC     subst     )        */
/* void             UcnToE8nSubst(      SAP_E8   * e8_dst    ,        */
/*                                const SAP_CHAR * uc_src    ,        */
/*                                const int        noof_char ,        */
/*                                const SAP_UC subst         )        */
/*                                                                    */
/* void             A7nToE8nInPlace(    void     * e8_or_a7  ,        */
/*                                const int        noof_char )        */
/* void             E8nToA7nInPlace(    void     * e8_or_a7  ,        */
/*                                const int        noof_char )        */
/*                                                                    */
/* Utilities:                                                         */
/*                                                                    */
/* int              E8sLen(       const SAP_E8    * e8_src   )        */
/*                  E8sCpy(             SAP_E8    * e8_dest  ,        */
/*                                const SAP_E8    * e8_src   )        */
/* int              E8sCmp(       const SAP_E8    * e8_left  ,        */
/*                                const SAP_E8    * e8_right )        */
/*                                                                    */
/* int              UcsToE8s_ErrCnt (void)                            */
/* void             UcsToE8s_ErrReset (void)                          */
/*--------------------------------------------------------------------*/

/* sapuc.h :
 * 
 * typedef unsigned char   SAP_E8;
 * #define                 E8Null   ((SAP_E8)0)
 */

typedef SAP_E8          SAP_E8_A ;

#ifndef SAPccQ /* normal */
  #define E8sLen(a_EBCDIC_string) strlen(a_EBCDIC_string)
  #define E8sCpy(dest,src)        strcpy(dest,src)
  #define E8sCmp(left,right)      strcmp(left,right)
#else /* for the ccU checker */
  extern size_tR   E8sLen( const SAP_E8 * );
  extern SAP_E8 *  E8sCpy(       SAP_E8 *, const SAP_E8 * );
  extern int       E8sCmp( const SAP_E8 *, const SAP_E8 * );
#endif

#define E8_MASK 0xFF


/* Converting single characters
 */
extern char     E8ToB8( const SAP_E8 a_E500 );
extern SAP_UC   E8ToUc( const SAP_E8 a_E500 );
extern SAP_U2   E8ToU2( const SAP_E8 a_E500 );
extern SAP_E8 charToE8( const char a_char );
extern SAP_E8   UcToE8( const SAP_UC a_uc );
extern SAP_E8   U2ToE8( const SAP_U2 a_u2 );

extern SAP_E8   A7ToE8( const SAP_A7 a_a7 );
extern SAP_A7   E8ToA7( const SAP_E8 a_e8 );

/* Converting strings on the fly.
 * -----------------------------
 * E8sToUcsVola() and UcsToE8sVola() return pointers to some static
 * internal memory. So please don't wait too long before using
 * the values. But there is enough memory to handle at least fife
 * strings at the same time.
 *
 * UcsToE8sVola() will never fail. If there is a Unicode character
 * in the input, which does not belong to the ASCII range, it is
 * replaced by a default character.
 * For control of this substitution, there are three auxilliary
 * functions:
 *
 * UcsToE8s_ErrReset() clears the error counter.
 *
 * UcsToE8s_ErrCnt() returns the number of substituted characters
 * since the last call to UcsToE8s_ErrReset()
 *
 * These procedures can be used to build complicated expressions:
 *
 * if ( 0==memcmpR( ascii_command,
 *         ( netProt == USE_IP_NET ? UcsToE8sVola( command1 )
 *                                 : UcsToE8sVola( long_command ) )
 * {
 *   handle_cmd( E8sToUcsVola( ascii_command ) );
 * }
 *
 *
 * This set of procedures is  not  thread-safe:
 *   UcsToE8s_ErrCnt, UcsToE8s_ErrReset,
 *   E8sToUcsVola, E8nToUcnVola,
 *   UcsToE8sVola, UcnToE8nVola .
 *
 * All the other procedures for converting from or to SAP_E8 are
 * designed to be thread-safe.
 */
const SAP_UC *    E8sToUcsVola( const SAP_E8   * e8_val    );
const SAP_CHAR *  E8nToUcnVola( const SAP_E8   * e8_val    , 
/* SAPUNICODEOK_MIXEDLEN */     const int        noof_char );
const SAP_E8 *    UcsToE8sVola( const SAP_UC   * uc_val    );
const SAP_E8 *    UcnToE8nVola( const SAP_CHAR * uc_val    , 
/* SAPUNICODEOK_MIXEDLEN */     const int        noof_char );
int               UcsToE8s_ErrCnt (void);
void              UcsToE8s_ErrReset (void);

/* Converting strings from one buffer into a different buffer.
 * -----------------------------------------------------------
 * .._dst must point to an destination area, which is large enough.
 *
 * The routines UcsToE8sCheck and UcnToE8nCheck can fail
 * with the return code 1 if the source contains illegal characters.
 *
 * The routines UcsToE8sSubst and UcnToE8nSubst never fail,
 * because they will use the given substitute character to replace any
 * invalid input character. If the substitution character itself is
 * not valied, '#' is used.
 * 
 */
void             E8sToUcs(            SAP_UC   * uc_dst    ,
				const SAP_E8   * e8_src    );
void             E8nToUcn(            SAP_CHAR * uc_dst    ,
				const SAP_E8   * AS4_EXT_PTR  e8_src    ,
/* SAPUNICODEOK_MIXEDLEN */     const int        noof_char );
SAPRETURN        UcsToE8sCheck(       SAP_E8   * e8_dst    ,
				const SAP_UC   * uc_src    );
SAPRETURN        UcnToE8nCheck(       SAP_E8   * e8_dst    ,
				const SAP_CHAR * uc_src    ,
/* SAPUNICODEOK_MIXEDLEN */     const int        noof_char );
void             UcsToE8sSubst(       SAP_E8   * e8_dst    ,
				const SAP_UC   * uc_src    ,
				const SAP_UC     subst     );
void             UcnToE8nSubst(       SAP_E8   * e8_dst    ,
				const SAP_CHAR * uc_src    ,
/* SAPUNICODEOK_MIXEDLEN */     const int        noof_char ,
				const SAP_UC subst         );
void             A7nToE8nInPlace(     void     * e8_or_a7  ,
                                const int        noof_char );
void             E8nToA7nInPlace(     void     * e8_or_a7  ,
                                const int        noof_char );


/* A7 <-> E8 */

const SAP_A7*    E8sToA7sVola(  const SAP_E8   * e8_val    );
const SAP_A7*    E8nToA7nVola(  const SAP_E8   * e8_val    ,
				const int        noof_char );
const SAP_E8*    A7sToE8sVola(  const SAP_A7   * a7_val    );
const SAP_E8*    A7nToE8nVola(  const SAP_A7   * a7_val    ,
			        const int        noof_char );


/**********************************************************************/
/* SAP_UC_MB: multibyte character representation of SAP_UC            */
/*--------------------------------------------------------------------*/
/* This type contains the multibyte character representation of       */
/* of SAP_UC                                                          */
/* in Unicode-Mode this is Utf8                                       */
/* in Non-Unicode-Mode the encoding is determined by the active       */
/* codepage ( see setlocale() ) and thereby varies during runtime     */
/* The type SAP_UC_MB is compatible to char and can be used to call   */
/* platform functions if they support Utf8                            */
/*--------------------------------------------------------------------*/
/* Types:                                                             */
/*                  SAP_UC_MB                                         */
/*                                                                    */
/* Constants:                                                         */
/*                  UCMBNull                                          */
/*                                                                    */
/* Converting strings:                                                */
/*                                                                    */
/* SAPRETURN        UcnToUcmbn(       SAP_UC_MB ** pp_out   ,         */
/*                              const SAP_UC_MB  * p_outend ,         */
/*                                    SAP_CHAR  ** pp_in    ,         */
/*                              const SAP_CHAR   * p_inend  )         */
/*                                                                    */
/* SAPRETURN        UcmbnToUcn(       SAP_CHAR  ** pp_out   ,         */
/*                              const SAP_CHAR   * p_outend ,         */
/*                                    SAP_UC_MB ** pp_in    ,         */
/*                              const SAP_UC_MB  * p_inend  )         */
/*                                                                    */
/* Utilities:                                                         */
/*                                                                    */
/* int              UCMBsLen(     const SAP_UC_MB * um_src   )        */
/*                  UCMBsCpy(           SAP_UC_MB * um_dest  ,        */
/*                                const SAP_UC_MB * um_src   )        */
/* int              UCMBsCmp(     const SAP_UC_MB * um_left  ,        */
/*                                const SAP_UC_MB * um_right )        */
/*                                                                    */
/*--------------------------------------------------------------------*/

/* sapuc.h :
 * 
 * typedef char            SAP_UC_MB;
 * #define                 UCMBNull   ((SAP_UC_MB)0)
 */

#ifndef SAPccQ /* normal */
  #define UCMBsLen(a_multibyte_string) strlen(a_multibyte_string)
  #define UCMBsCpy(dest,src)           strcpy(dest,src)
  #define UCMBsCmp(left,right)         strcmp(left,right)
#else /* for the ccU checker */
  extern size_t       UCMBsLen( const SAP_UC_MB * );
  extern SAP_UC_MB *  UCMBsCpy(       SAP_UC_MB *, const SAP_UC_MB * );
  extern int          UCMBsCmp( const SAP_UC_MB *, const SAP_UC_MB * );
#endif

/*----------------------------------------------------------------------
 * UcnToUcmbn  -  Convert from universal character string to
 *                universal multibyte string
 * UcmbnToUcn  -  Convert from universal multibyte string to
 *                universal character string
 *                begin and end of source and target buffer are marked
 *                with a pair of pointers
 *
 * Parameters:
 *
 *    pp_out          IN  begin of target buffer
 *                    OUT points behind the last converted character
 *                        in the target buffer
 *
 *    p_outend        IN  points behind end of source string
 *
 *    pp_in           IN  begin of source string
 *                    OUT points behind the last converted characters
 *                        in the source string
 *
 *    p_inend         IN  ponts behind end of source string
 *
 * Return:
 *
 *    SAP_O_K         Conversion was possible and output area
 *                    was long enough.
 *
 *    SAPETOOLONG     Conversion was possible, but output area was
 *                    too short.
 *                    Only *pp_out points behind the last written byte
 *                    This may be less than p_inend because a multi-byte
 *                    character could fall onto the boundary.
 */

SAPRETURN        UcnToUcmbn(       SAP_UC_MB ** pp_out   ,
                             const SAP_UC_MB  * p_outend ,
                                   SAP_CHAR  ** pp_in    ,
                             const SAP_CHAR   * p_inend  );


SAPRETURN        UcmbnToUcn(       SAP_CHAR  ** pp_out   ,
                             const SAP_CHAR   * p_outend ,
                                   SAP_UC_MB ** pp_in    ,
                             const SAP_UC_MB  * p_inend  );


/* Make names compatible:
 */

#define UCNull UC_NULL

END_externC

#endif /* SAPUC2_H */


/*
 * Old names. Please dont use any longer.
 *
 * Once all sources are modified and all open corrections are back,
 * these macros will be delted again.
 *
 *      __old__             __new__              */
#define SAP_MB              SAP_B8
#define A7Tochar            A7ToB8
#define A7aToUca            A7nToUcnVola
#define A7aToUca_AAN        A7nToUcn
/* fine A7sToUcs            A7sToUcsVola   * no double step */
#define A7sToUcs_AA         A7sToUcs
#define B7aToUca_AAN        B7nToUcn
/* fine B7sToUcs            B7sToUcsVola   * no double step */
#define B7sToUcs_AA         B7sToUcs
#define B7tochar            B7toB8
#define E8Tochar            E8ToB8
#define E8aToUca            E8nToUcnVola
/* fine E8sToUcs            E8sToUcsVola   * no double step */
#define E8sToUcs_AA         E8sToUcs
#define MbsLen              B8sLen
#define MbsToUcs            B8sToUcsNolim
#define U2sToUtf7s_AAAA     U2nToUtf7n
#define U2sToUtf8s_AAAA     U2nToUtf8n
#define U4sToUtf16s_AAAA    U4nToUtf16n
#define U4sToUtf7s_AAAA     U4nToUtf7n
#define U4sToUtf8s_AAAA     U4nToUtf8n
#define UcaToA7a            UcnToA7nVola
#define UcaToA7a_AANS       UcnToA7nSubst
#define UcaToA7a_EAAN       UcnToA7nCheck
#define UcaToB7a_AAN        UcnToB7n
#define UcaToE8a            UcnToE8nVola
#define UcaToE8a_EAAN       UcnToE8nCheck
#define UcaToE8a_AANS       UcnToE8nSubst
#define UcsToA7s            UcsToA7sVola
#define UcsToA7s_AAS        UcsToA7sSubst
#define UcsToA7sSubstNolim  UcsToA7sSubst   /* intermediate name */
#define UcsToA7s_EAA        UcsToA7sCheck
#define UcsToA7sCheckNolim  UcsToA7sCheck   /* intermediate name */
/* fine UcsToB7s            UcsToB7sVola   * no double step */
#define UcsToB7sNolim       UcsToB7s        /* intermediate name */
#define UcsToB7s_AA         UcsToB7s
/* fine UcsToE8s            UcsToE8sVola   * no double step */
#define UcsToE8s_AAS        UcsToE8sSubst
#define UcsToE8s_EAA        UcsToE8sCheck
#define UcsToMbs            UcsToB8sNolim
#define UcsToU2sNolim       UcsToU2s
#define UcsToU2s_AA         UcsToU2s
#define UcsToU2s_AAN        UcnToU2n
#define UcsToUTF16s_AA      UcsToUTF16sNolim
#define Utf16sToUcs_AA      Utf16sToUs
#define Utf16aToUca_AAN     Utf16nToUn
#define Utf16sToU4s_AAAA    Utf16nToU4n
#define Utf16sToUtf8s_AAAA  Utf16nToUtf8n
#define Utf7sToU2s_AAAA     Utf7nToU2n
#define Utf7sToU4s_AAAA     Utf7aToU4a
#define Utf8sToU2s_AAAA     Utf8nToU2n
#define Utf8sToU4s_AAAA     Utf8nToU4n
#define Utf8sToUtf16s_AAAA  Utf8nToUtf16n
