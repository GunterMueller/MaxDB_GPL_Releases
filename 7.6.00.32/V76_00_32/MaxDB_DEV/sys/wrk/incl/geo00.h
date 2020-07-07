
/*!
  @file           geo00.h
   @author         DanielD
   @special area   RTE | Basic Declarations
   @brief          Basic Declarations
   @see            http://pwww.bea.sap-ag.de/Entwicklung/cpp/C-Style/commonDeclarations.html

\if EMIT_LICENCE

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


//
//   FreeBSD portions added by Kai Mosebach,
//   For more informations see : http://www.komadev.de/sapdb
//

\endif
 */



#ifndef GEO00_H
#define GEO00_H

#include <string.h>
#include <stdarg.h>

#ifndef SAPDB_USE_REUSESDK
#include "SAPDBCommon/SAPDB_Types.h"
#else
#include "SAPDBCommon/SAPDB_Types-sdk.h" /* nocheck */
#endif

/*
 * compiler specific stuff
 */
#if defined(WIN32)
#define NEEDED_WIN32_WINNT 0x0500

#if !defined(_WIN32_WINNT)
# define _WIN32_WINNT  NEEDED_WIN32_WINNT
#else
# if _WIN32_WINNT < NEEDED_WIN32_WINNT
#  undef _WIN32_WINNT
#  define _WIN32_WINNT  NEEDED_WIN32_WINNT
# endif
#endif


# include <malloc.h>
#if defined (RTE_USE_SOCK)
# include <winsock2.h>
#endif
# include <windows.h>
#else
# if defined(AIX)
#   include <malloc.h>
#   include <stdlib.h>
# else
#if !defined(NMP) && !defined(FREEBSD)
#   include <alloca.h>
#endif
# endif
#endif

#ifdef FREEBSD
#include <stdbool.h>
#endif

#ifndef RTE_COMPILERFEATURES_H
/* compatibility with old geo00.h */
#undef externC
#ifndef SAPDB_USE_REUSESDK
#include "RunTime/RTE_CompilerFeatures.h"
#else
#include "RunTime/RTE_CompilerFeatures-sdk.h"  /* nocheck */
#endif
#endif

/* end compiler specific stuff */

#define undef_ceo00 (-1)

#if defined (__cplusplus)

#define global          extern "C"
#if !defined(AIX) || __IBMCPP__ >= 500
/* WIN32, SUN, HPUX, DEC/OSF1, SNI/NMP, LINUX, FREEBSD */
#define TYPENAME_MEO00  typename
#define EXPLICIT_MEO00  explicit
#else
#define TYPENAME_MEO00
#define EXPLICIT_MEO00
#endif
#define REFCAST_MEO00(type_) *(type_ *)

#if !defined(HPUX) && !defined(AIX) && !defined(LINUX) && !defined(FREEBSD)
#define PLACEMENT_DELETE_MEO00
#endif

#if !defined(HPUX)
#define TEMPLATE_CLASS_MEO00
#define TEMPLATE_TYPE_MEO00
#else
#define TEMPLATE_CLASS_MEO00    class
#define TEMPLATE_TYPE_MEO00 typename
#endif

inline int feo00_MemCmp (const void * p1, const void * p2, int len)
{
    return memcmp (p1, p2, len);   /* this depends on the c library */
                                   /* currently, all platforms      */
                                   /* compare as unsigned           */
}

#else

#define global
#endif

#define MEMCMP_EO00(p1, p2, len)  memcmp (p1, p2, len)
#define MIN_EO00(v1, v2) (((v1) < (v2)) ? (v1) : (v2))
#define MAX_EO00(v1, v2) (((v1) > (v2)) ? (v1) : (v2))

#define ALIGN_EO00(v1, alignment) ((((v1) + (alignment) - 1)/(alignment))*(alignment))
#define ALIGN_2BYTE_EO00(v1)      ( ( ( (v1) + 1 ) / 2 ) * 2 )
#define ALIGN_4BYTE_EO00(v1)      ( ( ( (v1) + 3 ) / 4 ) * 4 )
#define ALIGN_8BYTE_EO00(v1)      ( ( ( (v1) + 7 ) / 8 ) * 8 )

#define C_STRING_ZERO_BYTE_LEN_EO00 1


/* portable integer types */
typedef unsigned char             teo00_Bool;
typedef unsigned char             teo00_Byte;
typedef signed   char             teo00_Int1;
typedef unsigned char             teo00_Uint1;
typedef signed   short            teo00_Int2;
typedef unsigned short            teo00_Uint2;
typedef signed   int              teo00_Int4;
typedef unsigned int              teo00_Uint4;


#if defined (_WIN32) || defined (_WIN64)
# if (defined (_INTEGRAL_MAX_BITS) && _INTEGRAL_MAX_BITS >= 64) || defined (_WIN64)
   typedef signed __int64         teo00_Int8;
   typedef unsigned __int64       teo00_Uint8;
# else
#  error __int64 type not supported
# endif
#else
# if defined (BIT64)
   typedef signed long            teo00_Int8;
   typedef unsigned long          teo00_Uint8;
# else
   typedef signed long long       teo00_Int8;
   typedef unsigned long long     teo00_Uint8;
# endif
#endif


#if defined (_WIN64) || defined (_WIN32)
   typedef signed   int           teo00_Int;
   typedef unsigned int           teo00_Uint;
#else
   typedef signed   long          teo00_Int;
   typedef unsigned long          teo00_Uint;
#endif

#if defined (_WIN64)
   typedef signed   __int64		  teo00_Longint;
   typedef unsigned __int64       teo00_Longuint;
#else
   typedef signed   long          teo00_Longint;
   typedef unsigned long          teo00_Longuint;
#endif

#if defined (_WIN32)
#if defined (RTE_USE_SOCK)
   typedef SOCKET            	  teo00_Socket;
#endif
#else
   typedef int               	  teo00_Socket;
#endif

#endif

