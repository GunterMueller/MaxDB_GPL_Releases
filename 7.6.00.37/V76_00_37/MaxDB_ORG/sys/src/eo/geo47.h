/*!
  @file           geo47.h
  @author         JoergM
  @special area   UNICODE and other string conversions
  @brief          Defines data types and definitions for unicode handling
              This is currently reduced to what is needed for ggg90 and
              vgg90.cpp, but it is expandable...
              The whole stuff makes it compatible with "TCHAR.H"...
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



\endif
*/



#ifndef GEO47_H
#define GEO47_H

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#if defined(__cplusplus) && defined ( USE_STL )
//
// --- Standard Template Library (STL)
//
# include <vector>
# include <string>

# if defined (CPP_STD_IOSTREAM) || defined (_WIN32)
#  include <iostream>
# else
#  include <iostream.h>
# endif

#endif /* __cplusplus */


#if defined(WIN32)
# include <TCHAR.H>
#else
/*
 * Not WIN32 environment does not know what thchar.h is....
 */
# ifdef UNICODE
#  include <wchar.h>
# else /* !UNICODE */
#  include <ctype.h>
#  include <stdio.h>
#  include <strings.h>
# endif

#endif

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/

#if defined(_WIN32) /*------- _WIN32 -------------------*/


#else  /*--------------------- UNIX --------------------*/

# ifdef UNICODE

# if defined(__cplusplus)
extern "C" {
# endif
 
#ifndef __TCHAR
typedef wchar_t       _TCHAR;
typedef wchar_t       TCHAR;
#define __TCHAR
#endif /* __TCHAR */

#  define _TEOF       WEOF
 
#  define __T(x)      L ## x
 
#  define _tcslen     wstrlen
#  define _itot       _itow
#  define _totupper   towupper
 
#  if defined(__cplusplus)
}
#  endif
 
# else /* !UNICODE */
 
#  if defined(__cplusplus)
extern "C" {
#  endif
 
#ifndef __TCHAR
typedef char          _TCHAR;
typedef char          TCHAR;
#define __TCHAR
#endif /* __TCHAR */
 
#  define _TEOF       EOF
 
#  define __T(x)      x
 
#  define _tcslen     strlen
#  define _itot       sql47_MS_itoa
#  define _ttoi       atoi
#  define _totupper   toupper
#  define _tcscpy     strcpy
#  define _tcsncpy    strncpy
#  define _tcscmp     strcmp
#  define _tcsncmp    strncmp
#  define _tcsicmp    strcasecmp
#  define _tcsnicmp   strncasecmp
#  define _tcschr     strchr

#  define _stprintf    sprintf
#  define _vstprintf   vsprintf

#  if defined(__cplusplus)
}
#  endif
 
# endif /* !UNICODE */
 
# define _T(x)        __T(x)
 
#endif /*----------------- _WIN32/UNIX -----------------*/



#if defined(__cplusplus) && defined ( USE_STL )
//
// --- Using Standard Template Library (STL)
//
#if defined(_UNICODE)
# define _tcout       wcout
# define _tcin        wcin
# define _tcerr       wcerr 
# define _tclog       wclog 
#else     
# define _tcout       cout 
# define _tcin        cin  
# define _tcerr       cerr 
# define _tclog       clog
#endif

#endif /* __cplusplus */


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

#if defined(__cplusplus) && defined ( USE_STL )
//
// --- Using Standard Template Library (STL)
//
#if defined(CPP_NAMESPACE) || defined (_WIN32)
 using namespace std;
#endif

#if defined(_UNICODE)
  typedef wstring   _tString;
  typedef wostream  _tOstream;
  typedef wistream  _tIstream;
#else
  typedef string    _tString;
  typedef ostream   _tOstream;
  typedef istream   _tIstream;
#endif

typedef _tString::value_type _tStrChar;

#if defined (SUN) || defined (SOLARIS) || defined (_IBMR2)
  typedef vector<_tString, allocator> _tStrVector;
#else
  typedef vector<_tString>            _tStrVector;
#endif

typedef _tStrVector::iterator _tStrVectorIter;

#endif /* __cplusplus */

/*===========================================================================*
 *  FUNCTION PROTOTYPES                                                      *
 *===========================================================================*/

# if defined(__cplusplus)
extern "C" {
# endif

#if !defined(WIN32) && !defined(UNICODE)
/*!
   @despription    Microsoft itoa compatible number to ascii conversion
   @param          n [in] the number to be converted
   @param          buffer [out] the buffer for the result (must have enough space!!!)
   @param          radix [in] the radix to use (2,4,8,10,16 other default to 10)
   @return         always returns buffer [no error return!!!]

 */

char *sql47_MS_itoa(int n, char *buffer, int radix);

/*!
   @despription    Integer to ascii conversion

    Converts a given number into an decimal digits string.
    If this function is called with buflen = 0, only the length of the buffer needed to convert the number
    is returned but no conversion takes place. In that case the buffer is not needed and NULL can
    be supplied.
 
   @param          n [in] the number to be converted
   @param          buffer [out] the buffer for the result (must have enough space or NULL (if buflen = 0)!!!)  
   @param          buflen [in] the maximum buffer length to be used including ZERO byte or 0
   @return         length of converted/to be converted string (excluding ZERO byte) or a value < 0 if the buflen
                   is to small to hold the string

 */

  int sql47_itoa(int n, char *buffer, int buflen);
#endif

#  if defined(__cplusplus)
}
#  endif

/*===========================================================================*
 *  EXTERNAL VARIABLES                                                       *
 *===========================================================================*/



#endif  /* GEO47_H */
