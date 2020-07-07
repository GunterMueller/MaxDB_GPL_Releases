/*!
  @file    SAPDB_ToString.hpp
  @author  JoergM, FrankH
  @ingroup Common
  @brief   Value to String conversion i.e. for argument passing

  This class supports a set of constructors to create a string from argument.
  The number formats are named as those known from C++ IOStreams. For ease of
  use some macro definition are given, that allows 'smaller' argument line.
  This macros do not have the common 'SAPDB_' prefix, otherwise, they wouldn't
  be 'small' anymore. I.e. thne format specifier have to be used with prefix SAPDB_ToStringClass::

  @li noformat   = 0x0000,
  @li //  internal   = 0x0001, // pad between sign and value (currently not supported)
  @li left       = 0x0002, // pad after value
  @li right      = 0x0004, // pad before value (default)
  @li boolalpha  = 0x0008, // use symbolic representation of true and false
  @li dec        = 0x0010, // integer base: base 10 output (decimal) (default)
  @li oct        = 0x0020, // base 8 output (octal)
  @li hex        = 0x0040, // base 16 output (hexadecimal)
  @li showbase   = 0x0080, // on output prefix oct by 0 and hex by 0x
  @li showpoint  = 0x0100, // print leading zeros
  @li uppercase  = 0x0200, // 'E','X' rather than 'e','x'
  @li showpos    = 0x0400, // explicit '+' for positiv ints
  @li scientific = 0x0800, // floating-point notation: d.dddddddEdd
  @li fixed      = 0x1000, // ddd.dd
  @li //  skipws     = 0x2000, // to skip leading white space (currently not supported)
  @li ptr        = 0x8000  // Pointer

  Following default conversion formats are used

  SAPDB_Int1, SAPDB_Int2, SAPDB_Int4, SAPDB_Int8, SAPDB_Long
  SAPDB_UInt1, SAPDB_UInt2, SAPDB_UInt4, SAPDB_UInt8, SAPDB_ULong -> SAPDB_ToString::dec

  Number in hexadecimal representation show only as much digits as they consist of.

  example: ToStr((SAPDB_Int2)-1, SAPDB_ToString::hex) -> "0xffff" !

  @li SAPDB_Char * -> no modification (passed)

  @li SAPDB_Real4,SAPDB_Real8 -> SAPDB_ToString::noformat (_T_g) Default precision for floating point
  numbers is 6 digits.

  @li Any pointer (not SAPDB_Char *) -> SAPDB_ToString::ptr, SAPDB_ToString::showbase, SAPDB_ToString::hex

  To specifier Outputput of an string pointer as pointer use
  
  SAPDB_ToString( stringPtr, SAPDB_ToString::ptr, SAPDB_ToString::showbase, SAPDB_ToString::hex )

  ToStr() can be used instead of the ctor SAPDB_ToStringClass()

  Since this is too long, a short form can be used

    ToStr( stringPtr, _T_p )

  -------------------------------------------------------------------------

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
*****************************************************************************/

#ifndef SAPDB_TOSTRING_HPP
#define SAPDB_TOSTRING_HPP

//! maximum size of a number in character presentation
#define MAX_CHARACTER_ALLOWED_IN_PRESENTATION 15

#include "SAPDBCommon/SAPDB_Types.hpp"
/*!
  @brief Format specifier macro definitions

  This is the list of predefined format specifier macro definitions
@li _T_b bool as text 'true' or 'false'
@li _T_p pointer as hexadecimal number uppercase with prefix '0X'
@li _T_d decimal number
@li _T_h hexadecimal number using lowercase for a-f without prefix '0x' 
@li _T_H hexadecimal number using uppercase for A-F without prefix '0x' 
@li _T_x hexadecimal number using lowercase for a-f with prefix '0x' 
@li _T_X hexadecimal number using uppercase for A-F with prefix '0x' 
@li _T_o octal number with prefix '0'
@li _T_e real value in scientific format, exponent 'e' lowercase
@li _T_f real value in fixed format
@li _T_g real value in scientifc or fixed format whatever suits with lowercase exponent 'e'
@li _T_z add leading zeros

  @note Handling leading zero is not possible without a width specification.
  _T_z must be ignored if no width is given and is always used in combination with another format.

  example
  
  ToStr((SAPDB_UInt4)12, 6, _T_z, _T_h) would produce      "00000c"
  ToStr((SAPDB_UInt4)12, _T_h)          would produce      "c"
  ToStr((SAPDB_UInt4)12, _T_z, _T_h)    would also produce "c"
  
 */
#define _T_b SAPDB_ToStringClass::boolalpha //!< bool as text
#define _T_d SAPDB_ToStringClass::dec //!< decimal number
#define _T_h SAPDB_ToStringClass::hex //!< hexadecimal number without base
#define _T_H SAPDB_ToStringClass::hex,SAPDB_ToStringClass::uppercase //!< hexadecimal number without base
#define _T_x SAPDB_ToStringClass::hex,SAPDB_ToStringClass::showbase  //!< hexadecimal number with base
#define _T_X SAPDB_ToStringClass::hex,SAPDB_ToStringClass::uppercase,SAPDB_ToStringClass::showbase  //!< hexadecimal number with base
#define _T_o SAPDB_ToStringClass::oct,SAPDB_ToStringClass::showbase  //!< octal number
#define _T_e SAPDB_ToStringClass::scientific  //!< scientific notation
#define _T_f SAPDB_ToStringClass::fixed  //!< fixed number
#define _T_g SAPDB_ToStringClass::noformat //!< variable fixed or scientific notation
#define _T_p SAPDB_ToStringClass::ptr,SAPDB_ToStringClass::hex,SAPDB_ToStringClass::showbase //!< pointer
#define _T_z SAPDB_ToStringClass::showpoint //!< leading zeros

#define ToStr SAPDB_ToString //!< For simplification of coding only...

/*!
  @class SAPDB_ToStringClass
  @brief string class for message output
 */
class SAPDB_ToStringClass
{
private:
    SAPDB_Char m_PresentationBuffer[MAX_CHARACTER_ALLOWED_IN_PRESENTATION+1]; //!< internal only used buffer for value representation
    const SAPDB_Char *m_OutputString ; //!< pointer to actually used output string
          SAPDB_Char *m_Buf          ; //!< pointer to actually used buffer

public:
    //!< formatting flags (names almost identical with C++ standard stream buffer formatting flags)
    enum fmtflags
    {
            noformat   = 0x0000, //!< no specific format
         //  internal   = 0x0001, // pad between sign and value (not supported)
            left       = 0x0002, //!< pad after value
            right      = 0x0004, //!< pad before value (default)
            boolalpha  = 0x0008, //!< use symbolic representation of true and false
            dec        = 0x0010, //!< integer base: base 10 output (decimal) (default)
            oct        = 0x0020, //!< base 8 output (octal)
            hex        = 0x0040, //!< base 16 output (hexadecimal)
            showbase   = 0x0080, //!< on output prefix oct by 0 and hex by 0x
            showpoint  = 0x0100, //!< print leading zeros
            uppercase  = 0x0200, //!< 'E','X' rather than 'e','x'
            showpos    = 0x0400, //!< explicit '+' for positiv ints
            scientific = 0x0800, //!< floating-point notation: d.dddddddEdd
            fixed      = 0x1000, //!< ddd.dd
         //  skipws     = 0x2000, // to skip leading white space (not supported)
            ptr        = 0x8000  //!< Pointer
    } ;

    /*!
    @brief ctor for simple string

    No conversion needed

    @param String [in] the string
    */
    explicit SAPDB_ToStringClass(const SAPDB_Char *String)
    { m_Buf = 0; m_OutputString = String ; }

    /*!
    @brief ctor for simple string

    No conversion needed

    @param String [in] the string
    */
    explicit SAPDB_ToStringClass(SAPDB_Char *String)
    { FillStringFormatBuffer( (const SAPDB_Char *)String, 0, 0, noformat ); }

    /*!
    @brief ctor for simple string

    No conversion needed

    @param String [in] the string
    */
    explicit SAPDB_ToStringClass(const SAPDB_UTF8 *String)
    { m_Buf = 0; m_OutputString = (const SAPDB_Char *)String ; }

    /*!
    @brief ctor for simple string

    No conversion needed

    @param String [in] the string
    */
    explicit SAPDB_ToStringClass(SAPDB_UTF8 *String)
    { FillStringFormatBuffer( (const SAPDB_Char *)String, 0, 0, noformat ); }

    /*!-------------------------------------------------------------------------
    @brief ctor for simple string with format flags

    No conversion needed

    @param String [in] the string
    @param fmtflags [in] ignored
    */
    SAPDB_ToStringClass(const SAPDB_Char *String, const fmtflags )
    { m_Buf = 0; m_OutputString = String ; }

    /*!-------------------------------------------------------------------------
    @brief ctor for simple string with format flags

    No conversion needed

    @param String [in] the string
    @param fmtflags [in] ignored
    */
    SAPDB_ToStringClass(SAPDB_Char *String, const fmtflags )
    { FillStringFormatBuffer( (const SAPDB_Char *)String, 0, 0, noformat ); }

    /*!-------------------------------------------------------------------------
    @brief ctor for simple string with format flags

    No conversion needed

    @param String [in] the string
    @param fmtflags [in] ignored
    */
    SAPDB_ToStringClass(const SAPDB_UTF8 *String, const fmtflags )
    { m_Buf = 0; m_OutputString = (const SAPDB_Char *)String ; }

    /*!-------------------------------------------------------------------------
    @brief ctor for simple string with format flags

    No conversion needed

    @param String [in] the string
    @param fmtflags [in] ignored
    */
    SAPDB_ToStringClass(SAPDB_UTF8 *String, const fmtflags )
    { FillStringFormatBuffer( (const SAPDB_Char *)String, 0, 0, noformat ); }

    /*!-------------------------------------------------------------------------
    @brief ctor for simple string with format flags and width specification

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)

    @param String [in] the string
    @param Width [in] output string minimum width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    */
    SAPDB_ToStringClass( const SAPDB_Char *String,
                         const SAPDB_UInt2 Width,
                         const fmtflags Flag1=noformat,
                         const fmtflags Flag2=noformat)
    {   FillStringFormatBuffer( String, Width, 0, Flag1 | Flag2 ); }

    /*!-------------------------------------------------------------------------
    @brief ctor for simple string with format flags and width specification

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)

    @param String [in] the string
    @param Width [in] output string minimum width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    */
    SAPDB_ToStringClass( const SAPDB_UTF8 *String,
                         const SAPDB_UInt2 Width,
                         const fmtflags Flag1=noformat,
                         const fmtflags Flag2=noformat)
    {   FillStringFormatBuffer( (const SAPDB_Char *)String, Width, 0, Flag1 | Flag2 ); }

    /*!-------------------------------------------------------------------------
    @brief ctor for simple string with format flags and width and precision specification

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)

    @param String [in] the string
    @param Width [in] output string minimum width
    @param Precision [in] output string minimum width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    */
    SAPDB_ToStringClass( const SAPDB_Char *String,
                        const SAPDB_UInt2 Width,
                        const SAPDB_UInt2 Precision,
                        const fmtflags Flag1=noformat,
                        const fmtflags Flag2=noformat)
    {   FillStringFormatBuffer( String, Width, Precision, Flag1 | Flag2 ); }

    /*!-------------------------------------------------------------------------
    @brief ctor for simple string with format flags and width and precision specification

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)

    @param String [in] the string
    @param Width [in] output string minimum width
    @param Precision [in] output string minimum width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    */
    SAPDB_ToStringClass( const SAPDB_UTF8 *String,
                        const SAPDB_UInt2 Width,
                        const SAPDB_UInt2 Precision,
                        const fmtflags Flag1=noformat,
                        const fmtflags Flag2=noformat)
    {   FillStringFormatBuffer( (const SAPDB_Char *)String, Width, Precision, Flag1 | Flag2 ); }

    /*!-------------------------------------------------------------------------
    @brief ctor for one byte signed number with format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit SAPDB_ToStringClass(const SAPDB_Int1 Number,
                                 const fmtflags Flag1=noformat,
                                 const fmtflags Flag2=noformat,
                                 const fmtflags Flag3=noformat,
                                 const fmtflags Flag4=noformat);

    /*!-------------------------------------------------------------------------
    @brief ctor for one byte signed number with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
                        SAPDB_ToStringClass::showpoint


    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    SAPDB_ToStringClass(const SAPDB_Int1 Number,
                        const SAPDB_UInt2 Width ,
                        const fmtflags Flag1=noformat,
                        const fmtflags Flag2=noformat,
                        const fmtflags Flag3=noformat,
                        const fmtflags Flag4=noformat);

    // ---------------- UINT 1 -----------------

    /*!-------------------------------------------------------------------------
    @brief ctor for one byte signed number with format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit SAPDB_ToStringClass(const SAPDB_UInt1 Number,
                            const fmtflags Flag1=noformat,
                            const fmtflags Flag2=noformat,
                            const fmtflags Flag3=noformat,
                            const fmtflags Flag4=noformat);

    /*!-------------------------------------------------------------------------
    @brief ctor for one byte unsigned number with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
                        SAPDB_ToStringClass::showpoint


    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
             SAPDB_ToStringClass(const SAPDB_UInt1 Number,
                    const SAPDB_UInt2 Width ,
                    const fmtflags Flag1=noformat,
                    const fmtflags Flag2=noformat,
                    const fmtflags Flag3=noformat,
                    const fmtflags Flag4=noformat);

    // ---------------- INT 2 -----------------

    /*!-------------------------------------------------------------------------
    @brief ctor for two byte signed number with format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit SAPDB_ToStringClass(const SAPDB_Int2 Number,
                            const fmtflags Flag1=noformat,
                            const fmtflags Flag2=noformat,
                            const fmtflags Flag3=noformat,
                            const fmtflags Flag4=noformat);

    /*!-------------------------------------------------------------------------
    @brief ctor for two byte signed number with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
                        SAPDB_ToStringClass::showpoint


    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
             SAPDB_ToStringClass(const SAPDB_Int2 Number,
                    const SAPDB_UInt2 Width ,
                    const fmtflags Flag1=noformat,
                    const fmtflags Flag2=noformat,
                    const fmtflags Flag3=noformat,
                    const fmtflags Flag4=noformat);

    // ---------------- UINT 2 -----------------

    /*!-------------------------------------------------------------------------
    @brief ctor for two byte unsigned number with format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit SAPDB_ToStringClass(const SAPDB_UInt2 Number,
                            const fmtflags Flag1=noformat,
                            const fmtflags Flag2=noformat,
                            const fmtflags Flag3=noformat,
                            const fmtflags Flag4=noformat);

    /*!-------------------------------------------------------------------------
    @brief ctor for two byte unsigned number with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
                        SAPDB_ToStringClass::showpoint


    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
             SAPDB_ToStringClass(const SAPDB_UInt2 Number,
                    const SAPDB_UInt2 Width ,
                    const fmtflags Flag1=noformat,
                    const fmtflags Flag2=noformat,
                    const fmtflags Flag3=noformat,
                    const fmtflags Flag4=noformat);

    // ---------------- INT 4-----------------

    /*!-------------------------------------------------------------------------
    @brief ctor for four byte signed number with format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit SAPDB_ToStringClass(const SAPDB_Int4 Number,
                            const fmtflags Flag1=noformat,
                            const fmtflags Flag2=noformat,
                            const fmtflags Flag3=noformat,
                            const fmtflags Flag4=noformat);

    /*!-------------------------------------------------------------------------
    @brief ctor for four byte signed number with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
                        SAPDB_ToStringClass::showpoint


    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
             SAPDB_ToStringClass(const SAPDB_Int4 Number,
                    const SAPDB_UInt2 Width ,
                    const fmtflags Flag1=noformat,
                    const fmtflags Flag2=noformat,
                    const fmtflags Flag3=noformat,
                    const fmtflags Flag4=noformat);

    // ---------------- UINT 4 -----------------

    /*!-------------------------------------------------------------------------
    @brief ctor for four byte unsigned number with format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit SAPDB_ToStringClass(const SAPDB_UInt4 Number,
                            const fmtflags Flag1=noformat,
                            const fmtflags Flag2=noformat,
                            const fmtflags Flag3=noformat,
                            const fmtflags Flag4=noformat);

    /*!-------------------------------------------------------------------------
    @brief ctor for four byte unsigned number with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
                        SAPDB_ToStringClass::showpoint


    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
             SAPDB_ToStringClass(const SAPDB_UInt4 Number,
                    const SAPDB_UInt2 Width ,
                    const fmtflags Flag1=noformat,
                    const fmtflags Flag2=noformat,
                    const fmtflags Flag3=noformat,
                    const fmtflags Flag4=noformat);

#if !defined (BIT64) && !defined (_WIN64)
    // ---------------- INT8 -----------------

    /*!-------------------------------------------------------------------------
    @brief ctor for eight byte signed number with format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit SAPDB_ToStringClass(const SAPDB_Int8 Number,
                            const fmtflags Flag1=noformat,
                            const fmtflags Flag2=noformat,
                            const fmtflags Flag3=noformat,
                            const fmtflags Flag4=noformat);

    /*!-------------------------------------------------------------------------
    @brief ctor for eight byte unsigned number with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
                        SAPDB_ToStringClass::showpoint


    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
             SAPDB_ToStringClass(const SAPDB_Int8 Number,
                    const SAPDB_UInt2 Width ,
                    const fmtflags Flag1=noformat,
                    const fmtflags Flag2=noformat,
                    const fmtflags Flag3=noformat,
                    const fmtflags Flag4=noformat);

    // ---------------- UINT8 -----------------

    /*!-------------------------------------------------------------------------
    @brief ctor for eight byte unsigned number with format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit SAPDB_ToStringClass(const SAPDB_UInt8 Number,
                            const fmtflags Flag1=noformat,
                            const fmtflags Flag2=noformat,
                            const fmtflags Flag3=noformat,
                            const fmtflags Flag4=noformat);

    /*!-------------------------------------------------------------------------
    @brief ctor for eight byte unsigned number with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
                        SAPDB_ToStringClass::showpoint
 

    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
             SAPDB_ToStringClass(const SAPDB_UInt8 Number,
                    const SAPDB_UInt2 Width ,
                    const fmtflags Flag1=noformat,
                    const fmtflags Flag2=noformat,
                    const fmtflags Flag3=noformat,
                    const fmtflags Flag4=noformat);
#endif

#if defined (_WIN32) && defined (_WIN64)
    // ---------------- long -----------------

    /*!-------------------------------------------------------------------------
    @brief ctor for long on Windows for 64Bit with format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit SAPDB_ToStringClass(const long Number,
                            const fmtflags Flag1=noformat,
                            const fmtflags Flag2=noformat,
                            const fmtflags Flag3=noformat,
                            const fmtflags Flag4=noformat);

    /*!-------------------------------------------------------------------------
    @brief ctor for long on Windows for 64Bit with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
                        SAPDB_ToStringClass::showpoint
 

    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
             SAPDB_ToStringClass(const long Number,
                    const SAPDB_UInt2 Width ,
                    const fmtflags Flag1=noformat,
                    const fmtflags Flag2=noformat,
                    const fmtflags Flag3=noformat,
                    const fmtflags Flag4=noformat);

    // ---------------- unsigned long ---------

    /*!-------------------------------------------------------------------------
    @brief ctor for long on Windows for 64Bit unsigned number with format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit SAPDB_ToStringClass(const unsigned long Number,
                            const fmtflags Flag1=noformat,
                            const fmtflags Flag2=noformat,
                            const fmtflags Flag3=noformat,
                            const fmtflags Flag4=noformat);

    /*!-------------------------------------------------------------------------
    @brief ctor for long on Windows for 64Bit unsigned number with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
                        SAPDB_ToStringClass::showpoint
 

    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
             SAPDB_ToStringClass(const unsigned long Number,
                    const SAPDB_UInt2 Width ,
                    const fmtflags Flag1=noformat,
                    const fmtflags Flag2=noformat,
                    const fmtflags Flag3=noformat,
                    const fmtflags Flag4=noformat);
#endif



    // ---------------- LONG-----------------

    /*!-------------------------------------------------------------------------
    @brief ctor for SAPDB_Long signed number with format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit SAPDB_ToStringClass(const SAPDB_Long Number,
                            const fmtflags Flag1=noformat,
                            const fmtflags Flag2=noformat,
                            const fmtflags Flag3=noformat,
                            const fmtflags Flag4=noformat);
    /*!-------------------------------------------------------------------------
    @brief ctor for SAPDB_Long signed number with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
                        SAPDB_ToStringClass::showpoint
 

    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
             SAPDB_ToStringClass(const SAPDB_Long Number,
                    const SAPDB_UInt2 Width ,
                    const fmtflags Flag1=noformat,
                    const fmtflags Flag2=noformat,
                    const fmtflags Flag3=noformat,
                    const fmtflags Flag4=noformat);

    // ---------------- ULONG-----------------

    /*!-------------------------------------------------------------------------
    @brief ctor for SAPDB_Long unsigned number with format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit SAPDB_ToStringClass(const SAPDB_ULong Number,
                            const fmtflags Flag1=noformat,
                            const fmtflags Flag2=noformat,
                            const fmtflags Flag3=noformat,
                            const fmtflags Flag4=noformat);

    /*!-------------------------------------------------------------------------
    @brief ctor for SAPDB_Long unsigned number with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
                        SAPDB_ToStringClass::showpoint
 

    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
             SAPDB_ToStringClass(const SAPDB_ULong Number,
                    const SAPDB_UInt2 Width ,
                    const fmtflags Flag1=noformat,
                    const fmtflags Flag2=noformat,
                    const fmtflags Flag3=noformat,
                    const fmtflags Flag4=noformat);

    /*!-------------------------------------------------------------------------
    @brief ctor for floating point number with format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::scientific
                        SAPDB_ToStringClass::fixed
                        SAPDB_ToStringClass::showpoint
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param Real [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit SAPDB_ToStringClass(const SAPDB_Real8  Real,
                            const fmtflags Flag1=noformat,
                            const fmtflags Flag2=noformat,
                            const fmtflags Flag3=noformat,
                            const fmtflags Flag4=noformat) ;

    /*!-------------------------------------------------------------------------
    @brief ctor for floating point number with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::scientific
                        SAPDB_ToStringClass::fixed
                        SAPDB_ToStringClass::showpoint
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param Real [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
             SAPDB_ToStringClass(const SAPDB_Real8  Real,
                            const SAPDB_UInt2 Width ,
                            const fmtflags Flag1=noformat,
                            const fmtflags Flag2=noformat,
                            const fmtflags Flag3=noformat,
                            const fmtflags Flag4=noformat) ;

    /*!-------------------------------------------------------------------------
    @brief ctor for floating point number with width and precision specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::scientific
                        SAPDB_ToStringClass::fixed
                        SAPDB_ToStringClass::showpoint
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param Real [in] the string
    @param Width [in] the minimum output width
    @param Precision [in] the given number of precision digits to show
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
             SAPDB_ToStringClass(const SAPDB_Real8  Real,
                            const SAPDB_UInt2 Width ,
                            const SAPDB_UInt2 Precision,
                            const fmtflags Flag1=noformat,
                            const fmtflags Flag2=noformat,
                            const fmtflags Flag3=noformat,
                            const fmtflags Flag4=noformat) ;

    /*!-------------------------------------------------------------------------
    @brief ctor for Pointer (address)  with width and precision specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::ptr (default)
                        SAPDB_ToStringClass::showbase (default - set)
                        SAPDB_ToStringClass::uppercase (default)

    @param Addr [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit SAPDB_ToStringClass(const void *Addr,
                            const fmtflags Flag1=SAPDB_ToStringClass::showbase,
                            const fmtflags Flag2=noformat,
                            const fmtflags Flag3=noformat,
                            const fmtflags Flag4=noformat);

    /*!-------------------------------------------------------------------------
    @brief ctor for Pointer (address)  with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::ptr (default)
                        SAPDB_ToStringClass::showbase (default - set)
                        SAPDB_ToStringClass::uppercase (default)
                        SAPDB_ToStringClass::showpoint

    @param Addr [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
             SAPDB_ToStringClass(const void *Addr,
                            const SAPDB_UInt2 Width ,
                            const fmtflags Flag1=SAPDB_ToStringClass::showbase,
                            const fmtflags Flag2=noformat,
                            const fmtflags Flag3=noformat,
                            const fmtflags Flag4=noformat);

#ifndef CPP_BOOL_NOT_IMPLEMENTED
    /*!-------------------------------------------------------------------------
    @brief ctor for bool and format flags

    supported fmtflags: SAPDB_ToStringClass::boolalpha (default "true", "false" )
                        SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::uppercase (default - not set)

    @param Number [in] the string
    @param Flag1 [in] first format flag (default boolalpha)
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit SAPDB_ToStringClass(const SAPDB_Bool Number,
                            const fmtflags Flag1=boolalpha,
                            const fmtflags Flag2=noformat,
                            const fmtflags Flag3=noformat,
                            const fmtflags Flag4=noformat);
    /*!-------------------------------------------------------------------------
    @brief ctor for bool with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::boolalpha (default "true", "false" )
                        SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::uppercase (default - not set)

    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag (default boolalpha)
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
             SAPDB_ToStringClass(const SAPDB_Bool Number,
                            const SAPDB_UInt2 Width,
                            const fmtflags Flag1=boolalpha,
                            const fmtflags Flag2=noformat,
                            const fmtflags Flag3=noformat,
                            const fmtflags Flag4=noformat);

#endif /* CPP_BOOL_NOT_IMPLEMENTED */

    /*!-------------------------------------------------------------------------
    @brief copy ctor

    Copies one string class to another
     */
    SAPDB_ToStringClass(SAPDB_ToStringClass const &toCopy);

    /*!-------------------------------------------------------------------------
    @brief dtor

    No dynamic memory allocated, nothing to do...
     */
    ~SAPDB_ToStringClass();

    /*!-------------------------------------------------------------------------
    @brief cast operator (SAPDB_Char *)
    @return pointer to output string
    */
    operator       SAPDB_Char * () const { return ( const_cast<SAPDB_Char *>(m_OutputString) ) ; }

    /*!-------------------------------------------------------------------------
    @brief cast operator (const SAPDB_Char *)
    @return pointer to output string
    */
    operator const SAPDB_Char * () const { return ( m_OutputString ) ; }

    /*!-------------------------------------------------------------------------
    @brief assignment operator
    @return pointer to output string
    */
    SAPDB_ToStringClass & operator=(const SAPDB_ToStringClass &);

    /*!-------------------------------------------------------------------------
    @brief return length of output string
    @return length of output string in bytes
    */
    SAPDB_Int4 const Length() const;
private:

    void FillBoolAlphaFormatBuffer( const SAPDB_Bool isTrue,
                                    const SAPDB_UInt2 Width,
                                    const int         Flags );

    void FillNumberFormatBuffer( SAPDB_Char *formatBuffer,
                                 const SAPDB_UInt2 Width,
                                 const int         Flags,
                                 const bool        SignedValue = true,
                                 const bool        QuadWord = false );

    void FillSignedNumberFormatBuffer( const SAPDB_Int8  Number,
                                   const SAPDB_UInt2 Width,
                                   const int         Flags,
                                   const bool        QuadWord=false );

    void FillUnsignedNumberFormatBuffer( const SAPDB_UInt8  Number,
                                     const SAPDB_UInt2 Width,
                                     const int         Flags,
                                     const bool        QuadWord=false );

    void FillStringFormatBuffer( const SAPDB_Char *String,
                                 const SAPDB_UInt2 Width,
                                 const SAPDB_UInt2 Precision,
                                 const int         Flags=noformat );

    void FillRealFormatBuffer  ( const SAPDB_UInt2 Width,
                                 const SAPDB_UInt2 Precision,
                                 const int         Flags,
                                 const SAPDB_Real8 Number) ;
}; /*! @endclass SAPDB_ToStringClass */

/*!
  @brief Simple String
 */
inline SAPDB_ToStringClass
SAPDB_ToString( const SAPDB_Char *String) { return SAPDB_ToStringClass(String); }

/*!
  @brief Simple String with format flags
 */
inline SAPDB_ToStringClass 
SAPDB_ToString( const SAPDB_Char *String,
                const SAPDB_ToStringClass::fmtflags fmtflags) { return SAPDB_ToStringClass(String, fmtflags); }

/*!
  @brief Simple String with width specification and format flags
 */
inline SAPDB_ToStringClass 
SAPDB_ToString( const SAPDB_Char *String,
                const SAPDB_UInt2 Width,
                const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat) { return SAPDB_ToStringClass(String, Width, Flag1, Flag2); }

/*!
  @brief Simple String with width and precision specification and format flags
 */
inline SAPDB_ToStringClass
SAPDB_ToString( const SAPDB_Char *String,
                const SAPDB_UInt2 Width,
                const SAPDB_UInt2 Precision,
                const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat) { return SAPDB_ToStringClass(String, Width, Precision, Flag1, Flag2); }

//! Macro to convert any integer into SAPDB_ToStringClass
#define SAPDB_INTEGER_TO_STRING_FUNCTION(type_name_) \
    inline SAPDB_ToStringClass \
    SAPDB_ToString( const type_name_ Number, \
                    const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat, \
                    const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat, \
                    const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat, \
                    const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat) \
    { \
        return SAPDB_ToStringClass(Number, Flag1, Flag2, Flag3, Flag4); \
    } \
\
    inline SAPDB_ToStringClass \
    SAPDB_ToString( const type_name_ Number, \
                    const SAPDB_UInt2 Width , \
                    const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat, \
                    const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat, \
                    const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat, \
                    const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat) \
    { \
        return SAPDB_ToStringClass(Number, Width, Flag1, Flag2, Flag3, Flag4); \
    }

/* END OF SAPDB_INTEGER_TO_STRING_FUNCTION(type_name_) MACRO */

//! Macro to convert any floating point number into SAPDB_ToStringClass
#define SAPDB_FLOAT_TO_STRING_FUNCTION(type_name_) \
    inline SAPDB_ToStringClass \
    SAPDB_ToString( const type_name_ Number, \
                    const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat, \
                    const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat, \
                    const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat, \
                    const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat) \
    { \
        return SAPDB_ToStringClass(Number, Flag1, Flag2, Flag3, Flag4); \
    } \
\
    inline SAPDB_ToStringClass \
    SAPDB_ToString( const type_name_ Number, \
                    const SAPDB_UInt2 Width , \
                    const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat, \
                    const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat, \
                    const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat, \
                    const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat) \
    { \
        return SAPDB_ToStringClass(Number, Width, Flag1, Flag2, Flag3, Flag4); \
    } \
\
    inline SAPDB_ToStringClass \
    SAPDB_ToString( const type_name_ Number, \
                    const SAPDB_UInt2 Width , \
                    const SAPDB_UInt2 Precision , \
                    const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat, \
                    const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat, \
                    const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat, \
                    const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat) \
    { \
        return SAPDB_ToStringClass(Number, Width, Flag1, Flag2, Flag3, Flag4); \
    }

/* END OF SAPDB_FLOAT_TO_STRING_FUNCTION(type_name_) MACRO */

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_Int1 Number, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given format flags
  @param Number [in] the number
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_Int1 Number, const SAPDB_UInt2 Width, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given width specification and format flags
  @param Number [in] the number
  @param Width [in] the minimum output width
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */
SAPDB_INTEGER_TO_STRING_FUNCTION(SAPDB_Int1)
 
/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_UInt1 Number, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given format flags
  @param Number [in] the number
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_UInt1 Number, const SAPDB_UInt2 Width, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given width specification and format flags
  @param Number [in] the number
  @param Width [in] the minimum output width
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */
SAPDB_INTEGER_TO_STRING_FUNCTION(SAPDB_UInt1)

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_Int2 Number, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given format flags
  @param Number [in] the number
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_Int2 Number, const SAPDB_UInt2 Width, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given width specification and format flags
  @param Number [in] the number
  @param Width [in] the minimum output width
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */
SAPDB_INTEGER_TO_STRING_FUNCTION(SAPDB_Int2)

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_UInt2 Number, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given format flags
  @param Number [in] the number
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_UInt2 Number, const SAPDB_UInt2 Width, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given width specification and format flags
  @param Number [in] the number
  @param Width [in] the minimum output width
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */
SAPDB_INTEGER_TO_STRING_FUNCTION(SAPDB_UInt2)

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_Int4 Number, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given format flags
  @param Number [in] the number
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_Int4 Number, const SAPDB_UInt2 Width, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given width specification and format flags
  @param Number [in] the number
  @param Width [in] the minimum output width
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */
SAPDB_INTEGER_TO_STRING_FUNCTION(SAPDB_Int4)

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_UInt4 Number, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given format flags
  @param Number [in] the number
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_UInt4 Number, const SAPDB_UInt2 Width, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given width specification and format flags
  @param Number [in] the number
  @param Width [in] the minimum output width
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */
SAPDB_INTEGER_TO_STRING_FUNCTION(SAPDB_UInt4)

#if !defined (BIT64) && !defined (_WIN64)
/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_Int8 Number, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given format flags
  @param Number [in] the number
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_Int8 Number, const SAPDB_UInt2 Width, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given width specification and format flags
  @param Number [in] the number
  @param Width [in] the minimum output width
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */
SAPDB_INTEGER_TO_STRING_FUNCTION(SAPDB_Int8)

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_UInt8 Number, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given format flags
  @param Number [in] the number
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_UInt8 Number, const SAPDB_UInt2 Width, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given width specification and format flags
  @param Number [in] the number
  @param Width [in] the minimum output width
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */
SAPDB_INTEGER_TO_STRING_FUNCTION(SAPDB_UInt8)
#endif

#if defined (_WIN32) && defined (_WIN64)
/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const long Number, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given format flags
  @param Number [in] the number
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const long Number, const SAPDB_UInt2 Width, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given width specification and format flags
  @param Number [in] the number
  @param Width [in] the minimum output width
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */
SAPDB_INTEGER_TO_STRING_FUNCTION(long)

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const unsigned long Number, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given format flags
  @param Number [in] the number
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const unsigned long Number, const SAPDB_UInt2 Width, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given width specification and format flags
  @param Number [in] the number
  @param Width [in] the minimum output width
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */
SAPDB_INTEGER_TO_STRING_FUNCTION(unsigned long)
#endif

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_Long Number, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given format flags
  @param Number [in] the number
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_Long Number, const SAPDB_UInt2 Width, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given width specification and format flags
  @param Number [in] the number
  @param Width [in] the minimum output width
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */
SAPDB_INTEGER_TO_STRING_FUNCTION(SAPDB_Long)

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_ULong Number, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given format flags
  @param Number [in] the number
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_ULong Number, const SAPDB_UInt2 Width, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given width specification and format flags
  @param Number [in] the number
  @param Width [in] the minimum output width
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */
SAPDB_INTEGER_TO_STRING_FUNCTION(SAPDB_ULong)

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_Real8 Number, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given format flags
  @param Number [in] the number
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */

/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_Real8 Number, const SAPDB_UInt2 Width, const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given width specification and format flags
  @param Number [in] the number
  @param Width [in] the minimum output width
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */
 
/*!
  @fn inline SAPDB_ToStringClass SAPDB_ToString(const SAPDB_Real8 Number, const SAPDB_UInt2 Width, const SAPDB_UInt2 Precision ,const SAPDB_ToStringClass::fmtflags Flag1, const SAPDB_ToStringClass::fmtflags Flag2, const SAPDB_ToStringClass::fmtflags Flag3, const SAPDB_ToStringClass::fmtflags Flag4)

  @brief helper function that returns a SAPDB_ToStringClass for a number with given width specification and format flags
  @param Number [in] the number
  @param Width [in] the minimum output width
  @param Precision [in] the wanted precision 
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */
SAPDB_FLOAT_TO_STRING_FUNCTION(SAPDB_Real8)

/*!
  @brief Function for converting address with formatting flags
  @param Addr [in] the address
  @param Flag1 [in] first format flag (default showbase)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */
inline SAPDB_ToStringClass 
SAPDB_ToString( const void *Addr,
                const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::showbase,
                const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat) { return SAPDB_ToStringClass(Addr, Flag1, Flag2, Flag3, Flag4); }

/*!
  @brief Function for converting address with formatting flags
  @param Addr [in] the address
  @param Width [in] the minimum output width
  @param Flag1 [in] first format flag (default showbase)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */
inline SAPDB_ToStringClass 
SAPDB_ToString( const void *Addr,
                const SAPDB_UInt2 Width ,
                const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::showbase,
                const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat) { return SAPDB_ToStringClass(Addr, Width, Flag1, Flag2, Flag3, Flag4); }

#ifndef CPP_BOOL_NOT_IMPLEMENTED
/*!
  @brief Function for converting bool with formatting flags
  @param Number [in] the bool
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */
inline SAPDB_ToStringClass
SAPDB_ToString( const SAPDB_Bool Number,
                const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::boolalpha,
                const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat) { return SAPDB_ToStringClass(Number, Flag1, Flag2, Flag3, Flag4); }

/*!
  @brief Function for converting bool with formatting flags
  @param Number [in] the bool
  @param Width [in] the minimum output width
  @param Flag1 [in] first format flag (default boolalpha)
  @param Flag2 [in] second format flag
  @param Flag3 [in] third format flag
  @param Flag4 [in] forth format flag
  @return a SAPDB_ToStringClass object
 */
inline SAPDB_ToStringClass
SAPDB_ToString( const SAPDB_Bool Number,
                const SAPDB_UInt2 Width,
                const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::boolalpha,
                const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat) { return SAPDB_ToStringClass(Number, Width, Flag1, Flag2, Flag3, Flag4); }
#endif /* CPP_BOOL_NOT_IMPLEMENTED */

/*!
    @brief concatenation of a fixed string prefix and a SAPDB_ToStringClass object and optional suffix

    This function  is used to create SAPDB_ToStringClass objects like T002 or Job007ForIo...

    @param prefix [in] fixed string prefix
    @param valueString [in] string object to add
    @param postfix [in] fixed string postfix
    @return a SAPDB_ToStringClass object
 */
SAPDB_ToStringClass SAPDB_ToString( SAPDB_Char const *prefix
                                  , SAPDB_ToStringClass & valueString
                                  , SAPDB_Char const *postfix=0);

#endif // SAPDB_TOSTRING_HPP
