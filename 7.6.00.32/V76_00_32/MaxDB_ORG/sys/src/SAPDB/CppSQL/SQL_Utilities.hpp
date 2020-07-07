/*!
 * \file    SQL_Utilities.hpp
 * \author  PeterG (GoertzP)
 * \brief   C++ SQL Class: Utilities
 */

/*

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



*/


#ifndef SQL_UTILITIES_HPP
#define SQL_UTILITIES_HPP


/************************************************************************/
/* Includes                                                             */
/************************************************************************/

#include "livecachetypes.h" /* no check */


/************************************************************************/
/* Declaration of several helper functions                              */
/************************************************************************/

externC DECLSPEC_EXPORT
unsigned int dbpStrLen( const DbpTypeUnicode * s );

externC DECLSPEC_EXPORT
DbpTypeUnicode * dbpStrCat( DbpTypeUnicode * dest,
                            const DbpTypeUnicode * src );

externC DECLSPEC_EXPORT
DbpTypeUnicode * dbpStrnCat( DbpTypeUnicode * dest,
                             const DbpTypeUnicode * src,
                             unsigned int maxlen );

externC DECLSPEC_EXPORT
DbpTypeUnicode * dbpStrChr( const DbpTypeUnicode * s,
                            const DbpTypeUnicode c );

externC DECLSPEC_EXPORT
DbpTypeUnicode * dbpStrnChr( const DbpTypeUnicode * s,
                             const DbpTypeUnicode c,
                             unsigned int maxlen );

/* PTS 1111387 strcmp or strncmp doesn't return pointer to string !
externC DECLSPEC_EXPORT
DbpTypeUnicode * dbpStrCmp( DbpTypeUnicode * dest,
                            const DbpTypeUnicode *src );
externC DECLSPEC_EXPORT
DbpTypeUnicode * dbpStrnCmp( DbpTypeUnicode * dest,
                             const DbpTypeUnicode * src,
                             unsigned int maxlen );
*/

externC DECLSPEC_EXPORT
int dbpStrCmp( DbpTypeUnicode * dest, const DbpTypeUnicode *src );

externC DECLSPEC_EXPORT
int dbpStrnCmp( DbpTypeUnicode * dest, const DbpTypeUnicode * src,
                unsigned int maxlen );

externC DECLSPEC_EXPORT
DbpTypeUnicode * dbpStrCpy( DbpTypeUnicode * dest,
                            const DbpTypeUnicode * src );

externC DECLSPEC_EXPORT
DbpTypeUnicode * dbpStrnCpy( DbpTypeUnicode * dest,
                             const DbpTypeUnicode * src,
                             unsigned int maxlen );

externC DECLSPEC_EXPORT
DbpTypeUnicode * dbpStrrChr( const DbpTypeUnicode * s,
                             const DbpTypeUnicode c );

externC DECLSPEC_EXPORT
DbpTypeUnicode * dbpStrnrChr( const DbpTypeUnicode * s,
                              const DbpTypeUnicode c,
                              unsigned int maxlen );


#endif /* SQL_UTILITIES_HPP */

