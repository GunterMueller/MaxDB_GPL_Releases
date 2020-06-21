/*================================================================
    ========== licence begin  GPL
    Copyright (c) 2005 SAP AG

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

*/

#ifndef CONFIG_H
#define CONFIG_H

#define VERSION  "\"expat_1.95.8\""

#define XML_NS 1
#define XML_DTD 1
#define XML_CONTEXT_BYTES 1024

#if defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN

#include <memory.h>
#include <string.h>

/* we will assume all Windows platforms are little endian */
#define XML_BYTE_ORDER 1234

/* Windows has memmove() available. */
#define HAVE_MEMMOVE

#else

#include <stdlib.h>

#if defined(ALPHA) | defined(I386)
#define XML_BYTE_ORDER 12
#else
#define XML_BYTE_ORDER 21
#endif

#if defined(WHAT_SO_EVER)
/* Define to empty if the keyword does not work.  */
#undef const

/* Define if you have a working `mmap' system call.  */
#undef HAVE_MMAP

/* Define to `long' if <sys/types.h> doesn't define.  */
#undef off_t

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
#undef size_t

/* Define if you have the ANSI C header files.  */
#undef STDC_HEADERS

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
#undef WORDS_BIGENDIAN

/* Define if you have the bcopy function.  */
#undef HAVE_BCOPY

/* Define if you have the getpagesize function.  */
#undef HAVE_GETPAGESIZE

/* Define if you have the memmove function.  */
#undef HAVE_MEMMOVE

/* Define if you have the <fcntl.h> header file.  */
#undef HAVE_FCNTL_H

/* Define if you have the <unistd.h> header file.  */
#undef HAVE_UNISTD_H

#define XML_NS
#define XML_DTD

#ifdef WORDS_BIGENDIAN
#define XML_BYTE_ORDER 21
#else
#define XML_BYTE_ORDER 12
#endif

#define XML_CONTEXT_BYTES 1024

#ifndef HAVE_MEMMOVE
#ifdef HAVE_BCOPY
#define memmove(d,s,l) bcopy((s),(d),(l))
#else
#define memmove(d,s,l) ;punting on memmove;
#endif

#endif
#endif

#endif

#endif /* CONFIG_H */
