/*!
  @defgroup       XML global files used for XML features   
*/
/*!
  @ingroup        XML
  @file           XML_UTF8.h
  @author         FerdiF
  @brief          basis functions for handling XML UTF-8 character strings
                  ( see also: http://www.w3.org/TR/REC-xml )
  @see            
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


#ifndef XML_UTF8_H
#define XML_UTF8_H

#include "SAPDB/SAPDBCommon/SAPDB_Types.h"

/*! type for UCS2 chars */
typedef unsigned int  XML_UnicodeCharCode;      

/*!
   @brief          Converts Unicode character code into UTF-8 character 
                   and returns the length of the UTF-8 character in bytes
   @param          dest [in/out] ptr to UTF-8 character buffer, on return 
                                 ptr to first pos after generated UTF-8    
                                 character
   @param          code [in]     Unicode character code
   @return         size of UTF-8 char in bytes

 */
SAPDB_Int XML_UTF8CharFromCode(SAPDB_UTF8 *dest, XML_UnicodeCharCode code);

/*!
   @brief          Converts UTF-8 character to its Unicode character code
   @param          text [in] ptr to UTF-8 character
   @return         Unicode character code
 */
XML_UnicodeCharCode XML_UTF8CharToCode( const SAPDB_UTF8 *text );

/*!
   @brief          Copy next UTF-8 character and return length of UTF-8 
                   character
   @param          dest [out] ptr to buffer for the copied UTF-8 character
   @param          src [in] ptr to UTF-8 character string
   @return         size of UTF-8 character in bytes
 */
SAPDB_Int XML_UTF8GetChar(SAPDB_UTF8 *dest, const SAPDB_UTF8 *src);

/*!
   @brief          Checks if character code is a &lt;BaseChar> as defined in XML
   @param          c [in] character code
   @return         SAPDB_FALSE or SAPDB_TRUE
 */
SAPDB_Bool XML_UTF8IsBaseChar( XML_UnicodeCharCode c );

/*!
   @brief          Checks if character code is a &lt;CombiningChar> as defined 
                   in XML
   @param          c [in] character code
   @return         SAPDB_FALSE or SAPDB_TRUE
 */
SAPDB_Bool XML_UTF8IsCombiningChar(XML_UnicodeCharCode c);

/*!
   @brief          Checks if character code is a &lt;Digit> as defined in XML
   @param          c [in] character code
   @return         SAPDB_FALSE or SAPDB_TRUE
 */
SAPDB_Bool XML_UTF8IsDigit( XML_UnicodeCharCode c );

/*!
   @brief          Checks if character code is a &lt;Extender> as defined in XML
   @param          c [in] character code
   @return         SAPDB_FALSE or SAPDB_TRUE
 */
SAPDB_Bool XML_UTF8IsExtender( XML_UnicodeCharCode c );

/*!
   @brief          Checks if character code is aa &lt;Ideographic> as defined in XML
   @param          c [in] character code
   @return         SAPDB_FALSE or SAPDB_TRUE
 */
SAPDB_Bool XML_UTF8IsIdeographic(XML_UnicodeCharCode c);

/*!
   @brief          Checks if character code is aa &lt;Letter> as defined in XML
   @param          c [in] character code
   @return         SAPDB_FALSE or SAPDB_TRUE
 */
SAPDB_Bool XML_UTF8IsLetter(XML_UnicodeCharCode c);

/*!
   @brief          Checks if character code is a &lt;NameChar> as defined in XML
   @param          c [in] character code
   @return         SAPDB_FALSE or SAPDB_TRUE
 */
SAPDB_Bool XML_UTF8IsNameChar(XML_UnicodeCharCode c);

/*!
   @brief          calculates length of next UTF-8 character
   @param          text [in] ptr to UTF-8 character string
   @return         length in bytes of next UTF-8 character
 */
SAPDB_Int XML_UTF8SingleCharLength ( const SAPDB_UTF8 *text );

/*!
   @brief          skips WhiteSpaceCharacters within an UTF-8 character string
                   and returns position of first NON Whitespace char
   @param          p [in] ptr to UTF-8 character string
   @return         ptr to first NON Whitespace character
                   or to 0 terminated end of string
 */
SAPDB_UTF8* XML_UTF8SkipWhiteSpaces( SAPDB_UTF8 *p );

/*!
   @brief          normalizes value strings and returns position of the first
                   char
   @param          p [in] ptr to UTF-8 character string
   @return         ptr to first character
                   or to 0 terminated end of string
 */
SAPDB_UTF8* XML_UTF8NormalizeValue( SAPDB_UTF8 *p );

#endif /* XML_UTF8_H */
