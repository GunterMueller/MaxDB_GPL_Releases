/*!
  @file    Msg_Arg.hpp
  @author  JoergM
  @ingroup Common
  @brief   General purpose Msg_OptArg implementation
  
  The class implements a general purpose Tag+Value class derived from Msg_IOptArg.

  The value string can be specified in the same way as SAPDB_ToStringClass allows.
  
  See SAPDBCommon/SAPDB_ToString.hpp for details.

  The Tag and the value may be modified afterwards by using the SetTag() and SetValue()
  member functions if tag or values changed or are not known when Msg_OptArg is constructed.

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

#ifndef MSG_ARG_HPP
#define MSG_ARG_HPP

#include "Messages/Msg_IOptArg.hpp"
#include "Messages/Msg_IArg.hpp"
#include "SAPDBCommon/SAPDB_ToString.hpp"
#include "SAPDBCommon/SAPDB_MemCopyMoveNoCheck.hpp"
#include "SAPDBCommon/SAPDB_string.h"

/*!
  @class Msg_OptArg
  @brief Message argument interface class for valid tags

  A message argument consists of a tag and a value string. Since value strings may be unicode
  all value strings are returned as zero terminated UTF8 strings. An empty string ((SAPDB_UTF8 const *)"") is a valid
  value string, but an empty tag string is not allowed! This class validates this using assertions.

  This class also copies the tag string and uses identical form of constructors for value strings like SAPDB_ToString class.
  
 */
class Msg_OptArg : public Msg_IOptArg
{
public:
    /*!
      @brief default ctor

      While value must not necessarily be known during contruction, the tag is most times a constant string.
      Therefore the default constructor fill 'Msg_IOptArg::UnknownTag' and sets 'Msg_IOptArg::UndefinedValue'.
     */
    Msg_OptArg()
        : m_Value(Msg_IOptArg::UndefinedValue)
    {
        SetTag(0);
    }

    /*!
      @brief ctor

      While value must not necessarily be known during contruction, the tag is most times a constant string.
      Therefor it makes sends to be able to derive from this class, which is no pure interface class, but restricts
      the tag usage to non empty character strings.
      @param tag [in] the tag string
     */
    Msg_OptArg( SAPDB_Char const *tag)
        : m_Value(Msg_IOptArg::UndefinedValue)
    {
        SetTag(tag);
    }

    /*!
      @brief ctor for SAPDB_ToStringClass value

      No conversion needed

      @param tag [in] the tag string
      @param value [in] the value 
     */
    Msg_OptArg( SAPDB_Char const *tag
           , SAPDB_ToStringClass const &value)
        : m_Value(value)
    {
        SetTag(tag);
    }

    /*!
    @brief ctor for simple string

    No conversion needed

    @param tag [in] the tag string
    @param String [in] the string
    */
    explicit Msg_OptArg( SAPDB_Char const *tag,
                     const SAPDB_Char *String)
        : m_Value(String)
    {
        SetTag(tag);
    }

    /*!
    @brief ctor for simple UTF8 string

    No conversion needed

    @param tag [in] the tag string
    @param String [in] the string
    */
    explicit Msg_OptArg( SAPDB_Char const *tag,
                     const SAPDB_UTF8 *String)
        : m_Value(String)
    {
        SetTag(tag);
    }

    /*!-------------------------------------------------------------------------
    @brief ctor for simple string with format flags

    No conversion needed

    @param tag [in] the tag string
    @param String [in] the string
     param SAPDB_ToStringClass::fmtflags [in] ignored
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const SAPDB_Char *String,
            const SAPDB_ToStringClass::fmtflags )
        : m_Value(String)
    {
        SetTag(tag);
    }

    /*!-------------------------------------------------------------------------
    @brief ctor for simple UTF8 string with format flags

    No conversion needed

    @param tag [in] the tag string
    @param String [in] the string
     param SAPDB_ToStringClass::fmtflags [in] ignored
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const SAPDB_UTF8 *String,
            const SAPDB_ToStringClass::fmtflags )
        : m_Value(String)
    {
        SetTag(tag);
    }

    /*!-------------------------------------------------------------------------
    @brief ctor for simple string with format flags and width specification

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)

    @param tag [in] the tag string
    @param String [in] the string
    @param Width [in] output string minimum width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const SAPDB_Char *String,
            const SAPDB_UInt2 Width,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat)
        : m_Value(String, Width, Flag1, Flag2)
    {
        SetTag(tag);
    }

    /*!-------------------------------------------------------------------------
    @brief ctor for simple UTF8 string with format flags and width specification

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)

    @param tag [in] the tag string
    @param String [in] the string
    @param Width [in] output string minimum width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const SAPDB_UTF8 *String,
            const SAPDB_UInt2 Width,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat)
        : m_Value(String, Width, Flag1, Flag2)
    {
        SetTag(tag);
    }

    /*!-------------------------------------------------------------------------
    @brief ctor for simple string with format flags and width and precision specification

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)

    @param tag [in] the tag string
    @param String [in] the string
    @param Width [in] output string minimum width
    @param Precision [in] output string minimum width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const SAPDB_Char *String,
            const SAPDB_UInt2 Width,
            const SAPDB_UInt2 Precision,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat)
        : m_Value(String, Width, Precision, Flag1, Flag2)
    {
        SetTag(tag);
    }

    /*!-------------------------------------------------------------------------
    @brief ctor for simple UTF8 string with format flags and width and precision specification

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)

    @param tag [in] the tag string
    @param String [in] the string
    @param Width [in] output string minimum width
    @param Precision [in] output string minimum width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const SAPDB_UTF8 *String,
            const SAPDB_UInt2 Width,
            const SAPDB_UInt2 Precision,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat)
        : m_Value(String, Width, Precision, Flag1, Flag2)
    {
        SetTag(tag);
    }

    /*!-------------------------------------------------------------------------
    @brief ctor for one byte signed number with format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_OptArg( SAPDB_Char const *tag,
                     const SAPDB_Int1 Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

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


    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const SAPDB_Int1 Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Width, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

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

    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_OptArg( SAPDB_Char const *tag,
                     const SAPDB_UInt1 Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

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


    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const SAPDB_UInt1 Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Width, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }


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
 
    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_OptArg( SAPDB_Char const *tag,
                     const SAPDB_Int2 Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

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

    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const SAPDB_Int2 Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Width, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

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
 
    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_OptArg( SAPDB_Char const *tag,
                     const SAPDB_UInt2 Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

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


    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const SAPDB_UInt2 Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Width, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

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
 
    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_OptArg( SAPDB_Char const *tag,
                     const SAPDB_Int4 Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

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


    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const SAPDB_Int4 Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Width, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

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
 
    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_OptArg( SAPDB_Char const *tag,
                     const SAPDB_UInt4 Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

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


    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const SAPDB_UInt4 Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Width, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_OptArg( SAPDB_Char const *tag,
                     const SAPDB_Int8 Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

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


    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const SAPDB_Int8 Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Width, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_OptArg( SAPDB_Char const *tag,
                     const SAPDB_UInt8 Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const SAPDB_UInt8 Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Width, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }
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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_OptArg( SAPDB_Char const *tag,
                     const long Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const long Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Width, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_OptArg( SAPDB_Char const *tag, 
                     const unsigned long Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const unsigned long Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Width, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }
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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_OptArg( SAPDB_Char const *tag,
                     const SAPDB_Long Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }
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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const SAPDB_Long Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Width, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_OptArg( SAPDB_Char const *tag,
                     const SAPDB_ULong Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_OptArg( SAPDB_Char const *tag, 
            const SAPDB_ULong Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Width, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

    /*!-------------------------------------------------------------------------
    @brief ctor for floating point number with format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::scientific
                        SAPDB_ToStringClass::fixed
                        SAPDB_ToStringClass::showpoint
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param tag [in] the tag string
    @param Real [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_OptArg( SAPDB_Char const *tag,
                     const SAPDB_Real8  Real,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat) 
        : m_Value(Real, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

    /*!-------------------------------------------------------------------------
    @brief ctor for floating point number with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::scientific
                        SAPDB_ToStringClass::fixed
                        SAPDB_ToStringClass::showpoint
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param tag [in] the tag string
    @param Real [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const SAPDB_Real8  Real,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Real, Width, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

    /*!-------------------------------------------------------------------------
    @brief ctor for floating point number with width and precision specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::scientific
                        SAPDB_ToStringClass::fixed
                        SAPDB_ToStringClass::showpoint
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param tag [in] the tag string
    @param Real [in] the string
    @param Width [in] the minimum output width
    @param Precision [in] the given number of precision digits to show
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const SAPDB_Real8  Real,
            const SAPDB_UInt2 Width ,
            const SAPDB_UInt2 Precision,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Real, Width, Precision, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

    /*!-------------------------------------------------------------------------
    @brief ctor for Pointer (address)  with width and precision specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::ptr (default)
                        SAPDB_ToStringClass::showbase (default - set)
                        SAPDB_ToStringClass::uppercase (default)

    @param tag [in] the tag string
    @param Addr [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_OptArg( SAPDB_Char const *tag,
                     const void *Addr,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::showbase,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Addr, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

    /*!-------------------------------------------------------------------------
    @brief ctor for Pointer (address)  with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::ptr (default)
                        SAPDB_ToStringClass::showbase (default - set)
                        SAPDB_ToStringClass::uppercase (default)
                        SAPDB_ToStringClass::showpoint

    @param tag [in] the tag string
    @param Addr [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const void *Addr,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::showbase,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Addr, Width, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

#ifndef CPP_BOOL_NOT_IMPLEMENTED
    /*!-------------------------------------------------------------------------
    @brief ctor for bool and format flags

    supported fmtflags: SAPDB_ToStringClass::boolalpha (default "true", "false" )
                        SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::uppercase (default - not set)

    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag (default boolalpha)
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_OptArg( SAPDB_Char const *tag,
                     const SAPDB_Bool Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::boolalpha,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }
    /*!-------------------------------------------------------------------------
    @brief ctor for bool with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::boolalpha (default "true", "false" )
                        SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::uppercase (default - not set)

    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag (default boolalpha)
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_OptArg( SAPDB_Char const *tag,
            const SAPDB_Bool Number,
            const SAPDB_UInt2 Width,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::boolalpha,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_Value(Number, Width, Flag1, Flag2, Flag3, Flag4)
    {
        SetTag(tag);
    }

#endif /* CPP_BOOL_NOT_IMPLEMENTED */

    /*!-------------------------------------------------------------------------
    @brief copy ctor

    Copies one argument to another
    @param toCopy [in] another Msg_OptArg instance to clone from
     */
    Msg_OptArg(Msg_OptArg const &toCopy)
        : m_Value("")
    {
        if ( this != &toCopy )
        {
            m_Tag = toCopy.m_Tag;
            m_Value = toCopy.m_Value;
        }
    }

    /*!-------------------------------------------------------------------------
    @brief copy ctor from different Msg_IOptArg implementation

    Copies one argument to another
    @param toCopy [in] another Msg_IOptArg derived instance to clone from
     */
    Msg_OptArg(Msg_IOptArg const * toCopy)
        : m_Value("")
    {
        if ( this != toCopy && toCopy )
        {
            SetTag(toCopy->Tag());
            // enforce copy action into own member variable
            SetValue((SAPDB_UTF8 *)toCopy->Value());
        }
    }

    // wow a nice number of constructors..... but finally this was the last one...

    /*!
      @brief dtor

      needed since we have some virtual member functions...
     */
    virtual ~Msg_OptArg()
    {}

    /*!-------------------------------------------------------------------------
    @brief assignment operator
    @param toAssignFrom [in] argument to assign from 
    @return pointer to output string
    */
    Msg_OptArg & operator=(const Msg_IOptArg &toAssignFrom)
    {
        if ( this != &toAssignFrom )
        {
            SetTag(toAssignFrom.Tag());
            // enforce copy action into own member variable
            SetValue((SAPDB_UTF8 *)toAssignFrom.Value());
        }
        return *this;
    }

    /*!
      @brief return fixed tag string as zero terminated C String

      This routine always returns a valid constant C String
      NULL must never be returned!

      @return zero terminated C String
     */
    virtual SAPDB_Char const *Tag() const { return m_Tag; }

    /*!
      @brief return fixed value string as zero terminated C String

      This routine always returns a valid value C String.
      NULL must never be returned!

      @return zero terminated C String
     */
    virtual SAPDB_UTF8 const *Value() const { return (SAPDB_UTF8 const *)((const SAPDB_Char *)m_Value); }

    /*!
      @brief cast operator needed for automatic cast to Msg_IOptArg pointer
      @return should return the 'this' pointer
     */
    virtual operator Msg_IOptArg const * () const { return this; }

    /*!
      @brief set tag after construction

      Since this is a protected member function the deriving class still may decide to allow the caller to change
      the tag value after construction or forbid it.

      @param tag [in] the new tag value
      @return none
     */
    void SetTag(SAPDB_Char const *tag)
    {
        if ( !tag || 0 == tag[0] )
        {
            m_Tag = MSG_IARG_UNKNOWN_TAG;
        }
        else
        {
            m_Tag = tag;
#if defined(SAPDB_SLOW)
            ValidateTag(tag);
#endif
        }
    }

    /*!
      @brief set value from constant character string

      @param value [in] the new value
      @return none
     */
    void SetValue(SAPDB_Char const *value)
    {
        if ( !value )
        {
            m_Value = SAPDB_ToStringClass(Msg_IOptArg::UndefinedValue);
        }
        else
        {
            m_Value = SAPDB_ToStringClass(value);
        }
    }

    /*!
      @brief set value from character string

      Character string is copied

      @param value [in] the new value
      @return none
     */
    void SetValue(SAPDB_Char *value)
    {
        if ( !value )
        {
            m_Value = SAPDB_ToStringClass(Msg_IOptArg::UndefinedValue);
        }
        else
        {
            m_Value = SAPDB_ToStringClass(value);
        }
    }

    /*!
      @brief set value from constant UTF8 character string

      @param value [in] the new value
      @return none
     */
    void SetValue(SAPDB_UTF8 const *value)
    {
        if ( !value )
        {
            m_Value = SAPDB_ToStringClass(Msg_IOptArg::UndefinedValue);
        }
        else
        {
            m_Value = SAPDB_ToStringClass(value);
        }
    }

    /*!
      @brief set value from UTF8 character string

      UTF8 character string is copied

      @param value [in] the new value
      @return none
     */
    void SetValue(SAPDB_UTF8 *value)
    {
        if ( !value )
        {
            m_Value = SAPDB_ToStringClass(Msg_IOptArg::UndefinedValue);
        }
        else
        {
            m_Value = SAPDB_ToStringClass(value);
        }
    }

    /*!
      @brief set value from SAPDB_ToStringClass instance

      @param value [in] the new value
      @return none
     */
    void SetValue(SAPDB_ToStringClass const &value)
    {
        m_Value = value;
    }

private:
    SAPDB_Char const *m_Tag; //!< the current tag value
    SAPDB_ToStringClass m_Value; //!< the current value
#if defined(SAPDB_SLOW)
    /*!
      @brief validate a tag string
      
      Since this is a protected member function the deriving class still may decide to allow the caller to change
      the tag value after construction or forbid it.

      @param tag [in] the new tag value
      @return none
     */
    void ValidateTag(SAPDB_Char const *tag)
    {
        // NIL tag check
        SAPDBERR_ASSERT_ARGUMENT(tag != 0);

        // empty tag check
        SAPDBERR_ASSERT_ARGUMENT(tag[0] != 0);

        // valid first character must be letter or underscore
        SAPDBERR_ASSERT_ARGUMENT( (tag[0] >= 'a' && tag[0] <= 'z')
                               || (tag[0] >= 'A' && tag[0] <= 'Z')
                               || (tag[0] == '_') );

        // maximum tag size check and validate of tag characters
        for ( SAPDB_Int4 iTag = 1; tag[iTag] != 0; ++iTag )
        {
            SAPDBERR_ASSERT_ARGUMENT( ((SAPDB_UInt4)iTag) < Msg_IOptArg::MaxTagLength );
            SAPDBERR_ASSERT_ARGUMENT( (tag[iTag] >= 'a' && tag[iTag] <= 'z')
                                   || (tag[iTag] >= 'A' && tag[iTag] <= 'Z')
                                   ||  tag[iTag] == '_' 
                                   || (tag[iTag] >= '0' && tag[iTag] <= '9') );
        }
    }
#endif
    friend class Msg_Arg;
};

/*!
  @class Msg_Arg
  @brief Message argument interface class for requiered tags

  A message argument consists of a tag and a value string. Since value strings may be unicode
  all value strings are returned as zero terminated UTF8 strings. An empty string ((SAPDB_UTF8 const *)"") is a valid
  value string, but an empty tag string is not allowed! This class validates this using assertions.

  This class also copies the tag string and uses identical form of constructors for value strings like SAPDB_ToString class.
  
 */
class Msg_Arg : public Msg_IArg
{
public:
    /*!
      @brief default ctor

      While value must not necessarily be known during contruction, the tag is most times a constant string.
      Therefore the default constructor fill 'Msg_IOptArg::UnknownTag' and sets 'Msg_IOptArg::UndefinedValue'.
     */
    Msg_Arg() 
        : m_MsgArg() {}

    /*!
      @brief ctor

      While value must not necessarily be known during contruction, the tag is most times a constant string.
      Therefor it makes sends to be able to derive from this class, which is no pure interface class, but restricts
      the tag usage to non empty character strings.
      @param tag [in] the tag string
     */
    Msg_Arg( SAPDB_Char const *tag)
        : m_MsgArg( tag) {}

    /*!
      @brief ctor for SAPDB_ToStringClass value

      No conversion needed

      @param tag [in] the tag string
      @param value [in] the value 
     */
    Msg_Arg( SAPDB_Char const *tag
           , SAPDB_ToStringClass const &value)
        : m_MsgArg( tag, value ) {}

    /*!
    @brief ctor for simple string

    No conversion needed

    @param tag [in] the tag string
    @param String [in] the string
    */
    explicit Msg_Arg( SAPDB_Char const *tag,
                     const SAPDB_Char *String)
        : m_MsgArg( tag, String ) {}

    /*!
    @brief ctor for simple UTF8 string

    No conversion needed

    @param tag [in] the tag string
    @param String [in] the string
    */
    explicit Msg_Arg( SAPDB_Char const *tag,
                     const SAPDB_UTF8 *String)
        : m_MsgArg( tag, String ) {}

    /*!-------------------------------------------------------------------------
    @brief ctor for simple string with format flags

    No conversion needed

    @param tag [in] the tag string
    @param String [in] the string
     param SAPDB_ToStringClass::fmtflags [in] ignored
    */
    Msg_Arg( SAPDB_Char const *tag,
            const SAPDB_Char *String,
            const SAPDB_ToStringClass::fmtflags )
        : m_MsgArg( tag, String ) {}

    /*!-------------------------------------------------------------------------
    @brief ctor for simple UTF8 string with format flags

    No conversion needed

    @param tag [in] the tag string
    @param String [in] the string
     param SAPDB_ToStringClass::fmtflags [in] ignored
    */
    Msg_Arg( SAPDB_Char const *tag,
            const SAPDB_UTF8 *String,
            const SAPDB_ToStringClass::fmtflags )
        : m_MsgArg( tag, String ) {}

    /*!-------------------------------------------------------------------------
    @brief ctor for simple string with format flags and width specification

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)

    @param tag [in] the tag string
    @param String [in] the string
    @param Width [in] output string minimum width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    */
    Msg_Arg( SAPDB_Char const *tag,
            const SAPDB_Char *String,
            const SAPDB_UInt2 Width,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, String, Width, Flag1, Flag2 ) {}

    /*!-------------------------------------------------------------------------
    @brief ctor for simple UTF8 string with format flags and width specification

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)

    @param tag [in] the tag string
    @param String [in] the string
    @param Width [in] output string minimum width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    */
    Msg_Arg( SAPDB_Char const *tag,
            const SAPDB_UTF8 *String,
            const SAPDB_UInt2 Width,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, String, Width, Flag1, Flag2 ) {}

    /*!-------------------------------------------------------------------------
    @brief ctor for simple string with format flags and width and precision specification

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)

    @param tag [in] the tag string
    @param String [in] the string
    @param Width [in] output string minimum width
    @param Precision [in] output string minimum width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    */
    Msg_Arg( SAPDB_Char const *tag,
            const SAPDB_Char *String,
            const SAPDB_UInt2 Width,
            const SAPDB_UInt2 Precision,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, String, Width, Precision, Flag1, Flag2 ) {}

    /*!-------------------------------------------------------------------------
    @brief ctor for simple UTF8 string with format flags and width and precision specification

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)

    @param tag [in] the tag string
    @param String [in] the string
    @param Width [in] output string minimum width
    @param Precision [in] output string minimum width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    */
    Msg_Arg( SAPDB_Char const *tag,
            const SAPDB_UTF8 *String,
            const SAPDB_UInt2 Width,
            const SAPDB_UInt2 Precision,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, String, Width, Precision, Flag1, Flag2 ) {}

    /*!-------------------------------------------------------------------------
    @brief ctor for one byte signed number with format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::showbase (default - not set)
                        SAPDB_ToStringClass::dec (default)
                        SAPDB_ToStringClass::oct
                        SAPDB_ToStringClass::hex
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_Arg( SAPDB_Char const *tag,
                     const SAPDB_Int1 Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Flag1, Flag2, Flag3, Flag4 ) {}

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


    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_Arg( SAPDB_Char const *tag,
            const SAPDB_Int1 Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Width, Flag1, Flag2, Flag3, Flag4 ) {}

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

    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_Arg( SAPDB_Char const *tag,
                     const SAPDB_UInt1 Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Flag1, Flag2, Flag3, Flag4 ) {}

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


    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_Arg( SAPDB_Char const *tag,
            const SAPDB_UInt1 Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Width, Flag1, Flag2, Flag3, Flag4 ) {}

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
 
    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_Arg( SAPDB_Char const *tag,
                     const SAPDB_Int2 Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Flag1, Flag2, Flag3, Flag4 ) {}

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

    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_Arg( SAPDB_Char const *tag,
            const SAPDB_Int2 Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Width, Flag1, Flag2, Flag3, Flag4 ) {}

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
 
    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_Arg( SAPDB_Char const *tag,
                     const SAPDB_UInt2 Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Flag1, Flag2, Flag3, Flag4 ) {}

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


    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_Arg( SAPDB_Char const *tag,
            const SAPDB_UInt2 Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Width, Flag1, Flag2, Flag3, Flag4 ) {}

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
 
    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_Arg( SAPDB_Char const *tag,
                     const SAPDB_Int4 Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Flag1, Flag2, Flag3, Flag4 ) {}

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


    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_Arg( SAPDB_Char const *tag,
            const SAPDB_Int4 Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Width, Flag1, Flag2, Flag3, Flag4 ) {}

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
 
    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_Arg( SAPDB_Char const *tag,
                     const SAPDB_UInt4 Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Flag1, Flag2, Flag3, Flag4 ) {}

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


    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_Arg( SAPDB_Char const *tag,
            const SAPDB_UInt4 Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Width, Flag1, Flag2, Flag3, Flag4 ) {}

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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_Arg( SAPDB_Char const *tag,
                     const SAPDB_Int8 Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Flag1, Flag2, Flag3, Flag4 ) {}

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


    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_Arg( SAPDB_Char const *tag,
            const SAPDB_Int8 Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Width, Flag1, Flag2, Flag3, Flag4 ) {}

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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_Arg( SAPDB_Char const *tag,
                     const SAPDB_UInt8 Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Flag1, Flag2, Flag3, Flag4 ) {}

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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_Arg( SAPDB_Char const *tag,
            const SAPDB_UInt8 Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Width, Flag1, Flag2, Flag3, Flag4 ) {}
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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_Arg( SAPDB_Char const *tag,
                     const long Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Flag1, Flag2, Flag3, Flag4 ) {}

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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_Arg( SAPDB_Char const *tag,
            const long Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Width, Flag1, Flag2, Flag3, Flag4 ) {}

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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_Arg( SAPDB_Char const *tag, 
                     const unsigned long Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Flag1, Flag2, Flag3, Flag4 ) {}

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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_Arg( SAPDB_Char const *tag,
            const unsigned long Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Width, Flag1, Flag2, Flag3, Flag4 ) {}
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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_Arg( SAPDB_Char const *tag,
                     const SAPDB_Long Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Flag1, Flag2, Flag3, Flag4 ) {}

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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_Arg( SAPDB_Char const *tag,
            const SAPDB_Long Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Width, Flag1, Flag2, Flag3, Flag4 ) {}

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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_Arg( SAPDB_Char const *tag,
                     const SAPDB_ULong Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Flag1, Flag2, Flag3, Flag4 ) {}

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
 

    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_Arg( SAPDB_Char const *tag, 
            const SAPDB_ULong Number,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Width, Flag1, Flag2, Flag3, Flag4 ) {}

    /*!-------------------------------------------------------------------------
    @brief ctor for floating point number with format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::scientific
                        SAPDB_ToStringClass::fixed
                        SAPDB_ToStringClass::showpoint
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param tag [in] the tag string
    @param Real [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_Arg( SAPDB_Char const *tag,
                     const SAPDB_Real8  Real,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat) 
        : m_MsgArg( tag, Real, Flag1, Flag2, Flag3, Flag4 ) {}

    /*!-------------------------------------------------------------------------
    @brief ctor for floating point number with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::scientific
                        SAPDB_ToStringClass::fixed
                        SAPDB_ToStringClass::showpoint
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param tag [in] the tag string
    @param Real [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_Arg( SAPDB_Char const *tag,
            const SAPDB_Real8  Real,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Real, Width, Flag1, Flag2, Flag3, Flag4 ) {}

    /*!-------------------------------------------------------------------------
    @brief ctor for floating point number with width and precision specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::scientific
                        SAPDB_ToStringClass::fixed
                        SAPDB_ToStringClass::showpoint
                        SAPDB_ToStringClass::uppercase (default - not set)
 

    @param tag [in] the tag string
    @param Real [in] the string
    @param Width [in] the minimum output width
    @param Precision [in] the given number of precision digits to show
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_Arg( SAPDB_Char const *tag,
            const SAPDB_Real8  Real,
            const SAPDB_UInt2 Width ,
            const SAPDB_UInt2 Precision,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Real, Width, Precision, Flag1, Flag2, Flag3, Flag4 ) {}

    /*!-------------------------------------------------------------------------
    @brief ctor for Pointer (address)  with width and precision specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::ptr (default)
                        SAPDB_ToStringClass::showbase (default - set)
                        SAPDB_ToStringClass::uppercase (default)

    @param tag [in] the tag string
    @param Addr [in] the string
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_Arg( SAPDB_Char const *tag,
                     const void *Addr,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::showbase,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Addr, Flag1, Flag2, Flag3, Flag4 ) {}

    /*!-------------------------------------------------------------------------
    @brief ctor for Pointer (address)  with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::ptr (default)
                        SAPDB_ToStringClass::showbase (default - set)
                        SAPDB_ToStringClass::uppercase (default)
                        SAPDB_ToStringClass::showpoint

    @param tag [in] the tag string
    @param Addr [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_Arg( SAPDB_Char const *tag,
            const void *Addr,
            const SAPDB_UInt2 Width ,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::showbase,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Addr, Width, Flag1, Flag2, Flag3, Flag4 ) {}

#ifndef CPP_BOOL_NOT_IMPLEMENTED
    /*!-------------------------------------------------------------------------
    @brief ctor for bool and format flags

    supported fmtflags: SAPDB_ToStringClass::boolalpha (default "true", "false" )
                        SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::uppercase (default - not set)

    @param tag [in] the tag string
    @param Number [in] the string
    @param Flag1 [in] first format flag (default boolalpha)
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    explicit Msg_Arg( SAPDB_Char const *tag,
                     const SAPDB_Bool Number,
                     const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::boolalpha,
                     const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
                     const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Flag1, Flag2, Flag3, Flag4 ) {}

   /*!-------------------------------------------------------------------------
    @brief ctor for bool with width specification and format flags

    supported fmtflags: SAPDB_ToStringClass::boolalpha (default "true", "false" )
                        SAPDB_ToStringClass::left
                        SAPDB_ToStringClass::right (default)
                        SAPDB_ToStringClass::uppercase (default - not set)

    @param tag [in] the tag string
    @param Number [in] the string
    @param Width [in] the minimum output width
    @param Flag1 [in] first format flag (default boolalpha)
    @param Flag2 [in] second format flag
    @param Flag3 [in] third format flag
    @param Flag4 [in] forth format flag
    */
    Msg_Arg( SAPDB_Char const *tag,
            const SAPDB_Bool Number,
            const SAPDB_UInt2 Width,
            const SAPDB_ToStringClass::fmtflags Flag1=SAPDB_ToStringClass::boolalpha,
            const SAPDB_ToStringClass::fmtflags Flag2=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag3=SAPDB_ToStringClass::noformat,
            const SAPDB_ToStringClass::fmtflags Flag4=SAPDB_ToStringClass::noformat)
        : m_MsgArg( tag, Number, Width, Flag1, Flag2, Flag3, Flag4 ) {}

#endif /* CPP_BOOL_NOT_IMPLEMENTED */

    /*!-------------------------------------------------------------------------
    @brief copy ctor

    Copies one argument to another
    @param toCopy [in] another Msg_Arg instance to clone from
     */
    Msg_Arg(Msg_Arg const &toCopy)
        : m_MsgArg(toCopy.m_MsgArg)
    {
    }

    /*!-------------------------------------------------------------------------
    @brief copy ctor from different Msg_IArg implementation

    Copies one argument to another
    @param toCopy [in] another Msg_IArg derived instance to clone from
     */
    Msg_Arg(Msg_IArg const * toCopy)
        : m_MsgArg(toCopy->GetArg())
    {
    }

    /*!-------------------------------------------------------------------------
    @brief copy ctor from different Msg_IArg implementation

    Copies one argument to another
    @param toCopy [in] another Msg_IArg derived instance to clone from
     */
    Msg_Arg(Msg_IOptArg const * toCopy)
        : m_MsgArg(toCopy)
    {
    }

    // wow a nice number of constructors..... but finally this was the last one...

    /*!
      @brief dtor

      needed since we have some virtual member functions...
     */
    virtual ~Msg_Arg()
    {}

    /*!-------------------------------------------------------------------------
    @brief assignment operator
    @param toAssignFrom [in] argument to assign from 
    @return pointer to output string
    */
    Msg_Arg & operator=(const Msg_IArg &toAssignFrom)
    {
        if ( this != &toAssignFrom )
        {
            m_MsgArg.SetTag(toAssignFrom.Tag());
            m_MsgArg.SetValue(toAssignFrom.Value());
        }
        return *this;
    }

    /*!
      @brief return fixed tag string as zero terminated C String

      This routine always returns a valid constant C String
      NULL must never be returned!

      @return zero terminated C String
     */
    virtual SAPDB_Char const *Tag() const { return m_MsgArg.Tag(); }

    /*!
      @brief return fixed value string as zero terminated C String

      This routine always returns a valid value C String.
      NULL must never be returned!

      @return zero terminated C String
     */
    virtual SAPDB_UTF8 const *Value() const { return m_MsgArg.Value(); }

    /*!
      @brief cast operator needed for automatic cast to Msg_IArg pointer
      @return should return the 'this' pointer
     */
    virtual operator Msg_IArg const * () const { return this; }

    /*!
      @brief cast operator needed to retrieve Msg_IOptArg pointer
      @return returns pointer to the internal Msg_OptArg member
     */
    virtual Msg_IOptArg const * GetArg() const { return &m_MsgArg; }

    /*!
      @brief set tag after construction

      Since this is a protected member function the deriving class still may decide to allow the caller to change
      the tag value after construction or forbid it.

      @param tag [in] the new tag value
      @return none
     */
    void SetTag(SAPDB_Char const *tag) { m_MsgArg.SetTag(tag); }

    /*!
      @brief set value from constant character string

      @param value [in] the new value
      @return none
     */
    void SetValue(SAPDB_Char const *value) { m_MsgArg.SetValue(value); }

    /*!
      @brief set value from character string

      Character string is copied

      @param value [in] the new value
      @return none
     */
    void SetValue(SAPDB_Char *value) { m_MsgArg.SetValue(value); }

    /*!
      @brief set value from constant UTF8 character string

      @param value [in] the new value
      @return none
     */
    void SetValue(SAPDB_UTF8 const *value) { m_MsgArg.SetValue(value); }

    /*!
      @brief set value from UTF8 character string

      UTF8 character string is copied

      @param value [in] the new value
      @return none
     */
    void SetValue(SAPDB_UTF8 *value) { m_MsgArg.SetValue(value); }

    /*!
      @brief set value from SAPDB_ToStringClass instance

      @param value [in] the new value
      @return none
     */
    void SetValue(SAPDB_ToStringClass const &value) { m_MsgArg.SetValue(value); }

private:
    Msg_OptArg m_MsgArg; //!< the argument
};

#endif // MSG_ARG_HPP
