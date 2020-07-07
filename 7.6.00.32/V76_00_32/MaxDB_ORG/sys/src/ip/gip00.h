/*!***************************************************************************

  module      : gip00.h

  -------------------------------------------------------------------------

  responsible : BurkhardD

  special area: General Header for OS specific definitions

  description : Platform and Operatingsystem independed macros and definition.
                Compiler and Linker names Environment variables and Switches


  Author: Burkhard Diesing
  last changed: 2000-08-09  12:33 Marco Paskamp  AIX-C Compiler changed
  see also    :

  -------------------------------------------------------------------------

  copyright:    Copyright (c) 1999-2005 SAP AG




    ========== licence begin  GPL
    Copyright (c) 1999-2005 SAP AG

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


*****************************************************************************/

/* Compiler an Linker definitions */

#if defined (WIN32)

/* Maxim L. (Intel): select Intel compiler if used for build */
#ifdef __INTEL_COMPILER
#define COMPILER_IP00 "icl"
#define LINKER_IP00   "xilink"
#else
#define COMPILER_IP00 "cl"
#define LINKER_IP00   "link"
#endif /* __INTEL_COMPILER */
#define CPP_COMPILER_IP00 COMPILER_IP00
#define CPP_LINKER_IP00 LINKER_IP00
#endif

#if defined (_IBMR2) && defined (_LONG_LONG)
#define COMPILER_IP00 "xlc"
#define LINKER_IP00   "xlc_r"
#define CPP_COMPILER_IP00 "xlC"
#define CPP_LINKER_IP00 "xlC_r"
#endif

#if defined OSF1
#define COMPILER_IP00 "cc"
#define CPP_COMPILER_IP00 "cxx"
#define LINKER_IP00   COMPILER_IP00
#define CPP_LINKER_IP00 CPP_COMPILER_IP00
#endif

#if defined HP9 || defined PA11 || defined PA20W || defined HP_IA64
#define COMPILER_IP00 "cc"
#define CPP_COMPILER_IP00 "aCC"
#define LINKER_IP00   COMPILER_IP00
#define CPP_LINKER_IP00 CPP_COMPILER_IP00
#endif

#if defined (LINUX) || defined (FREEBSD)
/* Maxim L. (Intel): select Intel compiler if used for build */
#ifdef __INTEL_COMPILER
#define CPP_COMPILER_IP00 "icpc"
#else
#define CPP_COMPILER_IP00 "c++"
#endif /* __INTEL_COMPILER */
#endif

#ifndef COMPILER_IP00
#ifdef __INTEL_COMPILER
#define COMPILER_IP00 "icc"
#else
#define COMPILER_IP00 "cc"
#endif /* __INTEL_COMPILER */
#endif
#ifndef LINKER_IP00
#define LINKER_IP00   COMPILER_IP00
#endif
#ifndef CPP_COMPILER_IP00
#define CPP_COMPILER_IP00 "CC"
#endif
#ifndef CPP_LINKER_IP00
#define CPP_LINKER_IP00 CPP_COMPILER_IP00
#endif

/* Compiler and linker switches */


/* Directorys and Path specifications */
#  define EXEPATH_IP00 "pgm"
#  define LIBPATH_IP00 "lib"
#if defined (WIN32)
#  define SHLPATH_IP00 "pgm"
#else
#  define SHLPATH_IP00 "lib"
#endif

/* PTS 1104821, PTS 1118161 */
#ifndef SHL64PATH_IP00
#  if defined BIT64 && ! defined OSF1 && !(defined LINUX &&  (defined IA64 || defined S390X || defined SDBonPPC64)) && !defined HP_IA64
#    define EXE64PATH_IP00 "pgm/pgm64"
#    if defined WIN32
#      define SHL64PATH_IP00 "pgm64"
#      define LIB64PATH_IP00 "lib\\lib64"
#    else
#      define SHL64PATH_IP00 "lib64"
#      define LIB64PATH_IP00 "lib/lib64"
#    endif
#  else
#    define EXE64PATH_IP00 "pgm"
#    define LIB64PATH_IP00 "lib"
#    define SHL64PATH_IP00 ""
#  endif
#endif


/* Filename and directory extensions */
#if (defined WIN32)
#define CEXT_IP00   ".c"
#define CPPEXT_IP00 ".cpp"
#define OEXT_IP00   ".obj"
#define ARCEXT_IP00 ".lib"
#define EEXT_IP00   ".exe"
#define DLLEXT_IP00 ".dll"
#define PATHSEP_IP00 "\\"
#else
#define CEXT_IP00   ".c"
#define CPPEXT_IP00 ".C"
#define OEXT_IP00   ".o"
#define ARCEXT_IP00 ".a"
#define EEXT_IP00   ""
#if (defined HPUX) && !(defined HP_IA64)
#define DLLEXT_IP00 ".sl"
#else
#define DLLEXT_IP00 ".so"
#endif
#define PATHSEP_IP00 "/"
#endif


/* Environment Variables */
#if (defined WIN32)
#define LDLIBPATH_IP00 "PATH"
#define ENVSEP_IP00 ";"
#define SHELL_IP00 getenv("COMSPEC")
#else

#if defined OSF1 || defined SUN || defined SOLARIS || defined LINUX || defined FREEBSD
#define LDLIBPATH_IP00 "LD_LIBRARY_PATH"
#endif

/* PTS 1112264 */
#if defined HPUX
#if defined BIT64
#define LDLIBPATH_IP00 "LD_LIBRARY_PATH"
#else
#define LDLIBPATH_IP00 "SHLIB_PATH"
#endif
#endif

#ifndef LDLIBPATH_IP00
#define LDLIBPATH_IP00 "LIBPATH"
#endif

#define ENVSEP_IP00 ":"
#define SHELL_IP00 "/bin/sh")
#endif

