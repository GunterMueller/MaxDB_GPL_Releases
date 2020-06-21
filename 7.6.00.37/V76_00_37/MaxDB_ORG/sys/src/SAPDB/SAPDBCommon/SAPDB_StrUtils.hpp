/*!========================================================================

  @file         SAPDB_StrUtils.hpp
  @ingroup      
  @author       RaymondR

  @brief        String utilities 

                Some string functions like lowercase string comparison.

  @since        2003-10-21  17:15
  @sa           

  ==========================================================================

  \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 2003-2005 SAP AG

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
 ============================================================================*/


#ifndef SAPDB_STRUTILS_HPP
#define SAPDB_STRUTILS_HPP

/*===========================================================================*
 *  INCLUDES                                                                 *
 *===========================================================================*/

#include <ctype.h>

#include    "SAPDBCommon/SAPDB_Types.hpp"

/*===========================================================================*
 *  DEFINES                                                                  *
 *===========================================================================*/


/*===========================================================================*
 *  MACROS                                                                   *
 *===========================================================================*/


/*===========================================================================*
 *  CLASSES, STRUCTURES, TYPES, UNIONS ...                                   *
 *===========================================================================*/

/*!
  @brief    Compare strings.

  Compare two strings, ignore case.

  @param    str1           [in]  - First string.
  @param    str2           [in]  - Second string.
  @param    pLen           [in]  - if not null, length to compare

  @return   true indicates that str1 is identical to str2 else false.
*/
inline SAPDB_Bool SAPDB_StringsIdentical( SAPDB_Char const * str1, SAPDB_Char const  * str2, SAPDB_UInt4 * const pLen = 0 )
{
    int c1, c2;

    do 
    { 
        c1 = tolower(*(str1++) );
        c2 = tolower(*(str2++) );
        if (( 0 != pLen ) && ( 0 == --(*pLen) )) break;
    }  
    while ( c1 && c1 == c2 );

    return c1 == c2;
}

/*!
  @brief    Compare strings.

  Compare two strings, ignore case.

  @param    str1           [in]  - First string.
  @param    str2           [in]  - Second string.
  @param    len            [in]  - length to compare

  @return   true indicates that str1 is identical to str2 else false.
*/
inline SAPDB_Bool SAPDB_StringsIdentical( SAPDB_Char const * str1, SAPDB_Char const  * str2, SAPDB_UInt4 len )
{
    return SAPDB_StringsIdentical( str1, str2, &len );
}

/*---------------------------------------------------------------------------*/

/*!
  @brief    Find a substring in a string.

  Find a substring in a string from left to right, ignore case..

  @param    str            [in]   - string.
  @param    substr         [in]   - Substring to be located. 

  @return   A pointer to the first occurrence of 'substr' 
  in string, or NULL if 'substr' is not found.
*/
inline SAPDB_Char * const SAPDB_FindStr( SAPDB_Char * const str, SAPDB_Char const * const substr )
{
    SAPDB_Char *       currStr = reinterpret_cast<SAPDB_Char*>(str);
    SAPDB_Char *       tmpstr;
    SAPDB_Char const * tmpsubstr;

    if ( URI_NULL_CHAR == substr[0] )
        return  str;

    while ( currStr[0] )
    {
        tmpstr = currStr;
        tmpsubstr = static_cast<SAPDB_Char const * >(substr);

        while ( tmpstr[0] && tmpsubstr[0] && !(tolower(tmpstr[0]) - tolower(tmpsubstr[0])) )
            tmpstr++, tmpsubstr++;

        if (URI_NULL_CHAR == tmpsubstr[0])
            return currStr;

        currStr++;
    }

    return 0;
}


#endif  /* SAPDB_STRUTILS_HPP */
