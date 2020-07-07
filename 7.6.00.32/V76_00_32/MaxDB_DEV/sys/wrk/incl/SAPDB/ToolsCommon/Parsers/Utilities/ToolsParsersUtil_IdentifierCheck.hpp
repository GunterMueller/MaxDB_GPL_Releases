/*!
    @file    ToolsParsersUtil_IdentifierCheck.hpp
    @author  BerndV
    @brief   Class to analyze parameter values

    \if EMIT_LICENCE

    ========== licence begin  GPL
    Copyright (c) 1998-2005 SAP AG

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

#ifndef _ToolsParsersUtil_IdentifierCheck_HPP_
#define _ToolsParsersUtil_IdentifierCheck_HPP_

/*
  -----------------------------------------------------------------------------
  includes
  -----------------------------------------------------------------------------
 */

/*! @brief Class to analyze parameter values which are "identifiers" */
class ToolsParsersUtil_IdentifierCheck
{
public:
  /*! @brief  checks whether it is a "special identifier"

              A "special identifier" is a sequence of characters which is enclosed
              in double quotes. This function checks only the double quotes at the 
              first and the last position of the string.

     @param   szString [IN] zero terminated string

     @return  true if it is a "special identifier"
   */
  static bool IsSpecialIdentifier
    ( const char * szString );
  /*! @brief  checks whether it is a "special identifier"
      @see    IsVerySimpleIdentifier 
   */
  static bool IsSpecialIdentifier
      ( const unsigned char * szString ){return IsSpecialIdentifier((const char *) szString);}

  /*! @brief   checks whether it is a "very simple identifier"

              A "very simple identifier" is a non empty string. It starts with a 
              letter ('A...Z','a...z') or a special character ('#','@','$'). 
              After the first charcater a "very simple identifier" has letters 
              ('A...Z','a...z'), special characters ('#','@','$','_') or digits ('0...9').

     @param   szString [IN] zero terminated ASCII string

     @return  true if it is a "very simple identifier"
   */
  static bool IsVerySimpleIdentifier
    ( const char * szString ) {return IsVerySimpleIdentifier((const unsigned char *) szString);}
  /*! @brief   checks whether it is a "very simple identifier"
      @see    IsVerySimpleIdentifier 
   */
  static bool IsVerySimpleIdentifier
      ( const unsigned char * szString );

  /*! @brief   checks whether it is a "simple identifier"

              A "simple identifier" is a non empty string. It starts with a 
              letter ('A...Z','a...z'), a special character ('#','@','$') or 
              a "language specific character" (like german umlauts). 
              After the first charcater  a "simple identifier" has letters 
              ('A...Z','a...z'), special characters ('#','@','$','_'),a "language 
              specific character" (like german umlauts) or digits ('0...9').
              The function uses the same "extended language letters" like the 
              MaxDB kernel.

     @param   szString [IN] zero terminated string

     @return  true if it is a "simple identifier"
   */
  static bool IsSimpleIdentifier
      ( const char * szString ) {return IsSimpleIdentifier((const unsigned char *) szString);}
  /*! @brief   checks whether it is a "simple identifier"
      @see    IsSimpleIdentifier 
   */
  static bool IsSimpleIdentifier
      ( const unsigned char * szString );

  /*! @brief  makes a string uppercase 

              This function uses the the same lowercase/uppercase table 
              like the MaxDB kernel for "simple identifiers" and guarantees 
              the same conversion like the MaxDB kernel.

      @param  szString [INOUT] zero terminated string
   */
  static void KernelToupper
      ( char * szString ) {return KernelToupper((unsigned char *) szString);}
  /*! @brief  makes a string uppercase 
      @see    KernelToupper 
   */
  static void KernelToupper
      ( unsigned char * szString );

  /*! @brief  makes a string lowercase 

              This function uses the the same lowercase/uppercase table 
              like the MaxDB kernel for "simple identifiers" and guarantees 
              the same conversion like the MaxDB kernel.

      @param  szString [INOUT] zero terminated ASCII string
   */
  static void KernelTolower
      ( char * szString ) {return KernelTolower((unsigned char *) szString);}
  /*! @brief   makes a string lowercase 
      @see    KernelTolower 
   */
  static void KernelTolower
      ( unsigned char * szString );

  /*! @brief  makes a string uppercase with the C runtime function

      @param  szString [INOUT] zero terminated string
   */
  static void CRuntimeToupper
      ( char * szString ) {return CRuntimeToupper((unsigned char *) szString);}
  /*! @brief  makes a string uppercase with the C runtime function
      @see    CRuntimeToupper 
   */
  static void CRuntimeToupper
      ( unsigned char * szString );

  /*! @brief  makes a string lowercase with the C runtime function

      @param  szString [INOUT] zero terminated string
   */
  static void CRuntimeTolower
      ( char * szString ) {return CRuntimeTolower((unsigned char *) szString);}
  /*! @brief  makes a string lowercase with the C runtime function
      @see    CRuntimeTolower
   */
  static void CRuntimeTolower
      ( unsigned char * szString );

private:
   /* @brief private identifier check */
   static bool IsIdentifier
      ( const unsigned char * szString,
        const bool            oFirst  [],
        const bool            oTail   [] );

  static unsigned char oTolower           [256];
  static unsigned char oToupper           [256];

  static bool          oSimpleFirst       [256];
  static bool          oSimpleTail        [256];

  static bool          oVerySimpleFirst   [256];
  static bool          oVerySimpleTail    [256];
}; // end class ToolsParsersUtil_IdentifierCheck

#endif // _ToolsParsersUtil_IdentifierCheck_HPP_
