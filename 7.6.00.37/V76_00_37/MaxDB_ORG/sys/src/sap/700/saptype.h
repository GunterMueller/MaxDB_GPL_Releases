/* @(#) $Id: //sapdb/V76/c_00/b_37/sys/src/sap/700/saptype.h#1 $ SAP*/
/* CCQ_CCU_FILE_OFF */
/**********************************************************************/
/**********************************************************************/
/*                                                                    */
/*  (C) Copyright SAP AG, Walldorf  1996 - 1997                       */
/*                                                                    */
/*                                                                    */
/*  saptype.h                                                         */
/*                                                                    */
/*  saptype.h (explanation of the file name)                          */
/*  =========                                                         */
/*                                                                    */
/*                                                                    */
/*  Author:   Uli Schoenbeck (d022181)                                */
/*  Reviewer:                                                         */
/*                                                                    */
/*  Description: Include file with definition of the current platform */
/*                                                                    */
/*  Created:                                                          */
/*    12.08.1996                                                      */
/*                                                                    */
/*  Modified: 22.08.1996 Ralf Hackmann                                */
/*  Remarks:  adapted to AS/400                                       */
/*                                                                    */
/*  Modified:                                                         */
/*  Remarks:                                                          */
/*                                                                    */
/**********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef SAPTYPE_H

  #if defined(__cplusplus) && !defined(SAPccQC)
    #ifndef externC
      #define externC extern "C"
    #endif
    #ifndef BEGIN_externC
      #define BEGIN_externC extern "C" {
      #define END_externC }
    #endif
  #else
    #ifndef externC
      #define externC extern
    #endif
    #ifndef BEGIN_externC
      #define BEGIN_externC
      #define END_externC
    #endif
  #endif

  /* defines to support correct handling of C standard library when
     compiling ANSI C++.
     ANSI C++ requires that:
     1. different names for the C standard headers
        (e.g. <cstdlib> instead of <stdlib.h>)
     2. the content of the C standard headers lies in the namespace std
        (e.g. std::strlen instead of strlen)
  */
  #if defined(CPP_USE_NEW_C_HEADERS)
    #define NS_STD_C_HEADER std ::
    #define BEGIN_NS_STD_C_HEADER namespace std {
    #define END_NS_STD_C_HEADER }
  #else
    #define NS_STD_C_HEADER
    #define BEGIN_NS_STD_C_HEADER
    #define END_NS_STD_C_HEADER
  #endif

/*--------------------------------------------------------------------*/
/* Holger Stasch: export specification needed on NT for variables     */
/*                specified in DATA section of dwexport.nt            */
/*--------------------------------------------------------------------*/

  # undef __SAP_DLL_DECLSPEC
  # if defined(__SAP_DESIGNATED_DLL)
  #  if defined(SAPonNT)
  #   if defined(__SAP_DLL_EXPORT)
  #    define __SAP_DLL_DECLSPEC __declspec(dllexport)
  #   else
  #    define __SAP_DLL_DECLSPEC __declspec(dllimport)
  #   endif
  #  else
  #   define __SAP_DLL_DECLSPEC
  #  endif
  # else
  #  define __SAP_DLL_DECLSPEC
  # endif

  #define SAPTYPE_H

  /* NI has a maximum of 2048 Handles and needs at least masks of    */
  /* this size                                                       */
  /* AIX is broken...                                                */
  #if ! defined (SAPonRS6000)
    #ifdef FD_SETSIZE
      #if FD_SETSIZE < 2048
        #undef FD_SETSIZE
        #define FD_SETSIZE 2048
      #endif
    #else
      #define FD_SETSIZE 2048
    #endif
  #endif

  #if defined (SAPonALPHA)

    /* ################################################################## */
    /* START: old 'typealphaosf.h'-file */
    /* ################################################################## */



    #define SAPTYPE_HF cU("DEC Alpha with OSF/1")


    /**********************************************************************/
    /*  switches for conditional compiling                                */
    /**********************************************************************/


    #define SAPonUNIX    1
    #define SAPonSYS5    1
    #define SAPonSYS5_2  1
    #define SAPonOSF1    1

    #define SAPwithX     1
    #define SAPwithSHM   1


    /**********************************************************************/
    /*  Modifiers for exported Windows DLL functions                      */
    /**********************************************************************/
    #ifndef SAP_MODIFIERS
      #define SAP_MODIFIERS
      #define SAP_FAR
      #define SAP_PASCAL
      #define SAP_EXPORT
      #define SAP_LOADDS
      #define API_DF
      #define API_DF_C
    #endif /* SAP_MODIFIERS */

    /**********************************************************************/
    /*  classification of int, float and char representation              */
    /*  - correct classification is checked in abcglob.c                  */
    /*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
    /*            SAPwithFLOAT_IEEE/370                                   */
    /*            SAPwithCHAR_ASCII/EBCDIC                                */
    /**********************************************************************/
    #define SAPwithINT_LITTLEENDIAN  1
    #define SAPwithFLOAT_IEEE        1
    #define SAPwithCHAR_ASCII        1

    /**********************************************************************/
    /* always 64-bit compilation                                          */
    /**********************************************************************/
    #ifndef SAPwith64_BIT
      #define SAPwith64_BIT
    #endif

    /**********************************************************************/
    /*                                                                    */
    /*  Platform identification                                           */
    /*                                                                    */
    /*  SAP_SYSTEMID can be used to uniquely identify the platform, where */
    /*  SAP is running.                                                   */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    #define SAP_SYSTEMID 0x120


  #elif defined (SAPonBS2P)

    /* ################################################################## */
    /* START: old 'typebs2p.h'-file */
    /* ################################################################## */



    #define SAPTYPE_HF cU("BS2000 (370/390) with POSIX V1.0 in System V mode")

    /* we use the typerm600.h as basis of typebs2p.h */
    /**********************************************************************/
    /*  switches for conditional compiling                                */
    /**********************************************************************/


    #define SAPonUNIX    1
    #define SAPonSYS5    1
    #define SAPonSYS5_2  1
    #define SAPonSYS5_4  1

    #define SAPonSINIX   1
    #define SAPonSINIX5_42   1

    /* #define SAPwithX     1 */
    #define SAPwithSHM   1


    /**********************************************************************/
    /*  Modifiers for exported Windows DLL functions                      */
    /**********************************************************************/
    #ifndef SAP_MODIFIERS
      #define SAP_MODIFIERS
      #define SAP_FAR
      #define SAP_PASCAL
      #define SAP_EXPORT
      #define SAP_LOADDS
      #define API_DF
      #define API_DF_C
    #endif /* SAP_MODIFIERS */


    /**********************************************************************/
    /*  classification of int, float and char representation              */
    /*  - correct classification is checked in abcglob.c                  */
    /*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
    /*            SAPwithFLOAT_IEEE/370                                   */
    /*            SAPwithCHAR_ASCII/EBCDIC                                */
    /**********************************************************************/
    #define SAPwithINT_BIGENDIAN  1
    #define SAPwithFLOAT_370      1
    #define SAPwithCHAR_EBCDIC    1


    /**********************************************************************/
    /*                                                                    */
    /*  Platform identification                                           */
    /*                                                                    */
    /*  SAP_SYSTEMID can be used to uniquely identify the platform, where */
    /*  SAP is running.                                                   */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    #define SAP_SYSTEMID  0x1300




  #elif defined (SAPonDGUX)

    /* ################################################################## */
    /* START: old 'typedg.h'-file */
    /* ################################################################## */



    #define SAPTYPE_HF cU("Data General on M88K RISC CPU with SYSV 5.4 rel 3.10")


    /**********************************************************************/
    /*  switches for conditional compiling                                */
    /**********************************************************************/

    #define SAPonUNIX   1
    #define DGUX	    1

    #define SAPonSYS5   1
    #define SAPonSYS5_4 1

    #define SAPwithX    1
    #define SAPwithSHM  1


    /**********************************************************************/
    /*  Modifiers for exported Windows DLL functions                      */
    /**********************************************************************/
    #ifndef SAP_MODIFIERS
      #define SAP_MODIFIERS
      #define SAP_FAR
      #define SAP_PASCAL
      #define SAP_EXPORT
      #define SAP_LOADDS
      #define API_DF
      #define API_DF_C
    #endif /* SAP_MODIFIERS */


    /**********************************************************************/
    /*  classification of int, float and char representation              */
    /*  - correct classification is checked in abcglob.c                  */
    /*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
    /*            SAPwithFLOAT_IEEE/370                                   */
    /*            SAPwithCHAR_ASCII/EBCDIC                                */
    /**********************************************************************/
    #define SAPwithINT_BIGENDIAN  1
    #define SAPwithFLOAT_IEEE     1
    #define SAPwithCHAR_ASCII     1


    /**********************************************************************/
    /*                                                                    */
    /*  Platform identification                                           */
    /*                                                                    */
    /*  SAP_SYSTEMID can be used to uniquely identify the platform, where */
    /*  SAP is running.                                                   */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    #define SAP_SYSTEMID  0x181




  #elif defined (SAPonHPPA)

    /* ################################################################## */
    /* START: old 'typehp.h'-file */
    /* ################################################################## */

    #if !defined (SAPonHPIA64)
    #define SAPTYPE_HF cU("HP/9000 (PA-RISC) with HP-UX")
    #else
    #define SAPTYPE_HF cU("HP (IA-64) with HP-UX")
    #endif


    /**********************************************************************/
    /*  switches for conditional compiling                                */
    /**********************************************************************/

    #define SAPonHP9000_825  1
    #define SAPonHP9000_8xx  1

    #define SAPonUNIX        1
    #define SAPonSYS5        1
    #define SAPonSYS5_2      1
    #define SAPonHP_UX       1

    #define SAPwithX         1
    #define SAPwithSHM       1


    /**********************************************************************/
    /*  Modifiers for exported Windows DLL functions                      */
    /**********************************************************************/
    #ifndef SAP_MODIFIERS
      #define SAP_MODIFIERS
      #define SAP_FAR
      #define SAP_PASCAL
      #define SAP_EXPORT
      #define SAP_LOADDS
      #define API_DF
      #define API_DF_C
    #endif /* SAP_MODIFIERS */


    /**********************************************************************/
    /*  classification of int, float and char representation              */
    /*  - correct classification is checked in abcglob.c                  */
    /*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
    /*            SAPwithFLOAT_IEEE/370                                   */
    /*            SAPwithCHAR_ASCII/EBCDIC                                */
    /**********************************************************************/
    #define SAPwithINT_BIGENDIAN  1
    #define SAPwithFLOAT_IEEE     1
    #define SAPwithCHAR_ASCII     1


    /**********************************************************************/
    /* detect 64/32-bit compilation model and set according flags         */
    /* see hp cc manpage, +DD64 option, LP64 data model                   */
    /**********************************************************************/
    #if defined(__LP64__)
      #ifndef SAPwith64_BIT
        #define SAPwith64_BIT
      #endif
    #endif

    /**********************************************************************/
    /* set compile flag for oracle 64-bit/oci headers                     */
    /**********************************************************************/
    #ifdef SAPwith64_BIT
      #define SS_64BIT_SERVER 1
      #define HPPA64 1
    #endif

    /**********************************************************************/
    /*                                                                    */
    /*  Platform identification                                           */
    /*                                                                    */
    /*  SAP_SYSTEMID can be used to uniquely identify the platform, where */
    /*  SAP is running.                                                   */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    #ifdef SAPonHPIA64
      #define SAP_SYSTEMID  0x112
    #elif defined(SAPwith64_BIT)
      #define SAP_SYSTEMID  0x111
    #else
      #define SAP_SYSTEMID  0x110
    #endif

    /**********************************************************************/
    /*                                                                    */
    /*  IA64 generally needs 16 byte alignment for the spilled floating   */
    /*  point registers in structures like jmp_buf and ucontext_t.        */
    /*                                                                    */
    /*  Intel's IA64 ABI specifies 'long double' to be 16 byte for LP64.  */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    #ifdef SAPonHPIA64
      #define PLATFORM_MAX_T platform_max_t
      typedef long double platform_max_t;
    #endif




  #elif defined (SAPonM88K)

    /* ################################################################## */
    /* START: old 'typem88k.h'-file */
    /* ################################################################## */



    #define SAPTYPE_HF cU("Motorola on M88K risc cpu with SYSV 5.4 rel 4")


    /**********************************************************************/
    /*  switches for conditional compiling                                */
    /**********************************************************************/


    #define SAPonUNIX   1
    #define SAPonSYS5   1
    #define SAPonSYS5_4 1

    #define SAPwithX    1
    #define SAPwithSHM  1


    /**********************************************************************/
    /*  Modifiers for exported Windows DLL functions                      */
    /**********************************************************************/
    #ifndef SAP_MODIFIERS
      #define SAP_MODIFIERS
      #define SAP_FAR
      #define SAP_PASCAL
      #define SAP_EXPORT
      #define SAP_LOADDS
      #define API_DF
      #define API_DF_C
    #endif /* SAP_MODIFIERS */


    /**********************************************************************/
    /*  classification of int, float and char representation              */
    /*  - correct classification is checked in abcglob.c                  */
    /*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
    /*            SAPwithFLOAT_IEEE/370                                   */
    /*            SAPwithCHAR_ASCII/EBCDIC                                */
    /**********************************************************************/
    #define SAPwithINT_BIGENDIAN  1
    #define SAPwithFLOAT_IEEE     1
    #define SAPwithCHAR_ASCII     1


    /**********************************************************************/
    /*                                                                    */
    /*  Platform identification                                           */
    /*                                                                    */
    /*  SAP_SYSTEMID can be used to uniquely identify the platform, where */
    /*  SAP is running.                                                   */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    #define SAP_SYSTEMID 0x171




  #elif defined (SAPonApple)

    /* ################################################################## */
    /* START: old 'typemac.h'-file */
    /* ################################################################## */



    #define SAPTYPE_HF cU("Apple Macintosh with Mac OS 7.x")


    /**********************************************************************/
    /*  switches for conditional compiling                                */
    /**********************************************************************/



    /**********************************************************************/
    /*  Modifiers for exported Windows DLL functions                      */
    /**********************************************************************/
    #ifndef SAP_MODIFIERS
      #define SAP_MODIFIERS
      #define SAP_FAR
      #define SAP_PASCAL
      #define SAP_EXPORT
      #define SAP_LOADDS
      #define API_DF
      #define API_DF_C
    #endif /* SAP_MODIFIERS */


    /**********************************************************************/
    /*  classification of int, float and char representation              */
    /*  - correct classification is checked in abcglob.c                  */
    /*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
    /*            SAPwithFLOAT_IEEE/370                                   */
    /*            SAPwithCHAR_ASCII/EBCDIC                                */
    /**********************************************************************/
    #define SAPwithINT_BIGENDIAN  1
    #define SAPwithFLOAT_IEEE     1
    #define SAPwithCHAR_ASCII     1


    /**********************************************************************/
    /*                                                                    */
    /*  Platform identification                                           */
    /*                                                                    */
    /*  SAP_SYSTEMID can be used to uniquely identify the platform, where */
    /*  SAP is running.                                                   */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    #define SAP_SYSTEMID  0x402




  #elif defined (SAPonNCR)

    /* ################################################################## */
    /* START: old 'typencr.h'-file */
    /* ################################################################## */



    #define SAPTYPE_HF cU("NCR on Intel Pentium with Unix SYSV 5.4.x")


    /**********************************************************************/
    /*  switches for conditional compiling                                */
    /**********************************************************************/

    #define SAPonUNIX    1

    #define SAPonSYS5    1
    #define SAPonSYS5_4  1

    #define SAPwithX     1
    #define SAPwithSHM   1


    /**********************************************************************/
    /*  Modifiers for exported Windows DLL functions                      */
    /**********************************************************************/
    #ifndef SAP_MODIFIERS
      #define SAP_MODIFIERS
      #define SAP_FAR
      #define SAP_PASCAL
      #define SAP_EXPORT
      #define SAP_LOADDS
      #define API_DF
      #define API_DF_C
    #endif /* SAP_MODIFIERS */


    /**********************************************************************/
    /*  classification of int, float and char representation              */
    /*  - correct classification is checked in abcglob.c                  */
    /*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
    /*            SAPwithFLOAT_IEEE/370                                   */
    /*            SAPwithCHAR_ASCII/EBCDIC                                */
    /**********************************************************************/
    #define SAPwithINT_LITTLEENDIAN  1
    #define SAPwithFLOAT_IEEE     1
    #define SAPwithCHAR_ASCII     1


    /**********************************************************************/
    /*                                                                    */
    /*  Platform identification                                           */
    /*                                                                    */
    /*  SAP_SYSTEMID can be used to uniquely identify the platform, where */
    /*  SAP is running.                                                   */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    #define SAP_SYSTEMID  0x1




  #elif defined (SAPonNT)

    /* ################################################################## */
    /* START: old 'typent.h'-file */
    /* ################################################################## */

    #define SAPTYPE_HF cU("PC with Windows NT")


    /**********************************************************************/
    /*  switches for conditional compiling                                */
    /**********************************************************************/

    /* #pragma warning */
    #ifdef PRAGMA_WARNING
    #include "pw_value.h"
    #endif

    /**********************************************************************/
    /*  Modifiers for exported Windows DLL functions                      */
    /**********************************************************************/
    #ifndef SAP_MODIFIERS
    #define SAP_MODIFIERS
    #define SAP_FAR
    #define SAP_PASCAL
    #define SAP_EXPORT
    #define SAP_LOADDS

    #if defined(_OS2DAPIE_)
     #define SAP_STDCALL
    #else
     #define SAP_STDCALL _stdcall
    #endif

    #define _SYSTEM
    #define SAP_API SAP_FAR SAP_LOADDS SAP_PASCAL SAP_STDCALL
    #define API_DF
    #define API_DF_C
    #endif /* SAP_MODIFIERS */


    /**********************************************************************/
    /*  classification of int, float and char representation              */
    /*  - correct classification is checked in abcglob.c                  */
    /*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
    /*            SAPwithFLOAT_IEEE/370                                   */
    /*            SAPwithCHAR_ASCII/EBCDIC                                */
    /**********************************************************************/
    #define SAPwithINT_LITTLEENDIAN  1
    #define SAPwithFLOAT_IEEE     1
    #define SAPwithCHAR_ASCII     1

    #ifndef ECONNREFUSED
      #define ECONNREFUSED WSAECONNREFUSED
    #endif

    #ifndef ECONNRESET
      #define ECONNRESET WSAECONNRESET
    #endif

    /* portable 16 bit / 32 bit source code ..............................*/
    #define _export
    #define __export  _export
    #define _huge
    #define _near


    /**********************************************************************/
    /*                                                                    */
    /*  some UNIX C-rtl routines are not available on Windows NT and      */
    /*  were ported by SAP.                                               */
    /*                                                                    */
    /*  To avoid changes to all source code files and related makefiles   */
    /*  we decided to define prototypes for these functions here          */
    /*                                                                    */
    /*  The programmer may also include the file with the original        */
    /*  prototypes (e.g., in ntport.h and ntutil.h)                       */
    /*                                                                    */
    /**********************************************************************/

    #ifndef sleep
    int sleep (int sec);
    #endif

    #ifndef TcpipInit
    int TcpipInit (void);
    #endif

    #ifndef getopt
            int getopt(int argc, char *argv[], const char *optstring);
    #endif


    /**********************************************************************/
    /*                                                                    */
    /*  Platform identification                                           */
    /*                                                                    */
    /*  SAP_SYSTEMID can be used to uniquely identify the platform, where */
    /*  SAP is running.                                                   */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    #ifdef WIN64
      #ifdef _M_IA64
        #define SAP_SYSTEMID 0x231
      #else
        #define SAP_SYSTEMID 0x232
      #endif
      #ifndef SAPwith64_BIT
        #define SAPwith64_BIT
      #endif
    #else
      #define SAP_SYSTEMID 0x230
    #endif

    /**********************************************************************/
    /* SAPwithTHREADS: Supports threads                                   */
    /**********************************************************************/
    #define SAPwithTHREADS 1

    /**********************************************************************/
    /*                                                                    */
    /*  IA64 generally needs 16 byte alignment for the spilled floating   */
    /*  point registers in structures like jmp_buf and ucontext_t.        */
    /*                                                                    */
    /*  There's no 16 byte fundamental type on NT so we align an array.   */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    #if defined(_M_IA64) || defined(_M_AMD64)
      #define PLATFORM_MAX_T platform_max_t
      typedef __declspec(align(16)) char platform_max_t[16];
    #endif



  #elif defined (SAPonOS2_2x)

    /* ################################################################## */
    /* START: old 'typeos2.h'-file */
    /* ################################################################## */



    #define SAPTYPE_HF cU("PC with OS/2 (32 bit)")


    /**********************************************************************/
    /*  switches for conditional compiling                                */
    /**********************************************************************/


    /* casting when passing a 32 bit pointer to a 16 bit API */
    #define SAP16PTR  _Seg16
    #pragma stack16 (4096)


    /**********************************************************************/
    /*  Modifiers for exported Windows DLL functions                      */
    /**********************************************************************/
    #ifndef SAP_MODIFIERS
      #define SAP_MODIFIERS
      #define SAP_FAR
      #define SAP_PASCAL
      #define SAP_EXPORT
      #define SAP_LOADDS
      #define API_DF
      #define API_DF_C
    #endif /* SAP_MODIFIERS */


    /**********************************************************************/
    /*  classification of int, float and char representation              */
    /*  - correct classification is checked in abcglob.c                  */
    /*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
    /*            SAPwithFLOAT_IEEE/370                                   */
    /*            SAPwithCHAR_ASCII/EBCDIC                                */
    /**********************************************************************/
    #define SAPwithINT_LITTLEENDIAN  1
    #define SAPwithFLOAT_IEEE        1
    #define SAPwithCHAR_ASCII        1


    /**********************************************************************/
    /*                                                                    */
    /*  Platform identification                                           */
    /*                                                                    */
    /*  SAP_SYSTEMID can be used to uniquely identify the platform, where */
    /*  SAP is running.                                                   */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    #define SAP_SYSTEMID 0x220



  #elif defined (SAPonOS400)

    /* ################################################################## */
    /* AS/400 part                                                        */
    /* ################################################################## */


    #define SAPTYPE_HF cU("IBM AS/400 with OS/400")

	#pragma comment(copyright,"(C) Copyright SAP AG 2005-2006. All rights reserved.") 

    /**********************************************************************/
    /*  switches for conditional compiling                                */
    /**********************************************************************/

    #define SAPwithSHM 1

    #ifdef _POSIX_SOURCE
    #undef _POSIX_SOURCE
    #include <pthread.h>
    #define _POSIX_SOURCE 1
	  #else
    #include <pthread.h>
    #endif

    /**********************************************************************/
    /*  Modifiers for exported Windows DLL functions                      */
    /**********************************************************************/
    #ifndef SAP_MODIFIERS
      #define SAP_MODIFIERS
      #define SAP_FAR
      #define SAP_PASCAL
      #define SAP_EXPORT
      #define SAP_LOADDS
      #define API_DF
      #define API_DF_C
    #endif /* SAP_MODIFIERS */

    /**********************************************************************/
    /*  classification of int, float and char representation              */
    /*  - correct classification is checked in abcglob.c                  */
    /*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
    /*            SAPwithFLOAT_IEEE/370                                   */
    /*            SAPwithCHAR_ASCII/EBCDIC                                */
    /**********************************************************************/
    #define SAPwithINT_BIGENDIAN  1
    #define SAPwithFLOAT_IEEE     1

    /**********************************************************************/
    /* detect 64/32-bit compilation switch and set flags                  */
    /**********************************************************************/
    #if defined(_LONG_LONG)
      #ifndef SAPwith64_BIT
        #define SAPwith64_BIT
      #endif
    #endif

    /**********************************************************************/
    /*                                                                    */
    /*  Platform identification                                           */
    /*                                                                    */
    /*  SAP_SYSTEMID can be used to uniquely identify the platform, where */
    /*  SAP is running.                                                   */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    #ifdef SAPwithCHAR_EBCDIC
      #ifndef __LLP64_IFC__
        #define SAP_SYSTEMID 0x241          /* increased for wcharisucs2 - AF */
      #else
        #define SAP_SYSTEMID 0x261          /* EBCDIC and Teraspace - 2       */
      #endif
    #else
      #ifndef __LLP64_IFC__
        #define SAP_SYSTEMID 0x250          /* ASCII and no Teraspace - 2     */
      #else
        #define SAP_SYSTEMID 0x270          /* ASCII and Teraspace - 2        */
      #endif
    #endif

    #define AS4_BYTE_ALIGN(p, type) \
    p = (type)((unsigned char*)p + ( ((unsigned long)p % 16) ? \
        (16 - ((unsigned long)p % 16)) : 0 ))



  #elif defined (SAPonPTX)

    /* ################################################################## */
    /* START: old 'typeptx.h'-file */
    /* ################################################################## */



    #define SAPTYPE_HF cU("Sequent on INTEL CPU with Dynix/ptx 4.0")
    #define SAPTYPE_HV "4.0"


    /**********************************************************************/
    /*  switches for conditional compiling                                */
    /**********************************************************************/

    #define SAPonUNIX    1

    #define SAPonSYS5    1
    #define SAPonSYS5_4  1

    #define SAPwithX     1
    #define SAPwithSHM   1


    /**********************************************************************/
    /*  Modifiers for exported Windows DLL functions                      */
    /**********************************************************************/
    #ifndef SAP_MODIFIERS
      #define SAP_MODIFIERS
      #define SAP_FAR
      #define SAP_PASCAL
      #define SAP_EXPORT
      #define SAP_LOADDS
      #define API_DF
      #define API_DF_C
    #endif /* SAP_MODIFIERS */


    /**********************************************************************/
    /*  classification of int, float and char representation              */
    /*  - correct classification is checked in abcglob.c                  */
    /*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
    /*            SAPwithFLOAT_IEEE/370                                   */
    /*            SAPwithCHAR_ASCII/EBCDIC                                */
    /**********************************************************************/
    #define SAPwithINT_LITTLEENDIAN  1
    #define SAPwithFLOAT_IEEE        1
    #define SAPwithCHAR_ASCII        1


    /**********************************************************************/
    /*                                                                    */
    /*  Platform identification                                           */
    /*                                                                    */
    /*  SAP_SYSTEMID can be used to uniquely identify the platform, where */
    /*  SAP is running.                                                   */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    #ifdef _XOPEN_SOURCE
      #undef _XOPEN_SOURCE
    #endif
    #ifdef _INCLUDE_XOPEN_SOURCE_EXTENDED
      #undef _INCLUDE_XOPEN_SOURCE_EXTENDED
    #endif

    #define SAP_SYSTEMID  0x191


  #elif defined (SAPonRM600)

    /* ################################################################## */
    /* START: old 'typerm600.h'-file */
    /* ################################################################## */



    #define SAPTYPE_HF cU("RM600 (MIPS) with SINIX V5.4 in System V mode")


    /**********************************************************************/
    /*  switches for conditional compiling                                */
    /**********************************************************************/


    #define SAPonUNIX    1
    #define SAPonSYS5    1
    #define SAPonSYS5_2  1
    #define SAPonSYS5_4  1

    #define SAPonSINIX   1
    #define SAPonSINIX5_42   1

    #define SAPwithX     1
    #define SAPwithSHM   1

    #define _XPG_IV

    #ifdef _XOPEN_SOURCE_EXTENDED
        #define _XOPEN_SOURCE 1
    #endif

    #ifdef _XOPEN_SOURCE
        typedef unsigned char uchar;
        typedef unsigned char unchar;
        typedef unsigned short ushort;
        typedef unsigned int uint;
        typedef unsigned long ulong;
        typedef unsigned char u_char;
        typedef unsigned short u_short;
        typedef unsigned int u_int;
        typedef unsigned long u_long;
        typedef signed char s_char;

    /* sys/time.h */
    /* this is BSD stuff, so one should better #undef _XOPEN_SOURCE to get it !! */

    #ifndef _SYS_TIME_H
    #ifndef _SYS_TIMEZONE_RM600
    #define _SYS_TIMEZONE_RM600
    struct timezone {
          int     tz_minuteswest;
          int     tz_dsttime;
    };
    #endif
    #endif
    #endif /* _XOPEN_SOURCE */


    /**********************************************************************/
    /*  Modifiers for exported Windows DLL functions                      */
    /**********************************************************************/
    #ifndef SAP_MODIFIERS
      #define SAP_MODIFIERS
      #define SAP_FAR
      #define SAP_PASCAL
      #define SAP_EXPORT
      #define SAP_LOADDS
      #define API_DF
      #define API_DF_C
    #endif /* SAP_MODIFIERS */


    /**********************************************************************/
    /*  classification of int, float and char representation              */
    /*  - correct classification is checked in abcglob.c                  */
    /*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
    /*            SAPwithFLOAT_IEEE/370                                   */
    /*            SAPwithCHAR_ASCII/EBCDIC                                */
    /**********************************************************************/
    #define SAPwithINT_BIGENDIAN  1
    #define SAPwithFLOAT_IEEE     1
    #define SAPwithCHAR_ASCII     1


    /**********************************************************************/
    /* detect 64/32-bit compilation model and set according flags         */
    /* see rm600 cc manpage, -K lp64 option, LP64 data model              */
    /**********************************************************************/
    #if defined(__LP64__)
      #ifndef SAPwith64_BIT
        #define SAPwith64_BIT
      #endif
      #define SAPonRM600_64BIT
    #endif

    /**********************************************************************/
    /* set compile flag for oracle 64-bit/oci headers                     */
    /**********************************************************************/
    #ifdef SAPwith64_BIT
      #define SS_64BIT_SERVER 1
    #endif

    /**********************************************************************/
    /*                                                                    */
    /*  Platform identification                                           */
    /*                                                                    */
    /*  SAP_SYSTEMID can be used to uniquely identify the platform, where */
    /*  SAP is running.                                                   */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    #ifdef SAPwith64_BIT
      #define SAP_SYSTEMID  0x143
    #else
      #define SAP_SYSTEMID  0x142
    #endif

    /**********************************************************************/
    /*  Modification for Blended Codepage on Reliant                      */
    /**********************************************************************/
    #undef isprint
    #undef isdigit
    #undef isalpha
    #undef iscntrl
    #undef isgraph
    #undef toupper
    #undef tolower
    #undef isalnum
    #undef isspace
    #undef ispunct
    #undef isascii


  #elif defined (SAPonRS6000)

    /* ################################################################## */
    /* START: old 'typers6000.h'-file */
    /* ################################################################## */

    #if defined (SAPwithPASE400)
	    #define SAPTYPE_HF cU("IBM iSeries with OS400")
	    #ifdef SAPccQ
	    #	define __offsetof(x,y) 0
	    #endif
		#else
	    #define SAPTYPE_HF cU("IBM RS/6000 with AIX")
    #endif

    /**********************************************************************/
    /*  switches for conditional compiling                                */
    /**********************************************************************/


    #define SAPonUNIX   1
    #define SAPonSYS5   1
    #define SAPonSYS5_2 1
    #define SAPonAIX    1

    #define SAPwithX    1
    #define SAPwithSHM  1


    /**********************************************************************/
    /*  Modifiers for exported Windows DLL functions                      */
    /**********************************************************************/
    #ifndef SAP_MODIFIERS
      #define SAP_MODIFIERS
      #define SAP_FAR
      #define SAP_PASCAL
      #define SAP_EXPORT
      #define SAP_LOADDS
      #define API_DF
      #define API_DF_C
    #endif /* SAP_MODIFIERS */

    /**********************************************************************/
    /*  classification of int, float and char representation              */
    /*  - correct classification is checked in abcglob.c                  */
    /*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
    /*            SAPwithFLOAT_IEEE/370                                   */
    /*            SAPwithCHAR_ASCII/EBCDIC                                */
    /**********************************************************************/
    #define SAPwithINT_BIGENDIAN  1
    #define SAPwithFLOAT_IEEE     1
    #define SAPwithCHAR_ASCII     1


    /**********************************************************************/
    /* detect 64/32-bit compilation model and set according flags         */
    /**********************************************************************/
    #if defined(__64BIT__)
      #ifndef SAPwith64_BIT
        #define SAPwith64_BIT
      #endif
    #endif

    /**********************************************************************/
    /* set compile flag for oracle 64-bit/oci headers                     */
    /**********************************************************************/
    #ifdef SAPwith64_BIT
      #define AIXRIOS64 1
    #endif

    /**********************************************************************/
    /*                                                                    */
    /*  Platform identification                                           */
    /*                                                                    */
    /*  SAP_SYSTEMID can be used to uniquely identify the platform, where */
    /*  SAP is running.                                                   */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    #ifdef SAPwith64_BIT
      #define SAP_SYSTEMID 0x144
    #else
      #define SAP_SYSTEMID 0x140
    #endif




  #elif (defined (SAPonMVSOE) || defined (SAPonOS390))

    /* ################################################################## */
    /* START: old 'type390.h'-file                                        */
    /* ################################################################## */

    /**********************************************************************/
    /*  switches for conditional compiling                                */
    /**********************************************************************/

    #define SAPTYPE_HF cU("IBM S/390 with OS/390 OpenEdition")
    #define SAPTYPE_HV "OS390R3"

    /**********************************************************************/
    /*  switches for conditional compiling                                */
    /**********************************************************************/

    /* this was : #define SAPonRS6000 1                  */
    /* files to be changed are: pfclock.c and sapparam.c */
    #define SAPonS390 1

    /* used in many files: */
    #define SAPonUNIX   1

    /* MVS/OE is System V */
    #define SAPonSYS5   1

    /* MVS/OE is System V_2 ???? */
    /* sapcscsa.c:  */
    /*  one of the following must be set: SYS5_2, withSHM, OS2, MPE, VMS, MVS */
    /*  but: SAPonSYS5_2 and SAPwithSHM do the same thing! */
    /* saplock.c: */
    /*  determines how semaphores are implemented */
    /* sapparam.c: */
    /*  set string constant "saponsys5_2 */
    /* sapscsa.c: */
    /*  how to implement shared memory */
    /* */
    /* we need SYSTEM5_4 signal handling and waitpid() definition */
    /* so we try defining the constant */
    #define SAPonSYS5_4 1
    /* #define SAPonSYS5_2 1 */

    /* SAPonAIX is used in many files, so leave it for the moment... */
    /* all occurrences must be checked! */
    /* (HD:) removed : #define SAPonAIX    1 */
    #define SAPonMVSOE    1
    #define SAPonOS390    1

    /* OE has X-Windows support */
    #define SAPwithX 1

    /* not used in any C/H-file! */
    #define SAPwithFIFO 1

    /* occurs in sapscsa.c and sapcscsa.c: but SAPonSYS5_2 is used there*/
    #define SAPwithSHM 1

    /* (TG:) Setting _NFILE for OE */
    /* SAP code requires that _NFILE is a constant value which is known at compile */
    /*   time.  The value must be as large as the number of open files which may   */
    /*   be opened by one SAP process.  It is for example used to loop thru all    */
    /*   open files after a fork and close them.                                   */
    #include <stdio.h>
    #undef  _NFILE
    #define _NFILE 200

    /* (TG:) Including string.h for inline placement of string functions */
    /*       to avoid calls to the library                               */
    #include <string.h>

    /* (WR:) Defining signals for OE */
    #include <signal.h>
    #ifndef NSIG             /* NSIG is used to establish a signal handler for all */
    #define NSIG 33          /*   possible signals.  32 is highest value of con-   */
    #endif                   /*   secutive signals.                                */
    #ifndef SIGIOT
    #define SIGIOT SIGABRT   /* old signal name */
    #endif

    #ifndef SIGEMT
    #define SIGEMT SIGABRT   /* Emulator Trap Instruction, said to be */
                             /* extremely rare. Presumably irrelevant */
    #endif                   /* for OE */

    /* Length of nodename from utsname (missing from utsname.h), */
    /*    needed at least in tpUNIX.h                            */
    #ifndef _SYS_NMLN
    #define _SYS_NMLN 32
    #endif

    /**********************************************************************/
    /*  Modifiers for exported Windows DLL functions                      */
    /**********************************************************************/
    #ifndef SAP_MODIFIERS
    #define SAP_MODIFIERS
    #define SAP_FAR
    #define SAP_PASCAL
    #define SAP_EXPORT
    #define SAP_LOADDS
    #define API_DF
    #define API_DF_C
    #endif /* SAP_MODIFIERS */

    /**********************************************************************/
    /*  classification of int, float and char representation              */
    /*  - correct classification is checked in abcglob.c                  */
    /*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
    /*            SAPwithFLOAT_IEEE/370                                   */
    /*            SAPwithCHAR_ASCII/EBCDIC                                */
    /**********************************************************************/
    #define SAPwithINT_BIGENDIAN  1
    #ifndef SAPwithFLOAT_IEEE
      #define SAPwithFLOAT_IEEE
    #endif
    #ifdef SAPwithFLOAT_370
      #undef SAPwithFLOAT_370
      #error S/370 float format not supported with R/3
    #endif

    #if defined(SAPwithCHAR_ASCII) && !defined(SAPwithCHAR_EBCDIC)
      #ifdef __STRING_CODE_SET__
        #undef  __STRING_CODE_SET__
      #endif
      #define __STRING_CODE_SET__ ISO8859-1
    #elif !defined(SAPwithCHAR_ASCII) && defined(SAPwithCHAR_EBCDIC)
      #ifdef __STRING_CODE_SET__
        #undef __STRING_CODE_SET__
      #endif
    #else
      #error Either SAPwithCHAR_ASCII or SAPwithCHAR_EBCDIC must be defined on compiler invocation
      #if defined(SAPwithCHAR_ASCII) && defined(SAPwithCHAR_EBCDIC)
        #error You have defined  B O T H  macros
      #else
        #error You have defined  N O N E  of the macros
      #endif
    #endif

    /**********************************************************************/
    /*  Enhanced ASCII settings (Native ASCII)                            */
    /**********************************************************************/
    #if defined(__XPLINK__) && __CHARSET_LIB == 1
      #pragma runopts("FILETAG(AUTOCVT,AUTOTAG)")
      #define _ENHANCED_ASCII_EXT 0xFFFFFFFF   /* enable ASCII version of */
                                     /* add'l LE functions (APAR PQ63405) */
      #define NASCII 1               /* simple test for native ASCII build*/
      #if !defined(SAPwithCHAR_ASCII)
        #error This is an Enhanced ASCII build, but SAPwithCHAR_ASCII is not set
      #endif
    #endif

    /**********************************************************************/
    /* detect 64/32-bit compilation model and set according flags         */
    /**********************************************************************/
    #if defined(_LP64)
      #ifndef SAPwith64_BIT
        #define SAPwith64_BIT
      #endif
    #endif

    /**********************************************************************/
    /*                                                                    */
    /*  Platform identification                                           */
    /*                                                                    */
    /*  SAP_SYSTEMID can be used to uniquely identify the platform, where */
    /*  SAP is running.                                                   */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    /* was 0x140 for RS/6000  0x141 is not used for any other system right now */
    #define SAP_SYSTEMID 0x141



  #elif defined (SAPonRW320)

    /* ################################################################## */
    /* START: old 'typerw320.h'-file */
    /* ################################################################## */



    #define SAPTYPE_HF cU("RW320 (MIPS) with IRIX 4.0.5")


    /**********************************************************************/
    /*  switches for conditional compiling                                */
    /**********************************************************************/


    #define SAPonUNIX    1
    #define SAPonSYS5    1
    #define SAPonSYS5_2  1
    #define SAPonSYS5_4  1

    #define SAPonSINIX   1

    #define SAPwithX     1
    #define SAPwithSHM   1


    /**********************************************************************/
    /*  Modifiers for exported Windows DLL functions                      */
    /**********************************************************************/
    #ifndef SAP_MODIFIERS
      #define SAP_MODIFIERS
      #define SAP_FAR
      #define SAP_PASCAL
      #define SAP_EXPORT
      #define SAP_LOADDS
      #define API_DF
      #define API_DF_C
    #endif /* SAP_MODIFIERS */


    /**********************************************************************/
    /*  classification of int, float and char representation              */
    /*  - correct classification is checked in abcglob.c                  */
    /*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
    /*            SAPwithFLOAT_IEEE/370                                   */
    /*            SAPwithCHAR_ASCII/EBCDIC                                */
    /**********************************************************************/
    #define SAPwithINT_BIGENDIAN  1
    #define SAPwithFLOAT_IEEE     1
    #define SAPwithCHAR_ASCII     1


    /**********************************************************************/
    /*                                                                    */
    /*  Platform identification                                           */
    /*                                                                    */
    /*  SAP_SYSTEMID can be used to uniquely identify the platform, where */
    /*  SAP is running.                                                   */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    #define SAP_SYSTEMID 0x142

  #elif defined (SAPonSGI)

	#define SAPTYPE_HF cU("SGI (MIPS) with IRIX 6.X")


	/**********************************************************************/
	/*  switches for conditional compiling                                */
	/**********************************************************************/

	#define SAPonSGI   1

	#define SAPonUNIX    1
	#define SAPonSYS5    1
	#define SAPonSYS5_2  1
	#define SAPonSYS5_4  1

	#define SAPwithX     1
	#define SAPwithSHM   1


	/**********************************************************************/
	/*  Modifiers for exported Windows DLL functions                      */
	/**********************************************************************/
	#ifndef SAP_MODIFIERS
	#define SAP_MODIFIERS
	#define SAP_FAR
	#define SAP_PASCAL
	#define SAP_EXPORT
	#define SAP_LOADDS
	#define API_DF
	#define API_DF_C
	#endif /* SAP_MODIFIERS */


	/**********************************************************************/
	/*  classification of int, float and char representation              */
	/*  - correct classification is checked in abcglob.c                  */
	/*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
	/*            SAPwithFLOAT_IEEE/370                                   */
	/*            SAPwithCHAR_ASCII/EBCDIC                                */
	/**********************************************************************/
	#define SAPwithINT_BIGENDIAN  1
	#define SAPwithFLOAT_IEEE     1
	#define SAPwithCHAR_ASCII     1


	/**********************************************************************/
	/*                                                                    */
	/*	Platform identification                                           */
	/*                                                                    */
	/*  SAP_SYSTEMID can be used to uniquely identify the platform, where */
	/*  SAP is running.                                                   */
	/*                                                                    */
	/*  Conventions:                                                      */
	/*    see saptypeb.h                                                  */
	/*                                                                    */
	/**********************************************************************/
	#define SAP_SYSTEMID 0x144

	#include "nisgi.h"
	struct timezone {
	   int tz_minuteswest;
	   int tz_dsttime;
        };


  #elif defined (SAPonSUN)

    #include <sys/isa_defs.h>

    /* ################################################################## */
    /* START: old 'typesun.h'-file */
    /* ################################################################## */




    /* avoid name space pollution */
    #undef sun
    #undef unix
    #undef sparc

    /**********************************************************************/
    /*  switches for conditional compiling                                */
    /**********************************************************************/


    #define SAPonUNIX    1

    #define SAPonSYS5    1
    #define SAPonSYS5_4  1

    #define SAPwithX     1
    #define SAPwithSHM   1


    /**********************************************************************/
    /*  Modifiers for exported Windows DLL functions                      */
    /**********************************************************************/
    #ifndef SAP_MODIFIERS
      #define SAP_MODIFIERS
      #define SAP_FAR
      #define SAP_PASCAL
      #define SAP_EXPORT
      #define SAP_LOADDS
      #define API_DF
      #define API_DF_C
    #endif /* SAP_MODIFIERS */


    /**********************************************************************/
    /*  classification of int, float and char representation              */
    /*  - correct classification is checked in abcglob.c                  */
    /*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
    /*            SAPwithFLOAT_IEEE/370                                   */
    /*            SAPwithCHAR_ASCII/EBCDIC                                */
    /**********************************************************************/
    #if defined _BIG_ENDIAN
      #define SAPwithINT_BIGENDIAN  1
    #else
      #define SAPwithINT_LITTLEENDIAN  1
    #endif
    #define SAPwithFLOAT_IEEE     1
    #define SAPwithCHAR_ASCII     1


    /**********************************************************************/
    /* detect 64/32-bit compilation model and set according flags         */
    /* --- not yet implemented ----                                       */
    /**********************************************************************/
    #if defined _LP64
      #ifndef SAPwith64_BIT
        #define SAPwith64_BIT
      #endif
    #endif

    /**********************************************************************/
    /* set compile flag for oracle 64-bit/oci headers                     */
    /**********************************************************************/
    #ifdef SAPwith64_BIT
      #define SS_64BIT_SERVER 1
    #endif

    /**********************************************************************/
    /*                                                                    */
    /*  Platform identification                                           */
    /*                                                                    */
    /*  SAP_SYSTEMID can be used to uniquely identify the platform, where */
    /*  SAP is running.                                                   */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    #if defined __sparc || defined __sparcv9
      #ifdef SAPwith64_BIT
        #define SAPTYPE_HF cU("Solaris on SPARCV9 CPU")
        #define SAP_SYSTEMID 0x172
      #else
        #define SAPTYPE_HF cU("Solaris on SPARC CPU")
        #define SAP_SYSTEMID 0x171
      #endif
    #elif defined __i386 || defined __amd64
      #ifdef SAPwith64_BIT
        #define SAPTYPE_HF cU("Solaris on x86_64 CPU")
        #define SAP_SYSTEMID 0x174
      #else
        #define SAPTYPE_HF cU("Solaris on x86 CPU")
        #define SAP_SYSTEMID 0x173
      #endif
    #endif


  #elif defined (SAPonLIN)

    #if defined __i386__
        #define SAPTYPE_HF cU("Intel x86 with Linux")
        #define SAP_SYSTEMID 0x183
        #define SAPwithINT_LITTLEENDIAN  1
    #elif defined __ia64__
        #define SAPTYPE_HF cU("Intel ia64 with Linux")
        #define SAP_SYSTEMID 0x184
        #define SAPwithINT_LITTLEENDIAN  1
        #define SAPwith64_BIT
    #elif defined __s390x__
        #define SAPTYPE_HF cU("IBM zSeries with Linux")
        #define SAP_SYSTEMID 0x185
        #define SAPwithINT_BIGENDIAN     1
        #define SAPwith64_BIT
    #elif defined __x86_64__
        #define SAPTYPE_HF cU("AMD/Intel x86_64 with Linux")
        #define SAP_SYSTEMID 0x186
        #define SAPwithINT_LITTLEENDIAN  1
        #define SAPwith64_BIT
    #elif defined __PPC64__
        #define SAPTYPE_HF cU("IBM pSeries with Linux")
        #define SAP_SYSTEMID 0x187
        #define SAPwithINT_BIGENDIAN     1
        #define SAPwith64_BIT
    #else
        #define SAPTYPE_HF cU("New architecture with Linux")
        #define SAP_SYSTEMID 0x18F	/* this can be changed !! */
        #if #machine(bigendian)
           #define SAPwithINT_BIGENDIAN     1
        #else
           #define SAPwithINT_LITTLEENDIAN  1
        #endif
        #if defined(__LP64__)
           #define SAPwith64_BIT
        #endif
        #define OPEN_MAX 256
    #endif


    /**********************************************************************/
    /*  switches for conditional compiling                                */
    /**********************************************************************/

    #define SAPonLINUX   1
    #define SAPonUNIX    1
    #define SAPonSYS5    1
    #define SAPonSYS5_4  1

    #define SAPwithX     1
    #define SAPwithSHM   1


    /**********************************************************************/
    /*  Modifiers for exported Windows DLL functions                      */
    /**********************************************************************/
    #ifndef SAP_MODIFIERS
    #define SAP_MODIFIERS
    #define SAP_FAR
    #define SAP_PASCAL
    #define SAP_EXPORT
    #define SAP_LOADDS
    #define API_DF
    #define API_DF_C
    #endif /* SAP_MODIFIERS */

    /**********************************************************************/
    /*  classification of int, float and char representation              */
    /*  - correct classification is checked in abcglob.c                  */
    /*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
    /*            SAPwithFLOAT_IEEE/370                                   */
    /*            SAPwithCHAR_ASCII/EBCDIC                                */
    /**********************************************************************/
    #define SAPwithFLOAT_IEEE        1
    #define SAPwithCHAR_ASCII        1

    /*
     * Terrible hack to avoid clashes
     */
    #ifndef _FEATURES_H
      #ifdef _POSIX_SOURCE
        #undef _POSIX_SOURCE
      #endif
      #ifdef _XOPEN_SOURCE
        #undef _XOPEN_SOURCE
      #endif
      #ifdef _XOPEN_SOURCE_EXTENDED
        #undef _XOPEN_SOURCE_EXTENDED
      #endif
      #undef FD_SETSIZE
    #endif

    typedef double SAP_DOUBLE __attribute__ ((aligned (8)));
    #define SAP_DOUBLE_MIN  DBL_MIN
    #define SAP_DOUBLE_MAX  DBL_MAX
    #define SAP_DOUBLE_DEFINED

    #define _NFILE 256 /* for sapdaem.c */
    #define TF_ACKNOW TCP_NODELAY

    /**********************************************************************/
    /*                                                                    */
    /*  IA64 generally needs 16 byte alignment for the spilled floating   */
    /*  point registers in structures like jmp_buf and ucontext_t.        */
    /*                                                                    */
    /*  Intel's IA64 ABI specifies 'long double' to be 16 byte for LP64.  */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    #if defined(__ia64__)
      #define PLATFORM_MAX_T platform_max_t
      typedef long double platform_max_t;
    #else
      #define PLATFORM_MAX_T SAP_DOUBLE
    #endif

  #elif defined (SAPonVMS)

    /* ################################################################## */
    /* START: old 'typevms.h'-file */
    /* ################################################################## */

    #ifdef __alpha
    #define SAPTYPE_HF cU("DEC Alpha with OpenVMS")
    #else
    #define SAPTYPE_HF cU("DEC VAX with OpenVMS")
    #endif
    /*--------------------------------------------------------------------*/
    /*  Switches of conditional compiling                                 */
    /*--------------------------------------------------------------------*/
    /*  Note: Only those switches don't do any harm that are not used.    */
    /*--------------------------------------------------------------------*/

    #define SAPonOpenVMS

    #ifdef __alpha
    #define SAPTYPE_HV "6.2"
    #define SAPonAXP
    #else
    #define SAPTYPE_HV "6.2"
    #define SAPonVAX
    #endif

    #define SAPwithSockets
    #define SAPwithX         /* version 10 or 11 */
    #define SAPwithFIFO
    #define SAPwithMSG

    #define SAPwithCC
    #define SAPwithANSI_PARAMS
    #define SAPwithANSI_VARARGS 	     /* use stdarg.h */
    #define SAPwithANSI_VOID

    #define SAPwithANSI_CONST
    #define SAPwithANSI_VOLATILE


    /**********************************************************************/
    /*  Modifiers for exported Windows DLL functions                      */
    /**********************************************************************/
    #ifndef SAP_MODIFIERS
    #define SAP_MODIFIERS
    #define SAP_FAR
    #define SAP_PASCAL
    #define SAP_EXPORT
    #define SAP_LOADDS
    #define API_DF
    #define API_DF_C
    #endif /* SAP_MODIFIERS */


    /**********************************************************************/
    /*  classification of int, float and char representation              */
    /*  - correct classification is checked in abcglob.c                  */
    /*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
    /*            SAPwithFLOAT_IEEE/370                                   */
    /*            SAPwithCHAR_ASCII/EBCDIC                                */
    /**********************************************************************/

    #define SAPwithINT_LITTLEENDIAN
    #ifdef __ALPHA
    /* on alpha: SAP uses IEEE float but oracle uses G_float */
    #define SAPwithFLOAT_IEEE
    #define ORAwithFLOAT_VMSGFLOAT
    #endif
    #ifdef __VAX
    #define SAPwithFLOAT_VMSGFLOAT
    #endif

    /**/
    /*--------------------------------------------------------------------*/
    /*  machine independent simple data types                             */
    /*--------------------------------------------------------------------*/

    typedef	short		key_t;	    /* Semaphore or shm key           */

    /*--------------------------------------------------------------------*/
    /* For ORACLE data access.                                            */

    #define TWO_BYTES_TO_USHORT(addr_twobytes) \
            (unsigned short)  \
            (((SAP_BYTE)((addr_twobytes)[0]) << 8) \
            | (SAP_BYTE)((addr_twobytes)[1]))

    /*--------------------------------------------------------------------*/
    /*  For nicer declarations                                            */
    /*--------------------------------------------------------------------*/

    #define PARAMS(pp)  pp              /* Declare procedures like:       */
    				    /* int proc1 PARAMS((char*,int)); */
    				    /* But:                           */
    				    /* This C-compiler does not like  */
    				    /* paramter lists in procedure    */
    				    /* declarations.                  */
    #define NOPARAMS (void)             /* For procedures without params. */



    /**
     ** VMS++  Porting Release 1.0   Peter Simon  , 24-MAR-1992 15:29:47.58
     **/

    #define MAXNAMLEN 40

    /*
    ** ++VMS 13-JUL-93 Christian Ihrig
    **
    ** some defines for the good old caddr_t - Problem in in.h and xlib.h
    */

    #if !defined (CADDR_T) && !defined(__CADDR_T)
    typedef char *caddr_t;
    #endif
    #ifndef CADDR_T
    #define CADDR_T
    #endif
    #ifndef __CADDR_T
    #define __CADDR_T
    #endif

    /*
    ** ++VMS 19-Mar-91 DEC Markus Schuetz
    **
    **	Macro to replace UNIX unlink() system call
    */
    #define unlink(p1)  remove((p1))

    /**
     ** VMS++  Porting Release 1.1
     ** Werner Rehm , 16-SEP-1992 17:03:43.45
     ** Makro to define strerror to vmsstrerror,
     ** because strerror(EVMSERR) return NULL -> access violation
     **/

    #define strerror vmsstrerror

    char    *vmsstrdup (char *src);
    char    *strdup (char *src);
    #define strdup	vmsstrdup

    #define accept	    vms_accept
    #define	connect	    vms_connect
    #define	listen	    vms_listen
    #define select	    vms_select
    #define send	    vms_send

    int vms_set_sendmode  ( int sd, int mode );
    int vms_get_recquota  ( int sd );
    int vms_get_sendquota ( int sd );
    int vms_get_nbflag    ( int sd );

    /***++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++***/

    /**** solange bis es im types.h drin ist... ****/
    /****         ( geklaut von HPUX )          ****/

    /* Types, macros, etc. for select() */

    #  ifndef MAXFUPLIM
    /*
     * MAXFUPLIM is the absolute limit of open files per process.  No process,
     * even super-user processes may increase u.u_maxof beyond MAXFUPLIM.
     * MAXFUPLIM means maximum files upper limit.
     * Important Note:  This definition should actually go into h/param.h, but
     * since it is needed by the select() macros which follow, it had to go here.
     * I did not put it in both files since h/param.h includes this file and that
     * would be error prone anyway.
     */
    #    define MAXFUPLIM       2048

    /*
     * These macros are used for select().  select() uses bit masks of file
     * descriptors in longs.  These macros manipulate such bit fields (the
     * file sysrem macros uses chars).  FD_SETSIZE may be defined by the user,
     * but must be >= u.u_highestfd + 1.  Since we know the absolute limit on
     * number of per process open files is 2048, we need to define FD_SETSIZE
     * to be large enough to accomodate this many file descriptors.  Unless the
     * user has this many files opened, he should redefine FD_SETSIZE to a
     * smaller number.
     */
    #    ifndef _KERNEL
    #      ifndef FD_SETSIZE
    #        define FD_SETSIZE MAXFUPLIM
    #      endif
    #    else /* not _KERNEL */
    #      define FD_SETSIZE MAXFUPLIM
    #    endif /* _KERNEL */

         typedef long fd_mask;

    #    define NFDBITS (sizeof(fd_mask) * 8)     /* 8 bits per byte */

    #    ifndef howmany
    #      define howmany(x,y)  (((x)+((y)-1))/(y))
    #    endif

         typedef struct fd_set {
           fd_mask fds_bits[howmany(FD_SETSIZE, NFDBITS)];
         } fd_set;

    #    define FD_SET(n,p)  ((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
    #    define FD_CLR(n,p) ((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
    #    define FD_ISSET(n,p) ((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))

    #    ifndef _KERNEL
    #        define FD_ZERO(p)     memset((char *)(p), (char) 0, sizeof(*(p)))
    #    endif /* not _KERNEL */
    #   endif /* not MAXFUPLIM */

    /***------------------------------------------------------------------------***/

    /**
     ** VMS++  Porting Release 1.1
     ** Werner Rehm ,  4-AUG-1992 09:41:45.90
     ** Makro to define P_tmpdir which is defines in <stdio.h> for UNIX
     **/

    #define  P_tmpdir "SYS$SCRATCH:"

    /**
     ** VMS++  Porting Release 1.1
     ** Christian Ihrig 25-AUG-1992
     **
     ** Macro to replace UNIX gmtime() system call with localtime(),
     ** because VMS don't supports the GMT time and so localtime()
     ** ends with the same results as gmtime() under UNIX
     **
     **/

    #define gmtime localtime

    #include <stat.h>
    #include <unixlib.h>
    #include <unixio.h>

    typedef unsigned long 	VMSPid;

    /* oft implicit function da in UNIX keine header Datei benoetigt wird */

    int sleep ( unsigned __secs);
    int gethostname ( char *__name , int __len );
    int putenv( char *__log );

    /**********************************************************************/
    /*                                                                    */
    /*  Platform identification                                           */
    /*                                                                    */
    /*  SAP_SYSTEMID can be used to uniquely identify the platform, where */
    /*  SAP is running.                                                   */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/

    #ifdef __alpha
    #define SAP_SYSTEMID 0x0320
    #else
    #define SAP_SYSTEMID 0x0310
    #endif



  #elif defined (SAPonWINDOWS)

    /* ################################################################## */
    /* START: old 'typewin.h'-file */
    /* ################################################################## */



    #define SAPTYPE_HF cU("PC with WINDOWS")


    /**********************************************************************/
    /*  switches for conditional compiling                                */
    /**********************************************************************/


    /* in future only standard ANSI C will be supported !!! */
    #ifndef NO_STDC
      # define __STDC__
    #endif

    #define WINDOWS                  /* for NOVELL TCPIP API */
    #define WSOCKETS_DLL             /* for MS TCPIP API */
    #define WINDLL_EXPORT FAR PASCAL


    /**********************************************************************/
    /*   Modifiers for exported Windows DLL functions                     */
    /**********************************************************************/
    #ifndef SAP_MODIFIERS
      #define SAP_MODIFIERS
      #define SAP_FAR __far
      #define SAP_PASCAL __pascal
      #define SAP_EXPORT __export

      #ifdef NO_SAP_LOADDS
        #define SAP_LOADDS
      #else
        #define SAP_LOADDS __loadds
      #endif

      #define API_DF  SAP_FAR SAP_LOADDS SAP_PASCAL
      #define API_DF_C  SAP_FAR SAP_LOADDS
    #endif /* SAP_MODIFIERS */


    /**********************************************************************/
    /*  classification of int, float and char representation              */
    /*  - correct classification is checked in abcglob.c                  */
    /*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
    /*            SAPwithFLOAT_IEEE/370                                   */
    /*            SAPwithCHAR_ASCII/EBCDIC                                */
    /**********************************************************************/
    #define SAPwithINT_LITTLEENDIAN  1
    #define SAPwithFLOAT_IEEE        1
    #define SAPwithCHAR_ASCII        1


    /**********************************************************************/
    /*                                                                    */
    /*  Platform identification                                           */
    /*                                                                    */
    /*  SAP_SYSTEMID can be used to uniquely identify the platform, where */
    /*  SAP is running.                                                   */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    #define SAP_SYSTEMID 0x220

  #elif defined (SAPonDARW)



    /**********************************************************************/
    /*  switches for conditional compiling                                */
    /**********************************************************************/

    #define SAPonDARWIN   1
    #define SAPonUNIX     1
    #define SAPonBSD   1

    #define SAPwithSHM       1

    /**********************************************************************/
    /*  Modifiers for exported Windows DLL functions                      */
    /**********************************************************************/
    #ifndef SAP_MODIFIERS
        #define SAP_MODIFIERS
        #define SAP_FAR
        #define SAP_PASCAL
        #define SAP_EXPORT
        #define SAP_LOADDS
        #define API_DF
        #define API_DF_C
    #endif /* SAP_MODIFIERS */

    /**********************************************************************/
    /*  classification of int, float and char representation              */
    /*  - correct classification is checked in abcglob.c                  */
    /*  - values: SAPwithINT_BIGENDIAN/LITTLEENDIAN                       */
    /*            SAPwithFLOAT_IEEE/370                                   */
    /*            SAPwithCHAR_ASCII/EBCDIC                                */
    /**********************************************************************/
	#if defined __i386__
        #define SAPTYPE_HF cU("Apple Macintosh with Mac OS X (Darwin) INTEL")
        #define SAPwithINT_LITTLEENDIAN  1
	#else
		#define SAPTYPE_HF cU("Apple Macintosh with Mac OS X (Darwin) PPC")
		#define SAPwithINT_BIGENDIAN  1
	#endif
    #define SAPwithFLOAT_IEEE     1
    #define SAPwithCHAR_ASCII     1


    #define _NFILE 256 /* for sapdaem.c */

    /**********************************************************************/
    /*                                                                    */
    /*  Platform identification                                           */
    /*                                                                    */
    /*  SAP_SYSTEMID can be used to uniquely identify the platform, where */
    /*  SAP is running.                                                   */
    /*                                                                    */
    /*  Conventions:                                                      */
    /*    see saptypeb.h                                                  */
    /*                                                                    */
    /**********************************************************************/
    #define SAP_SYSTEMID 0x401

  #else
    #error Please set a SAPon<PLATFORM> switch for saptype.h
  #endif

#endif  /* SAPTYPE_H */

#ifdef __cplusplus
}
#endif

/**********************************************************************/
/*                                                                    */
/*  saptypeb.h:  platform independent defines                         */
/*                                                                    */
/*  If SAP_SET_PLATFORM is set:                                       */
/*    saptypeb.h is not included                                      */
/*    SAP_SET_PLATFORM will be reset                                  */
/*                                                                    */
/*  If SAP_SET_PLATFORM is not set:                                   */
/*    saptypeb.h is included                                          */
/*                                                                    */
/*  Usage:                                                            */
/*    #define SAP_SET_PLATFORM                                        */
/*    #include "saptype.h"       /o without saptypeb.h        o/      */
/*                                                                    */
/*    #include "curses.h"        /o conflicts with saptypeb.h o/      */
/*                               /o e.g. MIN, MAX             o/      */
/*                                                                    */
/*    #include "saptypeb.h"                                           */
/*                                                                    */
/**********************************************************************/
#ifndef SAP_SET_PLATFORM
    #include "saptypeb.h"

	#if defined(SAPonOS400) || defined(SAPwithPASE400)
	  #include "o4global.h"
	#endif

    #ifdef SAPonWINDOWS

      #ifdef WIN_FPRINTF

          #include "wnport.h"

          #undef fprintf
          #define fprintf WINfprintf

          #undef vfprintf
          #define vfprintf WINvfprintf

          #undef fwrite
          #define fwrite WINfwrite

          #undef putc
          #define putc WINputc

          #undef stderr
          #define stderr NULL

      #endif  /* WIN_FPRINTF */

    #endif /* SAPonWINDOWS */

#else

    #undef SAP_SET_PLATFORM

#endif

