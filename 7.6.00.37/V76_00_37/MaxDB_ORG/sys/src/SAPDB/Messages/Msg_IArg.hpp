/*!
  @file    Msg_IArg.hpp
  @author  JoergM
  @ingroup Common
  @brief   Message argument interface class
  
  All message arguemts must be derived from this class.

  This class is used as base class for arguments given to constructor of Msg_List class instances.

  To derive from this class two member functions are needed. 

  The message arguments consists of a tag string which is a 255 character limited string, that must
  start with a Ascii7 letter follwed by up to 254 Ascii7 digits, Ascii7 letters or underscore characters.
  If a tag is specified is capital letter tag, it is silently converted into lowercase.

  A value may be any valid UTF8 string. An empty string is an allowed value.

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
 */

#ifndef MSG_IARG_HPP
#define MSG_IARG_HPP

#include "SAPDBCommon/SAPDB_Types.hpp"

class Msg_IOptArg;
/*!
  @class Msg_IArg
  @brief Message argument 'pure' interface class

  A message argument consists of a tag and a value string. Since value strings may be unicode
  all value strings are returned as zero terminated UTF8 strings. An empty string ((SAPDB_UTF8 const *)"") is a valid
  value string, but an empty tag string is not allowed! Use 'Msg_IArg::UnknownTag' instead!

  The implementator of an Msg_IArg derived class must be aware of this restriction and should validate it using assertions...

  For a derived general purpose message argument have a look at class Msg_ShortTextArg.
 */
class Msg_IArg
{
public:
    /*!
      @brief return fixed tag string as zero terminated C String

      This routine always returns a valid constant C String
      NULL must never be returned!

      @return zero terminated C String
     */
    virtual SAPDB_Char const *Tag() const = 0;

    /*!
      @brief return fixed value string as zero terminated C String

      This routine always returns a valid value C String.
      NULL must never be returned!

      @return zero terminated C String
     */
    virtual SAPDB_UTF8 const *Value() const = 0;

    /*!
      @brief cast operator needed for automatic cast to Msg_IArg pointer
      @return should return the 'this' pointer
     */
    virtual operator Msg_IArg const * () const = 0;

    /*!
      @brief cast operator needed to retrieve Msg_IOptArg pointer
      @return returns pointer to the internal Msg_Arg member
     */
    virtual Msg_IOptArg const *GetArg() const = 0;

    /*! undefined value string constant (should be returned if value is not set */
    static SAPDB_UTF8 const * const UndefinedValue;

    /*! undefined tag string constant (should be returned if tag is not known */
    static SAPDB_Char const * const UnknownTag;

    /*! tag lengts must not be limited. XML does not specify a tag limit, but a meaningfull artificial
        limitation will not hurt and ease to check if a given tag is really limited... */
    static SAPDB_UInt4 const MaxTagLength;
}; /*! @endclass Msg_IArg */


#endif /* MSG_IARG_HPP */
