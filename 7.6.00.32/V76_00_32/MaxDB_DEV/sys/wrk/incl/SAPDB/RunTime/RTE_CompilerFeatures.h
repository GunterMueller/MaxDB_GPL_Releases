/*!**************************************************************************

  module       : RTE_CompilerFeatures.h

  -------------------------------------------------------------------------

  responsible : JoergM

  special area: Compiler Specific Features
  description : Defines for conditional compilation

  Some Compilers have compiuler specific feature, that forces conditional compilation.
  This file tries to collect these feature to prevent at least compiler specific 
  conditional compiling in other modules.

  last changed: 2000-12-04  11:16
  first created:2000-12-04  10:08

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 2000-2005 SAP AG




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

#   FreeBSD portions added by Kai Mosebach, 
#   For more informations see : http://www.komadev.de/sapdb
#

    
*****************************************************************************/


#ifndef RTE_COMPILERFEATURES_H
#define RTE_COMPILERFEATURES_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined (HP)
#  define signed
#endif

#if (defined (AIX) && __IBMCPP__ < 500) || ( (defined (SUN) || defined (SOLARIS)) && !defined(_BOOL) ) || !defined(__cplusplus)
#  define HAS_NO_BOOL_TYPE
#endif

#if defined (_MSC_VER) || (defined (AIX) && __IBMCPP__ < 500) || defined (DEC) || defined (HP) || defined (SNI)

/*
  MS VC++ and others have problems in operator []
  converting index parameter to int when
  operator [] const exists
 */
#  define HAS_BUGGY_OVERLOADING_CONVERSIONS

#endif

#if defined (_MSC_VER)

/* disables storage class warning in extern "C" functions for VC 6.0 [DD] */
#  pragma warning (disable: 4042)

#endif

#if defined(OSF1) && defined(__cplusplus)

/* disables warning about missing constructure for structures containing const elements */
#  pragma message disable noctobutconrefm

#endif

#if defined (__cplusplus)

#  define externPascal              extern "C"
#  define externC                   extern "C"
#  define externCpp
#  define REFERENCE_CAST(TYPE_)     *(TYPE_ *)
#  define VAR_ARRAY_REF	&
#  define VAR_VALUE_REF	&

#if !defined(AIX) || __IBMCPP__ >= 500
/* WIN32, SUN, SOLARIS, HPUX, DEC/OSF1, SNI/NMP, LINUX, FREEBSD */
#  define CONST_CAST(TYPE_,EXPR_) const_cast< TYPE_ >(EXPR_)
#  define REINTERPRET_CAST(TYPE_,EXPR_) reinterpret_cast< TYPE_ >(EXPR_)
#  define STATIC_CAST(TYPE_,EXPR_) static_cast< TYPE_ >(EXPR_)
#else
#  define CONST_CAST(TYPE_,EXPR_) ((TYPE_)(EXPR_))
#  define REINTERPRET_CAST(TYPE_,EXPR_) ((TYPE_)(EXPR_))
#  define STATIC_CAST(TYPE_,EXPR_) ((TYPE_)(EXPR_))
/*
 * These keywords are not supported by the AIX compiler (C++ Standard of 1992 ...)
 * If you are carefull, you are able to ignore them sometimes...
 */
#  define explicit
#  define using
#  define namespace struct
#  define mutable
#  define typename
#endif

#if !defined(HPUX) && !defined(AIX) && !defined(LINUX) && !defined(FREEBSD)
#  define HAS_PLACEMENT_DELETE
#endif

#if !defined(HPUX)
#  define TEMPLATE_CLASS
#  define TEMPLATE_TYPE
#else
#  define TEMPLATE_CLASS class
#  define TEMPLATE_TYPE  typename
#endif

#else

#  define externPascal
#  define externC
#  define externCpp
#  define VAR_ARRAY_REF
#  define VAR_VALUE_REF *

#endif 

#if defined(LINUX) || defined (FREEBSD)
// 2001-07-26 The linux compiler warns about that the contructor is private
//            and there is no friend defined.
#define CONSTRUCTOR_MAY_NOT_BE_PRIVATE_AND_NO_FRIEND_DEFINED 1
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/

/*!
   @brief computes the offset of a struct or class member
 */

#define RTE_OFFSET_OF(T,M) (((char*) &((T*) 0)->M) - (char*) 0)

/*===========================================================================*
 *  STRUCTURES, TYPES, UNIONS ...                                            *
 *===========================================================================*/


/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

/* For alloca support */
#if defined(_WIN32)

#    include <malloc.h>

#else

#    if defined(AIX)

#        include <malloc.h>
#        include <stdlib.h>

#elif !defined(NMP) && !defined(FREEBSD)

#        include <alloca.h>

#endif

#endif

#endif  /* RTE_COMPILERFEATURES_H */
