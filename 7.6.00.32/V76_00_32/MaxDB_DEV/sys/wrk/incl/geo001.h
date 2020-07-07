/*!
  @file           geo001.h
  @author         RaymondR
  @special area   RTE - system specific types, defines ...
  @brief
  @see

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



#ifndef GEO001_H
#define GEO001_H



/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined(_WIN32)

#define NEEDED_WIN32_WINNT 0x0500

#if !defined(_WIN32_WINNT)
# define _WIN32_WINNT  NEEDED_WIN32_WINNT
#else
# if _WIN32_WINNT < NEEDED_WIN32_WINNT
#  undef _WIN32_WINNT
#  define _WIN32_WINNT  NEEDED_WIN32_WINNT
# endif
#endif

# include <windows.h>
#else /* UNIX */
# include <errno.h>
# include <unistd.h>
#endif


/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined(_WIN32)
# define NO_ERROR_EO001                   NO_ERROR
# define ERROR_FILE_NOT_FOUND_EO001       ERROR_FILE_NOT_FOUND
# define ERROR_PATH_NOT_FOUND_EO001       ERROR_PATH_NOT_FOUND
# define ERROR_ACCESS_DENIED_EO001        ERROR_ACCESS_DENIED
# define ERROR_NOT_ENOUGH_MEMORY_EO001    ERROR_NOT_ENOUGH_MEMORY
# define ERROR_OUTOFMEMORY_EO001          ERROR_OUTOFMEMORY
#else /* UNIX */
# define NO_ERROR_EO001                   0
# define ERROR_FILE_NOT_FOUND_EO001       ENOENT
# define ERROR_PATH_NOT_FOUND_EO001       ENOENT
# define ERROR_ACCESS_DENIED_EO001        EACCES
# define ERROR_NOT_ENOUGH_MEMORY_EO001    ENOMEM
# define ERROR_OUTOFMEMORY_EO001          ENOMEM
#endif


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!!!teo001_Pid should not be used anymore. Use RTE_PID instead (cf. RTE_Types.h) */

#if defined(_WIN32) || defined(WIN32)
 typedef unsigned long   teo001_SystemRc;
 typedef DWORD           teo001_Pid;
#else
 typedef int             teo001_SystemRc;
#if defined (LINUX) || (defined (FREEBSD) && defined (LINUXTHREADS) && !defined FREEBSD_THREADS)
 /* special case... pid_t must not be used in typedef */
 /* FreeBSD might be compiled using linuxthread implementation, so we rely on linux here */
 typedef int             teo001_Pid;
#else
 typedef pid_t           teo001_Pid;
#endif
#endif

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/


/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO001_H */
